// Process referee: two CG-protocol bots, Fidelity csb::Game.
// Catalog maps or Agade generate (rotate + ±30 jitter, issue #2).
#include "maps/catalog.h"
#include "progress.h"
#include "physics.h"
#include "agade_maps.h"

#include <algorithm>
#include <chrono>
#include <cerrno>
#include <cmath>
#include <csignal>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <poll.h>
#include <spawn.h>
#include <string>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

extern char** environ;

thread_local bool g_friendly_collision = false;

struct GameRec {
    int map = 0;
    int side = 0;
    int repeat = 0;
    int winner = -1;
    int mapped_winner = -1;
    int turns = 0;
    std::string reason;
    bool side_swapped = false;
};

struct Agg {
    int games = 0, wins_a = 0, wins_b = 0, draws = 0;
    std::vector<GameRec> recs;
};

static void die(const char* msg) {
    std::fprintf(stderr, "process_duel: %s\n", msg);
    std::exit(2);
}

static std::string json_escape(const std::string& s) {
    std::string o;
    o.reserve(s.size() + 8);
    for (char c : s) {
        if (c == '"' || c == '\\') {
            o.push_back('\\');
            o.push_back(c);
        } else if (c == '\n') {
            o += "\\n";
        } else {
            o.push_back(c);
        }
    }
    return o;
}

struct BotProc {
    pid_t pid = -1;
    int in_fd = -1;   // parent writes child's stdin
    int out_fd = -1;  // parent reads child's stdout
    std::string buf;
    std::string path;

    void close_fds() {
        if (in_fd >= 0) {
            close(in_fd);
            in_fd = -1;
        }
        if (out_fd >= 0) {
            close(out_fd);
            out_fd = -1;
        }
    }

    void kill_wait() {
        if (pid > 0) {
            kill(pid, SIGTERM);
            int st = 0;
            waitpid(pid, &st, 0);
            pid = -1;
        }
        close_fds();
    }

    bool spawn(const char* bin) {
        path = bin;
        if (!std::strchr(bin, '/')) die("bot path must contain /");
        int pin[2], pout[2];
        if (pipe(pin) != 0 || pipe(pout) != 0) return false;
        int devnull = open("/dev/null", O_WRONLY);
        if (devnull < 0) return false;
        posix_spawn_file_actions_t fa;
        if (posix_spawn_file_actions_init(&fa) != 0) {
            close(devnull);
            return false;
        }
        posix_spawn_file_actions_adddup2(&fa, pin[0], STDIN_FILENO);
        posix_spawn_file_actions_adddup2(&fa, pout[1], STDOUT_FILENO);
        posix_spawn_file_actions_adddup2(&fa, devnull, STDERR_FILENO);
        posix_spawn_file_actions_addclose(&fa, pin[0]);
        posix_spawn_file_actions_addclose(&fa, pin[1]);
        posix_spawn_file_actions_addclose(&fa, pout[0]);
        posix_spawn_file_actions_addclose(&fa, pout[1]);
        posix_spawn_file_actions_addclose(&fa, devnull);
        char* argv[] = {const_cast<char*>(bin), nullptr};
        int rc = posix_spawn(&pid, bin, &fa, nullptr, argv, environ);
        posix_spawn_file_actions_destroy(&fa);
        close(pin[0]);
        close(pout[1]);
        close(devnull);
        if (rc != 0) {
            close(pin[1]);
            close(pout[0]);
            pid = -1;
            return false;
        }
        in_fd = pin[1];
        out_fd = pout[0];
        int fl = fcntl(out_fd, F_GETFL, 0);
        if (fl >= 0) fcntl(out_fd, F_SETFL, fl | O_NONBLOCK);
        return true;
    }

    bool write_all(const std::string& s) {
        size_t off = 0;
        while (off < s.size()) {
            ssize_t n = write(in_fd, s.data() + off, s.size() - off);
            if (n < 0) {
                if (errno == EINTR) continue;
                return false;
            }
            off += static_cast<size_t>(n);
        }
        return true;
    }

    // Read one line. timeout_ms for the whole wait. Returns false on timeout/eof.
    bool read_line(int timeout_ms, std::string* line) {
        auto t0 = std::chrono::steady_clock::now();
        while (true) {
            auto nl = buf.find('\n');
            if (nl != std::string::npos) {
                *line = buf.substr(0, nl);
                if (!line->empty() && line->back() == '\r') line->pop_back();
                buf.erase(0, nl + 1);
                return true;
            }
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                               std::chrono::steady_clock::now() - t0)
                               .count();
            int left = timeout_ms - static_cast<int>(elapsed);
            if (left < 0) left = 0;
            pollfd pfd{out_fd, POLLIN, 0};
            int pr = poll(&pfd, 1, left);
            if (pr == 0) return false;
            if (pr < 0) {
                if (errno == EINTR) continue;
                return false;
            }
            char tmp[4096];
            ssize_t n = read(out_fd, tmp, sizeof(tmp));
            if (n < 0) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) continue;
                if (errno == EINTR) continue;
                return false;
            }
            if (n == 0) return false;
            buf.append(tmp, static_cast<size_t>(n));
        }
    }
};

static bool parse_move(const std::string& line, int* tx, int* ty, std::string* tok) {
    int x = 0, y = 0;
    char tbuf[64] = {0};
    if (std::sscanf(line.c_str(), "%d %d %63s", &x, &y, tbuf) < 3) return false;
    *tx = x;
    *ty = y;
    *tok = tbuf;
    return true;
}

static int view_angle_deg(const csb::Pod& c) {
    if (!c.hasRotated && c.angle > -0.02 && c.angle < 0.0) return -1;
    if (!c.hasRotated && std::fabs(c.angle + csb::kDegToRad) < 1e-9) return -1;
    double deg = c.angle * csb::kRadToDeg;
    while (deg >= 360.0) deg -= 360.0;
    while (deg < 0.0) deg += 360.0;
    return static_cast<int>(std::floor(deg + 0.5));
}

static std::string view_line(const csb::Game& g, int pod_i, int ncp) {
    const csb::Pod& p = g.pods[static_cast<size_t>(pod_i)];
    int nloc = csb_progress::LocalNext(p.next, ncp);
    char buf[128];
    std::snprintf(buf, sizeof(buf), "%d %d %d %d %d %d\n",
                  static_cast<int>(p.p.x), static_cast<int>(p.p.y),
                  static_cast<int>(p.s.x), static_cast<int>(p.s.y),
                  view_angle_deg(p), nloc);
    return buf;
}

static std::string header_for_track(const std::vector<std::pair<int, int>>& raw) {
    std::string s = "3\n";
    s += std::to_string(static_cast<int>(raw.size()));
    s += "\n";
    for (const auto& pt : raw) {
        s += std::to_string(pt.first);
        s += " ";
        s += std::to_string(pt.second);
        s += "\n";
    }
    return s;
}

static std::string header_for(int map) {
    const auto& raw = GetTournamentMapsRaw()[static_cast<size_t>(map)];
    std::vector<std::pair<int, int>> t;
    t.reserve(raw.size());
    for (const auto& pt : raw) t.push_back({(int)pt.x, (int)pt.y});
    return header_for_track(t);
}

struct Cli {
    const char* bot_a = nullptr;
    const char* bot_b = nullptr;
    std::vector<int> maps;
    std::vector<int> sides;
    int repeats = 15;
    int first_ms = 1000;
    int later_ms = 75;
    int max_turns = 500;
    const char* out = nullptr;
    int gen_maps = 0;
    uint32_t gen_seed = 1;
    std::vector<int> gen_instances;
};

static std::vector<int> parse_list(const char* s, int lo, int hi) {
    std::vector<int> v;
    std::string t(s);
    size_t i = 0;
    while (i < t.size()) {
        size_t j = t.find(',', i);
        if (j == std::string::npos) j = t.size();
        std::string tok = t.substr(i, j - i);
        auto dash = tok.find('-');
        if (dash != std::string::npos) {
            int a = std::atoi(tok.c_str());
            int b = std::atoi(tok.c_str() + dash + 1);
            for (int x = a; x <= b; x++) {
                if (x >= lo && x <= hi) v.push_back(x);
            }
        } else {
            int x = std::atoi(tok.c_str());
            if (x >= lo && x <= hi) v.push_back(x);
        }
        i = j + 1;
    }
    return v;
}

static Cli parse_cli(int argc, char** argv) {
    Cli c;
    c.maps.clear();
    c.sides = {0, 1};
    for (int i = 1; i < argc; i++) {
        std::string a = argv[i];
        auto need = [&]() -> const char* {
            if (i + 1 >= argc) die("missing arg");
            return argv[++i];
        };
        if (a == "--bot-a")
            c.bot_a = need();
        else if (a == "--bot-b")
            c.bot_b = need();
        else if (a == "--maps")
            c.maps = parse_list(need(), 0, 17);
        else if (a == "--sides")
            c.sides = parse_list(need(), 0, 1);
        else if (a == "--repeats")
            c.repeats = std::atoi(need());
        else if (a == "--first-turn-ms")
            c.first_ms = std::atoi(need());
        else if (a == "--time-budget-ms")
            c.later_ms = std::atoi(need());
        else if (a == "--max-turns")
            c.max_turns = std::atoi(need());
        else if (a == "--out")
            c.out = need();
        else if (a == "--gen-maps")
            c.gen_maps = std::atoi(need());
        else if (a == "--gen-seed")
            c.gen_seed = static_cast<uint32_t>(std::strtoul(need(), nullptr, 10));
        else if (a == "--gen-instances")
            c.gen_instances = parse_list(need(), 0, 10000);
        else
            die("unknown flag");
    }
    if (!c.bot_a || !c.bot_b) die("need --bot-a and --bot-b");
    if (!c.gen_instances.empty()) {
        c.maps = c.gen_instances;
        c.gen_maps = 0;
    } else if (c.gen_maps > 0) {
        c.maps.clear();
        for (int i = 0; i < c.gen_maps; i++) c.maps.push_back(i);
    } else if (c.maps.empty()) {
        for (int i = 0; i < 18; i++) c.maps.push_back(i);
    }
    if (c.repeats < 1) c.repeats = 1;
    return c;
}

static bool already_done(const Agg& a, int map, int side, int rep) {
    for (const auto& g : a.recs) {
        if (g.map == map && g.side == side && g.repeat == rep) return true;
    }
    return false;
}

static void write_json(const Cli& cli, const Agg& a, const char* path) {
    std::string tmp = std::string(path) + ".tmp";
    FILE* fp = std::fopen(tmp.c_str(), "w");
    if (!fp) die("cannot write --out tmp");
    double wr = a.games ? (double)a.wins_a / (double)a.games : 0.0;
    const auto& m0 = GetTournamentMapsRaw()[0];
    std::fprintf(fp,
                 "{\"schema_version\":\"searchbot.duel.v1\",\"bot_a\":\"%s\",\"bot_b\":\"%s\","
                 "\"games\":%d,\"wins_a\":%d,\"wins_b\":%d,\"draws\":%d,\"winrate_a\":%.10g,"
                 "\"first_turn_ms\":%d,\"later_turn_ms\":%d,\"repeats_are_wallclock\":true,"
                 "\"map0_cps\":[",
                 json_escape(cli.bot_a).c_str(), json_escape(cli.bot_b).c_str(), a.games,
                 a.wins_a, a.wins_b, a.draws, wr, cli.first_ms, cli.later_ms);
    for (size_t i = 0; i < m0.size(); i++) {
        if (i) std::fputc(',', fp);
        std::fprintf(fp, "[%d,%d]", (int)m0[i].x, (int)m0[i].y);
    }
    std::fprintf(fp, "],\"games_detail\":[");
    for (size_t i = 0; i < a.recs.size(); i++) {
        const auto& g = a.recs[i];
        if (i) std::fputc(',', fp);
        std::fprintf(fp,
                     "{\"map\":%d,\"side\":%d,\"repeat\":%d,\"winner\":%d,\"mapped_winner\":%d,"
                     "\"turns\":%d,\"reason\":\"%s\",\"side_swapped\":%s}",
                     g.map, g.side, g.repeat, g.winner, g.mapped_winner, g.turns,
                     json_escape(g.reason).c_str(), g.side_swapped ? "true" : "false");
    }
    std::fprintf(fp, "]}\n");
    std::fclose(fp);
    if (std::rename(tmp.c_str(), path) != 0) die("rename --out failed");
}

static bool load_resume(const char* path, Agg* a) {
    FILE* fp = std::fopen(path, "r");
    if (!fp) return false;
    std::fseek(fp, 0, SEEK_END);
    long n = std::ftell(fp);
    std::fseek(fp, 0, SEEK_SET);
    if (n <= 0) {
        std::fclose(fp);
        return false;
    }
    std::string s(static_cast<size_t>(n), '\0');
    if (std::fread(s.data(), 1, static_cast<size_t>(n), fp) != static_cast<size_t>(n)) {
        std::fclose(fp);
        return false;
    }
    std::fclose(fp);
    // Minimal resume: scan "map":N,"side":N,"repeat":N triples and counters.
    const char* p = s.c_str();
    while ((p = std::strstr(p, "\"map\":"))) {
        GameRec g;
        if (std::sscanf(p, "\"map\":%d,\"side\":%d,\"repeat\":%d,\"winner\":%d,\"mapped_winner\":%d,"
                           "\"turns\":%d,\"reason\":\"%63[^\"]\"",
                        &g.map, &g.side, &g.repeat, &g.winner, &g.mapped_winner, &g.turns,
                        reinterpret_cast<char*>(g.reason.data())) >= 6) {
            // reason via a small buf
        }
        char reason[64] = {0};
        int got = std::sscanf(p,
                              "\"map\":%d,\"side\":%d,\"repeat\":%d,\"winner\":%d,\"mapped_winner\":%d,"
                              "\"turns\":%d,\"reason\":\"%63[^\"]\"",
                              &g.map, &g.side, &g.repeat, &g.winner, &g.mapped_winner, &g.turns,
                              reason);
        if (got >= 7) {
            g.reason = reason;
            g.side_swapped = (g.side == 1);
            a->recs.push_back(g);
            a->games++;
            if (g.mapped_winner == 0)
                a->wins_a++;
            else if (g.mapped_winner == 1)
                a->wins_b++;
            else
                a->draws++;
        }
        p += 6;
    }
    return a->games > 0;
}

static GameRec play_one(const Cli& cli, int map, int side,
                        const std::vector<std::pair<int, int>>& cps) {
    GameRec rec;
    rec.map = map;
    rec.side = side;
    rec.side_swapped = (side == 1);

    const int ncp = static_cast<int>(cps.size());
    std::vector<csb::Point> track;
    track.reserve(cps.size());
    for (const auto& pt : cps) track.push_back({(double)pt.first, (double)pt.second});

    BotProc A, B;
    if (!A.spawn(cli.bot_a) || !B.spawn(cli.bot_b)) {
        rec.reason = "crash";
        rec.mapped_winner = -1;
        A.kill_wait();
        B.kill_wait();
        return rec;
    }
    std::string hdr = header_for_track(cps);
    if (!A.write_all(hdr) || !B.write_all(hdr)) {
        rec.reason = "crash";
        A.kill_wait();
        B.kill_wait();
        return rec;
    }

    csb::Game game;
    game.initialize(track, 3);

    // team0 pods in Game are 0,1; team1 are 2,3.
    // side 0: A is team0. side 1: A is team1.
    const int a_pods[2] = {side == 0 ? 0 : 2, side == 0 ? 1 : 3};
    const int b_pods[2] = {side == 0 ? 2 : 0, side == 0 ? 3 : 1};

    auto feed = [&](BotProc& bot, const int own[2], const int opp[2]) {
        std::string s;
        s += view_line(game, own[0], ncp);
        s += view_line(game, own[1], ncp);
        s += view_line(game, opp[0], ncp);
        s += view_line(game, opp[1], ncp);
        return bot.write_all(s);
    };

    auto read_two = [&](BotProc& bot, int timeout_ms, int* tx0, int* ty0, std::string* t0,
                        int* tx1, int* ty1, std::string* t1) -> bool {
        std::string l0, l1;
        if (!bot.read_line(timeout_ms, &l0)) return false;
        if (!bot.read_line(timeout_ms, &l1)) return false;
        return parse_move(l0, tx0, ty0, t0) && parse_move(l1, tx1, ty1, t1);
    };

    for (int turn = 1; turn <= cli.max_turns; turn++) {
        rec.turns = turn;
        int budget = (turn == 1) ? (cli.first_ms + 500) : (cli.later_ms + 150);

        if (!feed(A, a_pods, b_pods)) {
            rec.reason = "crash";
            rec.mapped_winner = 1;
            rec.winner = (side == 0) ? 1 : 0;
            break;
        }
        int ax0, ay0, ax1, ay1, bx0, by0, bx1, by1;
        std::string at0, at1, bt0, bt1;
        bool a_ok = read_two(A, budget, &ax0, &ay0, &at0, &ax1, &ay1, &at1);

        if (!feed(B, b_pods, a_pods)) {
            rec.reason = "crash";
            rec.mapped_winner = 0;
            rec.winner = (side == 0) ? 0 : 1;
            break;
        }
        bool b_ok = read_two(B, budget, &bx0, &by0, &bt0, &bx1, &by1, &bt1);

        if (!a_ok && !b_ok) {
            rec.reason = "dual_timeout";
            rec.winner = -1;
            rec.mapped_winner = -1;
            break;
        }
        if (!a_ok) {
            rec.reason = "timeout";
            rec.mapped_winner = 1;
            rec.winner = (side == 0) ? 1 : 0;
            break;
        }
        if (!b_ok) {
            rec.reason = "timeout";
            rec.mapped_winner = 0;
            rec.winner = (side == 0) ? 0 : 1;
            break;
        }

        game.applyAction(a_pods[0], ax0, ay0, at0);
        game.applyAction(a_pods[1], ax1, ay1, at1);
        game.applyAction(b_pods[0], bx0, by0, bt0);
        game.applyAction(b_pods[1], bx1, by1, bt1);
        game.nextTurn();

        int w = game.checkWinner();
        if (w == 0 || w == 1) {
            rec.winner = w;
            rec.reason = "finished";
            rec.mapped_winner = (side == 0) ? w : (1 - w);
            break;
        }
        if (w == -1) {
            rec.winner = -1;
            rec.mapped_winner = -1;
            rec.reason = "draw";
            break;
        }
        if (turn == cli.max_turns) {
            rec.winner = -1;
            rec.mapped_winner = -1;
            rec.reason = "max_turns";
        }
    }

    A.kill_wait();
    B.kill_wait();
    return rec;
}

int main(int argc, char** argv) {
    signal(SIGPIPE, SIG_IGN);
    Cli cli = parse_cli(argc, argv);
    Agg agg;
    if (cli.out) load_resume(cli.out, &agg);

    std::vector<std::vector<std::pair<int, int>>> tracks;
    tracks.reserve(cli.maps.size());
    if (!cli.gen_instances.empty()) {
        for (int inst : cli.gen_instances)
            tracks.push_back(GenerateAgadeMap(cli.gen_seed, inst));
    } else if (cli.gen_maps > 0) {
        for (int i = 0; i < cli.gen_maps; i++)
            tracks.push_back(GenerateAgadeMap(cli.gen_seed, i));
    } else {
        const auto& cat = GetTournamentMapsRaw();
        for (int map : cli.maps) {
            std::vector<std::pair<int, int>> t;
            for (const auto& pt : cat[static_cast<size_t>(map)])
                t.push_back({(int)pt.x, (int)pt.y});
            tracks.push_back(std::move(t));
        }
    }

    for (size_t mi = 0; mi < cli.maps.size(); mi++) {
        const int map = cli.maps[mi];
        const auto& cps = tracks[mi];
        for (int side : cli.sides) {
            for (int rep = 0; rep < cli.repeats; rep++) {
                if (already_done(agg, map, side, rep)) continue;
                GameRec g = play_one(cli, map, side, cps);
                g.repeat = rep;
                agg.recs.push_back(g);
                agg.games++;
                if (g.mapped_winner == 0)
                    agg.wins_a++;
                else if (g.mapped_winner == 1)
                    agg.wins_b++;
                else
                    agg.draws++;
                std::fprintf(stderr, "game map=%d side=%d rep=%d winner=%d mapped=%d turns=%d %s wr=%.3f\n",
                             g.map, g.side, g.repeat, g.winner, g.mapped_winner, g.turns,
                             g.reason.c_str(),
                             agg.games ? (double)agg.wins_a / agg.games : 0.0);
                if (cli.out) write_json(cli, agg, cli.out);
            }
        }
    }
    if (!cli.out) {
        double wr = agg.games ? (double)agg.wins_a / (double)agg.games : 0.0;
        const auto& m0 = GetTournamentMapsRaw()[0];
        std::printf(
            "{\"schema_version\":\"searchbot.duel.v1\",\"bot_a\":\"%s\",\"bot_b\":\"%s\","
            "\"games\":%d,\"wins_a\":%d,\"wins_b\":%d,\"draws\":%d,\"winrate_a\":%.10g,"
            "\"first_turn_ms\":%d,\"later_turn_ms\":%d,\"repeats_are_wallclock\":true,"
            "\"map0_cps\":[",
            json_escape(cli.bot_a).c_str(), json_escape(cli.bot_b).c_str(), agg.games,
            agg.wins_a, agg.wins_b, agg.draws, wr, cli.first_ms, cli.later_ms);
        for (size_t i = 0; i < m0.size(); i++) {
            if (i) std::putchar(',');
            std::printf("[%d,%d]", (int)m0[i].x, (int)m0[i].y);
        }
        std::printf("],\"games_detail\":[");
        for (size_t i = 0; i < agg.recs.size(); i++) {
            const auto& g = agg.recs[i];
            if (i) std::putchar(',');
            std::printf(
                "{\"map\":%d,\"side\":%d,\"repeat\":%d,\"winner\":%d,\"mapped_winner\":%d,"
                "\"turns\":%d,\"reason\":\"%s\",\"side_swapped\":%s}",
                g.map, g.side, g.repeat, g.winner, g.mapped_winner, g.turns,
                json_escape(g.reason).c_str(), g.side_swapped ? "true" : "false");
        }
        std::printf("]}\n");
        std::fflush(stdout);
    } else {
        // reprint final object to stdout for tests
        FILE* fp = std::fopen(cli.out, "r");
        if (fp) {
            char buf[4096];
            size_t n;
            while ((n = std::fread(buf, 1, sizeof(buf), fp)) > 0)
                std::fwrite(buf, 1, n, stdout);
            std::fclose(fp);
        }
    }
    return 0;
}

#pragma GCC optimize("Ofast,unroll-loops,inline,omit-frame-pointer")
#pragma GCC target("avx2,fma,bmi,bmi2")
#include <array>
#include <cmath>
#include <cstring>
#include <ctime>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace arena {

constexpr int THRUST_SHIELD = -1;
constexpr int THRUST_BOOST = -2;

struct BotAction {
    double target_x = 0;
    double target_y = 0;
    int thrust = 0;
    std::string thrust_str() const {
        if (thrust == THRUST_SHIELD) return "SHIELD";
        if (thrust == THRUST_BOOST) return "BOOST";
        return std::to_string(thrust);
    }
};

namespace ultimate_ga {

// ── Constants ──────────────────────────────────────────────────
static constexpr double PI = 3.14159265358979323846;
static constexpr double DEG_TO_RAD = PI / 180.0;
static constexpr double RAD_TO_DEG = 180.0 / PI;
static constexpr int MAX_H = 8;
static constexpr int MAX_POP = 96;

// Blocker aggression: proximity reward constants
static constexpr double BLOCKER_PROX_CLOSE = 800.0;
static constexpr double BLOCKER_PROX_MED = 2000.0;
static constexpr double BLOCKER_PROX_FAR = 5000.0;
static constexpr double BLOCKER_PROX_WEIGHT = 3.5;
static constexpr double BLOCKER_PERSTEP_PROX_WEIGHT = 0.5;
static constexpr double BLOCKER_INTERPOSE_WEIGHT = 0.8;
static constexpr double BLOCKER_SPEED_LOSS_WEIGHT = 1.5;
static constexpr double BLOCKER_MISALIGN_WEIGHT = 0.4;

// ── RNG (per-bot instance, thread-safe) ────────────────────────
struct RNG {
    unsigned s = 42;
    unsigned xr() { s = (214013 * s + 2531011); return (s >> 16) & 0x7FFF; }
    int ri(int a, int b) { return b <= a ? a : a + (int)(xr() % (unsigned)(b - a + 1)); }
    double rd() { return (double)xr() / 32767.0; }
    void seed(unsigned v) { s = v; }
};

// ── Pod state for internal GA sim ──────────────────────────────
struct Pod {
    double x, y;
    double vx, vy;
    double angle;
    int next;
    int lap;
    int shieldtimer;
    int boosted;
    int isFirstTurn;
};

// ── Physics Engine (from C_SRC) ────────────────────────────────

inline double newCollide(const Pod* p1, const Pod* p2) {
    double dx = p2->x - p1->x;
    double dy = p2->y - p1->y;
    double pLength2 = dx * dx + dy * dy;
    if (pLength2 <= 640000.0) return 0.0;

    double dvx = p2->vx - p1->vx;
    double dvy = p2->vy - p1->vy;
    double dot_val = dx * dvx + dy * dvy;

    if (dot_val > 0.0) return 10.0;

    double vLength2 = dvx * dvx + dvy * dvy;
    if (vLength2 == 0.0) return 10.0;

    double disc = dot_val * dot_val - vLength2 * (pLength2 - 640000.0);
    if (disc <= 0.0) return 10.0;

    double t = (-dot_val - sqrt(disc)) / vLength2;
    return t;
}

inline int cpCollide(double p1x, double p1y, double p2x, double p2y,
                     double cpx, double cpy) {
    double dx = p2x - p1x;
    double dy = p2y - p1y;
    double pd2 = dx * dx + dy * dy;
    double ppx = p1x, ppy = p1y;

    if (pd2 != 0.0) {
        double u = ((cpx - p1x) * dx + (cpy - p1y) * dy) / pd2;
        if (u > 1.0) { ppx = p2x; ppy = p2y; }
        else if (u > 0.0) { ppx = p1x + u * dx; ppy = p1y + u * dy; }
    }

    double distSQ = (ppx - cpx) * (ppx - cpx) + (ppy - cpy) * (ppy - cpy);
    return distSQ < 360000.0;
}

inline void bounce(Pod* a, Pod* b) {
    double nx = b->x - a->x;
    double ny = b->y - a->y;
    double dist = sqrt(nx * nx + ny * ny);
    nx /= dist; ny /= dist;

    double rvx = a->vx - b->vx;
    double rvy = a->vy - b->vy;

    double m1 = (a->shieldtimer == 4) ? 0.1 : 1.0;
    double m2 = (b->shieldtimer == 4) ? 0.1 : 1.0;

    double force = (nx * rvx + ny * rvy) / (m1 + m2);
    if (force < 120.0) force += 120.0;
    else force += force;

    a->vx -= nx * force * m1;
    a->vy -= ny * force * m1;
    b->vx += nx * force * m2;
    b->vy += ny * force * m2;

    if (dist <= 800.0) {
        double ddiff = dist - 800.0;
        a->x += nx * -(-ddiff / 2.0 + 0.00001);
        a->y += ny * -(-ddiff / 2.0 + 0.00001);
        b->x += nx * (-ddiff / 2.0 + 0.00001);
        b->y += ny * (-ddiff / 2.0 + 0.00001);
    }
}

inline void nextTurn(Pod pods[4], const double* CX, const double* CY,
                     int ncp, int laps) {
    double remaining = 1.0;
    double curps_x[4], curps_y[4];
    for (int i = 0; i < 4; i++) { curps_x[i] = pods[i].x; curps_y[i] = pods[i].y; }

    while (remaining > 0.0) {
        double first = remaining;
        int ci = -1, cj = -1;

        for (int i = 3; i > 0; --i) {
            for (int j = i - 1; j >= 0; --j) {
                double t = newCollide(&pods[i], &pods[j]);
                if (t <= first) { first = t; ci = i; cj = j; }
            }
        }

        for (int i = 0; i < 4; i++) {
            pods[i].x += pods[i].vx * first;
            pods[i].y += pods[i].vy * first;
        }

        remaining -= first;

        if (ci != -1) {
            bounce(&pods[ci], &pods[cj]);
        }

        if (remaining > 0.0 && ci != -1) {
            int c_idx[2] = {ci, cj};
            for (int k = 0; k < 2; k++) {
                int idx = c_idx[k];
                if (pods[idx].lap >= laps) continue;
                if (cpCollide(curps_x[idx], curps_y[idx], pods[idx].x, pods[idx].y,
                              CX[pods[idx].next], CY[pods[idx].next])) {
                    pods[idx].next++;
                    if (pods[idx].next >= ncp) { pods[idx].next = 0; pods[idx].lap++; }
                }
            }
            curps_x[ci] = pods[ci].x; curps_y[ci] = pods[ci].y;
            curps_x[cj] = pods[cj].x; curps_y[cj] = pods[cj].y;
        }
    }

    for (int i = 0; i < 4; i++) {
        pods[i].vx = trunc(pods[i].vx * 0.85);
        pods[i].vy = trunc(pods[i].vy * 0.85);
        pods[i].x = floor(pods[i].x + 0.5);
        pods[i].y = floor(pods[i].y + 0.5);
        if (pods[i].shieldtimer > 0) pods[i].shieldtimer--;
    }

    for (int i = 0; i < 4; i++) {
        if (pods[i].lap >= laps) continue;
        if (cpCollide(curps_x[i], curps_y[i], pods[i].x, pods[i].y,
                      CX[pods[i].next], CY[pods[i].next])) {
            pods[i].next++;
            if (pods[i].next >= ncp) { pods[i].next = 0; pods[i].lap++; }
        }
    }

    for (int i = 0; i < 4; i++) {
        if (pods[i].lap >= laps) continue;
        double dx = pods[i].x - CX[pods[i].next];
        double dy = pods[i].y - CY[pods[i].next];
        if (dx * dx + dy * dy == 360000.0) {
            pods[i].next++;
            if (pods[i].next >= ncp) { pods[i].next = 0; pods[i].lap++; }
        }
    }
}

inline void apply_action(Pod* p, double target_x, double target_y,
                         int thrust, int shield, int boost) {
    if (shield) {
        p->shieldtimer = 4;
        if (p->isFirstTurn) p->isFirstTurn = 0;
        return;
    }

    double dx = target_x - p->x;
    double dy = target_y - p->y;

    if (dx != 0.0 || dy != 0.0) {
        double a = atan2(dy, dx);
        if (p->isFirstTurn) {
            p->isFirstTurn = 0;
            p->angle = a;
        } else {
            double diff = fmod(a - p->angle, 2 * PI);
            if (diff >= PI) diff -= 2 * PI;
            else if (diff <= -PI) diff += 2 * PI;
            if (diff <= -18.0 * DEG_TO_RAD) a = p->angle - 18.0 * DEG_TO_RAD;
            else if (diff >= 18.0 * DEG_TO_RAD) a = p->angle + 18.0 * DEG_TO_RAD;
            p->angle = a;
        }
    }

    int t_val = 0;
    if (boost && p->boosted == 0) {
        t_val = 650;
        p->boosted = 1;
    } else {
        t_val = thrust;
        if (t_val > 200) t_val = 200;
        if (t_val < 0) t_val = 0;
    }

    if (p->shieldtimer > 0) t_val = 0;
    if (dx == 0.0 && dy == 0.0) t_val = 0;

    double cc = cos(p->angle);
    double cs = sin(p->angle);
    if (fabs(cc - (-0.28)) < 5e-16 && fabs(fabs(cs) - 0.96) < 5e-16) {
        cc = -0.28;
        cs = (cs > 0) ? 0.96 : -0.96;
    }

    p->vx += cc * t_val;
    p->vy += cs * t_val;
}

// Fast version: angle already known (skips atan2 + 2 trig calls)
// Use when caller already has the desired new angle (e.g., from GA delta)
inline void apply_action_direct(Pod* p, double new_angle,
                                int thrust, int shield, int boost) {
    if (shield) {
        p->shieldtimer = 4;
        if (p->isFirstTurn) p->isFirstTurn = 0;
        return;
    }

    if (p->isFirstTurn) {
        p->isFirstTurn = 0;
    }
    p->angle = new_angle;

    int t_val = 0;
    if (boost && p->boosted == 0) {
        t_val = 650;
        p->boosted = 1;
    } else {
        t_val = thrust;
        if (t_val > 200) t_val = 200;
        if (t_val < 0) t_val = 0;
    }

    if (p->shieldtimer > 0) t_val = 0;

    double cc = cos(p->angle);
    double cs = sin(p->angle);
    if (fabs(cc - (-0.28)) < 5e-16 && fabs(fabs(cs) - 0.96) < 5e-16) {
        cc = -0.28;
        cs = (cs > 0) ? 0.96 : -0.96;
    }

    p->vx += cc * t_val;
    p->vy += cs * t_val;
}

// Fast path for GA-controlled pods: angle delta already known, skip atan2
inline void apply_action_delta(Pod* p, double delta_deg, int thrust,
                               int shield, int boost) {
    if (shield) {
        p->shieldtimer = 4;
        if (p->isFirstTurn) p->isFirstTurn = 0;
        return;
    }

    // Clamp and apply delta directly (no atan2 needed!)
    double delta_rad = delta_deg * DEG_TO_RAD;
    if (delta_rad > 18.0 * DEG_TO_RAD) delta_rad = 18.0 * DEG_TO_RAD;
    else if (delta_rad < -18.0 * DEG_TO_RAD) delta_rad = -18.0 * DEG_TO_RAD;

    if (p->isFirstTurn) {
        p->isFirstTurn = 0;
        // On first turn, angle is set to target direction
        // For GA pods we just set angle = current + delta
    }
    p->angle += delta_rad;

    int t_val = 0;
    if (boost && p->boosted == 0) {
        t_val = 650; p->boosted = 1;
    } else {
        t_val = thrust;
        if (t_val > 200) t_val = 200;
        if (t_val < 0) t_val = 0;
    }
    if (p->shieldtimer > 0) t_val = 0;

    p->vx += cos(p->angle) * t_val;
    p->vy += sin(p->angle) * t_val;
}

// ── GA Individual ──────────────────────────────────────────────
struct Ind {
    double ra[MAX_H], ba[MAX_H];
    int rt[MAX_H], bt[MAX_H];
    int rs, bs;
    double sc;
};

// ── Evaluate (with optional IBR opponent predictions) ──────────
inline double evaluate(
    const Ind* ind, const Pod* base, int H,
    const double* CX, const double* CY, const double* EX, const double* EY,
    const double* DTE, const double* RRX, const double* RRY,
    int ncp, int laps, int mx,
    int rp, int bp, int orp, int obp, int rb,
    int fboost, int rtimeout,
    double dw, double aw, double sw, double lw, double apw,
    double byw, double opw, double sfw, double faw, double rw,
    const double* opp_ra = nullptr, const int* opp_rt = nullptr,
    const double* opp_ba = nullptr, const int* opp_bt = nullptr
) {
    Pod sim[4]; memcpy(sim, base, 4 * sizeof(Pod));
    int ir_cp = sim[rp].next, ir_lap = sim[rp].lap;
    int ior_cp = sim[orp].next, ior_lap = sim[orp].lap;
    double r_act = (double)H + 0.3, o_act = (double)H + 0.3;
    double opp_init_speed = sqrt(sim[orp].vx*sim[orp].vx + sim[orp].vy*sim[orp].vy);
    double blocker_prox_sum = 0.0;  // accumulated per-step proximity reward

    for (int t = 0; t < H; t++) {
        int pre_next[4];
        for (int k = 0; k < 4; k++) pre_next[k] = sim[k].next;

        // Our runner
        int rth = ind->rt[t];
        int rb_flag = 0, rs_flag = 0;
        if (t == 0 && fboost && sim[rp].shieldtimer == 0) rb_flag = 1;
        else if (t == ind->rs && ind->rs < 3 && sim[rp].shieldtimer == 0) rs_flag = 1;

        double ang_rad = sim[rp].angle + ind->ra[t] * DEG_TO_RAD;
        double r_tx = sim[rp].x + cos(ang_rad) * 10000.0;
        double r_ty = sim[rp].y + sin(ang_rad) * 10000.0;
        apply_action(&sim[rp], r_tx, r_ty, rth, rs_flag, rb_flag);

        // Our blocker
        int bth = ind->bt[t];
        int bs_flag = 0;
        if (t == ind->bs && ind->bs < 3 && sim[bp].shieldtimer == 0) bs_flag = 1;

        double bang_rad = sim[bp].angle + ind->ba[t] * DEG_TO_RAD;
        double b_tx = sim[bp].x + cos(bang_rad) * 10000.0;
        double b_ty = sim[bp].y + sin(bang_rad) * 10000.0;
        apply_action(&sim[bp], b_tx, b_ty, bth, bs_flag, 0);

        // Opponent behavior: predicted (IBR) or naive (legacy)
        if (opp_ra) {
            // IBR: use predicted opponent actions
            double oa = sim[orp].angle + opp_ra[t] * DEG_TO_RAD;
            apply_action(&sim[orp], sim[orp].x + cos(oa)*10000.0,
                         sim[orp].y + sin(oa)*10000.0, opp_rt[t], 0, 0);
            double oba = sim[obp].angle + opp_ba[t] * DEG_TO_RAD;
            apply_action(&sim[obp], sim[obp].x + cos(oba)*10000.0,
                         sim[obp].y + sin(oba)*10000.0, opp_bt[t], 0, 0);
        } else {
            // Legacy naive: opp runner aims at entry points
            apply_action(&sim[orp], EX[sim[orp].next], EY[sim[orp].next], 200, 0, 0);
            // Legacy naive: opp blocker chases our runner
            double tx2 = sim[rp].x + sim[rp].vx * 1.5;
            double ty2 = sim[rp].y + sim[rp].vy * 1.5;
            double dx_cp = CX[sim[rp].next] - sim[rp].x;
            double dy_cp = CY[sim[rp].next] - sim[rp].y;
            double dcp = sqrt(dx_cp * dx_cp + dy_cp * dy_cp);
            if (dcp > 0) { tx2 -= 500 * dx_cp / dcp; ty2 -= 500 * dy_cp / dcp; }
            double dd = sqrt((sim[obp].x - sim[rp].x) * (sim[obp].x - sim[rp].x) +
                             (sim[obp].y - sim[rp].y) * (sim[obp].y - sim[rp].y));
            int sh = (dd < 850 && sim[obp].shieldtimer == 0) ? 1 : 0;
            apply_action(&sim[obp], tx2, ty2, 200, sh, 0);
        }

        nextTurn(sim, CX, CY, ncp, laps);

        for (int k = 0; k < 4; k++) {
            if (sim[k].next != pre_next[k]) {
                if (k == rp && r_act > (double)H) r_act = (double)t + 0.5;
                if (k == orp && o_act > (double)H) o_act = (double)t + 0.5;
            }
        }

        // Per-step blocker proximity tracking
        if (!rtimeout) {
            double bdx = sim[bp].x - sim[orp].x;
            double bdy = sim[bp].y - sim[orp].y;
            double bd2 = bdx * bdx + bdy * bdy;
            if (bd2 < BLOCKER_PROX_FAR * BLOCKER_PROX_FAR) {
                double bd = sqrt(bd2);
                double norm = 1.0 - bd / BLOCKER_PROX_FAR;
                blocker_prox_sum += norm * norm;
            }
        }
    }

    // Early exit on off-map runner
    if (sim[rp].x < -2000 || sim[rp].x > 18000 ||
        sim[rp].y < -2000 || sim[rp].y > 11000)
        return -1e9;

    double s = 0;
    // Boundary penalties
    for (int k = 0; k < 2; k++) {
        int pi2 = (k == 0) ? rp : bp;
        if (sim[pi2].x < -1000 || sim[pi2].x > 17000 ||
            sim[pi2].y < -1000 || sim[pi2].y > 10000)
            s -= 100000.0;
    }

    // Win/loss
    if (sim[rp].lap >= laps) s += 1e9;
    if (sim[orp].lap >= laps) s -= 1e9;

    // CP crossing bonus
    {
        int cur = sim[rp].lap * ncp + sim[rp].next;
        int ini = ir_lap * ncp + ir_cp;
        int crossed = cur - ini;
        if (crossed > 0) s += crossed * 30000.0;
    }

    // Runner remaining distance
    if (sim[rp].lap < laps) {
        int rl = sim[rp].lap * ncp + sim[rp].next;
        if (rl < mx) {
            double dx = sim[rp].x - EX[sim[rp].next];
            double dy = sim[rp].y - EY[sim[rp].next];
            s -= (DTE[rl] + sqrt(dx * dx + dy * dy)) * dw;
        }
    }

    // Progress differential: reward being ahead of opponent
    {
        int our_progress = sim[rp].lap * ncp + sim[rp].next;
        int opp_progress = sim[orp].lap * ncp + sim[orp].next;
        int diff = our_progress - opp_progress;
        s += diff * 5000.0;
        if (diff == 0) {
            double our_dx = sim[rp].x - CX[sim[rp].next], our_dy = sim[rp].y - CY[sim[rp].next];
            double opp_dx = sim[orp].x - CX[sim[orp].next], opp_dy = sim[orp].y - CY[sim[orp].next];
            s += (sqrt(opp_dx*opp_dx+opp_dy*opp_dy) - sqrt(our_dx*our_dx+our_dy*our_dy)) * 0.5;
        }
    }

    // Cornering quality: when close to CP, reward velocity toward next-next CP
    {
        double dx_to_cp = CX[sim[rp].next] - sim[rp].x, dy_to_cp = CY[sim[rp].next] - sim[rp].y;
        double dist_to_cp = sqrt(dx_to_cp*dx_to_cp + dy_to_cp*dy_to_cp);
        if (dist_to_cp < 3000.0) {
            int next_next = (sim[rp].next + 1) % ncp;
            double nnx = CX[next_next] - sim[rp].x, nny = CY[next_next] - sim[rp].y;
            double nnd = sqrt(nnx*nnx + nny*nny);
            if (nnd > 0) { double alignment = (sim[rp].vx*nnx + sim[rp].vy*nny) / nnd; s += alignment * 1.5; }
        }
    }


    // Velocity alignment (LEGACY: no velocity field blending)
    {
        double dx = EX[sim[rp].next] - sim[rp].x;
        double dy = EY[sim[rp].next] - sim[rp].y;
        double d = sqrt(dx * dx + dy * dy);
        if (d > 0) {
            double nx = dx / d, ny = dy / d;
            s += (sim[rp].vx * nx + sim[rp].vy * ny) * aw;
            double lat = sim[rp].vx * ny - sim[rp].vy * nx;
            s -= fabs(lat) * lw;
            double ta = atan2(dy, dx);
            double ae = ta - sim[rp].angle;
            while (ae > PI) ae -= 2 * PI;
            while (ae < -PI) ae += 2 * PI;
            s -= fabs(ae * RAD_TO_DEG) * apw;
        }
        s += sqrt(sim[rp].vx * sim[rp].vx + sim[rp].vy * sim[rp].vy) * sw;
    }

    // Cornering quality: near CP, reward velocity toward NEXT CP
    {
        double dx_cp = CX[sim[rp].next] - sim[rp].x;
        double dy_cp = CY[sim[rp].next] - sim[rp].y;
        double dist_cp_sq = dx_cp * dx_cp + dy_cp * dy_cp;
        if (dist_cp_sq < 9000000.0) {  // 3000^2
            int nn = (sim[rp].next + 1) % ncp;
            double nnx = CX[nn] - sim[rp].x, nny = CY[nn] - sim[rp].y;
            double nnd_sq = nnx * nnx + nny * nny;
            if (nnd_sq > 1.0) {
                double nnd = sqrt(nnd_sq);
                double alignment = (sim[rp].vx * nnx + sim[rp].vy * nny) / nnd;
                s += alignment * 0.3;
            }
        }
    }


    // Activation time
    s -= 1000.0 * r_act;

    // Runner evasion: velocity-aware proximity penalty (replaces weak bypass)
    {
        // Predict positions (2-step extrapolation with 0.85 friction)
        double pred_obx = sim[obp].x + sim[obp].vx * 1.85;
        double pred_oby = sim[obp].y + sim[obp].vy * 1.85;
        double pred_rx = sim[rp].x + sim[rp].vx * 1.85;
        double pred_ry = sim[rp].y + sim[rp].vy * 1.85;
        double dx = pred_obx - pred_rx, dy = pred_oby - pred_ry;
        double d2 = dx*dx + dy*dy;
        double threshold2 = 3000.0 * 3000.0;  // 9M
        if (d2 < threshold2) {
            double penalty = byw * 20.0 * (1.0 - d2 / threshold2);
            // Closing velocity: penalize more when pods approach each other
            double cvx = sim[obp].vx - sim[rp].vx, cvy = sim[obp].vy - sim[rp].vy;
            double closing = -(dx*cvx + dy*cvy);  // positive = approaching
            if (closing > 0 && d2 > 1.0) penalty += closing * 0.5;
            // Shield awareness: shielded blocker is 10× mass
            if (sim[obp].shieldtimer >= 3) penalty *= 1.5;
            s -= penalty;
        }
    }

    // Opponent delay
    s += 10000.0 * opw * o_act;
    if (sim[orp].next == ior_cp && sim[orp].lap == ior_lap) {
        double dx = sim[orp].x - CX[sim[orp].next], dy = sim[orp].y - CY[sim[orp].next];
        s += 10.0 * opw * sqrt(dx * dx + dy * dy);
    }

    // Blocker evaluation
    if (rtimeout) {
        if (sim[bp].lap < laps) {
            int bl = sim[bp].lap * ncp + sim[bp].next;
            if (bl < mx) {
                double dx = sim[bp].x - EX[sim[bp].next], dy = sim[bp].y - EY[sim[bp].next];
                s -= (DTE[bl] + sqrt(dx * dx + dy * dy)) * dw;
            }
        }
    } else {
        // Stay in front of opp runner
        {
            double bx = sim[bp].x - sim[orp].x, by = sim[bp].y - sim[orp].y;
            double cx = CX[rb] - sim[orp].x, cy = CY[rb] - sim[orp].y;
            s -= sfw * atan2(fabs(bx * cy - by * cx), bx * cx + by * cy);
        }
        // Face opp runner
        {
            double dx = sim[orp].x - sim[bp].x, dy = sim[orp].y - sim[bp].y;
            double ta = atan2(dy, dx);
            double ae = ta - sim[bp].angle;
            while (ae > PI) ae -= 2 * PI;
            while (ae < -PI) ae += 2 * PI;
            s -= faw * fabs(ae * RAD_TO_DEG);
        }
        // Ram rest point
        {
            double dx = sim[bp].x - RRX[rb], dy = sim[bp].y - RRY[rb];
            double d = sqrt(dx * dx + dy * dy);
            s -= rw * (d < 300 ? (d - 300) * 0.1 : d - 300);
        }
        // Direct proximity reward: strong incentive to stay close to opp runner
        {
            double dx = sim[bp].x - sim[orp].x, dy = sim[bp].y - sim[orp].y;
            double dist = sqrt(dx * dx + dy * dy);
            if (dist < BLOCKER_PROX_FAR) {
                double norm = (BLOCKER_PROX_FAR - dist) / (BLOCKER_PROX_FAR - BLOCKER_PROX_CLOSE);
                if (norm > 1.0) norm = 1.0;
                s += norm * BLOCKER_PROX_WEIGHT * 1000.0;
                if (dist < BLOCKER_PROX_MED) {
                    double close_norm = (BLOCKER_PROX_MED - dist) / (BLOCKER_PROX_MED - BLOCKER_PROX_CLOSE);
                    if (close_norm > 1.0) close_norm = 1.0;
                    s += close_norm * close_norm * BLOCKER_PROX_WEIGHT * 2000.0;
                }
            }
            if (dist > BLOCKER_PROX_MED) {
                s -= (dist - BLOCKER_PROX_MED) * 0.5;
            }
        }
        // Interposition: reward being between opp runner and their next CP
        {
            double opp_to_cp_x = CX[sim[orp].next] - sim[orp].x;
            double opp_to_cp_y = CY[sim[orp].next] - sim[orp].y;
            double opp_to_cp_d = sqrt(opp_to_cp_x * opp_to_cp_x + opp_to_cp_y * opp_to_cp_y);
            if (opp_to_cp_d > 1.0) {
                double nx = opp_to_cp_x / opp_to_cp_d;
                double ny = opp_to_cp_y / opp_to_cp_d;
                double bx = sim[bp].x - sim[orp].x;
                double by = sim[bp].y - sim[orp].y;
                double proj = bx * nx + by * ny;
                double lat = fabs(bx * ny - by * nx);
                if (proj > 0 && proj < opp_to_cp_d) {
                    double frac = proj / opp_to_cp_d;
                    double pos_quality = 1.0 - fabs(frac - 0.35) * 2.0;
                    if (pos_quality < 0) pos_quality = 0;
                    double lat_quality = 1.0 - lat / 1500.0;
                    if (lat_quality < 0) lat_quality = 0;
                    s += pos_quality * lat_quality * BLOCKER_INTERPOSE_WEIGHT * 1000.0;
                }
            }
        }
        // Opponent activation delay bonus
        s += 15000.0 * o_act;
        // Opponent runner speed disruption: reward velocity loss
        {
            double opp_end_speed = sqrt(sim[orp].vx*sim[orp].vx + sim[orp].vy*sim[orp].vy);
            double speed_loss = opp_init_speed - opp_end_speed;
            if (speed_loss > 0) s += speed_loss * BLOCKER_SPEED_LOSS_WEIGHT;
        }
        // Opponent velocity misalignment: reward when opp velocity points away from their CP
        {
            double dx = CX[sim[orp].next] - sim[orp].x;
            double dy = CY[sim[orp].next] - sim[orp].y;
            double d = sqrt(dx*dx + dy*dy);
            if (d > 0) {
                double alignment = (sim[orp].vx*dx + sim[orp].vy*dy) / d;
                s -= alignment * BLOCKER_MISALIGN_WEIGHT;
            }
        }
        // Per-step proximity accumulated reward
        s += BLOCKER_PERSTEP_PROX_WEIGHT * blocker_prox_sum * 1000.0;
    }

    // Shield costs
    if (ind->rs < 3) s -= 330.0;
    if (ind->bs < 3) s -= 495.0;

    // Thrust bonus
    if (ind->rs >= H && ind->rt[0] > 0) s += ind->rt[0] * 0.16;
    if (ind->bs >= H && ind->bt[0] > 0) s += ind->bt[0] * 0.06;

    return s;
}

// ── IBR Phase 1: Evaluate from opponent's perspective ──────────
inline double evaluate_opp_perspective(
    const Ind* ind, const Pod* base, int H,
    const double* CX, const double* CY, const double* EX, const double* EY,
    const double* DTE, int ncp, int laps, int mx,
    int rp, int bp, int orp, int obp,
    const double* our_ra, const int* our_rt,
    const double* our_ba, const int* our_bt,
    double dw, double aw, double sw
) {
    Pod sim[4]; memcpy(sim, base, 4 * sizeof(Pod));
    int ior_cp = sim[orp].next, ior_lap = sim[orp].lap;
    int ir_cp = sim[rp].next, ir_lap = sim[rp].lap;
    for (int t = 0; t < H; t++) {
        // Our pods follow stored previous solution
        { double a = sim[rp].angle + our_ra[t]*DEG_TO_RAD;
          apply_action(&sim[rp], sim[rp].x+cos(a)*10000.0, sim[rp].y+sin(a)*10000.0, our_rt[t], 0, 0); }
        { double a = sim[bp].angle + our_ba[t]*DEG_TO_RAD;
          apply_action(&sim[bp], sim[bp].x+cos(a)*10000.0, sim[bp].y+sin(a)*10000.0, our_bt[t], 0, 0); }
        // Opponent pods are optimized by the GA
        { double a = sim[orp].angle + ind->ra[t]*DEG_TO_RAD;
          apply_action(&sim[orp], sim[orp].x+cos(a)*10000.0, sim[orp].y+sin(a)*10000.0, ind->rt[t], 0, 0); }
        { double a = sim[obp].angle + ind->ba[t]*DEG_TO_RAD;
          apply_action(&sim[obp], sim[obp].x+cos(a)*10000.0, sim[obp].y+sin(a)*10000.0, ind->bt[t], 0, 0); }
        nextTurn(sim, CX, CY, ncp, laps);
    }
    // Early exit on off-map opponent runner
    if (sim[orp].x < -2000 || sim[orp].x > 18000 ||
        sim[orp].y < -2000 || sim[orp].y > 11000)
        return -1e9;

    double s = 0;
    // Opponent wins/loses
    if (sim[orp].lap >= laps) s += 1e9;
    if (sim[rp].lap >= laps) s -= 1e9;
    // Opponent runner progress
    { int cur=sim[orp].lap*ncp+sim[orp].next, ini=ior_lap*ncp+ior_cp;
      if(cur-ini>0) s+=(cur-ini)*15000.0; }
    // Opponent runner distance
    if (sim[orp].lap < laps) { int rl=sim[orp].lap*ncp+sim[orp].next;
        if (rl<mx) { double dx=sim[orp].x-EX[sim[orp].next],dy=sim[orp].y-EY[sim[orp].next];
          s -= dw*(DTE[rl]+sqrt(dx*dx+dy*dy)); } }
    // Opponent runner velocity toward target
    { double dx=EX[sim[orp].next]-sim[orp].x,dy=EY[sim[orp].next]-sim[orp].y,d=sqrt(dx*dx+dy*dy);
      if(d>0){double nx=dx/d,ny=dy/d; s+=(sim[orp].vx*nx+sim[orp].vy*ny)*aw;}
      s+=sqrt(sim[orp].vx*sim[orp].vx+sim[orp].vy*sim[orp].vy)*sw; }
    // Progress differential (SWAPPED: reward opp ahead, penalize us)
    { int opp_progress=sim[orp].lap*ncp+sim[orp].next; int our_progress=sim[rp].lap*ncp+sim[rp].next;
      int diff=opp_progress-our_progress; s+=diff*5000.0;
      if(diff==0){double opp_dx=sim[orp].x-CX[sim[orp].next],opp_dy=sim[orp].y-CY[sim[orp].next];
        double our_dx=sim[rp].x-CX[sim[rp].next],our_dy=sim[rp].y-CY[sim[rp].next];
        s+=(sqrt(our_dx*our_dx+our_dy*our_dy)-sqrt(opp_dx*opp_dx+opp_dy*opp_dy))*0.5;} }
    // Penalize our runner progress
    { int cur=sim[rp].lap*ncp+sim[rp].next, ini=ir_lap*ncp+ir_cp;
      if(cur-ini>0) s-=(cur-ini)*10000.0; }
    // Reward opponent blocker proximity to our runner
    { double dx=sim[obp].x-sim[rp].x,dy=sim[obp].y-sim[rp].y;
      s -= 0.5*sqrt(dx*dx+dy*dy); }
    return s;
}








// ── IBR Phase 1: Mini-GA for opponent prediction ───────────────
static constexpr int OPP_PS = 32;
inline void run_opp_prediction_ga(
    RNG& rng, const Pod* base, int H, int mx,
    const double* CX, const double* CY, const double* EX, const double* EY,
    const double* DTE, int ncp, int laps,
    int rp, int bp, int orp, int obp,
    const double* our_ra, const int* our_rt,
    const double* our_ba, const int* our_bt,
    const double* opp_prev_ra, const int* opp_prev_rt,
    const double* opp_prev_ba, const int* opp_prev_bt,
    int has_opp_prev,
    int budget_us,
    double dw, double aw, double sw,
    // Observed opponent behavior from last turn
    int has_obs, double obs_r_delta, int obs_r_thrust,
    double obs_b_delta, int obs_b_thrust,
    double* out_ora, int* out_ort, double* out_oba, int* out_obt
) {
    Ind pop[OPP_PS];
    int idx = 0;
    // Seed 0: shifted previous opponent prediction
    if (has_opp_prev) {
        for (int t=0;t<H-1;t++){pop[0].ra[t]=opp_prev_ra[t+1];pop[0].rt[t]=opp_prev_rt[t+1];
            pop[0].ba[t]=opp_prev_ba[t+1];pop[0].bt[t]=opp_prev_bt[t+1];}
        pop[0].ra[H-1]=rng.rd()*36-18;pop[0].rt[H-1]=rng.ri(0,200);
        pop[0].ba[H-1]=rng.rd()*36-18;pop[0].bt[H-1]=rng.ri(0,200);
        pop[0].rs=H;pop[0].bs=H;idx=1;
    }
    // Seed: heuristic (aim at entry points, thrust=200)
    if (idx < OPP_PS) {
        for (int t=0;t<H;t++){pop[idx].ra[t]=0;pop[idx].rt[t]=200;pop[idx].ba[t]=0;pop[idx].bt[t]=200;}
        pop[idx].rs=H;pop[idx].bs=H;idx++;
    }
    // Seed: repeat observed opponent behavior from last turn
    if (has_obs && idx < OPP_PS) {
        for (int t=0;t<H;t++){
            pop[idx].ra[t]=obs_r_delta;pop[idx].rt[t]=obs_r_thrust;
            pop[idx].ba[t]=obs_b_delta;pop[idx].bt[t]=obs_b_thrust;
        }
        pop[idx].rs=H;pop[idx].bs=H;idx++;
    }
    // Fill rest with random
    for (int i=idx;i<OPP_PS;i++) {
        for (int t=0;t<H;t++) {
            double v=rng.ri(-400,400)/10.0;pop[i].ra[t]=v<-18?-18:(v>18?18:v);
            int raw=rng.ri(-100,500);pop[i].rt[t]=raw<0?0:(raw>200?200:raw);
            v=rng.ri(-400,400)/10.0;pop[i].ba[t]=v<-18?-18:(v>18?18:v);
            raw=rng.ri(-100,500);pop[i].bt[t]=raw<0?0:(raw>200?200:raw);
        }
        pop[i].rs=H;pop[i].bs=H;
    }
    int best=0,worst=0;
    for (int i=0;i<OPP_PS;i++) {
        pop[i].sc=evaluate_opp_perspective(&pop[i],base,H,CX,CY,EX,EY,DTE,ncp,laps,mx,rp,bp,orp,obp,our_ra,our_rt,our_ba,our_bt,dw,aw,sw);
        if(pop[i].sc>pop[best].sc)best=i;if(pop[i].sc<pop[worst].sc)worst=i;
    }
    double wsc=pop[worst].sc;
    struct timespec t0,tn;clock_gettime(CLOCK_MONOTONIC,&t0);
    int iters=0;Ind child;
    while(1) {
        iters++;if((iters&127)==0){clock_gettime(CLOCK_MONOTONIC,&tn);
            long el=(tn.tv_sec-t0.tv_sec)*1000000L+(tn.tv_nsec-t0.tv_nsec)/1000L;if(el>=budget_us)break;}
        int p1=rng.ri(0,OPP_PS-1),p2=rng.ri(0,OPP_PS-1);int par=(pop[p1].sc>=pop[p2].sc)?p1:p2;
        if(rng.ri(0,3)==0){p1=rng.ri(0,OPP_PS-1);p2=rng.ri(0,OPP_PS-1);int par2=(pop[p1].sc>=pop[p2].sc)?p1:p2;
            for(int t=0;t<H;t++){if(rng.rd()<0.5){child.ra[t]=pop[par].ra[t];child.rt[t]=pop[par].rt[t];}else{child.ra[t]=pop[par2].ra[t];child.rt[t]=pop[par2].rt[t];}
                if(rng.rd()<0.5){child.ba[t]=pop[par].ba[t];child.bt[t]=pop[par].bt[t];}else{child.ba[t]=pop[par2].ba[t];child.bt[t]=pop[par2].bt[t];}}
        }else{for(int t=0;t<H;t++){child.ra[t]=pop[par].ra[t];child.rt[t]=pop[par].rt[t];child.ba[t]=pop[par].ba[t];child.bt[t]=pop[par].bt[t];
                if(rng.rd()<0.5){double v=rng.ri(-400,400)/10.0;child.ra[t]=v<-18?-18:(v>18?18:v);}
                if(rng.rd()<0.5){int raw=rng.ri(-100,500);child.rt[t]=raw<0?0:(raw>200?200:raw);}
                if(rng.rd()<0.5){double v=rng.ri(-400,400)/10.0;child.ba[t]=v<-18?-18:(v>18?18:v);}
                if(rng.rd()<0.5){int raw=rng.ri(-100,500);child.bt[t]=raw<0?0:(raw>200?200:raw);}}}
        child.rs=H;child.bs=H;
        {int si=rng.ri(0,H-1);double v=child.ra[si]+rng.rd()*24-12;child.ra[si]=v<-18?-18:(v>18?18:v);}
        {int si=rng.ri(0,H-1);int v=child.rt[si]+rng.ri(-50,50);child.rt[si]=v<0?0:(v>200?200:v);}
        {int si=rng.ri(0,H-1);double v=child.ba[si]+rng.rd()*24-12;child.ba[si]=v<-18?-18:(v>18?18:v);}
        {int si=rng.ri(0,H-1);int v=child.bt[si]+rng.ri(-50,50);child.bt[si]=v<0?0:(v>200?200:v);}
        child.sc=evaluate_opp_perspective(&child,base,H,CX,CY,EX,EY,DTE,ncp,laps,mx,rp,bp,orp,obp,our_ra,our_rt,our_ba,our_bt,dw,aw,sw);
        if(child.sc>pop[best].sc)best=worst;pop[worst]=child;
        if(child.sc>wsc){worst=0;wsc=pop[0].sc;for(int i=1;i<OPP_PS;i++)if(pop[i].sc<wsc){worst=i;wsc=pop[i].sc;}}
    }
    memcpy(out_ora,pop[best].ra,H*sizeof(double));memcpy(out_ort,pop[best].rt,H*sizeof(int));
    memcpy(out_oba,pop[best].ba,H*sizeof(double));memcpy(out_obt,pop[best].bt,H*sizeof(int));
}

// ── GA Engine (with optional IBR opponent predictions) ─────────
inline void run_combined_ga(
    RNG& rng,
    const Pod* base, int H, int PS, int mx,
    const double* CX, const double* CY,
    const double* EX, const double* EY,
    const double* DTE, const double* RRX, const double* RRY,
    int ncp, int laps,
    int rp, int bp, int orp, int obp, int rb,
    int fboost, int rtimeout,
    const double* prev_ra, const int* prev_rt,
    const double* prev_ba, const int* prev_bt,
    int prev_rs, int prev_bs, int has_prev,
    int budget_us,
    double dw, double aw, double sw, double lw, double apw,
    double byw, double opw, double sfw, double faw, double rw,
    double* out_ra, int* out_rt, double* out_ba, int* out_bt,
    int* out_rs, int* out_bs,
    const double* opp_ra = nullptr, const int* opp_rt = nullptr,
    const double* opp_ba = nullptr, const int* opp_bt = nullptr
) {
    Ind pop[MAX_POP];
    int idx = 0;

    // Seed 0: previous best shifted
    if (has_prev) {
        for (int t = 0; t < H - 1; t++) {
            pop[0].ra[t] = prev_ra[t + 1]; pop[0].rt[t] = prev_rt[t + 1];
            pop[0].ba[t] = prev_ba[t + 1]; pop[0].bt[t] = prev_bt[t + 1];
        }
        pop[0].ra[H - 1] = rng.rd() * 36 - 18; pop[0].rt[H - 1] = rng.ri(0, 200);
        pop[0].ba[H - 1] = rng.rd() * 36 - 18; pop[0].bt[H - 1] = rng.ri(0, 200);
        pop[0].rs = (prev_rs > 0 && prev_rs < H) ? prev_rs - 1 : H;
        pop[0].bs = (prev_bs > 0 && prev_bs < H) ? prev_bs - 1 : H;
        idx = 1;
        // Warm start: 4 perturbed variants of seed 0
        for (int pv = 0; pv < 4 && idx < PS; pv++) {
            memcpy(&pop[idx], &pop[0], sizeof(Ind));
            for (int t = 0; t < H; t++) {
                double va = pop[idx].ra[t] + rng.rd() * 12 - 6;
                pop[idx].ra[t] = va < -18 ? -18 : (va > 18 ? 18 : va);
                int vt = pop[idx].rt[t] + rng.ri(-30, 30);
                pop[idx].rt[t] = vt < 0 ? 0 : (vt > 200 ? 200 : vt);
                double vba = pop[idx].ba[t] + rng.rd() * 12 - 6;
                pop[idx].ba[t] = vba < -18 ? -18 : (vba > 18 ? 18 : vba);
                int vbt = pop[idx].bt[t] + rng.ri(-30, 30);
                pop[idx].bt[t] = vbt < 0 ? 0 : (vbt > 200 ? 200 : vbt);
            }
            pop[idx].rs = pop[0].rs; pop[idx].bs = pop[0].bs;
            idx++;
        }
    }

    // Heuristic seeds
    {
        Pod hsim[4]; memcpy(hsim, base, 4 * sizeof(Pod));
        for (int t = 0; t < H; t++) {
            double tx = EX[hsim[rp].next], ty = EY[hsim[rp].next];
            double ddx = tx - hsim[rp].x, ddy = ty - hsim[rp].y;
            double dd = sqrt(ddx * ddx + ddy * ddy);
            if (dd > 0) {
                double ux = ddx / dd, uy = ddy / dd;
                double perp = hsim[rp].vx * (-uy) + hsim[rp].vy * ux;
                double cf = fabs(perp) * 3.0; if (cf > 800) cf = 800;
                if (perp > 0) { tx += uy * cf; ty -= ux * cf; }
                else { tx -= uy * cf; ty += ux * cf; }
                if (dd < 2500) {
                    int nnc = (hsim[rp].next + 1) % ncp;
                    double bl = (2500 - dd) / 2500;
                    tx = tx * (1 - bl) + EX[nnc] * bl; ty = ty * (1 - bl) + EY[nnc] * bl;
                }
            }
            double ta = atan2(ty - hsim[rp].y, tx - hsim[rp].x);
            double diff = ta - hsim[rp].angle;
            while (diff > PI) diff -= 2 * PI;
            while (diff < -PI) diff += 2 * PI;
            double diff_deg = diff * RAD_TO_DEG;
            if (diff_deg > 18) diff_deg = 18;
            if (diff_deg < -18) diff_deg = -18;

            if (idx < PS) { pop[idx].ra[t] = diff_deg; pop[idx].rt[t] = 200; }
            if (idx + 1 < PS) { pop[idx + 1].ra[t] = diff_deg; pop[idx + 1].rt[t] = 150; }

            double ang_rad2 = hsim[rp].angle + diff_deg * DEG_TO_RAD;
            double r_tx2 = hsim[rp].x + cos(ang_rad2) * 10000.0;
            double r_ty2 = hsim[rp].y + sin(ang_rad2) * 10000.0;
            apply_action(&hsim[rp], r_tx2, r_ty2, 200, 0, 0);

            double btx = RRX[rb], bty = RRY[rb];
            double bd = sqrt((hsim[bp].x - hsim[orp].x) * (hsim[bp].x - hsim[orp].x) +
                             (hsim[bp].y - hsim[orp].y) * (hsim[bp].y - hsim[orp].y));
            if (bd < 3000) { btx = hsim[orp].x + hsim[orp].vx * 2; bty = hsim[orp].y + hsim[orp].vy * 2; }

            double bta = atan2(bty - hsim[bp].y, btx - hsim[bp].x);
            double bdiff = bta - hsim[bp].angle;
            while (bdiff > PI) bdiff -= 2 * PI;
            while (bdiff < -PI) bdiff += 2 * PI;
            double bdiff_deg = bdiff * RAD_TO_DEG;
            if (bdiff_deg > 18) bdiff_deg = 18;
            if (bdiff_deg < -18) bdiff_deg = -18;

            if (idx < PS) { pop[idx].ba[t] = bdiff_deg; pop[idx].bt[t] = 200; }
            if (idx + 1 < PS) { pop[idx + 1].ba[t] = bdiff_deg; pop[idx + 1].bt[t] = 200; }

            double bang_rad2 = hsim[bp].angle + bdiff_deg * DEG_TO_RAD;
            double b_tx2 = hsim[bp].x + cos(bang_rad2) * 10000.0;
            double b_ty2 = hsim[bp].y + sin(bang_rad2) * 10000.0;
            apply_action(&hsim[bp], b_tx2, b_ty2, 200, 0, 0);

            apply_action(&hsim[orp], EX[hsim[orp].next], EY[hsim[orp].next], 200, 0, 0);
            apply_action(&hsim[obp], hsim[rp].x, hsim[rp].y, 200, 0, 0);

            nextTurn(hsim, CX, CY, ncp, laps);
        }
        if (idx < PS) { pop[idx].rs = H; pop[idx].bs = H; idx++; }
        if (idx < PS) { pop[idx].rs = H; pop[idx].bs = H; idx++; }
    }

    // Aggressive intercept seed: blocker aims 3 turns ahead of opponent runner
    if (idx + 1 < PS) {
        Pod isim[4]; memcpy(isim, base, 4 * sizeof(Pod));
        for (int t = 0; t < H; t++) {
            // Runner: aim at entry point
            double itx = EX[isim[rp].next], ity = EY[isim[rp].next];
            double ita = atan2(ity - isim[rp].y, itx - isim[rp].x);
            double idiff = ita - isim[rp].angle;
            while (idiff > PI) idiff -= 2 * PI;
            while (idiff < -PI) idiff += 2 * PI;
            double idiff_deg = idiff * RAD_TO_DEG;
            if (idiff_deg > 18) idiff_deg = 18;
            if (idiff_deg < -18) idiff_deg = -18;
            pop[idx].ra[t] = idiff_deg; pop[idx].rt[t] = 200;

            // Blocker: intercept 3 turns ahead of opponent
            double pred_x = isim[orp].x + isim[orp].vx * 3;
            double pred_y = isim[orp].y + isim[orp].vy * 3;
            double ibta = atan2(pred_y - isim[bp].y, pred_x - isim[bp].x);
            double ibdiff = ibta - isim[bp].angle;
            while (ibdiff > PI) ibdiff -= 2 * PI;
            while (ibdiff < -PI) ibdiff += 2 * PI;
            double ibdiff_deg = ibdiff * RAD_TO_DEG;
            if (ibdiff_deg > 18) ibdiff_deg = 18;
            if (ibdiff_deg < -18) ibdiff_deg = -18;
            pop[idx].ba[t] = ibdiff_deg; pop[idx].bt[t] = 200;

            // Simulate forward
            apply_action(&isim[rp], isim[rp].x + cos(isim[rp].angle + idiff_deg * DEG_TO_RAD) * 10000.0,
                         isim[rp].y + sin(isim[rp].angle + idiff_deg * DEG_TO_RAD) * 10000.0, 200, 0, 0);
            apply_action(&isim[bp], isim[bp].x + cos(isim[bp].angle + ibdiff_deg * DEG_TO_RAD) * 10000.0,
                         isim[bp].y + sin(isim[bp].angle + ibdiff_deg * DEG_TO_RAD) * 10000.0, 200, 0, 0);
            apply_action(&isim[orp], EX[isim[orp].next], EY[isim[orp].next], 200, 0, 0);
            apply_action(&isim[obp], isim[rp].x, isim[rp].y, 200, 0, 0);
            nextTurn(isim, CX, CY, ncp, laps);
        }
        pop[idx].rs = H; pop[idx].bs = H; idx++;
    }

    // Shield variants
    {
        int src = 0;
        for (int sv = 0; sv < 4 && idx < PS; sv++) {
            memcpy(&pop[idx], &pop[src], sizeof(Ind));
            switch (sv) {
                case 0: pop[idx].rs = 0; pop[idx].bs = H; break;
                case 1: pop[idx].rs = H; pop[idx].bs = 0; break;
                case 2: pop[idx].rs = 1; pop[idx].bs = H; break;
                case 3: pop[idx].rs = H; pop[idx].bs = 1; break;
            }
            idx++;
        }
    }

    // Extreme seeds
    if (idx + 4 <= PS) {
        for (int t = 0; t < H; t++) {
            pop[idx].ra[t] = 0; pop[idx].rt[t] = 200; pop[idx].ba[t] = 0; pop[idx].bt[t] = 200;
            pop[idx + 1].ra[t] = 0; pop[idx + 1].rt[t] = 0; pop[idx + 1].ba[t] = 0; pop[idx + 1].bt[t] = 0;
            pop[idx + 2].ra[t] = -18; pop[idx + 2].rt[t] = 200; pop[idx + 2].ba[t] = -18; pop[idx + 2].bt[t] = 200;
            pop[idx + 3].ra[t] = 18; pop[idx + 3].rt[t] = 200; pop[idx + 3].ba[t] = 18; pop[idx + 3].bt[t] = 200;
        }
        pop[idx].rs = H; pop[idx].bs = H;
        pop[idx + 1].rs = H; pop[idx + 1].bs = H;
        pop[idx + 2].rs = H; pop[idx + 2].bs = H;
        pop[idx + 3].rs = H; pop[idx + 3].bs = H;
        idx += 4;
    }

    // Fill remaining with random
    for (int i = idx; i < PS; i++) {
        for (int t = 0; t < H; t++) {
            double v = rng.ri(-400, 400) / 10.0;
            pop[i].ra[t] = v < -18 ? -18 : (v > 18 ? 18 : v);
            int raw = rng.ri(-100, 500);
            pop[i].rt[t] = raw < 0 ? 0 : (raw > 200 ? 200 : raw);

            v = rng.ri(-400, 400) / 10.0;
            pop[i].ba[t] = v < -18 ? -18 : (v > 18 ? 18 : v);
            raw = rng.ri(-100, 500);
            pop[i].bt[t] = raw < 0 ? 0 : (raw > 200 ? 200 : raw);
        }
        pop[i].rs = rng.ri(0, H + 4); pop[i].bs = rng.ri(0, H + 4);
    }

    // Initial evaluation
    int best = 0, worst = 0;
    for (int i = 0; i < PS; i++) {
        pop[i].sc = evaluate(&pop[i], base, H, CX, CY, EX, EY, DTE, RRX, RRY,
                             ncp, laps, mx, rp, bp, orp, obp, rb, fboost, rtimeout,
                             dw, aw, sw, lw, apw, byw, opw, sfw, faw, rw,
                             opp_ra, opp_rt, opp_ba, opp_bt);
        if (pop[i].sc > pop[best].sc) best = i;
        if (pop[i].sc < pop[worst].sc) worst = i;
    }
    double wsc = pop[worst].sc;

    // Evolution loop
    struct timespec t0, tn; clock_gettime(CLOCK_MONOTONIC, &t0);
    int iters = 0; double amplitude = 1.0;
    Ind child;

    while (1) {
        iters++;
        if ((iters & 255) == 0) {
            clock_gettime(CLOCK_MONOTONIC, &tn);
            long el = (tn.tv_sec - t0.tv_sec) * 1000000L + (tn.tv_nsec - t0.tv_nsec) / 1000L;
            if (el >= budget_us) break;
            amplitude = 1.0 - (double)el / (double)budget_us;
            if (amplitude < 0) amplitude = 0;
        }

        // Stagnation
        if (pop[best].sc < wsc + 0.3) {
            for (int i = 0; i < PS; i++) if (i != best) pop[i].sc -= 2000;
            wsc -= 2000;
        }

        // Tournament select
        int p1 = rng.ri(0, PS - 1), p2 = rng.ri(0, PS - 1);
        int par = (pop[p1].sc >= pop[p2].sc) ? p1 : p2;
        double threshold = 0.25 + amplitude;

        if (rng.ri(0, 4) == 0) {
            // LEGACY: Uniform crossover
            p1 = rng.ri(0, PS - 1); p2 = rng.ri(0, PS - 1);
            int par2 = (pop[p1].sc >= pop[p2].sc) ? p1 : p2;
            for (int t = 0; t < H; t++) {
                if (rng.rd() < 0.5) { child.ra[t] = pop[par].ra[t]; child.rt[t] = pop[par].rt[t]; }
                else { child.ra[t] = pop[par2].ra[t]; child.rt[t] = pop[par2].rt[t]; }
                if (rng.rd() < 0.5) { child.ba[t] = pop[par].ba[t]; child.bt[t] = pop[par].bt[t]; }
                else { child.ba[t] = pop[par2].ba[t]; child.bt[t] = pop[par2].bt[t]; }
            }
            child.rs = (rng.rd() < 0.5) ? pop[par].rs : pop[par2].rs;
            child.bs = (rng.rd() < 0.5) ? pop[par].bs : pop[par2].bs;
        } else {
            // Mutation
            for (int t = 0; t < H; t++) {
                child.ra[t] = pop[par].ra[t]; child.rt[t] = pop[par].rt[t];
                child.ba[t] = pop[par].ba[t]; child.bt[t] = pop[par].bt[t];
                if (rng.rd() < threshold) { double v = rng.ri(-400, 400) / 10.0; child.ra[t] = v < -18 ? -18 : (v > 18 ? 18 : v); }
                if (rng.rd() < threshold) { int raw = rng.ri(-100, 500); child.rt[t] = raw < 0 ? 0 : (raw > 200 ? 200 : raw); }
                if (rng.rd() < threshold) { double v = rng.ri(-400, 400) / 10.0; child.ba[t] = v < -18 ? -18 : (v > 18 ? 18 : v); }
                if (rng.rd() < threshold) { int raw = rng.ri(-100, 500); child.bt[t] = raw < 0 ? 0 : (raw > 200 ? 200 : raw); }
            }
            child.rs = pop[par].rs; child.bs = pop[par].bs;
            if (rng.rd() < threshold) child.rs = rng.ri(0, H + 4);
            if (rng.rd() < threshold) child.bs = rng.ri(0, H + 4);
        }

        // Small mutation
        { int si = rng.ri(0, H - 1); double v = child.ra[si] + rng.rd() * 24 - 12; child.ra[si] = v < -18 ? -18 : (v > 18 ? 18 : v); }
        { int si = rng.ri(0, H - 1); int v = child.rt[si] + rng.ri(-50, 50); child.rt[si] = v < 0 ? 0 : (v > 200 ? 200 : v); }
        { int si = rng.ri(0, H - 1); double v = child.ba[si] + rng.rd() * 24 - 12; child.ba[si] = v < -18 ? -18 : (v > 18 ? 18 : v); }
        { int si = rng.ri(0, H - 1); int v = child.bt[si] + rng.ri(-50, 50); child.bt[si] = v < 0 ? 0 : (v > 200 ? 200 : v); }

        child.sc = evaluate(&child, base, H, CX, CY, EX, EY, DTE, RRX, RRY,
                            ncp, laps, mx, rp, bp, orp, obp, rb, fboost, rtimeout,
                            dw, aw, sw, lw, apw, byw, opw, sfw, faw, rw,
                            opp_ra, opp_rt, opp_ba, opp_bt);

        if (child.sc > pop[best].sc) best = worst;
        pop[worst] = child;
        if (child.sc > wsc) {
            worst = 0; wsc = pop[0].sc;
            for (int i = 1; i < PS; i++) if (pop[i].sc < wsc) { worst = i; wsc = pop[i].sc; }
        }
    }

    // Output
    memcpy(out_ra, pop[best].ra, H * sizeof(double));
    memcpy(out_rt, pop[best].rt, H * sizeof(int));
    memcpy(out_ba, pop[best].ba, H * sizeof(double));
    memcpy(out_bt, pop[best].bt, H * sizeof(int));
    *out_rs = pop[best].rs; *out_bs = pop[best].bs;
}

}  // namespace ultimate_ga
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// UltimateBot — full arena::Bot implementation
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

class UltimateBot {
 public:
    void init(int laps,
              const std::vector<std::pair<double, double>>& checkpoints) {
        laps_ = laps;
        ncp_ = static_cast<int>(checkpoints.size());
        CX_.resize(ncp_); CY_.resize(ncp_);
        for (int i = 0; i < ncp_; i++) {
            CX_[i] = checkpoints[i].first;
            CY_[i] = checkpoints[i].second;
        }
        compute_entry_points();
        compute_dte();
        compute_ram_rest_points();
        compute_weights();
        // Precompute per-CP headings
        for (int i = 0; i < ncp_; i++) {
            int n = (i + 1) % ncp_;
            heading_to_next_[i] = atan2(CY_[n] - CY_[i], CX_[n] - CX_[i]);
        }
        turn_ = 0;
        rng_.seed(42);
        // Reset per-game state
        for (int i = 0; i < 2; i++) {
            ml_[i] = 0; ol_[i] = 0;
            pmc_[i] = -1; poc_[i] = -1;
            msc_[i] = 0; mbu_[i] = false;
        }
        has_prev_ = false;
        has_opp_prev_ = false;
        prev_rs_ = H_; prev_bs_ = H_;
    }

    std::pair<BotAction, BotAction> get_actions(
        const std::array<std::array<double, 6>, 4>& pods) {
        turn_++;
        using namespace ultimate_ga;

        double px[4], py[4], pvx[4], pvy[4], pang[4];
        int pnc[4], plp[4], psd[4];

        // Parse my pods (0,1)
        for (int i = 0; i < 2; i++) {
            px[i] = pods[i][0]; py[i] = pods[i][1];
            pvx[i] = pods[i][2]; pvy[i] = pods[i][3];
            pang[i] = pods[i][4]; pnc[i] = static_cast<int>(pods[i][5]);
            if (pang[i] < 0) {
                pang[i] = atan2(CY_[pnc[i]] - py[i], CX_[pnc[i]] - px[i]) * RAD_TO_DEG;
                if (pang[i] < 0) pang[i] += 360;
            }
            if (pmc_[i] == -1) pmc_[i] = pnc[i];
            else if (pnc[i] == 0 && pmc_[i] == ncp_ - 1) ml_[i]++;
            pmc_[i] = pnc[i]; plp[i] = ml_[i]; psd[i] = msc_[i];
            if (msc_[i] > 0) msc_[i]--;
        }

        // Parse opp pods (2,3)
        for (int i = 0; i < 2; i++) {
            px[i + 2] = pods[i + 2][0]; py[i + 2] = pods[i + 2][1];
            pvx[i + 2] = pods[i + 2][2]; pvy[i + 2] = pods[i + 2][3];
            pang[i + 2] = pods[i + 2][4]; pnc[i + 2] = static_cast<int>(pods[i + 2][5]);
            if (pang[i + 2] < 0) {
                pang[i + 2] = atan2(CY_[pnc[i + 2]] - py[i + 2], CX_[pnc[i + 2]] - px[i + 2]) * RAD_TO_DEG;
                if (pang[i + 2] < 0) pang[i + 2] += 360;
            }
            if (poc_[i] == -1) poc_[i] = pnc[i + 2];
            else if (pnc[i + 2] == 0 && poc_[i] == ncp_ - 1) ol_[i]++;
            poc_[i] = pnc[i + 2]; plp[i + 2] = ol_[i]; psd[i + 2] = 0;
        }

        // Compute observed opponent actions from last turn
        if (has_opp_obs_) {
            for (int i = 0; i < 2; i++) {
                // Angle delta = current angle - previous angle (what they turned)
                double ang_now = pang[i + 2] * DEG_TO_RAD;
                double ang_prev = prev_opp_ang_[i] * DEG_TO_RAD;
                double delta = ang_now - ang_prev;
                while (delta > PI) delta -= 2 * PI;
                while (delta < -PI) delta += 2 * PI;
                obs_opp_delta_[i] = delta * RAD_TO_DEG;

                // Thrust estimate from velocity change
                // new_vx = trunc((old_vx + cos(angle) * thrust) * 0.85)
                // Approximate: thrust ≈ (vx - old_vx*0.85) / cos(angle) / 0.85
                double ca = cos(ang_now), sa = sin(ang_now);
                double dvx = pvx[i + 2] - prev_opp_vx_[i] * 0.85;
                double dvy = pvy[i + 2] - prev_opp_vy_[i] * 0.85;
                // Project velocity change onto heading direction
                double thrust_est = 0;
                if (ca * ca + sa * sa > 0.1) {
                    thrust_est = (dvx * ca + dvy * sa);
                }
                obs_opp_thrust_[i] = std::max(0, std::min(200, (int)(thrust_est + 0.5)));
            }
        }
        // Save current state for next turn's observation
        for (int i = 0; i < 2; i++) {
            prev_opp_ang_[i] = pang[i + 2];
            prev_opp_x_[i] = px[i + 2]; prev_opp_y_[i] = py[i + 2];
            prev_opp_vx_[i] = pvx[i + 2]; prev_opp_vy_[i] = pvy[i + 2];
        }
        has_opp_obs_ = true;

        int budget_us = (turn_ == 1) ? 1000000 : 75000;  // 1000ms first turn, 75ms after

        // Role assignment using DTE
        auto rprog = [&](int idx) -> double {
            int prog = plp[idx] * ncp_ + pnc[idx];
            int clamped = prog < mx_ ? prog : mx_ - 1;
            double dx = px[idx] - EX_[pnc[idx]], dy = py[idx] - EY_[pnc[idx]];
            return DTE_[clamped] + sqrt(dx * dx + dy * dy);
        };

        int rp = (rprog(1) < rprog(0) - 200) ? 1 : 0;
        int bp = 1 - rp;
        int orp_abs = (rprog(2) < rprog(3)) ? 2 : 3;
        int obp_abs = 5 - orp_abs;

        // Blocker target CP
        int rb = pnc[orp_abs];
        {
            double od = hypot(px[orp_abs] - CX_[rb], py[orp_abs] - CY_[rb]);
            double md = hypot(px[bp] - CX_[rb], py[bp] - CY_[rb]);
            if (md > od + 2200) {
                int nb = (rb + 1) % ncp_;
                double od2 = od + hypot(CX_[rb] - CX_[nb], CY_[rb] - CY_[nb]);
                double md2 = hypot(px[bp] - CX_[nb], py[bp] - CY_[nb]);
                if (md2 < od2 - 2200) rb = nb;
                else rb = (nb + 1) % ncp_;
            }
        }

        // Boost check for runner
        int fboost = 0;
        if (!mbu_[rp] && psd[rp] == 0) {
            double dd = hypot(px[rp] - EX_[pnc[rp]], py[rp] - EY_[pnc[rp]]);
            double ta = atan2(EY_[pnc[rp]] - py[rp], EX_[pnc[rp]] - px[rp]) * RAD_TO_DEG;
            if (ta < 0) ta += 360;
            double ae = ta - pang[rp];
            while (ae > 180) ae -= 360;
            while (ae < -180) ae += 360;
            if (dd > 5000 && fabs(ae) < 5) fboost = 1;
        }

        int rtimeout = 0;

        // Build base pods for GA
        Pod base[4];
        for (int i = 0; i < 4; i++) {
            base[i].x = px[i]; base[i].y = py[i];
            base[i].vx = pvx[i]; base[i].vy = pvy[i];
            base[i].angle = pang[i] * DEG_TO_RAD;
            base[i].next = pnc[i]; base[i].lap = plp[i];
            base[i].shieldtimer = psd[i];
            base[i].boosted = (i < 2 && mbu_[i]) ? 1 : 0;
            base[i].isFirstTurn = (turn_ == 1) ? 1 : 0;
        }

        double out_ra[MAX_H], out_ba[MAX_H];
        int out_rt[MAX_H], out_bt[MAX_H];
        int out_rs, out_bs;

        // ── IBR Two-Phase Pipeline ──
        double pred_ora[MAX_H]={}, pred_oba[MAX_H]={};
        int pred_ort[MAX_H]={}, pred_obt[MAX_H]={};
        bool use_ibr = has_prev_ && turn_ > 1;
        int phase2_budget = budget_us;

        if (use_ibr) {
            // Phase 1: Predict opponent moves (5ms — optimal balance)
            int p1_budget = 5000;
            // Shift our previous solution by 1 (turn 0 already executed)
            double shifted_ra[ultimate_ga::MAX_H], shifted_ba[ultimate_ga::MAX_H];
            int shifted_rt[ultimate_ga::MAX_H], shifted_bt[ultimate_ga::MAX_H];
            for (int t = 0; t < H_ - 1; t++) {
                shifted_ra[t] = prev_ra_[t+1]; shifted_rt[t] = prev_rt_[t+1];
                shifted_ba[t] = prev_ba_[t+1]; shifted_bt[t] = prev_bt_[t+1];
            }
            shifted_ra[H_-1] = 0; shifted_rt[H_-1] = 200;
            shifted_ba[H_-1] = 0; shifted_bt[H_-1] = 200;
            run_opp_prediction_ga(
                rng_, base, H_, mx_,
                CX_.data(), CY_.data(), EX_.data(), EY_.data(),
                DTE_.data(), ncp_, laps_,
                rp, bp, orp_abs, obp_abs,
                shifted_ra, shifted_rt, shifted_ba, shifted_bt,
                opp_prev_ra_, opp_prev_rt_, opp_prev_ba_, opp_prev_bt_,
                has_opp_prev_ ? 1 : 0,
                p1_budget, dw_, aw_, sw_,
                has_opp_obs_ ? 1 : 0,
                obs_opp_delta_[orp_abs - 2], obs_opp_thrust_[orp_abs - 2],
                obs_opp_delta_[obp_abs - 2], obs_opp_thrust_[obp_abs - 2],
                pred_ora, pred_ort, pred_oba, pred_obt);
            // Save opponent prediction for next turn's Phase 1 seeding
            memcpy(opp_prev_ra_, pred_ora, H_ * sizeof(double));
            memcpy(opp_prev_rt_, pred_ort, H_ * sizeof(int));
            memcpy(opp_prev_ba_, pred_oba, H_ * sizeof(double));
            memcpy(opp_prev_bt_, pred_obt, H_ * sizeof(int));
            has_opp_prev_ = true;
            phase2_budget = budget_us - p1_budget;
            if (phase2_budget < 30000) phase2_budget = 30000;
        }

        // Phase 2: Our best response
        run_combined_ga(
            rng_, base, H_, PS_, mx_,
            CX_.data(), CY_.data(), EX_.data(), EY_.data(),
            DTE_.data(), RRX_.data(), RRY_.data(),
            ncp_, laps_,
            rp, bp, orp_abs, obp_abs, rb,
            fboost, rtimeout,
            prev_ra_, prev_rt_, prev_ba_, prev_bt_,
            prev_rs_, prev_bs_, has_prev_ ? 1 : 0,
            phase2_budget,
            dw_, aw_, sw_, lw_, apw_, byw_, opw_, sfw_, faw_, rw_,
            out_ra, out_rt, out_ba, out_bt,
            &out_rs, &out_bs,
            use_ibr ? pred_ora : nullptr,
            use_ibr ? pred_ort : nullptr,
            use_ibr ? pred_oba : nullptr,
            use_ibr ? pred_obt : nullptr);
    


        // Save for next turn
        memcpy(prev_ra_, out_ra, H_ * sizeof(double));
        memcpy(prev_rt_, out_rt, H_ * sizeof(int));
        memcpy(prev_ba_, out_ba, H_ * sizeof(double));
        memcpy(prev_bt_, out_bt, H_ * sizeof(int));
        prev_rs_ = out_rs; prev_bs_ = out_bs;
        has_prev_ = true;

        // Convert runner output
        double r_da = std::max(-18.0, std::min(18.0, out_ra[0]));
        double r_ang_deg = fmod(pang[rp] + r_da, 360.0);
        if (r_ang_deg < 0) r_ang_deg += 360;
        double r_rad = r_ang_deg * DEG_TO_RAD;
        double rtx = px[rp] + cos(r_rad) * 10000;
        double rty = py[rp] + sin(r_rad) * 10000;

        bool r_shield = out_rs == 0 && psd[rp] == 0;
        bool r_boost = fboost && !mbu_[rp] && !r_shield;

        BotAction runner_act;
        runner_act.target_x = rtx;
        runner_act.target_y = rty;
        if (r_shield) {
            runner_act.thrust = THRUST_SHIELD; msc_[rp] = 3;
        } else if (r_boost) {
            runner_act.thrust = THRUST_BOOST; mbu_[rp] = true;
        } else {
            runner_act.thrust = std::max(0, std::min(200, out_rt[0]));
        }

        // Convert blocker output
        double b_da = std::max(-18.0, std::min(18.0, out_ba[0]));
        double b_ang_deg = fmod(pang[bp] + b_da, 360.0);
        if (b_ang_deg < 0) b_ang_deg += 360;
        double b_rad = b_ang_deg * DEG_TO_RAD;
        double btx = px[bp] + cos(b_rad) * 10000;
        double bty = py[bp] + sin(b_rad) * 10000;

        bool b_shield = out_bs == 0 && psd[bp] == 0;
        bool b_boost_flag = false;
        if (!mbu_[bp] && !b_shield && psd[bp] == 0) {
            double dd = hypot(px[bp] - px[orp_abs], py[bp] - py[orp_abs]);
            if (dd > 5000) {
                double ba_t = atan2(py[orp_abs] - py[bp], px[orp_abs] - px[bp]) * RAD_TO_DEG;
                if (ba_t < 0) ba_t += 360;
                double ae_b = ba_t - pang[bp];
                while (ae_b > 180) ae_b -= 360;
                while (ae_b < -180) ae_b += 360;
                if (fabs(ae_b) < 10) b_boost_flag = true;
            }
        }

        BotAction blocker_act;
        blocker_act.target_x = btx;
        blocker_act.target_y = bty;
        if (b_shield) {
            blocker_act.thrust = THRUST_SHIELD; msc_[bp] = 3;
        } else if (b_boost_flag) {
            blocker_act.thrust = THRUST_BOOST; mbu_[bp] = true;
        } else {
            blocker_act.thrust = std::max(0, std::min(200, out_bt[0]));
        }

        // Return in pod order (pod 0, pod 1)
        if (rp == 0) return {runner_act, blocker_act};
        else return {blocker_act, runner_act};
    }

 private:
    static constexpr int H_ = 6;
    static constexpr int PS_ = 80;

    int laps_ = 0;
    int ncp_ = 0;
    int mx_ = 0;
    int turn_ = 0;
    ultimate_ga::RNG rng_;

    std::vector<double> CX_, CY_, EX_, EY_, DTE_, RRX_, RRY_;
    double heading_to_next_[20] = {};  // atan2 of CP[i]->CP[i+1]

    // Per-game mutable state
    int ml_[2] = {}, ol_[2] = {};
    int pmc_[2] = {}, poc_[2] = {};
    int msc_[2] = {};
    bool mbu_[2] = {};

    // Previous best
    bool has_prev_ = false;
    double prev_ra_[ultimate_ga::MAX_H] = {};
    int prev_rt_[ultimate_ga::MAX_H] = {};
    double prev_ba_[ultimate_ga::MAX_H] = {};
    int prev_bt_[ultimate_ga::MAX_H] = {};
    int prev_rs_ = 6, prev_bs_ = 6;

    // IBR: Previous opponent prediction (for seeding Phase 1)
    bool has_opp_prev_ = false;
    double opp_prev_ra_[ultimate_ga::MAX_H] = {};
    int opp_prev_rt_[ultimate_ga::MAX_H] = {};
    double opp_prev_ba_[ultimate_ga::MAX_H] = {};
    int opp_prev_bt_[ultimate_ga::MAX_H] = {};

    // Observation tracking: what the opponent actually did last turn
    bool has_opp_obs_ = false;
    double prev_opp_ang_[2] = {};   // Previous angles for opp pods 2,3
    double prev_opp_x_[2] = {}, prev_opp_y_[2] = {};  // Previous positions
    double prev_opp_vx_[2] = {}, prev_opp_vy_[2] = {}; // Previous velocities
    double obs_opp_delta_[2] = {};   // Observed angle deltas (degrees)
    int obs_opp_thrust_[2] = {};     // Observed thrust estimates

    // Eval weights
    double dw_, aw_, sw_, lw_, apw_, byw_, opw_, sfw_, faw_, rw_;

    void compute_entry_points() {
        EX_ = CX_; EY_ = CY_;
        bool has_sharp = false;
        for (int i = 0; i < ncp_; i++) {
            int p = (i - 1 + ncp_) % ncp_, n = (i + 1) % ncp_;
            double v1x = CX_[i] - CX_[p], v1y = CY_[i] - CY_[p];
            double v2x = CX_[n] - CX_[i], v2y = CY_[n] - CY_[i];
            double l1 = hypot(v1x, v1y), l2 = hypot(v2x, v2y);
            double cos_theta = 1.0;
            if (l1 > 0 && l2 > 0) {
                cos_theta = std::max(-1.0, std::min(1.0, (v1x * v2x + v1y * v2y) / (l1 * l2)));
            }
            if (cos_theta < 0.2) has_sharp = true;
            has_sharp_ = has_sharp;
            double corner_cut = (ncp_ >= 5 || cos_theta < 0.2) ? 600.0 : 300.0;
            double shift_dist = corner_cut * (1.0 - cos_theta) / 2.0;
            double dx = CX_[p] - CX_[n], dy = CY_[p] - CY_[n];
            double dd = hypot(dx, dy);
            if (dd > 0) {
                EX_[i] = CX_[i] + shift_dist * dx / dd;
                EY_[i] = CY_[i] + shift_dist * dy / dd;
            }
        }
    }

    void compute_dte() {
        mx_ = laps_ * ncp_ + 1;
        DTE_.assign(mx_, 0.0);
        for (int i = mx_ - 2; i >= 0; i--) {
            int c = i % ncp_, nc2 = (c + 1) % ncp_;
            DTE_[i] = DTE_[i + 1] + hypot(CX_[c] - CX_[nc2], CY_[c] - CY_[nc2]);
        }
    }

    void compute_ram_rest_points() {
        RRX_.resize(ncp_); RRY_.resize(ncp_);
        for (int i = 0; i < ncp_; i++) {
            int p = (i - 1 + ncp_) % ncp_, n = (i + 1) % ncp_;
            double dx = CX_[p] + CX_[n] - 2 * CX_[i];
            double dy = CY_[p] + CY_[n] - 2 * CY_[i];
            double d = hypot(dx, dy);
            if (d > 0) {
                RRX_[i] = CX_[i] + 1000 * dx / d;
                RRY_[i] = CY_[i] + 1000 * dy / d;
            } else {
                RRX_[i] = CX_[i]; RRY_[i] = CY_[i];
            }
        }
    }

    bool has_sharp_ = false;

    void compute_weights() {
        double total_dist = 0;
        for (int i = 0; i < ncp_; i++) {
            int n = (i + 1) % ncp_;
            total_dist += hypot(CX_[i] - CX_[n], CY_[i] - CY_[n]);
        }
        double avg_dist = total_dist / ncp_;
        bool use_handling = (ncp_ >= 5 || has_sharp_) && avg_dist <= 6500.0;

        if (use_handling) {
            dw_ = 2.9; aw_ = 3.7; sw_ = 0.2; lw_ = 1.0; apw_ = 60.0;
            byw_ = 25.0; opw_ = 1.3; sfw_ = 35.0; faw_ = 17.5; rw_ = 0.045;
        } else {
            dw_ = 2.3; aw_ = 1.7; sw_ = 0.6; lw_ = 1.2; apw_ = 43.0;
            byw_ = 15.0; opw_ = 1.2; sfw_ = 25.0; faw_ = 12.5; rw_ = 0.045;
        }
    }
};

}  // namespace arena

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int laps;
    std::cin >> laps; std::cin.ignore();
    int checkpoint_count;
    std::cin >> checkpoint_count; std::cin.ignore();

    std::vector<std::pair<double, double>> checkpoints(checkpoint_count);
    for (int i = 0; i < checkpoint_count; i++) {
        int cx, cy;
        std::cin >> cx >> cy; std::cin.ignore();
        checkpoints[i] = {(double)cx, (double)cy};
    }

    arena::UltimateBot bot;
    bot.init(laps, checkpoints);

    while (true) {
        std::array<std::array<double, 6>, 4> pods;
        for (int i = 0; i < 2; i++) {
            int x, y, vx, vy, angle, next_cp_id;
            std::cin >> x >> y >> vx >> vy >> angle >> next_cp_id; std::cin.ignore();
            pods[i] = {(double)x, (double)y, (double)vx, (double)vy,
                       (double)angle, (double)next_cp_id};
        }
        for (int i = 0; i < 2; i++) {
            int x, y, vx, vy, angle, next_cp_id;
            std::cin >> x >> y >> vx >> vy >> angle >> next_cp_id; std::cin.ignore();
            pods[i + 2] = {(double)x, (double)y, (double)vx, (double)vy,
                           (double)angle, (double)next_cp_id};
        }

        auto [act0, act1] = bot.get_actions(pods);

        std::cout << (int)act0.target_x << " " << (int)act0.target_y << " "
                  << act0.thrust_str() << std::endl;
        std::cout << (int)act1.target_x << " " << (int)act1.target_y << " "
                  << act1.thrust_str() << std::endl;
    }
    return 0;
}

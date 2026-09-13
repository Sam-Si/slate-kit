#pragma GCC optimize("O3,unroll-loops,inline,omit-frame-pointer")
#pragma GCC target("avx2,fma,bmi,bmi2")
#include <immintrin.h>

#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace inoryy_ga {

constexpr int POD_COUNT = 4;
constexpr double POD_RADIUS = 400.0;
constexpr double CHECKPOINT_RADIUS = 600.0;
constexpr double POD_DIAMETER = 2.0 * POD_RADIUS;
constexpr double POD_RSQ = POD_DIAMETER * POD_DIAMETER;
constexpr double CP_RSQ = 360000.0;  // 600^2
constexpr double MIN_IMPULSE = 120.0;
constexpr double FRICTION = 0.85;
constexpr int MAX_THRUST = 200;
constexpr int BOOST_THRUST = 650;
constexpr int SHIELD_ACTIVE_TIMER = 4;
constexpr int TIMEOUT_INITIAL = 100;
constexpr int TIMEOUT_RESET = 101;
constexpr int DEFAULT_LAPS = 3;
constexpr double NO_COLLISION = 10.0;
constexpr double PI = 3.14159265358979323846;
constexpr double TWO_PI = 2.0 * PI;
constexpr double INV_TWO_PI = 0.15915494309189534561;
constexpr double DEG_TO_RAD = PI / 180.0;
constexpr double RAD_TO_DEG = 180.0 / PI;
constexpr double MAX_ROTATE = 18.0 * DEG_TO_RAD;
constexpr double EPSILON = 0.00001;
constexpr double NORMAL_INV_MASS = 1.0;
constexpr double SHIELD_INV_MASS = 0.1;
constexpr double SNAP_COS = -0.28;
constexpr double SNAP_SIN = 0.96;
constexpr double SNAP_TOLERANCE = 5e-16;

constexpr int GA_HORIZON = 7;
constexpr int GA_POP_SIZE = 80;
constexpr int OPP_POP_SIZE = 32;
constexpr int FIRST_TURN_BUDGET_US = 960000;
constexpr int FIRST_TURN_HARD_US = 985000;
constexpr int TARGET_TURN_BUDGET_US = 74200;  // Total turn target: 74.20 ms
constexpr int HARD_TURN_BUDGET_US = 74700;    // Hard ceiling: 74.70 ms
constexpr int GA_TARGET_DEADLINE_US =
    74000;  // GA search exits at 74.00 ms elapsed
constexpr int IBR_BUDGET_US = 2200;
constexpr int MCTS_BUDGET_US = 1000;
constexpr int STALL_GUARD_TURNS = 80;

constexpr double WIN_BONUS = 1e9;
constexpr double CP_CROSS_BONUS = 15000.0;
constexpr double ACTIVATION_PENALTY = 1000.0;
constexpr double PROGRESS_DIFF_WEIGHT = 10000.0;
constexpr double BLOCKER_DIST_THRESHOLD = 2000.0;
constexpr double BLOCKER_PROX_CLOSE = 800.0;
constexpr double BLOCKER_PROX_MED = 2000.0;
constexpr double BLOCKER_PROX_FAR = 5000.0;
constexpr double BLOCKER_PROX_FAR_SQ = 25000000.0;
constexpr double INV_BLOCKER_PROX_FAR = 1.0 / 5000.0;
constexpr double BLOCKER_PROX_WEIGHT = 5.0;
constexpr double BLOCKER_PERSTEP_PROX_WEIGHT = 0.5;
constexpr double BLOCKER_INTERPOSE_WEIGHT = 2.0;
constexpr double BLOCKER_SPEED_LOSS_WEIGHT = 20.0;
constexpr double BLOCKER_MISALIGN_WEIGHT = 0.4;

constexpr int TRIG_LUT_SIZE = 361;
struct TrigLUT {
  double rad_val[TRIG_LUT_SIZE];
  constexpr TrigLUT() : rad_val{} {
    for (int i = 0; i < TRIG_LUT_SIZE; i++) {
      double deg = (i - 180) * 0.1;
      rad_val[i] = deg * DEG_TO_RAD;
    }
  }
};
constexpr TrigLUT g_trig_lut{};

inline double fast_atan2(double y, double x) {
  if (x == 0.0 && y == 0.0) return 0.0;
  double ax = std::abs(x), ay = std::abs(y);
  double c = (ax > ay) ? ay / ax : ax / ay;
  double c2 = c * c;
  double a =
      c * (0.999866 + c2 * (-0.3302995 + c2 * (0.180141 - c2 * 0.085133)));
  if (ax <= ay) a = (PI / 2.0) - a;
  if (x < 0.0) a = PI - a;
  return (y < 0.0) ? -a : a;
}

struct SinCos {
  double s, c;
};
inline SinCos fast_sincos(double x) {
  double k = __builtin_rint(x * INV_TWO_PI);
  x -= k * TWO_PI;
  double x2 = x * x;
  double s = x * (1.0 + x2 * (-0.1666666664 +
                              x2 * (0.0083333315 +
                                    x2 * (-0.0001984090 + x2 * 0.0000027526))));
  double c =
      1.0 + x2 * (-0.5 + x2 * (0.0416666664 +
                               x2 * (-0.0013888884 +
                                     x2 * (0.0000248010 - x2 * 0.0000002752))));
  return {s, c};
}

struct Vec2 {
  double x = 0.0;
  double y = 0.0;
  inline double norm() const { return std::sqrt(x * x + y * y); }
  inline double norm2() const { return x * x + y * y; }
  inline double dot(const Vec2& o) const { return x * o.x + y * o.y; }
  inline double dist(const Vec2& o) const {
    double dx = x - o.x, dy = y - o.y;
    return std::sqrt(dx * dx + dy * dy);
  }
  inline double dist2(const Vec2& o) const {
    double dx = x - o.x, dy = y - o.y;
    return dx * dx + dy * dy;
  }
};

struct Pod {
  Vec2 pos;
  Vec2 vel;
  double angle = 0.0;
  int next = 0;
  int shieldtimer = 0;
  int boosted = 0;
  bool won = false;
  bool isFirstTurn = false;

  inline void applyRotate(double target_angle) {
    double delta = target_angle - angle;
    double k = __builtin_rint(delta * INV_TWO_PI);
    double da = delta - k * TWO_PI;
    if (da <= -MAX_ROTATE)
      angle -= MAX_ROTATE;
    else if (da >= MAX_ROTATE)
      angle += MAX_ROTATE;
    else
      angle = target_angle;
  }

  inline void applyRotateFirst(double target_angle) { angle = target_angle; }

  inline void applyThrust(int thrust) {
    if (thrust == 0) return;
    SinCos sc = fast_sincos(angle);
    double cc = sc.c, cs = sc.s;
    if (std::abs(cc - SNAP_COS) < SNAP_TOLERANCE &&
        std::abs(std::abs(cs) - SNAP_SIN) < SNAP_TOLERANCE) {
      cc = SNAP_COS;
      cs = (cs > 0 ? SNAP_SIN : -SNAP_SIN);
    }
    vel.x += cc * thrust;
    vel.y += cs * thrust;
  }

  inline void applyActionDelta(double angle_delta_deg, int thrust, int shield,
                               int boost) {
    if (__builtin_expect(shield, 0)) {
      shieldtimer = SHIELD_ACTIVE_TIMER;
      if (__builtin_expect(isFirstTurn, 0)) isFirstTurn = false;
      return;
    }
    int idx = static_cast<int>(angle_delta_deg * 10.0 + 180.5);
    if (__builtin_expect(static_cast<unsigned>(idx) > 360U, 0)) {
      idx = (idx < 0) ? 0 : 360;
    }
    if (__builtin_expect(isFirstTurn, 0)) isFirstTurn = false;
    angle += g_trig_lut.rad_val[idx];
    if (__builtin_expect(shieldtimer == 0, 1)) {
      int t_val = thrust;
      if (__builtin_expect(boost && boosted == 0, 0)) {
        t_val = BOOST_THRUST;
        boosted = 1;
      } else {
        if (t_val > MAX_THRUST)
          t_val = MAX_THRUST;
        else if (t_val < 0)
          t_val = 0;
      }
      if (t_val > 0) applyThrust(t_val);
    }
  }

  inline void endTurn() {
    vel.x = std::trunc(vel.x * FRICTION);
    vel.y = std::trunc(vel.y * FRICTION);
    pos.x = std::floor(pos.x + 0.5);
    pos.y = std::floor(pos.y + 0.5);
    if (__builtin_expect(shieldtimer > 0, 0)) shieldtimer--;
  }

  inline double newCollide(const Pod& other, double rsq, double first) const {
    double dx = other.pos.x - pos.x;
    if (std::abs(dx) > 2300.0) return NO_COLLISION;
    double dy = other.pos.y - pos.y;
    if (std::abs(dy) > 2300.0) return NO_COLLISION;
    double pLength2 = dx * dx + dy * dy;
    if (pLength2 <= rsq) return 0.0;
    if (pLength2 > 5290000.0) return NO_COLLISION;
    Vec2 rel_p = {dx, dy};

    Vec2 rel_vel = {other.vel.x - vel.x, other.vel.y - vel.y};
    double dot_val = rel_p.dot(rel_vel);
    if (dot_val > 0.0) return NO_COLLISION;

    double vLength2 = rel_vel.norm2();
    if (vLength2 == 0.0) return NO_COLLISION;

    double disc = dot_val * dot_val - vLength2 * (pLength2 - rsq);
    if (disc <= 0.0) return NO_COLLISION;

    double req = -dot_val - first * vLength2;
    if (req >= 0.0 && req * req > disc) return NO_COLLISION;

    double discdist = std::sqrt(disc);
    return (-dot_val - discdist) / vLength2;
  }

  inline void passCheckpoint(int podn, int totalRaceCps, int* playerTimeout) {
    next = next + 1;
    if (__builtin_expect(next >= totalRaceCps, 0)) {
      next = totalRaceCps - 1;
      won = true;
    }
    if (podn < 2)
      playerTimeout[0] = TIMEOUT_RESET;
    else
      playerTimeout[1] = TIMEOUT_RESET;
  }
};

inline bool cpCollide(Vec2 p1, Vec2 p2, Vec2 cp, double cp_rsq) {
  double min_x = (p1.x < p2.x ? p1.x : p2.x) - 600.0;
  if (cp.x < min_x) return false;
  double max_x = (p1.x > p2.x ? p1.x : p2.x) + 600.0;
  if (cp.x > max_x) return false;
  double min_y = (p1.y < p2.y ? p1.y : p2.y) - 600.0;
  if (cp.y < min_y) return false;
  double max_y = (p1.y > p2.y ? p1.y : p2.y) + 600.0;
  if (cp.y > max_y) return false;
  double dx = p2.x - p1.x;
  double dy = p2.y - p1.y;
  Vec2 pp = p1;
  double pd2 = dx * dx + dy * dy;
  if (pd2 != 0.0) {
    double u = ((cp.x - p1.x) * dx + (cp.y - p1.y) * dy) / pd2;
    if (u > 1.0)
      pp = p2;
    else if (u > 0.0) {
      pp.x = p1.x + u * dx;
      pp.y = p1.y + u * dy;
    }
  }
  pp.x -= cp.x;
  pp.y -= cp.y;
  return (pp.x * pp.x + pp.y * pp.y) < cp_rsq;
}

struct alignas(64) SimGame {
  Pod pods[POD_COUNT];
  const Vec2* globalCp = nullptr;
  int globalCpSize = 0;
  int ncp = 0;
  int totalRaceCps = 0;
  int playerTimeout[2] = {TIMEOUT_INITIAL, TIMEOUT_INITIAL};

  inline void copyFrom(const SimGame& o) {
    const double* src = reinterpret_cast<const double*>(&o);
    double* dst = reinterpret_cast<double*>(this);
#pragma GCC unroll 8
    for (int i = 0; i < 32; i += 4) {
      __m256d v = _mm256_load_pd(src + i);
      _mm256_store_pd(dst + i, v);
    }
  }

  inline void setPodState(int idx, double x, double y, double vx, double vy,
                          double angle_rad, int next_cp, int shield_timer,
                          int has_boosted) {
    if (idx < 0 || idx >= POD_COUNT) return;
    Pod& pod = pods[idx];
    pod.pos = {x, y};
    pod.vel = {vx, vy};
    pod.angle = angle_rad;
    pod.next = next_cp;
    pod.shieldtimer = shield_timer;
    pod.boosted = has_boosted;
    pod.won = false;
    pod.isFirstTurn = (std::abs(angle_rad + 0.0174533) < 0.001) ||
                      (std::abs(angle_rad) < 0.001);
  }

  inline void applyAction(int pod_idx, int target_x, int target_y,
                          int thrust_val) {
    if (pod_idx < 0 || pod_idx >= POD_COUNT) return;
    Pod& pod = pods[pod_idx];
    double dx = static_cast<double>(target_x) - pod.pos.x;
    double dy = static_cast<double>(target_y) - pod.pos.y;
    if (dx != 0.0 || dy != 0.0) {
      double target_angle = fast_atan2(dy, dx);
      if (__builtin_expect(pod.isFirstTurn, 0)) {
        pod.isFirstTurn = false;
        pod.applyRotateFirst(target_angle);
      } else
        pod.applyRotate(target_angle);
    }
    int thrust = (pod.shieldtimer > 0 || (dx == 0.0 && dy == 0.0))
                     ? 0
                     : std::clamp(thrust_val, 0, MAX_THRUST);
    if (thrust > 0) pod.applyThrust(thrust);
  }

  inline void applyAction(int pod_idx, int target_x, int target_y,
                          std::string_view thrust_str) {
    if (pod_idx < 0 || pod_idx >= POD_COUNT) return;
    Pod& pod = pods[pod_idx];
    Vec2 target = {static_cast<double>(target_x),
                   static_cast<double>(target_y)};
    bool is_shield = false, is_boost = false;
    int val = 0;
    bool is_num = false;

    if (thrust_str == "200") {
      val = 200;
      is_num = true;
    } else if (thrust_str == "0") {
      val = 0;
      is_num = true;
    } else if (!thrust_str.empty()) {
      if (thrust_str[0] == 'S')
        is_shield = true;
      else if (thrust_str[0] == 'B')
        is_boost = true;
      else {
        is_num = true;
        bool neg = (thrust_str[0] == '-');
        size_t i = neg ? 1 : 0;
        int v = 0;
        for (; i < thrust_str.size(); ++i) {
          if (thrust_str[i] >= '0' && thrust_str[i] <= '9')
            v = v * 10 + (thrust_str[i] - '0');
          else
            break;
        }
        val = neg ? -v : v;
      }
    }

    if (is_num) {
      if (val < 0) {
        pod.shieldtimer = SHIELD_ACTIVE_TIMER;
        if (pod.isFirstTurn) pod.isFirstTurn = false;
        return;
      }
      if (val > MAX_THRUST) {
        if (pod.isFirstTurn) pod.isFirstTurn = false;
        return;
      }
    }

    double dx = target.x - pod.pos.x;
    double dy = target.y - pod.pos.y;
    if (dx != 0.0 || dy != 0.0) {
      double target_angle = fast_atan2(dy, dx);
      if (__builtin_expect(pod.isFirstTurn, 0)) {
        pod.isFirstTurn = false;
        pod.applyRotateFirst(target_angle);
      } else
        pod.applyRotate(target_angle);
    }

    int thrust = 0;
    bool used_shield = false;
    if (is_shield) {
      pod.shieldtimer = SHIELD_ACTIVE_TIMER;
      used_shield = true;
      thrust = 0;
    } else if (is_boost) {
      if (pod.boosted == 0) {
        thrust = BOOST_THRUST;
        pod.boosted = 1;
      } else
        thrust = MAX_THRUST;
    } else if (is_num) {
      thrust = std::clamp(val, 0, MAX_THRUST);
    }

    if (!used_shield && pod.shieldtimer > 0) thrust = 0;
    if (dx == 0.0 && dy == 0.0) thrust = 0;
    if (thrust > 0) pod.applyThrust(thrust);
  }

  inline void forwardTime(double t) {
    for (int i = 0; i < POD_COUNT; ++i) {
      pods[i].pos.x += pods[i].vel.x * t;
      pods[i].pos.y += pods[i].vel.y * t;
    }
  }

  inline void bounce(int idx_a, int idx_b) {
    Pod& pa = pods[idx_a];
    Pod& pb = pods[idx_b];
    Vec2 normal = {pb.pos.x - pa.pos.x, pb.pos.y - pa.pos.y};
    double dist = normal.norm();
    double inv_dist = 1.0 / dist;
    normal.x *= inv_dist;
    normal.y *= inv_dist;

    Vec2 relv = {pa.vel.x - pb.vel.x, pa.vel.y - pb.vel.y};
    double m1 = (pa.shieldtimer == SHIELD_ACTIVE_TIMER) ? SHIELD_INV_MASS
                                                        : NORMAL_INV_MASS;
    double m2 = (pb.shieldtimer == SHIELD_ACTIVE_TIMER) ? SHIELD_INV_MASS
                                                        : NORMAL_INV_MASS;
    double force = normal.dot(relv) / (m1 + m2);
    if (force < MIN_IMPULSE)
      force += MIN_IMPULSE;
    else
      force += force;

    double ix = normal.x * -force;
    double iy = normal.y * -force;
    pa.vel.x += ix * m1;
    pa.vel.y += iy * m1;
    pb.vel.x -= ix * m2;
    pb.vel.y -= iy * m2;

    if (dist <= POD_DIAMETER) {
      double push = (POD_DIAMETER - dist) * 0.5 + EPSILON;
      double px = normal.x * push;
      double py = normal.y * push;
      pa.pos.x -= px;
      pa.pos.y -= py;
      pb.pos.x += px;
      pb.pos.y += py;
    }
  }

  inline void nextTurn(int skip_rp = -1, int keep_bp = -1) {
    double remaining = 1.0;
    Vec2 curps[POD_COUNT] = {pods[0].pos, pods[1].pos, pods[2].pos,
                             pods[3].pos};

    auto ignore_pair = [skip_rp, keep_bp](int a, int b) -> bool {
      if (skip_rp < 0) return false;
      if (a == skip_rp) return b != keep_bp;
      if (b == skip_rp) return a != keep_bp;
      return false;
    };

    if (__builtin_expect(skip_rp < 0, 1)) {
      while (remaining > 0.0) {
        double first = remaining;
        int col_i = 0, col_j = 0;
        double tx;

        tx = pods[3].newCollide(pods[2], POD_RSQ, first);
        if (tx <= first) {
          first = tx;
          col_i = 3;
          col_j = 2;
        }
        tx = pods[3].newCollide(pods[1], POD_RSQ, first);
        if (tx <= first) {
          first = tx;
          col_i = 3;
          col_j = 1;
        }
        tx = pods[3].newCollide(pods[0], POD_RSQ, first);
        if (tx <= first) {
          first = tx;
          col_i = 3;
          col_j = 0;
        }
        tx = pods[2].newCollide(pods[1], POD_RSQ, first);
        if (tx <= first) {
          first = tx;
          col_i = 2;
          col_j = 1;
        }
        tx = pods[2].newCollide(pods[0], POD_RSQ, first);
        if (tx <= first) {
          first = tx;
          col_i = 2;
          col_j = 0;
        }
        tx = pods[1].newCollide(pods[0], POD_RSQ, first);
        if (tx <= first) {
          first = tx;
          col_i = 1;
          col_j = 0;
        }

        forwardTime(first);
        if (__builtin_expect(col_i == col_j, 1)) break;
        remaining -= first;
        bounce(col_i, col_j);

        if (cpCollide(curps[col_i], pods[col_i].pos, globalCp[pods[col_i].next],
                      CP_RSQ)) {
          pods[col_i].passCheckpoint(col_i, totalRaceCps, playerTimeout);
        }
        if (cpCollide(curps[col_j], pods[col_j].pos, globalCp[pods[col_j].next],
                      CP_RSQ)) {
          pods[col_j].passCheckpoint(col_j, totalRaceCps, playerTimeout);
        }
        curps[col_i] = pods[col_i].pos;
        curps[col_j] = pods[col_j].pos;
      }
    } else {
      while (remaining > 0.0) {
        double first = remaining;
        int col_i = 0, col_j = 0;
        double tx;

        if (!ignore_pair(3, 2)) {
          tx = pods[3].newCollide(pods[2], POD_RSQ, first);
          if (tx <= first) {
            first = tx;
            col_i = 3;
            col_j = 2;
          }
        }
        if (!ignore_pair(3, 1)) {
          tx = pods[3].newCollide(pods[1], POD_RSQ, first);
          if (tx <= first) {
            first = tx;
            col_i = 3;
            col_j = 1;
          }
        }
        if (!ignore_pair(3, 0)) {
          tx = pods[3].newCollide(pods[0], POD_RSQ, first);
          if (tx <= first) {
            first = tx;
            col_i = 3;
            col_j = 0;
          }
        }
        if (!ignore_pair(2, 1)) {
          tx = pods[2].newCollide(pods[1], POD_RSQ, first);
          if (tx <= first) {
            first = tx;
            col_i = 2;
            col_j = 1;
          }
        }
        if (!ignore_pair(2, 0)) {
          tx = pods[2].newCollide(pods[0], POD_RSQ, first);
          if (tx <= first) {
            first = tx;
            col_i = 2;
            col_j = 0;
          }
        }
        if (!ignore_pair(1, 0)) {
          tx = pods[1].newCollide(pods[0], POD_RSQ, first);
          if (tx <= first) {
            first = tx;
            col_i = 1;
            col_j = 0;
          }
        }

        forwardTime(first);
        if (__builtin_expect(col_i == col_j, 1)) break;
        remaining -= first;
        bounce(col_i, col_j);

        if (cpCollide(curps[col_i], pods[col_i].pos, globalCp[pods[col_i].next],
                      CP_RSQ)) {
          pods[col_i].passCheckpoint(col_i, totalRaceCps, playerTimeout);
        }
        if (cpCollide(curps[col_j], pods[col_j].pos, globalCp[pods[col_j].next],
                      CP_RSQ)) {
          pods[col_j].passCheckpoint(col_j, totalRaceCps, playerTimeout);
        }
        curps[col_i] = pods[col_i].pos;
        curps[col_j] = pods[col_j].pos;
      }
    }

    playerTimeout[0]--;
    playerTimeout[1]--;

#pragma GCC unroll 4
    for (int i = 0; i < POD_COUNT; ++i) {
      pods[i].endTurn();
      if (cpCollide(curps[i], pods[i].pos, globalCp[pods[i].next], CP_RSQ)) {
        pods[i].passCheckpoint(i, totalRaceCps, playerTimeout);
      } else if (__builtin_expect(!pods[i].won, 1)) {
        double d_sq = pods[i].pos.dist2(globalCp[pods[i].next]);
        if (__builtin_expect(d_sq == CP_RSQ, 0))
          pods[i].passCheckpoint(i, totalRaceCps, playerTimeout);
      }
    }
  }
};

struct RNG {
  uint32_t s = 42;
  uint64_t s64 = 88172645463325252ULL;

  inline uint32_t u32() {
    s = (214013u * s + 2531011u);
    return s;
  }
  inline uint64_t u64() {
    s64 ^= s64 >> 12;
    s64 ^= s64 << 25;
    s64 ^= s64 >> 27;
    return s64 * 0x2545F4914F6CDD1DULL;
  }
  inline int ri(int a, int b) {
    if (b <= a) return a;
    uint32_t range = static_cast<uint32_t>(b - a + 1);
    return a + static_cast<int>((static_cast<uint64_t>(u32()) * range) >> 32);
  }
  inline double rd() { return (u32() >> 8) * (1.0 / 16777216.0); }
  inline void seed(uint32_t v) {
    s = v;
    s64 = (static_cast<uint64_t>(v) << 32) ^ 88172645463325252ULL;
  }
};

inline int rand_thrust(RNG& rng) {
  int raw = rng.ri(-100, 500);
  if (raw < 0) return 0;
  if (raw > 200) return 200;
  return raw;
}

struct alignas(64) Ind {
  double ra[GA_HORIZON];
  int rt[GA_HORIZON];
  double ba[GA_HORIZON];
  int bt[GA_HORIZON];
  int rs;
  int bs;
  double sc;

  Ind() {}
  Ind(const Ind&) = default;
  Ind& operator=(const Ind&) = default;

  inline void copyFrom(const Ind& o) {
    const double* src = reinterpret_cast<const double*>(&o);
    double* dst = reinterpret_cast<double*>(this);
#pragma GCC unroll 6
    for (int i = 0; i < 24; i += 4) {
      __m256d v = _mm256_load_pd(src + i);
      _mm256_store_pd(dst + i, v);
    }
  }
};

struct EvalContext {
  int ncp = 0;
  int laps = 0;
  int mx = 0;
  int H = GA_HORIZON;
  const double* EX = nullptr;
  const double* EY = nullptr;
  const double* DTE = nullptr;
  const double* RRX = nullptr;
  const double* RRY = nullptr;
  int rp = 0;
  int bp = 1;
  int orp = 2;
  int obp = 3;
  int rb = 0;
  int fboost = 0;
  int rtimeout = 0;
  int is_runner_pinned = 0;
  double dw = 2.3, aw = 1.7, sw = 0.6, lw = 1.2, apw = 43.0;
  double byw = 15.0, opw = 1.2, sfw = 40.0, faw = 20.0, rw = 0.15;
  const double* opp_ra = nullptr;
  const int* opp_rt = nullptr;
  const double* opp_ba = nullptr;
  const int* opp_bt = nullptr;
  int racing_only = 0;
  const double* out_dx = nullptr;
  const double* out_dy = nullptr;
  const double* out_ux = nullptr;
  const double* out_uy = nullptr;
  const double* out_len = nullptr;
  double opp_init_speed = 0.0;
};

inline double evaluate_opp_perspective(const Ind* ind, const SimGame& base_game,
                                       const EvalContext& ctx,
                                       const double* our_ra, const int* our_rt,
                                       const double* our_ba,
                                       const int* our_bt) {
  constexpr int H = GA_HORIZON;
  SimGame sim;
  sim.copyFrom(base_game);
  int orp = ctx.orp;
  int obp = ctx.obp;
  int rp = ctx.rp;
  int bp = ctx.bp;
  int ior_cp = sim.pods[orp].next;
  int ir_cp = sim.pods[rp].next;

  for (int t = 0; t < H; t++) {
    sim.pods[rp].applyActionDelta(our_ra[t], our_rt[t], 0, 0);
    sim.pods[bp].applyActionDelta(our_ba[t], our_bt[t], 0, 0);
    sim.pods[orp].applyActionDelta(ind->ra[t], ind->rt[t], 0, 0);
    sim.pods[obp].applyActionDelta(ind->ba[t], ind->bt[t], 0, 0);
    sim.nextTurn();
  }

  if (__builtin_expect(
          sim.pods[orp].pos.x < -2000.0 || sim.pods[orp].pos.x > 18000.0 ||
              sim.pods[orp].pos.y < -2000.0 || sim.pods[orp].pos.y > 11000.0,
          0))
    return -1e9;

  double s = 0.0;
  if (__builtin_expect(sim.pods[orp].won || sim.pods[obp].won, 0))
    s += WIN_BONUS;
  if (__builtin_expect(sim.pods[rp].won || sim.pods[bp].won, 0)) s -= WIN_BONUS;

  int crossed = sim.pods[orp].next - ior_cp;
  if (crossed > 0) s += crossed * CP_CROSS_BONUS;

  if (!sim.pods[orp].won) {
    int rl = sim.pods[orp].next;
    if (rl < ctx.mx) {
      double dx = sim.pods[orp].pos.x - ctx.EX[sim.pods[orp].next % ctx.ncp];
      double dy = sim.pods[orp].pos.y - ctx.EY[sim.pods[orp].next % ctx.ncp];
      s -= ctx.dw * (ctx.DTE[rl] + std::sqrt(dx * dx + dy * dy));
    }
  }

  {
    double dx = ctx.EX[sim.pods[orp].next % ctx.ncp] - sim.pods[orp].pos.x;
    double dy = ctx.EY[sim.pods[orp].next % ctx.ncp] - sim.pods[orp].pos.y;
    double d = std::sqrt(dx * dx + dy * dy);
    if (d > 0.0) {
      double inv_d = 1.0 / d;
      s += (sim.pods[orp].vel.x * dx * inv_d +
            sim.pods[orp].vel.y * dy * inv_d) *
           ctx.aw;
    }
    s += sim.pods[orp].vel.norm() * ctx.sw;
  }

  {
    int opp_progress = sim.pods[orp].next;
    int our_progress = sim.pods[rp].next;
    int diff = opp_progress - our_progress;
    s += diff * PROGRESS_DIFF_WEIGHT;

    double opp_dx = sim.pods[orp].pos.x - sim.globalCp[sim.pods[orp].next].x;
    double opp_dy = sim.pods[orp].pos.y - sim.globalCp[sim.pods[orp].next].y;
    double our_dx = sim.pods[rp].pos.x - sim.globalCp[sim.pods[rp].next].x;
    double our_dy = sim.pods[rp].pos.y - sim.globalCp[sim.pods[rp].next].y;
    double opp_dist = std::sqrt(opp_dx * opp_dx + opp_dy * opp_dy);
    double our_dist = std::sqrt(our_dx * our_dx + our_dy * our_dy);
    double opp_rem =
        ((sim.pods[orp].next < ctx.mx) ? ctx.DTE[sim.pods[orp].next] : 0.0) +
        opp_dist;
    double our_rem =
        ((sim.pods[rp].next < ctx.mx) ? ctx.DTE[sim.pods[rp].next] : 0.0) +
        our_dist;
    double opp_lead = our_rem - opp_rem;
    s += opp_lead * 0.60;
  }

  {
    double d_bp = sim.pods[orp].pos.dist2(sim.pods[bp].pos);
    if (d_bp < 810000.0) {
      s -= 80.0 * (1.0 - d_bp * (1.0 / 810000.0));
    }
  }

  {
    int our_crossed = sim.pods[rp].next - ir_cp;
    if (our_crossed > 0) s -= our_crossed * 10000.0;

    int target_cp = sim.pods[rp].next;
    double cx = sim.globalCp[target_cp].x - sim.pods[rp].pos.x;
    double cy = sim.globalCp[target_cp].y - sim.pods[rp].pos.y;
    double c_d2 = cx * cx + cy * cy;

    double bx = sim.pods[obp].pos.x - sim.pods[rp].pos.x;
    double by = sim.pods[obp].pos.y - sim.pods[rp].pos.y;
    double b_d2 = bx * bx + by * by;
    double b_dist = std::sqrt(b_d2);

    if (c_d2 > 1.0 && b_d2 > 1.0) {
      double dot = bx * cx + by * cy;
      if (dot > 0.0) {
        double cos_ray = dot / (b_dist * std::sqrt(c_d2));
        if (cos_ray > 1.0) cos_ray = 1.0;
        s += cos_ray * cos_ray * 5000.0;
      } else {
        s -= 3000.0;
      }
    }

    double ta = fast_atan2(-by, -bx);
    double ae = ta - sim.pods[obp].angle;
    double k = __builtin_rint(ae * INV_TWO_PI);
    ae -= k * TWO_PI;
    s -= 20.0 * std::abs(ae * RAD_TO_DEG);

    if (b_dist < 1200.0) {
      double dvx = sim.pods[obp].vel.x - sim.pods[rp].vel.x;
      double dvy = sim.pods[obp].vel.y - sim.pods[rp].vel.y;
      s -= std::sqrt(dvx * dvx + dvy * dvy) * 2.0;
      if (b_dist <= 850.0) s += 2500.0;
    }
    if (b_dist < BLOCKER_PROX_FAR) {
      constexpr double INV_FAR_CLOSE =
          1.0 / (BLOCKER_PROX_FAR - BLOCKER_PROX_CLOSE);
      double norm = (BLOCKER_PROX_FAR - b_dist) * INV_FAR_CLOSE;
      if (norm > 1.0) norm = 1.0;
      s += norm * 1000.0;
    }
  }

  return s;
}

inline void run_opp_prediction_ga(RNG& rng, const SimGame& base_game,
                                  const EvalContext& ctx, const double* our_ra,
                                  const int* our_rt, const double* our_ba,
                                  const int* our_bt, const double* opp_prev_ra,
                                  const int* opp_prev_rt,
                                  const double* opp_prev_ba,
                                  const int* opp_prev_bt, int has_opp_prev,
                                  int budget_us, double* out_ora, int* out_ort,
                                  double* out_oba, int* out_obt) {
  constexpr int H = GA_HORIZON;
  constexpr int PS = OPP_POP_SIZE;
  alignas(64) Ind pop[PS];
  alignas(64) double scores[PS];
  int idx = 0;
  int orp = ctx.orp;
  int obp = ctx.obp;
  int rp = ctx.rp;
  int bp = ctx.bp;

  if (has_opp_prev && opp_prev_ra && opp_prev_rt && opp_prev_ba &&
      opp_prev_bt) {
    for (int t = 0; t < H - 1; t++) {
      pop[0].ra[t] = opp_prev_ra[t + 1];
      pop[0].rt[t] = opp_prev_rt[t + 1];
      pop[0].ba[t] = opp_prev_ba[t + 1];
      pop[0].bt[t] = opp_prev_bt[t + 1];
    }
    pop[0].ra[H - 1] = rng.rd() * 36.0 - 18.0;
    pop[0].rt[H - 1] = rand_thrust(rng);
    pop[0].ba[H - 1] = rng.rd() * 36.0 - 18.0;
    pop[0].bt[H - 1] = rand_thrust(rng);
    pop[0].rs = H;
    pop[0].bs = H;
    idx = 1;

    for (int pv = 0; pv < 3 && idx < PS; pv++) {
      pop[idx] = pop[0];
      for (int t = 0; t < H; t++) {
        double da = pop[idx].ra[t] + rng.rd() * 10.0 - 5.0;
        pop[idx].ra[t] = std::clamp(da, -18.0, 18.0);
        int th = pop[idx].rt[t] + rng.ri(-30, 30);
        pop[idx].rt[t] = std::clamp(th, 0, 200);

        double bda = pop[idx].ba[t] + rng.rd() * 10.0 - 5.0;
        pop[idx].ba[t] = std::clamp(bda, -18.0, 18.0);
        int bth = pop[idx].bt[t] + rng.ri(-30, 30);
        pop[idx].bt[t] = std::clamp(bth, 0, 200);
      }
      idx++;
    }
  }

  // 2. Tactical Heuristic Seeds: Multi-archetype from 24,246 real replay turns
  for (int mode = 0; mode < 4 && idx < PS; mode++) {
    SimGame hsim;
    hsim.copyFrom(base_game);
    for (int t = 0; t < H; t++) {
      int orp_cp = hsim.pods[orp].next % ctx.ncp;
      double orp_tx = ctx.EX[orp_cp], orp_ty = ctx.EY[orp_cp];
      if (mode == 2) {
        double dx0 = orp_tx - hsim.pods[orp].pos.x,
               dy0 = orp_ty - hsim.pods[orp].pos.y;
        if (dx0 * dx0 + dy0 * dy0 < 6250000.0) {
          int nxt_cp = (hsim.pods[orp].next + 1) % ctx.ncp;
          orp_tx = 0.75 * orp_tx + 0.25 * ctx.EX[nxt_cp];
          orp_ty = 0.75 * orp_ty + 0.25 * ctx.EY[nxt_cp];
        }
      }
      double orp_ta = fast_atan2(orp_ty - hsim.pods[orp].pos.y,
                                 orp_tx - hsim.pods[orp].pos.x);
      double diff_a =
          (orp_ta - hsim.pods[orp].angle -
           __builtin_rint((orp_ta - hsim.pods[orp].angle) * INV_TWO_PI) *
               TWO_PI) *
          RAD_TO_DEG;
      double orp_da = std::clamp(diff_a, -18.0, 18.0);
      int orp_th = 200;
      if (mode == 1 || mode == 2) {
        if (std::abs(diff_a) > 75.0) orp_th = 0;
      } else if (mode == 3) {
        orp_th = std::clamp(static_cast<int>(200.0 - std::abs(diff_a) * 2.5), 0,
                            200);
      }

      double obp_tx, obp_ty;
      if (mode == 2) {
        int r_cp = hsim.pods[rp].next % ctx.ncp;
        obp_tx = ctx.RRX[r_cp];
        obp_ty = ctx.RRY[r_cp];
      } else {
        obp_tx = hsim.pods[rp].pos.x + hsim.pods[rp].vel.x * 2.0;
        obp_ty = hsim.pods[rp].pos.y + hsim.pods[rp].vel.y * 2.0;
      }
      double obp_ta = fast_atan2(obp_ty - hsim.pods[obp].pos.y,
                                 obp_tx - hsim.pods[obp].pos.x);
      double obp_da = std::clamp(
          (obp_ta - hsim.pods[obp].angle -
           __builtin_rint((obp_ta - hsim.pods[obp].angle) * INV_TWO_PI) *
               TWO_PI) *
              RAD_TO_DEG,
          -18.0, 18.0);

      pop[idx].ra[t] = orp_da;
      pop[idx].rt[t] = orp_th;
      pop[idx].ba[t] = obp_da;
      pop[idx].bt[t] = 200;

      hsim.pods[rp].applyActionDelta(our_ra[t], our_rt[t], 0, 0);
      hsim.pods[bp].applyActionDelta(our_ba[t], our_bt[t], 0, 0);
      hsim.pods[orp].applyActionDelta(orp_da, orp_th, 0, 0);
      hsim.pods[obp].applyActionDelta(obp_da, 200, 0, 0);
      hsim.nextTurn();
    }
    pop[idx].rs = H;
    pop[idx].bs = H;
    idx++;
  }

  if (idx < PS) {
    for (int t = 0; t < H; t++) {
      pop[idx].ra[t] = 0;
      pop[idx].rt[t] = 200;
      pop[idx].ba[t] = 0;
      pop[idx].bt[t] = 200;
    }
    pop[idx].rs = H;
    pop[idx].bs = H;
    idx++;
  }

  for (int i = idx; i < PS; i++) {
    for (int t = 0; t < H; t++) {
      int raw_a = rng.ri(-180, 180);
      pop[i].ra[t] = raw_a * 0.1;
      pop[i].rt[t] = rand_thrust(rng);
      int raw_ba = rng.ri(-180, 180);
      pop[i].ba[t] = raw_ba * 0.1;
      pop[i].bt[t] = rand_thrust(rng);
    }
    pop[i].rs = H;
    pop[i].bs = H;
  }

  int best = 0, worst = 0;
  for (int i = 0; i < PS; i++) {
    pop[i].sc = evaluate_opp_perspective(&pop[i], base_game, ctx, our_ra,
                                         our_rt, our_ba, our_bt);
    scores[i] = pop[i].sc;
    if (scores[i] > scores[best]) best = i;
    if (scores[i] < scores[worst]) worst = i;
  }
  double wsc = scores[worst];
  double best_sc = scores[best];

  struct timespec t0, tn;
  clock_gettime(CLOCK_MONOTONIC, &t0);
  int iters = PS;
  Ind child;

  while (true) {
    iters++;
    if (__builtin_expect((iters & 255) == 0, 0)) {
      clock_gettime(CLOCK_MONOTONIC, &tn);
      long el = (tn.tv_sec - t0.tv_sec) * 1000000L +
                (tn.tv_nsec - t0.tv_nsec) / 1000L;
      if (el >= budget_us) break;
    }

    uint64_t r_par = rng.u64();
    int p1 = static_cast<int>(
        (static_cast<uint32_t>(r_par) * static_cast<uint64_t>(PS)) >> 32);
    int p2 =
        static_cast<int>(((r_par >> 32) * static_cast<uint64_t>(PS)) >> 32);
    int par = (scores[p1] >= scores[p2]) ? p1 : p2;

    child.copyFrom(pop[par]);
    int si = rng.ri(0, H - 1);
    if (rng.rd() < 0.6) {
      double da = child.ra[si] + rng.rd() * 8.0 - 4.0;
      child.ra[si] = std::clamp(da, -18.0, 18.0);
      double bda = child.ba[si] + rng.rd() * 8.0 - 4.0;
      child.ba[si] = std::clamp(bda, -18.0, 18.0);
    } else {
      int raw_a = rng.ri(-180, 180);
      child.ra[si] = raw_a * 0.1;
      child.rt[si] = rand_thrust(rng);
      int raw_ba = rng.ri(-180, 180);
      child.ba[si] = raw_ba * 0.1;
      child.bt[si] = rand_thrust(rng);
    }

    child.sc = evaluate_opp_perspective(&child, base_game, ctx, our_ra, our_rt,
                                        our_ba, our_bt);
    if (child.sc > best_sc) {
      best_sc = child.sc;
      best = worst;
    }
    pop[worst].copyFrom(child);
    scores[worst] = child.sc;
    if (child.sc > wsc) {
      worst = 0;
      wsc = scores[0];
#pragma GCC unroll 8
      for (int i = 1; i < PS; i++) {
        if (scores[i] < wsc) {
          worst = i;
          wsc = scores[i];
        }
      }
    }
  }

  std::memcpy(out_ora, pop[best].ra, H * sizeof(double));
  std::memcpy(out_ort, pop[best].rt, H * sizeof(int));
  std::memcpy(out_oba, pop[best].ba, H * sizeof(double));
  std::memcpy(out_obt, pop[best].bt, H * sizeof(int));
}

struct V40ChampionFitnessEvaluator {
  static inline double evaluate(const Ind* ind, const SimGame& base_game,
                                const EvalContext& ctx) {
    constexpr int H = GA_HORIZON;
    (void)ctx.H;
    const double* EX = ctx.EX;
    const double* EY = ctx.EY;
    int ncp = ctx.ncp;
    int rp = ctx.rp;
    int bp = ctx.bp;
    int orp = ctx.orp;
    int obp = ctx.obp;
    int fboost = ctx.fboost;
    int rtimeout = ctx.rtimeout;
    const double* opp_ra = ctx.opp_ra;
    const int* opp_rt = ctx.opp_rt;
    const double* opp_ba = ctx.opp_ba;
    const int* opp_bt = ctx.opp_bt;
    int racing_only = ctx.racing_only;

    SimGame sim;
    sim.copyFrom(base_game);
    int ir_cp = sim.pods[rp].next;
    int ior_cp = sim.pods[orp].next;
    double r_act = static_cast<double>(H) + 0.3;
    double o_act = static_cast<double>(H) + 0.3;
    double blocker_prox_sum = 0.0;
    double friendly_penalty = 0.0;

    for (int t = 0; t < H; t++) {
      int rth = ind->rt[t];
      int rb_flag = (__builtin_expect(t == 0 && fboost, 0) &&
                     sim.pods[rp].shieldtimer == 0)
                        ? 1
                        : 0;
      int rs_flag =
          (!rb_flag && __builtin_expect(t == ind->rs && ind->rs < 3, 0) &&
           sim.pods[rp].shieldtimer == 0)
              ? 1
              : 0;
      sim.pods[rp].applyActionDelta(ind->ra[t], rth, rs_flag, rb_flag);

      int bth = ind->bt[t];
      int bs_flag = (__builtin_expect(t == ind->bs && ind->bs < 3, 0) &&
                     sim.pods[bp].shieldtimer == 0)
                        ? 1
                        : 0;
      sim.pods[bp].applyActionDelta(ind->ba[t], bth, bs_flag, 0);

      if (opp_ra) {
        sim.pods[orp].applyActionDelta(opp_ra[t], opp_rt[t], 0, 0);
      } else {
        sim.applyAction(orp, static_cast<int>(EX[sim.pods[orp].next % ncp]),
                        static_cast<int>(EY[sim.pods[orp].next % ncp]), 200);
      }
      if (opp_ba) {
        sim.pods[obp].applyActionDelta(opp_ba[t], opp_bt[t], 0, 0);
      } else {
        int our_target_cp = sim.pods[rp].next % ncp;
        sim.applyAction(obp, static_cast<int>(EX[our_target_cp]),
                        static_cast<int>(EY[our_target_cp]), 200);
      }

      if (__builtin_expect(racing_only, 0)) {
        sim.nextTurn(rp, bp);
      } else {
        sim.nextTurn(-1, -1);
      }

      if (__builtin_expect(
              r_act > static_cast<double>(H) && sim.pods[rp].next != ir_cp, 0))
        r_act = static_cast<double>(t) + 0.5;
      if (__builtin_expect(
              o_act > static_cast<double>(H) && sim.pods[orp].next != ior_cp,
              0))
        o_act = static_cast<double>(t) + 0.5;

      double bpx = sim.pods[bp].pos.x;
      double bpy = sim.pods[bp].pos.y;

      if (__builtin_expect(!rtimeout, 1)) {
        double bdx = bpx - sim.pods[orp].pos.x;
        double bdy = bpy - sim.pods[orp].pos.y;
        double bd2 = bdx * bdx + bdy * bdy;
        if (bd2 < BLOCKER_PROX_FAR_SQ) {
          double bd = std::sqrt(bd2);
          double norm = 1.0 - bd * INV_BLOCKER_PROX_FAR;
          blocker_prox_sum += norm * norm;
        }
      }

      double fdx = sim.pods[rp].pos.x - bpx;
      double fdy = sim.pods[rp].pos.y - bpy;
      double fd2 = fdx * fdx + fdy * fdy;
      if (__builtin_expect(fd2 < 810000.0, 0)) {
        double fvx = sim.pods[rp].vel.x - sim.pods[bp].vel.x;
        double fvy = sim.pods[rp].vel.y - sim.pods[bp].vel.y;
        double f_closing = -(fdx * fvx + fdy * fvy);
        if (f_closing > 0.0) {
          friendly_penalty += 3000.0 + f_closing * 2.0;
        }
      }
    }

    double out_dist = 0.0;
    if (sim.pods[rp].pos.x < -1000.0)
      out_dist += (-1000.0 - sim.pods[rp].pos.x);
    else if (sim.pods[rp].pos.x > 17000.0)
      out_dist += (sim.pods[rp].pos.x - 17000.0);
    if (sim.pods[rp].pos.y < -1000.0)
      out_dist += (-1000.0 - sim.pods[rp].pos.y);
    else if (sim.pods[rp].pos.y > 10000.0)
      out_dist += (sim.pods[rp].pos.y - 10000.0);

    const double* DTE = ctx.DTE;
    const double* RRX = ctx.RRX;
    const double* RRY = ctx.RRY;
    int mx = ctx.mx;
    int rb = ctx.rb;
    double dw = ctx.dw;
    double aw = ctx.aw;
    double sw = ctx.sw;
    double lw = ctx.lw;
    double apw = ctx.apw;
    double byw = ctx.byw;
    double opw = ctx.opw;
    double sfw = ctx.sfw;
    double faw = ctx.faw;
    double rw = ctx.rw;
    double s = 0.0;
    if (__builtin_expect(out_dist > 0.0, 0)) s -= 50000.0 + out_dist * 200.0;

    if (__builtin_expect(
            sim.pods[rp].pos.x < -1000.0 || sim.pods[rp].pos.x > 17000.0 ||
                sim.pods[rp].pos.y < -1000.0 || sim.pods[rp].pos.y > 10000.0,
            0))
      s -= 100000.0;
    if (__builtin_expect(
            sim.pods[bp].pos.x < -1000.0 || sim.pods[bp].pos.x > 17000.0 ||
                sim.pods[bp].pos.y < -1000.0 || sim.pods[bp].pos.y > 10000.0,
            0))
      s -= 100000.0;

    s -= friendly_penalty;

    if (__builtin_expect(ctx.is_runner_pinned, 0)) {
      double odx = sim.pods[rp].pos.x - sim.pods[obp].pos.x;
      double ody = sim.pods[rp].pos.y - sim.pods[obp].pos.y;
      double od2 = odx * odx + ody * ody;
      if (od2 < 1440000.0) {
        double od = std::sqrt(od2);
        s -= (1200.0 - od) * 20.0;
      }
    }

    if (__builtin_expect(sim.pods[rp].won || sim.pods[bp].won, 0))
      s += WIN_BONUS;
    if (__builtin_expect(
            !racing_only && (sim.pods[orp].won || sim.pods[obp].won), 0))
      s -= WIN_BONUS;

    int crossed = sim.pods[rp].next - ir_cp;
    if (crossed > 0)
      s += crossed * CP_CROSS_BONUS - static_cast<double>(r_act) * 400.0;

    double d_ex = -1.0;
    double dx_ex = 0.0, dy_ex = 0.0;
    if (!sim.pods[rp].won) {
      int rl = sim.pods[rp].next;
      if (rl < mx) {
        int cur_cp = sim.pods[rp].next % ncp;
        bool is_final_leg_target = (sim.pods[rp].next >= sim.totalRaceCps - 1);
        double tx = is_final_leg_target ? sim.globalCp[sim.pods[rp].next].x
                                        : EX[cur_cp];
        double ty = is_final_leg_target ? sim.globalCp[sim.pods[rp].next].y
                                        : EY[cur_cp];
        bool tx_shifted = false;
        if (!racing_only) {
          double ob_dx = sim.pods[obp].pos.x - tx;
          double ob_dy = sim.pods[obp].pos.y - ty;
          double ob_d2 = ob_dx * ob_dx + ob_dy * ob_dy;
          if (ob_d2 < 1440000.0 && ob_d2 > 1.0) {
            double inv_ob_d = 500.0 / std::sqrt(ob_d2);
            tx -= ob_dx * inv_ob_d;
            ty -= ob_dy * inv_ob_d;
            double cur_cpx = sim.globalCp[sim.pods[rp].next].x;
            double cur_cpy = sim.globalCp[sim.pods[rp].next].y;
            double cpd2 = (tx - cur_cpx) * (tx - cur_cpx) +
                          (ty - cur_cpy) * (ty - cur_cpy);
            if (cpd2 > 250000.0) {
              double inv_cpd = 500.0 / std::sqrt(cpd2);
              tx = cur_cpx + (tx - cur_cpx) * inv_cpd;
              ty = cur_cpy + (ty - cur_cpy) * inv_cpd;
            }
            tx_shifted = true;
          }
        }
        double dx = sim.pods[rp].pos.x - tx;
        double dy = sim.pods[rp].pos.y - ty;
        double d_target = std::sqrt(dx * dx + dy * dy);
        s -= (DTE[rl] + d_target) * dw;
        d_ex = d_target;
        dx_ex = -dx;
        dy_ex = -dy;
      }
    }

    double dx_to_cp = sim.globalCp[sim.pods[rp].next].x - sim.pods[rp].pos.x;
    double dy_to_cp = sim.globalCp[sim.pods[rp].next].y - sim.pods[rp].pos.y;
    double d2_to_cp = dx_to_cp * dx_to_cp + dy_to_cp * dy_to_cp;

    if (!racing_only) {
      int our_progress = sim.pods[rp].next;
      int opp_progress = sim.pods[orp].next;
      int diff = our_progress - opp_progress;
      s += diff * PROGRESS_DIFF_WEIGHT;

      double opp_dx = sim.pods[orp].pos.x - sim.globalCp[sim.pods[orp].next].x;
      double opp_dy = sim.pods[orp].pos.y - sim.globalCp[sim.pods[orp].next].y;
      double opp_dist = std::sqrt(opp_dx * opp_dx + opp_dy * opp_dy);
      double our_dist = std::sqrt(d2_to_cp);
      double opp_rem =
          ((sim.pods[orp].next < mx) ? DTE[sim.pods[orp].next] : 0.0) +
          opp_dist;
      double our_rem =
          ((sim.pods[rp].next < mx) ? DTE[sim.pods[rp].next] : 0.0) + our_dist;
      double lead = opp_rem - our_rem;
      s += lead * 0.60;
    }

    bool is_final_leg = (sim.pods[rp].next >= sim.totalRaceCps - 1);
    if (!is_final_leg) {
      if (d2_to_cp < 9000000.0) {
        int next_next = (sim.pods[rp].next % ncp + 1) % ncp;
        double nnx = sim.globalCp[next_next].x - sim.pods[rp].pos.x;
        double nny = sim.globalCp[next_next].y - sim.pods[rp].pos.y;
        double nnd = std::sqrt(nnx * nnx + nny * nny);
        if (nnd > 0.0) {
          double alignment =
              (sim.pods[rp].vel.x * nnx + sim.pods[rp].vel.y * nny) / nnd;
          s += alignment * 0.4;
        }
      }
    }

    {
      int cur_cp = sim.pods[rp].next % ncp;
      double dx = dx_ex;
      double dy = dy_ex;
      double d = d_ex;
      if (__builtin_expect(d < 0.0, 0)) {
        dx = EX[cur_cp] - sim.pods[rp].pos.x;
        dy = EY[cur_cp] - sim.pods[rp].pos.y;
        d = std::sqrt(dx * dx + dy * dy);
      }
      if (d > 1e-6) {
        double inv_d = 1.0 / d;
        double nx = dx * inv_d, ny = dy * inv_d;
        s += (sim.pods[rp].vel.x * nx + sim.pods[rp].vel.y * ny) * aw;
        if (!is_final_leg && d < 3000.0) {
          if (__builtin_expect(ctx.out_ux != nullptr, 1)) {
            if (ctx.out_len[cur_cp] > 1.0) {
              double exit_align = sim.pods[rp].vel.x * ctx.out_ux[cur_cp] +
                                  sim.pods[rp].vel.y * ctx.out_uy[cur_cp];
              s += exit_align * 2.0;
            }
          } else {
            int next_cp_id = (sim.pods[rp].next % ncp + 1) % ncp;
            double out_dx = EX[next_cp_id] - EX[cur_cp];
            double out_dy = EY[next_cp_id] - EY[cur_cp];
            double out_len = std::sqrt(out_dx * out_dx + out_dy * out_dy);
            if (out_len > 1.0) {
              double out_ux = out_dx / out_len, out_uy = out_dy / out_len;
              double exit_align =
                  sim.pods[rp].vel.x * out_ux + sim.pods[rp].vel.y * out_uy;
              s += exit_align * 2.0;
            }
          }
        }
        double lat = sim.pods[rp].vel.x * ny - sim.pods[rp].vel.y * nx;
        s -= std::abs(lat) * lw;
        double ta = fast_atan2(dy, dx);
        double ae = ta - sim.pods[rp].angle;
        double k = __builtin_rint(ae * INV_TWO_PI);
        ae -= k * TWO_PI;
        s -= std::abs(ae * RAD_TO_DEG) * apw;
      }

      double r_spd = sim.pods[rp].vel.norm();
      double in_len = d;
      if (!is_final_leg && in_len < 3100.0) {
        double in_dx = dx, in_dy = dy;
        int next_cp_id = (cur_cp + 1) % ncp;
        double out_dx, out_dy, out_len;
        if (__builtin_expect(ctx.out_dx != nullptr, 1)) {
          out_dx = ctx.out_dx[cur_cp];
          out_dy = ctx.out_dy[cur_cp];
          out_len = ctx.out_len[cur_cp];
        } else {
          out_dx = EX[next_cp_id] - EX[cur_cp];
          out_dy = EY[next_cp_id] - EY[cur_cp];
          out_len = std::sqrt(out_dx * out_dx + out_dy * out_dy);
        }
        double turn_deg = 0.0;
        if (in_len > 1.0 && out_len > 1.0) {
          turn_deg = std::abs(fast_atan2(in_dx * out_dy - in_dy * out_dx,
                                         in_dx * out_dx + in_dy * out_dy)) *
                     RAD_TO_DEG;
        }
        constexpr double INV_80 = 1.0 / 80.0;
        constexpr double INV_3100 = 1.0 / 3100.0;
        double corner_tightness =
            std::clamp((turn_deg - 48.0) * INV_80, 0.0, 1.0);
        double corner_prox = (3100.0 - in_len) * INV_3100;
        double cw = corner_tightness * corner_prox;

        if (cw > 0.04) {
          s += r_spd * (sw * 0.35 - cw * 0.30);
        } else {
          s += r_spd * sw;
        }
      } else {
        s += r_spd * sw;
      }
    }

    s -= ACTIVATION_PENALTY * r_act;

    if (__builtin_expect(racing_only, 0)) {
      if (ind->rs < 3) s -= 5000.0;
      if (ind->rs >= H && ind->rt[0] > 0) s += ind->rt[0] * 0.16;
      return s;
    }

    {
      double pred_obx = sim.pods[obp].pos.x + sim.pods[obp].vel.x * 1.85;
      double pred_oby = sim.pods[obp].pos.y + sim.pods[obp].vel.y * 1.85;
      double pred_rx = sim.pods[rp].pos.x + sim.pods[rp].vel.x * 1.85;
      double pred_ry = sim.pods[rp].pos.y + sim.pods[rp].vel.y * 1.85;
      double dx = pred_obx - pred_rx, dy = pred_oby - pred_ry;
      double d2 = dx * dx + dy * dy;
      constexpr double threshold2 = 810000.0;
      constexpr double inv_threshold2 = 1.0 / 810000.0;
      if (d2 < threshold2) {
        double penalty = byw * 80.0 * (1.0 - d2 * inv_threshold2);
        if (sim.pods[obp].shieldtimer >= 3) penalty *= 1.5;
        s -= penalty;
      }
    }

    double bx = sim.pods[bp].pos.x - sim.pods[orp].pos.x;
    double by = sim.pods[bp].pos.y - sim.pods[orp].pos.y;
    double b_d2 = bx * bx + by * by;

    s += 10000.0 * opw * o_act;
    double opp_to_cp_x =
        sim.globalCp[sim.pods[orp].next].x - sim.pods[orp].pos.x;
    double opp_to_cp_y =
        sim.globalCp[sim.pods[orp].next].y - sim.pods[orp].pos.y;
    double opp_to_cp_d =
        std::sqrt(opp_to_cp_x * opp_to_cp_x + opp_to_cp_y * opp_to_cp_y);
    if (sim.pods[orp].next == ior_cp) {
      s += 10.0 * opw * opp_to_cp_d;
    }

    if (rtimeout == 1) {
      if (!sim.pods[bp].won) {
        int bl = sim.pods[bp].next;
        if (bl < mx) {
          double dx = sim.pods[bp].pos.x - EX[sim.pods[bp].next % ncp];
          double dy = sim.pods[bp].pos.y - EY[sim.pods[bp].next % ncp];
          s -= (DTE[bl] + std::sqrt(dx * dx + dy * dy)) * dw;
        }
      }
    } else {
      int target_rb = (sim.pods[orp].next > rb) ? sim.pods[orp].next : rb;
      if (__builtin_expect(target_rb >= sim.totalRaceCps, 0))
        target_rb = sim.totalRaceCps - 1;
      int target_rb_mod = target_rb % ncp;
      double b_dist = std::sqrt(b_d2);
      {
        double cx = sim.globalCp[target_rb].x - sim.pods[orp].pos.x,
               cy = sim.globalCp[target_rb].y - sim.pods[orp].pos.y;
        double c_d2 = cx * cx + cy * cy;
        if (c_d2 > 1.0 && b_d2 > 1.0) {
          double dot = bx * cx + by * cy;
          if (dot > 0.0) {
            double cos_ray = dot / (b_dist * std::sqrt(c_d2));
            if (cos_ray > 1.0) cos_ray = 1.0;
            s += cos_ray * cos_ray * 5000.0;
          } else {
            s -= 3000.0;  // Behind runner penalty
          }
        }
        s -= sfw * fast_atan2(std::abs(bx * cy - by * cx), bx * cx + by * cy);
      }
      {
        double ta = fast_atan2(-by, -bx);
        double ae = ta - sim.pods[bp].angle;
        double k = __builtin_rint(ae * INV_TWO_PI);
        ae -= k * TWO_PI;
        s -= faw * std::abs(ae * RAD_TO_DEG);
      }
      {
        double dx = sim.pods[bp].pos.x - RRX[target_rb_mod],
               dy = sim.pods[bp].pos.y - RRY[target_rb_mod];
        double d = std::sqrt(dx * dx + dy * dy);
        s -= rw * (d < 300.0 ? (d - 300.0) * 0.1 : d - 300.0);
      }
      {
        double dist = b_dist;
        if (dist < 1200.0) {
          double dvx = sim.pods[bp].vel.x - sim.pods[orp].vel.x;
          double dvy = sim.pods[bp].vel.y - sim.pods[orp].vel.y;
          s -= std::sqrt(dvx * dvx + dvy * dvy) * 2.0;
          if (dist <= 850.0) {
            s += 2500.0;
            s -= sim.pods[bp].vel.norm() * 1.5;
          }
        }
        if (dist < BLOCKER_PROX_FAR) {
          constexpr double INV_FAR_CLOSE =
              1.0 / (BLOCKER_PROX_FAR - BLOCKER_PROX_CLOSE);
          constexpr double INV_MED_CLOSE =
              1.0 / (BLOCKER_PROX_MED - BLOCKER_PROX_CLOSE);
          double norm = (BLOCKER_PROX_FAR - dist) * INV_FAR_CLOSE;
          if (norm > 1.0) norm = 1.0;
          s += norm * BLOCKER_PROX_WEIGHT * 1000.0;
          if (dist < BLOCKER_PROX_MED) {
            double close_norm = (BLOCKER_PROX_MED - dist) * INV_MED_CLOSE;
            if (close_norm > 1.0) close_norm = 1.0;
            s += close_norm * close_norm * BLOCKER_PROX_WEIGHT * 2000.0;
          }
        }
        if (dist > BLOCKER_PROX_MED) s -= (dist - BLOCKER_PROX_MED) * 0.5;
      }
      if (opp_to_cp_d > 1.0) {
        double inv_opp_cp_d = 1.0 / opp_to_cp_d;
        double nx = opp_to_cp_x * inv_opp_cp_d;
        double ny = opp_to_cp_y * inv_opp_cp_d;
        double proj = bx * nx + by * ny;
        double lat = std::abs(bx * ny - by * nx);
        if (proj > 0 && proj < opp_to_cp_d) {
          double frac = proj * inv_opp_cp_d;
          double pos_quality = std::max(0.0, 1.0 - std::abs(frac - 0.35) * 2.0);
          double lat_quality = std::max(0.0, 1.0 - lat * (1.0 / 1500.0));
          s += pos_quality * lat_quality * BLOCKER_INTERPOSE_WEIGHT * 1000.0;
        }
        double alignment = sim.pods[orp].vel.x * nx + sim.pods[orp].vel.y * ny;
        s -= alignment * BLOCKER_MISALIGN_WEIGHT;
      }
      s += CP_CROSS_BONUS * o_act;
      {
        double opp_init_speed = __builtin_expect(ctx.opp_init_speed > 0.0, 1)
                                    ? ctx.opp_init_speed
                                    : base_game.pods[orp].vel.norm();
        double opp_end_speed = sim.pods[orp].vel.norm();
        double speed_loss = opp_init_speed - opp_end_speed;
        if (speed_loss > 0) s += speed_loss * BLOCKER_SPEED_LOSS_WEIGHT;
      }
      s += BLOCKER_PERSTEP_PROX_WEIGHT * blocker_prox_sum * 1000.0;
    }

    if (ind->rs < 3) s -= 5000.0;
    if (ind->bs < 3) s -= 2000.0;
    if (ind->rs >= H && ind->rt[0] > 0) s += ind->rt[0] * 0.16;
    if (ind->bs >= H && ind->bt[0] > 0) s += ind->bt[0] * 0.01;

    return s;
  }
};

static constexpr int MCTS_N_ANGLES = 7;
static constexpr int MCTS_N_THRUSTS = 4;
static constexpr int MCTS_N_CANDIDATES = MCTS_N_ANGLES * MCTS_N_THRUSTS;
static constexpr double MCTS_ANGLES[MCTS_N_ANGLES] = {-18, -12, -6, 0,
                                                      6,   12,  18};
static constexpr int MCTS_THRUSTS[MCTS_N_THRUSTS] = {0, 100, 150, 200};
static constexpr int MCTS_TOP_K = 5;

inline int mcts_first_move_probe(RNG& rng, const SimGame& base_game,
                                 const EvalContext& ctx, int budget_us,
                                 Ind* out_seeds) {
  constexpr int H = GA_HORIZON;
  double sum_score[MCTS_N_CANDIDATES] = {};
  int visit_count[MCTS_N_CANDIDATES] = {};
  Ind probe;
  probe.rs = H;
  probe.bs = H;
  struct timespec t0, tn;
  clock_gettime(CLOCK_MONOTONIC, &t0);
  int round = 0;
  while (true) {
    int c = round % MCTS_N_CANDIDATES;
    round++;
    int ai = c / MCTS_N_THRUSTS;
    int ti = c % MCTS_N_THRUSTS;
    probe.ra[0] = MCTS_ANGLES[ai];
    probe.rt[0] = MCTS_THRUSTS[ti];
    for (int t = 1; t < H; t++) {
      int raw_a = rng.ri(-180, 180);
      probe.ra[t] = raw_a * 0.1;
      probe.rt[t] = rand_thrust(rng);
    }
    for (int t = 0; t < H; t++) {
      int raw_ba = rng.ri(-180, 180);
      probe.ba[t] = raw_ba * 0.1;
      probe.bt[t] = rand_thrust(rng);
    }
    double sc = V40ChampionFitnessEvaluator::evaluate(&probe, base_game, ctx);
    sum_score[c] += sc;
    visit_count[c]++;
    if (c == MCTS_N_CANDIDATES - 1) {
      clock_gettime(CLOCK_MONOTONIC, &tn);
      long el = (tn.tv_sec - t0.tv_sec) * 1000000L +
                (tn.tv_nsec - t0.tv_nsec) / 1000L;
      if (el >= budget_us) break;
    }
  }
  int ranked[MCTS_N_CANDIDATES];
  for (int i = 0; i < MCTS_N_CANDIDATES; i++) ranked[i] = i;
  for (int i = 0; i < MCTS_TOP_K; i++) {
    int best_j = i;
    double best_avg =
        sum_score[ranked[i]] / std::max(1, visit_count[ranked[i]]);
    for (int j = i + 1; j < MCTS_N_CANDIDATES; j++) {
      double avg = sum_score[ranked[j]] / std::max(1, visit_count[ranked[j]]);
      if (avg > best_avg) {
        best_avg = avg;
        best_j = j;
      }
    }
    if (best_j != i) std::swap(ranked[i], ranked[best_j]);
  }
  int seeds_created = 0;
  for (int k = 0; k < MCTS_TOP_K; k++) {
    int c = ranked[k];
    int ai = c / MCTS_N_THRUSTS;
    int ti = c % MCTS_N_THRUSTS;
    Ind& seed = out_seeds[k];
    seed.ra[0] = MCTS_ANGLES[ai];
    seed.rt[0] = MCTS_THRUSTS[ti];
    for (int t = 1; t < H; t++) {
      int raw_a = rng.ri(-180, 180);
      seed.ra[t] = raw_a * 0.1;
      seed.rt[t] = rand_thrust(rng);
    }
    for (int t = 0; t < H; t++) {
      int raw_ba = rng.ri(-180, 180);
      seed.ba[t] = raw_ba * 0.1;
      seed.bt[t] = rand_thrust(rng);
    }
    seed.rs = H;
    seed.bs = H;
    seeds_created++;
  }
  return seeds_created;
}

template <typename FitnessEvaluator>
struct GeneticOptimizer {
  static int run(RNG& rng, const SimGame& base_game, const EvalContext& ctx,
                 int budget_us, const double* prev_ra, const int* prev_rt,
                 const double* prev_ba, const int* prev_bt, int prev_rs,
                 int prev_bs, int has_prev, double* out_ra, int* out_rt,
                 double* out_ba, int* out_bt, int* out_rs, int* out_bs,
                 const Ind* lock_runner = nullptr,
                 const Ind* lock_blocker = nullptr, int count_setup = 0,
                 struct timespec t_turn_start = {0, 0},
                 int hard_deadline_us = 0) {
    struct timespec t_entry;
    clock_gettime(CLOCK_MONOTONIC, &t_entry);

    alignas(64) Ind pop[GA_POP_SIZE];
    int PS = GA_POP_SIZE;
    constexpr int H = GA_HORIZON;
    int idx = 0;

    if (has_prev && prev_ra && prev_rt && prev_ba && prev_bt) {
      for (int t = 0; t < H - 1; t++) {
        pop[0].ra[t] = prev_ra[t + 1];
        pop[0].rt[t] = prev_rt[t + 1];
        pop[0].ba[t] = prev_ba[t + 1];
        pop[0].bt[t] = prev_bt[t + 1];
      }
      pop[0].ra[H - 1] = rng.rd() * 36.0 - 18.0;
      pop[0].rt[H - 1] = rand_thrust(rng);
      pop[0].ba[H - 1] = rng.rd() * 36.0 - 18.0;
      pop[0].bt[H - 1] = rand_thrust(rng);
      pop[0].rs = (prev_rs > 0 && prev_rs < H) ? prev_rs - 1 : H;
      pop[0].bs = (prev_bs > 0 && prev_bs < H) ? prev_bs - 1 : H;
      idx = 1;

      for (int pv = 0; pv < 4 && idx < PS; pv++) {
        pop[idx] = pop[0];
        for (int t = 0; t < H; t++) {
          double va = pop[idx].ra[t] + rng.rd() * 12.0 - 6.0;
          pop[idx].ra[t] = std::clamp(va, -18.0, 18.0);
          int vt = pop[idx].rt[t] + rng.ri(-30, 30);
          pop[idx].rt[t] = std::clamp(vt, 0, 200);

          double vba = pop[idx].ba[t] + rng.rd() * 12.0 - 6.0;
          pop[idx].ba[t] = std::clamp(vba, -18.0, 18.0);
          int vbt = pop[idx].bt[t] + rng.ri(-30, 30);
          pop[idx].bt[t] = std::clamp(vbt, 0, 200);
        }
        pop[idx].rs = pop[0].rs;
        pop[idx].bs = pop[0].bs;
        idx++;
      }
    }

    {
      SimGame hsim = base_game;
      for (int t = 0; t < H; t++) {
        int cur_cp = hsim.pods[ctx.rp].next % ctx.ncp;
        double tx = ctx.EX[cur_cp], ty = ctx.EY[cur_cp];
        double ddx = tx - hsim.pods[ctx.rp].pos.x,
               ddy = ty - hsim.pods[ctx.rp].pos.y;
        double dd = std::sqrt(ddx * ddx + ddy * ddy);
        if (dd > 0) {
          double ux = ddx / dd, uy = ddy / dd;
          double perp =
              hsim.pods[ctx.rp].vel.x * (-uy) + hsim.pods[ctx.rp].vel.y * ux;
          double cf = std::abs(perp) * 3.0;
          if (cf > 800.0) cf = 800.0;
          if (perp > 0) {
            tx += uy * cf;
            ty -= ux * cf;
          } else {
            tx -= uy * cf;
            ty += ux * cf;
          }
          if (dd < 2500.0) {
            int nnc = (cur_cp + 1) % ctx.ncp;
            double bl = (2500.0 - dd) / 2500.0;
            tx = tx * (1.0 - bl) + ctx.EX[nnc] * bl;
            ty = ty * (1.0 - bl) + ctx.EY[nnc] * bl;
          }
        }
        double ta = fast_atan2(ty - hsim.pods[ctx.rp].pos.y,
                               tx - hsim.pods[ctx.rp].pos.x);
        double diff = ta - hsim.pods[ctx.rp].angle;
        diff -= __builtin_rint(diff * INV_TWO_PI) * TWO_PI;
        double diff_deg = std::clamp(diff * RAD_TO_DEG, -18.0, 18.0);

        if (idx < PS) {
          pop[idx].ra[t] = diff_deg;
          pop[idx].rt[t] = 200;
        }
        if (idx + 1 < PS) {
          pop[idx + 1].ra[t] = diff_deg;
          pop[idx + 1].rt[t] = 150;
        }

        double btx = ctx.RRX[ctx.rb % ctx.ncp], bty = ctx.RRY[ctx.rb % ctx.ncp];
        double bd = hsim.pods[ctx.bp].pos.dist(hsim.pods[ctx.orp].pos);
        if (bd < 3000.0) {
          btx = hsim.pods[ctx.orp].pos.x + hsim.pods[ctx.orp].vel.x * 2.0;
          bty = hsim.pods[ctx.orp].pos.y + hsim.pods[ctx.orp].vel.y * 2.0;
        }
        double bta = fast_atan2(bty - hsim.pods[ctx.bp].pos.y,
                                btx - hsim.pods[ctx.bp].pos.x);
        double bdiff = bta - hsim.pods[ctx.bp].angle;
        bdiff -= __builtin_rint(bdiff * INV_TWO_PI) * TWO_PI;
        double bdiff_deg = std::clamp(bdiff * RAD_TO_DEG, -18.0, 18.0);

        if (idx < PS) {
          pop[idx].ba[t] = bdiff_deg;
          pop[idx].bt[t] = 200;
        }
        if (idx + 1 < PS) {
          pop[idx + 1].ba[t] = bdiff_deg;
          pop[idx + 1].bt[t] = 200;
        }

        hsim.pods[ctx.rp].applyActionDelta(diff_deg, 200, 0, 0);
        hsim.pods[ctx.bp].applyActionDelta(bdiff_deg, 200, 0, 0);
        hsim.applyAction(
            ctx.orp,
            static_cast<int>(ctx.EX[hsim.pods[ctx.orp].next % ctx.ncp]),
            static_cast<int>(ctx.EY[hsim.pods[ctx.orp].next % ctx.ncp]), 200);
        hsim.applyAction(ctx.obp, static_cast<int>(hsim.pods[ctx.rp].pos.x),
                         static_cast<int>(hsim.pods[ctx.rp].pos.y), 200);
        hsim.nextTurn();
      }
      if (idx < PS) {
        pop[idx].rs = H;
        pop[idx].bs = H;
        idx++;
      }
      if (idx < PS) {
        pop[idx].rs = H;
        pop[idx].bs = H;
        idx++;
      }
    }

    if (ctx.is_runner_pinned && idx < PS) {
      double odx = base_game.pods[ctx.rp].pos.x - base_game.pods[ctx.obp].pos.x;
      double ody = base_game.pods[ctx.rp].pos.y - base_game.pods[ctx.obp].pos.y;
      double p1x = -ody, p1y = odx;
      int cur_cp = base_game.pods[ctx.rp].next % ctx.ncp;
      double tcpx = ctx.EX[cur_cp] - base_game.pods[ctx.rp].pos.x;
      double tcpy = ctx.EY[cur_cp] - base_game.pods[ctx.rp].pos.y;
      double dot1 = p1x * tcpx + p1y * tcpy;
      double ex = (dot1 >= 0.0) ? p1x : -p1x;
      double ey = (dot1 >= 0.0) ? p1y : -p1y;
      double ea = fast_atan2(ey, ex);
      double ediff = ea - base_game.pods[ctx.rp].angle;
      ediff -= __builtin_rint(ediff * INV_TWO_PI) * TWO_PI;
      double ediff_deg = std::clamp(ediff * RAD_TO_DEG, -18.0, 18.0);
      for (int t = 0; t < H; t++) {
        pop[idx].ra[t] = ediff_deg;
        pop[idx].rt[t] = 200;
        pop[idx].ba[t] = 0.0;
        pop[idx].bt[t] = 200;
      }
      pop[idx].rs = H;
      pop[idx].bs = H;
      idx++;
    }

    if (idx + 1 < PS) {
      SimGame isim = base_game;
      for (int t = 0; t < H; t++) {
        int cur_cp = isim.pods[ctx.rp].next % ctx.ncp;
        double itx = ctx.EX[cur_cp], ity = ctx.EY[cur_cp];
        double ita = fast_atan2(ity - isim.pods[ctx.rp].pos.y,
                                itx - isim.pods[ctx.rp].pos.x);
        double idiff = ita - isim.pods[ctx.rp].angle;
        idiff -= __builtin_rint(idiff * INV_TWO_PI) * TWO_PI;
        double idiff_deg = std::clamp(idiff * RAD_TO_DEG, -18.0, 18.0);
        pop[idx].ra[t] = idiff_deg;
        pop[idx].rt[t] = 200;

        double pred_x =
            isim.pods[ctx.orp].pos.x + isim.pods[ctx.orp].vel.x * 3.0;
        double pred_y =
            isim.pods[ctx.orp].pos.y + isim.pods[ctx.orp].vel.y * 3.0;
        double ibta = fast_atan2(pred_y - isim.pods[ctx.bp].pos.y,
                                 pred_x - isim.pods[ctx.bp].pos.x);
        double ibdiff = ibta - isim.pods[ctx.bp].angle;
        ibdiff -= __builtin_rint(ibdiff * INV_TWO_PI) * TWO_PI;
        double ibdiff_deg = std::clamp(ibdiff * RAD_TO_DEG, -18.0, 18.0);
        pop[idx].ba[t] = ibdiff_deg;
        pop[idx].bt[t] = 200;

        isim.pods[ctx.rp].applyActionDelta(idiff_deg, 200, 0, 0);
        isim.pods[ctx.bp].applyActionDelta(ibdiff_deg, 200, 0, 0);
        isim.applyAction(
            ctx.orp,
            static_cast<int>(ctx.EX[isim.pods[ctx.orp].next % ctx.ncp]),
            static_cast<int>(ctx.EY[isim.pods[ctx.orp].next % ctx.ncp]), 200);
        isim.applyAction(ctx.obp, static_cast<int>(isim.pods[ctx.rp].pos.x),
                         static_cast<int>(isim.pods[ctx.rp].pos.y), 200);
        isim.nextTurn();
      }
      pop[idx].rs = H;
      pop[idx].bs = H;
      idx++;
    }

    {
      Ind mcts_seeds[MCTS_TOP_K];
      int n_mcts = mcts_first_move_probe(rng, base_game, ctx, MCTS_BUDGET_US,
                                         mcts_seeds);
      for (int k = 0; k < n_mcts && idx < PS; k++) {
        pop[idx] = mcts_seeds[k];
        idx++;
      }
    }

    {
      int src = 0;
      for (int sv = 0; sv < 4 && idx < PS; sv++) {
        pop[idx] = pop[src];
        switch (sv) {
          case 0:
            pop[idx].rs = 0;
            pop[idx].bs = H;
            break;
          case 1:
            pop[idx].rs = H;
            pop[idx].bs = 0;
            break;
          case 2:
            pop[idx].rs = 1;
            pop[idx].bs = H;
            break;
          case 3:
            pop[idx].rs = H;
            pop[idx].bs = 1;
            break;
        }
        idx++;
      }
    }

    if (idx + 4 <= PS) {
      for (int t = 0; t < H; t++) {
        pop[idx].ra[t] = 0;
        pop[idx].rt[t] = 200;
        pop[idx].ba[t] = 0;
        pop[idx].bt[t] = 200;
        pop[idx + 1].ra[t] = 0;
        pop[idx + 1].rt[t] = 0;
        pop[idx + 1].ba[t] = 0;
        pop[idx + 1].bt[t] = 0;
        pop[idx + 2].ra[t] = -18;
        pop[idx + 2].rt[t] = 200;
        pop[idx + 2].ba[t] = -18;
        pop[idx + 2].bt[t] = 200;
        pop[idx + 3].ra[t] = 18;
        pop[idx + 3].rt[t] = 200;
        pop[idx + 3].ba[t] = 18;
        pop[idx + 3].bt[t] = 200;
      }
      for (int k = 0; k < 4; k++) {
        pop[idx + k].rs = H;
        pop[idx + k].bs = H;
      }
      idx += 4;
    }

    for (int i = idx; i < PS; i++) {
      for (int t = 0; t < H; t++) {
        pop[i].ra[t] = std::clamp(rng.ri(-400, 400) * 0.1, -18.0, 18.0);
        pop[i].rt[t] = rand_thrust(rng);
        pop[i].ba[t] = std::clamp(rng.ri(-400, 400) * 0.1, -18.0, 18.0);
        pop[i].bt[t] = rand_thrust(rng);
      }
      pop[i].rs = rng.ri(0, H + 4);
      pop[i].bs = rng.ri(0, H + 4);
    }

    if (__builtin_expect(lock_runner != nullptr, 0)) {
      for (int i = 0; i < PS; i++) {
        std::memcpy(pop[i].ra, lock_runner->ra, H * sizeof(double));
        std::memcpy(pop[i].rt, lock_runner->rt, H * sizeof(int));
        pop[i].rs = lock_runner->rs;
      }
    }
    if (__builtin_expect(lock_blocker != nullptr, 0)) {
      for (int i = 0; i < PS; i++) {
        std::memcpy(pop[i].ba, lock_blocker->ba, H * sizeof(double));
        std::memcpy(pop[i].bt, lock_blocker->bt, H * sizeof(int));
        pop[i].bs = lock_blocker->bs;
      }
    }
    alignas(64) double scores[PS];
    int best = 0, worst = 0;
    for (int i = 0; i < PS; i++) {
      pop[i].sc = FitnessEvaluator::evaluate(&pop[i], base_game, ctx);
      scores[i] = pop[i].sc;
      if (scores[i] > scores[best]) best = i;
      if (scores[i] < scores[worst]) worst = i;
    }
    double wsc = scores[worst];
    double best_sc = scores[best];

    struct timespec tn;
    clock_gettime(CLOCK_MONOTONIC, &tn);
    if (t_turn_start.tv_sec == 0) t_turn_start = (count_setup ? t_entry : tn);
    long el = (tn.tv_sec - t_turn_start.tv_sec) * 1000000L +
              (tn.tv_nsec - t_turn_start.tv_nsec) / 1000L;
    if (hard_deadline_us <= 0) hard_deadline_us = budget_us + 500;
    int iters = PS;
    double amplitude = 1.0;
    uint32_t thresh_u32 =
        static_cast<uint32_t>((0.25 + amplitude) * 16777216.0);
    Ind child;

    while (el < budget_us) {
      long rem_us = budget_us - el;
      int chunk = 32;
      if (rem_us < 120)
        chunk = 1;
      else if (rem_us < 350)
        chunk = 2;
      else if (rem_us < 800)
        chunk = 4;
      else if (rem_us < 2000)
        chunk = 8;
      else if (rem_us < 6000)
        chunk = 16;

      amplitude =
          1.0 - static_cast<double>(el) / static_cast<double>(budget_us);
      if (amplitude < 0.0) amplitude = 0.0;
      thresh_u32 = static_cast<uint32_t>((0.25 + amplitude) * 16777216.0);

      for (int step = 0; step < chunk; ++step) {
        iters++;

        if (__builtin_expect(best_sc < wsc + 0.3, 0)) {
          for (int i = 0; i < PS; i++) {
            if (i != best) {
              pop[i].sc -= 2000.0;
              scores[i] -= 2000.0;
            }
          }
          wsc -= 2000.0;
        }

        uint64_t r_par = rng.u64();
        int p1 = static_cast<int>(
            (static_cast<uint32_t>(r_par) * static_cast<uint64_t>(PS)) >> 32);
        int p2 =
            static_cast<int>(((r_par >> 32) * static_cast<uint64_t>(PS)) >> 32);
        int par = (scores[p1] >= scores[p2]) ? p1 : p2;

        if (rng.u32() < 858993460U) {
          uint64_t r_par2 = rng.u64();
          int p3 = static_cast<int>(
              (static_cast<uint32_t>(r_par2) * static_cast<uint64_t>(PS)) >>
              32);
          int p4 = static_cast<int>(
              ((r_par2 >> 32) * static_cast<uint64_t>(PS)) >> 32);
          int par2 = (scores[p3] >= scores[p4]) ? p3 : p4;
          uint32_t mask = rng.u32();
          for (int t = 0; t < H; t++) {
            bool m_r = (mask >> (t * 2)) & 1;
            child.ra[t] = m_r ? pop[par].ra[t] : pop[par2].ra[t];
            child.rt[t] = m_r ? pop[par].rt[t] : pop[par2].rt[t];
            bool m_b = (mask >> (t * 2 + 1)) & 1;
            child.ba[t] = m_b ? pop[par].ba[t] : pop[par2].ba[t];
            child.bt[t] = m_b ? pop[par].bt[t] : pop[par2].bt[t];
          }
          child.rs = (mask & 0x4000) ? pop[par].rs : pop[par2].rs;
          child.bs = (mask & 0x8000) ? pop[par].bs : pop[par2].bs;
        } else {
          child.copyFrom(pop[par]);
          for (int t = 0; t < H; t++) {
            if ((rng.u32() >> 8) < thresh_u32) {
              int raw = rng.ri(-400, 400);
              if (raw < -180)
                raw = -180;
              else if (raw > 180)
                raw = 180;
              child.ra[t] = raw * 0.1;
            }
            if ((rng.u32() >> 8) < thresh_u32) {
              child.rt[t] = rand_thrust(rng);
            }
            if ((rng.u32() >> 8) < thresh_u32) {
              int raw = rng.ri(-400, 400);
              if (raw < -180)
                raw = -180;
              else if (raw > 180)
                raw = 180;
              child.ba[t] = raw * 0.1;
            }
            if ((rng.u32() >> 8) < thresh_u32) {
              child.bt[t] = rand_thrust(rng);
            }
          }
          if ((rng.u32() >> 8) < thresh_u32) child.rs = rng.ri(0, H + 4);
          if ((rng.u32() >> 8) < thresh_u32) child.bs = rng.ri(0, H + 4);
        }

        {
          int si = rng.ri(0, H - 1);
          double v = child.ra[si] + rng.rd() * 24.0 - 12.0;
          child.ra[si] = (v < -18.0) ? -18.0 : ((v > 18.0) ? 18.0 : v);
        }
        {
          int si = rng.ri(0, H - 1);
          int v = child.rt[si] + rng.ri(-50, 50);
          child.rt[si] = (v < 0) ? 0 : ((v > 200) ? 200 : v);
        }
        {
          int si = rng.ri(0, H - 1);
          double v = child.ba[si] + rng.rd() * 24.0 - 12.0;
          child.ba[si] = (v < -18.0) ? -18.0 : ((v > 18.0) ? 18.0 : v);
        }
        {
          int si = rng.ri(0, H - 1);
          int v = child.bt[si] + rng.ri(-50, 50);
          child.bt[si] = (v < 0) ? 0 : ((v > 200) ? 200 : v);
        }

        if (__builtin_expect(lock_runner != nullptr, 0)) {
          std::memcpy(child.ra, lock_runner->ra, H * sizeof(double));
          std::memcpy(child.rt, lock_runner->rt, H * sizeof(int));
          child.rs = lock_runner->rs;
        }
        if (__builtin_expect(lock_blocker != nullptr, 0)) {
          std::memcpy(child.ba, lock_blocker->ba, H * sizeof(double));
          std::memcpy(child.bt, lock_blocker->bt, H * sizeof(int));
          child.bs = lock_blocker->bs;
        }

        child.sc = FitnessEvaluator::evaluate(&child, base_game, ctx);
        if (child.sc > best_sc) {
          best_sc = child.sc;
          best = worst;
        }
        pop[worst].copyFrom(child);
        scores[worst] = child.sc;
        if (child.sc > wsc) {
          worst = 0;
          wsc = scores[0];
#pragma GCC unroll 8
          for (int i = 1; i < PS; i++) {
            if (scores[i] < wsc) {
              worst = i;
              wsc = scores[i];
            }
          }
        }
      }
      clock_gettime(CLOCK_MONOTONIC, &tn);
      el = (tn.tv_sec - t_turn_start.tv_sec) * 1000000L +
           (tn.tv_nsec - t_turn_start.tv_nsec) / 1000L;
      if (el >= hard_deadline_us) break;
    }

    std::memcpy(out_ra, pop[best].ra, H * sizeof(double));
    std::memcpy(out_rt, pop[best].rt, H * sizeof(int));
    std::memcpy(out_ba, pop[best].ba, H * sizeof(double));
    std::memcpy(out_bt, pop[best].bt, H * sizeof(int));
    *out_rs = pop[best].rs;
    *out_bs = pop[best].bs;
    return iters;
  }
};

class ChampionBot {
 public:
  ChampionBot() = default;

  void init(int laps,
            const std::vector<std::pair<double, double>>& checkpoints) {
    laps_ = laps + 1;
    ncp_ = static_cast<int>(checkpoints.size());
    CX_.resize(ncp_);
    CY_.resize(ncp_);
    for (int i = 0; i < ncp_; i++) {
      CX_[i] = checkpoints[i].first;
      CY_[i] = checkpoints[i].second;
    }
    compute_entry_points();
    compute_dte();
    compute_ram_rest_points();
    compute_exit_vectors();
    compute_weights();

    cp_buf_size_ = 0;
    global_cp_buf_.resize(laps_ * ncp_ + 2);
    for (int lap = 0; lap < laps_; lap++) {
      for (int c = 0; c < ncp_; c++) {
        global_cp_buf_[cp_buf_size_++] = {CX_[c], CY_[c]};
      }
    }
    global_cp_buf_[cp_buf_size_++] = {CX_[0], CY_[0]};

    turn_ = 0;
    prev_rp_ = -1;
    prev_orp_abs_ = -1;
    has_prev_ = false;
    has_opp_prev_ = false;
    for (int i = 0; i < 2; i++) {
      runner_stuck_turns_[i] = 0;
      ml_[i] = 0;
      ol_[i] = 0;
      pmc_[i] = -1;
      poc_[i] = -1;
      shield_cooldown_[i] = 0;
      boost_used_[i] = false;
    }
    rng_.seed(42);
  }

  struct ActionOutput {
    int target_x = 0;
    int target_y = 0;
    int thrust = 0;
    std::string thrust_str() const {
      if (thrust == -1) return "SHIELD";
      if (thrust == -2) return "BOOST";
      return std::to_string(thrust);
    }
  };

  std::pair<ActionOutput, ActionOutput> get_actions(
      const std::array<std::array<double, 6>, 4>& pods,
      struct timespec t_turn_start = {0, 0}) {
    if (t_turn_start.tv_sec == 0 && t_turn_start.tv_nsec == 0) {
      clock_gettime(CLOCK_MONOTONIC, &t_turn_start);
    }
    turn_++;
    double px[4], py[4], pvx[4], pvy[4], pang[4];
    int pnc[4], plp[4];

    for (int i = 0; i < 2; i++) {
      px[i] = pods[i][0];
      py[i] = pods[i][1];
      pvx[i] = pods[i][2];
      pvy[i] = pods[i][3];
      pang[i] = pods[i][4];
      pnc[i] = static_cast<int>(pods[i][5]);
      if (pang[i] < 0) {
        pang[i] = fast_atan2(CY_[pnc[i]] - py[i], CX_[pnc[i]] - px[i]) *
                  inoryy_ga::RAD_TO_DEG;
        if (pang[i] < 0) pang[i] += 360.0;
      }
      if (pmc_[i] == -1)
        pmc_[i] = pnc[i];
      else if (pnc[i] == 0 && pmc_[i] == ncp_ - 1) {
        ml_[i]++;
        runner_stuck_turns_[i] = 0;
      } else if (pnc[i] != pmc_[i])
        runner_stuck_turns_[i] = 0;
      else
        runner_stuck_turns_[i]++;
      pmc_[i] = pnc[i];
      plp[i] = ml_[i];
      if (shield_cooldown_[i] > 0) shield_cooldown_[i]--;
    }

    for (int i = 0; i < 2; i++) {
      px[i + 2] = pods[i + 2][0];
      py[i + 2] = pods[i + 2][1];
      pvx[i + 2] = pods[i + 2][2];
      pvy[i + 2] = pods[i + 2][3];
      pang[i + 2] = pods[i + 2][4];
      pnc[i + 2] = static_cast<int>(pods[i + 2][5]);
      if (pang[i + 2] < 0) {
        pang[i + 2] = fast_atan2(CY_[pnc[i + 2]] - py[i + 2],
                                 CX_[pnc[i + 2]] - px[i + 2]) *
                      inoryy_ga::RAD_TO_DEG;
        if (pang[i + 2] < 0) pang[i + 2] += 360.0;
      }
      if (poc_[i] == -1)
        poc_[i] = pnc[i + 2];
      else if (pnc[i + 2] == 0 && poc_[i] == ncp_ - 1)
        ol_[i]++;
      poc_[i] = pnc[i + 2];
      plp[i + 2] = ol_[i];
    }

    auto rprog = [&](int idx) -> double {
      int prog = plp[idx] * ncp_ + pnc[idx];
      int clamped = prog < mx_ ? prog : mx_ - 1;
      double dx = px[idx] - EX_[pnc[idx]], dy = py[idx] - EY_[pnc[idx]];
      return DTE_[clamped] + std::sqrt(dx * dx + dy * dy);
    };

    int rp = (rprog(1) < rprog(0) - 200.0) ? 1 : 0;
    int bp = 1 - rp;

    int prog2 = plp[2] * ncp_ + pnc[2];
    int prog3 = plp[3] * ncp_ + pnc[3];
    int orp_abs = 2;
    if (prog3 > prog2) {
      orp_abs = 3;
    } else if (prog2 > prog3) {
      orp_abs = 2;
    } else {
      orp_abs = (rprog(2) < rprog(3)) ? 2 : 3;
    }
    int obp_abs = 5 - orp_abs;

    if (rp != prev_rp_) {
      has_prev_ = false;
      prev_rp_ = rp;
    }
    if (orp_abs != prev_orp_abs_) {
      has_opp_prev_ = false;
      prev_orp_abs_ = orp_abs;
    }

    int rb = plp[orp_abs] * ncp_ + pnc[orp_abs];
    {
      int cur_cp = rb % ncp_;
      double od =
          std::hypot(px[orp_abs] - CX_[cur_cp], py[orp_abs] - CY_[cur_cp]);
      double md = std::hypot(px[bp] - CX_[cur_cp], py[bp] - CY_[cur_cp]);
      bool blocker_needs_cp1 = (plp[bp] == 0 && pnc[bp] <= 1);
      if (!blocker_needs_cp1 &&
          (md > od + 1200.0 || (od < 1800.0 && md > od + 300.0))) {
        int nb = (cur_cp + 1) % ncp_;
        double od2 =
            od + std::hypot(CX_[cur_cp] - CX_[nb], CY_[cur_cp] - CY_[nb]);
        double md2 = std::hypot(px[bp] - CX_[nb], py[bp] - CY_[nb]);
        if (md2 < od2 - 800.0 && rb + 1 < mx_) {
          rb = rb + 1;
        }
      }
    }

    int fboost = 0;
    if (!boost_used_[rp] && shield_cooldown_[rp] == 0) {
      if (turn_ == 1) {
        double d_start_cp =
            std::hypot(px[rp] - CX_[pnc[rp]], py[rp] - CY_[pnc[rp]]);
        int cp0 = 0, cp1 = 1 % ncp_, cp2 = 2 % ncp_;
        double v01x = CX_[cp1] - CX_[cp0], v01y = CY_[cp1] - CY_[cp0];
        double v12x = CX_[cp2] - CX_[cp1], v12y = CY_[cp2] - CY_[cp1];
        double l01 = std::hypot(v01x, v01y), l12 = std::hypot(v12x, v12y);
        double cos_turn = (l01 > 0.0 && l12 > 0.0)
                              ? (v01x * v12x + v01y * v12y) / (l01 * l12)
                              : 1.0;
        bool hold_boost = (d_start_cp < 4200.0 && cos_turn <= -0.50);
        if (d_start_cp > 3800.0 && !hold_boost) fboost = 1;
      } else {
        double dd = std::hypot(px[rp] - EX_[pnc[rp]], py[rp] - EY_[pnc[rp]]);
        double ta = fast_atan2(EY_[pnc[rp]] - py[rp], EX_[pnc[rp]] - px[rp]) *
                    inoryy_ga::RAD_TO_DEG;
        if (ta < 0) ta += 360.0;
        double ae = ta - pang[rp];
        while (ae > 180.0) ae -= 360.0;
        while (ae < -180.0) ae += 360.0;
        if (dd > 5000.0 && std::abs(ae) < 5.0) fboost = 1;
      }
    }

    int rtimeout = (runner_stuck_turns_[rp] >= 30) ? 1 : 0;
    bool is_runner_pinned =
        (runner_stuck_turns_[rp] >= 6 && std::hypot(pvx[rp], pvy[rp]) < 250.0 &&
         std::hypot(px[rp] - px[obp_abs], py[rp] - py[obp_abs]) < 1200.0);

    inoryy_ga::SimGame base_game;
    base_game.globalCp = global_cp_buf_.data();
    base_game.globalCpSize = cp_buf_size_;
    base_game.ncp = ncp_;
    base_game.totalRaceCps = (laps_ - 1) * ncp_ + 1;
    for (int i = 0; i < 4; i++) {
      int flattened_next = plp[i] * ncp_ + pnc[i];
      base_game.setPodState(i, px[i], py[i], pvx[i], pvy[i],
                            pang[i] * inoryy_ga::DEG_TO_RAD, flattened_next,
                            (i < 2 ? shield_cooldown_[i] : 0),
                            (i < 2 && boost_used_[i]) ? 1 : 0);
      base_game.pods[i].isFirstTurn = (turn_ == 1);
    }

    int ga_budget_us = (turn_ == 1) ? inoryy_ga::FIRST_TURN_BUDGET_US
                                    : inoryy_ga::GA_TARGET_DEADLINE_US;
    int hard_budget_us = (turn_ == 1) ? inoryy_ga::FIRST_TURN_HARD_US
                                      : inoryy_ga::HARD_TURN_BUDGET_US;
    bool use_ibr = has_prev_ && turn_ > 1;
    int ibr_budget = use_ibr ? inoryy_ga::IBR_BUDGET_US : 0;

    EvalContext ctx;
    ctx.ncp = ncp_;
    ctx.laps = laps_;
    ctx.mx = mx_;
    ctx.H = inoryy_ga::GA_HORIZON;
    ctx.EX = EX_.data();
    ctx.EY = EY_.data();
    ctx.DTE = DTE_.data();
    ctx.RRX = RRX_.data();
    ctx.RRY = RRY_.data();
    ctx.rp = rp;
    ctx.bp = bp;
    ctx.orp = orp_abs;
    ctx.obp = obp_abs;
    ctx.rb = rb;
    ctx.fboost = fboost;
    ctx.rtimeout = rtimeout;
    ctx.is_runner_pinned = is_runner_pinned ? 1 : 0;
    ctx.dw = dw_;
    ctx.aw = aw_;
    ctx.sw = sw_;
    ctx.lw = lw_;
    ctx.apw = apw_;
    ctx.byw = byw_;
    ctx.opw = opw_;
    ctx.sfw = sfw_;
    ctx.faw = faw_;
    ctx.rw = rw_;
    ctx.out_dx = out_dx_.data();
    ctx.out_dy = out_dy_.data();
    ctx.out_ux = out_ux_.data();
    ctx.out_uy = out_uy_.data();
    ctx.out_len = out_len_.data();
    ctx.opp_init_speed = base_game.pods[orp_abs].vel.norm();

    double pred_ora[inoryy_ga::GA_HORIZON], pred_oba[inoryy_ga::GA_HORIZON];
    int pred_ort[inoryy_ga::GA_HORIZON], pred_obt[inoryy_ga::GA_HORIZON];

    if (use_ibr) {
      double shifted_ra[inoryy_ga::GA_HORIZON],
          shifted_ba[inoryy_ga::GA_HORIZON];
      int shifted_rt[inoryy_ga::GA_HORIZON], shifted_bt[inoryy_ga::GA_HORIZON];
      for (int t = 0; t < inoryy_ga::GA_HORIZON - 1; t++) {
        shifted_ra[t] = prev_ra_[t + 1];
        shifted_rt[t] = prev_rt_[t + 1];
        shifted_ba[t] = prev_ba_[t + 1];
        shifted_bt[t] = prev_bt_[t + 1];
      }
      shifted_ra[inoryy_ga::GA_HORIZON - 1] = 0;
      shifted_rt[inoryy_ga::GA_HORIZON - 1] = 200;
      shifted_ba[inoryy_ga::GA_HORIZON - 1] = 0;
      shifted_bt[inoryy_ga::GA_HORIZON - 1] = 200;

      inoryy_ga::run_opp_prediction_ga(
          rng_, base_game, ctx, shifted_ra, shifted_rt, shifted_ba, shifted_bt,
          opp_prev_ra_, opp_prev_rt_, opp_prev_ba_, opp_prev_bt_,
          has_opp_prev_ ? 1 : 0, ibr_budget, pred_ora, pred_ort, pred_oba,
          pred_obt);

      std::memcpy(opp_prev_ra_, pred_ora,
                  inoryy_ga::GA_HORIZON * sizeof(double));
      std::memcpy(opp_prev_rt_, pred_ort, inoryy_ga::GA_HORIZON * sizeof(int));
      std::memcpy(opp_prev_ba_, pred_oba,
                  inoryy_ga::GA_HORIZON * sizeof(double));
      std::memcpy(opp_prev_bt_, pred_obt, inoryy_ga::GA_HORIZON * sizeof(int));
      has_opp_prev_ = true;

      ctx.opp_ra = pred_ora;
      ctx.opp_rt = pred_ort;
      ctx.opp_ba = pred_oba;
      ctx.opp_bt = pred_obt;
    }

    double out_ra[inoryy_ga::GA_HORIZON], out_ba[inoryy_ga::GA_HORIZON];
    int out_rt[inoryy_ga::GA_HORIZON], out_bt[inoryy_ga::GA_HORIZON];
    int out_rs = inoryy_ga::GA_HORIZON, out_bs = inoryy_ga::GA_HORIZON;

    rng_.seed(2654435761u * static_cast<unsigned>(turn_ + 1) + 1u);
    int ga_sims =
        inoryy_ga::GeneticOptimizer<inoryy_ga::V40ChampionFitnessEvaluator>::
            run(rng_, base_game, ctx, ga_budget_us, prev_ra_, prev_rt_,
                prev_ba_, prev_bt_, prev_rs_, prev_bs_, has_prev_ ? 1 : 0,
                out_ra, out_rt, out_ba, out_bt, &out_rs, &out_bs, nullptr,
                nullptr, 1, t_turn_start, hard_budget_us);

    std::memcpy(prev_ra_, out_ra, inoryy_ga::GA_HORIZON * sizeof(double));
    std::memcpy(prev_rt_, out_rt, inoryy_ga::GA_HORIZON * sizeof(int));
    std::memcpy(prev_ba_, out_ba, inoryy_ga::GA_HORIZON * sizeof(double));
    std::memcpy(prev_bt_, out_bt, inoryy_ga::GA_HORIZON * sizeof(int));
    prev_rs_ = out_rs;
    prev_bs_ = out_bs;
    has_prev_ = true;

    double r_da = std::clamp(out_ra[0], -18.0, 18.0);
    double r_ang_deg = std::fmod(pang[rp] + r_da, 360.0);
    if (r_ang_deg < 0) r_ang_deg += 360.0;
    double r_rad = r_ang_deg * inoryy_ga::DEG_TO_RAD;
    double rtx = px[rp] + std::cos(r_rad) * 10000.0;
    double rty = py[rp] + std::sin(r_rad) * 10000.0;
    bool r_boost = fboost && !boost_used_[rp];
    bool r_shield = !r_boost && (out_rs == 0 && shield_cooldown_[rp] == 0);
    if (!r_shield && !r_boost && shield_cooldown_[rp] == 0) {
      for (int opp : {obp_abs, orp_abs}) {
        double tc = base_game.pods[rp].newCollide(base_game.pods[opp],
                                                  inoryy_ga::POD_RSQ, 1.0);
        if (tc >= 0.0 && tc <= 1.0) {
          double dx = base_game.pods[opp].pos.x - base_game.pods[rp].pos.x;
          double dy = base_game.pods[opp].pos.y - base_game.pods[rp].pos.y;
          double dvx = base_game.pods[rp].vel.x - base_game.pods[opp].vel.x;
          double dvy = base_game.pods[rp].vel.y - base_game.pods[opp].vel.y;
          double closing = (dx * dvx + dy * dvy);
          double closing_spd = closing / inoryy_ga::POD_DIAMETER;
          double dv_mag = std::hypot(dvx, dvy);
          if (closing > 0.0 && (closing_spd > 250.0 || dv_mag > 350.0)) {
            inoryy_ga::Ind sol_no, sol_sh;
            std::memcpy(sol_no.ra, out_ra,
                        inoryy_ga::GA_HORIZON * sizeof(double));
            std::memcpy(sol_no.rt, out_rt, inoryy_ga::GA_HORIZON * sizeof(int));
            std::memcpy(sol_no.ba, out_ba,
                        inoryy_ga::GA_HORIZON * sizeof(double));
            std::memcpy(sol_no.bt, out_bt, inoryy_ga::GA_HORIZON * sizeof(int));
            sol_no.rs = inoryy_ga::GA_HORIZON;  // No shield
            sol_no.bs = out_bs;

            sol_sh = sol_no;
            sol_sh.rs = 0;  // Shield on turn 0

            double sc_no = inoryy_ga::V40ChampionFitnessEvaluator::evaluate(
                &sol_no, base_game, ctx);
            double sc_sh = inoryy_ga::V40ChampionFitnessEvaluator::evaluate(
                &sol_sh, base_game, ctx);

            double true_no = sc_no - (sol_no.rt[0] * 0.16);
            double true_sh = sc_sh + 5000.0;

            if (true_sh > true_no + 200.0) {
              r_shield = true;
              break;
            }
          }
        }
      }
    }

    if (turn_ == 1) {
      rtx = CX_[pnc[rp]];
      rty = CY_[pnc[rp]];
    }

    ActionOutput act_runner;
    act_runner.target_x = std::lround(rtx);
    act_runner.target_y = std::lround(rty);
    if (r_shield) {
      act_runner.thrust = -1;  // SHIELD
      shield_cooldown_[rp] = 3;
    } else if (r_boost) {
      act_runner.thrust = -2;  // BOOST
      boost_used_[rp] = true;
    } else {
      act_runner.thrust = (turn_ == 1) ? 200 : std::clamp(out_rt[0], 0, 200);
    }

    double b_da = std::clamp(out_ba[0], -18.0, 18.0);
    double b_ang_deg = std::fmod(pang[bp] + b_da, 360.0);
    if (b_ang_deg < 0) b_ang_deg += 360.0;
    double b_rad = b_ang_deg * inoryy_ga::DEG_TO_RAD;
    double btx = px[bp] + std::cos(b_rad) * 10000.0;
    double bty = py[bp] + std::sin(b_rad) * 10000.0;
    bool b_shield = (out_bs == 0 && shield_cooldown_[bp] == 0);
    if (!b_shield && shield_cooldown_[bp] == 0) {
      double tc = base_game.pods[bp].newCollide(base_game.pods[orp_abs],
                                                inoryy_ga::POD_RSQ, 1.0);
      if (tc >= 0.0 && tc <= 1.0) {
        double dx = base_game.pods[orp_abs].pos.x - base_game.pods[bp].pos.x;
        double dy = base_game.pods[orp_abs].pos.y - base_game.pods[bp].pos.y;
        double dvx = base_game.pods[bp].vel.x - base_game.pods[orp_abs].vel.x;
        double dvy = base_game.pods[bp].vel.y - base_game.pods[orp_abs].vel.y;
        double closing = (dx * dvx + dy * dvy);
        double closing_spd = closing / inoryy_ga::POD_DIAMETER;
        double dv_mag = std::hypot(dvx, dvy);

        if (closing > 0.0 && (closing_spd > 180.0 || dv_mag > 200.0)) {
          inoryy_ga::Ind sol_no, sol_sh;
          std::memcpy(sol_no.ra, out_ra,
                      inoryy_ga::GA_HORIZON * sizeof(double));
          std::memcpy(sol_no.rt, out_rt, inoryy_ga::GA_HORIZON * sizeof(int));
          std::memcpy(sol_no.ba, out_ba,
                      inoryy_ga::GA_HORIZON * sizeof(double));
          std::memcpy(sol_no.bt, out_bt, inoryy_ga::GA_HORIZON * sizeof(int));
          sol_no.rs = out_rs;
          sol_no.bs = inoryy_ga::GA_HORIZON;  // No shield

          sol_sh = sol_no;
          sol_sh.bs = 0;  // Shield on turn 0

          double sc_no = inoryy_ga::V40ChampionFitnessEvaluator::evaluate(
              &sol_no, base_game, ctx);
          double sc_sh = inoryy_ga::V40ChampionFitnessEvaluator::evaluate(
              &sol_sh, base_game, ctx);

          double true_no = sc_no - (sol_no.bt[0] * 0.01);
          double true_sh = sc_sh + 2000.0;

          if (true_sh > true_no) {
            b_shield = true;
          }
        }
      }
    }
    bool b_boost_flag = false;
    if (!boost_used_[bp] && !b_shield && shield_cooldown_[bp] == 0) {
      double dd = std::hypot(px[bp] - px[orp_abs], py[bp] - py[orp_abs]);
      if (dd > 5000.0) {
        double ba_t = fast_atan2(py[orp_abs] - py[bp], px[orp_abs] - px[bp]) *
                      inoryy_ga::RAD_TO_DEG;
        if (ba_t < 0) ba_t += 360.0;
        double ae_b = ba_t - pang[bp];
        while (ae_b > 180.0) ae_b -= 360.0;
        while (ae_b < -180.0) ae_b += 360.0;
        if (std::abs(ae_b) < 10.0) b_boost_flag = true;
      }
    }

    if (turn_ == 1) {
      btx = CX_[pnc[bp]];
      bty = CY_[pnc[bp]];
    }

    ActionOutput act_blocker;
    act_blocker.target_x = std::lround(btx);
    act_blocker.target_y = std::lround(bty);
    if (b_shield) {
      act_blocker.thrust = -1;  // SHIELD
      shield_cooldown_[bp] = 3;
    } else if (b_boost_flag) {
      act_blocker.thrust = -2;  // BOOST
      boost_used_[bp] = true;
    } else {
      int b_thr = (turn_ == 1) ? 200 : std::clamp(out_bt[0], 0, 200);
      if (turn_ > 1 && turn_ <= 5) {
        double b_dx_rb = CX_[rb % ncp_] - px[bp],
               b_dy_rb = CY_[rb % ncp_] - py[bp];
        double b_ta = fast_atan2(b_dy_rb, b_dx_rb) * inoryy_ga::RAD_TO_DEG;
        if (b_ta < 0) b_ta += 360.0;
        double b_ae = b_ta - pang[bp];
        while (b_ae > 180.0) b_ae -= 360.0;
        while (b_ae < -180.0) b_ae += 360.0;
        if (std::abs(b_ae) > 75.0) b_thr = 0;
      }
      act_blocker.thrust = b_thr;
    }

    int player_stall = std::min(runner_stuck_turns_[0], runner_stuck_turns_[1]);
    if (player_stall >= inoryy_ga::STALL_GUARD_TURNS) {
      double dr = std::hypot(px[rp] - CX_[pnc[rp]], py[rp] - CY_[pnc[rp]]);
      double db = std::hypot(px[bp] - CX_[pnc[bp]], py[bp] - CY_[pnc[bp]]);
      int sp = (db < dr) ? bp : rp;
      ActionOutput& stall_act = (sp == rp) ? act_runner : act_blocker;
      stall_act.target_x = std::lround(CX_[pnc[sp]]);
      stall_act.target_y = std::lround(CY_[pnc[sp]]);
      stall_act.thrust = 200;
    }

    struct timespec t_finish;
    clock_gettime(CLOCK_MONOTONIC, &t_finish);
    long el_turn = (t_finish.tv_sec - t_turn_start.tv_sec) * 1000000L +
                   (t_finish.tv_nsec - t_turn_start.tv_nsec) / 1000L;
    std::fprintf(stderr, "T%d %.2fms GA sims:%d\n", turn_, el_turn / 1000.0,
                 ga_sims);

    if (rp == 0)
      return {act_runner, act_blocker};
    else
      return {act_blocker, act_runner};
  }

 private:
  void compute_entry_points() {
    EX = CX_;
    EY = CY_;
    bool sharp = false;
    for (int i = 0; i < ncp_; i++) {
      int p = (i - 1 + ncp_) % ncp_, n = (i + 1) % ncp_;
      double v1x = CX_[i] - CX_[p], v1y = CY_[i] - CY_[p];
      double v2x = CX_[n] - CX_[i], v2y = CY_[n] - CY_[i];
      double l1 = std::hypot(v1x, v1y), l2 = std::hypot(v2x, v2y);
      double cos_theta = 1.0;
      if (l1 > 0 && l2 > 0)
        cos_theta =
            std::max(-1.0, std::min(1.0, (v1x * v2x + v1y * v2y) / (l1 * l2)));
      if (cos_theta < 0.2) sharp = true;
      double corner_cut = (ncp_ >= 5 || cos_theta < 0.2) ? 600.0 : 300.0;
      double shift = corner_cut * (1.0 - cos_theta) / 2.0;
      double max_shift = (cos_theta < -0.2) ? 180.0 : 250.0;
      if (shift > max_shift) shift = max_shift;
      double dx = CX_[p] - CX_[n], dy = CY_[p] - CY_[n],
             dd = std::hypot(dx, dy);
      if (dd > 0) {
        EX[i] = CX_[i] + shift * dx / dd;
        EY[i] = CY_[i] + shift * dy / dd;
      }
    }
    has_sharp_ = sharp;
    EX_ = EX;
    EY_ = EY;
  }

  void compute_dte() {
    mx_ = laps_ * ncp_ + 1;
    DTE_.assign(mx_, 0.0);
    for (int i = mx_ - 2; i >= 0; i--) {
      int c = i % ncp_, nc = (c + 1) % ncp_;
      DTE_[i] = DTE_[i + 1] + std::hypot(CX_[c] - CX_[nc], CY_[c] - CY_[nc]);
    }
  }

  void compute_ram_rest_points() {
    RRX_.resize(ncp_);
    RRY_.resize(ncp_);
    for (int i = 0; i < ncp_; i++) {
      int p = (i - 1 + ncp_) % ncp_, n = (i + 1) % ncp_;
      double dx = CX_[p] + CX_[n] - 2 * CX_[i];
      double dy = CY_[p] + CY_[n] - 2 * CY_[i];
      double d = std::hypot(dx, dy);
      if (d > 0) {
        RRX_[i] = CX_[i] + 1000.0 * dx / d;
        RRY_[i] = CY_[i] + 1000.0 * dy / d;
      } else {
        RRX_[i] = CX_[i];
        RRY_[i] = CY_[i];
      }
    }
  }

  void compute_exit_vectors() {
    out_dx_.resize(ncp_);
    out_dy_.resize(ncp_);
    out_ux_.resize(ncp_);
    out_uy_.resize(ncp_);
    out_len_.resize(ncp_);
    for (int i = 0; i < ncp_; i++) {
      int next_cp_id = (i + 1) % ncp_;
      double odx = EX_[next_cp_id] - EX_[i];
      double ody = EY_[next_cp_id] - EY_[i];
      double olen = std::hypot(odx, ody);
      out_dx_[i] = odx;
      out_dy_[i] = ody;
      out_len_[i] = olen;
      if (olen > 1.0) {
        out_ux_[i] = odx / olen;
        out_uy_[i] = ody / olen;
      } else {
        out_ux_[i] = 0.0;
        out_uy_[i] = 0.0;
      }
    }
  }

  void compute_weights() {
    double total_dist = 0;
    for (int i = 0; i < ncp_; i++) {
      total_dist += std::hypot(CX_[i] - CX_[(i + 1) % ncp_],
                               CY_[i] - CY_[(i + 1) % ncp_]);
    }
    double avg_dist = total_dist / ncp_;
    bool handling = (ncp_ >= 5 || has_sharp_) && avg_dist <= 6500.0;
    if (handling) {
      dw_ = 2.9;
      aw_ = 3.7;
      sw_ = 0.2;
      lw_ = 1.0;
      apw_ = 60.0;
      byw_ = 25.0;
      opw_ = 1.3;
      sfw_ = 50.0;
      faw_ = 25.0;
      rw_ = 0.15;
    } else {
      dw_ = 2.3;
      aw_ = 1.7;
      sw_ = 0.6;
      lw_ = 1.2;
      apw_ = 43.0;
      byw_ = 15.0;
      opw_ = 1.2;
      sfw_ = 40.0;
      faw_ = 20.0;
      rw_ = 0.15;
    }
  }

  int laps_ = 3;
  int ncp_ = 0;
  int mx_ = 0;
  int turn_ = 0;
  int prev_rp_ = -1;
  int prev_orp_abs_ = -1;
  bool has_sharp_ = false;
  bool has_prev_ = false;
  bool has_opp_prev_ = false;

  double dw_ = 2.3, aw_ = 1.7, sw_ = 0.6, lw_ = 1.2, apw_ = 43.0;
  double byw_ = 15.0, opw_ = 1.2, sfw_ = 40.0, faw_ = 20.0, rw_ = 0.15;

  std::vector<double> CX_, CY_, EX_, EY_, DTE_, RRX_, RRY_;
  std::vector<double> out_dx_, out_dy_, out_ux_, out_uy_, out_len_;
  std::vector<double> EX, EY;
  std::vector<inoryy_ga::Vec2> global_cp_buf_;
  int cp_buf_size_ = 0;

  double prev_ra_[inoryy_ga::GA_HORIZON] = {};
  int prev_rt_[inoryy_ga::GA_HORIZON] = {};
  double prev_ba_[inoryy_ga::GA_HORIZON] = {};
  int prev_bt_[inoryy_ga::GA_HORIZON] = {};
  int prev_rs_ = inoryy_ga::GA_HORIZON;
  int prev_bs_ = inoryy_ga::GA_HORIZON;

  double opp_prev_ra_[inoryy_ga::GA_HORIZON] = {};
  int opp_prev_rt_[inoryy_ga::GA_HORIZON] = {};
  double opp_prev_ba_[inoryy_ga::GA_HORIZON] = {};
  int opp_prev_bt_[inoryy_ga::GA_HORIZON] = {};

  int ml_[2] = {}, ol_[2] = {};
  int pmc_[2] = {}, poc_[2] = {};
  int runner_stuck_turns_[2] = {};
  int shield_cooldown_[2] = {};
  bool boost_used_[2] = {};

  inoryy_ga::RNG rng_;
};

}  // namespace inoryy_ga

int main() {
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(nullptr);

  int laps;
  if (!(std::cin >> laps)) return 0;
  int nc;
  std::cin >> nc;
  std::vector<std::pair<double, double>> cps(nc);
  for (int i = 0; i < nc; i++) {
    int x, y;
    std::cin >> x >> y;
    cps[i] = {static_cast<double>(x), static_cast<double>(y)};
  }

  inoryy_ga::ChampionBot bot;
  bot.init(laps, cps);

  while (true) {
    std::array<std::array<double, 6>, 4> pods;
    int x0;
    if (!(std::cin >> x0)) return 0;
    struct timespec t_turn_start;
    clock_gettime(CLOCK_MONOTONIC, &t_turn_start);
    int y0, vx0, vy0, a0, n0;
    if (!(std::cin >> y0 >> vx0 >> vy0 >> a0 >> n0)) return 0;
    pods[0] = {static_cast<double>(x0),  static_cast<double>(y0),
               static_cast<double>(vx0), static_cast<double>(vy0),
               static_cast<double>(a0),  static_cast<double>(n0)};
    for (int i = 1; i < 2; i++) {
      int x, y, vx, vy, a, n;
      if (!(std::cin >> x >> y >> vx >> vy >> a >> n)) return 0;
      pods[i] = {static_cast<double>(x),  static_cast<double>(y),
                 static_cast<double>(vx), static_cast<double>(vy),
                 static_cast<double>(a),  static_cast<double>(n)};
    }
    for (int i = 0; i < 2; i++) {
      int x, y, vx, vy, a, n;
      if (!(std::cin >> x >> y >> vx >> vy >> a >> n)) return 0;
      pods[i + 2] = {static_cast<double>(x),  static_cast<double>(y),
                     static_cast<double>(vx), static_cast<double>(vy),
                     static_cast<double>(a),  static_cast<double>(n)};
    }

    auto [act0, act1] = bot.get_actions(pods, t_turn_start);
    std::cout << act0.target_x << " " << act0.target_y << " "
              << act0.thrust_str() << "\n";
    std::cout << act1.target_x << " " << act1.target_y << " "
              << act1.thrust_str() << "\n";
    std::cout.flush();
  }
  return 0;
}

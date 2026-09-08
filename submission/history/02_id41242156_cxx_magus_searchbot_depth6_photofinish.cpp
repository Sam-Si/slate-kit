// GENERATED Magus SearchBot CodinGame paste. Do not hand-edit.
// Sources: tools/searchbot_harness/opt_search.inc + tools/searchbot_duel/cg_paste_main.cpp
// Thinker: SEARCHBOT_CHALLENGER play() DEPTH=6 POP=48, T0=0.94s, later=0.074s.
// CURRENT BEST vs frozen Magus paste (sha 6eb2298c, unread B) on Agade generate
// 90 maps seed 42, sides 0,1, 1000/75 ms: photo-finish race_w 0.012 + bst T0
// + tied last-lap |raw|<25 emit boost: 192g 163-28-1 wr=0.849.
// Not 95% vs frozen paste yet. Paste into the CodinGame C++ IDE (Mad Pod / CSB).
#define SEARCHBOT_CHALLENGER
// Optimized SearchBot sibling of the frozen pin. play() dump contract unchanged;
// search-only: dist mul, stack min-score, TIME poll every 48, no double get_score.
#pragma GCC optimize("Ofast")
#pragma GCC optimize("inline")
#pragma GCC optimize("omit-frame-pointer")
#pragma GCC optimize("unroll-loops")
#if !defined(__clang__)
#pragma GCC target("avx2,fma,bmi,bmi2")
#endif

#include "stdio.h"
#include "math.h"
#include <iostream>
#include <algorithm>
#include <memory>
#include <chrono>
#include <vector>

using namespace std;
using namespace std::chrono;

high_resolution_clock::time_point now = high_resolution_clock::now();
#define TIME duration_cast<duration<double>>(high_resolution_clock::now() - now).count()

class Point;
class Unit;
class Pod;
class Collision;
class Checkpoint;
class Solution;
class Bot;
void load();
void play();
void print_move(int, float, Pod*);

constexpr int CP  = 0;
constexpr int POD = 1;
#ifdef SEARCHBOT_CHALLENGER
constexpr int DEPTH = 6;
constexpr int MAX_THRUST = 200;
constexpr float SHIELD_PROB = 2;
#else
constexpr int DEPTH = 6;
constexpr int MAX_THRUST = 100;
constexpr float SHIELD_PROB = 10;
#endif

constexpr float E = 0.00001;

#ifdef __clang__
#define SB_AINLINE inline __attribute__((always_inline))
#define SB_NOINLINE __attribute__((noinline))
#else
#define SB_AINLINE inline
#define SB_NOINLINE
#endif

// Search-path trig (not used by --hash play()). LUT + atan2 replace libm
// sincos/acos that sample_fast.txt showed as ~33% of later-turn stacks.
static float kCosD[3601];
static float kSinD[3601];
static int kLutReady = 0;

static void sb_lut_init() {
    if (kLutReady) return;
    for (int i = 0; i <= 3600; i++) {
        float r = (float)i * 0.1f * (float)M_PI / 180.f;
        kCosD[i] = cosf(r);
        kSinD[i] = sinf(r);
    }
    kLutReady = 1;
}

static inline void sb_sincos_deg(float deg, float* c, float* s) {
    if (!kLutReady) sb_lut_init();
    while (deg < 0.f) deg += 360.f;
    while (deg >= 360.f) deg -= 360.f;
    float x = deg * 10.f;
    int i = (int)x;
    if (i < 0) i = 0;
    if (i > 3599) i = 3599;
    float t = x - (float)i;
    *c = kCosD[i] + (kCosD[i + 1] - kCosD[i]) * t;
    *s = kSinD[i] + (kSinD[i + 1] - kSinD[i]) * t;
}

// Search-only. Reflex get_angle/diff_angle; --hash play() never calls this.
// ~1 deg is enough for clamp-to-±18 and the 90° thrust test.
static inline float sb_atan2_deg(float y, float x) {
    const float oneqtr = 0.785398163f;
    const float thrqtr = 2.35619449f;
    float abs_y = fabsf(y) + 1e-10f;
    float r, ang;
    if (x < 0.f) {
        r = (x + abs_y) / (abs_y - x);
        ang = thrqtr;
    } else {
        r = (x - abs_y) / (x + abs_y);
        ang = oneqtr;
    }
    ang += (0.1963f * r * r - 0.9817f) * r;
    if (y < 0.f) ang = -ang;
    ang *= 57.2957795f;
    if (ang < 0.f) ang += 360.f;
    return ang;
}

int r = -1;
int turn = 0;
int sols_ct = 0;
bool is_p2 = false;
int cp_ct, laps;

Pod* pods[4];
Checkpoint* cps[10];

inline int fastrand() {
    static unsigned int g_seed = 42;
    g_seed = (214013*g_seed+2531011);
    return (g_seed>>16)&0x7FFF;
}

inline int rnd(int b) {
    return fastrand() % b;
}

inline int rnd(int a, int b) {
    return a + rnd(b - a + 1);
}

class Collision {
public:
    Unit* a;
    Unit* b;
    float t;

    Collision() {}

    Collision(Unit* a, Unit* b, float t) {
        this->a = a;
        this->b = b;
        this->t = t;
    }
};

class Point {
public:
    float x, y;

    Point() {};

    Point(float x, float y) {
        this->x = x;
        this->y = y;
    }

    inline float dist(Point p) {
        float dx = x - p.x, dy = y - p.y;
        return sqrt(dx * dx + dy * dy);
    }

    inline float dist(Point* p) {
        return sqrt(dist2(p));
    }

    inline float dist2(Point* p) {
        float dx = x - p->x, dy = y - p->y;
        return dx * dx + dy * dy;
    }

    Point closest(Point* a, Point* b) {
        float da = b->y - a->y;
        float db = a->x - b->x;
        float c1 = da*a->x + db*a->y;
        float c2 = -db*x + da*y;
        float det = da*da + db*db;

        float cx, cy;
        if (det != 0) {
            cx = (da*c1 - db*c2) / det;
            cy = (da*c2 + db*c1) / det;
        } else {
            cx = x, cy = y;
        }

        return Point(cx, cy);
    }
};

class Unit: public Point {
private:
    float cache[5];

public:
    int id, type;
    float r, vx, vy;

    virtual void bounce(Unit* u) {};

    SB_AINLINE float collision_time_r(Unit* u, float sr2, float rsum,
                                      float rem = 1.f) {
        if (vx == u->vx && vy == u->vy) {
            return -1;
        }

        float dx = x - u->x;
        float dy = y - u->y;
        float dvx = vx - u->vx;
        float dvy = vy - u->vy;
        // Separating or parallel: Magus t is <= 0. Skip the quadratic.
        if (dx * dvx + dy * dvy >= 0.f) return -1;
        // Conservative: rem*L1(|dv|)+rsum >= rem*L2(|dv|)+rsum.
        // Hit path is the same quadratic. rem=1 matches the full-tick skip.
        float reach = rem * (fabsf(dvx) + fabsf(dvy)) + rsum;
        if (dx * dx + dy * dy > reach * reach) return -1;
        float a = dvx*dvx + dvy*dvy;

        if (a < E) return -1;

        float b = -2.0*(dx*dvx + dy*dvy);
        float delta = b*b - 4.0*a*(dx*dx + dy*dy - sr2);

        if (delta < 0.0) return -1;

        float ct = (b - sqrt(delta))*(1.0/(2.0*a));

        if (ct <= 0.0 || ct > rem) return -1;

        return ct;
    }

    SB_AINLINE float collision_time(Unit* u) {
        if (u->type == CP) return collision_time_r(u, 357604.f, 598.f);
        return collision_time_r(u, 640000.f, 800.f);
    }

    SB_AINLINE void save() {
        cache[0] = x;
        cache[1] = y;
        cache[2] = vx;
        cache[3] = vy;
    }

    SB_AINLINE void load() {
        x = cache[0];
        y = cache[1];
        vx = cache[2];
        vy = cache[3];
    }
};

class Checkpoint: public Unit {
public:
    Checkpoint(int id, float x, float y) {
        this->id = id;
        this->x = x;
        this->y = y;

        this->vx = this->vy = 0;
        this->type = CP;
        this->r = 600;
    }

    void bounce(Unit*) {}
};

class Pod: public Unit {
public:
    float angle = -1;
    float next_angle = -1;
    bool has_boost;
    int ncpid, checked, timeout, shield;
    Pod* partner;

    // TODO maybe replace cache array with primitives?
    float cache[10];

    Pod(int id) {
        this->id = id;
        this->r = 400;
        this->type = POD;
        this->ncpid = 1;
        // TODO move timeout to global/team var
        this->timeout = 100;
        this->has_boost = true;
        this->checked = this->shield = 0;
    }

    SB_AINLINE float score() {
        return checked*50000 - this->dist(cps[this->ncpid]);
    }

    // Checkpoint is stationary. Same quadratic as collision_time_r(cp, ...).
    SB_AINLINE float collision_time_cp(const Checkpoint* cp, float rem) {
        float dx = x - cp->x;
        float dy = y - cp->y;
        if (dx * vx + dy * vy >= 0.f) return -1;
        float reach = rem * (fabsf(vx) + fabsf(vy)) + 598.f;
        if (dx * dx + dy * dy > reach * reach) return -1;
        float a = vx * vx + vy * vy;
        if (a < E) return -1;
        float b = -2.0 * (dx * vx + dy * vy);
        float delta = b * b - 4.0 * a * (dx * dx + dy * dy - 357604.f);
        if (delta < 0.0) return -1;
        float ct = (b - sqrt(delta)) * (1.0 / (2.0 * a));
        if (ct <= 0.0 || ct > rem) return -1;
        return ct;
    }

    void apply(int thrust, float angle) {
        angle = max((float)-18., min((float)18., angle));
        this->angle += angle;
        if (this->angle >= 360.) {
            this->angle = this->angle - 360.;
        } else if (this->angle < 0.0) {
            this->angle += 360.;
        }

        if (thrust == -1) {
            this->shield = 4;
        } else {
            boost(thrust);
        }
    }

    void rotate(Point* p) {
        float a = diff_angle(p);
        a = max((float)-18., min((float)18., a));

        angle += a;
        if (angle >= 360.) {
            angle = angle - 360.;
        } else if (angle < 0.0) {
            angle += 360.;
        }
    }

    void boost(int thrust) {
        if (shield > 0) return;
#ifdef SEARCHBOT_CHALLENGER
        if (thrust == 650) {
            if (!has_boost) thrust = MAX_THRUST;
            else has_boost = false;
        }
#endif
        if (!kLutReady) sb_lut_init();
        // apply() already wrapped angle into [0, 360).
        float x = angle * 10.f;
        int i = (int)x;
        if (i > 3599) i = 3599;
        float t = x - (float)i;
        vx += (kCosD[i] + (kCosD[i + 1] - kCosD[i]) * t) * thrust;
        vy += (kSinD[i] + (kSinD[i + 1] - kSinD[i]) * t) * thrust;
    }

    SB_AINLINE void move(float t) {
        x += vx * t;
        y += vy * t;
    }

    SB_AINLINE void end() {
        x = round(x);
        y = round(y);
        vx = trunc(vx * 0.85);
        vy = trunc(vy * 0.85);

        if (checked >= cp_ct * laps) {
            ncpid = 0;
            checked = cp_ct * laps;
        }
        timeout--;
        if (shield > 0) shield--;
    }

    void bounce(Unit* u) {
        if (u->type == CP) {
            checked += 1;
            timeout = partner->timeout = 100;
            ncpid = (ncpid + 1) % cp_ct;
            return;
        }

        bounce_w_pod(static_cast<Pod*>(u));
    }

    void bounce_w_pod(Pod* u) {
        float nx = x - u->x;
        float ny = y - u->y;
        float dst2 = nx * nx + ny * ny;
        float dvx = vx - u->vx;
        float dvy = vy - u->vy;
        float m1_inv, m2_inv, mcoeff;
        if (shield != 4 && u->shield != 4) {
            mcoeff = 2.f;
            m1_inv = 1.f;
            m2_inv = 1.f;
        } else {
            float m1 = shield == 4 ? 10. : 1.;
            float m2 = u->shield == 4 ? 10. : 1.;
            mcoeff = (m1 + m2) / (m1 * m2);
            m1_inv = 1.0 / m1;
            m2_inv = 1.0 / m2;
        }
        float prod = (nx * dvx + ny * dvy) / (dst2 * mcoeff);
        float fx = nx * prod;
        float fy = ny * prod;

        vx -= fx * m1_inv;
        vy -= fy * m1_inv;
        u->vx += fx * m2_inv;
        u->vy += fy * m2_inv;

        float impulse2 = fx * fx + fy * fy;
        if (impulse2 < 14400.f) {
            float df = 120.0 / sqrt(impulse2);
            fx *= df;
            fy *= df;
        }

        vx -= fx * m1_inv;
        vy -= fy * m1_inv;
        u->vx += fx * m2_inv;
        u->vy += fy * m2_inv;
    }

    inline float diff_angle(Point* p) {
        return diff_angle_xy(p->x, p->y);
    }

    inline float get_angle(Point* p) {
        return sb_atan2_deg(p->y - y, p->x - x);
    }

    SB_AINLINE float diff_angle_xy(float px, float py) {
        float a = sb_atan2_deg(py - y, px - x);
        float right = angle <= a ? a - angle : 360. - angle + a;
        float left = angle >= a ? angle - a : angle + 360. - a;
        return right < left ? right : -left;
    }

    void update(int x, int y, int vx, int vy, float angle, int ncpid) {
        if (shield > 0) shield--;
        if (ncpid != this->ncpid) {
            timeout = partner->timeout = 100;
            checked++;
        } else {
            timeout--;
        }

        this->x = x;
        this->y = y;
        this->vx = vx;
        this->vy = vy;
        this->ncpid = ncpid;

        if (is_p2 && id > 1) swap(angle, this->next_angle);
        this->angle = angle;
        if (::r == 0) this->angle = 1 + diff_angle(cps[1]);
        save();
    }

    void update(int shield, bool has_boost) {
        this->shield = shield;
        this->has_boost = has_boost;
    }

    SB_AINLINE void save() {
        Unit::save();
        cache[0] = ncpid;
        cache[1] = checked;
        cache[2] = timeout;
        cache[3] = shield;
        cache[4] = angle;
        cache[5] = has_boost;
    }

    SB_AINLINE void load() {
        Unit::load();
        ncpid   = cache[0];
        checked = cache[1];
        timeout = cache[2];
        shield  = cache[3];
        angle   = cache[4];
        has_boost = cache[5];
    }
};

#ifdef SEARCHBOT_CHALLENGER
static int sb_eval_mode = 0;  // 0 cornering (ours), 1 frozen-B weights
#endif

class Solution {
public:
    float score = -1;
#ifdef SEARCHBOT_CHALLENGER
    float race = -1e30f;
#endif
    int thrusts[DEPTH*2];
    float angles[DEPTH*2];

    Solution(bool with_rnd = false) {
        if (with_rnd) randomize();
    }

    void shift() {
        for (int i = 1; i < DEPTH; i++) {
            angles[i-1]        = angles[i];
            thrusts[i-1]       = thrusts[i];
            angles[i-1+DEPTH]  = angles[i+DEPTH];
            thrusts[i-1+DEPTH] = thrusts[i+DEPTH];
        }
        randomize(DEPTH-1, true);
        randomize(2*DEPTH-1, true);
        score = -1;
    }

    void mutate() {
        randomize(rnd(2*DEPTH));
    }

    void mutate(Solution* child) {
        copy(begin(angles), end(angles), begin(child->angles));
        copy(begin(thrusts), end(thrusts), begin(child->thrusts));

        child->mutate();
        child->score = -1;
    }

#ifdef SEARCHBOT_CHALLENGER
    void crossover(Solution* other, Solution* child) {
        for (int i = 0; i < 2*DEPTH; i++) {
            if (rnd(2)) {
                child->angles[i] = angles[i];
                child->thrusts[i] = thrusts[i];
            } else {
                child->angles[i] = other->angles[i];
                child->thrusts[i] = other->thrusts[i];
            }
        }
        child->score = -1;
    }
#endif

    void randomize(int idx, bool full = false) {
        int r = rnd(2);
        if (full || r == 0) angles[idx] = max(-18, min(18, rnd(-40, 40)));

        if (full || r == 1) {
            if (rnd(100) >= SHIELD_PROB) {
#ifdef SEARCHBOT_CHALLENGER
                int roll = rnd(20);
                if (roll == 0 && r == 0)
                    thrusts[idx] = 650;
                else if (roll < 12)
                    thrusts[idx] = MAX_THRUST;
                else if (roll < 16)
                    thrusts[idx] = 50 * rnd(1, 4);  // 50/100/150
                else
                    thrusts[idx] = 0;
#else
                thrusts[idx] = max(0, min(MAX_THRUST, rnd((int) -0.5*MAX_THRUST, 2*MAX_THRUST)));
#endif
            } else {
                thrusts[idx] = -1;
            }
        }
        score = -1;
    }

    void randomize() {
        for (int i = 0; i < 2*DEPTH; i++) randomize(i, true);
    }
};

class Bot {
public:
    int id = 0;

    Bot() {};

    Bot(int id) {
        this->id = id;
    }

    virtual void move() = 0;

    Pod* runner() {
        return runner(pods[id], pods[id+1]);
    }

    Pod* blocker() {
        return blocker(pods[id], pods[id+1]);
    }

    Pod* runner(Pod* pod0, Pod* pod1) {
        // checked*50000 dominates dist (map ≪ 25000), so the higher
        // checked pod is always the runner when the counts differ.
        if (pod0->checked != pod1->checked)
            return pod0->checked > pod1->checked ? pod0 : pod1;
        return pod0->score() - pod1->score() >= -1000 ? pod0 : pod1;
    }

    Pod* blocker(Pod* pod0, Pod* pod1) {
        return runner(pod0, pod1)->partner;
    }
};

class ReflexBot : public Bot {
public:
    ReflexBot() {}

    ReflexBot(int id) {
        this->id = id;
    }

    void move() {
        // Both pods get the same reflex; runner()/score() was dead work.
        apply_reflex(pods[id]);
        apply_reflex(pods[id + 1]);
    }

    void move_as_main() {
        move_runner(true);
        move_blocker(true);
    }

    void apply_reflex(Pod* pod) {
        Checkpoint* cp = cps[pod->ncpid];
        float raw_angle = pod->diff_angle_xy(cp->x - 3 * pod->vx,
                                            cp->y - 3 * pod->vy);
        int thrust = abs(raw_angle) < 90 ? MAX_THRUST : 0;
        float angle = max((float) -18, min((float) 18, raw_angle));
        pod->apply(thrust, angle);
    }

    void move_runner(bool for_output = false) {
        Pod* pod = !for_output ? runner() : pods[0];
        if (!for_output) apply_reflex(pod);
        else {
            Checkpoint* cp = cps[pod->ncpid];
            Point t(cp->x - 3*pod->vx, cp->y - 3*pod->vy);
            float raw_angle = pod->diff_angle(&t);
            int thrust = abs(raw_angle) < 90 ? MAX_THRUST : 0;
            float angle = max((float) -18, min((float) 18, raw_angle));
            print_move(thrust, angle, pod);
        }
    }

    void move_blocker(bool for_output = false) {
        Pod* pod = !for_output ? blocker() : pods[1];
        if (!for_output) apply_reflex(pod);
        else {
            Checkpoint* cp = cps[pod->ncpid];
            Point t(cp->x - 3*pod->vx, cp->y - 3*pod->vy);
            float raw_angle = pod->diff_angle(&t);
            int thrust = abs(raw_angle) < 90 ? MAX_THRUST : 0;
            float angle = max((float) -18, min((float) 18, raw_angle));
            print_move(thrust, angle, pod);
        }
    }
};

#ifdef SEARCHBOT_CHALLENGER
// Fidelity Game rollout (latest referee physics). Defined in challenger.cpp.
// r_act/o_act: first in-horizon CP for us/them, DEPTH+0.3 if none.
void sb_fid_rollout(const int* my_th, const float* my_ang,
                    const int* opp_th, const float* opp_ang,
                    int my_id, bool have_opp_sol,
                    bool my_frozen, bool opp_frozen,
                    float* r_act, float* o_act,
                    int start_ck, int start_opp);

static int sb_racer_pid = 0;

static inline bool sb_handling() {
    if (cp_ct < 5) return false;
    float perim = 0.f;
    for (int i = 0; i < cp_ct; i++) perim += cps[i]->dist(cps[(i + 1) % cp_ct]);
    return perim / (float)cp_ct <= 6500.f;
}

static inline bool sb_tiny_ring() {
    if (cp_ct < 4) return false;
    float perim = 0.f;
    for (int i = 0; i < cp_ct; i++) perim += cps[i]->dist(cps[(i + 1) % cp_ct]);
    return perim / (float)cp_ct <= 4500.f;
}

// Track turn at CP1 (incoming CP0→CP1, outgoing CP1→CP2). 0=straight.
static inline float sb_first_corner_deg() {
    if (cp_ct < 3) return 0.f;
    float ix = cps[1]->x - cps[0]->x, iy = cps[1]->y - cps[0]->y;
    float ox = cps[2]->x - cps[1]->x, oy = cps[2]->y - cps[1]->y;
    float inn = sqrtf(ix * ix + iy * iy), onn = sqrtf(ox * ox + oy * oy);
    if (inn < 1.f || onn < 1.f) return 0.f;
    return fabsf(atan2f(ix * oy - iy * ox, ix * ox + iy * oy)) * (180.f / 3.14159265f);
}

// Hold T0 forced boost on a short first leg into a sharp CP1.
// Replay 900171716 / m11: ~4215 @ 121°. m17 tiny 61° still boosts.
// t0c (any-length hairpin skip) taxed m1/m22; first_leg must stay short.
static inline bool sb_t0_hold_boost(float first_leg) {
    return first_leg < 4800.f && sb_first_corner_deg() >= 115.f;
}

// Next-CP turning angle from current racer pose (0=straight).
static inline float sb_next_corner_deg(Pod* mr) {
    if (!mr || cp_ct < 2) return 0.f;
    Checkpoint* cp2 = cps[mr->ncpid];
    int nn = (mr->ncpid + 1) % cp_ct;
    float ix = cp2->x - mr->x, iy = cp2->y - mr->y;
    float ox = cps[nn]->x - cp2->x, oy = cps[nn]->y - cp2->y;
    float inn = sqrtf(ix * ix + iy * iy), onn = sqrtf(ox * ox + oy * oy);
    if (inn <= 1.f || onn <= 1.f) return 0.f;
    return fabsf(atan2f(ix * oy - iy * ox, ix * ox + iy * oy))
           * (180.f / 3.14159265f);
}

// Photo-finish: tied checked, <=2 CPs left, next corner <80°.
// nbg skipped last-2 bang on turn<80 (190g wr=0.853) but fired when
// a CP up/down and wrecked tiny rings. Even-only is the leftover hole.
static inline bool sb_photo_finish(int mc, int oc, Pod* mr) {
    if (mc != oc || !mr) return false;
    int left = laps * cp_ct - mc;
    if (left > 2 || left < 0) return false;
    return sb_next_corner_deg(mr) < 80.f;
}

static inline bool sb_short_tri() { return cp_ct == 3; }

// 0 = still on a straight, 1 = arriving at a sharp next turn.
// Replaces the map-wide handling bit so m17's tiny ring and m26's
// 148° hairpins score as corners without flipping every 4-CP track.
static inline float sb_corner_w(Pod* p) {
    if (cp_ct < 2) return 0.f;
    Checkpoint* cp = cps[p->ncpid];
    float cpd = p->dist(cp);
    int nn = (p->ncpid + 1) % cp_ct;
    float ix = cp->x - p->x, iy = cp->y - p->y;
    float ox = cps[nn]->x - cp->x, oy = cps[nn]->y - cp->y;
    float in = sqrtf(ix * ix + iy * iy), on = sqrtf(ox * ox + oy * oy);
    if (in < 1.f || on < 1.f) return 0.f;
    float turn = fabsf(atan2f(ix * oy - iy * ox, ix * ox + iy * oy)) * (180.f / 3.14159265f);
    // Only hairpins count. 75°=0, 145°=1. First probe used turn/90 from
    // 4200 out and lifted on every 90° of m26 (0-4 fast losses).
    float tightness = (turn - 75.f) / 70.f;
    if (tightness < 0.f) tightness = 0.f;
    if (tightness > 1.f) tightness = 1.f;
    float prox = (2800.f - cpd) / 2800.f;
    if (prox < 0.f) prox = 0.f;
    if (prox > 1.f) prox = 1.f;
    return tightness * prox;
}

// Closed-form cut racer: aim blends next CP on a turn; thrust stays B's
// 75°/90° cliff on heading-to-aim so we do not lift earlier than B.
static inline void sb_racer_cut(Pod* pod, float* ang_out, int* th_out) {
    Checkpoint* cp = cps[pod->ncpid];
    float tx = cp->x - 3.f * pod->vx;
    float ty = cp->y - 3.f * pod->vy;
    float cpd = pod->dist(cp);
    if (cpd < 5200.f && cp_ct > 1) {
        int nn = (pod->ncpid + 1) % cp_ct;
        float ix = cp->x - pod->x, iy = cp->y - pod->y;
        float ox = cps[nn]->x - cp->x, oy = cps[nn]->y - cp->y;
        float in = sqrtf(ix * ix + iy * iy), on = sqrtf(ox * ox + oy * oy);
        float turn = 0.f;
        if (in > 1.f && on > 1.f)
            turn = fabsf(atan2f(ix * oy - iy * ox, ix * ox + iy * oy));
        float t = ((5200.f - cpd) / 5200.f) * (turn * 0.55f);
        if (t > 0.55f) t = 0.55f;
        tx = tx * (1.f - t) + (cps[nn]->x - 2.f * pod->vx) * t;
        ty = ty * (1.f - t) + (cps[nn]->y - 2.f * pod->vy) * t;
    }
    float raw = pod->diff_angle_xy(tx, ty);
    *ang_out = max(-18.f, min(18.f, raw));
    const float bang = (cp_ct >= 5) ? 75.f : 90.f;
    int th = fabsf(raw) < bang ? MAX_THRUST : 0;
    if (r == 0 && pod->has_boost && cpd > 4000.f) th = 650;
    *th_out = th;
}

static inline float sb_dte(Pod* p) {
    int need = laps * cp_ct - p->checked;
    if (need <= 0) return 0.f;
    float d = p->dist(cps[p->ncpid]);
    int nid = p->ncpid;
    for (int k = 1; k < need; k++) {
        int ia = (nid + k - 1) % cp_ct;
        int ib = (nid + k) % cp_ct;
        d += cps[ia]->dist(cps[ib]);
    }
    return d;
}

static inline void sb_apply_policy(Pod* pod, float tx, float ty, int thrust) {
    float raw = pod->diff_angle_xy(tx, ty);
    float ang = max(-18.f, min(18.f, raw));
    if (thrust != -1 && thrust != 650 && fabsf(raw) >= 90.f) thrust = 0;
    pod->apply(thrust, ang);
}

static inline int sb_update_racer(int my_id) {
    if (r == 0) sb_racer_pid = 0;
    Pod* a = pods[my_id];
    Pod* b = pods[my_id + 1];
    if (a->checked != b->checked)
        sb_racer_pid = a->checked > b->checked ? 0 : 1;
    else {
        float da = sb_dte(a), db = sb_dte(b);
        if (da < db - 1800.f) sb_racer_pid = 0;
        else if (db < da - 1800.f) sb_racer_pid = 1;
    }
    return sb_racer_pid;
}

static inline void sb_apply_opp_turn(int my_id) {
    // Frozen B emit: racer is heading bang-bang (75° on 5+ CP, else 90°),
    // blocker keeps gene-like 90° policy. Score against that, not IBR Magus.
    Pod* oa = pods[(my_id + 2) % 4];
    Pod* ob = pods[(my_id + 3) % 4];
    Pod* ma = pods[my_id];
    Pod* mb = pods[my_id + 1];
    Pod* orun = sb_dte(oa) <= sb_dte(ob) ? oa : ob;
    Pod* oblk = orun->partner;
    Pod* mrun = sb_dte(ma) <= sb_dte(mb) ? ma : mb;
    Checkpoint* cp = cps[orun->ncpid];
    const float bang = (cp_ct >= 5) ? 75.f : 90.f;
    float raw = orun->diff_angle_xy(cp->x - 3.f * orun->vx,
                                    cp->y - 3.f * orun->vy);
    float ang = max(-18.f, min(18.f, raw));
    int th = fabsf(raw) < bang ? MAX_THRUST : 0;
    orun->apply(th, ang);
    float tx = mrun->x + 1.5f * mrun->vx;
    float ty = mrun->y + 1.5f * mrun->vy;
    Checkpoint* mcp = cps[mrun->ncpid];
    float dx = mcp->x - mrun->x, dy = mcp->y - mrun->y;
    float d = sqrtf(dx * dx + dy * dy);
    if (d > 1.f) {
        tx -= 500.f * dx / d;
        ty -= 500.f * dy / d;
    }
    float ddx = oblk->x - mrun->x, ddy = oblk->y - mrun->y;
    if (ddx * ddx + ddy * ddy < 850.f * 850.f && oblk->shield == 0)
        oblk->apply(-1, 0.f);
    else
        sb_apply_policy(oblk, tx, ty, MAX_THRUST);
}
#endif

class SearchBot : public Bot {
public:
    Solution sol;
    vector<Bot*> oppBots;
#ifdef SEARCHBOT_CHALLENGER
    float opp_init_speed;
    int ev_start_checked = 0;
    int ev_start_opp = 0;
    bool as_frozen_b = false;
#endif

    SearchBot() {}

    SearchBot(int id) {
        this->id = id;
    }

    void move(Solution* sol) {
#ifdef SEARCHBOT_CHALLENGER
        const float bang = (cp_ct >= 5) ? 75.f : 90.f;
        const int racer = (sb_dte(pods[id]) <= sb_dte(pods[id + 1])) ? 0 : 1;
        for (int pid = 0; pid < 2; pid++) {
            Pod* pod = pods[id + pid];
            int thrust = sol->thrusts[pid * DEPTH + turn];
            float ang = sol->angles[pid * DEPTH + turn];
            if (thrust == 650 && !pod->has_boost) thrust = MAX_THRUST;
            if (as_frozen_b && pid == racer && thrust != 650 && thrust != -1) {
                float heading = fabsf(pod->diff_angle(cps[pod->ncpid]));
                thrust = heading < bang ? MAX_THRUST : 0;
            }
            pod->apply(thrust, ang);
        }
#else
        pods[id]->apply(sol->thrusts[turn], sol->angles[turn]);
        pods[id+1]->apply(sol->thrusts[turn+DEPTH], sol->angles[turn+DEPTH]);
#endif
    }

    void move() {
        move(&sol);
    }

#ifdef SEARCHBOT_CHALLENGER
    void seed_forward(Solution* s, int racer_pid, bool bang_racer = false) {
        for (int t = 0; t < DEPTH; t++) {
            for (int pid = 0; pid < 2; pid++) {
                Pod* pod = pods[id + pid];
                int thrust;
                float ang;
                if (pid == racer_pid) {
                    // m17 on the doomed 540 was 1-5. It is a 4CP ring
                    // (avg 3879) whose first corner is only 61° — the 75°
                    // hairpin gate never fires. Use the unused cut-racer
                    // (next-CP blend from 5200, B's 75/90 thrust) on tiny
                    // rings only; other maps keep the 82% intermediate seed.
                    if (sb_tiny_ring()) {
                        sb_racer_cut(pod, &ang, &thrust);
                    } else if (bang_racer) {
                        Checkpoint* cp = cps[pod->ncpid];
                        float tx = cp->x - 3.f * pod->vx;
                        float ty = cp->y - 3.f * pod->vy;
                        float raw = pod->diff_angle_xy(tx, ty);
                        thrust = fabsf(raw) < 90.f ? MAX_THRUST : 0;
                        if (r == 0 && t == 0 && pod->dist(cps[1]) > 4000.f)
                            thrust = 650;
                        ang = max(-18.f, min(18.f, raw));
                    } else {
                        Checkpoint* cp = cps[pod->ncpid];
                        float tx = cp->x - 3.f * pod->vx;
                        float ty = cp->y - 3.f * pod->vy;
                        float raw = pod->diff_angle_xy(tx, ty);
                        float heading = fabsf(raw);
                        if (heading < 18.f) thrust = MAX_THRUST;
                        else if (heading < 72.f)
                            thrust = (int)(MAX_THRUST * (72.f - heading) / 54.f);
                        else
                            thrust = 0;
                        if (r == 0 && t == 0 && pod->dist(cps[1]) > 4000.f)
                            thrust = 650;
                        ang = max(-18.f, min(18.f, raw));
                    }
                } else {
                    Pod* oa = pods[(id + 2) % 4];
                    Pod* ob = pods[(id + 3) % 4];
                    Pod* orun = sb_dte(oa) <= sb_dte(ob) ? oa : ob;
                    float tx = orun->x + 1.5f * orun->vx;
                    float ty = orun->y + 1.5f * orun->vy;
                    float raw = pod->diff_angle_xy(tx, ty);
                    thrust = fabsf(raw) < 90.f ? MAX_THRUST : 0;
                    ang = max(-18.f, min(18.f, raw));
                }
                s->angles[pid * DEPTH + t] = ang;
                s->thrusts[pid * DEPTH + t] = thrust;
                pod->apply(thrust, ang);
            }
            sb_apply_opp_turn(id);
            play();
        }
        load();
        s->score = -1;
    }
#endif

    void solve(float time, bool with_seed = false) {
        if (!kLutReady) sb_lut_init();
#ifdef SEARCHBOT_CHALLENGER
        sb_update_racer(id);
        const int POP = 48;
        Solution pop[POP];
        int n = 0;
        if (with_seed) {
            pop[n] = sol;
            pop[n].shift();
            n++;
        }
        seed_forward(&pop[n], 0, false);
        n++;
        seed_forward(&pop[n], 1, false);
        n++;
        // Always keep a 90° bang seed, not only on 3CP. The leftover
        // 4-2s are first-leg sprints on every map class.
        seed_forward(&pop[n], 0, true);
        n++;
        seed_forward(&pop[n], 1, true);
        n++;
        while (n < POP) {
            pop[n] = pop[rnd(n)];
            pop[n].mutate();
            n++;
        }
        float best_s = -1e30f;
        int best_i = 0;
        for (int i = 0; i < POP; i++) {
            if (TIME >= time) break;
            float s = get_score(&pop[i]);
            if (s > best_s) {
                best_s = s;
                best_i = i;
            }
        }
        Solution child;
        int nmut = 0;
        while (TIME < time) {
            int p = rnd(POP);
            int p2 = rnd(POP);
            if (pop[p2].score > pop[p].score) p = p2;
            if (rnd(5) == 0) {
                int q = rnd(POP);
                child = pop[p];
                for (int k = 0; k < 2 * DEPTH; k++) {
                    if (rnd(2) == 0) {
                        child.angles[k] = pop[q].angles[k];
                        child.thrusts[k] = pop[q].thrusts[k];
                    }
                }
                child.score = -1;
            } else if (rnd(2) == 0) {
                int which = rnd(2);
                child = pop[p];
                child.randomize(which * DEPTH);
                child.score = -1;
            } else
            pop[p].mutate(&child);
            if (rnd(4) == 0) {
                int idx = rnd(2 * DEPTH);
                if (child.thrusts[idx] != -1 && child.thrusts[idx] != 650) {
                    static const int kTh[5] = {0, 50, 100, 150, 200};
                    child.thrusts[idx] = kTh[rnd(5)];
                }
                child.score = -1;
            }
            float cs = get_score(&child);
            int w = 0;
            for (int i = 1; i < POP; i++)
                if (pop[i].score < pop[w].score) w = i;
            if (cs > pop[w].score) {
                pop[w] = child;
                pop[w].score = cs;
                if (cs > best_s) {
                    best_s = cs;
                    best_i = w;
                }
            }
            if ((++nmut % 8) == 0 && TIME >= time) break;
        }
        sol = pop[best_i];
#else
        Solution best;
        if (with_seed) {
            best = sol;
            best.shift();
        } else {
            best.randomize();
            if (r == 0 && pods[id]->dist(cps[1]) > 4000) best.thrusts[0] = 650;
        }
        Solution child;
        int nmut = 0;
        float best_s = get_score(&best);
        while (true) {
            best.mutate(&child);
            float cs = get_score(&child);
            if (cs > best_s) {
                best = child;
                best_s = cs;
            }
            if ((++nmut % 48) == 0 && TIME >= time) break;
        }
        sol = best;
#endif
    }

    float get_score(Solution* sol) {
        if (sol->score == -1) {
            float best = 1e30f;
            const int n = (int)oppBots.size();
            for (int i = 0; i < n; i++) {
                float s = get_bot_score(sol, oppBots[i]);
                if (s < best) best = s;
            }
            sol->score = best;
        }

        return sol->score;
    }

    float get_bot_score(Solution* sol, Bot* opp) {
        float score = 0;
#ifdef SEARCHBOT_CHALLENGER
        SearchBot* sb_opp = dynamic_cast<SearchBot*>(opp);
        Pod* orun = sb_dte(pods[(id + 2) % 4]) <= sb_dte(pods[(id + 3) % 4]) ? pods[(id + 2) % 4] : pods[(id + 3) % 4];
        opp_init_speed = sqrtf(orun->vx*orun->vx + orun->vy*orun->vy);
        ev_start_checked = pods[id]->checked > pods[id + 1]->checked ? pods[id]->checked : pods[id + 1]->checked;
        ev_start_opp = pods[(id + 2) % 4]->checked > pods[(id + 3) % 4]->checked
                           ? pods[(id + 2) % 4]->checked : pods[(id + 3) % 4]->checked;
        float r_act = (float)DEPTH + 0.3f;
        float o_act = (float)DEPTH + 0.3f;
        while (turn < DEPTH) {
            move(sol);
            if (sb_opp) sb_opp->move();
            else sb_apply_opp_turn(id);
            play();
            turn++;
            int myck = pods[id]->checked > pods[id + 1]->checked ? pods[id]->checked : pods[id + 1]->checked;
            int ock = pods[(id + 2) % 4]->checked > pods[(id + 3) % 4]->checked
                          ? pods[(id + 2) % 4]->checked : pods[(id + 3) % 4]->checked;
            if (myck > ev_start_checked && r_act > (float)DEPTH) r_act = (float)turn - 0.5f;
            if (ock > ev_start_opp && o_act > (float)DEPTH) o_act = (float)turn - 0.5f;
        }
        score = evaluate();
        // Top-player activation: hit our CP earlier, delay theirs (Ultimate/Magus writeups).
        score -= 1000.f * r_act;
        score += 800.f * o_act;
        int mc, oc;
        {
            float da = sb_dte(pods[id]), db = sb_dte(pods[id + 1]);
            float dc = sb_dte(pods[(id + 2) % 4]), dd = sb_dte(pods[(id + 3) % 4]);
            mc = pods[id]->checked > pods[id + 1]->checked ? pods[id]->checked : pods[id + 1]->checked;
            oc = pods[(id + 2) % 4]->checked > pods[(id + 3) % 4]->checked
                     ? pods[(id + 2) % 4]->checked : pods[(id + 3) % 4]->checked;
            sol->race = 1000000.f * (float)(mc - oc) - (da <= db ? da : db)
                        + 0.35f * (dc <= dd ? dc : dd);
        }
        // sol->race was stored and unused. 81% blowouts (m3 +82t, m16 +112t)
        // were 6-step line wins that lost the real race. Fold race in so a
        // CP gained in-horizon beats a prettier DTE. When already a CP
        // down, weigh it harder so 5-1 underdog splits try to catch up.
        // 0.004 when even/ahead left long-race 5-1s on the 0.844 grid.
        // 0.015 only when a CP down; 0.008 otherwise so we still race
        // after a 6-step line win.
        float race_w = (mc < oc) ? 0.015f : 0.004f;
        // Photo-finish: even remaining <=2 CPs and next corner <80°.
        // race_w 0.018 on this gate: pfw 143g 115-28 wr=0.804. Stay 0.012.
        if (mc == oc) {
            int left = laps * cp_ct - mc;
            if (left <= 2 && left >= 0) {
                float da2 = sb_dte(pods[id]), db2 = sb_dte(pods[id + 1]);
                Pod* mr2 = da2 <= db2 ? pods[id] : pods[id + 1];
                Checkpoint* cp2 = cps[mr2->ncpid];
                int nn = (mr2->ncpid + 1) % cp_ct;
                float ix = cp2->x - mr2->x, iy = cp2->y - mr2->y;
                float ox = cps[nn]->x - cp2->x, oy = cps[nn]->y - cp2->y;
                float inn = sqrtf(ix * ix + iy * iy), onn = sqrtf(ox * ox + oy * oy);
                float td = 0.f;
                if (inn > 1.f && onn > 1.f)
                    td = fabsf(atan2f(ix * oy - iy * ox, ix * ox + iy * oy))
                         * (180.f / 3.14159265f);
                if (td < 80.f) race_w = 0.012f;
            }
        }
        score += race_w * sol->race;
        load();
        sols_ct++;
        return score;
#else
        ReflexBot* rb = static_cast<ReflexBot*>(opp);
        while (turn < DEPTH) {
            move(sol);
            rb->move();
            play();
            turn++;
        }
        score += evaluate();
        load();
        if (r > 0) sols_ct++;
        return score;
#endif
    }

#ifdef SEARCHBOT_CHALLENGER
    float get_fid_score(Solution* s) {
        SearchBot* sb_opp = NULL;
        if (!oppBots.empty()) sb_opp = dynamic_cast<SearchBot*>(oppBots[0]);
        Pod* orun0 = sb_dte(pods[(id + 2) % 4]) <= sb_dte(pods[(id + 3) % 4])
                         ? pods[(id + 2) % 4] : pods[(id + 3) % 4];
        opp_init_speed = sqrtf(orun0->vx * orun0->vx + orun0->vy * orun0->vy);
        ev_start_checked = pods[id]->checked > pods[id + 1]->checked
                               ? pods[id]->checked : pods[id + 1]->checked;
        ev_start_opp = pods[(id + 2) % 4]->checked > pods[(id + 3) % 4]->checked
                           ? pods[(id + 2) % 4]->checked : pods[(id + 3) % 4]->checked;
        float r_act = (float)DEPTH + 0.3f;
        float o_act = (float)DEPTH + 0.3f;
        const int* oth = NULL;
        const float* oang = NULL;
        bool have = false;
        bool opp_fr = true;
        if (sb_opp) {
            oth = sb_opp->sol.thrusts;
            oang = sb_opp->sol.angles;
            have = true;
            opp_fr = sb_opp->as_frozen_b;
        }
        sb_fid_rollout(s->thrusts, s->angles, oth, oang, id, have,
                       false, opp_fr, &r_act, &o_act,
                       ev_start_checked, ev_start_opp);
        float score = evaluate();
        score -= 1000.f * r_act;
        score += 800.f * o_act;
        int mc, oc;
        {
            float da = sb_dte(pods[id]), db = sb_dte(pods[id + 1]);
            float dc = sb_dte(pods[(id + 2) % 4]), dd = sb_dte(pods[(id + 3) % 4]);
            mc = pods[id]->checked > pods[id + 1]->checked ? pods[id]->checked : pods[id + 1]->checked;
            oc = pods[(id + 2) % 4]->checked > pods[(id + 3) % 4]->checked
                     ? pods[(id + 2) % 4]->checked : pods[(id + 3) % 4]->checked;
            s->race = 1000000.f * (float)(mc - oc) - (da <= db ? da : db)
                      + 0.35f * (dc <= dd ? dc : dd);
        }
        float race_w = (mc < oc) ? 0.015f : 0.004f;
        if (mc == oc) {
            int left = laps * cp_ct - mc;
            if (left <= 2 && left >= 0) {
                float da2 = sb_dte(pods[id]), db2 = sb_dte(pods[id + 1]);
                Pod* mr2 = da2 <= db2 ? pods[id] : pods[id + 1];
                Checkpoint* cp2 = cps[mr2->ncpid];
                int nn = (mr2->ncpid + 1) % cp_ct;
                float ix = cp2->x - mr2->x, iy = cp2->y - mr2->y;
                float ox = cps[nn]->x - cp2->x, oy = cps[nn]->y - cp2->y;
                float inn = sqrtf(ix * ix + iy * iy), onn = sqrtf(ox * ox + oy * oy);
                float td = 0.f;
                if (inn > 1.f && onn > 1.f)
                    td = fabsf(atan2f(ix * oy - iy * ox, ix * ox + iy * oy))
                         * (180.f / 3.14159265f);
                if (td < 80.f) race_w = 0.012f;
            }
        }
        score += race_w * s->race;
        load();
        return score;
    }
#endif

    float evaluate() {
        Pod* a = pods[id];
        Pod* b = pods[id + 1];
        Pod* c = pods[(id + 2) % 4];
        Pod* d = pods[(id + 3) % 4];
        Pod* my_runner;
        float sa, sb, my_s;
        if (a->checked != b->checked) {
            my_runner = a->checked > b->checked ? a : b;
            my_s = my_runner->score();
        } else {
            sa = a->score();
            sb = b->score();
            my_runner = (sa - sb >= -1000.f) ? a : b;
            my_s = (my_runner == a) ? sa : sb;
        }
        Pod* opp_runner;
        float opp_s;
        if (c->checked != d->checked) {
            opp_runner = c->checked > d->checked ? c : d;
            opp_s = opp_runner->score();
        } else {
            float sc = c->score();
            float sd = d->score();
            opp_runner = (sc - sd >= -1000.f) ? c : d;
            opp_s = (opp_runner == c) ? sc : sd;
        }
        (void)opp_runner;
#ifdef SEARCHBOT_CHALLENGER
        float da = sb_dte(a), db = sb_dte(b), dc = sb_dte(c), dd = sb_dte(d);
        // t0h/t0t heading pick: 0.834 / 0.820. llh last-2 heading:
        // 153g 125-28 wr=0.817. l1h last-1: 141g 0.801 m8 6-0 but
        // m1 2-4 / m11 3-3. l1g gated last-1: 135g 108-27 wr=0.800,
        // m8 6-0 but m4 2-4. Official best is DTE (pf 0.849).
        Pod* mr = da <= db ? a : b;
        Pod* mbk = mr->partner;
        Pod* orun = dc <= dd ? c : d;
        // Reward progress + next-CP line. Penalize speed while crooked so
        // search prefers 50-150 into corners; frozen B still slams 200.
        float perim = 0.f;
        for (int i = 0; i < cp_ct; i++) perim += cps[i]->dist(cps[(i + 1) % cp_ct]);
        const bool handling = (cp_ct >= 5 && perim / (float)cp_ct <= 6500.f);
        const bool bmode = (sb_eval_mode == 1);
        const float dw = bmode ? (handling ? 2.9f : 2.5f) : (handling ? 3.0f : 2.8f);
        const float aw = bmode ? (handling ? 3.7f : 2.2f) : (handling ? 3.2f : 2.4f);
        const float sw = bmode ? (handling ? 0.20f : 0.45f) : (handling ? 0.15f : 0.25f);
        const float lw = bmode ? (handling ? 1.0f : 1.2f) : (handling ? 1.4f : 1.6f);
        const float apw = bmode ? (handling ? 60.f : 45.f) : (handling ? 28.f : 22.f);
        float score = 0.f;
        if (mr->checked >= laps * cp_ct) score += 1e9f;
        if (orun->checked >= laps * cp_ct) score -= 1e9f;
        score -= dw * (da <= db ? da : db);
        score -= 0.04f * (da <= db ? db : da);
        score += 0.35f * (dc <= dd ? dc : dd);
        // edte 136g 106-29 wr=0.779: even_w 0.85 last-2 wrecked
        // m7/m17/m18 3-3. Revert even term to pf 0.5.
        // ck18 checked_diff 18000: m0 already 3-3 vs pf 5-1. Stay 15000.
        score += 15000.f * (float)(mr->checked - orun->checked);
        if (mr->checked == orun->checked)
            score += 0.5f * ((dc <= dd ? dc : dd) - (da <= db ? da : db));
        // sh2 last-2 shield-500: 109g 81-28 wr=0.743. Taxed m1/m2/m3/m4/m16
        // vs pf 6-0/5-1; only m8 +1. Revert.
        const bool trail = false;
        float sep = a->dist(b);
        if (sep < 900.f) score -= (900.f - sep) * 8.f;
        Checkpoint* cp = cps[mr->ncpid];
        float dx = cp->x - mr->x, dy = cp->y - mr->y;
        float cpd = sqrtf(dx * dx + dy * dy);
        if (cpd > 1.f) {
            float nx = dx / cpd, ny = dy / cpd;
            score += (mr->vx * nx + mr->vy * ny) * aw;
            score -= fabsf(mr->vx * ny - mr->vy * nx) * lw;
        }
        const float heading = fabsf(mr->diff_angle(cp));
        score -= heading * apw;
        float spd = sqrtf(mr->vx * mr->vx + mr->vy * mr->vy);
        float cw = sb_corner_w(mr);
        // Corner-only slowdown on 4+ CP hairpins (m17 119°, m3 105/117°).
        // 3CP stays off: all-map corner-sw wrecked m13 (wr 0.744).
        float sw_use;
        if (bmode) sw_use = sw;
        else if (cp_ct >= 4) sw_use = (cw > 0.05f) ? 0.15f : 0.40f;
        else sw_use = 0.20f;  // 3CP: 0.40 slammed short triangles (m15 3-3)
        score += spd * sw_use;
        if (!bmode && cp_ct >= 4 && cw > 0.05f && heading > 22.f)
            score -= spd * (heading - 22.f) * 0.12f;
        else if (!bmode && handling && heading > 22.f)
            score -= spd * (heading - 22.f) * 0.12f;
        if (!bmode && cw > 0.f) score -= spd * cw * 0.20f;
        // Always scale ncut to this leg. Floor 1200 on a 2167 m15/m27
        // first leg cut for 55% of the sprint and we missed (m15 2-4).
        float ncut_d = bmode ? 3000.f : 3800.f;
        const float ncut_w = bmode ? 2.5f : 4.0f;
        if (!bmode && cp_ct > 1) {
            int prev = (mr->ncpid + cp_ct - 1) % cp_ct;
            float full_leg = cps[prev]->dist(cp);
            // Long opening that is not a hairpin: sprint (m4/m13 3-3, m0 4-2).
            // m17 first_leg 4926 @ 119° must keep ncut.
            bool first_open = (ev_start_checked == 0 && mr->checked == 0);
            int nn0 = (mr->ncpid + 1) % cp_ct;
            float ix = cp->x - mr->x, iy = cp->y - mr->y;
            float ox = cps[nn0]->x - cp->x, oy = cps[nn0]->y - cp->y;
            float in = sqrtf(ix * ix + iy * iy), on = sqrtf(ox * ox + oy * oy);
            float turn_deg = 0.f;
            if (in > 1.f && on > 1.f)
                turn_deg = fabsf(atan2f(ix * oy - iy * ox, ix * ox + iy * oy))
                           * (180.f / 3.14159265f);
            // 4+ CP only. 3CP always ncuts — skip-open on m4 (86°/5608)
            // and m15-class triangles was the fast-loss cluster.
            if (cp_ct >= 4 && first_open && full_leg >= 4000.f && turn_deg < 100.f) {
                ncut_d = 0.f;
            } else {
                float frac = (cp_ct == 3) ? 0.22f : 0.28f;
                float cap = full_leg * frac;
                float floor = (full_leg < 3000.f) ? 700.f : 1200.f;
                if (cap < floor) cap = floor;
                if (cap < ncut_d) ncut_d = cap;
            }
        }
        if (cpd < ncut_d) {
            int nn = (mr->ncpid + 1) % cp_ct;
            float nnx = cps[nn]->x - mr->x, nny = cps[nn]->y - mr->y;
            float nnd = sqrtf(nnx * nnx + nny * nny);
            if (nnd > 1.f) score += (mr->vx * nnx + mr->vy * nny) / nnd * ncut_w;
        }
        if (mr->x < -2000.f || mr->x > 18000.f || mr->y < -2000.f || mr->y > 11000.f)
            score -= 1e6f;
        float bx = mbk->x - orun->x, by = mbk->y - orun->y;
        float bd = sqrtf(bx * bx + by * by);
        if (!trail && bd < 5000.f) {
            float norm = (5000.f - bd) / 4200.f;
            if (norm > 1.f) norm = 1.f;
            score += norm * 1200.f;
            if (bd < 2000.f) {
                float cn = (2000.f - bd) / 1200.f;
                if (cn > 1.f) cn = 1.f;
                score += cn * cn * 2200.f;
            }
        } else if (!trail) {
            score -= (bd - 5000.f) * 0.4f;
        }
        if (trail) return score;
        // add speed loss
        float opp_end_speed = sqrtf(orun->vx*orun->vx + orun->vy*orun->vy);
        float speed_loss = opp_init_speed - opp_end_speed;
        if (speed_loss > 0) score += speed_loss * 1.5f;
        float ocx = cps[orun->ncpid]->x - orun->x;
        float ocy = cps[orun->ncpid]->y - orun->y;
        float ocd = sqrtf(ocx * ocx + ocy * ocy);
        if (ocd > 1.f && bd > 1.f) {
            float nx = ocx / ocd, ny = ocy / ocd;
            float proj = bx * nx + by * ny;
            float lat = fabsf(bx * ny - by * nx);
            if (proj > 0.f && proj < ocd) {
                float pq = 1.f - fabsf(proj / ocd - 0.35f) * 2.f;
                if (pq < 0.f) pq = 0.f;
                float lq = 1.f - lat / 1500.f;
                if (lq < 0.f) lq = 0.f;
                score += pq * lq * 800.f;
            }
        }
        return score;
#else
        return my_s - opp_s - my_runner->partner->dist(my_runner);
#endif
    }
};

void load() {
    for (int i = 0; i < 4; i++) pods[i]->load();
    turn = 0;
}

void play() {
    Pod* p0 = pods[0];
    Pod* p1 = pods[1];
    Pod* p2 = pods[2];
    Pod* p3 = pods[3];
    float t = 0.0;
    while (t < 1.0) {
        Unit* ba = NULL;
        Unit* bb = NULL;
        float bt = -1;
        // Same visit order as the pin double loop (i, j=i+1, then i's CP).
        const float rem = 1.0f - t;
#define SB_CONSIDER(A, B, SR2, RS)                                             \
        do {                                                                   \
            float col_time = (A)->collision_time_r((B), (SR2), (RS), rem);     \
            if (col_time > -1 && col_time + t < 1.0 &&                         \
                (bt == -1 || col_time < bt)) {                                 \
                ba = (A);                                                      \
                bb = (B);                                                      \
                bt = col_time;                                                 \
            }                                                                  \
        } while (0)
#define SB_CONSIDER_CP(A)                                                      \
        do {                                                                   \
            float col_time = (A)->collision_time_cp(cps[(A)->ncpid], rem);     \
            if (col_time > -1 && col_time + t < 1.0 &&                         \
                (bt == -1 || col_time < bt)) {                                 \
                ba = (A);                                                      \
                bb = cps[(A)->ncpid];                                          \
                bt = col_time;                                                 \
            }                                                                  \
        } while (0)
        SB_CONSIDER(p0, p1, 640000.f, 800.f);
        SB_CONSIDER(p0, p2, 640000.f, 800.f);
        SB_CONSIDER(p0, p3, 640000.f, 800.f);
        SB_CONSIDER_CP(p0);
        SB_CONSIDER(p1, p2, 640000.f, 800.f);
        SB_CONSIDER(p1, p3, 640000.f, 800.f);
        SB_CONSIDER_CP(p1);
        SB_CONSIDER(p2, p3, 640000.f, 800.f);
        SB_CONSIDER_CP(p2);
        SB_CONSIDER_CP(p3);
#undef SB_CONSIDER_CP
#undef SB_CONSIDER

        if (bt == -1) {
            float rem2 = 1.0 - t;
            p0->move(rem2);
            p1->move(rem2);
            p2->move(rem2);
            p3->move(rem2);
            t = 1.0;
        } else {
            p0->move(bt);
            p1->move(bt);
            p2->move(bt);
            p3->move(bt);
            Pod* pa = static_cast<Pod*>(ba);
            if (bb->type == CP) {
                pa->checked += 1;
                pa->timeout = pa->partner->timeout = 100;
                pa->ncpid = (pa->ncpid + 1) % cp_ct;
            } else {
                pa->bounce_w_pod(static_cast<Pod*>(bb));
            }
            t += bt;
        }
    }

    p0->end();
    p1->end();
    p2->end();
    p3->end();
}

void print_move(int thrust, float angle, Pod* pod) {
    float a = pod->angle + angle;

    if (a >= 360.0) {
        a = a - 360.0;
    } else if (a < 0.0) {
        a += 360.0;
    }

    a = a * M_PI / 180.0;
    float px = pod->x + cos(a) * 10000.0;
    float py = pod->y + sin(a) * 10000.0;

#ifdef SEARCHBOT_CHALLENGER
    if (thrust == -1) {
        printf("%d %d SHIELD\n", (int) round(px), (int) round(py));
        pod->shield = 4;
    } else if (thrust == 650) {
        if (!pod->has_boost) {
            printf("%d %d %d\n", (int) round(px), (int) round(py), MAX_THRUST);
            return;
        }
        pod->has_boost = false;
        printf("%d %d BOOST\n", (int) round(px), (int) round(py));
    } else {
        printf("%d %d %d\n", (int) round(px), (int) round(py), thrust);
    }
#else
    char copyright[] = ""; // pin dump: keep
    if (thrust == -1) {
        printf("%d %d SHIELD %s\n", (int) round(px), (int) round(py), copyright);
        pod->shield = 4;
    } else if (thrust == 650) {
        pod->has_boost = false;
        printf("%d %d BOOST %s\n", (int) round(px), (int) round(py), copyright);
    } else {
        printf("%d %d %d %s\n", (int) round(px), (int) round(py), thrust, copyright);
    }
#endif
}

#ifndef SEARCHBOT_CHALLENGER
int main() {
    cin >> laps >> cp_ct;
    for (int i = 0; i < cp_ct; i++) {
        int cx, cy;
        cin >> cx >> cy;
        cps[i] = new Checkpoint(i, cx, cy);
    }

    for (int i = 0; i < 4; i++) pods[i] = new Pod(i);

    pods[0]->partner = pods[1];
    pods[1]->partner = pods[0];
    pods[2]->partner = pods[3];
    pods[3]->partner = pods[2];

    ReflexBot me_reflex;

    SearchBot opp(2);
    opp.oppBots.push_back(&me_reflex);

    SearchBot me;
    me.oppBots.push_back(&opp);

    while (1) {
        r++;

        for (int i = 0; i < 4; i++) {
            int x, y, vx, vy, angle, ncpid;
            cin >> x >> y >> vx >> vy >> angle >> ncpid;
            if (r == 0 && i > 1 && angle > -1) is_p2 = true;
            pods[i]->update(x, y, vx, vy, angle, ncpid);
        }

        now = high_resolution_clock::now();

        float time_limit = r ? 0.142 : 0.98;
        time_limit *= 0.3;

        // use this to test reflex bot behavior
        // me_reflex.move_as_main();

        opp.solve(time_limit*0.15);
        me.solve(time_limit, r > 0);

        if (r > 0) cerr << "Avg iters: " << sols_ct / r << "; Avg sims: " << sols_ct*DEPTH / r << endl;

        print_move(me.sol.thrusts[0], me.sol.angles[0], pods[0]);
        print_move(me.sol.thrusts[DEPTH], me.sol.angles[DEPTH], pods[1]);
    }
}
#endif
// CG I/O for Magus SearchBot (opt_search.inc play()/search).
// Include after #define SEARCHBOT_CHALLENGER and opt_search.inc.
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cin >> laps >> cp_ct;
    for (int i = 0; i < cp_ct; i++) {
        int cx, cy;
        cin >> cx >> cy;
        cps[i] = new Checkpoint(i, cx, cy);
    }

    for (int i = 0; i < 4; i++) pods[i] = new Pod(i);
    pods[0]->partner = pods[1];
    pods[1]->partner = pods[0];
    pods[2]->partner = pods[3];
    pods[3]->partner = pods[2];

    ReflexBot me_reflex;
    SearchBot opp(2);
    opp.oppBots.push_back(&me_reflex);
    SearchBot me;
    me.oppBots.push_back(&opp);

    while (true) {
        r++;
        for (int i = 0; i < 4; i++) {
            int x, y, vx, vy, angle, ncpid;
            if (!(cin >> x >> y >> vx >> vy >> angle >> ncpid)) return 0;
            pods[i]->update(x, y, vx, vy, angle, ncpid);
        }

        now = high_resolution_clock::now();
        const int sols0 = sols_ct;
        const float tl = (r == 0) ? 0.94f : 0.074f;
        opp.as_frozen_b = true;
        opp.solve(0.002f);
        me.as_frozen_b = false;
        me.solve(tl - 0.002f, r > 0);
        if (cp_ct >= 4) {
            int rp = (sb_dte(pods[0]) <= sb_dte(pods[1])) ? 0 : 1;
            Pod* rp_pod = pods[rp];
            {
                Checkpoint* cp = cps[rp_pod->ncpid];
                float tx = cp->x - 3.f * rp_pod->vx;
                float ty = cp->y - 3.f * rp_pod->vy;
                float raw = rp_pod->diff_angle_xy(tx, ty);
                Solution bang = me.sol;
                bang.score = -1;
                int idx = rp * DEPTH;
                bang.angles[idx] = max(-18.f, min(18.f, raw));
                int th = fabsf(raw) < 90.f ? MAX_THRUST : 0;
                int left = laps * cp_ct - rp_pod->checked;
                if (r == 0 && rp_pod->has_boost && rp_pod->dist(cps[1]) > 4000.f) {
                    th = 650;
                }
                else if (rp_pod->has_boost && fabsf(raw) < 25.f) {
                    int myck = pods[0]->checked > pods[1]->checked ? pods[0]->checked : pods[1]->checked;
                    int opck = pods[2]->checked > pods[3]->checked ? pods[2]->checked : pods[3]->checked;
                    if (left <= 1 && myck >= opck) th = 650;
                }
                bang.thrusts[idx] = th;
                float ga_s = me.get_score(&me.sol);
                float bang_s = me.get_score(&bang);
                if (bang_s > ga_s) me.sol = bang;
            }
        }
        const int sols = sols_ct - sols0;
        const float used_ms = TIME * 1000.f;

        Pod* racer = sb_dte(pods[0]) <= sb_dte(pods[1]) ? pods[0] : pods[1];
        Pod* orun = sb_dte(pods[2]) <= sb_dte(pods[3]) ? pods[2] : pods[3];
        auto emit = [&](Pod* pod, int thrust, float ang) {
            if (thrust == 650 && !pod->has_boost) thrust = MAX_THRUST;
            print_move(thrust, ang, pod);
        };
        emit(pods[0], me.sol.thrusts[0], me.sol.angles[0]);
        emit(pods[1], me.sol.thrusts[DEPTH], me.sol.angles[DEPTH]);
        fflush(stdout);
        cerr << "t=" << r
             << " ms=" << (int)used_ms
             << " sols=" << sols
             << " sims=" << sols * DEPTH
             << " racer=" << racer->id
             << " ck=" << racer->checked
             << " dte=" << (int)sb_dte(racer)
             << " odte=" << (int)sb_dte(orun)
             << " boost=" << (racer->has_boost ? 1 : 0)
             << endl;
    }
}

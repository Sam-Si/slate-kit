#pragma GCC optimize("O3,unroll-loops,inline,omit-frame-pointer")
#pragma GCC target("avx2,fma,bmi,bmi2")
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <ctime>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#ifndef sincos
#define sincos(x, s, c) do { *(s)=sin(x); *(c)=cos(x); } while(0)
#endif

namespace opp_nn {
inline void predict(const float*, float* output) {
for(int i=0;i<8;i++) output[i]=0;
}
inline int logits_to_thrust(float, float, float) { return 100; }
} // namespace opp_nn


namespace csb {
constexpr int POD_COUNT = 4;
constexpr double POD_RADIUS = 400.0;
constexpr double CHECKPOINT_RADIUS = 600.0;
constexpr double POD_DIAMETER = 2.0 * POD_RADIUS;
constexpr double POD_RSQ = POD_DIAMETER * POD_DIAMETER;
constexpr double CP_RSQ = CHECKPOINT_RADIUS * CHECKPOINT_RADIUS;
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
constexpr double INIT_ANGLE_SENTINEL = -0.0174533;
constexpr double SENTINEL_TOLERANCE = 0.001;
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
constexpr TrigLUT _trig_lut{};
struct Point {
double x;
double y;
inline double norm() const {
return std::sqrt(x * x + y * y);
}
inline double dot(const Point& other) const {
return x * other.x + y * other.y;
}
inline double dist(const Point& other) const {
return std::sqrt((x - other.x) * (x - other.x) + (y - other.y) * (y - other.y));
}
};
struct Pod {
Point pos;
Point vel;
double angle;
int next;
int shieldtimer;
int boosted;
bool won;
bool isFirstTurn;
inline void applyRotate(double target_angle) {
double delta = target_angle - angle;
double da = delta - TWO_PI * std::trunc(delta * INV_TWO_PI);
if (da >= PI) da -= TWO_PI;
else if (da <= -PI) da += TWO_PI;
if (da <= -MAX_ROTATE) {
angle -= MAX_ROTATE;
} else if (da >= MAX_ROTATE) {
angle += MAX_ROTATE;
} else {
angle = target_angle;
}
}
inline void applyRotateFirst(double target_angle) {
angle = target_angle;
}
inline void applyThrust(int thrust) {
if (thrust == 0) return;
double cc, cs;
sincos(angle, &cs, &cc);
if (std::abs(cc - SNAP_COS) < SNAP_TOLERANCE &&
std::abs(std::abs(cs) - SNAP_SIN) < SNAP_TOLERANCE) {
cc = SNAP_COS;
cs = (cs > 0 ? SNAP_SIN : -SNAP_SIN);
}
vel.x += cc * thrust;
vel.y += cs * thrust;
}
inline void applyActionDelta(double angle_delta_deg, int thrust, int shield, int boost) {
if (shield) {
shieldtimer = SHIELD_ACTIVE_TIMER;
if (isFirstTurn) isFirstTurn = false;
return;
}
int idx = (int)(angle_delta_deg * 10.0 + 180.5);
if (idx < 0) idx = 0;
else if (idx > 360) idx = 360;
if (isFirstTurn) isFirstTurn = false;
angle += _trig_lut.rad_val[idx];
if (shieldtimer == 0) {
int t_val = thrust;
if (boost && boosted == 0) {
t_val = BOOST_THRUST; boosted = 1;
} else {
if (t_val > MAX_THRUST) t_val = MAX_THRUST;
else if (t_val < 0) t_val = 0;
}
if (t_val > 0) {
applyThrust(t_val);
}
}
}
inline void endTurn() {
vel.x = std::trunc(vel.x * FRICTION);
vel.y = std::trunc(vel.y * FRICTION);
pos.x = std::floor(pos.x + 0.5);
pos.y = std::floor(pos.y + 0.5);
if (shieldtimer > 0) {
shieldtimer--;
}
}
inline double newCollide(const Pod& other, double rsq, double first) const {
Point rel_p = {other.pos.x - pos.x, other.pos.y - pos.y};
double pLength2 = rel_p.x * rel_p.x + rel_p.y * rel_p.y;
if (pLength2 <= rsq) {
return 0.0;
}
Point rel_vel = {other.vel.x - vel.x, other.vel.y - vel.y};
double dot_val = rel_p.dot(rel_vel);
if (dot_val > 0.0) {
return NO_COLLISION;
}
double vLength2 = rel_vel.x * rel_vel.x + rel_vel.y * rel_vel.y;
if (vLength2 == 0.0) {
return NO_COLLISION;
}
double disc = dot_val * dot_val - vLength2 * (pLength2 - rsq);
if (disc <= 0.0) {
return NO_COLLISION;
}
double req = -dot_val - first * vLength2;
if (req >= 0.0 && req * req > disc) {
return NO_COLLISION;
}
double discdist = std::sqrt(disc);
double t1 = (-dot_val - discdist) / vLength2;
return t1;
}
inline void passCheckpoint(int podn, int globalCpSize, int* playerTimeout) {
next = next + 1;
if (next >= globalCpSize) {
next = globalCpSize - 1;
won = true;
}
if (podn < 2) {
playerTimeout[0] = TIMEOUT_RESET;
} else {
playerTimeout[1] = TIMEOUT_RESET;
}
}
};
inline bool cpCollide(Point p1, Point p2, Point cp, double cp_rsq) {
double dx = p2.x - p1.x;
double dy = p2.y - p1.y;
Point pp = p1;
double pd2 = dx * dx + dy * dy;
if (pd2 != 0.0) {
double u = ((cp.x - p1.x) * dx + (cp.y - p1.y) * dy) / pd2;
if (u > 1.0) {
pp = p2;
} else if (u > 0.0) {
pp.x = p1.x + u * dx;
pp.y = p1.y + u * dy;
}
}
pp.x -= cp.x;
pp.y -= cp.y;
double distSQ = pp.x * pp.x + pp.y * pp.y;
return distSQ < cp_rsq;
}
struct Game {
Pod pods[POD_COUNT];
Point globalCp[256];
int globalCpSize;
int playerTimeout[2];
Game() {
globalCpSize = 0;
playerTimeout[0] = TIMEOUT_INITIAL;
playerTimeout[1] = TIMEOUT_INITIAL;
}
inline void setPodState(int pod_idx, double x, double y, double vx, double vy,
double angle_rad, int next_cp, int shield_timer, int has_boosted) {
if (pod_idx < 0 || pod_idx >= POD_COUNT) return;
Pod& pod = pods[pod_idx];
pod.pos = {x, y};
pod.vel = {vx, vy};
pod.angle = angle_rad;
pod.next = next_cp;
pod.shieldtimer = shield_timer;
pod.boosted = has_boosted;
pod.won = false;
pod.isFirstTurn = (std::abs(angle_rad - INIT_ANGLE_SENTINEL) < SENTINEL_TOLERANCE) ||
(std::abs(angle_rad) < SENTINEL_TOLERANCE);
}
inline void setPlayerTimeouts(int t0, int t1) {
playerTimeout[0] = t0;
playerTimeout[1] = t1;
}
inline void applyAction(int pod_idx, int target_x, int target_y,
const std::string_view thrust_str) {
if (pod_idx < 0 || pod_idx >= POD_COUNT) return;
Pod& pod = pods[pod_idx];
Point target = {static_cast<double>(target_x), static_cast<double>(target_y)};
bool is_shield = false;
bool is_boost = false;
int val = 0;
bool is_num = false;
if (thrust_str == "200") {
val = 200;
is_num = true;
} else if (thrust_str == "0") {
val = 0;
is_num = true;
} else if (!thrust_str.empty()) {
if (thrust_str[0] == 'S') {
is_shield = true;
} else if (thrust_str[0] == 'B') {
is_boost = true;
} else {
is_num = true;
bool neg = (thrust_str[0] == '-');
size_t i = neg ? 1 : 0;
int v = 0;
for (; i < thrust_str.size(); ++i) {
if (thrust_str[i] >= '0' && thrust_str[i] <= '9') {
v = v * 10 + (thrust_str[i] - '0');
} else {
break;
}
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
double target_angle = std::atan2(dy, dx);
if (pod.isFirstTurn) {
pod.isFirstTurn = false;
pod.applyRotateFirst(target_angle);
} else {
pod.applyRotate(target_angle);
}
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
} else {
thrust = MAX_THRUST;
}
} else if (is_num) {
thrust = val;
if (thrust < 0) thrust = 0;
if (thrust > MAX_THRUST) thrust = MAX_THRUST;
}
if (!used_shield && pod.shieldtimer > 0) {
thrust = 0;
}
if (dx == 0.0 && dy == 0.0) {
thrust = 0;
}
if (thrust > 0) {
pod.applyThrust(thrust);
}
}
inline void initialize(const std::vector<Point>& track, int laps = DEFAULT_LAPS) {
globalCpSize = 0;
for (int i = 0; i < laps; ++i) {
for (const auto& cp : track) {
if (globalCpSize < 255) {
globalCp[globalCpSize++] = cp;
}
}
}
if (globalCpSize < 256) {
globalCp[globalCpSize++] = track[0];
}
const Point startPointMult[POD_COUNT] = {
{500.0, -500.0}, {-500.0, 500.0}, {1500.0, -1500.0}, {-1500.0, 1500.0}
};
double dx = track[1].x - track[0].x;
double dy = track[1].y - track[0].y;
double dd = std::sqrt(dx * dx + dy * dy);
Point cp1minus0 = {dx / dd, dy / dd};
for (int podN = 0; podN < POD_COUNT; ++podN) {
Pod& pod = pods[podN];
pod.angle = -1.0 * DEG_TO_RAD;
pod.next = 1;
pod.shieldtimer = 0;
pod.boosted = 0;
pod.won = false;
pod.isFirstTurn = true;
pod.vel = {0.0, 0.0};
pod.pos.x = std::floor(track[0].x + cp1minus0.y * startPointMult[podN].x + 0.5);
pod.pos.y = std::floor(track[0].y + cp1minus0.x * startPointMult[podN].y + 0.5);
}
playerTimeout[0] = TIMEOUT_INITIAL;
playerTimeout[1] = TIMEOUT_INITIAL;
}
inline void forwardTime(double t) {
pods[0].pos.x += pods[0].vel.x * t; pods[0].pos.y += pods[0].vel.y * t;
pods[1].pos.x += pods[1].vel.x * t; pods[1].pos.y += pods[1].vel.y * t;
pods[2].pos.x += pods[2].vel.x * t; pods[2].pos.y += pods[2].vel.y * t;
pods[3].pos.x += pods[3].vel.x * t; pods[3].pos.y += pods[3].vel.y * t;
}
inline void bounce(int idx_a, int idx_b) {
Pod& pod_a = pods[idx_a];
Pod& pod_b = pods[idx_b];
Point normal = {pod_b.pos.x - pod_a.pos.x, pod_b.pos.y - pod_a.pos.y};
double dist = normal.norm();
double inv_dist = 1.0 / dist;
normal.x *= inv_dist;
normal.y *= inv_dist;
Point relv = {pod_a.vel.x - pod_b.vel.x, pod_a.vel.y - pod_b.vel.y};
double m1 = (pod_a.shieldtimer == SHIELD_ACTIVE_TIMER) ? SHIELD_INV_MASS : NORMAL_INV_MASS;
double m2 = (pod_b.shieldtimer == SHIELD_ACTIVE_TIMER) ? SHIELD_INV_MASS : NORMAL_INV_MASS;
double force = normal.dot(relv) / (m1 + m2);
if (force < MIN_IMPULSE) {
force += MIN_IMPULSE;
} else {
force += force;
}
Point impulse = normal;
impulse.x *= -force;
impulse.y *= -force;
pod_a.vel.x += impulse.x * m1;
pod_a.vel.y += impulse.y * m1;
pod_b.vel.x += -impulse.x * m2;
pod_b.vel.y += -impulse.y * m2;
if (dist <= POD_DIAMETER) {
double dist_diff = dist - POD_DIAMETER;
double push = -dist_diff * 0.5 + EPSILON;
double neg_push = -push;
pod_a.pos.x += normal.x * neg_push;
pod_a.pos.y += normal.y * neg_push;
pod_b.pos.x += normal.x * push;
pod_b.pos.y += normal.y * push;
}
}
inline void nextTurn() {
/* OPT free-flight: most GA steps have no pod-pod hit — skip multi-bounce loop body churn */
Point curps[POD_COUNT] = {
pods[0].pos, pods[1].pos, pods[2].pos, pods[3].pos
};
double first = 1.0;
int col_i = -1, col_j = -1;
double tx;
/* OPT: coarse far-apart screen — if all pair dist^2 > 4e6, zero collision work */
{
bool any_near = false;
for (int i = 0; i < 4 && !any_near; i++) {
for (int j = i + 1; j < 4; j++) {
double dx = pods[j].pos.x - pods[i].pos.x;
double dy = pods[j].pos.y - pods[i].pos.y;
if (dx * dx + dy * dy < 4000000.0) { any_near = true; break; }
}
}
if (!any_near) {
pods[0].pos.x += pods[0].vel.x; pods[0].pos.y += pods[0].vel.y;
pods[1].pos.x += pods[1].vel.x; pods[1].pos.y += pods[1].vel.y;
pods[2].pos.x += pods[2].vel.x; pods[2].pos.y += pods[2].vel.y;
pods[3].pos.x += pods[3].vel.x; pods[3].pos.y += pods[3].vel.y;
pods[0].endTurn(); pods[1].endTurn(); pods[2].endTurn(); pods[3].endTurn();
if (cpCollide(curps[0], pods[0].pos, globalCp[pods[0].next], CP_RSQ)) pods[0].passCheckpoint(0, globalCpSize, playerTimeout);
if (cpCollide(curps[1], pods[1].pos, globalCp[pods[1].next], CP_RSQ)) pods[1].passCheckpoint(1, globalCpSize, playerTimeout);
if (cpCollide(curps[2], pods[2].pos, globalCp[pods[2].next], CP_RSQ)) pods[2].passCheckpoint(2, globalCpSize, playerTimeout);
if (cpCollide(curps[3], pods[3].pos, globalCp[pods[3].next], CP_RSQ)) pods[3].passCheckpoint(3, globalCpSize, playerTimeout);
playerTimeout[0]--;
playerTimeout[1]--;
if (!pods[0].won) { double dx = pods[0].pos.x - globalCp[pods[0].next].x; double dy = pods[0].pos.y - globalCp[pods[0].next].y; if (dx * dx + dy * dy == CP_RSQ) pods[0].passCheckpoint(0, globalCpSize, playerTimeout); }
if (!pods[1].won) { double dx = pods[1].pos.x - globalCp[pods[1].next].x; double dy = pods[1].pos.y - globalCp[pods[1].next].y; if (dx * dx + dy * dy == CP_RSQ) pods[1].passCheckpoint(1, globalCpSize, playerTimeout); }
if (!pods[2].won) { double dx = pods[2].pos.x - globalCp[pods[2].next].x; double dy = pods[2].pos.y - globalCp[pods[2].next].y; if (dx * dx + dy * dy == CP_RSQ) pods[2].passCheckpoint(2, globalCpSize, playerTimeout); }
if (!pods[3].won) { double dx = pods[3].pos.x - globalCp[pods[3].next].x; double dy = pods[3].pos.y - globalCp[pods[3].next].y; if (dx * dx + dy * dy == CP_RSQ) pods[3].passCheckpoint(3, globalCpSize, playerTimeout); }
return;
}
}
tx = pods[3].newCollide(pods[2], POD_RSQ, first);
if (tx <= first) { first = tx; col_i = 3; col_j = 2; }
tx = pods[3].newCollide(pods[1], POD_RSQ, first);
if (tx <= first) { first = tx; col_i = 3; col_j = 1; }
tx = pods[3].newCollide(pods[0], POD_RSQ, first);
if (tx <= first) { first = tx; col_i = 3; col_j = 0; }
tx = pods[2].newCollide(pods[1], POD_RSQ, first);
if (tx <= first) { first = tx; col_i = 2; col_j = 1; }
tx = pods[2].newCollide(pods[0], POD_RSQ, first);
if (tx <= first) { first = tx; col_i = 2; col_j = 0; }
tx = pods[1].newCollide(pods[0], POD_RSQ, first);
if (tx <= first) { first = tx; col_i = 1; col_j = 0; }
if (col_i < 0) {
/* free-flight fused move + end-turn */
pods[0].pos.x += pods[0].vel.x; pods[0].pos.y += pods[0].vel.y;
pods[1].pos.x += pods[1].vel.x; pods[1].pos.y += pods[1].vel.y;
pods[2].pos.x += pods[2].vel.x; pods[2].pos.y += pods[2].vel.y;
pods[3].pos.x += pods[3].vel.x; pods[3].pos.y += pods[3].vel.y;
pods[0].endTurn(); pods[1].endTurn(); pods[2].endTurn(); pods[3].endTurn();
if (cpCollide(curps[0], pods[0].pos, globalCp[pods[0].next], CP_RSQ)) pods[0].passCheckpoint(0, globalCpSize, playerTimeout);
if (cpCollide(curps[1], pods[1].pos, globalCp[pods[1].next], CP_RSQ)) pods[1].passCheckpoint(1, globalCpSize, playerTimeout);
if (cpCollide(curps[2], pods[2].pos, globalCp[pods[2].next], CP_RSQ)) pods[2].passCheckpoint(2, globalCpSize, playerTimeout);
if (cpCollide(curps[3], pods[3].pos, globalCp[pods[3].next], CP_RSQ)) pods[3].passCheckpoint(3, globalCpSize, playerTimeout);
playerTimeout[0]--;
playerTimeout[1]--;
if (!pods[0].won) { double dx = pods[0].pos.x - globalCp[pods[0].next].x; double dy = pods[0].pos.y - globalCp[pods[0].next].y; if (dx * dx + dy * dy == CP_RSQ) pods[0].passCheckpoint(0, globalCpSize, playerTimeout); }
if (!pods[1].won) { double dx = pods[1].pos.x - globalCp[pods[1].next].x; double dy = pods[1].pos.y - globalCp[pods[1].next].y; if (dx * dx + dy * dy == CP_RSQ) pods[1].passCheckpoint(1, globalCpSize, playerTimeout); }
if (!pods[2].won) { double dx = pods[2].pos.x - globalCp[pods[2].next].x; double dy = pods[2].pos.y - globalCp[pods[2].next].y; if (dx * dx + dy * dy == CP_RSQ) pods[2].passCheckpoint(2, globalCpSize, playerTimeout); }
if (!pods[3].won) { double dx = pods[3].pos.x - globalCp[pods[3].next].x; double dy = pods[3].pos.y - globalCp[pods[3].next].y; if (dx * dx + dy * dy == CP_RSQ) pods[3].passCheckpoint(3, globalCpSize, playerTimeout); }
return;
}
/* collision path (multi-bounce) */
double remaining = 1.0;
while (remaining > 0.0) {
first = remaining;
col_i = 0; col_j = 0;
tx = pods[3].newCollide(pods[2], POD_RSQ, first);
if (tx <= first) { first = tx; col_i = 3; col_j = 2; }
tx = pods[3].newCollide(pods[1], POD_RSQ, first);
if (tx <= first) { first = tx; col_i = 3; col_j = 1; }
tx = pods[3].newCollide(pods[0], POD_RSQ, first);
if (tx <= first) { first = tx; col_i = 3; col_j = 0; }
tx = pods[2].newCollide(pods[1], POD_RSQ, first);
if (tx <= first) { first = tx; col_i = 2; col_j = 1; }
tx = pods[2].newCollide(pods[0], POD_RSQ, first);
if (tx <= first) { first = tx; col_i = 2; col_j = 0; }
tx = pods[1].newCollide(pods[0], POD_RSQ, first);
if (tx <= first) { first = tx; col_i = 1; col_j = 0; }
forwardTime(first);
remaining -= first;
if (col_i != col_j) {
bounce(col_i, col_j);
}
if (remaining > 0.0) {
int colliders[2] = {col_i, col_j};
for (int ci : colliders) {
if (cpCollide(curps[ci], pods[ci].pos, globalCp[pods[ci].next], CP_RSQ)) {
pods[ci].passCheckpoint(ci, globalCpSize, playerTimeout);
}
}
curps[col_i] = pods[col_i].pos;
curps[col_j] = pods[col_j].pos;
}
}
pods[0].endTurn(); pods[1].endTurn(); pods[2].endTurn(); pods[3].endTurn();
if (cpCollide(curps[0], pods[0].pos, globalCp[pods[0].next], CP_RSQ)) pods[0].passCheckpoint(0, globalCpSize, playerTimeout);
if (cpCollide(curps[1], pods[1].pos, globalCp[pods[1].next], CP_RSQ)) pods[1].passCheckpoint(1, globalCpSize, playerTimeout);
if (cpCollide(curps[2], pods[2].pos, globalCp[pods[2].next], CP_RSQ)) pods[2].passCheckpoint(2, globalCpSize, playerTimeout);
if (cpCollide(curps[3], pods[3].pos, globalCp[pods[3].next], CP_RSQ)) pods[3].passCheckpoint(3, globalCpSize, playerTimeout);
playerTimeout[0]--;
playerTimeout[1]--;
if (!pods[0].won) { double dx = pods[0].pos.x - globalCp[pods[0].next].x; double dy = pods[0].pos.y - globalCp[pods[0].next].y; if (dx * dx + dy * dy == CP_RSQ) pods[0].passCheckpoint(0, globalCpSize, playerTimeout); }
if (!pods[1].won) { double dx = pods[1].pos.x - globalCp[pods[1].next].x; double dy = pods[1].pos.y - globalCp[pods[1].next].y; if (dx * dx + dy * dy == CP_RSQ) pods[1].passCheckpoint(1, globalCpSize, playerTimeout); }
if (!pods[2].won) { double dx = pods[2].pos.x - globalCp[pods[2].next].x; double dy = pods[2].pos.y - globalCp[pods[2].next].y; if (dx * dx + dy * dy == CP_RSQ) pods[2].passCheckpoint(2, globalCpSize, playerTimeout); }
if (!pods[3].won) { double dx = pods[3].pos.x - globalCp[pods[3].next].x; double dy = pods[3].pos.y - globalCp[pods[3].next].y; if (dx * dx + dy * dy == CP_RSQ) pods[3].passCheckpoint(3, globalCpSize, playerTimeout); }
}
};
}
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
class Bot {
public:
virtual ~Bot() = default;
virtual void init(
int laps,
const std::vector<std::pair<double, double>>& checkpoints) = 0;
virtual std::pair<BotAction, BotAction> get_actions(
const std::array<std::array<double, 6>, 4>& pods) = 0;
virtual std::unique_ptr<Bot> clone() const = 0;
};
}
namespace arena {
namespace ultimate_ga {
static constexpr double PI = 3.14159265358979323846;
static constexpr double DEG_TO_RAD = PI / 180.0;
static constexpr double RAD_TO_DEG = 180.0 / PI;
static constexpr int MAX_H = 8;
static constexpr int MAX_POP = 128;
static constexpr int SEARCH_MODE = 0;
static constexpr int BEAM_WIDTH = 64;
static constexpr int BEAM_N_ANGLES = 5;
static constexpr int BEAM_N_THRUSTS = 2;
static constexpr int BEAM_ACTIONS_PER_POD = BEAM_N_ANGLES * BEAM_N_THRUSTS;
static constexpr int BEAM_COMBINED = BEAM_ACTIONS_PER_POD * BEAM_ACTIONS_PER_POD;
static constexpr double BEAM_ANGLES[BEAM_N_ANGLES] = {-18.0, -6.0, 0.0, 6.0, 18.0};
static constexpr int BEAM_THRUSTS[BEAM_N_THRUSTS] = {0, 200};
static constexpr int BEAM_GRID_CELL = 500;
static constexpr int BEAM_GRID_W = 40;
static constexpr int BEAM_GRID_H = 24;
static constexpr double CP_RADIUS_SQ = 358801.0;
static constexpr double CP_RADIUS_SQ_EXACT = 360000.0;
static constexpr double POD_RADIUS = 400.0;
static constexpr double FRICTION = 0.85;
static constexpr double SHIELD_MASS = 10.0;
static constexpr int SHIELD_DURATION = 4;
static constexpr int BOOST_THRUST = 650;
static constexpr int MAX_THRUST = 200;
static constexpr double MAX_TURN_DEG = 18.0;
static constexpr double MAX_TURN_RAD = MAX_TURN_DEG * DEG_TO_RAD;
static constexpr int FIRST_TURN_BUDGET_US = 1000000;
static constexpr int TURN_BUDGET_US = 74500;
static constexpr int IBR_BUDGET_US = 500; /* OPT: less opp-GA, more phase2 sims */
static constexpr int MIN_PHASE2_BUDGET_US = 30000;
static constexpr int MCTS_BUDGET_US = 0; /* OPT: skip MCTS seed spend */
static constexpr int GA_POP_SIZE = 48; /* OPT: less seed eval, more evolution iters */
static constexpr int OPP_POP_SIZE = 32;
static constexpr int GA_HORIZON = 4; /* OPT: fewer steps/eval → more evals/ms */
static constexpr double CROSSOVER_PROB = 0.25;
static constexpr double MUTATION_BASE_PROB = 0.25;
static constexpr bool BANG_BANG_THRUST = true; /* OPT: discrete thr {0,200} */
static constexpr int SA_NUM_STARTS = 5;
static constexpr double SA_INIT_TEMP = 800.0;
static constexpr double SA_COOLING_RATE = 0.9993;
static constexpr double SA_MIN_TEMP = 0.1;
static constexpr int SA_MUTATIONS_MIN = 1;
static constexpr int SA_MUTATIONS_MAX = 3;
static constexpr double SA_SHIELD_TOGGLE_PROB = 0.05;
static constexpr double SA_BOOST_TOGGLE_PROB = 0.02;
static constexpr double SA_THRUST_RANDOMIZE_PROB = 0.3;
static constexpr double SA_ANGLE_STDDEV_MIN = 1.0;
static constexpr double SA_ANGLE_STDDEV_MAX = 10.0;
static constexpr int SA_MIN_MUTATIONS = 1;
static constexpr int SA_MAX_MUTATIONS = 3;
static constexpr double SA_THRUST_RAND_PROB = 0.3;
static constexpr double WIN_BONUS = 1e9;
static constexpr double CEM_ELITE_FRAC = 0.15;
static constexpr double CEM_ALPHA = 0.75;
static constexpr double CEM_INIT_ANGLE_STD = 12.0;
static constexpr double CEM_MIN_ANGLE_STD = 1.0;
static constexpr int CEM_N_THRUST_CATS = 5;
static constexpr int CEM_THRUST_CATS[CEM_N_THRUST_CATS] = {0, 50, 100, 150, 200};
static constexpr double CP_CROSS_BONUS = 15000.0;
static constexpr double ACTIVATION_PENALTY = 1000.0;
static constexpr double PROGRESS_DIFF_WEIGHT = 5000.0;
static constexpr double OPP_PROGRESS_PENALTY = 10000.0;
static constexpr double OPP_BLOCKER_PROX_WEIGHT = 0.5;
static constexpr double BLOCKER_DIST_THRESHOLD = 2200.0;
static constexpr double BLOCKER_SHIELD_DIST = 850.0;
static constexpr double BLOCKER_INTERCEPT_LEAD = 1.5;
static constexpr double BLOCKER_CP_OFFSET = 500.0;
static constexpr double BLOCKER_PROX_CLOSE = 800.0;
static constexpr double BLOCKER_PROX_MED = 2000.0;
static constexpr double BLOCKER_PROX_FAR = 5000.0;
static constexpr double BLOCKER_PROX_WEIGHT = 3.5;
static constexpr double BLOCKER_PERSTEP_PROX_WEIGHT = 0.5;
static constexpr double BLOCKER_INTERPOSE_WEIGHT = 0.8;
static constexpr double BLOCKER_SPEED_LOSS_WEIGHT = 1.5;
static constexpr double BLOCKER_MISALIGN_WEIGHT = 0.4;
static constexpr double HANDLING_AVG_DIST = 6500.0;
static constexpr int HANDLING_MIN_CPS = 5;
struct RNG {
unsigned s = 42;
unsigned xr() { s = (214013 * s + 2531011); return (s >> 16) & 0x7FFF; }
int ri(int a, int b) { return b <= a ? a : a + (int)(xr() % (unsigned)(b - a + 1)); }
double rd() { return (double)xr() / 32767.0; }
void seed(unsigned v) { s = v; }
};
inline int rand_thrust(RNG& rng) {
if (BANG_BANG_THRUST) return rng.ri(0, 1) * 200;
int raw = rng.ri(-100, 500);
return raw < 0 ? 0 : (raw > 200 ? 200 : raw);
}
inline int mutate_thrust(RNG& rng, int current) {
if (BANG_BANG_THRUST) return (current == 0) ? 200 : 0;
int v = current + rng.ri(-50, 50);
return v < 0 ? 0 : (v > 200 ? 200 : v);
}
struct Ind {
double ra[MAX_H], ba[MAX_H];
int rt[MAX_H], bt[MAX_H];
int rs, bs;
double sc;
};
inline double evaluate(
const Ind* ind, const csb::Game& base_game, int H,
const double* EX, const double* EY,
const double* DTE, const double* RRX, const double* RRY,
int ncp, int laps, int mx,
int rp, int bp, int orp, int obp, int rb,
int fboost, int rtimeout,
double dw, double aw, double sw, double lw, double apw,
double byw, double opw, double sfw, double faw, double rw,
const double* opp_ra = nullptr, const int* opp_rt = nullptr,
const double* opp_ba = nullptr, const int* opp_bt = nullptr
) {
/* OPT: avoid full Game copy (globalCp[256] ~4KB) — copy pods + used CPs only */
csb::Game sim;
std::memcpy(sim.pods, base_game.pods, sizeof(sim.pods));
sim.globalCpSize = base_game.globalCpSize;
std::memcpy(sim.globalCp, base_game.globalCp, (size_t)base_game.globalCpSize * sizeof(csb::Point));
sim.playerTimeout[0] = base_game.playerTimeout[0];
sim.playerTimeout[1] = base_game.playerTimeout[1];
int ir_cp = sim.pods[rp].next;
int ior_cp = sim.pods[orp].next;
double r_act = (double)H + 0.3, o_act = (double)H + 0.3;
double opp_init_speed = sqrt(sim.pods[orp].vel.x*sim.pods[orp].vel.x + sim.pods[orp].vel.y*sim.pods[orp].vel.y);
double blocker_prox_sum = 0.0;
for (int t = 0; t < H; t++) {
int pre_next[4];
for (int k = 0; k < 4; k++) pre_next[k] = sim.pods[k].next;
int rth = ind->rt[t];
int rb_flag = 0, rs_flag = 0;
if (t == 0 && fboost && sim.pods[rp].shieldtimer == 0) rb_flag = 1;
else if (t == ind->rs && ind->rs < 3 && sim.pods[rp].shieldtimer == 0) rs_flag = 1;
sim.pods[rp].applyActionDelta(ind->ra[t], rth, rs_flag, rb_flag);
int bth = ind->bt[t];
int bs_flag = 0;
if (t == ind->bs && ind->bs < 3 && sim.pods[bp].shieldtimer == 0) bs_flag = 1;
sim.pods[bp].applyActionDelta(ind->ba[t], bth, bs_flag, 0);
if (opp_ra) {
sim.pods[orp].applyActionDelta(opp_ra[t], opp_rt[t], 0, 0);
sim.pods[obp].applyActionDelta(opp_ba[t], opp_bt[t], 0, 0);
} else {
/* OPT: string-free opp actions via applyRotate+applyThrust (no string_view parse) */
{
csb::Pod& op = sim.pods[orp];
double tx = EX[op.next % ncp], ty = EY[op.next % ncp];
double ta = std::atan2(ty - op.pos.y, tx - op.pos.x);
if (op.isFirstTurn) op.applyRotateFirst(ta); else op.applyRotate(ta);
if (op.shieldtimer == 0) op.applyThrust(200);
}
{
csb::Pod& bp2 = sim.pods[obp];
double tx2 = sim.pods[rp].pos.x + sim.pods[rp].vel.x * 1.5;
double ty2 = sim.pods[rp].pos.y + sim.pods[rp].vel.y * 1.5;
double dx_cp = sim.globalCp[sim.pods[rp].next].x - sim.pods[rp].pos.x;
double dy_cp = sim.globalCp[sim.pods[rp].next].y - sim.pods[rp].pos.y;
double dcp = sqrt(dx_cp * dx_cp + dy_cp * dy_cp);
if (dcp > 0) { tx2 -= 500 * dx_cp / dcp; ty2 -= 500 * dy_cp / dcp; }
double dd = sqrt((bp2.pos.x - sim.pods[rp].pos.x) * (bp2.pos.x - sim.pods[rp].pos.x) +
(bp2.pos.y - sim.pods[rp].pos.y) * (bp2.pos.y - sim.pods[rp].pos.y));
int sh = (dd < 850 && bp2.shieldtimer == 0) ? 1 : 0;
if (sh) {
bp2.shieldtimer = csb::SHIELD_ACTIVE_TIMER;
} else {
double ta = std::atan2(ty2 - bp2.pos.y, tx2 - bp2.pos.x);
if (bp2.isFirstTurn) bp2.applyRotateFirst(ta); else bp2.applyRotate(ta);
if (bp2.shieldtimer == 0) bp2.applyThrust(200);
}
}
}
sim.nextTurn();
for (int k = 0; k < 4; k++) {
if (sim.pods[k].next != pre_next[k]) {
if (k == rp && r_act > (double)H) r_act = (double)t + 0.5;
if (k == orp && o_act > (double)H) o_act = (double)t + 0.5;
}
}
if (!rtimeout) {
double bdx = sim.pods[bp].pos.x - sim.pods[orp].pos.x;
double bdy = sim.pods[bp].pos.y - sim.pods[orp].pos.y;
double bd2 = bdx * bdx + bdy * bdy;
if (bd2 < BLOCKER_PROX_FAR * BLOCKER_PROX_FAR) {
double bd = sqrt(bd2);
double norm = 1.0 - bd / BLOCKER_PROX_FAR;
blocker_prox_sum += norm * norm;
}
}
}
if (sim.pods[rp].pos.x < -2000 || sim.pods[rp].pos.x > 18000 ||
sim.pods[rp].pos.y < -2000 || sim.pods[rp].pos.y > 11000)
return -1e9;
double s = 0;
for (int k = 0; k < 2; k++) {
int pi2 = (k == 0) ? rp : bp;
if (sim.pods[pi2].pos.x < -1000 || sim.pods[pi2].pos.x > 17000 ||
sim.pods[pi2].pos.y < -1000 || sim.pods[pi2].pos.y > 10000)
s -= 100000.0;
}
if (sim.pods[rp].won) s += WIN_BONUS;
if (sim.pods[orp].won) s -= WIN_BONUS;
{
int cur = sim.pods[rp].next;
int ini = ir_cp;
int crossed = cur - ini;
if (crossed > 0) s += crossed * CP_CROSS_BONUS;
}
if (!sim.pods[rp].won) {
int rl = sim.pods[rp].next;
if (rl < mx) {
double dx = sim.pods[rp].pos.x - EX[sim.pods[rp].next % ncp];
double dy = sim.pods[rp].pos.y - EY[sim.pods[rp].next % ncp];
s -= (DTE[rl] + sqrt(dx * dx + dy * dy)) * dw;
}
}
{
int our_progress = sim.pods[rp].next;
int opp_progress = sim.pods[orp].next;
int diff = our_progress - opp_progress;
s += diff * PROGRESS_DIFF_WEIGHT;
if (diff == 0) {
double our_dx = sim.pods[rp].pos.x - sim.globalCp[sim.pods[rp].next].x, our_dy = sim.pods[rp].pos.y - sim.globalCp[sim.pods[rp].next].y;
double opp_dx = sim.pods[orp].pos.x - sim.globalCp[sim.pods[orp].next].x, opp_dy = sim.pods[orp].pos.y - sim.globalCp[sim.pods[orp].next].y;
s += (sqrt(opp_dx*opp_dx+opp_dy*opp_dy) - sqrt(our_dx*our_dx+our_dy*our_dy)) * 0.5;
}
}
{
double dx_to_cp = sim.globalCp[sim.pods[rp].next].x - sim.pods[rp].pos.x, dy_to_cp = sim.globalCp[sim.pods[rp].next].y - sim.pods[rp].pos.y;
double dist_to_cp = sqrt(dx_to_cp*dx_to_cp + dy_to_cp*dy_to_cp);
if (dist_to_cp < 3000.0) {
int next_next = (sim.pods[rp].next % ncp + 1) % ncp;
double nnx = sim.globalCp[next_next].x - sim.pods[rp].pos.x, nny = sim.globalCp[next_next].y - sim.pods[rp].pos.y;
double nnd = sqrt(nnx*nnx + nny*nny);
if (nnd > 0) { double alignment = (sim.pods[rp].vel.x*nnx + sim.pods[rp].vel.y*nny) / nnd; s += alignment * 1.5; }
}
}
{
double dx = EX[sim.pods[rp].next % ncp] - sim.pods[rp].pos.x;
double dy = EY[sim.pods[rp].next % ncp] - sim.pods[rp].pos.y;
double d = sqrt(dx * dx + dy * dy);
if (d > 0) {
double nx = dx / d, ny = dy / d;
s += (sim.pods[rp].vel.x * nx + sim.pods[rp].vel.y * ny) * aw;
double lat = sim.pods[rp].vel.x * ny - sim.pods[rp].vel.y * nx;
s -= fabs(lat) * lw;
double ta = atan2(dy, dx);
double ae = ta - sim.pods[rp].angle;
while (ae > PI) ae -= 2 * PI;
while (ae < -PI) ae += 2 * PI;
s -= fabs(ae * RAD_TO_DEG) * apw;
}
s += sqrt(sim.pods[rp].vel.x * sim.pods[rp].vel.x + sim.pods[rp].vel.y * sim.pods[rp].vel.y) * sw;
}
{
double dx_cp = sim.globalCp[sim.pods[rp].next].x - sim.pods[rp].pos.x;
double dy_cp = sim.globalCp[sim.pods[rp].next].y - sim.pods[rp].pos.y;
double dist_cp_sq = dx_cp * dx_cp + dy_cp * dy_cp;
if (dist_cp_sq < 9000000.0) {
int nn = (sim.pods[rp].next % ncp + 1) % ncp;
double nnx = sim.globalCp[nn].x - sim.pods[rp].pos.x, nny = sim.globalCp[nn].y - sim.pods[rp].pos.y;
double nnd_sq = nnx * nnx + nny * nny;
if (nnd_sq > 1.0) {
double nnd = sqrt(nnd_sq);
double alignment = (sim.pods[rp].vel.x * nnx + sim.pods[rp].vel.y * nny) / nnd;
s += alignment * 0.3;
}
}
}
s -= ACTIVATION_PENALTY * r_act;
{
double pred_obx = sim.pods[obp].pos.x + sim.pods[obp].vel.x * 1.85;
double pred_oby = sim.pods[obp].pos.y + sim.pods[obp].vel.y * 1.85;
double pred_rx = sim.pods[rp].pos.x + sim.pods[rp].vel.x * 1.85;
double pred_ry = sim.pods[rp].pos.y + sim.pods[rp].vel.y * 1.85;
double dx = pred_obx - pred_rx, dy = pred_oby - pred_ry;
double d2 = dx*dx + dy*dy;
double threshold2 = 3000.0 * 3000.0;
if (d2 < threshold2) {
double penalty = byw * 20.0 * (1.0 - d2 / threshold2);
double cvx = sim.pods[obp].vel.x - sim.pods[rp].vel.x, cvy = sim.pods[obp].vel.y - sim.pods[rp].vel.y;
double closing = -(dx*cvx + dy*cvy);
if (closing > 0 && d2 > 1.0) penalty += closing * 0.5;
if (sim.pods[obp].shieldtimer >= 3) penalty *= 1.5;
s -= penalty;
}
}
s += 10000.0 * opw * o_act;
if (sim.pods[orp].next == ior_cp) {
double dx = sim.pods[orp].pos.x - sim.globalCp[sim.pods[orp].next].x, dy = sim.pods[orp].pos.y - sim.globalCp[sim.pods[orp].next].y;
s += 10.0 * opw * sqrt(dx * dx + dy * dy);
}
if (rtimeout == 1) {
if (!sim.pods[bp].won) {
int bl = sim.pods[bp].next;
if (bl < mx) {
double dx = sim.pods[bp].pos.x - EX[sim.pods[bp].next % ncp], dy = sim.pods[bp].pos.y - EY[sim.pods[bp].next % ncp];
s -= (DTE[bl] + sqrt(dx * dx + dy * dy)) * dw;
}
}
} else if (rtimeout == 2) {
double dx_br = sim.pods[rp].pos.x - sim.pods[bp].pos.x, dy_br = sim.pods[rp].pos.y - sim.pods[bp].pos.y;
double dist_br = sqrt(dx_br*dx_br + dy_br*dy_br);
if (dist_br < 2000) {
s += (2000 - dist_br) * 2.0;
} else {
s -= (dist_br - 2000) * 1.0;
}
double cp_dx = sim.globalCp[sim.pods[rp].next].x - sim.pods[rp].pos.x;
double cp_dy = sim.globalCp[sim.pods[rp].next].y - sim.pods[rp].pos.y;
double cp_d = sqrt(cp_dx*cp_dx + cp_dy*cp_dy);
if (cp_d > 1.0 && dist_br > 1.0) {
double alignment = (dx_br * cp_dx + dy_br * cp_dy) / (dist_br * cp_d);
s += alignment * 3000.0;
}
if (dist_br > 1.0) {
double vel_toward = (sim.pods[bp].vel.x * dx_br + sim.pods[bp].vel.y * dy_br) / dist_br;
if (vel_toward > 0) s += vel_toward * 5.0;
}
} else {
{
double bx = sim.pods[bp].pos.x - sim.pods[orp].pos.x, by = sim.pods[bp].pos.y - sim.pods[orp].pos.y;
double cx = sim.globalCp[rb].x - sim.pods[orp].pos.x, cy = sim.globalCp[rb].y - sim.pods[orp].pos.y;
s -= sfw * atan2(fabs(bx * cy - by * cx), bx * cx + by * cy);
}
{
double dx = sim.pods[orp].pos.x - sim.pods[bp].pos.x, dy = sim.pods[orp].pos.y - sim.pods[bp].pos.y;
double ta = atan2(dy, dx);
double ae = ta - sim.pods[bp].angle;
while (ae > PI) ae -= 2 * PI;
while (ae < -PI) ae += 2 * PI;
s -= faw * fabs(ae * RAD_TO_DEG);
}
{
double dx = sim.pods[bp].pos.x - RRX[rb % ncp], dy = sim.pods[bp].pos.y - RRY[rb % ncp];
double d = sqrt(dx * dx + dy * dy);
s -= rw * (d < 300 ? (d - 300) * 0.1 : d - 300);
}
{
double dx = sim.pods[bp].pos.x - sim.pods[orp].pos.x, dy = sim.pods[bp].pos.y - sim.pods[orp].pos.y;
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
{
double opp_to_cp_x = sim.globalCp[sim.pods[orp].next].x - sim.pods[orp].pos.x;
double opp_to_cp_y = sim.globalCp[sim.pods[orp].next].y - sim.pods[orp].pos.y;
double opp_to_cp_d = sqrt(opp_to_cp_x * opp_to_cp_x + opp_to_cp_y * opp_to_cp_y);
if (opp_to_cp_d > 1.0) {
double nx = opp_to_cp_x / opp_to_cp_d;
double ny = opp_to_cp_y / opp_to_cp_d;
double bx = sim.pods[bp].pos.x - sim.pods[orp].pos.x;
double by = sim.pods[bp].pos.y - sim.pods[orp].pos.y;
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
s += CP_CROSS_BONUS * o_act;
{
double opp_end_speed = sqrt(sim.pods[orp].vel.x*sim.pods[orp].vel.x + sim.pods[orp].vel.y*sim.pods[orp].vel.y);
double speed_loss = opp_init_speed - opp_end_speed;
if (speed_loss > 0) s += speed_loss * BLOCKER_SPEED_LOSS_WEIGHT;
}
{
double dx = sim.globalCp[sim.pods[orp].next].x - sim.pods[orp].pos.x;
double dy = sim.globalCp[sim.pods[orp].next].y - sim.pods[orp].pos.y;
double d = sqrt(dx*dx + dy*dy);
if (d > 0) {
double alignment = (sim.pods[orp].vel.x*dx + sim.pods[orp].vel.y*dy) / d;
s -= alignment * BLOCKER_MISALIGN_WEIGHT;
}
}
s += BLOCKER_PERSTEP_PROX_WEIGHT * blocker_prox_sum * 1000.0;
}
if (ind->rs < 3) s -= 5000.0;
if (ind->bs < 3) s -= 2000.0;
if (ind->rs >= H && ind->rt[0] > 0) s += ind->rt[0] * 0.16;
if (ind->bs >= H && ind->bt[0] > 0) s += ind->bt[0] * 0.01;
return s;
}
inline double evaluate_opp_perspective(
const Ind* ind, const csb::Game& base_game, int H,
const double* EX, const double* EY,
const double* DTE, int ncp, int laps, int mx,
int rp, int bp, int orp, int obp,
const double* our_ra, const int* our_rt,
const double* our_ba, const int* our_bt,
double dw, double aw, double sw
) {
csb::Game sim = base_game;
int ior_cp = sim.pods[orp].next;
int ir_cp = sim.pods[rp].next;
for (int t = 0; t < H; t++) {
sim.pods[rp].applyActionDelta(our_ra[t], our_rt[t], 0, 0);
sim.pods[bp].applyActionDelta(our_ba[t], our_bt[t], 0, 0);
sim.pods[orp].applyActionDelta(ind->ra[t], ind->rt[t], 0, 0);
sim.pods[obp].applyActionDelta(ind->ba[t], ind->bt[t], 0, 0);
sim.nextTurn();
}
if (sim.pods[orp].pos.x < -2000 || sim.pods[orp].pos.x > 18000 ||
sim.pods[orp].pos.y < -2000 || sim.pods[orp].pos.y > 11000)
return -1e9;
double s = 0;
if (sim.pods[orp].won) s += WIN_BONUS;
if (sim.pods[rp].won) s -= WIN_BONUS;
{ int cur=sim.pods[orp].next, ini=ior_cp;
if(cur-ini>0) s+=(cur-ini)*CP_CROSS_BONUS; }
if (!sim.pods[orp].won) { int rl=sim.pods[orp].next;
if (rl<mx) { double dx=sim.pods[orp].pos.x-EX[sim.pods[orp].next % ncp],dy=sim.pods[orp].pos.y-EY[sim.pods[orp].next % ncp];
s -= dw*(DTE[rl]+sqrt(dx*dx+dy*dy)); } }
{ double dx=EX[sim.pods[orp].next % ncp]-sim.pods[orp].pos.x,dy=EY[sim.pods[orp].next % ncp]-sim.pods[orp].pos.y,d=sqrt(dx*dx+dy*dy);
if(d>0){double nx=dx/d,ny=dy/d; s+=(sim.pods[orp].vel.x*nx+sim.pods[orp].vel.y*ny)*aw;}
s+=sqrt(sim.pods[orp].vel.x*sim.pods[orp].vel.x+sim.pods[orp].vel.y*sim.pods[orp].vel.y)*sw; }
{ int opp_progress=sim.pods[orp].next; int our_progress=sim.pods[rp].next;
int diff=opp_progress-our_progress; s+=diff*PROGRESS_DIFF_WEIGHT;
if(diff==0){double opp_dx=sim.pods[orp].pos.x-sim.globalCp[sim.pods[orp].next].x,opp_dy=sim.pods[orp].pos.y-sim.globalCp[sim.pods[orp].next].y;
double our_dx=sim.pods[rp].pos.x-sim.globalCp[sim.pods[rp].next].x,our_dy=sim.pods[rp].pos.y-sim.globalCp[sim.pods[rp].next].y;
s+=(sqrt(our_dx*our_dx+our_dy*our_dy)-sqrt(opp_dx*opp_dx+opp_dy*opp_dy))*0.5;} }
{ int cur=sim.pods[rp].next, ini=ir_cp;
if(cur-ini>0) s-=(cur-ini)*10000.0; }
{ double dx=sim.pods[obp].pos.x-sim.pods[rp].pos.x,dy=sim.pods[obp].pos.y-sim.pods[rp].pos.y;
s -= 0.5*sqrt(dx*dx+dy*dy); }
return s;
}
static constexpr int OPP_PS = OPP_POP_SIZE;
inline void run_opp_prediction_ga(
RNG& rng, const csb::Game& base_game, int H, int mx,
const double* EX, const double* EY,
const double* DTE, int ncp, int laps,
int rp, int bp, int orp, int obp,
const double* our_ra, const int* our_rt,
const double* our_ba, const int* our_bt,
const double* opp_prev_ra, const int* opp_prev_rt,
const double* opp_prev_ba, const int* opp_prev_bt,
int has_opp_prev,
int budget_us,
double dw, double aw, double sw,
int has_obs, double obs_r_delta, int obs_r_thrust,
double obs_b_delta, int obs_b_thrust,
double* out_ora, int* out_ort, double* out_oba, int* out_obt
) {
Ind pop[OPP_PS];
int idx = 0;
if (has_opp_prev) {
for (int t=0;t<H-1;t++){pop[0].ra[t]=opp_prev_ra[t+1];pop[0].rt[t]=opp_prev_rt[t+1];
pop[0].ba[t]=opp_prev_ba[t+1];pop[0].bt[t]=opp_prev_bt[t+1];}
pop[0].ra[H-1]=rng.rd()*36-18;pop[0].rt[H-1]=rand_thrust(rng);
pop[0].ba[H-1]=rng.rd()*36-18;pop[0].bt[H-1]=rand_thrust(rng);
pop[0].rs=H;pop[0].bs=H;idx=1;
}
if (idx < OPP_PS) {
for (int t=0;t<H;t++){pop[idx].ra[t]=0;pop[idx].rt[t]=200;pop[idx].ba[t]=0;pop[idx].bt[t]=200;}
pop[idx].rs=H;pop[idx].bs=H;idx++;
}
if (has_obs && idx < OPP_PS) {
for (int t=0;t<H;t++){
pop[idx].ra[t]=obs_r_delta;pop[idx].rt[t]=obs_r_thrust;
pop[idx].ba[t]=obs_b_delta;pop[idx].bt[t]=obs_b_thrust;
}
pop[idx].rs=H;pop[idx].bs=H;idx++;
}
for (int i=idx;i<OPP_PS;i++) {
for (int t=0;t<H;t++) {
double v=rng.ri(-400,400)/10.0;pop[i].ra[t]=v<-18?-18:(v>18?18:v);
pop[i].rt[t]=rand_thrust(rng);
v=rng.ri(-400,400)/10.0;pop[i].ba[t]=v<-18?-18:(v>18?18:v);
pop[i].bt[t]=rand_thrust(rng);
}
pop[i].rs=H;pop[i].bs=H;
}
int best=0,worst=0;
for (int i=0;i<OPP_PS;i++) {
pop[i].sc=evaluate_opp_perspective(&pop[i],base_game,H,EX,EY,DTE,ncp,laps,mx,rp,bp,orp,obp,our_ra,our_rt,our_ba,our_bt,dw,aw,sw);
if(pop[i].sc>pop[best].sc)best=i;
if(pop[i].sc<pop[worst].sc)worst=i;
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
if(rng.rd()<0.5){child.rt[t]=rand_thrust(rng);}
if(rng.rd()<0.5){double v=rng.ri(-400,400)/10.0;child.ba[t]=v<-18?-18:(v>18?18:v);}
if(rng.rd()<0.5){child.bt[t]=rand_thrust(rng);}}}
child.rs=H;child.bs=H;
{int si=rng.ri(0,H-1);double v=child.ra[si]+rng.rd()*24-12;child.ra[si]=v<-18?-18:(v>18?18:v);}
{int si=rng.ri(0,H-1);child.rt[si]=mutate_thrust(rng,child.rt[si]);}
{int si=rng.ri(0,H-1);double v=child.ba[si]+rng.rd()*24-12;child.ba[si]=v<-18?-18:(v>18?18:v);}
{int si=rng.ri(0,H-1);child.bt[si]=mutate_thrust(rng,child.bt[si]);}
child.sc=evaluate_opp_perspective(&child,base_game,H,EX,EY,DTE,ncp,laps,mx,rp,bp,orp,obp,our_ra,our_rt,our_ba,our_bt,dw,aw,sw);
if(child.sc>pop[best].sc)best=worst;
pop[worst]=child;
if(child.sc>wsc){worst=0;wsc=pop[0].sc;for(int i=1;i<OPP_PS;i++)if(pop[i].sc<wsc){worst=i;wsc=pop[i].sc;}}
}
memcpy(out_ora,pop[best].ra,H*sizeof(double));memcpy(out_ort,pop[best].rt,H*sizeof(int));
memcpy(out_oba,pop[best].ba,H*sizeof(double));memcpy(out_obt,pop[best].bt,H*sizeof(int));
}
static constexpr int MCTS_N_ANGLES = 7;
static constexpr int MCTS_N_THRUSTS = 4;
static constexpr int MCTS_N_CANDIDATES = MCTS_N_ANGLES * MCTS_N_THRUSTS;
static constexpr double MCTS_ANGLES[MCTS_N_ANGLES] = {-18, -12, -6, 0, 6, 12, 18};
static constexpr int MCTS_THRUSTS[MCTS_N_THRUSTS] = {0, 100, 150, 200};
static constexpr int MCTS_TOP_K = 5;
inline int mcts_first_move_probe(
RNG& rng,
const csb::Game& base_game, int H,
const double* EX, const double* EY,
const double* DTE, const double* RRX, const double* RRY,
int ncp, int laps, int mx,
int rp, int bp, int orp, int obp, int rb,
int fboost, int rtimeout,
double dw, double aw, double sw, double lw, double apw,
double byw, double opw, double sfw, double faw, double rw,
const double* opp_ra, const int* opp_rt,
const double* opp_ba, const int* opp_bt,
int budget_us,
Ind* out_seeds
) {
double sum_score[MCTS_N_CANDIDATES] = {};
int visit_count[MCTS_N_CANDIDATES] = {};
int total_visits = 0;
Ind probe;
probe.rs = H; probe.bs = H;
struct timespec t0, tn;
clock_gettime(CLOCK_MONOTONIC, &t0);
int round = 0;
while (1) {
int c = round % MCTS_N_CANDIDATES;
round++;
int ai = c / MCTS_N_THRUSTS;
int ti = c % MCTS_N_THRUSTS;
probe.ra[0] = MCTS_ANGLES[ai];
probe.rt[0] = MCTS_THRUSTS[ti];
for (int t = 1; t < H; t++) {
double v = rng.ri(-400, 400) / 10.0;
probe.ra[t] = v < -18 ? -18 : (v > 18 ? 18 : v);
int raw = rng.ri(-100, 500);
probe.rt[t] = BANG_BANG_THRUST ? (rng.ri(0,1)*200) : (raw < 0 ? 0 : (raw > 200 ? 200 : raw));
}
for (int t = 0; t < H; t++) {
double v = rng.ri(-400, 400) / 10.0;
probe.ba[t] = v < -18 ? -18 : (v > 18 ? 18 : v);
int raw = rng.ri(-100, 500);
probe.bt[t] = raw < 0 ? 0 : (raw > 200 ? 200 : raw);
}
double sc = evaluate(&probe, base_game, H, EX, EY, DTE, RRX, RRY,
ncp, laps, mx, rp, bp, orp, obp, rb, fboost, rtimeout,
dw, aw, sw, lw, apw, byw, opw, sfw, faw, rw,
opp_ra, opp_rt, opp_ba, opp_bt);
sum_score[c] += sc;
visit_count[c]++;
total_visits++;
if (c == MCTS_N_CANDIDATES - 1) {
clock_gettime(CLOCK_MONOTONIC, &tn);
long el = (tn.tv_sec - t0.tv_sec) * 1000000L + (tn.tv_nsec - t0.tv_nsec) / 1000L;
if (el >= budget_us) break;
}
}
int ranked[MCTS_N_CANDIDATES];
for (int i = 0; i < MCTS_N_CANDIDATES; i++) ranked[i] = i;
for (int i = 0; i < MCTS_TOP_K; i++) {
int best_j = i;
double best_avg = sum_score[ranked[i]] / visit_count[ranked[i]];
for (int j = i + 1; j < MCTS_N_CANDIDATES; j++) {
double avg = sum_score[ranked[j]] / visit_count[ranked[j]];
if (avg > best_avg) { best_avg = avg; best_j = j; }
}
if (best_j != i) { int tmp = ranked[i]; ranked[i] = ranked[best_j]; ranked[best_j] = tmp; }
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
double v = rng.ri(-400, 400) / 10.0;
seed.ra[t] = v < -18 ? -18 : (v > 18 ? 18 : v);
int raw = rng.ri(-100, 500);
seed.rt[t] = BANG_BANG_THRUST ? (rng.ri(0,1)*200) : (raw < 0 ? 0 : (raw > 200 ? 200 : raw));
}
for (int t = 0; t < H; t++) {
double v = rng.ri(-400, 400) / 10.0;
seed.ba[t] = v < -18 ? -18 : (v > 18 ? 18 : v);
int raw = rng.ri(-100, 500);
seed.bt[t] = raw < 0 ? 0 : (raw > 200 ? 200 : raw);
}
seed.rs = H; seed.bs = H;
seeds_created++;
}
return seeds_created;
}
inline int run_combined_ga(
RNG& rng,
const csb::Game& base_game, int H, int PS, int mx,
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
if (has_prev) {
for (int t = 0; t < H - 1; t++) {
pop[0].ra[t] = prev_ra[t + 1]; pop[0].rt[t] = prev_rt[t + 1];
pop[0].ba[t] = prev_ba[t + 1]; pop[0].bt[t] = prev_bt[t + 1];
}
pop[0].ra[H - 1] = rng.rd() * 36 - 18; pop[0].rt[H - 1] = rand_thrust(rng);
pop[0].ba[H - 1] = rng.rd() * 36 - 18; pop[0].bt[H - 1] = rand_thrust(rng);
pop[0].rs = (prev_rs > 0 && prev_rs < H) ? prev_rs - 1 : H;
pop[0].bs = (prev_bs > 0 && prev_bs < H) ? prev_bs - 1 : H;
idx = 1;
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
{
csb::Game hsim = base_game;
for (int t = 0; t < H; t++) {
double tx = EX[hsim.pods[rp].next % ncp], ty = EY[hsim.pods[rp].next % ncp];
double ddx = tx - hsim.pods[rp].pos.x, ddy = ty - hsim.pods[rp].pos.y;
double dd = sqrt(ddx * ddx + ddy * ddy);
if (dd > 0) {
double ux = ddx / dd, uy = ddy / dd;
double perp = hsim.pods[rp].vel.x * (-uy) + hsim.pods[rp].vel.y * ux;
double cf = fabs(perp) * 3.0; if (cf > 800) cf = 800;
if (perp > 0) { tx += uy * cf; ty -= ux * cf; }
else { tx -= uy * cf; ty += ux * cf; }
if (dd < 2500) {
int nnc = (hsim.pods[rp].next % ncp + 1) % ncp;
double bl = (2500 - dd) / 2500;
tx = tx * (1 - bl) + EX[nnc] * bl; ty = ty * (1 - bl) + EY[nnc] * bl;
}
}
double ta = atan2(ty - hsim.pods[rp].pos.y, tx - hsim.pods[rp].pos.x);
double diff = ta - hsim.pods[rp].angle;
while (diff > PI) diff -= 2 * PI;
while (diff < -PI) diff += 2 * PI;
double diff_deg = diff * RAD_TO_DEG;
if (diff_deg > 18) diff_deg = 18;
if (diff_deg < -18) diff_deg = -18;
if (idx < PS) { pop[idx].ra[t] = diff_deg; pop[idx].rt[t] = 200; }
if (idx + 1 < PS) { pop[idx + 1].ra[t] = diff_deg; pop[idx + 1].rt[t] = 150; }
double ang_rad2 = hsim.pods[rp].angle + diff_deg * DEG_TO_RAD;
double r_tx2 = hsim.pods[rp].pos.x + cos(ang_rad2) * 10000.0;
double r_ty2 = hsim.pods[rp].pos.y + sin(ang_rad2) * 10000.0;
hsim.applyAction(rp, (int)r_tx2, (int)r_ty2, "200");
double btx = RRX[rb % ncp], bty = RRY[rb % ncp];
double bd = sqrt((hsim.pods[bp].pos.x - hsim.pods[orp].pos.x) * (hsim.pods[bp].pos.x - hsim.pods[orp].pos.x) +
(hsim.pods[bp].pos.y - hsim.pods[orp].pos.y) * (hsim.pods[bp].pos.y - hsim.pods[orp].pos.y));
if (bd < 3000) { btx = hsim.pods[orp].pos.x + hsim.pods[orp].vel.x * 2; bty = hsim.pods[orp].pos.y + hsim.pods[orp].vel.y * 2; }
double bta = atan2(bty - hsim.pods[bp].pos.y, btx - hsim.pods[bp].pos.x);
double bdiff = bta - hsim.pods[bp].angle;
while (bdiff > PI) bdiff -= 2 * PI;
while (bdiff < -PI) bdiff += 2 * PI;
double bdiff_deg = bdiff * RAD_TO_DEG;
if (bdiff_deg > 18) bdiff_deg = 18;
if (bdiff_deg < -18) bdiff_deg = -18;
if (idx < PS) { pop[idx].ba[t] = bdiff_deg; pop[idx].bt[t] = 200; }
if (idx + 1 < PS) { pop[idx + 1].ba[t] = bdiff_deg; pop[idx + 1].bt[t] = 200; }
double bang_rad2 = hsim.pods[bp].angle + bdiff_deg * DEG_TO_RAD;
double b_tx2 = hsim.pods[bp].pos.x + cos(bang_rad2) * 10000.0;
double b_ty2 = hsim.pods[bp].pos.y + sin(bang_rad2) * 10000.0;
hsim.applyAction(bp, (int)b_tx2, (int)b_ty2, "200");
hsim.applyAction(orp, (int)EX[hsim.pods[orp].next % ncp], (int)EY[hsim.pods[orp].next % ncp], "200");
hsim.applyAction(obp, (int)hsim.pods[rp].pos.x, (int)hsim.pods[rp].pos.y, "200");
hsim.nextTurn();
}
if (idx < PS) { pop[idx].rs = H; pop[idx].bs = H; idx++; }
if (idx < PS) { pop[idx].rs = H; pop[idx].bs = H; idx++; }
}
if (idx + 1 < PS) {
csb::Game isim = base_game;
for (int t = 0; t < H; t++) {
double itx = EX[isim.pods[rp].next % ncp], ity = EY[isim.pods[rp].next % ncp];
double ita = atan2(ity - isim.pods[rp].pos.y, itx - isim.pods[rp].pos.x);
double idiff = ita - isim.pods[rp].angle;
while (idiff > PI) idiff -= 2 * PI;
while (idiff < -PI) idiff += 2 * PI;
double idiff_deg = idiff * RAD_TO_DEG;
if (idiff_deg > 18) idiff_deg = 18;
if (idiff_deg < -18) idiff_deg = -18;
pop[idx].ra[t] = idiff_deg; pop[idx].rt[t] = 200;
double pred_x = isim.pods[orp].pos.x + isim.pods[orp].vel.x * 3;
double pred_y = isim.pods[orp].pos.y + isim.pods[orp].vel.y * 3;
double ibta = atan2(pred_y - isim.pods[bp].pos.y, pred_x - isim.pods[bp].pos.x);
double ibdiff = ibta - isim.pods[bp].angle;
while (ibdiff > PI) ibdiff -= 2 * PI;
while (ibdiff < -PI) ibdiff += 2 * PI;
double ibdiff_deg = ibdiff * RAD_TO_DEG;
if (ibdiff_deg > 18) ibdiff_deg = 18;
if (ibdiff_deg < -18) ibdiff_deg = -18;
pop[idx].ba[t] = ibdiff_deg; pop[idx].bt[t] = 200;
isim.applyAction(rp, (int)(isim.pods[rp].pos.x + cos(isim.pods[rp].angle + idiff_deg * DEG_TO_RAD) * 10000.0),
(int)(isim.pods[rp].pos.y + sin(isim.pods[rp].angle + idiff_deg * DEG_TO_RAD) * 10000.0), "200");
isim.applyAction(bp, (int)(isim.pods[bp].pos.x + cos(isim.pods[bp].angle + ibdiff_deg * DEG_TO_RAD) * 10000.0),
(int)(isim.pods[bp].pos.y + sin(isim.pods[bp].angle + ibdiff_deg * DEG_TO_RAD) * 10000.0), "200");
isim.applyAction(orp, (int)EX[isim.pods[orp].next % ncp], (int)EY[isim.pods[orp].next % ncp], "200");
isim.applyAction(obp, (int)isim.pods[rp].pos.x, (int)isim.pods[rp].pos.y, "200");
isim.nextTurn();
}
pop[idx].rs = H; pop[idx].bs = H; idx++;
}
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
struct timespec mcts_t0, mcts_t1;
clock_gettime(CLOCK_MONOTONIC, &mcts_t0);
{
Ind mcts_seeds[MCTS_TOP_K];
int n_mcts = mcts_first_move_probe(rng, base_game, H, EX, EY, DTE, RRX, RRY,
ncp, laps, mx, rp, bp, orp, obp, rb,
fboost, rtimeout,
dw, aw, sw, lw, apw, byw, opw, sfw, faw, rw,
opp_ra, opp_rt, opp_ba, opp_bt,
MCTS_BUDGET_US, mcts_seeds);
for (int k = 0; k < n_mcts && idx < PS; k++) {
pop[idx] = mcts_seeds[k];
idx++;
}
}
clock_gettime(CLOCK_MONOTONIC, &mcts_t1);
long mcts_elapsed_us = (mcts_t1.tv_sec - mcts_t0.tv_sec) * 1000000L +
(mcts_t1.tv_nsec - mcts_t0.tv_nsec) / 1000L;
int ga_budget_us = budget_us - (int)mcts_elapsed_us;
if (ga_budget_us < MIN_PHASE2_BUDGET_US) ga_budget_us = MIN_PHASE2_BUDGET_US;
for (int i = idx; i < PS; i++) {
for (int t = 0; t < H; t++) {
double v = rng.ri(-400, 400) / 10.0;
pop[i].ra[t] = v < -18 ? -18 : (v > 18 ? 18 : v);
pop[i].rt[t] = rand_thrust(rng);
v = rng.ri(-400, 400) / 10.0;
pop[i].ba[t] = v < -18 ? -18 : (v > 18 ? 18 : v);
pop[i].bt[t] = rand_thrust(rng);
}
pop[i].rs = rng.ri(0, H + 4); pop[i].bs = rng.ri(0, H + 4);
}
int best = 0, worst = 0;
for (int i = 0; i < PS; i++) {
pop[i].sc = evaluate(&pop[i], base_game, H, EX, EY, DTE, RRX, RRY,
ncp, laps, mx, rp, bp, orp, obp, rb, fboost, rtimeout,
dw, aw, sw, lw, apw, byw, opw, sfw, faw, rw,
opp_ra, opp_rt, opp_ba, opp_bt);
if (pop[i].sc > pop[best].sc) best = i;
if (pop[i].sc < pop[worst].sc) worst = i;
}
double wsc = pop[worst].sc;
struct timespec t0, tn; clock_gettime(CLOCK_MONOTONIC, &t0);
int iters = 0; double amplitude = 1.0;
Ind child;
while (1) {
iters++;
if ((iters & 511) == 0) {
clock_gettime(CLOCK_MONOTONIC, &tn);
long el = (tn.tv_sec - t0.tv_sec) * 1000000L + (tn.tv_nsec - t0.tv_nsec) / 1000L;
if (el >= ga_budget_us) break;
amplitude = 1.0 - (double)el / (double)ga_budget_us;
if (amplitude < 0) amplitude = 0;
}
if (pop[best].sc < wsc + 0.3) {
for (int i = 0; i < PS; i++) if (i != best) pop[i].sc -= 2000;
wsc -= 2000;
}
int p1 = rng.ri(0, PS - 1), p2 = rng.ri(0, PS - 1);
int par = (pop[p1].sc >= pop[p2].sc) ? p1 : p2;
double threshold = 0.25 + amplitude;
if (rng.ri(0, 4) == 0) {
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
for (int t = 0; t < H; t++) {
child.ra[t] = pop[par].ra[t]; child.rt[t] = pop[par].rt[t];
child.ba[t] = pop[par].ba[t]; child.bt[t] = pop[par].bt[t];
if (rng.rd() < threshold) { double v = rng.ri(-400, 400) / 10.0; child.ra[t] = v < -18 ? -18 : (v > 18 ? 18 : v); }
if (rng.rd() < threshold) { child.rt[t] = rand_thrust(rng); }
if (rng.rd() < threshold) { double v = rng.ri(-400, 400) / 10.0; child.ba[t] = v < -18 ? -18 : (v > 18 ? 18 : v); }
if (rng.rd() < threshold) { child.bt[t] = rand_thrust(rng); }
}
child.rs = pop[par].rs; child.bs = pop[par].bs;
if (rng.rd() < threshold) child.rs = rng.ri(0, H + 4);
if (rng.rd() < threshold) child.bs = rng.ri(0, H + 4);
}
{ int si = rng.ri(0, H - 1); double v = child.ra[si] + rng.rd() * 24 - 12; child.ra[si] = v < -18 ? -18 : (v > 18 ? 18 : v); }
{ int si = rng.ri(0, H - 1); child.rt[si] = mutate_thrust(rng, child.rt[si]); }
{ int si = rng.ri(0, H - 1); double v = child.ba[si] + rng.rd() * 24 - 12; child.ba[si] = v < -18 ? -18 : (v > 18 ? 18 : v); }
{ int si = rng.ri(0, H - 1); child.bt[si] = mutate_thrust(rng, child.bt[si]); }
child.sc = evaluate(&child, base_game, H, EX, EY, DTE, RRX, RRY,
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
memcpy(out_ra, pop[best].ra, H * sizeof(double));
memcpy(out_rt, pop[best].rt, H * sizeof(int));
memcpy(out_ba, pop[best].ba, H * sizeof(double));
memcpy(out_bt, pop[best].bt, H * sizeof(int));
*out_rs = pop[best].rs; *out_bs = pop[best].bs;
return iters;
}
inline int run_cem(
RNG& rng,
const csb::Game& base_game, int H, int PS, int mx,
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
double ra_mean[MAX_H], ra_std[MAX_H];
double ba_mean[MAX_H], ba_std[MAX_H];
double rt_prob[MAX_H][CEM_N_THRUST_CATS];
double bt_prob[MAX_H][CEM_N_THRUST_CATS];
if (has_prev) {
for (int t = 0; t < H; t++) {
int src = (t < H - 1) ? t + 1 : H - 1;
ra_mean[t] = prev_ra[src];
ba_mean[t] = prev_ba[src];
ra_std[t] = CEM_INIT_ANGLE_STD * 0.6;
ba_std[t] = CEM_INIT_ANGLE_STD * 0.6;
for (int c = 0; c < CEM_N_THRUST_CATS; c++) {
double diff_r = (double)(CEM_THRUST_CATS[c] - prev_rt[src]);
rt_prob[t][c] = exp(-diff_r * diff_r / 5000.0);
double diff_b = (double)(CEM_THRUST_CATS[c] - prev_bt[src]);
bt_prob[t][c] = exp(-diff_b * diff_b / 5000.0);
}
double sum_r = 0, sum_b = 0;
for (int c = 0; c < CEM_N_THRUST_CATS; c++) { sum_r += rt_prob[t][c]; sum_b += bt_prob[t][c]; }
for (int c = 0; c < CEM_N_THRUST_CATS; c++) { rt_prob[t][c] /= sum_r; bt_prob[t][c] /= sum_b; }
}
} else {
for (int t = 0; t < H; t++) {
ra_mean[t] = 0; ba_mean[t] = 0;
ra_std[t] = CEM_INIT_ANGLE_STD;
ba_std[t] = CEM_INIT_ANGLE_STD;
for (int c = 0; c < CEM_N_THRUST_CATS; c++) {
rt_prob[t][c] = 1.0 / CEM_N_THRUST_CATS;
bt_prob[t][c] = 1.0 / CEM_N_THRUST_CATS;
}
}
}
int n_elite = (int)(PS * CEM_ELITE_FRAC);
if (n_elite < 2) n_elite = 2;
if (n_elite > PS) n_elite = PS;
Ind pop[MAX_POP];
Ind best_ever;
best_ever.sc = -1e18;
auto sample_angle = [&](double mean, double std) -> double {
double u1 = (rng.xr() + 1.0) / 32769.0;
double u2 = rng.rd();
double z = sqrt(-2.0 * log(u1)) * cos(2.0 * PI * u2);
double v = mean + std * z;
if (v < -18.0) v = -18.0;
if (v > 18.0) v = 18.0;
return v;
};
auto sample_thrust = [&](const double* probs) -> int {
double r = rng.rd();
double cum = 0;
for (int c = 0; c < CEM_N_THRUST_CATS - 1; c++) {
cum += probs[c];
if (r < cum) return CEM_THRUST_CATS[c];
}
return CEM_THRUST_CATS[CEM_N_THRUST_CATS - 1];
};
struct timespec t0, tn;
clock_gettime(CLOCK_MONOTONIC, &t0);
int total_iters = 0;
while (1) {
for (int i = 0; i < PS; i++) {
for (int t = 0; t < H; t++) {
pop[i].ra[t] = sample_angle(ra_mean[t], ra_std[t]);
pop[i].rt[t] = sample_thrust(rt_prob[t]);
pop[i].ba[t] = sample_angle(ba_mean[t], ba_std[t]);
pop[i].bt[t] = sample_thrust(bt_prob[t]);
}
pop[i].rs = H;
pop[i].bs = H;
}
if (has_prev && total_iters == 0) {
for (int t = 0; t < H - 1; t++) {
pop[0].ra[t] = prev_ra[t + 1]; pop[0].rt[t] = prev_rt[t + 1];
pop[0].ba[t] = prev_ba[t + 1]; pop[0].bt[t] = prev_bt[t + 1];
}
pop[0].ra[H-1] = ra_mean[H-1]; pop[0].rt[H-1] = sample_thrust(rt_prob[H-1]);
pop[0].ba[H-1] = ba_mean[H-1]; pop[0].bt[H-1] = sample_thrust(bt_prob[H-1]);
pop[0].rs = (prev_rs > 0 && prev_rs < H) ? prev_rs - 1 : H;
pop[0].bs = (prev_bs > 0 && prev_bs < H) ? prev_bs - 1 : H;
}
if (best_ever.sc > -1e17 && total_iters > 0) {
pop[0] = best_ever;
}
for (int i = 0; i < PS; i++) {
pop[i].sc = evaluate(&pop[i], base_game, H, EX, EY, DTE, RRX, RRY,
ncp, laps, mx, rp, bp, orp, obp, rb, fboost, rtimeout,
dw, aw, sw, lw, apw, byw, opw, sfw, faw, rw,
opp_ra, opp_rt, opp_ba, opp_bt);
}
total_iters += PS;
int sorted_idx[MAX_POP];
for (int i = 0; i < PS; i++) sorted_idx[i] = i;
for (int i = 1; i < PS; i++) {
int key = sorted_idx[i];
double key_sc = pop[key].sc;
int j = i - 1;
while (j >= 0 && pop[sorted_idx[j]].sc < key_sc) {
sorted_idx[j + 1] = sorted_idx[j];
j--;
}
sorted_idx[j + 1] = key;
}
if (pop[sorted_idx[0]].sc > best_ever.sc) {
best_ever = pop[sorted_idx[0]];
}
double alpha = CEM_ALPHA;
for (int t = 0; t < H; t++) {
double sum_ra = 0, sum_ba = 0;
for (int e = 0; e < n_elite; e++) {
int idx = sorted_idx[e];
sum_ra += pop[idx].ra[t];
sum_ba += pop[idx].ba[t];
}
double new_ra_mean = sum_ra / n_elite;
double new_ba_mean = sum_ba / n_elite;
double var_ra = 0, var_ba = 0;
for (int e = 0; e < n_elite; e++) {
int idx = sorted_idx[e];
double d = pop[idx].ra[t] - new_ra_mean;
var_ra += d * d;
d = pop[idx].ba[t] - new_ba_mean;
var_ba += d * d;
}
double new_ra_std = sqrt(var_ra / n_elite);
double new_ba_std = sqrt(var_ba / n_elite);
if (new_ra_std < CEM_MIN_ANGLE_STD) new_ra_std = CEM_MIN_ANGLE_STD;
if (new_ba_std < CEM_MIN_ANGLE_STD) new_ba_std = CEM_MIN_ANGLE_STD;
ra_mean[t] = alpha * new_ra_mean + (1 - alpha) * ra_mean[t];
ra_std[t] = alpha * new_ra_std + (1 - alpha) * ra_std[t];
ba_mean[t] = alpha * new_ba_mean + (1 - alpha) * ba_mean[t];
ba_std[t] = alpha * new_ba_std + (1 - alpha) * ba_std[t];
double new_rt_prob[CEM_N_THRUST_CATS] = {};
double new_bt_prob[CEM_N_THRUST_CATS] = {};
for (int e = 0; e < n_elite; e++) {
int idx = sorted_idx[e];
int best_c = 0;
int best_diff = abs(pop[idx].rt[t] - CEM_THRUST_CATS[0]);
for (int c = 1; c < CEM_N_THRUST_CATS; c++) {
int diff = abs(pop[idx].rt[t] - CEM_THRUST_CATS[c]);
if (diff < best_diff) { best_diff = diff; best_c = c; }
}
new_rt_prob[best_c] += 1.0;
best_c = 0; best_diff = abs(pop[idx].bt[t] - CEM_THRUST_CATS[0]);
for (int c = 1; c < CEM_N_THRUST_CATS; c++) {
int diff = abs(pop[idx].bt[t] - CEM_THRUST_CATS[c]);
if (diff < best_diff) { best_diff = diff; best_c = c; }
}
new_bt_prob[best_c] += 1.0;
}
for (int c = 0; c < CEM_N_THRUST_CATS; c++) {
new_rt_prob[c] /= n_elite;
new_bt_prob[c] /= n_elite;
rt_prob[t][c] = alpha * new_rt_prob[c] + (1 - alpha) * rt_prob[t][c];
bt_prob[t][c] = alpha * new_bt_prob[c] + (1 - alpha) * bt_prob[t][c];
}
double sum_r = 0, sum_b = 0;
for (int c = 0; c < CEM_N_THRUST_CATS; c++) { sum_r += rt_prob[t][c]; sum_b += bt_prob[t][c]; }
if (sum_r > 0) for (int c = 0; c < CEM_N_THRUST_CATS; c++) rt_prob[t][c] /= sum_r;
if (sum_b > 0) for (int c = 0; c < CEM_N_THRUST_CATS; c++) bt_prob[t][c] /= sum_b;
}
clock_gettime(CLOCK_MONOTONIC, &tn);
long el = (tn.tv_sec - t0.tv_sec) * 1000000L + (tn.tv_nsec - t0.tv_nsec) / 1000L;
if (el >= budget_us) break;
}
memcpy(out_ra, best_ever.ra, H * sizeof(double));
memcpy(out_rt, best_ever.rt, H * sizeof(int));
memcpy(out_ba, best_ever.ba, H * sizeof(double));
memcpy(out_bt, best_ever.bt, H * sizeof(int));
*out_rs = best_ever.rs; *out_bs = best_ever.bs;
return total_iters;
}
inline int run_sa(
RNG& rng,
const csb::Game& base_game, int H, int PS, int mx,
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
(void)PS;
int K = SA_NUM_STARTS;
int per_start_us = budget_us / K;
if (per_start_us < 5000) { K = 1; per_start_us = budget_us; }
Ind global_best;
global_best.sc = -1e18;
int total_iters = 0;
for (int start = 0; start < K; start++) {
Ind current;
if (start == 0 && has_prev) {
for (int t = 0; t < H - 1; t++) {
current.ra[t] = prev_ra[t + 1]; current.rt[t] = prev_rt[t + 1];
current.ba[t] = prev_ba[t + 1]; current.bt[t] = prev_bt[t + 1];
}
current.ra[H - 1] = rng.rd() * 36 - 18; current.rt[H - 1] = rand_thrust(rng);
current.ba[H - 1] = rng.rd() * 36 - 18; current.bt[H - 1] = rand_thrust(rng);
current.rs = (prev_rs > 0 && prev_rs < H) ? prev_rs - 1 : H;
current.bs = (prev_bs > 0 && prev_bs < H) ? prev_bs - 1 : H;
} else {
for (int t = 0; t < H; t++) {
double v = rng.ri(-400, 400) / 10.0;
current.ra[t] = v < -18 ? -18 : (v > 18 ? 18 : v);
current.rt[t] = rand_thrust(rng);
v = rng.ri(-400, 400) / 10.0;
current.ba[t] = v < -18 ? -18 : (v > 18 ? 18 : v);
current.bt[t] = rand_thrust(rng);
}
current.rs = rng.ri(0, H + 4);
current.bs = rng.ri(0, H + 4);
}
current.sc = evaluate(&current, base_game, H, EX, EY, DTE, RRX, RRY,
ncp, laps, mx, rp, bp, orp, obp, rb, fboost, rtimeout,
dw, aw, sw, lw, apw, byw, opw, sfw, faw, rw,
opp_ra, opp_rt, opp_ba, opp_bt);
Ind best_in_start = current;
double temperature = SA_INIT_TEMP;
struct timespec t0, tn;
clock_gettime(CLOCK_MONOTONIC, &t0);
int iters = 0;
while (1) {
iters++;
if ((iters & 511) == 0) {
clock_gettime(CLOCK_MONOTONIC, &tn);
long el = (tn.tv_sec - t0.tv_sec) * 1000000L + (tn.tv_nsec - t0.tv_nsec) / 1000L;
if (el >= per_start_us) break;
}
Ind neighbor;
memcpy(&neighbor, &current, sizeof(Ind));
int n_mutations = SA_MUTATIONS_MIN + rng.ri(0, SA_MUTATIONS_MAX - SA_MUTATIONS_MIN);
for (int m = 0; m < n_mutations; m++) {
int t = rng.ri(0, H - 1);
double angle_stddev = 1.0 + 9.0 * (temperature / SA_INIT_TEMP);
double u1 = rng.rd();
double u2 = rng.rd();
if (u1 < 0.001) u1 = 0.001;
double gauss = sqrt(-2.0 * log(u1)) * cos(2.0 * PI * u2);
if (rng.rd() < 0.5) {
double v = neighbor.ra[t] + gauss * angle_stddev;
neighbor.ra[t] = v < -18 ? -18 : (v > 18 ? 18 : v);
} else {
double v = neighbor.ba[t] + gauss * angle_stddev;
neighbor.ba[t] = v < -18 ? -18 : (v > 18 ? 18 : v);
}
if (rng.rd() < SA_THRUST_RANDOMIZE_PROB) {
if (rng.rd() < 0.5) {
if (rng.rd() < 0.4) {
static constexpr int TC[] = {0, 50, 100, 150, 200};
neighbor.rt[t] = TC[rng.ri(0, 4)];
} else {
neighbor.rt[t] = mutate_thrust(rng, neighbor.rt[t]);
}
} else {
if (rng.rd() < 0.4) {
static constexpr int TC[] = {0, 50, 100, 150, 200};
neighbor.bt[t] = TC[rng.ri(0, 4)];
} else {
neighbor.bt[t] = mutate_thrust(rng, neighbor.bt[t]);
}
}
}
}
if (rng.rd() < SA_SHIELD_TOGGLE_PROB) {
if (rng.rd() < 0.5) {
neighbor.rs = (neighbor.rs >= H) ? rng.ri(0, 2) : H;
} else {
neighbor.bs = (neighbor.bs >= H) ? rng.ri(0, 2) : H;
}
}
neighbor.sc = evaluate(&neighbor, base_game, H, EX, EY, DTE, RRX, RRY,
ncp, laps, mx, rp, bp, orp, obp, rb, fboost, rtimeout,
dw, aw, sw, lw, apw, byw, opw, sfw, faw, rw,
opp_ra, opp_rt, opp_ba, opp_bt);
double delta = neighbor.sc - current.sc;
if (delta >= 0) {
current = neighbor;
} else {
double accept_prob = exp(delta / temperature);
if (rng.rd() < accept_prob) {
current = neighbor;
}
}
if (current.sc > best_in_start.sc) {
best_in_start = current;
}
temperature *= SA_COOLING_RATE;
if (temperature < SA_MIN_TEMP) temperature = SA_MIN_TEMP;
}
total_iters += iters;
if (best_in_start.sc > global_best.sc) {
global_best = best_in_start;
}
}
memcpy(out_ra, global_best.ra, H * sizeof(double));
memcpy(out_rt, global_best.rt, H * sizeof(int));
memcpy(out_ba, global_best.ba, H * sizeof(double));
memcpy(out_bt, global_best.bt, H * sizeof(int));
*out_rs = global_best.rs; *out_bs = global_best.bs;
return total_iters;
}
struct BeamState {
csb::Game game;
double ra[MAX_H], ba[MAX_H];
int rt[MAX_H], bt[MAX_H];
int rs, bs;
double sc;
int depth;
};
inline int beam_diversity_hash(const BeamState& st, int rp, int bp) {
int rx = (int)((st.game.pods[rp].pos.x + 2000) / BEAM_GRID_CELL);
int ry = (int)((st.game.pods[rp].pos.y + 2000) / BEAM_GRID_CELL);
int bx = (int)((st.game.pods[bp].pos.x + 2000) / BEAM_GRID_CELL);
int by = (int)((st.game.pods[bp].pos.y + 2000) / BEAM_GRID_CELL);
rx = (rx < 0) ? 0 : ((rx >= BEAM_GRID_W) ? BEAM_GRID_W - 1 : rx);
ry = (ry < 0) ? 0 : ((ry >= BEAM_GRID_H) ? BEAM_GRID_H - 1 : ry);
bx = (bx < 0) ? 0 : ((bx >= BEAM_GRID_W) ? BEAM_GRID_W - 1 : bx);
by = (by < 0) ? 0 : ((by >= BEAM_GRID_H) ? BEAM_GRID_H - 1 : by);
return ((rx * BEAM_GRID_H + ry) * BEAM_GRID_W + bx) * BEAM_GRID_H + by;
}
inline int run_beam_search(
RNG& rng,
const csb::Game& base_game, int H, int PS, int mx,
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
static BeamState beam_cur[BEAM_WIDTH];
static BeamState candidates[BEAM_WIDTH * 2];
(void)rng; (void)PS; (void)rb; (void)has_prev;
(void)prev_rs; (void)prev_bs;
(void)dw; (void)aw; (void)sw; (void)lw; (void)apw;
(void)byw; (void)opw; (void)sfw; (void)faw; (void)rw;
(void)RRX; (void)RRY; (void)mx; (void)laps; (void)fboost; (void)rtimeout;
int n_beam = 0;
int total_expansions = 0;
struct timespec t0, tn;
clock_gettime(CLOCK_MONOTONIC, &t0);
beam_cur[0].game = base_game;
beam_cur[0].depth = 0;
beam_cur[0].rs = H; beam_cur[0].bs = H;
beam_cur[0].sc = -1e18;
for (int t = 0; t < H; t++) {
beam_cur[0].ra[t] = 0; beam_cur[0].rt[t] = 200;
beam_cur[0].ba[t] = 0; beam_cur[0].bt[t] = 200;
}
n_beam = 1;
if (prev_ra && n_beam < BEAM_WIDTH) {
beam_cur[1] = beam_cur[0];
for (int t = 0; t < H - 1; t++) {
beam_cur[1].ra[t] = prev_ra[t + 1]; beam_cur[1].rt[t] = prev_rt[t + 1];
beam_cur[1].ba[t] = prev_ba[t + 1]; beam_cur[1].bt[t] = prev_bt[t + 1];
}
beam_cur[1].ra[H - 1] = 0; beam_cur[1].rt[H - 1] = 200;
beam_cur[1].ba[H - 1] = 0; beam_cur[1].bt[H - 1] = 200;
n_beam = 2;
}
BeamState best_complete;
best_complete.sc = -1e18;
bool has_complete = false;
for (int depth = 0; depth < H; depth++) {
clock_gettime(CLOCK_MONOTONIC, &tn);
long el = (tn.tv_sec - t0.tv_sec) * 1000000L + (tn.tv_nsec - t0.tv_nsec) / 1000L;
if (el >= budget_us) break;
static constexpr int HASH_SLOTS = 4096;
static int hash_keys[HASH_SLOTS];
static double hash_scores[HASH_SLOTS];
static int hash_indices[HASH_SLOTS];
for (int i = 0; i < HASH_SLOTS; i++) hash_keys[i] = -1;
int n_candidates = 0;
for (int bi = 0; bi < n_beam; bi++) {
const BeamState& parent = beam_cur[bi];
for (int ra_i = 0; ra_i < BEAM_N_ANGLES; ra_i++) {
for (int rt_i = 0; rt_i < BEAM_N_THRUSTS; rt_i++) {
for (int ba_i = 0; ba_i < BEAM_N_ANGLES; ba_i++) {
for (int bt_i = 0; bt_i < BEAM_N_THRUSTS; bt_i++) {
total_expansions++;
double r_angle = BEAM_ANGLES[ra_i];
int r_thrust = BEAM_THRUSTS[rt_i];
double b_angle = BEAM_ANGLES[ba_i];
int b_thrust = BEAM_THRUSTS[bt_i];
BeamState child;
child.game = parent.game;
child.depth = depth + 1;
child.rs = parent.rs; child.bs = parent.bs;
for (int t = 0; t < depth; t++) {
child.ra[t] = parent.ra[t]; child.rt[t] = parent.rt[t];
child.ba[t] = parent.ba[t]; child.bt[t] = parent.bt[t];
}
child.ra[depth] = r_angle;
child.rt[depth] = r_thrust;
child.ba[depth] = b_angle;
child.bt[depth] = b_thrust;
for (int t = depth + 1; t < H; t++) {
child.ra[t] = 0.0; child.rt[t] = 200;
child.ba[t] = 0.0; child.bt[t] = 200;
}
child.game.pods[rp].applyActionDelta(r_angle, r_thrust, 0, 0);
child.game.pods[bp].applyActionDelta(b_angle, b_thrust, 0, 0);
if (opp_ra) {
child.game.pods[orp].applyActionDelta(opp_ra[depth], opp_rt[depth], 0, 0);
child.game.pods[obp].applyActionDelta(opp_ba[depth], opp_bt[depth], 0, 0);
} else {
child.game.applyAction(orp,
(int)EX[child.game.pods[orp].next % ncp],
(int)EY[child.game.pods[orp].next % ncp], "200");
child.game.applyAction(obp,
(int)EX[child.game.pods[obp].next % ncp],
(int)EY[child.game.pods[obp].next % ncp], "200");
}
child.game.nextTurn();
Ind eval_ind;
for (int t = 0; t < H; t++) {
eval_ind.ra[t] = child.ra[t]; eval_ind.rt[t] = child.rt[t];
eval_ind.ba[t] = child.ba[t]; eval_ind.bt[t] = child.bt[t];
}
eval_ind.rs = child.rs; eval_ind.bs = child.bs;
child.sc = evaluate(&eval_ind, base_game, H, EX, EY, DTE, RRX, RRY,
ncp, laps, mx, rp, bp, orp, obp, rb, fboost, rtimeout,
dw, aw, sw, lw, apw, byw, opw, sfw, faw, rw,
opp_ra, opp_rt, opp_ba, opp_bt);
int h = beam_diversity_hash(child, rp, bp);
int slot = h & (HASH_SLOTS - 1);
int probes = 0;
while (probes < 8) {
int idx = (slot + probes) & (HASH_SLOTS - 1);
if (hash_keys[idx] == -1) {
hash_keys[idx] = h;
hash_scores[idx] = child.sc;
if (n_candidates < BEAM_WIDTH * 2) {
hash_indices[idx] = n_candidates;
candidates[n_candidates++] = child;
}
break;
} else if (hash_keys[idx] == h) {
if (child.sc > hash_scores[idx]) {
hash_scores[idx] = child.sc;
int ci = hash_indices[idx];
if (ci < BEAM_WIDTH * 2) candidates[ci] = child;
}
break;
}
probes++;
}
if (probes >= 8 && n_candidates < BEAM_WIDTH * 2) {
candidates[n_candidates++] = child;
}
}
}
}
}
clock_gettime(CLOCK_MONOTONIC, &tn);
long el2 = (tn.tv_sec - t0.tv_sec) * 1000000L + (tn.tv_nsec - t0.tv_nsec) / 1000L;
if (el2 >= budget_us) break;
}
int n_keep = n_candidates < BEAM_WIDTH ? n_candidates : BEAM_WIDTH;
static int sel_idx[BEAM_WIDTH * 2];
for (int i = 0; i < n_candidates; i++) sel_idx[i] = i;
for (int k = 0; k < n_keep; k++) {
int best_j = k;
double best_sc = candidates[sel_idx[k]].sc;
for (int j = k + 1; j < n_candidates; j++) {
if (candidates[sel_idx[j]].sc > best_sc) {
best_sc = candidates[sel_idx[j]].sc;
best_j = j;
}
}
if (best_j != k) { int tmp = sel_idx[k]; sel_idx[k] = sel_idx[best_j]; sel_idx[best_j] = tmp; }
beam_cur[k] = candidates[sel_idx[k]];
if (depth == H - 1) {
if (!has_complete || beam_cur[k].sc > best_complete.sc) {
best_complete = beam_cur[k]; has_complete = true;
}
}
}
n_beam = n_keep;
if (n_beam == 0) break;
}
int best_idx = 0;
for (int i = 1; i < n_beam; i++) {
if (beam_cur[i].sc > beam_cur[best_idx].sc) best_idx = i;
}
const BeamState& result = (has_complete && best_complete.sc > beam_cur[best_idx].sc)
? best_complete : beam_cur[best_idx];
for (int t = 0; t < H; t++) {
out_ra[t] = result.ra[t]; out_rt[t] = result.rt[t];
out_ba[t] = result.ba[t]; out_bt[t] = result.bt[t];
}
*out_rs = result.rs; *out_bs = result.bs;
return total_expansions;
}
}
class UltimateBot : public Bot {
public:
void init(int laps,
const std::vector<std::pair<double, double>>& checkpoints) override {
laps_ = laps + 1;
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
for (int i = 0; i < ncp_; i++) {
int n = (i + 1) % ncp_;
heading_to_next_[i] = atan2(CY_[n] - CY_[i], CX_[n] - CX_[i]);
}
turn_ = 0;
rng_.seed(42);
for (int i = 0; i < 2; i++) {
ml_[i] = 0; ol_[i] = 0;
pmc_[i] = -1; poc_[i] = -1;
msc_[i] = 0; mbu_[i] = false;
}
has_prev_ = false;
has_opp_prev_ = false;
prev_rs_ = H_; prev_bs_ = H_;
nn_angle_bias_[0] = nn_angle_bias_[1] = 0;
nn_thrust_bias_[0] = nn_thrust_bias_[1] = 0;
nn_last_pred_angle_[0] = nn_last_pred_angle_[1] = 0;
nn_last_pred_thrust_[0] = nn_last_pred_thrust_[1] = 0;
nn_obs_count_ = 0;
runner_zero_streak_ = 0;
}
std::pair<BotAction, BotAction> get_actions(
const std::array<std::array<double, 6>, 4>& pods) override {
turn_++;
using namespace ultimate_ga;
double px[4], py[4], pvx[4], pvy[4], pang[4];
int pnc[4], plp[4], psd[4];
for (int i = 0; i < 2; i++) {
px[i] = pods[i][0]; py[i] = pods[i][1];
pvx[i] = pods[i][2]; pvy[i] = pods[i][3];
pang[i] = pods[i][4]; pnc[i] = static_cast<int>(pods[i][5]);
if (pang[i] < 0) {
pang[i] = atan2(CY_[pnc[i]] - py[i], CX_[pnc[i]] - px[i]) * RAD_TO_DEG;
if (pang[i] < 0) pang[i] += 360;
}
if (pmc_[i] == -1) pmc_[i] = pnc[i];
else if (pnc[i] == 0 && pmc_[i] == ncp_ - 1) { ml_[i]++; runner_stuck_turns_[i] = 0; }
else if (pnc[i] != pmc_[i]) { runner_stuck_turns_[i] = 0; }
else { runner_stuck_turns_[i]++; }
pmc_[i] = pnc[i]; plp[i] = ml_[i]; psd[i] = msc_[i];
if (msc_[i] > 0) msc_[i]--;
}
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
if (has_opp_obs_) {
for (int i = 0; i < 2; i++) {
double ang_now = pang[i + 2] * DEG_TO_RAD;
double ang_prev = prev_opp_ang_[i] * DEG_TO_RAD;
double delta = ang_now - ang_prev;
while (delta > PI) delta -= 2 * PI;
while (delta < -PI) delta += 2 * PI;
obs_opp_delta_[i] = delta * RAD_TO_DEG;
double ca = cos(ang_now), sa = sin(ang_now);
double dvx = pvx[i + 2] - prev_opp_vx_[i] * 0.85;
double dvy = pvy[i + 2] - prev_opp_vy_[i] * 0.85;
double thrust_est = 0;
if (ca * ca + sa * sa > 0.1) {
thrust_est = (dvx * ca + dvy * sa);
}
obs_opp_thrust_[i] = std::max(0, std::min(200, (int)(thrust_est + 0.5)));
}
}
for (int i = 0; i < 2; i++) {
prev_opp_ang_[i] = pang[i + 2];
prev_opp_x_[i] = px[i + 2]; prev_opp_y_[i] = py[i + 2];
prev_opp_vx_[i] = pvx[i + 2]; prev_opp_vy_[i] = pvy[i + 2];
}
has_opp_obs_ = true;
if (has_opp_obs_ && nn_obs_count_ > 0) {
float alpha = std::min(0.5f, 2.0f / (nn_obs_count_ + 1));
for (int i = 0; i < 2; i++) {
float angle_err = nn_last_pred_angle_[i] - (float)obs_opp_delta_[i];
float thrust_err = nn_last_pred_thrust_[i] - (float)obs_opp_thrust_[i];
nn_angle_bias_[i] = (1 - alpha) * nn_angle_bias_[i] + alpha * angle_err;
}
}
if (has_opp_obs_) nn_obs_count_++;
int budget_us = (turn_ == 1) ? ultimate_ga::FIRST_TURN_BUDGET_US : ultimate_ga::TURN_BUDGET_US;
struct timespec turn_start_; clock_gettime(CLOCK_MONOTONIC, &turn_start_);
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
{
double d2_to_our_cp = hypot(px[2] - CX_[pnc[rp]], py[2] - CY_[pnc[rp]]);
double d3_to_our_cp = hypot(px[3] - CX_[pnc[rp]], py[3] - CY_[pnc[rp]]);
double camp_r = 2000.0;
bool p2_camp = d2_to_our_cp < camp_r;
bool p3_camp = d3_to_our_cp < camp_r;
if (p2_camp && !p3_camp && orp_abs != 2) { orp_abs = 2; obp_abs = 3; }
else if (p3_camp && !p2_camp && orp_abs != 3) { orp_abs = 3; obp_abs = 2; }
}
int rb = pnc[orp_abs];
{
double od = hypot(px[orp_abs] - CX_[rb], py[orp_abs] - CY_[rb]);
double md = hypot(px[bp] - CX_[rb], py[bp] - CY_[rb]);
if (md > od + ultimate_ga::BLOCKER_DIST_THRESHOLD) {
int nb = (rb + 1) % ncp_;
double od2 = od + hypot(CX_[rb] - CX_[nb], CY_[rb] - CY_[nb]);
double md2 = hypot(px[bp] - CX_[nb], py[bp] - CY_[nb]);
if (md2 < od2 - ultimate_ga::BLOCKER_DIST_THRESHOLD) rb = nb;
else rb = (nb + 1) % ncp_;
}
}
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
if (runner_stuck_turns_[rp] >= 30) rtimeout = 1;
if (plp[rp] == laps_ - 1 && runner_stuck_turns_[rp] >= 20) rtimeout = 2;
csb::Game base_game;
base_game.globalCpSize = 0;
for (int lap = 0; lap < laps_; lap++) {
for (int c = 0; c < ncp_; c++) {
if (base_game.globalCpSize < 255) {
base_game.globalCp[base_game.globalCpSize++] = {CX_[c], CY_[c]};
}
}
}
if (base_game.globalCpSize < 256) {
base_game.globalCp[base_game.globalCpSize++] = {CX_[0], CY_[0]};
}
for (int i = 0; i < 4; i++) {
int flattened_next = plp[i] * ncp_ + pnc[i];
base_game.setPodState(i, px[i], py[i], pvx[i], pvy[i],
pang[i] * DEG_TO_RAD, flattened_next,
psd[i], (i < 2 && mbu_[i]) ? 1 : 0);
base_game.pods[i].isFirstTurn = (turn_ == 1);
}
double out_ra[MAX_H], out_ba[MAX_H];
int out_rt[MAX_H], out_bt[MAX_H];
int out_rs, out_bs;
double pred_ora[MAX_H]={}, pred_oba[MAX_H]={};
int pred_ort[MAX_H]={}, pred_obt[MAX_H]={};
bool use_ibr = false; /* OPT: skip opp prediction GA; all budget to phase2 */
int phase2_budget = budget_us;
if (turn_ > 1) {
int pod_order[4] = {rp, bp, orp_abs, obp_abs};
float nn_input[38];
int fi = 0;
for (int pi = 0; pi < 4; pi++) {
int p = pod_order[pi];
int nc = pnc[p];
double dx_cp = CX_[nc] - px[p];
double dy_cp = CY_[nc] - py[p];
double dist_cp = hypot(dx_cp, dy_cp);
double cp_angle = atan2(dy_cp, dx_cp);
double pod_angle_rad = pang[p] * DEG_TO_RAD;
double angle_to_cp = cp_angle - pod_angle_rad;
while (angle_to_cp > PI) angle_to_cp -= 2*PI;
while (angle_to_cp < -PI) angle_to_cp += 2*PI;
double speed = hypot(pvx[p], pvy[p]);
nn_input[fi++] = (float)dx_cp;
nn_input[fi++] = (float)dy_cp;
nn_input[fi++] = (float)pvx[p];
nn_input[fi++] = (float)pvy[p];
nn_input[fi++] = (float)speed;
nn_input[fi++] = (float)dist_cp;
nn_input[fi++] = (float)angle_to_cp;
}
if (has_opp_obs_) {
nn_input[fi++] = (float)obs_opp_delta_[orp_abs - 2];
nn_input[fi++] = (float)obs_opp_thrust_[orp_abs - 2];
nn_input[fi++] = (float)obs_opp_delta_[obp_abs - 2];
nn_input[fi++] = (float)obs_opp_thrust_[obp_abs - 2];
} else {
nn_input[fi++] = 0.0f; nn_input[fi++] = 100.0f;
nn_input[fi++] = 0.0f; nn_input[fi++] = 100.0f;
}
for (int oi = 0; oi < 2; oi++) {
int op = (oi == 0) ? orp_abs : obp_abs;
int nc = pnc[op];
int prev_cp = (nc - 1 + ncp_) % ncp_;
int next_cp = (nc + 1) % ncp_;
double v1x = CX_[nc] - CX_[prev_cp], v1y = CY_[nc] - CY_[prev_cp];
double v2x = CX_[next_cp] - CX_[nc], v2y = CY_[next_cp] - CY_[nc];
double l1 = hypot(v1x, v1y), l2 = hypot(v2x, v2y);
float cos_theta = 1.0f;
if (l1 > 0 && l2 > 0)
cos_theta = (float)std::max(-1.0, std::min(1.0, (v1x*v2x + v1y*v2y)/(l1*l2)));
nn_input[fi++] = cos_theta;
}
for (int oi = 0; oi < 2; oi++) {
int op = (oi == 0) ? orp_abs : obp_abs;
double d0 = hypot(px[op]-px[0], py[op]-py[0]);
double d1 = hypot(px[op]-px[1], py[op]-py[1]);
nn_input[fi++] = (float)std::min(d0, d1);
}
int our_prog = std::max(plp[rp]*ncp_+pnc[rp], plp[bp]*ncp_+pnc[bp]);
int opp_prog = std::max(plp[orp_abs]*ncp_+pnc[orp_abs], plp[obp_abs]*ncp_+pnc[obp_abs]);
nn_input[fi++] = (float)(our_prog - opp_prog);
nn_input[fi++] = (float)turn_;
float nn_output[8];
opp_nn::predict(nn_input, nn_output);
int nn_thrust_a = opp_nn::logits_to_thrust(nn_output[1], nn_output[2], nn_output[3]);
int nn_thrust_b = opp_nn::logits_to_thrust(nn_output[5], nn_output[6], nn_output[7]);
nn_last_pred_angle_[orp_abs - 2] = nn_output[0] * (float)RAD_TO_DEG;
nn_last_pred_thrust_[orp_abs - 2] = (float)nn_thrust_a;
nn_last_pred_angle_[obp_abs - 2] = nn_output[4] * (float)RAD_TO_DEG;
nn_last_pred_thrust_[obp_abs - 2] = (float)nn_thrust_b;
float corr_ora_rad = nn_output[0] - nn_angle_bias_[orp_abs - 2] * (float)DEG_TO_RAD;
float corr_oba_rad = nn_output[4] - nn_angle_bias_[obp_abs - 2] * (float)DEG_TO_RAD;
float corr_ort = (float)nn_thrust_a;
float corr_obt = (float)nn_thrust_b;
float max_turn_rad = (float)MAX_TURN_RAD;
corr_ora_rad = std::max(-max_turn_rad, std::min(max_turn_rad, corr_ora_rad));
corr_ort = std::max(0.0f, std::min(200.0f, corr_ort));
corr_oba_rad = std::max(-max_turn_rad, std::min(max_turn_rad, corr_oba_rad));
corr_obt = std::max(0.0f, std::min(200.0f, corr_obt));
double nn_ora = std::max(-18.0, std::min(18.0, (double)corr_ora_rad * RAD_TO_DEG));
int nn_ort = std::max(0, std::min(200, (int)corr_ort));
double nn_oba = std::max(-18.0, std::min(18.0, (double)corr_oba_rad * RAD_TO_DEG));
int nn_obt = std::max(0, std::min(200, (int)corr_obt));
for (int t = 0; t < H_; t++) {
pred_ora[t] = nn_ora * DEG_TO_RAD;
pred_ort[t] = nn_ort;
pred_oba[t] = nn_oba * DEG_TO_RAD;
pred_obt[t] = nn_obt;
}
}
if (use_ibr) {
int p1_budget = ultimate_ga::IBR_BUDGET_US;
double shifted_ra[ultimate_ga::MAX_H], shifted_ba[ultimate_ga::MAX_H];
int shifted_rt[ultimate_ga::MAX_H], shifted_bt[ultimate_ga::MAX_H];
for (int t = 0; t < H_ - 1; t++) {
shifted_ra[t] = prev_ra_[t+1]; shifted_rt[t] = prev_rt_[t+1];
shifted_ba[t] = prev_ba_[t+1]; shifted_bt[t] = prev_bt_[t+1];
}
shifted_ra[H_-1] = 0; shifted_rt[H_-1] = 200;
shifted_ba[H_-1] = 0; shifted_bt[H_-1] = 200;
run_opp_prediction_ga(
rng_, base_game, H_, mx_,
EX_.data(), EY_.data(),
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
memcpy(opp_prev_ra_, pred_ora, H_ * sizeof(double));
memcpy(opp_prev_rt_, pred_ort, H_ * sizeof(int));
memcpy(opp_prev_ba_, pred_oba, H_ * sizeof(double));
memcpy(opp_prev_bt_, pred_obt, H_ * sizeof(int));
has_opp_prev_ = true;
phase2_budget = budget_us - p1_budget;
if (phase2_budget < ultimate_ga::MIN_PHASE2_BUDGET_US) phase2_budget = ultimate_ga::MIN_PHASE2_BUDGET_US;
}
if (ultimate_ga::SEARCH_MODE == 1) {
ga_iters_ = run_cem(
rng_, base_game, H_, PS_, mx_,
EX_.data(), EY_.data(),
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
} else if (ultimate_ga::SEARCH_MODE == 2) {
ga_iters_ = run_beam_search(
rng_, base_game, H_, PS_, mx_,
EX_.data(), EY_.data(),
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
} else if (ultimate_ga::SEARCH_MODE == 3) {
ga_iters_ = run_sa(
rng_, base_game, H_, PS_, mx_,
EX_.data(), EY_.data(),
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
} else {
ga_iters_ = run_combined_ga(
rng_, base_game, H_, PS_, mx_,
EX_.data(), EY_.data(),
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
}
struct timespec turn_end_; clock_gettime(CLOCK_MONOTONIC, &turn_end_);
long turn_us = (turn_end_.tv_sec - turn_start_.tv_sec) * 1000000L +
(turn_end_.tv_nsec - turn_start_.tv_nsec) / 1000L;
double turn_ms = turn_us / 1000.0;
int ga_sims = ga_iters_;
memcpy(prev_ra_, out_ra, H_ * sizeof(double));
memcpy(prev_rt_, out_rt, H_ * sizeof(int));
memcpy(prev_ba_, out_ba, H_ * sizeof(double));
memcpy(prev_bt_, out_bt, H_ * sizeof(int));
prev_rs_ = out_rs; prev_bs_ = out_bs;
has_prev_ = true;
auto is_way_off = [](double x, double y) {
return x < -2000 || x > 18000 || y < -2000 || y > 11000;
};
for (int pod : {rp, bp}) {
if (is_way_off(px[pod], py[pod])) {
double tx = CX_[pnc[pod]], ty = CY_[pnc[pod]];
BotAction recovery;
recovery.target_x = lround(tx);
recovery.target_y = lround(ty);
recovery.thrust = 200;
int other = (pod == rp) ? bp : rp;
double* oa = (other == rp) ? out_ra : out_ba;
int* ot = (other == rp) ? out_rt : out_bt;
double da = std::max(-18.0, std::min(18.0, oa[0]));
double ad = fmod(pang[other] + da, 360.0); if (ad < 0) ad += 360;
double rd = ad * DEG_TO_RAD;
BotAction normal;
normal.target_x = lround(px[other] + cos(rd) * 10000);
normal.target_y = lround(py[other] + sin(rd) * 10000);
normal.thrust = std::max(0, std::min(200, ot[0]));
if (pod == rp) return {recovery, normal};
else return {normal, recovery};
}
}
double r_da = std::max(-18.0, std::min(18.0, out_ra[0]));
double r_ang_deg = fmod(pang[rp] + r_da, 360.0);
if (r_ang_deg < 0) r_ang_deg += 360;
double r_rad = r_ang_deg * DEG_TO_RAD;
double rtx = px[rp] + cos(r_rad) * 10000;
double rty = py[rp] + sin(r_rad) * 10000;
bool r_shield = out_rs == 0 && psd[rp] == 0;
bool r_boost = fboost && !mbu_[rp] && !r_shield;
BotAction runner_act;
runner_act.target_x = lround(rtx);
runner_act.target_y = lround(rty);
if (r_shield) {
runner_act.thrust = THRUST_SHIELD; msc_[rp] = 3;
} else if (r_boost) {
runner_act.thrust = THRUST_BOOST; mbu_[rp] = true;
} else {
runner_act.thrust = std::max(0, std::min(200, out_rt[0]));
}
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
blocker_act.target_x = lround(btx);
blocker_act.target_y = lround(bty);
if (b_shield) {
blocker_act.thrust = THRUST_SHIELD; msc_[bp] = 3;
} else if (b_boost_flag) {
blocker_act.thrust = THRUST_BOOST; mbu_[bp] = true;
} else {
blocker_act.thrust = std::max(0, std::min(200, out_bt[0]));
}
const char* mode_name = (ultimate_ga::SEARCH_MODE == 3) ? "SA" :
(ultimate_ga::SEARCH_MODE == 2) ? "Beam" :
(ultimate_ga::SEARCH_MODE == 1) ? "CEM" : "GA";
fprintf(stderr, "T%d %.1fms %s:%d sims:%d\n", turn_, turn_ms, mode_name,
ga_sims, ga_sims + ultimate_ga::GA_POP_SIZE);
if (rp == 0) return {runner_act, blocker_act};
else return {blocker_act, runner_act};
}
std::unique_ptr<Bot> clone() const override {
return std::make_unique<UltimateBot>();
}
private:
static constexpr int H_ = ultimate_ga::GA_HORIZON;
static constexpr int PS_ = ultimate_ga::GA_POP_SIZE;
int laps_ = 0;
int actual_laps_ = 0;
int ncp_ = 0;
int mx_ = 0;
int turn_ = 0;
int ga_iters_ = 0;
ultimate_ga::RNG rng_;
std::vector<double> CX_, CY_, EX_, EY_, DTE_, RRX_, RRY_;
double heading_to_next_[20] = {};
int ml_[2] = {}, ol_[2] = {};
int pmc_[2] = {}, poc_[2] = {};
int msc_[2] = {};
bool mbu_[2] = {};
int runner_stuck_turns_[2] = {};
bool has_prev_ = false;
double prev_ra_[ultimate_ga::MAX_H] = {};
int prev_rt_[ultimate_ga::MAX_H] = {};
double prev_ba_[ultimate_ga::MAX_H] = {};
int prev_bt_[ultimate_ga::MAX_H] = {};
int prev_rs_ = 6, prev_bs_ = 6;
bool has_opp_prev_ = false;
double opp_prev_ra_[ultimate_ga::MAX_H] = {};
int opp_prev_rt_[ultimate_ga::MAX_H] = {};
double opp_prev_ba_[ultimate_ga::MAX_H] = {};
int opp_prev_bt_[ultimate_ga::MAX_H] = {};
bool has_opp_obs_ = false;
int runner_zero_streak_ = 0;
bool runner_finished_[2] = {false, false};
int max_progress_[2] = {0, 0};
double prev_opp_ang_[2] = {};
double prev_opp_x_[2] = {}, prev_opp_y_[2] = {};
double prev_opp_vx_[2] = {}, prev_opp_vy_[2] = {};
double obs_opp_delta_[2] = {};
int obs_opp_thrust_[2] = {};
float nn_angle_bias_[2] = {0, 0};
float nn_thrust_bias_[2] = {0, 0};
float nn_last_pred_angle_[2] = {0, 0};
float nn_last_pred_thrust_[2] = {0, 0};
int nn_obs_count_ = 0;
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
bool use_handling = (ncp_ >= ultimate_ga::HANDLING_MIN_CPS || has_sharp_) && avg_dist <= ultimate_ga::HANDLING_AVG_DIST;
if (use_handling) {
dw_ = 2.9; aw_ = 3.7; sw_ = 0.2; lw_ = 1.0; apw_ = 60.0;
byw_ = 25.0; opw_ = 1.3; sfw_ = 50.0; faw_ = 25.0; rw_ = 0.15;
} else {
dw_ = 2.3; aw_ = 1.7; sw_ = 0.6; lw_ = 1.2; apw_ = 43.0;
byw_ = 15.0; opw_ = 1.2; sfw_ = 40.0; faw_ = 20.0; rw_ = 0.15;
}
}
};
}


int main(){
std::ios_base::sync_with_stdio(false);
std::cin.tie(nullptr);
int laps;std::cin>>laps;std::cin.ignore();
int nc;std::cin>>nc;std::cin.ignore();
std::vector<std::pair<double,double>>cps(nc);
for(int i=0;i<nc;i++){int x,y;std::cin>>x>>y;std::cin.ignore();cps[i]={(double)x,(double)y};}
arena::UltimateBot bot;bot.init(laps,cps);
while(true){
std::array<std::array<double,6>,4>pods;
for(int i=0;i<2;i++){int x,y,vx,vy,a,n;std::cin>>x>>y>>vx>>vy>>a>>n;std::cin.ignore();
pods[i]={(double)x,(double)y,(double)vx,(double)vy,(double)a,(double)n};}
for(int i=0;i<2;i++){int x,y,vx,vy,a,n;std::cin>>x>>y>>vx>>vy>>a>>n;std::cin.ignore();
pods[i+2]={(double)x,(double)y,(double)vx,(double)vy,(double)a,(double)n};}
auto[a0,a1]=bot.get_actions(pods);
std::cout<<(int)a0.target_x<<" "<<(int)a0.target_y<<" "<<a0.thrust_str()<<std::endl;
std::cout<<(int)a1.target_x<<" "<<(int)a1.target_y<<" "<<a1.thrust_str()<<std::endl;
}return 0;}
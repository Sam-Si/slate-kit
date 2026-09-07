#define CG_BOT_AMALGAM_GENERATED 1
#define CG_STANDALONE
#define CG_BOT_AMALGAM 1
#include <cmath>
extern thread_local bool g_friendly_collision;
namespace csb {
namespace fast {
inline constexpr double kFriction = 0.85;
inline constexpr double kPodCollisionRsq = 640000.0;
inline constexpr double kGaFarApartC = 3360000.0;
inline constexpr double kMinImpulse = 120.0;
inline constexpr double kShieldMassFast = 10.0;
inline constexpr double kNormalMassFast = 1.0;
inline constexpr int kShieldTimerActivate = 4;
struct Pod {
int id = 0;
int team = 0;
struct Vec2 {
double x = 0;
double y = 0;
Vec2() = default;
Vec2(double x_, double y_) : x(x_), y(y_) {}
} pos, vel;
double angle = -1.0;
int next_cp_id = 0;
bool boost_available = true;
int shield_cd = 0;
int timeout = 0;
int laps_completed = 0;
double Mass() const {
return (shield_cd == kShieldTimerActivate) ? kShieldMassFast : kNormalMassFast;
}
void Move(double t) {
pos.x += vel.x * t;
pos.y += vel.y * t;
}
void EndTurn() {
pos.x = std::floor(pos.x + 0.5);
pos.y = std::floor(pos.y + 0.5);
vel.x = std::trunc(vel.x * kFriction);
vel.y = std::trunc(vel.y * kFriction);
if (shield_cd > 0) shield_cd--;
}
};
inline double GetCollisionTime(const Pod& p1, const Pod& p2) {
double x = p1.pos.x - p2.pos.x;
double y = p1.pos.y - p2.pos.y;
double c = x * x + y * y - kPodCollisionRsq;
if (c > kGaFarApartC) return -1.0;
double vx = p1.vel.x - p2.vel.x;
double vy = p1.vel.y - p2.vel.y;
double a = vx * vx + vy * vy;
if (a < 0.00001) return -1.0;
double b = 2.0 * (x * vx + y * vy);
if (c >= 0.0 && b >= 0.0) return -1.0;
double delta = b * b - 4.0 * a * c;
if (delta < 0.0) return -1.0;
double t = (-b - std::sqrt(delta)) / (2.0 * a);
if (t < 0.0) return -1.0;
return t;
}
inline void ResolveCollision(Pod& p1, Pod& p2) {
if ((p1.id == 0 && p2.id == 1) || (p1.id == 2 && p2.id == 3)) {
g_friendly_collision = true;
}
double m1 = p1.Mass();
double m2 = p2.Mass();
double mcoeff = (m1 + m2) / (m1 * m2);
double nx = p1.pos.x - p2.pos.x;
double ny = p1.pos.y - p2.pos.y;
double nxnysquare = nx * nx + ny * ny;
double dvx = p1.vel.x - p2.vel.x;
double dvy = p1.vel.y - p2.vel.y;
double product = nx * dvx + ny * dvy;
double fx = (nx * product) / (nxnysquare * mcoeff);
double fy = (ny * product) / (nxnysquare * mcoeff);
p1.vel.x -= fx / m1;
p1.vel.y -= fy / m1;
p2.vel.x += fx / m2;
p2.vel.y += fy / m2;
double impulse = std::sqrt(fx * fx + fy * fy);
if (impulse < kMinImpulse) {
fx = fx * kMinImpulse / impulse;
fy = fy * kMinImpulse / impulse;
}
p1.vel.x -= fx / m1;
p1.vel.y -= fy / m1;
p2.vel.x += fx / m2;
p2.vel.y += fy / m2;
}
inline void SimulateTurn(Pod* p) {
double t_current = 0.0;
int col_count = 0;
while (t_current < 1.0 && col_count < 10) {
double first_col_t = 2.0;
Pod* col_p1 = nullptr;
Pod* col_p2 = nullptr;
double t;
t = GetCollisionTime(p[0], p[1]);
if (t >= 0.0 && t + t_current < 1.0 && t < first_col_t) { first_col_t = t; col_p1 = &p[0]; col_p2 = &p[1]; }
t = GetCollisionTime(p[0], p[2]);
if (t >= 0.0 && t + t_current < 1.0 && t < first_col_t) { first_col_t = t; col_p1 = &p[0]; col_p2 = &p[2]; }
t = GetCollisionTime(p[0], p[3]);
if (t >= 0.0 && t + t_current < 1.0 && t < first_col_t) { first_col_t = t; col_p1 = &p[0]; col_p2 = &p[3]; }
t = GetCollisionTime(p[1], p[2]);
if (t >= 0.0 && t + t_current < 1.0 && t < first_col_t) { first_col_t = t; col_p1 = &p[1]; col_p2 = &p[2]; }
t = GetCollisionTime(p[1], p[3]);
if (t >= 0.0 && t + t_current < 1.0 && t < first_col_t) { first_col_t = t; col_p1 = &p[1]; col_p2 = &p[3]; }
t = GetCollisionTime(p[2], p[3]);
if (t >= 0.0 && t + t_current < 1.0 && t < first_col_t) { first_col_t = t; col_p1 = &p[2]; col_p2 = &p[3]; }
if (first_col_t > 1.0 - t_current) {
p[0].Move(1.0 - t_current);
p[1].Move(1.0 - t_current);
p[2].Move(1.0 - t_current);
p[3].Move(1.0 - t_current);
t_current = 1.0;
break;
}
if (first_col_t < 0.0001) first_col_t = 0.0001;
p[0].Move(first_col_t);
p[1].Move(first_col_t);
p[2].Move(first_col_t);
p[3].Move(first_col_t);
if (col_p1 && col_p2) ResolveCollision(*col_p1, *col_p2);
t_current += first_col_t;
col_count++;
}
if (t_current < 1.0) {
p[0].Move(1.0 - t_current);
p[1].Move(1.0 - t_current);
p[2].Move(1.0 - t_current);
p[3].Move(1.0 - t_current);
}
p[0].EndTurn();
p[1].EndTurn();
p[2].EndTurn();
p[3].EndTurn();
}
}
}
#include <cmath>
#include <algorithm>
namespace ga_pure {
inline constexpr double kMaxRotateDeg = 18.0;
inline constexpr double kBoundsPenalty = -100000.0;
inline constexpr double kCpCrossedPoints = 15000.0;
inline constexpr double kBoundMinX = -1000.0;
inline constexpr double kBoundMaxX = 17000.0;
inline constexpr double kBoundMinY = -1000.0;
inline constexpr double kBoundMaxY = 10000.0;
inline double NormalizeAngleDeg(double a) {
while (a >= 360.0) a -= 360.0;
while (a < 0.0) a += 360.0;
return a;
}
inline double ShortestAngleDiffDeg(double current, double target) {
double diff = target - current;
while (diff > 180.0) diff -= 360.0;
while (diff < -180.0) diff += 360.0;
return diff;
}
inline double RadToDeg(double radians) { return radians * (180.0 / 3.14159265358979323846); }
inline double ClampAngleShiftDeg(double shift, double max_rot = kMaxRotateDeg) {
return std::max(-max_rot, std::min(max_rot, shift));
}
inline double AngleShiftTowardTarget(double pod_angle_deg, double px, double py,
double tx, double ty,
double max_rot = kMaxRotateDeg) {
const double desired = RadToDeg(std::atan2(ty - py, tx - px));
const double shift = ShortestAngleDiffDeg(pod_angle_deg, desired);
return ClampAngleShiftDeg(shift, max_rot);
}
inline double BoundsPenalty(double x, double y) {
if (x < kBoundMinX || x > kBoundMaxX || y < kBoundMinY || y > kBoundMaxY) {
return kBoundsPenalty;
}
return 0.0;
}
inline double CpCrossedBonus(int cps_crossed) {
if (cps_crossed <= 0) return 0.0;
return static_cast<double>(cps_crossed) * kCpCrossedPoints;
}
inline double DistWeightTerm(double remain, double dist_weight) {
return remain * dist_weight;
}
inline double AlignTerm(double vx, double vy, double nx, double ny, double align_weight) {
return (vx * nx + vy * ny) * align_weight;
}
inline double LateralTerm(double vx, double vy, double nx, double ny, double lateral_penalty) {
const double lateral = vx * ny - vy * nx;
return std::fabs(lateral) * lateral_penalty;
}
inline double AngleErrorTerm(double abs_angle_err_deg, double angle_penalty) {
return abs_angle_err_deg * angle_penalty;
}
inline double SpeedTerm(double vx, double vy, double speed_bonus) {
return std::sqrt(vx * vx + vy * vy) * speed_bonus;
}
inline double FreeFlightFrictionVel(double v, double friction = 0.85) {
return std::trunc(v * friction);
}
inline double FastActPenalty(double activation_turns, double weight = 1000.0) {
return weight * activation_turns;
}
inline double RunnerKinematicsScore(double px, double py, double vx, double vy, double angle_deg,
double epx, double epy,
double align_weight, double lateral_penalty,
double angle_penalty, double speed_bonus) {
double score = 0.0;
const double dx = epx - px;
const double dy = epy - py;
const double d = std::sqrt(dx * dx + dy * dy);
if (d > 0.0) {
const double nx = dx / d;
const double ny = dy / d;
score += AlignTerm(vx, vy, nx, ny, align_weight);
score -= LateralTerm(vx, vy, nx, ny, lateral_penalty);
const double target_angle = RadToDeg(std::atan2(dy, dx));
const double angle_err = std::fabs(ShortestAngleDiffDeg(angle_deg, target_angle));
score -= AngleErrorTerm(angle_err, angle_penalty);
}
score += SpeedTerm(vx, vy, speed_bonus);
return score;
}
}
#pragma GCC optimize("O3,inline,omit-frame-pointer,unroll-loops")
#include <algorithm>
#include <cassert>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <thread>
#include <future>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <functional>
#include <memory>
#include <queue>
#include <cstddef>
using namespace std;
#if !defined(CG_BOT_AMALGAM)
#include "ga_pure.h"
#endif
static constexpr double kCgFriction = 0.85;
constexpr double DEFAULT_TURN_TIME_LIMIT_MS = 75.0;
constexpr double DEFAULT_FIRST_TURN_TIME_LIMIT_MS = 1000.0;
constexpr double DEFAULT_TURN_TAIL_RESERVE_MS = 5.0;
constexpr double DEFAULT_FIRST_TURN_TAIL_RESERVE_MS = 25.0;
constexpr double DEFAULT_SEARCH_SAFETY_MS = 2.0;
constexpr double DEFAULT_FIRST_TURN_SEARCH_SAFETY_MS = 50.0;
inline std::atomic<uint64_t> g_sim_turn_count{0};
inline void ResetSimTurnCount() { g_sim_turn_count.store(0, std::memory_order_relaxed); }
inline uint64_t GetSimTurnCount() { return g_sim_turn_count.load(std::memory_order_relaxed); }
inline void NoteSimTurn() { g_sim_turn_count.fetch_add(1, std::memory_order_relaxed); }
inline double GaSearchDeadlineMs(double hard_limit_ms, bool first_turn,
double turn_tail, double first_tail) {
const double tail = first_turn ? first_tail : turn_tail;
double d = hard_limit_ms - tail;
if (d < 2.0) d = hard_limit_ms * 0.9;
if (d < 1.0) d = 1.0;
return d;
}
inline double GaLoopLimitMs(double search_deadline_ms, bool first_turn,
double search_safety, double first_safety) {
const double safety = first_turn ? first_safety : search_safety;
double lim = search_deadline_ms - safety;
if (lim < 1.0) lim = 1.0;
return lim;
}
constexpr double RUNNER_BYPASS_WEIGHT = 20.0;
constexpr double RUNNER_FAST_ACT_PENALTY = 1000.0;
constexpr double OPPONENT_DELAY_WEIGHT = 10000.0;
constexpr double OPPONENT_DISTANCE_WEIGHT = 10.0;
constexpr double BLOCKER_REST_THRESHOLD = 300.0;
constexpr double BLOCKER_REST_SOFT_FACTOR = 0.1;
constexpr double BLOCKER_REST_PENALTY_FACTOR = 0.045;
constexpr double BLOCKER_FACING_WEIGHT = 30.0;
constexpr double BLOCKER_STAY_IN_FRONT_WEIGHT = 30.0;
constexpr double RUNNER_SHIELD_COST = 330.0;
constexpr double RUNNER_THRUST_BONUS = 0.16;
constexpr double BLOCKER_SHIELD_COST = 495.0;
constexpr double BLOCKER_THRUST_BONUS = 0.06;
#ifdef CG_STANDALONE
extern const double PI;
extern double cos_lut[360];
extern double sin_lut[360];
extern thread_local bool g_friendly_collision;
extern thread_local int g_runner_id;
void InitLUT();
void SeedRand(uint32_t seed);
uint32_t FastRand();
int FastRandInt(int min, int max);
class Timer {
std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
public:
void Start();
double ElapsedMs() const;
};
struct Vec2 {
double x, y;
Vec2();
Vec2(double x, double y);
Vec2 Add(const Vec2& o) const;
Vec2 Sub(const Vec2& o) const;
double DistanceSq(const Vec2& o) const;
double Distance(const Vec2& o) const;
};
class GameEngine {
public:
static double NormalizeAngle(double a);
static double ShortestAngleDiff(double current, double target);
static double RadToDeg(double radians);
};
struct PodAction {
double tx, ty;
int thrust;
};
struct Pod {
int id;
int team;
Vec2 pos, vel;
double angle;
int next_cp_id;
bool boost_available;
int shield_cd;
int timeout;
int laps_completed;
Pod();
double Mass() const;
void ApplyGAAction(double angle_shift, int thrust);
void ApplyServerAction(double tx, double ty, int thrust_val);
void Move(double t);
void EndTurn();
};
struct BotConfig {
int horizon = 6;
int population = 50;
double dist_weight = 1.5;
double align_weight = 3.0;
double speed_bonus = 0.5;
double lateral_penalty = 0.5;
double angle_penalty = 25.0;
double corner_cut_dist = 300.0;
double block_weight = 1.0;
double shield_penalty = 50.0;
double shield_ram_dist = 850.0;
double opp_penalty = 0.5;
double opp_model_ms = 0.0;
double runner_bypass_weight = 20.0;
double blocker_stay_in_front_weight = 30.0;
double blocker_facing_weight = 30.0;
double runner_evasion_weight = 1.0;
double turn_time_limit_ms = 75.0;
double first_turn_time_limit_ms = 1000.0;
double turn_tail_reserve_ms = 5.0;
double first_turn_tail_reserve_ms = 25.0;
double search_safety_ms = 2.0;
double first_turn_search_safety_ms = 50.0;
int num_threads = 1;
std::string name = "DefaultGA";
void Randomize();
};
class IBot {
public:
virtual ~IBot() = default;
virtual std::string GetName() const = 0;
virtual void Initialize(int laps, int cp_count, const std::vector<Vec2>& cps, int team_id) = 0;
virtual std::vector<PodAction> GetActions(const std::vector<Pod>& pods) = 0;
virtual void SetRoles(int runner_idx, int blocker_idx) {}
};
#else
#error "this paste is the standalone CodinGame build"
#endif
const int MAX_HORIZON = 8;
const int MAX_POP = 64;
struct Action {
double angle = 0;
int thrust = 0;
void Randomize();
void MutateAggressive(double amplitude);
void SmallMutate();
};
struct Solution {
double score;
Action runner_moves[MAX_HORIZON];
Action blocker_moves[MAX_HORIZON];
int runner_shield_step;
int blocker_shield_step;
Solution();
void Randomize(int horizon);
void MutateFromOne(const Solution& parent, int horizon, double amplitude);
void CrossoverFromTwo(const Solution& a, const Solution& b, int horizon);
};
Action MakeGoToTarget(const Pod& pod, double tx, double ty, int thrust_val = 200);
class GABotThreadPool {
struct Worker {
std::thread thread;
std::mutex mutex;
std::condition_variable cv;
std::function<void()> task = nullptr;
bool has_task = false;
bool stop = false;
};
std::vector<std::unique_ptr<Worker>> workers;
std::mutex master_mutex;
std::condition_variable master_cv;
std::atomic<int> active_workers{0};
public:
GABotThreadPool(int num_threads) {
for (int i = 0; i < num_threads - 1; ++i) {
auto w = std::make_unique<Worker>();
Worker* ptr = w.get();
ptr->thread = std::thread([this, ptr]() {
while (true) {
std::function<void()> local_task = nullptr;
{
std::unique_lock<std::mutex> lock(ptr->mutex);
ptr->cv.wait(lock, [ptr]() { return ptr->has_task || ptr->stop; });
if (ptr->stop) return;
local_task = std::move(ptr->task);
ptr->has_task = false;
}
if (local_task) {
local_task();
}
int prev = active_workers.fetch_sub(1);
if (prev == 1) {
std::unique_lock<std::mutex> lock(master_mutex);
master_cv.notify_one();
}
}
});
workers.push_back(std::move(w));
}
}
~GABotThreadPool() {
for (auto& w : workers) {
{
std::unique_lock<std::mutex> lock(w->mutex);
w->stop = true;
}
w->cv.notify_one();
if (w->thread.joinable()) {
w->thread.join();
}
}
}
void RunParallel(const std::vector<std::function<void()>>& tasks) {
if (tasks.empty()) return;
int n_tasks = tasks.size();
int n_workers = workers.size();
int workers_to_run = std::min(n_tasks - 1, n_workers);
if (workers_to_run < 0) workers_to_run = 0;
active_workers.store(workers_to_run);
for (int i = 0; i < workers_to_run; ++i) {
{
std::unique_lock<std::mutex> lock(workers[i]->mutex);
workers[i]->task = tasks[i + 1];
workers[i]->has_task = true;
}
workers[i]->cv.notify_one();
}
tasks[0]();
if (workers_to_run > 0) {
std::unique_lock<std::mutex> lock(master_mutex);
master_cv.wait(lock, [this]() { return active_workers.load() == 0; });
}
}
};
struct MigrationChannel {
std::mutex mutex;
Solution solution;
bool initialized = false;
};
class GABot : public IBot {
int laps_ = 3;
int cp_count_ = 0;
std::vector<Vec2> cps_;
int team_id_ = 0;
int runner_idx_ = 0;
int blocker_idx_ = 1;
BotConfig config_;
bool has_prev_best_ = false;
Solution prev_best_;
int turn_count_ = 0;
double avg_dist_ = 0.0;
std::shared_ptr<GABotThreadPool> thread_pool_;
std::vector<double> dist_to_end_;
std::vector<Vec2> entry_points_;
std::vector<Vec2> ram_rest_points_;
std::vector<double> cp_distances_;
int total_cps_in_race_ = 0;
public:
static bool verbose;
GABot(BotConfig config = BotConfig());
std::string GetName() const override;
void Initialize(int laps, int cp_count, const std::vector<Vec2>& cps, int team_id) override;
void SetRoles(int runner_idx, int blocker_idx) override { runner_idx_ = runner_idx; blocker_idx_ = blocker_idx; }
std::vector<PodAction> GetActions(const std::vector<Pod>& pods) override;
};
#ifndef CG_BOT_AMALGAM
class Evolution {
public:
static void ApplyBasicProxy(Pod& p, const std::vector<Vec2>& cps) {
const Vec2& tgt = cps[p.next_cp_id];
const double shift = ga_pure::AngleShiftTowardTarget(
p.angle, p.pos.x, p.pos.y, tgt.x, tgt.y);
p.ApplyGAAction(shift, 200);
}
};
#endif
#ifdef CG_STANDALONE
#ifndef CG_BOT_AMALGAM
#include "fast.h"
#endif
const double PI = 3.14159265358979323846;
double cos_lut[360];
double sin_lut[360];
thread_local uint32_t xor_state = 2463534242;
thread_local bool g_friendly_collision = false;
void SeedRand(uint32_t seed) {
xor_state = seed;
}
void InitLUT() {
for (int i = 0; i < 360; ++i) {
cos_lut[i] = std::cos(i * PI / 180.0);
sin_lut[i] = std::sin(i * PI / 180.0);
}
}
uint32_t FastRand() {
xor_state ^= xor_state << 13;
xor_state ^= xor_state >> 17;
xor_state ^= xor_state << 5;
return xor_state;
}
int FastRandInt(int min, int max) {
uint32_t range = max - min + 1;
uint64_t multi = (uint64_t)FastRand() * range;
return min + (int)(multi >> 32);
}
void Timer::Start() { start_time = std::chrono::high_resolution_clock::now(); }
double Timer::ElapsedMs() const { return std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - start_time).count(); }
Vec2::Vec2() : x(0), y(0) {}
Vec2::Vec2(double x, double y) : x(x), y(y) {}
Vec2 Vec2::Add(const Vec2& o) const { return Vec2(x + o.x, y + o.y); }
Vec2 Vec2::Sub(const Vec2& o) const { return Vec2(x - o.x, y - o.y); }
double Vec2::DistanceSq(const Vec2& o) const { return (x - o.x)*(x - o.x) + (y - o.y)*(y - o.y); }
double Vec2::Distance(const Vec2& o) const { return std::sqrt(DistanceSq(o)); }
double GameEngine::NormalizeAngle(double a) {
while (a >= 360.0) a -= 360.0;
while (a < 0.0) a += 360.0;
return a;
}
double GameEngine::ShortestAngleDiff(double current, double target) {
double diff = target - current;
while (diff > 180.0) diff -= 360.0;
while (diff < -180.0) diff += 360.0;
return diff;
}
double GameEngine::RadToDeg(double radians) { return radians * 180.0 / PI; }
Pod::Pod() : id(0), team(0), pos(0,0), vel(0,0), angle(-1.0), next_cp_id(0), boost_available(true), shield_cd(0), timeout(0), laps_completed(0) {}
double Pod::Mass() const { return (shield_cd == 4) ? 10.0 : 1.0; }
struct TrigLUT {
double cos_val[3600];
double sin_val[3600];
TrigLUT() {
for (int i = 0; i < 3600; ++i) {
double rad = (i * 0.1) * (3.14159265358979323846 / 180.0);
cos_val[i] = std::cos(rad);
sin_val[i] = std::sin(rad);
}
}
inline void Lookup(double deg, double& out_cos, double& out_sin) const {
double a = deg;
while (a >= 360.0) a -= 360.0;
while (a < 0.0) a += 360.0;
int idx = static_cast<int>(a * 10.0 + 0.5);
if (idx >= 3600) idx = 0;
out_cos = cos_val[idx];
out_sin = sin_val[idx];
}
};
inline const TrigLUT& GetTrigLUT() {
static TrigLUT lut;
return lut;
}
void Pod::ApplyGAAction(double angle_shift, int thrust_val) {
if (thrust_val == -1) { shield_cd = 4; thrust_val = 0; }
else if (shield_cd > 0) { thrust_val = 0; }
if (thrust_val == 650) boost_available = false;
if (angle < 0) angle = 0;
else angle = GameEngine::NormalizeAngle(angle + angle_shift);
double c, s;
GetTrigLUT().Lookup(angle, c, s);
vel.x += c * thrust_val;
vel.y += s * thrust_val;
}
void Pod::ApplyServerAction(double tx, double ty, int thrust_val) {
if (thrust_val == -1) { shield_cd = 4; thrust_val = 0; }
else if (shield_cd > 0) { thrust_val = 0; }
if (thrust_val == 650) {
if (boost_available) { thrust_val = 650; boost_available = false; }
else thrust_val = 200;
}
if (tx == pos.x && ty == pos.y) return;
double target_angle = GameEngine::RadToDeg(std::atan2(ty - pos.y, tx - pos.x));
if (angle < 0) {
angle = GameEngine::NormalizeAngle(target_angle);
} else {
double diff = GameEngine::ShortestAngleDiff(angle, target_angle);
if (diff > 18.0) diff = 18.0;
if (diff < -18.0) diff = -18.0;
angle = GameEngine::NormalizeAngle(angle + diff);
}
double rad = angle * PI / 180.0;
vel.x += std::cos(rad) * thrust_val;
vel.y += std::sin(rad) * thrust_val;
}
void Pod::Move(double t) {
pos.x += vel.x * t;
pos.y += vel.y * t;
}
void Pod::EndTurn() {
pos.x = std::floor(pos.x + 0.5);
pos.y = std::floor(pos.y + 0.5);
vel.x = ga_pure::FreeFlightFrictionVel(vel.x, kCgFriction);
vel.y = ga_pure::FreeFlightFrictionVel(vel.y, kCgFriction);
if (shield_cd > 0) shield_cd--;
}
inline void FastSimulateTurn(Pod* pods) {
static_assert(sizeof(Pod) == sizeof(csb::fast::Pod), "standalone Pod layout must match csb::fast::Pod");
static_assert(offsetof(Pod, id) == offsetof(csb::fast::Pod, id), "id offset");
static_assert(offsetof(Pod, team) == offsetof(csb::fast::Pod, team), "team offset");
static_assert(offsetof(Pod, pos) == offsetof(csb::fast::Pod, pos), "pos offset");
static_assert(offsetof(Pod, vel) == offsetof(csb::fast::Pod, vel), "vel offset");
static_assert(offsetof(Pod, angle) == offsetof(csb::fast::Pod, angle), "angle offset");
static_assert(offsetof(Pod, next_cp_id) == offsetof(csb::fast::Pod, next_cp_id), "next_cp_id offset");
static_assert(offsetof(Pod, boost_available) == offsetof(csb::fast::Pod, boost_available), "boost_available offset");
static_assert(offsetof(Pod, shield_cd) == offsetof(csb::fast::Pod, shield_cd), "shield_cd offset");
static_assert(offsetof(Pod, timeout) == offsetof(csb::fast::Pod, timeout), "timeout offset");
static_assert(offsetof(Pod, laps_completed) == offsetof(csb::fast::Pod, laps_completed), "laps_completed offset");
csb::fast::SimulateTurn(reinterpret_cast<csb::fast::Pod*>(pods));
}
#endif
static inline void SearchSimulateTurn(Pod* pods) {
FastSimulateTurn(pods);
NoteSimTurn();
}
static thread_local Pod g_sim[4];
static inline double GetCPIntersectionTime(const Pod& p, const Vec2& cp) {
double x = p.pos.x - cp.x;
double y = p.pos.y - cp.y;
double vx = p.vel.x;
double vy = p.vel.y;
double a = vx * vx + vy * vy;
if (a < 0.00001) return -1.0;
double b = 2.0 * (x * vx + y * vy);
double c = x * x + y * y - 360000.0;
if (c < 0.0) return 0.0;
double delta = b * b - 4.0 * a * c;
if (delta < 0.0) return -1.0;
double t = (-b - std::sqrt(delta)) / (2.0 * a);
if (t < 0.0) return -1.0;
return t;
}
void Action::Randomize() {
angle = FastRandInt(-180, 180) / 10.0;
int raw = FastRandInt(-100, 500);
thrust = (raw < 0) ? 0 : (raw > 200) ? 200 : raw;
}
void Action::MutateAggressive(double amplitude) {
double threshold = 0.25 + amplitude;
if (FastRandInt(0, 1000) / 1000.0 < threshold) {
angle = FastRandInt(-400, 400) / 10.0;
if (angle < -18.0) angle = -18.0;
if (angle > 18.0) angle = 18.0;
}
if (FastRandInt(0, 1000) / 1000.0 < threshold) {
int raw = FastRandInt(-100, 500);
thrust = (raw < 0) ? 0 : (raw > 200) ? 200 : raw;
}
}
void Action::SmallMutate() {
angle += FastRandInt(-120, 120) / 10.0;
if (angle < -18.0) angle = -18.0;
if (angle > 18.0) angle = 18.0;
thrust += FastRandInt(-50, 50);
if (thrust < 0) thrust = 0;
if (thrust > 200) thrust = 200;
}
void BotConfig::Randomize() {
horizon = FastRandInt(4, 8);
population = FastRandInt(20, 48);
dist_weight = FastRandInt(5, 25) / 10.0;
align_weight = FastRandInt(5, 50) / 10.0;
speed_bonus = FastRandInt(0, 10) / 10.0;
lateral_penalty = FastRandInt(0, 20) / 10.0;
angle_penalty = FastRandInt(10, 60);
corner_cut_dist = FastRandInt(0, 600);
block_weight = FastRandInt(0, 30) / 10.0;
shield_penalty = FastRandInt(0, 100);
shield_ram_dist = FastRandInt(600, 1200);
opp_penalty = FastRandInt(0, 20) / 10.0;
opp_model_ms = 0;
runner_bypass_weight = FastRandInt(50, 500) / 10.0;
blocker_stay_in_front_weight = FastRandInt(50, 400) / 10.0;
blocker_facing_weight = FastRandInt(50, 400) / 10.0;
runner_evasion_weight = FastRandInt(1, 10) / 10.0;
}
Action MakeGoToTarget(const Pod& pod, double tx, double ty, int thrust_val) {
const double shift = ga_pure::AngleShiftTowardTarget(
pod.angle, pod.pos.x, pod.pos.y, tx, ty);
return {shift, thrust_val};
}
Solution::Solution() : score(-1e18), runner_shield_step(MAX_HORIZON), blocker_shield_step(MAX_HORIZON) {}
void Solution::Randomize(int horizon) {
for (int i = 0; i < horizon; ++i) {
runner_moves[i].Randomize();
blocker_moves[i].Randomize();
}
runner_shield_step = FastRandInt(0, 7);
blocker_shield_step = FastRandInt(0, 7);
}
void Solution::MutateFromOne(const Solution& parent, int horizon, double amplitude) {
double threshold = 0.25 + amplitude;
for (int t = 0; t < horizon; ++t) {
runner_moves[t] = parent.runner_moves[t];
runner_moves[t].MutateAggressive(amplitude);
blocker_moves[t] = parent.blocker_moves[t];
blocker_moves[t].MutateAggressive(amplitude);
}
runner_shield_step = parent.runner_shield_step;
blocker_shield_step = parent.blocker_shield_step;
if (FastRandInt(0, 1000) / 1000.0 < threshold) runner_shield_step = FastRandInt(0, 7);
if (FastRandInt(0, 1000) / 1000.0 < threshold) blocker_shield_step = FastRandInt(0, 7);
}
void Solution::CrossoverFromTwo(const Solution& a, const Solution& b, int horizon) {
for (int t = 0; t < horizon; ++t) {
runner_moves[t] = (FastRandInt(0, 1) == 0) ? a.runner_moves[t] : b.runner_moves[t];
blocker_moves[t] = (FastRandInt(0, 1) == 0) ? a.blocker_moves[t] : b.blocker_moves[t];
}
runner_shield_step = (FastRandInt(0, 1) == 0) ? a.runner_shield_step : b.runner_shield_step;
blocker_shield_step = (FastRandInt(0, 1) == 0) ? a.blocker_shield_step : b.blocker_shield_step;
}
struct SimCtx {
const vector<Vec2>* cps;
const vector<double>* dist_to_end;
const vector<Vec2>* entry_points;
const vector<Vec2>* ram_rest_points;
int cp_count;
int laps;
int start_idx;
int opp_start_idx;
int runner_idx;
int ram_beacon;
bool risk_timeout;
const BotConfig* config;
bool force_boost;
Action opp_moves[MAX_HORIZON];
bool has_opp_blocker_override = false;
Action opp_blocker_override;
};
struct TeamAction {
Action runner_move;
Action blocker_move;
int runner_shield_step = MAX_HORIZON;
int blocker_shield_step = MAX_HORIZON;
std::string name;
};
static double EvaluateTacticalCell(const TeamAction& my_act, const TeamAction& opp_act,
const vector<Pod>& base_pods, const SimCtx& ctx) {
const int n = ctx.cp_count;
const int runner_pod = ctx.start_idx + ctx.runner_idx;
const int blocker_pod = ctx.start_idx + (1 - ctx.runner_idx);
const auto& cps = *ctx.cps;
const auto& dte = *ctx.dist_to_end;
const auto& ep = *ctx.entry_points;
std::memcpy(g_sim, base_pods.data(), 4 * sizeof(Pod));
g_friendly_collision = false;
int opp0 = ctx.opp_start_idx, opp1 = ctx.opp_start_idx + 1;
int opp0_lin = g_sim[opp0].laps_completed * n + g_sim[opp0].next_cp_id;
int opp1_lin = g_sim[opp1].laps_completed * n + g_sim[opp1].next_cp_id;
double opp0_d = g_sim[opp0].pos.Distance(cps[g_sim[opp0].next_cp_id]);
double opp1_d = g_sim[opp1].pos.Distance(cps[g_sim[opp1].next_cp_id]);
int opp_runner, opp_blocker;
if (opp0_lin > opp1_lin || (opp0_lin == opp1_lin && opp0_d < opp1_d)) {
opp_runner = opp0; opp_blocker = opp1;
} else {
opp_runner = opp1; opp_blocker = opp0;
}
int r_thr = my_act.runner_move.thrust;
if (my_act.runner_shield_step == 0 && g_sim[runner_pod].shield_cd == 0) r_thr = -1;
g_sim[runner_pod].ApplyGAAction(my_act.runner_move.angle, r_thr);
int b_thr = my_act.blocker_move.thrust;
if (my_act.blocker_shield_step == 0 && g_sim[blocker_pod].shield_cd == 0) b_thr = -1;
g_sim[blocker_pod].ApplyGAAction(my_act.blocker_move.angle, b_thr);
int opp_r_thr = opp_act.runner_move.thrust;
if (opp_act.runner_shield_step == 0 && g_sim[opp_runner].shield_cd == 0) opp_r_thr = -1;
g_sim[opp_runner].ApplyGAAction(opp_act.runner_move.angle, opp_r_thr);
int opp_b_thr = opp_act.blocker_move.thrust;
if (opp_act.blocker_shield_step == 0 && g_sim[opp_blocker].shield_cd == 0) opp_b_thr = -1;
g_sim[opp_blocker].ApplyGAAction(opp_act.blocker_move.angle, opp_b_thr);
SearchSimulateTurn(g_sim);
for (int i = 0; i < 4; ++i) {
double dx = g_sim[i].pos.x - cps[g_sim[i].next_cp_id].x;
double dy = g_sim[i].pos.y - cps[g_sim[i].next_cp_id].y;
if (dx * dx + dy * dy <= 360000.0) {
g_sim[i].next_cp_id++;
if (g_sim[i].next_cp_id >= n) {
g_sim[i].next_cp_id = 0;
g_sim[i].laps_completed++;
}
}
}
for (int t = 1; t <= 3; ++t) {
{
const Vec2& tgt = ep[g_sim[runner_pod].next_cp_id];
Action a = MakeGoToTarget(g_sim[runner_pod], tgt.x, tgt.y, 200);
g_sim[runner_pod].ApplyGAAction(a.angle, a.thrust);
}
{
double tx = g_sim[opp_runner].pos.x + g_sim[opp_runner].vel.x;
double ty = g_sim[opp_runner].pos.y + g_sim[opp_runner].vel.y;
Action a = MakeGoToTarget(g_sim[blocker_pod], tx, ty, 200);
g_sim[blocker_pod].ApplyGAAction(a.angle, a.thrust);
}
{
const Vec2& tgt = ep[g_sim[opp_runner].next_cp_id];
Action a = MakeGoToTarget(g_sim[opp_runner], tgt.x, tgt.y, 200);
g_sim[opp_runner].ApplyGAAction(a.angle, a.thrust);
}
{
double tx = g_sim[runner_pod].pos.x + g_sim[runner_pod].vel.x;
double ty = g_sim[runner_pod].pos.y + g_sim[runner_pod].vel.y;
Action a = MakeGoToTarget(g_sim[opp_blocker], tx, ty, 200);
g_sim[opp_blocker].ApplyGAAction(a.angle, a.thrust);
}
SearchSimulateTurn(g_sim);
for (int i = 0; i < 4; ++i) {
double dx = g_sim[i].pos.x - cps[g_sim[i].next_cp_id].x;
double dy = g_sim[i].pos.y - cps[g_sim[i].next_cp_id].y;
if (dx * dx + dy * dy <= 360000.0) {
g_sim[i].next_cp_id++;
if (g_sim[i].next_cp_id >= n) {
g_sim[i].next_cp_id = 0;
g_sim[i].laps_completed++;
}
}
}
}
double score = 0.0;
int r_lin = g_sim[runner_pod].laps_completed * n + g_sim[runner_pod].next_cp_id;
score -= dte[r_lin] + g_sim[runner_pod].pos.Distance(ep[g_sim[runner_pod].next_cp_id]);
int opp_lin = g_sim[opp_runner].laps_completed * n + g_sim[opp_runner].next_cp_id;
score += dte[opp_lin] + g_sim[opp_runner].pos.Distance(ep[g_sim[opp_runner].next_cp_id]);
double blocker_opp_dist = g_sim[blocker_pod].pos.Distance(g_sim[opp_runner].pos);
if (blocker_opp_dist < 1200.0) {
score += 2000.0 * (1200.0 - blocker_opp_dist) / 1200.0;
}
if (g_friendly_collision) {
score -= 5000.0;
}
return score;
}
static double SimulateAndEvaluate(const Solution& sol, const vector<Pod>& base_pods,
const SimCtx& ctx, int horizon) {
const int n = ctx.cp_count;
const int runner_pod = ctx.start_idx + ctx.runner_idx;
const int blocker_pod = ctx.start_idx + (1 - ctx.runner_idx);
const auto& cps = *ctx.cps;
const auto& dte = *ctx.dist_to_end;
const auto& ep = *ctx.entry_points;
const BotConfig& config = *ctx.config;
std::memcpy(g_sim, base_pods.data(), 4 * sizeof(Pod));
g_friendly_collision = false;
int init_cp = g_sim[runner_pod].next_cp_id;
int init_lap = g_sim[runner_pod].laps_completed;
int init_blocker_cp = g_sim[blocker_pod].next_cp_id;
int init_blocker_lap = g_sim[blocker_pod].laps_completed;
int opp0 = ctx.opp_start_idx, opp1 = ctx.opp_start_idx + 1;
int opp0_lin = g_sim[opp0].laps_completed * n + g_sim[opp0].next_cp_id;
int opp1_lin = g_sim[opp1].laps_completed * n + g_sim[opp1].next_cp_id;
double opp0_d = g_sim[opp0].pos.Distance(cps[g_sim[opp0].next_cp_id]);
double opp1_d = g_sim[opp1].pos.Distance(cps[g_sim[opp1].next_cp_id]);
int opp_runner, opp_blocker;
if (opp0_lin > opp1_lin || (opp0_lin == opp1_lin && opp0_d < opp1_d)) {
opp_runner = opp0; opp_blocker = opp1;
} else {
opp_runner = opp1; opp_blocker = opp0;
}
double runner_activation = (double)horizon + 0.3;
double opp_activation = (double)horizon + 0.3;
int init_opp_cp = g_sim[opp_runner].next_cp_id;
int init_opp_lap = g_sim[opp_runner].laps_completed;
Pod g_sim_turn_1[4];
double runner_activation_turn_1 = (double)horizon + 0.3;
double opp_activation_turn_1 = (double)horizon + 0.3;
for (int t = 0; t < horizon; ++t) {
Pod runner_start = g_sim[runner_pod];
Pod opp_runner_start = g_sim[opp_runner];
int r_thr = sol.runner_moves[t].thrust;
if (t == 0 && ctx.force_boost && g_sim[runner_pod].boost_available) {
r_thr = 650;
} else if (t == sol.runner_shield_step && sol.runner_shield_step < 3 && g_sim[runner_pod].shield_cd == 0) {
r_thr = -1;
}
g_sim[runner_pod].ApplyGAAction(sol.runner_moves[t].angle, r_thr);
int b_thr = sol.blocker_moves[t].thrust;
if (t == sol.blocker_shield_step && sol.blocker_shield_step < 3 && g_sim[blocker_pod].shield_cd == 0)
b_thr = -1;
g_sim[blocker_pod].ApplyGAAction(sol.blocker_moves[t].angle, b_thr);
g_sim[opp_runner].ApplyGAAction(ctx.opp_moves[t].angle, ctx.opp_moves[t].thrust);
if (t == 0 && ctx.has_opp_blocker_override) {
g_sim[opp_blocker].ApplyGAAction(ctx.opp_blocker_override.angle, ctx.opp_blocker_override.thrust);
} else {
double tx = g_sim[runner_pod].pos.x + g_sim[runner_pod].vel.x;
double ty = g_sim[runner_pod].pos.y + g_sim[runner_pod].vel.y;
double desired = GameEngine::RadToDeg(atan2(ty - g_sim[opp_blocker].pos.y,
tx - g_sim[opp_blocker].pos.x));
double shift = GameEngine::ShortestAngleDiff(g_sim[opp_blocker].angle, desired);
shift = max(-18.0, min(18.0, shift));
int opp_thr = 200;
double opp_dist = g_sim[opp_blocker].pos.Distance(g_sim[runner_pod].pos);
if (opp_dist < config.shield_ram_dist && g_sim[opp_blocker].shield_cd == 0) opp_thr = -1;
g_sim[opp_blocker].ApplyGAAction(shift, opp_thr);
}
SearchSimulateTurn(g_sim);
{
double dx0 = g_sim[0].pos.x - cps[g_sim[0].next_cp_id].x;
double dy0 = g_sim[0].pos.y - cps[g_sim[0].next_cp_id].y;
if (dx0 * dx0 + dy0 * dy0 <= 360000.0) {
g_sim[0].next_cp_id++;
if (g_sim[0].next_cp_id >= n) {
g_sim[0].next_cp_id = 0;
g_sim[0].laps_completed++;
}
if (0 == runner_pod && runner_activation > horizon) {
double t_hit = GetCPIntersectionTime(runner_start, cps[runner_start.next_cp_id]);
runner_activation = (double)t + (t_hit >= 0.0 ? t_hit : 0.0);
}
if (0 == opp_runner && opp_activation > horizon) {
double t_hit = GetCPIntersectionTime(opp_runner_start, cps[opp_runner_start.next_cp_id]);
opp_activation = (double)t + (t_hit >= 0.0 ? t_hit : 0.0);
}
}
double dx1 = g_sim[1].pos.x - cps[g_sim[1].next_cp_id].x;
double dy1 = g_sim[1].pos.y - cps[g_sim[1].next_cp_id].y;
if (dx1 * dx1 + dy1 * dy1 <= 360000.0) {
g_sim[1].next_cp_id++;
if (g_sim[1].next_cp_id >= n) {
g_sim[1].next_cp_id = 0;
g_sim[1].laps_completed++;
}
if (1 == runner_pod && runner_activation > horizon) {
double t_hit = GetCPIntersectionTime(runner_start, cps[runner_start.next_cp_id]);
runner_activation = (double)t + (t_hit >= 0.0 ? t_hit : 0.0);
}
if (1 == opp_runner && opp_activation > horizon) {
double t_hit = GetCPIntersectionTime(opp_runner_start, cps[opp_runner_start.next_cp_id]);
opp_activation = (double)t + (t_hit >= 0.0 ? t_hit : 0.0);
}
}
double dx2 = g_sim[2].pos.x - cps[g_sim[2].next_cp_id].x;
double dy2 = g_sim[2].pos.y - cps[g_sim[2].next_cp_id].y;
if (dx2 * dx2 + dy2 * dy2 <= 360000.0) {
g_sim[2].next_cp_id++;
if (g_sim[2].next_cp_id >= n) {
g_sim[2].next_cp_id = 0;
g_sim[2].laps_completed++;
}
if (2 == runner_pod && runner_activation > horizon) {
double t_hit = GetCPIntersectionTime(runner_start, cps[runner_start.next_cp_id]);
runner_activation = (double)t + (t_hit >= 0.0 ? t_hit : 0.0);
}
if (2 == opp_runner && opp_activation > horizon) {
double t_hit = GetCPIntersectionTime(opp_runner_start, cps[opp_runner_start.next_cp_id]);
opp_activation = (double)t + (t_hit >= 0.0 ? t_hit : 0.0);
}
}
double dx3 = g_sim[3].pos.x - cps[g_sim[3].next_cp_id].x;
double dy3 = g_sim[3].pos.y - cps[g_sim[3].next_cp_id].y;
if (dx3 * dx3 + dy3 * dy3 <= 360000.0) {
g_sim[3].next_cp_id++;
if (g_sim[3].next_cp_id >= n) {
g_sim[3].next_cp_id = 0;
g_sim[3].laps_completed++;
}
if (3 == runner_pod && runner_activation > horizon) {
double t_hit = GetCPIntersectionTime(runner_start, cps[runner_start.next_cp_id]);
runner_activation = (double)t + (t_hit >= 0.0 ? t_hit : 0.0);
}
if (3 == opp_runner && opp_activation > horizon) {
double t_hit = GetCPIntersectionTime(opp_runner_start, cps[opp_runner_start.next_cp_id]);
opp_activation = (double)t + (t_hit >= 0.0 ? t_hit : 0.0);
}
}
}
if (t == 0) {
std::memcpy(g_sim_turn_1, g_sim, 4 * sizeof(Pod));
runner_activation_turn_1 = runner_activation;
opp_activation_turn_1 = opp_activation;
}
}
auto evaluate_state = [&](const Pod* state, double r_act, double o_act) -> double {
const Pod& runner = state[runner_pod];
const Pod& opp_run = state[opp_runner];
double score = 0;
score += ga_pure::BoundsPenalty(runner.pos.x, runner.pos.y);
score += ga_pure::BoundsPenalty(state[blocker_pod].pos.x, state[blocker_pod].pos.y);
if (runner.laps_completed >= ctx.laps) score += 1e9;
if (opp_run.laps_completed >= ctx.laps) score -= 1e9;
int initial_linear = init_lap * n + init_cp;
int current_linear = runner.laps_completed * n + runner.next_cp_id;
int cps_crossed = current_linear - initial_linear;
score += ga_pure::CpCrossedBonus(cps_crossed);
if (runner.laps_completed < ctx.laps) {
int runner_lin = runner.laps_completed * n + runner.next_cp_id;
double runner_remain = dte[runner_lin] + runner.pos.Distance(ep[runner.next_cp_id]);
score -= ga_pure::DistWeightTerm(runner_remain, config.dist_weight);
}
score += ga_pure::RunnerKinematicsScore(
runner.pos.x, runner.pos.y, runner.vel.x, runner.vel.y, runner.angle,
ep[runner.next_cp_id].x, ep[runner.next_cp_id].y,
config.align_weight, config.lateral_penalty,
config.angle_penalty, config.speed_bonus);
score -= ga_pure::FastActPenalty(r_act, RUNNER_FAST_ACT_PENALTY);
{
double ox = state[opp_blocker].pos.x - runner.pos.x;
double oy = state[opp_blocker].pos.y - runner.pos.y;
double cx = cps[runner.next_cp_id].x - runner.pos.x;
double cy = cps[runner.next_cp_id].y - runner.pos.y;
double cross_val = ox * cy - oy * cx;
double dot_val = ox * cx + oy * cy;
double angle_diff = atan2(fabs(cross_val), dot_val);
score += config.runner_bypass_weight * angle_diff;
}
double opp_delay_score = OPPONENT_DELAY_WEIGHT * config.opp_penalty * o_act;
if (opp_run.next_cp_id == init_opp_cp && opp_run.laps_completed == init_opp_lap) {
opp_delay_score += OPPONENT_DISTANCE_WEIGHT * config.opp_penalty * opp_run.pos.Distance(cps[opp_run.next_cp_id]);
}
score += opp_delay_score;
double blocker_score = 0;
{
const Pod& blocker = state[blocker_pod];
if (ctx.risk_timeout) {
if (blocker.laps_completed < ctx.laps) {
int blocker_lin = blocker.laps_completed * n + blocker.next_cp_id;
blocker_score -= (dte[blocker_lin] + blocker.pos.Distance(ep[blocker.next_cp_id])) * config.dist_weight;
}
} else {
bool enforce_camp = !(init_blocker_lap == 0 && init_blocker_cp <= 1);
if (enforce_camp) {
const Vec2& rrp = (*ctx.ram_rest_points)[ctx.ram_beacon];
double bd = blocker.pos.Distance(rrp);
double d_adj = bd - BLOCKER_REST_THRESHOLD;
if (d_adj < 0) d_adj *= BLOCKER_REST_SOFT_FACTOR;
blocker_score -= BLOCKER_REST_PENALTY_FACTOR * d_adj;
double dx = opp_run.pos.x - blocker.pos.x;
double dy = opp_run.pos.y - blocker.pos.y;
double d_sq = dx * dx + dy * dy;
if (d_sq > 0) {
int ang = (int)round(blocker.angle);
ang = (ang % 360 + 360) % 360;
double fx = cos_lut[ang];
double fy = sin_lut[ang];
double dot = (fx * dx + fy * dy) / sqrt(d_sq);
blocker_score -= config.blocker_facing_weight * (1.0 - dot) * 1.57;
}
double bx = blocker.pos.x - opp_run.pos.x;
double by = blocker.pos.y - opp_run.pos.y;
double cx = (*ctx.cps)[opp_run.next_cp_id].x - opp_run.pos.x;
double cy = (*ctx.cps)[opp_run.next_cp_id].y - opp_run.pos.y;
double d1_sq = bx * bx + by * by;
double d2_sq = cx * cx + cy * cy;
if (d1_sq > 0 && d2_sq > 0) {
double dot = (bx * cx + by * cy) / sqrt(d1_sq * d2_sq);
blocker_score -= config.blocker_stay_in_front_weight * (1.0 - dot) * 1.57;
}
} else {
double dist_to_runner = blocker.pos.Distance(runner.pos);
blocker_score -= dist_to_runner * config.dist_weight * 0.1;
}
}
}
score += blocker_score * config.block_weight;
return score;
};
double score_turn_1 = evaluate_state(g_sim_turn_1, runner_activation_turn_1, opp_activation_turn_1);
double score_turn_N = evaluate_state(g_sim, runner_activation, opp_activation);
double final_score = 0.10 * score_turn_1 + 0.90 * score_turn_N;
if (sol.runner_shield_step == 0) final_score -= RUNNER_SHIELD_COST;
else final_score += RUNNER_THRUST_BONUS * max(0, sol.runner_moves[0].thrust);
if (sol.blocker_shield_step == 0) final_score -= BLOCKER_SHIELD_COST;
else final_score += BLOCKER_THRUST_BONUS * max(0, sol.blocker_moves[0].thrust);
if (g_friendly_collision) {
final_score -= 10000.0;
}
return final_score;
}
static Solution RunGA(const vector<Pod>& base_pods, Timer& timer, double time_limit_ms,
const SimCtx& ctx, const BotConfig& config,
int thread_id, int num_threads,
MigrationChannel* shared_migrants,
std::atomic<bool>* stop_flag,
int* out_iters,
const Solution* warm_start,
const Solution* intra_warm_start = nullptr) {
int pop_size = min(config.population, (int)MAX_POP);
int horizon = min(config.horizon, (int)MAX_HORIZON);
int runner_pod = ctx.start_idx + ctx.runner_idx;
int blocker_pod = ctx.start_idx + (1 - ctx.runner_idx);
const auto& cps = *ctx.cps;
int n = ctx.cp_count;
Solution pop[MAX_POP];
double scores[MAX_POP];
int idx = 0;
if (warm_start) {
for (int t = 0; t < horizon - 1; ++t) {
pop[0].runner_moves[t] = warm_start->runner_moves[t + 1];
pop[0].blocker_moves[t] = warm_start->blocker_moves[t + 1];
}
pop[0].runner_moves[horizon - 1].Randomize();
pop[0].blocker_moves[horizon - 1].Randomize();
pop[0].runner_shield_step = (warm_start->runner_shield_step > 0) ? warm_start->runner_shield_step - 1 : MAX_HORIZON;
pop[0].blocker_shield_step = (warm_start->blocker_shield_step > 0) ? warm_start->blocker_shield_step - 1 : MAX_HORIZON;
idx = 1;
} else if (intra_warm_start) {
pop[0] = *intra_warm_start;
idx = 1;
}
if (idx > 0 && idx + 14 < pop_size) {
pop[idx] = pop[0];
for (int t = 0; t < horizon; ++t) {
pop[idx].runner_moves[t].SmallMutate();
pop[idx].blocker_moves[t].SmallMutate();
}
idx++;
pop[idx] = pop[0];
pop[idx].runner_shield_step = (pop[0].runner_shield_step < 3) ? MAX_HORIZON : 0;
pop[idx].blocker_shield_step = (pop[0].blocker_shield_step < 3) ? MAX_HORIZON : 1;
idx++;
}
{
Pod rsim = base_pods[runner_pod];
Action runner_h[MAX_HORIZON];
Vec2 rsim_pos[MAX_HORIZON];
for (int t = 0; t < horizon; ++t) {
const Vec2& tgt = (*ctx.entry_points)[rsim.next_cp_id];
runner_h[t] = MakeGoToTarget(rsim, tgt.x, tgt.y, 200);
rsim.ApplyGAAction(runner_h[t].angle, runner_h[t].thrust);
rsim.pos.x += rsim.vel.x; rsim.pos.y += rsim.vel.y;
rsim.vel.x = ga_pure::FreeFlightFrictionVel(rsim.vel.x, kCgFriction); rsim.vel.y = ga_pure::FreeFlightFrictionVel(rsim.vel.y, kCgFriction);
rsim.pos.x = round(rsim.pos.x); rsim.pos.y = round(rsim.pos.y);
rsim_pos[t] = rsim.pos;
if (rsim.pos.DistanceSq(cps[rsim.next_cp_id]) <= 360000) {
rsim.next_cp_id++;
if (rsim.next_cp_id >= n) rsim.next_cp_id = 0;
}
}
int opp0 = ctx.opp_start_idx, opp1 = ctx.opp_start_idx + 1;
int o0lin = base_pods[opp0].laps_completed * n + base_pods[opp0].next_cp_id;
int o1lin = base_pods[opp1].laps_completed * n + base_pods[opp1].next_cp_id;
int opp_r = (o0lin >= o1lin) ? opp0 : opp1;
Pod bsim = base_pods[blocker_pod];
Action blocker_h[MAX_HORIZON];
for (int t = 0; t < horizon; ++t) {
double tx = base_pods[opp_r].pos.x + base_pods[opp_r].vel.x * (t + 2);
double ty = base_pods[opp_r].pos.y + base_pods[opp_r].vel.y * (t + 2);
blocker_h[t] = MakeGoToTarget(bsim, tx, ty, 200);
bsim.ApplyGAAction(blocker_h[t].angle, blocker_h[t].thrust);
bsim.pos.x += bsim.vel.x; bsim.pos.y += bsim.vel.y;
bsim.vel.x = ga_pure::FreeFlightFrictionVel(bsim.vel.x, kCgFriction); bsim.vel.y = ga_pure::FreeFlightFrictionVel(bsim.vel.y, kCgFriction);
bsim.pos.x = round(bsim.pos.x); bsim.pos.y = round(bsim.pos.y);
}
auto fill_seed = [&](int i, int r_thr_mult, double r_angle_mult, int r_shld, int b_shld) {
if (i >= pop_size) return;
for (int t = 0; t < horizon; ++t) {
pop[i].runner_moves[t] = runner_h[t];
if (r_thr_mult != 200) pop[i].runner_moves[t].thrust = r_thr_mult;
if (r_angle_mult != 1.0) {
pop[i].runner_moves[t].angle *= r_angle_mult;
pop[i].runner_moves[t].angle = max(-18.0, min(18.0, pop[i].runner_moves[t].angle));
}
pop[i].blocker_moves[t] = blocker_h[t];
}
pop[i].runner_shield_step = r_shld;
pop[i].blocker_shield_step = b_shld;
};
auto inject_static = [&](int i, double r_ang, int r_thr, double b_ang, int b_thr) {
if (i >= pop_size) return;
for (int t = 0; t < horizon; ++t) {
pop[i].runner_moves[t].angle = r_ang;
pop[i].runner_moves[t].thrust = r_thr;
pop[i].blocker_moves[t].angle = b_ang;
pop[i].blocker_moves[t].thrust = b_thr;
}
pop[i].runner_shield_step = MAX_HORIZON;
pop[i].blocker_shield_step = MAX_HORIZON;
};
int active_spec = (num_threads > 1) ? thread_id : 0;
if (active_spec == 0) {
fill_seed(idx,     200, 1.0,  MAX_HORIZON, MAX_HORIZON);
fill_seed(idx + 1, 150, 1.0,  MAX_HORIZON, MAX_HORIZON);
fill_seed(idx + 2, 200, 0.5,  MAX_HORIZON, MAX_HORIZON);
fill_seed(idx + 3, 200, 1.3,  MAX_HORIZON, MAX_HORIZON);
fill_seed(idx + 4, 200, 1.0,  MAX_HORIZON, 0);
fill_seed(idx + 5, 200, 1.0,  0,           MAX_HORIZON);
idx += 6;
}
else if (active_spec == 1) {
fill_seed(idx,     200, 1.0,  MAX_HORIZON, MAX_HORIZON);
fill_seed(idx + 1, 200, 0.9,  MAX_HORIZON, MAX_HORIZON);
fill_seed(idx + 2, 200, 1.1,  MAX_HORIZON, MAX_HORIZON);
fill_seed(idx + 3, 200, 1.0,  0,           MAX_HORIZON);
idx += 4;
}
else if (active_spec == 2) {
for (int k = 0; k < 4 && idx < pop_size; ++k) {
Pod bsim_lead = base_pods[blocker_pod];
for (int t = 0; t < horizon; ++t) {
pop[idx].runner_moves[t] = runner_h[t];
double tx = base_pods[opp_r].pos.x + base_pods[opp_r].vel.x * (t + 1 + k);
double ty = base_pods[opp_r].pos.y + base_pods[opp_r].vel.y * (t + 1 + k);
pop[idx].blocker_moves[t] = MakeGoToTarget(bsim_lead, tx, ty, 200);
bsim_lead.ApplyGAAction(pop[idx].blocker_moves[t].angle, 200);
bsim_lead.pos.x += bsim_lead.vel.x; bsim_lead.pos.y += bsim_lead.vel.y;
bsim_lead.vel.x = ga_pure::FreeFlightFrictionVel(bsim_lead.vel.x, kCgFriction); bsim_lead.vel.y = ga_pure::FreeFlightFrictionVel(bsim_lead.vel.y, kCgFriction);
bsim_lead.pos.x = round(bsim_lead.pos.x); bsim_lead.pos.y = round(bsim_lead.pos.y);
}
pop[idx].runner_shield_step = MAX_HORIZON;
pop[idx].blocker_shield_step = MAX_HORIZON;
idx++;
}
}
else if (active_spec == 3) {
const Vec2& rrp = (*ctx.ram_rest_points)[ctx.ram_beacon];
Pod bsim2 = base_pods[blocker_pod];
for (int t = 0; t < horizon; ++t) {
pop[idx].runner_moves[t] = runner_h[t];
pop[idx].blocker_moves[t] = MakeGoToTarget(bsim2, rrp.x, rrp.y, 200);
bsim2.ApplyGAAction(pop[idx].blocker_moves[t].angle, 200);
bsim2.pos.x += bsim2.vel.x; bsim2.pos.y += bsim2.vel.y;
bsim2.vel.x = ga_pure::FreeFlightFrictionVel(bsim2.vel.x, kCgFriction); bsim2.vel.y = ga_pure::FreeFlightFrictionVel(bsim2.vel.y, kCgFriction);
bsim2.pos.x = round(bsim2.pos.x); bsim2.pos.y = round(bsim2.pos.y);
}
pop[idx].runner_shield_step = MAX_HORIZON;
pop[idx].blocker_shield_step = MAX_HORIZON;
idx++;
const Vec2& cp_wait = cps[base_pods[opp_r].next_cp_id];
Pod bsim3 = base_pods[blocker_pod];
for (int t = 0; t < horizon; ++t) {
pop[idx].runner_moves[t] = runner_h[t];
pop[idx].blocker_moves[t] = MakeGoToTarget(bsim3, cp_wait.x, cp_wait.y, 200);
bsim3.ApplyGAAction(pop[idx].blocker_moves[t].angle, 200);
bsim3.pos.x += bsim3.vel.x; bsim3.pos.y += bsim3.vel.y;
bsim3.vel.x = ga_pure::FreeFlightFrictionVel(bsim3.vel.x, kCgFriction); bsim3.vel.y = ga_pure::FreeFlightFrictionVel(bsim3.vel.y, kCgFriction);
bsim3.pos.x = round(bsim3.pos.x); bsim3.pos.y = round(bsim3.pos.y);
}
pop[idx].runner_shield_step = MAX_HORIZON;
pop[idx].blocker_shield_step = MAX_HORIZON;
idx++;
}
else if (active_spec == 4) {
fill_seed(idx,     150, 1.0,  MAX_HORIZON, MAX_HORIZON);
fill_seed(idx + 1, 100, 1.0,  MAX_HORIZON, MAX_HORIZON);
fill_seed(idx + 2,  50, 1.0,  MAX_HORIZON, MAX_HORIZON);
fill_seed(idx + 3,   0, 1.0,  MAX_HORIZON, MAX_HORIZON);
idx += 4;
}
else if (active_spec == 5) {
fill_seed(idx,     200, 1.2,  MAX_HORIZON, MAX_HORIZON);
fill_seed(idx + 1, 200, 1.4,  MAX_HORIZON, MAX_HORIZON);
fill_seed(idx + 2, 200, 0.6,  MAX_HORIZON, MAX_HORIZON);
fill_seed(idx + 3, 200, 1.5,  MAX_HORIZON, MAX_HORIZON);
idx += 4;
}
else if (active_spec == 6) {
Solution best_mc_sol;
double best_mc_score = -1e18;
for (int k = 0; k < 100; ++k) {
Solution sol;
for (int t = 0; t < horizon; ++t) {
sol.runner_moves[t] = runner_h[t];
sol.blocker_moves[t] = blocker_h[t];
}
sol.runner_shield_step = MAX_HORIZON;
sol.blocker_shield_step = MAX_HORIZON;
for (int t = 0; t < horizon; ++t) {
double amp = (t < 3) ? 1.0 : 0.4;
sol.runner_moves[t].MutateAggressive(amp);
sol.blocker_moves[t].MutateAggressive(amp);
}
if (FastRandInt(0, 4) == 0) sol.runner_shield_step = FastRandInt(0, 2);
if (FastRandInt(0, 4) == 0) sol.blocker_shield_step = FastRandInt(0, 2);
double score = SimulateAndEvaluate(sol, base_pods, ctx, horizon);
if (score > best_mc_score) {
best_mc_score = score;
best_mc_sol = sol;
}
}
if (idx < pop_size) {
pop[idx] = best_mc_sol;
idx++;
}
for (int j = 0; j < 3 && idx < pop_size; ++j) {
pop[idx] = best_mc_sol;
for (int t = 0; t < horizon; ++t) {
pop[idx].runner_moves[t].SmallMutate();
pop[idx].blocker_moves[t].SmallMutate();
}
idx++;
}
}
else if (active_spec == 7) {
struct JointAction {
Action runner;
Action blocker;
int r_shld;
int b_shld;
};
JointAction A[8];
Pod rsim_mm = base_pods[runner_pod];
Pod bsim_mm = base_pods[blocker_pod];
const Vec2& my_cp_target = (*ctx.entry_points)[rsim_mm.next_cp_id];
const Vec2& goalie_pt = (*ctx.ram_rest_points)[ctx.ram_beacon];
Action r_racing = MakeGoToTarget(rsim_mm, my_cp_target.x, my_cp_target.y, 200);
Action b_intercept = MakeGoToTarget(bsim_mm, base_pods[opp_r].pos.x + base_pods[opp_r].vel.x * 2.0, base_pods[opp_r].pos.y + base_pods[opp_r].vel.y * 2.0, 200);
Action b_camp = MakeGoToTarget(bsim_mm, goalie_pt.x, goalie_pt.y, 200);
Action b_ram_direct = MakeGoToTarget(bsim_mm, base_pods[opp_r].pos.x, base_pods[opp_r].pos.y, 200);
Action b_boost = MakeGoToTarget(bsim_mm, rsim_pos[0].x, rsim_pos[0].y, 200);
int opp_blocker_pod = ctx.opp_start_idx + (opp_r == ctx.opp_start_idx ? 1 : 0);
double opp_b_dir = atan2(base_pods[opp_blocker_pod].pos.y - rsim_mm.pos.y, base_pods[opp_blocker_pod].pos.x - rsim_mm.pos.x);
double evade_angle = opp_b_dir + 3.14159265358979323846 / 2.0;
Action r_evade = MakeGoToTarget(rsim_mm, rsim_mm.pos.x + cos(evade_angle)*1000.0, rsim_mm.pos.y + sin(evade_angle)*1000.0, 200);
Action r_slingshot = r_racing;
r_slingshot.angle *= 1.4;
r_slingshot.angle = max(-18.0, min(18.0, r_slingshot.angle));
A[0] = {r_racing, b_intercept, MAX_HORIZON, MAX_HORIZON};
Action r_brake = r_racing; r_brake.thrust = 0;
A[1] = {r_brake, b_intercept, MAX_HORIZON, MAX_HORIZON};
A[2] = {r_racing, b_camp, MAX_HORIZON, MAX_HORIZON};
A[3] = {r_racing, b_boost, MAX_HORIZON, MAX_HORIZON};
A[4] = {r_evade, b_intercept, MAX_HORIZON, MAX_HORIZON};
Action r_shld = r_racing; r_shld.thrust = -1;
Action b_shld = b_intercept; b_shld.thrust = -1;
A[5] = {r_shld, b_shld, 0, 0};
A[6] = {r_slingshot, b_intercept, MAX_HORIZON, MAX_HORIZON};
A[7] = {r_racing, b_ram_direct, MAX_HORIZON, MAX_HORIZON};
JointAction B[8];
Pod opp_rsim = base_pods[opp_r];
Pod opp_bsim = base_pods[opp_blocker_pod];
const Vec2& opp_cp_target = cps[opp_rsim.next_cp_id];
Action opp_r_racing = MakeGoToTarget(opp_rsim, opp_cp_target.x, opp_cp_target.y, 200);
Action opp_r_racing_base = (ctx.opp_moves[0].thrust >= 0) ? ctx.opp_moves[0] : opp_r_racing;
Action opp_b_intercept = MakeGoToTarget(opp_bsim, rsim_mm.pos.x + rsim_mm.vel.x * 2.0, rsim_mm.pos.y + rsim_mm.vel.y * 2.0, 200);
Action opp_b_camp = MakeGoToTarget(opp_bsim, goalie_pt.x, goalie_pt.y, 200);
Action opp_b_ram_direct = MakeGoToTarget(opp_bsim, rsim_mm.pos.x, rsim_mm.pos.y, 200);
double my_b_dir = atan2(bsim_mm.pos.y - opp_rsim.pos.y, bsim_mm.pos.x - opp_rsim.pos.x);
double opp_evade_angle = my_b_dir + 3.14159265358979323846 / 2.0;
Action opp_r_evade = MakeGoToTarget(opp_rsim, opp_rsim.pos.x + cos(opp_evade_angle)*1000.0, opp_rsim.pos.y + sin(opp_evade_angle)*1000.0, 200);
Action opp_r_slingshot = opp_r_racing_base;
opp_r_slingshot.angle *= 1.4;
opp_r_slingshot.angle = max(-18.0, min(18.0, opp_r_slingshot.angle));
B[0] = {opp_r_racing_base, opp_b_intercept, MAX_HORIZON, MAX_HORIZON};
Action opp_r_brake = opp_r_racing_base; opp_r_brake.thrust = 0;
B[1] = {opp_r_brake, opp_b_intercept, MAX_HORIZON, MAX_HORIZON};
B[2] = {opp_r_racing_base, opp_b_camp, MAX_HORIZON, MAX_HORIZON};
Action opp_b_boost = MakeGoToTarget(opp_bsim, opp_rsim.pos.x + opp_rsim.vel.x, opp_rsim.pos.y + opp_rsim.vel.y, 200);
B[3] = {opp_r_racing_base, opp_b_boost, MAX_HORIZON, MAX_HORIZON};
B[4] = {opp_r_evade, opp_b_intercept, MAX_HORIZON, MAX_HORIZON};
Action opp_r_shld = opp_r_racing_base; opp_r_shld.thrust = -1;
Action opp_b_shld = opp_b_intercept; opp_b_shld.thrust = -1;
B[5] = {opp_r_shld, opp_b_shld, 0, 0};
B[6] = {opp_r_slingshot, opp_b_intercept, MAX_HORIZON, MAX_HORIZON};
B[7] = {opp_r_racing_base, opp_b_ram_direct, MAX_HORIZON, MAX_HORIZON};
double M_payoff[8][8];
for (int i = 0; i < 8; ++i) {
for (int j = 0; j < 8; ++j) {
Solution sol;
for (int t = 0; t < horizon; ++t) {
sol.runner_moves[t] = runner_h[t];
sol.blocker_moves[t] = blocker_h[t];
}
sol.runner_moves[0] = A[i].runner;
sol.blocker_moves[0] = A[i].blocker;
sol.runner_shield_step = A[i].r_shld;
sol.blocker_shield_step = A[i].b_shld;
SimCtx local_ctx = ctx;
local_ctx.opp_moves[0] = B[j].runner;
local_ctx.has_opp_blocker_override = true;
local_ctx.opp_blocker_override = B[j].blocker;
M_payoff[i][j] = SimulateAndEvaluate(sol, base_pods, local_ctx, horizon);
}
}
int best_i = 0;
double best_minimax_value = -1e18;
for (int i = 0; i < 8; ++i) {
double min_val = 1e18;
for (int j = 0; j < 8; ++j) {
if (M_payoff[i][j] < min_val) {
min_val = M_payoff[i][j];
}
}
if (min_val > best_minimax_value) {
best_minimax_value = min_val;
best_i = i;
}
}
Solution best_mm_sol;
for (int t = 0; t < horizon; ++t) {
best_mm_sol.runner_moves[t] = runner_h[t];
best_mm_sol.blocker_moves[t] = blocker_h[t];
}
best_mm_sol.runner_moves[0] = A[best_i].runner;
best_mm_sol.blocker_moves[0] = A[best_i].blocker;
best_mm_sol.runner_shield_step = A[best_i].r_shld;
best_mm_sol.blocker_shield_step = A[best_i].b_shld;
if (idx < pop_size) {
pop[idx] = best_mm_sol;
idx++;
}
for (int j = 0; j < 3 && idx < pop_size; ++j) {
pop[idx] = best_mm_sol;
for (int t = 0; t < horizon; ++t) {
pop[idx].runner_moves[t].SmallMutate();
pop[idx].blocker_moves[t].SmallMutate();
}
idx++;
}
}
if (idx + 3 < pop_size) {
inject_static(idx,      0.0, 200,   0.0, 200);
inject_static(idx + 1,  0.0,   0,   0.0,   0);
inject_static(idx + 2, -18.0, 200, -18.0, 200);
inject_static(idx + 3,  18.0, 200,  18.0, 200);
idx += 4;
}
}
for (; idx < pop_size; ++idx)
pop[idx].Randomize(horizon);
int worst_idx = 0, best_idx = 0;
for (int i = 0; i < pop_size; ++i) {
scores[i] = SimulateAndEvaluate(pop[i], base_pods, ctx, horizon);
pop[i].score = scores[i];
if (scores[i] < scores[worst_idx]) worst_idx = i;
if (scores[i] > scores[best_idx]) best_idx = i;
}
double worst_score = scores[worst_idx];
int iterations = 0;
double amplitude = 1.0;
int check_freq = 32;
if (time_limit_ms < 15.0) check_freq = 8;
else if (time_limit_ms >= 500.0) check_freq = 256;
while (true) {
iterations++;
if (GABot::verbose && iterations == check_freq) {
cerr << "    [Thread-" << thread_id << "] OS_TID=" << std::this_thread::get_id()
<< " started concurrent GA search (time_budget=" << time_limit_ms << "ms)" << endl;
}
if (iterations % check_freq == 0) {
if (stop_flag) {
if (thread_id == 0) {
double elapsed = timer.ElapsedMs();
if (elapsed >= time_limit_ms) {
stop_flag->store(true, std::memory_order_relaxed);
break;
}
amplitude = 1.0 - elapsed / time_limit_ms;
if (time_limit_ms - elapsed < 2.0) check_freq = 1;
else if (time_limit_ms - elapsed < 5.0) check_freq = 4;
} else {
if (stop_flag->load(std::memory_order_relaxed)) {
break;
}
}
} else {
double elapsed = timer.ElapsedMs();
if (elapsed >= time_limit_ms) break;
amplitude = 1.0 - elapsed / time_limit_ms;
if (time_limit_ms - elapsed < 2.0) check_freq = 1;
else if (time_limit_ms - elapsed < 5.0) check_freq = 4;
}
}
if (stop_flag && thread_id > 0 && iterations % 8 == 0) {
if (stop_flag->load(std::memory_order_relaxed)) {
break;
}
}
if (shared_migrants && num_threads > 1 && iterations % 128 == 0) {
{
std::lock_guard<std::mutex> lock(shared_migrants[thread_id].mutex);
shared_migrants[thread_id].solution = pop[best_idx];
shared_migrants[thread_id].initialized = true;
}
int neighbor_id = (thread_id + FastRandInt(1, num_threads - 1)) % num_threads;
Solution migrant;
bool got_migrant = false;
{
std::lock_guard<std::mutex> lock(shared_migrants[neighbor_id].mutex);
if (shared_migrants[neighbor_id].initialized) {
migrant = shared_migrants[neighbor_id].solution;
got_migrant = true;
}
}
if (got_migrant) {
pop[worst_idx] = migrant;
scores[worst_idx] = migrant.score;
if (scores[worst_idx] > scores[best_idx]) best_idx = worst_idx;
worst_idx = 0; worst_score = scores[0];
for (int i = 1; i < pop_size; ++i) {
if (scores[i] < worst_score) { worst_idx = i; worst_score = scores[i]; }
}
}
}
if (scores[best_idx] < worst_score + 0.3) {
for (int i = 0; i < pop_size; ++i) {
if (i != best_idx) scores[i] -= 2000.0;
}
worst_score -= 2000.0;
}
int p1 = FastRandInt(0, pop_size - 1);
int p2 = FastRandInt(0, pop_size - 1);
int parent1 = (scores[p1] >= scores[p2]) ? p1 : p2;
if (FastRandInt(0, 4) == 0) {
p1 = FastRandInt(0, pop_size - 1);
p2 = FastRandInt(0, pop_size - 1);
int parent2 = (scores[p1] >= scores[p2]) ? p1 : p2;
pop[worst_idx].CrossoverFromTwo(pop[parent1], pop[parent2], horizon);
} else {
pop[worst_idx].MutateFromOne(pop[parent1], horizon, amplitude);
}
pop[worst_idx].runner_moves[FastRandInt(0, horizon - 1)].SmallMutate();
pop[worst_idx].blocker_moves[FastRandInt(0, horizon - 1)].SmallMutate();
double child_score = SimulateAndEvaluate(pop[worst_idx], base_pods, ctx, horizon);
if (child_score > scores[best_idx]) best_idx = worst_idx;
scores[worst_idx] = child_score;
pop[worst_idx].score = child_score;
if (child_score > worst_score) {
worst_idx = 0; worst_score = scores[0];
for (int i = 1; i < pop_size; ++i) {
if (scores[i] < worst_score) { worst_idx = i; worst_score = scores[i]; }
}
}
}
if (out_iters) *out_iters = iterations;
if (GABot::verbose && config.num_threads <= 1 && thread_id == 0) {
cerr << "  GA iters=" << iterations << " best=" << scores[best_idx] << endl;
}
return pop[best_idx];
}
static double ApplySearchSafetyMs(double time_limit_ms, const BotConfig& config) {
const bool firstish = time_limit_ms >= 500.0;
const double safety = firstish ? config.first_turn_search_safety_ms : config.search_safety_ms;
double extra = (config.num_threads > 1) ? 1.5 : 0.0;
double adjusted = time_limit_ms - safety - extra;
if (adjusted < 1.0) adjusted = 1.0;
return adjusted;
}
static Solution RunGAParallel(const vector<Pod>& base_pods, Timer& timer, double time_limit_ms,
const SimCtx& ctx, const BotConfig& config,
GABotThreadPool* pool,
const Solution* warm_start,
const Solution* intra_warm_start = nullptr) {
const double adjusted_limit = ApplySearchSafetyMs(time_limit_ms, config);
if (config.num_threads <= 1 || !pool) {
return RunGA(base_pods, timer, adjusted_limit, ctx, config, 0, 1, nullptr, nullptr, nullptr, warm_start, intra_warm_start);
}
int num_threads = config.num_threads;
std::vector<MigrationChannel> shared_migrants(num_threads);
std::atomic<bool> stop_flag{false};
std::vector<Solution> thread_results(num_threads);
std::vector<int> thread_results_iters(num_threads, 0);
uint32_t base_seed = FastRand();
std::vector<std::function<void()>> tasks(num_threads);
for (int i = 0; i < num_threads; ++i) {
tasks[i] = [i, num_threads, &base_pods, &timer, adjusted_limit, &ctx, &config,
&shared_migrants, &stop_flag, &thread_results, &thread_results_iters, base_seed, warm_start, intra_warm_start]() {
SeedRand(base_seed + i * 1337 + 1);
int iters = 0;
thread_results[i] = RunGA(base_pods, timer, adjusted_limit, ctx, config,
i, num_threads, shared_migrants.data(), &stop_flag,
&iters, warm_start, intra_warm_start);
thread_results_iters[i] = iters;
};
}
pool->RunParallel(tasks);
Solution best_sol;
double best_score = -1e18;
for (int i = 0; i < num_threads; ++i) {
if (thread_results[i].score > best_score) {
best_score = thread_results[i].score;
best_sol = thread_results[i];
}
}
if (GABot::verbose) {
cerr << "  Parallel GA (" << num_threads << " threads) best=" << best_score << " total_iters=";
int total = 0;
for (int i = 0; i < num_threads; ++i) total += thread_results_iters[i];
cerr << total << " [" << thread_results_iters[0];
for (int i = 1; i < num_threads; ++i) cerr << "," << thread_results_iters[i];
cerr << "]" << endl;
}
return best_sol;
}
bool GABot::verbose = false;
void SetGABotVerbose(bool verbose) { GABot::verbose = verbose; }
GABot::GABot(BotConfig config) : config_(config) {
if (config_.num_threads > 1) {
thread_pool_ = std::make_shared<GABotThreadPool>(config_.num_threads);
}
}
string GABot::GetName() const { return config_.name; }
void GABot::Initialize(int laps, int cp_count, const vector<Vec2>& cps, int team_id) {
laps_ = laps;
cp_count_ = cp_count;
cps_ = cps;
team_id_ = team_id;
has_prev_best_ = false;
total_cps_in_race_ = laps * cp_count;
bool has_sharp_turn = false;
for (int i = 0; i < cp_count; ++i) {
int prev = (i + cp_count - 1) % cp_count;
int next = (i + 1) % cp_count;
Vec2 v1 = cps[i].Sub(cps[prev]);
Vec2 v2 = cps[next].Sub(cps[i]);
double len1 = sqrt(v1.x * v1.x + v1.y * v1.y);
double len2 = sqrt(v2.x * v2.x + v2.y * v2.y);
if (len1 > 0 && len2 > 0) {
double dot = v1.x * v2.x + v1.y * v2.y;
double cos_theta = dot / (len1 * len2);
if (cos_theta < 0.2) {
has_sharp_turn = true;
break;
}
}
}
double total_dist = 0;
for (int i = 0; i < cp_count; ++i) {
total_dist += cps[i].Distance(cps[(i + 1) % cp_count]);
}
avg_dist_ = total_dist / cp_count;
if (config_.name == "DefaultGA") {
bool use_handling = (cp_count >= 5 || has_sharp_turn) && (avg_dist_ <= 6500.0);
if (use_handling) {
config_.name = "Bot_6_Handling";
config_.horizon = 4;
config_.population = 80;
config_.dist_weight = 2.9;
config_.align_weight = 3.7;
config_.speed_bonus = 0.2;
config_.lateral_penalty = 1.0;
config_.angle_penalty = 60.0;
config_.corner_cut_dist = 600.0;
config_.block_weight = 4.3;
config_.shield_penalty = 4.0;
config_.shield_ram_dist = 600.0;
config_.opp_penalty = 1.3;
config_.opp_model_ms = 0.0;
config_.runner_bypass_weight = 25.0;
config_.blocker_stay_in_front_weight = 35.0;
config_.blocker_facing_weight = 35.0;
config_.runner_evasion_weight = 1.5;
} else {
config_.name = "Bot_1_SpeedBlock";
config_.horizon = 4;
config_.population = 80;
config_.dist_weight = 2.3;
config_.align_weight = 1.7;
config_.speed_bonus = 0.6;
config_.lateral_penalty = 1.2;
config_.angle_penalty = 43.0;
config_.corner_cut_dist = 300.0;
config_.block_weight = 6.8;
config_.shield_penalty = 93.0;
config_.shield_ram_dist = 1000.0;
config_.opp_penalty = 1.2;
config_.opp_model_ms = 0.0;
config_.runner_bypass_weight = 15.0;
config_.blocker_stay_in_front_weight = 25.0;
config_.blocker_facing_weight = 25.0;
config_.runner_evasion_weight = 1.0;
}
}
cp_distances_.resize(cp_count);
for (int i = 0; i < cp_count; i++) {
int next = (i + 1) % cp_count;
cp_distances_[i] = cps[i].Distance(cps[next]);
}
entry_points_.resize(cp_count);
for (int i = 0; i < cp_count; ++i) {
int prev = (i + cp_count - 1) % cp_count;
int next = (i + 1) % cp_count;
Vec2 v1 = cps[i].Sub(cps[prev]);
Vec2 v2 = cps[next].Sub(cps[i]);
double len1 = sqrt(v1.x * v1.x + v1.y * v1.y);
double len2 = sqrt(v2.x * v2.x + v2.y * v2.y);
double cos_theta = 1.0;
if (len1 > 0 && len2 > 0) {
double dot = v1.x * v2.x + v1.y * v2.y;
cos_theta = dot / (len1 * len2);
cos_theta = std::max(-1.0, std::min(1.0, cos_theta));
}
double shift_dist = config_.corner_cut_dist * (1.0 - cos_theta) / 2.0;
double dx = cps[prev].x - cps[next].x;
double dy = cps[prev].y - cps[next].y;
double d = sqrt(dx * dx + dy * dy);
if (d > 0) {
entry_points_[i] = {cps[i].x + shift_dist * dx / d, cps[i].y + shift_dist * dy / d};
} else {
entry_points_[i] = cps[i];
}
}
ram_rest_points_.resize(cp_count);
for (int i = 0; i < cp_count; ++i) {
int prev = (i + cp_count - 1) % cp_count;
int next = (i + 1) % cp_count;
double dx = cps[prev].x + cps[next].x - 2.0 * cps[i].x;
double dy = cps[prev].y + cps[next].y - 2.0 * cps[i].y;
double d = sqrt(dx * dx + dy * dy);
if (d > 0) {
ram_rest_points_[i] = {cps[i].x + 1000.0 * dx / d, cps[i].y + 1000.0 * dy / d};
} else {
ram_rest_points_[i] = cps[i];
}
}
dist_to_end_.resize(total_cps_in_race_ + 1, 0.0);
dist_to_end_[total_cps_in_race_ - 1] = 0.0;
for (int i = total_cps_in_race_ - 2; i >= 0; --i) {
int cp = i % cp_count;
int next_cp = (cp + 1) % cp_count;
dist_to_end_[i] = dist_to_end_[i + 1] + cps[cp].Distance(cps[next_cp]);
}
}
static Solution GenerateHeuristicOpponentModel(const vector<Pod>& base_pods, const SimCtx& ctx, int horizon) {
Solution sol;
Pod opp_sim[4];
std::memcpy(opp_sim, base_pods.data(), 4 * sizeof(Pod));
int opp_run_idx = ctx.opp_start_idx;
int opp_block_idx = ctx.opp_start_idx + 1;
const auto& cps = *ctx.cps;
int opp0_lin = opp_sim[opp_run_idx].laps_completed * ctx.cp_count + opp_sim[opp_run_idx].next_cp_id;
int opp1_lin = opp_sim[opp_block_idx].laps_completed * ctx.cp_count + opp_sim[opp_block_idx].next_cp_id;
double opp0_d = opp_sim[opp_run_idx].pos.Distance(cps[opp_sim[opp_run_idx].next_cp_id]);
double opp1_d = opp_sim[opp_block_idx].pos.Distance(cps[opp_sim[opp_block_idx].next_cp_id]);
int opp_runner = opp_run_idx;
if (opp1_lin > opp0_lin || (opp0_lin == opp1_lin && opp1_d < opp0_d)) {
opp_runner = opp_block_idx;
}
sol.runner_shield_step = MAX_HORIZON;
sol.blocker_shield_step = MAX_HORIZON;
for (int t = 0; t < horizon; ++t) {
Vec2 cp_target = cps[opp_sim[opp_runner].next_cp_id];
double dx_r = cp_target.x - opp_sim[opp_runner].pos.x;
double dy_r = cp_target.y - opp_sim[opp_runner].pos.y;
double target_angle_r = GameEngine::RadToDeg(atan2(dy_r, dx_r));
double shift_r = GameEngine::ShortestAngleDiff(opp_sim[opp_runner].angle, target_angle_r);
shift_r = std::max(-18.0, std::min(18.0, shift_r));
int thrust_r = 200;
sol.runner_moves[t] = {shift_r, thrust_r};
opp_sim[opp_runner].ApplyGAAction(shift_r, thrust_r);
opp_sim[opp_runner].pos.x += opp_sim[opp_runner].vel.x;
opp_sim[opp_runner].pos.y += opp_sim[opp_runner].vel.y;
opp_sim[opp_runner].vel.x = ga_pure::FreeFlightFrictionVel(opp_sim[opp_runner].vel.x, kCgFriction);
opp_sim[opp_runner].vel.y = ga_pure::FreeFlightFrictionVel(opp_sim[opp_runner].vel.y, kCgFriction);
opp_sim[opp_runner].pos.x = round(opp_sim[opp_runner].pos.x);
opp_sim[opp_runner].pos.y = round(opp_sim[opp_runner].pos.y);
if (opp_sim[opp_runner].pos.DistanceSq(cps[opp_sim[opp_runner].next_cp_id]) <= 360000) {
opp_sim[opp_runner].next_cp_id = (opp_sim[opp_runner].next_cp_id + 1) % ctx.cp_count;
}
}
return sol;
}
vector<PodAction> GABot::GetActions(const vector<Pod>& pods) {
Timer timer;
timer.Start();
ResetSimTurnCount();
int start_idx = team_id_ * 2;
int opp_start_idx = (1 - team_id_) * 2;
turn_count_++;
auto race_remaining = [&](int idx) {
int lin = pods[idx].laps_completed * cp_count_ + pods[idx].next_cp_id;
return dist_to_end_[lin] + pods[idx].pos.Distance(entry_points_[pods[idx].next_cp_id]);
};
double r0 = race_remaining(start_idx);
double r1 = race_remaining(start_idx + 1);
if (r1 < r0 - 200.0) {
runner_idx_ = 1; blocker_idx_ = 0;
} else {
runner_idx_ = 0; blocker_idx_ = 1;
}
int opp0 = opp_start_idx, opp1 = opp_start_idx + 1;
int n = cp_count_;
int o0lin = pods[opp0].laps_completed * n + pods[opp0].next_cp_id;
int o1lin = pods[opp1].laps_completed * n + pods[opp1].next_cp_id;
double o0d = pods[opp0].pos.Distance(cps_[pods[opp0].next_cp_id]);
double o1d = pods[opp1].pos.Distance(cps_[pods[opp1].next_cp_id]);
int opp_runner_pod = (o0lin > o1lin || (o0lin == o1lin && o0d < o1d)) ? opp0 : opp1;
int blocker_pod_idx = start_idx + blocker_idx_;
int ram_beacon = pods[opp_runner_pod].next_cp_id;
{
double opp_dist = pods[opp_runner_pod].pos.Distance(cps_[pods[opp_runner_pod].next_cp_id]);
double my_dist = pods[blocker_pod_idx].pos.Distance(cps_[pods[opp_runner_pod].next_cp_id]);
if (my_dist > opp_dist + 2200) {
int next_beacon = (ram_beacon + 1) % n;
double opp_dist2 = opp_dist + cps_[ram_beacon].Distance(cps_[next_beacon]);
double my_dist2 = pods[blocker_pod_idx].pos.Distance(cps_[next_beacon]);
if (my_dist2 < opp_dist2 - 2200)
ram_beacon = next_beacon;
else
ram_beacon = (next_beacon + 1) % n;
}
}
const Pod& opp_runner = pods[opp_runner_pod];
bool opp_force_boost = false;
if (opp_runner.boost_available) {
double dist = opp_runner.pos.Distance(entry_points_[opp_runner.next_cp_id]);
double target_angle = GameEngine::RadToDeg(std::atan2(entry_points_[opp_runner.next_cp_id].y - opp_runner.pos.y,
entry_points_[opp_runner.next_cp_id].x - opp_runner.pos.x));
double diff = std::abs(GameEngine::ShortestAngleDiff(opp_runner.angle, target_angle));
if (dist > 5000.0 && diff < 5.0) {
opp_force_boost = true;
}
}
SimCtx opp_ctx;
opp_ctx.cps = &cps_;
opp_ctx.dist_to_end = &dist_to_end_;
opp_ctx.entry_points = &entry_points_;
opp_ctx.ram_rest_points = &ram_rest_points_;
opp_ctx.cp_count = cp_count_;
opp_ctx.laps = laps_;
opp_ctx.start_idx = opp_start_idx;
opp_ctx.opp_start_idx = start_idx;
opp_ctx.runner_idx = opp_runner_pod - opp_start_idx;
opp_ctx.ram_beacon = pods[start_idx + runner_idx_].next_cp_id;
opp_ctx.risk_timeout = false;
opp_ctx.force_boost = opp_force_boost;
BotConfig opp_config = config_;
opp_config.block_weight = 0.0;
opp_config.opp_penalty = 0.0;
opp_config.population = 32;
opp_config.horizon = 4;
opp_config.num_threads = 1;
opp_ctx.config = &opp_config;
{
Pod rsim = pods[start_idx + runner_idx_];
for (int t = 0; t < MAX_HORIZON; ++t) {
const Vec2& tgt = entry_points_[rsim.next_cp_id];
opp_ctx.opp_moves[t] = MakeGoToTarget(rsim, tgt.x, tgt.y, 200);
rsim.ApplyGAAction(opp_ctx.opp_moves[t].angle, opp_ctx.opp_moves[t].thrust);
rsim.pos.x += rsim.vel.x; rsim.pos.y += rsim.vel.y;
rsim.vel.x = ga_pure::FreeFlightFrictionVel(rsim.vel.x, kCgFriction); rsim.vel.y = ga_pure::FreeFlightFrictionVel(rsim.vel.y, kCgFriction);
rsim.pos.x = round(rsim.pos.x); rsim.pos.y = round(rsim.pos.y);
if (rsim.pos.DistanceSq(cps_[rsim.next_cp_id]) <= 360000) {
rsim.next_cp_id++;
if (rsim.next_cp_id >= cp_count_) rsim.next_cp_id = 0;
}
}
}
const bool is_first_turn = (turn_count_ == 1);
const double hard_limit = is_first_turn ? config_.first_turn_time_limit_ms
: config_.turn_time_limit_ms;
const double search_deadline = GaSearchDeadlineMs(
hard_limit, is_first_turn,
config_.turn_tail_reserve_ms, config_.first_turn_tail_reserve_ms);
const double total_budget = search_deadline;
bool tight_budget = (hard_limit < 15.0);
if (hard_limit < 10.0) {
config_.population = 12;
config_.horizon = 3;
opp_config.population = 8;
opp_config.horizon = 3;
} else if (tight_budget) {
config_.population = 16;
config_.horizon = 4;
opp_config.population = 12;
opp_config.horizon = 3;
} else if (hard_limit < 40.0) {
config_.population = std::min(config_.population, 28);
config_.horizon = std::min(config_.horizon, 5);
opp_config.population = std::min(opp_config.population, 16);
opp_config.horizon = std::min(opp_config.horizon, 4);
}
double t0_limit, t1_limit, t2_limit, time_limit;
if (is_first_turn) {
t0_limit = total_budget * 0.10;
t1_limit = total_budget * 0.40;
t2_limit = total_budget * 0.50;
time_limit = total_budget;
} else if (tight_budget) {
t0_limit = total_budget * 0.25;
t1_limit = total_budget * 0.50;
t2_limit = total_budget * 0.50;
time_limit = total_budget;
} else {
t0_limit = total_budget * 0.13;
t1_limit = total_budget * 0.46;
t2_limit = total_budget * 0.60;
time_limit = total_budget;
}
bool force_boost = false;
int runner_pod_idx = start_idx + runner_idx_;
const Pod& runner_pod = pods[runner_pod_idx];
if (runner_pod.boost_available) {
double dist = runner_pod.pos.Distance(entry_points_[runner_pod.next_cp_id]);
double target_angle = GameEngine::RadToDeg(std::atan2(entry_points_[runner_pod.next_cp_id].y - runner_pod.pos.y,
entry_points_[runner_pod.next_cp_id].x - runner_pod.pos.x));
double diff = std::abs(GameEngine::ShortestAngleDiff(runner_pod.angle, target_angle));
if (dist > 5000.0 && diff < 5.0) {
force_boost = true;
}
}
SimCtx ctx;
ctx.cps = &cps_;
ctx.dist_to_end = &dist_to_end_;
ctx.entry_points = &entry_points_;
ctx.ram_rest_points = &ram_rest_points_;
ctx.cp_count = cp_count_;
ctx.laps = laps_;
ctx.start_idx = start_idx;
ctx.opp_start_idx = opp_start_idx;
ctx.runner_idx = runner_idx_;
ctx.ram_beacon = ram_beacon;
ctx.risk_timeout = pods[start_idx + blocker_idx_].timeout >= 60;
ctx.force_boost = force_boost;
ctx.config = &config_;
if (!tight_budget) {
if (turn_count_ == 1) {
config_.horizon = 8;
} else {
double opp_dist = pods[start_idx + blocker_idx_].pos.Distance(opp_runner.pos);
if (opp_dist < 3000.0) {
config_.horizon = 8;
} else {
config_.horizon = 6;
}
}
}
auto stage_cap = [&](double abs_limit) {
return std::min(abs_limit, search_deadline);
};
Solution opp_sol;
if (tight_budget) {
opp_sol = GenerateHeuristicOpponentModel(pods, ctx, config_.horizon);
for (int t = 0; t < MAX_HORIZON; ++t) {
ctx.opp_moves[t] = opp_sol.runner_moves[t];
}
} else {
opp_sol = RunGAParallel(pods, timer, stage_cap(t0_limit), opp_ctx, opp_config, thread_pool_.get(), nullptr);
for (int t = 0; t < MAX_HORIZON; ++t) {
ctx.opp_moves[t] = opp_sol.runner_moves[t];
}
}
Solution baseline_sol;
if (!tight_budget) {
baseline_sol = RunGAParallel(pods, timer, stage_cap(t1_limit), ctx, config_, thread_pool_.get(), has_prev_best_ ? &prev_best_ : nullptr);
Solution adaptive_opp_sol;
for (int t = 0; t < MAX_HORIZON; ++t) {
opp_ctx.opp_moves[t] = baseline_sol.runner_moves[t];
}
adaptive_opp_sol = RunGAParallel(pods, timer, stage_cap(t2_limit), opp_ctx, opp_config, thread_pool_.get(), nullptr, &opp_sol);
for (int t = 0; t < MAX_HORIZON; ++t) {
ctx.opp_moves[t] = adaptive_opp_sol.runner_moves[t];
}
}
Solution best = RunGAParallel(pods, timer, stage_cap(time_limit), ctx, config_, thread_pool_.get(),
has_prev_best_ ? &prev_best_ : nullptr,
tight_budget ? nullptr : &baseline_sol);
prev_best_ = best;
has_prev_best_ = true;
const bool run_hybrid = (timer.ElapsedMs() + 1.5) < hard_limit;
TeamAction chosen{best.runner_moves[0], best.blocker_moves[0], best.runner_shield_step, best.blocker_shield_step, "GA"};
double max_weight = 0.0;
int T = 1000;
if (run_hybrid) {
std::vector<TeamAction> my_actions;
my_actions.push_back({best.runner_moves[0], best.blocker_moves[0], best.runner_shield_step, best.blocker_shield_step, "GA"});
{
Pod rsim = pods[start_idx + runner_idx_];
const Vec2& ep_tgt = entry_points_[rsim.next_cp_id];
Action r_act = MakeGoToTarget(rsim, ep_tgt.x, ep_tgt.y, 200);
Pod bsim = pods[start_idx + blocker_idx_];
const Vec2& bp_tgt = ram_rest_points_[ram_beacon];
Action b_act = MakeGoToTarget(bsim, bp_tgt.x, bp_tgt.y, 200);
my_actions.push_back({r_act, b_act, MAX_HORIZON, MAX_HORIZON, "Race_Block"});
}
{
Pod rsim = pods[start_idx + runner_idx_];
const Vec2& ep_tgt = entry_points_[rsim.next_cp_id];
Action r_act = MakeGoToTarget(rsim, ep_tgt.x, ep_tgt.y, 200);
Pod bsim = pods[start_idx + blocker_idx_];
double tx = opp_runner.pos.x + opp_runner.vel.x * 2.0;
double ty = opp_runner.pos.y + opp_runner.vel.y * 2.0;
Action b_act = MakeGoToTarget(bsim, tx, ty, 200);
my_actions.push_back({r_act, b_act, MAX_HORIZON, MAX_HORIZON, "Race_Ram"});
}
{
Pod rsim = pods[start_idx + runner_idx_];
const Vec2& ep_tgt = entry_points_[rsim.next_cp_id];
Action r_act = MakeGoToTarget(rsim, ep_tgt.x, ep_tgt.y, 200);
Pod bsim = pods[start_idx + blocker_idx_];
double tx = opp_runner.pos.x + opp_runner.vel.x;
double ty = opp_runner.pos.y + opp_runner.vel.y;
Action b_act = MakeGoToTarget(bsim, tx, ty, 0);
my_actions.push_back({r_act, b_act, MAX_HORIZON, 0, "Race_Shield"});
}
{
Pod rsim = pods[start_idx + runner_idx_];
const Vec2& ep_tgt = entry_points_[rsim.next_cp_id];
Action r_act = MakeGoToTarget(rsim, ep_tgt.x, ep_tgt.y, 0);
Pod bsim = pods[start_idx + blocker_idx_];
const Vec2& bp_tgt = ram_rest_points_[ram_beacon];
Action b_act = MakeGoToTarget(bsim, bp_tgt.x, bp_tgt.y, 200);
my_actions.push_back({r_act, b_act, 0, MAX_HORIZON, "Shield_Block"});
}
{
Pod rsim = pods[start_idx + runner_idx_];
const Vec2& ep_tgt = entry_points_[rsim.next_cp_id];
Action r_act = MakeGoToTarget(rsim, ep_tgt.x, ep_tgt.y, 0);
Pod bsim = pods[start_idx + blocker_idx_];
const Vec2& bp_tgt = ram_rest_points_[ram_beacon];
Action b_act = MakeGoToTarget(bsim, bp_tgt.x, bp_tgt.y, 200);
my_actions.push_back({r_act, b_act, MAX_HORIZON, MAX_HORIZON, "Brake_Block"});
}
std::vector<TeamAction> opp_actions;
opp_actions.push_back({opp_sol.runner_moves[0], opp_sol.blocker_moves[0], opp_sol.runner_shield_step, opp_sol.blocker_shield_step, "OppGA"});
{
Pod rsim = pods[opp_runner_pod];
const Vec2& ep_tgt = entry_points_[rsim.next_cp_id];
Action r_act = MakeGoToTarget(rsim, ep_tgt.x, ep_tgt.y, 200);
Pod bsim = pods[opp_start_idx + (1 - (opp_runner_pod - opp_start_idx))];
const Vec2& bp_tgt = ram_rest_points_[pods[start_idx + runner_idx_].next_cp_id];
Action b_act = MakeGoToTarget(bsim, bp_tgt.x, bp_tgt.y, 200);
opp_actions.push_back({r_act, b_act, MAX_HORIZON, MAX_HORIZON, "Race_Block"});
}
{
Pod rsim = pods[opp_runner_pod];
const Vec2& ep_tgt = entry_points_[rsim.next_cp_id];
Action r_act = MakeGoToTarget(rsim, ep_tgt.x, ep_tgt.y, 200);
Pod bsim = pods[opp_start_idx + (1 - (opp_runner_pod - opp_start_idx))];
double tx = pods[start_idx + runner_idx_].pos.x + pods[start_idx + runner_idx_].vel.x * 2.0;
double ty = pods[start_idx + runner_idx_].pos.y + pods[start_idx + runner_idx_].vel.y * 2.0;
Action b_act = MakeGoToTarget(bsim, tx, ty, 200);
opp_actions.push_back({r_act, b_act, MAX_HORIZON, MAX_HORIZON, "Race_Ram"});
}
{
Pod rsim = pods[opp_runner_pod];
const Vec2& ep_tgt = entry_points_[rsim.next_cp_id];
Action r_act = MakeGoToTarget(rsim, ep_tgt.x, ep_tgt.y, 200);
Pod bsim = pods[opp_start_idx + (1 - (opp_runner_pod - opp_start_idx))];
double tx = pods[start_idx + runner_idx_].pos.x + pods[start_idx + runner_idx_].vel.x;
double ty = pods[start_idx + runner_idx_].pos.y + pods[start_idx + runner_idx_].vel.y;
Action b_act = MakeGoToTarget(bsim, tx, ty, 0);
opp_actions.push_back({r_act, b_act, MAX_HORIZON, 0, "Race_Shield"});
}
{
Pod rsim = pods[opp_runner_pod];
const Vec2& ep_tgt = entry_points_[rsim.next_cp_id];
Action r_act = MakeGoToTarget(rsim, ep_tgt.x, ep_tgt.y, 0);
Pod bsim = pods[opp_start_idx + (1 - (opp_runner_pod - opp_start_idx))];
const Vec2& bp_tgt = ram_rest_points_[pods[start_idx + runner_idx_].next_cp_id];
Action b_act = MakeGoToTarget(bsim, bp_tgt.x, bp_tgt.y, 200);
opp_actions.push_back({r_act, b_act, 0, MAX_HORIZON, "Shield_Block"});
}
{
Pod rsim = pods[opp_runner_pod];
const Vec2& ep_tgt = entry_points_[rsim.next_cp_id];
Action r_act = MakeGoToTarget(rsim, ep_tgt.x, ep_tgt.y, 0);
Pod bsim = pods[opp_start_idx + (1 - (opp_runner_pod - opp_start_idx))];
const Vec2& bp_tgt = ram_rest_points_[pods[start_idx + runner_idx_].next_cp_id];
Action b_act = MakeGoToTarget(bsim, bp_tgt.x, bp_tgt.y, 200);
opp_actions.push_back({r_act, b_act, MAX_HORIZON, MAX_HORIZON, "Brake_Block"});
}
int K = 6;
std::vector<std::vector<double>> M(K, std::vector<double>(K, 0.0));
for (int i = 0; i < K; ++i) {
if (timer.ElapsedMs() + 0.5 >= hard_limit) break;
for (int j = 0; j < K; ++j) {
M[i][j] = EvaluateTacticalCell(my_actions[i], opp_actions[j], pods, ctx);
}
}
std::vector<double> R1(K, 0.0), R2(K, 0.0);
std::vector<double> avg_sigma1(K, 0.0), avg_sigma2(K, 0.0);
for (int step = 0; step < T; ++step) {
if ((step & 63) == 0 && timer.ElapsedMs() + 0.3 >= hard_limit) {
T = step > 0 ? step : 1;
break;
}
std::vector<double> P1(K, 0.0), P2(K, 0.0);
double sum1 = 0.0, sum2 = 0.0;
for (int i = 0; i < K; ++i) {
if (R1[i] > 0.0) { P1[i] = R1[i]; sum1 += R1[i]; }
if (R2[i] > 0.0) { P2[i] = R2[i]; sum2 += R2[i]; }
}
if (sum1 > 0.0) {
for (int i = 0; i < K; ++i) P1[i] /= sum1;
} else {
for (int i = 0; i < K; ++i) P1[i] = 1.0 / K;
}
if (sum2 > 0.0) {
for (int i = 0; i < K; ++i) P2[i] /= sum2;
} else {
for (int i = 0; i < K; ++i) P2[i] = 1.0 / K;
}
for (int i = 0; i < K; ++i) {
avg_sigma1[i] += P1[i];
avg_sigma2[i] += P2[i];
}
std::vector<double> v1(K, 0.0), v2(K, 0.0);
double ev1 = 0.0, ev2 = 0.0;
for (int i = 0; i < K; ++i) {
for (int j = 0; j < K; ++j) {
v1[i] += M[i][j] * P2[j];
v2[j] -= M[i][j] * P1[i];
}
}
for (int i = 0; i < K; ++i) {
ev1 += v1[i] * P1[i];
ev2 += v2[i] * P2[i];
}
for (int i = 0; i < K; ++i) {
R1[i] += v1[i] - ev1;
R2[i] += v2[i] - ev2;
}
}
int best_act_idx = 0;
max_weight = -1.0;
for (int i = 0; i < K; ++i) {
if (avg_sigma1[i] > max_weight) {
max_weight = avg_sigma1[i];
best_act_idx = i;
}
}
chosen = my_actions[best_act_idx];
}
vector<PodAction> actions(2);
auto make_output = [&](const Action& a, int pod_idx, int shield_step) -> PodAction {
const Pod& pod = pods[pod_idx];
int out_thrust = max(0, min(200, a.thrust));
if (pod_idx == start_idx + runner_idx_ && force_boost && pod.boost_available) {
out_thrust = 650;
} else if (shield_step == 0 && pod.shield_cd == 0) {
out_thrust = -1;
}
double shift = max(-18.0, min(18.0, a.angle));
double final_angle = GameEngine::NormalizeAngle(pod.angle + shift);
double rad = final_angle * PI / 180.0;
double tx = pod.pos.x + cos(rad) * 10000.0;
double ty = pod.pos.y + sin(rad) * 10000.0;
return {tx, ty, out_thrust};
};
actions[runner_idx_] = make_output(chosen.runner_move, start_idx + runner_idx_, chosen.runner_shield_step);
actions[blocker_idx_] = make_output(chosen.blocker_move, start_idx + blocker_idx_, chosen.blocker_shield_step);
const double elapsed_ms = timer.ElapsedMs();
const uint64_t sims = GetSimTurnCount();
if (verbose) {
cerr << "[T" << turn_count_ << "] " << fixed << setprecision(1)
<< elapsed_ms << "ms/" << hard_limit << "hard R" << runner_idx_
<< " Chosen: " << chosen.name;
if (run_hybrid && T > 0) cerr << " w=" << max_weight / T;
cerr << " S:" << setprecision(0) << best.score
<< " sims=" << sims
<< " sim/s=" << setprecision(0) << (elapsed_ms > 0.0 ? sims * 1000.0 / elapsed_ms : 0.0)
<< endl;
}
assert(elapsed_ms < hard_limit + 0.25);
return actions;
}
#ifndef CG_BOT_AMALGAM
struct HeuristicBlocker {
static PodAction GetAction(const Pod& blocker, const vector<Pod>& pods, const vector<Vec2>& cps, int opp_start_idx) {
const Pod& opp0 = pods[opp_start_idx];
const Pod& opp1 = pods[opp_start_idx + 1];
const Pod& opp_runner = (opp0.next_cp_id >= opp1.next_cp_id) ? opp0 : opp1;
const Pod& target_opp = (opp0.next_cp_id == opp1.next_cp_id) ?
(opp0.pos.DistanceSq(cps[opp0.next_cp_id]) < opp1.pos.DistanceSq(cps[opp1.next_cp_id]) ? opp0 : opp1)
: opp_runner;
double dist_to_opp = blocker.pos.Distance(target_opp.pos);
Vec2 intercept;
if (dist_to_opp < 1500) {
intercept = target_opp.pos;
intercept.x += target_opp.vel.x * 0.5;
intercept.y += target_opp.vel.y * 0.5;
} else if (dist_to_opp < 4000) {
intercept.x = target_opp.pos.x + target_opp.vel.x * 2.0;
intercept.y = target_opp.pos.y + target_opp.vel.y * 2.0;
} else {
Vec2 cp_target = cps[target_opp.next_cp_id];
double our_dist_to_cp = blocker.pos.Distance(cp_target);
double opp_dist_to_cp = target_opp.pos.Distance(cp_target);
if (our_dist_to_cp > opp_dist_to_cp + 1000) {
int next_cp = (target_opp.next_cp_id + 1) % cps.size();
cp_target = cps[next_cp];
}
intercept = cp_target;
}
int thrust = 200;
if (dist_to_opp < 900 && blocker.shield_cd == 0) {
double rel_vx = blocker.vel.x - target_opp.vel.x;
double rel_vy = blocker.vel.y - target_opp.vel.y;
double dx = target_opp.pos.x - blocker.pos.x;
double dy = target_opp.pos.y - blocker.pos.y;
double closing = (rel_vx * dx + rel_vy * dy);
if (closing > 0 && dist_to_opp < 850) {
return {intercept.x, intercept.y, -1};
}
}
if (dist_to_opp > 5000) {
double our_dist = blocker.pos.Distance(intercept);
if (our_dist < 1500) {
double speed = std::sqrt(blocker.vel.x*blocker.vel.x + blocker.vel.y*blocker.vel.y);
if (speed > 200) thrust = 0;
else thrust = 100;
}
}
return {intercept.x, intercept.y, thrust};
}
};
struct OutputAction {
int tx, ty;
int thrust;
bool was_boost;
bool was_shield;
};
string GetTrollMessage(int pod_id, int runner_idx, int blocker_idx, const vector<Pod>& env, const OutputAction& act, int cp_count) {
int opp_runner = (env[2].laps_completed * cp_count + env[2].next_cp_id > env[3].laps_completed * cp_count + env[3].next_cp_id) ? 2 : 3;
int opp_blocker = 5 - opp_runner;
if (pod_id == runner_idx) {
if (act.was_boost) return "ZOOM! See ya!";
if (act.was_shield) return "SHIELD RAM ENGAGED!";
double dist_to_blocker = env[pod_id].pos.Distance(env[opp_blocker].pos);
if (dist_to_blocker < 1000) {
return "Can't touch this! 🕺";
}
double speed = env[pod_id].vel.Distance({0,0});
if (speed > 350) return "SPEEEED!";
return "Catch me if you can!";
} else {
if (act.was_shield) return "BOOM! Roadblock! 🛑";
double dist_to_opp_run = env[pod_id].pos.Distance(env[opp_runner].pos);
if (dist_to_opp_run < 1200) {
Vec2 blocker_to_opp = env[opp_runner].pos.Sub(env[pod_id].pos);
double dot = blocker_to_opp.x * env[opp_runner].vel.x + blocker_to_opp.y * env[opp_runner].vel.y;
if (dot > 0) return "Behind you! 🎯";
return "YOU SHALL NOT PASS!";
}
return "Camping is a lifestyle 🏕️";
}
}
#endif
#include <memory>
std::unique_ptr<IBot> CreateGABot(const BotConfig& config) {
return std::unique_ptr<IBot>(new GABot(config));
}
std::unique_ptr<IBot> CreateGABot(double time_budget_ms) {
BotConfig config;
const char* env_threads = std::getenv("BOT_THREADS");
if (env_threads) config.num_threads = std::max(1, std::atoi(env_threads));
else config.num_threads = 1;
config.turn_time_limit_ms = time_budget_ms;
config.first_turn_time_limit_ms = std::min(1000.0, time_budget_ms * 20.0);
return CreateGABot(config);
}
uint64_t GetLastSearchSimTurns() { return GetSimTurnCount(); }
#ifndef CG_BOT_NO_MAIN
#ifdef CG_BOT_AMALGAM
int main() {
ios::sync_with_stdio(false);
cin.tie(nullptr);
InitLUT();
int laps = 0;
int cp_count = 0;
if (!(cin >> laps >> cp_count)) return 0;
vector<Vec2> cps(cp_count);
for (int i = 0; i < cp_count; i++) {
cin >> cps[i].x >> cps[i].y;
}
BotConfig config;
config.name = "DefaultGA";
config.horizon = 6;
config.population = 48;
config.num_threads = 1;
config.turn_time_limit_ms = DEFAULT_TURN_TIME_LIMIT_MS;
config.first_turn_time_limit_ms = DEFAULT_FIRST_TURN_TIME_LIMIT_MS;
config.turn_tail_reserve_ms = DEFAULT_TURN_TAIL_RESERVE_MS;
config.first_turn_tail_reserve_ms = DEFAULT_FIRST_TURN_TAIL_RESERVE_MS;
config.search_safety_ms = DEFAULT_SEARCH_SAFETY_MS;
config.first_turn_search_safety_ms = DEFAULT_FIRST_TURN_SEARCH_SAFETY_MS;
GABot bot(config);
bot.Initialize(laps, cp_count, cps, 0);
vector<int> pod_laps(4, 0);
vector<int> prev_cp(4, 1);
int shield_cd_track[4] = {0, 0, 0, 0};
bool boost_available_0 = true;
bool boost_available_1 = true;
while (cin) {
vector<Pod> env(4);
bool ok = true;
for (int i = 0; i < 4; i++) {
int x, y, vx, vy, angle, next_cp_id;
if (!(cin >> x >> y >> vx >> vy >> angle >> next_cp_id)) {
ok = false;
break;
}
env[i].pos = Vec2(x, y);
env[i].vel = Vec2(vx, vy);
env[i].angle = (double)angle;
env[i].next_cp_id = next_cp_id;
env[i].id = i;
env[i].team = i / 2;
if (angle < 0) {
const Vec2& target_cp = cps[next_cp_id];
double rad = std::atan2(target_cp.y - env[i].pos.y, target_cp.x - env[i].pos.x);
env[i].angle = rad * 180.0 / PI;
if (env[i].angle < 0) env[i].angle += 360.0;
}
env[i].shield_cd = shield_cd_track[i];
if (shield_cd_track[i] > 0) shield_cd_track[i]--;
}
if (!ok) break;
env[0].boost_available = boost_available_0;
env[1].boost_available = boost_available_1;
env[2].boost_available = true;
env[3].boost_available = true;
for (int i = 0; i < 4; i++) {
if (env[i].next_cp_id == 1 && prev_cp[i] == cp_count - 1) pod_laps[i]++;
if (env[i].next_cp_id == 0 && prev_cp[i] == cp_count - 1) {
}
prev_cp[i] = env[i].next_cp_id;
env[i].laps_completed = pod_laps[i];
}
vector<PodAction> actions = bot.GetActions(env);
for (int i = 0; i < 2; i++) {
int out_thrust = actions[i].thrust;
const int tx = (int)actions[i].tx;
const int ty = (int)actions[i].ty;
if (out_thrust == 650) {
if (i == 0) boost_available_0 = false;
else boost_available_1 = false;
cout << tx << " " << ty << " BOOST\n";
shield_cd_track[i] = 0;
} else if (out_thrust == -1) {
cout << tx << " " << ty << " SHIELD\n";
shield_cd_track[i] = 4;
} else {
if (out_thrust < 0) out_thrust = 0;
if (out_thrust > 200) out_thrust = 200;
cout << tx << " " << ty << " " << out_thrust << "\n";
}
}
cout << flush;
}
return 0;
}
#else
int main() {
InitLUT();
cerr << "Hardware Concurrency (CPU Cores): " << std::thread::hardware_concurrency() << endl;
int laps;
cin >> laps; cin.ignore();
int cp_count;
cin >> cp_count; cin.ignore();
vector<Vec2> cps(cp_count);
for (int i = 0; i < cp_count; i++) {
cin >> cps[i].x >> cps[i].y; cin.ignore();
}
cerr << "Laps: " << laps << endl;
cerr << "Checkpoint Count: " << cp_count << endl;
for (int i = 0; i < cp_count; i++) {
cerr << "CP[" << i << "] = ("
<< cps[i].x << ", "
<< cps[i].y << ")" << endl;
}
BotConfig config;
config.name = "DefaultGA";
config.horizon = 6;
config.population = 48;
config.dist_weight = 1.5;
config.align_weight = 3.0;
config.speed_bonus = 0.5;
config.lateral_penalty = 0.5;
config.angle_penalty = 25;
config.corner_cut_dist = 300;
config.block_weight = 1.0;
config.shield_penalty = 50;
config.shield_ram_dist = 850;
config.opp_penalty = 0.5;
config.opp_model_ms = 0;
config.turn_time_limit_ms = DEFAULT_TURN_TIME_LIMIT_MS;
config.first_turn_time_limit_ms = DEFAULT_FIRST_TURN_TIME_LIMIT_MS;
config.turn_tail_reserve_ms = DEFAULT_TURN_TAIL_RESERVE_MS;
config.first_turn_tail_reserve_ms = DEFAULT_FIRST_TURN_TAIL_RESERVE_MS;
config.search_safety_ms = DEFAULT_SEARCH_SAFETY_MS;
config.first_turn_search_safety_ms = DEFAULT_FIRST_TURN_SEARCH_SAFETY_MS;
config.num_threads = 1;
GABot bot(config);
GABot::verbose = true;
bot.Initialize(laps, cp_count, cps, 0);
vector<int> pod_laps(4, 0);
vector<int> prev_cp(4, 1);
bool has_prediction = false;
vector<Pod> predicted(4);
vector<OutputAction> last_output(2);
bool pred_had_opp_collision[2] = {false, false};
double max_pos_err_clean = 0, max_vel_err_clean = 0;
int verify_turn = 0;
int shield_cd_track[4] = {0, 0, 0, 0};
bool boost_available_0 = true;
bool boost_available_1 = true;
while (1) {
vector<Pod> env(4);
for (int i = 0; i < 4; i++) {
int x, y, vx, vy, angle, next_cp_id;
cin >> x >> y >> vx >> vy >> angle >> next_cp_id; cin.ignore();
env[i].pos = Vec2(x, y);
env[i].vel = Vec2(vx, vy);
env[i].angle = (double)angle;
env[i].next_cp_id = next_cp_id;
if (angle < 0) {
Vec2 target_cp = cps[next_cp_id];
double rad = std::atan2(target_cp.y - env[i].pos.y, target_cp.x - env[i].pos.x);
env[i].angle = rad * 180.0 / PI;
if (env[i].angle < 0) env[i].angle += 360.0;
}
env[i].id = i;
env[i].shield_cd = shield_cd_track[i];
if (shield_cd_track[i] > 0) shield_cd_track[i]--;
}
env[0].boost_available = boost_available_0;
env[1].boost_available = boost_available_1;
for (int i = 0; i < 4; i++) {
if (env[i].next_cp_id == 1 && prev_cp[i] == cp_count - 1) pod_laps[i]++;
prev_cp[i] = env[i].next_cp_id;
env[i].laps_completed = pod_laps[i];
}
if (has_prediction) {
verify_turn++;
cerr << "--- PHYSICS VERIFY T" << verify_turn << " ---" << endl;
bool any_our_bug = false;
for (int i = 0; i < 2; i++) {
double pos_dx = env[i].pos.x - predicted[i].pos.x;
double pos_dy = env[i].pos.y - predicted[i].pos.y;
double pos_err = std::sqrt(pos_dx*pos_dx + pos_dy*pos_dy);
double vel_dx = env[i].vel.x - predicted[i].vel.x;
double vel_dy = env[i].vel.y - predicted[i].vel.y;
double vel_err = std::sqrt(vel_dx*vel_dx + vel_dy*vel_dy);
double angle_err = 0;
if (env[i].angle >= 0 && predicted[i].angle >= 0) {
angle_err = std::abs(GameEngine::ShortestAngleDiff((int)env[i].angle, (int)std::round(predicted[i].angle)));
}
bool had_opp_col = pred_had_opp_collision[i];
if (pos_err > 0.5 || vel_err > 0.5 || angle_err > 0.5) {
if (had_opp_col) {
cerr << "  Pod" << i << " COLLISION-EXPECTED: pos_err=" << fixed << setprecision(1)
<< pos_err << " vel_err=" << vel_err << endl;
} else {
any_our_bug = true;
cerr << "  *** Pod" << i << " PHYSICS BUG ***:" << endl;
cerr << "    Pos: predicted(" << (int)predicted[i].pos.x << "," << (int)predicted[i].pos.y
<< ") actual(" << (int)env[i].pos.x << "," << (int)env[i].pos.y
<< ") err=" << pos_err << endl;
cerr << "    Vel: predicted(" << (int)predicted[i].vel.x << "," << (int)predicted[i].vel.y
<< ") actual(" << (int)env[i].vel.x << "," << (int)env[i].vel.y
<< ") err=" << vel_err << endl;
if (angle_err > 0) {
cerr << "    Angle: predicted=" << (int)predicted[i].angle
<< " actual=" << (int)env[i].angle
<< " err=" << angle_err << endl;
}
max_pos_err_clean = std::max(max_pos_err_clean, pos_err);
max_vel_err_clean = std::max(max_vel_err_clean, vel_err);
}
}
}
if (!any_our_bug) {
cerr << "  OUR PODS OK (max_clean_pos=" << fixed << setprecision(1) << max_pos_err_clean
<< " max_clean_vel=" << max_vel_err_clean << ")" << endl;
}
}
cerr << "--- STATE DUMP ---" << endl;
for (int i = 0; i < 4; i++) {
cerr << "Pod " << i << ": Pos(" << env[i].pos.x << ", " << env[i].pos.y
<< ") Vel(" << env[i].vel.x << ", " << env[i].vel.y
<< ") Angle: " << env[i].angle << " NextCP: " << env[i].next_cp_id << endl;
}
vector<PodAction> actions = bot.GetActions(env);
int runner_idx = 0;
int blocker_idx = 1;
double score0 = pod_laps[0] * 50000 + env[0].next_cp_id * 1000 - env[0].pos.Distance(cps[env[0].next_cp_id]);
double score1 = pod_laps[1] * 50000 + env[1].next_cp_id * 1000 - env[1].pos.Distance(cps[env[1].next_cp_id]);
if (score1 > score0 + 1500) {
runner_idx = 1;
blocker_idx = 0;
}
for (int i = 0; i < 2; i++) {
bool use_boost = (actions[i].thrust == 650);
if (use_boost) {
if (i == 0) boost_available_0 = false;
else boost_available_1 = false;
}
int out_thrust = actions[i].thrust;
if (env[i].angle < 0 && out_thrust != -1) {
out_thrust = 200;
}
last_output[i].tx = (int)actions[i].tx;
last_output[i].ty = (int)actions[i].ty;
last_output[i].was_boost = use_boost;
last_output[i].was_shield = (out_thrust == -1);
last_output[i].thrust = use_boost ? 650 : out_thrust;
string msg = GetTrollMessage(i, runner_idx, blocker_idx, env, last_output[i], cp_count);
if (use_boost) {
cout << last_output[i].tx << " " << last_output[i].ty << " BOOST " << msg << endl;
shield_cd_track[i] = 0;
} else if (out_thrust == -1) {
cout << last_output[i].tx << " " << last_output[i].ty << " SHIELD " << msg << endl;
shield_cd_track[i] = 4;
} else {
cout << last_output[i].tx << " " << last_output[i].ty << " " << out_thrust << " " << msg << endl;
}
}
predicted = env;
for (int i = 0; i < 2; i++) {
if (last_output[i].was_shield) {
predicted[i].shield_cd = 4;
}
}
{
int thrust0 = last_output[0].thrust;
if (last_output[0].was_shield) thrust0 = -1;
predicted[0].ApplyServerAction((double)last_output[0].tx, (double)last_output[0].ty, thrust0);
}
{
int thrust1 = last_output[1].thrust;
if (last_output[1].was_shield) thrust1 = -1;
predicted[1].ApplyServerAction((double)last_output[1].tx, (double)last_output[1].ty, thrust1);
}
Evolution::ApplyBasicProxy(predicted[2], cps);
Evolution::ApplyBasicProxy(predicted[3], cps);
pred_had_opp_collision[0] = false;
pred_had_opp_collision[1] = false;
SearchSimulateTurn(predicted.data());
for (int our = 0; our < 2; our++) {
for (int opp = 2; opp < 4; opp++) {
double dx = predicted[our].pos.x - predicted[opp].pos.x;
double dy = predicted[our].pos.y - predicted[opp].pos.y;
double dist = std::sqrt(dx*dx + dy*dy);
if (dist < 1200.0) {
pred_had_opp_collision[our] = true;
}
dx = env[our].pos.x - env[opp].pos.x;
dy = env[our].pos.y - env[opp].pos.y;
dist = std::sqrt(dx*dx + dy*dy);
if (dist < 1600.0) {
pred_had_opp_collision[our] = true;
}
}
}
for (int p = 0; p < 4; p++) {
if (predicted[p].pos.DistanceSq(cps[predicted[p].next_cp_id]) <= 360000) {
predicted[p].next_cp_id = (predicted[p].next_cp_id + 1) % cp_count;
}
}
has_prediction = true;
}
}
#endif
#endif

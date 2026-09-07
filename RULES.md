# Rules (Fidelity)

Numeric law (radii, thrust, BOOST, rotate, friction, timeout, max turns,
impulse, shield) is taken from `referee/core/constants.h` or from
`referee/physics/physics.h` / `fidelity_math.h` / `fidelity_world_step.h`.
Playfield **16000×9000**, checkpoint count **2–8**, and turn budgets
**1000/75 ms** are public contest protocol / marketing, not those headers.
`process_duel` adds **+500 / +150 ms** kill slop on top of the CLI budgets.
The public contest page is secondary. Where it disagrees with the headers,
**this code wins**.

## Match

| | |
|---|---|
| Teams | 2 × **2 pods** (indices 0–1 team 0, 2–3 team 1) |
| Checkpoints / lap | **2–8**, radius **600** |
| Laps | default **3** |
| Playfield (marketing) | 16000 × 9000; pods **may leave** it |
| Win | First team with **one** pod finishing the multi-lap route |
| Lose | Other team wins · team CP-timeout · invalid output · one-sided agent timeout |
| Draw (`process_duel`) | Both agents time out · both teams finish the same turn (`checkWinner` −1) |
| First-turn budget | **1000 ms** |
| Later-turn budget | **75 ms** |
| Max turns | **500** (`kMaxGameTurns`) |

Global checkpoint list = `(track × laps) + [CP0 as finish]`.  
Example: 4 CPs, 3 laps → 13 global targets. Race starts aimed at global index **1**.

## Constants (`csb_constants`)

| Symbol | Value |
|---|---|
| `kPodRadius` | **400** (pair collides at centers ≤ **800**) |
| `kCpRadius` | **600** (segment test is strict `dist² < 600²`; after integer commit, exact `dist² == 360000` also scores) |
| `kMaxThrust` | **200** |
| `kBoostThrust` | **650** (once per pod) |
| `kMaxRotateDeg` | **18** |
| `kFriction` | **0.85** |
| `kMinImpulse` | **120** |
| `kTimeoutLimit` | **100** (team clock) |
| `kShieldTimerActivate` | **4** |
| `kShieldMassFactorFidelity` | **0.1** inverse-mass on the activation frame only |
| `kEpsilon` | `1e-5` |

## Bot I/O (contest protocol)

Startup: `laps`, `checkpointCount`, then that many `x y` pairs.

Each turn, four lines (your two pods, then opponent two):

```
x y vx vy angle_deg nextCheckPointId
```

`angle_deg` is **display degrees** (0 = East, 90 = South). Fidelity stores facing in **radians** and never integer-commits it. First-turn `angle` may be **−1**.

Output: exactly two lines

```
tx ty thrust
```

`thrust` is an integer **0–200**, or `BOOST`, or `SHIELD`.

| Input | Engine (`parseMove` / `applyFidelityMove`) |
|---|---|
| `0`–`200` | Accelerate by that amount after rotate |
| `BOOST` | If unused: thrust **650**, mark used; else **200** |
| `SHIELD` | `shieldtimer = 4`, thrust **0** this turn |
| Integer **&lt; 0 or &gt; 200** | `invalid_input`: **skip rotate and thrust**, no shield |
| Non-numeric non-keyword | same `invalid_input` |
| `tx == x && ty == y` | Skip rotate and thrust (shield already applied if requested) |

One-sided agent timeout is a **match loss**, separate from the 100-turn CP clock.
Both agents timing out is a **draw**. Out-of-range integer thrust is `invalid_input` (skip rotate+thrust), not a platform forfeit.

## One turn (code order)

1. Both players emit two actions.  
2. For each pod: parse → maybe shield/boost → maybe rotate → maybe thrust.  
3. World step: pod–pod collisions in double, then end-turn commit.  
4. Checkpoint tests, both team timeouts **−1**, turn **+1**.

### Rotate

- First successful rotate for that pod: snap to `atan2(ty−y, tx−x)` (**no 18° clamp**).  
- Later: at most **18°**. Exact 18° **does** max-rotate.  
- If `|\Delta| < 18°`, facing becomes the **exact** target angle.

### World step

- Collision scan order matches the Go referee: `i = 3..1`, `j = i−1..0`.  
- Bounce: min impulse **120**; if the projected force is already ≥ 120 it is **doubled**.  
- Shield inverse-mass **0.1** only while `shieldtimer == 4`. Timers 3,2,1 are normal mass but still **no thrust**.  
- End of turn: `vx,vy ← frictionTrunc(v)` (trunc of `v × 0.85` plus the ULP snaps in `fidelity_math.h`); `x,y ← roundHalfUp(p)` (`floor(p + 0.5)`), or `roundHalfUpBounce` if that pod bounced this turn; if `shieldtimer > 0` then `--`.  
- Angle is **not** committed to an integer.

### Checkpoints

Pass is a **segment test** on the free-flight path (restarted after a bounce), strict `dist² < 600²`.  
After the integer commit, a pod sitting on the exact circle (`dist² == 360000`) also scores.  
After any teammate scores a CP, the team timeout is set to `100 + 1` and then decremented, so the **next displayed frame** reads 100.

## Two physics in this archive

| Layer | Where | Role |
|---|---|---|
| **Fidelity** `csb::Game` | `referee/physics/` | Referee / `process_duel` / battle replay oracle |
| **Magus `play()`** | inside `bots/current/searchbot.cpp` | What the SearchBot **simulates while searching** |
| **`csb::fast::SimulateTurn`** | `referee/physics/fast.h` | Degrees collision fragment used by older GA pastes — **not** the Fidelity oracle |

Do not treat Magus `play()` or `csb::fast` as the CG keyframe oracle. The oracle is Fidelity.

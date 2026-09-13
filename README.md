# slate-kit

Local C++ simulation workspace. Build with `./setup.sh --verify`.

## The game

Two teams of **two pods** each race a closed checkpoint loop. Only **one**
pod on a team has to finish. The other may race, block, ram, or shield.

| | |
|---|---|
| Teams | 2 × 2 pods (indices 0–1 team 0, 2–3 team 1) |
| Checkpoints / lap | 2–8 circles, radius **600** |
| Laps | default **3** |
| Playfield (marketing) | 16000 × 9000; pods **may leave** it |
| Win | First team with one pod finishing the multi-lap route |
| Lose | Other team wins · team checkpoint-timeout · bad output · one-sided agent timeout |
| Draw | Both agents time out · both teams finish the same turn |
| First-turn budget | **1000 ms** |
| Later-turn budget | **75 ms** |
| Max turns | **500** |

Global checkpoint list = `(track × laps) + [first checkpoint as finish]`.
Example: 4 checkpoints, 3 laps → 13 global targets. Racing starts aimed at
global index **1**.

Each turn every pod outputs `tx ty thrust`. `thrust` is an integer
**0–200**, or `BOOST` (once per pod, **650**), or `SHIELD` (mass change
on the activation frame, no thrust). Facing may rotate at most **18°**
per turn after the first snap. Velocity is scaled by friction **0.85**
and truncated; positions round-half-up. Pods collide at center distance
**800**. Full turn order, bounce, and I/O edge cases: `RULES.md`.

## Setup

On Ubuntu or Debian:

```bash
./setup.sh
```

That is the whole check. It installs `clang` / `python3` if missing
(`apt-get` as root; `sudo` only when not root), compiles the referee and
the pastes this host can build, then runs `tools/verify.sh` (paste
sha256, battle JSON counts, protocol smoke, RULES vs constants, two
one-game Fidelity smokes). On Linux aarch64 the AVX GA pastes and
the catalog vs `ultimate_h4` smoke are SKIP, and `CXX` is pinned to
`clang++`. `--no-apt` skips the package step. `--verify` is the same as
default. `process_duel` is POSIX. Scores below are **one wall-clock run**
(TIME-based search). They are not a CI pin.

## What is in here

| Path | Why it is here |
|---|---|
| `submission/searchbot.cpp` | Current best contest paste |
| `submission/inoryy.cpp` | inoryy opponent contest paste |
| `submission/frozen_magus.cpp` | Unread Magus B used for generate grids |
| `submission/ga/` | Named historical GA contest pastes |
| `submission/history/` | Last 20 live contest submits, descriptively named |
| `referee/` | Fidelity `Game` headers + `process_duel` |
| `RULES.md` | Fidelity law from those headers, plus how Magus/`fast` differ |
| `battles/fenrir/` | **929** Fenrir JSON (every public replay we could find: live last-battles window + older submits + opponent-window harvest). `IDS.txt` lists the ids. |
| `battles/top5/` | **48** JSON: 12 recent games each for Agade, reCurse, YurkovAS, pb4 (disjoint; Fenrir is under `fenrir/`) |
| `battles/our_recent/` | **85** JSON from the last 32 days: **83** include SamSi; **2** are fenrir vs Agade |
| `battles/LAST_MONTH_IDS.txt` | **6513** numeric battle ids (file also has 2 comment lines) from 2026-08-05 to 2026-09-06. Ids only. |

## Current best bot

`submission/searchbot.cpp`  
sha256 `131bccec663f92583b2f68e80b692728e2de8086cc3fd2ac1317564ea6cbeb8c`

It is Magus SearchBot (`DEPTH=6`, `POP=48`) with Fenrir **emit** rules. It
is **not** Fenrir’s A2C net. Search inside the paste uses Magus `play()`,
not Fidelity. This file is the contest paste. The sha256 is the identity;
do not hand-edit.

The paste header also mentions a **43-replay** Fenrir emit study. That is
a subset used when writing the emit rules. `battles/fenrir/` is every
Fenrir replay we could still reach, not that 43-game set.

Measured on 2026-09-06 on one Mac (clang, `-mcpu=native`):

- Generate **13 maps × 2 sides × 1**, seed **42**, vs frozen Magus B, 1000/75 ms: **24–2, wr=0.923**, no A timeout
- Catalog **18 maps × 2 sides × 1** vs Ultimate H=4: **36–0**
- Same 36-game catalog grid vs nine named GAs: **304–20, wr=0.938** (no draws on that run)

See `submission/README.md` for the per-opponent table. Those 36-game figures are
one repeat, not a 15-repeat 540-game grid.

## Physics

The **oracle** is Fidelity `Game` in `referee/physics/`. Numeric law is
`referee/core/constants.h` (thrust **200**, BOOST **650**, rotate **18°**,
friction **0.85**, pod radius **400**, checkpoint radius **600**, team
timeout **100**, max turns **500**).

`process_duel` referees two protocol binaries on that oracle.
Default `--repeats` is **15**; default `--gen-seed` is **1**. Catalog maps
are the 18 layouts in `referee/core/maps/catalog.h` (`--maps 0-17`).

`./setup.sh` writes `bin/process_duel`, `bin/searchbot`, `bin/frozen_b`,
and every named GA binary. After that:

Catalog 18×2 vs Ultimate H=4 (the GA-panel grid used for the 36–0 figure):

```bash
./bin/process_duel \
  --bot-a ./bin/searchbot \
  --bot-b ./bin/ultimate_h4 \
  --maps 0-17 --sides 0,1 --repeats 1 \
  --first-turn-ms 1000 --time-budget-ms 75 \
  --out catalog.json
```

Generate 13×2 vs frozen Magus B (the 24–2 figure). Default `--gen-seed` is
1, so seed 42 must be passed. This block assumes `./setup.sh` already ran.

`process_duel` waits `first_ms+500` / `later_ms+150` (1500 / 225 with
these flags) before killing a silent bot. That is local slop, not the
contest 1000/75 budget. SearchBot self-limits to 0.94 / 0.074 s.

```bash
./bin/process_duel \
  --bot-a ./bin/searchbot \
  --bot-b ./bin/frozen_b \
  --gen-maps 13 --gen-seed 42 --sides 0,1 --repeats 1 \
  --first-turn-ms 1000 --time-budget-ms 75 \
  --out generate.json
```

The AVX GA pastes (all named `ga/` files except `legacy_amalgam.cpp`)
carry an unguarded `#pragma GCC target("avx2,fma,bmi,bmi2")`. That is
fine on x86_64 clang/g++. Apple clang ignores it (`-Wno-unknown-pragmas`).
aarch64 Linux clang/g++ may reject those pastes. The current SearchBot
paste and frozen Magus B wrap that pragma in `#if !defined(__clang__)`.
`process_duel` runs independent games on every hardware thread (`--jobs`
overrides). Each bot process is one core (`OMP_NUM_THREADS=1`).

`fast::SimulateTurn` is shipped because `physics.h` includes `fast.h`.
It is a **degrees collision fragment** for older GA search, not the
keyframe oracle. Compiling a GA paste “the same way” still referees on
Fidelity; the paste’s **internal** search may use Magus `play()` or its
own `fast` copy.

## Battles

Shipped `.json` files are **responses** from
`POST /services/gameResult/findByGameId` with request body `[gameId, null]`
(keys include `frames`, `gameId`, `agents`, `ranks`). They are not the
request array.

Fenrir’s live last-battles **list** is
`POST /services/gamesPlayersRanking/findLastBattlesByAgentId` with
`[6500255, null]` (~233 games). That list does not paginate. Older Fenrir
submits (`6434652`, `6423325`, …) list 0 on that API; those bodies are
here only because they were still on disk or still sat in an opponent’s
window. All 929 files are `findByGameId` `[id, null]` responses.

JSON bodies live under `battles/fenrir/`, `battles/top5/`, and
`battles/our_recent/`. `LAST_MONTH_IDS.txt` is the id list only.

## What this is not

- Fenrir’s weights or A2C trainer
- A claim that SearchBot search physics equals Fidelity
- The 539–1 catalog figure (that was a **previous** Magus paste, not this file)
- Cookies, session tokens, or committed binaries (`bin/` is local build output)

## Layout

```
submission/searchbot.cpp
submission/inoryy.cpp
submission/frozen_magus.cpp
submission/ga/*.cpp
submission/README.md
referee/{process_duel.cpp,agade_maps.h,core/,physics/}
battles/{fenrir,top5,our_recent,LAST_MONTH_IDS.txt}
setup.sh
tools/{cxx.sh,compile_referee.sh,compile_searchbot.sh,compile_bot.sh,verify.sh,host.sh}
RULES.md
```

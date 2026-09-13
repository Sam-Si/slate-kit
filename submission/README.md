# Contest pastes

Single-file pastes for the online IDE. Sources only. No committed binaries.

## Current best

`searchbot.cpp`

- sha256 `131bccec663f92583b2f68e80b692728e2de8086cc3fd2ac1317564ea6cbeb8c`
- 65249 bytes
- Magus SearchBot (`DEPTH=6`, `POP=48`, first think 0.94 s, later 0.074 s) plus Fenrir **emit** priors (no forced T0 BOOST; scored T0 BOOST on first-leg 5000–8000; later aligned racer BOOST; contact SHIELD if opponent &lt;900 and closing).
- Search rollouts use **Magus `play()`**, not Fidelity.
- Measured 2026-09-06 on this machine:
  - vs unread frozen Magus B, generate 13×2 seed 42, 1000/75 ms: **26g 24–2, wr=0.923**, no A timeout
  - vs `ultimate_h4`, catalog maps 0–17 × both sides × 1: **36–0**
  - vs nine named past GAs on that same 36-game grid: **304–20 / 324, wr=0.938**

This is **not** the older Magus paste that was documented as 539–1 on a 540-game catalog grid.

## Opponents

`frozen_magus.cpp`  
sha256 `a18a3505d3c98e49c8119d4be2a4ff3727df32b5b6ccab2e4fcba66c597e6d01`  
Unread generate-grid B. Not a GA.

`inoryy.cpp`  
inoryy ChampionBot contest paste (GA horizon 6, pop 80). Uses AVX2/`immintrin.h`; skip compile on Linux aarch64 with the other AVX pastes.

## Named GA lineage (`ga/`)

| File | What it is | 36-game catalog wr of *current searchbot* vs it |
|---|---|---:|
| `ultimate_h4.cpp` | UltimateBot H=4 | 1.000 (36–0) |
| `ultimate_opt.cpp` | Ultimate opt sibling | 0.944 (34–2) |
| `recommended.cpp` | Recommended H=5 | 0.944 (34–2) |
| `previous_submission.cpp` | Prior user GA paste | 0.944 (34–2) |
| `st_h8_p64.cpp` | H=8 P=64 | 0.944 (34–2) |
| `self_baseline_h4_p48.cpp` | Self baseline H=4 P=48 | 0.917 (33–3) |
| `h6_p48.cpp` | H=6 P=48 | 0.833 (30–6) |
| `legacy_amalgam.cpp` | Older in-repo GA + `fast` | 1.000 (36–0) |

Those 36-game figures are one repeat per map×side, not the 15-repeat 540-game grid.

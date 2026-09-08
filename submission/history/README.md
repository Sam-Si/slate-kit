# Last 20 contest submissions

Newest first. Each file is `NN_id<id>_<lang>_<what>.<ext>`:

- `NN` is newest-first rank in the last-20 list (`01` = latest paste)
- `id` is the contest `testSessionQuestionSubmissionId`
- `lang` is `cxx` or `python3`
- `what` is the thinker (SearchBot / Ultimate GA / f16 MLP / tinf / PPO / numpy)
  plus the runtime knobs that distinguish near-duplicates (depth, horizon,
  population, turn budget, byte size, MCTS)

`40873000` is a byte-identical duplicate of `40873174` and is not a separate
royale fighter.

C++ pastes compile to `bin/history/h_<id>`. Three `oppnn_stub` pastes need a
host `sincos` shim on Apple clang; the source files are unchanged.
`inoryy.cpp` in the parent folder is a later opponent paste, not one of
these 20.

Python pastes live here but are not Fidelity fighters: `40911934` is a numpy
policy net; `40777737` JIT-compiles embedded C with `-mavx2` and does not
load on Apple Silicon.

Ranking: `RANKING.md` after `python3 tools/history_royale.py`.

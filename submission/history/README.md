# Last 20 contest submissions

Newest first. Each file is `NN_id<id>_<lang>_<what>.<ext>`:

- `NN` is newest-first rank in the last-20 list (`01` = latest paste)
- `id` is the contest `testSessionQuestionSubmissionId`
- `lang` is `cxx` or `python3`
- `what` is the thinker (ChampionBot / SearchBot / Ultimate GA / f16 MLP /
  tinf / PPO / numpy) plus the runtime knobs that distinguish near-duplicates
  (depth, horizon, population, turn budget, byte size, MCTS)

`01` is live contest id `41275613`: inoryy_ga ChampionBot, horizon 7, pop 80,
74.2 ms turn budget, 1 ms MCTS. Exact paste from the IDE.

`40873000` is a byte-identical duplicate of `40873174` and is not a separate
royale fighter.

C++ pastes compile to `bin/history/h_<id>`. Three `oppnn_stub` pastes need a
host `sincos` shim on Apple clang; the source files are unchanged.
`inoryy.cpp` in the parent folder is an older horizon-6 ChampionBot opponent,
not this `01` paste.

Python pastes live here but are not Fidelity fighters: `40911934` is a numpy
policy net.

Ranking: `RANKING.md` after `python3 tools/history_royale.py`. That table
predates `41275613`.

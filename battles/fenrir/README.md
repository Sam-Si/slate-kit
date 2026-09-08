# Fenrir battles

929 public replay JSON files for codingamer **fenrir** (`userId` 1415495).

Live agent on the ladder (2026-09-08): **`6500255`**, rank 1, score 58.31.
`findLastBattlesByAgentId` `[6500255, null]` only returns that agent’s
current ~233-game window and does **not** paginate. Retired Fenrir
`agentId`s list 0.

This folder is every Fenrir body we could still reach:

- the live last-battles window
- older Fenrir submits still sitting in local corpora
- extra ids harvested from opponents’ last-battles lists

`IDS.txt` is the sorted game-id list (one id per line).

Each `battle_<id>.json` is a `POST /services/gameResult/findByGameId`
response with body `[id, null]` (`frames`, `gameId`, `agents`, `ranks`).

To add newer games: list `[6500255, null]` on
`gamesPlayersRanking/findLastBattlesByAgentId`, then fetch each missing id
with `gameResult/findByGameId` `[id, null]`. Retired Fenrir agent ids
return an empty list.
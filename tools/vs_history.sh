#!/usr/bin/env bash
# Play a challenger against every unique C++ history paste.
# Compiles the referee, the challenger, and missing history binaries.
#
#   ./tools/vs_history.sh                  # ./newbot.cpp or first newbot.cpp found
#   ./tools/vs_history.sh path/to/bot.cpp
#
# Default grid: maps 0-17 × both sides × 100 repeats = 3600 games each.
# Pass = winrate_a >= 0.90 against every opponent. Override with env:
#   MAPS=0 SIDES=0 REPEATS=2 MIN_WR=0.90 ./tools/vs_history.sh
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"

SRC="${1:-}"
if [[ -z "$SRC" ]]; then
  if [[ -f newbot.cpp ]]; then
    SRC=newbot.cpp
  else
    SRC="$(find . -name newbot.cpp -not -path './.git/*' | head -1 || true)"
  fi
fi
if [[ -z "$SRC" || ! -f "$SRC" ]]; then
  echo "vs_history: need a .cpp (pass it or put newbot.cpp in the repo root)" >&2
  exit 2
fi

MAPS="${MAPS:-0-17}"
SIDES="${SIDES:-0,1}"
REPEATS="${REPEATS:-100}"
MIN_WR="${MIN_WR:-0.90}"

echo "vs_history: challenger $SRC"
if [[ ! -x bin/process_duel ]]; then
  ./tools/compile_referee.sh
fi
./tools/compile_bot.sh "$SRC" newbot
mkdir -p bin/history

export VS_MAPS="$MAPS" VS_SIDES="$SIDES" VS_REPEATS="$REPEATS" VS_MIN_WR="$MIN_WR"
python3 - "$ROOT" <<'PY'
import json, os, subprocess, sys
from pathlib import Path

root = Path(sys.argv[1])
maps = os.environ["VS_MAPS"]
sides = os.environ["VS_SIDES"]
repeats = int(os.environ["VS_REPEATS"])
min_wr = float(os.environ["VS_MIN_WR"])
idx = json.loads((root / "submission/history/INDEX.json").read_text())
newbot = root / "bin/newbot"
duel = root / "bin/process_duel"
fail = 0
played = 0
for row in idx:
    if row["lang"] != "cxx":
        print(f"vs_history: SKIP python {row['id']}")
        continue
    if "duplicate_of_" in row["file"]:
        print(f"vs_history: SKIP duplicate {row['id']}")
        continue
    src = root / "submission/history" / row["file"]
    bot = root / "bin/history" / f"h_{row['id']}"
    if not bot.is_file():
        print(f"vs_history: compile {row['file']}")
        subprocess.check_call(
            ["./tools/compile_bot.sh", str(src), f"history/h_{row['id']}"],
            cwd=root,
        )
    out = root / "bin" / f"newbot_vs_{row['id']}.json"
    print(f"vs_history: duel vs {row['file']}", flush=True)
    rc = subprocess.call(
        [
            str(duel),
            "--bot-a",
            str(newbot),
            "--bot-b",
            str(bot),
            "--maps",
            maps,
            "--sides",
            sides,
            "--repeats",
            str(repeats),
            "--first-turn-ms",
            "1000",
            "--time-budget-ms",
            "75",
            "--out",
            str(out),
        ],
        cwd=root,
    )
    if rc != 0 or not out.is_file():
        print(f"vs_history: FAIL duel {row['id']} rc={rc}", file=sys.stderr)
        fail = 1
        continue
    j = json.loads(out.read_text())
    g = int(j.get("games") or 0)
    wa = int(j.get("wins_a") or 0)
    wr = (wa / g) if g else 0.0
    want = None
    # maps "0-17" × sides "0,1" × repeats
    nmaps = 0
    for tok in maps.split(","):
        if "-" in tok:
            a, b = tok.split("-", 1)
            nmaps += int(b) - int(a) + 1
        elif tok:
            nmaps += 1
    nsides = len([s for s in sides.split(",") if s != ""])
    want = nmaps * nsides * repeats
    print(
        f"vs_history: {row['id']} games={g} want={want} "
        f"{wa}-{j.get('wins_b')}-{j.get('draws')} wr={wr:.3f}"
    )
    if g != want or wr < min_wr:
        print(
            f"vs_history: FAIL {row['id']} wr={wr:.3f} need>={min_wr} games={g}/{want}",
            file=sys.stderr,
        )
        fail = 1
    else:
        played += 1
if played == 0:
    print("vs_history: FAIL no C++ history opponents played", file=sys.stderr)
    sys.exit(1)
if fail:
    sys.exit(1)
print(f"vs_history: PASS {played} opponents wr>={min_wr} on {maps} x {sides} x {repeats}")
PY

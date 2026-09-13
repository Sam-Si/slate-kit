#!/usr/bin/env bash
# Install compile deps on Ubuntu/Debian and build every binary this repo ships.
# macOS uses the clang++ already on PATH.
# Usage:
#   ./setup.sh           install if needed, compile, one-game referee smoke
#   ./setup.sh --no-apt  compile + smoke (no apt)
#   ./setup.sh --verify  compile, then run tools/verify.sh (no extra smoke)
# Apt uses apt-get as root; sudo only when not root.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")" && pwd)"
# shellcheck source=tools/host.sh
source "$ROOT/tools/host.sh"
DO_APT=1
DO_VERIFY=0
for a in "$@"; do
  case "$a" in
    --no-apt) DO_APT=0 ;;
    --verify) DO_VERIFY=1 ;;
    -h|--help)
      sed -n '2,7p' "$0"
      exit 0
      ;;
    *)
      echo "setup.sh: unknown flag $a" >&2
      exit 2
      ;;
  esac
done

have_clang=0
if command -v clang++ >/dev/null 2>&1; then
  have_clang=1
fi
have_gxx=0
if command -v g++ >/dev/null 2>&1; then
  have_gxx=1
fi
have_py=0
if command -v python3 >/dev/null 2>&1; then
  have_py=1
fi

is_debian=0
if [[ -f /etc/os-release ]]; then
  # shellcheck disable=SC1091
  . /etc/os-release
  case "${ID:-}|${ID_LIKE:-}" in
    ubuntu*|debian*|*debian*|*ubuntu*) is_debian=1 ;;
  esac
fi

if [[ "$is_debian" -eq 1 ]]; then
  pkgs=""
  if [[ "$have_clang" -eq 0 ]]; then
    pkgs="$pkgs clang"
  fi
  if [[ "$have_py" -eq 0 ]]; then
    pkgs="$pkgs python3"
  fi
  pkgs="${pkgs# }"
  if [[ -n "$pkgs" ]]; then
    if [[ "$DO_APT" -ne 1 ]]; then
      echo "setup: missing $pkgs. Install with:" >&2
      echo "  apt-get update && apt-get install -y $pkgs" >&2
      exit 1
    fi
    if ! command -v apt-get >/dev/null 2>&1; then
      echo "setup: apt-get not found; install $pkgs by hand" >&2
      exit 1
    fi
    echo "setup: apt-get install $pkgs"
    # Root containers have no sudo. Only use sudo when we are not root.
    # $pkgs is a space-separated name list built above.
    # shellcheck disable=SC2086
    if [[ "$(id -u)" -eq 0 ]]; then
      apt-get update
      apt-get install -y $pkgs
    elif command -v sudo >/dev/null 2>&1; then
      sudo apt-get update
      sudo apt-get install -y $pkgs
    else
      echo "setup: need root or sudo to install $pkgs" >&2
      echo "  apt-get update && apt-get install -y $pkgs" >&2
      exit 1
    fi
    have_clang=0
    if command -v clang++ >/dev/null 2>&1; then
      have_clang=1
    fi
    have_gxx=0
    if command -v g++ >/dev/null 2>&1; then
      have_gxx=1
    fi
    have_py=0
    if command -v python3 >/dev/null 2>&1; then
      have_py=1
    fi
  else
    echo "setup: compiler and python3 already on PATH"
  fi
fi

if [[ "$have_clang" -eq 0 && "$have_gxx" -eq 0 ]]; then
  echo "setup: need clang++ or g++. On Ubuntu: sudo apt-get install -y clang" >&2
  exit 1
fi
if [[ "$DO_VERIFY" -eq 1 && "$have_py" -eq 0 ]]; then
  echo "setup: need python3 for tools/verify.sh" >&2
  exit 1
fi
pin_host_cxx || exit 1

echo "setup: compile referee + core bots"
for name in $CORE_NAMES; do
  compile_named "$name"
done

echo "setup: compile remaining contest pastes"
for name in $AVX_NAMES; do
  if skip_named "$name"; then
    echo "setup: SKIP $name on this host"
    continue
  fi
  compile_named "$name"
done

echo "setup: binaries in $ROOT/bin"
ls -l "$ROOT/bin"

if [[ "$DO_VERIFY" -eq 1 ]]; then
  exec "$ROOT/tools/verify.sh"
fi

# Fast proof the referee can play two different compiled bots (one generated map).
echo "setup: one-game referee smoke (searchbot vs frozen_b)"
SMOKE_OUT="$ROOT/bin/smoke_generate.json"
"$ROOT/bin/process_duel" \
  --bot-a "$ROOT/bin/searchbot" \
  --bot-b "$ROOT/bin/frozen_b" \
  --gen-maps 1 --gen-seed 42 --sides 0 --repeats 1 \
  --first-turn-ms 1000 --time-budget-ms 75 \
  --out "$SMOKE_OUT"
if command -v python3 >/dev/null 2>&1; then
  python3 - "$SMOKE_OUT" <<'PY'
import json, sys
from pathlib import Path
j = json.loads(Path(sys.argv[1]).read_text())
recs = j.get("games_detail") or []
if j.get("games") != 1 or len(recs) != 1:
    print("setup: smoke bad shape", file=sys.stderr)
    sys.exit(1)
rec = recs[0]
if rec.get("reason") != "finished" or int(rec.get("turns") or 0) < 20:
    print(
        f"setup: smoke failed reason={rec.get('reason')} turns={rec.get('turns')}",
        file=sys.stderr,
    )
    sys.exit(1)
print(f"setup: smoke ok reason=finished turns={rec['turns']}")
PY
else
  grep -q '"reason":"finished"' "$SMOKE_OUT" || {
    echo "setup: smoke missing reason=finished" >&2
    exit 1
  }
  echo "setup: smoke ok (no python3 to check turns)"
fi

echo "setup: done. Full check: ./tools/verify.sh"

#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
# shellcheck source=cxx.sh
source "$ROOT/tools/cxx.sh"
mkdir -p "$ROOT/bin"
# process_duel is POSIX (posix_spawn, poll). Links libc only.
# $CXX_REF_FLAGS is set by tools/cxx.sh (intentional word split).
# shellcheck disable=SC2086
"$CXX" $CXX_REF_FLAGS \
  -I"$ROOT/referee/core" -I"$ROOT/referee/physics" -I"$ROOT/referee" \
  -o "$ROOT/bin/process_duel" "$ROOT/referee/process_duel.cpp"
echo "ok $ROOT/bin/process_duel"

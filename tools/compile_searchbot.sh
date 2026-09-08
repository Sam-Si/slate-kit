#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
# shellcheck source=cxx.sh
source "$ROOT/tools/cxx.sh"
mkdir -p "$ROOT/bin"
# $CXX_BOT_FLAGS is set by tools/cxx.sh (intentional word split).
# shellcheck disable=SC2086
"$CXX" $CXX_BOT_FLAGS \
  -o "$ROOT/bin/searchbot" "$ROOT/submission/searchbot.cpp"
echo "ok $ROOT/bin/searchbot"

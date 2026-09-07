#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
# shellcheck source=cxx.sh
source "$ROOT/tools/cxx.sh"
src="${1:?usage: compile_bot.sh <src.cpp> <out-name>}"
name="${2:?usage: compile_bot.sh <src.cpp> <out-name>}"
case "$src" in
  /*) ;;
  *) src="$ROOT/$src" ;;
esac
mkdir -p "$ROOT/bin"
# $CXX_BOT_FLAGS is set by tools/cxx.sh (intentional word split).
# shellcheck disable=SC2086
"$CXX" $CXX_BOT_FLAGS -o "$ROOT/bin/$name" "$src"
echo "ok $ROOT/bin/$name"

#!/usr/bin/env bash
# Install compile deps on Ubuntu/Debian and build every binary this repo ships.
# macOS uses the clang++ already on PATH.
# Usage:
#   ./setup.sh           install if needed, then compile
#   ./setup.sh --no-apt  compile only
#   ./setup.sh --verify  compile, then run tools/verify.sh
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
      echo "  sudo apt-get update && sudo apt-get install -y $pkgs" >&2
      exit 1
    fi
    if ! command -v apt-get >/dev/null 2>&1; then
      echo "setup: apt-get not found; install $pkgs by hand" >&2
      exit 1
    fi
    echo "setup: apt-get install $pkgs"
    sudo apt-get update
    # $pkgs is a space-separated name list built above.
    # shellcheck disable=SC2086
    sudo apt-get install -y $pkgs
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

if [[ "$HOST_LINUX_AARCH64" -eq 1 ]]; then
  for name in $AVX_NAMES; do
    echo "setup: SKIP $name on Linux aarch64"
  done
else
  echo "setup: compile AVX GA pastes"
  for name in $AVX_NAMES; do
    compile_named "$name"
  done
fi

echo "setup: binaries in $ROOT/bin"
ls -l "$ROOT/bin"

if [[ "$DO_VERIFY" -eq 1 ]]; then
  exec "$ROOT/tools/verify.sh"
fi
echo "setup: done. Optional smoke: ./tools/verify.sh"

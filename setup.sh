#!/usr/bin/env bash
# Install compile deps, build every binary this host can, then verify.
# Usage:
#   ./setup.sh           apt if needed, compile, tools/verify.sh
#   ./setup.sh --no-apt  compile + verify (no apt)
#   ./setup.sh --verify  same as default (kept for old docs)
# Apt uses apt-get as root; sudo only when not root.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")" && pwd)"
# shellcheck source=tools/host.sh
source "$ROOT/tools/host.sh"
DO_APT=1
for a in "$@"; do
  case "$a" in
    --no-apt) DO_APT=0 ;;
    --verify) ;;
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
  echo "setup: need clang++ or g++. On Ubuntu: apt-get install -y clang" >&2
  exit 1
fi
if [[ "$have_py" -eq 0 ]]; then
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

echo "setup: verify"
exec "$ROOT/tools/verify.sh"

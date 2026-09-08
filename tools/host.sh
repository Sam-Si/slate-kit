# Shared host + bot lists. Source this file; do not execute it.
# Sets: HOST_LINUX_AARCH64, CORE_NAMES, AVX_NAMES, AGENT_NAMES

HOST_LINUX_AARCH64=0
HOST_X86=0
if [[ "$(uname -s)" == Linux ]]; then
  case "$(uname -m)" in
    aarch64|arm64) HOST_LINUX_AARCH64=1 ;;
  esac
fi
case "$(uname -m)" in
  x86_64|amd64) HOST_X86=1 ;;
esac
pin_host_cxx() {
  if [[ "$HOST_LINUX_AARCH64" -ne 1 ]]; then
    return 0
  fi
  if ! command -v clang++ >/dev/null 2>&1; then
    echo "tools/host.sh: need clang++ on Linux aarch64" >&2
    return 1
  fi
  CXX="$(command -v clang++)"
  export CXX
}

# Always built. process_duel is a referee, not a CG agent.
CORE_NAMES="process_duel searchbot frozen_b legacy_amalgam"

# Unguarded #pragma GCC target("avx2,..."). SKIP compile on Linux aarch64.
AVX_NAMES="ultimate_h4 ultimate_opt recommended previous_submission st_h8_p64 self_baseline_h4_p48 parallel8_deep h6_p48 inoryy"

# Contest-protocol agents (never process_duel).
AGENT_NAMES="searchbot frozen_b legacy_amalgam $AVX_NAMES"

core_src() {
  case "$1" in
    process_duel) echo "" ;;
    searchbot) echo "submission/searchbot.cpp" ;;
    frozen_b) echo "submission/frozen_magus.cpp" ;;
    inoryy) echo "submission/inoryy.cpp" ;;
    legacy_amalgam) echo "submission/ga/legacy_amalgam.cpp" ;;
    *) echo "submission/ga/${1}.cpp" ;;
  esac
}

# inoryy.cpp includes <immintrin.h> and only builds on x86_64.
skip_named() {
  local name="$1"
  if [[ "$HOST_LINUX_AARCH64" -eq 1 ]]; then
    case " $AVX_NAMES " in
      *" $name "*) return 0 ;;
    esac
  fi
  if [[ "$name" == inoryy && "$HOST_X86" -eq 0 ]]; then
    return 0
  fi
  return 1
}

compiled_agent_names() {
  local n out=""
  for n in $AGENT_NAMES; do
    if skip_named "$n"; then
      continue
    fi
    out="$out $n"
  done
  echo "${out# }"
}

compile_named() {
  local name="$1"
  local src
  case "$name" in
    process_duel) "$ROOT/tools/compile_referee.sh" ;;
    searchbot) "$ROOT/tools/compile_searchbot.sh" ;;
    *)
      src="$(core_src "$name")"
      "$ROOT/tools/compile_bot.sh" "$src" "$name"
      ;;
  esac
}

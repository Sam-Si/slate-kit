# Shared compiler detection. Source this file; do not execute it.
# Sets: CXX, CXX_REF_FLAGS, CXX_BOT_FLAGS

if [[ -z "${CXX:-}" ]]; then
  if command -v clang++ >/dev/null 2>&1; then
    CXX="$(command -v clang++)"
  elif command -v g++ >/dev/null 2>&1; then
    CXX="$(command -v g++)"
  else
    echo "tools/cxx.sh: need clang++ or g++ on PATH" >&2
    return 1 2>/dev/null || exit 1
  fi
fi
if [[ "$(uname -s)" == Linux ]]; then
  case "$(uname -m)" in
    aarch64|arm64)
      case "$CXX" in
        *g++*)
          echo "tools/cxx.sh: need clang++ on Linux aarch64" >&2
          return 1 2>/dev/null || exit 1
          ;;
      esac
      ;;
  esac
fi

_uname="$(uname -s)"
if [[ "$_uname" == Darwin ]]; then
  _native="-mcpu=native"
else
  _native="-march=native"
fi

# -pthread: parallel8_deep.cpp. Harmless on the other pastes.
# -Wno-unknown-pragmas: Apple clang ignores #pragma GCC target.
_common="-std=c++17 -DNDEBUG -Wno-unknown-pragmas -pthread"
if "$CXX" --version 2>/dev/null | head -n 1 | grep -qi clang; then
  _common="${_common} -Wno-pragma-once-outside-header"
fi
CXX_REF_FLAGS="-O2 ${_common}"
CXX_BOT_FLAGS="-O3 -fno-math-errno -fomit-frame-pointer ${_native} ${_common}"
unset _uname _native _common

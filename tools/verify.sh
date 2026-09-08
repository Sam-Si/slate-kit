#!/usr/bin/env bash
# Prove a clean clone can compile and play.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"
# shellcheck source=host.sh
source "$ROOT/tools/host.sh"
pin_host_cxx || exit 1

fail() { echo "verify: FAIL $*" >&2; exit 1; }
ok() { echo "verify: ok $*"; }

if ! git rev-parse --git-dir >/dev/null 2>&1; then
  fail "need git checkout"
fi

# --- hashes (sha256sum on Ubuntu, shasum on Darwin) ---
file_sha() {
  if command -v sha256sum >/dev/null 2>&1; then
    sha256sum "$1" | awk '{print $1}'
  elif command -v shasum >/dev/null 2>&1; then
    shasum -a 256 "$1" | awk '{print $1}'
  else
    fail "need sha256sum or shasum"
  fi
}
SHA_SB=131bccec663f92583b2f68e80b692728e2de8086cc3fd2ac1317564ea6cbeb8c
SHA_FZ=a18a3505d3c98e49c8119d4be2a4ff3727df32b5b6ccab2e4fcba66c597e6d01
got_sb="$(file_sha submission/searchbot.cpp)"
got_fz="$(file_sha submission/frozen_magus.cpp)"
[[ "$got_sb" == "$SHA_SB" ]] || fail "searchbot sha $got_sb"
[[ "$got_fz" == "$SHA_FZ" ]] || fail "frozen sha $got_fz"
grep -q "$SHA_SB" README.md || fail "searchbot sha missing from README.md"
grep -q "$SHA_FZ" submission/README.md || fail "frozen sha missing from submission/README.md"
ok "paste sha256"

# --- counts + every battle JSON ---
python3 - <<'PY'
import json, pathlib, sys
root = pathlib.Path(".")
want = {"battles/fenrir": 237, "battles/top5": 48, "battles/our_recent": 85}
need = ("frames", "gameId", "agents", "ranks")
for rel, n in want.items():
    files = sorted((root / rel).glob("battle_*.json"))
    if len(files) != n:
        print(f"verify: {rel} count {len(files)} want {n}", file=sys.stderr)
        sys.exit(1)
    for f in files:
        try:
            j = json.loads(f.read_text())
        except Exception as e:
            print(f"verify: bad json {f}: {e}", file=sys.stderr)
            sys.exit(1)
        if not isinstance(j, dict) or any(k not in j for k in need):
            print(f"verify: bad shape {f}", file=sys.stderr)
            sys.exit(1)
ids = [ln.strip() for ln in (root / "battles/LAST_MONTH_IDS.txt").read_text().splitlines()
       if ln.strip() and ln.strip()[0].isdigit()]
if len(ids) != 6513:
    print(f"verify: LAST_MONTH_IDS numeric {len(ids)} want 6513", file=sys.stderr)
    sys.exit(1)
print("verify: ok battle counts and json")
PY

# --- setup --help ---
"$ROOT/setup.sh" --help >/dev/null || fail "setup.sh --help"
ok "setup.sh --help"

# --- compile the host set (same function as setup.sh) ---
for name in $CORE_NAMES; do
  compile_named "$name"
  [[ -x "bin/$name" ]] || fail "missing bin/$name"
done
for name in $AVX_NAMES; do
  if skip_named "$name"; then
    echo "verify: SKIP compile $name on this host"
    continue
  fi
  compile_named "$name"
  [[ -x "bin/$name" ]] || fail "missing bin/$name"
done
ok "binaries"

# --- protocol-smoke agents (never process_duel) ---
VERIFY_AGENTS="$(compiled_agent_names)"
export VERIFY_AGENTS
python3 - <<'PY'
import os, select, signal, subprocess, sys, time
from pathlib import Path

root = Path(".").resolve()
agents = [n for n in os.environ.get("VERIFY_AGENTS", "").split() if n]

stdin = (
    "3\n4\n"
    "12929 7191\n5614 2557\n4114 7440\n13515 2340\n"
    "12661 7613 0 0 -1 1\n"
    "13197 6769 0 0 -1 1\n"
    "12126 8458 0 0 -1 1\n"
    "13732 5924 0 0 -1 1\n"
).encode()

def parse_line(line: str) -> bool:
    parts = line.strip().split()
    if len(parts) < 3:
        return False
    try:
        int(parts[0]); int(parts[1])
    except ValueError:
        return False
    tok = parts[2]
    if tok in ("BOOST", "SHIELD"):
        return True
    try:
        t = int(tok)
    except ValueError:
        return False
    return 0 <= t <= 200

def smoke(bin_path: Path) -> bool:
    try:
        p = subprocess.Popen(
            [str(bin_path)],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.DEVNULL,
        )
    except OSError:
        return False
    try:
        if p.stdin is None or p.stdout is None:
            return False
        try:
            p.stdin.write(stdin)
            p.stdin.flush()
            p.stdin.close()
        except BrokenPipeError:
            return False
        fd = p.stdout.fileno()
        os.set_blocking(fd, False)
        deadline = time.time() + 5.0
        buf = b""
        lines = []
        while time.time() < deadline and len(lines) < 2:
            remain = max(0.0, deadline - time.time())
            r, _, _ = select.select([fd], [], [], remain)
            if not r:
                break
            try:
                chunk = os.read(fd, 4096)
            except BlockingIOError:
                continue
            except OSError:
                break
            if not chunk:
                break
            buf += chunk
            while b"\n" in buf and len(lines) < 2:
                line, buf = buf.split(b"\n", 1)
                s = line.decode("utf-8", "replace")
                if s.endswith("\r"):
                    s = s[:-1]
                if s.strip():
                    lines.append(s)
        return len(lines) >= 2 and parse_line(lines[0]) and parse_line(lines[1])
    except (BrokenPipeError, OSError):
        return False
    finally:
        if p.poll() is None:
            p.send_signal(signal.SIGTERM)
            try:
                p.wait(timeout=1)
            except subprocess.TimeoutExpired:
                p.kill()
                p.wait()

failed = []
for name in agents:
    path = root / "bin" / name
    if not path.is_file():
        print(f"verify: FAIL missing agent {name}", file=sys.stderr)
        sys.exit(1)
    good = smoke(path) or smoke(path)
    if not good:
        failed.append(name)
        print(f"verify: protocol-smoke fail {name}", file=sys.stderr)
    else:
        print(f"verify: ok protocol {name}")
if failed:
    sys.exit(1)
PY

# --- RULES vs constants.h (numeric; same-line number) ---
python3 - <<'PY'
import re, sys
from pathlib import Path

const = Path("referee/core/constants.h").read_text()
rules = Path("RULES.md").read_text()
want = {
    "kPodRadius": 400.0,
    "kCpRadius": 600.0,
    "kMaxThrust": 200.0,
    "kBoostThrust": 650.0,
    "kMaxRotateDeg": 18.0,
    "kFriction": 0.85,
    "kMinImpulse": 120.0,
    "kTimeoutLimit": 100.0,
    "kShieldTimerActivate": 4.0,
    "kShieldMassFactorFidelity": 0.1,
    "kEpsilon": 0.00001,
    "kMaxGameTurns": 500.0,
}
num_re = re.compile(r"[-+]?(?:\d+\.\d+|\d+)(?:[eE][-+]?\d+)?")
for sym, exp in want.items():
    m = re.search(rf"{sym}\s*=\s*([^;]+);", const)
    if not m:
        print(f"verify: {sym} missing in constants.h", file=sys.stderr)
        sys.exit(1)
    header = float(m.group(1).strip())
    if abs(header - exp) > 1e-12:
        print(f"verify: constants.h {sym}={header} want {exp}", file=sys.stderr)
        sys.exit(1)
    line = next((ln for ln in rules.splitlines() if sym in ln), None)
    if line is None:
        print(f"verify: {sym} missing in RULES.md", file=sys.stderr)
        sys.exit(1)
    nums = [float(x) for x in num_re.findall(line)]
    if not any(abs(n - exp) <= 1e-12 for n in nums):
        print(f"verify: RULES.md line for {sym} has {nums}, want {exp}", file=sys.stderr)
        sys.exit(1)
print("verify: ok RULES vs constants.h")
PY

# --- foreign + cookie scans (fragments so this file is not a hit) ---
python3 - <<'PY'
import subprocess, sys
from pathlib import Path

# needles split so the checker source never contains the joined string
foreign = [
    "mad_pod_" + "arena",
    "src/" + "engine",
    "src/" + "core",
    "docs/" + "SSOT",
    "third_" + "party/",
    "searchbot_" + "harness",
    "fenrir_" + "emit.h",
    "opt_" + "search.inc",
]
cookies = ["cg" + "Session", "remember" + "Me"]

# Include untracked files so the scan works before the first commit.
out = subprocess.check_output(
    ["git", "ls-files", "--cached", "--others", "--exclude-standard"],
    text=True,
)
files = []
for rel in out.splitlines():
    if rel.startswith("battles/") and rel.endswith(".json"):
        continue
    if rel.startswith("docs/egm/"):
        continue
    files.append(rel)

hits = []
for rel in files:
    try:
        text = Path(rel).read_text(errors="replace")
    except OSError:
        continue
    for n in foreign + cookies:
        if n in text:
            hits.append(f"{rel}: {n}")
if hits:
    print("verify: leak", *hits, sep="\n", file=sys.stderr)
    sys.exit(1)
print("verify: ok foreign/cookie scan")
PY

# --- Fidelity smokes ---
play() {
  local label="$1" out="$2"
  shift 2
  local try
  for try in 1 2; do
    rm -f "$out"
    if ! ./bin/process_duel "$@" --out "$out"; then
      if [[ "$try" -lt 2 ]]; then
        echo "verify: retry $label ($try) referee nonzero"
        continue
      fi
      fail "$label referee nonzero"
    fi
    if python3 - "$out" "$label" <<'PY'
import json, sys
from pathlib import Path
path, label = sys.argv[1], sys.argv[2]
j = json.loads(Path(path).read_text())
if j.get("games") != 1:
    print(f"verify: {label} games={j.get('games')} want 1", file=sys.stderr)
    sys.exit(2)
recs = j.get("games_detail") or []
if len(recs) != 1:
    print(f"verify: {label} detail {len(recs)}", file=sys.stderr)
    sys.exit(2)
rec = recs[0]
reason = rec.get("reason")
turns = int(rec.get("turns") or 0)
if reason != "finished" or turns < 20:
    print(f"verify: {label} try failed reason={reason} turns={turns}", file=sys.stderr)
    sys.exit(1)
print(f"verify: ok {label} winner={rec.get('mapped_winner')} turns={turns} reason={reason}")
PY
    then
      return 0
    fi
    if [[ "$try" -lt 2 ]]; then
      echo "verify: retry $label ($try)"
    fi
  done
  fail "$label did not finish a real game"
}

if [[ "$HOST_LINUX_AARCH64" -eq 1 ]]; then
  echo "verify: SKIP catalog vs ultimate_h4 on Linux aarch64"
else
  play "catalog map0" bin/smoke_catalog.json \
    --bot-a ./bin/searchbot --bot-b ./bin/ultimate_h4 \
    --maps 0 --sides 0 --repeats 1 \
    --first-turn-ms 1000 --time-budget-ms 75
fi

play "generate seed42" bin/smoke_generate.json \
  --bot-a ./bin/searchbot --bot-b ./bin/frozen_b \
  --gen-maps 1 --gen-seed 42 --sides 0 --repeats 1 \
  --first-turn-ms 1000 --time-budget-ms 75

echo "verify: PASS"

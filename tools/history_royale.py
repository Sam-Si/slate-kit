#!/usr/bin/env python3
"""Round-robin Fidelity ranking of submission/history binaries."""
from __future__ import annotations

import json
import os
import subprocess
import sys
import time
from concurrent.futures import ThreadPoolExecutor, as_completed
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
DUEL = ROOT / "bin" / "process_duel"
HIST_BIN = ROOT / "bin" / "history"
OUT_DIR = ROOT / "bin" / "royale"
RANK = ROOT / "submission" / "history" / "RANKING.md"
INDEX = ROOT / "submission" / "history" / "INDEX.json"

# 5 maps × 2 sides = 10 games per pair.
MAPS = "0,3,7,11,15"
SIDES = "0,1"
WORKERS = 4


def fighters() -> list[tuple[str, Path]]:
    idx = json.loads(INDEX.read_text())
    out = []
    for row in idx:
        if row["lang"] != "cxx":
            continue
        if "duplicate_of_" in row["file"]:
            continue
        sid = str(row["id"])
        binp = HIST_BIN / f"h_{sid}"
        if binp.is_file() and os.access(binp, os.X_OK):
            out.append((sid, binp))
    return out


def play(a: tuple[str, Path], b: tuple[str, Path]) -> dict:
    OUT_DIR.mkdir(parents=True, exist_ok=True)
    out = OUT_DIR / f"{a[0]}_vs_{b[0]}.json"
    if out.is_file():
        j = json.loads(out.read_text())
        return {
            "a": a[0],
            "b": b[0],
            "rc": 0,
            "sec": 0,
            "games": j.get("games"),
            "wins_a": j.get("wins_a"),
            "wins_b": j.get("wins_b"),
            "draws": j.get("draws"),
            "winrate_a": j.get("winrate_a"),
            "resumed": True,
        }
    cmd = [
        str(DUEL),
        "--bot-a", str(a[1]),
        "--bot-b", str(b[1]),
        "--maps", MAPS,
        "--sides", SIDES,
        "--repeats", "1",
        "--first-turn-ms", "1000",
        "--time-budget-ms", "75",
        "--out", str(out),
    ]
    t0 = time.time()
    r = subprocess.run(cmd, cwd=ROOT, capture_output=True, text=True)
    rec = {
        "a": a[0],
        "b": b[0],
        "rc": r.returncode,
        "sec": round(time.time() - t0, 1),
    }
    if out.is_file():
        j = json.loads(out.read_text())
        rec.update({
            "games": j.get("games"),
            "wins_a": j.get("wins_a"),
            "wins_b": j.get("wins_b"),
            "draws": j.get("draws"),
            "winrate_a": j.get("winrate_a"),
        })
    else:
        rec["err"] = (r.stderr or r.stdout)[-300:]
    return rec


def write_rank(results: list[dict], bots: list[tuple[str, Path]]) -> None:
    idx = {str(r["id"]): r for r in json.loads(INDEX.read_text())}
    stats = {sid: {"w": 0, "l": 0, "d": 0, "g": 0, "opp": 0} for sid, _ in bots}
    for rec in results:
        if rec.get("games") is None:
            continue
        a, b = rec["a"], rec["b"]
        wa, wb, dr, g = rec["wins_a"], rec["wins_b"], rec["draws"], rec["games"]
        stats[a]["w"] += wa
        stats[a]["l"] += wb
        stats[a]["d"] += dr
        stats[a]["g"] += g
        stats[a]["opp"] += 1
        stats[b]["w"] += wb
        stats[b]["l"] += wa
        stats[b]["d"] += dr
        stats[b]["g"] += g
        stats[b]["opp"] += 1
    rows = []
    for sid, _ in bots:
        s = stats[sid]
        wr = (s["w"] / s["g"]) if s["g"] else 0.0
        rows.append((wr, s["w"], -s["l"], sid, s))
    rows.sort(reverse=True)
    lines = [
        "# History battle royale",
        "",
        f"Grid: catalog maps `{MAPS}` × sides `{SIDES}` × 1 = 10 games per pair.",
        f"Fighters: {len(bots)} unique C++ pastes. Duplicate `40873000` omitted.",
        "Python pastes are listed below and were not in the C++ royale.",
        "",
        "| Rank | Submission | W-L-D | Games | Winrate |",
        "|---:|---|---:|---:|---:|",
    ]
    for i, (wr, w, nl, sid, s) in enumerate(rows, 1):
        meta = idx[sid]
        name = meta["file"]
        lines.append(
            f"| {i} | `{name}` | {s['w']}-{s['l']}-{s['d']} | {s['g']} | {wr:.3f} |"
        )
    lines += [
        "",
        "## Pair results",
        "",
        "| A | B | W-L-D | wr_a |",
        "|---|---|---:|---:|",
    ]
    for rec in results:
        if rec.get("games") is None:
            lines.append(f"| {rec['a']} | {rec['b']} | FAIL | |")
            continue
        lines.append(
            f"| {rec['a']} | {rec['b']} | {rec['wins_a']}-{rec['wins_b']}-{rec['draws']} | {rec['winrate_a']:.3f} |"
        )
    RANK.write_text("\n".join(lines) + "\n")
    (OUT_DIR / "results.json").write_text(json.dumps(results, indent=2) + "\n")


def main() -> int:
    import os
    bots = fighters()
    if len(bots) < 2:
        print("need at least 2 compiled history bots", file=sys.stderr)
        return 1
    if not DUEL.is_file():
        print("missing bin/process_duel", file=sys.stderr)
        return 1
    pairs = []
    for i, a in enumerate(bots):
        for b in bots[i + 1 :]:
            pairs.append((a, b))
    print(f"royale {len(bots)} bots {len(pairs)} pairs grid {MAPS} x {SIDES}", flush=True)
    results = []
    done = 0
    with ThreadPoolExecutor(max_workers=WORKERS) as ex:
        futs = {ex.submit(play, a, b): (a[0], b[0]) for a, b in pairs}
        for fut in as_completed(futs):
            rec = fut.result()
            results.append(rec)
            done += 1
            print(
                f"[{done}/{len(pairs)}] {rec['a']} vs {rec['b']} "
                f"{rec.get('wins_a')}-{rec.get('wins_b')} {rec.get('sec')}s",
                flush=True,
            )
            write_rank(results, bots)
    write_rank(results, bots)
    print("wrote", RANK)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

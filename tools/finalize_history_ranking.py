#!/usr/bin/env python3
"""Rewrite RANKING.md from completed pair JSONs. Does not run duels."""
from __future__ import annotations

import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
OUT_DIR = ROOT / "bin" / "royale"
RANK = ROOT / "submission" / "history" / "RANKING.md"
INDEX = ROOT / "submission" / "history" / "INDEX.json"
MAPS = "0,3,7,11,15"
SIDES = "0,1"


def main() -> int:
    idx_rows = json.loads(INDEX.read_text())
    idx = {str(r["id"]): r for r in idx_rows}
    fighters = [
        str(r["id"])
        for r in idx_rows
        if r["lang"] == "cxx" and "duplicate_of_" not in r["file"]
    ]
    results = []
    for i, a in enumerate(fighters):
        for b in fighters[i + 1 :]:
            p = OUT_DIR / f"{a}_vs_{b}.json"
            if not p.is_file():
                results.append({"a": a, "b": b, "games": None})
                continue
            j = json.loads(p.read_text())
            if j.get("games") != 10:
                results.append({"a": a, "b": b, "games": j.get("games"), "partial": True})
                continue
            results.append(
                {
                    "a": a,
                    "b": b,
                    "games": j["games"],
                    "wins_a": j["wins_a"],
                    "wins_b": j["wins_b"],
                    "draws": j["draws"],
                    "winrate_a": j["winrate_a"],
                }
            )
    complete = [r for r in results if r.get("games") == 10]
    stats = {sid: {"w": 0, "l": 0, "d": 0, "g": 0, "opp": 0, "sweeps": 0, "swept": 0} for sid in fighters}
    for rec in complete:
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
        if wa == 10:
            stats[a]["sweeps"] += 1
            stats[b]["swept"] += 1
        if wb == 10:
            stats[b]["sweeps"] += 1
            stats[a]["swept"] += 1
    rows = []
    for sid in fighters:
        s = stats[sid]
        wr = (s["w"] / s["g"]) if s["g"] else 0.0
        rows.append((wr, s["w"], -s["l"], sid, s))
    rows.sort(reverse=True)

    lines = [
        "# History battle royale",
        "",
        "Last 20 contest pastes (newest first in `INDEX.json`).",
        f"C++ field: {len(fighters)} unique binaries. Duplicate `40873000` omitted (byte-identical to `40873174`).",
        "Python pastes `40911934` and `40777737` are stored in this folder but were not Fidelity fighters:",
        "`40911934` is a numpy policy net; `40777737` JIT-compiles embedded C with `-mavx2` and does not load on Apple Silicon.",
        "",
        "## Method",
        "",
        f"- Referee: `bin/process_duel` (Fidelity).",
        f"- Grid: catalog maps `{MAPS}` × sides `{SIDES}` × repeats 1 = **10 games per pair**.",
        f"- Time: first turn 1000 ms, later 75 ms.",
        f"- Complete pairs: **{len(complete)} / {len(results)}** (need {len(fighters) * (len(fighters) - 1) // 2}).",
        "- Rank key: winrate, then wins, then fewer losses.",
        "- Win = 1, loss = 0, draw = 0. `winrate = wins / games`.",
        "",
        "## Ranking",
        "",
        "| Rank | Submission | W-L-D | Games | Winrate | Sweeps | Swept |",
        "|---:|---|---:|---:|---:|---:|---:|",
    ]
    for i, (wr, w, nl, sid, s) in enumerate(rows, 1):
        name = idx[sid]["file"]
        lines.append(
            f"| {i} | `{name}` | {s['w']}-{s['l']}-{s['d']} | {s['g']} | {wr:.3f} | {s['sweeps']} | {s['swept']} |"
        )

    lines += [
        "",
        "## Not in the C++ field",
        "",
        "| File | Why |",
        "|---|---|",
    ]
    for r in idx_rows:
        if r["lang"] != "cxx" or "duplicate_of_" in r["file"]:
            why = (
                f"byte-identical duplicate of `{r['desc'].split('duplicate_of_')[-1]}`"
                if "duplicate_of_" in r["file"]
                else "Python paste; not a compiled Fidelity fighter"
            )
            lines.append(f"| `{r['file']}` | {why} |")

    lines += [
        "",
        "## Pair results",
        "",
        "| A | B | W-L-D | wr_a |",
        "|---|---|---:|---:|",
    ]
    for rec in results:
        if rec.get("games") != 10:
            tag = "PARTIAL" if rec.get("partial") else "MISSING"
            lines.append(f"| {rec['a']} | {rec['b']} | {tag} | |")
            continue
        lines.append(
            f"| {rec['a']} | {rec['b']} | {rec['wins_a']}-{rec['wins_b']}-{rec['draws']} | {rec['winrate_a']:.3f} |"
        )

    # compact matrix, ranked order
    order = [sid for _, _, _, sid, _ in rows]
    pos = {sid: i for i, sid in enumerate(order)}
    cell = {(rec["a"], rec["b"]): rec for rec in complete}
    lines += [
        "",
        "## Head-to-head (wins for row vs column, 10-game pairs)",
        "",
    ]
    header = "| | " + " | ".join(order) + " |"
    sep = "|---|" + "|".join("---:" for _ in order) + "|"
    lines.append(header)
    lines.append(sep)
    for a in order:
        bits = [a]
        for b in order:
            if a == b:
                bits.append("—")
                continue
            rec = cell.get((a, b)) or cell.get((b, a))
            if rec is None:
                bits.append("?")
            elif rec["a"] == a:
                bits.append(str(rec["wins_a"]))
            else:
                bits.append(str(rec["wins_b"]))
        lines.append("| " + " | ".join(bits) + " |")

    RANK.write_text("\n".join(lines) + "\n")
    (OUT_DIR / "results.json").write_text(json.dumps(results, indent=2) + "\n")
    print(f"wrote {RANK} complete={len(complete)}/{len(results)}")
    return 0 if len(complete) == len(results) else 2


if __name__ == "__main__":
    raise SystemExit(main())

#!/usr/bin/env python3
"""Reconcile a named V9 debt CSV with current C++ *source shapes*.

This does NOT certify V9 gameplay parity or update the historic Qoder census.
It only identifies stale candidates and still-obvious no-op implementations.
"""
from __future__ import annotations

import argparse
from collections import Counter
import csv
import json
from pathlib import Path
import re

NAME = re.compile(r"^[A-Za-z_][A-Za-z_0-9]*$")
FUNCTION = re.compile(
    r"\bint\s+Lua([A-Za-z_][A-Za-z_0-9]*)\s*\(\s*"
    r"Lua_State\s*\*\s*L\s*\)\s*\{", re.MULTILINE
)
COMMENTS = re.compile(r"//[^\n]*|/\*.*?\*/", re.DOTALL)


def extract_block(source: str, brace_open: int) -> str | None:
    """Brace matching that skips C++ comments and quoted string literals."""
    depth = 0
    i = brace_open
    mode = "code"
    while i < len(source):
        c = source[i]
        n = source[i + 1] if i + 1 < len(source) else ""
        if mode == "line":
            if c == "\n":
                mode = "code"
        elif mode == "block":
            if c == "*" and n == "/":
                mode = "code"
                i += 1
        elif mode in ("string", "char"):
            if c == "\\":
                i += 1
            elif c == ('"' if mode == "string" else "'"):
                mode = "code"
        elif c == "/" and n == "/":
            mode = "line"
            i += 1
        elif c == "/" and n == "*":
            mode = "block"
            i += 1
        elif c == '"':
            mode = "string"
        elif c == "'":
            mode = "char"
        elif c == "{":
            depth += 1
        elif c == "}":
            depth -= 1
            if depth == 0:
                return source[brace_open + 1:i]
        i += 1
    return None


def shape(body: str | None) -> str:
    if body is None:
        return "UNBALANCED_BODY"
    text = COMMENTS.sub("", body).strip()
    if re.fullmatch(r"return\s+0\s*;", text):
        return "EXACT_RETURN_ZERO"
    if re.fullmatch(r"return\s+1\s*;", text):
        return "EXACT_RETURN_ONE"
    if re.fullmatch(
        r"(?:Lua_PushNumber\s*\(\s*L\s*,\s*"
        r"(?:[+-]?\d+)\s*\)\s*;\s*)+return\s+\d+\s*;", text
    ):
        return "CONSTANT_NUMERIC_RESULT"
    return "NONTRIVIAL_SOURCE_SHAPE"


def source_inventory(source_dir: Path) -> dict[str, list[dict]]:
    entries: dict[str, list[dict]] = {}
    paths = sorted(source_dir.glob("ScriptFuns_V9_*.cpp"))
    if not paths:
        raise ValueError(f"No ScriptFuns_V9_*.cpp files in {source_dir}")
    for path in paths:
        data = path.read_text(encoding="utf-8", errors="replace")
        for m in FUNCTION.finditer(data):
            body = extract_block(data, m.end() - 1)
            entries.setdefault(m.group(1), []).append({
                "file": path.name,
                "source_shape": shape(body),
                "line": data.count("\n", 0, m.start()) + 1,
            })
    return entries


def reconcile(snapshot_csv: Path, source_dir: Path) -> dict:
    entries = source_inventory(source_dir)
    rows = []
    seen = set()
    with snapshot_csv.open(encoding="utf-8-sig", newline="") as file:
        reader = csv.DictReader(file)
        if not {"name", "grade"}.issubset(reader.fieldnames or []):
            raise ValueError("CSV requires name,grade columns; grade = STUB or NOEFFECT")
        for record in reader:
            name = (record["name"] or "").strip()
            grade = (record["grade"] or "").strip().upper()
            if not NAME.fullmatch(name) or grade not in ("STUB", "NOEFFECT"):
                raise ValueError(f"Bad debt row name or grade: {name!r} {grade!r}")
            if name in seen:
                raise ValueError(f"Duplicate debt name: {name}")
            seen.add(name)
            matches = entries.get(name, [])
            if not matches:
                verdict = "SOURCE_DEFINITION_NOT_FOUND"
            elif len(matches) > 1:
                verdict = "DUPLICATE_SOURCE_DEFINITIONS"
            elif matches[0]["source_shape"] in ("EXACT_RETURN_ZERO", "EXACT_RETURN_ONE"):
                verdict = "EXACT_TRIVIAL_RETURN"
            elif matches[0]["source_shape"] == "CONSTANT_NUMERIC_RESULT":
                verdict = "CONSTANT_NUMERIC_RESULT"
            else:
                # May still do nothing, be unsafe, or differ from the ELF.
                verdict = "NEEDS_V9_SEMANTIC_REVIEW"
            rows.append({
                "name": name,
                "snapshot_grade": grade,
                "snapshot_blocker_class": (record.get("blocker_class") or "").strip(),
                "source_verdict": verdict,
                "definitions": matches,
                "snapshot_may_be_stale": (grade == "STUB" and verdict ==
                                          "NEEDS_V9_SEMANTIC_REVIEW"),
            })
    return {
        "schema_version": 1,
        "source_rows": len(rows),
        "source_verdict_counts": dict(sorted(Counter(
            x["source_verdict"] for x in rows).items())),
        "snapshot_grade_counts": dict(sorted(Counter(
            x["snapshot_grade"] for x in rows).items())),
        "scope": "Source shapes only; no proof of V9 behavior, no automatic debt removal.",
        "records": rows,
    }


def main() -> int:
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument("--snapshot", type=Path, required=True)
    p.add_argument("--source-dir", type=Path, required=True)
    p.add_argument("--output", type=Path, required=True)
    args = p.parse_args()
    if not args.snapshot.is_file() or not args.source_dir.is_dir():
        p.error("Missing snapshot file or source directory")
    output = args.output.resolve()
    if output == args.snapshot.resolve() or output.is_relative_to(args.source_dir.resolve()):
        p.error("Refusing to overwrite source code or the input snapshot")
    result = reconcile(args.snapshot, args.source_dir)
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(result, ensure_ascii=False, indent=2) + "\n",
                           encoding="utf-8")
    print(f"DEBT_ROWS={result['source_rows']} VERDICTS={result['source_verdict_counts']}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

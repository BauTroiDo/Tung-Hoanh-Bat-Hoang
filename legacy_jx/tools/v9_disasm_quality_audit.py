#!/usr/bin/env python3
"""Conservative, read-only audit of legacy V9 disassembly listings.

Disassembly is evidence, not a proof of behavior or a source-code patch.
Do not claim that absent listing == absent bytes, that first RET bounds a
function, or that a numeric address has an identified global owner.
"""
from __future__ import annotations

import argparse
from collections import Counter
from dataclasses import asdict, dataclass
import json
from pathlib import Path
import re

LINE = re.compile(r"^\s*0x([0-9a-fA-F]+):\s*([a-z][a-z0-9]*)\b\s*(.*)$")
HEADER = re.compile(r"^\s*;?\s*(?:Function|Name)\s*:\s*(\S.*?)\s*$", re.I)
START = re.compile(r"^\s*;?\s*(?:Virtual Address|Address)\s*:\s*0x([0-9a-fA-F]+)", re.I)
NINS = re.compile(r"^\s*;?\s*Instructions\s*:\s*(\d+)", re.I)
CALL = re.compile(r"\bcall\s+0x([0-9a-fA-F]+)\b", re.I)
BARE = re.compile(r"\[\s*(?:ds:)?0x([0-9a-fA-F]+)\s*\]", re.I)
REG_OFFSET = re.compile(
    r"\[\s*(?:[a-z]{2,3}:)?(e(?:ax|bx|cx|dx|si|di|bp|sp))\s*"
    r"[+]\s*0x([0-9a-fA-F]+)", re.I
)
REG_LOAD = re.compile(r"^(?:mov|add)\s+(e(?:ax|bx|cx|dx|si|di|bp)),\s*0x([0-9a-fA-F]+)$", re.I)
REG_DEREF = re.compile(r"\[\s*(?:[a-z]{2,3}:)?(e(?:ax|bx|cx|dx|si|di|bp))\b", re.I)
REGISTER_DEST = re.compile(r"^[a-z][a-z0-9]*\s+(e(?:ax|bx|cx|dx|si|di|bp))\s*,", re.I)
HEX = re.compile(r"0x([0-9a-fA-F]{6,8})\b", re.I)
# Ring-0, segment string-I/O and system ops are anomalous in this user-space
# Lua wrapper corpus. A flag means inspect bytes / disassembler alignment.
SUSPECT_OPS = frozenset(("outsb", "outsw", "outsd", "insb", "insw", "insd", "cli", "sti"))
PADDING = frozenset(("nop",))
TERMINATORS = frozenset(("ret", "retn", "retf", "jmp"))
DEFAULT_LO = 0x08000000
DEFAULT_HI = 0x0A000000


@dataclass(frozen=True)
class Instruction:
    address: int
    mnemonic: str
    operand: str


@dataclass
class Listing:
    name: str
    file: str
    entry: int | None
    declared_instructions: int | None
    instructions: list[Instruction]
    issues: list[str]


def parse_listing(text: str, source_name: str) -> Listing:
    name = Path(source_name).stem
    entry = None
    declared = None
    instructions: list[Instruction] = []
    issues: list[str] = []
    for line in text.splitlines():
        m = HEADER.match(line)
        if m:
            name = m.group(1).strip()
            continue
        m = START.match(line)
        if m:
            entry = int(m.group(1), 16)
            continue
        m = NINS.match(line)
        if m:
            declared = int(m.group(1))
            continue
        m = LINE.match(line)
        if m:
            inst = Instruction(int(m.group(1), 16), m.group(2).lower(),
                               m.group(3).split(";", 1)[0].strip())
            instructions.append(inst)
        elif re.match(r"^\s*0x[0-9a-fA-F]+:", line):
            issues.append("unparsed_instruction_line")
    if not instructions:
        issues.append("no_decoded_instructions")
    if entry is not None and instructions and instructions[0].address != entry:
        issues.append("entry_address_mismatch")
    if declared is not None and declared != len(instructions):
        issues.append("declared_instruction_count_mismatch")
    addrs = [i.address for i in instructions]
    if len(addrs) != len(set(addrs)) or addrs != sorted(addrs):
        issues.append("duplicate_or_unordered_instructions")
    return Listing(name, Path(source_name).name, entry, declared,
                   instructions, sorted(set(issues)))


def printable_immediate(immediate: int) -> bool:
    """Two or more *four-byte* ASCII immediates are a heuristic, not proof."""
    octets = immediate.to_bytes(4, "little")
    return all(32 <= b <= 126 for b in octets)


def analyze_listing(listing: Listing, entry_addresses: set[int],
                    lower: int = DEFAULT_LO, upper: int = DEFAULT_HI) -> dict:
    issues = list(listing.issues)
    ins = listing.instructions
    suspicious_ops = sorted({x.mnemonic for x in ins if x.mnemonic in SUSPECT_OPS})
    ascii_immediates = sorted({
        f"0x{int(m.group(1), 16):x}"
        for x in ins for m in HEX.finditer(x.operand)
        if printable_immediate(int(m.group(1), 16))
    })
    if suspicious_ops:
        issues.append("suspect_privileged_or_string_io_opcode")
    if len(ascii_immediates) >= 2:
        issues.append("multiple_printable_ascii_immediates")

    call_targets = {int(m.group(1), 16) for x in ins
                    for m in CALL.finditer(f"{x.mnemonic} {x.operand}")}
    # A target lacking a *named listing entry* can still have perfectly
    # readable bytes in the ELF. Never classify it as "unreadable".
    calls_without_listing = call_targets - entry_addresses

    direct_candidates: set[int] = set()
    offset_candidates: set[int] = set()
    materialized_candidates: set[int] = set()

    for x in ins:
        if x.mnemonic == "lea":
            continue  # LEA is address arithmetic, not a memory access.
        for m in BARE.finditer(x.operand):
            direct_candidates.add(int(m.group(1), 16))
        for m in REG_OFFSET.finditer(x.operand):
            offset_candidates.add(int(m.group(2), 16))

    # Keep a candidate only when a register loaded/materialized with an
    # immediate is actually dereferenced later. Invalidate on register
    # overwrite. This intentionally under-approximates dataflow.
    register_values: dict[str, int] = {}
    for x in ins:
        operand = x.operand
        for m in REG_DEREF.finditer(operand):
            if m.group(1).lower() in register_values:
                materialized_candidates.add(register_values[m.group(1).lower()])
        assigned = REGISTER_DEST.match(f"{x.mnemonic} {operand}")
        dest = assigned.group(1).lower() if assigned else None
        load = REG_LOAD.match(f"{x.mnemonic} {operand}")
        if dest is not None:
            if load is not None:
                register_values[dest] = int(load.group(2), 16)
            elif x.mnemonic not in ("cmp", "test"):
                register_values.pop(dest, None)

    def filter_data(values: set[int]) -> list[str]:
        return [f"0x{n:x}" for n in sorted(values)
                if lower <= n < upper and n not in entry_addresses]

    # A first RET/early JMP does not bound a function; interleaved control
    # flow and padding may follow. Report termination as metadata only.
    has_terminator = any(x.mnemonic in TERMINATORS for x in ins)
    if ins and not has_terminator:
        issues.append("no_terminator_in_listing")
    status = ("REVIEW_NONCODE" if suspicious_ops or len(ascii_immediates) >= 2
              else "REVIEW_LISTING" if issues else "PARSED")
    return {
        "name": listing.name, "file": listing.file,
        "entry": f"0x{listing.entry:x}" if listing.entry is not None else None,
        "declared_instructions": listing.declared_instructions,
        "parsed_instructions": len(ins),
        "status": status,
        "issues": sorted(set(issues)),
        "suspect_opcodes": suspicious_ops,
        "ascii_immediates": ascii_immediates,
        "calls_to_named_entries": len(call_targets & entry_addresses),
        "calls_without_named_listing": [f"0x{n:x}" for n in sorted(calls_without_listing)],
        "candidate_data_direct": filter_data(direct_candidates),
        "candidate_data_reg_offset": filter_data(offset_candidates),
        "candidate_data_materialized": filter_data(materialized_candidates),
    }


def audit_corpus(directory: Path) -> dict:
    paths = sorted(directory.glob("*.asm"), key=lambda p: p.name.lower())
    if not paths:
        raise ValueError(f"No *.asm files found at {directory}")
    listings = [parse_listing(p.read_text(encoding="utf-8", errors="replace"), p.name)
                for p in paths]
    entries = {x.entry for x in listings if x.entry is not None}
    records = [analyze_listing(x, entries) for x in listings]
    counts = Counter(r["status"] for r in records)
    referenced = {x for r in records for x in r["calls_without_named_listing"]}
    return {
        "schema_version": 1, "input_files": len(paths), "parsed_named_entries": len(entries),
        "status_counts": dict(sorted(counts.items())),
        "distinct_call_targets_without_named_listing": len(referenced),
        "scope": ("Static listing quality and candidate addresses only; "
                  "not code extent, ELF readability, owner identity, "
                  "349-name debt classification, or semantic port parity"),
        "records": records,
    }


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("input_dir", type=Path, help="Local directory of .asm listings")
    parser.add_argument("--output", type=Path, required=True,
                        help="JSON report path; never writes to input directory by default")
    parser.add_argument("--fail-on-review", action="store_true",
                        help="Exit 2 when any listing needs inspection")
    args = parser.parse_args()
    if not args.input_dir.is_dir():
        parser.error(f"Not a directory: {args.input_dir}")
    if args.output.resolve().is_relative_to(args.input_dir.resolve()):
        parser.error("Refusing to write reports within source input tree")
    result = audit_corpus(args.input_dir)
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(result, ensure_ascii=False, indent=2) + "\n",
                           encoding="utf-8")
    print(f"LISTINGS={result['input_files']} STATUS={result['status_counts']} "
          f"UNLISTED_TARGETS={result['distinct_call_targets_without_named_listing']}")
    if args.fail_on_review and any(r["status"] != "PARSED" for r in result["records"]):
        return 2
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

# V9 audit tools — evidence first

These offline, Python-standard-library tools improve the quality of original
JX V9 reverse-engineering evidence. They do not port gameplay and do not
claim that any of Qoder's 349 UNKNOWN_HELPER items are automatically resolved.

## Disassembly listing quality

Run:

    python legacy_jx/tools/v9_disasm_quality_audit.py C:/v9_decompiled/disasm --output C:/temp/v9-quality.json

The input directory is read-only; the program only reads top-level .asm
files. A report gives parsed function names/addresses, anomalies, direct
call targets without named listings, and candidate data-global references.

A REVIEW_NONCODE flag means that decoded instructions could be data/string,
NOT that the full function has been proven invalid. PARSED only means a
listing has readable syntax, not validated semantics or body boundaries.
The tool NEVER uses the first ret to delimit a function. An unlisted call
target may be ordinary ELF code, a thunk or an import. The ELF is not
actually read by this tool, and no address is granted a gameplay owner.

Candidate data-address heuristics recognize direct memory operands,
register + immediate dereferences, and later dereferences of registers
loaded by immediate. LEA arithmetic is excluded. The observed original
image range 0x08000000..0x0A000000 is a heuristic and includes code.
This tool does not infer cross-function aliases, all writes, function
boundaries, or memory safety of decoded instructions.

Use --fail-on-review to return exit 2 if anything needs manual inspection.

## Historic debt CSV recheck

Use an actual census CSV containing name,grade and optional blocker_class,
where grade is STUB or NOEFFECT and every name is unique:

    python legacy_jx/tools/v9_debt_source_recheck.py --snapshot C:/temp/qoder-debt.csv --source-dir legacy_jx/gameserver/Sources/Core/Src --output C:/temp/debt-review.json

The program reads the current ScriptFuns_V9_*.cpp and reports source shape:
EXACT_TRIVIAL_RETURN, CONSTANT_NUMERIC_RESULT, NEEDS_V9_SEMANTIC_REVIEW,
SOURCE_DEFINITION_NOT_FOUND, or DUPLICATE_SOURCE_DEFINITIONS.

NEEDS_V9_SEMANTIC_REVIEW does NOT mean implemented correctly. A historic
STUB with a nontrivial body only receives snapshot_may_be_stale=true;
the historic grade is never rewritten. A constant result may be valid
for particular Lua APIs, so this is not automatic evidence of a bug.

## Test evidence (2026-10-21)

    python -m unittest discover -v -s legacy_jx/tests/v9_disasm -p "test_*.py"

Windows Python 3.14: 19/19 PASS. Ubuntu Python: 19/19 PASS.

Seven original .asm samples were read from Qoder's remote server
C:/Server-Game/SwordOnline/v9_decompiled/disasm without editing them.
They are stored only in the separate local scratch build directory
Document-ThietKe/_build/v9_asm_reference_20261021, NOT in GitHub.

Seven-file sample: 6 PARSED, 1 REVIEW_NONCODE (HostKillAnimal.asm with
outsd). There were 28 call targets without *named listings among these
seven files*. This is not a global corpus-wide missing-code claim.

Eight named debt items from Qoder's earlier report were checked against
the current GitHub-main C++ source: 5 trivial bodies and 3 constant
numeric result bodies. Notably SetRandSeed remains trivial on our
GitHub-main source, while Qoder's newer server source calls g_RandomSeed.
This difference requires separate ELF behavior verification before porting.

## Limitations and next steps

Qoder's 9.90 report cites 1432 listings but one inspected Qoder disasm
directory contained 1121 .asm files. Corpus provenance and missing
directories must be reconciled before a full audit.

The published 349 UNKNOWN_HELPER count is historic, from the 01:28
snapshot on 2026-10-21: 199 HELD_BLOCKED, 37 HELD_WIDE, 29 OPEN_EXCLUSIVE,
14 OPEN_SHARED, 70 OPEN_NO_CARRIER. The full original 349-row CSV has
not yet been located. This tooling does not regenerate or certify
that figure without authenticated source evidence.

Next verify actual ELF binary bytes and image hash; review the 25
suspected corrupted listings; identify anonymous global 0x082E8CA0
from original initializer/readers/writers; obtain a complete census
and reconcile Lua traffic before promoting any STUB to CALLABLE.

No server/client runtime, DB, production deployment, protocol-capture
or Windows x64 GameServer build is performed by these tools.

"""Tests for conservative, offline V9 assembly listing inspection."""
import importlib.util
import json
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest

SOURCE = Path(__file__).resolve().parents[2] / "tools" / "v9_disasm_quality_audit.py"
spec = importlib.util.spec_from_file_location("v9_quality", SOURCE)
audit = importlib.util.module_from_spec(spec)
sys.modules[spec.name] = audit
spec.loader.exec_module(audit)


def listing(name, entry, rows, declared=None):
    return "\n".join([
        f"; Function: {name}", f"; Virtual Address: 0x{entry:x}",
        f"; Instructions: {len(rows) if declared is None else declared}", "",
        *rows,
    ]) + "\n"


class TestV9DisasmQuality(unittest.TestCase):
    def test_real_header_and_alternate_header(self):
        src = listing("GetCityOwner", 0x80fff40, [
            "0x80fff40: push ebp", "0x80fff41: mov ebp, esp",
            "0x8100039: ret",
        ])
        x = audit.parse_listing(src, "GetCityOwner.asm")
        self.assertEqual((x.name, x.entry, len(x.instructions), x.issues),
                         ("GetCityOwner", 0x80fff40, 3, []))
        y = audit.parse_listing("; Name: Other\n; Address: 0x8111111\n0x8111111: ret\n",
                                "Other.asm")
        self.assertEqual((y.name, y.entry), ("Other", 0x8111111))

    def test_detect_bad_disasm_string_io(self):
        src = listing("HostKillAnimal", 0x8260515, [
            "0x8260515: dec eax",
            "0x8260516: outsd dx, dword ptr [esi]",
            "0x826051a: imul ebp, dword ptr [esp + ebp*2 + 0x42], 0x73736f",
            "0x8260527: imul ebp, dword ptr [esp + ebp*2 + 0x50], 0x6579616c",
        ])
        x = audit.analyze_listing(audit.parse_listing(src, "HostKillAnimal.asm"), set())
        self.assertEqual(x["status"], "REVIEW_NONCODE")
        self.assertIn("outsd", x["suspect_opcodes"])

    def test_two_printable_immediates(self):
        src = listing("ASCII", 0x8100000, [
            "0x8100000: mov eax, 0x64636261",
            "0x8100005: cmp eax, 0x68676665",
            "0x810000a: ret",
        ])
        x = audit.analyze_listing(audit.parse_listing(src, "ASCII.asm"), set())
        self.assertIn("multiple_printable_ascii_immediates", x["issues"])

    def test_global_carrier_candidate_grammars_and_lea_veto(self):
        src = listing("Owner", 0x80fff40, [
            "0x80fff40: lea esi, [ebx + 0x830a820]",
            "0x80fff44: mov eax, dword ptr [edx + 0x8bc99e8]",
            "0x80fff4a: add esi, 0x830a820",
            "0x80fff4c: mov eax, dword ptr [esi]",
            "0x80fff50: ret",
        ])
        x = audit.analyze_listing(audit.parse_listing(src, "Owner.asm"), set())
        self.assertEqual(x["candidate_data_reg_offset"], ["0x8bc99e8"])
        self.assertEqual(x["candidate_data_materialized"], ["0x830a820"])

    def test_function_address_not_auto_global(self):
        src = listing("Caller", 0x8100000, [
            "0x8100000: mov eax, dword ptr [0x80fff40]",
            "0x8100005: ret",
        ])
        x = audit.analyze_listing(audit.parse_listing(src, "Caller.asm"),
                                  {0x80fff40, 0x8100000})
        self.assertEqual(x["candidate_data_direct"], [])

    def test_unnamed_target_not_claimed_missing_binary_bytes(self):
        src = listing("Msg2Faction", 0x8120ec0, [
            "0x8120ec0: call 0x804b70c",
            "0x8120ec5: ret",
        ])
        x = audit.analyze_listing(audit.parse_listing(src, "Msg2Faction.asm"),
                                  {0x8120ec0})
        self.assertEqual(x["calls_without_named_listing"], ["0x804b70c"])
        self.assertEqual(x["status"], "PARSED")

    def test_no_first_ret_body_truncation(self):
        src = listing("Branch", 0x8100000, [
            "0x8100000: ret", "0x8100001: nop",
            "0x8100002: mov eax, 1", "0x8100007: jmp 0x8100002",
        ])
        self.assertEqual(len(audit.parse_listing(src, "Branch.asm").instructions), 4)

    def test_detect_mismatches_duplicate_and_unparsed(self):
        x = audit.parse_listing(listing("Bad", 0x8100000, [
            "0x8100001: mov eax, 1", "0x8100001: mov ebx, 2",
            "0x8100003: ???",
        ], declared=10), "Bad.asm")
        for problem in ("declared_instruction_count_mismatch",
                        "entry_address_mismatch",
                        "duplicate_or_unordered_instructions",
                        "unparsed_instruction_line"):
            self.assertIn(problem, x.issues)

    def test_cli_corpus_json_and_fail_on_review(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            asm = root / "asm"
            asm.mkdir()
            (asm / "A.asm").write_text(listing("A", 0x8100000, ["0x8100000: ret"]),
                                       encoding="utf-8")
            (asm / "B.asm").write_text(listing("B", 0x8100010, [
                "0x8100010: call 0x8100000", "0x8100015: call 0x8123456",
                "0x810001a: ret"]), encoding="utf-8")
            (asm / "C.asm").write_text(listing("C", 0x8100020, [
                "0x8100020: outsd dx, dword ptr [esi]"]), encoding="utf-8")
            report = root / "audit.json"
            x = subprocess.run([sys.executable, str(SOURCE), str(asm), "--output",
                                str(report)], capture_output=True, text=True)
            self.assertEqual(x.returncode, 0, x.stderr)
            data = json.loads(report.read_text(encoding="utf-8"))
            self.assertEqual(data["input_files"], 3)
            self.assertEqual(data["distinct_call_targets_without_named_listing"], 1)
            self.assertEqual(data["status_counts"]["REVIEW_NONCODE"], 1)
            x = subprocess.run([sys.executable, str(SOURCE), str(asm), "--output",
                                str(root / "strict.json"), "--fail-on-review"],
                               capture_output=True, text=True)
            self.assertEqual(x.returncode, 2)
            x = subprocess.run([sys.executable, str(SOURCE), str(asm), "--output",
                                str(asm / "forbidden.json")],
                               capture_output=True, text=True)
            self.assertNotEqual(x.returncode, 0)
            self.assertFalse((asm / "forbidden.json").exists())

    def test_empty_corpus_is_error(self):
        with tempfile.TemporaryDirectory() as tmp:
            with self.assertRaises(ValueError):
                audit.audit_corpus(Path(tmp))


if __name__ == "__main__":
    unittest.main()

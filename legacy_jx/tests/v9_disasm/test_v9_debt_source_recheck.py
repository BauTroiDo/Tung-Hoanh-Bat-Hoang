"""Tests for V9 snapshot-to-current-source reconciliation (no parity claims)."""
import csv
import importlib.util
import json
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest

SOURCE = Path(__file__).resolve().parents[2] / "tools" / "v9_debt_source_recheck.py"
spec = importlib.util.spec_from_file_location("v9_recheck", SOURCE)
mod = importlib.util.module_from_spec(spec)
sys.modules[spec.name] = mod
spec.loader.exec_module(mod)


def make_source(folder: Path, text: str, name: str = "ScriptFuns_V9_GeneralCoreA.cpp"):
    (folder / name).write_text(text, encoding="utf-8")


def make_snapshot(path: Path, pairs: list[tuple[str, str]]):
    with path.open("w", newline="", encoding="utf-8") as f:
        w = csv.writer(f)
        w.writerow(["name", "grade", "blocker_class"])
        for n, g in pairs:
            w.writerow([n, g, "UNKNOWN_HELPER"])


class DebtRecheckTests(unittest.TestCase):
    def test_balanced_body_skips_string_comments_and_nested_braces(self):
        data = 'int LuaSetRandSeed(Lua_State* L) {\n' \
               ' const char* s = "{ // ignored }"; /* } */\n' \
               ' if (1) { g_RandomSeed(5); }\n return 1;\n}\n'
        start = data.index("{")
        self.assertIn("return 1", mod.extract_block(data, start))

    def test_exact_zero_and_one(self):
        self.assertEqual(mod.shape("return 0;"), "EXACT_RETURN_ZERO")
        self.assertEqual(mod.shape("/* abc */ return 1; // comment"),
                         "EXACT_RETURN_ONE")

    def test_numeric_constant_shape(self):
        self.assertEqual(mod.shape("Lua_PushNumber(L, 1); return 1;"),
                         "CONSTANT_NUMERIC_RESULT")
        self.assertEqual(mod.shape(
            "Lua_PushNumber(L, 1); Lua_PushNumber(L, 0); return 2;"
        ), "CONSTANT_NUMERIC_RESULT")
        self.assertEqual(mod.shape("Lua_PushNumber(L, f()); return 1;"),
                         "NONTRIVIAL_SOURCE_SHAPE")

    def test_unbalanced_body_does_not_pass(self):
        self.assertIsNone(mod.extract_block("{ if (x) {", 0))
        self.assertEqual(mod.shape(None), "UNBALANCED_BODY")

    def test_reconcile_and_stale_candidate_not_auto_fixed(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            source = root / "src"
            source.mkdir()
            make_source(source, """
int LuaOpenCreateTask(Lua_State* L) { return 0; }
int LuaAddContribution(Lua_State* L) {
 Lua_PushNumber(L, 1); Lua_PushNumber(L, 0); return 2;
}
int LuaSetRandSeed(Lua_State* L) {
 if (Lua_GetTopIndex(L) > 0) g_RandomSeed((int)Lua_ValueToNumber(L, 1));
 return 1;
}
""")
            snapshot = root / "snapshot.csv"
            make_snapshot(snapshot, [
                ("OpenCreateTask", "STUB"), ("AddContribution", "NOEFFECT"),
                ("SetRandSeed", "STUB"), ("Missing", "STUB"),
            ])
            data = mod.reconcile(snapshot, source)
            by_name = {r["name"]: r for r in data["records"]}
            self.assertEqual(by_name["OpenCreateTask"]["source_verdict"],
                             "EXACT_TRIVIAL_RETURN")
            self.assertEqual(by_name["AddContribution"]["source_verdict"],
                             "CONSTANT_NUMERIC_RESULT")
            self.assertEqual(by_name["SetRandSeed"]["source_verdict"],
                             "NEEDS_V9_SEMANTIC_REVIEW")
            self.assertTrue(by_name["SetRandSeed"]["snapshot_may_be_stale"])
            self.assertEqual(by_name["Missing"]["source_verdict"],
                             "SOURCE_DEFINITION_NOT_FOUND")
            self.assertEqual(data["source_rows"], 4)

    def test_duplicate_source_is_flagged(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            make_source(root, "int LuaFoo(Lua_State* L) { return 0; }")
            make_source(root, "int LuaFoo(Lua_State* L) { return 1; }",
                        "ScriptFuns_V9_GeneralCoreB.cpp")
            csvpath = root / "debts.csv"
            make_snapshot(csvpath, [("Foo", "STUB")])
            result = mod.reconcile(csvpath, root)
            self.assertEqual(result["records"][0]["source_verdict"],
                             "DUPLICATE_SOURCE_DEFINITIONS")

    def test_reject_invalid_grade_and_duplicate_name(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            make_source(root, "int LuaFoo(Lua_State* L) { return 0; }")
            f = root / "s.csv"
            make_snapshot(f, [("Foo", "PASS")])
            with self.assertRaises(ValueError):
                mod.reconcile(f, root)
            make_snapshot(f, [("Foo", "STUB"), ("Foo", "NOEFFECT")])
            with self.assertRaises(ValueError):
                mod.reconcile(f, root)

    def test_cli_output_json_and_no_source_overwrite(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            source = root / "src"
            source.mkdir()
            make_source(source, "int LuaFoo(Lua_State* L) { return 0; }")
            f = root / "s.csv"
            make_snapshot(f, [("Foo", "STUB")])
            output = root / "out.json"
            ok = subprocess.run([sys.executable, str(SOURCE), "--snapshot", str(f),
                                 "--source-dir", str(source), "--output", str(output)],
                                capture_output=True, text=True)
            self.assertEqual(ok.returncode, 0, ok.stderr)
            self.assertEqual(json.loads(output.read_text())["source_rows"], 1)
            refused = subprocess.run([sys.executable, str(SOURCE), "--snapshot", str(f),
                                      "--source-dir", str(source), "--output",
                                      str(source / "unsafe.json")],
                                     capture_output=True, text=True)
            self.assertNotEqual(refused.returncode, 0)
            self.assertFalse((source / "unsafe.json").exists())

    def test_missing_cpp_inventory_errors(self):
        with tempfile.TemporaryDirectory() as tmp:
            with self.assertRaises(ValueError):
                mod.source_inventory(Path(tmp))


if __name__ == "__main__":
    unittest.main()

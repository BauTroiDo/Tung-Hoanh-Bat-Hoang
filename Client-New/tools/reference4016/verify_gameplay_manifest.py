"""CN02-BULK-03 regression: isolated gameplay batches and PE64/Ghidra coverage.

No recovered proprietary bodies are stored in committed JSON; raw outputs
stay in Tool/_outputs. Ghidra pseudo-code status is NOT semantic parity.
"""
import hashlib
import json
import re
import unittest
from pathlib import Path
from build_gameplay_manifest import ROOT, GAME, PRIORITY, EXPECTED_SHA, native_unwind_ranges, sha, assign_type
from run_manifest_batches import planned
BASE=GAME/"Tool/_outputs/cn02-bulk/gameplay-batches"
MANIFEST=BASE/"manifest-v1.json"
RUNS=BASE/"pilot-runs-v1"
GHLOG=BASE/"ghidra-dynamic-v3-auto/console.log"
SUMMARY=ROOT/"reports/evidence/cn02-4016-il2cpp/gameplay-manifest-summary.json"
GROUPS=("Movement","NPC","Combat","Navigation")
SAMPLES={"Movement":(0x821410,286),"NPC":(0xDFB950,695),
         "Combat":(0x7D28F0,520),"Navigation":(0xE841D0,344)}

def read_log(path):
    b=Path(path).read_bytes()
    if b[:2] in (bytes((255,254)),bytes((254,255))):return b.decode("utf-16")
    return b.decode("utf8",errors="replace")
def audit():
    meta=json.loads(MANIFEST.read_text(encoding="utf8"))
    gh=read_log(GHLOG)
    imagebase,unwind=native_unwind_ranges(GAME/"Client-3D-2/update/GameAssembly.dll")
    per={}
    for group in GROUPS:
        runpath=RUNS/group
        status=json.loads((runpath/"run.json").read_text(encoding="utf8"))
        types=[t["type"] for t in meta["recommended"][group]]
        dump=(runpath/"Dump0/dump.cs")
        body=dump.read_text(encoding="utf8")
        pseudo=body.count("C pseudo-code:")
        native=len(re.findall(r"(?m)^\s*// RVA: 0x",body))
        found_ids=len(re.findall(r"(?m)^.*// TypeDefIndex:",body))
        per[group]={
          "selectedTypes":types,
          "exitCode":status["exitCode"],
          "elapsedSeconds":status["elapsedSeconds"],
          "maxBodyBytes":status["maxBodyBytes"],
          "methodBodyMarkers":native,
          "pseudoMarkers":pseudo,
          "typeMarkers":found_ids,
          "suspectMethods":body.count("[recovery: suspect"),
          "unknownMarkers":body.count("__unknown"),
          "dumpBytes":dump.stat().st_size,
          "usedOnlyTypeFilters":"--pseudo-include-assembly" not in status["command"],
          "hashFence":status["binarySHA256"]==EXPECTED_SHA
        }
    rows=re.findall(r"CN02_BATCH_ROW rva=0x([0-9a-f]+) nativeSpan=(\d+) decodedBytes=(\d+) functionBodyBytes=(\d+) disasmAccepted=(true|false) decompiled=(true|false) chars=(\d+)",gh)
    # Decode decimal metrics (log is UTF16 when PowerShell redirects).
    samples={hex(int(r,16)):{"nativeSpan":int(n),"decodedBytes":int(decoded),
         "functionBodyBytes":int(fn),"disasmAccepted":accepted=="true",
         "decompiled":success=="true","chars":int(chars)}
         for r,n,decoded,fn,accepted,success,chars in rows}
    return {
       "stage":"CN02-BULK-03",
       "manifestSchema":meta["schema"],"originalBinarySha256":EXPECTED_SHA,
       "sqliteSha256":meta["sqliteSha256"],
       "nativeImageBase":hex(imagebase),
       "pdataEntries":len(unwind),
       "manifestSha256":sha(MANIFEST),
       "groups":per,
       "allBatchesExitZero":all(x["exitCode"]==0 for x in per.values()),
       "totalBodiesWithPseudo":sum(x["pseudoMarkers"] for x in per.values()),
       "totalDumpBytes":sum(x["dumpBytes"] for x in per.values()),
       "ghidraSamples":samples,
       "ghidraExitOk":"Import succeeded" in gh and "CN02_BATCH_END" in gh,
       "ghidraReadOnly":"Discarded file import due to readOnly option" in gh,
       "note":"Method body markers are C-like decompiler output, NOT validated working C# source or correct semantics. PData unwind spans are independently decoded but are not guaranteed full logical methods. CN02-04 remains BLOCKED."
    }

class BatchManifestTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.manifest=json.loads(MANIFEST.read_text(encoding="utf8"))
        cls.result=audit()
        cls.nativeBase,cls.unwind=native_unwind_ranges(GAME/"Client-3D-2/update/GameAssembly.dll")
    def test_01_hash(self):self.assertEqual(sha(GAME/"Client-3D-2/update/GameAssembly.dll"),EXPECTED_SHA)
    def test_02_schema(self):self.assertEqual(self.manifest["schema"],"CN02-BULK-03-v1")
    def test_03_native_pe_range_count(self):
        self.assertEqual(self.nativeBase,0x180000000)
        self.assertGreater(len(self.unwind),300000)
    def test_04_four_groups(self):self.assertEqual(set(self.manifest["recommended"]),set(GROUPS))
    def test_05_two_types_each(self):
        self.assertEqual({g:len(self.manifest["recommended"][g]) for g in GROUPS},dict.fromkeys(GROUPS,2))
    def test_06_8_unique_types(self):
        names=[x["type"] for group in GROUPS for x in self.manifest["recommended"][group]]
        self.assertEqual(len(set(names)),8)
    def test_07_classify_selected(self):
        for g in GROUPS:
            for t in self.manifest["recommended"][g]:
                self.assertEqual(assign_type(t["type"]),g)
    def test_08_native_exact_entrypoints(self):
        for g in GROUPS:
            for typ in self.manifest["recommended"][g]:
                for e in typ["exactEntrypoints"]:
                    self.assertEqual(self.unwind[e["rva"]]-e["rva"],e["bytes"])
                    self.assertLessEqual(e["bytes"],8192)
    def test_09_planner(self):
        self.assertEqual(len(planned(MANIFEST)),4)
    def test_10_all_batch_exit_zero(self):self.assertTrue(self.result["allBatchesExitZero"])
    def test_11_all_type_only(self):
        for row in self.result["groups"].values():self.assertTrue(row["usedOnlyTypeFilters"])
    def test_12_all_reference_hash_fenced(self):
        for row in self.result["groups"].values():self.assertTrue(row["hashFence"])
    def test_13_nonempty_native_pseudo(self):
        for row in self.result["groups"].values():
            self.assertGreater(row["methodBodyMarkers"],5)
            self.assertEqual(row["methodBodyMarkers"],row["pseudoMarkers"])
    def test_14_batch_dump_size_bounded(self):
        for row in self.result["groups"].values():self.assertLess(row["dumpBytes"],5_000_000)
    def test_15_ghidra_exact_four(self):
        self.assertEqual(set(self.result["ghidraSamples"]),{hex(r) for r,n in SAMPLES.values()})
    def test_16_ghidra_bounds(self):
        for r,n in SAMPLES.values():
            row=self.result["ghidraSamples"][hex(r)]
            self.assertEqual(row["nativeSpan"],n)
            self.assertEqual(row["decodedBytes"],n)
            self.assertEqual(row["functionBodyBytes"],n)
    def test_17_ghidra_ready(self):
        self.assertTrue(self.result["ghidraExitOk"])
        self.assertTrue(self.result["ghidraReadOnly"])
        self.assertTrue(all(v["decompiled"] and v["chars"]>100 for v in self.result["ghidraSamples"].values()))
    def test_18_no_semantics_claim(self):
        self.assertIn("NOT validated",self.result["note"])
        self.assertIn("BLOCKED",self.result["note"])
    def test_19_no_raw_code_in_summary(self):
        raw=json.dumps(self.result)
        self.assertNotIn("C pseudo-code:",raw)
        self.assertNotIn("0x008DFD10:",raw)
    def test_20_manifest_outside_git(self):
        self.assertNotIn(str(ROOT).lower(),str(MANIFEST).lower())
        self.assertTrue(MANIFEST.is_relative_to(GAME/"Tool/_outputs"))
    def test_21_recommendation_native_cap(self):
        self.assertLessEqual(self.manifest["maxSingleUnwindSpanBytes"],8192)
        self.assertLessEqual(self.manifest["maxNativeEntrypointsPerType"],100)
    def test_22_ghidra_auto_sample_is_from_manifest(self):
        from launch_ghidra_manifest import sample
        chosen=sample(MANIFEST)
        self.assertEqual({p["group"]:(p["rva"],p["bytes"]) for p in chosen},SAMPLES)
    def test_23_ghidra_auto_refuses_git_output(self):
        from launch_ghidra_manifest import execute
        with self.assertRaises(ValueError):
            execute(MANIFEST,ROOT/"should-never-appear",dry_run=True)
    def test_24_auto_ghidra_run_pass(self):
        from launch_ghidra_manifest import sample
        run=json.loads((BASE/"ghidra-dynamic-v3-auto/run.json").read_text(encoding="utf8"))
        self.assertEqual(run["exitCode"],0)
        self.assertEqual(run["sampleRvas"],sample(MANIFEST))
        self.assertEqual(run["sourceHash"],EXPECTED_SHA)

if __name__=="__main__":
    summary=audit()
    SUMMARY.write_text(json.dumps(summary,indent=2,ensure_ascii=False),encoding="utf8")
    print("MANIFEST_METRICS",summary["totalBodiesWithPseudo"],"pseudo markers;",
          len(summary["ghidraSamples"]),"Ghidra probes; groups",len(summary["groups"]))
    unittest.main(verbosity=2)

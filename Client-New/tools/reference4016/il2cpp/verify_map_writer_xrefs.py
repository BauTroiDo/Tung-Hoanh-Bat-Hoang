"""Fail-closed evidence checks for Assembly-CSharp-only native xref audit."""
import json
import unittest
from pathlib import Path

ROOT=Path(__file__).resolve().parents[3]
EVID=ROOT/"reports/evidence/cn02-4016-il2cpp"
class MapWriterAuditTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.doc=json.loads((EVID/"map-writer-xrefs.json").read_text(encoding="utf8"))
        cls.field=json.loads((EVID/"map-field-source.json").read_text(encoding="utf8"))
    def test_source_identity(self):
        self.assertEqual(self.doc["binarySHA256"],self.field["binarySHA256"])
        self.assertEqual(self.doc["metadataSHA256"],self.field["metadataSHA256"])
    def test_owned_assembly_method_pointers(self):
        self.assertEqual(self.doc["asmCsharpPointerSlots"],19668)
        self.assertGreater(self.doc["uniqueExecutablePointers"],16000)
    def test_map_service_methods_coverage(self):
        rows=self.doc["mapServiceScopedMethods"]
        self.assertEqual(len(rows),40)
        self.assertIn(["Calibrate"],[x["names"] for x in rows])
        self.assertIn([".cctor"],[x["names"] for x in rows])
        self.assertIn(["UpdateWorldInfo"],[x["names"] for x in rows])
    def test_calibrate_has_no_direct_call(self):
        self.assertEqual(self.doc["directRelativeTransferCounts"]["Calibrate"],0)
        self.assertEqual(self.doc["instructionBoundaryConfirmedCounts"]["Calibrate"],0)
    def test_update_world_info_single_proven_caller(self):
        callers=[v for v in self.doc["directRelativeTransferCandidates"] if v["target"]=="UpdateWorldInfo"]
        self.assertEqual(len(callers),1)
        call=callers[0]
        self.assertEqual(call["fromRVA"],"0xc38d1f")
        self.assertEqual(call["owningFunctionStartRVA"],"0xc38990")
        self.assertEqual(call["status"],"INSTRUCTION_BOUNDARY_CONFIRMED")
        self.assertEqual(call["ownerLabels"][0]["type"],"ProcessGameUnity")
        self.assertEqual(call["ownerLabels"][0]["method"],"OnSyncWorld")
    def test_known_coordinate_caller_positive_controls(self):
        counts=self.doc["instructionBoundaryConfirmedCounts"]
        self.assertEqual(counts["Mps2WorldPos"],74)
        self.assertEqual(counts["WorldPos2Mps"],109)
        self.assertEqual(counts["Map2WorldPos"],19)
        self.assertEqual(counts["Solve3x3"],2)
    def test_solve_calls_in_calibrate_body(self):
        row=next(x for x in self.doc["mapServiceScopedMethods"] if x["names"]==["Calibrate"])
        self.assertEqual([x["targetRVA"] for x in row["selectedCalls"]],["0x8e0870","0x8e0870"])
    def test_constructor_contains_original_six_writes(self):
        row=next(x for x in self.doc["mapServiceScopedMethods"] if x["names"]==[".cctor"])
        writes={x["fieldCandidate"] for x in row["fieldOffsetWrites"] if x["baseRegister"]=="rcx"}
        self.assertTrue({"m00","m01","m10","m11","offsetX","offsetZ"}.issubset(writes))
    def test_mapping_not_overwritten_by_other_assemblies(self):
        entries=[x for x in self.doc["directRelativeTransferCandidates"] if x["ownerLabels"]]
        self.assertGreater(len(entries),80)
        self.assertTrue(any(x["ownerLabels"][0]["type"]=="JXMovement" for x in entries))
        self.assertFalse(any(x["ownerLabels"][0]["type"]=="Avx" for x in entries))
    def test_update_world_info_does_not_prove_matrix_rewrite(self):
        row=next(x for x in self.doc["mapServiceScopedMethods"] if x["names"]==["UpdateWorldInfo"])
        self.assertEqual(row["rva"],"0x8e0dd0")
        # The sole same-offset store is rdi+0x10, not a verified MapService
        # staticFields pointer. Never label it an offsetX mutation.
        self.assertEqual([(x["baseRegister"],x["offset"]) for x in row["fieldOffsetWrites"]],[("rdi",16)])
        self.assertTrue(all("NOT PROOF" in x["warning"] for x in row["fieldOffsetWrites"]))
    def test_no_runtime_coefficients_claimed(self):
        self.assertIn("no live Map53 values",self.doc["constraints"])
        self.assertNotIn("Map53Verified",self.doc)
        self.assertNotIn("actualMap53AffineCoefficients",self.doc)

if __name__=="__main__":unittest.main(verbosity=2)

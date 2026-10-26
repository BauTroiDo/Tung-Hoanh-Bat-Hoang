"""Regression tests for static Calibrate dispatch evidence, not Map53 acceptance."""
import copy,json,math,unittest
from pathlib import Path

ROOT=Path(__file__).resolve().parents[3]
EVIDENCE=ROOT/"reports/evidence/cn02-4016-il2cpp"
class CalibrateDispatchTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.data=json.loads((EVIDENCE/"calibrate-dispatch.json").read_text(encoding="utf8"))
        cls.methods=json.loads((EVIDENCE/"map-field-source.json").read_text(encoding="utf8"))
        cls.module=json.loads((EVIDENCE/"pe-module-candidates.json").read_text(encoding="utf8"))
    def test_binary_identity(self):
        self.assertEqual(self.data["binarySHA256"],"011357720B210AA1F392A8B62FF8DE090D3A895230A6E0135BCE707196871069")
    def test_exact_native_target(self):
        self.assertEqual(self.data["method"],"MapService.Calibrate")
        self.assertEqual(self.data["targetNativeRVA"],"0x8DD750")
    def test_absolute_pointer_only_in_codegen_slot(self):
        self.assertEqual(len(self.data["absolutePointerOccurrences"]),1)
        self.assertEqual(self.data["methodPointerSlotRVA"],"0x5CD7B88")
        self.assertEqual(self.data["absolutePointerOccurrences"][0]["rva"],self.data["methodPointerSlotRVA"])
        self.assertEqual(self.data["absolutePointerOccurrences"][0]["section"],".data")
    def test_pointer_slot_corresponds_to_method_token(self):
        row=int(self.data["methodToken"],16)&0xFFFFFF
        pointer_start=int(self.module["candidates"][0]["methodArrayRVA"],16)
        self.assertEqual(pointer_start+8*(row-1),int(self.data["methodPointerSlotRVA"],16))
    def test_no_direct_transfer_patterns(self):
        self.assertEqual(self.data["directRelativeTransfers"],[])
        self.assertIn("indirect",self.data["limits"])
    def test_log_has_source_hash(self):
        self.assertEqual(len(self.data["probeLogSHA256"]),64)
        int(self.data["probeLogSHA256"],16)
    def test_legacy_probes_explicitly_unpaired(self):
        probes=self.data["unpairedSanityProbes"]
        self.assertEqual(len(probes),2)
        self.assertEqual([p["assumedMpsXY"] for p in probes],[[51889,102090],[51889,102105]])
        self.assertTrue(all(p["status"]=="ASSUMED_MPS_UNPAIRED_NOT_VALIDATION" for p in probes))
    def test_projection_is_not_independent_evidence(self):
        self.assertNotIn("legacyToUnityAnchorPairs",self.data)
        self.assertNotIn("verifiedMap53",self.data)
        self.assertTrue(all(not x["insideNominalStaticSceneEnvelope"] for x in self.data["unpairedSanityProbes"]))
    def test_projection_is_finite_and_caveated(self):
        self.assertTrue(all(all(math.isfinite(v) for v in p["defaultProjectedUnityXZ"]) for p in self.data["unpairedSanityProbes"]))
        self.assertIn("Nominal",self.data["referenceSceneNominalBounds"]["caveat"])
        self.assertIn("unverified",self.data["limits"])
    def test_calibration_values_not_proven_runtime(self):
        self.assertEqual(len(self.methods["mapServiceFields"]["constructorDefaults"]),6)
        self.assertFalse(self.data.get("map53RuntimeCoefficientsVerified",False))
if __name__=="__main__":unittest.main(verbosity=2)

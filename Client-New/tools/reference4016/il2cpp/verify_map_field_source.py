"""Independent metadata-only checks for original 4016 MapService writer evidence.

PASS here validates static provenance, NOT effective Map53 runtime mapping.
"""
import json
import math
import struct
import unittest
from pathlib import Path

ROOT=Path(__file__).resolve().parents[3]
REPORT=ROOT/"reports/evidence/cn02-4016-il2cpp/map-field-source.json"
SCENE=ROOT/"reports/evidence/cn02-4016-static/reference-tile-origin-candidates.json"
FIELD_NAMES=["m00","m01","m10","m11","offsetX","offsetZ"]
DEFAULT_BITS=["0xC01A0275","0x40656042","0x4079096C","0x4054FDF4","0xC361851F","0xC4B33AE1"]
class MapFieldSourceEvidenceTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.data=json.loads(REPORT.read_text(encoding="utf8"))
        cls.info=cls.data["mapServiceFields"]
        cls.defaults=cls.info["constructorDefaults"]
        cls.methods=[x for x in cls.data["targets"] if x["owner"]=="MapService"]
    def test_source_file_fingerprints(self):
        self.assertEqual(self.data["binarySHA256"],"011357720B210AA1F392A8B62FF8DE090D3A895230A6E0135BCE707196871069")
        self.assertEqual(self.data["metadataSHA256"],"5586F416AA26EBE57EA7F6580D0A53E5E5F29C50E425E64A4D3F2A95D6069E08")
    def test_original_type_and_field_slice(self):
        self.assertEqual(self.info["mapServiceTypeIndex"],637)
        self.assertEqual(self.info["fieldFirstIndex"],5555)
        self.assertEqual(self.info["fieldLastIndex"],5588)
        self.assertEqual(self.info["fieldNames"][:6],FIELD_NAMES)
        self.assertIn("_mpsWorldScale",self.info["fieldNames"])
    def test_exact_six_constructor_defaults(self):
        self.assertEqual(len(self.defaults),6)
        self.assertEqual([x["field"] for x in self.defaults],FIELD_NAMES)
        self.assertEqual([x["rawWord"] for x in self.defaults],DEFAULT_BITS)
    def test_field_offsets_not_overlapping(self):
        self.assertEqual([x["staticFieldOffset"] for x in self.defaults],[0,4,8,12,16,20])
        self.assertEqual(len({x["writerRVA"] for x in self.defaults}),6)
    def test_float32_bit_preservation(self):
        for field in self.defaults:
            word=struct.unpack("<I",struct.pack("<f",field["float32"]))[0]
            self.assertEqual(f"0x{word:08X}",field["rawWord"])
    def test_calibrate_and_solver_references(self):
        methods={x["name"]:x for x in self.methods}
        self.assertEqual(methods["Calibrate"]["nativeRVA"],"0x8DD750")
        self.assertEqual(methods["Solve3x3"]["nativeRVA"],"0x8E0870")
        self.assertEqual(methods[".cctor"]["nativeRVA"],"0x8E1B40")
        self.assertEqual(self.info["calibrateSolveCallsites"],["0x8DD904","0x8DD99E"])
    def test_only_direct_calls_were_scanned(self):
        audit=self.data["calibrateCallerAudit"]
        self.assertEqual(audit["targetRVA"],"0x8DD750")
        self.assertEqual(audit["rawE8Candidates"],0)
        self.assertEqual(audit["callsites"],[])
        self.assertIn("indirect",audit["limits"])
    def test_default_matrix_nonsingular(self):
        a,b,c,d,ox,oz=[x["float32"] for x in self.defaults]
        det=a*d-b*c
        self.assertTrue(math.isfinite(det))
        self.assertGreater(abs(det),1.0)
    def test_roundtrip_fixture_only(self):
        a,b,c,d,ox,oz=[x["float32"] for x in self.defaults]
        for x,y in ((0,0),(10,20),(52032,101696)):
            u,v=x/512.0,y/512.0
            wx=a*u+b*v+ox
            wz=c*u+d*v+oz
            det=a*d-b*c
            rx=512*(d*(wx-ox)-b*(wz-oz))/det
            ry=512*(a*(wz-oz)-c*(wx-ox))/det
            self.assertAlmostEqual(rx,x,places=6)
            self.assertAlmostEqual(ry,y,places=6)
    def test_no_claim_of_map53_runtime_coefficients(self):
        self.assertNotIn("Map53Verified",self.data)
        self.assertIn("Global defaults only",self.info["caveat"])
        scene=json.loads(SCENE.read_text(encoding="utf8"))
        self.assertEqual(scene["independentReferenceToLegacyPairs"],0)
        self.assertTrue(all(p["legacyMap53XY"] is None for p in scene["sceneCandidates"]))
if __name__=="__main__":
    unittest.main(verbosity=2)

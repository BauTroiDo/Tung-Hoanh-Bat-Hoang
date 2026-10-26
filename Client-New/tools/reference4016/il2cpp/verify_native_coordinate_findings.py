"""Regression assertions on static native coordinate-analysis evidence.

No DLL executed; asserts only identities and bounded observed instruction patterns.
"""
import json
from pathlib import Path
import unittest
ROOT=Path(__file__).resolve().parents[3]
DATA=ROOT/"reports/evidence/cn02-4016-il2cpp"
def load(name):return json.loads((DATA/name).read_text(encoding="utf8"))
class Native4016Evidence(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.header=load("header-v39.json")
        cls.owner=load("method-owner-catalog.json")
        cls.module=load("pe-module-candidates.json")
        cls.native=load("native-method-candidates.json")
    def target(self,owner,name):
        rows=[r for r in self.native["targets"] if r["ownerType"]["name"]==owner and r["name"]==name]
        self.assertEqual(len(rows),1)
        return rows[0]
    def test_metadata_identity(self):
        self.assertEqual(self.header["version"],39)
        self.assertEqual(len(self.header["sections"]),31)
        self.assertEqual(self.owner["typeCount"],21485)
        self.assertEqual(self.owner["methodCount"],161267)
    def test_owner_image_coverage(self):
        self.assertEqual(len(self.owner["images"]),122)
        self.assertEqual(sum(x["typeCount"] for x in self.owner["images"]),21485)
        self.assertTrue(all(r["image"]["name"]=="Assembly-CSharp.dll" for r in self.owner["targetMethods"]))
    def test_binary_identity(self):
        self.assertEqual(self.module["sha256"],"011357720B210AA1F392A8B62FF8DE090D3A895230A6E0135BCE707196871069")
        self.assertEqual(self.native["binarySHA256"],self.module["sha256"])
    def test_codegen_module_structural_uniqueness(self):
        self.assertEqual(self.module["searchStatus"],"ONE_STRUCTURAL_CANDIDATE")
        self.assertEqual(self.module["candidates"][0]["methodPointerCount"],19668)
    def test_method_tokens_to_executable_addresses(self):
        names=[("MapService","Mps2WorldPos","0x8DFD10"),("MapService","WorldPos2Mps","0x8E18B0"),
               ("MapService","get_MpsWorldScale","0x8E2140"),("MapConverter","MapToWorldPos","0xA1FBB0")]
        for owner,name,rva in names:
            with self.subTest(owner=owner,name=name):
                row=self.target(owner,name)
                self.assertEqual(row["nativeRVA"],rva)
                self.assertEqual(row["status"],"STATIC_CODE_CANDIDATE")
    def test_map_converter_normalizes_two_axes(self):
        ins=self.target("MapConverter","MapToWorldPos")["instructions"]
        ops=[(i["mnemonic"],i["operands"]) for i in ins]
        self.assertEqual(sum(op=="divss" for op,arg in ops),2)
        self.assertEqual(sum(op=="mulss" for op,arg in ops),2)
        self.assertTrue(any(op=="mov" and "[rcx + 4], 0" in arg for op,arg in ops))
        self.assertEqual(ops[-1][0],"ret")
    def test_subworld_shift_sizes(self):
        rows=[r for r in self.native["targets"] if r["ownerType"]["name"]=="SubWorld" and r["name"]=="Map2Mps"]
        self.assertEqual(len(rows),2)
        selected=next(r for r in rows if r["nativeRVA"]=="0xC790B0")
        self.assertTrue(any(i["mnemonic"]=="sar" and i["operands"]=="eax, 0xa" for i in selected["instructions"]))
        self.assertGreaterEqual(sum("shl"==i["mnemonic"] and ", 5" in i["operands"] for i in selected["instructions"]),4)
    def test_inverse_has_512_constant_candidate(self):
        rows=self.target("MapService","WorldPos2Mps")["possibleRipRelativeFloats"]
        self.assertTrue(any(x["width"]==4 and x["float"]==512.0 for x in rows))
    def test_forward_has_inverse_512_constant_candidate(self):
        rows=self.target("MapService","Mps2WorldPos")["possibleRipRelativeFloats"]
        self.assertTrue(any(x["width"]==4 and x["float"]==0.001953125 for x in rows))
    def test_no_unverified_calibration_claim(self):
        self.assertNotIn("legacyToUnityVerified",self.native)
        self.assertEqual(self.native["moduleStatus"],"ONE_STRUCTURAL_CANDIDATE")
if __name__=="__main__":unittest.main(verbosity=2)

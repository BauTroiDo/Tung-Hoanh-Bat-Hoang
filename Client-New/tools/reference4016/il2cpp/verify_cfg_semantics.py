"""Evidence-based checks for 4016 MapService affine coordinate code.

Numerical fixtures are synthetic proof of inferred equations, NOT Map53 anchor
observations, and do not establish native rounding behavior.
"""
import json
import math
import unittest
from pathlib import Path

ROOT=Path(__file__).resolve().parents[3]
EVIDENCE=ROOT/"reports/evidence/cn02-4016-il2cpp/cfg-semantics.json"
BASELINE=ROOT/"reports/evidence/cn02-4016-il2cpp/native-method-candidates.json"

def forward(mps_x,mps_y,mat):
    a,b,c,d,tx,tz=mat
    u=mps_x/512.0;v=mps_y/512.0
    return a*u+b*v+tx,c*u+d*v+tz

def inverse(world_x,world_z,mat):
    a,b,c,d,tx,tz=mat
    det=a*d-b*c
    if not math.isfinite(det) or abs(det)<1e-10:
        raise ValueError("singular coordinate map")
    dx=world_x-tx;dz=world_z-tz
    return 512*(d*dx-b*dz)/det,512*(a*dz-c*dx)/det

class StaticMap4016Tests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.data=json.loads(EVIDENCE.read_text(encoding="utf8"))
        cls.earlier=json.loads(BASELINE.read_text(encoding="utf8"))
        cls.rows=cls.data["results"]
    def row(self,name,rva=None):
        rows=[x for x in self.rows if x["method"]==name and (rva is None or x["nativeRVA"]==rva)]
        self.assertEqual(len(rows),1)
        return rows[0]
    def text(self,name):
        return [(i["op"],i["args"]) for i in self.row(name)["instructions"]]
    def test_binary_identity(self):
        self.assertEqual(self.data["binarySHA256"],"011357720B210AA1F392A8B62FF8DE090D3A895230A6E0135BCE707196871069")
        self.assertGreater(self.data["unwindFunctionCount"],300000)
    def test_forward_frame_reaches_return(self):
        r=self.row("MapService.Mps2WorldPos")
        self.assertEqual(r["nativeRVA"],"0x8DFD10")
        self.assertTrue(r["hasReturn"])
        self.assertEqual(r["instructionCount"],52)
    def test_forward_matrix_columns_offsets_height_and_constant(self):
        ops=self.text("MapService.Mps2WorldPos")
        mul=[args for mnemonic,args in ops if mnemonic=="mulss"]
        self.assertTrue(all(any("[rax"+off+"]" in operand for operand in mul)
                            for off in ("", " + 4", " + 8", " + 0xc")))
        self.assertTrue(any(mnemonic=="addss" and "[rax + 0x10]" in args for mnemonic,args in ops))
        self.assertTrue(any(mnemonic=="addss" and "[rax + 0x14]" in args for mnemonic,args in ops))
        self.assertTrue(any(mnemonic=="movss" and "[rbx + 4]" in args for mnemonic,args in ops))
        rows=self.row("MapService.Mps2WorldPos")["ripFloatReferences"]
        self.assertEqual(sum(x["float32"]==1.0/512 for x in rows),2)
    def test_inverse_has_determinant_offsets_and_512(self):
        ops=self.text("MapService.WorldPos2Mps")
        self.assertGreaterEqual(sum(mnemonic=="divss" for mnemonic,args in ops),2)
        self.assertTrue(any(mnemonic=="subss" and "[rax + 0x10]" in args for mnemonic,args in ops))
        self.assertTrue(any(mnemonic=="subss" and "[rax + 0x14]" in args for mnemonic,args in ops))
        self.assertEqual(sum(x["float32"]==512.0 for x in self.row("MapService.WorldPos2Mps")["ripFloatReferences"]),2)
    def test_subworld_then_forward_is_real_call_chain(self):
        edges=self.row("MapService.Map2WorldPos")["branchAndCallEdges"]
        calls=[x["target"] for x in edges if x["kind"]=="call"]
        self.assertIn("0xC790F0",calls)
        self.assertIn("0x8DFD10",calls)
    def test_static_field_matrix_pointer(self):
        a=self.text("MapService.Mps2WorldPos")
        b=self.text("MapService.WorldPos2Mps")
        self.assertTrue(any(op=="mov" and "[rcx + 0xb8]" in x for op,x in a) or any(op=="mov" and "[rcx + 0xb8]" in x for op,x in b))
        self.assertTrue(any(op=="mov" and "[rdx + 0xb8]" in x for op,x in b))
    def test_world_scale_derived_from_both_matrix_columns(self):
        row=next(x for x in self.earlier["targets"] if x["ownerType"]["name"]=="MapService" and x["name"]=="get_MpsWorldScale")
        ins=row["instructions"]
        self.assertGreaterEqual(sum(x["mnemonic"]=="sqrtss" for x in ins),3)
        self.assertTrue(any(x["mnemonic"]=="movss" and "[rax + 0x48]" in x["operands"] for x in ins))
        self.assertEqual(sum(v["width"]==4 and v["float"]==1.0/512 for v in row["possibleRipRelativeFloats"]),2)
    def test_round_helper_is_unresolved(self):
        calls=[x for x in self.row("MapService.WorldPos2Mps")["branchAndCallEdges"] if x["kind"]=="call" and x["target"]=="0x740CE0"]
        self.assertEqual(len(calls),2)
    def test_invertible_synthetic_roundtrips(self):
        for mat in ((1,0,0,1,0,0),(10,0,0,20,125,-80),(0,-15,15,0,-300,250),(2,4,-3,7,1000,-500)):
            for mps in ((0,0),(52032,101696),(-2048,1599)):
                with self.subTest(matrix=mat,mps=mps):
                    result=inverse(*forward(*mps,mat),mat)
                    self.assertAlmostEqual(result[0],mps[0],places=7)
                    self.assertAlmostEqual(result[1],mps[1],places=7)
    def test_reject_singular_fixture(self):
        with self.assertRaisesRegex(ValueError,"singular"):
            inverse(2,3,(1,2,2,4,0,0))
    def test_no_map53_parameters_fabricated(self):
        self.assertNotIn("map53Coefficients",self.data)
        self.assertIn("no validated 3+1",self.data["purpose"])

if __name__=="__main__":unittest.main(verbosity=2)

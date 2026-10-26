"""Native instruction invariants for original 4016 world-sync contract.

These tests validate static packet-offset provenance and caller/callee edges,
not live execution, complete SubWorld.LoadMap semantics or Map53 calibration.
"""
import json, unittest
from pathlib import Path
ROOT=Path(__file__).resolve().parents[3]
EVIDENCE=ROOT/"reports/evidence/cn02-4016-il2cpp"
class WorldSyncContractTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.data=json.loads((EVIDENCE/"worldsync-contract.json").read_text(encoding="utf8"))
        cls.writer=json.loads((EVIDENCE/"map-writer-xrefs.json").read_text(encoding="utf8"))
        cls.a=cls.data["results"]["ProcessGameUnity.OnSyncWorld"]
        cls.b=cls.data["results"]["MapService.UpdateWorldInfo"]
        cls.load=cls.data["results"]["SubWorld.LoadMap"]
        cls.activate=cls.data["results"]["SubWorld.Activate"]
    def exact_call(self,row,name,location):
        target=[x for x in row["calls"] if name in x["knownMethodNames"]]
        self.assertEqual(len(target),1)
        self.assertEqual(target[0]["atRVA"],location)
        return target[0]
    def entry_ops(self,row):return [(x["op"],x["args"]) for x in row["entryInstructions"]]
    def ctx_ops(self,target):
        c=[x for x in self.b["subworldCallsiteContexts"] if x["targetRVA"]==target]
        self.assertEqual(len(c),1)
        return [(x["op"],x["args"]) for x in c[0]["instructions"]]
    def test_original_source_sha(self):
        self.assertEqual(self.data["binarySHA256"],self.writer["binarySHA256"])
        self.assertEqual(self.data["metadataSHA256"],self.writer["metadataSHA256"])
    def test_four_method_owners(self):
        self.assertEqual({k:x["rva"] for k,x in self.data["results"].items()},
            {"ProcessGameUnity.OnSyncWorld":"0xc38990","MapService.UpdateWorldInfo":"0x8e0dd0",
             "SubWorld.LoadMap":"0xc75de0","SubWorld.Activate":"0xc753c0"})
    def test_world_sync_receives_pointer_as_second_native_argument(self):
        self.assertIn(("mov","rdi, rdx"),self.entry_ops(self.a))
        self.assertIn(("movups","xmm0, xmmword ptr [rdi + 0x10]"),self.entry_ops(self.a))
    def test_world_sync_statically_copies_two_128bit_windows(self):
        values=[(x["op"],x["args"]) for x in self.a["worldInfoCallsiteContext"]]
        self.assertIn(("movups","xmm1, xmmword ptr [rdi]"),values)
        self.assertIn(("movups","xmm0, xmmword ptr [rdi + 0x10]"),values)
    def test_on_sync_world_calls_update_with_local_copy(self):
        self.exact_call(self.a,"MapService.UpdateWorldInfo","0xc38d1f")
        ops=[(x["op"],x["args"]) for x in self.a["worldInfoCallsiteContext"]]
        self.assertIn(("lea","rcx, [rsp + 0x40]"),ops)
        self.assertIn(("xor","edx, edx"),ops)
    def test_exchange_network_and_toast_calls(self):
        self.exact_call(self.a,"GameServerNetwork.NotifyExchangeWorldSynced","0xc38ca5")
        self.exact_call(self.a,"ToastManager.ChangeWorld","0xc38d53")
    def test_update_world_info_contains_map_load_activate(self):
        self.exact_call(self.b,"SubWorld.LoadMap","0x8e1094")
        self.exact_call(self.b,"SubWorld.Activate","0x8e10e8")
    def test_update_world_info_resets_path_npc_state(self):
        self.exact_call(self.b,"NPCInfoService.ResetServerPosTracker","0x8e0f8e")
        self.exact_call(self.b,"GameManager.ForceCancelPathFinding","0x8e0fc1")
    def test_map_load_argument_offset_4(self):
        values=self.ctx_ops("0xc75de0")
        self.assertIn(("movups","xmm0, xmmword ptr [rbx]"),values)
        self.assertIn(("movq","r8, xmm0"),values)
        self.assertIn(("shr","r8, 0x20"),values)
    def test_map_load_argument_offset_8(self):
        values=self.ctx_ops("0xc75de0")
        self.assertIn(("psrldq","xmm1, 8"),values)
        self.assertIn(("movd","r9d, xmm1"),values)
    def test_map_load_argument_offset_20(self):
        values=self.ctx_ops("0xc75de0")
        self.assertIn(("movups","xmm2, xmmword ptr [rbx + 0x10]"),values)
        self.assertIn(("movq","rdx, xmm2"),values)
        self.assertIn(("shr","rdx, 0x20"),values)
    def test_activate_follows_load_and_clears_second_arg(self):
        values=self.ctx_ops("0xc753c0")
        self.assertIn(("xor","edx, edx"),values)
        self.assertLess(self.b["calls"].index(next(x for x in self.b["calls"] if "SubWorld.LoadMap" in x["knownMethodNames"])),
                        self.b["calls"].index(next(x for x in self.b["calls"] if "SubWorld.Activate" in x["knownMethodNames"])))
    def test_activate_unwind_fragment_not_complete_function(self):
        self.assertEqual(self.activate["unwindLengthBytes"],25)
        self.assertEqual(self.activate["decodedInstructionCount"],8)
    def test_no_map_calibration_claim(self):
        self.assertNotIn("runtimeMap53Coefficients",self.data)
        self.assertIn("no object type/parameter schema claimed",self.data["limitations"])
        self.assertEqual(self.writer["instructionBoundaryConfirmedCounts"]["Calibrate"],0)
if __name__=="__main__":unittest.main(verbosity=2)

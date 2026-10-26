"""Static evidence regression for 4016 SubWorld.LoadMap map-vs-template selection.

No test here exercises Unity, a live 4016 client or a 3+1 landmark gate.
"""
import json,unittest
from pathlib import Path
ROOT=Path(__file__).resolve().parents[3]
E=ROOT/"reports/evidence/cn02-4016-il2cpp"

class SelectorTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.d=json.loads((E/"loadmap-selector.json").read_text(encoding="utf8"))
        cls.native=json.loads((E/"worldsync-contract.json").read_text(encoding="utf8"))
        cls.fields=json.loads((E/"worldsync-field-types.json").read_text(encoding="utf8"))
        cls.sigs={x["method"]:x["params"] for x in cls.d["calledMethodSignatures"]}
        cls.ctx={x["names"][0]:x for x in cls.d["semanticCallContexts"]
                 if len(x["names"])==1 and x["names"][0] in (
                 "MapDataService.LoadForTemplate","GameManager.LoadScene",
                 "NPCManager.OnMapTemplateChanged")}
        cls.getters={x["name"]:x["instruction"] for x in cls.d["originalSubWorldFieldAccessors"]}
    def ins(self,method):
        return [(x["mnemonic"],x["operands"]) for x in self.ctx[method]["instructions"]]
    def test_original_sha(self):
        self.assertEqual(self.d["binarySHA256"],self.native["binarySHA256"])
        self.assertEqual(self.d["metadataSHA256"],self.native["metadataSHA256"])
    def test_full_native_scope_has_two_unwind_fragments(self):
        f=self.d["unwindFragments"]
        self.assertEqual([(x["startRVA"],x["endRVAExclusive"]) for x in f],
                         [("0xc75de0","0xc75fc5"),("0xc75fc5","0xc790a4")])
        self.assertEqual([x["bytes"] for x in f],[485,12511])
        self.assertEqual(sum(x["instructionCount"] for x in f),2925)
    def test_no_additional_method_pointer_in_loadmap_continuation(self):
        self.assertEqual(self.d["methodPointerConflicts"],[])
        self.assertEqual(self.d["outOfRangeBranches"],[])
    def test_entry_preserves_distinct_formal_arguments(self):
        ops=[(x["mnemonic"],x["operands"]) for x in self.d["entryInstructions"]]
        self.assertIn(("mov","edi, edx"),ops)
        self.assertIn(("mov","r14d, r8d"),ops)
        self.assertIn(("mov","rsi, rcx"),ops)
    def test_original_subworld_id_accessor(self):
        self.assertEqual(self.getters["SubWorld.get_SubWorldID"],"mov eax, dword ptr [rcx + 0x1c]")
        self.assertEqual(self.getters["SubWorld.set_SubWorldID"],"mov dword ptr [rcx + 0x1c], edx")
    def test_original_map_template_accessor(self):
        self.assertEqual(self.getters["SubWorld.get_MapTemplateID"],"mov eax, dword ptr [rcx + 0x20]")
        self.assertEqual(self.getters["SubWorld.set_MapTemplateID"],"mov dword ptr [rcx + 0x20], edx")
    def test_assign_subworld_from_nmapid(self):
        self.assertIn({"rva":"0xc7628f","mnemonic":"mov",
            "operands":"dword ptr [rsi + 0x1c], r14d"},self.d["subWorldInstanceOffsetSamples"])
    def test_assign_template_from_ntemplateid(self):
        self.assertIn({"rva":"0xc76326","mnemonic":"mov",
            "operands":"dword ptr [rsi + 0x20], edi"},self.d["subWorldInstanceOffsetSamples"])
    def test_data_service_signature(self):
        self.assertEqual([x["name"] for x in self.sigs["MapDataService.LoadForTemplate"]],["templateId"])
    def test_scene_service_signature(self):
        self.assertEqual([x["name"] for x in self.sigs["GameManager.LoadScene"]],["mapId"])
    def test_npc_template_service_signature(self):
        self.assertEqual([x["name"] for x in self.sigs["NPCManager.OnMapTemplateChanged"]],["newTemplateId"])
    def test_data_pack_receives_template_not_mapid(self):
        self.assertIn(("mov","ecx, edi"),self.ins("MapDataService.LoadForTemplate"))
        self.assertEqual(self.ctx["MapDataService.LoadForTemplate"]["atRVA"],"0xc762ac")
    def test_scene_receives_template_not_mapid(self):
        self.assertIn(("mov","edx, edi"),self.ins("GameManager.LoadScene"))
        self.assertEqual(self.ctx["GameManager.LoadScene"]["atRVA"],"0xc7638a")
    def test_npc_manager_receives_template(self):
        self.assertIn(("mov","edx, edi"),self.ins("NPCManager.OnMapTemplateChanged"))
        self.assertEqual(self.ctx["NPCManager.OnMapTemplateChanged"]["atRVA"],"0xc76170")
    def test_cleanup_before_new_map(self):
        known={x["knownNames"][0]:int(x["atRVA"],16) for x in self.d["knownMethodCalls"]
               if len(x["knownNames"])==1}
        self.assertLess(known["MapDataService.UnloadPack"],known["MapDataService.LoadForTemplate"])
        self.assertLess(known["MapDataService.LoadForTemplate"],known["GameManager.LoadScene"])
    def test_two_region_load_sites(self):
        matches=[x for x in self.d["knownMethodCalls"] if "KRegion.Load" in x["knownNames"]]
        self.assertEqual([x["atRVA"] for x in matches],["0xc76410","0xc765a6"])
    def test_template_map_identity_not_assumed(self):
        names={x["callArgument"]:x["sourceField"] for x in self.fields["nativeLoadMapBindings"][:3]}
        self.assertEqual(names["nTemplateId"],"MapCopyID")
        self.assertEqual(names["nMapId"],"SubWorld")
        synthetic={"SubWorld":53,"MapCopyID":811}
        self.assertNotEqual(synthetic[names["nTemplateId"]],synthetic[names["nMapId"]])
    def test_one_hop_calls_are_bounded_not_scene_resolve_verified(self):
        direct=self.d["firstHopSceneAndPackDispatch"]
        self.assertIn("0x8c9bf0",direct);self.assertIn("0x918b70",direct)
        self.assertLess(direct["0x8c9bf0"]["insnCount"],100)
        self.assertLess(direct["0x918b70"]["insnCount"],100)
        self.assertIn("Static disassembly cannot independently prove Map53 visual scene",self.d["limitations"])
    def test_no_runtime_acceptance(self):
        self.assertNotIn("map53AffineCoefficients",self.d)
        self.assertNotIn("passed3Plus1",self.d)
        self.assertIn("NOT_RUNTIME_ACCEPTANCE",self.fields["historicalMap53HarnessObservation"]["gate"])
if __name__=="__main__": unittest.main(verbosity=2)

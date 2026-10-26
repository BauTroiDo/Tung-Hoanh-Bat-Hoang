"""Metadata/native cross-validation of WORLD_SYNC -> SubWorld.LoadMap.

A PASS does not certify packet framing, original scene calibration, or live
gameplay. No source/client runtime mutation.
"""
from pathlib import Path
import json,unittest

ROOT=Path(__file__).resolve().parents[3]
E=ROOT/"reports/evidence/cn02-4016-il2cpp"
class WorldSyncTypesTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.doc=json.loads((E/"worldsync-field-types.json").read_text(encoding="utf8"))
        cls.native=json.loads((E/"worldsync-contract.json").read_text(encoding="utf8"))
        cls.world=cls.doc["resolvedWorldSyncType"]
        cls.methods={(m["owner"],m["name"]):m for m in cls.doc["methodDefinitions"]}
        cls.p=cls.doc["nativeLoadMapBindings"]
    def test_source_identity(self):
        self.assertEqual(self.doc["sourceSHA256"],self.native["metadataSHA256"])
        self.assertEqual(self.doc["sourceSHA256"],"5586F416AA26EBE57EA7F6580D0A53E5E5F29C50E425E64A4D3F2A95D6069E08")
    def test_original_world_sync_struct_identity(self):
        self.assertEqual(self.world["name"],"WORLD_SYNC")
        self.assertEqual(self.world["namespace"],"Network.Header")
        self.assertEqual(self.world["typeDefinitionIndex"],13018)
        self.assertEqual(self.world["byvalTypeRegistryIndex"],52052)
    def test_world_sync_has_original_member_names(self):
        self.assertEqual([f["name"] for f in self.world["fields"]],
            ["ProtocolType","SubWorld","Region","Weather","Frame","MapCopyID","szName"])
    def test_world_sync_field_indices_are_contiguous(self):
        self.assertEqual([x["fieldIndex"] for x in self.world["fields"]],list(range(67390,67397)))
    def test_on_sync_world_type_exact(self):
        p=self.methods[("ProcessGameUnity","OnSyncWorld")]["parameters"]
        self.assertEqual(len(p),1);self.assertEqual(p[0]["name"],"data")
        self.assertEqual(p[0]["typeIndex"],self.world["byvalTypeRegistryIndex"])
    def test_update_world_info_same_original_type(self):
        p=self.methods[("MapService","UpdateWorldInfo")]["parameters"]
        self.assertEqual(len(p),1);self.assertEqual(p[0]["name"],"data")
        self.assertEqual(p[0]["typeIndex"],self.world["byvalTypeRegistryIndex"])
    def test_original_loader_exact_parameter_names(self):
        p=self.methods[("SubWorld","LoadMap")]["parameters"]
        self.assertEqual([x["name"] for x in p],
            ["nTemplateId","nMapId","nRegion","nMpsX","nMpsY"])
        self.assertEqual(len({x["typeIndex"] for x in p}),1)
    def test_three_dword_sources_share_registry_type(self):
        f={x["name"]:x for x in self.world["fields"]}
        self.assertEqual([f[n]["typeRegistryIndex"] for n in ("SubWorld","Region","MapCopyID")],
                         [40360,40360,40360])
    def test_loader_template_is_map_copy_id(self):
        self.assertEqual(self.p[0]["callArgument"],"nTemplateId")
        self.assertEqual((self.p[0]["sourceField"],self.p[0]["sourceOffset"],self.p[0]["nativeRegister"]),
                         ("MapCopyID",20,"edx"))
    def test_loader_map_id_is_subworld(self):
        self.assertEqual((self.p[1]["callArgument"],self.p[1]["sourceField"],self.p[1]["sourceOffset"],self.p[1]["nativeRegister"]),
                         ("nMapId","SubWorld",4,"r8d"))
    def test_loader_region_is_region(self):
        self.assertEqual((self.p[2]["callArgument"],self.p[2]["sourceField"],self.p[2]["sourceOffset"],self.p[2]["nativeRegister"]),
                         ("nRegion","Region",8,"r9d"))
    def test_spawn_coordinates_zero_not_packet_position(self):
        self.assertEqual([(x["callArgument"],x["sourceValue"]) for x in self.p[3:]],
                         [("nMpsX","zeroed-esi"),("nMpsY","zeroed-esi")])
    def test_historical_map53_trace_field_values(self):
        trace=self.doc["historicalMap53HarnessObservation"]
        self.assertEqual(trace["observedFields"],{"SubWorld":53,"MapCopyID":53,"Region":6488114})
        self.assertEqual(trace["totalWorldSyncEvents"],2)
        self.assertEqual(trace["distinctWorldSyncEvents"],1)
    def test_historical_map53_loader_arguments_are_derived(self):
        self.assertEqual(self.doc["historicalMap53HarnessObservation"]["derivedLoaderArguments"],
                         {"nTemplateId":53,"nMapId":53,"nRegion":6488114,
                          "nMpsX":0,"nMpsY":0})
    def test_map53_harness_not_runtime_acceptance(self):
        trace=self.doc["historicalMap53HarnessObservation"]
        self.assertEqual(trace["kind"],"historical-local-test-harness-events")
        self.assertIn("NOT_RUNTIME_ACCEPTANCE",trace["gate"])
        self.assertEqual(len(trace["sourceSHA256"]),64)
    def test_original_calibrate_uses_three_server_unity_pairs(self):
        self.assertEqual([x["name"] for x in self.methods[("MapService","Calibrate")]["parameters"]],
            ["serverA","unityA","serverB","unityB","serverC","unityC"])
    def test_original_method_tokens(self):
        self.assertEqual(self.methods[("ProcessGameUnity","OnSyncWorld")]["token"],"0x60038a9")
        self.assertEqual(self.methods[("MapService","UpdateWorldInfo")]["token"],"0x60013b0")
        self.assertEqual(self.methods[("SubWorld","LoadMap")]["token"],"0x60039f9")
    def test_field_names_are_proven_but_runtime_coefficients_not(self):
        self.assertIn("full wire framing",self.doc["limitations"])
        self.assertIn("remain unverified",self.doc["limitations"])
        self.assertNotIn("map53Coefficients",self.doc)
    def test_no_fabricated_anchor_pairs(self):
        self.assertNotIn("landmarks",self.doc)
        self.assertNotIn("cn02GatePassed",self.doc)

if __name__=="__main__":unittest.main(verbosity=2)

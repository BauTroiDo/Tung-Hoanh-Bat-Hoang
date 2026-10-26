"""Regression: original 4016 maplist and scene variant callgraph evidence.

Positive controls require exact native codegen ownership and direct call RVAs.
This suite does not claim any Map53 concrete Addressables key or file exists.
"""
import json,unittest
from pathlib import Path
ROOT=Path(__file__).resolve().parents[3]
E=ROOT/"reports/evidence/cn02-4016-il2cpp"
class ScenePackResolverTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.d=json.loads((E/"scene-pack-resolver.json").read_text(encoding="utf8"))
        cls.old=json.loads((E/"loadmap-selector.json").read_text(encoding="utf8"))
        cls.m={row["method"]:row for row in cls.d["traces"]}
    def link(self,method,dest,at):
        rows=[c for c in self.m[method]["knownCallees"] if dest in c["registeredNames"]]
        self.assertIn(at,[r["rva"] for r in rows])
    def test_binary_hash(self):
        self.assertEqual(self.d["binarySHA256"],self.old["binarySHA256"])
    def test_metadata_hash(self):
        self.assertEqual(self.d["metadataSHA256"],self.old["metadataSHA256"])
    def test_reference_entry(self):
        self.assertEqual(self.d["referenceLoadMapNativeRVA"],"0xc75de0")
        self.assertIn("MapDataService.LoadForTemplate",self.m)
    def test_original_map_pack_continuation(self):
        x=self.m["MapDataService.LoadForTemplate"]
        self.assertEqual((x["rva"],x["endRVAExclusive"]),("0x8c9bf0","0x8c9f7c"))
        self.assertEqual(x["decodedInstructions"],175)
    def test_map_pack_calls_resolver(self):
        self.link("MapDataService.LoadForTemplate","MapDataService.ResolveMapFolderPath","0x8c9f59")
    def test_map_pack_unloads_old_obstacles(self):
        self.link("MapDataService.LoadForTemplate","SceneObstacleMap.Unload","0x8c9f34")
    def test_resolver_reads_maplist(self):
        self.link("MapDataService.ResolveMapFolderPath","MapDataService.ParseMaplistFile","0x8cd367")
        self.link("MapDataService.ResolveMapFolderPath","MapDataService.ParseMaplistFile","0x8cd391")
    def test_resolver_has_fallback_path(self):
        self.link("MapDataService.ResolveMapFolderPath","MapDataService.TryMaplistPath","0x8cd40b")
    def test_parser_reads_streaming_bytes(self):
        self.link("MapDataService.ParseMaplistFile","MapDataService.ReadStreamingBytes","0x8cc194")
        self.link("MapDataService.ParseMaplistFile","MapDataService.ParseMaplist","0x8cc19e")
    def test_game_manager_async_state_machine(self):
        self.assertEqual(self.m["<LoadScene>d__96.MoveNext"]["rva"],"0xa6f160")
        self.link("<LoadScene>d__96.MoveNext","GameManager.RequestLoadMap","0xa6f841")
        self.link("<LoadScene>d__96.MoveNext","GameManager.RequestLoadMap","0xa6f9a5")
    def test_scene_service_loader_state_machine(self):
        self.link("<LoadSceneAsync>d__24.MoveNext","SceneLoadService.LoadSceneInternal","0xc8dad7")
    def test_scene_variant_key_resolution(self):
        self.link("<LoadSceneInternal>d__26.MoveNext","SceneLoadService.ResolveVariantKeyAsync","0xc8ea74")
        self.link("<LoadSceneInternal>d__26.MoveNext","SceneLoadService.ResolveVariantKeyAsync","0xc8ecc7")
    def test_addressable_scene_load(self):
        self.link("<LoadSceneInternal>d__26.MoveNext","SceneLoadService.LoadAddressableMapSceneAsync","0xc8f16a")
        self.link("<LoadSceneInternal>d__26.MoveNext","SceneLoadService.LoadAddressableMapSceneAsync","0xc8f26e")
    def test_scene_container_prefabs(self):
        self.link("<LoadSceneInternal>d__26.MoveNext","SceneLoadService.LoadSceneContainerAsMapSceneAsync","0xc8f5ed")
        self.link("<LoadSceneInternal>d__26.MoveNext","SceneLoadService.LoadPrefabsAsync","0xc8f809")
    def test_ambiguous_thunks_not_named(self):
        x=self.m["<LoadSceneInternal>d__26.MoveNext"]
        self.assertTrue(any(r["ambiguousAliasCount"]>3 and r["registeredNames"]==[] for r in x["allCalls"]))
    def test_resolve_variant_entry(self):
        self.assertEqual(self.m["SceneLoadService.ResolveVariantKeyAsync"]["rva"],"0xc74650")
        self.assertEqual(self.m["SceneLoadService.LoadAddressableMapSceneAsync"]["rva"],"0xc73c00")
    def test_no_map53_runtime_acceptance(self):
        self.assertIn("Does not prove actual Addressables scene key",self.d["limits"])
        self.assertNotIn("verifiedMap53SceneKey",self.d)
        self.assertNotIn("map53Affine",self.d)
        self.assertNotIn("pairedAnchors",self.d)
if __name__=="__main__":unittest.main(verbosity=2)

"""Regression checks on read-only extracted analytical metadata, never asset payloads."""
import json
from pathlib import Path
import sys
import unittest

ROOT=Path(__file__).resolve().parents[2]
EVIDENCE=ROOT/"reports/evidence/cn02-4016-static"

class StaticReference4016Tests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.scene=json.loads((EVIDENCE/"scene-transform-inventory.json").read_text(encoding="utf8"))
        cls.meshes=json.loads((EVIDENCE/"mesh-bounds-inventory.json").read_text(encoding="utf8"))
    def test_source_identity(self):
        self.assertEqual(self.scene["sourceSha256"],"EEB567D492CE740AAD8A796F531D993917F511FDA60ECF4DDE25DA6C2B4DEC4A")
        self.assertEqual(self.scene["sourceByteLength"],9738990)
    def test_unmodified_original_has_29_scene_transforms(self):
        self.assertEqual(len(self.scene["sceneObjects"]),29)
        self.assertTrue(all(e["unresolvedParent"] is None for e in self.scene["sceneObjects"]))
        self.assertEqual(self.scene["counts"]["GameObject"],29)
        self.assertEqual(self.scene["counts"]["Transform"],29)
    def test_four_original_terrain_tile_transform_names(self):
        names={e["gameObject"] for e in self.scene["sceneObjects"]}
        self.assertTrue(all(f"TerrainBLH2a_{x:02d}_{z:02d}" in names for x in (1,2) for z in (1,2)))
    def test_original_terrain_mesh_metadata(self):
        rows=self.meshes["meshRecords"]
        self.assertEqual(len(rows),6)
        self.assertTrue(any(r["name"]=="TerrainBLH2a" and r["aabb"]["m_Extent"]["x"]==125 and r["aabb"]["m_Extent"]["z"]==125 for r in rows))
    def test_canonical_baked_scene_reference(self):
        hashes=self.meshes["assetBundle"][0]["sceneHashes"]
        self.assertTrue(any("PC_BLH_Bake.unity" in repr(row) for row in hashes))
    def test_no_unwarranted_claim_of_legacy_pairing(self):
        self.assertIn("not 3+1 paired legacy coordinates",self.scene["interpretation"])
        self.assertNotIn("legacyPairs",self.scene)
    def test_tile_pitch_and_unity_transform_identity(self):
        rows={e["gameObject"]:e for e in self.scene["sceneObjects"]}
        a=rows["TerrainBLH2a_01_01"]["worldPosition"]
        b=rows["TerrainBLH2a_01_02"]["worldPosition"]
        c=rows["TerrainBLH2a_02_01"]["worldPosition"]
        self.assertAlmostEqual(b[2]-a[2],250,places=3)
        self.assertAlmostEqual(c[0]-a[0],250,places=3)
        for name in (x for x in rows if x.startswith("TerrainBLH2a_")):
            self.assertEqual(rows[name]["worldScale"],[1.0,1.0,1.0])
            self.assertEqual(rows[name]["localRotation"],[0.0,0.0,0.0,1.0])
    def test_generated_candidates_are_not_verified_pairs(self):
        data=json.loads((EVIDENCE/"reference-tile-origin-candidates.json").read_text(encoding="utf8"))
        self.assertEqual(data["status"],"UNPAIRED_REFERENCE_SCENE_ONLY")
        self.assertEqual(data["independentReferenceToLegacyPairs"],0)
        self.assertEqual(len(data["sceneCandidates"]),4)
        self.assertTrue(all(not r["hasLegacy"] and r["legacyMap53XY"] is None and not r["landmarkCorrespondenceEstablished"] for r in data["sceneCandidates"]))

if __name__=="__main__":
    unittest.main(verbosity=2)

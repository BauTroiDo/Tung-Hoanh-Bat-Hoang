"""CN02-OFFLINE-01 regression: local Unity artifact integrity, not gameplay networking."""
import json
import unittest
from audit_unity_offline import ROOT, PROJECT, EVIDENCE, audit, digest, MAT_NAMES

REPORT=ROOT/"reports/evidence/cn02-offline01/client-offline-health.json"

class OfflineClientTests(unittest.TestCase):
 @classmethod
 def setUpClass(cls):cls.d=json.loads(REPORT.read_text(encoding="utf8"))
 def test_01_schema(self):
  self.assertEqual(self.d["schema"],"cn02-offline01-readonly-source-v1")
 def test_02_editor_version(self):
  self.assertEqual(self.d["unityEditorVersion"],"6000.6.7f1")
 def test_03_no_duplicate_project_guids(self):
  self.assertEqual(self.d["duplicateProjectGuids"],{})
 def test_04_no_assets_missing_meta(self):
  self.assertEqual(self.d["missingAssetMeta"],[])
 def test_05_all_project_package_refs_resolved(self):
  self.assertEqual(self.d["unresolvedGuids"],[])
  self.assertEqual(self.d["referenceCountsByCategory"].get("project"),120)
  self.assertGreater(self.d["referenceCountsByCategory"].get("package",0),0)
 def test_06_three_known_offline_scenes(self):
  self.assertEqual(set(self.d["requiredOfflineScenes"]),
                   {"BaLangHuyen.unity","Bootstrap.unity","MovementTest.unity"})
 def test_07_serialized_asset_scan_done(self):
  self.assertEqual(self.d["serializedScannedFileCount"],16)
  self.assertEqual(len(self.d["perFileReferenceCounts"]),16)
 def test_08_previous_materials_preserved_uncommitted(self):
  self.assertEqual(set(self.d["changedUncommittedAntigravityMaterialNames"]),set(MAT_NAMES))
 def test_09_editmode_live_unity_xml(self):
  r=self.d["editMode"]
  self.assertEqual((r["result"],r["total"],r["passed"],r["failed"],r["skipped"]),
                   ("Passed",29,29,0,0))
  self.assertEqual(r["sha256"],digest(EVIDENCE/"editmode.xml"))
 def test_10_playmode_live_unity_xml(self):
  r=self.d["playMode"]
  self.assertEqual((r["result"],r["total"],r["passed"],r["failed"],r["skipped"]),
                   ("Passed",18,18,0,0))
  self.assertEqual(r["sha256"],digest(EVIDENCE/"playmode.xml"))
 def test_11_player_executable_hash(self):
  p=EVIDENCE/"Builds/JXClientNew.exe"
  self.assertEqual(self.d["win64Player"]["exeSha256"],digest(p))
  self.assertGreater(p.stat().st_size,500_000)
 def test_12_player_companions(self):
  self.assertTrue(self.d["win64Player"]["companionFolderPresent"])
  self.assertTrue(self.d["win64Player"]["unityPlayerDllPresent"])
 def test_13_no_false_gameplay_acceptance(self):
  self.assertFalse(self.d["networkOrServerUsed"])
  self.assertFalse(self.d["runtimeGsAcceptanceVerified"])
  self.assertFalse(self.d["win64Player"]["launchOrNetworkAcceptanceVerified"])
  self.assertIn("SYNTHETIC",self.d["scope"])
 def test_14_recompute_readonly(self):
  new=audit()
  self.assertEqual(new["referenceCountsByCategory"],self.d["referenceCountsByCategory"])
  self.assertEqual(new["unresolvedGuids"],self.d["unresolvedGuids"])
 def test_15_compact_provenance_not_raw_assets(self):
  self.assertLess(REPORT.stat().st_size,20000)
  self.assertNotIn("m_VertexData",REPORT.read_text(encoding="utf8"))
if __name__=="__main__":unittest.main(verbosity=2)

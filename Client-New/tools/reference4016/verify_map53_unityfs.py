"""CN02-BULK-07 read-only Map53 UnityFS and local Addressables mirror regressions."""
import json
import sys
import unittest
from pathlib import Path
from audit_map53_unityfs import (ROOT, GAME, REFERENCE, CATALOG, SETTINGS,
                                OFFICIAL, PINS, audit, unityfs, lz4_raw, checksum)

SUMMARY=ROOT/"reports/evidence/cn02-4016-il2cpp/map53-offline-assets-summary.json"
class BundleAudit(unittest.TestCase):
 @classmethod
 def setUpClass(cls):
  cls.s=json.loads(SUMMARY.read_text(encoding="utf8"))
  cls.blobs={("assets" if "_assets_" in f else "scenes"):OFFICIAL/f for f in PINS}
 def test_01_catalog_target(self):
  self.assertEqual(self.s["catalogBuildTarget"],"StandaloneWindows64")
  self.assertEqual(self.s["addressablesVersion"],"2.8.1")
 def test_02_catalog_digest(self):
  self.assertEqual(self.s["catalogSha256"],checksum(CATALOG))
 def test_03_file_pins(self):
  for filename,(expected,length) in PINS.items():
   self.assertEqual(checksum(OFFICIAL/filename),expected)
   self.assertEqual((OFFICIAL/filename).stat().st_size,length)
 def test_04_has_two_bundles(self):
  self.assertEqual(set(self.s["bundles"]),{"assets","scenes"})
 def test_05_unityfs_header(self):
  for row in self.s["bundles"].values():
   self.assertEqual(row["format"],"UnityFS")
   self.assertEqual(row["formatVersion"],8)
   self.assertEqual(row["unityRevision"],"6000.3.9f1")
 def test_06_declared_sizes(self):
  for row in self.s["bundles"].values():
   self.assertEqual(row["declaredSize"],row["sizeBytes"])
 def test_07_two_catalog_names(self):
  self.assertTrue(all(x["catalogContainsExactName"] for x in self.s["bundles"].values()))
 def test_08_full_block_streams_decoded(self):
  for row in self.s["bundles"].values():
   self.assertEqual(row["blockCount"],row["verifiedBlockCount"])
   self.assertGreater(row["verifiedUncompressedBytes"],row["sizeBytes"]//2)
 def test_09_asset_blocks_nodes(self):
  x=self.s["bundles"]["assets"]
  self.assertEqual((x["blockCount"],x["nodeCount"]),(349,2))
 def test_10_scene_blocks_nodes(self):
  x=self.s["bundles"]["scenes"]
  self.assertEqual((x["blockCount"],x["nodeCount"]),(198,4))
 def test_11_no_scene_success_claim(self):
  self.assertFalse(self.s["fullUnityAssetDeserializeOrSceneLoadVerified"])
  self.assertFalse(self.s["liveMap53SceneRendered"])
  self.assertFalse(self.s["gsAuthoritativeMovementVerified"])
 def test_12_lz4_known_value(self):
  self.assertEqual(lz4_raw(bytes([0x32])+b"ABC"+bytes([3,0]),9),b"ABCABCABC")
 def test_13_lz4_repeated_single_char(self):
  self.assertEqual(lz4_raw(bytes([0x11])+b"A"+bytes([1,0]),6),b"AAAAAA")
 def test_14_lz4_truncated_rejected(self):
  with self.assertRaises(ValueError): lz4_raw(bytes([0x00,0x00,0x00]),4)
 def test_15_lz4_mismatch_rejected(self):
  with self.assertRaises(ValueError):lz4_raw(bytes([0x10])+b"A",999)
 def test_16_mirror_read_only_exact_map53(self):
  sys.path.insert(0,str(GAME/"Client-3D-2/LAN-LOCAL"))
  import m1_map53_asset_server as mirror
  r=mirror.paths()  # no HTTP listener and no modified log/cache
  self.assertGreaterEqual(len(r),1000)
  self.assertTrue(all(x in r for x in PINS))
 def test_17_deterministic_summary(self):
  a=audit()
  self.assertEqual(a,self.s)
 def test_18_no_raw_asset_payload_in_git_summary(self):
  raw=SUMMARY.read_text(encoding="utf8")
  self.assertLess(len(raw),10000)
  self.assertNotIn("CAB-a9e7",raw)
if __name__=="__main__":
 unittest.main(verbosity=2)

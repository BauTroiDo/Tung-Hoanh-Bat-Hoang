"""Regression for CN02-BULK-09 sanitized runtime checkpoint.

This validates a saved evidence *summary* and original metadata identity.
It cannot authenticate Bishop session logs or establish Map53 gameplay PASS.
"""
import json
import unittest
from pathlib import Path
from map53_qa_preflight import ROOT, GAME, REF_HASH, sha

SOURCE=ROOT/"reports/evidence/cn02-4016-il2cpp/cn02-bulk09-live-qa-summary.json"

class NativeQaSnapshotTests(unittest.TestCase):
 @classmethod
 def setUpClass(cls):
  cls.d=json.loads(SOURCE.read_text(encoding="utf8"))
 def test_01_schema(self):
  self.assertEqual(self.d["schema"],"cn02-bulk09-live-qa-snapshot-v1")
 def test_02_only_qa_allowlist(self):
  x=self.d["gateway"]
  self.assertTrue(x["nativeGsBridge"])
  self.assertTrue(x["gatewayProcessConfirmed"])
  self.assertEqual(x["allowlistedClient"],"192.168.1.13")
 def test_03_original_client_source_intact(self):
  self.assertEqual(self.d["client"]["originalMetadataSha256"],REF_HASH)
  self.assertEqual(sha(GAME/"Client-3D-2/update/Game_Data/il2cpp_data/Metadata/global-metadata.dat"),REF_HASH)
 def test_04_native_handshake_recorded(self):
  self.assertEqual(self.d["network"]["nativeGreetingBytes"],34)
  self.assertTrue(self.d["network"]["nativeGreetingPrefixValid"])
  self.assertFalse(self.d["network"]["credentialsSentBySmoke"])
 def test_05_auth_seen_not_world_entry(self):
  self.assertGreaterEqual(self.d["evidenceCounts"]["bishopAuthSuccess"],1)
  self.assertEqual(self.d["evidenceCounts"]["bishopPermitSent"],0)
  self.assertEqual(self.d["gate"]["bishopGsPermit"],"REJECTED")
 def test_06_no_worldsync_claim(self):
  self.assertEqual(self.d["gate"]["worldSync"],"NOT_VERIFIED")
 def test_07_no_scene_claim(self):
  self.assertEqual(self.d["gate"]["map53SceneRendered"],"NOT_VERIFIED")
 def test_08_no_movement_claim(self):
  self.assertEqual(self.d["gate"]["gsAuthoritativeMove"],"NOT_VERIFIED")
 def test_09_calibration_not_claimed(self):
  self.assertEqual(self.d["gate"]["activeAffineSixCoefficients"],"NOT_CAPTURED")
  self.assertEqual(self.d["gate"]["pairedLandmarks"],"NOT_CAPTURED")
 def test_10_acceptance_fails_closed(self):
  self.assertFalse(self.d["gate"]["runtimeEndToEndAccepted"])
  self.assertEqual(self.d["gate"]["cn0204"],"BLOCKED")
  self.assertEqual(self.d["gate"]["cn03"],"CLOSED")
 def test_11_mirror_only_catalog(self):
  self.assertEqual(self.d["assets"]["catalogHashGet200"],1)
  self.assertFalse(self.d["assets"]["map53SceneBundleRequestedDuringSnapshot"])
  self.assertFalse(self.d["assets"]["map53AssetsBundleRequestedDuringSnapshot"])
 def test_12_no_secrets(self):
  raw=SOURCE.read_text(encoding="utf8").lower()
  for bad in ("password", "bearer ", "raw_payload", "cookie", "role_bytes",
              "ticket=", "accountname", "mac_bytes", "digest="):
   self.assertNotIn(bad,raw)
 def test_13_raw_evidence_not_inside_repo(self):
  self.assertFalse(Path(self.d["rawClientEvidenceRoot"]).is_relative_to(ROOT))
 def test_14_sanitized_qa_snapshot_only(self):
  self.assertIn("Counters are cumulative",self.d["limits"][0])
  self.assertIn("do not bypass",self.d["limits"][1])

if __name__=="__main__":
 unittest.main(verbosity=2)

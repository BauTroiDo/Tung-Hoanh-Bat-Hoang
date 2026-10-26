"""CN02-BULK-08: regress isolated mirror HTTP and conservative Map53 selector contract.

The test checks a saved synthetic loopback preflight, NOT a live Game.exe
capture. Do not turn connectivity reachability into gameplay acceptance.
"""
import json
import unittest
from pathlib import Path
from map53_qa_preflight import (
 ROOT, GAME, RAW_BASE, REF_HASH, STAGED_HASH, project_template,
 map53_assertion, http_probe, tcp_gate, sha, PINS
)

REPORT=ROOT/"reports/evidence/cn02-4016-il2cpp/map53-qa-preflight-summary.json"
LOG=RAW_BASE/"isolated-v2/mirror-http-events.jsonl"

class QaPreflightTests(unittest.TestCase):
 @classmethod
 def setUpClass(cls):
  cls.m=json.loads(REPORT.read_text(encoding="utf8"))
 def test_01_schema(self):
  self.assertEqual(self.m["stage"],"CN02-BULK-08")
 def test_02_installed_original_metadata(self):
  self.assertEqual(self.m["metadataOriginalSha256"],REF_HASH)
  self.assertEqual(sha(GAME/"Client-3D-2/update/Game_Data/il2cpp_data/Metadata/global-metadata.dat"),REF_HASH)
 def test_03_staged_backup_hashes(self):
  self.assertEqual(self.m["metadataStagedSha256"],STAGED_HASH)
  self.assertTrue(self.m["originalBackupMatches"])
 def test_04_map53_named_projection(self):
  expected={"nTemplateId":53,"nMapId":53,"nRegion":6488114,"nMpsX":0,"nMpsY":0}
  self.assertEqual(self.m["nativeWorldSyncFixture"]["projected"],expected)
  self.assertTrue(self.m["nativeWorldSyncFixture"]["knownHistoricalMap53CaseMatches"])
 def test_05_template_zero_not_valid_for_historical_map53(self):
  z=self.m["reportedConverterZeroTemplateFixture"]
  self.assertEqual(z["projected"]["nTemplateId"],0)
  self.assertEqual(z["projected"]["nMapId"],53)
  self.assertFalse(z["knownHistoricalMap53CaseMatches"])
 def test_06_ids_are_independent_in_general(self):
  x=project_template({"MapCopyID":123,"SubWorld":999,"Region":1})
  self.assertEqual((x["nTemplateId"],x["nMapId"]),(123,999))
 def test_07_original_converter_status_not_overclaimed(self):
  self.assertIn("DOES NOT READ SERVER SOURCE",self.m["converterStatus"])
  self.assertIn("No new live",self.m["converterStatus"])
 def test_08_http_catalog_get_and_head(self):
  v=self.m["mirror"]["tests"]
  self.assertEqual(v["catalogHashGet"],{"status":200,"bytes":32})
  self.assertEqual(v["catalogHashHead"],{"status":200,"bytes":32})
 def test_09_http_settings(self):
  v=self.m["mirror"]["tests"]["addressablesSettingsGet"]
  self.assertEqual(v["status"],200)
  self.assertEqual(v["buildTarget"],"StandaloneWindows64")
 def test_10_http_two_official_bundles(self):
  v=self.m["mirror"]["tests"]
  for label,size in (("assets",31592781),("scenes",9738990)):
   self.assertEqual(v[label+"Head"]["sizeBytes"],size)
   self.assertEqual(v[label+"Head"]["status"],200)
   self.assertEqual(v[label+"Range"]["status"],206)
   self.assertEqual(v[label+"Range"]["returnedBytes"],7)
   self.assertEqual(v[label+"Range"]["signature"],"UnityFS")
 def test_11_unknown_resource_http_404(self):
  self.assertEqual(self.m["mirror"]["tests"]["missingResourceGets404"]["status"],404)
 def test_12_mirror_size_and_bound_sources(self):
  self.assertGreaterEqual(self.m["mirror"]["availableBundleNames"],1000)
  self.assertEqual(self.m["mirror"]["officialStagedSources"],7)
  self.assertEqual(self.m["mirror"]["cachedUnityFsSources"],1120)
 def test_13_exact_eight_isolated_http_records(self):
  self.assertTrue(LOG.is_file())
  rows=[json.loads(s) for s in LOG.read_text(encoding="utf8").splitlines()]
  self.assertEqual(len(rows),8)
  self.assertEqual(self.m["mirror"]["isolatedLogEntryCount"],8)
  self.assertEqual(sha(LOG),self.m["mirror"]["isolatedMirrorLogSHA256"])
 def test_14_raw_log_is_not_in_git(self):
  self.assertFalse(LOG.is_relative_to(ROOT))
  self.assertTrue(LOG.is_relative_to(GAME/"Tool/_outputs"))
 def test_15_only_synthetic_qa_not_gameplay_acceptance(self):
  self.assertFalse(self.m["originalGameClientExecuted"])
  self.assertFalse(self.m["metadataSwapped"])
  self.assertFalse(self.m["authoritativeGsMoveVerified"])
  self.assertFalse(self.m["originalSceneRendered"])
  self.assertFalse(self.m["qaRuntimeAcceptance"])
 def test_16_backend_connectivity_is_only_snapshot(self):
  ports=self.m["backendPorts"]
  self.assertEqual(set(ports),{"56722","51189","14180"})
  self.assertIn("port reachability",self.m["note"])
  self.assertFalse(ports["14180"]["reachable"])
  self.assertTrue(ports["56722"]["reachable"])
  self.assertTrue(ports["51189"]["reachable"])
 def test_17_fail_closed_output_in_repo(self):
  with self.assertRaises(ValueError):http_probe(ROOT/"forbidden-in-repo")
 def test_18_backend_skip_without_host(self):
  self.assertEqual(tcp_gate(None,[1])["skipped"],True)
 def test_19_no_raw_credentials_or_server_host_in_committed_summary(self):
  s=REPORT.read_text(encoding="utf8").lower()
  for x in ("password","token=","ticket=","account=","192.168.1.69"):
   self.assertNotIn(x,s)
 def test_20_four_pin_name_checks(self):
  self.assertEqual(len(PINS),2)
  self.assertTrue(all("pc_balanghuyen_" in n for n in PINS))
if __name__=="__main__":
 unittest.main(verbosity=2)

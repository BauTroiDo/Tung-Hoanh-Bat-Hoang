"""Regression for evidence-grade CN02-BULK-05; historical sources != live parity."""
import json,unittest
from pathlib import Path
from build_semantic_trace import (
 ROOT, GAME, QA, REF, EVIDENCE, DB, EXPECTED_BIN, EXPECTED_META,
 generate, sha, event_aggregate, http_aggregate, native_edges, scene_static_edges,
 named_world_bindings, method_contracts, jsonl
)
REPORT=ROOT/"reports/evidence/cn02-4016-il2cpp/semantic-trace-summary.json"
IDA=GAME/"Tool/_outputs/cn02-bulk/semantic-trace/ida73-smoke-run.json"

class EvidenceTests(unittest.TestCase):
 @classmethod
 def setUpClass(cls):
  cls.d=json.loads(REPORT.read_text(encoding="utf8"))
 def test_01_original_binary_hash(self):
  self.assertEqual(sha(REF/"GameAssembly.dll"),EXPECTED_BIN)
 def test_02_original_metadata_hash(self):
  self.assertEqual(sha(REF/"Game_Data/il2cpp_data/Metadata/global-metadata.dat"),EXPECTED_META)
 def test_03_region_records(self):
  self.assertEqual(self.d["historicalTraces"]["region"]["validJsonRecords"],124)
  self.assertEqual(self.d["historicalTraces"]["region"]["invalidJsonLines"],0)
 def test_04_world_events(self):
  self.assertEqual(self.d["historicalTraces"]["region"]["kindCounts"]["receivedWorldMap"],19)
  self.assertEqual(self.d["historicalTraces"]["region"]["kindCounts"]["subworldMapSelector"],19)
 def test_05_world_values(self):
  self.assertEqual(self.d["historicalTraces"]["region"]["observedWorldTriples"],
                   [{"subWorld":53,"mapCopyId":53,"region":6488114}])
 def test_06_requested_attempts(self):
  v=self.d["historicalTraces"]["region"]["kindCounts"]
  self.assertEqual(v["originalMapRequest"],19)
  self.assertEqual(v["originalMapLoadAttempt"],10)
 def test_07_not_render_proof(self):
  self.assertEqual(self.d["historicalTraces"]["region"]["avatarCreationPendingCount"],19)
  self.assertFalse(self.d["runtimeEndToEndPass"])
  self.assertFalse(self.d["map53CalibrationAccepted"])
 def test_08_fixed_trace_distinct(self):
  self.assertEqual(self.d["historicalTraces"]["fixed"]["validJsonRecords"],26)
  self.assertEqual(self.d["historicalTraces"]["fixed"]["kindCounts"]["receivedWorldMap"],5)
 def test_09_http_counts(self):
  h=self.d["historicalTraces"]["http"]
  self.assertEqual(h["jsonRecords"],2433)
  self.assertEqual(h["malformedLines"],2)
  self.assertEqual(h["httpStatusCounts"],{"200":7,"206":5,"404":2421})
 def test_10_local_pc_balang_responses(self):
  h=self.d["historicalTraces"]["http"]
  self.assertEqual(h["recordsMatchingPcBalanghuyenBundleName"],2)
  self.assertEqual(h["pcBalangStatusCounts"],{"200":2})
  self.assertEqual(h["pcBalangResponseLengthsBytes"],[9738990,31592781])
 def test_11_direct_native_edges(self):
  x=self.d["nativeDirectCallEdges"]
  self.assertEqual(len(x),3)
  self.assertTrue(all(e["verifiedDirectCall"] for e in x))
 def test_12_static_scene_edges(self):
  x=self.d["scenePathDirectCallEdges"]
  self.assertEqual(len(x),3)
  self.assertTrue(all(e["verifiedDirectCall"] for e in x))
 def test_13_method_rva_index(self):
  x=self.d["knownMethodRvaContracts"]
  self.assertEqual(len(x),7)
  self.assertTrue(all(t["metadataIndexMatches"] for t in x))
 def test_14_loadmap_named_fields(self):
  p=self.d["namedWorldSyncToLoadMap"]["formalBindings"]
  self.assertEqual({x["formalParameter"]:x["worldSyncField"] for x in p if "worldSyncField" in x},
    {"nTemplateId":"MapCopyID","nMapId":"SubWorld","nRegion":"Region"})
 def test_15_loadmap_native_offsets(self):
  p=self.d["namedWorldSyncToLoadMap"]["formalBindings"]
  self.assertEqual({x["formalParameter"]:x["byteOffset"] for x in p if "byteOffset" in x},
                   {"nTemplateId":20,"nMapId":4,"nRegion":8})
 def test_16_historical_loadmap_projection(self):
  self.assertEqual(self.d["namedWorldSyncToLoadMap"]["projectedHistoricalMapSelectorInputs"],
   [{"nTemplateId":53,"nMapId":53,"nRegion":6488114,"nMpsX":0,"nMpsY":0}])
 def test_17_auth_is_only_documented(self):
  stages=self.d["stageAssessments"]
  self.assertEqual(stages[0]["status"],"DOCUMENTED_HISTORICALLY")
  self.assertEqual(stages[1]["status"],"DOCUMENTED_HISTORICALLY")
 def test_18_world_observed_only_historical(self):
  self.assertEqual(self.d["stageAssessments"][2]["status"],
                   "OBSERVED_IN_HISTORICAL_INSTRUMENTED_EVENTS")
 def test_19_map_attempt_only(self):
  self.assertEqual(self.d["stageAssessments"][3]["status"],"OBSERVED_ATTEMPT")
 def test_20_scene_gate_blocked(self):
  self.assertEqual(self.d["stageAssessments"][4]["status"],"NOT_VERIFIED")
 def test_21_movement_gate_blocked(self):
  self.assertEqual(self.d["stageAssessments"][5]["status"],"NOT_VERIFIED")
 def test_22_not_conflate_sessions(self):
  self.assertIn("different historical",self.d["limits"])
  self.assertIn("CN02-04 BLOCKED",self.d["limits"])
 def test_23_sanitized_report(self):
  x=json.dumps(self.d,ensure_ascii=False)
  for fragment in ("password=", "credential=", "guid16=", "ticket=", "bearer ", "cdn-jx3d.tepaylink.vn"):
   self.assertNotIn(fragment,x.lower())
 def test_24_ida_headless_smoke_bound(self):
  info=json.loads(IDA.read_text(encoding="utf8"))
  self.assertEqual(info["reference"],"synthetic Rust smoke only")
  self.assertEqual(info["exitCode"],1)
  # IDA x64 Python DLL and stdlib now exist. The newer -L diagnostic gives
  # the actual remaining blocker: an unaccepted GUI license for batch mode.
  diag=(IDA.parent/"ida73-diagnostic.log").read_text(encoding="utf8",errors="replace")
  self.assertIn("License not yet accepted",diag)
  self.assertNotIn("python64.dll",info["output"])
 def test_25_no_proprietary_body_in_output(self):
  s=json.dumps(self.d)
  self.assertNotIn("C pseudo-code:",s)
  self.assertNotIn("Disassembly (x86_64",s)
 def test_26_data_stays_outside_git(self):
  self.assertTrue((QA/"M1-map53-region-events.jsonl").is_relative_to(GAME/"Client-3D-2"))
  self.assertFalse(IDA.is_relative_to(ROOT))
 def test_27_http_404_not_specific_current_bundle(self):
  h=self.d["historicalTraces"]["http"]
  self.assertEqual(h["pcBalangStatusCounts"],{"200":2})
  self.assertIn("NOT current",h["note"])
 def test_28_generate_deterministic(self):
  freshly=generate()
  self.assertEqual(freshly["nativeDirectCallEdges"],self.d["nativeDirectCallEdges"])
  self.assertEqual(freshly["stageAssessments"],self.d["stageAssessments"])
 def test_29_no_movement_claim_from_actor_id(self):
  count=self.d["historicalTraces"]["region"]["kindCounts"]["receivedNpcPosition"]
  self.assertGreater(count,0)
  self.assertEqual(self.d["stageAssessments"][-1]["status"],"NOT_VERIFIED")

if __name__=="__main__":
 unittest.main(verbosity=2)

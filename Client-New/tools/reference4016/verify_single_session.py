"""Synthetic-only unit tests for CN02-BULK-06 evidence contract.

These fixture events never happened: this is parser/gate regression, NOT QA.
"""
import copy
import unittest
from check_single_session import assess,PHASES,SOURCES,geometric_quality

def fixture():
 phases=[*PHASES,"affine","landmarks"]
 events=[{"seq":i+1,"tsMs":1000+100*i,"phase":phase,
          "sessionId":"qa-synthetic-20261026","actorId":"actor-synthetic",
          "origin":SOURCES[phase]} for i,phase in enumerate(phases)]
 e={x["phase"]:x for x in events}
 e["bishop_auth"]["accepted"]=True
 e["gs_sync"]["accepted"]=True
 e["world_sync"].update(subWorld=53,mapCopyId=53,region=6488114)
 e["load_map"].update(templateId=53,mapId=53,region=6488114,mpsX=0,mpsY=0)
 e["asset_verified"].update(catalogMatch=True,integrityOk=True)
 e["scene_ready"].update(mapId=53,avatarVisible=True)
 e["user_move_request"].update(userClickedAfterScene=True,requestId="move-synthetic")
 e["gs_move_ack"].update(requestId="move-synthetic",x=3.0,y=4.0)
 e["unity_move_echo"].update(requestId="move-synthetic",x=3.01,y=3.98)
 e["affine"].update(coefficients=[1,0,0,0,1,0],activeRuntimeMatrix=True)
 e["landmarks"].update(sameScene=True,points=[
  {"mps":[0,0],"unity":[0,0]},
  {"mps":[10,0],"unity":[10,0]},
  {"mps":[0,10],"unity":[0,10]},
  {"mps":[8,8],"unity":[8,8]}
 ])
 return {"schema":"cn02-single-session-v1","sessionId":"qa-synthetic-20261026",
         "actorId":"actor-synthetic","mode":"live","events":events,
         "provenance":{k:"a"*64 for k in
                       ("bishop","gameserver","original-client","scene-screenshot")}}

class GateTests(unittest.TestCase):
 def test_01_structural_fixture_pass(self):
  s=assess(fixture())
  self.assertTrue(s["structuralReadyForIndependentReview"])
  self.assertEqual(s["blockingReasons"],[])
 def test_02_never_accept_runtime(self):
  self.assertFalse(assess(fixture())["runtimeEndToEndAccepted"])
 def test_03_missing_scene(self):
  d=fixture();d["events"]=[x for x in d["events"] if x["phase"]!="scene_ready"]
  self.assertIn("missing_essential_phase",assess(d)["blockingReasons"])
 def test_04_wrong_world(self):
  d=fixture();d["events"][2]["mapCopyId"]=54
  self.assertIn("not_original_map53_worldsync",assess(d)["blockingReasons"])
 def test_05_unordered_sequence(self):
  d=fixture();d["events"][5]["seq"]=1
  self.assertIn("sequence_not_monotonic",assess(d)["blockingReasons"])
 def test_06_cross_actor(self):
  d=fixture();d["events"][7]["actorId"]="actor-intruder"
  self.assertIn("actor_or_session_mismatch",assess(d)["blockingReasons"])
 def test_07_ack_mismatch(self):
  d=fixture();d["events"][7]["requestId"]="unlinked"
  self.assertIn("movement_ack_echo_not_linked",assess(d)["blockingReasons"])
 def test_08_move_not_user_clicked(self):
  d=fixture();d["events"][6]["userClickedAfterScene"]=False
  self.assertIn("movement_not_user_initiated_after_scene",assess(d)["blockingReasons"])
 def test_09_credential_input_rejected(self):
  d=fixture();d["events"][0]["password"]="NEVER_LOG_THIS"
  with self.assertRaises(ValueError):assess(d)
 def test_10_no_active_affine(self):
  d=fixture();d["events"][9]["activeRuntimeMatrix"]=False
  self.assertIn("map53_affine_anchor_gate_incomplete",assess(d)["blockingReasons"])
 def test_11_collinear_points(self):
  d=fixture();d["events"][10]["points"][2]["mps"]=[20,0]
  self.assertIn("map53_affine_anchor_gate_incomplete",assess(d)["blockingReasons"])
 def test_12_holdout_not_gate_without_tolerance(self):
  d=fixture();d["events"][10]["points"][3]["unity"]=[18,18]
  result=assess(d)
  self.assertTrue(result["structuralReadyForIndependentReview"])
  self.assertGreater(result["affine"]["holdoutError"],1)
  self.assertTrue(result["affine"]["reviewerThresholdRequired"])
  self.assertFalse(result["runtimeEndToEndAccepted"])
 def test_13_screenshot_digest_missing(self):
  d=fixture();del d["provenance"]["scene-screenshot"]
  self.assertIn("missing_verifiable_source_digests",assess(d)["blockingReasons"])
 def test_14_mock_mode_rejected(self):
  d=fixture();d["mode"]="synthetic"
  self.assertIn("not_a_live_qa_session",assess(d)["blockingReasons"])
 def test_15_wrong_provenance_source(self):
  d=fixture();d["events"][7]["origin"]="original-client"
  self.assertIn("invalid_claimed_source_gs_move_ack",assess(d)["blockingReasons"])
 def test_16_bad_schema(self):
  d=fixture();d["schema"]="unsupported"
  with self.assertRaises(ValueError):assess(d)
 def test_17_no_real_ip_or_packet(self):
  s=assess(fixture())
  self.assertNotIn("password",str(s))
  self.assertNotIn("token",str(s))
  self.assertNotIn("raw_packet",str(s))
 def test_18_bad_movement_coordinates(self):
  d=fixture();d["events"][7]["x"]=float("nan")
  self.assertIn("invalid_movement_coordinates",assess(d)["blockingReasons"])
 def test_19_geometry_math(self):
  d=geometric_quality([1,0,0,0,1,0],[
    {"mps":[0,0],"unity":[0,0]},
    {"mps":[1,0],"unity":[1,0]},
    {"mps":[0,1],"unity":[0,1]},
    {"mps":[2,3],"unity":[2,3]}])
  self.assertTrue(d["threeNonCollinear"])
  self.assertEqual(d["holdoutError"],0)
if __name__=="__main__":
 unittest.main(verbosity=2)

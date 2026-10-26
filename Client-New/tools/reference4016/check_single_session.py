"""CN02-BULK-06: validate a *sanitized* single-session QA evidence envelope.

A structural check CANNOT authenticate claimed log sources or certify gameplay.
No raw packet, credential, ticket, GUID, address, or original client output
should be embedded in the envelope. Independent runtime review is mandatory.
"""
import argparse
import json
import math
import re
from pathlib import Path

ROOT=Path(__file__).resolve().parents[2]
OUTSIDE_GIT=(ROOT.parent/"Tool/_outputs/cn02-bulk/session-qa").resolve()
PHASES=(
 "bishop_auth", "gs_sync", "world_sync", "load_map", "asset_verified",
 "scene_ready", "user_move_request", "gs_move_ack", "unity_move_echo"
)
SOURCES={
 "bishop_auth":"bishop",
 "gs_sync":"gameserver",
 "world_sync":"gameserver",
 "load_map":"original-client",
 "asset_verified":"original-client",
 "scene_ready":"original-client",
 "user_move_request":"original-client",
 "gs_move_ack":"gameserver",
 "unity_move_echo":"original-client",
 "affine":"original-client",
 "landmarks":"qa-reviewer",
}
DENY_KEYS={"password","token","access_token","refresh_token","credential",
           "account","username","ip_address","ip","raw_packet","raw_payload",
           "ticket","cookie","authorization","email"}
def valid_number(v):
 return type(v) in (float,int) and math.isfinite(v)

def geometric_quality(affine,points):
 result={"coefficientsPresent":False,"threeNonCollinear":False,
         "sameScene":False,"holdoutError":None,"reviewerThresholdRequired":True}
 if not isinstance(affine,list) or len(affine)!=6 or not all(map(valid_number,affine)):
  return result
 result["coefficientsPresent"]=True
 if not isinstance(points,list) or len(points)!=4:
  return result
 try:
  xyz=[(p["mps"],p["unity"]) for p in points]
  if any(len(a)!=2 or len(b)!=2 or not all(map(valid_number,a+b)) for a,b in xyz):
   return result
  x1,y1=xyz[0][0];x2,y2=xyz[1][0];x3,y3=xyz[2][0]
  area=abs((x2-x1)*(y3-y1)-(x3-x1)*(y2-y1))
  result["threeNonCollinear"]=area>1e-8
  a,b,c,d,e,f=affine
  last=xyz[3]
  x,y=last[0];u,v=last[1]
  result["holdoutError"]=round(math.hypot(a*x+b*y+c-u,d*x+e*y+f-v),6)
 except (KeyError,TypeError,ValueError,OverflowError):
  pass
 return result

def forbidden_fields(value):
 if isinstance(value,dict):
  for k,v in value.items():
   if isinstance(k,str) and k.lower() in DENY_KEYS:return True
   if forbidden_fields(v):return True
 elif isinstance(value,list):
  return any(forbidden_fields(x) for x in value)
 return False

def assess(envelope):
 if not isinstance(envelope,dict): raise ValueError("Expected JSON object")
 if forbidden_fields(envelope): raise ValueError("Sensitive/raw data keys forbidden")
 if envelope.get("schema")!="cn02-single-session-v1":raise ValueError("Unexpected schema")
 sess=envelope.get("sessionId","")
 actor=envelope.get("actorId","")
 if not isinstance(sess,str) or not re.fullmatch(r"qa-[a-zA-Z0-9_-]{5,64}",sess):
  raise ValueError("Use pseudonymous qa-* session")
 if not isinstance(actor,str) or not re.fullmatch(r"actor-[a-zA-Z0-9_-]{3,64}",actor):
  raise ValueError("Use pseudonymous actor-* actor")
 events=envelope.get("events")
 if not isinstance(events,list) or len(events)>1000:raise ValueError("events must be bounded")
 byphase={}
 issues=[]
 prev_seq=-1;prev_ts=-math.inf
 for ev in events:
  if not isinstance(ev,dict):raise ValueError("Event must be JSON object")
  phase=ev.get("phase")
  if phase not in (*PHASES,"affine","landmarks"):
   issues.append("unrecognized_phase")
   continue
  if phase in byphase:issues.append("duplicate_phase_"+phase)
  byphase[phase]=ev
  seq=ev.get("seq");ts=ev.get("tsMs")
  if not isinstance(seq,int) or isinstance(seq,bool) or seq<=prev_seq:
   issues.append("sequence_not_monotonic")
  else:prev_seq=seq
  if not valid_number(ts) or ts<prev_ts:
   issues.append("timestamp_not_monotonic")
  else:prev_ts=ts
  if ev.get("sessionId")!=sess or ev.get("actorId")!=actor:
   issues.append("actor_or_session_mismatch")
  if ev.get("origin")!=SOURCES[phase]:
   issues.append("invalid_claimed_source_"+phase)
 if not all(phase in byphase for phase in PHASES):
  issues.append("missing_essential_phase")
 elif [byphase[x]["seq"] for x in PHASES]!=sorted(byphase[x]["seq"] for x in PHASES):
  issues.append("essential_phases_out_of_order")
 world=byphase.get("world_sync",{})
 selector=byphase.get("load_map",{})
 if (world.get("subWorld"),world.get("mapCopyId"),world.get("region"))!=(53,53,6488114):
  issues.append("not_original_map53_worldsync")
 if (selector.get("templateId"),selector.get("mapId"),selector.get("region"),
     selector.get("mpsX"),selector.get("mpsY"))!=(53,53,6488114,0,0):
  issues.append("not_original_map53_loadmap_selector")
 if byphase.get("bishop_auth",{}).get("accepted") is not True:issues.append("bishop_not_accepted")
 if byphase.get("gs_sync",{}).get("accepted") is not True:issues.append("gs_not_accepted")
 if byphase.get("asset_verified",{}).get("catalogMatch") is not True or byphase.get("asset_verified",{}).get("integrityOk") is not True:
  issues.append("scene_bundle_integrity_not_verified")
 scene=byphase.get("scene_ready",{})
 if scene.get("mapId")!=53 or scene.get("avatarVisible") is not True:
  issues.append("scene_not_ready_or_actor_not_visible")
 request=byphase.get("user_move_request",{})
 ack=byphase.get("gs_move_ack",{})
 echo=byphase.get("unity_move_echo",{})
 rid=request.get("requestId")
 if not isinstance(rid,str) or not rid or len(rid)>64:
  issues.append("bad_movement_request_reference")
 if request.get("userClickedAfterScene") is not True:
  issues.append("movement_not_user_initiated_after_scene")
 if not rid or rid!=ack.get("requestId") or rid!=echo.get("requestId"):
  issues.append("movement_ack_echo_not_linked")
 if any(not valid_number(event.get("x")) or not valid_number(event.get("y"))
        for event in (ack,echo)):
  issues.append("invalid_movement_coordinates")
 affine=byphase.get("affine",{})
 landmarks=byphase.get("landmarks",{})
 geometric=geometric_quality(affine.get("coefficients"),
                             landmarks.get("points"))
 geometric["sameScene"]=landmarks.get("sameScene") is True and affine.get("activeRuntimeMatrix") is True
 if not geometric["coefficientsPresent"] or not geometric["threeNonCollinear"] or not geometric["sameScene"]:
  issues.append("map53_affine_anchor_gate_incomplete")
 provenance=envelope.get("provenance",{})
 confirmed=isinstance(provenance,dict) and all(
  isinstance(provenance.get(k),str) and
  re.fullmatch("[0-9a-fA-F]{64}",provenance.get(k,""))
  for k in ("bishop","gameserver","original-client","scene-screenshot")
 )
 if not confirmed:issues.append("missing_verifiable_source_digests")
 if envelope.get("mode")!="live":issues.append("not_a_live_qa_session")
 ordered=[]
 for item in issues:
  if item not in ordered:ordered.append(item)
 return {
  "schema":"cn02-single-session-report-v1",
  "sessionPseudonym":sess,"actorPseudonym":actor,
  "eventCount":len(events),"distinctPhases":len(byphase),
  "structuralReadyForIndependentReview":not ordered,
  "blockingReasons":ordered,
  "movementEchoPositionDelta":round(math.hypot(ack["x"]-echo["x"],ack["y"]-echo["y"]),6)
   if all(valid_number(ev.get(k)) for ev in (ack,echo) for k in ("x","y")) else None,
  "affine":geometric,
  "runtimeEndToEndAccepted":False,
  "independentReviewRequired":True,
  "note":"Only checks self-consistency of claimed sanitized evidence. Source hashes are not authenticated by this script; raw log files, scene screenshot, game server ACK and holdout tolerance need independent verification. Mock fixtures are NEVER evidence of actual gameplay."
 }

def main():
 p=argparse.ArgumentParser()
 p.add_argument("--input",type=Path,required=True)
 p.add_argument("--output",type=Path,required=True)
 a=p.parse_args()
 source=a.input.resolve();out=a.output.resolve()
 if not (source.is_relative_to(OUTSIDE_GIT) and out.is_relative_to(OUTSIDE_GIT)):
  p.error("Input and output must stay outside Git under Tool/_outputs/cn02-bulk/session-qa/")
 if source==out or out.exists():p.error("Do not overwrite evidence files")
 result=assess(json.loads(source.read_text(encoding="utf8")))
 out.parent.mkdir(parents=True,exist_ok=True)
 out.write_text(json.dumps(result,indent=2),encoding="utf8")
 print("SESSION_EVIDENCE",result["structuralReadyForIndependentReview"],
       "RUNTIME_ACCEPTED",result["runtimeEndToEndAccepted"],
       "BLOCKERS",",".join(result["blockingReasons"]))
if __name__=="__main__": main()

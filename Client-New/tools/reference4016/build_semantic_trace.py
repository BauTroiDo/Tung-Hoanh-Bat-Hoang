"""CN02-BULK-05: evidence-graded login/worldsync/map/scene/walk trace.

This correlates separately recorded historical QA observations and native static
IL2CPP evidence. It is NOT a live capture, on-wire packet dump, or runtime proof.
Raw logs, credentials, original signatures and pseudo-code never enter summary.
"""
import argparse,collections,hashlib,json,sqlite3
from pathlib import Path

ROOT=Path(__file__).resolve().parents[2]
GAME=ROOT.parent
QA=GAME/"Client-3D-2/LAN-LOCAL"
REF=GAME/"Client-3D-2/update"
EVIDENCE=ROOT/"reports/evidence/cn02-4016-il2cpp"
DB=GAME/"Tool/_outputs/cn02-bulk/method-index.sqlite"
EXPECTED_BIN="011357720B210AA1F392A8B62FF8DE090D3A895230A6E0135BCE707196871069"
EXPECTED_META="5586F416AA26EBE57EA7F6580D0A53E5E5F29C50E425E64A4D3F2A95D6069E08"
SEMANTIC_EDGES=[
 ("ProcessGameUnity.OnSyncWorld","MapService.UpdateWorldInfo",0xC38D1F,0x8E0DD0),
 ("MapService.UpdateWorldInfo","SubWorld.LoadMap",0x8E1094,0xC75DE0),
 ("MapService.UpdateWorldInfo","SubWorld.Activate",0x8E10E8,0xC753C0),
]
CONTRACTS=(
 ("BishopNetwork::Login()",0x8F3750),
 ("GameServerNetwork::ConnectToServer()",0x92CA30),
 ("Network.Header.ProcessGameUnity::OnSyncWorld()",0xC38990),
 ("MapDataService::LoadForTemplate()",0x8C9BF0),
 ("VietKiem.Levels.Application.SceneLoadService::ResolveVariantKeyAsync()",0xC74650),
 ("WalkState::Enter()",0x87B290),
 ("StateMachine::ChangeState()",0x878A30),
)
def sha(path):
 h=hashlib.sha256()
 with Path(path).open("rb") as f:
  for ch in iter(lambda:f.read(4*1024*1024),b""):h.update(ch)
 return h.hexdigest().upper()

def jsonl(path):
 valid=[];invalid=0
 with Path(path).open(encoding="utf8",errors="replace") as f:
  for line in f:
   try:
    a=json.loads(line)
    if isinstance(a,dict):valid.append(a)
    else:invalid+=1
   except json.JSONDecodeError:invalid+=1
 return valid,invalid

def event_aggregate(path):
 records,invalid=jsonl(path)
 kinds=collections.Counter()
 worlds=set()
 pending=0
 for item in records:
  payload=item.get("payload")
  if not isinstance(payload,dict):continue
  kind=payload.get("kind")
  if not isinstance(kind,str):continue
  kinds[kind]+=1
  if kind=="receivedWorldMap":
   if all(isinstance(payload.get(k),int) for k in ("subWorld","mapCopyId","region")):
    worlds.add((payload["subWorld"],payload["mapCopyId"],payload["region"]))
  if kind=="originalAvatarCreationReturn" and payload.get("creationPending") is True:
   pending+=1
 return {
  "file":Path(path).name,"validJsonRecords":len(records),"invalidJsonLines":invalid,
  "kindCounts":dict(sorted(kinds.items())),"observedWorldTriples":[
   {"subWorld":w,"mapCopyId":t,"region":r} for w,t,r in sorted(worlds)],
  "avatarCreationPendingCount":pending,
  "note":"Instrumented historical Frida/test-harness events, NOT raw authenticated network frames."
 }

def http_aggregate(path):
 rows,invalid=jsonl(path)
 codes=collections.Counter()
 methods=collections.Counter()
 balang=0;pcbalang=0
 pcbalang_status=collections.Counter()
 pcbalang_lengths=[]
 for row in rows:
  code=row.get("status")
  if isinstance(code,int):codes[str(code)]+=1
  method=row.get("method")
  if isinstance(method,str):methods[method]+=1
  name=row.get("name")
  if isinstance(name,str):
   low=name.lower()
   if "balanghuyen" in low:
    balang+=1
    if "pc_balanghuyen" in low:
     pcbalang+=1
     if isinstance(row.get("status"),int):pcbalang_status[str(row["status"])]+=1
     if isinstance(row.get("length"),int):pcbalang_lengths.append(row["length"])
 return {
  "file":Path(path).name,"jsonRecords":len(rows),"malformedLines":invalid,
  "httpMethods":dict(sorted(methods.items())),
  "httpStatusCounts":dict(sorted(codes.items())),
  "recordsMatchingBalanghuyenBundleName":balang,
  "recordsMatchingPcBalanghuyenBundleName":pcbalang,
  "pcBalangStatusCounts":dict(sorted(pcbalang_status.items())),
  "pcBalangResponseLengthsBytes":sorted(pcbalang_lengths),
  "note":"Historical localhost HTTP mirror records are NOT current official CDN availability or proof of Unity scene completion. No URLs or asset names exported."
 }

def native_edges():
 d=json.loads((EVIDENCE/"worldsync-contract.json").read_text(encoding="utf8"))
 if d["binarySHA256"]!=EXPECTED_BIN or d["metadataSHA256"]!=EXPECTED_META:
  raise ValueError("static worldsync evidence wrong version")
 result=[]
 for owner,dest,callrva,target in SEMANTIC_EDGES:
  calls=d["results"][owner]["calls"]
  matched=any(int(x["atRVA"],16)==callrva and int(x["targetRVA"],16)==target for x in calls)
  if not matched:raise AssertionError("Callsite not present "+owner+" -> "+dest)
  result.append({"caller":owner,"callee":dest,"callsiteRva":hex(callrva),
                 "calleeRva":hex(target),"verifiedDirectCall":True})
 return result

def scene_static_edges():
 # The map-load split is verified by two independent static audits.
 l=json.loads((EVIDENCE/"loadmap-selector.json").read_text(encoding="utf8"))
 scene=json.loads((EVIDENCE/"scene-pack-resolver.json").read_text(encoding="utf8"))
 if l["binarySHA256"]!=EXPECTED_BIN or scene["binarySHA256"]!=EXPECTED_BIN:
  raise ValueError("Scene trace wrong binary")
 links=[
  ("SubWorld.LoadMap","MapDataService.LoadForTemplate",0xC762AC,0x8C9BF0),
  ("SubWorld.LoadMap","GameManager.LoadScene",0xC7638A,0x918B70),
 ]
 results=[]
 for source,dest,at,target in links:
  if not any(int(x["atRVA"],16)==at and int(x["targetRVA"],16)==target
             for x in l["knownMethodCalls"]):
   raise AssertionError("Static scene-call missing "+dest)
  results.append({"caller":source,"callee":dest,"callsiteRva":hex(at),
                  "calleeRva":hex(target),"verifiedDirectCall":True})
 folder=next(x for x in scene["traces"] if x["method"]=="MapDataService.LoadForTemplate")
 if not any(int(x["rva"],16)==0x8C9F59 and int(x["targetRVA"],16)==0x8CD240
            for x in folder["knownCallees"]):
  raise AssertionError("MapDataService.ResolveMapFolderPath link missing")
 results.append({"caller":"MapDataService.LoadForTemplate",
                 "callee":"MapDataService.ResolveMapFolderPath",
                 "callsiteRva":"0x8c9f59","calleeRva":"0x8cd240","verifiedDirectCall":True})
 return results

def method_contracts():
 c=sqlite3.connect("file:"+DB.resolve().as_posix()+"?mode=ro",uri=True)
 try:
  found=[]
  for label,rva in CONTRACTS:
   rv=[int(row[0]) for row in c.execute("SELECT rva FROM methods WHERE dotnet_signature=? AND assembly='Assembly-CSharp'",(label,))]
   if rva not in rv:raise AssertionError("Method anchor missing: "+label)
   found.append({"method":label,"rva":hex(rva),"metadataIndexMatches":True})
  return found
 finally:c.close()

def named_world_bindings(worldtriples):
 types=json.loads((EVIDENCE/"worldsync-field-types.json").read_text(encoding="utf8"))
 if types["sourceSHA256"]!=EXPECTED_META:raise ValueError("Metadata field evidence version drift")
 source={x["callArgument"]:x for x in types["nativeLoadMapBindings"]}
 expected={
  "nTemplateId":("MapCopyID",20,"edx"),
  "nMapId":("SubWorld",4,"r8d"),
  "nRegion":("Region",8,"r9d")
 }
 bindings=[]
 for formal,(field,offset,reg) in expected.items():
  item=source[formal]
  if (item["sourceField"],item["sourceOffset"],item["nativeRegister"])!=(field,offset,reg):
   raise AssertionError("Native WORLD_SYNC mapping drift")
  bindings.append({"formalParameter":formal,"worldSyncField":field,
                   "byteOffset":offset,"register":reg})
 for formal in ("nMpsX","nMpsY"):
  if source[formal].get("sourceValue")!="zeroed-esi":
   raise AssertionError("Expected zeroed MPS fields changed")
  bindings.append({"formalParameter":formal,"observedNativeArgument":"zero in path"})
 projected=[{"nTemplateId":e["mapCopyId"],"nMapId":e["subWorld"],
             "nRegion":e["region"],"nMpsX":0,"nMpsY":0} for e in worldtriples]
 return {"formalBindings":bindings,
         "projectedHistoricalMapSelectorInputs":projected,
         "qualification":"A projection of instrumented historical fields through verified native call convention, NOT a direct packet capture or Map53 scene/affine calibration."}

def detect_documentary_limits():
 # These are SEPARATE historical dates/sessions, NOT a single packet capture.
 auth=(QA/"M1-ACCOUNT-CHECK-AND-GS-HANDSHAKE-20261024.md").read_text(encoding="utf8",errors="replace")
 cont=(QA/"M1-P0-CONTINUATION-20261025.md").read_text(encoding="utf8",errors="replace")
 asset=(QA/"M1-MAP53-ASSET-RECOVERY-20261024.md").read_text(encoding="utf8",errors="replace")
 assert "Bishop authenticated successfully" in auth
 assert "real GS initial sync" in cont
 assert "No avatar/movement acceptance yet" in cont
 assert "Status: NOT PASS." in asset
 return {
   "historicalBishopAuthReported":True,
   "historicalGsInitialSyncReported":True,
   "historicalOriginalGsAckReported":("ACK once" in cont),
   "historicalAssetOrSceneGateNotPassed":True,
   "historicalAuthoritativeMoveAcceptance":False,
   "warning":"Statements come from 2026-10-24 and 2026-10-25 historical handoff documents; different sessions, not an independently verified fresh packet trace."
 }

def generate():
 binpath=REF/"GameAssembly.dll"
 metapath=REF/"Game_Data/il2cpp_data/Metadata/global-metadata.dat"
 if sha(binpath)!=EXPECTED_BIN or sha(metapath)!=EXPECTED_META:raise ValueError("4016 source checksum changed")
 region=event_aggregate(QA/"M1-map53-region-events.jsonl")
 fixed=event_aggregate(QA/"M1-map53-fixed-events.jsonl")
 http=http_aggregate(QA/"M1-map53-http-events.jsonl")
 secondary=detect_documentary_limits()
 hasworld=region["kindCounts"].get("receivedWorldMap",0)>0
 hasmap=region["kindCounts"].get("originalMapLoadAttempt",0)>0
 hasverifiedscene=any(region["kindCounts"].get(k,0)>0 for k in
    ("originalSceneLoaded","originalSceneLoadCompleted","verifiedRenderedMap53"))
 hasverifiedmove=any(region["kindCounts"].get(k,0)>0 for k in
    ("authoritativeMovementAck","originalAuthoritativeMoveConfirmed"))
 return {
  "stage":"CN02-BULK-05","sourceBinarySHA256":EXPECTED_BIN,
  "sourceMetadataSHA256":EXPECTED_META,
  "historicalTraces":{"region":region,"fixed":fixed,"http":http},
  "documentarySecondaryObservations":secondary,
  "nativeDirectCallEdges":native_edges(),
  "scenePathDirectCallEdges":scene_static_edges(),
  "namedWorldSyncToLoadMap":named_world_bindings(region["observedWorldTriples"]),
  "knownMethodRvaContracts":method_contracts(),
  "stageAssessments":[
   {"name":"Bishop authorization","status":"DOCUMENTED_HISTORICALLY","evidenceType":"retrospective QA report; no redacted raw ticket capture"},
   {"name":"GameServer initial sync/ACK","status":"DOCUMENTED_HISTORICALLY","evidenceType":"QA handoff for separate sessions; not same-session proof as instrumented events"},
   {"name":"WORLD_SYNC field/selector","status":"OBSERVED_IN_HISTORICAL_INSTRUMENTED_EVENTS" if hasworld else "NOT_SEEN",
    "evidenceType":"instrumented receivedWorldMap / subworldMapSelector values; static native mapping"},
   {"name":"SubWorld/Map load attempt","status":"OBSERVED_ATTEMPT" if hasmap else "NOT_SEEN","evidenceType":"originalMapLoadAttempt event, not scene success"},
   {"name":"Addressables/scene rendered","status":"NOT_VERIFIED" if not hasverifiedscene else "CLAIM_REQUIRES_INDEPENDENT_REVIEW",
    "evidenceType":"historical CDN/localhost logs with no independently confirmed rendered map"},
   {"name":"WalkState real authoritative movement","status":"NOT_VERIFIED" if not hasverifiedmove else "CLAIM_REQUIRES_INDEPENDENT_REVIEW",
    "evidenceType":"RVA only; no same-session server-authenticated move and echoed position"},
  ],
  "runtimeEndToEndPass":False,
  "map53CalibrationAccepted":False,
  "limits":"Do not join events from different historical runs into a single authenticated end-to-end packet trace. No raw GUID/password/name/ticket/payload persisted. Neither full scene load nor GameServer-authoritative movement was observed. Map53 6 runtime affine floats and genuine 3+1 paired original landmarks remain missing. CN02-04 BLOCKED / CN03 CLOSED."
 }

def main():
 p=argparse.ArgumentParser()
 p.add_argument("--out",type=Path,required=True)
 a=p.parse_args()
 s=generate()
 a.out.parent.mkdir(parents=True,exist_ok=True)
 a.out.write_text(json.dumps(s,ensure_ascii=False,indent=2),encoding="utf8")
 print("SEMANTIC_GATES",[(x["name"],x["status"]) for x in s["stageAssessments"]])
 print("EVENTS",s["historicalTraces"]["region"]["kindCounts"])
 print("HTTP_STATUS",s["historicalTraces"]["http"]["httpStatusCounts"])
 print("STATIC_EDGES",len(s["nativeDirectCallEdges"]),"RVA",len(s["knownMethodRvaContracts"]))
if __name__=="__main__":main()

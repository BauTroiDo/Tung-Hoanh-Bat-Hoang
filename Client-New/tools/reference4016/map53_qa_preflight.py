"""CN02-BULK-08: read-only localhost mirror exercise and native template selector preflight.

Loads the existing QA mirror as a Python module, but redirects *all* its HTTP
audit output outside reference client tree and listens only on an ephemeral
127.0.0.1 port. Does not run original Game.exe or write GameServer/Unity/cache.
"""
import argparse
import contextlib
import hashlib
import importlib.util
import json
import socket
import threading
import urllib.error
import urllib.request
from pathlib import Path

ROOT=Path(__file__).resolve().parents[2]
GAME=ROOT.parent
LAN=GAME/"Client-3D-2/LAN-LOCAL"
MIRROR_PY=LAN/"m1_map53_asset_server.py"
RAW_BASE=(GAME/"Tool/_outputs/cn02-bulk/map53-runtime-preflight").resolve()
EVID=ROOT/"reports/evidence/cn02-4016-il2cpp"
PINS={
 "pc_balanghuyen_assets_all_53fcdd524b8c67eafad5c013f036ed4b.bundle":
  31592781,
 "pc_balanghuyen_scenes_all_4e0a8e141624756b20ca4ccfa32087fb.bundle":
  9738990,
}
REF_HASH="5586F416AA26EBE57EA7F6580D0A53E5E5F29C50E425E64A4D3F2A95D6069E08"
STAGED_HASH="C677837DEC676A3F03A33333A9B94620ED373962E844704D6B9D1E451BFE1987"
def sha(path):
 h=hashlib.sha256()
 with Path(path).open("rb") as stream:
  for b in iter(lambda:stream.read(4*1024*1024),b""):
   h.update(b)
 return h.hexdigest().upper()

def project_template(world):
 """Source names/semantics supported by IL2CPP native mapping, not wire parsing."""
 return {
  "nTemplateId":world["MapCopyID"],
  "nMapId":world["SubWorld"],
  "nRegion":world["Region"],
  "nMpsX":0,"nMpsY":0,
 }

def map53_assertion(world):
 mapped=project_template(world)
 return {
  "projected":mapped,
  "knownHistoricalMap53CaseMatches":
     (mapped["nTemplateId"],mapped["nMapId"],mapped["nRegion"])==
      (53,53,6488114),
  "reason":("MapCopyID is nTemplateId / scene selector; a value 0 in the "
    "Map53 historical case is NOT equivalent to template 53. In general, "
    "SubWorld and Template IDs are independent and must not always be forced equal.")
 }

def http_probe(rawdir):
 rawdir=Path(rawdir).resolve()
 if rawdir==RAW_BASE or not rawdir.is_relative_to(RAW_BASE):
  raise ValueError("Only a dedicated fresh output directory under Tool/_outputs is allowed")
 if rawdir.exists():raise FileExistsError("Refuse to overwrite prior QA evidence")
 rawdir.mkdir(parents=True,exist_ok=False)
 spec=importlib.util.spec_from_file_location("m1_map53_asset_server_for_isolated_qa",MIRROR_PY)
 if spec is None or spec.loader is None:raise RuntimeError("Missing mirror script")
 module=importlib.util.module_from_spec(spec)
 spec.loader.exec_module(module)
 # Monkeypatch in memory *before* invoking the Handler, never change original script.
 module.LOG=rawdir/"mirror-http-events.jsonl"
 module.Handler.store=module.paths()
 store=module.Handler.store
 if not all(k in store for k in PINS):
  raise AssertionError("PC Map53 official bundles not mapped by mirror")
 results={"availableBundleNames":len(store),
          "officialStagedSources":sum("Map53Official4016" in str(v) for v in store.values()),
          "cachedUnityFsSources":sum(v.name=="__data" for v in store.values()),
          "tests":{}}
 with module.ThreadingHTTPServer(("127.0.0.1",0),module.Handler) as server:
  th=threading.Thread(target=server.serve_forever,daemon=True)
  th.start()
  base="http://127.0.0.1:%d%s"%(server.server_port,module.PREFIX)
  def req(path,method="GET",header=None):
   request=urllib.request.Request(base+path,method=method,headers=header or {})
   with urllib.request.urlopen(request,timeout=10) as rsp:
    return rsp.status,dict(rsp.headers),rsp.read() if method=="GET" else b""
  try:
   st,hd,_=req("catalog_1.0.hash","HEAD")
   if st!=200 or int(hd["Content-Length"])!=32:
    raise AssertionError("Catalog hash HEAD mismatch")
   results["tests"]["catalogHashHead"]={"status":st,"bytes":32}
   st,hd,body=req("catalog_1.0.hash")
   if st!=200 or len(body)!=32:raise AssertionError("Catalog hash response mismatch")
   results["tests"]["catalogHashGet"]={"status":st,"bytes":len(body)}
   st,hd,body=req("settings.json")
   decoded=json.loads(body)
   if st!=200 or decoded.get("m_buildTarget")!="StandaloneWindows64":
    raise AssertionError("Windows64 Addressables settings GET invalid")
   results["tests"]["addressablesSettingsGet"]={
     "status":st,"bytes":len(body),"buildTarget":decoded["m_buildTarget"]}
   for name,size in PINS.items():
    kind="assets" if "_assets_" in name else "scenes"
    st,hd,_=req(name,"HEAD")
    if st!=200 or int(hd["Content-Length"])!=size:
     raise AssertionError("Official PC bundle HEAD failed "+kind)
    results["tests"][kind+"Head"]={"status":st,"sizeBytes":size}
    st,hd,body=req(name,header={"Range":"bytes=0-6"})
    if st!=206 or body!=b"UnityFS" or hd.get("Content-Range")!=("bytes 0-6/%d"%size):
     raise AssertionError("Official PC bundle Range failed "+kind)
    results["tests"][kind+"Range"]={"status":st,"returnedBytes":len(body),
                                     "signature":"UnityFS","fullSizeBytes":size}
   try:
    req("not-a-real-catalog-resource.bundle")
    raise AssertionError("Missing resource did not fail")
   except urllib.error.HTTPError as err:
    if err.code!=404:raise
    results["tests"]["missingResourceGets404"]={"status":404}
  finally:
   server.shutdown()
   th.join(timeout=10)
   server.server_close()
 if not module.LOG.is_file():raise AssertionError("Isolated audit log missing")
 results["isolatedMirrorLogSHA256"]=sha(module.LOG)
 results["isolatedLogEntryCount"]=sum(1 for l in module.LOG.open(encoding="utf8") if l.strip())
 # All original client assets and HTTP mirror source were READ ONLY.
 return results

def tcp_gate(host,ports,timeout=1.1):
 if not host:return {"skipped":True,"reason":"No backend host supplied"}
 if not 0.2<=timeout<=3:raise ValueError("Invalid timeout")
 outputs={}
 for port in ports:
  try:
   with socket.create_connection((host,port),timeout=timeout):
    outputs[str(port)]={"reachable":True}
  except (TimeoutError,OSError):
   outputs[str(port)]={"reachable":False}
 return outputs

def run(rawdir,backend_host=None):
 original=LAN.parent/"update/Game_Data/il2cpp_data/Metadata/global-metadata.dat"
 staged=LAN/"global-metadata.map53local-4016.dat"
 backup=LAN/"global-metadata.map53local-original-20261024.dat"
 orig_digest=sha(original)
 staged_digest=sha(staged)
 backup_digest=sha(backup)
 if (orig_digest,staged_digest,backup_digest)!=(REF_HASH,STAGED_HASH,REF_HASH):
  raise RuntimeError("Original/staged metadata hashes changed; do not start QA")
 fields=json.loads((EVID/"worldsync-field-types.json").read_text(encoding="utf8"))
 assert {x["callArgument"]:x["sourceField"] for x in fields["nativeLoadMapBindings"]
  if x.get("sourceField") is not None}=={"nTemplateId":"MapCopyID","nMapId":"SubWorld","nRegion":"Region"}
 historical=map53_assertion({"SubWorld":53,"MapCopyID":53,"Region":6488114})
 candidate=map53_assertion({"SubWorld":53,"MapCopyID":0,"Region":6488114})
 if not historical["knownHistoricalMap53CaseMatches"] or candidate["knownHistoricalMap53CaseMatches"]:
  raise AssertionError("Map53 WORLD_SYNC negative fixture not distinguished")
 historical_report=(LAN/"M1-P0-CONTINUATION-20261025.md").read_text(encoding="utf8",errors="replace")
 if "MapCopyID0" not in historical_report:
  raise AssertionError("Historical converter concern no longer present in report")
 mirror=http_probe(rawdir)
 backend=tcp_gate(backend_host,[56722,51189,14180])
 return {
  "stage":"CN02-BULK-08",
  "metadataOriginalSha256":orig_digest,
  "metadataStagedSha256":staged_digest,
  "originalBackupMatches":backup_digest==orig_digest,
  "nativeWorldSyncFixture":historical,
  "reportedConverterZeroTemplateFixture":candidate,
  "converterStatus":"LOCAL PREFLIGHT DOES NOT READ SERVER SOURCE; see separate read-only server audit. No new live WORLD_SYNC packet captured.",
  "backendPorts":backend,
  "mirror":mirror,
  "originalGameClientExecuted":False,
  "metadataSwapped":False,
  "authoritativeGsMoveVerified":False,
  "originalSceneRendered":False,
  "qaRuntimeAcceptance":False,
  "note":"All HTTP observations were synthetic localhost client requests to an in-memory instance of the existing mirror, NOT a real Game.exe session. Backend TCP checks only verify port reachability at this instant; they do not authenticate or establish server health."
 }

def main():
 parser=argparse.ArgumentParser()
 parser.add_argument("--raw-dir",type=Path,required=True)
 parser.add_argument("--summary",type=Path,required=True)
 parser.add_argument("--backend-host")
 args=parser.parse_args()
 summary=run(args.raw_dir,args.backend_host)
 args.summary.parent.mkdir(parents=True,exist_ok=True)
 args.summary.write_text(json.dumps(summary,indent=2),encoding="utf8")
 print("MIRROR_HTTP_TESTS_PASS",len(summary["mirror"]["tests"]),
       "MAP53_TEMPLATE_ZERO_REJECTED",not summary["reportedConverterZeroTemplateFixture"]["knownHistoricalMap53CaseMatches"])
 print("BACKEND",summary["backendPorts"])
 print("RUNTIME_ACCEPTANCE",summary["qaRuntimeAcceptance"])
if __name__=="__main__":
 main()

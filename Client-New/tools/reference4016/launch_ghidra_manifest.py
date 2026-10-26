"""Generate and optionally run a bounded Ghidra PE64 probe from the gameplay manifest.

The actual method bounds come from .pdata calculated by build_gameplay_manifest,
NOT from hardcoded RVA values. All logs and binaries remain outside Git.
"""
import argparse
import json
import os
import subprocess
import time
from pathlib import Path
from build_gameplay_manifest import ROOT,GAME,DLL,EXPECTED_SHA,sha,OUTPUT_BASE,PRIORITY

GHIDRA=GAME/"Tool/_built/Ghidra/ghidra_12.2_PUBLIC/support/analyzeHeadless.bat"
SCRIPT=ROOT/"tools/reference4016/ghidra/CN02DynamicBatch.java"
def sample(manifest):
    data=json.loads(Path(manifest).read_text(encoding="utf8"))
    if data.get("schema")!="CN02-BULK-03-v1" or data.get("originalSha256")!=EXPECTED_SHA:
        raise ValueError("Unrecognized manifest")
    picks=[]
    for group in PRIORITY:
        chosen=None
        for typ in data["recommended"].get(group,[]):
            for entry in typ["exactEntrypoints"]:
                n=entry["bytes"]
                if 128<=n<=1500:
                    chosen={"group":group,"type":typ["type"],"rva":entry["rva"],"bytes":n}
                    break
            if chosen:break
        if chosen is None:raise ValueError("Group has no safe sample "+group)
        picks.append(chosen)
    if len({t["rva"] for t in picks})!=4:raise ValueError("Sample RVA collision")
    return picks

def execute(manifest,output,dry_run=False,timeout=240):
    output=Path(output).resolve()
    if output==OUTPUT_BASE or not output.is_relative_to(OUTPUT_BASE):
        raise ValueError("Output must be new dir inside authorized Tool gameplay-batches")
    if output.exists():raise FileExistsError("Refuse to overwrite Ghidra evidence")
    if not 60<=timeout<=600:raise ValueError("Bound timeout 60..600")
    if sha(DLL)!=EXPECTED_SHA:raise ValueError("Original binary source drift")
    picks=sample(manifest)
    cmd=[str(GHIDRA),str(output),"CN02DynamicManifest","-import",str(DLL),
         "-noanalysis","-readOnly","-deleteProject",
         "-log",str(output/"ghidra.log"),"-scriptlog",str(output/"script.log"),
         "-scriptPath",str(SCRIPT.parent),"-postScript",SCRIPT.name]
    cmd.extend("0x%x:%d"%(p["rva"],p["bytes"]) for p in picks)
    if dry_run:
        print("GHIDRA_MANIFEST_DRY_RUN",json.dumps(picks))
        return picks
    output.mkdir(parents=True)
    started=time.monotonic()
    result={"sampleRvas":picks,"command":cmd,"sourceHash":EXPECTED_SHA}
    # Desktop Commander processes may inherit stale PATH/JAVA_HOME even after
    # User environment was saved to registry; locate project-verified JDK.
    env=os.environ.copy()
    java_home=Path(env.get("JAVA_HOME") or r"C:\Program Files\Java\jdk-25.0.5.0.3")
    if not (java_home/"bin/java.exe").is_file():
        raise FileNotFoundError("Java 25 unavailable: "+str(java_home))
    env["JAVA_HOME"]=str(java_home)
    env["PATH"]=str(java_home/"bin")+os.pathsep+env.get("PATH","")
    with (output/"console.log").open("wb") as log:
        try:
            proc=subprocess.run(cmd,stdout=log,stderr=subprocess.STDOUT,
                                stdin=subprocess.DEVNULL,env=env,
                                timeout=timeout,check=False)
            result["exitCode"]=proc.returncode
        except subprocess.TimeoutExpired:
            result["exitCode"]=124
            result["timedOut"]=True
    result["elapsedSeconds"]=round(time.monotonic()-started,2)
    (output/"run.json").write_text(json.dumps(result,indent=2),encoding="utf8")
    print("GHIDRA_MANIFEST_RUN_EXIT",result["exitCode"],"elapsed",result["elapsedSeconds"])
    if result["exitCode"]:raise SystemExit(result["exitCode"])
    return picks
def main():
    p=argparse.ArgumentParser()
    p.add_argument("--manifest",type=Path,required=True)
    p.add_argument("--output",type=Path,required=True)
    p.add_argument("--timeout",type=int,default=240)
    p.add_argument("--dry-run",action="store_true")
    a=p.parse_args()
    execute(a.manifest,a.output,a.dry_run,a.timeout)
if __name__=="__main__":main()

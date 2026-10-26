"""Automated bounded Ghidra read-only PE64 probe for three CN02-BULK-04 domains."""
import argparse,json,os,subprocess,time
from pathlib import Path
from build_extended_manifest import OUT_BASE,EXPECTED_SHA,DLL,sha
from launch_ghidra_manifest import GHIDRA,SCRIPT

PREFERRED={
 "NetworkProtocol":"Network/Header/ProcessGameUnity",
 "SceneAddressables":"VietKiem/Levels/Application/SceneLoadService",
 "GameplayState":"WalkState"
}
def sample(manifest):
    m=json.loads(Path(manifest).read_text(encoding="utf8"))
    if m.get("schema")!="CN02-BULK-04-v1" or m.get("binarySHA256")!=EXPECTED_SHA:
        raise ValueError("Invalid manifest")
    selected=[]
    for domain,typename in PREFERRED.items():
        r=next((t for t in m["domains"][domain] if t["typeName"]==typename),None)
        if not r:raise ValueError("Missing preferred type "+typename)
        entry=next((rva for rva in r["boundedEntrypoints"]
                    if 128<=rva["bytes"]<=1200),None)
        if not entry:raise ValueError("No safe bounded entry for "+typename)
        selected.append({"domain":domain,"type":typename,
                         "rva":entry["rva"],"bytes":entry["bytes"]})
    if len({x["rva"] for x in selected})!=len(selected):
        raise ValueError("RVA alias collision")
    if sum(x["bytes"] for x in selected)>8192:
        raise ValueError("aggregate Ghidra work too large")
    return selected

def execute(manifest,out,timeout=240,dry_run=False):
    out=Path(out).resolve()
    if out==OUT_BASE or not out.is_relative_to(OUT_BASE):
        raise ValueError("Outside authorized raw-output folder")
    if out.exists():raise FileExistsError("Do not overwrite Ghidra run")
    if not 60<=timeout<=600:raise ValueError("Timeout not bounded")
    if sha(DLL)!=EXPECTED_SHA:raise ValueError("Original PE hash drift")
    probes=sample(manifest)
    args=[str(GHIDRA),str(out),"CN02ExtendedProbe",
          "-import",str(DLL),"-noanalysis","-readOnly","-deleteProject",
          "-log",str(out/"ghidra.log"),"-scriptlog",str(out/"script.log"),
          "-scriptPath",str(SCRIPT.parent),"-postScript",SCRIPT.name]
    args.extend("0x%x:%d"%(x["rva"],x["bytes"]) for x in probes)
    if dry_run:
        print("EXTENDED_GHIDRA_DRY_RUN",json.dumps(probes))
        return probes
    java=Path(os.environ.get("JAVA_HOME") or r"C:\Program Files\Java\jdk-25.0.5.0.3")
    if not (java/"bin/java.exe").is_file():
        raise FileNotFoundError("JDK25 unavailable")
    env=os.environ.copy()
    env["JAVA_HOME"]=str(java)
    env["PATH"]=str(java/"bin")+os.pathsep+env.get("PATH","")
    out.mkdir(parents=True)
    start=time.monotonic()
    result={"probes":probes,"binarySHA256":EXPECTED_SHA,"script":SCRIPT.name}
    try:
        with (out/"console.log").open("wb") as f:
            proc=subprocess.run(args,stdout=f,stderr=subprocess.STDOUT,
                                stdin=subprocess.DEVNULL,env=env,timeout=timeout)
        result["exitCode"]=proc.returncode
    except subprocess.TimeoutExpired:
        result["exitCode"]=124
        result["timedOut"]=True
    result["elapsedSeconds"]=round(time.monotonic()-start,2)
    (out/"run.json").write_text(json.dumps(result,ensure_ascii=False,indent=2),encoding="utf8")
    print("EXTENDED_GHIDRA_EXIT",result["exitCode"],"seconds",result["elapsedSeconds"])
    if result["exitCode"]:raise SystemExit(result["exitCode"])
    return result

def main():
    p=argparse.ArgumentParser()
    p.add_argument("--manifest",required=True,type=Path)
    p.add_argument("--output",required=True,type=Path)
    p.add_argument("--timeout",default=240,type=int)
    p.add_argument("--dry-run",action="store_true")
    a=p.parse_args()
    execute(a.manifest,a.output,a.timeout,a.dry_run)
if __name__=="__main__":main()

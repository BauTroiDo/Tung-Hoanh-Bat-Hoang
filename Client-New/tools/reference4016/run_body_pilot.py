"""Run a bounded IL2CPP method-body pilot without whole-assembly filter traps.

Dumper 8.0.0 treats --pseudo-include-assembly and --pseudo-include-type
as a UNION. This wrapper only permits explicit fully-qualified TYPE filters.
The target output directory must not exist: never overwrite baseline.
"""
import argparse
import hashlib
import json
import subprocess
import time
from pathlib import Path

ROOT=Path(__file__).resolve().parents[2]
GAME_ROOT=ROOT.parent
TOOLS=GAME_ROOT/"Tool"
ORIGINAL=GAME_ROOT/"Client-3D-2"/"update"
EXE=TOOLS/"_built"/"il2cpp-dumper-rs"/"il2cpp_dumper.exe"
BINARY=ORIGINAL/"GameAssembly.dll"
METADATA=ORIGINAL/"Game_Data"/"il2cpp_data"/"Metadata"/"global-metadata.dat"
KNOWN={
 "binary":"011357720B210AA1F392A8B62FF8DE090D3A895230A6E0135BCE707196871069",
 "metadata":"5586F416AA26EBE57EA7F6580D0A53E5E5F29C50E425E64A4D3F2A95D6069E08",
}
DEFAULT_TYPES=("MapService","GameWorld.SubWorld","Network.Header.ProcessGameUnity")
def sha256(p):
    h=hashlib.sha256()
    with p.open("rb") as f:
        for b in iter(lambda:f.read(8*1024*1024),b""):
            h.update(b)
    return h.hexdigest().upper()

def command(output,types=DEFAULT_TYPES,max_body=8192):
    if not types or len(types)>12:raise ValueError("Must specify 1-12 types")
    if len(set(types))!=len(types):raise ValueError("Duplicate type filter")
    if any(not s or s.strip()!=s or any(c.isspace() for c in s) for s in types):
        raise ValueError("Invalid type filter")
    if not 1<=max_body<=16384:raise ValueError("max_body must be 1..16384")
    cmd=[str(EXE),"--binary",str(BINARY),"--metadata",str(METADATA),
         "--output",str(output),"--method-body-mode","both"]
    for t in types:cmd+=["--pseudo-include-type",t]
    return cmd+["--pseudo-max-body-size",str(max_body),
                "--no-generate-dummy-dll","--no-il-recovery",
                "--no-dump-static-metadata","--no-pseudo-prompt"]

def run(args):
    out=Path(args.output).resolve()
    # Raw decompilation may expose proprietary code; never write inside Git.
    allowed=(TOOLS/"_outputs"/"cn02-bulk").resolve()
    if out==allowed or not out.is_relative_to(allowed):
        raise ValueError("Output must be a new child of Tool/_outputs/cn02-bulk")
    cmd=command(out,tuple(args.type) if args.type else DEFAULT_TYPES,args.max_body_size)
    if out.exists():raise FileExistsError("Refuse to overwrite run: "+str(out))
    if not EXE.is_file():raise FileNotFoundError(EXE)
    hashes={"binary":sha256(BINARY),"metadata":sha256(METADATA)}
    if hashes!=KNOWN:raise ValueError("Source hash mismatch; refuse")
    if args.dry_run:
        print("SAFE_COMMAND",json.dumps(cmd,ensure_ascii=False))
        return 0
    out.mkdir(parents=True)
    t=time.monotonic()
    state={"binarySHA256":hashes["binary"],"metadataSHA256":hashes["metadata"],
           "selectedTypes":tuple(args.type) if args.type else DEFAULT_TYPES,
           "maxBodyBytes":args.max_body_size,"command":cmd,"timeoutSeconds":args.timeout}
    try:
        with (out/"run.log").open("wb") as f:
            result=subprocess.run(cmd,stdout=f,stderr=subprocess.STDOUT,timeout=args.timeout)
        state["exitCode"]=result.returncode
    except subprocess.TimeoutExpired:
        state["exitCode"]=124
        state["timedOut"]=True
    finally:
        state["elapsedSeconds"]=round(time.monotonic()-t,2)
        (out/"run.json").write_text(json.dumps(state,indent=2),encoding="utf-8")
    print("RUN_RESULT",state["exitCode"],"SECONDS",state["elapsedSeconds"],"OUTPUT",out)
    return state["exitCode"]

def main():
    p=argparse.ArgumentParser()
    p.add_argument("--output",type=Path,required=True)
    p.add_argument("--type",action="append")
    p.add_argument("--max-body-size",type=int,default=8192)
    p.add_argument("--timeout",type=int,default=180)
    p.add_argument("--dry-run",action="store_true")
    a=p.parse_args()
    if not 10<=a.timeout<=600:p.error("timeout must be 10..600")
    raise SystemExit(run(a))
if __name__=="__main__":main()

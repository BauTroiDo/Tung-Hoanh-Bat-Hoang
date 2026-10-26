"""CN02-BULK-04: bounded three-domain IL2CPP pseudocode extraction.

Only per-type filters: il2cpp-dumper-rs uses OR between assembly and type.
Outputs stay outside Git, original binary SHA fenced, one domain at a time.
"""
import argparse,json
from pathlib import Path
from types import SimpleNamespace
from run_body_pilot import run, TOOLS, KNOWN

OUT_BASE=(TOOLS/"_outputs/cn02-bulk/extended").resolve()
DOMAINS=("NetworkProtocol","SceneAddressables","GameplayState")
def plan(manifest):
    data=json.loads(Path(manifest).read_text(encoding="utf8"))
    if data.get("schema")!="CN02-BULK-04-v1" or data.get("binarySHA256")!=KNOWN["binary"]:
        raise ValueError("Untrusted extended manifest")
    result=[]
    for domain in DOMAINS:
        records=data["domains"][domain]
        if not 2<=len(records)<=4:raise ValueError("Domain type count not bounded")
        if sum(x["methodBindings"] for x in records)>300:
            raise ValueError("Excess method bindings for "+domain)
        selectors=[x["dumperTypeSelector"] for x in records]
        if len(set(selectors))!=len(selectors) or any(not x for x in selectors):
            raise ValueError("Invalid type-only filter")
        if not all(x["boundedEntrypoints"] for x in records):
            raise ValueError("Unverified native method map")
        result.append((domain,selectors))
    return result

def execute(manifest,output,dry_run=False,max_body=8192,timeout=180):
    out=Path(output).resolve()
    if out==OUT_BASE or not out.is_relative_to(OUT_BASE):
        raise ValueError("Output must be child of Tool/_outputs/cn02-bulk/extended")
    if out.exists():raise FileExistsError("Refuse to overwrite extended batch output")
    steps=plan(manifest)
    if dry_run:
        print("EXTENDED_DRY_RUN",json.dumps(steps))
        return steps
    out.mkdir(parents=True)
    outcomes=[]
    for domain,selectors in steps:
        print("EXTENDED_RUN",domain,selectors,flush=True)
        code=run(SimpleNamespace(output=out/domain,type=selectors,max_body_size=max_body,
                                 timeout=timeout,dry_run=False))
        outcomes.append({"domain":domain,"exitCode":code,
                         "output":str(out/domain/"run.json")})
        (out/"run-results.json").write_text(json.dumps(outcomes,indent=2),encoding="utf8")
        if code:raise SystemExit(code)
    print("EXTENDED_ALL_PASS",len(outcomes))
    return outcomes
def main():
    p=argparse.ArgumentParser()
    p.add_argument("--manifest",required=True,type=Path)
    p.add_argument("--output",required=True,type=Path)
    p.add_argument("--timeout",type=int,default=180)
    p.add_argument("--max-body",type=int,default=8192)
    p.add_argument("--dry-run",action="store_true")
    a=p.parse_args()
    execute(a.manifest,a.output,a.dry_run,a.max_body,a.timeout)
if __name__=="__main__":main()

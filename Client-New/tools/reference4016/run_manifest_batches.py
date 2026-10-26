"""Execute approved CN02 gameplay types with bounded dumper and no Git writes."""
import argparse
import json
from pathlib import Path
from types import SimpleNamespace
from run_body_pilot import run, TOOLS, KNOWN
OUTPUT_BASE=(TOOLS/"_outputs/cn02-bulk/gameplay-batches").resolve()
EXPECTED_SHA=KNOWN["binary"]

GROUP_ORDER=("Movement","NPC","Combat","Navigation")
def planned(manifest, groups=GROUP_ORDER):
    raw=json.loads(Path(manifest).read_text(encoding="utf8"))
    if raw.get("schema")!="CN02-BULK-03-v1" or raw.get("originalSha256")!=EXPECTED_SHA:
        raise ValueError("Unexpected manifest schema or binary hash")
    result=[]
    for group in groups:
        if group not in GROUP_ORDER:raise ValueError("Unrecognized group "+group)
        entries=raw["recommended"].get(group,[])
        types=[entry["type"] for entry in entries]
        if not 1<=len(types)<=4:raise ValueError("Group has no bounded recommended type list: "+group)
        if len(set(types))!=len(types):raise ValueError("Duplicate selected types")
        if any(entry["boundedNativeEntrypoints"]<2 for entry in entries):
            raise ValueError("Unbounded group "+group)
        result.append((group,types))
    return result

def main():
    p=argparse.ArgumentParser()
    p.add_argument("--manifest",type=Path,required=True)
    p.add_argument("--output-root",type=Path,required=True)
    p.add_argument("--group",action="append",choices=GROUP_ORDER)
    p.add_argument("--max-body-size",type=int,default=8192)
    p.add_argument("--timeout",type=int,default=180)
    p.add_argument("--dry-run",action="store_true")
    a=p.parse_args()
    out=a.output_root.resolve()
    if out==OUTPUT_BASE or not out.is_relative_to(OUTPUT_BASE):
        raise ValueError("Output root outside authorized Tool/_outputs/gameplay-batches")
    if out.exists():raise FileExistsError("Refuse overwrite group runs "+str(out))
    batches=planned(a.manifest,tuple(a.group) if a.group else GROUP_ORDER)
    if a.dry_run:
        print("DRY_RUN",json.dumps(batches))
        return
    out.mkdir(parents=True)
    records=[]
    for group,types in batches:
        target=out/group
        print("RUN",group,",".join(types),flush=True)
        code=run(SimpleNamespace(output=target,type=types,
             max_body_size=a.max_body_size,timeout=a.timeout,dry_run=False))
        records.append({"group":group,"types":types,"exitCode":code,
                        "resultPath":str(target/"run.json")})
        (out/"group-runs.json").write_text(json.dumps(records,indent=2),encoding="utf8")
        if code:
            raise SystemExit(code)
    print("ALL_GROUPS_PASS",len(records))
if __name__=="__main__":main()

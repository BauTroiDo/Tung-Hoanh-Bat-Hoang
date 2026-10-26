"""CN02-BULK-04: three semantic domains from verified IL2CPP method index and .pdata.

RVA/unwind bounds are evidence, not a statement about recovered semantics.
Raw manifests belong in Tool/_outputs, never tracked in source control.
"""
import argparse
import json
import sqlite3
from pathlib import Path
from build_gameplay_manifest import (
    ROOT, GAME, DEFAULT_DB, DLL, EXPECTED_SHA,
    sha, native_unwind_ranges
)

OUT_BASE=(GAME/"Tool/_outputs/cn02-bulk/extended").resolve()
DOMAINS={
  "NetworkProtocol":[
    ("Assembly-CSharp","BishopNetwork","BishopNetwork"),
    ("Assembly-CSharp","GameServerNetwork","GameServerNetwork"),
    ("Assembly-CSharp","Network/Header/ProcessGameUnity","Network.Header.ProcessGameUnity"),
  ],
  "SceneAddressables":[
    ("Assembly-CSharp","MapDataService","MapDataService"),
    ("Assembly-CSharp","VietKiem/Levels/Application/SceneLoadService","VietKiem.Levels.Application.SceneLoadService"),
    ("Assembly-CSharp","VietKiem/Levels/Application/SceneLoader","VietKiem.Levels.Application.SceneLoader"),
  ],
  "GameplayState":[
    ("Assembly-CSharp","WalkState","WalkState"),
    ("Assembly-CSharp","AttackState","AttackState"),
    ("Assembly-CSharp","StateMachine","StateMachine"),
    ("Assembly-CSharp","DeathState","DeathState"),
  ],
}
def produce(db,binary,out,summary,maxspan=8192):
    out=Path(out).resolve()
    if out==OUT_BASE or not out.is_relative_to(OUT_BASE):
        raise ValueError("Raw manifest outside controlled analysis folder")
    if out.exists():raise FileExistsError("Refuse to overwrite baseline")
    if not 256<=maxspan<=8192:raise ValueError("maxspan must be 256..8192")
    if sha(binary)!=EXPECTED_SHA:raise ValueError("GameAssembly binary checksum mismatch")
    base,spans=native_unwind_ranges(binary)
    raw={"schema":"CN02-BULK-04-v1","binarySHA256":EXPECTED_SHA,
         "sqliteSHA256":sha(db),"imageBase":hex(base),
         "maxSpan":maxspan,"pdataEntries":len(spans),"domains":{}}
    with sqlite3.connect("file:"+Path(db).resolve().as_posix()+"?mode=ro",uri=True) as conn:
      for group,types in DOMAINS.items():
        items=[]
        for assembly,typename,selector in types:
            rows=[int(r[0]) for r in conn.execute(
                "SELECT rva FROM methods WHERE assembly=? AND type_name=? AND rva>0",
                (assembly,typename))]
            if not rows:raise ValueError("No method data for "+typename)
            rv=sorted(set(rows))
            bounded=[{"rva":r,"bytes":spans[r]-r} for r in rv
                     if r in spans and 0<spans[r]-r<=maxspan]
            items.append({"assembly":assembly,"typeName":typename,
                          "dumperTypeSelector":selector,
                          "methodBindings":len(rows),"distinctRvas":len(rv),
                          "boundedEntrypoints":bounded,
                          "nonBoundedOrUnmapped":len(rv)-len(bounded)})
        raw["domains"][group]=items
    out.parent.mkdir(parents=True,exist_ok=True)
    out.write_text(json.dumps(raw,ensure_ascii=False,indent=2),encoding="utf8")
    aggregate={}
    for group,types in raw["domains"].items():
        aggregate[group]={
            "types":[x["typeName"] for x in types],
            "dumperSelectors":[x["dumperTypeSelector"] for x in types],
            "methodBindings":sum(x["methodBindings"] for x in types),
            "distinctRvasByTypeSum":sum(x["distinctRvas"] for x in types),
            "boundedEntrypoints":sum(len(x["boundedEntrypoints"]) for x in types),
            "excludedNativeRvasByTypeSum":sum(x["nonBoundedOrUnmapped"] for x in types),
        }
    brief={
        "stage":"CN02-BULK-04",
        "originalBinarySHA256":EXPECTED_SHA,"manifestSHA256":sha(out),
        "manifestPath":str(out),"pdataRegions":len(spans),
        "groupSummary":aggregate,
        "caveat":"Raw C-like pseudo is not proof of correct runtime logic; .pdata is a single unwind region and may omit continuation frames. No code/body snippets committed."
    }
    summary=Path(summary)
    summary.parent.mkdir(parents=True,exist_ok=True)
    summary.write_text(json.dumps(brief,ensure_ascii=False,indent=2),encoding="utf8")
    return brief

def main():
    p=argparse.ArgumentParser()
    p.add_argument("--database",type=Path,default=DEFAULT_DB)
    p.add_argument("--binary",type=Path,default=DLL)
    p.add_argument("--output",type=Path,required=True)
    p.add_argument("--summary",type=Path,required=True)
    p.add_argument("--max-span",type=int,default=8192)
    a=p.parse_args()
    m=produce(a.database,a.binary,a.output,a.summary,a.max_span)
    for g,record in m["groupSummary"].items():
        print(g,len(record["types"]),"types",record["methodBindings"],
              "bindings",record["boundedEntrypoints"],"bounded .pdata")
if __name__=="__main__":main()

"""List symbol-name hints from locally owned IL2CPP metadata, read-only.

This is NOT IL decompilation or a recovered coordinate mapping. Export only bounded
identifier-name suggestions; not object code, string tables, tokens, or credentials.
"""
from pathlib import Path
import argparse
import hashlib
import json
import re
from datetime import datetime, timezone

PATTERN=re.compile(rb"[A-Za-z_][A-Za-z_0-9.]{4,96}")
TERMS=("worldpos", "worldtopos", "worldto", "topos", "worldmap", "subworld", "mapcoord", "mappos", "map2", "scenepos", "scenecoordinate", "regionpos", "mps", "mapid", "convertcoord", "coordinate", "sceneindex", "mapscene", "pos2", "world2", "maptransform")
def main():
    ap=argparse.ArgumentParser()
    ap.add_argument("--metadata",type=Path,required=True)
    ap.add_argument("--out",type=Path,required=True)
    a=ap.parse_args()
    raw=a.metadata.read_bytes()
    filtered={}
    for m in PATTERN.finditer(raw):
        word=m.group().decode("ascii")
        low=word.lower()
        if any(term in low for term in TERMS):
            filtered.setdefault(low, word)
    matches=sorted(filtered.values(),key=lambda x:x.lower())
    result={"sourceName":a.metadata.name,"sourceSha256":hashlib.sha256(raw).hexdigest().upper(),
            "sourceBytes":len(raw),"source":"original installed IL2CPP global-metadata.dat",
            "analysisUtc":datetime.now(timezone.utc).isoformat(),
            "keywords":TERMS,"distinctMatchedIdentifiers":len(matches),"matches":matches[:300],
            "limitations":"Text-only identifier hints; cannot infer call semantics, correct map IDs, coordinate transforms, or active runtime version. More static code analysis would be required."}
    a.out.parent.mkdir(parents=True,exist_ok=True)
    a.out.write_text(json.dumps(result,ensure_ascii=False,indent=2),encoding="utf8")
    print("DISTINCT_MATCHES",len(matches),"SAVED",len(result["matches"]))
    print("HINTS",json.dumps(matches[:180],ensure_ascii=False))
if __name__=="__main__":main()

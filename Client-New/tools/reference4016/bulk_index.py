"""Index a read-only IL2CPP dumper script.json into searchable SQLite.

Input script.json may contain more method *instances* than the IL2CPP
MethodDefinition count (inflated by generic instantiations). Do not conflate.
No proprietary signatures or full code are written into the summary JSON.
Output database belongs OUTSIDE the public source repository.
"""
import argparse
import collections
import hashlib
import json
import sqlite3
import tempfile
from pathlib import Path

SCHEMA = """
CREATE TABLE methods (
  id INTEGER PRIMARY KEY,
  rva INTEGER NOT NULL,
  assembly TEXT NOT NULL,
  type_name TEXT NOT NULL,
  dotnet_signature TEXT NOT NULL,
  native_symbol TEXT NOT NULL,
  c_signature TEXT NOT NULL,
  type_signature TEXT NOT NULL
);
CREATE INDEX idx_methods_rva ON methods(rva);
CREATE INDEX idx_methods_assembly_type ON methods(assembly, type_name);
CREATE INDEX idx_methods_dotnet ON methods(dotnet_signature);
"""
def digest(path):
    sha=hashlib.sha256()
    with path.open("rb") as handle:
        for part in iter(lambda: handle.read(8*1024*1024), b""):
            sha.update(part)
    return sha.hexdigest().upper()

def assemble_index(source, target, summary, binary=None, metadata=None):
    source=Path(source);target=Path(target);summary=Path(summary)
    if not source.is_file():
        raise FileNotFoundError(source)
    if target.exists():
        raise FileExistsError("Refusing to overwrite existing index: "+str(target))
    target.parent.mkdir(parents=True, exist_ok=True)
    summary.parent.mkdir(parents=True,exist_ok=True)
    with source.open(encoding="utf-8") as f:
        data=json.load(f)
    methods=data["ScriptMethod"]
    if not isinstance(methods, list) or not methods:
        raise ValueError("No ScriptMethod list")
    count_by_assembly=collections.Counter()
    count_type=set()
    addresses=set()
    missing_rva=0
    rows=[]
    con=sqlite3.connect(target)
    try:
        con.executescript(SCHEMA)
        for i,m in enumerate(methods, 1):
            name=str(m.get("Group",""))
            image,sep,klass=name.partition("/")
            if not sep:image,klass="__UNKNOWN__",name
            address=m.get("Address",0)
            if not isinstance(address,int):
                raise ValueError("Unexpected RVA data in ScriptMethod")
            if address<=0: missing_rva+=1
            else: addresses.add(address)
            count_by_assembly[image]+=1
            count_type.add((image,klass))
            rows.append((i,address,image,klass,str(m.get("DotNetSignature","")),
                         str(m.get("Name","")),str(m.get("Signature","")),
                         str(m.get("TypeSignature",""))))
            if len(rows)>=2000:
                con.executemany("INSERT INTO methods VALUES (?,?,?,?,?,?,?,?)",rows)
                rows.clear()
        if rows:con.executemany("INSERT INTO methods VALUES (?,?,?,?,?,?,?,?)",rows)
        con.commit()
        count=con.execute("SELECT COUNT(*) FROM methods").fetchone()[0]
        if count!=len(methods):raise AssertionError("Index count mismatch")
        image_count=con.execute("SELECT COUNT(DISTINCT assembly) FROM methods").fetchone()[0]
    except BaseException:
        con.close()
        target.unlink(missing_ok=True)
        raise
    con.close()
    result={
        "producer":"il2cpp-dumper-rs",
        "schemaVersion":1,
        "scriptMethodEntries":count,
        "uniqueRva":len(addresses),
        "entriesWithoutPositiveRva":missing_rva,
        "distinctTypeGroups":len(count_type),
        "distinctAssemblyGroups":image_count,
        "countsByAssembly":dict(sorted(count_by_assembly.items())),
        "metadataScriptEntries":{k:len(v) for k,v in data.items()
                                 if isinstance(v,(list,dict))},
        "scriptJsonSha256":digest(source),
        "referenceBinarySha256":digest(Path(binary)) if binary else None,
        "referenceMetadataSha256":digest(Path(metadata)) if metadata else None,
        "sqlitePath":str(target),
        "sqliteSizeBytes":target.stat().st_size,
        "notes":"ScriptMethod includes generated generic instantiations and is NOT identical to MethodDefinition count. Entire DB holds proprietary method signatures: keep it out of public Git. No function bodies were decompiled or validated."
    }
    summary.write_text(json.dumps(result,ensure_ascii=False,indent=2),encoding="utf-8")
    print("INDEX_OK",count,"entries",len(addresses),"distinct positive RVAs",
          len(count_type),"type groups",image_count,"assembly groups")
    print("DB",target,"bytes",result["sqliteSizeBytes"])
    return result

def main():
    p=argparse.ArgumentParser()
    p.add_argument("--source",required=True,type=Path)
    p.add_argument("--database",required=True,type=Path)
    p.add_argument("--summary",required=True,type=Path)
    p.add_argument("--binary",type=Path)
    p.add_argument("--metadata",type=Path)
    a=p.parse_args()
    assemble_index(a.source,a.database,a.summary,a.binary,a.metadata)

if __name__=="__main__":main()

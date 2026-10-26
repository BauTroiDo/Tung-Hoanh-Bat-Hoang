"""List object storage files and file-local IDs. Read-only, no asset exports."""
import collections
from pathlib import Path
import sys
ROOT=Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT/".tmp/reference4016-python-deps"))
import UnityPy
e=UnityPy.load(sys.argv[1])
g=collections.defaultdict(lambda: collections.Counter())
for o in e.objects:
    a=o.assets_file
    name=getattr(a,"name","?")
    g[str(name)][o.type.name]+=1
for name,counts in g.items():
    print("FILE",name,"COUNTS",dict(counts))
for o in e.objects:
    if o.type.name in ("Mesh","GameObject","Transform","MeshFilter"):
        print("SAMPLE",getattr(o.assets_file,"name","?"),o.type.name,o.path_id)
        if o.type.name=="MeshFilter":
            print("MESHREF", o.parse_as_dict().get("m_Mesh"))
        if sum(1 for x in e.objects if x.type.name in ("MeshFilter","Mesh","Transform","GameObject"))>0:
            pass
        break

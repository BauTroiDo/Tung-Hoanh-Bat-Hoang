"""Build a bounded, repeatable CN02 gameplay batch manifest from SQLite and native PE .pdata.

Full method/group/RVA data stays outside Git. The tracked summary contains only
aggregate counts, selected type labels, SHA and outcome notes.
A .pdata entry marks an unwind region, not proof of whole semantic function.
"""
import argparse, collections, hashlib, json, re, sqlite3, struct
from pathlib import Path

ROOT=Path(__file__).resolve().parents[2]
GAME=ROOT.parent
DEFAULT_DB=GAME/"Tool/_outputs/cn02-bulk/method-index.sqlite"
DLL=GAME/"Client-3D-2/update/GameAssembly.dll"
EXPECTED_SHA="011357720B210AA1F392A8B62FF8DE090D3A895230A6E0135BCE707196871069"
OUTPUT_BASE=(GAME/"Tool/_outputs/cn02-bulk/gameplay-batches").resolve()
PRIORITY={
 "Movement": ("JXMovement","PlayerMovement","NpcMovementController","MovementClock","MoveCmdTrace","NpcWalk"),
 "NPC": ("NPCManager","NPCInfoService","NPCSelectionService","NpcSpawnerQueue","NpcClickInteractor","NpcCellIndex"),
 "Combat": ("CombatPerformanceGuard","UiAP_Fight","UIAM_Combat","BattleFieldState","JX_SkillMoveFront"),
 "Navigation": ("AutoFindPathManager","AutoFindPath","NpcFindPath","XaPhuPathfinder"),
}
# Filter deliberately conservative: no Unity/framework/network UI and no nested state
# machines in first batch. A future manifest may expand these groups.
GROUP_KEYWORDS={
 "Movement":re.compile(r"movement|move(cmd|state)?|npcwalk",re.I),
 "NPC":re.compile(r"^npc|npcselection|npcinfo|npcs?pawn",re.I),
 "Combat":re.compile(r"combat|fight|battle|skill|attack",re.I),
 "Navigation":re.compile(r"path|navigation|route|navmesh",re.I),
}
def sha(path):
    h=hashlib.sha256()
    with Path(path).open("rb") as f:
        for piece in iter(lambda:f.read(4*1024*1024),b""): h.update(piece)
    return h.hexdigest().upper()

def native_unwind_ranges(binary):
    b=Path(binary).read_bytes()
    if b[:2]!=b"MZ":raise ValueError("Not MZ")
    pe=struct.unpack_from("<I",b,0x3C)[0]
    if b[pe:pe+4]!=b"PE\x00\x00":raise ValueError("Not PE")
    number=struct.unpack_from("<H",b,pe+6)[0]
    opt_size=struct.unpack_from("<H",b,pe+20)[0]
    opt=pe+24
    if struct.unpack_from("<H",b,opt)[0]!=0x20B:raise ValueError("Not PE64")
    image_base=struct.unpack_from("<Q",b,opt+24)[0]
    pdata_rva,pdata_size=struct.unpack_from("<II",b,opt+112+3*8)
    sections=opt+opt_size
    raw=None
    for i in range(number):
        row=sections+i*40
        virtual_size,va,raw_size,ptr=struct.unpack_from("<IIII",b,row+8)
        if va<=pdata_rva<va+max(virtual_size,raw_size):
            raw=ptr+pdata_rva-va
            break
    if raw is None or pdata_size==0 or raw+pdata_size>len(b):
        raise ValueError("Could not locate .pdata exception directory")
    spans={}
    for begin,end,_unwind in struct.iter_unpack("<III",b[raw:raw+pdata_size-(pdata_size%12)]):
        if begin and end>begin and end-begin<2**24:
            spans.setdefault(begin,end)
    return image_base,spans

def assign_type(type_name):
    basename=type_name.rsplit("/",1)[-1]
    if "/" in type_name or basename.startswith("<") or basename.startswith("Ui"):
        return None
    # Priority fixed and mutually exclusive; Navigation before Movement ("MovePath").
    if GROUP_KEYWORDS["Navigation"].search(basename):return "Navigation"
    if GROUP_KEYWORDS["Combat"].search(basename):return "Combat"
    if GROUP_KEYWORDS["NPC"].search(basename):return "NPC"
    if GROUP_KEYWORDS["Movement"].search(basename):return "Movement"
    return None

def build(db,binary,output,summary,max_span=8192,max_methods=100):
    output=Path(output).resolve()
    if output==OUTPUT_BASE or not output.is_relative_to(OUTPUT_BASE):
        raise ValueError("Manifest must remain outside Git below Tool/_outputs/cn02-bulk/gameplay-batches")
    if output.exists():raise FileExistsError("Refuse overwrite "+str(output))
    if sha(binary)!=EXPECTED_SHA:raise ValueError("Original GameAssembly SHA changed")
    if not 256<=max_span<=16384:raise ValueError("max_span out of bounds")
    if not 1<=max_methods<=300:raise ValueError("max_methods out of bounds")
    base,pdata=native_unwind_ranges(binary)
    rows=collections.defaultdict(list)
    with sqlite3.connect("file:"+Path(db).resolve().as_posix()+"?mode=ro",uri=True) as c:
        for typ,rva in c.execute("SELECT type_name,rva FROM methods WHERE assembly=? AND rva>0 ORDER BY type_name,rva",("Assembly-CSharp",)):
            group=assign_type(typ)
            if group:rows[(group,typ)].append(int(rva))
    available={}
    for (group,typ),rvlist in rows.items():
        distinct=sorted(set(rvlist))
        exact={r:pdata[r]-r for r in distinct if r in pdata}
        safe={r:n for r,n in exact.items() if n<=max_span}
        available.setdefault(group,[]).append({
           "type":typ,"scriptMethodBindings":len(rvlist),"distinctRvas":len(distinct),
           "nativeUnwindEntrypoints":len(exact),"boundedNativeEntrypoints":len(safe),
           "unwindByteSum":sum(exact.values()),
           "boundedUnwindByteSum":sum(safe.values()),
           "exactEntrypoints":[{"rva":r,"bytes":n} for r,n in sorted(safe.items())],
        })
    selected={}
    for group in PRIORITY:
        types=available.get(group,[])
        ranks={t:i for i,t in enumerate(PRIORITY[group])}
        # Prefer named gameplay core classes, then exact entry count. No UI bulk.
        types.sort(key=lambda x:(ranks.get(x["type"],999),
                  -x["boundedNativeEntrypoints"],x["type"]))
        candidates=[]
        for t in types:
            if t["boundedNativeEntrypoints"]<2:continue
            if t["nativeUnwindEntrypoints"]>max_methods:continue
            candidates.append(t)
        selected[group]=candidates[:2]
    manifest={
        "schema":"CN02-BULK-03-v1","originalSha256":EXPECTED_SHA,
        "sqliteSha256":sha(db),"imageBase":hex(base),
        "maxSingleUnwindSpanBytes":max_span,
        "maxNativeEntrypointsPerType":max_methods,
        "pdataRuntimeFunctionEntrypoints":len(pdata),
        "groups":available,"recommended":selected,
        "note":"Native .pdata spans, not verified whole function semantics. Only exact unwind starts are included. Adjacent continuations are NOT auto-merged. Method bindings != unique MethodDefs; generic/alias RVAs possible."
    }
    output.parent.mkdir(parents=True,exist_ok=True)
    output.write_text(json.dumps(manifest,indent=2),encoding="utf8")
    selected_groups={}
    for group,types in selected.items():
        selected_groups[group]={
            "selectedTypes":[t["type"] for t in types],
            "selectedBindings":sum(t["scriptMethodBindings"] for t in types),
            "selectedBoundedEntrypoints":sum(t["boundedNativeEntrypoints"] for t in types),
        }
    brief={
        "stage":"CN02-BULK-03","schema":"CN02-BULK-03-v1",
        "originalBinarySha256":EXPECTED_SHA,"nativeImageBase":hex(base),
        "pdataEntrypointCount":len(pdata),
        "groups":{g:{"candidateTypeCount":len(available.get(g,[])),
                    "candidateNativeEntrypoints":sum(t["nativeUnwindEntrypoints"] for t in available.get(g,[]))}
                 for g in PRIORITY},
        "recommendedGroups":selected_groups,
        "rawManifestPath":str(output),
        "rawManifestSha256":sha(output),
        "warning":"Exact unwind regions are not necessarily complete functions. This is a prioritized type/RVA batch manifest, not semantic decompilation or Map53 runtime acceptance."
    }
    summary=Path(summary)
    summary.parent.mkdir(parents=True,exist_ok=True)
    summary.write_text(json.dumps(brief,indent=2),encoding="utf8")
    return brief
def main():
    a=argparse.ArgumentParser()
    a.add_argument("--database",type=Path,default=DEFAULT_DB)
    a.add_argument("--binary",type=Path,default=DLL)
    a.add_argument("--output",type=Path,required=True)
    a.add_argument("--summary",type=Path,required=True)
    a.add_argument("--max-span",type=int,default=8192)
    a.add_argument("--max-methods",type=int,default=100)
    v=a.parse_args()
    result=build(v.database,v.binary,v.output,v.summary,v.max_span,v.max_methods)
    print("MANIFEST_OK",result["pdataEntrypointCount"],"native regions")
    for g,t in result["recommendedGroups"].items():
        print(g,",".join(t["selectedTypes"]),t["selectedBoundedEntrypoints"],"bounded native entries")
if __name__=="__main__":main()

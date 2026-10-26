"""Recover v39 metadata method parameters and candidate world-sync types.

All inputs are read-only. Field/type *names* are direct metadata facts.
World-sync record layout is only a native code observation; candidate
type names cannot be assumed to match until the type-index is resolved.
"""
from pathlib import Path
import argparse,hashlib,json,struct,re

TARGET_METHODS=[("ProcessGameUnity","OnSyncWorld"),("MapService","UpdateWorldInfo"),
                ("SubWorld","LoadMap"),("SubWorld","Activate"),("MapService","Calibrate")]
CANDIDATE=re.compile(r"(?i)(sync.*world|world.*sync|worldinfo|world.*change|change.*world|scene.*info|map.*info|new.*world|world.*data|world.*packet|subworld)")
def main():
    p=argparse.ArgumentParser()
    p.add_argument("--metadata",type=Path,required=True)
    p.add_argument("--worldsync-evidence",type=Path,required=True)
    p.add_argument("--map53-log",type=Path,required=True)
    p.add_argument("--output",type=Path,required=True)
    args=p.parse_args()
    b=args.metadata.read_bytes()
    if struct.unpack_from("<III",b,0)!=(0xFAB11BAF,39,380):raise ValueError("Not IL2CPP v39")
    sections=[struct.unpack_from("<III",b,8+12*i) for i in range(31)]
    so,sl,_=sections[2]
    po,ps,pc=sections[10]; to,ts,tc=sections[19]; mo,ms,mc=sections[5]
    fo,fs,fc=sections[11]
    if ps!=pc*12 or ts!=tc*82 or ms!=mc*32 or fs!=fc*12:
        raise ValueError("Metadata record strides")
    def st(idx):
        if not 0<=idx<sl:return None
        pos=so+idx;end=b.find(bytes([0]),pos,so+sl)
        if end<pos or end-pos>200:return None
        return b[pos:end].decode("utf8","replace")
    types=[]
    for i in range(tc):
        offset=to+i*82
        nm=st(struct.unpack_from("<I",b,offset)[0])
        namespace=st(struct.unpack_from("<I",b,offset+4)[0])
        field_start=struct.unpack_from("<I",b,offset+26)[0]
        type_byval=struct.unpack_from("<I",b,offset+8)[0]
        types.append({"index":i,"name":nm,"namespace":namespace,"fieldStart":field_start,
                      "byvalTypeRegistryIndex":type_byval})
    candidates=[]
    for t in types[:-1]:
        if not t["name"] or not CANDIDATE.search(t["name"]):continue
        first=t["fieldStart"];last=types[t["index"]+1]["fieldStart"]
        if not 0<=first<=last<=fc:continue
        fields=[]
        for idx in range(first,min(last,first+48)):
            off=fo+idx*12
            field_nm=st(struct.unpack_from("<I",b,off)[0])
            fields.append({"fieldIndex":idx,"name":field_nm,
                           "typeRegistryIndex":struct.unpack_from("<I",b,off+4)[0],
                           "token":hex(struct.unpack_from("<I",b,off+8)[0])})
        candidates.append({"typeDefinitionIndex":t["index"],
                           "byvalTypeRegistryIndex":t["byvalTypeRegistryIndex"],
                           "name":t["name"],"namespace":t["namespace"],
                           "totalFieldCount":last-first,"fields":fields})
    methods=[]
    requested=set(TARGET_METHODS)
    for i in range(mc):
        off=mo+i*32
        owner=struct.unpack_from("<H",b,off+4)[0]
        nm=st(struct.unpack_from("<I",b,off)[0])
        if (types[owner]["name"],nm) not in requested:continue
        start=struct.unpack_from("<H",b,off+14)[0]
        # v39 stores the parameter start in high 16 bits of word +12,
        # and the parameter count in the last 16 bits of method record.
        count=struct.unpack_from("<H",b,off+30)[0]
        if start+count>pc:raise ValueError("param range overflow")
        parameters=[]
        for idx in range(start,start+count):
            poff=po+idx*12
            pname=st(struct.unpack_from("<I",b,poff)[0])
            parameters.append({"parameterIndex":idx,"name":pname,
                               "typeIndex":struct.unpack_from("<I",b,poff+8)[0],
                               "token":hex(struct.unpack_from("<I",b,poff+4)[0])})
        methods.append({"methodIndex":i,"owner":types[owner]["name"],"name":nm,
                        "token":hex(struct.unpack_from("<I",b,off+20)[0]),
                        "parameterStart":start,"parameters":parameters})
    ev=json.loads(args.worldsync_evidence.read_text(encoding="utf8"))
    if not all(name in ev["results"] for name in ("ProcessGameUnity.OnSyncWorld","MapService.UpdateWorldInfo","SubWorld.LoadMap","SubWorld.Activate")):
        raise ValueError("Original native chain missing")
    # TypeDefinition +8 is the packed v39 byval registry index. Match
    # independently against the param registry index shared by BOTH methods.
    world_types=[x for x in candidates if x["name"]=="WORLD_SYNC"
                 and x["namespace"]=="Network.Header"]
    if len(world_types)!=1:raise ValueError("WORLD_SYNC type not unique")
    world=world_types[0]
    sig={(x["owner"],x["name"]):x for x in methods}
    src=sig[("ProcessGameUnity","OnSyncWorld")]["parameters"]
    dst=sig[("MapService","UpdateWorldInfo")]["parameters"]
    if len(src)!=1 or len(dst)!=1 or src[0]["name"]!="data" or dst[0]["name"]!="data":
        raise ValueError("World-sync signatures changed")
    if src[0]["typeIndex"]!=world["byvalTypeRegistryIndex"] or dst[0]["typeIndex"]!=world["byvalTypeRegistryIndex"]:
        raise ValueError("WORLD_SYNC parameter type does not match type definition")
    source_names=[x["name"] for x in world["fields"]]
    if source_names!=["ProtocolType","SubWorld","Region","Weather","Frame","MapCopyID","szName"]:
        raise ValueError("Unexpected original WORLD_SYNC fields")
    loader=sig[("SubWorld","LoadMap")]["parameters"]
    if [x["name"] for x in loader] != ["nTemplateId","nMapId","nRegion","nMpsX","nMpsY"]:
        raise ValueError("Original LoadMap method signature changed")
    ctx=ev["results"]["MapService.UpdateWorldInfo"]["subworldCallsiteContexts"]
    if len(ctx)!=2:raise ValueError("Missing native loader/activate callsite contexts")
    load=next(x for x in ctx if x["targetRVA"]=="0xc75de0")["instructions"]
    mnemonics=[(x["op"],x["args"]) for x in load]
    patterns=[("movups","xmm0, xmmword ptr [rbx]"),
              ("movups","xmm2, xmmword ptr [rbx + 0x10]"),
              ("movq","r8, xmm0"),("shr","r8, 0x20"),
              ("psrldq","xmm1, 8"),("movd","r9d, xmm1"),
              ("movq","rdx, xmm2"),("shr","rdx, 0x20"),
              ("mov","dword ptr [rsp + 0x28], esi"),
              ("mov","dword ptr [rsp + 0x20], esi")]
    if not all(x in mnemonics for x in patterns):
        raise ValueError("Original native field extraction patterns changed")
    # Field order + field primitive metadata and native offset observations
    # provide evidence for three DWORD aliases. The precise layout of
    # every other member must not be inferred from these three offsets.
    field_by_name={x["name"]:x for x in world["fields"]}
    for n in ("SubWorld","Region","MapCopyID"):
        if field_by_name[n]["typeRegistryIndex"]!=field_by_name["SubWorld"]["typeRegistryIndex"]:
            raise ValueError("World-sync 32-bit source field registry indices differ")
    assignments=[
        {"callArgument":"nTemplateId","sourceField":"MapCopyID","sourceOffset":20,"nativeRegister":"edx",
         "fieldTypeRegistryIndex":field_by_name["MapCopyID"]["typeRegistryIndex"]},
        {"callArgument":"nMapId","sourceField":"SubWorld","sourceOffset":4,"nativeRegister":"r8d",
         "fieldTypeRegistryIndex":field_by_name["SubWorld"]["typeRegistryIndex"]},
        {"callArgument":"nRegion","sourceField":"Region","sourceOffset":8,"nativeRegister":"r9d",
         "fieldTypeRegistryIndex":field_by_name["Region"]["typeRegistryIndex"]},
        {"callArgument":"nMpsX","sourceField":None,"sourceOffset":None,"nativeRegister":"stack +0x20",
         "sourceValue":"zeroed-esi"},
        {"callArgument":"nMpsY","sourceField":None,"sourceOffset":None,"nativeRegister":"stack +0x28",
         "sourceValue":"zeroed-esi"}
    ]
    if ("xor","esi, esi") not in [(x["op"],x["args"]) for x in ev["results"]["MapService.UpdateWorldInfo"]["entryInstructions"]]:
        raise ValueError("Zero-valued extra arguments not grounded")
    # The historical M1 file is a test-harness event trace, NOT a proof that
    # the real player entered/rendered Map53 or that coefficients were sampled.
    log_bytes=args.map53_log.read_bytes()
    log=json.loads(log_bytes.decode("utf8"))
    if log.get("hashes",{}).get("GameAssembly.dll","").upper()!=ev["binarySHA256"]:
        raise ValueError("Map53 historical log source fingerprint mismatch")
    events=[e["payload"] for e in log.get("events",[]) if e.get("type")=="send"]
    world_events=[e for e in events if e.get("kind")=="receivedWorldMap"]
    sync_values=[(e["subWorld"],e["mapCopyId"],e["region"]) for e in world_events]
    if not sync_values or len(set(sync_values))!=1:
        raise ValueError("Map53 original harness records are inconsistent")
    subworld,copy_id,region=sync_values[0]
    if (subworld,copy_id,region)!=(53,53,6488114):
        raise ValueError("Unexpected historical Map53 harness values")
    harness={"sourceFile":args.map53_log.name,
             "sourceSHA256":hashlib.sha256(log_bytes).hexdigest().upper(),
             "kind":"historical-local-test-harness-events",
             "distinctWorldSyncEvents":len(set(sync_values)),
             "totalWorldSyncEvents":len(sync_values),
             "observedFields":{"SubWorld":subworld,"MapCopyID":copy_id,"Region":region},
             "derivedLoaderArguments":{"nTemplateId":copy_id,"nMapId":subworld,"nRegion":region,
                                       "nMpsX":0,"nMpsY":0},
             "gate":"NOT_RUNTIME_ACCEPTANCE: historical emitted events only; no original 4016 rendered scene + mapped landmark pairing."}
    result={"sourceSHA256":hashlib.sha256(b).hexdigest().upper(),
            "methodDefinitions":methods,"candidateWorldTypeDefinitions":candidates,
            "resolvedWorldSyncType":world,
            "resolvedTypeParameterIndex":world["byvalTypeRegistryIndex"],
            "nativeLoadMapBindings":assignments,
            "historicalMap53HarnessObservation":harness,
            "recordSource":"Original IL2CPP v39 MethodDefinition, ParamDefinition, TypeDefinition, FieldDefinition, and SHA-checked 4016 native callsite",
            "limitations":"Original method and field names recovered with byval registry-index equality. The three named DWORD source offsets are supported by compiled native transfers; other field offsets, full wire framing, live Map53 coefficients and landmark pairing remain unverified."}
    args.output.parent.mkdir(parents=True,exist_ok=True)
    args.output.write_text(json.dumps(result,ensure_ascii=False,indent=2),encoding="utf8")
    print("TARGET_METHODS",len(methods))
    for x in methods:print("METHOD",x["owner"],x["name"],x["token"],x["parameters"])
    print("WORLD_TYPE_CANDIDATES",len(candidates))
    for x in candidates:
        if ("world" in x["name"].lower() or "map" in x["name"].lower()) and x["totalFieldCount"]<=40:
            print("TYPE",x["typeDefinitionIndex"],x["name"],x["namespace"],"FIELDCOUNT",x["totalFieldCount"],"FIELDNAMES",[f["name"] for f in x["fields"]][:35])
if __name__=="__main__":main()

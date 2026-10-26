"""Bounded static scene-key and map-pack resolver trace for original 4016.

Never runs Game.exe/GameAssembly.dll. Distinguishes observed native calls
from claimed runtime asset availability or successful scene loads.
"""
import argparse,hashlib,json,struct,sys
from pathlib import Path
ROOT=Path(__file__).resolve().parents[3]
sys.path.insert(0,str(ROOT/".tmp/reference4016-python-deps"))
from map_writer_xrefs import index_metadata
import pefile
from capstone import Cs,CS_ARCH_X86,CS_MODE_64,CS_GRP_CALL
from capstone.x86_const import X86_OP_IMM
FOCUS={
 "MapDataService.LoadForTemplate":0x8C9BF0,
 "MapDataService.ResolveMapFolderPath":0x8CD240,
 "MapDataService.ParseMaplistFile":0x8CC0F0,
 "MapDataService.TryMaplistPath":0x8CD820,
 "GameManager.LoadScene":0x918B70,
 "GameManager.LoadSceneByName":0x918B20,
 "<LoadScene>d__96.MoveNext":0xA6F160,
 "SceneLoadService.LoadSceneAsync":0xC73EF0,
 "SceneLoadService.LoadSceneInternal":0xC74170,
 "SceneLoadService.ResolveVariantKeyAsync":0xC74650,
 "SceneLoadService.LoadAddressableMapSceneAsync":0xC73C00,
 "<LoadSceneAsync>d__24.MoveNext":0xC8D5A0,
 "<LoadSceneInternal>d__26.MoveNext":0xC8E410,
 "SceneLoader.HandleLoadSceneRequest":0xC800B0,
}
def inspect(binary,metadata,module,loadmap):
    pb=binary.read_bytes();mb=metadata.read_bytes()
    binary_hash=hashlib.sha256(pb).hexdigest().upper()
    meta_hash=hashlib.sha256(mb).hexdigest().upper()
    if binary_hash!=module["sha256"] or binary_hash!=loadmap["binarySHA256"]:
        raise ValueError("Original binary identity mismatch")
    if meta_hash!=loadmap["metadataSHA256"]:raise ValueError("Metadata identity mismatch")
    pe=pefile.PE(data=pb,fast_load=True)
    base=pe.OPTIONAL_HEADER.ImageBase
    pdata=pe.OPTIONAL_HEADER.DATA_DIRECTORY[3]
    frames={start:end for start,end,u in struct.iter_unpack("<III",
            pe.get_data(pdata.VirtualAddress,pdata.Size))}
    codegen=module["candidates"][0]
    n=codegen["methodPointerCount"]
    buffer=pe.get_data(int(codegen["methodArrayRVA"],16),n*8)
    if len(buffer)!=n*8:raise ValueError("Codegen method pointer table truncated")
    ownership=index_metadata(mb)
    methods={}
    for idx,(va,) in enumerate(struct.iter_unpack("<Q",buffer),1):
        if va>=base and idx in ownership:
            m=ownership[idx]
            methods.setdefault(va-base,[]).append(m["type"]+"."+m["method"])
    for key,rva in FOCUS.items():
        if key not in methods.get(rva,[]):raise ValueError("Ownership mismatch: "+key)
    dis=Cs(CS_ARCH_X86,CS_MODE_64);dis.detail=True
    entries=[]
    for method,rva in FOCUS.items():
        if rva not in frames:raise ValueError("No unwind frame: "+method)
        begin=rva;end=frames[rva]
        if method=="MapDataService.LoadForTemplate":
            if end!=0x8C9DBA or frames.get(end)!=0x8C9F7C:
                raise ValueError("Unverified 2nd map-pack unwind fragment")
            end=frames[end]
        if end<=begin or end-begin>15000:raise ValueError("Unbounded native method")
        ins=list(dis.disasm(pe.get_data(begin,end-begin),base+begin))
        if not ins:raise ValueError("No disassembly: "+method)
        calls=[]
        direct_branches=0
        for j,i in enumerate(ins):
            if not i.group(CS_GRP_CALL):continue
            to=next((x.imm-base for x in i.operands if x.type==X86_OP_IMM),None)
            aliases=methods.get(to,[]) if to is not None else []
            # Shared native thunks have many unrelated MethodDef aliases;
            # they are NOT sound evidence for specific business calls.
            targetnames=aliases if len(aliases)<=3 else []
            calls.append({"rva":hex(i.address-base),"targetRVA":hex(to) if to is not None else None,
                          "registeredNames":targetnames,"ambiguousAliasCount":len(aliases) if len(aliases)>3 else 0,
                          "preparation":[{"op":x.mnemonic,"args":x.op_str}
                              for x in ins[max(0,j-5):j]] if targetnames else []})
        entries.append({"method":method,"rva":hex(rva),"endRVAExclusive":hex(end),
                        "decodedInstructions":len(ins),
                        "directCalls":len(calls),"knownCallees":[x for x in calls if x["registeredNames"]],
                        "allCalls":calls[:350],
                        "entry":[{"rva":hex(x.address-base),"op":x.mnemonic,"args":x.op_str} for x in ins[:15]]})
    return {"binarySHA256":binary_hash,"metadataSHA256":meta_hash,
            "traces":entries,
            "referenceLoadMapNativeRVA":loadmap["nativeRVA"],
            "limits":"Method-pointer-resolved static disassembly only. May miss indirect or compiler-generated calls. Does not prove actual Addressables scene key, bundle manifest entries, render completion, Map53 coefficients or 3+1 real anchors."}

def main():
    p=argparse.ArgumentParser()
    for a in ("binary","metadata","module","loadmap","out"):
        p.add_argument("--"+a,required=True,type=Path)
    a=p.parse_args()
    output=inspect(a.binary,a.metadata,json.loads(a.module.read_text(encoding="utf8")),
                   json.loads(a.loadmap.read_text(encoding="utf8")))
    a.out.parent.mkdir(parents=True,exist_ok=True)
    a.out.write_text(json.dumps(output,ensure_ascii=False,indent=2),encoding="utf8")
    for entry in output["traces"]:
        print("METHOD",entry["method"],entry["rva"],"ins",entry["decodedInstructions"],
              "direct",entry["directCalls"])
        for c in entry["knownCallees"][:35]:
            print("  CALL",c["rva"],"->",c["registeredNames"])
if __name__=="__main__":main()

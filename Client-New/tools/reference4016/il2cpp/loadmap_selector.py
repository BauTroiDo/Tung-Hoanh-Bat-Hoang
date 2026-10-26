"""Read-only original 4016 SubWorld.LoadMap PE64 continuation + selector analysis.

The 12.5KiB second .pdata fragment is essential: inspecting only the first
485-byte unwind entry incorrectly truncates the Map/scene path.
"""
import argparse, hashlib, json, struct,sys
from pathlib import Path
ROOT=Path(__file__).resolve().parents[3]
sys.path.insert(0,str(ROOT/".tmp/reference4016-python-deps"))
import pefile
from capstone import Cs,CS_ARCH_X86,CS_MODE_64,CS_GRP_CALL,CS_GRP_JUMP
from capstone.x86_const import X86_OP_IMM
from map_writer_xrefs import index_metadata
START=0xC75DE0
def analyze(binary,meta,module,worldsync):
    raw=binary.read_bytes();md=meta.read_bytes()
    if hashlib.sha256(raw).hexdigest().upper()!=module["sha256"] or (
         hashlib.sha256(raw).hexdigest().upper()!=worldsync["binarySHA256"]):
         raise ValueError("Original binary SHA mismatch")
    if hashlib.sha256(md).hexdigest().upper()!=worldsync["metadataSHA256"]:
        raise ValueError("Original metadata SHA mismatch")
    pe=pefile.PE(data=raw,fast_load=True)
    base=pe.OPTIONAL_HEADER.ImageBase
    dir=pe.OPTIONAL_HEADER.DATA_DIRECTORY[3]
    fd=pe.get_data(dir.VirtualAddress,dir.Size)
    frames={b:(b,e) for b,e,u in struct.iter_unpack("<III",fd[:len(fd)//12*12]) if b<e}
    if START not in frames or frames[START][1] not in frames:
        raise ValueError("Expected original loadmap unwind split not found")
    first=frames[START];second=frames[first[1]]
    if first!=(0xC75DE0,0xC75FC5) or second!=(0xC75FC5,0xC790A4):
        raise ValueError("Unexpected original LoadMap fragment boundary")
    module_row=module["candidates"][0]
    count=module_row["methodPointerCount"]
    arr=int(module_row["methodArrayRVA"],16)
    ptr_data=pe.get_data(arr,count*8)
    if len(ptr_data)!=count*8:raise ValueError("Codegen pointer table truncated")
    code_names=index_metadata(md)
    names={}
    for i,(va,) in enumerate(struct.iter_unpack("<Q",ptr_data),1):
        if va>=base and i in code_names:
            o=code_names[i]
            names.setdefault(va-base,[]).append(o["type"]+"."+o["method"])
    if "SubWorld.LoadMap" not in names.get(START,[]):
        raise ValueError("Native method ownership mismatch")
    other_method_ptrs=[(rva,ns) for rva,ns in names.items() if START<rva<second[1]]
    if other_method_ptrs:raise ValueError("Another codegen method starts within LoadMap fragment")
    getter_rvas={"SubWorld.get_SubWorldID":0x96AE10,
                 "SubWorld.set_SubWorldID":0xA7DC10,
                 "SubWorld.get_MapTemplateID":0x82CAA0,
                 "SubWorld.set_MapTemplateID":0x7E7630}
    accessors=[]
    single=Cs(CS_ARCH_X86,CS_MODE_64)
    for label,rva in getter_rvas.items():
        if label not in names.get(rva,[]):
            raise ValueError("Missing original SubWorld accessor pointer")
        inst=list(single.disasm(pe.get_data(rva,20),base+rva))
        if len(inst)<2 or inst[1].mnemonic!="ret":
            raise ValueError("Unexpected SubWorld getter/setter body")
        accessors.append({"name":label,"rva":hex(rva),
                          "instruction":inst[0].mnemonic+" "+inst[0].op_str})
    expected={
      "SubWorld.get_SubWorldID":"mov eax, dword ptr [rcx + 0x1c]",
      "SubWorld.set_SubWorldID":"mov dword ptr [rcx + 0x1c], edx",
      "SubWorld.get_MapTemplateID":"mov eax, dword ptr [rcx + 0x20]",
      "SubWorld.set_MapTemplateID":"mov dword ptr [rcx + 0x20], edx"}
    if {x["name"]:x["instruction"] for x in accessors}!=expected:
        raise ValueError("Native SubWorld instance layout differs from accessors")
    # Pull parameter names directly from v39 metadata for the called APIs.
    secs=[struct.unpack_from("<III",md,8+12*n) for n in range(31)]
    so,sl,_=secs[2];mo,ms,mc=secs[5];po,ps,pc=secs[10]
    if ms!=32*mc or ps!=12*pc:raise ValueError("Metadata parameter record bounds")
    def metadata_name(index):
        if index>=sl:raise ValueError("Invalid metadata string reference")
        a=so+index;b=md.find(bytes([0]),a,so+sl)
        return md[a:b].decode("utf8","replace")
    target_sigs=("MapDataService.LoadForTemplate","GameManager.LoadScene",
                 "NPCManager.OnMapTemplateChanged","SubWorld.LoadMap")
    signatures=[]
    for full in target_sigs:
        rvas=[(rva,labels) for rva,labels in names.items() if full in labels]
        if len(rvas)!=1:raise ValueError("Method name is not unique: "+full)
        rva=rvas[0][0]
        idxs=[x["metadataIndex"] for x in code_names.values()
              if x["type"]+"."+x["method"]==full]
        if len(idxs)!=1:raise ValueError("Metadata signature ambiguous")
        mi=idxs[0];pos=mo+32*mi
        first_param=struct.unpack_from("<H",md,pos+14)[0]
        num_params=struct.unpack_from("<H",md,pos+30)[0]
        if first_param+num_params>pc:raise ValueError("Parameter range invalid")
        params=[]
        for i in range(first_param,first_param+num_params):
            off=po+12*i
            params.append({"name":metadata_name(struct.unpack_from("<I",md,off)[0]),
                           "typeRegistryIndex":struct.unpack_from("<I",md,off+8)[0]})
        signatures.append({"method":full,"rva":hex(rva),"params":params})
    decoder=Cs(CS_ARCH_X86,CS_MODE_64);decoder.detail=True
    branches=[]
    mapped_calls=[]
    write_samples=[]
    fragment_info=[]
    entry=[]
    tail=[]
    target_contexts=[]
    interesting={"MapDataService.LoadForTemplate","MapDataService.UnloadPack",
                 "MapDataService.ResetTemplateTracking",
                 "GameManager.LoadScene","NPCManager.OnMapTemplateChanged",
                 "SubWorld.FindRegion","KRegion.Load","KRegion.Init"}
    for b,e in (first,second):
        code=pe.get_data(b,e-b)
        insns=list(decoder.disasm(code,base+b))
        if not insns or insns[0].address !=base+b:
            raise ValueError("Unable to decode fragment start")
        fragment_info.append({"startRVA":hex(b),"endRVAExclusive":hex(e),
                              "bytes":len(code),"instructionCount":len(insns),
                              "firstInsn":insns[0].mnemonic,"lastInsn":insns[-1].mnemonic})
        if b==first[0]:
            entry=[{"rva":hex(x.address-base),"mnemonic":x.mnemonic,"operands":x.op_str}
                   for x in insns[:18]]
        else:
            tail=[{"rva":hex(x.address-base),"mnemonic":x.mnemonic,"operands":x.op_str}
                  for x in insns[:22]]
        for j,ins in enumerate(insns):
            rva=ins.address-base
            if ins.group(CS_GRP_CALL):
                direct=next((op.imm-base for op in ins.operands if op.type==X86_OP_IMM),None)
                dest_names=names.get(direct,[]) if direct is not None else []
                call={"atRVA":hex(rva),"targetRVA":hex(direct) if direct is not None else None,
                      "knownNames":dest_names}
                mapped_calls.append(call)
                if any(x in interesting for x in dest_names):
                    target_contexts.append({
                        "atRVA":hex(rva),"targetRVA":hex(direct),
                        "names":dest_names,"instructions":[
                            {"rva":hex(x.address-base),"mnemonic":x.mnemonic,"operands":x.op_str}
                            for x in insns[max(0,j-16):j+3]]})
            if ins.group(CS_GRP_JUMP):
                target=next((op.imm-base for op in ins.operands if op.type==X86_OP_IMM),None)
                if target is not None and (target<START or target>=second[1]):
                    branches.append({"atRVA":hex(rva),"mnemonic":ins.mnemonic,
                                     "targetRVA":hex(target),"outsideTwoFragments":True})
            if (ins.mnemonic in ("mov","movss","movups","movaps","lea","add","sub","cmp","test")
                and any(f"[rsi + {hex(v)}]" in ins.op_str for v in (0x1c,0x20,0x24,0x28,0x30,0x40,0x50,0x58))):
                write_samples.append({"rva":hex(rva),"mnemonic":ins.mnemonic,"operands":ins.op_str})
    relevant=[x for x in mapped_calls if x["knownNames"]]
    used=dict()
    for call in mapped_calls:
        key=",".join(call["knownNames"]) if call["knownNames"] else f"unmapped:{call['targetRVA']}"
        used[key]=used.get(key,0)+1
    # One-hop bounded trace of the actual scene and data-pack selector API.
    adjacent={}
    for method_rva in (0x8C9BF0,0x918B70):
        if method_rva not in frames:raise ValueError("Missing scene/data selector unwind entry")
        b,e=frames[method_rva]
        if e-b>8192:raise ValueError("Oversized scene selector function")
        instrs=list(decoder.disasm(pe.get_data(b,e-b),base+b))
        call_list=[]
        for j,inst in enumerate(instrs):
            if not inst.group(CS_GRP_CALL):continue
            direct=next((op.imm-base for op in inst.operands if op.type==X86_OP_IMM),None)
            call_list.append({"rva":hex(inst.address-base),
                              "targetRVA":hex(direct) if direct is not None else None,
                              "names":names.get(direct,[]),
                              "preceding":[{"rva":hex(z.address-base),"op":z.mnemonic,"args":z.op_str}
                                           for z in instrs[max(0,j-6):j]] if names.get(direct,[]) else []})
        adjacent[hex(method_rva)]={
            "names":names.get(method_rva,[]),"functionEndRVAExclusive":hex(e),
            "insnCount":len(instrs),
            "calls":call_list,
            "firstInstructions":[{"rva":hex(z.address-base),"op":z.mnemonic,"args":z.op_str}
                                 for z in instrs[:12]]}
    def local_ctx(name):
        matches=[x for x in target_contexts if name in x["names"]]
        if len(matches)!=1:raise ValueError("Selector callsite not unique: "+name)
        return [(z["mnemonic"],z["operands"]) for z in matches[0]["instructions"]]
    # These are tested as machine instructions rather than inferred from
    # C# names (GameManager.LoadScene's arg happens to be called mapId).
    if ("mov","edi, edx") not in [(x["mnemonic"],x["operands"]) for x in entry]:
        raise ValueError("Original template argument not saved to EDI")
    if ("mov","r14d, r8d") not in [(x["mnemonic"],x["operands"]) for x in entry]:
        raise ValueError("Original map argument not saved to R14D")
    if not any(x["rva"]=="0xc7628f" and x["operands"]=="dword ptr [rsi + 0x1c], r14d" for x in write_samples):
        raise ValueError("SubWorldID write not matched")
    if not any(x["rva"]=="0xc76326" and x["operands"]=="dword ptr [rsi + 0x20], edi" for x in write_samples):
        raise ValueError("MapTemplateID write not matched")
    if ("mov","ecx, edi") not in local_ctx("MapDataService.LoadForTemplate"):
        raise ValueError("Pack loader selector not backed by nTemplateId")
    if ("mov","edx, edi") not in local_ctx("GameManager.LoadScene"):
        raise ValueError("Scene loader selector not backed by nTemplateId")
    if ("mov","edx, edi") not in local_ctx("NPCManager.OnMapTemplateChanged"):
        raise ValueError("NPC template selector not backed by nTemplateId")
    result={"binarySHA256":hashlib.sha256(raw).hexdigest().upper(),
            "metadataSHA256":hashlib.sha256(md).hexdigest().upper(),
            "method":"SubWorld.LoadMap","nativeRVA":hex(START),
            "unwindFragments":fragment_info,
            "continuationStart":hex(second[0]),"continuationEndExclusive":hex(second[1]),
            "methodPointerConflicts":other_method_ptrs,
            "entryInstructions":entry,"continuationEntryInstructions":tail,
            "originalSubWorldFieldAccessors":accessors,
            "calledMethodSignatures":signatures,
            "knownMethodCalls":relevant,
            "semanticCallContexts":target_contexts,
            "firstHopSceneAndPackDispatch":adjacent,
            "allDirectCalls":mapped_calls,
            "callFrequency":used,"outOfRangeBranches":branches,
            "subWorldInstanceOffsetSamples":write_samples[:120],
            "limitations":"Two contiguous PE unwind entries can correspond to one function, validated by codegen pointer uniqueness but not runtime. Static disassembly cannot independently prove Map53 visual scene load, resource availability, full field semantics, or live affine parameters."}
    return result
def main():
    p=argparse.ArgumentParser()
    for key in ("binary","metadata","module","worldsync","out"):
        p.add_argument("--"+key,required=True,type=Path)
    a=p.parse_args()
    result=analyze(a.binary,a.metadata,
        json.loads(a.module.read_text(encoding="utf8")),
        json.loads(a.worldsync.read_text(encoding="utf8")))
    a.out.parent.mkdir(parents=True,exist_ok=True)
    a.out.write_text(json.dumps(result,ensure_ascii=False,indent=2),encoding="utf8")
    print("FRAGMENTS",result["unwindFragments"])
    print("KNOWN_CALLS",len(result["knownMethodCalls"]))
    for item in result["knownMethodCalls"][:100]: print("CALL",item)
    print("OUTSIDE_BRANCHES",len(result["outOfRangeBranches"]))
    print("SUBWORLD_OFFSETS",result["subWorldInstanceOffsetSamples"][:25])
if __name__=="__main__":main()

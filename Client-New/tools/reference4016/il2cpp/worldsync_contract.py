"""Read-only bounded IL2CPP world-sync function argument/callflow inventory.

IMPORTANT: This exports only structured instruction features and referenced
method names, not executable payloads or source code. Association to a packet
layout and effective Map53 affine coefficients requires independent evidence.
"""
from pathlib import Path
import argparse, hashlib, json, struct, sys, bisect
ROOT=Path(__file__).resolve().parents[3]
sys.path.insert(0,str(ROOT/".tmp/reference4016-python-deps"))
import pefile
from capstone import Cs,CS_ARCH_X86,CS_MODE_64,CS_GRP_CALL
from capstone.x86_const import X86_OP_IMM,X86_OP_MEM,X86_REG_RIP
from map_writer_xrefs import index_metadata

FOCUS={"ProcessGameUnity.OnSyncWorld":0xC38990,"MapService.UpdateWorldInfo":0x8E0DD0,
       "SubWorld.LoadMap":0xC75DE0,"SubWorld.Activate":0xC753C0}
def analyze(binary,metadata,codegen,writer):
    d=binary.read_bytes();raw_meta=metadata.read_bytes()
    binary_sha=hashlib.sha256(d).hexdigest().upper()
    meta_sha=hashlib.sha256(raw_meta).hexdigest().upper()
    if binary_sha!=codegen["sha256"] or binary_sha!=writer["binarySHA256"]:raise ValueError("Original PE mismatch")
    if meta_sha!=writer["metadataSHA256"]:raise ValueError("Original metadata mismatch")
    pe=pefile.PE(data=d,fast_load=True)
    base=pe.OPTIONAL_HEADER.ImageBase
    arr=int(codegen["candidates"][0]["methodArrayRVA"],16)
    count=codegen["candidates"][0]["methodPointerCount"]
    tok_to_owner=index_metadata(raw_meta)
    method_by_rva={}
    # Fetch the whole 19,668-slot table once. Calling pe.get_data once
    # per slot is prohibitively slow on some pefile versions.
    pointer_table=pe.get_data(arr,8*count)
    if len(pointer_table)!=8*count:raise ValueError("Truncated codegen pointer table")
    for token,(va,) in enumerate(struct.iter_unpack("<Q",pointer_table),1):
        row=tok_to_owner.get(token)
        if va>=base and row:
            full=row["type"]+"."+row["method"]
            method_by_rva.setdefault(va-base,[]).append({"tokenRow":token,"name":full})
    for name,rva in FOCUS.items():
        if name not in [e["name"] for e in method_by_rva.get(rva,[])]:
            raise ValueError("Missing expected method pointer: "+name)
    pdata=pe.OPTIONAL_HEADER.DATA_DIRECTORY[3]
    exception_data=pe.get_data(pdata.VirtualAddress,pdata.Size)
    frames=list(struct.iter_unpack("<III",exception_data[:len(exception_data)//12*12]))
    frames.sort()
    starts=[x[0] for x in frames]
    def extent(start):
        i=bisect.bisect_right(starts,start)-1
        if i<0 or not frames[i][0]<=start<frames[i][1]:
            raise ValueError(f"Missing exact unwind frame for 0x{start:X}")
        if frames[i][0]!=start:raise ValueError("Expected exact method entry")
        return frames[i][1]-start
    decoder=Cs(CS_ARCH_X86,CS_MODE_64);decoder.detail=True
    results={}
    for name,start in FOCUS.items():
        length=extent(start)
        if length>16384:raise ValueError("Unsafe native method size")
        insns=list(decoder.disasm(pe.get_data(start,length),base+start))
        calls=[]
        inputs=[]
        snippets=[]
        load_contexts=[]
        for k,inst in enumerate(insns):
            rva=inst.address-base
            if inst.group(CS_GRP_CALL):
                direct=next((op.imm-base for op in inst.operands if op.type==X86_OP_IMM),None)
                if direct is not None:
                    calls.append({"atRVA":hex(rva),"targetRVA":hex(direct),
                                  "knownMethodNames":[m["name"] for m in method_by_rva.get(direct,[])]})
            for arg_index,op in enumerate(inst.operands):
                if op.type==X86_OP_MEM:
                    basereg=inst.reg_name(op.mem.base)
                    if basereg in ("rbx","rcx","rsi","rdi","rdx","r8","r9") and -0x200<=op.mem.disp<=0x400:
                        inputs.append({"rva":hex(rva),"mnemonic":inst.mnemonic,
                                      "reg":basereg,"displacement":op.mem.disp,
                                      "isWrite":arg_index==0})
            # Local callsite context has enough data to review argument flow.
            if name=="ProcessGameUnity.OnSyncWorld" and inst.mnemonic=="call" and inst.op_str.lower()==f"0x{base+0x8E0DD0:x}":
                begin=max(0,k-35)
                snippets=[{"rva":hex(x.address-base),"op":x.mnemonic,"args":x.op_str}
                          for x in insns[begin:k+5]]
            if name=="MapService.UpdateWorldInfo" and inst.mnemonic=="call" and inst.op_str.lower() in (
                    f"0x{base+0xC75DE0:x}",f"0x{base+0xC753C0:x}"):
                load_contexts.append({"targetRVA":hex(int(inst.op_str,16)-base),
                    "atRVA":hex(rva),"instructions":[
                        {"rva":hex(x.address-base),"op":x.mnemonic,"args":x.op_str}
                        for x in insns[max(0,k-24):k+3]]})
        results[name]={"rva":hex(start),"unwindLengthBytes":length,
                       "decodedInstructionCount":len(insns),
                       "calls":calls,"baseRegisterFieldAccesses":inputs,
                       "entryInstructions":[{"rva":hex(x.address-base),"op":x.mnemonic,"args":x.op_str}
                                            for x in insns[:95]],
                       "worldInfoCallsiteContext":snippets,
                       "subworldCallsiteContexts":load_contexts}
    return {"binarySHA256":binary_sha,"metadataSHA256":meta_sha,
            "results":results,"limitations":"Only direct codegen method associations, bounded PE64 unwind ranges, register-relative accesses and static callsite; no object type/parameter schema claimed without separate metadata corroboration."}
def main():
    p=argparse.ArgumentParser()
    p.add_argument("--binary",type=Path,required=True)
    p.add_argument("--metadata",type=Path,required=True)
    p.add_argument("--module",type=Path,required=True)
    p.add_argument("--writer-audit",type=Path,required=True)
    p.add_argument("--out",type=Path,required=True)
    args=p.parse_args()
    x=analyze(args.binary,args.metadata,json.loads(args.module.read_text(encoding="utf8")),
              json.loads(args.writer_audit.read_text(encoding="utf8")))
    args.out.parent.mkdir(parents=True,exist_ok=True)
    args.out.write_text(json.dumps(x,ensure_ascii=False,indent=2),encoding="utf8")
    for name,row in x["results"].items():
        print("METHOD",name,"RVA",row["rva"],"bytes",row["unwindLengthBytes"],
              "instructions",row["decodedInstructionCount"])
        print("DIRECT_CALLS",len(row["calls"]),
              "KNOWN_CALLEES",[x for x in row["calls"] if x["knownMethodNames"]][:25])
        if row["worldInfoCallsiteContext"]:
            print("WORLDINFO_CALLSITE",row["worldInfoCallsiteContext"][-10:])
        print("MEMORY_ACCESSES",len(row["baseRegisterFieldAccesses"]))
if __name__=="__main__":main()

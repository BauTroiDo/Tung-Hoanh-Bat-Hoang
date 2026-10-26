"""Statically audit writes/dispatch around MapService in the original 4016 PE64.

Relies on independently hashed IL2CPP metadata and original binary; no binary
loading into memory as executable code. Exports bounded instruction signatures,
not decompiled proprietary source.
"""
from pathlib import Path
import argparse, bisect, hashlib, json, struct, sys

ROOT=Path(__file__).resolve().parents[3]
sys.path.insert(0,str(ROOT/".tmp/reference4016-python-deps"))
import pefile
from capstone import Cs,CS_ARCH_X86,CS_MODE_64,CS_GRP_CALL
from capstone.x86_const import X86_OP_IMM,X86_OP_MEM,X86_OP_REG,X86_REG_RIP

NAME_TARGETS=["Calibrate","Mps2WorldPos","WorldPos2Mps","Map2WorldPos",
              "UpdateWorldInfo","Solve3x3"]
DEFAULT_OFFSETS={0:"m00",4:"m01",8:"m10",12:"m11",16:"offsetX",20:"offsetZ"}

def index_metadata(raw):
    sections=[struct.unpack_from("<III",raw,8+12*i) for i in range(31)]
    assert struct.unpack_from("<III",raw,0)==(0xFAB11BAF,39,380)
    str_off,str_len,_=sections[2]
    type_off,_,tc=sections[19]
    meth_off,_,mc=sections[5]
    def getname(i):
        if not 0<=i<str_len:return None
        off=str_off+i
        stop=raw.find(bytes([0]),off,str_off+str_len)
        if stop<off or stop-off>220:return None
        return raw[off:stop].decode("utf8","replace")
    types=[getname(struct.unpack_from("<I",raw,type_off+i*82)[0]) for i in range(tc)]
    mapping={}
    # Metadata MethodDef tokens restart at 1 in each assembly. Only the
    # first Assembly-CSharp.dll image owns token rows in this CodeGenModule.
    image_off,_,ic=sections[20]
    image_row=struct.unpack_from("<9I",raw,image_off)
    first_type=image_row[2]&0xffff
    image_type_count=image_row[2]>>16
    if first_type!=0 or image_type_count!=2669:raise ValueError("Unexpected Assembly-CSharp type ownership")
    for i in range(mc):
        p=meth_off+32*i
        owner=struct.unpack_from("<H",raw,p+4)[0]
        if not first_type<=owner<first_type+image_type_count:continue
        tok=struct.unpack_from("<I",raw,p+20)[0]
        row=tok&0xffffff
        if 0<row<=19668 and tok>>24==6:
            if row in mapping:raise ValueError("Duplicate token within Assembly-CSharp")
            mapping[row]={"method":getname(struct.unpack_from("<I",raw,p)[0]),"type":types[owner],"metadataIndex":i}
    if len(mapping)<19000 or mapping[0x13B8]["method"]!="Calibrate" or mapping[0x139D]["method"]!="Mps2WorldPos":
        raise ValueError("Wrong codegen token mapping")
    return mapping

def audit(binpath,metapath,module_path,fields_path):
    binary=binpath.read_bytes()
    metadata=metapath.read_bytes()
    module=json.loads(module_path.read_text(encoding="utf8"))
    fields=json.loads(fields_path.read_text(encoding="utf8"))
    digest=hashlib.sha256(binary).hexdigest().upper()
    if digest!=module["sha256"] or digest!=fields["binarySHA256"]:raise ValueError("PE source changed")
    if hashlib.sha256(metadata).hexdigest().upper()!=fields["metadataSHA256"]:raise ValueError("metadata changed")
    pe=pefile.PE(data=binary,fast_load=True)
    base=pe.OPTIONAL_HEADER.ImageBase
    pointer_base=int(module["candidates"][0]["methodArrayRVA"],16)
    n=module["candidates"][0]["methodPointerCount"]
    mapitems=index_metadata(metadata)
    entries={}
    for k in range(1,n+1):
        va=struct.unpack("<Q",pe.get_data(pointer_base+(k-1)*8,8))[0]
        if va>=base:
            entries.setdefault(va-base,[]).append({"tokenRow":k,**mapitems.get(k,{"method":"<unresolved>","type":"<unresolved>"})})
    pdata_dir=pe.OPTIONAL_HEADER.DATA_DIRECTORY[3]
    pdata=pe.get_data(pdata_dir.VirtualAddress,pdata_dir.Size)
    frames=[struct.unpack_from("<III",pdata,i) for i in range(0,len(pdata)-11,12)]
    frames.sort()
    beginnings=[f[0] for f in frames]
    def framefor(rva):
        k=bisect.bisect_right(beginnings,rva)-1
        if k<0:return None
        begin,end,_=frames[k]
        return (begin,end) if begin<=rva<end else None
    # Direct E8/E9 byte probes are not complete control-flow proofs.
    target_methods={x["name"]:int(x["nativeRVA"],16)
                    for x in fields["targets"] if x["owner"]=="MapService"
                    and x["name"] in NAME_TARGETS}
    direct=[]
    for sec in pe.sections:
        if not sec.Characteristics&0x20000000:continue
        rawsec=sec.get_data()
        for kind,code in (("call",0xe8),("jump",0xe9)):
            start=0
            while (pos:=rawsec.find(bytes([code]),start))>=0:
                start=pos+1
                if pos+5>len(rawsec):break
                here=sec.VirtualAddress+pos
                displacement=struct.unpack_from("<i",rawsec,pos+1)[0]
                destination=here+5+displacement
                name=next((k for k,v in target_methods.items() if v==destination),None)
                if name:
                    fr=framefor(here)
                    direct.append({"target":name,"kind":kind,"fromRVA":hex(here),
                        "owningFunctionStartRVA":hex(fr[0]) if fr else None,
                        "ownerLabels":entries.get(fr[0],[]) if fr else [],
                        "status":"RAW_BYTE_CANDIDATE_NOT_VERIFIED_BOUNDARY"})
    # Scoped inspection of every MapService body (not whole PE)
    decoder=Cs(CS_ARCH_X86,CS_MODE_64)
    decoder.detail=True
    maps=[(rva,labels) for rva,labels in entries.items() if any(x["type"]=="MapService" for x in labels)]
    analyses=[]
    for rva,labels in sorted(maps):
        fr=framefor(rva)
        sz=min(fr[1]-rva,8192) if fr else 512
        code=pe.get_data(rva,sz)
        stores=[]
        calls=[]
        readStatic=False
        for j,ins in enumerate(decoder.disasm(code,base+rva)):
            if j>800:break
            if ins.group(CS_GRP_CALL):
                target=next((op.imm-base for op in ins.operands if op.type==X86_OP_IMM),None)
                if target is not None and target in (0x8DD750,0x8E0870,0x8DFD10,0xC790F0):
                    calls.append({"atRVA":hex(ins.address-base),"targetRVA":hex(target)})
            if ins.mnemonic in ("mov","movss","movups","movaps","xor","lea"):
                operands=ins.operands
                if len(operands)>=1 and operands[0].type==X86_OP_MEM:
                    destination=operands[0].mem
                    if destination.disp in DEFAULT_OFFSETS:
                        stores.append({"rva":hex(ins.address-base),
                                       "offset":destination.disp,
                                       "fieldCandidate":DEFAULT_OFFSETS[destination.disp],
                                       "baseRegister":ins.reg_name(destination.base),
                                       "operation":ins.mnemonic,
                                       "warning":"Access to matching offset, NOT PROOF register is MapService staticFields"})
            if ins.mnemonic=="ret" and fr is None:break
        analyses.append({"rva":hex(rva),"names":[x["method"] for x in labels if x["type"]=="MapService"],
                         "instructionWindowBytes":sz,"fieldOffsetWrites":stores,
                         "selectedCalls":calls})
    # Second, independently decode each raw candidate from its owning PE
    # unwind entry, requiring the last complete instruction to be CALL/JMP.
    for row in direct:
        from_rva=int(row["fromRVA"],16)
        begin=int(row["owningFunctionStartRVA"],16) if row["owningFunctionStartRVA"] else None
        if begin is None or from_rva-begin>16384:
            row["status"]="UNWIND_UNRESOLVED"
            continue
        stream=pe.get_data(begin,from_rva-begin+5)
        decoded=list(decoder.disasm_lite(stream,base+begin))
        last=decoded[-1] if decoded else None
        target=target_methods[row["target"]]
        expected=f"0x{base+target:x}"
        if last and last[0]-base==from_rva and last[1]==5 and last[2] in ("call","jmp") and last[3]==expected:
            row["status"]="INSTRUCTION_BOUNDARY_CONFIRMED"
        else:
            row["status"]="NOT_CONFIRMED_INSTRUCTION_BOUNDARY"
    bytarget={k:sum(r["target"]==k for r in direct) for k in NAME_TARGETS}
    verified={k:sum(r["target"]==k and r["status"]=="INSTRUCTION_BOUNDARY_CONFIRMED" for r in direct) for k in NAME_TARGETS}
    return {"binarySHA256":digest,"metadataSHA256":hashlib.sha256(metadata).hexdigest().upper(),
            "asmCsharpPointerSlots":n,"uniqueExecutablePointers":len(entries),
            "MapServiceMethodBodies":len(analyses),
            "directRelativeTransferCounts":bytarget,
            "instructionBoundaryConfirmedCounts":verified,
            "directRelativeTransferCandidates":direct[:300],
            "mapServiceScopedMethods":analyses,
            "constraints":"Direct transfer matches are raw bytes. Offset writes are candidate offsets without register dataflow. Reflection, virtual, delegate and dynamic calls unverified; no live Map53 values."}

def main():
    a=argparse.ArgumentParser()
    a.add_argument("--binary",type=Path,required=True)
    a.add_argument("--metadata",type=Path,required=True)
    a.add_argument("--module",type=Path,required=True)
    a.add_argument("--fields",type=Path,required=True)
    a.add_argument("--out",type=Path,required=True)
    s=a.parse_args()
    out=audit(s.binary,s.metadata,s.module,s.fields)
    s.out.parent.mkdir(parents=True,exist_ok=True)
    s.out.write_text(json.dumps(out,ensure_ascii=False,indent=2),encoding="utf8")
    print("METHOD_BODIES",out["MapServiceMethodBodies"],"DIRECT_COUNTERS",out["directRelativeTransferCounts"])
    for row in out["mapServiceScopedMethods"]:
        if row["fieldOffsetWrites"] or row["selectedCalls"]:
            print("FUNCTION",row["rva"],row["names"],"STORES",len(row["fieldOffsetWrites"]),"SELECTED_CALLS",row["selectedCalls"])
    print("FIRST_CALLERS",out["directRelativeTransferCandidates"][:15])
if __name__=="__main__":main()

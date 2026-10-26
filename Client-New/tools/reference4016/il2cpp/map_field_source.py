"""Read-only bounded MapService constructor/provider reconnaissance (IL2CPP v39).

Lists metadata ownership and native methods with byte fingerprints only, never
executes or modifies GameAssembly.dll. Do not infer Map53 coefficients from names.
"""
from pathlib import Path
import argparse,hashlib,json,struct,sys
ROOT=Path(__file__).resolve().parents[3]
sys.path.insert(0,str(ROOT/".tmp/reference4016-python-deps"))
import pefile
from capstone import Cs,CS_ARCH_X86,CS_MODE_64,CS_GRP_CALL
from capstone.x86_const import X86_OP_IMM,X86_OP_MEM,X86_REG_RIP

def extract_map_fields(metadata,binary,targets):
    """Recover original field names and six .cctor float32 writes without execution."""
    import re
    raw=metadata.read_bytes()
    sec=[struct.unpack_from("<III",raw,8+i*12) for i in range(31)]
    string_off,string_len,_=sec[2]
    to,_,tc=sec[19]
    field_off,_,field_count=sec[11]
    row=next(x for x in targets if x["owner"]=="MapService" and x["name"]==".cctor")
    idx=row["ownerIndex"]
    if idx+1>=tc:raise ValueError("Missing next type")
    first=struct.unpack_from("<I",raw,to+idx*82+26)[0]
    last=struct.unpack_from("<I",raw,to+(idx+1)*82+26)[0]
    if first>=last or last>field_count:raise ValueError("Field table bounds")
    def get_name(i):
        offset=struct.unpack_from("<I",raw,field_off+12*i)[0]
        if offset>=string_len:raise ValueError("Invalid field name index")
        at=string_off+offset;end=raw.find(bytes([0]),at,string_off+string_len)
        return raw[at:end].decode("utf8")
    names=[get_name(i) for i in range(first,last)]
    wanted=["m00","m01","m10","m11","offsetX","offsetZ"]
    if names[:6]!=wanted:raise ValueError("First six MapService field names diverged")
    pe=pefile.PE(str(binary),fast_load=True)
    start=int(row["nativeRVA"],16)
    cs=Cs(CS_ARCH_X86,CS_MODE_64)
    defaults={}
    audit=[]
    for ins in cs.disasm(pe.get_data(start,1100),pe.OPTIONAL_HEADER.ImageBase+start):
        if ins.mnemonic!="mov":continue
        match=re.fullmatch(r"dword ptr \[rcx(?: \+ (0x[0-9a-f]+|[0-9]+))?\], (0x[0-9a-f]+|[0-9]+)",ins.op_str)
        if not match:continue
        off=int(match.group(1),0) if match.group(1) else 0
        word=int(match.group(2),0)&0xFFFFFFFF
        if off in (0,4,8,12,16,20) and off not in defaults:
            val=struct.unpack("<f",struct.pack("<I",word))[0]
            defaults[off]={"field":wanted[off//4],"staticFieldOffset":off,
                          "float32":val,"rawWord":f"0x{word:08X}",
                          "writerRVA":f"0x{ins.address-pe.OPTIONAL_HEADER.ImageBase:X}"}
    if len(defaults)!=6:raise ValueError("Six constructor default writes not found")
    calibrate=next(x for x in targets if x["owner"]=="MapService" and x["name"]=="Calibrate")
    entry=int(calibrate["nativeRVA"],16)
    dir=pe.OPTIONAL_HEADER.DATA_DIRECTORY[3]
    unw=pe.get_data(dir.VirtualAddress,dir.Size)
    extent=5000
    for j in range(0,len(unw)-11,12):
        begin,end,_=struct.unpack_from("<III",unw,j)
        if begin==entry:
            extent=end-begin
            break
    codes=list(cs.disasm(pe.get_data(entry,min(extent,8192)),pe.OPTIONAL_HEADER.ImageBase+entry))
    solvecalls=[f"0x{x.address-pe.OPTIONAL_HEADER.ImageBase:X}" for x in codes
                if x.mnemonic=="call" and x.op_str.lower()==f"0x{pe.OPTIONAL_HEADER.ImageBase+0x8E0870:x}"]
    if len(solvecalls)!=2:raise ValueError("Unexpected number of native Solve3x3 calls")
    return {"mapServiceTypeIndex":idx,"fieldFirstIndex":first,"fieldLastIndex":last,
            "fieldNames":names,"constructorDefaults":[defaults[i] for i in (0,4,8,12,16,20)],
            "calibrateRVA":calibrate["nativeRVA"],"calibrateSolve3x3RVA":"0x8E0870",
            "calibrateSolveCallsites":solvecalls,
            "caveat":"Global defaults only. No proof of effective Map53 runtime values or map scene alignment."}

def run(metadata,binary,module):
    raw=metadata.read_bytes();data=binary.read_bytes()
    if struct.unpack_from("<III",raw,0)!=(0xFAB11BAF,39,380):raise ValueError("expected v39 metadata")
    if hashlib.sha256(data).hexdigest().upper()!=module["sha256"]:raise ValueError("PE hash changed")
    sections=[struct.unpack_from("<III",raw,8+i*12) for i in range(31)]
    so,sl,_=sections[2]
    mo,ms,mc=sections[5];to,ts,tc=sections[19]
    def name(offset):
        if not 0<=offset<sl:return None
        begin=so+offset
        end=raw.find(b"\0",begin,so+sl)
        if end<begin or end-begin>256:return None
        try:return raw[begin:end].decode("utf8")
        except UnicodeDecodeError:return None
    types=[]
    for i in range(tc):
        t=to+82*i
        types.append({"index":i,"name":name(struct.unpack_from("<I",raw,t)[0]),"namespace":name(struct.unpack_from("<I",raw,t+4)[0])})
    pe=pefile.PE(data=data,fast_load=True)
    base=pe.OPTIONAL_HEADER.ImageBase
    count=module["candidates"][0]["methodPointerCount"]
    ptr_rva=int(module["candidates"][0]["methodArrayRVA"],16)
    cs=Cs(CS_ARCH_X86,CS_MODE_64);cs.detail=True
    name_targets={"MapService","MapConverter","MapSceneInfo","SubWorld","MapScene","MapSceneLoader"}
    index_set={t["index"] for t in types if t["name"] in name_targets}
    methods=[]
    for i in range(mc):
        off=mo+i*32
        owner=struct.unpack_from("<H",raw,off+4)[0]
        if owner not in index_set:continue
        token=struct.unpack_from("<I",raw,off+20)[0]
        nm=name(struct.unpack_from("<I",raw,off)[0])
        rownum=token&0xffffff
        item={"owner":types[owner]["name"],"ownerIndex":owner,"index":i,"name":nm,"token":f"0x{token:X}","nativeRVA":None}
        if 0<rownum<=count:
            va=struct.unpack("<Q",pe.get_data(ptr_rva+(rownum-1)*8,8))[0]
            if va>=base: item["nativeRVA"]=f"0x{va-base:X}"
        methods.append(item)
    print("METHODS",len(methods))
    for m in methods:
        if m["owner"]=="MapService":print("MAPSERVICE",m)
    return {"metadataSHA256":hashlib.sha256(raw).hexdigest().upper(),
            "binarySHA256":module["sha256"],"targets":methods,
            "limits":"Metadata ownership and pointer arrays are static; native Map53 values not read from runtime."}

def inspect_method(binary,record):
    """Show disassembly in console for bounded, source-owned static examination."""
    if not record or not record["nativeRVA"]:return
    pe=pefile.PE(str(binary),fast_load=True)
    start=int(record["nativeRVA"],16)
    base=pe.OPTIONAL_HEADER.ImageBase
    exception=pe.OPTIONAL_HEADER.DATA_DIRECTORY[3]
    records=pe.get_data(exception.VirtualAddress,exception.Size)
    length=512
    scope="unwind-unresolved"
    for off in range(0,len(records)-11,12):
        begin,end,unwind=struct.unpack_from("<III",records,off)
        if begin<=start<end:
            length=min(end-start,14000)
            scope="PE-pdata"
            break
    engine=Cs(CS_ARCH_X86,CS_MODE_64)
    engine.detail=True
    seen=0
    indirect=[]
    for inst in engine.disasm(pe.get_data(start,length),base+start):
        if seen>=450:break
        print("ASM",record["name"],hex(inst.address-base),inst.mnemonic,inst.op_str)
        seen+=1
        if inst.mnemonic=="ret" and scope!="PE-pdata":break
    print("DISASM",record["name"],"scope",scope,"bytes",length,"printed",seen)

def scan_calibrate_callers(binary,targets):
    """Enumerate direct E8 callsites to mapped Calibrate native RVA.

    Verify each candidate is an instruction boundary in a PE unwind function.
    Does not claim absence of indirect, generic or reflection-based invocations.
    """
    import bisect
    pe=pefile.PE(str(binary),fast_load=True)
    image_base=pe.OPTIONAL_HEADER.ImageBase
    cal=next(x for x in targets if x["owner"]=="MapService" and x["name"]=="Calibrate")
    dest=int(cal["nativeRVA"],16)
    directory=pe.OPTIONAL_HEADER.DATA_DIRECTORY[3]
    table=pe.get_data(directory.VirtualAddress,directory.Size)
    frames=sorted([struct.unpack_from("<III",table,k) for k in range(0,len(table)-11,12)])
    starts=[x[0] for x in frames]
    cs=Cs(CS_ARCH_X86,CS_MODE_64)
    method_rvas={int(m["nativeRVA"],16):m["owner"]+"."+str(m["name"])
                 for m in targets if m["nativeRVA"]}
    found=[]
    candidates=0
    for section in pe.sections:
        if not section.Characteristics & 0x20000000:continue
        view=section.get_data()
        origin=int(section.VirtualAddress)
        pos=-1
        while (pos:=view.find(b"\\xE8",pos+1))>=0:
            if pos+5>len(view):break
            location=origin+pos
            displacement=struct.unpack_from("<i",view,pos+1)[0]
            if location+5+displacement!=dest:continue
            candidates+=1
            idx=bisect.bisect_right(starts,location)-1
            frame=frames[idx] if idx>=0 and location<frames[idx][1] else None
            if frame is None:
                found.append({"callRVA":f"0x{location:X}","status":"NO_UNWIND_BOUNDARY"})
                continue
            beg,end,_=frame
            if location-beg>40000:
                found.append({"callRVA":f"0x{location:X}","functionRVA":f"0x{beg:X}","status":"FUNCTION_TOO_LARGE_TO_VALIDATE"})
                continue
            insns=list(cs.disasm(pe.get_data(beg,location-beg+5),image_base+beg))
            last=insns[-1] if insns else None
            ok=bool(last and last.address==image_base+location and last.mnemonic=="call"
                    and last.op_str==f"0x{image_base+dest:x}")
            found.append({"callRVA":f"0x{location:X}","functionRVA":f"0x{beg:X}",
                          "methodNameByExactPointer":method_rvas.get(beg),
                          "status":"DIRECT_CALL_VALIDATED" if ok else "UNVERIFIED_BYTE_SEQUENCE"})
    return {"targetRVA":f"0x{dest:X}","rawE8Candidates":candidates,"callsites":found,
            "limits":"Direct E8 only; indirect/virtual/reflection calls not exhaustively searched."}

def main():
    p=argparse.ArgumentParser()
    p.add_argument("--metadata",type=Path,required=True)
    p.add_argument("--binary",type=Path,required=True)
    p.add_argument("--module",type=Path,required=True)
    p.add_argument("--out",type=Path,required=True)
    p.add_argument("--detail-method",default=None)
    p.add_argument("--scan-callers",action="store_true")
    a=p.parse_args()
    r=run(a.metadata,a.binary,json.loads(a.module.read_text(encoding="utf8")))
    r["mapServiceFields"]=extract_map_fields(a.metadata,a.binary,r["targets"])
    print("STATIC_DEFAULTS",r["mapServiceFields"]["constructorDefaults"])
    if a.scan_callers:
        r["calibrateCallerAudit"]=scan_calibrate_callers(a.binary,r["targets"])
        print("CALIBRATE_CALLERS",r["calibrateCallerAudit"])
    if a.detail_method:
        matches=[x for x in r["targets"] if x["owner"]=="MapService" and x["name"]==a.detail_method]
        if len(matches)!=1:raise ValueError("detail method not uniquely MapService-owned")
        inspect_method(a.binary,matches[0])
    a.out.parent.mkdir(parents=True,exist_ok=True)
    a.out.write_text(json.dumps(r,ensure_ascii=False,indent=2),encoding="utf8")
if __name__=="__main__": main()

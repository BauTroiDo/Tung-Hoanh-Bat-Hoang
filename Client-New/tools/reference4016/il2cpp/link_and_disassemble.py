"""Link selected v39 method tokens to statically observed PE code pointers.

Uses unique Assembly-CSharp CodeGenModule structural record; output is a
candidate until global CodeRegistration and independent runtime/callgraph review.
Never executes or modifies the DLL. Limits disassembly to 80 instructions/method.
"""
import argparse,hashlib,json,struct,sys
from pathlib import Path
ROOT=Path(__file__).resolve().parents[3]
sys.path.insert(0,str(ROOT/".tmp/reference4016-python-deps"))
import pefile
from capstone import Cs,CS_ARCH_X86,CS_MODE_64
from capstone.x86_const import X86_OP_MEM,X86_REG_RIP

KEYS={"Mps2WorldPos","Map2WorldPos","WorldPos2Mps","MapToWorldPos","MpsToMapPos","Map2Mps","Mps2Map","WorldDeltaToMpsDelta","get_MpsWorldScale","GridToMpsCentreX","GridToMpsCentreY"}
def main():
    a=argparse.ArgumentParser();a.add_argument("--binary",type=Path,required=True)
    a.add_argument("--catalog",type=Path,required=True);a.add_argument("--module",type=Path,required=True)
    a.add_argument("--out",type=Path,required=True)
    args=a.parse_args()
    cat=json.loads(args.catalog.read_text(encoding="utf8"))
    module=json.loads(args.module.read_text(encoding="utf8"))
    if module["searchStatus"]!="ONE_STRUCTURAL_CANDIDATE":raise ValueError("Module not unique, fail closed")
    b=args.binary.read_bytes(); digest=hashlib.sha256(b).hexdigest().upper()
    if digest!=module["sha256"]:raise ValueError("PE source hash changed")
    pe=pefile.PE(data=b,fast_load=True);base=pe.OPTIONAL_HEADER.ImageBase
    selected=module["candidates"][0]
    count=selected["methodPointerCount"]
    arrRVA=int(selected["methodArrayRVA"],16)
    md=Cs(CS_ARCH_X86,CS_MODE_64);md.detail=True
    rows=[]
    for item in cat["targetMethods"]:
        if item["name"] not in KEYS:continue
        if item["image"]["name"]!="Assembly-CSharp.dll":
            raise ValueError("Target ownership unexpectedly changed")
        tokenRow=int(item["tokenRow"])
        if not 0<tokenRow<=count:raise ValueError("Token row outside codegen module count")
        va=struct.unpack("<Q",pe.get_data(arrRVA+(tokenRow-1)*8,8))[0]
        row={k:item[k] for k in ("methodIndex","name","ownerType","token","tokenRow")}
        row["nativeVA"]=f"0x{va:X}" if va else None
        row["nativeRVA"]=f"0x{va-base:X}" if va>=base else None
        if not va:row["status"]="NO_BODY";rows.append(row);continue
        rva=va-base
        section=next((s for s in pe.sections if s.VirtualAddress<=rva<s.VirtualAddress+s.Misc_VirtualSize),None)
        if not section or not (section.Characteristics&0x20000000):
            row["status"]="INVALID_NON_EXECUTABLE_POINTER";rows.append(row);continue
        stream=pe.get_data(rva,512)
        insns=[];samples=[]
        for ins in md.disasm(stream,va):
            if len(insns)>=80:break
            insns.append({"rva":f"0x{ins.address-base:X}","mnemonic":ins.mnemonic,"operands":ins.op_str})
            for op in ins.operands:
                if op.type==X86_OP_MEM and op.mem.base==X86_REG_RIP and op.size in (4,8):
                    address=ins.address+ins.size+op.mem.disp
                    rv=address-base
                    raw=pe.get_data(rv,op.size)
                    if len(raw)!=op.size:continue
                    val=struct.unpack("<f" if op.size==4 else "<d",raw)[0]
                    if -1000000<val<1000000:
                        samples.append({"atRVA":f"0x{ins.address-base:X}","dataRVA":f"0x{rv:X}","float":val,"width":op.size})
            if ins.mnemonic=="ret":break
        row["status"]="STATIC_CODE_CANDIDATE"
        row["instructionCount"]=len(insns)
        row["instructions"]=insns
        row["possibleRipRelativeFloats"]=samples[:20]
        rows.append(row)
    result={"binaryName":args.binary.name,"binarySHA256":digest,"metadataSHA256":cat["sourceSha256"],"moduleStatus":module["searchStatus"],"moduleRVA":selected["recordRVA"],"methodPointerCount":count,
      "limitations":"Static method slot pointers and bounded disassembly are not live callgraph or calibration proof. RIP floats may be data, not actual constants.","targets":rows}
    args.out.parent.mkdir(parents=True,exist_ok=True)
    args.out.write_text(json.dumps(result,indent=2,ensure_ascii=False),encoding="utf8")
    for row in rows:
        print("METHOD",row["ownerType"]["name"]+"."+row["name"],row["token"],row["nativeRVA"],row["status"],"count",row.get("instructionCount"))
        if row["name"] in ("Mps2WorldPos","WorldPos2Mps","get_MpsWorldScale","Map2WorldPos","MapToWorldPos","Map2Mps","Mps2Map"):
            for ins in row.get("instructions",[])[:22]:print("  ",ins["rva"],ins["mnemonic"],ins["operands"])
            print("  FLOAT_CANDIDATES",row.get("possibleRipRelativeFloats",[])[:10])
if __name__=="__main__":main()

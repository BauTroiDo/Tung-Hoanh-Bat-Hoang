"""Audit bounded PE64 function CFG, float constants, and map-instance field reads.

This tool never loads or executes the PE. Addresses are relative to its image base.
Output is diagnostic, not verified runtime coordinate calibration.
"""
import argparse, hashlib, json, struct, sys
from pathlib import Path
ROOT=Path(__file__).resolve().parents[3]
sys.path.insert(0,str(ROOT/".tmp/reference4016-python-deps"))
import pefile
from capstone import Cs, CS_ARCH_X86, CS_MODE_64, CS_GRP_CALL, CS_GRP_JUMP
from capstone.x86_const import X86_OP_IMM, X86_OP_MEM, X86_REG_RIP

TARGETS={"MapService.Mps2WorldPos","MapService.WorldPos2Mps","MapService.get_MpsWorldScale",
         "MapService.Map2WorldPos","MapService.WorldDeltaToMpsDelta",
         "MapConverter.MapToWorldPos","MapConverter.MpsToMapPos","SubWorld.Map2Mps"}

def analyze(binary,candidate):
    raw=binary.read_bytes()
    digest=hashlib.sha256(raw).hexdigest().upper()
    if digest != candidate["binarySHA256"]: raise ValueError("binary hash mismatch")
    pe=pefile.PE(data=raw,fast_load=True)
    base=pe.OPTIONAL_HEADER.ImageBase
    sec=pe.OPTIONAL_HEADER.DATA_DIRECTORY[3]
    frames=[]
    if sec.Size and sec.VirtualAddress:
        data=pe.get_data(sec.VirtualAddress,sec.Size)
        for offset in range(0,len(data)-11,12):
            begin,end,unwind=struct.unpack_from("<III",data,offset)
            if 0 < begin < end: frames.append((begin,end,unwind))
    md=Cs(CS_ARCH_X86,CS_MODE_64)
    md.detail=True
    records=[]
    for row in candidate["targets"]:
        name=row["ownerType"]["name"]+"."+row["name"]
        if name not in TARGETS or not row["nativeRVA"]:continue
        start=int(row["nativeRVA"],16)
        matches=[x for x in frames if x[0]<=start<x[1]]
        if len(matches)>1:raise ValueError("ambiguous unwind record")
        if matches:
            body_start,body_end,unwind=matches[0]
            bound=min(body_end-start,8192)
            scope="pdata"
        else:
            # No registered unwind entry for a leaf function. Don't scan
            # arbitrarily past an unconditional RET.
            body_start,body_end,unwind=start,None,None
            bound=512
            scope="bounded-leaf"
        stream=pe.get_data(start,bound)
        insns=[]
        edges=[]
        floats=[]
        fields={}
        return_seen=False
        for inst in md.disasm(stream,base+start):
            if len(insns)>=800:break
            insns.append({"rva":f"0x{inst.address-base:X}","op":inst.mnemonic,"args":inst.op_str})
            if inst.group(CS_GRP_CALL) or inst.group(CS_GRP_JUMP):
                direct=next((op.imm-base for op in inst.operands if op.type==X86_OP_IMM),None)
                edges.append({"from":f"0x{inst.address-base:X}","kind":"call" if inst.group(CS_GRP_CALL) else "branch",
                              "target":f"0x{direct:X}" if direct is not None else None,
                              "inFunction":bool(direct is not None and start<=direct<(body_end or start+bound))})
            for op in inst.operands:
                if op.type!=X86_OP_MEM:continue
                if op.mem.base==X86_REG_RIP:
                    rv=inst.address+inst.size+op.mem.disp-base
                    if op.size==4 and inst.mnemonic in {"movss","mulss","divss","addss","subss","ucomiss","comiss","maxss","minss"}:
                        bs=pe.get_data(rv,4)
                        if len(bs)==4:
                            value=struct.unpack("<f",bs)[0]
                            if abs(value)<1e8:
                                floats.append({"instructionRVA":f"0x{inst.address-base:X}","constantRVA":f"0x{rv:X}","float32":value,"op":inst.mnemonic})
                elif op.mem.disp and abs(op.mem.disp)<=0x200 and inst.mnemonic not in {"lea"}:
                    # Only a register-relative field offset, not a runtime value.
                    label=f"0x{op.mem.disp & 0xffffffff:X}"
                    fields[label]=fields.get(label,0)+1
            if inst.mnemonic.startswith("ret"):
                return_seen=True
                if scope=="bounded-leaf":break
        records.append({"method":name,"token":row["token"],"nativeRVA":row["nativeRVA"],
                        "startRVA":f"0x{start:X}","unwindStartRVA":f"0x{body_start:X}",
                        "unwindEndRVA":f"0x{body_end:X}" if body_end else None,
                        "unwindSource":scope,"instructionCount":len(insns),
                        "hasReturn":return_seen,"fieldOffsetCounts":fields,
                        "ripFloatReferences":floats,"branchAndCallEdges":edges,
                        "instructions":insns,
                        "warning":"Static branch listing; all map-specific values and caller argument semantics require independent verification."})
    return {"binarySHA256":digest,"imageBase":f"0x{base:X}","unwindFunctionCount":len(frames),
            "results":records,"purpose":"diagnostic only, no validated 3+1 legacy-to-Unity pairs"}
def main():
    p=argparse.ArgumentParser()
    p.add_argument("--binary",type=Path,required=True)
    p.add_argument("--candidates",type=Path,required=True)
    p.add_argument("--out",type=Path,required=True)
    a=p.parse_args()
    data=analyze(a.binary,json.loads(a.candidates.read_text(encoding="utf8")))
    a.out.parent.mkdir(parents=True,exist_ok=True)
    a.out.write_text(json.dumps(data,ensure_ascii=False,indent=2),encoding="utf8")
    print("UNWIND",data["unwindFunctionCount"],"FUNCTIONS",len(data["results"]))
    for r in data["results"]:
        print("METHOD",r["method"],r["nativeRVA"],"SCOPE",r["unwindSource"],
              "INSTRUCTIONS",r["instructionCount"],"RET",r["hasReturn"],
              "FLOATS",r["ripFloatReferences"],"FIELDS",r["fieldOffsetCounts"])
if __name__=="__main__":main()

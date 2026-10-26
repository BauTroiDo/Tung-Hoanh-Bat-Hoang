"""Static, read-only Calibrate dispatch audit on original 4016 PE64.

Enumerate exact absolute pointer references and direct relative transfer bytes.
Map position probes are diagnostics, NEVER independently paired landmarks.
"""
import argparse, hashlib, json, math, struct, sys
from pathlib import Path

ROOT=Path(__file__).resolve().parents[3]
sys.path.insert(0,str(ROOT / ".tmp/reference4016-python-deps"))
import pefile
from capstone import Cs, CS_ARCH_X86, CS_MODE_64

def occurrences(buffer,pattern):
    pos=0
    while (at:=buffer.find(pattern,pos))>=0:
        yield at
        pos=at+1

def verify_executable_instruction(pe,source_rva,target_rva,kind):
    # Use the PE section address and the source bytes to prove a local
    # instruction has matching displacement, without claiming control flow.
    ins=next(iter(Cs(CS_ARCH_X86,CS_MODE_64).disasm(pe.get_data(source_rva,5),
                                      pe.OPTIONAL_HEADER.ImageBase+source_rva)),None)
    if ins is None:return False
    return ins.size==5 and ins.mnemonic==kind and ins.op_str.lower()==f"0x{pe.OPTIONAL_HEADER.ImageBase+target_rva:x}"

def run(binary,method_data,static_scene,module,legacy_points=None,log_sha=None):
    raw=binary.read_bytes()
    sha=hashlib.sha256(raw).hexdigest().upper()
    if sha!=method_data["binarySHA256"]:raise ValueError("Original GameAssembly SHA has changed")
    pe=pefile.PE(data=raw,fast_load=True)
    base=pe.OPTIONAL_HEADER.ImageBase
    method=next(x for x in method_data["targets"] if x["owner"]=="MapService" and x["name"]=="Calibrate")
    dest=int(method["nativeRVA"],16)
    full_va=base+dest
    if not any(s.VirtualAddress<=dest<s.VirtualAddress+s.Misc_VirtualSize and bool(s.Characteristics&0x20000000) for s in pe.sections):
        raise ValueError("Mapped Calibrate RVA is not executable")
    if module["sha256"]!=sha or module["searchStatus"]!="ONE_STRUCTURAL_CANDIDATE":
        raise ValueError("Unvalidated codegen module")
    module_row=module["candidates"][0]
    token_row=int(method["token"],16)&0x00FFFFFF
    expected_slot_rva=int(module_row["methodArrayRVA"],16)+(token_row-1)*8
    if struct.unpack("<Q",pe.get_data(expected_slot_rva,8))[0]!=full_va:
        raise ValueError("Method pointer slot does not match Calibrate RVA")
    pointer_locations=[]
    for off in occurrences(raw,struct.pack("<Q",full_va)):
        try:
            rva=pe.get_rva_from_offset(off)
        except Exception:
            continue
        sec=next((s for s in pe.sections if s.VirtualAddress<=rva<s.VirtualAddress+s.Misc_VirtualSize),None)
        pointer_locations.append({"fileOffset":f"0x{off:X}","rva":f"0x{rva:X}",
                                  "section":sec.Name.rstrip(bytes([0])).decode("ascii","replace") if sec else None})
    branches=[]
    for sec in pe.sections:
        if not (sec.Characteristics&0x20000000):continue
        data=sec.get_data()
        for marker,kind in ((0xe8,"call"),(0xe9,"jmp")):
            i=0
            pat=bytes([marker])
            while (off:=data.find(pat,i))>=0:
                i=off+1
                if off+5>len(data):break
                at=sec.VirtualAddress+off
                delta=struct.unpack_from("<i",data,off+1)[0]
                if at+5+delta != dest:continue
                branches.append({"candidateRVA":f"0x{at:X}","kind":kind,
                                "decodesAsInstruction":verify_executable_instruction(pe,at,dest,kind)})
    defaults=method_data["mapServiceFields"]["constructorDefaults"]
    if [x["field"] for x in defaults] != ["m00","m01","m10","m11","offsetX","offsetZ"]:
        raise ValueError("Original MapService defaults mismatched")
    a,b,c,d,tx,tz=[x["float32"] for x in defaults]
    det=a*d-b*c
    scene=json.loads(static_scene.read_text(encoding="utf8"))
    tiles=scene["sceneCandidates"]
    if len(tiles)!=4 or any(t["hasLegacy"] for t in tiles):raise ValueError("Reference scene must be unpaired")
    x_values=[t["referenceWorldXYZ"][0] for t in tiles]
    z_values=[t["referenceWorldXYZ"][2] for t in tiles]
    bound={"xMin":min(x_values),"xMax":max(x_values)+250.0,
           "zMin":min(z_values),"zMax":max(z_values)+250.0,
           "caveat":"Nominal 2x2 250-unit tile envelope, not verified navigable geometry"}
    observations=[]
    for value in (legacy_points or []):
        x,y=value["mpsX"],value["mpsY"]
        u,v=x/512.,y/512.
        wx=a*u+b*v+tx;wz=c*u+d*v+tz
        observations.append({"source":value["source"],"assumedMpsXY":[x,y],
             "defaultProjectedUnityXZ":[wx,wz],
             "insideNominalStaticSceneEnvelope":bound["xMin"]<=wx<=bound["xMax"] and bound["zMin"]<=wz<=bound["zMax"],
             "status":"ASSUMED_MPS_UNPAIRED_NOT_VALIDATION"})
    if len(pointer_locations)!=1 or pointer_locations[0]["rva"]!=f"0x{expected_slot_rva:X}":
        raise ValueError("Calibrate pointer occurrence is not unique codegen slot")
    return {"binarySHA256":sha,"method":"MapService.Calibrate",
            "targetNativeRVA":f"0x{dest:X}","targetAbsoluteVA":f"0x{full_va:X}",
            "methodToken":method["token"],"methodPointerSlotRVA":f"0x{expected_slot_rva:X}",
            "absolutePointerOccurrences":pointer_locations,"directRelativeTransfers":branches,
            "defaultCoefficientDeterminant":det,"referenceSceneNominalBounds":bound,
            "probeLogSHA256":log_sha,"unpairedSanityProbes":observations,
            "limits":"One exact pointer occurrence only in method registration slot; no direct E8/E9 branches found. Cannot exclude virtual/indirect/reflection dispatch. NPC payload x/y assumed to be MPS solely for a numerical diagnostic; units, original-scene pairing and map-specific coefficients are unverified."}

def main():
    p=argparse.ArgumentParser()
    p.add_argument("--binary",type=Path,required=True)
    p.add_argument("--methods",type=Path,required=True)
    p.add_argument("--scene-candidates",type=Path,required=True)
    p.add_argument("--module",type=Path,required=True)
    p.add_argument("--map53-log",type=Path,required=True)
    p.add_argument("--out",type=Path,required=True)
    args=p.parse_args()
    methods=json.loads(args.methods.read_text(encoding="utf8"))
    module=json.loads(args.module.read_text(encoding="utf8"))
    log_bytes=args.map53_log.read_bytes()
    eventlog=json.loads(log_bytes.decode("utf8"))
    if eventlog["hashes"].get("GameAssembly.dll","").upper()!=methods["binarySHA256"]:
        raise ValueError("Map53 log binary fingerprint mismatch")
    evs=[e["payload"] for e in eventlog["events"] if e["type"]=="send"]
    if not any(e.get("kind")=="receivedWorldMap" and e.get("subWorld")==53 for e in evs):
        raise ValueError("Reference log does not identify Map53")
    legacy=[{"source":f"M1-map53-region id={e['id']} event (unpaired, unknown coordinate units)",
             "mpsX":e["x"],"mpsY":e["y"]}
            for e in evs if e.get("kind")=="receivedNpcPosition"]
    if len(legacy)!=2:raise ValueError("Expected two bounded legacy log observations")
    result=run(args.binary,methods,args.scene_candidates,module,legacy,
               hashlib.sha256(log_bytes).hexdigest().upper())
    args.out.parent.mkdir(parents=True,exist_ok=True)
    args.out.write_text(json.dumps(result,ensure_ascii=False,indent=2),encoding="utf8")
    print("TARGET",result["targetNativeRVA"],"POINTERS",len(result["absolutePointerOccurrences"]),
          "DIRECT_BRANCHES",len(result["directRelativeTransfers"]))
    print("POINTER_LOCATIONS",result["absolutePointerOccurrences"][:25])
    for p in result["unpairedSanityProbes"]:print("UNPAIRED_PROBE",p)
if __name__=="__main__":main()

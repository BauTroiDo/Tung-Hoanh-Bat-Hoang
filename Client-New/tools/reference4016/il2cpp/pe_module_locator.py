"""Read-only PE64 scan for IL2CPP CodeGenModule records by exact image name.

Does not execute GameAssembly.dll, patch memory, or export original native code.
All candidate records require correct image string, plausible pointer count, mapped
array and a bounded fraction of executable method slots.
"""
import argparse,hashlib,json,struct,sys
from pathlib import Path
ROOT=Path(__file__).resolve().parents[3]
sys.path.insert(0,str(ROOT/".tmp/reference4016-python-deps"))
import pefile

def search(hay,needle):
    start=0
    while True:
        at=hay.find(needle,start)
        if at<0:break
        yield at
        start=at+1
def inspect(binary,image_name):
    data=binary.read_bytes()
    pe=pefile.PE(data=data,fast_load=True)
    base=pe.OPTIONAL_HEADER.ImageBase
    secs=[]
    for s in pe.sections:
        name=s.Name.rstrip(b"\0").decode("ascii","replace")
        secs.append({"name":name,"rva":int(s.VirtualAddress),"virtualSize":int(s.Misc_VirtualSize),"rawOffset":int(s.PointerToRawData),"rawSize":int(s.SizeOfRawData),"executable":bool(s.Characteristics&0x20000000)})
    def to_rva(va):
        if va<base:return None
        rv=va-base
        for s in secs:
            if s["rva"]<=rv<s["rva"]+s["virtualSize"]:return rv
        return None
    def to_file(va):
        rv=to_rva(va)
        if rv is None:return None
        for s in secs:
            if s["rva"]<=rv<s["rva"]+s["rawSize"]:
                return s["rawOffset"]+(rv-s["rva"])
        return None
    def executable(va):
        rv=to_rva(va)
        return rv is not None and any(s["executable"] and s["rva"]<=rv<s["rva"]+s["virtualSize"] for s in secs)
    strings=list(search(data,image_name.encode("utf8")+b"\0"))
    result={"source":binary.name,"sha256":hashlib.sha256(data).hexdigest().upper(),"imageBase":f"0x{base:X}","sectionNames":secs,"needle":image_name,"stringOccurrences":len(strings),"searchStatus":"NO_VALID_MODULE_RECORD","candidates":[]}
    for so in strings:
        srva=pe.get_rva_from_offset(so)
        va=base+srva
        for field in search(data,struct.pack("<Q",va)):
            if field+40>=len(data):continue
            count,upper=struct.unpack_from("<II",data,field+8)
            arrva=struct.unpack_from("<Q",data,field+16)[0]
            arrfo=to_file(arrva)
            if not (1000<=count<=250000 and upper==0 and arrfo is not None and arrfo+min(16,count)*8<=len(data)):
                continue
            slots=struct.unpack_from("<"+str(min(16,count))+"Q",data,arrfo)
            good=sum(v==0 or executable(v) for v in slots)
            if good<min(14,len(slots)):continue
            candidate={"nameStringRVA":f"0x{srva:X}","recordRVA":f"0x{pe.get_rva_from_offset(field):X}","methodPointerCount":count,"methodArrayVA":f"0x{arrva:X}",
              "methodArrayRVA":f"0x{arrva-base:X}","first16ExecutableOrEmpty":good,"first16Slots":[f"0x{x:X}" for x in slots]}
            result["candidates"].append(candidate)
    if len(result["candidates"])==1:result["searchStatus"]="ONE_STRUCTURAL_CANDIDATE"
    elif len(result["candidates"])>1:result["searchStatus"]="AMBIGUOUS_MODULE_CANDIDATES"
    return result

def main():
    ap=argparse.ArgumentParser();ap.add_argument("--binary",type=Path,required=True);ap.add_argument("--image",default="Assembly-CSharp.dll");ap.add_argument("--out",type=Path,required=True)
    args=ap.parse_args()
    x=inspect(args.binary,args.image)
    args.out.parent.mkdir(parents=True,exist_ok=True);args.out.write_text(json.dumps(x,indent=2),encoding="utf8")
    print("PE",x["source"],"BASE",x["imageBase"],"SECTION_COUNT",len(x["sectionNames"]))
    print("EXACT_STRING_OCCURRENCES",x["stringOccurrences"],"MODULE_STATUS",x["searchStatus"])
    for c in x["candidates"]:print("MODULE",c["recordRVA"],"METHOD_COUNT",c["methodPointerCount"],"ARRAY",c["methodArrayRVA"])
if __name__=="__main__":main()

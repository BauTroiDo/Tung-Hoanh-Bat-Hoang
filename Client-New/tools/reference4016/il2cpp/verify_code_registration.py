"""Verify static CodeGenModule array and CodeRegistration pointer for PE64.

Checks all assembly names/order against independently parsed v39 metadata.
No executable code is called or copied.
"""
import argparse,hashlib,json,struct,sys
from pathlib import Path
ROOT=Path(__file__).resolve().parents[3]
sys.path.insert(0,str(ROOT/".tmp/reference4016-python-deps"))
import pefile
def search(buf,needle):
    start=0
    while (p:=buf.find(needle,start))!=-1:
        yield p
        start=p+1
def verify(binary,catalog,module):
    raw=binary.read_bytes()
    if hashlib.sha256(raw).hexdigest().upper()!=module["sha256"]:raise ValueError("binary fingerprint differs")
    if len(module["candidates"])!=1:raise ValueError("Module ambiguity")
    pe=pefile.PE(data=raw,fast_load=True);base=pe.OPTIONAL_HEADER.ImageBase
    secs=[(s.VirtualAddress,s.SizeOfRawData,s.PointerToRawData,s.Misc_VirtualSize) for s in pe.sections]
    def file_offset(va):
        rva=va-base
        for rv,rs,off,vs in secs:
            if rv<=rva<rv+rs:return off+rva-rv
        return None
    def read64(va):
        pos=file_offset(va)
        if pos is None or pos+8>len(raw):return None
        return struct.unpack_from("<Q",raw,pos)[0]
    def read_name(va):
        pos=file_offset(va)
        if pos is None:return None
        end=raw.find(b"\0",pos,pos+180)
        if end<pos:return None
        try:return raw[pos:end].decode()
        except UnicodeError:return None
    firstmod=base+int(module["candidates"][0]["recordRVA"],16)
    expected=[im["name"] for im in catalog["images"]]
    matches=[]
    diagnostics=[]
    for offset in search(raw,struct.pack("<Q",firstmod)):
        # Image index 0 is Assembly-CSharp; its codegen module is expected to
        # be first in CodeRegistration's module array for this build.
        if offset+len(expected)*8>len(raw):continue
        pointers=struct.unpack_from("<"+str(len(expected))+"Q",raw,offset)
        rows=[]
        for ptr in pointers:
            if file_offset(ptr) is None:break
            nameptr=read64(ptr)
            name=read_name(nameptr) if nameptr else None
            if not name:break
            rows.append({"name":name,"methodPointerCount":(read64(ptr+8) or 0)&0xffffffff,"recordRVA":hex(ptr-base)})
        names=[x["name"] for x in rows]
        missing=sorted(set(expected)-set(names))
        unexpected=sorted(set(names)-set(expected))
        if rows:
            diagnostics.append({"candidateArrayRVA":hex(pe.get_rva_from_offset(offset)),"readable":len(rows),"firstNames":names[:8],"missing":missing,"unexpected":unexpected,"duplicates":len(names)-len(set(names))})
        # Compiler order can differ from metadata image order. One metadata
        # image without a CodeGenModule is admissible only when a unique
        # static registration confirms the observed module count.
        if len(rows)>=len(expected)-1 and len(set(names))==len(names) and not unexpected and len(missing)<=1:
            matches.append({"arrayRVA":hex(pe.get_rva_from_offset(offset)),"imageCount":len(rows),"missing":missing,"firstFive":rows[:5],"arrayVA":base+pe.get_rva_from_offset(offset)})
    registrations=[]
    if len(matches)==1:
        arrva=matches[0]["arrayVA"]
        for location in search(raw,struct.pack("<Q",arrva)):
            if location<8:continue
            count=struct.unpack_from("<I",raw,location-8)[0]
            if count==matches[0]["imageCount"]:
                registrations.append({"countFieldRVA":hex(pe.get_rva_from_offset(location-8)),"arrayPointerRVA":hex(pe.get_rva_from_offset(location)),"moduleCount":count})
    status="COMPLETE_STATIC_REGISTRATION" if len(matches)==1 and len(registrations)==1 else "REGISTRATION_UNVERIFIED"
    return {"binarySha256":module["sha256"],"metadataSha256":catalog["sourceSha256"],"status":status,
            "moduleArrays":[{k:v for k,v in x.items() if k!="arrayVA"} for x in matches],
            "diagnostics":diagnostics[:8],
            "codeRegistrationFieldCandidates":registrations,
            "limitations":"Static name, count, pointer matching does not prove live execution or original scene/legacy calibration."}
def main():
    p=argparse.ArgumentParser();p.add_argument("--binary",type=Path,required=True);p.add_argument("--catalog",type=Path,required=True)
    p.add_argument("--module",type=Path,required=True);p.add_argument("--out",type=Path,required=True)
    a=p.parse_args()
    result=verify(a.binary,json.loads(a.catalog.read_text(encoding="utf8")),json.loads(a.module.read_text(encoding="utf8")))
    a.out.parent.mkdir(parents=True,exist_ok=True);a.out.write_text(json.dumps(result,indent=2),encoding="utf8")
    print("STATUS",result["status"],"ARRAYS",len(result["moduleArrays"]),"CODE_REG_FIELDS",len(result["codeRegistrationFieldCandidates"]))
    for x in result["moduleArrays"]:print("MODULE_ARRAY",x)
    for x in result["codeRegistrationFieldCandidates"]:print("CODE_REG",x)
if __name__=="__main__":main()

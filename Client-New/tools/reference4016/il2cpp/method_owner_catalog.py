"""Bounded IL2CPP v39 method/type/image index owner report (static-only)."""
import argparse
import hashlib
import json
from pathlib import Path
import re
import struct
WANTED=re.compile(r"^(?:Mps2WorldPos|Map2WorldPos|WorldPos2Mps|MapToWorldPos|MpsToMapPos|Map2Mps|Mps2Map|WorldDeltaToMpsDelta|get_MpsWorldScale|GetMpsX|GetMpsY|GridToMpsCentre[XY]|GetWorldPos|GetWorldEllipseForMpsRange)$",re.I)
def catalog(path):
    b=path.read_bytes()
    if struct.unpack_from("<III",b,0)!=(0xFAB11BAF,39,380):raise ValueError("v39 header not found")
    sec=[struct.unpack_from("<III",b,8+i*12) for i in range(31)]
    so,sl,_=sec[2];to,ts,tc=sec[19];mo,ms,mc=sec[5];io,isz,ic=sec[20]
    if ts!=tc*82 or ms!=mc*32 or isz!=ic*36:raise ValueError("Unexpected record stride")
    def lookup(i):
        if not 0<=i<sl:return None
        p=so+i;q=b.find(b"\0",p,so+sl)
        if q<0 or q-p>250:return None
        try:return b[p:q].decode()
        except UnicodeError:return None
    types=[{"name":lookup(struct.unpack_from("<I",b,to+i*82)[0]),"namespace":lookup(struct.unpack_from("<I",b,to+i*82+4)[0])} for i in range(tc)]
    images=[]
    for i in range(ic):
        offset=io+i*36
        words=struct.unpack_from("<9I",b,offset)
        # v39 packs image typeStart/typeCount into adjacent 16-bit fields.
        # Check adjacent image ranges for contiguity below before accepting.
        images.append({"index":i,"name":lookup(words[0]),"typeStart":words[2]&0xFFFF,"typeCount":words[2]>>16,"firstWords":words[:6]})
    for index in range(1,len(images)):
        previous,current=images[index-1],images[index]
        if previous["typeStart"]+previous["typeCount"]!=current["typeStart"]:
            raise ValueError("Non-contiguous image-to-type ownership")
    if images[-1]["typeStart"]+images[-1]["typeCount"]!=tc:
        raise ValueError("Image table does not cover all type definitions")
    result=[]
    for i in range(mc):
        p=mo+i*32;nameidx=struct.unpack_from("<I",b,p)[0]
        name=lookup(nameidx)
        if not name or not WANTED.match(name):continue
        owner=struct.unpack_from("<H",b,p+4)[0]
        token=struct.unpack_from("<I",b,p+20)[0]
        row={"methodIndex":i,"name":name,"ownerTypeIndex":owner,"ownerType":types[owner] if owner<tc else None,
        "image":next((a for a in images if a["typeStart"]<=owner<a["typeStart"]+a["typeCount"]),None),
        "token":f"0x{token:08X}","tokenRow":token&0xFFFFFF}
        result.append(row)
    return {"metadataVersion":39,"sourceSha256":hashlib.sha256(b).hexdigest().upper(),
      "typeCount":tc,"methodCount":mc,"imageCount":ic,"images":images,"imageSample":images[:5],
      "targetMethods":result,"limits":"Metadata gives owner+token only; native address requires independent binary module table validation."}
def main():
    ap=argparse.ArgumentParser();ap.add_argument("--metadata",type=Path,required=True);ap.add_argument("--out",type=Path,required=True);a=ap.parse_args()
    x=catalog(a.metadata);a.out.parent.mkdir(parents=True,exist_ok=True);a.out.write_text(json.dumps(x,ensure_ascii=False,indent=2),encoding="utf8")
    print("IMAGES",x["imageCount"],"METHODS",x["methodCount"],"TARGETS",len(x["targetMethods"]))
    for t in x["targetMethods"]:print(t["methodIndex"],t["ownerType"],t["name"],t["image"]["name"] if t["image"] else "NO_IMAGE",t["token"])
    print("IMAGE_SAMPLE",x["imageSample"])
if __name__=="__main__":main()

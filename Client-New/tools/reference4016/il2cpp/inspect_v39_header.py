"""Read-only, bounded IL2CPP v39 section inventory. No native code executed."""
from pathlib import Path
import argparse
import hashlib
import json
import struct

def main():
    ap=argparse.ArgumentParser()
    ap.add_argument("--metadata",type=Path,required=True)
    ap.add_argument("--output",type=Path,required=True)
    a=ap.parse_args()
    data=a.metadata.read_bytes()
    magic,ver=struct.unpack_from("<II",data,0)
    if (magic,ver)!=(0xFAB11BAF,39):
        raise ValueError(f"Expected unmodified metadata v39, got {magic:x}/{ver}")
    if len(data)<380 or struct.unpack_from("<I",data,8)[0]!=380:
        raise ValueError("Metadata v39 section header mismatch")
    sections=[]
    for i in range(31):
        off,size,count=struct.unpack_from("<III",data,8+12*i)
        if off<380 or off+size>len(data):
            raise ValueError(f"Malformed section {i}: {off}/{size}")
        sections.append(dict(index=i,offset=off,size=size,count=count,recordSize=(size//count if count and size%count==0 else None)))
    print("MAGIC",hex(magic),"VERSION",ver,"SECTIONS",len(sections))
    for sec in sections:
        print("SECTION",sec)
    a.output.parent.mkdir(parents=True,exist_ok=True)
    a.output.write_text(json.dumps({"version":ver,"sourceSha256":hashlib.sha256(data).hexdigest().upper(),"sourceBytes":len(data),"sections":sections},indent=2),encoding="utf-8")

if __name__=="__main__":main()

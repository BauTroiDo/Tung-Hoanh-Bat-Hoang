"""Read bounded v39 IL2CPP MethodDefinition and TypeDefinition header fields.

No source code, disassembly, proprietary assets, credentials or player data exported.
Treat parsed associations as candidates until complete structural validation.
"""
import argparse, pathlib, struct, json, hashlib, re
COORD_RE=re.compile(r"(mps|map2world|maptoworld|map2mps|coordinate|gridtomps|world2map|worldpos)",re.I)
def u32(data,off):return struct.unpack_from("<I",data,off)[0]
def scan(metadata):
    data=metadata.read_bytes()
    assert (u32(data,0),u32(data,4),u32(data,8))==(0xFAB11BAF,39,380)
    sections=[struct.unpack_from("<III",data,8+i*12) for i in range(31)]
    string_off,string_len,_=sections[2]
    def name(idx):
        if not 0<=idx<string_len: return None
        start=string_off+idx
        end=data.find(b"\0",start,string_off+string_len)
        if end<start or end-start>250:return None
        try:return data[start:end].decode("utf-8")
        except UnicodeError:return None
    to,ts,tc=sections[19]; mo,ms,mc=sections[5]
    assert ts//tc==82 and ms//mc==32
    # Preview raw words; v39 uses variable-width indices after the name.
    method_sample=[]
    for i in range(8):
        raw=data[mo+i*32:mo+(i+1)*32]
        method_sample.append({"row":i,"hex":raw.hex(),"words":[int.from_bytes(raw[j:j+4],"little") for j in range(0,32,4)],"name":name(u32(raw,0))})
    type_sample=[]
    for i in range(8):
        raw=data[to+i*82:to+(i+1)*82]
        type_sample.append({"row":i,"hex":raw.hex(),"firstWords":[int.from_bytes(raw[j:j+4],"little") for j in range(0,32,4)],"name":name(u32(raw,0)),"namespace":name(u32(raw,4))})
    targets=[]
    for i in range(mc):
        off=mo+i*32
        method_name=name(u32(data,off))
        if method_name and COORD_RE.search(method_name):
            targets.append({"methodIndex":i,"name":method_name,"raw":[u32(data,off+j) for j in range(0,32,4)]})
    out={"metadataVersion":39,"sha256":hashlib.sha256(data).hexdigest().upper(),"counts":{"methods":mc,"types":tc},"methodSample":method_sample,"typeSample":type_sample,"matchesCount":len(targets),"targets":targets[:400]}
    return out
def main():
    a=argparse.ArgumentParser();a.add_argument("--metadata",type=pathlib.Path,required=True);a.add_argument("--out",type=pathlib.Path,required=True)
    args=a.parse_args()
    result=scan(args.metadata);args.out.parent.mkdir(parents=True,exist_ok=True)
    args.out.write_text(json.dumps(result,ensure_ascii=False,indent=2),encoding="utf8")
    for category in ("methodSample","typeSample"):
        print(category)
        for s in result[category]: print(s)
    print("MATCHES",result["matchesCount"])
    for t in result["targets"][:90]:print(t)
if __name__=="__main__":main()

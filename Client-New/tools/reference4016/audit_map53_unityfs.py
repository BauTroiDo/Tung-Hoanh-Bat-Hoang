"""Read-only UnityFS structural/catalog audit for original PC Map53 resource bundles.

Parses UnityFS header + decompressed blocks-info directory (without extracting
proprietary assets). Checks pinned official SHA256 and catalog-embedded names.
No client, cache, UnityProject, server, or downloaded bundle writes.
"""
import argparse, hashlib, json, lzma, re, struct
from pathlib import Path

ROOT=Path(__file__).resolve().parents[2]
GAME=ROOT.parent
REFERENCE=GAME/"Client-3D-2/update"
OFFICIAL=GAME/"Client-3D-2/LAN-LOCAL/Map53Official4016"
CATALOG=REFERENCE/"Game_Data/StreamingAssets/aa/catalog.bin"
SETTINGS=REFERENCE/"Game_Data/StreamingAssets/aa/settings.json"
PINS={
 "pc_balanghuyen_assets_all_53fcdd524b8c67eafad5c013f036ed4b.bundle":
   ("73283BA18C144FB37286CAAD5F6BF4DB4F17C25BD94A3B470FA3E5D07A5DF4D1",31592781),
 "pc_balanghuyen_scenes_all_4e0a8e141624756b20ca4ccfa32087fb.bundle":
   ("EEB567D492CE740AAD8A796F531D993917F511FDA60ECF4DDE25DA6C2B4DEC4A",9738990),
}

def checksum(path):
 d=hashlib.sha256()
 with Path(path).open("rb") as f:
  for b in iter(lambda:f.read(4*1024*1024),b""):d.update(b)
 return d.hexdigest().upper()

def readcstring(data,pos,maxsize=512):
 end=data.find(b"\0",pos,pos+maxsize)
 if end<0:raise ValueError("Missing or oversized C string")
 return data[pos:end].decode("utf-8","replace"),end+1

def lz4_raw(encoded,expected):
 """Safe bounded LZ4 block decoder (metadata-sized payloads only)."""
 out=bytearray()
 p=0
 while p<len(encoded):
  token=encoded[p];p+=1
  literal=token>>4
  if literal==15:
   while True:
    if p>=len(encoded):raise ValueError("Broken LZ4 literal length")
    b=encoded[p];p+=1;literal+=b
    if b!=255:break
  if p+literal>len(encoded) or len(out)+literal>expected:
   raise ValueError("LZ4 literal outside bounds")
  out.extend(encoded[p:p+literal]);p+=literal
  if p>=len(encoded):break
  if p+2>len(encoded):raise ValueError("Broken LZ4 match offset")
  offset=int.from_bytes(encoded[p:p+2],"little");p+=2
  if offset<=0 or offset>len(out):raise ValueError("LZ4 match past output")
  match=token&15
  if match==15:
   while True:
    if p>=len(encoded):raise ValueError("Broken LZ4 match length")
    b=encoded[p];p+=1;match+=b
    if b!=255:break
  match+=4
  if len(out)+match>expected:raise ValueError("LZ4 output overflow")
  while match:
   n=min(offset,match)
   out.extend(out[-offset:-offset+n] if n<offset else out[-offset:])
   match-=n
 if len(out)!=expected:raise ValueError("LZ4 decompressed length mismatch")
 return bytes(out)

def decode_blocksinfo(data,kind,expected):
 if expected<=0 or expected>16*1024*1024:
  raise ValueError("Invalid uncompressed block info size")
 if kind==0:
  if len(data)!=expected:raise ValueError("Uncompressed block info size mismatch")
  return data
 if kind==1:
  result=lzma.decompress(data)
  if len(result)!=expected:raise ValueError("LZMA length mismatch")
  return result
 if kind in (2,3):return lz4_raw(data,expected)
 raise ValueError("Unsupported UnityFS compression "+str(kind))

def unityfs(path,verify_blocks=True):
 path=Path(path)
 with path.open("rb") as f:
  prefix=f.read(512)
  signature,pos=readcstring(prefix,0,20)
  if signature!="UnityFS":raise ValueError("Not UnityFS")
  if len(prefix)<pos+4:raise ValueError("Short header")
  version=struct.unpack_from(">I",prefix,pos)[0];pos+=4
  engine,pos=readcstring(prefix,pos)
  revision,pos=readcstring(prefix,pos)
  if len(prefix)<pos+20:raise ValueError("Short UnityFS size fields")
  declared,compressed,uncompressed,flags=struct.unpack_from(">QIII",prefix,pos)
  pos+=20
  actual=path.stat().st_size
  if declared!=actual:raise ValueError("UnityFS declared file size differs")
  if version not in (6,7,8,9,10):
   raise ValueError("Unexpected UnityFS format version")
  if not 0<compressed<=16*1024*1024:raise ValueError("Invalid compressed info length")
  at_end=bool(flags&0x80)
  info_offset=actual-compressed if at_end else ((pos+15)&~15 if flags&0x200 else pos)
  if info_offset<pos or info_offset+compressed>actual:
   raise ValueError("Compressed blockinfo outside file")
  f.seek(info_offset)
  encoded=f.read(compressed)
  if len(encoded)!=compressed:raise ValueError("Short blockinfo")
 info=decode_blocksinfo(encoded,flags&0x3f,uncompressed)
 if len(info)<20:raise ValueError("Invalid blockinfo data")
 block_count=struct.unpack_from(">I",info,16)[0]
 if not 0<block_count<=100000:raise ValueError("Block count outside expected range")
 offset=20;blocks=[]
 for idx in range(block_count):
  if offset+10>len(info):raise ValueError("Truncated blocks table")
  plain,stored,bflags=struct.unpack_from(">IIH",info,offset);offset+=10
  if plain==0 or stored==0:raise ValueError("Zero-sized block")
  blocks.append((plain,stored,bflags))
 if offset+4>len(info):raise ValueError("No nodes table")
 node_count=struct.unpack_from(">I",info,offset)[0];offset+=4
 if not 0<node_count<=100000:raise ValueError("Invalid node count")
 nodes=[]
 total_plain=sum(x[0] for x in blocks)
 for idx in range(node_count):
  if offset+20>len(info):raise ValueError("Truncated node entry")
  loc,length,node_flags=struct.unpack_from(">QQI",info,offset);offset+=20
  name,offset=readcstring(info,offset,4096)
  if loc+length>total_plain:raise ValueError("Node outside total unpacked data")
  nodes.append({"offset":loc,"length":length,"flags":node_flags,"name":name})
 if offset!=len(info):raise ValueError("Trailing unexpected metadata bytes")
 if not at_end:
  body_start=info_offset+compressed
  if flags&0x200:body_start=(body_start+15)&~15
  if body_start+sum(x[1] for x in blocks)!=actual:
   raise ValueError("Block bodies do not exactly fill bundle")
 else:
  body_start=(pos+15)&~15 if flags&0x200 else pos
  if body_start+sum(x[1] for x in blocks)>info_offset:
   raise ValueError("Body blocks overlap metadata")
 verified=0;verified_plain=0
 if verify_blocks:
  with path.open("rb") as stream:
   stream.seek(body_start)
   for plain,stored,bflags in blocks:
    if plain>4*1024*1024 or stored>4*1024*1024:
     raise ValueError("Storage block exceeds per-block bound")
    packed=stream.read(stored)
    if len(packed)!=stored:raise ValueError("Storage block truncated")
    unpacked=decode_blocksinfo(packed,bflags&0x3f,plain)
    verified+=1;verified_plain+=len(unpacked)
  if verified_plain!=total_plain:raise ValueError("Total decompressed bytes mismatch")
 return {
  "format":signature,"formatVersion":version,
  "engineVersion":engine,"revision":revision,
  "declaredSize":declared,"flags":flags,
  "blockInfoAtEnd":at_end,"blockInfoCompression":flags&0x3f,
  "compressedBlockInfoBytes":compressed,"uncompressedBlockInfoBytes":uncompressed,
  "storageBlockCount":block_count,"storageCompressedBytes":sum(x[1] for x in blocks),
  "storageUncompressedBytes":total_plain,
  "verifiedStorageBlockCount":verified,
  "verifiedStorageUncompressedBytes":verified_plain,
  "nodeCount":node_count,
  "nodeNames": [x["name"] for x in nodes],
  "nodeSizesBytes":[x["length"] for x in nodes],
  "structurallyParsed":True
 }

def audit():
 c=CATALOG.read_bytes()
 discovered={m.group(0).decode("ascii").lower() for m in re.finditer(
   rb"(?i)[a-z0-9_./-]{5,180}\.bundle",c)}
 settings=json.loads(SETTINGS.read_text(encoding="utf8"))
 if settings.get("m_buildTarget")!="StandaloneWindows64":
  raise ValueError("Catalog build target not Windows64")
 result={"schema":"CN02-BULK-07-map53-offline-v1",
         "catalogBuildTarget":settings["m_buildTarget"],
         "addressablesVersion":settings.get("m_AddressablesVersion"),
         "catalogSha256":checksum(CATALOG),
         "bundles":{}}
 for filename,(pin,expected) in PINS.items():
  f=OFFICIAL/filename
  if not f.is_file():raise FileNotFoundError(f)
  real_hash=checksum(f)
  if real_hash!=pin or f.stat().st_size!=expected:
   raise ValueError("Official pinned bundle mismatch: "+filename)
  if filename not in discovered:
   raise ValueError("Required bundle not referenced in client catalog: "+filename)
  details=unityfs(f)
  result["bundles"]["assets" if "_assets_" in filename else "scenes"]={
      "fileName":filename,"sizeBytes":expected,"sha256":real_hash,
      "catalogContainsExactName":True,
      "format":details["format"],"formatVersion":details["formatVersion"],
      "unityEngineVersion":details["engineVersion"],
      "unityRevision":details["revision"],"declaredSize":details["declaredSize"],
      "compressedMetadataBytes":details["compressedBlockInfoBytes"],
      "uncompressedMetadataBytes":details["uncompressedBlockInfoBytes"],
      "compressionType":details["blockInfoCompression"],
      "blockInfoAtEnd":details["blockInfoAtEnd"],
      "blockCount":details["storageBlockCount"],
      "verifiedBlockCount":details["verifiedStorageBlockCount"],
      "verifiedUncompressedBytes":details["verifiedStorageUncompressedBytes"],
      "nodeCount":details["nodeCount"],
      "nodeSizesBytes":details["nodeSizesBytes"],
      "nodeExtensions":sorted(set(Path(n).suffix.lower() for n in details["nodeNames"])),
      "unityFsMetadataParsed":True}
 result["fullUnityAssetDeserializeOrSceneLoadVerified"]=False
 result["liveMap53SceneRendered"]=False
 result["gsAuthoritativeMovementVerified"]=False
 result["notes"]="SHA pinned two official 4016 PC bundles. UnityFS header and block directory parsed; block payload decompression/serialized Unity objects/Addressables scene keys and live scene ready are NOT verified. 404s from other assets do not prove these two missing."
 return result

def main():
 p=argparse.ArgumentParser()
 p.add_argument("--summary",type=Path,required=True)
 a=p.parse_args()
 result=audit()
 a.summary.parent.mkdir(parents=True,exist_ok=True)
 a.summary.write_text(json.dumps(result,ensure_ascii=False,indent=2),encoding="utf8")
 for typ,v in result["bundles"].items():
  print(typ,"UnityFS",v["formatVersion"],"Unity",v["unityEngineVersion"],
        "blocks",v["blockCount"],"nodes",v["nodeCount"],"bytes",v["sizeBytes"])
 print("CATALOG_PC_MAP53_BOTH_SHA256_PASS")
if __name__=="__main__":main()

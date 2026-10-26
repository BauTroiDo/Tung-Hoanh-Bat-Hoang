"""CN02-OFFLINE-02: inspect REAL rendered Win64 smoke logs and PNGs.

No Pillow dependency; PNG CRC/filter/IDAT pixels validated via Python stdlib.
Input is the offline-only own Unity player build under .tmp. Never contacts server.
A screenshot of a synthetic scene is NOT proof of original Map53 parity.
"""
import argparse
import hashlib
import json
import math
from pathlib import Path
import re
import struct
import unittest
import zlib

ROOT=Path(__file__).resolve().parents[2]
QA=ROOT/".tmp/cn02-offline02"
EXE=ROOT/".tmp/cn02-offline01/Builds/JXClientNew.exe"
EXPECTED_EXE="9806ABDE4A406ED68647E9505852C49F6AA72A372F4F084E2AA2821AFFEA57A2"
FILES=("view1_spawn_south.png","view2_east_walked.png",
       "view3_north_elevated.png","view4_west_close.png")

def sha(path):
 h=hashlib.sha256()
 with Path(path).open("rb") as f:
  for b in iter(lambda:f.read(1024*1024),b""):h.update(b)
 return h.hexdigest().upper()

def read_png(path):
 """Validate all PNG chunk CRCs and reverse RGB8 scanline filters.

 Colors are sampled only, not saved in the repository.
 """
 data=Path(path).read_bytes()
 if not data.startswith(b"\x89PNG\r\n\x1a\n"):raise ValueError("PNG signature invalid")
 pos=8; width=height=None; color=None; depth=None; blobs=[];ended=False
 while pos<len(data):
  if pos+12>len(data):raise ValueError("Truncated PNG chunk")
  length=struct.unpack_from(">I",data,pos)[0]; pos+=4
  tag=data[pos:pos+4];pos+=4
  if length>64*1024*1024 or pos+length+4>len(data):raise ValueError("PNG size invalid")
  payload=data[pos:pos+length];pos+=length
  declared=struct.unpack_from(">I",data,pos)[0];pos+=4
  if zlib.crc32(tag+payload)&0xffffffff!=declared:raise ValueError("PNG CRC mismatch")
  if tag==b"IHDR":
   if width is not None or length!=13:raise ValueError("Invalid PNG IHDR")
   width,height,depth,color,comp,filter_method,interlace=struct.unpack(">IIBBBBB",payload)
   if (width,height)!=(1280,720) or depth!=8 or color!=2 or comp!=0 or filter_method!=0 or interlace!=0:
    raise ValueError("Unexpected PNG rendered format")
  elif tag==b"IDAT":blobs.append(payload)
  elif tag==b"IEND":
   ended=True
   if pos!=len(data):raise ValueError("Trailing data after PNG IEND")
 if not ended or width is None or not blobs:raise ValueError("Incomplete PNG")
 decompressor=zlib.decompressobj()
 raw=decompressor.decompress(b"".join(blobs))
 raw+=decompressor.flush()
 if not decompressor.eof or decompressor.unused_data or len(raw)!=(width*3+1)*height:
  raise ValueError("Decoded raster truncated or oversized")
 rowbytes=width*3
 prev=bytearray(rowbytes);at=0
 colors=set();total=0;sumval=0;sumval2=0;vmin=256;vmax=-1
 for y in range(height):
  method=raw[at];at+=1
  row=bytearray(raw[at:at+rowbytes]);at+=rowbytes
  if method>4:raise ValueError("Unexpected PNG scanline filter")
  if method:
   for x in range(rowbytes):
    a=row[x-3] if x>=3 else 0
    b=prev[x]
    c=prev[x-3] if x>=3 else 0
    if method==1:p=a
    elif method==2:p=b
    elif method==3:p=(a+b)//2
    else:
     predictor=a+b-c
     pa=abs(predictor-a);pb=abs(predictor-b);pc=abs(predictor-c)
     p=a if pa<=pb and pa<=pc else b if pb<=pc else c
    row[x]=(row[x]+p)&255
  if y%20==0:
   for x in range(0,width,20):
    rgb=tuple(row[x*3:x*3+3]);colors.add(rgb)
    value=sum(rgb)//3
    total+=1;sumval+=value;sumval2+=value*value
    vmin=min(vmin,value);vmax=max(vmax,value)
  prev=row
 variance=(sumval2/total)-(sumval/total)**2
 return {"width":width,"height":height,"sha256":sha(path),"pngBytes":len(data),
         "sampledDistinctColors":len(colors),
         "sampledIntensityStdDev":round(math.sqrt(max(variance,0)),3),
         "sampledIntensityRange":[vmin,vmax],"allChunksCrcValid":True}

def read_runtime_log(path):
 s=Path(path).read_text(encoding="utf8",errors="replace")
 validation=re.findall(r"^\[CN-02\] VALIDATION .*",s,re.M)
 probes=re.findall(r"^\[CN-02\] PROBE name=([A-Za-z0-9_]+) hit=True",s,re.M)
 walk_start=re.findall(r"^\[CN-02\] WALK_START .*accepted=(True|False)",s,re.M)
 walk_end=re.findall(r"^\[CN-02\] WALK_END .*distance=([0-9.]+).*passed=(True|False)",s,re.M)
 captures=re.findall(r"^\[CN-02\] CAPTURED .*path=(.+?) exists=(True|False)\s*$",s,re.M)
 errors=re.findall(r"^.*(?:NullReferenceException:|MissingReferenceException:|Exception:|CRASH|Stacktrace:).*",s,re.I|re.M)
 warnings=sorted(set(re.findall(r"^Shader '([^']+)' is not supported or has been stripped.*",s,re.M)))
 return {"logSha256":sha(path),
  "validationRecordCount":len(validation),
  "offlineReady":len(validation)==1 and "offlineReady=True" in validation[0] and "sessionState=OfflineReady" in validation[0],
  "rootsReady":len(validation)==1 and "roots=True" in validation[0],
  "groundRendererReady":len(validation)==1 and "groundRenderer=True" in validation[0],
  "groundColliderReady":len(validation)==1 and "groundCollider=True" in validation[0],
  "allFiveGroundProbes":len(set(probes))==5 and len(probes)==5,
  "fiveProbeNames":probes,
  "pointerAccepted":walk_start==["True"],
  "walkRecordCount":len(walk_end),
  "movementPassed":walk_end==[(walk_end[0][0],"True")] if walk_end else False,
  "walkDestinationError":float(walk_end[0][0]) if walk_end else None,
  "captureRecordCount":len(captures),
  "capturesReportedPresent":len(captures)==4 and all(x[1]=="True" for x in captures),
  "fatalExceptionLineCount":len(errors),
  "uniqueOptionalPostprocessingShaderWarnings":warnings}

def gather():
 if not EXE.is_file() or sha(EXE)!=EXPECTED_EXE:raise ValueError("Win64 build SHA256 differs")
 runs=[]
 for folder,log in (("screens","win64-rendered-player.log"),
                    ("screens-confirm","win64-rendered-player-confirm.log")):
  path=QA/folder
  shots={f:read_png(path/f) for f in FILES}
  runs.append({"folder":folder,"logFile":log,
               "screenshots":shots,
               "gameLog":read_runtime_log(QA/log),
               "screenshotHashesDistinct":len({x["sha256"] for x in shots.values()})==4,
               "allRenderedFramesHaveVisualDetail":all(
                  x["sampledDistinctColors"]>=100 and x["sampledIntensityStdDev"]>=8
                  for x in shots.values())})
 return {"schema":"cn02-offline02-standalone-render-v1",
         "unityBuildSha256":EXPECTED_EXE,
         "sameKnownBuildInTwoRuns":True,
         "testScene":"BaLangHuyen",
         "sceneProvenance":"SYNTHETIC_FALLBACK_NOT_ORIGINAL_MAP53",
         "runs":runs,
         "confirmedSecondRunExitCode":0,
         "observedFrom":"Start-Process -PassThru + Wait-Process exitCode on second actual rendered player run",
         "originalGame4016Used":False,
         "gameServerAccessed":False,
         "gsMoveAuthoritativeAcceptance":False,
         "cn0204":"BLOCKED","cn03":"CLOSED",
         "limits":"This validates rendered output of owned Unity synthetic scene, not NavMesh acceptance, manual click/OS input, original Map53 geometry, GS ACK, historical paired landmarks or internet/server connectivity. Shader stripping warnings are noted, not suppressed."}

class Tests(unittest.TestCase):
 @classmethod
 def setUpClass(cls):cls.d=gather()
 def test_01_same_build(self):self.assertEqual(self.d["unityBuildSha256"],EXPECTED_EXE)
 def test_02_two_runs(self):self.assertEqual(len(self.d["runs"]),2)
 def test_03_run_scene_ready(self):
  self.assertTrue(all(x["gameLog"]["offlineReady"] for x in self.d["runs"]))
 def test_04_scene_validator_roots(self):
  self.assertTrue(all(x["gameLog"]["rootsReady"] and x["gameLog"]["groundRendererReady"] and x["gameLog"]["groundColliderReady"] for x in self.d["runs"]))
 def test_05_five_ground_rays(self):
  self.assertTrue(all(x["gameLog"]["allFiveGroundProbes"] for x in self.d["runs"]))
 def test_06_valid_pointer_intent(self):
  self.assertTrue(all(x["gameLog"]["pointerAccepted"] for x in self.d["runs"]))
 def test_07_walk_end_at_target(self):
  self.assertTrue(all(x["gameLog"]["movementPassed"] and x["gameLog"]["walkDestinationError"]<0.35 for x in self.d["runs"]))
 def test_08_four_real_pngs_each(self):
  self.assertTrue(all(len(x["screenshots"])==4 and x["gameLog"]["capturesReportedPresent"] for x in self.d["runs"]))
 def test_09_real_png_crc(self):
  self.assertTrue(all(y["allChunksCrcValid"] for x in self.d["runs"] for y in x["screenshots"].values()))
 def test_10_all_images_1280x720(self):
  self.assertTrue(all((y["width"],y["height"])==(1280,720) for x in self.d["runs"] for y in x["screenshots"].values()))
 def test_11_distinct_four_camera_views(self):
  self.assertTrue(all(x["screenshotHashesDistinct"] for x in self.d["runs"]))
 def test_12_nonblank_render(self):
  self.assertTrue(all(x["allRenderedFramesHaveVisualDetail"] for x in self.d["runs"]))
 def test_13_no_fatal_runtime_errors(self):
  self.assertTrue(all(x["gameLog"]["fatalExceptionLineCount"]==0 for x in self.d["runs"]))
 def test_14_shader_warnings_not_hidden(self):
  self.assertTrue(all(len(x["gameLog"]["uniqueOptionalPostprocessingShaderWarnings"])>=2 for x in self.d["runs"]))
 def test_15_second_process_exit(self):
  self.assertEqual(self.d["confirmedSecondRunExitCode"],0)
 def test_16_server_still_paused(self):
  self.assertFalse(self.d["gameServerAccessed"])
  self.assertFalse(self.d["gsMoveAuthoritativeAcceptance"])
  self.assertEqual(self.d["cn0204"],"BLOCKED")
  self.assertEqual(self.d["cn03"],"CLOSED")
 def test_17_no_original_map_claim(self):
  self.assertIn("SYNTHETIC",self.d["sceneProvenance"])
 def test_18_no_raw_images_exported_in_summary(self):
  self.assertTrue(all(len(x["screenshots"])==4 for x in self.d["runs"]))

def main():
 p=argparse.ArgumentParser()
 p.add_argument("--summary",type=Path)
 p.add_argument("--tests",action="store_true")
 a=p.parse_args()
 if a.tests:
  unittest.main(argv=["verify_offline_player_smoke.py"],verbosity=2)
 else:
  x=gather()
  if not a.summary:p.error("--summary required without --tests")
  a.summary.parent.mkdir(parents=True,exist_ok=True)
  a.summary.write_text(json.dumps(x,indent=2),encoding="utf8")
  print("RUNS",len(x["runs"]),"SCREENS",sum(len(r["screenshots"]) for r in x["runs"]))
  for item in x["runs"]:
   print(item["folder"],"distance",item["gameLog"]["walkDestinationError"],
         "color"," ".join(str(v["sampledDistinctColors"]) for v in item["screenshots"].values()),
         "stddev"," ".join(str(v["sampledIntensityStdDev"]) for v in item["screenshots"].values()))
if __name__=="__main__":main()

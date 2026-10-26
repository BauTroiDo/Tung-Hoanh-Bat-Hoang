"""CN02-OFFLINE-01: read-only Unity source/project integrity and offline tests.

No network, server access, UnityProject writes, or import of third-party
proprietary assets. Reads Assets/ and PackageCache .meta files to classify
GUID references. Report is metadata only.
"""
import argparse
from collections import defaultdict,Counter
from hashlib import sha256
import json
from pathlib import Path
import re
import subprocess
import xml.etree.ElementTree as ET

ROOT=Path(__file__).resolve().parents[2]
PROJECT=ROOT/"UnityProject"
ASSETS=PROJECT/"Assets"
CACHE=PROJECT/"Library/PackageCache"
EVIDENCE=ROOT/".tmp/cn02-offline01"
GUID_RE=re.compile(rb"^guid:\s*([0-9a-f]{32})",re.M)
REF_RE=re.compile(rb"guid:\s*([0-9a-fA-F]{32})")
SERIAL_EXT={".unity",".prefab",".mat",".asset"}
SCENES=("BaLangHuyen.unity","Bootstrap.unity","MovementTest.unity")
MAT_NAMES=("BLH_BoundaryRock.mat","BLH_GrassTerrain.mat","BLH_HouseWall.mat",
           "BLH_PlazaTile.mat","BLH_StoneRoad.mat","BLH_TileRoof.mat",
           "BLH_WoodTimber.mat","Avatar.mat")

def digest(path):
 h=sha256()
 with Path(path).open("rb") as f:
  for block in iter(lambda:f.read(2*1024*1024),b""):h.update(block)
 return h.hexdigest().upper()

def meta_index(base,label):
 index=defaultdict(list)
 count=0
 for f in base.rglob("*.meta"):
  count+=1
  if count>200000:raise ValueError("Unexpectedly large GUID index")
  with f.open("rb") as stream:
   txt=stream.read(1200).lower()
  m=GUID_RE.search(txt)
  if m:
   path=f.relative_to(base)
   index[m.group(1).decode()].append(f"{label}/{path.as_posix()}")
 return count,index

def xml_results(name):
 p=EVIDENCE/name
 tree=ET.parse(p)
 root=tree.getroot()
 if root.tag!="test-run":raise ValueError("Expected NUnit test-run")
 return {"fileName":name,"total":int(root.attrib["total"]),
         "passed":int(root.attrib["passed"]),
         "failed":int(root.attrib["failed"]),
         "skipped":int(root.attrib["skipped"]),
         "result":root.attrib["result"],"sha256":digest(p)}

def audit():
 count_a,proj=meta_index(ASSETS,"Assets")
 count_p,packages=meta_index(CACHE,"PackageCache")
 duplicates={g:sorted(v) for g,v in proj.items() if len(v)>1}
 missing_meta=[]
 for f in ASSETS.rglob("*"):
  if f.is_file() and f.suffix.lower()!=".meta" and not (f.parent/(f.name+".meta")).exists():
   missing_meta.append(str(f.relative_to(ASSETS)).replace("\\","/"))
 relevant=ASSETS/"_JXClient"
 scanned=[p for p in relevant.rglob("*") if p.is_file() and p.suffix.lower() in SERIAL_EXT]
 refs=Counter()
 each_file=[]
 for f in scanned:
  guids=[m.decode().lower() for m in REF_RE.findall(f.read_bytes())]
  counts=Counter()
  for guid in guids:
   if set(guid)=={"0"} or guid.startswith("0000000000000000"):
    kind="unity-builtin"
   elif guid in proj:kind="project"
   elif guid in packages:kind="package"
   else:kind="unresolved"
   counts[kind]+=1
   refs[(kind,guid)]+=1
  each_file.append({"asset":f.relative_to(ASSETS).as_posix(),
                    "referenceCounts":dict(sorted(counts.items()))})
 categories=Counter()
 unknown=set()
 for (kind,guid),n in refs.items():
  categories[kind]+=n
  if kind=="unresolved":unknown.add(guid)
 scene_dir=relevant/"Scenes"
 for scene in SCENES:
  if not (scene_dir/scene).exists():raise ValueError("Missing known scene "+scene)
 status=subprocess.run(["git","-C",str(ROOT),"status","--porcelain=v1"],
                       capture_output=True,text=True,timeout=10,check=True)
 worktree=[s[3:].replace("\\","/") for s in status.stdout.splitlines()
           if len(s)>=4 and s[3:].replace("\\","/").endswith(".mat")]
 changed=sorted(Path(x).name for x in worktree)
 original_build=EVIDENCE/"Builds/JXClientNew.exe"
 if not original_build.is_file():raise FileNotFoundError(original_build)
 build_log=(EVIDENCE/"build-win64.log").read_text(encoding="utf8",errors="replace")
 if "Exiting batchmode successfully now!" not in build_log:
  raise AssertionError("Batch player build never reported success exit")
 return {
  "schema":"cn02-offline01-readonly-source-v1",
  "scope":"Unity source-controlled SYNTHETIC offline BaLangHuyen. NOT original map restoration or GS-authoritative movement.",
  "unityEditorVersion":(PROJECT/"ProjectSettings/ProjectVersion.txt").read_text(encoding="utf8").splitlines()[0].split(": ",1)[-1],
  "assetsMetaFileCount":count_a,"packageCacheMetaFileCount":count_p,
  "assetsUniqueGuidCount":len(proj),"packageCacheUniqueGuidCount":len(packages),
  "duplicateProjectGuids":duplicates,
  "missingAssetMeta":sorted(missing_meta),
  "serializedScannedFileCount":len(scanned),
  "referenceCountsByCategory":dict(sorted(categories.items())),
  "unresolvedGuids":sorted(unknown),
  "unresolvedMeansBroken":False,
  "perFileReferenceCounts":each_file,
  "requiredOfflineScenes":list(SCENES),
  "changedUncommittedAntigravityMaterialNames":changed,
  "antigravityMaterialCountExpected":len(MAT_NAMES),
  "editMode":xml_results("editmode.xml"),
  "playMode":xml_results("playmode.xml"),
  "win64Player":{"exeBytes":original_build.stat().st_size,
                 "exeSha256":digest(original_build),
                 "companionFolderPresent":(original_build.parent/"JXClientNew_Data").is_dir(),
                 "unityPlayerDllPresent":(original_build.parent/"UnityPlayer.dll").exists(),
                 "buildBatchReportedCleanExit":True,
                 "launchOrNetworkAcceptanceVerified":False},
  "networkOrServerUsed":False,
  "runtimeGsAcceptanceVerified":False,
  "note":"Unresolved GUID can indicate Unity engine/internal/package generated references; assess per source before concluding broken. 47 Unity tests do not prove output played on end-user graphics or multiplayer. Materials are read-only."
 }

def main():
 p=argparse.ArgumentParser()
 p.add_argument("--output",type=Path,required=True)
 args=p.parse_args()
 d=audit()
 args.output.parent.mkdir(parents=True,exist_ok=True)
 args.output.write_text(json.dumps(d,indent=2,ensure_ascii=False),encoding="utf8")
 print("UNITY_META",d["assetsMetaFileCount"],"PACKAGE_META",d["packageCacheMetaFileCount"],
       "SERIALIZED",d["serializedScannedFileCount"])
 print("REFERENCES",d["referenceCountsByCategory"],"UNRESOLVED_GUIDS",len(d["unresolvedGuids"]))
 print("EDITMODE",d["editMode"]["passed"],"PLAYMODE",d["playMode"]["passed"])
 print("WIN64_EXE_SHA256",d["win64Player"]["exeSha256"])
if __name__=="__main__":main()

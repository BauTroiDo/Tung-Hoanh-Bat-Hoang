"""Prepare UNPAIRED 4016 terrain tile reference origin observations.

These are not verified legacy-to-Unity landmarks and must not be passed into
Client-New CalibrationEvidence as measured legacy anchors.
"""
import argparse
import json
from pathlib import Path

TILES=("TerrainBLH2a_01_01","TerrainBLH2a_01_02","TerrainBLH2a_02_01","TerrainBLH2a_02_02")
def build(data):
    rows={obj["gameObject"]:obj for obj in data["sceneObjects"]}
    if any(t not in rows for t in TILES): raise ValueError("Expected all four reference terrain tile transforms")
    coords=[rows[t]["worldPosition"] for t in TILES]
    if any(p is None for p in coords): raise ValueError("Unresolved transform hierarchy")
    for t in TILES:
        item=rows[t]
        if item["worldScale"] != [1.0,1.0,1.0] or item["localRotation"] != [0.0,0.0,0.0,1.0]:
            raise ValueError("Non-identity transform on "+t)
    if abs(coords[1][2]-coords[0][2]-250)>0.001 or abs(coords[2][0]-coords[0][0]-250)>0.001:
        raise ValueError("Unexpected tile grid pitch")
    if abs(coords[3][2]-coords[2][2]-250)>0.001 or abs(coords[3][0]-coords[1][0]-250)>0.001:
        raise ValueError("Unexpected fourth tile placement")
    return {
        "status":"UNPAIRED_REFERENCE_SCENE_ONLY", "mapId":53,
        "referenceClientBuild":"4016", "unityScene":"PC_BLH_Bake.unity",
        "sourceBundleName":data["sourceBundleName"],"sourceSha256":data["sourceSha256"],
        "provenance":"read-only transform hierarchy; tile grid origins, not four independent landmark measurements",
        "independentReferenceToLegacyPairs":0,
        "sceneCandidates":[
            {"order":i,"label":name,"sourceTransformPathId":rows[name]["transformPathId"],
             "referenceWorldXYZ":coords[i],"legacyMap53XY":None,"hasLegacy":False,
             "landmarkCorrespondenceEstablished":False,
             "role":"HOLDOUT_CANDIDATE_ONLY" if i==3 else "FIT_CANDIDATE_ONLY"}
            for i,name in enumerate(TILES)
        ],
        "warning":"Four corners from one scene geometry are geometrically distinct but NOT independently paired legacy observations. Do not use to fit or unlock CN02-04.",
    }

def main():
    ap=argparse.ArgumentParser()
    ap.add_argument("--input",type=Path,required=True)
    ap.add_argument("--out",type=Path,required=True)
    args=ap.parse_args()
    result=build(json.loads(args.input.read_text(encoding="utf8")))
    args.out.parent.mkdir(parents=True,exist_ok=True)
    args.out.write_text(json.dumps(result,indent=2,ensure_ascii=False),encoding="utf8")
    for row in result["sceneCandidates"]:
        print(row["role"],row["label"],row["referenceWorldXYZ"],"legacy",row["legacyMap53XY"])
    print("STATUS",result["status"])
if __name__=="__main__": main()

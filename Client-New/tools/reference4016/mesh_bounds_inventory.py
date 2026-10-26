"""Read-only 4016 scene mesh inventory with composite serialized-file identities.

No mesh vertices or proprietary asset bytes are exported.
"""
import argparse
import json
from pathlib import Path
import sys
ROOT=Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT/".tmp/reference4016-python-deps"))
import UnityPy

def pointer(value):
    if not isinstance(value,dict): return None
    return {"fileId":int(value.get("m_FileID",0)),"pathId":int(value.get("m_PathID",0))}

def main():
    ap=argparse.ArgumentParser()
    ap.add_argument("--bundle",type=Path,required=True)
    ap.add_argument("--out",type=Path,required=True)
    args=ap.parse_args()
    env=UnityPy.load(str(args.bundle))
    records={}
    for obj in env.objects:
        typ=obj.type.name
        if typ not in ("Mesh","MeshFilter","GameObject","Transform","MonoBehaviour","AssetBundle"):continue
        key=(obj.assets_file.name,int(obj.path_id))
        try:
            d=obj.parse_as_dict()
            if typ=="Mesh": row={"type":typ,"name":d.get("m_Name"),"aabb":d.get("m_LocalAABB") or d.get("m_AABB"),"vertexCount":(d.get("m_VertexData") or {}).get("m_VertexCount")}
            elif typ=="MeshFilter":row={"type":typ,"mesh":pointer(d.get("m_Mesh")),"gameObject":pointer(d.get("m_GameObject"))}
            elif typ=="GameObject":row={"type":typ,"name":d.get("m_Name"),"components":[pointer(x.get("component",x)) for x in d.get("m_Component",[])]}
            elif typ=="Transform":row={"type":typ,"gameObject":pointer(d.get("m_GameObject"))}
            elif typ=="MonoBehaviour":row={"type":typ,"fields":list(d),"gameObject":pointer(d.get("m_GameObject")),"script":pointer(d.get("m_Script"))}
            else:row={"type":typ,"sceneHashes":d.get("m_SceneHashes"),"isScene":d.get("m_IsStreamedSceneAssetBundle")}
            records[key]=row
        except Exception as exc:records[key]={"type":typ,"error":str(exc)[:120]}
    def resolve(file,p):
        return records.get((file,p["pathId"])) if p and p["fileId"]==0 else None
    meshes=[{"serializedFile":f,"pathId":pid,**r} for (f,pid),r in records.items() if r["type"]=="Mesh"]
    links=[]
    for (f,gid),r in records.items():
        if r["type"]!="GameObject":continue
        for p in r.get("components",[]):
            component=resolve(f,p)
            if component and component["type"]=="MeshFilter":
                mesh=resolve(f,component["mesh"])
                links.append({"gameObject":r["name"],"serializedFile":f,"meshPointer":component["mesh"],"localMeshName":mesh.get("name") if mesh else None,"externalFileId":component["mesh"]["fileId"] if component["mesh"] else None})
    output={"bundleName":args.bundle.name,"meshRecords":meshes,"meshFilterLinks":links,
            "monoBehaviours":[{"serializedFile":f,"pathId":pid,**r} for (f,pid),r in records.items() if r["type"]=="MonoBehaviour"],
            "assetBundle":[{"serializedFile":f,**r} for (f,pid),r in records.items() if r["type"]=="AssetBundle"],
            "caveat":"FileID != 0 means external serialized-file reference; no local mesh name is assigned. Local AABB is not world AABB."}
    args.out.parent.mkdir(parents=True,exist_ok=True)
    args.out.write_text(json.dumps(output,ensure_ascii=False,indent=2,default=str),encoding="utf8")
    print("MESH_COUNT",len(meshes),"LINK_COUNT",len(links))
    for m in meshes:print("MESH",m["serializedFile"],m["pathId"],m["name"],"AABB",m["aabb"],"vertices",m["vertexCount"])
    for l in links:print("LINK",l["gameObject"],l["meshPointer"],"sameFileMesh",l["localMeshName"])
    for a in output["assetBundle"]:print("ASSETBUNDLE",a)
if __name__=="__main__":main()

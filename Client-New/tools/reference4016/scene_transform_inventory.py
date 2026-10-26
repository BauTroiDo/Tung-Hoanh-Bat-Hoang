"""Read-only original Unity 4016 scene transform inventory.

Writes only analytical metadata to Client-New (no models, textures, shaders, scripts, or assets).
No client execution. Does not modify the input bundle.
"""
from __future__ import annotations
import argparse
from collections import Counter
from datetime import datetime, timezone
import hashlib
import json
from pathlib import Path
import sys

PROJECT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(PROJECT / ".tmp" / "reference4016-python-deps"))
import UnityPy


def ref_id(v):
    if not isinstance(v, dict):
        return 0
    return int(v.get("m_PathID", v.get("path_id", 0)))


def vec(v, fields=("x", "y", "z")):
    return [float(v.get(f, 0)) for f in fields]


def qmult(a, b):
    ax, ay, az, aw = a
    bx, by, bz, bw = b
    return [
        aw * bx + ax * bw + ay * bz - az * by,
        aw * by - ax * bz + ay * bw + az * bx,
        aw * bz + ax * by - ay * bx + az * bw,
        aw * bw - ax * bx - ay * by - az * bz,
    ]


def rotate(q, v):
    # Assumes normalized Unity quaternion, inverse is conjugate.
    x, y, z, w = q
    t = [2 * (y*v[2]-z*v[1]), 2*(z*v[0]-x*v[2]), 2*(x*v[1]-y*v[0])]
    c = [y*t[2]-z*t[1], z*t[0]-x*t[2], x*t[1]-y*t[0]]
    return [v[i]+w*t[i]+c[i] for i in range(3)]


def add(a, b):
    return [a[i]+b[i] for i in range(3)]


def mul(a, b):
    return [a[i]*b[i] for i in range(3)]


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--bundle", type=Path, required=True)
    ap.add_argument("--out", type=Path, required=True)
    args = ap.parse_args()
    env = UnityPy.load(str(args.bundle))
    gos, trs, other = {}, {}, []
    counts = Counter()
    for o in env.objects:
        typ = o.type.name
        counts[typ] += 1
        if typ in ("GameObject", "Transform"):
            data = o.parse_as_dict()
            if typ == "GameObject":
                gos[int(o.path_id)] = {"name": data.get("m_Name", ""), "active": data.get("m_IsActive"), "components": [ref_id(v.get("component", v)) for v in data.get("m_Component", [])]}
            else:
                trs[int(o.path_id)] = {"go": ref_id(data.get("m_GameObject")), "parent": ref_id(data.get("m_Father")), "position": vec(data["m_LocalPosition"]), "rotation": vec(data["m_LocalRotation"], ("x", "y", "z", "w")), "scale": vec(data["m_LocalScale"]), "children": len(data.get("m_Children", []))}
        elif typ == "MonoBehaviour":
            try:
                data = o.parse_as_dict()
                other.append({"type": typ, "pathId": o.path_id, "fieldNames": list(data.keys())[:80], "scriptPathId": ref_id(data.get("m_Script"))})
            except Exception as e:
                other.append({"type": typ, "pathId": o.path_id, "parseError": type(e).__name__ + ": " + str(e)[:150]})
        elif typ == "AssetBundle":
            data = o.parse_as_dict()
            other.append({"type": typ, "name": data.get("m_Name"), "sceneHashes": data.get("m_SceneHashes"), "containerCount": len(data.get("m_Container", []))})
    calc = {}
    def world(tid, stack=()):
        if tid in calc:
            return calc[tid]
        if tid in stack:
            raise ValueError("cycle in transform hierarchy")
        t = trs[tid]
        pid = t["parent"]
        if pid and pid not in trs:
            result = {"unresolvedParent": pid, "position": None, "rotation": None, "scale": None}
        elif not pid:
            result = {"position": t["position"], "rotation": t["rotation"], "scale": t["scale"]}
        else:
            p = world(pid, stack + (tid,))
            if p["position"] is None:
                result = {"unresolvedParent": pid, "position": None, "rotation": None, "scale": None}
            else:
                result = {
                    "position": add(p["position"], rotate(p["rotation"], mul(p["scale"], t["position"]))),
                    "rotation": qmult(p["rotation"], t["rotation"]),
                    "scale": mul(p["scale"], t["scale"]),
                }
        calc[tid] = result
        return result
    entries = []
    for tid, t in sorted(trs.items()):
        w = world(tid)
        go = gos.get(t["go"], {})
        entries.append({
            "gameObject": go.get("name", "unknown"), "active": go.get("active"),
            "goPathId": t["go"], "transformPathId": tid, "parentTransformId": t["parent"],
            "localPosition": [round(v, 6) for v in t["position"]],
            "localRotation": [round(v, 8) for v in t["rotation"]],
            "localScale": [round(v, 6) for v in t["scale"]],
            "worldPosition": None if w["position"] is None else [round(v, 6) for v in w["position"]],
            "worldScale": None if w["scale"] is None else [round(v, 6) for v in w["scale"]],
            "unresolvedParent": w.get("unresolvedParent"),
            "componentCount": len(go.get("components", [])),
        })
    with args.bundle.open("rb") as f:
        digest = hashlib.file_digest(f, "sha256").hexdigest().upper()
    result = {
        "sourceBundleName": args.bundle.name,
        "sourceSha256": digest, "sourceByteLength": args.bundle.stat().st_size,
        "unityPyVersion": UnityPy.__version__,
        "analysisUtc": datetime.now(timezone.utc).isoformat(),
        "scope": "static metadata only, no asset export",
        "counts": dict(counts),
        "sceneObjects": entries,
        "nonGeometryMetadata": other,
        "interpretation": "These are transform values from the 4016 scene bundle, not 3+1 paired legacy coordinates; prefab and runtime additive geometry may be loaded separately.",
    }
    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.write_text(json.dumps(result, ensure_ascii=False, indent=2, default=str), encoding="utf8")
    print("WROTE", args.out, "go", len(gos), "transform", len(trs), "unresolved", sum(bool(e["unresolvedParent"]) for e in entries))
    for e in sorted(entries, key=lambda x: x["gameObject"]):
        print(e["gameObject"], "world=", e["worldPosition"], "parent=", e["parentTransformId"])
if __name__ == "__main__":
    main()

"""Read-only UnityFS structural probe for the user's local 4016 reference. No asset export."""
import collections
import hashlib
import json
import pathlib
import sys
import traceback

PROJECT = pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0, str(PROJECT / ".tmp" / "reference4016-python-deps"))
import UnityPy


def probe(path):
    path = pathlib.Path(path)
    print("FILE", path.name, "BYTES", path.stat().st_size, flush=True)
    with path.open("rb") as f:
        print("MAGIC", repr(f.read(28)), flush=True)
    with path.open("rb") as f:
        print("SHA256", hashlib.file_digest(f, "sha256").hexdigest(), flush=True)
    env = UnityPy.load(str(path))
    print("UNITYPY", getattr(UnityPy, "__version__", "unknown"), flush=True)
    print("FILES", [str(s)[:120] for s in list(env.files)[:30]], flush=True)
    print("OBJECT_COUNT", len(env.objects), flush=True)
    types = collections.Counter(o.type.name for o in env.objects)
    print("TYPE_COUNTS", json.dumps(types, ensure_ascii=False), flush=True)
    for kind in ("GameObject", "Transform", "SceneAsset", "AssetBundle"):
        refs = [o for o in env.objects if o.type.name == kind]
        for o in refs[:5]:
            try:
                data = o.parse_as_dict()
                keys = list(data)
                sample = {k: data[k] for k in keys if k in ("m_Name", "m_GameObject", "m_Father", "m_LocalPosition", "m_LocalRotation", "m_RootOrder", "m_ScenePaths", "m_PreloadTable")}
                print("SAMPLE", kind, "path_id", o.path_id, "keys", keys[:30], "data", json.dumps(sample, ensure_ascii=False, default=str)[:900], flush=True)
            except Exception as exc:
                print("SAMPLE_ERROR",kind, type(exc).__name__, str(exc)[:250], flush=True)


if __name__ == "__main__":
    for p in sys.argv[1:]:
        try: probe(p)
        except Exception: traceback.print_exc()

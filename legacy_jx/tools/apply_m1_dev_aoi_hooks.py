#!/usr/bin/env python3
"""Byte-preserving M1 DEV AOI hook stage/check for legacy JX source.

Never rewrites the original source by default. Checks exact source anchors
then writes patched copies of three legacy .cpp files into --output-dir.
Original legacy C++ files may contain CP936/CP1252 and MUST NOT be round-tripped
through a UTF-8 editor or GitHub text API. This script operates only on bytes.
"""
from __future__ import annotations
import argparse
import hashlib
import json
from pathlib import Path
import re

FILES = ("KNpcSet.cpp", "KNpc.cpp", "KSubWorld.cpp")
INCLUDE = b'#include "Jx3dM1DevAoiHooks.h"'
MARKERS = (
    b"JX3D_M1_DEV_AOI_EMIT_SPAWN",
    b"JX3D_M1_DEV_AOI_EMIT_MOVE",
    b"JX3D_M1_DEV_AOI_EMIT_REMOVE",
    b"JX3D_M1_DEV_AOI_EMIT_TICK",
)

def once(buffer: bytes, needle: bytes, label: str) -> None:
    seen = buffer.count(needle)
    if seen != 1:
        raise ValueError(f"{label}: expected one source anchor, found {seen}")

def splice_method_end(source: bytes, begin: bytes, until: bytes,
                      hook: bytes, label: str, newline: bytes) -> bytes:
    once(source, begin, label + " start")
    once(source, until, label + " next method")
    a = source.index(begin)
    b = source.index(until, a + len(begin))
    section = source[a:b]
    matches = list(re.finditer(rb"(?:^|\n)\}[ \t]*(?=\r?\n)", section))
    if len(matches) != 1:
        raise ValueError(f"{label}: unable to locate unique closing brace")
    close = a + matches[0].start()
    # Match includes the previous newline when at start of an earlier line.
    if source[close:close+1] == b"\n":
        close += 1
    return source[:close] + b"    " + hook + newline + source[close:]

def patch(filename: str, blob: bytes) -> bytes:
    if filename not in FILES:
        raise ValueError("unsupported legacy file")
    if any(m in blob for m in MARKERS) or INCLUDE in blob:
        raise ValueError("already patched or mixed-generation DEV hooks")
    newline = b"\r\n" if b"\r\n" in blob else b"\n"
    once(blob, b'#include "KCore.h"', filename + " initial header")
    blob = blob.replace(b'#include "KCore.h"',
        b'#include "KCore.h"' + newline + INCLUDE, 1)
    if filename == "KNpcSet.cpp":
        begin = b"int KNpcSet::AddServerNpcB("
        end = b"void KNpcSet::Remove("
        once(blob, begin, "NPC spawn")
        once(blob, end, "NPC remove method")
        a, b = blob.index(begin), blob.index(end)
        section = blob[a:b]
        reference = b"SubWorld[nSubWorld].m_Region[nRegion].AddRef(nMapX, nMapY, obj_npc);"
        once(section, reference, "region attach")
        match = re.search(re.escape(reference) + rb"[^\r\n]*\r?\n", section)
        if not match:
            raise ValueError("spawn anchor line missing")
        at = a + match.end()
        blob = blob[:at] + b"\tJX3D_M1_DEV_AOI_EMIT_SPAWN(i);" + newline + blob[at:]
        target = re.compile(rb"(?m)^([ \t]*)Npc\[nIdx\]\.ClearNpcState\(\);")
        if len(target.findall(blob)) != 2:
            raise ValueError("expected exactly Remove and RemoveAll release sites")
        blob = target.sub(lambda m: m.group(1) +
            b"JX3D_M1_DEV_AOI_EMIT_REMOVE(nIdx, Npc[nIdx].m_dwID);" +
            newline + m.group(0),blob)
        if blob.count(b"JX3D_M1_DEV_AOI_EMIT_REMOVE") != 2:
            raise ValueError("NPC removal patch missing")
    elif filename == "KNpc.cpp":
        blob = splice_method_end(
            blob,b"VOID KNpc::MovePos(",b"void KNpc::DoHurt(",
            b"JX3D_M1_DEV_AOI_EMIT_MOVE(m_Index);",
            "KNpc movement", newline)
    elif filename == "KSubWorld.cpp":
        blob = splice_method_end(
            blob,b"void KSubWorld::Activate()",b"int KSubWorld::GetDistance(",
            b"JX3D_M1_DEV_AOI_EMIT_TICK(m_SubWorldID);",
            "SubWorld tick", newline)
    return blob

def sha(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()

def main() -> None:
    p=argparse.ArgumentParser()
    p.add_argument("--source-dir",type=Path,required=True)
    p.add_argument("--output-dir",type=Path)
    p.add_argument("--manifest",type=Path)
    p.add_argument("--hook-header",type=Path,
                   help="Path to Jx3dM1DevAoiHooks.h (default source-dir header)")
    a=p.parse_args()
    if a.output_dir and a.source_dir.resolve() == a.output_dir.resolve():
        p.error("refusing to overwrite the source directory")
    outputs=[]
    for name in FILES:
        orig=a.source_dir/name
        raw=orig.read_bytes()
        converted=patch(name,raw)
        if converted==raw:
            raise RuntimeError("no patch generated for "+name)
        if a.output_dir:
            a.output_dir.mkdir(parents=True,exist_ok=True)
            result=a.output_dir/name
            if result.exists():
                raise FileExistsError("never overwrite already-staged source "+str(result))
            result.write_bytes(converted)
        outputs.append({"filename":name,"original_sha256":sha(raw),
                        "staged_sha256":sha(converted),
                        "original_bytes":len(raw),"staged_bytes":len(converted),
                        "writes_to_original":False})
    header=a.hook_header or (a.source_dir/"Jx3dM1DevAoiHooks.h")
    if a.output_dir:
        header_bytes=header.read_bytes()
        if b"#define JX3D_M1_DEV_AOI_EMIT_SPAWN" not in header_bytes or (
            b"#define JX3D_M1_DEV_AOI_EMIT_REMOVE" not in header_bytes):
            raise ValueError("invalid DEV hook macro header")
        destination=a.output_dir/"Jx3dM1DevAoiHooks.h"
        if destination.exists():
            raise FileExistsError("never overwrite already staged header "+str(destination))
        destination.write_bytes(header_bytes)
        outputs.append({"filename":destination.name,"original_sha256":sha(header_bytes),
                        "staged_sha256":sha(header_bytes),"original_bytes":len(header_bytes),
                        "staged_bytes":len(header_bytes),
                        "writes_to_original":False})
    report={"kind":"M1_DEV_AOI_STAGING_ONLY","feature_macro":"JX3D_M1_DEV_AOI",
            "shipping_default_off":True,"files":outputs}
    if a.manifest:
        a.manifest.parent.mkdir(parents=True,exist_ok=True)
        a.manifest.write_text(json.dumps(report,indent=2),encoding="utf8")
    print(json.dumps(report,indent=2))
if __name__=="__main__":
    main()

#!/usr/bin/env python3
"""Regression for byte-preserving legacy JX1 M1 DEV-only hook staging."""
from __future__ import annotations
from pathlib import Path
import hashlib
from apply_m1_dev_aoi_hooks import patch, FILES

ROOT=Path(__file__).resolve().parent.parent / "gameserver/Sources/Core/Src"
HEADER=b'#include "Jx3dM1DevAoiHooks.h"'
EXPECTED={
    "KNpcSet.cpp":[b"JX3D_M1_DEV_AOI_EMIT_SPAWN(i);",
                   b"JX3D_M1_DEV_AOI_EMIT_REMOVE(nIdx, Npc[nIdx].m_dwID);"],
    "KNpc.cpp":[b"JX3D_M1_DEV_AOI_EMIT_MOVE(m_Index);"],
    "KSubWorld.cpp":[b"JX3D_M1_DEV_AOI_EMIT_TICK(m_SubWorldID);"]
}
for fname in FILES:
    original=(ROOT/fname).read_bytes()
    hash0=hashlib.sha256(original).digest()
    altered=patch(fname,original)
    newline=b"\r\n" if b"\r\n" in original else b"\n"
    if altered.count(HEADER)!=1:
        raise AssertionError(fname+" hook header count")
    expected=EXPECTED[fname]
    for marker in expected:
        count=2 if b"EMIT_REMOVE" in marker else 1
        if altered.count(marker)!=count:
            raise AssertionError(fname+" hook count")
    # Inverse all inserted lines: no comment/binary/code/legacy encoding
    # bytes outside these ASCII insertions may have changed.
    reverted=altered.replace(HEADER+newline,b"")
    for marker in expected:
        prefix=b"\t" if fname=="KNpcSet.cpp" else b"    "
        # RemoveAll has two tabs, so remove long indent first.
        for indent in (b"\t\t",prefix):
            reverted=reverted.replace(indent+marker+newline,b"")
    if reverted!=original:
        # All declarations/header bytes must be preserved verbatim.
        raise AssertionError(fname+" source not restored exactly from inserted hooks")
    if hashlib.sha256((ROOT/fname).read_bytes()).digest()!=hash0:
        raise AssertionError(fname+" source file changed while checking")
    try:
        patch(fname,altered)
    except ValueError:
        pass
    else:
        raise AssertionError(fname+" double patch incorrectly allowed")
    print("PASS",fname,"byte-exact roundtrip",len(original),"to",len(altered))
print("M1_LEGACY_BYTE_PRESERVING_HOOK_TEST_PASS")

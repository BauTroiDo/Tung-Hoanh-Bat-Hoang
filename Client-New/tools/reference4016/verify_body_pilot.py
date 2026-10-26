"""Structural audit of scoped CN02-BULK-02 pseudo-code against original PE bytes.

A C pseudo-code marker is not proof of semantic correctness. Emit only metrics
and native RVA, never recovered proprietary bodies, to the tracked JSON.
"""
import bisect
import collections
import hashlib
import json
import re
import struct
import sys
import unittest
from pathlib import Path

ROOT=Path(__file__).resolve().parents[2]
GAME=ROOT.parent
OUT=GAME/"Tool/_outputs/cn02-bulk/body-pilot/type-only/Dump0/dump.cs"
OVERSCOPED=GAME/"Tool/_outputs/cn02-bulk/body-pilot/Dump0/dump.cs"
BINARY=GAME/"Client-3D-2/update/GameAssembly.dll"
SUMMARY=ROOT/"reports/evidence/cn02-4016-il2cpp/body-pilot-summary.json"
EXPECTED_RVAS={
    "MapService.Mps2WorldPos":0x8DFD10,
    "MapService.Map2WorldPos":0x8DFBF0,
    "MapService.UpdateWorldInfo":0x8E0DD0,
    "GameWorld.SubWorld.LoadMap":0xC75DE0,
    "Network.Header.ProcessGameUnity.OnSyncWorld":0xC38990,
}
TYPE_IDS={637,1989,1990,1991,1992,2007}

def native_pe_bytes(blob,rva,size):
    if blob[:2]!=b"MZ": raise ValueError("not MZ")
    pe=struct.unpack_from("<I",blob,0x3c)[0]
    if blob[pe:pe+4]!=b"PE\x00\x00":raise ValueError("not PE")
    ns=struct.unpack_from("<H",blob,pe+6)[0]
    op=struct.unpack_from("<H",blob,pe+20)[0]
    sections=pe+24+op
    for j in range(ns):
        row=sections+j*40
        virtual_size,virt,raw_size,raw=struct.unpack_from("<IIII",blob,row+8)
        if virt<=rva<virt+max(virtual_size,raw_size):
            off=raw+(rva-virt)
            if off+size>len(blob):raise ValueError("out of bounds")
            return blob[off:off+size]
    raise ValueError("RVA not in PE sections: "+hex(rva))

def audit():
    data=OUT.read_text(encoding="utf8")
    candidates=list(re.finditer(r"(?m)^\s*// RVA: 0x([0-9a-fA-F]+) Offset:",data))
    sections=list(re.finditer(r"(?m)^.*?// TypeDefIndex: (\d+)\s*$",data))
    types={int(m.group(1)) for m in sections}
    positions=[m.start() for m in sections]
    native=BINARY.read_bytes()
    sizes,comments,unknown,suspect=collections.Counter(),0,0,0
    per_type=collections.Counter()
    entries={}
    for index,m in enumerate(candidates):
        rva=int(m.group(1),16)
        fragment=data[m.start():candidates[index+1].start() if index+1<len(candidates) else len(data)]
        dis=re.search(r"/\* Disassembly \(x86_64, (\d+) instructions, 0x([0-9a-fA-F]+) bytes\):",fragment)
        pseudo=re.search(r"/\* C pseudo-code:\s*(.*?)\*/",fragment,re.S)
        if not dis or not pseudo:raise ValueError("Missing body marker at "+hex(rva))
        if "{" not in pseudo.group(1) or "}" not in pseudo.group(1):
            raise ValueError("Empty pseudo body at "+hex(rva))
        prefix=re.search(r"(?m)^\s*0x([0-9a-fA-F]{8}):\s+([0-9a-fA-F]+)\s+",fragment)
        if not prefix or int(prefix.group(1),16)!=rva:
            raise ValueError("Native first opcode does not match RVA "+hex(rva))
        first=bytes.fromhex(prefix.group(2))
        if native_pe_bytes(native,rva,len(first))!=first:
            raise ValueError("Original PE first bytes mismatch "+hex(rva))
        type_pos=bisect.bisect_right(positions,m.start())-1
        if type_pos<0:raise ValueError("RVA precedes TypeDef")
        type_id=int(sections[type_pos].group(1))
        per_type[type_id]+=1
        comments+=1
        if "[recovery: suspect" in pseudo.group(1):suspect+=1
        if "__unknown" in pseudo.group(1):unknown+=1
        sizes["disasmBytes"]+=int(dis.group(2),16)
        sizes["disasmInstructions"]+=int(dis.group(1))
        entries[rva]={"typeDefIndex":type_id,"instructions":int(dis.group(1)),
                      "coveredBytes":int(dis.group(2),16),"hasSuspect":("[recovery: suspect" in pseudo.group(1)),
                      "hasUnknown":("__unknown" in pseudo.group(1))}
    raw_log=(GAME/"Tool/_outputs/cn02-bulk/ghidra-pilot-v3/console.log").read_bytes()
    ghlog=(raw_log.decode("utf-16") if raw_log.startswith(bytes((255,254))) or
           raw_log.startswith(bytes((254,255))) else
           raw_log.decode("utf-8",errors="replace"))
    disrows=re.findall(r"CN02_GHIDRA_DISASM rva=0x([0-9a-f]+) decoded=(true|false) nativeSpan=([0-9]+) decodedBytes=([0-9]+)",ghlog)
    resrows=re.findall(r"CN02_GHIDRA_RESULT rva=0x([0-9a-f]+) status=([A-Z_]+) chars=([0-9]+) bodyAddresses=([0-9]+)",ghlog)
    ghdis={int(a,16):{"decoded":v=="true","expectedSpanBytes":int(s),"decodedBytes":int(b)} for a,v,s,b in disrows}
    ghresult={int(a,16):{"status":s,"pseudoChars":int(c),"functionBodyBytes":int(b)} for a,s,c,b in resrows}
    if len(ghdis)!=5 or len(ghresult)!=5:raise ValueError("Ghidra independent pilot incomplete")
    for rva in EXPECTED_RVAS.values():
        if rva not in ghdis or rva not in ghresult:raise ValueError("Missing Ghidra reference RVA")
    result={
      "stage":"CN02-BULK-02-body-pilot",
      "ghidraVersion":"12.2 official PUBLIC",
      "ghidraHeadlessExitCode":0 if "GHIDRA_PILOT_END" in ghlog and "Import succeeded" in ghlog else None,
      "ghidraReadOnlyImportDiscarded":("Discarded file import due to readOnly option" in ghlog),
      "ghidraSampleMethods":{hex(k):{**ghdis[k],**ghresult[k]} for k in sorted(ghdis)},
      "tool":"il2cpp-dumper-rs 8.0.0",
      "selectionMode":"TYPE_ONLY (do not combine assembly+type; filter is UNION)",
      "typeDefIndices":sorted(types),
      "methodsWithDisassemblyAndPseudo":comments,
      "distinctRVAs":len(entries),
      "aliasBindingsBeyondUniqueRVAs":len(candidates)-len(entries),
      "countsByTypeDefIndex":{str(k):v for k,v in sorted(per_type.items())},
      "nativeDisasmInstructionCount":sizes["disasmInstructions"],
      "nativeDisasmCoveredBytes":sizes["disasmBytes"],
      "recoverySuspectMethodCount":suspect,
      "pseudoUnknownMethodCount":unknown,
      "sourceSha256":hashlib.sha256(native).hexdigest().upper(),
      "selectedDumpSizeBytes":OUT.stat().st_size,
      "overscopedDumpSizeBytes":OVERSCOPED.stat().st_size,
      "expectedRvaFirstInstructionBytesVerified":{key:(rva in entries) for key,rva in EXPECTED_RVAS.items()},
      "loadMapPilotDisassemblyInstructions":entries[0xC75DE0]["instructions"],
      "loadMapPilotCoveredBytes":entries[0xC75DE0]["coveredBytes"],
      "loadMapKnownFullInstructionsFromPriorUnwindAudit":2925,
      "loadMapKnownFullNativeBytesFromPriorUnwindAudit":12996,
      "notes":"All 295 method entries (272 distinct RVAs; 23 aliases) contain native disassembly plus generated C-like pseudo, but this is NOT validated source, not a C# decompile, and is NOT necessarily complete. LoadMap output truncates the prior known contiguous native body (2925 instructions/12996 bytes). 14 pseudo bodies have suspect recovery notes; 5 contain __unknown. Raw proprietary code remains outside Git."
    }
    return result

class AuditTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.m=audit()
        from run_body_pilot import command,DEFAULT_TYPES
        cls.command=staticmethod(command)
        cls.default_types=DEFAULT_TYPES
    def test_01_selected_methods(self):self.assertEqual(self.m["methodsWithDisassemblyAndPseudo"],295)
    def test_02_distinct_rvas(self):self.assertEqual(self.m["distinctRVAs"],272)
    def test_03_only_expected_types(self):self.assertEqual(set(self.m["typeDefIndices"]),TYPE_IDS)
    def test_04_group_sum(self):self.assertEqual(sum(self.m["countsByTypeDefIndex"].values()),295)
    def test_05_all_independent_pe_bytes(self):
        self.assertTrue(all(self.m["expectedRvaFirstInstructionBytesVerified"].values()))
    def test_06_unchanged_binary_sha(self):
        self.assertEqual(self.m["sourceSha256"],"011357720B210AA1F392A8B62FF8DE090D3A895230A6E0135BCE707196871069")
    def test_07_suspect_count(self):self.assertEqual(self.m["recoverySuspectMethodCount"],14)
    def test_08_unknown_count(self):self.assertEqual(self.m["pseudoUnknownMethodCount"],5)
    def test_09_original_loadmap_larger_than_pilot(self):
        self.assertLess(self.m["loadMapPilotDisassemblyInstructions"],self.m["loadMapKnownFullInstructionsFromPriorUnwindAudit"])
        self.assertLess(self.m["loadMapPilotCoveredBytes"],self.m["loadMapKnownFullNativeBytesFromPriorUnwindAudit"])
    def test_10_original_larger_than_filtered(self):
        self.assertGreater(self.m["overscopedDumpSizeBytes"],self.m["selectedDumpSizeBytes"]*10)
    def test_11_runner_type_only(self):
        cmd=self.command(Path("X:/safe"),self.default_types)
        self.assertEqual(cmd.count("--pseudo-include-assembly"),0)
        self.assertEqual(cmd.count("--pseudo-include-type"),3)
        self.assertIn("--pseudo-max-body-size",cmd)
    def test_12_runner_rejects_unbounded(self):
        with self.assertRaises(ValueError):self.command(Path("X:/safe"),(),8192)
        with self.assertRaises(ValueError):self.command(Path("X:/safe"),("MapService",),100000)
        with self.assertRaises(ValueError):self.command(Path("X:/safe"),("MapService","MapService"),8192)
    def test_13_no_claim_all_recovered(self):
        self.assertIn("NOT validated source",self.m["notes"])
        self.assertIn("NOT necessarily complete",self.m["notes"])
    def test_14_sample_rva_map(self):self.assertTrue(self.m["expectedRvaFirstInstructionBytesVerified"]["GameWorld.SubWorld.LoadMap"])
    def test_15_nonempty_assembly(self):
        self.assertGreater(self.m["nativeDisasmInstructionCount"],300)
        self.assertGreater(self.m["nativeDisasmCoveredBytes"],2000)
    def test_16_no_proprietary_blob_in_summary(self):
        serial=json.dumps(self.m)
        self.assertNotIn("C pseudo-code:",serial)
        self.assertNotIn("CALL ->",serial)
    def test_17_ghidra_five_exact_rvas(self):
        self.assertEqual(len(self.m["ghidraSampleMethods"]),5)
        self.assertEqual(set(self.m["ghidraSampleMethods"]),{hex(x) for x in EXPECTED_RVAS.values()})
    def test_18_ghidra_bounded_coverage(self):
        for result in self.m["ghidraSampleMethods"].values():
            self.assertTrue(result["decoded"])
            self.assertEqual(result["expectedSpanBytes"],result["decodedBytes"])
            self.assertEqual(result["functionBodyBytes"],result["expectedSpanBytes"])
    def test_19_ghidra_nonempty_decomp(self):
        self.assertEqual(self.m["ghidraHeadlessExitCode"],0)
        for result in self.m["ghidraSampleMethods"].values():
            self.assertEqual(result["status"],"DECOMPILED")
            self.assertGreater(result["pseudoChars"],100)
    def test_20_ghidra_readonly(self):
        self.assertTrue(self.m["ghidraReadOnlyImportDiscarded"])
    def test_21_ghidra_loadmap_two_frames(self):
        self.assertEqual(self.m["ghidraSampleMethods"]["0xc75de0"]["expectedSpanBytes"],12996)
        self.assertGreater(self.m["ghidraSampleMethods"]["0xc75de0"]["functionBodyBytes"],
                           self.m["loadMapPilotCoveredBytes"])
    def test_22_runner_never_writes_into_source_repo(self):
        from run_body_pilot import run
        from types import SimpleNamespace
        a=SimpleNamespace(output=ROOT/"test-unsafe",type=[],
                          max_body_size=8192,timeout=30,dry_run=True)
        with self.assertRaises(ValueError):
            run(a)

if __name__=="__main__":
    import os
    os.environ["PYTHONDONTWRITEBYTECODE"]="1"
    m=audit()
    SUMMARY.parent.mkdir(parents=True,exist_ok=True)
    SUMMARY.write_text(json.dumps(m,ensure_ascii=False,indent=2),encoding="utf8")
    print("METRICS",m["methodsWithDisassemblyAndPseudo"],"suspect",m["recoverySuspectMethodCount"],
          "unknown",m["pseudoUnknownMethodCount"])
    unittest.main(verbosity=2)

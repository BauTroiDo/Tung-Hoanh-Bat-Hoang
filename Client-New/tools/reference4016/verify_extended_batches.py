"""CN02-BULK-04: compare native RVA contracts, three bounded batches and Ghidra probes.

No source-code recovery / reconstructed C snippets are committed. A static RVA
match does not imply packet or scene runtime behavior parity.
"""
import json,re,sqlite3,unittest
from pathlib import Path
from build_extended_manifest import ROOT,GAME,DLL,DEFAULT_DB,EXPECTED_SHA,sha,native_unwind_ranges,DOMAINS,OUT_BASE
from run_extended_batches import plan
from probe_extended import sample,execute

BASE=OUT_BASE
MANIFEST=BASE/"manifest-v1.json"
RUNS=BASE/"runs-v1"
GH=BASE/"ghidra-probe-v1"
SUMMARY=ROOT/"reports/evidence/cn02-4016-il2cpp/extended-summary.json"
EXPECTED_COUNTS={"NetworkProtocol":351,"SceneAddressables":175,"GameplayState":71}
CONTRACTS=(
  ("BishopNetwork::Login()",0x8F3750),
  ("GameServerNetwork::ConnectToServer()",0x92CA30),
  ("Network.Header.ProcessGameUnity::OnSyncWorld()",0xC38990),
  ("MapDataService::LoadForTemplate()",0x8C9BF0),
  ("VietKiem.Levels.Application.SceneLoadService::ResolveVariantKeyAsync()",0xC74650),
  ("WalkState::Enter()",0x87B290),
  ("StateMachine::ChangeState()",0x878A30),
)
def readlog(path):
    b=Path(path).read_bytes()
    return b.decode("utf-16") if b[:2] in (bytes((255,254)),bytes((254,255))) else b.decode("utf8",errors="replace")

def audit():
    manifest=json.loads(MANIFEST.read_text(encoding="utf8"))
    checked={}
    for group in DOMAINS:
        d=RUNS/group
        state=json.loads((d/"run.json").read_text(encoding="utf8"))
        dump=d/"Dump0/dump.cs"
        # Count markers streaming to avoid copying proprietary pseudo-code to summaries.
        counts={"pseudo":0,"rva":0,"suspect":0,"unknown":0}
        with dump.open(encoding="utf8") as reader:
            for line in reader:
                counts["pseudo"]+=line.count("C pseudo-code:")
                counts["rva"]+=1 if re.search(r"^\s*// RVA: 0x",line) else 0
                counts["suspect"]+=line.count("[recovery: suspect")
                counts["unknown"]+=line.count("__unknown")
        checked[group]={
            "selectedTypeCount":len(manifest["domains"][group]),
            "selectedTypes":[x["typeName"] for x in manifest["domains"][group]],
            "methodBindings":sum(x["methodBindings"] for x in manifest["domains"][group]),
            "boundedNativeEntries":sum(len(x["boundedEntrypoints"]) for x in manifest["domains"][group]),
            "exitCode":state["exitCode"],"elapsedSeconds":state["elapsedSeconds"],
            "typeOnlySelectors":state["command"].count("--pseudo-include-type"),
            "hasUnsafeAssemblyFilter":"--pseudo-include-assembly" in state["command"],
            "sourceSHA256":state["binarySHA256"],
            "dumpBytes":dump.stat().st_size,**counts,
        }
    text=readlog(GH/"console.log")
    rows=re.findall(r"CN02_BATCH_ROW rva=0x([0-9a-f]+) nativeSpan=(\d+) decodedBytes=(\d+) functionBodyBytes=(\d+) disasmAccepted=(true|false) decompiled=(true|false) chars=(\d+)",text)
    ghrows={hex(int(rva,16)):{
          "nativeSpan":int(n),"decodedBytes":int(b),"functionBodyBytes":int(f),
          "disasmAccepted":s=="true","decompiled":ok=="true","cChars":int(chars)}
        for rva,n,b,f,s,ok,chars in rows}
    run=json.loads((GH/"run.json").read_text(encoding="utf8"))
    with sqlite3.connect("file:"+DEFAULT_DB.resolve().as_posix()+"?mode=ro",uri=True) as c:
        anchors={}
        for signature,rva in CONTRACTS:
            matches=[int(r[0]) for r in c.execute(
                "SELECT rva FROM methods WHERE assembly=? AND dotnet_signature=?",
                ("Assembly-CSharp",signature))]
            anchors[signature]={"expectedRVA":hex(rva),"found":rva in matches}
    summary={
       "stage":"CN02-BULK-04","binarySHA256":sha(DLL),
       "manifestSHA256":sha(MANIFEST),"sqliteSHA256":sha(DEFAULT_DB),
       "groups":checked,"totalPseudoMarkers":sum(v["pseudo"] for v in checked.values()),
       "totalOutputBytes":sum(v["dumpBytes"] for v in checked.values()),
       "nativeContracts":anchors,
       "ghidraProbe":{"exitCode":run["exitCode"],"elapsedSeconds":run["elapsedSeconds"],
                      "sampleRvas":[hex(x["rva"]) for x in run["probes"]],
                      "rows":ghrows,
                      "readOnlyDiscardConfirmed":"Discarded file import due to readOnly option" in text,
                      "scriptCompleted":"CN02_BATCH_END" in text},
       "toolAudit":{
         "inventoryRoot":r"E:\Hack Game\Cong-cu",
         "IDA_7_3":{"available":"idat64.exe, ida64.exe and PE64 loaders observed",
                    "signing":"idat64.exe and ida64.exe Authenticode NotSigned",
                    "hexRays":"No verified hexrays64.dll / hexx64.dll in IDA 7.3 plugins",
                    "execution":"NOT RUN: provenance and license unverified; separate patched copy exists"},
         "otherTools":{
           "BinaryNinja":"binaryninja.exe Authenticode Valid; license/CLI not tested",
           "ILSpy":"ILSpy.exe Authenticode Valid; managed IL useful but not native IL2CPP bodies",
           "x64dbg":"x64dbg.exe Authenticode Valid; dynamic debugging not executed",
           "DetectItEasy":"diec.exe exists but NotSigned, not executed",
           "Ghidra":"12.2 independently tested, headless read-only static pipeline used"},
       },
       "limits":"Batch pseudo is unverified C-like code, not working C#. Contract signatures and RVA are static only; they do NOT prove live packet framing, login flow, scene load, network responses, combat or Map53 runtime. CN02-04 BLOCKED; CN03 CLOSED."
    }
    return summary

class ExtendedTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.m=json.loads(MANIFEST.read_text(encoding="utf8"))
        cls.result=audit()
        cls.image,cls.pdata=native_unwind_ranges(DLL)
    def test_01_sha(self):self.assertEqual(self.result["binarySHA256"],EXPECTED_SHA)
    def test_02_schema(self):self.assertEqual(self.m["schema"],"CN02-BULK-04-v1")
    def test_03_pdata(self):self.assertGreater(len(self.pdata),300000)
    def test_04_three_domains(self):self.assertEqual(set(self.m["domains"]),set(DOMAINS))
    def test_05_ten_types(self):self.assertEqual(sum(map(len,self.m["domains"].values())),10)
    def test_06_distinct_types(self):
        labels=[x["typeName"] for types in self.m["domains"].values() for x in types]
        self.assertEqual(len(labels),len(set(labels)))
    def test_07_exact_native_span(self):
        for types in self.m["domains"].values():
            for typ in types:
                for e in typ["boundedEntrypoints"]:
                    self.assertEqual(self.pdata[e["rva"]]-e["rva"],e["bytes"])
                    self.assertLessEqual(e["bytes"],8192)
    def test_08_nonzero_count(self):
        for x in self.result["groups"].values():
            self.assertGreater(x["methodBindings"],10)
            self.assertGreater(x["boundedNativeEntries"],2)
    def test_09_domain_plan(self):self.assertEqual(len(plan(MANIFEST)),3)
    def test_10_all_exit_zero(self):
        self.assertTrue(all(x["exitCode"]==0 for x in self.result["groups"].values()))
    def test_11_type_filter_only(self):
        self.assertTrue(all(not x["hasUnsafeAssemblyFilter"] for x in self.result["groups"].values()))
        self.assertEqual([x["typeOnlySelectors"] for x in self.result["groups"].values()],[3,3,4])
    def test_12_sha_fence(self):
        self.assertTrue(all(x["sourceSHA256"]==EXPECTED_SHA for x in self.result["groups"].values()))
    def test_13_pseudo(self):
        self.assertEqual({k:x["pseudo"] for k,x in self.result["groups"].items()},EXPECTED_COUNTS)
    def test_14_rva_pseudo_match(self):
        self.assertTrue(all(x["rva"]==x["pseudo"] for x in self.result["groups"].values()))
    def test_15_reasonable_output(self):
        self.assertTrue(all(50_000<=x["dumpBytes"]<=10_000_000 for x in self.result["groups"].values()))
    def test_16_total(self):self.assertEqual(self.result["totalPseudoMarkers"],597)
    def test_17_contracts(self):
        self.assertEqual(len(self.result["nativeContracts"]),7)
        self.assertTrue(all(x["found"] for x in self.result["nativeContracts"].values()))
    def test_18_probe_sample(self):
        p=sample(MANIFEST)
        self.assertEqual([x["domain"] for x in p],list(DOMAINS))
        self.assertEqual(len(p),3)
    def test_19_probe_three(self):
        self.assertEqual(len(self.result["ghidraProbe"]["rows"]),3)
        self.assertEqual(self.result["ghidraProbe"]["exitCode"],0)
    def test_20_decode_span(self):
        self.assertTrue(all(x["nativeSpan"]==x["decodedBytes"]==x["functionBodyBytes"]
                            for x in self.result["ghidraProbe"]["rows"].values()))
    def test_21_decomp_nonempty(self):
        self.assertTrue(all(x["decompiled"] and x["cChars"]>100
                            for x in self.result["ghidraProbe"]["rows"].values()))
    def test_22_read_only(self):
        self.assertTrue(self.result["ghidraProbe"]["readOnlyDiscardConfirmed"])
        self.assertTrue(self.result["ghidraProbe"]["scriptCompleted"])
    def test_23_refuse_git(self):
        with self.assertRaises(ValueError):execute(MANIFEST,ROOT/"UNSAFE",dry_run=True)
    def test_24_no_semantic_claim(self):
        self.assertIn("NOT prove live packet",self.result["limits"])
        self.assertIn("CN02-04 BLOCKED",self.result["limits"])
    def test_25_ida_not_executed(self):
        self.assertIn("NOT RUN",self.result["toolAudit"]["IDA_7_3"]["execution"])
    def test_26_no_proprietary_bodies_in_summary(self):
        s=json.dumps(self.result)
        self.assertNotIn("C pseudo-code:",s)
        self.assertNotIn("Disassembly (x86_64",s)
    def test_27_raw_evidence_outside_git(self):
        self.assertTrue(MANIFEST.is_relative_to(GAME/"Tool/_outputs"))
        self.assertFalse(MANIFEST.is_relative_to(ROOT))

if __name__=="__main__":
    result=audit()
    SUMMARY.parent.mkdir(parents=True,exist_ok=True)
    SUMMARY.write_text(json.dumps(result,ensure_ascii=False,indent=2),encoding="utf8")
    print("CN02_BULK04_SUMMARY",result["totalPseudoMarkers"],"pseudo,",
          len(result["nativeContracts"]),"RVA anchors,",
          len(result["ghidraProbe"]["rows"]),"Ghidra probes")
    unittest.main(verbosity=2)

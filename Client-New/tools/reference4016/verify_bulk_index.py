"""Regression for IL2CPP v39 bulk index and independent dumper/Inspector evidence."""
import json,re,sqlite3,unittest
from pathlib import Path

ROOT=Path(__file__).resolve().parents[2]
REPORT=ROOT/"reports/evidence/cn02-4016-il2cpp/bulk-summary.json"
BASE=Path(r"E:\Games\VLTK3D\SwordOnline\Tool\_outputs\cn02-bulk")
class BulkTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.s=json.loads(REPORT.read_text(encoding="utf8"))
        cls.con=sqlite3.connect(cls.s["sqlitePath"])
        cls.inspector=(BASE/"inspector/cs/il2cpp.cs").read_text(encoding="utf8")
        def read_log(path):
            raw=path.read_bytes()
            return raw.decode("utf-16") if raw.startswith(bytes((255,254))) or raw.startswith(bytes((254,255))) else raw.decode("utf-8",errors="replace")
        cls.ilog=read_log(BASE/"inspector_run.log")
        cls.dlog=read_log(BASE/"dumper_run.log")
    @classmethod
    def tearDownClass(cls):cls.con.close()
    def lookup(self,signature,rva):
        matches=[row[0] for row in self.con.execute(
            "SELECT rva FROM methods WHERE dotnet_signature = ?",(signature,))]
        self.assertIn(rva,matches)
    def test_01_source_sha(self):
        self.assertEqual(self.s["referenceBinarySha256"],"011357720B210AA1F392A8B62FF8DE090D3A895230A6E0135BCE707196871069")
        self.assertEqual(self.s["referenceMetadataSha256"],"5586F416AA26EBE57EA7F6580D0A53E5E5F29C50E425E64A4D3F2A95D6069E08")
    def test_02_script_method_rows(self):
        self.assertEqual(self.s["scriptMethodEntries"],414519)
        self.assertEqual(self.con.execute("SELECT COUNT(*) FROM methods").fetchone()[0],414519)
    def test_03_unique_native_rvas(self):self.assertEqual(self.s["uniqueRva"],198707)
    def test_04_distinct_assemblies(self):self.assertEqual(self.s["distinctAssemblyGroups"],120)
    def test_05_type_groups(self):self.assertEqual(self.s["distinctTypeGroups"],39504)
    def test_06_no_empty_rva(self):self.assertEqual(self.s["entriesWithoutPositiveRva"],0)
    def test_07_map_service_mapping(self):
        self.lookup("MapService::Mps2WorldPos()",0x8DFD10)
        self.lookup("MapService::Map2WorldPos()",0x8DFBF0)
    def test_08_world_mapping(self):
        self.lookup("GameWorld.SubWorld::LoadMap()",0xC75DE0)
        self.lookup("MapService::UpdateWorldInfo()",0x8E0DD0)
    def test_09_network_mapping(self):
        self.lookup("Network.Header.ProcessGameUnity::OnSyncWorld()",0xC38990)
    def test_10_dumper_metadata(self):
        self.assertIn("Metadata Version: 39",self.dlog)
        self.assertIn("Type Definitions: 21485",self.dlog)
        self.assertIn("Method Definitions: 161267",self.dlog)
        self.assertIn("DUMPER_BULK_EXIT=0",self.dlog) if "DUMPER_BULK_EXIT=0" in self.dlog else self.assertIn("All tasks completed successfully!",self.dlog)
    def test_11_inspector_metadata(self):
        self.assertIn("v39.0-TypeDefinitionSize2_GenericContainerSize2_TypeSize4_ParameterSize4",self.ilog)
        self.assertIn("Export finished",self.ilog)
    def test_12_codegen_crosscheck(self):
        for log in (self.ilog,self.dlog):
            self.assertRegex(log,r"(?i)(?:0x0*184adef50)")
            self.assertRegex(log,r"(?i)(?:0x0*1856de9a0)")
    def test_13_inspector_type_index(self):
        for type_name,index in (("MapService",637),("SubWorld",2007),("WORLD_SYNC",13018)):
            self.assertRegex(self.inspector,
                   r"(?m)^\s*public\s+(?:static\s+)?(?:class|struct)\s+"+
                   type_name+r"\b[^\n]*TypeDefIndex:\s*"+str(index))
    def test_14_inspector_type_index_boundary_discrepancy(self):
        seen={int(v) for v in re.findall(r"TypeDefIndex:\s*(\d+)",self.inspector)}
        missing=set(range(21485))-seen
        dump=BASE/"dumper/Dump0/dump.cs"
        header=re.compile(r"// Image (\d+): .+ - (\d+)")
        image_starts=set()
        with dump.open(encoding="utf8") as f:
            for line in f:
                match=header.match(line)
                if match:image_starts.add(int(match.group(2)))
        self.assertEqual(len(missing),121)
        self.assertTrue(missing<=image_starts)
        self.assertEqual(len(image_starts),122)
        self.assertIn(637,seen)
        self.assertIn(2007,seen)
        self.assertIn(13018,seen)
    def test_14_query_indices(self):
        indexes={r[1] for r in self.con.execute("pragma index_list(methods)")}
        self.assertTrue({"idx_methods_rva","idx_methods_assembly_type","idx_methods_dotnet"}<=indexes)
    def test_15_no_native_decompile_claim(self):
        self.assertIn("NOT identical to MethodDefinition count",self.s["notes"])
        self.assertIn("No function bodies were decompiled",self.s["notes"])
    def test_16_dumper_warning_is_reported(self):
        self.assertIn("Failed to write diffable cs",self.dlog)
        self.assertIn("No exact match for IL2CPP APIs",self.dlog)
if __name__=="__main__":unittest.main(verbosity=2)

import pathlib
import struct
import tempfile
import unittest

from audit_map53_npc_source_readonly import inspect


def save_region(root, rx, ry, count):
    directory = root / f"v_{ry:03d}"
    directory.mkdir(parents=True, exist_ok=True)
    npc = struct.pack("<I", count) + bytes(count * 69) if count else b""
    header = struct.pack("<I", 6)
    lengths = [2048, 0, len(npc), 0, 0, 0]
    offset = 0
    for n in lengths:
        header += struct.pack("<II", offset, n)
        offset += n
    data = header + bytes(2048) + npc
    (directory / f"{rx:03d}_region_s.dat").write_bytes(data)


class RegionNpcAuditTest(unittest.TestCase):
    def test_counts_static_source_not_runtime(self):
        with tempfile.TemporaryDirectory() as d:
            root = pathlib.Path(d)
            save_region(root, 98, 102, 1)
            save_region(root, 99, 102, 0)
            result = inspect(root)
            self.assertEqual(result["files"], 2)
            self.assertEqual(result["regions_with_npc_definitions"], 1)
            self.assertEqual(result["total_static_npc_definitions"], 1)
            self.assertIn("NOT", result["note"])
            self.assertEqual(result["errors"], [])

    def test_detect_truncated_npc_header(self):
        with tempfile.TemporaryDirectory() as d:
            root = pathlib.Path(d)
            save_region(root, 98, 102, 1)
            p = root / "v_102" / "098_region_s.dat"
            p.write_bytes(p.read_bytes()[:25])
            result = inspect(root)
            self.assertEqual(result["parsed"], 0)
            self.assertEqual(len(result["errors"]), 1)

    def test_region_bounds_nearby(self):
        with tempfile.TemporaryDirectory() as d:
            root = pathlib.Path(d)
            save_region(root, 98, 102, 2)
            save_region(root, 150, 102, 3)
            result = inspect(root)
            self.assertEqual(result["total_static_npc_definitions"], 5)
            self.assertEqual(len(result["nearby_regions"]), 1)
            self.assertEqual(result["nearby_regions"][0]["rx"], 98)


if __name__ == "__main__":
    unittest.main()

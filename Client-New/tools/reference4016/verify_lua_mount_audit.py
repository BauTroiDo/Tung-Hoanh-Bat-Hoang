"""Reject false script-presence claims and malformed PACK indexes."""
import struct
import tempfile
import unittest
from pathlib import Path
from lua_mount_audit import pack_ids, file_id, audit


def pack(entry_id):
    # One uncompressed three-byte entry, then one 16-byte index row.
    return struct.pack('<4sIIIII8s', b'PACK', 1, 35, 32, 0, 0, b'\0'*8) + b'abc' + struct.pack('<IIII', entry_id, 32, 3, 3)


class LuaMountAuditTests(unittest.TestCase):
    def test_hash_uses_win32_overflow(self):
        # Empty and one-character results can be calculated by hand.
        self.assertEqual(file_id(''), 0x12345678)
        self.assertEqual(file_id('a'), 0xedcbaff7)

    def test_lookup_normalizes_ascii_case(self):
        self.assertEqual(file_id(r'\Script\System\ONLINE.lua'), file_id(r'\script\system\online.lua'))

    def test_reads_real_index_position(self):
        self.assertEqual(pack_ids(pack(0x11223344)), {0x11223344})

    def test_rejects_truncated_index(self):
        with self.assertRaises(ValueError):
            pack_ids(pack(1)[:-1])

    def test_rejects_bad_signature(self):
        with self.assertRaises(ValueError):
            pack_ids(b'FAIL' + pack(1)[4:])

    def test_rejects_entry_outside_data(self):
        b = bytearray(pack(1))
        struct.pack_into('<I', b, 39, 9000)
        with self.assertRaises(ValueError):
            pack_ids(bytes(b))

    def test_unmounted_archive_does_not_resolve_script(self):
        with tempfile.TemporaryDirectory() as d:
            root = Path(d)
            (root/'pak').mkdir()
            (root/'pak/script.pak').write_bytes(pack(file_id(r'\script\system\online.lua')))
            (root/'package.ini').write_text('[Package]\nPath=\\pak\n')
            report = audit(root, [r'\script\system\online.lua'])
            self.assertEqual(report['scripts'][0]['archive_candidates'], ['script.pak'])
            self.assertEqual(report['scripts'][0]['mounted_candidates'], [])
            self.assertFalse(report['scripts'][0]['source_available'])

    def test_mounted_archive_and_loose_source_are_distinct(self):
        with tempfile.TemporaryDirectory() as d:
            root = Path(d)
            (root/'pak').mkdir()
            (root/'pak/script.pak').write_bytes(pack(file_id(r'\script\system\online.lua')))
            (root/'package.ini').write_text('[Package]\nPath=\\pak\n0=script.pak\n')
            item = audit(root, [r'\script\system\online.lua'])['scripts'][0]
            self.assertEqual(item['mounted_candidates'], ['script.pak'])
            self.assertFalse(item['loose_exists'])
            self.assertTrue(item['source_available'])

    def test_finds_loose_source_without_package(self):
        with tempfile.TemporaryDirectory() as d:
            root = Path(d)
            (root/'script/system').mkdir(parents=True)
            (root/'script/system/online.lua').write_text('function OnLine() end')
            item = audit(root, [r'\script\system\online.lua'])['scripts'][0]
            self.assertTrue(item['loose_exists'])
            self.assertTrue(item['source_available'])

    def test_rejects_package_path_escape(self):
        with tempfile.TemporaryDirectory() as d:
            root = Path(d)
            (root/'package.ini').write_text('[Package]\nPath=..\\elsewhere\n')
            with self.assertRaises(ValueError):
                audit(root, [])


if __name__ == '__main__':
    unittest.main(verbosity=2)

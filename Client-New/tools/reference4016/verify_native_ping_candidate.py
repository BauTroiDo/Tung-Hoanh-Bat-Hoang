"""Structural tests only; cannot certify network ping token semantics."""
import unittest
from native_ping_candidate import candidate_legacy_ping, inspect_legacy_candidate

class NativePingCandidateTests(unittest.TestCase):
    def test_converted_struct_length_and_opcode(self):
        packet=candidate_legacy_ping(bytes.fromhex("52 78 56 34 12"),0xABCD)
        self.assertEqual(len(packet),9)
        self.assertEqual(packet[0],122)
        self.assertEqual(packet,bytes.fromhex("7a 78 56 34 12 cd ab 00 00"))
    def test_preserves_server_token_verbatim(self):
        for value in (0,1,0xFFFFFFFF,0x80000000,0x11223344):
            packet=candidate_legacy_ping(b"\x52"+value.to_bytes(4,"little"),123)
            self.assertEqual(inspect_legacy_candidate(packet),(value,123))
    def test_distinguishes_gateway_clock_from_server_token(self):
        packet=candidate_legacy_ping(b"\x52\x00\x00\x00\x00",17)
        self.assertEqual(inspect_legacy_candidate(packet),(0,17))
    def test_rejects_short_or_extended_packet(self):
        for wrong in (b"",b"\x52",b"\x52\x00\x00\x00",b"\x52"+b"\0"*8):
            with self.assertRaises(ValueError):candidate_legacy_ping(wrong,42)
    def test_rejects_other_opcodes(self):
        with self.assertRaises(ValueError):candidate_legacy_ping(b"\x51\0\0\0\0",42)
    def test_rejects_non_bytes(self):
        with self.assertRaises(ValueError):candidate_legacy_ping(bytearray(b"\x52\0\0\0\0"),42)
    def test_bounds(self):
        for bad in (-1,0x100000000,1.5,None,True):
            with self.assertRaises(ValueError):candidate_legacy_ping(b"\x52\0\0\0\0",bad)
    def test_max_clock(self):
        self.assertEqual(inspect_legacy_candidate(candidate_legacy_ping(b"\x52\0\0\0\0",0xFFFFFFFF)),(0,0xFFFFFFFF))
    def test_raw_five_byte_not_accepted_as_legacy(self):
        with self.assertRaises(ValueError):inspect_legacy_candidate(bytes.fromhex("7a 78 56 34 12"))
    def test_padding_unrelated_to_native_opcode(self):
        x=candidate_legacy_ping(b"\x52"+bytes.fromhex("ff ff ff ff"),0)
        self.assertEqual(x[1:5],b"\xff"*4)
        self.assertEqual(x[5:9],b"\0"*4)

if __name__=="__main__":
    unittest.main(verbosity=2)

"""GP01 regression: original GS NPC proof before any 4016 NPC creation."""
import struct
import unittest

from native_npc_aoi import NativeNpcAoiProbe, build_dev_npc_snapshot_request
from world_conversion import SCHEMA


def gs_min(entity, x=50400, y=104400):
    layout = SCHEMA["layouts"]["NPC_NORMAL_SYNC"]
    p = bytearray(layout["size"])
    p[0] = 77
    for key, val, fmt in (("ID", entity, "<I"), ("MapX", x, "<i"), ("MapY", y, "<i")):
        struct.pack_into(fmt, p, layout["fields"][key]["offset"], val)
    return bytes(p)


def gs_full(entity, x=50400, y=104400):
    layout = SCHEMA["layouts"]["NPC_SYNC"]
    offset = layout["fields"]["m_szName"]["offset"]
    p = bytearray(offset + 4)
    p[0] = 76
    struct.pack_into("<H", p, 1, len(p) - 1)
    struct.pack_into("<I", p, layout["fields"]["ID"]["offset"], entity)
    struct.pack_into('<ii', p, layout['fields']['MapX']['offset'], x, y)
    p[offset:] = b"NPC\0"
    return bytes(p)


def native_req(entity):
    return struct.pack("<BI", 92, entity)


class NativeNpcAoiProbeTests(unittest.TestCase):
    def setUp(self):
        self.p = NativeNpcAoiProbe(40723, radius=3000, request_limit=4, burst=2)
        self.xy = (50401, 104412)

    def test_min_to_exact_legacy_request(self):
        request = self.p.observe_server(77, gs_min(1001), self.xy, now=1)
        self.assertIsNotNone(request)
        self.assertEqual(len(request), 69)
        self.assertEqual(request[:5], struct.pack("<BI", 73, 1001))
        self.assertEqual(request[5:], bytes(64))

    def test_repeated_min_does_not_flood_gs(self):
        self.assertIsNotNone(self.p.observe_server(77, gs_min(1001), self.xy, now=1))
        self.assertIsNone(self.p.observe_server(77, gs_min(1001), self.xy, now=1))
        self.assertEqual(self.p.tracker.total_requests, 1)

    def test_reject_unobserved_client_id(self):
        self.assertIsNone(self.p.request_from_native(native_req(2002), self.xy))
        self.assertEqual(self.p.native_rejected, 1)

    def test_client_cannot_request_own_player(self):
        self.assertIsNone(self.p.request_from_native(native_req(40723), self.xy))

    def test_client_reject_zero_id(self):
        self.assertIsNone(self.p.request_from_native(native_req(0), self.xy))

    def test_client_reject_invalid_wire(self):
        for packet in (b"", b"\\", b"\\x5c" + bytes(4), native_req(1)+b"\\0",
                       bytearray(native_req(1))):
            with self.assertRaises(ValueError):
                self.p.request_from_native(packet, self.xy)

    def test_reject_far_min(self):
        self.assertIsNone(self.p.observe_server(77, gs_min(1001, 60000), self.xy, now=1))
        self.assertNotIn(1001, self.p.seen_min)

    def test_accept_only_observed_client_request_once(self):
        self.p.observe_server(77, gs_min(1001), self.xy, now=1)
        self.p.tracker.sent.discard(1001) # simulate a tracked NPC whose request was pending
        packet = self.p.request_from_native(native_req(1001), self.xy)
        self.assertEqual(packet[:5], struct.pack("<BI", 73, 1001))
        self.assertIsNone(self.p.request_from_native(native_req(1001), self.xy))

    def test_second_id_outside_burst_limit(self):
        self.assertIsNotNone(self.p.observe_server(77, gs_min(1001), self.xy, now=5))
        self.assertIsNotNone(self.p.observe_server(77, gs_min(1002), self.xy, now=5))
        self.assertIsNone(self.p.observe_server(77, gs_min(1003), self.xy, now=5))
        self.assertEqual(self.p.tracker.total_requests, 2)

    def test_full_npc_meta_never_synthesizes_native_bytes(self):
        self.p.observe_server(77, gs_min(1001), self.xy, now=1)
        self.assertIsNone(self.p.observe_server(76, gs_full(1001), self.xy, now=2))
        self.assertEqual(self.p.full_metadata[1001]["id"], 1001)
        self.assertIn(1001, self.p.tracker.full)

    def test_bad_min_rejected(self):
        with self.assertRaises(ValueError):
            self.p.observe_server(77, gs_min(1)[:-1], self.xy)

    def test_bad_full_rejected(self):
        self.p.observe_server(77, gs_min(1001), self.xy, now=1)
        with self.assertRaises(ValueError):
            self.p.observe_server(76, gs_full(1001)[:-1], self.xy)

    def test_legacy_remove_clears_observed(self):
        self.p.observe_server(77, gs_min(1001), self.xy, now=1)
        layout = SCHEMA["layouts"]["NPC_REMOVE_SYNC"]
        p = bytearray(layout["size"])
        p[0] = 84
        struct.pack_into("<I", p, layout["fields"]["ID"]["offset"], 1001)
        self.assertIsNone(self.p.observe_server(84, bytes(p), self.xy))
        self.assertNotIn(1001, self.p.seen_min)

    def test_client_request_rejects_far_moved_player(self):
        self.p.observe_server(77, gs_min(1001), self.xy, now=1)
        self.assertIsNone(self.p.request_from_native(native_req(1001), (60000,104412)))

    def test_unobserved_full_does_not_pollute_session_cache(self):
        self.p.observe_server(76, gs_full(3333), self.xy)
        self.assertNotIn(3333, self.p.full_metadata)
        self.assertNotIn(3333, self.p.tracker.full)

    def test_dev_snapshot_request_is_complete_69_bytes(self):
        wire = build_dev_npc_snapshot_request()
        self.assertEqual(len(wire), 69)
        self.assertEqual(wire, struct.pack('<BI', 73, 0) + bytes(64))

    def test_full_packet_does_not_bootstrap_without_explicit_optin(self):
        self.p.observe_server(76, gs_full(1234), self.xy)
        self.assertNotIn(1234, self.p.full_metadata)

    def test_real_gs_snapshot_caches_nearby_npc_only(self):
        self.p.enable_dev_snapshot()
        self.p.observe_server(76, gs_full(1234), self.xy)
        self.assertIn(1234, self.p.full_metadata)
        self.assertEqual(self.p.full_metadata[1234]['id'], 1234)
        self.assertIn(1234, self.p.seen_min)
        self.assertEqual(self.p.tracker.discovered, 1)
        self.p.observe_server(76, gs_full(1234), self.xy)
        self.assertEqual(self.p.tracker.discovered, 1)

    def test_snapshot_rejects_far_and_player_and_bad_packet(self):
        self.p.enable_dev_snapshot()
        self.p.observe_server(76, gs_full(1234, 65000, 104400), self.xy)
        self.assertNotIn(1234, self.p.full_metadata)
        self.p.observe_server(76, gs_full(40723), self.xy)
        self.assertNotIn(40723, self.p.full_metadata)
        with self.assertRaises(ValueError):
            self.p.observe_server(76, gs_full(1234)[:-1], self.xy)

    def test_snapshot_is_bounded_by_original_request_limit(self):
        self.p = NativeNpcAoiProbe(40723, request_limit=2)
        self.p.enable_dev_snapshot()
        for entity in (1001, 1002, 1003):
            self.p.observe_server(76, gs_full(entity), self.xy)
        self.assertEqual(set(self.p.full_metadata), {1001, 1002})

    def test_failure_only_marks_known_npc(self):
        layout = SCHEMA['layouts']['NPC_REQUEST_FAIL']
        p = bytearray(layout['size'])
        p[0] = 155
        struct.pack_into('<I', p, layout['fields']['ID']['offset'], 1001)
        self.p.observe_server(155, bytes(p), self.xy)
        self.assertNotIn(1001, self.p.tracker.failed)
        self.p.observe_server(77, gs_min(1001), self.xy, now=1)
        self.p.observe_server(155, bytes(p), self.xy)
        self.assertIn(1001, self.p.tracker.failed)
        with self.assertRaises(ValueError):
            self.p.observe_server(155, bytes(p)[:-1], self.xy)


if __name__ == "__main__":
    unittest.main()

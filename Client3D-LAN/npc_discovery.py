"""Recover full NPC definitions from GameServer's short NPC visibility stream.

Source: KProtocolProcess::SyncNpcMin (opcode 77) requests full NPC by
SendClientCmdRequestNpc; KProtocolProcess::NpcRequestCommand calls
NpcSet.SyncNpc(id,client), which responds NPC_SYNC (opcode 76) or
NPC_REQUEST_FAIL (opcode 155).

The request *must* match NPC_REQUEST_COMMAND (69 bytes, packed) in
KProtocol.h/KProtocol.cpp, not just opcode+ID.
"""
import struct
import time

from world_conversion import SCHEMA, field

NPC_MIN = 77
NPC_FULL = 76
NPC_REQUEST = 73
NPC_REQUEST_FAIL = 155


def build_npc_request(npc_id):
    if type(npc_id) is not int or not 0 < npc_id <= 0xffffffff:
        raise ValueError("NPC ID must be positive UINT32")
    length = SCHEMA["layouts"]["NPC_REQUEST_FAIL"]["size"]
    if length != 69:
        raise ValueError("Unexpected source-backed NPC request size")
    # NPC_REQUEST_COMMAND and NPC_REQUEST_FAIL share this 69-byte struct.
    packet = struct.pack("<BI", NPC_REQUEST, npc_id) + bytes(length - 5)
    assert len(packet) == length
    return packet


class NPCRequestTracker:
    def __init__(self, player_id, *, limit=64, radius=3072, burst=8):
        if not player_id or limit < 1 or radius < 1 or burst < 1:
            raise ValueError("Invalid NPC request policy")
        self.player_id = player_id
        self.limit = limit
        self.radius = radius
        self.burst = burst
        self.sent = set()
        self.full = set()
        self.failed = set()
        self.total_requests = 0
        self.discovered = 0
        self.request_failures = 0
        self._window = -1
        self._window_count = 0

    def see_packet(self, opcode, packet, player_xy, *, now=None):
        """Return an optional c2s request; no network side effects."""
        if opcode == NPC_FULL:
            entity = field(packet, "NPC_SYNC", "ID")
            if entity != self.player_id and entity > 0:
                self.full.add(entity)
                self.discovered += 1
            return None
        if opcode == NPC_REQUEST_FAIL:
            if len(packet) != SCHEMA["layouts"]["NPC_REQUEST_FAIL"]["size"]:
                raise ValueError("Invalid NPC request failure")
            entity = field(packet, "NPC_REQUEST_FAIL", "ID")
            self.failed.add(entity)
            self.request_failures += 1
            return None
        if opcode != NPC_MIN:
            return None
        if len(packet) != SCHEMA["layouts"]["NPC_NORMAL_SYNC"]["size"]:
            raise ValueError("Invalid NPC_MIN packet")
        entity = field(packet, "NPC_NORMAL_SYNC", "ID")
        if entity <= 0 or entity == self.player_id or entity in self.sent or entity in self.full or entity in self.failed:
            return None
        x = field(packet, "NPC_NORMAL_SYNC", "MapX", "<i")
        y = field(packet, "NPC_NORMAL_SYNC", "MapY", "<i")
        if abs(x - player_xy[0]) > self.radius or abs(y - player_xy[1]) > self.radius:
            return None
        if self.total_requests >= self.limit:
            return None
        second = int(time.monotonic() if now is None else now)
        if second != self._window:
            self._window = second
            self._window_count = 0
        if self._window_count >= self.burst:
            return None
        self._window_count += 1
        self.sent.add(entity)
        self.total_requests += 1
        return build_npc_request(entity)

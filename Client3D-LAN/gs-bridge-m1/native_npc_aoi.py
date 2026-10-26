"""GP01 opt-in native4016 <-> original GS NPC discovery.

This module requests real NPC state from the original GS. It deliberately does
NOT emit a native s2c NPC packet: that marshaller still lacks verified fields.
The feature must remain opt-in until production conversion is accepted.
"""
import struct
from npc_discovery import NPCRequestTracker, build_npc_request
from world_conversion import SCHEMA, field, decode_npc_metadata


def build_dev_npc_snapshot_request():
    """Only a dedicated DEV CoreServer with matching opt-in accepts ID=0.

    Original GS request has a 64-byte trailing name; never forward native92
    with just its opcode relabeled. Both sides MUST opt in separately.
    """
    return struct.pack('<BI', 73, 0) + bytes(64)


class NativeNpcAoiProbe:
    def __init__(self, player_id, *, radius=3072, request_limit=48, burst=4):
        if type(player_id) is not int or player_id <= 0:
            raise ValueError("Authoritative player ID required")
        self.player_id = player_id
        self.radius = radius
        self.tracker = NPCRequestTracker(player_id, radius=radius,
                                         limit=request_limit, burst=burst)
        self.seen_min = {}
        self.full_metadata = {}
        self.native_rejected = 0
        self.native_repeats = 0
        self.server_removed = 0
        self.snapshot_expected = False

    def enable_dev_snapshot(self):
        """Called ONLY after a real client ACK and DEV snapshot request."""
        self.snapshot_expected = True

    def observe_server(self, opcode, packet, player_xy, *, now=None):
        """Process a schema-validated GS packet and optionally request full NPC.

        The caller must have completed the actual GS login ACK and validated
        packet framing via split_batch. No guessed native response is emitted.
        """
        if opcode == 77:
            layout = SCHEMA["layouts"]["NPC_NORMAL_SYNC"]
            if len(packet) != layout["size"] or packet[0] != 77:
                raise ValueError("Invalid authoritative NPC_MIN")
            entity = field(packet, "NPC_NORMAL_SYNC", "ID")
            x = field(packet, "NPC_NORMAL_SYNC", "MapX", "<i")
            y = field(packet, "NPC_NORMAL_SYNC", "MapY", "<i")
            if entity <= 0 or entity == self.player_id:
                return None
            if abs(x - player_xy[0]) > self.radius or abs(y - player_xy[1]) > self.radius:
                return None
            if entity not in self.seen_min and len(self.seen_min) >= self.tracker.limit:
                return None
            self.seen_min[entity] = (x, y)
            return self.tracker.see_packet(opcode, packet, player_xy, now=now)

        if opcode == 76:
            info = decode_npc_metadata(packet)
            entity = info["id"]
            if entity and entity != self.player_id:
                x, y = info['legacy_position']
                nearby = (abs(x - player_xy[0]) <= self.radius and
                          abs(y - player_xy[1]) <= self.radius)
                if (entity in self.seen_min or
                    (self.snapshot_expected and nearby and
                     len(self.seen_min) < self.tracker.limit)):
                    if nearby:
                        self.seen_min[entity] = (x, y)
                        self.full_metadata[entity] = info
                        if entity not in self.tracker.full:
                            self.tracker.see_packet(opcode, packet, player_xy, now=now)
            return None

        if opcode == 155:
            layout = SCHEMA['layouts']['NPC_REQUEST_FAIL']
            if len(packet) != layout['size'] or packet[0] != 155:
                raise ValueError('Malformed authoritative NPC_REQUEST_FAIL')
            entity = field(packet, 'NPC_REQUEST_FAIL', 'ID')
            if entity in self.seen_min:
                self.tracker.see_packet(opcode, packet, player_xy, now=now)
            return None

        if opcode == 84:
            if len(packet) != SCHEMA["layouts"]["NPC_REMOVE_SYNC"]["size"] or packet[0] != 84:
                raise ValueError("Malformed authoritative NPC_REMOVE")
            entity = field(packet, "NPC_REMOVE_SYNC", "ID")
            self.seen_min.pop(entity, None)
            self.full_metadata.pop(entity, None)
            self.tracker.sent.discard(entity)
            self.tracker.full.discard(entity)
            self.tracker.failed.discard(entity)
            self.server_removed += 1
        return None

    def request_from_native(self, packet, player_xy):
        """Convert an observed native 5-byte request; unknown IDs fail closed.

        A client-supplied ID is never treated as proof that NPC exists.
        Requests already automatically sent from GS NPC_MIN are not repeated.
        """
        if type(packet) is not bytes or len(packet) != 5 or packet[0] != 92:
            raise ValueError("Invalid native c2s_requestnpc framing")
        entity = struct.unpack_from("<I", packet, 1)[0]
        if entity == 0 or entity == self.player_id or entity not in self.seen_min:
            self.native_rejected += 1
            return None
        x, y = self.seen_min[entity]
        if abs(x - player_xy[0]) > self.radius or abs(y - player_xy[1]) > self.radius:
            self.native_rejected += 1
            return None
        if entity in self.tracker.sent or entity in self.tracker.full or entity in self.tracker.failed:
            self.native_repeats += 1
            return None
        if self.tracker.total_requests >= self.tracker.limit:
            self.native_rejected += 1
            return None
        self.tracker.sent.add(entity)
        self.tracker.total_requests += 1
        return build_npc_request(entity)

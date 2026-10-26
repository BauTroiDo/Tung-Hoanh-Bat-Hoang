"""Bounded native Map-53 session using the original Bishop ticket and GS.

Protocol-synced means the real native ACK arrived, not that Unity rendered a
scene. Scene/player spawn and actual movement acceptance require separate
runtime evidence. No legacy packet is passed to Unity solely by opcode value.
"""
from dataclasses import dataclass
from pathlib import Path
import logging
import os
from map53_collision_contract import Map53CollisionContract
import select
import struct
import time

from legacy_gameserver import GameServerConnection
from world_conversion import split_batch
from gs_native_relay import (build_native_greeting, recv_wire, decode_frame,
                             encode_frame, authorize_initial_frame)
from native_initial_packets import convert_initial, player_appearance
from native_movement import (bounded_client_movement, authoritative_position,
                             authoritative_move_target, DuplicateMoveGuard)
from native_npc_aoi import NativeNpcAoiProbe, build_dev_npc_snapshot_request
from gs_world_scalar_projection import project_named_initial_packet

LOG = logging.getLogger('client3d-gateway')


@dataclass
class NativeTrafficBudget:
    window_start: float
    window_bytes: int = 0
    window_frames: int = 0
    total_bytes: int = 0
    total_frames: int = 0
    last_activity: float = None

    def __post_init__(self):
        self.last_activity = self.window_start

    def charge(self, size, *, frames=0, now, activity=True):
        if size < 0 or frames < 0:
            raise ValueError('Negative native traffic charge')
        if now - self.window_start >= 10:
            self.window_start = now
            self.window_bytes = self.window_frames = 0
        if self.window_bytes + size > 8*1024*1024 or self.window_frames + frames > 4096:
            raise ValueError('Native M1 traffic rate limit')
        self.window_bytes += size
        self.window_frames += frames
        self.total_bytes += size
        self.total_frames += frames
        if activity:
            self.last_activity = now

    def check_idle(self, now):
        if now - self.last_activity >= 90:
            raise TimeoutError('Native M1 session idle timeout')


@dataclass
class SessionSummary:
    entity_id: int
    position: tuple[int, int]
    backend_acks: int = 0
    moves_sent: int = 0
    confirmed_updates: int = 0
    duplicate_moves_filtered: int = 0
    out_of_range_moves_rejected: int = 0


@dataclass
class NativePingPair:
    """Pair an original GS ping token with ONE native client timing sample.

    GS opcode 143 = 5-byte server PING_COMMAND carrying m_nSendPingTime.
    Native client opcode 82 = 5-byte client timing sample.
    Legacy c2s_ping 122 = 9-byte PING_CLIENTREPLY_COMMAND. Never invent
    the server token, never send a 5-byte legacy 122 packet, and never
    issue a second response to the same GS ping.
    """
    pending_server_tick: int | None = None
    replies_sent: int = 0
    unmatched_client_pings: int = 0

    def receive_gs_ping(self, packet):
        if not isinstance(packet, bytes) or len(packet) != 5 or packet[0] != 143:
            raise ValueError('Expected exact original GS s2c_ping packet')
        self.pending_server_tick = struct.unpack_from('<I', packet, 1)[0]
        LOG.info('M1 ping source opcode=143 bytes=5 server_tick=%d', self.pending_server_tick)

    def receive_native_ping(self, clear):
        if not isinstance(clear, bytes) or len(clear) != 5 or clear[0] != 82:
            raise ValueError('Expected exact native client ping packet')
        if self.pending_server_tick is None:
            self.unmatched_client_pings += 1
            return None
        client_time = struct.unpack_from('<I', clear, 1)[0]
        server_tick = self.pending_server_tick
        self.pending_server_tick = None
        self.replies_sent += 1
        return struct.pack('<BII', 122, server_tick, client_time)


def run_native_session(client, broker, peer_ip, *, first_wire=None,
                       greeting_sent=False, map_list=None, max_seconds=90):
    if map_list is None:
        map_list = Path(__file__).with_name('M1-original-maplist.ini').read_bytes()
    client.settimeout(4)
    if not greeting_sent:
        client.sendall(build_native_greeting())
    wire = first_wire if first_wire is not None else recv_wire(client)
    permit, rx = authorize_initial_frame(wire, broker, peer_ip)
    LOG.info('M1 native GS first frame authorized bytes=%d',len(wire))
    backend = GameServerConnection({'host': permit.gs_host, 'port': permit.gs_port,
                                    'ticket': permit.guid})
    LOG.info('M1 original GS login transport opened')
    tx = 0xffffffff
    started = time.monotonic()
    budget = NativeTrafficBudget(started)
    budget.charge(len(wire), frames=1, now=started)
    next_progress = started + 60
    summary = None
    ping_pair = NativePingPair()
    duplicate_moves = DuplicateMoveGuard()
    collision_contract = None
    if os.environ.get('MAP53_COLLISION_CONTRACT_ENABLED', '1') != '0':
        collision_contract = Map53CollisionContract.load_validated()
        LOG.info('M1 Map53 collision contract verified version=%s sha256=%s regions=%d',
                 collision_contract.version, collision_contract.sha256[:16], collision_contract.non_empty_region_count)
    deadline = None if max_seconds is None else time.monotonic() + max_seconds

    def send_native(packet):
        nonlocal tx
        encoded, tx = encode_frame(packet, tx)
        budget.charge(len(encoded), frames=1, now=time.monotonic(), activity=False)
        client.sendall(encoded)

    try:
        info = backend.initial_sync()
        LOG.info('M1 original GS initial sync received batches=%d bytes=%d',
                 len(info['batches']),sum(len(b) for b in info['batches']))
        initial = convert_initial(info['batches'], map_list)
        if struct.unpack_from('<I', initial.legacy_end, 5)[0] != info['client_key']:
            raise ValueError('Original GS sync key mismatch')
        summary = SessionSummary(initial.entity_id, initial.position)
        # GP01 is opt-in until native NPC marshalling is independently verified.
        npc_probe = (NativeNpcAoiProbe(summary.entity_id)
                     if os.environ.get('GP01_NPC_DISCOVERY_ENABLED', '0') == '1'
                     else None)
        current_player = next(p for b in info['batches'] for op, p in split_batch(b) if op == 68)
        for packet in initial.packets:
            send_native(packet)
        LOG.info('M1 native initial sent map=53 entity=%d packets=%d omitted=%s',
                 summary.entity_id, len(initial.packets), initial.omitted_opcodes)
        LOG.info('M1 traffic policy window_seconds=10 bytes=8388608 frames=4096 idle_seconds=90 fixed_lifetime=%s', max_seconds)
        while deadline is None or time.monotonic() < deadline:
            now = time.monotonic()
            budget.check_idle(now)
            if now >= next_progress:
                LOG.info('M1 session alive entity=%d age_seconds=%.1f total_frames=%d total_bytes=%d',
                         summary.entity_id, now-started, budget.total_frames, budget.total_bytes)
                next_progress = now + 60
            ready, _, _ = select.select([client, backend.sock], [], [], 1)
            if client in ready:
                try:
                    wire = recv_wire(client)
                except ConnectionError:
                    if summary.backend_acks:
                        return summary
                    raise
                clear, rx = decode_frame(wire, rx)
                budget.charge(len(wire), frames=1, now=time.monotonic())
                if clear == b'\x62':
                    if summary.backend_acks:
                        raise ValueError('Repeated native initial sync ACK')
                    # Native SyncEnd emits its ACK before scene loading finishes.
                    # Follow that protocol; do not fabricate a scene-ready event.
                    backend.send(initial.legacy_end)
                    summary.backend_acks = 1
                    LOG.info('M1 original GS ACK sent once entity=%d', summary.entity_id)
                    # GP01 snapshot only if both the bridge and a dedicated DEV
                    # server opt in. Baseline original GS treats ID 0 as invalid.
                    if (npc_probe is not None and
                        os.environ.get('GP01_DEV_NPC_SNAPSHOT', '0') == '1'):
                        backend.send(build_dev_npc_snapshot_request())
                        npc_probe.enable_dev_snapshot()
                        LOG.info('GP01 DEV NPC snapshot requested after real ACK entity=%d',
                                 summary.entity_id)
                elif clear and clear[0] in (85, 117):
                    if not summary.backend_acks:
                        raise PermissionError('Movement before original GS ACK')
                    movement_command = bounded_client_movement(clear, 53, summary.position)
                    if movement_command is None:
                        summary.out_of_range_moves_rejected += 1
                        if summary.out_of_range_moves_rejected % 25 == 1:
                            native_x, native_y = struct.unpack_from('<ii', clear, 1)
                            LOG.warning('M1 stale far movement rejected without session close entity=%d count=%d native=(%d,%d) gs=(%d,%d)',
                                        summary.entity_id, summary.out_of_range_moves_rejected,
                                        native_x, native_y, *summary.position)
                    elif duplicate_moves.should_send(movement_command, time.monotonic()):
                        backend.send(movement_command)
                        summary.moves_sent += 1
                        if summary.moves_sent % 25 == 1:
                            native_x, native_y = struct.unpack_from('<ii', clear, 1)
                            legacy_x, legacy_y = struct.unpack_from('<ii', movement_command, 1)
                            col_info = ''
                            if collision_contract is not None:
                                b_dec = collision_contract.eval_legacy_barrier(legacy_x, legacy_y)
                                s_dec = collision_contract.eval_legacy_segment(summary.position[0], summary.position[1], legacy_x, legacy_y)
                                col_info = f' contract_cell=({b_dec.region_x},{b_dec.region_y}):({b_dec.cell_x},{b_dec.cell_y}) shape={b_dec.shape} kind={b_dec.kind} seg_walkable={s_dec.walkable}'
                            LOG.info('M1 movement forwarded entity=%d count=%d native=(%d,%d) legacy=(%d,%d) gs=(%d,%d)%s',
                                     summary.entity_id, summary.moves_sent, native_x, native_y,
                                     legacy_x, legacy_y, *summary.position, col_info)
                    else:
                        summary.duplicate_moves_filtered += 1
                        if summary.duplicate_moves_filtered % 25 == 1:
                            LOG.info('M1 identical repeat suppressed entity=%d count=%d',
                                     summary.entity_id, summary.duplicate_moves_filtered)
                elif len(clear) == 5 and clear[0] == 82:
                    # Native timing samples are NOT a valid 5-byte legacy ping.
                    # Only respond after the *real* GS opcode143 token arrived.
                    reply = ping_pair.receive_native_ping(clear)
                    if reply is not None:
                        backend.send(reply)
                        LOG.info('M1 ping sent opcode=122 bytes=%d entity=%d server_tick=%d client_tick=%d',
                                 len(reply), summary.entity_id, *struct.unpack_from('<II', reply, 1))
                elif clear and clear[0] == 251 and 3 <= len(clear) <= 1024:
                    # Automatic chat subscriptions are outside the one-player
                    # movement slice. They are never forwarded to legacy GS.
                    LOG.info('M1 native chat subscription omitted bytes=%d', len(clear))
                elif len(clear) == 5 and clear[0] == 92:
                    if npc_probe is None:
                        LOG.info('M1 native NPC request omitted (GP01 gated off)')
                    else:
                        try:
                            request = npc_probe.request_from_native(clear, summary.position)
                        except (ValueError, KeyError, IndexError, struct.error) as exc:
                            LOG.warning('GP01 ignored malformed native NPC request: %s', exc)
                            request = None
                        if request is not None:
                            backend.send(request)
                            LOG.info('GP01 forwarded verified native NPC request to GS id=%d',
                                     struct.unpack_from('<I', clear, 1)[0])
                else:
                    raise ValueError('Unsupported native M1 command')
            if backend.sock in ready:
                try:
                    batch = backend.receive()
                except ConnectionError:
                    if summary.backend_acks:
                        return summary
                    raise
                budget.charge(len(batch), now=time.monotonic())
                for opcode, packet in split_batch(batch):
                    if npc_probe is not None and opcode in (76, 77, 84, 155):
                        pre_full = len(npc_probe.full_metadata)
                        try:
                            original_request = npc_probe.observe_server(
                                opcode, packet, summary.position, now=time.monotonic())
                        except (ValueError, KeyError, IndexError, struct.error) as exc:
                            LOG.warning('GP01 ignored malformed GS NPC record opcode=%d bytes=%d: %s',
                                        opcode, len(packet), exc)
                            original_request = None
                        if original_request is not None:
                            backend.send(original_request)
                            LOG.info('GP01 requested real GS NPC metadata id=%d',
                                     struct.unpack_from('<I', original_request, 1)[0])
                        if len(npc_probe.full_metadata) > pre_full:
                            LOG.info('GP01 observed real GS NPC full count=%d; native spawn still gated',
                                     len(npc_probe.full_metadata))
                    if opcode in (77, 78, 206):
                        entity = struct.unpack_from('<I', packet, 1)[0]
                        if entity != summary.entity_id:
                            continue
                        update = authoritative_position(packet, summary.entity_id)
                        if opcode == 206:
                            last_cmd = duplicate_moves.previous
                            target = struct.unpack_from('<ii', last_cmd, 1) if last_cmd else None
                            LOG.warning('M1 GS forced position entity=%d gs=(%d,%d) last_move_target=%s age_ms=%s',
                                        summary.entity_id, *update.position, target,
                                        round(1000 * (time.monotonic()-duplicate_moves.previous_at))
                                        if duplicate_moves.previous_at is not None else None)
                        send_native(update.packet)
                        if update.position != summary.position:
                            summary.confirmed_updates += 1
                            summary.position = update.position
                            LOG.info('M1 authoritative position entity=%d x=%d y=%d',
                                     summary.entity_id, *summary.position)
                    elif opcode in (85, 86):
                        if struct.unpack_from('<I', packet, 1)[0] == summary.entity_id:
                            send_native(authoritative_move_target(packet, summary.entity_id))
                    elif opcode == 70:
                        send_native(project_named_initial_packet(packet, 's2c_synccurplayernormal').raw)
                    elif opcode == 143:
                        ping_pair.receive_gs_ping(packet)
                    elif opcode == 174:
                        # Legacy GS replies with the exact native client-time
                        # sample that was paired with its own server token.
                        send_native(b'\x83' + packet[1:])
                        LOG.info('M1 ping echo gs_opcode=174 native_opcode=131 bytes=%d entity=%d client_tick=%d',
                                 len(packet), summary.entity_id, struct.unpack_from('<I', packet, 1)[0])
                    elif opcode == 73:
                        if struct.unpack_from('<i', packet, 1)[0] != 53:
                            raise ValueError('Native M1 map transition is unsupported')
                    elif opcode == 67:
                        raise ValueError('Unexpected repeated legacy initial sync end')
                    else:
                        # split_batch has already validated the known source
                        # structure. Unsupported optional features are omitted,
                        # never raw-forwarded or relabelled as native packets.
                        LOG.info('M1 legacy feature omitted opcode=%d bytes=%d', opcode, len(packet))
        raise TimeoutError('Native M1 session deadline')
    finally:
        backend.close()

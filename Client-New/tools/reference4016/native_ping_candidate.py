"""Offline candidate for native 4016 ping -> legacy JX1 GameServer compatibility.

NOT a live gateway patch and NOT verified end-to-end. Before activation,
verify that the 4-byte token in Unity's 0x52 reply is precisely the
unmodified last server ping token, and verify native handling of the echoed
server reply. No invented GS ACKs, no access to character/database state.
"""
import struct

NATIVE_PING_REPLY = 0x52
LEGACY_C2S_PING = 0x7A
NATIVE_LENGTH = 5
LEGACY_LENGTH = 9

def candidate_legacy_ping(native_reply: bytes, observed_gateway_tick_ms: int) -> bytes:
    """Produce a 9-byte STRUCTURAL candidate, not an approved live converter.

    The legacy m_dwClientTime has no equivalent in the observed native
    5-byte reply. A gateway-measured monotonic tick is an explicit proxy
    that the legacy server merely echoes; it cannot be claimed to be the
    client's own clock. This must be reviewed with real client ping behavior.
    """
    if not isinstance(native_reply, bytes) or len(native_reply) != NATIVE_LENGTH:
        raise ValueError("Only exact 5-byte native ping reply is recognized")
    if native_reply[0] != NATIVE_PING_REPLY:
        raise ValueError("Unexpected native opcode")
    if type(observed_gateway_tick_ms) is not int or not 0 <= observed_gateway_tick_ms <= 0xFFFFFFFF:
        raise ValueError("Gateway tick must be measured uint32")
    server_time, = struct.unpack_from("<I",native_reply,1)
    return struct.pack("<BII",LEGACY_C2S_PING,server_time,observed_gateway_tick_ms)

def inspect_legacy_candidate(packet:bytes)->tuple[int,int]:
    if not isinstance(packet,bytes) or len(packet)!=LEGACY_LENGTH or packet[0]!=LEGACY_C2S_PING:
        raise ValueError("Invalid legacy ping candidate")
    return struct.unpack_from("<II",packet,1)

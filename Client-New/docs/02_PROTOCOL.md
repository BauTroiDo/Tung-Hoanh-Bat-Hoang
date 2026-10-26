# 02 — Hợp đồng giao thức đề xuất Client-New v1

**Tình trạng: DESIGN DRAFT; chưa triển khai trên live gateway**. Các tên DTO dưới đây là thiết kế C# mới, KHÔNG phải opcode thực tế được kiểm chứng của client 4016 hoặc JX1 GS. Không dùng làm wire bytes tới gs001.exe trực tiếp.

## Mục tiêu tách adapter

Client-New dùng **ITransport** (PC/Web/Mobile) đến Gateway *endpoint v1 sẽ được triển khai sau*, Gateway chuyển đổi sang giao thức Bishop/legacy GS. Không thay đổi Bishop hay gs001 để thỏa một gói mock.

## Transport

- Message envelope có protocolVersion, messageType, sessionEpoch, seq, requestId, payloadLength và payload; quy cách byte/JSON/binary phải quyết định sau interop + security review.
- Target first candidate: WebSocket binary frames/WSS cho PC/Web/Mobile để thống nhất transport. LAN ws:// dùng chỉ khi môi trường test được cô lập; deployment internet phải TLS/wss://.
- Limit frame (ban đầu đề xuất <= 64 KiB, snapshot chia chunk nếu cần), rate limit, timeout, backpressure, cancellation.
- Không copy nguyên framing legacy Bishop/GS vào WebSocket rồi cho client Web kết nối raw GS.

## Message flow tối thiểu

~~~text
Client  -> Gateway: Hello(protocolVersions, clientBuild, capabilities)
Gateway -> Client : HelloAck(selectedVersion, serverEpoch, limits)
Client  -> Gateway: LoginRequest(... secured credential exchange; never logged)
Gateway -> Client : LoginResult(session handle or error)
Gateway -> Client : RoleList(authenticated role references)
Client  -> Gateway: SelectRole(roleId)
Gateway -> Client : JoinWorldAccepted(worldSessionId, authoritativeMapId, epoch)
Gateway -> Client : WorldSnapshot(entityId, mapId, position, snapshotSeq, ...)
Client              loads map scene & validates ground readiness
Client  -> Gateway: WorldReady(worldSessionId, mapEpoch, acceptedSnapshotSeq)
Client  -> Gateway: MoveRequest(seq, mapEpoch, destinationMapCoord, movementMode)
Gateway -> Client : MoveResult(seq, accepted/denied, reason, authoritativePosition)
Gateway -> Client : PositionUpdate(entityId, serverTick, position, mapEpoch)
~~~

All above are **target contract proposals**. Gateway must validate actual Bishop/GS messages and tickets, no fabricated authoritative result.

## Domain fields

| Field | Type / guard |
|---|---|
| protocolVersion | int, negotiated, must be recognized |
| sessionEpoch | uint monotonic per login, invalidated on new session |
| mapEpoch | uint monotonically incremented on transitions |
| entityId | uint32 > 0, from real GS |
| mapId | int32, existing JX1 map ID; Map53 test uses 53 |
| position | integer fixed-point or explicit coordinates per docs/03 |
| moveSeq | uint32 per session, monotonic and deduplicated |
| requestId | 128-bit random unique opaque, not credential |
| serverTick | uint64 monotonic within session |
| roleId | opaque server-controlled identifier; no user-supplied arbitrary IDs |

## Authentication & data protection

- No credentials/session ticket in metrics or test fixtures.
- Bishop authentication **via gateway only**; token tied to account/role/client peer/session epoch, expiry and single-use on GS handoff.
- Verify role ownership server-side; reject replay, role switch without auth, malformed UTF, oversized string/count.
- No in-memory "trust user location" bypass: map and position check against GS.
- TLS/WSS when using any external network; local plaintext only isolated test and document risk.

## Legacy compatibility baseline (read-only facts)

Known native 4016 GS login: opcode 0x6C, 65-byte plaintext payload (plus 2-byte native frame header); original JX1 GS login: opcode 0x42 followed by 16-byte Bishop token. Legacy bridge handled one-time token and original Map53 snapshot through 14180. Multiple packet mappings remain incomplete; custom v1 adapter must be explicitly implemented, not claimed existing.

Existing ports previously verified: Bishop56722, GS51189, Gateway14180; check live state when integrating. Do not bind new endpoint on live 14180 without approved change plan, compatibility tests and rollback.

## Contract tests trước khi nối thật

1. Hello unsupported version => close with generic error, no auth mutation.
2. Length invalid / duplicate sequence / map epoch mismatch => reject.
3. Role not owned and expired/replayed token => reject.
4. 1 player joins Map53 with real GS entity/position; no fake snapshot.
5. One click after WorldReady => outbound request, GS validation, position update.
6. Reconnect grants new session and discards stale move/scene responses.
7. Disconnect during asset load => no leaked session or phantom actor.

Open decision: envelope encoding protobuf vs MessagePack/custom binary. Owner Network Agent proposes benchmark and tests in a change request; **do not commit irreversible wire format yet**.

## 2026-10-26 — DEV native MPS axis-unit correction (deployed; scene/self-movement observed)

Legacy GS KRegion/Map2Mps/Mps2Map define 512x1024 MPS per region (16x32 cells of 32). Original 4016 KRegion metadata defines 1024x1024 (32x32 cells of 32). Boundary conversion must preserve region identity: nativeX=2*legacyX, nativeY=legacyY; inbound native X quantizes to legacy with floor(X/2), at most 0.5 legacy MPS error. Keep gateway authoritative position summaries in legacy units. Apply to initial NPC MapX/MapY, packed WORLD_SYNC.Region, local authoritative position and walk/run target echoes, and inverse client movement before distance validation. Fixed-point X offsets require doubled remainder modulo one 32768-unit native cell; carry the fractional integer into native MPS X. Y offsets stay unchanged. Reject overflow/malformed identity/length. Do not change affine coefficients or fabricate actor/NPC positions. This corrects protocol units, not calibration acceptance; 3+1 landmarks and real user movement remain required for CN02-04.

## 2026-10-26 — DEV session traffic policy (implementation/acceptance pending)

A valid active native session must not disconnect merely because its lifetime total exceeds4096 frames or8MiB. Apply that budget per10-second traffic window, counting both native frames and original GS batch bytes as before. Validate before sending; reject bursts above either budget. Retain malformed-frame, authentication, ACK, movement and socket guards. Controlled DEV dispatcher should allow active sessions without a fixed300-second lifetime; explicit diagnostic max_seconds remains supported. Close sessions after90seconds without received traffic; do not synthesize ping or alter real timestamp pairing. Required regression: sustained traffic beyond the old cap stays accepted; instantaneous overload remains rejected; optional lifetime and inactivity deadlines still work. Runtime acceptance requires a real client session surviving beyond the former failure interval.

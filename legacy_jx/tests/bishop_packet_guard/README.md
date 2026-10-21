# Bishop V9 packet input hardening

This change is a **defensive first P0 source slice**, not a claim of complete
gateway or original V9 protocol certification.

The Bishop gateways receive bytes from clients, GameServers, the AccountServer
and Goddess/RoleServer. Every peer can send malformed packets, even when an
upstream IOCP transport is already framing messages.

## Scope

- `Intercessor.cpp`: do not read client login packets shorter than
  `1 + sizeof(KLoginAccountInfo)`. Before routing AccountServer packets,
  validate the 1-byte opcode and `KAccountHead` header. For the RoleServer
  route, require the 32-bit identity field or fixed TProcessData header
  before reading it; use a bounded memcpy to read the unaligned long
  packet's identity. Reject unknown/invalid player identities.
- `GamePlayer.cpp`: require nonempty input before `CPackager::Peek`; check
  queued login and extended account-response sizes before field reads.
- `GameServer.cpp`: check null/empty ingress; for fixed-format handlers
  require exact lengths **before** checking internal string field bounds.
  This covers login permission, map/server info, item/GM route commands,
  role lifecycle, requests and broadcast messages. For the variable-length
  `tagUpdateMapID`, check the fixed header minimum, validate the map
  configuration suffix and bound path formatting. Check listbox text length
  before copying into `char buf[ACC_LENGTH]`. Use bounded message
  formatting for account registration.
- `SmartClient.cpp`: validate AccountServer verification packet size
  before reading the returned Operate field.
- `BishopPacketGuard.h` contains portable, no-allocation primitives.

**No protocol opcodes, wire structs or ABI packing were changed.**
The original Chinese-source file bytes were preserved using a Latin-1
byte roundtrip, not re-encoding them as UTF-8.

## Test (standalone, network-free)

```sh
cmake -S legacy_jx/tests/bishop_packet_guard -B /tmp/jx-bishop -G Ninja
cmake --build /tmp/jx-bishop
ctest --test-dir /tmp/jx-bishop --output-on-failure
```

The test includes negative cases for null/empty/short/oversized packet
views, misaligned 32-bit read, integer-overflow offsets, non-NUL fixed
strings, and path traversal attempts. It uses always-on `CHECK` rather
than Release-disabled `assert`.

The gate also requires MSVC `Release|x64` complete source rebuild
with `PostBuildEventUseInBuild=false` to suppress the original project's
copy-into-bin post-build script. Older external `resource.hm`, `lock.ico`,
`common.lib` and `engine.lib` were used temporarily in an isolated
worktree for linking, **not** added to Git. New Bishop.exe is test-only
and must not replace any running instance.

## Not yet covered — separate acceptance gates

- The tests validate guard primitives; they are **not** full actual
  Bishop handler packet fuzz or end-to-end login with the original PC
  JX V9 client, which requires a real client/protocol fixture.
- AccountServer authentication remains legacy and must be reviewed for
  replay, session binding, credential protection and TLS/trust decisions.
- Both Bishop listeners currently bind `INADDR_ANY` (client 56722,
  GameServer 56732 in the inspected DEV setup). A separate network/peer
  authentication and firewall review is required before Internet exposure.
- Other transport protocols and large fragmented packet reassembly need
  separate structured fuzz tests and packet-length caps.
- Handshake compatibility between the 34-byte Bishop greeting and the
  42-byte original Rainbow V9 client handler is still a known unresolved
  integration issue; these guards do not resolve it.
- Windows x64 compilability does not imply successful runtime login, and
  this patch is NOT deployed to the running Bishop service.

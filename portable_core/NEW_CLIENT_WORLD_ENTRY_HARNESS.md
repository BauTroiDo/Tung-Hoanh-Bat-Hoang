# NEW JX1 3D client: offline AuthService-to-AOI owner integration gate

This test specifically resumes the previously unfinished **gameplay path**
after replacing original Client V9 with a completely new PC/Web/Mobile
client contract. It is a **test harness**, not a new socket listener or
a deployed GameServer.

`tests/new_client_world_entry_tests.cpp` composes the real,
already-merged portable classes:

1. **Auth issuer** emits two OS-CSPRNG tickets from authenticated
   server-side identities. These test identities are fixtures; the
   test does not implement credential verification.
2. **WorldGateway** atomically consumes both tickets, bound to
   separate server-chosen connection IDs; replay and realm mismatch
   are rejected.
3. **World owner** builds two `TrustedWorldContext` records from
   validated sessions and server-selected character keys.
4. **AOI** upserts both characters, prepares visibility events and
   admits a **synthetic frame placeholder** into the bounded send pool.
   The frame is not a network codec or actual new-client packet.
5. An authenticated application-level ACK posted to the world owner
   queue commits the prepared AOI state. A later verified login for the
   same account revokes its prior generation; the old context is
   detached, stale ACKs do not commit, and a replacement connection
   receives a full AOI reset.
6. Both sessions and account tickets are closed/revoked cleanly.

## Verified scope versus open runtime work

This is a single-process offline integration of real C++17
`AuthTicketAuthority`, `WorldServerIntegrationAdapter`, and
`SpatialAoi`, with no mocks of these classes but **without** genuine
network, real passwords, GameServer entity registry, packet codec,
map coordinate conversion or PC/Web/Mobile client binaries.

The WorldServer adapter remains **disabled by default** in ordinary
construction; the harness explicitly enables it for testing. The
old Kingsoft `KSubWorldSet::Activate` path in
`legacy_jx/gameserver/Sources/Core/Src/KSubWorldSet.cpp` calls
`SubWorld[i].Activate()` in a sequential loop (around lines 622–628);
each `KSubWorld::Activate` updates region/NPC/gameplay state. These
are candidates for the real owner-thread integration, **not yet hooked**.

Next real integration milestone: establish a reviewed server-controlled
mapping of JX character and NPC identities + coordinates into
`EntityKey` and `SpatialAoi`, wire true bounded new-client
transport serialization/ACK, and run two actual new clients on the
DEV server. Keep the Graviti/Qoder legacy source worktrees intact
during integration; user requested completing playable game first,
not a wholesale server redesign.

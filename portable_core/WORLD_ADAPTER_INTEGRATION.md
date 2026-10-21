# JX1 3D — WorldServer/AOI integration adapter (foundation)

This slice resumes the unfinished **gameplay WorldRuntime integration**.
It selectively incorporates already tested portable world-adapter source from
the clean `ai/chatgpt-platform` reference worktree into GitHub main's
`portable_core`, without overwriting any newer `main` source or AuthTicket
code. Main's `AoiSessionBridge` gained only the required
`accessStatus()` and `invalidateForResync()` forwarders.

## Included portable C++17 modules

- `AoiDeliveryBudget`: one trusted viewer session's exact-frame byte
  admission, retry counters/timeouts, ACK requirement and resync.
- `AoiDeliveryPool`: bounded sessions/pending bytes/per-tick network
  scheduling; fairness and reliable cleanup on ACK or disconnect.
- `WorldServerIntegrationAdapter`: owned on a **single authoritative
  WorldServer thread**. Requires trusted server-origin identity and
  scene/connection generations, marshals foreign-thread ACK/failure
  callbacks into a bounded owner queue, denies stale ACKs after reconnect,
  triggers forced resync on uncertain failures/queue overflow.
- The adapter is **OFF by default** until explicitly constructed with
  `enabled=true`, and no legacy `CoreServer.dll` or GameServer binary
  is linked or changed in this PR.
- No wire opcodes, client V9 compatibility, Bishop or gameplay behavior is
  added. The JX1 3D client is the **new** PC/Web/Mobile client.

## Required next gameplay hookup

1. On the real GameServer/WorldServer owner loop, construct and bind the
   adapter explicitly, with reviewed hard quotas. Do not attach sessions
   directly from a network callback.
2. After AuthService/WorldGateway has **actually verified** a new-client
   session, create trusted `TrustedWorldContext` from server-owned IDs;
   not from client-supplied role IDs/generations. Attach/detach only on
   WorldServer owner thread. Invalidate stale generations on reconnect/map
   transitions.
3. Feed canonical NPC/player/entity positions into `SpatialAoi` from
   the authoritative world tick. Define coordinate units/map ID mapping
   before translating legacy Kingsoft coordinates into the new 3D space.
4. Encode AOI replication events with a separately vetted bounded
   **new-client** transport codec. Admission requires actual encoded
   bytes, not estimated payload sizes. Send synchronously or copy into
   a bounded transport queue, and only acknowledge from an authenticated
   application-level ACK on the corresponding session.
5. Add E2E tests using two real new JX1 3D client sessions: enter map,
   visibility spawn/despawn, movement, reconnect, map teleport, retries
   and disconnect. Then move from default-OFF to approved DEV runtime.

## Verification status

Build with the existing `portable_core/CMakeLists.txt` on Windows x64 and
Ubuntu; new tests `world_server_adapter_tests`,
`aoi_delivery_budget_tests`, `aoi_delivery_pool_tests` run beside
the original AuthTicket/WorldRuntime/SpatialAOI tests.

All tests in this module are **offline portable unit tests**.
The integration adapter is not yet attached to the live legacy GameServer,
and no gameplay runtime acceptance has been performed.

Do not merge Graviti's unfinished 349-helper legacy changes or alter
dirty worktrees during this source-only integration.

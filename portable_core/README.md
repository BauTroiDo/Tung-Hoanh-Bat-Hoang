# Portable Core and World Runtime Foundation (CORE-01/02/37/38) — Version 0.2

STATUS: isolated implementation + unit-tested prototype on Windows (MSVC/GCC) and Linux (Ubuntu WSL GCC). NOT linked into the legacy GameServer, NOT a full JX Linux port, NOT a production Code Generator.

Provides an original C++17 library with:
- monotonic clock interface based on std::chrono::steady_clock;
- bounds-checked explicit little-endian binary primitives including u64;
- length-prefixed byte-string read/write with strict bounds;
- tests for golden endian bytes, malformed input, no partial read/write, 10000 deterministic fuzz samples;
- no Win32 or POSIX platform API includes in module source.
- **WorldRuntime (CORE-01/02/37 prototype):** bounded multi-producer/single-owner command queue, non-blocking backpressure, explicit owner binding, exception-isolated draining and integer-accurate monotonic 18 Hz tick accounting with bounded catch-up. This is intentionally NOT linked into the legacy WorldServer yet.

### Spatial AOI visibility foundation (CORE-06/07)

- `mmo::portable::SpatialAoi` is a world-thread-owned **2D cell grid + exact-radius filter**. It uses immutable `EntityKey` (runtimeId, generation, worldId, instanceId) to isolate maps/instances and guard ID reuse.
- `AoiObserverView` retains which entities each player already knows, with scene epoch. Plans deterministic `Reset` / `Remove` / `Create` / `Update` deltas.
- Query limits `maxViewRadius`, `maxQueryCells`, `maxVisible`, `maxEvents` reject excessive visibility work **without changing the observer snapshot**. Numeric radius and coordinate computations have overflow guards; negative-grid positions use true floor division.
- Stale generations cannot respawn after deletion until a newer generation is used. Updating position/visibility requires a strictly higher entity revision.
- Per-producer/world-owner boundaries are explicit: call `bindOwnerToCurrentThread()` and perform upsert, erase and plan on the bound world thread; never use it as a concurrently mutable registry.
- **Important integration limitations:** AOI is not yet attached to JX `KRegion`/`KSubWorld` or network packets; no reliable delivery/ack/resync strategy, view ownership binding to authenticated sessions, line of sight, 3D height filter, entity history eviction, full world pressure limits or production metrics. Server-authoritative coordinates and scene epochs must be supplied by integration. Planner updating observer-known state must be coordinated with successful network send, or a replay/resync mechanism is necessary.

### AOI Session Bridge — authenticated-context adapter (CORE-07 / CORE-35 test harness)

- `AoiSessionBridge` is a **portable integration contract**, not JX GameServer integration. It wraps the stop-and-wait replicator and requires a valid viewer key plus monotonic, server-controlled `connectionGeneration` and `sceneEpoch`.
- An ACK must be supplied by the **authenticated server transport context** with matching viewer, connection generation, scene epoch and in-flight sequence. Wrong identifiers are rejected before commit; a change of scene/connection invalidates staged and committed state, forcing a new Reset snapshot.
- Identity fields are **not security credentials**. Never trust a client-supplied connection generation, session binding, viewer key or scene epoch. Authenticate the socket/transport and resolve the authoritative session on the world owner thread before calling `acknowledge`.
- `advanceScene`/`advanceConnection` only accept increasing nonzero generations. Teleporting to another world/instance or reusing a viewer runtime ID requires a **new bridge** with a new immutable `EntityKey`. Do not reuse connections after generation wrap.
- Event budgeting remains with `SpatialAoi`/`AoiReplicationSession`; **no byte-accurate packet-size limits, global connection admission, authenticated wire ACK, retry policy, or live JX transport integration yet**. Production enabling requires those plus replay/DoS review, metrics and feature-flag rollback.
- Dedicated `aoi_session_bridge_tests` covers wrong viewer/connection/scene/sequence, stale ACK after transitions, resend/reset behavior and foreign-thread rejection. Test in MSVC, Windows GCC and Linux before integration.

### AOI Replication / Scene Synchronization (CORE-07 / CORE-35 prototype)

- `AoiReplicationSession` wraps `SpatialAoi` with **prepare -> retry/acknowledge -> commit**. A prepared delta does NOT advance committed client-known entities until trusted delivery confirmation.
- At most one in-flight batch per viewer (stop-and-wait); retries reuse the exact sequence and events to support deduplication. `invalidateForResync()` discards uncertain state so the next prepare creates a full Reset+Create snapshot.
- Uses monotonic nanoseconds for timeout detection, validates scene epoch and sequence numbers, rejects wrong-owner thread calls and delegates per-client event budgeting to the AOI planner.
- **Important:** `acknowledge(seq)` is NOT an authenticated network protocol. Only call after verified transport/client acknowledgement bound to the authenticated session. A successful TCP send()/queue operation alone is NOT proof that the client applied the packet. On uncertain delivery, disconnect, teleport or sequence mismatch, resync is required.
- **Not yet production-ready:** reliable wire encoding/ack/dedup, explicit retry policy, bounded per-client memory/backpressure, global admission limit, client capability negotiation, anti-replay and session-lifetime binding, E2E JX packet compatibility.

The code is self-contained and not linked into the existing JX server.
Out-of-tree CMake build required; do not place build output in legacy JX bin/lib/Release directories.

Note that byte strings are NOT validated UTF-8 here; generated semantic validators will do so.
Do not use extracted opcode enums to infer production packet lengths or layouts.
Verified on Ubuntu 26.04 WSL using GCC 15.2 / CMake 4.2 / Ninja: Linux x86-64 ELF executable built and both CTest suites passed. AddressSanitizer + UndefinedBehaviorSanitizer both passed; see `Document-ThietKe/PORTABLE_CORE_LINUX_VERIFICATION_20261019.md`. This is a native Linux binary executing inside WSL, NOT proof that the legacy JX WorldServer builds/runs on a standalone Linux host.

The three CTest executables test the portable codec, randomized property cases, and owner-thread world runtime. WorldRuntime verifies multi-producer/single-consumer FIFO, backpressure, shutdown with queued commands, callback exception isolation, reentrant drain refusal and exact 18 Hz integer tick accounting. 12,000 worker-posted callbacks are checked for at-most-once execution and per-producer ordering.

The two original codec CTest executables test deterministic golden little-endian bytes, invalid/short buffer reads, a fixed-seed 10,000-input smoke test, 25,000 randomized integer/string round trips, truncated byte strings, null bytes, and strict buffer bounds. Run tests with your chosen OS/compiler before use in a larger server.

**World integration contract:** Bind `WorldCommandQueue` once on the authoritative world loop thread; network/database worker threads submit commands containing validated immutable values or stable entity keys (never raw `Npc[]`/`Player[]` pointers). Drain with a fixed per-tick command limit; inspect `DrainResult` for attempted, failed, wrongOwner, unbound, reentrant and remaining; report queue fullness as backpressure. Maintain the legacy 18 Hz tick until independent benchmarks justify a change. `TickBudget` is a scheduling primitive, not a time sleeping loop; it reports skipped tick counts and must be integrated behind a feature flag only after the Qoder-owned JX runtime is stable.

**Production gaps:** callback failure diagnostics, queue/pacer metrics, fairness between traffic classes, actor lifetime/resolution, state replay, bounded wall-time per tick, compatibility load testing and live JX integration are NOT implemented. A callback throwing is counted and suppressed; the caller must add diagnostics and safe recovery before using in production.

Linux WSL build (from the PC): `cmake -S portable_core -B <out-of-tree> -G Ninja -DCMAKE_BUILD_TYPE=Debug && cmake --build <out-of-tree> && ctest --test-dir <out-of-tree> --output-on-failure`. Sanitizer flags: `-fsanitize=address,undefined` for compiler and linker. No JX runtime linking has occurred.

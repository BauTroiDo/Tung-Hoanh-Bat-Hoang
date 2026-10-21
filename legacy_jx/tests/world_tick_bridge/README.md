# GAME-01 — optional Kingsoft CoreServer world-owner tick seam

The first **source-level** link towards the new JX1 3D WorldRuntime is
now available on the legacy Kingsoft Win64 CoreServer world update path.

## Exact hook

In `Src/KSubWorldSet.cpp`, `KSubWorldSet::MainLoop()` runs every
`SubWorld[i].Activate()` before incrementing `m_dwCurrentTime`.
Immediately after that increment the source includes:

```cpp
#if defined(_SERVER) && defined(JX3D_ENABLE_LEGACY_WORLD_TICK_HOOK)
    Jx3dNotifyWorldTick(
        static_cast<uint32_t>(m_dwCurrentTime),
        static_cast<uint32_t>(MAX_SUBWORLD));
#endif
```

**The define is ABSENT from all project configurations by default.**
Compiling the existing CoreServer does not execute a callback.
The `Core.vcxproj` compiles `Src/Jx3dLegacyWorldTickBridge.cpp`,
a small, standalone C++11 bridge exporting an **explicit C ABI**:

- `Jx3dRegisterWorldTickCallback(callback, context)` — register once
  from a trusted server host **before the owner loop starts**.
  Duplicate/null registration fails. Callback and context must stay
  valid for the lifetime of CoreServer (no runtime hot-unload).
- `Jx3dNotifyWorldTick(completedLegacyFrame, subWorldSlots)` —
  synchronous metadata-only owner-tick signal. Never call this
  from IOCP/network worker threads. **No NPC/player pointer, map
  coordinates, character or client data is currently copied.**
- `Jx3dWorldTickFaults()` — counts exceptions raised by the
  callback. The first exception disables further invocations rather
  than propagating into Kingsoft's gameplay loop.

This registration interface is for the future trusted WorldServer host,
**not** a client opcode, listener or account endpoint. It does not
currently instantiate or drive `WorldServerIntegrationAdapter`:
the safe gameplay/identity/coordinate transfer is the next slice.

## Independent test

`legacy_jx/tests/world_tick_bridge` builds the **actual bridge cpp**
on MSVC Win64 and native Ubuntu, using `CMakeLists.txt`.
`world_tick_bridge_test.cpp` asserts default no-op, one registration,
callback values, replacement rejection, exception containment and
disabled-on-failure behavior.

The **real** `Core.vcxproj` compiles the surgically modified
`KSubWorldSet.cpp` and new bridge CPP in `Server Release|x64`
using Visual Studio 18 (MSBuild `ClCompile` exit 0).
Both object files have COFF machine `0x8664`. The owner loop was
compiled a second time with the opt-in macro provided in the MSVC
`CL` environment: `Jx3dNotifyWorldTick` was verified as an actual
undefined external relocation in `KSubWorldSet.obj` using `dumpbin`.
The optional branch therefore compiles, but is **not enabled** in the
normal project configuration.
The GitHub checkout is missing generated legacy `msado15.tlh` and
`msado15.tli`; for offline Core compilation only, the exact
local reference files were SHA-copied into the **isolated worktree**
and removed after verification. They are not in the commit.

## Not yet implemented / rollout HOLD

1. No stable mapping of legacy player/NPC/item IDs, SubWorld/map,
   instance/scene generations or tile/3D coordinate units.
2. No transfer from the live CoreServer tick to the native
   `WorldServerIntegrationAdapter` / `SpatialAoi`.
3. No real client PC/Web/Mobile transport framing, login network service,
   packet ACK or two-user gameplay E2E.
4. This is source-slice compilation + portable unit tests,
   **not** a full relink of GameServer/CoreServer or a live DEV deploy.
5. Enabling the macro requires deliberate review of the host registration,
   owner-thread affinity, bounded data copies, callback lifetime,
   CPU/time budgets and failure policy. Do not enable in DEV by default.
6. Do not incorporate Graviti's 349 UNKNOWN_HELPER changes or merge
   the original dirty `jxOnline` and `chatgpt-jx-integration`
   checkouts wholesale.

Roadmap after this: GAME-02 trusted entity snapshot/mapping, then
real World/AOI adapter owner tick; GAME-03 new-client networking and
two-client spawn/movement/visibility acceptance. No original Client V9
is required for the new JX1 3D client.

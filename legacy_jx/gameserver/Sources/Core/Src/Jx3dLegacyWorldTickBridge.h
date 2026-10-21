#ifndef JX3D_LEGACY_WORLD_TICK_BRIDGE_H
#define JX3D_LEGACY_WORLD_TICK_BRIDGE_H

// C ABI seam between the legacy CoreServer.dll and a future JX1 3D world
// adapter. Registration is allowed once at startup, before the world loop.
// The callback and its context must remain valid until the process exits.
// No packet parsing, network I/O or legacy gameplay mutations are performed.
#include <stdint.h>

#if defined(_WIN32)
#define JX3D_TICK_CALL __cdecl
#if defined(CORE_EXPORTS)
#define JX3D_TICK_EXPORT __declspec(dllexport)
#else
#define JX3D_TICK_EXPORT
#endif
#else
#define JX3D_TICK_CALL
#define JX3D_TICK_EXPORT
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef void (JX3D_TICK_CALL *Jx3dWorldTickCallback)(
    uint32_t completedLegacyFrame, uint32_t subWorldSlots, void* context);

// Server host must register exactly once before the world owner loop starts.
// Duplicate/NULL registration is rejected without replacing the callback.
JX3D_TICK_EXPORT bool JX3D_TICK_CALL Jx3dRegisterWorldTickCallback(
    Jx3dWorldTickCallback callback, void* context);

// Called ONLY by the authoritative legacy world-owner tick, after all
// SubWorld[i].Activate() calls. Failure cannot unwind legacy gameplay.
// This reports frame metadata only, not entity positions or identities.
JX3D_TICK_EXPORT void JX3D_TICK_CALL Jx3dNotifyWorldTick(
    uint32_t completedLegacyFrame, uint32_t subWorldSlots);

// Nonzero indicates callback raised a C++ exception and was disabled.
JX3D_TICK_EXPORT uint32_t JX3D_TICK_CALL Jx3dWorldTickFaults(void);

// Experimental snapshot contract: separate, default-OFF compile flag.
// The producer copies *values* on the world tick, never KPlayer*/KNpc*.
// Coordinates are legacy 2D map pixels, NOT a 3D position. The SubWorld
// array slot and configured map ID are deliberately separate fields.
// sourceNpcId is process-local and may wrap; npcSlot is reusable. Neither
// is a globally persistent EntityKey. Generation/sceneEpoch are NOT inferred.
#define JX3D_SNAPSHOT_MAX_ENTITIES 512u
typedef struct Jx3dLegacyEntitySample {
    uint32_t npcSlot;
    uint32_t sourceNpcId;
    uint32_t mapSlot;
    uint32_t mapId;
    uint32_t playerSlot; // zero for non-player and unverified mapping
    uint32_t playerId;   // zero for non-player and unverified mapping
    uint32_t kind;
    int32_t pixelX;
    int32_t pixelY;
    int32_t direction;
} Jx3dLegacyEntitySample;

// Borrowed samples are valid ONLY for this synchronous callback. Copy them
// before returning; never store a pointer to this stack-owned batch.
// complete=false MUST NOT be reconciled as an authoritative full frame:
// exceeding the bounded budget could otherwise cause false AOI despawns.
typedef void (JX3D_TICK_CALL *Jx3dWorldSnapshotCallback)(
    uint32_t frame, const Jx3dLegacyEntitySample* samples,
    uint32_t count, bool complete, void* context);

// Register exactly once at startup. Bounded [1, 512] rows. Callback and
// context must remain alive for process lifetime. No network or gameplay
// mutations from callback. No registration means producer is inert.
JX3D_TICK_EXPORT bool JX3D_TICK_CALL Jx3dRegisterWorldSnapshotCallback(
    Jx3dWorldSnapshotCallback callback, void* context, uint32_t maxRows);
JX3D_TICK_EXPORT uint32_t JX3D_TICK_CALL Jx3dWorldSnapshotLimit(void);
// Only the legacy world owner calls this, after SubWorld Activate.
// Fails closed on bad arguments or a throwing callback.
JX3D_TICK_EXPORT void JX3D_TICK_CALL Jx3dPublishWorldSnapshot(
    uint32_t frame, const Jx3dLegacyEntitySample* samples,
    uint32_t count, bool complete);
JX3D_TICK_EXPORT uint32_t JX3D_TICK_CALL Jx3dWorldSnapshotFaults(void);

#ifdef __cplusplus
}
#endif
#endif

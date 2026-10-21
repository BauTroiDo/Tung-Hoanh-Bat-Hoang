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

#ifdef __cplusplus
}
#endif
#endif

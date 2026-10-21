#include "Jx3dLegacyWorldTickBridge.h"

#include <atomic>
#include <mutex>

namespace {
struct WorldTickHookState {
    std::mutex registrationMutex;
    std::atomic<Jx3dWorldTickCallback> callback{nullptr};
    void* context = nullptr;  // immutable after callback publication
    std::atomic<uint32_t> faults{0};
    std::atomic<Jx3dWorldSnapshotCallback> snapshotCallback{nullptr};
    void* snapshotContext = nullptr; // immutable after callback publication
    uint32_t snapshotMaxRows = 0;
    std::atomic<uint32_t> snapshotFaults{0};
};

WorldTickHookState& state() {
    static WorldTickHookState instance;
    return instance;
}
} // namespace

extern "C" bool JX3D_TICK_CALL Jx3dRegisterWorldTickCallback(
    Jx3dWorldTickCallback callback, void* context) {
    if (!callback) return false;
    auto& s = state();
    std::lock_guard<std::mutex> hold(s.registrationMutex);
    if (s.callback.load(std::memory_order_acquire)) return false;
    s.context = context;
    s.callback.store(callback, std::memory_order_release);
    return true;
}

extern "C" void JX3D_TICK_CALL Jx3dNotifyWorldTick(
    uint32_t completedLegacyFrame, uint32_t subWorldSlots) {
    auto& s = state();
    const auto callback = s.callback.load(std::memory_order_acquire);
    if (!callback || s.faults.load(std::memory_order_acquire)) return;
    try {
        callback(completedLegacyFrame, subWorldSlots, s.context);
    } catch (...) {
        // Fail closed: never allow a JX3D adapter callback to unwind the
        // legacy server owner loop. The callback stays disabled.
        s.faults.fetch_add(1, std::memory_order_acq_rel);
    }
}

extern "C" uint32_t JX3D_TICK_CALL Jx3dWorldTickFaults(void) {
    return state().faults.load(std::memory_order_acquire);
}

extern "C" bool JX3D_TICK_CALL Jx3dRegisterWorldSnapshotCallback(
    Jx3dWorldSnapshotCallback callback, void* context, uint32_t maxRows) {
    if (!callback || !maxRows || maxRows > JX3D_SNAPSHOT_MAX_ENTITIES)
        return false;
    auto& s = state();
    std::lock_guard<std::mutex> hold(s.registrationMutex);
    if (s.snapshotCallback.load(std::memory_order_acquire)) return false;
    s.snapshotContext = context;
    s.snapshotMaxRows = maxRows;
    s.snapshotCallback.store(callback, std::memory_order_release);
    return true;
}

extern "C" uint32_t JX3D_TICK_CALL Jx3dWorldSnapshotLimit(void) {
    auto& s = state();
    if (!s.snapshotCallback.load(std::memory_order_acquire) ||
        s.snapshotFaults.load(std::memory_order_acquire)) return 0;
    return s.snapshotMaxRows;
}

extern "C" void JX3D_TICK_CALL Jx3dPublishWorldSnapshot(
    uint32_t frame, const Jx3dLegacyEntitySample* samples,
    uint32_t count, bool complete) {
    auto& s = state();
    const auto callback = s.snapshotCallback.load(std::memory_order_acquire);
    if (!callback || s.snapshotFaults.load(std::memory_order_acquire)) return;
    if (count > s.snapshotMaxRows || (count != 0 && !samples)) {
        s.snapshotFaults.fetch_add(1, std::memory_order_acq_rel);
        return;
    }
    try {
        callback(frame, samples, count, complete, s.snapshotContext);
    } catch (...) {
        // Never unwind through the legacy owner loop; disable this callback.
        s.snapshotFaults.fetch_add(1, std::memory_order_acq_rel);
    }
}

extern "C" uint32_t JX3D_TICK_CALL Jx3dWorldSnapshotFaults(void) {
    return state().snapshotFaults.load(std::memory_order_acquire);
}

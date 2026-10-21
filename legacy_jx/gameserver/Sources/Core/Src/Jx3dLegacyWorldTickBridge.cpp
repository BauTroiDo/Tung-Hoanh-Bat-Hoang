#include "Jx3dLegacyWorldTickBridge.h"

#include <atomic>
#include <mutex>

namespace {
struct WorldTickHookState {
    std::mutex registrationMutex;
    std::atomic<Jx3dWorldTickCallback> callback{nullptr};
    void* context = nullptr;  // immutable after callback publication
    std::atomic<uint32_t> faults{0};
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

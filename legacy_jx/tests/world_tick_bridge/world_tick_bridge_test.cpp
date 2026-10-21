#include "../../gameserver/Sources/Core/Src/Jx3dLegacyWorldTickBridge.h"

#include <cstdio>
#include <stdexcept>

#define CHECK(x) do { if (!(x)) { std::fprintf(stderr, "FAILED line %d: %s\n", __LINE__, #x); return 1; } } while (false)

struct TickFixture {
    int count{0};
    uint32_t lastFrame{0};
    uint32_t lastSlots{0};
};

static void JX3D_TICK_CALL onWorldTick(uint32_t frame,
                                      uint32_t slots,
                                      void* context) {
    auto* const f = static_cast<TickFixture*>(context);
    ++f->count;
    f->lastFrame = frame;
    f->lastSlots = slots;
    if (frame == 13) throw std::runtime_error("test callback failure");
}

static void JX3D_TICK_CALL otherTick(uint32_t, uint32_t, void*) {}

int main() {
    TickFixture first{};
    Jx3dNotifyWorldTick(1, 32);
    CHECK(first.count == 0);
    CHECK(Jx3dWorldTickFaults() == 0);
    CHECK(!Jx3dRegisterWorldTickCallback(nullptr, &first));
    CHECK(Jx3dRegisterWorldTickCallback(&onWorldTick, &first));
    CHECK(!Jx3dRegisterWorldTickCallback(&otherTick, nullptr));

    Jx3dNotifyWorldTick(10, 64);
    CHECK(first.count == 1);
    CHECK(first.lastFrame == 10);
    CHECK(first.lastSlots == 64);
    Jx3dNotifyWorldTick(11, 32);
    CHECK(first.count == 2);
    CHECK(first.lastFrame == 11);
    CHECK(first.lastSlots == 32);
    CHECK(Jx3dWorldTickFaults() == 0);

    // No exception may escape into the existing Kingsoft tick loop.
    Jx3dNotifyWorldTick(13, 100);
    CHECK(Jx3dWorldTickFaults() == 1);
    CHECK(first.count == 3);
    Jx3dNotifyWorldTick(14, 100);
    CHECK(first.count == 3);
    CHECK(first.lastFrame == 13);
    std::puts("JX3D_WORLD_TICK_BRIDGE_PASS");
    return 0;
}

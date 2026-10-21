#include "../../gameserver/Sources/Core/Src/Jx3dLegacyWorldTickBridge.h"

#include <cstdio>
#include <cstring>
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

struct SnapshotFixture {
    int calls{0};
    uint32_t lastFrame{0};
    uint32_t lastCount{0};
    bool lastComplete{false};
    Jx3dLegacyEntitySample first{};
};
static void JX3D_TICK_CALL onSnapshot(uint32_t frame,
    const Jx3dLegacyEntitySample* samples, uint32_t count,
    bool complete, void* context) {
    auto* const f = static_cast<SnapshotFixture*>(context);
    ++f->calls;
    f->lastFrame = frame;
    f->lastCount = count;
    f->lastComplete = complete;
    if (count) f->first = samples[0]; // owned copy, never store samples
    if (frame == 77) throw std::runtime_error("snapshot callback failure");
}

int main(int argc, char** argv) {
    if (argc > 1) {
        SnapshotFixture invalid{};
        CHECK(Jx3dRegisterWorldSnapshotCallback(&onSnapshot, &invalid, 1));
        Jx3dLegacyEntitySample value{};
        if (std::strcmp(argv[1], "overlimit") == 0)
            Jx3dPublishWorldSnapshot(88, &value, 2, false);
        else
            Jx3dPublishWorldSnapshot(88, nullptr, 1, false);
        CHECK(invalid.calls == 0);
        CHECK(Jx3dWorldSnapshotFaults() == 1);
        CHECK(Jx3dWorldSnapshotLimit() == 0);
        Jx3dPublishWorldSnapshot(89, &value, 1, true);
        CHECK(invalid.calls == 0);
        return 0;
    }
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
    // The snapshot registration is independent of the legacy tick hook.
    SnapshotFixture snapshot{};
    Jx3dPublishWorldSnapshot(1, nullptr, 0, true); // default OFF
    CHECK(snapshot.calls == 0);
    CHECK(Jx3dWorldSnapshotLimit() == 0);
    CHECK(!Jx3dRegisterWorldSnapshotCallback(nullptr, &snapshot, 1));
    CHECK(!Jx3dRegisterWorldSnapshotCallback(&onSnapshot, &snapshot, 0));
    CHECK(!Jx3dRegisterWorldSnapshotCallback(&onSnapshot, &snapshot,
        JX3D_SNAPSHOT_MAX_ENTITIES + 1));
    CHECK(Jx3dRegisterWorldSnapshotCallback(&onSnapshot, &snapshot, 2));
    CHECK(!Jx3dRegisterWorldSnapshotCallback(&onSnapshot, &snapshot, 2));
    CHECK(Jx3dWorldSnapshotLimit() == 2);
    Jx3dLegacyEntitySample sample{};
    sample.npcSlot = 41;
    sample.sourceNpcId = 950;
    sample.mapSlot = 3;
    sample.mapId = 2001;
    sample.pixelX = 1024;
    sample.pixelY = 2048;
    sample.direction = 9;
    Jx3dPublishWorldSnapshot(70, &sample, 1, true);
    CHECK(snapshot.calls == 1 && snapshot.lastCount == 1);
    CHECK(snapshot.lastComplete && snapshot.lastFrame == 70);
    CHECK(snapshot.first.npcSlot == 41 && snapshot.first.sourceNpcId == 950);
    CHECK(snapshot.first.mapSlot == 3 && snapshot.first.mapId == 2001);
    sample.pixelX = -777; // callback must already own the previous value
    CHECK(snapshot.first.pixelX == 1024 && snapshot.first.pixelY == 2048);
    Jx3dPublishWorldSnapshot(71, &sample, 1, false);
    CHECK(snapshot.calls == 2 && !snapshot.lastComplete);
    CHECK(snapshot.first.pixelX == -777);
    Jx3dPublishWorldSnapshot(77, nullptr, 0, false); // throws, fail closed
    CHECK(snapshot.calls == 3 && Jx3dWorldSnapshotFaults() == 1);
    CHECK(Jx3dWorldSnapshotLimit() == 0);
    Jx3dPublishWorldSnapshot(78, &sample, 1, true);
    CHECK(snapshot.calls == 3);
    CHECK(Jx3dWorldTickFaults() == 1); // separate fault counters
    std::puts("JX3D_WORLD_TICK_BRIDGE_PASS");
    return 0;
}

#include "mmo/AoiSessionBridge.h"
#include <atomic>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <thread>

using namespace mmo::portable;
#define CHECK(e) do { if (!(e)) { std::cerr << "FAIL at line " << __LINE__ << ": " #e "\n"; return 1; } } while(false)

static EntityKey key(std::uint64_t id, std::uint32_t generation = 1) {
    return {id, generation, 500, 7};
}
static bool contains(const ReplicationBatch& batch, AoiDelta::Kind kind) {
    for (const auto& e : batch.events) if (e.kind == kind) return true;
    return false;
}
int main() {
    bool rejected = false;
    try { AoiSessionBridge invalid(key(1), 0, 10); }
    catch (const std::invalid_argument&) { rejected = true; }
    CHECK(rejected);
    rejected = false;
    try { AoiSessionBridge invalid(key(1), 100, 0); }
    catch (const std::invalid_argument&) { rejected = true; }
    CHECK(rejected);

    SpatialAoi world(100, 500, 100, 100);
    AoiSessionBridge bridge(key(1), 100, 10, 16);
    const AuthenticatedAoiAck firstAck{key(1), 100, 10, 1};
    CHECK(bridge.prepare(world, 0, 0, 200, 100).status == ReplicationStatus::Unbound);
    CHECK(bridge.acknowledge(firstAck) == ReplicationStatus::Unbound);
    CHECK(!bridge.advanceScene(11));
    CHECK(bridge.sceneEpoch() == 10);
    CHECK(world.bindOwnerToCurrentThread());
    CHECK(bridge.bindOwnerToCurrentThread());
    CHECK(world.upsert({key(1), 0, 0, 1, true}) == AoiStatus::Ok);
    CHECK(world.upsert({key(2), 10, 10, 1, true}) == AoiStatus::Ok);

    auto first = bridge.prepare(world, 0, 0, 200, 100);
    CHECK(first.status == ReplicationStatus::Prepared);
    CHECK(first.batch.sequence == 1 && first.batch.sceneEpoch == 10);
    CHECK(contains(first.batch, AoiDelta::Kind::Reset));
    CHECK(contains(first.batch, AoiDelta::Kind::Create));
    CHECK(bridge.committedVisibleCount() == 0);
    CHECK(bridge.hasInFlight());
    CHECK(bridge.timedOut(120, 20));

    CHECK(bridge.acknowledge({key(99), 100, 10, 1}) == ReplicationStatus::WrongViewer);
    CHECK(bridge.acknowledge({key(1), 99, 10, 1}) == ReplicationStatus::WrongConnection);
    CHECK(bridge.acknowledge({key(1), 100, 9, 1}) == ReplicationStatus::WrongEpoch);
    CHECK(bridge.acknowledge({key(1), 100, 10, 2}) == ReplicationStatus::WrongSequence);
    CHECK(bridge.committedVisibleCount() == 0);
    CHECK(bridge.acknowledge(firstAck) == ReplicationStatus::Prepared);
    CHECK(bridge.committedVisibleCount() == 1 && !bridge.hasInFlight());
    CHECK(bridge.lastAcknowledgedSequence() == 1);

    CHECK(world.upsert({key(2), 20, 20, 2, true}) == AoiStatus::Ok);
    auto update = bridge.prepare(world, 0, 0, 200, 200);
    CHECK(update.status == ReplicationStatus::Prepared && update.batch.sequence == 2);
    CHECK(contains(update.batch, AoiDelta::Kind::Update));
    CHECK(!bridge.advanceScene(10) && !bridge.advanceScene(0));
    CHECK(bridge.hasInFlight());
    CHECK(bridge.advanceScene(11));
    CHECK(!bridge.hasInFlight() && bridge.committedVisibleCount() == 0);
    CHECK(bridge.acknowledge({key(1), 100, 10, 2}) == ReplicationStatus::WrongEpoch);
    CHECK(bridge.acknowledge({key(1), 100, 11, 2}) == ReplicationStatus::NotPending);

    auto newScene = bridge.prepare(world, 0, 0, 200, 300);
    CHECK(newScene.status == ReplicationStatus::Prepared && newScene.batch.sceneEpoch == 11);
    CHECK(newScene.batch.sequence == 3);
    CHECK(contains(newScene.batch, AoiDelta::Kind::Reset));
    CHECK(bridge.acknowledge({key(1), 100, 11, 3}) == ReplicationStatus::Prepared);
    CHECK(bridge.committedVisibleCount() == 1);

    CHECK(world.upsert({key(2), 22, 22, 3, true}) == AoiStatus::Ok);
    auto pending = bridge.prepare(world, 0, 0, 200, 350);
    CHECK(pending.status == ReplicationStatus::Prepared && pending.batch.sequence == 4);
    CHECK(!bridge.advanceConnection(100) && !bridge.advanceConnection(99));
    CHECK(bridge.advanceConnection(101));
    CHECK(!bridge.hasInFlight() && bridge.committedVisibleCount() == 0);
    CHECK(bridge.acknowledge({key(1), 100, 11, 4}) == ReplicationStatus::WrongConnection);
    CHECK(bridge.acknowledge({key(1), 101, 11, 4}) == ReplicationStatus::NotPending);
    auto recovered = bridge.prepare(world, 0, 0, 200, 400);
    CHECK(recovered.status == ReplicationStatus::Prepared && recovered.batch.sequence == 5);
    CHECK(contains(recovered.batch, AoiDelta::Kind::Reset));
    CHECK(bridge.acknowledge({key(1), 101, 11, 5}) == ReplicationStatus::Prepared);
    CHECK(bridge.committedVisibleCount() == 1);

    std::atomic<bool> foreignRejected{false};
    std::thread foreign([&] {
        foreignRejected =
            !bridge.bindOwnerToCurrentThread() &&
            bridge.prepare(world, 0, 0, 10, 500).status == ReplicationStatus::WrongThread &&
            bridge.acknowledge({key(1), 101, 11, 6}) == ReplicationStatus::WrongThread &&
            !bridge.advanceScene(12) &&
            !bridge.advanceConnection(102);
    });
    foreign.join();
    CHECK(foreignRejected && bridge.sceneEpoch() == 11);
    CHECK(bridge.connectionGeneration() == 101);

    std::cout << "PASS: authenticated session/scene gate, stale ACK isolation, "
                 "resync and world-thread ownership\n";
    return 0;
}

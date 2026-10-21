#include "mmo/AoiDeliveryBudget.h"
#include <atomic>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <thread>
#include <vector>

#define CHECK(e) do { if (!(e)) { std::cerr << "FAIL line " << __LINE__ << ": " #e "\n"; return 1; } } while(false)
using namespace mmo::portable;
namespace {
EntityKey key(std::uint64_t id) { return {id, 1, 300, 7}; }
}
int main() {
    bool rejected = false;
    try { AoiBudgetedSession bad(key(1), 5, 9, {0, 2, 50}); }
    catch (const std::invalid_argument&) { rejected = true; }
    CHECK(rejected);
    rejected = false;
    try { AoiBudgetedSession bad(key(1), 5, 9, {8, 0, 50}); }
    catch (const std::invalid_argument&) { rejected = true; }
    CHECK(rejected);
    rejected = false;
    try { AoiBudgetedSession bad(key(1), 5, 9, {8, 2, 0}); }
    catch (const std::invalid_argument&) { rejected = true; }
    CHECK(rejected);

    SpatialAoi world(100, 500, 100, 100);
    AoiBudgetedSession budget(key(1), 5, 9, {8, 2, 50}, 16);
    CHECK(budget.prepare(world, 0, 0, 100, 100).status == ReplicationStatus::Unbound);
    CHECK(budget.admit(1, {1}, 100) == AoiDeliveryStatus::Unbound);
    CHECK(budget.authorizeSendAttempt(1, 101) == AoiDeliveryStatus::Unbound);
    CHECK(budget.acknowledge({key(1),5,9,1}) == ReplicationStatus::Unbound);
    CHECK(!budget.invalidateForResync());
    CHECK(world.bindOwnerToCurrentThread());
    CHECK(budget.bindOwnerToCurrentThread());
    CHECK(world.upsert({key(1),0,0,1,true}) == AoiStatus::Ok);
    CHECK(world.upsert({key(2),30,20,1,true}) == AoiStatus::Ok);

    const auto first = budget.prepare(world, 0, 0, 100, 100);
    CHECK(first.status == ReplicationStatus::Prepared &&
          first.batch.events[0].kind == AoiDelta::Kind::Reset);
    CHECK(budget.acknowledge({key(1),5,9,first.batch.sequence}) ==
          ReplicationStatus::NotPending);
    CHECK(budget.admit(first.batch.sequence+1, {1}, 101) ==
          AoiDeliveryStatus::WrongSequence);
    CHECK(budget.admit(first.batch.sequence, std::vector<std::uint8_t>(9, 55), 101) ==
          AoiDeliveryStatus::FrameTooLarge);
    CHECK(budget.committedVisibleCount() == 0 && !budget.hasAdmittedFrame());
    CHECK(budget.authorizeSendAttempt(first.batch.sequence,102) ==
          AoiDeliveryStatus::NotAdmitted);

    // Failed encode/admission cannot commit view, next query is full RESET.
    const auto second = budget.prepare(world, 0, 0, 100, 200);
    CHECK(second.status == ReplicationStatus::Prepared);
    CHECK(second.batch.sequence != first.batch.sequence &&
          second.batch.events[0].kind == AoiDelta::Kind::Reset);
    CHECK(budget.admit(second.batch.sequence, std::vector<std::uint8_t>(8, 42), 201) ==
          AoiDeliveryStatus::Accepted);
    CHECK(budget.admittedBytes() == 8 && budget.hasAdmittedFrame());
    const auto expected = budget.admittedFrame();
    CHECK(budget.admit(second.batch.sequence, {9}, 202) ==
          AoiDeliveryStatus::AlreadyAdmitted);
    CHECK(budget.authorizeSendAttempt(first.batch.sequence, 203) ==
          AoiDeliveryStatus::WrongSequence);
    CHECK(budget.acknowledge({key(1),5,9,second.batch.sequence}) ==
          ReplicationStatus::NotPending); // no attempted transmission yet
    CHECK(budget.authorizeSendAttempt(second.batch.sequence, 204) ==
          AoiDeliveryStatus::Accepted);
    CHECK(budget.attemptCount() == 1 && budget.admittedFrame() == expected);
    CHECK(budget.acknowledge({key(99),5,9,second.batch.sequence}) ==
          ReplicationStatus::WrongViewer);
    CHECK(budget.acknowledge({key(1),5,8,second.batch.sequence}) ==
          ReplicationStatus::WrongEpoch);
    CHECK(budget.hasAdmittedFrame() && budget.committedVisibleCount() == 0);
    CHECK(budget.acknowledge({key(1),5,9,second.batch.sequence}) ==
          ReplicationStatus::Prepared);
    CHECK(!budget.hasAdmittedFrame() && budget.admittedFrame().empty());
    CHECK(budget.committedVisibleCount() == 1);
    CHECK(budget.acknowledge({key(1),5,9,second.batch.sequence}) ==
          ReplicationStatus::NotPending);
    CHECK(budget.prepare(world,0,0,100,240).status == ReplicationStatus::NoChanges);

    // Retry budget is strict, no optimistic commit after two attempts.
    CHECK(world.upsert({key(2),32,20,2,true}) == AoiStatus::Ok);
    const auto third = budget.prepare(world,0,0,100,300);
    CHECK(third.status == ReplicationStatus::Prepared &&
          third.batch.events.size() == 1);
    CHECK(budget.admit(third.batch.sequence,{1,2,3},301) == AoiDeliveryStatus::Accepted);
    CHECK(budget.authorizeSendAttempt(third.batch.sequence,302) == AoiDeliveryStatus::Accepted);
    CHECK(budget.authorizeSendAttempt(third.batch.sequence,303) == AoiDeliveryStatus::Accepted);
    CHECK(budget.admittedFrame() == std::vector<std::uint8_t>({1,2,3}));
    CHECK(budget.authorizeSendAttempt(third.batch.sequence,304) == AoiDeliveryStatus::RetryLimit);
    CHECK(budget.committedVisibleCount() == 0 && !budget.hasAdmittedFrame());
    CHECK(budget.acknowledge({key(1),5,9,third.batch.sequence}) == ReplicationStatus::NotPending);

    // Invalid empty frame, deadline, and clock rollback all fail closed.
    auto fourth = budget.prepare(world,0,0,100,400);
    CHECK(fourth.status == ReplicationStatus::Prepared);
    CHECK(budget.admit(fourth.batch.sequence,{},401) == AoiDeliveryStatus::EmptyFrame);
    fourth = budget.prepare(world,0,0,100,410);
    CHECK(fourth.status == ReplicationStatus::Prepared);
    CHECK(budget.admit(fourth.batch.sequence,{1},411) == AoiDeliveryStatus::Accepted);
    CHECK(budget.authorizeSendAttempt(fourth.batch.sequence,461) == AoiDeliveryStatus::Expired);
    CHECK(budget.committedVisibleCount() == 0);
    fourth = budget.prepare(world,0,0,100,470);
    CHECK(fourth.status == ReplicationStatus::Prepared);
    CHECK(budget.admit(fourth.batch.sequence,{1},471) == AoiDeliveryStatus::Accepted);
    CHECK(budget.authorizeSendAttempt(fourth.batch.sequence,470) ==
          AoiDeliveryStatus::InvalidClock);

    // Scene/connection generation changes cancel pending frame and stale ACKs.
    auto scene = budget.prepare(world,0,0,100,500);
    CHECK(budget.admit(scene.batch.sequence,{1},501) == AoiDeliveryStatus::Accepted);
    CHECK(!budget.advanceScene(9) && budget.hasAdmittedFrame());
    CHECK(budget.advanceScene(10) && !budget.hasAdmittedFrame());
    CHECK(budget.acknowledge({key(1),5,9,scene.batch.sequence}) ==
          ReplicationStatus::NotPending);
    auto connection = budget.prepare(world,0,0,100,600);
    CHECK(connection.batch.sceneEpoch == 10 &&
          connection.batch.events[0].kind == AoiDelta::Kind::Reset);
    CHECK(budget.admit(connection.batch.sequence,{1,2},601) == AoiDeliveryStatus::Accepted);
    CHECK(budget.advanceConnection(6) && !budget.hasAdmittedFrame());
    CHECK(!budget.advanceConnection(6));
    CHECK(budget.invalidateForResync());
    CHECK(budget.lastAcknowledgedSequence() == second.batch.sequence);

    std::atomic<bool> foreignRejected{false};
    std::thread foreign([&] {
        foreignRejected =
            !budget.bindOwnerToCurrentThread() &&
            budget.prepare(world,0,0,10,700).status == ReplicationStatus::WrongThread &&
            budget.admit(123,{1},700) == AoiDeliveryStatus::WrongThread &&
            budget.authorizeSendAttempt(123,701) == AoiDeliveryStatus::WrongThread &&
            budget.acknowledge({key(1),6,10,123}) == ReplicationStatus::WrongThread &&
            !budget.invalidateForResync() && !budget.advanceScene(11);
    });
    foreign.join();
    CHECK(foreignRejected);
    const auto stats = budget.counters();
    CHECK(stats.admitted >= 5 && stats.acknowledged == 1);
    CHECK(stats.rejectedFrame == 2 && stats.retryLimit == 1 && stats.timeouts == 1);
    CHECK(stats.attempts == 3 && stats.rejectedAck == 2 && stats.resyncs >= 6);
    std::cout << "PASS: bounded exact frame, ACK gating, retry/timeout, resync, "
                 "scene/connection isolation, owner thread and counters\n";
    return 0;
}

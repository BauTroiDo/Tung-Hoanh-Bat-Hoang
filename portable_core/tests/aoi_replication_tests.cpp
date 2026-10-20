#include "mmo/AoiReplication.h"
#include <atomic>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <thread>

#define CHECK(e) do { if (!(e)) { std::cerr << "FAIL line " << __LINE__ << ": " #e "\n"; return 1; } } while(false)
using namespace mmo::portable;
static EntityKey makeKey(std::uint64_t id, std::uint32_t gen=1) {
    return {id,gen,500,7};
}
static bool hasKind(const ReplicationBatch& b, AoiDelta::Kind kind, EntityKey key) {
    for (const auto& e : b.events)
        if (e.kind==kind && e.key==key) return true;
    return false;
}
int main() {
    bool invalid=false;
    try { AoiReplicationSession bad(makeKey(0)); }
    catch (const std::invalid_argument&) { invalid=true; }
    CHECK(invalid);
    invalid=false;
    try { AoiReplicationSession bad(makeKey(1),0); }
    catch (const std::invalid_argument&) { invalid=true; }
    CHECK(invalid);

    SpatialAoi world(100,500,100,100);
    AoiReplicationSession session(makeKey(1),16);
    CHECK(session.prepare(world,0,0,200,7,100).status == ReplicationStatus::Unbound);
    CHECK(session.retry().status == ReplicationStatus::Unbound);
    CHECK(session.acknowledge(1) == ReplicationStatus::Unbound);
    CHECK(!session.invalidateForResync());
    CHECK(world.bindOwnerToCurrentThread());
    CHECK(session.bindOwnerToCurrentThread());
    CHECK(world.upsert({makeKey(1),0,0,1,true}) == AoiStatus::Ok);
    CHECK(world.upsert({makeKey(2),10,20,1,true}) == AoiStatus::Ok);
    CHECK(world.upsert({makeKey(3),25,20,1,true}) == AoiStatus::Ok);

    // Prepare: staged client knowledge stays uncommitted until transport confirmation.
    auto first=session.prepare(world,0,0,200,7,100);
    CHECK(first.status == ReplicationStatus::Prepared);
    CHECK(first.batch.sequence == 1 && first.batch.sceneEpoch == 7);
    CHECK(first.batch.events.size() == 3);
    CHECK(first.batch.events[0].kind == AoiDelta::Kind::Reset);
    CHECK(hasKind(first.batch,AoiDelta::Kind::Create,makeKey(2)));
    CHECK(hasKind(first.batch,AoiDelta::Kind::Create,makeKey(3)));
    CHECK(session.committedVisibleCount() == 0 && session.committedEpoch() == 0);
    CHECK(session.hasInFlight());
    CHECK(session.prepare(world,0,0,200,7,200).status == ReplicationStatus::InFlight);
    CHECK(!session.timedOut(109,10));
    CHECK(session.timedOut(110,10));
    CHECK(!session.timedOut(90,10));
    CHECK(!session.timedOut(999,0));

    auto retry=session.retry();
    CHECK(retry.status == ReplicationStatus::Prepared);
    CHECK(retry.batch.sequence == 1 && retry.batch.events.size()==3);
    CHECK(session.acknowledge(0)==ReplicationStatus::WrongSequence);
    CHECK(session.acknowledge(2)==ReplicationStatus::WrongSequence);
    CHECK(session.committedVisibleCount()==0);
    CHECK(session.acknowledge(1)==ReplicationStatus::Prepared);
    CHECK(!session.hasInFlight());
    CHECK(session.lastAcknowledgedSequence()==1);
    CHECK(session.committedVisibleCount()==2 && session.committedEpoch()==7);
    CHECK(session.acknowledge(1)==ReplicationStatus::NotPending);
    CHECK(session.retry().status==ReplicationStatus::NotPending);
    CHECK(session.prepare(world,0,0,200,7,120).status==ReplicationStatus::NoChanges);

    // One entity revision -> one UPDATE. Disconnect during in-flight forces RESET.
    CHECK(world.upsert({makeKey(2),14,20,2,true})==AoiStatus::Ok);
    auto update=session.prepare(world,0,0,200,7,300);
    CHECK(update.status==ReplicationStatus::Prepared &&
          update.batch.sequence==2 && update.batch.events.size()==1);
    CHECK(hasKind(update.batch,AoiDelta::Kind::Update,makeKey(2)));
    CHECK(session.committedVisibleCount()==2);
    CHECK(session.invalidateForResync());
    CHECK(!session.hasInFlight());
    CHECK(session.committedVisibleCount()==0 && session.committedEpoch()==0);
    CHECK(session.acknowledge(2)==ReplicationStatus::NotPending);

    // New sequence after resync: old acknowledgements cannot advance state.
    auto reset=session.prepare(world,0,0,200,8,400);
    CHECK(reset.status==ReplicationStatus::Prepared &&
          reset.batch.sequence==3 && reset.batch.events.size()==3);
    CHECK(reset.batch.events[0].kind==AoiDelta::Kind::Reset);
    CHECK(session.acknowledge(2)==ReplicationStatus::WrongSequence);
    CHECK(session.committedVisibleCount()==0);
    CHECK(session.acknowledge(3)==ReplicationStatus::Prepared);
    CHECK(session.committedEpoch()==8 && session.committedVisibleCount()==2);

    // Removal must be durable only after ACK; failed/uncertain transmission may retry.
    CHECK(world.erase(makeKey(3))==AoiStatus::Ok);
    auto removed=session.prepare(world,0,0,200,8,500);
    CHECK(removed.status==ReplicationStatus::Prepared &&
          removed.batch.sequence==4 && removed.batch.events.size()==1);
    CHECK(hasKind(removed.batch,AoiDelta::Kind::Remove,makeKey(3)));
    CHECK(session.committedVisibleCount()==2);
    CHECK(session.retry().batch.sequence==4);
    CHECK(session.acknowledge(4)==ReplicationStatus::Prepared);
    CHECK(session.committedVisibleCount()==1);

    // Epoch switch always produces RESET + complete snapshot even with no entity changes.
    auto anotherScene=session.prepare(world,0,0,200,9,600);
    CHECK(anotherScene.status==ReplicationStatus::Prepared &&
          anotherScene.batch.sequence==5 && anotherScene.batch.events.size()==2);
    CHECK(anotherScene.batch.events[0].kind==AoiDelta::Kind::Reset);
    CHECK(session.acknowledge(5)==ReplicationStatus::Prepared);
    CHECK(session.committedEpoch()==9);

    // Client packet budget overflow: no pending batch, known view unchanged.
    AoiReplicationSession lowBudget(makeKey(1),1);
    CHECK(lowBudget.bindOwnerToCurrentThread());
    auto refused=lowBudget.prepare(world,0,0,200,1,700);
    CHECK(refused.status==ReplicationStatus::PlannerRejected);
    CHECK(refused.plannerStatus==AoiStatus::EventBudgetExceeded);
    CHECK(lowBudget.committedVisibleCount()==0 && !lowBudget.hasInFlight());
    CHECK(lowBudget.prepare(world,0,0,999,1,710).status==ReplicationStatus::PlannerRejected);

    // Foreign thread cannot plan, retry, ACK, resync or rebind.
    std::atomic<bool> rejected{false};
    std::thread foreign([&] {
        rejected = session.prepare(world,0,0,50,9,750).status==ReplicationStatus::WrongThread &&
            session.retry().status==ReplicationStatus::WrongThread &&
            session.acknowledge(123)==ReplicationStatus::WrongThread &&
            !session.invalidateForResync() && !session.bindOwnerToCurrentThread();
    });
    foreign.join();
    CHECK(rejected && session.committedEpoch()==9);
    CHECK(session.prepare(world,0,0,5,9,800).status==ReplicationStatus::Prepared);
    CHECK(session.hasInFlight());
    CHECK(session.invalidateForResync() && !session.hasInFlight());
    CHECK(session.lastAcknowledgedSequence()==5);

    std::cout << "PASS: prepare/ack commit, stop-and-wait, resend, timeout,"
        << " resync, stale ACK, removal, epoch, budgets, owner-thread safety\n";
    return 0;
}

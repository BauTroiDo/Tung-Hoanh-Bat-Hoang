#include "mmo/AoiDeliveryPool.h"
#include <atomic>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <thread>
#include <vector>
using namespace mmo::portable;
#define CHECK(e) do { if (!(e)) { std::cerr << "FAILED line " << __LINE__ << ": " #e "\n"; return 1; } } while(false)

namespace {
EntityKey viewer(std::uint64_t id) { return {id+1000,1,70,3}; }
AoiDeliveryLimits deliveryLimits() { return {8,3,50}; }
std::uint64_t planAndAdmit(AoiDeliveryPool& pool, SpatialAoi& world,
    std::uint64_t id, std::uint64_t now, std::size_t size=6) {
    const auto prepared = pool.prepare(id,world,0,0,100,now);
    if (prepared.status != AoiPoolStatus::Ok ||
        prepared.replication.status != ReplicationStatus::Prepared ||
        prepared.replication.batch.events.empty() ||
        prepared.replication.batch.events[0].kind != AoiDelta::Kind::Reset) {
        throw std::runtime_error("missing expected full RESET");
    }
    const auto seq = prepared.replication.batch.sequence;
    const auto admitted = pool.admit(id,seq,
        std::vector<std::uint8_t>(size,static_cast<std::uint8_t>(id)),now+1);
    if (admitted.status != AoiPoolStatus::Ok ||
        admitted.delivery != AoiDeliveryStatus::Accepted)
        throw std::runtime_error("expected admission");
    return seq;
}
}
int main() {
    bool invalid=false;
    try { AoiDeliveryPool p({0,24,12,2,10}); }
    catch (const std::invalid_argument&) { invalid=true; }
    CHECK(invalid);
    AoiDeliveryPool pool({3,24,12,2,10});
    SpatialAoi world(100,300,100,100);
    CHECK(pool.add(1,viewer(1),10,20,deliveryLimits())==AoiPoolStatus::Unbound);
    CHECK(pool.beginTick(1)==AoiPoolStatus::Unbound);
    CHECK(pool.nextSend(100).status==AoiPoolStatus::Unbound);
    CHECK(pool.bindOwnerToCurrentThread() && world.bindOwnerToCurrentThread());
    CHECK(world.upsert({{99,1,70,3},10,10,1,true})==AoiStatus::Ok);
    CHECK(pool.add(0,viewer(1),10,20,deliveryLimits())==AoiPoolStatus::Invalid);
    CHECK(pool.add(1,viewer(1),10,20,deliveryLimits())==AoiPoolStatus::Ok);
    CHECK(pool.add(1,viewer(1),10,20,deliveryLimits())==AoiPoolStatus::Duplicate);
    CHECK(pool.add(2,viewer(2),10,20,deliveryLimits())==AoiPoolStatus::Ok);
    CHECK(pool.add(3,viewer(3),10,20,deliveryLimits())==AoiPoolStatus::Ok);
    CHECK(pool.add(4,viewer(4),10,20,deliveryLimits())==AoiPoolStatus::SessionLimit);
    CHECK(pool.sessionCount()==3 && pool.pendingBytes()==0);
    const auto seq1=planAndAdmit(pool,world,1,100);
    const auto seq2=planAndAdmit(pool,world,2,100);
    const auto seq3=planAndAdmit(pool,world,3,100);
    CHECK(pool.pendingBytes()==18 && pool.pendingSessions()==3);
    CHECK(pool.nextSend(102).status==AoiPoolStatus::TickNotStarted);
    CHECK(pool.beginTick(0)==AoiPoolStatus::TickOrder);
    CHECK(pool.beginTick(1)==AoiPoolStatus::Ok);
    CHECK(pool.beginTick(1)==AoiPoolStatus::TickOrder);
    auto send=pool.nextSend(102);
    CHECK(send.status==AoiPoolStatus::Ok && send.sessionId==1 &&
          send.sequence==seq1 && send.frame && send.frame->size()==6 &&
          (*send.frame)[0]==1);
    const auto first=send.frame;
    send=pool.nextSend(102);
    CHECK(send.status==AoiPoolStatus::Ok && send.sessionId==2 &&
          send.sequence==seq2 && send.frame && (*send.frame)[0]==2);
    CHECK(pool.tickAttempts()==2 && pool.tickBytes()==12);
    CHECK(pool.nextSend(102).status==AoiPoolStatus::TickBudgetExhausted);
    CHECK(pool.acknowledge(1,{viewer(2),10,20,seq1}).replication==
          ReplicationStatus::WrongViewer);
    CHECK(pool.pendingBytes()==18);
    CHECK(pool.acknowledge(1,{viewer(1),10,20,seq1}).replication==
          ReplicationStatus::Prepared);
    CHECK(pool.pendingBytes()==12 && pool.pendingSessions()==2);
    CHECK(pool.beginTick(2)==AoiPoolStatus::Ok);
    CHECK(pool.nextSend(103).sessionId==3); // first fair turn
    CHECK(pool.nextSend(103).status==AoiPoolStatus::NoEligible); // retry backoff
    CHECK(pool.nextSend(102).status==AoiPoolStatus::ClockRollback);
    CHECK(pool.nextSend(114).sessionId==2); // pacing permits retry in next tick
    CHECK(pool.tickAttempts()==2);
    CHECK(pool.acknowledge(2,{viewer(2),10,20,seq2}).replication==
          ReplicationStatus::Prepared);
    CHECK(pool.acknowledge(3,{viewer(3),10,20,seq3}).replication==
          ReplicationStatus::Prepared);
    CHECK(pool.pendingBytes()==0 && pool.pendingSessions()==0);
    CHECK(pool.remove(2)==AoiPoolStatus::Ok && pool.sessionCount()==2);
    CHECK(pool.remove(2)==AoiPoolStatus::NotFound);

    // Test global cap without changing per-session limits.
    AoiDeliveryPool capped({3,12,8,2,10});
    CHECK(capped.bindOwnerToCurrentThread());
    CHECK(capped.add(1,viewer(1),10,20,deliveryLimits())==AoiPoolStatus::Ok);
    CHECK(capped.add(2,viewer(2),10,20,deliveryLimits())==AoiPoolStatus::Ok);
    CHECK(capped.add(3,viewer(3),10,20,deliveryLimits())==AoiPoolStatus::Ok);
    const auto ca=planAndAdmit(capped,world,1,200);
    const auto cb=planAndAdmit(capped,world,2,200);
    CHECK(capped.pendingBytes()==12);
    const auto unadmitted=capped.prepare(3,world,0,0,100,200);
    CHECK(unadmitted.replication.status==ReplicationStatus::Prepared);
    const auto reject=capped.admit(3,unadmitted.replication.batch.sequence,
                                  std::vector<std::uint8_t>(6,3),201);
    CHECK(reject.status==AoiPoolStatus::PendingBytesLimit);
    CHECK(capped.pendingBytes()==12 && capped.pendingSessions()==2);
    const auto retried=capped.prepare(3,world,0,0,100,210);
    CHECK(retried.replication.status==ReplicationStatus::Prepared);
    CHECK(retried.replication.batch.events[0].kind==AoiDelta::Kind::Reset);
    CHECK(capped.admit(3,retried.replication.batch.sequence,
                        std::vector<std::uint8_t>(9,3),211).status==
          AoiPoolStatus::FrameExceedsTickLimit);
    CHECK(capped.pendingBytes()==12);
    CHECK(capped.beginTick(1)==AoiPoolStatus::Ok);
    CHECK(capped.nextSend(203).sessionId==1);
    CHECK(capped.nextSend(203).status==AoiPoolStatus::TickBudgetExhausted);
    CHECK(capped.acknowledge(1,{viewer(1),10,20,ca}).replication==
          ReplicationStatus::Prepared);
    CHECK(capped.pendingBytes()==6);
    CHECK(capped.advanceScene(2,21)==AoiPoolStatus::Ok);
    CHECK(capped.pendingBytes()==0);
    CHECK(capped.acknowledge(2,{viewer(2),10,20,cb}).replication==
          ReplicationStatus::NotPending);
    const auto late=planAndAdmit(capped,world,3,300);
    CHECK(capped.pendingBytes()==6);
    CHECK(capped.beginTick(2)==AoiPoolStatus::Ok);
    CHECK(capped.nextSend(351).status==AoiPoolStatus::NoEligible); // expired
    CHECK(capped.pendingBytes()==0 && capped.pendingSessions()==0);
    CHECK(capped.acknowledge(3,{viewer(3),10,20,late}).replication==
          ReplicationStatus::NotPending);
    CHECK(capped.remove(3)==AoiPoolStatus::Ok);
    CHECK(capped.sessionCount()==2);

    // Larger deterministic fairness regression: every one of 64 sessions
    // gets exactly one turn across 16 ticks (4 sends/tick), no starvation.
    AoiDeliveryPool fair({64,192,12,4,10});
    CHECK(fair.bindOwnerToCurrentThread());
    for (std::uint64_t id=1; id<=64; ++id) {
        CHECK(fair.add(id,viewer(id),10,20,deliveryLimits())==AoiPoolStatus::Ok);
        planAndAdmit(fair,world,id,1000,3);
    }
    CHECK(fair.sessionCount()==64 && fair.pendingBytes()==192);
    for (std::uint64_t tick=1; tick<=16; ++tick) {
        CHECK(fair.beginTick(tick)==AoiPoolStatus::Ok);
        for (std::uint64_t slot=1; slot<=4; ++slot) {
            const auto ready=fair.nextSend(1001+tick);
            const auto expected=(tick-1)*4+slot;
            CHECK(ready.status==AoiPoolStatus::Ok && ready.sessionId==expected);
            CHECK(ready.sequence==1 && ready.frame && ready.frame->size()==3);
            CHECK(fair.acknowledge(expected,{viewer(expected),10,20,1}).replication==
                  ReplicationStatus::Prepared);
        }
        CHECK(fair.tickAttempts()==4 && fair.tickBytes()==12);
    }
    CHECK(fair.pendingBytes()==0 && fair.pendingSessions()==0);
    CHECK(fair.beginTick(17)==AoiPoolStatus::Ok);
    CHECK(fair.nextSend(1019).status==AoiPoolStatus::NoEligible);

    std::atomic<bool> foreignRejected{false};
    std::thread foreign([&] {
        foreignRejected=!capped.bindOwnerToCurrentThread() &&
            capped.add(4,viewer(4),10,20,deliveryLimits())==AoiPoolStatus::WrongThread &&
            capped.beginTick(3)==AoiPoolStatus::WrongThread &&
            capped.nextSend(400).status==AoiPoolStatus::WrongThread &&
            capped.remove(1)==AoiPoolStatus::WrongThread &&
            capped.invalidateForResync(2)==AoiPoolStatus::WrongThread;
    });
    foreign.join();
    CHECK(foreignRejected && capped.sessionCount()==2);
    CHECK(capped.remove(1)==AoiPoolStatus::Ok);
    CHECK(capped.remove(2)==AoiPoolStatus::Ok);
    CHECK(capped.sessionCount()==0 && capped.pendingBytes()==0);
    std::cout << "PASS: global byte/session admission, fair RR, tick quotas, "
                 "retry pacing, expiry, authenticated ACK, cleanup and ownership\n";
    (void)first;
    return 0;
}

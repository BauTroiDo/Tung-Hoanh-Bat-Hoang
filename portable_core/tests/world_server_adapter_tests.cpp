#include "mmo/WorldServerIntegrationAdapter.h"
#include <iostream>
#include <thread>
using namespace mmo::portable;
#define CHECK(e) do { if (!(e)) { std::cerr << "FAILED " << __LINE__ << ": " #e "\n"; return 1; } } while(false)
int main() {
    const WorldAdapterLimits limits{2,1,2,2};
    const AoiPoolLimits pool{2,8,8,1,1};
    TrustedWorldContext c{1,{100,1,1,1},10,20};
    WorldServerIntegrationAdapter off(limits,pool);
    CHECK(off.bindOwnerToCurrentThread());
    CHECK(!off.attach(c,{8,3,50},8));
    CHECK(off.postAck(c,1)==WorldCommandQueue::PostStatus::Invalid);
    CHECK(off.beginTick(1)==AoiPoolStatus::Invalid);
    WorldServerIntegrationAdapter a(limits,pool,true);
    CHECK(!a.attach(c,{8,3,50},8));
    CHECK(a.bindOwnerToCurrentThread());
    CHECK(a.attach(c,{8,3,50},8));
    CHECK(!a.attach(c,{8,3,50},8));
    CHECK(a.upsert({{200,1,1,1},1,1,1,true})==AoiStatus::Ok);
    auto p=a.prepare(c,0,0,20,1);
    CHECK(p.replication.status==ReplicationStatus::Prepared);
    auto seq=p.replication.batch.sequence;
    CHECK(a.admit(c,seq,{1,2,3,4},2).delivery==AoiDeliveryStatus::Accepted);
    CHECK(a.beginTick(1)==AoiPoolStatus::Ok);
    CHECK(a.nextSend(3).sequence==seq);
    // Wrong context is queued but cannot mutate world.
    auto bad=c; ++bad.viewer.generation;
    CHECK(a.postAck(bad,seq)==WorldCommandQueue::PostStatus::Accepted);
    CHECK(a.postAck(c,seq+1)==WorldCommandQueue::PostStatus::Accepted);
    CHECK(a.postAck(c,seq)==WorldCommandQueue::PostStatus::Full);
    CHECK(a.drain().remaining==1);
    CHECK(a.drain().remaining==0);
    CHECK(a.metrics().acceptedAcks==0 && a.metrics().rejectedCommands==2);
    // Queued ACK from old socket rejected after authoritative reconnect.
    CHECK(a.postAck(c,seq)==WorldCommandQueue::PostStatus::Accepted);
    auto next=c; ++next.connectionGeneration;
    CHECK(a.transition(next)); CHECK(!a.transition(c));
    a.drain(); CHECK(a.metrics().acceptedAcks==0);
    CHECK(a.prepare(c,0,0,20,4).status==AoiPoolStatus::Invalid);
    p=a.prepare(next,0,0,20,4); seq=p.replication.batch.sequence;
    CHECK(p.replication.status==ReplicationStatus::Prepared);
    CHECK(p.replication.batch.events.front().kind==AoiDelta::Kind::Reset);
    CHECK(a.admit(next,seq,{4,3,2,1},5).delivery==AoiDeliveryStatus::Accepted);
    CHECK(a.beginTick(2)==AoiPoolStatus::Ok);
    CHECK(a.nextSend(5).sequence==seq);
    WorldCommandQueue::PostStatus posted{};
    bool foreignAttach=true; bool wrongDrain=false;
    std::thread worker([&] {
        foreignAttach=a.attach({2,{101,1,1,1},1,1},{8,3,50},8);
        wrongDrain=a.drain().wrongOwner;
        posted=a.postAck(next,seq);
    }); worker.join();
    CHECK(!foreignAttach && wrongDrain && posted==WorldCommandQueue::PostStatus::Accepted);
    a.drain(); CHECK(a.metrics().acceptedAcks==1);
    CHECK(a.postAck(next,seq)==WorldCommandQueue::PostStatus::Accepted);
    a.drain(); CHECK(a.metrics().acceptedAcks==1);
    CHECK(a.upsert({{200,1,1,1},2,1,2,true})==AoiStatus::Ok);
    p=a.prepare(next,0,0,20,6); seq=p.replication.batch.sequence;
    CHECK(p.replication.status==ReplicationStatus::Prepared);
    CHECK(a.admit(next,seq,{1,2,3},7).delivery==AoiDeliveryStatus::Accepted);
    CHECK(a.postSendFailure(next,seq)==WorldCommandQueue::PostStatus::Accepted);
    a.drain();
    p=a.prepare(next,0,0,20,8);
    CHECK(p.replication.status==ReplicationStatus::Prepared);
    CHECK(p.replication.batch.events.front().kind==AoiDelta::Kind::Reset);
    auto seq2=p.replication.batch.sequence;
    CHECK(a.admit(next,seq2,{9,9},9).delivery==AoiDeliveryStatus::Accepted);
    CHECK(a.postSendFailure(next,seq)==WorldCommandQueue::PostStatus::Accepted);
    a.drain(); // old send failure cannot discard new in-flight data
    CHECK(a.beginTick(3)==AoiPoolStatus::Ok);
    CHECK(a.nextSend(9).sequence==seq2);
    CHECK(a.postAck(next,seq2)==WorldCommandQueue::PostStatus::Accepted);
    a.drain(); CHECK(a.metrics().acceptedAcks==2);
    // Scene transition rejects queued old-scene ACK.
    CHECK(a.postAck(next,seq2)==WorldCommandQueue::PostStatus::Accepted);
    auto scene=next; ++scene.sceneEpoch;
    CHECK(a.transition(scene)); a.drain();
    CHECK(a.metrics().acceptedAcks==2);
    CHECK(a.detach(scene)); CHECK(!a.attach(scene,{8,3,50},8));
    CHECK(a.attach({2,{101,1,1,1},1,1},{8,3,50},8));
    CHECK(!a.attach({3,{102,1,1,1},1,1},{8,3,50},8));
    CHECK(a.upsert({{201,1,1,1},1,1,1,true})==AoiStatus::Ok);
    CHECK(a.upsert({{202,1,1,1},1,1,1,true})==AoiStatus::Invalid);
    // Full ingress must not lose an uncertain transport failure.
    TrustedWorldContext c2{2,{101,1,1,1},1,1};
    p=a.prepare(c2,0,0,20,10); seq=p.replication.batch.sequence;
    CHECK(a.admit(c2,seq,{1,2},11).delivery==AoiDeliveryStatus::Accepted);
    CHECK(a.beginTick(4)==AoiPoolStatus::Ok);
    CHECK(a.nextSend(11).sequence==seq);
    CHECK(a.postAck(c2,seq)==WorldCommandQueue::PostStatus::Accepted);
    CHECK(a.postAck(c2,seq)==WorldCommandQueue::PostStatus::Accepted);
    CHECK(a.postSendFailure(c2,seq)==WorldCommandQueue::PostStatus::Full);
    CHECK(a.nextSend(12).status==AoiPoolStatus::Invalid);
    a.drain(); a.drain();
    CHECK(a.metrics().acceptedAcks==2);
    p=a.prepare(c2,0,0,20,12);
    CHECK(p.replication.status==ReplicationStatus::Prepared);
    CHECK(p.replication.batch.events.front().kind==AoiDelta::Kind::Reset);
    seq=p.replication.batch.sequence;
    CHECK(a.admit(c2,seq,{1,2},13).delivery==AoiDeliveryStatus::Accepted);
    CHECK(a.beginTick(5)==AoiPoolStatus::Ok);
    CHECK(a.nextSend(13).sequence==seq);
    CHECK(a.beginTick(6)==AoiPoolStatus::Ok);
    CHECK(a.nextSend(100).status==AoiPoolStatus::NoEligible); // expire/release
    CHECK(a.postAck(c2,seq)==WorldCommandQueue::PostStatus::Accepted);
    a.drain(); CHECK(a.metrics().acceptedAcks==2);
    p=a.prepare(c2,0,0,20,101);
    CHECK(p.replication.status==ReplicationStatus::Prepared);
    CHECK(p.replication.batch.events.front().kind==AoiDelta::Kind::Reset);
    CHECK(a.admit(c2,p.replication.batch.sequence,std::vector<std::uint8_t>(9,1),102).delivery
        !=AoiDeliveryStatus::Accepted);
    CHECK(a.prepare(c2,0,0,20,103).replication.status==ReplicationStatus::Prepared);
    a.closeIngress();
    CHECK(a.postAck(scene,seq2)==WorldCommandQueue::PostStatus::Closed);
    CHECK(a.drain().failed==0);
    // Teardown must remain possible while lost-failure recovery blocks ACKs.
    WorldServerIntegrationAdapter shutting({1,1,2,2},{1,4,4,1,1},true);
    CHECK(shutting.bindOwnerToCurrentThread());
    CHECK(shutting.attach(c,{4,3,100},8));
    auto last=shutting.prepare(c,0,0,20,1);
    CHECK(last.replication.status==ReplicationStatus::Prepared);
    auto lastSeq=last.replication.batch.sequence;
    CHECK(shutting.admit(c,lastSeq,{1,2,3,4},1).delivery==AoiDeliveryStatus::Accepted);
    CHECK(shutting.beginTick(1)==AoiPoolStatus::Ok);
    CHECK(shutting.nextSend(1).sequence==lastSeq);
    CHECK(shutting.postAck(c,lastSeq)==WorldCommandQueue::PostStatus::Accepted);
    CHECK(shutting.postSendFailure(c,lastSeq)==WorldCommandQueue::PostStatus::Full);
    shutting.closeIngress();
    auto stale=c; ++stale.connectionGeneration;
    CHECK(!shutting.detach(stale));
    bool foreignDetach=true;
    std::thread cleanupWorker([&] { foreignDetach=shutting.detach(c); });
    cleanupWorker.join(); CHECK(!foreignDetach);
    CHECK(shutting.detach(c)); // old implementation refused while latch was set
    CHECK(!shutting.detach(c));
    CHECK(shutting.drain().remaining==0);
    CHECK(shutting.metrics().acceptedAcks==0);
    TrustedWorldContext replacement{2,{101,1,1,1},1,1};
    CHECK(shutting.attach(replacement,{4,3,100},8)); // pool session slot released
    last=shutting.prepare(replacement,0,0,20,2);
    CHECK(last.replication.status==ReplicationStatus::Prepared);
    CHECK(shutting.admit(replacement,last.replication.batch.sequence,{4,3,2,1},2).delivery
        ==AoiDeliveryStatus::Accepted); // aggregate pending bytes also released
    std::cout << "world adapter regressions passed\n";
}

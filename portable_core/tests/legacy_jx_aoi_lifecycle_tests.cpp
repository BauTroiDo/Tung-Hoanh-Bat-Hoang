#include "mmo/LegacyJxAoiLifecycleBridge.h"
#include <iostream>
#include <thread>
using namespace mmo::portable;
#define CHECK(expr) do { if (!(expr)) { std::cerr << "CHECK FAILED line " << __LINE__ << ": " #expr "\n"; return 1; } } while(false)

static LegacyJxNpcSnapshot snap(std::int32_t slot=10, std::uint32_t npcId=204,
                                 std::int32_t x=100, std::int32_t y=200) {
    LegacyJxNpcSnapshot s;
    s.npcArrayIndex=slot; s.npcDwId=npcId;
    s.subWorldArrayIndex=7; s.getMpsMapIndex=7;
    s.mapId=53; s.regionIndex=3; s.totalRegions=200;
    s.mpsX=x; s.mpsY=y; s.instanceId=4;
    return s;
}

int main() {
    int checks=0;
    WorldServerIntegrationAdapter off({8,2,2,2},{1,8,8,1,1},true);
    LegacyJxAoiLifecycleBridge disabled(off,1000,100,2,3);
    CHECK(!disabled.bindOwnerToCurrentThread()); ++checks;
    CHECK(disabled.observeSpawn(snap()).status==LegacyNpcSyncStatus::Disabled); ++checks;

    WorldServerIntegrationAdapter a({8,2,2,3},{1,8,8,1,1},true);
    LegacyJxAoiLifecycleBridge bridge(a,1000,100,2,2,true);
    CHECK(bridge.observeSpawn(snap()).status==LegacyNpcSyncStatus::Unbound); ++checks;
    CHECK(bridge.bindOwnerToCurrentThread()); ++checks;
    auto first=bridge.observeSpawn(snap());
    CHECK(first.status==LegacyNpcSyncStatus::Ok && first.key.worldId==53 &&
          first.key.worldId!=7 && first.key.runtimeId==204 &&
          first.key.generation==1 && first.key.instanceId==4); ++checks;
    CHECK(bridge.activeCount()==1 && bridge.lastGeneration(204)==1); ++checks;
    CHECK(bridge.observeSpawn(snap()).status==LegacyNpcSyncStatus::AlreadyActive); ++checks;
    CHECK(bridge.observeSpawn(snap(11,204)).status==LegacyNpcSyncStatus::AlreadyActive); ++checks;
    auto moved=bridge.observeMove(snap(10,204,110,210));
    CHECK(moved.status==LegacyNpcSyncStatus::Ok && moved.key==first.key); ++checks;
    CHECK(bridge.observeMove(snap(10,999)).status==LegacyNpcSyncStatus::StaleIdentity); ++checks;
    auto otherMap=snap(); otherMap.mapId=54;
    CHECK(bridge.observeMove(otherMap).status==LegacyNpcSyncStatus::MapChanged); ++checks;
    otherMap=snap(); otherMap.instanceId=8;
    CHECK(bridge.observeMove(otherMap).status==LegacyNpcSyncStatus::MapChanged); ++checks;
    auto invalid=snap(); invalid.regionIndex=999;
    CHECK(bridge.observeMove(invalid).status==LegacyNpcSyncStatus::BadSnapshot); ++checks;
    CHECK(bridge.observeRemove(10,999).status==LegacyNpcSyncStatus::StaleIdentity); ++checks;
    CHECK(bridge.observeRemove(11,204).status==LegacyNpcSyncStatus::SlotNotActive); ++checks;
    LegacyNpcSyncStatus foreign=LegacyNpcSyncStatus::Ok;
    std::thread other([&] { foreign=bridge.observeRemove(10,204).status; });
    other.join();
    CHECK(foreign==LegacyNpcSyncStatus::WrongThread && bridge.activeCount()==1); ++checks;
    CHECK(bridge.observeRemove(10,204).status==LegacyNpcSyncStatus::Ok); ++checks;
    CHECK(bridge.activeCount()==0); ++checks;
    CHECK(a.upsert({first.key,110,210,4,true})==AoiStatus::StaleGeneration); ++checks;
    auto replacement=bridge.observeSpawn(snap(10,204,300,400));
    CHECK(replacement.status==LegacyNpcSyncStatus::Ok && replacement.key.generation==2); ++checks;
    CHECK(bridge.lastGeneration(204)==2); ++checks;
    CHECK(bridge.observeMove(snap(10,204,301,401)).status==LegacyNpcSyncStatus::Ok); ++checks;
    auto second=bridge.observeSpawn(snap(11,205));
    CHECK(second.status==LegacyNpcSyncStatus::Ok && second.key.generation==1); ++checks;
    CHECK(bridge.observeSpawn(snap(12,206)).status==LegacyNpcSyncStatus::Capacity); ++checks;
    CHECK(bridge.observeRemove(11,205).status==LegacyNpcSyncStatus::Ok); ++checks;
    CHECK(bridge.observeSpawn(snap(11,206)).status==LegacyNpcSyncStatus::Capacity); ++checks;
    CHECK(bridge.observeRemove(10,204).status==LegacyNpcSyncStatus::Ok); ++checks;
    CHECK(bridge.observeRemove(10,204).status==LegacyNpcSyncStatus::SlotNotActive); ++checks;
    CHECK(bridge.observeSpawn(snap(10,204)).key.generation==3); ++checks;
    // AOI adapter's active-entity limit is lower than the bridge's. The
    // rejected spawn must not consume a generation or leave an active slot.
    WorldServerIntegrationAdapter bounded({8,2,1,2},{1,8,8,1,1},true);
    LegacyJxAoiLifecycleBridge reject(bounded,1000,100,2,2,true);
    CHECK(reject.bindOwnerToCurrentThread()); ++checks;
    CHECK(reject.observeSpawn(snap()).status==LegacyNpcSyncStatus::Ok); ++checks;
    auto rejected=reject.observeSpawn(snap(11,205));
    CHECK(rejected.status==LegacyNpcSyncStatus::AoiRejected &&
          rejected.aoiStatus==AoiStatus::Invalid); ++checks;
    CHECK(reject.activeCount()==1 && reject.lastGeneration(205)==0); ++checks;
    CHECK(reject.observeRemove(10,204).status==LegacyNpcSyncStatus::Ok); ++checks;
    CHECK(reject.observeSpawn(snap(11,205)).key.generation==1); ++checks;
    std::cout << "LEGACY_JX_LIFECYCLE_PASS " << checks << "/" << checks << "\n";
    return 0;
}

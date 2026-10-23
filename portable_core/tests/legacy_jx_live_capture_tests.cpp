#include "mmo/LegacyJxLiveSnapshotCapture.h"
#include <iostream>
#include <thread>
using namespace mmo::portable;
#define CHECK(expr) do { if (!(expr)) { std::cerr << "CHECK FAILED " << __LINE__ << ": " #expr "\n"; return 1; } } while(false)

struct FakeLegacyNpc {
    std::uint32_t m_dwID=0;
    int m_SubWorldIndex=0, m_RegionIndex=-1;
    int x=0, y=0, reportedIndex=0, calls=0;
    void GetMpsPos(int* px,int* py,int* idx) {
        ++calls; *px=x; *py=y; *idx=reportedIndex;
    }
};
struct FakeLegacyWorld {
    int m_SubWorldID=-1;
    void* m_Region=nullptr;
    int m_nTotalRegion=0;
};
int main() {
    int count=0;
    WorldServerIntegrationAdapter a({4,1,4,10},{1,8,8,1,1},true);
    LegacyJxAoiLifecycleBridge bridge(a,4,3,3,8,true);
    FakeLegacyNpc npcs[4];
    FakeLegacyWorld worlds[3];
    npcs[1].m_dwID=500;
    npcs[1].m_SubWorldIndex=2;
    npcs[1].m_RegionIndex=3;
    npcs[1].x=2950; npcs[1].y=360; npcs[1].reportedIndex=2;
    worlds[2].m_SubWorldID=53;
    worlds[2].m_Region=reinterpret_cast<void*>(0x1);
    worlds[2].m_nTotalRegion=40;
    LegacyJxNpcSnapshot snap;
    auto read=[&]() {return captureLegacyJxNpcOnOwner(
        bridge,npcs,4,worlds,3,1,0,true,snap);};
    CHECK(read()==LegacyJxCaptureStatus::WrongThread); ++count;
    CHECK(npcs[1].calls==0); ++count;
    CHECK(bridge.bindOwnerToCurrentThread()); ++count;
    CHECK(read()==LegacyJxCaptureStatus::Ok); ++count;
    CHECK(snap.mapId==53 && snap.getMpsMapIndex==2 &&
        snap.subWorldArrayIndex==2 && snap.npcDwId==500); ++count;
    CHECK(snap.mpsX==2950 && snap.mpsY==360 && npcs[1].calls==1); ++count;
    auto spawned=bridge.observeSpawn(snap);
    CHECK(spawned.status==LegacyNpcSyncStatus::Ok &&
        spawned.key.worldId==53 && spawned.key.generation==1); ++count;
    npcs[1].x=2999; npcs[1].y=380;
    CHECK(read()==LegacyJxCaptureStatus::Ok &&
        bridge.observeMove(snap).status==LegacyNpcSyncStatus::Ok); ++count;
    CHECK(bridge.observeRemove(1,500).status==LegacyNpcSyncStatus::Ok); ++count;
    // Slot is recycled by legacy KNpcSet::Remove/RemoveAll.
    npcs[1].m_dwID=500;
    CHECK(read()==LegacyJxCaptureStatus::Ok &&
        bridge.observeSpawn(snap).key.generation==2); ++count;
    auto before=snap;
    npcs[1].reportedIndex=53; // bug: treat reported index as actual map ID
    CHECK(read()==LegacyJxCaptureStatus::MpsMapIndexMismatch); ++count;
    CHECK(snap.getMpsMapIndex==before.getMpsMapIndex); ++count;
    npcs[1].reportedIndex=2;
    npcs[1].m_RegionIndex=-1;
    CHECK(read()==LegacyJxCaptureStatus::InvalidRegion); ++count;
    npcs[1].m_RegionIndex=3;
    worlds[2].m_Region=nullptr;
    CHECK(read()==LegacyJxCaptureStatus::UnloadedWorld); ++count;
    worlds[2].m_Region=reinterpret_cast<void*>(0x1);
    worlds[2].m_SubWorldID=0;
    CHECK(read()==LegacyJxCaptureStatus::InvalidActualMapId); ++count;
    worlds[2].m_SubWorldID=53;
    npcs[1].m_SubWorldIndex=4;
    CHECK(read()==LegacyJxCaptureStatus::InvalidSubWorld); ++count;
    npcs[1].m_SubWorldIndex=2;
    npcs[1].m_dwID=0;
    CHECK(read()==LegacyJxCaptureStatus::InactiveNpc); ++count;
    npcs[1].m_dwID=500;
    CHECK(captureLegacyJxNpcOnOwner(bridge,npcs,4,worlds,3,0,0,true,snap)
        ==LegacyJxCaptureStatus::InvalidNpcSlot); ++count;
    CHECK(captureLegacyJxNpcOnOwner(bridge,
        static_cast<FakeLegacyNpc*>(nullptr),4,worlds,3,1,0,true,snap)
        ==LegacyJxCaptureStatus::MissingArrays); ++count;
    int calls=npcs[1].calls;
    LegacyJxCaptureStatus wrong=LegacyJxCaptureStatus::Ok;
    std::thread worker([&] {wrong=read();}); worker.join();
    CHECK(wrong==LegacyJxCaptureStatus::WrongThread); ++count;
    CHECK(npcs[1].calls==calls); ++count;
    std::cout << "LEGACY_JX_LIVE_CAPTURE_PASS " << count << "/" << count << "\n";
    return 0;
}

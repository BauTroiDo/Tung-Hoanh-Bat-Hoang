#include "mmo/LegacyJxLiveSnapshotCapture.h"
#include "../../legacy_jx/gameserver/Sources/Core/Src/Jx3dM1DevAoiHooks.h"
#include <iostream>
#include <thread>
using namespace mmo::portable;
#define CHECK(e) do { if (!(e)) { std::cerr << "CHECK FAILED at " << __LINE__ << ": " #e "\n"; return 1; } ++cases; } while (false)

struct FakeNpc {
    unsigned int m_dwID=0;
    int m_SubWorldIndex=0;
    int m_RegionIndex=-1;
    int x=0,y=0,index=0;
    void GetMpsPos(int* px,int* py,int* pi) { *px=x;*py=y;*pi=index; }
};
struct FakeWorld {
    int m_SubWorldID=-1;
    void* m_Region=nullptr;
    int m_nTotalRegion=0;
};
struct Context {
    LegacyJxAoiLifecycleBridge& bridge;
    FakeNpc* npcs;
    FakeWorld* worlds;
    int spawns=0,moves=0,removes=0,ticks=0,errors=0;
};

static void doSpawn(void* ptr,int slot) {
    Context& c=*static_cast<Context*>(ptr);
    LegacyJxNpcSnapshot s;
    if (captureLegacyJxNpcOnOwner(c.bridge,c.npcs,8,c.worlds,3,slot,0,true,s)
            !=LegacyJxCaptureStatus::Ok ||
        c.bridge.observeSpawn(s).status!=LegacyNpcSyncStatus::Ok) ++c.errors;
    else ++c.spawns;
}
static void doMove(void* ptr,int slot) {
    Context& c=*static_cast<Context*>(ptr);
    LegacyJxNpcSnapshot s;
    if (captureLegacyJxNpcOnOwner(c.bridge,c.npcs,8,c.worlds,3,slot,0,true,s)
            !=LegacyJxCaptureStatus::Ok ||
        c.bridge.observeMove(s).status!=LegacyNpcSyncStatus::Ok) ++c.errors;
    else ++c.moves;
}
static void doRemove(void* ptr,int slot,unsigned int id) {
    Context& c=*static_cast<Context*>(ptr);
    if (c.bridge.observeRemove(slot,id).status!=LegacyNpcSyncStatus::Ok)
        ++c.errors;
    else ++c.removes;
}
static void doTick(void* ptr,int actualMapId) {
    Context& c=*static_cast<Context*>(ptr);
    if (actualMapId!=53) ++c.errors;
    else ++c.ticks;
}
int main() {
    int cases=0;
    WorldServerIntegrationAdapter adapter({8,2,4,8},{1,8,8,1,1},true);
    LegacyJxAoiLifecycleBridge bridge(adapter,8,3,4,8,true);
    FakeNpc npcs[8]; FakeWorld worlds[3];
    worlds[2].m_SubWorldID=53;
    worlds[2].m_Region=reinterpret_cast<void*>(0x1);
    worlds[2].m_nTotalRegion=10;
    npcs[1].m_dwID=301;npcs[1].m_SubWorldIndex=2;npcs[1].m_RegionIndex=3;
    npcs[1].x=2950;npcs[1].y=360;npcs[1].index=2;
    Context ctx{bridge,npcs,worlds};
    // Unregistered hooks are no-ops even when the build flag is set.
    JX3D_M1_DEV_AOI_EMIT_SPAWN(1);
    CHECK(ctx.spawns==0);
    CHECK(bridge.bindOwnerToCurrentThread());
    jx3d_m1_dev_aoi::Callbacks cb;
    cb.context=&ctx;cb.onSpawn=&doSpawn;cb.onMove=&doMove;
    cb.onRemove=&doRemove;cb.onTick=&doTick;
    CHECK(jx3d_m1_dev_aoi::registerOnWorldOwner(cb));
    CHECK(!jx3d_m1_dev_aoi::registerOnWorldOwner(cb));
    JX3D_M1_DEV_AOI_EMIT_SPAWN(1);
    CHECK(ctx.spawns==1 && bridge.activeCount()==1 && bridge.lastGeneration(301)==1);
    npcs[1].x=2965;npcs[1].y=370;
    JX3D_M1_DEV_AOI_EMIT_MOVE(1);
    CHECK(ctx.moves==1 && ctx.errors==0);
    JX3D_M1_DEV_AOI_EMIT_TICK(53);
    CHECK(ctx.ticks==1);
    int oldSpawnCount=ctx.spawns;
    std::thread wrongThread([&] {JX3D_M1_DEV_AOI_EMIT_SPAWN(1);JX3D_M1_DEV_AOI_EMIT_REMOVE(1,301);});
    wrongThread.join();
    CHECK(ctx.spawns==oldSpawnCount && ctx.removes==0 && bridge.activeCount()==1);
    JX3D_M1_DEV_AOI_EMIT_REMOVE(1,301);
    CHECK(ctx.removes==1 && bridge.activeCount()==0);
    JX3D_M1_DEV_AOI_EMIT_SPAWN(1);
    CHECK(ctx.spawns==2 && bridge.lastGeneration(301)==2);
    CHECK(ctx.errors==0);
    CHECK(jx3d_m1_dev_aoi::unregisterOnWorldOwner());
    JX3D_M1_DEV_AOI_EMIT_REMOVE(1,301);
    CHECK(ctx.removes==1 && bridge.activeCount()==1);
    // The explicit observer remains responsible for teardown when deregistering.
    CHECK(bridge.observeRemove(1,301).status==LegacyNpcSyncStatus::Ok);
    std::cout << "LEGACY_JX_DEV_HOOK_DISPATCH_PASS " << cases << "/" << cases << "\n";
    return 0;
}

#pragma once
#if defined(_SERVER) && defined(JX3D_M1_DEV_AOI)
#include "Jx3dM1DevAoiHooks.h"
#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max
#include "M1Portable/include/mmo/LegacyJxLiveSnapshotCapture.h"
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <vector>

namespace jx3d_m1_owner {
using namespace mmo::portable;
struct Slot { unsigned int id=0; unsigned int instance=0; long long map=0; };
struct Event { const char* kind; int slot; unsigned int id; unsigned int generation;
    unsigned int instance; long long map; long long x,y; };
struct State {
    WorldServerIntegrationAdapter adapter;
    LegacyJxAoiLifecycleBridge bridge;
    std::vector<Slot> slots;
    Event events[128]; unsigned int eventCount=0;
    unsigned long long omitted=0,spawn=0,bootstrap=0,move=0,remove=0,ticks=0,bad=0;
    unsigned int scan=1; DWORD owner=0; ULONGLONG lastLog=0;
    FILE* log=nullptr; bool stopped=false,testAttempted=false;
    static WorldAdapterLimits limits() {
        WorldAdapterLimits v; v.queueCapacity=16; v.commandsPerDrain=16;
        v.maxSessionIdentities=1; v.maxEntityIdentities=MAX_NPC;
        v.maxEventsPerSession=128; return v;
    }
    static AoiPoolLimits poolLimits() {
        AoiPoolLimits v; v.maxSessions=1; v.maxPendingBytes=65536;
        v.maxBytesPerTick=65536; v.maxAttemptsPerTick=1; v.minRetryNs=1000000; return v;
    }
    State():adapter(limits(),poolLimits(),true),
        bridge(adapter,MAX_NPC,MAX_SUBWORLD,MAX_NPC,MAX_NPC*64u,true),slots(MAX_NPC) {}
    void record(const char* kind,int slot,const LegacyNpcSyncResult& r,
                const LegacyJxNpcSnapshot* s=nullptr) {
        if(eventCount==128) {++omitted;return;}
        Event& e=events[eventCount++]; e.kind=kind;e.slot=slot;
        e.id=(unsigned int)r.key.runtimeId;e.generation=r.key.generation;
        e.instance=r.key.instanceId;e.map=r.key.worldId;
        e.x=s?s->mpsX:0;e.y=s?s->mpsY:0;
    }
    void erase(int slot,unsigned int id,const char* why) {
        if(slot<=0||slot>=MAX_NPC||!slots[slot].id) return;
        auto r=bridge.observeRemove(slot,id);
        if(r.status!=LegacyNpcSyncStatus::Ok) {++bad;return;}
        slots[slot]=Slot();++remove;record(why,slot,r);
    }
    void sample(int slot,const char* cause,bool moving=false) {
        if(!bridge.isOwnerThread()||slot<=0||slot>=MAX_NPC) return;
        LegacyJxNpcSnapshot s;
        // SubWorld array index is the process-local instance namespace. The
        // configured map ID is captured separately, never confused with index.
        auto capture=captureLegacyJxNpcOnOwner(bridge,Npc,MAX_NPC,SubWorld,MAX_SUBWORLD,
                                             slot,0,true,s);
        if(capture!=LegacyJxCaptureStatus::Ok) {++bad;return;}
        s.instanceId=(unsigned int)s.subWorldArrayIndex+1;
        Slot& old=slots[slot];
        if(old.id && (old.id!=s.npcDwId||old.map!=s.mapId||old.instance!=s.instanceId))
            erase(slot,old.id,"identity_remove");
        if(!slots[slot].id) {
            auto r=bridge.observeSpawn(s);
            if(r.status!=LegacyNpcSyncStatus::Ok) {++bad;return;}
            slots[slot].id=s.npcDwId; slots[slot].map=s.mapId; slots[slot].instance=s.instanceId;
            if(cause[0]=='b') ++bootstrap; else ++spawn;
            record(cause,slot,r,&s);
        } else if(moving) {
            auto r=bridge.observeMove(s);
            if(r.status!=LegacyNpcSyncStatus::Ok) {++bad;return;}
            ++move;record("move",slot,r,&s);
        }
    }
    void flush(bool final=false) {
        if(!log) return;
        ULONGLONG now=GetTickCount64();
        if(!final && now-lastLog<5000) return;
        for(unsigned int i=0;i<eventCount;++i) {
            const Event& e=events[i];
            fprintf(log,"{\"event\":\"%s\",\"slot\":%d,\"id\":%u,\"generation\":%u,\"instance\":%u,\"map\":%lld,\"x\":%lld,\"y\":%lld}\n",
                    e.kind,e.slot,e.id,e.generation,e.instance,e.map,e.x,e.y);
        }
        fprintf(log,"{\"event\":\"%s\",\"pid\":%lu,\"owner_tid\":%lu,\"ticks\":%llu,\"active\":%zu,\"bootstrap\":%llu,\"spawn\":%llu,\"move\":%llu,\"remove\":%llu,\"rejected_samples\":%llu,\"omitted_events\":%llu}\n",
                final?"stop":"summary",GetCurrentProcessId(),owner,ticks,bridge.activeCount(),bootstrap,spawn,move,remove,bad,omitted);
        fflush(log);eventCount=0;lastLog=now;
    }
    void testOnce() {
        if(testAttempted) return;
        testAttempted=true;
        char opt[8]={};
        if(GetEnvironmentVariableA("JX3D_M1_DEV_AOI_TEST",opt,sizeof(opt))!=1||opt[0]!='1') return;
        // A one-shot exercise of the real NPC methods, only in an empty world.
        // No character objects, combat, loot, scripts, or DB calls are used.
        for(int i=1;i<MAX_NPC;++i) if(Npc[i].m_dwID&&Npc[i].m_Kind==kind_player) {
            fprintf(log,"{\"event\":\"native_test_skipped\",\"reason\":\"player_present\"}\n");return;
        }
        if(!g_SubWorldSet.EnsureMapLoadedById(53)) {
            fprintf(log,"{\"event\":\"native_test_skipped\",\"reason\":\"map53_unavailable\"}\n");return;
        }
        int exemplar=0;
        for(int i=1;i<MAX_NPC;++i) {
            const KNpc& n=Npc[i];
            if(n.m_dwID&&n.m_SubWorldIndex>=0&&n.m_SubWorldIndex<MAX_SUBWORLD&&
               SubWorld[n.m_SubWorldIndex].m_SubWorldID==53&&n.m_RegionIndex>=0&&
               n.m_RegionIndex<SubWorld[n.m_SubWorldIndex].m_nTotalRegion&&
               n.m_NpcSettingIdx>0&&n.m_NpcSettingIdx<32768&&n.m_Level>0&&n.m_Level<=65535) {
                exemplar=i;break;
            }
        }
        if(!exemplar) {fprintf(log,"{\"event\":\"native_test_skipped\",\"reason\":\"no_map53_template\"}\n");return;}
        KNpc& n=Npc[exemplar];
        if(SubWorld[n.m_SubWorldIndex].m_nCellWidth<2) return;
        const unsigned int packed=((unsigned int)n.m_NpcSettingIdx<<16)|(unsigned int)n.m_Level;
        lastLog=0;flush(); // preserve the following three real lifecycle events
        const size_t before=bridge.activeCount();
        const auto spawnBefore=spawn,moveBefore=move,removeBefore=remove;
        int slot=NpcSet.AddServerNpcB((int)packed,n.m_SubWorldIndex,n.m_RegionIndex,
                                     n.m_MapX,n.m_MapY,n.m_OffX,n.m_OffY,0);
        if(slot<=0||slot>=MAX_NPC) {fprintf(log,"{\"event\":\"native_test_failed\",\"reason\":\"add_failed\"}\n");return;}
        struct Cleanup {int slot;~Cleanup(){if(slot>0) NpcSet.Remove(slot,FALSE);}} cleanup{slot};
        const unsigned int id=Npc[slot].m_dwID;
        int x0=0,y0=0,w0=-1,x1=0,y1=0,w1=-1;
        Npc[slot].GetMpsPos(&x0,&y0,&w0);
        // Use the real public movement command and normal status handler;
        // this reaches private ServeMove/MovePos without bypassing path checks.
        Npc[slot].SendSerCommand(do_walk,x0+32,y0);
        Npc[slot].ProcCommand(1);
        for(int step=0;step<8&&move==moveBefore;++step) Npc[slot].ProcStatus();
        Npc[slot].GetMpsPos(&x1,&y1,&w1);
        NpcSet.Remove(slot,FALSE);cleanup.slot=0;
        const bool ok=spawn==spawnBefore+1&&move>moveBefore&&remove==removeBefore+1&&
                      bridge.activeCount()==before&&w0==w1&&(x0!=x1||y0!=y1);
        fprintf(log,"{\"event\":\"native_test_result\",\"pass\":%s,\"map\":53,\"slot\":%d,\"id\":%u,\"x0\":%d,\"x1\":%d,\"active_before\":%zu,\"active_after\":%zu,\"client_aoi_pass\":false}\n",
                ok?"true":"false",slot,id,x0,x1,before,bridge.activeCount());
        lastLog=0;flush();
    }
};
inline State*& state() {static State* s=nullptr;return s;}
inline void spawnCallback(void* p,int slot) {static_cast<State*>(p)->sample(slot,"spawn");}
inline void moveCallback(void* p,int slot) {static_cast<State*>(p)->sample(slot,"move_seed",true);}
inline void removeCallback(void* p,int slot,unsigned int id) {static_cast<State*>(p)->erase(slot,id,"remove");}
inline void tickCallback(void* p,int) {++static_cast<State*>(p)->ticks;}
inline void begin() noexcept {
    static bool attempted=false;
    if(attempted) return;
    attempted=true;
    char enabled[8]={};
    if(GetEnvironmentVariableA("JX3D_M1_DEV_AOI",enabled,sizeof(enabled))!=1||enabled[0]!='1') return;
    try {
        std::unique_ptr<State> s(new State);
        if(!s->bridge.bindOwnerToCurrentThread()) return;
        char path[MAX_PATH];
        sprintf_s(path,"Logs\\jx3d-m1-aoi-%lu.jsonl",GetCurrentProcessId());
        fopen_s(&s->log,path,"ab");
        if(!s->log) return;
        s->owner=GetCurrentThreadId();
        jx3d_m1_dev_aoi::Callbacks cb;
        cb.context=s.get();cb.onSpawn=spawnCallback;cb.onMove=moveCallback;
        cb.onRemove=removeCallback;cb.onTick=tickCallback;
        if(!jx3d_m1_dev_aoi::registerOnWorldOwner(cb)) {fclose(s->log);return;}
        state()=s.release();
        fprintf(state()->log,"{\"event\":\"registered\",\"pid\":%lu,\"owner_tid\":%lu,\"max_npc\":%d,\"max_subworld\":%d,\"network_delivery\":false}\n",GetCurrentProcessId(),GetCurrentThreadId(),MAX_NPC,MAX_SUBWORLD);
        // Existing static NPCs were created before the first owner tick.
        for(int i=1;i<MAX_NPC;++i) if(Npc[i].m_dwID) state()->sample(i,"bootstrap");
        state()->flush();
    } catch(...) {
        jx3d_m1_dev_aoi::unregisterOnWorldOwner();
        // Never let an optional observer unwind gameplay.
    }
}
inline void end() noexcept {
    State* s=state();
    if(!s||s->stopped||!s->bridge.isOwnerThread()) return;
    if(!jx3d_m1_dev_aoi::isActiveOnOwner()) {s->stopped=true;s->flush(true);return;}
    try {
        s->testOnce();
        // Bounded reconciliation covers NPCs loaded outside AddServerNpcB and
        // valid map transfers. Invalid position samples never imply despawn.
        for(int n=0;n<256;++n) {
            unsigned int i=s->scan++;
            if(s->scan>=MAX_NPC) s->scan=1;
            if(s->slots[i].id && (!Npc[i].m_dwID||Npc[i].m_dwID!=s->slots[i].id))
                s->erase(i,s->slots[i].id,"reconcile_remove");
            if(Npc[i].m_dwID) s->sample(i,"reconcile_spawn");
        }
        s->flush();
    } catch(...) {jx3d_m1_dev_aoi::unregisterOnWorldOwner();s->stopped=true;}
}
inline void stop() noexcept {
    State* s=state();
    if(!s||s->stopped||!s->bridge.isOwnerThread()) return;
    jx3d_m1_dev_aoi::unregisterOnWorldOwner();s->stopped=true;
    s->flush(true);if(s->log) {fclose(s->log);s->log=nullptr;}
    // State is retained until process exit: no shared legacy pointers are
    // deleted while shutdown may still contain worker callbacks.
}
}
#pragma pop_macro("max")
#pragma pop_macro("min")
#else
namespace jx3d_m1_owner { inline void begin(){} inline void end(){} inline void stop(){} }
#endif

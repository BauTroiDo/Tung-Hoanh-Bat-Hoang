#pragma once
// Explicit DEV-only callback registry. No callbacks run on foreign threads.
#if defined(_SERVER) && defined(JX3D_M1_DEV_AOI)
#include <thread>
#include <mutex>
namespace jx3d_m1_dev_aoi {
struct Callbacks {
    void* context=nullptr;
    void (*onSpawn)(void*,int)=nullptr;
    void (*onMove)(void*,int)=nullptr;
    void (*onRemove)(void*,int,unsigned int)=nullptr;
    void (*onTick)(void*,int)=nullptr;
};
struct Registry {
    std::recursive_mutex mutex;
    Callbacks callbacks;
    std::thread::id owner;
    bool active=false,invoking=false;
};
inline Registry& registry() {static Registry r;return r;}
inline bool onOwner(const Registry& r) {return r.active&&r.owner==std::this_thread::get_id();}
inline bool registerOnWorldOwner(const Callbacks& cb) {
    Registry& r=registry();std::lock_guard<std::recursive_mutex> lock(r.mutex);
    if(r.active||r.invoking||!cb.context||!cb.onSpawn||!cb.onMove||!cb.onRemove||!cb.onTick) return false;
    r.owner=std::this_thread::get_id();r.callbacks=cb;r.active=true;return true;
}
inline bool unregisterOnWorldOwner() {
    Registry& r=registry();std::lock_guard<std::recursive_mutex> lock(r.mutex);
    if(!onOwner(r)||r.invoking) return false;
    r.active=false;r.callbacks=Callbacks();return true;
}
inline bool isActiveOnOwner() {
    Registry& r=registry();std::lock_guard<std::recursive_mutex> lock(r.mutex);return onOwner(r);
}
template<class Invoke> inline void dispatch(Invoke fn) {
    Registry& r=registry();
    // Foreign workers do not block the world owner, inspect Npc[], or race
    // callback publication/teardown. A busy or recursive dispatch is dropped.
    std::unique_lock<std::recursive_mutex> lock(r.mutex,std::try_to_lock);
    if(!lock.owns_lock()||!onOwner(r)||r.invoking) return;
    r.invoking=true;
    try {fn(r.callbacks);} catch(...) {r.active=false;}
    r.invoking=false;
}
inline void emitSpawn(int slot) {dispatch([=](const Callbacks& c){c.onSpawn(c.context,slot);});}
inline void emitMove(int slot) {dispatch([=](const Callbacks& c){c.onMove(c.context,slot);});}
inline void emitRemove(int slot,unsigned int id) {dispatch([=](const Callbacks& c){c.onRemove(c.context,slot,id);});}
inline void emitTick(int world) {dispatch([=](const Callbacks& c){c.onTick(c.context,world);});}
}
#define JX3D_M1_DEV_AOI_EMIT_SPAWN(slot) ::jx3d_m1_dev_aoi::emitSpawn(slot)
#define JX3D_M1_DEV_AOI_EMIT_MOVE(slot) ::jx3d_m1_dev_aoi::emitMove(slot)
#define JX3D_M1_DEV_AOI_EMIT_REMOVE(slot,id) ::jx3d_m1_dev_aoi::emitRemove(slot,id)
#define JX3D_M1_DEV_AOI_EMIT_TICK(world) ::jx3d_m1_dev_aoi::emitTick(world)
#else
#define JX3D_M1_DEV_AOI_EMIT_SPAWN(slot) ((void)0)
#define JX3D_M1_DEV_AOI_EMIT_MOVE(slot) ((void)0)
#define JX3D_M1_DEV_AOI_EMIT_REMOVE(slot,id) ((void)0)
#define JX3D_M1_DEV_AOI_EMIT_TICK(world) ((void)0)
#endif

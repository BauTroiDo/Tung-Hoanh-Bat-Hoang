#pragma once
// M1 GAME-02 DEV-only observational hooks. No network, no DB and no mutations.
// Disabled by default. Enable ONLY in a separately rebuilt DEV server with
// _SERVER and JX3D_M1_DEV_AOI defined; explicitly register on world owner thread.
#if defined(_SERVER) && defined(JX3D_M1_DEV_AOI)
#include <thread>
namespace jx3d_m1_dev_aoi {
struct Callbacks {
    void* context;
    void (*onSpawn)(void*, int);
    void (*onMove)(void*, int);
    void (*onRemove)(void*, int, unsigned int);
    void (*onTick)(void*, int);
    Callbacks():context(0),onSpawn(0),onMove(0),onRemove(0),onTick(0){}
};
struct Registry {
    Callbacks callbacks;
    std::thread::id owner;
    bool active;
    bool invoking;
    Registry():owner(),active(false),invoking(false){}
};
inline Registry& registry() {
    // Function-local static is a single logical instance across TUs (C++11+).
    static Registry r;
    return r;
}
inline bool registerOnWorldOwner(const Callbacks& cb) {
    Registry& r=registry();
    if (r.active || !cb.context || !cb.onSpawn || !cb.onMove ||
        !cb.onRemove || !cb.onTick) return false;
    r.owner=std::this_thread::get_id();
    r.callbacks=cb;
    r.active=true;
    return true;
}
inline bool onOwner(const Registry& r) {
    return r.active && r.owner==std::this_thread::get_id();
}
inline bool unregisterOnWorldOwner() {
    Registry& r=registry();
    if (!onOwner(r) || r.invoking) return false;
    r.active=false;
    r.callbacks=Callbacks();
    return true;
}
inline bool enter(Registry& r) {
    if (!onOwner(r) || r.invoking) return false;
    r.invoking=true;
    return true;
}
inline void emitSpawn(int slot) {
    Registry& r=registry();
    if (!enter(r)) return;
    try { r.callbacks.onSpawn(r.callbacks.context,slot); }
    catch (...) { r.active=false; }
    r.invoking=false;
}
inline void emitMove(int slot) {
    Registry& r=registry();
    if (!enter(r)) return;
    try { r.callbacks.onMove(r.callbacks.context,slot); }
    catch (...) { r.active=false; }
    r.invoking=false;
}
inline void emitRemove(int slot, unsigned int id) {
    Registry& r=registry();
    if (!enter(r)) return;
    try { r.callbacks.onRemove(r.callbacks.context,slot,id); }
    catch (...) { r.active=false; }
    r.invoking=false;
}
inline void emitTick(int worldId) {
    Registry& r=registry();
    if (!enter(r)) return;
    try { r.callbacks.onTick(r.callbacks.context,worldId); }
    catch (...) { r.active=false; }
    r.invoking=false;
}
} // namespace jx3d_m1_dev_aoi
#define JX3D_M1_DEV_AOI_EMIT_SPAWN(slot) ::jx3d_m1_dev_aoi::emitSpawn(slot)
#define JX3D_M1_DEV_AOI_EMIT_MOVE(slot) ::jx3d_m1_dev_aoi::emitMove(slot)
#define JX3D_M1_DEV_AOI_EMIT_REMOVE(slot,id) ::jx3d_m1_dev_aoi::emitRemove(slot,id)
#define JX3D_M1_DEV_AOI_EMIT_TICK(world) ::jx3d_m1_dev_aoi::emitTick(world)
#else
// No bytecode generated, no function definitions, no dependencies in shipping
// builds and in clients compiled without _SERVER.
#define JX3D_M1_DEV_AOI_EMIT_SPAWN(slot) ((void)0)
#define JX3D_M1_DEV_AOI_EMIT_MOVE(slot) ((void)0)
#define JX3D_M1_DEV_AOI_EMIT_REMOVE(slot,id) ((void)0)
#define JX3D_M1_DEV_AOI_EMIT_TICK(world) ((void)0)
#endif

#pragma once
// DEV-only lifecycle bridge between authoritative legacy KNpc snapshots and
// WorldServerIntegrationAdapter. No legacy globals or client-origin identities.
// NOT wired to production KSubWorld/KNpcSet; opt-in construction only.
#include "mmo/LegacyJxAoiSnapshot.h"
#include "mmo/WorldServerIntegrationAdapter.h"
#include <cstdint>
#include <limits>
#include <thread>
#include <unordered_map>
#include <unordered_set>

namespace mmo { namespace portable {

enum class LegacyNpcSyncStatus {
    Ok, Disabled, Unbound, WrongThread, BadSnapshot, AlreadyActive,
    SlotNotActive, StaleIdentity, MapChanged, Capacity, GenerationOverflow,
    RevisionOverflow, AoiRejected
};

struct LegacyNpcSyncResult {
    LegacyNpcSyncStatus status = LegacyNpcSyncStatus::BadSnapshot;
    AoiStatus aoiStatus = AoiStatus::Invalid;
    EntityKey key;
};

// A bridge instance is owned by exactly one WorldServer owner thread.
// ObserveSpawn must happen only AFTER legacy KNpcSet has assigned an ID and
// attached the NPC to a valid region. ObserveRemove must run BEFORE
// KNpcSet::Remove/RemoveAll clears the slot. A map/instance transfer is a
// remove+spawn, never an in-place change of the EntityKey.
class LegacyJxAoiLifecycleBridge final {
public:
    LegacyJxAoiLifecycleBridge(WorldServerIntegrationAdapter& aoi,
        std::int32_t maxNpcs, std::int32_t maxSubWorlds,
        std::size_t maxActiveSlots, std::size_t maxIdHistory,
        bool enabled = false)
        : adapter_(aoi), maxNpcs_(maxNpcs), maxSubWorlds_(maxSubWorlds),
          maxActive_(maxActiveSlots), maxHistory_(maxIdHistory), enabled_(enabled) {}

    bool bindOwnerToCurrentThread() {
        if (!enabled_) return false;
        if (bound_) return owner();
        if (!adapter_.bindOwnerToCurrentThread()) return false;
        thread_ = std::this_thread::get_id();
        bound_ = true;
        return true;
    }

    LegacyNpcSyncResult observeSpawn(LegacyJxNpcSnapshot snapshot) {
        const auto gate = ownerGate();
        if (gate != LegacyNpcSyncStatus::Ok) return {gate};
        if (active_.count(snapshot.npcArrayIndex) ||
            activeIds_.count(snapshot.npcDwId)) return {LegacyNpcSyncStatus::AlreadyActive};
        if (active_.size() >= maxActive_) return {LegacyNpcSyncStatus::Capacity};
        const auto prior = generations_.find(snapshot.npcDwId);
        if (prior == generations_.end() && generations_.size() >= maxHistory_)
            return {LegacyNpcSyncStatus::Capacity};
        if (prior != generations_.end() &&
            prior->second == std::numeric_limits<std::uint32_t>::max())
            return {LegacyNpcSyncStatus::GenerationOverflow};
        snapshot.spawnGeneration =
            prior == generations_.end() ? 1u : prior->second + 1;
        snapshot.revision = 1;
        AoiEntity entity;
        if (convertLegacyJxNpcSnapshot(snapshot, maxNpcs_, maxSubWorlds_, entity)
                != LegacyJxSnapshotStatus::Ok)
            return {LegacyNpcSyncStatus::BadSnapshot};
        // All bookkeeping is allocated before touching the SpatialAoi world.
        auto history = generations_.emplace(snapshot.npcDwId, snapshot.spawnGeneration);
        auto id = activeIds_.insert(snapshot.npcDwId);
        auto slot = active_.emplace(snapshot.npcArrayIndex,
            Active{entity.key, 1, snapshot.npcDwId});
        const auto status = adapter_.upsert(entity);
        if (status != AoiStatus::Ok) {
            active_.erase(slot.first);
            activeIds_.erase(id.first);
            if (history.second) generations_.erase(history.first);
            return {LegacyNpcSyncStatus::AoiRejected, status};
        }
        if (!history.second) history.first->second = snapshot.spawnGeneration;
        return {LegacyNpcSyncStatus::Ok, status, entity.key};
    }

    LegacyNpcSyncResult observeMove(LegacyJxNpcSnapshot snapshot) {
        const auto gate = ownerGate();
        if (gate != LegacyNpcSyncStatus::Ok) return {gate};
        const auto it = active_.find(snapshot.npcArrayIndex);
        if (it == active_.end()) return {LegacyNpcSyncStatus::SlotNotActive};
        Active& current = it->second;
        if (snapshot.npcDwId != current.npcDwId)
            return {LegacyNpcSyncStatus::StaleIdentity};
        if (snapshot.mapId <= 0 || static_cast<std::uint64_t>(snapshot.mapId) != current.key.worldId ||
            snapshot.instanceId != current.key.instanceId)
            return {LegacyNpcSyncStatus::MapChanged};
        if (current.revision == std::numeric_limits<std::uint64_t>::max())
            return {LegacyNpcSyncStatus::RevisionOverflow};
        snapshot.spawnGeneration = current.key.generation;
        snapshot.revision = current.revision + 1;
        AoiEntity entity;
        if (convertLegacyJxNpcSnapshot(snapshot, maxNpcs_, maxSubWorlds_, entity)
                != LegacyJxSnapshotStatus::Ok)
            return {LegacyNpcSyncStatus::BadSnapshot};
        const auto result = adapter_.upsert(entity);
        if (result != AoiStatus::Ok)
            return {LegacyNpcSyncStatus::AoiRejected, result};
        current.revision = snapshot.revision;
        return {LegacyNpcSyncStatus::Ok, result, entity.key};
    }

    LegacyNpcSyncResult observeRemove(std::int32_t slotIndex, std::uint32_t npcDwId) {
        const auto gate = ownerGate();
        if (gate != LegacyNpcSyncStatus::Ok) return {gate};
        const auto it = active_.find(slotIndex);
        if (it == active_.end()) return {LegacyNpcSyncStatus::SlotNotActive};
        if (it->second.npcDwId != npcDwId)
            return {LegacyNpcSyncStatus::StaleIdentity};
        const EntityKey key = it->second.key;
        const auto status = adapter_.erase(key);
        if (status != AoiStatus::Ok)
            return {LegacyNpcSyncStatus::AoiRejected, status};
        activeIds_.erase(npcDwId);
        active_.erase(it);
        return {LegacyNpcSyncStatus::Ok, status, key};
    }

    std::size_t activeCount() const { return owner() ? active_.size() : 0; }
    std::uint32_t lastGeneration(std::uint32_t id) const {
        if (!owner()) return 0;
        const auto it = generations_.find(id);
        return it == generations_.end() ? 0 : it->second;
    }

private:
    struct Active { EntityKey key; std::uint64_t revision; std::uint32_t npcDwId; };
    LegacyNpcSyncStatus ownerGate() const {
        if (!enabled_) return LegacyNpcSyncStatus::Disabled;
        if (!bound_) return LegacyNpcSyncStatus::Unbound;
        return owner() ? LegacyNpcSyncStatus::Ok : LegacyNpcSyncStatus::WrongThread;
    }
    bool owner() const { return bound_ && thread_ == std::this_thread::get_id(); }
    WorldServerIntegrationAdapter& adapter_;
    const std::int32_t maxNpcs_;
    const std::int32_t maxSubWorlds_;
    const std::size_t maxActive_;
    const std::size_t maxHistory_;
    const bool enabled_;
    bool bound_ = false;
    std::thread::id thread_;
    std::unordered_map<std::int32_t, Active> active_;
    std::unordered_set<std::uint32_t> activeIds_;
    std::unordered_map<std::uint32_t, std::uint32_t> generations_;
};

}} // namespace mmo::portable

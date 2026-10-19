#pragma once
// Portable server-authoritative 2D AOI planning. No network, OS or JX global state.
// Every mutation and query must run on a single explicitly-bound world thread.
#include <cstddef>
#include <cstdint>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace mmo { namespace portable {

struct EntityKey {
    std::uint64_t runtimeId = 0;
    std::uint32_t generation = 0;
    std::uint32_t worldId = 0;
    std::uint32_t instanceId = 0;
};
bool operator==(const EntityKey& a, const EntityKey& b) noexcept;
bool operator!=(const EntityKey& a, const EntityKey& b) noexcept;
struct EntityKeyHash {
    std::size_t operator()(const EntityKey& key) const noexcept;
};
bool entityKeyLess(const EntityKey& a, const EntityKey& b) noexcept;

struct AoiEntity {
    EntityKey key;
    std::int32_t x = 0;
    std::int32_t y = 0;
    std::uint64_t revision = 0;
    bool visible = true;
};

// One ObserverView per active player/session; never share this object between
// world threads or reuse it for a different EntityKey. Epoch is authoritative
// scene epoch: it must change on teleport/map reset/realm reset.
class AoiObserverView final {
public:
    explicit AoiObserverView(EntityKey viewer) : viewer_(viewer) {}
    const EntityKey& viewer() const noexcept { return viewer_; }
    std::size_t knownCount() const noexcept { return known_.size(); }
    std::uint64_t lastEpoch() const noexcept { return epoch_; }
    bool initialized() const noexcept { return initialized_; }
private:
    friend class SpatialAoi;
    const EntityKey viewer_;
    std::uint64_t epoch_ = 0;
    bool initialized_ = false;
    std::unordered_map<EntityKey, std::uint64_t, EntityKeyHash> known_;
};

enum class AoiStatus {
    Ok, Unbound, WrongThread, Invalid, StaleGeneration, StaleRevision,
    NotFound, TooManyCells, TooManyVisible, EventBudgetExceeded
};

struct AoiDelta {
    enum class Kind { Reset, Remove, Create, Update };
    Kind kind = Kind::Create;
    EntityKey key;
    std::int32_t x = 0;
    std::int32_t y = 0;
    std::uint64_t revision = 0;
    std::uint64_t sceneEpoch = 0;
};
struct AoiPlan {
    AoiStatus status = AoiStatus::Ok;
    std::vector<AoiDelta> events;
    std::size_t totalVisible = 0;
    std::size_t cellsScanned = 0;
};

// Grid is a broad phase; exact Euclidean radius is checked in 64-bit integer
// after coordinate-delta prefiltering. Uses floor division for negative cells.
// This prototype has no line-of-sight/collision checks and no wire serialization.
class SpatialAoi final {
public:
    explicit SpatialAoi(std::uint32_t cellSize = 256,
        std::uint32_t maxViewRadius = 4096, std::size_t maxQueryCells = 4096,
        std::size_t maxVisible = 4096);
    SpatialAoi(const SpatialAoi&) = delete;
    SpatialAoi& operator=(const SpatialAoi&) = delete;

    bool bindOwnerToCurrentThread() noexcept;
    AoiStatus upsert(const AoiEntity& entity);
    AoiStatus erase(const EntityKey& key);
    AoiPlan plan(AoiObserverView& observer, std::int32_t centerX,
        std::int32_t centerY, std::uint32_t radius, std::uint64_t sceneEpoch,
        std::size_t maxEvents);
    std::size_t entityCount() const noexcept { return entities_.size(); }
    std::size_t occupiedCellCount() const noexcept { return cells_.size(); }

private:
    struct CellKey {
        std::uint32_t worldId = 0;
        std::uint32_t instanceId = 0;
        std::int64_t x = 0;
        std::int64_t y = 0;
        bool operator==(const CellKey& b) const noexcept {
            return worldId == b.worldId && instanceId == b.instanceId &&
                x == b.x && y == b.y;
        }
    };
    struct CellHash {
        std::size_t operator()(const CellKey& c) const noexcept;
    };
    struct SlotKey {
        std::uint32_t worldId = 0;
        std::uint32_t instanceId = 0;
        std::uint64_t runtimeId = 0;
        bool operator==(const SlotKey& b) const noexcept {
            return worldId == b.worldId && instanceId == b.instanceId &&
                runtimeId == b.runtimeId;
        }
    };
    struct SlotHash {
        std::size_t operator()(const SlotKey& s) const noexcept;
    };
    struct SlotInfo {
        std::uint32_t highestGeneration = 0;
        bool active = false;
    };

    AoiStatus ownerStatus() const noexcept;
    CellKey cellFor(const AoiEntity& entity) const noexcept;
    void removeActive(const EntityKey& key);
    static bool inRadius(const AoiEntity& entity, std::int32_t x,
        std::int32_t y, std::uint32_t radius) noexcept;

    const std::uint32_t cellSize_;
    const std::uint32_t maxViewRadius_;
    const std::size_t maxQueryCells_;
    const std::size_t maxVisible_;
    bool bound_ = false;
    std::thread::id owner_;
    std::unordered_map<EntityKey, AoiEntity, EntityKeyHash> entities_;
    std::unordered_map<CellKey, std::unordered_set<EntityKey, EntityKeyHash>, CellHash> cells_;
    // Retain generation tombstones after removal to reject stale resurrected NPC IDs.
    std::unordered_map<SlotKey, SlotInfo, SlotHash> slots_;
};
}} // namespace mmo::portable

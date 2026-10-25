#include "mmo/SpatialAoi.h"
#include <algorithm>
#include <limits>
#include <stdexcept>
#include <tuple>
#include <utility>

namespace mmo { namespace portable {
namespace {
std::size_t mix(std::size_t seed, std::uint64_t value) noexcept {
    const auto hashed = std::hash<std::uint64_t>{}(value);
    return seed ^ (hashed + std::size_t(0x9e3779b9u) + (seed << 6) + (seed >> 2));
}
std::int64_t floorCell(std::int64_t coordinate, std::uint32_t cellSize) noexcept {
    const auto size = static_cast<std::int64_t>(cellSize);
    auto q = coordinate / size;
    if (coordinate % size < 0) --q;
    return q;
}
std::int64_t clampCoordinate(std::int64_t n) noexcept {
    return std::max<std::int64_t>(std::numeric_limits<std::int32_t>::min(),
        std::min<std::int64_t>(std::numeric_limits<std::int32_t>::max(), n));
}
using Current = std::unordered_map<EntityKey, AoiEntity, EntityKeyHash>;
void sortEvents(std::vector<AoiDelta>& events) {
    std::sort(events.begin(), events.end(), [](const AoiDelta& a, const AoiDelta& b) {
        if (a.kind != b.kind) return a.kind < b.kind;
        return entityKeyLess(a.key, b.key);
    });
}
} // namespace

bool operator==(const EntityKey& a, const EntityKey& b) noexcept {
    return a.runtimeId == b.runtimeId && a.generation == b.generation &&
        a.worldId == b.worldId && a.instanceId == b.instanceId;
}
bool operator!=(const EntityKey& a, const EntityKey& b) noexcept { return !(a == b); }
bool entityKeyLess(const EntityKey& a, const EntityKey& b) noexcept {
    return std::tie(a.worldId, a.instanceId, a.runtimeId, a.generation) <
           std::tie(b.worldId, b.instanceId, b.runtimeId, b.generation);
}
std::size_t EntityKeyHash::operator()(const EntityKey& k) const noexcept {
    std::size_t seed = 0;
    seed = mix(seed, k.worldId);
    seed = mix(seed, k.instanceId);
    seed = mix(seed, k.runtimeId);
    return mix(seed, k.generation);
}
std::size_t SpatialAoi::CellHash::operator()(const CellKey& c) const noexcept {
    std::size_t seed = 0;
    seed = mix(seed, c.worldId); seed = mix(seed, c.instanceId);
    seed = mix(seed, static_cast<std::uint64_t>(c.x));
    return mix(seed, static_cast<std::uint64_t>(c.y));
}
std::size_t SpatialAoi::SlotHash::operator()(const SlotKey& s) const noexcept {
    std::size_t seed = 0;
    seed = mix(seed, s.worldId); seed = mix(seed, s.instanceId);
    return mix(seed, s.runtimeId);
}

SpatialAoi::SpatialAoi(std::uint32_t cellSize, std::uint32_t maxViewRadius,
    std::size_t maxQueryCells, std::size_t maxVisible)
    : cellSize_(cellSize), maxViewRadius_(maxViewRadius),
      maxQueryCells_(maxQueryCells), maxVisible_(maxVisible) {
    // Signed 64-bit distance squares remain bounded for radius <= INT32_MAX.
    if (cellSize == 0 || maxViewRadius == 0 ||
        maxViewRadius > static_cast<std::uint32_t>(std::numeric_limits<std::int32_t>::max()) ||
        maxQueryCells == 0 || maxVisible == 0)
        throw std::invalid_argument("AOI configuration must have positive limits");
}

bool SpatialAoi::bindOwnerToCurrentThread() noexcept {
    if (!bound_) {
        owner_ = std::this_thread::get_id();
        bound_ = true;
    }
    return owner_ == std::this_thread::get_id();
}
AoiStatus SpatialAoi::ownerStatus() const noexcept {
    if (!bound_) return AoiStatus::Unbound;
    if (owner_ != std::this_thread::get_id()) return AoiStatus::WrongThread;
    return AoiStatus::Ok;
}
SpatialAoi::CellKey SpatialAoi::cellFor(const AoiEntity& entity) const noexcept {
    return {entity.key.worldId, entity.key.instanceId,
        floorCell(entity.x, cellSize_), floorCell(entity.y, cellSize_)};
}
void SpatialAoi::removeActive(const EntityKey& key) {
    auto it = entities_.find(key);
    if (it == entities_.end()) return;
    const auto c = cellFor(it->second);
    auto bucket = cells_.find(c);
    if (bucket != cells_.end()) {
        bucket->second.erase(key);
        if (bucket->second.empty()) cells_.erase(bucket);
    }
    entities_.erase(it);
}
AoiStatus SpatialAoi::upsert(const AoiEntity& value) {
    auto status = ownerStatus();
    if (status != AoiStatus::Ok) return status;
    const auto& k = value.key;
    if (k.runtimeId == 0 || k.generation == 0) return AoiStatus::Invalid;
    SlotKey slot{k.worldId, k.instanceId, k.runtimeId};
    auto history = slots_.find(slot);
    if (history != slots_.end()) {
        if (k.generation < history->second.highestGeneration ||
            (k.generation == history->second.highestGeneration && !history->second.active))
            return AoiStatus::StaleGeneration;
        if (k.generation == history->second.highestGeneration) {
            const auto old = entities_.find(k);
            if (old == entities_.end()) return AoiStatus::StaleGeneration;
            if (value.revision < old->second.revision ||
                (value.revision == old->second.revision &&
                (value.x != old->second.x || value.y != old->second.y ||
                 value.visible != old->second.visible)))
                return AoiStatus::StaleRevision;
            if (value.revision == old->second.revision) return AoiStatus::Ok;
            const auto prevCell = cellFor(old->second);
            const auto newCell = cellFor(value);
            if (!(prevCell == newCell)) {
                auto previous = cells_.find(prevCell);
                if (previous != cells_.end()) {
                    previous->second.erase(k);
                    if (previous->second.empty()) cells_.erase(previous);
                }
                cells_[newCell].insert(k);
            }
            old->second = value;
            return AoiStatus::Ok;
        }
        if (history->second.active) {
            EntityKey oldKey{k.runtimeId, history->second.highestGeneration,
                k.worldId, k.instanceId};
            removeActive(oldKey);
        }
        history->second = {k.generation, true};
    } else {
        slots_.emplace(slot, SlotInfo{k.generation, true});
    }
    entities_[k] = value;
    cells_[cellFor(value)].insert(k);
    return AoiStatus::Ok;
}
AoiStatus SpatialAoi::erase(const EntityKey& k) {
    auto status = ownerStatus();
    if (status != AoiStatus::Ok) return status;
    auto item = entities_.find(k);
    if (item == entities_.end()) return AoiStatus::NotFound;
    removeActive(k);
    SlotKey slot{k.worldId, k.instanceId, k.runtimeId};
    auto it = slots_.find(slot);
    if (it != slots_.end()) it->second.active = false;
    return AoiStatus::Ok;
}
bool SpatialAoi::inRadius(const AoiEntity& entity,
    std::int32_t x, std::int32_t y, std::uint32_t radius) noexcept {
    const std::int64_t dx = static_cast<std::int64_t>(entity.x) - x;
    const std::int64_t dy = static_cast<std::int64_t>(entity.y) - y;
    const std::int64_t r = radius;
    if (dx > r || dx < -r || dy > r || dy < -r) return false;
    return dx * dx + dy * dy <= r * r;
}

AoiPlan SpatialAoi::plan(AoiObserverView& observer,
    std::int32_t centerX, std::int32_t centerY, std::uint32_t radius,
    std::uint64_t sceneEpoch, std::size_t maxEvents) {
    AoiPlan result;
    result.status = ownerStatus();
    if (result.status != AoiStatus::Ok) return result;
    if (observer.viewer_.runtimeId == 0 || observer.viewer_.generation == 0 ||
        sceneEpoch == 0 || radius > maxViewRadius_) {
        result.status = AoiStatus::Invalid;
        return result;
    }
    const auto x1 = floorCell(clampCoordinate(static_cast<std::int64_t>(centerX) - radius), cellSize_);
    const auto x2 = floorCell(clampCoordinate(static_cast<std::int64_t>(centerX) + radius), cellSize_);
    const auto y1 = floorCell(clampCoordinate(static_cast<std::int64_t>(centerY) - radius), cellSize_);
    const auto y2 = floorCell(clampCoordinate(static_cast<std::int64_t>(centerY) + radius), cellSize_);
    const std::uint64_t nx = static_cast<std::uint64_t>(x2 - x1 + 1);
    const std::uint64_t ny = static_cast<std::uint64_t>(y2 - y1 + 1);
    if (nx > maxQueryCells_ || ny > maxQueryCells_ / nx) {
        result.status = AoiStatus::TooManyCells;
        return result;
    }
    result.cellsScanned = static_cast<std::size_t>(nx * ny);

    Current visible;
    for (auto cx = x1; cx <= x2; ++cx) {
        for (auto cy = y1; cy <= y2; ++cy) {
            const auto bucket = cells_.find({observer.viewer_.worldId,
                observer.viewer_.instanceId, cx, cy});
            if (bucket == cells_.end()) continue;
            for (const auto& key : bucket->second) {
                if (key == observer.viewer_) continue;
                const auto e = entities_.find(key);
                if (e == entities_.end() || !e->second.visible ||
                    !inRadius(e->second, centerX, centerY, radius))
                    continue;
                visible.emplace(key, e->second);
                if (visible.size() > maxVisible_) {
                    result.status = AoiStatus::TooManyVisible;
                    return result;
                }
            }
        }
    }
    result.totalVisible = visible.size();
    const bool reset = !observer.initialized_ || observer.epoch_ != sceneEpoch;
    if (reset) result.events.push_back({
        AoiDelta::Kind::Reset, {}, 0, 0, 0, sceneEpoch});
    if (!reset) {
        for (const auto& kv : observer.known_) {
            if (visible.find(kv.first) == visible.end()) {
                result.events.push_back({AoiDelta::Kind::Remove, kv.first,
                    0, 0, kv.second, sceneEpoch});
            }
        }
    }
    for (const auto& kv : visible) {
        const auto old = reset ? observer.known_.end() : observer.known_.find(kv.first);
        if (old == observer.known_.end() || reset) {
            result.events.push_back({AoiDelta::Kind::Create, kv.first,
                kv.second.x, kv.second.y, kv.second.revision, sceneEpoch});
        } else if (old->second != kv.second.revision) {
            result.events.push_back({AoiDelta::Kind::Update, kv.first,
                kv.second.x, kv.second.y, kv.second.revision, sceneEpoch});
        }
    }
    if (result.events.size() > maxEvents) {
        result.status = AoiStatus::EventBudgetExceeded;
        result.events.clear();
        return result;
    }
    sortEvents(result.events);
    observer.known_.clear();
    observer.known_.reserve(visible.size());
    for (const auto& kv : visible) observer.known_.emplace(kv.first, kv.second.revision);
    observer.epoch_ = sceneEpoch;
    observer.initialized_ = true;
    return result;
}
}} // namespace mmo::portable

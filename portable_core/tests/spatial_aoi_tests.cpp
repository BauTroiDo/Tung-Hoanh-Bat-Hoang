#include "mmo/SpatialAoi.h"
#include <atomic>
#include <cstdint>
#include <iostream>
#include <limits>
#include <random>
#include <stdexcept>
#include <thread>
#include <vector>
#define CHECK(expr) do { if (!(expr)) { \
    std::cerr << "FAILED line " << __LINE__ << ": " #expr << "\n"; return 1; \
} } while(false)

namespace {
using namespace mmo::portable;
EntityKey key(std::uint64_t id, std::uint32_t generation = 1,
    std::uint32_t world = 100, std::uint32_t instance = 7) {
    return {id, generation, world, instance};
}
AoiEntity ent(EntityKey k, std::int32_t x, std::int32_t y,
    std::uint64_t rev = 1, bool visible = true) {
    return {k, x, y, rev, visible};
}
bool has(const AoiPlan& p, AoiDelta::Kind k, EntityKey entity) {
    for (const auto& event : p.events)
        if (event.kind == k && event.key == entity) return true;
    return false;
}
}

int main() {
    using namespace mmo::portable;
    bool threw = false;
    try { SpatialAoi bad(0, 100, 100, 100); }
    catch (const std::invalid_argument&) { threw = true; }
    CHECK(threw);
    threw = false;
    try { SpatialAoi unsafe(64, std::numeric_limits<std::uint32_t>::max(), 20, 20); }
    catch (const std::invalid_argument&) { threw = true; }
    CHECK(threw);

    SpatialAoi aoi(100, 500, 100, 20);
    AoiObserverView observer(key(1));
    CHECK(aoi.upsert(ent(key(2), 10, 0)) == AoiStatus::Unbound);
    CHECK(aoi.plan(observer, 0, 0, 100, 1, 20).status == AoiStatus::Unbound);
    CHECK(aoi.bindOwnerToCurrentThread());
    CHECK(aoi.upsert(ent(key(0), 0, 0)) == AoiStatus::Invalid);
    CHECK(aoi.upsert(ent(key(2), 10, 0)) == AoiStatus::Ok);
    CHECK(aoi.upsert(ent(key(3), 100, 0)) == AoiStatus::Ok);
    CHECK(aoi.upsert(ent(key(4), 101, 0)) == AoiStatus::Ok);
    CHECK(aoi.upsert(ent(key(5), 60, 80)) == AoiStatus::Ok);
    CHECK(aoi.upsert(ent(key(6), -100, 0)) == AoiStatus::Ok);
    CHECK(aoi.upsert(ent(key(7), 0, -101)) == AoiStatus::Ok);
    CHECK(aoi.upsert(ent(key(8), 0, 0, 1, false)) == AoiStatus::Ok);
    CHECK(aoi.upsert(ent(key(9,1,100,8), 0, 0)) == AoiStatus::Ok);
    CHECK(aoi.upsert(ent(key(10,1,101,7), 0, 0)) == AoiStatus::Ok);
    CHECK(aoi.upsert(ent(key(1), 0, 0)) == AoiStatus::Ok);
    CHECK(aoi.entityCount() == 10);

    // Initial synchronization resets client scene then creates 4 exact-radius entities,
    // excluding self/invisible, another realm/instance and outside distance.
    auto first = aoi.plan(observer, 0, 0, 100, 17, 20);
    CHECK(first.status == AoiStatus::Ok);
    CHECK(first.events.size() == 5);
    CHECK(first.events[0].kind == AoiDelta::Kind::Reset);
    CHECK(first.events[0].sceneEpoch == 17);
    CHECK(first.totalVisible == 4 && observer.knownCount() == 4);
    CHECK(has(first, AoiDelta::Kind::Create, key(2)));
    CHECK(has(first, AoiDelta::Kind::Create, key(3)));
    CHECK(has(first, AoiDelta::Kind::Create, key(5)));
    CHECK(has(first, AoiDelta::Kind::Create, key(6)));
    CHECK(!has(first, AoiDelta::Kind::Create, key(4)));
    CHECK(!has(first, AoiDelta::Kind::Create, key(8)));
    CHECK(!has(first, AoiDelta::Kind::Create, key(9,1,100,8)));
    CHECK(!has(first, AoiDelta::Kind::Create, key(1)));

    // No changes -> no duplicate creates; revision bump -> only Update.
    auto noChange = aoi.plan(observer, 0, 0, 100, 17, 20);
    CHECK(noChange.status == AoiStatus::Ok && noChange.events.empty());
    CHECK(aoi.upsert(ent(key(2), 10, 0, 1)) == AoiStatus::Ok);
    CHECK(aoi.upsert(ent(key(2), 10, 1, 1)) == AoiStatus::StaleRevision);
    CHECK(aoi.upsert(ent(key(2), 10, 1, 0)) == AoiStatus::StaleRevision);
    CHECK(aoi.upsert(ent(key(2), 10, 1, 2)) == AoiStatus::Ok);
    auto update = aoi.plan(observer, 0, 0, 100, 17, 20);
    CHECK(update.events.size() == 1);
    CHECK(update.events[0].kind == AoiDelta::Kind::Update &&
          update.events[0].key == key(2) && update.events[0].revision == 2);

    // Move across a cell and past observer radius; next observer move makes it reappear.
    CHECK(aoi.upsert(ent(key(2), 300, 0, 3)) == AoiStatus::Ok);
    auto gone = aoi.plan(observer, 0, 0, 100, 17, 20);
    CHECK(gone.events.size() == 1 && has(gone, AoiDelta::Kind::Remove, key(2)));
    auto movedView = aoi.plan(observer, 300, 0, 100, 17, 20);
    CHECK(movedView.status == AoiStatus::Ok);
    CHECK(has(movedView, AoiDelta::Kind::Create, key(2)));
    CHECK(has(movedView, AoiDelta::Kind::Remove, key(3)));

    // Different scene epoch always begins with RESET + full snapshot, atomically.
    const auto previouslyKnown = observer.knownCount();
    const auto failedEpoch = aoi.plan(observer, 300, 0, 100, 18, 1);
    CHECK(failedEpoch.status == AoiStatus::EventBudgetExceeded);
    CHECK(failedEpoch.events.empty() && observer.lastEpoch() == 17 &&
          observer.knownCount() == previouslyKnown);
    const auto reset = aoi.plan(observer, 300, 0, 100, 18, 20);
    CHECK(reset.status == AoiStatus::Ok && reset.events.size() == 2);
    CHECK(reset.events[0].kind == AoiDelta::Kind::Reset &&
        has(reset, AoiDelta::Kind::Create, key(2)));
    CHECK(observer.lastEpoch() == 18);

    // Bounded visibility and cell fanout fail before updating client-known state.
    CHECK(aoi.plan(observer, 0, 0, 501, 18, 10).status == AoiStatus::Invalid);
    CHECK(aoi.plan(observer, 0, 0, 500, 18, 10).status == AoiStatus::TooManyCells);
    CHECK(observer.lastEpoch() == 18);
    SpatialAoi crowded(100, 100, 64, 2);
    CHECK(crowded.bindOwnerToCurrentThread());
    AoiObserverView crowdedView(key(100));
    for (int i = 0; i < 3; ++i)
        CHECK(crowded.upsert(ent(key(200+i), i, 0)) == AoiStatus::Ok);
    const auto full = crowded.plan(crowdedView, 0, 0, 10, 1, 99);
    CHECK(full.status == AoiStatus::TooManyVisible &&
          !crowdedView.initialized() && crowdedView.knownCount() == 0);

    // RuntimeId reuse MUST bump generation and cannot resurrect stale packets.
    AoiObserverView second(key(1));
    const auto initial = aoi.plan(second, 0, 0, 100, 9, 20);
    CHECK(initial.status == AoiStatus::Ok);
    CHECK(aoi.upsert(ent(key(3,2), 1, 1, 1)) == AoiStatus::Ok);
    CHECK(aoi.entityCount() == 10); // replacement, never two active generations
    CHECK(aoi.upsert(ent(key(3,1), 2, 2, 999)) == AoiStatus::StaleGeneration);
    auto replaced = aoi.plan(second, 0, 0, 100, 9, 20);
    CHECK(has(replaced, AoiDelta::Kind::Remove, key(3,1)));
    CHECK(has(replaced, AoiDelta::Kind::Create, key(3,2)));
    CHECK(aoi.erase(key(3,1)) == AoiStatus::NotFound);
    CHECK(aoi.erase(key(3,2)) == AoiStatus::Ok);
    CHECK(aoi.upsert(ent(key(3,2), 1, 1, 100)) == AoiStatus::StaleGeneration);
    CHECK(aoi.upsert(ent(key(3,3), 1, 1, 1)) == AoiStatus::Ok);
    CHECK(aoi.plan(second, 0, 0, 100, 9, 20).status == AoiStatus::Ok);
    CHECK(aoi.occupiedCellCount() > 0);

    // Bound thread ownership blocks remote queries/mutations, without changing state.
    std::atomic<bool> rejected{false};
    std::thread other([&] {
        rejected = aoi.upsert(ent(key(99), 1, 1)) == AoiStatus::WrongThread &&
            aoi.plan(second, 0, 0, 10, 9, 20).status == AoiStatus::WrongThread &&
            !aoi.bindOwnerToCurrentThread();
    });
    other.join();
    CHECK(rejected && aoi.entityCount() == 10);

    // Negative cell floor division, overflow near INT32 endpoints and stable sort.
    SpatialAoi edges(64, 200, 256, 20);
    CHECK(edges.bindOwnerToCurrentThread());
    const auto min = std::numeric_limits<std::int32_t>::min();
    const auto max = std::numeric_limits<std::int32_t>::max();
    CHECK(edges.upsert(ent(key(2), min, min)) == AoiStatus::Ok);
    CHECK(edges.upsert(ent(key(3), min + 1, min + 1)) == AoiStatus::Ok);
    CHECK(edges.upsert(ent(key(4), max, max)) == AoiStatus::Ok);
    AoiObserverView atMin(key(100));
    auto minResult = edges.plan(atMin, min, min, 3, 1, 8);
    CHECK(minResult.status == AoiStatus::Ok &&
          has(minResult, AoiDelta::Kind::Create, key(2)) &&
          has(minResult, AoiDelta::Kind::Create, key(3)) &&
          !has(minResult, AoiDelta::Kind::Create, key(4)));
    CHECK(minResult.events[1].key == key(2) && minResult.events[2].key == key(3));
    AoiObserverView atMax(key(101));
    auto maxResult = edges.plan(atMax, max, max, 2, 1, 8);
    CHECK(maxResult.status == AoiStatus::Ok &&
          has(maxResult, AoiDelta::Kind::Create, key(4)));

    // Repeated deterministic randomized queries: same snapshot -> same event sequence.
    SpatialAoi randomAoi(128, 512, 256, 500);
    CHECK(randomAoi.bindOwnerToCurrentThread());
    std::mt19937 rng(0xA01BEEF);
    for (int i = 0; i < 400; ++i) {
        const auto x = static_cast<std::int32_t>(rng()%1024)-512;
        const auto y = static_cast<std::int32_t>(rng()%1024)-512;
        CHECK(randomAoi.upsert(ent(key(1000+i), x, y)) == AoiStatus::Ok);
    }
    AoiObserverView viewA(key(900)), viewB(key(900));
    for (int step = 0; step < 100; ++step) {
        const auto x = static_cast<std::int32_t>(rng()%800)-400;
        const auto y = static_cast<std::int32_t>(rng()%800)-400;
        const auto p1 = randomAoi.plan(viewA, x, y, 256, 1, 500);
        const auto p2 = randomAoi.plan(viewB, x, y, 256, 1, 500);
        CHECK(p1.status == AoiStatus::Ok && p2.status == AoiStatus::Ok);
        CHECK(p1.events.size() == p2.events.size());
        for (std::size_t j = 0; j < p1.events.size(); ++j) {
            const auto& a=p1.events[j]; const auto& b=p2.events[j];
            CHECK(a.kind == b.kind && a.key == b.key && a.revision == b.revision);
        }
    }
    std::cout << "PASS: isolated AOI realms, stable ID generations, exact radius,"
        << " negative cells, epoch reset, atomic budget, deterministic deltas (100 steps)\n";
    return 0;
}

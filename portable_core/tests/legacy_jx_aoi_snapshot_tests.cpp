#include "mmo/LegacyJxAoiSnapshot.h"
#include <cstdlib>
#include <cstdint>
#include <limits>
#include <iostream>
using namespace mmo::portable;
#define CHECK(expr) do { if (!(expr)) { std::cerr << "CHECK FAILED: " << #expr << " at " << __LINE__ << "\n"; return 1; } } while (false)
int main() {
    LegacyJxNpcSnapshot s;
    s.npcArrayIndex=100; s.npcDwId=87210; s.spawnGeneration=8;
    s.subWorldArrayIndex=7; s.getMpsMapIndex=7; s.mapId=53;
    s.regionIndex=3; s.totalRegions=200;
    s.mpsX=2950; s.mpsY=360; s.revision=12;
    AoiEntity out;
    auto ok=[&]{return convertLegacyJxNpcSnapshot(s,10000,1024,out);};
    int cases=0;
    CHECK(ok()==LegacyJxSnapshotStatus::Ok);++cases;
    CHECK(out.key.worldId==53 && out.key.worldId!=7 && out.key.runtimeId==87210 && out.key.generation==8);++cases;
    CHECK(out.x==2950 && out.y==360 && out.revision==12 && out.visible);++cases;
    auto original=out;
    s.getMpsMapIndex=53; CHECK(ok()==LegacyJxSnapshotStatus::MapIndexMismatch); ++cases;
    CHECK(out.key.worldId==original.key.worldId && out.revision==original.revision);++cases;
    s.getMpsMapIndex=7; s.mapId=-1; CHECK(ok()==LegacyJxSnapshotStatus::InvalidMapId);++cases;
    s.mapId=53; s.npcArrayIndex=0;CHECK(ok()==LegacyJxSnapshotStatus::InvalidNpcArraySlot);++cases;
    s.npcArrayIndex=10000;CHECK(ok()==LegacyJxSnapshotStatus::InvalidNpcArraySlot);++cases;
    s.npcArrayIndex=100;s.npcDwId=0;CHECK(ok()==LegacyJxSnapshotStatus::InvalidNpcId);++cases;
    s.npcDwId=87210;s.spawnGeneration=0;CHECK(ok()==LegacyJxSnapshotStatus::MissingSpawnGeneration);++cases;
    s.spawnGeneration=8;s.subWorldArrayIndex=1024;CHECK(ok()==LegacyJxSnapshotStatus::InvalidSubWorldIndex);++cases;
    s.subWorldArrayIndex=7;s.regionIndex=-1;CHECK(ok()==LegacyJxSnapshotStatus::InvalidRegion);++cases;
    s.regionIndex=3;s.mpsX=static_cast<std::int64_t>(std::numeric_limits<std::int32_t>::max())+1;
    CHECK(ok()==LegacyJxSnapshotStatus::PositionOutOfBounds);++cases;
    s.mpsX=-2950;s.mpsY=-360;s.revision=0;CHECK(ok()==LegacyJxSnapshotStatus::MissingRevision);++cases;
    s.revision=13;s.instanceId=22;s.visible=false;
    CHECK(ok()==LegacyJxSnapshotStatus::Ok);++cases;
    CHECK(out.x==-2950 && out.y==-360 && out.key.instanceId==22 && !out.visible);++cases;
    std::cout << "LEGACY_JX_SNAPSHOT_TEST_PASS " << cases << "/" << cases << "\n";
}

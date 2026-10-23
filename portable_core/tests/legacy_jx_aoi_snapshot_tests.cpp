#include "mmo/LegacyJxAoiSnapshot.h"
#include <cassert>
#include <cstdint>
#include <limits>
#include <iostream>
using namespace mmo::portable;
int main() {
    LegacyJxNpcSnapshot s;
    s.npcArrayIndex=100; s.npcDwId=87210; s.spawnGeneration=8;
    s.subWorldArrayIndex=7; s.getMpsMapIndex=7; s.mapId=53;
    s.regionIndex=3; s.totalRegions=200;
    s.mpsX=2950; s.mpsY=360; s.revision=12;
    AoiEntity out;
    auto ok=[&]{return convertLegacyJxNpcSnapshot(s,10000,1024,out);};
    int cases=0;
    assert(ok()==LegacyJxSnapshotStatus::Ok);++cases;
    assert(out.key.worldId==53 && out.key.worldId!=7 && out.key.runtimeId==87210 && out.key.generation==8);++cases;
    assert(out.x==2950 && out.y==360 && out.revision==12 && out.visible);++cases;
    auto original=out;
    s.getMpsMapIndex=53; assert(ok()==LegacyJxSnapshotStatus::MapIndexMismatch); ++cases;
    assert(out.key.worldId==original.key.worldId && out.revision==original.revision);++cases;
    s.getMpsMapIndex=7; s.mapId=-1; assert(ok()==LegacyJxSnapshotStatus::InvalidMapId);++cases;
    s.mapId=53; s.npcArrayIndex=0;assert(ok()==LegacyJxSnapshotStatus::InvalidNpcArraySlot);++cases;
    s.npcArrayIndex=10000;assert(ok()==LegacyJxSnapshotStatus::InvalidNpcArraySlot);++cases;
    s.npcArrayIndex=100;s.npcDwId=0;assert(ok()==LegacyJxSnapshotStatus::InvalidNpcId);++cases;
    s.npcDwId=87210;s.spawnGeneration=0;assert(ok()==LegacyJxSnapshotStatus::MissingSpawnGeneration);++cases;
    s.spawnGeneration=8;s.subWorldArrayIndex=1024;assert(ok()==LegacyJxSnapshotStatus::InvalidSubWorldIndex);++cases;
    s.subWorldArrayIndex=7;s.regionIndex=-1;assert(ok()==LegacyJxSnapshotStatus::InvalidRegion);++cases;
    s.regionIndex=3;s.mpsX=static_cast<std::int64_t>(std::numeric_limits<std::int32_t>::max())+1;
    assert(ok()==LegacyJxSnapshotStatus::PositionOutOfBounds);++cases;
    s.mpsX=-2950;s.mpsY=-360;s.revision=0;assert(ok()==LegacyJxSnapshotStatus::MissingRevision);++cases;
    s.revision=13;s.instanceId=22;s.visible=false;
    assert(ok()==LegacyJxSnapshotStatus::Ok);++cases;
    assert(out.x==-2950 && out.y==-360 && out.key.instanceId==22 && !out.visible);++cases;
    std::cout << "LEGACY_JX_SNAPSHOT_TEST_PASS " << cases << "/" << cases << "\n";
}

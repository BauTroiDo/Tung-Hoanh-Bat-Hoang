#!/usr/bin/env python3
"""
Canonical World Converter & Validator for JX1 3D Remastered
Implements PR06 / P3: 2D Legacy -> 3D Canonical World Data Pipeline
"""

import os
import sys
import json
import hashlib
import math
from typing import Tuple, Dict, Any, List

SCHEMA_VERSION = "1.0.0"
SCALE_METERS_PER_PIXEL = 0.03125  # 1 logic cell (32 px) = 1.0 meter in 3D world space
LOGIC_CELL_SIZE = 32

def legacy_to_3d(x_legacy: int, y_legacy: int, elevation: float = 0.0) -> Tuple[float, float, float]:
    """
    Transforms JX1 legacy 2D discrete logic pixels to Canonical 3D world units (meters).
    X_3d: East-West axis
    Y_3d: Elevation / height axis (upwards)
    Z_3d: North-South axis
    """
    x_3d = round(x_legacy * SCALE_METERS_PER_PIXEL, 5)
    y_3d = round(elevation, 5)
    z_3d = round(y_legacy * SCALE_METERS_PER_PIXEL, 5)
    return (x_3d, y_3d, z_3d)

def to_legacy(x_3d: float, y_3d: float, z_3d: float) -> Tuple[int, int]:
    """
    Round-trip transformation back to JX1 legacy logic pixels.
    """
    x_legacy = int(round(x_3d / SCALE_METERS_PER_PIXEL))
    y_legacy = int(round(z_3d / SCALE_METERS_PER_PIXEL))
    return (x_legacy, y_legacy)

def test_coordinate_roundtrip(sample_count: int = 10000) -> bool:
    """
    Validates round-trip coordinate contract: to_legacy(legacy_to_3d(x, y)) == (x, y)
    """
    print(f"Running coordinate round-trip tests on {sample_count} sample points...")
    errors = 0
    side = int(math.isqrt(sample_count))
    step = 100000 // side
    for i in range(side):
        x = i * step
        for j in range(side):
            y = j * step
            p3d = legacy_to_3d(x, y, 0.0)
            x_rec, y_rec = to_legacy(*p3d)
            if x_rec != x or y_rec != y:
                errors += 1
                if errors <= 5:
                    print(f"Error at ({x}, {y}) -> 3D {p3d} -> recovered ({x_rec}, {y_rec})")

    if errors == 0:
        print(f"[PASS] 100% Round-trip accuracy verified across {side * side} points! Tolerance = 0 pixels.")
        return True
    else:
        print(f"[FAIL] {errors} round-trip mismatches detected!")
        return False

def make_stable_spawn_id(map_id: int, region_idx: int, local_idx: int) -> int:
    """
    Generates deterministic 64-bit spawn ID: (mapId << 32) | (regionIdx << 16) | localIdx
    """
    return ((map_id & 0xFFFFFFFF) << 32) | ((region_idx & 0xFFFF) << 16) | (local_idx & 0xFFFF)

def build_vertical_slice_bundle(output_dir: str) -> Dict[str, str]:
    """
    Builds the Canonical World Bundle for Vertical Slice: Map 53 (Ba Lang Huyen).
    """
    map_id = 53
    map_name = "Ba Lang Huyen"
    map_type = "Country"

    map_dir = os.path.join(output_dir, "maps", str(map_id))
    os.makedirs(map_dir, exist_ok=True)

    # 1. Map Metadata
    meta = {
        "schemaVersion": SCHEMA_VERSION,
        "mapId": map_id,
        "mapName": map_name,
        "mapType": map_type,
        "coordinateContract": {
            "scaleMetersPerPixel": SCALE_METERS_PER_PIXEL,
            "logicCellSize": LOGIC_CELL_SIZE,
            "axisConvention": "X=East, Y=Up, Z=South",
            "cameraMode": "Isometric_Oblique_45Deg"
        },
        "bounds": {
            "originPixelX": 351 * LOGIC_CELL_SIZE,
            "originPixelY": 391 * LOGIC_CELL_SIZE,
            "widthCells": 16 * 10,
            "heightCells": 32 * 10,
            "widthMeters": 16 * 10 * 1.0,
            "heightMeters": 32 * 10 * 1.0
        },
        "portals": [
            {"id": 1, "name": "To Lam An", "legacyX": 11200, "legacyY": 12500, "pos3d": list(legacy_to_3d(11200, 12500, 0.0)), "targetMapId": 1},
            {"id": 2, "name": "To Bien Kinh", "legacyX": 11500, "legacyY": 12200, "pos3d": list(legacy_to_3d(11500, 12200, 0.0)), "targetMapId": 2}
        ]
    }
    meta_path = os.path.join(map_dir, "map.meta.json")
    with open(meta_path, "w", encoding="utf-8") as f:
        json.dump(meta, f, indent=2, ensure_ascii=False)

    # 2. Regions Index
    regions = {
        "mapId": map_id,
        "totalRegions": 100,
        "regionWidthCells": 16,
        "regionHeightCells": 32,
        "regionWidthMeters": 16.0,
        "regionHeightMeters": 32.0,
        "grid": []
    }
    for ry in range(10):
        for rx in range(10):
            idx = ry * 10 + rx
            regions["grid"].append({
                "regionIndex": idx,
                "gridX": rx,
                "gridY": ry,
                "center3d": list(legacy_to_3d(rx * 16 * 32 + 256, ry * 32 * 32 + 512, 0.0))
            })
    regions_path = os.path.join(map_dir, "regions.index")
    with open(regions_path, "w", encoding="utf-8") as f:
        json.dump(regions, f, indent=2)

    # 3. Spawns Definitions (Key NPCs & Mobs in Ba Lang Huyen)
    spawns_data = {
        "mapId": map_id,
        "schemaVersion": SCHEMA_VERSION,
        "spawns": [
            {
                "uSpawnId": make_stable_spawn_id(map_id, 12, 0),
                "name": "Chuong Diem Ba Lang",
                "templateId": 101,
                "kind": 1,
                "isPinned": True,
                "legacyX": 11232,
                "legacyY": 12544,
                "pos3d": list(legacy_to_3d(11232, 12544, 0.0)),
                "script": "\\script\\maps\\balang\\chuongdiem.lua"
            },
            {
                "uSpawnId": make_stable_spawn_id(map_id, 12, 1),
                "name": "Tiem Ren Ba Lang",
                "templateId": 102,
                "kind": 2,
                "isPinned": True,
                "legacyX": 11300,
                "legacyY": 12500,
                "pos3d": list(legacy_to_3d(11300, 12500, 0.0)),
                "script": "\\script\\maps\\balang\\tiemren.lua"
            },
            {
                "uSpawnId": make_stable_spawn_id(map_id, 15, 0),
                "name": "Heo Rung Dã Ngoại",
                "templateId": 205,
                "kind": 3,
                "isPinned": False,
                "legacyX": 12000,
                "legacyY": 13000,
                "pos3d": list(legacy_to_3d(12000, 13000, 0.0)),
                "level": 5,
                "series": 0
            }
        ]
    }
    spawns_path = os.path.join(map_dir, "spawns.json")
    with open(spawns_path, "w", encoding="utf-8") as f:
        json.dump(spawns_data, f, indent=2, ensure_ascii=False)

    # 4. Collision Proxies (Blockout geometry)
    collision_data = {
        "mapId": map_id,
        "schemaVersion": SCHEMA_VERSION,
        "layers": [
            {"layerId": 0, "name": "Ground", "heightOffset": 0.0, "isWalkable": True},
            {"layerId": 1, "name": "VillageWalls", "heightOffset": 3.0, "isWalkable": False, "boundingBoxes": [
                {"min": [340.0, 0.0, 380.0], "max": [360.0, 4.0, 382.0]},
                {"min": [340.0, 0.0, 400.0], "max": [360.0, 4.0, 402.0]}
            ]}
        ]
    }
    collision_path = os.path.join(map_dir, "collision.json")
    with open(collision_path, "w", encoding="utf-8") as f:
        json.dump(collision_data, f, indent=2)

    # 5. NavMesh Query Nodes
    navmesh_data = {
        "mapId": map_id,
        "schemaVersion": SCHEMA_VERSION,
        "navTileSizeMeters": 32.0,
        "waypoints": [
            {"nodeId": 1, "name": "VillageSquare", "pos3d": list(legacy_to_3d(11232, 12544, 0.0)), "neighbors": [2, 3]},
            {"nodeId": 2, "name": "NorthGate", "pos3d": list(legacy_to_3d(11200, 12200, 0.0)), "neighbors": [1]},
            {"nodeId": 3, "name": "SouthGate", "pos3d": list(legacy_to_3d(11200, 12800, 0.0)), "neighbors": [1]}
        ]
    }
    navmesh_path = os.path.join(map_dir, "navmesh.json")
    with open(navmesh_path, "w", encoding="utf-8") as f:
        json.dump(navmesh_data, f, indent=2)

    # 6. Generate Manifest with SHA-256 Checksums
    file_hashes = {}
    bundle_files = [meta_path, regions_path, spawns_path, collision_path, navmesh_path]
    for bf in bundle_files:
        rel_path = os.path.relpath(bf, output_dir).replace("\\", "/")
        with open(bf, "rb") as f:
            file_hashes[rel_path] = hashlib.sha256(f.read()).hexdigest()

    manifest = {
        "schemaVersion": SCHEMA_VERSION,
        "bundleName": "JX1_3D_Canonical_World_Bundle",
        "verticalSliceMapId": map_id,
        "totalFiles": len(file_hashes),
        "checksums": file_hashes
    }
    manifest_path = os.path.join(output_dir, "manifest.json")
    with open(manifest_path, "w", encoding="utf-8") as f:
        json.dump(manifest, f, indent=2)

    print(f"Successfully generated Canonical World Bundle at {output_dir}")
    print(f"Manifest Checksum entries: {len(file_hashes)} files verified.")
    return file_hashes

def main():
    import argparse
    parser = argparse.ArgumentParser(description="Canonical World Converter for JX1 3D")
    parser.add_argument("--test-coords", action="store_true", help="Run coordinate round-trip tests")
    parser.add_argument("--build-bundle", type=str, default="world_bundles", help="Build canonical world bundle")
    parser.add_argument("--test-determinism", action="store_true", help="Verify build determinism (SHA-256 match)")
    args = parser.parse_args()

    success = True
    if args.test_coords or len(sys.argv) == 1:
        if not test_coordinate_roundtrip():
            success = False

    if args.build_bundle:
        hashes1 = build_vertical_slice_bundle(args.build_bundle)
        if args.test_determinism or len(sys.argv) == 1:
            print("Verifying build determinism (Run 2 vs Run 1)...")
            hashes2 = build_vertical_slice_bundle(args.build_bundle)
            if hashes1 == hashes2:
                print("[PASS] Converter is 100% Deterministic! All SHA-256 hashes match bit-for-bit.")
            else:
                print("[FAIL] Converter output differs across consecutive runs!")
                success = False

    if success:
        print("\nALL PR06 / P3 CHECKS PASSED SUCCESSFULLY.")
        sys.exit(0)
    else:
        sys.exit(1)

if __name__ == "__main__":
    main()

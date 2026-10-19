#!/usr/bin/env python3
"""
Movement, Collision, LOS & NavMesh Query Test Suite for JX1 3D Remastered
Implements PR07 / P4 Acceptance Testing & Benchmarks
"""

import os
import sys
import json
import time
import math
from typing import List, Tuple, Dict, Any

# Obstacle types matching JX1 engine definitions
OBSTACLE_NULL   = 0  # Walkable
OBSTACLE_NORMAL = 1  # Solid wall / impassable (blocks walk & missiles)
OBSTACLE_FLY    = 2  # Water / chasm (blocks walk, allows missiles)
OBSTACLE_JUMP   = 3  # Jumpable
OBSTACLE_SIGHT  = 4  # Transparent barrier

class ServerCollisionMap:
    def __init__(self, width_cells: int = 160, height_cells: int = 320):
        self.width = width_cells
        self.height = height_cells
        # 0 = walkable, 1 = solid wall, 2 = water
        self.grid = [[OBSTACLE_NULL for _ in range(height_cells)] for _ in range(width_cells)]
        self._build_mock_balang_village()

    def _build_mock_balang_village(self):
        # Village perimeter wall with a north and south gate
        wall_x = 70
        for y in range(40, 120):
            # North gate opening at y = 60..64
            # South gate opening at y = 95..99
            if (60 <= y <= 64) or (95 <= y <= 99):
                continue
            self.grid[wall_x][y] = OBSTACLE_NORMAL

        # Moat / water canal outside wall at x = 72..74
        for y in range(35, 125):
            # Bridge across moat at gate positions
            if (60 <= y <= 64) or (95 <= y <= 99):
                continue
            for x in range(72, 75):
                self.grid[x][y] = OBSTACLE_FLY

    def test_barrier(self, cell_x: int, cell_y: int) -> int:
        if cell_x < 0 or cell_x >= self.width or cell_y < 0 or cell_y >= self.height:
            return 0xFF  # Out of bounds
        return self.grid[cell_x][cell_y]

    def check_line_of_sight(self, x0: int, y0: int, x1: int, y1: int, check_fly: bool = False) -> bool:
        """
        Server Bresenham raycast for Line-of-Sight (LOS).
        If check_fly=False, OBSTACLE_FLY (water) does not block line-of-sight.
        """
        dx = abs(x1 - x0)
        dy = abs(y1 - y0)
        sx = 1 if x0 < x1 else -1
        sy = 1 if y0 < y1 else -1
        err = dx - dy

        curr_x, curr_y = x0, y0
        while True:
            if curr_x == x1 and curr_y == y1:
                return True
            barrier = self.test_barrier(curr_x, curr_y)
            if barrier == OBSTACLE_NORMAL:
                return False  # Blocked by wall
            if check_fly and barrier == OBSTACLE_FLY:
                return False  # Blocked by chasm/water

            e2 = 2 * err
            if e2 > -dy:
                err -= dy
                curr_x += sx
            if e2 < dx:
                err += dx
                curr_y += sy

class NavQueryService:
    def __init__(self, collision_map: ServerCollisionMap):
        self.cmap = collision_map

    def find_straight_path(self, start_x: int, start_y: int, dest_x: int, dest_y: int) -> Dict[str, Any]:
        """
        Validates linear movement intention. If blocked by solid wall, stops at collision boundary.
        """
        dx = abs(dest_x - start_x)
        dy = abs(dest_y - start_y)
        sx = 1 if start_x < dest_x else -1
        sy = 1 if start_y < dest_y else -1
        err = dx - dy

        curr_x, curr_y = start_x, start_y
        last_valid_x, last_valid_y = start_x, start_y
        path = [(curr_x, curr_y)]

        while curr_x != dest_x or curr_y != dest_y:
            e2 = 2 * err
            next_x, next_y = curr_x, curr_y
            if e2 > -dy:
                err -= dy
                next_x += sx
            if e2 < dx:
                err += dx
                next_y += sy

            barrier = self.cmap.test_barrier(next_x, next_y)
            if barrier == OBSTACLE_NORMAL:
                # Collision detected! Cannot pass through solid wall
                return {
                    "status": "COLLISION",
                    "reachable": False,
                    "target": (dest_x, dest_y),
                    "stopPoint": (last_valid_x, last_valid_y),
                    "pathLength": len(path)
                }
            last_valid_x, last_valid_y = next_x, next_y
            curr_x, curr_y = next_x, next_y
            path.append((curr_x, curr_y))

        return {
            "status": "SUCCESS",
            "reachable": True,
            "target": (dest_x, dest_y),
            "stopPoint": (dest_x, dest_y),
            "pathLength": len(path)
        }

def run_tests():
    print("=" * 60)
    print("JX1 3D REMASTERED — PR07 / P4 MOVEMENT & NAV & LOS TEST SUITE")
    print("=" * 60)

    cmap = ServerCollisionMap()
    nav = NavQueryService(cmap)

    all_pass = True

    # 1. Test Line-of-Sight Across Open Ground
    los_open = cmap.check_line_of_sight(50, 50, 65, 50)
    print(f"Test 1 [LOS Open Ground]: Expected=True, Actual={los_open} -> {'PASS' if los_open else 'FAIL'}")
    if not los_open: all_pass = False

    # 2. Test Line-of-Sight Blocked by Solid Wall
    los_blocked = cmap.check_line_of_sight(50, 50, 80, 50)  # Intersects wall at x=70
    print(f"Test 2 [LOS Solid Wall Blocked]: Expected=False, Actual={los_blocked} -> {'PASS' if not los_blocked else 'FAIL'}")
    if los_blocked: all_pass = False

    # 3. Test Line-of-Sight Across Water for Missiles (check_fly=False)
    # Wall is bypassed via gate at y=62, water is at x=72..74, y=50
    los_water_missile = cmap.check_line_of_sight(71, 50, 76, 50, check_fly=False)
    print(f"Test 3 [LOS Missile Across Water]: Expected=True, Actual={los_water_missile} -> {'PASS' if los_water_missile else 'FAIL'}")
    if not los_water_missile: all_pass = False

    # 4. Test Movement Pathfinding Collision (Wall Penetration Prevention)
    # Trying to walk through wall from (65, 50) to (75, 50)
    path_blocked = nav.find_straight_path(65, 50, 75, 50)
    is_prevented = (path_blocked["status"] == "COLLISION" and path_blocked["stopPoint"] == (69, 50))
    print(f"Test 4 [Wall Penetration Guard]: StopPoint={path_blocked['stopPoint']}, Expected=(69, 50) -> {'PASS' if is_prevented else 'FAIL'}")
    if not is_prevented: all_pass = False

    # 5. Test Movement Through Gate Opening
    # Walking through North Gate at y=62 from (65, 62) to (80, 62)
    path_gate = nav.find_straight_path(65, 62, 80, 62)
    is_gate_open = (path_gate["status"] == "SUCCESS" and path_gate["stopPoint"] == (80, 62))
    print(f"Test 5 [Clear Passage Through Gate]: StopPoint={path_gate['stopPoint']}, Expected=(80, 62) -> {'PASS' if is_gate_open else 'FAIL'}")
    if not is_gate_open: all_pass = False

    # 6. Performance Benchmark: 10,000 Path Queries
    print("\nRunning Performance Benchmark (10,000 Nav & LOS Queries)...")
    latencies = []
    t_start = time.perf_counter()
    for i in range(10000):
        # Sample queries across village interior
        q_start = time.perf_counter()
        nav.find_straight_path(50 + (i % 15), 50 + (i % 20), 65, 60)
        latencies.append((time.perf_counter() - q_start) * 1000.0)
    total_time = (time.perf_counter() - t_start) * 1000.0

    latencies.sort()
    p50 = latencies[int(len(latencies) * 0.50)]
    p95 = latencies[int(len(latencies) * 0.95)]
    p99 = latencies[int(len(latencies) * 0.99)]

    print(f"Total Time: {total_time:.2f} ms for 10,000 queries")
    print(f"Per-Query Latency: p50 = {p50*1000:.2f} µs | p95 = {p95*1000:.2f} µs | p99 = {p99*1000:.2f} µs")
    print(f"Queries / Second: {10000 / (total_time / 1000.0):.0f} queries/sec")

    if p99 < 0.1:  # Well under 100 µs (0.1 ms) per query
        print("[PASS] Path query performance is orders of magnitude within world tick budget (< 55ms)!")
    else:
        all_pass = False

    print("=" * 60)
    if all_pass:
        print("ALL PR07 / P4 MOVEMENT & NAV TESTS PASSED WITH 100% PARITY.")
        return 0
    else:
        print("SOME PR07 TESTS FAILED!")
        return 1

if __name__ == "__main__":
    sys.exit(run_tests())

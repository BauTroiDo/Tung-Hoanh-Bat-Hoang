#!/usr/bin/env python3
"""
Client 3D Isometric Vertical Slice Adapter Harness for JX1 Remastered
Simulates Client 3D Engine (Unity URP) integration with Canonical World Bundle & GameServer Protocol
"""

import os
import sys
import json
import time
import math
from typing import Dict, Any, List, Optional

if sys.stdout.encoding != 'utf-8':
    sys.stdout.reconfigure(encoding='utf-8')

class GameObject3D:
    def __init__(self, entity_id: int, name: str, pos3d: List[float], kind: int, is_player: bool = False):
        self.entity_id = entity_id
        self.name = name
        self.position = list(pos3d)
        self.target_position = list(pos3d)
        self.rotation_yaw = 0.0  # In degrees (0 = South, 90 = East, 180 = North, 270 = West)
        self.kind = kind
        self.is_player = is_player
        self.anim_state = "IDLE"  # IDLE, RUN, ATTACK, HIT, DIE
        self.hp = 1000
        self.max_hp = 1000
        self.is_dead = False

    def look_at(self, target_pos: List[float]):
        dx = target_pos[0] - self.position[0]
        dz = target_pos[2] - self.position[2]
        if abs(dx) > 0.001 or abs(dz) > 0.001:
            angle_rad = math.atan2(dx, dz)
            self.rotation_yaw = round(math.degrees(angle_rad), 2)

    def interpolate_movement(self, dt: float, speed_mps: float = 6.0):
        if self.position == self.target_position:
            self.anim_state = "IDLE"
            return

        dx = self.target_position[0] - self.position[0]
        dz = self.target_position[2] - self.position[2]
        dist = math.hypot(dx, dz)
        step = speed_mps * dt

        if step >= dist:
            self.position = list(self.target_position)
            self.anim_state = "IDLE"
        else:
            self.position[0] += (dx / dist) * step
            self.position[2] += (dz / dist) * step
            self.look_at(self.target_position)
            self.anim_state = "RUN"

class Client3DIsometricScene:
    def __init__(self, bundle_dir: str, map_id: int = 53):
        self.bundle_dir = bundle_dir
        self.map_id = map_id
        self.entities: Dict[int, GameObject3D] = {}
        self.player_id = 10001
        self.camera_fov = 35.0
        self.camera_pitch = 45.0  # Fixed Isometric Oblique 45 degrees
        self.camera_distance = 25.0
        self.load_world_bundle()

    def load_world_bundle(self):
        map_meta_path = os.path.join(self.bundle_dir, "maps", str(self.map_id), "map.meta.json")
        spawns_path = os.path.join(self.bundle_dir, "maps", str(self.map_id), "spawns.json")
        
        with open(map_meta_path, "r", encoding="utf-8") as f:
            self.meta = json.load(f)
        with open(spawns_path, "r", encoding="utf-8") as f:
            self.spawns_data = json.load(f)

        print(f"[Client3D] Loaded Canonical World Scene for Map {self.map_id} ({self.meta['mapName']})")
        print(f"[Client3D] Camera Config: Mode={self.meta['coordinateContract']['cameraMode']}, Pitch={self.camera_pitch}°, Distance={self.camera_distance}m")

        # Instantiate Player in 3D Scene at Village Square
        player_start = [351.0, 0.0, 392.0]
        self.entities[self.player_id] = GameObject3D(self.player_id, "Player_HiepKhach", player_start, kind=0, is_player=True)

        # Instantiate Spawns
        for s in self.spawns_data["spawns"]:
            eid = s["uSpawnId"] & 0xFFFFFFFF  # Map to 32-bit entity ID for client
            self.entities[eid] = GameObject3D(eid, s["name"], s["pos3d"], s["kind"])
            print(f"  -> Bound 3D Entity [{eid}]: '{s['name']}' at {s['pos3d']}")

    def get_player(self) -> GameObject3D:
        return self.entities[self.player_id]

    def handle_click_to_move(self, target_3d: List[float]):
        player = self.get_player()
        player.target_position = list(target_3d)
        player.look_at(target_3d)
        player.anim_state = "RUN"
        print(f"[Input] Click-to-Move Intent to {target_3d} (Heading Yaw: {player.rotation_yaw}°)")

    def interact_with_npc(self, npc_entity_id: int) -> bool:
        player = self.get_player()
        npc = self.entities.get(npc_entity_id)
        if not npc:
            print(f"[Interaction] Entity {npc_entity_id} not found!")
            return False
        
        dist = math.hypot(player.position[0] - npc.position[0], player.position[2] - npc.position[2])
        if dist > 5.0:
            print(f"[Interaction] NPC '{npc.name}' is too far ({dist:.2f}m > 5.0m)! Moving closer...")
            self.handle_click_to_move(npc.position)
            return False

        print(f"[Interaction] Successfully opened Dialogue with '{npc.name}' at distance {dist:.2f}m!")
        print(f"  Dialogue Tree: [1] Xem Hang Hoa  [2] Nhan Nhiem Vu  [3] Tam Biet")
        return True

    def cast_skill_on_target(self, target_entity_id: int, skill_id: int = 101, damage: int = 450):
        player = self.get_player()
        target = self.entities.get(target_entity_id)
        if not target:
            return False

        player.look_at(target.position)
        player.anim_state = "ATTACK"
        print(f"[Combat] Player casts Skill {skill_id} towards '{target.name}' (Distance: {math.hypot(player.position[0]-target.position[0], player.position[2]-target.position[2]):.2f}m)")

        # Server Authoritative Damage Application
        target.anim_state = "HIT"
        target.hp = max(0, target.hp - damage)
        print(f"[Combat] Target '{target.name}' took {damage} damage! Remaining HP: {target.hp}/{target.max_hp}")

        if target.hp == 0:
            target.anim_state = "DIE"
            target.is_dead = True
            print(f"[Combat] Target '{target.name}' was defeated! Playing death animation & scheduling respawn.")
        return True

def run_prototype():
    print("=" * 65)
    print("JX1 3D REMASTERED — PR08 / P5 CLIENT 3D VERTICAL SLICE PROTOTYPE")
    print("=" * 65)

    bundle_dir = "c:\\Server-Game\\SwordOnline\\jxOnline-r78\\world_bundles"
    scene = Client3DIsometricScene(bundle_dir, map_id=53)

    player = scene.get_player()
    print(f"\nInitial Player Position: {player.position}")

    # 1. Simulate Click-to-Move to Blacksmith (Tiem Ren) at [353.125, 0.0, 390.625]
    print("\n--- Phase 1: Click-to-Move & 3D Movement Interpolation ---")
    scene.handle_click_to_move([353.125, 0.0, 390.625])
    
    # Tick 5 simulation frames (dt = 0.1s)
    for frame in range(1, 6):
        player.interpolate_movement(dt=0.1, speed_mps=6.0)
        print(f"  Frame {frame} (t={frame*0.1:.1f}s): Pos={player.position}, AnimState={player.anim_state}")

    # 2. Simulate NPC Dialogue Interaction with Blacksmith
    print("\n--- Phase 2: NPC Dialogue Interaction ---")
    blacksmith_eid = [e for e in scene.entities.values() if "Tiem Ren" in e.name][0].entity_id
    success_dialog = scene.interact_with_npc(blacksmith_eid)
    if not success_dialog:
        print("[FAIL] Dialogue interaction failed!")
        return 1

    # 3. Simulate Combat Engagement with Wild Boar
    print("\n--- Phase 3: Server-Authoritative Combat Execution ---")
    boar_eid = [e for e in scene.entities.values() if "Heo Rung" in e.name][0].entity_id
    # Attack 1
    scene.cast_skill_on_target(boar_eid, skill_id=101, damage=450)
    # Attack 2
    scene.cast_skill_on_target(boar_eid, skill_id=101, damage=600)

    # 4. Verification Check
    boar = scene.entities[boar_eid]
    if boar.is_dead and boar.anim_state == "DIE":
        print("\n[PASS] Combat resolution verified! Monster state transitioned correctly to DIE.")
    else:
        print("\n[FAIL] Monster death verification failed!")
        return 1

    print("=" * 65)
    print("ALL PR08 / P5 CLIENT 3D VERTICAL SLICE CHECKS PASSED.")
    return 0

if __name__ == "__main__":
    sys.exit(run_prototype())

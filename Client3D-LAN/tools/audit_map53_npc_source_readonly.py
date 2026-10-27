"""Read-only audit of static NPC section headers for GS DEV Map 53.

Does not attach to GameServer or change map data. Static NPC definitions are
NOT proof of runtime spawn/visibility; feature flags and GS AOI may defer NPCs.
"""
from __future__ import annotations
from collections import Counter
from pathlib import Path
import argparse
import json
import struct


def inspect(root: Path, center: tuple[int, int] = (98, 102)):
    rows = []
    errors = []
    files = list(root.glob("v_*/*_region_s.dat"))
    for file in sorted(files):
        try:
            ry = int(file.parent.name[2:])
            rx = int(file.name.split("_", 1)[0])
            data = file.read_bytes()
            if len(data) < 52:
                raise ValueError("truncated header")
            n = struct.unpack_from("<I", data, 0)[0]
            if n != 6:
                raise ValueError(f"region section count {n}, expected 6")
            entries = [struct.unpack_from("<II", data, 4 + 8 * j) for j in range(n)]
            payload_start = 4 + n * 8
            if any(payload_start + off + size > len(data) for off, size in entries):
                raise ValueError("out-of-bounds section")
            offset, size = entries[2]  # REGION_NPC_FILE_INDEX
            if size > 0 and size < 4:
                raise ValueError("truncated NPC count")
            count = (struct.unpack_from("<I", data, payload_start + offset)[0]
                     if size >= 4 else 0)
            if count > 1000:
                raise ValueError(f"implausible NPC count {count}")
            rows.append(dict(rx=rx, ry=ry, npc_section_bytes=size,
                             static_npc_definitions=count))
        except (ValueError, OSError, struct.error) as ex:
            errors.append(dict(file=str(file), error=str(ex)))
    populated = [x for x in rows if x["static_npc_definitions"] > 0]
    cx, cy = center
    nearby = [x for x in rows if abs(x["rx"] - cx) <= 2
              and abs(x["ry"] - cy) <= 2 and x["static_npc_definitions"] > 0]
    return dict(map_id=53, source="Map53 GS loose country/balanghuyen region_s.dat",
                files=len(files), parsed=len(rows), errors=errors,
                empty_npc_section_count=sum(x["npc_section_bytes"] == 0 for x in rows),
                regions_with_npc_definitions=len(populated),
                total_static_npc_definitions=sum(x["static_npc_definitions"] for x in rows),
                center_region=[cx, cy], nearby_regions=nearby,
                note="Static definitions do NOT establish GS runtime NPC instances or AOI packets")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("root", type=Path)
    parser.add_argument("--rx", type=int, default=98)
    parser.add_argument("--ry", type=int, default=102)
    args = parser.parse_args()
    report = inspect(args.root, (args.rx, args.ry))
    print(json.dumps(report, indent=2))
    if report["errors"] or report["files"] != report["parsed"]:
        raise SystemExit(2)

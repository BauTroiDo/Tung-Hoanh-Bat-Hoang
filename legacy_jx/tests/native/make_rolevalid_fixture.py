"""Generate a synthetic QA TRoleData BLOB from a 664-byte QA base-info fixture.

Usage: python make_rolevalid_fixture.py post2_A.bin post2_A_rolevalid.bin

FOR QA ONLY. The input is deliberately fake, not a real character archive.
The offsets are from the validated Windows x64 ABI:
TRoleBaseInfo 664, offsetof(TRoleData,dwDataLen)=698,
offsetof(TRoleData,pBuffer)=702.
"""
from pathlib import Path
import sys
import struct

def main() -> None:
    if len(sys.argv) != 3:
        raise SystemExit("Usage: script.py post2_A.bin output.bin")
    source, output = map(Path, sys.argv[1:])
    if output.exists():
        raise SystemExit("Refusing to overwrite existing fixture")
    payload = bytearray(source.read_bytes())
    if len(payload) != 664 or payload[4:8] != b"QA_A":
        raise SystemExit("Unexpected synthetic QA base-info source")
    payload.extend(bytes(704 - len(payload)))
    struct.pack_into("<i", payload, 698, len(payload))
    output.write_bytes(payload)
    print("QA_ROLE_DATA_LENGTH", len(payload), "ROLE_OFFSET", 702)

if __name__ == "__main__":
    main()

# JX273 P0 — Stage 2: revision wire codec and consistent role snapshot (2026-10-25)

**Disposition: native QA PASS; NOT yet a wired save protocol. Issue #27 stays P0 / BLOCKED.**

## Purpose
The legacy c2s_rolekfjhu_saveroleinfo path lacks expected revision.
Goddess still writes it via updateBlobData_n() without comparing the
committed trade fence. Stage 1 CAS protects writes only when explicitly used.

## Source changes

1. gateway/Headers/RoleSaveRevisionWire.h and gameserver/Headers/RoleSaveRevisionWire.h

   Byte-identical header-only 24-byte trailer codec: JXREV001 magic/version,
   little-endian 64-bit revision, bounded role-name digest and integrity hash.
   Decode returns Valid, Absent, Invalid; a missing trailer is NOT revision 0.
   Name digest prevents accidental mismatch, not hostile forgery; real account
   authentication and authenticated server role binding are still required.

2. gateway/Sources/MultiServer/Goddess/RoleSaveRevisionCAS.h

   LoadSnapshot() reads StudentBlob and revision within a consistent SQLite
   snapshot transaction and publishes both together only on success. Fails
   safely for missing/duplicated roles, fenced roles missing version,
   oversized or NULL data, and SQL failures. Stage 1 CAS behavior unchanged.

3. tests/native/role_revision_wire_qa.cpp
4. tests/native/role_save_snapshot_qa.cpp

## Native x64 QA

- Compiled Gateway codec: 29/29 PASS, exit 0 (10,000 revision roundtrips).
- Compiled GameServer codec: 29/29 PASS, exit 0.
- Consistent SQLite role snapshot: 28/28 PASS, exit 0.
- Prior CAS regression on native SQLite: 32/32 PASS, exit 0.
- TOTAL: 118/118 native assertions PASS.
- Fresh isolated Goddess MSVC x64 Rebuild PASS, zero C/LNK errors.
  Existing legacy warnings remain, including size_t to int conversions.
- Evidence and disposable test DBs:
  C:/Server-Game/SwordOnline/maintenance-builds/jx273_save_ack_gate_20261025/
    wire-qa-20261025/
    snapshot-qa-20261025/
    wire-snapshot-goddess-x64/
- No production services started, no binaries replaced.

## P0 still unresolved — no deployment

- Goddess _GetRoleInfo does NOT append revision trailer yet. Bishop
  forwards TRoleData length to GameServer; GS AddCharacter currently
  receives the original blob. Size validation must be added at each hop.
- GS SavePlayerData still sends unversioned c2s_rolekfjhu_saveroleinfo.
  Goddess ClientNode::_SaveRoleInfo still calls the legacy SaveRoleInfo()
  and updateBlobData_n() without expectedRevision. It may still overwrite
  a newer trade snapshot.
- A new protocol must bind role name + actual player session/slot +
  revision when loading, enforce CAS when saving, increment token after
  a trade ACK for both roles, handle delayed ACK and recycled slots,
  and resync on rejection. Never use a blanket save block that prevents
  ordinary progress.
- Real client login/relog, transfer, 2 GS, dropped ACK, real items,
  maximum packet capacity, and same-folder BDB recovery on loaded role
  data are not covered by these codec/snapshot tests.
- Keep Issue #27 OPEN and this GitHub branch/PR DRAFT.

## Isolation

Source worktree:
C:/Server-Game/SwordOnline/maintenance-builds/jx273_role_cas_source_20261025.
Live ServerNew/_bin_v2_ binaries, DB, config unchanged. Temporary QA
project variants disable postbuild copying. No QA DB or executables in Git.

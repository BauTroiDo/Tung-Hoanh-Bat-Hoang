# JX273 / Issue #27 — Stage 5: durable Goddess ordinary-save CAS + nonce ACK (2026-10-25)

**Disposition: server-side SQLite and Rainbow QA PASS. NOT production-ready. Keep Draft PR #29; Issue #27 remains open (P0).**

## What changed

Goddess now recognizes two layouts under legacy large-message save opcode 10:
- **Legacy:** exactly a TRoleData blob, no trailer; calls SaveRoleInfo(). Stage 4 SQLite trade-fence/role-revision guard still blocks post-trade legacy overwrites, preserving older ACK opcode 56 and marker 791030.
- **Versioned:** exactly a TRoleData blob plus the Stage 4 36-byte JXREV001 save trailer (revision, role digest, integrity and nonzero 64-bit nonce). Goddess validates bounded role name, real TRoleData dwDataLen, exact reassembled message length, server-side SQLite mode, trailer version and nonce before any database write. It calls SaveRoleInfoVersioned()->sqlitel_db::updateBlobData_versioned_receipt().

Versioned requests get opcode 56 with marker 791031 and 25 payload bytes:
status int8, expectedRevision uint64 LE, committedRevision uint64 LE (zero on failure), requestNonce uint64 LE. GS must match expected, nonce and session identity before trusting this ACK. This protocol is **not yet consumed by GameServer**.

The new RoleSaveVersionedReceipt.h commits three operations in one SQLite BEGIN IMMEDIATE transaction: revision CAS advance, StudentBlob update, and receipt (role_name, nonce, expected, committed, blob length + 64-bit fingerprint). A matching lost ACK replay returns the saved result without another blob write or revision increase. If a later transaction moved the role to a newer revision, replay returns stale -17 rather than claiming the old token remains current. Same nonce plus a different expected/blob returns -13. Missing role returns -14; mismatched revision returns -17. Transaction failure rolls back the change.

Hash is an integrity/collision detector, **not cryptographic authentication**. Real client/account authentication is handled separately; nonce generation and GS session fencing require their own runtime validation.

## Files

- gateway/Sources/MultiServer/Goddess/RoleSaveVersionedReceipt.h (new).
- gateway/Sources/MultiServer/Goddess/sqlitel_db.h/.cpp: versioned receipt API wrapper.
- gateway/Sources/MultiServer/Goddess/IDBRoleServer.h/.cpp: versioned server-side save entrypoint.
- gateway/Sources/MultiServer/Goddess/ClientNode.cpp: validation, no legacy fallback for versioned payload, new ACK.
- tests/native/role_versioned_save_receipt_qa.cpp: native SQLite receipt/replay/regression.
- tests/native/qa_versioned_save_client.cpp: actual Rainbow save packet/ACK tests.
- tests/native/make_rolevalid_fixture.py: QA-only synthetic role serialization sizing helper.

## Test evidence

All isolated to maintenance-builds/jx273_role_protocol_stage5_20261025.
- Final MSVC Goddess x64 **Rebuild PASS, zero compiler/link errors**.
- Native SQLite C++ x64 **34/34 PASS**, including nonce replay, changed blob collision, stale expected revision, trade between saves, fail-closed previously-fenced role, close/reopen persistent receipts, replay of earlier receipt after newer version returns -17.
- Actual Rainbow to new Goddess 127.0.0.1:15001 (original source never bound to LAN):
  1. after trade, save QA_A revision 2 -> 3, nonce 500001: ACK 1, next 3.
  2. exact same request replay: ACK 1, next 3, no second receipt.
  3. same nonce with changed blob: NACK -13.
  4. different nonce with stale expected revision: NACK -17.
  5. valid next progress rev 3 -> 4, nonce 500003: ACK 1, next 4.
- Restart Goddess **in same disposable runtime directory**:
  - replay last committed request => ACK 1, next 4;
  - replay older committed request at revision 2 => NACK -17;
  - both assertions PASS.
- SQLite integrity_check ok; QA_A version 4, synthetic gold 72, role B version 2 and synthetic gold 230; exactly 2 role_save_receipts for QA_A (nonce 500001 and 500003); QA_A blob length 704, ekey 5743; no unrelated role changed.

### Test fixture caveat

The earlier synthetic 664-byte trade blobs cover TRoleBaseInfo only. Actual Windows x64 TRoleData has minimum header 702 bytes (TRoleBaseInfo=664, offsetof(dwDataLen)=698, offsetof(pBuffer)=702), so the first live save attempt with an incomplete 664-byte fake was correctly rejected *before* any SQLite write. QA then created a separate 704-byte TRoleData synthetic archive with dwDataLen=704. **Do not remove or weaken length validation to make incomplete test data pass.** No real player's archive was used.

## P0 blockers

1. **GameServer still sends the unversioned ordinary save**, and CoreServer still handles only the old save-ACK marker 791030. Stage 5 versioned Goddess API is reachable only by an explicit new-style save request, not by current GS gameplay. The existing post-trade legacy save will correctly NACK; progress after trade still cannot be saved by current GS.
2. Next stage: GS request generation with nonce and expected token tied to concrete KPlayer session; one in-flight save per role, ACK marker 791031 handling, idempotent retry of same bytes/nonce after ACK loss, fail-closed relog/resync for stale revision and slot reuse; concurrent save and trade ordering must not permit overwrite or silent loss.
3. Versioned login Goddess→Bishop→GS has compiled but has not been tested end-to-end with a real serialized character. Synthetic blobs are not real item inventories.
4. Multi-GS nonce uniqueness, malicious ACK, SQLite lock/busy behavior, interrupted DB transactions, BDB migration path updateBlobData() delete+insert, transfer/disconnect gating, 2 GS and actual item/gold inventory must be tested before release.

**No production processes, binary, source config or real role DB were changed. Do not merge or deploy this Stage 5 Goddess alone.**

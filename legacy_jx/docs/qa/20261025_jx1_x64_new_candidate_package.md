# SwordOnline JX1 x64 ? candidate build and test bundle (2026-10-25)

## Build status
- Base source checkpoint: 51c15bc on checkpoint/jxonline-273-triage-20261024.
- Build direct from the CURRENT WORKTREE (NOT clean HEAD; 181 outstanding changes).
- QA build outputs isolated outside game binaries and DB; post-build deploy copy disabled.
- Fresh CoreServer.dll (x64): MSVC BuildTools 2022 / Rebuild exit 0, zero C/LNK errors.
- Fresh Goddess executable (x64): Rebuild exit 0, zero C/LNK errors.
- Fresh gs001.exe (x64): Rebuild exit 0 against new CoreServer.lib, zero C/LNK errors.
- PE machine field on all three produced binaries verified as AMD64 0x8664.
- Original Core, GameServer and Goddess .vcxproj files verified byte-identical to original SHA256 after builds.
- Temporary per-build QA project files saved outside source and removed from source worktree.

## QA results (fresh candidate)
- FinalRuntime 60/60 PASS, exit 0.
- PersistenceE2E 27/27 PASS, exit 0.
- RuntimeVerified 60/60 PASS, exit 0.
- Total 147/147 PASS against freshly built CoreServer.dll.
- Isolated native Rainbow -> Goddess QA network had already passed 8/8 synthetic money/ACK cases at checkpoint 51c15bc.
- This candidate was NOT booted against real player data or real 3D client.
- Candidate Apply-JX1-Candidate.ps1: PowerShell parser clean, dry-run passed with no write.
- INSTALLER SMOKE in separate fake stopped-server directory: three new SHA matches and three backups match original production binaries byte-for-byte. No production file changed.

## Release files

- ZIP path: C:\Server-Game\SwordOnline\maintenance-builds\JX1_20261025_NEW_BUILD\JX1_SwordOnline_x64_20261025_QA_Candidate.zip
- ZIP size: 2300039 bytes
- ZIP SHA256: A977887170737D393D67E31775281C545274375AFA1C7CB3B8CDC602C6CD2A25
- ZIP contains only 3 new binaries, installer, README, manifest and source-fingerprint/QA summary files; excludes databases, production configurations and account data.
- Source-file fingerprinting records WIP source hashes; checkout of Git HEAD alone cannot necessarily reproduce this build.

## New binary checksums

- gs/CoreServer.dll SHA256 C51011FF90DA5F161EF08D6D7E18EB7CE9955FC224CDB548B1F14208BB459145 bytes 12321280
- gs/gs001.exe SHA256 9C6BAC615085843869607F0B89486EE39EC4C019DB87F9D560F4657FE29B8EAF bytes 1363968
- gw/Goddess.exe SHA256 46651B88FB3A4CF5B2B714EA71C73904CBC0DDE04691E56ABEF14E9FCADA7D47 bytes 1300992

## Manual test deployment

1. All production processes were OFF as confirmed by user; assistant performed no production service start/restart.
2. Extract ZIP under a separate folder; do NOT directly extract over ServerNew.
3. Run powershell -ExecutionPolicy Bypass -File .\Apply-JX1-Candidate.ps1 for dry-run.
4. Only upon explicit decision: powershell -ExecutionPolicy Bypass -File .\Apply-JX1-Candidate.ps1 -Apply
5. Type APPLY JX1 20261025 exactly when prompted. Script refuses if SwordOnline processes are running, verifies SHA, saves binary backups and does not start services.
6. Use existing Bishop/Relay and game resources/configs; they were not rebuilt or packaged.
7. Test character login, movement, gold and ITEM trade, relog, ACK recovery and rollback using test characters with disposable/snapshotted data.

## Remaining production blockers (do not misreport)

- Legacy asynchronous single-role saves still bypass the new trade_role_fence and can overwrite newer atomic-trade snapshots. Requires protocol/persistence reconciliation.
- Same-folder Berkeley DB crash/restart recovery was NOT demonstrated; restart with only the committed SQLite database in a clean QA runtime passed.
- Real GameServer+client inventory transfer, control-unlock, login/relog not yet end-to-end verified.
- Two-role CPackager max serialized load and Snowflake-style TxID monotonicity across clock/server changes need validation.
- 181 unrelated WIP/legacy output entries remain in Git; no attempt to merge, push or discard them.

## Isolation

- The prior production process absence was explained by user manually stopping services.
- No live production service or character DB was touched. No Git remote configured.

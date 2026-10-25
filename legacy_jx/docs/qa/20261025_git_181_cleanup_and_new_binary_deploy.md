# JX1 Git triage and new-binary deployment ? 2026-10-25

## Completed
- The 181 collapsed Git-status entries were classified with a manifest at:
  C:/Server-Game/SwordOnline/maintenance-backups/jxonline-181-20261025/status-inventory.json
- 60 source/project changes preserved in WIP checkpoint ba2c5f4 (9363 insertions, 3861 deletions).
  This does not mean that all legacy source changes passed independent acceptance.
- Remaining 121 collapsed status entries (128 Git-visible files in the stash)
  including old generated binaries, local .bak files, scratch scripts and
  intermediate build files were saved by Git stash with include-untracked.
- Archive Git object: a91bf2521c8fb839d9c1720a971c9970c252fc07
- Permanent protection ref: refs/archive/jxonline-remaining-121-20261025
- Working tree clean, staged files 0. Source branch checkpoint/jxonline-273-triage-20261024.
- No Git remote configured; no merge or push.

## Archived-file recovery
To inspect: git stash show --stat --include-untracked stash@{0}
To restore, preferably in an isolated worktree or after making a new checkpoint:
git stash apply refs/archive/jxonline-remaining-121-20261025
The archive is NOT recommended for live deployment because it includes old
generated binaries. Keep the protective ref and avoid pruning it.

## Production installation completed while services were off

Target root: C:/Server-Game/SwordOnline/ServerNew/_bin_v2_
Binary install source: C:/Server-Game/SwordOnline/maintenance-builds/JX1_20261025_NEW_BUILD/package

Three replaced files, each independently verified against the candidate SHA256:
- gs/CoreServer.dll: C51011FF90DA5F161EF08D6D7E18EB7CE9955FC224CDB548B1F14208BB459145
- gs/gs001.exe: 9C6BAC615085843869607F0B89486EE39EC4C019DB87F9D560F4657FE29B8EAF
- gw/Goddess.exe: 46651B88FB3A4CF5B2B714EA71C73904CBC0DDE04691E56ABEF14E9FCADA7D47

Rollback binaries:
C:/Server-Game/SwordOnline/ServerNew/_bin_v2_/_candidate_patch_backups/JX1_20261025_20261025_090643

None of the production database, user accounts, config, scripts, map files or
other runtime binaries were changed. All services remain OFF for user's test.

## QA and remaining risks
- CoreServer, GameServer and Goddess x64 build: PASS without compiler/link errors.
- Fresh CoreServer test suites 147/147 PASS.
- Isolated Goddess real Rainbow ACK tests 8/8 PASS for synthetic gold transfers,
  with SQLite replay check in clean QA runtime.
- Not accepted yet: real client inventory transfer, relog/unlock, legacy async
  single-role-save race against atomic trade, same-folder Berkeley DB recovery
  after abnormal termination, multi-process TxID ordering, full capacity limits.
- The candidate was built while unrelated worktree libraries were modified.
  Their old Git states were restored by stashing, so future clean builds may
  need fresh dependency rebuilds. Existing produced/deployed candidate hashes
  remain unchanged.

QA archive:
C:/Server-Game/SwordOnline/maintenance-builds/JX1_20261025_NEW_BUILD/JX1_SwordOnline_x64_20261025_QA_Candidate.zip
SHA256 A977887170737D393D67E31775281C545274375AFA1C7CB3B8CDC602C6CD2A25

## Next step
User starts the services manually and tries login, movement, gold/item trade
with test accounts only. Backup game player data before first live trial.

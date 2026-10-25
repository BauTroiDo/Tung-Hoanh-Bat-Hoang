#ifndef JX273_LEGACY_ROLE_SAVE_GUARD_SQL_H
#define JX273_LEGACY_ROLE_SAVE_GUARD_SQL_H

// One atomic SQLite UPDATE: an unversioned write must never cross a
// committed trade fence or overwrite a role migrated to revisioned saves.
// Keeping this SQL in a shared header makes the native QA test execute the
// exact same statement used by sqlitel_db::updateBlobData_n().
namespace JX273LegacyRoleSaveGuard {
static const char* const kUpdate =
    "UPDATE role_Blob SET StudentBlob=?, ekey=? "
    "WHERE roleName=? AND (?=0 OR ekey IS NULL OR ekey<=?) "
    "AND NOT EXISTS(SELECT 1 FROM trade_role_fence "
    "WHERE role_name=role_Blob.roleName) "
    "AND NOT EXISTS(SELECT 1 FROM role_save_revision "
    "WHERE role_name=role_Blob.roleName);";
}
#endif

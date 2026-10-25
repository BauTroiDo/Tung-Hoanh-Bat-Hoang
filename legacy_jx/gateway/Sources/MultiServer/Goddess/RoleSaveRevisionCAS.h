#pragma once
// Stage 1 of JX273 versioned role saves. No legacy packet is accepted here.
// Caller must supply the authenticated UTF-8 database role name.
#include <sqlite3.h>
#include <string>
#include <vector>

namespace JXRoleSaveRevision {
static inline bool Ensure(sqlite3* db) {
    if (!db) return false;
    const char* sql = "CREATE TABLE IF NOT EXISTS role_save_revision ("
        "role_name TEXT PRIMARY KEY, revision INTEGER NOT NULL "
        "CHECK(revision >= 0));";
    return sqlite3_exec(db, sql, NULL, NULL, NULL) == SQLITE_OK;
}
static inline bool PrepareRole(sqlite3* db, const char* sql,
                               const std::string& role, sqlite3_stmt** stmt) {
    *stmt = NULL;
    if (!db || role.empty() ||
        sqlite3_prepare_v2(db, sql, -1, stmt, NULL) != SQLITE_OK) return false;
    if (sqlite3_bind_text(*stmt, 1, role.c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK) {
        sqlite3_finalize(*stmt);
        *stmt = NULL;
        return false;
    }
    return true;
}
static inline int Get(sqlite3* db, const std::string& role, sqlite3_int64* current) {
    if (!current || !Ensure(db)) return -14;
    sqlite3_stmt* stmt = NULL;
    if (!PrepareRole(db, "SELECT revision FROM role_save_revision WHERE role_name=?",
                     role, &stmt)) return -14;
    const int step = sqlite3_step(stmt);
    if (step == SQLITE_ROW) {
        *current = sqlite3_column_int64(stmt, 0);
        sqlite3_finalize(stmt);
        return (*current >= 0) ? 1 : -14;
    }
    sqlite3_finalize(stmt);
    if (step != SQLITE_DONE) return -14;
    // Older committed trades may predate this revision table.
    // A missing revision for a fenced role is NOT a safe zero revision.
    if (!PrepareRole(db, "SELECT 1 FROM trade_role_fence WHERE role_name=?",
                     role, &stmt)) return -14;
    const int fenced = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    if (fenced == SQLITE_ROW) return -17;
    if (fenced != SQLITE_DONE) return -14;
    *current = 0;
    return 1;
}
// Called inside the existing two-role BEGIN IMMEDIATE transaction, after
// the trade receipt/fences are prepared and before COMMIT.
// Replay exits before this method so it cannot increment twice.
static inline bool Advance(sqlite3* db, const std::string& role) {
    sqlite3_stmt* stmt = NULL;
    if (!PrepareRole(db,
        "INSERT OR IGNORE INTO role_save_revision(role_name,revision) VALUES(?,0)",
        role, &stmt)) return false;
    const int insertResult = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    if (insertResult != SQLITE_DONE) return false;
    if (!PrepareRole(db,
        "UPDATE role_save_revision SET revision=revision+1 "
        "WHERE role_name=? AND revision<9223372036854775807",
        role, &stmt)) return false;
    const int updateResult = sqlite3_step(stmt);
    const int updated = sqlite3_changes(db);
    sqlite3_finalize(stmt);
    return updateResult == SQLITE_DONE && updated == 1;
}
// This is an opt-in DB API. It is NOT wired into the legacy ordinary
// save opcode until Goddess/Bishop/GameServer negotiate a revision token.
static inline int Save(sqlite3* db, const std::string& role,
                       const void* blob, int bytes, sqlite3_int64 expected,
                       sqlite3_int64* nextRevision) {
    if (!db || !blob || bytes <= 0 || expected < 0 ||
        expected == 9223372036854775807LL || !nextRevision || role.empty()) return -14;
    if (!Ensure(db)) return -14;
    if (sqlite3_exec(db, "BEGIN IMMEDIATE TRANSACTION", NULL, NULL, NULL) != SQLITE_OK)
        return -14;
    int result = -14;
    do {
        sqlite3_int64 found = -1;
        const int state = Get(db, role, &found);
        if (state != 1) { result = state; break; }
        if (found != expected) { result = -17; break; }
        sqlite3_stmt* stmt = NULL;
        if (!PrepareRole(db,
            "INSERT OR IGNORE INTO role_save_revision(role_name,revision) VALUES(?,0)",
            role, &stmt)) break;
        const int created = sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        if (created != SQLITE_DONE) break;
        if (!PrepareRole(db,
            "UPDATE role_save_revision SET revision=revision+1 "
            "WHERE role_name=? AND revision=? AND revision<9223372036854775807",
            role, &stmt)) break;
        if (sqlite3_bind_int64(stmt, 2, expected) != SQLITE_OK) {
            sqlite3_finalize(stmt);
            break;
        }
        const int advanced = sqlite3_step(stmt);
        const int updated = sqlite3_changes(db);
        sqlite3_finalize(stmt);
        if (advanced != SQLITE_DONE) break;
        if (updated != 1) { result = -17; break; }
        stmt = NULL;
        const char* updateBlob =
            "UPDATE role_Blob SET StudentBlob=? WHERE roleName=?";
        if (sqlite3_prepare_v2(db, updateBlob, -1, &stmt, NULL) != SQLITE_OK) break;
        const bool bound =
            sqlite3_bind_blob(stmt, 1, blob, bytes, SQLITE_TRANSIENT) == SQLITE_OK &&
            sqlite3_bind_text(stmt, 2, role.c_str(), -1, SQLITE_TRANSIENT) == SQLITE_OK;
        const int saved = bound ? sqlite3_step(stmt) : SQLITE_ERROR;
        const int count = sqlite3_changes(db);
        sqlite3_finalize(stmt);
        if (saved != SQLITE_DONE || count != 1) break;
        result = 1;
    } while (false);
    if (result == 1 &&
        sqlite3_exec(db, "COMMIT TRANSACTION", NULL, NULL, NULL) == SQLITE_OK) {
        *nextRevision = expected + 1;
        return 1;
    }
    sqlite3_exec(db, "ROLLBACK TRANSACTION", NULL, NULL, NULL);
    return result == 1 ? -14 : result;
}
// Read a role blob and its revision from one consistent SQLite snapshot.
// Do not fetch the role blob and then query its revision in separate
// transactions: a trade between the two reads would create a stale token.
static inline int LoadSnapshot(sqlite3* db, const std::string& role,
                               std::vector<unsigned char>* blob,
                               sqlite3_int64* revision) {
    if (!db || role.empty() || !blob || !revision || !Ensure(db))
        return -14;
    if (sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, NULL) != SQLITE_OK)
        return -14;
    int result = -14;
    sqlite3_int64 candidateRevision = -1;
    std::vector<unsigned char> candidate;
    do {
        sqlite3_stmt* stmt = NULL;
        if (!PrepareRole(db,
            "SELECT StudentBlob FROM role_Blob WHERE roleName=? LIMIT 2",
            role, &stmt)) break;
        const int first = sqlite3_step(stmt);
        if (first != SQLITE_ROW) {
            sqlite3_finalize(stmt);
            break;
        }
        const void* data = sqlite3_column_blob(stmt, 0);
        const int length = sqlite3_column_bytes(stmt, 0);
        if (!data || length <= 0 || length > 16 * 1024 * 1024) {
            sqlite3_finalize(stmt);
            break;
        }
        candidate.assign((const unsigned char*)data,
                         (const unsigned char*)data + length);
        const int second = sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        if (second != SQLITE_DONE) break; // duplicate roleName or SQL error
        const int read = Get(db, role, &candidateRevision);
        if (read != 1) {
            result = read;
            break;
        }
        result = 1;
    } while (false);
    if (result == 1 &&
        sqlite3_exec(db, "COMMIT TRANSACTION", NULL, NULL, NULL) == SQLITE_OK) {
        blob->swap(candidate);
        *revision = candidateRevision;
        return 1;
    }
    sqlite3_exec(db, "ROLLBACK TRANSACTION", NULL, NULL, NULL);
    return result == 1 ? -14 : result;
}
} // namespace JXRoleSaveRevision

#pragma once
// Stage 5: durable idempotent revision-CAS character saves (Goddess SQLite).
// The (role,nonce,expected,blob fingerprint) receipt commits atomically with
// the revision and StudentBlob; an ACK lost after COMMIT is safe to replay.
// This API is opt-in: legacy opcodes must not bypass the trade fence.
#include "RoleSaveRevisionCAS.h"
#include <stdint.h>
#include <string>

namespace JXRoleSaveReceipt {
static inline uint64_t Fingerprint(const void* p, int n) {
    const unsigned char* b = (const unsigned char*)p;
    uint64_t h = UINT64_C(14695981039346656037);
    for (int i=0; i<n; ++i) { h ^= b[i]; h *= UINT64_C(1099511628211); }
    return h;
}
static inline bool Ensure(sqlite3* db) {
    return db && JXRoleSaveRevision::Ensure(db) &&
        sqlite3_exec(db,
            "CREATE TABLE IF NOT EXISTS role_save_receipts("
            "role_name TEXT NOT NULL, nonce INTEGER NOT NULL,"
            "expected_revision INTEGER NOT NULL,"
            "committed_revision INTEGER NOT NULL,"
            "blob_bytes INTEGER NOT NULL, blob_digest INTEGER NOT NULL,"
            "PRIMARY KEY(role_name,nonce));",
            NULL,NULL,NULL)==SQLITE_OK;
}
static inline int Save(sqlite3* db, const std::string& role,
    const void* blob, int bytes, sqlite3_int64 expected, uint64_t nonce,
    sqlite3_int64* nextRevision) {
    if (!db || !blob || bytes<=0 || bytes>128*1024 || role.empty() ||
        expected<0 || expected==INT64_MAX || nonce==0 || !nextRevision ||
        !Ensure(db)) return -14;
    if (sqlite3_exec(db,"BEGIN IMMEDIATE TRANSACTION",NULL,NULL,NULL)!=SQLITE_OK)
        return -14;
    int result=-14;
    const uint64_t hash=Fingerprint(blob,bytes);
    sqlite3_int64 output=-1;
    do {
        sqlite3_stmt* stmt=NULL;
        if (sqlite3_prepare_v2(db,
            "SELECT expected_revision,committed_revision,blob_bytes,blob_digest "
            "FROM role_save_receipts WHERE role_name=? AND nonce=?",
            -1,&stmt,NULL)!=SQLITE_OK) break;
        const bool ok=sqlite3_bind_text(stmt,1,role.c_str(),-1,SQLITE_TRANSIENT)==SQLITE_OK &&
            sqlite3_bind_int64(stmt,2,(sqlite3_int64)nonce)==SQLITE_OK;
        const int s=ok?sqlite3_step(stmt):SQLITE_ERROR;
        if(s==SQLITE_ROW) {
            const sqlite3_int64 previous=sqlite3_column_int64(stmt,0);
            const sqlite3_int64 committed=sqlite3_column_int64(stmt,1);
            const int length=sqlite3_column_int(stmt,2);
            const uint64_t saved=(uint64_t)sqlite3_column_int64(stmt,3);
            output=committed;
            result=(previous==expected && length==bytes && saved==hash &&
                    committed==expected+1)?1:-13;
            // A later trade/save could already have advanced the role. In
            // that case replaying the old ACK must not advertise a stale token.
            if (result==1) {
                sqlite3_int64 currentRevision=-1;
                const int got=JXRoleSaveRevision::Get(db,role,&currentRevision);
                if(got!=1) result=got;
                else if(currentRevision!=committed) result=-17;
            }
        } else if(s!=SQLITE_DONE) result=-14;
        else result=0;
        sqlite3_finalize(stmt);
        if(result!=0)break;
        sqlite3_int64 current=-1;
        result=JXRoleSaveRevision::Get(db,role,&current);
        if(result!=1)break;
        if(current!=expected){result=-17;break;}
        stmt=NULL;
        if (sqlite3_prepare_v2(db,
            "INSERT OR IGNORE INTO role_save_revision(role_name,revision) VALUES(?,0)",
            -1,&stmt,NULL)!=SQLITE_OK) {result=-14;break;}
        const bool bound=sqlite3_bind_text(stmt,1,role.c_str(),-1,SQLITE_TRANSIENT)==SQLITE_OK;
        const int inserted=bound?sqlite3_step(stmt):SQLITE_ERROR;
        sqlite3_finalize(stmt);
        if(inserted!=SQLITE_DONE){result=-14;break;}
        stmt=NULL;
        if(sqlite3_prepare_v2(db,
            "UPDATE role_save_revision SET revision=revision+1 "
            "WHERE role_name=? AND revision=? AND revision<9223372036854775807",
            -1,&stmt,NULL)!=SQLITE_OK){result=-14;break;}
        const bool revBound=sqlite3_bind_text(stmt,1,role.c_str(),-1,SQLITE_TRANSIENT)==SQLITE_OK &&
            sqlite3_bind_int64(stmt,2,expected)==SQLITE_OK;
        const int revStep=revBound?sqlite3_step(stmt):SQLITE_ERROR;
        const int revChanged=sqlite3_changes(db);
        sqlite3_finalize(stmt);
        if(revStep!=SQLITE_DONE || revChanged!=1){result=-17;break;}
        stmt=NULL;
        if(sqlite3_prepare_v2(db,"UPDATE role_Blob SET StudentBlob=? WHERE roleName=?",
            -1,&stmt,NULL)!=SQLITE_OK){result=-14;break;}
        const bool blobBound=sqlite3_bind_blob(stmt,1,blob,bytes,SQLITE_TRANSIENT)==SQLITE_OK &&
            sqlite3_bind_text(stmt,2,role.c_str(),-1,SQLITE_TRANSIENT)==SQLITE_OK;
        const int blobStep=blobBound?sqlite3_step(stmt):SQLITE_ERROR;
        const int blobChanged=sqlite3_changes(db);
        sqlite3_finalize(stmt);
        if(blobStep!=SQLITE_DONE || blobChanged!=1){result=-14;break;}
        stmt=NULL;
        if(sqlite3_prepare_v2(db,
            "INSERT INTO role_save_receipts(role_name,nonce,expected_revision,"
            "committed_revision,blob_bytes,blob_digest) VALUES(?,?,?,?,?,?)",
            -1,&stmt,NULL)!=SQLITE_OK){result=-14;break;}
        const bool receiptBound=
            sqlite3_bind_text(stmt,1,role.c_str(),-1,SQLITE_TRANSIENT)==SQLITE_OK &&
            sqlite3_bind_int64(stmt,2,(sqlite3_int64)nonce)==SQLITE_OK &&
            sqlite3_bind_int64(stmt,3,expected)==SQLITE_OK &&
            sqlite3_bind_int64(stmt,4,expected+1)==SQLITE_OK &&
            sqlite3_bind_int(stmt,5,bytes)==SQLITE_OK &&
            sqlite3_bind_int64(stmt,6,(sqlite3_int64)hash)==SQLITE_OK;
        const int receiptStep=receiptBound?sqlite3_step(stmt):SQLITE_ERROR;
        sqlite3_finalize(stmt);
        if(receiptStep!=SQLITE_DONE){result=-14;break;}
        output=expected+1;
        result=1;
    } while(false);
    if(result==1 &&
       sqlite3_exec(db,"COMMIT TRANSACTION",NULL,NULL,NULL)==SQLITE_OK){
        *nextRevision=output;
        return 1;
    }
    sqlite3_exec(db,"ROLLBACK TRANSACTION",NULL,NULL,NULL);
    return result==1?-14:result;
}
} // namespace JXRoleSaveReceipt

#ifndef JX273_SAFE_BDB_ROLE_IMPORT_H
#define JX273_SAFE_BDB_ROLE_IMPORT_H
// Offline BDB -> SQLite migrations are create-only and fail closed.
// A maintenance import cannot delete/reinsert an existing character or
// overwrite committed trade/ordinary-save revisions.
#include <sqlite3.h>
#include <string>
namespace JX273SafeBdbImport {
static inline bool ImportNew(sqlite3* db,const std::string& role,
                             const void* blob,int bytes,sqlite3_int64 ekey) {
    if (!db || role.empty() || role.size()>128 || !blob ||
        bytes<=0 || bytes>128*1024 || ekey<0) return false;
    if (sqlite3_exec(db,"BEGIN IMMEDIATE TRANSACTION",NULL,NULL,NULL)!=SQLITE_OK)
        return false;
    sqlite3_stmt* st=NULL;
    const char* insert =
        "INSERT INTO role_Blob(roleName,StudentBlob,ekey) "
        "SELECT ?,?,? "
        "WHERE NOT EXISTS(SELECT 1 FROM role_Blob WHERE roleName=?) "
        "AND NOT EXISTS(SELECT 1 FROM trade_role_fence WHERE role_name=?) "
        "AND NOT EXISTS(SELECT 1 FROM role_save_revision WHERE role_name=?);";
    bool success=false;
    if(sqlite3_prepare_v2(db,insert,-1,&st,NULL)==SQLITE_OK) {
        const bool bound=
            sqlite3_bind_text(st,1,role.c_str(),-1,SQLITE_TRANSIENT)==SQLITE_OK &&
            sqlite3_bind_blob(st,2,blob,bytes,SQLITE_TRANSIENT)==SQLITE_OK &&
            sqlite3_bind_int64(st,3,ekey)==SQLITE_OK &&
            sqlite3_bind_text(st,4,role.c_str(),-1,SQLITE_TRANSIENT)==SQLITE_OK &&
            sqlite3_bind_text(st,5,role.c_str(),-1,SQLITE_TRANSIENT)==SQLITE_OK &&
            sqlite3_bind_text(st,6,role.c_str(),-1,SQLITE_TRANSIENT)==SQLITE_OK;
        const int code=bound?sqlite3_step(st):SQLITE_ERROR;
        const int rows=sqlite3_changes(db);
        success=(code==SQLITE_DONE && rows==1);
    }
    if(st) sqlite3_finalize(st);
    if(success &&
        sqlite3_exec(db,"COMMIT TRANSACTION",NULL,NULL,NULL)==SQLITE_OK)
        return true;
    sqlite3_exec(db,"ROLLBACK TRANSACTION",NULL,NULL,NULL);
    return false;
}
}
#endif

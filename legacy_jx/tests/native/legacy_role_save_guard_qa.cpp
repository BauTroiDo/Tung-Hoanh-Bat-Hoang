#include <sqlite3.h>
#include <cstdio>
#include <cstring>
#include <string>
#include <thread>
#include <atomic>
#include <chrono>
#include "LegacyRoleSaveGuardSql.h"
static int passes=0;
static void Check(bool pass,const char* what) {
    printf("%s %s\n",pass?"PASS":"FAIL",what);
    if(!pass) exit(3);
    ++passes;
}
static bool Sql(sqlite3* db,const char* sql) {
    return sqlite3_exec(db,sql,NULL,NULL,NULL)==SQLITE_OK;
}
static int Save(sqlite3* db,const char* name,const char* data,
                sqlite3_int64 key) {
    sqlite3_stmt* st=nullptr;
    if(sqlite3_prepare_v2(db,JX273LegacyRoleSaveGuard::kUpdate,-1,&st,nullptr)!=SQLITE_OK)
        return -1;
    bool bound=sqlite3_bind_blob(st,1,data,(int)strlen(data),SQLITE_TRANSIENT)==SQLITE_OK &&
        sqlite3_bind_int64(st,2,key)==SQLITE_OK &&
        sqlite3_bind_text(st,3,name,-1,SQLITE_TRANSIENT)==SQLITE_OK &&
        sqlite3_bind_int64(st,4,key)==SQLITE_OK &&
        sqlite3_bind_int64(st,5,key)==SQLITE_OK;
    const int step=bound?sqlite3_step(st):SQLITE_ERROR;
    const int changed=sqlite3_changes(db);
    sqlite3_finalize(st);
    if(step!=SQLITE_DONE)return -1;
    return changed;
}
static std::string GetBlob(sqlite3* db,const char* name) {
    sqlite3_stmt* st=nullptr;
    if(sqlite3_prepare_v2(db,"SELECT StudentBlob FROM role_Blob WHERE roleName=?",-1,&st,nullptr)!=SQLITE_OK)
        return "?";
    sqlite3_bind_text(st,1,name,-1,SQLITE_TRANSIENT);
    std::string result;
    if(sqlite3_step(st)==SQLITE_ROW) {
        const void* b=sqlite3_column_blob(st,0);
        result.assign((const char*)b,sqlite3_column_bytes(st,0));
    }
    sqlite3_finalize(st);
    return result;
}
static sqlite3_int64 GetKey(sqlite3* db,const char* name) {
    sqlite3_stmt* st=nullptr;
    sqlite3_prepare_v2(db,"SELECT ekey FROM role_Blob WHERE roleName=?",-1,&st,nullptr);
    sqlite3_bind_text(st,1,name,-1,SQLITE_TRANSIENT);
    sqlite3_int64 key=-1;
    if(sqlite3_step(st)==SQLITE_ROW)key=sqlite3_column_int64(st,0);
    sqlite3_finalize(st);
    return key;
}
int main(int argc,char**argv) {
    Check(argc==2,"disposable database required");
    sqlite3* a=nullptr;sqlite3* b=nullptr;
    Check(sqlite3_open(argv[1],&a)==SQLITE_OK,"open SQLite");
    Check(Sql(a,"CREATE TABLE role_Blob(ID INTEGER PRIMARY KEY,roleName TEXT UNIQUE,StudentBlob BLOB,ekey INTEGER)"),"create role table");
    Check(Sql(a,"CREATE TABLE trade_role_fence(role_name TEXT PRIMARY KEY,last_tx_id INTEGER)"),"create fence");
    Check(Sql(a,"CREATE TABLE role_save_revision(role_name TEXT PRIMARY KEY,revision INTEGER NOT NULL)"),"create ledger");
    Check(Sql(a,"INSERT INTO role_Blob VALUES(1,'QA_A',x'4130',10),(2,'QA_B',x'4230',20),(3,'QA_REV',x'4330',30)"),"seed roles");
    Check(Save(a,"QA_A","A1",11)==1,"legacy progress before trade allowed");
    Check(GetBlob(a,"QA_A")=="A1" && GetKey(a,"QA_A")==11,"blob and ekey updated");
    Check(Save(a,"QA_A","STALE",9)==0,"old ekey rejected");
    Check(GetBlob(a,"QA_A")=="A1","rejected stale write preserves role");
    Check(Save(a,"QA_A","A2",0)==1,"unversioned pre-trade save allowed");
    Check(Sql(a,"INSERT INTO trade_role_fence VALUES('QA_A',12345)"),"simulate committed trade fence");
    Check(Save(a,"QA_A","LATE",0)==0,"legacy save after trade rejected");
    Check(GetBlob(a,"QA_A")=="A2","committed role preserved from late save");
    Check(Save(a,"QA_A","HIGH_KEY",999)==0,"high ekey cannot bypass trade fence");
    Check(Save(a,"QA_B","B1",21)==1,"unfenced counterpart legacy save works");
    Check(Sql(a,"INSERT INTO role_save_revision VALUES('QA_REV',1)"),"CAS migration fixture");
    Check(Save(a,"QA_REV","BYPASS",0)==0,"legacy cannot overwrite CAS role");
    Check(GetBlob(a,"QA_REV")=="C0" && GetKey(a,"QA_REV")==30,"CAS role unchanged");
    Check(Save(a,"NO_ROLE","ORPHAN",0)==0,"cannot create role with update");
    Check(Save(a,"QA_B","B2",0)==1,"existing unfenced version zero allowed");
    Check(sqlite3_open(argv[1],&b)==SQLITE_OK,"open concurrent DB connection");
    sqlite3_busy_timeout(b,3000);
    Check(Sql(a,"BEGIN IMMEDIATE TRANSACTION"),"writer transaction locks DB");
    Check(Sql(a,"UPDATE role_Blob SET StudentBlob=x'4233' WHERE roleName='QA_B'"),"trade writes second role");
    Check(Sql(a,"INSERT INTO trade_role_fence VALUES('QA_B',99999)"),"fence committed with trade");
    std::atomic<int> waiting{-99};
    std::thread late([&] {waiting.store(Save(b,"QA_B","LATE_FROM_OTHER_CONNECTION",0));});
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    Check(Sql(a,"COMMIT TRANSACTION"),"trade atomically committed");
    late.join();
    Check(waiting.load()==0,"late save on second connection blocked");
    Check(GetBlob(a,"QA_B")=="B3","trade snapshot wins after concurrent save");
    Check(GetKey(a,"QA_B")==0,"legacy ekey unchanged by committed trade");
    Check(Sql(a,"INSERT INTO role_save_revision VALUES('QA_B',2)"),"fenced and revisioned state");
    Check(Save(a,"QA_B","LATE_AGAIN",0)==0,"both guards still block");
    Check(Sql(a,"PRAGMA integrity_check"),"SQLite remains accessible");
    Check(sqlite3_close(b)==SQLITE_OK,"close second connection");
    Check(sqlite3_close(a)==SQLITE_OK,"close initial DB");
    Check(sqlite3_open(argv[1],&a)==SQLITE_OK,"cold reopen");
    Check(GetBlob(a,"QA_A")=="A2" && GetBlob(a,"QA_B")=="B3","committed data persists after restart");
    Check(sqlite3_close(a)==SQLITE_OK,"final close");
    printf("SUMMARY %d/%d PASS\n",passes,passes);
    return 0;
}

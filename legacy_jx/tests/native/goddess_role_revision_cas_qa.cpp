#include <sqlite3.h>
#include <cstdio>
#include <cstring>
#include <string>
#include "RoleSaveRevisionCAS.h"

static int checks=0;
static void Check(bool ok,const char* what) {
    printf("%s %s\n", ok?"PASS":"FAIL", what);
    ++checks;
    if (!ok) { fflush(stdout); exit(2); }
}
static bool Exec(sqlite3* db,const char* sql) {
    return sqlite3_exec(db,sql,NULL,NULL,NULL)==SQLITE_OK;
}
static std::string Blob(sqlite3* db,const char* role) {
    sqlite3_stmt* s=NULL;
    if(sqlite3_prepare_v2(db,"SELECT StudentBlob FROM role_Blob WHERE roleName=?",-1,&s,NULL)!=SQLITE_OK)
        return "?";
    sqlite3_bind_text(s,1,role,-1,SQLITE_TRANSIENT);
    std::string result;
    if(sqlite3_step(s)==SQLITE_ROW) {
        const char* p=(const char*)sqlite3_column_blob(s,0);
        int n=sqlite3_column_bytes(s,0);
        result.assign(p,p+n);
    }
    sqlite3_finalize(s);
    return result;
}
int main(int argc,char**argv) {
    const char* path=argc>1?argv[1]:"role_save_cas_native.db";
    sqlite3* db=NULL;
    Check(sqlite3_open(path,&db)==SQLITE_OK,"db open");
    Check(Exec(db,"CREATE TABLE role_Blob(roleName TEXT PRIMARY KEY,StudentBlob BLOB,ekey INTEGER)"),
          "fixture role_Blob");
    Check(Exec(db,"CREATE TABLE trade_role_fence(role_name TEXT PRIMARY KEY,last_tx_id INTEGER NOT NULL)"),
          "fixture trade_role_fence");
    Check(Exec(db,"INSERT INTO role_Blob VALUES('QA_A',x'4130',11),('QA_B',x'4230',22),('QA_OLD',x'4330',33)"),"fixture roles");
    Check(Exec(db,"INSERT INTO trade_role_fence VALUES('QA_OLD',123)"),"old fenced fixture");
    Check(JXRoleSaveRevision::Ensure(db),"create revision schema");
    sqlite3_int64 rev=987;
    Check(JXRoleSaveRevision::Get(db,"QA_A",&rev)==1 && rev==0,"fresh role revision zero");
    Check(JXRoleSaveRevision::Get(db,"QA_OLD",&rev)==-17,"legacy fenced role with absent version fails closed");
    Check(JXRoleSaveRevision::Save(db,"QA_A","A1",2,0,&rev)==1 && rev==1,"ordinary save A rev0 -> rev1");
    Check(JXRoleSaveRevision::Save(db,"QA_B","B1",2,0,&rev)==1 && rev==1,"ordinary save B rev0 -> rev1");
    Check(JXRoleSaveRevision::Save(db,"QA_A","STALE",5,0,&rev)==-17 &&
          Blob(db,"QA_A")=="A1","late ordinary save cannot replace newer snapshot");
    Check(Exec(db,"BEGIN IMMEDIATE TRANSACTION"),"trade BEGIN IMMEDIATE");
    Check(Exec(db,"UPDATE role_Blob SET StudentBlob=x'4132' WHERE roleName='QA_A'"),"trade writes A");
    Check(Exec(db,"UPDATE role_Blob SET StudentBlob=x'4232' WHERE roleName='QA_B'"),"trade writes B");
    Check(JXRoleSaveRevision::Advance(db,"QA_A"),"trade advances A");
    Check(JXRoleSaveRevision::Advance(db,"QA_B"),"trade advances B");
    Check(Exec(db,"COMMIT TRANSACTION"),"trade COMMIT");
    Check(JXRoleSaveRevision::Get(db,"QA_A",&rev)==1 && rev==2,"trade durable A revision 2");
    Check(JXRoleSaveRevision::Get(db,"QA_B",&rev)==1 && rev==2,"trade durable B revision 2");
    Check(JXRoleSaveRevision::Save(db,"QA_A","OLD_SAVE",8,1,&rev)==-17 &&
          Blob(db,"QA_A")=="A2","save queued before trade cannot overwrite");
    Check(JXRoleSaveRevision::Save(db,"QA_A","A3",2,2,&rev)==1 && rev==3,
          "post-trade progression saves successfully");
    Check(JXRoleSaveRevision::Save(db,"QA_B","B3",2,2,&rev)==1 && rev==3,
          "counterparty progression saves successfully");
    Check(JXRoleSaveRevision::Save(db,"MISSING","BAD",3,0,&rev)==-14,
          "missing role cannot create orphan revision");
    Check(JXRoleSaveRevision::Get(db,"MISSING",&rev)==1 && rev==0,
          "failed missing role update rolled back revision");
    Check(JXRoleSaveRevision::Save(db,"QA_OLD","BAD",3,0,&rev)==-17 &&
          Blob(db,"QA_OLD")=="C0","old trade fence protected during migration");
    Check(sqlite3_close(db)==SQLITE_OK,"db close");
    db=NULL;
    Check(sqlite3_open(path,&db)==SQLITE_OK,"reopen committed database");
    Check(JXRoleSaveRevision::Get(db,"QA_A",&rev)==1 && rev==3,"relog/restart token persisted");
    Check(JXRoleSaveRevision::Save(db,"QA_A","STALE",5,2,&rev)==-17 &&
          Blob(db,"QA_A")=="A3","stale save rejected after restart");
    Check(JXRoleSaveRevision::Save(db,"QA_A","A4",2,3,&rev)==1 && rev==4,
          "fresh post-restart save succeeds");
    Check(JXRoleSaveRevision::Get(db,"QA_B",&rev)==1 && rev==3,
          "independent role revision preserved");
    Check(sqlite3_close(db)==SQLITE_OK,"final close");
    printf("SUMMARY: %d/%d PASS\n",checks,checks);
    return 0;
}

#include <sqlite3.h>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>
#include "RoleSaveVersionedReceipt.h"
static int count=0;
static void Check(bool ok,const char* label) {
    printf("%s %s\n",ok?"PASS":"FAIL",label);
    fflush(stdout);
    if(!ok) exit(7);
    ++count;
}
static bool Exec(sqlite3* db,const char* s) {
    return sqlite3_exec(db,s,NULL,NULL,NULL)==SQLITE_OK;
}
static std::string Blob(sqlite3* db,const char* name) {
    sqlite3_stmt* stmt=NULL;
    if(sqlite3_prepare_v2(db,"SELECT StudentBlob FROM role_Blob WHERE roleName=?",
        -1,&stmt,NULL)!=SQLITE_OK)return {};
    sqlite3_bind_text(stmt,1,name,-1,SQLITE_TRANSIENT);
    std::string value;
    if(sqlite3_step(stmt)==SQLITE_ROW){
        const char* b=(const char*)sqlite3_column_blob(stmt,0);
        value.assign(b,(size_t)sqlite3_column_bytes(stmt,0));
    }
    sqlite3_finalize(stmt);
    return value;
}
int main(int argc,char** argv) {
    Check(argc==2,"isolated fixture path");
    sqlite3* db=NULL;
    Check(sqlite3_open(argv[1],&db)==SQLITE_OK,"open SQLite");
    Check(Exec(db,"CREATE TABLE role_Blob(roleName TEXT PRIMARY KEY,StudentBlob BLOB,ekey INTEGER)"),
          "create roles table");
    Check(Exec(db,"CREATE TABLE trade_role_fence(role_name TEXT PRIMARY KEY,last_tx_id INTEGER)"),
          "create trade fence");
    Check(Exec(db,"INSERT INTO role_Blob VALUES('QA_A',x'4130',10),('QA_B',x'4230',20),"
          "('QA_OLD',x'4330',30)"),"seed roles");
    Check(Exec(db,"INSERT INTO trade_role_fence VALUES('QA_OLD',100)"),
          "older fenced role");
    sqlite3_int64 next=55;
    using JXRoleSaveReceipt::Save;
    Check(Save(db,"QA_A","A1",2,0,12345,&next)==1 && next==1,
          "save revision zero to one");
    Check(Blob(db,"QA_A")=="A1","save blob correct");
    Check(Save(db,"QA_A","A1",2,0,12345,&next)==1 && next==1,
          "replay same nonce idempotent");
    Check(Save(db,"QA_A","HACK",4,0,12345,&next)==-13,
          "nonce collision wrong blob blocked");
    Check(Save(db,"QA_A","A1",2,1,12345,&next)==-13,
          "nonce collision wrong expected blocked");
    Check(Save(db,"QA_A","STALE",5,0,12346,&next)==-17,
          "new nonce stale revision blocked");
    Check(Blob(db,"QA_A")=="A1","stale write preserves data");
    Check(Save(db,"QA_A","A2",2,1,12346,&next)==1 && next==2,
          "fresh progress advances");
    Check(Save(db,"QA_B","B1",2,0,22345,&next)==1 && next==1,
          "second role independent");
    Check(Save(db,"QA_OLD","BAD",3,0,33333,&next)==-17,
          "old trade fenced role refuses invented zero");
    Check(Save(db,"MISSING","BAD",3,0,34567,&next)==-14,
          "missing role transaction rolled back");
    Check(Save(db,"QA_A","N",1,2,0,&next)==-14,"zero nonce rejected");
    Check(Exec(db,"BEGIN IMMEDIATE"),"transaction for atomic trade");
    Check(Exec(db,"UPDATE role_Blob SET StudentBlob=x'4133' WHERE roleName='QA_A'"),
          "trade replaces role snapshot");
    Check(JXRoleSaveRevision::Advance(db,"QA_A"),"trade advances rev");
    Check(Exec(db,"COMMIT"),"trade commits snapshot+revision");
    Check(Save(db,"QA_A","STALE",5,2,12347,&next)==-17,
          "pre-trade save rejected");
    Check(Save(db,"QA_A","A4",2,3,12348,&next)==1 && next==4,
          "post-trade progress succeeds");
    Check(Exec(db,"PRAGMA integrity_check"),"integrity check executes");
    Check(sqlite3_close(db)==SQLITE_OK,"close SQLite");
    Check(sqlite3_open(argv[1],&db)==SQLITE_OK,"cold reopen SQLite");
    Check(Save(db,"QA_A","A4",2,3,12348,&next)==1 && next==4,
          "ACK loss recovery after restart");
    Check(Save(db,"QA_A","A5",2,4,12349,&next)==1 && next==5,
          "next save after ACK replay");
    Check(Blob(db,"QA_A")=="A5","latest blob durable");
    Check(Save(db,"QA_A","A4",2,3,12348,&next)==-17,
          "older committed receipt fails resync when role moved on");
    sqlite3_stmt* s=NULL;
    Check(sqlite3_prepare_v2(db,"SELECT COUNT(*) FROM role_save_receipts WHERE role_name='QA_A'",
        -1,&s,NULL)==SQLITE_OK,"receipt count query");
    Check(sqlite3_step(s)==SQLITE_ROW && sqlite3_column_int(s,0)==4,
          "no duplicate receipt created for replay");
    sqlite3_finalize(s);
    Check(sqlite3_close(db)==SQLITE_OK,"final close");
    printf("SUMMARY %d/%d PASS\n",count,count);
    return 0;
}

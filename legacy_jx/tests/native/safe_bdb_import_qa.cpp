#include <sqlite3.h>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <thread>
#include <atomic>
#include <chrono>
#include "SafeBdbRoleImport.h"
static int passed=0;
static void Check(bool value,const char* what) {
    printf("%s %s\n",value?"PASS":"FAIL",what);
    if(!value) exit(5);
    ++passed;
}
static bool Exec(sqlite3* db,const char* s) {
    return sqlite3_exec(db,s,NULL,NULL,NULL)==SQLITE_OK;
}
static std::string Get(sqlite3* db,const char* role) {
    sqlite3_stmt* s=NULL;
    if(sqlite3_prepare_v2(db,"SELECT StudentBlob FROM role_Blob WHERE roleName=?",-1,&s,NULL)!=SQLITE_OK)
        return "?";
    sqlite3_bind_text(s,1,role,-1,SQLITE_TRANSIENT);
    std::string blob;
    if(sqlite3_step(s)==SQLITE_ROW) {
        const char* b=(const char*)sqlite3_column_blob(s,0);
        blob.assign(b,sqlite3_column_bytes(s,0));
    }
    sqlite3_finalize(s);
    return blob;
}
static sqlite3_int64 Id(sqlite3* db,const char* role) {
    sqlite3_stmt* s=NULL;
    sqlite3_prepare_v2(db,"SELECT ID FROM role_Blob WHERE roleName=?",-1,&s,NULL);
    sqlite3_bind_text(s,1,role,-1,SQLITE_TRANSIENT);
    sqlite3_int64 id=-1;
    if(sqlite3_step(s)==SQLITE_ROW)id=sqlite3_column_int64(s,0);
    sqlite3_finalize(s);
    return id;
}
int main(int argc,char**argv){
    Check(argc==2,"isolated DB argument");
    sqlite3 *db=NULL,*other=NULL;
    Check(sqlite3_open(argv[1],&db)==SQLITE_OK,"SQLite open");
    Check(Exec(db,"PRAGMA journal_mode=WAL"),"WAL concurrency setting");
    Check(Exec(db,"CREATE TABLE role_Blob(ID INTEGER PRIMARY KEY AUTOINCREMENT,roleName TEXT,StudentBlob BLOB,ekey INTEGER)"),"legacy role schema");
    Check(Exec(db,"CREATE TABLE trade_role_fence(role_name TEXT PRIMARY KEY,last_tx_id INTEGER)"),"trade fence schema");
    Check(Exec(db,"CREATE TABLE role_save_revision(role_name TEXT PRIMARY KEY,revision INTEGER)"),"revision schema");
    Check(Exec(db,"INSERT INTO role_Blob(roleName,StudentBlob,ekey) VALUES('LIVE',x'4c495645',1234)"),"live role fixture");
    const sqlite3_int64 liveId=Id(db,"LIVE");
    Check(liveId>0,"live ID assigned");
    Check(!JX273SafeBdbImport::ImportNew(db,"LIVE","BAD",3,5678),"cannot overwrite an existing role");
    Check(Get(db,"LIVE")=="LIVE" && Id(db,"LIVE")==liveId,"live blob and ID preserved");
    Check(Exec(db,"INSERT INTO trade_role_fence VALUES('FENCED',100)"),"orphan fence fixture");
    Check(!JX273SafeBdbImport::ImportNew(db,"FENCED","COPY",4,100),"orphan fenced role cannot be imported");
    Check(Get(db,"FENCED").empty(),"no role inserted for fence");
    Check(Exec(db,"INSERT INTO role_save_revision VALUES('REVISED',9)"),"orphan revision fixture");
    Check(!JX273SafeBdbImport::ImportNew(db,"REVISED","OLD",3,9),"orphan versioned role cannot be imported");
    Check(JX273SafeBdbImport::ImportNew(db,"NEW_ROLE","NEW",3,45),"new role create-only import");
    Check(Get(db,"NEW_ROLE")=="NEW" && Id(db,"NEW_ROLE")>liveId,"fresh role inserted correctly");
    Check(!JX273SafeBdbImport::ImportNew(db,"NEW_ROLE","STALE",5,90),"duplicate migration denied");
    Check(Get(db,"NEW_ROLE")=="NEW","duplicate import does not erase data");
    Check(Exec(db,"INSERT INTO trade_role_fence VALUES('LIVE',101)"),"existing trade fence");
    Check(!JX273SafeBdbImport::ImportNew(db,"LIVE","BAD",3,1),"trade fence refuses overwrite");
    Check(Exec(db,"INSERT INTO role_save_revision VALUES('LIVE',13)"),"existing role revision");
    Check(!JX273SafeBdbImport::ImportNew(db,"LIVE","BAD",3,1),"both protections refuse overwrite");
    Check(!JX273SafeBdbImport::ImportNew(db,"", "BAD",3,1),"empty name rejected");
    Check(!JX273SafeBdbImport::ImportNew(db,"A", NULL,3,1),"null blob rejected");
    Check(!JX273SafeBdbImport::ImportNew(db,"A","BAD",0,1),"zero blob rejected");
    Check(!JX273SafeBdbImport::ImportNew(db,"A","BAD",3,-1),"negative ekey rejected");
    const char zero[]={'A','\0','Z'};
    Check(JX273SafeBdbImport::ImportNew(db,"BINARY",zero,3,0),"binary with zero imported");
    Check(Get(db,"BINARY").size()==3 && Get(db,"BINARY")[2]=='Z',"binary payload byte-exact");
    Check(JX273SafeBdbImport::ImportNew(db,"test' OR 1=1--","Q",1,1),"SQL injection-like role treated as literal");
    Check(Get(db,"LIVE")=="LIVE","SQL role quoting never changes existing role");
    Check(sqlite3_open(argv[1],&other)==SQLITE_OK,"second connection");
    sqlite3_busy_timeout(other,3000);
    Check(Exec(db,"BEGIN IMMEDIATE TRANSACTION"),"writer locked");
    Check(Exec(db,"INSERT INTO role_Blob(roleName,StudentBlob,ekey) VALUES('RACING',x'57494e',5)"),"new committed role wins");
    std::atomic<bool> imported{true};
    std::thread late([&]{imported.store(JX273SafeBdbImport::ImportNew(other,"RACING","LOSE",4,6));});
    std::this_thread::sleep_for(std::chrono::milliseconds(90));
    Check(Exec(db,"COMMIT"),"transaction committed");
    late.join();
    Check(!imported.load(),"late import refused across SQLite connections");
    Check(Get(db,"RACING")=="WIN","race cannot overwrite newly committed role");
    Check(Exec(db,"PRAGMA integrity_check"),"integrity pragma works");
    Check(sqlite3_close(other)==SQLITE_OK,"second connection close");
    Check(sqlite3_close(db)==SQLITE_OK,"first connection close");
    Check(sqlite3_open(argv[1],&db)==SQLITE_OK,"cold reopen");
    Check(Get(db,"LIVE")=="LIVE" && Get(db,"RACING")=="WIN","restart retains original blobs");
    Check(sqlite3_close(db)==SQLITE_OK,"final close");
    printf("SUMMARY %d/%d PASS\n",passed,passed);
    return 0;
}

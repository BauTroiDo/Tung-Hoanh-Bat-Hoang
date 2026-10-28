#include <sqlite3.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <thread>
#include <atomic>
#include <chrono>
#include <string>
#include "RoleSaveVersionedReceipt.h"
#pragma pack(push,1)
struct Role {int gold,bank,item,tick;};
#pragma pack(pop)
static int passes=0;
static void Check(bool ok,const char* name) {
    printf("%s %s\n",ok?"PASS":"FAIL",name);
    if(!ok)exit(5);
    ++passes;
}
static bool Exec(sqlite3* d,const char* s) {
    return sqlite3_exec(d,s,NULL,NULL,NULL)==SQLITE_OK;
}
static bool Update(sqlite3* d,const char* name,const Role& role) {
    sqlite3_stmt* s=NULL;
    if(sqlite3_prepare_v2(d,"UPDATE role_Blob SET StudentBlob=? WHERE roleName=?",-1,&s,NULL)!=SQLITE_OK)
        return false;
    const bool bound=
        sqlite3_bind_blob(s,1,&role,sizeof(role),SQLITE_TRANSIENT)==SQLITE_OK &&
        sqlite3_bind_text(s,2,name,-1,SQLITE_TRANSIENT)==SQLITE_OK;
    const int step=bound?sqlite3_step(s):SQLITE_ERROR;
    const int count=sqlite3_changes(d);
    sqlite3_finalize(s);
    return step==SQLITE_DONE && count==1;
}
static Role Read(sqlite3* d,const char* name) {
    Role role={-1,-1,-1,-1};
    sqlite3_stmt* s=NULL;
    if(sqlite3_prepare_v2(d,"SELECT StudentBlob FROM role_Blob WHERE roleName=?",-1,&s,NULL)!=SQLITE_OK)
        return role;
    sqlite3_bind_text(s,1,name,-1,SQLITE_TRANSIENT);
    if(sqlite3_step(s)==SQLITE_ROW &&
       sqlite3_column_bytes(s,0)==sizeof(role))
        memcpy(&role,sqlite3_column_blob(s,0),sizeof(role));
    sqlite3_finalize(s);
    return role;
}
static bool Fence(sqlite3* d,const char* name,int tx) {
    sqlite3_stmt* s=NULL;
    if(sqlite3_prepare_v2(d,
        "INSERT INTO trade_role_fence(role_name,last_tx_id) VALUES(?,?) "
        "ON CONFLICT(role_name) DO UPDATE SET last_tx_id=excluded.last_tx_id",
        -1,&s,NULL)!=SQLITE_OK)return false;
    const bool bound=sqlite3_bind_text(s,1,name,-1,SQLITE_TRANSIENT)==SQLITE_OK &&
         sqlite3_bind_int(s,2,tx)==SQLITE_OK;
    const int step=bound?sqlite3_step(s):SQLITE_ERROR;
    sqlite3_finalize(s);
    return step==SQLITE_DONE;
}
static bool Trade(sqlite3* d,const Role& a,const Role& b,int tx,
                  std::atomic<bool>* locked=NULL,unsigned int holdMs=0) {
    if(!Exec(d,"BEGIN IMMEDIATE TRANSACTION"))return false;
    const bool prepared=Update(d,"QA_A",a) && Update(d,"QA_B",b) &&
        JXRoleSaveRevision::Advance(d,"QA_A") &&
        JXRoleSaveRevision::Advance(d,"QA_B") &&
        Fence(d,"QA_A",tx) && Fence(d,"QA_B",tx);
    if(!prepared) {Exec(d,"ROLLBACK");return false;}
    if(locked)locked->store(true);
    if(holdMs)std::this_thread::sleep_for(std::chrono::milliseconds(holdMs));
    return Exec(d,"COMMIT TRANSACTION");
}
static int Receipts(sqlite3* d) {
    sqlite3_stmt* s=NULL;
    sqlite3_prepare_v2(d,"SELECT COUNT(*) FROM role_save_receipts WHERE role_name='QA_A'",-1,&s,NULL);
    int n=-1;
    if(sqlite3_step(s)==SQLITE_ROW)n=sqlite3_column_int(s,0);
    sqlite3_finalize(s);
    return n;
}
static sqlite3_int64 Rev(sqlite3* d,const char* role) {
    sqlite3_int64 n=-1;
    return JXRoleSaveRevision::Get(d,role,&n)==1?n:-1;
}
static bool Assets(const Role& a,const Role& b) {
    return a.gold+b.gold==300 && a.bank+b.bank==50 &&
           a.item!=b.item &&
           ((a.item==111 && b.item==222)||(a.item==222 && b.item==111));
}
int main(int argc,char**argv) {
    Check(argc==2,"disposable SQLite path");
    sqlite3* a=NULL;sqlite3* b=NULL;sqlite3* c=NULL;
    Check(sqlite3_open(argv[1],&a)==SQLITE_OK,"open DB");
    sqlite3_busy_timeout(a,4000);
    Check(Exec(a,"PRAGMA journal_mode=WAL"),"enable WAL");
    Check(Exec(a,"CREATE TABLE role_Blob(ID INTEGER PRIMARY KEY,roleName TEXT UNIQUE,StudentBlob BLOB,ekey INTEGER)"),"create roles");
    Check(Exec(a,"CREATE TABLE trade_role_fence(role_name TEXT PRIMARY KEY,last_tx_id INTEGER)"),"create fences");
    Check(JXRoleSaveReceipt::Ensure(a),"create revision and receipt schema");
    Role ra={100,20,111,0},rb={200,30,222,0};
    Check(Exec(a,"INSERT INTO role_Blob VALUES(1,'QA_A',x'00000000000000000000000000000000',10),"
                 "(2,'QA_B',x'00000000000000000000000000000000',20)"),"seed two characters");
    Check(Update(a,"QA_A",ra) && Update(a,"QA_B",rb),"seed assets and inventory");
    Check(Assets(Read(a,"QA_A"),Read(a,"QA_B")),"initial conservation");
    Check(sqlite3_open(argv[1],&b)==SQLITE_OK,"second DB connection");
    sqlite3_busy_timeout(b,4000);
    Role afterA={70,20,222,1},afterB={230,30,111,1};
    std::atomic<bool> tradeLocked{false};
    std::atomic<bool> tradeDone{false};
    std::thread trade([&]{tradeDone.store(Trade(a,afterA,afterB,1001,&tradeLocked,180));});
    while(!tradeLocked.load())std::this_thread::yield();
    Role stale={100,20,111,9};
    sqlite3_int64 next=-1;
    const int staleResult=JXRoleSaveReceipt::Save(b,"QA_A",&stale,sizeof(stale),0,70001,&next);
    trade.join();
    Check(tradeDone.load(),"two-role atomic trade committed");
    Check(staleResult==-17,"late pre-trade save loses CAS race");
    Check(Rev(a,"QA_A")==1 && Rev(a,"QA_B")==1,"both trade revisions advanced");
    Check(Assets(Read(a,"QA_A"),Read(a,"QA_B")),"assets preserved after late save");
    Check(Read(a,"QA_A").gold==70 && Read(a,"QA_B").gold==230,"gold conserved after trade");
    Role progress={70,20,222,42};
    Check(JXRoleSaveReceipt::Save(a,"QA_A",&progress,sizeof(progress),1,70002,&next)==1 &&
          next==2,"ordinary progression after trade saves");
    Check(JXRoleSaveReceipt::Save(b,"QA_A",&progress,sizeof(progress),1,70002,&next)==1 &&
          next==2,"lost ACK retry idempotent on another connection");
    Check(Receipts(a)==1,"lost ACK replay makes no duplicate receipt");
    Role secondA={65,20,111,43},secondB={235,30,222,2};
    Check(Trade(a,secondA,secondB,1002),"second two-role trade committed");
    Check(Rev(a,"QA_A")==3 && Rev(a,"QA_B")==2,"version advance after second trade");
    Check(JXRoleSaveReceipt::Save(b,"QA_A",&progress,sizeof(progress),1,70002,&next)==-17,
          "old committed ACK cannot rewind after later trade");
    Check(Assets(Read(a,"QA_A"),Read(a,"QA_B")),"item ownership conserved after second trade");
    Role postTrade={65,20,111,44};
    Check(JXRoleSaveReceipt::Save(a,"QA_A",&postTrade,sizeof(postTrade),3,70003,&next)==1 &&
          next==4,"posttrade progression persisted");
    Check(sqlite3_open(argv[1],&c)==SQLITE_OK,"third competing connection");
    sqlite3_busy_timeout(c,4000);
    Role competitor1={65,20,111,45}, competitor2={65,20,111,46};
    std::atomic<bool> go{false};
    int one=-99,two=-99;
    std::thread w1([&]{while(!go.load())std::this_thread::yield();
        sqlite3_int64 rev=-1;
        one=JXRoleSaveReceipt::Save(b,"QA_A",&competitor1,sizeof(competitor1),4,70004,&rev);});
    std::thread w2([&]{while(!go.load())std::this_thread::yield();
        sqlite3_int64 rev=-1;
        two=JXRoleSaveReceipt::Save(c,"QA_A",&competitor2,sizeof(competitor2),4,70005,&rev);});
    go.store(true);
    w1.join();w2.join();
    Check((one==1 && two==-17)||(two==1 && one==-17),
          "two competing revisions produce one winner");
    Check(Rev(a,"QA_A")==5,"one CAS revision advance only");
    Check(Receipts(a)==3,"three distinct ordinary-save receipts");
    Check(Assets(Read(a,"QA_A"),Read(a,"QA_B")),"no item or gold duplicated by competing saves");
    Check(Exec(a,"PRAGMA integrity_check"),"SQLite integrity check");
    Check(sqlite3_close(c)==SQLITE_OK,"close competitor");
    Check(sqlite3_close(b)==SQLITE_OK,"close second connection");
    Check(sqlite3_close(a)==SQLITE_OK,"close writer");
    Check(sqlite3_open(argv[1],&a)==SQLITE_OK,"reopen after simulated restart");
    Check(Rev(a,"QA_A")==5 && Rev(a,"QA_B")==2,"versions persistent across restart");
    Check(Receipts(a)==3,"receipt count persistent across restart");
    Check(Assets(Read(a,"QA_A"),Read(a,"QA_B")),"assets conserved across restart");
    Check(sqlite3_close(a)==SQLITE_OK,"final close");
    printf("SUMMARY %d/%d PASS\n",passes,passes);
    return 0;
}

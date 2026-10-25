#include <cstdio>
#include <string>
#include <vector>
#include <sqlite3.h>
#include "RoleSaveRevisionCAS.h"
static int passed=0;
static void Check(bool b,const char* why) {
    if (!b) {printf("FAIL %s\n",why);fflush(stdout);exit(4);}
    ++passed;printf("PASS %s\n",why);
}
static bool SQL(sqlite3* db,const char* script) {
    return sqlite3_exec(db,script,NULL,NULL,NULL)==SQLITE_OK;
}
static std::string Bytes(const std::vector<unsigned char>& v) {
    return std::string((const char*)v.data(),v.size());
}
int main(int argc,char**argv) {
    Check(argc==2,"fixture path supplied");
    sqlite3* db=NULL;
    Check(sqlite3_open(argv[1],&db)==SQLITE_OK,"open fresh DB");
    Check(SQL(db,"CREATE TABLE role_Blob(roleName TEXT,StudentBlob BLOB,ekey INTEGER)"),"role schema");
    Check(SQL(db,"CREATE TABLE trade_role_fence(role_name TEXT PRIMARY KEY,last_tx_id INTEGER)"),"trade fence schema");
    Check(SQL(db,"INSERT INTO role_Blob VALUES('QA_A',x'4130',7),('QA_B',x'4230',8),('QA_LEGACY',x'4C30',9)"),"seed roles");
    Check(SQL(db,"INSERT INTO trade_role_fence VALUES('QA_LEGACY',500)"),"seed fenced legacy role");
    sqlite3_int64 rev=999;
    std::vector<unsigned char> blob;
    Check(JXRoleSaveRevision::LoadSnapshot(db,"QA_A",&blob,&rev)==1 && rev==0 &&
          Bytes(blob)=="A0","snapshot of new role revision zero");
    Check(JXRoleSaveRevision::LoadSnapshot(db,"QA_LEGACY",&blob,&rev)==-17 &&
          rev==0 && Bytes(blob)=="A0","old fenced role refuses missing revision and preserves result");
    Check(JXRoleSaveRevision::LoadSnapshot(db,"NO_ROLE",&blob,&rev)==-14 &&
          rev==0 && Bytes(blob)=="A0","missing role no overwrite");
    Check(JXRoleSaveRevision::Save(db,"QA_A","A1",2,0,&rev)==1 && rev==1,"CAS save");
    Check(JXRoleSaveRevision::LoadSnapshot(db,"QA_A",&blob,&rev)==1 &&
          Bytes(blob)=="A1" && rev==1,"atomic read after ordinary save");
    Check(SQL(db,"BEGIN IMMEDIATE TRANSACTION"),"begin two-role trade");
    Check(SQL(db,"UPDATE role_Blob SET StudentBlob=x'4132' WHERE roleName='QA_A'"),"trade role A");
    Check(SQL(db,"UPDATE role_Blob SET StudentBlob=x'4232' WHERE roleName='QA_B'"),"trade role B");
    Check(JXRoleSaveRevision::Advance(db,"QA_A"),"trade advance role A");
    Check(JXRoleSaveRevision::Advance(db,"QA_B"),"trade advance role B");
    Check(SQL(db,"COMMIT TRANSACTION"),"trade commit");
    Check(JXRoleSaveRevision::LoadSnapshot(db,"QA_A",&blob,&rev)==1 &&
          Bytes(blob)=="A2" && rev==2,"post-trade snapshot revision consistent");
    Check(JXRoleSaveRevision::Save(db,"QA_A","OLD",3,1,&rev)==-17 &&
          Bytes(blob)=="A2","stale after trade fails");
    Check(JXRoleSaveRevision::Save(db,"QA_A","A3",2,2,&rev)==1 &&
          rev==3,"progress saves after trade");
    Check(JXRoleSaveRevision::LoadSnapshot(db,"QA_A",&blob,&rev)==1 &&
          Bytes(blob)=="A3" && rev==3,"progress snapshot");
    Check(JXRoleSaveRevision::LoadSnapshot(db,"QA_B",&blob,&rev)==1 &&
          Bytes(blob)=="B2" && rev==1,"counterpart snapshot");
    Check(SQL(db,"INSERT INTO role_Blob VALUES('QA_B',x'4278',8)"),"duplicate names fixture");
    rev=100;
    Check(JXRoleSaveRevision::LoadSnapshot(db,"QA_B",&blob,&rev)==-14 &&
          rev==100 && Bytes(blob)=="B2","duplicate role names rejected with no partial output");
    Check(sqlite3_close(db)==SQLITE_OK,"close DB");
    Check(sqlite3_open(argv[1],&db)==SQLITE_OK,"reopen DB");
    Check(JXRoleSaveRevision::LoadSnapshot(db,"QA_A",&blob,&rev)==1 &&
          Bytes(blob)=="A3" && rev==3,"relog loads matching persistent blob and revision");
    Check(sqlite3_close(db)==SQLITE_OK,"final close");
    printf("SUMMARY %d/%d PASS\n",passed,passed);
    return 0;
}

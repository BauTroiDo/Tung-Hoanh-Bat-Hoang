#include <cstdio>
#include <cstring>
#include <stdint.h>
#include <vector>
#include <string>
#include "RoleSaveRevisionWire.h"

static int passed=0, failed=0;
static void Check(bool good, const char* name) {
    if (!good) {
        ++failed;
        printf("FAIL %s\n", name);
    } else {
        ++passed;
        printf("PASS %s\n", name);
    }
}
int main() {
    using namespace JXRoleRevisionWire;
    unsigned char trailer[kTrailerBytes] = {0};
    Check(kTrailerBytes==24,"ABI length 24");
    Check(Encode(trailer,sizeof(trailer),"QA_A",0),"new role encode rev0");
    Check(memcmp(trailer,"JXREV001",8)==0,"magic and version");
    Check(ReadU64(trailer+8)==0,"little endian zero");
    uint64_t got=777;
    std::vector<unsigned char> payload(64+sizeof(trailer),0xA5);
    memcpy(payload.data()+64,trailer,sizeof(trailer));
    Check(Decode(payload.data(),payload.size(),64,"QA_A",&got)==Valid && got==0,"decode revision zero");
    got=999;
    Check(Decode(payload.data(),64,64,"QA_A",&got)==Absent && got==999,"absent is NOT zero");
    Check(Decode(payload.data(),64-1,64,"QA_A",&got)==Invalid,"short blob");
    Check(Decode(NULL,payload.size(),64,"QA_A",&got)==Invalid,"null payload");
    Check(Decode(payload.data(),payload.size(),0,"QA_A",&got)==Invalid,"zero-length role blob");
    Check(Decode(payload.data(),payload.size(),64,"QA_B",&got)==Invalid,"role mismatch");
    Check(Decode(payload.data(),payload.size()+1,64,"QA_A",&got)==Invalid,"extra suffix");
    Check(Decode(payload.data(),payload.size()-1,64,"QA_A",&got)==Invalid,"truncated suffix");
    Check(!Encode(trailer,23,"QA_A",1),"short output");
    Check(!Encode(NULL,24,"QA_A",1),"null output");
    Check(!Encode(trailer,24,"",1),"empty role");
    Check(!Encode(trailer,24,"12345678901234567890123456789012",1),"unterminated 32-name");
    Check(!Encode(trailer,24,"QA_A",UINT64_MAX),"overflow revision");
    Check(Encode(trailer,24,"QA_A",INT64_MAX),"max valid revision");
    memcpy(payload.data()+64,trailer,24);
    Check(Decode(payload.data(),payload.size(),64,"QA_A",&got)==Valid && got==INT64_MAX,"max valid decode");
    payload[64+8]=0xff;
    payload[64+15]=0xff;
    Check(Decode(payload.data(),payload.size(),64,"QA_A",&got)==Invalid,"tamper revision");
    Check(Encode(trailer,24,"QA_A",30),"reencode after tamper");
    memcpy(payload.data()+64,trailer,24);
    payload[64+21]^=0x4;
    Check(Decode(payload.data(),payload.size(),64,"QA_A",&got)==Invalid,"tamper checksum");
    Check(Encode(trailer,24,"QA_A",30),"reencode for name tamper");
    memcpy(payload.data()+64,trailer,24);
    payload[64+16]^=0x2;
    Check(Decode(payload.data(),payload.size(),64,"QA_A",&got)==Invalid,"tamper name digest");
    Check(Encode(trailer,24,"QA_A",30),"reencode for magic tamper");
    memcpy(payload.data()+64,trailer,24);
    payload[64]^=0x5;
    Check(Decode(payload.data(),payload.size(),64,"QA_A",&got)==Invalid,"tamper magic");
    unsigned char utfName[]={0x54,0xE1,0xBB,0x95,0x00};
    Check(Encode(trailer,24,(const char*)utfName,65536),"UTF8 role name encode");
    memcpy(payload.data()+64,trailer,24);
    Check(Decode(payload.data(),payload.size(),64,(const char*)utfName,&got)==Valid && got==65536,"UTF8 role name decode");
    bool series=true;
    for (uint64_t n=0;n!=10000;++n) {
        unsigned char buf[24];
        uint64_t out=0;
        if (!Encode(buf,24,"QA_A",n)) {series=false;break;}
        memcpy(payload.data()+64,buf,24);
        if (Decode(payload.data(),payload.size(),64,"QA_A",&out)!=Valid || out!=n) {series=false;break;}
    }
    Check(series,"10K revision round-trips");
    printf("SUMMARY %d/%d PASS\n",passed,passed+failed);
    return failed?1:0;
}

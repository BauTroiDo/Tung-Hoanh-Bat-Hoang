#include <cstdio>
#include <cstring>
#include <stdint.h>
#include <vector>
#include "RoleSaveRevisionWire.h"
static int n=0;
static void Check(bool ok,const char* what) {
    printf("%s %s\n",ok?"PASS":"FAIL",what);
    if(!ok) exit(4);
    ++n;
}
int main() {
    using namespace JXRoleRevisionWire;
    std::vector<unsigned char> role(128+kSaveBytes,0x7e);
    unsigned char* tail=role.data()+128;
    uint64_t rev=999,nonce=777;
    Check(kTrailerBytes==24 && kSaveBytes==36,"login trailer 24 save extension 36");
    Check(EncodeSave(tail,kSaveBytes,"QA_A",0,1),"encode zero revision");
    Check(DecodeSave(role.data(),role.size(),128,"QA_A",&rev,&nonce)==Valid &&
          rev==0 && nonce==1,"decode zero revision nonce");
    Check(DecodeSave(role.data(),role.size(),128,"QA_B",&rev,&nonce)==Invalid,"wrong role");
    Check(DecodeSave(role.data(),128,128,"QA_A",&rev,&nonce)==Invalid,"legacy save is not revision zero");
    Check(DecodeSave(role.data(),role.size()-1,128,"QA_A",&rev,&nonce)==Invalid,"short trailer rejected");
    Check(DecodeSave(role.data(),role.size()+1,128,"QA_A",&rev,&nonce)==Invalid,"extra bytes rejected");
    Check(!EncodeSave(tail,kSaveBytes,"QA_A",3,0),"nonce zero forbidden");
    Check(!EncodeSave(tail,kSaveBytes-1,"QA_A",3,2),"small output rejected");
    Check(!EncodeSave(tail,kSaveBytes,"",3,2),"empty role rejected");
    Check(!EncodeSave(tail,kSaveBytes,"QA_A",UINT64_MAX,2),"revision overflow rejected");
    Check(EncodeSave(tail,kSaveBytes,"QA_A",UINT64_MAX/2,UINT64_MAX),"large values encoded");
    Check(DecodeSave(role.data(),role.size(),128,"QA_A",&rev,&nonce)==Valid &&
          rev==UINT64_MAX/2 && nonce==UINT64_MAX,"large values decoded");
    tail[24]^=1;
    Check(DecodeSave(role.data(),role.size(),128,"QA_A",&rev,&nonce)==Invalid,"tampered nonce rejected");
    Check(EncodeSave(tail,kSaveBytes,"QA_A",35,123456),"restore valid trailer");
    tail[32]^=1;
    Check(DecodeSave(role.data(),role.size(),128,"QA_A",&rev,&nonce)==Invalid,"checksum corruption rejected");
    Check(EncodeSave(tail,kSaveBytes,"QA_A",35,123456),"prepare checksum test");
    tail[8]^=1;
    Check(DecodeSave(role.data(),role.size(),128,"QA_A",&rev,&nonce)==Invalid,"revision corruption rejected");
    bool good=true;
    for(uint64_t i=1;i<=10000;i++) {
        if(!EncodeSave(tail,kSaveBytes,"QA_A",i,i*41)) {good=false;break;}
        uint64_t got=0,gotNonce=0;
        if(DecodeSave(role.data(),role.size(),128,"QA_A",&got,&gotNonce)!=Valid ||
           got!=i || gotNonce!=i*41) {good=false;break;}
    }
    Check(good,"10K save nonce/revision roundtrips");
    printf("SUMMARY %d/%d PASS\n",n,n);
    return 0;
}

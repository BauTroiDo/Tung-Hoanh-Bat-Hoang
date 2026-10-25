#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <windows.h>
#include <objbase.h>
#include <initguid.h>
#include "IClient.h"
#include "RoleSaveRevisionWire.h"
#include <atomic>
#include <cstdio>
#include <cstring>
#include <vector>
#include <fstream>
#include <cstdint>
#include <cstddef>

#pragma pack(push,1)
struct RoleSaveRequest {
    unsigned char opcode;
    uint32_t dataLen;
    uint32_t identity;
    bool leave;
    uint32_t server;
    unsigned char data[1];
};
#pragma pack(pop)
static_assert(offsetof(RoleSaveRequest,data)==14,"TProcessData ABI header");
enum : unsigned char { SAVE_OPCODE=10, SAVE_ACK_OPCODE=56 };
std::atomic<int> online{0};
static void CALLBACK Notice(LPVOID,const unsigned long& event) {
    if(event==0x100) online.store(1);
}
using Factory=HRESULT (__stdcall *)(REFIID,void**);
static std::vector<unsigned char> Load(const char* file) {
    std::ifstream s(file,std::ios::binary|std::ios::ate);
    if(!s)return {};
    auto n=(size_t)s.tellg();
    s.seekg(0);
    std::vector<unsigned char> result(n);
    s.read((char*)result.data(),n);
    return s?result:std::vector<unsigned char>();
}
static bool Send(IClient* cl,const std::vector<unsigned char>& payload) {
    size_t offset=0;
    while(offset<payload.size()){
        const size_t chunk=payload.size()-offset>510?510:payload.size()-offset;
        unsigned char header[6]={SAVE_OPCODE,
            (unsigned char)((offset==0?0xC0:0x30)|((offset+chunk==payload.size())?0x0C:0)),0,0,0,0};
        std::vector<unsigned char> frame;
        frame.insert(frame.end(),header,header+6);
        frame.insert(frame.end(),payload.begin()+offset,payload.begin()+offset+chunk);
        if(FAILED(cl->SendPackToServer(frame.data(),frame.size())))return false;
        offset+=chunk;
    }
    return true;
}
int main(int argc,char**argv){
    setvbuf(stdout,nullptr,_IONBF,0);
    const bool replay=(argc>1 && strcmp(argv[1],"reconnect")==0);
    const auto base=Load("post2_A_rolevalid.bin");
    if(base.size()<570)return 2;
    auto make=[&](int money,uint64_t revision,uint64_t nonce) {
        auto blob=base;
        memcpy(blob.data()+432,&money,4);
        std::vector<unsigned char> result(offsetof(RoleSaveRequest,data)+blob.size()+36,0);
        auto p=(RoleSaveRequest*)result.data();
        p->opcode=SAVE_OPCODE;p->identity=7;p->server=1;
        p->dataLen=(uint32_t)result.size();p->leave=false;
        memcpy(result.data()+offsetof(RoleSaveRequest,data),blob.data(),blob.size());
        if(!JXRoleRevisionWire::EncodeSave(result.data()+offsetof(RoleSaveRequest,data)+blob.size(),36,
               "QA_A",revision,nonce)) return std::vector<unsigned char>();
        return result;
    };
    HMODULE dll=LoadLibraryA("Rainbow.dll");
    if(!dll)return 3;
    auto factory=(Factory)GetProcAddress(dll,"CreateInterface");
    if(!factory)return 4;
    IClientFactory* f=nullptr;
    if(FAILED(factory(IID_IClientFactory,(void**)&f))||!f)return 5;
    if(FAILED(f->SetEnvironment(8,10,131072)))return 6;
    IClient* cl=nullptr;
    if(FAILED(f->CreateClientInterface(IID_IESClient,(void**)&cl))||!cl)return 7;
    f->Release();
    if(FAILED(cl->Startup()))return 8;
    cl->RegisterMsgFilter(nullptr,Notice);
    if(FAILED(cl->FsGameServerConnectTo("127.0.0.1",15001)))return 9;
    for(int i=0;i<40&&!online.load();++i)Sleep(60);
    if(!online.load())return 10;
    int passed=0;
    auto verify=[&](const char* name,int money,uint64_t expected,uint64_t nonce,
                    int expectedStatus,uint64_t next){
        auto request=make(money,expected,nonce);
        if(request.empty() || !Send(cl,request))return false;
        bool success=false;
        for(int i=0;i<80;i++){
            size_t bytes=0;
            const void* raw=cl->GetPackFromServer(bytes);
            if(raw && bytes>=39){
                const unsigned char* b=(const unsigned char*)raw;
                if(b[0]!=SAVE_ACK_OPCODE)continue;
                uint32_t marker=0,identity=0;
                memcpy(&marker,b+1,4);memcpy(&identity,b+5,4);
                const int status=(int)(signed char)b[14];
                const uint64_t ackExpected=JXRoleRevisionWire::ReadU64(b+15);
                const uint64_t ackNext=JXRoleRevisionWire::ReadU64(b+23);
                const uint64_t ackNonce=JXRoleRevisionWire::ReadU64(b+31);
                success=bytes==39 && marker==791031 && identity==7 &&
                    status==expectedStatus && ackExpected==expected &&
                    ackNext==next && ackNonce==nonce;
                printf("%s EXPECT_STATUS=%d ACTUAL=%d EXPECT_REV=%llu NEXT=%llu NONCE=%llu VALID=%d\n",
                    name,expectedStatus,status,(unsigned long long)ackExpected,
                    (unsigned long long)ackNext,(unsigned long long)ackNonce,success?1:0);
                break;
            }
            Sleep(70);
        }
        if(success)++passed;
        return success;
    };
    bool ok;
    if(!replay){
        ok=verify("POST_TRADE_PROGRESS",71,2,500001,1,3) &&
           verify("ACK_LOSS_REPLAY",71,2,500001,1,3) &&
           verify("NONCE_COLLISION",72,2,500001,-13,0) &&
           verify("STALE_DIFFERENT_NONCE",71,2,500002,-17,0) &&
           verify("NEXT_PROGRESSION",72,3,500003,1,4);
    } else {
        ok=verify("RESTART_REPLAY",72,3,500003,1,4) &&
           verify("OLD_RECEIPT_STALE",71,2,500001,-17,0);
    }
    printf("VERSIONED_SAVE_NETWORK=%d PASS_COUNT=%d\n",ok?1:0,passed);
    cl->Shutdown();cl->Cleanup();cl->Release();
    return ok?0:12;
}

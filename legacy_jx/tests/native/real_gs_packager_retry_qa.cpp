#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <vector>
#include "Buffer.h"
#include "RoleSaveRevisionWire.h"
#include "RoleSaveRetryCache.h"

static int pass=0;
static void Check(bool yes, const char* what) {
    printf("%s %s\n",yes?"PASS":"FAIL",what);
    if(!yes) std::exit(3);
    ++pass;
}
#pragma pack(push,1)
struct WireSave {
    unsigned char opcode;
    uint32_t dataLen;
    uint32_t identity;
    bool leave;
    uint32_t gs;
    unsigned char blob[1];
};
#pragma pack(pop)
static_assert(offsetof(WireSave,blob)==14,"legacy header must be packed");
static void Probe(size_t payloadLen) {
    const unsigned char code=10;
    std::vector<unsigned char> packed(payloadLen);
    for(size_t i=0;i<packed.size();++i)
        packed[i]=(unsigned char)((i*31 + payloadLen*7)&255);
    CPackager source(128*1024,32);
    size_t firstPart=payloadLen>14?14:payloadLen;
    source.AddData(code,packed.data(),firstPart);
    if(firstPart<payloadLen)
        source.AddData(code,packed.data()+firstPart,payloadLen-firstPart);
    std::vector<std::vector<unsigned char> > frames;
    CBuffer* head=source.GetHeadPack(code);
    Check(head!=NULL,"source packager created first frame");
    for(CBuffer* frame=head; frame;) {
        const unsigned char* raw=frame->GetBuffer();
        const size_t length=frame->GetUsed();
        Check(length>=6 && length<=516,"frame length within Rainbow protocol");
        Check(raw[0]==code,"frame opcode unchanged");
        Check((raw[1]&3)==0,"frame flags have no error bits");
        if(frames.empty())Check((raw[1]&0xC0)==0xC0,"first frame flagged header");
        else Check((raw[1]&0x30)==0x30,"later frame flagged continuation");
        Check(raw[2]==0 && raw[3]==0 && raw[4]==0 && raw[5]==0,
              "GS user data header is zero");
        frames.push_back(std::vector<unsigned char>(raw,raw+length));
        frame->Release();
        frame=source.GetNextPack(code);
    }
    Check((frames.back()[1]&0x0c)==0x0c,"final frame flagged tail");
    std::vector<unsigned char> reassembled;
    for(size_t i=0;i<frames.size();++i)
        reassembled.insert(reassembled.end(),frames[i].begin()+6,frames[i].end());
    Check(reassembled==packed,"GameServer packager reassembles byte-identical data");
    Check(frames.size()==((payloadLen+509)/510),"frame count expected");
    // Cache the same real frame bytes as Stage7 and confirm deep copy.
    JXRoleSaveRetry::Cache cache;
    Check(cache.Store(7,"QA_A",2,500001,1000,frames),"real frames cached");
    Check(cache.Find(7)->frames==frames,"initial bytes intact");
    if(!frames.empty())frames[0][6]^=0xff;
    Check(cache.Find(7)->frames!=frames,"cached frames immutable");
    Check(cache.RecordRetry(7,6000),"real-frame retry tracked");
    Check(cache.Find(7)->frames.size()==((payloadLen+509)/510),
          "replay sends same frame count");
    Check(cache.RemoveIfMatch(7,"QA_A",2,500001),"matching ACK removes real packets");
}
int main() {
    Probe(14);
    Probe(510);
    Probe(511);
    Probe(754);
    Probe(1019);
    Probe(4096);
    Probe(128*1024);
    printf("SUMMARY %d/%d PASS\n",pass,pass);
}

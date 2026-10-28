#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>
#include <climits>
#include "RoleSaveRetryCache.h"
static int good=0;
static void Check(bool pass,const char* name) {
    printf("%s %s\n",pass?"PASS":"FAIL",name);
    if(!pass) exit(4);
    ++good;
}
int main() {
    using namespace JXRoleSaveRetry;
    Cache journal;
    std::vector<std::vector<unsigned char> > frames;
    frames.push_back(std::vector<unsigned char>{10,0xc0,1,2,3});
    frames.push_back(std::vector<unsigned char>{10,0x3c,4,5,6});
    Check(journal.Store(7,"QA_A",2,500001,1000,frames),"store snapshot frames");
    Check(journal.Find(7) && journal.Find(7)->frames.size()==2,"two transport frames");
    Check(journal.Find(7)->frames[0]==frames[0],"first frame identical");
    Check(journal.Find(7)->frames[1]==frames[1],"second frame identical");
    frames[0][2]=99;
    Check(journal.Find(7)->frames[0][2]==1,"retry frames deep copied, immutable to source");
    Check(journal.Matches(7,"QA_A",2,500001),"correct session token");
    Check(!journal.Matches(7,"QA_B",2,500001),"different role rejected");
    Check(!journal.Matches(7,"QA_A",3,500001),"different revision rejected");
    Check(!journal.Matches(7,"QA_A",2,500002),"different nonce rejected");
    Check(!journal.Matches(8,"QA_A",2,500001),"different slot rejected");
    Check(!journal.Find(7)->Due(5999,5000),"before timeout do not retry");
    Check(journal.Find(7)->Due(6000,5000),"at timeout retry eligible");
    Check(journal.RecordRetry(7,6000),"record first retransmission");
    Check(journal.Find(7)->retries==1,"retry count exactly one");
    Check(!journal.Find(7)->Due(6500,5000),"retry interval resets");
    Check(journal.Find(7)->Due(11000,5000),"second timeout eligible");
    Check(journal.RecordRetry(7,11000) && journal.RecordRetry(7,16000),
          "count bounded retries");
    Check(journal.Find(7)->retries==3,"three retries tracked");
    Check(journal.Find(7)->frames[0][2]==1,"repeated frames still identical");
    Check(!journal.RemoveIfMatch(7,"QA_A",2,500002),"wrong ACK nonce never frees retry");
    Check(journal.Find(7)!=NULL,"retry retained after wrong ACK");
    Check(!journal.RemoveIfMatch(7,"QA_B",2,500001),"wrong role ACK never frees");
    Check(journal.RemoveIfMatch(7,"QA_A",2,500001),"matching ACK removes pending");
    Check(journal.Find(7)==NULL,"duplicate ACK cannot find pending");
    Check(!journal.RecordRetry(7,20000),"cannot retry after accepted ACK");
    Check(!journal.Store(0,"QA_A",2,123,1000,frames),"invalid player slot rejected");
    Check(!journal.Store(7,"",2,123,1000,frames),"empty role rejected");
    Check(!journal.Store(7,std::string(32,'x'),2,123,1000,frames),
          "oversized role name rejected");
    Check(!journal.Store(7,"QA_A",2,0,1000,frames),"zero nonce rejected");
    Check(!journal.Store(7,"QA_A",UINT64_MAX,1,1000,frames),
          "revision overflow rejected");
    std::vector<std::vector<unsigned char> > none;
    Check(!journal.Store(7,"QA_A",2,1,1000,none),"empty frames rejected");
    std::vector<std::vector<unsigned char> > oversized;
    oversized.push_back(std::vector<unsigned char>(1025,0x11));
    Check(!journal.Store(7,"QA_A",2,1,1000,oversized),"oversized network frame rejected");
    std::vector<std::vector<unsigned char> > emptyFrame;
    emptyFrame.push_back(std::vector<unsigned char>());
    Check(!journal.Store(7,"QA_A",2,1,1000,emptyFrame),"zero length frame rejected");
    std::vector<std::vector<unsigned char> > maxFrames(257,std::vector<unsigned char>(1024,0x3c));
    Check(!journal.Store(7,"QA_A",2,1,1000,maxFrames),"memory budget enforced");
    Check(journal.Store(7,"QA_A",2,123,UINT32_MAX-100,frames),"wrap fixture");
    Check(!journal.Find(7)->Due(100,5000),"tick wrap no premature timeout");
    Check(journal.Find(7)->Due(5000,5000),"tick wrap due correctly");
    Check(journal.Store(7,"QA_B",0,321,2,frames),"recycled slot replaces old cache");
    Check(!journal.Matches(7,"QA_A",2,123),"recycled slot rejects old identity");
    Check(journal.Matches(7,"QA_B",0,321),"recycled slot tracks new role");
    journal.Clear();
    Check(journal.Entries().empty(),"clear pending on shutdown");
    printf("SUMMARY %d/%d PASS\n",good,good);
    return 0;
}

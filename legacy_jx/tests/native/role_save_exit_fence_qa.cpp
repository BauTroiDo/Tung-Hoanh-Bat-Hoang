#include <cstdio>
#include <cstdlib>
#include "RoleSaveExitFence.h"
static int passed=0;
static void Check(bool condition,const char* name) {
    printf("%s %s\n",condition?"PASS":"FAIL",name);
    if(!condition)exit(7);
    ++passed;
}
int main() {
    using JXRoleSaveExitFence::BlocksRelease;
    Check(!BlocksRelease(false,false,false),"legacy no-version role can use legacy lifecycle");
    Check(!BlocksRelease(false,true,false),"legacy save unchanged");
    Check(!BlocksRelease(false,false,true),"non-versioned semantics left unchanged");
    Check(!BlocksRelease(false,true,true),"non-versioned pending does not add version policy");
    Check(!BlocksRelease(true,true,false),"verified versioned role can release after ACK");
    Check(BlocksRelease(true,true,true),"pending ACK blocks leave/unlock");
    Check(BlocksRelease(true,false,true),"trade race invalidated revision but ACK pending");
    Check(BlocksRelease(true,false,false),"negative ACK blocks leave/unlock");
    bool protocol=true, loaded=true, pending=true;
    Check(BlocksRelease(protocol,loaded,pending),"before first save ACK");
    pending=false; loaded=true;
    Check(!BlocksRelease(protocol,loaded,pending),"after successful CAS ACK");
    pending=true; loaded=false;
    Check(BlocksRelease(protocol,loaded,pending),"trade commits while save pending");
    pending=false;
    Check(BlocksRelease(protocol,loaded,pending),"retry exhausted and no authoritative token");
    loaded=true;
    Check(!BlocksRelease(protocol,loaded,pending),"fresh authoritative re-login permits release");
    printf("SUMMARY %d/%d PASS\n",passed,passed);
    return 0;
}

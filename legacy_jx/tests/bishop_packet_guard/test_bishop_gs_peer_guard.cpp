#include "BishopGsPeerGuard.h"
#include <cstdio>
#include <cstring>
#include <string>

#define CHECK(test) do { if (!(test)) { std::fprintf(stderr, "FAILED at line %d: %s\n", __LINE__, #test); return 1; } } while (0)

int main()
{
    using namespace BishopGsPeerGuard;
    CHECK(ValidateAllowlist("127.0.0.1"));
    CHECK(ValidateAllowlist("127.0.0.1,192.168.1.69"));
    CHECK(ValidateAllowlist(" 127.0.0.1 , 192.168.1.69 "));
    CHECK(PeerAllowed("127.0.0.1 : 56732", "127.0.0.1"));
    CHECK(PeerAllowed("192.168.1.69 : 43210", "127.0.0.1,192.168.1.69"));
    CHECK(PeerAllowed(" 192.168.1.69:65535 ", "127.0.0.1, 192.168.1.69"));
    CHECK(!PeerAllowed("10.0.0.1 : 43210", "127.0.0.1,192.168.1.69"));
    CHECK(!PeerAllowed("127.0.0.1 : 1234", ""));
    CHECK(!PeerAllowed("127.0.0.1 : 1234", nullptr));
    CHECK(!PeerAllowed(nullptr, "127.0.0.1"));
    CHECK(!PeerAllowed("", "127.0.0.1"));
    CHECK(!PeerAllowed("127.0.0.1 : 0", "127.0.0.1"));
    CHECK(!PeerAllowed("127.0.0.1 : 65536", "127.0.0.1"));
    CHECK(!PeerAllowed("127.0.0.1 : -1", "127.0.0.1"));
    CHECK(!PeerAllowed("127.0.0.1", "127.0.0.1"));
    CHECK(!PeerAllowed("127.0.0.1 : 22 garbage", "127.0.0.1"));
    CHECK(!PeerAllowed("127.0.0.1:4:5", "127.0.0.1"));
    CHECK(!PeerAllowed("127.0.0.2:88", "127.0.0.1"));
    CHECK(!PeerAllowed("127.000.0.1:88", "127.0.0.1"));
    CHECK(!PeerAllowed("127.0.0.256:88", "127.0.0.1"));
    CHECK(!PeerAllowed("127.0.0.1:88", "127.0.0.1,not-a-host"));
    CHECK(!PeerAllowed("127.0.0.1:88", "127.0.0.1,"));
    CHECK(!PeerAllowed("127.0.0.1:88", "*,127.0.0.1"));
    CHECK(!PeerAllowed("127.0.0.1:88", "127.0.0.0/8"));
    CHECK(!PeerAllowed("127.0.0.1:88", "127.0.0.1;192.168.1.69"));
    CHECK(!PeerAllowed("127.0.0.1:88", "127.0.0.1\n192.168.1.69"));
    CHECK(!ValidateAllowlist("0.0.0.01"));
    CHECK(!ValidateAllowlist("127.0.0.1 192.168.1.69"));
    CHECK(!ValidateAllowlist("127.0.0.1,127.0.0.01"));
    std::string many;
    for (int i=0; i<33; ++i) {
        if (!many.empty()) many += ",";
        many += "127.0.0.1";
    }
    CHECK(!ValidateAllowlist(many.c_str()));
    many.resize(many.rfind(','));
    CHECK(ValidateAllowlist(many.c_str()));
    CHECK(PeerAllowed("127.0.0.1:88", many.c_str()));
    std::puts("BISHOP_GS_TRUST_GUARD_PASS");
    return 0;
}

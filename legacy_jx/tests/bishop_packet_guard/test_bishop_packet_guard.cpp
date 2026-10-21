#include "BishopPacketGuard.h"
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <limits>
#include <cstdio>
#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s line %d\n", #c, __LINE__); return 1; } } while (0)
using namespace BishopPacketGuard;

int main()
{
    const unsigned char packet[] = {0x61,0x11,0x22,0x33,0x44,0x55};
    CHECK(!HasBytes(nullptr, 6, 1));
    CHECK(!HasBytes(packet, 0, 1));
    CHECK(!HasBytes(packet, 3, 4));
    CHECK(HasBytes(packet, 4, 4));
    CHECK(ExactBytes(packet, 6, 6));
    CHECK(!ExactBytes(packet, 5, 6));
    CHECK(!ExactBytes(nullptr, 6, 6));

    char wellFormed[] = {'a', 'b', 0, 'Z'};
    char noTerminator[] = {'a','b','c'};
    CHECK(HasNul(wellFormed));
    CHECK(!HasNul(noTerminator));
    CHECK(!HasNul(noTerminator, 0));
    CHECK(!HasNul(nullptr, 6));

    std::uint32_t out = 0;
    CHECK(ReadU32(packet, sizeof(packet), 1, out));
    std::uint32_t expected = 0;
    std::memcpy(&expected, packet + 1, sizeof(expected));
    CHECK(out == expected);
    CHECK(!ReadU32(packet, sizeof(packet), 3, out));
    CHECK(!ReadU32(nullptr, sizeof(packet), 0, out));
    CHECK(!ReadU32(packet, sizeof(packet), std::numeric_limits<std::size_t>::max(), out));
    CHECK(!ReadU32(packet, 3, 4, out));

    char safeName[] = "world_1-A";
    char traversal[] = "../secret";
    char slash[] = "folder\\bad";
    char separator[] = "a:b";
    char control[] = {'A', '\x01', 0};
    CHECK(SafeMapSuffix(safeName));
    CHECK(!SafeMapSuffix(traversal));
    CHECK(!SafeMapSuffix(slash));
    CHECK(!SafeMapSuffix(separator));
    CHECK(!SafeMapSuffix(control));
    CHECK(!SafeMapSuffix(noTerminator));
    CHECK(!SafeMapSuffix("", 1));
    CHECK(!SafeMapSuffix(nullptr, 100));
    std::puts("BISHOP_PACKET_GUARD_PASS");
    return 0;
}

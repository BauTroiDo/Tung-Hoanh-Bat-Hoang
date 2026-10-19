#include "mmo/PortableCore.h"
#include <cstdint>
#include <iostream>
#include <random>
#include <string>
#include <vector>
#define CHECK(expr) do { if(!(expr)) { std::cerr << "FAIL " << __LINE__ << " " << #expr << "\n"; return 1; } } while(false)
int main() {
    using namespace mmo::portable;
    BinaryWriter w;
    CHECK(w.writeU8(0x2a));
    CHECK(w.writeU16(0x1234));
    CHECK(w.writeU32(0x89abcdefu));
    CHECK(w.writeU64(0xfedcba9876543210ull));
    CHECK(w.writeStringU16("hi"));
    const std::vector<std::uint8_t> golden={
        0x2a,0x34,0x12,0xef,0xcd,0xab,0x89,0x10,0x32,0x54,0x76,0x98,0xba,0xdc,0xfe,0x02,0x00,0x68,0x69};
    CHECK(w.bytes()==golden);
    BinaryReader r(w.bytes().data(),w.size());
    std::uint8_t a=0;std::uint16_t b=0;std::uint32_t c=0;std::uint64_t d=0;
    std::string text;
    CHECK(r.readU8(a) && a==0x2a);
    CHECK(r.readU16(b) && b==0x1234);
    CHECK(r.readU32(c) && c==0x89abcdefu);
    CHECK(r.readU64(d) && d==0xfedcba9876543210ull);
    CHECK(r.readStringU16(text) && text=="hi" && r.remaining()==0);
    BinaryWriter cap(2);
    CHECK(cap.writeU16(0xffff) && !cap.writeU8(1) && cap.size()==2);
    CHECK(!cap.writeStringU16("ab") && cap.size()==2);
    BinaryReader shortOne(golden.data(),1);
    b=777;
    CHECK(!shortOne.readU16(b) && b==777 && shortOne.offset()==0);
    const std::uint8_t trunc[]={3,0,'a','b'};
    BinaryReader tr(trunc,sizeof(trunc));
    text="keep";
    CHECK(!tr.readStringU16(text) && text=="keep" && tr.offset()==0);
    BinaryReader empty(nullptr,0);
    CHECK(!empty.readU8(a) && empty.offset()==0);
    BinaryWriter out(10);
    CHECK(!out.writeStringU16(std::string(10,'x')) && out.size()==0);
    CHECK(!out.writeStringU16(std::string(70000,'x'),70000) && out.size()==0);
    std::mt19937 rng(12345);
    for (int i=0;i<10000;++i) {
        std::vector<std::uint8_t> raw(static_cast<std::size_t>(rng()%24));
        for (auto& n:raw) n=static_cast<std::uint8_t>(rng());
        BinaryReader fuzz(raw.empty()?nullptr:raw.data(),raw.size());
        while(fuzz.remaining()>=8) {
            auto before=fuzz.offset();std::uint64_t value=0;
            CHECK(fuzz.readU64(value) && fuzz.offset()==before+8);
        }
        auto before=fuzz.offset();std::uint64_t value=0;
        CHECK(!fuzz.readU64(value) && fuzz.offset()==before);
    }
    SteadyClock clock;
    auto before=clock.nowNanoseconds(),after=clock.nowNanoseconds();
    CHECK(after>=before);
    std::cout << "PASS: endian golden/u64, bounds, atomic failure, 10000 fuzz smoke, monotonic clock\n";
    return 0;
}

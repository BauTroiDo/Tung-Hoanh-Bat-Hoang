#include "mmo/PortableCore.h"
#include <cstdint>
#include <iostream>
#include <limits>
#include <random>
#include <string>
#include <vector>

#define REQUIRE(condition) do { if (!(condition)) { \
    std::cerr << "FAIL at line " << __LINE__ << ": " << #condition << "\n"; return 1; \
} } while (false)

int main() {
    using namespace mmo::portable;
    std::mt19937_64 random(0x43A8C0DEu);

    // Deterministic 25k cross-field round trips with all unsigned integer widths.
    for (int sample = 0; sample < 25000; ++sample) {
        const std::uint8_t u8 = static_cast<std::uint8_t>(random());
        const std::uint16_t u16 = static_cast<std::uint16_t>(random());
        const std::uint32_t u32 = static_cast<std::uint32_t>(random());
        const std::uint64_t u64 = random();
        std::string byteString;
        const std::size_t len = static_cast<std::size_t>(random() % 130);
        for (std::size_t index = 0; index < len; ++index) {
            // Binary payload may contain NUL bytes: codec must not truncate it.
            byteString.push_back(static_cast<char>(random() & 0xffu));
        }

        BinaryWriter writer(512);
        REQUIRE(writer.writeU8(u8));
        REQUIRE(writer.writeU16(u16));
        REQUIRE(writer.writeU32(u32));
        REQUIRE(writer.writeU64(u64));
        REQUIRE(writer.writeStringU16(byteString, 130));
        const auto& encoded = writer.bytes();
        REQUIRE(encoded.size() == 1 + 2 + 4 + 8 + 2 + byteString.size());

        BinaryReader reader(encoded.data(), encoded.size());
        std::uint8_t decodedU8 = 0;
        std::uint16_t decodedU16 = 0;
        std::uint32_t decodedU32 = 0;
        std::uint64_t decodedU64 = 0;
        std::string decodedString;
        REQUIRE(reader.readU8(decodedU8) && decodedU8 == u8);
        REQUIRE(reader.readU16(decodedU16) && decodedU16 == u16);
        REQUIRE(reader.readU32(decodedU32) && decodedU32 == u32);
        REQUIRE(reader.readU64(decodedU64) && decodedU64 == u64);
        REQUIRE(reader.readStringU16(decodedString, 130) && decodedString == byteString);
        REQUIRE(reader.remaining() == 0);

        // Every truncated string prefix must be rejected without consuming data,
        // even when the prefix itself is present and advertises a larger length.
        BinaryReader firstField(encoded.data(), 0);
        std::uint8_t unchanged = 0xa5;
        REQUIRE(!firstField.readU8(unchanged) && unchanged == 0xa5 && firstField.offset() == 0);
        const std::size_t stringStart = 1 + 2 + 4 + 8;
        for (std::size_t trunc = 0; trunc < byteString.size() + 2; ++trunc) {
            BinaryReader truncated(encoded.data() + stringStart, trunc);
            std::string sentinel = "unchanged";
            REQUIRE(!truncated.readStringU16(sentinel, 130));
            REQUIRE(sentinel == "unchanged" && truncated.offset() == 0);
        }
    }

    // Zero-length byte strings and max length policy boundaries.
    BinaryWriter zeroLength(2);
    REQUIRE(zeroLength.writeStringU16("", 0));
    REQUIRE(zeroLength.size() == 2);
    BinaryReader zeroReader(zeroLength.bytes().data(), zeroLength.size());
    std::string decoded = "sentinel";
    REQUIRE(zeroReader.readStringU16(decoded, 0) && decoded.empty());
    REQUIRE(zeroReader.remaining() == 0);

    BinaryWriter boundary(4);
    REQUIRE(boundary.writeStringU16("ab", 2) && boundary.size() == 4);
    REQUIRE(!boundary.writeU8(1) && boundary.size() == 4);
    BinaryWriter tooSmall(3);
    REQUIRE(!tooSmall.writeStringU16("ab", 2) && tooSmall.size() == 0);
    REQUIRE(!tooSmall.writeStringU16("ab", 1) && tooSmall.size() == 0);

    // Null pointer, nonzero length must fail without dereferencing.
    BinaryReader nullBytes(nullptr, 8);
    std::uint64_t keep = std::numeric_limits<std::uint64_t>::max();
    REQUIRE(!nullBytes.readU64(keep) &&
            keep == std::numeric_limits<std::uint64_t>::max() &&
            nullBytes.offset() == 0);

    std::cout << "PASS: 25000 deterministic codec round trips, truncated-byte rejection, "
              << "binary strings and bounds\n";
    return 0;
}

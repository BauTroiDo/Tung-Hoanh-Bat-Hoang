#include "mmo/PortableCore.h"
#include <chrono>
#include <climits>
#include <limits>
static_assert(CHAR_BIT == 8, "Only 8-bit bytes are supported");
namespace mmo { namespace portable {
std::uint64_t SteadyClock::nowNanoseconds() const noexcept {
    using namespace std::chrono;
    return static_cast<std::uint64_t>(duration_cast<nanoseconds>(
        steady_clock::now().time_since_epoch()).count());
}
bool BinaryWriter::fits(std::size_t n) const noexcept {
    return n <= max_ && bytes_.size() <= max_ - n;
}
bool BinaryWriter::writeU8(std::uint8_t v) {
    if (!fits(1)) return false;
    bytes_.push_back(v); return true;
}
bool BinaryWriter::writeU16(std::uint16_t v) {
    if (!fits(2)) return false;
    for (unsigned shift=0; shift<16; shift+=8)
        bytes_.push_back(static_cast<std::uint8_t>(v>>shift));
    return true;
}
bool BinaryWriter::writeU32(std::uint32_t v) {
    if (!fits(4)) return false;
    for (unsigned shift=0; shift<32; shift+=8)
        bytes_.push_back(static_cast<std::uint8_t>(v>>shift));
    return true;
}
bool BinaryWriter::writeU64(std::uint64_t v) {
    if (!fits(8)) return false;
    for (unsigned shift=0; shift<64; shift+=8)
        bytes_.push_back(static_cast<std::uint8_t>(v>>shift));
    return true;
}
bool BinaryWriter::writeStringU16(const std::string& s, std::size_t maxLength) {
    if (s.size()>maxLength || s.size()>std::numeric_limits<std::uint16_t>::max()
        || s.size()>std::numeric_limits<std::size_t>::max()-2 || !fits(s.size()+2))
        return false;
    auto n=static_cast<std::uint16_t>(s.size());
    bytes_.push_back(static_cast<std::uint8_t>(n));
    bytes_.push_back(static_cast<std::uint8_t>(n>>8));
    bytes_.insert(bytes_.end(),s.begin(),s.end());
    return true;
}
bool BinaryReader::fits(std::size_t n) const noexcept {
    return data_!=nullptr && offset_<=size_ && n<=size_-offset_;
}
bool BinaryReader::readU8(std::uint8_t& v) {
    if (!fits(1)) return false;
    v=data_[offset_++]; return true;
}
bool BinaryReader::readU16(std::uint16_t& v) {
    if (!fits(2)) return false;
    v=static_cast<std::uint16_t>(
        static_cast<std::uint16_t>(data_[offset_]) |
        (static_cast<std::uint16_t>(data_[offset_+1])<<8));
    offset_+=2; return true;
}
bool BinaryReader::readU32(std::uint32_t& v) {
    if (!fits(4)) return false;
    std::uint32_t temp=0;
    for (unsigned i=0;i<4;++i) temp|=std::uint32_t(data_[offset_+i])<<(8*i);
    v=temp; offset_+=4; return true;
}
bool BinaryReader::readU64(std::uint64_t& v) {
    if (!fits(8)) return false;
    std::uint64_t temp=0;
    for (unsigned i=0;i<8;++i) temp|=std::uint64_t(data_[offset_+i])<<(8*i);
    v=temp; offset_+=8; return true;
}
bool BinaryReader::readStringU16(std::string& s, std::size_t maxLength) {
    if (!fits(2)) return false;
    auto n=static_cast<std::uint16_t>(
        static_cast<std::uint16_t>(data_[offset_]) |
        (static_cast<std::uint16_t>(data_[offset_+1])<<8));
    if (n>maxLength || n>size_-offset_-2) return false;
    s.assign(reinterpret_cast<const char*>(data_+offset_+2),n);
    offset_+=2+n;return true;
}
}}

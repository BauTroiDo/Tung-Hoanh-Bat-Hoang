#pragma once
// Original HHTT portable foundation. No Win32/POSIX platform headers.
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace mmo { namespace portable {
class IMonotonicClock {
public:
    virtual ~IMonotonicClock() = default;
    virtual std::uint64_t nowNanoseconds() const noexcept = 0;
};
class SteadyClock final : public IMonotonicClock {
public:
    std::uint64_t nowNanoseconds() const noexcept override;
};
class BinaryWriter {
public:
    explicit BinaryWriter(std::size_t maxBytes = 65536) : max_(maxBytes) {}
    bool writeU8(std::uint8_t v);
    bool writeU16(std::uint16_t v);
    bool writeU32(std::uint32_t v);
    bool writeU64(std::uint64_t v);
    bool writeStringU16(const std::string& bytes, std::size_t maxLength = 4096);
    const std::vector<std::uint8_t>& bytes() const noexcept { return bytes_; }
    std::size_t size() const noexcept { return bytes_.size(); }
private:
    bool fits(std::size_t n) const noexcept;
    std::size_t max_;
    std::vector<std::uint8_t> bytes_;
};
class BinaryReader {
public:
    BinaryReader(const std::uint8_t* data, std::size_t size) : data_(data), size_(size) {}
    bool readU8(std::uint8_t& v);
    bool readU16(std::uint16_t& v);
    bool readU32(std::uint32_t& v);
    bool readU64(std::uint64_t& v);
    // Input is a byte-string; UTF-8 validation belongs in the generated semantic layer.
    bool readStringU16(std::string& bytes, std::size_t maxLength = 4096);
    std::size_t offset() const noexcept { return offset_; }
    std::size_t remaining() const noexcept { return size_ - offset_; }
private:
    bool fits(std::size_t n) const noexcept;
    const std::uint8_t* data_;
    std::size_t size_;
    std::size_t offset_ = 0;
};
}} // namespaces

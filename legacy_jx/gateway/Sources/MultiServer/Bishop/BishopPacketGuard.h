#ifndef JX_BISHOP_PACKET_GUARD_H
#define JX_BISHOP_PACKET_GUARD_H

// Offline-testable, allocation-free Bishop wire-input validation.
// Reject malformed/short packets before any cast, indexing or strlen.
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace BishopPacketGuard
{
inline bool HasBytes(const void* bytes, std::size_t length, std::size_t minimum)
{
    return bytes != nullptr && length >= minimum;
}

inline bool ExactBytes(const void* bytes, std::size_t length, std::size_t expected)
{
    return bytes != nullptr && length == expected;
}

inline bool HasNul(const char* chars, std::size_t capacity)
{
    return chars != nullptr && capacity != 0 &&
        std::memchr(chars, 0, capacity) != nullptr;
}

template<std::size_t N>
inline bool HasNul(const char (&chars)[N])
{
    return HasNul(chars, N);
}

inline bool ReadU32(const void* data, std::size_t length,
                    std::size_t offset, std::uint32_t& value)
{
    if (data == nullptr || offset > length ||
        length - offset < sizeof(value))
        return false;
    std::memcpy(&value, static_cast<const unsigned char*>(data) + offset,
                sizeof(value));
    return true;
}

// A map configuration suffix must not become an arbitrary filesystem path.
inline bool SafeMapSuffix(const char* chars, std::size_t capacity)
{
    if (!HasNul(chars, capacity) || chars[0] == 0)
        return false;
    for (std::size_t i = 0; i < capacity && chars[i] != 0; ++i)
    {
        const char c = chars[i];
        if (!((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
              (c >= '0' && c <= '9') || c == '_' || c == '-'))
            return false;
    }
    return true;
}

template<std::size_t N>
inline bool SafeMapSuffix(const char (&chars)[N])
{
    return SafeMapSuffix(chars, N);
}
}
#endif

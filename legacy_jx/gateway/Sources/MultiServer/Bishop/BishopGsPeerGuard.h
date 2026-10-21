#ifndef JX_BISHOP_GS_PEER_GUARD_H
#define JX_BISHOP_GS_PEER_GUARD_H

// Defense in depth for the unauthenticated legacy GameServer listener.
// CSocketAddress::GetAsString returns "a.b.c.d : port" (with spaces).
// This is an IP allowlist only, NOT cryptographic mutual authentication.
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace BishopGsPeerGuard
{
inline bool Space(char c) { return c == ' ' || c == '\t'; }
inline void SkipSpace(const char*& p) { while (Space(*p)) ++p; }

inline bool ParseIPv4(const char*& p, std::uint32_t& address)
{
    address = 0;
    for (int octet = 0; octet != 4; ++octet)
    {
        if (*p < '0' || *p > '9') return false;
        unsigned int value = 0;
        unsigned int digits = 0;
        do {
            if (++digits > 3) return false;
            value = value * 10 + static_cast<unsigned int>(*p - '0');
            if (value > 255) return false;
            ++p;
        } while (*p >= '0' && *p <= '9');
        if (digits > 1 && p[-static_cast<int>(digits)] == '0') return false;
        address = (address << 8) | value;
        if (octet != 3)
        {
            if (*p != '.') return false;
            ++p;
        }
    }
    return true;
}

inline bool ParsePeerEndpoint(const char* text, std::uint32_t& address)
{
    if (!text) return false;
    const char* p = text;
    SkipSpace(p);
    if (!ParseIPv4(p, address)) return false;
    SkipSpace(p);
    if (*p != ':') return false;
    ++p;
    SkipSpace(p);
    if (*p < '0' || *p > '9') return false;
    unsigned int port = 0;
    unsigned int digits = 0;
    while (*p >= '0' && *p <= '9')
    {
        if (++digits > 5) return false;
        port = port * 10 + static_cast<unsigned int>(*p - '0');
        if (port > 65535) return false;
        ++p;
    }
    SkipSpace(p);
    return port != 0 && *p == '\0';
}

// Explicit comma-separated IPv4 list, no wildcards, CIDR, hostnames or
// implicit trust. Empty/malformed config fails closed; capped at 32 peers.
inline bool ValidateAllowlist(const char* text)
{
    if (!text || !*text) return false;
    const char* p = text;
    unsigned int count = 0;
    for (;;)
    {
        SkipSpace(p);
        std::uint32_t address = 0;
        if (!ParseIPv4(p, address) || ++count > 32) return false;
        SkipSpace(p);
        if (*p == '\0') return true;
        if (*p != ',') return false;
        ++p;
    }
}

inline bool PeerAllowed(const char* endpoint, const char* allowlist)
{
    if (!ValidateAllowlist(allowlist)) return false;
    std::uint32_t peer = 0;
    if (!ParsePeerEndpoint(endpoint, peer)) return false;
    const char* p = allowlist;
    for (;;)
    {
        SkipSpace(p);
        std::uint32_t entry = 0;
        if (!ParseIPv4(p, entry)) return false;
        if (entry == peer) return true;
        SkipSpace(p);
        if (*p == '\0') return false;
        ++p; // Already validated comma-separated allowlist
    }
}
}
#endif

#ifndef JX_ROLE_SAVE_REVISION_WIRE_H
#define JX_ROLE_SAVE_REVISION_WIRE_H

// Internal Goddess <-> Bishop <-> GameServer role-blob extension.
// Stage 2: opt-in codec only. Never write this trailer into StudentBlob
// and never silently interpret an unversioned legacy save as revision 0.
#include <stddef.h>
#include <stdint.h>
#include <string.h>

namespace JXRoleRevisionWire {
enum { kTrailerBytes = 24, kRoleNameCapacity = 32 };
enum DecodeResult { Invalid = -1, Absent = 0, Valid = 1 };

static inline uint32_t ReadU32(const unsigned char* p) {
    return (uint32_t)p[0] | (uint32_t(p[1]) << 8) |
           (uint32_t(p[2]) << 16) | (uint32_t(p[3]) << 24);
}
static inline uint64_t ReadU64(const unsigned char* p) {
    return uint64_t(ReadU32(p)) | (uint64_t(ReadU32(p + 4)) << 32);
}
static inline void WriteU32(unsigned char* p, uint32_t n) {
    for (int i = 0; i != 4; ++i) p[i] = (unsigned char)(n >> (8 * i));
}
static inline void WriteU64(unsigned char* p, uint64_t n) {
    for (int i = 0; i != 8; ++i) p[i] = (unsigned char)(n >> (8 * i));
}
// A bounded name digest helps reject a trailer belonging to a different role.
// It is NOT an authentication token. Role identity must be checked separately.
static inline bool NameDigest(const char* role, uint32_t* digest) {
    if (!role || !digest) return false;
    uint32_t h = 2166136261u;
    for (size_t i = 0; i != kRoleNameCapacity; ++i) {
        const unsigned char c = (unsigned char)role[i];
        if (c == 0) {
            if (i == 0) return false;
            *digest = h;
            return true;
        }
        h = (h ^ c) * 16777619u;
    }
    return false;
}
static inline void DigestTrailer(const unsigned char* trailer,
                                 uint32_t* digest) {
    uint32_t h = 2166136261u;
    for (int i = 0; i != 20; ++i) h = (h ^ trailer[i]) * 16777619u;
    *digest = h;
}
static inline bool Encode(unsigned char* out, size_t space,
                          const char* role, uint64_t revision) {
    uint32_t nameHash = 0;
    if (!out || space < kTrailerBytes || !NameDigest(role, &nameHash) ||
        revision > INT64_MAX) return false;
    static const unsigned char magic[8] =
        {'J', 'X', 'R', 'E', 'V', '0', '0', '1'};
    memcpy(out, magic, 8);
    WriteU64(out + 8, revision);
    WriteU32(out + 16, nameHash);
    uint32_t check = 0;
    DigestTrailer(out, &check);
    WriteU32(out + 20, check);
    return true;
}
// The blob length is supplied by TRoleData::dwDataLen, validated by caller.
// Exact suffix length is mandatory: no extra bytes or unknown version.
// A missing trailer means "not negotiated", NOT "version zero".
static inline DecodeResult Decode(const void* raw, size_t received,
                                  size_t blobBytes, const char* role,
                                  uint64_t* revision) {
    if (!raw || !revision || blobBytes == 0 || received < blobBytes)
        return Invalid;
    if (received == blobBytes) return Absent;
    if (received - blobBytes != kTrailerBytes) return Invalid;
    const unsigned char* p = (const unsigned char*)raw + blobBytes;
    static const unsigned char magic[8] =
        {'J', 'X', 'R', 'E', 'V', '0', '0', '1'};
    uint32_t nameHash = 0, check = 0;
    if (memcmp(p, magic, 8) != 0 || !NameDigest(role, &nameHash))
        return Invalid;
    DigestTrailer(p, &check);
    if (ReadU32(p + 16) != nameHash || ReadU32(p + 20) != check)
        return Invalid;
    const uint64_t found = ReadU64(p + 8);
    if (found > INT64_MAX) return Invalid;
    *revision = found;
    return Valid;
}
// Ordinary-save requests and ACKs bind the revision to a unique request nonce.
// The login trailer remains 24 bytes for backwards parsing. Save uses 36.
enum { kSaveBytes = kTrailerBytes + 12 };
static inline bool EncodeSave(unsigned char* out, size_t space,
                              const char* role, uint64_t revision,
                              uint64_t requestNonce) {
    if (!out || space < kSaveBytes || !requestNonce ||
        !Encode(out, kTrailerBytes, role, revision)) return false;
    WriteU64(out + kTrailerBytes, requestNonce);
    uint32_t integrity = 2166136261u;
    for (size_t i = 0; i < kTrailerBytes + 8; ++i)
        integrity = (integrity ^ out[i]) * 16777619u;
    WriteU32(out + kTrailerBytes + 8, integrity);
    return true;
}
static inline DecodeResult DecodeSave(const void* raw, size_t received,
                                      size_t blobBytes, const char* role,
                                      uint64_t* revision, uint64_t* nonce) {
    if (!raw || !revision || !nonce || blobBytes == 0 ||
        received < blobBytes || received - blobBytes != kSaveBytes)
        return Invalid;
    uint64_t found = 0;
    if (Decode(raw, blobBytes + kTrailerBytes, blobBytes, role,
               &found) != Valid) return Invalid;
    const unsigned char* suffix =
        (const unsigned char*)raw + blobBytes;
    const uint64_t id = ReadU64(suffix + kTrailerBytes);
    if (id == 0) return Invalid;
    uint32_t integrity = 2166136261u;
    for (size_t i = 0; i < kTrailerBytes + 8; ++i)
        integrity = (integrity ^ suffix[i]) * 16777619u;
    if (ReadU32(suffix + kTrailerBytes + 8) != integrity)
        return Invalid;
    *revision = found;
    *nonce = id;
    return Valid;
}
} // namespace JXRoleRevisionWire
#endif

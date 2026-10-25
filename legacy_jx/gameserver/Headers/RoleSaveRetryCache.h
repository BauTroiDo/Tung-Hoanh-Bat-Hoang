#ifndef JX_ROLE_SAVE_RETRY_CACHE_H
#define JX_ROLE_SAVE_RETRY_CACHE_H

// Cache the actual framed Rainbow payload, not a mutable TRoleData pointer.
// A retry must retain exact frame bytes, expected revision, nonce and role.
// No database writes or network operations take place in this helper.
#include <map>
#include <string>
#include <vector>
#include <stdint.h>
#include <cstring>

namespace JXRoleSaveRetry {
struct Pending {
    std::string role;
    uint64_t expected;
    uint64_t nonce;
    uint32_t sentTick;
    unsigned int retries;
    std::vector<std::vector<unsigned char> > frames;
    Pending() : expected(0), nonce(0), sentTick(0), retries(0) {}
    bool Due(uint32_t now, uint32_t timeoutMs) const {
        return uint32_t(now - sentTick) >= timeoutMs;
    }
};
class Cache {
    std::map<int, Pending> entries_;
public:
    bool Store(int slot, const std::string& role, uint64_t expected,
               uint64_t nonce, uint32_t now,
               const std::vector<std::vector<unsigned char> >& frames) {
        if (slot <= 0 || role.empty() || role.size() >= 32 ||
            nonce == 0 || expected > INT64_MAX || frames.empty())
            return false;
        size_t total = 0;
        for (size_t i = 0; i < frames.size(); ++i) {
            if (frames[i].empty() || frames[i].size() > 1024) return false;
            total += frames[i].size();
            if (total > 256*1024) return false;
        }
        Pending p;
        p.role = role; p.expected = expected; p.nonce = nonce;
        p.sentTick = now; p.retries = 0; p.frames = frames;
        entries_[slot] = p;
        return true;
    }
    const Pending* Find(int slot) const {
        std::map<int, Pending>::const_iterator it = entries_.find(slot);
        return it == entries_.end() ? NULL : &it->second;
    }
    bool Matches(int slot, const std::string& role, uint64_t expected,
                 uint64_t nonce) const {
        const Pending* p = Find(slot);
        return p && p->role == role && p->expected == expected &&
               p->nonce == nonce;
    }
    bool RemoveIfMatch(int slot, const std::string& role, uint64_t expected,
                       uint64_t nonce) {
        if (!Matches(slot, role, expected, nonce)) return false;
        entries_.erase(slot); return true;
    }
    bool RecordRetry(int slot, uint32_t now) {
        std::map<int, Pending>::iterator it = entries_.find(slot);
        if (it == entries_.end()) return false;
        ++it->second.retries; it->second.sentTick = now; return true;
    }
    void Remove(int slot) { entries_.erase(slot); }
    void Clear() { entries_.clear(); }
    const std::map<int, Pending>& Entries() const { return entries_; }
};
} // namespace JXRoleSaveRetry
#endif

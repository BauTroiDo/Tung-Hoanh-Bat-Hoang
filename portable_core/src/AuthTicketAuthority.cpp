#include "mmo/AuthTicketAuthority.h"

#include <array>
#include <limits>
#include <utility>

#if defined(_WIN32)
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <bcrypt.h>
#else
#include <cerrno>
#include <sys/random.h>
#endif

namespace mmo::portable {
namespace {
bool operatingSystemRandom(std::array<unsigned char, 32>& bytes) {
#if defined(_WIN32)
    return BCryptGenRandom(nullptr, bytes.data(),
              static_cast<ULONG>(bytes.size()),
              BCRYPT_USE_SYSTEM_PREFERRED_RNG) == 0;
#elif defined(__linux__)
    std::size_t done = 0;
    while (done < bytes.size()) {
        const ssize_t n = getrandom(bytes.data() + done, bytes.size() - done, 0);
        if (n < 0 && errno == EINTR) continue;
        if (n <= 0) return false;
        done += static_cast<std::size_t>(n);
    }
    return true;
#else
#error "AuthTicketAuthority requires a reviewed operating-system CSPRNG"
#endif
}
std::string hexToken(const std::array<unsigned char,32>& bytes) {
    static constexpr char alphabet[] = "0123456789abcdef";
    std::string out(64, '0');
    for (std::size_t i=0; i<bytes.size(); ++i) {
        out[2*i] = alphabet[bytes[i] >> 4];
        out[2*i+1] = alphabet[bytes[i] & 0x0f];
    }
    return out;
}
} // namespace

AuthTicketAuthority::AuthTicketAuthority(std::size_t maxAccounts, NowProvider now)
    : maxAccounts_(maxAccounts),
      now_(now ? std::move(now) : NowProvider([] { return Clock::now(); })) {}

bool AuthTicketAuthority::isCanonicalToken(std::string_view token) {
    if (token.size() != 64) return false;
    for (const char c : token)
        if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f')))
            return false;
    return true;
}

void AuthTicketAuthority::revokeLocked(std::uint64_t accountId) {
    const auto it = accounts_.find(accountId);
    if (it == accounts_.end()) return;
    if (!it->second.pendingToken.empty())
        pending_.erase(it->second.pendingToken);
    accounts_.erase(it);
}

void AuthTicketAuthority::pruneExpiredLocked(Clock::time_point now) {
    for (auto it = pending_.begin(); it != pending_.end(); ) {
        if (now < it->second.deadline) { ++it; continue; }
        const auto state = accounts_.find(it->second.accountId);
        if (state != accounts_.end() &&
            state->second.generation == it->second.generation &&
            state->second.pendingToken == it->first)
            accounts_.erase(state);
        it = pending_.erase(it);
    }
}

IssuedAuthTicket AuthTicketAuthority::issue(std::uint64_t accountId,
                                            std::uint32_t realm,
                                            std::chrono::seconds ttl) {
    if (accountId == 0 || realm == 0 || ttl.count() < 1 || ttl.count() > 300)
        return {AuthTicketStatus::InvalidRequest, {}};

    std::array<unsigned char,32> randomness{};
    if (!operatingSystemRandom(randomness))
        return {AuthTicketStatus::EntropyUnavailable, {}};
    const std::string token = hexToken(randomness);
    randomness.fill(0);

    const Clock::time_point now = now_();
    if (Clock::time_point::max() - now < ttl)
        return {AuthTicketStatus::InvalidRequest, {}};

    std::lock_guard<std::mutex> hold(mutex_);
    pruneExpiredLocked(now);
    if (accounts_.find(accountId) == accounts_.end() &&
        accounts_.size() >= maxAccounts_)
        return {AuthTicketStatus::Capacity, {}};
    if (generation_ == std::numeric_limits<std::uint64_t>::max())
        return {AuthTicketStatus::GenerationExhausted, {}};

    // A collision never overwrites a live ticket. Do not issue a guessable
    // substitute when OS entropy fails or an unlikely collision occurs.
    if (pending_.find(token) != pending_.end())
        return {AuthTicketStatus::EntropyUnavailable, {}};

    revokeLocked(accountId);
    const std::uint64_t generation = ++generation_;
    accounts_.emplace(accountId, State{generation,realm,0,token});
    pending_.emplace(token, Pending{accountId,generation,realm,now + ttl});
    return {AuthTicketStatus::Ok, token};
}

AuthTicketConsumption AuthTicketAuthority::consume(std::string_view token,
                                                   std::uint32_t realm,
                                                   std::uint64_t connection) {
    if (!isCanonicalToken(token) || realm == 0 || connection == 0)
        return {AuthTicketStatus::InvalidRequest, {}};

    std::lock_guard<std::mutex> hold(mutex_);
    auto it = pending_.find(std::string(token));
    if (it == pending_.end())
        return {AuthTicketStatus::NotFound, {}};
    const auto now = now_();
    if (now >= it->second.deadline) {
        revokeLocked(it->second.accountId);
        return {AuthTicketStatus::Expired, {}};
    }
    if (realm != it->second.realm)
        return {AuthTicketStatus::WrongRealm, {}};

    auto state = accounts_.find(it->second.accountId);
    if (state == accounts_.end() ||
        state->second.generation != it->second.generation ||
        state->second.pendingToken != it->first) {
        pending_.erase(it);
        return {AuthTicketStatus::NotFound, {}};
    }

    const AuthenticatedWorldSession accepted {
        it->second.accountId, realm, it->second.generation, connection
    };
    state->second.pendingToken.clear();
    state->second.connectionId = connection;
    pending_.erase(it);
    return {AuthTicketStatus::Ok, accepted};
}

bool AuthTicketAuthority::valid(const AuthenticatedWorldSession& session) const {
    if (!session.accountId || !session.realmId ||
        !session.generation || !session.connectionId)
        return false;
    std::lock_guard<std::mutex> hold(mutex_);
    auto it = accounts_.find(session.accountId);
    return it != accounts_.end() &&
           it->second.pendingToken.empty() &&
           it->second.generation == session.generation &&
           it->second.realm == session.realmId &&
           it->second.connectionId == session.connectionId;
}

bool AuthTicketAuthority::close(const AuthenticatedWorldSession& session) {
    if (!session.accountId || !session.realmId ||
        !session.generation || !session.connectionId)
        return false;
    std::lock_guard<std::mutex> hold(mutex_);
    const auto it = accounts_.find(session.accountId);
    if (it == accounts_.end() || !it->second.pendingToken.empty() ||
        it->second.generation != session.generation ||
        it->second.realm != session.realmId ||
        it->second.connectionId != session.connectionId)
        return false;
    accounts_.erase(it);
    return true;
}

bool AuthTicketAuthority::revokeAccount(std::uint64_t accountId) {
    if (!accountId) return false;
    std::lock_guard<std::mutex> hold(mutex_);
    const bool exists = accounts_.find(accountId) != accounts_.end();
    revokeLocked(accountId);
    return exists;
}

std::size_t AuthTicketAuthority::trackedAccounts() const {
    std::lock_guard<std::mutex> hold(mutex_);
    return accounts_.size();
}
} // namespace mmo::portable

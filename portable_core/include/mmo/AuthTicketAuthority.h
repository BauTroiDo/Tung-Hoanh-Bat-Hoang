#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <map>
#include <mutex>
#include <string>
#include <string_view>

namespace mmo::portable {

// A trusted AuthService issues this opaque ticket only AFTER authenticating
// the account. Neither account IDs nor connection generations are credentials.
enum class AuthTicketStatus {
    Ok,
    InvalidRequest,
    NotFound,
    Expired,
    WrongRealm,
    Capacity,
    EntropyUnavailable,
    GenerationExhausted
};

struct IssuedAuthTicket {
    AuthTicketStatus status{AuthTicketStatus::InvalidRequest};
    // 256 bits from an operating-system CSPRNG, hexadecimal lower-case.
    // Bearer secret: never log, persist in plaintext or expose over non-TLS.
    std::string token;
};

struct AuthenticatedWorldSession {
    std::uint64_t accountId{0};
    std::uint32_t realmId{0};
    std::uint64_t generation{0};
    // Assigned by a trusted server-side transport, NEVER read from wire.
    std::uint64_t connectionId{0};
};

struct AuthTicketConsumption {
    AuthTicketStatus status{AuthTicketStatus::InvalidRequest};
    AuthenticatedWorldSession session{};
};

// A safe, stand-alone prototype for in-process AuthService + WorldGateway.
// For different processes/hosts, replace the in-memory ledger with a shared
// strongly-consistent atomic consume/claim store. No wire auth protocol here.
class AuthTicketAuthority final {
public:
    using Clock = std::chrono::steady_clock;
    using NowProvider = std::function<Clock::time_point()>;

    explicit AuthTicketAuthority(std::size_t maxAccounts = 16384,
                                 NowProvider now = {});
    AuthTicketAuthority(const AuthTicketAuthority&) = delete;
    AuthTicketAuthority& operator=(const AuthTicketAuthority&) = delete;

    // AuthService-only. Re-issue revokes any outstanding ticket AND the
    // earlier active session for the account (single-login policy).
    IssuedAuthTicket issue(std::uint64_t authenticatedAccountId,
                           std::uint32_t realmId,
                           std::chrono::seconds ttl = std::chrono::seconds(30));

    // WorldGateway-only. Atomic consume; the original token cannot be reused.
    // Connection ID must be selected by the authenticated server transport.
    AuthTicketConsumption consume(std::string_view token,
                                  std::uint32_t expectedRealm,
                                  std::uint64_t trustedConnectionId);

    // WorldServer validates this returned session on the authoritative
    // owner thread on every admission/rebinding and rejects stale sessions.
    bool valid(const AuthenticatedWorldSession& session) const;
    bool close(const AuthenticatedWorldSession& session);

    // AuthService-only explicit logout/ban/revocation.
    bool revokeAccount(std::uint64_t authenticatedAccountId);
    std::size_t trackedAccounts() const;

private:
    struct Pending {
        std::uint64_t accountId;
        std::uint64_t generation;
        std::uint32_t realm;
        Clock::time_point deadline;
    };
    struct State {
        std::uint64_t generation;
        std::uint32_t realm;
        std::uint64_t connectionId;
        std::string pendingToken;
    };

    static bool isCanonicalToken(std::string_view token);
    void pruneExpiredLocked(Clock::time_point now);
    void revokeLocked(std::uint64_t accountId);

    mutable std::mutex mutex_;
    const std::size_t maxAccounts_;
    NowProvider now_;
    std::uint64_t generation_{0};
    std::map<std::string, Pending> pending_;
    std::map<std::uint64_t, State> accounts_;
};

} // namespace mmo::portable

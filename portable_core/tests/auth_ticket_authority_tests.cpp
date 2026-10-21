#include "mmo/AuthTicketAuthority.h"

#include <atomic>
#include <chrono>
#include <cstdio>
#include <string>
#include <thread>
#include <vector>

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAILED at line %d: %s\n", __LINE__, #c); return 1; } } while(0)

using mmo::portable::AuthTicketAuthority;
using mmo::portable::AuthTicketStatus;
using mmo::portable::AuthenticatedWorldSession;

int main() {
    using namespace std::chrono;
    std::atomic<std::int64_t> ticks{10000};
    AuthTicketAuthority authority(3, [&] {
        return AuthTicketAuthority::Clock::time_point(milliseconds(ticks.load()));
    });

    CHECK(authority.issue(0,1).status == AuthTicketStatus::InvalidRequest);
    CHECK(authority.issue(1,0).status == AuthTicketStatus::InvalidRequest);
    CHECK(authority.issue(1,1,seconds(0)).status == AuthTicketStatus::InvalidRequest);
    CHECK(authority.issue(1,1,seconds(301)).status == AuthTicketStatus::InvalidRequest);
    CHECK(authority.consume("invalid",1,99).status == AuthTicketStatus::InvalidRequest);
    CHECK(authority.consume(std::string(64,'A'),1,99).status == AuthTicketStatus::InvalidRequest);

    const auto issued = authority.issue(101,2,seconds(30));
    CHECK(issued.status == AuthTicketStatus::Ok);
    CHECK(issued.token.size() == 64);
    for (char c : issued.token)
        CHECK((c>='0' && c<='9') || (c>='a' && c<='f'));
    CHECK(authority.consume(issued.token,2,0).status == AuthTicketStatus::InvalidRequest);
    CHECK(authority.consume(issued.token,0,100).status == AuthTicketStatus::InvalidRequest);
    CHECK(authority.consume(issued.token,3,100).status == AuthTicketStatus::WrongRealm);
    auto result = authority.consume(issued.token,2,100);
    CHECK(result.status == AuthTicketStatus::Ok);
    CHECK(result.session.accountId == 101);
    CHECK(result.session.realmId == 2);
    CHECK(result.session.connectionId == 100);
    CHECK(authority.valid(result.session));
    CHECK(authority.consume(issued.token,2,101).status == AuthTicketStatus::NotFound);
    AuthenticatedWorldSession spoof = result.session;
    ++spoof.connectionId;
    CHECK(!authority.valid(spoof));
    CHECK(!authority.close(spoof));

    // Re-issue from a verified AuthService login invalidates stale world
    // generations and old ticket records; old world cannot close new lease.
    const auto updated = authority.issue(101,2);
    CHECK(updated.status == AuthTicketStatus::Ok);
    CHECK(updated.token != issued.token);
    CHECK(!authority.valid(result.session));
    CHECK(!authority.close(result.session));
    const auto newer = authority.consume(updated.token,2,200);
    CHECK(newer.status == AuthTicketStatus::Ok);
    CHECK(newer.session.generation > result.session.generation);
    CHECK(authority.valid(newer.session));

    const auto expired = authority.issue(102,2,seconds(1));
    CHECK(expired.status == AuthTicketStatus::Ok);
    ticks.fetch_add(1000);
    CHECK(authority.consume(expired.token,2,333).status == AuthTicketStatus::Expired);
    CHECK(authority.consume(expired.token,2,333).status == AuthTicketStatus::NotFound);
    CHECK(authority.trackedAccounts() == 1);

    // Full ledger refuses additional accounts; explicit close makes room.
    CHECK(authority.issue(104,2).status == AuthTicketStatus::Ok);
    CHECK(authority.issue(105,2).status == AuthTicketStatus::Ok);
    CHECK(authority.trackedAccounts() == 3);
    CHECK(authority.issue(106,2).status == AuthTicketStatus::Capacity);
    CHECK(authority.close(newer.session));
    CHECK(!authority.valid(newer.session));
    CHECK(authority.issue(106,2).status == AuthTicketStatus::Ok);
    CHECK(authority.trackedAccounts() == 3);
    CHECK(authority.revokeAccount(106));
    CHECK(!authority.revokeAccount(106));

    // Expired outstanding entries are safely evicted on new issuance.
    ticks.fetch_add(310000);
    CHECK(authority.issue(107,3).status == AuthTicketStatus::Ok);
    CHECK(authority.trackedAccounts() == 1);
    CHECK(authority.revokeAccount(107));

    // Atomic one-time consumption of the same ticket by 64 World connections.
    for (int pass=0; pass<10; ++pass) {
        const auto race = authority.issue(900,1);
        CHECK(race.status == AuthTicketStatus::Ok);
        std::atomic<int> successes{0};
        std::vector<std::thread> workers;
        for (int i=0; i<64; ++i)
            workers.emplace_back([&,i] {
                auto reply = authority.consume(race.token,1,static_cast<std::uint64_t>(i+1));
                if (reply.status == AuthTicketStatus::Ok) {
                    ++successes;
                    (void)reply;
                }
            });
        for (auto& thread : workers) thread.join();
        CHECK(successes.load() == 1);
        CHECK(authority.revokeAccount(900));
    }
    CHECK(authority.trackedAccounts() == 0);

    // Revoke a token before it reaches WorldServer.
    const auto revoked = authority.issue(700,5);
    CHECK(revoked.status == AuthTicketStatus::Ok);
    CHECK(authority.revokeAccount(700));
    CHECK(authority.consume(revoked.token,5,77).status == AuthTicketStatus::NotFound);

    std::puts("AUTH_TICKET_CORE_PASS");
    return 0;
}

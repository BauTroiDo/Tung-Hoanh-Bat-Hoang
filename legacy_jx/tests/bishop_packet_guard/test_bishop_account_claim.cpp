#include "BishopAccountClaimSession.h"
#include <atomic>
#include <cstdio>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#define CHECK(test) do { if (!(test)) { std::fprintf(stderr, "FAIL line %d: %s\n", __LINE__, #test); return 1; } } while (0)

int main()
{
    BishopAccountClaimSession alice;
    BishopAccountClaimSession bob;

    CHECK(!alice.TryReserve("player"));
    alice.Begin();
    bob.Begin();
    CHECK(alice.TryReserve("Player"));
    CHECK(alice.TryReserve("PLAYER"));
    CHECK(!bob.TryReserve("player"));
    CHECK(!alice.TryReserve("another-account"));
    alice.ReleaseClaim();
    CHECK(bob.TryReserve("PLAYER"));
    CHECK(!alice.TryReserve("Player"));
    bob.End();
    CHECK(alice.TryReserve("player"));
    alice.End();
    bob.Begin();
    CHECK(bob.TryReserve("Player"));
    bob.End();

    CHECK(!BishopAccountClaimRegistry::Instance().Claim("", 1));
    CHECK(!BishopAccountClaimRegistry::Instance().Claim("player", 0));
    CHECK(BishopAccountClaimRegistry::CanonicalAccount("UserABC") == "userabc");
    CHECK(BishopAccountClaimRegistry::CanonicalAccount(nullptr).empty());
    CHECK(BishopAccountClaimRegistry::CanonicalAccount("").empty());
    CHECK(BishopAccountClaimRegistry::CanonicalAccount(std::string(65,'x').c_str()).empty());

    auto& registry = BishopAccountClaimRegistry::Instance();
    const auto first = registry.NextToken();
    const auto second = registry.NextToken();
    CHECK(first != second && first != 0 && second != 0);
    CHECK(registry.Claim("token-test",first));
    CHECK(!registry.Claim("token-test",second));
    CHECK(!registry.Release("token-test",second));
    CHECK(!registry.Claim("token-test",second));
    CHECK(registry.Release("token-test",first));
    CHECK(registry.Claim("token-test",second));
    CHECK(registry.Release("token-test",second));

    for (int round=0; round<20; ++round)
    {
        std::vector<std::unique_ptr<BishopAccountClaimSession>> sessions;
        for (int i=0;i<64;++i)
        {
            sessions.emplace_back(new BishopAccountClaimSession());
            sessions.back()->Begin();
        }
        std::atomic<int> successes(0);
        std::vector<std::thread> workers;
        for (int i=0;i<64;++i)
        {
            workers.emplace_back([&,i] {
                if (sessions[i]->TryReserve("SAme_Account")) ++successes;
            });
        }
        for (auto& t : workers) t.join();
        CHECK(successes.load()==1);
        for (auto& session : sessions) session->End();
        BishopAccountClaimSession reused;
        reused.Begin();
        CHECK(reused.TryReserve("same_account"));
        reused.End();
    }

    // Concurrent reserve and cleanup of a reused connection must not leak
    // a claim into the next generation.
    for (int round=0; round<20; ++round)
    {
        BishopAccountClaimSession race;
        race.Begin();
        std::thread worker([&] { race.TryReserve("cleanup_race"); });
        race.End();
        worker.join();
        race.Begin();
        CHECK(race.TryReserve("cleanup_race"));
        race.End();
    }
    std::puts("BISHOP_ACCOUNT_CLAIM_LOCAL_PASS");
    return 0;
}

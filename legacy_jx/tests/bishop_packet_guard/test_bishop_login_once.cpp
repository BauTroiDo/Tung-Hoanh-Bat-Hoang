#include "BishopLoginAttemptGate.h"
#include <atomic>
#include <cstdio>
#include <thread>
#include <vector>

#define CHECK(x) do { if (!(x)) { std::fprintf(stderr, "FAILED line %d: %s\n", __LINE__, #x); return 1; } } while (0)

static int RaceOneConnection()
{
    BishopLoginAttemptGate gate;
    std::atomic<int> admitted(0);
    std::vector<std::thread> workers;
    workers.reserve(64);
    for (int i = 0; i < 64; ++i)
        workers.emplace_back([&]() {
            if (gate.TryClaim())
                admitted.fetch_add(1);
        });
    for (auto& worker : workers)
        worker.join();
    CHECK(admitted.load() == 1);
    CHECK(gate.Claimed());
    CHECK(!gate.TryClaim());
    return 0;
}

int main()
{
    BishopLoginAttemptGate gate;
    CHECK(!gate.Claimed());
    CHECK(gate.TryClaim());
    CHECK(!gate.TryClaim());
    CHECK(gate.Claimed());
    gate.ResetForNewConnection();
    CHECK(!gate.Claimed());
    CHECK(gate.TryClaim());
    for (int cycle = 0; cycle < 20; ++cycle)
        if (RaceOneConnection() != 0) return 1;
    std::puts("BISHOP_LOGIN_ATTEMPT_ONCE_PASS");
    return 0;
}

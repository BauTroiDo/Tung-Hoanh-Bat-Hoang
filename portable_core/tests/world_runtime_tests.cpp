#include "mmo/WorldRuntime.h"
#include <atomic>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <thread>
#include <vector>

#define CHECK(expr) do { if (!(expr)) { \
    std::cerr << "FAIL at " << __LINE__ << ": " << #expr << "\n"; return 1; \
} } while(false)

int main() {
    using namespace mmo::portable;
    bool invalid = false;
    try { WorldCommandQueue noCapacity(0); }
    catch (const std::invalid_argument&) { invalid = true; }
    CHECK(invalid);
    WorldCommandQueue queue(2);
    CHECK(queue.tryPost({}) == WorldCommandQueue::PostStatus::Invalid);
    CHECK(queue.tryPost([]{}) == WorldCommandQueue::PostStatus::Accepted);
    CHECK(queue.tryPost([]{}) == WorldCommandQueue::PostStatus::Accepted);
    CHECK(queue.tryPost([]{}) == WorldCommandQueue::PostStatus::Full);
    CHECK(queue.pending() == 2);
    CHECK(queue.drain(3).unbound);
    CHECK(queue.bindOwnerToCurrentThread());
    std::atomic<bool> foreignDrained{false};
    std::atomic<bool> foreignBind{true};
    std::thread wrongThread([&] {
        foreignDrained = queue.drain(2).wrongOwner;
        foreignBind = queue.bindOwnerToCurrentThread();
    });
    wrongThread.join();
    CHECK(foreignDrained && !foreignBind);
    auto part = queue.drain(1);
    CHECK(part.attempted == 1 && part.remaining == 1);
    CHECK(queue.drain(0).remaining == 1);
    CHECK(queue.drain(8).attempted == 1);
    CHECK(queue.pending() == 0);

    // A callback may enqueue further work without deadlocking; the bounded
    // tick budget prevents one chain from starving other world systems.
    int value = 0;
    CHECK(queue.tryPost([&] {
        ++value;
        (void)queue.tryPost([&] { ++value; });
    }) == WorldCommandQueue::PostStatus::Accepted);
    CHECK(queue.drain(1).attempted == 1);
    CHECK(value == 1 && queue.pending() == 1);
    CHECK(queue.drain(1).attempted == 1 && value == 2);
    bool nestedRejected = false;
    CHECK(queue.tryPost([&] {
        auto nested = queue.drain(1000);
        nestedRejected = nested.reentrant && nested.attempted == 0;
        (void)queue.tryPost([&] { ++value; });
    }) == WorldCommandQueue::PostStatus::Accepted);
    CHECK(queue.drain(1).attempted == 1 && nestedRejected && queue.pending() == 1);
    CHECK(queue.drain(1).attempted == 1 && value == 3);
    CHECK(queue.tryPost([] { throw std::runtime_error("expected"); }) ==
        WorldCommandQueue::PostStatus::Accepted);
    CHECK(queue.tryPost([&] { ++value; }) == WorldCommandQueue::PostStatus::Accepted);
    auto errors = queue.drain(2);
    CHECK(errors.attempted == 2 && errors.failed == 1 && value == 4);
    queue.close();
    CHECK(queue.closed());
    CHECK(queue.tryPost([]{}) == WorldCommandQueue::PostStatus::Closed);
    CHECK(queue.pending() == 0);
    WorldCommandQueue gracefulClose(1);
    CHECK(gracefulClose.bindOwnerToCurrentThread());
    int flushCount = 0;
    CHECK(gracefulClose.tryPost([&]{ ++flushCount; }) ==
        WorldCommandQueue::PostStatus::Accepted);
    gracefulClose.close();
    CHECK(gracefulClose.tryPost([]{}) == WorldCommandQueue::PostStatus::Closed);
    CHECK(gracefulClose.drain(1).attempted == 1 && flushCount == 1);

    // Multi-producer, single-owner stress: 6x2000 commands; per-producer
    // ordering must hold, no duplicated/drop work under transient backpressure.
    constexpr int producers = 6;
    constexpr int perProducer = 2000;
    WorldCommandQueue concurrent(128);
    CHECK(concurrent.bindOwnerToCurrentThread());
    std::vector<int> last(producers, -1);
    std::vector<int> counts(producers, 0);
    std::atomic<int> completedProducers{0};
    std::atomic<int> postFailures{0};
    std::atomic<bool> correct{true};
    std::vector<std::thread> writers;
    for (int p = 0; p < producers; ++p) {
        writers.emplace_back([&, p] {
            for (int seq = 0; seq < perProducer; ++seq) {
                for (;;) {
                    auto r = concurrent.tryPost([&, p, seq] {
                        if (seq != last[p] + 1) correct = false;
                        last[p] = seq;
                        counts[p]++;
                    });
                    if (r == WorldCommandQueue::PostStatus::Accepted) break;
                    if (r != WorldCommandQueue::PostStatus::Full) {
                        ++postFailures;
                        return;
                    }
                    std::this_thread::yield();
                }
            }
            ++completedProducers;
        });
    }
    const auto deadline = std::chrono::steady_clock::now() +
        std::chrono::seconds(12);
    std::size_t totalExecuted = 0;
    while (completedProducers < producers || concurrent.pending() > 0) {
        auto result = concurrent.drain(48);
        totalExecuted += result.attempted;
        CHECK(result.failed == 0 && !result.wrongOwner);
        if (std::chrono::steady_clock::now() > deadline) {
            // If producers hang, join would also hang. Failing here forces
            // harness timeout to catch deadlock without false PASS.
            CHECK(false && "producer stress timed out");
        }
        std::this_thread::yield();
    }
    for (auto& writer : writers) writer.join();
    CHECK(postFailures == 0 && correct);
    CHECK(totalExecuted == producers * perProducer);
    for (int p = 0; p < producers; ++p) {
        CHECK(counts[p] == perProducer && last[p] == perProducer - 1);
    }
    CHECK(concurrent.pending() == 0);

    // Integer-only 18Hz timing: 1 sec == 18 ticks exactly; no drift
    // from a truncated 55,555,555ns period. Catch-up is limited to 3.
    TickBudget tick(18, 3);
    CHECK(tick.poll(123).steps == 0);  // auto-reset
    CHECK(tick.poll(123 + 55555555).steps == 0);
    CHECK(tick.poll(123 + 55555556).steps == 1);
    auto late = tick.poll(123 + 1000000000ull);
    CHECK(late.steps == 3 && late.skipped == 14);
    CHECK(tick.accountedTicks() == 18);
    CHECK(tick.poll(123 + 1000000000ull).steps == 0);
    auto next = tick.poll(123 + 2000000000ull);
    CHECK(next.steps == 3 && next.skipped == 15);
    CHECK(tick.accountedTicks() == 36);
    auto rewind = tick.poll(0);
    CHECK(rewind.steps == 0 && rewind.skipped == 0);
    CHECK(tick.accountedTicks() == 0);
    invalid = false;
    try { TickBudget bad(0, 3); }
    catch (const std::invalid_argument&) { invalid = true; }
    CHECK(invalid);
    invalid = false;
    try { TickBudget bad(18, 0); }
    catch (const std::invalid_argument&) { invalid = true; }
    CHECK(invalid);
    std::cout << "PASS: world owner, capped queue, bounded drain, exception isolation,"
        << " 12000 multithread posts, monotonic 18Hz tick budget\n";
    return 0;
}

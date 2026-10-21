#include <chrono>
#include <cstdint>
#include <cstdio>
#include <thread>
#include <type_traits>

#include "KWin32.h"
#include "KTimer.h"

static_assert(sizeof(LONGLONG) == 8, "Win64 LONGLONG Linux ABI must be 64 bits");
static_assert(sizeof(DWORD) == 4, "Win64 DWORD Linux ABI must be 32 bits");

static int verify(bool condition, const char* message)
{
    if (condition)
        return 0;
    std::fprintf(stderr, "KTimer failure: %s\n", message);
    return 1;
}

int main()
{
    using namespace std::chrono;
    KTimer timer;

    // The legacy API must be deterministic before Start/Stop.
    if (verify(timer.GetElapse() == 0, "unstarted GetElapse"))
        return 1;
    if (verify(timer.GetElapseFrequency() == 0, "unstarted raw ticks"))
        return 2;
    if (verify(timer.GetInterval() == 0, "unstarted interval"))
        return 3;

    timer.Start();
    if (verify(timer.GetInterval() == 0, "Stop not called yet"))
        return 4;
    std::this_thread::sleep_for(milliseconds(65));
    const LONGLONG elapsed = timer.GetElapse();
    const LONGLONG elapsedNanos = timer.GetElapseFrequency();
    if (verify(elapsed >= 35 && elapsed < 20000, "monotonic milliseconds"))
        return 5;
    if (verify(elapsedNanos > 30000000LL && elapsedNanos < 20000000000LL,
               "high-resolution monotonic nanoseconds"))
        return 6;
    timer.Stop();
    const LONGLONG frozen = timer.GetInterval();
    if (verify(frozen >= 35 && frozen < 20000, "Stop-Start interval milliseconds"))
        return 7;
    std::this_thread::sleep_for(milliseconds(25));
    if (verify(timer.GetInterval() == frozen, "Stop-Start interval is frozen"))
        return 8;

    timer.Start();
    if (verify(timer.GetInterval() == 0, "restart resets stopped sample"))
        return 9;
    if (verify(timer.Passed(120) == FALSE, "Passed early must be false"))
        return 10;
    std::this_thread::sleep_for(milliseconds(155));
    if (verify(timer.Passed(120) == TRUE, "Passed after threshold"))
        return 11;
    if (verify(timer.Passed(120) == FALSE, "Passed resets its origin"))
        return 12;

    // Old Linux GetFPS could dereference null once the threshold elapsed.
    if (verify(timer.GetFPS(nullptr) == FALSE, "null FPS pointer"))
        return 13;
    timer.Start();
    int fps = -1;
    for (int i = 0; i < 4; ++i)
        if (verify(timer.GetFPS(&fps) == FALSE, "FPS before one second"))
            return 14;
    std::this_thread::sleep_for(milliseconds(1150));
    if (verify(timer.GetFPS(&fps) == TRUE, "FPS full second threshold"))
        return 15;
    if (verify(fps == 4, "FPS counts requests during interval"))
        return 16;
    if (verify(timer.GetFPS(&fps) == FALSE, "FPS resets timer"))
        return 17;

    std::printf("JX_LINUX_KTIMER_PASS elapsed_ms=%lld interval_ms=%lld fps=%d\n",
                static_cast<long long>(elapsed),
                static_cast<long long>(frozen), fps);
    return 0;
}

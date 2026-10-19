#pragma once
// HHTT portable WorldRuntime primitives. Not yet linked to legacy JX WorldServer.
#include <cstddef>
#include <cstdint>
#include <deque>
#include <functional>
#include <mutex>
#include <thread>

namespace mmo { namespace portable {

// Multiple threads may produce work. Only an explicitly bound world thread
// may execute it. Functions MUST capture stable IDs/immutable data rather
// than pointers into legacy JX global arrays from foreign threads.
class WorldCommandQueue final {
public:
    using Command = std::function<void()>;
    enum class PostStatus { Accepted, Full, Closed, Invalid };
    struct DrainResult {
        std::size_t attempted = 0;
        std::size_t failed = 0;
        std::size_t remaining = 0;
        bool wrongOwner = false;
        bool unbound = false;
        bool reentrant = false;
    };

    explicit WorldCommandQueue(std::size_t capacity);
    WorldCommandQueue(const WorldCommandQueue&) = delete;
    WorldCommandQueue& operator=(const WorldCommandQueue&) = delete;

    // Bind once, on the actual WorldServer owner thread.
    bool bindOwnerToCurrentThread();
    PostStatus tryPost(Command command);
    // Non-blocking producer shutdown: no new posts accepted, queued work
    // remains available for the owner to drain.
    void close();
    bool closed() const;
    std::size_t pending() const;
    std::size_t capacity() const noexcept { return capacity_; }
    // Runs <= limit commands, without holding the queue lock during callback.
    // Catches callback exceptions and counts failures so one task cannot
    // interrupt execution of later tasks. Caller handles diagnostics/recovery.
    DrainResult drain(std::size_t limit);
private:
    mutable std::mutex mutex_;
    const std::size_t capacity_;
    std::deque<Command> commands_;
    bool closed_ = false;
    bool ownerBound_ = false;
    bool draining_ = false;
    std::thread::id owner_;
};

// Pure integer monotonic tick budget. The baseline JX WorldServer uses 18 Hz.
// Late frames cannot create unlimited catch-up work; skipped frames are
// reported explicitly. This is not a sleep loop or a WorldServer replacement.
class TickBudget final {
public:
    struct PollResult {
        std::uint32_t steps = 0;
        std::uint64_t skipped = 0;
    };
    explicit TickBudget(std::uint32_t hz = 18, std::uint32_t maxCatchUpSteps = 3);
    void reset(std::uint64_t nowNanoseconds) noexcept;
    PollResult poll(std::uint64_t nowNanoseconds) noexcept;
    std::uint32_t frequencyHz() const noexcept { return hz_; }
    std::uint64_t accountedTicks() const noexcept { return accounted_; }
private:
    std::uint32_t hz_;
    std::uint32_t maxCatchUp_;
    bool initialized_ = false;
    std::uint64_t anchorNs_ = 0;
    std::uint64_t accounted_ = 0;
};
}} // namespace mmo::portable

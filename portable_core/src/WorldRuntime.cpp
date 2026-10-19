#include "mmo/WorldRuntime.h"
#include <algorithm>
#include <limits>
#include <stdexcept>
#include <utility>

namespace mmo { namespace portable {

WorldCommandQueue::WorldCommandQueue(std::size_t capacity) : capacity_(capacity) {
    if (capacity_ == 0) throw std::invalid_argument("world queue capacity must be positive");
}

bool WorldCommandQueue::bindOwnerToCurrentThread() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!ownerBound_) {
        owner_ = std::this_thread::get_id();
        ownerBound_ = true;
    }
    return owner_ == std::this_thread::get_id();
}

WorldCommandQueue::PostStatus WorldCommandQueue::tryPost(Command command) {
    if (!command) return PostStatus::Invalid;
    std::lock_guard<std::mutex> lock(mutex_);
    if (closed_) return PostStatus::Closed;
    if (commands_.size() == capacity_) return PostStatus::Full;
    commands_.push_back(std::move(command));
    return PostStatus::Accepted;
}

void WorldCommandQueue::close() {
    std::lock_guard<std::mutex> lock(mutex_);
    closed_ = true;
}

bool WorldCommandQueue::closed() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return closed_;
}

std::size_t WorldCommandQueue::pending() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return commands_.size();
}

WorldCommandQueue::DrainResult WorldCommandQueue::drain(std::size_t limit) {
    DrainResult result;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        result.unbound = !ownerBound_;
        result.wrongOwner = ownerBound_ && owner_ != std::this_thread::get_id();
        result.reentrant = draining_;
        if (result.unbound || result.wrongOwner || result.reentrant || limit == 0) {
            result.remaining = commands_.size();
            return result;
        }
        draining_ = true;
    }
    for (std::size_t i = 0; i < limit; ++i) {
        Command work;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (commands_.empty()) break;
            work = std::move(commands_.front());
            commands_.pop_front();
        }
        ++result.attempted;
        try { work(); }
        catch (...) { ++result.failed; }
    }
    {
        std::lock_guard<std::mutex> lock(mutex_);
        result.remaining = commands_.size();
        draining_ = false;
    }
    return result;
}

TickBudget::TickBudget(std::uint32_t hz, std::uint32_t maxCatchUpSteps)
    : hz_(hz), maxCatchUp_(maxCatchUpSteps) {
    if (hz_ == 0 || hz_ > 1000 || maxCatchUp_ == 0) {
        throw std::invalid_argument("invalid world tick configuration");
    }
}

void TickBudget::reset(std::uint64_t nowNanoseconds) noexcept {
    anchorNs_ = nowNanoseconds;
    accounted_ = 0;
    initialized_ = true;
}

TickBudget::PollResult TickBudget::poll(std::uint64_t nowNanoseconds) noexcept {
    if (!initialized_ || nowNanoseconds < anchorNs_) {
        reset(nowNanoseconds);
        return {};
    }
    constexpr std::uint64_t billion = UINT64_C(1000000000);
    const std::uint64_t elapsed = nowNanoseconds - anchorNs_;
    const std::uint64_t seconds = elapsed / billion;
    const std::uint64_t fraction = elapsed % billion;
    const std::uint64_t h = hz_;
    // Multiplication is bounded: hz_ <= 1000, fraction < 1e9.
    const std::uint64_t max = std::numeric_limits<std::uint64_t>::max();
    const std::uint64_t partial = (fraction * h) / billion;
    std::uint64_t expected = seconds > (max - partial) / h
        ? max
        : seconds * h + partial;
    if (expected <= accounted_) return {};
    const std::uint64_t due = expected - accounted_;
    const auto execute = static_cast<std::uint32_t>(
        std::min<std::uint64_t>(due, maxCatchUp_));
    accounted_ = expected;
    return {execute, due - execute};
}
}} // namespace mmo::portable

#ifndef JX_BISHOP_LOGIN_ATTEMPT_GATE_H
#define JX_BISHOP_LOGIN_ATTEMPT_GATE_H

// One initial login command per live TCP connection/slot.
// Prevents replay of the login opcode from replacing queued credentials
// and restarting CGamePlayer's in-flight login task.
#include <atomic>

class BishopLoginAttemptGate
{
public:
    BishopLoginAttemptGate() : claimed_(false) {}

    bool TryClaim()
    {
        bool expected = false;
        return claimed_.compare_exchange_strong(expected, true,
                    std::memory_order_acq_rel, std::memory_order_acquire);
    }

    void ResetForNewConnection()
    {
        claimed_.store(false, std::memory_order_release);
    }

    bool Claimed() const
    {
        return claimed_.load(std::memory_order_acquire);
    }

private:
    BishopLoginAttemptGate(const BishopLoginAttemptGate&) = delete;
    BishopLoginAttemptGate& operator=(const BishopLoginAttemptGate&) = delete;

    std::atomic<bool> claimed_;
};
#endif

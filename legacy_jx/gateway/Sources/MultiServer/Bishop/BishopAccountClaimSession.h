#ifndef JX_BISHOP_ACCOUNT_CLAIM_SESSION_H
#define JX_BISHOP_ACCOUNT_CLAIM_SESSION_H

// In-process Bishop account admission, supplementary to AccountServer.
// This does not authorize credentials or provide cross-process tickets.
#include <atomic>
#include <cstdint>
#include <map>
#include <mutex>
#include <string>

class BishopAccountClaimRegistry
{
public:
    static BishopAccountClaimRegistry& Instance()
    {
        static BishopAccountClaimRegistry registry;
        return registry;
    }

    static std::string CanonicalAccount(const char* account)
    {
        if (!account) return std::string();
        std::string key;
        for (unsigned int i = 0; i != 64; ++i)
        {
            const unsigned char c = static_cast<unsigned char>(account[i]);
            if (c == 0) return key;
            key.push_back(c >= 'A' && c <= 'Z'
                ? static_cast<char>(c - 'A' + 'a') : static_cast<char>(c));
        }
        // Reject nonterminated/overlong names; do not truncate and collide.
        return std::string();
    }

    std::uint64_t NextToken()
    {
        return tokenCounter_.fetch_add(1, std::memory_order_relaxed);
    }

    bool Claim(const std::string& key, std::uint64_t token)
    {
        if (key.empty() || token == 0) return false;
        std::lock_guard<std::mutex> hold(mutex_);
        auto result = claims_.emplace(key, token);
        return result.second || result.first->second == token;
    }

    bool Release(const std::string& key, std::uint64_t token)
    {
        if (key.empty() || token == 0) return false;
        std::lock_guard<std::mutex> hold(mutex_);
        auto it = claims_.find(key);
        if (it == claims_.end() || it->second != token)
            return false;
        claims_.erase(it);
        return true;
    }

private:
    BishopAccountClaimRegistry() : tokenCounter_(1) {}
    BishopAccountClaimRegistry(const BishopAccountClaimRegistry&) = delete;
    BishopAccountClaimRegistry& operator=(const BishopAccountClaimRegistry&) = delete;

    std::atomic<std::uint64_t> tokenCounter_;
    std::mutex mutex_;
    std::map<std::string, std::uint64_t> claims_;
};

class BishopAccountClaimSession
{
public:
    BishopAccountClaimSession() : token_(0) {}
    ~BishopAccountClaimSession() { End(); }

    void Begin()
    {
        std::lock_guard<std::mutex> hold(mutex_);
        ReleaseLocked();
        token_ = BishopAccountClaimRegistry::Instance().NextToken();
    }

    bool TryReserve(const char* account)
    {
        const std::string key = BishopAccountClaimRegistry::CanonicalAccount(account);
        if (key.empty()) return false;
        std::lock_guard<std::mutex> hold(mutex_);
        if (token_ == 0) return false;
        if (!claimed_.empty()) return claimed_ == key;
        if (!BishopAccountClaimRegistry::Instance().Claim(key, token_))
            return false;
        claimed_ = key;
        return true;
    }

    void ReleaseClaim()
    {
        std::lock_guard<std::mutex> hold(mutex_);
        ReleaseLocked();
    }

    void End()
    {
        std::lock_guard<std::mutex> hold(mutex_);
        ReleaseLocked();
        token_ = 0;
    }

private:
    BishopAccountClaimSession(const BishopAccountClaimSession&) = delete;
    BishopAccountClaimSession& operator=(const BishopAccountClaimSession&) = delete;

    void ReleaseLocked()
    {
        if (!claimed_.empty())
        {
            BishopAccountClaimRegistry::Instance().Release(claimed_, token_);
            claimed_.clear();
        }
    }

    std::mutex mutex_;
    std::uint64_t token_;
    std::string claimed_;
};
#endif

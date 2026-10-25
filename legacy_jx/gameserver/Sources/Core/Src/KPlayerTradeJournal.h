//=============================================================================
// KPlayerTradeJournal.h
// Durable Append-Only Write-Ahead Transaction Journal for Multi-Player Trade Persistence
// Kingsoft JX1 V9 Win64 Port - Phase 3 Remediation Final P0
//=============================================================================

#ifndef KPLAYER_TRADE_JOURNAL_H
#define KPLAYER_TRADE_JOURNAL_H

#include <windows.h>
#include <cstdint>
#include <cstddef>
#include <string>
#include <vector>
#include <set>

#ifndef CORE_API
#ifdef CORE_EXPORTS
#define CORE_API __declspec(dllexport)
#else
#define CORE_API
#endif
#endif

#pragma pack(push, 1)

#define TRADE_JOURNAL_MAGIC   0x54584A31  // 'TXJ1'
#define TRADE_JOURNAL_VERSION 2

enum ETradeTxState : uint32_t {
    TX_STATE_INVALID       = 0,
    TX_STATE_PREPARING     = 1, // WAL written with Pre states of both participants
    TX_STATE_COMMITTED     = 2, // Durable commit point reached! WAL written with Post states
    TX_STATE_APPLIED       = 3, // Both participants saved to database successfully
    TX_STATE_ABORTED       = 4, // Aborted before commit; recovery coordinator will enforce PRE state
    TX_STATE_APPLY_PENDING = 5  // Waiting for Goddess atomic commit confirmation
};

enum ETradeTxRecordType : uint32_t {
    TX_REC_INVALID       = 0,
    TX_REC_PREPARE       = 1, // Durably recorded PRE-trade snapshot
    TX_REC_COMMIT        = 2, // Durably recorded POST-trade snapshot (COMMITTED)
    TX_REC_APPLIED       = 3, // Database confirmed both roles saved (APPLIED)
    TX_REC_ABORT         = 4, // Transaction aborted before commit (ABORTED)
    TX_REC_APPLY_PENDING = 5
};

struct KTradeTxJournalHeader {
    uint32_t dwMagic;            // 0x54584A31
    uint32_t dwVersion;          // 2
    uint32_t dwRecordType;       // ETradeTxRecordType
    uint64_t uTxId;              // Unique monotonic 64-bit Transaction ID
    uint32_t dwState;            // ETradeTxState
    uint64_t uTimestamp;         // Timestamp (FILETIME)
    uint32_t dwPlayerA_Index;
    char     szPlayerA_Name[32];
    uint32_t dwDataLenA;         // preData length in PREPARE, postData length in COMMIT, 0 otherwise
    uint32_t dwPlayerB_Index;
    char     szPlayerB_Name[32];
    uint32_t dwDataLenB;         // preData length in PREPARE, postData length in COMMIT, 0 otherwise
    uint32_t dwPayloadCRC;       // CRC32 of payload (dataA + dataB)
    uint32_t dwHeaderCRC;        // CRC32 of header fields (from dwMagic through dwPayloadCRC)
    // Legacy compatibility fields
    uint32_t dwPreLenA;
    uint32_t dwPostLenA;
    uint32_t dwPreLenB;
    uint32_t dwPostLenB;
};

#pragma pack(pop)

struct KTradeTxJournalEntry {
    KTradeTxJournalHeader header;
    std::vector<uint8_t>  preDataA;
    std::vector<uint8_t>  postDataA;
    std::vector<uint8_t>  preDataB;
    std::vector<uint8_t>  postDataB;
    uint32_t uGenerationA;
    uint32_t uGenerationB;
};

class CORE_API KPlayerTradeJournal {
public:
    static KPlayerTradeJournal& Instance();

    // Configuration
    void SetJournalPath(const char* szPath);
    const char* GetJournalPath() const;

    // Transaction Lifecycle
    // BeginTransaction durably flushes PREPARING record to disk with CRC32.
    // Returns 0 on failure.
    uint64_t BeginTransaction(uint32_t dwPlayerA, const char* szNameA, const void* pPreA, uint32_t dwPreLenA,
                              uint32_t dwPlayerB, const char* szNameB, const void* pPreB, uint32_t dwPreLenB,
                              uint32_t uGenA = 1, uint32_t uGenB = 1);

    bool RecordPostTrade(uint64_t uTxId, const void* pPostA, uint32_t dwPostLenA,
                                          const void* pPostB, uint32_t dwPostLenB);

    // Commit durably flushes COMMIT record to disk with CRC32.
    bool Commit(uint64_t uTxId);
    bool SetApplyPending(uint64_t uTxId);
    bool MarkApplied(uint64_t uTxId);
    bool Abort(uint64_t uTxId);

    // ACK & Lifecycle handling
    bool OnTradePairAck(uint64_t uTxId, int nResultCode);

    // Player lock coordination during recovery
    bool IsPlayerLocked(const char* szPlayerName) const;
    void LockPlayer(const char* szPlayerName);
    void UnlockPlayer(const char* szPlayerName);
    void ClearLockedPlayers();

    // State query
    uint32_t GetTxState(uint64_t uTxId) const;
    bool IsTxProcessed(uint64_t uTxId) const;

    // Crash Recovery
    bool ScanAndRecover(std::vector<KTradeTxJournalEntry>& outPendingCommits,
                        std::vector<KTradeTxJournalEntry>& outAbortedRollbacks);

    // Runtime Lifecycle Recovery Hook
    bool StartupRecovery(void* pDatabaseClient);
    bool DispatchTradePairToGoddess(uint64_t uTxId, void* pDatabaseClient = NULL);
    bool GetTxParticipants(uint64_t uTxId, uint32_t& outIdxA, std::string& outNameA, uint32_t& outIdxB, std::string& outNameB) const;

    // Helpers
    static uint32_t ComputeCRC(const void* pData, size_t nLen);
    static uint64_t GenerateTxId();

private:
    KPlayerTradeJournal();
    ~KPlayerTradeJournal();

    std::string m_strJournalPath;
    mutable CRITICAL_SECTION m_csLock;
    std::set<std::string> m_setLockedPlayers;

    bool AppendRecordToDisk(uint32_t dwRecordType, uint64_t uTxId,
                            uint32_t dwPlayerA, const char* szNameA, const void* pDataA, uint32_t dwLenA,
                            uint32_t dwPlayerB, const char* szNameB, const void* pDataB, uint32_t dwLenB);
};

#endif // KPLAYER_TRADE_JOURNAL_H

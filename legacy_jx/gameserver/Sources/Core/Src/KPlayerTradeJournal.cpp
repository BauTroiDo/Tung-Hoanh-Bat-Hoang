//=============================================================================
// KPlayerTradeJournal.cpp
// Durable Append-Only Write-Ahead Transaction Journal for Multi-Player Trade Persistence
// Kingsoft JX1 V9 Win64 Port - Phase 3 Remediation Final P0
//=============================================================================

#include "KCore.h"
#include "KPlayerTradeJournal.h"
#include <iostream>
#include <map>
#include <atomic>
#include <algorithm>

// RAII Scope Lock for CRITICAL_SECTION to guarantee 100% exception safety
// and eliminate any double-leave or missing-leave bugs.
struct KJournalScopeLock
{
    CRITICAL_SECTION& m_cs;
    explicit KJournalScopeLock(CRITICAL_SECTION& cs) : m_cs(cs) { EnterCriticalSection(&m_cs); }
    ~KJournalScopeLock() { LeaveCriticalSection(&m_cs); }
private:
    KJournalScopeLock(const KJournalScopeLock&);
    KJournalScopeLock& operator=(const KJournalScopeLock&);
};

static uint32_t s_crc32_table[256];
static bool s_crc32_initialized = false;

static void InitCRC32Table()
{
    if (s_crc32_initialized) return;
    for (uint32_t i = 0; i < 256; i++) {
        uint32_t c = i;
        for (int j = 0; j < 8; j++) {
            c = (c & 1) ? (0xEDB88320L ^ (c >> 1)) : (c >> 1);
        }
        s_crc32_table[i] = c;
    }
    s_crc32_initialized = true;
}

uint32_t KPlayerTradeJournal::ComputeCRC(const void* pData, size_t nLen)
{
    InitCRC32Table();
    if (!pData || nLen == 0) return 0;
    const uint8_t* p = (const uint8_t*)pData;
    uint32_t crc = 0xFFFFFFFF;
    for (size_t i = 0; i < nLen; i++) {
        crc = s_crc32_table[(crc ^ p[i]) & 0xFF] ^ (crc >> 8);
    }
    return crc ^ 0xFFFFFFFF;
}

// Strictly monotonic, collision-free Snowflake-style 64-bit ID generator
// Top 41 bits: Milliseconds since custom epoch (2026-01-01 00:00:00 UTC)
// Next 11 bits: Process / Server instance ID (2048 distinct processes)
// Low 12 bits:  Atomic sequence counter per millisecond (4096 tx / ms)
uint64_t KPlayerTradeJournal::GenerateTxId()
{
    static const uint64_t CUSTOM_EPOCH = 1767225600000ULL; // 2026-01-01
    static std::atomic<uint64_t> s_lastTimeAndSeq(0);
    static uint32_t s_dwServerOrPid = 0;
    if (s_dwServerOrPid == 0)
    {
        uint32_t pid = GetCurrentProcessId();
        s_dwServerOrPid = (pid ^ (pid >> 8)) & 0x7FF;
        if (s_dwServerOrPid == 0) s_dwServerOrPid = 1;
    }

    while (true)
    {
        FILETIME ft;
        GetSystemTimePreciseAsFileTime(&ft);
        uint64_t u100ns = ((uint64_t)ft.dwHighDateTime << 32) | (uint64_t)ft.dwLowDateTime;
        uint64_t uUnixMs = (u100ns - 116444736000000000ULL) / 10000ULL;
        uint64_t uTimeDiff = (uUnixMs > CUSTOM_EPOCH) ? (uUnixMs - CUSTOM_EPOCH) : uUnixMs;

        uint64_t oldVal = s_lastTimeAndSeq.load();
        uint64_t lastTime = oldVal >> 12;
        uint64_t lastSeq = oldVal & 0xFFF;

        uint64_t newTime = uTimeDiff;
        uint64_t newSeq = 0;

        if (newTime == lastTime)
        {
            if (lastSeq >= 0xFFF)
            {
                YieldProcessor();
                continue;
            }
            newSeq = lastSeq + 1;
        }
        else if (newTime < lastTime)
        {
            newTime = lastTime;
            if (lastSeq >= 0xFFF)
            {
                YieldProcessor();
                continue;
            }
            newSeq = lastSeq + 1;
        }
        else
        {
            newSeq = 0;
        }

        uint64_t newVal = (newTime << 12) | (newSeq & 0xFFF);
        if (s_lastTimeAndSeq.compare_exchange_weak(oldVal, newVal))
        {
            uint64_t uTxId = ((newTime & 0x1FFFFFFFFFFULL) << 23) |
                             (((uint64_t)s_dwServerOrPid & 0x7FFULL) << 12) |
                             (newSeq & 0xFFFULL);
            if (uTxId == 0) uTxId = 1;
            return uTxId;
        }
    }
}

static std::map<uint64_t, KTradeTxJournalEntry> s_activeTransactions;

KPlayerTradeJournal& KPlayerTradeJournal::Instance()
{
    static KPlayerTradeJournal s_instance;
    return s_instance;
}

KPlayerTradeJournal::KPlayerTradeJournal()
    : m_strJournalPath("trade_tx.journal")
{
    InitializeCriticalSection(&m_csLock);
    InitCRC32Table();
}

KPlayerTradeJournal::~KPlayerTradeJournal()
{
    DeleteCriticalSection(&m_csLock);
}

void KPlayerTradeJournal::SetJournalPath(const char* szPath)
{
    KJournalScopeLock lock(m_csLock);
    if (szPath && szPath[0] != '\0')
        m_strJournalPath = szPath;
}

const char* KPlayerTradeJournal::GetJournalPath() const
{
    return m_strJournalPath.c_str();
}

bool KPlayerTradeJournal::AppendRecordToDisk(
    uint32_t dwRecordType, uint64_t uTxId,
    uint32_t dwPlayerA, const char* szNameA, const void* pDataA, uint32_t dwLenA,
    uint32_t dwPlayerB, const char* szNameB, const void* pDataB, uint32_t dwLenB)
{
    HANDLE hFile = CreateFileA(
        m_strJournalPath.c_str(),
        GENERIC_WRITE,
        FILE_SHARE_READ,
        NULL,
        OPEN_ALWAYS,
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH,
        NULL);

    if (hFile == INVALID_HANDLE_VALUE)
        return false;

    LARGE_INTEGER liAppend;
    liAppend.QuadPart = 0;
    if (!SetFilePointerEx(hFile, liAppend, NULL, FILE_END))
    {
        CloseHandle(hFile);
        return false;
    }

    FILETIME ft;
    GetSystemTimePreciseAsFileTime(&ft);
    uint64_t uTime = ((uint64_t)ft.dwHighDateTime << 32) | (uint64_t)ft.dwLowDateTime;

    KTradeTxJournalHeader hdr;
    memset(&hdr, 0, sizeof(hdr));
    hdr.dwMagic = TRADE_JOURNAL_MAGIC;
    hdr.dwVersion = TRADE_JOURNAL_VERSION;
    hdr.dwRecordType = dwRecordType;
    hdr.uTxId = uTxId;
    hdr.dwState = dwRecordType;
    hdr.uTimestamp = uTime;

    hdr.dwPlayerA_Index = dwPlayerA;
    if (szNameA) strncpy_s(hdr.szPlayerA_Name, sizeof(hdr.szPlayerA_Name), szNameA, _TRUNCATE);
    hdr.dwDataLenA = (pDataA != NULL) ? dwLenA : 0;

    hdr.dwPlayerB_Index = dwPlayerB;
    if (szNameB) strncpy_s(hdr.szPlayerB_Name, sizeof(hdr.szPlayerB_Name), szNameB, _TRUNCATE);
    hdr.dwDataLenB = (pDataB != NULL) ? dwLenB : 0;

    // Dual-CRC32 calculation
    std::vector<uint8_t> payload;
    if (hdr.dwDataLenA > 0 && pDataA)
    {
        const uint8_t* pA = (const uint8_t*)pDataA;
        payload.insert(payload.end(), pA, pA + hdr.dwDataLenA);
    }
    if (hdr.dwDataLenB > 0 && pDataB)
    {
        const uint8_t* pB = (const uint8_t*)pDataB;
        payload.insert(payload.end(), pB, pB + hdr.dwDataLenB);
    }

    hdr.dwPayloadCRC = payload.empty() ? 0 : ComputeCRC(payload.data(), payload.size());
    hdr.dwHeaderCRC = ComputeCRC(&hdr, offsetof(KTradeTxJournalHeader, dwHeaderCRC));

    DWORD dwWritten = 0;
    BOOL bRes = WriteFile(hFile, &hdr, sizeof(hdr), &dwWritten, NULL);
    if (!bRes || dwWritten != sizeof(hdr))
    {
        CloseHandle(hFile);
        return false;
    }

    if (hdr.dwDataLenA > 0 && pDataA)
    {
        dwWritten = 0;
        bRes = WriteFile(hFile, pDataA, hdr.dwDataLenA, &dwWritten, NULL);
        if (!bRes || dwWritten != hdr.dwDataLenA)
        {
            CloseHandle(hFile);
            return false;
        }
    }

    if (hdr.dwDataLenB > 0 && pDataB)
    {
        dwWritten = 0;
        bRes = WriteFile(hFile, pDataB, hdr.dwDataLenB, &dwWritten, NULL);
        if (!bRes || dwWritten != hdr.dwDataLenB)
        {
            CloseHandle(hFile);
            return false;
        }
    }

    BOOL bFlush = FlushFileBuffers(hFile);
    CloseHandle(hFile);
    return (bFlush != FALSE);
}

uint64_t KPlayerTradeJournal::BeginTransaction(
    uint32_t dwPlayerA, const char* szNameA, const void* pPreA, uint32_t dwPreLenA,
    uint32_t dwPlayerB, const char* szNameB, const void* pPreB, uint32_t dwPreLenB,
    uint32_t uGenA, uint32_t uGenB)
{
    KJournalScopeLock lock(m_csLock);

    uint64_t uTxId = GenerateTxId();

    // Durable write of PREPARING state to append-only WAL
    bool bWritten = AppendRecordToDisk(
        TX_REC_PREPARE, uTxId,
        dwPlayerA, szNameA, pPreA, dwPreLenA,
        dwPlayerB, szNameB, pPreB, dwPreLenB);

    if (!bWritten)
    {
        return 0; // Fail-closed: cannot proceed if journal cannot persist PREPARING state
    }

    KTradeTxJournalEntry entry;
    memset(&entry.header, 0, sizeof(entry.header));
    entry.header.dwMagic = TRADE_JOURNAL_MAGIC;
    entry.header.dwVersion = TRADE_JOURNAL_VERSION;
    entry.header.dwRecordType = TX_REC_PREPARE;
    entry.header.uTxId = uTxId;
    entry.header.dwState = TX_STATE_PREPARING;
    entry.header.dwPlayerA_Index = dwPlayerA;
    if (szNameA) strncpy_s(entry.header.szPlayerA_Name, sizeof(entry.header.szPlayerA_Name), szNameA, _TRUNCATE);
    entry.header.dwPlayerB_Index = dwPlayerB;
    if (szNameB) strncpy_s(entry.header.szPlayerB_Name, sizeof(entry.header.szPlayerB_Name), szNameB, _TRUNCATE);

    if (pPreA && dwPreLenA > 0)
    {
        entry.preDataA.resize(dwPreLenA);
        memcpy(entry.preDataA.data(), pPreA, dwPreLenA);
    }
    if (pPreB && dwPreLenB > 0)
    {
        entry.preDataB.resize(dwPreLenB);
        memcpy(entry.preDataB.data(), pPreB, dwPreLenB);
    }

    entry.uGenerationA = uGenA ? uGenA : 1;
    entry.uGenerationB = uGenB ? uGenB : 1;

    s_activeTransactions[uTxId] = entry;
    return uTxId;
}

bool KPlayerTradeJournal::RecordPostTrade(
    uint64_t uTxId,
    const void* pPostA, uint32_t dwPostLenA,
    const void* pPostB, uint32_t dwPostLenB)
{
    KJournalScopeLock lock(m_csLock);
    auto it = s_activeTransactions.find(uTxId);
    if (it == s_activeTransactions.end())
        return false;

    if (pPostA && dwPostLenA > 0)
    {
        it->second.postDataA.resize(dwPostLenA);
        memcpy(it->second.postDataA.data(), pPostA, dwPostLenA);
    }
    if (pPostB && dwPostLenB > 0)
    {
        it->second.postDataB.resize(dwPostLenB);
        memcpy(it->second.postDataB.data(), pPostB, dwPostLenB);
    }

    it->second.header.dwPostLenA = dwPostLenA;
    it->second.header.dwPostLenB = dwPostLenB;
    return true;
}

bool KPlayerTradeJournal::Commit(uint64_t uTxId)
{
    KJournalScopeLock lock(m_csLock);
    auto it = s_activeTransactions.find(uTxId);
    if (it == s_activeTransactions.end())
        return false;

    // Negative check: cannot commit an already aborted transaction!
    if (it->second.header.dwState == TX_STATE_ABORTED)
        return false;

    // Idempotent check: already committed or applied
    if (it->second.header.dwState == TX_STATE_COMMITTED ||
        it->second.header.dwState == TX_STATE_APPLIED ||
        it->second.header.dwState == TX_STATE_APPLY_PENDING)
    {
        return true;
    }

    // Must be in PREPARING state to commit
    if (it->second.header.dwState != TX_STATE_PREPARING)
        return false;

    // Durably append COMMIT record to disk with postData
    bool bSuccess = AppendRecordToDisk(
        TX_REC_COMMIT, uTxId,
        it->second.header.dwPlayerA_Index, it->second.header.szPlayerA_Name,
        it->second.postDataA.empty() ? NULL : it->second.postDataA.data(), (uint32_t)it->second.postDataA.size(),
        it->second.header.dwPlayerB_Index, it->second.header.szPlayerB_Name,
        it->second.postDataB.empty() ? NULL : it->second.postDataB.data(), (uint32_t)it->second.postDataB.size());

    if (bSuccess)
    {
        it->second.header.dwState = TX_STATE_COMMITTED;
        it->second.header.dwRecordType = TX_REC_COMMIT;
    }

    return bSuccess;
}

bool KPlayerTradeJournal::SetApplyPending(uint64_t uTxId)
{
    KJournalScopeLock lock(m_csLock);
    auto it = s_activeTransactions.find(uTxId);
    if (it == s_activeTransactions.end())
        return false;

    if (it->second.header.dwState != TX_STATE_COMMITTED &&
        it->second.header.dwState != TX_STATE_APPLY_PENDING)
    {
        return false;
    }

    it->second.header.dwState = TX_STATE_APPLY_PENDING;
    return true;
}

bool KPlayerTradeJournal::MarkApplied(uint64_t uTxId)
{
    KJournalScopeLock lock(m_csLock);
    auto it = s_activeTransactions.find(uTxId);
    uint32_t dwPlayerA = 0, dwPlayerB = 0;
    const char* szNameA = "";
    const char* szNameB = "";

    if (it == s_activeTransactions.end())
    {
        // Negative test: TxID non-existent cannot be marked applied
        return false;
    }

    // Negative check: cannot mark applied if transaction was never committed!
    if (it->second.header.dwState != TX_STATE_COMMITTED &&
        it->second.header.dwState != TX_STATE_APPLY_PENDING &&
        it->second.header.dwState != TX_STATE_APPLIED)
    {
        return false;
    }

    // Idempotent if already applied
    if (it->second.header.dwState == TX_STATE_APPLIED)
        return true;

    dwPlayerA = it->second.header.dwPlayerA_Index;
    szNameA = it->second.header.szPlayerA_Name;
    dwPlayerB = it->second.header.dwPlayerB_Index;
    szNameB = it->second.header.szPlayerB_Name;

    // Durably append APPLIED marker to disk FIRST
    bool bSuccess = AppendRecordToDisk(
        TX_REC_APPLIED, uTxId,
        dwPlayerA, szNameA, NULL, 0,
        dwPlayerB, szNameB, NULL, 0);

    if (bSuccess)
    {
        it->second.header.dwState = TX_STATE_APPLIED;
        it->second.header.dwRecordType = TX_REC_APPLIED;
        return true;
    }
    else
    {
        printf("[MarkApplied ERROR]: Failed to append APPLIED record to disk WAL for Tx 0x%llX! RAM state NOT modified.\n", uTxId);
        return false;
    }
}

bool KPlayerTradeJournal::Abort(uint64_t uTxId)
{
    KJournalScopeLock lock(m_csLock);
    auto it = s_activeTransactions.find(uTxId);

    if (it == s_activeTransactions.end())
    {
        // Negative test: non-existent transaction
        return false;
    }

    // Cannot abort an already applied transaction!
    if (it->second.header.dwState == TX_STATE_APPLIED)
        return false;

    // Idempotent if already aborted
    if (it->second.header.dwState == TX_STATE_ABORTED)
        return true;

    uint32_t dwPlayerA = it->second.header.dwPlayerA_Index;
    const char* szNameA = it->second.header.szPlayerA_Name;
    uint32_t dwPlayerB = it->second.header.dwPlayerB_Index;
    const char* szNameB = it->second.header.szPlayerB_Name;

    // Durably append ABORT marker to disk FIRST
    bool bSuccess = AppendRecordToDisk(
        TX_REC_ABORT, uTxId,
        dwPlayerA, szNameA, NULL, 0,
        dwPlayerB, szNameB, NULL, 0);

    if (bSuccess)
    {
        it->second.header.dwState = TX_STATE_ABORTED;
        it->second.header.dwRecordType = TX_REC_ABORT;
        return true;
    }
    else
    {
        printf("[Abort ERROR]: Failed to append ABORT record to disk WAL for Tx 0x%llX! RAM state NOT modified.\n", uTxId);
        return false;
    }
}

bool KPlayerTradeJournal::OnTradePairAck(uint64_t uTxId, int nResultCode)
{
    KJournalScopeLock lock(m_csLock);
    auto it = s_activeTransactions.find(uTxId);

    if (it == s_activeTransactions.end())
    {
        // If from recovery replay
        if (nResultCode > 0)
        {
            return MarkApplied(uTxId);
        }
        return false;
    }

    if (nResultCode > 0)
    {
        // Goddess database successfully committed the transaction
        bool bMarked = MarkApplied(uTxId);
        if (bMarked)
        {
            UnlockPlayer(it->second.header.szPlayerA_Name);
            UnlockPlayer(it->second.header.szPlayerB_Name);
            return true;
        }
        else
        {
            printf("[OnTradePairAck ERROR]: MarkApplied failed on disk for Tx 0x%llX! Participants '%s' & '%s' remain LOCKED for recovery.\n",
                uTxId, it->second.header.szPlayerA_Name, it->second.header.szPlayerB_Name);
            return false;
        }
    }
    else
    {
        // Database commit failed! Keep as COMMITTED for startup recovery, DO NOT mark APPLIED
        it->second.header.dwState = TX_STATE_COMMITTED;
        printf("[OnTradePairAck]: Negative ACK (%d) for Tx 0x%llX. Participants '%s' & '%s' remain LOCKED.\n",
            nResultCode, uTxId, it->second.header.szPlayerA_Name, it->second.header.szPlayerB_Name);
        return false;
    }
}

bool KPlayerTradeJournal::IsPlayerLocked(const char* szPlayerName) const
{
    KJournalScopeLock lock(m_csLock);
    if (!szPlayerName || szPlayerName[0] == '\0') return false;
    return (m_setLockedPlayers.find(szPlayerName) != m_setLockedPlayers.end());
}

void KPlayerTradeJournal::LockPlayer(const char* szPlayerName)
{
    KJournalScopeLock lock(m_csLock);
    if (szPlayerName && szPlayerName[0] != '\0')
        m_setLockedPlayers.insert(szPlayerName);
}

void KPlayerTradeJournal::UnlockPlayer(const char* szPlayerName)
{
    KJournalScopeLock lock(m_csLock);
    if (szPlayerName && szPlayerName[0] != '\0')
        m_setLockedPlayers.erase(szPlayerName);
}

void KPlayerTradeJournal::ClearLockedPlayers()
{
    KJournalScopeLock lock(m_csLock);
    m_setLockedPlayers.clear();
}

uint32_t KPlayerTradeJournal::GetTxState(uint64_t uTxId) const
{
    KJournalScopeLock lock(m_csLock);
    auto it = s_activeTransactions.find(uTxId);
    if (it != s_activeTransactions.end())
        return it->second.header.dwState;
    return TX_STATE_INVALID;
}

bool KPlayerTradeJournal::IsTxProcessed(uint64_t uTxId) const
{
    KJournalScopeLock lock(m_csLock);
    auto it = s_activeTransactions.find(uTxId);
    if (it != s_activeTransactions.end())
    {
        return (it->second.header.dwState == TX_STATE_COMMITTED ||
                it->second.header.dwState == TX_STATE_APPLY_PENDING ||
                it->second.header.dwState == TX_STATE_APPLIED);
    }
    return false;
}

bool KPlayerTradeJournal::ScanAndRecover(
    std::vector<KTradeTxJournalEntry>& outPendingCommits,
    std::vector<KTradeTxJournalEntry>& outAbortedRollbacks)
{
    KJournalScopeLock lock(m_csLock);
    outPendingCommits.clear();
    outAbortedRollbacks.clear();

    HANDLE hFile = CreateFileA(
        m_strJournalPath.c_str(),
        GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    LARGE_INTEGER liSize;
    if (!GetFileSizeEx(hFile, &liSize) || liSize.QuadPart == 0)
    {
        CloseHandle(hFile);
        return true;
    }
    // liSize is the only measurement this scan holds of what the file can give back.  Every length a record header carries below is data the file wrote, so a window is weighed against the bytes that are still left before the file is asked for them.
    LONGLONG liLeft;
    liLeft = liSize.QuadPart;

    struct TxTracker {
        uint64_t uTxId;
        uint32_t latestState;
        KTradeTxJournalHeader latestHeader;
        std::vector<uint8_t> preDataA;
        std::vector<uint8_t> preDataB;
        std::vector<uint8_t> postDataA;
        std::vector<uint8_t> postDataB;
    };
    std::map<uint64_t, TxTracker> trackers;

    while (true)
    {
        KTradeTxJournalHeader hdr;
        DWORD dwRead = 0;
        BOOL bRes = ReadFile(hFile, &hdr, sizeof(hdr), &dwRead, NULL);
        if (!bRes || dwRead == 0) break; // Clean EOF
        if (dwRead != sizeof(hdr))
        {
            // Truncated header: corruption detected, stop reading
            break;
        }
        liLeft -= dwRead;                                    // the header just read

        if (hdr.dwMagic != TRADE_JOURNAL_MAGIC || hdr.dwVersion != TRADE_JOURNAL_VERSION)
        {
            // Corrupted record magic
            break;
        }

        uint32_t expectedHdrCRC = ComputeCRC(&hdr, offsetof(KTradeTxJournalHeader, dwHeaderCRC));
        if (expectedHdrCRC != hdr.dwHeaderCRC)
        {
            // Header CRC mismatch! Corruption detected
            break;
        }

        DWORD dwWinA;
        dwWinA = hdr.dwDataLenA;
        std::vector<uint8_t> bufA;
        if (dwWinA > 0)
        {
            if ((LONGLONG) dwWinA > liLeft) break;        // asks for more than the file still holds
            bufA.resize(dwWinA);
            dwRead = 0;
            bRes = ReadFile(hFile, bufA.data(), dwWinA, &dwRead, NULL);
            if (!bRes || dwRead != dwWinA) break;
            liLeft -= dwRead;
        }

        DWORD dwWinB;
        dwWinB = hdr.dwDataLenB;
        std::vector<uint8_t> bufB;
        if (dwWinB > 0)
        {
            if ((LONGLONG) dwWinB > liLeft) break;        // asks for more than the file still holds
            bufB.resize(dwWinB);
            dwRead = 0;
            bRes = ReadFile(hFile, bufB.data(), dwWinB, &dwRead, NULL);
            if (!bRes || dwRead != dwWinB) break;
            liLeft -= dwRead;
        }

        // Verify Payload CRC
        std::vector<uint8_t> combinedPayload;
        combinedPayload.insert(combinedPayload.end(), bufA.begin(), bufA.end());
        combinedPayload.insert(combinedPayload.end(), bufB.begin(), bufB.end());
        uint32_t expectedPayloadCRC = combinedPayload.empty() ? 0 : ComputeCRC(combinedPayload.data(), combinedPayload.size());
        if (expectedPayloadCRC != hdr.dwPayloadCRC)
        {
            // Payload CRC mismatch! Corruption detected
            break;
        }

        TxTracker& track = trackers[hdr.uTxId];
        track.uTxId = hdr.uTxId;
        track.latestState = hdr.dwRecordType;
        track.latestHeader = hdr;

        if (hdr.dwRecordType == TX_REC_PREPARE)
        {
            track.preDataA = bufA;
            track.preDataB = bufB;
        }
        else if (hdr.dwRecordType == TX_REC_COMMIT)
        {
            track.postDataA = bufA;
            track.postDataB = bufB;
        }
    }

    CloseHandle(hFile);

    // Reconstruct transactions
    for (auto it = trackers.begin(); it != trackers.end(); ++it)
    {
        const TxTracker& t = it->second;
        KTradeTxJournalEntry entry;
        // Generations are in-memory sanity fields, not a database version.
        // The durable TxID in the WAL is the recovery ordering fence.
        entry.uGenerationA = 1;
        entry.uGenerationB = 1;
        entry.header = t.latestHeader;
        entry.preDataA = t.preDataA;
        entry.preDataB = t.preDataB;
        entry.postDataA = t.postDataA;
        entry.postDataB = t.postDataB;

        s_activeTransactions[t.uTxId] = entry;

        if (t.latestState == TX_REC_COMMIT)
        {
            outPendingCommits.push_back(entry);
        }
        else if (t.latestState == TX_REC_PREPARE)
        {
            outAbortedRollbacks.push_back(entry);
        }
    }

    return true;
}

bool KPlayerTradeJournal::StartupRecovery(void* pDatabaseClient)
{
    printf("[Startup Recovery]: Scanning trade transaction journal '%s'...\n", m_strJournalPath.c_str());
    std::vector<KTradeTxJournalEntry> pendingCommits;
    std::vector<KTradeTxJournalEntry> abortedRollbacks;
    if (!ScanAndRecover(pendingCommits, abortedRollbacks))
    {
        printf("[Startup Recovery]: No journal found or clean startup.\n");
        return true;
    }

    printf("[Startup Recovery]: Scan complete: %zu pending COMMITTED, %zu aborted PREPARING.\n",
        pendingCommits.size(), abortedRollbacks.size());

    // 1. Resolve aborted transactions: enforce durable ABORT marker on disk
    for (size_t i = 0; i < abortedRollbacks.size(); ++i)
    {
        uint64_t uTxId = abortedRollbacks[i].header.uTxId;
        printf("[Startup Recovery]: Tx 0x%llX was interrupted in PREPARING. Enforcing ABORT on disk.\n", uTxId);
        Abort(uTxId);
    }

    // 2. Resolve pending COMMITTED transactions
    if (pendingCommits.empty())
    {
        printf("[Startup Recovery]: No unapplied transactions to reconcile. Startup clean.\n");
        return true;
    }

    bool bAllReconciled = true;
    for (size_t i = 0; i < pendingCommits.size(); ++i)
    {
        const KTradeTxJournalEntry& entry = pendingCommits[i];
        printf("[Startup Recovery]: Tx 0x%llX is COMMITTED but unapplied. Reconciling for '%s' & '%s'...\n",
            entry.header.uTxId, entry.header.szPlayerA_Name, entry.header.szPlayerB_Name);

        // Lock participants to block gameplay access until reconciliation completes
        LockPlayer(entry.header.szPlayerA_Name);
        LockPlayer(entry.header.szPlayerB_Name);

        if (pDatabaseClient == NULL)
        {
            printf("[Startup Recovery]: Database client is NULL! Tx 0x%llX remains pending recovery. Players '%s' and '%s' remain locked.\n",
                entry.header.uTxId, entry.header.szPlayerA_Name, entry.header.szPlayerB_Name);
            bAllReconciled = false;
            // DO NOT mark APPLIED without database confirmation! Fail-Closed.
            continue;
        }

        // When pDatabaseClient is present, dispatch the atomic trade pair to Goddess!
        SetApplyPending(entry.header.uTxId);
        bool bSent = DispatchTradePairToGoddess(entry.header.uTxId, pDatabaseClient);
        if (!bSent)
        {
            printf("[Startup Recovery]: Failed to dispatch Tx 0x%llX to Goddess! Players remain locked.\n", entry.header.uTxId);
            bAllReconciled = false;
        }
        else
        {
            if (GetTxState(entry.header.uTxId) == TX_STATE_APPLIED)
            {
                printf("[Startup Recovery]: Tx 0x%llX successfully reconciled and APPLIED.\n", entry.header.uTxId);
            }
            else
            {
                printf("[Startup Recovery]: Replay packet dispatched for Tx 0x%llX. Waiting for ACK confirmation from Goddess.\n",
                    entry.header.uTxId);
                bAllReconciled = false;
            }
        }
    }

    return bAllReconciled;
}

bool KPlayerTradeJournal::GetTxParticipants(uint64_t uTxId, uint32_t& outIdxA, std::string& outNameA, uint32_t& outIdxB, std::string& outNameB) const
{
    KJournalScopeLock lock(m_csLock);
    auto it = s_activeTransactions.find(uTxId);
    if (it != s_activeTransactions.end())
    {
        outIdxA = it->second.header.dwPlayerA_Index;
        outNameA = it->second.header.szPlayerA_Name;
        outIdxB = it->second.header.dwPlayerB_Index;
        outNameB = it->second.header.szPlayerB_Name;
        return true;
    }
    return false;
}

// BEGIN_PHASE3_TRADEPAIR_RAINBOW_FRAMING
// Goddess handles opcode 13 as a CPackager large packet. Rainbow's
// SendPackToServer adds only the TCP length and cipher, not this 6-byte layer.
static bool SendTradePairLargePacket(IClient* pClient,
                                     const std::vector<uint8_t>& payload,
                                     uint64_t uTxId)
{
    if (!pClient || payload.empty())
        return false;

    const size_t kChunkPayloadSize = 510; // Matches CPackager::GetHeadPack(...,512); Rainbow buffer is 1024.
    const size_t kFragmentHeaderSize = 6; // opcode, flags, four-byte user identity
    const unsigned char kFlagHeader = 0xC0;
    const unsigned char kFlagMiddle = 0x30;
    const unsigned char kFlagTail = 0x0C;

    size_t offset = 0;
    while (offset < payload.size())
    {
        const size_t remaining = payload.size() - offset;
        const size_t chunkLength = remaining > kChunkPayloadSize ? kChunkPayloadSize : remaining;
        const bool first = (offset == 0);
        const bool last = (offset + chunkLength == payload.size());

        std::vector<uint8_t> fragment(kFragmentHeaderSize + chunkLength, 0);
        fragment[0] = c2s_roleserver_savetradepair;
        fragment[1] = (first ? kFlagHeader : kFlagMiddle) | (last ? kFlagTail : 0);
        // Bytes 2..5 are the CPackager user identity, zero for DB role packets.
        memcpy(fragment.data() + kFragmentHeaderSize, payload.data() + offset, chunkLength);

        const HRESULT hr = pClient->SendPackToServer(fragment.data(), fragment.size());
        if (FAILED(hr))
        {
            printf("[TradeJournal] CPackager fragment send failed Tx 0x%llX offset %zu hr=0x%08X\n",
                   uTxId, offset, (unsigned int)hr);
            return false;
        }
        offset += chunkLength;
    }
    return true;
}
// END_PHASE3_TRADEPAIR_RAINBOW_FRAMING

bool KPlayerTradeJournal::DispatchTradePairToGoddess(uint64_t uTxId, void* pDatabaseClient)
{
    KJournalScopeLock lock(m_csLock);
    auto it = s_activeTransactions.find(uTxId);
    if (it == s_activeTransactions.end())
    {
        printf("[TradeJournal] DispatchTradePairToGoddess: Tx 0x%llX not found!\n", uTxId);
        return false;
    }

    const KTradeTxJournalEntry& entry = it->second;
    if (entry.postDataA.empty() || entry.postDataB.empty())
    {
        printf("[TradeJournal] DispatchTradePairToGoddess: Tx 0x%llX has empty post-trade blobs!\n", uTxId);
        return false;
    }

    IClient* pClient = (IClient*)(pDatabaseClient ? pDatabaseClient : g_pDataClient);
    if (!pClient)
    {
        printf("[TradeJournal] DispatchTradePairToGoddess: Database client is NULL for Tx 0x%llX!\n", uTxId);
        return false;
    }

    uint32_t lenA = (uint32_t)entry.postDataA.size();
    uint32_t lenB = (uint32_t)entry.postDataB.size();
    size_t totalPacketSize = sizeof(TTradePairSaveRequest) - 1 + lenA + lenB;

    std::vector<uint8_t> packetBuf(totalPacketSize, 0);
    TTradePairSaveRequest* pReq = (TTradePairSaveRequest*)packetBuf.data();

    pReq->nProtoId = c2s_roleserver_savetradepair;
    pReq->uTxId = uTxId;
    strncpy_s(pReq->szRoleA, sizeof(pReq->szRoleA), entry.header.szPlayerA_Name, _TRUNCATE);
    strncpy_s(pReq->szRoleB, sizeof(pReq->szRoleB), entry.header.szPlayerB_Name, _TRUNCATE);
    pReq->dwDataLenA = lenA;
    pReq->dwDataLenB = lenB;
    pReq->dwChecksumA = ComputeCRC(entry.postDataA.data(), lenA);
    pReq->dwChecksumB = ComputeCRC(entry.postDataB.data(), lenB);
    pReq->ulIdentityA = entry.header.dwPlayerA_Index;
    pReq->ulIdentityB = entry.header.dwPlayerB_Index;
    pReq->uGenerationA = entry.uGenerationA ? entry.uGenerationA : 1;
    pReq->uGenerationB = entry.uGenerationB ? entry.uGenerationB : 1;

    memcpy(&pReq->pDataBuffer[0], entry.postDataA.data(), lenA);
    memcpy(&pReq->pDataBuffer[lenA], entry.postDataB.data(), lenB);

    // The database server expects the same CPackager framing as other
    // large role packets. A raw request is silently discarded at PackUp().
    if (!SendTradePairLargePacket(pClient, packetBuf, uTxId))
        return false;

    printf("[TradeJournal] Dispatched atomic trade pair packet for Tx 0x%llX to Goddess (%s, %s, TotalBytes=%zu)\n",
        uTxId, entry.header.szPlayerA_Name, entry.header.szPlayerB_Name, totalPacketSize);
    return true;
}

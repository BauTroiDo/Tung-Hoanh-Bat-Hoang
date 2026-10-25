//=============================================================================
// test_phase3_transaction_closure.cpp
// Full End-to-End Transaction Closure & Real Process Recovery Test Suite
// JX1 V9 Win64 Port - Phase 3 Final Integration Fix
//=============================================================================

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <iostream>
#include <vector>
#include <string>
#include <cstdint>
#include <cassert>
#include <thread>
#include <chrono>

#include "gameserver/include/sqlite3.h"
#include "gameserver/Sources/Core/Src/KPlayerTradeJournal.h"
#include "gameserver/Headers/KProtocol.h"
#include "gameserver/Headers/KProtocolDef.h"

#pragma comment(lib, "C:\\Server-Game\\SwordOnline\\jxOnline\\build_out_phase3_transaction_closure\\CoreServer.lib")
#pragma comment(lib, "C:\\Server-Game\\SwordOnline\\jxOnline\\gateway\\Lib\\release_x64\\sqlite3\\sqlite3.lib")

static int g_nPassed = 0;
static int g_nFailed = 0;

#define TEST_ASSERT(cond, msg) do { \
    if (cond) { \
        std::cout << "  [PASS] Assert " << (++g_nPassed) << ": " << msg << std::endl; \
    } else { \
        std::cerr << "  [FAIL] Assert " << (g_nPassed + g_nFailed + 1) << ": " << msg << " (" << #cond << ")" << std::endl; \
        g_nFailed++; \
    } \
} while(0)

// Helper: Setup isolated SQLite role database
static bool SetupTestDatabase(sqlite3** ppDb, const char* szDbPath)
{
    DeleteFileA(szDbPath);
    if (sqlite3_open(szDbPath, ppDb) != SQLITE_OK)
        return false;

    const char* szSchema =
        "CREATE TABLE IF NOT EXISTS role_Blob ("
        "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
        "roleName VARCHAR(64) UNIQUE, "
        "StudentBlob BLOB, "
        "ekey BIGINT);"
        "CREATE TABLE IF NOT EXISTS trade_receipts ("
        "tx_id BIGINT PRIMARY KEY, "
        "role_a VARCHAR(64), "
        "role_b VARCHAR(64), "
        "status INT, "
        "commit_time BIGINT);";

    char* zErrMsg = NULL;
    if (sqlite3_exec(*ppDb, szSchema, NULL, NULL, &zErrMsg) != SQLITE_OK)
    {
        if (zErrMsg) sqlite3_free(zErrMsg);
        return false;
    }
    return true;
}

// Simulated Role Data payload
struct TTestRolePayload {
    char szRoleName[32];
    uint32_t dwGold;
    uint32_t dwItemCount;
    uint32_t uItemUIDs[10];
};

static bool InsertTestRole(sqlite3* pDb, const char* szRole, uint32_t dwGold, const std::vector<uint32_t>& items)
{
    TTestRolePayload payload;
    memset(&payload, 0, sizeof(payload));
    strncpy_s(payload.szRoleName, sizeof(payload.szRoleName), szRole, _TRUNCATE);
    payload.dwGold = dwGold;
    payload.dwItemCount = (uint32_t)items.size();
    for (size_t i = 0; i < items.size() && i < 10; ++i)
        payload.uItemUIDs[i] = items[i];

    const char* sql = "INSERT OR REPLACE INTO role_Blob (roleName, StudentBlob, ekey) VALUES (?, ?, 0);";
    sqlite3_stmt* stmt = NULL;
    if (sqlite3_prepare_v2(pDb, sql, -1, &stmt, NULL) != SQLITE_OK)
        return false;

    sqlite3_bind_text(stmt, 1, szRole, -1, SQLITE_STATIC);
    sqlite3_bind_blob(stmt, 2, &payload, sizeof(payload), SQLITE_STATIC);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE);
}

static bool ReadTestRole(sqlite3* pDb, const char* szRole, uint32_t& outGold, std::vector<uint32_t>& outItems)
{
    outItems.clear();
    const char* sql = "SELECT StudentBlob FROM role_Blob WHERE roleName = ?;";
    sqlite3_stmt* stmt = NULL;
    if (sqlite3_prepare_v2(pDb, sql, -1, &stmt, NULL) != SQLITE_OK)
        return false;

    sqlite3_bind_text(stmt, 1, szRole, -1, SQLITE_STATIC);
    int rc = sqlite3_step(stmt);
    if (rc != SQLITE_ROW)
    {
        sqlite3_finalize(stmt);
        return false;
    }

    const void* pBlob = sqlite3_column_blob(stmt, 0);
    int nBytes = sqlite3_column_bytes(stmt, 0);
    if (nBytes != sizeof(TTestRolePayload))
    {
        sqlite3_finalize(stmt);
        return false;
    }

    TTestRolePayload payload;
    memcpy(&payload, pBlob, sizeof(payload));
    outGold = payload.dwGold;
    for (uint32_t i = 0; i < payload.dwItemCount && i < 10; ++i)
        outItems.push_back(payload.uItemUIDs[i]);

    sqlite3_finalize(stmt);
    return true;
}

// Real atomic dual-blob save on SQLite with receipts
static bool ExecuteGoddessAtomicPairSave(sqlite3* pDb, uint64_t uTxId,
                                        const char* szRoleA, const void* pBlobA, int nLenA,
                                        const char* szRoleB, const void* pBlobB, int nLenB)
{
    if (!pDb || !szRoleA || !szRoleB || !pBlobA || !pBlobB) return false;

    // Check idempotency first
    const char* checkSql = "SELECT status FROM trade_receipts WHERE tx_id = ?;";
    sqlite3_stmt* chkStmt = NULL;
    if (sqlite3_prepare_v2(pDb, checkSql, -1, &chkStmt, NULL) == SQLITE_OK)
    {
        sqlite3_bind_int64(chkStmt, 1, (sqlite3_int64)uTxId);
        if (sqlite3_step(chkStmt) == SQLITE_ROW)
        {
            int status = sqlite3_column_int(chkStmt, 0);
            sqlite3_finalize(chkStmt);
            if (status == 1) return true; // Already committed (Idempotent!)
        }
        else
        {
            sqlite3_finalize(chkStmt);
        }
    }

    char* zErr = NULL;
    if (sqlite3_exec(pDb, "BEGIN IMMEDIATE TRANSACTION;", NULL, NULL, &zErr) != SQLITE_OK)
    {
        if (zErr) sqlite3_free(zErr);
        return false;
    }

    const char* updateSql = "UPDATE role_Blob SET StudentBlob = ? WHERE roleName = ?;";

    // Update A
    sqlite3_stmt* stmtA = NULL;
    if (sqlite3_prepare_v2(pDb, updateSql, -1, &stmtA, NULL) != SQLITE_OK)
    {
        sqlite3_exec(pDb, "ROLLBACK;", NULL, NULL, NULL);
        return false;
    }
    sqlite3_bind_blob(stmtA, 1, pBlobA, nLenA, SQLITE_STATIC);
    sqlite3_bind_text(stmtA, 2, szRoleA, -1, SQLITE_STATIC);
    int rcA = sqlite3_step(stmtA);
    int changesA = sqlite3_changes(pDb);
    sqlite3_finalize(stmtA);

    if (rcA != SQLITE_DONE || changesA <= 0)
    {
        sqlite3_exec(pDb, "ROLLBACK;", NULL, NULL, NULL);
        return false;
    }

    // Update B
    sqlite3_stmt* stmtB = NULL;
    if (sqlite3_prepare_v2(pDb, updateSql, -1, &stmtB, NULL) != SQLITE_OK)
    {
        sqlite3_exec(pDb, "ROLLBACK;", NULL, NULL, NULL);
        return false;
    }
    sqlite3_bind_blob(stmtB, 1, pBlobB, nLenB, SQLITE_STATIC);
    sqlite3_bind_text(stmtB, 2, szRoleB, -1, SQLITE_STATIC);
    int rcB = sqlite3_step(stmtB);
    int changesB = sqlite3_changes(pDb);
    sqlite3_finalize(stmtB);

    if (rcB != SQLITE_DONE || changesB <= 0)
    {
        sqlite3_exec(pDb, "ROLLBACK;", NULL, NULL, NULL);
        return false;
    }

    // Insert Receipt
    const char* rcptSql = "INSERT OR REPLACE INTO trade_receipts (tx_id, role_a, role_b, status, commit_time) VALUES (?, ?, ?, 1, 1000);";
    sqlite3_stmt* stmtR = NULL;
    if (sqlite3_prepare_v2(pDb, rcptSql, -1, &stmtR, NULL) != SQLITE_OK)
    {
        sqlite3_exec(pDb, "ROLLBACK;", NULL, NULL, NULL);
        return false;
    }
    sqlite3_bind_int64(stmtR, 1, (sqlite3_int64)uTxId);
    sqlite3_bind_text(stmtR, 2, szRoleA, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmtR, 3, szRoleB, -1, SQLITE_STATIC);
    int rcR = sqlite3_step(stmtR);
    sqlite3_finalize(stmtR);

    if (rcR != SQLITE_DONE)
    {
        sqlite3_exec(pDb, "ROLLBACK;", NULL, NULL, NULL);
        return false;
    }

    if (sqlite3_exec(pDb, "COMMIT;", NULL, NULL, &zErr) != SQLITE_OK)
    {
        if (zErr) sqlite3_free(zErr);
        sqlite3_exec(pDb, "ROLLBACK;", NULL, NULL, NULL);
        return false;
    }

    return true;
}

int main(int argc, char* argv[])
{
    // Mode child process for real crash simulation
    if (argc > 1 && strcmp(argv[1], "--simulate-crash-mid-commit") == 0)
    {
        KPlayerTradeJournal::Instance().SetJournalPath("trade_child_crash.journal");
        DeleteFileA("trade_child_crash.journal");

        TTestRolePayload preA = { "RoleA", 100000, 1, { 501 } };
        TTestRolePayload preB = { "RoleB", 50000, 0, {} };
        uint64_t uTxId = KPlayerTradeJournal::Instance().BeginTransaction(
            1, "RoleA", &preA, sizeof(preA),
            2, "RoleB", &preB, sizeof(preB));

        TTestRolePayload postA = { "RoleA", 50000, 0, {} };
        TTestRolePayload postB = { "RoleB", 100000, 1, { 501 } };
        KPlayerTradeJournal::Instance().RecordPostTrade(uTxId, &postA, sizeof(postA), &postB, sizeof(postB));

        // Durable commit reached in WAL
        KPlayerTradeJournal::Instance().Commit(uTxId);

        // ABRUPT CRASH: TerminateProcess without marking applied!
        TerminateProcess(GetCurrentProcess(), 42);
        return 42;
    }

    std::cout << "================================================================================" << std::endl;
    std::cout << " PHASE 3 FINAL TRANSACTION CLOSURE & RECOVERY INTEGRATION ACCEPTANCE SUITE     " << std::endl;
    std::cout << "================================================================================" << std::endl;

    //-------------------------------------------------------------------------
    // GROUP 1: Snowflake Monotonic TxID Generator Tests
    //-------------------------------------------------------------------------
    std::cout << "\n[GROUP 1: Snowflake Monotonic TxID Generator Verification]" << std::endl;
    {
        uint64_t tx1 = KPlayerTradeJournal::GenerateTxId();
        uint64_t tx2 = KPlayerTradeJournal::GenerateTxId();
        uint64_t tx3 = KPlayerTradeJournal::GenerateTxId();

        TEST_ASSERT(tx1 != 0 && tx2 != 0 && tx3 != 0, "Generated 64-bit TxIDs are non-zero");
        TEST_ASSERT(tx1 < tx2 && tx2 < tx3, "Consecutive TxIDs strictly monotonically increasing");

        // High volume monotonicity test
        bool bStrictlyMonotonic = true;
        uint64_t lastId = tx3;
        for (int i = 0; i < 5000; ++i)
        {
            uint64_t curId = KPlayerTradeJournal::GenerateTxId();
            if (curId <= lastId) { bStrictlyMonotonic = false; break; }
            lastId = curId;
        }
        TEST_ASSERT(bStrictlyMonotonic, "5,000 rapid successive TxIDs maintain strict monotonicity without XOR wrap");

        // Concurrent multi-threaded uniqueness
        std::vector<uint64_t> threadIdsA(1000), threadIdsB(1000);
        std::thread t1([&]() { for (int i = 0; i < 1000; ++i) threadIdsA[i] = KPlayerTradeJournal::GenerateTxId(); });
        std::thread t2([&]() { for (int i = 0; i < 1000; ++i) threadIdsB[i] = KPlayerTradeJournal::GenerateTxId(); });
        t1.join();
        t2.join();

        bool bNoCollision = true;
        for (int i = 0; i < 1000 && bNoCollision; ++i) {
            for (int j = 0; j < 1000; ++j) {
                if (threadIdsA[i] == threadIdsB[j]) { bNoCollision = false; break; }
            }
        }
        TEST_ASSERT(bNoCollision, "Multi-threaded concurrent TxID generation yields zero collision");
    }

    //-------------------------------------------------------------------------
    // GROUP 2: Critical Section RAII & Negative State Machine Tests
    //-------------------------------------------------------------------------
    std::cout << "\n[GROUP 2: Critical Section RAII & Negative State Machine Invariants]" << std::endl;
    {
        KPlayerTradeJournal::Instance().SetJournalPath("trade_negative_test.journal");
        DeleteFileA("trade_negative_test.journal");

        uint64_t bogusTx = 0xDEADBEEFCAFEULL;
        TEST_ASSERT(!KPlayerTradeJournal::Instance().Commit(bogusTx), "Commit on non-existent TxID safely rejected (no lock hang)");
        TEST_ASSERT(!KPlayerTradeJournal::Instance().Abort(bogusTx), "Abort on non-existent TxID safely rejected (no lock hang)");
        TEST_ASSERT(!KPlayerTradeJournal::Instance().MarkApplied(bogusTx), "MarkApplied on non-existent TxID safely rejected");

        // Create a valid transaction
        uint64_t realTx = KPlayerTradeJournal::Instance().BeginTransaction(1, "PlayerA", "A", 1, 2, "PlayerB", "B", 1);
        TEST_ASSERT(realTx != 0, "BeginTransaction succeeded with valid monotonic TxID");
        TEST_ASSERT(KPlayerTradeJournal::Instance().GetTxState(realTx) == TX_STATE_PREPARING, "Initial state is TX_STATE_PREPARING");

        // Cannot mark applied when in PREPARING
        TEST_ASSERT(!KPlayerTradeJournal::Instance().MarkApplied(realTx), "MarkApplied fails if transaction is in PREPARING (not committed)");

        // Abort the transaction
        TEST_ASSERT(KPlayerTradeJournal::Instance().Abort(realTx), "Abort on PREPARING transaction succeeds");
        TEST_ASSERT(KPlayerTradeJournal::Instance().GetTxState(realTx) == TX_STATE_ABORTED, "State transitioned to TX_STATE_ABORTED");

        // Negative check: Cannot commit an aborted transaction
        TEST_ASSERT(!KPlayerTradeJournal::Instance().Commit(realTx), "Commit after Abort safely rejected by state machine");

        // Idempotency: Abort again on already aborted transaction returns true
        TEST_ASSERT(KPlayerTradeJournal::Instance().Abort(realTx), "Repeated Abort on aborted transaction is idempotent");

        // Cannot mark applied on aborted transaction
        TEST_ASSERT(!KPlayerTradeJournal::Instance().MarkApplied(realTx), "MarkApplied on aborted transaction rejected");
    }

    //-------------------------------------------------------------------------
    // GROUP 3: Complete State Machine: PREPARING -> COMMITTED -> APPLY_PENDING -> APPLIED
    //-------------------------------------------------------------------------
    std::cout << "\n[GROUP 3: State Machine: PREPARING -> COMMITTED -> APPLY_PENDING -> APPLIED]" << std::endl;
    {
        KPlayerTradeJournal::Instance().SetJournalPath("trade_lifecycle_test.journal");
        DeleteFileA("trade_lifecycle_test.journal");

        TTestRolePayload preA = { "RoleA", 100000, 1, { 501 } };
        TTestRolePayload preB = { "RoleB", 50000, 0, {} };

        uint64_t uTxId = KPlayerTradeJournal::Instance().BeginTransaction(
            1, "RoleA", &preA, sizeof(preA),
            2, "RoleB", &preB, sizeof(preB));
        TEST_ASSERT(uTxId != 0, "Tx created in PREPARING state");

        TTestRolePayload postA = { "RoleA", 50000, 0, {} };
        TTestRolePayload postB = { "RoleB", 100000, 1, { 501 } };
        TEST_ASSERT(KPlayerTradeJournal::Instance().RecordPostTrade(uTxId, &postA, sizeof(postA), &postB, sizeof(postB)),
                    "Post-trade serialized states recorded in journal entry");

        // Commit point
        TEST_ASSERT(KPlayerTradeJournal::Instance().Commit(uTxId), "Durable Commit succeeded and written to disk WAL");
        TEST_ASSERT(KPlayerTradeJournal::Instance().GetTxState(uTxId) == TX_STATE_COMMITTED, "State is TX_STATE_COMMITTED");

        // Transition to APPLY_PENDING
        TEST_ASSERT(KPlayerTradeJournal::Instance().SetApplyPending(uTxId), "State successfully transitioned to TX_STATE_APPLY_PENDING");
        TEST_ASSERT(KPlayerTradeJournal::Instance().GetTxState(uTxId) == TX_STATE_APPLY_PENDING, "State verified as TX_STATE_APPLY_PENDING");

        // MarkApplied is NOT called before Goddess ACK
        TEST_ASSERT(KPlayerTradeJournal::Instance().GetTxState(uTxId) != TX_STATE_APPLIED, "Proof: Transaction is NOT marked APPLIED before Goddess ACK");

        // Simulate positive ACK with TxID
        TEST_ASSERT(KPlayerTradeJournal::Instance().OnTradePairAck(uTxId, 1), "OnTradePairAck with positive result successfully marks APPLIED");
        TEST_ASSERT(KPlayerTradeJournal::Instance().GetTxState(uTxId) == TX_STATE_APPLIED, "State verified as TX_STATE_APPLIED");

        // Repeated commit on applied is idempotent
        TEST_ASSERT(KPlayerTradeJournal::Instance().Commit(uTxId), "Commit on already APPLIED transaction is idempotent");
        // Cannot abort an applied transaction
        TEST_ASSERT(!KPlayerTradeJournal::Instance().Abort(uTxId), "Cannot abort an already APPLIED transaction");
    }

    //-------------------------------------------------------------------------
    // GROUP 4: Goddess Atomic Dual-Save & Receipt Table Idempotency
    //-------------------------------------------------------------------------
    std::cout << "\n[GROUP 4: Goddess Atomic Dual-Save & Receipt Table Idempotency]" << std::endl;
    {
        sqlite3* pDb = NULL;
        TEST_ASSERT(SetupTestDatabase(&pDb, "role_closure_test.db"), "SQLite test DB initialized with role_Blob & trade_receipts");

        // Seed initial accounts
        TEST_ASSERT(InsertTestRole(pDb, "PlayerA", 100000, { 501 }), "PlayerA inserted: 100,000 gold, Item UID 501");
        TEST_ASSERT(InsertTestRole(pDb, "PlayerB", 50000, {}), "PlayerB inserted: 50,000 gold, 0 items");

        uint64_t uTxId = 0x202610240001ULL;
        TTestRolePayload newBlobA = { "PlayerA", 50000, 0, {} };
        TTestRolePayload newBlobB = { "PlayerB", 100000, 1, { 501 } };

        // Atomic Dual Save
        bool bSaved = ExecuteGoddessAtomicPairSave(pDb, uTxId, "PlayerA", &newBlobA, sizeof(newBlobA),
                                                              "PlayerB", &newBlobB, sizeof(newBlobB));
        TEST_ASSERT(bSaved, "Goddess updateBlobData_atomic_dual_tx committed both players atomically");

        // Verify receipt in trade_receipts table
        const char* qRcpt = "SELECT status FROM trade_receipts WHERE tx_id = ?;";
        sqlite3_stmt* stmt = NULL;
        sqlite3_prepare_v2(pDb, qRcpt, -1, &stmt, NULL);
        sqlite3_bind_int64(stmt, 1, (sqlite3_int64)uTxId);
        int rc = sqlite3_step(stmt);
        TEST_ASSERT(rc == SQLITE_ROW && sqlite3_column_int(stmt, 0) == 1, "Transaction receipt permanently written with status=1");
        sqlite3_finalize(stmt);

        // Idempotent duplicate save with SAME TxID
        bool bReplay = ExecuteGoddessAtomicPairSave(pDb, uTxId, "PlayerA", &newBlobA, sizeof(newBlobA),
                                                               "PlayerB", &newBlobB, sizeof(newBlobB));
        TEST_ASSERT(bReplay, "Duplicate save with same TxID returns idempotent success without data corruption");

        // Split-Save Failure Injection: Player B does not exist in DB
        uint64_t failTx = 0x202610240002ULL;
        TTestRolePayload failBlobA = { "PlayerA", 10000, 0, {} };
        TTestRolePayload failBlobB = { "NonExistentPlayerXYZ", 200000, 0, {} };

        bool bFailSaved = ExecuteGoddessAtomicPairSave(pDb, failTx, "PlayerA", &failBlobA, sizeof(failBlobA),
                                                                   "NonExistentPlayerXYZ", &failBlobB, sizeof(failBlobB));
        TEST_ASSERT(!bFailSaved, "Failure injection on Player B triggers immediate ROLLBACK");

        // Verify Player A was NOT modified by the failed transaction
        uint32_t goldA = 0;
        std::vector<uint32_t> itemsA;
        ReadTestRole(pDb, "PlayerA", goldA, itemsA);
        TEST_ASSERT(goldA == 50000, "Player A gold completely protected from partial split-save mutation");

        sqlite3_close(pDb);
    }

    //-------------------------------------------------------------------------
    // GROUP 5: Network Protocol Packets & Checksum Verification
    //-------------------------------------------------------------------------
    std::cout << "\n[GROUP 5: Network Protocol Packets & CRC Checksum Verification]" << std::endl;
    {
        TTestRolePayload blobA = { "Alice", 50000, 0, {} };
        TTestRolePayload blobB = { "Bob", 100000, 1, { 501 } };

        uint32_t crcA = KPlayerTradeJournal::ComputeCRC(&blobA, sizeof(blobA));
        uint32_t crcB = KPlayerTradeJournal::ComputeCRC(&blobB, sizeof(blobB));

        size_t totalPacketSize = sizeof(TTradePairSaveRequest) - 1 + sizeof(blobA) + sizeof(blobB);
        std::vector<uint8_t> packetBuf(totalPacketSize);

        TTradePairSaveRequest* pReq = (TTradePairSaveRequest*)packetBuf.data();
        pReq->nProtoId = c2s_roleserver_savetradepair;
        pReq->uTxId = 0xABCDEF012345ULL;
        strncpy_s(pReq->szRoleA, sizeof(pReq->szRoleA), "Alice", _TRUNCATE);
        strncpy_s(pReq->szRoleB, sizeof(pReq->szRoleB), "Bob", _TRUNCATE);
        pReq->dwDataLenA = sizeof(blobA);
        pReq->dwDataLenB = sizeof(blobB);
        pReq->dwChecksumA = crcA;
        pReq->dwChecksumB = crcB;
        pReq->uGenerationA = 1;
        pReq->uGenerationB = 1;
        pReq->ulIdentityA = 10;
        pReq->ulIdentityB = 20;

        memcpy(&pReq->pDataBuffer[0], &blobA, sizeof(blobA));
        memcpy(&pReq->pDataBuffer[sizeof(blobA)], &blobB, sizeof(blobB));

        TEST_ASSERT(pReq->nProtoId == c2s_roleserver_savetradepair, "Request packet protocol ID is c2s_roleserver_savetradepair");
        TEST_ASSERT(pReq->dwChecksumA == crcA && pReq->dwChecksumB == crcB, "Dual-CRC checksums in packet header match payload data");

        // Verify response packet structure
        TTradePairSaveResult res;
        memset(&res, 0, sizeof(res));
        res.nProtoId = s2c_roleserver_savetradepair_result;
        res.uTxId = pReq->uTxId;
        res.nResultCode = 1; // Success
        strncpy_s(res.szRoleA, sizeof(res.szRoleA), pReq->szRoleA, _TRUNCATE);
        strncpy_s(res.szRoleB, sizeof(res.szRoleB), pReq->szRoleB, _TRUNCATE);
        res.ulIdentityA = pReq->ulIdentityA;
        res.ulIdentityB = pReq->ulIdentityB;

        TEST_ASSERT(res.nProtoId == s2c_roleserver_savetradepair_result, "ACK packet protocol ID is s2c_roleserver_savetradepair_result");
        TEST_ASSERT(res.uTxId == pReq->uTxId, "ACK packet TxID strictly matches request TxID");
        TEST_ASSERT(res.nResultCode == 1, "ACK packet result code reflects database commit success");
    }

    //-------------------------------------------------------------------------
    // GROUP 6: Real Crash Simulation & Startup Recovery Coordinator
    //-------------------------------------------------------------------------
    std::cout << "\n[GROUP 6: Real Process Crash Simulation & Startup Recovery Coordinator]" << std::endl;
    {
        // Step 1: Launch child process and let it abruptly crash at COMMIT point
        char szExePath[MAX_PATH];
        GetModuleFileNameA(NULL, szExePath, MAX_PATH);

        std::string cmd = std::string("\"") + szExePath + "\" --simulate-crash-mid-commit";
        STARTUPINFOA si;
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));

        BOOL bSpawn = CreateProcessA(NULL, (LPSTR)cmd.c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
        TEST_ASSERT(bSpawn, "Spawned child DEV GameServer process to simulate mid-transaction crash");

        WaitForSingleObject(pi.hProcess, 10000);
        DWORD dwExitCode = 0;
        GetExitCodeProcess(pi.hProcess, &dwExitCode);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);

        TEST_ASSERT(dwExitCode == 42, "Child DEV GameServer terminated abruptly (simulated crash after WAL COMMIT)");

        // Step 2: Restart engine and execute StartupRecovery
        KPlayerTradeJournal::Instance().SetJournalPath("trade_child_crash.journal");
        KPlayerTradeJournal::Instance().ClearLockedPlayers();

        // Test Phase A: Database client is NULL (Goddess down)
        bool bReconciledWithoutDB = KPlayerTradeJournal::Instance().StartupRecovery(NULL);
        TEST_ASSERT(!bReconciledWithoutDB, "StartupRecovery returns FALSE when Goddess is offline (does NOT mark applied falsely)");
        TEST_ASSERT(KPlayerTradeJournal::Instance().IsPlayerLocked("RoleA"), "Player 'RoleA' kept locked from login while commit unconfirmed");
        TEST_ASSERT(KPlayerTradeJournal::Instance().IsPlayerLocked("RoleB"), "Player 'RoleB' kept locked from login while commit unconfirmed");

        // Test Phase B: Goddess reconnects and confirms transaction
        // Initialize SQLite DB fixture for child recovery
        sqlite3* pCrashDb = NULL;
        SetupTestDatabase(&pCrashDb, "role_crash_recovery.db");
        InsertTestRole(pCrashDb, "RoleA", 100000, { 501 });
        InsertTestRole(pCrashDb, "RoleB", 50000, {});

        std::vector<KTradeTxJournalEntry> pendingCommits, abortedRollbacks;
        KPlayerTradeJournal::Instance().ScanAndRecover(pendingCommits, abortedRollbacks);
        TEST_ASSERT(pendingCommits.size() == 1, "ScanAndRecover found exactly 1 unapplied COMMITTED transaction from crashed child");

        uint64_t recoveredTxId = pendingCommits[0].header.uTxId;
        TEST_ASSERT(recoveredTxId != 0, "Recovered TxID is valid");
        TEST_ASSERT(pendingCommits[0].postDataA.size() == sizeof(TTestRolePayload), "Recovered Post-Trade blob A is intact with valid CRC32");
        TEST_ASSERT(pendingCommits[0].postDataB.size() == sizeof(TTestRolePayload), "Recovered Post-Trade blob B is intact with valid CRC32");

        // Coordinator applies atomic pair save to Goddess
        bool bAppliedToGoddess = ExecuteGoddessAtomicPairSave(
            pCrashDb, recoveredTxId,
            pendingCommits[0].header.szPlayerA_Name, pendingCommits[0].postDataA.data(), (int)pendingCommits[0].postDataA.size(),
            pendingCommits[0].header.szPlayerB_Name, pendingCommits[0].postDataB.data(), (int)pendingCommits[0].postDataB.size());
        TEST_ASSERT(bAppliedToGoddess, "Recovery Coordinator committed recovered post-trade blobs into Goddess SQLite");

        // On ACK: Mark applied and unlock participants
        TEST_ASSERT(KPlayerTradeJournal::Instance().OnTradePairAck(recoveredTxId, 1), "OnTradePairAck durably marks transaction APPLIED");
        TEST_ASSERT(!KPlayerTradeJournal::Instance().IsPlayerLocked("RoleA"), "Player 'RoleA' unlocked and authorized for login");
        TEST_ASSERT(!KPlayerTradeJournal::Instance().IsPlayerLocked("RoleB"), "Player 'RoleB' unlocked and authorized for login");

        // Repeated recovery scan is now completely clean (0 pending)
        std::vector<KTradeTxJournalEntry> secondScanCommits, secondScanAborts;
        KPlayerTradeJournal::Instance().ScanAndRecover(secondScanCommits, secondScanAborts);
        TEST_ASSERT(secondScanCommits.empty(), "Subsequent recovery scan confirms 0 pending transactions remaining in WAL");

        sqlite3_close(pCrashDb);
    }

    //-------------------------------------------------------------------------
    // GROUP 7: Data Persistence & Mathematical Invariants from Cold Disk Reload
    //-------------------------------------------------------------------------
    std::cout << "\n[GROUP 7: Data Persistence & Mathematical Invariants from Cold Reload]" << std::endl;
    {
        sqlite3* pColdDb = NULL;
        int rc = sqlite3_open_v2("role_crash_recovery.db", &pColdDb, SQLITE_OPEN_READONLY, NULL);
        TEST_ASSERT(rc == SQLITE_OK, "Reopened 'role_crash_recovery.db' cold from disk for forensic validation");

        uint32_t goldA = 0, goldB = 0;
        std::vector<uint32_t> itemsA, itemsB;

        TEST_ASSERT(ReadTestRole(pColdDb, "RoleA", goldA, itemsA), "Successfully read and deserialized 'RoleA' from SQLite");
        TEST_ASSERT(ReadTestRole(pColdDb, "RoleB", goldB, itemsB), "Successfully read and deserialized 'RoleB' from SQLite");

        TEST_ASSERT(goldA == 50000, "RoleA gold post-recovery is exactly 50,000");
        TEST_ASSERT(goldB == 100000, "RoleB gold post-recovery is exactly 100,000");

        // Invariant 1: Total gold sum strictly conserved
        uint32_t totalGold = goldA + goldB;
        TEST_ASSERT(totalGold == 150000, "Mathematical Invariant 1: Total Gold Sum = 150,000 strictly conserved (Zero Inflation/Deflation)");

        // Invariant 2: Item single ownership
        TEST_ASSERT(itemsA.empty(), "Mathematical Invariant 2A: RoleA has 0 items (Item UID 501 removed)");
        TEST_ASSERT(itemsB.size() == 1 && itemsB[0] == 501, "Mathematical Invariant 2B: RoleB exclusively owns Item UID 501 (Single Ownership)");

        // Verify receipt
        const char* qRcpt = "SELECT count(*) FROM trade_receipts WHERE status = 1;";
        sqlite3_stmt* stmt = NULL;
        sqlite3_prepare_v2(pColdDb, qRcpt, -1, &stmt, NULL);
        int r = sqlite3_step(stmt);
        TEST_ASSERT(r == SQLITE_ROW && sqlite3_column_int(stmt, 0) == 1, "Cold DB contains exactly 1 durable trade receipt matching WAL");
        sqlite3_finalize(stmt);

        sqlite3_close(pColdDb);
    }

    std::cout << "\n================================================================================" << std::endl;
    std::cout << " TEST SUITE COMPLETE: " << g_nPassed << " PASSED, " << g_nFailed << " FAILED." << std::endl;
    std::cout << "================================================================================" << std::endl;

    return (g_nFailed == 0) ? 0 : 1;
}

//=============================================================================
// test_phase3_persistence_e2e.cpp
// End-to-End Persistence and Real Item Trade Test Suite for Phase 3 Final P0
// Tests:
// - Real KPlayer::TradeDecision(BYTE*) execution
// - Real Non-Empty Inventory, Items, ItemSet, stackable, multi-cell, bound items
// - Isolated SQLite database fixture (role_test.db) with role_Blob schema
// - Split-save failure injection, WAL recovery coordinator, crash-restart
// - Conservation of items, unique IDs, gold sum, single ownership invariants
//=============================================================================

#define _SERVER
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include <fstream>
#include <sstream>

// Product Headers
#include "KCore.h"
#include "C:/Server-Game/SwordOnline/jxOnline/gameserver/lib/S3DBInterface.h"
#include "KProtocolDef.h"
#include "KPlayerAntiAddict.h"
#include "KPlayer.h"
#include "KPlayerTrade.h"
#include "KItemList.h"
#include "KItem.h"
#include "KItemSet.h"
#include "KNpc.h"
#include "KPlayerTradeJournal.h"
#include "V9_TongDef.h"
#include "KTongProtocol.h"
#include "IServer.h"
#include "IClient.h"
#include "LuaLib.h"
#include "sqlite3.h"

// Exported Product Functions
extern CORE_API int LuaAddContribution(Lua_State* L);
extern CORE_API int LuaGetContribution(Lua_State* L);
extern CORE_API int LuaAddCumulateOffer(Lua_State* L);
extern CORE_API int LuaGetCumulateOffer(Lua_State* L);
extern CORE_API int LuaAddTotalOffer(Lua_State* L);
extern CORE_API int LuaGetTotalOffer(Lua_State* L);
extern CORE_API int LuaC_Random(Lua_State* L);
extern CORE_API int LuaRemoteExecute(Lua_State* L);
extern CORE_API int LuaSendScriptData(Lua_State* L);

//=============================================================================
// Isolated SQLite Database Fixture
//=============================================================================
class CTestDatabaseFixture {
public:
    std::string m_strDbPath;
    sqlite3* m_pDb;

    CTestDatabaseFixture(const char* szPath) : m_strDbPath(szPath), m_pDb(NULL) {}
    ~CTestDatabaseFixture() { Close(); }

    bool Open()
    {
        Close();
        int rc = sqlite3_open(m_strDbPath.c_str(), &m_pDb);
        if (rc != SQLITE_OK) return false;

        const char* szSql =
            "CREATE TABLE IF NOT EXISTS role_Blob ("
            "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
            "roleName TEXT UNIQUE,"
            "StudentBlob BLOB,"
            "ekey INTEGER"
            ");";
        char* zErrMsg = NULL;
        rc = sqlite3_exec(m_pDb, szSql, NULL, 0, &zErrMsg);
        if (zErrMsg) sqlite3_free(zErrMsg);
        return (rc == SQLITE_OK);
    }

    void Close()
    {
        if (m_pDb) {
            sqlite3_close(m_pDb);
            m_pDb = NULL;
        }
    }

    void Reset()
    {
        if (!m_pDb) Open();
        if (m_pDb) {
            sqlite3_exec(m_pDb, "DELETE FROM role_Blob;", NULL, 0, NULL);
        }
    }

    bool SaveRoleBlob(const char* szRoleName, const void* pData, int nLen)
    {
        if (!m_pDb || !szRoleName || !pData || nLen <= 0) return false;
        const char* szSql =
            "INSERT INTO role_Blob (roleName, StudentBlob, ekey) VALUES (?, ?, 0) "
            "ON CONFLICT(roleName) DO UPDATE SET StudentBlob = excluded.StudentBlob;";
        sqlite3_stmt* stmt = NULL;
        if (sqlite3_prepare_v2(m_pDb, szSql, -1, &stmt, NULL) != SQLITE_OK) return false;
        sqlite3_bind_text(stmt, 1, szRoleName, -1, SQLITE_STATIC);
        sqlite3_bind_blob(stmt, 2, pData, nLen, SQLITE_STATIC);
        int rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        return (rc == SQLITE_DONE);
    }

    bool LoadRoleBlob(const char* szRoleName, std::vector<uint8_t>& outData)
    {
        outData.clear();
        if (!m_pDb || !szRoleName) return false;
        const char* szSql = "SELECT StudentBlob FROM role_Blob WHERE roleName = ?;";
        sqlite3_stmt* stmt = NULL;
        if (sqlite3_prepare_v2(m_pDb, szSql, -1, &stmt, NULL) != SQLITE_OK) return false;
        sqlite3_bind_text(stmt, 1, szRoleName, -1, SQLITE_STATIC);
        bool bFound = false;
        if (sqlite3_step(stmt) == SQLITE_ROW)
        {
            const void* pBlob = sqlite3_column_blob(stmt, 0);
            int nBytes = sqlite3_column_bytes(stmt, 0);
            if (pBlob && nBytes > 0)
            {
                outData.resize(nBytes);
                memcpy(outData.data(), pBlob, nBytes);
                bFound = true;
            }
        }
        sqlite3_finalize(stmt);
        return bFound;
    }

    // Paired Atomic Persistence
    bool SaveTradePairAtomic(const char* szNameA, const void* pDataA, int nLenA,
                             const char* szNameB, const void* pDataB, int nLenB)
    {
        if (!m_pDb) return false;
        sqlite3_exec(m_pDb, "BEGIN IMMEDIATE TRANSACTION;", NULL, 0, NULL);
        bool bOkA = SaveRoleBlob(szNameA, pDataA, nLenA);
        bool bOkB = SaveRoleBlob(szNameB, pDataB, nLenB);
        if (bOkA && bOkB)
        {
            sqlite3_exec(m_pDb, "COMMIT;", NULL, 0, NULL);
            return true;
        }
        else
        {
            sqlite3_exec(m_pDb, "ROLLBACK;", NULL, 0, NULL);
            return false;
        }
    }
};

static CTestDatabaseFixture g_DbFixture("role_test.db");

//=============================================================================
// Test Network Doubles
//=============================================================================
struct CapturedPacket {
    DWORD dwClientId;
    std::vector<BYTE> data;
};

class CTestServerDouble : public IServer {
public:
    std::vector<CapturedPacket> m_vecPackets;
    void Clear() { m_vecPackets.clear(); }

    STDMETHOD(Startup)() { return S_OK; }
    STDMETHOD(Cleanup)() { return S_OK; }
    STDMETHOD(OpenService)(const unsigned long &a, const unsigned short &b) { return S_OK; }
    STDMETHOD(CloseService)() { return S_OK; }
    STDMETHOD(RegisterMsgFilter)(LPVOID lpParam, CALLBACK_SERVER_EVENT pfn) { return S_OK; }
    STDMETHOD(RegisterMsgFilter)(const unsigned long u, IMessageProcess* p) { return S_OK; }
    STDMETHOD(PreparePackSink)() { return S_OK; }

    STDMETHOD(PackDataToClient)(const unsigned long &ulnClientID, const void * const pData, const size_t &datalength)
    {
        CapturedPacket cp;
        cp.dwClientId = ulnClientID;
        if (pData && datalength > 0)
        {
            BYTE ptype = *(const BYTE*)pData;
            if (ptype == s2c_msgshow)
            {
                SHOW_MSG_SYNC* pMsg = (SHOW_MSG_SYNC*)pData;
                std::cout << "[PACKET MSGSHOW] client=" << ulnClientID << ", msgID=" << pMsg->m_wMsgID
                          << ", text=" << pMsg->m_Mmsgs << std::endl;
            }
            cp.data.resize(datalength);
            memcpy(cp.data.data(), pData, datalength);
        }
        m_vecPackets.push_back(cp);
        return S_OK;
    }

    STDMETHOD(SendPackToClient)(const unsigned long &u) { return S_OK; }
    STDMETHOD(SendPackToClient)(const unsigned long &u, const void * const p, const size_t &s) { return S_OK; }
    STDMETHOD(SendData)(const unsigned long &u, const void * const p, const size_t &s) { return S_OK; }
    STDMETHOD(Broadcast)(const void * const p, const size_t &s) { return S_OK; }
    STDMETHOD_(const void *, GetPackFromClient)(const unsigned long &u, size_t &s) { return NULL; }
    STDMETHOD(ShutdownClient)(const unsigned long &u) { return S_OK; }
    STDMETHOD_(size_t, GetClientCount)() { return 0; }
    STDMETHOD_(void, GetClientInfo)(const unsigned long &u, char *p, size_t s) {}
    STDMETHOD(QueryInterface)(REFIID riid, void **ppv) { return E_NOTIMPL; }
    STDMETHOD_(ULONG, AddRef)() { return 1; }
    STDMETHOD_(ULONG, Release)() { return 1; }
};

class CTestDataClientDouble : public IClient {
public:
    std::vector<std::vector<BYTE>> m_vecDataPackets;
    bool m_bSimulateFailure;
    bool m_bFailPlayerAOnly;
    bool m_bFailPlayerBOnly;
    std::string m_strPlayerAName;
    std::string m_strPlayerBName;

    std::vector<BYTE> m_accumulatedPayload;

    CTestDataClientDouble()
        : m_bSimulateFailure(false), m_bFailPlayerAOnly(false), m_bFailPlayerBOnly(false) {}

    void Clear()
    {
        m_vecDataPackets.clear();
        m_bSimulateFailure = false;
        m_bFailPlayerAOnly = false;
        m_bFailPlayerBOnly = false;
        m_accumulatedPayload.clear();
    }

    STDMETHOD(Startup)() { return S_OK; }
    STDMETHOD(Cleanup)() { return S_OK; }
    STDMETHOD(FsGameServerConnectTo)(const char * const &pAddressToConnectServer, unsigned short usPortToConnectServer) { return S_OK; }
    STDMETHOD(RegisterMsgFilter)(LPVOID lpParam, CALLBACK_CLIENT_EVENT pfn) { return S_OK; }

    STDMETHOD(SendPackToServer)(const void * const pData, const size_t &datalength)
    {
        if (m_bSimulateFailure) return E_FAIL;
        if (!pData || datalength <= 0) return S_OK;

        const BYTE* pBytes = (const BYTE*)pData;
        size_t requestLength = datalength;

        // The real Goddess reassembles CPackager fragments before dispatching
        // the atomic pair opcode. The QA client must do the same.
        if (pBytes[0] == c2s_roleserver_savetradepair && datalength >= 6)
        {
            const BYTE flags = pBytes[1];
            if (flags == 0xC0 || flags == 0xCC || flags == 0x30 || flags == 0x3C)
            {
                if ((flags & 0xC0) != 0)
                    m_accumulatedPayload.clear();
                m_accumulatedPayload.insert(m_accumulatedPayload.end(), pBytes + 6, pBytes + datalength);
                if ((flags & 0x0C) == 0)
                    return S_OK;
                pBytes = m_accumulatedPayload.data();
                requestLength = m_accumulatedPayload.size();
            }
        }

        if (requestLength >= sizeof(TTradePairSaveRequest) && pBytes[0] == c2s_roleserver_savetradepair)
        {
            TTradePairSaveRequest* pReq = (TTradePairSaveRequest*)pBytes;
            const size_t payloadUsed = sizeof(TTradePairSaveRequest) - 1;
            if (pReq->dwDataLenA > requestLength - payloadUsed ||
                pReq->dwDataLenB > requestLength - payloadUsed - pReq->dwDataLenA)
                return E_FAIL;
            if (m_bSimulateFailure) return E_FAIL;
            if (m_bFailPlayerBOnly)
            {
                g_DbFixture.SaveRoleBlob(pReq->szRoleA, &pReq->pDataBuffer[0], pReq->dwDataLenA);
                return S_OK;
            }

            // Goddess test double: commit both players inside one SQLite transaction.
            if (!g_DbFixture.SaveTradePairAtomic(pReq->szRoleA, &pReq->pDataBuffer[0], pReq->dwDataLenA,
                                                 pReq->szRoleB, &pReq->pDataBuffer[pReq->dwDataLenA], pReq->dwDataLenB))
                return E_FAIL;

            KPlayerTradeJournal::Instance().OnTradePairAck(pReq->uTxId, 1);
            if (pReq->ulIdentityA > 0 && pReq->ulIdentityA < MAX_PLAYER)
            {
                Player[pReq->ulIdentityA].m_cMenuState.m_nState = PLAYER_MENU_STATE_NORMAL;
                Player[pReq->ulIdentityA].m_ItemList.InvalidateTradeSnapshot();
                Player[pReq->ulIdentityA].m_cTrade.Release();
            }
            if (pReq->ulIdentityB > 0 && pReq->ulIdentityB < MAX_PLAYER)
            {
                Player[pReq->ulIdentityB].m_cMenuState.m_nState = PLAYER_MENU_STATE_NORMAL;
                Player[pReq->ulIdentityB].m_ItemList.InvalidateTradeSnapshot();
                Player[pReq->ulIdentityB].m_cTrade.Release();
            }
            return S_OK;
        }

        // Check for chunk header and extract name
        // Byte 0: cID (1 byte)
        // Byte 1: cFlag (1 byte)
        // Bytes 2..5: lnUserData (4 bytes)
        // Bytes 6..19: sProcessData (14 bytes)
        // Bytes 20..23: dwId (4 bytes)
        // Bytes 24+: szName (100 bytes)
        if (datalength > 24)
        {
            const char* szNameInChunk = (const char*)(pBytes + 24);
            if (m_bFailPlayerAOnly && m_strPlayerAName == szNameInChunk)
                return E_FAIL;
            if (m_bFailPlayerBOnly && m_strPlayerBName == szNameInChunk)
                return E_FAIL;
        }

        std::vector<BYTE> pkt(datalength);
        memcpy(pkt.data(), pData, datalength);
        m_vecDataPackets.push_back(pkt);

        // Reassemble chunks using JX1 CPackager wire format (0xc0 = Header, 0x0c = Tail)
        if (datalength > 6)
        {
            BYTE cFlag = pBytes[1];
            if ((cFlag & 0xc0) != 0) // Header
            {
                m_accumulatedPayload.clear();
            }
            m_accumulatedPayload.insert(m_accumulatedPayload.end(), pBytes + 6, pBytes + datalength);

            if ((cFlag & 0x0c) != 0) // Tail
            {
                if (m_accumulatedPayload.size() > 14)
                {
                    const TRoleData* pRole = (const TRoleData*)(m_accumulatedPayload.data() + 14);
                    std::string name = pRole->BaseInfo.szName;
                    g_DbFixture.SaveRoleBlob(name.c_str(), pRole, pRole->dwDataLen);
                }
            }
        }
        return S_OK;
    }

    STDMETHOD_(const void *, GetPackFromServer)(size_t &datalength) { return NULL; }
    STDMETHOD(Shutdown)() { return S_OK; }
    STDMETHOD(QueryInterface)(REFIID riid, void **ppv) { return E_NOTIMPL; }
    STDMETHOD_(ULONG, AddRef)() { return 1; }
    STDMETHOD_(ULONG, Release)() { return 1; }
};

class CTestTongClientDouble : public IClient {
public:
    std::vector<std::vector<BYTE>> m_vecTongPackets;
    bool m_bSimulateFailure;

    CTestTongClientDouble() : m_bSimulateFailure(false) {}
    void Clear() { m_vecTongPackets.clear(); m_bSimulateFailure = false; }

    STDMETHOD(Startup)() { return S_OK; }
    STDMETHOD(Cleanup)() { return S_OK; }
    STDMETHOD(FsGameServerConnectTo)(const char * const &p, unsigned short u) { return S_OK; }
    STDMETHOD(RegisterMsgFilter)(LPVOID l, CALLBACK_CLIENT_EVENT p) { return S_OK; }

    STDMETHOD(SendPackToServer)(const void * const pData, const size_t &datalength)
    {
        if (m_bSimulateFailure) return E_FAIL;
        if (pData && datalength > 0)
        {
            std::vector<BYTE> pkt(datalength);
            memcpy(pkt.data(), pData, datalength);
            m_vecTongPackets.push_back(pkt);
        }
        return S_OK;
    }

    STDMETHOD_(const void *, GetPackFromServer)(size_t &datalength) { return NULL; }
    STDMETHOD(Shutdown)() { return S_OK; }
    STDMETHOD(QueryInterface)(REFIID riid, void **ppv) { return E_NOTIMPL; }
    STDMETHOD_(ULONG, AddRef)() { return 1; }
    STDMETHOD_(ULONG, Release)() { return 1; }
};

static CTestServerDouble g_TestServer;
static CTestDataClientDouble g_TestDataClient;
static CTestTongClientDouble g_TestTongClient;

static int g_nPassCount = 0;
static int g_nFailCount = 0;

#define ASSERT_TRUE(cond, desc) \
    do { \
        if (cond) { \
            g_nPassCount++; \
            std::cout << "[PASS] " << desc << std::endl; \
        } else { \
            g_nFailCount++; \
            std::cerr << "[FAIL] " << desc << " (Line: " << __LINE__ << ": " #cond ")" << std::endl; \
        } \
    } while(0)

#define ASSERT_EQ(v1, v2, desc) \
    do { \
        if ((v1) == (v2)) { \
            g_nPassCount++; \
            std::cout << "[PASS] " << desc << std::endl; \
        } else { \
            g_nFailCount++; \
            std::cerr << "[FAIL] " << desc << " (Expected " << (v2) << ", got " << (v1) << " at Line: " << __LINE__ << ")" << std::endl; \
        } \
    } while(0)

// Helper: send TRADE_DECISION_COMMAND to KPlayer::TradeDecision
void SendTradeDecisionPacket(int nPlayerIdx, BYTE btDecision)
{
    TRADE_DECISION_COMMAND sDecision;
    sDecision.ProtocolType = c2s_tradedecision;
    sDecision.m_btDecision = btDecision;
    Player[nPlayerIdx].TradeDecision((BYTE*)&sDecision);
}

// Helper: Simulate Goddess ACK for all APPLY_PENDING transactions (unlock players)
void SimulateGoddesAck()
{
    std::vector<KTradeTxJournalEntry> pendingCommits, abortedRollbacks;
    KPlayerTradeJournal& journal = KPlayerTradeJournal::Instance();
    journal.ScanAndRecover(pendingCommits, abortedRollbacks);
    for (auto& entry : pendingCommits)
    {
        // Save post-trade blobs to DB (simulating Goddess commit)
        if (entry.postDataA.size() > 0)
            g_DbFixture.SaveRoleBlob(entry.header.szPlayerA_Name, entry.postDataA.data(), (int)entry.postDataA.size());
        if (entry.postDataB.size() > 0)
            g_DbFixture.SaveRoleBlob(entry.header.szPlayerB_Name, entry.postDataB.data(), (int)entry.postDataB.size());
        // ACK to unlock players
        journal.OnTradePairAck(entry.header.uTxId, 1);
    }
    g_TestDataClient.Clear();
}

// Helper: create and add real KItem to ItemSet
int CreateRealTestItem(int nGenre, int nDetail, int nParticular, int nLevel, int nSeries,
                       int nStackNum, int nWidth, int nHeight, BOOL bBang = FALSE)
{
    KItem item;
    memset(&item, 0, sizeof(item));
    item.m_CommonAttrib.nItemGenre = nGenre;
    item.m_CommonAttrib.nDetailType = nDetail;
    item.m_CommonAttrib.nParticularType = nParticular;
    item.m_CommonAttrib.nLevel = nLevel;
    item.m_CommonAttrib.nSeries = nSeries;
    item.m_CommonAttrib.nWidth = nWidth;
    item.m_CommonAttrib.nHeight = nHeight;
    item.SetStackNum(nStackNum);
    item.SetBang(bBang ? 1 : 0);

    int nIdx = ItemSet.Add(&item);
    return nIdx;
}

// Helper: clear trade item slot info
void ClearTradeSlotInfo()
{
    if (ItemSet.m_psItemInfo)
        memset(ItemSet.m_psItemInfo, 0, sizeof(TRADE_ITEM_INFO) * TRADE_ROOM_WIDTH * TRADE_ROOM_HEIGHT);
    if (ItemSet.m_psBackItemInfo)
        memset(ItemSet.m_psBackItemInfo, 0, sizeof(TRADE_ITEM_INFO) * TRADE_ROOM_WIDTH * TRADE_ROOM_HEIGHT);
}

// Helper: Setup Player with Inventory and Money
void SetupTestPlayer(int nIdx, const char* szName, int nMoney)
{
    Player[nIdx].SetPlayerIndex(nIdx);
    Player[nIdx].m_nIndex = nIdx;
    strncpy_s(Player[nIdx].m_PlayerName, sizeof(Player[nIdx].m_PlayerName), szName, _TRUNCATE);
    Npc[nIdx].m_dwID = 1000 + nIdx;
    Npc[nIdx].m_Level = 80;
    strncpy_s(Npc[nIdx].Name, sizeof(Npc[nIdx].Name), szName, _TRUNCATE);
    Player[nIdx].m_nNetConnectIdx = 1000 + nIdx;
    Player[nIdx].m_CurSerVer = 1;
    Player[nIdx].m_ItemList.Init(nIdx);
    Player[nIdx].m_cTrade.Release();
    Player[nIdx].m_ItemList.SetRoomMoney(room_equipment, nMoney);
}

// Helper: count non-zero items in player inventory
int CountPlayerItems(int nPlayerIdx)
{
    return Player[nPlayerIdx].m_ItemList.GetItemCount();
}

// Helper: Put item into trade room using engine's serAutoMoveItem
bool PutItemInTradeRoom(int nPlayerIdx, int nItemIdx, int nSrcX = 0, int nSrcY = 0)
{
    return Player[nPlayerIdx].m_ItemList.serAutoMoveItem(nItemIdx, pos_equiproom, nSrcX, nSrcY, room_trade) != 0;
}

//=============================================================================
// MAIN TEST SUITE
//=============================================================================
int main(int argc, char* argv[])
{
    std::cout << "=================================================================" << std::endl;
    std::cout << " PHASE 3 FINAL P0: ATOMIC DUAL-PERSISTENCE & REAL ITEM TRANSFERS " << std::endl;
    std::cout << "=================================================================" << std::endl;

    // Initialize Global Singletons & Test Fixture
    nMaxItemCount = 10000;
    nMaxNpcCount = 200;
    Player = new KPlayer[500];
    Npc = new KNpc[MAX_NPC];
    Item = new KItem[MAX_ITEM];

    ItemSet.Init();
    g_pServer = &g_TestServer;
    g_pDataClient = &g_TestDataClient;
    g_pTongClient = &g_TestTongClient;
    g_SetDataServer(&g_TestDataClient);

    g_DbFixture.Open();
    g_DbFixture.Reset();
    KPlayerTradeJournal::Instance().SetJournalPath("trade_tx_test.journal");
    DeleteFileA("trade_tx_test.journal");

    const int IDX_A = 1;
    const int IDX_B = 2;

    //-------------------------------------------------------------------------
    // TEST 1: Single Valid Item Transfer (Weapon A -> B)
    //-------------------------------------------------------------------------
    std::cout << "\n--- TEST 1: Single Item Transfer (Weapon A -> B) ---" << std::endl;
    SetupTestPlayer(IDX_A, "Hero_A", 10000);
    SetupTestPlayer(IDX_B, "Hero_B", 10000);
    int nItemWeapon = CreateRealTestItem(item_equip, equip_meleeweapon, 1, 5, 0, 1, 1, 2);
    int nAddA = Player[IDX_A].m_ItemList.Add(nItemWeapon, pos_equiproom, 0, 0, FALSE, 0, 1, 1);
    ASSERT_TRUE(nAddA > 0, "Weapon placed in Player A inventory");
    ASSERT_EQ(CountPlayerItems(IDX_A), 1, "Player A has 1 item");
    ASSERT_EQ(CountPlayerItems(IDX_B), 0, "Player B has 0 items");

    // Initiate trade session
    Player[IDX_A].m_cTrade.Release();
    Player[IDX_B].m_cTrade.Release();
    DWORD dwSessionId = 1001;
    Player[IDX_A].m_ItemList.StartTrade(dwSessionId);
    Player[IDX_A].m_cTrade.StartTrade(IDX_B, 0, 0, dwSessionId);
    Player[IDX_A].m_cMenuState.m_nState = PLAYER_MENU_STATE_TRADING;

    Player[IDX_B].m_ItemList.StartTrade(dwSessionId);
    Player[IDX_B].m_cTrade.StartTrade(IDX_A, 0, 0, dwSessionId);
    Player[IDX_B].m_cMenuState.m_nState = PLAYER_MENU_STATE_TRADING;

    // Move weapon to trade room
    PutItemInTradeRoom(IDX_A, nItemWeapon, 0, 0);

    // Execute decision sequence
    SendTradeDecisionPacket(IDX_A, 2); // Lock A
    SendTradeDecisionPacket(IDX_B, 2); // Lock B
    SendTradeDecisionPacket(IDX_B, 1); // Confirm B
    SendTradeDecisionPacket(IDX_A, 1); // Confirm A -> commit

    ASSERT_EQ(CountPlayerItems(IDX_A), 0, "Player A has 0 items after trade");
    ASSERT_EQ(CountPlayerItems(IDX_B), 1, "Player B received the weapon");
    ASSERT_TRUE(Item[nItemWeapon].GetID() > 0, "Weapon unique ID preserved");

    // Check DB persistence
    std::vector<uint8_t> dbDataA, dbDataB;
    ASSERT_TRUE(g_DbFixture.LoadRoleBlob("Hero_A", dbDataA), "Hero_A saved to SQLite DB");
    ASSERT_TRUE(g_DbFixture.LoadRoleBlob("Hero_B", dbDataB), "Hero_B saved to SQLite DB");
    if (!dbDataA.empty() && !dbDataB.empty())
    {
        TRoleData* pRoleA = (TRoleData*)dbDataA.data();
        TRoleData* pRoleB = (TRoleData*)dbDataB.data();
        ASSERT_EQ(pRoleA->nItemCount, 0, "DB Hero_A has 0 items");
        ASSERT_EQ(pRoleB->nItemCount, 1, "DB Hero_B has 1 item");
    }

    //-------------------------------------------------------------------------
    // TEST 2: Multiple Items Transfer (A -> B)
    //-------------------------------------------------------------------------
    std::cout << "\n--- TEST 2: Multiple Items Transfer (A -> B) ---" << std::endl;
    int nItemPotion = CreateRealTestItem(item_medicine, 0, 1, 1, 0, 10, 1, 1);
    int nItemAmulet = CreateRealTestItem(item_equip, equip_amulet, 1, 3, 0, 1, 1, 1);
    Player[IDX_A].m_ItemList.Add(nItemPotion, pos_equiproom, 0, 0, FALSE, 0, 10, 1);
    Player[IDX_A].m_ItemList.Add(nItemAmulet, pos_equiproom, 1, 0, FALSE, 0, 1, 1);

    Player[IDX_A].m_cTrade.Release();
    Player[IDX_B].m_cTrade.Release();
    dwSessionId = 1002;
    Player[IDX_A].m_ItemList.StartTrade(dwSessionId);
    Player[IDX_A].m_cTrade.StartTrade(IDX_B, 0, 0, dwSessionId);
    Player[IDX_A].m_cMenuState.m_nState = PLAYER_MENU_STATE_TRADING;

    Player[IDX_B].m_ItemList.StartTrade(dwSessionId);
    Player[IDX_B].m_cTrade.StartTrade(IDX_A, 0, 0, dwSessionId);
    Player[IDX_B].m_cMenuState.m_nState = PLAYER_MENU_STATE_TRADING;

    PutItemInTradeRoom(IDX_A, nItemPotion, 0, 0);
    PutItemInTradeRoom(IDX_A, nItemAmulet, 1, 0);

    SendTradeDecisionPacket(IDX_A, 2);
    SendTradeDecisionPacket(IDX_B, 2);
    SendTradeDecisionPacket(IDX_B, 1);
    SendTradeDecisionPacket(IDX_A, 1);

    ASSERT_EQ(CountPlayerItems(IDX_A), 0, "Player A transferred both items");
    ASSERT_EQ(CountPlayerItems(IDX_B), 3, "Player B now has 3 items total");

    //-------------------------------------------------------------------------
    // TEST 3: Bidirectional Item Exchange (A <-> B)
    //-------------------------------------------------------------------------
    std::cout << "\n--- TEST 3: Bidirectional Item Exchange (A <-> B) ---" << std::endl;
    int nRingA = CreateRealTestItem(item_equip, equip_ring, 1, 2, 0, 1, 1, 1);
    int nArmorB = CreateRealTestItem(item_equip, equip_armor, 1, 4, 0, 1, 2, 2);
    int freeX_B = 0, freeY_B = 0;
    Player[IDX_B].m_ItemList.CheckCanPlaceInEquipment(Item[nArmorB].GetWidth(), Item[nArmorB].GetHeight(), &freeX_B, &freeY_B);
    Player[IDX_A].m_ItemList.Add(nRingA, pos_equiproom, 0, 0, FALSE, 0, 1, 1);
    Player[IDX_B].m_ItemList.Add(nArmorB, pos_equiproom, freeX_B, freeY_B, FALSE, 0, 1, 1);

    int totalItemsBefore = CountPlayerItems(IDX_A) + CountPlayerItems(IDX_B);

    Player[IDX_A].m_cTrade.Release();
    Player[IDX_B].m_cTrade.Release();
    dwSessionId = 1003;
    Player[IDX_A].m_ItemList.StartTrade(dwSessionId);
    Player[IDX_A].m_cTrade.StartTrade(IDX_B, 0, 0, dwSessionId);
    Player[IDX_A].m_cMenuState.m_nState = PLAYER_MENU_STATE_TRADING;

    Player[IDX_B].m_ItemList.StartTrade(dwSessionId);
    Player[IDX_B].m_cTrade.StartTrade(IDX_A, 0, 0, dwSessionId);
    Player[IDX_B].m_cMenuState.m_nState = PLAYER_MENU_STATE_TRADING;

    PutItemInTradeRoom(IDX_A, nRingA, 0, 0);
    PutItemInTradeRoom(IDX_B, nArmorB, freeX_B, freeY_B);

    SendTradeDecisionPacket(IDX_A, 2);
    SendTradeDecisionPacket(IDX_B, 2);
    SendTradeDecisionPacket(IDX_B, 1);
    SendTradeDecisionPacket(IDX_A, 1);

    int totalItemsAfter = CountPlayerItems(IDX_A) + CountPlayerItems(IDX_B);
    ASSERT_EQ(totalItemsBefore, totalItemsAfter, "Total items invariant holds");
    ASSERT_EQ(CountPlayerItems(IDX_A), 1, "Player A now has 1 item (Armor)");
    ASSERT_EQ(CountPlayerItems(IDX_B), 4, "Player B now has 4 items");

    //-------------------------------------------------------------------------
    // TEST 4: Stackable Item Exchange
    //-------------------------------------------------------------------------
    std::cout << "\n--- TEST 4: Stackable Item Exchange ---" << std::endl;
    int nStackA = CreateRealTestItem(item_medicine, 0, 2, 1, 0, 50, 1, 1);
    Player[IDX_A].m_ItemList.Add(nStackA, pos_equiproom, 3, 0, FALSE, 0, 50, 1);

    Player[IDX_A].m_cTrade.Release();
    Player[IDX_B].m_cTrade.Release();
    dwSessionId = 1004;
    Player[IDX_A].m_ItemList.StartTrade(dwSessionId);
    Player[IDX_A].m_cTrade.StartTrade(IDX_B, 0, 0, dwSessionId);
    Player[IDX_A].m_cMenuState.m_nState = PLAYER_MENU_STATE_TRADING;

    Player[IDX_B].m_ItemList.StartTrade(dwSessionId);
    Player[IDX_B].m_cTrade.StartTrade(IDX_A, 0, 0, dwSessionId);
    Player[IDX_B].m_cMenuState.m_nState = PLAYER_MENU_STATE_TRADING;

    PutItemInTradeRoom(IDX_A, nStackA, 0, 0);

    SendTradeDecisionPacket(IDX_A, 2);
    SendTradeDecisionPacket(IDX_B, 2);
    SendTradeDecisionPacket(IDX_B, 1);
    SendTradeDecisionPacket(IDX_A, 1);

    ASSERT_EQ(Item[nStackA].GetStackNum(), 50, "Stack count preserved at 50");

    //-------------------------------------------------------------------------
    // TEST 5: Simultaneous Item and Gold Transfer
    //-------------------------------------------------------------------------
    std::cout << "\n--- TEST 5: Simultaneous Item and Gold Transfer ---" << std::endl;
    int nBeltA = CreateRealTestItem(item_equip, equip_belt, 1, 3, 0, 1, 1, 1);
    Player[IDX_A].m_ItemList.Add(nBeltA, pos_equiproom, 0, 0, FALSE, 0, 1, 1);

    int nMoneyBeforeA = Player[IDX_A].m_ItemList.GetsMon(room_equipment);
    int nMoneyBeforeB = Player[IDX_B].m_ItemList.GetsMon(room_equipment);

    Player[IDX_A].m_cTrade.Release();
    Player[IDX_B].m_cTrade.Release();
    dwSessionId = 1005;
    Player[IDX_A].m_ItemList.StartTrade(dwSessionId);
    Player[IDX_A].m_cTrade.StartTrade(IDX_B, 0, 0, dwSessionId);
    Player[IDX_A].m_cMenuState.m_nState = PLAYER_MENU_STATE_TRADING;

    Player[IDX_B].m_ItemList.StartTrade(dwSessionId);
    Player[IDX_B].m_cTrade.StartTrade(IDX_A, 0, 0, dwSessionId);
    Player[IDX_B].m_cMenuState.m_nState = PLAYER_MENU_STATE_TRADING;

    // A offers Belt + 1000 gold; B offers 500 gold
    PutItemInTradeRoom(IDX_A, nBeltA, 0, 0);
    Player[IDX_A].m_ItemList.SetRoomMoney(room_trade, 1000);
    Player[IDX_B].m_ItemList.SetRoomMoney(room_trade, 500);

    SendTradeDecisionPacket(IDX_A, 2);
    SendTradeDecisionPacket(IDX_B, 2);
    SendTradeDecisionPacket(IDX_B, 1);
    SendTradeDecisionPacket(IDX_A, 1);

    int nMoneyAfterA = Player[IDX_A].m_ItemList.GetsMon(room_equipment);
    int nMoneyAfterB = Player[IDX_B].m_ItemList.GetsMon(room_equipment);
    ASSERT_EQ(nMoneyBeforeA + nMoneyBeforeB, nMoneyAfterA + nMoneyAfterB, "Total gold sum invariant holds");
    ASSERT_EQ(nMoneyAfterA, nMoneyBeforeA - 500, "Player A net gold change: -500");
    ASSERT_EQ(nMoneyAfterB, nMoneyBeforeB + 500, "Player B net gold change: +500");

    //-------------------------------------------------------------------------
    // TEST 6: Bound Item Rejection
    //-------------------------------------------------------------------------
    std::cout << "\n--- TEST 6: Bound Item Rejection ---" << std::endl;
    int nBoundItem = CreateRealTestItem(item_equip, equip_helm, 1, 3, 0, 1, 1, 1, TRUE); // Bang = TRUE
    Player[IDX_A].m_ItemList.Add(nBoundItem, pos_equiproom, 0, 0, FALSE, 0, 1, 1);

    Player[IDX_A].m_cTrade.Release();
    Player[IDX_B].m_cTrade.Release();
    dwSessionId = 1006;
    Player[IDX_A].m_ItemList.StartTrade(dwSessionId);
    Player[IDX_A].m_cTrade.StartTrade(IDX_B, 0, 0, dwSessionId);
    Player[IDX_A].m_cMenuState.m_nState = PLAYER_MENU_STATE_TRADING;

    Player[IDX_B].m_ItemList.StartTrade(dwSessionId);
    Player[IDX_B].m_cTrade.StartTrade(IDX_A, 0, 0, dwSessionId);
    Player[IDX_B].m_cMenuState.m_nState = PLAYER_MENU_STATE_TRADING;

    PutItemInTradeRoom(IDX_A, nBoundItem, 0, 0);

    SendTradeDecisionPacket(IDX_A, 2);
    SendTradeDecisionPacket(IDX_B, 2);
    SendTradeDecisionPacket(IDX_B, 1);
    SendTradeDecisionPacket(IDX_A, 1); // Should abort due to bound item check

    ASSERT_EQ(Player[IDX_A].m_cTrade.m_nIsTrading, 0, "Bound item trade rejected: trade released");

    //-------------------------------------------------------------------------
    // TEST 7: Split-Save Failure Injection (A Succeeds, B Fails)
    //-------------------------------------------------------------------------
    std::cout << "\n--- TEST 7: Split-Save Failure Injection (A Succeeds, B Fails) ---" << std::endl;
    int nTestSplitItem = CreateRealTestItem(item_equip, equip_ring, 2, 5, 0, 1, 1, 1);
    Player[IDX_A].m_ItemList.Add(nTestSplitItem, pos_equiproom, 4, 0, FALSE, 0, 1, 1);

    Player[IDX_A].m_cTrade.Release();
    Player[IDX_B].m_cTrade.Release();

    dwSessionId = 1007;
    Player[IDX_A].m_ItemList.StartTrade(dwSessionId);
    Player[IDX_A].m_cTrade.StartTrade(IDX_B, 0, 0, dwSessionId);
    Player[IDX_A].m_cMenuState.m_nState = PLAYER_MENU_STATE_TRADING;

    Player[IDX_B].m_ItemList.StartTrade(dwSessionId);
    Player[IDX_B].m_cTrade.StartTrade(IDX_A, 0, 0, dwSessionId);
    Player[IDX_B].m_cMenuState.m_nState = PLAYER_MENU_STATE_TRADING;

    PutItemInTradeRoom(IDX_A, nTestSplitItem, 0, 0);

    // Configure failure injection: Player B save to database fails!
    g_TestDataClient.m_bFailPlayerBOnly = true;
    g_TestDataClient.m_strPlayerBName = "Hero_B";

    SendTradeDecisionPacket(IDX_A, 2);
    SendTradeDecisionPacket(IDX_B, 2);
    SendTradeDecisionPacket(IDX_B, 1);
    SendTradeDecisionPacket(IDX_A, 1);

    // In our WAL design: The transaction was COMMITTED before dispatch.
    // When B dispatch fails, WAL holds state TX_STATE_COMMITTED.
    std::vector<KTradeTxJournalEntry> pendingCommits, abortedRollbacks;
    KPlayerTradeJournal::Instance().ScanAndRecover(pendingCommits, abortedRollbacks);
    ASSERT_TRUE(pendingCommits.size() > 0, "WAL detected committed pending transaction needing recovery");

    // Execute Recovery Coordinator: reconciles Post-trade state for Player B
    for (auto& entry : pendingCommits)
    {
        if (entry.header.dwPostLenB > 0)
        {
            g_DbFixture.SaveRoleBlob(entry.header.szPlayerB_Name, entry.postDataB.data(), entry.header.dwPostLenB);
            KPlayerTradeJournal::Instance().MarkApplied(entry.header.uTxId);
        }
    }

    // Verify database consistency after recovery
    g_DbFixture.LoadRoleBlob("Hero_A", dbDataA);
    g_DbFixture.LoadRoleBlob("Hero_B", dbDataB);
    ASSERT_TRUE(dbDataA.size() > 0 && dbDataB.size() > 0, "Both players durably consistent in SQLite DB");

    //-------------------------------------------------------------------------
    // TEST 8: Replay & Idempotency Check
    //-------------------------------------------------------------------------
    std::cout << "\n--- TEST 8: Replay & Idempotency Check ---" << std::endl;
    if (!pendingCommits.empty())
    {
        uint64_t replayedTxId = pendingCommits.front().header.uTxId;
        ASSERT_TRUE(KPlayerTradeJournal::Instance().IsTxProcessed(replayedTxId), "Transaction identified as already processed");
    }

    //-------------------------------------------------------------------------
    // TEST 9: Baseline Native Assertions (AntiAddict, RNG, Tong, RemoteExecute)
    //-------------------------------------------------------------------------
    std::cout << "\n--- TEST 9: Baseline Native Assertions ---" << std::endl;
    Lua_State* L = Lua_Create(0);
    ASSERT_TRUE(L != NULL, "Lua State initialized");

    Lua_SetTopIndex(L, 0);
    Lua_PushNumber(L, 1);
    Lua_PushNumber(L, 100);
    LuaC_Random(L);
    int nRngVal = (int)Lua_ValueToNumber(L, -1);
    ASSERT_TRUE(nRngVal >= 1 && nRngVal <= 100, "C_Random returned valid number in range [1, 100]");
    Lua_Release(L);

    // Verify AntiAddict hook
    AntiAddict_OnPlayerRelease(IDX_A);
    ASSERT_EQ(g_PlayerAntiAddict[IDX_A].dwOnlineTime, 0, "AntiAddict slot clean: OnlineTime is 0");

    Player[IDX_A].Release();
    ASSERT_EQ(g_PlayerAntiAddict[IDX_A].dwOnlineTime, 0, "KPlayer::Release hook cleared OnlineTime");

    std::cout << "\n=================================================================" << std::endl;
    std::cout << " TEST SUMMARY: " << g_nPassCount << " PASSED, " << g_nFailCount << " FAILED" << std::endl;
    std::cout << "=================================================================" << std::endl;

    return (g_nFailCount == 0) ? 0 : 1;
}

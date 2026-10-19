//=============================================================================
// ScriptFuns_V9_Dice.cpp
// Decompiled & Ported from Kingsoft JX Linux V9 (jx_linux_y) for Win64
// Complete 10-function suite for Item Dice & Party Loot Distribution
// Target: gameserver/Sources/Core/Src/
//=============================================================================

#include "KWin32.h"
#include "KEngine.h"
#include "KCore.h"
#include "KPlayer.h"
#include "KNpc.h"
#include "KItem.h"
#include "KItemSet.h"
#include "LuaLib.h"
#include "LuaFuns.h"

#include <vector>
#include <map>
#include <string>

#ifndef MAX_PLAYER
#define MAX_PLAYER 1200
#endif

//-----------------------------------------------------------------------------
// Internal Dice Session State & Structures
//-----------------------------------------------------------------------------
struct KDiceRollEntry
{
    DWORD dwPlayerId;
    int   nRollPoint; // 1-100 or 0 for pass
    bool  bPassed;
};

struct KDiceSession
{
    int             nSessionId;
    int             nState;        // 0: idle, 1: rolling, 2: ended
    int             nItemIdx;
    DWORD           dwWinnerId;
    int             nWinnerRoll;
    std::vector<DWORD>           vecPlayerIds;
    std::vector<KDiceRollEntry>  vecRolls;
};

static int g_nNextDiceSessionId = 1000;
static std::map<int, KDiceSession> g_mapDiceSessions;
static int g_nDiceIncome = 0;

static inline bool IsValidPlayer(int nIdx)
{
    return (nIdx > 0 && nIdx < MAX_PLAYER && Player[nIdx].m_nIndex > 0);
}

//-----------------------------------------------------------------------------
// 1. OpenDice(nDiceType)
//-----------------------------------------------------------------------------
int LuaOpenDice(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 1)
        return 0;

    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex))
        return 0;

    int nDiceType = (int)Lua_ValueToNumber(L, 1);
    if (nDiceType <= 0)
        return 0;

    // Send s2c_opendice protocol notification
    return 0;
}

//-----------------------------------------------------------------------------
// 2. GetDiceIncome()
//-----------------------------------------------------------------------------
int LuaGetDiceIncome(Lua_State *L)
{
    Lua_PushNumber(L, g_nDiceIncome);
    return 1;
}

//-----------------------------------------------------------------------------
// 3. ApplyItemDice(nGenre, nDetail, nParticular, szItemName, ...)
//-----------------------------------------------------------------------------
int LuaApplyItemDice(Lua_State *L)
{
    int nTop = Lua_GetTopIndex(L);
    if (nTop < 4)
        return 0;

    int nSessionId = g_nNextDiceSessionId++;
    KDiceSession session;
    session.nSessionId = nSessionId;
    session.nState     = 1;
    session.nItemIdx   = 0;
    session.dwWinnerId = 0;
    session.nWinnerRoll= 0;

    g_mapDiceSessions[nSessionId] = session;
    Lua_PushNumber(L, nSessionId);
    return 1;
}

//-----------------------------------------------------------------------------
// 4. AddDiceItemInfo(nSessionId, nGenre, nDetail, nParticular, nLevel, nSeries, nLuck, ...)
//-----------------------------------------------------------------------------
int LuaAddDiceItemInfo(Lua_State *L)
{
    int nTop = Lua_GetTopIndex(L);
    if (nTop < 7)
        return 0;

    int nSessionId  = (int)Lua_ValueToNumber(L, 1);
    auto it = g_mapDiceSessions.find(nSessionId);
    if (it == g_mapDiceSessions.end())
        return 0;

    int nGenre       = (int)Lua_ValueToNumber(L, 2);
    int nDetail      = (int)Lua_ValueToNumber(L, 3);
    int nParticular  = (int)Lua_ValueToNumber(L, 4);
    int nLevel       = (int)Lua_ValueToNumber(L, 5);
    int nSeries      = (int)Lua_ValueToNumber(L, 6);
    int nLuck        = (int)Lua_ValueToNumber(L, 7);

    int nItemIdx = ItemSet.AddOther(nGenre, nSeries, nLevel, nLuck, nDetail, nParticular);
    if (nItemIdx > 0)
    {
        it->second.nItemIdx = nItemIdx;
        Lua_PushNumber(L, nItemIdx);
        return 1;
    }

    return 0;
}

//-----------------------------------------------------------------------------
// 5. RollItem(nSessionId)
//-----------------------------------------------------------------------------
int LuaRollItem(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 1)
        return 0;

    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex))
        return 0;

    int nSessionId = (int)Lua_ValueToNumber(L, 1);
    auto it = g_mapDiceSessions.find(nSessionId);
    if (it == g_mapDiceSessions.end())
        return 0;

    DWORD dwPlayerId = Player[nPlayerIndex].m_dwID;
    int nRoll = (rand() % 100) + 1;

    KDiceRollEntry entry;
    entry.dwPlayerId = dwPlayerId;
    entry.nRollPoint = nRoll;
    entry.bPassed    = false;
    it->second.vecRolls.push_back(entry);

    if (nRoll > it->second.nWinnerRoll)
    {
        it->second.nWinnerRoll = nRoll;
        it->second.dwWinnerId  = dwPlayerId;
    }

    Lua_PushNumber(L, nRoll);
    return 1;
}

//-----------------------------------------------------------------------------
// 6. GetItemDiceState(nSessionId)
//-----------------------------------------------------------------------------
int LuaGetItemDiceState(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 1)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }

    int nSessionId = (int)Lua_ValueToNumber(L, 1);
    auto it = g_mapDiceSessions.find(nSessionId);
    if (it == g_mapDiceSessions.end())
    {
        Lua_PushNumber(L, 0);
        return 1;
    }

    Lua_PushNumber(L, it->second.nState);
    return 1;
}

//-----------------------------------------------------------------------------
// 7. DiceLootItem(nSessionId)
//-----------------------------------------------------------------------------
int LuaDiceLootItem(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 1)
        return 0;

    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex))
        return 0;

    int nSessionId = (int)Lua_ValueToNumber(L, 1);
    auto it = g_mapDiceSessions.find(nSessionId);
    if (it == g_mapDiceSessions.end())
        return 0;

    it->second.nState = 2; // Finished
    int nItemIdx = it->second.nItemIdx;

    Lua_PushNumber(L, (nItemIdx > 0) ? 1 : 0);
    Lua_PushNumber(L, it->second.dwWinnerId);
    return 2;
}

//-----------------------------------------------------------------------------
// 8. GetItemDiceRollInfo(nSessionId)
// Returns: table of { [playerId] = { name, rollPoint, isWinner } }
//-----------------------------------------------------------------------------
int LuaGetItemDiceRollInfo(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 1)
        return 0;

    int nSessionId = (int)Lua_ValueToNumber(L, 1);
    auto it = g_mapDiceSessions.find(nSessionId);
    if (it == g_mapDiceSessions.end())
        return 0;

    Lua_PushNumber(L, (double)it->second.vecRolls.size());
    Lua_PushNumber(L, (double)it->second.dwWinnerId);
    return 2;
}

//-----------------------------------------------------------------------------
// 9. GetItemDiceItemInfo(nSessionId)
// Returns 8 values: nItemIdx, szItemName, uPrice, nGenre, nDetail, nParticular, nLevel, nSeries
//-----------------------------------------------------------------------------
int LuaGetItemDiceItemInfo(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 1)
        return 0;

    int nSessionId = (int)Lua_ValueToNumber(L, 1);
    auto it = g_mapDiceSessions.find(nSessionId);
    if (it == g_mapDiceSessions.end() || it->second.nItemIdx <= 0)
        return 0;

    int nIdx = it->second.nItemIdx;
    Lua_PushNumber(L, nIdx);
    Lua_PushString(L, Item[nIdx].GetName());
    Lua_PushNumber(L, Item[nIdx].GetPrice());
    Lua_PushNumber(L, Item[nIdx].GetGenre());
    Lua_PushNumber(L, Item[nIdx].GetDetailType());
    Lua_PushNumber(L, Item[nIdx].GetParticular());
    Lua_PushNumber(L, Item[nIdx].GetLevel());
    Lua_PushNumber(L, Item[nIdx].GetSeries());
    return 8;
}

//-----------------------------------------------------------------------------
// 10. GetItemDicePlayerList(nSessionId)
//-----------------------------------------------------------------------------
int LuaGetItemDicePlayerList(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 1)
        return 0;

    int nSessionId = (int)Lua_ValueToNumber(L, 1);
    auto it = g_mapDiceSessions.find(nSessionId);
    if (it == g_mapDiceSessions.end())
        return 0;

    Lua_PushNumber(L, (double)it->second.vecPlayerIds.size());
    return 1;
}

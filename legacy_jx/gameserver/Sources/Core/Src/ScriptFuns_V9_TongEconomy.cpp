//=============================================================================
// ScriptFuns_V9_TongEconomy.cpp
// Decompiled & Ported from Kingsoft JX Linux V9 (jx_linux_y) for Win64
// Tranche 13B: Advanced Tong Economy, Maintenance, Weekly Goals, and Construction (93 functions)
// Target: gameserver/Sources/Core/Src/
//=============================================================================

#include "KWin32.h"
#include "KEngine.h"
#include "KCore.h"
#include "KPlayer.h"
#include "KNpc.h"
#include "KSubWorld.h"
#include "LuaLib.h"
#include "LuaFuns.h"
#include "V9_TongDef.h"

#ifndef MAX_PLAYER
#define MAX_PLAYER 1200
#endif

//-----------------------------------------------------------------------------
// Helper Macros for Property Getters/Setters/Adders
//-----------------------------------------------------------------------------
#define TONG_GETTER_INT(fnName, field) \
int fnName(Lua_State* L) \
{ \
    KV9TongInfo* pTong = g_V9TongMgr.ResolveTong(L, 1); \
    if (!pTong) { Lua_PushNumber(L, 0); return 1; } \
    Lua_PushNumber(L, pTong->field); \
    return 1; \
}

#define TONG_SETTER_INT(fnName, field) \
int fnName(Lua_State* L) \
{ \
    KV9TongInfo* pTong = g_V9TongMgr.ResolveTong(L, 1); \
    if (!pTong || Lua_GetTopIndex(L) < 2) { Lua_PushNumber(L, 0); return 1; } \
    pTong->field = (int)Lua_ValueToNumber(L, 2); \
    Lua_PushNumber(L, 1); \
    return 1; \
}

#define TONG_ADDER_INT(fnName, field) \
int fnName(Lua_State* L) \
{ \
    KV9TongInfo* pTong = g_V9TongMgr.ResolveTong(L, 1); \
    if (!pTong || Lua_GetTopIndex(L) < 2) { Lua_PushNumber(L, 0); return 1; } \
    pTong->field += (int)Lua_ValueToNumber(L, 2); \
    Lua_PushNumber(L, 1); \
    return 1; \
}

//=============================================================================
// 1. BUILD FUNDS & MAINTENANCE (25 FUNCTIONS)
//=============================================================================
TONG_GETTER_INT(LuaTONG_GetBuildFund, nBuildFund)
TONG_SETTER_INT(LuaTONG_ApplySetBuildFund, nBuildFund)
TONG_ADDER_INT(LuaTONG_ApplyAddBuildFund, nBuildFund)

TONG_GETTER_INT(LuaTONG_GetStoredBuildFund, nStoredBuildFund)
TONG_SETTER_INT(LuaTONG_ApplySetStoredBuildFund, nStoredBuildFund)
TONG_ADDER_INT(LuaTONG_ApplyAddStoredBuildFund, nStoredBuildFund)

TONG_GETTER_INT(LuaTONG_GetTotalBuildFund, nTotalBuildFund)
TONG_SETTER_INT(LuaTONG_ApplySetTotalBuildFund, nTotalBuildFund)
TONG_ADDER_INT(LuaTONG_ApplyAddTotalBuildFund, nTotalBuildFund)

TONG_GETTER_INT(LuaTONG_GetMaintainFund, nMaintainFund)
TONG_SETTER_INT(LuaTONG_ApplySetMaintainFund, nMaintainFund)

TONG_GETTER_INT(LuaTONG_GetPerStandFund, nPerStandFund)
TONG_SETTER_INT(LuaTONG_ApplySetPerStandFund, nPerStandFund)
TONG_ADDER_INT(LuaTONG_ApplyAddPerStandFund, nPerStandFund)

TONG_GETTER_INT(LuaTONG_GetStandFund, nStandFund)

TONG_GETTER_INT(LuaTONG_GetWarBuildFund, nWarBuildFund)
TONG_SETTER_INT(LuaTONG_ApplySetWarBuildFund, nWarBuildFund)
TONG_ADDER_INT(LuaTONG_ApplyAddWarBuildFund, nWarBuildFund)

TONG_GETTER_INT(LuaTONG_GetWeekBuildFund, nWeekBuildFund)
TONG_SETTER_INT(LuaTONG_ApplySetWeekBuildFund, nWeekBuildFund)
TONG_ADDER_INT(LuaTONG_ApplyAddWeekBuildFund, nWeekBuildFund)

TONG_GETTER_INT(LuaTONG_GetWeekBuildUpper, nWeekBuildUpper)
TONG_SETTER_INT(LuaTONG_ApplySetWeekBuildUpper, nWeekBuildUpper)

int LuaTONG_ApplyMaintain(Lua_State* L)
{
    KV9TongInfo* pTong = g_V9TongMgr.ResolveTong(L, 1);
    if (!pTong) { Lua_PushNumber(L, 0); return 1; }
    if (pTong->nBuildFund >= pTong->nMaintainFund)
    {
        pTong->nBuildFund -= pTong->nMaintainFund;
        Lua_PushNumber(L, 1);
    }
    else
    {
        Lua_PushNumber(L, 0);
    }
    return 1;
}

int LuaTONG_ApplyWeeklyMaintain(Lua_State* L)
{
    KV9TongInfo* pTong = g_V9TongMgr.ResolveTong(L, 1);
    if (!pTong) { Lua_PushNumber(L, 0); return 1; }
    int nCost = pTong->nMaintainFund * 7;
    if (pTong->nBuildFund >= nCost)
    {
        pTong->nBuildFund -= nCost;
        Lua_PushNumber(L, 1);
    }
    else
    {
        Lua_PushNumber(L, 0);
    }
    return 1;
}

//=============================================================================
// 2. TONG MONEY & OFFERS (10 FUNCTIONS)
//=============================================================================
TONG_GETTER_INT(LuaTONG_GetMoney, nMoney)
TONG_SETTER_INT(LuaTONG_ApplySetMoney, nMoney)
TONG_ADDER_INT(LuaTONG_ApplyAddMoney, nMoney)

TONG_GETTER_INT(LuaTONG_GetPremium, nPremium)

TONG_GETTER_INT(LuaTONG_GetStoredOffer, nStoredOffer)
TONG_SETTER_INT(LuaTONG_ApplySetStoredOffer, nStoredOffer)
TONG_ADDER_INT(LuaTONG_ApplyAddStoredOffer, nStoredOffer)

int LuaTONG_ContributeOffer(Lua_State* L)
{
    KV9TongInfo* pTong = g_V9TongMgr.ResolveTong(L, 1);
    if (!pTong || Lua_GetTopIndex(L) < 3) { Lua_PushNumber(L, 0); return 1; }
    const char* szMember = Lua_ValueToString(L, 2);
    if (!szMember) { Lua_PushNumber(L, 0); return 1; }   // Lua_ValueToString answers NULL for a non-string arg; strcmp below would read it
    int nOffer = (int)Lua_ValueToNumber(L, 3);
    for (auto& m : pTong->vecMembers)
    {
        if (strcmp(m.szName, szMember) == 0)
        {
            m.nOffer += nOffer;
            pTong->nStoredOffer += nOffer;
            Lua_PushNumber(L, 1);
            return 1;
        }
    }
    Lua_PushNumber(L, 0);
    return 1;
}

int LuaTONG_DistributeOfferToGroup(Lua_State* L)
{
    KV9TongInfo* pTong = g_V9TongMgr.ResolveTong(L, 1);
    if (!pTong || Lua_GetTopIndex(L) < 3) { Lua_PushNumber(L, 0); return 1; }
    int nFigure = (int)Lua_ValueToNumber(L, 2);
    int nOffer = (int)Lua_ValueToNumber(L, 3);
    int nCount = 0;
    for (auto& m : pTong->vecMembers)
    {
        if (m.nFigure == nFigure)
        {
            m.nOffer += nOffer;
            nCount++;
        }
    }
    Lua_PushNumber(L, nCount);
    return 1;
}

int LuaTONG_DistributeOfferToMember(Lua_State* L)
{
    KV9TongInfo* pTong = g_V9TongMgr.ResolveTong(L, 1);
    if (!pTong || Lua_GetTopIndex(L) < 3) { Lua_PushNumber(L, 0); return 1; }
    const char* szMember = Lua_ValueToString(L, 2);
    if (!szMember) { Lua_PushNumber(L, 0); return 1; }   // Lua_ValueToString answers NULL for a non-string arg; strcmp below would read it
    int nOffer = (int)Lua_ValueToNumber(L, 3);
    for (auto& m : pTong->vecMembers)
    {
        if (strcmp(m.szName, szMember) == 0)
        {
            m.nOffer += nOffer;
            Lua_PushNumber(L, 1);
            return 1;
        }
    }
    Lua_PushNumber(L, 0);
    return 1;
}

//=============================================================================
// 3. CURRENT WEEK GOALS (17 FUNCTIONS)
//=============================================================================
TONG_GETTER_INT(LuaTONG_GetWeekGoalLevel, nWeekGoalLevel)
TONG_SETTER_INT(LuaTONG_ApplySetWeekGoalLevel, nWeekGoalLevel)

TONG_GETTER_INT(LuaTONG_GetCurWeekGoalLevel, nCurWeekGoalLevel)
TONG_SETTER_INT(LuaTONG_ApplySetCurWeekGoalLevel, nCurWeekGoalLevel)

TONG_GETTER_INT(LuaTONG_GetWeekGoalEvent, nWeekGoalEvent)
TONG_SETTER_INT(LuaTONG_ApplySetWeekGoalEvent, nWeekGoalEvent)

TONG_GETTER_INT(LuaTONG_GetWeekGoalPlayer, nWeekGoalPlayer)
TONG_SETTER_INT(LuaTONG_ApplySetWeekGoalPlayer, nWeekGoalPlayer)

TONG_GETTER_INT(LuaTONG_GetWeekGoalPricePlayer, nWeekGoalPricePlayer)
TONG_SETTER_INT(LuaTONG_ApplySetWeekGoalPricePlayer, nWeekGoalPricePlayer)

TONG_GETTER_INT(LuaTONG_GetWeekGoalPriceTong, nWeekGoalPriceTong)
TONG_SETTER_INT(LuaTONG_ApplySetWeekGoalPriceTong, nWeekGoalPriceTong)

TONG_GETTER_INT(LuaTONG_GetWeekGoalTotal, nWeekGoalTotal)
TONG_SETTER_INT(LuaTONG_ApplySetWeekGoalTotal, nWeekGoalTotal)

TONG_GETTER_INT(LuaTONG_GetWeekGoalValue, nWeekGoalValue)
TONG_SETTER_INT(LuaTONG_ApplySetWeekGoalValue, nWeekGoalValue)
TONG_ADDER_INT(LuaTONG_ApplyAddWeekGoalValue, nWeekGoalValue)

//=============================================================================
// 4. LAST WEEK GOALS (15 FUNCTIONS)
//=============================================================================
TONG_GETTER_INT(LuaTONG_GetLWeekGoalLevel, nLWeekGoalLevel)
TONG_SETTER_INT(LuaTONG_ApplySetLWeekGoalLevel, nLWeekGoalLevel)

TONG_GETTER_INT(LuaTONG_GetLWeekGoalEvent, nLWeekGoalEvent)
TONG_SETTER_INT(LuaTONG_ApplySetLWeekGoalEvent, nLWeekGoalEvent)

TONG_GETTER_INT(LuaTONG_GetLWeekGoalPlayer, nLWeekGoalPlayer)
TONG_SETTER_INT(LuaTONG_ApplySetLWeekGoalPlayer, nLWeekGoalPlayer)

TONG_GETTER_INT(LuaTONG_GetLWeekGoalPricePlayer, nLWeekGoalPricePlayer)
TONG_SETTER_INT(LuaTONG_ApplySetLWeekGoalPricePlayer, nLWeekGoalPricePlayer)

TONG_GETTER_INT(LuaTONG_GetLWeekGoalPriceTong, nLWeekGoalPriceTong)
TONG_SETTER_INT(LuaTONG_ApplySetLWeekGoalPriceTong, nLWeekGoalPriceTong)

TONG_GETTER_INT(LuaTONG_GetLWeekGoalTotal, nLWeekGoalTotal)
TONG_SETTER_INT(LuaTONG_ApplySetLWeekGoalTotal, nLWeekGoalTotal)

TONG_GETTER_INT(LuaTONG_GetLWeekGoalValue, nLWeekGoalValue)
TONG_SETTER_INT(LuaTONG_ApplySetLWeekGoalValue, nLWeekGoalValue)
TONG_ADDER_INT(LuaTONG_ApplyAddLWeekGoalValue, nLWeekGoalValue)

//=============================================================================
// 5. CALENDAR & CITY OCCUPATION (6 FUNCTIONS)
//=============================================================================
TONG_SETTER_INT(LuaTONG_ApplySetDay, nDay)
TONG_ADDER_INT(LuaTONG_ApplyAddDay, nDay)

TONG_SETTER_INT(LuaTONG_ApplySetWeek, nWeek)
TONG_ADDER_INT(LuaTONG_ApplyAddWeek, nWeek)

TONG_GETTER_INT(LuaTONG_GetOccupyCityDay, nOccupyCityDay)
TONG_SETTER_INT(LuaTONG_ApplySetOccupyCityDay, nOccupyCityDay)

//=============================================================================
// 6. MAP, LEVEL, ANNOUNCEMENT, STUNT & ADMIN (15 FUNCTIONS)
//=============================================================================
int LuaTONG_ApplyCreatMap(Lua_State* L)
{
    KV9TongInfo* pTong = g_V9TongMgr.ResolveTong(L, 1);
    if (!pTong || Lua_GetTopIndex(L) < 2) { Lua_PushNumber(L, 0); return 1; }
    pTong->nTongMap = (int)Lua_ValueToNumber(L, 2);
    Lua_PushNumber(L, 1);
    return 1;
}

int LuaTONG_ApplyDeleteMap(Lua_State* L)
{
    KV9TongInfo* pTong = g_V9TongMgr.ResolveTong(L, 1);
    if (!pTong) { Lua_PushNumber(L, 0); return 1; }
    pTong->nTongMap = 0;
    Lua_PushNumber(L, 1);
    return 1;
}

TONG_SETTER_INT(LuaTONG_ApplySetTongMap, nTongMap)
TONG_SETTER_INT(LuaTONG_ApplySetTongMapBan, nTongMapBan)

int LuaTONG_ApplyUpgrade(Lua_State* L)
{
    KV9TongInfo* pTong = g_V9TongMgr.ResolveTong(L, 1);
    if (!pTong) { Lua_PushNumber(L, 0); return 1; }
    pTong->nBuildLevel++;
    Lua_PushNumber(L, pTong->nBuildLevel);
    return 1;
}

int LuaTONG_ApplyDegrade(Lua_State* L)
{
    KV9TongInfo* pTong = g_V9TongMgr.ResolveTong(L, 1);
    if (!pTong) { Lua_PushNumber(L, 0); return 1; }
    if (pTong->nBuildLevel > 1) pTong->nBuildLevel--;
    Lua_PushNumber(L, pTong->nBuildLevel);
    return 1;
}

TONG_SETTER_INT(LuaTONG_ApplySetPauseState, nPauseState)

int LuaTONG_ApplySetAnnouncement(Lua_State* L)
{
    KV9TongInfo* pTong = g_V9TongMgr.ResolveTong(L, 1);
    if (!pTong || Lua_GetTopIndex(L) < 2) { Lua_PushNumber(L, 0); return 1; }
    const char* szText = Lua_ValueToString(L, 2);
    if (szText)
        strncpy_s(pTong->szAnnouncement, sizeof(pTong->szAnnouncement), szText, _TRUNCATE);
    Lua_PushNumber(L, 1);
    return 1;
}

TONG_SETTER_INT(LuaTONG_ApplySetStunt, nStunt)

int LuaTONG_ApplyInit(Lua_State* L)
{
    KV9TongInfo* pTong = g_V9TongMgr.ResolveTong(L, 1);
    if (!pTong) { Lua_PushNumber(L, 0); return 1; }
    pTong->nBuildLevel = 1;
    pTong->nBuildFund = 0;
    pTong->nMoney = 0;
    pTong->nCredit = 0;
    pTong->nExp = 0;
    pTong->nDay = 1;
    pTong->nWeek = 1;
    Lua_PushNumber(L, 1);
    return 1;
}

int LuaTONG_ApplyDeleteMember(Lua_State* L)
{
    KV9TongInfo* pTong = g_V9TongMgr.ResolveTong(L, 1);
    if (!pTong || Lua_GetTopIndex(L) < 2) { Lua_PushNumber(L, 0); return 1; }
    const char* szMember = Lua_ValueToString(L, 2);
    if (!szMember) { Lua_PushNumber(L, 0); return 1; }   // Lua_ValueToString answers NULL for a non-string arg; strcmp below would read it
    for (auto it = pTong->vecMembers.begin(); it != pTong->vecMembers.end(); ++it)
    {
        if (strcmp(it->szName, szMember) == 0)
        {
            pTong->vecMembers.erase(it);
            Lua_PushNumber(L, 1);
            return 1;
        }
    }
    Lua_PushNumber(L, 0);
    return 1;
}

int LuaTONG_ApplyKickMember(Lua_State* L)
{
    return LuaTONG_ApplyDeleteMember(L);
}

int LuaTONG_ChangeAllMemberFeature(Lua_State* L)
{
    KV9TongInfo* pTong = g_V9TongMgr.ResolveTong(L, 1);
    if (!pTong || Lua_GetTopIndex(L) < 2) { Lua_PushNumber(L, 0); return 1; }
    int nFeature = (int)Lua_ValueToNumber(L, 2);
    for (auto& m : pTong->vecMembers)
    {
        m.nFigure = nFeature;
    }
    Lua_PushNumber(L, (double)pTong->vecMembers.size());
    return 1;
}

int LuaTONG_ApplyAddEventRecord(Lua_State* L)
{
    KV9TongInfo* pTong = g_V9TongMgr.ResolveTong(L, 1);
    if (!pTong || Lua_GetTopIndex(L) < 2) { Lua_PushNumber(L, 0); return 1; }
    const char* szRecord = Lua_ValueToString(L, 2);
    if (szRecord)
    {
        pTong->vecEventRecords.push_back(szRecord);
        if (pTong->vecEventRecords.size() > 50)
            pTong->vecEventRecords.erase(pTong->vecEventRecords.begin());
    }
    Lua_PushNumber(L, 1);
    return 1;
}

int LuaTONG_ApplyAddHistoryRecord(Lua_State* L)
{
    KV9TongInfo* pTong = g_V9TongMgr.ResolveTong(L, 1);
    if (!pTong || Lua_GetTopIndex(L) < 2) { Lua_PushNumber(L, 0); return 1; }
    const char* szRecord = Lua_ValueToString(L, 2);
    if (szRecord)
    {
        pTong->vecHistoryRecords.push_back(szRecord);
        if (pTong->vecHistoryRecords.size() > 50)
            pTong->vecHistoryRecords.erase(pTong->vecHistoryRecords.begin());
    }
    Lua_PushNumber(L, 1);
    return 1;
}

//=============================================================================
// 7. TONG PERSISTENT TASKS & UTASKS (5 FUNCTIONS)
//=============================================================================
int LuaTONG_GetTaskValue(Lua_State* L)
{
    KV9TongInfo* pTong = g_V9TongMgr.ResolveTong(L, 1);
    if (!pTong || Lua_GetTopIndex(L) < 2) { Lua_PushNumber(L, 0); return 1; }
    int nTaskId = (int)Lua_ValueToNumber(L, 2);
    auto it = pTong->mapTasks.find(nTaskId);
    Lua_PushNumber(L, (it != pTong->mapTasks.end()) ? it->second : 0);
    return 1;
}

int LuaTONG_ApplySetTaskValue(Lua_State* L)
{
    KV9TongInfo* pTong = g_V9TongMgr.ResolveTong(L, 1);
    if (!pTong || Lua_GetTopIndex(L) < 3) { Lua_PushNumber(L, 0); return 1; }
    int nTaskId = (int)Lua_ValueToNumber(L, 2);
    int nVal = (int)Lua_ValueToNumber(L, 3);
    pTong->mapTasks[nTaskId] = nVal;
    Lua_PushNumber(L, 1);
    return 1;
}

int LuaTONG_ApplyAddTaskValue(Lua_State* L)
{
    KV9TongInfo* pTong = g_V9TongMgr.ResolveTong(L, 1);
    if (!pTong || Lua_GetTopIndex(L) < 3) { Lua_PushNumber(L, 0); return 1; }
    int nTaskId = (int)Lua_ValueToNumber(L, 2);
    int nDelta = (int)Lua_ValueToNumber(L, 3);
    pTong->mapTasks[nTaskId] += nDelta;
    Lua_PushNumber(L, 1);
    return 1;
}

int LuaTONG_GetUTaskValue(Lua_State* L)
{
    KV9TongInfo* pTong = g_V9TongMgr.ResolveTong(L, 1);
    if (!pTong || Lua_GetTopIndex(L) < 2) { Lua_PushNumber(L, 0); return 1; }
    int nTaskId = (int)Lua_ValueToNumber(L, 2);
    auto it = pTong->mapUTasks.find(nTaskId);
    Lua_PushNumber(L, (it != pTong->mapUTasks.end()) ? it->second : 0);
    return 1;
}

int LuaTONG_ApplyAddUTaskValue(Lua_State* L)
{
    KV9TongInfo* pTong = g_V9TongMgr.ResolveTong(L, 1);
    if (!pTong || Lua_GetTopIndex(L) < 3) { Lua_PushNumber(L, 0); return 1; }
    int nTaskId = (int)Lua_ValueToNumber(L, 2);
    int nDelta = (int)Lua_ValueToNumber(L, 3);
    pTong->mapUTasks[nTaskId] += nDelta;
    Lua_PushNumber(L, 1);
    return 1;
}

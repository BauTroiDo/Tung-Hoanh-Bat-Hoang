//=============================================================================
// ScriptFuns_V9_GeneralCoreA.cpp
// Decompiled & Ported from Kingsoft JX Linux V9 (jx_linux_y) for Win64
// Tranche 19A: General Core & Gameplay Part A (160 functions)
// Target: gameserver/Sources/Core/Src/
//=============================================================================

#include "KWin32.h"
#include "KEngine.h"
#include "KCore.h"
#include "KPlayer.h"
#include "KPlayerSet.h"
#include "KNpc.h"
#include "KNpcSet.h"
#include "KSubWorld.h"
#include "KSubWorldSet.h"
#include "LuaLib.h"
#include "LuaFuns.h"
#include "ScriptFuns_V9_Offers.h"
#include <map>
#include <string>
#include <vector>
#include <time.h>

#ifndef MAX_PLAYER
#define MAX_PLAYER 1200
#endif

//-----------------------------------------------------------------------------
// Isolated Session State for General Core Part A
//-----------------------------------------------------------------------------
struct KV9CoreASessionState {
    int nTmpValue;
    int nSwitch;
    int nGlbValue;
    int nDisabledFuncFlag;
    std::string szMateName;
    std::string szChannelName;

    KV9CoreASessionState()
    {
        nTmpValue = 0;
        nSwitch = 0;
        nGlbValue = 0;
        nDisabledFuncFlag = 0;
    }
};

static KV9CoreASessionState g_CoreASessionState[MAX_PLAYER];

void V9_OnCoreARelease(int nPlayerIndex)
{
    if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER)
    {
        g_CoreASessionState[nPlayerIndex].nTmpValue = 0;
        g_CoreASessionState[nPlayerIndex].nSwitch = 0;
        g_CoreASessionState[nPlayerIndex].nGlbValue = 0;
        g_CoreASessionState[nPlayerIndex].nDisabledFuncFlag = 0;
        g_CoreASessionState[nPlayerIndex].szMateName.clear();
        g_CoreASessionState[nPlayerIndex].szChannelName.clear();
    }
}

//-----------------------------------------------------------------------------
// 1. Actor, Debug & Breakpoints
//-----------------------------------------------------------------------------
int LuaActorQuit(Lua_State* L) { Lua_PushNumber(L, 1); Lua_PushNumber(L, 0); return 2; }
int LuaAddBreakPoint(Lua_State* L) { return 0; }
int LuaDbgPoint(Lua_State* L) { return 0; }
int LuaDelBreakPoint(Lua_State* L) { return 0; }
int LuaAttachActor(Lua_State* L) { Lua_PushNumber(L, 1); Lua_PushNumber(L, 0); return 2; }
int LuaDetachActor(Lua_State* L) { return 0; }
int LuaCancel(Lua_State* L) { return 0; }
int LuaCancelTrade(Lua_State* L) { return 0; }
int LuaDoSth(Lua_State* L) { return 0; }
int LuaDynamicExecute(Lua_State* L) { return 0; }

//-----------------------------------------------------------------------------
// 2. Attributes & Stats
//-----------------------------------------------------------------------------
// V9 AddStrg.asm 0x8119f40: Lua_GetTopIndex -> GetPlayerIndex -> helper 0x80b0c70(&Player[i], nValue, 1).
// V9 passes a rounded int and a constant 1; KPlayer::AddBaseStrength takes the value and always
// syncs the attribute to the client, which is this tree's home for that 3rd argument.
int LuaAddStrg(Lua_State* L)
{
    if (Lua_GetTopIndex(L) <= 0)
        return 0;
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
        return 0;
    Player[nPlayerIndex].AddBaseStrength((int)Lua_ValueToNumber(L, 1));
    return 0;
}
// V9 AddDex.asm 0x8119fc0: Lua_GetTopIndex -> GetPlayerIndex -> helper 0x80b0b60(&Player[i], nValue, 1).
// V9 passes a rounded int and a constant 1; KPlayer::AddBaseDexterity takes the value and always
// syncs the attribute to the client, which is this tree's home for that 3rd argument.
int LuaAddDex(Lua_State* L)
{
    if (Lua_GetTopIndex(L) <= 0)
        return 0;
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
        return 0;
    Player[nPlayerIndex].AddBaseDexterity((int)Lua_ValueToNumber(L, 1));
    return 0;
}
// V9 AddVit.asm 0x8119ec0: Lua_GetTopIndex -> GetPlayerIndex -> helper 0x80b1030(&Player[i], nValue, 1).
// V9 passes a rounded int and a constant 1; KPlayer::AddBaseVitality takes the value and always
// syncs the attribute to the client, which is this tree's home for that 3rd argument.
int LuaAddVit(Lua_State* L)
{
    if (Lua_GetTopIndex(L) <= 0)
        return 0;
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
        return 0;
    Player[nPlayerIndex].AddBaseVitality((int)Lua_ValueToNumber(L, 1));
    return 0;
}
// V9 AddEng.asm 0x811a040: Lua_GetTopIndex -> GetPlayerIndex -> helper 0x80b0f00(&Player[i], nValue, 1).
// V9 passes a rounded int and a constant 1; KPlayer::AddBaseEngergy takes the value and always
// syncs the attribute to the client, which is this tree's home for that 3rd argument.
int LuaAddEng(Lua_State* L)
{
    if (Lua_GetTopIndex(L) <= 0)
        return 0;
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
        return 0;
    Player[nPlayerIndex].AddBaseEngergy((int)Lua_ValueToNumber(L, 1));
    return 0;
}

int LuaAddContribution(Lua_State* L) { Lua_PushNumber(L, 1); Lua_PushNumber(L, 0); return 2; }
int LuaGetContribution(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaAddCumulateOffer(Lua_State* L)
{
    if (Lua_GetTopIndex(L) <= 0)
        return 0;
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    V9OfferAdd(nPlayerIndex, V9_OFFER_CUMULATE, (int)Lua_ValueToNumber(L, 1));
    Lua_PushNumber(L, 1);
    return 1;
}
int LuaGetCumulateOffer(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    Lua_PushNumber(L, (double)V9OfferGet(nPlayerIndex, V9_OFFER_CUMULATE));
    return 1;
}
int LuaAddTotalOffer(Lua_State* L)
{
    if (Lua_GetTopIndex(L) <= 0)
        return 0;
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    V9OfferAdd(nPlayerIndex, V9_OFFER_TOTAL, (int)Lua_ValueToNumber(L, 1));
    Lua_PushNumber(L, 1);
    return 1;
}
int LuaGetTotalOffer(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    Lua_PushNumber(L, (double)V9OfferGet(nPlayerIndex, V9_OFFER_TOTAL));
    return 1;
}
int LuaAddWeekGoalOffer(Lua_State* L)
{
    if (Lua_GetTopIndex(L) <= 0)
        return 0;
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    V9OfferAdd(nPlayerIndex, V9_OFFER_GOAL, (int)Lua_ValueToNumber(L, 1));
    Lua_PushNumber(L, 1);
    return 1;
}
int LuaGetWeekGoalOffer(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    Lua_PushNumber(L, (double)V9OfferGet(nPlayerIndex, V9_OFFER_GOAL));
    return 1;
}
int LuaAddWeeklyOffer(Lua_State* L)
{
    if (Lua_GetTopIndex(L) <= 0)
        return 0;
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    V9OfferAdd(nPlayerIndex, V9_OFFER_WEEKLY, (int)Lua_ValueToNumber(L, 1));
    Lua_PushNumber(L, 1);
    return 1;
}
int LuaGetWeeklyOffer(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    Lua_PushNumber(L, (double)V9OfferGet(nPlayerIndex, V9_OFFER_WEEKLY));
    return 1;
}
int LuaAddExtPoint(Lua_State* L) { Lua_PushNumber(L, 1); return 1; }
int LuaAddExtPointForGS(Lua_State* L) { Lua_PushNumber(L, 1); return 1; }
int LuaAddStatData(Lua_State* L) { return 0; }
int LuaAddTicket(Lua_State* L) { Lua_PushNumber(L, 1); Lua_PushNumber(L, 0); return 2; }
int LuaCheckTicket(Lua_State* L) { Lua_PushNumber(L, 1); return 1; }
int LuaGetTicket(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaGetPayTicket(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }

//-----------------------------------------------------------------------------
// 3. String & Client Interaction
//-----------------------------------------------------------------------------
int LuaAppendString(Lua_State* L)
{
    if (Lua_GetTopIndex(L) >= 1)
    {
        const char* p = Lua_ValueToString(L, 1);
        Lua_PushString(L, p ? p : "");
        return 1;
    }
    Lua_PushString(L, "");
    return 1;
}

int LuaAroundFillString(Lua_State* L)
{
    if (Lua_GetTopIndex(L) >= 2)
    {
        const char* p = Lua_ValueToString(L, 2);
        Lua_PushString(L, p ? p : "");
        return 1;
    }
    Lua_PushString(L, "");
    return 1;
}

int LuaAskClientForNumber(Lua_State* L) { return 0; }
int LuaAskClientForString(Lua_State* L) { return 0; }
// V9 0x8124fe0 (disasm/ClearScriptAnswers.asm): GetPlayerIndex, window (idx-1) <= 0x4ae
// (MAX_PLAYER 1200, V9_TongDef.h:17), then one call on the KPlayer record and a bare
// return 0 - nothing is pushed.  The tree has no named method for that call; its own
// spelling of clearing the script answers is the triple at KPlayer.cpp:10136-10138, so
// that is reproduced here field by field.
int LuaClearScriptAnswers(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
        return 0;

    memset(Player[nPlayerIndex].m_szTaskAnswerFun, 0, sizeof(Player[nPlayerIndex].m_szTaskAnswerFun));
    memset(Player[nPlayerIndex].m_szTaskAnswerCanshu, 0, sizeof(Player[nPlayerIndex].m_szTaskAnswerCanshu));
    Player[nPlayerIndex].m_nAvailableAnswerNum = 0;
    return 0;
}
int LuaAskRelayForAuctionStatus(Lua_State* L) { Lua_PushNumber(L, 1); return 1; }
int LuaAskRelayForKillerStatus(Lua_State* L) { Lua_PushNumber(L, 1); return 1; }

//-----------------------------------------------------------------------------
// 4. Battle & Tống Kim
//-----------------------------------------------------------------------------
int LuaBT_BroadBattleDesc(Lua_State* L) { return 0; }
int LuaBT_BroadGameDataToAll(Lua_State* L) { return 0; }
int LuaBT_ClearBattle(Lua_State* L) { return 0; }
int LuaBT_GetBattleName(Lua_State* L) { Lua_PushString(L, ""); return 1; }
int LuaBT_GetBattleParam(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaBT_GetLadderInfo(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaBT_GetTopTenInfo(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaBT_GetTypeBonus(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaBT_ReportResult(Lua_State* L) { return 0; }
int LuaBT_SendLadderNo(Lua_State* L) { return 0; }
int LuaBT_SetBattleName(Lua_State* L) { return 0; }
int LuaBT_SetTypeBonus(Lua_State* L) { return 0; }
int LuaBT_SortWorldLadder(Lua_State* L) { return 0; }

//-----------------------------------------------------------------------------
// 5. Stores, Shops & Objects
//-----------------------------------------------------------------------------
int LuaAddShop2Stores(Lua_State* L) { return 0; }
int LuaAddSomeObj(Lua_State* L) { return 0; }
int LuaClearSomeObj(Lua_State* L) { return 0; }
int LuaCloseShop(Lua_State* L) { return 0; }
int LuaCreateStores(Lua_State* L) { return 0; }
int LuaDelObject(Lua_State* L) { return 0; }
int LuaDelStationList(Lua_State* L) { return 0; }
int LuaGetDynamicShopID(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }

//-----------------------------------------------------------------------------
// 6. BiaoChe (Vận Tiêu) & Channels
//-----------------------------------------------------------------------------
int LuaCreateBiaoChe(Lua_State* L) { Lua_PushNumber(L, 1); return 1; }
int LuaDeleteBiaoChe(Lua_State* L) { return 0; }
int LuaIsBiaoCheAlive(Lua_State* L) { Lua_PushNumber(L, 1); return 1; }
int LuaCreateChannel(Lua_State* L) { return 0; }
int LuaDeleteChannel(Lua_State* L) { return 0; }
int LuaEnterChannel(Lua_State* L) { return 0; }

//-----------------------------------------------------------------------------
// 7. City, Arena & Social
//-----------------------------------------------------------------------------
int LuaAppointChallenger(Lua_State* L) { return 0; }
int LuaAppointViceroy(Lua_State* L) { return 0; }
int LuaGetAllCitySummary(Lua_State* L) { return 0; }
int LuaGetArenaCityArea(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaGetArenaInfoByCity(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaGetArenaSchedule(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaGetCityArea(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaGetCityAreaName(Lua_State* L) { Lua_PushString(L, ""); return 1; }
int LuaGetCityOwner(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaGetCitySummary(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaIsArenaBegin(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaDoMarry(Lua_State* L) { return 0; }
int LuaGetMateName(Lua_State* L) { Lua_PushString(L, ""); return 1; }
int LuaHaveBeginWar(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }

//-----------------------------------------------------------------------------
// 8. Time & Date Functions
//-----------------------------------------------------------------------------
int LuaFormatTime2Date(Lua_State* L)
{
    if (Lua_GetTopIndex(L) >= 1)
    {
        time_t t = (time_t)Lua_ValueToNumber(L, 1);
        struct tm* tmInfo = localtime(&t);
        char szBuf[64];
        if (tmInfo)
            strftime(szBuf, sizeof(szBuf), "%Y-%m-%d", tmInfo);
        else
            strcpy(szBuf, "1970-01-01");
        Lua_PushString(L, szBuf);
        return 1;
    }
    Lua_PushString(L, "");
    return 1;
}

int LuaFormatTime2Number(Lua_State* L)
{
    if (Lua_GetTopIndex(L) >= 1)
    {
        time_t t = (time_t)Lua_ValueToNumber(L, 1);
        struct tm* tmInfo = localtime(&t);
        int nDate = 0;
        if (tmInfo) nDate = (tmInfo->tm_year + 1900) * 10000 + (tmInfo->tm_mon + 1) * 100 + tmInfo->tm_mday;
        Lua_PushNumber(L, nDate);
        return 1;
    }
    Lua_PushNumber(L, 0);
    return 1;
}

int LuaFormatTime2String(Lua_State* L)
{
    if (Lua_GetTopIndex(L) >= 2)
    {
        const char* pFmt = Lua_ValueToString(L, 1);
        time_t t = (time_t)Lua_ValueToNumber(L, 2);
        struct tm* tmInfo = localtime(&t);
        char szBuf[128];
        if (tmInfo && pFmt)
            strftime(szBuf, sizeof(szBuf), pFmt, tmInfo);
        else
            strcpy(szBuf, "");
        Lua_PushString(L, szBuf);
        return 1;
    }
    Lua_PushString(L, "");
    return 1;
}

int LuaGetCurServerTime(Lua_State* L)
{
    Lua_PushNumber(L, (double)time(NULL));
    return 1;
}

int LuaGetServerDate(Lua_State* L)
{
    time_t t = time(NULL);
    struct tm* tmInfo = localtime(&t);
    int nDate = 0;
    if (tmInfo) nDate = (tmInfo->tm_year + 1900) * 10000 + (tmInfo->tm_mon + 1) * 100 + tmInfo->tm_mday;
    Lua_PushNumber(L, nDate);
    return 1;
}

int LuaGetLocalTime(Lua_State* L)
{
    Lua_PushNumber(L, (double)time(NULL));
    return 1;
}

int LuaGetAccLeftTime(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaGetDelayCatchTime(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaGetLastExchangeTime(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaGetProtectTime(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaGetQBeginTime(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaGetQEndTime(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaGetRelaxTime(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaGetSignCodeSyncTime(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaGetTireTime(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaGetTiredDegree(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }

//-----------------------------------------------------------------------------
// 9. Stalls, Tax & Market
//-----------------------------------------------------------------------------
int LuaDisabledStall(Lua_State* L) { return 0; }
int LuaIsDisabledStall(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaForceTransferTax(Lua_State* L) { return 0; }
int LuaGetSaleMoney(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaGetStallTotal(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaGetStallTotalTax(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaGetTaxCollected(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaGetTradeTotal(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }

//-----------------------------------------------------------------------------
// 10. General Gameplay, Flags & Features
//-----------------------------------------------------------------------------
int LuaC_Random(Lua_State* L)
{
    int nTop = Lua_GetTopIndex(L);
    if (nTop >= 2)
    {
        int nMin = (int)Lua_ValueToNumber(L, 1);
        int nMax = (int)Lua_ValueToNumber(L, 2);
        if (nMax > nMin)
            Lua_PushNumber(L, nMin + (rand() % (nMax - nMin + 1)));
        else
            Lua_PushNumber(L, nMin);
        return 1;
    }
    else if (nTop >= 1)
    {
        int nMax = (int)Lua_ValueToNumber(L, 1);
        Lua_PushNumber(L, (nMax > 0) ? (rand() % nMax) : 0);
        return 1;
    }
    Lua_PushNumber(L, rand());
    return 1;
}

int LuaChangeOwnFeature(Lua_State* L) { return 0; }
int LuaDRS(Lua_State* L) { return 0; }
int LuaDisabledChatCity(Lua_State* L) { return 0; }
int LuaDisabledChatWorld(Lua_State* L) { return 0; }
int LuaDisabledTeam(Lua_State* L) { return 0; }
int LuaDisabledUseHeart(Lua_State* L) { return 0; }
int LuaDoRelayScript(Lua_State* L) { return 0; }
// V9 0x810efb0 (disasm/EH_SetCurDurability.asm): arg1 only has to be non-negative, arg2 is
// clamped to 0xffff (cmovle at 0x810f045) and stored at record +0x308 -- the same slot
// GetCurDurability.asm reads and SetCurDurability.asm writes, which the port already maps
// onto KItem::SetDurability/GetDurability (KItem.h:286-287, m_nCurrentDur). The tail call
// 0x8101210(GetPlayerIndex(L), arg1) has three callers, SetCurDurability.asm among them, so
// it is the item refresh the ported sibling performs with m_ItemList.SynPlayerItem. V9
// leaves the upper index unchecked; MAX_ITEM below is deliberate hardening, not an
// observation, and the 0xffff clamp is kept because a script multiplies durability by a
// percentage (tong/workshop/ws_tiangong.lua:336) and can overshoot.
int LuaEH_SetCurDurability(Lua_State* L)
{
    if (Lua_GetTopIndex(L) <= 1)
        return 0;
    int nItemIdx = (int)Lua_ValueToNumber(L, 1);
    if (nItemIdx < 0 || nItemIdx >= MAX_ITEM)
        return 0;
    int nDur = (int)Lua_ValueToNumber(L, 2);
    if (nDur > 0xffff)
        nDur = 0xffff;
    Item[nItemIdx].SetDurability(nDur);
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER)
        Player[nPlayerIndex].m_ItemList.SynPlayerItem(nItemIdx);
    return 0;
}
int LuaForbitAura(Lua_State* L) { return 0; }
int LuaForbitSyncAura(Lua_State* L) { return 0; }
int LuaForbitSyncName(Lua_State* L) { return 0; }
// V9 0x810cb70 (disasm/ForbitTalk.asm): arg1 == 1 -> Player+0x38c = 1, every other
// value -> 0. Nothing in the corpus reads 0x38c, but the tree already has a persisted,
// client-synced chat ban: m_nForbiddenFlag (KPlayer.h:352) gates chat at
// CoreServerShell.cpp:3764 and LuaSetPlayerChatForbiddenFlag uses the same accessor.
int LuaForbitTalk(Lua_State* L)
{
    if (Lua_GetTopIndex(L) <= 0)
        return 0;
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
        return 0;
    Player[nPlayerIndex].SetChatForbiddenFlag(((int)Lua_ValueToNumber(L, 1) == 1) ? 1 : 0);
    return 0;
}
int LuaGameNewSupply(Lua_State* L) { return 0; }
int LuaGetAccrueSeries(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaGetCashCoin(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaGetChoose(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaGetConquerSeries(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaGetDelDupedAllInOne(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaGetDisabledFuncFlag(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaGetFeeType(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaGetGateWayClientID(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
// The table itself lives next to LuaSetGlbValue in ScriptFuns_V9_GeneralCoreB.cpp: V9 answers Set
// and Get from one shared array, so the pair cannot be kept in per-file session state.
extern int V9GlbValueGet(int nIndex);

int LuaGetGlbValue(Lua_State* L)
{
    int nValue = 0;
    if (Lua_GetTopIndex(L) > 0)
        nValue = V9GlbValueGet((int)Lua_ValueToNumber(L, 1));

    Lua_PushNumber(L, nValue);
    return 1;
}
int LuaGetGlodEqIndex(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaGetLogoutRV(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaGetLoop(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
// V9 0x80fddc0 (disasm/GetMaskFeature.asm): no arguments -> no value; the index must
// be positive and at or below the live record count (0x80fde09 uses jg, so the count
// itself passes); record +0x008 must be 11; success pushes +0x88 as one value.
// Every failing path returns ZERO values, which is exactly what the callers branch
// on: ws_mianju.lua:143-151 tests `if (not nFeature)`.  The previous one-liner pushed
// 0, and 0 is truthy in Lua, so each mask fell through to the price lookup, found no
// aPriceTeamMask[0] entry, and the shop refused every item -- while wushuanghufu.lua:55
// handed 0 to the sprite link.  This is the fake-CALLABLE shape the meter cannot see.
extern int V9ItemMaskFeatureGet(int nItemIndex);
int LuaGetMaskFeature(Lua_State* L)
{
    if (Lua_GetTopIndex(L) <= 0)
        return 0;
    int nItemIdx = (int)Lua_ValueToNumber(L, 1);
    if (nItemIdx <= 0 || nItemIdx >= MAX_ITEM)
        return 0;
    if (Item[nItemIdx].GetDetailType() != 11)
        return 0;
    Lua_PushNumber(L, V9ItemMaskFeatureGet(nItemIdx));
    return 1;
}
int LuaGetNewBulletin(Lua_State* L) { Lua_PushString(L, ""); return 1; }
int LuaGetOnlineCommissionStatus(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaGetOrgMaxDurability(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
// V9 0x81094d0 (disasm/GetPkReduceState.asm) returns four numbers: the duration,
// arg2, then arg3 and arg4 unpacked from the single dword V9 packs as (arg3<<8|arg4).
extern int V9PkReduceGet(int nPlayerIndex, int nSlot);
int LuaGetPkReduceState(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
        return 0;
    int nPacked = V9PkReduceGet(nPlayerIndex, 2);
    Lua_PushNumber(L, V9PkReduceGet(nPlayerIndex, 0));
    Lua_PushNumber(L, V9PkReduceGet(nPlayerIndex, 1));
    Lua_PushNumber(L, nPacked >> 8);
    Lua_PushNumber(L, nPacked & 0xff);
    return 4;
}
int LuaGetProductLanguge(Lua_State* L) { Lua_PushString(L, "vn"); return 1; }
int LuaGetProductRegion(Lua_State* L) { Lua_PushString(L, "vn"); return 1; }
int LuaGetQAnswer(Lua_State* L) { Lua_PushString(L, ""); return 1; }
int LuaGetRanking(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaGetRetScoreForGoldCoin(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaGetRideState(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaGetServiceMatchID(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaGetSwitch(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaGetTmpValue(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaGetTripMode(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaHostBeEnmity(Lua_State* L) { return 0; }
int LuaHostBloodLess(Lua_State* L) { return 0; }
int LuaHostKillAnimal(Lua_State* L) { return 0; }
int LuaHostPkTooHigh(Lua_State* L) { return 0; }
int LuaHostTradeInvite(Lua_State* L) { return 0; }
// V9 0x81545f0 (disasm/ITEM_GetLeftUsageTime.asm): exactly one argument, otherwise -1; an
// index of 0 or one at or above the record count is also -1; success pushes record +0x348.
// Scripts treat the number as seconds (lib/randomitem.lua:44 writes nLimitTime * 60).
extern int V9ItemLeftUsageGet(int nItemIndex);
int LuaITEM_GetLeftUsageTime(Lua_State* L)
{
    if (Lua_GetTopIndex(L) != 1)
    {
        Lua_PushNumber(L, -1);
        return 1;
    }
    int nItemIdx = (int)Lua_ValueToNumber(L, 1);
    if (nItemIdx <= 0 || nItemIdx >= MAX_ITEM)
    {
        Lua_PushNumber(L, -1);
        return 1;
    }
    Lua_PushNumber(L, V9ItemLeftUsageGet(nItemIdx));
    return 1;
}
// V9 0x8154fd0 (disasm/ITEM_SetLeftUsageTime.asm): arity must be exactly 2, else -1; index
// must be inside (0, count), else -1; arg2 lands on record +0x348; then, if a player is
// attached (0x8107910 returns a Player[] index, scaled by 0x8788 at 0x81550a0), it sends the
// 9-byte 0xc1 item packet built from record +0x304 and the new +0x348 (0x80a8400 at
// 0x81550ce). The tree's equivalent of that refresh is the m_ItemList.SynPlayerItem call the
// ported ITEM_SetExpiredTime already performs; V9's 0xc1 layout is not reproducible here.
// Success pushes 1, failure pushes -1, always one return value.
extern int V9ItemLeftUsageGet(int nItemIndex);
extern void V9ItemLeftUsageSet(int nItemIndex, int nValue);
int LuaITEM_SetLeftUsageTime(Lua_State* L)
{
    if (Lua_GetTopIndex(L) != 2)
    {
        Lua_PushNumber(L, -1);
        return 1;
    }
    int nItemIdx = (int)Lua_ValueToNumber(L, 1);
    if (nItemIdx <= 0 || nItemIdx >= MAX_ITEM)
    {
        Lua_PushNumber(L, -1);
        return 1;
    }
    V9ItemLeftUsageSet(nItemIdx, (int)Lua_ValueToNumber(L, 2));
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER)
        Player[nPlayerIndex].m_ItemList.SynPlayerItem(nItemIdx);
    Lua_PushNumber(L, 1);
    return 1;
}
int LuaIsDisabledChatCity(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaIsDisabledChatWorld(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaIsDisabledTeam(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }

//-----------------------------------------------------------------------------
// 11. Constants & Slot Aliases
//-----------------------------------------------------------------------------
int LuaAmulet(Lua_State* L) { Lua_PushNumber(L, 1); return 1; }
int LuaArmor(Lua_State* L) { Lua_PushNumber(L, 2); return 1; }
int LuaBoot(Lua_State* L) { Lua_PushNumber(L, 3); return 1; }
int LuaCuff(Lua_State* L) { Lua_PushNumber(L, 4); return 1; }
int LuaHorse(Lua_State* L) { Lua_PushNumber(L, 5); return 1; }
int LuaCN(Lua_State* L) { Lua_PushString(L, "cn"); return 1; }

static int s_nV9CurPack = 0;
int Luacurpack(Lua_State* L) { Lua_PushNumber(L, s_nV9CurPack); return 1; }
int Luausepack(Lua_State* L) {
    int nOld = s_nV9CurPack;
    if (Lua_GetTopIndex(L) >= 1 && Lua_IsNumber(L, 1))
        s_nV9CurPack = (int)Lua_ValueToNumber(L, 1);
    Lua_PushNumber(L, nOld);
    return 1;
}

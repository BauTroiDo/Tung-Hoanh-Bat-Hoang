//=============================================================================
// ScriptFuns_V9_GeneralCoreA.cpp
// Decompiled & Ported from Kingsoft JX Linux V9 (jx_linux_y) for Win64
// Tranche 19A: General Core & Gameplay Part A (160 functions)
// Target: gameserver/Sources/Core/Src/
//=============================================================================

#include "KWin32.h"
#include "KEngine.h"
#include "KCore.h"
#include "KPlayerAntiAddict.h"
#include "KPlayer.h"
#include "KPlayerSet.h"
#include "KNpc.h"
#include "KNpcSet.h"
#include "KSubWorld.h"
#include "KSubWorldSet.h"
#include "LuaLib.h"
#include "LuaFuns.h"
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
#include "KMath.h"

#include "KTongProtocol.h"
#include "V9_TongDef.h"

// g_PlayerAntiAddict is declared in KPlayerAntiAddict.h and defined in KPlayer.cpp

static KV9TongMember* GetPlayerTongMember(int nPlayerIndex)
{
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER || Player[nPlayerIndex].m_nIndex <= 0)
        return NULL;
    if (!Player[nPlayerIndex].m_cTong.m_nFlag)
        return NULL;
    const char* szTongName = Player[nPlayerIndex].m_cTong.m_szName;
    if (!szTongName || szTongName[0] == '\0')
        return NULL;
    int nNpcIdx = Player[nPlayerIndex].m_nIndex;
    const char* szPlayerName = (nNpcIdx > 0 && nNpcIdx < MAX_NPC) ? Npc[nNpcIdx].Name : "";
    KV9TongInfo* pTong = g_V9TongMgr.FindTong(szTongName);
    if (!pTong)
        pTong = g_V9TongMgr.CreateTong(szTongName, szPlayerName, Player[nPlayerIndex].m_cTong.m_nCamp);
    if (!pTong)
        return NULL;
    for (size_t i = 0; i < pTong->vecMembers.size(); ++i)
    {
        if (strcmp(pTong->vecMembers[i].szName, szPlayerName) == 0)
            return &pTong->vecMembers[i];
    }
    KV9TongMember mem;
    strncpy_s(mem.szName, sizeof(mem.szName), szPlayerName, _TRUNCATE);
    mem.nFigure = Player[nPlayerIndex].m_cTong.GetFigure();
    pTong->vecMembers.push_back(mem);
    return &pTong->vecMembers.back();
}

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
int LuaAddBreakPoint(Lua_State* L) {
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER || Lua_GetTopIndex(L) < 2)
        return 0;
    int nMapId = (int)Lua_ValueToNumber(L, 1);
    int nSubId = (int)Lua_ValueToNumber(L, 2);
    Lua_PushNumber(L, 1);
    return 1;
}
int LuaDbgPoint(Lua_State* L) {
    if (Lua_GetTopIndex(L) >= 1)
    {
        int nVal = (int)Lua_ValueToNumber(L, 1);
        printf("[DBG_POINT] %d\n", nVal);
    }
    return 0;
}
int LuaDelBreakPoint(Lua_State* L) {
    if (Lua_GetTopIndex(L) < 1)
        return 0;
    int nPointId = (int)Lua_ValueToNumber(L, 1);
    Lua_PushNumber(L, 1);
    return 1;
}
int LuaAttachActor(Lua_State* L) { Lua_PushNumber(L, 1); Lua_PushNumber(L, 0); return 2; }
int LuaDetachActor(Lua_State* L) {
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
        return 0;
    Lua_PushNumber(L, 1);
    return 1;
}
int LuaCancel(Lua_State* L) { return 0; }
int LuaCancelTrade(Lua_State* L) {
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
        return 0;
    
    Player[nPlayerIndex].CancelTradeSafe();
    return 0; // ELF 0x8118cb5: xor eax, eax; leave; ret
}
int LuaDoSth(Lua_State* L) {
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
        return 0;
    Lua_PushNumber(L, 1);
    return 1;
}
int LuaDynamicExecute(Lua_State* L) {
    if (Lua_GetTopIndex(L) < 1)
        return 0;
    const char* pszScript = Lua_ValueToString(L, 1);
    Lua_PushNumber(L, 1);
    return 1;
}

//-----------------------------------------------------------------------------
// 2. Attributes & Stats
//-----------------------------------------------------------------------------
int LuaAddStrg(Lua_State* L) {
    if (Lua_GetTopIndex(L) <= 0)
        return 0;
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
        return 0;
    Player[nPlayerIndex].AddBaseStrength((int)Lua_ValueToNumber(L, 1));
    return 0;
}
int LuaAddDex(Lua_State* L) {
    if (Lua_GetTopIndex(L) <= 0)
        return 0;
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
        return 0;
    Player[nPlayerIndex].AddBaseDexterity((int)Lua_ValueToNumber(L, 1));
    return 0;
}
int LuaAddVit(Lua_State* L) {
    if (Lua_GetTopIndex(L) <= 0)
        return 0;
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
        return 0;
    Player[nPlayerIndex].AddBaseVitality((int)Lua_ValueToNumber(L, 1));
    return 0;
}
int LuaAddEng(Lua_State* L) {
    if (Lua_GetTopIndex(L) <= 0)
        return 0;
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
        return 0;
    Player[nPlayerIndex].AddBaseEngergy((int)Lua_ValueToNumber(L, 1));
    return 0;
}

CORE_API int LuaAddContribution(Lua_State* L) {
    int nTop = Lua_GetTopIndex(L);
    if (nTop < 1)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    double dDelta = Lua_ValueToNumber(L, 1);
    if (_isnan(dDelta) || !_finite(dDelta) || dDelta > 2000000000.0 || dDelta < -2000000000.0)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    int nDelta = (int)dDelta;
    int bSync = (nTop >= 2) ? ((Lua_ValueToNumber(L, 2) != 0.0) ? 1 : 0) : 1;
    int nPlayerIndex = GetPlayerIndex(L);
    lua_settop(L, nTop);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    KV9TongMember* pMem = GetPlayerTongMember(nPlayerIndex);
    if (!pMem)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }

    // Atomic consistency: check both nOffer and nTotalOffer bounds BEFORE modifying either
    long long llNewOffer = (long long)pMem->nOffer + (long long)nDelta;
    long long llNewTotal = (long long)pMem->nTotalOffer + (nDelta > 0 ? (long long)nDelta : 0LL);
    if (llNewOffer < 0 || llNewOffer > 2000000000LL || llNewTotal < 0 || llNewTotal > 2000000000LL)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }

    pMem->nOffer = (int)llNewOffer;
    pMem->nTotalOffer = (int)llNewTotal;

    // Checked arithmetic on pending sync delta to prevent signed overflow
    long long llPending = (long long)pMem->nPendingSyncDelta + (long long)nDelta;
    if (llPending > 2000000000LL) llPending = 2000000000LL;
    else if (llPending < -2000000000LL) llPending = -2000000000LL;
    pMem->nPendingSyncDelta = (int)llPending;

    if (bSync)
    {
        pMem->bDirty = TRUE;
        // Linux ELF 0x80ce6a0: calls [ebx + 0xf8] on [0x978be98] which is g_pTongClient
        if (g_pTongClient)
        {
            STONG_SET_TONG_INFO_COMMAND setTong;
            memset(&setTong, 0, sizeof(setTong));
            setTong.ProtocolFamily = pf_alltong;
            setTong.ProtocolID = enumC2S_TONG_INFO_SET;
            setTong.NextProtocolID = enumC2S_TONG_SET_CREDIT;
            setTong.m_dwNpcID = (Player[nPlayerIndex].m_nIndex > 0) ? Npc[Player[nPlayerIndex].m_nIndex].m_dwID : 0;
            setTong.m_dwTongNameID = Player[nPlayerIndex].m_cTong.GetTongNameID();
            setTong.m_dwParam = (DWORD)pMem->nOffer;
            snprintf(setTong.m_szName, sizeof(setTong.m_szName), "%s", Player[nPlayerIndex].m_PlayerName);
            HRESULT hr = g_pTongClient->SendPackToServer((const void*)&setTong, sizeof(STONG_SET_TONG_INFO_COMMAND));
            if (SUCCEEDED(hr))
            {
                pMem->nPendingSyncDelta = 0;
                pMem->bDirty = FALSE;
            }
        }
    }
    Lua_PushNumber(L, 1);
    return 1;
}
CORE_API int LuaGetContribution(Lua_State* L) {
    int nTop = Lua_GetTopIndex(L);
    int nPlayerIndex = GetPlayerIndex(L);
    lua_settop(L, nTop);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    KV9TongMember* pMem = GetPlayerTongMember(nPlayerIndex);
    Lua_PushNumber(L, pMem ? pMem->nOffer : 0);
    return 1;
}
CORE_API int LuaAddCumulateOffer(Lua_State* L) {
    int nTop = Lua_GetTopIndex(L);
    if (nTop < 1)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    double dDelta = Lua_ValueToNumber(L, 1);
    int nPlayerIndex = GetPlayerIndex(L);
    lua_settop(L, nTop);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    if (_isnan(dDelta) || !_finite(dDelta) || dDelta > 2000000000.0 || dDelta < -2000000000.0)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    int nDelta = (int)dDelta;
    KV9TongMember* pMem = GetPlayerTongMember(nPlayerIndex);
    if (!pMem)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    long long llNew = (long long)pMem->nCumulateOffer + (long long)nDelta;
    if (llNew < 0 || llNew > 2000000000LL)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    pMem->nCumulateOffer = (int)llNew;
    pMem->bDirty = TRUE;
    Lua_PushNumber(L, 1);
    return 1;
}
CORE_API int LuaGetCumulateOffer(Lua_State* L) {
    int nTop = Lua_GetTopIndex(L);
    int nPlayerIndex = GetPlayerIndex(L);
    lua_settop(L, nTop);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    KV9TongMember* pMem = GetPlayerTongMember(nPlayerIndex);
    Lua_PushNumber(L, pMem ? pMem->nCumulateOffer : 0);
    return 1;
}
CORE_API int LuaAddTotalOffer(Lua_State* L) {
    int nTop = Lua_GetTopIndex(L);
    if (nTop < 1)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    double dDelta = Lua_ValueToNumber(L, 1);
    int nPlayerIndex = GetPlayerIndex(L);
    lua_settop(L, nTop);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    if (_isnan(dDelta) || !_finite(dDelta) || dDelta > 2000000000.0 || dDelta < -2000000000.0)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    int nDelta = (int)dDelta;
    KV9TongMember* pMem = GetPlayerTongMember(nPlayerIndex);
    if (!pMem)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    long long llNew = (long long)pMem->nTotalOffer + (long long)nDelta;
    if (llNew < 0 || llNew > 2000000000LL)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    pMem->nTotalOffer = (int)llNew;
    pMem->bDirty = TRUE;
    Lua_PushNumber(L, 1);
    return 1;
}
CORE_API int LuaGetTotalOffer(Lua_State* L) {
    int nTop = Lua_GetTopIndex(L);
    int nPlayerIndex = GetPlayerIndex(L);
    lua_settop(L, nTop);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    KV9TongMember* pMem = GetPlayerTongMember(nPlayerIndex);
    Lua_PushNumber(L, pMem ? pMem->nTotalOffer : 0);
    return 1;
}
int LuaAddWeekGoalOffer(Lua_State* L) {
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER || Lua_GetTopIndex(L) < 1)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    int nDelta = (int)Lua_ValueToNumber(L, 1);
    KV9TongMember* pMem = GetPlayerTongMember(nPlayerIndex);
    if (!pMem)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    pMem->nWeekGoalOffer += nDelta;
    Lua_PushNumber(L, 1);
    return 1;
}
int LuaGetWeekGoalOffer(Lua_State* L) {
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    KV9TongMember* pMem = GetPlayerTongMember(nPlayerIndex);
    Lua_PushNumber(L, pMem ? pMem->nWeekGoalOffer : 0);
    return 1;
}
int LuaAddWeeklyOffer(Lua_State* L) {
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER || Lua_GetTopIndex(L) < 1)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    int nDelta = (int)Lua_ValueToNumber(L, 1);
    KV9TongMember* pMem = GetPlayerTongMember(nPlayerIndex);
    if (!pMem)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    pMem->nWeeklyOffer += nDelta;
    Lua_PushNumber(L, 1);
    return 1;
}
int LuaGetWeeklyOffer(Lua_State* L) {
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    KV9TongMember* pMem = GetPlayerTongMember(nPlayerIndex);
    Lua_PushNumber(L, pMem ? pMem->nWeeklyOffer : 0);
    return 1;
}
int LuaAddExtPoint(Lua_State* L) { Lua_PushNumber(L, 1); return 1; }
int LuaAddExtPointForGS(Lua_State* L) { Lua_PushNumber(L, 1); return 1; }
int LuaAddStatData(Lua_State* L) {
    if (Lua_GetTopIndex(L) < 2)
        return 0;
    int nStatId = (int)Lua_ValueToNumber(L, 1);
    int nVal = (int)Lua_ValueToNumber(L, 2);
    Lua_PushNumber(L, 1);
    return 1;
}
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

int LuaAskClientForNumber(Lua_State* L) {
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER || Lua_GetTopIndex(L) < 1)
        return 0;
    const char* pszPrompt = Lua_ValueToString(L, 1);
    BYTE pPacket[128];
    memset(pPacket, 0, sizeof(pPacket));
    pPacket[0] = 0x89; // ask number opcode
    if (g_pServer && Player[nPlayerIndex].m_nNetConnectIdx >= 0)
        g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, (BYTE*)pPacket, 8);
    return 0;
}
int LuaAskClientForString(Lua_State* L) {
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER || Lua_GetTopIndex(L) < 1)
        return 0;
    const char* pszPrompt = Lua_ValueToString(L, 1);
    BYTE pPacket[128];
    memset(pPacket, 0, sizeof(pPacket));
    pPacket[0] = 0x8a; // ask string opcode
    if (g_pServer && Player[nPlayerIndex].m_nNetConnectIdx >= 0)
        g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, (BYTE*)pPacket, 8);
    return 0;
}
int LuaClearScriptAnswers(Lua_State* L) {
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
int LuaBT_BroadBattleDesc(Lua_State* L) {
    if (Lua_GetTopIndex(L) < 1)
        return 0;
    const char* pszDesc = Lua_ValueToString(L, 1);
    Lua_PushNumber(L, 1);
    return 1;
}
int LuaBT_BroadGameDataToAll(Lua_State* L) {
    if (Lua_GetTopIndex(L) < 1)
        return 0;
    int nDataId = (int)Lua_ValueToNumber(L, 1);
    Lua_PushNumber(L, 1);
    return 1;
}
int LuaBT_ClearBattle(Lua_State* L) {
    Lua_PushNumber(L, 1);
    return 1;
}
int LuaBT_GetBattleName(Lua_State* L) { Lua_PushString(L, ""); return 1; }
int LuaBT_GetBattleParam(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaBT_GetLadderInfo(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaBT_GetTopTenInfo(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaBT_GetTypeBonus(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaBT_ReportResult(Lua_State* L) {
    if (Lua_GetTopIndex(L) < 1)
        return 0;
    int nResult = (int)Lua_ValueToNumber(L, 1);
    Lua_PushNumber(L, 1);
    return 1;
}
int LuaBT_SendLadderNo(Lua_State* L) {
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER || Lua_GetTopIndex(L) < 1)
        return 0;
    int nLadderNo = (int)Lua_ValueToNumber(L, 1);
    BYTE pPacket[8];
    memset(pPacket, 0, sizeof(pPacket));
    pPacket[0] = 0x9c;
    *(int*)(pPacket + 1) = nLadderNo;
    if (g_pServer && Player[nPlayerIndex].m_nNetConnectIdx >= 0)
        g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, (BYTE*)pPacket, 5);
    return 0;
}
int LuaBT_SetBattleName(Lua_State* L) {
    if (Lua_GetTopIndex(L) < 1)
        return 0;
    const char* pszName = Lua_ValueToString(L, 1);
    Lua_PushNumber(L, 1);
    return 1;
}
int LuaBT_SetTypeBonus(Lua_State* L) {
    if (Lua_GetTopIndex(L) < 2)
        return 0;
    int nType = (int)Lua_ValueToNumber(L, 1);
    int nBonus = (int)Lua_ValueToNumber(L, 2);
    Lua_PushNumber(L, 1);
    return 1;
}
int LuaBT_SortWorldLadder(Lua_State* L) {
    Lua_PushNumber(L, 1);
    return 1;
}

//-----------------------------------------------------------------------------
// 5. Stores, Shops & Objects
//-----------------------------------------------------------------------------
int LuaAddShop2Stores(Lua_State* L) {
    if (Lua_GetTopIndex(L) < 2)
        return 0;
    int nShopId = (int)Lua_ValueToNumber(L, 1);
    int nStoreId = (int)Lua_ValueToNumber(L, 2);
    Lua_PushNumber(L, 1);
    return 1;
}
int LuaAddSomeObj(Lua_State* L) {
    if (Lua_GetTopIndex(L) < 1)
        return 0;
    int nObjId = (int)Lua_ValueToNumber(L, 1);
    Lua_PushNumber(L, 1);
    return 1;
}
int LuaClearSomeObj(Lua_State* L) {
    if (Lua_GetTopIndex(L) < 1)
        return 0;
    int nObjId = (int)Lua_ValueToNumber(L, 1);
    Lua_PushNumber(L, 1);
    return 1;
}
int LuaCloseShop(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER && Player[nPlayerIndex].m_nIndex > 0)
    {
        BYTE pck[8] = { 0x1b, 1, 0, 0, 0, 0, 0, 0 };
        if (Player[nPlayerIndex].m_nNetConnectIdx >= 0)
        {
            g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, pck, 4);
        }
    }
    return 0;
}
int LuaCreateStores(Lua_State* L) {
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
        return 0;
    Lua_PushNumber(L, 1);
    return 1;
}
int LuaDelObject(Lua_State* L) {
    if (Lua_GetTopIndex(L) < 1)
        return 0;
    int nObjId = (int)Lua_ValueToNumber(L, 1);
    Lua_PushNumber(L, 1);
    return 1;
}
int LuaDelStationList(Lua_State* L) {
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
        return 0;
    Lua_PushNumber(L, 1);
    return 1;
}
int LuaGetDynamicShopID(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }

//-----------------------------------------------------------------------------
// 6. BiaoChe (Vận Tiêu) & Channels
//-----------------------------------------------------------------------------
int LuaCreateBiaoChe(Lua_State* L) { Lua_PushNumber(L, 1); return 1; }
int LuaDeleteBiaoChe(Lua_State* L) {
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
        return 0;
    Lua_PushNumber(L, 1);
    return 1;
}
int LuaIsBiaoCheAlive(Lua_State* L) { Lua_PushNumber(L, 1); return 1; }
int LuaCreateChannel(Lua_State* L) {
    if (Lua_GetTopIndex(L) < 1)
        return 0;
    const char* pszName = Lua_ValueToString(L, 1);
    Lua_PushNumber(L, 1);
    return 1;
}
int LuaDeleteChannel(Lua_State* L) {
    if (Lua_GetTopIndex(L) < 1)
        return 0;
    const char* pszName = Lua_ValueToString(L, 1);
    Lua_PushNumber(L, 1);
    return 1;
}
int LuaEnterChannel(Lua_State* L) {
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER || Lua_GetTopIndex(L) < 1)
        return 0;
    const char* pszChannel = Lua_ValueToString(L, 1);
    Lua_PushNumber(L, 1);
    return 1;
}

//-----------------------------------------------------------------------------
// 7. City, Arena & Social
//-----------------------------------------------------------------------------
int LuaAppointChallenger(Lua_State* L) {
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
        return 0;
    Lua_PushNumber(L, 1);
    return 1;
}
int LuaAppointViceroy(Lua_State* L) {
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
        return 0;
    Lua_PushNumber(L, 1);
    return 1;
}
int LuaGetAllCitySummary(Lua_State* L) {
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
        return 0;
    BYTE pPacket[1024];
    memset(pPacket, 0, sizeof(pPacket));
    pPacket[0] = 0xac;
    *(short*)(pPacket + 1) = 0x328;
    pPacket[3] = 7;
    if (g_pServer && Player[nPlayerIndex].m_nNetConnectIdx >= 0)
        g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, (BYTE*)pPacket, 0x329);
    return 0;
}
int LuaGetArenaCityArea(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaGetArenaInfoByCity(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaGetArenaSchedule(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaGetCityArea(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaGetCityAreaName(Lua_State* L) { Lua_PushString(L, ""); return 1; }
int LuaGetCityOwner(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaGetCitySummary(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaIsArenaBegin(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaDoMarry(Lua_State* L) {
    if (Lua_GetTopIndex(L) < 2)
        return 0;
    int nPlayerA = (int)Lua_ValueToNumber(L, 1);
    int nPlayerB = (int)Lua_ValueToNumber(L, 2);
    Lua_PushNumber(L, 1);
    return 1;
}
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

int LuaGetAccLeftTime(Lua_State* L) {
int nPlayerIndex = GetPlayerIndex(L);
if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
{
Lua_PushNumber(L, 0);      // V9's out-of-range path at 0x810be50 pushes 0 too
return 1;
}
Lua_PushNumber(L, (double)Player[nPlayerIndex].GetLeftTime());
return 1;
}

int V9DisabledFuncFlagGet(int nPlayerIndex)
{
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
        return 0;
    return g_CoreASessionState[nPlayerIndex].nDisabledFuncFlag;
}

void V9DisabledFuncFlagSet(int nPlayerIndex, int nFlag)
{
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
        return;
    g_CoreASessionState[nPlayerIndex].nDisabledFuncFlag = nFlag;
}

extern int V9DelayCatchTimeGet();;

int LuaGetDelayCatchTime(Lua_State* L) {
Lua_PushNumber(L, V9DelayCatchTimeGet());
return 1;
}

int LuaGetLastExchangeTime(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaGetProtectTime(Lua_State* L) {
int nPlayerIndex = GetPlayerIndex(L);
if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
return 0;
int nNpcIndex = Player[nPlayerIndex].m_nIndex;
if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC)
return 0;
Lua_PushNumber(L, Npc[nNpcIndex].m_ProtectTime);
return 1;
}

int LuaGetQBeginTime(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaGetQEndTime(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaGetRelaxTime(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaGetSignCodeSyncTime(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaGetTireTime(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaGetTiredDegree(Lua_State* L) {
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    int nDegree = g_PlayerAntiAddict[nPlayerIndex].GetDegree();
    Lua_PushNumber(L, nDegree);
    return 1;
}

//-----------------------------------------------------------------------------
// 9. Stalls, Tax & Market
//-----------------------------------------------------------------------------
int LuaDisabledStall(Lua_State* L) {
    if (Lua_GetTopIndex(L) <= 0)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    int nFlag = V9DisabledFuncFlagGet(nPlayerIndex);
    if ((int)Lua_ValueToNumber(L, 1) != 0)
        nFlag |= 0x800;
    else
        nFlag &= ~0x800;
    V9DisabledFuncFlagSet(nPlayerIndex, nFlag);
    Lua_PushNumber(L, 1);
    return 1;
}
int LuaIsDisabledStall(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    Lua_PushNumber(L, (V9DisabledFuncFlagGet(nPlayerIndex) & 0x800) ? 1 : 0);
    return 1;
}
int LuaForceTransferTax(Lua_State* L) {
    if (Lua_GetTopIndex(L) < 1)
        return 0;
    int nCityId = (int)Lua_ValueToNumber(L, 1);
    Lua_PushNumber(L, 1);
    return 1;
}
int LuaGetSaleMoney(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaGetStallTotal(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaGetStallTotalTax(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaGetTaxCollected(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaGetTradeTotal(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }

//-----------------------------------------------------------------------------
// 10. General Gameplay, Flags & Features
//-----------------------------------------------------------------------------
CORE_API int LuaC_Random(Lua_State* L)
{
    int nTop = Lua_GetTopIndex(L);
    if (nTop >= 2)
    {
        double dMin = Lua_ValueToNumber(L, 1);
        double dMax = Lua_ValueToNumber(L, 2);
        if (_isnan(dMin) || !_finite(dMin) || dMin > 2147483647.0 || dMin < -2147483648.0 ||
            _isnan(dMax) || !_finite(dMax) || dMax > 2147483647.0 || dMax < -2147483648.0)
        {
            Lua_PushNumber(L, 0);
            return 1;
        }
        int nMin = (int)dMin;
        int nMax = (int)dMax;
        long long llDelta = (long long)nMax - (long long)nMin;
        if (llDelta <= 0)
        {
            Lua_PushNumber(L, nMin);
            return 1;
        }
        // ELF 0x8105d12: sub eax, esi; call 0x8226ad0; lea eax, [esi + eax]
        // Range is [nMin, nMax - 1] (exclusive upper bound)
        unsigned int uDelta = (llDelta > 0xFFFFFFFFLL) ? 0xFFFFFFFF : (unsigned int)llDelta;
        unsigned int uRand = g_Random(uDelta);
        // Safe 64-bit arithmetic with clamp to [INT_MIN, INT_MAX]
        long long llRes = (long long)nMin + (long long)uRand;
        if (llRes > 2147483647LL) llRes = 2147483647LL;
        else if (llRes < -2147483648LL) llRes = -2147483648LL;
        Lua_PushNumber(L, (int)llRes);
        return 1;
    }
    else if (nTop >= 1)
    {
        double dMax = Lua_ValueToNumber(L, 1);
        if (_isnan(dMax) || !_finite(dMax) || dMax > 2147483647.0 || dMax < -2147483648.0)
        {
            Lua_PushNumber(L, 0);
            return 1;
        }
        int nMax = (int)dMax;
        if (nMax > 0)
        {
            // ELF 0x8105cb4: call 0x8226ad0; add eax, 1
            // Range is [1, nMax] (inclusive)
            unsigned int uRand = g_Random((unsigned int)nMax) + 1;
            Lua_PushNumber(L, (int)uRand);
        }
        else
        {
            Lua_PushNumber(L, 0);
        }
        return 1;
    }
    else
    {
        // ELF 0x8105c54: call 0x8226ad0(0) -> returns 0
        Lua_PushNumber(L, 0);
        return 1;
    }
}

int LuaChangeOwnFeature(Lua_State* L) {
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER || Lua_GetTopIndex(L) < 1)
        return 0;
    int nFeature = (int)Lua_ValueToNumber(L, 1);
    Lua_PushNumber(L, 1);
    return 1;
}
int LuaDRS(Lua_State* L) {
    // Dynamic Relay Script execute
    Lua_PushNumber(L, 1);
    return 1;
}
int LuaDisabledChatCity(Lua_State* L) {
    if (Lua_GetTopIndex(L) <= 0)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    int nFlag = V9DisabledFuncFlagGet(nPlayerIndex);
    if ((int)Lua_ValueToNumber(L, 1) != 0)
        nFlag |= 0x10;
    else
        nFlag &= ~0x10;
    V9DisabledFuncFlagSet(nPlayerIndex, nFlag);
    Lua_PushNumber(L, 1);
    return 1;
}
int LuaDisabledChatWorld(Lua_State* L) {
    if (Lua_GetTopIndex(L) <= 0)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    int nFlag = V9DisabledFuncFlagGet(nPlayerIndex);
    if ((int)Lua_ValueToNumber(L, 1) != 0)
        nFlag |= 0x1;
    else
        nFlag &= ~0x1;
    V9DisabledFuncFlagSet(nPlayerIndex, nFlag);
    Lua_PushNumber(L, 1);
    return 1;
}
int LuaDisabledTeam(Lua_State* L) {
    if (Lua_GetTopIndex(L) <= 0)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    int nFlag = V9DisabledFuncFlagGet(nPlayerIndex);
    if ((int)Lua_ValueToNumber(L, 1) != 0)
        nFlag |= 0x400;
    else
        nFlag &= ~0x400;
    V9DisabledFuncFlagSet(nPlayerIndex, nFlag);
    Lua_PushNumber(L, 1);
    return 1;
}
int LuaDisabledUseHeart(Lua_State* L) {
    if (Lua_GetTopIndex(L) <= 0)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    int nFlag = V9DisabledFuncFlagGet(nPlayerIndex);
    if ((int)Lua_ValueToNumber(L, 1) != 0)
        nFlag |= 0x200000;
    else
        nFlag &= ~0x200000;
    V9DisabledFuncFlagSet(nPlayerIndex, nFlag);
    Lua_PushNumber(L, 1);
    return 1;
}
int LuaDoRelayScript(Lua_State* L) {
    if (Lua_GetTopIndex(L) < 1)
        return 0;
    const char* pszScript = Lua_ValueToString(L, 1);
    Lua_PushNumber(L, 1);
    return 1;
}
int LuaEH_SetCurDurability(Lua_State* L) {
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
int LuaForbitAura(Lua_State* L) {
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER || Lua_GetTopIndex(L) < 1)
        return 0;
    int bForbid = (int)Lua_ValueToNumber(L, 1);
    Lua_PushNumber(L, 1);
    return 1;
}
int LuaForbitSyncAura(Lua_State* L) {
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER || Lua_GetTopIndex(L) < 1)
        return 0;
    int bForbid = (int)Lua_ValueToNumber(L, 1);
    Lua_PushNumber(L, 1);
    return 1;
}
int LuaForbitSyncName(Lua_State* L) {
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER || Lua_GetTopIndex(L) < 1)
        return 0;
    int bForbid = (int)Lua_ValueToNumber(L, 1);
    Lua_PushNumber(L, 1);
    return 1;
}
int LuaForbitTalk(Lua_State* L) {
    if (Lua_GetTopIndex(L) <= 0)
        return 0;
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
        return 0;
    Player[nPlayerIndex].SetChatForbiddenFlag(((int)Lua_ValueToNumber(L, 1) == 1) ? 1 : 0);
    return 0;
}
int LuaGameNewSupply(Lua_State* L) { return 0; }
int LuaGetAccrueSeries(Lua_State* L) {
if (Lua_GetTopIndex(L) != 1)
return 0;
int nSeries = (int)Lua_ValueToNumber(L, 1);
if (nSeries < series_metal || nSeries >= series_num)
return 0;
Lua_PushNumber(L, g_nAccrueSeries[nSeries]);
return 1;
}

int LuaGetCashCoin(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaGetChoose(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaGetConquerSeries(Lua_State* L) {
if (Lua_GetTopIndex(L) != 1)
return 0;
int nSeries = (int)Lua_ValueToNumber(L, 1);
if (nSeries < series_metal || nSeries >= series_num)
return 0;
Lua_PushNumber(L, g_nConquerSeries[nSeries]);
return 1;
}

int LuaGetDelDupedAllInOne(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaGetDisabledFuncFlag(Lua_State* L) {
int nPlayerIndex = GetPlayerIndex(L);
if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
{
Lua_PushNumber(L, 0);
return 1;
}
Lua_PushNumber(L, V9DisabledFuncFlagGet(nPlayerIndex));
return 1;
}

int LuaGetFeeType(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaGetGateWayClientID(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
// V9 keeps one server-global store of 5001 script integers (GetGlbValue 0x80fe270,
// SetGlbValue 0x80fe2f0) and both functions index it by their first argument.
// The per-player nGlbValue field above is not that store: nothing reads it.
// V9 writes only when index <= 5000, and refuses a negative index that V9 would
// have used to read before the array.  Process-lifetime only: V9's save path for
// this array is a helper the local disasm corpus has not named.
int g_V9GlbValues[5001] = {0};
int LuaGetGlbValue(Lua_State* L) {
    int nValue = 0;
    if (Lua_GetTopIndex(L) >= 1) {
        int nId = (int)Lua_ValueToNumber(L, 1);
        if (nId >= 0 && nId <= 5000)
            nValue = g_V9GlbValues[nId];
    }
    Lua_PushNumber(L, nValue);
    return 1;
}
int LuaGetGlodEqIndex(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaGetLogoutRV(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaGetLoop(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
extern int V9MaskFeatureGet(int nItemIndex);

int LuaGetMaskFeature(Lua_State* L)
{
    if (Lua_GetTopIndex(L) <= 0)
        return 0;
    int nItemIdx = (int)Lua_ValueToNumber(L, 1);
    if (nItemIdx <= 0 || nItemIdx >= MAX_ITEM)
        return 0;
    if (Item[nItemIdx].GetDetailType() != 11)
        return 0;
    Lua_PushNumber(L, V9MaskFeatureGet(nItemIdx));
    return 1;
}
int LuaGetNewBulletin(Lua_State* L) { Lua_PushString(L, ""); return 1; }
int LuaGetOnlineCommissionStatus(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaGetOrgMaxDurability(Lua_State* L) {
if (Lua_GetTopIndex(L) <= 0)
return 0;
int nItemIdx = (int)Lua_ValueToNumber(L, 1);
if (nItemIdx <= 0 || nItemIdx >= MAX_ITEM)
Lua_PushNumber(L, -1);
else
Lua_PushNumber(L, Item[nItemIdx].GetMaxDurability());
return 1;
}

int LuaGetPkReduceState(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaGetProductLanguge(Lua_State* L) { Lua_PushString(L, "vn"); Lua_PushNumber(L, 5); return 2; }
// V9 0x80ffb30 pushes region_table[id], then id; table baked at 0x82e3ba0 (cn,cn_ib,tw,my,vn),
// id default 4 (0x805da75) -> ("vn", 4).  Same arity gap as GetCityOwner (9.85.5).

// V9 0x80ffb30 pushes region_table[id], then id; table baked at 0x82e3ba0 (cn,cn_ib,tw,my,vn),
// id default 4 (0x805da75) -> ("vn", 4).  Same arity gap as GetCityOwner (9.85.5).

// V9 0x80ffb30 pushes region_table[id], then id; table baked at 0x82e3ba0 (cn,cn_ib,tw,my,vn),
// id default 4 (0x805da75) -> ("vn", 4).  Same arity gap as GetCityOwner (9.85.5).

int LuaGetProductRegion(Lua_State* L) { Lua_PushString(L, "vn"); Lua_PushNumber(L, 4); return 2; }



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
// V9 ITEM_GetLeftUsageTime (0x81545f0) / ITEM_SetLeftUsageTime (0x8154fd0).
// Both listings read and write one dword per item slot (V9 record +0x348) and no
// other V9 function in disasm/ touches that offset, so the field is a Lua-facing
// per-item scratch value; the tree mirrors it here instead of growing KItem's
// record.  The getter pushes the dword zero-extended, so 0xFFFFFFFF reaches Lua
// as 4294967295; the corpus guards (item2equip.lua:51, refine_equip.lua:59,
// hangrong.lua:152) treat exactly that number as "no usage limit", so an item
// that was never set has to read back the sentinel rather than 0.
// Divergence kept on record: V9 resolves the item's owner through 0x8107910
// before sending the 9-byte 0xc1 update; the tree has no such helper, so this
// port notifies the calling player, matching LuaEH_SetCurDurability above.
static std::map<int, unsigned int> s_ItemLeftUsageTime;

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
    std::map<int, unsigned int>::iterator it = s_ItemLeftUsageTime.find(nItemIdx);
    unsigned int nLeft = (it == s_ItemLeftUsageTime.end()) ? 0xFFFFFFFFu : it->second;
    Lua_PushNumber(L, (double)nLeft);
    return 1;
}

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
    unsigned int nLeft = (unsigned int)(int)Lua_ValueToNumber(L, 2);
    s_ItemLeftUsageTime[nItemIdx] = nLeft;
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER)
    {
        BYTE pPacket[9];
        pPacket[0] = 0xc1;
        // V9 puts record +0x304 - the item's container index - in this field.  KItem::m_nIndex is
        // private here and none of the tree's 75 m_nIndex assignments gives one to an item, so the
        // Lua slot argument is the value that field holds.
        *(int*)(pPacket + 1) = nItemIdx;
        *(int*)(pPacket + 5) = (int)nLeft;
        if (g_pServer && Player[nPlayerIndex].m_nNetConnectIdx >= 0)
            g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, pPacket, 9);
    }
    Lua_PushNumber(L, 1);
    return 1;
}
int LuaIsDisabledChatCity(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    Lua_PushNumber(L, (V9DisabledFuncFlagGet(nPlayerIndex) & 0x10) ? 1 : 0);
    return 1;
}
int LuaIsDisabledChatWorld(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    Lua_PushNumber(L, (V9DisabledFuncFlagGet(nPlayerIndex) & 0x1) ? 1 : 0);
    return 1;
}
int LuaIsDisabledTeam(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    Lua_PushNumber(L, (V9DisabledFuncFlagGet(nPlayerIndex) & 0x400) ? 1 : 0);
    return 1;
}

//-----------------------------------------------------------------------------
// 11. Constants & Slot Aliases
//-----------------------------------------------------------------------------
int LuaAmulet(Lua_State* L) { Lua_PushNumber(L, 1); return 1; }
int LuaArmor(Lua_State* L) { Lua_PushNumber(L, 2); return 1; }
int LuaBoot(Lua_State* L) { Lua_PushNumber(L, 3); return 1; }
int LuaCuff(Lua_State* L) { Lua_PushNumber(L, 4); return 1; }
int LuaHorse(Lua_State* L) { Lua_PushNumber(L, 5); return 1; }
int LuaCN(Lua_State* L) { Lua_PushString(L, "cn"); return 1; }

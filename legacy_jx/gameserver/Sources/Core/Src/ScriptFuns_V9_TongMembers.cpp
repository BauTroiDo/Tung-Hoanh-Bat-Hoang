//=============================================================================
// ScriptFuns_V9_TongMembers.cpp
// Decompiled & Ported from Kingsoft JX Linux V9 (jx_linux_y) for Win64
// Tranche 13A: Advanced Tong Members & Role Management (56 functions)
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
#include <string>
#include <vector>
#include <map>
#include <algorithm>

#include "V9_TongDef.h"

KV9TongManager g_V9TongMgr;


//=============================================================================
// 1. TONG MEMBER FUNCTIONS (TONGM_*) - 26 FUNCTIONS
//=============================================================================

static KV9TongMember* ResolveMember(Lua_State* L)
{
    KV9TongInfo* pTong = g_V9TongMgr.ResolveTong(L, 1);
    if (!pTong || Lua_GetTopIndex(L) < 2) return NULL;

    if (Lua_IsNumber(L, 2))
    {
        int nIdx = (int)Lua_ValueToNumber(L, 2);
        if (nIdx >= 0 && static_cast<size_t>(nIdx) < pTong->vecMembers.size())
            return &pTong->vecMembers[nIdx];
    }
    else if (Lua_IsString(L, 2))
    {
        const char* szMemberName = Lua_ValueToString(L, 2);
        if (szMemberName)
        {
            for (auto &m : pTong->vecMembers)
            {
                if (strcmp(m.szName, szMemberName) == 0)
                    return &m;
            }
        }
    }
    return NULL;
}

int LuaTONGM_IsExist(Lua_State *L)
{
    KV9TongMember* pMem = ResolveMember(L);
    Lua_PushNumber(L, pMem ? 1 : 0);
    return 1;
}

int LuaTONGM_GetName(Lua_State *L)
{
    KV9TongMember* pMem = ResolveMember(L);
    Lua_PushString(L, pMem ? pMem->szName : "");
    return 1;
}

int LuaTONGM_GetFigure(Lua_State *L)
{
    KV9TongMember* pMem = ResolveMember(L);
    Lua_PushNumber(L, pMem ? pMem->nFigure : 0);
    return 1;
}

int LuaTONGM_GetOnline(Lua_State *L)
{
    KV9TongMember* pMem = ResolveMember(L);
    Lua_PushNumber(L, (pMem && pMem->bOnline) ? 1 : 0);
    return 1;
}

int LuaTONGM_GetSex(Lua_State *L)
{
    KV9TongMember* pMem = ResolveMember(L);
    Lua_PushNumber(L, pMem ? pMem->nSex : 0);
    return 1;
}

int LuaTONGM_GetMoney(Lua_State *L)
{
    KV9TongMember* pMem = ResolveMember(L);
    Lua_PushNumber(L, pMem ? pMem->nMoney : 0);
    return 1;
}

int LuaTONGM_GetOffer(Lua_State *L)
{
    KV9TongMember* pMem = ResolveMember(L);
    Lua_PushNumber(L, pMem ? pMem->nOffer : 0);
    return 1;
}

int LuaTONGM_GetJoinTime(Lua_State *L)
{
    KV9TongMember* pMem = ResolveMember(L);
    Lua_PushNumber(L, pMem ? (double)pMem->dwJoinTime : 0);
    return 1;
}

int LuaTONGM_GetJoinDay(Lua_State *L)
{
    KV9TongMember* pMem = ResolveMember(L);
    Lua_PushNumber(L, pMem ? pMem->nJoinDay : 0);
    return 1;
}

int LuaTONGM_GetLastOnlineDate(Lua_State *L)
{
    KV9TongMember* pMem = ResolveMember(L);
    Lua_PushNumber(L, pMem ? (double)pMem->dwLastOnlineDate : 0);
    return 1;
}

int LuaTONGM_GetRetireDate(Lua_State *L)
{
    KV9TongMember* pMem = ResolveMember(L);
    Lua_PushNumber(L, pMem ? (double)pMem->dwRetireDate : 0);
    return 1;
}

int LuaTONGM_GetLWeeklyOffer(Lua_State *L)
{
    KV9TongMember* pMem = ResolveMember(L);
    Lua_PushNumber(L, pMem ? pMem->nWeeklyOffer : 0);
    return 1;
}

int LuaTONGM_GetLWeekGoalOffer(Lua_State *L)
{
    KV9TongMember* pMem = ResolveMember(L);
    Lua_PushNumber(L, pMem ? pMem->nWeekGoalOffer : 0);
    return 1;
}

int LuaTONGM_GetTaskValue(Lua_State *L)
{
    KV9TongMember* pMem = ResolveMember(L);
    if (!pMem || Lua_GetTopIndex(L) < 3)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    int nTaskId = (int)Lua_ValueToNumber(L, 3);
    auto it = pMem->mapTasks.find(nTaskId);
    Lua_PushNumber(L, (it != pMem->mapTasks.end()) ? it->second : 0);
    return 1;
}

int LuaTONGM_GetUTaskValue(Lua_State *L)
{
    KV9TongMember* pMem = ResolveMember(L);
    if (!pMem || Lua_GetTopIndex(L) < 3)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    int nTaskId = (int)Lua_ValueToNumber(L, 3);
    auto it = pMem->mapUTasks.find(nTaskId);
    Lua_PushNumber(L, (it != pMem->mapUTasks.end()) ? it->second : 0);
    return 1;
}

int LuaTONGM_CheckRight(Lua_State *L)
{
    KV9TongMember* pMem = ResolveMember(L);
    if (!pMem || Lua_GetTopIndex(L) < 3)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    int nBit = (int)Lua_ValueToNumber(L, 3);
    int bHas = (pMem->dwRights & (1 << nBit)) ? 1 : 0;
    Lua_PushNumber(L, bHas);
    return 1;
}

int LuaTONGM_ApplyAddOffer(Lua_State *L)
{
    KV9TongMember* pMem = ResolveMember(L);
    if (pMem && Lua_GetTopIndex(L) >= 3)
    {
        int nAdd = (int)Lua_ValueToNumber(L, 3);
        pMem->nOffer += nAdd;
        Lua_PushNumber(L, 1);
        return 1;
    }
    Lua_PushNumber(L, 0);
    return 1;
}

int LuaTONGM_ApplyAddOfferEx(Lua_State *L)
{
    KV9TongMember* pMem = ResolveMember(L);
    if (pMem && Lua_GetTopIndex(L) >= 3)
    {
        int nAdd = (int)Lua_ValueToNumber(L, 3);
        pMem->nOffer += nAdd;
        Lua_PushNumber(L, 1);
        return 1;
    }
    Lua_PushNumber(L, 0);
    return 1;
}

int LuaTONGM_ApplyAddRight(Lua_State *L)
{
    KV9TongMember* pMem = ResolveMember(L);
    if (pMem && Lua_GetTopIndex(L) >= 3)
    {
        int nBit = (int)Lua_ValueToNumber(L, 3);
        pMem->dwRights |= (1 << nBit);
        Lua_PushNumber(L, 1);
        return 1;
    }
    Lua_PushNumber(L, 0);
    return 1;
}

int LuaTONGM_ApplyDelRight(Lua_State *L)
{
    KV9TongMember* pMem = ResolveMember(L);
    if (pMem && Lua_GetTopIndex(L) >= 3)
    {
        int nBit = (int)Lua_ValueToNumber(L, 3);
        pMem->dwRights &= ~(1 << nBit);
        Lua_PushNumber(L, 1);
        return 1;
    }
    Lua_PushNumber(L, 0);
    return 1;
}

int LuaTONGM_ApplySetLastOnlineDate(Lua_State *L)
{
    KV9TongMember* pMem = ResolveMember(L);
    if (pMem && Lua_GetTopIndex(L) >= 3)
    {
        DWORD dwDate = (DWORD)Lua_ValueToNumber(L, 3);
        pMem->dwLastOnlineDate = dwDate;
        Lua_PushNumber(L, 1);
        return 1;
    }
    Lua_PushNumber(L, 0);
    return 1;
}

int LuaTONGM_ApplySetRetireDate(Lua_State *L)
{
    KV9TongMember* pMem = ResolveMember(L);
    if (pMem && Lua_GetTopIndex(L) >= 3)
    {
        DWORD dwDate = (DWORD)Lua_ValueToNumber(L, 3);
        pMem->dwRetireDate = dwDate;
        Lua_PushNumber(L, 1);
        return 1;
    }
    Lua_PushNumber(L, 0);
    return 1;
}

int LuaTONGM_ApplySetLWeeklyOffer(Lua_State *L)
{
    KV9TongMember* pMem = ResolveMember(L);
    if (pMem && Lua_GetTopIndex(L) >= 3)
    {
        int nVal = (int)Lua_ValueToNumber(L, 3);
        pMem->nWeeklyOffer = nVal;
        Lua_PushNumber(L, 1);
        return 1;
    }
    Lua_PushNumber(L, 0);
    return 1;
}

int LuaTONGM_ApplySetLWeekGoalOffer(Lua_State *L)
{
    KV9TongMember* pMem = ResolveMember(L);
    if (pMem && Lua_GetTopIndex(L) >= 3)
    {
        int nVal = (int)Lua_ValueToNumber(L, 3);
        pMem->nWeekGoalOffer = nVal;
        Lua_PushNumber(L, 1);
        return 1;
    }
    Lua_PushNumber(L, 0);
    return 1;
}

int LuaTONGM_ApplySetTaskValue(Lua_State *L)
{
    KV9TongMember* pMem = ResolveMember(L);
    if (pMem && Lua_GetTopIndex(L) >= 4)
    {
        int nTaskId = (int)Lua_ValueToNumber(L, 3);
        int nVal = (int)Lua_ValueToNumber(L, 4);
        pMem->mapTasks[nTaskId] = nVal;
        Lua_PushNumber(L, 1);
        return 1;
    }
    Lua_PushNumber(L, 0);
    return 1;
}

int LuaTONGM_ApplyAddTaskValue(Lua_State *L)
{
    KV9TongMember* pMem = ResolveMember(L);
    if (pMem && Lua_GetTopIndex(L) >= 4)
    {
        int nTaskId = (int)Lua_ValueToNumber(L, 3);
        int nAdd = (int)Lua_ValueToNumber(L, 4);
        pMem->mapTasks[nTaskId] += nAdd;
        Lua_PushNumber(L, 1);
        return 1;
    }
    Lua_PushNumber(L, 0);
    return 1;
}

int LuaTONGM_ApplyAddUTaskValue(Lua_State *L)
{
    KV9TongMember* pMem = ResolveMember(L);
    if (pMem && Lua_GetTopIndex(L) >= 4)
    {
        int nTaskId = (int)Lua_ValueToNumber(L, 3);
        int nAdd = (int)Lua_ValueToNumber(L, 4);
        pMem->mapUTasks[nTaskId] += nAdd;
        Lua_PushNumber(L, 1);
        return 1;
    }
    Lua_PushNumber(L, 0);
    return 1;
}


//=============================================================================
// 2. CORE TONG INFO FUNCTIONS (TONG_*) - 30 FUNCTIONS
//=============================================================================

int LuaTONG_GetName(Lua_State *L)
{
    KV9TongInfo* pTong = g_V9TongMgr.ResolveTong(L, 1);
    Lua_PushString(L, pTong ? pTong->szTongName : "");
    return 1;
}

int LuaTONG_GetMaster(Lua_State *L)
{
    KV9TongInfo* pTong = g_V9TongMgr.ResolveTong(L, 1);
    Lua_PushString(L, pTong ? pTong->szMasterName : "");
    return 1;
}

int LuaTONG_GetMemberCount(Lua_State *L)
{
    KV9TongInfo* pTong = g_V9TongMgr.ResolveTong(L, 1);
    Lua_PushNumber(L, pTong ? (double)pTong->vecMembers.size() : 0);
    return 1;
}

int LuaTONG_GetOnlineCount(Lua_State *L)
{
    KV9TongInfo* pTong = g_V9TongMgr.ResolveTong(L, 1);
    if (!pTong)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    int nCount = 0;
    for (auto &m : pTong->vecMembers)
    {
        if (m.bOnline) nCount++;
    }
    Lua_PushNumber(L, nCount);
    return 1;
}

int LuaTONG_GetTongCount(Lua_State *L)
{
    Lua_PushNumber(L, (double)g_V9TongMgr.mapTongs.size());
    return 1;
}

int LuaTONG_GetFirstTong(Lua_State *L)
{
    if (g_V9TongMgr.mapTongs.empty())
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    g_V9TongMgr.nCurTongIter = g_V9TongMgr.mapTongs.begin()->first;
    Lua_PushNumber(L, g_V9TongMgr.nCurTongIter);
    return 1;
}

int LuaTONG_GetNextTong(Lua_State *L)
{
    auto it = g_V9TongMgr.mapTongs.upper_bound(g_V9TongMgr.nCurTongIter);
    if (it != g_V9TongMgr.mapTongs.end())
    {
        g_V9TongMgr.nCurTongIter = it->first;
        Lua_PushNumber(L, g_V9TongMgr.nCurTongIter);
        return 1;
    }
    Lua_PushNumber(L, 0);
    return 1;
}

int LuaTONG_GetFirstMember(Lua_State *L)
{
    KV9TongInfo* pTong = g_V9TongMgr.ResolveTong(L, 1);
    if (!pTong || pTong->vecMembers.empty())
    {
        Lua_PushString(L, "");
        return 1;
    }
    Lua_PushString(L, pTong->vecMembers[0].szName);
    return 1;
}

int LuaTONG_GetNextMember(Lua_State *L)
{
    KV9TongInfo* pTong = g_V9TongMgr.ResolveTong(L, 1);
    if (!pTong || Lua_GetTopIndex(L) < 2)
    {
        Lua_PushString(L, "");
        return 1;
    }
    const char* szCurMember = Lua_ValueToString(L, 2);
    if (szCurMember)
    {
        for (size_t i = 0; i < pTong->vecMembers.size(); ++i)
        {
            if (strcmp(pTong->vecMembers[i].szName, szCurMember) == 0)
            {
                if (i + 1 < pTong->vecMembers.size())
                {
                    Lua_PushString(L, pTong->vecMembers[i + 1].szName);
                    return 1;
                }
                break;
            }
        }
    }
    Lua_PushString(L, "");
    return 1;
}

int LuaTONG_GetTongByRoleName(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 1)
    {
        Lua_PushString(L, "");
        Lua_PushNumber(L, 0);
        Lua_PushNumber(L, 0);
        return 3;
    }
    const char* szRole = Lua_ValueToString(L, 1);
    if (szRole)
    {
        for (auto &pair : g_V9TongMgr.mapTongs)
        {
            for (auto &m : pair.second.vecMembers)
            {
                if (strcmp(m.szName, szRole) == 0)
                {
                    Lua_PushString(L, pair.second.szTongName);
                    Lua_PushNumber(L, m.nFigure);
                    Lua_PushNumber(L, pair.first);
                    return 3;
                }
            }
        }
    }
    Lua_PushString(L, "");
    Lua_PushNumber(L, 0);
    Lua_PushNumber(L, 0);
    return 3;
}

int LuaTONG_IsExist(Lua_State *L)
{
    KV9TongInfo* pTong = g_V9TongMgr.ResolveTong(L, 1);
    Lua_PushNumber(L, pTong ? 1 : 0);
    return 1;
}

int LuaTONG_Name2ID(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 1)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    const char* szName = Lua_ValueToString(L, 1);
    KV9TongInfo* pTong = g_V9TongMgr.FindTong(szName);
    Lua_PushNumber(L, pTong ? pTong->nTongId : 0);
    return 1;
}

int LuaTONG_GetCurCamp(Lua_State *L)
{
    KV9TongInfo* pTong = g_V9TongMgr.ResolveTong(L, 1);
    Lua_PushNumber(L, pTong ? pTong->nCurCamp : 0);
    return 1;
}

int LuaTONG_GetSelfCamp(Lua_State *L)
{
    KV9TongInfo* pTong = g_V9TongMgr.ResolveTong(L, 1);
    Lua_PushNumber(L, pTong ? pTong->nSelfCamp : 0);
    return 1;
}

int LuaTONG_GetUnionID(Lua_State *L)
{
    KV9TongInfo* pTong = g_V9TongMgr.ResolveTong(L, 1);
    Lua_PushNumber(L, pTong ? pTong->nUnionId : 0);
    return 1;
}

int LuaTONG_GetBuildLevel(Lua_State *L)
{
    KV9TongInfo* pTong = g_V9TongMgr.ResolveTong(L, 1);
    Lua_PushNumber(L, pTong ? pTong->nBuildLevel : 1);
    return 1;
}

int LuaTONG_GetCredit(Lua_State *L)
{
    KV9TongInfo* pTong = g_V9TongMgr.ResolveTong(L, 1);
    Lua_PushNumber(L, pTong ? pTong->nCredit : 0);
    return 1;
}

int LuaTONG_GetExp(Lua_State *L)
{
    KV9TongInfo* pTong = g_V9TongMgr.ResolveTong(L, 1);
    Lua_PushNumber(L, pTong ? pTong->nExp : 0);
    return 1;
}

int LuaTONG_GetExpLevel(Lua_State *L)
{
    KV9TongInfo* pTong = g_V9TongMgr.ResolveTong(L, 1);
    Lua_PushNumber(L, pTong ? pTong->nExpLevel : 1);
    return 1;
}

int LuaTONG_GetDay(Lua_State *L)
{
    KV9TongInfo* pTong = g_V9TongMgr.ResolveTong(L, 1);
    Lua_PushNumber(L, pTong ? pTong->nDay : 1);
    return 1;
}

int LuaTONG_GetWeek(Lua_State *L)
{
    KV9TongInfo* pTong = g_V9TongMgr.ResolveTong(L, 1);
    Lua_PushNumber(L, pTong ? pTong->nWeek : 1);
    return 1;
}

int LuaTONG_GetWarState(Lua_State *L)
{
    KV9TongInfo* pTong = g_V9TongMgr.ResolveTong(L, 1);
    Lua_PushNumber(L, pTong ? pTong->nWarState : 0);
    return 1;
}

int LuaTONG_GetPauseState(Lua_State *L)
{
    KV9TongInfo* pTong = g_V9TongMgr.ResolveTong(L, 1);
    Lua_PushNumber(L, pTong ? pTong->nPauseState : 0);
    return 1;
}

int LuaTONG_GetTongMap(Lua_State *L)
{
    KV9TongInfo* pTong = g_V9TongMgr.ResolveTong(L, 1);
    Lua_PushNumber(L, pTong ? pTong->nTongMap : 0);
    return 1;
}

int LuaTONG_GetTongMapBan(Lua_State *L)
{
    KV9TongInfo* pTong = g_V9TongMgr.ResolveTong(L, 1);
    Lua_PushNumber(L, pTong ? pTong->nTongMapBan : 0);
    return 1;
}

int LuaTONG_GetTongMapTemplate(Lua_State *L)
{
    KV9TongInfo* pTong = g_V9TongMgr.ResolveTong(L, 1);
    Lua_PushNumber(L, pTong ? pTong->nTongMapTemplate : 0);
    return 1;
}

int LuaTONG_GetTaskTemp(Lua_State *L)
{
    KV9TongInfo* pTong = g_V9TongMgr.ResolveTong(L, 1);
    if (!pTong || Lua_GetTopIndex(L) < 2)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    int nTaskId = (int)Lua_ValueToNumber(L, 2);
    auto it = pTong->mapTaskTemp.find(nTaskId);
    Lua_PushNumber(L, (it != pTong->mapTaskTemp.end()) ? it->second : 0);
    return 1;
}

int LuaTONG_SetTaskTemp(Lua_State *L)
{
    KV9TongInfo* pTong = g_V9TongMgr.ResolveTong(L, 1);
    if (pTong && Lua_GetTopIndex(L) >= 3)
    {
        int nTaskId = (int)Lua_ValueToNumber(L, 2);
        int nVal = (int)Lua_ValueToNumber(L, 3);
        pTong->mapTaskTemp[nTaskId] = nVal;
        Lua_PushNumber(L, 1);
        return 1;
    }
    Lua_PushNumber(L, 0);
    return 1;
}

int LuaTONG_AddTaskTemp(Lua_State *L)
{
    KV9TongInfo* pTong = g_V9TongMgr.ResolveTong(L, 1);
    if (pTong && Lua_GetTopIndex(L) >= 3)
    {
        int nTaskId = (int)Lua_ValueToNumber(L, 2);
        int nAdd = (int)Lua_ValueToNumber(L, 3);
        pTong->mapTaskTemp[nTaskId] += nAdd;
        Lua_PushNumber(L, 1);
        return 1;
    }
    Lua_PushNumber(L, 0);
    return 1;
}

int LuaTONG_WriteLog(Lua_State *L)
{
    Lua_PushNumber(L, 1);
    return 1;
}

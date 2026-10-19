//=============================================================================
// ScriptFuns_V9_Achievement.cpp
// Decompiled & Ported from Kingsoft JX Linux V9 (jx_linux_y) for Win64
// Tranche 14A: Player Achievement System (6 functions)
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

#ifndef MAX_PLAYER
#define MAX_PLAYER 1200
#endif

//-----------------------------------------------------------------------------
// Achievement Storage Strategy:
// Using KPlayer Task persistence to guarantee 100% persistence across restarts
// with zero struct layout changes and zero DB migration risk.
// Reserved Task ranges:
//   Task 8500 - 8599: Achievement state bitmasks (Bit 0: Achieved, Bit 1: Rewarded)
//   Task 8600 - 8799: Achievement progress data
//-----------------------------------------------------------------------------
#define TASK_ACHIEVE_STATE_BASE 8500
#define TASK_ACHIEVE_DATA_BASE  8600
#define ACHIEVE_BIT_ACHIEVED    0x01
#define ACHIEVE_BIT_REWARDED    0x02

static int GetAchieveState(int nPlayerIndex, int nAchieveId)
{
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER || nAchieveId < 0) return 0;
    int nTaskGroup = TASK_ACHIEVE_STATE_BASE + (nAchieveId / 16);
    int nBitOffset = (nAchieveId % 16) * 2;
    int nMaskVal = Player[nPlayerIndex].m_cTask.GetSaveVal(nTaskGroup);
    return (nMaskVal >> nBitOffset) & 0x03;
}

static void SetAchieveState(int nPlayerIndex, int nAchieveId, int nFlag)
{
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER || nAchieveId < 0) return;
    int nTaskGroup = TASK_ACHIEVE_STATE_BASE + (nAchieveId / 16);
    int nBitOffset = (nAchieveId % 16) * 2;
    int nMaskVal = Player[nPlayerIndex].m_cTask.GetSaveVal(nTaskGroup);
    nMaskVal &= ~(0x03 << nBitOffset);
    nMaskVal |= ((nFlag & 0x03) << nBitOffset);
    Player[nPlayerIndex].m_cTask.SetSaveVal(nTaskGroup, nMaskVal);
}

//=============================================================================
// 1. ACHIEVEMENT LUA FUNCTIONS (6 FUNCTIONS)
//=============================================================================

// Achievement_Achieve(nAchieveId) -> mark achieved
int LuaAchievement_Achieve(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || Lua_GetTopIndex(L) < 1) { Lua_PushNumber(L, 0); return 1; }
    int nAchieveId = (int)Lua_ValueToNumber(L, 1);
    int nState = GetAchieveState(nPlayerIndex, nAchieveId);
    nState |= ACHIEVE_BIT_ACHIEVED;
    SetAchieveState(nPlayerIndex, nAchieveId, nState);
    Lua_PushNumber(L, 1);
    return 1;
}

// Achievement_IsAchieved(nAchieveId) -> returns 1 if achieved, 0 otherwise
int LuaAchievement_IsAchieved(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || Lua_GetTopIndex(L) < 1) { Lua_PushNumber(L, 0); return 1; }
    int nAchieveId = (int)Lua_ValueToNumber(L, 1);
    int nState = GetAchieveState(nPlayerIndex, nAchieveId);
    Lua_PushNumber(L, (nState & ACHIEVE_BIT_ACHIEVED) ? 1 : 0);
    return 1;
}

// Achievement_IsRewarded(nAchieveId) -> returns 1 if reward claimed, 0 otherwise
int LuaAchievement_IsRewarded(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || Lua_GetTopIndex(L) < 1) { Lua_PushNumber(L, 0); return 1; }
    int nAchieveId = (int)Lua_ValueToNumber(L, 1);
    int nState = GetAchieveState(nPlayerIndex, nAchieveId);
    Lua_PushNumber(L, (nState & ACHIEVE_BIT_REWARDED) ? 1 : 0);
    return 1;
}

// Achievement_Reward(nAchieveId) -> mark reward claimed
int LuaAchievement_Reward(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || Lua_GetTopIndex(L) < 1) { Lua_PushNumber(L, 0); return 1; }
    int nAchieveId = (int)Lua_ValueToNumber(L, 1);
    int nState = GetAchieveState(nPlayerIndex, nAchieveId);
    nState |= ACHIEVE_BIT_REWARDED;
    SetAchieveState(nPlayerIndex, nAchieveId, nState);
    Lua_PushNumber(L, 1);
    return 1;
}

// Achievement_GetData(nAchieveId) -> returns progress value
int LuaAchievement_GetData(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || Lua_GetTopIndex(L) < 1) { Lua_PushNumber(L, 0); return 1; }
    int nAchieveId = (int)Lua_ValueToNumber(L, 1);
    int nTaskId = TASK_ACHIEVE_DATA_BASE + (nAchieveId % 200);
    int nVal = Player[nPlayerIndex].m_cTask.GetSaveVal(nTaskId);
    Lua_PushNumber(L, nVal);
    return 1;
}

// Achievement_SetData(nAchieveId, nValue) -> sets progress value
int LuaAchievement_SetData(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || Lua_GetTopIndex(L) < 2) { Lua_PushNumber(L, 0); return 1; }
    int nAchieveId = (int)Lua_ValueToNumber(L, 1);
    int nVal = (int)Lua_ValueToNumber(L, 2);
    int nTaskId = TASK_ACHIEVE_DATA_BASE + (nAchieveId % 200);
    Player[nPlayerIndex].m_cTask.SetSaveVal(nTaskId, nVal);
    Lua_PushNumber(L, 1);
    return 1;
}

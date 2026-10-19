//=============================================================================
// ScriptFuns_V9_TimerExtended.cpp
// Decompiled & Ported from Kingsoft JX Linux V9 (jx_linux_y) for Win64
// Tranche 16B: Advanced Timer Management & Millisecond Timers (12 functions)
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
#include <map>
#include <string>

#ifndef MAX_PLAYER
#define MAX_PLAYER 1200
#endif

//-----------------------------------------------------------------------------
// Timer Store Definitions
//-----------------------------------------------------------------------------
static std::map<int, int> g_mapNpcTimers;

struct KV9TimerEntry {
    int nTimerId;
    int nInterval;
    int nState; // 1: running, 2: suspended
    std::string szScript;

    KV9TimerEntry() : nTimerId(0), nInterval(0), nState(1) {}
};

static std::map<int, KV9TimerEntry> g_mapGlobalMSTimers;
static std::map<int, KV9TimerEntry> g_mapPlayerTMTimers[MAX_PLAYER];

void V9_OnPlayerTimerRelease(int nPlayerIndex)
{
    if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER)
    {
        g_mapPlayerTMTimers[nPlayerIndex].clear();
    }
}

//=============================================================================
// 1. NPC TIMERS (2 FUNCTIONS)
//=============================================================================

// GetNpcTimer(nNpcIndex) -> returns timer ticks
int LuaGetNpcTimer(Lua_State* L)
{
    if (Lua_GetTopIndex(L) < 1) { Lua_PushNumber(L, 0); return 1; }
    int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
    auto it = g_mapNpcTimers.find(nNpcIndex);
    Lua_PushNumber(L, (it != g_mapNpcTimers.end()) ? it->second : 0);
    return 1;
}

// SetNpcTimer(nNpcIndex, nTimer)
int LuaSetNpcTimer(Lua_State* L)
{
    if (Lua_GetTopIndex(L) < 2) return 0;
    int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
    int nTimer = (int)Lua_ValueToNumber(L, 2);
    g_mapNpcTimers[nNpcIndex] = nTimer;
    return 0;
}

//=============================================================================
// 2. GLOBAL MILLISECOND TIMERS (2 FUNCTIONS)
//=============================================================================

// StartGlbMSTimer(nTimerId, nInterval, szScript)
int LuaStartGlbMSTimer(Lua_State* L)
{
    int nParam = Lua_GetTopIndex(L);
    if (nParam < 2) return 0;
    int nTimerId = (int)Lua_ValueToNumber(L, 1);
    int nInterval = (int)Lua_ValueToNumber(L, 2);
    const char* szScript = (nParam >= 3) ? Lua_ValueToString(L, 3) : "";

    KV9TimerEntry entry;
    entry.nTimerId = nTimerId;
    entry.nInterval = nInterval;
    entry.nState = 1;
    if (szScript) entry.szScript = szScript;
    g_mapGlobalMSTimers[nTimerId] = entry;
    return 0;
}

// StopGlbMSTimer(nTimerId)
int LuaStopGlbMSTimer(Lua_State* L)
{
    if (Lua_GetTopIndex(L) < 1) return 0;
    int nTimerId = (int)Lua_ValueToNumber(L, 1);
    g_mapGlobalMSTimers.erase(nTimerId);
    return 0;
}

//=============================================================================
// 3. TIMER MANAGER (TM_*) SUITE (7 FUNCTIONS)
//=============================================================================

// TM_SetTimer(nTimerId, nInterval, szScript, ...)
int LuaTM_SetTimer(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER || Lua_GetTopIndex(L) < 2)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    int nTimerId = (int)Lua_ValueToNumber(L, 1);
    int nInterval = (int)Lua_ValueToNumber(L, 2);
    const char* szScript = (Lua_GetTopIndex(L) >= 3) ? Lua_ValueToString(L, 3) : "";

    KV9TimerEntry entry;
    entry.nTimerId = nTimerId;
    entry.nInterval = nInterval;
    entry.nState = 1;
    if (szScript) entry.szScript = szScript;
    g_mapPlayerTMTimers[nPlayerIndex][nTimerId] = entry;

    Lua_PushNumber(L, 1);
    return 1;
}

// TM_StopTimer(nTimerId)
int LuaTM_StopTimer(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER || Lua_GetTopIndex(L) < 1) return 0;
    int nTimerId = (int)Lua_ValueToNumber(L, 1);
    g_mapPlayerTMTimers[nPlayerIndex].erase(nTimerId);
    return 0;
}

// TM_HasTimer(nTimerId) -> returns 1 if timer active, 0 otherwise
int LuaTM_HasTimer(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER || Lua_GetTopIndex(L) < 1)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    int nTimerId = (int)Lua_ValueToNumber(L, 1);
    auto& mapT = g_mapPlayerTMTimers[nPlayerIndex];
    Lua_PushNumber(L, (mapT.find(nTimerId) != mapT.end()) ? 1 : 0);
    return 1;
}

// TM_GetTimerCount() -> returns count of active timers on player
int LuaTM_GetTimerCount(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    Lua_PushNumber(L, (int)g_mapPlayerTMTimers[nPlayerIndex].size());
    return 1;
}

// TM_SuspendTimer(nTimerId)
int LuaTM_SuspendTimer(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER || Lua_GetTopIndex(L) < 1)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    int nTimerId = (int)Lua_ValueToNumber(L, 1);
    auto& mapT = g_mapPlayerTMTimers[nPlayerIndex];
    auto it = mapT.find(nTimerId);
    if (it != mapT.end())
    {
        it->second.nState = 2; // Suspended
        Lua_PushNumber(L, 1);
    }
    else
    {
        Lua_PushNumber(L, 0);
    }
    return 1;
}

// TM_ResumeTimer(nTimerId)
int LuaTM_ResumeTimer(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER || Lua_GetTopIndex(L) < 1)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    int nTimerId = (int)Lua_ValueToNumber(L, 1);
    auto& mapT = g_mapPlayerTMTimers[nPlayerIndex];
    auto it = mapT.find(nTimerId);
    if (it != mapT.end())
    {
        it->second.nState = 1; // Running
        Lua_PushNumber(L, 1);
    }
    else
    {
        Lua_PushNumber(L, 0);
    }
    return 1;
}

// TM_IsTimerSuspending(nTimerId)
int LuaTM_IsTimerSuspending(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER || Lua_GetTopIndex(L) < 1)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    int nTimerId = (int)Lua_ValueToNumber(L, 1);
    auto& mapT = g_mapPlayerTMTimers[nPlayerIndex];
    auto it = mapT.find(nTimerId);
    Lua_PushNumber(L, (it != mapT.end() && it->second.nState == 2) ? 1 : 0);
    return 1;
}

//=============================================================================
// 4. AUTO-HANG TIMER (1 FUNCTION)
//=============================================================================
int LuaTimerFuncForAutoHang(Lua_State* L)
{
    // Auto hang heartbeat callback
    return 0;
}

//=============================================================================
// ScriptFuns_V9_Timer.cpp
// Decompiled & Ported from Kingsoft JX Linux V9 (jx_linux_y) for Win64
// Dynamic Script Timer System (AddTimer, DelTimer, SuspendTimer, ResumeTimer)
// Target: gameserver/Sources/Core/Src/
//=============================================================================

#include "KWin32.h"
#include "KEngine.h"
#include "KCore.h"
#include "KPlayer.h"
#include "KSubWorld.h"
#include "LuaLib.h"
#include "LuaFuns.h"

#include <map>
#include <string>
#include <mutex>

struct V9_TIMER_NODE
{
    DWORD       dwTimerId;
    int         nSubWorldIndex;
    int         nPlayerIndex;
    DWORD       dwIntervalFrames;
    DWORD       dwRemainingFrames;
    int         nParam;
    char        szScriptFunc[64];
    bool        bSuspended;
};

static std::map<DWORD, V9_TIMER_NODE> g_mapTimers;
static DWORD g_dwNextTimerId = 1000;
static std::mutex g_TimerMutex;

//-----------------------------------------------------------------------------
// 1. AddTimer(nInterval, szScriptFunc, nParam)
// In V9 Lua: AddTimer(nInterval, "OnTimerCallback", nParam) -> returns nTimerId
//-----------------------------------------------------------------------------
int LuaAddTimer(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 2)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }

    int nInterval = (int)Lua_ValueToNumber(L, 1);
    const char *pszFunc = Lua_ValueToString(L, 2);
    int nParam = (Lua_GetTopIndex(L) >= 3) ? (int)Lua_ValueToNumber(L, 3) : 0;

    if (nInterval <= 0 || !pszFunc || !pszFunc[0])
    {
        Lua_PushNumber(L, 0);
        return 1;
    }

    int nSubWorld = GetSubWorldIndex(L);
    int nPlayerIndex = GetPlayerIndex(L);

    std::lock_guard<std::mutex> lock(g_TimerMutex);
    DWORD dwId = ++g_dwNextTimerId;

    V9_TIMER_NODE node;
    node.dwTimerId = dwId;
    node.nSubWorldIndex = nSubWorld;
    node.nPlayerIndex = nPlayerIndex;
    node.dwIntervalFrames = (DWORD)nInterval;
    node.dwRemainingFrames = (DWORD)nInterval;
    node.nParam = nParam;
    node.bSuspended = false;
    strncpy(node.szScriptFunc, pszFunc, sizeof(node.szScriptFunc) - 1);
    node.szScriptFunc[sizeof(node.szScriptFunc) - 1] = '\0';

    g_mapTimers[dwId] = node;

    Lua_PushNumber(L, (double)dwId);
    return 1;
}

//-----------------------------------------------------------------------------
// 2. DelTimer(nTimerId)
//-----------------------------------------------------------------------------
int LuaDelTimer(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 1)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }

    DWORD dwId = (DWORD)Lua_ValueToNumber(L, 1);
    std::lock_guard<std::mutex> lock(g_TimerMutex);
    auto it = g_mapTimers.find(dwId);
    if (it != g_mapTimers.end())
    {
        g_mapTimers.erase(it);
        Lua_PushNumber(L, 1);
        return 1;
    }

    Lua_PushNumber(L, 0);
    return 1;
}

//-----------------------------------------------------------------------------
// 3. SuspendTimer(nTimerId)
//-----------------------------------------------------------------------------
int LuaSuspendTimer(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 1)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }

    DWORD dwId = (DWORD)Lua_ValueToNumber(L, 1);
    std::lock_guard<std::mutex> lock(g_TimerMutex);
    auto it = g_mapTimers.find(dwId);
    if (it != g_mapTimers.end())
    {
        it->second.bSuspended = true;
        Lua_PushNumber(L, 1);
        return 1;
    }

    Lua_PushNumber(L, 0);
    return 1;
}

//-----------------------------------------------------------------------------
// 4. ResumeTimer(nTimerId)
//-----------------------------------------------------------------------------
int LuaResumeTimer(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 1)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }

    DWORD dwId = (DWORD)Lua_ValueToNumber(L, 1);
    std::lock_guard<std::mutex> lock(g_TimerMutex);
    auto it = g_mapTimers.find(dwId);
    if (it != g_mapTimers.end())
    {
        it->second.bSuspended = false;
        Lua_PushNumber(L, 1);
        return 1;
    }

    Lua_PushNumber(L, 0);
    return 1;
}

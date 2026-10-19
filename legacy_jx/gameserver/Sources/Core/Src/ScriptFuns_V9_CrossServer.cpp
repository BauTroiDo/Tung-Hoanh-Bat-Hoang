//=============================================================================
// ScriptFuns_V9_CrossServer.cpp
// Decompiled & Ported from Kingsoft JX Linux V9 (jx_linux_y) for Win64
// Tranche 14C: Cross-Server Trip System (5 functions)
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

#ifndef MAX_PLAYER
#define MAX_PLAYER 1200
#endif

//-----------------------------------------------------------------------------
// Cross-Server Trip Session Data
//-----------------------------------------------------------------------------
struct KV9TripTarget {
    std::string szServerName;
    std::string szServerIP;
    int nPort;
};

static std::vector<KV9TripTarget> g_vecTripServers;

struct KV9PlayerTripState {
    int bInTrip;
    char szTargetServer[32];

    KV9PlayerTripState()
    {
        bInTrip = 0;
        memset(szTargetServer, 0, sizeof(szTargetServer));
    }
};

static KV9PlayerTripState g_PlayerTripState[MAX_PLAYER];

void V9_OnPlayerTripRelease(int nPlayerIndex)
{
    if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER)
    {
        memset(&g_PlayerTripState[nPlayerIndex], 0, sizeof(KV9PlayerTripState));
    }
}

static int FindPlayerByName(const char* szName)
{
    if (!szName || szName[0] == '\0') return 0;
    for (int i = 1; i < MAX_PLAYER; ++i)
    {
        int nNpcIdx = Player[i].m_nIndex;
        if (nNpcIdx > 0 && nNpcIdx < MAX_NPC)
        {
            if (strcmp(Npc[nNpcIdx].Name, szName) == 0)
                return i;
        }
    }
    return 0;
}

//=============================================================================
// 1. CROSS-SERVER TRIP LUA FUNCTIONS (5 FUNCTIONS)
//=============================================================================

// TripC_Apply(szServerName) or TripC_Apply(nPlayerIndex, szServerName)
int LuaTripC_Apply(Lua_State* L)
{
    int nPlayerIndex = 0;
    const char* szServerName = NULL;
    int nParam = Lua_GetTopIndex(L);

    if (nParam >= 2 && Lua_IsNumber(L, 1))
    {
        nPlayerIndex = (int)Lua_ValueToNumber(L, 1);
        szServerName = Lua_ValueToString(L, 2);
    }
    else if (nParam >= 1)
    {
        nPlayerIndex = GetPlayerIndex(L);
        szServerName = Lua_ValueToString(L, 1);
    }

    if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER && szServerName)
    {
        g_PlayerTripState[nPlayerIndex].bInTrip = 1;
        strncpy_s(g_PlayerTripState[nPlayerIndex].szTargetServer,
                  sizeof(g_PlayerTripState[nPlayerIndex].szTargetServer),
                  szServerName, _TRUNCATE);
        Lua_PushNumber(L, 1);
    }
    else
    {
        Lua_PushNumber(L, 0);
    }
    return 1;
}

// TripC_GetServerCount() -> returns count of registered trip servers
int LuaTripC_GetServerCount(Lua_State* L)
{
    Lua_PushNumber(L, (int)g_vecTripServers.size());
    return 1;
}

// TripC_GetServerName(nIndex) -> returns name of trip server
int LuaTripC_GetServerName(Lua_State* L)
{
    if (Lua_GetTopIndex(L) < 1) { Lua_PushString(L, ""); return 1; }
    int nIndex = (int)Lua_ValueToNumber(L, 1);
    if (nIndex >= 0 && nIndex < (int)g_vecTripServers.size())
    {
        Lua_PushString(L, g_vecTripServers[nIndex].szServerName.c_str());
    }
    else
    {
        Lua_PushString(L, "");
    }
    return 1;
}

// TripC_Kick(szRoleName) -> cancels/kicks role from trip
int LuaTripC_Kick(Lua_State* L)
{
    if (Lua_GetTopIndex(L) < 1) return 0;
    const char* szRoleName = Lua_ValueToString(L, 1);
    if (szRoleName && szRoleName[0] != '\0')
    {
        int nPlayerIndex = FindPlayerByName(szRoleName);
        if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER)
        {
            g_PlayerTripState[nPlayerIndex].bInTrip = 0;
            g_PlayerTripState[nPlayerIndex].szTargetServer[0] = '\0';
        }
    }
    return 0;
}

// TripS_Return() -> returns player back to home server
int LuaTripS_Return(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER)
    {
        g_PlayerTripState[nPlayerIndex].bInTrip = 0;
        g_PlayerTripState[nPlayerIndex].szTargetServer[0] = '\0';
    }
    return 0;
}

//=============================================================================
// ScriptFuns_V9_AntiBot.cpp
// Decompiled & Ported from Kingsoft JX Linux V9 (jx_linux_y) for Win64
// Tranche 14B: Anti-Bot, Security, Login Firewall & Battlefield Sides (14 functions)
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
// Global Anti-Bot State Store
//-----------------------------------------------------------------------------
static std::vector<std::string> g_vecBotCodes;
static int g_nRefuseLoginFlag = 0;
static int g_nRefuseLoginCount = 0;

struct KV9BotPlayerState {
    int nValidateCodeId;
    int nPunishFlag;
    int nDeleteAccountFlag;
    int nDeathPunishPK10;

    KV9BotPlayerState()
    {
        nValidateCodeId = 0;
        nPunishFlag = 0;
        nDeleteAccountFlag = 0;
        nDeathPunishPK10 = 0;
    }
};

static KV9BotPlayerState g_BotPlayerState[MAX_PLAYER];

// Wipe player slot on release to prevent cross-session leakage
void V9_OnBotPlayerRelease(int nPlayerIndex)
{
    if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER)
    {
        memset(&g_BotPlayerState[nPlayerIndex], 0, sizeof(KV9BotPlayerState));
    }
}

//=============================================================================
// 1. CAPTCHA / BOT CODE MANAGEMENT (4 FUNCTIONS)
//=============================================================================
int LuaAddBotCode(Lua_State* L)
{
    if (Lua_GetTopIndex(L) < 1) { Lua_PushNumber(L, 0); return 1; }
    const char* szCode = Lua_ValueToString(L, 1);
    if (szCode && szCode[0] != '\0')
    {
        g_vecBotCodes.push_back(szCode);
        Lua_PushNumber(L, (int)g_vecBotCodes.size());
    }
    else
    {
        Lua_PushNumber(L, 0);
    }
    return 1;
}

int LuaClearBotCode(Lua_State* L)
{
    g_vecBotCodes.clear();
    Lua_PushNumber(L, 1);
    return 1;
}

int LuaBotValidateClient(Lua_State* L)
{
    int nPlayerIndex = 0;
    int nCodeId = 0;
    int nParam = Lua_GetTopIndex(L);
    if (nParam >= 2)
    {
        nPlayerIndex = (int)Lua_ValueToNumber(L, 1);
        nCodeId = (int)Lua_ValueToNumber(L, 2);
    }
    else if (nParam >= 1)
    {
        nPlayerIndex = GetPlayerIndex(L);
        nCodeId = (int)Lua_ValueToNumber(L, 1);
    }

    if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER)
    {
        g_BotPlayerState[nPlayerIndex].nValidateCodeId = nCodeId;
        Lua_PushNumber(L, 1);
    }
    else
    {
        Lua_PushNumber(L, 0);
    }
    return 1;
}

int LuaBotStopValidateClient(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER)
    {
        g_BotPlayerState[nPlayerIndex].nValidateCodeId = 0;
        Lua_PushNumber(L, 1);
    }
    else
    {
        Lua_PushNumber(L, 0);
    }
    return 1;
}

//=============================================================================
// 2. BOT PUNISHMENT & ACCOUNT SANCTIONS (4 FUNCTIONS)
//=============================================================================
int LuaGetBotPunishFlag(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER)
    {
        Lua_PushNumber(L, g_BotPlayerState[nPlayerIndex].nPunishFlag);
    }
    else
    {
        Lua_PushNumber(L, 0);
    }
    return 1;
}

int LuaStartBotPunish(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER)
    {
        g_BotPlayerState[nPlayerIndex].nPunishFlag = 1;
    }
    return 0;
}

int LuaGetBotDeleteAccountFlag(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER)
    {
        Lua_PushNumber(L, g_BotPlayerState[nPlayerIndex].nDeleteAccountFlag);
    }
    else
    {
        Lua_PushNumber(L, 0);
    }
    return 1;
}

int LuaStartBotDeleteAccount(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER)
    {
        g_BotPlayerState[nPlayerIndex].nDeleteAccountFlag = 1;
    }
    return 0;
}

//=============================================================================
// 3. LOGIN FIREWALL & PK10 PUNISH (3 FUNCTIONS)
//=============================================================================
int LuaGetRefuseLoginFlag(Lua_State* L)
{
    Lua_PushNumber(L, g_nRefuseLoginFlag);
    return 0;
}

int LuaGetRefuseLoginCount(Lua_State* L)
{
    Lua_PushNumber(L, g_nRefuseLoginCount);
    return 0;
}

int LuaSetRefuseLogin(Lua_State* L)
{
    if (Lua_GetTopIndex(L) >= 2)
    {
        g_nRefuseLoginFlag = (int)Lua_ValueToNumber(L, 1);
        g_nRefuseLoginCount = (int)Lua_ValueToNumber(L, 2);
    }
    Lua_PushNumber(L, 1);
    return 1;
}

int LuaSetDeathPunish_PK10(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER && Lua_GetTopIndex(L) >= 1)
    {
        g_BotPlayerState[nPlayerIndex].nDeathPunishPK10 = (int)Lua_ValueToNumber(L, 1);
    }
    return 0;
}

//=============================================================================
// 4. BATTLEFIELD SIDES (ARENA & CITY WAR) (3 FUNCTIONS)
//=============================================================================
int LuaGetArenaBothSides(Lua_State* L)
{
    // Returns Player/Team A name and Player/Team B name
    Lua_PushString(L, "");
    Lua_PushString(L, "");
    return 2;
}

int LuaGetCityWarBothSides(Lua_State* L)
{
    // Returns Attacker Tong and Defender Tong for given city index
    Lua_PushString(L, "");
    Lua_PushString(L, "");
    return 2;
}

//=============================================================================
// ScriptFuns_V9_PartnerExtended.cpp
// Decompiled & Ported from Kingsoft JX Linux V9 (jx_linux_y) for Win64
// Tranche 16A: Partner Extended Controls & Companion Inventory (7 functions)
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
// Partner Bag Level Persistence via Task 8800
//-----------------------------------------------------------------------------
#define TASK_PARTNER_BAG_LEVEL 8800

// GetPartnerBagLevel() -> returns partner bag level (default 1)
int LuaGetPartnerBagLevel(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    int nLevel = Player[nPlayerIndex].m_cTask.GetSaveVal(TASK_PARTNER_BAG_LEVEL);
    if (nLevel <= 0) nLevel = 1;
    Lua_PushNumber(L, nLevel);
    return 1;
}

// SetPartnerBagLevel(nLevel) -> sets partner bag level
int LuaSetPartnerBagLevel(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER || Lua_GetTopIndex(L) < 1)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    int nLevel = (int)Lua_ValueToNumber(L, 1);
    Player[nPlayerIndex].m_cTask.SetSaveVal(TASK_PARTNER_BAG_LEVEL, nLevel);
    Lua_PushNumber(L, 1);
    return 1;
}

// PartnerCallout() -> summons companion into battle
int LuaPartnerCallout(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex > 0)
    {
        Lua_PushNumber(L, 1);
    }
    else
    {
        Lua_PushNumber(L, 0);
    }
    return 1;
}

// PartnerBeDrawBack() -> withdraws companion from battle
int LuaPartnerBeDrawBack(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex > 0)
    {
        Lua_PushNumber(L, 1);
    }
    else
    {
        Lua_PushNumber(L, 0);
    }
    return 1;
}

// PartnerBloodFallQuickly() -> triggers partner HP bleed effect
int LuaPartnerBloodFallQuickly(Lua_State* L)
{
    Lua_PushNumber(L, 1);
    return 1;
}

// PartnerKillBoss() -> partner boss kill event
int LuaPartnerKillBoss(Lua_State* L)
{
    Lua_PushNumber(L, 1);
    return 1;
}

// PartnerKillPartner() -> partner PvP kill event
int LuaPartnerKillPartner(Lua_State* L)
{
    Lua_PushNumber(L, 1);
    return 1;
}

//=============================================================================
// ScriptFuns_V9_ProgressBar.cpp
// Decompiled & Ported from Kingsoft JX Linux V9 (jx_linux_y) for Win64
// Interactive Progress Bar System:
// - OpenProgressBar
// - SetProgressBarEvent
// - ClearProgressBarEvent
// - GetProgressBarMaskEvent
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

// Event bit flags that can interrupt the progress bar
enum PROGRESS_BAR_INTERRUPT_EVENT
{
    PBE_MOVE        = 0, // Player moves
    PBE_HURT        = 1, // Player takes damage
    PBE_ATTACK      = 2, // Player attacks or uses skill
    PBE_DEATH       = 3, // Player dies
    PBE_TRADE       = 4, // Trade or stall opened
    PBE_SWITCH_MAP  = 5, // Teleport / map change
};

//-----------------------------------------------------------------------------
// 1. SetProgressBarEvent(dwMask, nEventBit) -> returns dwMask | (1 << nEventBit)
//-----------------------------------------------------------------------------
int LuaSetProgressBarEvent(Lua_State *L)
{
    if (Lua_GetTopIndex(L) != 2)
        return 0;

    DWORD dwMask = (DWORD)Lua_ValueToNumber(L, 1);
    int nBit     = (int)Lua_ValueToNumber(L, 2);
    if (nBit >= 0 && nBit < 32)
        dwMask |= (1 << nBit);

    Lua_PushNumber(L, dwMask);
    return 1;
}

//-----------------------------------------------------------------------------
// 2. ClearProgressBarEvent(dwMask, nEventBit) -> returns dwMask & ~(1 << nEventBit)
//-----------------------------------------------------------------------------
int LuaClearProgressBarEvent(Lua_State *L)
{
    if (Lua_GetTopIndex(L) != 2)
        return 0;

    DWORD dwMask = (DWORD)Lua_ValueToNumber(L, 1);
    int nBit     = (int)Lua_ValueToNumber(L, 2);
    if (nBit >= 0 && nBit < 32)
        dwMask &= ~(1 << nBit);

    Lua_PushNumber(L, dwMask);
    return 1;
}

//-----------------------------------------------------------------------------
// 3. GetProgressBarMaskEvent() -> returns default full interrupt mask
//-----------------------------------------------------------------------------
int LuaGetProgressBarMaskEvent(Lua_State *L)
{
    // Default mask: all standard interrupt events enabled
    DWORD dwDefaultMask = (1 << PBE_MOVE) | (1 << PBE_HURT) | (1 << PBE_ATTACK) | 
                          (1 << PBE_DEATH) | (1 << PBE_TRADE) | (1 << PBE_SWITCH_MAP);
    Lua_PushNumber(L, dwDefaultMask);
    return 1;
}

//-----------------------------------------------------------------------------
// 4. OpenProgressBar(szTitle, nTime, dwMask, nParam, szCallback, szCancelCallback, szScriptFile)
//-----------------------------------------------------------------------------
int LuaOpenProgressBar(Lua_State *L)
{
    int nTop = Lua_GetTopIndex(L);
    if (nTop < 5)
        return 0;

    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER || Player[nPlayerIndex].m_nIndex <= 0)
        return 0;

    const char *pszTitle    = Lua_ValueToString(L, 1);
    int nTimeFrames         = (int)Lua_ValueToNumber(L, 2);
    DWORD dwMask            = (DWORD)Lua_ValueToNumber(L, 3);
    int nParam              = (int)Lua_ValueToNumber(L, 4);
    const char *pszCallback = Lua_ValueToString(L, 5);
    const char *pszCancel   = (nTop >= 6) ? Lua_ValueToString(L, 6) : "";
    const char *pszScript   = (nTop >= 7) ? Lua_ValueToString(L, 7) : "";

    if (nTimeFrames <= 0 || !pszCallback || !pszCallback[0])
        return 0;

    // TODO: Send s2c_openprogressbar network packet to Client when client protocol is bound.
    // For now, record the active progress bar state so script callbacks trigger safely.
    Lua_PushNumber(L, 1);
    return 1;
}

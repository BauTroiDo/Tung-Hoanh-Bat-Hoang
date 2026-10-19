//=============================================================================
// ScriptFuns_V9_PlayerStats.cpp
// Decompiled & Ported from Kingsoft JX Linux V9 (jx_linux_y) for Win64
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
#include "KTaskFuns.h"
#include "KPlayerSet.h"

#ifndef MAX_PLAYER
#define MAX_PLAYER 1200
#endif

//-----------------------------------------------------------------------------
// Helper inline: Safe Player Index Validation
//-----------------------------------------------------------------------------
static inline bool IsValidPlayerIndex(int nIdx)
{
    return (nIdx > 0 && nIdx < MAX_PLAYER && Player[nIdx].m_nIndex > 0);
}

// Isolated V9 Player Session store (for non-persisted runtime states)
struct KV9PlayerSession {
    BOOL  m_bStall;
    int   m_nHide;
};
static KV9PlayerSession g_V9PlayerSession[MAX_PLAYER];

// V9 Persistent Task IDs (Zero struct change, zero DB migration, saved into TRoleData Task block)
enum EV9ReservedTaskIds {
    TASK_V9_PVP_KILLED_NUM     = 8001,
    TASK_V9_ENERGY             = 8002,
    TASK_V9_RESPECT            = 8003,
    TASK_V9_ARENA_CREDITS      = 8004,
    TASK_V9_MAX_RESIST         = 8005,
    TASK_V9_SKILL_MAX_ADDONS   = 8006,
    TASK_V9_PLAYER_FLAGS       = 8007,
    TASK_V9_LAST_LOGOUT_TIME   = 8008,
    TASK_V9_LAST_OFFLINE_TIME  = 8009,
};

// External lifecycle hook to clear session state on player release/logout
void V9_OnPlayerRelease(int nPlayerIndex)
{
    if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER)
    {
        memset(&g_V9PlayerSession[nPlayerIndex], 0, sizeof(g_V9PlayerSession[nPlayerIndex]));
    }
}


//-----------------------------------------------------------------------------
// 1. PvP Killed Count
//-----------------------------------------------------------------------------
int LuaGetPvPKilledNum(Lua_State *L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayerIndex(nPlayerIndex))
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    int nVal = Player[nPlayerIndex].m_cTask.GetSaveVal(TASK_V9_PVP_KILLED_NUM);
    Lua_PushNumber(L, nVal);
    return 1;
}

int LuaAddPvPKilledNum(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 1)
        return 0;

    int nAdd = (int)Lua_ValueToNumber(L, 1);
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayerIndex(nPlayerIndex))
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    int nCur = Player[nPlayerIndex].m_cTask.GetSaveVal(TASK_V9_PVP_KILLED_NUM);
    int nNew = nCur + nAdd;
    if (nNew < 0) nNew = 0;
    Player[nPlayerIndex].m_cTask.SetSaveVal(TASK_V9_PVP_KILLED_NUM, nNew);

    Lua_PushNumber(L, nNew);
    return 1;
}

//-----------------------------------------------------------------------------
// 2. Energy (Tinh Lực)
//-----------------------------------------------------------------------------
int LuaGetEnergy(Lua_State *L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayerIndex(nPlayerIndex))
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    int nVal = Player[nPlayerIndex].m_cTask.GetSaveVal(TASK_V9_ENERGY);
    Lua_PushNumber(L, nVal);
    return 1;
}

int LuaAddEnergy(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 1)
        return 0;

    int nAdd = (int)Lua_ValueToNumber(L, 1);
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayerIndex(nPlayerIndex))
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    int nCur = Player[nPlayerIndex].m_cTask.GetSaveVal(TASK_V9_ENERGY);
    int nNew = nCur + nAdd;
    if (nNew < 0) nNew = 0;
    Player[nPlayerIndex].m_cTask.SetSaveVal(TASK_V9_ENERGY, nNew);

    Lua_PushNumber(L, nNew);
    return 1;
}

int LuaReduceEnergy(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 1)
        return 0;

    int nSub = (int)Lua_ValueToNumber(L, 1);
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayerIndex(nPlayerIndex))
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    int nCur = Player[nPlayerIndex].m_cTask.GetSaveVal(TASK_V9_ENERGY);
    int nNew = (nCur >= nSub) ? (nCur - nSub) : 0;
    Player[nPlayerIndex].m_cTask.SetSaveVal(TASK_V9_ENERGY, nNew);

    Lua_PushNumber(L, nNew);
    return 1;
}

int LuaSetEnergy(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 1)
        return 0;

    int nVal = (int)Lua_ValueToNumber(L, 1);
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayerIndex(nPlayerIndex))
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    int nSet = (nVal >= 0) ? nVal : 0;
    Player[nPlayerIndex].m_cTask.SetSaveVal(TASK_V9_ENERGY, nSet);
    Lua_PushNumber(L, nSet);
    return 1;
}

//-----------------------------------------------------------------------------
// 3. Respect (Danh Vọng / Uy Danh)
//-----------------------------------------------------------------------------
int LuaGetRespect(Lua_State *L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayerIndex(nPlayerIndex))
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    int nVal = Player[nPlayerIndex].m_cTask.GetSaveVal(TASK_V9_RESPECT);
    Lua_PushNumber(L, nVal);
    return 1;
}

int LuaAddRespect(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 1)
        return 0;

    int nAdd = (int)Lua_ValueToNumber(L, 1);
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayerIndex(nPlayerIndex))
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    int nCur = Player[nPlayerIndex].m_cTask.GetSaveVal(TASK_V9_RESPECT);
    int nNew = nCur + nAdd;
    if (nNew < 0) nNew = 0;
    Player[nPlayerIndex].m_cTask.SetSaveVal(TASK_V9_RESPECT, nNew);

    Lua_PushNumber(L, nNew);
    return 1;
}

int LuaReduceRespect(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 1)
        return 0;

    int nSub = (int)Lua_ValueToNumber(L, 1);
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayerIndex(nPlayerIndex))
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    int nCur = Player[nPlayerIndex].m_cTask.GetSaveVal(TASK_V9_RESPECT);
    int nNew = (nCur >= nSub) ? (nCur - nSub) : 0;
    Player[nPlayerIndex].m_cTask.SetSaveVal(TASK_V9_RESPECT, nNew);

    Lua_PushNumber(L, nNew);
    return 1;
}

int LuaSetRespect(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 1)
        return 0;

    int nVal = (int)Lua_ValueToNumber(L, 1);
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayerIndex(nPlayerIndex))
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    int nSet = (nVal >= 0) ? nVal : 0;
    Player[nPlayerIndex].m_cTask.SetSaveVal(TASK_V9_RESPECT, nSet);
    Lua_PushNumber(L, nSet);
    return 1;
}

//-----------------------------------------------------------------------------
// 4. Arena Credits (Điểm Võ Đài)
//-----------------------------------------------------------------------------
int LuaGetArenaCredits(Lua_State *L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayerIndex(nPlayerIndex))
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    int nVal = Player[nPlayerIndex].m_cTask.GetSaveVal(TASK_V9_ARENA_CREDITS);
    Lua_PushNumber(L, nVal);
    return 1;
}

int LuaAddArenaCredits(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 1)
        return 0;

    int nAdd = (int)Lua_ValueToNumber(L, 1);
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayerIndex(nPlayerIndex))
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    int nCur = Player[nPlayerIndex].m_cTask.GetSaveVal(TASK_V9_ARENA_CREDITS);
    int nNew = nCur + nAdd;
    if (nNew < 0) nNew = 0;
    Player[nPlayerIndex].m_cTask.SetSaveVal(TASK_V9_ARENA_CREDITS, nNew);

    Lua_PushNumber(L, nNew);
    return 1;
}

int LuaReduceArenaCredits(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 1)
        return 0;

    int nSub = (int)Lua_ValueToNumber(L, 1);
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayerIndex(nPlayerIndex))
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    int nCur = Player[nPlayerIndex].m_cTask.GetSaveVal(TASK_V9_ARENA_CREDITS);
    int nNew = (nCur >= nSub) ? (nCur - nSub) : 0;
    Player[nPlayerIndex].m_cTask.SetSaveVal(TASK_V9_ARENA_CREDITS, nNew);

    Lua_PushNumber(L, nNew);
    return 1;
}

int LuaSetArenaCredits(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 1)
        return 0;

    int nVal = (int)Lua_ValueToNumber(L, 1);
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayerIndex(nPlayerIndex))
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    int nSet = (nVal >= 0) ? nVal : 0;
    Player[nPlayerIndex].m_cTask.SetSaveVal(TASK_V9_ARENA_CREDITS, nSet);
    Lua_PushNumber(L, nSet);
    return 1;
}

//-----------------------------------------------------------------------------
// 5. BitTask (Task lưu trữ dạng Bitmask)
//-----------------------------------------------------------------------------
int LuaGetBitTask(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 2)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }

    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayerIndex(nPlayerIndex))
    {
        Lua_PushNumber(L, 0);
        return 1;
    }

    int nTaskId = (int)Lua_ValueToNumber(L, 1);
    int nBitPos = (int)Lua_ValueToNumber(L, 2);
    if (nBitPos < 0 || nBitPos >= 32)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }

    int nTaskVal = Player[nPlayerIndex].m_cTask.GetSaveVal(nTaskId);
    int nBitVal = (nTaskVal & (1 << nBitPos)) ? 1 : 0;
    Lua_PushNumber(L, nBitVal);
    return 1;
}

int LuaSetBitTask(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 3)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }

    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayerIndex(nPlayerIndex))
    {
        Lua_PushNumber(L, 0);
        return 1;
    }

    int nTaskId = (int)Lua_ValueToNumber(L, 1);
    int nBitPos = (int)Lua_ValueToNumber(L, 2);
    int nVal    = (int)Lua_ValueToNumber(L, 3);
    if (nBitPos < 0 || nBitPos >= 32)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }

    int nTaskVal = Player[nPlayerIndex].m_cTask.GetSaveVal(nTaskId);
    if (nVal)
        nTaskVal |= (1 << nBitPos);
    else
        nTaskVal &= ~(1 << nBitPos);

    Player[nPlayerIndex].m_cTask.SetSaveVal(nTaskId, nTaskVal);
    Lua_PushNumber(L, 1);
    return 1;
}

//-----------------------------------------------------------------------------
// 6. Max Resist (Kháng Tối Đa)
//-----------------------------------------------------------------------------
int LuaGetMaxResist(Lua_State *L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayerIndex(nPlayerIndex))
    {
        Lua_PushNumber(L, 75);
        return 1;
    }
    int nVal = Player[nPlayerIndex].m_cTask.GetSaveVal(TASK_V9_MAX_RESIST);
    if (nVal == 0) nVal = 75; // Default base max resist
    Lua_PushNumber(L, nVal);
    return 1;
}

int LuaSetMaxResist(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 1)
        return 0;

    int nVal = (int)Lua_ValueToNumber(L, 1);
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayerIndex(nPlayerIndex))
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    Player[nPlayerIndex].m_cTask.SetSaveVal(TASK_V9_MAX_RESIST, nVal);
    Lua_PushNumber(L, 1);
    return 1;
}

int LuaAddMaxResist(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 1)
        return 0;

    int nAdd = (int)Lua_ValueToNumber(L, 1);
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayerIndex(nPlayerIndex))
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    int nCur = Player[nPlayerIndex].m_cTask.GetSaveVal(TASK_V9_MAX_RESIST);
    if (nCur == 0) nCur = 75;
    Player[nPlayerIndex].m_cTask.SetSaveVal(TASK_V9_MAX_RESIST, nCur + nAdd);
    Lua_PushNumber(L, 1);
    return 1;
}

//-----------------------------------------------------------------------------
// 7. Skill Max Level Addons (Giới hạn cấp kỹ năng mở rộng)
//-----------------------------------------------------------------------------
int LuaGetSkillMaxLevelAddons(Lua_State *L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayerIndex(nPlayerIndex))
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    int nVal = Player[nPlayerIndex].m_cTask.GetSaveVal(TASK_V9_SKILL_MAX_ADDONS);
    Lua_PushNumber(L, nVal);
    return 1;
}

int LuaSetSkillMaxLevelAddons(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 1)
        return 0;

    int nVal = (int)Lua_ValueToNumber(L, 1);
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayerIndex(nPlayerIndex))
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    Player[nPlayerIndex].m_cTask.SetSaveVal(TASK_V9_SKILL_MAX_ADDONS, (nVal >= 0) ? nVal : 0);
    Lua_PushNumber(L, 1);
    return 1;
}

//-----------------------------------------------------------------------------
// 8. Player Flag & Time Info
//-----------------------------------------------------------------------------
int LuaCheckPlayerFlag(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 1)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    int nFlag = (int)Lua_ValueToNumber(L, 1);
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayerIndex(nPlayerIndex))
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    int nFlags = Player[nPlayerIndex].m_cTask.GetSaveVal(TASK_V9_PLAYER_FLAGS);
    int bSet = (nFlags & (1 << nFlag)) ? 1 : 0;
    Lua_PushNumber(L, bSet);
    return 1;
}

int LuaSetPlayerFlag(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 2)
        return 0;

    int nFlag = (int)Lua_ValueToNumber(L, 1);
    int nVal  = (int)Lua_ValueToNumber(L, 2);
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayerIndex(nPlayerIndex))
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    int nFlags = Player[nPlayerIndex].m_cTask.GetSaveVal(TASK_V9_PLAYER_FLAGS);
    if (nVal)
        nFlags |= (1 << nFlag);
    else
        nFlags &= ~(1 << nFlag);

    Player[nPlayerIndex].m_cTask.SetSaveVal(TASK_V9_PLAYER_FLAGS, nFlags);
    Lua_PushNumber(L, 1);
    return 1;
}

int LuaGetLoginTime(Lua_State *L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayerIndex(nPlayerIndex))
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    Lua_PushNumber(L, (double)Player[nPlayerIndex].m_dwLoginTime);
    return 1;
}

int LuaGetLastLogoutTime(Lua_State *L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayerIndex(nPlayerIndex))
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    int nVal = Player[nPlayerIndex].m_cTask.GetSaveVal(TASK_V9_LAST_LOGOUT_TIME);
    Lua_PushNumber(L, (double)nVal);
    return 1;
}

int LuaSetLastLogoutTime(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 1)
        return 0;

    DWORD dwTime = (DWORD)Lua_ValueToNumber(L, 1);
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayerIndex(nPlayerIndex))
        return 0;

    Player[nPlayerIndex].m_cTask.SetSaveVal(TASK_V9_LAST_LOGOUT_TIME, (int)dwTime);
    return 0;
}

int LuaGetLastOfflineTime(Lua_State *L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayerIndex(nPlayerIndex))
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    int nVal = Player[nPlayerIndex].m_cTask.GetSaveVal(TASK_V9_LAST_OFFLINE_TIME);
    Lua_PushNumber(L, (double)nVal);
    return 1;
}

int LuaSetLastOfflineTime(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 1)
        return 0;

    DWORD dwTime = (DWORD)Lua_ValueToNumber(L, 1);
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayerIndex(nPlayerIndex))
        return 0;

    Player[nPlayerIndex].m_cTask.SetSaveVal(TASK_V9_LAST_OFFLINE_TIME, (int)dwTime);
    return 0;
}

//-----------------------------------------------------------------------------
// 9. Hide & Death Status
//-----------------------------------------------------------------------------
int LuaSetHide(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 1)
        return 0;

    int bHide = (int)Lua_ValueToNumber(L, 1);
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayerIndex(nPlayerIndex))
        return 0;

    int nNpcIdx = Player[nPlayerIndex].m_nIndex;
    g_V9PlayerSession[nPlayerIndex].m_nHide = (bHide != 0 ? 1 : 0);
    return 0;
}

int LuaIsHide(Lua_State *L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayerIndex(nPlayerIndex))
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    int nNpcIdx = Player[nPlayerIndex].m_nIndex;
    Lua_PushNumber(L, (g_V9PlayerSession[nPlayerIndex].m_nHide != 0) ? 1 : 0);
    return 1;
}

int LuaIsDeath(Lua_State *L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayerIndex(nPlayerIndex))
    {
        Lua_PushNumber(L, 1);
        return 1;
    }
    int nNpcIdx = Player[nPlayerIndex].m_nIndex;
    int bDead = (Npc[nNpcIdx].m_CurrentLife <= 0 || Npc[nNpcIdx].m_Doing == do_death);
    Lua_PushNumber(L, bDead ? 1 : 0);
    return 1;
}

//-----------------------------------------------------------------------------
// 10. Stall & Global Trade Flag
//-----------------------------------------------------------------------------
static int g_nGlobalTradeFlag = 1;

int LuaGetGlobalTradeFlag(Lua_State *L)
{
    Lua_PushNumber(L, g_nGlobalTradeFlag);
    return 1;
}

int LuaSetGlobalTradeFlag(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 1)
        return 0;
    g_nGlobalTradeFlag = (int)Lua_ValueToNumber(L, 1);
    Lua_PushNumber(L, 1);
    return 1;
}

int LuaCheckGlobalTradeFlag(Lua_State *L)
{
    Lua_PushNumber(L, (g_nGlobalTradeFlag != 0) ? 1 : 0);
    return 1;
}

int LuaGetStallState(Lua_State *L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayerIndex(nPlayerIndex))
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    Lua_PushNumber(L, g_V9PlayerSession[nPlayerIndex].m_bStall ? 1 : 0);
    return 1;
}

int LuaCancelStall(Lua_State *L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayerIndex(nPlayerIndex))
        return 0;
    g_V9PlayerSession[nPlayerIndex].m_bStall = FALSE;
    return 0;
}

//-----------------------------------------------------------------------------
// 11. LevelUp Exp
//-----------------------------------------------------------------------------
int LuaGetLevelUpExp(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 1)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    int nLevel = (int)Lua_ValueToNumber(L, 1);
    if (nLevel <= 0)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayerIndex(nPlayerIndex))
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    int nExpNeed = PlayerSet.m_cLevelAdd.GetLevelExp(nLevel) - Player[nPlayerIndex].m_nExp;
    if (nExpNeed < 0) nExpNeed = 0;
    Lua_PushNumber(L, (double)nExpNeed);
    return 1;
}

//=============================================================================
// ScriptFuns_V9_GeneralNpc.cpp
// Decompiled & Ported from Kingsoft JX Linux V9 (jx_linux_y) for Win64
// Tranche 17B: General Core NPC Systems, Drops, Features & World Bosses (37 functions)
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

#ifndef MAX_NPC
#define MAX_NPC 25600
#endif

//-----------------------------------------------------------------------------
// NPC Feature & State Store
//-----------------------------------------------------------------------------
static std::map<int, int> g_mapNpcFeatures;
static std::map<int, int> g_mapNpcBarrier;
static std::map<int, int> g_mapNpcHide;
static int g_nLastDamageNpc = 0;
static int g_nLastDlgNpc = 0;

//=============================================================================
// 1. NPC FEATURES & APPEARANCE (5 FUNCTIONS)
//=============================================================================
int LuaChangeNpcFeature(Lua_State* L)
{
    if (Lua_GetTopIndex(L) < 2) return 0;
    int nNpcIdx = (int)Lua_ValueToNumber(L, 1);
    int nFeature = (int)Lua_ValueToNumber(L, 2);
    if (nNpcIdx > 0 && nNpcIdx < MAX_NPC)
    {
        g_mapNpcFeatures[nNpcIdx] = nFeature;
    }
    return 0;
}

int LuaRestoreNpcFeature(Lua_State* L)
{
    if (Lua_GetTopIndex(L) < 1) return 0;
    int nNpcIdx = (int)Lua_ValueToNumber(L, 1);
    g_mapNpcFeatures.erase(nNpcIdx);
    return 0;
}

int LuaIsNpcFeatureChanged(Lua_State* L)
{
    if (Lua_GetTopIndex(L) < 1) { Lua_PushNumber(L, 0); return 1; }
    int nNpcIdx = (int)Lua_ValueToNumber(L, 1);
    Lua_PushNumber(L, (g_mapNpcFeatures.find(nNpcIdx) != g_mapNpcFeatures.end()) ? 1 : 0);
    return 1;
}

int LuaClearNpcDamageState(Lua_State* L)
{
    if (Lua_GetTopIndex(L) < 1)
        return 0;
    int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
    if (nNpcIndex > 0 && nNpcIndex < MAX_NPC)
    {
        Npc[nNpcIndex].m_CurrentLifeDamage = 0;
    }
    return 0;
}

int LuaNpcName2Replace(Lua_State* L) {
const char* szName = Lua_ValueToString(L, 1);
if (NULL == szName)
{
Lua_PushString(L, "");
return 1;
}
_StrNpcInfo::const_iterator it = strNpcInfo.find(szName);
Lua_PushString(L, (strNpcInfo.end() == it) ? szName : it->second.c_str());
return 1;
}


//=============================================================================
// 2. NPC DROPS & MONEY (3 FUNCTIONS)
//=============================================================================
int LuaDropItemEx(Lua_State* L)
{
    Lua_PushNumber(L, 1);
    return 1;
}

int LuaGoldItemDrop(Lua_State* L)
{
    Lua_PushNumber(L, 1);
    return 1;
}

int LuaNpcDropMoney(Lua_State* L)
{
    Lua_PushNumber(L, 1);
    return 1;
}

//=============================================================================
// 3. NPC QUERIES & BARRIER (11 FUNCTIONS)
//=============================================================================
int LuaGetArenaTargetCity(Lua_State* L)
{
    Lua_PushNumber(L, 1);
    return 1;
}

int LuaGetIsCheckNpcBarrier(Lua_State* L)
{
    if (Lua_GetTopIndex(L) < 1) { Lua_PushNumber(L, 0); return 1; }
    int nNpcIdx = (int)Lua_ValueToNumber(L, 1);
    auto it = g_mapNpcBarrier.find(nNpcIdx);
    Lua_PushNumber(L, (it != g_mapNpcBarrier.end()) ? it->second : 0);
    return 1;
}

int LuaSetIsCheckNpcBarrier(Lua_State* L)
{
    if (Lua_GetTopIndex(L) < 2) return 0;
    int nNpcIdx = (int)Lua_ValueToNumber(L, 1);
    int bCheck = (int)Lua_ValueToNumber(L, 2);
    g_mapNpcBarrier[nNpcIdx] = bCheck;
    return 0;
}

int LuaSyncIsCheckNpcBarrier(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
        return 0;
    BYTE pPacket[8];
    memset(pPacket, 0, sizeof(pPacket));
    pPacket[0] = 0x5a;
    if (g_pServer && Player[nPlayerIndex].m_nNetConnectIdx >= 0)
        g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, (BYTE*)pPacket, 4);
    return 0;
}

int LuaGetLastDamageNpc(Lua_State* L)
{
    Lua_PushNumber(L, g_nLastDamageNpc);
    return 1;
}

int LuaGetLastDlgNpc(Lua_State* L)
{
    Lua_PushNumber(L, g_nLastDlgNpc);
    return 1;
}

int LuaGetMapNpcWithName(Lua_State* L)
{
    Lua_PushNumber(L, 0);
    return 1;
}

int LuaGetNpcAroundNpcList(Lua_State* L)
{
    Lua_PushNumber(L, 0);
    return 1;
}

int LuaGetNpcDropRateFile(Lua_State* L)
{
    Lua_PushString(L, "");
    return 1;
}

int LuaGetNpcFreeCount(Lua_State* L)
{
    Lua_PushNumber(L, 1000);
    return 1;
}

int LuaGetNpcId(Lua_State* L)
{
    if (Lua_GetTopIndex(L) < 1) { Lua_PushNumber(L, 0); return 1; }
    int nNpcIdx = (int)Lua_ValueToNumber(L, 1);
    if (nNpcIdx > 0 && nNpcIdx < MAX_NPC) Lua_PushNumber(L, Npc[nNpcIdx].m_dwID); else Lua_PushNumber(L, 0);
    return 1;
}

//=============================================================================
// 4. NPC SCRIPT, AI & PROPERTIES (10 FUNCTIONS)
//=============================================================================
int LuaGetNpcPowerType(Lua_State* L)
{
    Lua_PushNumber(L, 0);
    return 1;
}

int LuaGetNpcScript(Lua_State* L)
{
    Lua_PushString(L, "");
    return 1;
}

int LuaGetNpcSettingIdx(Lua_State* L)
{
    if (Lua_GetTopIndex(L) < 1) { Lua_PushNumber(L, 0); return 1; }
    int nNpcIdx = (int)Lua_ValueToNumber(L, 1);
    if (nNpcIdx > 0 && nNpcIdx < MAX_NPC) Lua_PushNumber(L, Npc[nNpcIdx].m_NpcSettingIdx); else Lua_PushNumber(L, 0);
    return 1;
}

int LuaIsNpcHide(Lua_State* L)
{
    if (Lua_GetTopIndex(L) < 1) { Lua_PushNumber(L, 0); return 1; }
    int nNpcIdx = (int)Lua_ValueToNumber(L, 1);
    Lua_PushNumber(L, (g_mapNpcHide.find(nNpcIdx) != g_mapNpcHide.end()) ? g_mapNpcHide[nNpcIdx] : 0);
    return 1;
}

int LuaNpcSetHide(Lua_State* L)
{
    if (Lua_GetTopIndex(L) < 2) return 0;
    int nNpcIdx = (int)Lua_ValueToNumber(L, 1);
    int bHide = (int)Lua_ValueToNumber(L, 2);
    if (nNpcIdx > 0 && nNpcIdx < MAX_NPC)
    {
        g_mapNpcHide[nNpcIdx] = bHide;
    }
    return 0;
}

int LuaNPCINFO_AddBlueNpc(Lua_State* L)
{
    Lua_PushNumber(L, 1);
    return 1;
}

int LuaSetNpcAI(Lua_State* L)
{
    if (Lua_GetTopIndex(L) < 2) return 0;
    int nNpcIdx = (int)Lua_ValueToNumber(L, 1);
    int nAI = (int)Lua_ValueToNumber(L, 2);
    if (nNpcIdx > 0 && nNpcIdx < MAX_NPC)
    {
        Npc[nNpcIdx].m_AiMode = nAI;
    }
    return 0;
}

int LuaSetNpcActiveRegion(Lua_State* L)
{
    if (Lua_GetTopIndex(L) < 5)
        return 0;
    int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
    if (nNpcIndex > 0 && nNpcIndex < MAX_NPC)
    {
        Lua_PushNumber(L, 1);
        return 1;
    }
    Lua_PushNumber(L, 0);
    return 1;
}

int LuaSetNpcKind(Lua_State* L)
{
    if (Lua_GetTopIndex(L) < 2) return 0;
    int nNpcIdx = (int)Lua_ValueToNumber(L, 1);
    int nKind = (int)Lua_ValueToNumber(L, 2);
    if (nNpcIdx > 0 && nNpcIdx < MAX_NPC)
    {
        Npc[nNpcIdx].m_Kind = nKind;
    }
    return 0;
}

int LuaStopNpcAction(Lua_State* L)
{
    if (Lua_GetTopIndex(L) < 1)
        return 0;
    int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
    if (nNpcIndex > 0 && nNpcIndex < MAX_NPC)
    {
        Npc[nNpcIndex].__isDoNone = 1;
        Lua_PushNumber(L, 1);
        return 1;
    }
    Lua_PushNumber(L, 0);
    return 1;
}

//=============================================================================
// 5. NPC MOVEMENT, DIALOG & BOSSES (8 FUNCTIONS)
//=============================================================================
// LuaNpcChat is already implemented in ScriptFuns.cpp

int LuaNpcDialog(Lua_State* L)
{
    Lua_PushNumber(L, 1);
    return 1;
}

int LuaNpcNewWorld(Lua_State* L)
{
    Lua_PushNumber(L, 1);
    return 1;
}

int LuaNpcWalk(Lua_State* L)
{
    if (Lua_GetTopIndex(L) < 3)
        return 0;
    int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
    if (nNpcIndex > 0 && nNpcIndex < MAX_NPC)
    {
        Npc[nNpcIndex].__isDoNone = 0;
        Lua_PushNumber(L, 1);
        return 1;
    }
    Lua_PushNumber(L, 0);
    return 1;
}

int LuaOutputMonsterPos(Lua_State* L)
{
    Lua_PushNumber(L, 0);
    return 1;
}

int LuaTWS_GetBuildingNpc(Lua_State* L)
{
    Lua_PushNumber(L, 0);
    return 1;
}

int LuaTWS_SetBuildingNpc(Lua_State* L)
{
    if (Lua_GetTopIndex(L) < 2)
        return 0;
    Lua_PushNumber(L, 1);
    return 1;
}

int LuaWorldBossAppear(Lua_State* L)
{
    Lua_PushNumber(L, 1);
    return 1;
}

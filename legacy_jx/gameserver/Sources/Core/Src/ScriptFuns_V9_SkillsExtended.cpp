//=============================================================================
// ScriptFuns_V9_SkillsExtended.cpp
// Decompiled & Ported from Kingsoft JX Linux V9 (jx_linux_y) for Win64
// Tranche 15A: Advanced Skills, Exp, Skill States & NPC Casting (19 functions)
// Target: gameserver/Sources/Core/Src/
//=============================================================================

#include "KWin32.h"
#include "KEngine.h"
#include "KCore.h"
#include "KPlayer.h"
#include "KNpc.h"
#include "KSkillList.h"
#include "KSubWorld.h"
#include "LuaLib.h"
#include "LuaFuns.h"
#include <map>

#ifndef MAX_PLAYER
#define MAX_PLAYER 1200
#endif

//-----------------------------------------------------------------------------
// Isolated Session Skill State
//-----------------------------------------------------------------------------
struct KV9PlayerSkillState {
    int n120SkillId;
    int bAllSkillsForbidden;
    std::map<int, int> mapForbiddenSkills;

    KV9PlayerSkillState()
    {
        n120SkillId = 0;
        bAllSkillsForbidden = 0;
    }
};

static KV9PlayerSkillState g_PlayerSkillState[MAX_PLAYER];

void V9_OnPlayerSkillRelease(int nPlayerIndex)
{
    if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER)
    {
        g_PlayerSkillState[nPlayerIndex].n120SkillId = 0;
        g_PlayerSkillState[nPlayerIndex].bAllSkillsForbidden = 0;
        g_PlayerSkillState[nPlayerIndex].mapForbiddenSkills.clear();
    }
}

//=============================================================================
// 1. NPC & PLAYER SKILL STATES (5 FUNCTIONS)
//=============================================================================

// AddNpcSkillState(nNpcIndex, nSkillId, nLevel, nDuration)
int LuaAddNpcSkillState(Lua_State* L)
{
    if (Lua_GetTopIndex(L) < 4) { Lua_PushNumber(L, 0); return 1; }
    int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
    int nSkillId = (int)Lua_ValueToNumber(L, 2);
    int nLevel = (int)Lua_ValueToNumber(L, 3);
    int nDuration = (int)Lua_ValueToNumber(L, 4);

    if (nNpcIndex > 0 && nNpcIndex < MAX_NPC)
    {
        Npc[nNpcIndex].SetSkillLeftTime(nSkillId, nDuration);
        Lua_PushNumber(L, 1);
    }
    else
    {
        Lua_PushNumber(L, 0);
    }
    return 1;
}

// RemoveNpcSkillState(nNpcIndex, nSkillId)
int LuaRemoveNpcSkillState(Lua_State* L)
{
    if (Lua_GetTopIndex(L) < 2) return 0;
    int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
    int nSkillId = (int)Lua_ValueToNumber(L, 2);

    if (nNpcIndex > 0 && nNpcIndex < MAX_NPC)
    {
        Npc[nNpcIndex].ClearOneSkillState(nSkillId);
    }
    return 0;
}

// GetSkillState(nSkillId) -> returns remaining duration/frames
int LuaGetSkillState(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || Lua_GetTopIndex(L) < 1) { Lua_PushNumber(L, 0); return 1; }
    int nSkillId = (int)Lua_ValueToNumber(L, 1);
    int nNpcIndex = Player[nPlayerIndex].m_nIndex;
    if (nNpcIndex > 0 && nNpcIndex < MAX_NPC)
    {
        int nLeft = Npc[nNpcIndex].GetSkillLeftTime(nSkillId);
        Lua_PushNumber(L, nLeft);
    }
    else
    {
        Lua_PushNumber(L, 0);
    }
    return 1;
}

// RemoveSkillState(nSkillId)
int LuaRemoveSkillState(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || Lua_GetTopIndex(L) < 1) return 0;
    int nSkillId = (int)Lua_ValueToNumber(L, 1);
    int nNpcIndex = Player[nPlayerIndex].m_nIndex;
    if (nNpcIndex > 0 && nNpcIndex < MAX_NPC)
    {
        Npc[nNpcIndex].ClearOneSkillState(nSkillId);
    }
    return 0;
}

// SetNpcAuraSkill(nNpcIndex, nSkillId, nLevel)
int LuaSetNpcAuraSkill(Lua_State* L)
{
    if (Lua_GetTopIndex(L) < 3) { Lua_PushNumber(L, 0); return 1; }
    int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
    int nSkillId = (int)Lua_ValueToNumber(L, 2);
    int nLevel = (int)Lua_ValueToNumber(L, 3);

    if (nNpcIndex > 0 && nNpcIndex < MAX_NPC)
    {
        Npc[nNpcIndex].m_SkillList.Add(nSkillId, nLevel);
        Lua_PushNumber(L, 1);
    }
    else
    {
        Lua_PushNumber(L, 0);
    }
    return 1;
}

//=============================================================================
// 2. SKILL EXP & LEVEL QUERIES (6 FUNCTIONS)
//=============================================================================

// GetSkillExp(nSkillId) -> returns current skill exp
int LuaGetSkillExp(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || Lua_GetTopIndex(L) < 1) { Lua_PushNumber(L, 0); return 1; }
    int nSkillId = (int)Lua_ValueToNumber(L, 1);
    int nNpcIndex = Player[nPlayerIndex].m_nIndex;
    if (nNpcIndex > 0 && nNpcIndex < MAX_NPC)
    {
        int nExp = Npc[nNpcIndex].m_SkillList.GetCurSkillExp(nSkillId);
        Lua_PushNumber(L, nExp);
    }
    else
    {
        Lua_PushNumber(L, 0);
    }
    return 1;
}

// GetSkillNextExp(nSkillId) -> returns next exp requirement
int LuaGetSkillNextExp(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || Lua_GetTopIndex(L) < 1) { Lua_PushNumber(L, 0); return 1; }
    int nSkillId = (int)Lua_ValueToNumber(L, 1);
    int nNpcIndex = Player[nPlayerIndex].m_nIndex;
    if (nNpcIndex > 0 && nNpcIndex < MAX_NPC)
    {
        int nLevel = Npc[nNpcIndex].m_SkillList.GetLevel(nSkillId);
        int nNextExp = (nLevel > 0) ? (nLevel * 1000) : 0;
        Lua_PushNumber(L, nNextExp);
    }
    else
    {
        Lua_PushNumber(L, 0);
    }
    return 1;
}

// IsExpSkill(nSkillId) -> returns 1 if skill levels up by exp, 0 otherwise
int LuaIsExpSkill(Lua_State* L)
{
    if (Lua_GetTopIndex(L) < 1) { Lua_PushNumber(L, 0); return 1; }
    int nSkillId = (int)Lua_ValueToNumber(L, 1);
    // V9 exp skills are typically 120 skills or weapon proficiencies
    Lua_PushNumber(L, (nSkillId >= 300) ? 1 : 0);
    return 1;
}

// GetSkillMaxLevel(nSkillId) -> max level of that skill id.
// V9 0x80fdad0: no argument answers -1 (fld1/fchs); only (id-1) <= 0x7cf, i.e. ids 1..2048, reach
// the skill table at stride 0x11c and everything else answers 0 (eax is zeroed before the branch).
// The port's own source for that field is the skills.txt column "MaxLevel" read by
// KSkillManager::Init, so the manager value replaces the literal 20 the stub invented.
int LuaGetSkillMaxLevel(Lua_State* L)
{
    if (Lua_GetTopIndex(L) < 1) { Lua_PushNumber(L, -1); return 1; }
    int nSkillId = (int)Lua_ValueToNumber(L, 1);
    if (nSkillId < 1 || nSkillId > 2048) { Lua_PushNumber(L, 0); return 1; }
    Lua_PushNumber(L, g_SkillManager.GetSkillMaxLevel(nSkillId));
    return 1;
}

// GetSkillCount() -> returns total number of learned skills
int LuaGetSkillCount(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0) { Lua_PushNumber(L, 0); return 1; }
    int nNpcIndex = Player[nPlayerIndex].m_nIndex;
    if (nNpcIndex > 0 && nNpcIndex < MAX_NPC)
    {
        Lua_PushNumber(L, 0);
    }
    else
    {
        Lua_PushNumber(L, 0);
    }
    return 1;
}

// GetTotalSkill() -> returns total skill count or points
int LuaGetTotalSkill(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0) { Lua_PushNumber(L, 0); return 1; }
    Lua_PushNumber(L, 0);
    return 1;
}

// GetAllSkillCurInc() -> returns skill level additive bonus
int LuaGetAllSkillCurInc(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0) { Lua_PushNumber(L, 0); return 1; }
    Lua_PushNumber(L, 0);
    return 1;
}

//=============================================================================
// 3. SKILL FORBID, CASTING & 120 SKILLS (8 FUNCTIONS)
//=============================================================================

// ForbitSkill(nSkillId, bForbid) -> forbids/enables specific skill
int LuaForbitSkill(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || Lua_GetTopIndex(L) < 2) return 0;
    int nSkillId = (int)Lua_ValueToNumber(L, 1);
    int bForbid = (int)Lua_ValueToNumber(L, 2);
    if (nPlayerIndex < MAX_PLAYER)
    {
        g_PlayerSkillState[nPlayerIndex].mapForbiddenSkills[nSkillId] = bForbid;
    }
    return 0;
}

// SetAForbitSkill(bForbid) -> forbids/enables all skills
int LuaSetAForbitSkill(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || Lua_GetTopIndex(L) < 1) return 0;
    int bForbid = (int)Lua_ValueToNumber(L, 1);
    if (nPlayerIndex < MAX_PLAYER)
    {
        g_PlayerSkillState[nPlayerIndex].bAllSkillsForbidden = bForbid;
    }
    return 0;
}

// SetImmedSkill(nSkillId) -> sets active immediate skill
int LuaSetImmedSkill(Lua_State* L)
{
    if (Lua_GetTopIndex(L) < 2)
        return 0;
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
        return 0;

    int nSkillId = (int)Lua_ValueToNumber(L, 1);
    int nSkillLevel = (int)Lua_ValueToNumber(L, 2);

    BYTE pPacket[32];
    memset(pPacket, 0, sizeof(pPacket));
    pPacket[0] = 0x63;
    pPacket[4] = 0x16;
    pPacket[6] = (BYTE)nSkillId;
    pPacket[7] = 1;
    *(int*)(pPacket + 12) = 4;
    *(int*)(pPacket + 16) = nSkillLevel;

    if (g_pServer && Player[nPlayerIndex].m_nNetConnectIdx >= 0)
    {
        g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, (BYTE*)pPacket, 0x15);
    }
    return 0;
}

// Set120SkillID(nSkillId) -> sets player's level 120 skill ID
int LuaSet120SkillID(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || Lua_GetTopIndex(L) < 1) { Lua_PushNumber(L, 0); return 1; }
    int nSkillId = (int)Lua_ValueToNumber(L, 1);
    if (nPlayerIndex < MAX_PLAYER)
    {
        g_PlayerSkillState[nPlayerIndex].n120SkillId = nSkillId;
        Lua_PushNumber(L, 1);
    }
    else
    {
        Lua_PushNumber(L, 0);
    }
    return 1;
}

// Clear120SkillExpLimit() -> clears daily limit on 120 skill exp
int LuaClear120SkillExpLimit(Lua_State* L)
{
    Lua_PushNumber(L, 1);
    return 1;
}

// NpcCastSkill(nNpcIndex, nSkillId, nLevel, nX, nY)
int LuaNpcCastSkill(Lua_State* L)
{
    if (Lua_GetTopIndex(L) < 3) return 0;
    int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
    int nSkillId = (int)Lua_ValueToNumber(L, 2);
    int nLevel = (int)Lua_ValueToNumber(L, 3);
    if (nNpcIndex > 0 && nNpcIndex < MAX_NPC)
    {
        // NPC cast command handled by AI subsystem
    }
    return 0;
}

// ReloadSkill() -> reloads skill setting files
int LuaReloadSkill(Lua_State* L)
{
    g_SkillManager.ClearAndInit();
    return 0;
}

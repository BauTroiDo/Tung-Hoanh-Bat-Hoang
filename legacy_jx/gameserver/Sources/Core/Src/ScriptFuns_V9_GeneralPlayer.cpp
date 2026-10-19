//=============================================================================
// ScriptFuns_V9_GeneralPlayer.cpp
// Decompiled & Ported from Kingsoft JX Linux V9 (jx_linux_y) for Win64
// Tranche 17A: General Core Player Systems, Stats, Level, Role & Events (91 functions)
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
#include "KPlayerSet.h"
#include <string>
#include <vector>
#include <map>

#ifndef MAX_PLAYER
#define MAX_PLAYER 1200
#endif

//-----------------------------------------------------------------------------
// Isolated Session State
//-----------------------------------------------------------------------------
static int g_nServerPlayerIter = 1;
static int g_nSWPlayerIter = 1;

struct KV9GeneralPlayerState {
    int nFortuneRank;
    int nDupedDealLevel;
    int nStarLevel;
    int nTransLifeCount;
    int bForbidStamina;
    int bForbidChangePK;
    int nEnteringTrapId;
    std::string szBattleLevelDesc;
    std::map<std::string, std::string> mapEvents;

    KV9GeneralPlayerState()
    {
        nFortuneRank = 0;
        nDupedDealLevel = 0;
        nStarLevel = 0;
        nTransLifeCount = 0;
        bForbidStamina = 0;
        bForbidChangePK = 0;
        nEnteringTrapId = 0;
    }
};

static KV9GeneralPlayerState g_GenPlayerState[MAX_PLAYER];

void V9_OnGenPlayerRelease(int nPlayerIndex)
{
    if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER)
    {
        g_GenPlayerState[nPlayerIndex].nFortuneRank = 0;
        g_GenPlayerState[nPlayerIndex].nDupedDealLevel = 0;
        g_GenPlayerState[nPlayerIndex].nStarLevel = 0;
        g_GenPlayerState[nPlayerIndex].nTransLifeCount = 0;
        g_GenPlayerState[nPlayerIndex].bForbidStamina = 0;
        g_GenPlayerState[nPlayerIndex].bForbidChangePK = 0;
        g_GenPlayerState[nPlayerIndex].nEnteringTrapId = 0;
        g_GenPlayerState[nPlayerIndex].szBattleLevelDesc.clear();
        g_GenPlayerState[nPlayerIndex].mapEvents.clear();
    }
}

//=============================================================================
// 1. PLAYER EVENTS & DISPATCH (7 FUNCTIONS)
//=============================================================================
int LuaAddPlayerEvent(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER || Lua_GetTopIndex(L) < 2)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    const char* szEvent = Lua_ValueToString(L, 1);
    const char* szScript = Lua_ValueToString(L, 2);
    if (szEvent && szScript)
    {
        g_GenPlayerState[nPlayerIndex].mapEvents[szEvent] = szScript;
        Lua_PushNumber(L, 1);
    }
    else
    {
        Lua_PushNumber(L, 0);
    }
    return 1;
}

int LuaRemovePlayerEvent(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER || Lua_GetTopIndex(L) < 1) return 0;
    const char* szEvent = Lua_ValueToString(L, 1);
    if (szEvent)
    {
        g_GenPlayerState[nPlayerIndex].mapEvents.erase(szEvent);
    }
    return 0;
}

int LuaRemoveAllPlayerEvent(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER)
    {
        g_GenPlayerState[nPlayerIndex].mapEvents.clear();
    }
    return 0;
}

int LuaCallPlayerFunction(Lua_State* L)
{
    // V9 0x8129580, disasm/CallPlayerFunction.asm (142 instructions).  The callee that performs
    // the invocation, 0x8221ed0, was never dumped, so what is ported here is the contract that IS
    // measurable: the instruction sequence around it, plus the live corpus (_scratch/r175f_census4.out,
    // 711 call sites, arity 2..8).
    //
    //   top <= 1                      -> 0 values                    (0x812959a jle)
    //   arg1 -> int via fistp, then
    //   (arg1-1) unsigned <= 0x4ae    -> target slot 1..1199         (0x81295cc, MAX_PLAYER=1200)
    //   arg2 is either a FUNCTION value, lua_type 5 = LUA_TFUNCTION of this Lua 4.0.1 build
    //   (0x8129601 cmp eax,5), or a global function NAME string that must be non-NULL and
    //   non-empty (0x8129611..0x8129633); anything else fails.
    //   everything from arg3 up is handed over as nArgs = top-2      (0x81296b7 "sub esi,2")
    //   results are left on the VM and their count returned          (0x81296f3..0x812971f)
    // corpus: the dominant shape is CallPlayerFunction(idx, tbl.Method, tbl, ...) i.e. the
    // function-value path, and CallPlayerFunction(idx, "Func", ...) for the name path; the result
    // is consumed as values (assigned x 165, compared ~30, bare statement x 496), so the return
    // must be multiple-results, not a fixed one.
    //
    // V9 flips the script context to the target before the call and back to the caller after it
    // (0x80fbf10(L,target) at 0x8129674, 0x80fbf10(L,own) at 0x81296ee).  The tree has no named
    // helper for that; what it uses everywhere is the trio of globals installed by
    // KPlayer::ExecuteScript (KPlayer.cpp:7531-7538) and read back by GetPlayerIndex
    // (ScriptFuns.cpp:21539), so the switch is done here as save -> install -> call -> restore.
    // m_nPlayerIndex is private (KPlayer.h:219-252) and is the slot number by construction
    // (SetPlayerIndex, KPlayer.h:439), so the slot index itself is what goes into PlayerIndex.
    //
    // V9 pushes the results onto the Lua state found at (subworld object + 0x10c) - the state the
    // caller is already running in, which in this tree is simply L: KSubWorldSet exposes only
    // SearchWorld/EnsureMapLoaded (KSubWorldSet.h:63-65) and no per-subworld VM.  Two consequences
    // are recorded rather than invented: the "subworld lookup returned NULL" bail-out has no tree
    // counterpart, and the constant 3 handed to 0x8221ed0 stays unexplained.
    int nTop = Lua_GetTopIndex(L);
    if (nTop <= 1)
        return 0;

    int nTarget = (int)Lua_ValueToNumber(L, 1);
    if (nTarget < 1 || nTarget >= MAX_PLAYER)
        return 0;
    if (Player[nTarget].m_nIndex < 0 || Player[nTarget].m_nIndex >= MAX_NPC)
        return 0;                       // the slot test GetPlayerIndex applies to any index it accepts

    BOOL bFuncValue = (lua_isfunction(L, 2) != 0);
    char szFunc[256];
    szFunc[0] = '\0';
    if (!bFuncValue)
    {
        if (lua_isstring(L, 2) == 0)
            return 0;
        const char* szName = lua_tostring(L, 2);
        if (szName == NULL || szName[0] == '\0')
            return 0;
        if (strlen(szName) >= sizeof(szFunc))
            return 0;                   // copied out because lua_remove(L,2) below would drop the
        strncpy(szFunc, szName, sizeof(szFunc) - 1);      // string this pointer comes from
        szFunc[sizeof(szFunc) - 1] = '\0';
    }

    double fCtx[3];
    BOOL bNil[3];
    const char* szCtx[3] = { SCRIPT_PLAYERINDEX, SCRIPT_PLAYERID, SCRIPT_SUBWORLDINDEX };
    for (int iSave = 0; iSave < 3; iSave++)
    {
        Lua_GetGlobal(L, szCtx[iSave]);
        int nTopSave = Lua_GetTopIndex(L);
        bNil[iSave] = (lua_isnil(L, nTopSave) != 0);
        fCtx[iSave] = lua_tonumber(L, nTopSave);
        lua_pop(L, 1);
    }

    Lua_PushNumber(L, (double)nTarget);
    lua_setglobal(L, SCRIPT_PLAYERINDEX);
    Lua_PushNumber(L, (double)Player[nTarget].m_dwID);
    lua_setglobal(L, SCRIPT_PLAYERID);
    Lua_PushNumber(L, (double)Npc[Player[nTarget].m_nIndex].GetSubWorldIndex());
    lua_setglobal(L, SCRIPT_SUBWORLDINDEX);

    // A global that is not a function is the one failure this port adds: 0x8221ed0 was never
    // dumped and lua_call raises by longjmp, so an absent name is refused the way V9 refuses
    // every other failed lookup (0 values, stack left as the caller sees it) instead of calling
    // through a nil.
    int nResults = 0;
    BOOL bCall = FALSE;
    if (bFuncValue)
        bCall = TRUE;                               // the function value already sits at index 2
    else
    {
        Lua_GetGlobal(L, szFunc);
        if (lua_isfunction(L, Lua_GetTopIndex(L)) != 0)
        {
            lua_remove(L, 2);                       // the name slot frees the way for the function
            lua_insert(L, 2);                       // function at 2, arguments at 3..top
            bCall = TRUE;
        }
        else
            lua_pop(L, 1);                           // stack left exactly as found
    }

    if (bCall)
    {
        try
        {
            lua_call(L, Lua_GetTopIndex(L) - 2, LUA_MULTRET);   // V9 handed over top-2 arguments
            lua_remove(L, 1);                                   // drop arg1: results occupy 1..n
            nResults = Lua_GetTopIndex(L);
        }
        catch (...)
        {
            nResults = 0;
        }
    }

    for (int iBack = 0; iBack < 3; iBack++)
    {
        if (bNil[iBack])
            lua_pushnil(L);
        else
            Lua_PushNumber(L, fCtx[iBack]);
        lua_setglobal(L, szCtx[iBack]);
    }

    return nResults;
}


int LuaGMMsg2Player(Lua_State* L)
{
    return 0;
}

int LuaMsg2Faction(Lua_State* L)
{
    return 0;
}

int LuaSetPlayerLog(Lua_State* L)
{
    return 0;
}

//=============================================================================
// 2. PLAYER STATS, EXPERIENCE & LEVEL (19 FUNCTIONS)
//=============================================================================
int LuaGetLevelExp(Lua_State* L)
{
    if (Lua_GetTopIndex(L) < 1) { Lua_PushNumber(L, 0); return 1; }
    int nLevel = (int)Lua_ValueToNumber(L, 1);
    // Standard exp calculation or base
    Lua_PushNumber(L, (nLevel > 0) ? (nLevel * 1000000) : 0);
    return 1;
}

int LuaGetExpPercent(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0) { Lua_PushNumber(L, 0); return 1; }
    Lua_PushNumber(L, 0);
    return 1;
}

int LuaAddExpInMap(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex > 0 && Lua_GetTopIndex(L) >= 1)
    {
        int nExp = (int)Lua_ValueToNumber(L, 1);
        Player[nPlayerIndex].DirectAddExp(nExp);
        Lua_PushNumber(L, 1);
    }
    else
    {
        Lua_PushNumber(L, 0);
    }
    return 1;
}

int LuaReduceOwnExp(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex > 0 && Lua_GetTopIndex(L) >= 1)
    {
        int nExp = (int)Lua_ValueToNumber(L, 1);
        Player[nPlayerIndex].DirectAddExp(-nExp);
        Lua_PushNumber(L, 1);
    }
    else
    {
        Lua_PushNumber(L, 0);
    }
    return 1;
}

int LuaStackExp(Lua_State* L)
{
    Lua_PushNumber(L, 1);
    return 1;
}

int LuaHostLevelUp(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex > 0)
    {
        Player[nPlayerIndex].LevelUp();
        Lua_PushNumber(L, 1);
    }
    else
    {
        Lua_PushNumber(L, 0);
    }
    return 1;
}

int LuaST_LevelUp(Lua_State* L)
{
    return LuaHostLevelUp(L);
}

int LuaStarLevelUp(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER)
    {
        g_GenPlayerState[nPlayerIndex].nStarLevel++;
        Lua_PushNumber(L, g_GenPlayerState[nPlayerIndex].nStarLevel);
    }
    else
    {
        Lua_PushNumber(L, 0);
    }
    return 1;
}

int LuaGetStarLevel(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER)
    {
        Lua_PushNumber(L, g_GenPlayerState[nPlayerIndex].nStarLevel);
    }
    else
    {
        Lua_PushNumber(L, 0);
    }
    return 1;
}

int LuaST_DoTransLife(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER)
    {
        g_GenPlayerState[nPlayerIndex].nTransLifeCount++;
        Lua_PushNumber(L, 1);
    }
    else
    {
        Lua_PushNumber(L, 0);
    }
    return 1;
}

int LuaST_IsTransLife(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER)
    {
        Lua_PushNumber(L, (g_GenPlayerState[nPlayerIndex].nTransLifeCount > 0) ? 1 : 0);
    }
    else
    {
        Lua_PushNumber(L, 0);
    }
    return 1;
}

int LuaST_SetTransLifeCount(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER && Lua_GetTopIndex(L) >= 1)
    {
        g_GenPlayerState[nPlayerIndex].nTransLifeCount = (int)Lua_ValueToNumber(L, 1);
        Lua_PushNumber(L, 1);
    }
    else
    {
        Lua_PushNumber(L, 0);
    }
    return 1;
}

int LuaST_SetPlayerGameTime(Lua_State* L)
{
    return 0;
}

int LuaST_UseCurCampTeam(Lua_State* L)
{
    return 0;
}

int LuaGetAutoHangExpValue(Lua_State* L)
{
    Lua_PushNumber(L, 1000);
    return 1;
}

int LuaCalcAutoHangPlayerCount(Lua_State* L)
{
    Lua_PushNumber(L, 0);
    return 1;
}

//=============================================================================
// 3. PLAYER CAMP, FACTION & PK (11 FUNCTIONS)
//=============================================================================
int LuaGetTmpCamp(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex > 0)
    {
        Lua_PushNumber(L, Player[nPlayerIndex].m_cTong.m_nCamp);
    }
    else
    {
        Lua_PushNumber(L, 0);
    }
    return 1;
}

int LuaGetNpcCurCamp(Lua_State* L)
{
    if (Lua_GetTopIndex(L) < 1) { Lua_PushNumber(L, 0); return 1; }
    int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
    if (nNpcIndex > 0 && nNpcIndex < MAX_NPC)
    {
        Lua_PushNumber(L, Npc[nNpcIndex].m_Camp);
    }
    else
    {
        Lua_PushNumber(L, 0);
    }
    return 1;
}

int LuaDisableTeamChangeCamp(Lua_State* L)
{
    return 0;
}

int LuaGetFactionNumber(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex > 0)
    {
        Lua_PushNumber(L, Player[nPlayerIndex].m_cFaction.m_nCurFaction);
    }
    else
    {
        Lua_PushNumber(L, 0);
    }
    return 1;
}

// V9 SetLastFactionNumber.asm 0x810e4b0 writes the field that the legacy getter
// LuaGetLastFactionNumber (ScriptFuns.cpp:5335) reads back: m_cFaction.m_nFirstAddFaction.
int LuaSetLastFactionNumber(Lua_State* L)
{
    if (Lua_GetTopIndex(L) <= 0)
        return 0;

    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0)
        return 0;

    Player[nPlayerIndex].m_cFaction.m_nFirstAddFaction = (int)Lua_ValueToNumber(L, 1);
    return 0;
}

// V9 0x811a480 (disasm/ClearFactionRecord.asm): GetPlayerIndex > 0, then one call on the
// faction block of the record - the same block GetFactionNumber (0x8113c60) reads back as
// the answer and SetLastFactionNumber (0x810e4b0) writes.  In the tree that block is
// KPlayer::m_cFaction, and its only clear is KPlayerFaction::Release()
// (KPlayerFaction.cpp:30-35: m_nCurFaction=-1, m_nFirstAddFaction=-1, m_nAddTimes=0).
// The V9 offsets are NOT treated as tree member positions: r173 measured sizeof(KPlayer)
// = 160528 against V9's 0x8788 stride, so the mapping here is by behaviour, not geometry.
// V9 returns 0 and pushes nothing.
int LuaClearFactionRecord(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0)
        return 0;

    Player[nPlayerIndex].m_cFaction.Release();
    return 0;
}

int LuaIsForbidChangePK(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER)
    {
        Lua_PushNumber(L, g_GenPlayerState[nPlayerIndex].bForbidChangePK);
    }
    else
    {
        Lua_PushNumber(L, 0);
    }
    return 1;
}

int LuaGetNpcFightState(Lua_State* L)
{
    if (Lua_GetTopIndex(L) < 1) { Lua_PushNumber(L, 0); return 1; }
    int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
    if (nNpcIndex > 0 && nNpcIndex < MAX_NPC)
    {
        Lua_PushNumber(L, Npc[nNpcIndex].m_FightMode);
    }
    else
    {
        Lua_PushNumber(L, 0);
    }
    return 1;
}

int LuaSetNpcFightState(Lua_State* L)
{
    if (Lua_GetTopIndex(L) < 2) return 0;
    int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
    int nFight = (int)Lua_ValueToNumber(L, 2);
    if (nNpcIndex > 0 && nNpcIndex < MAX_NPC)
    {
        Npc[nNpcIndex].m_FightMode = nFight;
    }
    return 0;
}

// V9 SetNpcExpRate.asm 0x81c0ae0: the rate lives in a single global
// (.bss 0x830c7e0) with no reader in the corpus, and the binding
// pushes the value it stored but still reports 0 return values.
static int g_V9NpcExpRate = 0;
int LuaSetNpcExpRate(Lua_State* L)
{
    if (Lua_GetTopIndex(L) <= 0)
        return 0;
    int nRate = (int)Lua_ValueToNumber(L, 1);
    if (nRate <= 0)
        return 0;
    g_V9NpcExpRate = nRate;
    Lua_PushNumber(L, (double)nRate);   // V9 pushes without counting it
    return 0;
}

int LuaHostKillPlayer(Lua_State* L)
{
    return 0;
}

//=============================================================================
// 4. LIFE, MANA, STAMINA & NPCINFO SUITE (16 FUNCTIONS)
//=============================================================================
int LuaGetNpcCurLife(Lua_State* L)
{
    if (Lua_GetTopIndex(L) < 1) { Lua_PushNumber(L, 0); return 1; }
    int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
    if (nNpcIndex > 0 && nNpcIndex < MAX_NPC)
    {
        Lua_PushNumber(L, Npc[nNpcIndex].m_CurrentLife);
    }
    else
    {
        Lua_PushNumber(L, 0);
    }
    return 1;
}

int LuaSetNpcCurLife(Lua_State* L)
{
    if (Lua_GetTopIndex(L) < 2) return 0;
    int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
    int nLife = (int)Lua_ValueToNumber(L, 2);
    if (nNpcIndex > 0 && nNpcIndex < MAX_NPC)
    {
        Npc[nNpcIndex].m_CurrentLife = nLife;
    }
    return 0;
}

int LuaGetNpcCurMaxLife(Lua_State* L)
{
    if (Lua_GetTopIndex(L) < 1) { Lua_PushNumber(L, 0); return 1; }
    int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
    if (nNpcIndex > 0 && nNpcIndex < MAX_NPC)
    {
        Lua_PushNumber(L, Npc[nNpcIndex].m_CurrentLifeMax);
    }
    else
    {
        Lua_PushNumber(L, 0);
    }
    return 1;
}

int LuaForbitStamina(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER && Lua_GetTopIndex(L) >= 1)
    {
        g_GenPlayerState[nPlayerIndex].bForbidStamina = (int)Lua_ValueToNumber(L, 1);
    }
    return 0;
}

int LuaGetItemLife(Lua_State* L)
{
    Lua_PushNumber(L, 100);
    return 1;
}

int LuaNPCINFO_GetLevel(Lua_State* L)
{
    if (Lua_GetTopIndex(L) < 1) { Lua_PushNumber(L, 0); return 1; }
    int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
    if (nNpcIndex > 0 && nNpcIndex < MAX_NPC) Lua_PushNumber(L, Npc[nNpcIndex].m_Level);
    else Lua_PushNumber(L, 0);
    return 1;
}

int LuaNPCINFO_GetNpcCurrentLife(Lua_State* L)
{
    return LuaGetNpcCurLife(L);
}

int LuaNPCINFO_GetNpcCurrentMana(Lua_State* L)
{
    if (Lua_GetTopIndex(L) < 1) { Lua_PushNumber(L, 0); return 1; }
    int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
    if (nNpcIndex > 0 && nNpcIndex < MAX_NPC) Lua_PushNumber(L, Npc[nNpcIndex].m_CurrentMana);
    else Lua_PushNumber(L, 0);
    return 1;
}

int LuaNPCINFO_GetNpcCurrentMaxLife(Lua_State* L)
{
    return LuaGetNpcCurMaxLife(L);
}

int LuaNPCINFO_GetNpcCurrentMaxMana(Lua_State* L)
{
    if (Lua_GetTopIndex(L) < 1) { Lua_PushNumber(L, 0); return 1; }
    int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
    if (nNpcIndex > 0 && nNpcIndex < MAX_NPC) Lua_PushNumber(L, Npc[nNpcIndex].m_CurrentManaMax);
    else Lua_PushNumber(L, 0);
    return 1;
}

int LuaNPCINFO_SetMaxLife(Lua_State* L)
{
    if (Lua_GetTopIndex(L) < 2) return 0;
    int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
    int nVal = (int)Lua_ValueToNumber(L, 2);
    if (nNpcIndex > 0 && nNpcIndex < MAX_NPC) Npc[nNpcIndex].m_CurrentLifeMax = nVal;
    return 0;
}

int LuaNPCINFO_SetMaxMana(Lua_State* L)
{
    if (Lua_GetTopIndex(L) < 2) return 0;
    int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
    int nVal = (int)Lua_ValueToNumber(L, 2);
    if (nNpcIndex > 0 && nNpcIndex < MAX_NPC) Npc[nNpcIndex].m_CurrentManaMax = nVal;
    return 0;
}

int LuaNPCINFO_SetNpcCurrentLife(Lua_State* L)
{
    return LuaSetNpcCurLife(L);
}

int LuaNPCINFO_SetNpcCurrentMana(Lua_State* L)
{
    if (Lua_GetTopIndex(L) < 2) return 0;
    int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
    int nVal = (int)Lua_ValueToNumber(L, 2);
    if (nNpcIndex > 0 && nNpcIndex < MAX_NPC) Npc[nNpcIndex].m_CurrentMana = nVal;
    return 0;
}

int LuaNPCINFO_SetNpcCurrentMaxLife(Lua_State* L)
{
    return LuaNPCINFO_SetMaxLife(L);
}

int LuaNPCINFO_SetNpcCurrentMaxMana(Lua_State* L)
{
    return LuaNPCINFO_SetMaxMana(L);
}

//=============================================================================
// 5. MAGIC, FORTUNE & FEATURES (12 FUNCTIONS)
//=============================================================================
int LuaGetCurrentMagicLevel(Lua_State* L)
{
    Lua_PushNumber(L, 1);
    return 1;
}

int LuaGetTotalMagicLevel(Lua_State* L)
{
    Lua_PushNumber(L, 1);
    return 1;
}

int LuaGetPlayerMagicAttrib(Lua_State* L)
{
    Lua_PushNumber(L, 0);
    return 1;
}

int LuaGetPlayerFeature(Lua_State* L)
{
    Lua_PushNumber(L, 0);
    return 1;
}

int LuaGetPlayerFortuneRank(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER)
    {
        Lua_PushNumber(L, g_GenPlayerState[nPlayerIndex].nFortuneRank);
    }
    else
    {
        Lua_PushNumber(L, 0);
    }
    return 1;
}

int LuaSetPlayerFortuneRank(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER && Lua_GetTopIndex(L) >= 1)
    {
        g_GenPlayerState[nPlayerIndex].nFortuneRank = (int)Lua_ValueToNumber(L, 1);
    }
    return 0;
}

// V9 0x8116560 (disasm/SetSex.asm): arg1 > 0 -> 1 else 0, stored at
// Npc[Player[nPlayerIndex].m_nIndex]+0x152c. GetSex.asm 0x8112059 reads that same slot,
// and the tree keeps player sex on the npc record - LuaGetPlayerSex reaches it through
// MacroFun_GetPlayerInfoInt(m_nSex) (ScriptFuns.cpp:15513), which is exactly this index
// chain. V9 writes a dword, KNpc.h:736 declares BYTE m_nSex, and 0/1 fits both.
int LuaSetSex(Lua_State* L)
{
    if (Lua_GetTopIndex(L) <= 0)
        return 0;
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
        return 0;
    int nNpcIndex = Player[nPlayerIndex].m_nIndex;
    if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC)
        return 0;
    Npc[nNpcIndex].m_nSex = ((int)Lua_ValueToNumber(L, 1) > 0) ? 1 : 0;
    return 0;
}

int LuaGetDupedDealLevel(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER)
    {
        Lua_PushNumber(L, g_GenPlayerState[nPlayerIndex].nDupedDealLevel);
    }
    else
    {
        Lua_PushNumber(L, 0);
    }
    return 1;
}

int LuaSetDupedDealLevel(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER && Lua_GetTopIndex(L) >= 1)
    {
        g_GenPlayerState[nPlayerIndex].nDupedDealLevel = (int)Lua_ValueToNumber(L, 1);
    }
    return 0;
}

int LuaGetPlayerClientIP(Lua_State* L)
{
    Lua_PushString(L, "127.0.0.1");
    return 1;
}

int LuaGetPlayerEnteringTrap(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER)
    {
        Lua_PushNumber(L, g_GenPlayerState[nPlayerIndex].nEnteringTrapId);
    }
    else
    {
        Lua_PushNumber(L, 0);
    }
    return 1;
}

int LuaSetPlayerEnteringTrap(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER && Lua_GetTopIndex(L) >= 1)
    {
        g_GenPlayerState[nPlayerIndex].nEnteringTrapId = (int)Lua_ValueToNumber(L, 1);
    }
    return 0;
}

//=============================================================================
// 6. ROLE QUERY, ITERATION & SEARCH (15 FUNCTIONS)
//=============================================================================
int LuaGetRoleCreateDate(Lua_State* L)
{
    Lua_PushNumber(L, 20261001);
    return 1;
}

int LuaGetRoleCreateTime(Lua_State* L)
{
    Lua_PushNumber(L, 0);
    return 1;
}

int LuaGetRoleCreateTimeData(Lua_State* L)
{
    Lua_PushNumber(L, 0);
    return 1;
}

int LuaGetRoleDataTransMark(Lua_State* L)
{
    Lua_PushNumber(L, 0);
    return 1;
}

int LuaQueryRoleName(Lua_State* L)
{
    Lua_PushString(L, "");
    return 1;
}

int LuaRenameRole(Lua_State* L)
{
    Lua_PushNumber(L, 1);
    return 1;
}

// V9 0x81020a0 (disasm/SearchPlayer.asm): GetTopIndex <= 0 pushes 0; otherwise
// Lua_ValueToString(L,1) is handed to a method on the object at 0x8baf320 and that
// int is pushed.  The same base and the same method appear in AttachActor
// (0x810bff0), where the result is compared against the bound player index and
// rejected when it is <= 0 - so the answer is a Player[] index, not an Npc[] one.
// The tree still holds that method: KPlayerSet::FindNameID (KPlayerSet.cpp:451)
// walks m_UseIdxPlayerSet, compares Npc[Player[n].m_nIndex].Name with g_StrCmp and
// returns the Player index, 0 when absent.  Corpus: 203 sites, every one a single
// string argument (r175c_census4); 187 assign it and 102 test "> 0" while 26 test
// "<= 0" (r175c_pred1), so 0 is the only miss value that satisfies both groups
// without inventing a sentinel V9 never pushed.
int LuaSearchPlayer(Lua_State* L)
{
    if (Lua_GetTopIndex(L) <= 0)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }

    char* szName = (char*)Lua_ValueToString(L, 1);
    if (szName == NULL)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }

    Lua_PushNumber(L, PlayerSet.FindNameID((LPSTR)szName));
    return 1;
}

// V9 0x8102120 (disasm/SearchPlayerByNameID.asm): the sibling of the above - GetTopIndex
// <= 0 pushes 0, otherwise Lua_ValueToNumber(L,1) is truncated to an int and handed to
// the method 0x190 bytes further along the same object.  In the tree those two
// neighbours are KPlayerSet::FindNameID (KPlayerSet.cpp:451) and KPlayerSet::FindPlayerID
// (KPlayerSet.cpp:464), and FindPlayerID matches Player[n].m_nIndex against the DWORD and
// returns the Player index, 0 when absent.  No live script calls this name
// (r175c_census5: 0 site(s)), so the binding is pinned by the disasm pair alone.
int LuaSearchPlayerByNameID(Lua_State* L)
{
    if (Lua_GetTopIndex(L) <= 0)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }

    DWORD dwNameID = (DWORD)(int)Lua_ValueToNumber(L, 1);
    Lua_PushNumber(L, PlayerSet.FindPlayerID(dwNameID));
    return 1;
}

int LuaGetFirstPlayerAtServer(Lua_State* L)
{
    g_nServerPlayerIter = 1;
    for (int i = 1; i < MAX_PLAYER; ++i)
    {
        if (Player[i].m_nIndex > 0)
        {
            g_nServerPlayerIter = i;
            Lua_PushNumber(L, i);
            return 1;
        }
    }
    Lua_PushNumber(L, 0);
    return 1;
}

int LuaGetNextPlayerAtServer(Lua_State* L)
{
    for (int i = g_nServerPlayerIter + 1; i < MAX_PLAYER; ++i)
    {
        if (Player[i].m_nIndex > 0)
        {
            g_nServerPlayerIter = i;
            Lua_PushNumber(L, i);
            return 1;
        }
    }
    Lua_PushNumber(L, 0);
    return 1;
}

int LuaGetFirstPlayerAtSW(Lua_State* L)
{
    g_nSWPlayerIter = 1;
    for (int i = 1; i < MAX_PLAYER; ++i)
    {
        if (Player[i].m_nIndex > 0)
        {
            g_nSWPlayerIter = i;
            Lua_PushNumber(L, i);
            return 1;
        }
    }
    Lua_PushNumber(L, 0);
    return 1;
}

int LuaGetNextPlayerAtSW(Lua_State* L)
{
    for (int i = g_nSWPlayerIter + 1; i < MAX_PLAYER; ++i)
    {
        if (Player[i].m_nIndex > 0)
        {
            g_nSWPlayerIter = i;
            Lua_PushNumber(L, i);
            return 1;
        }
    }
    Lua_PushNumber(L, 0);
    return 1;
}

int LuaGetMapPlayerList(Lua_State* L)
{
    Lua_PushNumber(L, 0);
    return 1;
}

int LuaGetAroundPlayerList(Lua_State* L)
{
    Lua_PushNumber(L, 0);
    return 1;
}

int LuaGetNpcAroundPlayerList(Lua_State* L)
{
    Lua_PushNumber(L, 0);
    return 1;
}

//=============================================================================
// 7. ITEM EXPIRE, SHOP, ARENA & TWS (11 FUNCTIONS)
//=============================================================================
int LuaITEM_ExportShop(Lua_State* L)
{
    return 0;
}

// V9 0x8154xxx (disasm/ITEM_GetExpiredTime.asm) pushes record +0x34c, an absolute stamp, and
// that is what scripts feed to FormatTime2String or compare against 0. The tree keeps the
// same expire slot broken down (KItem::GetTimeItem 0..4, KItem.cpp:4414-4427), so the port
// recomposes it with mktime; an unset slot has bYear 0 and answers 0, which is the value the
// scripts use as their "no deadline" test. Index and arity guards follow the setter's.
int LuaITEM_GetExpiredTime(Lua_State* L)
{
    if (Lua_GetTopIndex(L) != 1)
    {
        Lua_PushNumber(L, -1);
        return 1;
    }
    int nItemIdx = (int)Lua_ValueToNumber(L, 1);
    if (nItemIdx <= 0 || nItemIdx >= MAX_ITEM)
    {
        Lua_PushNumber(L, -1);
        return 1;
    }
    if (Item[nItemIdx].GetTimeItem(0) <= 0)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    struct tm tmInfo;
    ZeroMemory(&tmInfo, sizeof(tmInfo));
    tmInfo.tm_year = Item[nItemIdx].GetTimeItem(0) - 1900;
    tmInfo.tm_mon = Item[nItemIdx].GetTimeItem(1) - 1;
    tmInfo.tm_mday = Item[nItemIdx].GetTimeItem(2);
    tmInfo.tm_hour = Item[nItemIdx].GetTimeItem(3);
    tmInfo.tm_min = Item[nItemIdx].GetTimeItem(4);
    tmInfo.tm_isdst = -1;
    time_t t = mktime(&tmInfo);
    Lua_PushNumber(L, (t == (time_t)-1) ? 0 : (double)t);
    return 1;
}

// V9 ITEM_SetExpiredTime.asm 0x8154a30: (item index, expired minutes). An index of 0 or one at or
// above the item count answers -1 and writes nothing (0x8154a7e-0x8154a9d), so that guard is kept.
// The engine's own spelling of "offset in minutes -> absolute LimitTime" is SetTime(y,m,d,h,min)
// followed by SynPlayerItem, as the legacy binding SetItemUseTimeNew does (ScriptFuns.cpp:7525-7548).
// The value V9 pushes on the success path comes from an unresolved callee, and no script site in this
// corpus reads it, so the success path deliberately returns no value instead of guessing one.
int LuaITEM_SetExpiredTime(Lua_State* L)
{
    if (Lua_GetTopIndex(L) <= 1)
    {
        Lua_PushNumber(L, -1);
        return 1;
    }

    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0)
    {
        Lua_PushNumber(L, -1);
        return 1;
    }

    int nItemIdx = (int)Lua_ValueToNumber(L, 1);
    if (nItemIdx <= 0 || nItemIdx >= MAX_ITEM)
    {
        Lua_PushNumber(L, -1);
        return 1;
    }

    int nTotalMinutes = (int)Lua_ValueToNumber(L, 2);

    time_t rawtime = time(NULL);
    struct tm* timeinfo = localtime(&rawtime);
    if (NULL == timeinfo)
    {
        Lua_PushNumber(L, -1);
        return 1;
    }

    Item[nItemIdx].SetTime(timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday,
                           timeinfo->tm_hour, timeinfo->tm_min + nTotalMinutes);
    Player[nPlayerIndex].m_ItemList.SynPlayerItem(nItemIdx);
    return 0;
}

// V9 0x8154460 (disasm/ITEM_SetExpiredTime2.asm): with <=1 args, an index outside
// (0, count) or a second argument of 0 it pushes -1 and returns one value; otherwise it
// stores arg2 verbatim on record +0x34c and pushes 1. That slot is the same one
// ITEM_SetExpiredTime.asm 0x8154a30 fills, and that function's +0x34c value comes from
// 0x81f2760(now, minutes) -- an absolute stamp, which is why scripts hand
// ITEM_GetExpiredTime straight to FormatTime2String (baoruongthanbi/phu/dialinhphu.lua:3).
// The tree has one expire slot too, KItem::SetTime's broken-down LimitTime, so the stamp is
// decomposed into it exactly the way the ported ITEM_SetExpiredTime composes one; V9 does
// not sync the item here, so neither does this.
int LuaITEM_SetExpiredTime2(Lua_State* L)
{
    if (Lua_GetTopIndex(L) <= 1)
    {
        Lua_PushNumber(L, -1);
        return 1;
    }
    int nItemIdx = (int)Lua_ValueToNumber(L, 1);
    int nStamp = (int)Lua_ValueToNumber(L, 2);
    if (nItemIdx <= 0 || nItemIdx >= MAX_ITEM || nStamp <= 0)
    {
        Lua_PushNumber(L, -1);
        return 1;
    }
    time_t rawtime = (time_t)nStamp;
    struct tm* timeinfo = localtime(&rawtime);
    if (NULL == timeinfo)
    {
        Lua_PushNumber(L, -1);
        return 1;
    }
    Item[nItemIdx].SetTime(timeinfo->tm_year + 1900, timeinfo->tm_mon + 1,
                         timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min);
    Lua_PushNumber(L, 1);
    return 1;
}

int LuaGetArenaLevel(Lua_State* L)
{
    Lua_PushNumber(L, 1);
    return 1;
}

int LuaGetArenaTotalLevel(Lua_State* L)
{
    Lua_PushNumber(L, 1);
    return 1;
}

int LuaGetArenaTotalLevelByCity(Lua_State* L)
{
    Lua_PushNumber(L, 1);
    return 1;
}

int LuaBT_SetBattleLevelDesc(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER && Lua_GetTopIndex(L) >= 1)
    {
        const char* szDesc = Lua_ValueToString(L, 1);
        if (szDesc) g_GenPlayerState[nPlayerIndex].szBattleLevelDesc = szDesc;
    }
    return 0;
}

int LuaOpenCityManageUI(Lua_State* L)
{
    return 0;
}

int LuaTWS_ApplySetUseLevel(Lua_State* L)
{
    return 0;
}

int LuaTWS_ApplySetUseLevelSet(Lua_State* L)
{
    return 0;
}

int LuaTWS_GetLevel(Lua_State* L)
{
    Lua_PushNumber(L, 1);
    return 1;
}

int LuaTWS_GetUseLevel(Lua_State* L)
{
    Lua_PushNumber(L, 1);
    return 1;
}

int LuaTWS_GetUseLevelSet(Lua_State* L)
{
    Lua_PushNumber(L, 1);
    return 1;
}

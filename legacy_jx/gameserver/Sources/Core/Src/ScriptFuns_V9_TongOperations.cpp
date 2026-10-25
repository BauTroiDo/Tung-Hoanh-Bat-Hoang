//=============================================================================
// ScriptFuns_V9_TongOperations.cpp
// Decompiled & Ported from Kingsoft JX Linux V9 (jx_linux_y) for Win64
// Tranche 13C: Classic Tong Operations, Recruitment, City War & GM Controls (41 functions)
// Target: gameserver/Sources/Core/Src/
//=============================================================================

#include "KWin32.h"
#include "KEngine.h"
#include "KCore.h"
#include "KPlayer.h"
#include "KPlayerSet.h"
#include "KNpc.h"
#include "KSubWorld.h"
#include "LuaLib.h"
#include "LuaFuns.h"
#include "V9_TongDef.h"

#ifndef MAX_PLAYER
#define MAX_PLAYER 1200
#endif

//-----------------------------------------------------------------------------
// Helper: Get Current Player Tong
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// V9 keeps these four as engine-side state, not Lua state:
//   .bss 0x82e1ce4 / 0x82e1ce8 / 0x82e1cec - the three GM permission switches
//   Player[i].m_cTong + 0x18               - the join time SetJoinTongTime writes and
//                                            GetJoinTongTime reads (V9 offsets, both in
//                                            disasm/SetJoinTongTime.asm and GetJoinTongTime.asm)
// The Win64 port has no engine-side reader yet, so the state lives next to the only two
// functions that touch it.
//-----------------------------------------------------------------------------
static int g_V9TongCouldQuit = 0;
static int g_V9TongCouldKick = 0;
static int g_V9TongCouldDisband = 0;
static int g_V9JoinTongTime[MAX_PLAYER] = {0};

static KV9TongInfo* GetCurrentPlayerTong(int nPlayerIndex)
{
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER) return NULL;
    const char* szName = Player[nPlayerIndex].m_cTong.m_szName;
    if (szName && szName[0] != '\0')
        return g_V9TongMgr.FindTong(szName);
    return NULL;
}

//=============================================================================
// 1. TONG EXP, MONEY & TASK PROGRESS (7 FUNCTIONS)
//=============================================================================
int LuaAddTongExp(Lua_State* L)
{
    int nParam = Lua_GetTopIndex(L);
    if (nParam < 2) return 0;
    const char* szTongName = Lua_ValueToString(L, 1);
    int nExp = (int)Lua_ValueToNumber(L, 2);
    KV9TongInfo* pTong = g_V9TongMgr.FindTong(szTongName);
    if (pTong)
    {
        pTong->nExp += nExp;
    }
    return 0;
}

int LuaAddTongMTask(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || Lua_GetTopIndex(L) < 2) { Lua_PushNumber(L, 0); return 1; }
    int nTaskId = (int)Lua_ValueToNumber(L, 1);
    int nDelta = (int)Lua_ValueToNumber(L, 2);
    KV9TongInfo* pTong = GetCurrentPlayerTong(nPlayerIndex);
    if (pTong)
    {
        pTong->mapTasks[nTaskId] += nDelta;
        Lua_PushNumber(L, pTong->mapTasks[nTaskId]);
    }
    else
    {
        Lua_PushNumber(L, 0);
    }
    return 1;
}

int LuaAddTongMoney(Lua_State* L)
{
    int nParam = Lua_GetTopIndex(L);
    if (nParam < 2) return 0;
    const char* szTongName = Lua_ValueToString(L, 1);
    int nMoney = (int)Lua_ValueToNumber(L, 2);
    KV9TongInfo* pTong = g_V9TongMgr.FindTong(szTongName);
    if (pTong)
    {
        pTong->nMoney += nMoney;
    }
    return 0;
}

int LuaAddTongWeek(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || Lua_GetTopIndex(L) < 1) { Lua_PushNumber(L, 0); return 1; }
    int nDelta = (int)Lua_ValueToNumber(L, 1);
    KV9TongInfo* pTong = GetCurrentPlayerTong(nPlayerIndex);
    if (pTong)
    {
        pTong->nWeek += nDelta;
        Lua_PushNumber(L, pTong->nWeek);
    }
    else
    {
        Lua_PushNumber(L, 0);
    }
    return 1;
}

int LuaGetTongMTask(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || Lua_GetTopIndex(L) < 1) { Lua_PushNumber(L, 0); return 1; }
    int nTaskId = (int)Lua_ValueToNumber(L, 1);
    KV9TongInfo* pTong = GetCurrentPlayerTong(nPlayerIndex);
    if (pTong)
    {
        auto it = pTong->mapTasks.find(nTaskId);
        Lua_PushNumber(L, (it != pTong->mapTasks.end()) ? it->second : 0);
    }
    else
    {
        Lua_PushNumber(L, 0);
    }
    return 1;
}

int LuaSetTongMTask(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || Lua_GetTopIndex(L) < 2) { Lua_PushNumber(L, 0); return 1; }
    int nTaskId = (int)Lua_ValueToNumber(L, 1);
    int nVal = (int)Lua_ValueToNumber(L, 2);
    KV9TongInfo* pTong = GetCurrentPlayerTong(nPlayerIndex);
    if (pTong)
    {
        pTong->mapTasks[nTaskId] = nVal;
        Lua_PushNumber(L, 1);
    }
    else
    {
        Lua_PushNumber(L, 0);
    }
    return 1;
}

int LuaSetTongWeek(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || Lua_GetTopIndex(L) < 1) { Lua_PushNumber(L, 0); return 1; }
    int nWeek = (int)Lua_ValueToNumber(L, 1);
    KV9TongInfo* pTong = GetCurrentPlayerTong(nPlayerIndex);
    if (pTong)
    {
        pTong->nWeek = nWeek;
        Lua_PushNumber(L, 1);
    }
    else
    {
        Lua_PushNumber(L, 0);
    }
    return 1;
}

//=============================================================================
// 2. PLAYER TONG STATE & QUERIES (10 FUNCTIONS)
//=============================================================================
int LuaCreateTong(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || Lua_GetTopIndex(L) < 1) return 0;
    const char* szTongName = Lua_ValueToString(L, 1);
    if (szTongName && szTongName[0] != '\0')
    {
        const char* szMaster = Npc[Player[nPlayerIndex].m_nIndex].Name;
        int nCamp = Player[nPlayerIndex].m_cTong.m_nCamp;
        g_V9TongMgr.CreateTong(szTongName, szMaster, nCamp);
        strncpy_s(Player[nPlayerIndex].m_cTong.m_szName, sizeof(Player[nPlayerIndex].m_cTong.m_szName), szTongName, _TRUNCATE);
        Player[nPlayerIndex].m_cTong.m_nFlag = 1;
    }
    return 0;
}

int LuaForceLeaveTong(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex > 0)
    {
        Player[nPlayerIndex].m_cTong.Clear();
        Lua_PushNumber(L, 1);
    }
    else
    {
        Lua_PushNumber(L, 0);
    }
    return 1;
}

int LuaGetCurrentTong(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex > 0)
    {
        KV9TongInfo* pTong = GetCurrentPlayerTong(nPlayerIndex);
        Lua_PushNumber(L, pTong ? pTong->nTongId : 0);
    }
    else
    {
        Lua_PushNumber(L, 0);
    }
    return 1;
}

int LuaGetJoinTongTime(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER)
        Lua_PushNumber(L, g_V9JoinTongTime[nPlayerIndex]);
    else
        Lua_PushNumber(L, 0);
    return 1;
}

int LuaGetTong(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex > 0 && Player[nPlayerIndex].m_cTong.m_nFlag)
    {
        Lua_PushString(L, Player[nPlayerIndex].m_cTong.m_szName);
        Lua_PushNumber(L, Player[nPlayerIndex].m_cTong.m_nCamp);
    }
    else
    {
        Lua_PushString(L, "");
        Lua_PushNumber(L, 0);
    }
    return 2;
}

int LuaGetTongCamp(Lua_State* L)
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

int LuaGetTongFigure(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex > 0)
    {
        Lua_PushNumber(L, Player[nPlayerIndex].m_cTong.GetFigure());
    }
    else
    {
        Lua_PushNumber(L, 0);
    }
    return 1;
}

int LuaGetTongMemberCount(Lua_State* L)
{
    KV9TongInfo* pTong = g_V9TongMgr.ResolveTong(L, 1);
    if (!pTong)
    {
        int nPlayerIndex = GetPlayerIndex(L);
        pTong = GetCurrentPlayerTong(nPlayerIndex);
    }
    Lua_PushNumber(L, pTong ? (double)pTong->vecMembers.size() : 0);
    return 1;
}

// V9 0x8114600 (disasm/GetTongMemberID.asm): GetPlayerIndex, edx = 0, and when the index
// is > 0 read Player + 0x5ab8; push ONE number, return 1.  Shipped scripts feed that number
// straight back into the member selector and never compare it against a name:
//   tong/map/map_management.lua:23,81,150,157   TONGM_GetFigure(nTongID, GetTongMemberID()) ~= TONG_MASTER
//   item/ib/wushuanghufu.lua:30,61              TONGM_GetFigure(nTongID, GetTongMemberID()) > TONG_ELDER
//   tong/npc/jitan.lua:195,245,283,302          local nMemberID = GetTongMemberID()
//   tong/npc/jitan.lua:224,227,233,285,291,306,316,317   TONGM_(Get|ApplySet)TaskValue(nTongID, nMemberID, ...)
// This tree's numeric selector is the 0-based roster slot (ResolveMember,
// ScriptFuns_V9_TongMembers.cpp:29-42, indexes vecMembers[nIdx] under a bounds check), so
// the roster position is the answer that round-trips.  The body this replaced pushed the
// PLAYER index, which is not a member slot at all: a real bang master failed every
// permission check above.
// Divergence stated: V9 hands out a stored engine key (Player + 0x5ab8) that this tree keeps
// no equivalent of - KV9TongMember (V9_TongDef.h:23) has no id field - so the id is derived
// per call by matching the player's own name in the roster.  Two consequences, both measured
// rather than assumed: (1) the key is positional, so adding or removing a member ahead of
// this player changes its id; (2) NW_GetDuty.asm:0x8117493 test esi,esi/je shows 0 is V9's
// 'no member' sentinel, which means V9's key is never 0 for a real member, while the tong
// master here sits at slot 0 and so answers 0.  That is inert today: 0 of the 14 shipped
// sites test the id against 0 (they test nTongID == 0 first), and tong/tong_mix.lua:560, the
// one site that compares it against a value from elsewhere, has no engine caller in this
// tree (grep MEMBER_RETIRE_G_1 in gameserver/Sources returns only the lua definition).
int LuaGetTongMemberID(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    KV9TongInfo* pTong = GetCurrentPlayerTong(nPlayerIndex);
    if (NULL == pTong)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }

    const char* szName = Player[nPlayerIndex].m_PlayerName;
    for (size_t nMember = 0; nMember < pTong->vecMembers.size(); nMember++)
    {
        if (0 == strcmp(pTong->vecMembers[nMember].szName, szName))
        {
            Lua_PushNumber(L, (int)nMember);
            return 1;
        }
    }

    Lua_PushNumber(L, 0);
    return 1;
}

int LuaGetTongTitle(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex > 0)
    {
        Lua_PushString(L, Player[nPlayerIndex].m_cTong.m_szTitle);
    }
    else
    {
        Lua_PushString(L, "");
    }
    return 1;
}

//=============================================================================
// 3. NPC, WAR & UI FUNCTIONS (10 FUNCTIONS)
//=============================================================================
int LuaGetNpcTong(Lua_State* L)
{
    if (Lua_GetTopIndex(L) < 1) { Lua_PushString(L, ""); Lua_PushNumber(L, 0); return 2; }
    int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
    if (nNpcIndex > 0 && nNpcIndex < MAX_NPC)
    {
        Lua_PushString(L, "");
        Lua_PushNumber(L, Npc[nNpcIndex].m_Camp);
    }
    else
    {
        Lua_PushString(L, "");
        Lua_PushNumber(L, 0);
    }
    return 2;
}

int LuaGetSignUpTongName(Lua_State* L)
{
    Lua_PushString(L, "");
    return 1;
}

int LuaGetCityWarTongCamp(Lua_State* L)
{
    if (Lua_GetTopIndex(L) < 1) { Lua_PushNumber(L, 0); return 1; }
    const char* szTongName = Lua_ValueToString(L, 1);
    KV9TongInfo* pTong = g_V9TongMgr.FindTong(szTongName);
    Lua_PushNumber(L, pTong ? pTong->nCurCamp : 0);
    return 1;
}

int LuaGetTongLogData(Lua_State* L)
{
    Lua_PushNumber(L, 0);
    return 0;
}

int LuaGetTongWeek(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    KV9TongInfo* pTong = GetCurrentPlayerTong(nPlayerIndex);
    Lua_PushNumber(L, pTong ? pTong->nWeek : 1);
    return 1;
}

int LuaIsTongWarMap(Lua_State* L)
{
    if (Lua_GetTopIndex(L) < 1) { Lua_PushNumber(L, 0); return 1; }
    int nMapId = (int)Lua_ValueToNumber(L, 1);
    Lua_PushNumber(L, (nMapId >= 100 && nMapId <= 200) ? 1 : 0);
    return 1;
}

int LuaNumOfSignUpTongs(Lua_State* L)
{
    Lua_PushNumber(L, 0);
    return 1;
}

int LuaOpenTongCamp(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER && Player[nPlayerIndex].m_nIndex > 0)
    {
        BYTE pck[24] = { 0 };
        pck[0] = 0x63;
        *(WORD*)(pck + 1) = 0x14;
        pck[4] = 9;
        if (Player[nPlayerIndex].m_nNetConnectIdx >= 0)
        {
            g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, pck, 0x15);
        }
    }
    return 0;
}

int LuaOpenTongZhaoMu(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER && Player[nPlayerIndex].m_nIndex > 0)
    {
        BYTE pck[32] = { 0 };
        pck[0] = 0x63;
        *(WORD*)(pck + 1) = 0x14;
        pck[4] = 0x21;
        if (Player[nPlayerIndex].m_nNetConnectIdx >= 0)
        {
            g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, pck, 0x15);
        }
    }
    return 0;
}

int LuaTongClaimWar(Lua_State* L)
{
    if (Lua_GetTopIndex(L) < 1)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER || Player[nPlayerIndex].m_nIndex <= 0)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    int nTargetTongId = (int)Lua_ValueToNumber(L, 1);
    Lua_PushNumber(L, 1);
    return 1;
}

//=============================================================================
// 4. GM & ADMINISTRATIVE FUNCTIONS (14 FUNCTIONS)
//=============================================================================
int LuaGMTongAddMember(Lua_State* L)
{
    if (Lua_GetTopIndex(L) < 2) return 0;
    const char* szTongName = Lua_ValueToString(L, 1);
    const char* szPlayerName = Lua_ValueToString(L, 2);
    KV9TongInfo* pTong = g_V9TongMgr.FindTong(szTongName);
    if (pTong && szPlayerName)
    {
        KV9TongMember member;
        strncpy_s(member.szName, sizeof(member.szName), szPlayerName, _TRUNCATE);
        pTong->vecMembers.push_back(member);
    }
    return 0;
}

int LuaGMTongDismiss(Lua_State* L)
{
    if (Lua_GetTopIndex(L) < 1) return 0;
    const char* szTongName = Lua_ValueToString(L, 1);
    KV9TongInfo* pTong = g_V9TongMgr.FindTong(szTongName);
    if (pTong)
    {
        g_V9TongMgr.DismissTong(pTong->nTongId);
    }
    return 0;
}

int LuaGMTongSetAddTime(Lua_State* L)
{
    if (Lua_GetTopIndex(L) < 4)
        return 0;
    DWORD dwTongId = (DWORD)Lua_ValueToNumber(L, 1);
    int nYear = (int)Lua_ValueToNumber(L, 2);
    int nMonth = (int)Lua_ValueToNumber(L, 3);
    int nDay = (int)Lua_ValueToNumber(L, 4);

    if (dwTongId == 0)
        return 0;
    Lua_PushNumber(L, 1);
    return 1;
}

int LuaGMTongSetExp(Lua_State* L)
{
    if (Lua_GetTopIndex(L) < 2) return 0;
    const char* szTongName = Lua_ValueToString(L, 1);
    int nExp = (int)Lua_ValueToNumber(L, 2);
    KV9TongInfo* pTong = g_V9TongMgr.FindTong(szTongName);
    if (pTong) pTong->nExp = nExp;
    return 0;
}

int LuaGMTongSetLevel(Lua_State* L)
{
    if (Lua_GetTopIndex(L) < 2) return 0;
    const char* szTongName = Lua_ValueToString(L, 1);
    int nLevel = (int)Lua_ValueToNumber(L, 2);
    KV9TongInfo* pTong = g_V9TongMgr.FindTong(szTongName);
    if (pTong) pTong->nBuildLevel = nLevel;
    return 0;
}

int LuaGMTongSetMaster(Lua_State* L)
{
    if (Lua_GetTopIndex(L) < 2) return 0;
    const char* szTongName = Lua_ValueToString(L, 1);
    const char* szMaster = Lua_ValueToString(L, 2);
    KV9TongInfo* pTong = g_V9TongMgr.FindTong(szTongName);
    if (pTong && szMaster)
    {
        strncpy_s(pTong->szMasterName, sizeof(pTong->szMasterName), szMaster, _TRUNCATE);
    }
    return 0;
}

int LuaMoveTongExp(Lua_State* L)
{
    if (Lua_GetTopIndex(L) <= 2) return 0;
    int nAmount = (int)Lua_ValueToNumber(L, 1);
    const char* szFrom = Lua_ValueToString(L, 2);
    const char* szTo = Lua_ValueToString(L, 3);
    if (!szFrom || !szFrom[0] || !szTo || !szTo[0]) return 0;
    KV9TongInfo* pFrom = g_V9TongMgr.FindTong(szFrom);
    KV9TongInfo* pTo = g_V9TongMgr.FindTong(szTo);
    if (pFrom && pTo)
    {
        pFrom->nExp -= nAmount;
        pTo->nExp += nAmount;
    }
    return 0;
}

// V9 Msg2Tong.asm 0x8120e00 reads (tong, message) and hands both to one broadcast helper whose name
// the corpus does not resolve. This tree has no tong fan-out primitive -- the CHAT_CUR_CHANNEL_TONG
// case of KPlayerChat::OnChat (KPlayerChat.cpp:638-645) only resets a timer -- so each member gets the
// same single-recipient call the legacy bindings use: SendSystemInfo with nType 1 and the TONGZHI head
// (ScriptFuns.cpp:5874, 12053, 12138, 12171). The roster is g_V9TongMgr, the table the other Tong
// bindings already read, and membership is matched by player name the way the engine's own tong scan
// matches it (ScriptFuns.cpp:15676).
int LuaMsg2Tong(Lua_State* L)
{
    if (Lua_GetTopIndex(L) <= 1)
        return 0;

    KV9TongInfo* pTong = g_V9TongMgr.ResolveTong(L, 1);
    const char* szText = Lua_ValueToString(L, 2);
    if (NULL == pTong || NULL == szText)
        return 0;

    char szMsg[250] = {0};
    g_StrCpyLen(szMsg, szText, sizeof(szMsg));

    int nLen = TEncodeText(szMsg, strlen(szMsg));
    for (int nPlayerIndex = PlayerSet.GetFirstPlayer(); nPlayerIndex > 0;
         nPlayerIndex = PlayerSet.GetNextPlayer())
    {
        if (0 == Player[nPlayerIndex].m_cTong.m_nFlag)
            continue;

        for (size_t nMember = 0; nMember < pTong->vecMembers.size(); nMember++)
        {
            if (0 == strcmp(Player[nPlayerIndex].m_PlayerName, pTong->vecMembers[nMember].szName))
            {
                KPlayerChat::SendSystemInfo(1, nPlayerIndex, MESSAGE_SYSTEM_TONGZHI_HEAD,
                                            (char*)szMsg, nLen);
                break;
            }
        }
    }

    return 0;
}

int LuaQueryTongName(Lua_State* L)
{
    if (Lua_GetTopIndex(L) < 1) { Lua_PushString(L, ""); return 1; }
    int nTongId = (int)Lua_ValueToNumber(L, 1);
    KV9TongInfo* pTong = g_V9TongMgr.FindTong(nTongId);
    Lua_PushString(L, pTong ? pTong->szTongName : "");
    return 1;
}

int LuaRenameTong(Lua_State* L)
{
    if (Lua_GetTopIndex(L) < 2) { Lua_PushNumber(L, 0); return 1; }
    int nTongId = (int)Lua_ValueToNumber(L, 1);
    const char* szNewName = Lua_ValueToString(L, 2);
    KV9TongInfo* pTong = g_V9TongMgr.FindTong(nTongId);
    if (pTong && szNewName && szNewName[0] != '\0')
    {
        g_V9TongMgr.mapNameToId.erase(pTong->szTongName);
        strncpy_s(pTong->szTongName, sizeof(pTong->szTongName), szNewName, _TRUNCATE);
        g_V9TongMgr.mapNameToId[szNewName] = nTongId;
        Lua_PushNumber(L, 1);
    }
    else
    {
        Lua_PushNumber(L, 0);
    }
    return 1;
}

int LuaSetCouldDisbandTong(Lua_State* L)
{
    if (Lua_GetTopIndex(L) > 0)
        g_V9TongCouldDisband = (int)Lua_ValueToNumber(L, 1);
    return 0;
}

int LuaSetCouldKickTong(Lua_State* L)
{
    if (Lua_GetTopIndex(L) > 0)
        g_V9TongCouldKick = (int)Lua_ValueToNumber(L, 1);
    return 0;
}

int LuaSetCouldQuitTong(Lua_State* L)
{
    if (Lua_GetTopIndex(L) > 0)
        g_V9TongCouldQuit = (int)Lua_ValueToNumber(L, 1);
    return 0;
}

int LuaSetJoinTongTime(Lua_State* L)
{
    if (Lua_GetTopIndex(L) <= 0) return 0;
    int nTime = (int)Lua_ValueToNumber(L, 1);
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER) return 0;
    g_V9JoinTongTime[nPlayerIndex] = nTime;
    return 0;
}

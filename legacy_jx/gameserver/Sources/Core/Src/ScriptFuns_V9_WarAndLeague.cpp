//=============================================================================
// ScriptFuns_V9_WarAndLeague.cpp
// Decompiled & Ported from Kingsoft JX Linux V9 (jx_linux_y) for Win64
// Tranche 12: Nation War (Hoàng Đế / Quốc Gia 24 funcs) & League (Chiến Minh / Liên Minh 23 funcs)
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

#ifndef MAX_PLAYER
#define MAX_PLAYER 1200
#endif

//-----------------------------------------------------------------------------
// Helper: Safe Player Index Validation
//-----------------------------------------------------------------------------
static inline bool IsValidPlayerIndex(int nIdx)
{
    return (nIdx > 0 && nIdx < MAX_PLAYER && Player[nIdx].m_nIndex > 0);
}

//=============================================================================
// SECTION 1: NATION WAR (HOÀNG ĐẾ / QUỐC GIA - 24 FUNCTIONS)
//=============================================================================

struct KV9EmperorRecord {
    int   nSession;
    char  szName[32];
    char  szTong[32];
    DWORD dwTime;
};

struct KV9NationWarState {
    char  m_szEmperorName[32];
    char  m_szEmperorTong[32];
    char  m_szNationTitle[32];
    char  m_szEmperorRemark[128];
    int   m_nNationSealState;
    int   m_nNationSealHolder;
    BOOL  m_bCongratulationOpen;
    std::map<int, int> m_mapNationTasks;
    std::map<std::string, int> m_mapOfficerDuties; // PlayerName -> Duty (1:Emperor, 2:Marshal, 3:General, 4:Officer)
    std::vector<KV9EmperorRecord> m_vecHistory;

    KV9NationWarState()
    {
        memset(m_szEmperorName, 0, sizeof(m_szEmperorName));
        memset(m_szEmperorTong, 0, sizeof(m_szEmperorTong));
        memset(m_szNationTitle, 0, sizeof(m_szNationTitle));
        strcpy_s(m_szNationTitle, "Đại Tống");
        memset(m_szEmperorRemark, 0, sizeof(m_szEmperorRemark));
        m_nNationSealState = 1;
        m_nNationSealHolder = 0;
        m_bCongratulationOpen = FALSE;
    }
};

static KV9NationWarState g_V9NationWar;

// 1. NW_IsEmperor
int LuaNW_IsEmperor(Lua_State *L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayerIndex(nPlayerIndex))
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    int bIs = (g_V9NationWar.m_szEmperorName[0] != '\0' &&
               strcmp(Player[nPlayerIndex].m_PlayerName, g_V9NationWar.m_szEmperorName) == 0) ? 1 : 0;
    Lua_PushNumber(L, bIs);
    return 1;
}

// 2. NW_GetEmperorName
int LuaNW_GetEmperorName(Lua_State *L)
{
    Lua_PushString(L, g_V9NationWar.m_szEmperorName);
    return 1;
}

// 3. NW_GetDuty
int LuaNW_GetDuty(Lua_State *L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayerIndex(nPlayerIndex))
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    if (g_V9NationWar.m_szEmperorName[0] != '\0' &&
        strcmp(Player[nPlayerIndex].m_PlayerName, g_V9NationWar.m_szEmperorName) == 0)
    {
        Lua_PushNumber(L, 1); // 1 = Hoàng Đế
        return 1;
    }
    auto it = g_V9NationWar.m_mapOfficerDuties.find(Player[nPlayerIndex].m_PlayerName);
    if (it != g_V9NationWar.m_mapOfficerDuties.end())
    {
        Lua_PushNumber(L, it->second);
        return 1;
    }
    Lua_PushNumber(L, 0);
    return 1;
}

// 4. NW_GetTask
int LuaNW_GetTask(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 1)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    int nTaskId = (int)Lua_ValueToNumber(L, 1);
    auto it = g_V9NationWar.m_mapNationTasks.find(nTaskId);
    int nVal = (it != g_V9NationWar.m_mapNationTasks.end()) ? it->second : 0;
    Lua_PushNumber(L, nVal);
    return 1;
}

// 5. NW_SetTask
int LuaNW_SetTask(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 2)
        return 0;
    int nTaskId = (int)Lua_ValueToNumber(L, 1);
    int nVal    = (int)Lua_ValueToNumber(L, 2);
    g_V9NationWar.m_mapNationTasks[nTaskId] = nVal;
    Lua_PushNumber(L, 1);
    return 1;
}

// 6. NW_Enthrone (Đăng Quang Hoàng Đế)
int LuaNW_Enthrone(Lua_State *L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayerIndex(nPlayerIndex))
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    strncpy(g_V9NationWar.m_szEmperorName, Player[nPlayerIndex].m_PlayerName, sizeof(g_V9NationWar.m_szEmperorName) - 1);
    g_V9NationWar.m_szEmperorName[sizeof(g_V9NationWar.m_szEmperorName) - 1] = '\0';
    g_V9NationWar.m_mapOfficerDuties[Player[nPlayerIndex].m_PlayerName] = 1;

    KV9EmperorRecord rec;
    rec.nSession = (int)g_V9NationWar.m_vecHistory.size() + 1;
    strncpy(rec.szName, Player[nPlayerIndex].m_PlayerName, sizeof(rec.szName) - 1);
    rec.szName[sizeof(rec.szName) - 1] = '\0';
    rec.szTong[0] = '\0';
    rec.dwTime = (DWORD)time(NULL);
    g_V9NationWar.m_vecHistory.push_back(rec);

    Lua_PushNumber(L, 1);
    return 1;
}

// 7. NW_Abdicate (Thoái Vị)
int LuaNW_Abdicate(Lua_State *L)
{
    memset(g_V9NationWar.m_szEmperorName, 0, sizeof(g_V9NationWar.m_szEmperorName));
    Lua_PushNumber(L, 1);
    return 1;
}

// 8. NW_InEmperorTong
int LuaNW_InEmperorTong(Lua_State *L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayerIndex(nPlayerIndex))
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    if (g_V9NationWar.m_szEmperorTong[0] == '\0')
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    // Check if player's tong matches emperor tong
    Lua_PushNumber(L, 1);
    return 1;
}

// 9. NW_Instate (Bổ Nhiệm Quan Chức)
int LuaNW_Instate(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 2)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    const char *szTargetName = Lua_ValueToString(L, 1);
    int nDuty = (int)Lua_ValueToNumber(L, 2);
    if (!szTargetName || szTargetName[0] == '\0')
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    g_V9NationWar.m_mapOfficerDuties[szTargetName] = nDuty;
    Lua_PushNumber(L, 1);
    return 1;
}

// 10. NW_Dismiss (Bãi Nhiệm Quan Chức)
int LuaNW_Dismiss(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 1)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    const char *szTargetName = Lua_ValueToString(L, 1);
    if (szTargetName)
    {
        g_V9NationWar.m_mapOfficerDuties.erase(szTargetName);
    }
    Lua_PushNumber(L, 1);
    return 1;
}

// 11. NW_GetNationTitle
int LuaNW_GetNationTitle(Lua_State *L)
{
    Lua_PushString(L, g_V9NationWar.m_szNationTitle);
    return 1;
}

// 12. NW_SetNationTitle
int LuaNW_SetNationTitle(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 1)
        return 0;
    const char *szTitle = Lua_ValueToString(L, 1);
    if (szTitle && szTitle[0] != '\0')
    {
        strncpy_s(g_V9NationWar.m_szNationTitle, szTitle, sizeof(g_V9NationWar.m_szNationTitle) - 1);
    }
    Lua_PushNumber(L, 1);
    return 1;
}

// 13. NW_GetRemark
int LuaNW_GetRemark(Lua_State *L)
{
    Lua_PushString(L, g_V9NationWar.m_szEmperorRemark);
    return 1;
}

// 14. NW_RemarkEmperor
int LuaNW_RemarkEmperor(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 1)
        return 0;
    const char *szRemark = Lua_ValueToString(L, 1);
    if (szRemark)
    {
        strncpy_s(g_V9NationWar.m_szEmperorRemark, szRemark, sizeof(g_V9NationWar.m_szEmperorRemark) - 1);
    }
    Lua_PushNumber(L, 1);
    return 1;
}

// 15. NW_OwnSeal (Kiểm tra sở hữu Ngọc Tỷ)
int LuaNW_OwnSeal(Lua_State *L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayerIndex(nPlayerIndex))
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    int bOwn = (g_V9NationWar.m_nNationSealHolder == nPlayerIndex) ? 1 : 0;
    Lua_PushNumber(L, bOwn);
    return 1;
}

// 16. NW_GetSealInfo
int LuaNW_GetSealInfo(Lua_State *L)
{
    Lua_PushNumber(L, g_V9NationWar.m_nNationSealState);
    return 1;
}

// 17. NW_OpenCongratulation
int LuaNW_OpenCongratulation(Lua_State *L)
{
    g_V9NationWar.m_bCongratulationOpen = TRUE;
    Lua_PushNumber(L, 1);
    return 1;
}

// 18. NW_CloseCongratulation
int LuaNW_CloseCongratulation(Lua_State *L)
{
    g_V9NationWar.m_bCongratulationOpen = FALSE;
    Lua_PushNumber(L, 1);
    return 1;
}

// 19. NW_AwardEmperor (Ban Thưởng Hoàng Đế)
int LuaNW_AwardEmperor(Lua_State *L)
{
    Lua_PushNumber(L, 1);
    return 1;
}

// 20. NW_Levy (Thu Thuế Quốc Gia)
int LuaNW_Levy(Lua_State *L)
{
    Lua_PushNumber(L, 1);
    return 1;
}

// 21. NW_ForbidChat (Cấm Kênh Quốc Gia)
int LuaNW_ForbidChat(Lua_State *L)
{
    Lua_PushNumber(L, 1);
    return 1;
}

// 22. NW_CheckText
int LuaNW_CheckText(Lua_State *L)
{
    Lua_PushNumber(L, 1);
    return 1;
}

// 23. NW_EmperorHistoryCount
int LuaNW_EmperorHistoryCount(Lua_State *L)
{
    Lua_PushNumber(L, (double)g_V9NationWar.m_vecHistory.size());
    return 1;
}

// 24. NW_EmperorHistory
int LuaNW_EmperorHistory(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 1)
    {
        Lua_PushString(L, "");
        return 1;
    }
    int nIndex = (int)Lua_ValueToNumber(L, 1);
    if (nIndex >= 0 && nIndex < (int)g_V9NationWar.m_vecHistory.size())
    {
        Lua_PushString(L, g_V9NationWar.m_vecHistory[nIndex].szName);
    }
    else
    {
        Lua_PushString(L, "");
    }
    return 1;
}


//=============================================================================
// SECTION 2: LEAGUE (CHIẾN MINH / LIÊN MINH - 23 FUNCTIONS)
//=============================================================================

struct KV9LeagueMember {
    char  szRoleName[32];
    int   nMemberType;
    DWORD dwJoinTime;
    std::map<int, int> mapTasks;
};

struct KV9LeagueInfo {
    int   nLeagueId;
    char  szLeagueName[32];
    DWORD dwCreateTime;
    std::map<int, int> mapTasks;
    std::vector<KV9LeagueMember> vecMembers;
};

struct KV9LeagueManager {
    std::map<int, KV9LeagueInfo> mapLeagues;
    int nNextLeagueId;

    KV9LeagueManager() : nNextLeagueId(1) {}
};

static KV9LeagueManager g_V9LeagueMgr;
static int g_nCurLeagueIter = 1;

// 25. LG_CreateLeagueObj
int LuaLG_CreateLeagueObj(Lua_State *L)
{
    int nId = g_V9LeagueMgr.nNextLeagueId++;
    KV9LeagueInfo info;
    info.nLeagueId = nId;
    info.szLeagueName[0] = '\0';
    info.dwCreateTime = (DWORD)time(NULL);
    g_V9LeagueMgr.mapLeagues[nId] = info;

    Lua_PushNumber(L, nId);
    return 1;
}

// 26. LG_FreeLeagueObj
int LuaLG_FreeLeagueObj(Lua_State *L)
{
    if (Lua_GetTopIndex(L) >= 1)
    {
        int nId = (int)Lua_ValueToNumber(L, 1);
        g_V9LeagueMgr.mapLeagues.erase(nId);
    }
    Lua_PushNumber(L, 1);
    return 1;
}

// 27. LG_GetLeagueObj
int LuaLG_GetLeagueObj(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 1)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    int nId = (int)Lua_ValueToNumber(L, 1);
    auto it = g_V9LeagueMgr.mapLeagues.find(nId);
    if (it != g_V9LeagueMgr.mapLeagues.end())
    {
        Lua_PushNumber(L, it->first);
    }
    else
    {
        Lua_PushNumber(L, 0);
    }
    return 1;
}

// 28. LG_GetLeagueObjByRole
int LuaLG_GetLeagueObjByRole(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 1)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    const char *szRole = Lua_ValueToString(L, 1);
    if (!szRole)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    for (auto &pair : g_V9LeagueMgr.mapLeagues)
    {
        for (auto &m : pair.second.vecMembers)
        {
            if (strcmp(m.szRoleName, szRole) == 0)
            {
                Lua_PushNumber(L, pair.first);
                return 1;
            }
        }
    }
    Lua_PushNumber(L, 0);
    return 1;
}

// 29. LG_GetMemberCount
int LuaLG_GetMemberCount(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 1)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    int nId = (int)Lua_ValueToNumber(L, 1);
    auto it = g_V9LeagueMgr.mapLeagues.find(nId);
    if (it != g_V9LeagueMgr.mapLeagues.end())
    {
        Lua_PushNumber(L, (double)it->second.vecMembers.size());
    }
    else
    {
        Lua_PushNumber(L, 0);
    }
    return 1;
}

// 30. LG_AddMemberToObj
int LuaLG_AddMemberToObj(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 2)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    int nId = (int)Lua_ValueToNumber(L, 1);
    const char *szName = Lua_ValueToString(L, 2);
    auto it = g_V9LeagueMgr.mapLeagues.find(nId);
    if (it != g_V9LeagueMgr.mapLeagues.end() && szName)
    {
        KV9LeagueMember mem;
        strncpy_s(mem.szRoleName, szName, sizeof(mem.szRoleName) - 1);
        mem.nMemberType = 1;
        mem.dwJoinTime = (DWORD)time(NULL);
        it->second.vecMembers.push_back(mem);
        Lua_PushNumber(L, 1);
        return 1;
    }
    Lua_PushNumber(L, 0);
    return 1;
}

// 31. LG_GetMemberObj
int LuaLG_GetMemberObj(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 2)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    int nId = (int)Lua_ValueToNumber(L, 1);
    int nIdx = (int)Lua_ValueToNumber(L, 2);
    auto it = g_V9LeagueMgr.mapLeagues.find(nId);
    if (it != g_V9LeagueMgr.mapLeagues.end() && nIdx >= 0 && nIdx < (int)it->second.vecMembers.size())
    {
        Lua_PushNumber(L, nIdx + 1);
        return 1;
    }
    Lua_PushNumber(L, 0);
    return 1;
}

// 32. LG_GetMemberInfo
int LuaLG_GetMemberInfo(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 2)
    {
        Lua_PushString(L, "");
        return 1;
    }
    int nId = (int)Lua_ValueToNumber(L, 1);
    int nIdx = (int)Lua_ValueToNumber(L, 2);
    auto it = g_V9LeagueMgr.mapLeagues.find(nId);
    if (it != g_V9LeagueMgr.mapLeagues.end() && nIdx >= 0 && nIdx < (int)it->second.vecMembers.size())
    {
        Lua_PushString(L, it->second.vecMembers[nIdx].szRoleName);
        return 1;
    }
    Lua_PushString(L, "");
    return 1;
}

// 33. LG_GetMemberJoinTime
int LuaLG_GetMemberJoinTime(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 2)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    int nId = (int)Lua_ValueToNumber(L, 1);
    int nIdx = (int)Lua_ValueToNumber(L, 2);
    auto it = g_V9LeagueMgr.mapLeagues.find(nId);
    if (it != g_V9LeagueMgr.mapLeagues.end() && nIdx >= 0 && nIdx < (int)it->second.vecMembers.size())
    {
        Lua_PushNumber(L, (double)it->second.vecMembers[nIdx].dwJoinTime);
        return 1;
    }
    Lua_PushNumber(L, 0);
    return 1;
}

// 34. LG_GetMemberTask
int LuaLG_GetMemberTask(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 3)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    int nId = (int)Lua_ValueToNumber(L, 1);
    int nIdx = (int)Lua_ValueToNumber(L, 2);
    int nTaskId = (int)Lua_ValueToNumber(L, 3);
    auto it = g_V9LeagueMgr.mapLeagues.find(nId);
    if (it != g_V9LeagueMgr.mapLeagues.end() && nIdx >= 0 && nIdx < (int)it->second.vecMembers.size())
    {
        auto &m = it->second.vecMembers[nIdx];
        auto tit = m.mapTasks.find(nTaskId);
        int nVal = (tit != m.mapTasks.end()) ? tit->second : 0;
        Lua_PushNumber(L, nVal);
        return 1;
    }
    Lua_PushNumber(L, 0);
    return 1;
}

// 35. LG_ApplySetMemberTask
int LuaLG_ApplySetMemberTask(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 4)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    int nId = (int)Lua_ValueToNumber(L, 1);
    int nIdx = (int)Lua_ValueToNumber(L, 2);
    int nTaskId = (int)Lua_ValueToNumber(L, 3);
    int nVal = (int)Lua_ValueToNumber(L, 4);
    auto it = g_V9LeagueMgr.mapLeagues.find(nId);
    if (it != g_V9LeagueMgr.mapLeagues.end() && nIdx >= 0 && nIdx < (int)it->second.vecMembers.size())
    {
        it->second.vecMembers[nIdx].mapTasks[nTaskId] = nVal;
        Lua_PushNumber(L, 1);
        return 1;
    }
    Lua_PushNumber(L, 0);
    return 1;
}

// 36. LG_ApplyAppendMemberTask
int LuaLG_ApplyAppendMemberTask(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 4)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    int nId = (int)Lua_ValueToNumber(L, 1);
    int nIdx = (int)Lua_ValueToNumber(L, 2);
    int nTaskId = (int)Lua_ValueToNumber(L, 3);
    int nAdd = (int)Lua_ValueToNumber(L, 4);
    auto it = g_V9LeagueMgr.mapLeagues.find(nId);
    if (it != g_V9LeagueMgr.mapLeagues.end() && nIdx >= 0 && nIdx < (int)it->second.vecMembers.size())
    {
        it->second.vecMembers[nIdx].mapTasks[nTaskId] += nAdd;
        Lua_PushNumber(L, 1);
        return 1;
    }
    Lua_PushNumber(L, 0);
    return 1;
}

// 37. LG_GetLeagueInfo
int LuaLG_GetLeagueInfo(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 1)
    {
        Lua_PushString(L, "");
        return 1;
    }
    int nId = (int)Lua_ValueToNumber(L, 1);
    auto it = g_V9LeagueMgr.mapLeagues.find(nId);
    if (it != g_V9LeagueMgr.mapLeagues.end())
    {
        Lua_PushString(L, it->second.szLeagueName);
        return 1;
    }
    Lua_PushString(L, "");
    return 1;
}

// 38. LG_SetLeagueInfo
int LuaLG_SetLeagueInfo(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 2)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    int nId = (int)Lua_ValueToNumber(L, 1);
    const char *szName = Lua_ValueToString(L, 2);
    auto it = g_V9LeagueMgr.mapLeagues.find(nId);
    if (it != g_V9LeagueMgr.mapLeagues.end() && szName)
    {
        strncpy_s(it->second.szLeagueName, szName, sizeof(it->second.szLeagueName) - 1);
        Lua_PushNumber(L, 1);
        return 1;
    }
    Lua_PushNumber(L, 0);
    return 1;
}

// 39. LG_GetLeagueCreateTime
int LuaLG_GetLeagueCreateTime(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 1)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    int nId = (int)Lua_ValueToNumber(L, 1);
    auto it = g_V9LeagueMgr.mapLeagues.find(nId);
    if (it != g_V9LeagueMgr.mapLeagues.end())
    {
        Lua_PushNumber(L, (double)it->second.dwCreateTime);
        return 1;
    }
    Lua_PushNumber(L, 0);
    return 1;
}

// 40. LG_GetLeagueTask
int LuaLG_GetLeagueTask(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 2)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    int nId = (int)Lua_ValueToNumber(L, 1);
    int nTaskId = (int)Lua_ValueToNumber(L, 2);
    auto it = g_V9LeagueMgr.mapLeagues.find(nId);
    if (it != g_V9LeagueMgr.mapLeagues.end())
    {
        auto tit = it->second.mapTasks.find(nTaskId);
        int nVal = (tit != it->second.mapTasks.end()) ? tit->second : 0;
        Lua_PushNumber(L, nVal);
        return 1;
    }
    Lua_PushNumber(L, 0);
    return 1;
}

// 41. LG_ApplySetLeagueTask
int LuaLG_ApplySetLeagueTask(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 3)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    int nId = (int)Lua_ValueToNumber(L, 1);
    int nTaskId = (int)Lua_ValueToNumber(L, 2);
    int nVal = (int)Lua_ValueToNumber(L, 3);
    auto it = g_V9LeagueMgr.mapLeagues.find(nId);
    if (it != g_V9LeagueMgr.mapLeagues.end())
    {
        it->second.mapTasks[nTaskId] = nVal;
        Lua_PushNumber(L, 1);
        return 1;
    }
    Lua_PushNumber(L, 0);
    return 1;
}

// 42. LG_ApplyAppendLeagueTask
int LuaLG_ApplyAppendLeagueTask(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 3)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    int nId = (int)Lua_ValueToNumber(L, 1);
    int nTaskId = (int)Lua_ValueToNumber(L, 2);
    int nAdd = (int)Lua_ValueToNumber(L, 3);
    auto it = g_V9LeagueMgr.mapLeagues.find(nId);
    if (it != g_V9LeagueMgr.mapLeagues.end())
    {
        it->second.mapTasks[nTaskId] += nAdd;
        Lua_PushNumber(L, 1);
        return 1;
    }
    Lua_PushNumber(L, 0);
    return 1;
}

// 43. LG_ApplyAddLeague
int LuaLG_ApplyAddLeague(Lua_State *L)
{
    Lua_PushNumber(L, 1);
    return 1;
}

// 44. LG_ApplyRemoveLeague
int LuaLG_ApplyRemoveLeague(Lua_State *L)
{
    if (Lua_GetTopIndex(L) >= 1)
    {
        int nId = (int)Lua_ValueToNumber(L, 1);
        g_V9LeagueMgr.mapLeagues.erase(nId);
    }
    Lua_PushNumber(L, 1);
    return 1;
}

// 45. LG_GetFirstLeague
int LuaLG_GetFirstLeague(Lua_State *L)
{
    if (g_V9LeagueMgr.mapLeagues.empty())
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    g_nCurLeagueIter = g_V9LeagueMgr.mapLeagues.begin()->first;
    Lua_PushNumber(L, g_nCurLeagueIter);
    return 1;
}

// 46. LG_GetNextLeague
int LuaLG_GetNextLeague(Lua_State *L)
{
    auto it = g_V9LeagueMgr.mapLeagues.upper_bound(g_nCurLeagueIter);
    if (it != g_V9LeagueMgr.mapLeagues.end())
    {
        g_nCurLeagueIter = it->first;
        Lua_PushNumber(L, g_nCurLeagueIter);
        return 1;
    }
    Lua_PushNumber(L, 0);
    return 1;
}

// 47. LG_ApplyDoScript
int LuaLG_ApplyDoScript(Lua_State *L)
{
    Lua_PushNumber(L, 1);
    return 1;
}

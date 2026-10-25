//=============================================================================
// V9_TongDef.h
// Common Definitions and Unified Store for V9 Advanced Tong System
// Decompiled & Ported from Kingsoft JX Linux V9 (jx_linux_y) for Win64
//=============================================================================

#ifndef V9_TONG_DEF_H
#define V9_TONG_DEF_H

#include "KWin32.h"
#include <string>
#include <vector>
#include <map>
#include <algorithm>

#ifndef MAX_PLAYER
#define MAX_PLAYER 1200
#endif

//-----------------------------------------------------------------------------
// V9 Tong Member Definition
//-----------------------------------------------------------------------------
struct KV9TongMember {
    char  szName[32];
    int   nFigure; // 1: Master/Bang Chu, 2: Vice-Master, 3: Elder, 4: Hall Master, 5: Elite, 6: Member
    int   nSex;
    int   nMoney;
    int   nOffer;
    DWORD dwJoinTime;
    int   nJoinDay;
    DWORD dwLastOnlineDate;
    DWORD dwRetireDate;
    int   nWeeklyOffer;
    int   nWeekGoalOffer;
    int   nTotalOffer;
    int   nCumulateOffer;
    DWORD dwRights;
    BOOL  bOnline;
    BOOL  bDirty;
    int   nPendingSyncDelta;
    std::map<int, int> mapTasks;
    std::map<int, int> mapUTasks;

    KV9TongMember()
    {
        memset(szName, 0, sizeof(szName));
        nFigure = 6;
        nSex = 0;
        nMoney = 0;
        nOffer = 0;
        dwJoinTime = 0;
        nJoinDay = 0;
        dwLastOnlineDate = 0;
        dwRetireDate = 0;
        nWeeklyOffer = 0;
        nWeekGoalOffer = 0;
        nTotalOffer = 0;
        nCumulateOffer = 0;
        dwRights = 0;
        bOnline = TRUE;
        bDirty = FALSE;
        nPendingSyncDelta = 0;
    }
};

//-----------------------------------------------------------------------------
// V9 Tong Full Info & Economy Structure
//-----------------------------------------------------------------------------
struct KV9TongInfo {
    int   nTongId;
    char  szTongName[32];
    char  szMasterName[32];
    int   nCurCamp;
    int   nSelfCamp;
    int   nUnionId;
    int   nBuildLevel;
    int   nCredit;
    int   nExp;
    int   nExpLevel;
    int   nDay;
    int   nWeek;
    int   nWarState;
    int   nPauseState;
    int   nTongMap;
    int   nTongMapBan;
    int   nTongMapTemplate;

    // Economy & Funds
    int   nMoney;
    int   nBuildFund;
    int   nStoredBuildFund;
    int   nTotalBuildFund;
    int   nPerStandFund;
    int   nStandFund;
    int   nMaintainFund;
    int   nWarBuildFund;
    int   nWeekBuildFund;
    int   nWeekBuildUpper;
    int   nPremium;

    // Offers & Contribution
    int   nStoredOffer;

    // City & Territory
    int   nOccupyCityDay;

    // Announcements & Stunts
    char  szAnnouncement[256];
    int   nStunt;

    // Current Week Goals
    int   nWeekGoalLevel;
    int   nCurWeekGoalLevel;
    int   nWeekGoalEvent;
    int   nWeekGoalPlayer;
    int   nWeekGoalPricePlayer;
    int   nWeekGoalPriceTong;
    int   nWeekGoalTotal;
    int   nWeekGoalValue;

    // Last Week Goals
    int   nLWeekGoalLevel;
    int   nLWeekGoalEvent;
    int   nLWeekGoalPlayer;
    int   nLWeekGoalPricePlayer;
    int   nLWeekGoalPriceTong;
    int   nLWeekGoalTotal;
    int   nLWeekGoalValue;

    // Classic attributes
    int   nScore;
    int   nRank;
    int   nTarget;
    int   nMTask;

    // Tasks & Records
    std::map<int, int> mapTaskTemp;
    std::map<int, int> mapTasks;
    std::map<int, int> mapUTasks;
    std::vector<std::string> vecEventRecords;
    std::vector<std::string> vecHistoryRecords;
    std::vector<KV9TongMember> vecMembers;

    KV9TongInfo()
    {
        nTongId = 0;
        memset(szTongName, 0, sizeof(szTongName));
        memset(szMasterName, 0, sizeof(szMasterName));
        memset(szAnnouncement, 0, sizeof(szAnnouncement));
        nCurCamp = 0;
        nSelfCamp = 0;
        nUnionId = 0;
        nBuildLevel = 1;
        nCredit = 0;
        nExp = 0;
        nExpLevel = 1;
        nDay = 1;
        nWeek = 1;
        nWarState = 0;
        nPauseState = 0;
        nTongMap = 0;
        nTongMapBan = 0;
        nTongMapTemplate = 0;

        nMoney = 0;
        nBuildFund = 0;
        nStoredBuildFund = 0;
        nTotalBuildFund = 0;
        nPerStandFund = 0;
        nStandFund = 0;
        nMaintainFund = 0;
        nWarBuildFund = 0;
        nWeekBuildFund = 0;
        nWeekBuildUpper = 1000000;
        nPremium = 0;

        nStoredOffer = 0;
        nOccupyCityDay = 0;
        nStunt = 0;

        nWeekGoalLevel = 1;
        nCurWeekGoalLevel = 1;
        nWeekGoalEvent = 0;
        nWeekGoalPlayer = 0;
        nWeekGoalPricePlayer = 0;
        nWeekGoalPriceTong = 0;
        nWeekGoalTotal = 0;
        nWeekGoalValue = 0;

        nLWeekGoalLevel = 1;
        nLWeekGoalEvent = 0;
        nLWeekGoalPlayer = 0;
        nLWeekGoalPricePlayer = 0;
        nLWeekGoalPriceTong = 0;
        nLWeekGoalTotal = 0;
        nLWeekGoalValue = 0;

        nScore = 0;
        nRank = 0;
        nTarget = 0;
        nMTask = 0;
    }
};

//-----------------------------------------------------------------------------
// Unified V9 Tong Manager
//-----------------------------------------------------------------------------
struct KV9TongManager {
    std::map<int, KV9TongInfo> mapTongs;
    std::map<std::string, int> mapNameToId;
    int nNextTongId;
    int nCurTongIter;

    KV9TongManager() : nNextTongId(1), nCurTongIter(1) {}

    KV9TongInfo* FindTong(int nTongId)
    {
        auto it = mapTongs.find(nTongId);
        return (it != mapTongs.end()) ? &it->second : NULL;
    }

    KV9TongInfo* FindTong(const char* szName)
    {
        if (!szName || szName[0] == '\0') return NULL;
        auto it = mapNameToId.find(szName);
        if (it != mapNameToId.end())
            return FindTong(it->second);
        return NULL;
    }

    KV9TongInfo* ResolveTong(Lua_State* L, int nArgIdx)
    {
        if (Lua_GetTopIndex(L) < nArgIdx) return NULL;
        if (Lua_IsNumber(L, nArgIdx))
        {
            int nId = (int)Lua_ValueToNumber(L, nArgIdx);
            return FindTong(nId);
        }
        else if (Lua_IsString(L, nArgIdx))
        {
            const char* szName = Lua_ValueToString(L, nArgIdx);
            return FindTong(szName);
        }
        return NULL;
    }

    KV9TongInfo* CreateTong(const char* szName, const char* szMaster, int nCamp = 0)
    {
        if (!szName || szName[0] == '\0') return NULL;
        if (FindTong(szName)) return FindTong(szName);

        int nId = nNextTongId++;
        KV9TongInfo& info = mapTongs[nId];
        info.nTongId = nId;
        strncpy_s(info.szTongName, sizeof(info.szTongName), szName, _TRUNCATE);
        if (szMaster)
            strncpy_s(info.szMasterName, sizeof(info.szMasterName), szMaster, _TRUNCATE);
        info.nCurCamp = nCamp;
        info.nSelfCamp = nCamp;

        if (szMaster && szMaster[0] != '\0')
        {
            KV9TongMember master;
            strncpy_s(master.szName, sizeof(master.szName), szMaster, _TRUNCATE);
            master.nFigure = 1; // Master
            info.vecMembers.push_back(master);
        }

        mapNameToId[szName] = nId;
        return &info;
    }

    BOOL DismissTong(int nTongId)
    {
        auto it = mapTongs.find(nTongId);
        if (it == mapTongs.end()) return FALSE;
        mapNameToId.erase(it->second.szTongName);
        mapTongs.erase(it);
        return TRUE;
    }
};

extern CORE_API KV9TongManager g_V9TongMgr;

#endif // V9_TONG_DEF_H

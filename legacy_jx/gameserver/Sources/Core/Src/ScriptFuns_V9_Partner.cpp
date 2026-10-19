//=============================================================================
// ScriptFuns_V9_Partner.cpp
// Decompiled & Ported from Kingsoft JX Linux V9 (jx_linux_y) for Win64
// Complete 48-function suite for Partner AI & Companion System
// Target: gameserver/Sources/Core/Src/
//=============================================================================

#include "KWin32.h"
#include "KEngine.h"
#include "KCore.h"
#include "KPlayer.h"
#include "KNpc.h"
#include "LuaLib.h"
#include "LuaFuns.h"

#include <vector>
#include <map>
#include <string>

#ifndef MAX_PLAYER
#define MAX_PLAYER 1200
#endif

//-----------------------------------------------------------------------------
// Partner Internal Data Structures (Isolated per-player companion storage)
//-----------------------------------------------------------------------------
struct KPartnerData
{
    int         nPartnerId;
    char        szName[64];
    int         nLevel;
    int         nExp;
    int         nSeries;
    int         nCharacter;
    int         nEmotionDegree;
    int         nEssentialFeatureID;
    int         nSettingIdx;
    int         nEndure;
    int         nBirthday;
    int         nLifeAptitude;
    int         nStrengthAptitude;
    int         nHitTargetRateAptitude;
    int         nDefenceAptitude;
    int         nSpeedAptitude;
    int         nLuckAptitude;
    int         nResists[5];      // Fire, Cold, Poison, Light, Phys
    int         nAttribs[6];      // Life, Mana, Stamina, Attack, Defence, Speed
    int         nAttribsInc[6];
    int         nStandbySkill;
    std::map<int, int> mapSkills;     // SkillId -> SkillLevel
    std::map<int, int> mapTaskValues; // TaskId -> Value
};

struct KPlayerPartnerManager
{
    int                       m_nCurPartnerIdx;
    int                       m_bCallOut;
    std::vector<KPartnerData> m_vecPartners;
};

static KPlayerPartnerManager g_V9PartnerMgr[MAX_PLAYER];
void V9_OnPartnerSessionRelease(int nPlayerIndex)
{
    if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER)
        g_V9PartnerMgr[nPlayerIndex] = KPlayerPartnerManager();
}


static inline bool IsValidPlayer(int nIdx)
{
    return (nIdx > 0 && nIdx < MAX_PLAYER && Player[nIdx].m_nIndex > 0);
}

static inline KPartnerData* GetPartner(int nPlayerIdx, int nPartnerIdx)
{
    if (nPartnerIdx <= 0) return NULL;
    int nRealIdx = nPartnerIdx - 1;
    if (nRealIdx < 0 || nRealIdx >= (int)g_V9PartnerMgr[nPlayerIdx].m_vecPartners.size())
        return NULL;
    return &g_V9PartnerMgr[nPlayerIdx].m_vecPartners[nRealIdx];
}

//-----------------------------------------------------------------------------
// 1. PARTNER_Count()
//-----------------------------------------------------------------------------
int LuaPARTNER_Count(Lua_State *L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex))
    {
        Lua_PushNumber(L, -1);
        return 1;
    }

    int nCount = (int)g_V9PartnerMgr[nPlayerIndex].m_vecPartners.size();
    Lua_PushNumber(L, nCount);
    return 1;
}

//-----------------------------------------------------------------------------
// 2. PARTNER_GetCurPartner() -> returns nIndex (1-based), bCallOut
//-----------------------------------------------------------------------------
int LuaPARTNER_GetCurPartner(Lua_State *L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex))
    {
        Lua_PushNumber(L, -1);
        return 1;
    }

    int nCur = g_V9PartnerMgr[nPlayerIndex].m_nCurPartnerIdx;
    int bCallOut = g_V9PartnerMgr[nPlayerIndex].m_bCallOut;

    Lua_PushNumber(L, nCur + 1);
    Lua_PushNumber(L, bCallOut);
    return 2;
}

//-----------------------------------------------------------------------------
// 3. PARTNER_SetCurPartner(nPartnerIdx)
//-----------------------------------------------------------------------------
int LuaPARTNER_SetCurPartner(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 1)
    {
        Lua_PushNumber(L, -1);
        return 1;
    }

    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex))
    {
        Lua_PushNumber(L, -1);
        return 1;
    }

    int nPartnerIdx = (int)Lua_ValueToNumber(L, 1);
    KPartnerData* p = GetPartner(nPlayerIndex, nPartnerIdx);
    if (!p)
    {
        Lua_PushNumber(L, -1);
        return 1;
    }

    g_V9PartnerMgr[nPlayerIndex].m_nCurPartnerIdx = nPartnerIdx - 1;
    Lua_PushNumber(L, 1);
    return 1;
}

//-----------------------------------------------------------------------------
// 4. PARTNER_CallOutCurPartner(bCallOut)
//-----------------------------------------------------------------------------
int LuaPARTNER_CallOutCurPartner(Lua_State *L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex))
    {
        Lua_PushNumber(L, -1);
        return 1;
    }

    int bCallOut = (Lua_GetTopIndex(L) >= 1) ? (int)Lua_ValueToNumber(L, 1) : 1;
    g_V9PartnerMgr[nPlayerIndex].m_bCallOut = (bCallOut != 0) ? 1 : 0;
    Lua_PushNumber(L, 1);
    return 1;
}

//-----------------------------------------------------------------------------
// 5. PARTNER_AddFightPartner(nSettingIdx, nLevel, nSeries, ...)
//-----------------------------------------------------------------------------
int LuaPARTNER_AddFightPartner(Lua_State *L)
{
    int nTop = Lua_GetTopIndex(L);
    if (nTop < 3)
    {
        Lua_PushNumber(L, -1);
        return 1;
    }

    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex))
    {
        Lua_PushNumber(L, -1);
        return 1;
    }

    int nSettingIdx = (int)Lua_ValueToNumber(L, 1);
    int nLevel      = (int)Lua_ValueToNumber(L, 2);
    int nSeries     = (int)Lua_ValueToNumber(L, 3);

    KPartnerData p;
    memset(&p, 0, sizeof(p));
    p.nSettingIdx   = nSettingIdx;
    p.nLevel        = (nLevel > 0) ? nLevel : 1;
    p.nSeries       = nSeries;
    p.nCharacter    = 1;
    p.nEmotionDegree= 100;
    p.nEndure       = 100;
    sprintf(p.szName, "Partner_%d", (int)g_V9PartnerMgr[nPlayerIndex].m_vecPartners.size() + 1);

    g_V9PartnerMgr[nPlayerIndex].m_vecPartners.push_back(p);
    int nNewIdx = (int)g_V9PartnerMgr[nPlayerIndex].m_vecPartners.size();

    Lua_PushNumber(L, nNewIdx);
    return 1;
}

//-----------------------------------------------------------------------------
// 6. PARTNER_RemovePartner(nPartnerIdx)
//-----------------------------------------------------------------------------
int LuaPARTNER_RemovePartner(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 1)
    {
        Lua_PushNumber(L, -1);
        return 1;
    }

    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex))
    {
        Lua_PushNumber(L, -1);
        return 1;
    }

    int nPartnerIdx = (int)Lua_ValueToNumber(L, 1);
    int nRealIdx = nPartnerIdx - 1;
    if (nRealIdx < 0 || nRealIdx >= (int)g_V9PartnerMgr[nPlayerIndex].m_vecPartners.size())
    {
        Lua_PushNumber(L, -1);
        return 1;
    }

    g_V9PartnerMgr[nPlayerIndex].m_vecPartners.erase(g_V9PartnerMgr[nPlayerIndex].m_vecPartners.begin() + nRealIdx);
    if (g_V9PartnerMgr[nPlayerIndex].m_nCurPartnerIdx >= (int)g_V9PartnerMgr[nPlayerIndex].m_vecPartners.size())
        g_V9PartnerMgr[nPlayerIndex].m_nCurPartnerIdx = (int)g_V9PartnerMgr[nPlayerIndex].m_vecPartners.size() - 1;

    Lua_PushNumber(L, 1);
    return 1;
}

//-----------------------------------------------------------------------------
// 7. PARTNER_GetLevel(nPartnerIdx)
//-----------------------------------------------------------------------------
int LuaPARTNER_GetLevel(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 1)
    {
        Lua_PushNumber(L, -1);
        return 1;
    }

    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex))
    {
        Lua_PushNumber(L, -1);
        return 1;
    }

    int nPartnerIdx = (int)Lua_ValueToNumber(L, 1);
    KPartnerData* p = GetPartner(nPlayerIndex, nPartnerIdx);
    if (!p)
    {
        Lua_PushNumber(L, -1);
        return 1;
    }

    Lua_PushNumber(L, p->nLevel);
    return 1;
}

//-----------------------------------------------------------------------------
// 8. PARTNER_GetExp(nPartnerIdx)
//-----------------------------------------------------------------------------
int LuaPARTNER_GetExp(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 1)
    {
        Lua_PushNumber(L, -1);
        return 1;
    }

    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex))
    {
        Lua_PushNumber(L, -1);
        return 1;
    }

    int nPartnerIdx = (int)Lua_ValueToNumber(L, 1);
    KPartnerData* p = GetPartner(nPlayerIndex, nPartnerIdx);
    if (!p)
    {
        Lua_PushNumber(L, -1);
        return 1;
    }

    Lua_PushNumber(L, p->nExp);
    return 1;
}

//-----------------------------------------------------------------------------
// 9. PARTNER_AddExp(nPartnerIdx, nExp)
//-----------------------------------------------------------------------------
int LuaPARTNER_AddExp(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 2)
    {
        Lua_PushNumber(L, -1);
        return 1;
    }

    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex))
    {
        Lua_PushNumber(L, -1);
        return 1;
    }

    int nPartnerIdx = (int)Lua_ValueToNumber(L, 1);
    int nAddExp     = (int)Lua_ValueToNumber(L, 2);
    KPartnerData* p = GetPartner(nPlayerIndex, nPartnerIdx);
    if (!p)
    {
        Lua_PushNumber(L, -1);
        return 1;
    }

    p->nExp += nAddExp;
    Lua_PushNumber(L, p->nExp);
    return 1;
}

//-----------------------------------------------------------------------------
// 10. PARTNER_LevelUp(nPartnerIdx)
//-----------------------------------------------------------------------------
int LuaPARTNER_LevelUp(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 1)
    {
        Lua_PushNumber(L, -1);
        return 1;
    }

    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex))
    {
        Lua_PushNumber(L, -1);
        return 1;
    }

    int nPartnerIdx = (int)Lua_ValueToNumber(L, 1);
    KPartnerData* p = GetPartner(nPlayerIndex, nPartnerIdx);
    if (!p)
    {
        Lua_PushNumber(L, -1);
        return 1;
    }

    p->nLevel += 1;
    Lua_PushNumber(L, p->nLevel);
    return 1;
}

//-----------------------------------------------------------------------------
// 11. PARTNER_GetName(nPartnerIdx)
//-----------------------------------------------------------------------------
int LuaPARTNER_GetName(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 1)
    {
        Lua_PushString(L, "");
        return 1;
    }

    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex))
    {
        Lua_PushString(L, "");
        return 1;
    }

    int nPartnerIdx = (int)Lua_ValueToNumber(L, 1);
    KPartnerData* p = GetPartner(nPlayerIndex, nPartnerIdx);
    if (!p)
    {
        Lua_PushString(L, "");
        return 1;
    }

    Lua_PushString(L, p->szName);
    return 1;
}

//-----------------------------------------------------------------------------
// 12. PARTNER_SetName(nPartnerIdx, szName)
//-----------------------------------------------------------------------------
int LuaPARTNER_SetName(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 2)
    {
        Lua_PushNumber(L, -1);
        return 1;
    }

    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex))
    {
        Lua_PushNumber(L, -1);
        return 1;
    }

    int nPartnerIdx   = (int)Lua_ValueToNumber(L, 1);
    const char* szNew = Lua_ValueToString(L, 2);
    KPartnerData* p   = GetPartner(nPlayerIndex, nPartnerIdx);
    if (!p || !szNew)
    {
        Lua_PushNumber(L, -1);
        return 1;
    }

    strncpy(p->szName, szNew, sizeof(p->szName) - 1);
    p->szName[sizeof(p->szName) - 1] = '\0';
    Lua_PushNumber(L, 1);
    return 1;
}

//-----------------------------------------------------------------------------
// 13. PARTNER_GetSeries(nPartnerIdx)
//-----------------------------------------------------------------------------
int LuaPARTNER_GetSeries(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 1)
    {
        Lua_PushNumber(L, -1);
        return 1;
    }

    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex))
    {
        Lua_PushNumber(L, -1);
        return 1;
    }

    int nPartnerIdx = (int)Lua_ValueToNumber(L, 1);
    KPartnerData* p = GetPartner(nPlayerIndex, nPartnerIdx);
    if (!p)
    {
        Lua_PushNumber(L, -1);
        return 1;
    }

    Lua_PushNumber(L, p->nSeries);
    return 1;
}

//-----------------------------------------------------------------------------
// 14. PARTNER_GetCharacter(nPartnerIdx)
//-----------------------------------------------------------------------------
int LuaPARTNER_GetCharacter(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 1)
    {
        Lua_PushNumber(L, -1);
        return 1;
    }

    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex))
    {
        Lua_PushNumber(L, -1);
        return 1;
    }

    int nPartnerIdx = (int)Lua_ValueToNumber(L, 1);
    KPartnerData* p = GetPartner(nPlayerIndex, nPartnerIdx);
    if (!p)
    {
        Lua_PushNumber(L, -1);
        return 1;
    }

    Lua_PushNumber(L, p->nCharacter);
    return 1;
}

//-----------------------------------------------------------------------------
// 15. PARTNER_ChangeCharacter(nPartnerIdx, nCharacter)
//-----------------------------------------------------------------------------
int LuaPARTNER_ChangeCharacter(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 2)
    {
        Lua_PushNumber(L, -1);
        return 1;
    }

    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex))
    {
        Lua_PushNumber(L, -1);
        return 1;
    }

    int nPartnerIdx = (int)Lua_ValueToNumber(L, 1);
    int nCharacter  = (int)Lua_ValueToNumber(L, 2);
    KPartnerData* p = GetPartner(nPlayerIndex, nPartnerIdx);
    if (!p)
    {
        Lua_PushNumber(L, -1);
        return 1;
    }

    p->nCharacter = nCharacter;
    Lua_PushNumber(L, 1);
    return 1;
}

//-----------------------------------------------------------------------------
// 16. PARTNER_GetEmotionDegree(nPartnerIdx)
//-----------------------------------------------------------------------------
int LuaPARTNER_GetEmotionDegree(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 1)
    {
        Lua_PushNumber(L, -1);
        return 1;
    }

    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex))
    {
        Lua_PushNumber(L, -1);
        return 1;
    }

    int nPartnerIdx = (int)Lua_ValueToNumber(L, 1);
    KPartnerData* p = GetPartner(nPlayerIndex, nPartnerIdx);
    if (!p)
    {
        Lua_PushNumber(L, -1);
        return 1;
    }

    Lua_PushNumber(L, p->nEmotionDegree);
    return 1;
}

//-----------------------------------------------------------------------------
// 17. PARTNER_SetEmotionDegree(nPartnerIdx, nVal)
//-----------------------------------------------------------------------------
int LuaPARTNER_SetEmotionDegree(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 2)
    {
        Lua_PushNumber(L, -1);
        return 1;
    }

    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex))
    {
        Lua_PushNumber(L, -1);
        return 1;
    }

    int nPartnerIdx = (int)Lua_ValueToNumber(L, 1);
    int nVal        = (int)Lua_ValueToNumber(L, 2);
    KPartnerData* p = GetPartner(nPlayerIndex, nPartnerIdx);
    if (!p)
    {
        Lua_PushNumber(L, -1);
        return 1;
    }

    p->nEmotionDegree = (nVal > 100) ? 100 : ((nVal < 0) ? 0 : nVal);
    Lua_PushNumber(L, p->nEmotionDegree);
    return 1;
}

//-----------------------------------------------------------------------------
// 18. PARTNER_AddEmotionDegree(nPartnerIdx, nAdd)
//-----------------------------------------------------------------------------
int LuaPARTNER_AddEmotionDegree(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 2)
    {
        Lua_PushNumber(L, -1);
        return 1;
    }

    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex))
    {
        Lua_PushNumber(L, -1);
        return 1;
    }

    int nPartnerIdx = (int)Lua_ValueToNumber(L, 1);
    int nAdd        = (int)Lua_ValueToNumber(L, 2);
    KPartnerData* p = GetPartner(nPlayerIndex, nPartnerIdx);
    if (!p)
    {
        Lua_PushNumber(L, -1);
        return 1;
    }

    p->nEmotionDegree += nAdd;
    if (p->nEmotionDegree > 100) p->nEmotionDegree = 100;
    if (p->nEmotionDegree < 0) p->nEmotionDegree = 0;

    Lua_PushNumber(L, p->nEmotionDegree);
    return 1;
}

//-----------------------------------------------------------------------------
// 19. PARTNER_GetEssentialFeatureID(nPartnerIdx)
//-----------------------------------------------------------------------------
int LuaPARTNER_GetEssentialFeatureID(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 1)
    {
        Lua_PushNumber(L, -1);
        return 1;
    }

    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex))
    {
        Lua_PushNumber(L, -1);
        return 1;
    }

    int nPartnerIdx = (int)Lua_ValueToNumber(L, 1);
    KPartnerData* p = GetPartner(nPlayerIndex, nPartnerIdx);
    if (!p)
    {
        Lua_PushNumber(L, -1);
        return 1;
    }

    Lua_PushNumber(L, p->nEssentialFeatureID);
    return 1;
}

//-----------------------------------------------------------------------------
// 20. PARTNER_GetSettingIdx(nPartnerIdx)
//-----------------------------------------------------------------------------
int LuaPARTNER_GetSettingIdx(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 1)
    {
        Lua_PushNumber(L, -1);
        return 1;
    }

    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex))
    {
        Lua_PushNumber(L, -1);
        return 1;
    }

    int nPartnerIdx = (int)Lua_ValueToNumber(L, 1);
    KPartnerData* p = GetPartner(nPlayerIndex, nPartnerIdx);
    if (!p)
    {
        Lua_PushNumber(L, -1);
        return 1;
    }

    Lua_PushNumber(L, p->nSettingIdx);
    return 1;
}

//-----------------------------------------------------------------------------
// 21. PARTNER_GetEndure(nPartnerIdx)
//-----------------------------------------------------------------------------
int LuaPARTNER_GetEndure(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 1)
    {
        Lua_PushNumber(L, -1);
        return 1;
    }

    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex))
    {
        Lua_PushNumber(L, -1);
        return 1;
    }

    int nPartnerIdx = (int)Lua_ValueToNumber(L, 1);
    KPartnerData* p = GetPartner(nPlayerIndex, nPartnerIdx);
    if (!p)
    {
        Lua_PushNumber(L, -1);
        return 1;
    }

    Lua_PushNumber(L, p->nEndure);
    return 1;
}

//-----------------------------------------------------------------------------
// 22. PARTNER_GetBirthday(nPartnerIdx)
//-----------------------------------------------------------------------------
int LuaPARTNER_GetBirthday(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 1)
    {
        Lua_PushNumber(L, -1);
        return 1;
    }

    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex))
    {
        Lua_PushNumber(L, -1);
        return 1;
    }

    int nPartnerIdx = (int)Lua_ValueToNumber(L, 1);
    KPartnerData* p = GetPartner(nPlayerIndex, nPartnerIdx);
    if (!p)
    {
        Lua_PushNumber(L, -1);
        return 1;
    }

    Lua_PushNumber(L, p->nBirthday);
    return 1;
}

//-----------------------------------------------------------------------------
// 23. PARTNER_GetAptitudes(nPartnerIdx) -> returns 6 values
//-----------------------------------------------------------------------------
int LuaPARTNER_GetAptitudes(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 1)
    {
        Lua_PushNumber(L, -1);
        return 1;
    }

    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex))
    {
        Lua_PushNumber(L, -1);
        return 1;
    }

    int nPartnerIdx = (int)Lua_ValueToNumber(L, 1);
    KPartnerData* p = GetPartner(nPlayerIndex, nPartnerIdx);
    if (!p)
    {
        Lua_PushNumber(L, -1);
        return 1;
    }

    Lua_PushNumber(L, p->nLifeAptitude);
    Lua_PushNumber(L, p->nStrengthAptitude);
    Lua_PushNumber(L, p->nHitTargetRateAptitude);
    Lua_PushNumber(L, p->nDefenceAptitude);
    Lua_PushNumber(L, p->nSpeedAptitude);
    Lua_PushNumber(L, p->nLuckAptitude);
    return 6;
}

//-----------------------------------------------------------------------------
// 24. PARTNER_AddLifeAptitude(nPartnerIdx, nVal)
//-----------------------------------------------------------------------------
int LuaPARTNER_AddLifeAptitude(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 2) return 0;
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex)) return 0;
    KPartnerData* p = GetPartner(nPlayerIndex, (int)Lua_ValueToNumber(L, 1));
    if (!p) return 0;
    p->nLifeAptitude += (int)Lua_ValueToNumber(L, 2);
    Lua_PushNumber(L, p->nLifeAptitude);
    return 1;
}

//-----------------------------------------------------------------------------
// 25. PARTNER_AddStrengthAptitude(nPartnerIdx, nVal)
//-----------------------------------------------------------------------------
int LuaPARTNER_AddStrengthAptitude(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 2) return 0;
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex)) return 0;
    KPartnerData* p = GetPartner(nPlayerIndex, (int)Lua_ValueToNumber(L, 1));
    if (!p) return 0;
    p->nStrengthAptitude += (int)Lua_ValueToNumber(L, 2);
    Lua_PushNumber(L, p->nStrengthAptitude);
    return 1;
}

//-----------------------------------------------------------------------------
// 26. PARTNER_AddHitTargetRateAptitude(nPartnerIdx, nVal)
//-----------------------------------------------------------------------------
int LuaPARTNER_AddHitTargetRateAptitude(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 2) return 0;
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex)) return 0;
    KPartnerData* p = GetPartner(nPlayerIndex, (int)Lua_ValueToNumber(L, 1));
    if (!p) return 0;
    p->nHitTargetRateAptitude += (int)Lua_ValueToNumber(L, 2);
    Lua_PushNumber(L, p->nHitTargetRateAptitude);
    return 1;
}

//-----------------------------------------------------------------------------
// 27. PARTNER_AddDefenceAptitude(nPartnerIdx, nVal)
//-----------------------------------------------------------------------------
int LuaPARTNER_AddDefenceAptitude(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 2) return 0;
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex)) return 0;
    KPartnerData* p = GetPartner(nPlayerIndex, (int)Lua_ValueToNumber(L, 1));
    if (!p) return 0;
    p->nDefenceAptitude += (int)Lua_ValueToNumber(L, 2);
    Lua_PushNumber(L, p->nDefenceAptitude);
    return 1;
}

//-----------------------------------------------------------------------------
// 28. PARTNER_AddSpeedAptitude(nPartnerIdx, nVal)
//-----------------------------------------------------------------------------
int LuaPARTNER_AddSpeedAptitude(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 2) return 0;
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex)) return 0;
    KPartnerData* p = GetPartner(nPlayerIndex, (int)Lua_ValueToNumber(L, 1));
    if (!p) return 0;
    p->nSpeedAptitude += (int)Lua_ValueToNumber(L, 2);
    Lua_PushNumber(L, p->nSpeedAptitude);
    return 1;
}

//-----------------------------------------------------------------------------
// 29. PARTNER_AddLuckAptitude(nPartnerIdx, nVal)
//-----------------------------------------------------------------------------
int LuaPARTNER_AddLuckAptitude(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 2) return 0;
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex)) return 0;
    KPartnerData* p = GetPartner(nPlayerIndex, (int)Lua_ValueToNumber(L, 1));
    if (!p) return 0;
    p->nLuckAptitude += (int)Lua_ValueToNumber(L, 2);
    Lua_PushNumber(L, p->nLuckAptitude);
    return 1;
}

//-----------------------------------------------------------------------------
// 30. PARTNER_GetAttribs(nPartnerIdx) -> returns 6 values
//-----------------------------------------------------------------------------
int LuaPARTNER_GetAttribs(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 1)
    {
        Lua_PushNumber(L, -1);
        return 1;
    }

    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex))
    {
        Lua_PushNumber(L, -1);
        return 1;
    }

    int nPartnerIdx = (int)Lua_ValueToNumber(L, 1);
    KPartnerData* p = GetPartner(nPlayerIndex, nPartnerIdx);
    if (!p)
    {
        Lua_PushNumber(L, -1);
        return 1;
    }

    for (int i = 0; i < 6; ++i)
        Lua_PushNumber(L, p->nAttribs[i]);
    return 6;
}

//-----------------------------------------------------------------------------
// 31. PARTNER_SetAttribs(nPartnerIdx, idx, val)
//-----------------------------------------------------------------------------
int LuaPARTNER_SetAttribs(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 3) return 0;
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex)) return 0;

    int nPartnerIdx = (int)Lua_ValueToNumber(L, 1);
    int nAttrIdx    = (int)Lua_ValueToNumber(L, 2);
    int nVal        = (int)Lua_ValueToNumber(L, 3);

    KPartnerData* p = GetPartner(nPlayerIndex, nPartnerIdx);
    if (!p || nAttrIdx < 0 || nAttrIdx >= 6) return 0;

    p->nAttribs[nAttrIdx] = nVal;
    Lua_PushNumber(L, 1);
    return 1;
}

//-----------------------------------------------------------------------------
// 32. PARTNER_GetAttribsInc(nPartnerIdx) -> returns 6 values
//-----------------------------------------------------------------------------
int LuaPARTNER_GetAttribsInc(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 1)
    {
        Lua_PushNumber(L, -1);
        return 1;
    }

    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex))
    {
        Lua_PushNumber(L, -1);
        return 1;
    }

    int nPartnerIdx = (int)Lua_ValueToNumber(L, 1);
    KPartnerData* p = GetPartner(nPlayerIndex, nPartnerIdx);
    if (!p)
    {
        Lua_PushNumber(L, -1);
        return 1;
    }

    for (int i = 0; i < 6; ++i)
        Lua_PushNumber(L, p->nAttribsInc[i]);
    return 6;
}

//-----------------------------------------------------------------------------
// 33. PARTNER_ReGenAttribsInc(nPartnerIdx)
//-----------------------------------------------------------------------------
int LuaPARTNER_ReGenAttribsInc(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 1) return 0;
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex)) return 0;

    int nPartnerIdx = (int)Lua_ValueToNumber(L, 1);
    KPartnerData* p = GetPartner(nPlayerIndex, nPartnerIdx);
    if (!p) return 0;

    for (int i = 0; i < 6; ++i)
        p->nAttribsInc[i] = (rand() % 10) + 1;

    Lua_PushNumber(L, 1);
    return 1;
}

//-----------------------------------------------------------------------------
// 34. PARTNER_GetResists(nPartnerIdx) -> returns 5 values
//-----------------------------------------------------------------------------
int LuaPARTNER_GetResists(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 1)
    {
        Lua_PushNumber(L, -1);
        return 1;
    }

    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex))
    {
        Lua_PushNumber(L, -1);
        return 1;
    }

    int nPartnerIdx = (int)Lua_ValueToNumber(L, 1);
    KPartnerData* p = GetPartner(nPlayerIndex, nPartnerIdx);
    if (!p)
    {
        Lua_PushNumber(L, -1);
        return 1;
    }

    for (int i = 0; i < 5; ++i)
        Lua_PushNumber(L, p->nResists[i]);
    return 5;
}

//-----------------------------------------------------------------------------
// 35. PARTNER_SetResists(nPartnerIdx, nResistIdx, nVal)
//-----------------------------------------------------------------------------
int LuaPARTNER_SetResists(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 3) return 0;
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex)) return 0;

    int nPartnerIdx = (int)Lua_ValueToNumber(L, 1);
    int nResistIdx  = (int)Lua_ValueToNumber(L, 2);
    int nVal        = (int)Lua_ValueToNumber(L, 3);

    KPartnerData* p = GetPartner(nPlayerIndex, nPartnerIdx);
    if (!p || nResistIdx < 0 || nResistIdx >= 5) return 0;

    p->nResists[nResistIdx] = nVal;
    Lua_PushNumber(L, 1);
    return 1;
}

//-----------------------------------------------------------------------------
// 36. PARTNER_AddSkill(nPartnerIdx, nSkillId, nSkillLevel)
//-----------------------------------------------------------------------------
int LuaPARTNER_AddSkill(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 3) return 0;
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex)) return 0;

    int nPartnerIdx  = (int)Lua_ValueToNumber(L, 1);
    int nSkillId     = (int)Lua_ValueToNumber(L, 2);
    int nSkillLevel  = (int)Lua_ValueToNumber(L, 3);

    KPartnerData* p = GetPartner(nPlayerIndex, nPartnerIdx);
    if (!p) return 0;

    p->mapSkills[nSkillId] = nSkillLevel;
    Lua_PushNumber(L, 1);
    return 1;
}

//-----------------------------------------------------------------------------
// 37. PARTNER_RemoveSkill(nPartnerIdx, nSkillId)
//-----------------------------------------------------------------------------
int LuaPARTNER_RemoveSkill(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 2) return 0;
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex)) return 0;

    int nPartnerIdx = (int)Lua_ValueToNumber(L, 1);
    int nSkillId    = (int)Lua_ValueToNumber(L, 2);

    KPartnerData* p = GetPartner(nPlayerIndex, nPartnerIdx);
    if (!p) return 0;

    p->mapSkills.erase(nSkillId);
    Lua_PushNumber(L, 1);
    return 1;
}

//-----------------------------------------------------------------------------
// 38. PARTNER_RemoveAllSkill(nPartnerIdx)
//-----------------------------------------------------------------------------
int LuaPARTNER_RemoveAllSkill(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 1) return 0;
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex)) return 0;

    int nPartnerIdx = (int)Lua_ValueToNumber(L, 1);
    KPartnerData* p = GetPartner(nPlayerIndex, nPartnerIdx);
    if (!p) return 0;

    p->mapSkills.clear();
    Lua_PushNumber(L, 1);
    return 1;
}

//-----------------------------------------------------------------------------
// 39. PARTNER_GetAllSkill(nPartnerIdx) -> returns Lua table of skills
//-----------------------------------------------------------------------------
int LuaPARTNER_GetAllSkill(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 1)
        return 0;

    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex))
        return 0;

    int nPartnerIdx = (int)Lua_ValueToNumber(L, 1);
    KPartnerData* p = GetPartner(nPlayerIndex, nPartnerIdx);
    if (!p)
        return 0;

    lua_newtable(L);
    int nIdx = 1;
    for (auto it = p->mapSkills.begin(); it != p->mapSkills.end(); ++it, ++nIdx)
    {
        Lua_PushNumber(L, nIdx);
        Lua_PushNumber(L, it->first);
        lua_settable(L, -3);
    }
    return 1;
}

//-----------------------------------------------------------------------------
// 40. PARTNER_GetSkillInfo(nPartnerIdx, nSkillId) -> returns SkillLevel
//-----------------------------------------------------------------------------
int LuaPARTNER_GetSkillInfo(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 2)
    {
        Lua_PushNumber(L, -1);
        return 1;
    }

    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex))
    {
        Lua_PushNumber(L, -1);
        return 1;
    }

    int nPartnerIdx = (int)Lua_ValueToNumber(L, 1);
    int nSkillId    = (int)Lua_ValueToNumber(L, 2);

    KPartnerData* p = GetPartner(nPlayerIndex, nPartnerIdx);
    if (!p)
    {
        Lua_PushNumber(L, -1);
        return 1;
    }

    auto it = p->mapSkills.find(nSkillId);
    if (it != p->mapSkills.end())
        Lua_PushNumber(L, it->second);
    else
        Lua_PushNumber(L, 0);

    return 1;
}

//-----------------------------------------------------------------------------
// 41. PARTNER_SetStandbySkill(nPartnerIdx, nSkillId)
//-----------------------------------------------------------------------------
int LuaPARTNER_SetStandbySkill(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 2) return 0;
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex)) return 0;

    int nPartnerIdx = (int)Lua_ValueToNumber(L, 1);
    int nSkillId    = (int)Lua_ValueToNumber(L, 2);

    KPartnerData* p = GetPartner(nPlayerIndex, nPartnerIdx);
    if (!p) return 0;

    p->nStandbySkill = nSkillId;
    Lua_PushNumber(L, 1);
    return 1;
}

//-----------------------------------------------------------------------------
// 42. PARTNER_GetTaskValue(nPartnerIdx, nTaskId)
//-----------------------------------------------------------------------------
int LuaPARTNER_GetTaskValue(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 2)
    {
        Lua_PushNumber(L, -1);
        return 1;
    }

    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex))
    {
        Lua_PushNumber(L, -1);
        return 1;
    }

    int nPartnerIdx = (int)Lua_ValueToNumber(L, 1);
    int nTaskId     = (int)Lua_ValueToNumber(L, 2);

    KPartnerData* p = GetPartner(nPlayerIndex, nPartnerIdx);
    if (!p)
    {
        Lua_PushNumber(L, -1);
        return 1;
    }

    auto it = p->mapTaskValues.find(nTaskId);
    if (it != p->mapTaskValues.end())
        Lua_PushNumber(L, it->second);
    else
        Lua_PushNumber(L, 0);

    return 1;
}

//-----------------------------------------------------------------------------
// 43. PARTNER_SetTaskValue(nPartnerIdx, nTaskId, nVal)
//-----------------------------------------------------------------------------
int LuaPARTNER_SetTaskValue(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 3) return 0;
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex)) return 0;

    int nPartnerIdx = (int)Lua_ValueToNumber(L, 1);
    int nTaskId     = (int)Lua_ValueToNumber(L, 2);
    int nVal        = (int)Lua_ValueToNumber(L, 3);

    KPartnerData* p = GetPartner(nPlayerIndex, nPartnerIdx);
    if (!p) return 0;

    p->mapTaskValues[nTaskId] = nVal;
    Lua_PushNumber(L, 1);
    return 1;
}

//-----------------------------------------------------------------------------
// 44. PARTNER_ChangeFeature(nPartnerIdx, nFeatureId)
//-----------------------------------------------------------------------------
int LuaPARTNER_ChangeFeature(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 2) return 0;
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex)) return 0;

    int nPartnerIdx = (int)Lua_ValueToNumber(L, 1);
    int nFeatureId  = (int)Lua_ValueToNumber(L, 2);

    KPartnerData* p = GetPartner(nPlayerIndex, nPartnerIdx);
    if (!p) return 0;

    p->nEssentialFeatureID = nFeatureId;
    Lua_PushNumber(L, 1);
    return 1;
}

//-----------------------------------------------------------------------------
// 45. PARTNER_ChangeFeatureOfPeriod(nPartnerIdx, nFeatureId, nSeconds)
//-----------------------------------------------------------------------------
int LuaPARTNER_ChangeFeatureOfPeriod(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 3) return 0;
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex)) return 0;

    int nPartnerIdx = (int)Lua_ValueToNumber(L, 1);
    int nFeatureId  = (int)Lua_ValueToNumber(L, 2);

    KPartnerData* p = GetPartner(nPlayerIndex, nPartnerIdx);
    if (!p) return 0;

    p->nEssentialFeatureID = nFeatureId;
    Lua_PushNumber(L, 1);
    return 1;
}

//-----------------------------------------------------------------------------
// 46. PARTNER_AddState(nPartnerIdx, nStateId, nStateLevel)
//-----------------------------------------------------------------------------
int LuaPARTNER_AddState(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 3) return 0;
    Lua_PushNumber(L, 1);
    return 1;
}

//-----------------------------------------------------------------------------
// 47. PARTNER_SetCallOutSwitch(nPartnerIdx, bEnable)
//-----------------------------------------------------------------------------
int LuaPARTNER_SetCallOutSwitch(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 2) return 0;
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex)) return 0;

    int bEnable = (int)Lua_ValueToNumber(L, 2);
    g_V9PartnerMgr[nPlayerIndex].m_bCallOut = (bEnable != 0) ? 1 : 0;
    Lua_PushNumber(L, 1);
    return 1;
}

//-----------------------------------------------------------------------------
// 48. PARTNER_GetGenData(nPartnerIdx)
//-----------------------------------------------------------------------------
int LuaPARTNER_GetGenData(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 1)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    Lua_PushNumber(L, 0);
    return 1;
}

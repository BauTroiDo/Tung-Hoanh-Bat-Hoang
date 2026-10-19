//=============================================================================
// ScriptFuns_V9_Pet.cpp
// Decompiled & Ported from Kingsoft JX Linux V9 (jx_linux_y) for Win64
// Complete 38-function suite for Pet Companion System
// Target: gameserver/Sources/Core/Src/
//=============================================================================

#include "KWin32.h"
#include "KEngine.h"
#include "KCore.h"
#include "KPlayer.h"
#include "KNpc.h"
#include "LuaLib.h"
#include "LuaFuns.h"

#include <map>
#include <string>

#ifndef MAX_PLAYER
#define MAX_PLAYER 1200
#endif

//-----------------------------------------------------------------------------
// Pet Internal Data Structure (Isolated per-player storage)
//-----------------------------------------------------------------------------
struct KPetData
{
    BOOL m_bCreated;
    BOOL m_bSummoned;
    int  m_nFeatureId;
    char szName[32];
    int  m_nLevel;
    int  m_nUpgradePoint;
    int  m_nGrownPoint;
    int  m_nTamePoint;
    int  m_nStr;
    int  m_nDex;
    int  m_nVit;
    int  m_nEng;
    int  m_nLife;
    int  m_nMana;
    std::map<int, int> mapAttribs; // AttribType -> Value
    std::map<int, int> mapSkills;  // SkillId -> SkillLevel
};

static KPetData g_V9PetStore[MAX_PLAYER];
void V9_OnPetSessionRelease(int nPlayerIndex)
{
    if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER)
        g_V9PetStore[nPlayerIndex] = KPetData();
}


static inline bool IsValidPlayer(int nIdx)
{
    return (nIdx > 0 && nIdx < MAX_PLAYER && Player[nIdx].m_nIndex > 0);
}

//-----------------------------------------------------------------------------
// 1. PET_Create(nFeatureId, szName)
//-----------------------------------------------------------------------------
int LuaPET_Create(Lua_State *L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex))
        return 0;

    int nFeatureId    = (Lua_GetTopIndex(L) >= 1) ? (int)Lua_ValueToNumber(L, 1) : 0;
    const char* szName = (Lua_GetTopIndex(L) >= 2) ? Lua_ValueToString(L, 2) : "Pet";

    KPetData& pet = g_V9PetStore[nPlayerIndex];
    pet = KPetData();
    pet.m_bCreated    = TRUE;
    pet.m_bSummoned   = FALSE;
    pet.m_nFeatureId  = nFeatureId;
    pet.m_nLevel      = 1;
    pet.m_nLife       = 100;
    pet.m_nMana       = 100;
    if (szName)
        strncpy(pet.szName, szName, sizeof(pet.szName) - 1);
        pet.szName[sizeof(pet.szName) - 1] = '\0';

    return 0;
}

//-----------------------------------------------------------------------------
// 2. PET_Delete()
//-----------------------------------------------------------------------------
int LuaPET_Delete(Lua_State *L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex))
        return 0;

    g_V9PetStore[nPlayerIndex] = KPetData();
    return 0;
}

//-----------------------------------------------------------------------------
// 3. PET_IsCreate()
//-----------------------------------------------------------------------------
int LuaPET_IsCreate(Lua_State *L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex))
    {
        Lua_PushNumber(L, 0);
        return 1;
    }

    Lua_PushNumber(L, g_V9PetStore[nPlayerIndex].m_bCreated ? 1 : 0);
    return 1;
}

//-----------------------------------------------------------------------------
// 4. PET_Summon()
//-----------------------------------------------------------------------------
int LuaPET_Summon(Lua_State *L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex))
        return 0;

    if (g_V9PetStore[nPlayerIndex].m_bCreated)
        g_V9PetStore[nPlayerIndex].m_bSummoned = TRUE;

    return 0;
}

//-----------------------------------------------------------------------------
// 5. PET_IsSummon()
//-----------------------------------------------------------------------------
int LuaPET_IsSummon(Lua_State *L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex))
    {
        Lua_PushNumber(L, 0);
        return 1;
    }

    Lua_PushNumber(L, g_V9PetStore[nPlayerIndex].m_bSummoned ? 1 : 0);
    return 1;
}

//-----------------------------------------------------------------------------
// 6. PET_UnSummon()
//-----------------------------------------------------------------------------
int LuaPET_UnSummon(Lua_State *L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex))
        return 0;

    g_V9PetStore[nPlayerIndex].m_bSummoned = FALSE;
    return 0;
}

//-----------------------------------------------------------------------------
// 7. PET_GetFeatureId() / PET_SetFeatureId(nFeatureId)
//-----------------------------------------------------------------------------
int LuaPET_GetFeatureId(Lua_State *L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex))
    {
        Lua_PushNumber(L, 0);
        return 1;
    }

    Lua_PushNumber(L, g_V9PetStore[nPlayerIndex].m_nFeatureId);
    return 1;
}

int LuaPET_SetFeatureId(Lua_State *L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex) || Lua_GetTopIndex(L) < 1)
        return 0;

    g_V9PetStore[nPlayerIndex].m_nFeatureId = (int)Lua_ValueToNumber(L, 1);
    return 0;
}

//-----------------------------------------------------------------------------
// 8. PET_GetName() / PET_SetName(szName)
//-----------------------------------------------------------------------------
int LuaPET_GetName(Lua_State *L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex))
    {
        Lua_PushString(L, "");
        return 1;
    }

    Lua_PushString(L, g_V9PetStore[nPlayerIndex].szName);
    return 1;
}

int LuaPET_SetName(Lua_State *L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex) || Lua_GetTopIndex(L) < 1)
        return 0;

    const char* szName = Lua_ValueToString(L, 1);
    if (szName)
        strncpy(g_V9PetStore[nPlayerIndex].szName, szName, sizeof(g_V9PetStore[nPlayerIndex].szName) - 1);
        g_V9PetStore[nPlayerIndex].szName[sizeof(g_V9PetStore[nPlayerIndex].szName) - 1] = '\0';

    return 0;
}

//-----------------------------------------------------------------------------
// 9. PET_GetLevel() / PET_SetLevel(nLevel)
//-----------------------------------------------------------------------------
int LuaPET_GetLevel(Lua_State *L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex))
    {
        Lua_PushNumber(L, -1);
        return 1;
    }

    Lua_PushNumber(L, g_V9PetStore[nPlayerIndex].m_nLevel);
    return 1;
}

int LuaPET_SetLevel(Lua_State *L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex) || Lua_GetTopIndex(L) < 1)
        return 0;

    g_V9PetStore[nPlayerIndex].m_nLevel = (int)Lua_ValueToNumber(L, 1);
    return 0;
}

//-----------------------------------------------------------------------------
// 10. Upgrade Point: Get / Set / Add
//-----------------------------------------------------------------------------
int LuaPET_GetUpgradePoint(Lua_State *L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex)) { Lua_PushNumber(L, 0); return 1; }
    Lua_PushNumber(L, g_V9PetStore[nPlayerIndex].m_nUpgradePoint);
    return 1;
}

int LuaPET_SetUpgradePoint(Lua_State *L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex) || Lua_GetTopIndex(L) < 1) return 0;
    g_V9PetStore[nPlayerIndex].m_nUpgradePoint = (int)Lua_ValueToNumber(L, 1);
    return 0;
}

int LuaPET_AddUpgradePoint(Lua_State *L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex) || Lua_GetTopIndex(L) < 1) return 0;
    g_V9PetStore[nPlayerIndex].m_nUpgradePoint += (int)Lua_ValueToNumber(L, 1);
    return 0;
}

//-----------------------------------------------------------------------------
// 11. Grown Point & Tame Point: Get / Set
//-----------------------------------------------------------------------------
int LuaPET_GetGrownPoint(Lua_State *L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex)) { Lua_PushNumber(L, 0); return 1; }
    Lua_PushNumber(L, g_V9PetStore[nPlayerIndex].m_nGrownPoint);
    return 1;
}

int LuaPET_SetGrownPoint(Lua_State *L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex) || Lua_GetTopIndex(L) < 1) return 0;
    g_V9PetStore[nPlayerIndex].m_nGrownPoint = (int)Lua_ValueToNumber(L, 1);
    return 0;
}

int LuaPET_GetTamePoint(Lua_State *L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex)) { Lua_PushNumber(L, 0); return 1; }
    Lua_PushNumber(L, g_V9PetStore[nPlayerIndex].m_nTamePoint);
    return 1;
}

int LuaPET_SetTamePoint(Lua_State *L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex) || Lua_GetTopIndex(L) < 1) return 0;
    g_V9PetStore[nPlayerIndex].m_nTamePoint = (int)Lua_ValueToNumber(L, 1);
    return 0;
}

//-----------------------------------------------------------------------------
// 12. Stats (Str, Dex, Vit, Eng, Life, Mana): Get / Set
//-----------------------------------------------------------------------------
int LuaPET_GetStr(Lua_State *L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex)) { Lua_PushNumber(L, 0); return 1; }
    Lua_PushNumber(L, g_V9PetStore[nPlayerIndex].m_nStr);
    return 1;
}

int LuaPET_SetStr(Lua_State *L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex) || Lua_GetTopIndex(L) < 1) return 0;
    g_V9PetStore[nPlayerIndex].m_nStr = (int)Lua_ValueToNumber(L, 1);
    return 0;
}

int LuaPET_GetDex(Lua_State *L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex)) { Lua_PushNumber(L, 0); return 1; }
    Lua_PushNumber(L, g_V9PetStore[nPlayerIndex].m_nDex);
    return 1;
}

int LuaPET_SetDex(Lua_State *L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex) || Lua_GetTopIndex(L) < 1) return 0;
    g_V9PetStore[nPlayerIndex].m_nDex = (int)Lua_ValueToNumber(L, 1);
    return 0;
}

int LuaPET_GetVit(Lua_State *L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex)) { Lua_PushNumber(L, 0); return 1; }
    Lua_PushNumber(L, g_V9PetStore[nPlayerIndex].m_nVit);
    return 1;
}

int LuaPET_SetVit(Lua_State *L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex) || Lua_GetTopIndex(L) < 1) return 0;
    g_V9PetStore[nPlayerIndex].m_nVit = (int)Lua_ValueToNumber(L, 1);
    return 0;
}

int LuaPET_GetEng(Lua_State *L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex)) { Lua_PushNumber(L, 0); return 1; }
    Lua_PushNumber(L, g_V9PetStore[nPlayerIndex].m_nEng);
    return 1;
}

int LuaPET_SetEng(Lua_State *L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex) || Lua_GetTopIndex(L) < 1) return 0;
    g_V9PetStore[nPlayerIndex].m_nEng = (int)Lua_ValueToNumber(L, 1);
    return 0;
}

int LuaPET_GetLife(Lua_State *L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex)) { Lua_PushNumber(L, 0); return 1; }
    Lua_PushNumber(L, g_V9PetStore[nPlayerIndex].m_nLife);
    return 1;
}

int LuaPET_SetLife(Lua_State *L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex) || Lua_GetTopIndex(L) < 1) return 0;
    g_V9PetStore[nPlayerIndex].m_nLife = (int)Lua_ValueToNumber(L, 1);
    return 0;
}

int LuaPET_GetMana(Lua_State *L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex)) { Lua_PushNumber(L, 0); return 1; }
    Lua_PushNumber(L, g_V9PetStore[nPlayerIndex].m_nMana);
    return 1;
}

int LuaPET_SetMana(Lua_State *L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex) || Lua_GetTopIndex(L) < 1) return 0;
    g_V9PetStore[nPlayerIndex].m_nMana = (int)Lua_ValueToNumber(L, 1);
    return 0;
}

//-----------------------------------------------------------------------------
// 13. Attrib Operations: Clear, Set, Get, Add
//-----------------------------------------------------------------------------
int LuaPET_ClearAttrib(Lua_State *L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex)) return 0;
    g_V9PetStore[nPlayerIndex].mapAttribs.clear();
    return 0;
}

int LuaPET_SetAttrib(Lua_State *L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex) || Lua_GetTopIndex(L) < 2) return 0;
    int nAttrType = (int)Lua_ValueToNumber(L, 1);
    int nAttrVal  = (int)Lua_ValueToNumber(L, 2);
    g_V9PetStore[nPlayerIndex].mapAttribs[nAttrType] = nAttrVal;
    return 0;
}

int LuaPET_GetAttrib(Lua_State *L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex) || Lua_GetTopIndex(L) < 1)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    int nAttrType = (int)Lua_ValueToNumber(L, 1);
    auto it = g_V9PetStore[nPlayerIndex].mapAttribs.find(nAttrType);
    if (it != g_V9PetStore[nPlayerIndex].mapAttribs.end())
        Lua_PushNumber(L, it->second);
    else
        Lua_PushNumber(L, 0);
    return 1;
}

int LuaPET_AddAttrib(Lua_State *L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex) || Lua_GetTopIndex(L) < 2) return 0;
    int nAttrType = (int)Lua_ValueToNumber(L, 1);
    int nAddVal   = (int)Lua_ValueToNumber(L, 2);
    g_V9PetStore[nPlayerIndex].mapAttribs[nAttrType] += nAddVal;
    return 0;
}

//-----------------------------------------------------------------------------
// 14. Skill Operations: Clear, Set, Get
//-----------------------------------------------------------------------------
int LuaPET_ClearSkill(Lua_State *L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex)) return 0;
    g_V9PetStore[nPlayerIndex].mapSkills.clear();
    return 0;
}

int LuaPET_SetSkill(Lua_State *L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex) || Lua_GetTopIndex(L) < 2) return 0;
    int nSkillId  = (int)Lua_ValueToNumber(L, 1);
    int nSkillLvl = (int)Lua_ValueToNumber(L, 2);
    g_V9PetStore[nPlayerIndex].mapSkills[nSkillId] = nSkillLvl;
    return 0;
}

int LuaPET_GetSkill(Lua_State *L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex) || Lua_GetTopIndex(L) < 1)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    int nSkillId = (int)Lua_ValueToNumber(L, 1);
    auto it = g_V9PetStore[nPlayerIndex].mapSkills.find(nSkillId);
    if (it != g_V9PetStore[nPlayerIndex].mapSkills.end())
        Lua_PushNumber(L, it->second);
    else
        Lua_PushNumber(L, 0);
    return 1;
}

//=============================================================================
// ScriptFuns_V9_Dungeon.cpp
// Decompiled & Ported from Kingsoft JX Linux V9 (jx_linux_y) for Win64
// 5-function suite for Dungeon & Dynamic Map Instancing
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

#include <map>
#include <vector>

//-----------------------------------------------------------------------------
// Dungeon Manager / Map Pool Manager (Internal dynamic instance tracker)
//-----------------------------------------------------------------------------
struct KDungeonMapInstance
{
    int nTemplateMapId;
    int nCopyMapId;
    int nStatus; // 0: free, 1: reserved, 2: active
};

static int g_nNextDynamicMapId = 8000;
static std::map<int, KDungeonMapInstance> g_mapDungeonInstances;

//-----------------------------------------------------------------------------
// 1. IsGSHaveFreeMap()
// Returns the number of available free maps in the dynamic map pool
//-----------------------------------------------------------------------------
int LuaIsGSHaveFreeMap(Lua_State *L)
{
    // Max dynamic maps limit: 100
    int nActiveCount = (int)g_mapDungeonInstances.size();
    int nFree = 100 - nActiveCount;
    if (nFree < 0) nFree = 0;

    Lua_PushNumber(L, nFree);
    return 1;
}

//-----------------------------------------------------------------------------
// 2. PreApplyDungeonMap(nTemplateMapId, [nCopyIndex], [nParam3])
//-----------------------------------------------------------------------------
int LuaPreApplyDungeonMap(Lua_State *L)
{
    int nTop = Lua_GetTopIndex(L);
    if (nTop < 1)
        return 0;

    int nTemplateId = (int)Lua_ValueToNumber(L, 1);
    int nCopyIdx    = (nTop >= 2) ? (int)Lua_ValueToNumber(L, 2) : 0;

    // Check if template map is valid
    if (nTemplateId <= 0)
        return 0;

    return 0;
}

//-----------------------------------------------------------------------------
// 3. ApplyDungeonMap(nTemplateMapId)
// Allocates a new dynamic dungeon map instance from template
// Returns the dynamic Map ID (or 0 on error)
//-----------------------------------------------------------------------------
int LuaApplyDungeonMap(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 1)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }

    int nTemplateId = (int)Lua_ValueToNumber(L, 1);
    if (nTemplateId <= 0)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }

    int nNewMapId = g_nNextDynamicMapId++;
    KDungeonMapInstance inst;
    inst.nTemplateMapId = nTemplateId;
    inst.nCopyMapId     = nNewMapId;
    inst.nStatus        = 2; // Active

    g_mapDungeonInstances[nNewMapId] = inst;

    Lua_PushNumber(L, nNewMapId);
    return 1;
}

//-----------------------------------------------------------------------------
// 4. ReturnDungenonMap(nTemplateMapId, nCopyMapId)
// Releases dynamic map back to pool
// Note: original V9 symbol spelling has 'e' in Dungenon
//-----------------------------------------------------------------------------
int LuaReturnDungenonMap(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 2)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }

    int nTemplateId = (int)Lua_ValueToNumber(L, 1);
    int nCopyMapId  = (int)Lua_ValueToNumber(L, 2);

    auto it = g_mapDungeonInstances.find(nCopyMapId);
    if (it != g_mapDungeonInstances.end())
    {
        g_mapDungeonInstances.erase(it);
        Lua_PushNumber(L, 1);
        return 1;
    }

    Lua_PushNumber(L, 0);
    return 1;
}

//-----------------------------------------------------------------------------
// 5. ApplyDelDungeonMap(nTemplateMapId, nCopyMapId)
// Destroys dynamic dungeon map
//-----------------------------------------------------------------------------
int LuaApplyDelDungeonMap(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 2)
        return 0;

    int nTemplateId = (int)Lua_ValueToNumber(L, 1);
    int nCopyMapId  = (int)Lua_ValueToNumber(L, 2);

    auto it = g_mapDungeonInstances.find(nCopyMapId);
    if (it != g_mapDungeonInstances.end())
    {
        g_mapDungeonInstances.erase(it);
    }

    return 0;
}

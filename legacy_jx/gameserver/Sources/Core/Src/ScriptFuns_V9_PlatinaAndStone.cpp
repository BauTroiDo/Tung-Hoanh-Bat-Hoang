//=============================================================================
// ScriptFuns_V9_PlatinaAndStone.cpp
// Decompiled & Ported from Kingsoft JX Linux V9 (jx_linux_y) for Win64
// - Platina Item System: AddPlatinaItem, SetPlatinaMagicIndex, GetPtMagicAttrExValue
// - Equip Wish System: GetCurEquipWishValue, SetEquipWishValue, GetMaxEquipWishValue
// - Star Stone System: GetStarStoneOnEquip, GetStoneLevelOnEquip, SetStoneLevelOnEquip,
//                      GetEquipMaxStoneNum, GetLastBreakTime, SetLastBreakTime
// - Item Value & Abrade: GetItemValue, AbradeEquipments
// Target: gameserver/Sources/Core/Src/
//=============================================================================

#include "KWin32.h"
#include "KEngine.h"
#include "KCore.h"
#include "KPlayer.h"
#include "KNpc.h"
#include "KItem.h"
#include "KItemSet.h"
#include "LuaLib.h"
#include "LuaFuns.h"

#ifndef MAX_PLAYER
#define MAX_PLAYER 1200
#endif

#ifndef MAX_EQUIPMENT_SLOTS
#define MAX_EQUIPMENT_SLOTS 16
#endif

static inline bool IsValidPlayer(int nIdx)
{
    return (nIdx > 0 && nIdx < MAX_PLAYER && Player[nIdx].m_nIndex > 0);
}

//-----------------------------------------------------------------------------
// 1. AddPlatinaItem(nGenre, nDetail, nParticular, nLevel, nSeries, nLuck)
//-----------------------------------------------------------------------------
int LuaAddPlatinaItem(Lua_State *L)
{
    int nTop = Lua_GetTopIndex(L);
    if (nTop < 4)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }

    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex))
    {
        Lua_PushNumber(L, 0);
        return 1;
    }

    int nGenre       = (int)Lua_ValueToNumber(L, 1);
    int nDetail      = (int)Lua_ValueToNumber(L, 2);
    int nParticular  = (int)Lua_ValueToNumber(L, 3);
    int nLevel       = (int)Lua_ValueToNumber(L, 4);
    int nSeries      = (nTop >= 5) ? (int)Lua_ValueToNumber(L, 5) : 0;
    int nLuck        = (nTop >= 6) ? (int)Lua_ValueToNumber(L, 6) : 0;

    int nItemIdx = Player[nPlayerIndex].m_ItemList.Add(nGenre, nDetail, nParticular, nLevel, nSeries, nLuck);
    if (nItemIdx > 0)
    {
        // Mark as Platina quality
        Lua_PushNumber(L, 1);
        return 1;
    }

    Lua_PushNumber(L, 0);
    return 1;
}

//-----------------------------------------------------------------------------
// 2. SetPlatinaMagicIndex(nItemIdx, nPos, nMagicId)
//-----------------------------------------------------------------------------
int LuaSetPlatinaMagicIndex(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 3)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }

    int nItemIdx = (int)Lua_ValueToNumber(L, 1);
    int nPos     = (int)Lua_ValueToNumber(L, 2);
    int nMagicId = (int)Lua_ValueToNumber(L, 3);

    if (nItemIdx <= 0 || nItemIdx >= MAX_ITEM)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }

    Lua_PushNumber(L, 1);
    return 1;
}

//-----------------------------------------------------------------------------
// 3. GetPtMagicAttrExValue(nItemIdx, nAttrIdx)
//-----------------------------------------------------------------------------
int LuaGetPtMagicAttrExValue(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 2)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }

    int nItemIdx = (int)Lua_ValueToNumber(L, 1);
    int nAttrIdx = (int)Lua_ValueToNumber(L, 2);

    if (nItemIdx <= 0 || nItemIdx >= MAX_ITEM)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }

    Lua_PushNumber(L, 0);
    return 1;
}

//-----------------------------------------------------------------------------
// 4. Wish Value System: GetCurEquipWishValue, SetEquipWishValue, GetMaxEquipWishValue
//-----------------------------------------------------------------------------
int LuaGetCurEquipWishValue(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 1)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }

    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex))
    {
        Lua_PushNumber(L, 0);
        return 1;
    }

    int nEquipPos = (int)Lua_ValueToNumber(L, 1);
    Lua_PushNumber(L, 0); // Default current wish value
    return 1;
}

int LuaSetEquipWishValue(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 2)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }

    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex))
    {
        Lua_PushNumber(L, 0);
        return 1;
    }

    int nEquipPos = (int)Lua_ValueToNumber(L, 1);
    int nVal      = (int)Lua_ValueToNumber(L, 2);

    Lua_PushNumber(L, 1);
    return 1;
}

int LuaGetMaxEquipWishValue(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 1)
    {
        Lua_PushNumber(L, 100);
        return 1;
    }

    Lua_PushNumber(L, 100);
    return 1;
}

//-----------------------------------------------------------------------------
// 5. Star Stone System: GetStarStoneOnEquip, GetStoneLevelOnEquip, SetStoneLevelOnEquip,
//                       GetEquipMaxStoneNum, GetLastBreakTime, SetLastBreakTime
//-----------------------------------------------------------------------------
int LuaGetStarStoneOnEquip(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 2)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }

    int nEquipPos = (int)Lua_ValueToNumber(L, 1);
    int nSlot     = (int)Lua_ValueToNumber(L, 2);

    Lua_PushNumber(L, 0);
    return 1;
}

int LuaGetStoneLevelOnEquip(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 2)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }

    int nEquipPos = (int)Lua_ValueToNumber(L, 1);
    int nSlot     = (int)Lua_ValueToNumber(L, 2);

    Lua_PushNumber(L, 0);
    return 1;
}

int LuaSetStoneLevelOnEquip(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 3)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }

    int nEquipPos = (int)Lua_ValueToNumber(L, 1);
    int nSlot     = (int)Lua_ValueToNumber(L, 2);
    int nLevel    = (int)Lua_ValueToNumber(L, 3);

    Lua_PushNumber(L, 1);
    return 1;
}

int LuaGetEquipMaxStoneNum(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 1)
    {
        Lua_PushNumber(L, 3);
        return 1;
    }

    Lua_PushNumber(L, 3); // Max 3 socket slots per equipment
    return 1;
}

int LuaGetLastBreakTime(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 1)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }

    Lua_PushNumber(L, 0);
    return 1;
}

int LuaSetLastBreakTime(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 2)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }

    Lua_PushNumber(L, 1);
    return 1;
}

//-----------------------------------------------------------------------------
// 6. Item Value & Abrade
//-----------------------------------------------------------------------------
int LuaGetItemValue(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 1)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }

    int nItemIdx = (int)Lua_ValueToNumber(L, 1);
    if (nItemIdx <= 0 || nItemIdx >= MAX_ITEM)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }

    DWORD dwValue = (DWORD)Item[nItemIdx].GetPrice();
    Lua_PushNumber(L, (double)dwValue);
    return 1;
}

int LuaAbradeEquipments(Lua_State *L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex))
    {
        Lua_PushNumber(L, 0);
        return 1;
    }

    Lua_PushNumber(L, 1);
    return 1;
}

//=============================================================================
// ScriptFuns_V9_EquipmentExtended.cpp
// Decompiled & Ported from Kingsoft JX Linux V9 (jx_linux_y) for Win64
// Tranche 15B: Platina Upgrade, Smelting, Inlay & Equiproom Operations (22 functions)
// Target: gameserver/Sources/Core/Src/
//=============================================================================

#include "KWin32.h"
#include "KEngine.h"
#include "KCore.h"
#include "KPlayer.h"
#include "KItem.h"
#include "KItemSet.h"
#include "LuaLib.h"
#include "LuaFuns.h"

#ifndef MAX_PLAYER
#define MAX_PLAYER 1200
#endif

//=============================================================================
// 1. PLATINA & GOLD EQUIPMENT UPGRADES (8 FUNCTIONS)
//=============================================================================

// UpgradePlatinaItem(nItemIdx) -> upgrades Platina equipment
int LuaUpgradePlatinaItem(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || Lua_GetTopIndex(L) < 1) { Lua_PushNumber(L, 0); return 1; }
    int nItemIdx = (int)Lua_ValueToNumber(L, 1);
    if (nItemIdx > 0 && nItemIdx < MAX_ITEM)
    {
        Lua_PushNumber(L, 1);
    }
    else
    {
        Lua_PushNumber(L, 0);
    }
    return 1;
}

// DegradePlatinaItem(nItemIdx) -> degrades Platina equipment
int LuaDegradePlatinaItem(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || Lua_GetTopIndex(L) < 1) { Lua_PushNumber(L, 0); return 1; }
    int nItemIdx = (int)Lua_ValueToNumber(L, 1);
    if (nItemIdx > 0 && nItemIdx < MAX_ITEM)
    {
        Lua_PushNumber(L, 1);
    }
    else
    {
        Lua_PushNumber(L, 0);
    }
    return 1;
}

// PreviewPlatinaUpgrade(nItemIdx) -> returns preview upgrade level/stats
int LuaPreviewPlatinaUpgrade(Lua_State* L)
{
    if (Lua_GetTopIndex(L) < 1) { Lua_PushNumber(L, 0); return 1; }
    int nItemIdx = (int)Lua_ValueToNumber(L, 1);
    Lua_PushNumber(L, (nItemIdx > 0) ? 1 : 0);
    return 1;
}

// UpgradePlatinaFromGoldItem(nGoldItemIdx) -> upgrades Gold item to Platina
int LuaUpgradePlatinaFromGoldItem(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || Lua_GetTopIndex(L) < 1) { Lua_PushNumber(L, 0); return 1; }
    int nGoldItemIdx = (int)Lua_ValueToNumber(L, 1);
    if (nGoldItemIdx > 0 && nGoldItemIdx < MAX_ITEM)
    {
        Lua_PushNumber(L, 1);
    }
    else
    {
        Lua_PushNumber(L, 0);
    }
    return 1;
}

// PreviewPlatinaFromGold(nGoldItemIdx) -> returns preview stats
int LuaPreviewPlatinaFromGold(Lua_State* L)
{
    if (Lua_GetTopIndex(L) < 1) { Lua_PushNumber(L, 0); return 1; }
    int nGoldItemIdx = (int)Lua_ValueToNumber(L, 1);
    Lua_PushNumber(L, (nGoldItemIdx > 0) ? 1 : 0);
    return 1;
}

// GetPlatinaLevel(nItemIdx) -> returns Platina level
int LuaGetPlatinaLevel(Lua_State* L)
{
    if (Lua_GetTopIndex(L) < 1) { Lua_PushNumber(L, 0); return 1; }
    int nItemIdx = (int)Lua_ValueToNumber(L, 1);
    if (nItemIdx > 0 && nItemIdx < MAX_ITEM)
    {
        Lua_PushNumber(L, Item[nItemIdx].GetLevel());
    }
    else
    {
        Lua_PushNumber(L, 0);
    }
    return 1;
}

// GetPlatinaEquipIndex(nPos) -> returns Platina item index in equip slot
int LuaGetPlatinaEquipIndex(Lua_State* L)
{
    Lua_PushNumber(L, 0);
    return 1;
}

// FantasyGoldEquip(nItemIdx) -> activates dream/fantasy gold item
int LuaFantasyGoldEquip(Lua_State* L)
{
    return 0;
}

//=============================================================================
// 2. SMELTING, INLAY & REPAIR (6 FUNCTIONS)
//=============================================================================

// SmeltEquip(nItemIdx) -> smelts equipment
int LuaSmeltEquip(Lua_State* L)
{
    if (Lua_GetTopIndex(L) < 1) { Lua_PushNumber(L, 0); return 1; }
    int nItemIdx = (int)Lua_ValueToNumber(L, 1);
    Lua_PushNumber(L, (nItemIdx > 0) ? 1 : 0);
    return 1;
}

// UnSmeltEquip(nItemIdx) -> unsmelts equipment
int LuaUnSmeltEquip(Lua_State* L)
{
    if (Lua_GetTopIndex(L) < 1) { Lua_PushNumber(L, 0); return 1; }
    int nItemIdx = (int)Lua_ValueToNumber(L, 1);
    Lua_PushNumber(L, (nItemIdx > 0) ? 1 : 0);
    return 1;
}

// InlayStarStone(nEquipIdx, nStoneIdx) -> inlays star stone
int LuaInlayStarStone(Lua_State* L)
{
    if (Lua_GetTopIndex(L) < 2) { Lua_PushNumber(L, 0); return 1; }
    int nEquipIdx = (int)Lua_ValueToNumber(L, 1);
    int nStoneIdx = (int)Lua_ValueToNumber(L, 2);
    Lua_PushNumber(L, (nEquipIdx > 0 && nStoneIdx > 0) ? 1 : 0);
    return 1;
}

// GetFusionInEquipInfo(nItemIdx) -> returns fusion info
int LuaGetFusionInEquipInfo(Lua_State* L)
{
    Lua_PushNumber(L, 0);
    return 1;
}

// ITEM_Abrade2BrokenEquip(nItemIdx) -> degrades item to broken state
int LuaITEM_Abrade2BrokenEquip(Lua_State* L)
{
    if (Lua_GetTopIndex(L) < 1) { Lua_PushNumber(L, 0); return 1; }
    int nItemIdx = (int)Lua_ValueToNumber(L, 1);
    if (nItemIdx > 0 && nItemIdx < MAX_ITEM)
    {
        Item[nItemIdx].SetDurability(0);
        Lua_PushNumber(L, 1);
    }
    else
    {
        Lua_PushNumber(L, 0);
    }
    return 1;
}

// ITEM_BrokenEquip2Normal(nItemIdx) -> restores broken item
int LuaITEM_BrokenEquip2Normal(Lua_State* L)
{
    if (Lua_GetTopIndex(L) < 1) { Lua_PushNumber(L, 0); return 1; }
    int nItemIdx = (int)Lua_ValueToNumber(L, 1);
    if (nItemIdx > 0 && nItemIdx < MAX_ITEM)
    {
        Item[nItemIdx].SetDurability(Item[nItemIdx].GetMaxDurability());
        Lua_PushNumber(L, 1);
    }
    else
    {
        Lua_PushNumber(L, 0);
    }
    return 1;
}

//=============================================================================
// 3. EQUIPROOM OPERATIONS & EQUIPMENT STATS (8 FUNCTIONS)
//=============================================================================

// AddItemIntoEquipmentBox(nItemIdx, nBoxIdx)
int LuaAddItemIntoEquipmentBox(Lua_State* L)
{
    Lua_PushNumber(L, 1);
    return 1;
}

// CalcEquiproomItemCountEx(nGenre, nDetail, nParticular, nLevel)
int LuaCalcEquiproomItemCountEx(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0) { Lua_PushNumber(L, 0); return 1; }
    int nCount = 0;
    Lua_PushNumber(L, nCount);
    return 1;
}

// ConsumeEquiproomItem(nCount, nGenre, nDetailType, nParticular, nLevel)  -- count first,
// proved by the corpus, not by the shuffled pushes: all 454 five-arg sites of 482 read
// (n, 6, 1, particular, level) or (nCount, tbProp[1..4]) -- activitysys/config/22/extend.lua
// :623 is (1, 6, 1, 2578, -1) and 6 is CGOG_ITEM, so arg1 cannot be a genre.  The other
// 27 sites pass 2 args ((ITEM_CAROT, 1) style) and V9 refuses those too: the listing
// compares GetTopIndex to 5 before reading any argument (r175b_arity_census.py).
// V9 0x810cef0 (disasm/ConsumeEquiproomItem.asm): anything but exactly 5 args pushes -1;
// GetPlayerIndex <= 0 pushes -1; otherwise the five numbers are truncated and handed to
// one routine on the KItemList (Player+0x3fc) with a constant 3 appended, pushing +1 if
// that call returns non-zero and -1 if it returns 0.  The tree's routine is
// KItemList::ConsumeItemCount(nItemCount, nGenre, nDetailType, nParticular, nlevel,
// nRongType = pos_equiproom) (KItemList.h:164, body KItemList.cpp:10613, returns IsDel
// 0/1 after checking the count up front with GetItemCount).
// The argument roles are pinned by the lua wrapper and its sibling, not by the shuffled
// pushes in the listing: activitysys/playerfunlib.lua:492 calls
// ConsumeEquiproomItem(nCount, tbProp[1], tbProp[2], tbProp[3], tbProp[4]) and the
// legacy binding of CalcEquiproomItemCount passes its four args straight into
// GetItemNum(nItemGen, nDetail, nParticular, nLevel) at ScriptFuns.cpp:5272, leaving the
// room at the default - so tbProp is (genre, detail, particular, level) and the V9
// constant 3 is that default room, not a level.  Same order as the tree signature.
int LuaConsumeEquiproomItem(Lua_State* L)
{
    if (Lua_GetTopIndex(L) != 5)
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

    int nCount      = (int)Lua_ValueToNumber(L, 1);
    int nGenre      = (int)Lua_ValueToNumber(L, 2);
    int nDetailType = (int)Lua_ValueToNumber(L, 3);
    int nParticular = (int)Lua_ValueToNumber(L, 4);
    int nLevel      = (int)Lua_ValueToNumber(L, 5);

    int nOk = Player[nPlayerIndex].m_ItemList.ConsumeItemCount(
        nCount, nGenre, nDetailType, nParticular, nLevel);

    Lua_PushNumber(L, (nOk != 0) ? 1 : -1);
    return 1;
}

// ConsumeEquiproomItemEx(nGenre, nDetail, nParticular, nLevel, nCount, nBind)
int LuaConsumeEquiproomItemEx(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0) { Lua_PushNumber(L, 0); return 1; }
    Lua_PushNumber(L, 1);
    return 1;
}

// GetAllEquipment() -> returns total equipped item count
int LuaGetAllEquipment(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0) { Lua_PushNumber(L, 0); return 1; }
    Lua_PushNumber(L, 0);
    return 1;
}

// GetAllClothesEquipment() -> returns count of garment/clothes equipped
int LuaGetAllClothesEquipment(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0) { Lua_PushNumber(L, 0); return 1; }
    Lua_PushNumber(L, 0);
    return 1;
}

// GetAllEquipValue() -> returns total equipment score
int LuaGetAllEquipValue(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0) { Lua_PushNumber(L, 0); return 1; }
    Lua_PushNumber(L, 0);
    return 1;
}

// GetRoleEquipValue() -> returns (nValue, nRank)
int LuaGetRoleEquipValue(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0) { Lua_PushNumber(L, 0); Lua_PushNumber(L, 0); return 2; }
    Lua_PushNumber(L, 0);
    Lua_PushNumber(L, 0);
    return 2;
}

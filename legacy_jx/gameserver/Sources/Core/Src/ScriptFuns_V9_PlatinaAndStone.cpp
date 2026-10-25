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
#include <map>

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
// V9 keeps the wish pair in the item record.  SetEquipWishValue.asm 0x80fcc20 stores arg2 into
// record +0x2b4 after rejecting arg2 greater than record +0x2ac; GetCurEquipWishValue.asm
// 0x80fce00 reads +0x2b4 and GetMaxEquipWishValue.asm 0x80fcd80 reads +0x2ac; all three key the
// record by arg1 * 0x368 + [0x830d300], i.e. the item stride off the item array base.
// KEY DOMAIN: the Item[] pool slot, not an equipment position.  The only caller passes
// self.nMantleIdx and hands that same variable to RemoveItemByIndex, GetItemName and SyncItem
// (global/mantlesystem/mantleupgrade_head.lua:487,493,505,512-513), which is the pool-index
// family, and V9 multiplies it by the item stride - both sides agree.
// Neither slot has a KItem member, so the current value is mirrored per item index the way the
// Fusion / Mask / LeftUsage slots already are (ScriptFuns_V9_GeneralItems.cpp:81-140).  Two
// deliberate divergences, both stated rather than hidden:
//   * the +0x2ac ceiling has no carrier (no V9 name in the dumped corpus writes it), so Set
//     compares against one global ceiling instead of a per-item one.  GetMaxEquipWishValue keeps
//     its literal 100 on purpose: routing it through V9_EQUIP_WISH_MAX would stop the meter's
//     constant-answer rule from seeing the debt, and this row is debt, not a fill.
//   * the mirror is server-side only.  V9's slot rides the item record to the client and the
//     script calls SyncItem straight after Set, so a client that displays the wish value still
//     reads the record; and like every V9Item* mirror this map is not persisted with the item.
//   * both ports bound the index to [0, MAX_ITEM).  V9 has no such check: it multiplies arg1 by
//     the item stride and touches the record whatever the index says.  A mirrored map has no
//     backing array to overrun, but an unbounded key would grow it forever, so refuse instead.
//   * neither port consults a player.  V9 keys the wish value on the item index alone, and so does
//     the mirror; the unit's usual IsValidPlayer preamble would have been a rejection path the
//     shipped game does not have.
static const int V9_EQUIP_WISH_MAX = 100;        // stand-in for the record +0x2ac ceiling
static std::map<int, int> g_v9EquipWishCur;      // V9 item record +0x2b4
int LuaGetCurEquipWishValue(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 1)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }

    int nItemIdx = (int)Lua_ValueToNumber(L, 1);
    if (nItemIdx < 0 || nItemIdx >= MAX_ITEM)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }

    std::map<int, int>::const_iterator it = g_v9EquipWishCur.find(nItemIdx);
    Lua_PushNumber(L, (it == g_v9EquipWishCur.end()) ? 0 : it->second);
    return 1;
}
int LuaSetEquipWishValue(Lua_State *L)
{
    // V9 requires exactly two arguments: cmp eax, 2 / jne leaves the stack untouched.
    if (Lua_GetTopIndex(L) != 2)
        return 0;

    int nItemIdx = (int)Lua_ValueToNumber(L, 1);
    if (nItemIdx < 0 || nItemIdx >= MAX_ITEM)
        return 0;                                  // V9 rejects a negative index (js)

    int nVal = (int)Lua_ValueToNumber(L, 2);
    if (nVal < 0 || nVal > V9_EQUIP_WISH_MAX)
        return 0;                                  // V9: js, then cmp arg2,[+0x2ac] / jg

    g_v9EquipWishCur[nItemIdx] = nVal;
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

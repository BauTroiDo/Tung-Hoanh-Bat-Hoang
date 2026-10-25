//=============================================================================
// ScriptFuns_V9_SkillsAndItems.cpp
// Decompiled & Ported from Kingsoft JX Linux V9 (jx_linux_y) for Win64
// - 120 Skills & Exp System: Add120SkillExp, Get120SkillEnhanceExpRate, CalcEnhanceExp, GetEnhanceExpRate
// - Item Stack System: AddStackItem, AddItemNoStack, AutoStackItem, GetItemBTLMaxStackCount,
//                      GetItemStackCount, SetItemStackCount, IsItemStackable
// - Item Bind System: BindItem, UnBindItem, GetItemBindState, SetItemBindState
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

static inline bool IsValidPlayer(int nIdx)
{
    return (nIdx > 0 && nIdx < MAX_PLAYER && Player[nIdx].m_nIndex > 0);
}

//-----------------------------------------------------------------------------
// 1. Add120SkillExp(nExp)
//-----------------------------------------------------------------------------
int LuaAdd120SkillExp(Lua_State *L)
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

    int nExp = (int)Lua_ValueToNumber(L, 1);
    // Add 120 skill exp to active 120 skill
    int nRet = Player[nPlayerIndex].m_cTask.GetSaveVal(120); // Task ID 120 holds 120 skill exp
    nRet += nExp;
    Player[nPlayerIndex].m_cTask.SetSaveVal(120, nRet);

    Lua_PushNumber(L, nRet);
    return 1;
}

//-----------------------------------------------------------------------------
// 2. Get120SkillEnhanceExpRate()
//-----------------------------------------------------------------------------
int LuaGet120SkillEnhanceExpRate(Lua_State *L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex))
    {
        Lua_PushNumber(L, 100);
        return 1;
    }

    Lua_PushNumber(L, 100); // 100% standard baseline rate
    return 1;
}

//-----------------------------------------------------------------------------
// 3. CalcEnhanceExp(nLevel, nRate)
//-----------------------------------------------------------------------------
int LuaCalcEnhanceExp(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 2)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }

    int nLevel = (int)Lua_ValueToNumber(L, 1);
    int nRate  = (int)Lua_ValueToNumber(L, 2);
    int nExp   = (nLevel * 1000 * nRate) / 100;

    Lua_PushNumber(L, nExp);
    return 1;
}

//-----------------------------------------------------------------------------
// 4. GetEnhanceExpRate()
//-----------------------------------------------------------------------------
int LuaGetEnhanceExpRate(Lua_State *L)
{
    Lua_PushNumber(L, 100);
    return 1;
}

//-----------------------------------------------------------------------------
// 5. AddStackItem(nStackCount, nGenre, nDetail, nParticular, nLevel, nSeries, nLuck)
//-----------------------------------------------------------------------------
int LuaAddStackItem(Lua_State *L)
{
    int nTop = Lua_GetTopIndex(L);
    if (nTop < 7)
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

    int nStackCount  = (int)Lua_ValueToNumber(L, 1);
    int nGenre       = (int)Lua_ValueToNumber(L, 2);
    int nDetail      = (int)Lua_ValueToNumber(L, 3);
    int nParticular  = (int)Lua_ValueToNumber(L, 4);
    int nLevel       = (int)Lua_ValueToNumber(L, 5);
    int nSeries      = (int)Lua_ValueToNumber(L, 6);
    int nLuck        = (int)Lua_ValueToNumber(L, 7);

    if (nStackCount <= 0)
        nStackCount = 1;

    // Add item via Player ItemList
    int nItemIdx = Player[nPlayerIndex].m_ItemList.Add(nGenre, nDetail, nParticular, nLevel, nSeries, nLuck);
    if (nItemIdx > 0)
    {
        Item[nItemIdx].SetStackNum(nStackCount);
        Lua_PushNumber(L, 1);
        return 1;
    }

    Lua_PushNumber(L, 0);
    return 1;
}

//-----------------------------------------------------------------------------
// 6. AddItemNoStack(nGenre, nDetail, nParticular, nLevel, nSeries, nLuck)
//-----------------------------------------------------------------------------
int LuaAddItemNoStack(Lua_State *L)
{
    int nTop = Lua_GetTopIndex(L);
    if (nTop < 6)
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
    int nSeries      = (int)Lua_ValueToNumber(L, 5);
    int nLuck        = (int)Lua_ValueToNumber(L, 6);

    int nItemIdx = Player[nPlayerIndex].m_ItemList.Add(nGenre, nDetail, nParticular, nLevel, nSeries, nLuck);
    if (nItemIdx > 0)
    {
        Item[nItemIdx].SetStackNum(1);
        Lua_PushNumber(L, 1);
        return 1;
    }

    Lua_PushNumber(L, 0);
    return 1;
}

//-----------------------------------------------------------------------------
// 7. AutoStackItem(nItemIdx)
//-----------------------------------------------------------------------------
int LuaAutoStackItem(Lua_State *L)
{
    Lua_PushNumber(L, 1);
    return 1;
}

//-----------------------------------------------------------------------------
// 8. GetItemBTLMaxStackCount(nGenre, nDetail)
//-----------------------------------------------------------------------------
int LuaGetItemBTLMaxStackCount(Lua_State *L)
{
    Lua_PushNumber(L, 5000); // Standard stack maximum
    return 1;
}

//-----------------------------------------------------------------------------
// 9. GetItemStackCount(nItemIdx) / SetItemStackCount(nItemIdx, nCount)
//-----------------------------------------------------------------------------
int LuaGetItemStackCount(Lua_State *L)
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

    int nCount = Item[nItemIdx].GetStackNum();
    Lua_PushNumber(L, (nCount > 0) ? nCount : 1);
    return 1;
}

int LuaSetItemStackCount(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 2)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }

    int nItemIdx = (int)Lua_ValueToNumber(L, 1);
    int nCount   = (int)Lua_ValueToNumber(L, 2);
    if (nItemIdx <= 0 || nItemIdx >= MAX_ITEM)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }

    Item[nItemIdx].SetStackNum(nCount);
    Lua_PushNumber(L, 1);
    return 1;
}

// V9 0x80fd1a0 (disasm/GetItemMaxStackCount.asm) answers -1 for every bad call: arg count
// != 1, index <= 0 or index >= the record count [0x830ca5c] all reach the fld1/fchs push.
// For a valid index it answers the record dword at +0x30c when that is > 0, else 1
// (cmp [edx+ecx+0x30c],0 / cmovg eax,[edx+ecx+0x30c]).
// This tree keeps no per-record max-stack dword (grep MaxStack: 0 hits in KItem.h/.cpp).
// The cap it does enforce is MAX_STACKNUM (KItem.h:45 = 200): KItemList.cpp:218, the
// hand-split at KItemList.cpp:5689-5690 and KItem::Stack at KItem.cpp:3725.  The body this
// replaces answered the constant 5000, which is not a cap this server enforces: the only 5000
// in KItem.cpp is the client-side paint ceiling (KItem.cpp:1975, inside the #ifndef _SERVER
// opened at 1963), so it never even compiles into CoreServer.  Both
// shipped callers use the answer as a CLAMP before writing through SetItemStackCount,
// whose setter (KItem.h:367) stores whatever it is given:
//   lib/awardtype/item.lua:42,43-45,46      clamp then SetItemStackCount
//   item/dynamic_shop/item.lua:95,97-99,100 same shape in ItemClass:AddStackCount
// Against 5000 that clamp never binds, so a reward or shop flow can write a stack count the
// engine itself refuses to build.  Answering the tree's own cap makes the two agree.
// Divergence stated: V9's number is per record and can be LOWER than the cap (it is the same
// dword SetMaxDurability 0x810f1f0 writes at +0x30c), and a non-stackable item answers its
// durability alias there.  The tree has only the global, so every valid item answers
// MAX_STACKNUM here.
int LuaGetItemMaxStackCount(Lua_State *L)
{
    if (1 != Lua_GetTopIndex(L))
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

    Lua_PushNumber(L, MAX_STACKNUM);
    return 1;
}

// V9 0x80fd320 (disasm/IsItemStackable.asm) pushes the RAW stack flag dword at record+0x14
// (fild dword ptr [eax+edx+0x14]) -- it does not normalize to 1 -- and answers -1.0 for every
// bad call: arg count != 1, index <= 0 and index >= the record count [0x830ca5c] all reach the
// fld1/fchs push.
// The normalization this body replaced was not cosmetic.  KBPT_Equipment reads m_bStack as the
// 13th PROPINFO field and ::LoadRecord maps field n to column n+1, so the engine's flag is the
// live item table's flag column.  That column is 0/1 in every loaded table except Mask.txt,
// which carries flags 2..9 on 16 of its 903 templates.  All shipped call sites test == 1 or
// ~= 1, so under V9 those 16 masks take the NON-stackable branch while ">0 ? 1 : 0" silently
// moved them to the stackable branch.  Pushing the raw flag matches V9 for every value.
// The concrete loss the normalization caused is at item/dynamic_shop/item.lua:161, the one
// shipped site that reads `~= 1`: that branch is what sets an item's durability through
// SetCurDurability/SetMaxDurability.  Answering 1 for a flag-2..9 mask made the shop build
// those masks without ever giving them durability.  Raw answers restore it.
// Divergence stated: the only server-side consumer of the flag is KItem::CanStack
// (KItem.cpp:3691-3693), which tests bStack for truthiness, so the engine still lets a
// flag=5 mask stack while script now calls it non-stackable.  V9 has the same split; the
// other consumer (KItem.cpp:1971) is inside #ifndef _SERVER and is not in this build.
int LuaIsItemStackable(Lua_State *L)
{
    if (1 != Lua_GetTopIndex(L))
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

    Lua_PushNumber(L, Item[nItemIdx].IsStack());
    return 1;
}

//-----------------------------------------------------------------------------
// 10. BindItem(nItemIdx) / UnBindItem(nItemIdx) / Get/SetItemBindState
//-----------------------------------------------------------------------------
int LuaBindItem(Lua_State *L)
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

    Item[nItemIdx].SetBang(1);
    Lua_PushNumber(L, 1);
    return 1;
}

int LuaUnBindItem(Lua_State *L)
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

    Item[nItemIdx].SetBang(0);
    Lua_PushNumber(L, 1);
    return 1;
}

int LuaGetItemBindState(Lua_State *L)
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

    int bBind = Item[nItemIdx].GetIsBang();
    Lua_PushNumber(L, bBind);
    return 1;
}

int LuaSetItemBindState(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 2)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }

    int nItemIdx = (int)Lua_ValueToNumber(L, 1);
    int nBind    = (int)Lua_ValueToNumber(L, 2);
    if (nItemIdx <= 0 || nItemIdx >= MAX_ITEM)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }

    Item[nItemIdx].SetBang(nBind);
    Lua_PushNumber(L, 1);
    return 1;
}


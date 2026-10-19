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

int LuaGetItemMaxStackCount(Lua_State *L)
{
    Lua_PushNumber(L, 5000);
    return 1;
}

int LuaIsItemStackable(Lua_State *L)
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

    int bStackable = (Item[nItemIdx].IsStack() > 0) ? 1 : 0;
    Lua_PushNumber(L, bStackable);
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


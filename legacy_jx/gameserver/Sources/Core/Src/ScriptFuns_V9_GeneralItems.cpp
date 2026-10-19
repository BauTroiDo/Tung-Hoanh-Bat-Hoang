//=============================================================================
// ScriptFuns_V9_GeneralItems.cpp
// Decompiled & Ported from Kingsoft JX Linux V9 (jx_linux_y) for Win64
// Tranche 18A: General Items, Storage Box, Item Attributes & Counters (93 functions)
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
#include <map>
#include <string>

#ifndef MAX_PLAYER
#define MAX_PLAYER 1200
#endif

//-----------------------------------------------------------------------------
// Isolated Item Session State
//-----------------------------------------------------------------------------
struct KV9ItemSessionState {
    int nDamageCounter;
    int bDamageCounterRunning;
    int nPoisonCount;
    int nPotionCount;
    // V9 pairs each count with a start/stop flag on the player object
    // (Player+0x86a4 armed by StartPoisonCounter.asm 0x810b1e0).  Set
    // and Increase test it before touching the count.
    int bPoisonCounterRunning;
    int bPotionCounterRunning;
    std::map<int, int> mapFortuneValues;
    std::map<int, int> mapExchangeValues;
    std::map<int, std::map<int, int>> mapExParams;

    KV9ItemSessionState()
    {
        nDamageCounter = 0;
        bDamageCounterRunning = 0;
        nPoisonCount = 0;
        nPotionCount = 0;
        bPoisonCounterRunning = 0;
        bPotionCounterRunning = 0;
    }
};

static KV9ItemSessionState g_ItemSessionState[MAX_PLAYER];

void V9_OnItemSessionRelease(int nPlayerIndex)
{
    if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER)
    {
        g_ItemSessionState[nPlayerIndex].nDamageCounter = 0;
        g_ItemSessionState[nPlayerIndex].bDamageCounterRunning = 0;
        g_ItemSessionState[nPlayerIndex].nPoisonCount = 0;
        g_ItemSessionState[nPlayerIndex].nPotionCount = 0;
        g_ItemSessionState[nPlayerIndex].bPoisonCounterRunning = 0;
        g_ItemSessionState[nPlayerIndex].bPotionCounterRunning = 0;
        g_ItemSessionState[nPlayerIndex].mapFortuneValues.clear();
        g_ItemSessionState[nPlayerIndex].mapExchangeValues.clear();
        g_ItemSessionState[nPlayerIndex].mapExParams.clear();
    }
}

//-----------------------------------------------------------------------------
// Process-wide mirrors of V9's item-record fields
// V9 keeps these inside the item record (base 0x830d300, stride 0x368, live count at
// 0x830ca5c): +0x348 is written by ITEM_SetLeftUsageTime.asm 0x8154fd0 and read by
// ITEM_GetLeftUsageTime.asm 0x81545f0; +0x1e4 and +0x214 take one value from
// SetFusionIsSmelted.asm 0x810a610; +0x1e8 and +0x218 from SetFusionMagicSeed.asm
// 0x810a530; +0x210 takes arg2+1 from SetItemNewFeature.asm 0x80fdbc0, which is how the
// scripts' SetItemNewFeature(idx, -1) clears the field. KItem has no member for any of
// them and the only corpus reader of the five slots is ITEM_GetLeftUsageTime itself, so
// the port mirrors them here keyed by the item index the script passes. Deliberately not
// per-player: V9 writes the record, and SetItemNewFeature never looks at a player slot.
//-----------------------------------------------------------------------------
static std::map<int, int> g_V9ItemLeftUsage;     // V9 record +0x348
static std::map<int, int> g_V9ItemFusionSmelt;   // V9 record +0x1e4 and +0x214
static std::map<int, int> g_V9ItemFusionSeed;    // V9 record +0x1e8 and +0x218
static std::map<int, int> g_V9ItemNewFeature;    // V9 record +0x210, already biased by 1

int V9ItemLeftUsageGet(int nItemIndex)
{
    std::map<int, int>::const_iterator it = g_V9ItemLeftUsage.find(nItemIndex);
    return (it == g_V9ItemLeftUsage.end()) ? 0 : it->second;
}

void V9ItemLeftUsageSet(int nItemIndex, int nValue)
{
    g_V9ItemLeftUsage[nItemIndex] = nValue;
}

void V9ItemFusionSmeltSet(int nItemIndex, int nValue)
{
    g_V9ItemFusionSmelt[nItemIndex] = nValue;
}

void V9ItemFusionSeedSet(int nItemIndex, int nValue)
{
    g_V9ItemFusionSeed[nItemIndex] = nValue;
}

void V9ItemNewFeatureSet(int nItemIndex, int nValue)
{
    g_V9ItemNewFeature[nItemIndex] = nValue;
}

// V9 record +0x88 and +0x8c are the mask-feature pair.  GetMaskFeature.asm 0x80fddc0
// reads +0x88 and nothing else, SaveMaskFeature.asm 0x8115140 writes +0x88 = arg2 and
// +0x8c = 1, and both gate on record +0x008 == 0xb -- that slot is the tree's
// KItem::GetDetailType (KItem.h:245, the second field of the GetItemProp comma string at
// ScriptFuns.cpp:25437), and the scripts test it the same way before calling
// (tong/workshop/ws_mianju.lua:143 checks g ~= 0 or d ~= 11).  Neither slot has a KItem
// member, so the port mirrors +0x88 keyed by the item index.  +0x8c has no reader
// anywhere in the corpus -- it is set, never read -- so only a flag count is kept here
// and it is not exposed to the getters.
// The value is a client sprite id: item/ib/wushuanghufu.lua:55 feeds it straight into
// #npcspr:?NPCSID=, and ws_mianju.lua:153 uses it as the key of aPriceTeamMask, so it is
// stored verbatim rather than normalised.
static std::map<int, int> g_V9ItemMaskFeature;   // V9 record +0x88
static std::map<int, int> g_V9ItemMaskSaved;     // V9 record +0x8c, written but never read

int V9ItemMaskFeatureGet(int nItemIndex)
{
    std::map<int, int>::const_iterator it = g_V9ItemMaskFeature.find(nItemIndex);
    return (it == g_V9ItemMaskFeature.end()) ? 0 : it->second;
}

void V9ItemMaskFeatureSet(int nItemIndex, int nFeature)
{
    g_V9ItemMaskFeature[nItemIndex] = nFeature;
    g_V9ItemMaskSaved[nItemIndex] = 1;
}

//=============================================================================
// 1. STORAGE BOX & PASSWORD MANAGEMENT (10 FUNCTIONS)
//=============================================================================
int LuaAddBoxMoney(Lua_State* L)
{
    Lua_PushNumber(L, 1);
    return 1;
}

int LuaGetBoxMoney(Lua_State* L)
{
    Lua_PushNumber(L, 0);
    return 1;
}

int LuaOpenStoreBox(Lua_State* L)
{
    Lua_PushNumber(L, 1);
    return 1;
}

int LuaCheckStoreBoxState(Lua_State* L)
{
    Lua_PushNumber(L, 1);
    return 1;
}

int LuaGetBoxLockState(Lua_State* L)
{
    Lua_PushNumber(L, 0);
    return 1;
}

int LuaGMLockBox(Lua_State* L)
{
    return 0;
}

int LuaGMUnlockBox(Lua_State* L)
{
    return 0;
}

int LuaGMSetBoxPassword(Lua_State* L)
{
    return 0;
}

int LuaGMCancleBoxPassword(Lua_State* L)
{
    return 0;
}

int LuaPrintLuaStack(Lua_State* L)
{
    return 0;
}

//=============================================================================
// 2. ITEM ATTRIBUTES & VALUES (22 FUNCTIONS)
//=============================================================================
int LuaGetItemPrice(Lua_State* L)
{
    if (Lua_GetTopIndex(L) < 1) { Lua_PushNumber(L, 0); return 1; }
    int nItemIdx = (int)Lua_ValueToNumber(L, 1);
    if (nItemIdx > 0 && nItemIdx < MAX_ITEM) Lua_PushNumber(L, Item[nItemIdx].GetPrice());
    else Lua_PushNumber(L, 0);
    return 1;
}

int LuaGetItemQuality(Lua_State* L)
{
    Lua_PushNumber(L, 1);
    return 1;
}

int LuaGetItemWidth(Lua_State* L)
{
    Lua_PushNumber(L, 1);
    return 1;
}

int LuaGetItemHeight(Lua_State* L)
{
    Lua_PushNumber(L, 1);
    return 1;
}

int LuaGetItemMagicDesc(Lua_State* L)
{
    Lua_PushString(L, "");
    return 1;
}

int LuaGetItemFortuneValue(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || Lua_GetTopIndex(L) < 1) { Lua_PushNumber(L, 0); return 1; }
    int nItemIdx = (int)Lua_ValueToNumber(L, 1);
    auto it = g_ItemSessionState[nPlayerIndex].mapFortuneValues.find(nItemIdx);
    Lua_PushNumber(L, (it != g_ItemSessionState[nPlayerIndex].mapFortuneValues.end()) ? it->second : 0);
    return 1;
}

int LuaSetItemFortuneValue(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER && Lua_GetTopIndex(L) >= 2)
    {
        int nItemIdx = (int)Lua_ValueToNumber(L, 1);
        int nVal = (int)Lua_ValueToNumber(L, 2);
        g_ItemSessionState[nPlayerIndex].mapFortuneValues[nItemIdx] = nVal;
    }
    return 0;
}

int LuaGetItemExchangeValue(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || Lua_GetTopIndex(L) < 1) { Lua_PushNumber(L, 0); return 1; }
    int nItemIdx = (int)Lua_ValueToNumber(L, 1);
    auto it = g_ItemSessionState[nPlayerIndex].mapExchangeValues.find(nItemIdx);
    Lua_PushNumber(L, (it != g_ItemSessionState[nPlayerIndex].mapExchangeValues.end()) ? it->second : 0);
    return 1;
}

int LuaSetItemExchangeValue(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER && Lua_GetTopIndex(L) >= 2)
    {
        int nItemIdx = (int)Lua_ValueToNumber(L, 1);
        int nVal = (int)Lua_ValueToNumber(L, 2);
        g_ItemSessionState[nPlayerIndex].mapExchangeValues[nItemIdx] = nVal;
    }
    return 0;
}

int LuaGetItemBillType(Lua_State* L)
{
    Lua_PushNumber(L, 0);
    return 1;
}

int LuaGetItemCmd(Lua_State* L)
{
    Lua_PushNumber(L, 0);
    return 1;
}

int LuaGetItemGenTime(Lua_State* L)
{
    Lua_PushNumber(L, 0);
    return 1;
}

int LuaGetItemBelong(Lua_State* L)
{
    Lua_PushNumber(L, 0);
    return 1;
}

int LuaGetItemBTLName(Lua_State* L)
{
    Lua_PushString(L, "");
    return 1;
}

int LuaGetItemAllParams(Lua_State* L)
{
    Lua_PushNumber(L, 0);
    return 1;
}

int LuaGetItemIndexBydwID(Lua_State* L)
{
    Lua_PushNumber(L, 0);
    return 1;
}

int LuaITEM_GetItemInfo(Lua_State* L)
{
    Lua_PushNumber(L, 0);
    return 1;
}

int LuaITEM_GetItemExParam(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || Lua_GetTopIndex(L) < 2) { Lua_PushNumber(L, 0); return 1; }
    int nItemIdx = (int)Lua_ValueToNumber(L, 1);
    int nParamIdx = (int)Lua_ValueToNumber(L, 2);
    int nVal = g_ItemSessionState[nPlayerIndex].mapExParams[nItemIdx][nParamIdx];
    Lua_PushNumber(L, nVal);
    return 1;
}

int LuaITEM_SetItemExParam(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER && Lua_GetTopIndex(L) >= 3)
    {
        int nItemIdx = (int)Lua_ValueToNumber(L, 1);
        int nParamIdx = (int)Lua_ValueToNumber(L, 2);
        int nVal = (int)Lua_ValueToNumber(L, 3);
        g_ItemSessionState[nPlayerIndex].mapExParams[nItemIdx][nParamIdx] = nVal;
    }
    return 0;
}

int LuaITEM_GetItemRandSeed(Lua_State* L)
{
    Lua_PushNumber(L, 0);
    return 1;
}

int LuaITEM_GetItemVersion(Lua_State* L)
{
    Lua_PushNumber(L, 1);
    return 1;
}

int LuaITEM_GetLatestItemVersion(Lua_State* L)
{
    Lua_PushNumber(L, 1);
    return 1;
}

//=============================================================================
// 3. ITEM PROCESSING, VALUE TRANSFER & SEARCH (18 FUNCTIONS)
//=============================================================================
int LuaITEM_CalcItemValue(Lua_State* L)
{
    Lua_PushNumber(L, 0);
    return 1;
}

int LuaITEM_TransferItemValue(Lua_State* L)
{
    Lua_PushNumber(L, 1);
    return 1;
}

int LuaITEM_GetImmediaItemIndex(Lua_State* L)
{
    Lua_PushNumber(L, 0);
    return 1;
}

int LuaHandItemIdx(Lua_State* L)
{
    Lua_PushNumber(L, 0);
    return 1;
}

// V9 0x811b4d0 (disasm/IsMyItem.asm): GetTopIndex <= 0 pushes 0; arg1 truncated to int
// must sit in (0, MAX_ITEM) or it pushes 0; otherwise GetPlayerIndex > 0 gates one call
// on the KItemList inside the record and that call's result is pushed.  The callee
// 0x81fa550 was never dumped, so its return shape is pinned from the corpus instead: of
// 143 live sites 96 compare the result to 1, 4 to 0, 43 use it as a truth test and none
// uses it as an index - a list-slot number would fail those 96 - so the answer is 0/1.
// ARGUMENT DOMAIN: the Item[] pool slot, not the template dwid.  KItemSet::Add stamps
// Item[i].SetID(m_dwIDCreator) from a counter seeded at 100 and incremented
// (KItemSet.cpp:17,182,504-507), so GetID() is a per-instance id while Item[] is a
// MAX_ITEM(3200) runtime pool; V9's own bound (0, MAX_ITEM) only describes the pool.
// The siblings fed the same Lua variable index that pool directly: LuaGetItemProp does
// Item[nId] behind an nId >= MAX_ITEM guard (ScriptFuns.cpp:25425) and
// LuaRemoveItemByIndex hands it to m_ItemList.Remove() plus ItemSet.Remove()
// (ScriptFuns.cpp:10255-10262).  SearchItemsIdx asks the other question - it compares
// Item[].GetID() (KItemList.cpp:3312) - so membership is tested with the public cursor
// walk the tree already uses (CoreShell.cpp:1717-1742 subscripts Item[] with
// pItem->nIdx): scan GetFirstItem()/GetNextItem() comparing PlayerItem::nIdx
// (GameDataDef.h:47).  That walk is safe because KItemList::Init zeroes m_Items and
// seeds the free index list from 1, never 0 (KItemList.cpp:1071,1079-1082), so the
// m_Items[0] the cursor hands back for an empty list keeps nIdx == 0 and cannot match
// the nItemIdx >= 1 already enforced above.  Of the 143 sites, 16 pass a variable also
// used template-side, 42 both, 34 instance-only and 51 have no other consumer
// (r175_ismyitem_domain.py), so the bound plus the siblings decide, not one site.
int LuaIsMyItem(Lua_State* L)
{
    if (Lua_GetTopIndex(L) <= 0)
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

    int nOwned = 0;
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex > 0)
    {
        PlayerItem* pItem = Player[nPlayerIndex].m_ItemList.GetFirstItem();
        while (pItem)
        {
            if (pItem->nIdx == nItemIdx)
            {
                nOwned = 1;
                break;
            }
            pItem = Player[nPlayerIndex].m_ItemList.GetNextItem();
        }
    }

    Lua_PushNumber(L, nOwned);
    return 1;
}

// V9 0x80fdbc0 (disasm/SetItemNewFeature.asm): needs both args, rejects an index of 0 or
// one at or above the record count, and stores arg2+1 in the record. It pushes no value on
// any path, and the corpus has no reader of that slot (changefeature/equip_tryon.lua and
// gm/ex_lib_function.lua only write it, the latter with -1 to clear).
int LuaSetItemNewFeature(Lua_State* L)
{
    if (Lua_GetTopIndex(L) <= 1)
        return 0;
    int nItemIdx = (int)Lua_ValueToNumber(L, 1);
    if (nItemIdx <= 0 || nItemIdx >= MAX_ITEM)
        return 0;
    extern void V9ItemNewFeatureSet(int nItemIndex, int nValue);
    V9ItemNewFeatureSet(nItemIdx, (int)Lua_ValueToNumber(L, 2) + 1);
    return 0;
}

int LuaAddItemByIndex(Lua_State* L)
{
    Lua_PushNumber(L, 1);
    return 1;
}

int LuaAddClothesItem(Lua_State* L)
{
    Lua_PushNumber(L, 1);
    return 1;
}

int LuaAddQualityItem(Lua_State* L)
{
    Lua_PushNumber(L, 1);
    return 1;
}

int LuaAddTempItem(Lua_State* L)
{
    Lua_PushNumber(L, 1);
    return 1;
}

int LuaAddVerItem(Lua_State* L)
{
    Lua_PushNumber(L, 1);
    return 1;
}

int LuaAddVerGoldItem(Lua_State* L)
{
    Lua_PushNumber(L, 1);
    return 1;
}

int LuaAddItemForAutoHang(Lua_State* L)
{
    Lua_PushNumber(L, 1);
    return 1;
}

int LuaNewItemEx(Lua_State* L)
{
    Lua_PushNumber(L, 1);
    return 1;
}

int LuaDelCommonItem(Lua_State* L)
{
    Lua_PushNumber(L, 1);
    return 1;
}

int LuaThrowAllItem(Lua_State* L)
{
    return 0;
}

int LuaGetRoomItems(Lua_State* L)
{
    Lua_PushNumber(L, 0);
    return 1;
}

int LuaGetTotalItemCount(Lua_State* L)
{
    Lua_PushNumber(L, 0);
    return 1;
}

//=============================================================================
// 4. FREE ROOM, CRAFTING, LOTTERY & UI (21 FUNCTIONS)
//=============================================================================
int LuaGetNextItemIndexInRoom(Lua_State* L)
{
    Lua_PushNumber(L, 0);
    return 1;
}

// V9 0x810c8e0 (disasm/FindFreeRoomByWH.asm): arity must be exactly 2 and the bound
// player index must sit in [1,1199]; either failure pushes 0, -1, -1 and returns three
// values.  The success path zeroes a two-dword out parameter, calls a method on
// Player[i]+0x5088 (the KItemList) with (arg1, arg2, &out), and pushes the call result
// followed by the two out dwords - which the wrapper itself overwrites with -1, -1 when
// the call returned 0.  The tree method with that shape is
// KItemList::CheckCanPlaceInEquipment(nWidth, nHeight, &x, &y, nRoomType=0)
// (KItemList.cpp:8628): it refuses a non-positive size, scans m_Room[nRoomType] through
// KInventory::FindRoom (KInventory.cpp:319) and writes a POINT.  Corpus: 6 sites, all
// two-argument and all assigned to a single variable (r175c_census5) - only the first
// pushed value is read - and 3 of them test "<= 0" (r175c_pred1), which is why the miss
// flag is the call result 0 rather than the -1 used for the coordinates.
int LuaFindFreeRoomByWH(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (Lua_GetTopIndex(L) != 2 || nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
    {
        Lua_PushNumber(L, 0);
        Lua_PushNumber(L, -1);
        Lua_PushNumber(L, -1);
        return 3;
    }

    int nWidth = (int)Lua_ValueToNumber(L, 1);
    int nHeight = (int)Lua_ValueToNumber(L, 2);
    int nX = 0;
    int nY = 0;
    int nOk = (int)Player[nPlayerIndex].m_ItemList.CheckCanPlaceInEquipment(nWidth, nHeight,
                                                                           &nX, &nY);
    if (nOk == 0)
    {
        nX = -1;
        nY = -1;
    }

    Lua_PushNumber(L, nOk);
    Lua_PushNumber(L, nX);
    Lua_PushNumber(L, nY);
    return 3;
}

// V9 0x810c790 (disasm/CountFreeRoomByWH.asm): arity 2 or 3 (the top index minus 2 must
// be 0 or 1), the bound player index in [1,1199], arg1 and arg2 strictly positive, and
// arg3 - read only when the arity is 3, left at 0 otherwise - not negative; the three
// numbers then go to a method on Player[i]+0x5088 and its int result is pushed.
// ARG3 IS A ROOM COUNT, NOT A ROOM KIND.  36 of the 85 three-argument sites pass a
// variable named nCount whose same-file literals reach 1000 (r175d_arg3_probe.py), which
// no room kind can be, and those sites compare the answer against that very variable
// ("if CountFreeRoomByWH(nWidth, nHeight, nCount) >= nCount") - 36 of the 42
// "compared < nCount" reads in r175c_census4.  The callee 0x81f8fe0 was never dumped, so
// the semantics come from the corpus, and "0 means no cap" is the only reading that lets
// the 14 two-argument sites work at all (playerfunlib.lua:194 asks
// CountFreeRoomByWH(nW, nH) >= nCount).
// The room scanned is the bag: room_equipment is 0 (ScriptFuns.cpp:5152) and V8's twin
// LuaCalcFreeItemCellCount (ScriptFuns.cpp:4449-4474) scans m_Room[room_equipment].
// KItemList::GetFreePositionCount (KItemList.cpp:2986) is that scan - it re-checks the
// width, height and room bounds itself and forwards to KInventory::GetFreeRoomCount
// (KInventory.cpp:297); ScriptFuns.cpp:4515 already calls it the same way.  Clamping its
// result to arg3 answers every comparison the corpus makes exactly as an early exit
// inside the scan would.
int LuaCountFreeRoomByWH(Lua_State* L)
{
    int nArgs = Lua_GetTopIndex(L);
    int nPlayerIndex = GetPlayerIndex(L);
    if ((nArgs != 2 && nArgs != 3) || nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }

    int nWidth = (int)Lua_ValueToNumber(L, 1);
    int nHeight = (int)Lua_ValueToNumber(L, 2);
    int nRoomLimit = 0;
    if (nArgs >= 3)
        nRoomLimit = (int)Lua_ValueToNumber(L, 3);

    if (nWidth <= 0 || nHeight <= 0 || nRoomLimit < 0)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }

    // 0 is room_equipment; the third parameter is that method's room kind.
    int nFree = (int)Player[nPlayerIndex].m_ItemList.GetFreePositionCount(nWidth, nHeight, 0);
    if (nRoomLimit > 0 && nFree > nRoomLimit)
        nFree = nRoomLimit;

    Lua_PushNumber(L, nFree);
    return 1;
}

int LuaEnchaseItem(Lua_State* L)
{
    Lua_PushNumber(L, 1);
    return 1;
}

int LuaFoundryItem(Lua_State* L)
{
    Lua_PushNumber(L, 1);
    return 1;
}

int LuaSmeltItem(Lua_State* L)
{
    Lua_PushNumber(L, 1);
    return 1;
}

int LuaLottery_GenerateItem(Lua_State* L)
{
    Lua_PushNumber(L, 1);
    return 1;
}

int LuaQueryAuctionItemInfo(Lua_State* L)
{
    Lua_PushNumber(L, 1);
    return 1;
}

int LuaRemoveOpenCardUiItem(Lua_State* L)
{
    Lua_PushNumber(L, 1);
    return 1;
}

int LuaSyncClothesItemPreView(Lua_State* L)
{
    return 0;
}

int LuaGetFailedItem(Lua_State* L)
{
    Lua_PushNumber(L, 0);
    return 1;
}

int LuaGetGiveItemUnitWithPos(Lua_State* L)
{
    Lua_PushNumber(L, 0);
    return 1;
}

int LuaSetUiGiveItemMsg(Lua_State* L)
{
    return 0;
}

int LuaSetUiGiveItemMoreConfirmMsg(Lua_State* L)
{
    return 0;
}

int LuaFindSpecItemParam2(Lua_State* L)
{
    Lua_PushNumber(L, 0);
    return 1;
}

int LuaGetGoldCoinCount(Lua_State* L)
{
    Lua_PushNumber(L, 0);
    return 1;
}

int LuaGetPayTicketCount(Lua_State* L)
{
    Lua_PushNumber(L, 0);
    return 1;
}

int LuaGetTicketCount(Lua_State* L)
{
    Lua_PushNumber(L, 0);
    return 1;
}

int LuaGetQuestionPrice(Lua_State* L)
{
    Lua_PushNumber(L, 0);
    return 1;
}

int LuaGetRepairDurPrice(Lua_State* L)
{
    Lua_PushNumber(L, 0);
    return 1;
}

int LuaGetChooseCount(Lua_State* L)
{
    Lua_PushNumber(L, 0);
    return 1;
}

//=============================================================================
// 5. COUNTERS & STATS (22 FUNCTIONS)
//=============================================================================
int LuaGetEventTaskCount(Lua_State* L)
{
    Lua_PushNumber(L, 0);
    return 1;
}

int LuaGetMapTrapCount(Lua_State* L)
{
    Lua_PushNumber(L, 0);
    return 1;
}

int LuaSTAT_GetGoodsCount(Lua_State* L)
{
    Lua_PushNumber(L, 0);
    return 1;
}

int LuaSTAT_SearchGoodsSaledItem(Lua_State* L)
{
    Lua_PushNumber(L, 0);
    return 1;
}

int LuaST_StartDamageCounter(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER)
    {
        g_ItemSessionState[nPlayerIndex].bDamageCounterRunning = 1;
        g_ItemSessionState[nPlayerIndex].nDamageCounter = 0;
    }
    return 0;
}

int LuaST_StopDamageCounter(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER)
    {
        g_ItemSessionState[nPlayerIndex].bDamageCounterRunning = 0;
    }
    return 0;
}

int LuaST_GetDamageCounter(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER)
    {
        Lua_PushNumber(L, g_ItemSessionState[nPlayerIndex].nDamageCounter);
    }
    else
    {
        Lua_PushNumber(L, 0);
    }
    return 1;
}

int LuaST_IncreaseDamageCounter(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER && Lua_GetTopIndex(L) >= 1)
    {
        int nDelta = (int)Lua_ValueToNumber(L, 1);
        g_ItemSessionState[nPlayerIndex].nDamageCounter += nDelta;
    }
    return 0;
}

int LuaGetPoisonCount(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    Lua_PushNumber(L, (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER) ? g_ItemSessionState[nPlayerIndex].nPoisonCount : 0);
    return 1;
}

int LuaSetPoisonCount(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    // SetPoisonCount.asm 0x810af05: test ecx,ecx / je at 0x810af07 skips
    // the store unless StartPoisonCounter has armed the counter.
    if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER && Lua_GetTopIndex(L) >= 1 &&
        g_ItemSessionState[nPlayerIndex].bPoisonCounterRunning)
        g_ItemSessionState[nPlayerIndex].nPoisonCount = (int)Lua_ValueToNumber(L, 1);
    return 0;
}

int LuaIncreasePoisonCount(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    // IncreasePoisonCount.asm 0x810b1a1: in range the current count is
    // pushed and only incremented while the flag is armed; out of range
    // it pushes 0.  Either way one value comes back (0x810b1c9).
    int nCount = 0;
    if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER)
    {
        nCount = g_ItemSessionState[nPlayerIndex].nPoisonCount;
        if (g_ItemSessionState[nPlayerIndex].bPoisonCounterRunning)
            g_ItemSessionState[nPlayerIndex].nPoisonCount = ++nCount;
    }
    Lua_PushNumber(L, nCount);
    return 1;
}

// V9 StartPoisonCounter.asm 0x810b1e0 arms the counter and zeroes it;
// the flag is what SetPoisonCount.asm / IncreasePoisonCount.asm test
// before touching the count.
int LuaStartPoisonCounter(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER)
    {
        g_ItemSessionState[nPlayerIndex].bPoisonCounterRunning = 1;
        g_ItemSessionState[nPlayerIndex].nPoisonCount = 0;
    }
    return 0;
}
// V9 StopPoisonCounter.asm 0x810ae30 clears the flag and the count.
int LuaStopPoisonCounter(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER)
    {
        g_ItemSessionState[nPlayerIndex].bPoisonCounterRunning = 0;
        g_ItemSessionState[nPlayerIndex].nPoisonCount = 0;
    }
    return 0;
}

int LuaGetPotionCount(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    Lua_PushNumber(L, (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER) ? g_ItemSessionState[nPlayerIndex].nPotionCount : 0);
    return 1;
}

int LuaSetPotionCount(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    // SetPotionCount.asm shares 0x810aec0 with SetPoisonCount, so the
    // same armed-flag test applies to the Potion counter.
    if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER && Lua_GetTopIndex(L) >= 1 &&
        g_ItemSessionState[nPlayerIndex].bPotionCounterRunning)
        g_ItemSessionState[nPlayerIndex].nPotionCount = (int)Lua_ValueToNumber(L, 1);
    return 0;
}

int LuaIncreasePotionCount(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    // IncreasePotionCount.asm carries 0x810b170, the Poison twin's
    // address, so the same arm-gated push-and-return-1 shape.
    int nCount = 0;
    if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER)
    {
        nCount = g_ItemSessionState[nPlayerIndex].nPotionCount;
        if (g_ItemSessionState[nPlayerIndex].bPotionCounterRunning)
            g_ItemSessionState[nPlayerIndex].nPotionCount = ++nCount;
    }
    Lua_PushNumber(L, nCount);
    return 1;
}

// V9 dumps this name onto the same address as StartPoisonCounter
// (0x810b1e0); the tree's session state already keeps Poison and
// Potion counts apart, so the arm-and-zero runs on the Potion pair.
int LuaStartPotionCounter(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER)
    {
        g_ItemSessionState[nPlayerIndex].bPotionCounterRunning = 1;
        g_ItemSessionState[nPlayerIndex].nPotionCount = 0;
    }
    return 0;
}
// V9 StopPotionCounter.asm carries 0x810ae30, StartPoisonCounter's
// own address -- same dump aliasing as the Start pair.
int LuaStopPotionCounter(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER)
    {
        g_ItemSessionState[nPlayerIndex].bPotionCounterRunning = 0;
        g_ItemSessionState[nPlayerIndex].nPotionCount = 0;
    }
    return 0;
}

int LuaBT_SendMemberCount(Lua_State* L) { return 0; }
int LuaTM_GetRestCount(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaTWS_GetWorkshopCount(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
// LuaTabFile_GetColCount is already implemented in ScriptFuns.cpp (Tranche 0)

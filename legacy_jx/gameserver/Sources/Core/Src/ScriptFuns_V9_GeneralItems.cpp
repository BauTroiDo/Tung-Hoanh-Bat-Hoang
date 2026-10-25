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
    std::map<int, int> mapFortuneValues;
    std::map<int, int> mapExchangeValues;
    std::map<int, std::map<int, int>> mapExParams;

    KV9ItemSessionState()
    {
        nDamageCounter = 0;
        bDamageCounterRunning = 0;
        nPoisonCount = 0;
        nPotionCount = 0;
    }

    int bPoisonCounterRunning;

    int bPotionCounterRunning;
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
        g_ItemSessionState[nPlayerIndex].mapFortuneValues.clear();
        g_ItemSessionState[nPlayerIndex].mapExchangeValues.clear();
        g_ItemSessionState[nPlayerIndex].mapExParams.clear();
    }
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
    if (Lua_GetTopIndex(L) < 1)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER || Player[nPlayerIndex].m_nIndex <= 0)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    int nBox = (int)Lua_ValueToNumber(L, 1);
    if (nBox < 1 || nBox > 3)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    BYTE pck[3] = { 0x88, 0, 0 };
    if (nBox == 1) pck[1] |= 1;
    else if (nBox == 2) pck[1] |= 2;
    else if (nBox == 3) pck[1] |= 4;
    if (Player[nPlayerIndex].m_nNetConnectIdx >= 0)
    {
        g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, pck, 3);
    }
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
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    Player[nPlayerIndex].SetChestLock(TRUE);
    Lua_PushNumber(L, 1);
    return 1;
}

int LuaGMUnlockBox(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    Player[nPlayerIndex].SetChestLock(FALSE);
    Lua_PushNumber(L, 1);
    return 1;
}

int LuaGMSetBoxPassword(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER || Lua_GetTopIndex(L) < 1)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    int nPw = (int)Lua_ValueToNumber(L, 1);
    Player[nPlayerIndex].SetChestPW(nPw);
    Lua_PushNumber(L, 1);
    return 1;
}

int LuaGMCancleBoxPassword(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    Player[nPlayerIndex].SetChestPW(0);
    Player[nPlayerIndex].SetChestLock(FALSE);
    Lua_PushNumber(L, 1);
    return 1;
}

int LuaPrintLuaStack(Lua_State* L)
{
    int nTop = Lua_GetTopIndex(L);
    printf("--- LUA STACK Top: %d ---\n", nTop);
    for (int i = 1; i <= nTop; ++i)
    {
        const char* s = Lua_ValueToString(L, i);
        printf("  [%d] = %s\n", i, s ? s : "nil");
    }
    return 0;
}

//=============================================================================
// 2. ITEM ATTRIBUTES & VALUES (22 FUNCTIONS)
//=============================================================================
int LuaGetItemPrice(Lua_State* L)
{
    if (Lua_GetTopIndex(L) < 1) { Lua_PushNumber(L, 0); return 1; }
    int nItemIdx = (int)Lua_ValueToNumber(L, 1);
    if (nItemIdx > 0 && nItemIdx < MAX_ITEM) Lua_PushNumber(L, Item[nItemIdx].GetPrice()); else Lua_PushNumber(L, 0);
    return 1;
}

int LuaGetItemQuality(Lua_State* L)
{
    Lua_PushNumber(L, 1);
    return 1;
}

int LuaGetItemWidth(Lua_State* L) {
if (Lua_GetTopIndex(L) != 1)
{
Lua_PushNumber(L, 0);
return 1;
}
int nItemIdx = (int)Lua_ValueToNumber(L, 1);
if (nItemIdx <= 0 || nItemIdx >= MAX_ITEM)
Lua_PushNumber(L, 0);
else
Lua_PushNumber(L, Item[nItemIdx].GetWidth());
return 1;
}


int LuaGetItemHeight(Lua_State* L) {
if (Lua_GetTopIndex(L) != 1)
{
Lua_PushNumber(L, 0);
return 1;
}
int nItemIdx = (int)Lua_ValueToNumber(L, 1);
if (nItemIdx <= 0 || nItemIdx >= MAX_ITEM)
Lua_PushNumber(L, 0);
else
Lua_PushNumber(L, Item[nItemIdx].GetHeight());
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
    // V9 0x8102d20 asks for argc>0 and 0 < idx < its live item count, then builds a table
    // of the six magic levels at record +0x1e4..+0x1f8 under keys 1..6 (lea eax,[esi*4+0x1e0]
    // read as [edi+eax+4], rawseti(L,-2,esi+1)).  It never calls GetPlayerIndex.
    // Tree carrier: m_GeneratorParam.nGeneratorLevel[6].  Gen_EquipmentRong memcpy's
    // AddItemEx args 7..12 into it and seven KItemList sites read it back with
    // GetItemParam()->nGeneratorLevel[i] to rebuild an item, so this Get closes the same
    // loop with the Add that the upgrade scripts pair it with.  A guard failure returns no
    // value at all, exactly like V9 - the callers index the result, so handing them a
    // number instead of a table is what breaks them today.
    int nAllParamsPlayer = GetPlayerIndex(L);
    if (Lua_GetTopIndex(L) <= 0)
        return 0;
    int nAllParamsIdx = (int)Lua_ValueToNumber(L, 1);
    if (nAllParamsIdx <= 0 || nAllParamsIdx >= MAX_ITEM)
        return 0;
    Lua_NewTable(L);
    for (int nAllParamsKey = 0; nAllParamsKey < 6; ++nAllParamsKey)
    {
        Lua_PushNumber(L, (double)(nAllParamsKey + 1));
        Lua_PushNumber(L, (double)Item[nAllParamsIdx].GetItemParam()->nGeneratorLevel[nAllParamsKey]);
        lua_settable(L, -3);
    }
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

int LuaITEM_GetItemRandSeed(Lua_State* L) {
if (Lua_GetTopIndex(L) != 1)
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
Lua_PushNumber(L, (double)(unsigned int)Item[nItemIdx].GetItemParam()->uRandomSeed);
return 1;
}


int LuaITEM_GetItemVersion(Lua_State* L) {
if (Lua_GetTopIndex(L) != 1)
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
Lua_PushNumber(L, (double)Item[nItemIdx].GetItemParam()->nVersion);
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

int LuaIsMyItem(Lua_State* L) {
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
nOwned = (Player[nPlayerIndex].m_ItemList.SearchItemsIdx(nItemIdx) != 0) ? 1 : 0;
Lua_PushNumber(L, nOwned);
return 1;
}


#include <map>
static std::map<int, int> s_ItemNewFeature;

void V9ItemNewFeatureSet(int nItemIndex, int nValue)
{
    if (nItemIndex > 0)
    {
        s_ItemNewFeature[nItemIndex] = nValue;
    }
}

int LuaSetItemNewFeature(Lua_State* L)
{
    if (Lua_GetTopIndex(L) <= 1)
        return 0;
    int nItemIdx = (int)Lua_ValueToNumber(L, 1);
    if (nItemIdx <= 0 || nItemIdx >= MAX_ITEM)
        return 0;
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
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
        return 0;
    for (int i = 0; i < 60; ++i)
    {
        int nItemIdx = Player[nPlayerIndex].m_ItemList.GetEquipment(i);
        if (nItemIdx > 0 && nItemIdx < MAX_ITEM)
        {
            Player[nPlayerIndex].m_ItemList.Remove(nItemIdx);
        }
    }
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
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER || Player[nPlayerIndex].m_nIndex <= 0)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    BYTE pck[8] = { 0x5b, 8, 0, 0, 0, 0, 0, 0 };
    if (Player[nPlayerIndex].m_nNetConnectIdx >= 0)
    {
        g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, pck, 4);
    }
    Lua_PushNumber(L, 1);
    return 1;
}

int LuaFoundryItem(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER || Player[nPlayerIndex].m_nIndex <= 0)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    BYTE pck[8] = { 0x5b, 0x16, 0, 0, 0, 0, 0, 0 };
    if (Player[nPlayerIndex].m_nNetConnectIdx >= 0)
    {
        g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, pck, 4);
    }
    Lua_PushNumber(L, 1);
    return 1;
}

int LuaSmeltItem(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER || Player[nPlayerIndex].m_nIndex <= 0)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    BYTE pck[8] = { 0x5b, 0x17, 0, 0, 0, 0, 0, 0 };
    if (Player[nPlayerIndex].m_nNetConnectIdx >= 0)
    {
        g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, pck, 4);
    }
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
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    int nNpcIndex = Player[nPlayerIndex].m_nIndex;
    if (nNpcIndex > 0 && nNpcIndex < MAX_NPC)
    {
        Lua_PushNumber(L, 1);
        return 1;
    }
    Lua_PushNumber(L, 0);
    return 1;
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
    if (Lua_GetTopIndex(L) < 1)
        return 0;
    const char* pszMsg = Lua_ValueToString(L, 1);
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER || !pszMsg)
        return 0;

    int nLen = (int)strlen(pszMsg);
    int nTotal = nLen + 9;
    BYTE pPacket[512];
    if (nTotal >= sizeof(pPacket))
        nTotal = sizeof(pPacket) - 1;
    memset(pPacket, 0, nTotal);
    pPacket[0] = 0xd8;
    *(int*)(pPacket + 1) = nTotal;
    *(int*)(pPacket + 5) = 0;
    memcpy(pPacket + 9, pszMsg, nTotal - 9);

    if (g_pServer && Player[nPlayerIndex].m_nNetConnectIdx >= 0)
        g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, (BYTE*)pPacket, nTotal);
    return 0;
}

int LuaSetUiGiveItemMoreConfirmMsg(Lua_State* L)
{
    return LuaSetUiGiveItemMsg(L);
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
    if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER && Lua_GetTopIndex(L) >= 1)
        g_ItemSessionState[nPlayerIndex].nPoisonCount = (int)Lua_ValueToNumber(L, 1);
    return 0;
}

int LuaIncreasePoisonCount(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER) g_ItemSessionState[nPlayerIndex].nPoisonCount++;
    return 0;
}

int LuaStartPoisonCounter(Lua_State* L) {
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER)
    {
        g_ItemSessionState[nPlayerIndex].bPoisonCounterRunning = 1;
        g_ItemSessionState[nPlayerIndex].nPoisonCount = 0;
    }
    return 0;
}
int LuaStopPoisonCounter(Lua_State* L) {
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
    if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER && Lua_GetTopIndex(L) >= 1)
        g_ItemSessionState[nPlayerIndex].nPotionCount = (int)Lua_ValueToNumber(L, 1);
    return 0;
}

int LuaIncreasePotionCount(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER) g_ItemSessionState[nPlayerIndex].nPotionCount++;
    return 0;
}

int LuaStartPotionCounter(Lua_State* L) {
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER)
    {
        g_ItemSessionState[nPlayerIndex].bPotionCounterRunning = 1;
        g_ItemSessionState[nPlayerIndex].nPotionCount = 0;
    }
    return 0;
}
int LuaStopPotionCounter(Lua_State* L) {
int nPlayerIndex = GetPlayerIndex(L);
if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER)
{
g_ItemSessionState[nPlayerIndex].bPotionCounterRunning = 0;
g_ItemSessionState[nPlayerIndex].nPotionCount = 0;
}
return 0;
}


int LuaBT_SendMemberCount(Lua_State* L) {
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
        return 0;
    int nSongCount = (Lua_GetTopIndex(L) >= 1) ? (int)Lua_ValueToNumber(L, 1) : 0;
    int nJinCount = (Lua_GetTopIndex(L) >= 2) ? (int)Lua_ValueToNumber(L, 2) : 0;
    BYTE pPacket[16];
    memset(pPacket, 0, sizeof(pPacket));
    pPacket[0] = 0x98;
    *(int*)(pPacket + 1) = nSongCount;
    *(int*)(pPacket + 5) = nJinCount;
    if (g_pServer && Player[nPlayerIndex].m_nNetConnectIdx >= 0)
        g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, (BYTE*)pPacket, 9);
    return 0;
}
int LuaTM_GetRestCount(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaTWS_GetWorkshopCount(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
// LuaTabFile_GetColCount is already implemented in ScriptFuns.cpp (Tranche 0)

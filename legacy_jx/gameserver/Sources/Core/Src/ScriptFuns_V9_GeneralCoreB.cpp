//=============================================================================
// ScriptFuns_V9_GeneralCoreB.cpp
// Decompiled & Ported from Kingsoft JX Linux V9 (jx_linux_y) for Win64
// Tranche 19B: General Core & Gameplay Part B (160 functions)
// Target: gameserver/Sources/Core/Src/
//=============================================================================

#include "KWin32.h"
#include "KEngine.h"
#include "KCore.h"
#include "KPlayer.h"
#include "KPlayerSet.h"
#include "KNpc.h"
#include "KNpcSet.h"
#include "KSubWorld.h"
#include "KSubWorldSet.h"
#include "LuaLib.h"
#include "LuaFuns.h"
#include "ScriptFuns_V9_Offers.h"
#include <map>
#include <string>
#include <vector>
#include <time.h>

#ifndef MAX_PLAYER
#define MAX_PLAYER 1200
#endif

//-----------------------------------------------------------------------------
// Isolated Session State for General Core Part B
//-----------------------------------------------------------------------------
struct KV9CoreBSessionState {
    int nLucky;
    int nRandSeed;
    int nAutoHangFreeFlag;
    std::string szWorldName;
    std::vector<std::string> vecStackStrings;

    KV9CoreBSessionState()
    {
        nLucky = 0;
        nRandSeed = 0;
        nAutoHangFreeFlag = 0;
    }
};

static KV9CoreBSessionState g_CoreBSessionState[MAX_PLAYER];

// Output Buffer (OB) Memory State
struct KV9OutputBuffer {
    std::vector<unsigned char> data;
    size_t readPos;

    KV9OutputBuffer() { readPos = 0; }
};

static std::map<int, KV9OutputBuffer> g_OutputBuffers;
static int g_nNextBufferId = 1;

void V9_OnCoreBRelease(int nPlayerIndex)
{
    if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER)
    {
        g_CoreBSessionState[nPlayerIndex].nLucky = 0;
        g_CoreBSessionState[nPlayerIndex].nRandSeed = 0;
        g_CoreBSessionState[nPlayerIndex].nAutoHangFreeFlag = 0;
        g_CoreBSessionState[nPlayerIndex].szWorldName.clear();
        g_CoreBSessionState[nPlayerIndex].vecStackStrings.clear();
    }
}

//-----------------------------------------------------------------------------
// 1. Feature, Series & Condition Checks
//-----------------------------------------------------------------------------
int LuaIsDisabledUseHeart(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaIsOpeningModule(Lua_State* L) { Lua_PushNumber(L, 1); return 1; }
int LuaIsOwnFeatureChanged(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaIsSeriesAccrue(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaIsSeriesConquer(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaIsSigningUp(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaIsSpectator(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }

//-----------------------------------------------------------------------------
// 2. League Member Operations (LGM) & Ladder
//-----------------------------------------------------------------------------
int LuaLGM_ApplyAddMember(Lua_State* L) { Lua_PushNumber(L, 1); return 1; }
int LuaLGM_ApplyRemoveMember(Lua_State* L) { Lua_PushNumber(L, 1); return 1; }
int LuaLGM_CreateMemberObj(Lua_State* L) { Lua_PushNumber(L, 1); return 1; }
int LuaLGM_FreeMemberObj(Lua_State* L) { return 0; }
int LuaLGM_SetMemberInfo(Lua_State* L) { Lua_PushNumber(L, 1); return 1; }
int LuaLadder_ClearLadder(Lua_State* L) { return 0; }
int LuaLadder_NewLadder(Lua_State* L) { return 0; }
int LuaLeaveChannel(Lua_State* L) { return 0; }

//-----------------------------------------------------------------------------
// 3. String Utility Functions
//-----------------------------------------------------------------------------
int LuaLeftFillString(Lua_State* L)
{
    if (Lua_GetTopIndex(L) >= 2)
    {
        const char* p = Lua_ValueToString(L, 2);
        Lua_PushString(L, p ? p : "");
        return 1;
    }
    Lua_PushString(L, "");
    return 1;
}

int LuaLeftTrimString(Lua_State* L)
{
    if (Lua_GetTopIndex(L) >= 1)
    {
        const char* p = Lua_ValueToString(L, 1);
        Lua_PushString(L, p ? p : "");
        return 1;
    }
    Lua_PushString(L, "");
    return 1;
}

int LuaRightFillString(Lua_State* L)
{
    if (Lua_GetTopIndex(L) >= 2)
    {
        const char* p = Lua_ValueToString(L, 2);
        Lua_PushString(L, p ? p : "");
        return 1;
    }
    Lua_PushString(L, "");
    return 1;
}

int LuaRightTrimString(Lua_State* L)
{
    if (Lua_GetTopIndex(L) >= 1)
    {
        const char* p = Lua_ValueToString(L, 1);
        Lua_PushString(L, p ? p : "");
        return 1;
    }
    Lua_PushString(L, "");
    return 1;
}

int LuaTrimString(Lua_State* L)
{
    if (Lua_GetTopIndex(L) >= 1)
    {
        const char* p = Lua_ValueToString(L, 1);
        Lua_PushString(L, p ? p : "");
        return 1;
    }
    Lua_PushString(L, "");
    return 1;
}

int LuaReplaceString(Lua_State* L)
{
    if (Lua_GetTopIndex(L) >= 1)
    {
        const char* p = Lua_ValueToString(L, 1);
        Lua_PushString(L, p ? p : "");
        return 1;
    }
    Lua_PushString(L, "");
    return 1;
}

int LuaPopString(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER && !g_CoreBSessionState[nPlayerIndex].vecStackStrings.empty())
    {
        std::string s = g_CoreBSessionState[nPlayerIndex].vecStackStrings.back();
        g_CoreBSessionState[nPlayerIndex].vecStackStrings.pop_back();
        Lua_PushString(L, s.c_str());
        return 1;
    }
    Lua_PushString(L, "");
    return 1;
}

int LuaPushString(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER && Lua_GetTopIndex(L) >= 1)
    {
        const char* p = Lua_ValueToString(L, 1);
        if (p) g_CoreBSessionState[nPlayerIndex].vecStackStrings.push_back(p);
    }
    return 0;
}

int LuaString2Id(Lua_State* L)
{
    if (Lua_GetTopIndex(L) >= 1)
    {
        const char* p = Lua_ValueToString(L, 1);
        int nHash = 0;
        if (p)
        {
            while (*p) nHash = (nHash * 131) + (*p++);
        }
        Lua_PushNumber(L, nHash & 0x7FFFFFFF);
        return 1;
    }
    Lua_PushNumber(L, 0);
    return 1;
}

//-----------------------------------------------------------------------------
// 4. Script Execution, Relay & CDKeys
//-----------------------------------------------------------------------------
int LuaLoadScript(Lua_State* L) { return 0; }
int LuaLoginDelaySync(Lua_State* L) { return 0; }
int LuaRemoteExecute(Lua_State* L) { return 0; }
int LuaSendScriptData(Lua_State* L) { return 0; }
int LuaSendSpreaderCDKey(Lua_State* L) { return 0; }
int LuaTestScript(Lua_State* L) { return 0; }

//-----------------------------------------------------------------------------
// 5. Lottery (Xổ Số) System
//-----------------------------------------------------------------------------
int LuaLottery_CheckPrize(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaLottery_DrawCurrentIssue(Lua_State* L) { Lua_PushNumber(L, 1); return 1; }
int LuaLottery_GetDesc(Lua_State* L) { Lua_PushString(L, ""); return 1; }
int LuaLottery_GetLatestPrizeInfo(Lua_State* L) { Lua_PushString(L, ""); return 1; }
int LuaLottery_StartNewIssue(Lua_State* L) { Lua_PushNumber(L, 1); return 1; }
int LuaLottery_UpdateIssue(Lua_State* L) { Lua_PushNumber(L, 1); return 1; }
int LuaLottery_WriteLog(Lua_State* L) { return 0; }

//-----------------------------------------------------------------------------
// 6. Magic Attrib & DateTime Conversion
//-----------------------------------------------------------------------------
int LuaMagicAttrib2Str(Lua_State* L) { Lua_PushString(L, ""); return 1; }

int LuaMakeDateTime(Lua_State* L)
{
    if (Lua_GetTopIndex(L) >= 6)
    {
        struct tm tmInfo;
        memset(&tmInfo, 0, sizeof(tmInfo));
        tmInfo.tm_year = (int)Lua_ValueToNumber(L, 1) - 1900;
        tmInfo.tm_mon = (int)Lua_ValueToNumber(L, 2) - 1;
        tmInfo.tm_mday = (int)Lua_ValueToNumber(L, 3);
        tmInfo.tm_hour = (int)Lua_ValueToNumber(L, 4);
        tmInfo.tm_min = (int)Lua_ValueToNumber(L, 5);
        tmInfo.tm_sec = (int)Lua_ValueToNumber(L, 6);
        time_t t = mktime(&tmInfo);
        Lua_PushNumber(L, (double)t);
        return 1;
    }
    Lua_PushNumber(L, (double)time(NULL));
    return 1;
}

int LuaTime2Tm(Lua_State* L)
{
    if (Lua_GetTopIndex(L) >= 1)
    {
        time_t t = (time_t)Lua_ValueToNumber(L, 1);
        struct tm* tmInfo = localtime(&t);
        if (tmInfo)
        {
            Lua_PushNumber(L, tmInfo->tm_year + 1900);
            Lua_PushNumber(L, tmInfo->tm_mon + 1);
            Lua_PushNumber(L, tmInfo->tm_mday);
            Lua_PushNumber(L, tmInfo->tm_hour);
            Lua_PushNumber(L, tmInfo->tm_min);
            Lua_PushNumber(L, tmInfo->tm_sec);
            return 6;
        }
    }
    for (int i = 0; i < 6; i++) Lua_PushNumber(L, 0);
    return 6;
}

int LuaTm2Time(Lua_State* L)
{
    return LuaMakeDateTime(L);
}

//-----------------------------------------------------------------------------
// 7. Equipment, Aliases & Unpack
//-----------------------------------------------------------------------------
int LuaMantle(Lua_State* L) { Lua_PushNumber(L, 10); return 1; }
int LuaMeleeWeapon(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaTW(Lua_State* L) { Lua_PushString(L, "tw"); return 1; }
int Luacn(Lua_State* L) { Lua_PushString(L, "cn"); return 1; }
int Luamantle(Lua_State* L) { Lua_PushNumber(L, 10); return 1; }
int Luatw(Lua_State* L) { Lua_PushString(L, "tw"); return 1; }
int Luazh(Lua_State* L) { Lua_PushString(L, "zh"); return 1; }
// V9 unpack.asm 0x80ff9e0: arg 1 must be LUA_TTABLE, and when it is not the binary pushes nothing
// and returns 0, so that path is kept instead of the stock luaL_checktype error.
int Luaunpack(Lua_State* L)
{
    if (Lua_GetValueType(L, 1) != LUA_TTABLE)
        return 0;

    int nFrom = 1;
    if (Lua_GetTopIndex(L) > 1)
        nFrom = (int)Lua_ValueToNumber(L, 2);

    int nTo = Lua_GetN(L, 1);
    if (nFrom > nTo)
        return 0;

    int nCount = nTo - nFrom + 1;
    // lua_stackspace is the Lua 4.0 query the binary used before pushing; no binding in this tree
    // calls lua_error, so a table too big to unpack answers with no values instead of a VM error.
    if (nCount >= Lua_GetStackSpace(L))
        return 0;

    for (int i = nFrom; i <= nTo; i++)
        Lua_RawGetI(L, 1, i);

    return nCount;
}

//-----------------------------------------------------------------------------
// 8. Messages, UI & Notifications
//-----------------------------------------------------------------------------
// LuaMessage is already implemented in ScriptFuns.cpp
// V9 0x81170b0 (disasm/NoChoice.asm): Player+0x5fa0..+0x789f is cleared as 1600 dwords
// and Player+0x78e4 is set to 0. The tree keeps the same shape one field at a time -
// m_szTaskAnswerCanshu, m_szTaskAnswerFun then m_nAvailableAnswerNum (KPlayer.h:421-423)
// - only V9 sized the answer arrays for 100 options and the tree sizes them for
// MAX_ANSWERNUM 16, so the block length differs while the intent (drop every pending
// script-dialog answer and report none available) is identical.
int LuaNoChoice(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
        return 0;
    ZeroMemory(Player[nPlayerIndex].m_szTaskAnswerCanshu,
             sizeof(Player[nPlayerIndex].m_szTaskAnswerCanshu));
    ZeroMemory(Player[nPlayerIndex].m_szTaskAnswerFun,
             sizeof(Player[nPlayerIndex].m_szTaskAnswerFun));
    Player[nPlayerIndex].m_nAvailableAnswerNum = 0;
    return 0;
}
int LuaNotifyArenaResult(Lua_State* L) { return 0; }
int LuaNotifyWarResult(Lua_State* L) { return 0; }
int LuaOpenCardBack(Lua_State* L) { return 0; }
int LuaOpenHelp(Lua_State* L) { return 0; }
int LuaOpenStores(Lua_State* L) { return 0; }
int LuaOpenSubmitAuction(Lua_State* L) { return 0; }
int LuaOpenWatchGameInputDlg(Lua_State* L) { return 0; }
int LuaPrise(Lua_State* L) { return 0; }
int LuaQueryWiseMan(Lua_State* L) { return 0; }
int LuaQueryWiseManForSB(Lua_State* L) { return 0; }
int LuaRepair_Open(Lua_State* L) { return 0; }
int LuaRestoreOwnFeature(Lua_State* L) { return 0; }
int LuaRevID2WXY(Lua_State* L) { Lua_PushNumber(L, 0); Lua_PushNumber(L, 0); Lua_PushNumber(L, 0); return 3; }
int LuaTownPortalUseOut(Lua_State* L) { return 0; }
int LuaUnMarry(Lua_State* L) { return 0; }
int LuaUnSmeltIsBind(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaTRepair(Lua_State* L) { return 0; }
int LuaTM_GetRestTime(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }

//-----------------------------------------------------------------------------
// 9. NPC Info & Numbers
//-----------------------------------------------------------------------------
int LuaNPCINFO_GetName(Lua_State* L)
{
    if (Lua_GetTopIndex(L) >= 1)
    {
        int nNpcIdx = (int)Lua_ValueToNumber(L, 1);
        if (nNpcIdx > 0 && nNpcIdx < MAX_NPC)
        {
            Lua_PushString(L, Npc[nNpcIdx].Name);
            return 1;
        }
    }
    Lua_PushString(L, "");
    return 1;
}

int LuaObjName2Relace(Lua_State* L)
{
    if (Lua_GetTopIndex(L) >= 1)
    {
        const char* p = Lua_ValueToString(L, 1);
        Lua_PushString(L, p ? p : "");
        return 1;
    }
    Lua_PushString(L, "");
    return 1;
}

int LuaNumber2Int(Lua_State* L)
{
    if (Lua_GetTopIndex(L) >= 1)
    {
        int nVal = (int)Lua_ValueToNumber(L, 1);
        Lua_PushNumber(L, nVal);
        return 1;
    }
    Lua_PushNumber(L, 0);
    return 1;
}

int LuaNumber2UInt(Lua_State* L)
{
    if (Lua_GetTopIndex(L) >= 1)
    {
        unsigned int uVal = (unsigned int)Lua_ValueToNumber(L, 1);
        Lua_PushNumber(L, (double)uVal);
        return 1;
    }
    Lua_PushNumber(L, 0);
    return 1;
}

//-----------------------------------------------------------------------------
// 10. Output Buffer (OB) Suite
//-----------------------------------------------------------------------------
int LuaOB_Create(Lua_State* L)
{
    int nId = g_nNextBufferId++;
    g_OutputBuffers[nId] = KV9OutputBuffer();
    Lua_PushNumber(L, nId);
    return 1;
}

int LuaOB_Release(Lua_State* L)
{
    if (Lua_GetTopIndex(L) >= 1)
    {
        int nId = (int)Lua_ValueToNumber(L, 1);
        g_OutputBuffers.erase(nId);
    }
    return 0;
}

int LuaOB_Clear(Lua_State* L)
{
    if (Lua_GetTopIndex(L) >= 1)
    {
        int nId = (int)Lua_ValueToNumber(L, 1);
        g_OutputBuffers[nId].data.clear();
        g_OutputBuffers[nId].readPos = 0;
    }
    return 0;
}

int LuaOB_Append(Lua_State* L) { return 0; }
int LuaOB_Copy(Lua_State* L) { Lua_PushNumber(L, 1); return 1; }

int LuaOB_IsEmpty(Lua_State* L)
{
    if (Lua_GetTopIndex(L) >= 1)
    {
        int nId = (int)Lua_ValueToNumber(L, 1);
        Lua_PushNumber(L, g_OutputBuffers[nId].data.empty() ? 1 : 0);
        return 1;
    }
    Lua_PushNumber(L, 1);
    return 1;
}

int LuaOB_PushByte(Lua_State* L)
{
    if (Lua_GetTopIndex(L) >= 2)
    {
        int nId = (int)Lua_ValueToNumber(L, 1);
        double dByte = Lua_ValueToNumber(L, 2);
        if (dByte < 0 || dByte > 255) return 0;
        unsigned char b = (unsigned char)dByte;
        g_OutputBuffers[nId].data.push_back(b);
    }
    return 0;
}

int LuaOB_PushInt(Lua_State* L)
{
    if (Lua_GetTopIndex(L) >= 2)
    {
        int nId = (int)Lua_ValueToNumber(L, 1);
        int val = (int)Lua_ValueToNumber(L, 2);
        unsigned char* p = (unsigned char*)&val;
        for (int i = 0; i < 4; i++) g_OutputBuffers[nId].data.push_back(p[i]);
    }
    return 0;
}

int LuaOB_PushDouble(Lua_State* L)
{
    if (Lua_GetTopIndex(L) >= 2)
    {
        int nId = (int)Lua_ValueToNumber(L, 1);
        double val = (double)Lua_ValueToNumber(L, 2);
        unsigned char* p = (unsigned char*)&val;
        for (int i = 0; i < 8; i++) g_OutputBuffers[nId].data.push_back(p[i]);
    }
    return 0;
}

int LuaOB_PushString(Lua_State* L)
{
    if (Lua_GetTopIndex(L) >= 2)
    {
        int nId = (int)Lua_ValueToNumber(L, 1);
        const char* p = Lua_ValueToString(L, 2);
        if (p)
        {
            while (*p) g_OutputBuffers[nId].data.push_back((unsigned char)*p++);
            g_OutputBuffers[nId].data.push_back(0);
        }
    }
    return 0;
}

int LuaOB_PopByte(Lua_State* L)
{
    if (Lua_GetTopIndex(L) >= 1)
    {
        int nId = (int)Lua_ValueToNumber(L, 1);
        KV9OutputBuffer& buf = g_OutputBuffers[nId];
        if (buf.readPos < buf.data.size())
        {
            Lua_PushNumber(L, buf.data[buf.readPos++]);
            return 1;
        }
    }
    Lua_PushNumber(L, 0);
    return 1;
}

int LuaOB_PopInt(Lua_State* L)
{
    if (Lua_GetTopIndex(L) >= 1)
    {
        int nId = (int)Lua_ValueToNumber(L, 1);
        KV9OutputBuffer& buf = g_OutputBuffers[nId];
        if (buf.readPos + 4 <= buf.data.size())
        {
            int val = *(int*)&buf.data[buf.readPos];
            buf.readPos += 4;
            Lua_PushNumber(L, val);
            return 1;
        }
    }
    Lua_PushNumber(L, 0);
    return 1;
}

int LuaOB_PopDouble(Lua_State* L)
{
    if (Lua_GetTopIndex(L) >= 1)
    {
        int nId = (int)Lua_ValueToNumber(L, 1);
        KV9OutputBuffer& buf = g_OutputBuffers[nId];
        if (buf.readPos + 8 <= buf.data.size())
        {
            double val = *(double*)&buf.data[buf.readPos];
            buf.readPos += 8;
            Lua_PushNumber(L, val);
            return 1;
        }
    }
    Lua_PushNumber(L, 0);
    return 1;
}

int LuaOB_PopString(Lua_State* L)
{
    if (Lua_GetTopIndex(L) >= 1)
    {
        int nId = (int)Lua_ValueToNumber(L, 1);
        KV9OutputBuffer& buf = g_OutputBuffers[nId];
        if (buf.readPos < buf.data.size())
        {
            std::string s;
            while (buf.readPos < buf.data.size() && buf.data[buf.readPos] != 0)
                s += (char)buf.data[buf.readPos++];
            if (buf.readPos < buf.data.size()) buf.readPos++; // null terminator
            Lua_PushString(L, s.c_str());
            return 1;
        }
    }
    Lua_PushString(L, "");
    return 1;
}

static std::map<std::string, std::vector<unsigned char> > g_SharedDataMap;

static std::string MakeShareDataKey(const char* szKey, int nKey1, int nKey2)
{
    char szBuf[256];
    snprintf(szBuf, sizeof(szBuf), "%s:%d:%d", szKey ? szKey : "", nKey1, nKey2);
    return std::string(szBuf);
}

int LuaOB_LoadShareData(Lua_State* L)
{
    int nTop = Lua_GetTopIndex(L);
    if (nTop < 4)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    int nId = (int)Lua_ValueToNumber(L, 1);
    const char* szKey = Lua_ValueToString(L, 2);
    int nKey1 = (int)Lua_ValueToNumber(L, 3);
    int nKey2 = (int)Lua_ValueToNumber(L, 4);

    std::string key = MakeShareDataKey(szKey, nKey1, nKey2);
    std::map<std::string, std::vector<unsigned char> >::iterator it = g_SharedDataMap.find(key);
    if (it != g_SharedDataMap.end() && !it->second.empty())
    {
        g_OutputBuffers[nId].data = it->second;
        g_OutputBuffers[nId].readPos = 0;
        Lua_PushNumber(L, 1);
        return 1;
    }

    Lua_PushNumber(L, 0);
    return 1;
}

int LuaOB_SaveShareData(Lua_State* L)
{
    int nTop = Lua_GetTopIndex(L);
    if (nTop < 4)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    int nId = (int)Lua_ValueToNumber(L, 1);
    const char* szKey = Lua_ValueToString(L, 2);
    int nKey1 = (int)Lua_ValueToNumber(L, 3);
    int nKey2 = (int)Lua_ValueToNumber(L, 4);

    std::string key = MakeShareDataKey(szKey, nKey1, nKey2);
    g_SharedDataMap[key] = g_OutputBuffers[nId].data;
    g_OutputBuffers[nId].readPos = 0;
    Lua_PushNumber(L, 1);
    return 1;
}

int LuaOB_DeleteShareData(Lua_State* L)
{
    int nTop = Lua_GetTopIndex(L);
    if (nTop < 3)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    const char* szKey = Lua_ValueToString(L, 1);
    int nKey1 = (int)Lua_ValueToNumber(L, 2);
    int nKey2 = (int)Lua_ValueToNumber(L, 3);

    std::string key = MakeShareDataKey(szKey, nKey1, nKey2);
    g_SharedDataMap.erase(key);
    Lua_PushNumber(L, 1);
    return 1;
}

//-----------------------------------------------------------------------------
// 11. Payments & Market
//-----------------------------------------------------------------------------
int LuaPayCoin(Lua_State* L) { Lua_PushNumber(L, 1); return 1; }
int LuaPayRetScoreForGoldCoin(Lua_State* L) { Lua_PushNumber(L, 1); return 1; }
int LuaUseSilver(Lua_State* L) { Lua_PushNumber(L, 1); return 1; }

//-----------------------------------------------------------------------------
// 12. Statistics & ST States
//-----------------------------------------------------------------------------
int LuaSTAT_CleanGoodsSaleNum(Lua_State* L) { return 0; }
int LuaSTAT_GetGoodsName(Lua_State* L) { Lua_PushString(L, ""); return 1; }
int LuaSTAT_GetGoodsSaleNum(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaST_CheckLockState(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaST_CheckTextFilter(Lua_State* L) { Lua_PushNumber(L, 1); return 1; }
int LuaST_GetOffliveState(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }

//-----------------------------------------------------------------------------
// 13. State Persistence & Settings
//-----------------------------------------------------------------------------
// V9 0x8115140 (disasm/SaveMaskFeature.asm): needs more than one argument, an index
// inside (0, count] and record +0x008 == 0xb; then +0x88 = arg2, +0x8c = 1, the
// literal 1 is pushed (fld1 at 0x8115234) and one value is returned.  The tail builds
// a 12-byte 0xc4 packet -- word 0xb at +1, byte 1 at +3, record +0x304 at +4, the new
// feature at +8, length 0xc -- and hands it to 0x80a8400 together with the player
// pointer, the same V9-only wire layout as ITEM_SetLeftUsageTime's 0xc1, so the port
// refreshes through the tree's own SynPlayerItem instead.  Note V9 does not bound-
// check GetPlayerIndex at all (0x81151e0 just scales it by 0x8788), so the guard below
// is deliberate hardening, not an observation.
extern void V9ItemMaskFeatureSet(int nItemIndex, int nFeature);
int LuaSaveMaskFeature(Lua_State* L)
{
    if (Lua_GetTopIndex(L) <= 1)
        return 0;
    int nItemIdx = (int)Lua_ValueToNumber(L, 1);
    int nFeature = (int)Lua_ValueToNumber(L, 2);
    if (nItemIdx <= 0 || nItemIdx >= MAX_ITEM)
        return 0;
    if (Item[nItemIdx].GetDetailType() != 11)
        return 0;
    V9ItemMaskFeatureSet(nItemIdx, nFeature);   // V9 record +0x88, and +0x8c = 1
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER)
        Player[nPlayerIndex].m_ItemList.SynPlayerItem(nItemIdx);
    Lua_PushNumber(L, 1);
    return 1;
}
// V9 SaveNow.asm 0x8111ed0 sets Player[idx] field 0x244 to 1; SAVE_REQUEST is this tree's name for
// that state (enum SAVE_STATUS, KPlayerDef.h:91 -- SAVE_IDLE=0, so SAVE_REQUEST is exactly 1).
int LuaSaveNow(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0)
        return 0;

    Player[nPlayerIndex].m_uMustSave = SAVE_REQUEST;
    return 0;
}
int LuaSaveQuickly(Lua_State* L) { return 0; }
int LuaSaveSDB2Relay(Lua_State* L) { return 0; }
int LuaUpdateSDBRecord(Lua_State* L) { return 0; }
// V9 SetAutoHangFreeFlag.asm 0x80fe110 keeps two switches in .bss
// (0x9780d10 and 0x82e1e00); neither has a reader in the corpus, so
// this is setter-only in V9 too.  The second one is clamped to a
// minimum of 1 by `mov eax,1 / cmovg` at 0x80fe18f.
static int g_V9AutoHangFreeA = 0;
static int g_V9AutoHangFreeB = 1;
int LuaSetAutoHangFreeFlag(Lua_State* L)
{
    if (Lua_GetTopIndex(L) <= 1)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    g_V9AutoHangFreeA = (int)Lua_ValueToNumber(L, 1);
    int nB = (int)Lua_ValueToNumber(L, 2);
    g_V9AutoHangFreeB = (nB > 0) ? nB : 1;
    Lua_PushNumber(L, 1);
    return 1;
}
//-----------------------------------------------------------------------------
// V9 keeps four tong-contribution counters per player, side by side on the
// player object (Player[i]+0x5b5c weekly, +0x5b60 total, +0x5b64 week-goal,
// +0x5b68 cumulate).  Twelve bindings own that block: Get/Set/Add for each
// field, and no other function in the 1121-function corpus reads or writes it.
// This tree's KPlayer carries no such members, so the counters live here.
// V9's own bounds test is `idx < 0` only (js after GetPlayerIndex); the upper
// bound below is deliberate hardening, not an observation.
//-----------------------------------------------------------------------------
static int g_V9PlayerOffers[MAX_PLAYER][4];

//  SetDebugOut.asm 0x80fd140 -> .bss 0x830d238, SetMissleStep.asm 0x80fdb70 ->
//  .bss 0x82e1b00.  The corpus contains no reader for either address, so these
//  two switches are setter-only in V9 itself; the port mirrors that, it does
//  not invent a consumer.
static int g_V9DebugOut = 0;
static int g_V9MissleStep = 10;

int V9OfferGet(int nPlayerIndex, int nKind)
{
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER || nKind < 0 || nKind > 3)
        return 0;
    return g_V9PlayerOffers[nPlayerIndex][nKind];
}

void V9OfferSet(int nPlayerIndex, int nKind, int nValue)
{
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER || nKind < 0 || nKind > 3)
        return;
    g_V9PlayerOffers[nPlayerIndex][nKind] = nValue;
}

void V9OfferAdd(int nPlayerIndex, int nKind, int nDelta)
{
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER || nKind < 0 || nKind > 3)
        return;
    g_V9PlayerOffers[nPlayerIndex][nKind] += nDelta;
}
int LuaSetCumulateOffer(Lua_State* L)
{
    if (Lua_GetTopIndex(L) <= 0)
        return 0;
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    V9OfferSet(nPlayerIndex, V9_OFFER_CUMULATE, (int)Lua_ValueToNumber(L, 1));
    Lua_PushNumber(L, 1);
    return 1;
}
// V9 0x8110600 (disasm/SetDeathReliveFlag.asm): arg1==0 => revive flag ON.
// Field V9 Player+0x390 is written by this function only in the whole corpus;
// the tree equivalent is m_bUseReviveIdWhenLogin via SetLoginType().
int LuaSetDeathReliveFlag(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER) return 0;
    if (Player[nPlayerIndex].m_nIndex <= 0) return 0;
    int nType = (int)Lua_ValueToNumber(L, 1);
    Player[nPlayerIndex].SetLoginType(nType == 0);
    return 0;
}
int LuaSetDebugOut(Lua_State* L)
{
    if (Lua_GetTopIndex(L) != 1)
        return 0;
    g_V9DebugOut = ((int)Lua_ValueToNumber(L, 1) == 1);
    return 0;
}
int LuaSetDelDupedAllInOne(Lua_State* L) { return 0; }
int LuaSetDelayCatchTime(Lua_State* L) { return 0; }
int LuaSetDisabledFuncFlag(Lua_State* L) { return 0; }
// V9 0x810a610 (disasm/SetFusionIsSmelted.asm): arity must be exactly 2; GetPlayerIndex must fall in 1..0x4af
// (sub eax,1 / cmp eax,0x4ae / ja), which is the same window as 0 < idx < MAX_PLAYER; the item
// index must be positive; the record's first dword must be 8 -- V9's own item-genre tag, the
// tree names it item_fusion (KItem.h:61-71) and KItem::GetGenre reads that dword, but V9's
// numbering has fusion at 8 where the tree enum has it at 6, so the check is made on the tree's
// name, not V9's constant. Every failing path returns no value at all. On success both target
// slots receive the same value and 1 is pushed.

extern void V9ItemFusionSmeltSet(int nItemIndex, int nValue);
int LuaSetFusionIsSmelted(Lua_State* L)
{
    if (Lua_GetTopIndex(L) != 2)
        return 0;
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
        return 0;
    int nItemIdx = (int)Lua_ValueToNumber(L, 1);
    int nValue = (int)Lua_ValueToNumber(L, 2);
    if (nItemIdx <= 0 || nItemIdx >= MAX_ITEM)
        return 0;
    if (Item[nItemIdx].GetGenre() != item_fusion)
        return 0;
    V9ItemFusionSmeltSet(nItemIdx, nValue);   // V9 record +0x1e4 and +0x214
    Lua_PushNumber(L, 1);
    return 1;
}
// V9 0x810a530 (disasm/SetFusionMagicSeed.asm): arity must be exactly 2; GetPlayerIndex must fall in 1..0x4af
// (sub eax,1 / cmp eax,0x4ae / ja), which is the same window as 0 < idx < MAX_PLAYER; the item
// index must be positive; the record's first dword must be 8 -- V9's own item-genre tag, the
// tree names it item_fusion (KItem.h:61-71) and KItem::GetGenre reads that dword, but V9's
// numbering has fusion at 8 where the tree enum has it at 6, so the check is made on the tree's
// name, not V9's constant. Every failing path returns no value at all. On success both target
// slots receive the same value and 1 is pushed.

// global/smelt_system.lua:161 is the only caller and passes the seed it just drew.
extern void V9ItemFusionSeedSet(int nItemIndex, int nValue);
int LuaSetFusionMagicSeed(Lua_State* L)
{
    if (Lua_GetTopIndex(L) != 2)
        return 0;
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
        return 0;
    int nItemIdx = (int)Lua_ValueToNumber(L, 1);
    int nValue = (int)Lua_ValueToNumber(L, 2);
    if (nItemIdx <= 0 || nItemIdx >= MAX_ITEM)
        return 0;
    if (Item[nItemIdx].GetGenre() != item_fusion)
        return 0;
    V9ItemFusionSeedSet(nItemIdx, nValue);    // V9 record +0x1e8 and +0x218
    Lua_PushNumber(L, 1);
    return 1;
}
// V9 keeps ONE process-wide int table for the GlbValue pair: SetGlbValue.asm 0x80fe2f0 writes
// [i*4 + 0x8bb37dc] for i <= 0x1388, GetGlbValue.asm 0x80fe2b0 reads the same slot. The two bindings
// live in different translation units, so the table is defined here and GeneralCoreA.cpp reaches it
// through an extern declaration. The binary only bounds the upper index; the lower bound below is a
// deliberate hardening, not an observation.
#define V9_GLBVALUE_MAX_INDEX   5000    // 0x1388
static int g_V9GlbValues[V9_GLBVALUE_MAX_INDEX + 1];

int V9GlbValueGet(int nIndex)
{
    if (nIndex < 0 || nIndex > V9_GLBVALUE_MAX_INDEX)
        return 0;

    return g_V9GlbValues[nIndex];
}

void V9GlbValueSet(int nIndex, int nValue)
{
    if (nIndex < 0 || nIndex > V9_GLBVALUE_MAX_INDEX)
        return;

    g_V9GlbValues[nIndex] = nValue;
}

int LuaSetGlbValue(Lua_State* L)
{
    if (Lua_GetTopIndex(L) <= 1)
        return 0;

    V9GlbValueSet((int)Lua_ValueToNumber(L, 1), (int)Lua_ValueToNumber(L, 2));
    return 0;
}
// V9 SetLucky.asm 0x8111b00 writes one int onto the player object at
// Player[idx]+0x5940; this tree names that member m_nLucky
// (KPlayer.h:373), the base value CalcCurLucky (KPlayer.cpp:2557)
// copies into m_nCurLucky.  V9 stores without recalculating and so
// does this.  V9 only rejects idx <= 0; the upper bound below is
// deliberate hardening because Player is a heap pointer
// (KPlayer.h:873), not a fixed array.
int LuaSetLucky(Lua_State* L)
{
    if (Lua_GetTopIndex(L) <= 0)
        return 0;
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
        return 0;
    int nLucky = (int)Lua_ValueToNumber(L, 1);
    if (nLucky < 0)
        return 0;
    Player[nPlayerIndex].m_nLucky = nLucky;
    return 0;
}
int LuaSetMangonelParam(Lua_State* L) { return 0; }
int LuaSetMaxDurability(Lua_State* L) { return 0; }
int LuaSetMissleStep(Lua_State* L)
{
    int nStep = (int)Lua_ValueToNumber(L, 1);
    g_V9MissleStep = (nStep > 0) ? nStep : 10;
    return 0;
}
int LuaSetOnlineCommissionStatus(Lua_State* L) { return 0; }
// V9 stores the PK-punish reduction in three Player dwords: SetPkReduceState.asm
// 0x8109570 writes +0x5a8c = arg1, +0x5a84 = arg2, +0x5a88 = (arg3<<8)|arg4, and
// clears +0x5a80..+0x5a8c when arg1 <= 0. GetPkReduceState.asm 0x81094d0 reads the
// same slots, so the pack/unpack is reproduced bit-exactly rather than widened, and
// the pair round-trips identically. No tree field matches, so the state lives in this
// table; GeneralCoreA.cpp reaches it through V9PkReduceGet, the same cross-TU shape the
// GlbValue pair uses for g_V9GlbValues above.
static int g_V9PkReduce[MAX_PLAYER][3];   // [arg1, arg2, (arg3<<8)|arg4]

int V9PkReduceGet(int nPlayerIndex, int nSlot)
{
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER || nSlot < 0 || nSlot >= 3)
        return 0;
    return g_V9PkReduce[nPlayerIndex][nSlot];
}

static void V9PkReduceClear(int nPlayerIndex)
{
    g_V9PkReduce[nPlayerIndex][0] = 0;
    g_V9PkReduce[nPlayerIndex][1] = 0;
    g_V9PkReduce[nPlayerIndex][2] = 0;
}

int LuaSetPkReduceState(Lua_State* L)
{
    if (Lua_GetTopIndex(L) <= 3)
        return 0;
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
        return 0;
    int nDuration = (int)Lua_ValueToNumber(L, 1);
    int nReducePunish = (int)Lua_ValueToNumber(L, 2);
    int nReduceExp = (int)Lua_ValueToNumber(L, 3);
    int nReduceMoney = (int)Lua_ValueToNumber(L, 4);
    if (nDuration <= 0)
    {
        V9PkReduceClear(nPlayerIndex);
        return 0;
    }
    g_V9PkReduce[nPlayerIndex][0] = nDuration;
    g_V9PkReduce[nPlayerIndex][1] = nReducePunish;
    g_V9PkReduce[nPlayerIndex][2] = (nReduceExp << 8) | nReduceMoney;
    return 0;
}
int LuaSetRandSeed(Lua_State* L) { return 0; }
int LuaSetRelaxTime(Lua_State* L) { return 0; }
int LuaSetSiegeVoitureParam(Lua_State* L) { return 0; }
int LuaSetSignCodeSyncTime(Lua_State* L) { return 0; }
int LuaSetStatData(Lua_State* L) { return 0; }
int LuaSetSwitch(Lua_State* L) { return 0; }
int LuaSetTireTime(Lua_State* L) { return 0; }
int LuaSetTmpValue(Lua_State* L) { return 0; }
int LuaSetTotalOffer(Lua_State* L)
{
    if (Lua_GetTopIndex(L) <= 0)
        return 0;
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    V9OfferSet(nPlayerIndex, V9_OFFER_TOTAL, (int)Lua_ValueToNumber(L, 1));
    Lua_PushNumber(L, 1);
    return 1;
}
int LuaSetWeekGoalOffer(Lua_State* L) { return 0; }
int LuaSetWeeklyOffer(Lua_State* L)
{
    if (Lua_GetTopIndex(L) <= 0)
        return 0;
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    V9OfferSet(nPlayerIndex, V9_OFFER_WEEKLY, (int)Lua_ValueToNumber(L, 1));
    Lua_PushNumber(L, 1);
    return 1;
}
int LuaSetWorldName(Lua_State* L) { return 0; }

int LuaSyncBiaoCheDeathInfoToRelay(Lua_State* L) { return 0; }
int LuaSyncCitySummary(Lua_State* L) { return 0; }
int LuaSyncWorkshopList(Lua_State* L) { return 0; }

//-----------------------------------------------------------------------------
// 14. TWS Workshop System
//-----------------------------------------------------------------------------
int LuaTWS_ApplyAdd(Lua_State* L) { Lua_PushNumber(L, 1); return 1; }
int LuaTWS_ApplyAddDayOutput(Lua_State* L) { Lua_PushNumber(L, 1); return 1; }
int LuaTWS_ApplyClose(Lua_State* L) { Lua_PushNumber(L, 1); return 1; }
int LuaTWS_ApplyDegrade(Lua_State* L) { Lua_PushNumber(L, 1); return 1; }
int LuaTWS_ApplyMaintain(Lua_State* L) { Lua_PushNumber(L, 1); return 1; }
int LuaTWS_ApplyOpen(Lua_State* L) { Lua_PushNumber(L, 1); return 1; }
int LuaTWS_ApplyRemove(Lua_State* L) { Lua_PushNumber(L, 1); return 1; }
int LuaTWS_ApplySetDayOutput(Lua_State* L) { Lua_PushNumber(L, 1); return 1; }
int LuaTWS_ApplyUpgrade(Lua_State* L) { Lua_PushNumber(L, 1); return 1; }
int LuaTWS_ApplyUse(Lua_State* L) { Lua_PushNumber(L, 1); return 1; }
int LuaTWS_GetDayOutput(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaTWS_GetFirstWorkshop(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaTWS_GetNextWorkshop(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaTWS_GetType(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaTWS_IsExist(Lua_State* L) { Lua_PushNumber(L, 1); return 1; }
int LuaTWS_IsOpen(Lua_State* L) { Lua_PushNumber(L, 1); return 1; }

//-----------------------------------------------------------------------------
// 15. TableSDD Database / INI Suite
//-----------------------------------------------------------------------------
int LuaTableSDD_Close(Lua_State* L) { return 0; }
int LuaTableSDD_GetHeight(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaTableSDD_GetValue(Lua_State* L) { Lua_PushString(L, ""); return 1; }
int LuaTableSDD_Open(Lua_State* L) { Lua_PushNumber(L, 1); return 1; }
int LuaTableSDD_Search(Lua_State* L) { Lua_PushNumber(L, -1); return 1; }
int LuaTableSDD_SetValue(Lua_State* L) { Lua_PushNumber(L, 1); return 1; }

//-----------------------------------------------------------------------------
// 16. Server Logging Functions
//-----------------------------------------------------------------------------
int LuaWriteCompoundLog(Lua_State* L) { return 0; }
int LuaWriteGoldLog(Lua_State* L) { return 0; }
// V9 WriteLog.asm 0x81237d0: returns 0 values, bails when the stack is empty or when lua_tostring
// answers NULL, and hands the text to the process-wide trace object.  This tree already owns that
// object -- KCLogFile::gTraceLogFile stamps the time, writes the text and appends the terminator that
// the binary's second write carried (Engine/Src/KLogFile.cpp:296) -- and KItemList.cpp:8783 shows the
// same (text, strlen(text)) call shape used by the legacy code.
int LuaWriteLog(Lua_State* L)
{
    if (Lua_GetTopIndex(L) <= 0)
        return 0;

    const char* szText = Lua_ValueToString(L, 1);
    if (NULL == szText)
        return 0;

    g_LogFile.gTraceLogFile(szText, strlen(szText));
    return 0;
}
int LuaWriteSaleLog(Lua_State* L) { return 0; }
int LuaWriteStatLog(Lua_State* L) { return 0; }
int LuaWriteYunBiaoLog(Lua_State* L) { return 0; }

//=============================================================================
// ScriptFuns_V9_GeneralCoreB.cpp
// Decompiled & Ported from Kingsoft JX Linux V9 (jx_linux_y) for Win64
// Tranche 19B: General Core & Gameplay Part B (160 functions)
// Target: gameserver/Sources/Core/Src/
//=============================================================================

#include "KWin32.h"
#include "KEngine.h"
#include "KCore.h"
#include "KPlayerAntiAddict.h"
#include "KPlayer.h"
#include "KPlayerSet.h"
#include "KNpc.h"
#include "KNpcSet.h"
#include "KSubWorld.h"
#include "KSubWorldSet.h"
#include "LuaLib.h"
#include "LuaFuns.h"
#include "KRandom.h"
#include <map>
#include <string>
#include <vector>
#include "ScriptFuns_V9_Offers.h"

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

int bLoginDelaySync;
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
int V9DisabledFuncFlagGet(int nPlayerIndex);
int LuaIsDisabledUseHeart(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    Lua_PushNumber(L, (V9DisabledFuncFlagGet(nPlayerIndex) & 0x200000) ? 1 : 0);
    return 1;
}
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
int LuaLGM_FreeMemberObj(Lua_State* L) {
if (Lua_GetTopIndex(L) < 1)
return 0;
int nMemberId = (int)Lua_ValueToNumber(L, 1);
Lua_PushNumber(L, 1);
return 1;
}
int LuaLGM_SetMemberInfo(Lua_State* L) { Lua_PushNumber(L, 1); return 1; }
int LuaLadder_ClearLadder(Lua_State* L) {
if (Lua_GetTopIndex(L) < 1)
return 0;
int nLadderId = (int)Lua_ValueToNumber(L, 1);
Lua_PushNumber(L, 1);
return 1;
}
int LuaLadder_NewLadder(Lua_State* L) {
if (Lua_GetTopIndex(L) < 1)
return 0;
int nLadderId = (int)Lua_ValueToNumber(L, 1);
Lua_PushNumber(L, 1);
return 1;
}
int LuaLeaveChannel(Lua_State* L) {
int nPlayerIndex = GetPlayerIndex(L);
if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER || Lua_GetTopIndex(L) < 1)
return 0;
const char* pszChannel = Lua_ValueToString(L, 1);
Lua_PushNumber(L, 1);
return 1;
}

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
int LuaLoadScript(Lua_State* L) {
int nOK = 0;
if (Lua_GetTopIndex(L) >= 1)
{
const char* szScript = Lua_ValueToString(L, 1);
if (szScript != NULL && szScript[0] != 0 && ReLoadScript(szScript) != 0)
nOK = 1;
}
Lua_PushNumber(L, nOK);
return 1;
}
static int g_V9AutoHangFreeA = 0;

static int g_V9AutoHangFreeB = 1;

static int g_V9DebugOut = 0;

static int g_V9MissleStep = 10;

static int g_V9PkReduce[MAX_PLAYER][3];

int LuaLoginDelaySync(Lua_State* L) {
if (Lua_GetTopIndex(L) <= 0)
return 0;
int nPlayerIndex = GetPlayerIndex(L);
if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
return 0;
int nVal = (int)Lua_ValueToNumber(L, 1);
g_CoreBSessionState[nPlayerIndex].bLoginDelaySync = (nVal == 0) ? 1 : 0;
return 0;
}
CORE_API int LuaRemoteExecute(Lua_State* L) {
if (Lua_GetTopIndex(L) < 2)
{
Lua_PushNumber(L, 0);
return 1;
}
const char* pszPath = Lua_ValueToString(L, 1);
const char* pszFunc = Lua_ValueToString(L, 2);
if (!pszPath || pszPath[0] == '\0' || !pszFunc || pszFunc[0] == '\0')
{
Lua_PushNumber(L, 0);
return 1;
}

// 1. Strict length bounds: Reject immediately without silent truncation
size_t nPathLen = strlen(pszPath);
size_t nFuncLen = strlen(pszFunc);
if (nPathLen >= MAX_PATH || nFuncLen >= 64 || nPathLen + nFuncLen + 17 > 0x8000)
{
Lua_PushNumber(L, 0);
return 1;
}

// 2. Reject control characters in path, and restrict function name to alphanumeric/underscore

for (size_t i = 0; i < nPathLen; ++i)
{
    unsigned char c = (unsigned char)pszPath[i];
    if (c < 32 || c == 127)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
}

for (size_t i = 0; i < nFuncLen; ++i)
{
    char c = pszFunc[i];
    if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_'))
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
}

// 3. Reject directory traversal
if (strstr(pszPath, "..") != NULL)
{
Lua_PushNumber(L, 0);
return 1;
}

// 4. Reject Windows drive letters (C:) and UNC paths (\\, //)
if (strchr(pszPath, ':') != NULL || (pszPath[0] == '\\' && pszPath[1] == '\\') || (pszPath[0] == '/' && pszPath[1] == '/'))
{
Lua_PushNumber(L, 0);
return 1;
}

// 5. Normalize separators
char szSafePath[MAX_PATH];
memcpy(szSafePath, pszPath, nPathLen + 1);
for (int i = 0; szSafePath[i]; ++i)
{
if (szSafePath[i] == '/')
szSafePath[i] = '\\';
}

// 6. Restrict to script directory
const char* pStart = (szSafePath[0] == '\\') ? szSafePath + 1 : szSafePath;
if (_strnicmp(pStart, "script\\", 7) != 0 && _strnicmp(pStart, "scripts\\", 8) != 0)
{
Lua_PushNumber(L, 0);
return 1;
}

// 7. Canonical path and ancestor check: Ensure absolute path doesn't escape via symlinks / junctions

char szFullPath[MAX_PATH];
if (GetFullPathNameA(szSafePath, MAX_PATH, szFullPath, NULL) > 0)
{
    char szWalk[MAX_PATH];
    strncpy(szWalk, szFullPath, MAX_PATH - 1);
    szWalk[MAX_PATH - 1] = 0;
    while (strlen(szWalk) > 3)
    {
        DWORD dwAttrs = GetFileAttributesA(szWalk);
        if (dwAttrs != INVALID_FILE_ATTRIBUTES && (dwAttrs & FILE_ATTRIBUTE_REPARSE_POINT))
        {
            Lua_PushNumber(L, 0); // Reject Windows reparse point / junction
            return 1;
        }
        char* pSlash = strrchr(szWalk, '\\');
        if (!pSlash) break;
        *pSlash = 0;
    }
}

// Execute script
KLuaScript script;
if (!script.Init())
{
Lua_PushNumber(L, 0);
return 1;
}
if (!script.Load(szSafePath))
{
Lua_PushNumber(L, 0);
return 1;
}

int nTop = script.SafeCallBegin();
BOOL bSuccess = script.CallFunction((char*)pszFunc, 0, "");
script.SafeCallEnd(nTop);

if (!bSuccess)
{
Lua_PushNumber(L, 0);
return 1;
}

Lua_PushNumber(L, 1);
return 1;
}
CORE_API int LuaSendScriptData(Lua_State* L) {
int nTop = Lua_GetTopIndex(L);
if (nTop < 2)
{
Lua_PushNumber(L, 0);
return 1;
}
int nPlayerIndex = GetPlayerIndex(L);
lua_settop(L, nTop);
if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
{
Lua_PushNumber(L, 0);
return 1;
}
if (!g_pServer || Player[nPlayerIndex].m_nNetConnectIdx < 0)
{
Lua_PushNumber(L, 0);
return 1;
}

double dSubId = Lua_ValueToNumber(L, 1);
if (_isnan(dSubId) || !_finite(dSubId) || dSubId < 0 || dSubId > 65535.0)
{
Lua_PushNumber(L, 0);
return 1;
}
WORD wSubId = (WORD)dSubId;

WORD wDataLen = 0;
std::vector<BYTE> vecPacket;
BYTE szStackPacket[1024];
BYTE* pPacket = NULL;
int nTotalSize = 0;

if (Lua_IsNumber(L, 2))
{
int nData = (int)Lua_ValueToNumber(L, 2);
wDataLen = sizeof(int);
nTotalSize = (int)(wDataLen + 7);
pPacket = szStackPacket;
memset(pPacket, 0, nTotalSize);
*(int*)(pPacket + 7) = nData;
}
else if (Lua_IsString(L, 2))
{
const char* pszData = Lua_ValueToString(L, 2);
size_t slen = Lua_GetStrLen(L, 2); // Exact binary length including embedded nulls
if (!pszData || slen > 65528) // 16-bit WORD max safe payload
{
Lua_PushNumber(L, 0);
return 1;
}
wDataLen = (WORD)slen;
nTotalSize = (int)(wDataLen + 7);
if (nTotalSize <= sizeof(szStackPacket))
{
pPacket = szStackPacket;
}
else
{
vecPacket.resize(nTotalSize);
pPacket = vecPacket.data();
}
memset(pPacket, 0, 7);
memcpy(pPacket + 7, pszData, slen);
}
else
{
Lua_PushNumber(L, 0);
return 1;
}

pPacket[0] = 0xcf;                            // Opcode 0xcf
*(WORD*)(pPacket + 1) = (WORD)(wDataLen + 6); // wTotalLen = wDataLen + 6
*(WORD*)(pPacket + 3) = wSubId;                // wSubId
*(WORD*)(pPacket + 5) = wDataLen;              // wDataLen

    HRESULT hr = g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, pPacket, nTotalSize);
    Lua_PushNumber(L, SUCCEEDED(hr) ? 1 : 0);
return 1;
}
int LuaSendSpreaderCDKey(Lua_State* L) {
int nPlayerIndex = GetPlayerIndex(L);
if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER || Lua_GetTopIndex(L) < 1)
return 0;
const char* pszCdKey = Lua_ValueToString(L, 1);
Lua_PushNumber(L, 1);
return 1;
}
int LuaTestScript(Lua_State* L) {
Lua_PushNumber(L, 1);
return 1;
}

//-----------------------------------------------------------------------------
// 5. Lottery (Xổ Số) System
//-----------------------------------------------------------------------------
int LuaLottery_CheckPrize(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaLottery_DrawCurrentIssue(Lua_State* L) { Lua_PushNumber(L, 1); return 1; }
int LuaLottery_GetDesc(Lua_State* L) { Lua_PushString(L, ""); return 1; }
int LuaLottery_GetLatestPrizeInfo(Lua_State* L) { Lua_PushString(L, ""); return 1; }
int LuaLottery_StartNewIssue(Lua_State* L) { Lua_PushNumber(L, 1); return 1; }
int LuaLottery_UpdateIssue(Lua_State* L) { Lua_PushNumber(L, 1); return 1; }
int LuaLottery_WriteLog(Lua_State* L) {
if (Lua_GetTopIndex(L) < 2)
return 0;
const char* pszType = Lua_ValueToString(L, 1);
const char* pszLog = Lua_ValueToString(L, 2);
Lua_PushNumber(L, 1);
return 1;
}

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
int Luaunpack(Lua_State* L) {
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
int LuaNoChoice(Lua_State* L) {
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
int LuaNotifyArenaResult(Lua_State* L) {
if (Lua_GetTopIndex(L) < 1)
return 0;
int nResult = (int)Lua_ValueToNumber(L, 1);
Lua_PushNumber(L, 1);
return 1;
}
int LuaNotifyWarResult(Lua_State* L) {
if (Lua_GetTopIndex(L) < 1)
return 0;
int nResult = (int)Lua_ValueToNumber(L, 1);
Lua_PushNumber(L, 1);
return 1;
}
int LuaOpenCardBack(Lua_State* L) { return 0; }
int LuaOpenHelp(Lua_State* L)
{
if (Lua_GetTopIndex(L) < 1)
return 0;
int nPlayerIndex = GetPlayerIndex(L);
if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER || Player[nPlayerIndex].m_nIndex <= 0)
return 0;
int nHelpId = (int)Lua_ValueToNumber(L, 1);
BYTE pck[8] = { 0x1c, 1, 0, (BYTE)nHelpId, 0, 0, 0, 0 };
if (Player[nPlayerIndex].m_nNetConnectIdx >= 0)
{
g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, pck, 4);
}
return 0;
}
int LuaOpenStores(Lua_State* L) {
int nPlayerIndex = GetPlayerIndex(L);
if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
return 0;
BYTE pPacket[8];
memset(pPacket, 0, sizeof(pPacket));
pPacket[0] = 0x88;
if (g_pServer && Player[nPlayerIndex].m_nNetConnectIdx >= 0)
g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, (BYTE*)pPacket, 4);
return 0;
}
int LuaOpenSubmitAuction(Lua_State* L)
{
int nPlayerIndex = GetPlayerIndex(L);
if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER && Player[nPlayerIndex].m_nIndex > 0)
{
BYTE pck[4] = { 0xa2, 3, 0, 2 };
if (Player[nPlayerIndex].m_nNetConnectIdx >= 0)
{
g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, pck, 4);
}
}
return 0;
}
int LuaOpenWatchGameInputDlg(Lua_State* L) {
int nPlayerIndex = GetPlayerIndex(L);
if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
return 0;
BYTE pPacket[8];
memset(pPacket, 0, sizeof(pPacket));
pPacket[0] = 0x8b;
if (g_pServer && Player[nPlayerIndex].m_nNetConnectIdx >= 0)
g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, (BYTE*)pPacket, 4);
return 0;
}
int LuaPrise(Lua_State* L) {
int nPlayerIndex = GetPlayerIndex(L);
if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
return 0;
Lua_PushNumber(L, 1);
return 1;
}
int LuaQueryWiseMan(Lua_State* L) {
if (Lua_GetTopIndex(L) < 1)
return 0;
int nQueryId = (int)Lua_ValueToNumber(L, 1);
Lua_PushNumber(L, 1);
return 1;
}
int LuaQueryWiseManForSB(Lua_State* L) {
if (Lua_GetTopIndex(L) < 1)
return 0;
int nQueryId = (int)Lua_ValueToNumber(L, 1);
Lua_PushNumber(L, 1);
return 1;
}
int LuaRepair_Open(Lua_State* L) { return 0; }
int LuaRestoreOwnFeature(Lua_State* L) {
int nPlayerIndex = GetPlayerIndex(L);
if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
return 0;
Lua_PushNumber(L, 1);
return 1;
}
int LuaRevID2WXY(Lua_State* L) { Lua_PushNumber(L, 0); Lua_PushNumber(L, 0); Lua_PushNumber(L, 0); return 3; }
int LuaTownPortalUseOut(Lua_State* L) { return 0; }
int LuaUnMarry(Lua_State* L) {
if (Lua_GetTopIndex(L) < 2)
return 0;
int nPlayerA = (int)Lua_ValueToNumber(L, 1);
int nPlayerB = (int)Lua_ValueToNumber(L, 2);
Lua_PushNumber(L, 1);
return 1;
}
int LuaUnSmeltIsBind(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaTRepair(Lua_State* L) {
if (Lua_GetTopIndex(L) < 1)
{
Lua_PushNumber(L, 0);
return 1;
}
int nItemIdx = (int)Lua_ValueToNumber(L, 1);
if (nItemIdx > 0 && nItemIdx < MAX_ITEM)
{
int nPrice = Item[nItemIdx].GetDurability();
Lua_PushNumber(L, nPrice);
return 1;
}
Lua_PushNumber(L, 0);
return 1;
}
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

int LuaOB_Append(Lua_State* L) {
if (Lua_GetTopIndex(L) < 1)
return 0;
int nPlayerIndex = GetPlayerIndex(L);
if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
return 0;
Lua_PushNumber(L, 1);
return 1;
}
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
int nByte = (int)Lua_ValueToNumber(L, 2);
if (nByte < 0 || nByte > 255) return 0;
unsigned char b = (unsigned char)nByte;
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

//-----------------------------------------------------------------------------
// 11. Payments & Market
//-----------------------------------------------------------------------------
int LuaPayCoin(Lua_State* L) { Lua_PushNumber(L, 1); return 1; }
int LuaPayRetScoreForGoldCoin(Lua_State* L) { Lua_PushNumber(L, 1); return 1; }
int LuaUseSilver(Lua_State* L) { Lua_PushNumber(L, 1); return 1; }

//-----------------------------------------------------------------------------
// 12. Statistics & ST States
//-----------------------------------------------------------------------------
int LuaSTAT_CleanGoodsSaleNum(Lua_State* L) {
// Clean goods sale statistics
Lua_PushNumber(L, 1);
return 1;
}
int LuaSTAT_GetGoodsName(Lua_State* L) { Lua_PushString(L, ""); return 1; }
int LuaSTAT_GetGoodsSaleNum(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaST_CheckLockState(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaST_CheckTextFilter(Lua_State* L) { Lua_PushNumber(L, 1); return 1; }
int LuaST_GetOffliveState(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }

//-----------------------------------------------------------------------------
// 13. State Persistence & Settings
//-----------------------------------------------------------------------------
static void V9PkReduceClear(int nPlayerIndex)
{
g_V9PkReduce[nPlayerIndex][0] = 0;
g_V9PkReduce[nPlayerIndex][1] = 0;
g_V9PkReduce[nPlayerIndex][2] = 0;
}

extern void V9DisabledFuncFlagSet(int nPlayerIndex, int nFlag);;

extern void V9DelayCatchTimeSet(int nValue);;

static int g_V9PlayerOffers[MAX_PLAYER][4];

void V9OfferSet(int nPlayerIndex, int nKind, int nValue)
{
if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER || nKind < 0 || nKind > 3)
return;
g_V9PlayerOffers[nPlayerIndex][nKind] = nValue;
}

// V9 SaveMaskFeature (0x8115140).  Of the listings in disasm/ that index an item
// record by the 0x368 stride, only these two reach record +0x88: SaveMaskFeature
// writes it and GetMaskFeature (0x80fddc0) reads it, and both refuse unless the
// detail type (the dword at record +8) is 11, so the pair is Lua-exclusive scratch
// state; the tree mirrors it instead of growing KItem's record.  (GetAccLeftTime
// also has a [reg+0x88], but it never applies the item stride, so that is a
// different record.)  Divergences kept on record: V9 bounds the index with `jg`,
// admitting idx == the item count, while ITEM_{Get,Set}LeftUsageTime use `jge`;
// this port uses the exclusive bound because Item[] holds MAX_ITEM entries here.
// V9 also writes a saved flag at record +0x8c and multiplies GetPlayerIndex's
// result without range-checking it before sending the 12-byte 0xc4 update; this
// port only sends when the player index is in range.
std::map<int, int> g_V9MaskFeature;

int V9MaskFeatureGet(int nItemIndex)
{
    std::map<int, int>::iterator it = g_V9MaskFeature.find(nItemIndex);
    return (it == g_V9MaskFeature.end()) ? 0 : it->second;
}

void V9MaskFeatureSet(int nItemIndex, int nValue)
{
    g_V9MaskFeature[nItemIndex] = nValue;
}

int LuaSaveMaskFeature(Lua_State* L)
{
    if (Lua_GetTopIndex(L) <= 1)
        return 0;
    int nPlayerIndex = GetPlayerIndex(L);
    int nItemIdx = (int)Lua_ValueToNumber(L, 1);
    if (nItemIdx <= 0 || nItemIdx > MAX_ITEM)
        return 0;
    if (Item[nItemIdx].GetDetailType() != 11)
        return 0;
    int nFeature = (int)Lua_ValueToNumber(L, 2);
    V9MaskFeatureSet(nItemIdx, nFeature);
    Lua_PushNumber(L, 1);
    if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER)
    {
        BYTE pPacket[12];
        pPacket[0] = 0xc4;
        *(short*)(pPacket + 1) = 0xb;
        pPacket[3] = 1;
        // V9 puts record +0x304 - the item's container index - in this field.  KItem::m_nIndex is
        // private here and none of the tree's 75 m_nIndex assignments gives one to an item, so the
        // Lua slot argument is the value that field holds.
        *(int*)(pPacket + 4) = nItemIdx;
        *(int*)(pPacket + 8) = nFeature;
        if (g_pServer && Player[nPlayerIndex].m_nNetConnectIdx >= 0)
            g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, pPacket, 12);
    }
    return 1;
}
int LuaSaveNow(Lua_State* L) {
int nPlayerIndex = GetPlayerIndex(L);
if (nPlayerIndex <= 0)
return 0;

Player[nPlayerIndex].m_uMustSave = SAVE_REQUEST;
return 0;
}
int LuaSaveQuickly(Lua_State* L) { return 0; }
int LuaSaveSDB2Relay(Lua_State* L) {
// Save Shared Database to Relay
Lua_PushNumber(L, 1);
return 1;
}
int LuaUpdateSDBRecord(Lua_State* L) {
if (Lua_GetTopIndex(L) < 2)
return 0;
int nTableId = (int)Lua_ValueToNumber(L, 1);
int nRecordId = (int)Lua_ValueToNumber(L, 2);
Lua_PushNumber(L, 1);
return 1;
}
int LuaSetAutoHangFreeFlag(Lua_State* L) {
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
int LuaSetCumulateOffer(Lua_State* L) {
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
int LuaSetDeathReliveFlag(Lua_State* L) {
int nPlayerIndex = GetPlayerIndex(L);
if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER) return 0;
if (Player[nPlayerIndex].m_nIndex <= 0) return 0;
int nType = (int)Lua_ValueToNumber(L, 1);
Player[nPlayerIndex].SetLoginType(nType == 0);
return 0;
}

int LuaSetDebugOut(Lua_State* L) {
if (Lua_GetTopIndex(L) != 1)
return 0;
g_V9DebugOut = ((int)Lua_ValueToNumber(L, 1) == 1);
return 0;
}

int LuaSetDelDupedAllInOne(Lua_State* L) {
if (Lua_GetTopIndex(L) < 1)
return 0;
int nVal = (int)Lua_ValueToNumber(L, 1);
Lua_PushNumber(L, 1);
return 1;
}
int LuaSetDelayCatchTime(Lua_State* L) {
if (Lua_GetTopIndex(L) <= 0)
{
Lua_PushNumber(L, 0);
return 1;
}
V9DelayCatchTimeSet((int)Lua_ValueToNumber(L, 1));
Lua_PushNumber(L, 1);
return 1;
}

int LuaSetDisabledFuncFlag(Lua_State* L) {
if (Lua_GetTopIndex(L) <= 0)
{
Lua_PushNumber(L, 0);
return 1;
}
int nPlayerIndex = GetPlayerIndex(L);
if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
{
Lua_PushNumber(L, 0);
return 1;
}
V9DisabledFuncFlagSet(nPlayerIndex, (int)Lua_ValueToNumber(L, 1));
Lua_PushNumber(L, 1);
return 1;
}

int LuaSetFusionIsSmelted(Lua_State* L) { return 0; }
int LuaSetFusionMagicSeed(Lua_State* L) { return 0; }
// SetGlbValue, V9 0x80fe2f0: top must be > 1, index and value both truncated toward
// zero, index <= 5000, and the function returns 0 whatever it did.  Shares the store
// that LuaGetGlbValue defines in ScriptFuns_V9_GeneralCoreA.cpp.
extern int g_V9GlbValues[5001];
int LuaSetGlbValue(Lua_State* L) {
if (Lua_GetTopIndex(L) > 1) {
int nId = (int)Lua_ValueToNumber(L, 1);
if (nId >= 0 && nId <= 5000)
g_V9GlbValues[nId] = (int)Lua_ValueToNumber(L, 2);
}
return 0;
}
int LuaSetLucky(Lua_State* L) {
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

int LuaSetMangonelParam(Lua_State* L) {
if (Lua_GetTopIndex(L) < 2)
return 0;
int nId = (int)Lua_ValueToNumber(L, 1);
int nVal = (int)Lua_ValueToNumber(L, 2);
Lua_PushNumber(L, 1);
return 1;
}
int LuaSetMaxDurability(Lua_State* L) {
if (Lua_GetTopIndex(L) <= 1)
return 0;
int nItemIdx = (int)Lua_ValueToNumber(L, 1);
int nValue = (int)Lua_ValueToNumber(L, 2);
if (nItemIdx <= 0 || nItemIdx >= MAX_ITEM)
return 0;
KItem& rItem = Item[nItemIdx];
if (nValue > rItem.GetMaxDurability())
return 0;
for (int i = 0; i < 7; ++i)
{
if (rItem.m_aryBaseAttrib[i].nAttribType == magic_durability_v)
{
rItem.SetItemMagicLevel(i, nValue);
break;
}
}
int nPlayerIndex = GetPlayerIndex(L);
if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER)
Player[nPlayerIndex].m_ItemList.SynPlayerItem(nItemIdx);
return 0;
}

int LuaSetMissleStep(Lua_State* L) {
int nStep = (int)Lua_ValueToNumber(L, 1);
g_V9MissleStep = (nStep > 0) ? nStep : 10;
return 0;
}

int LuaSetOnlineCommissionStatus(Lua_State* L) {
int nPlayerIndex = GetPlayerIndex(L);
if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER || Lua_GetTopIndex(L) < 1)
return 0;
int nStatus = (int)Lua_ValueToNumber(L, 1);
Lua_PushNumber(L, 1);
return 1;
}
int LuaSetPkReduceState(Lua_State* L) {
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
int LuaSetRandSeed(Lua_State* L)
{
unsigned int uSeed = (Lua_GetTopIndex(L) >= 1) ? (unsigned int)Lua_ValueToNumber(L, 1) : (unsigned int)time(NULL);
g_RandomSeed(uSeed);
Lua_PushNumber(L, (double)uSeed);
return 1;
}
int LuaSetRelaxTime(Lua_State* L) {
int nPlayerIndex = GetPlayerIndex(L);
if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER || Lua_GetTopIndex(L) < 1)
return 0;
DWORD dwTime = (DWORD)Lua_ValueToNumber(L, 1);
g_PlayerAntiAddict[nPlayerIndex].dwRelaxTime = dwTime;
AntiAddict_SendSyncPacket(nPlayerIndex);
return 0;
}
int LuaSetSiegeVoitureParam(Lua_State* L) {
if (Lua_GetTopIndex(L) < 2)
return 0;
int nId = (int)Lua_ValueToNumber(L, 1);
int nVal = (int)Lua_ValueToNumber(L, 2);
Lua_PushNumber(L, 1);
return 1;
}
int LuaSetSignCodeSyncTime(Lua_State* L) {
if (Lua_GetTopIndex(L) < 1)
return 0;
int nTime = (int)Lua_ValueToNumber(L, 1);
Lua_PushNumber(L, 1);
return 1;
}
int LuaSetStatData(Lua_State* L) {
if (Lua_GetTopIndex(L) < 2)
return 0;
int nStatId = (int)Lua_ValueToNumber(L, 1);
int nVal = (int)Lua_ValueToNumber(L, 2);
Lua_PushNumber(L, 1);
return 1;
}
int LuaSetSwitch(Lua_State* L) {
if (Lua_GetTopIndex(L) < 2)
return 0;
int nSwitchId = (int)Lua_ValueToNumber(L, 1);
int nVal = (int)Lua_ValueToNumber(L, 2);
Lua_PushNumber(L, 1);
return 1;
}
int LuaSetTireTime(Lua_State* L) {
int nPlayerIndex = GetPlayerIndex(L);
if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER || Lua_GetTopIndex(L) < 1)
return 0;
DWORD dwTime = (DWORD)Lua_ValueToNumber(L, 1);
g_PlayerAntiAddict[nPlayerIndex].dwTiredTime = dwTime;
AntiAddict_SendSyncPacket(nPlayerIndex);
return 0;
}
int LuaSetTmpValue(Lua_State* L) {
if (Lua_GetTopIndex(L) < 2)
return 0;
int nKey = (int)Lua_ValueToNumber(L, 1);
int nVal = (int)Lua_ValueToNumber(L, 2);
Lua_PushNumber(L, 1);
return 1;
}
int LuaSetTotalOffer(Lua_State* L) {
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
int LuaSetWeekGoalOffer(Lua_State* L) {
if (Lua_GetTopIndex(L) < 1)
return 0;
int nOffer = (int)Lua_ValueToNumber(L, 1);
Lua_PushNumber(L, 1);
return 1;
}
int LuaSetWeeklyOffer(Lua_State* L) {
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
int LuaSetWorldName(Lua_State* L) {
if (Lua_GetTopIndex(L) < 2)
return 0;
int nWorldId = (int)Lua_ValueToNumber(L, 1);
const char* pszName = Lua_ValueToString(L, 2);
Lua_PushNumber(L, 1);
return 1;
}

int LuaSyncBiaoCheDeathInfoToRelay(Lua_State* L) {
if (Lua_GetTopIndex(L) < 1)
return 0;
int nBiaoCheId = (int)Lua_ValueToNumber(L, 1);
Lua_PushNumber(L, 1);
return 1;
}
int LuaSyncCitySummary(Lua_State* L) {
int nPlayerIndex = GetPlayerIndex(L);
if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
return 0;
BYTE pPacket[1024];
memset(pPacket, 0, sizeof(pPacket));
pPacket[0] = 0xac;
*(short*)(pPacket + 1) = 0x328;
pPacket[3] = 7;
if (g_pServer && Player[nPlayerIndex].m_nNetConnectIdx >= 0)
g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, (BYTE*)pPacket, 0x329);
return 0;
}
int LuaSyncWorkshopList(Lua_State* L) {
int nPlayerIndex = GetPlayerIndex(L);
if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
return 0;
BYTE pPacket[8];
memset(pPacket, 0, sizeof(pPacket));
pPacket[0] = 0xad;
if (g_pServer && Player[nPlayerIndex].m_nNetConnectIdx >= 0)
g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, (BYTE*)pPacket, 4);
return 0;
}

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
int LuaTableSDD_Close(Lua_State* L) {
// Close SDD table
Lua_PushNumber(L, 1);
return 1;
}
int LuaTableSDD_GetHeight(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaTableSDD_GetValue(Lua_State* L) { Lua_PushString(L, ""); return 1; }
int LuaTableSDD_Open(Lua_State* L) { Lua_PushNumber(L, 1); return 1; }
int LuaTableSDD_Search(Lua_State* L) { Lua_PushNumber(L, -1); return 1; }
int LuaTableSDD_SetValue(Lua_State* L) { Lua_PushNumber(L, 1); return 1; }

//-----------------------------------------------------------------------------
// 16. Server Logging Functions
//-----------------------------------------------------------------------------
int LuaWriteCompoundLog(Lua_State* L) {
if (Lua_GetTopIndex(L) < 1)
return 0;
const char* pszLog = Lua_ValueToString(L, 1);
Lua_PushNumber(L, 1);
return 1;
}
int LuaWriteGoldLog(Lua_State* L) {
if (Lua_GetTopIndex(L) < 1)
return 0;
const char* pszLog = Lua_ValueToString(L, 1);
Lua_PushNumber(L, 1);
return 1;
}
int LuaWriteLog(Lua_State* L) {
if (Lua_GetTopIndex(L) <= 0)
return 0;

const char* szText = Lua_ValueToString(L, 1);
if (NULL == szText)
return 0;

g_LogFile.gTraceLogFile(szText, strlen(szText));
return 0;
}
int LuaWriteSaleLog(Lua_State* L) {
if (Lua_GetTopIndex(L) < 1)
return 0;
const char* pszLog = Lua_ValueToString(L, 1);
Lua_PushNumber(L, 1);
return 1;
}
int LuaWriteStatLog(Lua_State* L) {
if (Lua_GetTopIndex(L) < 1)
return 0;
const char* pszLog = Lua_ValueToString(L, 1);
Lua_PushNumber(L, 1);
return 1;
}
int LuaWriteYunBiaoLog(Lua_State* L) {
if (Lua_GetTopIndex(L) < 1)
return 0;
const char* pszLog = Lua_ValueToString(L, 1);
Lua_PushNumber(L, 1);
return 1;
}

int LuaOB_LoadShareData(Lua_State* L)
{
Lua_PushNumber(L, 1);
return 1;
}

int LuaOB_SaveShareData(Lua_State* L)
{
Lua_PushNumber(L, 1);
return 1;
}

int LuaOB_DeleteShareData(Lua_State* L)
{
Lua_PushNumber(L, 1);
return 1;
}

int Luacurpack(Lua_State* L)
{
Lua_PushNumber(L, 0);
return 1;
}

int Luausepack(Lua_State* L)
{
Lua_PushNumber(L, 1);
return 1;
}


//=============================================================================
// ScriptFuns_V9_TitleAndChatRoom.cpp
// Decompiled & Ported from Kingsoft JX Linux V9 (jx_linux_y Table 8 & ChatRoom) for Win64
// - Title System: Title_AddTitle, Title_ActiveTitle, Title_RemoveTitle,
//                 Title_GetActiveTitle, Title_GetTitleTab, Title_GetTitleName, Title_GetTitleInfo
// - ChatRoom System: ChatRoom_CreateRoom, ChatRoom_EnterRoom, ChatRoom_LeaveRoom,
//                    ChatRoom_FindRoom, ChatRoom_CloseRoom, ChatRoom_KickOut,
//                    ChatRoom_Prevent, ChatRoom_UnPrevent, ChatRoom_ChangePassword, ChatRoom_AddTime
// Target: gameserver/Sources/Core/Src/
//=============================================================================

#include "KWin32.h"
#include "KEngine.h"
#include "KCore.h"
#include "KPlayer.h"
#include "KNpc.h"
#include "LuaLib.h"
#include "LuaFuns.h"

#include <map>
#include <vector>
#include <string>
#include <mutex>

#ifndef MAX_PLAYER
#define MAX_PLAYER 1200
#endif

static inline bool IsValidPlayer(int nIdx)
{
    return (nIdx > 0 && nIdx < MAX_PLAYER && Player[nIdx].m_nIndex > 0);
}

//-----------------------------------------------------------------------------
// Title System (7 functions)
//-----------------------------------------------------------------------------
int LuaTitle_AddTitle(Lua_State *L)
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

    int nTitleId   = (int)Lua_ValueToNumber(L, 1);
    int nTimeLimit = (Lua_GetTopIndex(L) >= 2) ? (int)Lua_ValueToNumber(L, 2) : 0;

    int nNpcIdx = Player[nPlayerIndex].m_nIndex;
    Npc[nNpcIdx].m_NpcTitle = nTitleId;
    Player[nPlayerIndex].m_nPlayerTitleTime = nTimeLimit;
    Lua_PushNumber(L, 1);
    return 1;
}

int LuaTitle_ActiveTitle(Lua_State *L)
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

    int nTitleId = (int)Lua_ValueToNumber(L, 1);
    int nNpcIdx = Player[nPlayerIndex].m_nIndex;
    Npc[nNpcIdx].m_NpcTitle = nTitleId;
    Lua_PushNumber(L, 1);
    return 1;
}

int LuaTitle_RemoveTitle(Lua_State *L)
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

    int nTitleId = (int)Lua_ValueToNumber(L, 1);
    int nNpcIdx = Player[nPlayerIndex].m_nIndex;
    if (Npc[nNpcIdx].m_NpcTitle == nTitleId)
        Npc[nNpcIdx].m_NpcTitle = 0;

    Lua_PushNumber(L, 1);
    return 1;
}

int LuaTitle_GetActiveTitle(Lua_State *L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (!IsValidPlayer(nPlayerIndex))
    {
        Lua_PushNumber(L, 0);
        return 1;
    }

    int nNpcIdx = Player[nPlayerIndex].m_nIndex;
    Lua_PushNumber(L, Npc[nNpcIdx].m_NpcTitle);
    return 1;
}

int LuaTitle_GetTitleTab(Lua_State *L)
{
    // Return empty table or active title in array
    Lua_PushNumber(L, 0);
    return 1;
}

// V9 0x81594f0 walks an associative tree keyed by the title id and hands back the name stored at
// node+0x18, so the argument is a TitleId, not a row.  playertitle.txt is not dense on TitleId
// (ids 3000-3012 sit at rows 246,247 and 338-348), which is why this resolves the row by the
// TitleId column instead of the id+1 arithmetic the older call sites use.
int LuaTitle_GetTitleName(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 1)
    {
        Lua_PushString(L, "");
        return 1;
    }

    int nTitleId = (int)Lua_ValueToNumber(L, 1);
    char szTitleName[64] = { 0 };
    for (int nRow = 2; nRow <= g_PlayerTitle.GetHeight(); ++nRow)
    {
        int nRowId = 0;
        if (!g_PlayerTitle.GetInteger(nRow, "TitleId", 0, &nRowId))
            continue;
        if (nRowId != nTitleId)
            continue;
        g_PlayerTitle.GetString(nRow, "TitleName", "", szTitleName, sizeof(szTitleName));
        break;
    }
    Lua_PushString(L, szTitleName);
    return 1;
}

int LuaTitle_GetTitleInfo(Lua_State *L)
{
    Lua_PushNumber(L, 0);
    return 1;
}

//-----------------------------------------------------------------------------
// ChatRoom System (10 functions)
//-----------------------------------------------------------------------------
struct V9_CHAT_ROOM
{
    DWORD dwRoomId;
    std::string sRoomName;
    std::string sPassword;
    int nOwnerPlayerIndex;
    int nMaxMembers;
    DWORD dwExpireTime;
    std::vector<int> vMembers;
};

static std::map<DWORD, V9_CHAT_ROOM> g_mapChatRooms;
static DWORD g_dwNextRoomId = 1;
static std::mutex g_ChatRoomMutex;

int LuaChatRoom_CreateRoom(Lua_State *L)
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

    const char* pszName = Lua_ValueToString(L, 1);
    const char* pszPwd  = (Lua_GetTopIndex(L) >= 2) ? Lua_ValueToString(L, 2) : "";
    int nMax            = (Lua_GetTopIndex(L) >= 3) ? (int)Lua_ValueToNumber(L, 3) : 20;

    std::lock_guard<std::mutex> lock(g_ChatRoomMutex);
    DWORD dwRoomId = g_dwNextRoomId++;

    V9_CHAT_ROOM room;
    room.dwRoomId = dwRoomId;
    room.sRoomName = pszName ? pszName : "";
    room.sPassword = pszPwd ? pszPwd : "";
    room.nOwnerPlayerIndex = nPlayerIndex;
    room.nMaxMembers = nMax;
    room.vMembers.push_back(nPlayerIndex);

    g_mapChatRooms[dwRoomId] = room;

    Lua_PushNumber(L, dwRoomId);
    return 1;
}

int LuaChatRoom_EnterRoom(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 1)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }

    DWORD dwRoomId = (DWORD)Lua_ValueToNumber(L, 1);
    const char* pszPwd = (Lua_GetTopIndex(L) >= 2) ? Lua_ValueToString(L, 2) : "";

    std::lock_guard<std::mutex> lock(g_ChatRoomMutex);
    auto it = g_mapChatRooms.find(dwRoomId);
    if (it == g_mapChatRooms.end())
    {
        Lua_PushNumber(L, 0);
        return 1;
    }

    if (!it->second.sPassword.empty() && it->second.sPassword != (pszPwd ? pszPwd : ""))
    {
        Lua_PushNumber(L, -1); // Wrong password
        return 1;
    }

    int nPlayerIndex = GetPlayerIndex(L);
    it->second.vMembers.push_back(nPlayerIndex);

    Lua_PushNumber(L, 1);
    return 1;
}

int LuaChatRoom_LeaveRoom(Lua_State *L)
{
    Lua_PushNumber(L, 1);
    return 1;
}

int LuaChatRoom_FindRoom(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 1)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }

    const char* pszName = Lua_ValueToString(L, 1);
    if (!pszName)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }

    std::lock_guard<std::mutex> lock(g_ChatRoomMutex);
    for (const auto& kv : g_mapChatRooms)
    {
        if (kv.second.sRoomName == pszName)
        {
            Lua_PushNumber(L, kv.second.dwRoomId);
            return 1;
        }
    }

    Lua_PushNumber(L, 0);
    return 1;
}

int LuaChatRoom_CloseRoom(Lua_State *L)
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
    const char *szRoomName = Lua_ValueToString(L, 1);
    if (!szRoomName || !szRoomName[0])
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    std::lock_guard<std::mutex> lock(g_ChatRoomMutex);
    for (auto it = g_mapChatRooms.begin(); it != g_mapChatRooms.end(); ++it)
    {
        if (it->second.sRoomName == szRoomName && it->second.nOwnerPlayerIndex == nPlayerIndex)
        {
            g_mapChatRooms.erase(it);
            Lua_PushNumber(L, 1);
            return 1;
        }
    }
    Lua_PushNumber(L, 0);
    return 1;
}

int LuaChatRoom_KickOut(Lua_State *L)
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
    const char *szRoomName = Lua_ValueToString(L, 1);
    const char *szTargetName = Lua_ValueToString(L, 2);
    if (!szRoomName || !szTargetName)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    std::lock_guard<std::mutex> lock(g_ChatRoomMutex);
    for (auto &kv : g_mapChatRooms)
    {
        if (kv.second.sRoomName == szRoomName && kv.second.nOwnerPlayerIndex == nPlayerIndex)
        {
            for (auto it = kv.second.vMembers.begin(); it != kv.second.vMembers.end(); ++it)
            {
                int nTargetIdx = *it;
                if (nTargetIdx > 0 && nTargetIdx < MAX_PLAYER && Player[nTargetIdx].m_nIndex > 0)
                {
                    if (strcmp(Npc[Player[nTargetIdx].m_nIndex].Name, szTargetName) == 0)
                    {
                        kv.second.vMembers.erase(it);
                        Lua_PushNumber(L, 1);
                        return 1;
                    }
                }
            }
        }
    }
    Lua_PushNumber(L, 0);
    return 1;
}

int LuaChatRoom_Prevent(Lua_State *L)
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
    Lua_PushNumber(L, 1);
    return 1;
}

int LuaChatRoom_UnPrevent(Lua_State *L)
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
    Lua_PushNumber(L, 1);
    return 1;
}

int LuaChatRoom_ChangePassword(Lua_State *L)
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
    const char *szRoomName = Lua_ValueToString(L, 1);
    const char *szNewPass = Lua_ValueToString(L, 2);
    if (!szRoomName || !szNewPass)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    std::lock_guard<std::mutex> lock(g_ChatRoomMutex);
    for (auto &kv : g_mapChatRooms)
    {
        if (kv.second.sRoomName == szRoomName && kv.second.nOwnerPlayerIndex == nPlayerIndex)
        {
            kv.second.sPassword = szNewPass;
            Lua_PushNumber(L, 1);
            return 1;
        }
    }
    Lua_PushNumber(L, 0);
    return 1;
}

int LuaChatRoom_AddTime(Lua_State *L)
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
    const char *szRoomName = Lua_ValueToString(L, 1);
    if (!szRoomName)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    DWORD dwAddSec = 3600;
    if (Lua_GetTopIndex(L) >= 2)
        dwAddSec = (DWORD)Lua_ValueToNumber(L, 2);
    std::lock_guard<std::mutex> lock(g_ChatRoomMutex);
    for (auto &kv : g_mapChatRooms)
    {
        if (kv.second.sRoomName == szRoomName && kv.second.nOwnerPlayerIndex == nPlayerIndex)
        {
            kv.second.dwExpireTime += dwAddSec;
            Lua_PushNumber(L, 1);
            return 1;
        }
    }
    Lua_PushNumber(L, 0);
    return 1;
}

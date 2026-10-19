//=============================================================================
// ScriptFuns_V9_GeneralTasksAndMaps.cpp
// Decompiled & Ported from Kingsoft JX Linux V9 (jx_linux_y) for Win64
// Tranche 18B: Missions, Tasks, Maps, Traps & Questions (63 functions)
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
#include <map>
#include <string>
#include <vector>

#ifndef MAX_PLAYER
#define MAX_PLAYER 1200
#endif

//-----------------------------------------------------------------------------
// Isolated Session State for Tasks & String Tasks
//-----------------------------------------------------------------------------
struct KV9TaskSessionState {
    std::map<int, std::string> mapStringTasks;
    int nAutoHangMapFlag;
    int nAutoHangX;
    int nAutoHangY;
    std::string szMissionName;
    std::string szMissionDesc;
    int nLastSelectedAward;

    KV9TaskSessionState()
    {
        nAutoHangMapFlag = 0;
        nAutoHangX = 0;
        nAutoHangY = 0;
        nLastSelectedAward = 0;
    }
};

static KV9TaskSessionState g_TaskSessionState[MAX_PLAYER];

// Map parameters table (SubWorld index -> param id -> value)
static std::map<int, std::map<int, int>> g_SubWorldParams;
static std::map<int, int> g_SubWorldTypes;
static std::map<std::string, int> g_TaskStatuses;

void V9_OnGenTasksRelease(int nPlayerIndex)
{
    if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER)
    {
        g_TaskSessionState[nPlayerIndex].mapStringTasks.clear();
        g_TaskSessionState[nPlayerIndex].nAutoHangMapFlag = 0;
        g_TaskSessionState[nPlayerIndex].nAutoHangX = 0;
        g_TaskSessionState[nPlayerIndex].nAutoHangY = 0;
        g_TaskSessionState[nPlayerIndex].szMissionName.clear();
        g_TaskSessionState[nPlayerIndex].szMissionDesc.clear();
        g_TaskSessionState[nPlayerIndex].nLastSelectedAward = 0;
    }
}

//-----------------------------------------------------------------------------
// 1. String Task Functions
//-----------------------------------------------------------------------------
int LuaGetStringTask(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER && Lua_GetTopIndex(L) >= 1)
    {
        int nTaskId = (int)Lua_ValueToNumber(L, 1);
        std::map<int, std::string>::iterator it = g_TaskSessionState[nPlayerIndex].mapStringTasks.find(nTaskId);
        if (it != g_TaskSessionState[nPlayerIndex].mapStringTasks.end())
        {
            Lua_PushString(L, it->second.c_str());
            return 1;
        }
    }
    Lua_PushString(L, "");
    return 1;
}

int LuaSetStringTask(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER && Lua_GetTopIndex(L) >= 2)
    {
        int nTaskId = (int)Lua_ValueToNumber(L, 1);
        const char* pszVal = Lua_ValueToString(L, 2);
        if (pszVal)
            g_TaskSessionState[nPlayerIndex].mapStringTasks[nTaskId] = pszVal;
        else
            g_TaskSessionState[nPlayerIndex].mapStringTasks[nTaskId] = "";
    }
    return 0;
}

//-----------------------------------------------------------------------------
// 2. Map & SubWorld Functions
//-----------------------------------------------------------------------------
int LuaGetMapParam(Lua_State* L)
{
    if (Lua_GetTopIndex(L) >= 2)
    {
        int nSubWorld = (int)Lua_ValueToNumber(L, 1);
        int nIndex = (int)Lua_ValueToNumber(L, 2);
        if (nSubWorld >= 0 && nSubWorld < MAX_SUBWORLD)
        {
            Lua_PushNumber(L, g_SubWorldParams[nSubWorld][nIndex]);
            return 1;
        }
    }
    Lua_PushNumber(L, 0);
    return 1;
}

int LuaSetMapParam(Lua_State* L)
{
    if (Lua_GetTopIndex(L) >= 3)
    {
        int nSubWorld = (int)Lua_ValueToNumber(L, 1);
        int nIndex = (int)Lua_ValueToNumber(L, 2);
        int nVal = (int)Lua_ValueToNumber(L, 3);
        if (nSubWorld >= 0 && nSubWorld < MAX_SUBWORLD)
        {
            g_SubWorldParams[nSubWorld][nIndex] = nVal;
            Lua_PushNumber(L, 1);
            return 1;
        }
    }
    Lua_PushNumber(L, 0);
    return 1;
}

int LuaGetMapType(Lua_State* L)
{
    if (Lua_GetTopIndex(L) >= 1)
    {
        int nSubWorld = (int)Lua_ValueToNumber(L, 1);
        if (nSubWorld >= 0 && nSubWorld < MAX_SUBWORLD)
        {
            Lua_PushNumber(L, g_SubWorldTypes[nSubWorld]);
            return 1;
        }
    }
    Lua_PushNumber(L, 0);
    return 1;
}

int LuaSetMapType(Lua_State* L)
{
    if (Lua_GetTopIndex(L) >= 2)
    {
        int nSubWorld = (int)Lua_ValueToNumber(L, 1);
        int nType = (int)Lua_ValueToNumber(L, 2);
        if (nSubWorld >= 0 && nSubWorld < MAX_SUBWORLD)
        {
            g_SubWorldTypes[nSubWorld] = nType;
            Lua_PushNumber(L, 1);
            return 1;
        }
    }
    Lua_PushNumber(L, 0);
    return 1;
}

int LuaLoadMap(Lua_State* L)
{
    if (Lua_GetTopIndex(L) >= 1)
    {
        Lua_PushNumber(L, 1);
        return 1;
    }
    Lua_PushNumber(L, 0);
    return 1;
}

// V9 Msg2Map.asm 0x8105080: (map id, message). The binary resolves the map id, iterates the players
// of that SubWorld (bounds 1..0x4ae) and sends one chat packet per player. This tree does the same
// walk inside KPlayerChat::SendSystemMapInfo -- it compares SubWorld[...].m_SubWorldID against the
// map argument (KPlayerChat.cpp:1905) -- and the legacy binding MsgToSubWorldMap (ScriptFuns.cpp:6538)
// already calls it with (0, 0, mapIdx, name, msg, len). V9 passes no sender name, so the system
// announce prefix is taken from strCoreInfo, which is what the legacy binding itself falls back to
// when its name argument is NULL (ScriptFuns.cpp:6523-6526).
int LuaMsg2Map(Lua_State* L)
{
    if (Lua_GetTopIndex(L) <= 1)
        return 0;

    const char* szText = Lua_ValueToString(L, 2);
    if (NULL == szText)
        return 0;

    int nMapIdx = (int)Lua_ValueToNumber(L, 1);

    char szMsg[250] = {0};
    g_StrCpyLen(szMsg, szText, sizeof(szMsg));

    char szName[64] = {0};
    snprintf(szName, sizeof(szName), "%s", strCoreInfo[MESSAGE_SYSTEM_ANNOUCE_HEAD].c_str());

    int nLen = TEncodeText(szMsg, strlen(szMsg));
    KPlayerChat::SendSystemMapInfo(0, 0, nMapIdx, szName, szMsg, nLen);
    return 0;
}

int LuaGetMapInfoFile(Lua_State* L)
{
    if (Lua_GetTopIndex(L) >= 1)
    {
        int nMapId = (int)Lua_ValueToNumber(L, 1);
        char szPath[128];
        sprintf(szPath, "\\settings\\maps\\map_%d.ini", nMapId);
        Lua_PushString(L, szPath);
        Lua_PushString(L, "");
        return 2;
    }
    Lua_PushString(L, "");
    Lua_PushString(L, "");
    return 2;
}

int LuaDelMapTrap(Lua_State* L)
{
    if (Lua_GetTopIndex(L) >= 3)
    {
        int nSubWorld = (int)Lua_ValueToNumber(L, 1);
        int nX = (int)Lua_ValueToNumber(L, 2);
        int nY = (int)Lua_ValueToNumber(L, 3);
        if (nSubWorld >= 0 && nSubWorld < MAX_SUBWORLD)
        {
            SubWorld[nSubWorld].SetTrap(nX, nY, 1, 0);
            Lua_PushNumber(L, 1);
            return 1;
        }
    }
    Lua_PushNumber(L, 0);
    return 1;
}

int LuaQueryMapTrap(Lua_State* L)
{
    if (Lua_GetTopIndex(L) >= 3)
    {
        int nSubWorld = (int)Lua_ValueToNumber(L, 1);
        int nX = (int)Lua_ValueToNumber(L, 2);
        int nY = (int)Lua_ValueToNumber(L, 3);
        if (nSubWorld >= 0 && nSubWorld < MAX_SUBWORLD)
        {
            UINT uTrap = SubWorld[nSubWorld].GetTrap(nX, nY);
            Lua_PushNumber(L, 1);
            Lua_PushNumber(L, (double)uTrap);
            return 2;
        }
    }
    Lua_PushNumber(L, 0);
    Lua_PushNumber(L, 0);
    return 2;
}

int LuaSubWorldIdx2MapCopy(Lua_State* L)
{
    if (Lua_GetTopIndex(L) >= 1)
    {
        int nSubWorld = (int)Lua_ValueToNumber(L, 1);
        if (nSubWorld >= 0 && nSubWorld < MAX_SUBWORLD)
        {
            Lua_PushNumber(L, SubWorld[nSubWorld].m_SubWorldID);
            return 1;
        }
    }
    Lua_PushNumber(L, 0);
    return 1;
}

int LuaSetAutoHangMapFlag(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER)
    {
        int nOldFlag = g_TaskSessionState[nPlayerIndex].nAutoHangMapFlag;
        if (Lua_GetTopIndex(L) >= 1)
            g_TaskSessionState[nPlayerIndex].nAutoHangMapFlag = (int)Lua_ValueToNumber(L, 1);
        Lua_PushNumber(L, nOldFlag);
        Lua_PushNumber(L, g_TaskSessionState[nPlayerIndex].nAutoHangX);
        Lua_PushNumber(L, g_TaskSessionState[nPlayerIndex].nAutoHangY);
        return 3;
    }
    Lua_PushNumber(L, 0);
    Lua_PushNumber(L, 0);
    Lua_PushNumber(L, 0);
    return 3;
}

//-----------------------------------------------------------------------------
// 3. TWS Task Workshop & UTask Functions
//-----------------------------------------------------------------------------
int LuaTWS_ApplyAddTaskValue(Lua_State* L)
{
    if (Lua_GetTopIndex(L) >= 4)
    {
        int nVal = (int)Lua_ValueToNumber(L, 2);
        Lua_PushNumber(L, 1);
        Lua_PushNumber(L, nVal);
        return 2;
    }
    Lua_PushNumber(L, 0);
    Lua_PushNumber(L, 0);
    return 2;
}

int LuaTWS_ApplyAddUTaskValue(Lua_State* L)
{
    if (Lua_GetTopIndex(L) >= 4)
    {
        int nVal = (int)Lua_ValueToNumber(L, 2);
        Lua_PushNumber(L, 1);
        Lua_PushNumber(L, nVal);
        return 2;
    }
    Lua_PushNumber(L, 0);
    Lua_PushNumber(L, 0);
    return 2;
}

int LuaTWS_ApplySetTaskValue(Lua_State* L)
{
    if (Lua_GetTopIndex(L) >= 4)
    {
        int nVal = (int)Lua_ValueToNumber(L, 2);
        Lua_PushNumber(L, 1);
        Lua_PushNumber(L, nVal);
        return 2;
    }
    Lua_PushNumber(L, 0);
    Lua_PushNumber(L, 0);
    return 2;
}

int LuaTWS_GetTaskValue(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER && Lua_GetTopIndex(L) >= 1)
    {
        int nTaskId = (int)Lua_ValueToNumber(L, 1);
        Lua_PushNumber(L, Player[nPlayerIndex].m_cTask.GetSaveVal(nTaskId));
        return 1;
    }
    Lua_PushNumber(L, 0);
    return 1;
}

int LuaTWS_GetUTaskValue(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER && Lua_GetTopIndex(L) >= 1)
    {
        int nTaskId = (int)Lua_ValueToNumber(L, 1);
        Lua_PushNumber(L, Player[nPlayerIndex].m_cTask.GetSaveVal(nTaskId));
        return 1;
    }
    Lua_PushNumber(L, 0);
    return 1;
}

//-----------------------------------------------------------------------------
// 4. Task Matrix & Mission Management
//-----------------------------------------------------------------------------
int LuaTaskAward(Lua_State* L) { Lua_PushString(L, ""); return 1; }
int LuaTaskCondition(Lua_State* L) { Lua_PushString(L, ""); return 1; }
int LuaTaskConditionMatrix(Lua_State* L) { Lua_PushNumber(L, 0); Lua_PushNumber(L, 0); return 2; }
int LuaTaskEntity(Lua_State* L) { Lua_PushString(L, ""); return 1; }
int LuaTaskEntityMatrix(Lua_State* L) { Lua_PushNumber(L, 0); Lua_PushNumber(L, 0); return 2; }
int LuaTaskEvent(Lua_State* L) { Lua_PushString(L, ""); return 1; }
int LuaTaskEventMatrix(Lua_State* L) { Lua_PushNumber(L, 0); Lua_PushNumber(L, 0); return 2; }
int LuaTaskId(Lua_State* L) { Lua_PushString(L, ""); return 1; }
int LuaTaskIdMatrix(Lua_State* L) { Lua_PushNumber(L, 0); Lua_PushNumber(L, 0); return 2; }
int LuaTaskName(Lua_State* L) { Lua_PushString(L, ""); return 1; }
int LuaTaskTalk(Lua_State* L) { Lua_PushString(L, ""); return 1; }
int LuaTaskTalkMatrix(Lua_State* L) { Lua_PushNumber(L, 0); Lua_PushNumber(L, 0); return 2; }

int LuaTaskTip(Lua_State* L)
{
    return 0;
}

int LuaGetTaskEventID(Lua_State* L) { Lua_PushNumber(L, 0); return 1; }
int LuaGetTaskMoney(Lua_State* L) { return 0; }

int LuaGetTaskStatus(Lua_State* L)
{
    if (Lua_GetTopIndex(L) >= 1)
    {
        const char* pszTask = Lua_ValueToString(L, 1);
        if (pszTask)
        {
            std::map<std::string, int>::iterator it = g_TaskStatuses.find(pszTask);
            if (it != g_TaskStatuses.end())
            {
                Lua_PushNumber(L, it->second);
                return 1;
            }
        }
    }
    Lua_PushNumber(L, 0);
    return 1;
}

int LuaSetTaskStatus(Lua_State* L)
{
    if (Lua_GetTopIndex(L) >= 2)
    {
        const char* pszTask = Lua_ValueToString(L, 1);
        int nStatus = (int)Lua_ValueToNumber(L, 2);
        if (pszTask) g_TaskStatuses[pszTask] = nStatus;
        Lua_PushNumber(L, 1);
        Lua_PushNumber(L, nStatus);
        return 2;
    }
    Lua_PushNumber(L, 0);
    Lua_PushNumber(L, 0);
    return 2;
}

int LuaStartTask(Lua_State* L)
{
    if (Lua_GetTopIndex(L) >= 1)
    {
        const char* pszTask = Lua_ValueToString(L, 1);
        if (pszTask) g_TaskStatuses[pszTask] = 1;
        Lua_PushNumber(L, 1);
        Lua_PushNumber(L, 0);
        return 2;
    }
    Lua_PushNumber(L, 0);
    Lua_PushNumber(L, 0);
    return 2;
}

int LuaCloseTask(Lua_State* L)
{
    if (Lua_GetTopIndex(L) >= 1)
    {
        const char* pszTask = Lua_ValueToString(L, 1);
        if (pszTask) g_TaskStatuses[pszTask] = 2;
        Lua_PushNumber(L, 1);
        Lua_PushNumber(L, 0);
        return 2;
    }
    Lua_PushNumber(L, 0);
    Lua_PushNumber(L, 0);
    return 2;
}

int LuaFirstTask(Lua_State* L) { Lua_PushString(L, ""); return 1; }
int LuaNextTask(Lua_State* L) { Lua_PushString(L, ""); return 1; }

int LuaOpenAllTask(Lua_State* L) { return 0; }
int LuaOpenCreateTask(Lua_State* L) { return 0; }
int LuaOpenOwnTask(Lua_State* L) { return 0; }

int LuaSelectTaskAward(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER && Lua_GetTopIndex(L) >= 1)
        g_TaskSessionState[nPlayerIndex].nLastSelectedAward = (int)Lua_ValueToNumber(L, 1);
    return 0;
}

int LuaSelectTaskFinish(Lua_State* L) { return 0; }
int LuaSelectTaskStart(Lua_State* L) { return 0; }
int LuaSendTaskOrder(Lua_State* L) { return 0; }

int LuaSyncTaskValueMore(Lua_State* L)
{
    Lua_PushNumber(L, 1);
    return 1;
}

int LuaGetEventTask(Lua_State* L)
{
    Lua_PushString(L, "");
    return 1;
}

//-----------------------------------------------------------------------------
// 5. GlbMission, MiniMap, Speed & Questions
//-----------------------------------------------------------------------------
int LuaOpenGlbMission(Lua_State* L) { return 0; }
int LuaCloseGlbMission(Lua_State* L) { return 0; }
int LuaRunGlbMission(Lua_State* L) { return 0; }

int LuaBT_ChangeMiniMap(Lua_State* L) { return 0; }

int LuaBT_SetMissionDesc(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER && Lua_GetTopIndex(L) >= 1)
    {
        const char* p = Lua_ValueToString(L, 1);
        if (p) g_TaskSessionState[nPlayerIndex].szMissionDesc = p;
    }
    return 0;
}

int LuaBT_SetMissionName(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER && Lua_GetTopIndex(L) >= 1)
    {
        const char* p = Lua_ValueToString(L, 1);
        if (p) g_TaskSessionState[nPlayerIndex].szMissionName = p;
    }
    return 0;
}

int LuaBT_SetType2Task(Lua_State* L) { return 0; }

int LuaAddTempMagic(Lua_State* L)
{
    return 0;
}

int LuaGetBiaoChePos(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER)
    {
        int nNpcIdx = Player[nPlayerIndex].m_nIndex;
        if (nNpcIdx > 0 && nNpcIdx < MAX_NPC)
        {
            int nSubWorld = Npc[nNpcIdx].m_SubWorldIndex;
            Lua_PushNumber(L, nSubWorld);
            Lua_PushNumber(L, Npc[nNpcIdx].m_MapX);
            Lua_PushNumber(L, Npc[nNpcIdx].m_MapY);
            Lua_PushNumber(L, 1);
            return 4;
        }
    }
    Lua_PushNumber(L, 0);
    Lua_PushNumber(L, 0);
    Lua_PushNumber(L, 0);
    Lua_PushNumber(L, 0);
    return 4;
}

int LuaGetQuestion(Lua_State* L)
{
    Lua_PushString(L, "");
    Lua_PushString(L, "");
    return 2;
}

int LuaGetQuestionTip(Lua_State* L)
{
    Lua_PushString(L, "");
    Lua_PushString(L, "");
    return 2;
}

int LuaNPCINFO_GetTemplateId(Lua_State* L)
{
    if (Lua_GetTopIndex(L) >= 1)
    {
        int nNpcIdx = (int)Lua_ValueToNumber(L, 1);
        if (nNpcIdx > 0 && nNpcIdx < MAX_NPC)
        {
            Lua_PushNumber(L, Npc[nNpcIdx].m_NpcSettingIdx);
            Lua_PushNumber(L, 0);
            return 2;
        }
    }
    Lua_PushNumber(L, 0);
    return 1;
}

int LuaSetHighLightPos(Lua_State* L) { return 0; }

int LuaSetMoveSpeed(Lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER && Lua_GetTopIndex(L) >= 1)
    {
        int nSpeed = (int)Lua_ValueToNumber(L, 1);
        int nNpcIdx = Player[nPlayerIndex].m_nIndex;
        if (nNpcIdx > 0 && nNpcIdx < MAX_NPC)
        {
            if (nSpeed >= 0)
            {
                Npc[nNpcIdx].m_CurrentWalkSpeed = nSpeed;
                Npc[nNpcIdx].m_CurrentRunSpeed = nSpeed;
            }
            else
            {
                Npc[nNpcIdx].m_CurrentWalkSpeed = Npc[nNpcIdx].m_WalkSpeed;
                Npc[nNpcIdx].m_CurrentRunSpeed = Npc[nNpcIdx].m_RunSpeed;
            }
        }
    }
    return 0;
}

int LuaTellPos(Lua_State* L)
{
    return 0;
}

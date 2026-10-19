#ifndef KMISSION_H
#define KMISSION_H

#ifdef _SERVER
#include "KCore.h"
#include "KEngine.h"
#include "KLinkArrayTemplate.h"
#include "KTaskFuns.h"
#include "KPlayerDef.h"

#include "KSubWorld.h"
#include "KSubWorldSet.h"
#include "../../Engine/src/Text.h"   //后来加的
#include "KMissionArray.h"

#include "KPlayerChat.h"
#include "KSortScript.h"
#include "TaskDef.h"
#include <set>

//Mission
//每个Mission有自已的独立的全局变量
//为每一个Mission记录玩家列表的统一的机制
//包括最大人数，等等都要有
//加个Chanel，有利于消息发送
//有功能可以编列所有的参加者id
//extern  _StrCoreInfo   strCoreInfo;
extern int g_PlayerTimerCallBackFun(void * pOwner, char * szScriptFileName);
extern int g_MissionTimerCallBackFun(void * pOwner, char * szScriptFileName);
#define MAX_TIMER_PERMISSION 10  //同时运行的M 数量	10

//#define MAX_MISSION_PARAM	16
//#define MAX_GLBMISSION_PARAM	3

typedef struct 
{
	UINT m_ulPlayerIndex;
	UINT m_ulPlayerID;
	unsigned char m_ucPlayerGroup;
	UINT m_ulJoinTime;
	int			  m_nParam1;
	int			  m_nParam2;
	int			  m_nParam[MAX_MISSION_PARAM];
}TMissionPlayerInfo;

/*
typedef struct 
{
	UINT m_ulNpcIndex;
	UINT m_ulNpcID;
	UINT m_ucNpcGroup;
	UINT m_ulJoinTime;
}TMissionNpcInfo;
*/

//管理玩家集合的模板类

template<class T , UINT ulSize>
class _KMissionPlayerArray:public KLinkArrayTemplate<T , ulSize>
{
public:
	UINT FindSame(T *pT)
	{
		int nIdx = 0;
		while(1)
		{
			nIdx = m_UseIdxTemplate.GetNext(nIdx);
			if (!nIdx)
				break;
			
			if (pT->m_ulPlayerIndex == m_Data[nIdx].m_ulPlayerIndex)
				return nIdx;
		}
		return 0;
	};

	int GetParam1(UINT ulIndex) const
	{
		if (ulIndex >= ulSize) 
			return 0;
		return m_Data[ulIndex].m_nParam1;
	};

	BOOL SetParam1(UINT ulIndex, int nParam1)
	{
		if (ulIndex >= ulSize) 
			return FALSE;
		
		m_Data[ulIndex].m_nParam1 =  nParam1;
		return TRUE;
	};

	int GetParam2(UINT ulIndex) const
	{
		if (ulIndex >= ulSize) 
			return 0;
		return m_Data[ulIndex].m_nParam2;
	};

	BOOL SetParam2(UINT ulIndex, int nParam2)
	{
		if (ulIndex >= ulSize) 
			return FALSE;
		
		m_Data[ulIndex].m_nParam2 =  nParam2;
		return TRUE;
	};
	int GetParam(UINT ulIndex, int nParam) const
	{
		if (ulIndex >= ulSize) 
			return 0;
		return m_Data[ulIndex].m_nParam[nParam];
	};

	BOOL SetParam(UINT ulIndex, int nParam, int nValue);
};

//NPC
/*template<class T , UINT ulSize>
class _KMissionNpcArray:public KLinkArrayTemplate<T , ulSize>
{
public:
	UINT FindSame(T *pT)
	{
		int nIdx = 0;
		while(1)
		{
			nIdx = m_UseIdxTemplate.GetNext(nIdx);
			if (!nIdx)
				break;
			
			
			if (pT->m_ulNpcID == m_Data[nIdx].m_ulNpcID)
			{
				m_Data[nIdx].m_ulNpcIndex = pT->m_ulNpcIndex;
				return nIdx;
			}
		}
		return 0;
	};
};
*/

typedef _KMissionPlayerArray<TMissionPlayerInfo, MAX_PLAYER_> KMissionPlayerArray;
//typedef _KMissionNpcArray<TMissionNpcInfo, MAX_NPC> KMissionNpcArray;

//管理Mission时间触发器集合的模板类
typedef KTimerFunArray <KTimerTaskFun,MAX_TIMER_PERMISSION,g_MissionTimerCallBackFun> KMissionTimerArray;

//#define MAX_MISSION_VALUE_COUNT 1000
class PlayerRank {
public:
	PlayerRank(char* Name)
	{
		if (Name)
		{
			snprintf(name, sizeof(name), "%s", Name);
			id = g_FileName2Id(name);
		}
	}
	PlayerRank(PlayerRank& s)
	{
		snprintf(name, sizeof(name), "%s", s.name);
		group = s.group;
		point = s.point;
		player = s.player;
		npc = s.npc;
		bekilled = s.bekilled;
		serieskill = s.serieskill;
		getitem = s.getitem;

		id = s.id;
	}
	bool Compare(const PlayerRank* src)
	{
		if (!src)
			return false;

		if (point > src->point)
			return true;

		if (point == src->point)
		{
			if (player > src->player)
				return true;

			if (npc == src->npc)
			{
				if (npc > src->npc)
					return true;
			}
		}
		return false;
	}
	char name[64];

	BYTE group;
	unsigned short point;
	unsigned short player;
	unsigned short npc;
	unsigned short bekilled;
	unsigned short serieskill;
	unsigned short getitem;

	int id;
};

class PlayerRankCompare
{
public:
	bool operator () (const PlayerRank* lhs, const PlayerRank* rhs) const
	{
		if (!lhs || !rhs)
			return false;
		return ((PlayerRank*)lhs)->Compare(rhs);
	}
};

class KMission
{
	//int	m_MissionValue[MAX_MISSION_VALUE_COUNT];
	//char m_MissionValue[MAX_MISSIONARRAY_VALUE_COUNT][16];
	UINT m_ulMissionId;
	void * m_pOwner;
	//-----------------------------------------
	bool m_bMissionLadder;
	char m_szMissionName[64];
	int	m_nLadderParam;
	int	m_nGlbLadderParam[MAX_GLBMISSION_PARAM];


public:
	KMissionTimerArray	m_cTimerTaskSet;
	KMission()
	{
		m_pOwner = NULL;
		for (int i = 1; i < MAX_TIMER_PERMISSION; i ++)
		{
			KTimerTaskFun * pTimer = (KTimerTaskFun*)m_cTimerTaskSet.GetData(i);
			if (pTimer)
			pTimer->SetOwner(this);
		}
		//memset(m_MissionValue, 0, sizeof(m_MissionValue));
		m_ulMissionId = 0;
		//------------------
		m_bMissionLadder = false;
		m_szMissionName[0] = 0;
		m_nLadderParam = 0;
		memset(m_nGlbLadderParam, 0, sizeof(m_nGlbLadderParam));

	};

	BOOL	Activate();
	
	void SetOwner(void * pOwner)
	{
		m_pOwner = pOwner;
	};
	
	void * GetOwner()
	{
		return m_pOwner;
	};
	
	void	Init()
	{
//		memset(m_MissionValue, 0, sizeof(m_MissionValue));
		m_ulMissionId = 0;
		//--------------------------
		m_bMissionLadder = false;
		memset(m_szMissionName, 0, sizeof(m_szMissionName));
		m_nLadderParam = 0;
		memset(m_nGlbLadderParam, 0, sizeof(m_nGlbLadderParam));

	};
	//----------------------------------------
	void 	SetPlayerParam(UINT ulIndex, int nParam, int nValue);
	void    UpRankAllParam(int nParam);

	TMissionLadderInfo m_MissionLadder[MISSION_STATNUM];
	std::set<class PlayerRank*, PlayerRankCompare> m_RankGlobal;

	/*void SetMission(UINT ulValueId, const char* szValue)
	{
		if (ulValueId >= MAX_MISSIONARRAY_VALUE_COUNT)
			return ;
		strcpy(m_MissionValue[ulValueId], szValue);
	};

int GetMissionValue(UINT ulValueId) const
	{
		if (ulValueId >= MAX_MISSIONARRAY_VALUE_COUNT)
			return 0;
		
		int nResult = 0;
		if (m_MissionValue[ulValueId][0])
			nResult = atoi(m_MissionValue[ulValueId]);
		return nResult;
	};	
	
	const char* GetMissionString(UINT ulValueId) const
	{
		if (ulValueId >= MAX_MISSIONARRAY_VALUE_COUNT)
			return "";
		return m_MissionValue[ulValueId];
	};*/

	TMissionLadderInfo* GetMin(
		TMissionLadderInfo* const aMSLDList,
		const int n);
	
	void ListSort(
		TMissionLadderInfo* const aMSLDList,	
		const int n);
	
	void DataCopy(TMissionLadderInfo* Desc, TMissionPlayerInfo* Source);
	void SetMissionLadder(const char* szName, int nLadderParam, int* nGlbParam)
	{
		m_bMissionLadder=true;

		if  (szName)
		   memcpy(m_szMissionName, szName, strlen(szName));

		m_nLadderParam = nLadderParam;	//积分模式？
		for(int i=0;i<MAX_GLBMISSION_PARAM;i++)
			m_nGlbLadderParam[i] = nGlbParam[i];
	}
	const char* GetMissionName()
	{
		return m_szMissionName;
	}
	
	int GetMissionLadderParam()
	{
		return m_nLadderParam;
	}
	//----------------------------------------
	

	BOOL SetMissionId(UINT ulMissionId)
	{
		m_ulMissionId = ulMissionId;
		return TRUE;
	}

	UINT GetMissionId()
	{
		return m_ulMissionId;
	}
	
	UINT Msg2Group( char * strMsg, unsigned char ucGroup)
	{
		if (!strMsg || !strMsg[0]) 
			return 0;
		int nCount = 0;
		int nIdx = 0;
		int nLength;
	            nLength = TEncodeText(strMsg, strlen(strMsg));
		while(1)
		{
			nIdx =	m_MissionPlayer.m_UseIdxTemplate.GetNext(nIdx);
			if (!nIdx)
				break;
			
			if (m_MissionPlayer.m_Data[nIdx].m_ucPlayerGroup == ucGroup)
			{
				KPlayerChat::SendSystemInfo(1, m_MissionPlayer.m_Data[nIdx].m_ulPlayerIndex, "通知"/*strCoreInfo[MESSAGE_SYSTEM_TONGZHI_HEAD].c_str()*/, (char *) strMsg, nLength);
				nCount ++;
			}
		}
		return nCount;
	}

	UINT Msg2All(char * strMsg)
	{
		if (!strMsg || !strMsg[0]) 
			return 0;
		int nCount = 0;
		int nIdx = 0;
		int nLength;
		        nLength= TEncodeText(strMsg, strlen(strMsg));
		while(1)
		{
			nIdx =	m_MissionPlayer.m_UseIdxTemplate.GetNext(nIdx);
			if (!nIdx)
				break;
			KPlayerChat::SendSystemInfo(1, m_MissionPlayer.m_Data[nIdx].m_ulPlayerIndex, "战场"/*strCoreInfo[MESSAGE_SYSTEM_DUCHUAN_HEAD].c_str()*/, (char*)strMsg,nLength);//strlen(strMsg)
			nCount ++;
		}
		return nCount;
	}

	UINT Msg2Player(char * strMsg, UINT ulPlayerIndex)
	{
		if (!strMsg || !strMsg[0]) 
			return 0;
		int nLength;
	    nLength = TEncodeText(strMsg, strlen(strMsg));
		KPlayerChat::SendSystemInfo(1, ulPlayerIndex, "通知"/*strCoreInfo[MESSAGE_SYSTEM_TONGZHI_HEAD].c_str()*/, (char *)strMsg, nLength);
	};

	UINT GetNextPlayer(UINT ulIdx, unsigned char ucGroup, UINT &ulPlayerIndex)
	{
		ulPlayerIndex = 0;
		while(1)
		{
			ulIdx =	m_MissionPlayer.m_UseIdxTemplate.GetNext(ulIdx);
			if (ulIdx == 0) 
				return 0;
			
			if (ucGroup)
			{
				if (m_MissionPlayer.m_Data[ulIdx].m_ucPlayerGroup == ucGroup)
				{
					ulPlayerIndex = m_MissionPlayer.m_Data[ulIdx].m_ulPlayerIndex;
					return ulIdx;
				}
			}
			else
			{
				ulPlayerIndex = m_MissionPlayer.m_Data[ulIdx].m_ulPlayerIndex;
				return ulIdx;
			}
		}
		return 0;
	}
	
	UINT GetPlayerCount() const
	{ 
		return m_MissionPlayer.GetUsedCount();
	};
	
	/*void SetMissionValue(UINT ulValueId, int nValue)
	{
		if (ulValueId >= MAX_MISSION_VALUE_COUNT)
			return ;
		m_MissionValue[ulValueId] = nValue;
	};*/

	UINT GetGroupPlayerCount (unsigned char ucGroup ) const 
	{
		if (ucGroup == 0)
			 return GetPlayerCount();
		UINT ulGroupPlayerCount = 0;
		int nIdx = 0;
		while(1)
		{
			nIdx = m_MissionPlayer.m_UseIdxTemplate.GetNext(nIdx);
			if (!nIdx)
				break;
			
			if (m_MissionPlayer.m_Data[nIdx].m_ucPlayerGroup == ucGroup)
				ulGroupPlayerCount ++;
		}
		return ulGroupPlayerCount;
	};	
	
	/*int GetMissionValue(UINT ulValueId) const
	{
		if (ulValueId >= MAX_MISSION_VALUE_COUNT)
			return 0;
		return m_MissionValue[ulValueId];
	};*/
	BOOL	JoinMission(DWORD dwScriptId, int nParam1, int nParam2);

	BOOL	ExecuteScript(char * ScriptFileName, char * szFunName, int nParam);
	BOOL	ExecuteScript(char * ScriptFileName,DWORD dwScriptId,  char * szFunName, int nParam);

	BOOL StartTimer(unsigned char ucTimerId, UINT ulTimerInterval)
	{
		KTimerTaskFun Timer;
		Timer.SetTimer(ulTimerInterval, ucTimerId);
		Timer.SetOwner(this);
		m_cTimerTaskSet.Add(&Timer);
		return TRUE;
	}

	BOOL StopTimer(unsigned char ucTimerId)
	{
		KTimerTaskFun Timer;
		Timer.SetTimer(1, ucTimerId);
		KTimerTaskFun * pTimer = m_cTimerTaskSet.GetData(&Timer);
		if (pTimer)
		{
			pTimer->CloseTimer();
		   m_cTimerTaskSet.Remove(pTimer);
		}
		return TRUE;
	}

	DWORD GetTimerRestTimer(unsigned char ucTimerId)
	{
		KTimerTaskFun Timer;
		Timer.SetTimer(1, ucTimerId);
		//Timer.SetTaskId(ucTimerId);
		KTimerTaskFun * pTimer = m_cTimerTaskSet.GetData(&Timer);
		if (pTimer)
			return pTimer->GetRestTime();
		return 0;
	}

	BOOL StopMission();
	
	KMissionPlayerArray m_MissionPlayer;

/*	UINT FindSame(UINT ulPlayerIndex )
	{
		if (ulPlayerIndex >= MAX_PLAYER)
		{
			return 0;
		}
		int nIdx = 0;

		while(1)
		{
			nIdx = m_UseIdx.GetNext(nIdx);
			if (!nIdx)
				break;
			
			if (m_MissionPlayer[nIdx].ulPlayerIndex == ulPlayerIndex)
				return nIdx;
		}
		return 0;
	};
*/

	UINT	AddPlayer(UINT ulPlayerIndex, UINT ulPlayerID, unsigned char ucPlayerGroup, int ulJoinTime = 0, int nParam1 = 0, int nParam2 = 0);
	
	BOOL	RemovePlayerc(UINT ulPlayerIndex, UINT ulPlayerID = 0,int nSubWorldx=-1);

	UINT GetMissionPlayer_DataIndex(UINT ulPlayerIndex)//
	{
		if (ulPlayerIndex >= MAX_PLAYER_)
			return 0;
		TMissionPlayerInfo Info;
		Info.m_ulPlayerIndex = ulPlayerIndex;
		return m_MissionPlayer.FindSame(&Info);
	}

	UINT GetMissionPlayer_PlayerIndex(UINT ulDataIndex)
	{
		if (ulDataIndex > m_MissionPlayer.GetTotalCount())
			return 0;
		return m_MissionPlayer.m_Data[ulDataIndex].m_ulPlayerIndex;		
	}
	
	UINT GetMissionPlayer_GroupId(UINT ulDataIndex)
	{
		if (ulDataIndex > m_MissionPlayer.GetTotalCount())
			return 0;
		return m_MissionPlayer.m_Data[ulDataIndex].m_ucPlayerGroup;
	}
};
#endif
#endif

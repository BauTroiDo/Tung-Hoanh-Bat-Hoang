#include "KCore.h"
#include "KPlayer.h"
#include "KMission.h"
#include "KNpc.h"
#include "KRegion.h"

#ifdef _SERVER

BOOL KMissionPlayerArray::SetParam(UINT ulIndex, int nParam, int nValue)
{
	if (ulIndex >= ulSize) 
		return FALSE;
   
	if (nParam>0)
	   m_Data[ulIndex].m_nParam[nParam] =  nValue;	 //设置人物数据 的某项数据 的值	1-15项
	
	m_Data[ulIndex].m_nParam[0]	  = MISSION_AVAILABLE_VALUE;

	if (nParam == MISSION_PARAM_AVAILABLE)
	{
		Npc[Player[m_Data[ulIndex].m_ulPlayerIndex].m_nIndex].m_nMissionGroup = nValue==MISSION_AVAILABLE_VALUE?(int)m_Data[ulIndex].m_ucPlayerGroup:-1;
		Player[m_Data[ulIndex].m_ulPlayerIndex].SendMSGroup();	//增加频道
	}
	return TRUE;
};

//获取最小值
TMissionLadderInfo* KMission::GetMin(TMissionLadderInfo* const aMSLDList,const int n)
{
	TMissionLadderInfo* tmpData;
	tmpData = &aMSLDList[0];
	INT32 i;
	while(true)
	{
		for(i=0;i<n;++i)
		{
			if(aMSLDList[i].Name[0] == '\0') //如果名称为空 就返回
				return &aMSLDList[i];
			
			if(tmpData->nParam[m_nLadderParam] > aMSLDList[i].nParam[m_nLadderParam])
			{
				tmpData = &aMSLDList[i];
				break;
			}
		}
		if(i==n)
		break;
	}
	return tmpData;
}

void KMission::DataCopy(TMissionLadderInfo* Desc, TMissionPlayerInfo* Source)
{
	snprintf(Desc->Name, sizeof(Desc->Name), "%s", Player[Source->m_ulPlayerIndex].m_PlayerName); //玩家的名称
	Desc->ucGroup = Source->m_ucPlayerGroup;
	for(int i =0;i<MAX_MISSION_PARAM;i++)
		Desc->nParam[i] = Source->m_nParam[i];
}

void KMission::ListSort(TMissionLadderInfo* const aMSLDList,const int n)
{
	int i,j;
	for(i=0;i<n;++i)
	{
		TMissionLadderInfo* tmpData = &aMSLDList[i];
		for(j=i+1;j<n;++j)
		{
			if( tmpData->nParam[m_nLadderParam] < aMSLDList[j].nParam[m_nLadderParam] )
			{
				TMissionLadderInfo tmp;
				memcpy(&tmp, &aMSLDList[i], sizeof(aMSLDList[i]));
				*tmpData = aMSLDList[j];
				memcpy(&aMSLDList[j], &tmp, sizeof(tmp));
			}
		}
	}
}


BOOL KMission::Activate()
{
	m_cTimerTaskSet.Activate();   //时间任务参数
	return TRUE;
}

BOOL	KMission::JoinMission(DWORD dwScriptId, int nParam1, int nParam2)
{
	KLuaScript* pScript = (KLuaScript*)g_GetScript(dwScriptId);
	if (pScript == NULL)
	{
		printf("-------Script not found [%d][%s]-------- \n", dwScriptId, "JoinMission");
		return FALSE;
	}

	int nTopIndex = 0;
	try
	{
		KSubWorld* pSubWorld = (KSubWorld*)GetOwner();
		if (pSubWorld)
		{
			Lua_PushNumber(pScript->m_LuaState, pSubWorld->m_nIndex);
			pScript->SetGlobalName(SCRIPT_SUBWORLDINDEX);
		}

		nTopIndex = pScript->SafeCallBegin();

		pScript->CallFunction("JoinMission", 0, "dd", nParam1, nParam2);

		nTopIndex = pScript->SafeCallBegin();

		pScript->SafeCallEnd(nTopIndex);

		return TRUE;
	}
	catch (...)
	{
		nTopIndex = pScript->SafeCallBegin();
		pScript->SafeCallEnd(nTopIndex);

		printf("----KMission执行脚本发生意外,Script[%s]----\n", "JoinMission");
		return FALSE;
	}
	return TRUE;
}

BOOL	KMission::ExecuteScript(char * ScriptFileName, char * szFunName, int nParam)
{
	if (!ScriptFileName || !ScriptFileName[0] || !szFunName  || !szFunName[0]) return FALSE;
	return ExecuteScript(ScriptFileName,g_FileName2Id(ScriptFileName), szFunName, nParam);	
}
//服务器端执行脚本
BOOL	KMission::ExecuteScript(char * ScriptFileName,DWORD dwScriptId,  char * szFunName, int nParam)
{
	    if (!szFunName) return false;

		KLuaScript * pScript = (KLuaScript* )g_GetScript(dwScriptId);
		int nTopIndex = 0;

		if (pScript==NULL)
		{
			printf("-------脚本不存在,执行[%s][%s]失败!-------- \n",ScriptFileName,szFunName);
			return FALSE;
		}

	try
	{

		if (pScript)
		{
			KSubWorld * pSubWorld = (KSubWorld*)GetOwner();
			
			if (pSubWorld)
			{
				Lua_PushNumber(pScript->m_LuaState, pSubWorld->m_nIndex);
				pScript->SetGlobalName(SCRIPT_SUBWORLDINDEX);
			}
			
			nTopIndex=pScript->SafeCallBegin();

			 if (nParam==0)
				pScript->CallFunction(szFunName,0,"");
			 else
			    pScript->CallFunction(szFunName,0, "d", nParam);

			nTopIndex=pScript->SafeCallBegin();


			pScript->SafeCallEnd(nTopIndex);
		}
		return TRUE;
	}
	catch(...)
	{
		if (pScript)
		{
			nTopIndex=pScript->SafeCallBegin();
			pScript->SafeCallEnd(nTopIndex);   
		}
		printf("----KMission执行脚本发生意外,Script[%s]----\n",szFunName);
		return FALSE;
	}
	return TRUE;
}

int g_MissionTimerCallBackFun(void * pOwner, char * szScriptFile)
{//计时器运行函数
	if (!pOwner) return FALSE;
	KMission *pMission = (KMission*)pOwner;
	pMission->ExecuteScript(szScriptFile, "OnTimer", 0);
	if (szScriptFile)
		printf("timer %s\n", szScriptFile);  //路径
	return 1;
}

//----------------------------------//获取排名数据
void KMission::SetPlayerParam(UINT ulIndex, int nParam, int nValue)
{
	m_MissionPlayer.SetParam(ulIndex, nParam, nValue);	  //某个人的数据1-15项目
	
	/*m_nLadderParam = nParam;

	if (m_bMissionLadder)
	{
		//if(m_nLadderParam==nParam)	 //要求哪项数据
		{//如果等于 这个 miss下的服务器数据  就发送
			TMissionLadderSelfInfo tmpDataSelf;
			memset(&tmpDataSelf, 0, sizeof(tmpDataSelf));
			memset(m_MissionLadder, 0, sizeof(m_MissionLadder));
			int nIdx = 0,i=0;
			UINT nPlayerIndex = 0;
			nIdx = GetNextPlayer(nIdx,FALSE,nPlayerIndex);	//历遍所有玩家
			while (nIdx)
			{	
				TMissionLadderInfo* tmpData = GetMin(m_MissionLadder, MISSION_STATNUM);	//计算前10排名 获取数据
				if (tmpData->nParam[m_nLadderParam] < m_MissionPlayer.m_Data[nIdx].m_nParam[m_nLadderParam])
				{
					DataCopy(tmpData, &m_MissionPlayer.m_Data[nIdx]); //拷贝数据
				}
				nIdx = GetNextPlayer(nIdx,FALSE,nPlayerIndex);
			}

			ListSort(m_MissionLadder, MISSION_STATNUM);	  //进行排名
			
			int nNameLen = strlen(m_szMissionName);
			if (nNameLen >= sizeof(tmpDataSelf.szMissionName))
				nNameLen = sizeof(tmpDataSelf.szMissionName) - 1;
			memcpy(tmpDataSelf.szMissionName, m_szMissionName, nNameLen);

			tmpDataSelf.nGlbParam[0]=this->GetGroupPlayerCount(m_nGlbLadderParam[0]); //队伍1
			tmpDataSelf.nGlbParam[1]=this->GetGroupPlayerCount(m_nGlbLadderParam[1]); //队伍2
			tmpDataSelf.nGlbParam[2]=this->GetTimerRestTimer(m_nGlbLadderParam[2]);	 //剩余时间
			
			nIdx = 0;
			nPlayerIndex = 0;
			nIdx = GetNextPlayer(nIdx,FALSE, nPlayerIndex);
			while (nIdx)
			{//向所有人发送排名数据	
				tmpDataSelf.ucGroup = m_MissionPlayer.m_Data[nIdx].m_ucPlayerGroup;
				for (i =0;i<MAX_MISSION_PARAM;i++)
					tmpDataSelf.nParam[i] = m_MissionPlayer.m_Data[nIdx].m_nParam[i]; //传递 数据

				Player[nPlayerIndex].SendMSRank(&tmpDataSelf,m_MissionLadder);	      //发送数据

				nIdx = GetNextPlayer(nIdx,FALSE, nPlayerIndex);
			}
		}
	}*/
};


//----------------------------------//获取排名数据
void KMission::UpRankAllParam(int nParam)
{
	if (m_bMissionLadder)
	{
		//m_nLadderParam = nParam;
		//if(m_nLadderParam==nParam)	 //要求哪项数据
		{//如果等于 这个 miss下的服务器数据  就发送
			TMissionLadderSelfInfo tmpDataSelf;
			memset(&tmpDataSelf, 0, sizeof(tmpDataSelf));
			memset(&m_MissionLadder, 0, sizeof(m_MissionLadder));
			int nIdx = 0,i=0;
			UINT nPlayerIndex = 0;
			nIdx = GetNextPlayer(nIdx,FALSE,nPlayerIndex);	//历遍所有玩家
			//TMissionLadderInfo* tmpData = GetMin(m_MissionLadder, MISSION_STATNUM);	//计算前10排名 获取数据
			while (nIdx)
			{//获取这个人的数据	
			   TMissionLadderInfo* tmpData = GetMin(m_MissionLadder, MISSION_STATNUM);	//计算前10排名 获取数据
			   if (tmpData->nParam[m_nLadderParam] < m_MissionPlayer.m_Data[nIdx].m_nParam[m_nLadderParam])
			   { 	  
				  DataCopy(tmpData, &m_MissionPlayer.m_Data[nIdx]); //拷贝数据
			   }  

				nIdx = GetNextPlayer(nIdx,FALSE,nPlayerIndex);
			}
			
			ListSort(m_MissionLadder, MISSION_STATNUM);	  //对这10人 进行排名
			
			int nNameLen = strlen(m_szMissionName);
			if (nNameLen >= sizeof(tmpDataSelf.szMissionName))
				nNameLen = sizeof(tmpDataSelf.szMissionName) - 1;
			memcpy(tmpDataSelf.szMissionName, m_szMissionName, nNameLen);

			tmpDataSelf.nGlbParam[0] = this->GetGroupPlayerCount(m_nGlbLadderParam[0]); //队伍1
			tmpDataSelf.nGlbParam[1] = this->GetGroupPlayerCount(m_nGlbLadderParam[1]); //队伍2
			//tmpDataSelf.nGlbParam[2] = this->GetTimerRestTimer(m_nGlbLadderParam[2]);	 //剩余时间
			int nTimeMission = 60;
			g_GameOption.GetInteger("GameOption", "TimeSongJin", 60, &nTimeMission);
			tmpDataSelf.nGlbParam[2] = (nTimeMission * 1080) - (SubWorld[nParam].m_MissionArray.GetMissionValue(m_nGlbLadderParam[2]) * 1080);

			//printf("====== m_nGlbLadderParam %d %d %d ======\n", m_nGlbLadderParam[0], m_nGlbLadderParam[1], m_nGlbLadderParam[2]);
			//printf("====== tmpDataSelf.nGlbParam %d %d %d ======\n", tmpDataSelf.nGlbParam[0], tmpDataSelf.nGlbParam[1], tmpDataSelf.nGlbParam[2]);
			nIdx = 0;
			nPlayerIndex = 0;
			    nIdx = GetNextPlayer(nIdx,FALSE,nPlayerIndex);
			while (nIdx)
			{//向所有人发送排名数据	
				tmpDataSelf.ucGroup = m_MissionPlayer.m_Data[nIdx].m_ucPlayerGroup;
				for (i =0;i<MAX_MISSION_PARAM;i++)
				{
					if (i==0)
					  tmpDataSelf.nParam[i] = MISSION_AVAILABLE_VALUE;
					else
					  tmpDataSelf.nParam[i] = m_MissionPlayer.m_Data[nIdx].m_nParam[i]; //传递 数据
				}
				
				Player[nPlayerIndex].SendMSRank(&tmpDataSelf,m_MissionLadder);	      //发送数据
				nIdx = GetNextPlayer(nIdx,FALSE,nPlayerIndex);
			}
		}
	}
};


UINT	KMission::AddPlayer(UINT ulPlayerIndex, UINT ulPlayerID, unsigned char ucPlayerGroup, int ulJoinTime, int nParam1, int nParam2)
{
	if (ulPlayerIndex >= MAX_PLAYER || ulPlayerID == 0)
		return 0;
	
	UINT i = 0;
	TMissionPlayerInfo * pInfo = m_MissionPlayer.FindFree(i);
	
	if (!i || !pInfo)
		return 0;
	
	pInfo->m_ulPlayerIndex = ulPlayerIndex;
	pInfo->m_ulPlayerID = ulPlayerID;  ////player dwid
	pInfo->m_nParam1 = nParam1;
	pInfo->m_nParam2 = nParam2;
	pInfo->m_ucPlayerGroup = ucPlayerGroup;
	
	if (!ulJoinTime)
		pInfo->m_ulJoinTime = g_SubWorldSet.GetGameTime();
	else
		pInfo->m_ulJoinTime = ulJoinTime;
	m_MissionPlayer.m_FreeIdxTemplate.Remove(i);
	m_MissionPlayer.m_UseIdxTemplate.Insert(i);
	m_MissionPlayer.m_ulFreeSize --;
	
	m_MissionPlayer.SetParam(i, MISSION_PARAM_AVAILABLE, MISSION_AVAILABLE_VALUE); //初始化为 0
	return i;
}

BOOL	KMission::RemovePlayerc(UINT ulPlayerIndex, UINT ulPlayerID,int nSubWorldx)
{
	if (ulPlayerIndex >= MAX_PLAYER)
		return 0;
	TMissionPlayerInfo Info;
	Info.m_ulPlayerIndex = ulPlayerIndex;
	Info.m_ulPlayerID = ulPlayerID;	//player dwid
	char szScriptT[256]={0};
    DWORD nTdataIdx = m_MissionPlayer.FindSame(&Info);
	if (nTdataIdx)
	{
	  if (!m_MissionPlayer.Remove(nTdataIdx)) //删除节点
	     printf("-- (%s)RemovePlayerc that bai! --\n",m_szMissionName);
	}
	else 
		 printf("-- (%s)Khong co du lieu cho Nhan vat nay, vui long kiem tra --\n",m_szMissionName);

	Npc[Player[ulPlayerIndex].m_nIndex].m_nMissionGroup = -1;
	Player[ulPlayerIndex].SendMSGroup();  //移除频道
	if (m_ulMissionId)
	{	
		char nMissPathIdx[32]={0};
		
		if (nSubWorldx >= 0 && nSubWorldx < MAX_SUBWORLD)
		{
			//sprintf(nMissPathIdx,"%d_map",nSubWorldx);
			sprintf(nMissPathIdx,"%d", m_ulMissionId);
			g_MissionTabFile.GetString(nMissPathIdx, 2,"", szScriptT,sizeof(szScriptT));  //m_ulMissionId + 1
		}
		
		if (szScriptT[0])//离开战场
			ExecuteScript(szScriptT, "OnLeave", ulPlayerIndex);
	}  
	printf("--Roi :RemovePlayerc (Midx:%d)(pidx:%d)(dw:%d)(%s) OK!!!..\n",m_ulMissionId,ulPlayerIndex,ulPlayerID,szScriptT);
	return TRUE;
}

BOOL KMission::StopMission()
{
		UINT nTargetIndex = 0;
		INT nIdx = 0;
		    nIdx = GetNextPlayer(nIdx,FALSE, nTargetIndex);
		while (nIdx)
		{	
			Npc[Player[nTargetIndex].m_nIndex].m_nMissionGroup = -1;
			Player[nTargetIndex].SendMSGroup();
			nIdx = GetNextPlayer(nIdx, FALSE,nTargetIndex);
		}

		m_ulMissionId = 0;
		Init();
		m_cTimerTaskSet.Clear();
		m_MissionPlayer.Clear();
		return TRUE;
} 


#endif

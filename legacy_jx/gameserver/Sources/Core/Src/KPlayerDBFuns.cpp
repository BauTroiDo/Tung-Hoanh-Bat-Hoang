#include "KCore.h"
#ifdef _SERVER
#include "KEngine.h"
#include "KSubWorldSet.h"
#include "KSubWorld.h"
#include "KPlayer.h"
#include "KPlayerTradeJournal.h"
#include "KSG_StringProcess.h"
#ifndef _STANDALONE
//#include "..\MultiServer\Common\Buffer.h"
//using OnlineGameLib::Win32::CPackager;
//using OnlineGameLib::Win32::CBuffer;
#include "../../../lib/S3DBInterface.h"
#else
#include "S3DBInterface.h"
#endif
#include "KNpc.h"
#include "KItem.h"
#include "KItemList.h"
#include "KItemGenerator.h"
#include "KItemSet.h"
#include "KNpcSet.h"
#include "KPlayerSet.h"
#include "KItemChangeRes.h"
#include <time.h>
//#include "MyAssert.H"
#include "KTaskFuns.h"

#include "CRC32.h"

//#include "package.h"
//\Common\Buffer.h	 \MultiServer\Common\Buffer.h
//-----------------------------------后来加的

//CPackager			m_theRecv_p;
//CPackager			m_theSend_p;
//------------------------------------

// 是否将数据库存取档的数据保存下来以供调试
//#define DEBUGOPT_DB_ROLEDATA_OUT 

//KList g_DBMsgList;

//主要用于在线的存档
BOOL KPlayer::SavePlayerData(bool bUnLock)
{
	if (KPlayerTradeJournal::Instance().IsPlayerLocked(this->m_PlayerName))
	{
		printf("[SavePlayerData Blocked]: Player '%s' is locked by Trade Journal Recovery. Skipping save to protect data integrity.\n", this->m_PlayerName);
		return FALSE;
	}

	if (!g_pDataClient)
	{
		printf("--------------Nhan vat bi ngat ket noi khoi Database qua trinh luu tru bi loi--------------- \n");
		return FALSE;
	}

	if (this->m_nPlayerIndex <= 0 || this->m_nPlayerIndex >= MAX_PLAYER)
	{
		printf("-------------------Chi so nhan vat (%d) vuot qua gioi han, luu tru khong thanh cong-----------------------\n",this->m_nPlayerIndex);
		return FALSE;
	}
	int loopCount=0;
	TProcessData sProcessData;
	sProcessData.nProtoId   = c2s_rolekfjhu_saveroleinfo;	  //发往goddes存档
	sProcessData.ulIdentity = this->m_nPlayerIndex;
	sProcessData.bLeave     = bUnLock;
	sProcessData.uSelServer = this->m_CurSerVer;
	//printf("-----------存档索引:%d-----------\n", nIndex);
	TRoleData* pData = (TRoleData *)m_GaemSaveBuffer;	//赋值了最后存档时间
//    cout << "-------角色("<< pData->BaseInfo.szName <<")GS开始存档:物品("<< pData->nItemCount <<")个------索引:" << nIndex << endl;
	 if (NULL==pData||!pData)
	 {
		 printf("-------pData==NULL Save failed-------\n");
		 return false;
	 }
	 if (pData->nItemCount<=0)
	 {
		int nActualRamItems = this->m_ItemList.GetItemCount();
		if (nActualRamItems > 0)
		{
			printf("XXXXXXXXXXXXXXXXXXXXXX(%s)XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n",pData->BaseInfo.szName);
			printf("XXXX Save XXXX [Serialization Anomaly]: RAM has %d items but serialized count is 0!! XXXXX\n", nActualRamItems);
			printf("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n");
		}
		else
		{
			// Legitimate empty inventory: character currently owns 0 items
			printf("[Save Info]: Character (%s) inventory is empty (0 items).\n", pData->BaseInfo.szName);
		}
	 }

	//printf("-----------存档长度C:%d-----------\n", pData->dwDataLen);
	if (pData && pData->dwDataLen)
	{	//size_t == unsigned int
		sProcessData.nDataLen = pData->dwDataLen + sizeof(TProcessData) - 1;
		/*	pData->dwDataLen += 4;
		DWORD	dwCRC = 0;
		dwCRC = CRC32(dwCRC, pData, pData->dwDataLen - 4);*/
		// CRC END	容器增加数据
		//printf("-----------存档长度D:%d-----------\n", pData->dwDataLen);
		m_theSend_p.AddData(c2s_rolekfjhu_saveroleinfo, (const BYTE*)&sProcessData, sizeof(TProcessData) - 1);  //14b
		//printf("-----------存档长度K:%d-----------\n", pData->dwDataLen);
		//m_theSend.AddData(c2s_rolekfjhu_saveroleinfo, (const BYTE*)pData, pData->dwDataLen - 4);
		m_theSend_p.AddData(c2s_rolekfjhu_saveroleinfo, (const BYTE*)pData, pData->dwDataLen);
		//printf("-----------存档长度L:%d-----------\n", pData->dwDataLen);
		//m_theSend.AddData(c2s_rolekfjhu_saveroleinfo, (const BYTE*)&dwCRC, 4);
		//printf("-----------存档长度E:%d-----------\n", pData->dwDataLen);

#ifndef _STANDALONE
		//printf("-----------存档长度F:%d-----------\n", pData->dwDataLen);
		CBuffer* pBuffer = m_theSend_p.GetHeadPack(c2s_rolekfjhu_saveroleinfo);
		//printf("-----------存档长度H:%d-----------\n", pData->dwDataLen);
#else
		size_t actLen = 0;
		//printf("-----------存档长度I:%d-----------\n", pData->dwDataLen);
		char* pBuffer = m_theSend_p.GetHeadPack(c2s_rolekfjhu_saveroleinfo, actLen);
		//printf("-----------存档长度J:%d-----------\n", pData->dwDataLen);
#endif
		int nBufferSize=0;
		while(pBuffer)
		{//发往goddes 
			loopCount++;
			nBufferSize+=pBuffer->GetUsed();
#ifndef _STANDALONE
			//printf("-----------存档长度A:%d-----------\n", pBuffer->GetUsed());
			HRESULT hrSend = g_pDataClient->SendPackToServer(pBuffer->GetBuffer(), pBuffer->GetUsed());
			if (FAILED(hrSend))
			{
				try { if (pBuffer) { pBuffer->Release(); pBuffer = NULL; } } catch(...) {}
				m_theSend_p.DelData(c2s_rolekfjhu_saveroleinfo);
				return FALSE;
			}
#else
			//printf("-----------存档长度B:%d-----------\n",actLen);
			g_pDataClient->SendPackToServer(pBuffer, actLen);
#endif

#ifndef _STANDALONE
			try
			{
				if (pBuffer)
				{
					pBuffer->Release();
					pBuffer = NULL;
				}
			}
			catch(...) 
			{
				//TRACE("SAFE_RELEASE error\n");
			}
#endif

#ifndef _STANDALONE
			pBuffer = m_theSend_p.GetNextPack(c2s_rolekfjhu_saveroleinfo);   //下一个包
#else
			pBuffer = m_theSend.GetNextPack(c2s_rolekfjhu_saveroleinfo, actLen);
#endif
		}
		//删除这个包
		m_theSend_p.DelData(c2s_rolekfjhu_saveroleinfo);
#ifndef _STANDALONE
		try
		{
			if (pBuffer)
			{
				pBuffer->Release();
				pBuffer = NULL;
			}
		}
		catch(...) 
		{
		}
#endif
		printf("-------Saving %s data, size:(%d)--loop:%d--sendSize:%d--itemCount:%d-----\n", pData->BaseInfo.szName, sProcessData.nDataLen,loopCount,nBufferSize,pData->nItemCount);
		//m_pCoreServerShell->SetSaveStatus(nIndex, SAVE_DOING);
		if  (bUnLock)
		   m_uMustSave=SAVE_DONE; 

		return TRUE;
	}

	return FALSE;
}


int KPlayer::AddDBPlayer(char * szPlayerName, int sex, DWORD * pdwID)
{
	return FALSE;
}		
//没返回 导致卡号
int KPlayer::LoadDBPlayerInfo(BYTE *pPlayerInfo, int &nStep, unsigned int &nParam)
{
	_ASSERT(pPlayerInfo);
	int nRet = 0;
	int nRetValue = 0;

	switch(nStep)
	{
	case STEP_BASE_INFO:
		//数据库中玩家基本信息表
         m_pCurStatusOffset = pPlayerInfo;	
		if ((nRet = LoadPlayerBaseInfo(pPlayerInfo, m_pCurStatusOffset, nParam)) == 1)
		{
			nRetValue = SendSyncData(nStep, nParam); //同步数据  有问题 出现卡号
			nStep++;  // 自加
			nParam = 0;
			printf("[StartGame Client] nStep:(%d),nRet:(%d) LoadPlayerBaseInfo  Succes  !!!!!! \n",nStep,nRetValue);
		}
		else 
		{
			if (nRet == -1)
			{//登录失败
				nStep++;
				nParam = 0;
				printf("[StartGame Client] nStep:(%d),nRet:(%d) LoadPlayerBaseInfo [Warring]: Failed !!!!!! \n",nStep,nRetValue);
				return 0;
			}
			else
			{
				nRetValue = TRUE;//SendSyncData(nStep, nParam);
			}
		}
		break;
	case STEP_FIGHT_SKILL_LIST:
		//************************************************
		// 玩家的战斗技能列表
		if ((nRet = LoadPlayerFightSkillList(pPlayerInfo, m_pCurStatusOffset, nParam)) == 1)
		{
			nRetValue = TRUE;//SendSyncData(nStep, nParam);
			nStep++;
			nParam = 0;
			printf("[StartGame Client] nStep:(%d),nRet:(%d) LoadPlayerFightSkillList  Succes  !!!!!! \n",nStep,nRetValue);
		}
		else
		{
			if (nRet == -1)
			{
				nStep++;
				nParam = 0;
				printf("[StartGame Client] nStep:(%d),nRet:(%d) LoadPlayerFightSkillList [Warring]: Failed !!!!!! \n",nStep,nRetValue);
				return 0;
			}
			else
			{
				nRetValue = TRUE;//SendSyncData(nStep, nParam);
			}
		}
		break;
	case STEP_LIFE_SKILL_LIST:
		//*************************************************
		// 玩家的生活技能列表
		if ((nRet = LoadPlayerLifeSkilllList(pPlayerInfo, m_pCurStatusOffset, nParam)) == 1)
		{
			nRetValue = TRUE;//SendSyncData(nStep, nParam);
			nStep++;
			nParam = 0;
			printf("[StartGame Client] nStep:(%d),nRet:(%d) LoadPlayerLifeSkilllList  Succes  !!!!!! \n",nStep,nRetValue);
		}
		else
		{
			if (nRet == -1)
			{
				nStep++;
				nParam = 0;
				printf("[StartGame Client] nStep:(%d),nRet:(%d) LoadPlayerLifeSkilllList  [Warring]: Failed !!!!!! \n",nStep,nRetValue);
				return 0;
			}
			else
			{
				nRetValue =TRUE;// SendSyncData(nStep, nParam);
			}
		}
		
		break;
		
	case STEP_TASK_LIST:
		//*************************************************		
		// 玩家的任务列表
		if ((nRet = LoadPlayerTaskList(pPlayerInfo, m_pCurStatusOffset, nParam)) == 1)
		{
			
			int i = 0;
			for (i  = 0; i < TASKVALUE_MAXWAYPOINT_COUNT; ++i)  //返回点的索引 201---203
			{
				int nWayPoint =m_cTask.GetSaveVal(TASKVALUE_SAVEWAYPOINT_BEGIN + i);// m_cTask.nSave[TASKVALUE_SAVEWAYPOINT_BEGIN + i];
				if (nWayPoint)
				{
					KIndexNode * pNewNode = new KIndexNode;
					pNewNode->m_nIndex = nWayPoint;
					m_PlayerWayPointList.AddTail(pNewNode);
				}
			}
			
			for (i  = 0; i < TASKVALUE_MAXSTATION_COUNT / 2;++i) //返回城市的索引 210---241
			{
				DWORD Stations = 0;
				if (Stations = (DWORD) m_cTask.GetSaveVal(TASKVALUE_SAVESTATION_BEGIN + i))
				{
					int nStation1 = (int) HIWORD(Stations);
					int nStation2 = (int) LOWORD(Stations);
					
					if (nStation1)
					{
						KIndexNode * pNewNode = new KIndexNode;
						pNewNode->m_nIndex = nStation1;
						m_PlayerStationList.AddTail(pNewNode);
					}
					
					if (nStation2)
					{
						KIndexNode * pNewNode = new KIndexNode;
						pNewNode->m_nIndex = nStation2;
						m_PlayerStationList.AddTail(pNewNode);
					}
				}
			}
			g_TimerTask.LoadTask(this);	   //个人时间任务
			nRetValue = TRUE;//SendSyncData(nStep, nParam);
			nStep++;
			nParam = 0;
			printf("[StartGame Client] nStep:(%d),nRet:(%d) LoadPlayerTaskList  Succes  !!!!!! \n",nStep,nRetValue);
		}
		else
		{
			if (nRet == -1)
			{
				nStep++;
				nParam = 0;
				printf("[StartGame Client] nStep:(%d),nRet:(%d) LoadPlayerTaskList  [Warring]: Failed !!!!!! \n",nStep,nRetValue);
				return 0;
			}
			else
			{
				nRetValue = TRUE;//SendSyncData(nStep, nParam);
			}
		}
		break;
	case STEP_ITEM_LIST:
		//*************************************************
		// 玩家的装备列表
		if ((nRet = LoadPlayerItemList(pPlayerInfo,m_pCurStatusOffset,nParam)) == 1)
		{
			nRetValue = SendSyncData(nStep, nParam);  //同步数据
			nStep++;
			nParam = 0;
			printf("[StartGame Client] nStep:(%d),nRet:(%d) LoadPlayerItemList  Succes  !!!!!! \n",nStep,nRetValue);
		}
		else
		{
			if (nRet == -1)
			{
				nStep++;
				nParam = 0;
				printf("[StartGame Client] nStep:(%d),nRet:(%d) LoadPlayerItemList [Warring]: Failed !!!!!! \n",nStep,nRetValue);
				return 0;
			}
			else
			{
				nRetValue = TRUE;//SendSyncData(nStep, nParam);
			}
		}
		break;
	case STEP_FRIEND_LIST:
		//*************************************************
		// 玩家的装备列表
		if ((nRet = LoadPlayerFriendList(pPlayerInfo, m_pCurStatusOffset, nParam)) == 1)
		{
			nRetValue = TRUE;//SendSyncData(nStep, nParam);  //同步数据
			nStep++;
			nParam = 0;
			printf("[StartGame Client] nStep:(%d),nRet:(%d) LoadPlayerFriendList  Succes  !!!!!! \n",nStep,nRetValue);
		}
		else
		{
			if (nRet == -1)
			{
				nStep++;
				nParam = 0;
				printf("[StartGame Client] nStep:(%d),nRet:(%d) LoadPlayerFriendList [Warring]: Failed !!!!!! \n",nStep,nRetValue);
				return 0;
			}
			else
			{//步骤没有增加,继续循环
				nRetValue = TRUE;//SendSyncData(nStep, nParam);
			}
		} 
		break;
	default:
		{
			nStep = STEP_SYNC_END;  //结束
			nRetValue =	TRUE;
		}
		break;
	}
	return nRetValue;   //没返回导致卡号
}

int  KPlayer::SetReBornByte(int nIntValue,int nByteNumber,int nByteValue)
{
	BYTE * pByte =	NULL;
	
	nByteValue = (nByteValue & 0xff);
	
	if (nByteNumber > 4 || nByteNumber <= 0) ///4
		return nIntValue;
	
	pByte = (BYTE*)&nIntValue;
	*(pByte + (nByteNumber -1)) = (BYTE)nByteValue;
	//nIntValue = (nIntValue | (0xff << ((nByteNumber - 1) * 8) )) ;
	//Lua_PushNumber(L, nIntValue);
	return nIntValue;
}

int  KPlayer::GetReBornByte(int nIntValue,int nByteNumber)
{
	int nByteValue = 0;
	
	if (nByteNumber > 4 || nByteNumber <= 0) 
		return nByteValue;
	
	nByteValue = (nIntValue & (0xff << ((nByteNumber - 1) * 8) )) >> ((nByteNumber - 1) * 8);
	//Lua_PushNumber(L, nByteValue);
	return nByteValue;
}


int KPlayer::UpdateDBPlayerInfo(BYTE* pPlayerInfo,DWORD dwBufferLen,BOOL nIsShow)   //人物的基本数据包
{
	//注意，存档取档都必须按照P顺序
	if (!pPlayerInfo)
		return -1;

	//数据库中玩家基本信息表
	SavePlayerBaseInfo(pPlayerInfo);   //1	基本

	// 玩家的战斗技能列表
	SavePlayerFightSkillList(pPlayerInfo);	//2	   战斗技能

	// 玩家的生活技能列表
	SavePlayerLifeSkilllList(pPlayerInfo);	//3		生活技能

	// 玩家的任务列表
	SavePlayerTaskList(pPlayerInfo);	    //4		 任务

	// 玩家的物品列表
	SavePlayerItemList(pPlayerInfo,dwBufferLen);		//5		 物品

	//玩家的好友列表//
	SavePlayerFriendList(pPlayerInfo,dwBufferLen,nIsShow);	  //6	朋友

	return 1;	
}
//加载玩家基本数据
int	KPlayer::LoadPlayerBaseInfo(BYTE  * pRoleBuffer, BYTE * &pRoleBaseBuffer, unsigned int &nParam)
{
	if (!pRoleBuffer)
	{
		KASSERT(pRoleBuffer);
		return -1;
	}
	
	TRoleData * pRoleData = (TRoleData*)pRoleBuffer;
	
	if (nParam != 0) 
		return -1;
	
	int		nSex;
	int		nLevel;	
#define PLAYER_MALE_NPCTEMPLATEID		-1
#define PLAYER_FEMALE_NPCTEMPLATEID		-2

	nLevel = pRoleData->BaseInfo.ifightlevel;
	nSex   = pRoleData->BaseInfo.bSex;
	if (nSex)
		nSex = MAKELONG(nLevel, PLAYER_FEMALE_NPCTEMPLATEID);
	else
		nSex = MAKELONG(nLevel, PLAYER_MALE_NPCTEMPLATEID);


	//登入点
	m_sLoginRevivalPos.m_nSubWorldID = pRoleData->BaseInfo.irevivalid;
	m_sLoginRevivalPos.m_ReviveID    = pRoleData->BaseInfo.irevivalx;
	//战斗模式
	BYTE m_FigthVal = pRoleData->BaseInfo.cFightMode;
	BOOL m_IsCloseTranse = FALSE;
//---------------------------------------------------------------------------------------------
label_retry:
	POINT Pos;  //从模板中获取重生点数据
	g_SubWorldSet.GetRevivalPosFromId(m_sLoginRevivalPos.m_nSubWorldID,m_sLoginRevivalPos.m_ReviveID,&Pos);
	m_sLoginRevivalPos.m_nMpsX = Pos.x;    //赋值重生点
	m_sLoginRevivalPos.m_nMpsY = Pos.y;
//---------------------------------------------------------------------------------------------
	PLAYER_REVIVAL_POS	tempPos;
	POINT sPos;  //从模板中获取重生点数据
	if (m_TempEnTerMapIndx)
	{//如果是制定地图登陆的
		int nTempRevID = g_SubWorldSet.GetRevivalPosFromId(m_TempEnTerMapIndx,-1,&sPos);
		tempPos.m_nSubWorldID=m_TempEnTerMapIndx;
		tempPos.m_nMpsX=sPos.x;
		tempPos.m_nMpsY=sPos.y;
		printf("[----[Start Game] TempEnTerMapIndx: Name:%s SubWorldID:%d x:%d y:%d RevID:%d --------]\n",pRoleData->BaseInfo.szName,tempPos.m_nSubWorldID,sPos.x,sPos.y,nTempRevID);
	}
	else if (pRoleData->BaseInfo.cUseRevive)  //是否利用重生点登陆
	{//设定的重生点地图
		tempPos.m_nSubWorldID = m_sLoginRevivalPos.m_nSubWorldID;
		tempPos.m_ReviveID = m_sLoginRevivalPos.m_ReviveID;
		tempPos.m_nMpsX = m_sLoginRevivalPos.m_nMpsX;
		tempPos.m_nMpsY = m_sLoginRevivalPos.m_nMpsY;
	}
	else
	{//当前下线的地图  跨服  转 gs
	 	//int m_RoleSerVer                      = pRoleData->BaseInfo.isWaiGua;     //原来的服务器编号 
		//int m_ExPcSerVer                      = pRoleData->BaseInfo.iRoleParm6;   //目标服务器
		/*if (m_ExPcSerVer!=0 && m_RoleSerVer == nCurSerIdx)
		{//如果是转服的 并且 是同一个服务器的 使用随机上线点
            POINT sPos;
		    int  nReSetMap;
				g_GameSetTing.GetInteger("Gameconfig","ReSetTransPos",53,&nReSetMap);
			int  nReviveidx = g_SubWorldSet.GetRevivalPosRandIdx(nReSetMap,&sPos);//从汴京 选个上线点
			tempPos.m_nSubWorldID = nReSetMap;
			tempPos.m_nMpsY=sPos.x;
			tempPos.m_nMpsX=sPos.y;
			m_FigthVal = FALSE;    //战斗模式 设置为非战斗模式
			m_IsCloseTranse = TRUE;//关闭转服状态
			m_sLoginRevivalPos.m_nSubWorldID=nReSetMap;
			m_sLoginRevivalPos.m_ReviveID=nReviveidx;
			m_sLoginRevivalPos.m_nMpsX =sPos.x;   //赋值重生点
			m_sLoginRevivalPos.m_nMpsY =sPos.y;
		}
		else*/
		//{//跨服  和 正常登陆的
			tempPos.m_nSubWorldID = pRoleData->BaseInfo.ientergameid;
			tempPos.m_nMpsX       = pRoleData->BaseInfo.ientergamex;
			tempPos.m_nMpsY       = pRoleData->BaseInfo.ientergamey;
		//}
	}
    
	int nSubWorldIdx =g_SubWorldSet.SearchWorld(tempPos.m_nSubWorldID);
	printf("---[login]mg_SubWorldSet.SearchWorld Map:%d---\n",tempPos.m_nSubWorldID);
    if (nSubWorldIdx<0)
	{//如果这个地图不存在的话
        //nSubWorldIdx=g_SubWorldSet.SearchWorld(78); //襄阳
		int nMapid=53,nReviveidx=19;
		//g_GameSetTing.GetInteger2("ServerConfig","PlayerRevPosId",&nMapid,&nReviveidx);  
		nMapid = _gameGlobalSet.PlayerInfo.mSubWorld;
		nReviveidx = _gameGlobalSet.PlayerInfo.mReviveId;
	    g_SubWorldSet.GetRevivalPosFromId(nMapid,nReviveidx,&sPos);
		nSubWorldIdx=g_SubWorldSet.SearchWorld(nMapid);
        tempPos.m_nSubWorldID=nMapid;
        tempPos.m_nMpsY=sPos.x;
        tempPos.m_nMpsX=sPos.y;
		m_FigthVal = FALSE;
		//m_IsCloseTranse =TRUE;
		//----------设置重生点-----------------
        m_sLoginRevivalPos.m_nSubWorldID=nMapid;
        m_sLoginRevivalPos.m_ReviveID=nReviveidx;
		m_sLoginRevivalPos.m_nMpsX =sPos.x;   //赋值重生点
	    m_sLoginRevivalPos.m_nMpsY =sPos.y;
		//printf("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< \n");
		printf("[Error]LoadPlayerBaseInfo: %d(%d)! DBFuns.cpp \n",nSubWorldIdx,nMapid);
		//printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> \n");
	}
    
//增加玩家
	m_nIndex = NpcSet.AddServerNpcA(nSex,nSubWorldIdx,tempPos.m_nMpsX,tempPos.m_nMpsY,-1,0,0,0,0,0);

	if(m_nIndex <= 0) 
	{
		if (m_TempEnTerMapIndx)
		{
			printf("---[Error]m_TempEnTerMapIndx 地图:%d---\n",m_TempEnTerMapIndx);
			m_TempEnTerMapIndx = 0;
			
			goto label_retry;
		}
		else if (pRoleData->BaseInfo.cUseRevive)
		{
			printf("---[Error]pRoleData->BaseInfo.cUseRevive 地图:%d---\n",m_sLoginRevivalPos.m_nSubWorldID);
			pRoleData->BaseInfo.cUseRevive = 0;
			goto label_retry;
		}
		else
		{
		   printf("---[Error]返回 -1 ---\n");
		   return -1;
		}
	}
	//归零
	m_TempEnTerMapIndx = 0;

	m_sDeathRevivalPos = m_sLoginRevivalPos;  //赋值给死亡复活点

	KNpc* pNpc = &Npc[m_nIndex];
	pNpc->m_Kind = kind_player;

	pNpc->SetPlayerIdx(m_nPlayerIndex);  //第几个编号

	pNpc->m_Level = nLevel;

	//pNpc->m_IsVip = pRoleData->BaseInfo.isvip;             //当前会员
	//玩家信息
	//strcpy(pNpc->Name, pRoleData->BaseInfo.szName);
	snprintf(pNpc->Name, sizeof(pNpc->Name), "%s", pRoleData->BaseInfo.szName);
	//char nstrName[64]={0};
	pNpc->SetstrInfo(STR_CHEGNHAO_NAME,pRoleData->BaseInfo.szchenhao);
	pNpc->SetstrInfo(STR_LIENDAU_NAME,pRoleData->BaseInfo.szAlias);//lien dau

	if  (checkGmList(pRoleData->BaseInfo.caccname))
		m_isGm=1;
	else
		m_isGm=0;

	m_nForbiddenFlag = pRoleData->BaseInfo.nForbiddenFlag; //是否被禁言

	if (m_nForbFlag==1 || m_nLiXian==1 || m_nLiXian==2)		 
	{//离线在上线的 解除禁言
	   m_nForbiddenFlag=0;
	   m_nLiXian=0;
	}
	m_CurSerVer                      = pRoleData->BaseInfo.isWaiGua;     //当前的服务器编号 
	m_TempSerVer                     = pRoleData->BaseInfo.iRoleParm6;
	if (m_IsCloseTranse)             //取消转服状态
		m_TempSerVer = 0;

	m_nAttributePoint = pRoleData->BaseInfo.ileftprop;   //属性点
	m_nSkillPoint     = pRoleData->BaseInfo.ileftfight;  //技能点



	m_nStrength		= pRoleData->BaseInfo.ipower;
	m_nDexterity	= pRoleData->BaseInfo.iagility;
	m_nVitality		= pRoleData->BaseInfo.iouter;
	m_nEngergy		= pRoleData->BaseInfo.iinside;
	m_nLucky		= pRoleData->BaseInfo.iluck;  //基本幸运值
	m_cTong.Clear();
	m_cTong.DBSetTongNameID(pRoleData->BaseInfo.dwTongID);  //设置帮会

	m_nCurStrength = m_nStrength;
	if  (m_nCurStrength<0) m_nCurStrength=0;
	m_nCurDexterity = m_nDexterity;
	if  (m_nCurDexterity<0) m_nCurDexterity=0;
	m_nCurVitality = m_nVitality;
	if  (m_nCurVitality<0) m_nCurVitality=0;
	m_nCurEngergy = m_nEngergy;
	if  (m_nCurEngergy<0) m_nCurEngergy=0;

	m_nCurLucky = m_nLucky;
	m_nUpExp = 0; 
	this->SetFirstDamage();    //设置基本伤害
	this->SetBaseAttackRating();
	this->SetBaseDefence();

	//玩家等级信息
	m_nExp			= pRoleData->BaseInfo.fightexp;
	m_nNextLevelExp = PlayerSet.m_cLevelAdd.GetLevelExp(pNpc->m_Level);
	m_dwLeadLevel	= pRoleData->BaseInfo.ileadlevel;
	m_dwLeadExp		= pRoleData->BaseInfo.ileadexp;
   	int nSave = 0;
	//门派信息
	m_cFaction.m_nCurFaction = (char)pRoleData->BaseInfo.nSect;
	m_cFaction.m_nFirstAddFaction = (char)pRoleData->BaseInfo.nFirstSect;
	m_cFaction.m_nAddTimes			= pRoleData->BaseInfo.ijoincount;
	pNpc->m_btRankId				= pRoleData->BaseInfo.isectrole;
	pNpc->m_NpcTitle               = pRoleData->BaseInfo.iPltitle;	  //新称号
	m_nPlayerTitleTime               = pRoleData->BaseInfo.iPltitleTime;	  //新称号
	if (m_nPlayerTitleTime <= KSG_GetCurSec()) 
	{
		pNpc->m_NpcTitle = 0;
		m_nPlayerTitleTime = 0;
	}
	//pNpc->m_CurNpcTitle       = pRoleData->BaseInfo.iPltitle;
	pNpc->m_btRankFFId			= pRoleData->BaseInfo.irankrole;
	pNpc->m_ExItemId				= pRoleData->BaseInfo.iexitemrole;
	pNpc->m_ExBoxId			    = pRoleData->BaseInfo.iexboxrole;
	m_nWorldStat	                    = pRoleData->BaseInfo.nWorldStat;  //排名
	m_nSectStat		                = pRoleData->BaseInfo.nSectStat;
	m_nChestPW                      = pRoleData->BaseInfo.ipassrole;    //锁频密码
	if (m_nChestPW > 0)
		m_CUnlocked = 1;
		//SetChestPW(m_nChestPW);//fix lock

	//pNpc->m_IsInCity		        = GetReBornByte(m_nChestPW,1);//m_nChestPW;哪个	城市的 居民
	//pNpc->IsJinYan                  = GetReBornByte(m_nChestPW,2);//个人的经验倍率
	//pNpc->IsLuKey					= GetReBornByte(m_nChestPW,3);//个人的幸运倍率
	//pNpc->IsJinQian				    = GetReBornByte(m_nChestPW,4);//个人的银两倍率

	//if (pNpc->IsJinYan<=0)
		//pNpc->IsJinYan=0;
	//if (pNpc->IsLuKey<=0)
		//pNpc->IsLuKey=1;
	//if (pNpc->IsJinQian<=0)
		//pNpc->IsJinQian=1;

	//SetChestPW(m_nChestPW);
	pNpc->IsCreatTongBan            = pRoleData->BaseInfo.isCreatTongBan; //是否已经有同伴
	//pNpc->m_IsWaiGua                = pRoleData->BaseInfo.isWaiGua;	
	pNpc->m_IsSerLock               = pRoleData->BaseInfo.isSerLock;
  //  KNewPlayerAttribute nLucks;
	pNpc->m_Series	= pRoleData->BaseInfo.ifiveprop;
//	m_nLucky    = nLucks.m_nLucky[pNpc->m_Series]; //幸运值
	pNpc->m_Camp	= pRoleData->BaseInfo.iteam;
	int	nClient = 0;	
	nClient		= pRoleData->BaseInfo.iclientx;
	m_ItemList.SetServer(nClient);
	pNpc->m_nSex	= pRoleData->BaseInfo.bSex;
	pNpc->m_LifeMax	= pRoleData->BaseInfo.imaxlife;
	pNpc->m_StaminaMax = pRoleData->BaseInfo.imaxstamina;
	pNpc->m_ManaMax = pRoleData->BaseInfo.imaxinner;
	pNpc->m_NuqiMax =pRoleData ->BaseInfo.imaxnuqi;
    nSave		    = pRoleData->BaseInfo.isavemoney;
	pNpc->m_LifeReplenish = PLAYER_LIFE_REPLENISH;
	pNpc->m_ManaReplenish = PLAYER_MANA_REPLENISH;
	pNpc->m_StaminaGain   = PLAYER_STAMINA_GAIN;
	pNpc->m_StaminaLoss   = PLAYER_STAMINA_LOSS;
	//现金和贮物箱中的钱
	int nServer = 0;
	//	this->m_ItemList.Init(GetPlayerIndex());
	nServer		          = pRoleData->BaseInfo.iservermon;
	m_ItemList.SetClient(nServer, nSave,0);
	
	this->SetBaseResistData();
	this->SetBaseSpeedAndRadius();
	pNpc->RestoreNpcBaseInfo();

	pNpc->m_CurrentNuQi = 0;//pRoleData->BaseInfo.icurnuqi;  //当前怒气
	pNpc->m_CurrentLife = pRoleData->BaseInfo.icurlife;
	//fix quai mau npc
	if (pNpc->m_CurrentLife<=0)
	    pNpc->m_CurrentLife=1;	
	pNpc->m_CurrentMana = pRoleData->BaseInfo.icurinner;
	if (pNpc->m_CurrentMana<=0)
	    pNpc->m_CurrentMana=1;
	pNpc->m_CurrentStamina = pRoleData->BaseInfo.icurstamina;
	if (pNpc->m_CurrentStamina<=0)
		pNpc->m_CurrentStamina=1;

	pNpc->m_CJtaskID    = pRoleData->BaseInfo.icjtaskid; //抽奖次数

	for (int q=0;q<MAX_FUMO_COUNT;++q)
	{
		/* pNpc->m_nFuMoNum[q].nNpcIdx     =  pRoleData->BaseInfo.ifumo[q].ifparam;
		 pNpc->m_nFuMoNum[q].nNpcSetings =  pRoleData->BaseInfo.ifumo[q].ifsetno;
		 pNpc->m_nFuMoNum[q].nNpcSkill   =  pRoleData->BaseInfo.ifumo[q].ifskill;
		 pNpc->m_nFuMoNum[q].nSkillLevel =  pRoleData->BaseInfo.ifumo[q].ilevel;
		 */
		 if (pNpc->m_NpcTitle>0)
		 {
			 char mKey[16]={0};
			 snprintf(mKey, sizeof(mKey),"ExtSkill%d",q+1);
			 g_PlayerTitle.GetInteger(pNpc->m_NpcTitle+1,mKey,0,&pNpc->m_ExtSkill[q]);
		 }
		 else
		 {
			 pNpc->m_ExtSkill[q]=0;
		 }
	} 

	pNpc->m_ZhenYuan=pRoleData->BaseInfo.iZhenYuan;             //真元值
    pNpc->m_JinMaiBingJia=pRoleData->BaseInfo.iJinMaiBingJia;        //经脉兵甲值
	//PK信息
	m_cPK.SetNormalPKState(pRoleData->BaseInfo.cPkStatus);
	m_cPK.SetPKValue(pRoleData->BaseInfo.ipkvalue);
	m_cRepute.SetReputeValue(pRoleData->BaseInfo.ireputevalue);
	m_cFuYuan.SetFuYuanValue(pRoleData->BaseInfo.ifuyuanvalue);
	m_cReBorn.SetReBornValue(pRoleData->BaseInfo.irebornvalue);
	m_cVip.SetRoleVipValue(pRoleData->BaseInfo.isvip);
	// 初始化部分数据（这些数据数据库不存储）
	m_BuyInfo.Clear();
	//m_NewBuyInfo.Clear();
	m_cMenuState.Release();
	m_cChat.Release();
	m_cTeam.Release();
	m_cTeam.SetCanTeamFlag(m_nPlayerIndex, TRUE);

	/*
	pRoleData->BaseInfo.iRoleParm1  = pNpc->GetSkillLeftTime(440);		 //经验剩余时间
	pRoleData->BaseInfo.iRoleParm2  = pNpc->GetSkillLeftTime(441);		 //幸运剩余时间
	pRoleData->BaseInfo.iRoleParm3  = pNpc->GetSkillLeftTime(442);		 //全抗剩余时间
	pRoleData->BaseInfo.iRoleParm4  = pNpc->m_GameliveTime;
	*/

	m_MapTimeInfo.nExpLeftTime		  = pRoleData->BaseInfo.iRoleParm1;
	m_MapTimeInfo.nLuckLeftTime		  = pRoleData->BaseInfo.iRoleParm2;
	m_MapTimeInfo.nKangLeftTime		  = pRoleData->BaseInfo.iRoleParm3;
	m_MapTimeInfo.nYaoLeftTime		  = pRoleData->BaseInfo.iRoleParm11;
	m_MapTimeInfo.nSupManaMauLeftTime = pRoleData->BaseInfo.iRoleParm12;
	m_MapTimeInfo.nSupKyNangLeftTime = pRoleData->BaseInfo.iRoleParm13;
	m_MapTimeInfo.nNhatKyLeftTime	 = pRoleData->BaseInfo.iRoleParm14;

	if (pRoleData->BaseInfo.iRoleParm4>0)
	   pNpc->m_GameliveTime = pRoleData->BaseInfo.iRoleParm4;
	else
	   pNpc->m_GameliveTime = -1;

	m_MapTimeInfo.nMapLetTime   = pRoleData->BaseInfo.iRoleParm4;

	pNpc->m_RestNameCount	    = pRoleData->BaseInfo.iRoleParm5;
    m_nCreatRoleTime            = pRoleData->BaseInfo.iRoleParm8;//角色注册的时间
	m_nLimitMaxLevel            = pRoleData->BaseInfo.iRoleParm10;//等级上限
	m_nPeapleIdx = 0;
	m_nObjectIdx = 0;
	memset(m_szTaskAnswerFun, 0, sizeof(m_szTaskAnswerFun));
	memset(m_szTaskAnswerCanshu, 0, sizeof(m_szTaskAnswerCanshu));
	m_nAvailableAnswerNum = 0;
	//m_nCheckWaiGua=0;
	Npc[m_nIndex].m_ActionScriptID = 0;
	Npc[m_nIndex].m_TrapScriptID = 0;
	m_nViewEquipTime = 0;

	pNpc->m_Experience = 0;   //经验倍率
//	memset(pNpc->m_szChatBuffer, 0, sizeof(pNpc->m_szChatBuffer));
//	pNpc->m_nCurChatTime = 0;

	pNpc->m_WeaponType = g_ItemChangeRes.GetWeaponRes(0, 0, 0);
	pNpc->m_ArmorType  = g_ItemChangeRes.GetArmorRes(0, 0);
	pNpc->m_HelmType   = g_ItemChangeRes.GetHelmRes(0, 0);
	pNpc->m_HorseType  = g_ItemChangeRes.GetHorseRes(0, 0); //马的外观
	pNpc->m_bRideHorse = FALSE;
	pNpc->m_PifengType = 0;//g_ItemChangeRes.GetPifengRes(0, 0); //披风
	pNpc->m_ChiBangType = 0;
	nParam = 1;
	// 登入游戏时战斗模式
	pNpc->m_FightMode = m_FigthVal;//pRoleData->BaseInfo.cFightMode;
	// 是否是处于跨服务器状态（转换地图状态）
	if (pRoleData->BaseInfo.cIsExchange)
	{

	}
	pRoleBaseBuffer = (BYTE *)&pRoleData->pBuffer;
	
	return 1;
}

//加载玩家的装备清单  登录时同步装备 
int	KPlayer::LoadPlayerItemList(BYTE * pRoleBuffer , BYTE* &pItemBuffer, unsigned int &nParam)
{
	KASSERT(pRoleBuffer); //接受角色数据
	
	int nItemCount = ((TRoleData *)pRoleBuffer)->nItemCount;

	//TDBItemData * pItemData = (TDBItemData *)pItemBuffer;

	if (nItemCount <= 0)
	{
	   printf("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX \n");
	   printf("XXXX上线XXXX 警告:物品数量为NULL,数量请检查!! XXXXXXXXXXXXXXXXXX \n");
	   printf("XXXX上线XXXX 警告:物品数量为NULL,数量请检查!! XXXXXXXXXXXXXXXXXX \n");
	   printf("XXXX上线XXXX 警告:物品数量为NULL,数量请检查!! XXXXXXXXXXXXXXXXXX \n");
	   printf("XXXX上线XXXX 警告:物品数量为NULL,数量请检查!! XXXXXXXXXXXXXXXXXX \n");
	   printf("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX \n");
	   return 1;
	}

	if (nParam != 0)
	{
		//如果要求获得的物品号大于实际的物品数量，则退出
	  if (nParam >= nItemCount)  //原来的
		  return -1;
	   //  nParam=nItemCount;
	}

	int nItemClass;
	int nLocal = 0;
	int nItemX = 0;
	int nItemY = 0;
	int nBegin = nParam;
	int nEnd   = nParam + DBLOADPERTIME_ITEM;
	BOOL bGetEquiptResult = 0;

	if ( nEnd >=nItemCount)
		 nEnd = nItemCount;

	nParam = nEnd;	
	
	TDBItemData * pItemData=NULL;// = (TDBItemData *)pItemBuffer;

	if (nBegin == 0)
		pItemData = (TDBItemData *)((BYTE*)pRoleBuffer + ((TRoleData*)pRoleBuffer)->dwItemOffset);
	else
		pItemData = (TDBItemData *)pItemBuffer;

	if  (pItemData==NULL)
	{
		printf("XXXXXXXXXXXX 警告:物品数据为NULL,数量(%d)请检查!! XXXXXXXXXXXXXXXXX \n",nItemCount);
		return 1;
	}

	time_t rawtime;
	struct tm * timeinfo;	
	time (&rawtime);
	timeinfo = localtime (&rawtime);  //获取本地时间
		
	for (int i = nBegin ; i < nEnd; i++)
	{
		if  (!pItemData) break;
		
		KItem NewItem;	
		ZeroMemory(&NewItem, sizeof(NewItem));
		
		//物件基本信息**************************************************************
		nItemClass	                            = pItemData->iequipclasscode;
		NewItem.m_CommonAttrib.nItemGenre       = nItemClass;
		NewItem.m_CommonAttrib.nDetailType		= pItemData->idetailtype;
		NewItem.m_CommonAttrib.nParticularType	= pItemData->iparticulartype;
		NewItem.m_CommonAttrib.nLevel			= pItemData->ilevel;
		NewItem.m_CommonAttrib.nSeries			= pItemData->iseries;
		NewItem.m_CommonAttrib.nStackNum		= pItemData->istacknum;
		NewItem.m_CommonAttrib.nEnChance		= pItemData->ienchance;  //强化点数
		NewItem.m_CommonAttrib.nPoint			= pItemData->ipoint;
		NewItem.m_CommonAttrib.nIsWhere			= pItemData->iswhere;    //是辨识的了
		NewItem.m_CommonAttrib.nRongpiont       = pItemData->iRongpiont;  //熔炼
		nItemX			                        = pItemData->ix;
		nItemY			                        = pItemData->iy;
		nLocal			                        = pItemData->ilocal;     //位置
		//增加物件至世界的ItemSet中,以及增加到玩家物品列表中**************************************************************
		//NewItem.SetID(dwItemID); 魔法属性参数
		NewItem.m_GeneratorParam.nGeneratorLevel[0] = pItemData->iparam1;
		NewItem.m_GeneratorParam.nGeneratorLevel[1] = pItemData->iparam2;
		NewItem.m_GeneratorParam.nGeneratorLevel[2] = pItemData->iparam3;
		NewItem.m_GeneratorParam.nGeneratorLevel[3] = pItemData->iparam4;
		NewItem.m_GeneratorParam.nGeneratorLevel[4] = pItemData->iparam5;
		NewItem.m_GeneratorParam.nGeneratorLevel[5] = pItemData->iparam6;
		
		//NewItem.SetID(dwItemID); 熔炼魔法属性参数
		NewItem.m_GeneratorParam.nRGeneratorLevel[0] = pItemData->iparamr1;
		NewItem.m_GeneratorParam.nRGeneratorLevel[1] = pItemData->iparamr2;
		NewItem.m_GeneratorParam.nRGeneratorLevel[2] = pItemData->iparamr3;
		NewItem.m_GeneratorParam.nRGeneratorLevel[3] = pItemData->iparamr4;
		NewItem.m_GeneratorParam.nRGeneratorLevel[4] = pItemData->iparamr5;
		NewItem.m_GeneratorParam.nRGeneratorLevel[5] = pItemData->iparamr6;

		//NewItem.SetID(dwItemID); 宝石魔法属性参数
	/*	NewItem.m_GeneratorParam.nBGeneratorLevel[0] = pItemData->iparamb1;
		NewItem.m_GeneratorParam.nBGeneratorLevel[1] = pItemData->iparamb2;
		NewItem.m_GeneratorParam.nBGeneratorLevel[2] = pItemData->iparamb3;
		NewItem.m_GeneratorParam.nBGeneratorLevel[3] = pItemData->iparamb4;
		NewItem.m_GeneratorParam.nBGeneratorLevel[4] = pItemData->iparamb5;
		NewItem.m_GeneratorParam.nBGeneratorLevel[5] = pItemData->iparamb6;	*/
       
		//NewItem.SetID(dwItemID); 基本属性参数（可变的）
		NewItem.m_GeneratorParam.nJGeneratorLevel[0] = pItemData->iparamj1;
		NewItem.m_GeneratorParam.nJGeneratorLevel[1] = pItemData->iparamj2;
		NewItem.m_GeneratorParam.nJGeneratorLevel[2] = pItemData->iparamj3;
		NewItem.m_GeneratorParam.nJGeneratorLevel[3] = pItemData->iparamj4;
		NewItem.m_GeneratorParam.nJGeneratorLevel[4] = pItemData->iparamj5;
		NewItem.m_GeneratorParam.nJGeneratorLevel[5] = pItemData->iparamj6;
		NewItem.m_GeneratorParam.nJGeneratorLevel[6] = pItemData->iparamj7;

		NewItem.m_GeneratorParam.nVersion			 = pItemData->iequipversion;
		NewItem.m_GeneratorParam.uRandomSeed		 = pItemData->irandseed;    //随机种子
		NewItem.m_GeneratorParam.nLuck				 = pItemData->ilucky;

		NewItem.m_CommonAttrib.nGoldId				 = pItemData->igoldid;      //黄金ID
		NewItem.m_CommonAttrib.nIsPlatina            = pItemData->isplatina;    //是否白金

		NewItem.m_CommonAttrib.nRongNum				 = pItemData->irongNum;     //可溶属性数量
		NewItem.m_CommonAttrib.nWengangPin			 = pItemData->iwengangPin;  //纹纲的品质
		NewItem.m_CommonAttrib.nBinfujiazhi			 = pItemData->ibinfujiazhi;
		NewItem.m_CommonAttrib.nTradePrice		     = pItemData->iIsBang;      //是否绑定
		NewItem.m_CommonAttrib.nIsBang		     	 = pItemData->itradeprice;      //是否绑定
		NewItem.m_CommonAttrib.LockItem.nState		 = pItemData->ilockstate;
		NewItem.m_CommonAttrib.LockItem.dwLockTime	 = pItemData->ilocktime;
		NewItem.m_CommonAttrib.nIsKuaiJie		     = pItemData->iIsKuaiJie;   //是否快捷栏
        NewItem.m_CommonAttrib.nISMagic		         = pItemData->iIsMagic;     //是否魔法属性
		NewItem.m_CommonAttrib.nSkillType		     = pItemData->iSkillType; 
		NewItem.m_CommonAttrib.nUseMap	             = pItemData->iparamb1;		//专用地图
		NewItem.m_CommonAttrib.nRes	                 = pItemData->iparamb2;		//改变的装备外观
		NewItem.m_CommonAttrib.nChiBangRes	         = pItemData->iparamb3;		//翅膀

		if (pItemData->iparamb4<0)
			pItemData->iparamb4 = 0;

		if (pItemData->iparamb4>1)
			pItemData->iparamb4 = 1;

		NewItem.m_CommonAttrib.nUseKind  	         = pItemData->iparamb4;		//使用类型
		snprintf(NewItem.m_CommonAttrib.szWonName, sizeof(NewItem.m_CommonAttrib.szWonName), "%s", pItemData->iWonName);
	//	NewItem.m_CommonAttrib.szWonName = pItemData->iWonName;
		//根据不同的物件类型加载
		//不同的相关资源信息
		switch(nItemClass)
		{
		case item_equip:
			{		
			//	((TRoleData *)pRoleBuffer)->BaseInfo.szName; 
			   if (!pItemData->igoldid)	
			   {
			        bGetEquiptResult = ItemGen.Gen_ExistEquipment(\
					NewItem.m_CommonAttrib.nDetailType,\
					NewItem.m_CommonAttrib.nParticularType,\
					NewItem.m_CommonAttrib.nSeries,\
					NewItem.m_CommonAttrib.nLevel,\
					NewItem.m_GeneratorParam.nGeneratorLevel,\
					NewItem.m_GeneratorParam.nLuck,\
					NewItem.m_GeneratorParam.nVersion,\
					&NewItem,\
					NewItem.m_CommonAttrib.nEnChance,\
					NewItem.m_CommonAttrib.nPoint,\
                    NewItem.m_GeneratorParam.nRGeneratorLevel,\
					NewItem.m_CommonAttrib.nRongpiont,\
					NewItem.m_CommonAttrib.nIsWhere); //生成随机白蓝紫装装备			   
			   }
			   else
			   {
				 if (NewItem.m_CommonAttrib.nIsPlatina==0) //黄金
				 {	
					 if (NewItem.m_CommonAttrib.nPoint==8) //随机黄金
						 bGetEquiptResult = ItemGen.GetGoldItemByRandom(
						 NewItem.m_CommonAttrib.nGoldId,\
						 NewItem.m_CommonAttrib.nDetailType,\
						 NewItem.m_CommonAttrib.nParticularType,\
						 NewItem.m_CommonAttrib.nSeries,\
						 NewItem.m_CommonAttrib.nLevel,\
						 NewItem.m_GeneratorParam.nGeneratorLevel,\
						 NewItem.m_GeneratorParam.nRGeneratorLevel,\
						 NewItem.m_GeneratorParam.nLuck,NewItem.m_GeneratorParam.nVersion,\
						 &NewItem,\
						 NewItem.m_CommonAttrib.nEnChance,NewItem.m_CommonAttrib.nRongpiont,\
                         TRUE,8);
					 else
			            bGetEquiptResult =	ItemGen.GetGoldItemByRong(NewItem.m_CommonAttrib.nDetailType,\
			            NewItem.m_CommonAttrib.nParticularType,\
			            NewItem.m_CommonAttrib.nSeries,\
			            NewItem.m_CommonAttrib.nLevel,\
			            NewItem.m_GeneratorParam.nGeneratorLevel,\
			            NewItem.m_GeneratorParam.nRGeneratorLevel,\
			            NewItem.m_GeneratorParam.nLuck,\
			            NewItem.m_GeneratorParam.nVersion,\
			            &NewItem,\
			            NewItem.m_CommonAttrib.nEnChance,\
			            NewItem.m_CommonAttrib.nRongpiont,\
			            NewItem.m_CommonAttrib.nGoldId,\
					    NULL,TRUE);//NewItem.m_GeneratorParam.nBGeneratorLevel);
				 }
				 else//白金
					 bGetEquiptResult =	ItemGen.GetPlatinaItemByRong(NewItem.m_CommonAttrib.nDetailType,\
					 NewItem.m_CommonAttrib.nParticularType,\
					 NewItem.m_CommonAttrib.nSeries,\
					 NewItem.m_CommonAttrib.nLevel,\
					 NewItem.m_GeneratorParam.nGeneratorLevel,\
					 NewItem.m_GeneratorParam.nRGeneratorLevel,\
					 NewItem.m_GeneratorParam.nLuck,\
					 NewItem.m_GeneratorParam.nVersion,\
					 &NewItem,\
					 NewItem.m_CommonAttrib.nEnChance,\
					 NewItem.m_CommonAttrib.nRongpiont,\
					 NewItem.m_CommonAttrib.nGoldId,\
				     NULL,TRUE);//NewItem.m_GeneratorParam.nBGeneratorLevel);
			   }  
			} 
           break;
		case item_medicine:	
			{// 药品 
				     bGetEquiptResult = ItemGen.Gen_Medicine(\
					       NewItem.m_CommonAttrib.nDetailType,\
					       NewItem.m_CommonAttrib.nLevel,\
					       NewItem.m_GeneratorParam.nVersion,\
					       &NewItem,\
					       NewItem.m_CommonAttrib.nStackNum);  //药品
			}
			break;
		case item_mine:				// 矿石
			{
				bGetEquiptResult = ItemGen.Gen_Mine(NewItem.m_CommonAttrib.nParticularType,\
					&NewItem,\
					NewItem.m_CommonAttrib.nStackNum,\
					NewItem.m_CommonAttrib.nLevel,\
					NewItem.m_CommonAttrib.nSeries,\
					NewItem.m_GeneratorParam.nLuck,\
					NewItem.m_GeneratorParam.nJGeneratorLevel);
			}		
			break;
		case item_materials:			// 药材
			break;
		case item_task:				// 产生任务物品
				bGetEquiptResult = ItemGen.Gen_Quest(NewItem.m_CommonAttrib.nDetailType,&NewItem,NewItem.m_CommonAttrib.nStackNum,NewItem.m_CommonAttrib.nLevel);
			break;
		case item_fusion:				// 产生纹纲
		{
			    bGetEquiptResult = ItemGen.Gen_Fusion(NewItem.m_CommonAttrib.nParticularType,\
				                                                                    &NewItem,\
					                                        NewItem.m_CommonAttrib.nStackNum,\
					                                           NewItem.m_CommonAttrib.nLevel,\
					                                          NewItem.m_CommonAttrib.nSeries,\
					                                          NewItem.m_GeneratorParam.nLuck,NULL);
		}  
			break;
		case item_townportal:
				bGetEquiptResult = ItemGen.Gen_TownPortal(&NewItem);
			break;
		default:
			break;
		}

		// 把生成的装备的耐久度调整为数据库中存储的。
		if (pItemData->idurability != 0)
			NewItem.SetDurability(pItemData->idurability);

		NewItem.SetTime(pItemData->iyear,pItemData->imonth,pItemData->iday,pItemData->ihour,pItemData->imin);

		KLockItem pLock;
		pLock.nState = static_cast<int32_t>(pItemData->ilockstate);
		pLock.dwLockTime = static_cast<int32_t>(pItemData->ilocktime);
		//printf("pLock - nState: %d, dwLockTime: %d\n", pLock.nState, pLock.dwLockTime);
		if (pLock.nState ==  LOCK_STATE_UNLOCK)
		{
			if (pLock.dwLockTime <= KSG_GetCurSec())
			{
				pLock.nState = LOCK_STATE_NORMAL;
				pLock.dwLockTime = 0;
			}
		}
		//printf("pLock (after check) - nState: %d, dwLockTime: %d <= KSG: %d\n", pLock.nState, pLock.dwLockTime, KSG_GetCurSec());
		NewItem.SetLockItem(&pLock);
        NewItem.SetBang(pItemData->iIsBang);
		NewItem.SetTradePrice(pItemData->itradeprice);
		NewItem.SetKuaiJie(pItemData->iIsKuaiJie);
		NewItem.SetMagic(pItemData->iIsMagic);
		NewItem.SetWonName(pItemData->iWonName);
		NewItem.SetSkillType(pItemData->iSkillType);
		NewItem.SetUseMapIdx(pItemData->iparamb1);
		NewItem.SetUseKind(pItemData->iparamb4);
		NewItem.SetItemResIdx(pItemData->iparamb2);

		//NewItem.SetIsCanUse(pItemSync->m_IsCanUse);
		char nTmepInfo[256];
		if  (bGetEquiptResult)
		{
			int nIndex = ItemSet.Add(&NewItem);   // 增加节点 创建内存指针 设置索引 和dwidx

			if (nIndex <= 0) 
				continue ;

			if (nLocal==pos_give)
				printf("--nLocal==pos_give :%d Name:%s ----\n",nIndex,Item[nIndex].GetName());
				m_ItemList.Add(nIndex, nLocal, nItemX, nItemY,TRUE,0,1,TRUE);

            if  (_gameGlobalSet.gameItemInfo.nshowinfo)
			    snprintf(nTmepInfo, sizeof(nTmepInfo),"--[%s]San xuat[%d:%d]:%s,Item |:[%d,%d,%d]Vi tri:[%d,%d,%d]--",m_PlayerName,i,nItemCount,Item[nIndex].GetName(), pItemData->iequipclasscode,pItemData->idetailtype,pItemData->iparticulartype,pItemData->ilocal,pItemData->ix,pItemData->iy);
		
			if (pItemData->iparamb5 > 0)
			{
				m_ItemList.PlaceItemToRef(pItemData->iparamb5 - 1, nIndex);
			}
		}
		else
		{
			if  (_gameGlobalSet.gameItemInfo.nshowinfo)
			    snprintf(nTmepInfo, sizeof(nTmepInfo),"--[%s]San xuat[%d:%d]:That bai,Muc|:[%d,%d,%d]Vi tri:[%d,%d,%d]--",m_PlayerName,i,nItemCount,pItemData->iequipclasscode,pItemData->idetailtype,pItemData->iparticulartype,pItemData->ilocal,pItemData->ix,pItemData->iy);
		}

		if  (_gameGlobalSet.gameItemInfo.nshowinfo)
		      g_LogFile.gTraceLogFile(nTmepInfo,strlen(nTmepInfo),"LoadPlayerItemList");
		


		pItemData++; //下一个物品
	}

	pItemBuffer	= (BYTE *)pItemData;

	if (nParam >= nItemCount)
		return 1;
	else 
		return 0;

	//return 1;
}

//加载战斗技能列表
int	KPlayer::LoadPlayerFightSkillList(BYTE * pRoleBuffer, BYTE * &pFightBuffer, unsigned int& nParam)
{
	KASSERT(pRoleBuffer);
	int nSkillCount = 0;
	//char szSkillId[100];
	//char szSkillLevel[100];
	nSkillCount = ((TRoleData*)(pRoleBuffer))->nFightSkillCount;  //战斗技能总数
	
	if (nSkillCount == 0)	
		return 1;

	int nIsAll=0; 
     	//g_GameSetTing.GetInteger("SYSTEM","IsClearAllSkill",0,&nIsAll);
	nIsAll = _gameGlobalSet.SubWorldInfo.nIsClearAllSkill;
	if (nIsAll==1)
		return 1;

	
	if (nParam >= nSkillCount)
	//	nParam=nSkillCount;
		return -1;
	
	int nBegin	= nParam;
	int nEnd	= nBegin + DBLOADPERTIME_SKILL;
	if (nEnd > nSkillCount) 	
		nEnd = nSkillCount;

	nParam = nEnd;

	TDBSkillData * pSkillData = NULL;
	if (nBegin == 0)
		pSkillData = (TDBSkillData *)((BYTE*)pRoleBuffer + ((TRoleData*)pRoleBuffer)->dwFSkillOffset);
	else
		pSkillData = (TDBSkillData*)pFightBuffer;

	for (int i = nBegin ; i < nEnd; i++, pSkillData++)
	{
		int nCurExpSkill=0;
		int nSkillId = 0;
		int nSkillLevel = 0;
		int nExpSkillPonit = 0;	//熟练度加的额外技能点
		    nCurExpSkill = pSkillData->m_nSkillExp;	//熟练度
		    nSkillId = pSkillData->m_nSkillId;		//技能ID
		    nSkillLevel = pSkillData->m_nSkillLevel;//技能等级
			nExpSkillPonit=pSkillData->m_nExpSkillPonit;//熟练度升的技能点
		try
		{ 
		  Npc[m_nIndex].m_SkillList.Add(nSkillId, nSkillLevel,0,0,0,nCurExpSkill,nExpSkillPonit);  //添加技能数据
		}
		catch (...)
		{
			continue;
		} 
	}

	pFightBuffer = (BYTE*) pSkillData;

	if (nParam >= nSkillCount) 
		return 1;
	else 
		return 0;
}
//加载生活技能列表
int	KPlayer::LoadPlayerLifeSkilllList(BYTE * pRoleBuffer, BYTE * &pSkillBuffer, unsigned int& nParam)
{
	KASSERT(pRoleBuffer);
	int nSkillCount = 0;
	//char szSkillId[100];
	//char szSkillLevel[100];

	nSkillCount = ((TRoleData*)(pRoleBuffer))->nLiveSkillCount;
	
	if (nSkillCount == 0)
		return 1;

	if (nParam >= nSkillCount)	
       // nParam=nSkillCount;
		return -1;

	int nBegin	= nParam;
	int nEnd	= nBegin + DBLOADPERTIME_SKILL;
	if (nEnd > nSkillCount) 	
		nEnd = nSkillCount;

	nParam = nEnd;

	TDBSkillData * pSkillData = (TDBSkillData*)pSkillBuffer;
	for (int i = nBegin ; i < nEnd; i++, pSkillData++)
	{
		if  (!pSkillData) break;
		int nCurExpSkill=0;
		int nSkillId = 0;
		int nSkillLevel = 0;
		nCurExpSkill = pSkillData->m_nSkillExp;
		nSkillId    = pSkillData->m_nSkillId;
		nSkillLevel = pSkillData->m_nSkillLevel;
		try
		{ 
		  Npc[m_nIndex].m_SkillList.Add(nSkillId, nSkillLevel,0,0,0,nCurExpSkill,pSkillData->m_nExpSkillPonit);  //添加技能数据
		}
		catch (...)
		{
			continue;
		}
	}
	pSkillBuffer = (BYTE*) pSkillData;

	if (nParam >= nSkillCount) 
		return 1;
	else 
		return 0;

}
//加载玩家朋友列表
int	KPlayer::LoadPlayerFriendList(BYTE * pRoleBuffer, BYTE * &pFriendBuffer, unsigned int &nParam)
{
	KASSERT(pRoleBuffer);

	/*if (nParam == 1)
		return 0;
	if (nParam == 2)
		return 1; */

	int		nFriendCount;
	DWORD	dwID;
	char	szName[NAME_LENGHT]={0};
	nFriendCount = ((TRoleData*)pRoleBuffer)->nFriendCount;

	if (nFriendCount == 0)
	{
		nParam = 2;
		return 1;
	}

	int nIsAll=0; 
	  // g_GameSetTing.GetInteger("SYSTEM","IsClearAllFriend",0,&nIsAll);
	nIsAll = _gameGlobalSet.SubWorldInfo.nIsClearAllFriend;
	   if (nIsAll==1)
	   {
		 nParam = 2;
		 return 1;
	   }

	char * pCurFriend = (char *)pFriendBuffer;
	for (int i = 0; i < nFriendCount;i++)
	{
		snprintf(szName, sizeof(szName), "%s", (char *)pFriendBuffer);
		if (!szName[0])
			continue;
		dwID = g_FileName2Id(szName);
		m_cChat.DataBaseAddOne(this->m_nPlayerIndex, dwID, szName);
		pFriendBuffer += (strlen(szName) + 1);
	}

	this->m_cChat.StartSendFriendData();

	nParam = 1;
	return 1;
}
//加载人物玩家任务列表
int	KPlayer::LoadPlayerTaskList(BYTE * pRoleBuffer, BYTE * &pTaskBuffer, unsigned int& nParam)
{
	//KASSERT(pRoleBuffer);
	if (!pRoleBuffer) return 1;
	while(m_PlayerStationList.GetHead())
	{
		KIndexNode * pNode = (KIndexNode * ) m_PlayerStationList.GetHead();
		m_PlayerStationList.RemoveHead();  //返回城市点
		if (pNode)
		{
			delete pNode;
			pNode=NULL;
		}
	}
	while(m_PlayerWayPointList.GetHead())  //进过的地点
	{
		KIndexNode * pNode = (KIndexNode * ) m_PlayerWayPointList.GetHead();
		m_PlayerWayPointList.RemoveHead();
		if (pNode)
		{
			delete pNode;
			pNode=NULL;
		}
	}

	if(nParam == 0)	
	   m_cTask.Release();

	  m_cTask.Init(m_nNetConnectIdx);

	int nTaskCount = 0,nTaskId = 0,nTaskDegee = 0;

	nTaskCount = ((TRoleData* )pRoleBuffer)->nTaskCount;  //获取人物数据库可以的 任务数量

	if (nTaskCount == 0) 
		return 1;

	int nIsAll=0; 
	  // g_GameSetTing.GetInteger("SYSTEM","IsClearAllTask",0,&nIsAll);
	nIsAll = _gameGlobalSet.SubWorldInfo.nIsClearAllTask;
	if (nIsAll==1)
		return 1;

	if (nParam >= nTaskCount ) 
       // nParam=nTaskCount;
		return -1;

	int nBegin	= nParam;
	int nEnd	= nBegin + DBLOADPERTIME_TASK;
	if (nEnd > nTaskCount) nEnd = nTaskCount;
	    nParam = nEnd;
	//TDBTaskData * pTaskData = (TDBTaskData*) pTaskBuffer;
	TDBTaskData * pTaskData = NULL;
	if (nBegin == 0)
		pTaskData = (TDBTaskData *)((BYTE*)pRoleBuffer + ((TRoleData*)pRoleBuffer)->dwTaskOffset);
	else
	    pTaskData = (TDBTaskData*)pTaskBuffer;

	//printf("*****************************************\n");
	for (int i = nBegin; i < nEnd; i++ , pTaskData++)
	{
		if (!pTaskData) break;
		nTaskId    = pTaskData->m_nTaskId;
		nTaskDegee = pTaskData->m_nTaskValue;
		if (nTaskId >= MAX_TASK)  //任务数量限制
			continue;

		if  (nTaskDegee)
		   m_cTask.SetSaveVal(nTaskId,nTaskDegee,true);  //上线从新设置 任务设置
	}

	pTaskBuffer = (BYTE*)pTaskData;

	if (nParam >= nTaskCount) 
	  return 1;
	else 
	  return 0;
}

//保存玩家的基本信息
int	KPlayer::SavePlayerBaseInfo(BYTE * pRoleBuffer)
{
	_ASSERT(pRoleBuffer);
	if (m_nIndex <= 0) return -1;

	KNpc * pNpc = &Npc[m_nIndex];
	TRoleData * pRoleData = (TRoleData*)pRoleBuffer;
	//玩家信息
	memset(pRoleData, 0, sizeof(TRoleData));

	pRoleData->bBaseNeedUpdate = 1;
	snprintf(pRoleData->BaseInfo.szName, sizeof(pRoleData->BaseInfo.szName), "%s", m_PlayerName);
	char nstrName[64]={0};
	pNpc->GetstrInfo(STR_CHEGNHAO_NAME,nstrName,sizeof(nstrName));
	strcpy(pRoleData->BaseInfo.szchenhao,nstrName);

	char nstrLienDau[64]={0};
	pNpc->GetstrInfo(STR_LIENDAU_NAME,nstrLienDau,sizeof(nstrLienDau));
	strcpy(pRoleData->BaseInfo.szAlias,nstrLienDau);

	if (m_AccoutName[0])
		strcpy(pRoleData->BaseInfo.caccname, m_AccoutName);
	pRoleData->BaseInfo.nForbiddenFlag = m_nForbiddenFlag;  //禁止发言？
	pRoleData->BaseInfo.ileftprop      = m_nAttributePoint;
	pRoleData->BaseInfo.ileftfight     = m_nSkillPoint;
	pRoleData->BaseInfo.ipower         = m_nStrength;
	pRoleData->BaseInfo.iagility       = m_nDexterity;
	pRoleData->BaseInfo.iouter         = m_nVitality;
	pRoleData->BaseInfo.iinside	    = m_nEngergy;
	pRoleData->BaseInfo.iluck          = m_nLucky;
	pRoleData->BaseInfo.dwTongID       = m_cTong.GetTongNameID();   //帮会名称

	//玩家的显示信息-- 原来没有的	-- remark by spe because client ui display changed 2003/07/21
	/*
	pRoleData->BaseInfo.ihelmres = pNpc->m_HelmType;
	pRoleData->BaseInfo.iarmorres = pNpc->m_ArmorType;
	pRoleData->BaseInfo.iweaponres = pNpc->m_WeaponType;
    */	
	//玩家等级信息
	pRoleData->BaseInfo.fightexp    = m_nExp;
	pRoleData->BaseInfo.ifightlevel = pNpc->m_Level;
	
	pRoleData->BaseInfo.ileadlevel  = m_dwLeadLevel;
	pRoleData->BaseInfo.ileadexp    = m_dwLeadExp;
	//门派信息
	pRoleData->BaseInfo.nSect       = m_cFaction.m_nCurFaction;
	pRoleData->BaseInfo.nFirstSect  = m_cFaction.m_nFirstAddFaction;
	pRoleData->BaseInfo.ijoincount  = m_cFaction.m_nAddTimes;
	pRoleData->BaseInfo.isectrole   = pNpc->m_btRankId;
	pRoleData->BaseInfo.irankrole   = pNpc->m_btRankFFId;
	pRoleData->BaseInfo.iPltitle    = pNpc->m_NpcTitle;
	pRoleData->BaseInfo.iPltitleTime    = m_nPlayerTitleTime;

	pRoleData->BaseInfo.iexitemrole = pNpc->m_ExItemId; // hang trang
	pRoleData->BaseInfo.iexboxrole  = pNpc->m_ExBoxId; // ruong mo rong
	pRoleData->BaseInfo.nWorldStat	= m_nWorldStat;
	pRoleData->BaseInfo.nSectStat	= m_nSectStat;
	if  (nExpSkillIdx>0)
	    pRoleData->BaseInfo.iRoleParm1  = pNpc->GetSkillLeftTime(nExpSkillIdx);		 //经验剩余时间
	else
		pRoleData->BaseInfo.iRoleParm1  = 0;

	if  (nLuckySkillIdx>0)
	    pRoleData->BaseInfo.iRoleParm2  = pNpc->GetSkillLeftTime(nLuckySkillIdx);		 //幸运剩余时间
	else
        pRoleData->BaseInfo.iRoleParm2  = 0;

	if  (nKangkillIdx>0)
	    pRoleData->BaseInfo.iRoleParm3  = pNpc->GetSkillLeftTime(nKangkillIdx);		    //全抗剩余时间
	else
		pRoleData->BaseInfo.iRoleParm3  = 0;

	if  (nYaoPinSkillIdx>0)
	    pRoleData->BaseInfo.iRoleParm11  = pNpc->GetSkillLeftTime(nYaoPinSkillIdx);		    //全抗剩余时间
	else
		pRoleData->BaseInfo.iRoleParm11  = 0;

	if  (nSupManaMauSkillIdx>0)
	    pRoleData->BaseInfo.iRoleParm12  = pNpc->GetSkillLeftTime(nSupManaMauSkillIdx);		    //全抗剩余时间
	else
		pRoleData->BaseInfo.iRoleParm12  = 0;

	if  (nSupKyNangSkillIdx>0)
	    pRoleData->BaseInfo.iRoleParm13  = pNpc->GetSkillLeftTime(nSupKyNangSkillIdx);		    //全抗剩余时间
	else
		pRoleData->BaseInfo.iRoleParm13  = 0;

	if  (nNhatKySkillIdx>0)
	    pRoleData->BaseInfo.iRoleParm14  = pNpc->GetSkillLeftTime(nNhatKySkillIdx);		    //全抗剩余时间
	else
		pRoleData->BaseInfo.iRoleParm14  = 0;

	pRoleData->BaseInfo.iRoleParm4  = pNpc->m_GameliveTime;

	pRoleData->BaseInfo.iRoleParm5  = pNpc->m_RestNameCount;
	pRoleData->BaseInfo.iRoleParm10 = m_nLimitMaxLevel;
	if (pNpc->m_RestNameCount>0)
	{
	   char nRoleName[NAME_LENGHT]={0};
	   pNpc->GetstrInfo(STR_ROLE_CHANGENAME,nRoleName,sizeof(nRoleName));
	   int nLen = strlen(nRoleName);
	   //nLen = nLen > 31 ? 31 : nLen;
	   if (nLen <= NAME_LENGHT)
	       snprintf(pRoleData->BaseInfo.cpartnercode, sizeof(pRoleData->BaseInfo.cpartnercode), "%s", nRoleName);
	   else
		   pRoleData->BaseInfo.iRoleParm5 = 0;
	}

	//pRoleData->BaseInfo.iRoleParm6  = 0;
	pRoleData->BaseInfo.iRoleParm7  = 0;
	pRoleData->BaseInfo.iRoleParm8  = m_nCreatRoleTime;
	pRoleData->BaseInfo.iRoleParm9  = 0;
	//pRoleData->BaseInfo.iRoleParm10 = 0;

	//m_nChestPW = SetReBornByte(m_nChestPW,1,pNpc->m_IsInCity);//m_nChestPW;哪个	城市的 居民
	//m_nChestPW = SetReBornByte(m_nChestPW,2,pNpc->IsJinYan);  //个人的经验倍率
	//m_nChestPW = SetReBornByte(m_nChestPW,3,pNpc->IsLuKey);   //个人的幸运倍率
	//m_nChestPW = SetReBornByte(m_nChestPW,4,pNpc->IsJinQian); //个人的银两倍率

	pRoleData->BaseInfo.ipassrole   = m_nChestPW;       //锁屏幕密码
	pRoleData->BaseInfo.icjtaskid   = pNpc->m_CJtaskID;  //抽奖次数
	pRoleData->BaseInfo.isCreatTongBan = pNpc->IsCreatTongBan;  // 同伴
	pRoleData->BaseInfo.isWaiGua       = m_CurSerVer;           // 当前的存档的服务器编号  区
	pRoleData->BaseInfo.isSerLock      = pNpc->m_IsSerLock;     //是否锁住某人

	pRoleData->BaseInfo.iZhenYuan           = pNpc->m_ZhenYuan;             //真元值
    pRoleData->BaseInfo.iJinMaiBingJia      = pNpc->m_JinMaiBingJia;        //经脉兵甲值

	//现金和贮物箱中的钱
	int nServer = 0;
	int nSave = 0;
	int	nClient	=0;
	nServer     = m_ItemList.GetsMon(room_equipment);
	nClient     = m_ItemList.GetxLient(room_equipment);
	nSave	    = m_ItemList.GetsMon(room_repository);
	pRoleData->BaseInfo.iservermon		= nServer;
	pRoleData->BaseInfo.isavemoney		= nSave;
	pRoleData->BaseInfo.iclientx		= nClient;
	//pRoleData->BaseInfo.isavemoney	= nSave;
	pRoleData->BaseInfo.ifiveprop		= pNpc->m_Series;
	pRoleData->BaseInfo.iteam			= pNpc->m_Camp;
	pRoleData->BaseInfo.bSex			= pNpc->m_nSex;
	pRoleData->BaseInfo.imaxlife		= pNpc->m_LifeMax;
	pRoleData->BaseInfo.imaxnuqi		= pNpc->m_NuqiMax; //怒气
	pRoleData->BaseInfo.imaxstamina		= pNpc->m_StaminaMax;
	pRoleData->BaseInfo.imaxinner		= pNpc->m_ManaMax;
	pRoleData->BaseInfo.icurlife		= pNpc->m_CurrentLife;
	pRoleData->BaseInfo.icurinner		= pNpc->m_CurrentMana;
	pRoleData->BaseInfo.icurstamina		= pNpc->m_CurrentStamina;
	//pRoleData->BaseInfo.isvip		    = pNpc->m_IsVip; //当前会员

	//登入点
	pRoleData->BaseInfo.irevivalid      = 	m_sLoginRevivalPos.m_nSubWorldID;
	pRoleData->BaseInfo.irevivalx       = 	m_sLoginRevivalPos.m_ReviveID;
	pRoleData->BaseInfo.irevivaly       = 	0;

	if (m_bExchangeServer)	// 准备跨服务器：把存盘点设为目标点
	{//转换地图中
		pRoleData->BaseInfo.cUseRevive   = 0; //不使用重生点上线
		pRoleData->BaseInfo.ientergameid = m_sExchangePos.m_dwMapID;
		pRoleData->BaseInfo.ientergamex  = m_sExchangePos.m_nX;
		pRoleData->BaseInfo.ientergamey  = m_sExchangePos.m_nY;
		pRoleData->BaseInfo.cFightMode   = (BYTE)pNpc->m_FightMode;
	}
	else if (pNpc->m_SubWorldIndex >= 0 && pNpc->m_RegionIndex >= 0 && pNpc->m_Doing != do_death && pNpc->m_Doing != do_revive)
	{//如果在线的话没有死亡的 或者 正在重生的 当前下线地图
		//printf("-------------在线游戏的存档-----------\n");
		pRoleData->BaseInfo.cUseRevive   = m_bUseReviveIdWhenLogin;   //是否利用重生点

		pRoleData->BaseInfo.ientergameid = SubWorld[pNpc->m_SubWorldIndex].m_SubWorldID;
		int pMap;
		pNpc->GetMpsPos(&pRoleData->BaseInfo.ientergamex, &pRoleData->BaseInfo.ientergamey,&pMap);  //获取当前在线的坐标
		pRoleData->BaseInfo.cFightMode = (BYTE)pNpc->m_FightMode;

	}
	else
	{//如果死亡了 或 正在重生 或 直接退出游戏的 
		pRoleData->BaseInfo.cUseRevive = 1;  //使用重生点上线
		pRoleData->BaseInfo.cFightMode = 0;
		//printf("------------死亡时存档-----------\n");
		if (pNpc->m_Doing == do_death || pNpc->m_Doing == do_revive)
		{//如果是死亡了的
			pRoleData->BaseInfo.icurlife    = pNpc->m_LifeMax;
			pRoleData->BaseInfo.icurinner   = pNpc->m_ManaMax;
			pRoleData->BaseInfo.icurstamina = pNpc->m_StaminaMax;
			//pRoleData->BaseInfo.isvip       = 0;  //死亡就存档为0
		}
		else
		{
            //printf("-------------直接退出游戏的存档-----------\n");

		}
	}
	//是同一个服务器的才存档
	//if (m_TempSerVer!=0 &&/* m_CurSerVer!= m_TempSerVer && */m_CurSerVer == nCurSerIdx)
	//{//如果是跨服的 是同一个服务器 就存档
	//	pRoleData->BaseInfo.cUseRevive = 0; 
	//	pRoleData->BaseInfo.iRoleParm6 = m_TempSerVer;
	//	pRoleData->BaseInfo.ientergameid = m_sExchangePos.m_dwMapID;
	//	pRoleData->BaseInfo.ientergamex  = m_sExchangePos.m_nX;
	//	pRoleData->BaseInfo.ientergamey  = m_sExchangePos.m_nY;
	//	pRoleData->BaseInfo.cFightMode   = (BYTE)pNpc->m_FightMode;
	//}
	//else
	pRoleData->BaseInfo.iRoleParm6 = 0;

	//PK相关
	pRoleData->BaseInfo.cPkStatus = (BYTE)m_cPK.GetNormalPKState();
	pRoleData->BaseInfo.ipkvalue = m_cPK.GetPKValue();
//声望
	pRoleData->BaseInfo.ireputevalue = m_cRepute.GetReputeValue();
//福源
	pRoleData->BaseInfo.ifuyuanvalue = m_cFuYuan.GetFuYuanValue();
//转生
	pRoleData->BaseInfo.irebornvalue = m_cReBorn.GetReBornValue();  
//会员
	pRoleData->BaseInfo.isvip = m_cVip.GetRoleVipValue(); 

	pRoleData->dwFSkillOffset = (BYTE * )pRoleData->pBuffer - (BYTE *)pRoleBuffer; //下一个存档战斗技能数据的开始偏移

	//printf("存档:保存=玩家基本数据列表1=信息成功\n");
	return 1;	
}
//保存装备清单
int	KPlayer::SavePlayerItemList(BYTE * pRoleBuffer, DWORD dwBufferLen)
{
	_ASSERT(pRoleBuffer);

	TRoleData * pRoleData = (TRoleData*)pRoleBuffer;
	//物品得开始偏移
	TDBItemData * pItemData = (TDBItemData*)((BYTE*)pRoleData + pRoleData->dwItemOffset);

	//int nDesItemCount = ((TRoleData *)pRoleData)->nItemCount;	 //需要存档的原来的数量

	int	nItemCount = 0;
//	char szSection[100]={0};
//	char szKey[100];

	int nIdx = 0;
	while(1) //永远循环
	{
		nIdx = m_ItemList.m_UseIdxItemList.GetNext(nIdx);

		if (nIdx <= 0)
			break;

		int nItemIndex = m_ItemList.m_Items[nIdx].nIdx;

		// one record per item goes into the caller's blob, so the run has to stop
		// before a record that does not fit: dwBufferLen is what the caller handed
		// out as sizeof(m_GaemSaveBuffer) (KPlayer.cpp:931) and its sibling
		// SavePlayerFriendList already bounds its own run with it (:1765)
		if ((size_t)((BYTE *)pItemData - (BYTE *)pRoleBuffer) + sizeof(TDBItemData) > dwBufferLen)
			break;

		//sprintf(szSection, "%s%d", SECTION_ITEM, nItemCount + 1); //item+数字 
		//玩家基本信息*****************************************************************8	
		pItemData->iequipclasscode =  Item[nItemIndex].m_CommonAttrib.nItemGenre;
		pItemData->iequipcode      =  0;//Item[nItemIndex].GetID();
		pItemData->idetailtype     =  Item[nItemIndex].m_CommonAttrib.nDetailType;
		pItemData->iparticulartype =  Item[nItemIndex].m_CommonAttrib.nParticularType;
		pItemData->ilevel          =  Item[nItemIndex].m_CommonAttrib.nLevel;
		pItemData->iseries         =  Item[nItemIndex].m_CommonAttrib.nSeries;

		pItemData->ilocal          =  m_ItemList.m_Items[nIdx].nPlace;
		pItemData->ix              =  m_ItemList.m_Items[nIdx].nX;
		pItemData->iy              =  m_ItemList.m_Items[nIdx].nY;

		pItemData->iequipversion   = Item[nItemIndex].GetItemParam()->nVersion;
		pItemData->irandseed = Item[nItemIndex].GetItemParam()->uRandomSeed;
		pItemData->iparam1  =  Item[nItemIndex].GetItemParam()->nGeneratorLevel[0];//蓝装属性
		pItemData->iparam2  =  Item[nItemIndex].GetItemParam()->nGeneratorLevel[1];
		pItemData->iparam3  =  Item[nItemIndex].GetItemParam()->nGeneratorLevel[2];
		pItemData->iparam4  =  Item[nItemIndex].GetItemParam()->nGeneratorLevel[3];
		pItemData->iparam5  =  Item[nItemIndex].GetItemParam()->nGeneratorLevel[4];
		pItemData->iparam6  =  Item[nItemIndex].GetItemParam()->nGeneratorLevel[5];
		pItemData->iparamr1 =  Item[nItemIndex].GetItemParam()->nRGeneratorLevel[0];//熔炼属性
		pItemData->iparamr2 =  Item[nItemIndex].GetItemParam()->nRGeneratorLevel[1];
		pItemData->iparamr3 =  Item[nItemIndex].GetItemParam()->nRGeneratorLevel[2];
		pItemData->iparamr4 =  Item[nItemIndex].GetItemParam()->nRGeneratorLevel[3];
		pItemData->iparamr5 =  Item[nItemIndex].GetItemParam()->nRGeneratorLevel[4];
		pItemData->iparamr6 =  Item[nItemIndex].GetItemParam()->nRGeneratorLevel[5];
		pItemData->iparamj1 =  Item[nItemIndex].GetItemParam()->nJGeneratorLevel[0];//基本属性
		pItemData->iparamj2 =  Item[nItemIndex].GetItemParam()->nJGeneratorLevel[1];
		pItemData->iparamj3 =  Item[nItemIndex].GetItemParam()->nJGeneratorLevel[2];
		pItemData->iparamj4 =  Item[nItemIndex].GetItemParam()->nJGeneratorLevel[3];
		pItemData->iparamj5 =  Item[nItemIndex].GetItemParam()->nJGeneratorLevel[4];
		pItemData->iparamj6 =  Item[nItemIndex].GetItemParam()->nJGeneratorLevel[5];
        pItemData->iparamj7 =  Item[nItemIndex].GetItemParam()->nJGeneratorLevel[6];
		pItemData->iparamb1 =  Item[nItemIndex].GetUseMapIdx();//专用地图//Item[nItemIndex].GetItemParam()->nBGeneratorLevel[0];//宝石属性
		pItemData->iparamb2 =  Item[nItemIndex].GetItemResIdx();//改变的装备外观Item[nItemIndex].GetItemParam()->nBGeneratorLevel[1];
		pItemData->iparamb3 =  Item[nItemIndex].GetChiBangRes();//存档翅膀Item[nItemIndex].GetItemParam()->nBGeneratorLevel[2];
		pItemData->iparamb4 =  Item[nItemIndex].GetUseKind();   //使用的类型
		//pItemData->iparamb5 =  0;//Item[nItemIndex].GetItemParam()->nBGeneratorLevel[4];
		pItemData->iparamb5 = m_ItemList.FindHotKey(nItemIndex);
		pItemData->iparamb6 =  0;//Item[nItemIndex].GetItemParam()->nBGeneratorLevel[5];
		pItemData->ilucky   =  Item[nItemIndex].GetItemParam()->nLuck; //保存幸运值
		pItemData->idurability  = Item[nItemIndex].GetDurability();
		pItemData->istacknum    = Item[nItemIndex].GetStackNum();
		pItemData->iidentify    = 0;
		pItemData->igoldid      = Item[nItemIndex].GetGoldId();
		pItemData->isplatina    = Item[nItemIndex].GetIsPlatima();
		pItemData->irongNum     = Item[nItemIndex].GetRongNum(); //可熔炼数量
		pItemData->iwengangPin  = Item[nItemIndex].GetWengangPin();
		pItemData->ibinfujiazhi = Item[nItemIndex].GetBinfujiazhi();
		pItemData->iRongpiont   = Item[nItemIndex].IsRong();

		pItemData->ienchance    = Item[nItemIndex].GetEnChance();
		pItemData->ipoint       = Item[nItemIndex].GetPoint();   
		pItemData->iswhere      = Item[nItemIndex].GetIsWhere(); 
		pItemData->iyear        = Item[nItemIndex].GetTime()->bYear;
		pItemData->imonth       = Item[nItemIndex].GetTime()->bMonth;
		pItemData->iday         = Item[nItemIndex].GetTime()->bDay;
		pItemData->ihour        = Item[nItemIndex].GetTime()->bHour;
		pItemData->imin         = Item[nItemIndex].GetTime()->bMin;
		pItemData->itradeprice  = Item[nItemIndex].GetTradePrice();
        pItemData->iIsBang      = Item[nItemIndex].GetIsBang();        //是否绑定
		pItemData->ilockstate	= Item[nItemIndex].GetLockItem()->nState;
		pItemData->ilocktime	= Item[nItemIndex].GetLockItem()->dwLockTime;
		pItemData->iIsKuaiJie   = Item[nItemIndex].GetIsKuaiJie();     //是否快捷栏
		pItemData->iIsMagic     = Item[nItemIndex].GetIsMagic();       //是否魔法属性
		pItemData->iSkillType   = Item[nItemIndex].GetSkillType();
		snprintf(pItemData->iWonName, sizeof(pItemData->iWonName), "%s", Item[nItemIndex].GetWonName()); //Item[nItemIndex].GetWonName()
		//*****************************************************************************8
		//++pItemData;
		//++nItemCount;
		if  (_gameGlobalSet.gameItemInfo.nshowinfo)
		{
			char nTmepInfo[256];
			snprintf(nTmepInfo, sizeof(nTmepInfo),"--[%s]存档[%d]:%s,物品|:[%d,%d,%d]位置:[%d,%d,%d]--",m_PlayerName,nItemCount,Item[nItemIndex].GetName(),pItemData->iequipclasscode,pItemData->idetailtype,pItemData->iparticulartype,pItemData->ilocal,pItemData->ix,pItemData->iy);
			g_LogFile.gTraceLogFile(nTmepInfo,strlen(nTmepInfo),"SavePlayerItemList");
		}

		pItemData++;
		nItemCount++;
	}

	pRoleData->nItemCount = nItemCount;  //存档

	pRoleData->dwFriendOffset = (BYTE *)pItemData - (BYTE *)pRoleBuffer;		 //下个偏移是  朋友列表
	pRoleData->dwDataLen = (BYTE*)pItemData - (BYTE*)pRoleBuffer;                //全部数据的总长度
	//printf("存档:保存=玩家基本物品装备5=信息成功\n");
	return 1;
}

int	KPlayer::SavePlayerFightSkillList(BYTE * pRoleBuffer)
{
	_ASSERT(pRoleBuffer);
	if (m_nIndex <= 0) 
		return FALSE;
	TRoleData * pRoleData = (TRoleData *)pRoleBuffer; //角色基本数据
	TDBSkillData * pSkillData = (TDBSkillData *) (pRoleBuffer + pRoleData->dwFSkillOffset); //技能数据
	int nCount = Npc[m_nIndex].m_SkillList.UpdateDBSkillList((BYTE *)pSkillData);
	if (nCount > 0)
	{
		pRoleData->nFightSkillCount = nCount;
		pRoleData->dwLSkillOffset = (BYTE*)pSkillData - pRoleBuffer + sizeof(TDBSkillData) * nCount;
	}
	else
	{
		pRoleData->nFightSkillCount  = 0;
		pRoleData->dwLSkillOffset = (BYTE*)pSkillData - pRoleBuffer;  //下个偏移是 生活技能
	}
	//printf("存档:保存=玩家战斗技能数据2=信息成功\n");
	return 1;
}

int	KPlayer::SavePlayerLifeSkilllList(BYTE * pRoleBuffer)
{
	_ASSERT(pRoleBuffer);
	TRoleData * pRoleData = (TRoleData *)pRoleBuffer;
	TDBSkillData * pSkillData = (TDBSkillData *) (pRoleBuffer + pRoleData->dwLSkillOffset);
	pRoleData->nLiveSkillCount  = 0;
	pRoleData->dwTaskOffset = (BYTE*)pSkillData - pRoleBuffer;	//下个偏移是 任务
	//printf("存档:保存=玩家生活技能数据3=信息成功\n");
	return 1;
}

int	KPlayer::SavePlayerFriendList(BYTE * pRoleBuffer,DWORD dwBufferLen,BOOL nIsShow)
{
	_ASSERT(pRoleBuffer);

	int		nFriendNo;
	//char	szBuffer[32];
	CChatFriend	*pFriend;
	TRoleData * pRoleData = (TRoleData *)pRoleBuffer;

	nFriendNo = m_cChat.m_cFriendTeam[0].m_nFriendNo;
	char * pCurFriend = (char *)((BYTE *)pRoleData + pRoleData->dwFriendOffset);
	int nTotalFriendNo  = 0;
	// r32: dwFriendOffset is read out of the stored record, so this run can
	// start anywhere the file claims - the block's end is the only fact that
	// says whether that start and the names that follow it are inside it
	DWORD dwUsed = (DWORD)(pCurFriend - (char *)pRoleBuffer);
	if (dwUsed >= dwBufferLen)
		return -1;
	DWORD dwRoom = dwBufferLen - dwUsed;
	pFriend = (CChatFriend*)m_cChat.m_cFriendTeam[0].m_cEveryOne.GetHead();
	while (pFriend)
	{
		DWORD dwLen = (DWORD)strlen(pFriend->m_szName) + 1;	//the name and its terminator
		if (dwLen >= dwRoom)
			break;	// r32: nFriendCount stays the number of names actually written
		snprintf(pCurFriend, sizeof(pFriend->m_szName), "%s", pFriend->m_szName);
		pCurFriend += dwLen;
		dwRoom -= dwLen;
		nTotalFriendNo++;
		pFriend = (CChatFriend*)pFriend->GetNext();
	}
//	strcpy(pRoleData->BaseInfo.szName, m_PlayerName);
	pRoleData->nFriendCount = nTotalFriendNo;
	///_ASSERT(nTotalFriendNo == nFriendNo);
	pRoleData->dwDataLen = (BYTE*)pCurFriend - (BYTE*)pRoleBuffer;	   //数据的总字节大小数
	//if (nIsShow)
	//   printf("---------[生成存档包6]:(%s)成功-------- \n",pRoleData->BaseInfo.szName);
	//printf("----------------------------------------------\n");
	return 1;
}

//保存任务到数据库
int	KPlayer::SavePlayerTaskList(BYTE * pRoleBuffer)
{
	_ASSERT(pRoleBuffer);

//	if (m_nIndex <= 0) return -1;
	//KNpc * pNpc = &Npc[m_nIndex];

	TRoleData * pRoleData = (TRoleData *) pRoleBuffer;
	int nTaskCount = 0,nTaskId = 0,nTaskDegee = 0;
//	char szTaskIDKey[100];
//	char szTaskValueKey[100];

	KIndexNode * pNode = (KIndexNode*)m_PlayerStationList.GetHead();
	int n = 0;
	//memset(&m_cTask.nSave[TASKVALUE_SAVESTATION_BEGIN], 0, (TASKVALUE_MAXSTATION_COUNT / 2) * sizeof(int));
	//memset(&m_cTask.nSave[TASKVALUE_SAVEWAYPOINT_BEGIN], 0, TASKVALUE_MAXWAYPOINT_COUNT * sizeof(int));
	  m_cTask.SetSaveVal(TASKVALUE_SAVESTATION_BEGIN,0);//
	  m_cTask.SetSaveVal(TASKVALUE_SAVEWAYPOINT_BEGIN,0);

	while(pNode)
	{
		if (n >= TASKVALUE_MAXSTATION_COUNT)  // 城市返回点
			break;
		DWORD dwValue = m_cTask.GetSaveVal(TASKVALUE_SAVESTATION_BEGIN + n/2);//m_cTask.nSave[TASKVALUE_SAVESTATION_BEGIN + n/2];//210+16=226
		DWORD OrValue = pNode->m_nIndex ;
		OrValue = OrValue << ((n % 2) * 16);
		dwValue = dwValue | OrValue;
		//m_cTask.nSave[TASKVALUE_SAVESTATION_BEGIN + n/2] = dwValue; //赋值
		m_cTask.SetSaveVal(TASKVALUE_SAVESTATION_BEGIN + n/2,dwValue);
		n++;
		pNode = (KIndexNode*)pNode->GetNext();
	}
	
	n = 0;
	pNode = (KIndexNode*) m_PlayerWayPointList.GetHead();
	while(pNode)
	{
		if (n >= TASKVALUE_MAXWAYPOINT_COUNT) 
			break;  //返回点？
		if (pNode->m_nIndex == 0) continue;
		//m_cTask.nSave[TASKVALUE_SAVEWAYPOINT_BEGIN + n] = pNode->m_nIndex;  //赋值 设置任务号 201--203
		m_cTask.SetSaveVal(TASKVALUE_SAVEWAYPOINT_BEGIN + n,pNode->m_nIndex);
		n++;
		pNode = (KIndexNode*)pNode->GetNext();
	}
	
	g_TimerTask.SaveTask(this);
//玩家在游戏当中的任务保存
	TDBTaskData * pTaskData = (TDBTaskData *)(pRoleBuffer + pRoleData->dwTaskOffset);
	//printf("**********************************\n");	
	for (int i = 0; i < MAX_TASK; i++)
	{//一个不小的延时
		//if (m_cTask.nSave[i]>0)
        if  (!pTaskData)
				break;

		int ver=m_cTask.GetSaveVal(i);
		if (ver)
		{
		  pTaskData->m_nTaskId = i;  //任务号
		  pTaskData->m_nTaskValue =ver;  //值
		  nTaskCount++;
		  pTaskData++;
		}
	}
	//printf("**********************************\n");	
	pRoleData->nTaskCount = nTaskCount;  //任务数量
	pRoleData->dwItemOffset = (BYTE *)pTaskData - pRoleBuffer;	//下个偏移是 物品
	//printf("存档:保存=(%s)任务清单数据4=信息成功\n",pRoleData->BaseInfo.szName);//pRoleData->BaseInfo.szName
	return 1;
}

#endif



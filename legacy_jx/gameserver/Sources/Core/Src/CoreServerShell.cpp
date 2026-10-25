/*****************************************************************************************
//	外界访问Core server 接口方法
//	Copyright : Kingsoft 2002
//	Author	:   Wooy (Wu yue)
//	CreateTime:	2002-12-20
------------------------------------------------------------------------------------------
*****************************************************************************************/
#include "KCore.h"
#include "CoreServerShell.h"
#include "KThread.h"
#include "KPlayer.h"
#include "KNpc.h"
#include "KItemList.h"
#include "KSubWorldSet.h"
#include "KProtocolProcess.h"
#include "KNewProtocolProcess.h"
#include "KPlayerSet.h"
#include "KSkills.h"
#include "KLadder.h"
#include "KPlayerTradeJournal.h"

#include "KProtocolDef.h"
#include "KProtocol.h"
#include "KRelayProtocol.h"
#include "KTongProtocol.h"


#ifdef _STANDALONE
#include "KLadder.cpp"
#endif

//#include "KNetServer.h"
//#include "../MultiServer/Heaven/Interface/iServer.h"
#ifdef _STANDALONE
#include "IClient.h"
#else
#include "../../../lib/S3DBInterface.h"
#include "../../../Headers/IClient.h"
#include "../../../Headers/IClient.h"
#include "../../../Headers/KGmProtocol.h"
#endif

#include "LuaFuns.h"
#include "KSortScript.h"
#include "KSubWorld.h"
#include "nb30.h"

#include "malloc.h"

extern "C"
{
   #include "miracl.h"
   #include "mirdef.h"
}
#include <Windows.h>
#include <iostream>	//#include <iostream.h>

#include <ppl.h>

//游戏状态
/*enum PLAYER_GAME_STATUS
{
	enumPlayerBegin = 0,
	enumPlayerSyncEnd,
	enumPlayerPlaying,
	enumPlayerExchangingServer,
};

enum NetStatus
{
	enumNetUnconnect = 0,
	enumNetConnected,
};*/

enum PLAYER_EXCHANGE_STATUS
{
	enumExchangeBegin = 0,
	enumExchangeSearchingWay,
	enumExchangeWaitForGameSvrRespone,
	enumExchangeCleaning,
};



class CoreServerShell : public iCoreServerShell
{
public:
	void  mandatoryShutDownClient(int nIndex);//强制性关闭客户端,用于卡号处理
	UINT  GetLoopRate();
	void  GetGuid(int nIndex, void* pGuid);
	UINT GetExchangeMap(int nIndex);
	bool IsPlayerLoginTimeOut(int nIndex);
	void RemovePlayerLoginTimeOut(int nIndex);
	bool IsPlayerExchangingServer(int nIndex);
	int  getClientIsMobile(int nIndex);
	int  serverKickOutGame(int nIndex);
	void ProcessClientMessage(int nIndex, const char* pChar, int nSize);
	void ProcessNewClientMessage(IClient*, DWORD, DWORD, int nIndex, const char* pChar, int nSize);
	void SendNetMsgToTransfer(IClient* pClient);
//	int  GetRegisterMa();  //检测获取注册码
//	int  getMAC(char *mac);
	void SendNetMsgToChat(IClient* pClient);
	void SendNetMsgToTong(IClient* pClient);
	void ProcessBroadcastMessage(const char* pChar, int nSize);
	void ProcessExecuteMessage(const char* pChar, int nSize);
	void ClientDisconnect(int nIndex,BOOL nIsOnline=FALSE);
	void CheckSerMaps(int nIndex,BOOL nIsShow=FALSE,BOOL isOnline=TRUE);
	void CheckItemAddtoEquipment(int nIndex);
	bool SavePlayerData(int nIndex,bool isLeave);
	void RemoveQuitingPlayer(int nIndex);
	void* SavePlayerDataAtOnce(int nIndex);
	bool IsCharacterQuiting(int nIndex);
	bool IsShutdownClient(int nIndex);
	bool IsShutdownClientNoserver(int nIndex);
	bool IsShutdownClientOutserver(int nIndex);
	int  GetGoldMissidVal(int nGoldMid);
	int  GetClientNetConnectIdx(int nIndex);
	int  IsCharacterLiXian(int nIndex);
	void SetCharacterLiXian(int nIndex,int i);
 	int  SetAttackInfo(const char * pMsg,int nPlayerIdx);
	int  SetAttackWarCityInfo(const char * pMsg,int nPlayerIdx);
	void SetAttackCamp(char * nTongName,int nType);
	void SetAttackMsg(void *pdata,int nType);
	bool CheckProtocolSize(const char* pChar, int nSize);
	bool PlayerDbLoading(int nPlayerIndex, int bSyncEnd, int& nStep, unsigned int& nParam);
	int  AttachPlayer(const unsigned long lnID, GUID* pGuid,int model=0);
	void GetPlayerIndexByGuid(GUID* pGuid, int* pnIndex, int* plnID);
	void AddPlayerToWorld(int nIndex);
	void* PreparePlayerForExchange(int nIndex);
	void PreparePlayerForLoginFailed(int nIndex);
	void RemovePlayerForExchange(int nIndex);
	void RecoverPlayerExchange(int nIndex);

	int  AddCharacter(DWORD nExtPoint, LONG nChangeExtPoint, void* pBuffer, GUID* pGuid,DWORD nLeftTime=0,int nVipType=0,LONG nGameTime=0,BOOL nKeepForBt=TRUE,bool isExTrans=false,unsigned int nTempEnTerMapIndx=0,unsigned int  nTempKeyId=0,int m_IsFirstLogin=1);
//	int	 AddTempTaskValue(int nIndex, const char* pData);
	//设置登录状态信息
/*	int	 SetPlayeGameStates(int nPlayerIdx,int nStats);
	int	 SetPlayeNetStates(int nPlayerIdx,int nStats);
	int	 SetPlayePingStratTime(int nPlayerIdx,int npTime);
	int	 SetPlayePingReplyTime(int nPlayerIdx,int npTime);
	int	 SetPlayeExchangeStatus(int nPlayerIdx,int nStats);
	int	 GetPlayeNetStates(int nPlayerIdx);
*/
	void doGoldthreadScript(const void *pData);
	void doTaskScript(const void *pData);
	void gm_LocalScript(char *msg);
	void gm_addNpc(const void *pData);
	void SendSystemCahtInfo(char *msg);
	//向游戏发送操作
	int	 OperationRequest(unsigned int uOper, UINT_PTR uParam, int nParam,int inParam=0);
    int OperationRequestInfo(unsigned int uOper, UINT_PTR uParam, int nParam,int inParam=0,void *pdata=NULL,char *strdata=NULL);
	//获取连接状况
	int	 GetConnectInfo(KCoreConnectInfo* pInfo,int nIndex=0);

	int  GetNpcInfoByNamw(KNpcInfo* pInfo,char *strName);

	int  gm_OperationRequest(const void *nData);

	int  item_OperationRequest(const void *nData);

	int  sendItemData(const void *nData);
	//BOOL ValidPingTime(int nIndex);
	//从游戏世界获取数据
	INT_PTR	 GetGameData(unsigned int uDataId, UINT_PTR uParam, int nParam);
	int GetGameData_New(unsigned int uDataId, UINT_PTR uParam, int nParam,char *strdata=NULL);
    int	 GetGameDataInfo(unsigned int uDataId, unsigned long uParam, int nParam,char *strdata=NULL,void *pdata=NULL);
	void ExecuteScript(RELAY_REMOTE* pRemote);
	INT  GetExpData(int nIndex,int nType);
	//服务器端日常活动，core如果要寿终正寝则返回0，否则返回非0值
	int  Breathe();//服务器端
	//释放接口对象
	void Release();
	void SetSaveStatus(int nIndex, UINT uStatus);
	void SetSaveStatus(char *strName, UINT uStatus);
	void SetChangeRoleStatus(int nIndex, int uStatus,char *nRoleName);
	UINT GetSaveStatus(int nIndex);

	BOOL GroupChat(IClient* pClient, DWORD FromIP, unsigned long FromRelayID, DWORD channid, BYTE tgtcls, DWORD tgtid, const void* pData, size_t size, char* szSender);
	void SetLadder(void* pData, size_t uSize);
	BOOL PayForSpeech(int nIndex, int nType);

	void ExeCoreTask();
	void CheckItemTime();
	void SynCoreItemData();
	void  SavePaiShopData();
	void ReleaseDbData();
	void ExeCoreNpcState();

	void sendToClient_globalNews(const char *pData);
	void sendToClient_localNews(const char *pData);
	void sendToClient_globalCountNews(const char *pData);
	
	int  OnSetTran(LPVOID pServer=NULL);
	int  OnSetChat(LPVOID pServer=NULL);
	int	 OnSetTong(LPVOID pServer=NULL);
	int	 OnSetData(LPVOID pServer=NULL);
	int  OnSetGateWay(LPVOID pServer=NULL);

	void SetTongData(int nPlayerIdx, void* data);
	void SyncBlackList(int nPlayerIdx, std::list<string> &blackList);
	BOOL CheckBlackList(int nPlayerIdx, char* szName);
	void OnTradePairSaveResult(unsigned __int64 uTxId, int nResultCode, const char* szRoleA, const char* szRoleB);
private:
	int	 OnLunch(LPVOID pServer);  //启动服务器
	/*int  OnSetTran(LPVOID pServer);
	int  OnSetChat(LPVOID pServer);
	int	 OnSetTong(LPVOID pServer);
	int	 OnSetData(LPVOID pServer);
	int  OnSetGateWay(LPVOID pServer);*/
	int	 OnShutdown();
};

static CoreServerShell	g_CoreServerShell;

CORE_API void g_InitCore(char *nParmName);

#ifndef _STANDALONE
extern "C" __declspec(dllexport)
#endif

/*iCoreServerShell* CoreGetServerShell()
{
	g_InitCore();
	return &g_CoreServerShell;
} */

//单机设置

iCoreServerShell* CoreGetServerShell(const char *gamePass)
{ 
	g_InitCore((char *)gamePass);
	return &g_CoreServerShell;
}

void CoreServerShell::Release()
{
	g_ReleaseCore();
}
//获取服务器的运行循环的次数
UINT CoreServerShell::GetLoopRate()
{
	return g_SubWorldSet.m_nLoopRate;
}


int CoreServerShell::sendItemData(const void *nData)
{
	if  (!nData) return false;

	VIEW_LIAN_ITEM_SYNC	 *sLView = (VIEW_LIAN_ITEM_SYNC*)nData;  //协议

	sLView->ProtocolType =  s2c_viewlianitem;
	int nIndex = PlayerSet.FindNameID(sLView->m_WonName); 
	if (nIndex>0 && nIndex<MAX_PLAYER)
	{
	   if (g_pServer)
		  g_pServer->PackDataToClient(Player[nIndex].m_nNetConnectIdx, sLView, sizeof(VIEW_LIAN_ITEM_SYNC));
	}

	return true;
}

int CoreServerShell::item_OperationRequest(const void *nData)
{
	if  (!nData) return false;
	//ExeNoBackScript(char * ScriptFileName, char * szFunName,int nKind = 0,int nParama = 0,int nParamb = 0,int nParamc = 0,char *strInfo=NULL);
	tagItemAction* pInfo = (tagItemAction *)nData;
	if  (!pInfo->nRoleName[0])  return false;

	switch (pInfo->cCmdType)
	{
	case enum_ITEM_ID_FIND://目标gs执行 再获取数据转发给源gs的角色(nGmRoleName)
		{
			//printf("---ITEM操作item_OperationRequest:%s %s %u ----\n",pInfo->nGmRoleName,pInfo->nRoleName,pInfo->nId);

			int nIndex = PlayerSet.FindNameID(pInfo->nRoleName); 
			if (nIndex>0 && nIndex<MAX_PLAYER)
			{
				if (pInfo->nGmRoleName[0])
				{
					int nSrcIndex = PlayerSet.FindNameID(pInfo->nGmRoleName);
					if (nSrcIndex>0 && nSrcIndex<MAX_PLAYER)
					{ //如果是在同一个 gs了 酒吧直接 发送了 
						if (!Player[nSrcIndex].SendLianItemInfo(nSrcIndex ,pInfo->nId))  //查看者索引 nIndex
						{   
							Player[nSrcIndex].m_ItemList.msgshow("链接:该链接失效!");
							break;
						}
					}
					else
					{//不同一个gs的 转发
						/*int	nIdx,nItemidx;
						nIdx      = Player[nIndex].m_ItemList.SearchID(pInfo->nId);
						nItemidx  = Player[nIndex].m_ItemList.SearchItemsIdx(pInfo->nId);
						if (nIdx <= 0 || nItemidx<=0)
						break;*/
						if (!Player[nIndex].SendLianItemInfo(nData)) //查看者索引 nIndex
						{   
							Player[nIndex].m_ItemList.msgshow("链接:被获取失败!");
							break;
						}

					}
				}
				
			}
		}
		break;
	default:
		break;
	}

	

	return true;
}
//agQueryMapInfo* pInfo,
int CoreServerShell::gm_OperationRequest(const void *nData)
{
	if  (!nData) return false;
	//ExeNoBackScript(char * ScriptFileName, char * szFunName,int nKind = 0,int nParama = 0,int nParamb = 0,int nParamc = 0,char *strInfo=NULL);
	tagQueryMapInfo* pInfo = (tagQueryMapInfo *)nData;

	int nIndex = PlayerSet.FindNameID(pInfo->nName);

	if (nIndex>0 && nIndex<MAX_PLAYER)
		Player[nIndex].ExeNoBackScript("\\script\\system\\gm_operation.lua","gm_operation",pInfo->nProtoKind,pInfo->nKey,pInfo->nVerX,pInfo->nVerY,pInfo->nName);

	//switch (pInfo->nProtoKind)
	//{
	//case enum_COMMAND_ID_MOVETOP_results_GM://源gs执行
	//	{//目标执行移动到发送过来的地图和坐标
	//		printf("---ＧＭ移动:%s 移动到:map:%d,x:%d,y:%d ----\n",pInfo->nName,pInfo->nKey,pInfo->nVerX,pInfo->nVerY);
	//	}
	//	break;
	//case enum_COMMAND_ID_LEVEL_GM:
	//	{
	//		printf("---ＧＭ设置等级:%s----\n",pInfo->nName);
	//	}
	//	break;
 //   case enum_COMMAND_ID_MONEY_GM:
	//	{
	//		printf("---ＧＭ设置银两:%s----\n",pInfo->nName);
	//	}
	//	break;
	//case enum_COMMAND_ID_XU_GM:
	//	{
	//		printf("---ＧＭ设置金币:%s----\n",pInfo->nName);
	//	}
	//	break;
	//case enum_COMMAND_ID_TASK_GM:
	//	{
	//		printf("---ＧＭ设置任务ID:%s----\n",pInfo->nName);
	//	}
	//	break;
 //   case enum_COMMAND_ID_FORBITCHAT_GM:
	//	{
	//		printf("---ＧＭ设置禁言:%s----\n",pInfo->nName);
	//	}
	//	break;
	//case enum_COMMAND_ID_KICKOUTROLE_GM:
	//	{//ｇｍ踢某人下线
	//	   printf("---ＧＭ开始踢掉玩家下线:%s----\n",pInfo->nName);
	//	}
	//	break;
	//default:
	//	break;
	//}
	return true;
}



int CoreServerShell::GetNpcInfoByNamw(KNpcInfo* pInfo,char *strName)
{
	if  (!strName || !pInfo) return 0;

	int nIndex = PlayerSet.FindNameID(strName);
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return 0; 

	int _NpxIndx = Player[nIndex].m_nIndex;

	if (_NpxIndx>0 && _NpxIndx<MAX_NPC)
	{
		int nMapIndx=-1,x=0,y=0;
		Npc[_NpxIndx].GetMpsPos(&x,&y,&nMapIndx);

		if  (nMapIndx>=0 && nMapIndx<MAX_SUBWORLD)
		{
		  pInfo->nMapIndx = SubWorld[nMapIndx].m_SubWorldID;
		  pInfo->nPosX =x;
		  pInfo->nPosY =y;
		}
		else
		{
			pInfo->nMapIndx = nMapIndx;
			pInfo->nPosX =x;
			pInfo->nPosY =y;
		}

		return _NpxIndx;
	}
	return 0;
}
	//获取连接状况
int	 CoreServerShell::GetConnectInfo(KCoreConnectInfo* pInfo,int nIndex)
{
	if (pInfo)
	{
		pInfo->nNumPlayer = PlayerSet.GetPlayerNumber(); 

		if (nIndex > 0 && nIndex < MAX_PLAYER)
		{
		  sprintf(pInfo->nClientIp,"127.0.0.1");//Player[nIndex].m_PlayerIpInfo);
		}
		else
		  sprintf(pInfo->nClientIp, "127.0.0.1");
	}

	return 1;
}

//GS 调用的函数 用于扩展点的更新获取等
int CoreServerShell::AddCharacter(DWORD nExtPoint, LONG nChangeExtPoint, void* pBuffer, GUID* pGuid,DWORD nLeftTime,int nVipType,LONG nGameTime,BOOL nKeepForBt,
	                              bool isExTrans,unsigned int nTempEnTerMapIndx,unsigned int nTempKeyId,int m_IsFirstLogin)
{
	int nIdx = 0;
	const TRoleData* pData = (const TRoleData*)pBuffer;

	//printf("-----AddCharacter(isExTrans:%d, nTempEnTerMapIndx:%d, nTempKeyId:%d, m_IsFirstLogin:%d\n",isExTrans, nTempEnTerMapIndx, nTempKeyId, m_IsFirstLogin);

	if (pData && pData->BaseInfo.szName[0])
	{
		if (KPlayerTradeJournal::Instance().IsPlayerLocked((char*)pData->BaseInfo.szName))
		{
			printf("[AddCharacter BLOCKED]: Player '%s' is locked by Trade Journal Recovery. Login REJECTED.\n", (char*)pData->BaseInfo.szName);
			return 0;
		}
		nIdx = 0;
		nIdx = PlayerSet.Add((char*)pData->BaseInfo.szName, pGuid,nKeepForBt);  //增加玩家链表信息，索引等

		if (nIdx <= 0 || nIdx >= MAX_PLAYER)
		{
		   printf("------AddCharacter:Name(%s)nIdx(%d)Faild------\n",(char*)pData->BaseInfo.szName,nIdx);
		   return 0;
		}

		/*strcpy(Player[nIdx].m_AccoutName, pData->BaseInfo.caccname);
		strcpy(Player[nIdx].m_PlayerName, pData->BaseInfo.szName);*/
		snprintf(Player[nIdx].m_AccoutName, sizeof(Player[nIdx].m_AccoutName),"%s", pData->BaseInfo.caccname);
		sprintf(Player[nIdx].m_PlayerName, "%s",pData->BaseInfo.szName);

		DWORD	dwLen = pData->dwDataLen;
		// the length is the record's own claim about its size, and nothing between
		// reading it and this copy tested it against the buffer it lands in: the
		// save buffer is the budget, and an over-long record stops at its edge.  How
		// much valid data follows pBuffer is the allocation's question, not this one.
		if (dwLen > sizeof(Player[nIdx].m_GaemSaveBuffer))
			dwLen = sizeof(Player[nIdx].m_GaemSaveBuffer);
		ZeroMemory(Player[nIdx].m_GaemSaveBuffer, sizeof(Player[nIdx].m_GaemSaveBuffer));

		memcpy(Player[nIdx].m_GaemSaveBuffer,(BYTE*)pBuffer,dwLen);
        //玩家档,登录产生东西调用
		Player[nIdx].m_pStatusLoadPlayerInfo = (void*)Player[nIdx].m_GaemSaveBuffer;  // 玩家数据包


		if (!nKeepForBt)
		{//如果是服务器离线的
            Player[nIdx].m_nLiXian=0;	       //解除离线
			Player[nIdx].m_nForbFlag=1;
			Player[nIdx].m_nForbiddenFlag=0;   //解除禁言
			printf("------AddCharacter: khoi phuc trang thai offlive thanh cong------\n");
		} 
		/*const char *nPinfo=	 g_pServer->GetClientInfo(Player[nIdx].m_nNetConnectIdx);
		if (nPinfo) 
			sprintf(Player[nIdx].m_PlayerIpInfo,nPinfo); */
		// 扩展点，用于活动
	    //printf("----AddCharacter:, nIdx (%d), tai khoan: %s, nhan vat: %s, nItemCount: %d, dwDataLen: %u----\n",nIdx,Player[nIdx].m_AccoutName,Player[nIdx].m_PlayerName,pData->nItemCount,pData->dwDataLen);

		//Player[nIdx].m_GameLeftTime = nGameTime;
		Player[nIdx].SetExtPoint(nExtPoint, nChangeExtPoint);
        Player[nIdx].SetLeftTineAndVipType(nLeftTime,nVipType,0);
		Player[nIdx].SetExtGameTime(nGameTime);

	    if  (isExTrans)
		{//Neu la chuyen gs qua, can set mot che do chien dau, trang thai cuoi ngua, vv 如果是转gs过来的需要设置一个 战斗模式 骑马状态等


		}
		Player[nIdx].m_IsFirstLogin = m_IsFirstLogin;
		Player[nIdx].m_TempEnTerMapIndx = nTempEnTerMapIndx;   //Ban do dang nhap tam thoi do client truyen qua  客户端传过来的临时登陆地图    
		Player[nIdx].m_TempKeyId = nTempKeyId;                 // 重生点keyiD

	   if  (nVipType ==0)
	   {//包月
		 if (nGameTime>0 )
			Player[nIdx].m_Isinvalid = FALSE;
		 else
			Player[nIdx].m_Isinvalid = TRUE;
	   }
	
		return nIdx;
	}
	return 0;
}
//没返回 导致卡号
bool CoreServerShell::PlayerDbLoading(int nPlayerIndex, int bSyncEnd, int& nStep, unsigned int& nParam)
{    
	TRoleData* pData = (TRoleData *)Player[nPlayerIndex].m_pStatusLoadPlayerInfo;
	
	if (bSyncEnd)
	{//如果是已经同步完毕的 就直接返回
		Player[nPlayerIndex].m_pStatusLoadPlayerInfo = NULL;
		nStep = 0;//STEP_SYNC_END;
		nParam = 0;
		return true;
	}
	else if (pData)	
	{
		return Player[nPlayerIndex].LoadDBPlayerInfo((BYTE *)pData, nStep, nParam);
	}
	return false;
}
//开始登陆游戏 增加玩家到游戏地图中
void CoreServerShell::AddPlayerToWorld(int nIndex)
{
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (KPlayerTradeJournal::Instance().IsPlayerLocked(Player[nIndex].m_PlayerName))
	{
		printf("[AddPlayerToWorld BLOCKED]: Player '%s' is locked by Trade Journal Recovery. Rejected.\n", Player[nIndex].m_PlayerName);
		return;
	}
	Player[nIndex].LaunchPlayer();

}

void CoreServerShell::ProcessClientMessage(int nIndex, const char* pChar, int nSize)
{
	PlayerSet.ProcessClientMessage(nIndex, pChar, nSize);
}

void CoreServerShell::ProcessNewClientMessage(IClient* pTransfer,
									   DWORD dwFromIP, DWORD dwFromRelayID,
									   int nPlayerIndex,
									   const char* pChar, int nSize)
{
	g_NewProtocolProcess.ProcessNetMsg(pTransfer, dwFromIP, dwFromRelayID,
										nPlayerIndex, (BYTE*)pChar, nSize);

}

void CoreServerShell::SendNetMsgToTransfer(IClient* pClient)
{
	g_NewProtocolProcess.SendNetMsgToTransfer(pClient);
}



void CoreServerShell::SendNetMsgToChat(IClient* pClient)
{
	g_NewProtocolProcess.SendNetMsgToChat(pClient);
}

void CoreServerShell::SendNetMsgToTong(IClient* pClient)
{
	g_NewProtocolProcess.SendNetMsgToTong(pClient);
}

void CoreServerShell::ProcessBroadcastMessage(const char* pChar, int nSize)
{
	g_NewProtocolProcess.BroadcastLocalServer(pChar, nSize);
}
//执行本地脚本
void CoreServerShell::ProcessExecuteMessage(const char* pChar, int nSize)
{
	g_NewProtocolProcess.ExecuteLocalServer(pChar, nSize);
}
//必须在存档前处理
void  CoreServerShell::CheckItemAddtoEquipment(int nIndex)
{
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)  return;
	//处理 给与界面和交易界面的存档
	if (Player[nIndex].CheckTrading())
	{
		Player[nIndex].CancelTradeSafe();
	}
	//开始检测给与物品 
	Player[nIndex].CheckItemAddtoEquipment(room_give);
}

//网络断开时调用---存档
bool CoreServerShell::SavePlayerData(int nIndex,bool isLeave)
{
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return FALSE;
	//处理 给与界面和交易界面的存档
	if (Player[nIndex].CheckTrading())
	{
		Player[nIndex].CancelTradeSafe();
	}
	//开始检测给与物品 
	Player[nIndex].CheckItemAddtoEquipment(room_give);

	if (Player[nIndex].Save())	                         //生成数据包
	{
		if (g_pDataClient)
		{//立即存档了。
			Player[nIndex].SavePlayerData(isLeave);
		}
	} 
	return TRUE;
}


void CoreServerShell::CheckSerMaps(int nIndex,BOOL nIsShow,BOOL isOnline)
{
		PlayerSet.CheckMaps(nIndex,nIsShow,isOnline);
}
//强制性关闭客户端 用于卡号处理
void CoreServerShell::mandatoryShutDownClient(int nIndex)
{
	//PlayerSet.mandatoryShutDownClient(nIndex); 
	PlayerSet.PrepareRemove(nIndex,TRUE);  //直接断开网络
}

//离开游戏时调用---客户端端开链接
void CoreServerShell::ClientDisconnect(int nIndex,BOOL nIsOnline)
{
//	PlayerSet.Remove(nClient);
	PlayerSet.__PrepareRemove(nIndex); 
}
//离开游戏时调用
void CoreServerShell::RemoveQuitingPlayer(int nIndex)
{
	if (!Player || nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;

	if (Player[nIndex].IsWaitingRemove())
	{
		PlayerSet.RemoveQuiting(nIndex);
	}
}


INT CoreServerShell::GetExpData(int nIndex,int nType)
{

	if (nIndex<=0 || nIndex >= MAX_PLAYER)
	{
		return 0;
	}

   if (nType==0)
   {
	   return Player[nIndex].GetExtPointChanged();

   }
   else
   {
	   return Player[nIndex].GetExtTimeChanged();
   }
   return 0;
}

int	CoreServerShell::GetGameDataInfo(unsigned int uDataId, unsigned long uParam, int nParam,char *strdata,void *pdata)
  {
	  int nRet = 0;
	  switch(uDataId)
	  {
	  case SGDI_TONG_CHECK_SET_POWER:
	  {
		  if (pdata)
		  {
			  STONG_SERVER_TO_CORE_CHECK_SET* pCheck = (STONG_SERVER_TO_CORE_CHECK_SET*)pdata;
			  if (!pCheck)
				  break;

			  if (nParam <= 0 || nParam >= MAX_PLAYER)
				  break;

			  nRet = Player[nParam].m_cTong.CheckSetCondition(pCheck);
		  }
	  }
	  break;
	  case SGDI_CHARACTER_ACCOUNT:
		  if (strdata)
			  nRet = PlayerSet.GetPlayerAccount(nParam,strdata);
		  break;
	  case SGDI_CHARACTER_NAME:
		  if (strdata)
			  nRet = PlayerSet.GetPlayerName(nParam, strdata);
		  break;
	  case SGDI_CHARACTER_SEX:
	  {
		  if (strdata)
		  {
			  int idx = NpcSet.SearchNameID(g_FileName2Id(strdata));
			  if (idx <= 0 || idx >= MAX_NPC)
				  break;

			  nRet = Npc[idx].m_nSex;
			  break;
		  }
		  else
		  {
			  if (nParam <= 0 || nParam >= MAX_PLAYER)
				  break;

			  nRet = Npc[Player[nParam].m_nIndex].m_nSex;
		  }
	  }
	  break;
		  // 判断加入帮会条件是否合适
		  // uParam : 传入得 char point ，用于接收帮会名称
		  // nParam : struct STONG_SERVER_TO_CORE_CHECK_ADD_CONDITION point
	  case SGDI_TONG_CHECK_ADD_CONDITION:
		  {
			  nRet = 0;
			  STONG_SERVER_TO_CORE_CHECK_ADD_CONDITION* pAdd = (STONG_SERVER_TO_CORE_CHECK_ADD_CONDITION*)pdata;
			  if (pAdd == NULL)
				  break;

			  if (pAdd->m_nSelfIdx <= 0 || pAdd->m_nSelfIdx >= MAX_PLAYER)
				  break;

			  if (pAdd->m_nTargetIdx <= 0 || pAdd->m_nTargetIdx >= MAX_PLAYER)
				  break;

			  if (Player[pAdd->m_nSelfIdx].m_cTong.CheckAddCondition(pAdd->m_nTargetIdx))
			  {
				  pAdd->m_nLevel = Npc[Player[pAdd->m_nTargetIdx].m_nIndex].m_Level;
				  if (strdata)
					  Player[pAdd->m_nSelfIdx].m_cTong.GetTongName(strdata);
				  nRet = 1;
			  }
		  }
		  break;
	  case SGDI_TONG_APPLY_CREATE:
		  if (pdata)
		  {
			  STONG_SERVER_TO_CORE_APPLY_CREATE	*pApply = (STONG_SERVER_TO_CORE_APPLY_CREATE*)pdata;

			  if (NULL == pApply) 
				  break;

			  int		nPlayerIdx;
			  int		nCamp;
			  char	szTongName[NAME_LENGHT];

			  szTongName[sizeof(szTongName) - 1] = 0;
			  memcpy(szTongName, pApply->m_szTongName, sizeof(szTongName));
			  nPlayerIdx = pApply->m_nPlayerIdx;
			  nCamp = pApply->m_nCamp;

			  if (nPlayerIdx <= 0 || nPlayerIdx >= MAX_PLAYER)
			  {
				  nRet = TRUE; 
				  printf(">>>>>>>>>>SGDI_TONG_APPLY_CREATE 服务器创建帮会(%d,%d,%s)消息 ERROR<<<<<<<<<<\n",nPlayerIdx,nCamp,szTongName);
				  break;
			  }
			  pApply->m_nLevel = Npc[Player[nPlayerIdx].m_nIndex].m_Level;
			  nRet = Player[nPlayerIdx].m_cTong.CheckCreateCondition(nCamp, szTongName);
		  }
		  break;
		  // 获得帮会信息
		  // uParam : 传入的 STONG_SERVER_TO_CORE_GET_INFO point
	  case SGDI_TONG_GET_INFO:
		  {
			  STONG_SERVER_TO_CORE_GET_INFO	*pInfo = (STONG_SERVER_TO_CORE_GET_INFO*)pdata;
			  if (NULL==pInfo) break;
			  switch (pInfo->m_nInfoID)
			  {
			  case enumTONG_APPLY_INFO_ID_SELF:
				  {
					  if (pInfo->m_nSelfIdx <= 0 || pInfo->m_nSelfIdx >= MAX_PLAYER)
						  break;
					  Player[pInfo->m_nSelfIdx].m_cTong.SendSelfInfo();
				  }
				  break;
			  case enumTONG_APPLY_INFO_ID_MASTER:
				  break;
			  case enumTONG_APPLY_INFO_ID_DIRECTOR:
				  break;
			  case enumTONG_APPLY_INFO_ID_SEARCH:
			  case  enumTONG_APPLY_INFO_ID_POWER:
			  case enumTONG_APPLY_INFO_ID_MANAGER:
				  {
					  nRet = 0;
					  if (pInfo->m_nSelfIdx <= 0 || pInfo->m_nSelfIdx >= MAX_PLAYER)
						  break;
					  if (Player[pInfo->m_nSelfIdx].m_cTong.CanGetManagerInfo((DWORD)pInfo->m_nParam1))
						  nRet = 1;
				  }
				  break;
			  case enumTONG_APPLY_INFO_ID_MEMBER:
				  {
					  nRet = 0;
					  if (pInfo->m_nSelfIdx <= 0 || pInfo->m_nSelfIdx >= MAX_PLAYER)
						  break;
					  if (Player[pInfo->m_nSelfIdx].m_cTong.CanGetMemberInfo((DWORD)pInfo->m_nParam1))
						  nRet = 1;
				  }
				  break;
			  case enumTONG_APPLY_INFO_ID_ONE:
				  break;

			  case enumTONG_APPLY_INFO_ID_TONG_HEAD:
				  {
					  nRet = 0;
					  if (pInfo->m_nSelfIdx <= 0 || pInfo->m_nSelfIdx >= MAX_PLAYER)
						  break;

					  nRet = Player[pInfo->m_nSelfIdx].m_cTong.GetTongNameID();
				  }
				  break;
			  }
		  }
		  break;
		  // 申请加入帮会
		  // uParam : struct STONG_SERVER_TO_CORE_APPLY_ADD point
	  case SGDI_TONG_APPLY_ADD:
		  if (pdata)
		  {
			  STONG_SERVER_TO_CORE_APPLY_ADD	*pAdd = (STONG_SERVER_TO_CORE_APPLY_ADD*)pdata;
			  if (NULL==pAdd) break;
			  if (pAdd->m_nPlayerIdx <= 0 || pAdd->m_nPlayerIdx >= MAX_PLAYER)
				  break;
			  //服务器端的
			  Player[pAdd->m_nPlayerIdx].m_cTong.TransferAddApply(pAdd->m_dwNpcID); //
		  }
		  break;
		  // 判断是否有任命权利
		  // uParam : 传入的 TONG_APPLY_INSTATE_COMMAND point
		  // nParam : PlayerIndex
	  case SGDI_TONG_INSTATE_POWER:
		  if (pdata)
		  {
			  nRet = 0;
			  TONG_APPLY_INSTATE_COMMAND	*pApply = (TONG_APPLY_INSTATE_COMMAND*)pdata;
			  if (NULL==pApply) break;
			  if (nParam <= 0 || nParam >= MAX_PLAYER)
				  break;
			  if (Player[nParam].m_nIndex <= 0)
				  break;
			  nRet = Player[nParam].m_cTong.CheckInstatePower(pApply);
		  }
		  break;

		  // 被任命，帮会数据变化
		  // uParam : 传入的 STONG_SERVER_TO_CORE_BE_INSTATED point
	  case SGDI_TONG_BE_INSTATED:
		  if (pdata)
		  {
			  STONG_SERVER_TO_CORE_BE_INSTATED	*pInstated = (STONG_SERVER_TO_CORE_BE_INSTATED*)pdata;
			  if (NULL==pInstated) break;
			  if (pInstated->m_nPlayerIdx <= 0 || pInstated->m_nPlayerIdx >= MAX_PLAYER)
				  break;
			  if (Player[pInstated->m_nPlayerIdx].m_nIndex <= 0)
				  break;
			  Player[pInstated->m_nPlayerIdx].m_cTong.BeInstated(pInstated);
		  }
		  break;

		  // 判断是否有踢人权利
		  // uParam : 传入的 TONG_APPLY_KICK_COMMAND point
		  // nParam : PlayerIndex
	  case SGDI_TONG_KICK_POWER:
		  if (pdata)
		  {
			  nRet = 0;
			  TONG_APPLY_KICK_COMMAND	*pKick = (TONG_APPLY_KICK_COMMAND*)pdata;
			  if (NULL==pKick) break;
			  if (nParam <= 0 || nParam >= MAX_PLAYER)
				  break;
			  if (Player[nParam].m_nIndex <= 0)
				  break;
			  nRet = Player[nParam].m_cTong.CheckKickPower(pKick);
		  }
		  break;

		  // 被踢出帮会
		  // uParam : 传入的 STONG_SERVER_TO_CORE_BE_KICKED point
	  case SGDI_TONG_BE_KICKED:
		  if (pdata)
		  {
			  STONG_SERVER_TO_CORE_BE_KICKED	*pKicked = (STONG_SERVER_TO_CORE_BE_KICKED*)pdata;
			  if (NULL==pKicked) break;
			  if (pKicked->m_nPlayerIdx <= 0 || pKicked->m_nPlayerIdx >= MAX_PLAYER)
				  break;
			  if (Player[pKicked->m_nPlayerIdx].m_nIndex <= 0)
				  break;
			  Player[pKicked->m_nPlayerIdx].m_cTong.BeKicked(pKicked);
		  }
		  break;

		  // 离开帮会
		  // uParam : 传入的 TONG_APPLY_LEAVE_COMMAND point
		  // nParam : PlayerIndex
	  case SGDI_TONG_LEAVE_POWER:
		  if (pdata)
		  {
			  nRet = 0;
			  TONG_APPLY_LEAVE_COMMAND	*pLeave = (TONG_APPLY_LEAVE_COMMAND*)pdata;
			  if (NULL==pLeave) break;
			  if (nParam <= 0 || nParam >= MAX_PLAYER)
				  break;
			  if (Player[nParam].m_nIndex <= 0)
				  break;
			  nRet = Player[nParam].m_cTong.CheckLeavePower(pLeave);
		  }
		  break;

	  case SGDI_TONG_SAVE_MONEY_POWER:
	  {
		  if (pdata)
		  {
			  nRet = 0;
			  TONG_APPLY_SAVE_COMMAND* pSave = (TONG_APPLY_SAVE_COMMAND*)pdata;
			  if (NULL == pSave) break;
			  if (nParam <= 0 || nParam >= MAX_PLAYER)
				  break;
			  if (Player[nParam].m_nIndex <= 0)
				  break;
			  nRet = Player[nParam].m_cTong.CheckSaveMoneyPower(pSave);
		  }
	  }
	  break;

	  case SGDI_TONG_MONEY_POWER:
		  if (pdata)
		  {
			  nRet = 0;
			  TONG_APPLY_SAVE_COMMAND	*pSave = (TONG_APPLY_SAVE_COMMAND*)pdata;
			  if (NULL==pSave) break;
			  if (nParam <= 0 || nParam >= MAX_PLAYER)
				  break;
			  if (Player[nParam].m_nIndex <= 0)
				  break;
			  nRet = Player[nParam].m_cTong.CheckMoneyPower(pSave);
		  }
		  break;

		  // 离开帮会
		  // uParam : 传入的 STONG_SERVER_TO_CORE_LEAVE point
	  case SGDI_TONG_LEAVE:
		  if (pdata)
		  {
			  STONG_SERVER_TO_CORE_LEAVE	*pLeave = (STONG_SERVER_TO_CORE_LEAVE*)pdata;
			  if (NULL==pLeave) break;
			  if (pLeave->m_nPlayerIdx <= 0 || pLeave->m_nPlayerIdx >= MAX_PLAYER)
				  break;
			  if (Player[pLeave->m_nPlayerIdx].m_nIndex <= 0)
				  break;
			  Player[pLeave->m_nPlayerIdx].m_cTong.Leave(pLeave);
		  }
		  break;

		  // 离开帮会判断
		  // uParam : 传入的 TONG_APPLY_CHANGE_MASTER_COMMAND point
		  // nParam : PlayerIndex
	  case SGDI_TONG_CHANGE_MASTER_POWER:
		  if (pdata)
		  {
			  nRet = 0;
			  TONG_APPLY_CHANGE_MASTER_COMMAND	*pChange = (TONG_APPLY_CHANGE_MASTER_COMMAND*)pdata;
			  if (NULL==pChange) break;
			  if (nParam <= 0 || nParam >= MAX_PLAYER)
				  break;
			  if (Player[nParam].m_nIndex <= 0)
				  break;
			  nRet = Player[nParam].m_cTong.CheckChangeMasterPower(pChange);
		  }
		  break;

		  // 能否接受传位判断
		  // uParam : 传入的 STONG_SERVER_TO_CORE_CHECK_GET_MASTER_POWER point
	  case SGDI_TONG_GET_MASTER_POWER:
		  if (pdata)
		  {
			  nRet = 0;
			  STONG_SERVER_TO_CORE_CHECK_GET_MASTER_POWER	*pCheck = (STONG_SERVER_TO_CORE_CHECK_GET_MASTER_POWER*)pdata;
			  if (NULL==pCheck) break;
			  if (pCheck->m_nPlayerIdx <= 0 || pCheck->m_nPlayerIdx >= MAX_PLAYER)
				  break;
			  if (Player[pCheck->m_nPlayerIdx].m_nIndex <= 0)
				  break;
			  nRet = Player[pCheck->m_nPlayerIdx].m_cTong.CheckGetMasterPower(pCheck);
		  }
		  break;

		  // 传位导致身份改变
		  // uParam : 传入的 STONG_SERVER_TO_CORE_CHANGE_AS point
	  case SGDI_TONG_CHANGE_AS:
		  if (pdata)
		  {
			  STONG_SERVER_TO_CORE_CHANGE_AS	*pAs = (STONG_SERVER_TO_CORE_CHANGE_AS*)pdata;
			  if (NULL==pAs) break;
			  if (pAs->m_nPlayerIdx <= 0 || pAs->m_nPlayerIdx >= MAX_PLAYER)
				  break;
			  if (Player[pAs->m_nPlayerIdx].m_nIndex <= 0)
				  break;
			  Player[pAs->m_nPlayerIdx].m_cTong.ChangeAs(pAs);
		  }
		  break;
	  case SGDI_TONG_CHANGE_TITLE:
	  {
		  if (pdata)
		  {
			  STONG_SERVER_TO_CORE_CHANGE_TITLE* pChange = (STONG_SERVER_TO_CORE_CHANGE_TITLE*)pdata;
			  if (pChange == NULL)
				  break;

			  if (pChange->m_nPlayerIdx <= 0 || pChange->m_nPlayerIdx >= MAX_PLAYER)
				  break;

			  if (Player[pChange->m_nPlayerIdx].m_nIndex <= 0 || Player[pChange->m_nPlayerIdx].m_nIndex >= MAX_NPC)
				  break;

			  Player[pChange->m_nPlayerIdx].m_cTong.ChangeTitle(pChange);
		  }
	  }
	  break;
	  case SGDI_TONG_CHANGE_MONEY:
		  if (pdata)
		  {
			  STONG_SERVER_TO_CORE_MONEY* pChange = (STONG_SERVER_TO_CORE_MONEY*)pdata;
			  if (NULL == pChange) 
				  break;
			  
			  switch(pChange->nType)
			  {
			  case 0://Save
				  Player[pChange->m_nPlayerIdx].Pay(pChange->m_nMoney);
				  break;
			  case 1:
				  Player[pChange->m_nPlayerIdx].Earn(pChange->m_nMoney);
				  break;
			  case 2:
				  int nIdx,nCount = 0;
				  nIdx = PlayerSet.GetFirstPlayer();
				  while (nIdx)
				  {
					  if (Player[nIdx].m_cTong.GetTongNameID() == pChange->m_dwTongNameID && Player[nIdx].m_cTong.GetFigure() == enumTONG_FIGURE_MANAGER)
					  {
						  nCount++;
					  }
					  nIdx = PlayerSet.GetNextPlayer();
				  }
				  int nMoney =0;

				  if  (nCount>0)
					  nMoney = pChange->m_nMoney / nCount;
				  else
					  break;

				  nIdx = PlayerSet.GetFirstPlayer();
				  while (nIdx)
				  {
					  if (Player[nIdx].m_cTong.GetTongNameID() == pChange->m_dwTongNameID && Player[nIdx].m_cTong.GetFigure() == enumTONG_FIGURE_MANAGER)
					  {
						  Player[nIdx].Earn(nMoney);
					  }
					  nIdx = PlayerSet.GetNextPlayer();
				  }
				  Player[pChange->m_nPlayerIdx].Earn(pChange->m_nMoney - (nMoney * nCount));
				  break;
			  }

			  int nIdx;
			  nIdx = PlayerSet.GetFirstPlayer();
			  while (nIdx)
			  {
				  if (Player[nIdx].m_cTong.GetTongNameID() == pChange->m_dwTongNameID)
				  {
					  Player[nIdx].m_cTong.ChangeMoney(pChange->m_dwMoney);
				  }
				  nIdx = PlayerSet.GetNextPlayer();
			  }
		  }
		  break;
		  // 帮主换了
		  // uParam : 传入的 STONG_SERVER_TO_CORE_CHANGE_MASTER point
	  case SGDI_TONG_CHANGE_MASTER:
		  if (pdata)
		  {
			  STONG_SERVER_TO_CORE_CHANGE_MASTER	*pChange = (STONG_SERVER_TO_CORE_CHANGE_MASTER*)pdata;
			  if (NULL==pChange) break;
			  int nIdx;
			  nIdx = PlayerSet.GetFirstPlayer();
			  while (nIdx)
			  {
				  if (Player[nIdx].m_cTong.GetTongNameID() == pChange->m_dwTongNameID)
				  {
					  Player[nIdx].m_cTong.ChangeMaster(pChange->m_szName);
				  }
				  nIdx = PlayerSet.GetNextPlayer();
			  }
		  }
		  break;

		  // 获得帮会名字符串转换成的 dword
		  // nParam : PlayerIndex
	  case SGDI_TONG_GET_TONG_NAMEID:
		  {
			  if (nParam <= 0 || nParam >= MAX_PLAYER)
				  break;
			  if (Player[nParam].m_nIndex <= 0)
				  break;
			  nRet = Player[nParam].m_cTong.GetTongNameID();
		  }
		  break;

		  // 登陆时候获得帮会信息
		  // uParam : 传入的 STONG_SERVER_TO_CORE_LOGIN point
	  case SGDI_TONG_LOGIN:
		  if (pdata)
		  {
			  STONG_SERVER_TO_CORE_LOGIN	*pLogin = (STONG_SERVER_TO_CORE_LOGIN*)pdata;
			  if (NULL==pLogin) break;
			  if (pLogin->m_dwParam <= 0 || pLogin->m_dwParam >= MAX_PLAYER)
				  break;
			  if (Player[pLogin->m_dwParam].m_nIndex <= 0)
				  break;
			  Player[pLogin->m_dwParam].m_cTong.Login(pLogin);
		  }
		  break;

		  // 通知core发送某玩家的帮会信息
		  // nParam : player index
	  case SGDI_TONG_SEND_SELF_INFO:
		  {
			  if (nParam <= 0 || nParam >= MAX_PLAYER)
				  break;
			  if (Player[nParam].m_nIndex <= 0)
				  break;
			  Player[nParam].m_cTong.SendSelfInfo();
		  }
		  break;
	  default:
		  break;
	  }

	  return nRet;
  }

int CoreServerShell::GetGameData_New(unsigned int uDataId, UINT_PTR uParam, int nParam,char *strdata) 
{
	int nRet = 0;
	switch(uDataId)
	{
	case SGDI_CHARACTER_ACCOUNT:
		if (strdata)
			nRet = PlayerSet.GetPlayerAccount(nParam,strdata);
		break;
	case SGDI_CHARACTER_NAME:
		if (strdata)
			nRet = PlayerSet.GetPlayerName(nParam, strdata);
		break;
		// 判断加入帮会条件是否合适
		// uParam : 传入得 char point ，用于接收帮会名称
		// nParam : struct STONG_SERVER_TO_CORE_CHECK_ADD_CONDITION point
	case SGDI_TONG_CHECK_ADD_CONDITION:
		{
			nRet = 0;
			STONG_SERVER_TO_CORE_CHECK_ADD_CONDITION* pAdd = (STONG_SERVER_TO_CORE_CHECK_ADD_CONDITION*)nParam;
			if (NULL == pAdd) 
				break;
			if (pAdd->m_nSelfIdx <= 0 || pAdd->m_nSelfIdx >= MAX_PLAYER)
				break;
			if (pAdd->m_nTargetIdx <= 0 || pAdd->m_nTargetIdx >= MAX_PLAYER || g_FileName2Id(Npc[Player[pAdd->m_nTargetIdx].m_nIndex].Name) != pAdd->m_dwNameID)
				break;
			if (Player[pAdd->m_nSelfIdx].m_cTong.CheckAddCondition(pAdd->m_nTargetIdx))
			{
				if (strdata)
					Player[pAdd->m_nSelfIdx].m_cTong.GetTongName(strdata);
				nRet = 1;
			}
		}
		break;
	default:
		break;
	}

	return nRet;
}
//--------------------------------------------------------------------------
//	功能：从游戏世界获取数据
//	参数：unsigned int uDataId --> 表示获取游戏数据的数据项内容索引，其值为梅举类型
//							GAMEDATA_INDEX的取值之一。
//		  unsigned int uParam  --> 依据uDataId的取值情况而定
//		  int nParam --> 依据uDataId的取值情况而定
//	返回：依据uDataId的取值情况而定。
//--------------------------------------------------------------------------
INT_PTR 	CoreServerShell::GetGameData(unsigned int uDataId, UINT_PTR uParam, int nParam)
{
	// SGDI_CHECK_TONG below returns the address of a tong name that lives inside a player, and most
	// of the cases read their payload back out of uParam as a struct pointer, so both halves of
	// this boundary have to be as wide as an address to survive the trip.  The remaining cases
	// hand back a small count, and their callers still read an int out of it.
	INT_PTR nRet = 0;
	switch(uDataId)
	{
	case SGDI_TONG_CHECK_POWER:
		if (uParam)
		{
			nRet = 0;
			TONG_APPLY_POWER_COMMAND* pApply = (TONG_APPLY_POWER_COMMAND*)uParam;
			if (nParam <= 0 || nParam >= MAX_PLAYER)
				break;
			if (Player[nParam].m_nIndex <= 0)
				break;
			nRet = Player[nParam].m_cTong.CheckPowerPower(pApply);
		}
		break;
	case SGDI_CHARACTER_ACCOUNT:
		if (uParam)
		{
			nRet = PlayerSet.GetPlayerAccount(nParam, (char *)uParam);
			if (nRet == FALSE)
				((char *)uParam)[0] = 0;
		}
		break;
	case SGDI_CHARACTER_NAME:
		if (uParam)
		{
			nRet = PlayerSet.GetPlayerName(nParam, (char*)uParam);
			if (nRet == FALSE)
				((char *)uParam)[0] = 0;
		}
		break;
	case SGDI_CHARACTER_EXTPOINTCHANGED:
		if (uParam)
		{
			if (uParam >= MAX_PLAYER)
			{
				nRet = 0;
				break;
			}
			nRet = Player[uParam].GetExtPointChanged();
		}
		break;
   
	case SGDI_CHARACTER_EXTPOINT:
		if (uParam)
		{
			if (uParam >= MAX_PLAYER)
			{
				nRet = 0;
				break;
			}
			nRet = Player[uParam].GetExtPoint();
		}
		break;
	case SGDI_CHARACTER_EXTTIMECHANGED:
		if (uParam)
		{
			if (uParam >= MAX_PLAYER)
			{
				nRet = 0;
				break;
			}
			nRet = Player[uParam].GetExtTimeChanged();
		}
		break;
    case SGDI_CHARACTER_LEFTTIME:
		if (uParam)
		{
			if (uParam >= MAX_PLAYER)
			{
				nRet = 0;
				break;
			}
			nRet = Player[uParam].GetLeftTime();
		}
		break;
	case SGDI_CHARACTER_VIPTYPE:
		if (uParam)
		{
			if (uParam >= MAX_PLAYER)
			{
				nRet = 0;
				break;
			}
			nRet = Player[uParam].GetVipType();
		}
		break;
	case SGDI_LOADEDMAP_ID:
		if (uParam)
		{
			int i;
			int nMax = nParam;

			// `< bound || > bound' is true for every nMax except the bound itself, so this clamped
			// every request to the maximum and a caller could never ask for fewer loaded maps.
			if(nMax > MAX_SUBWORLD) nMax = MAX_SUBWORLD;
			int nCount =0;
			for (i = 0; i < nMax; i++)
			{
				if (SubWorld[i].m_SubWorldID != -1)
				{
					((int *)uParam)[nCount] = SubWorld[i].m_SubWorldID; 
					// ((char *)uParam)[i] = (char)SubWorld[i].m_SubWorldID;
					nCount++;
				}
				/*else
				{
				nRet = i;
				break;
				}*/
			}
			return nCount;
		}
		break;
		/*if (uParam)
		{
		int i;
		int nMax = nParam;
		if (nMax > MAX_SUBWORLD)
		nMax = MAX_SUBWORLD;
		int nReg=0;
		for (i = 0; i < nMax; ++i)
		{
		if (SubWorld[i].m_SubWorldID >-1)
		{
		((char *)uParam)[i] = (char)SubWorld[i].m_SubWorldID;
		nReg++;
		}
		}
		nRet=nReg;
		}
		break;*/
	case SGDI_CHARACTER_ID:
		if (uParam)
		{
			if (uParam >= MAX_PLAYER)
			{
				nRet = 0;
				break;
			}

			nRet = Player[uParam].m_dwID;
			break;
		}
		break;
	case SGDI_CHECK_TONG:
		{
			if (uParam >= MAX_PLAYER)
			{
				nRet = 0;
				break;
			}
			   if (Player[uParam].m_cTong.m_nFlag)
			      nRet =(INT_PTR)&(Player[uParam].m_cTong.m_szName);
			   else
				  nRet =0;
			break;

		}
		break;
	case SGDI_CHARACTER_NETID:
		if (uParam)
		{
			if (uParam >= MAX_PLAYER)
			{
				nRet = -1;
				break;
			}
			nRet = Player[uParam].m_nNetConnectIdx;
		}
		break;

	// 传入帮会建立参数，返回条件是否成立
	// uParam : struct STONG_SERVER_TO_CORE_APPLY_CREATE point
	// return : 条件是否成立
	case SGDI_TONG_APPLY_CREATE:
		if (uParam)
		{
			STONG_SERVER_TO_CORE_APPLY_CREATE	*pApply = (STONG_SERVER_TO_CORE_APPLY_CREATE*)uParam;

			if (NULL==pApply) break;

			int		nPlayerIdx;
			int		nCamp;
			char	szTongName[NAME_LENGHT];

			szTongName[sizeof(szTongName) - 1] = 0;
			memcpy(szTongName, pApply->m_szTongName, sizeof(szTongName));
			nPlayerIdx = pApply->m_nPlayerIdx;
			nCamp = pApply->m_nCamp;

			if (nPlayerIdx <= 0 || nPlayerIdx >= MAX_PLAYER)
			{
				nRet = TRUE;  //
				break;
			}
			nRet = Player[nPlayerIdx].m_cTong.CheckCreateCondition(nCamp, szTongName);
		}
		break;

	// 申请加入帮会
	// uParam : struct STONG_SERVER_TO_CORE_APPLY_ADD point
	case SGDI_TONG_APPLY_ADD:
		if (uParam)
		{
			STONG_SERVER_TO_CORE_APPLY_ADD	*pAdd = (STONG_SERVER_TO_CORE_APPLY_ADD*)uParam;
			if (NULL==pAdd) break;
			if (pAdd->m_nPlayerIdx <= 0 || pAdd->m_nPlayerIdx >= MAX_PLAYER)
				break;
			//服务器端的
			Player[pAdd->m_nPlayerIdx].m_cTong.TransferAddApply(pAdd->m_dwNpcID);
		}
		break;
	case SGDI_TONG_APPLY_INVITE:
		if (uParam)
		{
			STONG_SERVER_TO_CORE_APPLY_INVITE* pInvite = (STONG_SERVER_TO_CORE_APPLY_INVITE*)uParam;
			if (pInvite->m_nPlayerIdx <= 0 || pInvite->m_nPlayerIdx >= MAX_PLAYER)
				break;
			//服务器端的
			Player[pInvite->m_nPlayerIdx].m_cTong.TransferInviteApply(pInvite->m_dwNpcID);
		}
		break;

	// 判断加入帮会条件是否合适
	// uParam : 传入得 char point ，用于接收帮会名称
	// nParam : struct STONG_SERVER_TO_CORE_CHECK_ADD_CONDITION point
	case SGDI_TONG_CHECK_ADD_CONDITION:
		{
			nRet = 0;
			STONG_SERVER_TO_CORE_CHECK_ADD_CONDITION	*pAdd=NULL;
			pAdd = (STONG_SERVER_TO_CORE_CHECK_ADD_CONDITION*)nParam;
			if (NULL==pAdd) break;
			if (pAdd->m_nSelfIdx <= 0 || pAdd->m_nSelfIdx >= MAX_PLAYER)
				break;
			if (pAdd->m_nTargetIdx <= 0 || pAdd->m_nTargetIdx >= MAX_PLAYER || g_FileName2Id(Npc[Player[pAdd->m_nTargetIdx].m_nIndex].Name) != pAdd->m_dwNameID)
				break;
			if (Player[pAdd->m_nSelfIdx].m_cTong.CheckAddCondition(pAdd->m_nTargetIdx))
			{
				Player[pAdd->m_nSelfIdx].m_cTong.GetTongName((char*)uParam);
				nRet = 1;
			}
		}
		break;

	// 获得帮会信息
	// uParam : 传入的 STONG_SERVER_TO_CORE_GET_INFO point
	case SGDI_TONG_GET_INFO:
		{
			STONG_SERVER_TO_CORE_GET_INFO	*pInfo = (STONG_SERVER_TO_CORE_GET_INFO*)uParam;
			if (NULL==pInfo) break;
			switch (pInfo->m_nInfoID)
			{
			case enumTONG_APPLY_INFO_ID_SELF:
				{
					if (pInfo->m_nSelfIdx <= 0 || pInfo->m_nSelfIdx >= MAX_PLAYER)
						break;
					Player[pInfo->m_nSelfIdx].m_cTong.SendSelfInfo();
				}
				break;
			case enumTONG_APPLY_INFO_ID_MASTER:
				break;
			case enumTONG_APPLY_INFO_ID_DIRECTOR:
				break;
			case enumTONG_APPLY_INFO_ID_MANAGER:
				{
					nRet = 0;
					if (pInfo->m_nSelfIdx <= 0 || pInfo->m_nSelfIdx >= MAX_PLAYER)
						break;
					if (Player[pInfo->m_nSelfIdx].m_cTong.CanGetManagerInfo((DWORD)pInfo->m_nParam1))
						nRet = 1;
				}
				break;
			case enumTONG_APPLY_INFO_ID_MEMBER:
				{
					nRet = 0;
					if (pInfo->m_nSelfIdx <= 0 || pInfo->m_nSelfIdx >= MAX_PLAYER)
						break;
					if (Player[pInfo->m_nSelfIdx].m_cTong.CanGetMemberInfo((DWORD)pInfo->m_nParam1))
						nRet = 1;
				}
				break;
			case enumTONG_APPLY_INFO_ID_ONE:
				break;

			case enumTONG_APPLY_INFO_ID_TONG_HEAD:
				{
					nRet = 0;
					if (pInfo->m_nSelfIdx <= 0 || pInfo->m_nSelfIdx >= MAX_PLAYER)
						break;
					int		nPlayer;
					if (Npc[Player[pInfo->m_nSelfIdx].m_nIndex].m_dwID == (DWORD)pInfo->m_nParam1)
						nPlayer = pInfo->m_nSelfIdx;
					else
						nPlayer = Player[pInfo->m_nSelfIdx].FindAroundPlayer((DWORD)pInfo->m_nParam1);
					if (nPlayer == -1)
						break;
					nRet = Player[nPlayer].m_cTong.GetTongNameID();
				}
				break;
			}
		}
		break;

	// 判断是否有任命权利
	// uParam : 传入的 TONG_APPLY_INSTATE_COMMAND point
	// nParam : PlayerIndex
	case SGDI_TONG_INSTATE_POWER:
		if (uParam)
		{
			nRet = 0;
			TONG_APPLY_INSTATE_COMMAND	*pApply = (TONG_APPLY_INSTATE_COMMAND*)uParam;
			if (NULL==pApply) break;
			if (nParam <= 0 || nParam >= MAX_PLAYER)
				break;
			if (Player[nParam].m_nIndex <= 0)
				break;
			nRet = Player[nParam].m_cTong.CheckInstatePower(pApply);
		}
		break;

	// 被任命，帮会数据变化
	// uParam : 传入的 STONG_SERVER_TO_CORE_BE_INSTATED point
	case SGDI_TONG_BE_INSTATED:
		if (uParam)
		{
			STONG_SERVER_TO_CORE_BE_INSTATED	*pInstated = (STONG_SERVER_TO_CORE_BE_INSTATED*)uParam;
			if (NULL==pInstated) break;
			if (pInstated->m_nPlayerIdx <= 0 || pInstated->m_nPlayerIdx >= MAX_PLAYER)
				break;
			if (Player[pInstated->m_nPlayerIdx].m_nIndex <= 0)
				break;
			Player[pInstated->m_nPlayerIdx].m_cTong.BeInstated(pInstated);
		}
		break;

	// 判断是否有踢人权利
	// uParam : 传入的 TONG_APPLY_KICK_COMMAND point
	// nParam : PlayerIndex
	case SGDI_TONG_KICK_POWER:
		if (uParam)
		{
			nRet = 0;
			TONG_APPLY_KICK_COMMAND	*pKick = (TONG_APPLY_KICK_COMMAND*)uParam;
			if (NULL==pKick) break;
			if (nParam <= 0 || nParam >= MAX_PLAYER)
				break;
			if (Player[nParam].m_nIndex <= 0)
				break;
			nRet = Player[nParam].m_cTong.CheckKickPower(pKick);
		}
		break;

	// 被踢出帮会
	// uParam : 传入的 STONG_SERVER_TO_CORE_BE_KICKED point
	case SGDI_TONG_BE_KICKED:
		if (uParam)
		{
			STONG_SERVER_TO_CORE_BE_KICKED	*pKicked = (STONG_SERVER_TO_CORE_BE_KICKED*)uParam;
			if (NULL==pKicked) break;
			if (pKicked->m_nPlayerIdx <= 0 || pKicked->m_nPlayerIdx >= MAX_PLAYER)
				break;
			if (Player[pKicked->m_nPlayerIdx].m_nIndex <= 0)
				break;
			Player[pKicked->m_nPlayerIdx].m_cTong.BeKicked(pKicked);
		}
		break;

	// 离开帮会
	// uParam : 传入的 TONG_APPLY_LEAVE_COMMAND point
	// nParam : PlayerIndex
	case SGDI_TONG_LEAVE_POWER:
		if (uParam)
		{
			nRet = 0;
			TONG_APPLY_LEAVE_COMMAND	*pLeave = (TONG_APPLY_LEAVE_COMMAND*)uParam;
			if (NULL==pLeave) break;
			if (nParam <= 0 || nParam >= MAX_PLAYER)
				break;
			if (Player[nParam].m_nIndex <= 0)
				break;
			nRet = Player[nParam].m_cTong.CheckLeavePower(pLeave);
		}
		break;

	case SGDI_TONG_MONEY_POWER:
		if (uParam)
		{
			nRet = 0;
			TONG_APPLY_SAVE_COMMAND	*pSave = (TONG_APPLY_SAVE_COMMAND*)uParam;
			if (NULL==pSave) break;
			if (nParam <= 0 || nParam >= MAX_PLAYER)
				break;
			if (Player[nParam].m_nIndex <= 0)
				break;
			nRet = Player[nParam].m_cTong.CheckMoneyPower(pSave);
		}
		break;

	// 离开帮会
	// uParam : 传入的 STONG_SERVER_TO_CORE_LEAVE point
	case SGDI_TONG_LEAVE:
		if (uParam)
		{
			STONG_SERVER_TO_CORE_LEAVE	*pLeave = (STONG_SERVER_TO_CORE_LEAVE*)uParam;
			if (NULL==pLeave) break;
			if (pLeave->m_nPlayerIdx <= 0 || pLeave->m_nPlayerIdx >= MAX_PLAYER)
				break;
			if (Player[pLeave->m_nPlayerIdx].m_nIndex <= 0)
				break;
			Player[pLeave->m_nPlayerIdx].m_cTong.Leave(pLeave);
		}
		break;

	// 离开帮会判断
	// uParam : 传入的 TONG_APPLY_CHANGE_MASTER_COMMAND point
	// nParam : PlayerIndex
	case SGDI_TONG_CHANGE_MASTER_POWER:
		if (uParam)
		{
			nRet = 0;
			TONG_APPLY_CHANGE_MASTER_COMMAND	*pChange = (TONG_APPLY_CHANGE_MASTER_COMMAND*)uParam;
			if (NULL==pChange) break;
			if (nParam <= 0 || nParam >= MAX_PLAYER)
				break;
			if (Player[nParam].m_nIndex <= 0)
				break;
			nRet = Player[nParam].m_cTong.CheckChangeMasterPower(pChange);
		}
		break;

	// 能否接受传位判断
	// uParam : 传入的 STONG_SERVER_TO_CORE_CHECK_GET_MASTER_POWER point
	case SGDI_TONG_GET_MASTER_POWER:
		if (uParam)
		{
			nRet = 0;
			STONG_SERVER_TO_CORE_CHECK_GET_MASTER_POWER	*pCheck = (STONG_SERVER_TO_CORE_CHECK_GET_MASTER_POWER*)uParam;
			if (NULL==pCheck) break;
			if (pCheck->m_nPlayerIdx <= 0 || pCheck->m_nPlayerIdx >= MAX_PLAYER)
				break;
			if (Player[pCheck->m_nPlayerIdx].m_nIndex <= 0)
				break;
			nRet = Player[pCheck->m_nPlayerIdx].m_cTong.CheckGetMasterPower(pCheck);
		}
		break;

	// 传位导致身份改变
	// uParam : 传入的 STONG_SERVER_TO_CORE_CHANGE_AS point
	case SGDI_TONG_CHANGE_AS:
		if (uParam)
		{
			STONG_SERVER_TO_CORE_CHANGE_AS	*pAs = (STONG_SERVER_TO_CORE_CHANGE_AS*)uParam;
			if (NULL==pAs) break;
			if (pAs->m_nPlayerIdx <= 0 || pAs->m_nPlayerIdx >= MAX_PLAYER)
				break;
			if (Player[pAs->m_nPlayerIdx].m_nIndex <= 0)
				break;
			Player[pAs->m_nPlayerIdx].m_cTong.ChangeAs(pAs);
		}
		break;
	case SGDI_TONG_CHANGE_MONEY:
		if (uParam)
		{
			STONG_SERVER_TO_CORE_MONEY	*pChange = (STONG_SERVER_TO_CORE_MONEY*)uParam;
			if (NULL==pChange) break;
			switch(pChange->nType)
			{
			case 0://Save
				Player[pChange->m_nPlayerIdx].Pay(pChange->m_nMoney);
				break;
			case 1:
				Player[pChange->m_nPlayerIdx].Earn(pChange->m_nMoney);
				break;
			case 2:
				int nIdx,nCount = 0;
				nIdx = PlayerSet.GetFirstPlayer();
				while (nIdx)
				{
					if (Player[nIdx].m_cTong.GetTongNameID() == pChange->m_dwTongNameID && Player[nIdx].m_cTong.GetFigure() == enumTONG_FIGURE_MANAGER)
					{
						nCount++;
					}
					nIdx = PlayerSet.GetNextPlayer();
				}
				int nMoney =0;

                if  (nCount>0)
				    nMoney = pChange->m_nMoney / nCount;
				else
					break;

				nIdx = PlayerSet.GetFirstPlayer();
				while (nIdx)
				{
					if (Player[nIdx].m_cTong.GetTongNameID() == pChange->m_dwTongNameID && Player[nIdx].m_cTong.GetFigure() == enumTONG_FIGURE_MANAGER)
					{
						Player[nIdx].Earn(nMoney);
					}
					nIdx = PlayerSet.GetNextPlayer();
				}
				Player[pChange->m_nPlayerIdx].Earn(pChange->m_nMoney - (nMoney * nCount));
				break;
			}

			int nIdx;
			nIdx = PlayerSet.GetFirstPlayer();
			while (nIdx)
			{
				if (Player[nIdx].m_cTong.GetTongNameID() == pChange->m_dwTongNameID)
				{
					Player[nIdx].m_cTong.ChangeMoney(pChange->m_dwMoney);
				}
				nIdx = PlayerSet.GetNextPlayer();
			}
		}
		break;
	// 帮主换了
	// uParam : 传入的 STONG_SERVER_TO_CORE_CHANGE_MASTER point
	case SGDI_TONG_CHANGE_MASTER:
		if (uParam)
		{
			STONG_SERVER_TO_CORE_CHANGE_MASTER	*pChange = (STONG_SERVER_TO_CORE_CHANGE_MASTER*)uParam;
			if (NULL==pChange) break;
			int nIdx;
			nIdx = PlayerSet.GetFirstPlayer();
			while (nIdx)
			{
				if (Player[nIdx].m_cTong.GetTongNameID() == pChange->m_dwTongNameID)
				{
					Player[nIdx].m_cTong.ChangeMaster(pChange->m_szName);
				}
				nIdx = PlayerSet.GetNextPlayer();
			}
		}
		break;

	// 获得帮会名字符串转换成的 dword
	// nParam : PlayerIndex
	case SGDI_TONG_GET_TONG_NAMEID:
		{
			if (nParam <= 0 || nParam >= MAX_PLAYER)
				break;
			if (Player[nParam].m_nIndex <= 0)
				break;
			nRet = Player[nParam].m_cTong.GetTongNameID();
		}
		break;

	// 登陆时候获得帮会信息
	// uParam : 传入的 STONG_SERVER_TO_CORE_LOGIN point
	case SGDI_TONG_LOGIN:
		if (uParam)
		{
			STONG_SERVER_TO_CORE_LOGIN	*pLogin = (STONG_SERVER_TO_CORE_LOGIN*)uParam;
			if (NULL==pLogin) break;
			if (pLogin->m_dwParam <= 0 || pLogin->m_dwParam >= MAX_PLAYER)
				break;
			if (Player[pLogin->m_dwParam].m_nIndex <= 0)
				break;
			Player[pLogin->m_dwParam].m_cTong.Login(pLogin);
		}
		break;
		
	// 通知core发送某玩家的帮会信息
	// nParam : player index
	case SGDI_TONG_SEND_SELF_INFO:
		{
			if (nParam <= 0 || nParam >= MAX_PLAYER)
				break;
			if (Player[nParam].m_nIndex <= 0)
				break;
			Player[nParam].m_cTong.SendSelfInfo();
		}
		break;


	default:
		break;
	}
	return nRet;
}




//设置人物的网络状态
/*int	 CoreServerShell::SetPlayePingReplyTime(int nPlayerIdx,int npTime)
{
	if (nPlayerIdx>0 && nPlayerIdx<=MAX_PLAYER)
	{
		Player[nPlayerIdx].m_nReplyPingTime=npTime;
		return 1;
	}
	
	return 0;
}

int	 CoreServerShell::SetPlayeExchangeStatus(int nPlayerIdx,int nStats)
{
		if (nPlayerIdx>0 && nPlayerIdx<=MAX_PLAYER)
		{
			Player[nPlayerIdx].m_nExchangeStatus=nStats;
			return 1;
		}
		
		return 0;
}


//设置人物的网络状态
int	 CoreServerShell::SetPlayePingStratTime(int nPlayerIdx,int npTime)
{
	if (nPlayerIdx>0 && nPlayerIdx<=MAX_PLAYER)
	{
		Player[nPlayerIdx].m_nSendPingTime=npTime;
		return 1;
	}
	
	return 0;
}

int	 CoreServerShell::GetPlayeNetStates(int nPlayerIdx)
{
	if (nPlayerIdx>0 && nPlayerIdx<=MAX_PLAYER)
	{
		return 	Player[nPlayerIdx].m_nNetStatus;
	}
	return -1;
}

//设置人物的网络状态
int	 CoreServerShell::SetPlayeNetStates(int nPlayerIdx,int nStats)
{
	if (nPlayerIdx>0 && nPlayerIdx<=MAX_PLAYER)
	{
		int nIndex = nPlayerIdx;

		if (nStats == enumNetUnconnect)
		{
			if (Player[nIndex].m_nGameStatus == enumPlayerPlaying\
				|| (Player[nIndex].m_nGameStatus  == enumPlayerExchangingServer\
				&& Player[nIndex].m_nExchangeStatus != enumExchangeCleaning))	// 跨服务器时自己处理
			{//游戏中 或 转换地图
				ClientDisconnect(nIndex);  // 设置离开游戏状态--断开客户端链接
			}
			else
			{//登陆失败
				PreparePlayerForLoginFailed(nIndex);
			}
			
			Player[nIndex].m_nGameStatus = enumPlayerBegin;
			//m_pGameStatus[lnID].nPlayerIndex = 0;
			Player[nIndex].m_nExchangeStatus = enumExchangeBegin;   //设置开始转服
			Player[nIndex].m_nReplyPingTime = 0;
			Player[nIndex].m_nSendPingTime = 0;

		}
		if (nStats == enumNetConnected)
		{//设置网络链接状态
			Player[nIndex].m_nGameStatus = enumPlayerBegin;
			//m_pGameStatus[lnID].nPlayerIndex = 0;
			Player[nIndex].m_nExchangeStatus = enumExchangeBegin;
			Player[nIndex].m_nReplyPingTime = 0;
			Player[nIndex].m_nSendPingTime = 0;
			
		} 

		Player[nPlayerIdx].m_nNetStatus=nStats;
		return 1;
	}	
	return 0;
}

//设置人物的游戏状态
int	 CoreServerShell::SetPlayeGameStates(int nPlayerIdx,int nStats)
{
	 if (nPlayerIdx>0 && nPlayerIdx<=MAX_PLAYER)
	 {
		Player[nPlayerIdx].m_nGameStatus=nStats;
		return 1;
	 }

	 return 0;
}
*/

void CoreServerShell::doGoldthreadScript(const void *pData)
{
	if (!pData) return;
	GS_TASK_GROUPMAN *pS3BroadCast;
	pS3BroadCast = (GS_TASK_GROUPMAN *)pData;  //pS3BroadCast->nPath
    g_SubWorldSet.SubExecuteScriptA("\\script\\task\\goldthread.lua","goldthread",99);
}

void CoreServerShell::doTaskScript(const void *pData)
{
	if (!pData) 
		return;

	GS_TASK_GROUPMAN *pS3BroadCast;
	pS3BroadCast = (GS_TASK_GROUPMAN *)pData;

	if  (pS3BroadCast->isOpen)
	{
	     g_SubWorldSet.SubExecuteScriptA(pS3BroadCast->nPath,"taskcontent",pS3BroadCast->nTaskNo,pS3BroadCast->nNextTime);
		 printf("Open Execution succeeded(%d),Task Name:%s,cTime:%s,nTime:%s \n",pS3BroadCast->nTaskNo,pS3BroadCast->taskName,pS3BroadCast->nCurTime,pS3BroadCast->nNextTime);
	}
	else
	{
		 g_SubWorldSet.SubExecuteScriptA(pS3BroadCast->nPath,"colsetask",pS3BroadCast->nTaskNo,NULL);
		 printf("Closed succeeded(%d),Task Name:%s \n",pS3BroadCast->nTaskNo,pS3BroadCast->taskName);
	}

	if   (pS3BroadCast->isSave)
	{
		KIniFile taskData;
		if (taskData.Load("\\Settings\\Task\\taskdata.ini",true))
		{
			char nSec[64],nKey[64];
			sprintf(nSec,"Task_%d",pS3BroadCast->nTaskNo-1);
			taskData.WriteString(nSec,"TaskName",pS3BroadCast->taskName);
			taskData.WriteString(nSec,"TaskOpenTime",pS3BroadCast->nCurTime);
			taskData.WriteString(nSec,"TaskNextTime",pS3BroadCast->nNextTime);
			taskData.WriteInteger(nSec,"IsOpen",(int)pS3BroadCast->isOpen);
			taskData.WriteInteger(nSec,"isShow",(int)pS3BroadCast->isShow);	
			taskData.Save("\\Settings\\Task\\taskdata.ini");
			taskData.Clear();
		}
	}

}

void CoreServerShell::gm_LocalScript(char *msg)
{
	if  (!msg) return;
	ReLoadScript(msg);
}

void CoreServerShell::gm_addNpc(const void *pData)
{
	if (!pData) return;
	tagMsgInGame	*pGatewayBroadCast;
	pGatewayBroadCast = (tagMsgInGame *)pData;
	g_SubWorldSet.SubExecuteScriptA(pGatewayBroadCast->strMsg,"gm_addNpc",pGatewayBroadCast->nMgsIdx,NULL);

}


void CoreServerShell::SendSystemCahtInfo(char *msg)
{
	if  (!msg) return;
	char nMsg[256];
	ZeroMemory(nMsg,sizeof(nMsg));
	snprintf(nMsg, sizeof(nMsg), "%s", msg);
	int nleg=TEncodeText(nMsg,strlen(nMsg));
	const char * szMsg = nMsg;
	if (szMsg)
		KPlayerChat::SendSystemInfo(0,0,strCoreInfo[MESSAGE_SYSTEM_ANNOUCE_HEAD].c_str(), (char *)szMsg, nleg);  //strlen(szMsg) 
}


int 	CoreServerShell::OperationRequestInfo(unsigned int uOper, UINT_PTR uParam, int nParam,int inParam,void *pdata,char *strdata)
{
	int nRet = 1;
	switch(uOper)
	{
	case SSOI_SETGOLADMISS:
		g_GlobalMissionArray.SetMissionValue((UINT)uParam,nParam);
		break;
	case SSOI_BROADCASTING:         //发公告
		nRet = PlayerSet.Broadcasting(strdata, nParam);
		break;
	case SSOI_LAUNCH:	            //启动服务
		nRet = OnLunch((LPVOID)uParam);
		break;
	case SSOI_SHUTDOWN:	            //关闭服务
		nRet = OnShutdown();
		break;
	case SSOI_RELOAD_WELCOME_MSG:   //发送欢迎语
		//PlayerSet.ReloadWelcomeMsg(nParam);
		if (uParam==0)
		   PlayerSet.SendWelcomeMsg(nParam);
		if (uParam==1 && pdata)
		{
			PlayerSet.SendAttackMsg(pdata,inParam);
		}
		break;
	// relay 帮会创建成功，通知 core 进行相应的处理
	case SSOI_TONG_CREATE:
		{
			if (!pdata) break;

			STONG_SERVER_TO_CORE_CREATE_SUCCESS	*pCreate = (STONG_SERVER_TO_CORE_CREATE_SUCCESS*)pdata;
			if (pCreate->m_nPlayerIdx <= 0 || pCreate->m_nPlayerIdx >= MAX_PLAYER)
			{
				nRet = 0;
				break;
			}
			if (Player[pCreate->m_nPlayerIdx].m_nIndex)
			{//查找帮主名dwID
				DWORD	dwID = g_FileName2Id(Npc[Player[pCreate->m_nPlayerIdx].m_nIndex].Name);
				if (dwID != pCreate->m_dwPlayerNameID)
				{
					nRet = 0;
					break;
				}
			}
			else
			{
				nRet = 0;
				break;
			}
			//通知客户单创建帮会成功，并开始通知创建频道
			nRet = Player[pCreate->m_nPlayerIdx].CreateTong(pCreate->m_nCamp, pCreate->m_szTongName, pCreate->m_szMasterTitle);
		}
		break;

	case SSOI_TONG_REFUSE_ADD:
		if (pdata)
		{//拒绝加入帮会
			STONG_SERVER_TO_CORE_REFUSE_ADD	*pRefuse = (STONG_SERVER_TO_CORE_REFUSE_ADD*)pdata;
			if (pRefuse->m_nSelfIdx > 0 && pRefuse->m_nSelfIdx <= MAX_PLAYER)
			{
				Player[pRefuse->m_nSelfIdx].m_cTong.SendRefuseMessage(pRefuse->m_nTargetIdx, pRefuse->m_dwNameID);
			}
		}
		break;

	case SSOI_TONG_ADD:
		if (pdata)
		{
			nRet = 0;
			STONG_SERVER_TO_CORE_ADD_SUCCESS	*pAdd = (STONG_SERVER_TO_CORE_ADD_SUCCESS*)pdata;
			if (pAdd->m_nPlayerIdx <= 0 || pAdd->m_nPlayerIdx >= MAX_PLAYER)
				break;
			if (Player[pAdd->m_nPlayerIdx].m_nIndex <= 0)
				break;
			if (g_FileName2Id(Npc[Player[pAdd->m_nPlayerIdx].m_nIndex].Name) != pAdd->m_dwPlayerNameID)
				break;
			Player[pAdd->m_nPlayerIdx].m_cTong.AddTong(
				pAdd->m_nCamp,
				pAdd->m_szTongName,
				pAdd->m_szMasterName,
				pAdd->m_szTitleName);
		}
		break;

	default:
		nRet = 0;
		break;
	}	
	return nRet;
}


//--------------------------------------------------------------------------
//	功能：向游戏发送操作
//	参数：unsigned int uDataId --> Core外部客户对core的操作请求的索引定义
//							其值为梅举类型GAMEOPERATION_INDEX的取值之一。
//		  unsigned int uParam  --> 依据uOperId的取值情况而定
//		  int nParam --> 依据uOperId的取值情况而定
//	返回：如果成功发送操作请求，函数返回非0值，否则返回0值。
//--------------------------------------------------------------------------
int	CoreServerShell::OperationRequest(unsigned int uOper, UINT_PTR uParam, int nParam,int inParam)
{
	int nRet = 1;
	switch(uOper)
	{
	case SSOI_SETGOLADMISS:
		g_GlobalMissionArray.SetMissionValue((UINT)uParam,nParam);
		break;
	case SSOI_BROADCASTING:         //发公告
		nRet = PlayerSet.Broadcasting((char*)uParam, nParam);
		break;
	case SSOI_LAUNCH:	            //启动服务
		nRet = OnLunch((LPVOID)uParam);
		break;
	case SSOI_SHUTDOWN:	            //关闭服务
		nRet = OnShutdown();
		break;
	case SSOI_RELOAD_WELCOME_MSG:   //发送欢迎语
		if (uParam==0)
		   PlayerSet.SendWelcomeMsg(nParam);
		break;
	// relay 帮会创建成功，通知 core 进行相应的处理
	case SSOI_TONG_CREATE:
		{
			STONG_SERVER_TO_CORE_CREATE_SUCCESS	*pCreate = (STONG_SERVER_TO_CORE_CREATE_SUCCESS*)uParam;
			if (pCreate->m_nPlayerIdx <= 0 || pCreate->m_nPlayerIdx >= MAX_PLAYER)
			{
				nRet = 0;
				break;
			}
			if (Player[pCreate->m_nPlayerIdx].m_nIndex)
			{//查找帮主名dwID
				DWORD	dwID = g_FileName2Id(Npc[Player[pCreate->m_nPlayerIdx].m_nIndex].Name);
				if (dwID != pCreate->m_dwPlayerNameID)
				{
					nRet = 0;
					break;
				}
			}
			else
			{
				nRet = 0;
				break;
			}
			//通知客户单创建帮会成功，并开始通知创建频道
			nRet = Player[pCreate->m_nPlayerIdx].CreateTong(pCreate->m_nCamp, pCreate->m_szTongName, pCreate->m_szMasterTitle);
		}
		break;

	case SSOI_TONG_REFUSE_ADD:
		if (uParam)
		{//拒绝加入帮会
			STONG_SERVER_TO_CORE_REFUSE_ADD	*pRefuse = (STONG_SERVER_TO_CORE_REFUSE_ADD*)uParam;
			if (pRefuse->m_nSelfIdx > 0 && pRefuse->m_nSelfIdx <= MAX_PLAYER)
			{
				Player[pRefuse->m_nSelfIdx].m_cTong.SendRefuseMessage(pRefuse->m_nTargetIdx, pRefuse->m_dwNameID);
			}
		}
		break;

	case SSOI_TONG_ADD:
		if (uParam)
		{
			nRet = 0;
			STONG_SERVER_TO_CORE_ADD_SUCCESS	*pAdd = (STONG_SERVER_TO_CORE_ADD_SUCCESS*)uParam;
			if (pAdd->m_nPlayerIdx <= 0 || pAdd->m_nPlayerIdx >= MAX_PLAYER)
				break;
			if (Player[pAdd->m_nPlayerIdx].m_nIndex <= 0)
				break;
			if (g_FileName2Id(Npc[Player[pAdd->m_nPlayerIdx].m_nIndex].Name) != pAdd->m_dwPlayerNameID)
				break;
			Player[pAdd->m_nPlayerIdx].m_cTong.AddTong(
				pAdd->m_nCamp,
				pAdd->m_szTongName,
				pAdd->m_szMasterName,
				pAdd->m_szTitleName);
		}
		break;

	default:
		nRet = 0;
		break;
	}	
	return nRet;
}

int CoreServerShell::OnSetTran(LPVOID pServer)
{
	//g_SetTranServer(pServer);
	g_pTranClient= reinterpret_cast<IClient *>(pServer);
	return true;
}

int CoreServerShell::OnSetChat(LPVOID pServer)
{
	//g_SetChatServer(pServer);
	g_pChatClient= reinterpret_cast<IClient *>(pServer);
	return true;

}

int CoreServerShell::OnSetTong(LPVOID pServer)
{
	//g_SetTonServer(pServer);
	g_pTongClient= reinterpret_cast<IClient *>(pServer);
	return true;

}


int CoreServerShell::OnSetData(LPVOID pServer)
{
	//g_SetDataServer(pServer);
	g_pDataClient= reinterpret_cast<IClient *>(pServer);
	// Final P0: Real GameServer Startup Crash Recovery hook
	bool bRecoverySuccess = KPlayerTradeJournal::Instance().StartupRecovery(g_pDataClient);
	if (!bRecoverySuccess)
	{
		printf("[CoreServerShell::OnSetData]: StartupRecovery had unreconciled transactions. Recovery barrier active.\n");
	}
	return bRecoverySuccess ? 1 : 0;
	
}
int CoreServerShell::OnSetGateWay(LPVOID pServer)
{
	//g_SetGateWayServer(pServer);
	g_pGateWayClient= reinterpret_cast<IClient *>(pServer);
	return true;

}

//void g_SetTranServer(LPVOID pServer)
//{
//	g_pTranClient= reinterpret_cast<IClient *>(pServer);
//}
//
//void g_SetChatServer(LPVOID pServer)
//{
//	g_pChatClient= reinterpret_cast<IClient *>(pServer);
//}
//
//void g_SetTonServer(LPVOID pServer)
//{
//	g_pTongClient= reinterpret_cast<IClient *>(pServer);
//
//}
//
//void g_SetDataServer(LPVOID pServer)
//{
//	g_pDataClient= reinterpret_cast<IClient *>(pServer);
//
//}
//
//void g_SetGateWayServer(LPVOID pServer)
//{
//	g_pGateWayClient= reinterpret_cast<IClient *>(pServer);
//}



//启动服务器执行脚本
int CoreServerShell::OnLunch(LPVOID pServer)
{      
	g_SetServer(pServer);

	g_SetFilePath("\\script");
	KLuaScript * pStartScript =(KLuaScript*)g_GetScript("\\script\\system\\serverscript.lua");
	int i = 0;
	
	if (!pStartScript)
		printf("Load ServerScript failed!,脚本加载失败!\n");
	else
	{	
		pStartScript->CallFunction("StartGame",0,"");
	}

	//PlayerSet.ReloadWelcomeMsg(); //发送欢迎语

	return true;
}

int CoreServerShell::OnShutdown()
{
	return true;
}
#define		GAME_FPS					18
//服务端日常活动，core如果要寿终正寝则返回0，否则返回非0值
int CoreServerShell::Breathe()
{
	static bool s_bServerTimerWarned = false;
	KLuaScript * pStartScript =(KLuaScript*)g_GetScript("\\script\\servertimer.lua");
	if (pStartScript)
	{
		if(g_SubWorldSet.GetGameTime()%GAME_FPS==0)
		{
			SYSTEMTIME aSysTime;
			GetSystemTime(&aSysTime);
			if(aSysTime.wSecond ==0)
			{
				pStartScript->CallFunction("Main",0,"");
			}
		}
	}
	else if (!s_bServerTimerWarned)
	{
		s_bServerTimerWarned = true;
		printf("Load servertimer.lua failed!!\n");
	}

	//g_SubWorldSet.MessageLoop();  //地图消息处理循环
	g_SubWorldSet.MainLoop();       //主循环
	return true;
}

bool CoreServerShell::CheckProtocolSize(const char* pChar, int nSize)
{
	WORD wCheckSize;
	BYTE nProtocol = (BYTE)pChar[0];

	if (nProtocol >= c2s_end || nProtocol <= c2s_gameserverbegin)
	{
		printf("[error]错误的协议序号!\n");
		return false;
	}

	if (g_nProtocolSize[nProtocol - c2s_gameserverbegin - 1] == -1)
	{
		wCheckSize = *(WORD*)&pChar[1] + PROTOCOL_MSG_SIZE;
	}
	else
	{
		wCheckSize = g_nProtocolSize[nProtocol - c2s_gameserverbegin - 1];  //181-64=117
	}

	if (wCheckSize != nSize)
	{

//#ifndef _WIN32
		printf("[Canh Bao] wCheckSize != nSize:  nProtocol: %d, wCheckSize: %d, nSize: %d\n", nProtocol, wCheckSize, nSize);
//#endif
		return false;
	}

	return true;
}

//设置玩家的 网络连接号 和存档时间
int CoreServerShell::AttachPlayer(const unsigned long lnID,GUID* pGuid,int model)
{
	return PlayerSet.AttachPlayer(lnID,pGuid,model);
}
//BISOP踢掉游戏玩家 或 是GS 存档时调用 时调用 立即保存玩家档
void* CoreServerShell::SavePlayerDataAtOnce(int nIndex)
{
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
	{
		return NULL;
	}
/*	int nWorldMapid[1]; 
	     nWorldMapid[0] = SubWorld[Npc[Player[nIndex].m_nIndex].m_SubWorldIndex].m_SubWorldID;
	
    int nReg = NpcSet.CheckForBit(nWorldMapid,"MapID");
	
    if (nReg)
	{
		Player[nIndex].SetLoginType(1);
        Player[nIndex].SetRevivalPos(53,19);
	}

	if (Player[nIndex].Save())	                    //这里还需要生成数据包吗？
	{//生成数据包
		Player[nIndex].m_uMustSave = SAVE_REQUEST;	//立即存档
		return &Player[nIndex].m_GaemSaveBuffer;	//存档新包
	}
	else
	{
		Player[nIndex].m_uMustSave = SAVE_REQUEST;	//立即存档
		return &Player[nIndex].m_GaemSaveBuffer;	//存档老包
	} 
  */
     return &Player[nIndex].m_GaemSaveBuffer;	    //存档新包
}
//离线
void CoreServerShell::SetCharacterLiXian(int nIndex,int i)
{
 	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
 	{
 		return;
 	}
	
	Player[nIndex].m_nLiXian = i;
	Player[nIndex].m_nForbiddenFlag=0;   //解除禁言
	//NpcSet.SearchID()
}


void CoreServerShell::SetAttackMsg(void *pdata,int nType)
{
   PlayerSet.SendAttackMsg(pdata,nType);
}

void CoreServerShell::SetAttackCamp(char * nTongName,int nType)
{
	//NpcSet.SearchID(nNpcDwId);//int	KNpcSet::SearchByTongName(LPSTR szName)
	PlayerSet.FindByTongName(nTongName); //恢复阵营

	/*char nNmae[32];
	sprintf(nNmae,nTongName);*/

	STONG_ATTACK_SENDBACK  nTemp;
	ZeroMemory(&nTemp,sizeof(STONG_ATTACK_SENDBACK));
	snprintf(nTemp.m_szAttackName, sizeof(nTemp.m_szAttackName), "%s", nTongName);

	if (nType)
	   PlayerSet.SendAttackMsg(&nTemp,3);
	else
	   PlayerSet.SendAttackMsg(&nTemp,1);

}

int  CoreServerShell::SetAttackWarCityInfo(const char * pMsg,int nPlayerIdx)
{
	if  (pMsg)
	    Player[nPlayerIdx].m_cTong.SetAttAckCityInfo(pMsg);
	return 1;
}


int CoreServerShell::SetAttackInfo(const char * pMsg,int nPlayerIdx)
{
	// 	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
	// 	{
	// 		return;
	// 	}	
	//Player[nIndex].m_nLiXian = i;
	//NpcSet.SearchID(nNpcDwId);
	STONG_ATTACK_INFO_SYNC	*pSync = (STONG_ATTACK_INFO_SYNC*)pMsg; //接收数据
	 /*
	 name=凤翔
	 idx=2
	 mapidx=1
	 tongmaster=暂无帮会
	 mastername=暂无
	 levle=1
shushou=0
	
	
	*/

/*	
	for (int i=0;i<pSync->m_WarCityCount;++i)
	{
		printf("-接收城市(%d)信息成功(名称:%s,索引:%d,编号:%d,帮会:%s,城主:%s,等级:%d,税收:%d)!-\n",i,pSync->snWarInfo[i].m_szMapName,pSync->snWarInfo[i].m_idx,pSync->snWarInfo[i].m_mapidx,pSync->snWarInfo[i].m_Tongmaster,pSync->snWarInfo[i].m_Mastername,pSync->snWarInfo[i].m_levle,pSync->snWarInfo[i].m_shushou);

	} 

	int nNpcIdx=NpcSet.SearchID(pSync->m_dwNpcID);//PlayerSet.FindSame(nNpcDwId);

	int nPlayerIdx=0;

	 if (nNpcIdx>0)
	     nPlayerIdx=Npc[nNpcIdx].GetPlayerIdx();
  */
	if (nPlayerIdx>0)
	{
	   Player[nPlayerIdx].m_cTong.SetAttAckInfo(pMsg);
	   Player[nPlayerIdx].m_cTong.m_AttackState = pSync->m_AttackState;

	   if  (pSync->m_AttackState>0)
	   {
	       Player[nPlayerIdx].m_cTong.m_AttackCamp =  pSync->m_nTempCamp;

		  if (pSync->m_szAttackName) 
		   snprintf(Player[nPlayerIdx].m_cTong.m_AttackName, sizeof(Player[nPlayerIdx].m_cTong.m_AttackName), "%s", pSync->m_szAttackName);
		   
		   if ( pSync->m_AttackState==1)
		   {
			   Npc[Player[nPlayerIdx].m_nIndex].m_CurrentCamp=7;
		       Npc[Player[nPlayerIdx].m_nIndex].SetCurrentCamp(7);
		   }
		   if ( pSync->m_AttackState==2)
		   {
			  Npc[Player[nPlayerIdx].m_nIndex].m_CurrentCamp= 8;
		      Npc[Player[nPlayerIdx].m_nIndex].SetCurrentCamp(8);
		   }
		   /*char nMsg[64];
		   sprintf(nMsg,"状态:%d,宣战阵营:%d,人物阵营:%d",nAttackState,Player[nPlayerIdx].m_cTong.m_AttackCamp,Npc[nNpcIdx].m_CurrentCamp);
		   Player[nPlayerIdx].m_ItemList.msgshow(nMsg);	*/
	   }

	return Player[nPlayerIdx].m_nNetConnectIdx;
	}

	return -1;
}


int CoreServerShell::IsCharacterLiXian(int nIndex)
{
 	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
 		return FALSE;

	return Player[nIndex].m_nLiXian;
}

bool CoreServerShell::IsCharacterQuiting(int nIndex)
{
	if (!Player || nIndex <= 0 || nIndex >= MAX_PLAYER)
		return FALSE;

	return Player[nIndex].IsWaitingRemove();
}

//获取GoldMissid 值
int  CoreServerShell::GetGoldMissidVal(int nGoldMid)
{
	 return g_GlobalMissionArray.GetMissionValue(nGoldMid);
}

bool CoreServerShell::IsShutdownClient(int nIndex)
{
	if (!Player || nIndex <= 0 || nIndex >= MAX_PLAYER)
		return FALSE;

	if (Player[nIndex].m_nNetConnectIdx>= 0)
	{
		try
		{
			if (g_pServer)
			   g_pServer->ShutdownClient(Player[nIndex].m_nNetConnectIdx);
		}
		catch (...)
		{
			return FALSE;
		}
	  
	   return TRUE;
	}

	return FALSE;
}

bool CoreServerShell::IsShutdownClientOutserver(int nIndex)
{
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return FALSE;

	if (Player[nIndex].m_nNetConnectIdx>= 0)
	{
		try
		{
			if (g_pServer)
			{
				//if  (Player[nIndex].m_CurSerVer==nCurSerIdx) //不是这个服务器的全部踢掉
				{
					if (Player[nIndex].getMobileClient())
					{
						printf("GM Kick out player one from Mobile.\n");
						Player[nIndex].KickOutGame();
					}
					else
						g_pServer->ShutdownClient(Player[nIndex].m_nNetConnectIdx);
				}
			}
		}
		catch (...)
		{
			return FALSE;
		}

		return TRUE;
	}

	return FALSE;
}

bool CoreServerShell::IsShutdownClientNoserver(int nIndex)
{
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return FALSE;


	if (Player[nIndex].m_nNetConnectIdx>= 0)
	{
		try
		{
			if (g_pServer)
			{
				//if  (Player[nIndex].m_CurSerVer!=nCurSerIdx) //不是这个服务器的全部踢掉
				{
					if (Player[nIndex].getMobileClient())
					{
						printf("GM Kick out player one from Mobile.\n");
						Player[nIndex].KickOutGame();
					}
					else
				      g_pServer->ShutdownClient(Player[nIndex].m_nNetConnectIdx);
				}
			}
		}
		catch (...)
		{
			return FALSE;
		}

		return TRUE;
	}

	return FALSE;
}

int CoreServerShell::GetClientNetConnectIdx(int nIndex)
{
	if (!Player || nIndex <= 0 || nIndex >= MAX_PLAYER)
		return -1;

	return Player[nIndex].m_nNetConnectIdx;
}



bool CoreServerShell::IsPlayerLoginTimeOut(int nIndex)
{
	if (!Player || nIndex <= 0 || nIndex >= MAX_PLAYER)
		return false;

	return Player[nIndex].IsLoginTimeOut();
}

void CoreServerShell::RemovePlayerLoginTimeOut(int nIndex)
{
	if (!Player || nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;

	if (Player[nIndex].IsLoginTimeOut())
		PlayerSet.RemoveLoginTimeOut(nIndex);
}

 /*
int CoreServerShell::AddTempTaskValue(int nIndex, const char* pData)
{
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return 0;

	return Player[nIndex].AddTempTaskValue((void *)pData);
}  */
//获取网络号
void CoreServerShell::GetPlayerIndexByGuid(GUID* pGuid, int* pnIndex, int* plnID)
{
	*pnIndex = PlayerSet.GetPlayerIndexByGuid(pGuid);

	if (*pnIndex)
	{
		*plnID = Player[*pnIndex].m_nNetConnectIdx;
	}
	else
	{
		*plnID = -1;
	}

	if (*plnID == -1)
	{
		*pnIndex = 0;
	}
}
 //跨服
void* CoreServerShell::PreparePlayerForExchange(int nIndex)
{
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return NULL;

	PlayerSet.PrepareExchange(nIndex);         //生成新的数据包

	return &Player[nIndex].m_GaemSaveBuffer;   //返回新的存档数据包
}

int CoreServerShell::serverKickOutGame(int nIndex)
{
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return false;
	Player[nIndex].KickOutGame();
	return true;
}


int CoreServerShell::getClientIsMobile(int nIndex)
{
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return false;
	return Player[nIndex].getMobileClient();
}

bool CoreServerShell::IsPlayerExchangingServer(int nIndex)
{
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return false;

	return Player[nIndex].IsExchangingServer();
}
//通知离开游戏时调用
void CoreServerShell::RemovePlayerForExchange(int nIndex)
{
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;

	PlayerSet.RemoveExchanging(nIndex);
}

void CoreServerShell::GetGuid(int nIndex, void* pGuid)
{
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	memcpy(pGuid, &Player[nIndex].m_Guid, sizeof(GUID));
}

UINT CoreServerShell::GetExchangeMap(int nIndex)
{
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return -1;

	return Player[nIndex].m_sExchangePos.m_dwMapID;
}
//取消跨服状态
void CoreServerShell::RecoverPlayerExchange(int nIndex)
{
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;

	Player[nIndex].m_bExchangeServer = FALSE;

	if (Player[nIndex].m_nIndex > 0)
	{
		KNpc* pNpc = &Npc[Player[nIndex].m_nIndex];
		pNpc->m_bExchangeServer = FALSE;
		pNpc->m_FightMode = pNpc->m_OldFightMode;
	}

	Player[nIndex].Earn(Player[nIndex].m_nPrePayMoney);
	Player[nIndex].m_nPrePayMoney = 0;
}

void CoreServerShell::SetSaveStatus(char *strName, UINT uStatus)
{
	if  (!strName) return;

	int  nIndex = PlayerSet.FindNameID(strName);

	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;

	Player[nIndex].m_uMustSave = uStatus;
}
//是否马上存档
void CoreServerShell::SetSaveStatus(int nIndex, UINT uStatus)
{
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;

	Player[nIndex].m_uMustSave = uStatus;

}

UINT CoreServerShell::GetSaveStatus(int nIndex)
{
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return SAVE_IDLE; //不存档

	return Player[nIndex].m_uMustSave;
}

void CoreServerShell::SetChangeRoleStatus(int nIndex, int uStatus,char *nRoleName)
{
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	
	KNpc* pNpc = &Npc[Player[nIndex].m_nIndex];

	if  (uStatus==1)
	{//存在了
	   //pNpc->SetstrInfo(STR_CHECK_NAME_RET,"是");
		Player[nIndex].m_ItemList.msgshow("提示:该角色名已经存在,不能更改!");
		return;
	}
	/*else
	{//没有存在
		pNpc->SetstrInfo(STR_CHECK_NAME_RET,"否");
	}*/

	int nLen = strlen(nRoleName);
	
	if (nLen > NAME_LENGHT);
	{   
		Player[nIndex].m_ItemList.msgshow("提示:<color=gyellow>该角色名长度不符合规则,不能更改!<color>");
		return;
	}

	pNpc->SetstrInfo(STR_ROLE_CHANGENAME,nRoleName);
	pNpc->m_RestNameCount ++;

	Player[nIndex].m_ItemList.msgshow("提示:<color=gyellow>改角色名成功!<color>");

	//g_GameSetTing.GetInteger2()

	if (Player[nIndex].m_nNetConnectIdx >= 0)
	{
		printf("GM Kick out changerolename player one.\n");
		g_pServer->ShutdownClient(Player[nIndex].m_nNetConnectIdx);
	}

}



//登陆失败
void CoreServerShell::PreparePlayerForLoginFailed(int nIndex)
{
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;

	PlayerSet.PrepareLoginFailed(nIndex);
}
//BOOL CoreServerShell::ValidPingTime(int nIndex)
//{
//	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
//		return FALSE;
//
//	if (Player[nIndex].m_uLastPingTime == -1)
//		return TRUE;
//	
//#define	MAX_PING_TIME	(60 * 20)	//	1min
//	if (g_SubWorldSet.GetGameTime() - Player[nIndex].m_uLastPingTime > MAX_PING_TIME)
//	{
//		return FALSE;
//	}
//	return TRUE;
//}
//频道消息聊天
BOOL CoreServerShell::GroupChat(IClient* pClient, DWORD FromIP, unsigned long FromRelayID, DWORD channid, BYTE tgtcls, DWORD tgtid, const void* pData, size_t size, char* szSender)
{
	switch(tgtcls)
	{

	case tgtcls_team:
		{{
		if (tgtid < 0 || tgtid >= MAX_TEAM)
			return FALSE;

		size_t pckgsize = sizeof(tagExtendProtoHeader) + size;
#ifdef WIN32
		tagExtendProtoHeader* pExHeader = (tagExtendProtoHeader*)_alloca(pckgsize);
#else
		tagExtendProtoHeader* pExHeader = (tagExtendProtoHeader*)(new char[pckgsize]);
#endif
		pExHeader->ProtocolType = s2c_extendchat;
		pExHeader->wLength = pckgsize - 1;
		memcpy(pExHeader + 1, pData, size);

		int nTargetIdx;
		// 给队长发
		nTargetIdx = g_Team[tgtid].m_nCaptain;
//		if (FromRelayID != Player[nTargetIdx].m_nNetConnectIdx)
		if(!Player[nTargetIdx].m_cChat.OnBlackList(szSender))
			g_pServer->SendData(Player[nTargetIdx].m_nNetConnectIdx, pData, size);
		// 给队员发
		for (int i = 0; i <	MAX_TEAM_MEMBER; ++i)
		{
			nTargetIdx = g_Team[tgtid].m_nMember[i];
			if (nTargetIdx < 0)
				continue;

//			if (FromRelayID != Player[nTargetIdx].m_nNetConnectIdx)
			if (!Player[nTargetIdx].m_cChat.OnBlackList(szSender))
				g_pServer->PackDataToClient(Player[nTargetIdx].m_nNetConnectIdx, pExHeader, pckgsize);
		}
#ifndef WIN32
		delete ((char*)pExHeader);
        (char*)pExHeader=NULL;
#endif
		}}
		break;

	case tgtcls_fac:
		{{
		size_t pckgsize = sizeof(tagExtendProtoHeader) + size;
#ifdef WIN32
		tagExtendProtoHeader* pExHeader = (tagExtendProtoHeader*)_alloca(pckgsize);
#else
		tagExtendProtoHeader* pExHeader = (tagExtendProtoHeader*)(new char[pckgsize]);
#endif
		pExHeader->ProtocolType = s2c_extendchat;
		pExHeader->wLength = pckgsize - 1;
		memcpy(pExHeader + 1, pData, size);

		int nTargetIdx;
		nTargetIdx = PlayerSet.GetFirstPlayer();
		while (nTargetIdx)
		{
			if (Player[nTargetIdx].m_cFaction.m_nCurFaction >= 0 && Player[nTargetIdx].m_cFaction.m_nCurFaction == tgtid && !Player[nTargetIdx].m_cChat.OnBlackList(szSender))//&& FromRelayID != Player[nTargetIdx].m_nNetConnectIdx   (>= 0 because -1 is this field's 'no faction' value and the wire's tgtid can be 0xFFFFFFFF, the same bits)
				g_pServer->PackDataToClient(Player[nTargetIdx].m_nNetConnectIdx, pExHeader, pckgsize);

			nTargetIdx = PlayerSet.GetNextPlayer();
		}
#ifndef WIN32
		delete ((char*)pExHeader);
		(char*)pExHeader=NULL;
#endif
		}}
		break;

	case tgtcls_tong:
		{{
		size_t pckgsize = sizeof(tagExtendProtoHeader) + size;
#ifdef WIN32
		tagExtendProtoHeader* pExHeader = (tagExtendProtoHeader*)_alloca(pckgsize);
#else
		tagExtendProtoHeader* pExHeader = (tagExtendProtoHeader*)(new char[pckgsize]);
#endif
		pExHeader->ProtocolType = s2c_extendchat;
		pExHeader->wLength = pckgsize - 1;
		memcpy(pExHeader + 1, pData, size);

		int nTargetIdx;
		nTargetIdx = PlayerSet.GetFirstPlayer();
		while (nTargetIdx)
		{
			if (Player[nTargetIdx].m_cTong.GetTongNameID() == tgtid && !Player[nTargetIdx].m_cChat.OnBlackList(szSender))//				&& FromRelayID != Player[nTargetIdx].m_nNetConnectIdx)
				g_pServer->PackDataToClient(Player[nTargetIdx].m_nNetConnectIdx, pExHeader, pckgsize);

			nTargetIdx = PlayerSet.GetNextPlayer();
		}
#ifndef WIN32
		delete ((char*)pExHeader);
		(char*)pExHeader=NULL;
#endif
		}}
		break;
	case tgtcls_msgr:
		{{
			size_t pckgsize = sizeof(tagExtendProtoHeader) + size;
#ifdef WIN32
			tagExtendProtoHeader* pExHeader = (tagExtendProtoHeader*)_alloca(pckgsize);
#else
			tagExtendProtoHeader* pExHeader = (tagExtendProtoHeader*)(new char[pckgsize]);
#endif
			pExHeader->ProtocolType = s2c_extendchat;
			pExHeader->wLength = pckgsize - 1;
			memcpy(pExHeader + 1, pData, size);
			
			int nTargetIdx;
			nTargetIdx = PlayerSet.GetFirstPlayer();
			while (nTargetIdx)
			{
				if (Npc[Player[nTargetIdx].m_nIndex].m_nMissionGroup >= 0 && Npc[Player[nTargetIdx].m_nIndex].m_nMissionGroup == tgtid && !Player[nTargetIdx].m_cChat.OnBlackList(szSender))  // -1 (KNpc.cpp:201) is 'no group' and a wire tgtid of 0xFFFFFFFF is the same bits once converted
					g_pServer->PackDataToClient(Player[nTargetIdx].m_nNetConnectIdx, pExHeader, pckgsize);
				nTargetIdx = PlayerSet.GetNextPlayer();
			}
#ifndef WIN32
			delete ((char*)pExHeader);
#endif
		}}
		break;

	case tgtcls_scrn:
		{{//附近聊天 - nearby

//		int nMaxRelayPlayer = (1024 - 32 - sizeof(CHAT_GROUPMAN) - size) / sizeof(WORD);
//		if (nMaxRelayPlayer <= 0)
//			return FALSE;


		int idxNPC = Player[tgtid].m_nIndex;
		int idxSubWorld = Npc[idxNPC].m_SubWorldIndex;
		int idxRegion = Npc[idxNPC].m_RegionIndex;
//		_ASSERT(idxSubWorld >= 0 && idxRegion >= 0);
		int nOX = Npc[idxNPC].m_MapX;
		int nOY = Npc[idxNPC].m_MapY;
		int nTX = 0;
		int nTY = 0;
		if (idxSubWorld < 0 || idxRegion < 0)
			return FALSE;


//		size_t basesize = sizeof(CHAT_GROUPMAN) + size;
//		BYTE buffer[1024];
//
//		CHAT_GROUPMAN* pCgc = (CHAT_GROUPMAN*)buffer;
//		pCgc->ProtocolType = chat_groupman;
//		pCgc->wChatLength = size;
//		pCgc->byHasIdentify = false;
//
//		void* pExPckg = pCgc + 1;
//		memcpy(pExPckg, pData, size);
//
//		WORD* pPlayers = (WORD*)((BYTE*)pExPckg + size);
//
//
//		pCgc->wPlayerCount = 0;


		size_t pckgsize = sizeof(tagExtendProtoHeader) + size;
#ifdef WIN32
		tagExtendProtoHeader* pExHeader = (tagExtendProtoHeader*)_alloca(pckgsize);
#else
		tagExtendProtoHeader* pExHeader = (tagExtendProtoHeader*)(new char[pckgsize]);
#endif
		pExHeader->ProtocolType = s2c_extendchat;
		pExHeader->wLength = pckgsize - 1;
		memcpy(pExHeader + 1, pData, size);

#define	MAX_SYNC_RANGE	23
		static POINT POff[9] = 
		{
			{0, 0},
			{0, 32},
			{-16, 32},
			{-16, 0},
			{-16, -32},
			{0, -32},
			{16, -32},
			{16, 0},
			{16, 32},
		};

		KRegion* pRegionBase = &SubWorld[idxSubWorld].m_Region[idxRegion];

		for (int i = -1; i < 8; ++i)
		{
			KRegion* pRegion = NULL;
			if (i < 0)
				pRegion = pRegionBase;
			else
			{
				if (pRegionBase->m_nConnectRegion[i] < 0)
					continue;
				pRegion = &SubWorld[idxSubWorld].m_Region[pRegionBase->m_nConnectRegion[i]];
			}
			if (pRegion == NULL)
				continue;


			KIndexNode *pNode = (KIndexNode *)pRegion->m_PlayerList.GetHead();
			while(pNode)
			{
//				_ASSERT(pNode->m_nIndex > 0 && pNode->m_nIndex < MAX_PLAYER);

				//if (FromRelayID != Player[pNode->m_nIndex].m_nNetConnectIdx)
				{
					int nTargetNpc = Player[pNode->m_nIndex].m_nIndex;
					if (nTargetNpc > 0)
					{
						nTX = Npc[nTargetNpc].m_MapX + POff[i + 1].x;
						nTY = Npc[nTargetNpc].m_MapY + POff[i + 1].y;
						
						if ((nTX - nOX) * (nTX - nOX) + (nTY - nOY) * (nTY - nOY) < MAX_SYNC_RANGE * MAX_SYNC_RANGE)
							if(!Player[pNode->m_nIndex].m_cChat.OnBlackList(szSender))
								g_pServer->PackDataToClient(Player[pNode->m_nIndex].m_nNetConnectIdx, pExHeader, pckgsize);
					}


//					pPlayers[pCgc->wPlayerCount] = (WORD)Player[pNode->m_nIndex].m_nNetConnectIdx;
//					++ pCgc->wPlayerCount;
//
//					if (pCgc->wPlayerCount >= nMaxRelayPlayer)
//					{
//						size_t pckgsize = basesize + sizeof(WORD) * pCgc->wPlayerCount;
//						pCgc->wSize = pckgsize - 1;
//						
//						pClient->SendPackToServer(pCgc, pckgsize);
//
//						pCgc->wPlayerCount = 0;
//					}
				}

				pNode = (KIndexNode *)pNode->GetNext();
			}
		}

//		if (pCgc->wPlayerCount > 0)
//		{
//			size_t pckgsize = basesize + sizeof(WORD) * pCgc->wPlayerCount;
//			pCgc->wSize = pckgsize - 1;
//
//			pClient->SendPackToServer(pCgc, pckgsize);
//		}

#ifndef WIN32
		delete (char*)pExHeader;
		(char*)pExHeader=NULL;
#endif
		}}
		break;


	case tgtcls_bc:
		{{
		size_t pckgsize = sizeof(tagExtendProtoHeader) + size;
#ifdef WIN32
		tagExtendProtoHeader* pExHeader = (tagExtendProtoHeader*)_alloca(pckgsize);
#else
		tagExtendProtoHeader* pExHeader = (tagExtendProtoHeader*)(new char[pckgsize]);
#endif
		pExHeader->ProtocolType = s2c_extendchat;
		pExHeader->wLength = pckgsize - 1;
		memcpy(pExHeader + 1, pData, size);

		int nTargetIdx;
		    nTargetIdx = PlayerSet.GetFirstPlayer();
		while (nTargetIdx)
		{
			if (!Player[nTargetIdx].m_cChat.OnBlackList(szSender))
				g_pServer->PackDataToClient(Player[nTargetIdx].m_nNetConnectIdx, pExHeader, pckgsize);

			nTargetIdx = PlayerSet.GetNextPlayer();
		}
#ifndef WIN32
		delete ((char*)pExHeader);
		(char*)pExHeader=NULL;
#endif
		}}
		break;


	default:
		break;
	}
	return TRUE;
}

void CoreServerShell::sendToClient_globalNews(const char *pData)
{
	tagMsgInGame	*pGatewayBroadCast;
	pGatewayBroadCast = (tagMsgInGame *)pData;


	PLAYER_SCRIPTACTION_SYNC UiInfo;
	ZeroStruct(UiInfo);
	UiInfo.m_bUIId = UI_NEWSINFO;
	UiInfo.m_bOptionNum = NEWSMESSAGE_NORMAL;
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;

	if (pGatewayBroadCast->nMgsIdx!=-1)
	{//数字
		*((int *)(UiInfo.m_pContent)) = pGatewayBroadCast->nMgsIdx;
		UiInfo.m_bParam1 = 1;
		UiInfo.m_nBufferLen = sizeof(int);
	}
	else 
	{//字符串
		g_StrCpyLen(UiInfo.m_pContent,pGatewayBroadCast->strMsg, 256);
		UiInfo.m_nBufferLen = strlen(((char *)UiInfo.m_pContent));
		UiInfo.m_bParam1 = 0;
	}
	
	/*UiInfo.m_bParam2 = 1;
	UiInfo.ProtocolType = (BYTE)s2c_scriptaction;
	UiInfo.m_wProtocolLong = sizeof(PLAYER_SCRIPTACTION_SYNC) - MAX_SCIRPTACTION_BUFFERNUM + UiInfo.m_nBufferLen - 1;
	g_NewProtocolProcess.BroadcastGlobal(&UiInfo, UiInfo.m_wProtocolLong + 1);*/

	UiInfo.m_bParam2 = 1;
	UiInfo.ProtocolType = (BYTE)s2c_scriptaction;
	UiInfo.m_wProtocolLong = sizeof(PLAYER_SCRIPTACTION_SYNC) - MAX_SCIRPTACTION_BUFFERNUM + UiInfo.m_nBufferLen - 1;
	g_NewProtocolProcess.BroadcastLocalServer(&UiInfo, UiInfo.m_wProtocolLong + 1);

	return;
}

void CoreServerShell::sendToClient_localNews(const char *pData)
{
	tagMsgInGame	*pGatewayBroadCast;
	pGatewayBroadCast = (tagMsgInGame *)pData;

	PLAYER_SCRIPTACTION_SYNC UiInfo;
	ZeroStruct(UiInfo);
	UiInfo.m_bUIId = UI_NEWSINFO;
	UiInfo.m_bOptionNum = NEWSMESSAGE_NORMAL;  //普通消息 显示就消亡
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;  //显示UI界面

	if (pGatewayBroadCast->nMgsIdx!=-1)
	{
		*((int *)(UiInfo.m_pContent)) = pGatewayBroadCast->nMgsIdx;
		UiInfo.m_bParam1 = 1;
		UiInfo.m_nBufferLen = sizeof(int);
	}
	else 
	{
		g_StrCpyLen(UiInfo.m_pContent,pGatewayBroadCast->strMsg, 256);
		UiInfo.m_nBufferLen = strlen(((char *)UiInfo.m_pContent));
		UiInfo.m_bParam1 = 0;
	}

	UiInfo.m_bParam2 = 1;
	UiInfo.ProtocolType = (BYTE)s2c_scriptaction;
	UiInfo.m_wProtocolLong = sizeof(PLAYER_SCRIPTACTION_SYNC) - MAX_SCIRPTACTION_BUFFERNUM + UiInfo.m_nBufferLen - 1;
	g_NewProtocolProcess.BroadcastLocalServer(&UiInfo, UiInfo.m_wProtocolLong + 1);

	return ;
}

void CoreServerShell:: sendToClient_globalCountNews(const char *pData)
{
	tagMsgInGame	*pGatewayBroadCast;
	pGatewayBroadCast = (tagMsgInGame *)pData;

	PLAYER_SCRIPTACTION_SYNC UiInfo;
	ZeroStruct(UiInfo);
	UiInfo.m_bUIId = UI_NEWSINFO;
	UiInfo.m_bOptionNum = NEWSMESSAGE_COUNTING;  //倒计时消息
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;

	int nTime =pGatewayBroadCast->nValIdx;

	if (nTime <= 0)
		nTime = 1;

	if (pGatewayBroadCast->nMgsIdx!=-1)
	{
		*((int *)(UiInfo.m_pContent)) = pGatewayBroadCast->nMgsIdx;
		UiInfo.m_bParam1 = 1;
		*(int *)((char *)UiInfo.m_pContent + sizeof(int)) = nTime;
		UiInfo.m_nBufferLen = sizeof(int) * 2;
	}
	else 
	{
		g_StrCpyLen(UiInfo.m_pContent,pGatewayBroadCast->strMsg, 256);
		UiInfo.m_nBufferLen = strlen(((char *)UiInfo.m_pContent));
		*(int *)((char *)UiInfo.m_pContent + UiInfo.m_nBufferLen) = nTime;
		UiInfo.m_nBufferLen += sizeof(int);
		UiInfo.m_bParam1 = 0;
	}

	//UiInfo.m_bParam2 = 1;
	//UiInfo.ProtocolType = (BYTE)s2c_scriptaction;   //显示UI的协议
	//UiInfo.m_wProtocolLong = sizeof(PLAYER_SCRIPTACTION_SYNC) - MAX_SCIRPTACTION_BUFFERNUM + UiInfo.m_nBufferLen - 1;
	//g_NewProtocolProcess.BroadcastGlobal(&UiInfo, UiInfo.m_wProtocolLong + 1);


	UiInfo.m_bParam2 = 1;
	UiInfo.ProtocolType = (BYTE)s2c_scriptaction;
	UiInfo.m_wProtocolLong = sizeof(PLAYER_SCRIPTACTION_SYNC) - MAX_SCIRPTACTION_BUFFERNUM + UiInfo.m_nBufferLen - 1;
	g_NewProtocolProcess.BroadcastLocalServer(&UiInfo, UiInfo.m_wProtocolLong + 1);

	return ;
}


//设置排名
void CoreServerShell::SetLadder(void* pData, size_t uSize)
{
	Ladder.Init(pData, uSize);
}

void CoreServerShell::ExeCoreNpcState()
{
	if (!Player) return;
	for (int i = 0; i < MAX_SUBWORLD; i++)       //服务器端 地图列表
	{
		SubWorld[i].stateActivate();
	}
}

void CoreServerShell::ExeCoreTask()
{
	if (!Player) return;
	//g_SubWorldSet.TaskMainLoop(); //取消
	//Concurrency::parallel_for(1,MAX_PLAYER,[&](int i)
	for (int i=1;i<MAX_PLAYER;i++)
	{  
	  if (Player[i].m_isInGame && Player[i].m_nNetConnectIdx>=0)
	  {//在线的 
    //      if (Player[i].m_nVipType==0)
		  //{
			 // if (Player[i].m_GameLeftTime>0)
			 // { //包月
				//	  Player[i].m_GameLeftTime--;

				//	  if ( Player[i].m_GameLeftTime<=0)
				//		   Player[i].m_Isinvalid = TRUE;
				//	  else
				//		   Player[i].m_Isinvalid = FALSE;

				//	  if ( Player[i].m_GameLeftTime<=540)
				//	  {
				//		  char nMsg[64]={0};
				//		  sprintf(nMsg,"提示:<color=gyellow>在游戏中还可以在线 %d<color> 秒!",Player[i].m_GameLeftTime);
				//		  Player[i].m_ItemList.msgshow(nMsg);
				//	  }
			 // }
			 // else
				//   Player[i].m_Isinvalid = TRUE;

			 // if  ( Player[i].m_Isinvalid)
			 // {//发送时间到的提示
				//  if (g_pServer)
				//     g_pServer->ShutdownClient( Player[i].m_nNetConnectIdx);
			 // }
		  //}

		  /*if (Player[i].m_uNextSaveTime<=0)
             Player[i].m_uNextSaveTime = g_SubWorldSet.m_nLoopRate;

		  if (Player[i].CanSave() && g_SubWorldSet.m_nLoopRate - Player[i].m_uNextSaveTime >= defPLAYER_SAVE_TIMEOUT)
		  {//个人存档时间 有可能存空档的BUG
			  if (Player[i].Save())
			  { //生成数据包
					//Player[i].m_uMustSave = SAVE_REQUEST;
					Player[i].m_uNextSaveTime = g_SubWorldSet.m_nLoopRate;
					Player[i].SavePlayerData(false);	
			  }
		  }*/
		  //离线挂机
		  if (Player[i].m_nLiXian==2)
		  {
			  if  (Player[i].m_uMustSave==SAVE_KICKOUT)
			  {//已经 是设置退出网关状态就不执行了
				  Player[i].m_nLiXian = 0;
				  Player[i].m_nForbFlag = 0;
				  Player[i].Save();
				  Player[i].m_uMustSave = SAVE_DOING;	
				  if (g_pDataClient)
				  {//存档
					  Player[i].SavePlayerData(false);   //参数　存档后是否反馈给ｇｓ
				  }
				  //直接退出游戏
				  tagLeaveGame	sLeaveGame;	
				  sLeaveGame.cProtocol = c2s_leavegame;   // 发送往bishop 通知 paysys
				  sLeaveGame.cCmdType  = NORMAL_LEAVEGAME;

				  tagLeaveGame2 lg2;

				  lg2.ProtocolFamily = pf_normal;
				  lg2.ProtocolID     = c2s_leavegame;	   // S3 玩家离开游戏
				  lg2.cCmdType       = NORMAL_LEAVEGAME;
				  lg2.nSelServer     = nCurSerIdx;         // MAKELONG(m_GsNumBer, m_SerVerNo);

				   char szName[ACC_LENGTH]={0},szRoleName[NAME_LENGHT]={0};
				  // GetGameData(SGDI_CHARACTER_ACCOUNT, (unsigned int)szName, i);
				   GetGameDataInfo(SGDI_CHARACTER_ACCOUNT, 0, i,szName);
				   sLeaveGame.nExtPoint    = GetExpData(i,0);
				   sLeaveGame.nGameExtTime  = GetExpData(i,1);
				   //GetGameData(SGDI_CHARACTER_NAME, (unsigned int)szRoleName, i);
				    GetGameDataInfo(SGDI_CHARACTER_NAME, 0, i,szRoleName);

				   snprintf((char *)sLeaveGame.nClientIp, sizeof(sLeaveGame.nClientIp), "%s", (char *)szRoleName);
				   snprintf((char *)sLeaveGame.szAccountName, sizeof(sLeaveGame.szAccountName), "%s", (char *)szName);

				   if (g_pGateWayClient)
					   g_pGateWayClient->SendPackToServer(&sLeaveGame, sizeof(tagLeaveGame)); //通知bishop

				   snprintf((char *)lg2.szAccountName, sizeof(lg2.szAccountName), "%s", (char *)szName);

				   if (g_pTranClient)//通知S3
					   g_pTranClient->SendPackToServer(&lg2, sizeof(tagLeaveGame2));
				   if (g_pChatClient)
					   g_pChatClient->SendPackToServer(&lg2, sizeof(tagLeaveGame2));
				   if (g_pTongClient)
					   g_pTongClient->SendPackToServer(&lg2, sizeof(tagLeaveGame2));


				   tagRoleEnterGame reg;
				   reg.ProtocolType = c2s_roleserver_lock;
				   reg.bLock  = false;  //删除锁定
				   reg.Lixian = false;           //是否离线的
				   reg.nRegServerIndex = 0;  //属于那个大区
				   reg.nGsNumber = 0;        //GS索引号
				   //GetGameData(SGDI_CHARACTER_NAME, (unsigned int)reg.Name,i);
				    GetGameDataInfo(SGDI_CHARACTER_NAME,0,i,reg.Name);
				   if (g_pDataClient)
					   g_pDataClient->SendPackToServer((const void *)&reg, sizeof(tagRoleEnterGame));
				   if (g_pGateWayClient)
					   g_pGateWayClient->SendPackToServer((const void *)&reg, sizeof(tagRoleEnterGame));

				   printf("--------Cancel offline successfully(%s) --------- \n",Npc[Player[i].m_nIndex].Name);
				   PlayerSet.RemoveInGame(i);
				  break;
			  }

			  int nLeftTime		 = Player[i].m_cTask.GetSaveVal(TASKVALUE_LIXIAN); //离线时间
			  if (nLeftTime <= 0) 
			  {   
					Player[i].m_nLiXian = 0;
					Player[i].m_nForbFlag = 0;
					printf("--(%s) Obtain Kick offline het BCH successfully!-- \n" ,Npc[Player[i].m_nIndex].Name);
			  }   
			  else 
			  {		//uy thac
				  Player[i].m_cTask.SetSaveVal(TASKVALUE_LIXIAN, nLeftTime - 30); //18
				  //#pragma omp atomic
				  Npc[Player[i].m_nIndex].m_nLixianTime++;

				  //int nLiXianInfo[4];
				  //g_GameSetTing.GetInt3("SYSTEM","LiXianLevelAndExp",nLiXianInfo);
				  if (Npc[Player[i].m_nIndex].m_nLixianTime >= 100 && !Npc[Player[i].m_nIndex].m_FightMode)
				  {
					  Npc[Player[i].m_nIndex].m_nLixianTime = 0;
					  //Player[m_nPlayerIdx].AddExp(GetRandomNumber(nLiXianInfo[1],nLiXianInfo[2]),m_Level); //离线经验
					  //int nExpBCH = GetRandomNumber(nLiXianInfo[1], nLiXianInfo[2])
					  //Player[i].DirectAddExp(nExpBCH); //离线经验
					  //printf("------(%s) Obtain offline Bach Cau Hoan EXP: %d successfully!------- \n" ,Npc[Player[i].m_nIndex].Name, nExpBCH);
					  //Player[i].ExecuteScript2("\\script\\system\\player\\offline.lua", "ontime_uythac", 0, 0);
					  //Player[i].ExecuteScript("\\script\\system\\player\\offline.lua","ontime_uythac",0);
					  printf("------(%s) Obtain offline Bach Cau Hoan successfully !------- \n" ,Npc[Player[i].m_nIndex].Name);
					  Player[i].Save(); 
					  if (g_pDataClient)
					  {//存档
						  Player[i].SavePlayerData(false);
					  }
				  } 
			  }
		  }//end if  离线
		 
      }//end if neconnet

	}
	//有时间限制的怪物

	//if (g_SubWorldSet.GetGameTime()-(g_SubWorldSet.GetGameTime()>>4<<4)==0)
	//{
	//  _timeNpcInfo::iterator it;
	//  for( it = timeNpcInfo.begin(); it != timeNpcInfo.end(); ++it)
	//  { 
	//	if (it->second>0 && it->second<MAX_NPC)
	//	{//在这个范围内  NPC
	//		 if  (Npc[it->second].m_GameliveTime>0)
	//			 Npc[it->second].m_GameliveTime--;
	//		 if (Npc[it->second].m_GameliveTime==0)
	//		 {
	//			 Npc[it->second].m_GameliveTime = 0;
	//			 /*char nKey[64];
	//			 ZeroMemory(nKey,sizeof(nKey));
	//			 sprintf(nKey,"%d_time",it->second);
	//			 std::string nItemKey= nKey;*/
	//			 timeNpcInfo.erase(it);
	//		 }
	//	}
	//  }
	//}
	/*Concurrency::parallel_for(1,MAX_PLAYER,[&](int i)
	{  
	  //printf("----并行:%d----\n",i);
	  if (Player[i].m_nNetConnectIdx>=0)
	  {//在线的 
          if (Player[i].m_nVipType==0)
		  {
			  if (Player[i].m_GameLeftTime>0)
			  { //包月
					  Player[i].m_GameLeftTime--;

					  if ( Player[i].m_GameLeftTime<=0)
						   Player[i].m_Isinvalid = TRUE;
					  else
						   Player[i].m_Isinvalid = FALSE;

					  if ( Player[i].m_GameLeftTime<=540)
					  {
						  char nMsg[64]={0};
						  sprintf(nMsg,"提示:<color=gyellow>在游戏中还可以在线 %d<color> 秒!",Player[i].m_GameLeftTime);
						  Player[i].m_ItemList.msgshow(nMsg);
					  }
			  }
			  else
				   Player[i].m_Isinvalid = TRUE;

			  if  ( Player[i].m_Isinvalid)
			  {//发送时间到的提示
				  if (g_pServer)
				     g_pServer->ShutdownClient( Player[i].m_nNetConnectIdx);
			  }
		  }

		  if (Player[i].CanSave() && g_SubWorldSet.m_nLoopRate - Player[i].m_uNextSaveTime >= defPLAYER_SAVE_TIMEOUT)
		  {//个人存档时间
			  if (Player[i].Save())
			  { //生成数据包
					Player[i].m_uMustSave = SAVE_REQUEST;
					Player[i].m_uNextSaveTime = g_SubWorldSet.m_nLoopRate;
					//m_ulNextSaveTime += m_ulDelayTimePerSave;
					Player[i].SavePlayerData(false);	  
			  }
		  }
		  //离线挂机
		  if (Player[i].m_nLiXian)
		  {
			  int nLeftTime = Player[i].m_cTask.GetSaveVal(TASKVALUE_LIXIAN); //离线时间

			  if (nLeftTime <= 0)
			  {
				  Player[i].m_nLiXian = 0;
			  }
			  else
			  {
				  Player[i].m_cTask.SetSaveVal(TASKVALUE_LIXIAN,nLeftTime - 18);
				  //#pragma omp atomic
				  Npc[Player[i].m_nIndex].m_nLixianTime++;

				  int nLiXianInfo[4];
				  g_GameSetTing.GetInt3("SYSTEM","LiXianLevelAndExp",nLiXianInfo);
				  if (Npc[Player[i].m_nIndex].m_Level >= nLiXianInfo[0] && Npc[Player[i].m_nIndex].m_nLixianTime >= 100 && !Npc[Player[i].m_nIndex].m_FightMode)
				  {
					  Npc[Player[i].m_nIndex].m_nLixianTime = 0;
					  //Player[m_nPlayerIdx].AddExp(GetRandomNumber(nLiXianInfo[1],nLiXianInfo[2]),m_Level); //离线经验
					  Player[i].DirectAddExp(GetRandomNumber(nLiXianInfo[1],nLiXianInfo[2])); //离线经验
					  printf("------(%s)获取离线经验成功!------- \n",Npc[Player[i].m_nIndex].Name);
					  if (Player[i].Save())	
						  Player[i].m_uMustSave = SAVE_REQUEST;	 //删除东西立即存档
					  if (g_pDataClient)
					  {//存档
						  Player[i].SavePlayerData(false);
					  }
				  }
			  }
		  }//end if  离线

      }//end if neconnet

	});*/
}

void CoreServerShell::ReleaseDbData()
{
	if  (g_DbActivity)
	{
		//g_DbActivity->removeLog();	  //清楚日志
		g_DbActivity->close();

		delete g_DbActivity;
		g_DbActivity=NULL;
	}

	if  (g_DbItemFile)
	{
		//g_DbItemFile->removeLog();	  //清楚日志
		g_DbItemFile->close();

		delete g_DbItemFile;
		g_DbItemFile=NULL;
	}

}


void CoreServerShell::SavePaiShopData()
{
	if (!g_DbItemFile)
		return;
	//先清空数据
	char nIdxInfo[32]={0};

	ZCursorS *cursor = g_DbItemFile->first();
	while(cursor)
	{
		TDBShopItemData* pRoleData = (TDBShopItemData*)cursor->data;
		if (pRoleData && cursor->size >= (int)sizeof(TDBShopItemData) && pRoleData->iid>0)
		{
			//printf("----删除:%04d,iid:%d KEY号:%s 角:%s 名:%s 密:%u --\n",pRoleData->iid,pRoleData->iid,pRoleData->iaccName,pRoleData->iWonName,pRoleData->itemName,pRoleData->iparamj3);
			sprintf(nIdxInfo,"剑侠%04d情缘",pRoleData->iid);
			g_DbItemFile->remove(nIdxInfo,strlen(nIdxInfo)+1);
		}
		if(!g_DbItemFile->next(cursor))
		  break;
	}

	printf("---[拍卖行]数据清空完毕---\n");
	//然后再增加
	char * pItemBuffer = (char*)_alloca(sizeof(TDBShopItemData));
	TDBShopItemData *nTempItem =(TDBShopItemData *)pItemBuffer;

	_ItemDataInfo::iterator it; 
	 for( it = iItemDataInfo.begin(); it != iItemDataInfo.end(); ++it)
	 {//开始更新数据
		if (it->second.iid>0)
		{
					//ReWhereSkillEnhance(it->second.nSkillIdx,it->second.nEnhance,1);
					nTempItem->iid             = it->second.iid;        //编号idx 1开始
					nTempItem->iequipgen       = it->second.iequipgen;  //
					nTempItem->idetailtype     = it->second.idetailtype;
					nTempItem->iparticulartype = it->second.iparticulartype; 
					snprintf(nTempItem->itemName, sizeof(nTempItem->itemName), "%s", it->second.itemName);
					nTempItem->ilevel          = it->second.ilevel;     //等级
					nTempItem->iseries         = it->second.iseries;    //武行
					nTempItem->irandseed       = it->second.irandseed;  //随机种子
					nTempItem->iparam2         = it->second.iparam2;    //蓝装
					nTempItem->iparam3         = it->second.iparam3;
					nTempItem->iparam5         = it->second.iparam5;
					nTempItem->iparam4         = it->second.iparam4;
					nTempItem->iparam6         = it->second.iparam6;
					nTempItem->iparam1         = it->second.iparam1;
					nTempItem->iparamr2        = it->second.iparamr2;   //熔炼
					nTempItem->iparamr3        = it->second.iparamr3;
					nTempItem->iparamr5        = it->second.iparamr5;
					nTempItem->iparamr4        = it->second.iparamr4;
					nTempItem->iparamr6        = it->second.iparamr6;
					nTempItem->iparamr1        = it->second.iparamr1;
					nTempItem->ilucky          = it->second.ilucky;   //装备的幸运值
					nTempItem->igoldid         = it->second.igoldid;  //是否黄金
					nTempItem->istacknum       = it->second.istacknum;//叠加数量
					nTempItem->ienchance       = it->second.ienchance;//加成
					nTempItem->ipoint          = it->second.ipoint;  //是否紫装 7为特殊装 8 为随机黄金   1-6 为紫装
					nTempItem->addtime         = it->second.addtime; //开始时间
					nTempItem->endtime         = it->second.endtime; //结束时间
					nTempItem->iPriceKind      = it->second.iPriceKind;   //货币类型
					nTempItem->iSellPrice      = it->second.iSellPrice;   //售卖价格
					nTempItem->iRongpiont      = it->second.iRongpiont;   //是否熔炼
					snprintf(nTempItem->iWonName, sizeof(nTempItem->iWonName), "%s", it->second.iWonName);  //挂售者
					snprintf(nTempItem->iaccName, sizeof(nTempItem->iaccName), "%s", it->second.iaccName);   //二级查询　KEY
					nTempItem->iswhere         = it->second.iswhere;    //是否洗练装
					nTempItem->isplatina       = it->second.isplatina;  //是否白金
					nTempItem->isSellOut       = it->second.isSellOut;  //是否已经卖出
					nTempItem->iparamj1        = it->second.iparamj1;   //保留字段 是否可以叠加
					nTempItem->iparamj2        = it->second.iparamj2;   //是否过期
					nTempItem->iparamj3        = it->second.iparamj3;   //物品取回密码
					nTempItem->iparamj5        = it->second.iparamj5;
					nTempItem->iparamj4        = it->second.iparamj4;   //是否已经兑换了收入
					nTempItem->iparamj6        = it->second.iparamj6;
					nTempItem->iparamj7        = it->second.iparamj7;

					if (!g_DbItemFile->add(it->second.iaccName,strlen(it->second.iaccName)+1,pItemBuffer,sizeof(TDBShopItemData)))   //pName strlen(szName) +1
					{
						 printf("---[拍卖行]数据更新%s失败:---\n",it->second.iaccName);
					}
		} //end if 
	 }//end for
	 printf("---[拍卖行]数据更新完毕---\n");
}
void CoreServerShell::SynCoreItemData()
{
	if (!Player) return;
	ITEMDATA_SYNC_END ItenSyncEnd;
	ZeroMemory(&ItenSyncEnd,sizeof(ItenSyncEnd));

	ItenSyncEnd.ProtocolType = s2c_itemdata_shop;
	ItenSyncEnd.iid = 0;
	//并行处理　增加效率
	/*Concurrency::parallel_for(1,MAX_PLAYER,[&](int i)
	{  
		//printf("----并行:%d----\n",i);
		if (Player[i].m_nNetConnectIdx>=0)
		{//在线的 
			//printf("----在线并行:%d----\n",i);
			_ItemDataInfo::iterator it; 
			for( it = iItemDataInfo.begin(); it != iItemDataInfo.end(); ++it)
			{
				if (it->second.iid>0)
				{
					//ReWhereSkillEnhance(it->second.nSkillIdx,it->second.nEnhance,1);
					ItenSyncEnd.iid             = it->second.iid;        //编号idx 1开始
					ItenSyncEnd.iequipgen       = it->second.iequipgen;  //
					ItenSyncEnd.idetailtype     = it->second.idetailtype;
					ItenSyncEnd.iparticulartype = it->second.iparticulartype; 
					sprintf(ItenSyncEnd.itemName,it->second.itemName);
					ItenSyncEnd.ilevel          = it->second.ilevel;     //等级
					ItenSyncEnd.iseries         = it->second.iseries;    //武行
					ItenSyncEnd.irandseed       = it->second.irandseed;  //随机种子
					ItenSyncEnd.iparam2         = it->second.iparam2;    //蓝装
					ItenSyncEnd.iparam3         = it->second.iparam3;
					ItenSyncEnd.iparam5         = it->second.iparam5;
					ItenSyncEnd.iparam4         = it->second.iparam4;
					ItenSyncEnd.iparam6         = it->second.iparam6;
					ItenSyncEnd.iparam1         = it->second.iparam1;
					ItenSyncEnd.iparamr2        = it->second.iparamr2;   //熔炼
					ItenSyncEnd.iparamr3        = it->second.iparamr3;
					ItenSyncEnd.iparamr5        = it->second.iparamr5;
					ItenSyncEnd.iparamr4        = it->second.iparamr4;
					ItenSyncEnd.iparamr6        = it->second.iparamr6;
					ItenSyncEnd.iparamr1        = it->second.iparamr1;
					ItenSyncEnd.ilucky          = it->second.ilucky;   //装备的幸运值
					ItenSyncEnd.igoldid         = it->second.igoldid;  //是否黄金
					ItenSyncEnd.istacknum       = it->second.istacknum;//叠加数量
					ItenSyncEnd.ienchance       = it->second.ienchance;//加成
					ItenSyncEnd.ipoint          = it->second.ipoint;  //是否紫装 7为特殊装 8 为随机黄金   1-6 为紫装
					ItenSyncEnd.addtime         = it->second.addtime; //开始时间
					ItenSyncEnd.endtime         = it->second.endtime; //结束时间
					ItenSyncEnd.iPriceKind      = it->second.iPriceKind;   //货币类型
					ItenSyncEnd.iSellPrice      = it->second.iSellPrice;   //售卖价格
					ItenSyncEnd.iRongpiont      = it->second.iRongpiont;   //是否熔炼
					sprintf(ItenSyncEnd.iWonName,it->second.iWonName);  //挂售者
					sprintf(ItenSyncEnd.iaccName,it->second.iaccName);   //二级查询　KEY
					ItenSyncEnd.iswhere         = it->second.iswhere;    //是否洗练装
					ItenSyncEnd.isplatina       = it->second.isplatina;  //是否白金
					ItenSyncEnd.isSellOut       = it->second.isSellOut;  //是否已经卖出
					ItenSyncEnd.iparamj1        = it->second.iparamj1;   //保留字段 是否可以叠加
					ItenSyncEnd.iparamj2        = it->second.iparamj2;   //是否过期
					ItenSyncEnd.iparamj3        = it->second.iparamj3;   //物品取回密码
					ItenSyncEnd.iparamj5        = it->second.iparamj5;
					ItenSyncEnd.iparamj4        = it->second.iparamj4;
					ItenSyncEnd.iparamj6        = it->second.iparamj6;
					ItenSyncEnd.iparamj7        = it->second.iparamj7;

					if (g_pServer)
						g_pServer->PackDataToClient(Player[i].m_nNetConnectIdx,&ItenSyncEnd, sizeof(ITEMDATA_SYNC_END));
				}
			}
		}
	}); //end for
	*/
	for (int i=1;i<MAX_PLAYER;i++)
	{
		if (Player[i].m_isInGame && Player[i].m_nNetConnectIdx>=0)
		{//在线的 
			_ItemDataInfo::iterator it; 
			for( it = iItemDataInfo.begin(); it != iItemDataInfo.end(); ++it)
			{
				if (it->second.iid>0)
				{
					//ReWhereSkillEnhance(it->second.nSkillIdx,it->second.nEnhance,1);
					ItenSyncEnd.iid             = it->second.iid;        //编号idx 1开始
					ItenSyncEnd.iequipgen       = it->second.iequipgen;  //
					ItenSyncEnd.idetailtype     = it->second.idetailtype;
					ItenSyncEnd.iparticulartype = it->second.iparticulartype; 
					snprintf(ItenSyncEnd.itemName, sizeof(ItenSyncEnd.itemName), "%s", it->second.itemName);
					ItenSyncEnd.ilevel          = it->second.ilevel;     //等级
					ItenSyncEnd.iseries         = it->second.iseries;    //武行
					ItenSyncEnd.irandseed       = it->second.irandseed;  //随机种子
					ItenSyncEnd.iparam2         = it->second.iparam2;    //蓝装
					ItenSyncEnd.iparam3         = it->second.iparam3;
					ItenSyncEnd.iparam5         = it->second.iparam5;
					ItenSyncEnd.iparam4         = it->second.iparam4;
					ItenSyncEnd.iparam6         = it->second.iparam6;
					ItenSyncEnd.iparam1         = it->second.iparam1;
					ItenSyncEnd.iparamr2        = it->second.iparamr2;   //熔炼
					ItenSyncEnd.iparamr3        = it->second.iparamr3;
					ItenSyncEnd.iparamr5        = it->second.iparamr5;
					ItenSyncEnd.iparamr4        = it->second.iparamr4;
					ItenSyncEnd.iparamr6        = it->second.iparamr6;
					ItenSyncEnd.iparamr1        = it->second.iparamr1;
					ItenSyncEnd.ilucky          = it->second.ilucky;   //装备的幸运值
					ItenSyncEnd.igoldid         = it->second.igoldid;  //是否黄金
					ItenSyncEnd.istacknum       = it->second.istacknum;//叠加数量
					ItenSyncEnd.ienchance       = it->second.ienchance;//加成
					ItenSyncEnd.ipoint          = it->second.ipoint;  //是否紫装 7为特殊装 8 为随机黄金   1-6 为紫装
					ItenSyncEnd.addtime         = it->second.addtime; //开始时间
					ItenSyncEnd.endtime         = it->second.endtime; //结束时间
					ItenSyncEnd.iPriceKind      = it->second.iPriceKind;   //货币类型
					ItenSyncEnd.iSellPrice      = it->second.iSellPrice;   //售卖价格
					ItenSyncEnd.iRongpiont      = it->second.iRongpiont;   //是否熔炼
					snprintf(ItenSyncEnd.iWonName, sizeof(ItenSyncEnd.iWonName), "%s", it->second.iWonName);  //挂售者
					snprintf(ItenSyncEnd.iaccName, sizeof(ItenSyncEnd.iaccName), "%s", it->second.iaccName);
					ItenSyncEnd.iswhere         = it->second.iswhere;    //是否洗练装
					ItenSyncEnd.isplatina       = it->second.isplatina;  //是否白金
					ItenSyncEnd.isSellOut       = it->second.isSellOut;  //是否已经卖出
                    ItenSyncEnd.iparamj1        = it->second.iparamj1;   //保留字段 是否可以叠加
					ItenSyncEnd.iparamj2        = it->second.iparamj2;   //是否过期
					ItenSyncEnd.iparamj3        = it->second.iparamj3;   //物品取回密码
					ItenSyncEnd.iparamj5        = it->second.iparamj5;
					ItenSyncEnd.iparamj4        = it->second.iparamj4;
					ItenSyncEnd.iparamj6        = it->second.iparamj6;
					ItenSyncEnd.iparamj7        = it->second.iparamj7;

					if (g_pServer)
						g_pServer->PackDataToClient(Player[i].m_nNetConnectIdx,&ItenSyncEnd, sizeof(ITEMDATA_SYNC_END));
				}
			}
		}
	}//end for 
	
}

void CoreServerShell::CheckItemTime()
{
	if (!Player) return;
//printf("--------------------------------------\n");
/*int nIsOmp=0;
    g_GameSetTing.GetInteger("Gameconfig","CoreCheckTime",0,&nIsOmp);

if (nIsOmp)
{
#pragma omp parallel for
	for (int i=1;i<MAX_PLAYER;i++)
	{
		if (Player[i].m_nNetConnectIdx>=0)
		{
			Player[i].m_ItemList.CheckItemTime();
			if (Player[i].m_cTong.m_nFlag && Player[i].m_cTong.m_Mapid!=47)//入帮派的都检测
			{//是帮派 并且不是 帮战地图都检测 删除
				Player[i].m_ItemList.CheckIsCityAndBang(); //检测是否是城主或 帮战地图图主
			}
		}
	}
}
else*/
//{
	for (int i=1;i<MAX_PLAYER;i++)
	{
		if (Player[i].m_isInGame && Player[i].m_nNetConnectIdx>=0)
		{
			Player[i].m_ItemList.CheckItemTime();
			/*if (Player[i].m_cTong.m_nFlag && Player[i].m_cTong.m_Mapid!=47)//入帮派的都检测
			{//是帮派 并且不是 帮战地图都检测 删除
				Player[i].m_ItemList.CheckIsCityAndBang(); //检测是否是城主或 帮战地图图主
			}*/
		}
	}
//}

}


//付费聊天
BOOL CoreServerShell::PayForSpeech(int nIndex, int nType)
{
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return FALSE;

	int	nMoney = 0;
	int nNpcIdx = Player[nIndex].m_nIndex;
	if (nNpcIdx <= 0)
	{
		//Player[nIndex].m_ItemList.msgshow("提示:对不起,您被禁言了!");
		printf("Loi: %d \n",nNpcIdx);
		return FALSE;
	}

	if (Player[nIndex].m_nForbiddenFlag >0/* & KPlayer::FF_CHAT*/)	//被禁言
	{
		Player[nIndex].m_ItemList.msgshow("Rat tiec, ban da bi cam chat!");
		return FALSE;
	}

	if (Player[nIndex].m_nLiXian > 0/* & KPlayer::FF_CHAT*/)	//被禁言
	{
		Player[nIndex].m_ItemList.msgshow("Xin loi, toi dang Uy Thac Khong the noi chuyen!");
		return FALSE;
	}

    int nCityLevel=10,nWorldLevel=30;
    char nMsg[128]={0};

	nCityLevel = _gameGlobalSet.SubWorldInfo.nCityLevel;
	nWorldLevel = _gameGlobalSet.SubWorldInfo.nWorldLevel;
	//g_GameSetTing.GetInteger2("SYSTEM","ForBitChat",&nCityLevel,&nWorldLevel);

	int nLevel   = Npc[nNpcIdx].m_Level;
	int nMaxMana = Npc[nNpcIdx].m_CurrentManaMax;

	switch (nType)
	{
	case 0:		//免费
		return TRUE;
		break;
	case 1:
		{//门派
			nMoney = 10;
			return Player[nIndex].Pay(nMoney);
		}
		break;
	case 2:
		{//公聊 使用内力
			if (nLevel < nWorldLevel)
			{
				sprintf(nMsg,"Cap do nho hon %d, ban khong the chat!",nWorldLevel);
				printf("Loi Chat: (%d):%s \n",nIndex,nMsg);
				Player[nIndex].m_ItemList.msgshow(nMsg);
				return FALSE;
			}
			return Npc[nNpcIdx].Cost(attrib_mana, nMaxMana / 2);
		}
		break;
	case 3:
		{
			if (nLevel < nCityLevel)
			{
				sprintf(nMsg,"Cap do nho hon %d, ban khong the chat!",nCityLevel);
				Player[nIndex].m_ItemList.msgshow(nMsg);
				return FALSE;
			}
			return Npc[nNpcIdx].Cost(attrib_mana, nMaxMana / 10);
		}
		break;
	case 4:
		{
			if (nLevel < nWorldLevel)
			{
			    sprintf(nMsg,"Cap do nho hon %d, ban khong the chat!",nWorldLevel);
				Player[nIndex].m_ItemList.msgshow(nMsg);
				return FALSE;
			}

			return Npc[nNpcIdx].Cost(attrib_mana, nMaxMana * 4 / 5);
		}
		break;
	default:
		return FALSE;	//不认识的类别不发送
	}
}

void CoreServerShell::SetTongData(int nPlayerIdx, void* data)
{
	Player[nPlayerIdx].m_cTong.ChangeData(data);
}

void CoreServerShell::ExecuteScript(RELAY_REMOTE* pRemote)
{
	if (!pRemote->sScript || !pRemote->sFunction)
		return;

	char nCurFileName[128] = { 0 };
	snprintf(nCurFileName, sizeof(nCurFileName), "%s", pRemote->sScript);
	DWORD dwScriptId = g_FileName2Id(nCurFileName);

	KLuaScript* pScript = (KLuaScript*)g_GetScript(dwScriptId);
	if (pScript == NULL)
	{
		printf(strCoreInfo[L_SortScript_4].c_str(), pRemote->sScript, pRemote->sFunction);
		return;
	}

	g_SubWorldSet.SubExecuteScript(pRemote->sScript, pRemote->sFunction, 
		pRemote->sValue, pRemote->sParam, pRemote->nValue);
}

void CoreServerShell::SyncBlackList(int nPlayerIdx, std::list<string>& blackList)
{
	Player[nPlayerIdx].m_cChat.SyncBlackList(blackList);
}

BOOL CoreServerShell::CheckBlackList(int nPlayerIdx, char* szName)
{
	if (nPlayerIdx <= 0 || nPlayerIdx >= MAX_PLAYER)
		return FALSE;
	return Player[nPlayerIdx].m_cChat.OnBlackList(szName);
}
void CoreServerShell::OnTradePairSaveResult(unsigned __int64 uTxId, int nResultCode, const char* szRoleA, const char* szRoleB)
{
	printf("[CoreServerShell] OnTradePairSaveResult: Tx 0x%llX, result=%d (%s, %s)\n",
		uTxId, nResultCode, szRoleA ? szRoleA : "", szRoleB ? szRoleB : "");
	uint32_t dwState = KPlayerTradeJournal::Instance().GetTxState(uTxId);
	if (dwState != TX_STATE_APPLY_PENDING && dwState != TX_STATE_COMMITTED)
	{
		printf("[CoreServerShell] Tx 0x%llX not in pending state (%u). Ignoring ACK.\n", uTxId, dwState);
		return;
	}
	uint32_t idxA = 0, idxB = 0;
	std::string nameA, nameB;
	if (KPlayerTradeJournal::Instance().GetTxParticipants(uTxId, idxA, nameA, idxB, nameB))
	{
		if (szRoleA && szRoleA[0] && _stricmp(szRoleA, nameA.c_str()) != 0)
		{
			printf("[CoreServerShell] Mismatched RoleA in ACK (expected %s, got %s)!\n", nameA.c_str(), szRoleA);
			return;
		}
		if (szRoleB && szRoleB[0] && _stricmp(szRoleB, nameB.c_str()) != 0)
		{
			printf("[CoreServerShell] Mismatched RoleB in ACK (expected %s, got %s)!\n", nameB.c_str(), szRoleB);
			return;
		}
	}
	if (nResultCode > 0)
	{
		bool bAcked = KPlayerTradeJournal::Instance().OnTradePairAck(uTxId, nResultCode);
		if (bAcked)
		{
			TRADE_DECISION_SYNC sSync;
			sSync.ProtocolType = s2c_tradedecision;
			sSync.m_btDecision = 1;
			// Finalize Player A if still in slot with matching name
			if (idxA > 0 && idxA < MAX_PLAYER && strcmp(Player[idxA].m_PlayerName, nameA.c_str()) == 0)
			{
				Player[idxA].m_cMenuState.SetState(idxA, PLAYER_MENU_STATE_NORMAL);
				Player[idxA].m_ItemList.InvalidateTradeSnapshot();
				Player[idxA].m_cTrade.Release();
				if (Player[idxA].m_nNetConnectIdx >= 0 && g_pServer)
					g_pServer->PackDataToClient(Player[idxA].m_nNetConnectIdx, (BYTE*)&sSync, sizeof(TRADE_DECISION_SYNC));
			}
			// Finalize Player B if still in slot with matching name
			if (idxB > 0 && idxB < MAX_PLAYER && strcmp(Player[idxB].m_PlayerName, nameB.c_str()) == 0)
			{
				Player[idxB].m_cMenuState.SetState(idxB, PLAYER_MENU_STATE_NORMAL);
				Player[idxB].m_ItemList.InvalidateTradeSnapshot();
				Player[idxB].m_cTrade.Release();
				if (Player[idxB].m_nNetConnectIdx >= 0 && g_pServer)
					g_pServer->PackDataToClient(Player[idxB].m_nNetConnectIdx, (BYTE*)&sSync, sizeof(TRADE_DECISION_SYNC));
			}
		}
	}
	else
	{
		printf("[CoreServerShell] Goddess returned failure (%d) for Tx 0x%llX! Fail-Closed: preserving COMMITTED in WAL and keeping players locked.\n",
			nResultCode, uTxId);
		KPlayerTradeJournal::Instance().OnTradePairAck(uTxId, nResultCode);
	}
}

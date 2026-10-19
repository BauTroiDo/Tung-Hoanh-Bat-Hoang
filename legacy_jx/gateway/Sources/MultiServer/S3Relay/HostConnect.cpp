// HostConnect.cpp: implementation of the CHostConnect class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Global.h"
#include "HostConnect.h"
#include "HostServer.h"
#include "DealRelay.h"
#include "S3Relay.h"
#include "../../sword3paysys/s3accserver/AccountLoginDef.h"


CHostConnect::stdHostServer			CHostConnect::m_sHostServer;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHostConnect::CHostConnect(CHostServer* pHostServer, unsigned long id)
	: CNetConnect(pHostServer, id)//初始化 CNetConnect 实例  对应的网络连接号
{
	nGsNetIdx = id;
	nSerVerIndex = 1;
}


CHostConnect::~CHostConnect()
{
	m_serMaps.clear();
	nSerVerIndex =1;
}

// 收GS包 
void CHostConnect::RecvPackage(const void* pData, size_t size)
{
	EXTEND_HEADER* pHeader = (EXTEND_HEADER*)pData;

	if (pHeader->ProtocolFamily == pf_normal)
	{
		Proc0_Normal(pData, size);
	}
	else if (pHeader->ProtocolFamily == pf_relay)
	{
		Proc0_Relay(pData, size);
	}
	else if (pHeader->ProtocolFamily == pf_playercommunity)
	{//增加聊天频道
		Proc0_PlayerCommunity(pData, size);
	}
	else if (pHeader->ProtocolFamily == pf_udataserveridx)
	{//更新当前服务器的索引
		Proc0_ServerCommunity(pData, size);
	}
	else if (pHeader->ProtocolFamily == pf_udatasservermaps)
	{//更新当前服务器的所加载的地图
		Proc0_ServerMaps(pData, size);
	} 
}


void CHostConnect::Proc0_Normal(const void* pData, size_t size)
{
	EXTEND_HEADER* pHeader = (EXTEND_HEADER*)pData;

	if (pHeader->ProtocolID == c2s__entergamegkjyiruqine)
	{
		Proc1_Normal_EnterGame(pData, size);
	}
	else if (pHeader->ProtocolID == c2s_leavegame)
	{
		Proc1_Normal_LeaveGame(pData, size);
	}
}
//进入游戏调用 填充各个容器 帐号 角色结构等
void CHostConnect::Proc1_Normal_EnterGame(const void* pData, size_t size)
{
	tagEnterGame2* pEnterGame2 = (tagEnterGame2*)pData;

	std::_tstring strAcc = (char*)pEnterGame2->szAccountName;//进入游戏的帐号
	assert(!strAcc.empty());
	std::_tstring strRole = (char*)pEnterGame2->szCharacterName;//进入游戏的角色名
	assert(!strRole.empty());

	ACCINFO infoAcc;
	infoAcc.role = strRole;
	infoAcc.nameID = pEnterGame2->dwNameID;
	infoAcc.param = pEnterGame2->lnID;//客户端连接GS的网络唯一标示 多区会覆盖掉
    
	ROLEINFO infoRole;
	infoRole.acc = strAcc;
	infoRole.nameID = pEnterGame2->dwNameID;
	infoRole.param = pEnterGame2->lnID;//客户端连接GS的网络唯一标示 多区会覆盖掉

	PARAMINFO infoParam;
	infoParam.acc = strAcc;
	infoParam.role = strRole;
	infoParam.nameID = pEnterGame2->dwNameID;

	{{
	DUMMY_AUTOLOCKWRITE(m_lockPlayer);

	m_mapAcc[strAcc] = infoAcc;
	m_mapRole[strRole] = infoRole;
	m_mapParam[pEnterGame2->lnID] = infoParam;//有可能重复的 多区的时候 会覆盖掉 需要调整
	}}

	{{
	g_FriendMgr.SomeoneLogin(strRole,nGsNetIdx);//通知 这个 角色已经上线 
	}}

	rTRACE("Host:player login: %s [%s] (%08X,%08X,%08X)", strAcc.c_str(), strRole.c_str(), GetIP(), pEnterGame2->lnID,pEnterGame2->nSelSerVer,nGsNetIdx);
}
//离开游戏调用
void CHostConnect::Proc1_Normal_LeaveGame(const void* pData, size_t size)
{
	tagLeaveGame2* pLeaveGame = (tagLeaveGame2*)pData;

	std::_tstring strAcc = pLeaveGame->szAccountName;

	assert(!strAcc.empty());

	{{
	DUMMY_AUTOLOCKWRITE(m_lockPlayer);

	ACCMAP::iterator itAcc = m_mapAcc.find(strAcc);
	if (itAcc != m_mapAcc.end())
	{
		ACCINFO& rAccInfo = (*itAcc).second;

		{{
		//ensure
		//g_ChannelMgr.B_ClearPlayer(GetIP(), rAccInfo.param);
		g_FriendMgr.SomeoneLogout(rAccInfo.role);
		}}
		

		m_mapRole.erase(rAccInfo.role);
		m_mapParam.erase(rAccInfo.param);

		m_mapAcc.erase(itAcc);
	}
	}}
	rTRACE("---Host:(%u区)玩家账号(%s)退出游戏---",pLeaveGame->nSelServer, strAcc.c_str());
}
					

void CHostConnect::Proc0_Relay(const void* pData, size_t size)
{
	EXTEND_HEADER* pHeader = (EXTEND_HEADER*)pData;

	if (pHeader->ProtocolID == relay_c2c_data)
	{
		rTRACE("---relay_c2c_data   A---");
		Proc1_Relay_Data(pData, size);
	}
	else if (pHeader->ProtocolID == relay_c2c_askwaydata)
	{
		rTRACE("---relay_c2c_askwaydata   A---");
		Proc1_Relay_AskWay(pData, size);
	}
	else if (pHeader->ProtocolID == relay_s2c_loseway)
	{
		rTRACE("---relay_s2c_loseway   A---");
		Proc1_Relay_LoseWay(pData, size);
	}
	else if (pHeader->ProtocolID == relay_c2c_remote)
	{
		Proc1_Relay_Remote(pData, size);
	}
}

void CHostConnect::Proc1_Relay_Remote(const void* pData, size_t size)
{
	if (!pData)
		return;
	if (size <= 0)
		return;

	RELAY_REMOTE* pRemote = (RELAY_REMOTE*)pData;
	g_HostServer.BroadPackage(pRemote, size);
}

void CHostConnect::Proc1_Relay_Data(const void* pData, size_t size)
{
	RELAY_DATA* pRelayData = (RELAY_DATA*)pData;

	assert(pRelayData->nFromIP == 0);

	pRelayData->nFromIP          = gGetHostIP(local_adapt);  //当前的ip
	pRelayData->nFromRelayID = GetID();                   //当前的连接编号

	if (pRelayData->nToIP == 0)
	{
		rTRACE("---CHostConnect::Proc1_Relay_Data  pRelayData->nToIP == 0 ERROR  ---");
		return;
	}
	else if (pRelayData->nToIP==INADDR_BROADCAST)
	{
		g_HostServer.BroadPackage(pRelayData,size,pRelayData->nToRelayID);
		rTRACE("---CHostConnect::Proc1_Relay_Data  INADDR_BROADCAST -->gs OK A---");
		return;
	}
	else if (pRelayData->nToIP == gGetHostIP(local_adapt))
	{//如果相同 IP？ 不包括端口
		pRelayData->nToIP = 0;
		if (g_HostServer.IsConnectReady(pRelayData->nToRelayID))
		{
		   CNetConnectDup conndup = g_HostServer.FindNetConnect(pRelayData->nToRelayID);
		   if (conndup.IsValid())
		   {
			   conndup.SendPackage(pRelayData, size);
			   rTRACE("---CHostConnect::Proc1_Relay_Data  -->gs OK A---");
			   return;
		    }
		}
	}	
	else if (pRelayData->nToIP)
	{//如果相同 IP？ 包括端口
		pRelayData->nToIP = 0;
		if (g_HostServer.IsConnectReady(pRelayData->nToRelayID))
		{
			CNetConnectDup conndup = g_HostServer.FindNetConnect(pRelayData->nToRelayID);
			if (conndup.IsValid())
			{
				conndup.SendPackage(pRelayData, size);
				rTRACE("---CHostConnect::Proc1_Relay_Data  -->gs OK B---");
				return;
			}
		}
	}	
	else
	{
		CNetSockDupEx sockdup = dealrelay::FindRelaySockByIP(pRelayData->nToIP);
		if (sockdup.IsValid())
		{
			pRelayData->nToIP = 0;
			sockdup.SendPackage(pRelayData, size);
			rTRACE("---CHostConnect::Proc1_Relay_Data   -->gs OK C---");
			return;
		}

		/*if (g_RootClient.IsReady())
		{
		g_RootClient.SendPackage(pRelayData, size);
		return;
		}*/
	}


	//fail to relay, lose data
	size_t sizeLoseData = sizeof(RELAY_DATA) + size;
	RELAY_DATA* pLoseDataDup = (RELAY_DATA*)_alloca(sizeLoseData);

	gFillLosewayPckg(0, GetID(), 
		pRelayData, size, pLoseDataDup, sizeLoseData);

	SendPackage(pLoseDataDup, sizeLoseData);
	rTRACE("---CHostConnect::Proc1_Relay_Data   lose D---");
}
//跨服协议 接受gs发送过来的数据
void CHostConnect::Proc1_Relay_AskWay(const void* pData, size_t size)
{
	RELAY_ASKWAY_DATA* pAskWayData = (RELAY_ASKWAY_DATA*)pData;

	if (pAskWayData->seekMethod == rm_account_id)
	{
		//rTRACE("---Proc2_Relay_AskWay_AccountRoleID rm_account_id---");
		Proc2_Relay_AskWay_AccountRoleID(pData, size, TRUE);
	}
	else if (pAskWayData->seekMethod == rm_role_id)
	{
		//rTRACE("---Proc2_Relay_AskWay_AccountRoleID rm_role_id---");
		Proc2_Relay_AskWay_AccountRoleID(pData, size, FALSE);
	}
	else if (pAskWayData->seekMethod == rm_map_id)
	{
		//rTRACE("---开始执行跨服回应(%u)---",GetID());
		Proc2_Relay_AskWay_MapID(pData, size);
	}
	else if (pAskWayData->seekMethod == rm_gm)
	{
		//rTRACE("---Proc2_Relay_AskWay_GM rm_gm---");
		Proc2_Relay_AskWay_GM(pData, size);
	}

}

void CHostConnect::Proc1_Relay_LoseWay(const void* pData, size_t size)
{
	RELAY_DATA* pLoseData = (RELAY_DATA*)pData;

	assert (pLoseData->nFromIP == 0);
	pLoseData->nFromIP = gGetHostIP(local_adapt);
	pLoseData->nFromRelayID = GetID();

	if (pLoseData->nToIP == 0)
	{
		//arrived
		return;
	}
	else if (pLoseData->nToIP == gGetHostIP(local_adapt))
	{
		pLoseData->nToIP = 0;
		if (g_HostServer.IsConnectReady(pLoseData->nToRelayID))
		{
			CNetConnectDup conndup = g_HostServer.FindNetConnect(pLoseData->nToRelayID);
			if (conndup.IsValid())
			{
				conndup.SendPackage(pLoseData, size);
				return;
			}
		}
	}
	else if (pLoseData->nToIP == INADDR_BROADCAST)
	{
		assert(FALSE);
		return;
	}
	else
	{
		CNetSockDupEx sockdup = dealrelay::FindRelaySockByIP(pLoseData->nToIP);
		if (sockdup.IsValid())
		{
			pLoseData->nToIP = 0;
			sockdup.SendPackage(pLoseData, size);
			return;
		}

		if (g_RootClient.IsReady())
		{
			g_RootClient.SendPackage(pLoseData, size);
			return;
		}
	}


	//fail to relay, lose data
	//don't gen more loseway
}

void CHostConnect::Proc2_Relay_AskWay_AccountRoleID(const void* pData, size_t size, BOOL acc)
{
	RELAY_ASKWAY_DATA* pAskWayData = (RELAY_ASKWAY_DATA*)pData;

	assert(pAskWayData->nFromIP == 0);
	pAskWayData->nFromIP = gGetHostIP(local_adapt);
	pAskWayData->nFromRelayID = GetID();
	pAskWayData->seekRelayCount ++;


	std::_tstring strName = (char*)(pAskWayData + 1);
	assert(!strName.empty());

	DWORD nameid = -1;
	unsigned long param = -1;
	CNetConnectDup conndup;


	BOOL find = FALSE;

	if (acc ? FindPlayerByAcc(strName, NULL, &nameid, &param)
			: FindPlayerByRole(strName, NULL, &nameid, &param))
	{ //the player is at this gamesvr
		conndup = *this;

		find = TRUE;
	}
	else if (acc ? g_HostServer.FindPlayerByAcc(this, strName, &conndup, NULL, &nameid, &param)
				 : g_HostServer.FindPlayerByRole(this, strName, &conndup, NULL, &nameid, &param))
	{//the player is at other gamesvr in this group
		find = TRUE;
	}

	if (find)
	{
		assert(conndup.IsValid());

		size_t pckgsize = sizeof(DWORD)*2 + size;
		RELAY_ASKWAY_DATA* pClntWayData = (RELAY_ASKWAY_DATA*)_alloca(pckgsize);

		gFillClntwayPckg(nameid, param, pAskWayData, size, pClntWayData, pckgsize);

		conndup.SendPackage(pClntWayData, pckgsize);

		return;
	}


	if (acc)
	{
		//the player is at other group, send to root
		if (g_RootClient.IsReady())
		{
			g_RootClient.SendPackage(pAskWayData, size);
			return;
		}
	}

	//lose way
	size_t sizeLoseData = sizeof(RELAY_DATA) + size;
	RELAY_DATA* pLoseDataDup = (RELAY_DATA*)_alloca(sizeLoseData);

	gFillLosewayPckg(0, GetID(), pAskWayData, size, pLoseDataDup, sizeLoseData);

	SendPackage(pLoseDataDup, sizeLoseData);

}

void CHostConnect::Proc2_Relay_AskWay_MapID(const void* pData, size_t size)
{
	//RELAY_ASKWAY_DATA* pAskWayData = (RELAY_ASKWAY_DATA*)pData;
	//// 0  1  2  3  
	////assert(pAskWayData->nFromIP == 0);
	////pAskWayData->nFromIP      = GetID();//当前的服务器 idgGetHostIP(global_adapt);
	////pAskWayData->nFromRelayID = GetID();//唯一连接号
	//pAskWayData->seekRelayCount ++;

	//g_GatewayClient.PassAskWayMap(pAskWayData->nFromIP, pAskWayData);//发往 bishop GetID()
	RELAY_ASKWAY_DATA* pAskWayData = (RELAY_ASKWAY_DATA*)pData;
	assert(pAskWayData->nFromIP == 0);
	pAskWayData->nFromIP          = gGetHostIP(local_adapt);
	pAskWayData->nFromRelayID = GetID();//r3 连接gs的标示号
	pAskWayData->seekRelayCount ++;
	g_GatewayClient.PassAskWayMap(GetIP(), pAskWayData,nSerVerIndex);
}

void CHostConnect::Proc2_Relay_AskWay_GM(const void* pData, size_t size)
{
	RELAY_ASKWAY_DATA* pAskWayData = (RELAY_ASKWAY_DATA*)pData;

	assert (pAskWayData->nFromIP == 0);
	pAskWayData->nFromIP = gGetHostIP(local_adapt);
	pAskWayData->nFromRelayID = GetID();
	pAskWayData->seekRelayCount ++;

	if (g_RootClient.IsReady())
	{
		g_RootClient.SendPackage(pData, size);
		return;
	}

	//fail to relay, lose data

	size_t sizeLoseData = sizeof(RELAY_DATA) + size;
	RELAY_DATA* pLoseDataDup = (RELAY_DATA*)_alloca(sizeLoseData);

	gFillLosewayPckg(0, GetID(),pData, size, pLoseDataDup, sizeLoseData);

	SendPackage(pLoseDataDup, sizeLoseData);
}

void CHostConnect::Proc0_ServerCommunity(const void* pData, size_t size)
{
	EXTEND_HEADER* pHeader = (EXTEND_HEADER*)pData;

    if (pHeader->ProtocolID == relay_s2c_udateserver)
	{//
		tagLeaveGame2 * pServerData = (tagLeaveGame2*)pData;
		//nSerVerIndex = pServerData->nSelServer;//属于哪个区的gs
		nSerVerIndex = (short)HIWORD(pServerData->nSelServer);
		nGsNumBer    =  LOWORD(pServerData->nSelServer);
		m_sHostServer[nSerVerIndex] = GetID();//保存 各个 服务器客户端的连接号 即是GS 的标示编号
		setSerRegIndex(nSerVerIndex);
		setGsNumIndex(nGsNumBer);
		rTRACE("---Host:(%d)区服务器启动完毕:%08X----",nSerVerIndex,nGsNumBer);
	}
}

void CHostConnect::Proc0_ServerMaps(const void* pData, size_t size)
{
	EXTEND_HEADER* pHeader = (EXTEND_HEADER*)pData;
	if (pHeader->ProtocolID == relay_s2c_udatemaps)
	{//
		  tagUpMapGame2 * pUMI = (tagUpMapGame2*)pData;
			//m_sHostServer[pServerData->nSelServer] = GetID();//保存 各个 服务器客户端的连接号 即是GS 的标示编号
			//rTRACE("---(%d)区服务器启动完毕:%08X----",pServerData->nSelServer,m_sHostServer[pServerData->nSelServer]);
		  int  nMapCount = 0;//pUMI->cMapCount;
		 // int  *pMapID = (int *)pUMI->szMapID; 

		  /* CIniFile theMsgFile;
		  _tstring sAppPath = GetAppFullPath( NULL );

		  char nMapListPath[MAX_PATH],nTempPath[128];
		  ZeroMemory(nMapListPath,sizeof(nMapListPath));
		  ZeroMemory(nTempPath,sizeof(nTempPath));

		  sprintf(nTempPath,"maps\\WorldSet_%s.ini",pUMI->szparmName);
		  sprintf(nMapListPath,"%s%s",sAppPath.c_str(),nTempPath);

		  theMsgFile.SetFile(nMapListPath);

		  char nSection[32]={0},szKeyName[32]={0};
		  nMapCount=theMsgFile.ReadInteger("Init","Count",0);*/

		  m_serMaps.clear();

		  KIniFile _mapList;
		  char mapListPath[128];
		  ZeroMemory(mapListPath,sizeof(mapListPath));
		  sprintf(mapListPath,"\\maps\\WorldSet_%s.ini",pUMI->szparmName);

		 if ( _mapList.Load(mapListPath))
		 {
			 _mapList.GetInteger("Init","Count",0,&nMapCount);
			 rTRACE("--------------------------开始邦定地图------------------------------");
			 while ( --nMapCount >= 0 )
			 {
				 char szKeyName[32]={0};
				 sprintf(szKeyName, "World%02d", nMapCount);
				 int nMapIdx=0;
				  _mapList.GetInteger("World",szKeyName,0,&nMapIdx);
				 m_serMaps[nMapCount+1]=nMapIdx;//pMapID[nMapCount];
				 rTRACE("---绑定地图(%d:%d) ok---",nMapCount,nMapIdx /*pMapID[nMapCount]*/);
			 }
			 rTRACE("-------------------------邦定地图结束------------------------------");

			 _mapList.Clear();
		 }

	 
	}
}

BOOL CHostConnect::checkMapIn(int nMapId)
{
	for (SERMAP::iterator it = m_serMaps.begin(); it != m_serMaps.end(); ++it)
	{
		if (nMapId==(*it).second)
		{
		   rTRACE("---------找到符合的gs:%d---------",nMapId);
			return true;
		}
	}
	return false;
}

void CHostConnect::Proc0_PlayerCommunity(const void* pData, size_t size)
{
	EXTEND_HEADER* pHeader = (EXTEND_HEADER*)pData;

	if (pHeader->ProtocolID == playercomm_c2s_querychannelid)
	{//创建频道
		Proc1_PlayerCommunity_QueryChannelID(pData, size);
	}
	else if (pHeader->ProtocolID == playercomm_c2s_freechannelid)
	{
		Proc1_PlayerCommunity_FreeChannelID(pData, size);
	}
	else if (pHeader->ProtocolID == playercomm_c2s_subscribe)
	{
		Proc1_PlayerCommunity_Subscribe(pData, size);
	}
}

//主机连接入口 聊天频道 创建频道
void CHostConnect::Proc1_PlayerCommunity_QueryChannelID(const void* pData, size_t size)
{
	if (size < sizeof(PLAYERCOMM_QUERYCHANNELID) + sizeof(tagPlusSrcInfo))
	{
		rTRACE("错误:协议长度不一样");
		return;
	}
	PLAYERCOMM_QUERYCHANNELID* pPlayerCommQID = (PLAYERCOMM_QUERYCHANNELID*)pData;
	tagPlusSrcInfo* pSrcInfo = (tagPlusSrcInfo*)((BYTE*)pData + size) - 1;

	CNetConnectDup chatconndup;
	//if (g_HostServer.FindPlayerByRole(NULL, std::_tstring(pSomeoneChatCmd->someone), &hostconndup, NULL, &nameid, &param))
	if (!g_ChatServer.FindPlayerBySerNoAndGsNumber(nSerVerIndex, nGsNumBer, &chatconndup))
	{
		dTRACE("Proc1_PlayerCommunity_QueryChannelID Error: (Ser:%08X, Gs:%08X)", nSerVerIndex, nGsNumBer);
		return ;
	}

	DWORD ip= chatconndup.GetIP();  //获取这个gs的连接ip信息

	g_ChannelMgr.B_QueryChannelID(pPlayerCommQID->channel, GetIP(), pSrcInfo->lnID, pSrcInfo->nameid,ip);
}

void CHostConnect::Proc1_PlayerCommunity_FreeChannelID(const void* pData, size_t size)
{
	if (size < sizeof(PLAYERCOMM_FREECHANNELID) + sizeof(tagPlusSrcInfo))
		return;

	PLAYERCOMM_FREECHANNELID* pPlayerCommFID = (PLAYERCOMM_FREECHANNELID*)pData;
	tagPlusSrcInfo* pSrcInfo = (tagPlusSrcInfo*)((BYTE*)pData + size) - 1;


	CNetConnectDup chatconndup;
	//if (g_HostServer.FindPlayerByRole(NULL, std::_tstring(pSomeoneChatCmd->someone), &hostconndup, NULL, &nameid, &param))
	if (!g_ChatServer.FindPlayerBySerNoAndGsNumber(nSerVerIndex, nGsNumBer, &chatconndup))
	{
		dTRACE("Proc1_PlayerCommunity_FreeChannelID Error: (Ser:%08X, Gs:%08X)", nSerVerIndex, nGsNumBer);
		return ;
	}

	DWORD ip= chatconndup.GetIP();

	g_ChannelMgr.B_FreeChannID(pPlayerCommFID->channelid,ip,pSrcInfo->lnID);
}

void CHostConnect::Proc1_PlayerCommunity_Subscribe(const void* pData, size_t size)
{
	if (size < sizeof(PLAYERCOMM_SUBSCRIBE) + sizeof(tagPlusSrcInfo))
		return;

	PLAYERCOMM_SUBSCRIBE* pPlayerCommSub = (PLAYERCOMM_SUBSCRIBE*)pData;
	tagPlusSrcInfo* pSrcInfo = (tagPlusSrcInfo*)((BYTE*)pData + size) - 1;


	CNetConnectDup chatconndup;
	//if (g_HostServer.FindPlayerByRole(NULL, std::_tstring(pSomeoneChatCmd->someone), &hostconndup, NULL, &nameid, &param))
	if (!g_ChatServer.FindPlayerBySerNoAndGsNumber(nSerVerIndex, nGsNumBer, &chatconndup))
	{
		dTRACE("Proc1_PlayerCommunity_FreeChannelID Error: (Ser:%08X, Gs:%08X)", nSerVerIndex, nGsNumBer);
		return ;
	}

	DWORD ip= chatconndup.GetIP();


	if (pPlayerCommSub->subscribe)
		g_ChannelMgr.B_Subscribe(ip, pSrcInfo->lnID, pPlayerCommSub->channelid);//GetIP
	else
		g_ChannelMgr.B_Unsubscribe(ip, pSrcInfo->lnID, pPlayerCommSub->channelid);//GetIP

}


void CHostConnect::OnClientConnectCreate()
{
	rTRACE("host connect create: %08X", GetIP());//_ip2a(GetIP())

}

void CHostConnect::OnClientConnectClose()
{
	rTRACE("host connect close: %08X", GetIP());

	//g_ChannelMgr.B_ClearPlayer_All(GetIP());

	if (!g_RootClient.IsReady())
		return;

	size_t pckgsize = sizeof(RELAY_ASKWAY_DATA) + sizeof(EXTEND_HEADER) + sizeof(KServerInfo);

	RELAY_ASKWAY_DATA* pAskWayData = (RELAY_ASKWAY_DATA*)_alloca(pckgsize);
	pAskWayData->ProtocolFamily = pf_relay;
	pAskWayData->ProtocolID = relay_c2c_askwaydata;
	pAskWayData->nFromIP = 0;
	pAskWayData->nFromRelayID = 0;
	pAskWayData->seekRelayCount = 0;
	pAskWayData->seekMethod = rm_gm;
	pAskWayData->wMethodDataLength = 0;
	pAskWayData->routeDateLength = sizeof(EXTEND_HEADER) + sizeof(KServerInfo);

	EXTEND_HEADER* pHeader = (EXTEND_HEADER*)(pAskWayData + 1);
	pHeader->ProtocolFamily = pf_normal;
	pHeader->ProtocolID = s2c_gatewayinfo;
	KServerInfo* pSvrInfo = (KServerInfo*)(pHeader + 1);
	pSvrInfo->Size = sizeof(KServerInfo);
	pSvrInfo->Type = ServerInfo;
	pSvrInfo->Operate = 0;
	pSvrInfo->Version = ACCOUNT_CURRENT_VERSION;
	pSvrInfo->nValue = 0;
	pSvrInfo->Account[0] = 0;
	pSvrInfo->nServerType = server_Logout;
	pSvrInfo->nValue = GetIP();

	g_RootClient.SendPackage(pAskWayData, pckgsize);
}


BOOL CHostConnect::FindPlayerByAcc(const std::_tstring& acc, std::_tstring* pRole, DWORD* pNameID, unsigned long* pParam, std::_tstring* pRealAcc)
{
	DUMMY_AUTOLOCKREAD(m_lockPlayer);

	ACCMAP::iterator it = m_mapAcc.find(acc);
	if (it == m_mapAcc.end())
		return FALSE;

	const ACCINFO& infoAcc = (*it).second;

	if (pRole != NULL)
		*pRole = infoAcc.role;
	if (pNameID != NULL)
		*pNameID = infoAcc.nameID;
	if (pParam != NULL)
		*pParam = infoAcc.param;

	if (pRealAcc != NULL)
		*pRealAcc = (*it).first;

	return TRUE;
}

BOOL CHostConnect::FindPlayerByRole(const std::_tstring& role, std::_tstring* pAcc, DWORD* pNameID, unsigned long* pParam, std::_tstring* pRealRole)
{
	DUMMY_AUTOLOCKREAD(m_lockPlayer);

	ROLEMAP::iterator it = m_mapRole.find(role);
	if (it == m_mapRole.end())
		return FALSE;

	const ROLEINFO& infoRole = (*it).second;

	if (pAcc != NULL)
		*pAcc = infoRole.acc;
	if (pNameID != NULL)
		*pNameID = infoRole.nameID;
	if (pParam != NULL)
		*pParam = infoRole.param;

	if (pRealRole != NULL)
		*pRealRole = (*it).first;

	return TRUE;
}

UINT CHostConnect::FindHostConnectByidx(unsigned long param)
{
	if (m_sHostServer.count(param))
		return m_sHostServer[param];
	else
		return 0;
}
//这个角色 和 帐号 是否连接 每登陆一个玩家 都记录在案 
BOOL CHostConnect::FindPlayerByParam(unsigned long param, std::_tstring* pAcc, std::_tstring* pRole, DWORD* pNameID)
{
	DUMMY_AUTOLOCKREAD(m_lockPlayer);

	PARAMMAP::iterator it = m_mapParam.find(param);
	if (it == m_mapParam.end())
	{
		rTRACE("----发送(%08X区)消息CHostConnect::FindPlayerByParam失败A-----",param);
		return FALSE;
	}

	const PARAMINFO& infoParam = (*it).second;

	if (pAcc != NULL)
		*pAcc = infoParam.acc;
	if (pRole != NULL)
		*pRole = infoParam.role;
	if (pNameID != NULL)
		*pNameID = infoParam.nameID;

	return TRUE;
}

/*
BOOL CHostConnect::BroadOnPlayer(BOOL byAcc, DWORD fromIP, unsigned long fromRelayID, const void* pData, size_t size)
{
	assert(pData != NULL && size > 0);

	size_t methodsize = _NAME_LEN + sizeof(DWORD) * 2;
	size_t pckgsize = sizeof(RELAY_ASKWAY_DATA) + methodsize + size;

	RELAY_ASKWAY_DATA* pAskwayData = (RELAY_ASKWAY_DATA*)_alloca(pckgsize);
	pAskwayData->ProtocolFamily = pf_relay;
	pAskwayData->ProtocolID = relay_c2c_askwaydata;
	pAskwayData->nFromIP = fromIP;
	pAskwayData->nFromRelayID = fromRelayID;
	pAskwayData->seekRelayCount = 0;
	pAskwayData->seekMethod = byAcc ? rm_account_id : rm_role_id;
	pAskwayData->wMethodDataLength = methodsize;
	pAskwayData->routeDateLength = size;

	char* pTheName = (char*)(pAskwayData + 1);
	DWORD* pTheInfo = (DWORD*)(pTheName + _NAME_LEN);

	void* pRouteData = pTheInfo + 2;
	memcpy(pRouteData, pData, size);


	DUMMY_AUTOLOCKREAD(m_lockPlayer);

	for (PARAMMAP::const_iterator it = m_mapParam.begin(); it != m_mapParam.end(); it++)
	{
		const PARAMINFO& infoParam = (*it).second;
		strcpy(pTheName, byAcc ? infoParam.acc.c_str() : infoParam.role.c_str());
		pTheInfo[0] = infoParam.nameID;
		pTheInfo[1] = (*it).first;

		SendPackage(pAskwayData, pckgsize);
	}

	return TRUE;
}
*/

void CHostConnect::PrepareRecvs()
{
}

void CHostConnect::UnprepareRecvs()
{
	g_ChannelMgr.DoBlockOp(-1);
}


size_t CHostConnect::GetPlayerCount()
{
	DUMMY_AUTOLOCKREAD(m_lockPlayer);

	return m_mapRole.size();
}

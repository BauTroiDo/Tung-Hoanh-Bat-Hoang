// ChatConnect.cpp: implementation of the CChatConnect class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ChatConnect.h"
#include "ChatServer.h"
#include "S3Relay.h"
#include "Global.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CChatConnect::CChatConnect(CChatServer* pChatServer, unsigned long id)
	: CNetConnect(pChatServer, id)
{
	nSerVerIndex=1;
}

CChatConnect::~CChatConnect()
{
	nSerVerIndex=1;
}


void CChatConnect::OnClientConnectCreate()
{
	rTRACE("chat connect create: %08X",GetIP());//通知 S3 已经连接到的ip_ip2a(GetIP())
}

void CChatConnect::OnClientConnectClose()
{
	rTRACE("chat connect close:  %08X", GetIP());
	g_ChannelMgr.B_ClearPlayer_All(GetIP());
}

void CChatConnect::Proc0_PlayerCommunity(const void* pData, size_t size)
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
void CChatConnect::Proc1_PlayerCommunity_QueryChannelID(const void* pData, size_t size)
{
	if (size < sizeof(PLAYERCOMM_QUERYCHANNELID) + sizeof(tagPlusSrcInfo))
	{
		rTRACE("CChatConnect 错误:协议长度不一样");
		return;
	}
	PLAYERCOMM_QUERYCHANNELID* pPlayerCommQID = (PLAYERCOMM_QUERYCHANNELID*)pData;
	tagPlusSrcInfo* pSrcInfo = (tagPlusSrcInfo*)((BYTE*)pData + size) - 1;

	//std::_tstring rolename;//获取角色名称

	//if (!FindPlayerByParam(pSrcInfo->lnID, NULL, &rolename, NULL))//人物的GS连接id 查找出角色名
	//{
	//	rTRACE("CChatConnect 错误:包角色信息异常:%s | %u",rolename.c_str(),pSrcInfo->lnID);
	//	return;
	//}

	CNetConnectDup chatconndup;
	//if (g_HostServer.FindPlayerByRole(NULL, std::_tstring(pSomeoneChatCmd->someone), &hostconndup, NULL, &nameid, &param))
	if (!g_ChatServer.FindPlayerBySerNoAndGsNumber(nSerVerIndex, nGsNumBer, &chatconndup))
	{
		dTRACE("Proc1_PlayerCommunity_FreeChannelID Error: (Ser:%08X, Gs:%08X)", nSerVerIndex, nGsNumBer);
		return ;
	}

	DWORD ip= chatconndup.GetIP();

	g_ChannelMgr.B_QueryChannelID(pPlayerCommQID->channel,ip, pSrcInfo->lnID, pSrcInfo->nameid,GetIP());
}

void CChatConnect::Proc1_PlayerCommunity_FreeChannelID(const void* pData, size_t size)
{
	if (size < sizeof(PLAYERCOMM_FREECHANNELID) + sizeof(tagPlusSrcInfo))
		return;

	PLAYERCOMM_FREECHANNELID* pPlayerCommFID = (PLAYERCOMM_FREECHANNELID*)pData;
	tagPlusSrcInfo* pSrcInfo = (tagPlusSrcInfo*)((BYTE*)pData + size) - 1;

	g_ChannelMgr.B_FreeChannID(pPlayerCommFID->channelid, GetIP(), pSrcInfo->lnID);
}

void CChatConnect::Proc1_PlayerCommunity_Subscribe(const void* pData, size_t size)
{
	if (size < sizeof(PLAYERCOMM_SUBSCRIBE) + sizeof(tagPlusSrcInfo))
		return;

	PLAYERCOMM_SUBSCRIBE* pPlayerCommSub = (PLAYERCOMM_SUBSCRIBE*)pData;
	tagPlusSrcInfo* pSrcInfo = (tagPlusSrcInfo*)((BYTE*)pData + size) - 1;

	if (pPlayerCommSub->subscribe)
		g_ChannelMgr.B_Subscribe(GetIP(), pSrcInfo->lnID, pPlayerCommSub->channelid);//GetIP
	else
		g_ChannelMgr.B_Unsubscribe(GetIP(), pSrcInfo->lnID, pPlayerCommSub->channelid);//GetIP

}


void CChatConnect::Proc0_ServerCommunity(const void* pData, size_t size)
{
	EXTEND_HEADER* pHeader = (EXTEND_HEADER*)pData;

	if (pHeader->ProtocolID == relay_s2c_udateserver)
	{//
		tagLeaveGame2 * pServerData = (tagLeaveGame2*)pData;
		nSerVerIndex = (short)HIWORD(pServerData->nSelServer);//pServerData->nSelServer;//属于哪个区的gs
		nGsNumBer    =  LOWORD(pServerData->nSelServer);
		m_sChatServer[nSerVerIndex] = GetID();//保存 各个 服务器客户端的连接号 即是GS 的标示编号
		setSerRegIndex(nSerVerIndex);
		setGsNumIndex(nGsNumBer);
		rTRACE("---Chat:(%d)区服务器启动完毕:%08X----",nSerVerIndex,nGsNumBer);
	}
}

//收到数据包
void CChatConnect::RecvPackage(const void* pData, size_t size)
{
	BYTE protocol = *(BYTE*)pData;

	if (protocol == chat_someonechat)  // 收到密聊的消息
	{
		rTRACE("--------------收到GS密聊chat_someonechat消息！-------------");
		Proc0_SomeoneChat(pData, size);	
	}
	else if (protocol == chat_channelchat)
	{
		rTRACE("--------------收到chat_channelchat消息！-------------");
		Proc0_ChannelChat(pData, size);
	}
	else if (protocol == chat_groupman)
	{
		rTRACE("--------------收到chat_groupman频道消息！-------------");
		Proc0_GroupMan(pData, size);
	}
	else
	{//其他的
		EXTEND_HEADER* pHeader = (EXTEND_HEADER*)pData;
		if (pHeader->ProtocolFamily == pf_normal)
		{
			Proc0_Normal(pData, size);
		}
		else if (pHeader->ProtocolFamily == pf_udataserveridx)
		{//更新当前服务器的索引
			Proc0_ServerCommunity(pData, size);
		}
		else if (pHeader->ProtocolFamily == pf_playercommunity)
		{//增加聊天频道
			rTRACE("--------------收到Proc0_PlayerCommunity频道消息！-------------");
			Proc0_PlayerCommunity(pData, size);
		}
	}
}
//秘密聊
void CChatConnect::Proc0_SomeoneChat(const void* pData, size_t size)
{
	CHAT_SOMEONECHAT_CMD* pCsc = (CHAT_SOMEONECHAT_CMD*)pData;
	tagPlusSrcInfo* pSrcInfo = (tagPlusSrcInfo*)((BYTE*)pData + size) - 1;

	g_ChannelMgr.SomeoneChat(GetIP(), *pSrcInfo, pCsc);
}
////城市 和 世界 ，队伍 门派
void CChatConnect::Proc0_ChannelChat(const void* pData, size_t size)
{
	CHAT_CHANNELCHAT_CMD* pCcc = (CHAT_CHANNELCHAT_CMD*)pData;
	tagPlusSrcInfo* pSrcInfo = (tagPlusSrcInfo*)((BYTE*)pData + size) - 1;

	g_ChannelMgr.ChannelChat(GetIP(), *pSrcInfo, pCcc);
}

void CChatConnect::Proc0_GroupMan(const void* pData, size_t size)
{
	CHAT_GROUPMAN* pCgc = (CHAT_GROUPMAN*)pData;
	CHAT_CHANNELCHAT_SYNC* pCccSync = (CHAT_CHANNELCHAT_SYNC*)(pCgc + 1);
	
	if (pCccSync->ProtocolType != chat_channelchat)
		return;	//unexpect

	void* pSent = pCccSync + 1;
	void* pPlayersData = (BYTE*)pSent + pCccSync->sentlen;


	DWORD srcIP = GetIP();

	const size_t playercount = pCgc->wPlayerCount;
	pCgc->wPlayerCount = 0;

	if (pCgc->byHasIdentify)
	{
		tagPlusSrcInfo* pPlayers = (tagPlusSrcInfo*)(pPlayersData);
		for (size_t i = 0; i < playercount; ++i)
		{
			if (g_ChannelMgr.IsSubscribed(srcIP, pPlayers[i].lnID, pCccSync->channelid))
			{
				if (pCgc->wPlayerCount < i)
					pPlayers[pCgc->wPlayerCount] = pPlayers[i];
				++ pCgc->wPlayerCount;
			}
		}
	}
	else
	{
		WORD* pPlayers = (WORD*)(pPlayersData);
		for (size_t i = 0; i < playercount; ++i)
		{
			if (g_ChannelMgr.IsSubscribed(srcIP, (unsigned long)pPlayers[i], pCccSync->channelid))
			{
				if (pCgc->wPlayerCount < i)
					pPlayers[pCgc->wPlayerCount] = pPlayers[i];
				++ pCgc->wPlayerCount;
			}
		}
	}

	if (pCgc->wPlayerCount <= 0)
		return;


	size_t size2 = size - (playercount - pCgc->wPlayerCount) * (pCgc->byHasIdentify ? sizeof(tagPlusSrcInfo) : sizeof(WORD));
	pCgc->wSize = size2 - 1;
	///rTRACE("--------------发送 城市 和 世界 队伍 门派消息！-------------");
	SendPackage(pCgc, size2);
}


void CChatConnect::Proc0_Normal(const void* pData, size_t size)
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
void CChatConnect::Proc1_Normal_EnterGame(const void* pData, size_t size)
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
	infoRole.acc =  strAcc;
	infoRole.nameID = pEnterGame2->dwNameID;
	infoRole.param = pEnterGame2->lnID;//客户端连接GS的网络唯一标示 多区会覆盖掉

	PARAMINFO infoParam;
	infoParam.acc = strAcc;
	infoParam.role = strRole;
	infoParam.nameID = pEnterGame2->dwNameID;

	{{
		DUMMY_AUTOLOCKWRITE(m_lockPlayer);

		m_mapAcc[strAcc] = infoAcc;  //这个帐号下的角色信息
		m_mapRole[strRole] = infoRole;//这个角色下的 帐号信息
		m_mapParam[pEnterGame2->lnID] = infoParam;//有可能重复的 多区的时候 会覆盖掉 需要调整
	}}

	//{{
	//	g_FriendMgr.SomeoneLogin(strRole,nGsNetIdx);//通知 这个 角色已经上线 
	//}}

	rTRACE("Chat:player login: %s [%s] (%08X,%08X)", strAcc.c_str(), strRole.c_str(), GetIP(), pEnterGame2->lnID,pEnterGame2->nSelSerVer);
}
//离开游戏调用
void CChatConnect::Proc1_Normal_LeaveGame(const void* pData, size_t size)
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
				g_ChannelMgr.B_ClearPlayer(GetIP(), rAccInfo.param);
				//g_FriendMgr.SomeoneLogout(rAccInfo.role);
			}}


			m_mapRole.erase(rAccInfo.role);
			m_mapParam.erase(rAccInfo.param);

			m_mapAcc.erase(itAcc);
		}
	}}
	rTRACE("---Chat:(%u区)玩家账号(%s)退出游戏---",pLeaveGame->nSelServer, strAcc.c_str());
}

BOOL CChatConnect::FindPlayerByAcc(const std::_tstring& acc, std::_tstring* pRole, DWORD* pNameID, unsigned long* pParam, std::_tstring* pRealAcc)
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

BOOL CChatConnect::FindPlayerByRole(const std::_tstring& role, std::_tstring* pAcc, DWORD* pNameID, unsigned long* pParam, std::_tstring* pRealRole)
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

//这个角色 和 帐号 是否连接 每登陆一个玩家 都记录在案 
BOOL CChatConnect::FindPlayerByParam(unsigned long param, std::_tstring* pAcc, std::_tstring* pRole, DWORD* pNameID)
{
	DUMMY_AUTOLOCKREAD(m_lockPlayer);

	PARAMMAP::iterator it = m_mapParam.find(param);
	if (it == m_mapParam.end())
	{
		rTRACE("----发送(%08X)消息CChatConnect::FindPlayerByParam失败A-----",param);
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
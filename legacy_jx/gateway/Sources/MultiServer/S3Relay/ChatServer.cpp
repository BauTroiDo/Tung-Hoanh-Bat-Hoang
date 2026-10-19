// ChatServer.cpp: implementation of the CChatServer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ChatServer.h"
#include "ChatConnect.h"
#include "S3Relay.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CChatServer::CChatServer()
{

}

CChatServer::~CChatServer()
{

}


CNetConnect* CChatServer::CreateConnect(CNetServer* pNetServer, unsigned long id)
{
	return new CChatConnect((CChatServer*)pNetServer, id);
}

void CChatServer::DestroyConnect(CNetConnect* pConn)
{
	delete pConn;
	pConn=NULL;
}


void CChatServer::OnBuildup()
{
	AUTOLOCKWRITE(m_lockIpMap);

	rTRACE("chat server startup");
}

void CChatServer::OnClearup()
{
	AUTOLOCKWRITE(m_lockIpMap);

	m_mapIp2Connect.clear();

	rTRACE("chat server shutdown");
}
//赋值给这个IP 已经连接
void CChatServer::OnClientConnectCreate(CNetConnect* pConn)
{
	AUTOLOCKWRITE(m_lockIpMap);

	m_mapIp2Connect[pConn->GetIP()] = (CChatConnect*)pConn;
}
//清除已经连接的IP 容器 
void CChatServer::OnClientConnectClose(CNetConnect* pConn)
{
	AUTOLOCKWRITE(m_lockIpMap);

	m_mapIp2Connect.erase(pConn->GetIP());//删除这个KEY 
}

CNetConnectDup CChatServer::FindChatConnectByIP(DWORD IP)
{    
	AUTOLOCKREAD(m_lockIpMap);

	IP2CONNECTMAP::iterator it = m_mapIp2Connect.find(IP);
	if (it == m_mapIp2Connect.end())
		return CNetConnectDup();

	CChatConnect* pChatConn = (*it).second;
	if (!pChatConn)
		return CNetConnectDup();

	return CNetConnectDup(*pChatConn);
}


//通过IP找到 是哪个 GS 在连接 并属于哪个服务器 的人物数据
BOOL CChatServer::FindPlayerByIpParam(CChatConnect* pConn, DWORD ip, unsigned long param, CNetConnectDup* pConnDup, std::_tstring* pAcc, std::_tstring* pRole, DWORD* pNameID,bool isReturn)
{
	AUTOLOCKREAD(m_lockIpMap); //param 为玩家客户端连接 GS 的网络号标示IDX
	//这里要修改成为 网络IDX 查询才行  因为一机多区 是同一个IP 这样就区分不了是哪个GS 的数据
	IP2CONNECTMAP::const_iterator it = m_mapIp2Connect.find(ip);//查找时哪个GS的 多区时会覆盖掉
	if (it == m_mapIp2Connect.end())
	{
		rTRACE("----发送(%08X区)消息CChatServer::FindPlayerByIpParam失败A-----",ip);
		return FALSE;
	}

	if  (isReturn)
		return true;

	CChatConnect* pConnMe = (*it).second;

	if (pConnMe != pConn)
	{
		if (pConnMe->FindPlayerByParam(param, pAcc, pRole, pNameID))
		{
			if (pConnDup != NULL)
				*pConnDup = *pConnMe;

			return TRUE;
		}
	}

	return FALSE;
}


BOOL CChatServer::FindPlayerByAcc(CChatConnect* pConn, const std::_tstring& acc, CNetConnectDup* pConnDup, std::_tstring* pRole, DWORD* pNameID, unsigned long* pParam, std::_tstring* pRealAcc)
{
	AUTOLOCKREAD(m_lockIpMap);

	for (IP2CONNECTMAP::iterator it = m_mapIp2Connect.begin(); it != m_mapIp2Connect.end(); ++it)
	{
		CChatConnect* pConnMe = (*it).second;

		if (pConnMe != pConn)
		{
			if (pConnMe->FindPlayerByAcc(acc, pRole, pNameID, pParam, pRealAcc))
			{
				if (pConnDup != NULL)
					*pConnDup = *pConnMe;

				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL CChatServer::FindPlayerBySerNoAndGsNumber(unsigned int _nSerVerIndex, unsigned int _nGsNumBer,CNetConnectDup* pConnDup)
{
	AUTOLOCKREAD(m_lockIpMap);

	for (IP2CONNECTMAP::iterator it = m_mapIp2Connect.begin(); it != m_mapIp2Connect.end(); ++it)
	{
		CChatConnect* pConnMe = (*it).second;

		if (pConnMe != NULL)
		{
			if (pConnMe->getSerRegIndex()==_nSerVerIndex && pConnMe->getGsNumIndex()==_nGsNumBer)
			{
				if (pConnDup != NULL)
					*pConnDup = *pConnMe;

				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL CChatServer::FindPlayerByParam_new(CChatConnect* pConn,unsigned long param, CNetConnectDup* pConnDup, std::_tstring* pAcc, std::_tstring* pRole, DWORD* pNameID)
{

	AUTOLOCKREAD(m_lockIpMap);

	for (IP2CONNECTMAP::iterator it = m_mapIp2Connect.begin(); it != m_mapIp2Connect.end(); ++it)
	{
		CChatConnect* pConnMe = (*it).second;

		if (pConnMe != pConn)
		{
			if (pConnMe->FindPlayerByParam(param, pAcc, pRole, pNameID))
			{
				if (pConnDup != NULL)
					*pConnDup = *pConnMe;

				return TRUE;
			}
		}
	}

	return FALSE;
}


BOOL CChatServer::FindPlayerByRole(CChatConnect* pConn, const std::_tstring& role, CNetConnectDup* pConnDup, std::_tstring* pAcc, DWORD* pNameID, unsigned long* pParam, std::_tstring* pRealRole)
{
	AUTOLOCKREAD(m_lockIpMap);

	for (IP2CONNECTMAP::iterator it = m_mapIp2Connect.begin(); it != m_mapIp2Connect.end(); ++it)
	{
		CChatConnect* pConnMe = (*it).second;

		if (pConnMe != pConn)
		{
			if (pConnMe->FindPlayerByRole(role, pAcc, pNameID, pParam, pRealRole))
			{
				if (pConnDup != NULL)
					*pConnDup = *pConnMe;

				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL CChatServer::SendDataToGameServer(const void* pData, size_t size)
{    
	AUTOLOCKREAD(m_lockIpMap);

	/*IP2CONNECTMAP::iterator it = m_mapIp2Connect.find(IP);
	if (it == m_mapIp2Connect.end())
		return CNetConnectDup();*/

	for (IP2CONNECTMAP::iterator it = m_mapIp2Connect.begin(); it != m_mapIp2Connect.end(); ++it)
	{
		CChatConnect* pChatConn = (*it).second;
		if (pChatConn)
		{
			//CNetConnectDup connDup = CNetConnectDup(*pChatConn);
			//if (connDup.IsValid())
			//{
				pChatConn->SendPackage(pData,size);
			//}
		}
	}

	return TRUE;
}

BOOL CChatServer::TraceInfo()
{
	AUTOLOCKREAD(m_lockIpMap);

	std::_tstring info("message: [ChatServer] ");
	char buffer[_MAX_PATH];

	sprintf(buffer, "<total: %zu> : ", m_mapIp2Connect.size());
	info.append(buffer);

	for (IP2CONNECTMAP::iterator it = m_mapIp2Connect.begin(); it != m_mapIp2Connect.end(); ++it)
	{
		if (it != m_mapIp2Connect.begin())
			info.append(", ");

		sprintf(buffer, "%08X", (*it).first);
		info.append(buffer);
	}

	rTRACE(info.c_str());

	return TRUE;
}

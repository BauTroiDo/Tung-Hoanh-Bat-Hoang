// TongServer.cpp: implementation of the CTongServer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TongServer.h"
#include "TongConnect.h"
#include "S3Relay.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTongServer::CTongServer()
{

}

CTongServer::~CTongServer()
{

}


CNetConnect* CTongServer::CreateConnect(CNetServer* pNetServer, unsigned long id)
{
	return new CTongConnect((CTongServer*)pNetServer, id);
}

void CTongServer::DestroyConnect(CNetConnect* pConn)
{
	delete pConn;
	pConn=NULL;
}


void CTongServer::OnBuildup()
{
	AUTOLOCKWRITE(m_lockIpMap);

	rTRACE("tong server startup");
}

void CTongServer::OnClearup()
{
	AUTOLOCKWRITE(m_lockIpMap);

	m_mapIp2Connect.clear();

	rTRACE("tong server shutdown");
}


void CTongServer::OnClientConnectCreate(CNetConnect* pConn)
{
	AUTOLOCKWRITE(m_lockIpMap);

	m_mapIp2Connect[pConn->GetIP()] = (CTongConnect*)pConn;//GetIP
	//rTRACE("HHHHHHHHHHHHHHHHHHHHHH: Tong server IP: %ld", pConn->GetIP());
}

void CTongServer::OnClientConnectClose(CNetConnect* pConn)
{
	AUTOLOCKWRITE(m_lockIpMap);

	m_mapIp2Connect.erase(pConn->GetIP());
}


CNetConnectDup CTongServer::FindTongConnectByIP(DWORD IP)
{
	AUTOLOCKREAD(m_lockIpMap);

	IP2CONNECTMAP::iterator it = m_mapIp2Connect.find(IP);
	if (it == m_mapIp2Connect.end())
		return CNetConnectDup();

	CTongConnect* pTongConn = (*it).second;
	if (!pTongConn)
		return CNetConnectDup();

	return CNetConnectDup(*pTongConn);
}


BOOL CTongServer::TraceInfo()
{
	AUTOLOCKREAD(m_lockIpMap);

	std::_tstring info("message: [TongServer] ");
	char buffer[_MAX_PATH];

	sprintf(buffer, "<total: %zu> : ", m_mapIp2Connect.size());
	info.append(buffer);

	size_t idx = 0;
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

//通过IP找到 是哪个 GS 在连接 并属于哪个服务器 的人物数据
BOOL CTongServer::FindPlayerByIpParam(CTongConnect* pConn, DWORD ip, unsigned long param, CNetConnectDup* pConnDup, std::_tstring* pAcc, std::_tstring* pRole, DWORD* pNameID)
{
	AUTOLOCKREAD(m_lockIpMap); //param 为玩家客户端连接 GS 的网络号标示IDX
	//这里要修改成为 网络IDX 查询才行  因为一机多区 是同一个IP 这样就区分不了是哪个GS 的数据
	IP2CONNECTMAP::const_iterator it = m_mapIp2Connect.find(ip);//查找时哪个GS的 多区时会覆盖掉
	if (it == m_mapIp2Connect.end())
	{
		rTRACE("----发送(%08X区)消息CTongServer::FindPlayerByIpParam失败A-----",ip);
		return FALSE;
	}

	CTongConnect* pConnMe = (*it).second;

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

BOOL CTongServer::FindPlayerByRole(CTongConnect* pConn, const std::_tstring& role, CNetConnectDup* pConnDup, std::_tstring* pAcc, DWORD* pNameID, unsigned long* pParam, std::_tstring* pRealRole)
{
	AUTOLOCKREAD(m_lockIpMap);

	for (IP2CONNECTMAP::iterator it = m_mapIp2Connect.begin(); it != m_mapIp2Connect.end(); ++it)
	{
		CTongConnect* pConnMe = (*it).second;

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
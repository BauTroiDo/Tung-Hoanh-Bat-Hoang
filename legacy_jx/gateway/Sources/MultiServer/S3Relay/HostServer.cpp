// HostServer.cpp: implementation of the CHostServer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HostServer.h"
#include "HostConnect.h"
#include "S3Relay.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHostServer::CHostServer()
{
	m_ip=0;
}

CHostServer::~CHostServer()
{

}


CNetConnect* CHostServer::CreateConnect(CNetServer* pNetServer, unsigned long id)
{
	return new CHostConnect((CHostServer*)pNetServer, id);
}

void CHostServer::DestroyConnect(CNetConnect* pConn)
{
	delete pConn;
}

void CHostServer::OnBuildup()
{
	AUTOLOCKWRITE(m_lockIpMap);

	rTRACE("host server startup");
}

void CHostServer::OnClearup()
{
	AUTOLOCKWRITE(m_lockIpMap);

	m_mapIp2Connect.clear();

	m_mapidx2Connect.clear();

	rTRACE("host server shutdown");
}
//主服务器
void CHostServer::OnClientConnectCreate(CNetConnect* pConn)
{
	AUTOLOCKWRITE(m_lockIpMap);
	rTRACE("----创建CHostServer网络(%08X) %u 成功-----",pConn->GetIP(),pConn->GetID());
	m_mapIp2Connect[pConn->GetIP()] = (CHostConnect*)pConn;//赋值这个链接IP的链接数据
	m_mapidx2Connect[pConn->GetID()] = (CHostConnect*)pConn;//含端口
	//rTRACE("HHHHHHHHHHHHHHHHHHHHHH: Host server IP: %ld", pConn->GetIP());
}

void CHostServer::OnClientConnectClose(CNetConnect* pConn)
{
	AUTOLOCKWRITE(m_lockIpMap);
	rTRACE("----移除CHostServer(%08X) &u 成功-----",pConn->GetIP(),pConn->GetID());
	m_mapIp2Connect.erase(pConn->GetIP());
	m_mapidx2Connect.erase(pConn->GetID());
}
//bishop 专用
CNetConnectDup CHostServer::FindHostConnectByIP(DWORD IP)
{
	AUTOLOCKREAD(m_lockIpMap);

	IP2CONNECTMAP::iterator it = m_mapIp2Connect.find(IP);
	if (it == m_mapIp2Connect.end())
		return CNetConnectDup();

	CHostConnect* pHostConn = (*it).second;
	if (!pHostConn)
		return CNetConnectDup();

	return CNetConnectDup(*pHostConn);
}

CNetConnectDup CHostServer::FindHostConnectByMapID(int nMapID)
{
	AUTOLOCKREAD(m_lockIpMap);

	for (IP2CONNECTMAP::iterator it = m_mapIp2Connect.begin(); it != m_mapIp2Connect.end(); ++it)
	{
		CHostConnect* pConnMe = (*it).second;
			if (pConnMe->checkMapIn(nMapID)) //如果存在就返回
			   return CNetConnectDup(*pConnMe);
	}
	return CNetConnectDup();
}

CNetConnectDup CHostServer::FindHostConnectByID(DWORD IP)
{
	AUTOLOCKREAD(m_lockIpMap);

	IP2CONNECTMAP::iterator it = m_mapidx2Connect.find(IP);
	if (it == m_mapidx2Connect.end())
		return CNetConnectDup();

	CHostConnect* pHostConn = (*it).second;
	if (!pHostConn)
		return CNetConnectDup();

	return CNetConnectDup(*pHostConn);
}

BOOL CHostServer::FindPlayerByAcc(CHostConnect* pConn, const std::_tstring& acc, CNetConnectDup* pConnDup, std::_tstring* pRole, DWORD* pNameID, unsigned long* pParam, std::_tstring* pRealAcc)
{
	AUTOLOCKREAD(m_lockIpMap);

	for (IP2CONNECTMAP::iterator it = m_mapIp2Connect.begin(); it != m_mapIp2Connect.end(); ++it)
	{
		CHostConnect* pConnMe = (*it).second;

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

BOOL CHostServer::FindPlayerByRole(CHostConnect* pConn, const std::_tstring& role, CNetConnectDup* pConnDup, std::_tstring* pAcc, DWORD* pNameID, unsigned long* pParam, std::_tstring* pRealRole)
{
	AUTOLOCKREAD(m_lockIpMap);

	for (IP2CONNECTMAP::iterator it = m_mapIp2Connect.begin(); it != m_mapIp2Connect.end(); ++it)
	{
		CHostConnect* pConnMe = (*it).second;

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

BOOL CHostServer::SendDataToAllGameServer(const void* pData, size_t size)
{
	AUTOLOCKREAD(m_lockIpMap);

	for (IP2CONNECTMAP::iterator it = m_mapIp2Connect.begin(); it != m_mapIp2Connect.end(); ++it)
	{
		CHostConnect* pConnMe = (*it).second;

		if (pConnMe)
		{
		   pConnMe->SendPackage(pData,size);
		}
	}

	return TRUE;
}

BOOL CHostServer::FindPlayerBySerNoAndGsNumber(unsigned int _nSerVerIndex, unsigned int _nGsNumBer,CNetConnectDup* pConnDup)
{
	AUTOLOCKREAD(m_lockIpMap);

	for (IP2CONNECTMAP::iterator it = m_mapIp2Connect.begin(); it != m_mapIp2Connect.end(); ++it)
	{
		CHostConnect* pConnMe = (*it).second;

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
//通过IP找到 是哪个 GS 在连接 并属于哪个服务器 的人物数据
BOOL CHostServer::FindPlayerByParam_new(CHostConnect* pConn,unsigned long param, CNetConnectDup* pConnDup, std::_tstring* pAcc, std::_tstring* pRole, DWORD* pNameID)
{

	AUTOLOCKREAD(m_lockIpMap);

	for (IP2CONNECTMAP::iterator it = m_mapIp2Connect.begin(); it != m_mapIp2Connect.end(); ++it)
	{
		CHostConnect* pConnMe = (*it).second;

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

//通过IP找到 是哪个 GS 在连接 并属于哪个服务器 的人物数据
BOOL CHostServer::FindPlayerByIpParam(CHostConnect* pConn, DWORD ip, unsigned long param, CNetConnectDup* pConnDup, std::_tstring* pAcc, std::_tstring* pRole, DWORD* pNameID)
{
	AUTOLOCKREAD(m_lockIpMap); //param 为玩家客户端连接 GS 的网络号标示IDX
	//这里要修改成为 网络IDX 查询才行  因为一机多区 是同一个IP 这样就区分不了是哪个GS 的数据
	IP2CONNECTMAP::const_iterator it = m_mapIp2Connect.find(ip);//查找时哪个GS的 多区时会覆盖掉
	if (it == m_mapIp2Connect.end())
	{
		rTRACE("----发送(%08X区)消息CHostServer::FindPlayerByIpParam失败A-----",ip);
		return FALSE;
	}

	CHostConnect* pConnMe = (*it).second;

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

/*
BOOL CHostServer::BroadOnPlayer(BOOL byAcc, DWORD fromIP, unsigned long fromRelayID, const void* pData, size_t size)
{
	AUTOLOCKREAD(m_lockIpMap);

	for (IP2CONNECTMAP::const_iterator it = m_mapIp2Connect.begin(); it != m_mapIp2Connect.end(); ++it)
	{
		CHostConnect* pConnMe = (*it).second;

		pConnMe->BroadOnPlayer(byAcc, fromIP, fromRelayID, pData, size);
	}

	return FALSE;
}
*/


size_t CHostServer::GetPlayerCount()
{
	AUTOLOCKREAD(m_lockIpMap);

	size_t total = 0;

	for (IP2CONNECTMAP::iterator it = m_mapIp2Connect.begin(); it != m_mapIp2Connect.end(); ++it)
	{
		CHostConnect* pConn = (*it).second;

		assert(pConn);
		if (pConn)
			total += pConn->GetPlayerCount();
	}

	return total;
}


BOOL CHostServer::TraceInfo()
{
	AUTOLOCKREAD(m_lockIpMap);

	std::_tstring info("message: [HostServer] ");
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

BOOL CHostServer::TracePlayer()
{
	AUTOLOCKREAD(m_lockIpMap);

	std::_tstring info("message: [Player] : ");
	char buffer[_MAX_PATH];

	size_t total = 0;

	for (IP2CONNECTMAP::iterator it = m_mapIp2Connect.begin(); it != m_mapIp2Connect.end(); ++it)
	{
		CHostConnect* pConn = (*it).second;

		assert(pConn);
		if (pConn)
		{
			if (it != m_mapIp2Connect.begin())
				info.append(", ");

			size_t count = pConn->GetPlayerCount();
			total += count;

			sprintf(buffer, "%08X: %zu", (*it).first, count);
			info.append(buffer);
		}
	}

	sprintf(buffer, " <total: %zu>", total);
	info.append(buffer);

	rTRACE(info.c_str());

	return TRUE;
}


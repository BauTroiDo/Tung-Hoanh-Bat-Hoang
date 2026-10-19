// NetConnect.cpp: implementation of the CNetConnect class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Global.h"
#include "NetConnect.h"
#include "NetServer.h"

#include "S3Relay.h"

#include "../../Multiserver/Heaven/Interface/IServer.h"
#include "../../../headers/KProtocolDef.h"
#include "../../../headers/KProtocol.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNetConnectDup::CNetConnectDup(const CNetConnect& netconnect)
	 : m_pServer(NULL), m_IP(netconnect.GetIP()), m_id(netconnect.GetID())
{
	CNetServer* pNetServer = netconnect.GetServer();
	if (pNetServer)
	{
		m_pServer = pNetServer->GetServer();
		_SafeAddRef(m_pServer);
	}
}

BOOL CNetConnectDup::SendPackage(const void* pData, size_t size) const
{
	dTRACKSENDDATA(pData, size);

	if(!m_pServer)
		return FALSE;

	m_pServer->SendData(m_id, pData, size);
	return TRUE;
}

CNetConnectDup& CNetConnectDup::operator =(const CNetConnect& src)
{
	_SafeRelease(m_pServer);
	CNetServer* pNetServer = src.GetServer();
	if (pNetServer)
	{
		m_pServer = pNetServer->GetServer();
		_SafeAddRef(m_pServer);
	}
	m_IP = src.GetIP();
	m_id = src.GetID();
	return *this;
}

 DWORD g_FileName2Id(LPSTR lpFileName)
{
	////////////////////////////////////////////////////////////////
	if (lpFileName)
	{
		DWORD dwID = 0; //

		for (int i = 0;lpFileName[i]; ++i )
		{
			dwID =(dwID + (i + 1) * lpFileName[i])%0x8000000B*0xFFFFFFEF;
		}

		return (dwID ^ 0x12345678);	
	}

	return (DWORD)(-1);
}

CNetConnect::CNetConnect(CNetServer* pNetServer, unsigned long id)
	: m_pNetServer(pNetServer), m_id(id), m_IP(0), m_port(0), m_disconnect(0L),nSerVerIndex(1),nGsNumBer(1)//初始化 m_pNetServer 指针 已经是对应的网络号
{
	assert(m_pNetServer);
	char pNesInfo[128]={0};
	//const char* pInfo = 	
	m_pNetServer->GetServer()->GetClientInfo(id,pNesInfo,sizeof(pNesInfo));//获取GS连接的IP信息

	//char pNesInfo[128]={0};
	//sprintf_s(pNesInfo,sizeof(pNesInfo)-1,"%s",pInfo);

	if (pNesInfo[0])
	{
	    //::MessageBox(NULL,pNesInfo, "Warning", MB_OK | MB_ICONSTOP );
		m_IP = g_FileName2Id(pNesInfo);//_a2ip(pNesInfo);
		char* pColon =strchr(pNesInfo, ':');
		if (pColon)
			m_port = atoi(pColon + 1);  //加为了去除冒号  //端口
	}
}

CNetConnect::~CNetConnect()
{
}


void CNetConnect::_NotifyClientConnectCreate()
{
	OnClientConnectCreate();
}
//通知断开连接
void CNetConnect::_NotifyClientConnectClose()
{
	if (m_disconnect)
		return;
	++ m_disconnect;

	OnClientConnectClose();
}
//接受GS包
HRESULT CNetConnect::OnMessage( VOID* pData, size_t size )
{
	dTRACKRECVDATA(pData, size);
	RecvPackage(pData, size);

	return S_OK;
}

DWORD CNetConnect::Main(LPVOID lpParam)
{
#ifndef _WORKMODE_SINGLETHREAD
	EnterLoop();
#endif
//无限循环
	while (!IsAskingStop())
	{
#ifdef _WORKMODE_MULTITHREAD2
		BeginRoute();
		EndRoute();
		Sleep(2000);
		continue;
#else
		DoRoute();
		Sleep(breathe_interval);
#endif
	}

#ifndef _WORKMODE_SINGLETHREAD
	LeaveLoop();
#endif

	return 0;
}
//发包 向GS发包

BOOL CNetConnect::SendPackage(const void* pData, size_t size)
{
	IServer* pServer = m_pNetServer->GetServer();
	if (pServer == NULL)
		return FALSE;

	dTRACKSENDDATA(pData, size);
	pServer->SendData(m_id, pData, size);
	return TRUE;
}

BOOL CNetConnect::IsReady() const
{
	return m_pNetServer->GetServer() != NULL;
}

BOOL CNetConnect::DoRoute()
{
	IServer* pServer = m_pNetServer->GetServer();
	if (pServer == NULL)
		return FALSE;

	BeginRoute();

	for ( ; ; )
	{
		size_t size = 0;
		const void* pData = pServer->GetPackFromClient(m_id, size);//从这个网络号的GS接受包
		if (!pData || size <= 0)
			break;

		dTRACKRECVDATA(pData, size);
		RecvPackage(pData, size);
	}

	EndRoute();

	return TRUE;
}

BOOL CNetConnect::Route()
{
	return DoRoute();
}

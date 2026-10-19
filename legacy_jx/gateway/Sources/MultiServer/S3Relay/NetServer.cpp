// NetServer.cpp: implementation of the CNetServer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Global.h"
#include "NetServer.h"
#include "S3Relay.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNetServer::CNetServer()
	: m_pServer(NULL), m_ready(FALSE)
{
}

CNetServer::~CNetServer()
{
	//assert(!m_pServer);
	//_SafeRelease(m_pServer);
}


void __stdcall CNetServer::ServerEventNotify(
			LPVOID lpParam,
			const unsigned long &ulnID,
			const unsigned long &ulnEventType )
{
	CNetServer* pThis = (CNetServer*)lpParam;//
	assert(pThis);

	switch( ulnEventType )
	{
	case enumClientConnectCreate:
		pThis->_NotifyClientConnectCreate(ulnID);//连接这个编号
		//rTRACE("连接: 网络号:%u 成功！",ulnID);
		break;
	case enumClientConnectClose:
		pThis->_NotifyClientConnectClose(ulnID);
		//rTRACE("关闭: 网络号:%u 成功！",ulnID);
		break;
	}
}


BOOL CNetServer::Startup(size_t nPlayerMaxCount, size_t nPrecision, size_t maxFreeBuffers_Cache, size_t bufferSize_Cache,
						  unsigned long ulnAddressToListenOn, unsigned short usnPortToListenOn)
{
	if (m_pServer)
		return FALSE;

	AUTOLOCKWRITE(m_lockAccess);

	assert(!m_ready);
	m_ready = FALSE;

	assert(m_mapId2Connect.empty());
	m_mapId2Connect.clear();

	IServerFactory* pSvrFac = NULL;
	if (FAILED(g_libHeaven.CreateInterface(IID_IServerFactory, reinterpret_cast< void ** >(&pSvrFac))))
		return FALSE;

	pSvrFac->SetEnvironment(nPlayerMaxCount,nPrecision,maxFreeBuffers_Cache,bufferSize_Cache,0);	//nPlayerMaxCount
	pSvrFac->CreateServerInterface(IID_IIOCPServer, reinterpret_cast< void ** >(&m_pServer));

	pSvrFac->Release();

	//init server

	if (FAILED(m_pServer->Startup()))//开启S3网络监听线程
		return FALSE;

	m_pServer->RegisterMsgFilter((LPVOID)this, ServerEventNotify);

	OnBuildup();

	m_pServer->OpenService(ulnAddressToListenOn, usnPortToListenOn);//开放连接监听端口

	m_ready = TRUE;

	return TRUE;
}
//关闭所有连接
BOOL CNetServer::Shutdown()
{
	AUTOLOCKWRITE(m_lockAccess);

	m_ready = FALSE;

	if (m_pServer)
	{
		m_pServer->CloseService();

		OnClearup();

		{{
		for (ID2CONNECTMAP::iterator it = m_mapId2Connect.begin(); it != m_mapId2Connect.end(); ++it)
		{
			m_pServer->ShutdownClient((*it).first);

			CNetConnect* pConn = (*it).second;
			assert(pConn);
			if (pConn)
			{
#ifndef _WORKMODE_SINGLETHREAD
				pConn->Stop();
				DestroyConnect(pConn);
#endif
			}
		}
		m_mapId2Connect.clear();
		}}

		m_pServer->Cleanup();
		m_pServer->Release();
		m_pServer = NULL;
	}

	return TRUE;
}
//关闭某个连接
BOOL CNetServer::Disconnect(unsigned long id)
{
	if (!m_pServer)
		return FALSE;

	//the event will be do clearing !
	return SUCCEEDED(m_pServer->ShutdownClient(id));
}

//通知有客户端连接了
void CNetServer::_NotifyClientConnectCreate(unsigned long ulnID)
{
	AUTOLOCKWRITE(m_lockAccess);

	assert(m_pServer);
	assert(m_mapId2Connect.find(ulnID) == m_mapId2Connect.end());

	BOOL oncreate = FALSE;
	ID2CONNECTMAP::iterator itID = m_mapId2Connect.end();

	CNetConnect* pConn = CreateConnect(this, ulnID);//创建这个编号的 连接 GS 
	if (pConn == NULL)
		goto on_fail;

#ifdef _WORKMODE_MULTITHREAD2
	m_pServer->RegisterMsgFilter(ulnID, pConn);
#endif

	OnClientConnectCreate(pConn);
	pConn->_NotifyClientConnectCreate();
	oncreate = TRUE;
	//将这个连接的 放进容器来存起来
	{{
	std::pair<ID2CONNECTMAP::iterator, bool> insret = m_mapId2Connect.insert(ID2CONNECTMAP::value_type(ulnID, pConn));

	itID = insret.first;
	if (!insret.second)	//has existed ? unexpected
	{
		rTRACE("warning: 意外的重新连接.....");

		CNetConnect* pConnOld = (*itID).second;
		if (pConnOld)
		{
			try
			{
#ifndef _WORKMODE_SINGLETHREAD
				pConnOld->Stop();
#endif
				pConnOld->_NotifyClientConnectClose();//删除就的连接?
				DestroyConnect(pConnOld);
			}
			catch (...)
			{
				assert(FALSE);
			}
		}

		(*itID).second = pConn;//重新赋值新的连接数据
	}
	}}

#ifndef _WORKMODE_SINGLETHREAD
	if (!pConn->Start())
		goto on_fail;
#endif

	return;	//succ

on_fail:
	rTRACE("error: 编号:(%u)连接失败.....",ulnID);

	try
	{
		if (oncreate)
		{
			OnClientConnectClose(pConn);
			pConn->_NotifyClientConnectClose();
		}

		m_pServer->ShutdownClient(ulnID);
		if (pConn != NULL)
			DestroyConnect(pConn);

		if (itID != m_mapId2Connect.end())
			m_mapId2Connect.erase(itID);
	}
	catch (...)
	{
		assert(FALSE);
	}
}

void CNetServer::_NotifyClientConnectClose(unsigned long ulnID)
{
	AUTOLOCKWRITE(m_lockAccess);

	ID2CONNECTMAP::iterator it = m_mapId2Connect.find(ulnID);
	if (it != m_mapId2Connect.end())
	{
		CNetConnect* pConn = (*it).second;
		assert(pConn);

		if (pConn)
		{
			pConn->Stop();
			pConn->_NotifyClientConnectClose();
			OnClientConnectClose(pConn);
		}

		m_mapId2Connect.erase(it);
		DestroyConnect(pConn);
	}
}
//客户端的连接数量 即使 GS的连接数量 用于多区服务
size_t CNetServer::GetConnectCount() 
{
	AUTOLOCKREAD(m_lockAccess);

	return m_mapId2Connect.size();
}
//是否已经连接到了GS
BOOL CNetServer::IsConnectReady(unsigned long id)
{//
	AUTOLOCKREAD(m_lockAccess);

	return m_mapId2Connect.find(id) != m_mapId2Connect.end();
}
//根据索引查找 网络连接（是哪个GS连接了S3）（0 1 2 3...）
CNetConnectDup CNetServer::FindNetConnect(unsigned long id)
{
	AUTOLOCKREAD(m_lockAccess);

	ID2CONNECTMAP::iterator it = m_mapId2Connect.find(id);
	if (it == m_mapId2Connect.end())
		return CNetConnectDup();

	CNetConnect* pNetConn = (*it).second;
	if (pNetConn == NULL)
		return CNetConnectDup();

	return CNetConnectDup(*pNetConn);
}

//向某个GS发包 
BOOL CNetServer::BroadPackageByNetIdx(DWORD ulnID,const void* pData, size_t size)
{
	if (!m_ready || m_pServer == NULL)
		return FALSE;

	AUTOLOCKREAD(m_lockAccess);

	ID2CONNECTMAP::iterator it = m_mapId2Connect.find(ulnID);
	if (it != m_mapId2Connect.end())
	{
		CNetConnect* pNetConnect = (*it).second;
		assert(pNetConnect);
		if (pNetConnect)
			pNetConnect->SendPackage(pData, size);
	}
	return TRUE;
}
//向所有连接上的GS发包 
BOOL CNetServer::BroadPackage(const void* pData, size_t size,int nSerRegIndex)
{
	if (!m_ready || m_pServer == NULL)
		return FALSE;

	AUTOLOCKREAD(m_lockAccess);

	for (ID2CONNECTMAP::iterator it = m_mapId2Connect.begin(); it != m_mapId2Connect.end(); ++it)
	{
		CNetConnect* pNetConnect = (*it).second;
		assert(pNetConnect);
		if (nSerRegIndex==-1)
		{
			if (pNetConnect)
				pNetConnect->SendPackage(pData, size);
		}
		else
		{//同一个区的gs就发送
			if (pNetConnect && pNetConnect->getSerRegIndex()==nSerRegIndex)
				pNetConnect->SendPackage(pData, size);
		}
		
	}

	return TRUE;
}

//开始接收连接上的GS 发来的数据包 
BOOL CNetServer::Route()
{
	AUTOLOCKREAD(m_lockAccess);

	if (m_pServer == NULL)
		return FALSE;

	for (ID2CONNECTMAP::iterator it = m_mapId2Connect.begin(); it != m_mapId2Connect.end(); ++it)
	{
		CNetConnect* pConnect = (*it).second;
		assert(pConnect);
		if (pConnect != NULL)
			pConnect->Route();//开始接收包 

	}

	return TRUE;
}

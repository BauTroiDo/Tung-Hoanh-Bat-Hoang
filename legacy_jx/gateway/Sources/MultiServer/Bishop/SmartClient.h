/********************************************************************
	created:	2003/08/22
	file base:	SmartClient
	file ext:	h
	author:		liupeng
	
	purpose:	
*********************************************************************/
#ifndef __INCLUDE_SMARTCLIENT__
#define __INCLUDE_SMARTCLIENT__

#pragma once

#include "IClient.h"
#include "RainbowInterface.h"

#include "Library.h"
#include "Buffer.h"
#include "tstring.h"
#include "CriticalSection.h"

#include <list>

class CSmartClient
{
public:

	CSmartClient();
	virtual ~CSmartClient();

	bool Open( const char * const pAddressToConnectServer, 
		unsigned short usPortToConnectServer, 
		HWND hwndContain,
		const char *pName,
		const char *pPwd,
		bool bReLink = true ,unsigned short nMaxPlayerCount=0 );

	void Close();

	bool Valid() { return ( NULL != m_pClient ); };

	bool Send( const void * const pData, const size_t &datalength );
	const void *Recv( size_t &datalength );	

	static DWORD WINAPI	WorkingThreadFunction( void *pV );

	HWND  m_hwndContain;
	void reConnectPayServer(bool ver=false);
protected:

	bool CreateConnectThread();
	IClient* _Connect();
	void _Verify(IClient* pClient, bool bDisconnect);
	static void __stdcall EventNotify( LPVOID lpParam, 
				const unsigned long &ulnEventType );

	void _EventNotify( const unsigned long &ulnEventType );

	enum enumServerLoginErrorCode
	{
		enumConnectFailed	= 0xA1,
		enumUsrNamePswdErr,
		enumIPPortErr,
		enumException
	};

private:

	typedef HRESULT ( __stdcall * pfnCreateClientInterface )( REFIID riid, void **ppv );

	static CLibrary	m_theRainbowLib;

	CCriticalSection	m_csHistroyList;
	typedef std::list< CBuffer * > stdList;
	stdList m_theHistroyList;

	HANDLE m_hWorkingThread;

	bool m_bReLink;		//使用本类需要自动重连功能
	
	/*
	 * Config
	 */
	IClient		*m_pClient;

	_tstring		m_sUserName;
	_tstring		m_sUserPwd;

	_tstring		m_sSvrIP;
	unsigned short						m_nSvrPort;
	CBuffer::Allocator	m_theAllocator;

	CCriticalSection	m_csStatus;
	enum enumClientStatus
	{
		enumClientNormal = 0x00,
		enumClientConnect,//正在连接中
		enumClientVerify, //已经连接上了
		enumClientError,  //连接错误
	};

	enumClientStatus m_ClientStatus;

	bool IsNormal();
	bool IsConnecting();
	bool IsVerifying();
	bool IsError();

	void SwitchToConnecting();		//切换状态到重连,如有必要会启动重连线程
	bool SetClientStatus(enumClientStatus nNewS);

	unsigned short m_lnMaxPlayerCount;   //最大玩家数
};

extern CSmartClient g_theSmartClient;
#endif // __INCLUDE_SMARTCLIENT__

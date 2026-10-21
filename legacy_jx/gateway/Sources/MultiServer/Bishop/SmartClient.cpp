#include "stdafx.h"
#include "SmartClient.h"

#include "Macro.h"

#include "KProtocolDef.h"
#include "AccountLoginDef.h"
#include "Buffer.h"

#include "msg_define.h"
#include "Macro.h"
#include "KWin32.h"
#include "KSG_MD5_String.h"
#include "inoutmac.h"

extern HWND hbishopOutput;
extern HWND hMianHwd;
extern HWND hAccCheckList;
extern HWND hIpCheckList;
extern HWND hYuJianCheckList;
extern HWND hClientIpList;
extern HWND hForBitOpenIpList;
extern HWND hHaveConnetIpList;
extern HWND hDugList;

void AddOutputString_S(HWND hListCtrl, char* aStr);//添加操作输出文字
/*
using CPackager;

using CCriticalSection;
using CBuffer;
*/
CLibrary	CSmartClient::m_theRainbowLib( "rainbow.dll" );

static const DWORD g_dwServerIdentify = 0xAEFC07B5;

CSmartClient g_theSmartClient;

CSmartClient::CSmartClient()
	: m_hWorkingThread( NULL )
	, m_pClient( NULL )
	, m_hwndContain( NULL )
	, m_nSvrPort( 0 )
	, m_theAllocator(1024*128,500)	//128    1024*64  500
	, m_lnMaxPlayerCount(0)
{
	AddOutputString_S(hbishopOutput,"<<<<<<<<<SmartClient启动成功>>>>>>>>>");
}

CSmartClient::~CSmartClient()
{
	if ( m_pClient )
	{
		m_pClient->Cleanup();
	}

	SAFE_RELEASE( m_pClient );
}

bool CSmartClient::Open( const char * const pAddressToConnectServer, 
		unsigned short usPortToConnectServer, 
		HWND hwndContain,
		const char *pName,
		const char *pPwd,
		bool bReLink /* true */,unsigned short nMaxPlayerCount)
{
	if (!pName || !pPwd)
	{
		return false;
	}

	m_sUserName = pName;
	m_sUserPwd = pPwd;

	m_sSvrIP = pAddressToConnectServer;
	m_nSvrPort = usPortToConnectServer;

	m_hwndContain = hwndContain;

	m_lnMaxPlayerCount = nMaxPlayerCount;

	/*
	 *	m_bReLink == true ( default )
	 */
	m_bReLink = bReLink;

	m_ClientStatus = enumClientConnect;//连接 paysystem 
	//::MessageBox( NULL, "开始连接客户端", "Warning", MB_OK | MB_ICONERROR );
	IClient* pClient = _Connect();////连接 paysystem 
	if (pClient)
	{
		//::MessageBox( NULL, "创建客户端服务成功", "Warning", MB_OK | MB_ICONERROR );
		_Verify(pClient, false);
		SAFE_RELEASE(pClient);
	}
    //创建连接线程
	//::MessageBox( NULL, "开始创建连接线程", "Warning", MB_OK | MB_ICONERROR );
	bool ok = CreateConnectThread();

	return ok;
}

void CSmartClient::Close()
{
	SetClientStatus(enumClientError);	//停止线程
}

void __stdcall CSmartClient::EventNotify( LPVOID lpParam, 
						const unsigned long &ulnEventType )
{
	CSmartClient *pThis = reinterpret_cast< CSmartClient * >( lpParam );

	ASSERT( pThis );

	try
	{
		pThis->_EventNotify( ulnEventType );
	}
	catch(...)
	{
		TRACE( "CSmartClient::EventNotify exception!" );
	}
}

void CSmartClient::_EventNotify( const unsigned long &ulnEventType )
{
	switch( ulnEventType )
	{
	case enumServerConnectCreate:
		::PostMessage( m_hwndContain, WM_SERVER_STATUS, ACCOUNTSERVER_NOTIFY, CONNECTED );
		break;

	case enumServerConnectClose:
		{
			::PostMessage( m_hwndContain, WM_SERVER_STATUS, ACCOUNTSERVER_NOTIFY, DICONNECTED );
			SwitchToConnecting();
		}
		break;
	}
}

bool CSmartClient::CreateConnectThread()
{
	/*
	 * Start a working thread
	 */
	DWORD dwThreadID = 0;

	m_hWorkingThread = ::CreateThread( NULL, 
		0, 
		WorkingThreadFunction, 
		( void * )this, 
		0, 
		&dwThreadID );
			
	if ( m_hWorkingThread == INVALID_HANDLE_VALUE )
	{
		return false;
	}

	return true;
}


void CSmartClient::reConnectPayServer(bool ver)
{
   SetClientStatus(enumClientConnect);

   if (ver)
	   return;

	if (m_pClient)
	{//清除链接
		m_pClient->Cleanup();
		m_pClient = NULL;
	}
	//SAFE_RELEASE( pClient ); //释放
	_Connect();   //重新链接 paysys
	AddOutputString_S(hbishopOutput,"与Sword3PaySys连接丢失,正在重连...");
}


//工作活动线程
DWORD WINAPI CSmartClient::WorkingThreadFunction( void *pV )
{
	CSmartClient *pThis = reinterpret_cast< CSmartClient * >( pV );

	ASSERT( pThis );
	IClient* pClient = NULL;

	try
	{
		while (!pThis->IsError())
		{
			if (pThis->IsConnecting())
			{//断开连接 开启了重连
				if (pThis->m_pClient)
				{//清除链接
					pThis->m_pClient->Cleanup();
					pThis->m_pClient = NULL;
				}
				SAFE_RELEASE( pClient ); //释放
				pClient = pThis->_Connect();   //重新链接 paysys
				AddOutputString_S(hbishopOutput,"与Sword3PaySys连接已经丢失,正在重连...");
			}
			else if (pThis->IsVerifying())
			{//正常连接的
				pThis->_Verify(pClient, true);
				SAFE_RELEASE( pClient );
			}
			Sleep(10);
		}
	}
	catch(...)
	{
		::MessageBox( NULL, "Run a working thread is failed!", "CSmartClient class", MB_OK );
	}
	::PostMessage(pThis->m_hwndContain, WM_CLOSE, NULL, NULL);
	return 0;
}

IClient* CSmartClient::_Connect()
{
	if (!IsConnecting())
		return NULL;

#ifdef	CONSOLE_DEBUG
					cprintf( "Begin connect\n");
#endif	

	IClient *pClient = NULL;
	/*
	 * There is connectted the heaven by the rainbow
	 */

	pfnCreateClientInterface pClientFactroyFun = (pfnCreateClientInterface )(m_theRainbowLib.GetProcAddress( _T("CreateInterface")));
	IClientFactory *pClientFactory = NULL;
	
	if ( pClientFactroyFun && SUCCEEDED(pClientFactroyFun(IID_IClientFactory, reinterpret_cast< void ** >(&pClientFactory))))
	{
		pClientFactory->SetEnvironment(m_lnMaxPlayerCount,10,1024*1024);	//1024*1024
		
		pClientFactory->CreateClientInterface(IID_IESClient, reinterpret_cast< void **>(&pClient));
		
		SAFE_RELEASE(pClientFactory);
	}

	
	if (pClient)
	{
		pClient->Startup();	 //启动一个线程

		pClient->RegisterMsgFilter(reinterpret_cast<void *>(this), EventNotify);
		HRESULT hr;
	    hr = pClient->FsGameServerConnectTo(m_sSvrIP.c_str(), m_nSvrPort);

		if (FAILED(hr))
		{//开始连接 paysystem 
			//::MessageBox( NULL, "连接paysys服务器失败", m_sSvrIP.c_str(), MB_OK | MB_ICONERROR );
			pClient->Cleanup();
			
			SAFE_RELEASE( pClient );
				
#ifdef	CONSOLE_DEBUG
			cprintf( "Reconnect Failed\n");
#endif	
			::Sleep(5000);
			
			return NULL;
		}
		else
		{//没有链接的 重新链接 重新发包给 paysys 验证 用户 和 密码  //设置已经连接的状态
			//return pClient;
			//::MessageBox( NULL, "连接客户端成功", "Warning", MB_OK | MB_ICONERROR );
			SetClientStatus(enumClientVerify);	
		}
	}
	
	return pClient;
}
//主要用bishop启动登录使用
void CSmartClient::_Verify(IClient* pClient, bool bDisconnect)
{
	if (!IsVerifying())
		return;

	if (pClient == NULL)
		return;

#ifdef	CONSOLE_DEBUG
					cprintf( "Begin verify\n");
#endif	
	
	/*
	* Send a login command
	*/
	const size_t datalength = sizeof( KServerAccountUserLoginInfo ) + 1;
	
	BYTE *pData = new BYTE[datalength];
	
	KServerAccountUserLoginInfo serlogin;
	serlogin.Size = sizeof( KServerAccountUserLoginInfo );
	
	serlogin.Type = ServerAccountUserLoginInfo;
	serlogin.Version = ACCOUNT_CURRENT_VERSION;
	serlogin.Operate = g_dwServerIdentify;
	
	size_t lenTag, lenScr;
	
	lenTag = sizeof( serlogin.Account );
	lenScr = m_sUserName.size() + 1;
	
	lenTag = ( lenTag > lenScr ) ? lenScr : lenTag;
	//数据库的 用户名
	memcpy( serlogin.Account, m_sUserName.c_str(), lenTag );
	serlogin.Account[lenTag - 1] = '\0';
	
	lenTag = sizeof( serlogin.Password );
	//数据库的 登录密码
	KSG_StringToMD5String(serlogin.Password, m_sUserPwd.c_str() /*szPassword*/);
	
	serlogin.Address = -1;
	
	// add by spe 2003/08/08
	gGetMacAndIPAddress(NULL, NULL, serlogin.MacAddress, &serlogin.Address);
	
	if ( bDisconnect )
	{
		*pData = c2s_gatewayverifyagain;
	}
	else
	{//登录时
		*pData = c2s_gatewayverify;
	}
	memcpy( pData + 1, &serlogin, sizeof( KServerAccountUserLoginInfo ) );
	//发包给 paysys
	//::MessageBox(NULL, "发包给paysystem成功!", "Warning", MB_OK | MB_ICONERROR );
	pClient->SendPackToServer( ( const void * )pData, datalength );
	
	SAFE_DELETE_ARRAY( pData );		
	/*
	* Wait for write back
	*/
	while ( pClient )
	{
		if ( !IsVerifying() )
		{
			break;	//因为正常的状态变化引起的退出
		}
#ifdef	CONSOLE_DEBUG
					cprintf( "Wait for verify\n");
#endif	

		size_t nLen = 0;
		//收到 paysys 发来的数据包 
		const void *pData = pClient->GetPackFromServer( nLen );
	
		if ( !pData || 0 == nLen )
		{//一直等待接收 paysystem包 
			::Sleep( 1 );
			continue;
		}
		
		BYTE cProtocol = CPackager::Peek( pData );
		
		switch ( cProtocol )
		{
		case s2c_gatewayverify:
			{
				KAccountUserReturn* pReturn = ( KAccountUserReturn * )( ( ( char * )pData ) + 1 );
				
				nLen--;
				if ( pReturn &&
					( nLen == sizeof( KAccountUserReturn ) ) &&
					( g_dwServerIdentify == pReturn->Operate ) )
				{
					if ( ACTION_SUCCESS == pReturn->nReturn )
					{
						::PostMessage( m_hwndContain, WM_SERVER_LOGIN_SUCCESSFUL, 0L, 0L );
						pClient->QueryInterface( IID_IESClient, reinterpret_cast< void ** >( &m_pClient ) );

						if (SetClientStatus(enumClientNormal))
						{
							Send(NULL, 0);
#ifdef	CONSOLE_DEBUG
					cprintf( "Succeed verify\n");
#endif	

						}
					}
					else if ( E_ACCOUNT_OR_PASSWORD == pReturn->nReturn )
					{
						::PostMessage( m_hwndContain, WM_SERVER_LOGIN_FAILED, enumUsrNamePswdErr, 0L );

						SetClientStatus(enumClientError);
					}
					else if ( E_ADDRESS_OR_PORT == pReturn->nReturn )
					{
						::PostMessage( m_hwndContain, WM_SERVER_LOGIN_FAILED, enumIPPortErr, 0L );

						SetClientStatus(enumClientError);
					}
					else
					{//不知原因 启动失败
						::PostMessage( m_hwndContain, WM_SERVER_LOGIN_FAILED, enumException, 0L );

						SetClientStatus(enumClientError);
					}
					
				}
			}
			break;
			
		default:
			
			ASSERT( FALSE );

			SetClientStatus(enumClientError);
			
			break;
		}
	}	
}

bool CSmartClient::Send( const void * const pData, const size_t &datalength )
{
	if (IsNormal())
	{
		if (m_pClient)
		{
			/*
			 *	Send the histroy information to server
			 */
			{
				CCriticalSection::Owner locker( m_csHistroyList );

				stdList::iterator it;
				for ( it = m_theHistroyList.begin();it != m_theHistroyList.end();++it)
					{
						CBuffer *pBuffer = *it;

						if ( pBuffer )
						{
							m_pClient->SendPackToServer( 
								(const void *)(pBuffer->GetBuffer()), 
								pBuffer->GetUsed() );

							SAFE_RELEASE( pBuffer );
						}
					}

				m_theHistroyList.clear();
			}

			if (pData && datalength)
				m_pClient->SendPackToServer( pData, datalength );

			return true;
		}
	}
	else if (pData && datalength > 0)
	{
		/* Store data into buffer */
		CCriticalSection::Owner locker( m_csHistroyList );

		CBuffer *pBuffer = m_theAllocator.Allocate();

		ASSERT( pBuffer );

		pBuffer->AddData((BYTE*) pData, datalength );

		m_theHistroyList.push_back( pBuffer );
	}

	return false;
}

const void * CSmartClient::Recv( size_t &datalength )
{
	if (IsNormal())
	{
		if ( m_pClient )
		{
			return m_pClient->GetPackFromServer( datalength );
		}
	}

	datalength = 0;
	return NULL;
}

bool CSmartClient::IsNormal()
{
	CCriticalSection::Owner locker(m_csStatus);
	return (m_ClientStatus == enumClientNormal);
}

bool CSmartClient::IsConnecting()
{
	CCriticalSection::Owner locker(m_csStatus);
	return (m_ClientStatus == enumClientConnect);
}

bool CSmartClient::IsVerifying()
{
	CCriticalSection::Owner locker(m_csStatus);
	return (m_ClientStatus == enumClientVerify);
}

bool CSmartClient::IsError()
{
	CCriticalSection::Owner locker(m_csStatus);
	return (m_ClientStatus == enumClientError);
}

void CSmartClient::SwitchToConnecting()		//切换状态到重连,如有必要会停止重连线程
{
	if (m_bReLink)
		SetClientStatus(enumClientConnect);
	else
		SetClientStatus(enumClientError);	;	//停止线程
}

bool CSmartClient::SetClientStatus(enumClientStatus nNewS)
{
	CCriticalSection::Owner locker(m_csStatus);

	if (nNewS == enumClientError)
	{
		m_ClientStatus = nNewS;
		return true;
	}

	if (m_ClientStatus == enumClientNormal && nNewS == enumClientConnect)
	{
		m_ClientStatus = nNewS;
		return true;
	}

	if (m_ClientStatus == enumClientConnect &&
		(nNewS == enumClientConnect || nNewS == enumClientVerify)
		)
	{
		m_ClientStatus = nNewS;
		return true;
	}

	if (m_ClientStatus == enumClientVerify &&
		(nNewS == enumClientConnect || nNewS == enumClientNormal)
		)
	{
		m_ClientStatus = nNewS;
		return true;
	}

	return false;
}


void AddOutputString_S(HWND hListCtrl, char* aStr)
{//添加操作输出文字
	if ( hListCtrl && ::IsWindow( hListCtrl ) )
	{
		int nCount = ::SendMessage( hListCtrl, LB_GETCOUNT, 0, 0 ); //获取信息的数量
		if(nCount >= 100)
		{
			::SendMessage( hListCtrl, LB_DELETESTRING, 100, 0 );  //删除信息
		}
		int nIndex = ::SendMessage( hListCtrl, LB_INSERTSTRING, 0, ( LPARAM )aStr );//把新的信息添到第一个
	}
}
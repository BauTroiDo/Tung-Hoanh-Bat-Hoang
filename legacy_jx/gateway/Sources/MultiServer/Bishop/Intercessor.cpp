#include "stdafx.h"
#include "Intercessor.h"

#include "AccountLoginDef.h"
#include "LoginDef.h"

#include "KProtocolDef.h"
#include "KProtocol.h"

#include "Macro.h"
#include "Buffer.h"
#include "BishopPacketGuard.h"
#include "BishopGsPeerGuard.h"

#include "SmartClient.h"

//----------------------
extern CLogFile  bs_LogFile;
extern HWND hbishopOutput;
extern HWND hMianHwd;
extern HWND hAccCheckList;
extern HWND hIpCheckList;
extern HWND hYuJianCheckList;
extern HWND hClientIpList;
extern HWND hForBitOpenIpList;
extern HWND hHaveConnetIpList;
extern HWND hDugList;
/*
using CEvent;
using CCriticalSection;
using CPackager;
*/
CLibrary	CIntercessor::m_theHeavenLib("heaven.dll");
CLibrary	CIntercessor::m_theRainbowLib("rainbow.dll");

CIntercessor::CIntercessor(unsigned long lnMaxPlayerCount, CNetwork &theNetworkConfig)
		: m_hwndViewer( NULL )
//		, m_pAccSvrClient( NULL )
		, m_pDBRoleClient( NULL )
		, m_pGameSvrServer( NULL )
		, m_pPlayerServer( NULL )
		, m_hWorkingThread( INVALID_HANDLE_VALUE )
		, m_hHelperThread( INVALID_HANDLE_VALUE )
		, m_hQuitEvent( NULL, true, false, NULL /*"PM_Quit_Event"*/ )
		, m_hStartupWoringThreadEvent( NULL, false, false, NULL /*"PM_WORKING_EVENT"*/ )
		, m_hStartupHelperThreadEvent( NULL, false, false, NULL /*"PM_HELPER_EVENT"*/ )
		, m_theNetworkConfig(theNetworkConfig)
		, m_lnMaxPlayerCount(lnMaxPlayerCount)
{
		bDBRoleServerSucceed = false;
}

CIntercessor::~CIntercessor()
{

}
//创建 GAME客户端 接包工作线程
bool CIntercessor::Create( HWND hwndViewer /* NULL */ )
{
	m_hwndViewer = hwndViewer;
	
	/*
	 * Start a working thread
	 */
	DWORD dwThreadID = 0;

	m_hWorkingThread = ::CreateThread(NULL, 
		0, 
		WorkingThreadFunction, 
		(void *)this, 
		0, 
		&dwThreadID);
			
	if (m_hWorkingThread == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	/*
	 * Startup a helper thread
	 */
	dwThreadID = 0;

	m_hHelperThread = ::CreateThread( NULL, 
		0, 
		HelperThreadFunction, 
		(void *)this,
		0, 
		&dwThreadID);
			
	if (m_hHelperThread == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	/*std::thread t(&CIntercessor::Helper, this);
	t.detach();*/
//	m_pAccSvrClient = m_theNetworkConfig.CreateAccSvrClient( m_theNetworkConfig.GetAccSvrIP(), m_theNetworkConfig.GetAccSvrPort() );

//	ASSERT( m_pAccSvrClient );

//	m_pAccSvrClient->RegisterMsgFilter( ( void * )this, AccountEventNotify );

	return StartupNetwork(); //开始网络链接 工作
}

void CIntercessor::Destroy()
{
	m_hQuitEvent.Set();

	m_hStartupWoringThreadEvent.Set();
	m_hStartupHelperThreadEvent.Set();

	SAFE_CLOSEHANDLE( m_hWorkingThread );
	SAFE_CLOSEHANDLE( m_hHelperThread );

//	SAFE_RELEASE( m_pAccSvrClient );

	CleanNetwork();
}
/*
void __stdcall CIntercessor::AccountEventNotify( LPVOID lpParam, 
						const unsigned long &ulnEventType )
{
	CIntercessor *pPlayerManager = reinterpret_cast< CIntercessor * >( lpParam );

	ASSERT( pPlayerManager );

	try
	{
		pPlayerManager->_AccountEventNotify( ulnEventType );
	}
	catch(...)
	{
		TRACE( "CIntercessor::AccountEventNotify exception!" );
	}
}

void CIntercessor::_AccountEventNotify( const unsigned long &ulnEventType )
{
	switch( ulnEventType )
	{
	case enumServerConnectCreate:

		::PostMessage( m_hwndViewer, WM_SERVER_STATUS, ACCOUNTSERVER_NOTIFY, CONNECTED );

		break;

	case enumServerConnectClose:

		::PostMessage( m_hwndViewer, WM_SERVER_STATUS, ACCOUNTSERVER_NOTIFY, DICONNECTED );

		break;
	}
}	
*/
void __stdcall CIntercessor::DBRoleEventNotify( LPVOID lpParam, 
						const unsigned long &ulnEventType )
{
	CIntercessor *pPlayerManager = reinterpret_cast< CIntercessor * >( lpParam );

	ASSERT( pPlayerManager );

	try
	{
		pPlayerManager->_DBRoleEventNotify( ulnEventType );
	}
	catch(...)
	{
		TRACE( "CIntercessor::DBRoleEventNotify exception!" );
	}
}

void CIntercessor::_DBRoleEventNotify( const unsigned long &ulnEventType )
{
	switch( ulnEventType )
	{
	case enumServerConnectCreate:

		::PostMessage( m_hwndViewer, WM_SERVER_STATUS, DBROLESERVER_NOTIFY, CONNECTED );

		break;

	case enumServerConnectClose:

		::PostMessage( m_hwndViewer, WM_SERVER_STATUS, DBROLESERVER_NOTIFY, DICONNECTED );

		break;
	}
}

void __stdcall CIntercessor::PlayerEventNotify( LPVOID lpParam,
			const unsigned long &ulnID,
			const unsigned long &ulnEventType )
{
	CIntercessor *pPlayerManager = reinterpret_cast< CIntercessor * >( lpParam );

	ASSERT( pPlayerManager );

	try
	{
		pPlayerManager->_PlayerEventNotify( ulnID, ulnEventType );
	}
	catch(...)
	{
		TRACE( "CIntercessor::PlayerEventNotify exception!" );
	}
}

void CIntercessor::_PlayerEventNotify( const unsigned long &ulnID,
			const unsigned long &ulnEventType )
{
	//CCriticalSection::Owner locker( m_csPlayerAction );

	switch ( ulnEventType )
	{
	case enumClientConnectCreate:
		if (m_thePlayers.end() != m_thePlayers.find(ulnID)) {//通知玩家已经连接
			IPlayer *pPlayer = m_thePlayers[ulnID];
			ASSERT(pPlayer);
			pPlayer->Active();
		}
		break;

	case enumClientConnectClose:
		{
			PLAYER_MAP::iterator it;
			
			if (m_thePlayers.end() != (it = m_thePlayers.find(ulnID)))
			{
				IPlayer *pPlayer = m_thePlayers[ulnID];
				
				ASSERT(pPlayer);

				pPlayer->Inactive();
			}
		}
		break;
	}
}

bool CIntercessor::StartupNetwork()
{
	// A missing or malformed allowlist MUST fail before any listen socket opens.
	if (!BishopGsPeerGuard::ValidateAllowlist(m_theNetworkConfig.GetTrustedGameServerIPs()))
		return false;
	bool bPlayerServerSucceed = false;
	bDBRoleServerSucceed = false;
	bool bGameSvrServerSucceed = false;
	

	//m_lnMaxPlayerCount=m_theNetworkConfig.GetMaxPlayerCount();
	/*
	 * There is connectted the heaven by the rainbow
	 */

	/*
	 * For the db-role server
	 */
	pfnCreateClientInterface pClientFactroyFun = (pfnCreateClientInterface)(m_theRainbowLib.GetProcAddress( _T( "CreateInterface" ) ) );
	
	IClientFactory *pClientFactory = NULL;

	if (pClientFactroyFun && SUCCEEDED(pClientFactroyFun(IID_IClientFactory, reinterpret_cast<void **>(&pClientFactory))))
	{
		pClientFactory->SetEnvironment(m_lnMaxPlayerCount,10,1024*1024);	//(1024*1024);	  
		
		pClientFactory->CreateClientInterface(IID_IESClient, reinterpret_cast< void ** >(&m_pDBRoleClient));
		
		SAFE_RELEASE(pClientFactory);
	}

	if (m_pDBRoleClient)
	{//数据库客户端
		m_pDBRoleClient->Startup();

		m_pDBRoleClient->RegisterMsgFilter(reinterpret_cast<void *>(this),DBRoleEventNotify);

		if (SUCCEEDED(m_pDBRoleClient->FsGameServerConnectTo(m_theNetworkConfig.GetRoleSvrIP(),m_theNetworkConfig.GetRoleSvrPort())))
		{//开始连接 goddes 
			bDBRoleServerSucceed = true;
		}
	}

	/*
	 * We open the heaven to wait for the rainbow  开放一个服务 等待 客户端GS连接
	 */
	pfnCreateServerInterface pServerFactroyFun = 
		(pfnCreateServerInterface)(m_theHeavenLib.GetProcAddress(_T("CreateInterface")));

	IServerFactory *pServerFactory = NULL;

	if (pServerFactroyFun && 
		SUCCEEDED( pServerFactroyFun(IID_IServerFactory, reinterpret_cast< void ** >(&pServerFactory))))
	{//m_lnMaxPlayerCount
		pServerFactory->SetEnvironment(m_lnMaxPlayerCount,10,1000,1024*16,0);	   //1024*8 1024*128

		/*
		 * For player
		 */
		pServerFactory->CreateServerInterface(IID_IIOCPServer, reinterpret_cast< void ** >(&m_pPlayerServer));

		/*
		 * For gameserver
		 */
		pServerFactory->CreateServerInterface(IID_IIOCPServer, reinterpret_cast< void ** >( &m_pGameSvrServer ) );
		
		pServerFactory->Release();		
	}

	if (m_pGameSvrServer)
	{//gs连接的服务器端
		m_pGameSvrServer->Startup();//开始ｒｕｎ　线程
/*
		IServer *pCloneGameServer = NULL;
		m_pGameSvrServer->QueryInterface( IID_IIOCPServer, reinterpret_cast< void ** >( &pCloneGameServer ) );
		
		IClient *pCloneAcc = NULL;
		m_pAccSvrClient->QueryInterface( IID_IESClient, reinterpret_cast< void ** >( &pCloneAcc ) );
*/
		static CGameServer::NI ni;
		ni.pServer = m_pGameSvrServer;
		ni.pClient = NULL; //m_pAccSvrClient;
		ni.hwndContainer = m_hwndViewer;
		ni.trustedGameServerIPs = m_theNetworkConfig.GetTrustedGameServerIPs();
		
		m_pGameSvrServer->RegisterMsgFilter(reinterpret_cast< void * >(&ni), CGameServer::GameSvrEventNotify);
		///DWORD dwIP = inet_addr("192.168.100.7");
		if (SUCCEEDED(m_pGameSvrServer->OpenService(INADDR_ANY, m_theNetworkConfig.GetGameSvrOpenPort())))
		{//打开账号登陆端口	    56732       //GS 连接的端口
			bGameSvrServerSucceed = true;
		}
	}
	// 开放一个服务 等待 客户端game连接
	if (m_pPlayerServer)
	{//game连接的服务器端
		m_pPlayerServer->Startup();
		//注册玩家连接game连接状态
		m_pPlayerServer->RegisterMsgFilter( reinterpret_cast< void * >( this ), PlayerEventNotify);

		if (SUCCEEDED( m_pPlayerServer->OpenService(INADDR_ANY, m_theNetworkConfig.GetClientOpenPort())))
		{//打开客户端登陆游戏的端口	   56722 //玩家连接的端口
			bPlayerServerSucceed = true;			
		}
	}

	if (bPlayerServerSucceed && bDBRoleServerSucceed && bGameSvrServerSucceed)
	{//客户端服务 和 数据可 服务 和 GS 服务都创建的情况下
//		IClient *pCloneAcc = NULL;
//		m_pAccSvrClient->QueryInterface( IID_IESClient, reinterpret_cast< void ** >( &pCloneAcc ) );
		
		IServer *pClonePlayer = NULL;
		m_pPlayerServer->QueryInterface(IID_IIOCPServer, reinterpret_cast< void ** >(&pClonePlayer));
		
		IClient *pCloneDBRole = NULL;
		m_pDBRoleClient->QueryInterface(IID_IESClient, reinterpret_cast< void ** >(&pCloneDBRole));
		
		ASSERT( /* pCloneAcc && */ pClonePlayer && pCloneDBRole );
			
		CGamePlayer::ATTACH_NETWORK( NULL/* pCloneAcc */, pClonePlayer, pCloneDBRole );

		/*
		 * Create new player
		 */
		{
			CCriticalSection::Owner lock(m_csPlayerAction);
			
			/*
			 * Initialize all of player
			 */
			for ( UINT index = 0; index < m_lnMaxPlayerCount; ++index)
			{//初始化人物数据数组 初始化 可登陆游戏的 人物数组
				/*
				* Create a player node and add it into list
				*/
				IPlayer *pPlayer = new CGamePlayer(index);
				
				ASSERT(pPlayer);
				
				m_thePlayers[index] = pPlayer;
			}
		}

		IServer *pCloneGameServer = NULL;
		if (m_pGameSvrServer) m_pGameSvrServer->QueryInterface( IID_IIOCPServer, reinterpret_cast<void **>(&pCloneGameServer));

		CGameServer::Begin( pCloneGameServer );

		m_hStartupWoringThreadEvent.Set();
		m_hStartupHelperThreadEvent.Set();

	}
	
	return ( bPlayerServerSucceed && bDBRoleServerSucceed && bGameSvrServerSucceed );
}

//重连GODDESS
bool CIntercessor::RetConnnetGoddess()
{
  if (m_pDBRoleClient)
 { 
	 m_pDBRoleClient->Shutdown();
	if (SUCCEEDED(m_pDBRoleClient->FsGameServerConnectTo(m_theNetworkConfig.GetRoleSvrIP(),m_theNetworkConfig.GetRoleSvrPort())))
	{//开始连接 goddes 
		//bDBRoleServerSucceed = true;
		return true;
	}
	else
	{//已经连接
		return false;
	}
 }
  return false;
}
void CIntercessor::CleanNetwork()
{
	/*
	 * Clear player info
	 */
	{
		CCriticalSection::Owner locker(m_csPlayerAction);
		
		PLAYER_MAP::iterator it;

		for (it = m_thePlayers.begin(); it != m_thePlayers.end(); ++it)
		{
			IPlayer *pPlayer = (IPlayer *)((*it).second);
			
			ASSERT(pPlayer);
			
			SAFE_DELETE(pPlayer);
		}
		
		m_thePlayers.erase(m_thePlayers.begin(),m_thePlayers.end());
	}

	CGameServer::End();
	CGamePlayer::DETACH_NETWORK();

	/*
	 * Disconnect network and relase this resource
	 */
	if (m_pPlayerServer)
	{
		m_pPlayerServer->CloseService();

		m_pPlayerServer->Cleanup();
		
		SAFE_RELEASE(m_pPlayerServer);
	}

	if (m_pDBRoleClient)
	{
		m_pDBRoleClient->Cleanup();

		SAFE_RELEASE(m_pDBRoleClient);
	}

	if (m_pGameSvrServer)
	{
		m_pGameSvrServer->CloseService();

		m_pGameSvrServer->Cleanup();

		SAFE_RELEASE(m_pGameSvrServer);
	}
}
//接受客户端GMAE 数据包线程
DWORD WINAPI CIntercessor::WorkingThreadFunction(void *pV)
{
	CIntercessor *pThis = reinterpret_cast< CIntercessor * >(pV);

	ASSERT( pThis );

	try
	{
		pThis->Working();
	}
	catch(...)
	{
		::MessageBox( NULL, "Run a working thread is failed!", "CIntercessor class", MB_OK );
	}

	return 0;
}
//接受 game 发送过来的数据包 
int	CIntercessor::Working()
{//bishop 网络接包 工作线程 
//	::CoInitialize( NULL );
	
	m_hStartupWoringThreadEvent.Wait();
	
	while (!m_hQuitEvent.Wait(0))
	{
		/*
		 * For some alive player
		 */
		{
			CCriticalSection::Owner locker(m_csPlayerAction);
			
			for (size_t index = 0; index < m_lnMaxPlayerCount; ++index)
			{		
				IPlayer *pPlayer = m_thePlayers[index];
				
				//ASSERT(pPlayer);	

				if (pPlayer && pPlayer->IsActive())
				{
					/*
					 * Get data from player
					 */
					size_t datalength = 0;
					//接受 从客户端发来的登陆信息包
					const void *pData = m_pPlayerServer->GetPackFromClient(index, datalength);
					
					if (pData && datalength)
					{//回应 客户端发来的 数据包
						AnalyzePlayerRequire(index, pData, datalength);
					}
					
					/*
					* Execute work
					*/
					pPlayer->Run();
				}
			}
		}

		::Sleep( 1 );
	}

//	::CoUninitialize();

	return 0;
}
//工作线程 接受 PaySystem 和 Goddess 发过来的数据包
DWORD WINAPI CIntercessor::HelperThreadFunction(void *pV)
{
	CIntercessor *pThis = reinterpret_cast< CIntercessor * >(pV);

	ASSERT( pThis );

	try
	{
		pThis->Helper();
	}
	catch(...)
	{
		::MessageBox( NULL, "Startup a helper thread is failed!", "CIntercessor class", MB_OK );
	}

	return 0;
}
//工作线程 接受 PaySystem 和 Goddess 发过来的数据包
int	CIntercessor::Helper()
{
	m_hStartupHelperThreadEvent.Wait();
	
	static const DWORD dwTimeLimit = 1000 * 10;
	DWORD dwStart = ::GetTickCount();
	DWORD dwEnd = dwStart;

	while (1/*!m_hQuitEvent.Wait(0)*/)
	{
		/*
		 * ping account server  PaySystem
		 */
		if ((((dwEnd = ::GetTickCount()) - dwStart) > dwTimeLimit) || (dwStart > dwEnd))
		{
			dwStart = dwEnd;

			PING_COMMAND pc;
			pc.ProtocolType = c2s_ping;
			pc.m_dwTime		= dwStart;

			g_theSmartClient.Send(&pc, sizeof(PING_COMMAND));
//			m_pAccSvrClient->SendPackToServer( &pc, sizeof( PING_COMMAND ) );
		}

		/*
		 * Get data from account-server   PaySystem
		 */
		size_t dataLength = 0;
		const void *pData = g_theSmartClient.Recv( dataLength );
//		const void *pData = m_pAccSvrClient->GetPackFromServer( dataLength );

		while(pData && dataLength)
		{
			ASSERT(CPackager::Peek(pData) > g_nGlobalProtocolType);

			if (s2c_accountfdfsglogin == *(const char *)pData &&
                BishopPacketGuard::HasBytes(pData, dataLength, 1 + sizeof(KAccountHead)))
			{//如果等于帐号登陆的
				const KAccountHead *pAUR = (KAccountHead * )(((const char *)pData) + 1);
				
				UINT uID = pAUR->Operate;
				
				if ( uID <m_lnMaxPlayerCount && m_thePlayers.end() != m_thePlayers.find( uID ))
				{
					IPlayer *pPlayer = m_thePlayers[uID];
					
					ASSERT(pPlayer);
					//接包
					pPlayer->AppendData(CGamePlayer::enumOwnerAccSvr, pData, dataLength);
				}

				/*if (hDugList)
				{
					char nInfo[64]={0};
					sprintf(nInfo,"网络ID:(%u)帐号开始登陆(%u)。。",uID,m_lnMaxPlayerCount);
					AddOutputIpList(hDugList,nInfo);
				}*/
			}
			
			/*
			 * ping account server  paySystem 
			 */
			if ((((dwEnd = ::GetTickCount()) - dwStart) > dwTimeLimit) || (dwStart > dwEnd))
			{
				dwStart = dwEnd;
				
				PING_COMMAND pc;
				pc.ProtocolType = c2s_ping;
				pc.m_dwTime		= dwStart;
				
				g_theSmartClient.Send(&pc, sizeof(PING_COMMAND));
//				m_pAccSvrClient->SendPackToServer( &pc, sizeof( PING_COMMAND ) );
			}

			pData = g_theSmartClient.Recv( dataLength );
//			pData = m_pAccSvrClient->GetPackFromServer( dataLength );
		}
		
		/*
		 * Get data from role-database从 goddess 获取数据包
		 */
		dataLength = 0;
		if (bDBRoleServerSucceed && m_pDBRoleClient)
		   pData = m_pDBRoleClient->GetPackFromServer(dataLength);

		while(pData && dataLength)
		{
			BYTE cProtocol = CPackager::Peek(pData);

			UINT uID = (UINT)(-1);
            bool validIdentity = false;
            if (cProtocol < g_nGlobalProtocolType)
            {
                std::uint32_t id = 0;
                validIdentity = BishopPacketGuard::ReadU32(pData, dataLength, 2, id);
                if (validIdentity) uID = static_cast<UINT>(id);
            }
            else if (cProtocol > g_nGlobalProtocolType &&
                     BishopPacketGuard::HasBytes(pData, dataLength, offsetof(TProcessData, pDataBuffer)))
            {
                const TProcessData* pPD = static_cast<const TProcessData*>(pData);
                uID = pPD->ulIdentity;
                validIdentity = true;
            }

			if (validIdentity && uID < m_lnMaxPlayerCount && m_thePlayers.end() != m_thePlayers.find( uID ))
			{
				IPlayer *pPlayer = m_thePlayers[uID];

				ASSERT(pPlayer);
				
				if (pPlayer) pPlayer->AppendData(CGamePlayer::enumOwnerRoleSvr, pData, dataLength);
			}
			
			/*
			 * ping account server
			 */
			if ((((dwEnd = ::GetTickCount()) - dwStart) > dwTimeLimit) || (dwStart> dwEnd))
			{
				dwStart = dwEnd;
				
				PING_COMMAND pc;
				pc.ProtocolType = c2s_ping;
				pc.m_dwTime		= dwStart;

				g_theSmartClient.Send(&pc, sizeof(PING_COMMAND));
//				m_pAccSvrClient->SendPackToServer( &pc, sizeof( PING_COMMAND ) );
			}

			pData = m_pDBRoleClient->GetPackFromServer(dataLength);
		}
		
		::Sleep( 1 );
	}

	return 0;
}

bool CIntercessor::AnalyzePlayerRequire( size_t index, const void *pData, size_t datalength )
{
    if (!BishopPacketGuard::HasBytes(pData, datalength, 1) ||
        index >= m_lnMaxPlayerCount ||
        m_thePlayers.find(index) == m_thePlayers.end()) return false;
	ASSERT( pData && datalength );

	BYTE cProtocol = *( const BYTE * )pData;

	switch ( cProtocol )
	{
	case c2s_login_fhgyrinhkmvnsheypo:
		{
			if (!BishopPacketGuard::HasBytes(pData, datalength, 1 + sizeof(KLoginAccountInfo)))
                return false;
            const BYTE *pBuffer = (const BYTE *)pData + 1;

			KLoginAccountInfo *pLAI = (KLoginAccountInfo *)pBuffer;

			ASSERT( pLAI );

			IPlayer *pPlayer = m_thePlayers[index];

			if (pLAI && pPlayer)
			{				
				if (pPlayer->AppendData(CGamePlayer::enumOwnerPlayer, pData, datalength))
					pPlayer->DispatchTask(CGamePlayer::enumLogin);
			}
		}

		break;
		
	case c2s_dbplayerselect:
	case c2s_newplayer:
	case c2s_roleserver_deleteplayer: //删除角色
		{
			if (index < m_lnMaxPlayerCount && m_thePlayers.end() != m_thePlayers.find( index ))
			{
				IPlayer *pPlayer = m_thePlayers[index];
				
				ASSERT(pPlayer);
				
				pPlayer->AppendData( CGamePlayer::enumOwnerPlayer, pData, datalength);
			}
		}		
		break;

	default:
		break;
	}
	
	return true;
}

size_t CIntercessor::GetGameServerCount()
{
	if (m_pGameSvrServer)
	{
		return m_pGameSvrServer->GetClientCount();
	}

	return 0;
}

void CIntercessor::GetGameServerInfo( const unsigned long &ulnID ,char *nPinfo, size_t nInfoSize)
{
    //char nIpInfo[128]={0};
	if ( m_pGameSvrServer )
	{
		m_pGameSvrServer->GetClientInfo( ulnID,nPinfo ,nInfoSize);
		//return nIpInfo;
	}

	//return NULL;
}


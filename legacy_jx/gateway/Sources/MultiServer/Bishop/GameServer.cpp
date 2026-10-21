#include "Stdafx.h"
#include "GameServer.h"
#include "IPlayer.h"
#include "GamePlayer.h"
#include "Inifile.h"
#include "msg_define.h"
#include "resource.h"
#include <process.h>

#include "AccountLoginDef.h"
//#include "../../engine/Src/KIniFile.h"
#include "Macro.h"
#include "SmartClient.h"
#include "BishopPacketGuard.h"
#include "BishopGsPeerGuard.h"

extern CLogFile  bs_LogFile;
extern HWND hbishopOutput;
extern HWND hMianHwd;
extern HWND hAccCheckList;
extern HWND hIpCheckList;
extern HWND hClientIpList;
extern HWND hForBitOpenIpList;
extern HWND hHaveConnetIpList;
extern HWND hDugList;
extern HWND hYuJianCheckList;
/*
using CIniFile;
using GetAppFullPath;


using CCriticalSection;
using CEvent;
using CBuffer;
using CPackager;
using ToString;
using _tstring;
*/
CBuffer::Allocator						CGameServer::m_theGlobalAllocator(1024*128,500);	//(1024*64,500);

CCriticalSection						CGameServer::m_csMapIDAction;
CGameServer::stdClientSerVer			CGameServer::m_ClientSerVer;
CGameServer::stdMapIDConvert			CGameServer::m_theMapIDConvert;

CEvent									CGameServer::m_shQuitEvent( NULL, true, false, NULL/*"GS_QUIT_EVENT"*/ );
CEvent									CGameServer::m_shStartupManagerThreadEvent( NULL, false, false, NULL/*"GS_MANAGER_EVENT"*/ );

HANDLE									CGameServer::m_shManagerThread = NULL;

CGameServer::stdGameSvr					CGameServer::m_theGameServers;
CCriticalSection						CGameServer::m_csGameSvrAction;

/*
 * CGamePlayer Global Function
 */
bool CGameServer::SetupGlobalAllocator( size_t bufferSize, size_t maxFreeBuffers )
{
	return CGameServer::m_theGlobalAllocator.ReSet( bufferSize, maxFreeBuffers );
}

LONG CGameServer::m_slnIdentityCounts = 0L;

/*
 * A generator of guid
 */
static void GenGuid(GUID *guid)
{//对应的网络识别号码
	ASSERT(guid);

	static char szData[9] = {0};
	static DWORD dwBase = 0;

	if (FAILED(::CoCreateGuid(guid)))
	{//如果创建失败
		if ((dwBase >> 31) & 0x1)
		{
			dwBase = 0;
		}

		/*
		 * Make a 128 bits cipher
		 */
		guid->Data1 = dwBase++;
		guid->Data2 = (DWORD)rand() & 0xFFFF;
		guid->Data3 = ((DWORD)rand() >> 16) & 0xFFFF;

		sprintf((char *)szData, "%u%u",((DWORD)rand() + dwBase++),(DWORD)rand()); //%d%d
		memcpy(guid->Data4, szData, 8);		
	}
}

/*
 * class CGameServer
 */
CGameServer::CGameServer( IServer *pGameSvrServer,
						 IClient *pAccountClient,
						 UINT nIdentityID /* = -1 */ )
				: m_lnIdentityID( nIdentityID )
				, m_pGameSvrServer(pGameSvrServer)
//				, m_pAccountClient( pAccountClient )
				, m_nServerIP_Internet(0)
				, m_nServerIP_Intraner(0)
				, m_nServerPort(0)
				, m_dwCapability(-1)
				, m_nTwoRevivePos(53)
				, m_serGsNumber(0)
				, m_serVerNoIndex(0)

{
	LONG lnID = ::InterlockedExchangeAdd(&m_slnIdentityCounts,1);

	m_lnIdentityID = ((UINT)(-1) == m_lnIdentityID) ? lnID : m_lnIdentityID;
}

CGameServer::~CGameServer()
{
//	SAFE_RELEASE( m_pGameSvrServer );
//	SAFE_RELEASE( m_pAccountClient );

	m_thePackager.Empty();

	::InterlockedExchangeAdd( &m_slnIdentityCounts, -1 );
}

bool CGameServer::Create()
{
	ZeroMemory(m_theProcessProtocolFun, sizeof(m_theProcessProtocolFun));

	m_theProcessProtocolFun[c2s_registeraccount]  = &CGameServer::_RegisterAccount;
	m_theProcessProtocolFun[c2s__entergamegkjyiruqine] = &CGameServer::_NotifyEnterGame;        //Bishop 接受GS的通知开始进入游戏 转发给 paysys
	m_theProcessProtocolFun[c2s_leavegame] = &CGameServer::_NotifyLeaveGame;
	m_theProcessProtocolFun[c2s_permit_logftihmcginqknd] = &CGameServer::_NotifyPlayerLogin;	//通知玩家可以登录 了
	m_theProcessProtocolFun[c2s_updatemapinfo] = &CGameServer::_UpdateMapID;                    //更新地图数量容器map
	m_theProcessProtocolFun[c2s_updategameserverinfo] = &CGameServer::_UpdateGameSvrInfo;	    //更新服务器IP 和端口
	m_theProcessProtocolFun[c2s_requestsvrip] = &CGameServer::_RequestSvrIP;                    //S3发过来的转服协议
	m_theProcessProtocolFun[c2s_msginfo] = &CGameServer::_Rc2smsginfo; 
	m_theProcessProtocolFun[c2s_msginfo_all] = &CGameServer::c2smsginfo_all;                    //全区公告信息
	m_theProcessProtocolFun[c2s_gm_synroleaction] = &CGameServer::gm_synroleaction;                    //全区公告信息
	m_theProcessProtocolFun[c2s_roleserver_lock]  = &CGameServer::_LockOrUnlockRole;
	m_theProcessProtocolFun[c2s_gm_synroleaction_result] = &CGameServer::gm_synroleaction_result; 
	m_theProcessProtocolFun[c2s_item_synaction] = &CGameServer::c2s_itemsynaction; 
	m_theProcessProtocolFun[c2s_item_synaction_result] = &CGameServer::c2s_itemsynaction_result; 
	
	
	
	

	/*
	 * Query gameserver information	  查询 GS 信息 更新数据
	 */

	_QueryGameSvrInfo();  //获取GS 的 IP 和 端口

	return _QueryMapID(); //设置地图信息
}

bool CGameServer::Destroy()
{
	{
		//CCriticalSection::Owner lock( m_csAITS );

		KAutoCriticalSection_ AutoLock( m_csAITS );

		stdAccountAttachIn::iterator it;
		
		for ( it = m_theAccountInThisServer.begin(); it != m_theAccountInThisServer.end(); ++it)
		{
			string sAccountName = ( *it ).first;
			
			FreezeMoney(sAccountName.c_str(), 0,0);
		}
	}

	m_dwCapability	= 0;
	m_nServerPort	= 0;
	m_nServerIP_Intraner = 0;
	m_nServerIP_Internet = 0;
	m_sServerIPAddr_Internet = "";
	m_sServerIPAddr_Intraner = "";
	
	{
		CCriticalSection::Owner locker( m_csMapIDAction );

		stdMapIDConvert::iterator itM2C;

		for ( itM2C = m_theMapIDConvert.begin(); itM2C != m_theMapIDConvert.end(); ++itM2C)
		{
			stdServerList &sl = ( *itM2C ).second;

			if ( !sl.empty() )
			{
				sl.remove( this );
			}
		}
	}

	return true;
}
//GS 连接信息
void __stdcall CGameServer::GameSvrEventNotify( LPVOID lpParam,
			const unsigned long &ulnID,
			const unsigned long &ulnEventType )
{
	CGameServer::LPNI pNI = reinterpret_cast< CGameServer::NI * >( lpParam );

	ASSERT( pNI );

	CCriticalSection::Owner locker( CGameServer::m_csGameSvrAction );

	switch ( ulnEventType )
	{
	case enumClientConnectCreate:
		{
			// Check the real TCP peer (not a claimed packet IP) before
			// constructing/activating any GameServer object.
			char remoteEndpoint[128] = {0};
			if (!pNI->pServer) break;
			pNI->pServer->GetClientInfo(ulnID, remoteEndpoint, sizeof(remoteEndpoint));
			if (!BishopGsPeerGuard::PeerAllowed(remoteEndpoint,
					pNI->trustedGameServerIPs.c_str()))
			{
				pNI->pServer->ShutdownClient(ulnID);
				break;
			}
			//GS建立链接
			IGServer *pGServer = new CGameServer(pNI->pServer,pNI->pClient,ulnID);//

			ASSERT(pGServer);

			pGServer->Create();	 //开始向GS 查询获取信息

			/*if (hDugList)
			{
				char nInfo[64]={0};
				sprintf(nInfo,"创建(%u)链接成功",ulnID);
			    AddOutputIpList(hDugList,nInfo);
			}*/
             //有个连接容器保存起来
			std::pair< CGameServer::stdGameSvr::iterator, bool > result = 
				CGameServer::m_theGameServers.insert( CGameServer::stdGameSvr::value_type( ulnID, pGServer ) );
			//-------------后来增加的 建立一个MAP容器来保存网络连接的 IDX-------------------
			//----------------------------------------------------------------------------
			if ( result.second && pNI->hwndContainer && ::IsWindow( pNI->hwndContainer ) )
			{
				::PostMessage( pNI->hwndContainer, WM_GAMESERVER_EXCHANGE, ADD_GAMESERVER_ACTION, ulnID );
			}
		}
		break;

	case enumClientConnectClose:
		{
			stdGameSvr::iterator it;

			if ( CGameServer::m_theGameServers.end() != 
				( it = CGameServer::m_theGameServers.find( ulnID ) ) )
			{
				if ( pNI->hwndContainer && ::IsWindow( pNI->hwndContainer ) )
				{
					::PostMessage( pNI->hwndContainer, WM_GAMESERVER_EXCHANGE, DEL_GAMESERVER_ACTION, ulnID );
				}

				IGServer *pGServer = ( *it ).second;

				ASSERT(pGServer);

				CGameServer::m_theGameServers.erase(it);//删除了这个服务器
				pGServer->Destroy();

				/*stdClientSerVer::iterator it;
				for( it = m_ClientSerVer.begin(); it!= m_ClientSerVer.end(); ++it)
				{
				if (it->second == ulnID)
				{
				m_ClientSerVer[it->first] = 0;
				}
				}*/

				/*if (hDugList)
				{
				  char nInfo[64]={0};
				  sprintf(nInfo,"删除(%u)链接成功",ulnID);
				  AddOutputIpList(hDugList,nInfo);
				}*/
				SAFE_DELETE(pGServer);
			}
		}
		break;
	}
}

bool CGameServer::Begin( IServer *pGameSvrServer )
{
	/*
	 * Startup a manager thread
	 */
	DWORD dwThreadID = 0;

	m_shManagerThread = ::CreateThread( NULL, 
				0, 
				ManagerThreadFunction, 
				( void * )pGameSvrServer,
				0, 
				&dwThreadID );
			
	if ( m_shManagerThread == INVALID_HANDLE_VALUE )
	{
		return false;
	}

	m_shStartupManagerThreadEvent.Set();

	return true;
}

void CGameServer::End()
{
	m_shQuitEvent.Set();

	m_shStartupManagerThreadEvent.Set();

	if ( WAIT_TIMEOUT == ::WaitForSingleObject( m_shManagerThread, 5000 ) )
	{
		::TerminateThread( m_shManagerThread, 0 );
	}

	SAFE_CLOSEHANDLE( m_shManagerThread );


	/*
	 * MapID & GameServer
	 */
	{
		CCriticalSection::Owner locker( m_csMapIDAction );

		stdMapIDConvert::iterator itM2C;

		for ( itM2C = m_theMapIDConvert.begin(); itM2C != m_theMapIDConvert.end(); ++itM2C)
		{
			stdServerList &SL = ( *itM2C ).second;

			SL.clear();
		}

		m_theMapIDConvert.erase( m_theMapIDConvert.begin(), m_theMapIDConvert.end() );				
	}

	/*
	 * Clear gameserver information
	 */
	{
		CCriticalSection::Owner locker( m_csGameSvrAction );

		m_theGameServers.erase( m_theGameServers.begin(), m_theGameServers.end() );
	}
}

DWORD WINAPI CGameServer::ManagerThreadFunction( void *pParam )
{
	IServer *pGameSvrServer = ( IServer * )pParam;

	ASSERT(pGameSvrServer);

	m_shStartupManagerThreadEvent.Wait();

	stdGameSvr::iterator it;

	while ( !m_shQuitEvent.Wait( 0 ) )
	{
		CCriticalSection::Owner locker( CGameServer::m_csGameSvrAction );

		for ( it = CGameServer::m_theGameServers.begin(); it != CGameServer::m_theGameServers.end(); ++it)
		{
			UINT nlnID = ( *it ).first;

			size_t datalength = 0;
			//获取某个gs发来的数据
			const void *pData = pGameSvrServer->GetPackFromClient( nlnID, datalength );

			if ( 0 == datalength || NULL == pData )
			{
				continue;
			}

			IGServer *pGServer = ( *it ).second;

			if ( pGServer )
			{
				pGServer->AnalyzeRequire( pData, datalength );
			}			
		}

		::Sleep( 1 );
	}

	SAFE_RELEASE( pGameSvrServer );

	return 0;
}

bool CGameServer::AnalyzeRequire( const void *pData, size_t datalength )
{
    if (!BishopPacketGuard::HasBytes(pData, datalength, 1))
        return false;
	bool ok = true;

	BYTE cProtocol = CPackager::Peek( pData );
	
	if ( cProtocol < g_nGlobalProtocolType )
	{
		return LargePackProcess( cProtocol, pData, datalength );
	}
	else if ( cProtocol > g_nGlobalProtocolType )
	{
		return SmallPackProcess( cProtocol, pData, datalength );
	}
	return false;
}

bool CGameServer::LargePackProcess( BYTE cProtocol, const void *pData, size_t datalength )
{
	return true;
}

bool CGameServer::SmallPackProcess( BYTE cProtocol, const void *pData, size_t datalength )
{
	if ( cProtocol < c2s_end && m_theProcessProtocolFun[cProtocol] )
	{
		return ( this->*m_theProcessProtocolFun[cProtocol] )( ( const void * )pData, datalength );
	}

	return false;
}
//同步人物信息给GS
bool CGameServer::DispatchTask( UINT nTask, const void *pData, size_t datalength, DWORD nData ,
	                           DWORD nLeftTime,int nVipType,DWORD nExtGameLeftTime,unsigned int nTempEnTerMapIndx,unsigned int nTempKeyId)
{
	bool ok = true;

	switch ( nTask )
	{//
	case enumSyncRoleInfo:

		ok = _SyncRoleInfo( pData, datalength, nData,nLeftTime,nVipType,nExtGameLeftTime,nTempEnTerMapIndx,nTempKeyId); //nData  扩展点
//		AddOutputStringA(hbishopOutput,"同步人物(GS)数据成功");
		if (ok)
		{
			char msg[128];
			sprintf(msg,"%s","---bishop(_SyncRoleInfo)->发往GS成功:同步人物(GS)数据成功---");
			bs_LogFile.gTraceLogFile(msg,sizeof(msg));
		}
		break;
		
	case enumPlayerLogicLogout:

		ok = _PlayerLogicLogout( pData, datalength );

		break;
	case enumTaskProtocol:
	default:		
		break;
	}

	return true;
}
//从GS获取地图ID 地图的数量
bool CGameServer::_QueryMapID()
{
	tagQueryMapInfo qmi;
	qmi.cProtocol = s2c_querymapinfo;
	qmi.nProtoKind = enum_COMMAND_ID_MAP;
	memset(qmi.nName,0,sizeof(qmi.nName));
	qmi.nKey      = 0;
	qmi.nVerX     = 0;
	qmi.nVerY     = 0;
	if (m_pGameSvrServer) m_pGameSvrServer->SendData(m_lnIdentityID,(const void *)&qmi,sizeof(tagQueryMapInfo));

	return true;
}
//从GS服务器获取 链接 IP 和端口
bool CGameServer::_QueryGameSvrInfo()
{
	tagQueryGameSvrInfo qgsi;

	qgsi.cProtocol = s2c_querygameserverinfo;

	if (m_pGameSvrServer) m_pGameSvrServer->SendData(m_lnIdentityID,(const void *)&qgsi, sizeof(tagQueryGameSvrInfo));

	return true;
}
//更新地图编号容器
bool CGameServer::_UpdateMapID(const void *pData,size_t datalength)
{
    if (!BishopPacketGuard::HasBytes(pData, datalength,
        offsetof(tagUpdateMapID, szMapID))) return false;
    const tagUpdateMapID* incoming = static_cast<const tagUpdateMapID*>(pData);
    if (!BishopPacketGuard::SafeMapSuffix(incoming->szparmName)) return false;
	tagUpdateMapID *pUMI = (tagUpdateMapID *)pData;

	int  nGsIndex = (int)pUMI->cMapCount;
//	int	*	pMapID = (int *)pUMI->szMapID; 

	CIniFile theMsgFile;
	_tstring sAppPath = GetAppFullPath( NULL );

	char nMapListPath[MAX_PATH],nTempPath[128];
	ZeroMemory(nMapListPath,sizeof(nMapListPath));
	ZeroMemory(nTempPath,sizeof(nTempPath));

	const int a = snprintf(nTempPath, sizeof(nTempPath), "maps\\WorldSet_%s.ini", pUMI->szparmName);
    if (a < 0 || static_cast<size_t>(a) >= sizeof(nTempPath)) return false;
    const int b = snprintf(nMapListPath, sizeof(nMapListPath), "%s%s", sAppPath.c_str(), nTempPath);
    if (b < 0 || static_cast<size_t>(b) >= sizeof(nMapListPath)) return false;

	theMsgFile.SetFile(nMapListPath);

	char nSection[32]={0},szKeyName[32]={0};;
	int nMapCount=theMsgFile.ReadInteger("Init","Count",0);
    int nMapCount_=nMapCount;

	char msg[128]={0};
	sprintf(msg,"--------------------------Bat Dau Loaded Map------------------------------");
	AddOutputStringPlayer(hbishopOutput,msg);

	while ( --nMapCount_ >= 0 )
	{//先删除相同的地图
		sprintf(szKeyName, "World%02d", nMapCount_);
		UINT nMapIdx=0;
		nMapIdx=theMsgFile.ReadInteger("World",szKeyName,0);
		if (nMapIdx>0)
		{
			stdMapIDConvert::iterator it;
			if (m_theMapIDConvert.end() != (it = m_theMapIDConvert.find(nMapIdx)))
			{//如果有这个地图
				snprintf(msg, sizeof(msg),"---Xoa[%s]Ban do(%d) ok---",nTempPath,nMapIdx);
				AddOutputStringPlayer(hbishopOutput,msg); 
				m_theMapIDConvert.erase(it);
			}
		}		
	}
	 
	while ( --nMapCount >= 0 )
	{
		sprintf(szKeyName, "World%02d", nMapCount);
		UINT nMapIdx=0;
		nMapIdx=theMsgFile.ReadInteger("World",szKeyName,0);
		if (nMapIdx>0)
		{
			RegisterServer(nMapIdx /*pMapID[nMapCount]*/, ( IGServer * )this );
			sprintf(msg,"---Ban Do loaded(%d:%d) ok---",nMapCount,nMapIdx/*pMapID[nMapCount]*/);
			AddOutputStringPlayer(hbishopOutput,msg); //hCtrlPlayr
		}		
	}

	//int  nCunSerNo = pUMI->cMapCount;
	//UINT nMapIdx=0;
	//while (--nMapCount >= 0)
	//{   
	//	char szKeyName[32]={0};
	//	sprintf(szKeyName, "World%02d", nMapCount);
	//	nMapIdx = theConfigFile.ReadInteger( "World",szKeyName,0);
	//	//RegisterServer(pMapID[nMapCount], (IGServer *)this);  //压入地图信息
	//	RegisterServer(nMapIdx, (IGServer *)this); 
	//	
	//}
	////RegisterServer(1, (IGServer *)this);  //压入地图信息
	////m_ClientSerVer[nCunSerNo]= m_lnIdentityID;//服务器容器
	snprintf(msg, sizeof(msg),"------(gs)May chu(%s)[%d]Khoi dong hoan tat, nguoi choi co the dang nhap tro choi binh thuong!-------",nTempPath,m_lnIdentityID);
	AddOutputStringPlayer(hbishopOutput,msg); //hCtrlPlayr
	return true;
}

/////////////////////////////////获取本地IP设置////////////////////////////////
int CGameServer::_GetLocalIpAddress(DWORD *pIntranetAddr, DWORD *pInternetAddr)
{
//#ifndef _STANDALONE
//	return ;//gGetMacAndIPAddress(NULL, pIntranetAddr, NULL, pInternetAddr);
//#else
//	char	szIp[16];
	DWORD	dwAddr1, dwAddr2;
	dwAddr1 = dwAddr2 = 0;
	WSADATA wsData;  
    ::WSAStartup(MAKEWORD(2,2), &wsData);

	char strHost[MAX_PATH];
	strHost[0] = 0;
	if(SOCKET_ERROR != gethostname(strHost, sizeof(strHost)))  //获取主机的名称
	{
		struct hostent* hp;
		hp = gethostbyname(strHost);  //获取主机的ip
	
#ifndef WIN32  //L系统
		if (!hp || hp && hp->h_addr_list[0] && *(unsigned long *)hp->h_addr_list[0] == 0x0100007f)
		{
			int sock;
			struct ifreq ifr;
			char* ip = NULL;
			int err = -1;

			sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
			if (sock >= 0)
			{
				strcpy(ifr.ifr_name, "eth0");
				ifr.ifr_addr.sa_family = AF_INET;
				err = ioctl(sock, SIOCGIFADDR, &ifr);
				if (err == 0) 
				{
					dwAddr1 = (((struct sockaddr_in*) &(ifr.ifr_addr))->sin_addr).s_addr;
					ip =  inet_ntoa(((struct sockaddr_in*) &(ifr.ifr_addr))->sin_addr);
//					printf("IP-addr1: %s\n", ip); 
				}

				strcpy(ifr.ifr_name, "eth1");
				ifr.ifr_addr.sa_family = AF_INET;

				err = ioctl(sock, SIOCGIFADDR, &ifr);
				if (err == 0) {
					dwAddr2 = (((struct sockaddr_in*) &(ifr.ifr_addr))->sin_addr).s_addr;
					ip =  inet_ntoa(((struct sockaddr_in*) &(ifr.ifr_addr))->sin_addr);
//					printf("IP-addr2: %s\n", ip);
				}
				else
					dwAddr2 = dwAddr1;
				closesocket(sock);
			}
		}
		else
#endif
		{

		  if(hp && hp->h_addr_list[0])
		  { 
			dwAddr1 = *(unsigned long *)hp->h_addr_list[0];
/*
			*((char *)&dwAddr1) = hp->h_addr_list[0][3];
			*((char *)&dwAddr1 + 1) = hp->h_addr_list[0][2];
			*((char *)&dwAddr1 + 2) = hp->h_addr_list[0][1];
			*((char *)&dwAddr1 + 3) = hp->h_addr_list[0][0];*/
//			dwAddr1 = *(unsigned long *)hp->h_addr_list[0];/
			if (!dwAddr1)
				dwAddr1 = *(unsigned long *)hp->h_addr_list[1];
		  } 

		  if(hp && hp->h_addr_list[1])
		  {
			dwAddr2 = *(unsigned long *)hp->h_addr_list[1];
		
/*
			*((char *)&dwAddr2) = hp->h_addr_list[1][3];
			*((char *)&dwAddr2 + 1) = hp->h_addr_list[1][2];
			*((char *)&dwAddr2 + 2) = hp->h_addr_list[1][1];
			*((char *)&dwAddr2 + 3) = hp->h_addr_list[1][0];*/
//			dwAddr2 = *(unsigned long *)hp->h_addr_list[1];

			if (!dwAddr2)
				dwAddr2 = *(unsigned long *)hp->h_addr_list[0];
			
		  }
		  else
		  {
			dwAddr2 = dwAddr1;
		  }
		}
	}

	if ((dwAddr1 & 0x0000FFFF) == 0x0000a8c0)	// intranet
	{
		*pIntranetAddr = dwAddr1;
		*pInternetAddr = dwAddr2;
	}
	else
	{
		*pIntranetAddr = dwAddr2;
		*pInternetAddr = dwAddr1;
	}
    ::WSACleanup();

	return TRUE;//gGetMacAndIPAddress(NULL,pIntranetAddr,NULL,pInternetAddr);
//#endif
}

//从GS获取更新服务器端口
bool CGameServer::_UpdateGameSvrInfo( const void *pData, size_t datalength )
{
    if (!BishopPacketGuard::ExactBytes(pData, datalength, sizeof(tagGameSvrInfo)))
        return false;
	ASSERT( sizeof( tagGameSvrInfo ) == datalength );

	tagGameSvrInfo *pGSI = ( tagGameSvrInfo * )pData;
	
	m_dwCapability  = pGSI->wCapability;    //最大人数限制
	m_nServerPort	= pGSI->nPort;          //登陆游戏的端口	56766
	m_serVerNoIndex = pGSI->nSerNoIndex;    //归属大区的gs编号
	m_serGsNumber   = pGSI->nGsNumber;;     //同一个大区的gs编号
/*
	char strIP[32]={0};	
	iniFile.GetString("GameServer", "Ip","127.0.0.1",strIP,sizeof(strIP));	
	m_dwInternetIp=inet_addr(strIP);
	*/
	//---------------------------单机设置
	/*DWORD  m_dwIntranetIp=0,m_dwInternetIp=0;
	  _GetLocalIpAddress(&m_dwIntranetIp, &m_dwInternetIp);
	if (m_dwInternetIp!=140814528)      //inet_addr("192.168.100.8")
	   m_nServerIP_Internet = 140814528;//inet_addr("192.168.100.8");
	else
	   m_nServerIP_Internet = m_dwInternetIp;*/
	//-----------------------------------
	m_nServerIP_Internet = pGSI->nIPAddr_Internet;
	m_nServerIP_Intraner = pGSI->nIPAddr_Intraner;

	m_sServerIPAddr_Intraner = net_ntoa(m_nServerIP_Intraner);
	m_sServerIPAddr_Internet = net_ntoa(m_nServerIP_Internet);

	return true;
}


bool CGameServer::c2s_itemsynaction_result( const void *pData, size_t datalength )
{
    if (!BishopPacketGuard::ExactBytes(pData, datalength, sizeof(VIEW_LIAN_ITEM_SYNC)))
        return false;
    if (!BishopPacketGuard::HasNul(reinterpret_cast<const char*>(
            static_cast<const VIEW_LIAN_ITEM_SYNC*>(pData)->m_WonName),
            sizeof(static_cast<const VIEW_LIAN_ITEM_SYNC*>(pData)->m_WonName)))
        return false;
	if  (!pData) return false;
	ASSERT( sizeof( VIEW_LIAN_ITEM_SYNC ) == datalength);
	IGServer *pGServer =NULL;
	VIEW_LIAN_ITEM_SYNC *pItemData = ( VIEW_LIAN_ITEM_SYNC * )pData;
	CCriticalSection::Owner locker( CGameServer::m_csGameSvrAction );
	if  (pItemData->m_WonName[0])
	{//如果获取数据的玩家存在 就开始转发...
		RoleLoginInfo resInfo;
		memset(&resInfo,0,sizeof(resInfo));
		if (getRoleLock(pItemData->m_WonName,&resInfo))
		{
			pGServer = CGameServer::GetSerVerIdByIndex(resInfo.m_RegServerIndex,resInfo.m_GsNumBer);
			if (pGServer)
				pGServer->SendData(pGServer->GetID(),pData,sizeof(VIEW_LIAN_ITEM_SYNC));
		}
	}
	return true;
};

bool CGameServer::c2s_itemsynaction( const void *pData, size_t datalength )
{
    if (!BishopPacketGuard::ExactBytes(pData, datalength, sizeof(tagItemAction)))
        return false;
    if (!BishopPacketGuard::HasNul(reinterpret_cast<const char*>(
            static_cast<const tagItemAction*>(pData)->nRoleName),
            sizeof(static_cast<const tagItemAction*>(pData)->nRoleName)))
        return false;
    if (!BishopPacketGuard::HasNul(reinterpret_cast<const char*>(
            static_cast<const tagItemAction*>(pData)->nGmRoleName),
            sizeof(static_cast<const tagItemAction*>(pData)->nGmRoleName)))
        return false;
	if  (!pData) return false;
	tagItemAction *pRSI = ( tagItemAction * )pData;
	ASSERT( sizeof( tagItemAction ) == datalength);
	IGServer *pGServer =NULL;
	CCriticalSection::Owner locker( CGameServer::m_csGameSvrAction );
	RoleLoginInfo resInfo;
	memset(&resInfo,0,sizeof(resInfo));
	if (getRoleLock(pRSI->nRoleName,&resInfo))
	{
		pGServer = CGameServer::GetSerVerIdByIndex(resInfo.m_RegServerIndex,resInfo.m_GsNumBer);
		if (pGServer)
		{//对方的所在的gs
			tagItemAction qmi;
			qmi.cProtocol = s2c_queryiteminfo;
			qmi.cCmdType =(int)pRSI->cCmdType;
			snprintf(qmi.nGmRoleName, sizeof(qmi.nGmRoleName), "%s", pRSI->nGmRoleName);
			snprintf(qmi.nRoleName, sizeof(qmi.nRoleName), "%s", pRSI->nRoleName);
			qmi.Key      = pRSI->Key;
			qmi.verX     = pRSI->verX;
			qmi.verY     = pRSI->verY;
			qmi.nId      = pRSI->nId;
			pGServer->SendData(pGServer->GetID(),(const void *)&qmi,sizeof(tagItemAction));
		}
	}

	return true;

}
//执行从gs发送过来的数据
bool CGameServer::gm_synroleaction_result( const void *pData, size_t datalength )
{
    if (!BishopPacketGuard::ExactBytes(pData, datalength, sizeof(tagGmAction)))
        return false;
    if (!BishopPacketGuard::HasNul(reinterpret_cast<const char*>(
            static_cast<const tagGmAction*>(pData)->nRoleName),
            sizeof(static_cast<const tagGmAction*>(pData)->nRoleName)))
        return false;
    if (!BishopPacketGuard::HasNul(reinterpret_cast<const char*>(
            static_cast<const tagGmAction*>(pData)->nGmRoleName),
            sizeof(static_cast<const tagGmAction*>(pData)->nGmRoleName)))
        return false;
	if  (!pData) return false;
	tagGmAction *pRSI = ( tagGmAction * )pData;
	ASSERT( sizeof( tagGmAction ) == datalength);
	IGServer *pGServer =NULL;
	CCriticalSection::Owner locker( CGameServer::m_csGameSvrAction );
	RoleLoginInfo resInfo;
	memset(&resInfo,0,sizeof(resInfo));
	if (getRoleLock(pRSI->nGmRoleName,&resInfo))
	{//再转发数据到 源gs执行
		pGServer = CGameServer::GetSerVerIdByIndex(resInfo.m_RegServerIndex,resInfo.m_GsNumBer);
		if (pGServer)
		{
			tagQueryMapInfo qmi;
			qmi.cProtocol = s2c_querymapinfo;
			qmi.nProtoKind =(int)pRSI->cCmdType;
			snprintf(qmi.nName, sizeof(qmi.nName), "%s", pRSI->nGmRoleName);
			qmi.nKey      = pRSI->Key;
			qmi.nVerX     = pRSI->verX;
			qmi.nVerY     = pRSI->verY;
			pGServer->SendData(pGServer->GetID(),(const void *)&qmi,sizeof(tagQueryMapInfo));
		}
	}

	return true;
}

bool CGameServer::gm_synroleaction(const void *pData, size_t datalength)
{
    if (!BishopPacketGuard::ExactBytes(pData, datalength, sizeof(tagGmAction)))
        return false;
    if (!BishopPacketGuard::HasNul(reinterpret_cast<const char*>(
            static_cast<const tagGmAction*>(pData)->nRoleName),
            sizeof(static_cast<const tagGmAction*>(pData)->nRoleName)))
        return false;
    if (!BishopPacketGuard::HasNul(reinterpret_cast<const char*>(
            static_cast<const tagGmAction*>(pData)->nGmRoleName),
            sizeof(static_cast<const tagGmAction*>(pData)->nGmRoleName)))
        return false;
	if  (!pData) return false;
	tagGmAction *pRSI = ( tagGmAction * )pData;
	ASSERT( sizeof( tagGmAction ) == datalength);
	IGServer *pGServer =NULL;
	CCriticalSection::Owner locker( CGameServer::m_csGameSvrAction );
	RoleLoginInfo resInfo;
	memset(&resInfo,0,sizeof(resInfo));
	if (getRoleLock(pRSI->nRoleName,&resInfo))
	{
		pGServer = CGameServer::GetSerVerIdByIndex(resInfo.m_RegServerIndex,resInfo.m_GsNumBer);
		if (pGServer)
		{//对方的所在的gs
			tagQueryMapInfo qmi;
			qmi.cProtocol = s2c_querymapinfo;
			qmi.nProtoKind =(int)pRSI->cCmdType;
			if (qmi.nProtoKind==enum_COMMAND_ID_MOVETOP_GM)
				snprintf(qmi.nGmName, sizeof(qmi.nGmName), "%s", pRSI->nGmRoleName);
			else
				memset(qmi.nGmName,0,sizeof(qmi.nGmName));

			snprintf(qmi.nName, sizeof(qmi.nName), "%s", pRSI->nRoleName);
			qmi.nKey      = pRSI->Key;
			qmi.nVerX     = pRSI->verX;
			qmi.nVerY     = pRSI->verY;
			pGServer->SendData(pGServer->GetID(),(const void *)&qmi,sizeof(tagQueryMapInfo));
		}
	}

	//for ( it = CGameServer::m_theGameServers.begin();it != CGameServer::m_theGameServers.end(); ++it)
	//{
	//	pGServer =NULL;
	//	memset(&resInfo,0,sizeof(resInfo));
	//	IGServer *pGServer = ( *it ).second;
	//	if (pGServer && pGServer->getRoleLock(pRSI->nRoleName,&resInfo))
	//	{
	//		tagQueryMapInfo qmi;
	//		qmi.cProtocol = s2c_querymapinfo;
	//		qmi.nProtoKind =(int)pRSI->cCmdType;
	//		sprintf(qmi.nName,pRSI->nRoleName);
	//		//CGameServer::SendData(m_lnIdentityID,(const void *)&qmi,sizeof(tagQueryMapInfo));
	//		pGServer->SendData(pGServer->GetID(),(const void *)&qmi,sizeof(tagQueryMapInfo));

	//		//pGServer->SendData(pGServer->GetID(),pData,datalength);
	//	}
	//}

	return true;
}


bool CGameServer::c2smsginfo_all(const void *pData, size_t datalength)
{
    if (!BishopPacketGuard::ExactBytes(pData, datalength, sizeof(tagMsgInGame)))
        return false;
    if (!BishopPacketGuard::HasNul(reinterpret_cast<const char*>(
            static_cast<const tagMsgInGame*>(pData)->strMsg),
            sizeof(static_cast<const tagMsgInGame*>(pData)->strMsg)))
        return false;
	tagMsgInGame *pRSI = ( tagMsgInGame * )pData;
	ASSERT( sizeof( tagMsgInGame ) == datalength);
	IGServer *pGServer =NULL;

	tagMsgInGame nGsReturn;
	nGsReturn.cProtocol   = s2c_gwsyn_broadcast;
	nGsReturn.cCmdType = pRSI->cCmdType;
	nGsReturn.nSerIdx     = pRSI->nSerIdx;
	snprintf(nGsReturn.strMsg, sizeof(nGsReturn.strMsg), "%s", pRSI->strMsg);

	if  (pRSI->cCmdType==AR_ADDNPC_SERVER)
	{
		nGsReturn.nMgsIdx = pRSI->nMgsIdx;
		IGServer *pGServer = NULL;
		pGServer = CGameServer::QueryServer(nGsReturn.nMgsIdx,nGsReturn.nSerIdx);
		if (pGServer && m_pGameSvrServer)
			m_pGameSvrServer->SendData(pGServer->GetID(),(const void *)&nGsReturn, sizeof(tagMsgInGame));
		return true;
	}

	stdGameSvr::iterator it;
	CCriticalSection::Owner locker( CGameServer::m_csGameSvrAction );
	for ( it = CGameServer::m_theGameServers.begin();it != CGameServer::m_theGameServers.end(); ++it)
	{
		pGServer =NULL;
		IGServer *pGServer = ( *it ).second;
		if (pGServer)
		{
			if (m_pGameSvrServer && pGServer->GetSerNoIndex()==pRSI->nSerIdx)
				m_pGameSvrServer->SendData(pGServer->GetID(),(const void *)&nGsReturn, sizeof(tagMsgInGame));
		}
	}
	return true;
}


bool CGameServer::_Rc2smsginfo(const void *pData, size_t datalength)
{
    if (!BishopPacketGuard::ExactBytes(pData, datalength, sizeof(tagMsgInGame)))
        return false;
    if (!BishopPacketGuard::HasNul(reinterpret_cast<const char*>(
            static_cast<const tagMsgInGame*>(pData)->strMsg),
            sizeof(static_cast<const tagMsgInGame*>(pData)->strMsg)))
        return false;
	//return false;//取消
	tagMsgInGame *pRSI = ( tagMsgInGame * )pData;
	ASSERT( sizeof( tagMsgInGame ) == datalength);
	IGServer *pGServer =NULL;
    if (pRSI->cCmdType!=0)
   { 
	   tagMsgInGame nGsReturn;
       nGsReturn.cProtocol   = s2c_gwsyn_broadcast;
	   nGsReturn.cCmdType    = pRSI->cCmdType;
       nGsReturn.nMgsIdx     = pRSI->nMgsIdx;
	   nGsReturn.nSerIdx     = pRSI->nSerIdx;
	   nGsReturn.nValIdx     = pRSI->nValIdx;

	   if (pRSI->nSerIdx!=0)
	   {//某个大区
		   stdGameSvr::iterator it;
		   CCriticalSection::Owner locker( CGameServer::m_csGameSvrAction );
		   for ( it = CGameServer::m_theGameServers.begin();it != CGameServer::m_theGameServers.end(); ++it)
		   {
			   pGServer =NULL;
			   IGServer *pGServer = ( *it ).second;
			   if (pGServer && pGServer->GetSerNoIndex()==pRSI->nSerIdx)
			   {//整个大区 
				   if (m_pGameSvrServer)
					   m_pGameSvrServer->SendData(pGServer->GetID(),(const void *)&nGsReturn, sizeof(tagMsgInGame));
			   }
		   }
	   }
	   else
	   {//所有大区都推送
		   stdGameSvr::iterator it;
		   CCriticalSection::Owner locker( CGameServer::m_csGameSvrAction );
		   for ( it = CGameServer::m_theGameServers.begin();it != CGameServer::m_theGameServers.end(); ++it)
		   {
			   pGServer =NULL;
			   IGServer *pGServer = ( *it ).second;
			   if (pGServer)
			   {
				   if (m_pGameSvrServer)
					   m_pGameSvrServer->SendData(pGServer->GetID(),(const void *)&nGsReturn, sizeof(tagMsgInGame));
			   }
		   }
	   }
	   return true;
    } 
	//pRSI->cCmdType==0
	CIniFile theMsgFile;
	_tstring sAppPath = GetAppFullPath( NULL );
	_tstring sMsgFile;
	sMsgFile = sAppPath + "Settings\\msg\\systemmsg.ini";
	theMsgFile.SetFile(sMsgFile.c_str());
	char nSection[32]={0},pText[256]={0};
	int  uLength;
	sprintf(nSection,"%d",pRSI->nMgsIdx);
	uLength = sizeof(pText);
	theMsgFile.ReadString(nSection,"strTxt",pText,uLength,"");
	//g_theIniFile.ReadString( "Role", "Main", pText, 32, "" );
	if  (!pText[0])
		return false;

	if (pRSI->nSerIdx==0)
	{//全部的GS服务器发送信息
		CGameServer::SendToAll(pText,uLength,AP_NOTIFY_ALL_PLAYER);
	}
	else
	{
		//pGServer = CGameServer::GetSerVerIdByIndex(pRSI->nSerIdx);//GetServer(1);//0CGameServer::QueryServer(pRSI->dwMapID);
		//if (pGServer)
		//{  
		//	pGServer->SendText(pText,uLength,AP_NOTIFY_ALL_PLAYER);
		//}
		stdGameSvr::iterator it;
		CCriticalSection::Owner locker( CGameServer::m_csGameSvrAction );
		for ( it = CGameServer::m_theGameServers.begin();it != CGameServer::m_theGameServers.end(); ++it)
		{
			pGServer =NULL;
			IGServer *pGServer = ( *it ).second;
			if (pGServer && pGServer->GetSerNoIndex()==pRSI->nSerIdx)
				pGServer->SendText(pText,uLength,AP_NOTIFY_ALL_PLAYER);
		}
	}

	return true;
}

bool CGameServer::_RequestSvrIP( const void *pData, size_t datalength )
{
    if (!BishopPacketGuard::ExactBytes(pData, datalength, sizeof(tagRequestSvrIp)))
        return false;
	if  (!pData) return false;

	tagRequestSvrIp *pRSI = ( tagRequestSvrIp * )pData;

	ASSERT( sizeof( tagRequestSvrIp ) == datalength );

	DWORD dwIP = 0;                                //0为S3连接 
    //目标gs的地图
	IGServer *pGServer =CGameServer::QueryServer(pRSI->dwMapID,pRSI->nSerNoIndex);

	if (pGServer)
	{
		dwIP = pGServer->GetIP(pRSI->cIPType);  //获取这个gs的ip 没有端口
	}
	/*else
	{
	char nInfo[64]={0};
	sprintf(nInfo,"收到 _RequestSvrIP S3 (%u) ID(%d) 目标map(%u)请求失败(gs:%u)",dwIP,GetID(),pRSI->dwMapID,pRSI->nSerNoIndex);
	AddOutputIpList(hbishopOutput,nInfo);
	return false;
	}*/
	
	tagNotifySvrIp nsi;

	nsi.cProtocol = s2c_notifysvrip;
	nsi.pckgID  = pRSI->pckgID;
	nsi.dwMapID = pRSI->dwMapID;
	nsi.cIPType = pRSI->cIPType;
	nsi.dwSvrIP = dwIP;  //目标gs的真实ip
	//发往 S3  0为s3 连接
	if (m_pGameSvrServer) m_pGameSvrServer->SendData(m_lnIdentityID,( const void * )&nsi, sizeof( tagNotifySvrIp ) );//m_lnIdentityID

	char nInfo[128]={0};
	snprintf(nInfo, sizeof(nInfo),"收到 _RequestSvrIP S3 (%u) ID(%zu) 目标map(%u)请求成功(gs:%u)",dwIP,GetID(),pRSI->dwMapID,pRSI->nSerNoIndex);
	AddOutputIpList(hbishopOutput,nInfo);

	return true;
}
//接受GS 发送过来的数据包  并把数据发往 game 客户端通知可以登陆！
bool CGameServer::_NotifyPlayerLogin(const void *pData, size_t datalength)
{
    if (!BishopPacketGuard::ExactBytes(pData, datalength, sizeof(tagPermitPlayerLogin)))
        return false;
    if (!BishopPacketGuard::HasNul(reinterpret_cast<const char*>(
            static_cast<const tagPermitPlayerLogin*>(pData)->szRoleName),
            sizeof(static_cast<const tagPermitPlayerLogin*>(pData)->szRoleName)))
        return false;
    if (!BishopPacketGuard::HasNul(reinterpret_cast<const char*>(
            static_cast<const tagPermitPlayerLogin*>(pData)->szAccountName),
            sizeof(static_cast<const tagPermitPlayerLogin*>(pData)->szAccountName)))
        return false;
    if (!BishopPacketGuard::HasNul(reinterpret_cast<const char*>(
            static_cast<const tagPermitPlayerLogin*>(pData)->strRoleNmae),
            sizeof(static_cast<const tagPermitPlayerLogin*>(pData)->strRoleNmae)))
        return false;
	tagPermitPlayerLogin *pPPL = (tagPermitPlayerLogin *)pData;

	ASSERT(sizeof(tagPermitPlayerLogin)==datalength);

	bool ok = false;
	char Mgs[128]={0};

	char nTempRoleName[64];
    ZeroMemory(nTempRoleName,sizeof(nTempRoleName));
	snprintf(nTempRoleName, sizeof(nTempRoleName), "%s", pPPL->strRoleNmae);
	IPlayer *pPlayer = CGamePlayer::Get(nTempRoleName);

	if (pPlayer)
	{
		tagNotifyPlayerLogin npl;
		npl.cProtocol = s2c_notifypl_ayerlogin_hfkqotivbhd;  //发往客户端---通知客户端可以登陆了
		int nMinLen = strlen((const char *)pPPL->szRoleName);
		nMinLen = nMinLen > sizeof(npl.szRoleName) - 1 ? sizeof(npl.szRoleName) - 1 : nMinLen;
		memcpy((char *)npl.szRoleName, (const char *)pPPL->szRoleName, nMinLen);
		npl.szRoleName[nMinLen] = '\0';

		nMinLen = strlen((const char *)pPPL->szAccountName);
		nMinLen = nMinLen > sizeof(npl.szAccountName) - 1 ? sizeof(npl.szAccountName) - 1 : nMinLen;
		memcpy((char *)npl.szAccountName, (const char *)pPPL->szAccountName, nMinLen);
		npl.szAccountName[nMinLen] = '\0';

		memcpy(&(npl.guid), &(pPPL->guid), sizeof(GUID));//和唯一登陆标示
		
		npl.bPermit = pPPL->bPermit; 

		//如果开启预备建立角色
		if (BST_CHECKED == ::IsDlgButtonChecked(hMianHwd,IDC_CHECK_OPEN_YUJIAN))
		{
			if(hYuJianCheckList)
			{//账号检测
				char buf[ACC_LENGTH] = {0};
				int nCount = ::SendMessage(hYuJianCheckList,LB_GETCOUNT, 0, 0);
				bool isHave=false;
				for(int i=0; i<nCount;i++)
				{
					const LRESULT itemLen = ::SendMessage(hYuJianCheckList, LB_GETTEXTLEN, i, 0);
                    if (itemLen == LB_ERR || itemLen < 0 ||
                        static_cast<size_t>(itemLen) >= sizeof(buf)) continue;
                    ::SendMessage(hYuJianCheckList, LB_GETTEXT, i, (LPARAM)buf);	//获取字符串
					if (strstr((char *)pPPL->szAccountName,buf))
					{
						isHave =true;
						break;
					}
				}
				if  (isHave) //存在就过滤掉。可以登录
				{
					npl.nIPAddr = m_nServerIP_Internet; 
					npl.nPort	= m_nServerPort;
				}
				else//否则就不能登陆  
				{
					if (BST_CHECKED == ::IsDlgButtonChecked(hMianHwd,IDC_CHECK_OPEN_ALLSERVER))
					{//全区全服启用
						npl.nIPAddr = 0; 
						npl.nPort	= 3;
						npl.bPermit = false;
					}
					else
					{//没有开启全区全服
						BOOL bTranslated = TRUE;
						UINT _RegSerVerIndex = ::GetDlgItemInt(hMianHwd, IDC_EDIT_FU, &bTranslated, FALSE);
						UINT _GsNumBerIndex  = ::GetDlgItemInt(hMianHwd, IDC_EDIT_QUYU, &bTranslated, FALSE);
						if  (m_serVerNoIndex==_RegSerVerIndex  && m_serGsNumber==_GsNumBerIndex)
						{
							npl.nIPAddr = 0; 
							npl.nPort	= 3;
							npl.bPermit = false;
						}
						else
						{
							npl.nIPAddr = m_nServerIP_Internet; 
							npl.nPort	= m_nServerPort;
						}
					}					 
				}
			}	
		}
		else 
		{//不检测
			npl.nIPAddr = m_nServerIP_Internet; 
			npl.nPort	= m_nServerPort;        //把GS的游戏端口发往客户端 56766
		}

		
		//加入数据队列
		ok = pPlayer->AppendData(CGamePlayer::enumOwnerPlayer,(const void *)&npl,sizeof(tagNotifyPlayerLogin));
		if (ok)
		{
			snprintf(Mgs, sizeof(Mgs),"GS Send,LogIn Succes %s, NV: %s port: %d", (const char *)pPPL->szAccountName,(const char *)(pPPL->szRoleName),m_nServerPort);	 
			AddOutputIpList(hDugList,Mgs);
			bs_LogFile.gTraceLogFile(Mgs,sizeof(Mgs));
		}
	}
	else
	{
		snprintf(Mgs, sizeof(Mgs),"GS Send, Faild LogIn %s, NV: %s port: %d ", (const char *)pPPL->szAccountName,(const char *)(pPPL->szRoleName),m_nServerPort);	 
		AddOutputIpList(hDugList,Mgs);
	}

	return ok;
}

bool CGameServer::_RegisterAccount( const void *pData, size_t datalength )
{
    if (!BishopPacketGuard::ExactBytes(pData, datalength, sizeof(tagRegisterAccount)))
        return false;
    if (!BishopPacketGuard::HasNul(reinterpret_cast<const char*>(
            static_cast<const tagRegisterAccount*>(pData)->szAccountName),
            sizeof(static_cast<const tagRegisterAccount*>(pData)->szAccountName)))
        return false;
	ASSERT( pData && datalength );

	tagRegisterAccount *pRA = ( tagRegisterAccount * )pData;

	if ( sizeof( tagRegisterAccount ) != datalength )
	{
		return false;
	}
	//ASSERT(pRoleData->BaseInfo.szName[0] !='\0');
	char Mgs[128];
	snprintf(Mgs, sizeof(Mgs),"-------收到跨服GS信息;账号:%s ------ ", (const char *)pRA->szAccountName);
	AddOutputIpList(hDugList,Mgs);

	//CGamePlayer::Add((const char *)pRA->szAccountName,(IPlayer *)this);

	//AttatchAccountToGameServer((const char *)pRA->szAccountName);

	return true;
}
//通知开始进入游戏
bool CGameServer::_NotifyEnterGame(const void *pData, size_t datalength)
{
    if (!BishopPacketGuard::ExactBytes(pData, datalength, sizeof(tagEnterGame)))
        return false;
    if (!BishopPacketGuard::HasNul(reinterpret_cast<const char*>(
            static_cast<const tagEnterGame*>(pData)->szAccountName),
            sizeof(static_cast<const tagEnterGame*>(pData)->szAccountName)))
        return false;
	ASSERT(pData && datalength);

	tagEnterGame *pEG = (tagEnterGame *)pData;

	if (sizeof(tagEnterGame) != datalength)
	{
		return false;
	}

	BYTE *pAccountName = (BYTE *)(pEG->szAccountName);

	    PushAccount((const char *)pAccountName);

	return true;
}

//通知pays离开游戏
bool CGameServer::_NotifyLeaveGame(const void *pData, size_t datalength)
{
    if (!BishopPacketGuard::ExactBytes(pData, datalength, sizeof(tagLeaveGame)))
        return false;
    if (!BishopPacketGuard::HasNul(reinterpret_cast<const char*>(
            static_cast<const tagLeaveGame*>(pData)->szAccountName),
            sizeof(static_cast<const tagLeaveGame*>(pData)->szAccountName)))
        return false;
	ASSERT(pData && datalength);
	
	tagLeaveGame *pLG = (tagLeaveGame *)pData;

	ASSERT(sizeof(tagLeaveGame) == datalength);

	BYTE *pAccountName = (BYTE *)(pLG->szAccountName);
/*
	IPlayer *pPlayer = CGamePlayer::Get((const char *)(pLG->nClientIp));
	if (pPlayer)
	{//通知客户端 已经断开连接了 是否需要重新连接
		bool ok=false;
		char Mgs[128]={0};
		tagNotifyPlayerLogin npl;
		npl.cProtocol = s2c_logiclogout;  //发往客户端---通知客户端可以登陆了
		int nMinLen = strlen((const char *)pLG->nClientIp);
		nMinLen = nMinLen > sizeof(npl.szRoleName) ? sizeof(npl.szRoleName) : nMinLen;
		memcpy((char *)npl.szRoleName, (const char *)pLG->nClientIp, nMinLen);
		npl.szRoleName[nMinLen] = '\0';

		nMinLen = strlen((const char *)pLG->szAccountName);
		nMinLen = nMinLen > sizeof(npl.szAccountName) ? sizeof(npl.szAccountName) : nMinLen;
		memcpy((char *)npl.szAccountName, (const char *)pLG->szAccountName, nMinLen);
		npl.szAccountName[nMinLen] = '\0';

		//memcpy(&(npl.guid), &(pPPL->guid), sizeof(GUID));//和唯一登陆标示
		npl.bPermit = false; 
		ok = pPlayer->AppendData(CGamePlayer::enumOwnerPlayer,(const void *)&npl,sizeof(tagNotifyPlayerLogin));
		if (ok)
		{
			sprintf(Mgs,"--通知玩家断网,账号:%s,角色:%s,端口:%d--", (const char *)pLG->szAccountName,(const char *)(pLG->nClientIp),m_nServerPort);	 
			AddOutputIpList(hDugList,Mgs);
			bs_LogFile.gTraceLogFile(Mgs,sizeof(Mgs));
		}
	}
	*/
	return PopAccount((const char *)pAccountName,(pLG->cCmdType != HOLDACC_LEAVEGAME), pLG->nExtPoint,pLG->nGameExtTime);
}
//进入游戏时调用
bool CGameServer::PushAccount( const char *pAccountName )
{
	ASSERT( pAccountName );

	return ConsumeMoney( pAccountName );
}
//离开游戏时调用
bool CGameServer::PopAccount( const char *pAccountName, bool bUnlockAccount, LONG nExtPoint,LONG nGameExtTime )
{
	ASSERT(pAccountName);

	if (HaveAccountInGameServer(pAccountName))
	{//已经存在就删除
		if (bUnlockAccount)
		{//设置扩展点存档 数据库
			FreezeMoney( pAccountName, nExtPoint,nGameExtTime);	//告诉pays 退出游戏
		}

		DetachAccountFromGameServer(pAccountName);
		
		return true;
	}

	return false;
}

bool CGameServer::Attach( const char *pAccountName)
{
	return AttatchAccountToGameServer(pAccountName);
}

//往容器中 插入一个账号元素
bool CGameServer::AttatchAccountToGameServer( const char *pAccountName )
{
	if ( !pAccountName || pAccountName==NULL|| !pAccountName[0] )
		return false;

	if (NAME_LENGHT > strlen( pAccountName ) )
	{
		//CCriticalSection::Owner lock( m_csAITS );
		KAutoCriticalSection_ AutoLock(m_csAITS);
		//if (HaveAccountInGameServer(pAccountName))//已经存在就删除
		//	return true;

		std::pair< stdAccountAttachIn::iterator, bool > result = 
			m_theAccountInThisServer.insert( stdAccountAttachIn::value_type(pAccountName, reinterpret_cast< void * >( this ) ) ); //往容器中 插入一个账号元素

		return result.second;
	}

	return false;
}

bool CGameServer::HaveAccountInGameServer( const char *pAccountName )
{
	if ( !pAccountName || !pAccountName[0] )
	{
		return false;
	}

	if (NAME_LENGHT > strlen( pAccountName ) )
	{
		//CCriticalSection::Owner lock( m_csAITS );
		KAutoCriticalSection_ AutoLock( m_csAITS );

		stdAccountAttachIn::iterator it;

		if ( m_theAccountInThisServer.end() != 
			( it = m_theAccountInThisServer.find( pAccountName ) ) )
		{
			return true;
		}
	}

	return false;
}

bool CGameServer::DetachAccountFromGameServer( const char *pAccountName )
{
	if ( !pAccountName || !pAccountName[0] )
	{
		return false;
	}

	if (NAME_LENGHT > strlen( pAccountName ) )
	{
		//CCriticalSection::Owner lock( m_csAITS );
		KAutoCriticalSection_ AutoLock( m_csAITS );

		stdAccountAttachIn::iterator it;

		if ( m_theAccountInThisServer.end() != 
			( it = m_theAccountInThisServer.find( pAccountName ) ) )
		{
			CGameServer *pThis = reinterpret_cast< CGameServer * >( ( *it ).second );

			ASSERT( pThis && pThis == this );
			
			m_theAccountInThisServer.erase( it );

			return true;
		}
	}

	return false;
}

bool CGameServer::ConsumeMoney( const char *pAccountName )
{
	if ( !pAccountName || !pAccountName[0] )
	{
		return false;
	}

	if (NAME_LENGHT <= strlen( pAccountName ) )
	{
		return false;
	}

	CBuffer *pBuffer = m_theGlobalAllocator.Allocate();
	
	BYTE *pContext = const_cast< BYTE * >( pBuffer->GetBuffer() );
	
	const size_t len = sizeof( KAccountUser ) + 1;
	
	KAccountUser user;
	
	user.Size = sizeof( KAccountUser );
	user.Type = AccountUser;
	user.Version = ACCOUNT_CURRENT_VERSION;

	size_t length = strlen( pAccountName );
	length = ( length > LOGIN_USER_ACCOUNT_MAX_LEN - 1 ) ? LOGIN_USER_ACCOUNT_MAX_LEN - 1 : length;	// the terminator below lands on Account[length], so the slot has to stay free
	memcpy( user.Account, pAccountName, length );
	user.Account[length] = '\0';
	//开始进入游戏
	*pContext = c2s_gamelogin_dkiqopjgurnmvbgc;   //通知paysys
	memcpy( pContext + 1, &user, sizeof( KAccountUser ) );
	
	g_theSmartClient.Send( pContext, len );
	//m_pAccountClient->SendPackToServer( pContext, len );
	
	SAFE_RELEASE( pBuffer );

	return true;
}
//冻结金钱
bool CGameServer::FreezeMoney( const char *pAccountName, LONG nExtPoint,LONG nGameExtTime)
{
	if (!pAccountName || !pAccountName[0])
	{
		return false;
	}

	if (NAME_LENGHT <= strlen(pAccountName))
	{
		return false;
	}
    
	//char Address[128]={0},nIpInfo[128]={0}; //IP地址
	////const char* pInfo=
	//m_pGameSvrServer->GetClientInfo(m_lnIdentityID,nIpInfo);
	//if (nIpInfo[0])
	//{
	//	DWORD m_IP = _a2ip(nIpInfo);
	//	const unsigned char* pIpAddress=	(const unsigned char*)&m_IP;  
	//	sprintf(Address, "%d.%d.%d.%d", pIpAddress[0],pIpAddress[1],pIpAddress[2],pIpAddress[3]);
	//	//const char* pColon = strchr(pInfo, ':');
	//	//if (pColon)
	//	//m_port = atoi(pColon + 1);  //加为了去除冒号  //端口
	//}

	/*if (hHaveConnetIpList)
	{
		//AddOutputSerIpList(hHaveConnetIpList,pLG->nClientIp);
		char buf[64] = {0};
		int nCount = ::SendMessage(hHaveConnetIpList,LB_GETCOUNT, 0, 0);
		
		for(int i=0; i<nCount; ++i)
		{
			::SendMessage(hHaveConnetIpList,LB_GETTEXT,i,(LPARAM)buf);	//获取字符串
			// AfxMessageBox(buf);
			if (strstr(Address,buf))
			{
				::SendMessage(hHaveConnetIpList,LB_DELETESTRING,i,0);
				break;
			}
		}	
	}
 */
	CBuffer *pBuffer = m_theGlobalAllocator.Allocate();

	BYTE *pData = const_cast<BYTE *>(pBuffer->GetBuffer());

	const size_t datalength = sizeof(KAccountUserLogout) + 1;

	KAccountUserLogout user;

	user.Size      = sizeof(KAccountUserLogout);
	user.Type      = AccountUserLogout;
	user.Version   = ACCOUNT_CURRENT_VERSION;
	user.nExtPoint = nExtPoint;
	user.nGameExtTime = nGameExtTime;

	size_t length  = strlen(pAccountName);
	length = (length > LOGIN_USER_ACCOUNT_MAX_LEN - 1) ? LOGIN_USER_ACCOUNT_MAX_LEN - 1 : length;	// the terminator below lands on Account[length], so the slot has to stay free
	memcpy(user.Account, pAccountName, length);
	user.Account[length] = '\0';
    //开始退出游戏
	*pData = c2s_accountlogout;
	memcpy( pData + 1, &user, sizeof(KAccountUserLogout));
	g_theSmartClient.Send((const void *)pData, datalength);
	//m_pAccountClient->SendPackToServer( ( const void * )pData, datalength );
	SAFE_RELEASE(pBuffer);

	return true;
}

bool CGameServer::_PlayerLogicLogout( const void *pData, size_t datalength )
{
	tagLogicLogout ll;

	if ( sizeof( ll.szRoleName ) < datalength )
	{
		return false;
	}
	//通知这个客户端退出游戏了GS
	ll.cProtocol = s2c_logiclogout;
	memcpy( ll.szRoleName, pData, datalength );

	if (m_pGameSvrServer) m_pGameSvrServer->SendData( m_lnIdentityID, ( const void * )&ll, sizeof( tagLogicLogout ) );

	return true;
}
//同步角色信息
bool CGameServer::_SyncRoleInfo( const void *pData, size_t datalength, DWORD nData,
	                            DWORD nLeftTime,int nVipType,DWORD nExtGameLeftTime,unsigned int nTempEnTerMapIndx,unsigned int nTempKeyId)
{
	/*
	 * Send role data to the game server
	 */
	tagGuidableInfo ginfo;

	ginfo.cProtocol = s2c_syncogjfkdnvhf_roleinfo_cipher;

	GenGuid(&(ginfo.guid));	   //产生网络 唯一标识结构

	ginfo.nExtPoint    = nData;
	ginfo.nChangePoint = 0;
	ginfo.nLeftTime    = nLeftTime;
	ginfo.nVipType     = nVipType;
	ginfo.datalength   = datalength;
	ginfo.nGameTime    = nExtGameLeftTime;
	ginfo.nTempEnTerMapIndx = nTempEnTerMapIndx;
	ginfo.nTempKeyId   = nTempKeyId;
	//大包
	m_thePackager.AddData(s2c_syncogjfkdnvhf_roleinfo_cipher, (const char *)&ginfo, sizeof(tagGuidableInfo));
	m_thePackager.AddData(s2c_syncogjfkdnvhf_roleinfo_cipher, (const char *)pData, datalength);

	/*
	 * Begin to split this buffer	开始分割大包
	 */
	CBuffer *pBuffer = m_thePackager.GetHeadPack(s2c_syncogjfkdnvhf_roleinfo_cipher);

	while (pBuffer)
	{//发往GS 向哪个GS发包
		if (m_pGameSvrServer) m_pGameSvrServer->SendData(m_lnIdentityID, pBuffer->GetBuffer(),pBuffer->GetUsed());

		/*
		 * Your must relase this buffer
		 */
		SAFE_RELEASE(pBuffer);

		/*
		 * Get next package
		 */
		pBuffer = m_thePackager.GetNextPack(s2c_syncogjfkdnvhf_roleinfo_cipher);		
	}
	
	SAFE_RELEASE(pBuffer);

	m_thePackager.DelData(s2c_syncogjfkdnvhf_roleinfo_cipher);

	return true;
}

bool CGameServer::RegisterServer( UINT nID, IGServer *pGServer )
{
	ASSERT(pGServer);

	CCriticalSection::Owner locker(m_csMapIDAction);

	stdMapIDConvert::iterator it;

	if (m_theMapIDConvert.end() != (it = m_theMapIDConvert.find(nID)))
	{//如果存在了 就更新
		stdServerList& sl = (*it).second;

		sl.push_back(pGServer);
	}
	else
	{//否则就全新增加 
		stdServerList sl;

		sl.push_back(pGServer);
		
		std::pair<stdMapIDConvert::iterator, bool> result =m_theMapIDConvert.insert( stdMapIDConvert::value_type(nID,sl));

		return result.second;
	}

	return true;
}
//查找地图
IGServer *CGameServer::QueryServer( UINT nMapID,unsigned int nSerNoIndex)
{
	CCriticalSection::Owner locker(m_csMapIDAction);

	stdMapIDConvert::iterator it;

	if (m_theMapIDConvert.end() != (it = m_theMapIDConvert.find(nMapID)))
	{//如果有这个地图

		stdServerList& sl = (*it).second;
		 //MessageBox(0, "提示A","QueryServerA-BS提示:",MB_OK);

		if (!sl.empty())
		{
			IGServer *pGServer = NULL;
            //MessageBox(0, "提示B","QueryServerA-BS提示:",MB_OK);
			stdServerList::iterator it;
			for (it = sl.begin(); it != sl.end(); ++it)
			{
				pGServer = (IGServer *)(*it);

				ASSERT(pGServer);

				if (NULL==pGServer)
					continue;

				if (nSerNoIndex!=pGServer->GetSerNoIndex()) //如果是不同的gs 就返回空
					continue;
				//如果进入这个gs的人数大于 实际人数就返回空
				//if ( pGServer->GetContent() < pGServer->GetCapability() )  //人数限制
				//{//容量对比？
				  return pGServer;	   
				//}
			}
		}
	}
    //MessageBox(0, "提示D","QueryServerB-BS提示:",MB_OK);
	return NULL;
}


IGServer *CGameServer::GetSerVerIdByIndex(UINT nRegSerIndex,UINT nGsSerNumber)
{
	CCriticalSection::Owner locker( CGameServer::m_csGameSvrAction );

	stdGameSvr::iterator it;

	for ( it = CGameServer::m_theGameServers.begin();it != CGameServer::m_theGameServers.end(); ++it)
	{
		IGServer *pGServer = ( *it ).second;

		if (pGServer && pGServer->GetSerNoIndex()==nRegSerIndex && pGServer->GetSerGsNumber()==nGsSerNumber)
		{
			return pGServer;
		}
	}

	return NULL;
}

IGServer *CGameServer::GetServer( size_t nID )
{
	CCriticalSection::Owner locker( CGameServer::m_csGameSvrAction );
	
	stdGameSvr::iterator it;

	if ( CGameServer::m_theGameServers.end() != 
		(it = CGameServer::m_theGameServers.find(nID)))
	{
		IGServer *pGServer = ( *it ).second;

		ASSERT(pGServer);

		return pGServer;
	}

	return NULL;
}

size_t CGameServer::GetContent()
{
	//CCriticalSection::Owner lock(m_csAITS);
	KAutoCriticalSection_ AutoLock( m_csAITS );

	return m_theAccountInThisServer.size(); //账号再这个服务器 容器中元素个数
}

void CGameServer::SendToAll( const char *pText, int nLength, UINT uOption,BOOL nIsSavePai)
{
	stdGameSvr::iterator it;

	CCriticalSection::Owner locker( CGameServer::m_csGameSvrAction );

	for ( it = CGameServer::m_theGameServers.begin();it != CGameServer::m_theGameServers.end(); ++it)
		{
			IGServer *pGServer = ( *it ).second;

			if (pGServer)
			{
				pGServer->SendText(pText,nLength,uOption,nIsSavePai);
			}
		}
}


void CGameServer::SendData(  const unsigned long ulnClientID, const void * const pData, const size_t datalength )
{
	if (0 == datalength || !m_pGameSvrServer )
		return;
	m_pGameSvrServer->SendData(ulnClientID,pData,datalength);
}

void CGameServer::SendText( const char *pText, int nLength, UINT uOption,BOOL nIsSavePai)
{
	if ( !pText || 0 == nLength || !m_pGameSvrServer )
	{
		return;
	}

	tagGatewayBroadCast gbc;

	gbc.cProtocol = s2c_gateway_broadcast;
	gbc.uCmdType = uOption;
	gbc.iSavePai = nIsSavePai;
	int nLen = 0;
	if ( sizeof( gbc.szData ) > nLength )
	{
		strcpy( gbc.szData, pText );

		nLen = nLength; //数据的长度
	}

	gbc.szData[nLen] = '\0';

	m_pGameSvrServer->SendData( m_lnIdentityID, ( const void * )&gbc, sizeof( tagGatewayBroadCast ) );
}



CGameServer::stdRoleLockMap CGameServer::m_csRoleLock;
CCriticalSection	CGameServer::m_csCR;

//邦定这个角色名为这个gs的连接 是否已经登录这个角色
bool CGameServer::_LockOrUnlockRole( const void *pData, size_t dataLength )	//强行加解锁
{
	CCriticalSection::Owner lock( CGameServer::m_csCR );
	if (pData && dataLength == sizeof(tagRoleEnterGame))
	{
		tagRoleEnterGame* pRole = (tagRoleEnterGame*)pData;
		char szRole[NAME_LENGHT];
		strncpy(szRole, pRole->Name, NAME_LENGHT-1);
		szRole[NAME_LENGHT-1] = 0;
		bool bLock = pRole->bLock;

		if (szRole && szRole[0] != 0)
		{
			if (bLock)  //锁定
			{
				m_csRoleLock[szRole].m_idx = m_lnIdentityID;
				m_csRoleLock[szRole].m_GsNumBer =pRole->nGsNumber;
				m_csRoleLock[szRole].m_IsOffLine =pRole->Lixian;
				m_csRoleLock[szRole].m_RegServerIndex =pRole->nRegServerIndex;
			}
			else
			{//解锁
				stdRoleLockMap::iterator it = m_csRoleLock.find(szRole);
				if (it != m_csRoleLock.end())
					m_csRoleLock.erase(it);
			}
		}
	}
	return true;
}

//是否已经连接
int  CGameServer::getRoleLock(char* szRole,RoleLoginInfo *resInfo)
{
	CCriticalSection::Owner lock( CGameServer::m_csCR );
	if (szRole && szRole[0] != 0)
	{
		stdRoleLockMap::iterator it = m_csRoleLock.find(szRole);
		if (it != m_csRoleLock.end() && it->second.m_idx!= -1)
		{//如果存在　但是是离线的
			if  (resInfo)
			{
				resInfo->m_GsNumBer = it->second.m_GsNumBer;
				resInfo->m_idx = it->second.m_idx;
				resInfo->m_IsOffLine = it->second.m_IsOffLine;
				resInfo->m_RegServerIndex = it->second.m_RegServerIndex;
			}
			return true;
		}
	}
	return false;
}

//是否已经连接
int  CGameServer::IsRoleLock(char* szRole,RoleLoginInfo *resInfo)
{
	CCriticalSection::Owner lock( CGameServer::m_csCR );
	if (szRole && szRole[0] != 0)
	{
		stdRoleLockMap::iterator it = m_csRoleLock.find(szRole);
		if (it != m_csRoleLock.end() && it->second.m_idx!= -1)
		{//如果存在　但是是离线的
			if  (it->second.m_IsOffLine)
			{
				if  (resInfo)
				{
					resInfo->m_GsNumBer = it->second.m_GsNumBer;
					resInfo->m_idx = it->second.m_idx;
					resInfo->m_IsOffLine = it->second.m_IsOffLine;
					resInfo->m_RegServerIndex = it->second.m_RegServerIndex;
				}
				return 2;
			}
			else
				return 1;
		}
	}
	return 0;
}
//是否已经是这个连接
bool CGameServer::IsRoleLockBySelf(char* szRole)
{
	CCriticalSection::Owner lock(CGameServer::m_csCR);
	if (szRole && szRole[0] != 0)
	{
		stdRoleLockMap::iterator it = m_csRoleLock.find(szRole);
		if (it != m_csRoleLock.end()/* && it->second == m_nIndentity*/)
			return true;
	}
	return false;
}
//删除这个连接
bool CGameServer::UnlockRoleSelf(char* szRole)
{
	CCriticalSection::Owner lock(CGameServer::m_csCR);
	if (szRole && szRole[0] != 0)
	{
		stdRoleLockMap::iterator it = m_csRoleLock.find(szRole);
		if (it != m_csRoleLock.end() /*&& it->second == m_nIndentity*/)
		{
			m_csRoleLock.erase(it);
			return true;
		}
	}
	return false;
}
//删除全部角色连接
void CGameServer::UnlockAllRole(size_t ID)
{
	CCriticalSection::Owner lock( CGameServer::m_csCR );
	stdRoleLockMap::iterator it = m_csRoleLock.begin();
	while (it != m_csRoleLock.end())
	{
		if (it->second.m_idx == ID)
			it->second.m_idx= -1;
		it++;
	}
}

void AddOutputSerPlayer(HWND hListCtrl, char* aStr)
{//添加操作输出文字
	if (hListCtrl && ::IsWindow(hListCtrl))
	{
		int nCount = ::SendMessage(hListCtrl,LB_GETCOUNT,0,0); //获取信息的数量
		if(nCount >= 100)
		{
			::SendMessage(hListCtrl,LB_DELETESTRING,100,0);  //删除信息
		}
		int nIndex = ::SendMessage( hListCtrl, LB_INSERTSTRING, 0, ( LPARAM )aStr );//把新的信息添到第一个
	}
}

void AddOutputSerIpList(HWND hListCtrl, char* aStr)
{//添加操作输出文字
	if ( hListCtrl && ::IsWindow(hListCtrl))
	{
		int nCount = ::SendMessage(hListCtrl,LB_GETCOUNT,0,0); //获取信息的数量
		if(nCount >= 100)
		{
			::SendMessage(hListCtrl,LB_DELETESTRING,100,0);  //删除信息
		}
		int nIndex = ::SendMessage(hListCtrl,LB_INSERTSTRING,0,(LPARAM)aStr);//把新的信息添到第一个
	}
}

#include "Stdafx.h"


#include "GamePlayer.h"
#include "resource.h"
#include "AccountLoginDef.h"
#include "LoginDef.h"
#include "BishopPacketGuard.h"
#include "S3DBInterface.h"
#include "..\..\Engine\Src\KEngine.h"

#include "KProtocol.h"
#include "KProtocolDef.h"

#include "Utils.h"
#include "Macro.h"
#include "Exception.h"
#include "Buffer.h"

#include "SmartClient.h"

//#include "PassChecksum.h"
#define  GFDAGFDG "b0hAg13ZkuJmzHbzZA6OtCAvJEqlnqpB"
#define  TRGFDGGG "nGvXbbA43FkvGpl4rVWwdd0vpfW0A32s"
//----------------------	 
extern HWND hbishopOutput;
extern HWND hMianHwd;
extern HWND hAccCheckList;

extern HWND hAccCheckList_VIP;
extern HWND hIpCheckList;
extern HWND hYuJianCheckList;
extern HWND hClientIpList;
extern HWND hForBitOpenIpList;
extern HWND hHaveConnetIpList;
extern HWND hDugList;

CLogFile  bs_LogFile;

#include "Inifile.h"
/*
using CIniFile;
using GetAppFullPath;
//CBishopApp		CGamePlayer::m_theGetMain;
//----------------------
using Output;
using CException;
using CCriticalSection;
using ToString;
using Trace;
using _tstring;
using CBuffer;
using CPackager;
using net_aton;
*/
CBuffer::Allocator	CGamePlayer::m_theGlobalAllocator(1024*128,1000);	 //128 1000

LONG				CGamePlayer::m_slnIdentityCounts = 0;
LONG				CGamePlayer::m_lnWorkingCounts = 0;

const int			CGamePlayer::s_nRoleListCount	 = 3;
const int			CGamePlayer::s_nLoginTimeoutTimer	 = 60 * 1000;
const int			CGamePlayer::s_nProcessTimeoutTimer	 = 200 * 1000;

CCriticalSection		CGamePlayer::m_csMapSP;
CGamePlayer::stdMapSP	CGamePlayer::m_sthePlayerTable;

CPlayerCreator			CGamePlayer::m_thePlayerCreator;

//IClient * CGamePlayer::m_pAccSvrClient = NULL;
IServer * CGamePlayer::m_pPlayerServer = NULL;
IClient * CGamePlayer::m_pDBRoleClient = NULL;

/*
 * CGamePlayer Global Function
 */
bool CGamePlayer::SetupGlobalAllocator( size_t bufferSize, size_t maxFreeBuffers )
{
	return CGamePlayer::m_theGlobalAllocator.ReSet( bufferSize, maxFreeBuffers );
}

/*
 * CGamePlayer::CTask
 */
CGamePlayer::CTask::CTask()
{
	ASSERT( FALSE );
}

CGamePlayer::CTask::~CTask()
{
	//CCriticalSection::Owner	lock( m_csTask );

	stdVector::iterator theIterator;

	for ( theIterator = m_stdCommand.begin(); theIterator != m_stdCommand.end(); ++theIterator )
	{
		ICommand *pCmd = reinterpret_cast< ICommand * >( *theIterator );

		SAFE_DELETE( pCmd );
	}

	m_stdCommand.clear();
}

CGamePlayer::CTask::CTask( CGamePlayer *pReceiver, UINT nTaskID )
			: m_pReceiver( pReceiver )
			, m_indexCmd( 0 )
			, m_nTaskProgID( nTaskID )
{
}

size_t CGamePlayer::CTask::AddCmd( Action pFun )
{
	//CCriticalSection::Owner	lock( m_csTask );

	/*
	 * Convert a status to the other status
	 */

	/*
	 * Generate a command and push it into the task queue
	 */
	ICommand *pCmd = new CTaskCommand< CGamePlayer >( m_pReceiver, pFun );

	m_stdCommand.push_back( pCmd );
	
	size_t id = m_stdCommand.size();

	return id;
}

UINT CGamePlayer::CTask::Execute()
{
	//CCriticalSection::Owner	lock( m_csTask );

	if ( m_indexCmd < m_stdCommand.size() )
	{
		ICommand *pCmd = m_stdCommand[ m_indexCmd ];

		ASSERT( pCmd );

		UINT nResult = pCmd->Execute();

		switch ( nResult )
		{
		case enumSelAddDelRole:
		case enumLoginCreateRole:
		case enumLoginDeleteRole:
		case enumLoginSelectRole:
		case enumCompleted:
			Reset();

		case enumError:
		case enumNone:

			return nResult;
			break;

		case enumRepeat:

			return m_nTaskProgID;
			break;

		case enumToNextTask:
		default:
			break;
		}

		m_indexCmd ++;
		return m_nTaskProgID;
	}	

	Reset();	
	return enumCompleted;
}

/*
 * CGamePlayer class
 */
CGamePlayer::CDataQueue::CDataQueue( size_t bufferSize /*= 1024 * 64*/, size_t maxFreeBuffers /*= 1*/ )
				: m_theDQAllocator( bufferSize, maxFreeBuffers )
{

}

CGamePlayer::CDataQueue::~CDataQueue()
{
	Empty();
}

void CGamePlayer::CDataQueue::Empty()
{
	CCriticalSection::Owner locker( m_csQueue );
	
	stdDataMap::iterator it;
	for (it = m_theData.begin(); it != m_theData.end(); ++it)
	{
		LONG id = ( *it ).first;
		
		CBuffer *pBuffer = (*it).second;

		SAFE_RELEASE(pBuffer);
	}

	m_theData.erase(m_theData.begin(), m_theData.end());
}

bool CGamePlayer::CDataQueue::AddData( LONG lnID, const BYTE *pData, size_t datalength )
{
	bool ok = false;

	ASSERT(pData && datalength);

	CBuffer *pBuffer = m_theDQAllocator.Allocate();

	ASSERT(pBuffer);

	pBuffer->AddData(pData, datalength);

	{
		CCriticalSection::Owner locker(m_csQueue);

		pBuffer->AddRef();

		stdDataMap::iterator it;
		if (m_theData.end() != (it = m_theData.find(lnID)))
		{
			CBuffer *pTemp = (*it).second;
			SAFE_RELEASE(pTemp);
			m_theData.erase(it);
		}

		std::pair< stdDataMap::iterator, bool > result = m_theData.insert(stdDataMap::value_type(lnID,pBuffer));
		if (!(ok =result.second))
		{
			SAFE_RELEASE(pBuffer);
		}
	}

	SAFE_RELEASE(pBuffer);

	return ok;
}
//获取某个连接的数据包
CBuffer *CGamePlayer::CDataQueue::Attach(LONG lnID)
{
	CCriticalSection::Owner locker(m_csQueue);

	stdDataMap::iterator it;

	if (m_theData.end() != (it = m_theData.find(lnID)))
	{
		CBuffer *pBuffer = (*it).second;
		m_theData.erase(it);
		return pBuffer;
	}

	return NULL;
}

void CGamePlayer::CDataQueue::Detach(LONG lnID)
{
	//Clear(lnID);
}

void CGamePlayer::CDataQueue::Clear(LONG lnID)
{
	CCriticalSection::Owner locker(m_csQueue);
	
	stdDataMap::iterator it;
	if (m_theData.end() !=(it = m_theData.find(lnID)))
	{
		CBuffer *pBuffer = (*it).second;
		SAFE_RELEASE(pBuffer);
		m_theData.erase(it);
	}
}

/*
 * CGamePlayer class
 */
CGamePlayer::CGamePlayer(UINT nIdentityID /*  = ( UINT )( -1 ) */)
				: m_lnIdentityID(nIdentityID)
				, m_theLoginTask(this, enumLogin)
				, m_theSelAddDelTask(this, enumSelAddDelRole)
				, m_theLoginCreateRoleTask(this, enumLoginCreateRole)
				, m_theLoginSelectRoleTask(this, enumLoginSelectRole)
				, m_theLoginDeleteRoleTask(this, enumLoginDeleteRole)
				, m_theSafeCloseTask(this, enumSafeClose)
				, m_nCurrentTaskID(0)
				, m_nAttachServerID(-1)
				, m_bActiveStatus(false)
				, m_dwTaskBeginTimer(0)
				, m_dwTaskTotalTimer(0)
				, m_bAutoUnlockAccount(false)
                , m_bUseSuperPassword(false)
				, m_nExtPoint(0)
				, m_nVipType(0)
				, m_nLeftTime(0)
				, m_nTwoRevivePos(53)
				, m_nBishopVer(0)
				, m_nForBitOpenNum(0)
				, m_nExtGameLeftTime(0)
				, m_nForBitOpenSer(0)
				,__m_nIsOpenYuJian(false)
				,m_enterMapIdx(0)
				,m_selServer(1)
				//, hCtrlPlayr(0)
{
	SetCurrentTask(enumNone);

	LONG lnID = ::InterlockedExchangeAdd(&m_slnIdentityCounts,1);

	m_lnIdentityID = ((UINT)(-1) == m_lnIdentityID)?lnID:m_lnIdentityID;

	InitTaskProcessor();
	//SetPlayerRevivePos();

	//hWndPlayr   = ::FindWindow(NULL,"Bishop - [enable]");
   	//hCtrlPlayr  = ::GetDlgItem(hWndPlayr, IDC_LIST_GAMESERVER2 );
}

CGamePlayer::~CGamePlayer()
{
	/*{
		CCriticalSection::Owner locker( CGamePlayer::m_csMapSP );

		m_sthePlayerTable.erase(m_sthePlayerTable.begin(),m_sthePlayerTable.end());
	}*/

	::InterlockedExchangeAdd(&m_slnIdentityCounts, -1);
}

bool CGamePlayer::Active()
{
	SetCurrentTask(enumNone);

	m_nAttachServerID = -1;

	m_bActiveStatus = true;

	m_dwTaskBeginTimer = ::GetTickCount();
	m_dwTaskTotalTimer = s_nLoginTimeoutTimer;

	::InterlockedExchangeAdd(&m_lnWorkingCounts,1);
	if (hbishopOutput)
	{
		char msg[128]={0};
		//sprintf(msg,"--玩家网络号:%u 连接成功--",m_lnIdentityID);
		sprintf(msg,"--Player network number: %u successfully connected--",m_lnIdentityID);
		AddOutputStringPlayer(hbishopOutput,msg);
	}
	return true;
}

//再选择角色界面没有进到游戏的时退出游戏调用
bool CGamePlayer::Inactive()
{
	m_bActiveStatus = false;

	SetCurrentTask(enumNone);
	Del(m_sRoleName.c_str());	
	//DelByDwid(m_RoleNameDwID);

	m_dwTaskBeginTimer = 0;
	m_dwTaskTotalTimer = s_nLoginTimeoutTimer;

	if (m_bAutoUnlockAccount)
	{
		_UnlockAccount();

		m_bAutoUnlockAccount = false;
	}

	_ClearTaskQueue();
///////////////////////////////////////////////
/*	IGServer *pGServer = CGameServer::GetServer( m_nAttachServerID );

	if ( pGServer )
	{
		pGServer->DispatchTask( CGameServer::enumPlayerLogicLogout, m_sRoleName.c_str(), m_sRoleName.size() );
	}/*
/////////////////////////////////////////
	m_nAttachServerID = -1;
	
	/*
	* Clear this role info
	*/
	m_sAccountName	= "";
	m_sPassword		= "";
    m_sSuperPassword = ""; ////二级密码
    m_sDelRoleName   = "";
    m_bUseSuperPassword = false;  //二级密码

	m_sRoleName		= "";
	m_RoleNameDwID  = 0;
	m_selServer     = 1;
    m_enterMapIdx   = 0;

	::InterlockedExchangeAdd(&m_lnWorkingCounts,-1);
	char msg[128]={0};
	//sprintf(msg,"--玩家网络号:%u 退出成功--",m_lnIdentityID);
	sprintf(msg,"--Player network number: %u Exit successfully--",m_lnIdentityID);
	AddOutputStringPlayer(hbishopOutput,msg);
	return true;
}

UINT CGamePlayer::SafeClose()
{
	ASSERT( FALSE );
	
	return enumToNextTask;
}
//通知退出游戏
bool CGamePlayer::_UnlockAccount()
{
	const char *pAccountName = m_sAccountName.c_str();

	if ( !pAccountName || !pAccountName[0] )
	{
		return false;
	}

	if (NAME_LENGHT <= strlen(pAccountName))
	{
		return false;
	}
	/*
	char Address[128]={0}; //IP地址
	const char* pInfo=m_pPlayerServer->GetClientInfo(m_lnIdentityID);
	if (pInfo)
	{
		DWORD m_IP = _a2ip(pInfo);
		const unsigned char* pIpAddress=	(const unsigned char*)&m_IP;  
		sprintf(Address, "%d.%d.%d.%d", pIpAddress[0],pIpAddress[1],pIpAddress[2],pIpAddress[3]);
		//const char* pColon = strchr(pInfo, ':');
		//if (pColon)
		//m_port = atoi(pColon + 1);  //加为了去除冒号  //端口
	}
	*/

	CBuffer *pBuffer = m_theGlobalAllocator.Allocate();

	BYTE *pData = const_cast< BYTE * >( pBuffer->GetBuffer() );

	const size_t datalength = sizeof( KAccountUserLogout ) + 1;

	KAccountUserLogout user;

	user.Size         = sizeof( KAccountUserLogout );
	user.Type         = AccountUserLogout;
	user.Version      = ACCOUNT_CURRENT_VERSION;
	user.nExtPoint    = 0;
	user.nGameExtTime = 0;

	size_t length = strlen( pAccountName );
	length = ( length > LOGIN_USER_ACCOUNT_MAX_LEN - 1 ) ? LOGIN_USER_ACCOUNT_MAX_LEN - 1 : length;	// the terminator below lands on Account[length], so the slot has to stay free
	memcpy( user.Account, pAccountName, length );
	user.Account[length] = '\0';

	*pData = c2s_accountlogout;
	memcpy( pData + 1, &user, sizeof( KAccountUserLogout ) );

	g_theSmartClient.Send( ( const void * )pData, datalength );
//	m_pAccSvrClient->SendPackToServer( ( const void * )pData, datalength );

	SAFE_RELEASE( pBuffer );

	return true;	
}

void CGamePlayer::ATTACH_NETWORK( IClient *pAccSvrClient, 
				IServer *pPlayerServer, 
				IClient	*pDBRoleClient )
{
	ASSERT(m_slnIdentityCounts==0);

//	m_pAccSvrClient = pAccSvrClient;
	m_pPlayerServer = pPlayerServer;
	m_pDBRoleClient = pDBRoleClient;
}

void CGamePlayer::DETACH_NETWORK()
{
	ASSERT(m_slnIdentityCounts==0);

//	SAFE_RELEASE( m_pAccSvrClient );
	SAFE_RELEASE(m_pPlayerServer);
	SAFE_RELEASE(m_pDBRoleClient);
}

bool CGamePlayer::DispatchTask( UINT nTaskID )
{
	/*
	 * This player is processing a special tasks
	 */	
/*
	if ( IsWorking() )
	{
		return false;
	}
*/
	m_theLoginTask.Reset();

	m_theSelAddDelTask.Reset();

	m_theLoginCreateRoleTask.Reset();
	m_theLoginSelectRoleTask.Reset();
	m_theLoginDeleteRoleTask.Reset();

	SetCurrentTask( nTaskID );

	return true;	
}

bool CGamePlayer::IsWorking()
{
	return ( GetCurrentTask() != enumNone );
}

bool CGamePlayer::Run()
{
	LONG lnNextTask = enumNone;

	if (m_bActiveStatus && m_dwTaskBeginTimer)
	{
		DWORD dwCurTimer = ::GetTickCount();

		if (dwCurTimer - m_dwTaskBeginTimer > m_dwTaskTotalTimer)
		{
			m_pPlayerServer->ShutdownClient(m_lnIdentityID); //关闭线程
			char msg[128]={0};
			//sprintf(msg,"--玩家网络号:%u 登录超时,关闭连接--",m_lnIdentityID);
			sprintf(msg,"--Player network number: %u Login timeout, close connection--",m_lnIdentityID);
			AddOutputStringPlayer(hbishopOutput,msg);
			return false;
		}
	}

	switch ( GetCurrentTask() )
	{
	case enumNone:
		return true;
		break;

	case enumLogin:
		lnNextTask = m_theLoginTask.Execute();
		SetCurrentTask( lnNextTask );
		break;

	case enumSelAddDelRole:
		lnNextTask = m_theSelAddDelTask.Execute();
		SetCurrentTask( lnNextTask );
		break;

	case enumLoginCreateRole:
		lnNextTask = m_theLoginCreateRoleTask.Execute();
		SetCurrentTask( lnNextTask );
		break;

	case enumLoginSelectRole:
		lnNextTask = m_theLoginSelectRoleTask.Execute();
		SetCurrentTask( lnNextTask );
		break;

	case enumLoginDeleteRole:
		lnNextTask = m_theLoginDeleteRoleTask.Execute();
		SetCurrentTask( lnNextTask );
		break;

	case enumSafeClose:
		lnNextTask = m_theSafeCloseTask.Execute();
		SetCurrentTask( lnNextTask );
		break;

	case enumCompleted:
		lnNextTask = TaskCompleted() ? enumNone : enumCompleted;
		SetCurrentTask( lnNextTask );
		break;

	case enumError:
		SetCurrentTask( enumNone );
		break;

	default:
		break;
	}	

	return true;
}

bool CGamePlayer::AppendData(UINT nOwner, const void *pData, size_t dataLength)
{
	if (!BishopPacketGuard::HasBytes(pData, dataLength, 1) ||
	    nOwner >= enumOwnerTotal || !m_bActiveStatus )
	{
		return false;
	}

	BYTE cProtocol = CPackager::Peek(pData);
	
	if ( cProtocol < g_nGlobalProtocolType)
	{
		return LargePackProcess(nOwner,pData,dataLength);
	}
	else if (cProtocol > g_nGlobalProtocolType)
	{
		return SmallPackProcess(nOwner,pData,dataLength);
	}

	return true;
}

bool CGamePlayer::SmallPackProcess( UINT nOwner, const void *pData, size_t dataLength )
{
	switch ( nOwner )
	{
	case enumOwnerAccSvr:

		return DispatchTaskForAccount( pData, dataLength );
		break;
		
	case enumOwnerRoleSvr:
		
		return DispatchTaskForDBRole( pData, dataLength );
		break;
		
	case enumOwnerPlayer:
		
		return DispatchTaskForPlayer( pData, dataLength );
		break;

	default:
		break;
	}

	return false;
}

bool CGamePlayer::LargePackProcess(UINT nOwner, const void *pData, size_t dataLength)
{
	switch (nOwner)
	{
	case enumOwnerAccSvr:
		{
		   ASSERT(FALSE);
	    }
		break;
	case enumOwnerRoleSvr:
		{	
			bool ok = true;
			CBuffer *pBuffer = m_thePackager.PackUp(pData, dataLength);
			
			if (pBuffer)
			{
				ok = DispatchTaskForDBRole(pBuffer->GetBuffer(),pBuffer->GetUsed());
				
				SAFE_RELEASE(pBuffer);
			}
			
			return ok;
		}
		break;
	case enumOwnerPlayer:
		{
		  ASSERT(FALSE);
		}
		break;

	default:
		break;
	}

	return false;
}

bool CGamePlayer::DispatchTaskForAccount(const void *pData, size_t dataLength)
{
	if (NULL == pData || 0 == dataLength)
	{
		return false;
	}

	BYTE cProtocol = CPackager::Peek(pData);

	m_theDataQueue[enumOwnerAccSvr].AddData(cProtocol,(const BYTE *)pData,dataLength);

	return true;
}

bool CGamePlayer::DispatchTaskForDBRole(const void *pData, size_t dataLength)
{
	if ( NULL == pData || 0 == dataLength)
	{
		return false;
	}

	BYTE cProtocol = CPackager::Peek(pData);

	m_theDataQueue[enumOwnerRoleSvr].AddData(cProtocol,( const BYTE *)pData,dataLength);

	return true;
}

bool CGamePlayer::DispatchTaskForPlayer(const void *pData, size_t dataLength)
{
	if (NULL == pData || 0 == dataLength)
	{
		return false;
	}

	BYTE cProtocol = CPackager::Peek(pData);

	m_theDataQueue[enumOwnerPlayer].AddData(cProtocol,(const BYTE *)pData,dataLength);

	return true;
}

void CGamePlayer::_ClearTaskQueue()
{
	for ( int i=0; i<enumOwnerTotal; ++i )
	{
		m_theDataQueue[i].Empty();
	}
	
	m_thePackager.Empty();
}
//初始化任务线程
void CGamePlayer::InitTaskProcessor()
{
	/*
	 * Login main task
	 */
	m_theLoginTask.AddCmd(&CGamePlayer::WaitForAccPwd);
	m_theLoginTask.AddCmd(&CGamePlayer::QueryAccPwd);
	m_theLoginTask.AddCmd(&CGamePlayer::VerifyAccount);
	m_theLoginTask.AddCmd(&CGamePlayer::QueryRoleList);
	m_theLoginTask.AddCmd(&CGamePlayer::ProcessRoleList);

	/*
	 * Login branch task
	 */
	{
		/*
		 * m_theSelAddDelTask::SelAddDelRole
		 *
		 * switch( result )
		 * case m_theLoginCreateRoleTask
		 * case m_theLoginDeleteRoleTask
		 * case m_theLoginSelectRoleTask
		 */
		m_theSelAddDelTask.AddCmd(&CGamePlayer::SelAddDelRole);
		m_theSelAddDelTask.AddCmd(&CGamePlayer::QueryAccPwd);	  //检测二级密码
		m_theSelAddDelTask.AddCmd(&CGamePlayer::DelRole_WaitForVerify);

		/*
		 * m_theLoginCreateRoleTask::WaitForCreateResult
		 *
		 * successed : m_theLoginCreateRoleTask::ProcessRoleInfo
		 * failed	 : m_theSelAddDelTask::SelAddDelRole
		 */
		m_theLoginCreateRoleTask.AddCmd(&CGamePlayer::WaitForCreateResult); 
		m_theLoginCreateRoleTask.AddCmd(&CGamePlayer::ProcessRoleInfo);
		m_theLoginCreateRoleTask.AddCmd(&CGamePlayer::WaitForGameSvrPermit);

		m_theLoginDeleteRoleTask.AddCmd(&CGamePlayer::WaitForDeleteResult);

		m_theLoginSelectRoleTask.AddCmd(&CGamePlayer::ProcessRoleInfo);
		m_theLoginSelectRoleTask.AddCmd(&CGamePlayer::WaitForGameSvrPermit);
	}

	m_theSafeCloseTask.AddCmd(&CGamePlayer::SafeClose);

	/*
	 * Logout
	 */
    m_nVipType  = 0;
	m_nExtPoint = 0;
	m_nLeftTime = 0;
	m_nExtGameLeftTime = 0;
	//hWndPlayr=NULL; 
	//hCtrlPlayr=NULL;
    //SetPlayerRevivePos();
	/*/-------------------------------------------------------
	CIniFile theConfigFile;
	
	_tstring sAppPath = GetAppFullPath( NULL );
	
	_tstring sConfigFile;
	
	sConfigFile = sAppPath + "Bishop.cfg";
	
	theConfigFile.SetFile( sConfigFile.c_str() );
	
	//m_sRoleSvrIP.resize( MAX_STRING_LEN );
	m_nTwoRevivePos = theConfigFile.ReadInteger( "Network", "TwoRevivePos", 0 );
	*///-------------------------------------------------------
}

bool CGamePlayer::TaskCompleted()
{
	Trace( ToString( m_lnIdentityID ), "CGamePlayer::TaskCompleted" );

	/*
	 * Clear some data
	 */
	
	return true;
}

UINT CGamePlayer::WaitForAccPwd()
{
#ifdef	CONSOLE_DEBUG
	cprintf( "CGamePlayer::WaitForAccPwd...\n" );
	OutputDebugString("CGamePlayer::WaitForAccPwd...\n");
#endif

	CBuffer *pRetBuffer = m_theDataQueue[enumOwnerPlayer].Attach(c2s_login_fhgyrinhkmvnsheypo);
	 
	if (pRetBuffer)
	{
        if (!BishopPacketGuard::HasBytes(pRetBuffer->GetBuffer(),
            pRetBuffer->GetUsed(), 1 + sizeof(KLoginAccountInfo)))
        {
            SAFE_RELEASE(pRetBuffer);
            return enumError;
        }
		KLoginAccountInfo* pLAI = ( KLoginAccountInfo * )(pRetBuffer->GetBuffer() + 1/* size of a protocol byte */);

		UINT nNextTask = enumError;

		// the account and password arrive as fixed arrays with no promised NUL
		char szAccName[sizeof( pLAI->Account ) + 1];
		char szAccPwd[sizeof( pLAI->Password.szPassword ) + 1];
		ZeroMemory( szAccName, sizeof( szAccName ) );
		ZeroMemory( szAccPwd, sizeof( szAccPwd ) );
		memcpy( szAccName, pLAI->Account, sizeof( pLAI->Account ) );
		memcpy( szAccPwd, pLAI->Password.szPassword, sizeof( pLAI->Password.szPassword ) );
		const char *pAccName	= szAccName;
		const char *pAccPwd		= szAccPwd;

#ifdef	CONSOLE_DEBUG
	cprintf( "CGamePlayer::WaitForAccPwd [AccountName:%s]\n", pAccName );
		char msg[512]={0};
	sprintf(msg,"CGamePlayer::WaitForAccPwd [AccountName:%s]\n", pAccName );
	OutputDebugString(msg);
#endif
            
        // modify by Freeway Chen in 2003.7.1
        if (pLAI)
        {
            int nCheckProtocolVersion = true;

            #ifdef USE_KPROTOCOL_VERSION
             nCheckProtocolVersion = (pLAI->ProtocolVersion == KPROTOCOL_VERSION);
            #endif

            if (nCheckProtocolVersion)
            {
                // 如果协议版本相同，继续判断账号
                if (pAccName[0]) 
		        {
			        nNextTask = enumToNextTask;
			        
			        m_sAccountName	= pAccName;
			        m_sPassword		= pAccPwd ? pAccPwd : "";
                    m_bUseSuperPassword = false;
					m_selServer     = pLAI->nSelSererIdx;
					m_enterMapIdx   = pLAI->nEnterMapIdx;
		        }
            }
            else
            {
                // 如果协议版本不同，就提示用户出错，需要升级到新版本
      		    UINT nQueryResult = LOGIN_A_LOGIN | LOGIN_R_INVALID_PROTOCOLVERSION;

                _VerifyAccount_ToPlayer(nQueryResult, 0);

                // 是否需要玩家断开? 需要进一步确认
    			//m_pPlayerServer->ShutdownClient( m_lnIdentityID ); // 断开玩家
            }
        }
		SAFE_RELEASE(pRetBuffer);
		m_theDataQueue[enumOwnerPlayer].Detach(c2s_login_fhgyrinhkmvnsheypo);

		return nNextTask;
	}

	return enumRepeat;
}

UINT CGamePlayer::QueryAccPwd()
{
#ifdef	CONSOLE_DEBUG
	cprintf( "CGamePlayer::QueryAccPwd...\n" );
	OutputDebugString("CGamePlayer::QueryAccPwd...\n" );
#endif

/*	if ( !m_pAccSvrClient )
	{
		return enumError;
	}
*/
	m_dwTaskTotalTimer = s_nProcessTimeoutTimer;

	CBuffer *pBuffer = m_theGlobalAllocator.Allocate();

	BYTE *pData = const_cast< BYTE * >( pBuffer->GetBuffer() );

	const size_t datalength = sizeof( KAccountUserLoginInfo ) + 1;

	KAccountUserLoginInfo userlogin;
	userlogin.Size = sizeof( KAccountUserLoginInfo );

    if (m_bUseSuperPassword)  //检测二级密码
    {
    	userlogin.Type = AccountUserVerify;
    }
    else
    {
    	userlogin.Type = AccountUserLoginInfo;
    }

	userlogin.Version = ACCOUNT_CURRENT_VERSION;
	userlogin.Operate = m_lnIdentityID;

	int nMinLen = sizeof( userlogin.Account ) - 1;
	nMinLen = m_sAccountName.size() > nMinLen ? nMinLen: m_sAccountName.size();
	memcpy( userlogin.Account, m_sAccountName.c_str(), nMinLen );
	userlogin.Account[nMinLen] = '\0';

    if (m_bUseSuperPassword)
    {//检测二级密码
	    nMinLen = sizeof( userlogin.Password ) - 1;
	    nMinLen = m_sSuperPassword.size() > nMinLen ? nMinLen: m_sSuperPassword.size();	
	    memcpy( userlogin.Password, m_sSuperPassword.c_str(), nMinLen );
	    userlogin.Password[nMinLen] = '\0';
    }
    else
    {
	    nMinLen = sizeof( userlogin.Password ) - 1;
	    nMinLen = m_sPassword.size() > nMinLen ? nMinLen: m_sPassword.size();	
	    memcpy( userlogin.Password, m_sPassword.c_str(), nMinLen );
	    userlogin.Password[nMinLen] = '\0';
    }

	*pData = c2s_accountlogin;
	memcpy( pData + 1, &userlogin, sizeof( KAccountUserLoginInfo ) );

	m_bAutoUnlockAccount = true;
	m_theDataQueue[enumOwnerAccSvr].Empty();

	g_theSmartClient.Send( ( const void * )pData, datalength );
//	m_pAccSvrClient->SendPackToServer( ( const void * )pData, datalength );

	SAFE_RELEASE( pBuffer );

	return enumToNextTask;
}


//告诉pays 退出游戏
bool CGamePlayer::FreezeMoney( const char *pAccountName, LONG nExtPoint,LONG nGameExtTime)
{
	if ( !pAccountName || !pAccountName[0] )
	{
		return false;
	}
	
	if (NAME_LENGHT <= strlen( pAccountName ))
	{
		return false;
	}

	/*char Address[128]={0},nIpInfo[128]={0}; //IP地址
	//const char* pInfo=
		m_pPlayerServer->GetClientInfo(m_lnIdentityID,nIpInfo);
	if (nIpInfo[0])
	{
		DWORD m_IP = _a2ip(nIpInfo);
		const unsigned char* pIpAddress=	(const unsigned char*)&m_IP;  
		sprintf(Address, "%d.%d.%d.%d", pIpAddress[0],pIpAddress[1],pIpAddress[2],pIpAddress[3]);
		//const char* pColon = strchr(pInfo, ':');
		//if (pColon)
		//m_port = atoi(pColon + 1);  //加为了去除冒号  //端口
	}*/
	
	
	CBuffer *pBuffer = m_theGlobalAllocator.Allocate();
	
	BYTE *pData = const_cast< BYTE * >(pBuffer->GetBuffer());
	
	const size_t datalength = sizeof(KAccountUserLogout ) + 1;
	
	KAccountUserLogout user;
	
	user.Size      = sizeof(KAccountUserLogout);
	user.Type      = AccountUserLogout;
	user.Version   = ACCOUNT_CURRENT_VERSION;
	user.nExtPoint = nExtPoint;
	user.nGameExtTime = nGameExtTime;
	
	size_t length  = strlen( pAccountName );
	length = ( length > LOGIN_USER_ACCOUNT_MAX_LEN - 1 ) ? LOGIN_USER_ACCOUNT_MAX_LEN - 1 : length;	// the terminator below lands on Account[length], so the slot has to stay free
	memcpy( user.Account, pAccountName, length );
	user.Account[length] = '\0';
    //开始退出游戏
	*pData = c2s_accountlogout;
	memcpy( pData + 1, &user, sizeof( KAccountUserLogout ) );	
	g_theSmartClient.Send( ( const void * )pData, datalength );
	//m_pAccountClient->SendPackToServer( ( const void * )pData, datalength );	
	SAFE_RELEASE(pBuffer);	
	return true;
}

//接受从paysys 发送过来的检查账号的结果
UINT CGamePlayer::VerifyAccount()
{
#ifdef	CONSOLE_DEBUG
	cprintf( "CGamePlayer::VerifyAccount...\n" );
	OutputDebugString("CGamePlayer::VerifyAccount...\n" );
#endif
	
	m_nExtPoint = 0;
	m_nVipType  = 0;
	m_nLeftTime = 0;
	m_nExtGameLeftTime = 0;
    //接受从paysysy 发送过来的协议数据包
	CBuffer *pRetBuffer = m_theDataQueue[enumOwnerAccSvr].Attach(s2c_accountfdfsglogin);

	if (pRetBuffer)
	{
        if (!BishopPacketGuard::HasBytes(pRetBuffer->GetBuffer(),
            pRetBuffer->GetUsed(), 1 + sizeof(KAccountUserReturnExt)))
        {
            SAFE_RELEASE(pRetBuffer);
            return enumError;
        }
		KAccountUserReturnExt* pReturn = ( KAccountUserReturnExt * )( pRetBuffer->GetBuffer() + 1/* size of a protocol byte */ );

		int nResult = pReturn->nReturn;
		UINT nQueryResult = LOGIN_A_LOGIN;
		UINT nNextTask = enumError;

		char nAccount[ACC_LENGTH];
		// the same min the ternary computed, spelled so that the bound sits on the
		// name the copy and the terminator both read: the ternary's other arm is a
		// length() call no read of this line can size
		size_t used = m_sAccountName.length();
		if (used >= sizeof(nAccount))
			used = sizeof(nAccount) - 1;
		memcpy(nAccount,m_sAccountName.c_str(),used);
	    nAccount[used] = '\0';

		switch ( nResult )
		{
		case ACTION_SUCCESS:		/* Login Success ! 登录成功*/

#ifdef	CONSOLE_DEBUG
	cprintf( "CGamePlayer::VerifyAccount Successful!\n" );
	OutputDebugString("CGamePlayer::VerifyAccount Successful!\n" );
#endif
			// pReturn->Account;
			nQueryResult |= LOGIN_R_SUCCESS;

			nNextTask = enumToNextTask;

			m_nExtPoint = pReturn->nExtPoint;

			m_nVipType  = pReturn->nVipType;

			m_nLeftTime = pReturn->nLeftTime;

			m_nExtGameLeftTime = pReturn->nGameLeftTime;
			break;

		case E_ACCOUNT_OR_PASSWORD:	/* Login Failed ! ---账号或密码错误 */

#ifdef	CONSOLE_DEBUG
	cprintf( "CGamePlayer::VerifyAccount Failed! [name or pwd error]\n" );
	OutputDebugString("CGamePlayer::VerifyAccount Failed! [name or pwd error]\n" );
#endif
			nQueryResult |= LOGIN_R_ACCOUNT_OR_PASSWORD_ERROR;
			m_bAutoUnlockAccount = false;
			break;
		case E_ACCOUNT_EXIST:		/* Login Failed ! --- 已经登录 */

#ifdef	CONSOLE_DEBUG
	cprintf( "CGamePlayer::VerifyAccount Failed! [Already login]\n" );
	OutputDebugString("CGamePlayer::VerifyAccount Failed! [Already login]\n" );
#endif
			nQueryResult |= LOGIN_R_ACCOUNT_EXIST;
			m_bAutoUnlockAccount = false;
			break;

		case E_ACCOUNT_FREEZE:		/* Login Failed ! --- 账号给冻结 */

#ifdef	CONSOLE_DEBUG
	cprintf( "CGamePlayer::VerifyAccount Failed! [Account Freeze]\n" );
	OutputDebugString( "CGamePlayer::VerifyAccount Failed! [Account Freeze]\n" );
#endif

			nQueryResult |= LOGIN_R_FREEZE;
			m_bAutoUnlockAccount = false;
			FreezeMoney(nAccount,0,0); //告诉pays 退出账号
			break;

		case E_ACCOUNT_NODEPOSIT:	/* Login Failed ! --- 点卡不足 */

#ifdef	CONSOLE_DEBUG
	cprintf( "CGamePlayer::VerifyAccount Failed! [No Money]\n" );
	OutputDebugString( "CGamePlayer::VerifyAccount Failed! [No Money]\n" );
#endif

			nQueryResult |= LOGIN_R_TIMEOUT;  //使用时间到 点卡不足

			m_bAutoUnlockAccount = false;
			FreezeMoney(nAccount,0,0); //告诉pays 退出账号
			break;

		default:					/* Login Failed ! */
#ifdef	CONSOLE_DEBUG
	cprintf( "CGamePlayer::VerifyAccount Failed!\n" );  
	OutputDebugString( "CGamePlayer::VerifyAccount Failed!\n" );  
#endif
			nQueryResult |= LOGIN_R_FAILED;  //服务器忙 或满员
			m_bAutoUnlockAccount = false;
			FreezeMoney(nAccount,0,0); //告诉pays 退出账号
			break;
		}
		/*
		 * Notify the result to player  把结果告诉玩家 客户端
		 */
		if(!_VerifyAccount_ToPlayer(nQueryResult,pReturn->nLeftTime,pReturn->nExtPoint))
		{
			nNextTask=enumError;
			FreezeMoney(nAccount,0,0); //告诉pays 退出账号

			if (hbishopOutput)
				//AddOutputStringPlayer(hbishopOutput,"---单机端,登录出错---"); //hCtrlPlayr
				AddOutputStringPlayer(hbishopOutput,"---Single machine side, login error---"); //hCtrlPlayr

		}

		SAFE_RELEASE(pRetBuffer);
		m_theDataQueue[enumOwnerAccSvr].Detach(s2c_accountfdfsglogin);

		return nNextTask;
	}

	return enumRepeat;
}

bool CGamePlayer::_VerifyAccount_ToPlayer( UINT nQueryResult, unsigned long nLeftTime,unsigned long nExePiont)
{
	
	//SetBishopVer();
	bool nReg=true;
	SetPlayerRevivePos();

	KLoginAccountInfo lai;

	ZeroMemory( &lai, sizeof( KLoginAccountInfo ) );

	/*
	 * Account
	 */
	size_t used = sizeof( lai.Account );
	used = ( used > m_sAccountName.length() ) ? m_sAccountName.length() : ( used - 1 );
	memcpy( lai.Account, m_sAccountName.c_str(), used );
	lai.Account[used] = '\0';

	/*
	 * Password
	 */
	used = sizeof( lai.Password.szPassword );//ganlei
	used = ( used > m_sPassword.length() ) ? m_sPassword.length() : ( used - 1 );

	memcpy( lai.Password.szPassword, m_sPassword.c_str(), used );//ganlei
	lai.Password.szPassword[used] = '\0';//ganlei
	lai.nLeftTime = nLeftTime; //账号的剩余时间
    lai.nVipType  = m_nVipType;//会员类型
	lai.nExtPiont = nExePiont;
	lai.nSelSererIdx = m_nForBitOpenSer;
	sprintf(lai.nBishopver,"%.2lf",m_nBishopVer);	//m_nBishopVer


	if (hbishopOutput)
	{
			char msg[128];
			//sprintf(msg,"---当前发送的版本号:%s,原:%lf ---",lai.nBishopver,m_nBishopVer);
			sprintf(msg,"---Currently sent version number: %s, original: %lf ---",lai.nBishopver,m_nBishopVer);
			AddOutputStringPlayer(hbishopOutput,msg); //增加IP信息
	} 
	//----------------------------------------------开始检测账号是否被冻结
	int nRet=0;

	if (BST_CHECKED == ::IsDlgButtonChecked(hMianHwd,IDC_CHECK_OPEN_ALL_VIP))
	{//Vip开启动检测
		nRet=1;
		char buf[ACC_LENGTH] = {0};
		int nCount = ::SendMessage(hAccCheckList_VIP,LB_GETCOUNT, 0, 0);

		for(int i=0; i<nCount; ++i)
		{
			::SendMessage(hAccCheckList_VIP, LB_GETTEXT, i, (LPARAM)buf);	//获取字符串
			if (strstr(lai.Account,buf))
			{//在列表 就放行
				nRet=0;
				break;
			}
		}
		
	} 


	if(hAccCheckList)
	{//账号检测
		char buf[ACC_LENGTH] = {0};
		int nCount = ::SendMessage(hAccCheckList,LB_GETCOUNT, 0, 0);
		
		for(int i=0; i<nCount; ++i)
		{
			::SendMessage(hAccCheckList, LB_GETTEXT, i, (LPARAM)buf);	//获取字符串
			// AfxMessageBox(buf);
			if (strstr(lai.Account,buf))
			{
				nRet=2;
				break;
			}
		}
	}
/*

  const char* pInfo = m_pNetServer->GetServer()->GetClientInfo(id);
  if (pInfo)
  {
		m_IP = _a2ip(pInfo);
		
		  const char* pColon = strchr(pInfo, ':');
		  if (pColon)
		  m_port = atoi(pColon + 1);  //加为了去除冒号  //端口
	}
*/
	char Address[128]={0},nIpInfo[128]={0}; //IP地址

   //const char* pInfo=
   m_pPlayerServer->GetClientInfo(m_lnIdentityID,nIpInfo,sizeof(nIpInfo));
   if (nIpInfo[0])
   {
	  DWORD m_IP = _a2ip(nIpInfo);
	  const unsigned char* pIpAddress=(const unsigned char*)&m_IP;  
	  sprintf(Address, "%d.%d.%d.%d", pIpAddress[0],pIpAddress[1],pIpAddress[2],pIpAddress[3]);
	   //const char* pColon = strchr(pInfo, ':');
	   //if (pColon)
	   //m_port = atoi(pColon + 1);  //加为了去除冒号  //端口
   }

   if (hClientIpList)
   {
	   AddOutputStringPlayer(hClientIpList,Address); //增加客户端IP信息
   }

	if(hIpCheckList)
	{//过滤IP列表
		char buf[32] = {0};
		int nCount = ::SendMessage(hIpCheckList,LB_GETCOUNT, 0, 0);
		
		for(int i=0; i<nCount; ++i)
		{
			::SendMessage(hIpCheckList,LB_GETTEXT,i,(LPARAM)buf);	//获取字符串
			// AfxMessageBox(buf);
			if (strstr(Address,buf))
			{
				nRet=3;
				break;
			}
		}	
	}
//----------------------单机设置
	/*PassChecksum nJiemi;

	char strSverip[64];

	nJiemi.SimplyDecryptPassword(strSverip,GFDAGFDG); //解	127.0.

	if (strstr(Address,strSverip))
	{//如果等于就放行
	}
	else
	{
		nJiemi.SimplyDecryptPassword(strSverip,TRGFDGGG); //解	192.168.
		if (strstr(Address,strSverip))	
		{//如果等于就放行

		}
		else
		{//断开链接
			nQueryResult |= LOGIN_R_FREEZE;	
			m_bAutoUnlockAccount = false;
			nReg=false;
		    lai.nForbitopennum=0;
			return nReg;
		}
	}*/
///----------------------
	if (nRet)
	{//账号被冻结
		m_bAutoUnlockAccount = false;
		nReg=false;
		lai.nForbitopennum=0;	
		char msg[256]={0},nIp[32]={0};
		m_pPlayerServer->GetClientInfo(m_lnIdentityID,nIp,sizeof(nIp));

		switch(nRet)
		{
		case 1:
			nQueryResult |= LOGIN_R_NO_IN_PROGRESS;
			//sprintf(msg,"=====账号: %s IP:%s 被冻结中,非VIP禁止进入 1=====",lai.Account,nIp);
			sprintf(msg,"=====Account: %s IP: %s is LOGIN_R_NO_IN_PROGRESS, non-VIP are prohibited from entering 1=====",lai.Account,nIp);
			break;
		case 2://账号
			nQueryResult |= LOGIN_R_FREEZE;
			//sprintf(msg,"=====账号: %s IP:%s 被冻结中,禁止进入 2=====",lai.Account,nIp);
			sprintf(msg,"=====Account: %s IP: %s is LOGIN_R_FREEZE and access is prohibited 2=====",lai.Account,nIp);
			break;
		case 3://ip
			nQueryResult |= LOGIN_R_VALID;
			//sprintf(msg,"=====账号: %s IP:%s 被冻结中,禁止进入 3=====",lai.Account,nIp);
			sprintf(msg,"=====Account: %s IP: %s is LOGIN_R_VALID and access is prohibited 3=====",lai.Account,nIp);
			break;
		default:
			nQueryResult |= LOGIN_R_INVALID;
			//sprintf(msg,"=====账号: %s IP:%s 被冻结中,禁止进入 0=====",lai.Account,nIp);
			sprintf(msg,"=====Account: %s IP: %s is LOGIN_R_INVALID and access is prohibited 0=====",lai.Account,nIp);
			break;
		}
		//chu y 
		if (hbishopOutput)
			AddOutputStringPlayer(hbishopOutput,msg); //hCtrlPlayr
	}
	else
	{//账号 或IP 没有给冻结 开始检测多开
		if (hForBitOpenIpList)
		{
		   int mReg=0,nHaveConnetNum=0;
		   if (BST_CHECKED == ::IsDlgButtonChecked(hMianHwd,IDC_CHECK_FORBITIP))
		   { //是否开检测禁止多开客户端IP列表 
			  char buf[32] = {0}; 
			  /*
			  if (hHaveConnetIpList)
			  { 
				 int nCount = ::SendMessage(hHaveConnetIpList,LB_GETCOUNT, 0, 0);
				 for(int i=0; i<nCount; ++i)
				 {  
					 ::SendMessage(hHaveConnetIpList,LB_GETTEXT,i,(LPARAM)buf);	//获取字符串
					 if (strstr(Address,buf))
					 {//有相同  
						 nHaveConnetNum++;
					 } 
				 }

				 if (nHaveConnetNum>=(m_nForBitOpenNum-1))
				 {//服务器端阻断
					 nQueryResult |= LOGIN_R_FREEZE;  //直接断开连接	
					 m_bAutoUnlockAccount = false;
					 //FreezeMoney(lai.Account,0,0); //告诉pays 退出账号
					 nReg=false;
		             lai.nForbitopennum=0;
				 }
				 else
				 {
			         AddOutputIpList(hHaveConnetIpList,Address);
				 } 	
			  } 
			  */
		      int nCount = ::SendMessage(hForBitOpenIpList,LB_GETCOUNT, 0, 0);
		      for(int i=0; i<nCount; ++i)
			  { //是否禁止某个IP多开  
				  ::SendMessage(hForBitOpenIpList,LB_GETTEXT,i,(LPARAM)buf);	//获取字符串
				// AfxMessageBox(buf);
				  if (strstr(Address,buf))
				  {  
				     mReg=1;
					 break;
				  } 
			  }   
		   } 
			
			if (BST_CHECKED == ::IsDlgButtonChecked(hMianHwd,IDC_CHECK_FORBITALL))
			{//是否开启所有IP禁止多开
				mReg = 1;
			} 

			if(mReg)
			{//客户端阻断
			  lai.nForbitopennum= m_nForBitOpenNum;
			}
			else
			  lai.nForbitopennum= 0;
		}
	}
	//----------------------------------------------
    /*
	 * The other info
	 */
    lai.ProtocolVersion = KPROTOCOL_VERSION;

	lai.Param = nQueryResult;	  //结果

	lai.Size = sizeof(lai);	

	CBuffer *pBuffer = m_theGlobalAllocator.Allocate();

	BYTE *pData = const_cast< BYTE * >(pBuffer->GetBuffer());

	BYTE bProtocol = s2c_login;
	size_t sizeProtocol = sizeof(BYTE);
	memcpy(pData, &bProtocol, sizeProtocol );
	memcpy(pData + sizeProtocol,(BYTE *)(&lai),sizeof(lai));
    //把获取账号的数据 发往game --客户端
	m_pPlayerServer->SendData(m_lnIdentityID, pData, (sizeof(KLoginAccountInfo) + sizeof(BYTE)/* protocol id */ ) );
	pBuffer->Release();
	//chu y 
	return nReg;
}
//获取角色列表线程
UINT CGamePlayer::QueryRoleList()
{
#ifdef	CONSOLE_DEBUG
	cprintf( "CGamePlayer::QueryRoleList...\n" );
	OutputDebugString( "CGamePlayer::QueryRoleList...\n" );
#endif

	const UINT lenAN = strlen(m_sAccountName.c_str());//实际长度
	const UINT lenPL = lenAN + sizeof(TProcessData);
	
	CBuffer *pBuffer = m_theGlobalAllocator.Allocate();	

	TProcessData *pPlayerList = reinterpret_cast<TProcessData *>(const_cast<BYTE *>(pBuffer->GetBuffer()));

	ASSERT(pPlayerList);

	pPlayerList->nProtoId = c2s_roleserver_getrolelist;	   //发往goddess
	pPlayerList->nDataLen = lenAN;                         //名字的实际长度
	pPlayerList->ulIdentity = m_lnIdentityID;              //客户端链接号
    pPlayerList->uSelServer = m_selServer;
	pPlayerList->pDataBuffer[0] = s_nRoleListCount;

	memcpy(&pPlayerList->pDataBuffer[1],m_sAccountName.c_str(),lenAN);
	pPlayerList->pDataBuffer[lenAN + 1] = '\0';

	m_theDataQueue[enumOwnerRoleSvr].Empty();

	if (hbishopOutput)
	{
	  char msg[256]={0},nIp[64]={0};
	  m_pPlayerServer->GetClientInfo(m_lnIdentityID,nIp,sizeof(nIp));
	  //sprintf(m_ClientIp,"%s-%s",m_pPlayerServer->GetClientInfo(m_lnIdentityID),m_sAccountName.c_str());
	  //sprintf(msg,"--获取角色线程,获取账号: %s 长度:%d 角色清单(客户端ip:%s),%d区 发往goddess--",m_sAccountName.c_str(),lenAN,nIp,m_selServer);
	  snprintf(msg, sizeof(msg),"--Get the role thread, get the account: %s, length: %d, role list (client ip: %s), %d area, send to goddess--",m_sAccountName.c_str(),lenAN,nIp,m_selServer);
	  AddOutputStringPlayer(hbishopOutput,msg);
	  bs_LogFile.gTraceLogFile(msg,strlen(msg));
	/*const char* pInfo = m_pNetServer->GetServer()->GetClientInfo(id);
			if (pInfo)
			{
			  m_IP = _a2ip(pInfo);
			  const char* pColon = strchr(pInfo, ':');
			  if (pColon)
			  m_port = atoi(pColon + 1);  //加为了去除冒号  //端口
			}
	  */
	}
	if (m_pDBRoleClient)
	m_pDBRoleClient->SendPackToServer((const void *)pPlayerList,lenPL);

	SAFE_RELEASE(pBuffer);	

	return enumToNextTask;		//下一个线程
}

UINT CGamePlayer::ProcessRoleList()
{
#ifdef	CONSOLE_DEBUG
	cprintf( "CGamePlayer::ProcessRoleList...\n" );
	OutputDebugString( "CGamePlayer::ProcessRoleList...\n" );
#endif
	//接收goddess 发过来的数据包
	CBuffer *pRetBuffer = m_theDataQueue[enumOwnerRoleSvr].Attach(s2c_roleserver_getrolelist_result);

	if (pRetBuffer)
	{
		UINT nNextTask = enumError;

		const TProcessData *pPD = (const TProcessData *)pRetBuffer->GetBuffer();

		int nRoleCount = CPackager::Peek((const char *)pPD->pDataBuffer);

#ifdef	CONSOLE_DEBUG
	cprintf( "CGamePlayer::ProcessRoleList find %d role in list\n", nRoleCount );
		char msg[512]={0};
	sprintf(msg,"CGamePlayer::ProcessRoleList find %d role in list\n", nRoleCount );
	OutputDebugString( msg );
#endif

		if (nRoleCount >=0)
		{
			/*char nMsg[128]={0};
	        sprintf(nMsg,"收到goddess角色列表:%d | %d \n s2c_roleserver_getrolelist_result",nRoleCount,m_lnIdentityID);
	        ::MessageBox(NULL,nMsg,"b提示:",MB_OK|MB_ICONERROR);
			*/
			char msg[128]={0};
			//sprintf(msg,"--收到goddess角色列表:%d | %d 发往客户端game--",nRoleCount,m_lnIdentityID);
			sprintf(msg,"--Received goddess character list: %d | %d sent to client game--",nRoleCount,m_lnIdentityID);
			AddOutputStringPlayer(hbishopOutput,msg);
			bs_LogFile.gTraceLogFile(msg,sizeof(msg));

			m_theDataQueue[enumOwnerPlayer].Empty();
			//发往　game 客户端　连接端口的　56722
			m_pPlayerServer->SendData(m_lnIdentityID,pRetBuffer->GetBuffer(),pRetBuffer->GetUsed());

			nNextTask = enumSelAddDelRole;//增加角色或删除角色线程
		}

		SAFE_RELEASE(pRetBuffer);
		m_theDataQueue[enumOwnerRoleSvr].Detach(s2c_roleserver_getrolelist_result);

		return nNextTask;
	}

	return enumRepeat;
}
//选择添加或删除角色时调用的函数
UINT CGamePlayer::SelAddDelRole()
{
	CBuffer *pRetBuffer = NULL;

#ifdef	CONSOLE_DEBUG
	cprintf( "CGamePlayer::SelAddDelRole...\n" );
//	OutputDebugString( "CGamePlayer::SelAddDelRole...\n" );
#endif

	/*
	 * Select a role  选择这个角色进入游戏的 
	 */
	pRetBuffer = m_theDataQueue[enumOwnerPlayer].Attach(c2s_dbplayerselect);

	if (pRetBuffer)
	{
		//::MessageBox( NULL, "c2s_dbplayerselect is ok!", "c2s_dbplayerselect", MB_OK );
		UINT nNextTask = enumError;

		const tagDBSelPlayer *pDSPC = (const tagDBSelPlayer *)pRetBuffer->GetBuffer();
		char nAccount[ACC_LENGTH];
		// as in VerifyAccount: one clamp on the name both the copy and the
		// terminator read, instead of a ternary whose long arm is a call
		UINT used = (UINT)m_sAccountName.length();  //size_t
		if (used >= sizeof(nAccount))
			used = sizeof(nAccount) - 1;
		memcpy(nAccount,m_sAccountName.c_str(),used); //账号
		nAccount[used] = '\0';


#ifdef	CONSOLE_DEBUG
	    cprintf( "CGamePlayer::SelAddDelRole Select a role from list\n" );
	OutputDebugString( "CGamePlayer::SelAddDelRole Select a role from list\n" );
#endif

		if (_QueryRoleInfo_ToDBRole(pDSPC->szRoleName,nAccount))
		{
			nNextTask = enumLoginSelectRole;
		}
		else
		{//退出游戏
			nNextTask=enumError;	    //中断线程
			FreezeMoney(nAccount,0,0);  //告诉pays 退出账号	  
		}

		SAFE_RELEASE(pRetBuffer);
		m_theDataQueue[enumOwnerPlayer].Detach(c2s_dbplayerselect);

		return nNextTask;
	}

	/*
	 * Create a role  接受客户端发来的 删除 新建角色的数据包
	 */
	pRetBuffer = m_theDataQueue[enumOwnerPlayer].Attach(c2s_newplayer);

	if ( pRetBuffer )
	{
		UINT nNextTask = enumError;

		const TProcessData *pCRPD = (const TProcessData *)pRetBuffer->GetBuffer();		
		const NEW_PLAYER_COMMAND *pNPC = (const NEW_PLAYER_COMMAND *)(pCRPD->pDataBuffer);

		ASSERT(pNPC);
		
		char szRoleName[NAME_LENGHT];
		unsigned int namelen = pRetBuffer->GetUsed() - sizeof(TProcessData);//size_t
		m_selServer = pCRPD->uSelServer;//哪个区的在创建角色
		namelen = ( namelen < NAME_LENGHT - 1) ? namelen : NAME_LENGHT - 1;

		memcpy( szRoleName, pNPC->m_szName, namelen );
		szRoleName[namelen] = '\0';

		char sMsga[125];
		snprintf(sMsga, sizeof(sMsga),"SelAddDelRole Create a role %s|%d 区|%d|%d|%d",szRoleName,m_selServer,pNPC->m_btRoleNo,pNPC->m_btSeries,pNPC->m_NativePlaceId);
		AddOutputStringPlayer(hbishopOutput, sMsga);
		//::MessageBox(0,sMsga,"创建角色",MB_OK);

#ifdef	CONSOLE_DEBUG
	cprintf( "CGamePlayer::SelAddDelRole Create a role [Name : %s]\n", szRoleName );
		char msg[512]={0};
	sprintf(msg,"CGamePlayer::SelAddDelRole Create a role [Name : %s]\n", szRoleName );
	OutputDebugString( msg );
#endif
		m_theDataQueue[enumOwnerRoleSvr].Empty();

		if ( _CreateNewPlayer_ToDBRole(( const char * )szRoleName, 
					pNPC->m_btRoleNo, 
					pNPC->m_btSeries,
					pNPC->m_NativePlaceId,pCRPD->uSelServer))
		{
			nNextTask = enumLoginCreateRole;
		}
		else
		{
			tagNewDelRoleResponse ndrr;
			ndrr.cProtocol = s2c_rolenewdelresponse;
			ndrr.szRoleName[0] = '\0';
			ndrr.bSucceeded = false;
			m_pPlayerServer->SendData(m_lnIdentityID, (const void*)&ndrr, sizeof(tagNewDelRoleResponse));
		}
	    //char sMsga[125];
		//sprintf(sMsga,"%s|%d|%d|%d",pNPC->m_szName,pNPC->m_btRoleNo,pNPC->m_btSeries,pNPC->m_NativePlaceId);
		//::MessageBox(0,sMsga,"创建角色",MB_OK);
		SAFE_RELEASE(pRetBuffer);
		m_theDataQueue[enumOwnerPlayer].Detach(c2s_newplayer);

		return nNextTask;
	}

	/*
	 * Delete a role 接受客户端请求 删除角色
	 */
	/*pRetBuffer = m_theDataQueue[enumOwnerPlayer].Attach(c2s_roleserver_deleteplayer);

	if (pRetBuffer)
	{
		UINT nNextTask = enumError;

#ifdef	CONSOLE_DEBUG
	cprintf( "CGamePlayer::SelAddDelRole Del a role in list\n" );
#endif
		m_theDataQueue[enumOwnerRoleSvr].Empty();

		if (_DeleteRole_ToDBRole(pRetBuffer->GetBuffer(), pRetBuffer->GetUsed() ) )
		{
            nNextTask = enumToNextTask;
		}

		SAFE_RELEASE(pRetBuffer);
		m_theDataQueue[enumOwnerPlayer].Detach(c2s_roleserver_deleteplayer);

		if ( enumToNextTask != nNextTask )
		{//删除失败	将结果发往 客户端
			tagNewDelRoleResponse ndrr;
			ndrr.cProtocol = s2c_rolenewdelresponse;
			ndrr.szRoleName[0] = '\0';
			ndrr.bSucceeded = false;

			m_pPlayerServer->SendData( m_lnIdentityID, ( const void * )&ndrr, sizeof( tagNewDelRoleResponse ) );

			nNextTask = enumSelAddDelRole;
		}

		return nNextTask;
	}*/

	return enumRepeat;
}

UINT CGamePlayer::DelRole_WaitForVerify()
{
#ifdef	CONSOLE_DEBUG
	cprintf( "CGamePlayer::DelRole_WaitForVerify...\n" );
	OutputDebugString( "CGamePlayer::DelRole_WaitForVerify...\n" );
#endif
	CBuffer *pRetBuffer = m_theDataQueue[enumOwnerAccSvr].Attach(s2c_accountfdfsglogin);	//c2s_accountlogin
    
    if (pRetBuffer)
    {
		UINT nNextTask = enumError;

		KAccountUserReturnExt* pReturn = ( KAccountUserReturnExt * )( pRetBuffer->GetBuffer() + 1/* size of a protocol byte */ );

		int nResult = pReturn->nReturn;

		if (nResult == ACTION_SUCCESS)
        {
            const char *pRoleName = m_sDelRoleName.c_str();
	        
            const size_t lenRN = strlen( pRoleName );
	        const size_t lenRI = sizeof( TProcessData ) - 1 + lenRN;
	        
	        CBuffer *pBuffer = m_theGlobalAllocator.Allocate();
	        
	        TProcessData *pRoleInfo = reinterpret_cast< TProcessData * >( const_cast< BYTE * >( pBuffer->GetBuffer() ) );
	        
	        ASSERT( pRoleInfo );
	        //发包给 goeess 删除角色
	        pRoleInfo->nProtoId = c2s_roleserver_deleteplayer;
	        pRoleInfo->nDataLen = lenRN;
	        pRoleInfo->ulIdentity = m_lnIdentityID;
	        
	        memcpy( &pRoleInfo->pDataBuffer[0], pRoleName, lenRN );
	        pRoleInfo->pDataBuffer[lenRN] = '\0';
	        if  (m_pDBRoleClient)
	        m_pDBRoleClient->SendPackToServer( ( const void * )pRoleInfo, lenRI );
	        
	        SAFE_RELEASE(pBuffer);
        
            nNextTask = enumLoginDeleteRole;
        }
        else
        {//是失败。。。。
            tagNewDelRoleResponse ndrr;
            ndrr.cProtocol = s2c_rolenewdelresponse;
            ndrr.szRoleName[0] = '\0';
            ndrr.bSucceeded = false;

            m_pPlayerServer->SendData(m_lnIdentityID,( const void *)&ndrr, sizeof(tagNewDelRoleResponse));

            nNextTask = enumSelAddDelRole;  // 表示这次的动作完成，进入下一个等待流程
        }

		SAFE_RELEASE(pRetBuffer);
		m_theDataQueue[enumOwnerAccSvr].Detach(s2c_accountfdfsglogin);

        return nNextTask;
    }
	return enumRepeat;


}
//等待goddes创建角色的结果--发向 客户单 角色列表响应

UINT CGamePlayer::WaitForCreateResult()
{
#ifdef	CONSOLE_DEBUG
	cprintf( "CGamePlayer::WaitForCreateResult...\n" );
	OutputDebugString( "CGamePlayer::WaitForCreateResult...\n" );
#endif

	CBuffer *pRetBuffer = m_theDataQueue[enumOwnerRoleSvr].Attach(s2c_roleserver_createrole_result);

	if (pRetBuffer)
	{
		UINT nNextTask = enumError;

		TProcessData *pPD = ( TProcessData * )pRetBuffer->GetBuffer();

		ASSERT( pPD );

		char cResult = pPD->pDataBuffer[0];  //接受bishop 返回的结果
//创建删除角色结果 结构 发向 客户端 角色列表的响应
		tagNewDelRoleResponse ndrr;
		ndrr.cProtocol = s2c_rolenewdelresponse;
		ndrr.szRoleName[0] = '\0';
		ndrr.bSucceeded = false;

#ifdef	CONSOLE_DEBUG
		cprintf( "CGamePlayer::WaitForCreateResult %s\n", ( 1 == cResult ) ? "Successful" : "Failed" );
    char debugMessage[256];
    sprintf(debugMessage,"CGamePlayer::WaitForCreateResult %s\n", (1 == cResult) ? "Successful" : "Failed");
    OutputDebugString(debugMessage); 
#endif

		switch ( cResult )
		{
		case 1:	// Successed
			ndrr.bSucceeded = true;
            //发向客户端？
			m_pPlayerServer->SendData(m_lnIdentityID, (const void *)&ndrr, sizeof(tagNewDelRoleResponse));
			nNextTask = enumToNextTask;
			break;
		//case -1:	// Failed
		//case -2:	// Have exist
		default:
			ndrr.bSucceeded = false;
			m_pPlayerServer->SendData(m_lnIdentityID, (const void *)&ndrr, sizeof(tagNewDelRoleResponse));
			nNextTask = enumSelAddDelRole;
			break;
		}

		SAFE_RELEASE(pRetBuffer);
		m_theDataQueue[enumOwnerRoleSvr].Detach(s2c_roleserver_createrole_result);

		return nNextTask;
	}

	return enumRepeat;
}

UINT CGamePlayer::WaitForDeleteResult()
{
#ifdef	CONSOLE_DEBUG
		cprintf( "CGamePlayer::WaitForDeleteResult\n" );
    OutputDebugString("CGamePlayer::WaitForDeleteResult\n" );
#endif

	CBuffer *pRetBuffer = m_theDataQueue[enumOwnerRoleSvr].Attach(s2c_roleserver_deleterole_result);

	if (pRetBuffer)
	{
		UINT nNextTask = enumError;

		TProcessData *pPD = ( TProcessData * )pRetBuffer->GetBuffer();

		ASSERT( pPD );

		char cResult = pPD->pDataBuffer[0];

		tagNewDelRoleResponse ndrr;
		ndrr.cProtocol = s2c_rolenewdelresponse;
		ndrr.szRoleName[0] = '\0';
		ndrr.bSucceeded = false;	

#ifdef	CONSOLE_DEBUG
		cprintf( "CGamePlayer::WaitForDeleteResult %s\n", ( 1 == cResult ) ? "Successful" : "Failed" );
    char debugMessage[256];
    sprintf(debugMessage,"CGamePlayer::WaitForDeleteResult %s\n", ( 1 == cResult ) ? "Successful" : "Failed" );
    OutputDebugString(debugMessage); 
#endif

		switch ( cResult )
		{
		case 1:	// Successed

			ndrr.bSucceeded = true;

			m_pPlayerServer->SendData( m_lnIdentityID, ( const void * )&ndrr, sizeof( tagNewDelRoleResponse ) );

			nNextTask = enumSelAddDelRole;
			break;

		//case -1:	// Failed
		//case -2:	// Have exist
		default:

			ndrr.bSucceeded = false;

			m_pPlayerServer->SendData( m_lnIdentityID, ( const void * )&ndrr, sizeof( tagNewDelRoleResponse ) );

			nNextTask = enumSelAddDelRole;
			break;
		}

		SAFE_RELEASE(pRetBuffer);
		m_theDataQueue[enumOwnerRoleSvr].Detach(s2c_roleserver_deleterole_result);

		return nNextTask;
	}

	return enumRepeat;
}
//选择这个角色进入游戏的 获取角色名信息 发往goddes
bool CGamePlayer::_QueryRoleInfo_ToDBRole( const char *pRoleName,const char *pAccName )
{
	if ( NULL == pRoleName || '\0' == pRoleName[0] )
	{
		return false;
	}
	
	const UINT lenRN = strlen(pRoleName);//
	const UINT lenRI = sizeof(TProcessData) - 1 + lenRN;

	CBuffer *pBuffer = m_theGlobalAllocator.Allocate();
			
	TProcessData *pRoleInfo = reinterpret_cast<TProcessData *>(const_cast<BYTE *>(pBuffer->GetBuffer()));
	
	ASSERT(pRoleInfo);
	
	int nRet=0;

	if(hAccCheckList)
	{//账号角色检测
		char buf[NAME_LENGHT] = {0};
		int nCount = ::SendMessage(hAccCheckList,LB_GETCOUNT, 0, 0);
		char nRoleName[64]= {0};
		snprintf(nRoleName, sizeof(nRoleName), "%s", pRoleName);

		for(int i=0; i<nCount; ++i)
		{
			ZeroMemory(buf,sizeof(buf));
			::SendMessage(hAccCheckList, LB_GETTEXT, i, (LPARAM)buf);	//获取字符串
			// AfxMessageBox(buf);
			if (buf[0] && strstr(nRoleName,buf))
			{
				nRet=1;
				break;
			}
		}
		
		if (nRet) 
		{
		    SAFE_RELEASE(pBuffer);
			return FALSE;
		}
	}

/*	if (hHaveConnetIpList)
	{//写IP
		char Address[128]={0}; //IP地址
		const char* pInfo=m_pPlayerServer->GetClientInfo(m_lnIdentityID);
		if (pInfo)
		{
			DWORD m_IP = _a2ip(pInfo);
			const unsigned char* pIpAddress=	(const unsigned char*)&m_IP;  
			sprintf(Address, "%d.%d.%d.%d", pIpAddress[0],pIpAddress[1],pIpAddress[2],pIpAddress[3]);
			//const char* pColon = strchr(pInfo, ':');
			//if (pColon)
			//m_port = atoi(pColon + 1);  //加为了去除冒号  //端口
			AddOutputIpList(hHaveConnetIpList,Address);
		} 
	}*/

	pRoleInfo->nProtoId = c2s_roleserver_getroleinfo;
	pRoleInfo->nDataLen = lenRN;
	pRoleInfo->ulIdentity = m_lnIdentityID;
	// Goddess splits these two names into char[NAME_LENGHT] and char[ACC_LENGTH];
	// a longer pair would overrun its copies, so refuse it here instead
	if (NULL == pAccName || lenRN >= NAME_LENGHT || strlen(pAccName) >= ACC_LENGTH)
	{
		SAFE_RELEASE(pBuffer);
		return false;
	}
	const UINT lenAN = (UINT)strlen(pAccName);
	memcpy(&pRoleInfo->pDataBuffer[0],pRoleName,lenRN);
	pRoleInfo->pDataBuffer[lenRN] = '\0';
	memcpy(&pRoleInfo->pDataBuffer[lenRN],pAccName,lenAN);
	pRoleInfo->pDataBuffer[lenRN+lenAN] = '\0';
    
	pRoleInfo->uSelServer = m_selServer; //是这个区的角色数据

	//sprintf(pRoleInfo->nAccName,pAccName);
    //char sMsga[64];
	//sprintf(sMsga,"选择(%d区)角色检测(账号:%s,角色:%s)",m_selServer,pAccName,pRoleName);
	//AddOutputIpList(hbishopOutput,sMsga);
	if (m_pDBRoleClient)
	   m_pDBRoleClient->SendPackToServer((const void *)pRoleInfo,lenRI+lenAN);
	
	SAFE_RELEASE(pBuffer);
	return true;
}
//写进数据 创建新角色时存档
bool CGamePlayer::_CreateNewPlayer_ToDBRole( const char *pRoleName, 
						int nRoleSex, 
						int nRoleClass,
						unsigned short nMapID,unsigned int nServNoIndex)
{
	if ( NULL == pRoleName || '\0' == pRoleName[0] )
	{
		return false;
	}
	 
/*	int nLength = m_thePlayerCreator.CountUTF8Character((unsigned char*)pRoleName);
	if (nLength < 6 || nLength > 16)
		return false;

	if (!m_thePlayerCreator.CheckNameValid((unsigned char*)pRoleName))
		return false;
*/

	int nLength = m_thePlayerCreator.CountTCVN3Characters(pRoleName);
	if (nLength < 6 || nLength > 16)
		return false;

	if (!m_thePlayerCreator.IsTCVN3NameValid(pRoleName))
		return false;


	unsigned int datalength = 0; //size_t

	CPlayerCreator::ROLEPARAM	RP;

	int nMinLen = strlen( pRoleName );
	nMinLen = nMinLen > sizeof( RP.szName ) - 1 ? sizeof( RP.szName ) - 1 : nMinLen;
	memcpy( RP.szName, pRoleName, nMinLen );//角色名
	RP.szName[nMinLen] = '\0';

	nMinLen = m_sAccountName.size();
	nMinLen = nMinLen > sizeof( RP.szAccName ) - 1 ? sizeof( RP.szAccName ) - 1 : nMinLen;
	memcpy( RP.szAccName, m_sAccountName.c_str(), nMinLen );//帐号名
	RP.szAccName[nMinLen] = '\0';
	RP.nSex = nRoleSex;  //新建角色的 五行
	RP.nSeries = nRoleClass;//新建角色的性别
	RP.nMapID = nMapID;
    //赋值给角色数据包（写数据库）
	const TRoleData *pRoleData = m_thePlayerCreator.GetRoleData(datalength, &RP,nServNoIndex);//获取产生新人基本数据

	if (pRoleData && datalength)
	{
		/*char sMsg[128];
		sprintf(sMsg,"%d 区 物品的数量:%d \n出生地图:%d\n帐号:%s 角色:%s",m_selServer,pRoleData->nItemCount,pRoleData->BaseInfo.irevivalid,RP.szAccName,RP.szName);
		MessageBox(0,sMsg,"b角色创建成功",MB_OK);*/

		CBuffer *pData = m_theGlobalAllocator.Allocate();
		TProcessData *pPD = reinterpret_cast<TProcessData *>(const_cast<BYTE *>(pData->GetBuffer()));
//发往goddess 存档
		//pRoleData->BaseInfo.isWaiGua = m_selServer; //复值给角色 定位这个角色属于哪个区的数据

		pPD->nProtoId = c2s_roleserver_createroleinfo;
		pPD->ulIdentity = m_lnIdentityID;
		pPD->uSelServer = nServNoIndex;
		
		pPD->pDataBuffer[0] = 1;       // Create successed  创建成功标示
		pPD->nDataLen = datalength + 1 /* sizeof( pPD->pDataBuffer[0] ) */;
		memcpy(&pPD->pDataBuffer[1],pRoleData,datalength);

		/*
		 * Send a role-info to role dbserver
		 */
		UINT nRoleInfoLength = datalength + sizeof( TProcessData );

		m_thePackager.AddData(c2s_roleserver_createroleinfo,(const BYTE *)pPD,nRoleInfoLength);

		/*
		 * For create a role
		 */
		m_theDataQueue[enumOwnerRoleSvr].AddData(s2c_rolehjdkalritopq_getroleinfo_result, (const BYTE *)pPD,nRoleInfoLength);

		SAFE_RELEASE(pData);

		CBuffer *pPack = m_thePackager.GetHeadPack(c2s_roleserver_createroleinfo);
        //发送包个角色数据库
		while (pPack)
		{
			if (m_pDBRoleClient)
			   m_pDBRoleClient->SendPackToServer(pPack->GetBuffer(), pPack->GetUsed());

			SAFE_RELEASE(pPack);

			pPack = m_thePackager.GetNextPack(c2s_roleserver_createroleinfo);
		}

		SAFE_RELEASE(pPack);
		
		m_thePackager.DelData(c2s_roleserver_createroleinfo);

		return true;
	}

	return false;
}
//删除角色
UINT CGamePlayer::_DeleteRole_ToDBRole( const void *pData, size_t dataLength )
{
	tagDBDelPlayer *pDBDP = ( tagDBDelPlayer * )pData;
	
	if ( NULL == pData || 0 == dataLength )
	{
		if (hDugList)
			AddOutputIpList(hDugList,"--删除角色失败:数据为空--");

		return false;
	}
	
	// the three fields are fixed arrays from the socket, no promised NUL
	char szAccName[sizeof( pDBDP->szAccountName ) + 1];
	char szPassword[sizeof( pDBDP->Password.szPassword ) + 1];
	char szRoleName[sizeof( pDBDP->szRoleName ) + 1];
	ZeroMemory( szAccName, sizeof( szAccName ) );
	ZeroMemory( szPassword, sizeof( szPassword ) );
	ZeroMemory( szRoleName, sizeof( szRoleName ) );
	memcpy( szAccName, pDBDP->szAccountName, sizeof( pDBDP->szAccountName ) );
	memcpy( szPassword, pDBDP->Password.szPassword, sizeof( pDBDP->Password.szPassword ) );
	memcpy( szRoleName, pDBDP->szRoleName, sizeof( pDBDP->szRoleName ) );
	const char *pAccountName = szAccName;
	const char *pPassword = szPassword;
	const char *pRoleName = szRoleName;
	
	if ( !pAccountName || !pPassword || !pRoleName )
	{
		if (hDugList)
			AddOutputIpList(hDugList,"--删除角色失败:账号,密码,角色名为空--");
		return false;
	}
	
    
	if (m_sAccountName.compare(pAccountName) != 0)
    {
		if (hDugList)
		{
			AddOutputIpList(hDugList,"--删除角色失败:账号名校验错误--");
		}

        return false;
    }
	
    m_sSuperPassword = pPassword ? pPassword : "";
    m_sDelRoleName   = pRoleName ? pRoleName : ""; 
    m_bUseSuperPassword = true;
    
	/*if (hDugList)
	{
		char nRoelstr[128]={0};
		sprintf(nRoelstr,"--账号:%s,删除角色成功:%s,2密码:%s--",pAccountName,m_sDelRoleName.c_str(),m_sSuperPassword.c_str());
		AddOutputIpList(hDugList,nRoelstr);
	}*/

	return true;
}

/*
UINT CGamePlayer::_DeleteRole_ToDBRole( const void *pData, size_t dataLength )
{
	tagDBDelPlayer *pDBDP = ( tagDBDelPlayer * )pData;

	if ( NULL == pData || 0 == dataLength )
	{
		return false;
	}

	const char *pAccountName = pDBDP->szAccountName;
	const char *pPassword = pDBDP->Password.szPassword;//ganlei
	const char *pRoleName = pDBDP->szRoleName;

	if ( !pAccountName || !pPassword || !pRoleName )
	{
		return false;
	}

    
	if (m_sAccountName.compare( pAccountName ) != 0)
    {
        return false;
    }

	size_t datalength = 0;
	
	CPlayerCreator::ROLEPARAM	RP;
	
	int nMinLen = strlen( pRoleName );
	nMinLen = nMinLen > sizeof( RP.szName ) ? sizeof( RP.szName ) : nMinLen;
	memcpy( RP.szName, pRoleName, nMinLen );
	RP.szName[nMinLen] = '\0';
	
	nMinLen = m_sAccountName.size();
	nMinLen = nMinLen > sizeof( RP.szAccName ) ? sizeof( RP.szAccName ) : nMinLen;
	memcpy( RP.szAccName, m_sAccountName.c_str(), nMinLen );
	RP.szAccName[nMinLen] = '\0';
	
	const TRoleData *pRoleData = m_thePlayerCreator.GetRoleData( datalength, &RP );
	if ( pRoleData && datalength )
	{
		if (m_sAccountName.compare( pRoleData->BaseInfo.caccname) != 0)
		{
			return false;
		}
	}
	else
	{
		return false;
	}


    m_sSuperPassword = pPassword ? pPassword : "";
    m_sDelRoleName   = pRoleName ? pRoleName : ""; 
    m_bUseSuperPassword = true;
    
	return true;
}
*/
//无限线程获取 角色信息 发往GS
UINT CGamePlayer::ProcessRoleInfo()
{
#ifdef	CONSOLE_DEBUG
		cprintf( "CGamePlayer::ProcessRoleInfo...\n" );
    OutputDebugString( "CGamePlayer::ProcessRoleInfo...\n" );
#endif
	//接收goddes 发过来的存档包
	CBuffer *pRetBuffer = m_theDataQueue[enumOwnerRoleSvr].Attach(s2c_rolehjdkalritopq_getroleinfo_result);

	if (pRetBuffer)
	{
		UINT nNextTask = enumError;

		TProcessData *pPA = (TProcessData *)pRetBuffer->GetBuffer();
		
		ASSERT(m_lnIdentityID == pPA->ulIdentity);

		char cResult = pPA->pDataBuffer[0];  // 结果

#ifdef	CONSOLE_DEBUG
		cprintf( "CGamePlayer::ProcessRoleInfo Send info to gameserver\n" );
    OutputDebugString( "CGamePlayer::ProcessRoleInfo Send info to gameserver\n" );
#endif
		m_nAttachServerID = -1;
//错误在此，提示服务器满员-GS  同步角色信息到GS 连接哪个GS pPA->uSelServer;
		if (1 == cResult && _SyncRoleInfo_ToGameServerFs((const void *)(&pPA->pDataBuffer[1]),pPA->nDataLen-1, pPA->uSelServer))
		{
			nNextTask = enumToNextTask;
		//	MessageBox(0, "发包成功","发包-GS提示:",MB_OK);
		}
		else
		{//发送空包 返回给客户端  提示服务器满员
			tagNotifyPlayerLogin npl;
			memset(&npl, 0, sizeof(tagNotifyPlayerLogin));
			npl.cProtocol = s2c_notifypl_ayerlogin_hfkqotivbhd;

		   if (1 != cResult && pPA->bLeave)
		   {//如果是离线的　就　发往源ｇｓ　踢角色下线
			   _NoticeRoleOffLine_ToGameServer((const void *)(&pPA->pDataBuffer[1]),pPA->uSelServer,pPA->nDataLen);
			   char nInfo[128]={0};
			   sprintf(nInfo,"------角色在线 离线:%d,Reg:%u Gs:%u -----",(int)pPA->bLeave,pPA->uSelServer,pPA->nDataLen);
			   AddOutputIpList(hbishopOutput,nInfo);
			   npl.nIPAddr = 0; 
			   npl.nPort	= 1;
		   }
		   else if (1 == cResult)
		   {//ｇｓ没有开放的或满员
			   npl.nIPAddr = 0; 
			   npl.nPort   = 2;
		   }
		   else
		   {
			   npl.nIPAddr = 0; 
			   npl.nPort   = 4;
		   }
			
			npl.bPermit = false; //已经登陆
			m_pPlayerServer->SendData(m_lnIdentityID,(const void *)&npl,sizeof(tagNotifyPlayerLogin));
		}

		SAFE_RELEASE(pRetBuffer);
		m_theDataQueue[enumOwnerRoleSvr].Detach(s2c_rolehjdkalritopq_getroleinfo_result);

		return nNextTask;
	}

	return enumRepeat;
}

void CGamePlayer::_NoticeRoleOffLine_ToGameServer( const void *pData,UINT nRegSerIdx,unsigned int nGsNunber)
{
	const TRoleData *pRoleData = (const TRoleData *)(pData);
	
	IGServer *pGServer = NULL;
	pGServer = CGameServer::GetSerVerIdByIndex(nRegSerIdx,nGsNunber);
	//AddOutputStringPlayer(hbishopOutput ,"---_SyncRoleInfo_ToGameServerFs:开始---");	//hCtrlPlayr
	if (pGServer)
	{
		tagQueryMapInfo qmi;
		qmi.cProtocol = s2c_querymapinfo;
		qmi.nProtoKind = enum_COMMAND_ID_KICKOUTROLE;
		if (pRoleData->BaseInfo.szName[0] !='\0')
		  snprintf(qmi.nName, sizeof(qmi.nName), "%s", pRoleData->BaseInfo.szName);
		qmi.nKey      = 0;
		qmi.nVerX     = 0;
		qmi.nVerY     = 0;
		//CGameServer::SendData(m_lnIdentityID,(const void *)&qmi,sizeof(tagQueryMapInfo));
		pGServer->SendData(pGServer->GetID(),(const void *)&qmi,sizeof(tagQueryMapInfo));
		//AddOutputStringPlayer(hbishopOutput ,"---_NoticeRoleOffLine_ToGameServer:成功！---");	//hCtrlPlayr
	}
}
//服务器上 获取人数数据 同步到BISHOP
bool CGamePlayer::_SyncRoleInfo_ToGameServerFs( const void *pData, size_t dataLength,UINT nCurSerIdx)
{
	bool ok = false;
	BYTE nNum=0;
	//ASSERT( pData );
	//SetPlayerRevivePos();

	if (hbishopOutput && pData==NULL)
	{
		 AddOutputStringPlayer(hbishopOutput ,"_SyncRoleInfo_ToGameServerFs =空包");	//hCtrlPlayr
         return  ok;
	}
	TRoleData *pRoleData = (TRoleData *)(pData);//const
	IGServer *pGServer = NULL;
	if(strcmp(m_sAccountName.c_str(), pRoleData->BaseInfo.caccname)==0)
	{//账号和存档的账号相同
	  if (m_enterMapIdx)
	  {
		 // pRoleData->BaseInfo.cUseRevive = 1;
		 // pRoleData->BaseInfo.irevivalid = m_enterMapIdx;
		 // pRoleData->BaseInfo.irevivalx  = 10;
		  pGServer = CGameServer::QueryServer( m_enterMapIdx,pRoleData->BaseInfo.isWaiGua);
		  nNum=0;
	  }
	  else if (pRoleData->BaseInfo.cUseRevive)
	  {//重生点
		  pGServer = CGameServer::QueryServer( pRoleData->BaseInfo.irevivalid,pRoleData->BaseInfo.isWaiGua);
		  nNum=1;
	  }
	  else
	  {//下线地图
		  pGServer = CGameServer::QueryServer( pRoleData->BaseInfo.ientergameid,pRoleData->BaseInfo.isWaiGua);
		  nNum=2;
	  }

	  if (pGServer==NULL)  //如果是空连的话 再连一次 固定数据
	  {//默认的
		  pGServer = CGameServer::QueryServer(m_nTwoRevivePos,pRoleData->BaseInfo.isWaiGua);
		  nNum=4;
	  }
	}

 //   if (pRoleData->BaseInfo.iRoleParm6!=0 && pRoleData->BaseInfo.iRoleParm6!=pRoleData->BaseInfo.isWaiGua)
	//{
	//	pGServer=CGameServer::GetSerVerIdByIndex(pRoleData->BaseInfo.iRoleParm6);
	//	if (!pGServer)  //如果跨服的目标服务器不存在　就不垮了
	//	{
	//	   //pRoleData->BaseInfo.iRoleParm6 = 0;
 //          pGServer=CGameServer::GetSerVerIdByIndex(nCurSerIdx);
	//	}
	//}  
	//else
	//    pGServer=CGameServer::GetSerVerIdByIndex(nCurSerIdx);
	
	/*if (pRoleData->BaseInfo.cUseRevive)  //使用重生点登入
	{
		pGServer = CGameServer::QueryServer(pRoleData->BaseInfo.irevivalid);   //存档地图ID
		nNum=1;
	}
	else 
	{                                //不使用重生点登入
		pGServer = CGameServer::QueryServer(pRoleData->BaseInfo.ientergameid); //存档地图ID
		nNum=2;
	}
	
	if (pGServer==NULL)  //如果是空连的话 再连一次 固定数据
	{
	     pGServer = CGameServer::QueryServer(pRoleData->BaseInfo.irevivalid);
		 nNum=3;
	 }
	if (pGServer==NULL)  //如果是空连的话 再连一次 固定数据
	{
	     pGServer = CGameServer::QueryServer(m_nTwoRevivePos);
		 nNum=4;
	} */

/*
	char nReg[64];
	//_itoa(m_nTwoRevivePos,nReg,10);
	if (hCtrlPlayr==NULL)
	   sprintf(nReg,"本次登录点-空:%d--%d",m_nTwoRevivePos,hCtrlPlayr);
	   else
	   sprintf(nReg,"本次登录点-有:%d--%d",m_nTwoRevivePos,hCtrlPlayr);
    MessageBox(0, nReg,"重设重生点提示:",MB_OK);
*/
	if (pGServer)
	{
		//MessageBox(0, "开始连接gs","提示:",MB_OK);

		if (m_enterMapIdx && pRoleData->BaseInfo.cFightMode)
		    pRoleData->BaseInfo.cFightMode = 0;

		ASSERT(pRoleData->BaseInfo.szName[0] !='\0');
		CGamePlayer::Add((const char *)pRoleData->BaseInfo.szName,(IPlayer *)this);
		/*char nTempStr[64];
		ZeroMemory(nTempStr,sizeof(nTempStr));
		sprintf(nTempStr,pRoleData->BaseInfo.szName);
        CGamePlayer::AddByDwidx(g_FileName2Id(nTempStr),(IPlayer *)this);*/
		
		pGServer->Attach(m_sAccountName.c_str());
		
		m_nAttachServerID = pGServer->GetID();//获取连接的网络号
		
		m_theDataQueue[enumOwnerPlayer].Empty();
        //服务器上 获取人数数据 将结果返回到BISHOP  产生对应的登陆网络唯一编号
		ok = pGServer->DispatchTask( CGameServer::enumSyncRoleInfo,pData, dataLength, max(m_nExtPoint, 0),m_nLeftTime,m_nVipType,m_nExtGameLeftTime,m_enterMapIdx,0);
		m_nExtPoint = 0;//用完就清掉
		m_nExtGameLeftTime =0;
		
	}

   	if (hbishopOutput)
	{
		char msg[512]={0};
	  if (ok)
	     sprintf(msg,"Succes - Acc:%s,Name:%s,ireid:%d,ienter:%d,RePos:%d,iParm6:%d,nItem:%d isWaiGua:%u ent:%d",pRoleData->BaseInfo.caccname,pRoleData->BaseInfo.szName,pRoleData->BaseInfo.irevivalid,pRoleData->BaseInfo.ientergameid,m_nTwoRevivePos,pRoleData->BaseInfo.iRoleParm6,pRoleData->nItemCount,pRoleData->BaseInfo.isWaiGua,m_enterMapIdx);
	  else
         sprintf(msg,"Fail - Acc:%s,Name:%s,ireid:%d,ienter:%d,RePos:%d,iParm6:%d,nItem:%d isWaiGua:%u ent:%d",pRoleData->BaseInfo.caccname,pRoleData->BaseInfo.szName,pRoleData->BaseInfo.irevivalid,pRoleData->BaseInfo.ientergameid,m_nTwoRevivePos,pRoleData->BaseInfo.iRoleParm6,pRoleData->nItemCount,pRoleData->BaseInfo.isWaiGua,m_enterMapIdx);
	    AddOutputStringPlayer(hbishopOutput,msg); //hCtrlPlayr
		//bs_LogFile.gTraceLogFile(msg,sizeof(msg));
	}

	m_enterMapIdx = 0;
	//CloseHandle(hCtrlPlayr);
	//CloseHandle(hWndPlayr);

	return ok;
}

void pcharToDouble(const char* ch, double& res)
{
    sscanf(ch,"%lf",&res);
}

void CGamePlayer::SetPlayerRevivePos()
{
	//char nReg[8];
	//_itoa(m_nTwoRevivePos,nReg,10);
    //MessageBox(0, nReg,"重设重生点提示:",MB_OK);
	UINT nValue = 0;
	BOOL bTranslated = TRUE;
	nValue = ::GetDlgItemInt(hMianHwd,IDC_EDIT_TWO_REVIVE, &bTranslated, FALSE );
	m_nTwoRevivePos=nValue;

	nValue = ::GetDlgItemInt(hMianHwd,IDC_EDIT_FORBITOPENUM, &bTranslated, FALSE );
    m_nForBitOpenNum=nValue;
	nValue = ::GetDlgItemInt(hMianHwd,IDC_EDIT_SERVER, &bTranslated, FALSE );
	m_nForBitOpenSer=nValue;

	if (BST_CHECKED == ::IsDlgButtonChecked(hMianHwd,IDC_CHECK_OPEN_YUJIAN))
	    __m_nIsOpenYuJian = true;
	else
		__m_nIsOpenYuJian = false;

	char szBuffer[16]={0};
	::GetDlgItemText(hMianHwd,IDC_EDIT_BISHOPVER,szBuffer,16);
	 pcharToDouble(szBuffer,m_nBishopVer);

}

void CGamePlayer::SetBishopVer()
{
	//char nReg[8];
	//_itoa(m_nTwoRevivePos,nReg,10);
    //MessageBox(0, nReg,"重设重生点提示:",MB_OK);
	//double nValue =0.0;
	//BOOL bTranslated = TRUE;
	//::FindWindow(NULL,WindowsName) 
   	 //  hWndPlayr   = ::FindWindow(NULL,"Bishop - [enable]");
   	  // hCtrlPlayr  = ::GetDlgItem(hWndPlayr, IDC_LIST_GAMESERVER2 );
	char szBuffer[16]={0};
	::GetDlgItemText(hMianHwd,IDC_EDIT_BISHOPVER,szBuffer,16);
	//SetTwoRevivePos(nValue); szBuffer  atof()
	pcharToDouble(szBuffer,m_nBishopVer);

	//CloseHandle(mhWndPlayr);
	//CloseHandle(mhCtrlPlayr);
	//m_nBishopVer=atof(szBuffer);
	//char msg[64];
	//_itoa(nValue,nReg,10);
	//sprintf(msg,"地图重生点(%d)设置成功,主句柄:%d,类表句柄:%d!",m_nTwoRevivePos,hWndPlayr,hCtrlPlayr);
	//AddOutputStringPlayer(hCtrlPlayr,msg);
}


//等待GS通知 可以登陆GS了
UINT CGamePlayer::WaitForGameSvrPermit()
{
	//获取队列数据包
	CBuffer *pRetBuffer = m_theDataQueue[enumOwnerPlayer].Attach(s2c_notifypl_ayerlogin_hfkqotivbhd);

	if (pRetBuffer && m_pPlayerServer)
	{
		UINT nNextTask = enumError;

		tagNotifyPlayerLogin *pNPL = (tagNotifyPlayerLogin *)pRetBuffer->GetBuffer();

#ifdef	CONSOLE_DEBUG
		cprintf( "CGamePlayer::WaitForGameSvrPermit Notify player to login gameserver\n" );
	OutputDebugString("CGamePlayer::WaitForGameSvrPermit Notify player to login gameserver\n" );
#endif

		if (pNPL->bPermit)
		{
			m_bAutoUnlockAccount = false; //开锁
			nNextTask = enumToNextTask;
		}

		m_pPlayerServer->SendData(m_lnIdentityID,pRetBuffer->GetBuffer(),pRetBuffer->GetUsed());
		
		if (hbishopOutput)
		{
			char msg[128];
			//sprintf(msg,"---GS同意网络:%d登陆,发往客户端---",m_lnIdentityID);
			sprintf(msg,"---GS agrees with the network: %d logs in, sent to client---",m_lnIdentityID);
			AddOutputStringPlayer(hbishopOutput,msg); //hCtrlPlayr
		}

		SAFE_RELEASE(pRetBuffer);
		m_theDataQueue[enumOwnerPlayer].Detach(s2c_notifypl_ayerlogin_hfkqotivbhd);

		return nNextTask;
	}
	return enumRepeat;
}

bool CGamePlayer::NewAttach( unsigned int nRoleDwID)
{
	if (nRoleDwID>=0)
	{
		m_RoleNameDwID = nRoleDwID;

		return true;
	}

	return false;
}

bool CGamePlayer::Attach( const char *pRoleName )
{
	if ( pRoleName && pRoleName[0] )
	{
		m_sRoleName = pRoleName;

		return true;
	}

	return false;
}

bool CGamePlayer::Add( const char *pRoleName, IPlayer *pPlayer )
{
if ( NULL == pRoleName || NULL == pPlayer || !pRoleName[0] )
{
ASSERT( FALSE );

return false;
}

if ( pPlayer )
{
CCriticalSection::Owner locker( CGamePlayer::m_csMapSP );

std::pair< stdMapSP::iterator, bool > result = 
m_sthePlayerTable.insert( stdMapSP::value_type( pRoleName, pPlayer ) );

if ( result.second )
{
return pPlayer->Attach( pRoleName );
}
}

return false;
}

bool CGamePlayer::AddByDwidx(unsigned int RoleNameID, IPlayer *pPlayer )
{
	if ( RoleNameID<0 || NULL == pPlayer)
	{
		return false;
	}

	if (pPlayer)
	{
		CCriticalSection::Owner locker(CGamePlayer::m_csMapSP);
		char nTempStr[64];
		ZeroMemory(nTempStr,sizeof(nTempStr));
		sprintf(nTempStr,"%u",RoleNameID);
		std::pair< stdMapSP::iterator, bool > result = 
			m_sthePlayerTable.insert( stdMapSP::value_type(nTempStr,pPlayer));

		if ( result.second )
		{
			return pPlayer->NewAttach(RoleNameID);
		}
	}

	return false;
}



bool CGamePlayer::DelByDwid(unsigned nRoleNameDwID)
{
	if ( nRoleNameDwID<0)
	{
		return false;
	}
	    char nTempstr[64];
		ZeroMemory(nTempstr,sizeof(nRoleNameDwID));
		sprintf(nTempstr,"%u",nRoleNameDwID);
		CCriticalSection::Owner locker( CGamePlayer::m_csMapSP );

		stdMapSP::iterator it;

		if ( m_sthePlayerTable.end() != ( it = m_sthePlayerTable.find( nTempstr ) ) )
		{
			IPlayer *pPlayer = ( IPlayer * )( ( *it ).second );

			ASSERT( pPlayer );

			m_sthePlayerTable.erase( it );

			return true;
		}
	return false;
}

bool CGamePlayer::Del( const char *pRoleName )
{
	if ( !pRoleName || !pRoleName[0] )
	{
		return false;
	}

	{
		CCriticalSection::Owner locker( CGamePlayer::m_csMapSP );
		
		stdMapSP::iterator it;
		
		if ( m_sthePlayerTable.end() != ( it = m_sthePlayerTable.find( pRoleName ) ) )
		{
			IPlayer *pPlayer = ( IPlayer * )( ( *it ).second );
			
			ASSERT( pPlayer );
			
			m_sthePlayerTable.erase( it );
			
			return true;
		}
	}

	return false;
}

IPlayer *CGamePlayer::Get(char *pRoleName)
{
	if ( !pRoleName )
	{
		return NULL;
	}
	/*char nTempRoleName[64];
	ZeroMemory(nTempRoleName,sizeof(nTempRoleName));
	sprintf(nTempRoleName,"%u",g_FileName2Id(pRoleName));*/

	CCriticalSection::Owner locker(CGamePlayer::m_csMapSP);

	stdMapSP::iterator it;

	if (m_sthePlayerTable.end() != (it = m_sthePlayerTable.find(pRoleName)))
	{
		return (IPlayer *)((*it).second);
	}

	return NULL;
}

void AddOutputStringPlayer(HWND hListCtrl, char* aStr)
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

void AddOutputIpList(HWND hListCtrl, char* aStr)
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
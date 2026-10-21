/********************************************************************
	created:	2003/04/08
	file base:	GamePlayer
	file ext:	h
	author:		liupeng
	
	purpose:	
*********************************************************************/
#ifndef __INCLUDE_GAMEPLAYER_H__
#define __INCLUDE_GAMEPLAYER_H__

#include "IPlayer.h"
#include "ICommand.h"

#include "CriticalSection.h"
#include "tstring.h"
#include "Buffer.h"

#include "IClient.h"
#include "RainbowInterface.h"
#include "IServer.h"
#include "HeavenInterface.h"

#include "IGServer.h"
#include "GameServer.h"
#include "BishopLoginAttemptGate.h"
#include "BishopAccountClaimSession.h"

#include "PlayerCreator.h"

#include <vector>
#include <map>
#include <list>

using namespace std;

/*
 * class CGamePlayer
 */
class CGamePlayer : public IPlayer
{
public:

	CGamePlayer( UINT nIdentityID = ( UINT )( -1 ) );

	virtual ~CGamePlayer();

	/*
	 * Task option
	 */
	enum
	{		
		/*
		 * Task command
		 */
		enumLogin = 0,
		enumSelAddDelRole,
		enumLoginCreateRole,
		enumLoginSelectRole,
		enumLoginDeleteRole,
		
		enumSafeClose,

		/*
		 * Task status
		 */
		enumToGotoTask,
		enumToNextTask,
		enumCompleted,
		enumRepeat,
		enumError,
		enumNone
	};
	
	enum enumCDataQueueOwner
	{
		enumOwnerAccSvr = 0,
		enumOwnerRoleSvr,
		enumOwnerPlayer,
		//enumOwnerExitGame, //退出游戏
		enumOwnerTotal
	};

	virtual bool DispatchTask( UINT nTaskID );
	virtual bool IsWorking();
	virtual bool Run();
	virtual bool AppendData( UINT nOwner, const void *pData, size_t dataLength );
	
	static bool SetupGlobalAllocator( size_t bufferSize, size_t maxFreeBuffers );

	virtual bool Attach( const char *pRoleName );
	virtual bool NewAttach(unsigned int nRoleDwID);
	virtual bool IsActive() { return m_bActiveStatus; };
	virtual bool Active();
	virtual bool Inactive();

	static bool Add( const char *pRoleName, IPlayer *pPlayer );
	static bool AddByDwidx(unsigned int RoleNameID, IPlayer *pPlayer );
	static bool Del( const char *pRoleName );
	static bool DelByDwid(unsigned nRoleNameDwID);
	static IPlayer *Get(char *pRoleName );  // const 

	static void ATTACH_NETWORK( IClient *pAccSvrClient, 
				IServer *pPlayerServer, 
				IClient	*pDBRoleClient );

	static void DETACH_NETWORK();

	static LONG GetCapability() {return ::InterlockedExchange(&m_slnIdentityCounts,m_slnIdentityCounts);}
	static LONG GetWorking() {return ::InterlockedExchange(&m_lnWorkingCounts,m_lnWorkingCounts);}

	//void SetTwoRevivePos( unsigned short nTwoRevivePos ) { m_nTwoRevivePos = nTwoRevivePos;}
	void SetPlayerRevivePos();
	void SetBishopVer();
	int  getlnIdentityID(){return m_lnIdentityID;}

private:

	static CBuffer::Allocator	m_theGlobalAllocator;
    
	static LONG m_slnIdentityCounts;
	INT		m_lnIdentityID;
	
	static LONG m_lnWorkingCounts;

	UINT m_nExtPoint;
	UINT m_nExtGameLeftTime;
	int  m_nVipType;  //会员类型
	UINT m_nLeftTime;
    unsigned short 	m_nTwoRevivePos;
    double 	m_nBishopVer;
	bool __m_nIsOpenYuJian;
	unsigned short 	m_nForBitOpenNum; //限制多开的数量
	unsigned int    m_nForBitOpenSer;
	char  m_ClientIp[64];
	//HWND hWndPlayr; 
	//HWND hCtrlPlayr;
//    static CBishopApp	m_theGetMain;  
	/*
	 * Task system
	 */
	class CTask
	{
	public:

		explicit CTask( CGamePlayer *pReceiver, UINT nTaskID );
		~CTask();

		typedef UINT ( CGamePlayer::* Action )();

		size_t	AddCmd( Action pFun );
		
		UINT	Execute();
		void	Reset() { m_indexCmd = 0; }

	protected:

		CTask();

		//CCriticalSection	m_csTask;

	private:

		CGamePlayer *m_pReceiver;

		typedef vector< ICommand * > stdVector;

		stdVector	m_stdCommand;
		size_t		m_indexCmd;

		UINT		m_nTaskProgID;

	};

	CTask	m_theLoginTask;

	CTask	m_theSelAddDelTask;

	CTask	m_theLoginCreateRoleTask;
	CTask	m_theLoginSelectRoleTask;
	CTask	m_theLoginDeleteRoleTask;

	CTask	m_theSafeCloseTask;

	void SetCurrentTask( LONG nID ) { /* m_nCurrentTaskID = nID; */ ::InterlockedExchange( &m_nCurrentTaskID, nID ); }
	UINT GetCurrentTask() { return /* m_nCurrentTaskID; */ ::InterlockedExchange( &m_nCurrentTaskID, m_nCurrentTaskID ); }

	LONG m_nCurrentTaskID;
	BishopLoginAttemptGate m_loginGate;
	BishopAccountClaimSession m_accountClaimSession;
	
	void InitTaskProcessor();

	/*
	 * Information
	 */

	_tstring	m_sAccountName;
	_tstring	m_sPassword;
	_tstring	m_sRoleName;
	unsigned int m_RoleNameDwID;
	UINT m_selServer;
	unsigned int m_enterMapIdx;

    bool    m_bUseSuperPassword;
    _tstring	m_sSuperPassword;
    _tstring	m_sDelRoleName;

	UINT	m_nAttachServerID;
	
	bool	m_bActiveStatus;

	UINT	m_dwTaskBeginTimer;
	UINT	m_dwTaskTotalTimer;
	
	bool	m_bAutoUnlockAccount;

	static CCriticalSection CGamePlayer::m_csMapSP;

	typedef map< _tstring, IPlayer * > stdMapSP;

	static stdMapSP	m_sthePlayerTable;

	static const int s_nRoleListCount;
	static const int s_nLoginTimeoutTimer;
	static const int s_nProcessTimeoutTimer;

//	static IClient *m_pAccSvrClient;
	static IServer *m_pPlayerServer;
	static IClient *m_pDBRoleClient;

	/*
	 * class CDataQueue
	 */
	class CDataQueue
	{
	public:

		explicit CDataQueue( size_t bufferSize = 1024 * 128, size_t maxFreeBuffers = 1 );
		
		~CDataQueue();

		/*
		 * Function
		 */
		bool		AddData( LONG lnID, const BYTE *pData, size_t datalength );

		CBuffer *Attach( LONG lnID );
		void		Detach( LONG lnID );

		void		Clear( LONG lnID );

		void		Empty();

	private:
		/*
		 * Data
		 */
		typedef map< LONG, CBuffer * > stdDataMap;

		stdDataMap									m_theData;

		CBuffer::Allocator	m_theDQAllocator;

		CCriticalSection		m_csQueue;
	};

	CDataQueue	m_theDataQueue[enumOwnerTotal];

	CPackager			m_thePackager;

	static CPlayerCreator					m_thePlayerCreator;
	
protected:

	/*
	 * Helper function
	 */
	bool TaskCompleted();

	bool SmallPackProcess( UINT nOwner, const void *pData, size_t dataLength );
	bool LargePackProcess( UINT nOwner, const void *pData, size_t dataLength );
	
	bool DispatchTaskForAccount( const void *pData, size_t dataLength );
	bool DispatchTaskForDBRole( const void *pData, size_t dataLength );
	bool DispatchTaskForPlayer( const void *pData, size_t dataLength );

	/*
	 * Task function
	 */
	// {{

		/*
		 * Login
		 */
		UINT WaitForAccPwd();
		UINT QueryAccPwd();
		UINT VerifyAccount();
		UINT QueryRoleList();
		UINT ProcessRoleList();
		UINT SelAddDelRole();
		UINT DelRole_WaitForVerify();
		
		UINT WaitForCreateResult();

		UINT WaitForDeleteResult();

		UINT ProcessRoleInfo();
		UINT WaitForGameSvrPermit();

		/*
		 * Logout
		 */

	// }}

	UINT SafeClose();

	/*
	 * Help function for task
	 */
	void _ClearTaskQueue();

	bool _VerifyAccount_ToPlayer( UINT nQueryResult , unsigned long nLeftTime, unsigned long nExePiont=0);
	bool _QueryRoleInfo_ToDBRole( const char *pRoleName,const char *pAccName=NULL);
	bool _SyncRoleInfo_ToGameServerFs( const void *pData, size_t dataLength,UINT nCurSerIdx);

	void _NoticeRoleOffLine_ToGameServer( const void *pData,UINT nRegSerIdx,unsigned int nGsNunber);

	bool _CreateNewPlayer_ToDBRole( const char *pRoleName,
		int nRoleSex, 
		int nRoleClass, 
		unsigned short nMapID,unsigned int nServNoIndex=1);

	UINT _DeleteRole_ToDBRole( const void *pData, size_t dataLength );

	bool _UnlockAccount();
	bool FreezeMoney( const char *pAccountName, LONG nExtPoint,LONG nGameExtTime);
	
};
void AddOutputStringPlayer(HWND hListCtrl, char* aStr);//添加操作输出文字
void AddOutputIpList(HWND hListCtrl, char* aStr);

inline const char* _ip2a(DWORD ip) { in_addr ia; ia.s_addr = ip; return inet_ntoa(ia); }
inline DWORD _a2ip(const char* cp) { return inet_addr(cp); }

#endif // __INCLUDE_GAMEPLAYER_H__
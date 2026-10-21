/********************************************************************
	created:	2003/05/11
	file base:	GameServer
	file ext:	h
	author:		liupeng
	
	purpose:	
*********************************************************************/
#ifndef __INCLUDE_GAMESERVER_H__
#define __INCLUDE_GAMESERVER_H__

#include "Buffer.h"

#include "IGServer.h"

#include "KProtocol.h"

#include "IClient.h"
#include "RainbowInterface.h"
#include "IServer.h"
#include "HeavenInterface.h"

#include "CriticalSection.h"
#include "KCriticalSection.h"

#include "Event.h"

#include "KProtocolDef.h"

#include <map>
#include <list>
#include <string>

/*
 * class CGameServer
 */


class CGameServer : public IGServer
{
public:

	typedef struct tagNetworkInterface
	{
		IServer *pServer;
		IClient	*pClient;
		
		HWND	hwndContainer;
		std::string trustedGameServerIPs;
		
	}NI, NEAR *PNI, FAR *LPNI;
	
	CGameServer( IServer *pGameSvrServer,
		IClient	*pAccountClient,
		UINT nIdentityID = (UINT)(-1));
	
	virtual ~CGameServer();
	CGameServer(){};  //后来加的

	virtual bool Create();
	virtual bool Destroy();
	
	virtual bool Attach( const char *pAccountName );

	virtual void SendText( const char *pText, int nLength, UINT uOption,BOOL nIsSavePai=FALSE);

	virtual void SendData(  const unsigned long ulnClientID, const void * const pData, const size_t datalength );

	virtual bool AnalyzeRequire( const void *pData, size_t datalength );

	enum enumTask
	{
		enumTaskProtocol = 0x5,

		enumSyncRoleInfo,
		enumPlayerLogicLogout,
	};

	virtual bool DispatchTask( UINT nTask, const void *pData, size_t datalength, DWORD nData,DWORD nLeftTime,int nVipType=-1,DWORD nExtGameLeftTime=0,unsigned int nTempEnTerMapIndx=0,unsigned int nTempKeyId=0);

	static bool SetupGlobalAllocator( size_t bufferSize, size_t maxFreeBuffers );

	static void __stdcall GameSvrEventNotify( LPVOID lpParam,
			const unsigned long &ulnID,
			const unsigned long &ulnEventType );

	static bool Begin( IServer *pGameSvrServer );
	static void End();
	
	static void SendToAll( const char *pText, int nLength, UINT uOption,BOOL nIsSavePai=FALSE );
	
	static IGServer *QueryServer( UINT nMapID,unsigned int nSerNoIndex=1);
	static IGServer *GetServer( size_t nID );

	static IGServer *GetSerVerIdByIndex( UINT nRegSerIndex,UINT nGsSerNumber);

	virtual size_t GetID() { return m_lnIdentityID;};            //获取网络连接号 
	virtual size_t GetContent();
	virtual size_t GetCapability() { return m_dwCapability; };   //获取GS 最大人数限制
	virtual size_t GetSerNoIndex() { return m_serVerNoIndex;};  //获取GS 索引号码
	virtual size_t GetSerGsNumber() { return m_serGsNumber; };   //获得同一个大区的gs编号
	virtual int     getRoleLock(char* szRole,RoleLoginInfo *resInfo);
	
	virtual size_t GetIP( BYTE cType ) { if ( INTERNET_IP == cType ) { return m_nServerIP_Internet; } return m_nServerIP_Intraner; };
	virtual size_t GetIndentify() { return m_lnIdentityID; };   //获取网络连接号 

	void SetTwoRevivePos( unsigned short nTwoRevivePos ) { m_nTwoRevivePos = nTwoRevivePos; }
	
protected:

	bool _QueryMapID();
	bool _QueryGameSvrInfo();
	
	bool _RegisterAccount( const void *pData, size_t datalength );
	bool _UpdateMapID( const void *pData, size_t datalength );
	bool _UpdateGameSvrInfo( const void *pData, size_t datalength );
	int  _GetLocalIpAddress(DWORD *pIntranetAddr, DWORD *pInternetAddr);

	bool _NotifyPlayerLogin( const void *pData, size_t datalength );
	bool _NotifyEnterGame( const void *pData, size_t datalength );
	bool _NotifyLeaveGame( const void *pData, size_t datalength );
	bool _RequestSvrIP( const void *pData, size_t datalength );
	bool _Rc2smsginfo( const void *pData, size_t datalength );

	bool c2smsginfo_all( const void *pData, size_t datalength );
	bool gm_synroleaction( const void *pData, size_t datalength );
	bool gm_synroleaction_result( const void *pData, size_t datalength );
	bool c2s_itemsynaction( const void *pData, size_t datalength );
	bool c2s_itemsynaction_result( const void *pData, size_t datalength );
	bool _LockOrUnlockRole( const void *pData, size_t dataLength );


	bool _SyncRoleInfo( const void *pData, size_t datalength, DWORD nData, DWORD nLeftTime,int nVipType=-1,DWORD nExtGameLeftTime=0,unsigned int nTempEnTerMapIndx=0,unsigned int nTempKeyId=0);
	bool _PlayerLogicLogout( const void *pData, size_t datalength );
	
	bool LargePackProcess( BYTE cProtocol, const void *pData, size_t datalength );
	bool SmallPackProcess( BYTE cProtocol, const void *pData, size_t datalength );

	bool PushAccount( const char *pAccountName );
	bool PopAccount( const char *pAccountName, bool bUnlockAccount, LONG nExtPoint,LONG nGameExtTime);

	bool ConsumeMoney( const char *pAccountName );
	bool FreezeMoney( const char *pAccountName, LONG nExtPoint,LONG nGameExtTime);

private:

	/*
	 * Map ID convert to a game server index
	 */
	static CCriticalSection	m_csMapIDAction;
	typedef std::list< IGServer * >		        stdServerList;
	typedef std::map< size_t, stdServerList >	stdMapIDConvert; //
	static stdMapIDConvert						m_theMapIDConvert;
	static bool RegisterServer(UINT nID, IGServer *pGServer);

	typedef std::map<UINT, UINT >		stdClientSerVer;
	static stdClientSerVer				m_ClientSerVer;
	
	
	/*
	 * Process protocol function
	 */
	typedef bool (CGameServer::*ProcessProtocolFun[c2s_end])(const void *pData, size_t datalength);

	ProcessProtocolFun	m_theProcessProtocolFun;

private:

	static CBuffer::Allocator	m_theGlobalAllocator;

	static LONG		m_slnIdentityCounts;
	LONG			m_lnIdentityID;
	//char            m_ClentIp[64];
	
	IServer			*m_pGameSvrServer;
//	IClient			*m_pAccountClient;
	
	CPackager	m_thePackager;

	_tstring	m_sServerIPAddr_Intraner;
	_tstring	m_sServerIPAddr_Internet;

	unsigned long					m_nServerIP_Intraner;  //数据库联网IP
	unsigned long					m_nServerIP_Internet;  //数据库联网IP
	
	unsigned short					m_nServerPort;

    unsigned short 	                m_nTwoRevivePos;
	DWORD							m_dwCapability;
	unsigned int                    m_serVerNoIndex;
	unsigned int                    m_serGsNumber;

	/*
	 * Listen and accept data from network
	 */
	static CEvent		m_shQuitEvent;
	static CEvent		m_shStartupManagerThreadEvent;

	static HANDLE m_shManagerThread;

	static DWORD WINAPI	ManagerThreadFunction( void *pParam );

	/*
	 * Game server information
	 */	
	static CCriticalSection	m_csGameSvrAction;
	
	typedef std::map< size_t, IGServer * >			stdGameSvr;

	static stdGameSvr								m_theGameServers;//服务器容器
	
	/*
	 * Somebody attach the gameserver be save in this table
	 */
	//CCriticalSection			
	KCriticalSection_ 	m_csAITS;

	typedef std::map< std::string, void * >			stdAccountAttachIn;

	stdAccountAttachIn								m_theAccountInThisServer;

	bool AttatchAccountToGameServer( const char *pAccountName );
	bool HaveAccountInGameServer( const char *pAccountName );
	bool DetachAccountFromGameServer( const char *pAccountName );



	static CCriticalSection	m_csCR;

	typedef std::map< std::string, RoleLoginInfo >	stdRoleLockMap;  //size_t
	static stdRoleLockMap m_csRoleLock;

	//int  getRoleLock(char* szRole,RoleLoginInfo *resInfo);
	int  IsRoleLock(char* szRole,RoleLoginInfo *resInfo);
	bool IsRoleLockBySelf(char* szRole);
	bool UnlockRoleSelf(char* szRole);
	void UnlockAllRole(size_t ID);

};

void AddOutputSerPlayer(HWND hListCtrl, char* aStr);//添加操作输出文字
void AddOutputSerIpList(HWND hListCtrl, char* aStr);

//inline const char* _ip2a(DWORD ip) { in_addr ia; ia.s_addr = ip; return inet_ntoa(ia); }
//inline DWORD _a2ip(const char* cp) { return inet_addr(cp); }

#endif // __INCLUDE_GAMESERVER_H__
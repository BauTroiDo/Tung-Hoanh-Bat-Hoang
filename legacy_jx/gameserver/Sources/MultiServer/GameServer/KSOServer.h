#ifndef KSOServerH
#define	KSOServerH

#include <map>
#include <unordered_map>
#include "KEngine.h"
#include "IClient.h"
#ifndef _STANDALONE
#include "RainbowInterface.h"
#include "HeavenInterface.h"
#include "..\..\Core\src\CoreServerShell.h"
#include "UsesWinsock.h"
#include "Buffer.h"
//using OnlineGameLib::Win32::CPackager;

#else
#include "CoreServerShell.h"
#endif

#include "IServer.h"
#include "KTimer.h"
#include "KTransferUnit.h"
#include "RoleSaveRetryCache.h"

#include "KLinkArray.h"
///////////////////////////////////////////////////////////////

#define CHATSTATE "m_ChatState"
#define TONGSTATE "m_TongState"
#define TRANSTATE "m_TranState"
#define DATASTATE "m_pDataState"
#define GATWSTATE "m_GatewayState"

#define RNGJJGORUOGFG "/擔鍻%H镇廗"


//64bit
// The header pair decides the width of a MIRACL digit (mirdef.h: MIRACL 64,
// mr_utype __int64), so it has to be the pair that matches the library this
// configuration links - ms64.lib for x64, ms32.lib for Win32.  miracl.h and
// MIRACL32.H share the guard MIRACL_H, so exactly one of the two may be taken.
extern "C"
{
#ifdef _WIN64
#include "miracl.h"
#include "mirdef.h"
#else
#include "MIRACL32.H"
#include "MIRDEF32.H"
#endif
}


#ifndef MACRO_T_DEVICE_PROPERTY  
    #define MACRO_T_DEVICE_PROPERTY  
  
    #define PROPERTY_MAX_LEN    128 // 属性字段最大长度  
    typedef struct _T_DEVICE_PROPERTY  
    {  
        TCHAR szProperty[PROPERTY_MAX_LEN];  
    } T_DEVICE_PROPERTY;  
#endif  
  
#define WMI_QUERY_TYPENUM   7   // WMI查询支持的类型数  
  
#ifdef __cplusplus  
extern "C"  
{  
#endif  
  
/* 
功能：通过WMI获取设备属性 
参数说明： 
    iQueryType：需要查询的设备属性 
            0：网卡原生MAC地址 
            1：硬盘序列号 
            2：主板序列号 
            3：CPU ID 
            4：BIOS序列号 
            5：主板型号 
            6：网卡当前MAC地址 
    properties：存储设备属性值 
    iSize：可存储的最大设备个数 
返回值： 
     -1：不支持的设备属性值 
     -2：WMI连接失败 
     -3：不正确的WQL查询语句 
    >=0：获取的设备个数   
*/  
INT WMI_DeviceQuery( INT iQueryType, T_DEVICE_PROPERTY *properties, INT iSize );  
  
#ifdef __cplusplus  
}  
#endif
///////////////////////////////////////////////////////////////


//游戏状态
enum PLAYER_GAME_STATUS
{
	enumPlayerBegin = 0,
	enumPlayerSyncEnd,
	enumPlayerPlaying,
	enumPlayerExchangingServer,
	enumPlayerNone,
};

enum NetStatus
{
	enumNetUnconnect = 0,
	enumNetConnected,
};

struct GameStatus 
{
	/*GameStatus() 
	{
		nGameStatus = 0; 
		nNetStatus = 0;
		nExchangeStatus = 0;
		nPlayerIndex = 0;
		nSendPingTime = 0;
		nReplyPingTime = 0;
		//dwLastOperationTime=0;
	};*/
	int					nPlayerIndex;
	int					nGameStatus;
	int					nNetStatus;
	int					nExchangeStatus;
	DWORD				nSendPingTime;
	DWORD				nReplyPingTime;
	//int 	            dwLastOperationTime;
	int                 nNetidx;
	unsigned int        nErrorLoopCount;
	unsigned int        nPingLoopCount;
} ;

class  KSwordOnLineSever
{
private:
#ifdef _STANDALONE
	ZBuffer *net_buffer;
#endif
	int                 m_SerVerNo;
	int                 m_GsNumBer;
	int                 mTransSerVerNo;
    int                 TransPort;
	UINT                TransDwIp;
	int					m_nMaxPlayerCount;
	int					m_nPrecision;
	int	                m_snMaxBuffer;
	int	                m_snBufferSize;
	int					m_nMaxPlayer;
    int					m_nMaxGameStaus;
	int					m_nMaxClienLoop;
	//DWORD			    m_nGameLoop;
	DWORD			    m_nGameDay;
	int					m_nCurGameLoop;
	int					m_nServerPort;
	int					m_nGatewayPort;
	int					m_nDatabasePort;
	int					m_nTransferPort;
	int					m_nChatPort;
	int					m_nTongPort;
	DWORD				m_dwIntranetIp;
	DWORD				m_dwInternetIp;
	char				m_szGameServerIP1[16];
	char				m_szGameServerIP2[16];
	char				m_szGatewayIP[16];
	char				m_szDatabaseIP[16];
	char				m_szTransferIP[16];
	char				m_szChatIP[16];
	char				m_szTongIP[16];
	BOOL				m_bIsRunning;
//	BOOL				m_bSaveFlag;
//	int					m_nSaveCount;
	IServer*			m_pServer;
	IClient*			m_pGatewayClient;
	IClient*			m_pDatabaseClient;
	IClient*			m_pTransferClient;
	IClient*			m_pChatClient;
	IClient*			m_pTongClient;
	GameStatus*			m_pGameStatus;
	int                 m_errorCount;
	int                 m_pingCount;
	BOOL                m_IsWm;
	BOOL                inGo;

	BOOL                m_ChatState;
	BOOL                m_TongState;
	BOOL                m_TranState;
	BOOL                m_pDataState;
	BOOL                m_GatewayState;
	
	KLinkArray		m_FreeIdxNetStatus;	    //	可用表
	KLinkArray		m_UseIdxNetStatus;		//	已用表
	int				m_nListCurIdx;			//  用于 GetFirstPlayer 和 GetNextPlayer

	struct iCoreServerShell*	m_pCoreServerShell;
	//KTimer				m_Timer;
	typedef std::unordered_map<DWORD, KTransferUnit*>	IP2CONNECTUNIT;
	IP2CONNECTUNIT		m_mapIp2TransferUnit;
	JXRoleSaveRetry::Cache m_roleSaveRetryCache;
public:
	KSwordOnLineSever();
	~KSwordOnLineSever();
	TCHAR            m_curNetTime[64];
	char                m_parmName[32];
	//------------------网络链表操作------------
	HWND        nCurWindHwnd;
	KTimer		m_Timer;
	DWORD	    m_nGameLoop;
	BOOL	Init();
	int		FindSame(DWORD dwID);
	//int   FindNameID(LPSTR szName);            //服务器端用
	//int   FindByTongName(LPSTR szTongName);  //服务器端用
	//int   CheckLiXian(LPSTR szName);
	//int   FindClient(int nClient);             //根据连接号 查找玩家Playerid
	//int	FindPlayerID(DWORD dwID);
	int		GetFirstNetIdx();				     // 遍历所有玩家第一步
	int		GetNextNetIdx();				     // 遍历所有玩家下一步(这支函数必须在上一支调用之后才能调用)
	void    ClearNetStatus();
	VOID  CALLBACK   TimerProc(HWND hWnd,UINT nMsg,UINT nTimerid,DWORD dwTime) ;
    HWND    GetWindowHwnd(){return nCurWindHwnd;}
	//------------------------------------------
	BOOL				InitServer(char *parmName);
	void				    SetGo(BOOL n){inGo=n;};
	BOOL				GetGo(){return inGo;};

	IClient*            GetDataClient(){return m_pDatabaseClient;};
	IClient*            GetGatwClient(){return m_pGatewayClient;};
	IClient*            GetChatClient(){return m_pChatClient;};
	IClient*            GetTongClient(){return m_pTongClient;};
	IClient*            GetTranClient(){return m_pTransferClient;};

	void                ErrOut();
	BOOL				SetServerPort();
	BOOL				Breathe();
	int					GetNetStatus(const unsigned long lnID);
	void				SetNetStatus(const unsigned long lnID, NetStatus nStatus,bool isSave=false);
	void				SetRunningStatus(BOOL bStatus);
	void                SetS3RelayRunStatus(BOOL bStatus,int nType);
	void				PlayerLogoutGateway();
	void				Release();
//	int                 GetPCCode(char *code);
 //   int                 getMAC(char *mac);
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


		void EDPad_Encipher(char* pPlaintext, int nPlainLen);	    //一次一密码乱本译码函数
		void EDPad_Decipher(char* pCiphertext, int nCiphertextLen);	//一次一密码乱本解码函数
		bool JieMa_Decipher(char* pInTxtPass,char *pOutTxtPass);	//一次一密码乱本解码函数
	typedef std::unordered_map<std::string, BOOL>	CLIENTNETSTATE;
	CLIENTNETSTATE	m_mapClientNet;

	BOOL   GetClientNet(char * nClnet) 
	{
		if (m_mapClientNet.count(nClnet)>0)
			return m_mapClientNet[nClnet];

		return FALSE;
	}

	void   SetClientNet(char * nClnet,BOOL nState) 
	{
		m_mapClientNet[nClnet] = nState;
	}
	void    SendCilentPackData();
	void    SetClientConnetNet(int nIsThis);
//	int                 CheckKey(char *key);
private:
	int		            FindFree();
	void				MessageLoop();
/*
	void				GatewayLoop();
	void				DatabaseLoop();
	void				TransferLoop();
	void				ChatLoop();
	void				TongLoop();
	void				PlayerLoop();
	*/
	void				GatewayMessageProcess(const char* pChar, size_t nSize);
	void				DatabaseMessageProcess(const char* pChar, size_t nSize);
	void				DatabaseLargePackProcess(const char* pChar, size_t nSize);
	void				TransferMessageProcess(const char* pChar, size_t nSize);
	void				PlayerFsMessageProcess(const unsigned long lnID, const char* pChar, size_t nSize);
	void                PlayerFschatProcess(const unsigned long lnID, const char* pData, size_t dataLength);
	void				GatewayLargePackProcess(const void *pData, size_t dataLength);
	void				GatewaySmallPackProcess(const void *pData, size_t dataLength);
	void				GatewayBoardCastProcess(const char* pData, size_t dataLength);
	void				GwBoardCastProcess(const char* pData, size_t dataLength);
	void				TransferLargePackProcess(const void *pData, size_t dataLength, KTransferUnit* pUnit);
	void				TransferSmallPackProcess(const void *pData, size_t dataLength, KTransferUnit* pUnit);
	void				TransferAskWayMessageProcess(const char *pData, size_t dataLength);
	void				TransferLoseWayMessageProcess(const char *pData, size_t dataLength);
	void				ChatMessageProcess(const char *pChar, size_t nSize);
	void				ChatGroupMan(const void *pData, size_t dataLength);
	void				ChatSpecMan(const void *pData, size_t dataLength);

	BOOL				ConformAskWay(const void* pData, int nSize, DWORD *pdwID);
	void				MainLoop();
    void			    TongattackLoop();
	int					ProcessLoginProtocol(const unsigned long lnID, const char* pData, size_t dataLength);
	BOOL				ProcessSyncReplyProtocol(const unsigned long lnID, const char* pData, size_t dataLength);
	void				PingClient(const unsigned long lnID);
	void				ProcessPingReply(const unsigned long lnID, const char* pData, size_t dataLength);

	BOOL				SendGameDataToClient(const unsigned long lnID, const int nPlayerIndex);
	//void				SavePlayerData();
	BOOL				SavePlayerData(int nIndex, bool bUnLock);
	void                RetryPendingRoleSaves();
	void				ExitAllPlayer();
	void                ExitAllPlayerIngame();
	//void				PlayerLogoutGateway();
	void				PlayerExchangeServer();
	int 				GetLocalIpAddress(DWORD *pIntranet, DWORD *pInternet);

	// 帮会协议处理( relay 发过来的协议)
	void				TongMessageProcess(const char *pChar, size_t nSize);
	// 帮会协议处理(客户端通过帮会扩展协议发过来的协议)
	void				ProcessPlayerTongMsg(const unsigned long nPlayerIdx, const char* pData, size_t dataLength);

	BOOL				CheckPlayerID(unsigned long netidx, DWORD nameid);
	INT					GetPlayerID(unsigned long netidx);











};

#endif

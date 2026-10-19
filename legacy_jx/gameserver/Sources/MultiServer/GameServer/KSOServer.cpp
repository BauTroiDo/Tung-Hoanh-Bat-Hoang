#include "StdAfx.h"
#ifdef _STANDALONE
#ifndef __linux
#include <winsock2.h>
#include <malloc.h>
#else
#include <unistd.h>
#include <netdb.h>
#include <sys/time.h>
#endif
#include "KWin32.h"
#include "package.h"
#endif

#include "CRC32.h"
#include "..\..\Core\Src/KCoreData.h"
//#include "..\..\Core\Src/KGameglobalSet.h"

#ifdef _STANDALONE
#include "KCore.h"
#include "KPlayerSet.h"
#include "CRC32.c"
#endif

#ifndef WIN32
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#endif

#define TIME_FARM 200
#define TIME_FARM_H 100
#define _SERVER

#ifdef _STANDALONE
//	#ifdef WIN32
//		#define FAILED(x) ((x) != S_OK)
//	#else
	#define FAILED(x) ((x) <= 0)
//	#endif
#endif

#ifndef _STANDALONE
#include <crtdbg.h>
#include <objbase.h>
#include <initguid.h>
#include "Library.h"
#include "inoutmac.h"
#endif

#include <iostream>
//#include <iostream.h>
//#include <strstream>

#include "S3DBInterface.h"
#include "KProtocolDef.h"
#include "KProtocol.h"
#include "KRelayProtocol.h"
#include "KTongProtocol.h"

#include "KSOServer.h"
//#include <afxmt.h>
//#pragma comment(linker,"/STACK:102400000,1024000")
#include "../Common/KCriticalSection.h"

#ifndef _STANDALONE
/*
using OnlineGameLib::Win32::CBuffer;
using OnlineGameLib::Win32::CLibrary;
using OnlineGameLib::Win32::CCriticalSection;
*/
CLibrary g_theHeavenLibrary( "heaven.dll" );
CLibrary g_theRainbowLibrary( "rainbow.dll" );
CUsesWinsock g_SocketInit;
//CCriticalSection g_csFlow;
KCriticalSection_ g_csFlow;
 
#else
  CMutex g_mutexFlow;
#endif

CPackager			m_theRecv;
CPackager			m_theSend;

#define				GAME_FPS		18

#define		HARDWARE_ID		"8QC4OB_sSw_~0WA0PC6_jAaFraHK1Z00" //2860ff3f34f874293618
#define		GFDHFGRT		"QSyLgJODASq3SbACd00uE__0d_AQvMYd" //gioi han ip
#define		FDGFGYKL        "U0ASZS3ZCAQGxrqeNGAB06Mhc8dG3dcA"
#define		RIUHJFHI        "F36B291419E617501B2F6769BD3A9A1" 	//2018071505



using namespace std;

//初始化
KSwordOnLineSever g_pSOServer;

enum PLAYER_EXCHANGE_STATUS
{
	enumExchangeBegin = 0,
	enumExchangeSearchingWay,
	enumExchangeWaitForGameSvrRespone,
	enumExchangeCleaning,
};


int	g_nTongPCSize[defTONG_PROTOCOL_CLIENT_NUM] = 
{
	-1,										// enumS2C_TONG_CREATE_SUCCESS
	sizeof(STONG_CREATE_FAIL_SYNC),			// enumS2C_TONG_CREATE_FAIL
	sizeof(STONG_ADD_MEMBER_SUCCESS_SYNC),	// enumS2C_TONG_ADD_MEMBER_SUCCESS
	sizeof(STONG_ADD_MEMBER_FAIL_SYNC),		// enumS2C_TONG_ADD_MEMBER_FAIL
	-1,										// enumS2C_TONG_HEAD_INFO
	-1,										// enumS2C_TONG_MANAGER_INFO
	-1,										// enumS2C_TONG_MEMBER_INFO
	sizeof(STONG_BE_INSTATED_SYNC),			// enumS2C_TONG_BE_INSTATED
	sizeof(STONG_INSTATE_SYNC),				// enumS2C_TONG_INSTATE
	sizeof(STONG_KICK_SYNC),				// enumS2C_TONG_KICK
	sizeof(STONG_BE_KICKED_SYNC),			// enumS2C_TONG_BE_KICKED
	sizeof(STONG_LEAVE_SYNC),				// enumS2C_TONG_LEAVE
	sizeof(STONG_CHECK_GET_MASTER_POWER_SYNC),	// enumS2C_TONG_CHECK_CHANGE_MASTER_POWER
	sizeof(STONG_CHANGE_MASTER_FAIL_SYNC),	// enumS2C_TONG_CHANGE_MASTER_FAIL
	sizeof(STONG_CHANGE_AS_SYNC),			// enumS2C_TONG_CHANGE_AS
	sizeof(STONG_CHANGE_MASTER_SYNC),		// enumS2C_TONG_CHANGE_MASTER
	sizeof(STONG_LOGIN_DATA_SYNC),			// enumS2C_TONG_LOGIN_DATA
	sizeof(STONG_MONEY_SYNC),
	sizeof(STONG_MONEY_SYNC),
	sizeof(STONG_MONEY_SYNC),
	sizeof(STONG_POWER_SYNC),				// enumS2C_TONG_SET_POWER
	sizeof(STONG_POWER_SYNC),				// enumS2C_TONG_GET_POWER
	-1,										// enumS2C_TONG_SEARCH_INFO
};

#ifndef _STANDALONE
typedef HRESULT ( __stdcall * pfnCreateClientInterface )( 
			REFIID riid, 
			void **ppv 
		);

typedef HRESULT ( __stdcall * pfnCreateServerInterface )(
			REFIID	riid,
			void	**ppv
		);
#endif

#ifndef _STANDALONE
void __stdcall ServerEventNotify(
#else
void ServerEventNotify(								 
#endif
			LPVOID lpParam,
			const unsigned long &ulnID,
			const unsigned long &ulnEventType )
{

#ifndef _STANDALONE
	//CCriticalSection::Owner locker(g_csFlow);
	 KAutoCriticalSection_ AutoLock(g_csFlow);
#else
	g_mutexFlow.Lock();
#endif

	switch(ulnEventType)
	{
	case enumClientConnectCreate: //设置网络链接状态
		g_pSOServer.SetNetStatus(ulnID, enumNetConnected);
		//printf("----------网络(%d) 状态连接----------\n",ulnID);
		break;
	case enumClientConnectClose:  //离开游戏时调用 设置断开状态
		g_pSOServer.SetNetStatus(ulnID, enumNetUnconnect,true);
	    //printf("----------网络(%d) 状态断开----------\n",ulnID);
		break;
	}

#ifdef _STANDALONE
	g_mutexFlow.Unlock();
#endif

}


#ifndef _STANDALONE
void __stdcall GatewayClientEventNotify(
#else
void GatewayClientEventNotify(
#endif
			LPVOID lpParam,
			const unsigned long &ulnEventType )
{
	switch( ulnEventType )
	{
	case enumServerConnectCreate:
		g_pSOServer.m_mapClientNet[GATWSTATE]=TRUE;
		break;
	case enumServerConnectClose:
		//g_pSOServer.SetS3RelayRunStatus(FALSE,5);
		g_pSOServer.m_mapClientNet[GATWSTATE]=FALSE;
		printf("---------GateWay lost----------\n");
		//g_SOServer.SetRunningStatus(FALSE);  //设置GS停止运行
		break;
	}
}

#ifndef _STANDALONE
void __stdcall ChatClientEventNotify(
#else
void ChatClientEventNotify(
#endif
			LPVOID	lpParam,
			const unsigned long &ulnEventType )
{
	switch( ulnEventType )
	{
	case enumServerConnectCreate:
		g_pSOServer.m_mapClientNet[CHATSTATE]=TRUE;
		break;
	case enumServerConnectClose:
		//g_pSOServer.m_ChatState = FALSE;
		//g_pSOServer.SetS3RelayRunStatus(FALSE,1);
		g_pSOServer.m_mapClientNet[CHATSTATE]=FALSE;
		printf("-------Chat disconnect-----\n");
		break;
	}
}

#ifndef _STANDALONE
void __stdcall TongClientEventNotify(
#else
void TongClientEventNotify(
#endif
	LPVOID	lpParam,const unsigned long &ulnEventType)
{
	switch( ulnEventType )
	{
	case enumServerConnectCreate:	// 连接建立时候的处理
		g_pSOServer.m_mapClientNet[TONGSTATE]=TRUE;
		break;
	case enumServerConnectClose:	// 连接断开时候的处理
		//g_pSOServer.m_TongState = FALSE;
		g_pSOServer.m_mapClientNet[TONGSTATE]=FALSE;
		//g_pSOServer.SetS3RelayRunStatus(FALSE,2);
		printf("------Tong disconnect------\n");
		break;
	}
}

#ifndef _STANDALONE
void __stdcall DatabaseClientEventNotify(
#else
void DatabaseClientEventNotify(
#endif
			LPVOID lpParam,
			const unsigned long &ulnEventType)
{
	switch(ulnEventType)
	{
	case enumServerConnectCreate:
		g_pSOServer.m_mapClientNet[DATASTATE]=TRUE;
		break;
	case enumServerConnectClose:
		//g_pSOServer.SetS3RelayRunStatus(FALSE,4);
		g_pSOServer.m_mapClientNet[DATASTATE]=FALSE;
		printf("---------DataBase lost-----------\n");
		//g_pSOServer.SetRunningStatus(FALSE);//设置GS停止运行
		break;
	}
}

#ifndef _STANDALONE
void __stdcall TransferClientEventNotify(
#else
void TransferClientEventNotify(
#endif
			LPVOID lpParam,
			const unsigned long &ulnEventType )
{
	switch( ulnEventType )
	{
	case enumServerConnectCreate:
		g_pSOServer.m_mapClientNet[TRANSTATE]=TRUE;
		break;
	case enumServerConnectClose:
//		g_SOServer.SetRunningStatus(FALSE);
		g_pSOServer.m_mapClientNet[TRANSTATE]=FALSE;
		//g_pSOServer.SetS3RelayRunStatus(FALSE,3);
		//g_pSOServer.m_TranState = FALSE;
		break;
	}
}

KSwordOnLineSever::KSwordOnLineSever()
{
	m_nGameLoop = 0;
	m_nGameDay  = 0;
	m_nCurGameLoop=0;
	m_bIsRunning = TRUE;
	m_nServerPort = 56766;      //登陆游戏服务器的端口
	m_nGatewayPort = 56732;     //bishop 账号登陆IP
	m_nDatabasePort = 5001;
	m_nTransferPort = 5003;
	m_nChatPort	= 5004;
	m_nTongPort	= 5005;
	m_snBufferSize=1024*16;
	m_snMaxBuffer=1024;
	ZeroMemory(m_szGatewayIP, sizeof(m_szGatewayIP));
	ZeroMemory(m_szDatabaseIP, sizeof(m_szDatabaseIP));
	ZeroMemory(m_szTransferIP, sizeof(m_szTransferIP));
	ZeroMemory(m_szChatIP, sizeof(m_szChatIP));
	ZeroMemory(m_szTongIP, sizeof(m_szTongIP));
	ZeroMemory(m_parmName, sizeof(m_parmName));
	
	m_pServer = NULL;
	m_pGatewayClient = NULL;
	m_pDatabaseClient = NULL;
	m_pTransferClient = NULL;
	m_pChatClient = NULL;
	m_pTongClient = NULL;
	m_pCoreServerShell = NULL;
	m_pGameStatus = NULL;
	m_IsWm=TRUE;
	inGo=FALSE;

    m_ChatState = FALSE;
    m_TongState = FALSE;
    m_TranState = FALSE;
	m_pDataState = FALSE;
    m_GatewayState = FALSE;
	nCurWindHwnd = 0;
	ZeroMemory(m_curNetTime,sizeof(m_curNetTime));

	m_mapClientNet.clear();
}

KSwordOnLineSever::~KSwordOnLineSever()
{

}

//设置服务器端口
BOOL KSwordOnLineSever::SetServerPort()
{
	g_SetRootPath(NULL);
	g_SetFilePath("\\");
	KIniFile iniFile;
	if (!iniFile.Load("ServerCfg.ini"))
	{
		MessageBox(0,"GS启动失败","GS:",MB_OK); 
	}

	iniFile.GetInteger("GameServer", "Port",56766, &m_nServerPort);
/*
#ifdef WIN32
	iniFile.GetInteger("Overload", "MaxPlayer", 450, &m_nMaxPlayerCount);
	iniFile.GetInteger("Overload", "Precision", 10, &m_nPrecision);

#else
	iniFile.GetInteger("Overload", "MaxPlayer", 1000, &m_nMaxPlayerCount);
	iniFile.GetInteger("Overload", "Precision", 10, &m_nPrecision);
#endif */
	iniFile.Clear();

  if (m_pServer)
  {
	if (FAILED(m_pServer->OpenService(INADDR_ANY,m_nServerPort)))
	{//打开服务器端口
		//cout << "Can't OPEN server port\n" << endl;
		//MessageBox(0,"GS启动获取服务器IPA失败","GS:",MB_OK);
	}
	else
	{
		printf("-----二次开放端口成功:%d----\n",m_nServerPort);

	}
		//printf("-------------服务器端口A:%d-------------\n",m_nServerPort);
	/*	tagGameSvrInfo ni;			
			ni.cProtocol = c2s_updategameserverinfo;
			
			ni.nIPAddr_Internet = m_dwInternetIp;
			ni.nIPAddr_Intraner = m_dwIntranetIp;
			ni.nPort            = m_nServerPort;
			ni.wCapability      = m_nMaxPlayerCount;  //最大人数限制
			m_pGatewayClient->SendPackToServer((const void *)&ni, sizeof(tagGameSvrInfo));*/
  }
	return TRUE;
}

void KSwordOnLineSever::ErrOut()
{
	//cout<<".......服务器已经跳出g_pSOServer.Breathe()循环......"<<endl;
	printf(".......服务器已经退出活动...... \n");

}

//取余数
/*DWORD TakeRemainder(DWORD a,DWORD b)
{
	DWORD nRet = 0;
	//	DWORD nYuShu=0;
	__asm
	{
		mov eax,a
			mov ecx,b
			xor edx,edx
			idiv ecx
			mov nRet,edx
			//mov nYuShu,edx
	}
	return nRet;
}

//取商
DWORD TakeTrader(DWORD a,DWORD b)
{
	DWORD nRet = 0;
	//DWORD nYuShu=0;
	__asm
	{
		mov eax,a
			mov ecx,b
			xor edx,edx
			idiv ecx
			mov nRet,eax
			//mov nYuShu,edx
	}
	return nRet;
} */


int	KSwordOnLineSever::FindFree()
{
	return m_FreeIdxNetStatus.GetNext(0);
}

int KSwordOnLineSever::FindSame(DWORD dwID)
{
	int nUseIdx = 0;
	
	nUseIdx = m_UseIdxNetStatus.GetNext(0);
	while(nUseIdx>0)
	{
		if (nUseIdx<=m_nMaxGameStaus && m_pGameStatus[nUseIdx].nNetidx == dwID)
			return nUseIdx;

		nUseIdx = m_UseIdxNetStatus.GetNext(nUseIdx);
	}
	return 0;
}

int	KSwordOnLineSever::GetFirstNetIdx()
{
	m_nListCurIdx = m_UseIdxNetStatus.GetNext(0);
	return m_nListCurIdx;
}

int	KSwordOnLineSever::GetNextNetIdx()
{
	if (!m_nListCurIdx)
		return 0;
	m_nListCurIdx = m_UseIdxNetStatus.GetNext(m_nListCurIdx);
	return m_nListCurIdx;
}

void KSwordOnLineSever::ClearNetStatus()
{
	int nUsedIndex = m_UseIdxNetStatus.GetNext(0);
	
	while (nUsedIndex!= 0)
	{
		//Remove(nUsedIndex);
		m_FreeIdxNetStatus.Insert(nUsedIndex);
		m_UseIdxNetStatus.Remove(nUsedIndex);
		nUsedIndex = m_UseIdxNetStatus.GetNext(0);
	}
}
//-------------------------监控线程
//工作线程
DWORD WINAPI ThreadProcessGetGatway( void *pParam )
{
	IClient *pcServer = (IClient *)pParam;
	if (pcServer)
	{
	  cout << " - Gateway server is monitoring.!" << endl;
	  DWORD g_nServiceLoop = 0;
	  while(true)
	  {
		if(!g_pSOServer.GetClientNet(GATWSTATE))
		{//已经断开 就开始重连
			if  (g_nServiceLoop>=TIME_FARM && (g_nServiceLoop%TIME_FARM)==0)
			     g_pSOServer.SetClientConnetNet(1);
		}

		//try
		//{ 
			//g_pSOServer.PlayerLogoutGateway();           //网关断开 循环检测 是否退出游戏的
		//} 
		/*catch (...)
		{ 
			printf("Core PlayerLogoutGateway !\n");
			g_pSOServer.m_pGatewayClient->Shutdown();	 //关闭链接 然后重连
			g_pSOServer.m_mapClientNet[GATWSTATE] = FALSE;
			g_pSOServer.m_GatewayState = FALSE;
		}*/

		if (++g_nServiceLoop & 0x80000000)
		{
			g_nServiceLoop = 0;
		}
		if (g_nServiceLoop & 0x1)
		{
			::Sleep(10);
		}
	  }
	}
	
	return 0L;
}

DWORD WINAPI ThreadProcessDataClient( void *pParam )
{
	IClient *pcServer = (IClient *)pParam;
	if (pcServer)
	{
	   cout << " - Database server is monitoring.!" << endl;
	   DWORD g_nServiceLoop = 0;
	   while(true)
	   {
		if(!g_pSOServer.GetClientNet(DATASTATE))
		{//已经断开 就开始重连
			if  (g_nServiceLoop>=TIME_FARM && (g_nServiceLoop%TIME_FARM)==0)
				g_pSOServer.SetClientConnetNet(2);
		}
		if (++g_nServiceLoop & 0x80000000)
		{
			g_nServiceLoop = 0;
		}
		if (g_nServiceLoop & 0x1)
		{
			::Sleep(10);
		}
	   }
	}
	return 0L;
}

DWORD WINAPI ThreadProcessChatClient( void *pParam )
{
	IClient *pcServer = (IClient *)pParam;
	if (pcServer)
	{
	  cout << " - Chat server is monitoring.!" << endl;
	  DWORD g_nServiceLoop = 0;
	  while(true)
	  {
		if(!g_pSOServer.GetClientNet(CHATSTATE))
		{//已经断开 就开始重连
			if  (g_nServiceLoop>=TIME_FARM && (g_nServiceLoop%TIME_FARM)==0)
				g_pSOServer.SetClientConnetNet(3);
		}

		if (++g_nServiceLoop & 0x80000000)
		{
			g_nServiceLoop = 0;
		}
		if (g_nServiceLoop & 0x1)
		{
			::Sleep(10);
		}

	  }
	}
	return 0L;
}

DWORD WINAPI ThreadProcessTongClient( void *pParam )
{
	IClient *pcServer = (IClient *)pParam;
	if (pcServer)
	{
		cout << " - Tong server is monitoring.!" << endl;
	  DWORD g_nServiceLoop = 0;
	  while(true)
	  {
		if(!g_pSOServer.GetClientNet(TONGSTATE))
		{//已经断开 就开始重连
			if  (g_nServiceLoop>=TIME_FARM && (g_nServiceLoop%TIME_FARM)==0)
				g_pSOServer.SetClientConnetNet(4);
		}
		if (++g_nServiceLoop & 0x80000000)
		{
			g_nServiceLoop = 0;
		}
		if (g_nServiceLoop & 0x1)
		{
			::Sleep(10);
		}
	}
	}
	return 0L;
}

DWORD WINAPI ThreadProcessTranClient( void *pParam )
{
	IClient *pcServer = (IClient *)pParam;
	#define	MAX_STAT_QUERY_TIME 800*3600           //每四个小时查询一次 Query every 4 hours

	if (pcServer)
	{
	   cout << " - Tranfer server is monitoring.!" << endl;
	   DWORD g_nServiceLoop = 0;
	   while(true)
	   {

       // KAutoCriticalSection_ AutoLock(g_csFlow);

		if(!g_pSOServer.GetClientNet(TRANSTATE))
		{//已经断开 就开始重连
			if  (g_nServiceLoop>=TIME_FARM && (g_nServiceLoop%TIME_FARM)==0)
				g_pSOServer.SetClientConnetNet(5);
		}
		//排名查询
		//Ranking world query
		//if  (g_nServiceLoop>=MAX_STAT_QUERY_TIME && (g_nServiceLoop%MAX_STAT_QUERY_TIME)==0)
		//	g_pSOServer.SetClientConnetNet(8);
		//90000 * 24 / 24h
		#define	defMAX_STAT_QUERY_TIME_SECOUND	5550	// 1p		// lay du lieu xep hang ne  2,160,000/24h
		#define	defMAX_STAT_QUERY_TIME_MIN		333000	//60p
		#define	defMAX_STAT_QUERY_TIME_HOUR		7992000	//24h
		//if (g_nServiceLoop>=defMAX_STAT_QUERY_TIME_OLD && (g_nServiceLoop%defMAX_STAT_QUERY_TIME_OLD)==0)
		SYSTEMTIME systime;
		GetLocalTime(&systime);//7h sang tu dong cap nhat top
		if ((systime.wHour == 7 && systime.wMinute == 0 && systime.wSecond == 0 && (g_nServiceLoop%70)==0) || g_nServiceLoop == 0) 
		{
			//printf("g_nServiceLoop= %d,Time= %02d:%02d:%02d mili:%d\n",g_nServiceLoop ,systime.wHour ,systime.wMinute ,systime.wSecond,systime.wMilliseconds);
			g_pSOServer.SetClientConnetNet(8);
		}

		if  (g_nServiceLoop>=6 && (g_nServiceLoop%6)==0) //每半秒
		     g_pSOServer.SetClientConnetNet(9);
		
		if (++g_nServiceLoop & 0x80000000)
			g_nServiceLoop = 0;

		if (g_nServiceLoop & 0x1)
			::Sleep(10);
	
	   }
	}
	return 0L;
}

DWORD WINAPI ThreadSendClientData( void *pParam )
{
	IServer* pcMianServer =(IServer *)pParam;
	if (pcMianServer)
	{
		cout << "- Follow Client server is monitoring.!" << endl;
		DWORD g_nServiceLoop = 0;
		while(true)
		{
			g_pSOServer.SendCilentPackData();
		}
	}
	return 0L;
}

DWORD WINAPI ThreadSaveDataTask( void *pParam )
{
	IClient *pcServer = (IClient *)pParam;
	if (pcServer)
	{
		cout << " - Data task server is monitoring.!" << endl;
		DWORD g_nServiceLoop = 0;
		while(true)
		{
			//任务 存档
			if  (g_nServiceLoop>=TIME_FARM && (g_nServiceLoop%TIME_FARM)==0)
				g_pSOServer.SetClientConnetNet(6);
			//拍卖行同步
			if  (g_nServiceLoop>=TIME_FARM && (g_nServiceLoop%(300*TIME_FARM)) ==0)
				g_pSOServer.SetClientConnetNet(7);

			if (++g_nServiceLoop & 0x80000000)
			{
				g_nServiceLoop = 0;
			}
			if (g_nServiceLoop & 0x1)
			{
				::Sleep(10);
			}
		}
	}
	return 0L;
}




BOOL KSwordOnLineSever::InitServer(char *parmName)
{

	char nTempConfigPath[64];
	snprintf(nTempConfigPath, sizeof(nTempConfigPath),"%s_cfg.ini",parmName);
	sprintf(m_parmName,"%s",parmName);
	
	printf("-----InitServer:%s(%s) -----\n",parmName,nTempConfigPath);

	m_bIsRunning = TRUE;
	g_SetRootPath(NULL);
	g_SetFilePath("\\");
	KIniFile iniFile;
	//if (!iniFile.Load("ServerCfg.ini"))
	if (!iniFile.Load(nTempConfigPath))
        return FALSE;


	iniFile.GetInteger("GameServer", "Port",56766,&m_nServerPort);
	extern int g_nPort;
	if (g_nPort)//启动端口
		m_nServerPort = g_nPort;

	iniFile.GetString("Gateway", "Ip", "127.0.0.1", m_szGatewayIP,sizeof(m_szGatewayIP));
	iniFile.GetInteger("Gateway", "Port", 56732, &m_nGatewayPort);  //账号登陆端口
	iniFile.GetString("Database", "Ip", "127.0.0.1", m_szDatabaseIP,sizeof(m_szDatabaseIP));
	iniFile.GetInteger("Database", "Port", 5001, &m_nDatabasePort);
	iniFile.GetString("Transfer", "Ip", "127.0.0.1", m_szTransferIP,sizeof(m_szTransferIP));
	iniFile.GetInteger("Transfer", "Port", 5003, &m_nTransferPort);
	iniFile.GetString("Chat", "Ip", "127.0.0.1", m_szChatIP,sizeof(m_szChatIP));
	iniFile.GetInteger("Chat", "Port", 5004, &m_nChatPort);
	iniFile.GetString("Tong", "Ip", "127.0.0.1", m_szTongIP,sizeof(m_szTongIP));
	iniFile.GetInteger("Tong", "Port", 5005, &m_nTongPort);

#ifdef WIN32
	iniFile.GetInteger("Overload", "MaxPlayer", 500, &m_nMaxPlayerCount);
	iniFile.GetInteger("Overload", "Precision", 10, &m_nPrecision);
	//iniFile.GetInteger("Overload", "BufferSize", 65536, &m_snBufferSize);
	iniFile.GetInteger("Overload", "MaxGameStaus", 5000, &m_nMaxGameStaus);
	iniFile.GetInteger("Overload", "MaxClienLoop", 50, &m_nMaxClienLoop);
	
#else
	iniFile.GetInteger("Overload", "MaxPlayer", 1000, &m_nMaxPlayerCount);
	iniFile.GetInteger("Overload", "Precision", 10, &m_nPrecision);
	//iniFile.GetInteger("Overload", "BufferSize", 65536, &m_snBufferSize);
	iniFile.GetInteger("Overload", "MaxGameStaus", 5000, &m_nMaxGameStaus);
	iniFile.GetInteger("Overload", "MaxClienLoop", 50, &m_nMaxClienLoop);
#endif

    iniFile.GetInteger("Overload", "SerVerNo",1,&m_SerVerNo); ///用于多区的服务器编号 大区编号  m_SerVerNo  m_GsNumBer
	iniFile.GetInteger("Overload", "GsNumber",1,&m_GsNumBer); ///用于多区的服务器编号 同一个大区内的gs编号
	char m_szTransIP[16]={0};
	//iniFile.GetInteger("TransServer", "TransSerVerNo",1,&mTransSerVerNo);
	//iniFile.GetString("TransServer", "Ip", "127.0.0.1", m_szTransIP,sizeof(m_szTransIP));
	TransDwIp = inet_addr(m_szTransIP);
	iniFile.GetInteger("TransServer", "Port",0,&TransPort);
	iniFile.GetInteger("BufferSize","gsBufferSize",16*1024,&m_snBufferSize);
	iniFile.GetInteger("BufferSize","errorcount",50,&m_errorCount);
	iniFile.GetInteger("BufferSize","pingcount",20,&m_pingCount);
	
	
	
	
	
	
	
	m_nMaxPlayerCount   = MAX_PLAYER;
    m_nMaxGameStaus    = MAX_PLAYER;
	m_nMaxPlayer            = MAX_PLAYER;

	if  (m_nMaxGameStaus>m_nMaxPlayer)
		m_nMaxGameStaus = m_nMaxPlayer;

	if (m_nMaxPlayer <= 0)
	{
		cout << "Maximal player number <= 0!" << endl;
//		return FALSE;
	}

	/*if (m_pGameStatus->nNetidx==-1)
	{
		cout << "test server !" << endl;
	}*/

	if (!m_pGameStatus)
	{
		m_pGameStatus = new GameStatus[m_nMaxGameStaus+1]; //m_nMaxPlayer	 200  0 和 200 无效
	}
	int i;

	for (i = 0; i <=m_nMaxGameStaus; i++)
	{//0-500
		m_pGameStatus[i].nNetStatus     = enumNetUnconnect;
		m_pGameStatus[i].nNetidx        = -1;
		m_pGameStatus[i].nReplyPingTime = 0;
		m_pGameStatus[i].nSendPingTime  = 0;
		m_pGameStatus[i].nPlayerIndex   = 0;
		m_pGameStatus[i].nGameStatus     = enumPlayerNone;
		m_pGameStatus[i].nExchangeStatus = enumExchangeCleaning;
		m_pGameStatus[i].nErrorLoopCount = 0;
		m_pGameStatus[i].nPingLoopCount  = 0;
	}

	// 优化查找表
	m_FreeIdxNetStatus.Init(m_nMaxGameStaus+1);
	m_UseIdxNetStatus.Init(m_nMaxGameStaus+1);
	
	// 开始时所有的数组元素都为空
	for (i = m_nMaxGameStaus; i > 0; i--) //m_nMaxGameStaus-1
	{//1--500
		m_FreeIdxNetStatus.Insert(i);
	}

//	strcpy(m_szGameSvrIP, OnlineGameLib::Win32::net_ntoa(dwIp));
	
	/*
	 * Open this server to player
	 */

#ifndef _STANDALONE
	pfnCreateServerInterface pFactroyFun = (pfnCreateServerInterface)(g_theHeavenLibrary.GetProcAddress("CreateInterface"));

	IServerFactory *pServerFactory = NULL;

	if (pFactroyFun && SUCCEEDED(pFactroyFun(IID_IServerFactory, reinterpret_cast<void **>(&pServerFactory ))))
	{
		pServerFactory->SetEnvironment(m_nMaxPlayerCount,m_nPrecision,m_nPrecision,m_snBufferSize,0); //m_snMaxBuffer  m_nMaxPlayerCount
		
		pServerFactory->CreateServerInterface(IID_IIOCPServer, reinterpret_cast<void **>(&m_pServer));
		
		pServerFactory->Release();
	}
	else
	{
		return FALSE;
	}
#else
	m_pServer = new IServer(m_nMaxPlayer,4,200*1024);//20,64  64KB
	
#endif
	 //printf("=======m_pServer 内存大小 ：%d b============================\n",sizeof(IServer));
	if (!m_pServer)
	{
		cout << "Initialization failed! Don't find a correct heaven.dll" << endl;
//		return FALSE;
	}

//////////////////////////////////////////////////m_nMaxPlayerCount//////////////////////////////////
	//cout << " Gameserver Infomation\n Gax player    :   "<<m_nMaxPlayerCount<< "\n maximum player:   " <<m_nMaxPlayer <<"\n max npc       :   50.000\n max item      :   400.000\n max task      :   2.000"<< endl;
    printf("====Synchronize the latest official client===========\n");
    printf("===========================================\n");

	m_pServer->Startup();
    //注册信息过滤器函数
	m_pServer->RegisterMsgFilter(reinterpret_cast< void * >(m_pServer),ServerEventNotify);

	if (FAILED(m_pServer->OpenService(INADDR_ANY,m_nServerPort)))
	{//打开服务器端口
		//cout << "Can't get server ip" << endl;
		//MessageBox(0,"GS启动打开服务器端口失败,端口是否给占用了","GS:",MB_OK);
		cout<<"--Failed to start the server port, is the port is busy?--"<<endl;
		Sleep(5000);
		ExitProcess(0);  //退出进程
	}
    
	int inMacNum=0;
	     inMacNum=GetLocalIpAddress(&m_dwIntranetIp, &m_dwInternetIp);

	if (m_IsWm)
	{//直接地区外网IP
		KIniFile iniFile;
		if (iniFile.Load(nTempConfigPath))
		{
			char strIP[32]={0};
			if (iniFile.GetString("GameServer", "Ip", "", strIP, sizeof(strIP)))
			{
				DWORD dwIp = inet_addr(strIP);
				if (INADDR_NONE == dwIp)
				{
					printf("--Failed to start the server, [GameServer] Ip='%s' in %s is not an address--\n", strIP, nTempConfigPath);
					return FALSE;
				}
				m_dwInternetIp = dwIp;
				strncpy(m_szGameServerIP1, strIP, sizeof(m_szGameServerIP1) - 1);
				m_szGameServerIP1[sizeof(m_szGameServerIP1) - 1] = 0;
			}

			ZeroMemory(strIP, sizeof(strIP));
			if (iniFile.GetString("GameServer", "localIp", "", strIP, sizeof(strIP)))
			{
				DWORD dwIp = inet_addr(strIP);
				if (INADDR_NONE == dwIp)
				{
					printf("--Failed to start the server, [GameServer] localIp='%s' in %s is not an address--\n", strIP, nTempConfigPath);
					return FALSE;
				}
				m_dwIntranetIp = dwIp;
				strncpy(m_szGameServerIP2, strIP, sizeof(m_szGameServerIP2) - 1);
				m_szGameServerIP2[sizeof(m_szGameServerIP2) - 1] = 0;
			}
		}
		iniFile.Clear();
	}

	if (inMacNum<0 || inMacNum>2)
	{
	    cout<<"--Too many network cards, keep one virtual network card and one public network card, please check again--"<<endl;
		//Sleep(5000);
		//ExitProcess(0);  //退出进程
		//MessageBox(0,"GS启动获取服务器IPB失败","GS:",MB_OK);
	}

	const unsigned char* pIpAddress=NULL;

	      pIpAddress=(const unsigned char*)&m_dwIntranetIp;

    char Address[128]={0}; //IP地址

	sprintf(Address, "%d.%d.%d.%d", pIpAddress[0], pIpAddress[1],pIpAddress[2], pIpAddress[3]);

	cout <<" virtual IP   :   "<< Address << endl;

	pIpAddress=(const unsigned char*)&m_dwInternetIp;
	sprintf(Address, "%d.%d.%d.%d", pIpAddress[0], pIpAddress[1],pIpAddress[2], pIpAddress[3]);

	cout <<" public net IP   :   "<< Address << endl;
//----------------------------单机设置
      /*char ntnnn[64]={0},ndnnn[64]={0};
	   //PassChecksum nTss;
	   //nTss.SimplyDecryptPassword(ntnnn, GFDHFGRT);
	   JieMa_Decipher(GFDHFGRT,ntnnn);
	   sprintf(ndnnn,"%u",m_dwInternetIp);
	   if (!strstr(ndnnn,ntnnn))
	   //if (m_dwInternetIp!=atoi(ntnnn)
	   {
		   return FALSE;
	   }*/
/*
		char ntnnn[64]={0},ndnnn[64]={0};
	   PassChecksum nTss;
	   nTss.SimplyDecryptPassword(ntnnn, GFDHFGRT);
	   sprintf(ndnnn,"%u", m_dwInternetIp);
	   sprintf(ntnnn,"%u", inet_addr(ntnnn));
	   //if (!strstr(ndnnn, ntnnn))
	   if (m_dwInternetIp != atoi(ntnnn))
	   {
		   return FALSE;
	   }
*/

//-----------------------------
#ifndef _STANDALONE
	IServer *pCloneServer = NULL;
	m_pServer->QueryInterface(IID_IIOCPServer,( void **)&pCloneServer);
#else 
	IServer *pCloneServer = m_pServer;
#endif
	/*
	 * Init GameWorld
	 */
	if (m_pCoreServerShell == NULL)
		m_pCoreServerShell = ::CoreGetServerShell(parmName);//单机设置

	if (m_pCoreServerShell==NULL)
	{
		cout << "--Failed to initialize game data!--" << endl;
		return FALSE;
	}

	m_pCoreServerShell->OperationRequest(SSOI_LAUNCH, (UINT_PTR)pCloneServer, 0);  //发送开始游戏命令

	/*
	 * - Ket noi den database
	 */
#ifndef _STANDALONE
	iniFile.GetInteger("BufferSize", "DataBuffer",131072,&m_snBufferSize);
	pfnCreateClientInterface pClientFactroyFun = (pfnCreateClientInterface)(g_theRainbowLibrary.GetProcAddress("CreateInterface"));
	IClientFactory *pClientFactory = NULL;

	if (pClientFactroyFun && SUCCEEDED(pClientFactroyFun(IID_IClientFactory,reinterpret_cast< void ** >(&pClientFactory))))
	{   
		pClientFactory->SetEnvironment(m_nMaxPlayer,m_nPrecision,m_snBufferSize);  //128*1024

		pClientFactory->CreateClientInterface( IID_IESClient, reinterpret_cast< void ** >(&m_pDatabaseClient));

		pClientFactory->Release();
	}
	else
	{
//		return FALSE;
       cout << "Failed to Create pClientFactory." << endl;
	}
#else
//	net_buffer = new ZBuffer(20240000, 400);
	m_pDatabaseClient = new IClient(6000*1024, 6000*1024);//6000
#endif
	//printf("=======m_pDatabaseClient 内存大小 ：%d kb============================\n",sizeof(m_pDatabaseClient)/1024);
	if (!m_pDatabaseClient)
	{
		cout << "Initialization failed! Don't find a correct rainbow.dll" << endl;

//		return FALSE;
	}
	if (FAILED(m_pDatabaseClient->Startup()))
	{
		cout << "Can't not startup database client service!" << endl;
//		return FALSE;
	}
//	cout << " ___________________________________________"<<endl;
	cout << " Database IP   : " << m_szDatabaseIP << " - Port : " << m_nDatabasePort  <<endl;

	m_pDatabaseClient->RegisterMsgFilter(reinterpret_cast<void *>(m_pDatabaseClient),DatabaseClientEventNotify);
  	//printf("=======m_pDatabaseClient 内存大小 ：%d kb============================\n",sizeof(m_pDatabaseClient)/1024);
	if (FAILED(m_pDatabaseClient->FsGameServerConnectTo(m_szDatabaseIP,m_nDatabasePort)))
	{//链接这个端口
		m_pDataState = FALSE;
		cout << " - Database server connection failed.Please check again!" << endl;
	}
	else
	{
		m_pDataState = TRUE;
		cout << " - Database server connection succeeded..!" << endl;
	}

	m_pCoreServerShell->OnSetData(m_pDatabaseClient);
	/*
	 * - Ket noi den transfer
	 */

#ifndef _STANDALONE
	iniFile.GetInteger("BufferSize", "TransBuffer",131072,&m_snBufferSize);
	pClientFactroyFun = (pfnCreateClientInterface)(g_theRainbowLibrary.GetProcAddress("CreateInterface"));
	pClientFactory = NULL;

	if (pClientFactroyFun && SUCCEEDED(pClientFactroyFun(IID_IClientFactory, reinterpret_cast<void **>(&pClientFactory))))
	{
		pClientFactory->SetEnvironment(m_nMaxPlayer,m_nPrecision,m_snBufferSize); //1024 * 512

		pClientFactory->CreateClientInterface(IID_IESClient, reinterpret_cast< void ** >(&m_pTransferClient));

		pClientFactory->Release();
	}
	else
	{
	//	return FALSE;
		cout << " - 错误A SetEnvironment!" << endl;
	}
#else
	m_pTransferClient = new IClient(6000*1024, 6000*1024);
#endif
	//printf("=======m_pTransferClient 内存大小 ：%d kb============================\n",sizeof(m_pTransferClient)/1024);
	if (!m_pTransferClient)
	{
		cout << "Initialization failed! Don't find a correct rainbow.dll" << endl;
		cout << " - 错误B SetEnvironment!" << endl;
	}
		
	if (FAILED(m_pTransferClient->Startup()))
	{
		cout << "Can't not startup transfer client service!" << endl;
	}
	
	cout << " Transfer IP   : " << m_szTransferIP << " - Port : " << m_nTransferPort << endl;
	
	m_pTransferClient->RegisterMsgFilter( reinterpret_cast< void * >(m_pTransferClient),TransferClientEventNotify);
	//host server connet
	if (FAILED(m_pTransferClient->FsGameServerConnectTo(m_szTransferIP,m_nTransferPort)))
	{
		cout << "- Connect to Transfer failed!" << endl;
		m_TranState = FALSE;
	}
	else
	{	
		m_TranState = TRUE;
		cout << " - Connect to Transfer is succeeded..!" << endl;	
	}

	//IClient* pTranServer =m_pTransferClient;
	//m_pCoreServerShell->OperationRequest(SSOI_TRAN_INIT, (unsigned int)m_pTransferClient, 0);  //发送开始游戏命令
	m_pCoreServerShell->OnSetTran(m_pTransferClient);
#ifndef _STANDALONE
	iniFile.GetInteger("BufferSize", "ChatBuffer",131072,&m_snBufferSize);
	pClientFactroyFun = (pfnCreateClientInterface)(g_theRainbowLibrary.GetProcAddress("CreateInterface"));

	pClientFactory = NULL;

	if ( pClientFactroyFun && SUCCEEDED(pClientFactroyFun(IID_IClientFactory,reinterpret_cast< void ** >(&pClientFactory))))
	{
		pClientFactory->SetEnvironment(m_nMaxPlayer,m_nPrecision,m_snBufferSize); //1024 * 1024 * 4

		pClientFactory->CreateClientInterface(IID_IESClient,reinterpret_cast< void ** >(&m_pChatClient));

		pClientFactory->Release();
	}
	else
	{
//		return FALSE;
			cout << " - 错误D SetEnvironment!" << endl;
	}
#else
	m_pChatClient = new IClient(6000*1024, 6000*1024);
#endif
	//printf("=======m_pChatClient 内存大小 ：%d kb============================\n",sizeof(m_pChatClient)/1024);
	if (!m_pChatClient)
	{
		cout << "Initialization failed! Don't find a correct rainbow.dll" << endl;

//		return FALSE;
	}
		
	if (FAILED(m_pChatClient->Startup()))
	{
		cout << "Can't not startup chat client service!" << endl;
//		return FALSE;
	}
	
	cout << " Chat IP       : " << m_szChatIP << " - Port : " << m_nChatPort << endl;
	
	m_pChatClient->RegisterMsgFilter( reinterpret_cast< void * >(m_pChatClient), ChatClientEventNotify );
	
	if (FAILED( m_pChatClient->FsGameServerConnectTo( m_szChatIP,m_nChatPort)))
	{
		cout << "- Connect to Chat server failed!" << endl;
		m_ChatState = FALSE;
	}
	else
	{	
		m_ChatState = TRUE;
		cout << " - Connect to Chat server OK..!" << endl;
	}

	//IClient* pChatServer =m_pChatClient;
	//m_pCoreServerShell->OperationRequest(SSOI_CHAT_INIT, (unsigned int)m_pChatClient, 0);  //发送开始游戏命令
	m_pCoreServerShell->OnSetChat(m_pChatClient);
//------------------------ - Ket noi den tong ----------------------------
#ifndef _STANDALONE
	iniFile.GetInteger("BufferSize", "TongBuffer",131072,&m_snBufferSize);
	pClientFactroyFun = (pfnCreateClientInterface)(g_theRainbowLibrary.GetProcAddress("CreateInterface"));
	pClientFactory = NULL;
	if ( pClientFactroyFun && SUCCEEDED( pClientFactroyFun(IID_IClientFactory, reinterpret_cast< void ** >(&pClientFactory))))
	{
		pClientFactory->SetEnvironment(m_nMaxPlayer,m_nPrecision,m_snBufferSize); //1024 * 1024 * 4
		pClientFactory->CreateClientInterface(IID_IESClient, reinterpret_cast<void **>(&m_pTongClient));
		pClientFactory->Release();
	}
	else
	{
//		return FALSE;
			cout << " - 错误E SetEnvironment!" << endl;
	}
#else
	m_pTongClient = new IClient(6000*1024, 6000*1024);  //64*1024
#endif
	//printf("=======m_pTongClient 内存大小 ：%d kb============================\n",sizeof(m_pTongClient)/1024);

	if ( !m_pTongClient )
	{
		cout << "Initialization failed! Don't find a correct rainbow.dll" << endl;
//		return FALSE;
	}
		
	if (FAILED(m_pTongClient->Startup()))
	{
		cout << "Can't not startup tong client service!" << endl;
//		return FALSE;
	}
	
	cout << " Tong IP       : " << m_szTongIP << " - Port : " << m_nTongPort << endl;
	
	m_pTongClient->RegisterMsgFilter( reinterpret_cast< void * >(m_pTongClient), TongClientEventNotify );
	
	if (FAILED( m_pTongClient->FsGameServerConnectTo(m_szTongIP, m_nTongPort)))
	{
		cout << " - Connect to Tong server failed!" << endl;
		m_TongState = FALSE;
	}
	else
	{	
		m_TongState = TRUE;
		cout << " - Connect to Tong server OK..!" << endl;
	}
//------------------------ - Ket noi den tong end ----------------------------
	//IClient* pTongServer =m_pTongClient;
	//m_pCoreServerShell->OperationRequest(SSOI_TONG_INIT, (unsigned int)m_pTongClient, 0);  //发送开始游戏命令
	m_pCoreServerShell->OnSetTong(m_pTongClient);
	/*
	 * - Ket noi den gateway
	 */
#ifndef _STANDALONE
	iniFile.GetInteger("BufferSize", "GatewayBuffer",131072,&m_snBufferSize);
	pClientFactroyFun = (pfnCreateClientInterface )(g_theRainbowLibrary.GetProcAddress("CreateInterface"));

	pClientFactory = NULL;

	if (pClientFactroyFun && SUCCEEDED(pClientFactroyFun(IID_IClientFactory, reinterpret_cast<void **>(&pClientFactory))))
	{
		pClientFactory->SetEnvironment(m_nMaxPlayer,m_nPrecision,m_snBufferSize);  //1024 * 128

		pClientFactory->CreateClientInterface(IID_IESClient, reinterpret_cast<void ** >(&m_pGatewayClient) );

		pClientFactory->Release();
	}
	else
	{
//		return FALSE;
			cout << " - 错误F SetEnvironment!" << endl;
	}
#else
	m_pGatewayClient = new IClient(6000*1024, 6000*1024);  //IClient(64*1024,64*1024)
#endif
	//printf("=======m_pGatewayClient 内存大小 ：%d kb============================\n",sizeof(m_pGatewayClient)/1024);
	if (!m_pGatewayClient)
	{
		cout << "Initialization failed! Don't find a correct rainbow.dll" << endl;

//		return FALSE;
	}

	if (FAILED(m_pGatewayClient->Startup()))
	{//创建线程
		cout << "Can't not startup gateway client service!" << endl;
//		return FALSE;
	}

	// HANDLE nHd = m_pGatewayClient->GetTheaidHdw();

	cout << " Gateway IP    : " << m_szGatewayIP << " - Port : " << m_nGatewayPort << endl;

	m_pGatewayClient->RegisterMsgFilter( reinterpret_cast< void * >( m_pGatewayClient ), GatewayClientEventNotify );

	 if (FAILED( m_pGatewayClient->FsGameServerConnectTo(m_szGatewayIP,m_nGatewayPort)))
	 { 
		m_GatewayState = FALSE;
		cout << "- Connect to Gateway server failed!" << endl;
//		return FALSE;
	 } 
     else 
	 {
		 m_GatewayState = TRUE;
	     cout << " - Connect to Gateway server OK..!" << endl;
	 }

	// IClient* pGatewayServer =m_pGatewayClient;
	// m_pCoreServerShell->OperationRequest(SSOI_GATEWAYINIT, (unsigned int)m_pGatewayClient, 0);  //发送开始游戏命令
	m_pCoreServerShell->OnSetGateWay(m_pGatewayClient);
	cout << ">>>>>>>>>>>>>Server start succeeded!..<<<<<<<<<<<<<" << endl;	
	printf("==========================================\n");
	

	m_Timer.Start();
    //m_Timer.Stop();
	m_nGameLoop = 0;
	m_nGameDay  = 0;
	m_nCurGameLoop=0;


/*	//xep hang 
	if (m_pDatabaseClient)
	{
		TProcessData	ProcessData;
		ProcessData.nProtoId = c2s_gamestatistic;
		ProcessData.nDataLen = 1;
		ProcessData.ulIdentity = -1;
		ProcessData.pDataBuffer[0] = 0;
		m_pDatabaseClient->SendPackToServer(&ProcessData, sizeof(TProcessData));
		cout << ">>>>>>>>>>>>>Cap Nhat Danh Sach Xep Hang succeeded!..<<<<<<<<<<<<<" << endl;	
	}
*/
	//------------------------------发送更新S3各种数据
	tagLeaveGame2 lg2;
	lg2.ProtocolFamily = pf_udataserveridx;
	lg2.ProtocolID     = relay_s2c_udateserver;
	lg2.cCmdType       = NORMAL_LEAVEGAME;
	lg2.nSelServer     = MAKELONG(m_GsNumBer, m_SerVerNo);;//m_SerVerNo;
	if (m_pTransferClient)
		m_pTransferClient->SendPackToServer(&lg2,sizeof(tagLeaveGame2));
	if (m_pChatClient)
		m_pChatClient->SendPackToServer(&lg2,sizeof(tagLeaveGame2));
	if (m_pTongClient)
		m_pTongClient->SendPackToServer(&lg2,sizeof(tagLeaveGame2));
	
	tagUpMapGame2 pUMI;
	memset( &pUMI, 0,sizeof(tagUpMapGame2));

	pUMI.ProtocolFamily = pf_udatasservermaps;
	pUMI.ProtocolID        = relay_s2c_udatemaps;
	pUMI.cMapCount      = 0;
	sprintf(	pUMI.szparmName,"%s",parmName);
	
	if (m_pTransferClient) //长度不能大于1024
	  m_pTransferClient->SendPackToServer(&pUMI,sizeof(tagUpMapGame2));

	//开始监控线程
	DWORD dwThreadID = 0L;
	static HANDLE g_hThread = NULL;
	//IClient* GetGatwayClient =NULL;
	//GetGatwayClient = g_pSOServer.GetGatwClient();
	g_hThread = ::CreateThread(NULL,0,ThreadProcessGetGatway,(void *)m_pGatewayClient,0,&dwThreadID);
	//IClient* DataClient=NULL;
	//DataClient= g_pSOServer.GetDataClient();
	g_hThread = ::CreateThread(NULL,0,ThreadProcessDataClient,(void *)m_pDatabaseClient,0,&dwThreadID);
	//IClient* Chatlient =NULL;
	//Chatlient = g_pSOServer.GetChatClient();
	g_hThread = ::CreateThread(NULL,0,ThreadProcessChatClient,(void *)m_pChatClient,0,&dwThreadID);
	//IClient* Tonglient =NULL;
	//Tonglient = g_pSOServer.GetTongClient();
	g_hThread = ::CreateThread(NULL,0,ThreadProcessTongClient,(void *)m_pTongClient,0,&dwThreadID);
	//IClient* Tranlient =NULL;
	//Tranlient = g_pSOServer.GetTranClient();
	g_hThread = ::CreateThread(NULL,0,ThreadProcessTranClient,(void *)m_pTransferClient,0,&dwThreadID);
	g_hThread = ::CreateThread(NULL,0,ThreadSaveDataTask,(void *)m_pTransferClient,0,&dwThreadID);
	//创建一个发包线程
//	g_hThread = ::CreateThread(NULL,0,ThreadSendClientData,(void *)m_pServer,0,&dwThreadID);
	return TRUE;
}

void KSwordOnLineSever::Release()
{
	
#ifndef _STANDALONE
	//CCriticalSection::Owner locker(g_csFlow);
	 KAutoCriticalSection_ AutoLock(g_csFlow);
#else
	g_mutexFlow.Lock();
#endif
	
	if (m_pServer)
	{
		ExitAllPlayer();
		// Close service for no one can login again
		//m_pServer->CloseService();
		// Regist callback function to null
		//m_pServer->RegisterMsgFilter( reinterpret_cast< void * >(m_pServer),NULL);
		// Shut down all client and save their data.
		///const char *pChar = NULL;
		//MessageBox(0,"GS:服务器即将关闭,踢掉所有玩家!!","警告:",MB_OK);	
	}

	if (m_pCoreServerShell)
	{
		m_pCoreServerShell->Release();
		m_pCoreServerShell = NULL;
	}

	if (m_pGameStatus)
	{
		delete [] m_pGameStatus;
		m_pGameStatus = NULL;
	}

	if (m_pServer)
	{
		m_pServer->Cleanup();
		m_pServer->Release();
#ifndef _STANDALONE
		m_pServer = NULL;
#else
		delete m_pServer;
		m_pServer = NULL;
#endif
	}

	if (m_pGatewayClient)
	{
		m_pGatewayClient->Shutdown();
		m_pGatewayClient->Cleanup();
		m_pGatewayClient->Release();
		m_pGatewayClient = NULL;
	}

	if (m_pDatabaseClient)
	{
		m_pDatabaseClient->Shutdown();
		m_pDatabaseClient->Cleanup();
		m_pDatabaseClient->Release();
		m_pDatabaseClient = NULL;
	}

	if (m_pTransferClient)
	{
		m_pTransferClient->Shutdown();
		m_pTransferClient->Cleanup();
		m_pTransferClient->Release();
		m_pTransferClient = NULL;
	}

	if (m_pChatClient)
	{
		m_pChatClient->Shutdown();
		m_pChatClient->Cleanup();
		m_pChatClient->Release();
		m_pChatClient = NULL;
	}

	if (m_pTongClient)
	{
		m_pTongClient->Shutdown();
		m_pTongClient->Cleanup();
		m_pTongClient->Release();
		m_pTongClient = NULL;
	}

	IP2CONNECTUNIT::iterator it;
	for (it = m_mapIp2TransferUnit.begin(); it != m_mapIp2TransferUnit.end(); ++it)
	{
		KTransferUnit* pUnit = (*it).second;
		if (pUnit)
		{
			delete pUnit;
			pUnit = NULL;
		}
	}
	m_mapIp2TransferUnit.clear();
#ifdef _STANDALONE
	if (net_buffer)
	   delete net_buffer;

	if  (m_pTransferClient)
	{
		delete m_pTransferClient;	m_pTransferClient = NULL;
	}
	if  (m_pDatabaseClient)
	{	delete m_pDatabaseClient;	m_pDatabaseClient = NULL;}
	if (m_pGatewayClient)
	{	delete m_pGatewayClient;	m_pGatewayClient = NULL;}
#endif

#ifdef _STANDALONE
	g_mutexFlow.Unlock();
#endif
	
}

/////////////////////////////无限主循环线程////////////////////////////////////////////
BOOL KSwordOnLineSever::Breathe()
{

  if (inGo && m_pCoreServerShell && m_bIsRunning)
  {
//---------------------------进入循环前,检测一遍所有服务器的链接------------
        MessageLoop();     //网关协议循环

		if (m_nGameLoop>=1512000)
		{//重起 计时器
			m_nGameLoop = 0;
			m_nGameDay++;
			m_Timer.Passed(1);
			printf("-----重置计时器成功:%lld -----\n",m_Timer.GetElapse());
		}

		LONGLONG nMes = 1000;
		if (nMes * m_nGameLoop <= m_Timer.GetElapse()*GAME_FPS) //1秒18帧  时间溢出 
		{
			if (!GetGo())
			{
				Release();
				return FALSE;
			}

			MainLoop();  //主循环
			Sleep(0);
///////////////////////////////////////////////////////////////////////////////////GetPlayerNumber
			if (m_nGameLoop >= 1080 && (m_nGameLoop%1080)==0)	//每18帧执行一次 一分钟一次
				printf("-----NetCount= %zu, GameLoop= %u, SvrFPS= %lld/18, Time=%ud %u:%u ----- \n",
				m_pServer->GetClientCount(),
				m_nGameLoop,
				m_nGameLoop * 1000/m_Timer.GetElapse(),
				m_nGameDay,
				m_nGameLoop/1080/60,
				m_nGameLoop/1080%60);
/////////////////////////////////////////////////////////////////////////////////////////
		}
		else
		{
#ifndef __linux			//SwitchToThread();
			Sleep(1);
#else
			Sleep(1);  //停止一秒	usleep
#endif
		//	printf("-------时间判断有误,暂停 1 秒--------- \n");
		}
		return TRUE;
	}
	printf("main thread exit\n");
	return TRUE;
}

//--------------------------无限循环获取-网关协议循环----------------------------------
void KSwordOnLineSever::MessageLoop()
{
	int i;
	const char*	pChar = NULL;
	size_t	uSize = 0;
	int    nTiemLoop=0;

#ifndef _STANDALONE
	//CCriticalSection::Owner locker(g_csFlow);
	 KAutoCriticalSection_ AutoLock(g_csFlow);
#else
	g_mutexFlow.Lock();
#endif

	try
	{
	while(m_pGatewayClient)
	{//从BISP获取人物数据失败
	  nTiemLoop++;
	  if (m_nMaxClienLoop>0 && nTiemLoop>m_nMaxClienLoop)
		  break;	//跳出这个循环 每个人可以连续最大接收10次包
	
		pChar = (const char*)m_pGatewayClient->GetPackFromServer(uSize);  //链接Gateway
		if (!pChar|| 0 ==uSize)
			break;
		 GatewayMessageProcess(pChar, uSize); //从Gateway 获取人物数据档案
	}
	nTiemLoop=0;
	uSize = 0;
	pChar = NULL;
	while(m_pDatabaseClient)
	{//无限循环获取数据库信息(排名 和 存档)
		nTiemLoop++;
		if (m_nMaxClienLoop>0 && nTiemLoop>m_nMaxClienLoop)
			break;	//跳出这个循环 每个人可以连续最大接收10次包

		  pChar = (const char*)m_pDatabaseClient->GetPackFromServer(uSize);
		  if (!pChar || 0 == uSize)
			break;
		  DatabaseMessageProcess(pChar, uSize); 
	}
	//发送全局信息
	m_pCoreServerShell->SendNetMsgToTransfer(m_pTransferClient);	//原来有的

	nTiemLoop=0;
	uSize = 0;
	pChar = NULL;
	while(m_pTransferClient)
	{
		nTiemLoop++;
		if (m_nMaxClienLoop>0 && nTiemLoop>m_nMaxClienLoop)
		{
			//printf("---------循环太久(%d) 跳出循环!----------- \n",m_pGameStatus[i].nNetidx);
			break;	//跳出这个循环 每个人可以连续最大接收10次包
		}
		//从S3无限循环获取跨服或中转信息
		pChar = (const char*)m_pTransferClient->GetPackFromServer(uSize);
		if (!pChar || 0 == uSize)
			break;
		TransferMessageProcess(pChar, uSize);
	} 

	m_pCoreServerShell->SendNetMsgToChat(m_pChatClient); 	 	//原来有的

	nTiemLoop=0; 
	uSize = 0;
	pChar = NULL;
	while(m_pChatClient)
	{
		nTiemLoop++;
		if (m_nMaxClienLoop>0 && nTiemLoop>m_nMaxClienLoop)
			break;	//跳出这个循环 每个人可以连续最大接收10次包

		//从S3无限循环获取聊天信息
		pChar = (const char*)m_pChatClient->GetPackFromServer(uSize);
		if (!pChar || 0 == uSize)
			break;
		ChatMessageProcess(pChar, uSize); //发向客户端
	}

	m_pCoreServerShell->SendNetMsgToTong(m_pTongClient);		//原来有的
	nTiemLoop=0;
	uSize = 0;
	pChar = NULL;
	while(m_pTongClient)
	{//从S3无限循环获取帮会信息
		nTiemLoop++;
		if (m_nMaxClienLoop>0 && nTiemLoop>m_nMaxClienLoop)
			break;	//跳出这个循环 每个人可以连续最大接收10次包

		pChar = (const char*)m_pTongClient->GetPackFromServer(uSize);
		if (!pChar || 0 == uSize)
			break;
		TongMessageProcess(pChar, uSize);
	}

	pChar = NULL;
	uSize = 0;
	  for (i = 0; i < m_nMaxGameStaus; i++) //m_nMaxPlayer MAX_PLAYER m_nMaxGameStaus
	  {//最大玩家限制
		  if (enumNetUnconnect == GetNetStatus(i))  //已经断线的
		 	continue;

		  if (m_pGameStatus[i].nNetidx==-1)         //已经断线的
			 continue;
		  
		  if (m_pServer==NULL)
		  {  
			 printf("---------m_pServer=NULL,服务器GS已经断开!----------- \n");
			 continue;
		  } 

		 int nCurLoop=0;
		 while(m_pServer)
		 {//接受客户端玩家消息 发往---各个服务器  接完一个人  再到下一个人
			
			if (m_nMaxClienLoop>0 && nCurLoop>m_nMaxClienLoop)
				break;	//跳出这个循环 每个人可以连续最大接收10次包

			pChar = (const char*)m_pServer->GetPackFromClient(m_pGameStatus[i].nNetidx, uSize);
			if (!pChar || 0 == uSize)
				break;

			nCurLoop ++;
			PlayerFschatProcess(i, pChar, uSize);
            PlayerFsMessageProcess(i, pChar, uSize);
		 }
	  } 
	}
	catch (...)
	{
		TRACE("Core MessageLoop Error\n");
	}

#ifdef _STANDALONE
	g_mutexFlow.Unlock();
#endif
	
}
//聊天信息线程
void KSwordOnLineSever::ChatMessageProcess(const char *pChar, size_t nSize)
{
	//_ASSERT( pChar && nSize);
	if (NULL==pChar|| !pChar || nSize<=0) return;
	
#ifndef _STANDALONE
	BYTE cProtocol = CPackager::Peek(pChar);
#else
	BYTE cProtocol = *(BYTE *)pChar;
#endif	
	int j=0;
	switch (cProtocol)
	{
	case chat_coreitemdata:
		{

			CHAT_GROUPMAN* psR = (CHAT_GROUPMAN*)pChar;

			if  (psR->wChatLength!=202)
				break;
		}
	case chat_goldthread:
		{
			if (m_pCoreServerShell)
				m_pCoreServerShell->doGoldthreadScript(pChar);		
		}
		break;
	case chat_coretask:
		{
			//printf("---执行脚本任务到达----\n");
			//if (m_pCoreServerShell)
			//	m_pCoreServerShell->doTaskScript(pChar);
		} 
	break;
	case chat_caoritemtime:
		{
			//printf("---物品时间检测到达----\n");
			CHAT_GROUPMAN* psR = (CHAT_GROUPMAN*)pChar;
			if  (psR->wChatLength!=201)
				break;

			/*if (m_pCoreServerShell)
				m_pCoreServerShell->CheckItemTime();*/
		}
		break;
	case chat_relegate:  //从S3 获取的协议头  --频道消息发往客户端
		{//附近
			CHAT_RELEGATE* pCR = (CHAT_RELEGATE*)pChar;
			CHAT_CHANNELCHAT_SYNC* pCccSync = (CHAT_CHANNELCHAT_SYNC*)(pCR + 1);
			//printf("DEBUG, Sender: %s\n", pCccSync->someone);
			DWORD	theID = 0;
			switch (pCR->TargetCls)
			{
			case tgtcls_team:  // 组队
			case tgtcls_fac:   // 门派
			case tgtcls_msgr:  // 附近?
				theID = pCR->TargetID;
				break;
			case tgtcls_scrn:  //世界？
				 j = FindSame(pCR->TargetID);
				 if  (j<=0 ||j>m_nMaxGameStaus) return;
				 theID = m_pGameStatus[j].nPlayerIndex;
				break;
			default: 
				return;
			}
			printf("--chat_relegate聊天D --\n");
			m_pCoreServerShell->GroupChat(
				m_pChatClient,
				pCR->nFromIP,
				pCR->nFromRelayID,
				pCR->channelid,
				pCR->TargetCls,
				theID,
				pCR + 1,
				pCR->routeDateLength, pCccSync->someone);
		}
		break;
	case chat_groupman: //城市 和 世界  帮派
		printf("--chat_groupman聊天A --\n");
		ChatGroupMan(pChar, nSize);
		break;
	case chat_specman:// 私聊
		{
 //         printf("<<<<<<<接受从S3返回的私聊信息成功>>>>>> OK..!\n");
		  printf("--chat_specman聊天B --\n");
		  ChatSpecMan(pChar, nSize);  
		}
		break;
	case chat_everyone:   //GM公告
		{
		  printf("--chat_everyone聊天C --\n");
		  CHAT_EVERYONE* pCeo = (CHAT_EVERYONE*)pChar;
		 m_pCoreServerShell->OperationRequest(SSOI_BROADCASTING, (UINT_PTR)(pCeo + 1), pCeo->wChatLength);
		  //m_pCoreServerShell->OperationRequestInfo(SSOI_BROADCASTING, 0, pCeo->wChatLength,0,0,(char *)(pCeo + 1));
		}
		break;
	default:
		break;
	}
}
//////////////////从S3服务器获取帮会相关信息循环线程/////////////////////////////////////////////////
void KSwordOnLineSever::TongMessageProcess(const char *pChar, size_t nSize)
{
	if (NULL==pChar || !pChar)
		return;

	EXTEND_HEADER* pHeader = (EXTEND_HEADER*)pChar;

	if (pHeader->ProtocolFamily == pf_extend)
	{
		if (pHeader->ProtocolID == extend_s2c_passtosomeone)
		{
			EXTEND_PASSTOSOMEONE* pEps = (EXTEND_PASSTOSOMEONE*)pChar;

			if (CheckPlayerID(pEps->lnID ,pEps->nameid)) //网络ID
			{
				_ASSERT(sizeof(tagExtendProtoHeader) <= sizeof(EXTEND_PASSTOSOMEONE));
				unsigned long lnID = pEps->lnID;
				size_t pckgsize = sizeof(tagExtendProtoHeader) + pEps->datasize;
				tagExtendProtoHeader* pExHdr = (tagExtendProtoHeader*)(pEps + 1) - 1;
				pExHdr->ProtocolType = s2c_extendfriend;
				pExHdr->wLength = pckgsize - 1;

				m_pServer->PackDataToClient(lnID, pExHdr, pckgsize);
			}
		}
		else if (pHeader->ProtocolID == extend_s2c_passtobevy)
		{
			EXTEND_PASSTOBEVY* pEpb = (EXTEND_PASSTOBEVY*)pChar;

			if (pEpb->playercount > 0)
			{
				_ASSERT(sizeof(tagExtendProtoHeader) <= sizeof(EXTEND_PASSTOBEVY));
				void* pExPckg = pEpb + 1;
				WORD playercount = pEpb->playercount;
				tagPlusSrcInfo* pPlayers = (tagPlusSrcInfo*)((BYTE*)pExPckg + pEpb->datasize);
				size_t pckgsize = sizeof(tagExtendProtoHeader) + pEpb->datasize;
				tagExtendProtoHeader* pExHdr = (tagExtendProtoHeader*)pExPckg - 1;
				pExHdr->ProtocolType = s2c_extendfriend;
				pExHdr->wLength = pckgsize - 1;

				for (WORD i = 0; i < playercount; ++i)
				{
					if (CheckPlayerID(pPlayers[i].lnID, pPlayers[i].nameid))
						m_pServer->PackDataToClient(pPlayers[i].lnID, pExHdr, pckgsize);
				}
			}
		}
	}
	else if (pHeader->ProtocolFamily == pf_alltong)
	{
		// 协议长度检测
		if (nSize < sizeof(EXTEND_HEADER))
		{
			printf("---------接收帮会宣战信息失败A!-----------\n");
			return;
		}
		if (pHeader->ProtocolID >= enumS2C_TONG_NUM)
		{
			printf("---------接收帮会宣战信息失败B!-----------\n");
			return;
		}
		if (g_nTongPCSize[pHeader->ProtocolID] < 0)
		{
			if (nSize <= sizeof(EXTEND_HEADER) + 2)
			{
				printf("---------接收帮会宣战信息失败C!-----------\n");
				return;
			}
			WORD	wLength = *((WORD*)((BYTE*)pChar + sizeof(EXTEND_HEADER)));
			if (wLength != nSize)
			{
				printf("---------接收帮会宣战信息失败D!-----------\n");
				return;
			}
		}
		/*else if (g_nTongPCSize[pHeader->ProtocolID] != nSize)
		{//协议检测
			printf("---------接收帮会宣战信息失败E!-----------\n");
			return;
		}*/
		
		switch (pHeader->ProtocolID)
		{
		case enumS3C_TONG_SERVER_TIEM:
			{//当前的标准时间
				STONG_ATTACKSTIME_SENDBACK	*pInfo = (STONG_ATTACKSTIME_SENDBACK*)pChar;

				if (pInfo==NULL)
				{
					printf("============程序数据有误,请联系QQ:25557432========== \n");
					//getchar();
					Release();
					Sleep(5000);
					ExitProcess(0);  //退出进程
					break;
				}

				// Tong server time sync received; bypass expiration check
			}
			break;
		case enumS2C_TONG_ALLLIST_INFO:
			{//获取帮会列表信息
				STONG_LIST_INFO_SYNC	*pInfo = (STONG_LIST_INFO_SYNC*)pChar;
				if (NULL==pInfo)
					break;
				if ((int)pInfo->m_dwParam <= 0 || (int)pInfo->m_dwParam >= MAX_PLAYER)
					break;
				
				int nNetID = m_pCoreServerShell->GetGameData(SGDI_CHARACTER_NETID, pInfo->m_dwParam, 0);
				if (nNetID < 0)
					break;
				//printf("---------enumS2C_TONG_ALLLIST_INFO A!-----------\n");
				TONG_LIST_INFO_SYNC	sInfo;
				sInfo.ProtocolType		= s2c_extendtong;
				sInfo.m_btMsgId			= enumTONG_SYNC_ID_LIST_INFO;
				sInfo.m_btStateNo		= pInfo->m_btStartNo;    //开始编号
				sInfo.m_btCurNum		= pInfo->m_btCurNum;     //当前的数量
				sInfo.m_Count           = pInfo->m_TongCount;   //总帮会的数量
				//memcpy(sInfo.m_szTitle, pInfo->m_szTitle, sizeof(pInfo->m_szTitle));
				//memcpy(sInfo.m_szTongName, pInfo->m_szTongName, sizeof(pInfo->m_szTongName));
				memset(sInfo.m_sTongList, 0, sizeof(sInfo.m_sTongList));
				// the page depth is the header's, the count is the peer's: a BYTE reaches 255,
				// and the m_wLength computed below subtracts the same over-count from the
				// struct's size, so a long page is written past the array and sent as well
				if (sInfo.m_btCurNum > defTONG_ONE_PAGE_MAX_NUM)
					sInfo.m_btCurNum = defTONG_ONE_PAGE_MAX_NUM;
				memcpy(sInfo.m_sTongList, pInfo->m_sTongList, sizeof(TONG_ONE_MEMBER_INFO) * sInfo.m_btCurNum);
				sInfo.m_wLength = sizeof(sInfo) - 1 - sizeof(TONG_ONE_MEMBER_INFO) * (defTONG_ONE_PAGE_MAX_NUM - sInfo.m_btCurNum);
				//发往游戏客户端同步数据
				if (m_pServer)
					m_pServer->PackDataToClient(nNetID, &sInfo, sInfo.m_wLength + 1);
				 //printf("---------enumS2C_TONG_ALLLIST_INFO B!-----------\n");
			}
			break;
		case enumS2C_TONG_ATTACK_OVER:
			{
				STONG_ATTACK_SENDBACK	*pOver = (STONG_ATTACK_SENDBACK*)pChar; //接收数据
				if (pOver==NULL)
					break;
				if (m_pCoreServerShell)
				{
					    m_pCoreServerShell->SetAttackCamp(pOver->m_szAttackName,pOver->m_dwParam);
				}

				printf("============帮会(%s)宣战结束==========\n",pOver->m_szAttackName);
			}
			break;
		case  enumS2C_TONG_ATTACK_SENDBACK:
			{
				STONG_ATTACK_SENDBACK	*pSback = (STONG_ATTACK_SENDBACK*)pChar; //接收数据
				if (pSback==NULL)
					break;

				if (pSback->m_dwParam==1)
				{
				    printf("============帮会宣战成功==========\n");
					if (m_pCoreServerShell)
				      m_pCoreServerShell->OperationRequestInfo(SSOI_RELOAD_WELCOME_MSG,1,0,0,(void *)pChar);           //发送欢迎语
				}
				else
				{  	
					if (m_pCoreServerShell)
					   m_pCoreServerShell->SetAttackMsg(/*(int)&(pSback->m_szAttackName),(int)&(pSback->m_szDAttackName)*/(void *)pChar,2);
					printf("============帮会宣战失败:无此帮会或正在宣战==========\n");

				}
			}
			break;
		case enumS3C_TONG_SYNCITY_INFO:
			{//同步七城信息
				STONG_CITY_INFO_SYNC *pSync = (STONG_CITY_INFO_SYNC*)pChar; //接收数据
				if (pSync==NULL)
					break;

				for (int i = 0; i < m_nMaxGameStaus; i++) //m_nMaxPlayer
				{//遍历最大玩家限制
					if (enumNetUnconnect == GetNetStatus(i))
						continue;

					int	nPlayerIdx = m_pGameStatus[i].nPlayerIndex;

					if (nPlayerIdx <= 0 || nPlayerIdx >= MAX_PLAYER)
						continue;

					int nNetID;
						nNetID = m_pGameStatus[i].nNetidx;//m_pCoreServerShell->GetGameData(SGDI_CHARACTER_NETID,nPlayerIdx, 0);//m_pGameStatus[i].nNetidx;//

					if 	(nNetID<0)
						continue;

					m_pCoreServerShell->SetAttackWarCityInfo(pChar,nPlayerIdx); //设置服务器端信息
					//Player[nPlayerIdx].m_cTong.SetAttAckCityInfo(&pSync);    

					//----------设置客户端信息-----------------------------
				    CTONG_CITY_INFO_SYNC	sAinfo;
				    sAinfo.ProtocolType = s2c_extendtong;
				    sAinfo.m_btMsgId = enumTONG_SYNC_ID_CITY_INFO;

					sAinfo.m_WarCityCount =pSync->m_WarCityCount;

					if (sAinfo.m_WarCityCount>7 || sAinfo.m_WarCityCount<=0 )
						sAinfo.m_WarCityCount=7;

				    for(int i=0;i<sAinfo.m_WarCityCount;i++)
					{ //七城信息
					    sAinfo.snWarInfo[i].m_idx=pSync->snWarInfo[i].m_idx;
					    sAinfo.snWarInfo[i].m_mapidx=pSync->snWarInfo[i].m_mapidx;
					    sAinfo.snWarInfo[i].m_levle=pSync->snWarInfo[i].m_levle;
					    sAinfo.snWarInfo[i].m_shushou=pSync->snWarInfo[i].m_shushou;
					    snprintf(sAinfo.snWarInfo[i].m_Mastername, sizeof(sAinfo.snWarInfo[i].m_Mastername), "%s", pSync->snWarInfo[i].m_Mastername);
					    snprintf(sAinfo.snWarInfo[i].m_szMapName, sizeof(sAinfo.snWarInfo[i].m_szMapName), "%s", pSync->snWarInfo[i].m_szMapName);
					    snprintf(sAinfo.snWarInfo[i].m_Tongmaster, sizeof(sAinfo.snWarInfo[i].m_Tongmaster), "%s", pSync->snWarInfo[i].m_Tongmaster);
					}   
					sAinfo.m_wLength = sizeof(sAinfo) - 1;
				   if (m_pServer) //发往客户端
				     	m_pServer->PackDataToClient(nNetID, &sAinfo, sAinfo.m_wLength + 1);

				}
			}
			break;
		case  enumS2C_TONG_ATTACK_GETBACK:
			{//无限接收 同步帮战信息
			   //STONG_ATTACK_INFO_SYNC	*pSync = (STONG_ATTACK_INFO_SYNC*)pChar; //接收数据
			
				//if (m_pCoreServerShell)
				//	nNetID = m_pCoreServerShell->SetAttackInfo(pChar);

				
				/*
				int	nNetID;
				//	nNetID = m_pCoreServerShell->GetGameData(SGDI_CHARACTER_NETID,pSync->m_dwNpcID, 0);
				if 	(nNetID<0)
					break;

				CTONG_ATTTACK_INFO_SYNC	sAinfo;
				sAinfo.ProtocolType = s2c_extendtong;
				sAinfo.m_btMsgId = enumTONG_SYNC_ID_ATTACK_INFO;

				*/
			   /*
				for(int i=0;i<pSync->m_WarCityCount;i++)
				{//七城信息
				  sAinfo.snWarInfo[i].m_idx=pSync->snWarInfo[i].m_idx;
				  sAinfo.snWarInfo[i].m_mapidx=pSync->snWarInfo[i].m_mapidx;
				  sAinfo.snWarInfo[i].m_levle=pSync->snWarInfo[i].m_levle;
				  sAinfo.snWarInfo[i].m_shushou=pSync->snWarInfo[i].m_shushou;
				  sprintf(sAinfo.snWarInfo[i].m_Mastername,pSync->snWarInfo[i].m_Mastername);
				  sprintf(sAinfo.snWarInfo[i].m_szMapName,pSync->snWarInfo[i].m_szMapName);
				  sprintf(sAinfo.snWarInfo[i].m_Tongmaster,pSync->snWarInfo[i].m_Tongmaster);
				} */
				//  sAinfo.m_WarCityCount=pSync->m_WarCityCount;
			/*	  sAinfo.m_AttackState=pSync->m_AttackState;
				  sAinfo.m_AttackTime=pSync->m_AttackTime;
				  sAinfo.m_nTempCamp=pSync->m_nTempCamp;
				  sAinfo.m_nLevel=pSync->m_nLevel;
				  sAinfo.m_YDeathCount=pSync->m_YDeathCount;
	              sAinfo.m_DDeathCount=pSync->m_DDeathCount;//对战的死亡次数
				  sAinfo.m_nAttackNum=pSync->m_nAttackNum;	                        // 参战场数
				  sAinfo.m_nWinNum=pSync->m_nWinNum;								// 胜利场数
	              sAinfo.m_nLoseNum=pSync->m_nLoseNum;                              // 输掉场数
				  sAinfo.m_nMapidx=pSync->m_CurMapIdx;
				  sprintf(sAinfo.m_szAttackName,pSync->m_szAttackName);
				  sAinfo.m_wLength = sizeof(sAinfo) - 1;
			
				if (m_pServer) //发往客户端
					m_pServer->PackDataToClient(nNetID, &sAinfo, sAinfo.m_wLength + 1);
			  */
				    //m_pCoreServerShell->SetAttackInfo(pSync->m_dwNpcID,pSync->m_AttackState,pSync->m_nTempCamp,pSync->m_szAttackName);
				STONG_ATTACK_INFO_SYNC	*pSync = (STONG_ATTACK_INFO_SYNC*)pChar; //接收帮战数据

				if (pSync==NULL)
					break;

				//printf("============接收帮战更新信息成功A ========== \n");
				for (int i = 0; i < m_nMaxGameStaus; i++) //m_nMaxPlayer
				{//遍历最大玩家限制 0 --499
					if (enumNetUnconnect == GetNetStatus(i))
						continue;

					if (m_pGameStatus[i].nNetidx==-1)
						continue;
					
					int	nPlayerIdx = m_pGameStatus[i].nPlayerIndex;
					
					if (nPlayerIdx <= 0 || nPlayerIdx >= MAX_PLAYER)
						continue;

					
					DWORD nTongID;
					char  nTongName[NAME_LENGHT];
					INT_PTR  TongBase;
					     TongBase=m_pCoreServerShell->GetGameData(SGDI_CHECK_TONG,nPlayerIdx,0);

					//printf("============接收帮战更新信息成功B:%d ========== \n",TongBase);

					if  (TongBase==0)
						continue; //下一个

						 strcpy(nTongName,(const char*)TongBase);  //内存复制
						 nTongID=g_FileName2Id(nTongName);

				 if (nTongID == pSync->m_dwParam)
				 {//如果帮会相同
					int nNetID;
					    nNetID = m_pGameStatus[i].nNetidx;//m_pCoreServerShell->GetGameData(SGDI_CHARACTER_NETID,nPlayerIdx, 0);

						m_pCoreServerShell->SetAttackInfo(pChar,nPlayerIdx);

				    if (nNetID<0)
						continue;
					//同步到客户端
				    CTONG_ATTTACK_INFO_SYNC	sAinfo;
				    sAinfo.ProtocolType = s2c_extendtong;
				    sAinfo.m_btMsgId = enumTONG_SYNC_ID_ATTACK_INFO;
				    sAinfo.m_dwParam=pSync->m_dwParam;	 //帮会的名称ID
				    sAinfo.m_dwNpcID=pSync->m_dwNpcID;
				    sAinfo.m_AttackState=pSync->m_AttackState;
				    sAinfo.m_AttackTime=pSync->m_AttackTime;
				    sAinfo.m_nTempCamp=pSync->m_nTempCamp;
				    sAinfo.m_nLevel=pSync->m_nLevel;
				    sAinfo.m_YDeathCount=pSync->m_YDeathCount;
	                sAinfo.m_DDeathCount=pSync->m_DDeathCount;                        //对战的死亡次数
				    sAinfo.m_nAttackNum=pSync->m_nAttackNum;	                        // 参战场数
				    sAinfo.m_nWinNum=pSync->m_nWinNum;								    // 胜利场数
	                sAinfo.m_nLoseNum=pSync->m_nLoseNum;                              // 输掉场数
				    sAinfo.m_nMapidx=pSync->m_CurMapIdx;
				    snprintf(sAinfo.m_szAttackName, sizeof(sAinfo.m_szAttackName), "%s", pSync->m_szAttackName);
				    sAinfo.m_wLength = sizeof(sAinfo) - 1;
			
				    if (m_pServer) //发往客户端
					    m_pServer->PackDataToClient(nNetID, &sAinfo, sAinfo.m_wLength + 1);
				 } 
				} 
//				printf("-接收帮会宣战信息成功(NPCdwid:%d,状态:%d,阵营:%d,时间:%d,帮会:%s)!-\n",pSync->m_dwNpcID,pSync->m_AttackState,pSync->m_nTempCamp,pSync->m_AttackTime,pSync->m_szAttackName);
			}
			break;
			case enumS2C_TONG_CHANGE_DATA:
			{
				STONG_CHANGE_DATA_SYNC* pSync = (STONG_CHANGE_DATA_SYNC*)pChar;
				if (pSync == NULL)
					break;

				for (int i = 0; i < m_nMaxGameStaus; ++i) //m_nMaxPlayer
				{
					if (enumNetUnconnect == GetNetStatus(i))
						continue;

					int	nPlayerIdx = m_pGameStatus[i].nPlayerIndex;
					if (nPlayerIdx <= 0 || nPlayerIdx >= MAX_PLAYER)
						continue;

					int nNetID = m_pGameStatus[i].nNetidx;//m_pCoreServerShell->GetGameData(SGDI_CHARACTER_NETID,nPlayerIdx, 0);//m_pGameStatus[i].nNetidx;//
					if (nNetID < 0)
						continue;

					m_pCoreServerShell->SetTongData(nPlayerIdx, (void*)pSync);
				}
			}
			break;
			case enumS2C_TONG_CHANGE_ANNOUNCE:
			{
				STONG_ANNOUNCE_INFO_SYNC* pSync = (STONG_ANNOUNCE_INFO_SYNC*)pChar;
				if (pSync == NULL)
					break;

				for (int i = 0; i < m_nMaxGameStaus; ++i) 
				{
					if (enumNetUnconnect == GetNetStatus(i))
						continue;

					int	nPlayerIdx = m_pGameStatus[i].nPlayerIndex;
					if (nPlayerIdx <= 0 || nPlayerIdx >= MAX_PLAYER)
						continue;

					int nNetID = m_pGameStatus[i].nNetidx;
					if (nNetID < 0)
						continue;

					TONG_ANNOUNCE_SYNC	tChange;
					tChange.ProtocolType = s2c_extendtong;
					tChange.m_btMsgId = enumTONG_SYNC_ID_CHANGE_ANNOUNCE;
					strcpy(tChange.m_szAnnounce, pSync->m_szAnnounce);
					tChange.m_wLength = sizeof(tChange) - 1;

					if (m_pServer)
						m_pServer->PackDataToClient(nNetID, &tChange, tChange.m_wLength + 1);
				}
			}
			break;
			case enumS2C_TONG_CHANGE_TITLE:
			{
				STONG_CHANGE_TITLE_SYNC* pChange = (STONG_CHANGE_TITLE_SYNC*)pChar;
				if (!pChange)
					break;

				int nPlayerIdx = m_pGameStatus[FindSame(pChange->m_dwParam)].nPlayerIndex;
				if (nPlayerIdx <= 0 || nPlayerIdx >= MAX_PLAYER)
					break;

				STONG_SERVER_TO_CORE_CHANGE_TITLE sChange;
				sChange.m_nPlayerIdx = nPlayerIdx;
				sChange.m_dwTongNameID = pChange->m_dwTongNameID;
				strcpy(sChange.m_szTitle, pChange->m_szTitle);

				//printf("DEBUG, sChange.m_nPlayerIdx: %d\n", sChange.m_nPlayerIdx);
				//printf("DEBUG, sChange.m_szTitle: %s\n", sChange.m_szTitle);

				m_pCoreServerShell->GetGameDataInfo(SGDI_TONG_CHANGE_TITLE, 0, 0, 0, &sChange);
			}
			break;
		default:
			break;
		}
	}
	else if (pHeader->ProtocolFamily == pf_tong)
	{
		// 协议长度检测
		if (nSize < sizeof(EXTEND_HEADER))
			return;
		if (pHeader->ProtocolID >= enumS2C_TONG_NUM)
			return;
		if (g_nTongPCSize[pHeader->ProtocolID] < 0)
		{
			if (nSize <= sizeof(EXTEND_HEADER) + 2)
				return;
			WORD	wLength = *((WORD*)((BYTE*)pChar + sizeof(EXTEND_HEADER)));
			if (wLength != nSize)
				return;
		}
		else if (g_nTongPCSize[pHeader->ProtocolID] != nSize)
		{
			return;
		}

		switch (pHeader->ProtocolID)
		{
		case enumS2C_TONG_CREATE_SUCCESS:
			{//创建帮会成功
				STONG_CREATE_SUCCESS_SYNC	*pSync = (STONG_CREATE_SUCCESS_SYNC*)pChar;
				if  (!pSync) break;
				//同步给客户端
				STONG_SERVER_TO_CORE_CREATE_SUCCESS sSuccess;
				sSuccess.m_nCamp = pSync->m_btCamp;
				sSuccess.m_dwPlayerNameID = pSync->m_dwPlayerNameID;
				sSuccess.m_nPlayerIdx = pSync->m_dwParam;
				// m_btTongNameLength is the packet's own int: below zero the copy is a size_t
				// the width of the whole address space, and at NAME_LENGHT the terminator this
				// next line writes is one element past the field
				int nTongNameLen = pSync->m_btTongNameLength;
				if (nTongNameLen < 0 || nTongNameLen >= NAME_LENGHT)
					break;
				memcpy(sSuccess.m_szTongName, pSync->m_szTongName, nTongNameLen);
				sSuccess.m_szTongName[nTongNameLen] = 0;
				strcpy(sSuccess.m_szMasterTitle, pSync->m_szTitle);
                //通知客户端和S3创建频道
				//m_pCoreServerShell->OperationRequest(SSOI_TONG_CREATE, (unsigned int)&sSuccess, 0);	
				m_pCoreServerShell->OperationRequestInfo(SSOI_TONG_CREATE, 0, 0,0,&sSuccess);
				printf("<<<<<<<(%u)创建帮会(%s)成功>>>>>> OK..!\n",sSuccess.m_dwPlayerNameID, sSuccess.m_szTongName);
			}
			break;
		case enumS2C_TONG_CREATE_FAIL:
			{//创建帮会失败
				STONG_CREATE_FAIL_SYNC	*pFail = (STONG_CREATE_FAIL_SYNC*)pChar;
				int		nNetID;
				TONG_CREATE_FAIL_SYNC	sFail;

				sFail.ProtocolType = s2c_extendtong;
				sFail.m_btMsgId = enumTONG_SYNC_ID_CREATE_FAIL;
				sFail.m_btFailId = pFail->m_btFailID;
				sFail.m_wLength = sizeof(sFail) - 1;
				nNetID = m_pCoreServerShell->GetGameData(SGDI_CHARACTER_NETID, pFail->m_dwParam, 0);
				if (m_pServer)
					m_pServer->PackDataToClient(nNetID, &sFail, sFail.m_wLength + 1);
			
      			printf("<<<<<<<创建帮会失败>>>>>> OK..!\n");
			}
			break;
		case enumS2C_TONG_ADD_MEMBER_SUCCESS:
			{//添加成员成功
				STONG_ADD_MEMBER_SUCCESS_SYNC	*pSync = (STONG_ADD_MEMBER_SUCCESS_SYNC*)pChar;
				STONG_SERVER_TO_CORE_ADD_SUCCESS	sAdd;

				sAdd.m_nCamp = pSync->m_btCamp;
				sAdd.m_dwPlayerNameID = pSync->m_dwPlayerNameID;
				sAdd.m_nPlayerIdx = pSync->m_dwParam;
				memcpy(sAdd.m_szMasterName, pSync->m_szMasterName, sizeof(sAdd.m_szMasterName));
				memcpy(sAdd.m_szTitleName, pSync->m_szTitle, sizeof(sAdd.m_szTitleName));
				memcpy(sAdd.m_szTongName, pSync->m_szTongName, sizeof(sAdd.m_szTongName));
				m_pCoreServerShell->OperationRequestInfo(SSOI_TONG_ADD, 0, 0,0,&sAdd);
				//m_pCoreServerShell->OperationRequest(SSOI_TONG_CREATE, (unsigned int)&sAdd, 0);
			}
			break;
		case enumS2C_TONG_ADD_MEMBER_FAIL:
			{//添加成员失败
			    printf("警告:<<<<<<<帮会成员添加失败!>>>>>> ..!\n");
			 //	STONG_ADD_MEMBER_FAIL_SYNC	*pSync = (STONG_ADD_MEMBER_FAIL_SYNC*)pChar;
			}
			break;

		case enumS2C_TONG_HEAD_INFO: 
			{//接受S3发过来的帮会数据包=服务器端发送帮会信息给客户端
				STONG_HEAD_INFO_SYNC	*pInfo = (STONG_HEAD_INFO_SYNC*)pChar;
				if ((int)pInfo->m_dwParam <= 0 || (int)pInfo->m_dwParam >= MAX_PLAYER)
					break;
                //获取某人的网络连接号
				int nNetID = m_pCoreServerShell->GetGameData(SGDI_CHARACTER_NETID, pInfo->m_dwParam, 0);
				if (nNetID < 0)
					break;

				TONG_HEAD_INFO_SYNC	sInfo;
				sInfo.ProtocolType		= s2c_extendtong;
				sInfo.m_btMsgId			= enumTONG_SYNC_ID_HEAD_INFO;
				//sInfo.m_dwNpcID			= pInfo->m_dwNpcID;
				sInfo.m_dwMoney			= pInfo->m_dwMoney;
				sInfo.m_nCredit			= pInfo->m_nCredit;
				sInfo.m_btCamp			= pInfo->m_btCamp;
				sInfo.m_btLevel			= pInfo->m_btLevel;
				sInfo.m_btDirectorNum	= pInfo->m_btDirectorNum;
				sInfo.m_btManagerNum	= pInfo->m_btManagerNum;
				sInfo.m_dwMemberNum		= pInfo->m_dwMemberNum;
				strcpy(sInfo.m_szAnnounce, pInfo->m_szAnnounce);

				KTabFile nApplyList;
				char nApplyListPath[128];
				sprintf(nApplyListPath,"\\TongSet\\%s.txt",pInfo->m_szTongName);
				if (nApplyList.Load(nApplyListPath))
                    sInfo.m_btApplyNum = nApplyList.GetHeight();
				else
                    sInfo.m_btApplyNum = 0;
                nApplyList.Clear();

				KIniFile nTongZhaoMu; 
				if  (nTongZhaoMu.Load("\\TongSet\\TongSet.ini"))
					sInfo.m_btZhaoMuNum = nTongZhaoMu.GetSectionCount();
				else
                    sInfo.m_btZhaoMuNum = 0;
				nTongZhaoMu.Clear();

				memcpy(sInfo.m_szTongName, pInfo->m_szTongName, sizeof(pInfo->m_szTongName));//帮会名
				memset(sInfo.m_sMember, 0, sizeof(sInfo.m_sMember));                          //清空
				// one slot for the chief plus a director count the peer chose, against
				// m_sMember[1 + defTONG_MAX_DIRECTOR]: the chief's slot always goes out, so
				// it is the directors that have to stop at the array's own depth
				if (sInfo.m_btDirectorNum > defTONG_MAX_DIRECTOR)
					sInfo.m_btDirectorNum = defTONG_MAX_DIRECTOR;
				memcpy(sInfo.m_sMember, pInfo->m_sMember, sizeof(TONG_ONE_LEADER_INFO) * (1 + sInfo.m_btDirectorNum));  //帮主和长老一起发送
				sInfo.m_wLength = sizeof(sInfo) - 1 - sizeof(sInfo.m_sMember) + sizeof(TONG_ONE_LEADER_INFO) * (1 + sInfo.m_btDirectorNum);
			
				if (m_pServer)
					m_pServer->PackDataToClient(nNetID, &sInfo, sInfo.m_wLength + 1);
			}
			break;

		case enumS2C_TONG_MANAGER_INFO:
			{//获取队长的信息 发往客户端
				STONG_MANAGER_INFO_SYNC	*pInfo = (STONG_MANAGER_INFO_SYNC*)pChar;
				if ((int)pInfo->m_dwParam <= 0 || (int)pInfo->m_dwParam >= MAX_PLAYER)
					break;

				int nNetID = m_pCoreServerShell->GetGameData(SGDI_CHARACTER_NETID, pInfo->m_dwParam, 0);
				if (nNetID < 0)
					break;

				TONG_MANAGER_INFO_SYNC	sInfo;
				sInfo.ProtocolType		= s2c_extendtong;
				sInfo.m_btMsgId			= enumTONG_SYNC_ID_MANAGER_INFO;
				sInfo.m_dwMoney			= pInfo->m_dwMoney;
				sInfo.m_nCredit			= pInfo->m_nCredit;
				sInfo.m_btCamp			= pInfo->m_btCamp;
				sInfo.m_btLevel			= pInfo->m_btLevel;
				sInfo.m_btDirectorNum	= pInfo->m_btDirectorNum;
				sInfo.m_btManagerNum	= pInfo->m_btManagerNum;
				sInfo.m_dwMemberNum		= pInfo->m_dwMemberNum;
				sInfo.m_btStateNo		= pInfo->m_btStartNo;
				sInfo.m_btCurNum		= pInfo->m_btCurNum;
				memcpy(sInfo.m_szTongName, pInfo->m_szTongName, sizeof(pInfo->m_szTongName));
				memset(sInfo.m_sMember, 0, sizeof(sInfo.m_sMember));
				// the manager page, same pair of counts as the tong list above
				if (sInfo.m_btCurNum > defTONG_ONE_PAGE_MAX_NUM)
					sInfo.m_btCurNum = defTONG_ONE_PAGE_MAX_NUM;
				memcpy(sInfo.m_sMember, pInfo->m_sMember, sizeof(TONG_ONE_LEADER_INFO) * sInfo.m_btCurNum);
				sInfo.m_wLength = sizeof(sInfo) - 1 - sizeof(TONG_ONE_LEADER_INFO) * (defTONG_ONE_PAGE_MAX_NUM - sInfo.m_btCurNum);
				if (m_pServer)
					m_pServer->PackDataToClient(nNetID, &sInfo, sInfo.m_wLength + 1);
			}
			break;

		case enumS2C_TONG_MEMBER_INFO:
			{//获取帮众的信息 发往客户端
				STONG_MEMBER_INFO_SYNC	*pInfo = (STONG_MEMBER_INFO_SYNC*)pChar;
				if ((int)pInfo->m_dwParam <= 0 || (int)pInfo->m_dwParam >= MAX_PLAYER)
					break;

				int nNetID = m_pCoreServerShell->GetGameData(SGDI_CHARACTER_NETID, pInfo->m_dwParam, 0);
				if (nNetID < 0)
					break;

				TONG_MEMBER_INFO_SYNC	sInfo;
				sInfo.ProtocolType		= s2c_extendtong;
				sInfo.m_btMsgId			= enumTONG_SYNC_ID_MEMBER_INFO;
				sInfo.m_dwMoney			= pInfo->m_dwMoney;
				sInfo.m_nCredit			= pInfo->m_nCredit;
				sInfo.m_btCamp			= pInfo->m_btCamp;
				sInfo.m_btLevel			= pInfo->m_btLevel;
				sInfo.m_btDirectorNum	= pInfo->m_btDirectorNum;
				sInfo.m_btManagerNum	= pInfo->m_btManagerNum;
				sInfo.m_dwMemberNum		= pInfo->m_dwMemberNum;
				sInfo.m_btStateNo		= pInfo->m_btStartNo;   //开始编号
				sInfo.m_btCurNum		= pInfo->m_btCurNum;    //当前的数量
				memcpy(sInfo.m_szTitle, pInfo->m_szTitle, sizeof(pInfo->m_szTitle));
				memcpy(sInfo.m_szTongName, pInfo->m_szTongName, sizeof(pInfo->m_szTongName));
				memset(sInfo.m_sMember, 0, sizeof(sInfo.m_sMember));
				// and the member page
				if (sInfo.m_btCurNum > defTONG_ONE_PAGE_MAX_NUM)
					sInfo.m_btCurNum = defTONG_ONE_PAGE_MAX_NUM;
				memcpy(sInfo.m_sMember, pInfo->m_sMember, sizeof(TONG_ONE_MEMBER_INFO) * sInfo.m_btCurNum);
				sInfo.m_wLength = sizeof(sInfo) - 1 - sizeof(TONG_ONE_MEMBER_INFO) * (defTONG_ONE_PAGE_MAX_NUM - sInfo.m_btCurNum);
				//
				if (m_pServer)
					m_pServer->PackDataToClient(nNetID, &sInfo, sInfo.m_wLength + 1);
			}
			break;

		case enumS2C_TONG_BE_INSTATED:
			{//被任命
				STONG_BE_INSTATED_SYNC	*pSync = (STONG_BE_INSTATED_SYNC*)pChar;
				
				if  (!pSync)
					break;
				
				int nPlayerIdx = m_pGameStatus[FindSame(pSync->m_dwParam)].nPlayerIndex;

				if (nPlayerIdx <= 0 || nPlayerIdx >= MAX_PLAYER)
					break;

				STONG_SERVER_TO_CORE_BE_INSTATED	sInstated;
				sInstated.m_nPlayerIdx = nPlayerIdx;
				sInstated.m_btFigure = pSync->m_btFigure;
				sInstated.m_btPos = pSync->m_btPos;
				memcpy(sInstated.m_szName, pSync->m_szName, sizeof(pSync->m_szName));
				memcpy(sInstated.m_szTitle, pSync->m_szTitle, sizeof(pSync->m_szTitle));

				//printf("DEBUG, pSync->m_dwParam: %d\n", pSync->m_dwParam);
				//printf("DEBUG, sInstated.m_nPlayerIdx: %d\n", sInstated.m_nPlayerIdx);
				//printf("DEBUG, sInstated.m_btFigure: %d\n", sInstated.m_btFigure);
				//printf("DEBUG, sInstated.m_btPos: %d\n", sInstated.m_btPos);
				//printf("DEBUG, sInstated.m_szName: %s\n", sInstated.m_szName);
				//printf("DEBUG, sInstated.m_szTitle: %s\n", sInstated.m_szTitle);

				m_pCoreServerShell->GetGameDataInfo(SGDI_TONG_BE_INSTATED, 0, 0,0,&sInstated);
			}
			break;

		case enumS2C_TONG_INSTATE:
			{//任命职位
				STONG_INSTATE_SYNC	*pSync = (STONG_INSTATE_SYNC*)pChar;
				TONG_INSTATE_SYNC	sInstate;

				if ((int)pSync->m_dwParam <= 0 || (int)pSync->m_dwParam >= MAX_PLAYER)
					break;

				int nNetID = m_pCoreServerShell->GetGameData(SGDI_CHARACTER_NETID, pSync->m_dwParam, 0);
				if (nNetID < 0)
					break;

				sInstate.ProtocolType		= s2c_extendtong;
				sInstate.m_btMsgId			= enumTONG_SYNC_ID_INSTATE;
				sInstate.m_wLength			= sizeof(sInstate) - 1;
				sInstate.m_dwTongNameID		= pSync->m_dwTongNameID;
				sInstate.m_btSuccessFlag	= pSync->m_btSuccessFlag;
				sInstate.m_btOldFigure		= pSync->m_btOldFigure;
				sInstate.m_btOldPos			= pSync->m_btOldPos;
				sInstate.m_btNewFigure		= pSync->m_btNewFigure;
				sInstate.m_btNewPos			= pSync->m_btNewPos;
				memcpy(sInstate.m_szTitle, pSync->m_szTitle, sizeof(pSync->m_szTitle));
				memcpy(sInstate.m_szName, pSync->m_szName, sizeof(pSync->m_szName));

				//printf("DEBUG, pSync->m_dwParam: %d\n", pSync->m_dwParam);
				//printf("DEBUG, sInstate.m_dwTongNameID: %d\n", sInstate.m_dwTongNameID);
				//printf("DEBUG, sInstate.m_btSuccessFlag: %d\n", sInstate.m_btSuccessFlag);
				//printf("DEBUG, sInstate.m_btOldFigure: %d\n", sInstate.m_btOldFigure);
				//printf("DEBUG, sInstate.m_btOldPos: %d\n", sInstate.m_btOldPos);
				//printf("DEBUG, sInstate.m_btNewFigure: %d\n", sInstate.m_btNewFigure);
				//printf("DEBUG, sInstate.m_btNewPos: %d\n", sInstate.m_btNewPos);
				//printf("DEBUG, sInstate.m_szTitle: %s\n", sInstate.m_szTitle);
				//printf("DEBUG, sInstate.m_szName: %s\n", sInstate.m_szName);

				if (m_pServer)
					m_pServer->PackDataToClient(nNetID, &sInstate, sInstate.m_wLength + 1);
			}
			break;

		case enumS2C_TONG_KICK:
			{
				STONG_KICK_SYNC	*pSync = (STONG_KICK_SYNC*)pChar;
				TONG_KICK_SYNC	sKick;

				if ((int)pSync->m_dwParam <= 0 || (int)pSync->m_dwParam >= MAX_PLAYER)
					break;

				int nNetID = m_pCoreServerShell->GetGameData(SGDI_CHARACTER_NETID, pSync->m_dwParam, 0);
				if (nNetID < 0)
					break;

				sKick.ProtocolType		= s2c_extendtong;
				sKick.m_btMsgId			= enumTONG_SYNC_ID_KICK;
				sKick.m_wLength			= sizeof(sKick) - 1;
				sKick.m_dwTongNameID	= pSync->m_dwTongNameID;
				sKick.m_btSuccessFlag	= pSync->m_btSuccessFlag;
				sKick.m_btFigure		= pSync->m_btFigure;
				sKick.m_btPos			= pSync->m_btPos;
				memcpy(sKick.m_szName, pSync->m_szName, sizeof(pSync->m_szName));

				if (m_pServer)
					m_pServer->PackDataToClient(nNetID, &sKick, sKick.m_wLength + 1);
			}
			break;

		case enumS2C_TONG_BE_KICKED:
			{
				STONG_BE_KICKED_SYNC* pSync = (STONG_BE_KICKED_SYNC*)pChar;
				if (!pSync)
					break;

				int nPlayerIdx = m_pGameStatus[FindSame(pSync->m_dwParam)].nPlayerIndex;

				if (nPlayerIdx <= 0 || nPlayerIdx >= MAX_PLAYER)
					break;

				STONG_SERVER_TO_CORE_BE_KICKED	sKicked;
				sKicked.m_nPlayerIdx = nPlayerIdx;
				sKicked.m_btFigure = pSync->m_btFigure;
				sKicked.m_btPos = pSync->m_btPos;
				memcpy(sKicked.m_szName, pSync->m_szName, sizeof(pSync->m_szName));
				m_pCoreServerShell->GetGameDataInfo(SGDI_TONG_BE_KICKED, 0, 0, 0, &sKicked);
			}
		break;

		case enumS2C_TONG_LEAVE:
			{//叛离帮会
				STONG_LEAVE_SYNC	*pSync = (STONG_LEAVE_SYNC*)pChar;
				STONG_SERVER_TO_CORE_LEAVE	sLeave;
				sLeave.m_nPlayerIdx = pSync->m_dwParam;
				sLeave.m_bSuccessFlag = pSync->m_btSuccessFlag;
				memcpy(sLeave.m_szName, pSync->m_szName, sizeof(pSync->m_szName));
				m_pCoreServerShell->GetGameDataInfo(SGDI_TONG_LEAVE,0, 0,0,&sLeave);
			}
			break;

		case enumS2C_TONG_CHECK_CHANGE_MASTER_POWER:
			{
				STONG_CHECK_GET_MASTER_POWER_SYNC	*pSync = (STONG_CHECK_GET_MASTER_POWER_SYNC*)pChar;
				if (!pSync)
					break;

				int nPlayerIdx = m_pGameStatus[FindSame(pSync->m_dwParam)].nPlayerIndex;

				if (nPlayerIdx <= 0 || nPlayerIdx >= MAX_PLAYER)
					break;

				STONG_SERVER_TO_CORE_CHECK_GET_MASTER_POWER	sCheck;
				sCheck.m_dwTongNameID	= pSync->m_dwTongNameID;
				sCheck.m_btFigure		= pSync->m_btFigure;
				sCheck.m_btPos			= pSync->m_btPos;
				sCheck.m_nPlayerIdx = nPlayerIdx;
				memcpy(sCheck.m_szName, pSync->m_szName, sizeof(pSync->m_szName));

				int nRet = 0;
				if (m_pCoreServerShell)
					nRet = m_pCoreServerShell->GetGameDataInfo(SGDI_TONG_GET_MASTER_POWER, 0, 0,0,&sCheck);

				STONG_ACCEPT_MASTER_COMMAND	sAccept;

				sAccept.ProtocolFamily	= pf_tong;
				sAccept.ProtocolID		= enumC2S_TONG_ACCEPT_MASTER;
				sAccept.m_dwParam		= nPlayerIdx;
				sAccept.m_dwTongNameID	= pSync->m_dwTongNameID;
				sAccept.m_btFigure		= pSync->m_btFigure;
				sAccept.m_btPos			= pSync->m_btPos;
				if (nRet)
					sAccept.m_btAcceptFalg = 1;
				else
					sAccept.m_btAcceptFalg = 0;
				memcpy(sAccept.m_szName, pSync->m_szName, sizeof(pSync->m_szName));
				
				if (m_pTongClient)
					m_pTongClient->SendPackToServer((const void*)&sAccept, sizeof(sAccept));
			}
			break;

		case enumS2C_TONG_CHANGE_MASTER_FAIL:
			{//传位失败
				STONG_CHANGE_MASTER_FAIL_SYNC	*pFail = (STONG_CHANGE_MASTER_FAIL_SYNC*)pChar;

				if (!pFail)
					break;

				int nPlayerIdx = m_pGameStatus[FindSame(pFail->m_dwParam)].nPlayerIndex;

				if (nPlayerIdx <= 0 || nPlayerIdx >= MAX_PLAYER)
					break;

				int nPlayer = pFail->m_dwParam;
				switch (pFail->m_btFailID)
				{
				case 0:
					break;
				case 1:
					nPlayer = nPlayerIdx;	 //玩家链表索引号
					break;
				default:
					break;
				}

				if (nPlayer <= 0 || nPlayer >= MAX_PLAYER)
					break;
				int nNetID = m_pCoreServerShell->GetGameData(SGDI_CHARACTER_NETID, nPlayer, 0);
				if (nNetID < 0)
					break;

				TONG_CHANGE_MASTER_FAIL_SYNC	sFail;
				sFail.ProtocolType		= s2c_extendtong;
				sFail.m_btMsgId			= enumTONG_SYNC_ID_CHANGE_MASTER_FAIL;
				sFail.m_wLength			= sizeof(sFail) - 1;
				sFail.m_dwTongNameID	= pFail->m_dwTongNameID;
				sFail.m_btFailID		= pFail->m_btFailID;
				memcpy(sFail.m_szName, pFail->m_szName, sizeof(pFail->m_szName));
				if (m_pServer)
					m_pServer->PackDataToClient(nNetID, &sFail, sFail.m_wLength + 1);
			}
			break;

		case enumS2C_TONG_CHANGE_AS:
			{
				STONG_CHANGE_AS_SYNC	*pAs = (STONG_CHANGE_AS_SYNC*)pChar;

				if (!pAs)
					break;

				int nPlayerIdx = m_pGameStatus[FindSame(pAs->m_dwParam)].nPlayerIndex;

				if (nPlayerIdx <= 0 || nPlayerIdx >= MAX_PLAYER)
					break;

				STONG_SERVER_TO_CORE_CHANGE_AS	sChange;
				sChange.m_nPlayerIdx = nPlayerIdx;
				sChange.m_dwTongNameID = pAs->m_dwTongNameID;
				sChange.m_btFigure = pAs->m_btFigure;
				sChange.m_btPos = pAs->m_btPos;
				// 自己的新头衔
				memcpy(sChange.m_szTitle, pAs->m_szTitle, sizeof(pAs->m_szTitle));
				// 新帮主的名字
				memcpy(sChange.m_szName, pAs->m_szName, sizeof(pAs->m_szName));
				m_pCoreServerShell->GetGameDataInfo(SGDI_TONG_CHANGE_AS, 0, 0,0,&sChange);
			}
			break;

		case enumS2C_TONG_CHANGE_MASTER:
			{
				STONG_CHANGE_MASTER_SYNC	*pMaster = (STONG_CHANGE_MASTER_SYNC*)pChar;
				STONG_SERVER_TO_CORE_CHANGE_MASTER	sChange;
				sChange.m_dwTongNameID = pMaster->m_dwTongNameID;
				memcpy(sChange.m_szName, pMaster->m_szName, sizeof(pMaster->m_szName));
				m_pCoreServerShell->GetGameDataInfo(SGDI_TONG_CHANGE_MASTER, 0, 0,0,&sChange);
			}
			break;
		case enumS2C_TONG_MONEY_SAVE:
			{
				STONG_MONEY_SYNC	*pMoney = (STONG_MONEY_SYNC*)pChar;
				STONG_SERVER_TO_CORE_MONEY pChange;
				pChange.m_dwTongNameID = pMoney->m_dwTongNameID;
				pChange.m_dwMoney      = pMoney->m_dwMoney;
				pChange.m_nMoney       = pMoney->m_nMoney;
				pChange.nType          = 0;
				pChange.m_nPlayerIdx   = pMoney->m_dwParam;
				if (pChange.m_nPlayerIdx <= 0)
					break;
				m_pCoreServerShell->GetGameDataInfo(SGDI_TONG_CHANGE_MONEY, 0, 0,0,&pChange);
			}
			break;
		case enumS2C_TONG_MONEY_GET:
			{
				STONG_MONEY_SYNC	*pMoney = (STONG_MONEY_SYNC*)pChar;
				STONG_SERVER_TO_CORE_MONEY pChange;
				pChange.m_dwTongNameID = pMoney->m_dwTongNameID;
				pChange.m_dwMoney      = pMoney->m_dwMoney;
				pChange.m_nMoney       = pMoney->m_nMoney;
				pChange.nType          = 1;
				pChange.m_nPlayerIdx   = pMoney->m_dwParam;
				if (pChange.m_nPlayerIdx <= 0)
					break;
				m_pCoreServerShell->GetGameDataInfo(SGDI_TONG_CHANGE_MONEY, 0, 0,0,&pChange);
			}
			break;
		case enumS2C_TONG_MONEY_SND:
			{
				STONG_MONEY_SYNC	*pMoney = (STONG_MONEY_SYNC*)pChar;
				STONG_SERVER_TO_CORE_MONEY pChange;
				pChange.m_dwTongNameID = pMoney->m_dwTongNameID;
				pChange.m_dwMoney      = pMoney->m_dwMoney;
				pChange.m_nMoney       = pMoney->m_nMoney;
				pChange.nType          = 2;
				pChange.m_nPlayerIdx = pMoney->m_dwParam;
				if (pChange.m_nPlayerIdx <= 0)
					break;
				m_pCoreServerShell->GetGameDataInfo(SGDI_TONG_CHANGE_MONEY,0, 0,0,&pChange);
			}
			break;

		case enumS2C_TONG_LOGIN_DATA:
			{//登录时候 设置帮会信息
				STONG_LOGIN_DATA_SYNC	*pLogin = (STONG_LOGIN_DATA_SYNC*)pChar;
				STONG_SERVER_TO_CORE_LOGIN	sLogin;
                memset(&sLogin,0,sizeof(sLogin));
				sLogin.m_dwParam	= pLogin->m_dwParam;
				sLogin.m_nFlag		= pLogin->m_btFlag;
				sLogin.m_nCamp		= pLogin->m_btCamp;
				sLogin.m_nFigure	= pLogin->m_btFigure;
				sLogin.m_nPos		= pLogin->m_btPos;
				sLogin.m_nMoney		= pLogin->m_nMoney;
				sLogin.m_nDeathCount= pLogin->m_nDeathCount;

				if  (pLogin->m_szTongName[0])
				     memcpy(sLogin.m_szTongName, pLogin->m_szTongName, sizeof(pLogin->m_szTongName));
				if  (pLogin->m_szTitle[0])
				     memcpy(sLogin.m_szTitle, pLogin->m_szTitle, sizeof(pLogin->m_szTitle));
				if  (pLogin->m_szMaster[0])
				     memcpy(sLogin.m_szMaster, pLogin->m_szMaster, sizeof(pLogin->m_szMaster));
				if  (pLogin->m_szName[0])
				     memcpy(sLogin.m_szName, pLogin->m_szName, sizeof(pLogin->m_szName));

				sLogin.m_AttackState=pLogin->m_AttackState;
				sLogin.m_nTempCamp=pLogin->m_nTempCamp;
				sLogin.m_AttackTime=pLogin->m_AttackTime;
				sLogin.m_nLevel=pLogin->m_nLevel;
			///ogin.m_szAttackName=pLogin->m_szAttackName;
				if  (pLogin->m_szAttackName[0])
				     memcpy(sLogin.m_szAttackName, pLogin->m_szAttackName, sizeof(pLogin->m_szAttackName));
				m_pCoreServerShell->GetGameDataInfo(SGDI_TONG_LOGIN, 0, 0,0,&sLogin);	 //设置帮会信息
			}
			break;
		case enumS2C_TONG_SEARCH_INFO:
		{
			STONG_SEARCH_INFO_SYNC* pSearch = (STONG_SEARCH_INFO_SYNC*)pChar;
			if (!pSearch)
				break;

			if (pSearch->m_nPlayerIndex <= 0 || pSearch->m_nPlayerIndex >= MAX_PLAYER)
				break;

			int nNetID = m_pCoreServerShell->GetGameData(SGDI_CHARACTER_NETID, pSearch->m_nPlayerIndex, 0);
			if (nNetID < 0)
				break;

			TONG_SEARCH_RESULT_SYNC	sSync;
			sSync.ProtocolType = s2c_extendtong;
			sSync.m_btMsgId = enumTONG_SYNC_ID_SEARCH_RESULT;
			sSync.m_wLength = sizeof(sSync) - 1;
			sSync.m_nType = pSearch->m_nType;
			sSync.m_nCurNum = pSearch->m_nCurNum;
			sSync.m_btStateNo = pSearch->m_nStart;
			//printf("DEBUG, sSync.m_nType: %d\n", sSync.m_nType);
			//printf("DEBUG, sSync.m_nCurNum: %d\n", sSync.m_nCurNum);
			//printf("DEBUG, sSync.m_btStateNo: %d\n", sSync.m_btStateNo);
			for (int i = 0; i < sSync.m_nCurNum; ++i)
			{
				sSync.m_sMember[i].m_btFigure = pSearch->m_sMember[i].m_btFigure;
				sSync.m_sMember[i].m_btPos = pSearch->m_sMember[i].m_btPos;
				sSync.m_sMember[i].m_nPoint = pSearch->m_sMember[i].m_nPoint;
				snprintf(sSync.m_sMember[i].m_szName, sizeof(sSync.m_sMember[i].m_szName), "%s", pSearch->m_sMember[i].m_szName);

				//printf("DEBUG, sSync.m_sMember[i].m_btFigure: %d\n", sSync.m_sMember[i].m_btFigure);
				//printf("DEBUG, sSync.m_sMember[i].m_btPos: %d\n", sSync.m_sMember[i].m_btPos);
				//printf("DEBUG, sSync.m_sMember[i].m_nPoint: %d\n", sSync.m_sMember[i].m_nPoint);
				//printf("DEBUG, sSync.m_sMember[i].m_szName: %s\n", sSync.m_sMember[i].m_szName);
			}



			if (m_pServer)
				m_pServer->PackDataToClient(nNetID, &sSync, sSync.m_wLength + 1);
		}
		break;
		case enumS2C_TONG_SET_POWER:
		{
			STONG_POWER_SYNC* pSync = (STONG_POWER_SYNC*)pChar;
			if (!pSync)
				break;

			int nPlayerIdx = -1;

			if (pSync->m_bSetted)
			{
				nPlayerIdx = m_pGameStatus[FindSame(pSync->m_dwParam)].nPlayerIndex;
				if (nPlayerIdx <= 0 || nPlayerIdx >= MAX_PLAYER)
					break;
			}
			else
			{
				if (pSync->m_dwParam <= 0 || pSync->m_dwParam >= MAX_PLAYER)
					break;
				nPlayerIdx = pSync->m_dwParam;
			}


			int nNetID = m_pCoreServerShell->GetGameData(SGDI_CHARACTER_NETID, nPlayerIdx, 0);
			if (nNetID < 0)
				break;

			char name[NAME_LENGHT] = {0};
			m_pCoreServerShell->GetGameDataInfo(SGDI_CHARACTER_NAME, 0, pSync->m_dwParam, name);

			TONG_POWER_SYNC	tSync;
			tSync.ProtocolType = s2c_extendtong;
			tSync.m_btMsgId = enumTONG_SYNC_ID_SET_POWER;
			memcpy(tSync.m_btPower, pSync->m_btPower, sizeof(pSync->m_btPower));
			strcpy(tSync.m_szTargetName, pSync->m_TargetName);
			tSync.m_wLength = sizeof(tSync) - 1;

			if (m_pServer)
				m_pServer->PackDataToClient(nNetID, &tSync, tSync.m_wLength + 1);
		}
		break;
		case enumS2C_TONG_GET_POWER:
		{
			STONG_POWER_SYNC* pSync = (STONG_POWER_SYNC*)pChar;
			if (!pSync)
				break;
			if (pSync->m_dwParam <= 0 || pSync->m_dwParam >= MAX_PLAYER)
				break;

			int nNetID = m_pCoreServerShell->GetGameData(SGDI_CHARACTER_NETID, pSync->m_dwParam, 0);
			if (nNetID < 0)
				break;

			TONG_POWER_SYNC	tSync;
			tSync.ProtocolType = s2c_extendtong;
			tSync.m_btMsgId = enumTONG_SYNC_ID_GET_POWER;
			memcpy(tSync.m_btPower, pSync->m_btPower, sizeof(pSync->m_btPower));
			strcpy(tSync.m_szTargetName, pSync->m_TargetName);
			tSync.m_wLength = sizeof(tSync) - 1;

			if (m_pServer)
				m_pServer->PackDataToClient(nNetID, &tSync, tSync.m_wLength + 1);

		}
		break;
		default:
			break;
		}
	}
	else if (pHeader->ProtocolFamily == pf_friend)
	{//好友
		if (pHeader->ProtocolID == friend_s2c_syncblacklist)
		{
			FRIEND_SYNCBLACKLIST* pSync = (FRIEND_SYNCBLACKLIST*)pChar;
			if (pSync)
			{
				int nPlayerIdx = m_pGameStatus[FindSame(pSync->m_nParam)].nPlayerIndex;

				if (nPlayerIdx > 0 && nPlayerIdx < MAX_PLAYER)
				{
					std::list<std::string> blackList;
					for (int i = 0; i < defTONG_ONE_PAGE_MAX_NUM; i++)
					{
						if (pSync->m_szBlackList[i] && pSync->m_szBlackList[i][0])
						{
							blackList.push_back(pSync->m_szBlackList[i]);
						}
					}
					if(!blackList.empty())
						m_pCoreServerShell->SyncBlackList(nPlayerIdx, blackList);
				}
			}
		}
	}
}
////城市 和 世界 ，队伍 门派
void KSwordOnLineSever::ChatGroupMan(const void *pData, size_t dataLength)
{
	//_ASSERT(pData && dataLength);
	if  (NULL==pData || !pData || dataLength<=0) return;
	
	CHAT_GROUPMAN*	pCgc = (CHAT_GROUPMAN *)pData;
	_ASSERT(pCgc->wSize + 1 == dataLength);
	_ASSERT(sizeof(tagExtendProtoHeader) <= sizeof(CHAT_GROUPMAN));

	void* pExPckg = pCgc + 1;
	CHAT_CHANNELCHAT_SYNC* pCccSync = (CHAT_CHANNELCHAT_SYNC*)pExPckg;
	BYTE hasIdentify = pCgc->byHasIdentify;
	WORD playercount = pCgc->wPlayerCount;
	void* pPlayersData = (tagPlusSrcInfo*)((BYTE*)pExPckg + pCgc->wChatLength);
	size_t pckgsize = sizeof(tagExtendProtoHeader) + pCgc->wChatLength;
	tagExtendProtoHeader* pExHdr = (tagExtendProtoHeader*)pExPckg - 1;
	pExHdr->ProtocolType = s2c_extendchat;
	pExHdr->wLength = pckgsize - 1;

	if  (NULL==pPlayersData ||!pPlayersData) return;

	//printf("--chat_groupman聊天A:%d  %d--\n",hasIdentify,playercount);

	//printf("DEBUG, chatgroupman, sender: %s\n", pCccSync->someone);
	if (hasIdentify)
	{
		tagPlusSrcInfo* pPlayers = (tagPlusSrcInfo*)pPlayersData;
		for (int i = 0; i < playercount; i++)
		{
			//TODO: 要加检验NameID和lnID的一致性
			if (CheckPlayerID(pPlayers[i].lnID, pPlayers[i].nameid))
				if(!m_pCoreServerShell->CheckBlackList(GetPlayerID(pPlayers[i].lnID), pCccSync->someone))
					m_pServer->PackDataToClient(pPlayers[i].lnID, pExHdr, pckgsize);
		}
	}
	else
	{
		WORD* pPlayers = (WORD*)pPlayersData;
		for (int i = 0; i < playercount; i++)
		{
			//TODO: 不需要加检验NameID和lnID的一致性
			//if (pPlayers[i] >= 0)
			if (!m_pCoreServerShell->CheckBlackList(GetPlayerID((unsigned long)pPlayers[i]), pCccSync->someone))
				m_pServer->PackDataToClient((unsigned long)pPlayers[i], pExHdr, pckgsize);
		}
		/*for (int lnID = 0; lnID < m_nMaxGameStaus; lnID++)
		{
			int nIndex = m_pGameStatus[lnID].nPlayerIndex;

			if (nIndex <= 0 || nIndex>=MAX_PLAYER)
				continue; 

			if  (m_pServer)
			    m_pServer->PackDataToClient(m_pGameStatus[lnID].nNetidx, pExHdr, pckgsize);
		}*/
	}
}
//从S3获取信息发往客户端
void KSwordOnLineSever::ChatSpecMan(const void *pData, size_t dataLength)
{
	//_ASSERT(pData && dataLength);
	if  (NULL==pData || dataLength<=0) return;

	CHAT_SPECMAN*	pCsm = (CHAT_SPECMAN *)pData;
	_ASSERT(pCsm->wSize + 1 == dataLength);
//检查名字ID
	if (!CheckPlayerID(pCsm->lnID, pCsm->nameid))
		return;

	_ASSERT(sizeof(tagExtendProtoHeader) <= sizeof(CHAT_SPECMAN));
	void* pExPckg = pCsm + 1;

	// check black list
	CHAT_SOMEONECHAT_SYNC* pCscSync = (CHAT_SOMEONECHAT_SYNC*)pExPckg;
	if (m_pCoreServerShell->CheckBlackList(GetPlayerID(pCsm->lnID), pCscSync->someone))
		return;

	//printf("DEBUG, ChatSpectMan Sender: %s\n", pCscSync->someone);

	unsigned long lnID = pCsm->lnID;
	size_t pckgsize = sizeof(tagExtendProtoHeader) + pCsm->wChatLength;

	tagExtendProtoHeader* pExHeader = (tagExtendProtoHeader*)(pCsm + 1) - 1;
	pExHeader->ProtocolType = s2c_extendchat;
	pExHeader->wLength      = pckgsize - 1;

	m_pServer->PackDataToClient(lnID, pExHeader, pckgsize);
//	printf("<<<<<<<私聊信息发往客户端(client)成功>>>>>> OK..!\n");
}

//接收GODDES发回的角色数据包
void KSwordOnLineSever::DatabaseMessageProcess(const char* pData, size_t dataLength)
{
	//_ASSERT(pData&&dataLength);
	if  (NULL==pData ||!pData || dataLength<=0) return;

#ifndef _STANDALONE
	BYTE cProtocol = CPackager::Peek( pData );
#else
	BYTE cProtocol = *(BYTE *)pData;
#endif	
	try{

	// 大包处理（用于排名的数据）
	if ( cProtocol < s2c_micfghjkdtropackbegin )
	{
		DatabaseLargePackProcess(pData, dataLength);
		return;
	}

	switch (cProtocol)
	{
    case s2c_rolecheckname_result:
		{
			TCheckNameData *	pnPD = (TCheckNameData *)pData;

			if (pnPD->ulIdentity <= 0 || pnPD->ulIdentity >= MAX_PLAYER)
				break;

            if  (pnPD->nDataLen==1 || pnPD->nDataLen==-1)
			{
				m_pCoreServerShell->SetChangeRoleStatus(pnPD->ulIdentity,pnPD->nDataLen,pnPD->nRoleName);
				printf("[检测角色名成功]:账号:%s 角色:%s,索引[%d] 结果:%d 完成!\n", pnPD->nAccName, pnPD->nRoleName, pnPD->ulIdentity, pnPD->nDataLen);
			}
		}
		break;
	case s2c_roleserver_saverole_result:
		{
			TProcessData*	pPD = (TProcessData *)pData;

			if  (pPD->nDataLen!=791030)
				break;

			int nIndex = pPD->ulIdentity;

			if (nIndex <= 0 || nIndex >= MAX_PLAYER)
				break;
			printf("[Archive successfully]: Player character archive index [%d] completed!\n", nIndex);
			if (m_pCoreServerShell->IsCharacterLiXian(nIndex) != 2)
			{//正常退出的 不是离线的

			  if (m_pCoreServerShell->IsPlayerExchangingServer(nIndex) || m_pCoreServerShell->GetSaveStatus(nIndex)==SAVE_COUNT)
			  {
				  m_pCoreServerShell->SetSaveStatus(nIndex,SAVE_IDLE);
				  printf("---[Transferring to gs]: Index [%d] completed!----\n", nIndex);
		        break;
			  }

			  if (m_pCoreServerShell->GetSaveStatus(nIndex)==SAVE_DONE)
			  {//相同gs才设置掉线 用于网关退出网
			    m_pCoreServerShell->SetSaveStatus(nIndex,SAVE_REQUEST); //设置退出网关
			    m_pCoreServerShell->ClientDisconnect(nIndex);                           //设置离开游戏状态--断开客户端链接
				printf("[Close client]: Gateway exit successfully Index [%d] Completed!\n", nIndex);
			  }
			  else
			  {//如果是交易的 设置存档不掉线
                m_pCoreServerShell->SetSaveStatus(nIndex,SAVE_IDLE);
			  }
			}

		}
		break;
	default:
		printf("Protocol:(%d) -- database error \n", cProtocol);
		break;
	}

	}
	catch (...)
	{
		TRACE("Core DatabaseMessageProcess Error\n");
	}
}
//排名数据
void KSwordOnLineSever::DatabaseLargePackProcess(const char* pData, size_t dataLength)
{
	//_ASSERT( pData && dataLength );
	if  (NULL==pData ||!pData || dataLength<=0) return;

#ifndef _STANDALONE
	CBuffer *pBuffer = m_theRecv.PackUp( pData, dataLength );
#else
	char *pBuffer = m_theRecv.PackUp( pData, dataLength );
#endif

	if ( pBuffer )
	{
		try{
#ifndef _STANDALONE
	BYTE cProtocol = CPackager::Peek( pBuffer->GetBuffer() );
#else
	BYTE cProtocol = *(BYTE *)pBuffer;
#endif	
//		BYTE cProtocol = CPackager::Peek( pBuffer->GetBuffer() );

		switch ( cProtocol )
		{
		case s2c_gamestatistic:	//获取排名数据
			{
#ifndef _STANDALONE
				TProcessData*	pRD	= (TProcessData *)pBuffer->GetBuffer();
#else
				TProcessData*	pRD	= (TProcessData *)pBuffer;
#endif
				//_ASSERT( pRD->nDataLen == sizeof(TGAME_STAT_DATA)); //排名状态结构
				if  (pRD->nDataLen!=sizeof(TGAME_STAT_DATA))
					break;

				if (m_pCoreServerShell)
				{//设置排名
					m_pCoreServerShell->SetLadder((void *)pRD->pDataBuffer, pRD->nDataLen);
				}
			}
			break;

		default:
			break;			
		}

	}
	catch (...)
	{
		TRACE("Core DatabaseLargePackProcess Error\n");
	}

#ifndef _STANDALONE
		try
		{
			if( pBuffer != NULL )
			{
				pBuffer->Release();
				pBuffer = NULL; 
			}
		}
		catch(...) 
		{
			//TRACE("SAFE_RELEASE error\n");
		}
#endif
	}
}
//接收S3服务器传过来的数据
///////////////////////////////////////////////////////////////////////////////
void KSwordOnLineSever::TransferMessageProcess(const char* pChar, size_t nSize)
{
//#ifndef _STANDALONE
	//_ASSERT(pChar && nSize && nSize < 1024);
	if  (NULL==pChar ||!pChar || nSize<=0 || nSize>=1024) return;

	try
	{

	EXTEND_HEADER	*pEH = (EXTEND_HEADER *)pChar;

	if (nSize < sizeof(EXTEND_HEADER))
		return;

	// 是否寻路包？

	if (pEH->ProtocolID == relay_c2c_remote && pEH->ProtocolFamily == pf_relay)
	{
		RELAY_REMOTE* pRemote = (RELAY_REMOTE*)pEH;
		m_pCoreServerShell->ExecuteScript(pRemote);
		return;
	}

	if (pEH->ProtocolID == relay_c2c_askwaydata && pEH->ProtocolFamily == pf_relay)
	{//在目标gs中执行 装换协议转存数据后发往-S3  增加聊天频道 也会用到
		//printf("---收到s3发来的跨服信息relay_c2c_askwaydata --\n");
		TransferAskWayMessageProcess(pChar, nSize);
		return;
	}

	RELAY_DATA	*pRD = (RELAY_DATA *)pEH;
	if (nSize <= sizeof(RELAY_DATA) || nSize != pRD->routeDateLength + sizeof(RELAY_DATA))
		return;

	// 是否寻路失败包
	if (pEH->ProtocolID == relay_s2c_loseway && pEH->ProtocolFamily == pf_relay)
	{
		//printf("---收到s3发来的跨服信息relay_s2c_loseway --\n");
		TransferLoseWayMessageProcess(pChar + sizeof(RELAY_DATA), nSize - sizeof(RELAY_DATA));
		return;
	}

	if (pEH->ProtocolID != relay_c2c_data && pEH->ProtocolFamily != pf_relay)
		return;

	KTransferUnit* pUnit = NULL;
	IP2CONNECTUNIT::iterator it = m_mapIp2TransferUnit.find(pRD->nFromRelayID);

	if (it != m_mapIp2TransferUnit.end())
	{//如果存在了
		pUnit = (*it).second;
		//_ASSERT(pUnit);
	}
	else
	{
		pUnit = new KTransferUnit(pRD->nFromIP, pRD->nFromRelayID);//跨服的 ip 信息
		m_mapIp2TransferUnit.insert(IP2CONNECTUNIT::value_type(pRD->nFromRelayID, pUnit));
	}
	//printf("---收到s3发来的跨服pRD->nFromRelayID:%d  pRD->nFromIP:%d 信息A --\n",pRD->nFromRelayID,pRD->nFromIP);
#ifndef _STANDALONE
	BYTE cProtocol = CPackager::Peek(pChar, sizeof(RELAY_DATA));
#else
	BYTE cProtocol = *(BYTE *)(pChar + sizeof(RELAY_DATA));
#endif	

	if ( cProtocol < c2c_micropackbegin )
	{
		//printf("---收到s3发来的跨服信息A --\n");
		TransferLargePackProcess(pChar + sizeof(RELAY_DATA), pRD->routeDateLength, pUnit);
	}
	else if ( cProtocol > c2c_micropackbegin )
	{
		//printf("---收到s3发来的跨服信息B --\n");
		TransferSmallPackProcess(pChar + sizeof(RELAY_DATA), pRD->routeDateLength, pUnit);
	}
	else
	{
		//printf("---收到s3发来的跨服信息C --\n");
		_ASSERT( FALSE && "Error!" );
	}

	}
	catch (...)
	{
		TRACE("Core TransferMessageProcess Error\n");
	}
//#endif
}
//大包处理
void KSwordOnLineSever::TransferLargePackProcess(const void *pData, size_t dataLength, KTransferUnit *pUnit)
{
	//_ASSERT(pData && dataLength);
	if  (NULL==pData ||!pData || dataLength<=0) return;

	try{
#ifndef _STANDALONE
	CBuffer *pBuffer = pUnit->m_RecvData.PackUp( pData, dataLength );
#else
	char *pBuffer = pUnit->m_RecvData.PackUp( pData, dataLength );
#endif
	char	szChar[1024];

	if ( pBuffer )
	{
#ifndef _STANDALONE
		BYTE cProtocol = CPackager::Peek( pBuffer->GetBuffer() );
#else
		BYTE cProtocol = *(BYTE *)pBuffer;
#endif		
		
		switch ( cProtocol )
		{
		case c2c_transferroleinfo:
			{//改变角色信息 转服信息 //目标gs服务器执行的了
#ifndef _STANDALONE
				tagGuidableInfo *pGI = (tagGuidableInfo *)pBuffer->GetBuffer();
#else
				tagGuidableInfo *pGI = (tagGuidableInfo *)pBuffer;
#endif

				GUID guid;
				memcpy( &guid, &( pGI->guid ), sizeof( GUID ) );
				
				if ( pGI->datalength > 0 )
				{
					const TRoleData *pRoleData = ( const TRoleData * )( pGI->szData );
					/*DWORD	dwCRC = 0;
					dwCRC = CRC32(dwCRC, pRoleData, pRoleData->dwDataLen - 4);
					DWORD	dwCheck = *(DWORD *)(pGI->szData + pRoleData->dwDataLen - 4);
					BOOL	bCRC = (dwCheck == dwCRC);
					*/
					BOOL bCRC = TRUE;
					int nIdx = 0;
					if (bCRC)
					{//设定玩家数据
						nIdx = m_pCoreServerShell->AddCharacter(pGI->nExtPoint, pGI->nChangePoint, (void *)pRoleData,\
							&guid,pGI->nLeftTime,pGI->nVipType,pGI->nGameTime,1,false,0,0,0); //更新扩展点等。。。
					}

					else
					{
						FILE *fLog = fopen("crc_transfer_error", "a+b");
						if(fLog) 
						{
							char buffer[255];
							sprintf(buffer, "----\r\n%s\r\b%s\r\n", pRoleData->BaseInfo.szName, pRoleData->BaseInfo.caccname);
							fwrite(buffer, 1, strlen(buffer), fLog);
							fwrite(pRoleData, 1, pRoleData->dwDataLen, fLog);
							fclose(fLog);
						}
						printf("----------转服:人物数据有误----------\n");
					}

					RELAY_DATA*	pRD = (RELAY_DATA *)szChar;
					pRD->ProtocolFamily = pf_relay;
					pRD->ProtocolID = relay_c2c_data;
					pRD->nFromIP = 0;
					pRD->nFromRelayID = 0;
					pRD->nToIP      = pUnit->GetIp();     //源GS服务器
					pRD->nToRelayID = pUnit->GetRelayID();//源GS链接编号
					pRD->routeDateLength = sizeof(tagPermitPlayerExchange);

					tagPermitPlayerExchange *pPPEO = (tagPermitPlayerExchange *)(szChar + sizeof(RELAY_DATA));
					pPPEO->cProtocol = c2c_permitplayerexchangein;
					memcpy(&pPPEO->guid, &guid, sizeof(GUID));
					//目标服务器 ip 和 端口
					pPPEO->dwIp  = m_dwInternetIp; // TransDwIp;    //
					pPPEO->wPort = m_nServerPort;//TransPort;//m_nServerPort; //服务器端口

					if (nIdx)
					{
						pPPEO->bPermit = true;
						tagRegisterAccount	ra;
						ra.cProtocol = c2s_registeraccount;
						//strcpy((char *)ra.szAccountName, pRoleData->BaseInfo.caccname);
						snprintf(ra.szAccountName, sizeof(ra.szAccountName),"%s",pRoleData->BaseInfo.caccname);
						m_pGatewayClient->SendPackToServer(&ra, sizeof(tagRegisterAccount));

						printf("----------AddCharacter 同意[%s]源gs(%d)跨服端口:%d----------\n",ra.szAccountName,pPPEO->bPermit,m_nServerPort);
						/*m_pCoreServerShell->SetSaveStatus(nIdx,SAVE_DOING);
						m_pCoreServerShell->SavePlayerData(nIdx);*/
					}
					else
					{
						pPPEO->bPermit = false;
					}
					m_pTransferClient->SendPackToServer(pRD, sizeof(RELAY_DATA) + sizeof(tagPermitPlayerExchange));

					/*if (nIdx)
					pPPEO->bPermit = true;
					else
					pPPEO->bPermit = false;

					if (m_pTransferClient)
					m_pTransferClient->SendPackToServer(pRD, sizeof(RELAY_DATA) + sizeof(tagPermitPlayerExchange));

					if (nIdx)
					{
					tagRegisterAccount	ra;
					ra.cProtocol = c2s_registeraccount;
					strcpy((char *)ra.szAccountName, pRoleData->BaseInfo.caccname);
					if (m_pGatewayClient)
					m_pGatewayClient->SendPackToServer(&ra, sizeof(tagRegisterAccount));
					}*/
					
				}
				else
				{
//					cout << "Don't find any valid data!" << endl;
		        } 
			}
			break;
		default:
			break;
		}

#ifndef _STANDALONE
		try
		{
			if( pBuffer != NULL )
			{
				pBuffer->Release();
				pBuffer = NULL; 
			}
		}
		catch(...) 
		{
			//TRACE("SAFE_RELEASE error\n");
		}
#endif
    }	

	}
	catch (...)
	{
		TRACE("Core TransferLargePackProcess Error\n");
	}

}
//小包处理
void KSwordOnLineSever::TransferSmallPackProcess(const void *pData, size_t dataLength, KTransferUnit* pUnit)
{

	//_ASSERT(/*pUnit && */pData && dataLength);
	//if (!pUnit || !pData || !dataLength)
	//	return;
	if  (NULL==pData ||!pData || dataLength<=0) return;

	try{

#ifndef _STANDALONE
	BYTE cProtocol = CPackager::Peek( pData );
#else
	BYTE cProtocol = *(BYTE *)pData;
#endif	
	
	switch ( cProtocol )
	{
	case s2s_broadcast:
		{
			//printf("----收到 s2s_broadcast 信息-----\n");
			BYTE* pS = (BYTE *)pData;
			m_pCoreServerShell->ProcessBroadcastMessage((const char *)(pS + 1), dataLength - 1);
		}
		break;
	case s2s_execute:
		{//执行脚本线程
			BYTE* pS = (BYTE *)pData;
			m_pCoreServerShell->ProcessExecuteMessage((const char *)(pS + 1), dataLength - 1);
		}
		break;
	case c2c_permitplayerexchangein:
		{//这个是在源目标机器 执行
			tagPermitPlayerExchange* pPermit = (tagPermitPlayerExchange *)pData;

			int nIndex, lnID;
			m_pCoreServerShell->GetPlayerIndexByGuid(&pPermit->guid, &nIndex, &lnID);//通过guid 获取网络号
			
			if (nIndex == 0 || lnID == -1)
				break;

			int i ;
			i= FindSame(lnID);

			if  (i<=0 || i>m_nMaxGameStaus) break;

			if (m_pGameStatus[i].nExchangeStatus != enumExchangeWaitForGameSvrRespone)
				break;

			tagNotifyPlayerExchange	npe;
			if (pPermit->bPermit)
			{//源服务器 同意转服
//				SavePlayerData(nIndex);
				m_pCoreServerShell->RemovePlayerForExchange(nIndex);   //删除节点
				m_pCoreServerShell->SetSaveStatus(nIndex,SAVE_COUNT);
				
				//通知GAME连接服务器了
				npe.cProtocol = s2c_notifyplayerexchange;
				memcpy(&npe.guid, &pPermit->guid, sizeof(GUID));
				npe.nIPAddr   = pPermit->dwIp;
				npe.nPort     = pPermit->wPort;	 //同意登录的端口
				m_pServer->SendData(lnID, &npe, sizeof(tagNotifyPlayerExchange));

				//------------------通知网关 这个帐号退出游戏了
				tagLeaveGame	sLeaveGame;     //离开游戏时  扣除游戏点卡
				sLeaveGame.cProtocol = c2s_leavegame;
				sLeaveGame.cCmdType  = HOLDACC_LEAVEGAME;
				char	szName[ACC_LENGTH],szRoleName[NAME_LENGHT];
				//m_pCoreServerShell->GetGameData(SGDI_CHARACTER_ACCOUNT, (unsigned int)szName, nIndex);
				m_pCoreServerShell->GetGameDataInfo(SGDI_CHARACTER_ACCOUNT, 0, nIndex,szName);
				sLeaveGame.nExtPoint    = m_pCoreServerShell->GetExpData(nIndex,0);
				sLeaveGame.nGameExtTime = m_pCoreServerShell->GetExpData(nIndex,1);
				//m_pCoreServerShell->GetGameData(SGDI_CHARACTER_NAME, (unsigned int)szRoleName, nIndex);

				snprintf((char *)sLeaveGame.nClientIp, sizeof(sLeaveGame.nClientIp), "%s", (char *)szRoleName);
				snprintf((char *)sLeaveGame.szAccountName, sizeof(sLeaveGame.szAccountName), "%s", (char *)szName);
				if (m_pGatewayClient)
					m_pGatewayClient->SendPackToServer(&sLeaveGame, sizeof(tagLeaveGame));
				// 通知 S3 这个帐号退出游戏了----------------
				tagLeaveGame2 lg2;
				lg2.ProtocolFamily = pf_normal;
				lg2.ProtocolID     = c2s_leavegame;
				lg2.cCmdType       = HOLDACC_LEAVEGAME;
				snprintf((char *)lg2.szAccountName, sizeof(lg2.szAccountName), "%s", (char *)szName);
				lg2.nSelServer = m_SerVerNo;
				if (m_pTransferClient)	 //发往S3
					m_pTransferClient->SendPackToServer(&lg2, sizeof(tagLeaveGame2));
				if (m_pChatClient)
					m_pChatClient->SendPackToServer(&lg2, sizeof(tagLeaveGame2));
				if (m_pTongClient)
					m_pTongClient->SendPackToServer(&lg2, sizeof(tagLeaveGame2));
				//-------------------------------------------

				SetNetStatus(m_pGameStatus[i].nNetidx,enumNetUnconnect);
				m_pCoreServerShell->IsShutdownClient(m_pGameStatus[i].nPlayerIndex);
				m_pGameStatus[i].nExchangeStatus = enumExchangeCleaning;
				m_pGameStatus[i].nPlayerIndex = 0;

				tagRoleEnterGame reg;
				reg.ProtocolType = c2s_roleserver_lock;
				reg.bLock = false; //删除
				// m_SerVerNo  m_GsNumBer
				reg.Lixian = false;           //是否离线的
				reg.nRegServerIndex = m_SerVerNo;  //属于那个大区
				reg.nGsNumber = m_GsNumBer;        //GS索引号
				//strcpy((char *)reg.Name,(char *)szRoleName);
				//m_pCoreServerShell->GetGameData(SGDI_CHARACTER_NAME, (unsigned int)reg.Name, nIndex);
				m_pCoreServerShell->GetGameDataInfo(SGDI_CHARACTER_NAME,0, nIndex,reg.Name);
				if (m_pGatewayClient)
					m_pGatewayClient->SendPackToServer((const void *)&reg, sizeof(tagRoleEnterGame));
				//m_pCoreServerShell->RemovePlayerForExchange(nIndex);   //删除节点
				printf("----收到通知 同意转服啦!发往客户端(%d)-----\n",pPermit->wPort);

			}
			else
			{//提示 此路不通。。
				npe.cProtocol = s2c_notifyplayerexchange;
				memset(&npe.guid, 0, sizeof(GUID));
				// if false, ip to 0
				npe.nIPAddr = 0;
				npe.nPort   = 0;
				m_pServer->SendData(lnID, &npe, sizeof(tagNotifyPlayerExchange));
				
				//恢复转服状态
				m_pCoreServerShell->RecoverPlayerExchange(nIndex);

				tagRoleEnterGame reg;
				reg.ProtocolType = c2s_roleserver_lock;
				reg.bLock = true;
				// m_SerVerNo  m_GsNumBer
				reg.Lixian = false;           //是否离线的
				reg.nRegServerIndex = m_SerVerNo;  //属于那个大区
				reg.nGsNumber          = m_GsNumBer;        //GS索引号

				//m_pCoreServerShell->GetGameData(SGDI_CHARACTER_NAME, (unsigned int)reg.Name, nIndex);
				m_pCoreServerShell->GetGameDataInfo(SGDI_CHARACTER_NAME,0, nIndex,reg.Name);
				if (m_pDatabaseClient)
					m_pDatabaseClient->SendPackToServer((const void *)&reg, sizeof(tagRoleEnterGame));
				if (m_pGatewayClient)
					m_pGatewayClient->SendPackToServer((const void *)&reg, sizeof(tagRoleEnterGame));

				m_pGameStatus[i].nGameStatus = enumPlayerPlaying;
				m_pGameStatus[i].nExchangeStatus = enumExchangeBegin;
			}
		}
		break;
	case c2c_notifyexchange:
		{//收到通知 获取数据  源gs执行
			tagSearchWay *pSW = (tagSearchWay *)pData;

			if (!pSW || pSW->lnID < 0 || pSW->lnID >m_nMaxGameStaus)
			{
				printf("break 1 (%d)\n",pSW->lnID);
				break;
			}

			if (m_pGameStatus[pSW->lnID].nExchangeStatus != enumExchangeSearchingWay)
			{
				printf("break 2 (%d)\n",pSW->lnID);
				break;
			}
			
			int nIndex = m_pGameStatus[pSW->lnID].nPlayerIndex;
			
			if (nIndex <= 0 || nIndex >= MAX_PLAYER || nIndex != pSW->nIndex)
			{
				printf("break 3\n");
				break;
			}

			if (pSW->dwPlayerID != m_pCoreServerShell->GetGameData(SGDI_CHARACTER_ID, nIndex, 0))
			{
				printf("break 4\n");
				break;
			}

			char	szData[1024]={0};

			RELAY_DATA *pRD = (RELAY_DATA *)szData;
			
			pRD->ProtocolFamily = pf_relay;
			pRD->ProtocolID = relay_c2c_data;
			pRD->nToIP      = pUnit->GetIp();     //目标gs
			pRD->nToRelayID = pUnit->GetRelayID();//目标gs连接编号
			pRD->nFromIP = 0;
			pRD->nFromRelayID = 0;           //

			tagGuidableInfo gi;
			//跨服存档 生成新的数据包
			TRoleData* pData = (TRoleData *)m_pCoreServerShell->PreparePlayerForExchange(nIndex);  //获取人物的数据包

			if (!pData)
			{
				printf("break 获取人物数据失败!\n");
				break;
			}
			//转区设置
			//pData->BaseInfo.isWaiGua = 2;
			printf("----转gs获取人物数据成功:%s ItemsCount:%d !--->S3 \n",pData->BaseInfo.szName,pData->nItemCount);
			gi.cProtocol    = c2c_transferroleinfo;  
			m_pCoreServerShell->GetGuid(nIndex, &gi.guid);
			//从游戏中获取源数据 用于在 目标服务器进入游戏
			gi.nExtPoint    = m_pCoreServerShell->GetGameData(SGDI_CHARACTER_EXTPOINT, nIndex, 0);  //获取可以赠送的扩展点
			gi.nChangePoint = m_pCoreServerShell->GetGameData(SGDI_CHARACTER_EXTPOINTCHANGED, nIndex, 0);
            gi.nLeftTime    = m_pCoreServerShell->GetGameData(SGDI_CHARACTER_LEFTTIME, nIndex, 0);
            gi.nVipType     = m_pCoreServerShell->GetGameData(SGDI_CHARACTER_VIPTYPE, nIndex, 0);
			gi.nGameTime    = m_pCoreServerShell->GetGameData(SGDI_CHARACTER_EXTTIMECHANGED, nIndex, 0);
			gi.nTempEnTerMapIndx = 0;
			gi.nTempKeyId = 0;
			//pData->dwDataLen += 4;
			gi.datalength   = pData->dwDataLen;
			//DWORD	dwCRC = 0;
			//dwCRC = CRC32(dwCRC, pData, pData->dwDataLen - 4);  //校验码
            //替换内存中的游戏数据
			m_theSend.AddData(c2c_transferroleinfo,(const char*)&gi,sizeof(tagGuidableInfo));
			//m_theSend.AddData(c2c_transferroleinfo,(const char*)pData,gi.datalength - 4);
			m_theSend.AddData(c2c_transferroleinfo,(const char*)pData,gi.datalength);
			//m_theSend.AddData(c2c_transferroleinfo,(const char*)&dwCRC,4);
			//printf("----收到通知 开始转服:获取源服务器数据成功!-----\n");

#ifndef _STANDALONE
			CBuffer* pBuffer = m_theSend.GetHeadPack(c2c_transferroleinfo);//512
#else
			size_t actLen = 0;
			char* pBuffer = m_theSend.GetHeadPack(c2c_transferroleinfo,actLen);//512
#endif
			while(pBuffer)
			{

#ifndef _STANDALONE
				size_t nBodyLen = pBuffer->GetUsed();
				const void* pBody = pBuffer->GetBuffer();
#else
				size_t nBodyLen = actLen;
				const void* pBody = pBuffer;
#endif
				// both spellings of the body length are the packet built above, szData is
				// 1024 bytes with RELAY_DATA written at its front, and routeDateLength is a
				// WORD - the _ASSERT this replaces is compiled out of every configuration the
				// shipped build is swept with, so the room is checked where the copy reads it
				if (nBodyLen > sizeof(szData) - sizeof(RELAY_DATA))
					nBodyLen = sizeof(szData) - sizeof(RELAY_DATA);
				pRD->routeDateLength = (WORD)nBodyLen;

				_ASSERT(pRD->routeDateLength + sizeof(RELAY_DATA) < 1024);

				memcpy(szData + sizeof(RELAY_DATA), pBody, nBodyLen);
                //发往中转服务器 
				m_pTransferClient->SendPackToServer(szData, pRD->routeDateLength + sizeof(RELAY_DATA));
#ifndef _STANDALONE
				try
				{
					if (pBuffer)
					{
						pBuffer->Release();
						pBuffer = NULL;
					}
				}
				catch(...) 
				{
					//TRACE("SAFE_RELEASE error\n");
				}
#endif

#ifndef _STANDALONE
				pBuffer = m_theSend.GetNextPack(c2c_transferroleinfo);
#else
				pBuffer = m_theSend.GetNextPack(c2c_transferroleinfo, actLen);
#endif
			}
			m_theSend.DelData(c2c_transferroleinfo);   //删除数据？
#ifndef _STANDALONE
			try
			{
				if (pBuffer)
				{
					pBuffer->Release();
					pBuffer = NULL;
				}
			}
			catch(...) 
			{
				//TRACE("SAFE_RELEASE error\n");
			}
#endif
			m_pGameStatus[pSW->lnID].nExchangeStatus = enumExchangeWaitForGameSvrRespone;
		}
		break;
	}

    }
	catch (...)
	{
		TRACE("Core TransferSmallPackProcess Error\n");
	}
}

////大包
//	m_thePackager.AddData(s2c_syncogjfkdnvhf_roleinfo_cipher, (const char *)&ginfo, sizeof(tagGuidableInfo));
//	m_thePackager.AddData(s2c_syncogjfkdnvhf_roleinfo_cipher, (const char *)pData, datalength);
//
//	/*
//	 * Begin to split this buffer	开始分割大包
//	 */
//	CBuffer *pBuffer = m_thePackager.GetHeadPack(s2c_syncogjfkdnvhf_roleinfo_cipher);
//
//	while (pBuffer)
//	{//发往GS 向哪个GS发包
//		m_pGameSvrServer->SendData(m_lnIdentityID, pBuffer->GetBuffer(),pBuffer->GetUsed());
//
//		/*
//		 * Your must relase this buffer
//		 */
//		SAFE_RELEASE(pBuffer);
//
//		/*
//		 * Get next package
//		 */
//		pBuffer = m_thePackager.GetNextPack(s2c_syncogjfkdnvhf_roleinfo_cipher);		
//	}
//	
//	SAFE_RELEASE(pBuffer);
//
//	m_thePackager.DelData(s2c_syncogjfkdnvhf_roleinfo_cipher);

void KSwordOnLineSever::PlayerFschatProcess(const unsigned long lnID, const char* pData, size_t dataLength)
{
  	//_ASSERT(pData && dataLength);
	if  (NULL==pData ||!pData || dataLength<=0) return;

#ifndef _STANDALONE
	BYTE cProtocol = CPackager::Peek( pData );
#else
	BYTE cProtocol = *(BYTE *)pData;
#endif	

 try
 { 
	//int nGameStatus = m_pGameStatus[lnID].nGameStatus;  //游戏状态
	int nIndex = m_pGameStatus[lnID].nPlayerIndex;      //玩家索引链表号	nPlayeridx
	
	BYTE protocoltype = *(BYTE*)pData;

	if (protocoltype >= _c2s_begin_relay && protocoltype <= _c2s_end_relay)
	{
		IClient* pClient = NULL;
		switch (protocoltype)
		{
		case c2s_extend: pClient = m_pTransferClient;//S3 的中转服务器 
			break;
		case c2s_extendchat: pClient = m_pChatClient;
			break;
		case c2s_extendchat_new: pClient = m_pChatClient;
			break;
		case c2s_extendfriend: pClient = m_pTongClient;
			break;
		default:
			return;
		};

		if (pClient && nIndex && dataLength > sizeof(tagExtendProtoHeader))
		{		
			size_t exsize = dataLength - sizeof(tagExtendProtoHeader);
			const void* pExPckg = pData + sizeof(tagExtendProtoHeader);

			if (NULL==pExPckg||!pExPckg) return;

			if (protocoltype == c2s_extendchat)
			{
//				printf(">>>>>>>>>>发送频道消息,玩家序号(%d)<<<<<<<<<<...\n", nIndex);
				CHAT_CHANNELCHAT_CMD* pEh = (CHAT_CHANNELCHAT_CMD*)pExPckg;
				if (pEh->ProtocolType == chat_channelchat && pEh->channelid!=0)	//非GM频道要过滤和付钱
				{//付费聊天
					printf(">>>>>>>>>>nIndex:(%d)channelid:(%d)cost:(%d)<<<<<<<<<<...\n", nIndex,pEh->channelid,pEh->cost);
					if (!m_pCoreServerShell->PayForSpeech(nIndex, pEh->cost))
					{	
						printf(">>>>>>>>>> fail Chat Kenh nIndex:(%d)<<<<<<<<<<...\n", nIndex);
						return;
					}
					    printf(">>>>>>>>>> Chat Kenh Free, nIndex:(%d)<<<<<<<<<<...\n", nIndex);
				}

				CHAT_SOMEONECHAT_CMD* psEh = (CHAT_SOMEONECHAT_CMD*)pExPckg;
				if (psEh->ProtocolType == chat_someonechat)	//非GM频道要过滤和付钱
				{//私聊聊天
					if (m_pCoreServerShell->GetClientNetConnectIdx(nIndex)<0)
					{//检测对方是否在线,发送方
						return;
					}
				}
			}

			size_t pckgsize = exsize + sizeof(tagPlusSrcInfo);

			#ifdef WIN32
			void* pPckg2 = _alloca(pckgsize);//在栈上分配内存，用完马上释放。。。。
			#else
			void* pPckg2 = (new char[pckgsize]); //void
			#endif

			if  (NULL==pPckg2||!pPckg2) return;

			memcpy(pPckg2, pExPckg, exsize);
			tagPlusSrcInfo* pSrcInfo = (tagPlusSrcInfo*)((BYTE*)pPckg2 + exsize);
			
			if  (NULL==pSrcInfo ||!pSrcInfo)
			{
#ifdef WIN32

#else
				delete (char*)pPckg2;
				(char*)pPckg2=NULL;
#endif
				return;
			}
			
			pSrcInfo->nameid = m_pCoreServerShell->GetGameData(SGDI_CHARACTER_ID, nIndex, 0);//这个索引的名字ID 
			pSrcInfo->lnID   = m_pGameStatus[lnID].nNetidx;   //lnID; 每个GS都有一个唯一的这个网络ID
			pSrcInfo->nSelSerVer = m_SerVerNo;
	        //return;
			if (pClient)
			   pClient->SendPackToServer(pPckg2, pckgsize);  //给S3relay 发消息
            //printf(">>>>>>>>>>发送S3relay服务器消息(%d)(%d)，OK<<<<<<<<<<...\n", pSrcInfo->nameid,*(BYTE*)pData);

		#ifdef WIN32

		#else
			delete (char*)pPckg2;
			(char*)pPckg2=NULL;
        #endif

		}
		return;
	}
}
catch (...)
 { 
		TRACE("Core PlayerFschatProcess Error \n");
 } 

}
//-------------------------获取客户端发往服务器玩家相关信息--------------------------------------------------
void KSwordOnLineSever::PlayerFsMessageProcess(const unsigned long lnID, const char* pData, size_t dataLength)
{

	//_ASSERT(pData && dataLength);
	if  (NULL==pData||!pData || dataLength<=0) return;
#ifndef _STANDALONE
	BYTE cProtocol = CPackager::Peek(pData);
#else
	BYTE cProtocol = *(BYTE *)pData;
#endif	
 try
 { 
	int nGameStatus = m_pGameStatus[lnID].nGameStatus;       //游戏状态
	int nIndex      = m_pGameStatus[lnID].nPlayerIndex;      //玩家链表索引号
	
	BYTE protocoltype = *(BYTE*)pData;

	if (protocoltype >= _c2s_begin_relay && protocoltype <= _c2s_end_relay)
		return;

	//检查协议是否合法（大小）
	if (!m_pCoreServerShell->CheckProtocolSize(pData,dataLength))
	{
		m_pGameStatus[lnID].nErrorLoopCount++;
		if (m_pGameStatus[lnID].nErrorLoopCount>=m_errorCount)
		{//如果发生的错误大于某个数值,就踢掉连接
			m_pGameStatus[lnID].nErrorLoopCount=0;
			int nIndex = m_pGameStatus[lnID].nPlayerIndex;
			if (nIndex > 0 && nIndex<MAX_PLAYER)
				m_pCoreServerShell->IsShutdownClient(nIndex);
		}
		return;
	}

	//printf("player msg arrived...%d(%d)\n", nGameStatus,*(BYTE*)pData); //[wxb 2003-7-28]
	switch(nGameStatus)
	{
	case enumPlayerPlaying:  
		if (nIndex)
		{
			if (*(BYTE*)pData == c2s_ping)
			{
				ProcessPingReply(lnID, pData, dataLength);  //接收客户端发来的 ping的时间
			}
			else if (*(BYTE*)pData == c2s_extendtong)
			{//帮会相关

				ProcessPlayerTongMsg(nIndex, pData, dataLength);
//			    printf(">>>>>>>>>>发送S3relay服务器建帮消息(%d)(%d)请求类型(%d)，OK<<<<<<<<<<...\n", nIndex,*(BYTE*)pData,((STONG_PROTOCOL_HEAD*)pData)->m_btMsgId);
			}
			else 
			{//玩家的其他协议消息
				m_pCoreServerShell->ProcessClientMessage(nIndex, pData, dataLength);
				//printf(">>>客户端->服务器>>玩家序号(%d)协议序号(%d)请求类型(%d)，OK<<<<<<<<<<...\n", nIndex,*(BYTE*)pData-64,((STONG_PROTOCOL_HEAD*)pData)->m_btMsgId);
			}
		}
		break;
	case enumPlayerExchangingServer://开始跨服
		break;
	case enumPlayerSyncEnd:         //同步完毕 玩家增加到游戏中。
		if (ProcessSyncReplyProtocol(lnID, pData, dataLength))
		{//最后登录游戏成功。。。。。。。开始游戏吧
			m_pCoreServerShell->AddPlayerToWorld(nIndex);        //增加玩家到游戏世界
			m_pGameStatus[lnID].nGameStatus = enumPlayerPlaying; //游戏状态
			PingClient(lnID);                                    //同步完就开始发PING 客户端

			/*m_pCoreServerShell->SetSaveStatus(nIndex,SAVE_DOING);
			m_pCoreServerShell->SavePlayerData(nIndex);*/
			//通知bishop 进入游戏
			tagEnterGame eg;
			eg.cProtocol = c2s__entergamegkjyiruqine;
			//m_pCoreServerShell->GetGameData(SGDI_CHARACTER_ACCOUNT, (unsigned int)eg.szAccountName, nIndex);
			m_pCoreServerShell->GetGameDataInfo(SGDI_CHARACTER_ACCOUNT, 0, nIndex,(char *)eg.szAccountName);
			if (m_pGatewayClient)  //发往网关服务器
				m_pGatewayClient->SendPackToServer( ( const void * )&eg, sizeof( tagEnterGame ) );
			//发往中转服务器	S3
			tagEnterGame2 eg2;
			eg2.ProtocolFamily = pf_normal;
			eg2.ProtocolID = c2s__entergamegkjyiruqine;
			snprintf(( char * )eg2.szAccountName, sizeof(eg2.szAccountName), "%s", (char *)eg.szAccountName);
			eg2.dwNameID = m_pCoreServerShell->GetGameData(SGDI_CHARACTER_ID, nIndex, 0);
			//m_pCoreServerShell->GetGameData(SGDI_CHARACTER_NAME, (unsigned int)eg2.szCharacterName, nIndex);
			m_pCoreServerShell->GetGameDataInfo(SGDI_CHARACTER_NAME, 0, nIndex,eg2.szCharacterName);
			eg2.lnID = m_pGameStatus[lnID].nNetidx;//lnID; //网络号
			//printf("DEBUG, Send To S3, lnID = %d, NetID: %d\n", lnID, eg2.lnID);
			eg2.nSelSerVer = m_SerVerNo;

			if (m_pTransferClient) 
				m_pTransferClient->SendPackToServer( (const void *)&eg2, sizeof(tagEnterGame2) );
			if (m_pChatClient) 
				m_pChatClient->SendPackToServer( (const void *)&eg2, sizeof(tagEnterGame2) );
			if (m_pTongClient)
				m_pTongClient->SendPackToServer(&eg2, sizeof(tagEnterGame2));
			//printf("----通知S3(%s)登录游戏成功:%d---- \n",eg2.szAccountName,nIndex);
			//锁定角色数据库
			tagRoleEnterGame reg;
			reg.ProtocolType = c2s_roleserver_lock;
			reg.bLock = true;
			reg.Lixian = false;           //是否离线的
			reg.nRegServerIndex = m_SerVerNo;  //属于那个大区
			reg.nGsNumber = m_GsNumBer;        //GS索引号
			snprintf((char *)reg.Name, sizeof(reg.Name), "%s", (char *)eg2.szCharacterName);
			if (m_pDatabaseClient)
				m_pDatabaseClient->SendPackToServer((const void *)&reg, sizeof(tagRoleEnterGame));
			if (m_pGatewayClient)
				m_pGatewayClient->SendPackToServer((const void *)&reg, sizeof(tagRoleEnterGame));
			// 登录时获取帮会信息
			int	nPlayerIdx = m_pGameStatus[lnID].nPlayerIndex;

			if (nPlayerIdx > 0 && nPlayerIdx < MAX_PLAYER)
			{
				DWORD	dwTongNameID = m_pCoreServerShell->GetGameData(SGDI_TONG_GET_TONG_NAMEID, 0, nPlayerIdx);
				char	szName[NAME_LENGHT];
				szName[0] = 0;
				m_pCoreServerShell->GetGameDataInfo(SGDI_CHARACTER_NAME,0, nPlayerIdx,szName);

				int nSex = m_pCoreServerShell->GetGameDataInfo(SGDI_CHARACTER_SEX, 0, nPlayerIdx, 0);

				if (dwTongNameID > 0 && szName[0])
				{//有帮会的玩家 同步帮会信息到客户端  -->S3
					STONG_GET_LOGIN_DATA_COMMAND	sLogin;
					sLogin.ProtocolFamily	= pf_tong;
					sLogin.ProtocolID		= enumC2S_TONG_GET_LOGIN_DATA;
					sLogin.m_dwParam		= nPlayerIdx;
					sLogin.m_nParam			= nSex;
					sLogin.m_dwTongNameID	= dwTongNameID;
					snprintf(sLogin.m_szName, sizeof(sLogin.m_szName), "%s", szName);
					if (m_pTongClient)
						m_pTongClient->SendPackToServer((const void*)&sLogin, sizeof(sLogin));
				}
				else //没有加入帮会的
                    m_pCoreServerShell->GetGameData(SGDI_TONG_SEND_SELF_INFO, 0, nPlayerIdx);


//				CLIENT_SYNC_END	SyncEnd;
//				//BYTE	SyncEnd;
//				SyncEnd.ProtocolType = s2c_syncclientend;  //(BYTE)
//				SyncEnd.m_IsLogin    = TRUE;
//				SyncEnd.m_clientKey  = 9473290;
//
//#ifndef _STANDALONE
//				if (FAILED(m_pServer->PackDataToClient(m_pGameStatus[lnID].nNetidx,&SyncEnd, sizeof(CLIENT_SYNC_END)))) //(BYTE*)
//#else
//				if (!SUCCEEDED(m_pServer->PackDataToClient(m_pGameStatus[lnID].nNetidx,&SyncEnd, sizeof(CLIENT_SYNC_END))))//(BYTE*)
//#endif
//				{
//					break;
//				}

				printf("[Login] szAccountName:(%s),szName:(%s),Index:(%d) Succes!!!!!!---- \n", eg2.szAccountName,szName,nIndex);
			}
		}
		break;
	case enumPlayerBegin:  //玩家开始登陆
		{//获取玩家 nUseIdx 请求链表号
		    //printf("----玩家开始登录,网络号:%d---- \n", lnID);
			int nIndex = ProcessLoginProtocol(lnID, pData, dataLength);
			//printf("process login %d...\n", nIndex);
			if (nIndex)
			{
				if (SendGameDataToClient(lnID, nIndex))  //发包给客户端
				{
					//printf("process login %d ok...\n", nIndex);
					printf("----process login :%d, ok :%d---- \n", lnID,nIndex);
					m_pGameStatus[lnID].nGameStatus = enumPlayerSyncEnd;
					m_pGameStatus[lnID].nPlayerIndex = nIndex;
				}
				else
				{
					printf("enumPlayerBegin: login %d fail...\n", nIndex);
					m_pGameStatus[lnID].nGameStatus =  enumPlayerBegin;
					m_pGameStatus[lnID].nPlayerIndex = nIndex;
				}
			} 
		}
		break;
	}
} 
 catch (...)
 { 
		TRACE("Core PlayerMessageProcess Error\n");
 } 
}
//处理各个服务器送来的大小包 --从bishop获取大包数据   网关线程
void KSwordOnLineSever::GatewayMessageProcess(const char* pData, size_t dataLength)
{
	//_ASSERT(pData && dataLength);
	if  (NULL==pData||!pData || dataLength<=0) return;
#ifndef _STANDALONE
	BYTE cProtocol = CPackager::Peek( pData );
#else
	BYTE cProtocol = *(unsigned char *)pData;
#endif
	
	if ( cProtocol<s2c_micfghjkdtropackbegin)
	{//大包处理
		GatewayLargePackProcess(pData, dataLength);
	}
	else if ( cProtocol>s2c_micfghjkdtropackbegin)
	{//小包处理
		GatewaySmallPackProcess(pData, dataLength);
	}
	/*else
	{
		_ASSERT( FALSE && "Error!" );
	}*/		
}
//从bishop获取大包数据
void KSwordOnLineSever::GatewayLargePackProcess(const void *pData, size_t dataLength)
{
	if  (NULL==pData||!pData || dataLength<=0) return;
#ifndef _STANDALONE
	//_ASSERT(pData && dataLength);
	CBuffer *pBuffer = m_theRecv.PackUp(pData, dataLength);
#else
	char *pBuffer = m_theRecv.PackUp(pData, dataLength);
#endif
	if (pBuffer)
	{
#ifndef _STANDALONE
		BYTE cProtocol = CPackager::Peek(pBuffer->GetBuffer());
#else
		BYTE cProtocol = *(BYTE *)pBuffer;
#endif	
		try{

		switch (cProtocol)
		{
		case s2c_syncogjfkdnvhf_roleinfo_cipher:
			{//收到bishop发来的协议---
#ifndef _STANDALONE
				tagGuidableInfo *pGI = (tagGuidableInfo *)pBuffer->GetBuffer();
#else
				tagGuidableInfo *pGI = (tagGuidableInfo *)pBuffer;
#endif
				GUID guid;
				memcpy(&guid,&(pGI->guid), sizeof(GUID));
							
				if ( pGI->datalength > 0 )
				{
					const TRoleData *pRoleData = ( const TRoleData * )( pGI->szData );
					/*BOOL	bCRCCheck = TRUE;
					if (pRoleData->dwFriendOffset == pRoleData->dwDataLen - 4 && pRoleData->nFriendCount == 0)
					{//最后的 是 朋友列表的偏移
						// 说明是加过校验的，所以好友的偏移比长度少了4个字节
						DWORD	dwCRC = 0;
						dwCRC = CRC32(dwCRC, pRoleData, pRoleData->dwDataLen - 4);
						DWORD	dwCheck = *(DWORD *)(pGI->szData + pRoleData->dwDataLen - 4);
						if (dwCheck != dwCRC)
						{//写角色数据包
							FILE *fLog = fopen("crc_db_error", "a+b");
							if(fLog) 
							{
								char buffer[255];
								sprintf(buffer, "----\r\n%s\r\b%s\r\n", pRoleData->BaseInfo.szName, pRoleData->BaseInfo.caccname);
								fwrite(buffer, 1, strlen(buffer), fLog);
								fwrite(pRoleData, 1, pRoleData->dwDataLen, fLog);
								fclose(fLog);
							}
							bCRCCheck = FALSE;
						}
					}	*/				
					//把GS的游戏端口发往客户端 56766
					tagPermitPlayerLogin ppl;  //发往bishop 的协议---是否同意玩家登陆
					ppl.cProtocol = c2s_permit_logftihmcginqknd;
					memcpy(&(ppl.guid),&guid,sizeof(GUID));
					snprintf((char *)(ppl.szRoleName), sizeof(ppl.szRoleName), "%s", (const char *)(pRoleData->BaseInfo.szName));
					snprintf((char *)(ppl.szAccountName), sizeof(ppl.szAccountName), "%s", (const char *)(pRoleData->BaseInfo.caccname));
					snprintf(ppl.strRoleNmae, sizeof(ppl.strRoleNmae), "%s", pRoleData->BaseInfo.szName);
					ppl.bPermit = false;
					BOOL nKeepForBt=TRUE;
 
					int nIdx = m_pCoreServerShell->AddCharacter(pGI->nExtPoint, pGI->nChangePoint, (void *)pRoleData, &guid,pGI->nLeftTime,pGI->nVipType,pGI->nGameTime,nKeepForBt,false,pGI->nTempEnTerMapIndx,pGI->nTempKeyId); //更新扩展点 等信息。。。
						
					if (nIdx>0)
					{//网络链接成功
						ppl.bPermit = true;   //是否许可玩家登陆
						m_pCoreServerShell->OperationRequest(SSOI_RELOAD_WELCOME_MSG,0,nIdx);           //发送欢迎语
					    //printf("[SSOI_RELOAD_WELCOME_MSG] send to nIdx:(%d),caccname:(%s)------\n",nIdx,pRoleData->BaseInfo.caccname);		
					}					
				//	}发往bishop
					//printf("发包-服务器，是否(%d)许可玩家(%d                                                                                                                                       )登陆\n",ppl.bPermit,nIdx);
					if (m_pGatewayClient)
					   m_pGatewayClient->SendPackToServer((const void *)&ppl,sizeof(tagPermitPlayerLogin));
				}
			}
			break;

		default:
			break;			
		}

	}
	catch (...)
	{
		TRACE("Core GatewayLargePackProcess Error\n");
	}

#ifndef _STANDALONE
		try
		{
			if( pBuffer != NULL )
			{
				pBuffer->Release();
				pBuffer = NULL; 
			}
		}
		catch(...) 
		{
			//TRACE("SAFE_RELEASE error\n");
		}
#endif
	}
}
//从bishop获取小包信息,并更新BISHOP信息
void KSwordOnLineSever::GatewaySmallPackProcess(const void *pData, size_t dataLength)
{
	//_ASSERT( pData && dataLength );
	if  (NULL==pData||!pData || dataLength<=0) return;
#ifndef _STANDALONE
	BYTE cProtocol = CPackager::Peek( pData );
#else 
	BYTE cProtocol = *(BYTE *)pData;
#endif
/*	printf("-----REAL\ndatalen = %u, data = ", dataLength);
	for (int i = 0; i < 32; i++)
		printf("[%02X] ", ((BYTE *)pData)[i]);
	printf("\n");*/
	try
	{
	switch (cProtocol)
	{
	case c2s_item_synaction_result:
		{//获取数据方执行
			/*VIEW_LIAN_ITEM_SYNC *pItemData = ( VIEW_LIAN_ITEM_SYNC * )pData;
			printf("---数据 ITEM操作:%s  %u ----\n",pItemData->m_WonName,pItemData->m_RandomSeed);*/
			if (m_pCoreServerShell)
				m_pCoreServerShell->sendItemData(pData);
		}
		break;
	case s2c_queryiteminfo:
		{//对物品的跨GS操作
			if  (!pData) break;

			int reg=0;
			if (m_pCoreServerShell)
				reg = m_pCoreServerShell->item_OperationRequest(pData);

			//tagItemAction	*pGatewayBroadCast;
			//pGatewayBroadCast = (tagItemAction *)pData;
			//switch (pGatewayBroadCast->cCmdType)
			//{
			//case enum_ITEM_ID_FIND://目标gs执行 再获取数据转发给源gs的角色(nGmRoleName)
			//	{
			//		printf("---ITEM操作:%s %s %u ----\n",pGatewayBroadCast->nGmRoleName,pGatewayBroadCast->nRoleName,pGatewayBroadCast->nId);
			//		
			//	
			//	}
			//	break;
			//default:
			//	break;
			//}
		}
		break;
	case s2c_querymapinfo:
		//{//更新地图容器 GS开始时 更新
		//
		//	tagUpdateMapID pUMI;
		//	pUMI.cProtocol = c2s_updatemapinfo;
		//	pUMI.cMapCount = m_SerVerNo;//nMapCount; 当前服务器编号
		//	m_pGatewayClient->SendPackToServer((const void *)&pUMI,sizeof(tagUpdateMapID));

		//}
		//break;
		{
			if  (!pData) break;

			tagQueryMapInfo	*pGatewayBroadCast;
			pGatewayBroadCast = (tagQueryMapInfo *)pData;
			switch (pGatewayBroadCast->nProtoKind)
			{
			case enum_COMMAND_ID_MOVETOP_GM://目标gs执行
				{//把地图和坐标反馈给 nGmName 执行却换地图
					printf("---ＧＭ查询:%s 移动 %s:----\n",pGatewayBroadCast->nGmName,pGatewayBroadCast->nName);
					KNpcInfo nInfo;
					memset(&nInfo,0,sizeof(nInfo));
					int reg=0;
					if (m_pCoreServerShell)
						reg = m_pCoreServerShell->GetNpcInfoByNamw(&nInfo,pGatewayBroadCast->nName);

					if (reg)
					{
						tagGmAction pRSI;
						memset(&pRSI,0,sizeof(pRSI));
						pRSI.cProtocol = c2s_gm_synroleaction_result;
						pRSI.cCmdType  = enum_COMMAND_ID_MOVETOP_results_GM;
						snprintf(pRSI.nGmRoleName, sizeof(pRSI.nGmRoleName), "%s", pGatewayBroadCast->nGmName); //源人物
						pRSI.Key  = nInfo.nMapIndx;
						pRSI.verX = nInfo.nPosX;
						pRSI.verY = nInfo.nPosY;

						if  (m_pGatewayClient)
							m_pGatewayClient->SendPackToServer(&pRSI, sizeof(tagGmAction));
					}
				}
				break;
			case enum_COMMAND_ID_MAP:
				{//绑定bishop
					/*tagUpdateMapID *pUMI;
					char	sMapID[1024]; 
					
					int nMapCount = m_pCoreServerShell->GetGameData(SGDI_LOADEDMAP_ID, (UINT_PTR)&sMapID, sizeof(sMapID));
					int nTempDatalen=nMapCount * sizeof(char) + sizeof( tagUpdateMapID );

					pUMI = ( tagUpdateMapID * )new BYTE[ nTempDatalen ];
					memset( pUMI, 0, nTempDatalen);

					printf("---------要求地图数量:%d--------\n",nMapCount);
					
					printf("------datalen:%d sizeof(char):%d sizeof(tagUpdateMapID):%d ---\n",nTempDatalen, sizeof(char), sizeof( tagUpdateMapID ));

					if (nTempDatalen<1024)
					{
						pUMI->cProtocol = c2s_updatemapinfo;
						pUMI->cMapCount  = nMapCount;
						memcpy(pUMI->szMapID, sMapID, nMapCount * sizeof(char));
						m_pGatewayClient->SendPackToServer( ( const void * )pUMI, nTempDatalen );
					}
					else
                        printf("------datalen:%d > 1024 SendPack Error ---\n",nTempDatalen, sizeof(char), sizeof( tagUpdateMapID ));

					if(pUMI)
					{
						delete pUMI;
						pUMI = NULL;
					}*/

					tagUpdateMapID pUMI;
					ZeroMemory(&pUMI,sizeof(tagUpdateMapID));
					pUMI.cProtocol = c2s_updatemapinfo;
					pUMI.cMapCount  = m_GsNumBer;         //GS编号;
					sprintf(pUMI.szparmName,"%s",m_parmName);

		        	if  (m_pGatewayClient)
					    m_pGatewayClient->SendPackToServer(&pUMI, sizeof(tagUpdateMapID));


				}
				break;
			case enum_COMMAND_ID_KICKOUTROLE:
				{//踢除离线挂机的玩家
					if (m_pCoreServerShell)
					   m_pCoreServerShell->SetSaveStatus(pGatewayBroadCast->nName,SAVE_KICKOUT);//停止离线
					//m_pCoreServerShell->ClientDisconnect(nIndex);      //设置离开游戏状态--断开客户端链接
					printf("---开始踢掉离线的玩家:%s----\n",pGatewayBroadCast->nName);

				}
				break;
			case enum_COMMAND_ID_MOVETOP_results_GM://源gs执行
			case enum_COMMAND_ID_LEVEL_GM:
			case enum_COMMAND_ID_MONEY_GM:
			case enum_COMMAND_ID_XU_GM:
			case enum_COMMAND_ID_TASK_GM:
			case enum_COMMAND_ID_FORBITCHAT_GM:
			case enum_COMMAND_ID_KICKOUTROLE_GM:
			case enum_COMMAND_ID_FUYUAN_GM:   //富缘
			case enum_COMMAND_ID_SHENGWANG_GM://声望
			case enum_COMMAND_ID_EXP_GM:
				{//ｇｍ踢某人下线
					//printf("---ＧＭ开始踢掉玩家下线:%s----\n",pGatewayBroadCast->nName);
					if (m_pCoreServerShell)
						m_pCoreServerShell->gm_OperationRequest(pData);
				}
				break;
			default:
				break;
			}
				
			}
			break;	
	case s2c_querygameserverinfo:
		{//创建更新 BISHOP 的 连接环境	IP 端口等
			tagGameSvrInfo ni;
			
			ni.cProtocol = c2s_updategameserverinfo;
			
			ni.nIPAddr_Internet = m_dwInternetIp;
			ni.nIPAddr_Intraner = m_dwIntranetIp;
			ni.nPort            = m_nServerPort;
			ni.wCapability      = m_nMaxPlayerCount;  //最大人数限制
			ni.nSerNoIndex      = m_SerVerNo;         //大区的编号
			ni.nGsNumber        = m_GsNumBer;         //GS编号
			m_pGatewayClient->SendPackToServer( ( const void * )&ni, sizeof( tagGameSvrInfo ) );	
		}
		break;
	case s2c_gwsyn_broadcast://接受BISHOP 转发过来的 消息 主要用于 各个GS之间的信息通讯
		GwBoardCastProcess((const char*)pData, dataLength);
		break;
	case s2c_gateway_broadcast:
		GatewayBoardCastProcess((const char*)pData, dataLength);
		break;
	}
	}
	catch (...)
	{
		TRACE("Core GatewaySmallPackProcess Error\n");
	}
}

void KSwordOnLineSever::GwBoardCastProcess(const char* pData, size_t dataLength)
{
	if  (NULL==pData || !pData) return;

	if (dataLength != sizeof(tagMsgInGame))
		return;

	try
	{
		tagMsgInGame	*pGatewayBroadCast;
		pGatewayBroadCast = (tagMsgInGame *)pData;
		switch(pGatewayBroadCast->cCmdType)
		{//执行从BISHOP发送过来的 操作 命令
		case AR_ADDMSG_GLOBALNEWS_ALL:
			m_pCoreServerShell->sendToClient_globalNews(pData);
			break;
		case AR_ADDMSG_LOCALNEWS_ALL:
			m_pCoreServerShell->sendToClient_localNews(pData);
			break;
		case AR_ADDMSG_GLOBALCOUNTNEWS_ALL:
			m_pCoreServerShell->sendToClient_globalCountNews(pData);
			break;
		case AP_WARNING_ALL_PLAYER_QUIT:
			{//设置本区的某个全局MIss的值 
				m_pCoreServerShell->OperationRequest(SSOI_SETGOLADMISS, (unsigned int)pGatewayBroadCast->nMgsIdx, pGatewayBroadCast->nValIdx);
			}
			break;
		case AP_NOTIFY_GAMESERVER_SAFECLOSE:
			{//踢掉 不是本区的所有玩家
				//m_pCoreServerShell->OperationRequest(SSOI_KICKOUTPLAYER, (unsigned int)pGatewayBroadCast->nMgsIdx, pGatewayBroadCast->nValIdx);
				ExitAllPlayerIngame();
				printf("---开始踢掉所有跨服的玩家----\n");
			}
			break;
		case AP_NOTIFY_ALL_PLAYER://全区公告提示
			//printf("公告:%s \n",pGatewayBroadCast->strMsg);
			m_pCoreServerShell->SendSystemCahtInfo(pGatewayBroadCast->strMsg);
			break;
		case AR_LOADSCRIPT_SERVER:  //加载脚本
			m_pCoreServerShell->gm_LocalScript(pGatewayBroadCast->strMsg);
			break;
		case AR_ADDNPC_SERVER:      //刷怪
			m_pCoreServerShell->gm_addNpc(pData);
			break;
		case AR_OTHER_SERVER:       //其他的
			break;
		default:
			break;
		}

	}
	catch (...)
	{
		TRACE("Core GwBoardCastProcess Error\n");
	}
}

//执行从BISHOP发送过来的 命令
void KSwordOnLineSever::GatewayBoardCastProcess(const char* pData, size_t dataLength)
{
//#ifndef _STANDALONE
	if  (NULL==pData || !pData) return;

	if (dataLength != sizeof(tagGatewayBroadCast))
		return;
	
	try
	{
	tagGatewayBroadCast	*pGatewayBroadCast;
	pGatewayBroadCast = (tagGatewayBroadCast *)pData;
	switch(pGatewayBroadCast->uCmdType)
	{//执行从BISHOP发送过来的 操作 命令
	case AP_WARNING_ALL_PLAYER_QUIT:
		{//通知要安全离线--踢掉所有玩家
			if  (m_pCoreServerShell)
			    m_pCoreServerShell->OperationRequestInfo(SSOI_BROADCASTING, 0, strlen(pGatewayBroadCast->szData),0,0,pGatewayBroadCast->szData);
			// TODO: Save Player and ExitGame when time out
			printf("all_player_quit\n");

			if (pGatewayBroadCast->iSavePai)
			{
				if  (m_pCoreServerShell)
					m_pCoreServerShell->SavePaiShopData();
				printf("Safe save paishopdata to server\n");
			}

			ExitAllPlayer();           //踢掉所有玩家

			if  (m_pCoreServerShell)
				m_pCoreServerShell->ReleaseDbData();
			//SetRunningStatus(FALSE);   //设置GS停止运行
			//ExitAllPlayer();
		}
		break;
	case AP_NOTIFY_GAMESERVER_SAFECLOSE:
		{//安全关闭服务器
			if  (m_pCoreServerShell)
			     m_pCoreServerShell->OperationRequestInfo(SSOI_BROADCASTING, 0, strlen(pGatewayBroadCast->szData),0,0,pGatewayBroadCast->szData);
			
			printf("Safe close server\n");

			if (pGatewayBroadCast->iSavePai)
			{
				if  (m_pCoreServerShell)
					m_pCoreServerShell->SavePaiShopData();

				printf("Safe save paishopdata to server\n");
			}

			ExitAllPlayer();

			if  (m_pCoreServerShell)
				m_pCoreServerShell->ReleaseDbData();
			//SetRunningStatus(FALSE);//设置GS停止运行
		}
		break;
	case AP_NOTIFY_ALL_PLAYER:
		{//通知所有玩家的公告
			printf("-------------发布全服公告-----------\n");
			
			m_nGameLoop = 0;
			//m_nGameDay++;
			m_Timer.Passed(1);
			printf("------------重置计时器成功:%lld-------------\n",m_Timer.GetElapse());
	
			if  (m_pCoreServerShell)
			     m_pCoreServerShell->OperationRequestInfo(SSOI_BROADCASTING,/* (unsigned int)pGatewayBroadCast->szData*/0, strlen(pGatewayBroadCast->szData),0,0, pGatewayBroadCast->szData);
		}
		break;
	default:
		break;
	}

	}
	catch (...)
	{
		TRACE("Core GatewayBoardCastProcess Error\n");
	}
//#endif
}

//////////////////////////无限主循环////////////////////////////////////////
void KSwordOnLineSever::MainLoop()
{

#ifndef _STANDALONE
	//CCriticalSection::Owner locker( g_csFlow );
	 KAutoCriticalSection_ AutoLock(g_csFlow);
#else
	g_mutexFlow.lock();
#endif

	 PlayerLogoutGateway();                //网关断开 循环检测 是否退出游戏的
	 PlayerExchangeServer();               //跨服转换地图时 
    //m_pServer->PreparePackSink();  //准备发包？
    m_pCoreServerShell->Breathe();    //服务器端里的主循环
	//TongattackLoop();	                        //帮会宣战循环

// 定期向数据库查询排名情况

	if (m_nGameLoop & 0x01)                      //每帧都向所有客户端发包  
		m_pServer->SendPackToClient(-1);  //向所有链接发包

	int lnID = m_nGameLoop%m_nMaxGameStaus;	  //随机的
	int nIndex = m_pGameStatus[lnID].nPlayerIndex; //
	if (nIndex > 0 && nIndex < MAX_PLAYER && m_pGameStatus[lnID].nGameStatus == enumPlayerPlaying)
	{//在线的就检测
		if (m_pCoreServerShell && m_pCoreServerShell->IsCharacterLiXian(nIndex)==2)
			return;

#define	defMAX_PING_TIMEOUT		30*GAME_FPS
#define	defMAX_PING_INTERVAL	5*GAME_FPS
#define defMAX_CONNECTION_TIMEOUT (15 * GAME_FPS) 

		if (m_pGameStatus[lnID].nReplyPingTime != 0)	//上次返回的ping时间
		{//客户端有响应的  
			if (m_nGameLoop - m_pGameStatus[lnID].nSendPingTime > defMAX_PING_INTERVAL)
				PingClient(lnID);
		}  
		else if (m_pGameStatus[lnID].nReplyPingTime==0 && m_pGameStatus[lnID].nPingLoopCount<m_pingCount && (m_nGameLoop - m_pGameStatus[lnID].nSendPingTime > defMAX_PING_TIMEOUT))
		{//接受客户端返回的时间差 客户端没响应的
			m_pGameStatus[lnID].nPingLoopCount++;
			printf("Send ping to clientb.... \n");
			PingClient(lnID);	//索引
		}
		else if (m_nGameLoop - m_pGameStatus[lnID].nSendPingTime > defMAX_PING_TIMEOUT)
		{//超时了
			printf("no response from client(%u, %u), ShutdownClient it...\n", m_nGameLoop, m_pGameStatus[lnID].nSendPingTime);
			//设置断开链接
			if (FAILED(m_pServer->ShutdownClient(m_pGameStatus[lnID].nNetidx)))
			{//关闭失败 就强行删除
				if (m_pCoreServerShell)
					m_pCoreServerShell->mandatoryShutDownClient(nIndex);

				if (m_pGameStatus[lnID].nNetidx>0)
					SetNetStatus(m_pGameStatus[lnID].nNetidx,enumNetUnconnect,false);         
			}
		} 
	} 
	else if (m_pGameStatus[lnID].nNetidx >0 && m_pGameStatus[lnID].nNetStatus == enumNetConnected && m_pGameStatus[lnID].nGameStatus == enumPlayerBegin)
	{
		// 检查已经连接但是还没有发登录协议的玩家
		if (m_nGameLoop - m_pGameStatus[lnID].nSendPingTime > defMAX_CONNECTION_TIMEOUT)
		{
			printf("Shutdown non-login client[%u] for timeout\n",m_pGameStatus[lnID].nNetidx);
			//m_pServer->ShutdownClient(m_pGameStatus[lnID].nNetidx);
			if (FAILED(m_pServer->ShutdownClient(m_pGameStatus[lnID].nNetidx)))
			{//关闭失败 就强行删除
			}
			if (m_pGameStatus[lnID].nNetidx>0)
				SetNetStatus(m_pGameStatus[lnID].nNetidx,enumNetUnconnect,false);             //设置断开链接
		}
	}

	/*
	   if (m_nGameLoop & 0x01)  //每帧都向所有客户端发包  
	   { 
		   m_pServer->SendPackToClient(-1);  //向所有链接发包
	   } */

	   ++m_nGameLoop;

#ifdef _STANDALONE
 g_mutexFlow.Unlock();
#endif

}
//实时更新帮会宣战时间
void KSwordOnLineSever::TongattackLoop()
{ //每秒帮会宣战循环
	try
	{
		STONG_ATTACK_COMMAND	sAttack;
		sAttack.ProtocolFamily	= pf_alltong;
		sAttack.ProtocolID		= enumC2S_TONG_ATTACK;
		if (m_pTongClient)
				m_pTongClient->SendPackToServer((const void*)&sAttack, sizeof(sAttack));
	}
	catch (...)
	{
		printf("--------同步帮会出错------- \n");
	}
		
}



//ping 客户端
void KSwordOnLineSever::PingClient(const unsigned long lnID)
{
	//_ASSERT( lnID > 0 && lnID <=m_nMaxGameStaus);
	//_ASSERT(m_pGameStatus[lnID].nNetidx >0 && m_pGameStatus[lnID].nPlayerIndex > 0 && m_pGameStatus[lnID].nPlayerIndex < MAX_PLAYER);	
	//printf("PingClient(%d) called\n", lnID);
	if (lnID < 0 || lnID >m_nMaxGameStaus || m_pGameStatus[lnID].nNetidx<0 ||m_pGameStatus[lnID].nPlayerIndex<=0 || m_pGameStatus[lnID].nPlayerIndex>=MAX_PLAYER)
	    return;
	
	PING_COMMAND	pc;
	pc.m_dwTime = m_nGameLoop;
	pc.ProtocolType = s2c_ping;

	if (m_pServer)	  
	  m_pServer->PackDataToClient(m_pGameStatus[lnID].nNetidx, &pc, sizeof(PING_COMMAND));

	m_pGameStatus[lnID].nReplyPingTime = 0;
	m_pGameStatus[lnID].nSendPingTime = m_nGameLoop;  //备份这个时间和客户端通话的时间
}

void KSwordOnLineSever::ProcessPingReply(const unsigned long lnID, const char* pData, size_t dataLength)
{
	//_ASSERT(lnID <m_nMaxGameStaus);
	//_ASSERT(m_pGameStatus[lnID].nPlayerIndex > 0 && m_pGameStatus[lnID].nPlayerIndex < MAX_PLAYER);
	if (lnID < 0 || lnID >m_nMaxGameStaus || m_pGameStatus[lnID].nNetidx<0 ||m_pGameStatus[lnID].nPlayerIndex<=0 || m_pGameStatus[lnID].nPlayerIndex>=MAX_PLAYER)
		return;

	if  (NULL==pData || !pData) return;

	//printf("receive ping from client...\n");
	if (dataLength != sizeof(PING_CLIENTREPLY_COMMAND))
	{
		printf("ping cmd size not correct, may be Non-offical Client... \n");
		m_pServer->ShutdownClient(m_pGameStatus[lnID].nNetidx);
		return;
	}
	
	PING_CLIENTREPLY_COMMAND*	pPC = (PING_CLIENTREPLY_COMMAND *)pData;
	if (pPC->m_dwReplyServerTime != m_pGameStatus[lnID].nSendPingTime)
	{//时间校对，如果不对则关闭客户端
		printf("wrong time in ping cmd content,kill it... \n");
		m_pServer->ShutdownClient(m_pGameStatus[lnID].nNetidx);
		return;
	}

	m_pGameStatus[lnID].nReplyPingTime = m_nGameLoop;
	// reply client ping to client to announce ping interval;
	PING_COMMAND	pc;
	pc.ProtocolType = s2c_replyclientping;
	pc.m_dwTime = pPC->m_dwClientTime;  //发送客服端返回的时间给客户端设定和服务器通话的时间
	m_pServer->PackDataToClient(m_pGameStatus[lnID].nNetidx, &pc, sizeof(PING_COMMAND));
}

// 保持连接的玩家自动存盘
/*void KSwordOnLineSever::SavePlayerData()
{
	if (!m_pDatabaseClient)
		return;

	int i = 0;
	
	//TProcessData	sProcessData;

	//sProcessData.nProtoId = c2s_roleserver_saveroleinfo;
	//sProcessData.ulIdentity = -1;

	// 遍历网络连接
	for (i = 0; i < m_nMaxGameStaus;++i)
	{
		int nIndex = m_pGameStatus[i].nPlayerIndex;
	
		if (GetNetStatus(i) == enumNetUnconnect && !m_pCoreServerShell->IsCharacterLiXian(nIndex)) //网路断开 就不能存档了 
			continue;

		if (nIndex <= 0 || nIndex >=MAX_PLAYER)
			continue;

		if (m_pGameStatus[i].nNetidx==-1 && !m_pCoreServerShell->IsCharacterLiXian(nIndex))	//已经断网的了
		   	continue;	

		//TRoleData* pData = (TRoleData *)m_pCoreServerShell->SavePlayerData(nIndex); 
		if (m_pCoreServerShell->GetSaveStatus(nIndex) == SAVE_REQUEST)
		{//立即存档的
			SavePlayerData(nIndex, false);
		}
	}
}
*/
BOOL KSwordOnLineSever::SavePlayerData(int nIndex, bool bUnLock)
{
	if (!m_pDatabaseClient)
	{
		cout << "--------------GS与数据库断开链接---------------" << endl;
		return FALSE;
	}

	if (nIndex <= 0 || nIndex >= MAX_PLAYER)  //m_nMaxGameStaus
	{
		//cout << "-------------------GS人物索引超限-----------------------" << endl;
		printf("----------------GS人物索引(%d)超限,存档失败------------------\n",nIndex);
		return FALSE;
	}

	TProcessData sProcessData;

	sProcessData.nProtoId   = c2s_rolekfjhu_saveroleinfo;	  //发往goddes存档
	sProcessData.ulIdentity = nIndex;
	sProcessData.bLeave     = bUnLock;
	sProcessData.uSelServer = m_SerVerNo;
	//printf("-----------存档索引:%d-----------\n", nIndex);
	TRoleData* pData = (TRoleData *)m_pCoreServerShell->SavePlayerDataAtOnce(nIndex);	//赋值了最后存档时间
    //cout << "-------角色("<< pData->BaseInfo.szName <<")GS开始存档:物品("<< pData->nItemCount <<")个------索引:" << nIndex << endl;
	
	if  (NULL==pData||!pData ) 
	{
		printf("-------KSwordOnLineSever::pData==NULL存档失败-------\n");
		return FALSE;
	}

	if (pData->nItemCount<=0)
	 {	 		  
       printf("XXXXXXXXXXXXXXXXXXXXXX(%s)XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n",pData->BaseInfo.szName);
	   printf("XXXX存档XXXX 警告:物品数量为NULL,数量请检查!! XXXXXXXXXXXXXXXXXX\n");
	   printf("XXXX存档XXXX 警告:物品数量为NULL,数量请检查!! XXXXXXXXXXXXXXXXXX\n");
	   printf("XXXX存档XXXX 警告:物品数量为NULL,数量请检查!! XXXXXXXXXXXXXXXXXX\n");
	   printf("XXXX存档XXXX 警告:物品数量为NULL,数量请检查!! XXXXXXXXXXXXXXXXXX\n");
	   printf("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n");
	 }

	//printf("-----------存档长度C:%d-----------\n", pData->dwDataLen);
	if (pData && pData->dwDataLen)
	{	//size_t == unsigned int
		sProcessData.nDataLen = pData->dwDataLen + sizeof(TProcessData) - 1;
		
		/*pData->dwDataLen += 4;
		DWORD	dwCRC = 0;
		dwCRC = CRC32(dwCRC, pData, pData->dwDataLen - 4);*/
		// CRC END	容器增加数据
		//printf("-----------存档长度D:%d-----------\n", pData->dwDataLen);
		m_theSend.AddData(c2s_rolekfjhu_saveroleinfo, (const BYTE*)&sProcessData, sizeof(TProcessData) - 1);
		//printf("-----------存档长度K:%d-----------\n", pData->dwDataLen);

		//m_theSend.AddData(c2s_rolekfjhu_saveroleinfo, (const BYTE*)pData, pData->dwDataLen - 4);
		m_theSend.AddData(c2s_rolekfjhu_saveroleinfo, (const BYTE*)pData, pData->dwDataLen);
		//printf("-----------存档长度L:%d-----------\n", pData->dwDataLen);

		//m_theSend.AddData(c2s_rolekfjhu_saveroleinfo, (const BYTE*)&dwCRC, 4);
		//printf("-----------存档长度E:%d-----------\n", pData->dwDataLen);

#ifndef _STANDALONE
		//printf("-----------存档长度F:%d-----------\n", pData->dwDataLen);
		CBuffer* pBuffer = m_theSend.GetHeadPack(c2s_rolekfjhu_saveroleinfo);
		//printf("-----------存档长度H:%d-----------\n", pData->dwDataLen);
#else
		size_t actLen = 0;
		//printf("-----------存档长度I:%d-----------\n", pData->dwDataLen);
		char* pBuffer = m_theSend.GetHeadPack(c2s_rolekfjhu_saveroleinfo, actLen);
		//printf("-----------存档长度J:%d-----------\n", pData->dwDataLen);
#endif
		int loopCount=0;
		while(pBuffer)
		{//发往goddes
			loopCount ++;
#ifndef _STANDALONE
			//printf("-----------存档长度A:%d-----------\n", pBuffer->GetUsed());
			m_pDatabaseClient->SendPackToServer(pBuffer->GetBuffer(), pBuffer->GetUsed());
#else
			//printf("-----------存档长度B:%d-----------\n",actLen);
			m_pDatabaseClient->SendPackToServer(pBuffer, actLen);
#endif

#ifndef _STANDALONE
			try
			{
				if (pBuffer)
				{
					pBuffer->Release();
					pBuffer = NULL;
				}
			}
			catch(...) 
			{
				//TRACE("SAFE_RELEASE error\n");
			}
#endif

#ifndef _STANDALONE
			pBuffer = m_theSend.GetNextPack(c2s_rolekfjhu_saveroleinfo);   //下一个包
#else
			pBuffer = m_theSend.GetNextPack(c2s_rolekfjhu_saveroleinfo, actLen);
#endif
		}
		m_theSend.DelData(c2s_rolekfjhu_saveroleinfo);
#ifndef _STANDALONE
		try
		{
			if (pBuffer)
			{
				pBuffer->Release();
				pBuffer = NULL;
			}
		}
		catch(...) 
		{

		}
#endif
		printf("-------gs:Saving %s data, size:(%d)--sendCount:%d--itemCount:%d-----\n", pData->BaseInfo.szName, sProcessData.nDataLen,loopCount,pData->nItemCount);
		if  (bUnLock && m_pCoreServerShell)
		   m_pCoreServerShell->SetSaveStatus(nIndex,SAVE_DONE);

		return TRUE;
	}

	return FALSE;
}
//跨服处理 玩家改变服务器（转换地图）
void KSwordOnLineSever::PlayerExchangeServer()
{
	if (!m_pGatewayClient || !m_pCoreServerShell)
		return;

	int i;
	char szChar[1024]={0};
	// 遍历网络连接
	for (i = 0; i < m_nMaxGameStaus;i++)
	{
		int nIndex = m_pGameStatus[i].nPlayerIndex;

		if (nIndex <= 0 || nIndex >= MAX_PLAYER)
			continue;

		//if(m_pCoreServerShell->GetClientNetConnectIdx(nIndex)==-1)
		if (m_pGameStatus[i].nNetidx==-1)
		{//如果是掉线的了
			//g_pSOServer.SetNetStatus(i, enumNetUnconnect); //清空数据
			continue;
		}
		//检测是否 跨服状态
		if (m_pCoreServerShell->IsPlayerExchangingServer(nIndex))
		{   //转换地图
			if (!m_pTransferClient)
			{//如果转服服务器断开的话 就是通知客户端 转换地图
				tagNotifyPlayerExchange	npe;
				npe.cProtocol = s2c_notifyplayerexchange;
				memset(&npe.guid, 0, sizeof(GUID));

				npe.nIPAddr = 0;  //跨服的 ip
				npe.nPort   = 0;
				//发送客户端
				m_pServer->SendData(m_pGameStatus[i].nNetidx, &npe, sizeof(tagNotifyPlayerExchange));

			    //if (!SavePlayerData(nIndex, true)) //存档,没有生成存档包
				if (!m_pCoreServerShell->SavePlayerData(nIndex,false))	//生成存档包 存档
					continue;

				m_pCoreServerShell->RecoverPlayerExchange(nIndex);	//取消转服状态

				tagRoleEnterGame reg;
				reg.ProtocolType = c2s_roleserver_lock;
				reg.bLock = true;
				reg.Lixian = false;                                  //是否离线的
				reg.nRegServerIndex = m_SerVerNo;  //属于那个大区
				reg.nGsNumber = m_GsNumBer;         //GS索引号

				//m_pCoreServerShell->GetGameData(SGDI_CHARACTER_NAME, (unsigned int)reg.Name, nIndex);
				m_pCoreServerShell->GetGameDataInfo(SGDI_CHARACTER_NAME, 0, nIndex,reg.Name);

				if (m_pDatabaseClient)
					m_pDatabaseClient->SendPackToServer((const void *)&reg, sizeof(tagRoleEnterGame));
				if (m_pGatewayClient)
					m_pGatewayClient->SendPackToServer((const void *)&reg, sizeof(tagRoleEnterGame));

				m_pGameStatus[i].nGameStatus = enumPlayerPlaying;
				m_pGameStatus[i].nExchangeStatus = enumExchangeBegin;

				break;
			}
			//否则	开始跨服
			m_pGameStatus[i].nGameStatus = enumPlayerExchangingServer;

			switch (m_pGameStatus[i].nExchangeStatus)
			{
			case enumExchangeBegin:
				{//开始转服
				
					RELAY_ASKWAY_DATA	*pRAD = (RELAY_ASKWAY_DATA *)szChar;

					pRAD->ProtocolFamily = pf_relay;
					pRAD->ProtocolID        = relay_c2c_askwaydata;

					pRAD->nFromIP           = 0;//m_SerVerNo;// 0.0.0.0  哪个服务器 需要转服
					pRAD->nFromRelayID = 0;//m_SerVerNo;
					pRAD->seekRelayCount = 0;
					// 利用地图ID找SERVER
					pRAD->seekMethod = rm_map_id;
					pRAD->wMethodDataLength = 4;
					pRAD->routeDateLength = sizeof(tagSearchWay);

					//获取目标 地图id
					*(DWORD *)(szChar + sizeof(RELAY_ASKWAY_DATA)) = m_pCoreServerShell->GetExchangeMap(nIndex);

					tagSearchWay *pSW = (tagSearchWay *)(szChar + sizeof(RELAY_ASKWAY_DATA) + pRAD->wMethodDataLength);
					pSW->cProtocol = c2c_notifyexchange;
					pSW->lnID = i;
					pSW->nIndex = nIndex;// m_pGameStatus[i].nPlayerIndex;
					pSW->dwPlayerID = m_pCoreServerShell->GetGameData(SGDI_CHARACTER_ID, pSW->nIndex, 0);
					if  (m_pTransferClient)
					    m_pTransferClient->SendPackToServer(pRAD, sizeof(RELAY_ASKWAY_DATA) + pRAD->wMethodDataLength + pRAD->routeDateLength);
					
					m_pGameStatus[i].nExchangeStatus = enumExchangeSearchingWay;
					//printf("----收到通知(%d)开始转服B!-----\n",i);
				}
				break;
			case enumExchangeSearchingWay:
				break;
			case enumExchangeWaitForGameSvrRespone:
				break;
			case enumExchangeCleaning:
				break;
			}
		}
	}

}
//退出网关的
void KSwordOnLineSever::PlayerLogoutGateway()
{
	if (!m_pGatewayClient || !m_pCoreServerShell)
		return;

	tagLeaveGame	sLeaveGame;	
	sLeaveGame.cProtocol = c2s_leavegame;   // 发送往bishop 通知 paysys
	sLeaveGame.cCmdType  = NORMAL_LEAVEGAME;

	tagLeaveGame2 lg2;
	
	lg2.ProtocolFamily = pf_normal;
	lg2.ProtocolID     = c2s_leavegame;	   // S3 玩家离开游戏
	lg2.cCmdType       = NORMAL_LEAVEGAME;
	lg2.nSelServer     = m_SerVerNo;      //  MAKELONG(m_GsNumBer, m_SerVerNo);//
	// 遍历玩家（不能遍历连接，因为Timeout部分的玩家没有连接）
	for (int nIndex = 1; nIndex < MAX_PLAYER; nIndex++)
	{
		if (m_pCoreServerShell->IsPlayerLoginTimeOut(nIndex))
		{//超时登陆的
			char szName[ACC_LENGTH]={0},szRoleName[NAME_LENGHT]={0};
			//m_pCoreServerShell->GetGameData(SGDI_CHARACTER_ACCOUNT, (unsigned int)szName, nIndex);
			m_pCoreServerShell->GetGameDataInfo(SGDI_CHARACTER_ACCOUNT,0, nIndex,szName);
			sLeaveGame.nExtPoint    = m_pCoreServerShell->GetExpData(nIndex,0);
			sLeaveGame.nGameExtTime = m_pCoreServerShell->GetExpData(nIndex,1);
			//m_pCoreServerShell->GetGameData(SGDI_CHARACTER_NAME, (unsigned int)szRoleName, nIndex);
			m_pCoreServerShell->GetGameDataInfo(SGDI_CHARACTER_NAME,0, nIndex,szRoleName);

			//sprintf(sLeaveGame.nClientIp,"127.0.0.1");
			snprintf((char *)sLeaveGame.nClientIp, sizeof(sLeaveGame.nClientIp), "%s", (char *)szRoleName);
			snprintf((char *)sLeaveGame.szAccountName, sizeof(sLeaveGame.szAccountName), "%s", (char *)szName);

			if (m_pGatewayClient)
			   m_pGatewayClient->SendPackToServer(&sLeaveGame, sizeof(tagLeaveGame)); //通知bishop

			snprintf((char *)lg2.szAccountName, sizeof(lg2.szAccountName), "%s", (char *)szName);
			
			if (m_pTransferClient)//通知S3
				m_pTransferClient->SendPackToServer(&lg2, sizeof(tagLeaveGame2));
			if (m_pChatClient)
				m_pChatClient->SendPackToServer(&lg2, sizeof(tagLeaveGame2));
			if (m_pTongClient)
				m_pTongClient->SendPackToServer(&lg2, sizeof(tagLeaveGame2));


			tagRoleEnterGame reg;
			reg.ProtocolType = c2s_roleserver_lock;
			reg.bLock = false;            //删除锁定
			reg.Lixian = false;           //是否离线的
			reg.nRegServerIndex = m_SerVerNo;  //属于那个大区
			reg.nGsNumber = m_GsNumBer;        //GS索引号
			//m_pCoreServerShell->GetGameData(SGDI_CHARACTER_NAME, (unsigned int)reg.Name, nIndex);
			m_pCoreServerShell->GetGameDataInfo(SGDI_CHARACTER_NAME,0, nIndex,reg.Name);
			if (m_pDatabaseClient)
				m_pDatabaseClient->SendPackToServer((const void *)&reg, sizeof(tagRoleEnterGame));
			if (m_pGatewayClient)
				m_pGatewayClient->SendPackToServer((const void *)&reg, sizeof(tagRoleEnterGame));
			//cout <<sLeaveGame.szAccountName << " ----------登录超时leave game!-------------" << endl;
			printf("------(%s)Dang nhap qua thoi gian------- \n",sLeaveGame.szAccountName);
			//断开网络后才可以删除　 && m_pCoreServerShell->GetSaveStatus(nIndex)==SAVE_REQUEST
			if (m_pCoreServerShell) //已经断开网络的
			    m_pCoreServerShell->RemovePlayerLoginTimeOut(nIndex);
		}
		else if (m_pCoreServerShell->IsCharacterQuiting(nIndex))
		{//超过人数 或者设置 退出的

			if(!m_pCoreServerShell->IsShutdownClient(nIndex))
			{//断开客户端
				//cout << " ----------退出游戏,返回!-------------" << endl; 
				continue;
			}
            //已经断开网络的   || m_pCoreServerShell->GetSaveStatus(nIndex)==SAVE_DONE)
			if (m_pCoreServerShell->GetSaveStatus(nIndex)!=SAVE_REQUEST )
		        continue;

			if(m_pCoreServerShell->IsCharacterLiXian(nIndex) == 0 || m_pCoreServerShell->IsCharacterLiXian(nIndex) == 1)
			{//不是托管的
				char	szName[NAME_LENGHT]={0};
				//m_pCoreServerShell->GetGameData(SGDI_CHARACTER_ACCOUNT, (unsigned int)szName, nIndex);
				m_pCoreServerShell->GetGameDataInfo(SGDI_CHARACTER_ACCOUNT, 0, nIndex,szName);

				sLeaveGame.nExtPoint    = m_pCoreServerShell->GetExpData(nIndex,0);
				//m_pCoreServerShell->GetGameData(SGDI_CHARACTER_EXTPOINTCHANGED, nIndex, 0); // 获取要扣除的扩展点
				sLeaveGame.nGameExtTime = m_pCoreServerShell->GetExpData(nIndex,1);
				//m_pCoreServerShell->GetGameData(SGDI_CHARACTER_EXTTIMECHANGED, nIndex, 0);
				sprintf(sLeaveGame.nClientIp,"127.0.0.1");
				snprintf((char *)sLeaveGame.szAccountName, sizeof(sLeaveGame.szAccountName), "%s", (char *)szName);
				if (m_pGatewayClient)
					m_pGatewayClient->SendPackToServer(&sLeaveGame, sizeof(tagLeaveGame));

				snprintf((char *)lg2.szAccountName, sizeof(lg2.szAccountName), "%s", (char *)szName);

				if (m_pTransferClient)
					m_pTransferClient->SendPackToServer(&lg2, sizeof(tagLeaveGame2));

				if (m_pChatClient)
					m_pChatClient->SendPackToServer(&lg2, sizeof(tagLeaveGame2));

				if (m_pTongClient)
					m_pTongClient->SendPackToServer(&lg2, sizeof(tagLeaveGame2));


				tagRoleEnterGame reg;
				reg.ProtocolType = c2s_roleserver_lock;
				reg.bLock  = false;  //删除锁定
				reg.Lixian = false;           //是否离线的
				reg.nRegServerIndex = m_SerVerNo;  //属于那个大区
				reg.nGsNumber = m_GsNumBer;        //GS索引号

				//m_pCoreServerShell->GetGameData(SGDI_CHARACTER_NAME, (unsigned int)reg.Name, nIndex);
				m_pCoreServerShell->GetGameDataInfo(SGDI_CHARACTER_NAME,0, nIndex,reg.Name);

				if (m_pCoreServerShell->IsCharacterLiXian(nIndex) == 1)
				{//设置模式为2
					reg.bLock  = true;
					reg.Lixian = true;//是否离线的
					m_pCoreServerShell->SetCharacterLiXian(nIndex,2);
					printf("------(%s) Uy Thac Thanh Cong!------- \n",sLeaveGame.szAccountName);
				}

				if (m_pDatabaseClient)
					m_pDatabaseClient->SendPackToServer((const void *)&reg, sizeof(tagRoleEnterGame));

				if (m_pGatewayClient)
					m_pGatewayClient->SendPackToServer((const void *)&reg, sizeof(tagRoleEnterGame));
			}       
			m_pCoreServerShell->RemoveQuitingPlayer(nIndex);   //删除玩家立即存档

		}

	}
}
//获取网络状态
int KSwordOnLineSever::GetNetStatus(const unsigned long lnID)
{
	if (lnID<0 || lnID >m_nMaxGameStaus)  //网络号大于最大人数 设置为不链接状态
		return enumNetUnconnect;

	return m_pGameStatus[lnID].nNetStatus;
}
//设置网络状态
void KSwordOnLineSever::SetNetStatus(const unsigned long lnID, NetStatus nStatus,bool isSave)
{
	//if (lnID >= m_nMaxGameStaus)
	//	return;
	//const char* pInfo = m_pServer->GetClientInfo(lnID);
	 
	if (nStatus == enumNetUnconnect)
	{//设置断开连接状态
		int j;
		    j = FindSame(lnID); // 查找相同
		if (j>0 && j<=m_nMaxGameStaus)
		{
		   int nIndex = m_pGameStatus[j].nPlayerIndex;
		   if (m_pGameStatus[j].nGameStatus == enumPlayerPlaying\
			|| (m_pGameStatus[j].nGameStatus == enumPlayerExchangingServer\
			&& m_pGameStatus[j].nExchangeStatus != enumExchangeCleaning))	// 跨服务器时自己处理
		   { //游戏中 或 转换地图 开始存一次挡
			  
			   if (m_pCoreServerShell->GetClientNetConnectIdx(nIndex)>-1)
			   {//确保在 删除节点前断线前生成 存档包
				   m_pCoreServerShell->CheckSerMaps(nIndex,TRUE);
				   m_pCoreServerShell->SetSaveStatus(nIndex,SAVE_IDLE);
				   if (isSave)
			          m_pCoreServerShell->SavePlayerData(nIndex,isSave);	          //生成存档包,马上存档

				  printf("------SetNetStatus Exit the game and save successfully(%d,save:%d)------\n",nIndex,isSave);   
			   }
			   else
			   {
				   printf("------Exit the game, disconnected, no archive processing(%d)------\n",nIndex);
			   }
		   }  
		   else
		   {   //登陆失败
			   m_pCoreServerShell->PreparePlayerForLoginFailed(nIndex);
		   }

			   m_FreeIdxNetStatus.Insert(j);        //删除链表
			   m_UseIdxNetStatus.Remove(j);

			   m_pGameStatus[j].nGameStatus = enumPlayerNone;             //enumPlayerBegin;
			   m_pGameStatus[j].nPlayerIndex = 0;
			   m_pGameStatus[j].nExchangeStatus = enumExchangeCleaning;   //设置开始转服
			   m_pGameStatus[j].nReplyPingTime = 0;
			   m_pGameStatus[j].nSendPingTime = 0;
			   m_pGameStatus[j].nNetidx=-1;
			   m_pGameStatus[j].nNetStatus = nStatus;
			   m_pGameStatus[j].nErrorLoopCount = 0;
			   m_pGameStatus[j].nPingLoopCount  = 0;

			   //printf("------网络(%u)删除节点成功(%d),状态:%d------\n",lnID,j,m_pGameStatus[j].nNetStatus);

		}  

		/*
		m_pGameStatus[lnID].nGameStatus = enumPlayerBegin;
		m_pGameStatus[lnID].nPlayerIndex = 0;
		m_pGameStatus[lnID].nExchangeStatus = enumExchangeBegin;   //设置开始转服
		m_pGameStatus[lnID].nReplyPingTime = 0;
		m_pGameStatus[lnID].nSendPingTime = 0;
		*/

	}

	if (nStatus == enumNetConnected)
	{//设置网络链接状态
	   int i;
		i = FindFree();//1--500
		if (i>0 && i<=m_nMaxGameStaus)
		{
			m_FreeIdxNetStatus.Remove(i);
			m_UseIdxNetStatus.Insert(i);

			m_pGameStatus[i].nGameStatus = enumPlayerBegin;
			m_pGameStatus[i].nPlayerIndex = 0;
			m_pGameStatus[i].nExchangeStatus = enumExchangeBegin;//enumExchangeBegin;
			m_pGameStatus[i].nReplyPingTime = 0;
		    m_pGameStatus[i].nSendPingTime  = m_nGameLoop;
			m_pGameStatus[i].nNetidx=lnID;
			m_pGameStatus[i].nNetStatus = nStatus;
			m_pGameStatus[i].nErrorLoopCount = 0;
			m_pGameStatus[i].nPingLoopCount  = 0;
			printf("------网络id(%u)增加节点(%d),状态:%d 超时计时:%u------\n",lnID,i,m_pGameStatus[i].nNetStatus,m_pGameStatus[i].nSendPingTime);
		}
		/*
		m_pGameStatus[lnID].nGameStatus = enumPlayerBegin;
		m_pGameStatus[lnID].nPlayerIndex = 0;
		m_pGameStatus[lnID].nExchangeStatus = enumExchangeBegin;
		m_pGameStatus[lnID].nReplyPingTime = 0;
		m_pGameStatus[lnID].nSendPingTime = 0;
		*/
	}

	//m_pGameStatus[lnID].nNetStatus = nStatus;		
}
//登陆协议

//inline const char* _ip2a(DWORD ip) { in_addr ia; ia.s_addr = ip; return inet_ntoa(ia); }
//inline DWORD _a2ip(const char* cp) { return inet_addr(cp); }
int KSwordOnLineSever::ProcessLoginProtocol(const unsigned long lnID, const char* pData, size_t dataLength)
{

	if  (lnID<0 || lnID>m_nMaxGameStaus)
	{
		printf("----------ProcessLoginProtocol:错误的网络号--------\n");
		return 0;
	}

	if  (NULL==pData || !pData) return false;

	const char* pBuffer = pData;
//收到客户端的请求
	if (*pBuffer != c2s__loginfs_kfjghtueodnchsf)  //如果协议标示错误就返回0
	{
		printf("----------Failed to log in A-------- \n");
		m_pServer->ShutdownClient(m_pGameStatus[lnID].nNetidx);  //关闭客户端
		return 0;
	}
	else
	{
		//---------------IP过滤设置-----------------------------------------------------
			/*char szDesMsg[32];
		sprintf(szDesMsg,"%s",m_pServer->GetClientInfo(lnID)); //获取链接的网络号的 IP
        
		//int pRect[4];
        //sscanf(szDesMsg, "%d.%d.%d", &pRect[0], &pRect[1], &pRect[2]);
        //if (pRect[0]!=192 && pRect[1]!=168)
	   if ((pRect[0]==192 && pRect[1]==168) || (pRect[0]==127 && pRect[1]==0 && pRect[2]==0))
		{

		}
	    else
		{ 
			m_pServer->ShutdownClient(lnID);  //关闭客户端
		    //--SetRunningStatus(FALSE);---          //设置GS停止运行
			return 0;
		} */
		//---------------------------------------------------------------------------
		printf("----------The client is successful in sending packets:c2s__loginfs_kfjghtueodnchsf--------\n");
        //这个网络号无限增大
		tagLogicLogin *pLL = (tagLogicLogin *)pData;

		if  (pLL->cProtocol!=c2s__loginfs_kfjghtueodnchsf)
		{
			printf("----------Failed to log B--------\n");
			m_pServer->ShutdownClient(m_pGameStatus[lnID].nNetidx);  //关闭客户端
		    return 0;
		}

		//获取玩家的 nUseIdx 链表号	 playerid
		int nIdx = m_pCoreServerShell->AttachPlayer(m_pGameStatus[lnID].nNetidx,&pLL->guid);   //lnID 为网络号	将网络链接号赋值给人物

		if (nIdx)
		{
		   printf("The login request of client (%d) was accepted\n",m_pGameStatus[lnID].nNetidx);
           return nIdx;
		}
		else
		{
			// 非法的玩家，该怎么处理怎么处理
			/*
			buf.Format("{%08X-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X}"
			, guid.Data1
			, guid.Data2
			, guid.Data3
			, guid.Data4[0], guid.Data4[1]
			, guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5]
       ,      guid.Data4[6], guid.Data4[7]);
			
			*/
			printf("hop phap (%d), hop phap.C<%08X-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X> \n"\
				,lnID,pLL->guid.Data1,pLL->guid.Data2\
				,pLL->guid.Data3,pLL->guid.Data4[0],pLL->guid.Data4[1]\
				,pLL->guid.Data4[2],pLL->guid.Data4[3],pLL->guid.Data4[4],pLL->guid.Data4[5]\
                ,pLL->guid.Data4[6], pLL->guid.Data4[7]);

			m_pServer->ShutdownClient(m_pGameStatus[lnID].nNetidx);  //关闭客户端
			return 0;
		}
	}
	return 0;	
}
//有问题 容易卡号 就在这里了
BOOL KSwordOnLineSever::SendGameDataToClient(const unsigned long lnID, const int nPlayerIndex)
{
	BOOL bRet = FALSE;
//#ifndef _STANDALONE
	//_ASSERT(m_pServer);

	if  (!m_pServer || lnID<=0 || lnID>m_nMaxGameStaus) return false;

	int				nStep = 0;
	unsigned int	nParam = 0;
	int				bSyncEnd = 0;

	m_pServer->PreparePackSink(); //通知服务器准备发包

	while(true)  //循环同步 玩家登陆信息
	{
		bSyncEnd = (nStep == STEP_SYNC_END);   //如果 nStep == STEP_SYNC_END,就将 TRUE 赋值给 bSyncEnd, 否则 FALSE;
		//加载玩家数据 读档
		bRet = m_pCoreServerShell->PlayerDbLoading(nPlayerIndex, bSyncEnd, nStep, nParam);

		if (!bRet)
		{
			printf(".....File read failed:%d .....\n",nPlayerIndex);	//[wxb 2003-7-28]
			break;
		}

		//if (nParam!=0)
		//	continue;

		if (bSyncEnd)
		{	//通知客户端 同步完毕		
			m_pGameStatus[lnID].nGameStatus = enumPlayerSyncEnd;
			printf("[Dong Bo] Dong bo bSyncEnd:%d,nNetidx:%d\n",bSyncEnd,m_pGameStatus[lnID].nNetidx);
			CLIENT_SYNC_END	SyncEnd;
			//BYTE	SyncEnd;
			SyncEnd.ProtocolType = s2c_syncclientend;  //(BYTE)
			SyncEnd.m_IsLogin    = TRUE;
			SyncEnd.m_clientKey  = 9473290;
			g_SetRootPath(NULL);
			g_SetFilePath("\\");
			KIniFile iniFile;

			char nTempConfigPath[64];
			snprintf(nTempConfigPath, sizeof(nTempConfigPath),"%s_cfg.ini",m_parmName);
			if (iniFile.Load(nTempConfigPath))
			{
				int tempInt;
				iniFile.GetInteger("GameServer","ClientKey",8570294,&tempInt);
				SyncEnd.m_clientKey = tempInt;
				printf("[Dong Bo] nTempConfigPath:%s\n",nTempConfigPath);
				printf("[Dong Bo] SyncEnd.m_clientKey:%d\n",SyncEnd.m_clientKey);
				iniFile.Clear();
			}

			printf("[Login] lnID:%d,nPlayerIndex:%d Login and SendGameDataToClient Succes!!!!!!\n",lnID,nPlayerIndex);

#ifndef _STANDALONE
			if (FAILED(m_pServer->PackDataToClient(m_pGameStatus[lnID].nNetidx,&SyncEnd, sizeof(CLIENT_SYNC_END)))) //(BYTE*)
#else
			if (!SUCCEEDED(m_pServer->PackDataToClient(m_pGameStatus[lnID].nNetidx,&SyncEnd, sizeof(CLIENT_SYNC_END))))//(BYTE*)
#endif
			{
				bRet = FALSE;
				break;
			}

			break;
		}
	}   //end while
//是否能链接上客户端,否则就是失败!
#ifdef WIN32
	if (FAILED(m_pServer->SendPackToClient(m_pGameStatus[lnID].nNetidx)))
	{
		bRet = FALSE;
	}
#else
	if (m_pServer->SendPackToClient(m_pGameStatus[lnID].nNetidx) <= 0)
		bRet = FALSE;
#endif

	return bRet;
}
//是否已经同步完客户端
BOOL KSwordOnLineSever::ProcessSyncReplyProtocol(const unsigned long lnID, const char* pData, size_t dataLength)
{
	/*const char*	pBuffer = pData;

	if (*pBuffer != c2s_syncclientend)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}*/
	if  (NULL==pData || !pData) return false;

	CLIENT_SYNC_END *nCurInfo = (CLIENT_SYNC_END *)pData;
	g_SetRootPath(NULL);
	g_SetFilePath("\\");
	KIniFile iniFile;
	int      tempInt = 7547593;	// the default GetInteger is handed below: without the
								// ini this gate must refuse, not read a stack slot

	char nTempConfigPath[64];
	snprintf(nTempConfigPath, sizeof(nTempConfigPath),"%s_cfg.ini",m_parmName);
	if (iniFile.Load(nTempConfigPath))
	{
		iniFile.GetInteger("GameServer","ClientKey",7547593,&tempInt);
		iniFile.Clear();
	}

	if (nCurInfo->ProtocolType !=c2s_syncclientend || !nCurInfo->m_IsLogin ||nCurInfo->m_clientKey!=tempInt) return false;

	return true;	
}


void KSwordOnLineSever::ExitAllPlayerIngame()
{

#ifndef __linux	
			Sleep(4000);
#else
			Sleep(4);
#endif

	int lnID;
	for (lnID = 0; lnID < m_nMaxGameStaus;lnID++)
	{
		int nIndex = m_pGameStatus[lnID].nPlayerIndex;
		if (nIndex <= 0 || nIndex>=MAX_PLAYER)
			continue; 

		if (!m_pCoreServerShell->IsShutdownClientNoserver(nIndex))
		{//负责断开客户端
			continue;
		}
	}
}
//BiSHOP踢掉所有玩家
void KSwordOnLineSever::ExitAllPlayer()
{

	
#ifndef __linux			//SwitchToThread();
			Sleep(4000);
#else
		     //usleep(4000000);
			Sleep(4);
#endif

	int lnID;
	/*const char*	pData;
	unsigned int	uSize;
	tagLeaveGame	sLeaveGame;	

	sLeaveGame.cProtocol = c2s_leavegame;
	sLeaveGame.cCmdType = NORMAL_LEAVEGAME;
	*/
	for (lnID = 0; lnID < m_nMaxGameStaus; lnID++)
	{
		int nIndex = m_pGameStatus[lnID].nPlayerIndex;
		if (nIndex <= 0 || nIndex>=MAX_PLAYER)
			continue; 

		if  (m_pCoreServerShell)
			m_pCoreServerShell->IsShutdownClientOutserver(nIndex);

		if (m_pCoreServerShell && m_pCoreServerShell->IsCharacterLiXian(nIndex)==2)
		{
			printf("---Kick Uot Uy Thac:%d:%s----\n",nIndex);
			m_pCoreServerShell->SetSaveStatus(nIndex,SAVE_KICKOUT);//停止离线
		}
		//m_pCoreServerShell->ClientDisconnect(nIndex);      //设置离开游戏状态--断开客户端链接
		

		/*if (!m_pCoreServerShell->IsShutdownClient(nIndex))
		{//负责断开客户端
			continue;
		}*/
	/*
		BOOL bSave = m_pCoreServerShell->SavePlayerData(nIndex);  //存盘

		int nCount = 0;

		while(bSave)
		{
			nCount++;
			pData = (const char *)m_pDatabaseClient->GetPackFromServer(uSize);
			if (pData && uSize)
			{
				DatabaseMessageProcess(pData, uSize);
				if (m_pCoreServerShell->GetSaveStatus(nIndex) == SAVE_IDLE)
					break;
			}

			if (nCount > 1000)
				break;
#ifdef WIN32
			Sleep(1);
#else
			Sleep(1);
#endif
		}

		m_pCoreServerShell->ClientDisconnect(nIndex,TRUE);          //设置离开游戏状态--断开客户端链接

		if (m_pCoreServerShell->IsCharacterQuiting(nIndex))
		{
			char	szName[32];
	
			m_pCoreServerShell->GetGameData(SGDI_CHARACTER_ACCOUNT, (UINT_PTR)szName, nIndex);
			sLeaveGame.nExtPoint    = m_pCoreServerShell->GetGameData(SGDI_CHARACTER_EXTPOINTCHANGED, nIndex, 0);  //获取要扣除的扩展点
            sLeaveGame.nGameExtTime = m_pCoreServerShell->GetGameData(SGDI_CHARACTER_EXTTIMECHANGED, nIndex, 0);

			  sprintf(sLeaveGame.nClientIp,"127.0.0.1");

			//printf("------退出游戏的IP:%s,总人数:%d\n",nClientInfo.nClientIp,nClientInfo.nNumPlayer);
			strcpy((char *)sLeaveGame.szAccountName, (char *)szName);
			    m_pGatewayClient->SendPackToServer(&sLeaveGame, sizeof(tagLeaveGame)); //56732

			if (m_pTransferClient) //发往bishop
				m_pTransferClient->SendPackToServer(&sLeaveGame, sizeof(tagLeaveGame));

			m_pCoreServerShell->RemoveQuitingPlayer(nIndex);
		} 
		

#ifdef WIN32
			Sleep(1);
#else

			Sleep(1);
#endif */
	}
}
//设置GS是否运行
void KSwordOnLineSever::SetRunningStatus(BOOL bStatus)
{
	m_bIsRunning = bStatus;
}

void KSwordOnLineSever::SendCilentPackData()
{
	if (!inGo || !m_pCoreServerShell || !m_bIsRunning) return;
	
	 MessageLoop();     //网关协议循环

	LONGLONG nMes = 1000;
	if (nMes * m_nGameLoop <= m_Timer.GetElapse()*GAME_FPS) //1秒18帧  时间溢出 
	{	
		if (m_nGameLoop & 0x01)                      //每帧都向所有客户端发包  
			m_pServer->SendPackToClient(-1);  //向所有链接发包

	   int lnID = m_nGameLoop%m_nMaxGameStaus;	  //随机的
	   int nIndex = m_pGameStatus[lnID].nPlayerIndex; //
	   if (nIndex > 0 && nIndex < MAX_PLAYER && m_pGameStatus[lnID].nGameStatus == enumPlayerPlaying)
	   {//在线的就检测
		   if (m_pCoreServerShell && m_pCoreServerShell->IsCharacterLiXian(nIndex)==2)
			   // printf("-------玩家离线中,取消ping \n");
			   return;
		
#define	defMAX_PING_TIMEOUT		30*GAME_FPS
#define	defMAX_PING_INTERVAL	5*GAME_FPS
#define defMAX_CONNECTION_TIMEOUT (15 * GAME_FPS) 

		  if (m_pGameStatus[lnID].nReplyPingTime != 0)	//上次返回的ping时间
		  {//客户端有响应的  
			 if (m_nGameLoop - m_pGameStatus[lnID].nSendPingTime > defMAX_PING_INTERVAL)
			 { 
				PingClient(lnID);
				//printf("Send ping to clienta.... \n");
			 } 
		  }  
		  else if (m_pGameStatus[lnID].nReplyPingTime==0 && m_pGameStatus[lnID].nPingLoopCount<m_pingCount && (m_nGameLoop - m_pGameStatus[lnID].nSendPingTime > defMAX_PING_TIMEOUT))
		  {//接受客户端返回的时间差 客户端没响应的
			  m_pGameStatus[lnID].nPingLoopCount++;
			  printf("Send ping to clientb.... \n");
			  PingClient(lnID);	//索引
		  }
		  else if (m_nGameLoop - m_pGameStatus[lnID].nSendPingTime > defMAX_PING_TIMEOUT)
		  {//超时了
			  printf("no response from client(%u, %u), ShutdownClient it...\n", m_nGameLoop, m_pGameStatus[lnID].nSendPingTime);
             //设置断开链接
			  if (FAILED(m_pServer->ShutdownClient(m_pGameStatus[lnID].nNetidx)))
			  {//关闭失败 就强行删除
				  if (m_pCoreServerShell)
				     m_pCoreServerShell->mandatoryShutDownClient(nIndex);

				  if (m_pGameStatus[lnID].nNetidx>0)
					  SetNetStatus(m_pGameStatus[lnID].nNetidx,enumNetUnconnect,false);         
			  }
		  } 
	   } 
	   else if (m_pGameStatus[lnID].nNetidx >0 && m_pGameStatus[lnID].nNetStatus == enumNetConnected && m_pGameStatus[lnID].nGameStatus == enumPlayerBegin)
	   {
		   // 检查已经连接但是还没有发登录协议的玩家
		   if (m_nGameLoop - m_pGameStatus[lnID].nSendPingTime > defMAX_CONNECTION_TIMEOUT)
		   {
			   printf("Shutdown non-login client[%u] for timeout\n",m_pGameStatus[lnID].nNetidx);
			   //m_pServer->ShutdownClient(m_pGameStatus[lnID].nNetidx);
			   if (FAILED(m_pServer->ShutdownClient(m_pGameStatus[lnID].nNetidx)))
			   {//关闭失败 就强行删除
			   }
			   if (m_pGameStatus[lnID].nNetidx>0)
			      SetNetStatus(m_pGameStatus[lnID].nNetidx,enumNetUnconnect,false);             //设置断开链接
		   }
	   }

	   //if (m_nGameLoop & 0x01)  //每帧都向所有客户端发包  
	   //{ 
		  // m_pServer->SendPackToClient(-1);  //向所有链接发包
	   //} 
	}
}

void KSwordOnLineSever::SetClientConnetNet(int nIsThis)
{
	switch(nIsThis)
	{
	case 9:
		{//npc状态
			if (m_pCoreServerShell)
				m_pCoreServerShell->ExeCoreNpcState();
		}
		break;
	case 1:
		{//网关监控
		  if (m_pGatewayClient)
		  {
			if (FAILED(m_pGatewayClient->FsGameServerConnectTo(m_szGatewayIP,m_nGatewayPort)))
			{  
				cout << " - Gateway server reconnect failed...!" << endl;
			}  
			else
			{ 	
				g_pSOServer.SetClientNet(GATWSTATE,TRUE);
				cout << " - Gateway library server relink OK..!" << endl;
			}
		  }
		}
		break;
	case 2:
		{//数据库监控
		  if (m_pDatabaseClient)
		  {
			if (FAILED(m_pDatabaseClient->FsGameServerConnectTo(m_szDatabaseIP,m_nDatabasePort)))
			{  
				cout << " - Database server reconnect failed...!" << endl;
			}  
			else
			{ 	
				g_pSOServer.SetClientNet(DATASTATE,TRUE);
				cout << " - Database server reconnect OK..!" << endl;
			} 
		  }
		}
		break;
	case 3:
		{
		  if (m_pChatClient)
		  {
		    if (FAILED(m_pChatClient->FsGameServerConnectTo(m_szChatIP,m_nChatPort)))
			{  
				cout << " - Chat server reconnect failed..!" << endl;
			}  
			else
			{ 	
				g_pSOServer.SetClientNet(CHATSTATE,TRUE);
				//------------------------------发送更新S3各种数据
				tagLeaveGame2 lg2;
				lg2.ProtocolFamily = pf_udataserveridx;
				lg2.ProtocolID     = relay_s2c_udateserver;
				lg2.cCmdType       = NORMAL_LEAVEGAME;
				lg2.nSelServer     = MAKELONG(m_GsNumBer, m_SerVerNo);;//m_SerVerNo;
				if (m_pChatClient)
					m_pChatClient->SendPackToServer(&lg2,sizeof(tagLeaveGame2));
				cout << " - Chat server reconnect OK..!" << endl;
			}
		  }
		}
		break;
	case 4:
		{
		  if (m_pTongClient)
		  {
			if (FAILED(m_pTongClient->FsGameServerConnectTo(m_szTongIP,m_nTongPort)))
			{  
				cout << " - Tong server reconnect failed...!" << endl;
			}  
			else
			{ 	
				g_pSOServer.SetClientNet(TONGSTATE,TRUE);
				//------------------------------发送更新S3各种数据
				tagLeaveGame2 lg2;
				lg2.ProtocolFamily = pf_udataserveridx;
				lg2.ProtocolID     = relay_s2c_udateserver;
				lg2.cCmdType       = NORMAL_LEAVEGAME;
				lg2.nSelServer     = MAKELONG(m_GsNumBer, m_SerVerNo);;//m_SerVerNo;
				if (m_pTongClient)
					m_pTongClient->SendPackToServer(&lg2,sizeof(tagLeaveGame2));

				cout << " - Tong server reconnect OK..!" << endl;
			} 
		  }
		}
		break;
	case 5:
		{
		  if (m_pTransferClient)
		  {
			if (FAILED(m_pTransferClient->FsGameServerConnectTo(m_szTransferIP,m_nTransferPort)))
			{  
				cout << " - Transfer server reconnect failed" << endl;
			}  
			else
			{ 	
				g_pSOServer.SetClientNet(TRANSTATE,TRUE);

				//------------------------------发送更新S3各种数据
				tagLeaveGame2 lg2;
				lg2.ProtocolFamily = pf_udataserveridx;
				lg2.ProtocolID     = relay_s2c_udateserver;
				lg2.cCmdType       = NORMAL_LEAVEGAME;
				lg2.nSelServer     = MAKELONG(m_GsNumBer, m_SerVerNo);;//m_SerVerNo;
				if (m_pTransferClient)
					m_pTransferClient->SendPackToServer(&lg2,sizeof(tagLeaveGame2));

				tagUpMapGame2 pUMI;
				memset( &pUMI, 0,sizeof(tagUpMapGame2));
				pUMI.ProtocolFamily = pf_udatasservermaps;
				pUMI.ProtocolID        = relay_s2c_udatemaps;
				pUMI.cMapCount      = 0;
				sprintf(pUMI.szparmName,"%s",m_parmName);

				if (m_pTransferClient) //长度不能大于1024
					m_pTransferClient->SendPackToServer(&pUMI,sizeof(tagUpMapGame2));


				cout << " - Transfer server reconnect OK..!" << endl;
			}
		  }
		}
		break;
	case 6:
		{//任务　或　存档　监控
			if (m_pCoreServerShell)
				m_pCoreServerShell->ExeCoreTask();
         //检测限时物品
			if (m_pCoreServerShell)
				m_pCoreServerShell->CheckItemTime();
		}
		break;
	case 7:
		{//拍卖行 同步
			if (m_pCoreServerShell)
				m_pCoreServerShell->SynCoreItemData();
		}
		break;
	case 8:
		{
	//查询排名
			if (m_mapClientNet[DATASTATE] && m_pDatabaseClient)
			{
					TProcessData ProcessData;
					ProcessData.nProtoId = c2s_gamestatistic;
					ProcessData.nDataLen = 1;
					ProcessData.ulIdentity = -1;
					ProcessData.pDataBuffer[0] = 0;
					m_pDatabaseClient->SendPackToServer(&ProcessData, sizeof(TProcessData));
					printf(">>>>>>>>>>>>>Cap Nhat Danh Sach Xep Hang succeeded!..<<<<<<<<<<<<<\n");	
			}
		}
		break;
	default:
		break;
	}
}

void KSwordOnLineSever::SetS3RelayRunStatus(BOOL bStatus,int nType)
{
	/*if (nType==1)
	   m_ChatState = bStatus;
	else if (nType==2)
	   m_TongState = bStatus;
	else if (nType==3)
	   m_TranState = bStatus;
	else if (nType==4)
	   m_pDataState = bStatus;
	else if (nType==5)
	   m_GatewayState = bStatus;*/
	switch(nType)
	{
	case 1:
		{
	          m_ChatState = bStatus;
		}
		 break;
	case 2:
		 {
			  m_TongState = bStatus;
		 }
		 break;
	case 3:
		 {
			 m_TranState = bStatus;
		 }
		 break;
	case 4:
		 {
			  m_pDataState = bStatus;
		 }
		 break;
	case 5:
		 {
			 m_GatewayState = bStatus;
		 }
		 break;
	default:
		break;
	}
}

BOOL KSwordOnLineSever::ConformAskWay(const void* pData, int nSize, DWORD *lpnNetID)
{
	BOOL	bRet = FALSE;
	if  (NULL==pData||!pData ) return bRet;
	RELAY_ASKWAY_DATA*	pRAD = (RELAY_ASKWAY_DATA *)pData;

	// TODO: 检查是否寻路正确
	int i;
	int nMethod = pRAD->seekMethod;
	switch(nMethod)
	{
	case rm_role_id:
	case rm_account_id:
		{
			
			DWORD	dwNameID;
			DWORD	lnID;
			LPDWORD	lpdwPTR = (DWORD *)(((char *)pData) + sizeof(RELAY_ASKWAY_DATA) + pRAD->wMethodDataLength - sizeof(DWORD)*2);
			dwNameID = *lpdwPTR;
			lnID = *(lpdwPTR + 1); //网络ID

			//if  (nMethod==rm_role_id)
			 //  printf("---ConformAskWay:%d::%u----\n",nMethod,lnID);
			
			i=FindSame(lnID);

			if (i<=0 || i >m_nMaxGameStaus || m_pGameStatus[i].nPlayerIndex <= 0 || m_pGameStatus[i].nPlayerIndex >= MAX_PLAYER)
			{
				bRet = FALSE;
				break;
			}
			DWORD	dwNameIDbyIndex = m_pCoreServerShell->GetGameData(SGDI_CHARACTER_ID, m_pGameStatus[i].nPlayerIndex, 0);
			if (dwNameIDbyIndex != dwNameID)
			{
				bRet = FALSE;
				break;
			}
			*lpnNetID = i;
			bRet = TRUE;
		}
		break;
	case rm_map_id:
		{
			/*DWORD	dwMapID;
			printf("---ConformAskWay:rm_map_id----\n");
			dwMapID = *(DWORD *)(((char *)pData) + sizeof(RELAY_ASKWAY_DATA));

			if (0)	// TODO:check mapid here
			{
				bRet = FALSE;
				break;
			}*/
			//printf("---ConformAskWay:rm_map_id----\n");
			bRet = TRUE;
		} break;

	}
	return bRet;
}
/////////////////////////////////获取本地IP设置////////////////////////////////
int KSwordOnLineSever::GetLocalIpAddress(DWORD *pIntranetAddr, DWORD *pInternetAddr)
{
#ifndef _STANDALONE
	return gGetMacAndIPAddress(NULL, pIntranetAddr, NULL, pInternetAddr);
#else
	char	szIp[16];
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
#endif
}

//////////////////////////////////////////////////////////////////////////////////////////////////
void KSwordOnLineSever::TransferAskWayMessageProcess(const char *pData, size_t dataLength)
{

	if  (NULL==pData||!pData ) return;

	char	szChar[1024]={0};

	EXTEND_HEADER	*pEH = (EXTEND_HEADER *)pData;
	RELAY_ASKWAY_DATA	*pRAD = (RELAY_ASKWAY_DATA *)pData;
	
	RELAY_DATA	*pRD      = (RELAY_DATA *)szChar;
	pRD->ProtocolFamily    = pEH->ProtocolFamily;
	pRD->nToIP                   = pRAD->nFromIP;  
	pRD->nToRelayID         = pRAD->nFromRelayID;
	pRD->nFromIP              = 0;
	pRD->nFromRelayID     = 0;
	// routeDateLength arrives in the ask-way packet and is the length of the body the
	// copies below write behind a RELAY_DATA header into szChar; the TODO this replaces
	// was the check.  One bound, read once, feeds the field, the copies and the sends.
	size_t nRouteLen = pRAD->routeDateLength;
	if (nRouteLen > sizeof(szChar) - sizeof(RELAY_DATA))
		nRouteLen = sizeof(szChar) - sizeof(RELAY_DATA);
	pRD->routeDateLength = (WORD)nRouteLen;
	
	DWORD	lnID;
	if (!ConformAskWay(pRAD, dataLength, &lnID))
	{//转服 失败
		pRD->ProtocolID = relay_s2c_loseway;
		// the lose-way echo hands the whole request back, and the request is data;
		// the room behind the RELAY_DATA header is what it has to fit
		size_t nEchoLen = dataLength;
		if (nEchoLen > sizeof(szChar) - sizeof(RELAY_DATA))
			nEchoLen = sizeof(szChar) - sizeof(RELAY_DATA);
		memcpy(szChar + sizeof(RELAY_DATA), pData, nEchoLen);
		printf("---GS -->S3:relay_s2c_loseway--\n");
		m_pTransferClient->SendPackToServer(pRD, sizeof(RELAY_DATA) + nEchoLen);
	}
	else
	{
		pRD->ProtocolID = relay_c2c_data;
		switch(pRAD->seekMethod)
		{
		case rm_map_id:
			/*pRD->nToIP = mTransSerVerNo;
			pRD->nToRelayID = 0;
			memcpy(szChar + sizeof(RELAY_DATA), pData + sizeof(RELAY_ASKWAY_DATA) + pRAD->wMethodDataLength, pRD->routeDateLength);
			if (m_pTransferClient)
			    m_pTransferClient->SendPackToServer(pRD, sizeof(RELAY_DATA) + pRD->routeDateLength);*/
			printf("---GS -->S3:rm_map_id---\n");
			memcpy(szChar + sizeof(RELAY_DATA), pData + sizeof(RELAY_ASKWAY_DATA) + pRAD->wMethodDataLength, nRouteLen);
			m_pTransferClient->SendPackToServer(pRD, sizeof(RELAY_DATA) + pRD->routeDateLength);
			break;
		case rm_role_id:
		case rm_account_id:
			{//聊天频道

				//printf("---GS -->S3:rm_account_id or rm_role_id %d::%u---\n",pRAD->seekMethod,lnID);
				//break;
				int i=lnID;//FindSame(lnID);

				if (i<=0 || i >m_nMaxGameStaus)
					return;

				if (pf_normal != *(pData + sizeof(RELAY_ASKWAY_DATA) + pRAD->wMethodDataLength))
				{
					//if  (pRAD->seekMethod==rm_role_id)
					//     printf("---GS -->game  rm_role_id %d::%u :%d ---\n",pRAD->seekMethod,lnID,m_pGameStatus[i].nPlayerIndex);

					m_pCoreServerShell->ProcessNewClientMessage(
						m_pTransferClient,
						pRAD->nFromIP,
						pRAD->nFromRelayID,
						m_pGameStatus[i].nPlayerIndex, 
						(pData + sizeof(RELAY_ASKWAY_DATA) + pRAD->wMethodDataLength), 
						pRAD->routeDateLength);
				}
				else
				{
					
					m_pCoreServerShell->ProcessClientMessage(
						m_pGameStatus[i].nPlayerIndex, 
						(pData + sizeof(RELAY_ASKWAY_DATA) + pRAD->wMethodDataLength + 1), 
						pRAD->routeDateLength - 1);
				}
			}
			break;
		default:
			break;
		}
	}
}

void KSwordOnLineSever::TransferLoseWayMessageProcess(const char *pData, size_t dataLength)
{
	if (!m_pCoreServerShell || !m_pServer)
		return;

	if  (NULL==pData||!pData ) 
	{
		 printf("---Received s3 Cross-server messages sent: NULL==pData||!pData Cancel span--\n");
		return;
	}

	EXTEND_HEADER* pInHeader = (EXTEND_HEADER*)pData;

	if (pInHeader->ProtocolFamily == pf_relay)
	{
		tagSearchWay *pSW = NULL;

		switch (pInHeader->ProtocolID)
		{
		case relay_c2c_data:
			pSW = (tagSearchWay *)(pData + sizeof(RELAY_DATA));
			break;
		case relay_c2c_askwaydata:
			pSW = (tagSearchWay *)(pData + sizeof(RELAY_ASKWAY_DATA) + ((RELAY_ASKWAY_DATA*)pData)->wMethodDataLength);
			break;
		default:
			{
			  printf("---Received s3 Cross-server messages sent: pInHeader->ProtocolID Cancel span--\n");
			  return;
			}
		}

		int lnID, nIndex,i;
		
		//lnID = pSW->lnID;		//网络ID

		i = pSW->lnID;//FindSame(lnID);

		if (i<=0 || i >m_nMaxGameStaus)
		{
		   printf("---Received s3 Cross-server messages sent: %d<=0 || %d >m_nMaxGameStaus) Cancel span--\n",i);
		   return;

		}
		if (m_pGameStatus[i].nExchangeStatus != enumExchangeSearchingWay)
		{
			printf("---Received s3 Cross-server messages sent: != enumExchangeSearchingWay Cancel span--\n");
			return;
		}

		if (m_pCoreServerShell->GetGameData(SGDI_CHARACTER_ID, pSW->nIndex, 0) != pSW->dwPlayerID)
		{
			printf("---Received s3 Cross-server messages sent: != pSW->dwPlayerID Cancel span--\n");
			return;
		}

		nIndex = m_pGameStatus[i].nPlayerIndex;
		//提示 此路不通。。
		tagNotifyPlayerExchange	npe;
		npe.cProtocol = s2c_notifyplayerexchange; //跨服 转地图
		memset(&npe.guid, 0, sizeof(GUID));
		// if false, ip to 0
		npe.nIPAddr = 0;//TransDwIp;
		npe.nPort     = 0;//TransPort;
		m_pServer->SendData(m_pGameStatus[i].nNetidx, &npe, sizeof(tagNotifyPlayerExchange));
//恢复跨服前状态
		m_pCoreServerShell->RecoverPlayerExchange(nIndex);
//解锁角色
		tagRoleEnterGame reg;
		reg.ProtocolType = c2s_roleserver_lock;
		reg.bLock = true;
		reg.Lixian = false;                //是否离线的
		reg.nRegServerIndex = m_SerVerNo;  //属于那个大区
		reg.nGsNumber = m_GsNumBer;        //GS索引号

		//m_pCoreServerShell->GetGameData(SGDI_CHARACTER_NAME, (unsigned int)reg.Name, nIndex);			
		m_pCoreServerShell->GetGameDataInfo(SGDI_CHARACTER_NAME,0, nIndex,reg.Name);	
		if (m_pDatabaseClient)
			m_pDatabaseClient->SendPackToServer((const void *)&reg, sizeof(tagRoleEnterGame));
		if (m_pGatewayClient)
			m_pGatewayClient->SendPackToServer((const void *)&reg, sizeof(tagRoleEnterGame));
		m_pGameStatus[i].nExchangeStatus = enumExchangeBegin;
		m_pGameStatus[i].nGameStatus = enumPlayerPlaying;

		printf("---Received s3 Cross-server messages sent: relay_s2c_loseway(%s:%d) Cancel span--\n",reg.Name,m_pGameStatus[i].nNetidx);
	}
}

//GS从客服端获取帮会相关信息发往S3帮会服务器
void KSwordOnLineSever::ProcessPlayerTongMsg(const unsigned long nPlayerIdx, const char* pData, size_t dataLength)
{
	/*bool isRtuen=true;
	if  (isRtuen)
		return;*/
	
	if (nPlayerIdx <= 0 || nPlayerIdx >= MAX_PLAYER)
		return;

	if  (NULL==pData||!pData ) return;

	if (dataLength < sizeof(STONG_PROTOCOL_HEAD))
		return;
	int	nPLength = ((STONG_PROTOCOL_HEAD*)pData)->m_wLength;
	if (nPLength + 1 > dataLength)
		return;
	
	switch (((STONG_PROTOCOL_HEAD*)pData)->m_btMsgId)
	{
	// 发起设置帮会信息
	case enumTONG_COMMAND_ID_APPLY_SET_SEND:
	{
		DWORD dwTongNameID = m_pCoreServerShell->GetGameData(SGDI_TONG_GET_TONG_NAMEID, 0, nPlayerIdx);
		if (dwTongNameID == 0)
			break;

		TONG_APPLY_SEND_SETINFO	*pSend = (TONG_APPLY_SEND_SETINFO*)pData;
		if (!pSend)
			break;

		STONG_SERVER_TO_CORE_CHECK_SET sCheckSet;
		sCheckSet.ProtocolType = pSend->NextProtocolType;
		sCheckSet.m_inParam = pSend->m_inParam;
		strcpy(sCheckSet.m_strParam1, pSend->m_strParam1);
		strcpy(sCheckSet.m_strParam2, pSend->m_strParam2);

		int	nRet = FALSE;
		if (m_pCoreServerShell)
			nRet = m_pCoreServerShell->GetGameDataInfo(SGDI_TONG_CHECK_SET_POWER, 0, nPlayerIdx, 0, (void*)&sCheckSet);
		if (!nRet)
			break;

		STONG_SET_TONG_INFO_COMMAND setTong;
		setTong.ProtocolFamily       = pf_alltong;
		setTong.ProtocolID           = enumC2S_TONG_INFO_SET;
		setTong.NextProtocolID	     = pSend->NextProtocolType;
		setTong.m_dwTongNameID       = dwTongNameID;
		setTong.m_dwParam            = pSend->m_inParam;
		snprintf(setTong.m_szName, sizeof(setTong.m_szName), "%s", pSend->m_strParam1);
		snprintf(setTong.m_title, sizeof(setTong.m_title), "%s", pSend->m_strParam2);

		//printf("DEBUG, setTong.m_dwParam: %d\n", setTong.m_dwParam);
		//printf("DEBUG, setTong.m_szName: %s\n", setTong.m_szName);
		//printf("DEBUG, setTong.m_title: %s\n", setTong.m_title);
		
		if (m_pTongClient)
			m_pTongClient->SendPackToServer((const void*)&setTong,sizeof(STONG_SET_TONG_INFO_COMMAND));
		
		printf("---------发送设置帮会信息成功!----------- \n");
	}
	break;
	case enumTONG_COMMAND_ID_APPLY_ATTACK_SEND:
		{   //发宣战
			//检测是否在宣战时间内
			if (m_pCoreServerShell->GetGoldMissidVal(118)==0)
			{//等于0 就检测
				m_pCoreServerShell->SetAttackMsg(NULL,4);
				break;
			}
			//开始发送宣战信息
			TONG_APPLY_SEND_ATTACK	*pSend = (TONG_APPLY_SEND_ATTACK*)pData;
			STONG_GET_TONG_ATTACK_INFO_COMMAND	saTong;	
			//szPlayerName[0] = 0;
			//m_pCoreServerShell->GetGameData(SGDI_CHARACTER_NAME, (unsigned int)szPlayerName, sAdd.m_nTargetIdx);//获取申请人的名字
			saTong.ProtocolFamily       = pf_alltong;
			saTong.ProtocolID           = enumC2S_TONG_ATTACK_SEND_INFO;  //
			saTong.m_dwTongNameID       = pSend->m_dwYTongNameID;         // 
			saTong.m_dwNpcID            = pSend->m_NpcDwid;               //发送方的dwid 攻方
			saTong.m_dwParam            = pSend->m_dwDTongNameID;;		  //目标方的帮会 守方
			//saTong.m_wLength = sizeof(STONG_GET_TONG_ATTACK_INFO_COMMAND);
			if (m_pTongClient)
				m_pTongClient->SendPackToServer((const void*)&saTong,sizeof(STONG_GET_TONG_ATTACK_INFO_COMMAND));
			printf("---------发送发起帮会宣战成功!----------- \n");

		}
		break;
	// 获取宣战数据
	case enumTONG_COMMAND_ID_APPLY_ATTACK_GET:
		{
		  TONG_APPLY_GET_ATTACK	*pApply = (TONG_APPLY_GET_ATTACK*)pData;
		  //char	szPlayerName[32];
		  STONG_GET_TONG_ATTACK_INFO_COMMAND	saTong;

		  //szPlayerName[0] = 0;
		  //m_pCoreServerShell->GetGameData(SGDI_CHARACTER_NAME, (unsigned int)szPlayerName, sAdd.m_nTargetIdx);//获取申请人的名字
		  saTong.ProtocolFamily       = pf_alltong;
		  saTong.ProtocolID           = enumC2S_TONG_ATTACK_GET_INFO;  //
		  saTong.m_dwTongNameID       = pApply->m_dwTongNameID;        // 
		  saTong.m_dwNpcID            = pApply->m_NpcDwid;             //
		  saTong.m_dwParam            = 0;
		  //saTong.m_wLength = sizeof(STONG_GET_TONG_ATTACK_INFO_COMMAND);
		  //if (m_pTongClient)
	      //    m_pTongClient->SendPackToServer((const void*)&saTong,sizeof(STONG_GET_TONG_ATTACK_INFO_COMMAND));
		  //	printf("---------发送获取帮会宣战信息成功!-----------\n");
		}
		break;
	case enumTONG_COMMAND_ID_APPLY_CREATE:
		{//创建帮会
			TONG_APPLY_CREATE_COMMAND	*pApply = (TONG_APPLY_CREATE_COMMAND*)pData;

			STONG_SERVER_TO_CORE_APPLY_CREATE	sApply;
			sApply.m_nCamp = pApply->m_btCamp;
			sApply.m_nPlayerIdx = nPlayerIdx;
			memcpy(sApply.m_szTongName, pApply->m_szName, sizeof(pApply->m_szName));

			// 创建帮会条件检测
			int nRet = 0xff;
			if (m_pCoreServerShell)
				nRet = m_pCoreServerShell->GetGameDataInfo(SGDI_TONG_APPLY_CREATE, 0, 0, NULL,(void*)&sApply); // check dieu kien tao bang
			
			// 创建帮会条件成立
			if (nRet == 0)
			{
				char	szPlayerName[NAME_LENGHT];
				STONG_CREATE_COMMAND	sCreate;
               //获取创建者名字
				//m_pCoreServerShell->GetGameData(SGDI_CHARACTER_NAME, (unsigned int)szPlayerName, nPlayerIdx);
				m_pCoreServerShell->GetGameDataInfo(SGDI_CHARACTER_NAME, 0, nPlayerIdx,szPlayerName);
				sCreate.ProtocolFamily       = pf_tong;
				sCreate.ProtocolID           = enumC2S_TONG_CREATE;
				sCreate.m_btCamp             = pApply->m_btCamp;
				sCreate.m_dwParam            = nPlayerIdx;
				sCreate.m_blevel             = sApply.m_nLevel;
				//printf("DEBUG, sCreate.m_blevel: %d\n", sCreate.m_blevel);
				sCreate.m_dwPlayerNameID     = g_FileName2Id(szPlayerName);
				sCreate.m_btPlayerNameLength = strlen(szPlayerName);
				sCreate.m_btTongNameLength   = strlen(sApply.m_szTongName);
				// the two names share one 64-byte payload: what has to fit is their sum, not
				// each half against 64, or the second copy starts where the first already ended
				int nPlayerNameLen = sCreate.m_btPlayerNameLength;
				int nTongNameLen = sCreate.m_btTongNameLength;
				if (nPlayerNameLen > NAME_LENGHT - 2)
					nPlayerNameLen = NAME_LENGHT - 2;
				if (nTongNameLen > NAME_LENGHT - 1 - nPlayerNameLen)
					nTongNameLen = NAME_LENGHT - 1 - nPlayerNameLen;
				sCreate.m_btPlayerNameLength = nPlayerNameLen;
				sCreate.m_btTongNameLength = nTongNameLen;
				memcpy(sCreate.m_szBuffer, szPlayerName, nPlayerNameLen);
				memcpy(&sCreate.m_szBuffer[nPlayerNameLen], sApply.m_szTongName, nTongNameLen);
				sCreate.m_wLength = sizeof(STONG_CREATE_COMMAND) - sizeof(sCreate.m_szBuffer) + sCreate.m_btTongNameLength + sCreate.m_btPlayerNameLength;
				if (m_pTongClient)//向S3帮会服务器发包
					m_pTongClient->SendPackToServer((const void*)&sCreate, sCreate.m_wLength);
				printf(">>>>>>>>>>(%s)发送S3relay服务器创建帮会(%s)消息，OK<<<<<<<<<<...\n",szPlayerName, sApply.m_szTongName);
				break;
			}
			// 创建帮会条件不成立
			else
			{
				int		nNetID;
				TONG_CREATE_FAIL_SYNC	sFail;
				sFail.ProtocolType = s2c_extendtong;
				sFail.m_btMsgId = enumTONG_SYNC_ID_CREATE_FAIL;
				sFail.m_btFailId = nRet;
				sFail.m_wLength = sizeof(sFail) - 1;
				nNetID = m_pCoreServerShell->GetGameData(SGDI_CHARACTER_NETID, nPlayerIdx, 0);
				if (m_pServer)
					m_pServer->PackDataToClient(nNetID, &sFail, sFail.m_wLength + 1);
                printf(">>>>>>>>>>(%d)发送客户端 创建帮会(%s):%d 失败，OK<<<<<<<<<<...\n",sApply.m_nPlayerIdx, sApply.m_szTongName,nRet);
				break;
			}
		}
		break;
	case enumTONG_COMMAND_ID_APPLY_ADD:
		{//申请加入帮派
			TONG_APPLY_ADD_COMMAND	*pApply = (TONG_APPLY_ADD_COMMAND*)pData;
			if (pApply->m_wLength != sizeof(TONG_APPLY_ADD_COMMAND) - 1)
				break;
			STONG_SERVER_TO_CORE_APPLY_ADD	sAdd;
			sAdd.m_nPlayerIdx = nPlayerIdx; // id nguoi xin vao bang
			sAdd.m_dwNpcID    = pApply->m_dwNpcID; // id nguoi dc xin
			if (m_pCoreServerShell)//获取加入帮会的消息
				m_pCoreServerShell->GetGameDataInfo(SGDI_TONG_APPLY_ADD, 0, 0,0,&sAdd);
		}
		break;
	case enumTONG_COMMAND_ID_ACCEPT_ADD:
		{//同意或拒绝 加入帮会
			TONG_ACCEPT_MEMBER_COMMAND	*pAccept = (TONG_ACCEPT_MEMBER_COMMAND*)pData;
			if (pAccept->m_wLength != sizeof(TONG_ACCEPT_MEMBER_COMMAND) - 1)
				break;
			
			if (pAccept->m_nPlayerIdx  <= 0 || pAccept->m_nPlayerIdx  >= MAX_PLAYER) 
				break;

			char	szPlayerName[NAME_LENGHT];
			szPlayerName[0] = 0;
			//m_pCoreServerShell->GetGameData(SGDI_CHARACTER_NAME, (unsigned int)szPlayerName, pAccept->m_nPlayerIdx);//获取申请人的名字
			m_pCoreServerShell->GetGameDataInfo(SGDI_CHARACTER_NAME, 0, pAccept->m_nPlayerIdx,szPlayerName);//获取申请人的名字
			if (pAccept->m_btFlag == 0)
			{//按钮点击拒绝
				STONG_SERVER_TO_CORE_REFUSE_ADD	sRefuse;
				sRefuse.m_nSelfIdx   = nPlayerIdx;             //被申请人的Playerid
				sRefuse.m_nTargetIdx = pAccept->m_nPlayerIdx;  //申请人的playerid
				sRefuse.m_dwNameID   = g_FileName2Id(szPlayerName);//pAccept->m_dwNameID;    //申请人的名字ID
				m_pCoreServerShell->OperationRequestInfo(SSOI_TONG_REFUSE_ADD, 0, 0,0,&sRefuse);
				break;
			}
			else
			{//同意
				char	szTongName[NAME_LENGHT];
				STONG_SERVER_TO_CORE_CHECK_ADD_CONDITION sAdd;
				sAdd.m_nSelfIdx   = nPlayerIdx;              //被申请人的id
				sAdd.m_nTargetIdx = pAccept->m_nPlayerIdx;   //申请人的Playerid

				sAdd.m_dwNameID   = g_FileName2Id(szPlayerName);//pAccept->m_dwNameID;     //申请人的名字ID

				int	nRet = FALSE;
				if (m_pCoreServerShell)//检测条件
					//nRet = m_pCoreServerShell->GetGameData(SGDI_TONG_CHECK_ADD_CONDITION, (unsigned int)szTongName, (unsigned int)&sAdd);
				    nRet = m_pCoreServerShell->GetGameDataInfo(SGDI_TONG_CHECK_ADD_CONDITION,0,0,szTongName,&sAdd);
					// 向 relay 申请添加帮众
				if (nRet)
				{
					STONG_ADD_MEMBER_COMMAND	sTong;                        
					sTong.ProtocolFamily       = pf_tong;
					sTong.ProtocolID           = enumC2S_TONG_ADD_MEMBER;  //加入帮会
					sTong.m_dwParam            = sAdd.m_nTargetIdx;        //申请人的Playerid  
					sTong.m_dwPlayerNameID     = sAdd.m_dwNameID;          //申请人的名字ID
					sTong.m_nLevel             = sAdd.m_nLevel;
					sTong.m_gender			   = m_pCoreServerShell->GetGameDataInfo(SGDI_CHARACTER_SEX, 0, pAccept->m_nPlayerIdx, 0);
					sTong.m_btPlayerNameLength = strlen(szPlayerName);     //申请人的名字ID
					sTong.m_btTongNameLength   = strlen(szTongName);       //要加入帮会的名
					// same payload, same pair: the tong name goes first, the player name after it
					int nTongNameLen = sTong.m_btTongNameLength;
					int nPlayerNameLen = sTong.m_btPlayerNameLength;
					if (nTongNameLen > NAME_LENGHT - 2)
						nTongNameLen = NAME_LENGHT - 2;
					if (nPlayerNameLen > NAME_LENGHT - 1 - nTongNameLen)
						nPlayerNameLen = NAME_LENGHT - 1 - nTongNameLen;
					sTong.m_btTongNameLength = nTongNameLen;
					sTong.m_btPlayerNameLength = nPlayerNameLen;
					memcpy(sTong.m_szBuffer, szTongName, nTongNameLen);
					memcpy(&sTong.m_szBuffer[nTongNameLen], szPlayerName, nPlayerNameLen);
					sTong.m_wLength = sizeof(STONG_ADD_MEMBER_COMMAND) - sizeof(sTong.m_szBuffer) + sTong.m_btTongNameLength + sTong.m_btPlayerNameLength;
					if (m_pTongClient)
						m_pTongClient->SendPackToServer((const void*)&sTong, sTong.m_wLength);
				}
				//else
				//{//条件不成立 发送拒绝信息
				//	STONG_SERVER_TO_CORE_REFUSE_ADD	sRefuse;
				//	sRefuse.m_nSelfIdx   = nPlayerIdx;             //被申请人的Playerid
				//	sRefuse.m_nTargetIdx = pAccept->m_nPlayerIdx;  //申请人的playerid
				//	sRefuse.m_dwNameID   = g_FileName2Id(szPlayerName);;//pAccept->m_dwNameID;    //申请人的名字ID
				//    m_pCoreServerShell->OperationRequestInfo(SSOI_TONG_REFUSE_ADD, 0, 0,0,&sRefuse);
				//	break;
				//}
			}
		}
		break;
	case enumTONG_COMMAND_ID_APPLY_INFO:
		{//获取帮会信息
			TONG_APPLY_INFO_COMMAND	*pInfo = (TONG_APPLY_INFO_COMMAND*)pData;
			if (pInfo->m_wLength < sizeof(TONG_APPLY_INFO_COMMAND) - 1 - sizeof(pInfo->m_szBuf))
				break;

			STONG_SERVER_TO_CORE_GET_INFO	sGet;
			sGet.m_nSelfIdx = nPlayerIdx;
			sGet.m_nInfoID = pInfo->m_btInfoID;
			sGet.m_nParam1 = pInfo->m_nParam1;
			sGet.m_nParam2 = pInfo->m_nParam2;
			sGet.m_nParam3 = pInfo->m_nParam3;
			snprintf(sGet.m_szName, sizeof(sGet.m_szName), "%s", pInfo->m_szBuf);

			switch (pInfo->m_btInfoID)
			{
			case enumTONG_APPLY_INFO_ID_POWER:
			{
				int nRet = 0;
				if (m_pCoreServerShell)
					nRet = m_pCoreServerShell->GetGameDataInfo(SGDI_TONG_GET_INFO, 0, 0, 0, &sGet);
				if (nRet == 0)
					break;

				STONG_GET_POWER_COMMAND sGetPower;
				sGetPower.ProtocolFamily = pf_tong;
				sGetPower.ProtocolID = enumC2S_TONG_GET_POWER;
				sGetPower.m_nPlayerIndex = nPlayerIdx;
				sGetPower.m_dwTongNameID = pInfo->m_nParam1;
				snprintf(sGetPower.m_szName, sizeof(sGetPower.m_szName), "%s", pInfo->m_szBuf);

				//printf("%s\n", sGetPower.m_szName);

				if (m_pTongClient)//S3
					m_pTongClient->SendPackToServer((const void*)&sGetPower, sizeof(sGetPower));
			}
			break;
			case enumTONG_APPLY_INFO_ID_SEARCH:
			{
				int nRet = 0;
				if (m_pCoreServerShell)
					nRet = m_pCoreServerShell->GetGameDataInfo(SGDI_TONG_GET_INFO, 0, 0, 0, &sGet);
				if (nRet == 0)
					break;

				STONG_SEARCH_INFO_COMMAND sSearch;
				sSearch.ProtocolFamily = pf_tong;
				sSearch.ProtocolID = enumC2S_TONG_SEARCH_INFO;
				sSearch.m_nPlayerIndex = nPlayerIdx;
				sSearch.m_dwTongNameID = pInfo->m_nParam1;
				sSearch.m_nStart = pInfo->m_nParam2;
				sSearch.m_nNum = pInfo->m_nParam3;
				sSearch.m_nType = pInfo->m_nParam4;

				//printf("DEBUG, sSearch.m_nPlayerIndex: %d\n", sSearch.m_nPlayerIndex);
				//printf("DEBUG, sSearch.m_dwTongNameID: %d\n", sSearch.m_dwTongNameID);
				//printf("DEBUG, sSearch.m_nStart: %d\n", sSearch.m_nStart);
				//printf("DEBUG, sSearch.m_nNum: %d\n", sSearch.m_nNum);
				//printf("DEBUG, sSearch.m_nType: %d\n", sSearch.m_nType);

				if (m_pTongClient)//S3
					m_pTongClient->SendPackToServer((const void*)&sSearch, sizeof(sSearch));
			}
			break;
			case enumTONG_APPLY_INFO_ID_SELF: //自己查询自己帮会的信息
				//sGet.m_nSelfIdx = nPlayerIdx;
				//sGet.m_nInfoID = pInfo->m_btInfoID;
				//sGet.m_nParam1 = pInfo->m_nParam1;
				//sGet.m_nParam2 = pInfo->m_nParam2;
				//sGet.m_nParam3 = pInfo->m_nParam3;
				//sGet.m_szName[0] = 0;
				if (m_pCoreServerShell)
					m_pCoreServerShell->GetGameDataInfo(SGDI_TONG_GET_INFO,0, 0,0,&sGet);
				break;
			case enumTONG_APPLY_INFO_ID_MASTER:
				break;//帮主
			case enumTONG_APPLY_INFO_ID_DIRECTOR:
				break;//长老
			case enumTONG_APPLY_INFO_ID_MANAGER:
				{//队长
					int nRet = 0;
					if (m_pCoreServerShell)
						nRet = m_pCoreServerShell->GetGameDataInfo(SGDI_TONG_GET_INFO, 0, 0,0,&sGet);
					if (nRet == 0)
						break;

					STONG_GET_MANAGER_INFO_COMMAND	sGetManInfo;
					sGetManInfo.ProtocolFamily = pf_tong;
					sGetManInfo.ProtocolID = enumC2S_TONG_GET_MANAGER_INFO;
					sGetManInfo.m_dwParam = nPlayerIdx;
					sGetManInfo.m_nParam1 = pInfo->m_nParam1;
					sGetManInfo.m_nParam2 = pInfo->m_nParam2;
					sGetManInfo.m_nParam3 = pInfo->m_nParam3;
					if (m_pTongClient)//S3
						m_pTongClient->SendPackToServer((const void*)&sGetManInfo, sizeof(sGetManInfo));
				}
				break;
			case enumTONG_APPLY_INFO_ID_MEMBER:
				{//帮众
					int nRet = 0;
					//sGet.m_nSelfIdx = nPlayerIdx;
					//sGet.m_nInfoID = pInfo->m_btInfoID;
					//sGet.m_nParam1 = pInfo->m_nParam1;	  //tongid
					//sGet.m_nParam2 = pInfo->m_nParam2;	  //获取数据的开始位置
					//sGet.m_nParam3 = pInfo->m_nParam3;	  //每次获取的数量
					//sGet.m_szName[0] = 0;
					if (m_pCoreServerShell)
						nRet = m_pCoreServerShell->GetGameDataInfo(SGDI_TONG_GET_INFO, 0, 0,0,&sGet);
					if (nRet == 0)
						break;

					STONG_GET_MEMBER_INFO_COMMAND	sGetMemInfo;
					sGetMemInfo.ProtocolFamily = pf_tong;
					sGetMemInfo.ProtocolID = enumC2S_TONG_GET_MEMBER_INFO;
					sGetMemInfo.m_dwParam = nPlayerIdx;
					sGetMemInfo.m_nParam1 = pInfo->m_nParam1; // tong id
					sGetMemInfo.m_nParam2 = pInfo->m_nParam2; // start
					sGetMemInfo.m_nParam3 = pInfo->m_nParam3; // end

					if (m_pTongClient)//S3
						m_pTongClient->SendPackToServer((const void*)&sGetMemInfo, sizeof(sGetMemInfo));
				}
				break;
			case enumTONG_APPLY_INFO_ID_LIST: // lay danh sach bang hoi
				{//获取所有帮会列表
					/*int nRet = 0;
					sGet.m_nSelfIdx = nPlayerIdx;
					sGet.m_nInfoID = pInfo->m_btInfoID;
					sGet.m_nParam1 = pInfo->m_nParam1;	  //tongid
					sGet.m_nParam2 = pInfo->m_nParam2;	  //获取数据的开始位置
					sGet.m_nParam3 = pInfo->m_nParam3;	  //每次获取的数量
					sGet.m_szName[0] = 0;
					if (m_pCoreServerShell)
						nRet = m_pCoreServerShell->GetGameData(SGDI_TONG_GET_INFO, (UINT_PTR)&sGet, 0);
					if (nRet == 0)
						break;*/
					STONG_GET_LIST_INFO_COMMAND	sGetList;
					sGetList.ProtocolFamily = pf_alltong;
					sGetList.ProtocolID = enumC2S_TONG_GET_LIST_INFO;
					sGetList.m_dwParam = nPlayerIdx;
					sGetList.m_nParam1 = pInfo->m_nParam1;	  //tongid
					sGetList.m_nParam2 = pInfo->m_nParam2;	  //获取数据的开始位置
					sGetList.m_nParam3 = pInfo->m_nParam3;	  //每次获取的数量
					if (m_pTongClient)//S3
						m_pTongClient->SendPackToServer((const void*)&sGetList, sizeof(sGetList));
					//printf("---------lay danh sach bang hoi !-----------m_nParam: %d-%d-%d\n",pInfo->m_nParam1,pInfo->m_nParam2,pInfo->m_nParam3);
				}
				break;
			case enumTONG_APPLY_INFO_ID_ONE:
				break;
			case enumTONG_APPLY_INFO_ID_TONG_HEAD:
				{//客户端传过来的数据
					DWORD	dwTongNameID = 0;

					//sGet.m_nSelfIdx = nPlayerIdx;
					//sGet.m_nInfoID = pInfo->m_btInfoID;
					//sGet.m_nParam1 = pInfo->m_nParam1;
					//sGet.m_nParam2 = pInfo->m_nParam2;
					//sGet.m_nParam3 = pInfo->m_nParam3;
					//sGet.m_szName[0] = 0;

					if (sGet.m_nParam1 != 0) // lay thong tin bang khac
					{
						dwTongNameID = g_FileName2Id(pInfo->m_szBuf);
					}
					else // lay thong tin bang cua sender
					{
						if (m_pCoreServerShell)
							dwTongNameID = m_pCoreServerShell->GetGameDataInfo(SGDI_TONG_GET_INFO, 0, 0, 0, &sGet);
					}

					if (dwTongNameID == 0)
						break;
					//将客户端的请求发往S3帮会服务器 ---获取帮会帮主和长老的信息
					STONG_GET_TONG_HEAD_INFO_COMMAND	sGetTongHeadInfo;
					sGetTongHeadInfo.ProtocolFamily = pf_tong;
					sGetTongHeadInfo.ProtocolID = enumC2S_TONG_GET_HEAD_INFO;
					sGetTongHeadInfo.m_dwParam = nPlayerIdx;
					//sGetTongHeadInfo.m_dwNpcID = pInfo->m_nParam1;
					sGetTongHeadInfo.m_dwTongNameID = dwTongNameID;
					if (m_pTongClient)//S3
						m_pTongClient->SendPackToServer((const void*)&sGetTongHeadInfo, sizeof(sGetTongHeadInfo));
				}
				break;
			}
		}
		break;
	case enumTONG_COMMAND_ID_APPLY_INSTATE:
		{//任免职位
			TONG_APPLY_INSTATE_COMMAND	*pApply = (TONG_APPLY_INSTATE_COMMAND*)pData;
			if (pApply->m_wLength + 1 != sizeof(TONG_APPLY_INSTATE_COMMAND))
				break;

			int nRet = 0;
			if (m_pCoreServerShell)
				nRet = m_pCoreServerShell->GetGameDataInfo(SGDI_TONG_INSTATE_POWER, 0, nPlayerIdx,0,pApply);
			if (nRet == 0)
				break;

			STONG_INSTATE_COMMAND	sInstate;
			sInstate.ProtocolFamily	= pf_tong;
			sInstate.ProtocolID		= enumC2S_TONG_INSTATE;
			sInstate.m_btCurFigure	= pApply->m_btCurFigure;
			sInstate.m_btCurPos		= pApply->m_btCurPos;
			sInstate.m_btNewFigure	= pApply->m_btNewFigure;
			sInstate.m_btNewPos		= pApply->m_btNewPos;
			sInstate.m_dwParam		= nPlayerIdx;
			sInstate.m_nGender = m_pCoreServerShell->GetGameDataInfo(SGDI_CHARACTER_SEX, 0, 0, pApply->m_szName);
			sInstate.m_dwTongNameID	= pApply->m_dwTongNameID;
			snprintf(sInstate.m_szName, sizeof(sInstate.m_szName), "%s", pApply->m_szName);

			//printf("DEBUG, sInstate.m_btCurFigure: %d\n", sInstate.m_btCurFigure);
			//printf("DEBUG, sInstate.m_btCurPos: %d\n", sInstate.m_btCurPos);
			//printf("DEBUG, sInstate.m_btNewFigure: %d\n", sInstate.m_btNewFigure);
			//printf("DEBUG, sInstate.m_btNewPos: %d\n", sInstate.m_btNewPos);
			//printf("DEBUG, sInstate.m_dwParam: %d\n", sInstate.m_dwParam);
			//printf("DEBUG, sInstate.m_nGender: %d\n", sInstate.m_nGender);
			//printf("DEBUG, sInstate.m_dwTongNameID: %d\n", sInstate.m_dwTongNameID);
			//printf("DEBUG, sInstate.m_szName: %s\n", sInstate.m_szName);

			if (m_pTongClient)
				m_pTongClient->SendPackToServer((const void*)&sInstate, sizeof(sInstate));
		}
		break;

	case enumTONG_COMMAND_ID_APPLY_KICK:
		{//踢帮会出
			TONG_APPLY_KICK_COMMAND	*pKick = (TONG_APPLY_KICK_COMMAND*)pData;
			if (pKick->m_wLength + 1 != sizeof(TONG_APPLY_KICK_COMMAND))
				break;

			int nRet = 0;
			if (m_pCoreServerShell)
				nRet = m_pCoreServerShell->GetGameDataInfo(SGDI_TONG_KICK_POWER, 0, nPlayerIdx, 0, pKick);
			if (nRet == 0)
				break;

			STONG_KICK_COMMAND	sKick;
			sKick.ProtocolFamily	= pf_tong;
			sKick.ProtocolID		= enumC2S_TONG_KICK;
			sKick.m_dwParam			= nPlayerIdx;
			sKick.m_dwTongNameID	= pKick->m_dwTongNameID;
			sKick.m_btFigure		= pKick->m_btFigure;
			sKick.m_btPos			= pKick->m_btPos;
			memcpy(sKick.m_szName, pKick->m_szName, sizeof(pKick->m_szName));

			if (m_pTongClient)
				m_pTongClient->SendPackToServer((const void*)&sKick, sizeof(sKick));
		}
		break;
	case enumTONG_COMMAND_ID_APPLY_LEAVE:
		{//离开帮会
			TONG_APPLY_LEAVE_COMMAND	*pLeave = (TONG_APPLY_LEAVE_COMMAND*)pData;
			if (pLeave->m_wLength + 1 != sizeof(TONG_APPLY_LEAVE_COMMAND))
				break;
			int nRet = 0;
			if (m_pCoreServerShell)
				nRet = m_pCoreServerShell->GetGameDataInfo(SGDI_TONG_LEAVE_POWER, 0, nPlayerIdx,0,pLeave);
			if (nRet == 0)
				break;

			STONG_LEAVE_COMMAND	sLeave;
			sLeave.ProtocolFamily	= pf_tong;
			sLeave.ProtocolID		= enumC2S_TONG_LEAVE;
			sLeave.m_dwParam		= nPlayerIdx;
			sLeave.m_dwTongNameID	= pLeave->m_dwTongNameID;
			sLeave.m_btFigure		= pLeave->m_btFigure;
			sLeave.m_btPos			= pLeave->m_btPos;
			memcpy(sLeave.m_szName, pLeave->m_szName, sizeof(pLeave->m_szName));

			if (m_pTongClient)
				m_pTongClient->SendPackToServer((const void*)&sLeave, sizeof(sLeave));
		}
		break;
	case enumTONG_COMMAND_ID_APPLY_CHANGE_MASTER:
		{//传位
			TONG_APPLY_CHANGE_MASTER_COMMAND	*pChange = (TONG_APPLY_CHANGE_MASTER_COMMAND*)pData;
			if (pChange->m_wLength + 1 != sizeof(TONG_APPLY_CHANGE_MASTER_COMMAND))
				break;

			int nRet = 0;
			if (m_pCoreServerShell)
				nRet = m_pCoreServerShell->GetGameDataInfo(SGDI_TONG_CHANGE_MASTER_POWER, 0, nPlayerIdx,0,pChange);
			if (nRet == 0)
				break;

			//STONG_CHANGE_MASTER_COMMAND	sChange;
			//sChange.ProtocolFamily	= pf_tong;
			//sChange.ProtocolID		= enumC2S_TONG_CHANGE_MASTER;
			//sChange.m_btFigure		= pChange->m_btFigure;
			//sChange.m_btPos			= pChange->m_btPos;
			//sChange.m_dwParam		= nPlayerIdx;
			//sChange.m_dwTongNameID	= pChange->m_dwTongNameID;
			//memcpy(sChange.m_szName, pChange->m_szName, sizeof(pChange->m_szName));
			//
			//if (m_pTongClient)
			//	m_pTongClient->SendPackToServer((const void*)&sChange, sizeof(sChange));

			STONG_ACCEPT_MASTER_COMMAND	sAccept;

			sAccept.ProtocolFamily = pf_tong;
			sAccept.ProtocolID = enumC2S_TONG_ACCEPT_MASTER;
			sAccept.m_dwParam = nPlayerIdx;
			sAccept.m_dwTongNameID = pChange->m_dwTongNameID;
			sAccept.m_btFigure = pChange->m_btFigure;
			sAccept.m_btPos = pChange->m_btPos;

			if (nRet)
				sAccept.m_btAcceptFalg = 1;
			else
				sAccept.m_btAcceptFalg = 0;
			memcpy(sAccept.m_szName, pChange->m_szName, sizeof(pChange->m_szName));

			if (m_pTongClient)
				m_pTongClient->SendPackToServer((const void*)&sAccept, sizeof(sAccept));
		}
		break;
	case enumTONG_COMMAND_ID_APPLY_SAVE:
	case enumTONG_COMMAND_ID_APPLY_GET:
	case enumTONG_COMMAND_ID_APPLY_SND:
	{
		TONG_APPLY_SAVE_COMMAND* pSave = (TONG_APPLY_SAVE_COMMAND*)pData;
		if (pSave->m_wLength + 1 != sizeof(TONG_APPLY_SAVE_COMMAND))
			break;

		DWORD	dwTongNameID = m_pCoreServerShell->GetGameData(SGDI_TONG_GET_TONG_NAMEID, 0, nPlayerIdx);
		if (dwTongNameID == 0)
			break;

		//printf("DEBUG, pSave->m_szName: %s\n", pSave->m_szName);
		//printf("DEBUG, pSave->m_dwTongNameID: %d\n", pSave->m_dwTongNameID);
		//printf("DEBUG, pSave->m_dwMoney: %d\n", pSave->m_dwMoney);

		STONG_MONEY_COMMAND	sMoney;
		sMoney.ProtocolFamily = pf_tong;
		if (pSave->m_btMsgId == enumTONG_COMMAND_ID_APPLY_SAVE)
		{
			int nRet = 0;
			if (m_pCoreServerShell)
				nRet = m_pCoreServerShell->GetGameDataInfo(SGDI_TONG_SAVE_MONEY_POWER, 0, nPlayerIdx, 0, pSave);
			if (nRet == 0)
				break;

			sMoney.ProtocolID = enumC2S_TONG_MONEY_SAVE;
		}
		else if (pSave->m_btMsgId == enumTONG_COMMAND_ID_APPLY_GET)
		{
			int nRet = 0;
			if (m_pCoreServerShell)
				nRet = m_pCoreServerShell->GetGameDataInfo(SGDI_TONG_MONEY_POWER, 0, nPlayerIdx, 0, pSave);
			if (nRet == 0)
				break;

			sMoney.ProtocolID = enumC2S_TONG_MONEY_GET;
		}
		else
		{
			sMoney.ProtocolID = enumC2S_TONG_MONEY_SND;
		}

		sMoney.m_dwMoney = pSave->m_dwMoney;
		sMoney.m_dwParam = nPlayerIdx;
		sMoney.m_dwTongNameID = pSave->m_dwTongNameID;
		memcpy(sMoney.m_szName, pSave->m_szName, sizeof(pSave->m_szName));
		if (m_pTongClient)
			m_pTongClient->SendPackToServer((const void*)&sMoney, sizeof(sMoney));
	}
		break;
	case enumTONG_COMMAND_ID_APPLY_INVITE:
	{
		TONG_APPLY_INVITE_COMMAND* pApply = (TONG_APPLY_INVITE_COMMAND*)pData;
		if (pApply->m_wLength != sizeof(TONG_APPLY_INVITE_COMMAND) - 1)
			break;

		DWORD	dwTongNameID = 0;
		dwTongNameID = m_pCoreServerShell->GetGameData(SGDI_TONG_GET_TONG_NAMEID, 0, nPlayerIdx);
		if (dwTongNameID == 0) // nguoi moi ko nam trong bang hoi nao
			break;

		STONG_SERVER_TO_CORE_APPLY_INVITE	sInvite;
		sInvite.m_nPlayerIdx = nPlayerIdx;
		sInvite.m_dwNpcID = pApply->m_dwNpcID;
		if (m_pCoreServerShell)
			m_pCoreServerShell->GetGameData(SGDI_TONG_APPLY_INVITE, (UINT_PTR)&sInvite, 0);
	}
		break;
	case enumTONG_COMMAND_ID_APPLY_CHANGE_ANNOUNCE:
	{
		DWORD	dwTongNameID = 0;
		dwTongNameID = m_pCoreServerShell->GetGameData(SGDI_TONG_GET_TONG_NAMEID, 0, nPlayerIdx);
		if (dwTongNameID == 0)
			break;

		STONG_SERVER_TO_CORE_CHECK_SET sCheckSet;
		sCheckSet.ProtocolType = enumTONG_COMMAND_ID_APPLY_CHANGE_ANNOUNCE;
		sCheckSet.m_inParam = 0;
		strcpy(sCheckSet.m_strParam1, "");
		strcpy(sCheckSet.m_strParam2, "");

		int	nRet = FALSE;
		if (m_pCoreServerShell)
			nRet = m_pCoreServerShell->GetGameDataInfo(SGDI_TONG_CHECK_SET_POWER, 0, nPlayerIdx, 0, (void*)&sCheckSet);
		if (!nRet)
			break;

		TONG_APPLY_CHANGE_ANNOUNCE_COMMAND* pApply = (TONG_APPLY_CHANGE_ANNOUNCE_COMMAND*)pData;
		if (pApply->m_wLength + 1 != sizeof(TONG_APPLY_CHANGE_ANNOUNCE_COMMAND))
			break;

		STONG_CHANGE_ANNOUNCE_COMMAND	sChange;
		sChange.ProtocolFamily = pf_alltong;
		sChange.ProtocolID = enumC2S_TONG_CHANGE_ANNOUNCE;
		strcpy(sChange.m_szAnnounce, pApply->m_szAnnounce);
		sChange.m_dwParam = nPlayerIdx;
		sChange.m_dwTongNameID = dwTongNameID;
		if (m_pTongClient)
			m_pTongClient->SendPackToServer((const void*)&sChange, sizeof(sChange));
	}
		break;
	case enumTONG_COMMAND_ID_MORE_INFO:
	{
		if (m_pCoreServerShell)
		{
			DWORD	dwTongNameID = 0;
			m_pCoreServerShell->GetGameData(SGDI_TONG_GET_TONG_NAMEID, (UINT_PTR)&dwTongNameID, nPlayerIdx);
			if (dwTongNameID == 0)
				break;

			STONG_MORE_INFO_COMMAND inf;
			inf.ProtocolFamily = pf_tong;
			inf.ProtocolID = enumC2S_TONG_MORE_INFO;
			inf.m_dwParam = nPlayerIdx;
			inf.m_dwTongNameID = dwTongNameID;

			char name[NAME_LENGHT];
			m_pCoreServerShell->GetGameData(SGDI_CHARACTER_NAME, (UINT_PTR)name, nPlayerIdx);

			inf.m_dwPlayerID = g_FileName2Id(name);

			if (m_pTongClient)
				m_pTongClient->SendPackToServer((const void*)&inf, sizeof(inf));
		}
	}
		break;
	case enumTONG_COMMAND_ID_ACCEPT_INVITE:
	{
		TONG_ACCEPT_INVIVE_COMMAND* pAccept = (TONG_ACCEPT_INVIVE_COMMAND*)pData;
		if (pAccept->m_wLength != sizeof(TONG_ACCEPT_INVIVE_COMMAND) - 1)
			break;

		if (pAccept->m_nPlayerIdx <= 0 || pAccept->m_nPlayerIdx >= MAX_PLAYER)
			break;

		char	szPlayerName[NAME_LENGHT] = { 0 }; // ten cua nguoi dc moi
		m_pCoreServerShell->GetGameDataInfo(SGDI_CHARACTER_NAME, 0, nPlayerIdx, szPlayerName);

		if (pAccept->m_btFlag == 0) // tu choi loi moi gia nhap bang
		{
			STONG_SERVER_TO_CORE_REFUSE_ADD	sRefuse;
			sRefuse.m_nSelfIdx = nPlayerIdx;             
			sRefuse.m_nTargetIdx = pAccept->m_nPlayerIdx;  
			sRefuse.m_dwNameID = 0;
			m_pCoreServerShell->OperationRequestInfo(SSOI_TONG_REFUSE_ADD, 0, 0, 0, &sRefuse);
			break;
		}
		else // dong y loi moi gia nhap bang
		{
			char	szTongName[NAME_LENGHT] = { 0 };
			STONG_SERVER_TO_CORE_CHECK_INVITE_CONDITION sInvite;
			sInvite.m_nSelfIdx = pAccept->m_nPlayerIdx; // nguoi moi
			sInvite.m_nTargetIdx = nPlayerIdx; // nguoi dc moi
			sInvite.m_dwNameID = g_FileName2Id(szPlayerName); // dw id cua nguoi dc moi

			int	nRet = FALSE;
			if (m_pCoreServerShell)
				nRet = m_pCoreServerShell->GetGameDataInfo(SGDI_TONG_CHECK_ADD_CONDITION, 0, 0, szTongName, &sInvite);

			if (nRet)
			{
				STONG_ADD_MEMBER_COMMAND	sTong;
				sTong.ProtocolFamily = pf_tong;
				sTong.ProtocolID = enumC2S_TONG_ADD_MEMBER;  
				sTong.m_dwParam = sInvite.m_nTargetIdx;
				sTong.m_dwPlayerNameID = sInvite.m_dwNameID;
				sTong.m_nLevel = sInvite.m_nLevel;
				sTong.m_gender = m_pCoreServerShell->GetGameDataInfo(SGDI_CHARACTER_SEX, 0, pAccept->m_nPlayerIdx, 0);
				sTong.m_btPlayerNameLength = strlen(szPlayerName);
				sTong.m_btTongNameLength = strlen(szTongName);
				// and once more in the invite branch
				int nTongNameLen = sTong.m_btTongNameLength;
				int nPlayerNameLen = sTong.m_btPlayerNameLength;
				if (nTongNameLen > NAME_LENGHT - 2)
					nTongNameLen = NAME_LENGHT - 2;
				if (nPlayerNameLen > NAME_LENGHT - 1 - nTongNameLen)
					nPlayerNameLen = NAME_LENGHT - 1 - nTongNameLen;
				sTong.m_btTongNameLength = nTongNameLen;
				sTong.m_btPlayerNameLength = nPlayerNameLen;
				memcpy(sTong.m_szBuffer, szTongName, nTongNameLen);
				memcpy(&sTong.m_szBuffer[nTongNameLen], szPlayerName, nPlayerNameLen);
				sTong.m_wLength = sizeof(STONG_ADD_MEMBER_COMMAND) - sizeof(sTong.m_szBuffer) + sTong.m_btTongNameLength + sTong.m_btPlayerNameLength;
				if (m_pTongClient)
					m_pTongClient->SendPackToServer((const void*)&sTong, sTong.m_wLength);
			}
			//else
			//{
			//	STONG_SERVER_TO_CORE_REFUSE_ADD	sRefuse;
			//	sRefuse.m_nSelfIdx = nPlayerIdx;             
			//	sRefuse.m_nTargetIdx = pAccept->m_nPlayerIdx;  
			//	sRefuse.m_dwNameID = 0;
			//	m_pCoreServerShell->OperationRequestInfo(SSOI_TONG_REFUSE_ADD, 0, 0, 0, &sRefuse);
			//	break;
			//}
		}
	}
		break;
	case enumTONG_COMMAND_ID_APPLY_POWER:
	{
		DWORD	dwTongNameID = 0;
		dwTongNameID = m_pCoreServerShell->GetGameData(SGDI_TONG_GET_TONG_NAMEID, 0, nPlayerIdx);
		if (dwTongNameID == 0)
			break;

		TONG_APPLY_POWER_COMMAND* pApply = (TONG_APPLY_POWER_COMMAND*)pData;
		if (pApply->m_wLength + 1 != sizeof(TONG_APPLY_POWER_COMMAND))
			break;

		int nRet = m_pCoreServerShell->GetGameData(SGDI_TONG_CHECK_POWER, (UINT_PTR)pApply, nPlayerIdx);
		if (nRet == 0)
			break;

		STONG_POWER_COMMAND	sPower;
		sPower.ProtocolFamily = pf_tong;
		sPower.ProtocolID = enumC2S_TONG_SET_POWER;
		sPower.m_dwTongNameID = dwTongNameID;
		sPower.m_dwParam = nPlayerIdx;
		sPower.m_dwNameID = g_FileName2Id(pApply->m_szName);
		sPower.m_btPos = pApply->m_btPos;
		memcpy(sPower.m_btPower, pApply->m_btPower, sizeof(sPower.m_btPower));

		//printf("DEBUG, sPower.m_dwTongNameID: %d\n", sPower.m_dwTongNameID);
		//printf("DEBUG, sPower.m_dwParam: %d\n", sPower.m_dwParam);
		//printf("DEBUG, sPower.m_btPos: %d\n", sPower.m_btPos);
		//printf("DEBUG, sPower.m_dwNameID: %d\n", sPower.m_dwNameID);
		//printf("DEBUG, sPower.sPower: ");
		//for(int i = 0; i < 10; i++)
			//printf("%d ", sPower.m_btPower[i]);
		//printf("\n");

		if (m_pTongClient)
			m_pTongClient->SendPackToServer((const void*)&sPower, sizeof(sPower));
	}
		break;
	case enumTONG_COMMAND_ID_GET_POWER:
	{
		DWORD	dwTongNameID = 0;
		m_pCoreServerShell->GetGameData(SGDI_TONG_GET_TONG_NAMEID, (UINT_PTR)&dwTongNameID, nPlayerIdx);
		if (dwTongNameID == 0)
			break;

		TONG_APPLY_POWER_COMMAND* pApply = (TONG_APPLY_POWER_COMMAND*)pData;
		if (pApply->m_wLength + 1 != sizeof(TONG_APPLY_POWER_COMMAND))
			break;

		STONG_POWER_COMMAND	sPower;
		sPower.ProtocolFamily = pf_tong;
		sPower.ProtocolID = enumC2S_TONG_GET_POWER;
		memcpy(sPower.m_btPower, pApply->m_btPower, sizeof(pApply->m_btPower));
		sPower.m_dwParam = nPlayerIdx;
		sPower.m_dwTongNameID = dwTongNameID;
		sPower.m_btPos = pApply->m_btPos;
		sPower.m_dwNameID = g_FileName2Id(pApply->m_szName);

		if (m_pTongClient)
			m_pTongClient->SendPackToServer((const void*)&sPower, sizeof(sPower));
	}
		break;
	}
}

BOOL KSwordOnLineSever::CheckPlayerID(unsigned long netidx, DWORD nameid)
{
	//if (netidx < 0 || netidx >= m_nMaxGameStaus)
	//	return FALSE;

	int i =FindSame(netidx);

	if (i<=0 || i>m_nMaxGameStaus)
		return FALSE;

	int idx = m_pGameStatus[i].nPlayerIndex;
	if (idx <= 0 || idx >= MAX_PLAYER)
		return FALSE;

	if (nameid != m_pCoreServerShell->GetGameData(SGDI_CHARACTER_ID, idx, 0))
		return FALSE;

	return TRUE;
}

INT KSwordOnLineSever::GetPlayerID(unsigned long netidx)
{
	int i = FindSame(netidx);

	if (i <= 0 || i > m_nMaxGameStaus)
		return 0;

	int idx = m_pGameStatus[i].nPlayerIndex;
	if (idx <= 0 || idx >= MAX_PLAYER)
		return 0;

	return idx;
}

////////////////////////////////////////////
//获取机器码 
/* int  KSwordOnLineSever::getMAC(char *mac)
{ 
  unsigned long s1,s2;
  unsigned char vendor_id[]="--------";//CPU提供商ID
  char *cpu32;
  char *cpu64;
  char *cpuscs;
   _asm    // 得到CPU提供商信息 
   {   
	      xor eax,eax   // 将eax清0
		  cpuid         // 获取CPUID的指令
		  mov dword ptr vendor_id,ebx
		  mov dword ptr vendor_id[+4],edx
		  mov dword ptr vendor_id[+8],ecx  
   } 
   sprintf(cpuscs,"%s",vendor_id);

  _asm    // 得到CPU ID的高32位 
  { 
	  mov eax,01h    
      xor edx,edx
      cpuid
      mov s2,eax
  }
  sprintf(cpu32,"%08X",s2);

  _asm    // 得到CPU ID的低64位
  { 
	  mov eax,03h
      xor ecx,ecx
      xor edx,edx
      cpuid
      mov s1,edx
      mov s2,ecx
  }
  sprintf(cpu64,"%04X%04X",s1,s2);

	NCB ncb;           
	typedef struct _ASTAT_           
	{ 
		ADAPTER_STATUS       adapt;     
		NAME_BUFFER   NameBuff[30];           
	}ASTAT, *PASTAT; 
	
	ASTAT Adapter;           
	typedef struct _LANA_ENUM           
	{ 
		UCHAR   length;     
		UCHAR   lana[MAX_LANA];           
	}LANA_ENUM;           
	
	LANA_ENUM lana_enum;                 
	UCHAR   uRetCode;           
	memset(&ncb,   0,   sizeof(ncb));           
	memset(&lana_enum,   0,   sizeof(lana_enum));           
	
	ncb.ncb_command  =   NCBENUM;           
	ncb.ncb_buffer   =   (unsigned   char   *)&lana_enum;           
	ncb.ncb_length   =   sizeof(LANA_ENUM);           
	uRetCode         =   Netbios(&ncb);           
	if(uRetCode != NRC_GOODRET)           
		return  uRetCode;           
	
	for(int lana=0; lana <lana_enum.length; ++lana)           
	{ 
		ncb.ncb_command  =   NCBRESET;   
		ncb.ncb_lana_num =   lana_enum.lana[lana];   
		uRetCode         =   Netbios(&ncb);       
		if(uRetCode ==NRC_GOODRET)     
			break;   
	}   
	if(uRetCode!=NRC_GOODRET) 
		return uRetCode;           	
	memset(&ncb,0, sizeof(ncb));     
	ncb.ncb_command    =   NCBASTAT;     
	ncb.ncb_lana_num   =   lana_enum.lana[0]; 
	strcpy((char*)ncb.ncb_callname,   "* ");   
	ncb.ncb_buffer     =   (unsigned   char   *)&Adapter; 
	ncb.ncb_length     =   sizeof(Adapter); 
	uRetCode           =   Netbios(&ncb);     
	if(uRetCode != NRC_GOODRET)       
		return   uRetCode;           
	sprintf(mac, "%X%02X%02X%02X%02X%02X%08X%08X",           
		Adapter.adapt.adapter_address[0],           
		Adapter.adapt.adapter_address[1],           
		Adapter.adapt.adapter_address[2],           
		Adapter.adapt.adapter_address[3],           
		Adapter.adapt.adapter_address[4],           
		Adapter.adapt.adapter_address[5],
		cpu32,
		cpu64
		); 
	return   0;       
} 
 */

void KSwordOnLineSever::EDPad_Encipher(char* pPlaintext, int nPlainLen)
 {
	 if (pPlaintext && nPlainLen > 0)
	 {	
		 unsigned char	*pContent = (unsigned char*)pPlaintext;
#define	NUM_FIX_CIPHER_CHARA	3
		 unsigned char cCipher[NUM_FIX_CIPHER_CHARA + NUM_FIX_CIPHER_CHARA] = { 'W', 'o', 'o', 'y', 'u', 'e'};
		 if (nPlainLen > NUM_FIX_CIPHER_CHARA)
		 {
			 cCipher[0] = pContent[nPlainLen - 1] % 4;
			 if (cCipher[0])
				 cCipher[0] = cCipher[cCipher[0] + 2];
			 else
				 cCipher[0] = pContent[nPlainLen - 1];
			 
			 if (nPlainLen > NUM_FIX_CIPHER_CHARA + 1)
			 {
				 cCipher[1] = 'l';
				 if (nPlainLen > NUM_FIX_CIPHER_CHARA + 2)
				 {
					 cCipher[2] = pContent[nPlainLen - 2];
					 if (nPlainLen > NUM_FIX_CIPHER_CHARA + 3)
						 cCipher[1] =pContent[nPlainLen - 3];
				 }
			 }
		 }
		 for (int nPos = 0; nPos < nPlainLen; nPos++)
		 {
			 pContent[nPos] = (pContent[nPos] - 0x20 + (0xff - cCipher[cCipher[0] % 3])) % 0xdf + 0x20;
			 cCipher[0] = cCipher[1];	cCipher[1] = cCipher[2];
			 cCipher[2] = pContent[nPos];
		 }
	 }
 }


bool KSwordOnLineSever::JieMa_Decipher(char* pInTxtPass,char *pOutTxtPass)
{

	char  szfdfd[64]={0};
	miracl *mip=mirsys(600,16);//初始化600位的10进行制数
	mip->IOBASE=16;	//16进制模式
	//定义并初始化变量
	big m=mirvar(0);	//m 放明文：注册码SN
	big c=mirvar(0);	//c 放密文：用户名Name
	big n=mirvar(0);	//n 模数
	big e=mirvar(0);	//e 公钥

	TCHAR SN[256]={0};
	TCHAR temp[256]={0};
	//TCHAR strMacAddr[256]={0};
	snprintf(SN, sizeof(SN), "%s", pInTxtPass);
	//检查SN是否为16进制
	int	len =strlen(SN),i,j;
	for (i=0,j=0;i<len;++i)
	{
		if(isxdigit(SN[i])==0)
		{
			j=1;
			break;
		}
	} 	
	//如果输入的SN为16进制且长度不为0
	if (j!=1)
	{	
		cinstr(m,SN);	  								//注册码换成BIG
		cinstr(n,"A7404030B471A60D33917658A42664ED");	//初始化模数n	 
		cinstr(e,"19421");								//初始化公钥e  质数
		//当m<n时
		#ifdef _WIN64
		if(mr_compare(m,n)==-1)  //64位用 mr_compare 32位用 compare
		#else
		// ms32.lib spells the comparison `compare'; `mr_compare' is the 64-bit name.
		if(compare(m,n)==-1)
		#endif
		{
			powmod(m,e,n,c);//计算c=m^e mod n  //解密
			cotstr(c,temp);
		}
		else
			j=1;		
	}
	//释放内存
	mirkill(m);
	mirkill(c);
	mirkill(n);
	mirkill(e);
	mirexit();

	if(j==1)
	{//解密失败  
		return false;
	}
	else
	{//解密成功 
		strcpy(pOutTxtPass,temp);
		return true;
	}
}
 
 //--------------------------------------------------------------------------
 //	功能：一次一密码乱本解码函数
 //--------------------------------------------------------------------------
void KSwordOnLineSever::EDPad_Decipher(char* pCiphertext, int nCiphertextLen)
 {
	 if (pCiphertext && nCiphertextLen > 0)
	 {
		 unsigned char* pContent = (unsigned char*)pCiphertext;
#define	NUM_FIX_CIPHER_CHARA	3
		 unsigned char cCipher[NUM_FIX_CIPHER_CHARA + NUM_FIX_CIPHER_CHARA] = { 'W', 'o', 'o', 'y', 'u', 'e'};
		 int nPos;
		 for (nPos = nCiphertextLen - 1; nPos >= NUM_FIX_CIPHER_CHARA; nPos--)
			 pContent[nPos] = (pContent[nPos] - 0x20 + (pContent[nPos - 3 + (pContent[nPos - 3] % 3)] - 0x20)) % 0xdf + 0x20;
		 if (nCiphertextLen > NUM_FIX_CIPHER_CHARA)
		 {
			 cCipher[0] = pContent[nCiphertextLen - 1] % 4;
			 if (cCipher[0])
				 cCipher[0] = cCipher[cCipher[0] + 2];
			 else
				 cCipher[0] = pContent[nCiphertextLen - 1];
			 
			 if (nCiphertextLen > NUM_FIX_CIPHER_CHARA + 1)
			 {
				 cCipher[1] = 'l';
				 if (nCiphertextLen > NUM_FIX_CIPHER_CHARA + 2)
				 {
					 cCipher[2] = pContent[nCiphertextLen - 2];
					 if (nCiphertextLen > NUM_FIX_CIPHER_CHARA + 3)
						 cCipher[1] =pContent[nCiphertextLen - 3];
				 }
			 }
		 }
		 
		 if (nPos)
			 memcpy(cCipher + NUM_FIX_CIPHER_CHARA, pContent, nPos);
		 
		 for (;nPos >= 0; nPos--)
		 {
			 pContent[nPos] = (pContent[nPos] - 0x20 + (cCipher[nPos + (cCipher[nPos] % 3)] - 0x20)) % 0xdf + 0x20;
		 }
	 }
 }

VOID  CALLBACK  KSwordOnLineSever::TimerProc(HWND hWnd,UINT nMsg,UINT nTimerid,DWORD dwTime)   
{   
	switch(nTimerid)   
	{   
	case   1:   ///处理ID为1的定时器   
		//Do1(); 
		 KillTimer(GetWindowHwnd(),1);
		 printf("关闭计算器成功！");
		break;   
	case   2:   ///处理ID为2的定时器   
		//Do2();   
		break;   
	}   
}   


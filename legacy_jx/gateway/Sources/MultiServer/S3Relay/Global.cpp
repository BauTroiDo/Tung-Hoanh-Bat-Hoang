// Global.cpp: implementation of the CGlobal class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Global.h"
#include "S3Relay.h"
#include "..\..\Engine\Src\KEngine.h"

#include <map>
#include "../../../Headers/inoutmac.h"
#include "DoScript.h"
#include  <time.h>
//////////////////////////////////////////////////////////////////////
//-----------------
//#include <windows.h>
//#include <stdio.h>
//#include <tlhelp32.h>
//#include <psapi.h>
//#pragma comment (lib,"psapi.lib")
//-----------------
//////////////////////////////////////////////////////////////////////

const DWORD _localmask = 0x0000FFFF;
const DWORD _localtag = 0x0000A8C0;


const DWORD delay_reclient = 30000;

////////////
//variants

CMemory g_memory;

CHeavenLib g_libHeaven;
CRainbowLib g_libRainbow;

//CTongConnect g_TongConn;

CSockThread g_threadSock;


CRelayCenter g_RelayCenter;
CRelayServer g_RelayServer;

CRootCenter g_RootCenter;
CRootClient g_RootClient(&g_RootCenter, FALSE);

CDBCenter g_DBCenter;
CDBClient g_DBClient(&g_DBCenter, FALSE);

CGatewayCenter g_GatewayCenter;
CGatewayClient g_GatewayClient(&g_GatewayCenter, FALSE);

//CHostConnect g_HostConnect;
//CHostConnect m_sHostConnect;  
CHostServer g_HostServer;
CTongServer g_TongServer;
CChatServer g_ChatServer;

CChannelMgr g_ChannelMgr;
CFriendMgr g_FriendMgr;

CTongSet g_cTongSet;

#define GFDGFGGAG "bChmgAWrkCJmIzKrQ56OCCjZTEZuQepz"
#define GKHJTOQNH "FYK_W04JQ0AQENjAiedwQWMAUYKQTWQV"

#define KGLNHHRNA "b0hxvhdT10JmpHrxBB6OjCt_AEZbdqpz" //2019000000
//////////////////////////////////////////////////
#define   MAX_TASK_COUNT 100

const char file_relaycfg[] = "relay_config.ini";


const char sec_gateway[] = "gateway";
const char sec_relay[] = "relay";
const char sec_root[] = "root";
const char sec_host[] = "host";
const char sec_tong[] = "tong";
const char sec_chat[] = "chat";
const char sec_dbrole[] = "dbrole";


const char key_ip[] = "ip";
const char key_address[] = "address";
const char key_playercnt[] = "playercnt";
const char key_precision[] = "precision";
const char key_freebuffer[] = "freebuffer";
const char key_buffersize[] = "buffersize";
const char key_account[] = "account";
const char key_password[] = "password";
const char key_retryinterval[] = "retryinterval";

char* severtime[] =  
{  
	    "129.6.15.29",    //美国
		"132.163.4.101",  //美国
		"132.163.4.102",  //美国
		"132.163.4.103",  //美国
		"128.138.140.44", //美国
		//"58.220.207.226",
		//"47.92.108.218",
		//"202.112.29.82",
		//"202.108.6.95",
		//"20.25.108.11",
		//"182.92.12.11",
		//"115.28.122.198",
		NULL  
};

TCHAR  nCurTime[64]={0};

#define NET_TIME_PORT 37

/*NTP专用端口号字符串：中国123 美国 37 */


/////////////////////////////////////////////////

static bool gs_reclient_enable = FALSE;

static UINT32 gs_timerRoot = 0;
static UINT32 gs_timerGateway = 0;
static UINT32 gs_timerDB = 0;
static UINT32 gs_timerFriendUdtDB = 0;


struct serTaskInfo
{
	int  nIdx; // i+1
	int  nStartHour; // TaskTime
	int  nStartMin; // TaskTime
	int  nEndHour; // TaskETime
	int  nEndMin; // TaskETime
	int  nIsOpen;
	int  IsYes;
	time_t  nOpenLoop; // sau khoang nOpenLoop thoi gian -> se mo
	time_t  nCloseLoop; // sau khoang nCloseLoop thoi gian -> se dong
	int  nState;     //状态 1: da start, 0: chua start hoac da tat
	int  nJGTimes[2]; // TaskInterval
	int  EndTime[5]; // TaskEnd
	int  StartTime[5]; // TaskStart
	char nLuaField[128]; // path + TaskFile
	char ntaskName[64];
	char nNextTaskKey[64]; // NextTaskKey
	int  nIsSaceSyn; // IsSaceSyn
	int  _colseLoop;
	bool isHaveSend; // false
	int  _IsShowSyn; // IsShowSyn
};

typedef std::map<int,serTaskInfo> _MapLoopTaskInfo;
_MapLoopTaskInfo nMapLoopTask;
int  nCount=0;
bool _oPentask = false;
//nMapLoopTask.clear();
//extern bool LoadTaskInfo();
/////////////////////////////////////////////////


HWND gGetMainWnd();
HWND gGetListBox();

void gSetTakkOpen(bool ver)
{
	_oPentask=ver;
	if  (_oPentask)
	    rTRACE("---------各个GS开启完毕,任务系统开启更新--------");
	else
		rTRACE("---------各个G关闭完毕，任务系统关闭更新--------");
}
/////////////////////////////////////////////////
// functions

std::_tstring gGetLocateDirectory()
{
	static struct _LOCALDIR
	{
		std::_tstring path;

		_LOCALDIR()
		{
			TCHAR buff[_MAX_PATH] = {0};
			::GetModuleFileName(NULL, buff, _MAX_PATH);
			TCHAR* pCh = _tcsrchr(buff, _T('\\'));
			assert(pCh);
			*(pCh + 1) = 0;
			path = buff;

			assert(!path.empty());
		}
	} s_locdir;

	return s_locdir.path;
}

//---------------------------------------------------------------------------
// 函数:	Random
// 功能:	返回一个小于nMax的随机整数
// 参数:	nMax	:	最大值
// 返回:	一个小于nMax的随机数
//---------------------------------------------------------------------------
UINT s_RandomS3(UINT nMax)
{
	if (nMax)
	{
		nRandomSeed = nRandomSeed * IA + IC;

		if ((nRandomSeed%nMax)<0)	//溢出了
		{
			nRandomSeed=42;
			
			return nMax;
		}

		return nRandomSeed % nMax; //取余数
	}
	else
	{
		return 0;
	}
}



int GetRandomNumberS3(int nMin, int nMax)
{
	int nIsThis=0;
	
	if (nMin<0 && nMax<0)
	{
		nMin=-nMin;
		nMax=-nMax;
		nIsThis=1;
	}
	else if (nMin<0 && nMax>0)
	{
		
	}
	
	if  (nMax - nMin + 1 < 0)
		return 0;
	
	if  (nIsThis)
	{
		int nReg=s_RandomS3(nMax - nMin + 1) + nMin;
		nReg=-nReg;
		return nReg; //返回负值
	}
	else
		return s_RandomS3(nMax - nMin + 1) + nMin;	   //返回正值
	
}

BOOL gWritePrivateProfileStringEx(LPCSTR lpAppName, LPCSTR lpKeyName, LPCSTR lpFileName, LPCSTR lpDefault)
{
	std::_tstring path = gGetLocateDirectory() + lpFileName;
	
	::WritePrivateProfileString(lpAppName, lpKeyName, lpDefault, path.c_str());
		
	return 	TRUE;

}

BOOL gWritePrivateProfileIntEx(LPCSTR lpAppName, LPCSTR lpKeyName, LPCSTR lpFileName, INT nDefault)
{
	std::_tstring path = gGetLocateDirectory() + lpFileName;
	
	TCHAR buffer[_MAX_PATH] = {0};
	//::WritePrivateProfileString(lpAppName, lpKeyName, lpDefault, buffer, _MAX_PATH, path.c_str());
    snprintf(buffer, sizeof(buffer),"%d",nDefault); 
	::WritePrivateProfileString(lpAppName, lpKeyName, buffer, path.c_str());  
	return TRUE;

}

std::_tstring gGetPrivateProfileStringEx(LPCSTR lpAppName, LPCSTR lpKeyName, LPCSTR lpFileName, LPCSTR lpDefault)
{
	std::_tstring path = gGetLocateDirectory() + lpFileName;

	TCHAR buffer[_MAX_PATH] = {0};
	::GetPrivateProfileString(lpAppName, lpKeyName, lpDefault, buffer, _MAX_PATH, path.c_str());
	return buffer;
}

INT gGetPrivateProfileIntEx(LPCSTR lpAppName, LPCSTR lpKeyName, LPCSTR lpFileName, INT nDefault)
{
	std::_tstring path = gGetLocateDirectory() + lpFileName;

	return ::GetPrivateProfileInt(lpAppName, lpKeyName, nDefault, path.c_str());
}

TCHAR gGetPrivateProfileCharEx(LPCSTR lpAppName, LPCSTR lpKeyName, LPCSTR lpFileName, TCHAR chDefault)
{
	std::_tstring theStr = ::gGetPrivateProfileStringEx(lpAppName, lpKeyName, lpFileName);

	return theStr.empty() ? chDefault : theStr[0];
}
//获取节数量？
const std::vector<std::_tstring>& gGetPrivateProfileSectionEx(LPCTSTR lpAppName, LPCTSTR lpFileName)
{
	std::_tstring path = gGetLocateDirectory() + lpFileName;

	
	static std::vector<std::_tstring> s_vecstr;
	s_vecstr.clear();

	const DWORD MAX_BUFFSIZE = 4096;
	char buff[MAX_BUFFSIZE];
	DWORD cch = ::GetPrivateProfileSection(lpAppName, buff, MAX_BUFFSIZE, path.c_str());

	for (char* pc = buff; *pc != 0; pc += strlen(pc))
		s_vecstr.push_back(pc);

	return s_vecstr;
}
//获取节名
const std::vector<std::_tstring>& gGetPrivateProfileSectionNamesEx(LPCTSTR lpFileName)
{
	std::_tstring path = gGetLocateDirectory() + lpFileName;

	
	static std::vector<std::_tstring> s_vecstr;
	s_vecstr.clear();

	const DWORD MAX_BUFFSIZE = 4096;
	char buff[MAX_BUFFSIZE];
	DWORD cch = ::GetPrivateProfileSectionNames(buff, MAX_BUFFSIZE, path.c_str());

	for (char* pc = buff; *pc != 0; pc += s_vecstr.back().size())
		s_vecstr.push_back(pc);

	return s_vecstr;
}




BOOL gInitialize()
{
	gSetTakkOpen(TRUE);
	//InitScript();  //初始话脚本 
	LoadTaskInfo();
	g_IniScriptEngine();
	
	gs_reclient_enable = TRUE;


	//check the netcard, need 1 at least  //至少要开启一个虚拟网卡
	if (gGetMacAndIPAddress(NULL, NULL, NULL, NULL, _localtag, 0) <= 0)
	{
		rTRACE("[初始化] gGetMacAndIPAddress client - :失败!!!");
		return FALSE;
	}
	

	if (!g_memory.Initialize())
	{
		rTRACE("[初始化] g_memory client - :失败!!!");
		return FALSE;
	}


	if (!g_libHeaven.Initialize())
	{
		rTRACE("[初始化] g_libHeaven client - :失败!!!");
		return FALSE;
	}

	if (!g_libRainbow.Initialize())
	{
		rTRACE("[初始化] g_libRainbow client - :失败!!!");
		return FALSE;
	}

	if (!g_ChannelMgr.Initialize())
	{
		rTRACE("[初始化] g_ChannelMgr client - :失败!!!");
		return FALSE;
	}

	if (!g_FriendMgr.Initialize())
	{
		rTRACE("[初始化] g_FriendMgr client - :失败!!!");
		return FALSE;
	}


	const size_t playercntRelay = gGetPrivateProfileIntEx(sec_relay, key_playercnt, file_relaycfg, 0);
	const size_t precisionRelay = gGetPrivateProfileIntEx(sec_relay, key_precision, file_relaycfg, 0);
	const size_t freebufferRelay = gGetPrivateProfileIntEx(sec_relay, key_freebuffer, file_relaycfg, 0);
	const size_t buffersizeRelay = gGetPrivateProfileIntEx(sec_relay, key_buffersize, file_relaycfg, 0);
	const DWORD listenaddrRelay = INADDR_ANY;
	const unsigned short listenportRelay = RELAY_SIBLING_PORT;  //监听 7776 端口

	if (!g_RelayCenter.Initialize())
	{
		rTRACE("[初始化] g_RelayCenter client - :失败!!!");
		return FALSE;
	}
	if (!g_RelayServer.Startup(playercntRelay, precisionRelay, freebufferRelay, buffersizeRelay, listenaddrRelay, listenportRelay))
	{
		rTRACE("[初始化] Relay Server Startup - :失败!!!");
	}


	const size_t freebufferRoot = gGetPrivateProfileIntEx(sec_root, key_freebuffer, file_relaycfg, 0);
	const size_t buffersizeRoot = gGetPrivateProfileIntEx(sec_root, key_buffersize, file_relaycfg, 0);
	std::_tstring addrRoot = gGetPrivateProfileStringEx(sec_root, key_address, file_relaycfg);
	const unsigned short portRoot = RELAY_ROUTE_PORT;

	if (!g_RootCenter.Initialize())
	{
		rTRACE("[初始化]Create g_RootCenter: 失败!!!");
		return FALSE;
	}
	if (!g_RootClient.Startup(freebufferRoot, buffersizeRoot, addrRoot.c_str(), portRoot))
	{
		rTRACE("[初始化]root client - %s:失败!!!", addrRoot.c_str());
		gOnOneClientClose(oneclient_root);
	}


	const size_t freebufferDB = gGetPrivateProfileIntEx(sec_dbrole, key_freebuffer, file_relaycfg, 0);
	const size_t buffersizeDB = gGetPrivateProfileIntEx(sec_dbrole, key_buffersize, file_relaycfg, 0);
	std::_tstring addrDB = gGetPrivateProfileStringEx(sec_dbrole, key_address, file_relaycfg);
	const unsigned short portDB = ROLEDBSVR_ROLEDB_PORT; //连接角色存档服务器 godess 5001

	if (!g_DBCenter.Initialize())
	{
		rTRACE("[初始化] g_DBCenter client - :失败!!!");
		return FALSE;
	}
	if (!g_DBClient.Startup(freebufferDB, buffersizeDB, addrDB.c_str(), portDB))
	{
		rTRACE("[初始化] DBRole client - %s:失败!!!", addrDB.c_str());
		gOnOneClientClose(oneclient_db);
	}

	
	const size_t freebufferGateway = gGetPrivateProfileIntEx(sec_gateway, key_freebuffer, file_relaycfg, 0);
	const size_t buffersizeGateway = gGetPrivateProfileIntEx(sec_gateway, key_buffersize, file_relaycfg, 0);
	std::_tstring addrGateway      = gGetPrivateProfileStringEx(sec_gateway, key_address, file_relaycfg);
	const unsigned short portGateway = GATEWAY_GAMESVR_SERVICE_PORT; //连接bishop 56732

	if (!g_GatewayCenter.Initialize())
	{
		rTRACE("[初始化] g_GatewayCenter client - :失败!!!");
		return FALSE;
	}
	//连接bishop
	if (!g_GatewayClient.Startup(freebufferGateway, buffersizeGateway, addrGateway.c_str(), portGateway))
	{
		rTRACE("[初始化] g_GatewayClient client - :失败!!!");
		gOnOneClientClose(oneclient_gateway);
	}

	const size_t playercntHost = gGetPrivateProfileIntEx(sec_host, key_playercnt, file_relaycfg, 0);
	const size_t precisionHost = gGetPrivateProfileIntEx(sec_host, key_precision, file_relaycfg, 0);
	const size_t freebufferHost = gGetPrivateProfileIntEx(sec_host, key_freebuffer, file_relaycfg, 0);
	const size_t buffersizeHost = gGetPrivateProfileIntEx(sec_host, key_buffersize, file_relaycfg, 0);
	const DWORD listenaddrHost = INADDR_ANY;
	const unsigned short listenportHost = RELAY_GAMESVR_SERVICE_PORT;//开放 监听端口 监听 有哪个客户端连接  5003

	if (!g_HostServer.Startup(playercntHost, precisionHost, freebufferHost, buffersizeHost, listenaddrHost, listenportHost))
	{
		rTRACE("[初始化] g_HostServer client:失败!!!");
	}

	const size_t playercntTong = gGetPrivateProfileIntEx(sec_tong, key_playercnt, file_relaycfg, 0);
	const size_t precisionTong = gGetPrivateProfileIntEx(sec_tong, key_precision, file_relaycfg, 0);
	const size_t freebufferTong = gGetPrivateProfileIntEx(sec_tong, key_freebuffer, file_relaycfg, 0);
	const size_t buffersizeTong = gGetPrivateProfileIntEx(sec_tong, key_buffersize, file_relaycfg, 0);
	const DWORD listenaddrTong = INADDR_ANY;
	const unsigned short listenportTong = RELAY_GAMESVR_TONGSERVICE_PORT;//开放帮会监听端口 5005

	if (!g_TongServer.Startup(playercntTong, precisionTong, freebufferTong, buffersizeTong, listenaddrTong, listenportTong))
	{
		rTRACE("[初始化] g_TongServer client:失败!!!");
	}


	const size_t playercntChat = gGetPrivateProfileIntEx(sec_chat, key_playercnt, file_relaycfg, 0);
	const size_t precisionChat = gGetPrivateProfileIntEx(sec_chat, key_precision, file_relaycfg, 0);
	const size_t freebufferChat = gGetPrivateProfileIntEx(sec_chat, key_freebuffer, file_relaycfg, 0);
	const size_t buffersizeChat = gGetPrivateProfileIntEx(sec_chat, key_buffersize, file_relaycfg, 0);
	const DWORD listenaddrChat = INADDR_ANY;
	const unsigned short listenportChat = RELAY_GAMESVR_CHATSERVICE_PORT;//开放监听 聊天端口 5004 

	if (!g_ChatServer.Startup(playercntChat, precisionChat, freebufferChat, buffersizeChat, listenaddrChat, listenportChat))
	{
		rTRACE("[初始化] g_ChatServer client - :失败!!!");
	}

#ifdef _WORKMODE_SINGLETHREAD
	g_threadSock.Start();  //信息输出线程
	//	return FALSE;
#endif

	// 帮会数据初始化
	if (g_cTongDB.Open())
	{
		    rTRACE("tong database open ok!");
		if (g_cTongSet.InitFromDB())
			rTRACE("tongset init ok !");
		else
		    rTRACE("[初始化] g_cTongSet client - :失败!!!");
	}
	else
	{
		rTRACE("[初始化] g_cTongDB client - :失败!!!");
	} 

	return TRUE;
}

BOOL gUninitialize()
{
	//UninitScript();
	UnLoadAllScript();

	try
	{
		//disable reclient timer//关闭进程
		gOnOneClientCreate(oneclient_root);
		gOnOneClientCreate(oneclient_gateway);
		gOnOneClientCreate(oneclient_db);


		gs_reclient_enable = FALSE;


#ifdef _WORKMODE_SINGLETHREAD
		g_threadSock.Stop();
#endif


		g_ChatServer.Shutdown();
		g_TongServer.Shutdown();
		g_HostServer.Shutdown();

		g_GatewayClient.Shutdown();
		g_GatewayCenter.Uninitialize();

		g_DBClient.Shutdown();
		g_DBCenter.Uninitialize();

		g_RootClient.Shutdown();
		g_RootCenter.Uninitialize();

		g_RelayServer.Shutdown();
		g_RelayCenter.Uninitialize();

		//delay
		::Sleep(2000);

		g_ChannelMgr.Uninitialize();
		g_FriendMgr.Uninitialize();


		g_libRainbow.Uninitialize();
		g_libHeaven.Uninitialize();

		g_memory.Uninitialize();

		g_cTongDB.Close();

	}
	catch (...)
	{
		return FALSE;
	}

	return TRUE;
}


BOOL gIsLocalIP(DWORD ip)
{
	return (ip & _localmask) == _localtag;
}


const UCHAR* gGetHostMac(int adapt, size_t* pSize)
{
	static struct _MACSTORE
	{
		enum {MAC_SIZE = 6};

		UCHAR macLocal[MAC_SIZE];
		UCHAR macGlobal[MAC_SIZE];


		_MACSTORE()
		{
			memset(macLocal, 0, sizeof(macLocal));
			memset(macGlobal, 0, sizeof(macGlobal));

			gGetMacAndIPAddress(macLocal, NULL, macGlobal, NULL, _localtag, MAC_SIZE);
		}
	} _macstore;

	if (!pSize == NULL)
		*pSize = _MACSTORE::MAC_SIZE;

	switch (adapt)
	{
	case local_adapt: return _macstore.macLocal;
	case global_adapt: return _macstore.macGlobal;
	}

	if (!pSize == NULL)
		*pSize = 0;

	assert(FALSE);
	return NULL;
}


DWORD gGetHostIP(int adapt)
{
	static struct _IPSTORE
	{
		DWORD ipLocal;
		DWORD ipGlobal;

		_IPSTORE() : ipLocal(0), ipGlobal(0)
		{
			gGetMacAndIPAddress(NULL, &ipLocal, NULL, &ipGlobal, _localtag, 0);
		}
	} _ipstore;

	switch (adapt)
	{
	case local_adapt:
		return _ipstore.ipLocal;
	case global_adapt:
		{
			//----------------gioi han ip------------
			//PassChecksum nJmm;
			//char nMnn[64]={0},nSnn[64]={0};
			//sprintf(nSnn,_ip2a(_ipstore.ipGlobal));
			//nJmm.SimplyDecryptPassword(nMnn,GKHJTOQNH);
			/*if (strstr(nSnn,nMnn))
			{
			}
			else
			{
				nJmm.SimplyDecryptPassword(nMnn,GFDGFGGAG);

				if (strstr(nSnn,nMnn))
				{
				}
				else
				{
					gUninitialize();
					return 0;
				}
			}*/
			//--------------------------------------
		   return _ipstore.ipGlobal;   //外网IP
		}
	}

	assert(FALSE);
	return 0;
}



BOOL gConnectToSibling(const char* addr)
{
	const size_t freebuffer = gGetPrivateProfileIntEx(sec_relay, key_freebuffer, file_relaycfg, 0);
	const size_t buffersize = gGetPrivateProfileIntEx(sec_relay, key_buffersize, file_relaycfg, 0);

	CRelayClient* pRelayClient = new CRelayClient(&g_RelayCenter, TRUE);
	if (!pRelayClient->Startup(freebuffer, buffersize, addr, RELAY_SIBLING_PORT, FALSE))
		return FALSE;

	return TRUE;
}


size_t gFillClntwayPckg(DWORD nameid, unsigned long param, 
						const RELAY_ASKWAY_DATA* pAskWay, size_t sizeAskWay, RELAY_ASKWAY_DATA* pClntWay, size_t sizeClntWay)
{
	assert(sizeAskWay >= sizeof(EXTEND_HEADER));
	assert(sizeClntWay >= sizeAskWay + sizeof(DWORD) * 2);

	size_t sizeNeed = sizeAskWay + sizeof(DWORD) * 2;

	if (sizeClntWay < sizeNeed)
		return 0;

	// The two copies below move the packet's own fields - the seek method's length and the
	// route data's, both WORDs a sibling relay wrote - while the room they land in is what
	// this relay measured out of sizeAskWay.  Nothing but the packet's own size says those
	// fields add up to the bytes that arrived with it, and a claim larger than that is read
	// past the source and written past the caller's buffer by the same count.
	if (sizeof(RELAY_ASKWAY_DATA) + (size_t)pAskWay->wMethodDataLength
		+ (size_t)pAskWay->routeDateLength > sizeAskWay)
		return 0;

	*pClntWay = *pAskWay;
	pClntWay->seekRelayCount ++;

	const void* pMethodData = pAskWay + 1;
	void* pMethodDataDup = pClntWay + 1;
	memcpy(pMethodDataDup, pMethodData, pAskWay->wMethodDataLength);

	DWORD* pp = (DWORD*)((BYTE*)pMethodDataDup + pClntWay->wMethodDataLength);
	pp[0] = nameid; pp[1] = param;
	pClntWay->wMethodDataLength += sizeof(DWORD) * 2;

	const void* pRouteData = (BYTE*)pMethodData + pAskWay->wMethodDataLength;
	void* pRouteDataDup = pp + 2;
	memcpy(pRouteDataDup, pRouteData, pAskWay->routeDateLength);

	return sizeNeed;
}

size_t gFillLosewayPckg(DWORD nToIP, DWORD nToRelayID, 
					   const void* pLosePckg, size_t sizeLosePckg, void* pLoseway, size_t sizeLoseway)
{
	assert(sizeLosePckg >= sizeof(EXTEND_HEADER));
	assert(sizeLoseway >= sizeLosePckg + sizeof(RELAY_DATA));

	size_t sizeNeed = sizeLosePckg + sizeof(RELAY_DATA);

	if (sizeLoseway < sizeNeed)
		return 0;

	EXTEND_HEADER* pSrcHdr = (EXTEND_HEADER*)pLosePckg;

	RELAY_DATA* pLoseData = (RELAY_DATA*)pLoseway;

	pLoseData->ProtocolFamily = pSrcHdr->ProtocolFamily;
	pLoseData->ProtocolID = relay_s2c_loseway;

	pLoseData->nToIP = nToIP;
	pLoseData->nToRelayID = nToRelayID;
	pLoseData->nFromIP = 0;
	pLoseData->nFromRelayID = -1;
	pLoseData->routeDateLength = sizeLosePckg;

	void* pRouteData = pLoseData + 1;
	memcpy(pRouteData, pLosePckg, sizeLosePckg);

	return sizeNeed;
}

BOOL gIsLegalString(LPCTSTR sz, size_t lenlolmt, size_t lenhilmt)
{
	if (sz == NULL || lenhilmt < lenlolmt)
		return FALSE;
	for (size_t pl = 0; pl < lenlolmt; ++pl)
		if (sz[pl] == 0)
			return FALSE;
	for (size_t ph = lenlolmt; ph < lenhilmt; ++ph)
		if (sz[ph] == 0)
			return TRUE;
	return TRUE;
}

DWORD g_String2Id(LPCSTR pString)
{
	if ( pString )
	{
	DWORD Id = 0;
	for (int i = 0; pString[i]; i++)
	{
		Id = (Id + (i + 1) * pString[i]) % 0x8000000b * 0xffffffef;
	}
	return Id ^ 0x12345678;
	}	
	return ( DWORD )( -1 );
}

DWORD g_String2IdA(LPCSTR pString)
{
///////////////////////////////////////////////////////////////////////
	if ( pString )
	{
	DWORD dwID = 0; //DWORD
		
		for ( int i = 0; pString[i]; i++ )
		{
			dwID = ( dwID + ( i + 1 ) * pString[i]) % 0x8000000B * 0xFFFFFFEF;
		}
		return (dwID ^ 0x12345678);	
	}
	
	return (DWORD)(-1);
}


BOOL gOnOneClientCreate(ONECLIENT ct)
{
	if (!gs_reclient_enable)
		return TRUE;


	HWND mainwnd = gGetMainWnd();
	if (mainwnd == NULL)
		return FALSE;

	switch (ct)
	{
	case oneclient_root:
		if (gs_timerRoot)
		{
			if (::KillTimer(mainwnd, gs_timerRoot))
			{
				gs_timerRoot = 0;
//				rTRACE("disable reclient: root");
			}
		}
		break;
	case oneclient_gateway:
		if (gs_timerGateway)
		{
			if (::KillTimer(mainwnd, gs_timerGateway))
			{
				gs_timerGateway = 0;
				rTRACE("disable reclient: gateway");
			}
		}
		break;
	case oneclient_db:
		if (gs_timerDB)
		{
			if (::KillTimer(mainwnd, gs_timerDB))
			{
				gs_timerDB = 0;
				rTRACE("disable reclient: db");
			}
		}
		break;
	default:
		return FALSE;
	}

	return TRUE;
}

BOOL gOnOneClientClose(ONECLIENT ct)
{
	if (!gs_reclient_enable)
		return FALSE;

	HWND mainwnd = gGetMainWnd();
	if (mainwnd == NULL)
		return FALSE;

	switch (ct)
	{
	case oneclient_root:
		if (!gs_timerRoot)
		{
			INT rootinterval = gGetPrivateProfileIntEx(sec_root, key_retryinterval, file_relaycfg, 0);
			if (rootinterval > 0)
			{
				gs_timerRoot = ::SetTimer(mainwnd, timer_rootclient, rootinterval, NULL);
				if (!gs_timerRoot)
				{
					rTRACE("FAIL: enable root reclient");
					return FALSE;
				}
//				rTRACE("enable reclient: root");
			}
		}
		break;
	case oneclient_gateway:
		if (!gs_timerGateway)
		{
			INT gatewayinterval = gGetPrivateProfileIntEx(sec_gateway, key_retryinterval, file_relaycfg, 0);
			if (gatewayinterval > 0)
			{
				gs_timerGateway = ::SetTimer(mainwnd, timer_gatewayclient, gatewayinterval, NULL);
				if (!gs_timerGateway)
				{
					rTRACE("FAIL: enable gateway reclient");
					return FALSE;
				}
				rTRACE("enable reclient: gateway");
			}
		}
		break;
	case oneclient_db:
		if (!gs_timerDB)
		{
			INT dbinterval = gGetPrivateProfileIntEx(sec_dbrole, key_retryinterval, file_relaycfg, 0);
			if (dbinterval > 0)
			{
				gs_timerDB = ::SetTimer(mainwnd, timer_dbclient, dbinterval, NULL);
				if (!gs_timerDB)
				{
					rTRACE("FAIL: enable db reclient");
					return FALSE;
				}
				rTRACE("enable reclient: db");
			}
		}
		break;
	default:
		return FALSE;
	}

	return TRUE;
}

BOOL gReOneClient(ONECLIENT ct)
{
	if (!gs_reclient_enable)
		return FALSE;

	switch (ct)
	{
	case oneclient_root:
		{{

		/*if (g_RootClient.IsReady())
			return FALSE;

		const size_t freebufferRoot = gGetPrivateProfileIntEx(sec_root, key_freebuffer, file_relaycfg, 0);
		const size_t buffersizeRoot = gGetPrivateProfileIntEx(sec_root, key_buffersize, file_relaycfg, 0);
		std::_tstring addrRoot = gGetPrivateProfileStringEx(sec_root, key_address, file_relaycfg);
		const unsigned short portRoot = RELAY_ROUTE_PORT;

//		rTRACE("try reclient: root - %s", addrRoot.c_str());

		if (!g_RootClient.Startup(freebufferRoot, buffersizeRoot, addrRoot.c_str(), portRoot))
		{
			rTRACE("FAIL: reclient root - %s", addrRoot.c_str());
			//return FALSE;
		}*/

		}}
		break;

	case oneclient_gateway:
		{{
		if (g_GatewayClient.IsReady())
			return FALSE;

		const size_t freebufferGateway = gGetPrivateProfileIntEx(sec_gateway, key_freebuffer, file_relaycfg, 0);
		const size_t buffersizeGateway = gGetPrivateProfileIntEx(sec_gateway, key_buffersize, file_relaycfg, 0);
		std::_tstring addrGateway = gGetPrivateProfileStringEx(sec_gateway, key_address, file_relaycfg);
		const unsigned short portGateway = GATEWAY_GAMESVR_SERVICE_PORT;

		rTRACE("try reclient: gateway - %s", addrGateway.c_str());

		if (!g_GatewayClient.Startup(freebufferGateway, buffersizeGateway, addrGateway.c_str(), portGateway))
		{
			rTRACE("FAIL: reclient gateway - %s", addrGateway.c_str());
			//return FALSE;
		}
		}}
		break;

	case oneclient_db:
		{{
		if (g_DBClient.IsReady())
			return FALSE;

		const size_t freebufferDB = gGetPrivateProfileIntEx(sec_dbrole, key_freebuffer, file_relaycfg, 0);
		const size_t buffersizeDB = gGetPrivateProfileIntEx(sec_dbrole, key_buffersize, file_relaycfg, 0);
		std::_tstring addrDB = gGetPrivateProfileStringEx(sec_dbrole, key_address, file_relaycfg);
		const unsigned short portDB = ROLEDBSVR_ROLEDB_PORT;

		rTRACE("try reclient: db - %s", addrDB.c_str());

		if (!g_DBClient.Startup(freebufferDB, buffersizeDB, addrDB.c_str(), portDB))
		{
			rTRACE("FAIL: reclient db - %s", addrDB.c_str());
			//return FALSE;
		}
		}}
		break;
	}

	return TRUE;
}


BOOL gConsignClientSD(IClient* pClient)
{
	if (!pClient)
		return TRUE;

	HWND mainwnd = gGetMainWnd();
	if (!mainwnd)
		return FALSE;

	pClient->AddRef();
	::PostMessage(mainwnd, UM_CONSIGNCLIENTSD, (WPARAM)pClient, 0);

	return TRUE;
}


BOOL gStartFriendTimingUpdateDB(UINT elapse)
{
	if (gs_timerFriendUdtDB)
		return TRUE;

	HWND mainwnd = gGetMainWnd();
	if (!mainwnd)
		return FALSE;

	gs_timerFriendUdtDB = ::SetTimer(mainwnd, timer_friendudtdb, elapse, NULL);
	if (!gs_timerFriendUdtDB)
	{
		rTRACE("FAIL: start friend timing update db");
		return FALSE;
	}

	rTRACE("friend timing update db: start");

	return TRUE;
}

BOOL gStopFriendTimingUpdateDB()
{
	if (!gs_timerFriendUdtDB)
		return TRUE;

	HWND mainwnd = gGetMainWnd();
	if (!mainwnd)
		return FALSE;

	::KillTimer(mainwnd, gs_timerFriendUdtDB);
	gs_timerFriendUdtDB = 0;

	rTRACE("friend timing update db: stop");

	return TRUE;
}
//好友循环
BOOL gFriendTimingUpdateDB()
{
	if (!g_FriendMgr.BkgrndUpdateDB())
	{
		rTRACE("warning: update friend DB fail");
		return FALSE;
	}

	rTRACE("update friend DB succeed");//更新好友信息
	return TRUE;
}

//int    nS3loop=0;

//内存整理
BOOL ClearMer()
{
/*	HANDLE SnapShot=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);

	if(SnapShot==NULL)
	{
		return FALSE;
	}
	PROCESSENTRY32 ProcessInfo;//声明进程信息变量
	ProcessInfo.dwSize=sizeof(ProcessInfo);//设置ProcessInfo的大小
	//返回系统中第一个进程的信息
	BOOL Status=Process32First(SnapShot,&ProcessInfo);
	while(Status)
	{
		HANDLE hPro=OpenProcess(PROCESS_ALL_ACCESS,TRUE,ProcessInfo.th32ProcessID);
		if(hPro)
		{
			SetProcessWorkingSetSize(hPro,0,0);
			//内存整理
			EmptyWorkingSet(hPro);
			CloseHandle(hPro);
		}
		//printf("%s\n",ProcessInfo.szExeFile);
		//获取下一个进程的信息
		Status=Process32Next(SnapShot,&ProcessInfo);
	}*/
	return TRUE;
}


void SetTimeFromTP(ULONG ulTime,int n)    // 根据时间协议返回的时间设置系统时间
{
	
// Windows文件时间是一个64位的值，它是从1601年1月1日中午12:00到现在的时间间隔，
// 单位是1/1000 0000秒，即1000万分之1秒（100-nanosecond 	
	FILETIME ft;
	SYSTEMTIME st;
// 首先将基准时间（1900年1月1日0点0分0秒0毫秒）转化为Windows文件时间	
	st.wYear = 1900;
	st.wMonth = 1;
	st.wDay = 1;
	st.wHour = 0;
	st.wMinute = 0;
	st.wSecond = 0;
	st.wMilliseconds = 0; //毫秒
	SystemTimeToFileTime(&st, &ft);
// 然后将Time Protocol使用的基准时间加上以及逝去的时间，即ulTime
	LONGLONG *pLLong = (LONGLONG *)&ft;
	// 注意,文件时间单位是1/1000 0000秒，即1000万分之1秒（100-nanosecond ）
	*pLLong +=(LONGLONG)10000000 * ulTime;
	//*pLLong +=(LONGLONG)48*60*60*1000;//加上时间差*1000
	//再将时间转化回来，更新系统时间
	FileTimeToSystemTime(&ft, &st);

	sprintf(nCurTime,"%04d%02d%02d%02d",st.wYear,st.wMonth,st.wDay,st.wHour);

	//rTRACE("当前时间%d:%04d年%02d月%02d日%02d小时%02d分%02d秒%d毫秒 ",n, st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond,st.wMilliseconds);  

	//SetSystemTime(&st);
}


bool GetRemoteTime(char* szSever, unsigned long& ulRemoteTime)  
{  
    SOCKET sock = socket(AF_INET, SOCK_STREAM,IPPROTO_TCP);
	
    if(sock == INVALID_SOCKET)  
    {  
        return false;  
    } 
	
    unsigned long flag = 1;
	
    if ((::ioctlsocket(sock, FIONBIO, &flag) == 0))  
    {  
        sockaddr_in severAddr;  
        severAddr.sin_family = AF_INET;  
        severAddr.sin_port = htons(NET_TIME_PORT);                
        severAddr.sin_addr.S_un.S_addr = inet_addr(szSever);  
        ::connect(sock, (sockaddr*)&severAddr, sizeof(severAddr));
        struct fd_set mask;           
        FD_ZERO(&mask);  
        FD_SET(sock, &mask);  
        struct timeval timeout;  
        timeout.tv_sec = 5;      //超时时间 /TIMEOUT_CONNECT
        timeout.tv_usec = 0;          
        if (::select(0, NULL, &mask, NULL, &timeout) == 1)  
        {  
            timeout.tv_sec = 5;   //TIMEOUT_RECEIVE 收包超时时间
			
            if (::select(0, &mask, NULL, NULL, &timeout) == 1)  
            {                 
                if (::recv(sock, (char*)&ulRemoteTime,sizeof(ulRemoteTime), 0))  
                {  
                    ulRemoteTime = ntohl(ulRemoteTime);// - TIMEDEVIATION;  
                    ::closesocket(sock);  
                    return TRUE;  
                }             
            }             
        }  
		
    }  
    ::closesocket(sock);  
    return false;  
}

BOOL gDoLocalItemDataSyn()
{
	BYTE buffer[max_packagesize];
	CHAT_GROUPMAN* pCgm = (CHAT_GROUPMAN*)buffer;

	pCgm->ProtocolType = chat_coreitemdata;
	pCgm->wChatLength  = 202;
	pCgm->byHasIdentify= 0;

	g_ChannelMgr.BroadToGameServer(buffer,sizeof(CHAT_GROUPMAN));

	return TRUE;
}


bool LoadTaskInfo()
{
	nMapLoopTask.clear();

	g_SetRootPath(NULL);
	g_SetFilePath("\\");

	KIniFile g_TaskLoop;
	if  (!g_TaskLoop.Load("\\settings\\task\\tasklist.ini"))
	{
		rTRACE("[初始化] -------加载tasklist.ini:失败 ---------!!!");
		return false;
	}

	g_TaskLoop.GetInteger("List", "Count",1, &nCount);

	if (nCount>=MAX_TASK_COUNT)
		nCount = MAX_TASK_COUNT;

	if (nCount>0)
	{
		for (int i=0;i<nCount;i++)
		{
			char nTaskid[32]={0};
			snprintf(nTaskid, 32, "Task_%d",i);
			g_TaskLoop.GetInteger(nTaskid,"IsOpen",0,&nMapLoopTask[i+1].nIsOpen);
			if (nMapLoopTask[i+1].nIsOpen<=0)
			{
				nMapLoopTask[i+1].nIdx        = 0;
				nMapLoopTask[i+1].nCloseLoop  = 0;
				nMapLoopTask[i+1].nOpenLoop   = 60;
				nMapLoopTask[i+1].nState      = 0;
				continue;
			}
			nMapLoopTask[i+1].isHaveSend=false;
			nMapLoopTask[i+1].nIdx = i+1;
			g_TaskLoop.GetString(nTaskid, "TaskName","", nMapLoopTask[i+1].ntaskName,sizeof(nMapLoopTask[i+1].ntaskName));
			char nTempStr[64];
			ZeroMemory(nTempStr,sizeof(nTempStr));
			g_TaskLoop.GetString(nTaskid, "TaskFile","",nTempStr,sizeof(nTempStr));
			sprintf(nMapLoopTask[i+1].nLuaField,"\\script\\task\\%s",nTempStr);

			g_TaskLoop.GetInt2(nTaskid,"TaskInterval",nMapLoopTask[i+1].nJGTimes);
			g_TaskLoop.GetInteger2(nTaskid,"TaskTime",&nMapLoopTask[i+1].nStartHour,&nMapLoopTask[i+1].nStartMin);  //每天开始的时间
			g_TaskLoop.GetInteger2(nTaskid,"TaskETime",&nMapLoopTask[i+1].nEndHour,&nMapLoopTask[i+1].nEndMin);     //每天结束的时间
			g_TaskLoop.GetInt5(nTaskid,"TaskEnd",nMapLoopTask[i+1].EndTime);   //年/月/日/分/秒
			g_TaskLoop.GetInt5(nTaskid,"TaskStart",nMapLoopTask[i+1].StartTime); //年/月/日/分/秒
			g_TaskLoop.GetString(nTaskid,"NextTaskKey","",nMapLoopTask[i+1].nNextTaskKey,sizeof(nMapLoopTask[i+1].nNextTaskKey));
			g_TaskLoop.GetInteger(nTaskid,"IsSaceSyn",1,&nMapLoopTask[i+1].nIsSaceSyn);
			g_TaskLoop.GetInteger(nTaskid,"IsShowSyn",0,&nMapLoopTask[i+1]._IsShowSyn);
			
		}
	}
	g_TaskLoop.Clear();

	rTRACE("[初始化] 加载taskList信息成功!!!");
	return true;
}


void dotask()
{

	SYSTEMTIME timeinfo;
	GetLocalTime(&timeinfo);
	//GetSystemTime(&timeinfo);

	struct tm time;
	time.tm_year = timeinfo.wYear - 1900;//tm中的年份比实际年份小1900，需要减掉
	time.tm_mon = timeinfo.wMonth - 1;   //tm中的月份从0开始，需要减1
	time.tm_mday = timeinfo.wDay;
	time.tm_hour = timeinfo.wHour;
	time.tm_min = timeinfo.wMinute;
	time.tm_sec = timeinfo.wSecond;
	time_t rawtime = mktime(&time);

	//time_t rawtime;
	//struct tm * timeinfo;
	//time(&rawtime);    //距离现在的时间（秒）
	//timeinfo = localtime(&rawtime); //当前时间
	//if  (!timeinfo) return;

	//BYTE buffer_th[max_packagesize];
	//ZeroMemory(buffer_th,sizeof(buffer_th));
	//GS_TASK_GROUPMAN* pCgm_ = (GS_TASK_GROUPMAN*)buffer_th;
	//pCgm_->ProtocolType = chat_goldthread;
	//pCgm_->nTaskNo      = 0;
	//pCgm_->isOpen       = 0;
	//pCgm_->isSave       = 0;
	//sprintf(pCgm_->nCurTime,"00:00:00");
	//sprintf(pCgm_->nNextTime,"00:00:00");
	//sprintf(pCgm_->nPath,"\\script\\task\\goldthread.lua");
	//g_ChannelMgr.BroadToGameServer(buffer_th,sizeof(GS_TASK_GROUPMAN));

	if (nCount > 0)
	{
		_MapLoopTaskInfo::iterator it;
		for (it = nMapLoopTask.begin(); it != nMapLoopTask.end(); ++it)
		{
			if (it->second.nIdx > 0 && it->second.nIdx <= MAX_TASK_COUNT)
			{
				int nTaskNo = it->second.nIdx;

				BYTE buffer[max_packagesize];
				ZeroMemory(buffer, sizeof(buffer));
				GS_TASK_GROUPMAN* pCgm = (GS_TASK_GROUPMAN*)buffer;
				pCgm->ProtocolType = chat_coretask;
				pCgm->nTaskNo = nTaskNo;
				pCgm->isOpen = it->second.nIsOpen;
				pCgm->isSave = it->second.nIsSaceSyn;//是否保存 
				pCgm->isShow = it->second._IsShowSyn; //用来显示说明
				sprintf(pCgm->nCurTime, "00:00:00");
				sprintf(pCgm->nNextTime, "00:00:00");
				snprintf(pCgm->taskName, sizeof(pCgm->taskName), "%s", it->second.ntaskName);

				if (it->second.nIsOpen <= 0)
				{
					it->second.nCloseLoop = 0;
					it->second.nOpenLoop = 60;
					it->second.nState = 0;

					if (!it->second.isHaveSend)
					{
						it->second.isHaveSend = true;
						pCgm->isOpen = false;
						pCgm->isSave = true;
						pCgm->isShow = false;
						g_ChannelMgr.BroadToGameServer(buffer, sizeof(GS_TASK_GROUPMAN));
					}

					continue;
				}

				//状态恢复
				if (it->second.nState == 1 && it->second.nJGTimes[0] <= 0 && it->second.nEndHour < 0 && it->second.EndTime[0] <= 0)
				{ // ko set thoi gian ket thuc, ko set interval
					it->second.nCloseLoop++;
					if (timeinfo.wHour == 0 && it->second.nStartHour == 23)
					{
						it->second.nState = 0;
						it->second.nCloseLoop = 0;
						rTRACE("--------第[%d]个执行复原成功,名称:%s[%d时%d分]------A \n", it->second.nIdx, it->second.ntaskName, it->second.nStartHour, it->second.nStartMin);
					}
					else if (timeinfo.wHour > it->second.nStartHour || (timeinfo.wHour == it->second.nStartHour && timeinfo.wMinute > it->second.nStartMin))//相隔1分钟后复原
					{
						it->second.nState = 0;
						it->second.nCloseLoop = 0;
						rTRACE("--------第[%d]个执行复原成功[%d时%d分],名称:%s[%d时%d分]------B \n", it->second.nIdx, timeinfo.wHour, timeinfo.wMinute, it->second.ntaskName, it->second.nStartHour, it->second.nStartMin);
					}
				}

				if (it->second.nJGTimes[0] > 0 && it->second.nState == 1 && rawtime >= it->second.nCloseLoop)
				{//已经开始了 还没有准时关闭的 就关闭它	
					it->second.nCloseLoop = 0;
					it->second.nState = 0;   //已经关闭

					if (it->second.nJGTimes[1] > 0)
					{
						pCgm->isOpen = false;
						snprintf(pCgm->nPath, sizeof(pCgm->nPath), "%s", it->second.nLuaField);
						g_ChannelMgr.BroadToGameServer(buffer, sizeof(GS_TASK_GROUPMAN));
						rTRACE("第(%d)个关闭成功,持续时间:%d分,名称:%s\n", it->second.nIdx, it->second.nJGTimes[1], it->second.ntaskName);
					}
				}
				else if (it->second.EndTime[0] > 0 && it->second.nState == 1 && timeinfo.wMonth + 1 == it->second.EndTime[1] && timeinfo.wDay == it->second.EndTime[2] && timeinfo.wHour == it->second.EndTime[3] && timeinfo.wMinute == it->second.EndTime[4])  // && timeinfo.tm_sec==0
				{ // het gio -> dong task da mo
					it->second.nState = 0;
					pCgm->isOpen = false;
					snprintf(pCgm->nPath, sizeof(pCgm->nPath), "%s", it->second.nLuaField);
					g_ChannelMgr.BroadToGameServer(buffer, sizeof(GS_TASK_GROUPMAN));
					rTRACE("第(%d)个关闭成功,结束:%d年%d月%d日%d时%d分,名称:%s\n", it->second.nIdx, it->second.EndTime[0], it->second.EndTime[1], it->second.EndTime[2], it->second.EndTime[3], it->second.EndTime[4], it->second.ntaskName);
				}
				else if ((it->second.nEndHour >= 0 || it->second.nEndMin >= 0) && it->second.nState == 1 && timeinfo.wHour == it->second.nEndHour && timeinfo.wMinute == it->second.nEndMin)// && timeinfo.tm_sec==0
				{//准时结束 het gio -> dong task da mo
					it->second.nState = 0;
					pCgm->isOpen = false;
					snprintf(pCgm->nPath, sizeof(pCgm->nPath), "%s", it->second.nLuaField);
					g_ChannelMgr.BroadToGameServer(buffer, sizeof(GS_TASK_GROUPMAN));
					rTRACE("第(%d)个执行成功,结束:%d时%d分,每天定时结束,名称:%s\n", it->second.nIdx, it->second.nEndHour, it->second.nEndMin, it->second.ntaskName);
				}
				else if (it->second.StartTime[0] > 0 && it->second.EndTime[0] > 0 && it->second.nState == 0 && timeinfo.wMonth + 1 == it->second.StartTime[1] && timeinfo.wDay == it->second.StartTime[2] && timeinfo.wHour == it->second.StartTime[3] && timeinfo.wMinute == it->second.StartTime[4])// && timeinfo.tm_sec==0
				{//指定时间执行一次 den gio -> bat dau mo
					it->second.nState = 1;
					it->second._colseLoop = 0;
					snprintf(pCgm->nPath, sizeof(pCgm->nPath), "%s", it->second.nLuaField);
					g_ChannelMgr.BroadToGameServer(buffer, sizeof(GS_TASK_GROUPMAN));
					rTRACE("第(%d)个执行成功,开始:%d年%d月%日%d时%d分执行一次,名称:%s\n", it->second.nIdx, it->second.StartTime[0], it->second.StartTime[1], it->second.StartTime[2], it->second.StartTime[3], it->second.StartTime[4], it->second.ntaskName);
				}
				else if ((it->second.nStartHour > 0 || it->second.nStartMin > 0) && it->second.nState == 0 && timeinfo.wHour == it->second.nStartHour && timeinfo.wMinute == it->second.nStartMin) //timeinfo.tm_sec==0
				{//单独时间大于零的//定时执行 den gio -> bat dau mo
					it->second.nState = 1;
					it->second._colseLoop = 0;
					snprintf(pCgm->nPath, sizeof(pCgm->nPath), "%s", it->second.nLuaField);
					sprintf(pCgm->nCurTime, "%02d:%02d", it->second.nStartHour, it->second.nStartMin);
					g_ChannelMgr.BroadToGameServer(buffer, sizeof(GS_TASK_GROUPMAN));
					rTRACE("第(%d)个执行成功[%d时%d分],开始:%d时%d分,定时一次,名称:%s\n", it->second.nIdx, timeinfo.wHour, timeinfo.wMinute, it->second.nStartHour, it->second.nStartMin, it->second.ntaskName);
				}
				else if (it->second.nJGTimes[0] > 0 && (it->second.nState == 2 || it->second.nState == 0) && rawtime >= it->second.nOpenLoop) //每隔多少分钟执行一次
				{//单独间隔时间大于零的 持续多长时间 ko set thoi gian bat dau, ket thuc, chi set interval
					//timeinfo = localtime(&rawtime); //当前时间
					// strftime(pCgm->nCurTime,sizeof(pCgm->nCurTime),"%H:%M:%S",timeinfo); //"%Y:%m:%d %H:%M:%S"
					sprintf(pCgm->nCurTime, "%02d:%02d:%02d", timeinfo.wHour, timeinfo.wMinute, timeinfo.wSecond);
					it->second.nCloseLoop = rawtime + it->second.nJGTimes[1] * 60; //记录当前的秒数+间隔时间=下次关闭时间
					it->second.nOpenLoop = rawtime + it->second.nJGTimes[0] * 60; //下次开始的时间

					struct tm* Nexttimeinfo;
					Nexttimeinfo = localtime(&it->second.nOpenLoop); //当前时间
					strftime(pCgm->nNextTime, sizeof(pCgm->nNextTime), "%H:%M:%S", Nexttimeinfo);
					it->second.nState = 1;
					it->second._colseLoop = 0;

					snprintf(pCgm->nPath, sizeof(pCgm->nPath), "%s", it->second.nLuaField);
					g_ChannelMgr.BroadToGameServer(buffer, sizeof(GS_TASK_GROUPMAN));
					rTRACE("第(%d)个执行成功,隔:%d分,名称:%s,cTime:%s,nTime:%s \n", it->second.nIdx, it->second.nJGTimes[0], it->second.ntaskName, pCgm->nCurTime, pCgm->nNextTime);
				}
				else if (it->second.nStartHour == 0 && it->second.nStartMin == 0 && it->second.nJGTimes[0] == 0 && it->second.IsYes == 0)
				{//时间都等于0的 就是开始就执行一次后就不执行了 
					it->second.IsYes++;
					it->second._colseLoop = 0;
					snprintf(pCgm->nPath, sizeof(pCgm->nPath), "%s", it->second.nLuaField);
					g_ChannelMgr.BroadToGameServer(buffer, sizeof(GS_TASK_GROUPMAN));
					rTRACE("第(%d)个执行成功,服务器开机运行一次,名称:%s\n", it->second.nIdx, it->second.ntaskName);
				}
				else
				{
					if (it->second.nJGTimes[0] > 0 && rawtime > 60 && it->second.nOpenLoop == 60)
					{
						// timeinfo = localtime(&rawtime); 
						// strftime(pCgm->nCurTime,sizeof(pCgm->nCurTime),"%H:%M:%S",timeinfo); //"%Y:%m:%d %H:%M:%S"
						sprintf(pCgm->nCurTime, "%02d:%02d:%02d", timeinfo.wHour, timeinfo.wMinute, timeinfo.wSecond);
						it->second.nCloseLoop = rawtime + it->second.nJGTimes[1] * 60; //记录当前的秒数+间隔时间=下次关闭时间
						it->second.nOpenLoop = rawtime + it->second.nJGTimes[0] * 60;
						it->second.nState = 0;
						it->second._colseLoop = 0;

						struct tm* Nexttimeinfo;
						Nexttimeinfo = localtime(&it->second.nOpenLoop); //当前时间
						strftime(pCgm->nNextTime, sizeof(pCgm->nNextTime), "%H:%M:%S", Nexttimeinfo);
						//将秒数转换为年月日时分秒
						// sprintf( pCgm->nNextTime,"%02d:%02d:%02d",timeinfo.wHour,timeinfo.wMinute,timeinfo.wSecond);
						snprintf(pCgm->nPath, sizeof(pCgm->nPath), "%s", it->second.nLuaField);
						g_ChannelMgr.BroadToGameServer(buffer, sizeof(GS_TASK_GROUPMAN));
						rTRACE("第(%d)个<新>执行成功,隔:%d分,名称:%s,cTime:%s,nTime:%s \n", it->second.nIdx, it->second.nJGTimes[0], it->second.ntaskName, pCgm->nCurTime, pCgm->nNextTime);
					}
				}
			}//END IF
		}//END FOR
	}
}


BOOL gDoLocalScriptSyn()
{
	if (_oPentask)
	    dotask();
//	rTRACE("--------------）执行S3脚本引擎成功----------------");
    //int nCount=gGetPrivateProfileIntEx("List", "Count", "Settings//Task//tasklist.ini", 0);
	//BYTE buffer[max_packagesize];
	//CHAT_GROUPMAN* pCgm = (CHAT_GROUPMAN*)buffer;
	//pCgm->ProtocolType = chat_coretask;
	//pCgm->wChatLength  = 200;
	//pCgm->byHasIdentify= 0;
	//g_ChannelMgr.BroadToGameServer(buffer,sizeof(CHAT_GROUPMAN));
	////rTRACE("--------------执行S3脚本chat_coretask成功----------------");

	//pCgm->ProtocolType = chat_caoritemtime;
	//pCgm->wChatLength  = 201;
	//pCgm->byHasIdentify= 0;

	//g_ChannelMgr.BroadToGameServer(buffer,sizeof(CHAT_GROUPMAN));
	//rTRACE("--------------执行S3脚本chat_caoritemtime成功----------------");
    return 	TRUE;
}

BOOL gTongCityAndAcctackSyn()
{
		//try
		//{
		//	g_cTongSet.SynTongWarInfo();		
		//	g_cTongSet.SynWarCityInfo();  //同步七城信息
		//	g_cTongSet.SynAttackInfo();

		//	//rTRACE("--------------同步帮战七城,帮战信息数据成功----------------");
		//}
		//catch (...)
		//{
		//	rTRACE("--------------同步帮战七城信息数据异常----------------");
		//	return FALSE;
		//}

		return TRUE;
}


// Global.h: interface for the CGlobal class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GLOBAL_H__96341752_5601_43CD_8D7B_F05C99266EA2__INCLUDED_)
#define AFX_GLOBAL_H__96341752_5601_43CD_8D7B_F05C99266EA2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/////////////////////////////////////////////


#include <vector>

/////////////////////////////////////////////


#define DUMMY_AUTOLOCKREAD(lock)			0
#define DUMMY_AUTOLOCKREAD_(name, lock)		0

#define DUMMY_AUTOLOCKWRITE(lock)			0
#define DUMMY_AUTOLOCKWRITE_(name, lock)	0


/////////////////////////////////////////////
#include "../../../headers/ServerPort.h"

#include "../../../headers/KProtocol.h"
#include "../../../headers/KProtocolDef.h"
#include "../../../headers/KGmProtocol.h"
#include "../../../headers/KRelayProtocol.h"
#include "../../../headers/KTongProtocol.h"

#include "Memory.h"

#include "HeavenLib.h"
#include "RainbowLib.h"

#include "SockThread.h"

#include "RootCenter.h"
#include "RootClient.h"
#include "RelayCenter.h"
#include "RelayServer.h"
#include "GatewayCenter.h"
#include "GatewayClient.h"
#include "HostConnect.h"
#include "HostServer.h"
#include "TongServer.h"
#include "ChatServer.h"
#include "ChannelMgr.h"
#include "FriendMgr.h"
#include "DealRelay.h"
#include "DBConnect.h"
#include "KTongSet.h"
#include "TongDB.h"


/////////////////////////////////////////////////最大包的限制
const size_t max_packagesize = 1024 - 32;
const DWORD breathe_interval = 1;


const int local_adapt = 0;
const int global_adapt = 1;

const char file_warcfg[]	= "citywarset.ini";

const char sec_count[]		= "AreaCount";

const char key_name[]	    = "name";
const char key_idx[]		= "idx";
const char key_mapidx[]		= "mapidx";
const char key_tonname[]	= "tongmaster";
const char key_tongmaster[]	= "mastername";
const char key_level[]		= "levle";
const char key_shuis[]		= "shushou";

/////////////////////////////////////////////////

extern CMemory g_memory;

extern CHeavenLib g_libHeaven;
extern CRainbowLib g_libRainbow;


extern CSockThread g_threadSock;

//extern CTongConnect g_TongConn;

extern CRelayCenter g_RelayCenter;
extern CRelayServer g_RelayServer;

extern CRootCenter g_RootCenter;
extern CRootClient g_RootClient;

extern CGatewayCenter g_GatewayCenter;
extern CGatewayClient g_GatewayClient;

//extern CHostConnect g_HostConnect;
//extern CHostConnect m_sHostConnect;
extern CHostServer g_HostServer;
extern CTongServer g_TongServer;
extern CChatServer g_ChatServer;

extern CChannelMgr g_ChannelMgr;
extern CFriendMgr g_FriendMgr;

extern CDBCenter g_DBCenter;
extern CDBClient g_DBClient;

extern CTongSet g_cTongSet;

extern CTongDB	g_cTongDB;

/////////////////////////////////////////////////
//---------------------------------------------------------------------------
#define IM 15
#define IA 10
#define IC 150
//---------------------------------------------------------------------------
static UINT nRandomSeed = 42;


#ifndef MAX_PLAYER_PER_ACCOUNT
#define MAX_PLAYER_PER_ACCOUNT 3
#endif

/////////////////////////////////////////////////
UINT s_RandomS3(UINT nMax);
int GetRandomNumberS3(int nMin, int nMax);

BOOL gTongCityAndAcctackSyn();
BOOL gDoLocalScriptSyn();
BOOL gDoLocalItemDataSyn();


std::_tstring gGetLocateDirectory();

std::_tstring gGetPrivateProfileStringEx(LPCSTR lpAppName, LPCSTR lpKeyName, LPCSTR lpFileName, LPCSTR lpDefault = _T(""));
INT gGetPrivateProfileIntEx(LPCSTR lpAppName, LPCSTR lpKeyName, LPCSTR lpFileName, INT nDefault = -1);
TCHAR gGetPrivateProfileCharEx(LPCSTR lpAppName, LPCSTR lpKeyName, LPCSTR lpFileName, TCHAR chDefault = 0);

BOOL gWritePrivateProfileIntEx(LPCSTR lpAppName, LPCSTR lpKeyName, LPCSTR lpFileName, INT nDefault);
BOOL gWritePrivateProfileStringEx(LPCSTR lpAppName, LPCSTR lpKeyName, LPCSTR lpFileName, LPCSTR lpDefault);

const std::vector<std::_tstring>& gGetPrivateProfileSectionEx(LPCTSTR lpAppName, LPCTSTR lpFileName);
const std::vector<std::_tstring>& gGetPrivateProfileSectionNamesEx(LPCTSTR lpFileName);


BOOL gInitialize();
BOOL gUninitialize();

BOOL gIsLocalIP(DWORD ip);

const UCHAR* gGetHostMac(int adapt, size_t* pSize = NULL);
DWORD gGetHostIP(int adapt);

BOOL gConnectToSibling(const char* addr);

size_t gFillClntwayPckg(DWORD nameid, unsigned long param, 
						const RELAY_ASKWAY_DATA* pAskWay, size_t sizeAskWay, RELAY_ASKWAY_DATA* pClntWay, size_t sizeClntWay);
size_t gFillLosewayPckg(DWORD nToIP, DWORD nToRelayID, 
					   const void* pLosePckg, size_t sizeLosePckg, void* pLoseway, size_t sizeLoseway);

BOOL gIsLegalString(LPCTSTR sz, size_t lenlolmt, size_t lenhilmt);

extern DWORD g_String2Id(LPCSTR pString);
extern DWORD g_String2IdA(LPCSTR pString);



enum ONECLIENT {oneclient_root, oneclient_gateway, oneclient_db};

BOOL gOnOneClientCreate(ONECLIENT ct);
BOOL gOnOneClientClose(ONECLIENT ct);
BOOL gReOneClient(ONECLIENT ct);


BOOL gConsignClientSD(IClient* pClient);


BOOL gStartFriendTimingUpdateDB(UINT elapse);
BOOL gStopFriendTimingUpdateDB();
BOOL gFriendTimingUpdateDB();

void dotask();
bool LoadTaskInfo();
void gSetTakkOpen(bool ver);
#endif // !defined(AFX_GLOBAL_H__96341752_5601_43CD_8D7B_F05C99266EA2__INCLUDED_)

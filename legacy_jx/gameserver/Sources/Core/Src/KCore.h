//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KCore.h
// Date:	2000.08.08
// Code:	Daphnis Wang
// Desc:	Header File
//---------------------------------------------------------------------------
#ifndef KCore_H
#define KCore_H
#include <winsock2.h>
#include <windows.h>
//---------------------------------------------------------------------------
#pragma warning (disable: 4512)
#pragma warning (disable: 4786)
#pragma warning (disable: 4244)
//---------------------------------------------------------------------------
#ifndef MAX
#define MAX(x, y)					((x) > (y) ? (x) : (y))
#define MIN(x, y)					((x) < (y) ? (x) : (y))
#endif

#ifndef ZeroStruct
#define ZeroStruct(buf)					ZeroMemory(&(buf), sizeof(buf))
#endif

//---------------------------------------------------------------------------
#ifdef _STANDALONE
#define CORE_API
#else
#ifdef CORE_EXPORTS
#define CORE_API __declspec(dllexport)
#else
#define CORE_API __declspec(dllimport)
#endif
#endif
//---------------------------------------------------------------------------
#define	DIR_DOWN		0
#define	DIR_LEFTDOWN	1
#define	DIR_LEFT		2
#define	DIR_LEFTUP		3
#define	DIR_UP			4
#define	DIR_RIGHTUP		5
#define	DIR_RIGHT		6
#define	DIR_RIGHTDOWN	7
//---------------------------------------------------------------------------
#include "KWin32.h"
#include "KDebug.h"
#include <unordered_map>
#include "KMemBase.h"
#include "KStrBase.h"
#ifndef _SERVER
#include "KSpriteCache.h"
#include "KFont.h"
#endif
////////后来加的//////////////
#include <omp.h>
#include "KEngine.h"
#include "KTabFileCtrl.h"
#include "KIniFile.h"
#include "S3PDBConVBC.h"
#include "S3PDB_MSSQLServer_Connection.h"
#include "../../Engine/src/Text.h"
#include "../../Engine/src/KLogFile.h" 
//////////////////////////////
#include "KTabFile.h"
#include "KProtocol.h"
#include "KScriptList.h"
#include "KScriptCache.h"
#include "KSkillManager.h"
#include "MyAssert.H"
#include "KLogFile.h"
#include "DBTableS.h"
#include "KGameglobalSet.h"
class ISkill ;
#ifdef _SERVER
#ifdef _STANDALONE
#include "IServer.h"
#else
#include "../../../Headers/IServer.h"
#include "../../../Headers/IClient.h"
#include "..\..\MultiServer\Common\Buffer.h"
#endif
#else
#include "../../../Headers/IClient.h"
#include "KMusic.h"
#include "KSoundCache.h"
#endif

enum GAME_MAP_REF
{
	GAME_MAP_REF_NOR,
	GAME_MAP_REF_CLASS,
};


enum ITEM_MAGIC_LOAD_KIND
{
	MAGIC_LOAD_KIND_NOR,
	MAGIC_LOAD_KIND_LEVEL,
};


struct TDBactivitydata
{
	int  iid;           //编号idx 1开始
	int  nMapIndex;     //地图编号
	int  nIsOwer;       //是否给占领
	unsigned int nBeOwerTime; //最后给占领的时间
	char nOwerName[NAME_LENGHT]; //角色名
	char nNameKey[NAME_LENGHT];     //活动的名称
	char nOwerTongName[NAME_LENGHT];//所在帮会的名称
};

struct TDBShopItemData
{
	int iid;        //编号idx 1开始
	int iequipgen;  //
	int idetailtype;
	int iparticulartype;
	char itemName[64];//物品的名称
	int ilevel;     //等级
	int iseries;    //武行
	UINT irandseed; //随机种子
	int iparam1;
	int iparam2;    //蓝装
	int iparam3;
	int iparam4;
	int iparam5;
	int iparam6;
	int iparamr1;
	int iparamr2;   //熔炼
	int iparamr3;
	int iparamr4;
	int iparamr5;
	int iparamr6;
	int ilucky;   //装备的幸运值
	int igoldid;  //是否黄金
	int	istacknum;//叠加数量
	int ienchance;//加成
	BYTE ipoint;  //是否紫装 7为特殊装 8 为随机黄金   1-6 为紫装
	UINT addtime; //开始时间
	UINT endtime; //结束时间
	BYTE iPriceKind;   //货币类型
	int  iSellPrice;   //售卖价格
	BYTE iRongpiont;   //是否熔炼
	char iWonName[NAME_LENGHT]; //挂售者
	char iaccName[ACC_LENGTH]; //帐号
	int iswhere;    //是否洗练装
	int isplatina;  //是否白金
	UINT isSellOut; //是否已经卖出
	int iparamj1;   //(保留使用)是否叠加
	int iparamj2;   //是否过期
	UINT iparamj3;   //取回密码
	int iparamj5;
	int iparamj4;    //是否已经兑换了收入
	int iparamj6;
	int iparamj7;
};

struct sKillStateData
{
	int m_NpcIndex;
	bool m_IsMoreAura;
	int m_ActiveAuraID;
	int m_ActiveAuraIndex;
	int m_ActiveXinYunXingID;
	int nLevel;
	int nMainLevel;
	int nEnChance;
	int m_SubWorldIndex;
	int m_RegionIndex;
	int m_isModel;
};
//#define TASKCONTENT
//#define		BMPMAPDEBUG
extern int   nGameVersion;
extern int   nExpSkillIdx;
extern int   nLuckySkillIdx;
extern int   nKangkillIdx;
extern int   nYaoPinSkillIdx;
extern int   nSupManaMauSkillIdx;
extern int   nSupKyNangSkillIdx;
extern int   nNhatKySkillIdx;

typedef std::unordered_map<int, TDBShopItemData> _ItemDataInfo;
typedef std::unordered_map<std::string, std::string> _StrNpcInfo;
typedef std::unordered_map<int, std::string> _StrObjInfo;
typedef std::unordered_map<std::string, std::string> _StrCoreInfo;
typedef std::unordered_map<std::string, int> _timeNpcInfo;
typedef std::unordered_map<std::string, sKillStateData> _stateNpcInfo;
//---------------------------------------------------------------------------
#ifdef TOOLVERSION
extern CORE_API int g_ScreenX;
extern CORE_API int g_ScreenY;
#endif

#define ITOA(NUMBER)  #NUMBER

#define __TEXT_LINE__(LINE) ITOA(LINE)

//#ifdef _DEBUG
#define ATTENTION(MSG) __FILE__"("__TEXT_LINE__(__LINE__)") : ★ATTENTION★ → "#MSG
//#else
//#define ATTENTION(MSG) __FILE__"("__TEXT_LINE__(__LINE__)") : ATTENTION!! error: "#MSG
//#endif
//#define	NET_DEBUG

extern  CORE_API _ItemDataInfo  iItemDataInfo;
extern  CORE_API _StrNpcInfo    strNpcInfo;
extern  CORE_API _StrObjInfo    strObjInfo;
extern  CORE_API _StrCoreInfo   strCoreInfo;
extern  CORE_API _timeNpcInfo   timeNpcInfo;
extern  CORE_API _stateNpcInfo  stateNpcInfo;
extern CORE_API	KTabFile		g_OrdinSkillsSetting, g_MisslesSetting,g_NpcChangeName,g_TaskChangeName,g_ItemChangeName,g_SkillChangeName;
extern CORE_API	KTabFile		g_ObjChangeName,g_gmList;//if ( !m_cTabFile.Load(OBJ_DATA_FILE_NAME) )
extern CORE_API	KTabFile		g_SkillLevelSetting,g_CompEquip,g_CompAtlas;
extern CORE_API	KTabFile		g_NpcSetting,g_FsJinMai,g_ForbitMap,g_MoveMapPos,g_PlayerTitle;
extern CORE_API	KTabFile		g_NpcImageSetting,g_ForbitWaiGua,g_GetMsg;
extern CORE_API KTabFile		g_RankTabSetting,g_GMSetting,g_AccoutSet,g_ReBornSetting,g_PlayerVipSetting;
extern CORE_API	KIniFile        g_TaskSetConfig,g_NpcMapDropRate,g_GameSetTing, g_GameOption;
extern CORE_API	KIniFile        g_MagicBaoshiDesc,g_TaskLoop;
extern CORE_API S3PDBConVBC     *g_DataConn;
extern CORE_API S3PDBConVBC     *g_CityConn;
extern  KCLogFile        g_LogFile;
//extern CORE_API S3PDB_MSSQLServer_Connection     S3PDB_MSSQLServer_Connection();

extern CORE_API CPackager	  m_theRecv_p;
extern CORE_API CPackager  m_theSend_p;
BOOL CreateConnectio2n(S3PDBConVBC** ppInfo);

#ifdef TOOLVERSION
#ifndef __linux
extern CORE_API	KSpriteCache	g_SpriteCache;
#endif
#endif
#ifndef _SERVER
extern KSoundCache		g_SoundCache;
extern KSpriteCache	    g_SpriteCache;  // 原来没有的
extern KMusic			*g_pMusic;

extern unsigned int	* g_pAdjustColorTab;
extern unsigned int g_ulAdjustColorCount;

#endif

/*#ifdef _DEBUG
extern CORE_API BOOL g_bDebugScript;
#endif*/

extern KTabFile	 g_NpcKindFile; //记录Npc人物类型文件

#ifndef _SERVER
extern BOOL g_bUISelIntelActiveWithServer;//当前选择框是否与服务器端交互
extern BOOL g_bUISpeakActiveWithServer;
extern int g_bUISelLastSelCount;

extern CORE_API	KIniFile    g_MapTraffic;
extern CORE_API	KTabFile	cTabFile;

#endif

#ifdef _SERVER
extern int   nCurSerIdx;
extern int   nCurGsNumber;
extern int   nMaxSubWorldCount;
extern int   nMaxPlayerCount;
extern CORE_API int   nMaxNpcCount;
extern CORE_API int   nMaxItemCount;
extern int   nMaxObjCount;
extern int   nMaxMissleCount;
extern int   nSeparateWorldCount;
extern int   nSleepTime;
extern int   nIsOrdered;
extern int   nMaxSellCount;
extern int   nIsOutScript;
extern int   nMapIndex;

extern std::string   gameSetPath;
extern std::string   gameMapSetPath;


extern KLuaScript g_WorldScript;
extern CORE_API ZDBTableS       *g_DbItemFile;
extern CORE_API ZDBTableS       *g_DbActivity;
#endif

/*#ifndef _SERVER
class KImageNode : public KNode
{
public:
	char	m_szFile[32];
	int		m_nFrame;
	int		m_nXpos;
	int		m_nYpos;
};
#endif */

#ifndef _SERVER
extern char* g_GetStringRes(int nStringID, char * szString, int nMaxLen);
#endif

BOOL InitSkillSetting();
BOOL InitMissleSetting();
BOOL InitNpcSetting();
void g_ReleaseCore();

inline int checkGmList(char *roleName)
{
	if (!roleName) return 0;
	int _rows=g_gmList.GetHeight();
    char nName[64];
	for (int i=0;i<_rows;i++)
	{
		ZeroMemory(nName,sizeof(nName));
		g_gmList.GetString(i+1,1,"",nName,sizeof(nName));
		if (nName[0] && g_StrCmp(roleName,nName))
		  return 1;
	}
	return 0;
}

inline int GetRandomNumber(int nMin, int nMax) //-5 -10
{
	int nIsThis=0;

	if (nMin<0 && nMax<0)
	{
		nMin=-nMin;
		nMax=-nMax;
		nIsThis=1;
	}
	/*else if (nMin<0 && nMax>0)
	{

	}*/
	
	if  (nMax - nMin + 1 < 0)
		return 0;

	if  (nIsThis)
	{//负值
	   int nReg=g_Random(nMax - nMin + 1) + nMin;
	       nReg=-nReg;
	   return nReg; //返回负值
	}
	else
	   return g_Random(nMax - nMin + 1) + nMin;
}
int	 PositionToRoom(int P);

#ifdef _SERVER
void g_SetServer(LPVOID pServer);
void g_SetTranServer(LPVOID pServer);
void g_SetTonServer(LPVOID pServer);
void g_SetChatServer(LPVOID pServer);
CORE_API void g_SetDataServer(LPVOID pServer);
void g_SetGateWayServer(LPVOID pServer);

extern CORE_API IServer* g_pServer;
extern CORE_API IClient*	g_pTongClient;
extern CORE_API IClient*	g_pDataClient;
extern IClient* g_pGateWayClient;
extern IClient* g_pChatClient;
extern IClient* g_pTranClient;

#else
void g_SetClient(LPVOID pClient);
extern IClient* g_pClient;
//extern BOOL	g_bPingReply;
#endif
//---------------------------------------------------------------------------
#endif

//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KCore.cpp
// Date:	2000.08.08
// Code:	Daphnis Wang
// Desc:	Core class
//---------------------------------------------------------------------------
#include "KCore.h"
//#include "AWE/OBJPoolT.h"
#include "KEngine.h"
#include "KFilePath.h"
#include <errno.h>
//-------------------后来添加的
#include "S3PDB_MSSQLServer_Connection.h"

#include "KPakList.h"

//-------------------
#ifndef _SERVER
#include "KNpcResList.h"
#include "KBmpFile.h"
#include "ImgRef.h"
#include "../../Represent/iRepresent/iRepresentshell.h"
#endif

#include "KItemChangeRes.h"
#include "KNpcSet.h"
#include "KTabFile.h"
#include "KIniFile.h"    //后来添加的
#include "KSkills.h"
#include "KPlayerSet.h"
#include "KPlayerTeam.h"
#include "KMissleSet.h"
#include "KFaction.h"
#include "KMath.h"
#include "time.h"
#include "KPlayerTask.h"
#include "KSubWorldSet.h"
#include "KItemGenerator.h"
#include "KObjSet.h"
#include "KItemSet.h"
#include "KNpc.h"
#include "KPlayer.h"
#include "KNpcTemplate.h"
#include "CoreUseNameDef.h"
#include "KBuySell.h"
#include "KSortScript.h"
#include "KTaskFuns.h"
#include "TaskDef.h"
#include "LuaFuns.h"
#include "DBTableS.h"
#include "KGameglobalSet.h"



#ifdef _SERVER	
#include <string>
#include <tchar.h>

BOOL CreateConnection(_LPDATABASEINFO pDatabase, S3PDBConVBC** ppInfo);
std::string gGetLocateDirectory();

#endif


#pragma warning (disable: 4512)
#pragma warning (disable: 4786)

/*typedef struct _user
{
	char username[16];
	BYTE byAge;
	BYTE bySex;
	char szAddress[255];
}USER;*/

#ifdef _SERVER
#ifndef _STANDALONE
#include "../../../lib/S3DBInterface.h"
#else
#include "S3DBInterface.h"
#endif
#endif

#ifdef _SERVER
extern int g_WayPointPriceUnit;	//WayPoint表格中价格的单位量，WayPoint价格 = 单位量 * 表格数值
extern int g_StationPriceUnit;	//Station表格中价格的单位量，Station价格 = 单位量 * 表格数值
extern int g_DockPriceUnit;
KPakList	g_PakList;
#endif


#ifndef _SERVER
#define CLIENTWEAPONSKILL_TABFILE		"\\settings\\ClientWeaponSkill.txt"	
KTabFile g_ClientWeaponSkillTabFile;

#define ADJUSTCOLOR_TABFILE				"\\settings\\AdjustColor.txt"	
unsigned int InitAdjustColorTab();

unsigned int * g_pAdjustColorTab = NULL;
unsigned int g_ulAdjustColorCount = 0;

#endif

BOOL InitTaskSetting();


#ifndef _SERVER
#include "KMagicDesc.h"
#include "Scene/KScenePlaceC.h"
BOOL g_bUISelIntelActiveWithServer = FALSE;//当前选择框是否与服务器端交互
BOOL g_bUISpeakActiveWithServer = FALSE;
int	g_bUISelLastSelCount = 0;
extern KTabFile g_StringResourseTabFile;
KTabFile g_RankTabSetting;
#endif

//是否将脚本出错信息输出到文件供调试
//#define DEBUGOPT_SCRIPT_MSG_FILEOUT
#ifdef _SERVER
IServer* g_pServer;
IClient* g_pTongClient= NULL;
IClient* g_pDataClient= NULL;
IClient* g_pGateWayClient= NULL;
IClient* g_pChatClient= NULL;
IClient* g_pTranClient= NULL;

extern KTabFile g_EventItemTab;
extern KTabFile g_MineItemTab;
extern KTabFile g_FusionTab;

KTabFile		g_MoveMapPos;
ZDBTableS       *g_DbItemFile = NULL;
ZDBTableS       *g_DbActivity = NULL;//活动
#else
IClient* g_pClient;
//BOOL	 g_bPingReply;
#endif

//---------------------------------------------------------------------------
#ifdef TOOLVERSION
KSpriteCache	g_SpriteCache;   //spr 控制
#endif

KTabFile		g_OrdinSkillsSetting, g_MisslesSetting,g_PlayerTitle,g_NpcChangeName,g_TaskChangeName,g_ItemChangeName,g_SkillChangeName;
KTabFile		g_SkillLevelSetting,g_FsJinMai,g_ForbitMap,g_ObjChangeName,g_gmList;
KTabFile		g_NpcSetting,g_GMSetting,g_ForbitWaiGua,g_AccoutSet,g_ReBornSetting,g_PlayerVipSetting;
KTabFile		g_NpcKindFile,g_CompEquip,g_CompAtlas,g_GetMsg; //记录Npc人物类型文件
KIniFile        g_TaskSetConfig,g_NpcMapDropRate,g_GameSetTing,g_MagicBaoshiDesc,g_TaskLoop, g_GameOption;
S3PDBConVBC     *g_DataConn;
S3PDBConVBC     *g_CityConn;
//S3PDB_MSSQLServer_Connection     S3PDB_MSSQLServer_Connection();
KCLogFile        g_LogFile;

CPackager	  m_theRecv_p;
CPackager      m_theSend_p;

int				g_nMeleeWeaponSkill[MAX_MELEEWEAPON_PARTICULARTYPE_NUM];
int				g_nRangeWeaponSkill[MAX_RANGEWEAPON_PARTICULARTYPE_NUM];	
int				g_nHandSkill;
int             nGameVersion;//什么版本的GS
_ItemDataInfo   iItemDataInfo;
_StrNpcInfo    strNpcInfo;
_StrObjInfo    strObjInfo;
_StrCoreInfo   strCoreInfo;
_timeNpcInfo   timeNpcInfo;
_stateNpcInfo  stateNpcInfo;
int   nExpSkillIdx;
int   nLuckySkillIdx;
int   nKangkillIdx;
int   nYaoPinSkillIdx;
int   nNhatKySkillIdx;
int   nSupManaMauSkillIdx;
int   nSupKyNangSkillIdx;

#ifndef	_SERVER
KSoundCache		g_SoundCache;
//KSpriteCache	g_SpriteCache;   //spr 控制  ----原来没有的
KMusic			*g_pMusic = NULL;

KIniFile        g_MapTraffic;
KTabFile        cTabFile;
#endif

/*#ifdef _DEBUG
CORE_API BOOL	g_bDebugScript;//When True , Testing Debug, Script Will be ReLoaded EveryTime When to be Executed, Not Using ScriptCach.
#endif */

KLuaScript	*	g_pNpcLevelScript = NULL;
KLuaScript      g_WorldScript;
#ifdef	_SERVER
int   nCurSerIdx;
int   nCurGsNumber;
int   nSeparateWorldCount;
int   nSleepTime;
int   nIsOrdered;
int   nMaxSubWorldCount;
int   nMaxPlayerCount;
CORE_API int   nMaxNpcCount = 150000;
int   nMaxItemCount;
int   nMaxObjCount;
int   nMaxMissleCount;
int   nMaxSellCount;
int   nIsOutScript;
int   nMapIndex;
std::string gameSetPath;
std::string gameMapSetPath;

void KPIGetExePath( LPSTR lpExePath, DWORD dwSize );
int get_accountItem(DB *db, const DBT *pkey, const DBT *pdata, DBT *ikey);
int get_activitydata(DB *db, const DBT *pkey, const DBT *pdata, DBT *ikey);

int get_activitydata(DB *db, const DBT *pkey, const DBT *pdata, DBT *ikey) 
{
	//给定一个完整的buffer，得到account作为索引
	memset(ikey, 0, sizeof(DBT));
	if (pdata->size < sizeof(TDBactivitydata))
		return EINVAL;
	TDBactivitydata *pItemData = (TDBactivitydata *)pdata->data;
	//ikey->data = pRoleData->BaseInfo.caccname;	            //KEY 
	//ikey->size = strlen(pRoleData->BaseInfo.caccname) + 1;	//KEY值
	//char nIdxInfo[32]={0};
	//sprintf(nIdxInfo,"剑侠%04d情缘",pItemData->iid);
	ikey->data = pItemData->nNameKey;
	ikey->size = strlen(pItemData->nNameKey) + 1;

	return 0;
}

int get_accountItem(DB *db, const DBT *pkey, const DBT *pdata, DBT *ikey) 
{
	//给定一个完整的buffer，得到account作为索引
	memset(ikey, 0, sizeof(DBT));
	if (pdata->size < sizeof(TDBShopItemData))
		return EINVAL;
	TDBShopItemData *pItemData = (TDBShopItemData *)pdata->data;
	//ikey->data = pRoleData->BaseInfo.caccname;	            //KEY 
	//ikey->size = strlen(pRoleData->BaseInfo.caccname) + 1;	//KEY值
	//char nIdxInfo[32]={0};
	//sprintf(nIdxInfo,"剑侠%04d情缘",pItemData->iid);
	ikey->data = pItemData->iaccName;
	ikey->size = strlen(pItemData->iaccName) + 1;
	
	return 0;
}

#endif

void g_InitProtocol();
CRITICAL_SECTION rankSection;

//---------------------------------------------------------------------------
CORE_API void g_InitCore(char *nParmName)
{
  srand((unsigned)time(NULL));

  m_theSend_p.Empty();
//---------------------------------------------------------------------------
#ifdef	_SERVER	
	g_PakList.Open("\\package.ini");   //加载服务器的 PAK文件
	// printf("加载Pak列表成功,加载 %d 个PAK文件\n",nPakNum);
	timeNpcInfo.clear();
	stateNpcInfo.clear();
#endif
	char nTempSetPath[128];
	sprintf(nTempSetPath,"\\Settings\\gamesetting_%s.ini",nParmName);
	gameSetPath = nTempSetPath;
	printf("-------load gamesettings:%s -------\n",nTempSetPath);
	if  (g_GameSetTing.Load(nTempSetPath) || g_GameSetTing.Load(GAME_SETTING_FILE))
	{//nCurSerIdx
		_gameGlobalSet.Init();

		g_GameSetTing.GetInteger("SkillsIcon","ExpSkillIdx",0,&nExpSkillIdx);
		g_GameSetTing.GetInteger("SkillsIcon","LuckySkillIdx",0,&nLuckySkillIdx);
		g_GameSetTing.GetInteger("SkillsIcon","KangkillIdx",0,&nKangkillIdx);
		g_GameSetTing.GetInteger("SkillsIcon","YaoPinSkillIdx",0,&nYaoPinSkillIdx);
		g_GameSetTing.GetInteger("SkillsIcon","NhatKySkillIdx",0,&nNhatKySkillIdx);
		g_GameSetTing.GetInteger("SkillsIcon","SupManaMauSkillIdx",0,&nSupManaMauSkillIdx);
		g_GameSetTing.GetInteger("SkillsIcon","SupKyNangSkillIdx",0,&nSupKyNangSkillIdx);

		g_GameSetTing.GetInteger("Gameconfig","GameVersion",1,&nGameVersion);//1为中文版本 2 其他版本 需要替换字符串的
#ifdef	_SERVER	
		nIsOrdered = 0;	
		nSeparateWorldCount =10;
		//g_GameSetTing.GetInteger("Gameconfig","CoreIsOrdered",0,&nIsOrdered);
		//g_GameSetTing.GetInteger("Gameconfig","SeparateWorldCount",10,&nSeparateWorldCount);
		g_GameSetTing.GetInteger("Gameconfig","MaxSubWorldCount",0,&nMaxSubWorldCount);
		if (nMaxSubWorldCount <= 1) g_GameSetTing.GetInteger("ServerConfig","MaxSubWorldCount",1000,&nMaxSubWorldCount);
		extern BOOL g_bPositionSyncFixEnabled;
		extern BOOL g_bSpawnDefinitionSplitEnabled;
		g_GameSetTing.GetInteger("Gameconfig", "SpawnDefinitionSplit", 0, &g_bSpawnDefinitionSplitEnabled);
		g_GameSetTing.GetInteger("ServerConfig", "SpawnDefinitionSplit", g_bSpawnDefinitionSplitEnabled, &g_bSpawnDefinitionSplitEnabled);
		extern BOOL g_bLazyMapLoadingEnabled;
		g_GameSetTing.GetInteger("Gameconfig", "LazyMapLoading", 0, &g_bLazyMapLoadingEnabled);
		g_GameSetTing.GetInteger("ServerConfig", "LazyMapLoading", g_bLazyMapLoadingEnabled, &g_bLazyMapLoadingEnabled);
		extern BOOL g_bNpcActivationEnabled;
		g_GameSetTing.GetInteger("Gameconfig", "NpcActivation", 0, &g_bNpcActivationEnabled);
		g_GameSetTing.GetInteger("ServerConfig", "NpcActivation", g_bNpcActivationEnabled, &g_bNpcActivationEnabled);
		extern BOOL g_bSafeUnloadEnabled;
		extern DWORD g_dwMapIdleFrames;
		int nSafeUnload = 0;
		int nMapIdleSeconds = 300;
		g_GameSetTing.GetInteger("Gameconfig", "SafeUnload", 0, &nSafeUnload);
		g_GameSetTing.GetInteger("ServerConfig", "SafeUnload", nSafeUnload, &nSafeUnload);
		g_bSafeUnloadEnabled = (nSafeUnload != 0);
		g_GameSetTing.GetInteger("Gameconfig", "MapIdleSeconds", 300, &nMapIdleSeconds);
		g_GameSetTing.GetInteger("ServerConfig", "MapIdleSeconds", nMapIdleSeconds, &nMapIdleSeconds);
		if (nMapIdleSeconds < 10) nMapIdleSeconds = 10;
		g_dwMapIdleFrames = (DWORD)(nMapIdleSeconds * 18);
		extern int g_nMaxConcurrentLoads;
		extern int g_nMaxLoadedMapsBudget;
		g_GameSetTing.GetInteger("Gameconfig", "MaxConcurrentLoads", 2, &g_nMaxConcurrentLoads);
		g_GameSetTing.GetInteger("ServerConfig", "MaxConcurrentLoads", g_nMaxConcurrentLoads, &g_nMaxConcurrentLoads);
		g_GameSetTing.GetInteger("Gameconfig", "MaxLoadedMaps", 64, &g_nMaxLoadedMapsBudget);
		g_GameSetTing.GetInteger("ServerConfig", "MaxLoadedMaps", g_nMaxLoadedMapsBudget, &g_nMaxLoadedMapsBudget);
		g_GameSetTing.GetInteger("Gameconfig","PositionSyncFix",1,&g_bPositionSyncFixEnabled);
		g_GameSetTing.GetInteger("ServerConfig","PositionSyncFix",g_bPositionSyncFixEnabled,&g_bPositionSyncFixEnabled);
		//int nThreads=0;
		//g_GameSetTing.GetInteger("Gameconfig","CoreThreads",0,&nThreads);
		//if (nMaxSubWorldCount>=nSeparateWorldCount)
		g_GameSetTing.GetInteger("SYSTEM","ServerIdx",1,&nCurSerIdx);
		g_GameSetTing.GetInteger("SYSTEM","GsNumBer",1,&nCurGsNumber);
		
		g_GameSetTing.GetInteger("Gameconfig","CoreSleepTime",500,&nSleepTime);
		g_GameSetTing.GetInteger("Gameconfig","MaxPlayerCount",0,&nMaxPlayerCount);
		if (nMaxPlayerCount <= 1) g_GameSetTing.GetInteger("ServerConfig","MaxPlayerCount",500,&nMaxPlayerCount);
		g_GameSetTing.GetInteger("Gameconfig","MaxNpcCount",0,&nMaxNpcCount);
		if (nMaxNpcCount <= 1) g_GameSetTing.GetInteger("ServerConfig","MaxNpcCount",150000,&nMaxNpcCount);
		g_GameSetTing.GetInteger("Gameconfig","MaxItemCount",0,&nMaxItemCount);
		if (nMaxItemCount <= 1) g_GameSetTing.GetInteger("ServerConfig","MaxItemCount",200000,&nMaxItemCount);
		g_GameSetTing.GetInteger("Gameconfig","MaxObjCount",0,&nMaxObjCount);
		if (nMaxObjCount <= 1) g_GameSetTing.GetInteger("ServerConfig","MaxObjCount",20000,&nMaxObjCount);
		g_GameSetTing.GetInteger("Gameconfig","MaxMissleCount",0,&nMaxMissleCount);
		if (nMaxMissleCount <= 1) g_GameSetTing.GetInteger("ServerConfig","MaxMissleCount",100000,&nMaxMissleCount);
		g_GameSetTing.GetInteger("Item","MaxSellCount",0,&nMaxSellCount);
		g_GameSetTing.GetInteger2("Gameconfig","OutScriptWithMap",&nIsOutScript,&nMapIndex);
		printf("-------SeparateWorldCount:%d, ID skill: %d,%d,%d,%d,%d,%d,%d -------\n",nSeparateWorldCount,nExpSkillIdx,nLuckySkillIdx,nKangkillIdx,nYaoPinSkillIdx,nNhatKySkillIdx,nSupManaMauSkillIdx,nSupKyNangSkillIdx);
		g_ObjChangeName.Load("\\settings\\objdata.txt");
#endif
	}
	else
	{
		printf("--Failed to load configuration file:%s\n",GAME_SETTING_FILE);
		return;
	}

	if (g_GameOption.Load(GAME_OPTION_FILE))
	{

	}
	else
	{
		printf("--Can not load file: %s\n", GAME_OPTION_FILE);
		return;
	}

//---------------------------------------------------------------------------加载替换文件
	KTabFile nNpcInfo;
	if (nNpcInfo.Load("\\settings\\lang\\zh\\replacename_npc.txt"))
	{
		int  nRows = nNpcInfo.GetHeight()+1;

		for(int i=2;i<nRows;i++)
		{
		   char nyNpcName[64]={0},ndNpcName[64]={0};
           nNpcInfo.GetString(i,"sourcename","no npc name",nyNpcName,sizeof(nyNpcName));
		   nNpcInfo.GetString(i,"targetname","no npc name",ndNpcName,sizeof(ndNpcName));
		  // strNpcInfo[nyNpcName]=ndNpcName;
		   strNpcInfo.insert(make_pair(nyNpcName,ndNpcName));
		}
		nNpcInfo.Clear();
		//printf("----Loaded file \\settings\\lang\\zh\\replacename_npc.txt--\n");
	}

	if (nNpcInfo.Load("\\settings\\lang\\zh\\replacename_obj.txt"))
	{
		int  nRows = nNpcInfo.GetHeight()+1;

		for(int i=2;i<nRows;i++)
		{
			char ndNpcName[64]={0};//nyNpcName[64]={0},
			//nNpcInfo.GetString(i,"sourcename","no npc name",nyNpcName,sizeof(nyNpcName));
			nNpcInfo.GetString(i,"targetname","no npc name",ndNpcName,sizeof(ndNpcName));
			//strObjInfo[i]=ndNpcName;
			 strObjInfo.insert(make_pair(i,ndNpcName));
		}
	  nNpcInfo.Clear();
	}

	if (nNpcInfo.Load("\\settings\\lang\\zh\\stringtable_core.txt"))
	{
		int  nRows = nNpcInfo.GetHeight()+1;

		for(int i=2;i<nRows;i++)
		{
			char nyNpcName[64]={0},ndNpcName[128]={0};
			nNpcInfo.GetString(i,"key","no npc name",nyNpcName,sizeof(nyNpcName));
			nNpcInfo.GetString(i,"value","no npc name",ndNpcName,sizeof(ndNpcName));
			strCoreInfo.insert(make_pair(nyNpcName,ndNpcName));
		}
		nNpcInfo.Clear();
	}

//---------------------------------------------------------------------------

#ifndef	_SERVER	
	CHAR *szRandomMem1, *szRandomMem2, *szRandomMem3;	//这些变量没有应用价值，主要是防外挂找固定内存
#endif
	/*
	if  (SubWorld->m_suoHun)
	{
		printf("---test 生成 DUMP 文件\n");
	}
	*/
	SubWorld = new KSubWorld[MAX_SUBWORLD];
	//printf("---[内存]KSubWorld初始化:%ld * %ld = %ld mb\n",sizeof(KSubWorld),MAX_SUBWORLD,MAX_SUBWORLD * sizeof(KSubWorld)/(1024*1024));
	//ADD_CODEPOINT(SubWorld);
#ifndef	_SERVER	
	szRandomMem1 = new CHAR[((rand() % 64) + 6) * 1024];
#endif

	Player = new KPlayer[MAX_PLAYER];
	printf("---[Memory] KPlayer initialization:%zu * %d = %zu mb!\n",sizeof(KPlayer),MAX_PLAYER,MAX_PLAYER*sizeof(KPlayer)/(1024*1024));
	//ADD_CODEPOINT(Player);
#ifndef	_SERVER	
	szRandomMem2 = new CHAR[((rand() % 64) + 6) * 1024];
#endif

	Npc = new KNpc[MAX_NPC];
	printf("---[Memory] KNpc initialization:%zu * %d = %zu mb!\n",sizeof(KNpc),MAX_NPC,MAX_NPC*sizeof(KNpc)/(1024*1024));

	//ADD_CODEPOINT(Npc);
#ifndef	_SERVER		
	szRandomMem3 = new CHAR[((rand() % 64) + 6) * 1024];
#endif

	Item = new KItem[MAX_ITEM];
	printf("---[Memory] KItem initialization:%zu * %d = %zu mb!\n",sizeof(KItem),MAX_ITEM,sizeof(KItem)*MAX_ITEM/(1024*1024));
	//ADD_CODEPOINT(Item);
	
	Object = new KObj[MAX_OBJECT];
	printf("---[Memory] KObj initialization:%zu * %d = %zu mb!\n",sizeof(KObj),MAX_OBJECT,MAX_OBJECT*sizeof(KObj)/(1024*1024));

	//ADD_CODEPOINT(Object);
	
	Missle = new KMissle[MAX_MISSLE];
	printf("---[Memory] KMissle initialization:%zu * %d = %zu mb!\n",sizeof(KMissle),MAX_MISSLE,MAX_MISSLE*sizeof(KMissle)/(1024*1024));

	//ADD_CODEPOINT(Missle);
#ifndef	_SERVER	
	if (szRandomMem1)
	{
		delete[] szRandomMem1;
		szRandomMem1 = NULL;
	}
	
	if (szRandomMem2)
	{
		delete[] szRandomMem2;
		szRandomMem2 = NULL;
	}
	
	if (szRandomMem3)
	{
		delete[] szRandomMem3;
		szRandomMem3 = NULL;
	}
#endif

	_ASSERT(SubWorld && Player && Npc && Item && Object && Missle);

	printf("MaxSubWorldCount = %d\tMaxPlayerCount = %d\tMaxNpcCount = %d\n", MAX_SUBWORLD, MAX_PLAYER,MAX_NPC);
	printf("MaxItemCount = %d\tMaxObjCount = %d\n", MAX_ITEM, MAX_OBJECT);
	printf("MaxPlayerLevel = %d\tMaxNpcLevel = %d\n", MAX_LEVEL, MAX_NPC_LEVEL);
	printf("MaxSkillCount = %d\tMaxSkillLevel = %d\tMaxMissileCount = %d\n", MAX_NPCSKILL, MAX_SKILLLEVEL, MAX_MISSLE);
	printf("=========================================================\n");
//---------------------------------------------------------------------------
	g_InitProtocol();

#ifndef __linux
	g_RandomSeed(42);
#else
	g_RandomSeed(42);	   //设置一个随机种子	clock()
#endif

#ifdef _SERVER
	time_t ltime;
	time(&ltime);
	printf("Starting Core......%s\n", ctime(&ltime));
	printf("==========================================\n");
	
	//开始创建DB数据库
	//g_DbItemFile = new ZDBTableS("itemdata","itemdb");  //新建内存
	//int nIsGo=0;
	//if  (g_DbItemFile)
	//{

	//	if (g_DbItemFile->addIndex(get_accountItem)>-1 )//设置索引的类型
	//	{
	//        g_DbItemFile->open();
	//		nIsGo = 1;
	//	    printf("---[拍卖行]初始化成功!---\n");
	//	}
	//	else
	//		printf("---[拍卖行]数据库打开失败!---\n");
	//}
	//else
	//{
	//	printf("---[拍卖行]初始化失败!---\n");
	//}
	////开始给节点添加数据　
	//int m_ItemShopCount = 0;
	//if (nIsGo==1 && g_DbItemFile)
	//{//nMaxSellCount
	//	
	//	ZCursorS *cursor = g_DbItemFile->first();
	//	while(cursor)
	//	{	
	//		TDBShopItemData* pItemData = (TDBShopItemData*)cursor->data;
	//		if (pItemData && pItemData->iid>0)
	//		{
	//			int i = pItemData->iid;
	//			iItemDataInfo[i].iid =pItemData->iid;
	//			iItemDataInfo[i].iequipgen= pItemData->iequipgen;
	//			iItemDataInfo[i].idetailtype= pItemData->idetailtype;
	//			iItemDataInfo[i].iparticulartype= pItemData->iparticulartype;
	//			sprintf(iItemDataInfo[i].itemName,pItemData->itemName);
	//			iItemDataInfo[i].ilevel= pItemData->ilevel;     //等级
	//			iItemDataInfo[i].iseries= pItemData->iseries;    //武行
	//			iItemDataInfo[i].irandseed= pItemData->irandseed; //随机种子
	//			iItemDataInfo[i].iparam2= pItemData->iparam2;    //蓝装
	//			iItemDataInfo[i].iparam3= pItemData->iparam3;
	//			iItemDataInfo[i].iparam5=pItemData->iparam5;
	//			iItemDataInfo[i].iparam4= pItemData->iparam4;
	//			iItemDataInfo[i].iparam6=  pItemData->iparam6;
	//			iItemDataInfo[i].iparam1= pItemData->iparam1;
	//			iItemDataInfo[i].iparamr2= pItemData->iparamr2;   //熔炼
	//			iItemDataInfo[i].iparamr3= pItemData->iparamr3;
	//			iItemDataInfo[i].iparamr5= pItemData->iparamr5;
	//			iItemDataInfo[i].iparamr4= pItemData->iparamr4;
	//			iItemDataInfo[i].iparamr6= pItemData->iparamr6;
	//			iItemDataInfo[i].iparamr1= pItemData->iparamr1;
	//			iItemDataInfo[i].ilucky=  pItemData->ilucky;   //装备的幸运值
	//			iItemDataInfo[i].igoldid=  pItemData->igoldid;  //是否黄金
	//			iItemDataInfo[i].istacknum=  pItemData->istacknum;//叠加数量
	//			iItemDataInfo[i].ienchance=  pItemData->ienchance;//加成
	//			iItemDataInfo[i].ipoint= pItemData->ipoint;  //是否紫装 7为特殊装 8 为随机黄金   1-6 为紫装
	//			iItemDataInfo[i].addtime= pItemData->addtime; //开始时间
	//			iItemDataInfo[i].endtime=  pItemData->endtime; //结束时间
	//			iItemDataInfo[i].iPriceKind=  pItemData->iPriceKind;   //货币类型
	//			iItemDataInfo[i].iSellPrice=  pItemData->iSellPrice;   //售卖价格
	//			iItemDataInfo[i].iRongpiont=  pItemData->iRongpiont;   //是否熔炼
	//			sprintf(iItemDataInfo[i].iWonName,pItemData->iWonName); //挂售者
	//			sprintf(iItemDataInfo[i].iaccName,pItemData->iaccName); //二级查询　KEY
	//			iItemDataInfo[i].iswhere=  pItemData->iswhere;      //是否洗练装
	//			iItemDataInfo[i].isplatina=  pItemData->isplatina;  //是否白金
	//			iItemDataInfo[i].isSellOut=  pItemData->isSellOut;  //是否已经卖出 卖出的时间
	//			iItemDataInfo[i].iparamj1=  pItemData->iparamj1;    //(保留使用)
	//			iItemDataInfo[i].iparamj2=  pItemData->iparamj2;
	//			iItemDataInfo[i].iparamj3=  pItemData->iparamj3;    //取回的密码
	//			iItemDataInfo[i].iparamj5=  pItemData->iparamj5;
	//			iItemDataInfo[i].iparamj4=  pItemData->iparamj4;    //是否已经兑换了收入
	//			iItemDataInfo[i].iparamj6=  pItemData->iparamj6;
	//			iItemDataInfo[i].iparamj7=  pItemData->iparamj7;
	//			//sprintf(nIdxInfo,"%04d",pItemData->iid);
	//			//g_DbItemFile->remove(nIdxInfo,strlen(nIdxInfo)+1);
	//			m_ItemShopCount ++;
	//		}
	//		if(!g_DbItemFile->next(cursor))
	//			break;
	//	}
	//	printf("---[拍卖行]物品加载成功,共加载了:%d 个物品---\n",m_ItemShopCount);
	//}//end if isgo	
	////活动数据
	//
	//g_DbActivity = new ZDBTableS("activitydata","activitydb");  //新建内存
	////nIsGo=0;
	//if  (g_DbActivity)
	//{
	//	if (g_DbActivity->addIndex(get_activitydata)>-1 )//设置索引的类型
	//	{
	//		g_DbActivity->open();
	//		//nIsGo = 1;
	//		printf("---[活动数据]初始化成功!---\n");
	//	}
	//	else
	//		printf("---[活动数据]数据库打开失败!---\n");
	//}
	//else
	//{
	//	printf("---[活动数据]初始化失败!---\n");
	//}
	
#endif

#ifdef TOOLVERSION	
	g_SpriteCache.Init(256);
#endif

#ifndef _SERVER
	//g_bPingReply = TRUE;
	g_SoundCache.Init(256);
	g_SubWorldSet.m_cMusic.Init();//VS 2010 有错误 
#endif
	g_InitSeries();             //初始化 五行相生相克
#ifndef TOOLVERSION	 //不是工具
    if (!g_InitMath())
    {
    	printf("[Math]Failed to initialize math function!\n");
    }
	ItemSet.Init();	
    ItemGen.Init();
#ifndef _SERVER
	InitAdjustColorTab();                            //VS 2010 有错误状态颜色偏移  如 冰状态的颜色，毒状态的颜色//加载偏色表......

	g_MagicDesc.Init();                              //初始化客户端魔法属性类型
	g_MagicBaoshiDesc.Load("\\Settings\\BaoShiMagicDesc.ini");
#endif

#endif
	g_ItemChangeRes.Init();                          //VS 2010 有错误 初始化装备外观文件
   
	g_gmList.Load("\\settings\\gmgamelist.txt");
	g_ForbitWaiGua.Load(GAME_FORBITWAIGUA_FILE);
	g_ForbitMap.Load(GAME_FORBITMAP_FILE);
	if(!g_FsJinMai.Load(GAME_FSJINMAI_FILE))
        printf(" Configuration file failed to load meridian_level.txt\n");
	
	g_PlayerTitle.Load("\\Settings\\playertitle.txt");

	if  (!g_NpcMapDropRate.Load(MAP_DROPRATE_FILE))
		  printf("--ERROR:%s\n",MAP_DROPRATE_FILE);

    NpcSet.m_cGoldTemplate.Init();                       //黄金怪物模板初始化
    NpcSet.Init();
	ObjSet.Init();
	MissleSet.Init();
	//printf("-----开始加载脚本文件-----\n");
#ifdef _SERVER
    g_IniScriptEngine("\\script");//加载初始化脚本文件
#else
	g_IniScriptEngine("\\Ui",FALSE);
#endif

	if (!g_OrdinSkillsSetting.Load(SKILL_SETTING_FILE))   //初始化技能基本数据
        printf(" Configuration file failed to load skills.txt\n");
	g_MisslesSetting.Load(MISSLES_SETTING_FILE);     //子弹初始化  子技能
	g_NpcSetting.Load(NPC_SETTING_FILE);             //加载NPC模板
	
	InitSkillSetting();                              //技能   //初始化技能基本数据	
	InitMissleSetting();                             //子弹
	InitNpcSetting();                                //npc模板
	
#ifdef _SERVER
	TCHAR szExePath[MAX_PATH + 1]={0};
	KPIGetExePath(szExePath, MAX_PATH);
	const char log_directory[] = "gameser_log";
	const size_t log_threshold = 512 * 1024;

	TCHAR szLogDir[MAX_PATH + 1]={0};
	_tcscpy(szLogDir, szExePath);
	_tcscat(szLogDir, log_directory);
	_tcscat(szLogDir, "\\");
	_tcscat(szLogDir, nParmName);

	::CreateDirectory(szLogDir, NULL);

	if (!g_LogFile.Initialize(szLogDir,log_threshold))
	{
		printf("Failed to initialize log server!\n");
	}
	else
		g_LogFile.sLogProgram(TRUE,"GameServer");

	char nTempMapSetPath[128];
	sprintf(nTempMapSetPath,"\\maps\\WorldSet_%s.ini",nParmName);
	gameMapSetPath=nTempMapSetPath;
	g_SubWorldSet.LoadSerMaps(nTempMapSetPath);//加载服务器地图 和 刷怪

	if(!g_GMSetting.Load(GAME_GM_FILE))
		printf(" Configuration file failed to load gmgamelist.txt\n");
	if(!g_AccoutSet.Load(GAME_ACCT_FILE))
		printf(" Configuration file failed to load Accsetlist.txt\n");

#endif

	InitTaskSetting();                               //任务
	// 这个涉及到与技能相关的东西，所以必须放在技能初始化之后
	if (!PlayerSet.Init())
	{
		printf("Init PlayerSet Error!!!!\n");
	}


#ifdef _SERVER

	g_WorldScript.Init();
	g_WorldScript.RegisterFunctions(WorldScriptFuns, g_GetWorldScriptFunNum());  //注册服务器脚本函数

	if (!g_ReBornSetting.Load(GAME_REBORN_FILE))
	{
		printf(" g_ReBornSetting: Failed to load configuration rebornsetting.txt\n");
	}

	if (!g_PlayerVipSetting.Load(GAME_PVIP_FILE))
	{
		printf(" g_PlayerVipSetting:Failed to load configuration rolevipsetting.txt\n");
	}

	//if(!g_TaskLoop.Load(GAME_TASK_FILE))
	      //printf("g_TaskLoop  配置文件加载失败\n");

	//if  (g_SubWorldSet.LoadTaskInfo())
		//printf("---初始化任务系统成功---\n");

	g_TeamSet.Init();	

	if (!g_MoveMapPos.Load(GOLD_MOVE_POS))
		printf(" Failed to load configuration goldmovepos.txt\n");

	if (!GameData.Init())
		printf("Init GameData Error!!!!\n");
	else
		printf("Init GameData.ini OKE!!!!\n");
	
#endif

    g_CompAtlas.Load(GAME_ATLAS_FILE);
	g_GetMsg.Load(GAME_MSG_FILE);
	g_CompEquip.Load(GAME_COMP_FILE);
	
#ifndef _SERVER
	g_MapTraffic.Load("\\settings\\MapTraffic.ini");
	g_ScenePlace.Initialize();
	//g_GameClientScript.Init();
	//g_GameClientScript.RegisterFunctions(GameScriptFuns,g_GetWorldScriptFunNum());  //注册服务器脚本函数
#endif
	// 门派初始化：必须在技能初始化之后才能进行
	g_Faction.Init();	
#ifdef _SERVER
   	time(&ltime);
#endif
	 
	memset(g_nMeleeWeaponSkill, 0, sizeof(g_nMeleeWeaponSkill));
	memset(g_nRangeWeaponSkill, 0, sizeof(g_nRangeWeaponSkill));
	KTabFile Weapon_PhysicsSkillIdFile;
	if (Weapon_PhysicsSkillIdFile.Load(WEAPON_PHYSICSSKILLFILE))
	{
		int nHeight = Weapon_PhysicsSkillIdFile.GetHeight() - 1;

		for (int i = 0; i < nHeight; ++i)
		{
			int nDetail = 0;
			int nParticular  = 0;
			int nPhysicsSkill = 0;
			Weapon_PhysicsSkillIdFile.GetInteger(i + 2,WEAPON_DETAILTYPE, -1, &nDetail);
			Weapon_PhysicsSkillIdFile.GetInteger(i + 2,WEAPON_PARTICULARTYPE, -1, &nParticular);
			Weapon_PhysicsSkillIdFile.GetInteger(i + 2,WEAPON_SKILLID, -1, &nPhysicsSkill);
			
			//近程武器
			if (nDetail == 0 )
			{
				if (nParticular >= 0 && nParticular < MAX_MELEEWEAPON_PARTICULARTYPE_NUM && nPhysicsSkill > 0 && nPhysicsSkill < MAX_SKILL)
					g_nMeleeWeaponSkill[nParticular] = nPhysicsSkill;
			}
			else if (nDetail == 1)
			{
				if (nParticular >= 0 && nParticular < MAX_RANGEWEAPON_PARTICULARTYPE_NUM && nPhysicsSkill > 0 && nPhysicsSkill < MAX_SKILL)
					g_nRangeWeaponSkill[nParticular] = nPhysicsSkill;
			}
			else if (nDetail == -1) //空手
			{
				if (nPhysicsSkill > 0 && nPhysicsSkill < MAX_SKILL)
					g_nHandSkill = nPhysicsSkill;
			}
		}
	}

	Weapon_PhysicsSkillIdFile.Clear();

#ifndef TOOLVERSION
	if (!BuySell.Init())  //商店初始化
	{
		printf("---BuySell.Init! faild--- \n");
	}
#endif

#ifdef _SERVER	
	printf("End of Init ,Game Started! %s\n" , ctime(&ltime));
	printf("==========================================\n");
#endif
}

#ifdef _SERVER	
BOOL CreateConnection(_LPDATABASEINFO pDatabase, S3PDBConVBC** ppInfo)
{
	S3PDBConVBC* pCon = new S3PDB_MSSQLServer_Connection();
	if (pCon->OpenConnect(pDatabase))
	{
		*ppInfo = pCon;
		return TRUE;
	}

	if (pCon)
	{
		delete pCon;
		pCon = NULL;
	}
	return FALSE;
}

#define def_KEYNAMELEN	256
BOOL CreateConnectio2n(S3PDBConVBC** ppInfo)
{
	// database.ini is not there in every deployment, and the assignments below do not care:
	// each name becomes a std::string, whose constructor runs strlen over whatever the frame
	// above this function happened to hold.  Zeroed, a missing file means an empty name and a
	// connection that fails - not a read of the stack into the login path's connection strings.
	TCHAR szServer[def_KEYNAMELEN]={0};
	TCHAR szDataBase[def_KEYNAMELEN]={0};
	TCHAR szUser[def_KEYNAMELEN]={0};
	TCHAR szPassword[def_KEYNAMELEN]={0};

	KIniFile iniFile;
	if (iniFile.Load("database.ini"))
	{
		iniFile.GetString("account", "Server", "WIN-V73R1T8QVE5\\SQLEXPRESS", szServer, sizeof(szServer));
		iniFile.GetString("account", "DataBase", "account", szDataBase, sizeof(szDataBase));
		iniFile.GetString("account", "User", "sa", szUser, sizeof(szUser));
		iniFile.GetString("account", "PassWord", "123456", szPassword, sizeof(szPassword));
	}

	_DATABASEINFO DBInfo;
	DBInfo.strServer = szServer;
	DBInfo.strDataBase = szDataBase;
	DBInfo.strUser = szUser;
	DBInfo.strPassword = szPassword;

	S3PDBConVBC* pConn = new S3PDB_MSSQLServer_Connection();
	if (pConn->OpenConnect(&DBInfo))
	{
		*ppInfo = pConn;
		return TRUE;
	}
	else
		return FALSE;
}
#endif

BOOL InitTaskSetting()
{
#ifdef _SERVER
	if (!g_EventItemTab.Load(QUESTITEM_TABFILE))
	{
		printf("[TASK]CAN NOT LOAD EventItem Setting File %s\n", QUESTITEM_TABFILE);
	}

	if (!g_FusionTab.Load(FUSION_TABFILE))
	{
		printf("[TASK]CAN NOT LOAD FusionItem Setting File %s\n", FUSION_TABFILE);
	}
    
	if (!g_MineItemTab.Load(MINEITEM_TABFILE))
	{
		printf("[TASK]CAN NOT LOAD MineItem Setting File %s\n", MINEITEM_TABFILE);
	}
	if (!g_WayPointTabFile.Load(WORLD_WAYPOINT_TABFILE))
	{
		printf("[TASK]CAN NOT LOAD WayPoint Setting File %s\n", WORLD_WAYPOINT_TABFILE);
	}

	if (!g_StationTabFile.Load(WORLD_STATION_TABFILE))
	{
		printf("[TASK]CAN NOT LOAD WayPoint Setting File %s\n", WORLD_STATION_TABFILE);
	}

	if (!g_StationPriceTabFile.Load(WORLD_STATIONPRICE_TABFILE))
	{
		printf("[TASK]CAN NOT LOAD STATION PRICE TabFile %s\n", WORLD_STATIONPRICE_TABFILE);
	}

	if (!g_WayPointPriceTabFile.Load(WORLD_WAYPOINTPRICE_TABFILE))
	{
		printf("[TASK]CAN NOT LOAD WAYPOINT PRICE TabFile %s\n", WORLD_WAYPOINTPRICE_TABFILE);
	}
	
	if (!g_DockTabFile.Load(WORLD_DOCK_TABFILE))
	{
		printf("[TASK]CAN NOT LOAD DOCK TabFile %s\n", WORLD_DOCK_TABFILE);
	}

	if (!g_DockPriceTabFile.Load(WORLD_DOCKPRICE_TABFILE))
	{
		printf("[TASK]CAN NOT LOAD DOCK PRICE TabFile %s\n", WORLD_DOCKPRICE_TABFILE);
	}

	int nWidth1 = g_StationPriceTabFile.GetWidth();
	int nHeight1 = g_StationPriceTabFile.GetHeight();
	int nWidth2 = g_WayPointPriceTabFile.GetWidth();
	int nHeight2 = g_WayPointPriceTabFile.GetHeight();
	int nWidth3 = g_DockPriceTabFile.GetWidth();
	int nHeight3 = g_DockPriceTabFile.GetHeight();
	
	
	// three grids sized by a tab file's own dimensions.  As an int product each of
	// these wraps, and the wrap is what the allocation, the fill and the test below
	// would disagree about; one cell count per grid feeds all three
	const size_t nCells1 = (size_t)nWidth1 * nHeight1;
	const size_t nCells2 = (size_t)nWidth2 * nHeight2;
	const size_t nCells3 = (size_t)nWidth3 * nHeight3;
	KASSERT(nCells1);
	KASSERT(nCells2);
	KASSERT(nCells3);
	
	if (nCells1)
	{
		g_StationPriceTabFile.GetInteger(1,1, 1, &g_StationPriceUnit);
		g_pStationPriceTab = new int[nCells1];
		memset(g_pStationPriceTab, -1, sizeof(int) * nCells1);
	}
	
	if (nCells2)
	{
		g_WayPointPriceTabFile.GetInteger(1,1,1, &g_WayPointPriceUnit);
		g_pWayPointPriceTab = new int[nCells2];
		memset(g_pWayPointPriceTab, -1, sizeof(int) * nCells2);
	}
	
	if (nCells3)
	{
		g_DockPriceTabFile.GetInteger(1,1,1, &g_DockPriceUnit);
		g_pDockPriceTab = new int[nCells3];
		memset(g_pDockPriceTab, -1, sizeof(int) * nCells3);
	}
	if (!g_TimerTask.Init())
	{
		printf(" g_TimerTask.Init() Erroorororo!\n");
	}

	if (!g_MissionTabFile.Load(TASK_MISSION_SETTING_TABFILE))
	{
		printf("[error]Can Not Open %s\n", TASK_MISSION_SETTING_TABFILE);
	}
	
#endif
#ifndef _SERVER
	if (!g_StringResourseTabFile.Load(STRINGRESOURSE_TABFILE))
	{
		printf("[TASK]CAN NOT LOAD %s\n", STRINGRESOURSE_TABFILE);
	}

	if (!g_RankTabSetting.Load(PLAYER_RANK_SETTING_TABFILE))
	{
	    printf("[TASK]CAN NOT LOAD %s\n", PLAYER_RANK_SETTING_TABFILE);
	}
#endif
	printf(" InitTaskSetting() OK!!!..\n");
	return TRUE;
}

BOOL InitNpcSetting()
{
	int nNpcTemplateNum = g_NpcSetting.GetHeight() - 1;
	//g_DebugLog("人物模板数量%d", nNpcTemplateNum);
	//g_pNpcTemplate = new KNpcTemplate[nNpcTemplateNum * MAX_NPC_LEVEL]; //0,0为起点
	
	//memset(g_pNpcTemplate, 0, sizeof(void*) * MAX_NPCSTYLE * MAX_NPC_LEVEL);
	// 载入文件 人物类型.txt
//	g_SetFilePath(RES_INI_FILE_PATH);
	if ( !g_NpcKindFile.Load(NPC_RES_KIND_FILE_NAME) )
	{
		printf("无法打开%s文件!!!\n", NPC_RES_KIND_FILE_NAME);
	}
	
	//printf("---NpcTempleSize is %d * %d * %d = %d (%d mb)---\n", sizeof(KNpcTemplate) , nNpcTemplateNum , MAX_NPC_LEVEL, sizeof(KNpcTemplate) * nNpcTemplateNum * MAX_NPC_LEVEL,sizeof(KNpcTemplate) * nNpcTemplateNum * MAX_NPC_LEVEL/(1024*1024));
	
	//加载Npc等级设定的脚本文件，用于今后加载Npc时使用
#ifdef _SERVER

	g_pNpcLevelScript = (KLuaScript*)g_GetScript(NPC_LEVELSCRIPT_FILENAME);

	if (!g_pNpcLevelScript) 
	{
		printf("-%s Npc等级设定脚本无法加载-\n",NPC_LEVELSCRIPT_FILENAME);
	}

#else
	g_pNpcLevelScript = new KLuaScript;
	g_pNpcLevelScript->Init();
	if (!g_pNpcLevelScript->Load(NPC_LEVELSCRIPT_FILENAME))
	{
		//printf("[error]致命错误,无法正确读取%s\n", NPC_LEVELSCRIPT_FILENAME);
		delete g_pNpcLevelScript;
		g_pNpcLevelScript = NULL;
	}
#endif
		
#ifndef _SERVER
	g_NpcResList.Init();   //客户端加载外观文件动作表
#endif

	//printf(" InitNpcSetting(%d) OK..!!!!\n",nNpcTemplateNum);
	return TRUE;
}

BOOL	InitMissleSetting()
{
	int nMissleNum = g_MisslesSetting.GetHeight() - 1;


	//g_MisslesLib = new KMissle[nMissleNum]
	
	for (int i = 0; i < nMissleNum; ++i)
	{
		int nMissleId = 0;
		g_MisslesSetting.GetInteger(i + 2, "MissleId", -1, &nMissleId);//第二行开始

		if (nMissleId>=MAX_MISSLESTYLE)
           continue;

		if (nMissleId > 0)
		{
			g_MisslesLib[nMissleId].GetInfoFromTabFile(i + 2);
			g_MisslesLib[nMissleId].m_nMissleId = nMissleId;
		}
	}
//	g_DebugLog("子碟类占用空间为%d", sizeof(g_MisslesLib));
	//printf(" InitMissleSetting(%d)空间为:%d OK..!!!!\n",nMissleNum,sizeof(g_MisslesLib));
	return TRUE;
}

extern int g_LoadSkillInfo();
BOOL	InitSkillSetting()
{
	
	if (!g_SkillManager.Init())
	{
		_ASSERT(0);
	}

#ifndef _SERVER
	if (!g_ClientWeaponSkillTabFile.Load(CLIENTWEAPONSKILL_TABFILE))
	{
		printf("Can Not Load %s \n", CLIENTWEAPONSKILL_TABFILE);
	}
#endif
	//printf(" g_SkillManager.Init() OK..!!!!\n");
	return TRUE;
}
#ifdef _SERVER
BOOL	LoadNpcSettingFromBinFile(LPSTR BinFile = NPC_TEMPLATE_BINFILE)
{
	return FALSE;
}

BOOL	SaveAsBinFileFromNpcSetting(LPSTR BinFile = NPC_TEMPLATE_BINFILE)
{
	return FALSE;
}
#endif

//---------------------------------------------------------------------------

void g_ReleaseCore()
{

	int nNpcTemplateNum = g_NpcSetting.GetHeight() - 1;
	unsigned long i = 0;
	unsigned long j = 0;
	
	/*for (i = 0; i < nNpcTemplateNum; ++i)
	{
		for (j = 0; j < MAX_NPC_LEVEL; ++j)
		{
			if (g_pNpcTemplate[i][j])
			{
				delete ((KNpcTemplate *)g_pNpcTemplate[i][j]);
				g_pNpcTemplate[i][j] = NULL;
			}
		}
	} */
#ifdef _SERVER
	if (g_pServer)
	{
		g_pServer->Release();
		g_pServer = NULL;
	}

	if (g_pTongClient)
	{
		g_pTongClient->Release();
		g_pTongClient = NULL;
	}

	if (g_pDataClient)
	{
		g_pDataClient->Release();
		g_pDataClient = NULL;
	}
	
	if (g_pStationPriceTab)
	{
		delete [] g_pStationPriceTab;
		g_pStationPriceTab = NULL;
	}

	if (g_pWayPointPriceTab)
	{
		delete [] g_pWayPointPriceTab;
		g_pWayPointPriceTab = NULL;
	}
	
	if (g_pDockPriceTab)
	{
		delete [] g_pDockPriceTab;
		g_pDockPriceTab = NULL;
	}
#else
	g_SubWorldSet.Close();
	g_ScenePlace.ClosePlace();
	if (g_pNpcLevelScript)
	{
		delete g_pNpcLevelScript;
		g_pNpcLevelScript = NULL;
	}
	if (g_pAdjustColorTab)
	{
		delete []g_pAdjustColorTab;
		g_pAdjustColorTab = NULL;
		g_ulAdjustColorCount = 0;
	}
#endif

    g_UnInitMath();
}

#ifdef _SERVER


void KPIGetExePath( LPSTR lpExePath, DWORD dwSize )
{
	if ( lpExePath )
	{
		memset( lpExePath, 0, dwSize );
		GetModuleFileName( NULL, lpExePath, dwSize );
		DWORD dwEndAt;
		dwEndAt = strlen( lpExePath ) - 1;
		while ( dwEndAt >= 1 )
		{
			if ( lpExePath[dwEndAt-1] == '\\' )
			{
				lpExePath[dwEndAt] = 0;
				break;
			}
			dwEndAt--;
		}
	}
}

void g_SetServer(LPVOID pServer)
{               
	g_pServer = reinterpret_cast<IServer *>(pServer);
}

void g_SetTranServer(LPVOID pServer)
{
	g_pTranClient= reinterpret_cast<IClient *>(pServer);
}

void g_SetChatServer(LPVOID pServer)
{
	g_pChatClient= reinterpret_cast<IClient *>(pServer);
}

void g_SetTonServer(LPVOID pServer)
{
	g_pTongClient= reinterpret_cast<IClient *>(pServer);

}

void g_SetDataServer(LPVOID pServer)
{
	g_pDataClient= reinterpret_cast<IClient *>(pServer);
	
}

void g_SetGateWayServer(LPVOID pServer)
{
	g_pGateWayClient= reinterpret_cast<IClient *>(pServer);
}
// #define _tcsrchr strrchr
std::string gGetLocateDirectory()
{
	static struct _LOCALDIR
	{
		std::string path;
		
		_LOCALDIR()
		{
			TCHAR buff[_MAX_PATH] = {0};
			::GetModuleFileName(NULL, buff, _MAX_PATH);
			TCHAR* pCh = strrchr(buff, '\\');
			assert(pCh);
			*(pCh + 1) = 0;
			path = buff;
			
			assert(!path.empty());
		}
	} s_locdir;
	
	return s_locdir.path;
}

int PositionToRoom(int P)
{
	switch (P)
	{
	case pos_hand:
	case pos_equip:
	case pos_equiproom:
		return room_equipment;
	case pos_equiproomex:
		return room_equipmentex;
	case pos_repositoryroom:
		return room_repository;
/*	case pos_repositoryroom1:
		return room_repository1;
	case pos_repositoryroom2:
		return room_repository2;
	case pos_repositoryroom3:
		return room_repository3;
	case pos_repositoryroom4:
		return room_repository4;
	case pos_repositoryroom5:
		return room_repository5;*/
	case pos_traderoom:
		return room_equipment;
	case pos_trade1:
		return room_equipment;
	case pos_immediacy:
		return room_immediacy;
	case pos_give:
		return room_give;
/*	case pos_compound:
		return room_equipment;
	case pos_compoundroom:
		return room_compound;*/
	default:
		return room_equipment;
	}
	return room_equipment;
}

#endif

#ifndef _SERVER
void g_SetClient(LPVOID pClient)
{
	g_pClient = reinterpret_cast<IClient *>(pClient);
}
//状态颜色偏差表 状态持续颜色显示
unsigned int	InitAdjustColorTab()
{    
	g_pAdjustColorTab = NULL;
	g_ulAdjustColorCount = 0;
	KTabFile TabFile;
	if (!TabFile.Load(ADJUSTCOLOR_TABFILE))
	{
		_ASSERT(0);
		g_DebugLog("无法打开%s", ADJUSTCOLOR_TABFILE);
		return 0;
	}

	int nHeight = TabFile.GetHeight() - 1;
	
	if (nHeight <= 0)
	{
		TabFile.Clear();
		return 0;
	}

	g_pAdjustColorTab = (unsigned int *)new unsigned long [nHeight];
	g_ulAdjustColorCount = nHeight;

	for (int i = 0; i < nHeight; ++i)
	{
		BYTE bAlpha = 0;
		BYTE bRed	= 0;
		BYTE bGreen	= 0;
		BYTE bBlue	= 0;
		int nAlpha;
		int nRed;
		int nGreen;
		int nBlue;
		TabFile.GetInteger(i + 2, "ALPHA", 0x000000ff, &nAlpha);
		nAlpha	&= 0xff;
		TabFile.GetInteger(i + 2,"RED", 0, &nRed);
		nRed	&= 0xff;
		TabFile.GetInteger(i + 2,"GREEN",  0, &nGreen);
		nGreen	&= 0xff;
		TabFile.GetInteger(i + 2,"BLUE",  0, &nBlue);
		nBlue	&= 0xff;
		unsigned long ulAdjustColor = nAlpha << 24 | nRed << 16 | nGreen << 8 | nBlue;
		g_pAdjustColorTab[i] = ulAdjustColor;
	}
	TabFile.Clear();
	return g_ulAdjustColorCount;
}	  
#endif



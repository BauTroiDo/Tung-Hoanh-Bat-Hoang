#ifndef	KWorldH
#define	KWorldH

#ifdef _SERVER
#define	MAX_SUBWORLD	nMaxSubWorldCount
#else
#define	MAX_SUBWORLD	1  //客户端的地图限制
#include <map>
#include <vector>

#endif
#define	VOID_REGION		-2
//-------------------------------------------------------------
#include "KEngine.h"
#include "KRegion.h"
#include "KWeatherMgr.h"
#include "GameDataDef.h"
#include "kregionsearcher.h"

#ifdef _SERVER
#include "KMission.h"
#include "KMissionArray.h"
//#define MAX_SUBWORLD_MISSIONCOUNT 500  //在某个地图中的 MSid
//#define MAX_GLOBAL_MISSIONCOUNT 500    //全局 MSID 限制
typedef KMissionArray <KMission ,MAX_TIMER_PERMISSION> KSubWorldMissionArray;  //每个地图
typedef KMissionArray <KMission ,MAX_MISSIONARRAY_VALUE_COUNT> KGlobalMissionArray;
extern KGlobalMissionArray g_GlobalMissionArray;
#endif

//-------------------------------------------------------------
enum enum_MapState
{
	emMAP_STATE_UNLOADED = 0,
	emMAP_STATE_LOADING,
	emMAP_STATE_LOADED,
	emMAP_STATE_IDLE,
	emMAP_STATE_UNLOADING,
	emMAP_STATE_FAILED
};

#ifndef TOOLVERSION
class KSubWorld
#else
class CORE_API KSubWorld
#endif
{
public:	
	enum_MapState m_eMapState;
	BOOL		m_bIsPinned;
	UINT		m_dwIdleStartTime; // PR4: Idle TTL tracking
	char		m_szPathName[128];
	char		m_szNewWorldScript[MAX_RESOURCE_FILE_NAME_LEN];
	char		m_szNormalDropRateScript[MAX_RESOURCE_FILE_NAME_LEN];
	char		m_szNpcReviveScript[MAX_RESOURCE_FILE_NAME_LEN];
	int			m_nIndex;            //第几个地图
	int			m_SubWorldID;        //第几个地图的地图编号
	int			m_SubWhereTeam;      //当前地图属于那个队伍比赛用的
	int         m_nCurPKPunishState;
	int         m_nNoDropItemKind;   //地图不掉落物品的类型
	BOOL        m_IsShowLoop;        //是否显示地图信息
	
	int         m_WargKillNum;	     //正方 死亡次数
	int         m_WarsKillNum;		 //反方 死亡次数

	int         m_playerEnterCount_z;	     //正方 死亡次数
	int         m_playerEnterCount_f;		 //反方 死亡次数

	BYTE        m_IsWhereCity;        //今天是哪个城市被攻城 1,2.....7 用于写入城战文件 
    BYTE        m_WarIsWho;           //属于哪方的 或 帮派地图--共用  1 为攻方 2 为守方

	int         m_ShuiShouType;        //税收的类型  是增加 还是减少

    BYTE        m_IswarCity;            //是否被占领

	int         m_IsLuKey;	          //地图的幸运倍率
	int         m_IsJinYan;		      //地图的经验倍率
	int         m_IsJinQian;		  //地图的银两倍率

	int         m_WarCityManey; //城市建设资金
	int         m_WarCityJB;    //金币
	int         m_WarCityGX;    //贡献
	int         m_GuziGameStat;
    
	bool        m_suoHun;
	int         m_nWPKFlag;  //是否允许PK
	int         m_nWFight;   //是否战斗模式
	int         m_nWcomp;    //该地图的阵营		 Npc[nNpcIndex].SetCurrentCamp(nValue);
    int         m_nMapTime;  //能逗留的时间
    int         m_nSleepTime; //能延时的时间

	int         m_minLimitLevel;//限制的等级
	int         m_maxLimitLevel;//限制的等级

	int         m_nNpcZongNum;  //总波数
	int         m_nCurNpcNum;	//当前波数
	int         m_nCurNum;      //每波的怪物数

	int         m_nPleyerZongNum;  //总人数
	int         m_nCurPleyerNum;   //当前人数

	bool      _IsNoSeries;        //该地图的怪物 是否有五行相克
	bool      _IsNoDamage;     //没有伤害的地图 人物无敌存在
	bool      _IsNoCost;           //不消耗 内力 体力 血蓝
	bool      _IsNoLostItem;    //不掉装备
	bool      _IsNoLostMoney;   //不掉银两
	int        _maskType;            //面具类型
//------------------------------------------------------------
	/*KList		m_NpcList;							// 人物列表
	KList		m_ObjList;							// 物件列表
	KList		m_MissleList;						// 子弹列表
	KList		m_PlayerList;						// 玩家列表
	*/
	unsigned int GameData[14];

#ifdef _SERVER
	int			m_nNpcSyncCounter;					// 同步计数器
	int			m_nObjSyncCounter;
	KSubWorldMissionArray m_MissionArray;           //某个地图的MISS
	KLinkArray	m_cActRegionIdx;
#endif
	KRegion*	m_Region;

#ifndef _SERVER
	int			m_ClientRegionIdx[MAX_REGION];
	char		m_szMapPath[FILE_NAME_LENGTH];
#endif

int			m_nWorldRegionWidth;			//	地图的实际宽度
int			m_nWorldRegionHeight;			//  地图的实际高度	
int			m_nTotalRegion;					//	地图的实际面积大小
int			m_nRegionWidth;					//	地图中每块Region的格子宽度（固定16）
int			m_nRegionHeight;				//	地图中每块Region的格子高度（固定32）
int			m_nCellWidth;					//	Cell的像素宽度（固定32）
int			m_nCellHeight;					//	Cell的像素高度（固定32）
int			m_nRegionBeginX;		        //  地图的开始位置X LEFT		
int			m_nRegionBeginY;                //  地图的开始位置Y TOP
int	        m_nRegionEndX;                  //  地图的结束位置X Rigth
int         m_nRegionEndY;                  //  地图的结束位置Y Buttom
int			m_nWeather;						//	天气变化
int			m_nMacigCount;				    //	魔法属性的数量
UINT		m_dwCurrentTime;				//	当前帧
UINT		m_NewdwCurrentTime;				//	当前帧
KWorldMsg	m_WorldMessage;					//	消息
KList		m_NoneRegionNpcList;			//	不在地图上的NPC

typedef std::/*unordered_*/map<INT, std::string> _WorldMapInfo;                 //SkillId,Echance
_WorldMapInfo nWorldMapInfo;

#ifdef _SERVER
//KWeatherMgr *m_pWeatherMgr;
#endif

private:
	//	BYTE*		m_pNpcRef;							// 每个格子上的NPC数目
	//  BYTE*		m_pObjRef;							// 每个格子上的OBJ数目	
public:
	KSubWorld();
	~KSubWorld();
	void		Activate();
	void		stateActivate();
	void     showActivateRegionInfo();
	void		GetFreeObjPos(POINT& pos);
	void        GetFreePos(POINT& pos);
	VOID        GetFreeNewObjPos(POINT& pos);
	
	
	BOOL		CanPutObj(POINT pos,int nModle=0,BOOL nIsCheckNpc=FALSE);
	void		ObjChangeRegion(int nSrcRegionIdx, int nDesRegionIdx, int nObjIdx);
	void		MissleChangeRegion(int nSrcRegionIdx, int nDesRegionIdx, int nObjIdx);
	bool		AddPlayer(int nRegion, int nIdx);
	void		RemovePlayer(int nRegion, int nIdx);
	void		Close();
	int			GetDistance(int nRx1, int nRy1, int nRx2, int nRy2);						    // 像素级坐标
	void		NewMap2Mps(int nR, int nX, int nY, int nDx, int nDy, int *nRx, int *nRy);		// 格子坐标转像素坐标
 void        Map2Mps(int nRx, int nRy, int nX, int nY, int nDx, int nDy, int *pnX, int *pnY);// 格子坐标转像素坐标	
	void		Mps2MapnRodom(int * nR, int * nX, int * nY, int *nDx, int * nDy,int *Rx,int*Ry,int nOffwminx=1,int nOffwmaxx=1,int nOffhminy=1,int nOffhmaxy=1);	// 像素坐标转格子坐标
	void		Mps2Map(int Rx, int Ry, int * nR, int * nX, int * nY, int *nDx, int * nDy);	    // 像素坐标转格子坐标
	void		GetMps(int *nX, int *nY, int nSpeed, int nDir, int nMaxDir = 64);			    // 取得某方向某速度下一点的坐标
	BYTE		TestBarrier(int nMpsX, int nMpsY);
	void	        setBarrier(int nMpsX, int nMpsY,int ver);
	BYTE		TestBarrier(int nRegion, int nMapX, int nMapY, int nDx, int nDy, int nChangeX, int nChangeY);	// 检测下一点是否为障碍
	BYTE		TestBarrierMin(int nRegion, int nMapX, int nMapY, int nDx, int nDy, int nChangeX, int nChangeY);	// 检测下一点是否为障碍
	BYTE		GetBarrier(int nMpsX, int nMpsY);											// 取得某点的障碍信息
	UINT		GetTrap(int nMpsX, int nMpsY);
	BOOL		SetTrap(int nMpsX, int nMpsY,int nCellNum=1,DWORD uTrapScriptId=0);
	void		MessageLoop();
	int			FindRegion(int RegionID);													// 找到某ID的Region的索引
	int			FindFreeRegion(int nX = 0, int nY = 0);
	void		RemoveNpc(int nIdx);
#ifdef _SERVER
//	KScenePlaceC  *SplaceS;
	int			RevivalAllNpc();  //将地图上所有的Npc包括已死亡的Npc全部恢复成原始状态
	int	        DelAllNpcInWro(); //删除该地图上的所有NPC
	int         DelAllNpcInWroWithName(char *nName,int nType);
	int         GetNpcCount(int nNpcKind=0);
	int         MovePlayerOutMap(int nFigthstate,int nDesPos,int nIsComp,int nPunish,int nClearScript);
	int	        DelTypeNpcInWro(int nNpcSettings);
	//void        BroadCast(void* pBuffer, DWORD dwSize, int &nMaxCount, int nOX, int nOY);

	void		BroadCasts(const char* pBuffer, size_t uSize);
	int		    LoadMapS(int nIdx);
	int		    LoadMapMetadata(int nIdx);
	int		    LoadMapGrid(int nIdx);
	BOOL		EnsureMapLoaded();
	BOOL		UnloadMapGrid();
	BOOL		CanSafelyUnload();
	BOOL		IsLoaded() const { return m_Region != NULL && m_eMapState == emMAP_STATE_LOADED; }
	void		LoadObject(char* szPath, char* szFile);
	void		NpcChangeRegion(int nSrcRegionIdx, int nDesRegionIdx, int nNpcIdx);
	void		PlayerChangeRegion(int nSrcRegionIdx, int nDesRegionIdx, int nObjIdx);
	BOOL	SendSyncData(int nIdx, int nClient);
	int			GetRegionIndex(int nRegionID);
	int			FindNpcFromName(const char * szName);
	//加入激活列表
	VOID	  AddActiveRegion(INT nRegionIdx)		{ if  (nRegionIdx>=0 && nRegionIdx < m_nTotalRegion) m_cActRegionIdx.Insert(nRegionIdx + 1); }
	//从激活列表中删除
	VOID	 RemoveActiveRegion(INT nRegionIdx)	{ m_cActRegionIdx.Remove(nRegionIdx + 1); }
	int     	 getActiveRegionCount()	{ return m_cActRegionIdx.GetCount(); } //有多少个已经被激活的 区域

	void     clearAlltrap();
	void     clearAllObstacle();
	void     clearAllObj();
	void     ClearAllNpc(int bOnlyDynamicAdd);
	int        FindNpcPosWithName(int *x,int *y,char *strName);
#endif
	//客户端
#ifndef _SERVER
	BOOL		LoadMapC(int nIdx, int nRegion);
	void		NpcChangeRegion(int nSrcRegionIdx, int nDesRegionIdx, int nNpcIdx);
	void		Paint();
	void		Mps2Screen(int *Rx, int *Ry);
	void		Screen2Mps(int *Rx, int *Ry);
	void        Mps2MapClient(int Rx, int Ry, int * nR, int * nX, int * nY, int *nDx, int * nDy);
#endif

private:
	void		LoadTrap();
	void		ProcessMsg(KWorldMsgNode *pMsg);
#ifndef _SERVER
	void		LoadCell();	
#endif
public:
		KRegionSearcher GetRegionSearcher(KRegionSearcher::enumType eSearchType, INT nRange, INT nSearcherIdx, INT nRelation, INT nMapX = -1, INT nMapY = -1);
		KRegionSearcher GetRegionSearcher(KRegionSearcher::enumType eSearchType, INT nRange, INT nSearcherIdx, NPC_RELATION sRelation, INT nMapX = -1, INT nMapY = -1);
};

#ifndef TOOLVERSION
extern KSubWorld	*SubWorld;//SubWorld[MAX_SUBWORLD];
#else 
extern CORE_API KSubWorld	*SubWorld;//SubWorld[MAX_SUBWORLD];
#endif

#ifdef _SERVER
extern BOOL g_bLazyMapLoadingEnabled;
extern BOOL g_bSafeUnloadEnabled;
extern DWORD g_dwMapIdleFrames;
extern int g_nMaxConcurrentLoads;
extern int g_nMaxLoadedMapsBudget;
extern int g_nMapLoadsThisTick;
extern int g_nLoadedNonPinnedMapCount;
#endif

#endif

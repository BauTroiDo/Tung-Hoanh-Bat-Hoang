#ifndef	KRegionH
#define	KRegionH

//-----------------------------------------------------------------------
#include "KEngine.h"
#include "KWorldMsg.h"
//后来加的
#include "Scene/SceneDataDef.h"
#include <vector>
#include <list>
#include <map>
#include "KGameglobalSet.h"
#include "KSpawnDefinition.h"
//-----------------------------------------------------------------------
#ifdef _SERVER
#define	MAX_BROADCAST_COUNT		100
#else
#define	MAX_REGION		9


#endif

#define	REGION_GRID_WIDTH	16
#define	REGION_GRID_HEIGHT	32

#define	REGION_WIDTH			16
#define	REGION_HEIGHT			32

enum MOVE_OBJ_KIND
{
	obj_npc,
	obj_object,
	obj_obstacle,
	obj_missle,
};
struct KTrapGrid
{
	int data[REGION_GRID_WIDTH][REGION_GRID_HEIGHT];
};


#ifndef TOOLVERSION
class KRegion
#else
class CORE_API KRegion
#endif
{
	friend class	KSubWorld;
public:

	enum SCENE_REGION_PUBLIC_PARAM
	{
		SCENE_LOGIC_CELL_WIDTH			= 32,		//逻辑运算格子(cell)大小，单位：场景坐标
		SCENE_LOGIC_CELL_HEIGHT			= 32,		//逻辑运算格子(cell)大小，单位：场景坐标
		SCENE_LOGIC_CELL_WIDTH_1024		= 32768,	//逻辑运算格子大小扩大1024倍
		SCENE_LOGIC_CELL_HEIGHT_1024	= 32768,	//逻辑运算格子大小扩大1024倍
		SCENE_LOGIC_CELL_CENTER_H_1024	= 16384,	//逻辑运算格子中点偏移扩大1024倍
		SCENE_LOGIC_CELL_CENTER_V_1024	= 16384,	//逻辑运算格子中点偏移扩大1024倍
		SCENE_REGION_WIDTH				= 512,		//每个region的横宽，单位：场景坐标
		SCENE_REGION_HEIGHT				= 1024,		//每个region的纵宽，单位：场景坐标
		SCENE_REGION_CELL_NUM_H			= SCENE_REGION_WIDTH / SCENE_LOGIC_CELL_WIDTH,	//每个region横向有几个逻辑运算格子
		SCENE_REGION_CELL_NUM_V			= SCENE_REGION_HEIGHT / SCENE_LOGIC_CELL_HEIGHT,//每个region纵向有几个逻辑运算格子
	};

	typedef std::vector<INT> VEC_LIST;
	int         m_nSubMapidx;                       // 属于第几个地图的
	int			m_nIndex;							      // 地图区域的索引
	int			m_RegionID;

	KList		m_NpcList;							// 人物列表
	KList		m_ObjList;							// 物件列表
	KList		m_MissleList;						// 子弹列表
	KList		m_PlayerList;						// 玩家列表

	int			m_nConnectRegion[8];				// 相邻的地图索引
	int			m_nConRegionID[8];					// 相邻的地图ID
	KRegion*	m_pConRegion[8];					// 相邻的Region
	int			m_nRegionX;							// 在世界中的位置X（象素点）
	int			m_nRegionY;							// 在世界中的位置Y（象素点）
	int			m_nWidth;
	int			m_nHeight;
private:
//--------------------------------------------------------------------------
	enum _KEACT_TYPE
	{
		emACT_TYPE_NONE	= 0,	// 一切活动终止
		emACT_TYPE_WAIT,		        // 等待终止，可能尚有Missile、Obj未消失
		emACT_TYPE_ALL,			    // 全部活动，周围有玩家
	};

		class _KGridData
	{
	public:
		_KGridData()
		{
			Clear();
		}

		BOOL HasData(INT nX, INT nY) const
		{
			if (nX < 0 || nX >= REGION_WIDTH || nY < 0 || nY >= REGION_HEIGHT)
				return FALSE;
			return (m_adwDataFlag[nX] & (1 << nY)) != 0;
		}

		INT GetData(INT nX, INT nY) const
		{
			if (nX < 0 || nX >= REGION_WIDTH || nY < 0 || nY >= REGION_HEIGHT)
				return 0;
			return (INT)m_nData[nX][nY];
		}

		BOOL SetData(INT nX, INT nY, INT nData)
		{
			if (nX < 0 || nX >= REGION_WIDTH || nY < 0 || nY >= REGION_HEIGHT)
				return FALSE;

			m_nData[nX][nY] = (short)nData;
			if (nData)
				m_adwDataFlag[nX] |= (1 << nY);
			else
				m_adwDataFlag[nX] &= ~(1 << nY);
			return TRUE;
		}

		BOOL IncData(INT nX, INT nY)
		{
			if (nX < 0 || nX >= REGION_WIDTH || nY < 0 || nY >= REGION_HEIGHT)
				return FALSE;

			m_adwDataFlag[nX] |= (1 << nY);
			m_nData[nX][nY]++;
			return TRUE;
		}

		BOOL DecData(INT nX, INT nY)
		{
			if (nX < 0 || nX >= REGION_WIDTH || nY < 0 || nY >= REGION_HEIGHT)
				return FALSE;

			if (m_nData[nX][nY] > 0)
			{
				if (--m_nData[nX][nY] == 0)
					m_adwDataFlag[nX] &= ~(1 << nY);
			}
			return TRUE;
		}

		BOOL Clear()
		{
			memset(m_adwDataFlag, 0, sizeof(m_adwDataFlag));
			memset(m_nData, 0, sizeof(m_nData));
			return TRUE;
		}

	private:
		unsigned int m_adwDataFlag[REGION_WIDTH];
		short        m_nData[REGION_WIDTH][REGION_HEIGHT];
	};
//---------------------------------------------------------
#ifdef _SERVER
	int		m_Obstacle[REGION_GRID_WIDTH][REGION_GRID_HEIGHT];	// 地图障碍信息表
    KTrapGrid* m_pdwTrap; // On-demand trap allocation (saves ~1.4GB RAM)
	INT			m_nNearbyPlayerCount;				// 附近玩家数量
	INT			m_nNearbyActiveNpcCount;			// 附近的活动NPC数量
	_KEACT_TYPE	m_eActType;							// 激活类型

	//_KGridData	m_Obstacle;						            // 每个格子上的障碍信息
	//_KGridData	m_dwTrap;							            // 每个格子上的Trap信息
#endif
	int			m_nNpcSyncCounter;					// 同步计数器
	int			m_nNpcSyncCounter_s;					// 同步计数器

	int			m_nObjSyncCounter;
	int			m_nActive;							// 是否激活（是否有玩家在附近）
#ifdef _SERVER
	std::vector<KSpawnDefinition>	m_vSpawnDefs;	// PR1: Static NPC Spawn Definitions
#endif


	int*		m_pNpcRef;							// 每个格子上的NPC数目  BYTE
	int*		m_pObjRef;							// 每个格子上的OBJ数目
	//int*		m_pObstacleRef;					// 每个格子上的OBJ数目
	//int*		m_pMslRef;							// 每个格子上的MISSLE数目

	_KGridData	m_cNpcRef;						    // 每个格子上的NPC数目
	_KGridData	m_cObjRef;						    // 每个格子上的OBJ数目
public:
	KRegion();
	~KRegion();
	BOOL		Init(int nWidth, int nHeight);
	BOOL		Load(int nX, int nY);
#ifdef _SERVER
	// 载入服务器端地图上本region 的 object数据（包括npc、trap、box等）
	int		        LoadObjectS(int nSubWorld, int nX, int nY,KMapList* pMapListInfo);
	// 载入服务器端地图上本 region 的障碍数据
	BOOL		LoadServerObstacle(KPakFile *pFile, DWORD dwDataSize);
	// 载入服务器端地图上本 region 的 trap 数据
	BOOL		LoadServerTrap(KPakFile *pFile, DWORD dwDataSize);
	// 载入服务器端地图上本 region 的 npc 数据
	int		        LoadServerNpc(int nSubWorld, KPakFile *pFile, DWORD dwDataSize,KMapList* pMapLiInfo);
	int		        GetSpawnDefCount() const { return (int)m_vSpawnDefs.size(); }
	const KSpawnDefinition* GetSpawnDef(int nIdx) const { if (nIdx >= 0 && nIdx < (int)m_vSpawnDefs.size()) return &m_vSpawnDefs[nIdx]; return NULL; }
	KSpawnDefinition* GetSpawnDefMutable(int nIdx) { if (nIdx >= 0 && nIdx < (int)m_vSpawnDefs.size()) return &m_vSpawnDefs[nIdx]; return NULL; }
	VOID			ActivateRegionNpcs(int nSubWorld);
	// 载入服务器端地图上本 region 的 obj 数据
	BOOL		LoadServerObj(int nSubWorld, KPakFile *pFile, DWORD dwDataSize);


	int           DelAllNpc(int mSubWorldID=0);
	int           DelAllNpcWithName(char *nName,int nType);

	

#endif

#ifndef _SERVER
	// 载入客户端地图上本region 的 object数据（包括npc、box等）
	// 如果 bLoadNpcFlag == TRUE 需要载入 clientonly npc else 不载入
	BOOL		LoadObjectC(int nSubWorld, int nX, int nY, char *lpszPath);
	// 载入客户端地图上本 region 的 clientonlynpc 数据
	BOOL		LoadClientNpc(KPakFile *pFile, DWORD dwDataSize);
	// 载入客户端地图上本 region 的 clientonlyobj 数据
	BOOL		LoadClientObj(KPakFile *pFile, DWORD dwDataSize);
	// 载入障碍数据给小地图
	static void	LoadLittleMapData(int nX, int nY, char *lpszPath, INT *lpbtObstacle);

	static void GetFliePath(char* nName,char * nPath,int nMaxRows);
#endif
	void		  Close();
	void		  NewRegActivate(int  _minLimitLevel, int  _maxLimitLevel);
	void 		  showRegActivateInfo(int i);
	BYTE	 GetBarrier(int MapX, int MapY, int nDx, int nDy);	//	地图高度
	void       setBarrier(int nMapX, int nMapY,int ver);
	BYTE    GetNewBarrier(int nMapX, int nMapY, int nDx, int nDy,BOOL nIsCheckNpc=FALSE);
	// 按 像素点坐标 * 1024 的精度判断某个位置是否障碍
	// 参数：nGridX nGirdY ：本region格子坐标
	// 参数：nOffX nOffY ：格子内的偏移量(像素点 * 1024 精度)
	// 参数：bCheckNpc ：是否判断npc形成的障碍
	// 返回值：障碍类型(if 类型 == Obstacle_NULL 无障碍)
	BYTE		GetBarrierMin(int nGridX, int nGridY, int nOffX, int nOffY, BOOL bCheckNpc);
	BYTE	    GetBarrierNewMin(int nGridX, int nGridY, int nOffX, int nOffY, BOOL bCheckNpc);

	void		SetTrapNew(DWORD dwTrapId, int nMapX, int nMapY);
	DWORD		GetTrap(int MapX, int MapY);						//	得到Trap编号
    BOOL		SetTrap(int MapX, int MapY,int nCellNum=16,DWORD uTrapScriptId=0);
	BOOL		SetTrap(int nMapX, int nMapY,DWORD uTrapScriptId);

	void          clearTrap();
	void          clearObstacle();
	inline BOOL	IsActiveAll()
	{
#ifdef _SERVER
		return m_eActType == emACT_TYPE_ALL; 
#else
		return TRUE;
#endif
	}

    inline BOOL	IsActive() 
	{
#ifdef _SERVER
		return m_nActive; 
#else
		return TRUE;
#endif
	};
 
	BOOL	HasNpc(INT nMapX, INT nMapY) CONST { return m_cNpcRef.HasData(nMapX, nMapY); }
	INT		GetNpcRef(INT nMapX, INT nMapY) CONST { return m_cNpcRef.HasData(nMapX, nMapY); }
	BOOL	AddNpcRef(INT nMapX, INT nMapY) { return m_cNpcRef.IncData(nMapX, nMapY); }
	BOOL	DecNpcRef(INT nMapX, INT nMapY) { return m_cNpcRef.DecData(nMapX, nMapY); }

	BOOL	HasObj(INT nMapX, INT nMapY) CONST { return m_cObjRef.HasData(nMapX, nMapY); }
	INT		GetObjRef(INT nMapX, INT nMapY) CONST { return m_cObjRef.HasData(nMapX, nMapY); }
	BOOL	AddObjRef(INT nMapX, INT nMapY) { return m_cObjRef.IncData(nMapX, nMapY); }
	BOOL	DecObjRef(INT nMapX, INT nMapY) { return m_cObjRef.DecData(nMapX, nMapY); }

	int			GetRef(int nMapX, int nMapY, MOVE_OBJ_KIND nType);
	BOOL	AddRef(int nMapX, int nMapY, MOVE_OBJ_KIND nType);
	BOOL	DecRef(int nMapX, int nMapY, MOVE_OBJ_KIND nType);

	int			CheckNpcInDisten(int nScyNpcIdx, int nTarNpcIdx,int nDis);
	int			FindNpc(int nMapX, int nMapY, int nNpcIdx, DWORD nRelation,int nType=0);
	int	        FindNpcAuto(int nMapX, int nMapY, int nNpcIdx, DWORD nRelation,int nType=0,int nDis=0);
	//int		FindEquip(int nMapX, int nMapY);
	int			SearchNpcName(const char*  szName);
	int			FindObjectAuto(int nMapX, int nMapY,int nKind=-1,char *nItemName=NULL,int nNpcIdx=0);
	int			FindObject(int nObjID);
	 int       FindObject(int nMapX, int nMapY);
	void*	GetObjNode(int nIdx);
	//VEC_LIST::iterator	GetObjNode(INT nIdx);
	int			SearchNpc(DWORD dwNpcID);		// 寻找本区域内是否有某个指定 id 的 npc (zroc add)
#ifdef _SERVER
	//void		SendSyncData(int nClient);
	void		BroadCast(void *pBuffer, DWORD dwSize, int &nMaxCount, int nX, int nY);
	int			FindPlayer(DWORD dwId);
	BOOL		CheckPlayerIn(int nPlayerIdx);
#endif

#ifndef _SERVER
	void		Paint();
#endif
	void		AddNpc(int nIdx);
	void		RemoveNpc(int nIdx);
	//void	    RemoveNpcwor(int nIdx);
	void		AddMissle(int nIdx);
	void		RemoveMissle(int nIdx);
	void		AddObj(int nIdx);
	void		RemoveObj(int nIdx);
	void     RemoveAllObj();

	void     ClearAllNpc(int bOnlyDynamicAdd);
	bool     FindNpcPosWithName(int *x,int *y,char *strName);
	BOOL		AddPlayer(int nIdx);
	BOOL		RemovePlayer(int nIdx);

	BOOL	  AddNearByNpc(INT nAdd, BOOL bConRegion = TRUE);					// 增加区域内活动NPC的计数(由于指令要动态改变NPC的active状态，不能为私有成员)
private:
#ifdef _SERVER
	BOOL	AddNearbyPlayer(INT nAdd, BOOL bConRegion = TRUE);				// 增加区域内活动玩家计数
	BOOL	UpdateActiveType();
#endif

};


inline int KRegion::FindObject(int nMapX, int nMapY)
{

	if  (!GetRef(nMapX,nMapY,obj_object))
		return 0;

	KIndexNode *pNode = NULL;
	pNode = (KIndexNode *)m_ObjList.GetHead();
	while (pNode)
	{
		if (Object[pNode->m_nIndex].m_nMapX == nMapX && Object[pNode->m_nIndex].m_nMapY == nMapY)
			return pNode->m_nIndex;

		pNode = (KIndexNode *)pNode->GetNext();
	}

	return 0;
}


inline int KRegion::CheckNpcInDisten(int nScyNpcIdx, int nTarNpcIdx,int nDis)
{
	if  (nScyNpcIdx<=0 || nTarNpcIdx<=0)
		return FALSE;

	int nScyX,nScyY,nTarX,nTarY,nScymap,nTarmap;
	Npc[nScyNpcIdx].GetMpsPos(&nScyX, &nScyY,&nScymap);
	Npc[nTarNpcIdx].GetMpsPos(&nTarX, &nTarY,&nTarmap);

	if (nScymap!=nTarmap)
		return FALSE;

	int nCurDis=0;
	nCurDis = (int)sqrt(double((nScyX - nTarX)) * (nScyX - nTarX) + (nScyY - nTarY) * (nScyY - nTarY));

	if  (nCurDis<=nDis)
		return TRUE;
	else
		return FALSE;
}

//--------------------------------------------------------------------------
//	Find Npc
//--------------------------------------------------------------------------
inline int KRegion::FindNpc(int nMapX, int nMapY, int nNpcIdx, DWORD nRelation,int nType)
{

	if  (!GetRef(nMapX,nMapY,obj_npc))
		return 0;

	KIndexNode *pNode = NULL;
	
	pNode = (KIndexNode *)m_NpcList.GetHead();
	
	while(pNode)
	{
		if (Npc[pNode->m_nIndex].m_MapX == nMapX && Npc[pNode->m_nIndex].m_MapY == nMapY)
		{
          if (nType==0 && Npc[pNode->m_nIndex].m_CurrentLife>0)
		  {
			//if (nRelation==NpcSet.GetRelation(nNpcIdx, pNode->m_nIndex)) //位移对比 是敌人的话
			  if (nRelation & NpcSet.GetRelation(nNpcIdx, pNode->m_nIndex))
               return pNode->m_nIndex;  
		  }
		  else if (nType==1 && Npc[pNode->m_nIndex].m_CurrentLife>0)
		  {
			  if (nRelation==Npc[pNode->m_nIndex].m_NpcSettingIdx) 
			  {
                     return pNode->m_nIndex;
			  }
		  }
		  else if (nType==2 && Npc[pNode->m_nIndex].m_CurrentLife>0)
		  {
			  if (nRelation==Npc[pNode->m_nIndex].m_dwID) 
			  {
                     return pNode->m_nIndex;
			  }
		  }

		}
		pNode = (KIndexNode *)pNode->GetNext();
	}	
	return 0;
}

//挂机的使用的查找NPC
inline int KRegion::FindNpcAuto(int nMapX, int nMapY, int nNpcIdx, DWORD nRelation,int nType,int nDis)
{

	if  (!GetRef(nMapX,nMapY,obj_npc))
		return 0;

	KIndexNode *pNode = NULL;
	
	pNode = (KIndexNode *)m_NpcList.GetHead();
	
	while(pNode)
	{
		if (Npc[pNode->m_nIndex].m_MapX == nMapX && Npc[pNode->m_nIndex].m_MapY == nMapY)
		{
          if (nType==0 && Npc[pNode->m_nIndex].m_CurrentLife>0)
		  {
			//if (NpcSet.GetRelation(nNpcIdx, pNode->m_nIndex)==nRelation) //位移对比 是敌人的话
			if (nRelation & NpcSet.GetRelation(nNpcIdx, pNode->m_nIndex))
			{
			       if (Npc[pNode->m_nIndex].m_AttackerDwid!=Npc[nNpcIdx].m_dwID)
				   {
					   if  (nDis>0)
					   {
						   if (CheckNpcInDisten(nNpcIdx,pNode->m_nIndex,nDis))
							   return pNode->m_nIndex;
					   }
					   else
						   return pNode->m_nIndex;  
				   }  
			}
		  }
		  else if (nType==1 && Npc[pNode->m_nIndex].m_CurrentLife>0)
		  {
			  if (nRelation==Npc[pNode->m_nIndex].m_NpcSettingIdx) 
			  {
                    if (Npc[pNode->m_nIndex].m_AttackerDwid!=Npc[nNpcIdx].m_dwID)
					{
						if  (nDis>0)
						{
							if (CheckNpcInDisten(nNpcIdx,pNode->m_nIndex,nDis))
								return pNode->m_nIndex;
						}
						else
							return pNode->m_nIndex;  
					}

			  }
		  }
		  else if (nType==2 && Npc[pNode->m_nIndex].m_CurrentLife>0)
		  {
			  if (nRelation==Npc[pNode->m_nIndex].m_dwID) 
			  {
                     if (Npc[pNode->m_nIndex].m_AttackerDwid!=Npc[nNpcIdx].m_dwID)
					 {
						 if  (nDis>0)
						 {
							 if (CheckNpcInDisten(nNpcIdx,pNode->m_nIndex,nDis))
								 return pNode->m_nIndex;
						 }
						 else
							 return pNode->m_nIndex;  
					 } 
			  }
		  }

		}
		pNode = (KIndexNode *)pNode->GetNext();
	}	
	return 0;
}

#endif
#include "KCore.h"
#include <math.h>
#include "KRegion.h"
#include "KMath.h"
#include "KNpc.h"
#include "KPlayer.h"
#include "KNpcSet.h"
#include "KObjSet.h"
#include "KPlayerSet.h"
#include "KMissleSet.h"
#include <ppl.h>
#ifndef _STANDALONE
#include "crtdbg.h"
#endif
#include "Scene/ObstacleDef.h"
#ifdef _SERVER
//#include "KNetServer.h"
//#include "../MultiServer/Heaven/Interface/iServer.h"
//#include "scene/KScenePlaceC.h"
#endif
#ifndef _SERVER
#include "scene/KScenePlaceC.h"
#include "scene/kpathfinder.h"
#endif
#include "KSubWorld.h"

#ifndef TOOLVERSION
KSubWorld	*SubWorld = NULL;//SubWorld[MAX_SUBWORLD];
#else 
CORE_API KSubWorld	*SubWorld = NULL;//SubWorld[MAX_SUBWORLD];
#endif

#define		defLOGIC_CELL_WIDTH		32
#define		defLOGIC_CELL_HEIGHT		32

#ifdef _SERVER
KGlobalMissionArray g_GlobalMissionArray;
#endif

KSubWorld::KSubWorld()
{
	_maskType = 0;
	m_minLimitLevel=0;//限制的等级
	m_maxLimitLevel=0;//限制的等级
	_IsNoSeries = false;
	_IsNoDamage= false;

    _IsNoCost = false;          //不消耗 内力 体力 血蓝
 	_IsNoLostItem = false;   //不掉装备
	_IsNoLostMoney = false;  //不掉银两

	nWorldMapInfo.clear();
	m_Region = NULL;
	m_nRegionWidth	= 16;//512 / 32; //16
	m_nRegionHeight	= 32;//1024 / 32;//32
	m_nCellWidth = defLOGIC_CELL_WIDTH;
	m_nCellHeight = defLOGIC_CELL_HEIGHT;
	m_dwCurrentTime = 0;
	m_NewdwCurrentTime=0;
	m_SubWorldID = -1;
	nWorldMapInfo[STR_MAP_MASTER] = "空缺";
    nWorldMapInfo[STR_MAP_ZTONGNAME] = "空缺";
	nWorldMapInfo[STR_MAP_TGONGNAME] = "空缺";
	nWorldMapInfo[STR_MAP_TSHOUNAME] = "空缺";
	nWorldMapInfo[STR_MAP_TEMP_MASTER] = "空缺";

	m_WargKillNum = 0;	     //正方
	m_WarsKillNum = 0;		 //反方

	m_playerEnterCount_z =0;
	m_playerEnterCount_f =0;

    m_ShuiShouType = 0;
	m_IswarCity=0;

	m_IsLuKey=1;	          //地图的幸运倍率
	m_IsJinYan=0;		      //地图的经验倍率
	m_IsJinQian=1;		      //地图的银两倍率

	m_WarCityManey=0;
	m_WarCityJB=0;
	m_WarCityGX=0;
	m_nWPKFlag=-1;       //可切换PK模式模式
	m_GuziGameStat=-1;
	m_nWFight=-1;
	m_WarIsWho=0;        //公用模式
	m_IsWhereCity=0;
	m_nWcomp=-1;
	m_nMapTime=0;
	m_SubWhereTeam=0;
	m_nSleepTime=-1;
	m_IsShowLoop=FALSE;
	m_suoHun=false;
	m_nNpcZongNum=0;  //总波数
    m_nCurNpcNum=0;	  //当前波数
	m_nCurNum=0;      //每波的怪物数

	m_nCurPKPunishState = -1;
	m_nNoDropItemKind   = -1;
	m_nWeather = WEATHERID_NOTHING;
	m_nMacigCount = 0;
#ifndef _SERVER
	m_nWorldRegionWidth = 3;
	m_nWorldRegionHeight = 3;
	memset(m_ClientRegionIdx, 0, sizeof(this->m_ClientRegionIdx));
	memset(this->m_szMapPath, 0, sizeof(this->m_szMapPath));
#endif

#ifdef _SERVER
	m_nWorldRegionWidth = 0;
	m_nWorldRegionHeight = 0;
	m_nNpcSyncCounter = 0;
	m_nObjSyncCounter = 0;
	//m_pWeatherMgr = NULL;
	//MissionArry中0虽然无效，但还是加上去，容错.TotalCount 为空间减1，所以不会越界
	for (int i = 0; i <= m_MissionArray.GetTotalCount(); i++)
	{
		m_MissionArray.m_Data[i].SetOwner(this);  //设置所有者
	}
#endif
	m_nTotalRegion = m_nWorldRegionWidth * m_nWorldRegionHeight;
#ifdef _SERVER
	m_cActRegionIdx.Init(m_nTotalRegion + 1);
#endif // _SERVER
}

KSubWorld::~KSubWorld()
{
	if (m_Region)
	{
		delete [] m_Region;
		m_Region = NULL;
	}

/*#ifdef _SERVER
	if(m_pWeatherMgr)
	{
		delete m_pWeatherMgr;
		m_pWeatherMgr = NULL;
	}
#endif*/
}

int KSubWorld::FindRegion(int nRegion)
{
	for (int i = 0; i < m_nTotalRegion; i++)
	{
		if (m_Region[i].m_RegionID == nRegion)
			return i;
	}
	return -1;
}

int KSubWorld::FindFreeRegion(int nX, int nY)
{
	if (nX == 0 && nY == 0)
	{
		for (int i = 0; i < m_nTotalRegion; i++)
		{
			if (m_Region[i].m_RegionID == -1)
				return i;
		}
	}
	else
	{
		for (int i = 0; i < m_nTotalRegion; i++)
		{
			if (m_Region[i].m_RegionID == -1)
				return i;
			int nRegoinX = LOWORD(m_Region[i].m_RegionID);
			int nRegoinY = HIWORD(m_Region[i].m_RegionID);

			if ((nX - nRegoinX) * (nX - nRegoinX) + (nY - nRegoinY) * (nY - nRegoinY) > 2)	// 不在附近
				return i;
		}
	}
	return -1;
}

//extern int nActiveRegionCount;
//地图的循环

void KSubWorld::stateActivate()
{
	//if (m_SubWorldID < 0)
	//	return;
	////死亡需要重生的NPC循环
	//  KIndexNode* pNode = (KIndexNode *)m_NoneRegionNpcList.GetHead();
	//  while(pNode)
	//  {//不在地图上的	npc
	//	 if  (pNode->m_nIndex>0 && pNode->m_nIndex < MAX_NPC)//列表 死的怪物越多 就 越庞大
	//			Npc[pNode->m_nIndex].Activate();         //NPC 状态持续显示功能
	//	   pNode = (KIndexNode *)pNode->GetNext();
	//  }
}

//_KGridData	m_cObstacle;						                // 每个格子上的障碍信息
//_KGridData	m_cTrap;							                // 每个格子上的Trap信息	

void  KSubWorld::clearAlltrap()
{
	for (int i=1 ; i<m_nTotalRegion+1;i++)
		m_Region[i - 1].clearTrap();//m_dwTrap.Clear();
}

void  KSubWorld::clearAllObstacle()
{
	for (int i=1 ; i<m_nTotalRegion+1;i++)
		m_Region[i - 1].clearObstacle();//m_Obstacle.Clear();
}

void  KSubWorld::clearAllObj()
{
	for (int i=1 ; i<m_nTotalRegion+1;i++)
	{
		//m_Region[i - 1].m_cObjRef.Clear();
		m_Region[i - 1].RemoveAllObj();
	}
}


int  KSubWorld::FindNpcPosWithName(int *x,int *y,char *strName)
{
	for( int i = 0; i < m_nTotalRegion; i++ )
	{
		if  (m_Region[i].FindNpcPosWithName(x,y,strName))
			return true;
	}

	return false;
}

void  KSubWorld::ClearAllNpc(int bOnlyDynamicAdd)
{
	for( int i = 0; i < m_nTotalRegion; i++ )
	{
		m_Region[i].ClearAllNpc(bOnlyDynamicAdd);
	}

	KIndexNode* pCurNode = (KIndexNode*)m_NoneRegionNpcList.GetHead();

	while( pCurNode )
	{
		int nCurNpcIdx = pCurNode->m_nIndex;
		KIndexNode* pNextNode = (KIndexNode*)pCurNode->GetNext();

		if ((Npc[nCurNpcIdx].m_bDynamicAdd || !bOnlyDynamicAdd)  && !Npc[nCurNpcIdx].IsPlayer())
		{
			Npc[nCurNpcIdx].m_Node.Remove();
			Npc[nCurNpcIdx].m_Node.Release();
			NpcSet.Remove( nCurNpcIdx );
		}

		pCurNode = pNextNode;
	}
}


void KSubWorld::showActivateRegionInfo()
{
	for (int i = 0; i = m_cActRegionIdx.GetNext(i);)
	{
		if (i>=1 && i<m_nTotalRegion+1)
			m_Region[i - 1].showRegActivateInfo(i);
	}
}


void KSubWorld::Activate()
{
	if (m_SubWorldID < 0)
		return;

	extern BOOL g_bLazyMapLoadingEnabled;
	if (g_bLazyMapLoadingEnabled && (m_Region == NULL || m_eMapState != emMAP_STATE_LOADED))
		return;

	//Concurrency::Interlocked.Increment(ref totalPrimes);
	//InterLockedIncrement32();
	//InterlockedIncrement64();
	//if (MAX_SUBWORLD>=nSeparateWorldCount)
	//  InterlockedIncrement(&m_dwCurrentTime);
	//else
//#pragma omp atomic      //不支持代码块
    ++m_dwCurrentTime;	  //每秒18帧
#ifdef _SERVER
	extern BOOL g_bSafeUnloadEnabled;
	if (g_bSafeUnloadEnabled && !m_bIsPinned && m_eMapState == emMAP_STATE_LOADED)
	{
		if (getActiveRegionCount() == 0)
		{
			if (m_dwIdleStartTime == 0)
			{
				m_dwIdleStartTime = m_dwCurrentTime;
			}
			else
			{
				extern DWORD g_dwMapIdleFrames;
				extern int g_nMaxLoadedMapsBudget;
				extern int g_nLoadedNonPinnedMapCount;
				DWORD dwEffectiveThreshold = g_dwMapIdleFrames;
				if (g_nMaxLoadedMapsBudget > 0 && g_nLoadedNonPinnedMapCount > g_nMaxLoadedMapsBudget)
				{
					dwEffectiveThreshold = g_dwMapIdleFrames / 10;
					if (dwEffectiveThreshold < 180) dwEffectiveThreshold = 180;
				}
				if ((m_dwCurrentTime - m_dwIdleStartTime) >= dwEffectiveThreshold)
				{
					if (CanSafelyUnload())
					{
						UnloadMapGrid();
						return;
					}
				}
			}
		}
		else
		{
			m_dwIdleStartTime = 0;
		}
	}
#endif

	if (m_nMapTime>0)
	{
//#pragma omp atomic
		m_nMapTime--;
	}

	if (m_nSleepTime>0)
	{
//#pragma omp atomic
		m_nSleepTime--;
	}

#ifndef _SERVER
	g_ScenePlace.SetCurrentTime(m_dwCurrentTime);
	NpcSet.ClearActivateFlagOfAllNpc();           //全部设定为 否 ，执行循环处理
#endif
//	#pragma omp parallel for

#ifdef _SERVER
	for (int i = 0; i = m_cActRegionIdx.GetNext(i);)
	{
		if (i>=1 && i<m_nTotalRegion+1)
			m_Region[i - 1].NewRegActivate(m_minLimitLevel, m_maxLimitLevel);
	}

	//for (int i = 0; i < m_nTotalRegion; i++)
	//{
	//	if (m_Region[i].IsActive())
	//		m_Region[i].NewRegActivate();
	//}
	//for (int i = 0; i < m_nTotalRegion; i++)      //地图的总面积
	//	m_Region[i].NewRegActivate();   //NPC 和 玩家的循环

#else
	for (int i = 0; i < m_nTotalRegion; i++)      //地图的总面积
		m_Region[i].NewRegActivate();   //NPC 和 玩家的循环
#endif

#ifdef _SERVER
    //Mission  触发器持续状态
	m_MissionArray.Activate(); 

	//死亡需要重生的NPC循环
	KIndexNode* pNode = (KIndexNode *)m_NoneRegionNpcList.GetHead();
	while(pNode)
	{//不在地图上的	npc
       if  (pNode->m_nIndex>0 && pNode->m_nIndex < MAX_NPC) //列表 死的怪物越多 就 越庞大
			   Npc[pNode->m_nIndex].Activate();         //NPC 状态持续显示功能

		pNode = (KIndexNode *)pNode->GetNext();
	}
/*
	if(m_pWeatherMgr)
	{//天气的循环
		int nWeather = m_pWeatherMgr->Activate();
		if(m_nWeather != nWeather)
		{
			m_nWeather = nWeather;
			SYNC_WEATHER weatherMsg;
			weatherMsg.ProtocolType = s2c_changeweather;
			weatherMsg.WeatherID = (BYTE)m_nWeather;
			BroadCasts((const char*)&weatherMsg, sizeof(SYNC_WEATHER));
		}
	}
*/	
#endif
}

int KSubWorld::GetDistance(int nRx1, int nRy1, int nRx2, int nRy2)
{
	return (int)sqrt(double((nRx1 - nRx2)) * (nRx1 - nRx2) + (nRy1 - nRy2) * (nRy1 - nRy2));
}

void KSubWorld::NewMap2Mps(int nR, int nX, int nY, int nDx, int nDy, int *nRx, int *nRy)
{

	if (nR < 0 || nR >= m_nTotalRegion)
	{
		*nRx = 0;
		*nRy = 0;
		return;
	}

	int x, y;
	
	x = m_Region[nR].m_nRegionX;
	y = m_Region[nR].m_nRegionY;
	
	x += (nX<<5);// * KRegion::SCENE_LOGIC_CELL_WIDTH;
	y += (nY<<5);// * KRegion::SCENE_LOGIC_CELL_HEIGHT;
	
	x += (nDx >> 10);
	y += (nDy >> 10);
	
	*nRx = x;
	*nRy = y;
	
}


//格子中的 坐标转换成 像素坐标(废用)

void KSubWorld::Map2Mps(int nRx, int nRy, int nX, int nY, int nDx, int nDy, int *pnX, int *pnY)
{
	*pnX = (nRx * REGION_GRID_WIDTH + nX) * defLOGIC_CELL_WIDTH + (nDx >> 10);
	*pnY = (nRy * REGION_GRID_HEIGHT + nY) * defLOGIC_CELL_HEIGHT + (nDy >> 10);
} 
//像素转格子
// --
//
// --

void KSubWorld::Mps2Map(int Rx, int Ry, int * nR, int * nX, int * nY, int *nDx, int * nDy)
{
	if (m_nCellWidth == 0 || m_nCellHeight == 0 || m_nRegionWidth == 0 || m_nRegionHeight == 0)
		return;

	int x = (Rx>>9);// / KRegion::SCENE_REGION_WIDTH;
	int	 y = (Ry>>10);// / KRegion::SCENE_REGION_HEIGHT;
	//int x = Rx/(m_nRegionWidth * m_nCellWidth);	 //取商	  16*32 ==512 格子坐标
	//int	y = Ry/(m_nRegionHeight * m_nCellHeight);//取商	  32*32==1024 格子坐标

	*nX =  0;
	*nY =  0;
	*nDx = 0;
	*nDy = 0;
#ifdef _SERVER
	// 非法的坐标
	if (x >= m_nWorldRegionWidth + m_nRegionBeginX || y >= m_nWorldRegionHeight + m_nRegionBeginY || x < m_nRegionBeginX || y < m_nRegionBeginY)
	{//超出范围无效
		*nR = -1;    //区域无效
		return;
	}
#endif
	// 非法的坐标
#ifdef _SERVER
	*nR = GetRegionIndex(MAKELONG(x, y));//区域坐标 取出所在的区域编号
#else
	int nRegionID = MAKELONG(x, y);
	    *nR = FindRegion(nRegionID);
	if (*nR == -1)
		return;
#endif

	if (*nR<0 || *nR >= m_nTotalRegion)
	{
		*nR = -1;
		return;
	}

	x = Rx - m_Region[*nR].m_nRegionX;
	y = Ry - m_Region[*nR].m_nRegionY;

	*nX = (x>>5);//KRegion::SCENE_LOGIC_CELL_WIDTH;
	*nY = (y>>5);// KRegion::SCENE_LOGIC_CELL_HEIGHT;
	*nDx = (x&31) << 10;
	*nDy = (y&31) << 10;
}

//像素转格子
#ifndef _SERVER
void KSubWorld::Mps2MapClient(int Rx, int Ry, int * nR, int * nX, int * nY, int *nDx, int * nDy)
{
	if (m_nCellWidth == 0 || m_nCellHeight == 0 || m_nRegionWidth == 0 || m_nRegionHeight == 0)
		return;

	int x = Rx/(m_nRegionWidth * m_nCellWidth);	 //取商	  16*32 ==512
	int	y = Ry/(m_nRegionHeight * m_nCellHeight);//取商	  32*32==1024

	      /*if (Rx%(m_nRegionWidth * m_nCellWidth)>=(m_nRegionWidth * m_nCellWidth)/2)
			  x++;

		  if (Ry%(m_nRegionHeight * m_nCellHeight)>=(m_nRegionHeight * m_nCellHeight)/2)
			  y++; */

	*nX =  0;
	*nY =  0;
	*nDx = 0;
	*nDy = 0;

	int nRegionID = MAKELONG(x, y);

	    *nR = FindRegion(nRegionID);

	if (*nR == -1)
		return;

	if (*nR >= m_nTotalRegion)
	{
		*nR = -1;
		return;
	} 
//////加个障碍物判断/////////////////

/////////////////////////////////////
	x = Rx - m_Region[*nR].m_nRegionX;
	y = Ry - m_Region[*nR].m_nRegionY;

	*nX = x/m_nCellWidth;	 //取商
	*nY = y/m_nCellHeight;   //取商

	*nDx = (x - *nX * m_nCellWidth) << 10;	  //1024
	*nDy = (y - *nY * m_nCellHeight) << 10;
}
#endif
//////随机刷怪////////////////////////////////////////////////////
void KSubWorld::Mps2MapnRodom(int * nR, int * nX, int * nY, int *nDx, int * nDy,int *Rx,int*Ry,int nOffwminx,int nOffwmaxx,int nOffhminy,int nOffhmaxy)
{
	if (m_nCellWidth == 0 || m_nCellHeight == 0 || m_nRegionWidth == 0 || m_nRegionHeight == 0)
		return;
//	int x = Rx / (m_nRegionWidth * m_nCellWidth);
//	int	y = Ry / (m_nRegionHeight * m_nCellHeight);
	int x,y;
for (int n=0;n<m_nRegionEndX;++n)
{
      x =GetRandomNumber(m_nRegionBeginX+nOffwminx,m_nWorldRegionWidth +m_nRegionBeginX -nOffwmaxx);  
	  for (int s=0;s<m_nRegionEndY;++s)
	  {	 
	  y =GetRandomNumber(m_nRegionBeginY+nOffhminy,m_nWorldRegionHeight + m_nRegionBeginY-nOffhmaxy);
      *Rx=x*(m_nRegionWidth * m_nCellWidth); //=x*16*32
      *Ry=y*(m_nRegionHeight * m_nCellHeight);//=y*32*32
	//int Rxx=x*(m_nRegionWidth * m_nCellWidth);
         int nBarrier = GetBarrier(*Rx, *Ry); ///像素坐标障碍物判断	
	     if (nBarrier==Obstacle_NULL)
		 { 
		    break;
		 } 
         continue;
	  }

}
	*nX = 0;
	*nY = 0;
	*nDx = 0;
	*nDy = 0;
#ifdef _SERVER
	// 非法的坐标
	if (x >= m_nWorldRegionWidth + m_nRegionBeginX || y >= m_nWorldRegionHeight + m_nRegionBeginY || x < m_nRegionBeginX || y < m_nRegionBeginY)
	{
		*nR = -1;    //区域无效
		return;
	}
#endif
	// 非法的坐标
#ifdef _SERVER
	*nR = GetRegionIndex(MAKELONG(x, y));
#else
	int nRegionID = MAKELONG(x, y);
	*nR = FindRegion(nRegionID);
	if (*nR == -1)
		return;
#endif
	if (*nR >= m_nTotalRegion)
	{
		*nR = -1;
		return;
	}
	//////加个障碍物判断/////////////////
	
	/////////////////////////////////////
	x = *Rx - m_Region[*nR].m_nRegionX;
	y = *Ry - m_Region[*nR].m_nRegionY;
	
	/**nX = x / m_nCellWidth;
	*nY = y / m_nCellHeight;

	*nDx = (x - *nX * m_nCellWidth) << 10;
	*nDy = (y - *nY * m_nCellHeight) << 10;*/
	*nX = (x>>5);//KRegion::SCENE_LOGIC_CELL_WIDTH;
	*nY = (y>>5);// KRegion::SCENE_LOGIC_CELL_HEIGHT;
	*nDx = ((x&31) << 10);
	*nDy =( (y&31) << 10);
}


////////////////障碍物判断////////////////////////////////////////////
void	KSubWorld::setBarrier(int nMpsX, int nMpsY,int ver)
{

	if (m_nCellWidth == 0 || m_nCellHeight == 0 || m_nRegionWidth == 0 || m_nRegionHeight == 0)
		return;

	int x = (nMpsX>>9);// / (m_nRegionWidth * m_nCellWidth);
	int	y = (nMpsY>>10);// / (m_nRegionHeight * m_nCellHeight);

	if (x >= m_nWorldRegionWidth + m_nRegionBeginX || y >= m_nWorldRegionHeight + m_nRegionBeginY || x < m_nRegionBeginX || y < m_nRegionBeginY)
		return;

	int nRegion = GetRegionIndex(MAKELONG(x, y));

	if (nRegion >= m_nTotalRegion)
		return;

	x = nMpsX - m_Region[nRegion].m_nRegionX;
	y = nMpsY - m_Region[nRegion].m_nRegionY;

	int nCellX = (x>>5);// / m_nCellWidth;
	int nCellY = (y>>5);// / m_nCellHeight;

	int nOffX = x - (nCellX<<5);//* m_nCellWidth;
	int nOffY = y - (nCellY<<5);//* m_nCellHeight;

    m_Region[nRegion].setBarrier(nCellX, nCellY,ver);
}
//格子内粗略查找
////////////////障碍物判断////////////////////////////////////////////
BYTE	KSubWorld::TestBarrier(int nMpsX, int nMpsY)
{
	if (m_nCellWidth == 0 || m_nCellHeight == 0 || m_nRegionWidth == 0 || m_nRegionHeight == 0)
		return 0xff;
	
	int x = nMpsX / (m_nRegionWidth * m_nCellWidth);
	int	y = nMpsY / (m_nRegionHeight * m_nCellHeight);
#ifdef _SERVER
	// 非法的坐标
	if (x >= m_nWorldRegionWidth + m_nRegionBeginX || y >= m_nWorldRegionHeight + m_nRegionBeginY || x < m_nRegionBeginX || y < m_nRegionBeginY)
		return 0xff;
	int nRegion = GetRegionIndex(MAKELONG(x, y));
#else
	int nRegion = FindRegion(MAKELONG(x, y));
	if (nRegion == -1)
		return 0xff;
#endif
	if (nRegion >= m_nTotalRegion)
		return 0xff;
	
	x = nMpsX - m_Region[nRegion].m_nRegionX;
	y = nMpsY - m_Region[nRegion].m_nRegionY;
	
	int nCellX = x / m_nCellWidth;
	int nCellY = y / m_nCellHeight;
	
	int nOffX = x - nCellX * m_nCellWidth;
	int nOffY = y - nCellY * m_nCellHeight;
	
#ifndef _SERVER
	BYTE bRet = (BYTE)g_ScenePlace.GetObstacleInfo(nMpsX, nMpsY);  //获取障碍的信息
	if (bRet != Obstacle_NULL)
		return bRet;
	return m_Region[nRegion].GetBarrier(nCellX, nCellY, nOffX, nOffY);
#endif

#ifdef _SERVER
	return m_Region[nRegion].GetBarrier(nCellX, nCellY, nOffX, nOffY);
#endif
}
//格子内粗略查找
BYTE KSubWorld::TestBarrier(int nRegion, int nMapX, int nMapY, int nDx, int nDy, int nChangeX, int nChangeY)
{
	int nOldMapX = nMapX;
	int nOldMapY = nMapY;
	int nOldRegion = nRegion;

	nDx += nChangeX;
	nDy += nChangeY;

	if (nDx < 0)
	{
		nDx += (m_nCellWidth << 10);
		nMapX--;
	}
	else if (nDx >= (m_nCellWidth << 10))
	{
		nDx -= (m_nCellWidth << 10);
		nMapX++;
	}
	
	if (nDy < 0)
	{
		nDy += (m_nCellHeight << 10);
		nMapY--;
	}
	else if (nDy >= (m_nCellHeight << 10))
	{
		nDy -= (m_nCellHeight << 10);
		nMapY++;
	}
	
	if (nMapX < 0)
	{
		if (m_Region[nRegion].m_nConnectRegion[DIR_LEFT] == -1)
			return 0xff;

		nRegion = m_Region[nRegion].m_nConnectRegion[DIR_LEFT];
		nMapX += m_nRegionWidth;
	}
	else if (nMapX >= m_nRegionWidth)
	{
		if (m_Region[nRegion].m_nConnectRegion[DIR_RIGHT] == -1)
			return 0xff;
		nRegion = m_Region[nRegion].m_nConnectRegion[DIR_RIGHT];
		nMapX -= m_nRegionWidth;
	}
	
	if (nMapY < 0)
	{
		if (m_Region[nRegion].m_nConnectRegion[DIR_UP] == -1)
			return 0xff;
		nRegion = m_Region[nRegion].m_nConnectRegion[DIR_UP];;
		nMapY += m_nRegionHeight;
	}
	else if (nMapY >= m_nRegionHeight)
	{
		if (m_Region[nRegion].m_nConnectRegion[DIR_DOWN] == -1)
			return 0xff;
		nRegion = m_Region[nRegion].m_nConnectRegion[DIR_DOWN];
		nMapY -= m_nRegionHeight;
	}

	int nXf, nYf;
	nXf = (nDx >> 10);
	nYf = (nDy >> 10);

//需要检查的
#ifndef _SERVER
	int nMpsX, nMpsY;
	NewMap2Mps(nRegion, nMapX, nMapY, nDx, nDy, &nMpsX, &nMpsY);
	BYTE bRet = (BYTE)g_ScenePlace.GetObstacleInfo(nMpsX, nMpsY);
	if (bRet != Obstacle_NULL)
		return bRet;
	return m_Region[nRegion].GetBarrier(nMapX, nMapY, nXf, nYf);
#else
	return m_Region[nRegion].GetBarrier(nMapX, nMapY, nXf, nYf);
#endif
}
//格子内像数精确查找
BYTE KSubWorld::TestBarrierMin(int nRegion, int nMapX, int nMapY, int nDx, int nDy, int nChangeX, int nChangeY)
{
	if (!m_Region || nRegion < 0 || nRegion >= m_nTotalRegion)
		return 0xff;

	int nOldMapX = nMapX;
	int nOldMapY = nMapY;
	int nOldRegion = nRegion;

	nDx += nChangeX;
	nDy += nChangeY;

	if (nDx < 0)
	{
		nDx += (m_nCellWidth << 10);
		nMapX--;
	}
	else if (nDx >= (m_nCellWidth << 10))
	{
		nDx -= (m_nCellWidth << 10);
		nMapX++;
	}
	
	if (nDy < 0)
	{
		nDy += (m_nCellHeight << 10);
		nMapY--;
	}
	else if (nDy >= (m_nCellHeight << 10))
	{
		nDy -= (m_nCellHeight << 10);
		nMapY++;
	}
	
	if (nMapX < 0)
	{
		if (m_Region[nRegion].m_nConnectRegion[DIR_LEFT] == -1)
			return 0xff;

		nRegion = m_Region[nRegion].m_nConnectRegion[DIR_LEFT];
		if (nRegion < 0 || nRegion >= m_nTotalRegion)
			return 0xff;
		nMapX += m_nRegionWidth;
	}
	else if (nMapX >= m_nRegionWidth)
	{
		if (m_Region[nRegion].m_nConnectRegion[DIR_RIGHT] == -1)
			return 0xff;
		nRegion = m_Region[nRegion].m_nConnectRegion[DIR_RIGHT];
		if (nRegion < 0 || nRegion >= m_nTotalRegion)
			return 0xff;
		nMapX -= m_nRegionWidth;
	}
	
	if (nMapY < 0)
	{
		if (m_Region[nRegion].m_nConnectRegion[DIR_UP] == -1)
			return 0xff;
		nRegion = m_Region[nRegion].m_nConnectRegion[DIR_UP];
		if (nRegion < 0 || nRegion >= m_nTotalRegion)
			return 0xff;
		nMapY += m_nRegionHeight;
	}
	else if (nMapY >= m_nRegionHeight)
	{
		if (m_Region[nRegion].m_nConnectRegion[DIR_DOWN] == -1)
			return 0xff;
		nRegion = m_Region[nRegion].m_nConnectRegion[DIR_DOWN];
		if (nRegion < 0 || nRegion >= m_nTotalRegion)
			return 0xff;
		nMapY -= m_nRegionHeight;
	}

	if (nRegion < 0 || nRegion >= m_nTotalRegion)
		return 0xff;

	if (nMapX < 0 || nMapX >= m_nRegionWidth || nMapY < 0 || nMapY >= m_nRegionHeight)
		return 0xff;

	if (nMapX == nOldMapX && nMapY == nOldMapY && nRegion == nOldRegion)
		return Obstacle_NULL;

	return m_Region[nRegion].GetBarrierMin(nMapX, nMapY, nDx, nDy, false);
}

BOOL KSubWorld::SetTrap(int nMpsX, int nMpsY,int nCellNum,DWORD uTrapScriptId)
{
	int nRegion, nMapX, nMapY, nOffX, nOffY;
	    Mps2Map(nMpsX, nMpsY, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);
	if (nRegion == -1)
		return FALSE;

	return m_Region[nRegion].SetTrap(nMapX, nMapY,nCellNum,uTrapScriptId);
}

UINT KSubWorld::GetTrap(int nMpsX, int nMpsY)
{
	int nRegion, nMapX, nMapY, nOffX, nOffY;
	Mps2Map(nMpsX, nMpsY, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);
	if (nRegion == -1)
		return 0;
	return m_Region[nRegion].GetTrap(nMapX, nMapY);
}

BYTE KSubWorld::GetBarrier(int nMpsX, int nMpsY)
{
#ifdef _SERVER
	int nRegion, nMapX, nMapY, nOffX, nOffY;
	    Mps2Map(nMpsX, nMpsY, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);
	if (nRegion == -1)
		return 0xff;
	return m_Region[nRegion].GetBarrier(nMapX, nMapY, (nOffX >> 10), (nOffY) >> 10);
#else
	return (BYTE)g_ScenePlace.GetObstacleInfo(nMpsX, nMpsY);
#endif
}


#ifdef _SERVER
int KSubWorld::LoadMapMetadata(int nId)
{
	KMapList MapList;
	memset(&MapList, 0, sizeof(MapList));

	char szPathName[256] = {0};
	char szFileName[256] = {0};
	char szKeyName[64] = {0};
	char strMapKeyName[64] = {0};

	MapList.nSubWorldID = nId;
	m_SubWorldID = nId;

	sprintf(strMapKeyName, "%d_NpcSeriesAuto", nId);
	g_NpcMapDropRate.GetInteger("List", strMapKeyName, 0, &MapList.nNpcSeriesAuto);

	sprintf(strMapKeyName, "%d_NpcSeriesWood", nId);
	g_NpcMapDropRate.GetInteger("List", strMapKeyName, 0, &MapList.nNpcSeriesWood);

	sprintf(strMapKeyName, "%d_NpcSeriesMetal", nId);
	g_NpcMapDropRate.GetInteger("List", strMapKeyName, 0, &MapList.nNpcSeriesMetal);

	sprintf(strMapKeyName, "%d_NpcSeriesWater", nId);
	g_NpcMapDropRate.GetInteger("List", strMapKeyName, 0, &MapList.nNpcSeriesWater);

	sprintf(strMapKeyName, "%d_NpcSeriesFire", nId);
	g_NpcMapDropRate.GetInteger("List", strMapKeyName, 0, &MapList.nNpcSeriesFire);

	sprintf(strMapKeyName, "%d_NpcSeriesEarth", nId);
	g_NpcMapDropRate.GetInteger("List", strMapKeyName, 0, &MapList.nNpcSeriesEarth);

	sprintf(strMapKeyName, "%d_AutoGoldenNpc", nId);
	g_NpcMapDropRate.GetInteger("List", strMapKeyName, 0, &MapList.nAutoGoldenNpc);

	sprintf(strMapKeyName, "%d_GoldenType", nId);
	g_NpcMapDropRate.GetInteger("List", strMapKeyName, 0, &MapList.nGoldenType);

	sprintf(strMapKeyName, "%d_NpcDefense", nId);
	g_NpcMapDropRate.GetInteger("List", strMapKeyName, 0, &MapList.nNpcDefense);

	sprintf(strMapKeyName, "%d_NpcMaxLife", nId);
	g_NpcMapDropRate.GetInteger("List", strMapKeyName, 0, &MapList.nNpcMaxLife);

	int nTempInt = 0;
	sprintf(strMapKeyName, "%d_PunishState", nId);
	g_NpcMapDropRate.GetInteger("List", strMapKeyName, -1, &m_nCurPKPunishState);

	sprintf(strMapKeyName, "%d_NoDropItemKind", nId);
	g_NpcMapDropRate.GetInteger("List", strMapKeyName, -1, &m_nNoDropItemKind);

	sprintf(strMapKeyName, "%d_NpcSuoHun", nId);
	g_NpcMapDropRate.GetInteger("List", strMapKeyName, 0, &nTempInt);
	m_suoHun = (nTempInt > 0) ? true : false;

	sprintf(strMapKeyName, "%d_NpcExeSale", nId);
	g_NpcMapDropRate.GetInteger("List", strMapKeyName, 0, &MapList.nExpSale);

	sprintf(strMapKeyName, "%d_MapType", nId);
	g_NpcMapDropRate.GetString("List", strMapKeyName, "NoNpc", MapList.nMapType, sizeof(MapList.nMapType));

	sprintf(strMapKeyName, "%d_name", nId);
	g_NpcMapDropRate.GetString("List", strMapKeyName, "NoName", MapList.nMapName, sizeof(MapList.nMapName));
	nWorldMapInfo[STR_MAP_NAME] = MapList.nMapName;

	sprintf(strMapKeyName, "%d_NpcAutoLevelMin", nId);
	g_NpcMapDropRate.GetInteger("List", strMapKeyName, 0, &MapList.nMinLevel);
	sprintf(strMapKeyName, "%d_NpcAutoLevelMax", nId);
	g_NpcMapDropRate.GetInteger("List", strMapKeyName, 0, &MapList.nMaxLevel);
	sprintf(strMapKeyName, "%d_GoldenDropRate", nId);
	g_NpcMapDropRate.GetString("List", strMapKeyName, "", MapList.nGoldenDropRate, sizeof(MapList.nGoldenDropRate));
	sprintf(strMapKeyName, "%d_NormalDropRate", nId);
	g_NpcMapDropRate.GetString("List", strMapKeyName, "", MapList.nNormalDropRate, sizeof(MapList.nNormalDropRate));
	sprintf(strMapKeyName, "%d_NewWorldScript", nId);
	g_NpcMapDropRate.GetString("List", strMapKeyName, "", MapList.nNewWorldScript, sizeof(MapList.nNewWorldScript));
	sprintf(strMapKeyName, "%d_NewWorldParam", nId);
	g_NpcMapDropRate.GetString("List", strMapKeyName, "", MapList.nNewWorldParam, sizeof(MapList.nNewWorldParam));
	sprintf(strMapKeyName, "%d_NpcReviveScript", nId);
	g_NpcMapDropRate.GetString("List", strMapKeyName, "", MapList.nNpcReviveScript, sizeof(MapList.nNpcReviveScript));

	if (!MapList.nGoldenDropRate[0])
		sprintf(MapList.nGoldenDropRate, "\\settings\\mapdroprate\\tongyong.ini");
	if (!MapList.nNormalDropRate[0])
		sprintf(MapList.nNormalDropRate, "\\settings\\mapdroprate\\tongyong.ini");

	if (!strstr(MapList.nNewWorldScript, ".lua"))
	{
		g_NpcMapDropRate.GetString("List", "Default_NewWorldScript", "", MapList.nNewWorldScript, sizeof(MapList.nNewWorldScript));
	}
	strcpy(m_szNewWorldScript, MapList.nNewWorldScript);
	strcpy(m_szNormalDropRateScript, MapList.nNormalDropRate);
	strcpy(m_szNpcReviveScript, MapList.nNpcReviveScript);

	KIniFile sIniFile;
	sprintf(szKeyName, "%d", nId);
	g_NpcMapDropRate.GetString("List", szKeyName, "", szPathName, sizeof(szPathName));
	snprintf(m_szPathName, sizeof(m_szPathName), "%s", szPathName);

	g_SetFilePath("\\maps");
	snprintf(szFileName, sizeof(szFileName), "%s.wor", szPathName);
	g_LogFile.gTraceLogFile(szFileName, strlen(szFileName), "LoadMapMetadata");

	if (!sIniFile.Load(szFileName))
	{
		printf("-------loadmap:(%d) path error(%s)-------\n", nId, szFileName);
		m_SubWorldID = -1;
		m_eMapState = emMAP_STATE_FAILED;
		return -1;
	}

	m_nRegionWidth = 16;
	m_nRegionHeight = 32;

	RECT sRect;
	sIniFile.GetRect("MAIN", "rect", &sRect);
	m_nRegionBeginX = sRect.left;
	m_nRegionBeginY = sRect.top;
	m_nRegionEndX = sRect.right;
	m_nRegionEndY = sRect.bottom;
	m_nWorldRegionWidth = sRect.right - sRect.left + 1;
	m_nWorldRegionHeight = sRect.bottom - sRect.top + 1;
	m_nTotalRegion = (int)((size_t)m_nWorldRegionWidth * m_nWorldRegionHeight);

	if ((size_t)m_nWorldRegionWidth * m_nWorldRegionHeight > (size_t)0x7fffffff)
	{
		printf("map(%d) region grid %d x %d needs more than an int index\n",
			nId, m_nWorldRegionWidth, m_nWorldRegionHeight);
		m_SubWorldID = -1;
		m_eMapState = emMAP_STATE_FAILED;
		return -2;
	}

	m_cActRegionIdx.Init(m_nTotalRegion + 1);
	m_nCellWidth = defLOGIC_CELL_WIDTH;
	m_nCellHeight = defLOGIC_CELL_HEIGHT;

	sIniFile.Clear();

	if (m_nTotalRegion <= 0 || m_nWorldRegionWidth <= 0 || m_nWorldRegionHeight <= 0)
	{
		printf("-------map(%d) data error(W:%d,H:%d)-------\n", nId, m_nWorldRegionWidth, m_nWorldRegionHeight);
		m_SubWorldID = -1;
		m_eMapState = emMAP_STATE_FAILED;
		return -2;
	}

	// Classify pinned status
	m_bIsPinned = FALSE;
	if (strstr(MapList.nMapType, "City") ||
		strstr(MapList.nMapType, "Capital") ||
		strstr(MapList.nMapType, "Country"))
	{
		m_bIsPinned = TRUE;
	}

	m_eMapState = emMAP_STATE_UNLOADED;
	return 0;
}

int KSubWorld::LoadMapGrid(int nId)
{
	if (m_Region != NULL && m_eMapState == emMAP_STATE_LOADED)
		return 0; // Already loaded

	if (m_nTotalRegion <= 0 || m_nWorldRegionWidth <= 0 || m_nWorldRegionHeight <= 0)
		return -2;

	m_eMapState = emMAP_STATE_LOADING;

	if (m_Region)
	{
		Close();
		delete [] m_Region;
		m_Region = NULL;
	}

	m_Region = new KRegion[m_nTotalRegion];
	if (m_Region == NULL)
	{
		printf("-------map(%d) allocate memory(%d) failed-------\n", nId, m_nTotalRegion);
		m_SubWorldID = -1;
		m_eMapState = emMAP_STATE_FAILED;
		return -2;
	}

	char szPath[MAX_PATH] = {0};
	snprintf(szPath, sizeof(szPath), "\\maps\\%s", m_szPathName);

	int nX, nY, nIdx;

	// Load obstacles and connect 8 neighbor regions
	g_SetFilePath(szPath);
	for (nY = 0; nY < m_nWorldRegionHeight; ++nY)
	{
		for (nX = 0; nX < m_nWorldRegionWidth; ++nX)
		{
			nIdx = nY * m_nWorldRegionWidth + nX;
			if (m_Region[nIdx].Load(nX + m_nRegionBeginX, nY + m_nRegionBeginY))
			{
				m_Region[nIdx].Init(m_nRegionWidth, m_nRegionHeight);
				m_Region[nIdx].m_nIndex = nIdx;
				m_Region[nIdx].m_nSubMapidx = m_nIndex;
			}

			for (int i = 0; i < 8; i++)
			{
				short nTmpX, nTmpY;
				nTmpX = (short)LOWORD(m_Region[nIdx].m_nConRegionID[i]) - m_nRegionBeginX;
				nTmpY = (short)HIWORD(m_Region[nIdx].m_nConRegionID[i]) - m_nRegionBeginY;
				if (nTmpX < 0 || nTmpY < 0 || nTmpX >= m_nWorldRegionWidth || nTmpY >= m_nWorldRegionHeight)
				{
					m_Region[nIdx].m_nConnectRegion[i] = -1;
					m_Region[nIdx].m_pConRegion[i] = NULL;
					continue;
				}
				int nConIdx = nTmpY * m_nWorldRegionWidth + nTmpX;
				m_Region[nIdx].m_nConnectRegion[i] = nConIdx;
				m_Region[nIdx].m_nSubMapidx = m_nIndex;
				m_Region[nIdx].m_pConRegion[i] = &m_Region[nConIdx];
			}
		}
	}

	// Prepare MapList for NPC spawning
	KMapList MapList;
	memset(&MapList, 0, sizeof(MapList));
	MapList.nSubWorldID = nId;
	char strMapKeyName[64] = {0};

	sprintf(strMapKeyName, "%d_NpcSeriesAuto", nId);
	g_NpcMapDropRate.GetInteger("List", strMapKeyName, 0, &MapList.nNpcSeriesAuto);
	sprintf(strMapKeyName, "%d_NpcSeriesWood", nId);
	g_NpcMapDropRate.GetInteger("List", strMapKeyName, 0, &MapList.nNpcSeriesWood);
	sprintf(strMapKeyName, "%d_NpcSeriesMetal", nId);
	g_NpcMapDropRate.GetInteger("List", strMapKeyName, 0, &MapList.nNpcSeriesMetal);
	sprintf(strMapKeyName, "%d_NpcSeriesWater", nId);
	g_NpcMapDropRate.GetInteger("List", strMapKeyName, 0, &MapList.nNpcSeriesWater);
	sprintf(strMapKeyName, "%d_NpcSeriesFire", nId);
	g_NpcMapDropRate.GetInteger("List", strMapKeyName, 0, &MapList.nNpcSeriesFire);
	sprintf(strMapKeyName, "%d_NpcSeriesEarth", nId);
	g_NpcMapDropRate.GetInteger("List", strMapKeyName, 0, &MapList.nNpcSeriesEarth);
	sprintf(strMapKeyName, "%d_AutoGoldenNpc", nId);
	g_NpcMapDropRate.GetInteger("List", strMapKeyName, 0, &MapList.nAutoGoldenNpc);
	sprintf(strMapKeyName, "%d_GoldenType", nId);
	g_NpcMapDropRate.GetInteger("List", strMapKeyName, 0, &MapList.nGoldenType);
	sprintf(strMapKeyName, "%d_NpcDefense", nId);
	g_NpcMapDropRate.GetInteger("List", strMapKeyName, 0, &MapList.nNpcDefense);
	sprintf(strMapKeyName, "%d_NpcMaxLife", nId);
	g_NpcMapDropRate.GetInteger("List", strMapKeyName, 0, &MapList.nNpcMaxLife);
	sprintf(strMapKeyName, "%d_NpcExeSale", nId);
	g_NpcMapDropRate.GetInteger("List", strMapKeyName, 0, &MapList.nExpSale);
	sprintf(strMapKeyName, "%d_MapType", nId);
	g_NpcMapDropRate.GetString("List", strMapKeyName, "NoNpc", MapList.nMapType, sizeof(MapList.nMapType));
	sprintf(strMapKeyName, "%d_NpcAutoLevelMin", nId);
	g_NpcMapDropRate.GetInteger("List", strMapKeyName, 0, &MapList.nMinLevel);
	sprintf(strMapKeyName, "%d_NpcAutoLevelMax", nId);
	g_NpcMapDropRate.GetInteger("List", strMapKeyName, 0, &MapList.nMaxLevel);

	int nZNpcid = 0;
	snprintf(szPath, sizeof(szPath), "\\maps\\%s", m_szPathName);
	g_SetFilePath(szPath);
	for (nY = 0; nY < m_nWorldRegionHeight; ++nY)
	{
		for (nX = 0; nX < m_nWorldRegionWidth; ++nX)
		{
			int m_NpcNum = 0;
			try
			{
				m_NpcNum = m_Region[nY * m_nWorldRegionWidth + nX].LoadObjectS(m_nIndex, nX + m_nRegionBeginX, nY + m_nRegionBeginY, &MapList);
				nZNpcid += m_NpcNum;
			}
			catch (...)
			{
				printf("Load map(%d) object error: %s\n", nId, szPath);
			}
		}
	}

	m_eMapState = emMAP_STATE_LOADED;
	if (!m_bIsPinned)
	{
		extern int g_nLoadedNonPinnedMapCount;
		g_nLoadedNonPinnedMapCount++;
	}
	return nZNpcid;
}

BOOL KSubWorld::EnsureMapLoaded()
{
	if (m_SubWorldID < 0)
		return FALSE;

	if (m_Region != NULL && m_eMapState == emMAP_STATE_LOADED)
		return TRUE;

	if (m_eMapState == emMAP_STATE_LOADING)
		return FALSE; // Guard against recursive call

	extern int g_nMapLoadsThisTick;
	extern int g_nMaxConcurrentLoads;
	if (g_nMapLoadsThisTick >= g_nMaxConcurrentLoads)
	{
		printf("[LazyMap][Budget] Warning: MaxConcurrentLoads budget (%d) reached this tick! Loading Map ID %d synchronously.\n", g_nMaxConcurrentLoads, m_SubWorldID);
	}
	LARGE_INTEGER liFreq, liStart, liEnd;
	QueryPerformanceFrequency(&liFreq);
	QueryPerformanceCounter(&liStart);
	int nNpcNum = LoadMapGrid(m_SubWorldID);
	QueryPerformanceCounter(&liEnd);
	double dLoadMs = (double)(liEnd.QuadPart - liStart.QuadPart) * 1000.0 / (double)liFreq.QuadPart;
	g_nMapLoadsThisTick++;
	if (nNpcNum >= 0)
	{
		extern int g_nLoadedNonPinnedMapCount;
		printf("[LazyMap][Perf] Loaded on Demand Map ID: %d (SubWorld %d) in %0.2f ms, Regions: %d, NpcNum: %d (ActiveNonPinned: %d, LoadsThisTick: %d)\n", m_SubWorldID, m_nIndex, dLoadMs, m_nTotalRegion, nNpcNum, g_nLoadedNonPinnedMapCount, g_nMapLoadsThisTick);
		return TRUE;
	}
	else
	{
		printf("[LazyMap] Single-flight FAILED to load Map ID: %d (SubWorld %d)\n", m_SubWorldID, m_nIndex);
		return FALSE;
	}
}

BOOL KSubWorld::CanSafelyUnload()
{
	if (m_SubWorldID < 0 || m_Region == NULL || m_bIsPinned)
		return FALSE;

	if (m_eMapState != emMAP_STATE_LOADED)
		return FALSE;

	if (getActiveRegionCount() > 0)
		return FALSE;

#ifdef _SERVER
	// Verify no players are currently in this subworld
	for (int p = 1; p < MAX_PLAYER; ++p)
	{
		if (Player[p].m_nIndex > 0)
		{
			int nNpcIdx = Player[p].m_nIndex;
			if (nNpcIdx > 0 && nNpcIdx < MAX_NPC)
			{
				if (Npc[nNpcIdx].m_SubWorldIndex == m_nIndex)
					return FALSE;
			}
		}
	}

	// Verify no active missions on this subworld
	if (m_MissionArray.GetUsedCount() > 0)
		return FALSE;
#endif

	return TRUE;
}

BOOL KSubWorld::UnloadMapGrid()
{
	if (!CanSafelyUnload())
		return FALSE;

	m_eMapState = emMAP_STATE_UNLOADING;

#ifdef _SERVER
	// 1. Reset runtime NPC references in spawn definitions on all regions
	for (int r = 0; r < m_nTotalRegion; ++r)
	{
		for (int s = 0; s < m_Region[r].GetSpawnDefCount(); ++s)
		{
			KSpawnDefinition* pDef = m_Region[r].GetSpawnDefMutable(s);
			if (pDef)
			{
				pDef->nRuntimeNpcIndex = 0;
			}
		}
	}

	// 2. Remove all spawned NPCs in this subworld
	DelAllNpcInWro();

	// 3. Close and deallocate all regions
	if (m_Region)
	{
		for (int i = 0; i < m_nTotalRegion; ++i)
		{
			m_Region[i].Close();
		}
		delete [] m_Region;
		m_Region = NULL;
	}

	// 4. Reset active regions
	m_cActRegionIdx.Init(m_nTotalRegion + 1);
#endif

	// 5. Update state
	m_eMapState = emMAP_STATE_UNLOADED;
	m_dwIdleStartTime = 0;
	if (!m_bIsPinned)
	{
		extern int g_nLoadedNonPinnedMapCount;
		if (g_nLoadedNonPinnedMapCount > 0) g_nLoadedNonPinnedMapCount--;
	}

	printf("[SafeUnload] Cleanly Unloaded Map ID: %d (SubWorld %d) after idle TTL\n", m_SubWorldID, m_nIndex);
	return TRUE;
}

int KSubWorld::LoadMapS(int nId)
{
	int nRet = LoadMapMetadata(nId);
	if (nRet < 0)
		return nRet;

	extern BOOL g_bLazyMapLoadingEnabled;
	if (g_bLazyMapLoadingEnabled)
	{
		// If map is pinned (City, Country, Tong, Battlefield), load grid immediately
		if (m_bIsPinned)
		{
			return LoadMapGrid(nId);
		}
		// Otherwise leave unallocated (Lazy Load on first teleport/login)
		return 0;
	}
	else
	{
		// Default baseline behavior: eager load grid and obstacles
		return LoadMapGrid(nId);
	}
}
#endif
///却换地图的时候客户端加载地图信息
#ifndef _SERVER
BOOL KSubWorld::LoadMapC(int nId, int nRegion)
{
	static int	nXOff[8] = {0, -1, -1, -1, 0,  1, 1, 1};
	static int	nYOff[8] = {1, 1,  0,  -1, -1, -1, 0, 1};
	KIniFile	IniFile;
	KIniFile	Ini;

	/*if (m_Region)
	{
		delete [] m_Region;
		m_Region = NULL;
	}*/

	if (!m_Region)
	{//客户端 是 9大区域
		m_Region = new KRegion[MAX_REGION];
	}

	if (m_Region==NULL)
		return FALSE;

	if (nId != m_SubWorldID)
	{//如果地图不同的话

		SubWorld[0].Close();
		g_ScenePlace.ClosePlace();

		// NpcSet.RemoveAll(Player[CLIENT_PLAYER_INDEX].m_nIndex); -- later finish it. spe

		char szKeyName[32]={0}, szPathName[128]={0};

		g_SetFilePath("\\settings");
		IniFile.Load("MapList.ini");
		sprintf(szKeyName, "%d", nId);
		IniFile.GetString("List", szKeyName, "", szPathName, sizeof(szPathName));
		sprintf(m_szMapPath, "\\maps\\%s", szPathName);	 //区域的路径
		IniFile.Clear();
		m_SubWorldID = nId;
		g_ScenePlace.OpenPlace(nId);  //打开场景空间
		sprintf(szPathName, "\\maps\\%d.ini", nId);
		if (Ini.Load(szPathName))
			g_ScenePlace.LoadIni(&Ini);
		else
		{
			g_ScenePlace.ClearBgInfo();
		}
		Ini.Clear(); 
		//--------------
	
		//--------------
		 /*char nPath[128]={0};
		 sprintf(nPath,"%s.wor",m_szMapPath);

		 if  (IniWor.Load(nPath))
		 {
		   RECT	cRect;
		   IniWor.GetRect("MAIN", "rect", &cRect);
		   m_nRegionBeginX = cRect.left;
		   m_nRegionBeginY = cRect.top;
		   m_nRegionEndX   = cRect.right;
		   m_nRegionEndY   = cRect.bottom;
		   m_nWorldRegionWidth  = cRect.right - cRect.left + 1;   //宽度
		   m_nWorldRegionHeight = cRect.bottom - cRect.top + 1;   //高度
		   m_nTotalRegion = m_nWorldRegionWidth * m_nWorldRegionHeight;  //区域大小 面积
		   
		   char nMsg[128]={0};
		   sprintf(nMsg,"l:%u,t:%u,r:%u,b:%u",cRect.left,cRect.top,cRect.right,cRect.bottom);
		   GetFliePath("地图大小:",nMsg,1000);
		 }
		  IniWor.Clear();

		  char	RegionPathPrefix[180]={0};
		  for (int i=0;i<1000;i++)
			  for (int j=0;j<1000;j++)
			  {
				 sprintf(RegionPathPrefix, "%s\\v_%03d\\%03d_Region_C.dat", m_szMapPath, i, j);
				 KPakFile	Data;
				 if (Data.Open(RegionPathPrefix))
				 {
				    GetFliePath("障碍路径:",RegionPathPrefix,1000);
					Data.Close();
				 }
			  }
		*/
		m_nRegionWidth  = KScenePlaceRegionC::RWPP_AREGION_WIDTH / 32;  //=16
		m_nRegionHeight = KScenePlaceRegionC::RWPP_AREGION_HEIGHT / 32;//=32
		m_nCellWidth    = defLOGIC_CELL_WIDTH;
		m_nCellHeight   = defLOGIC_CELL_HEIGHT;
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_RegionIndex = -1;
	}

	int nX = LOWORD(nRegion);
	int nY = HIWORD(nRegion);

	int nIdx = FindRegion(nRegion);

	if (nIdx < 0)
	{//主人的 当前区域
		nIdx = m_ClientRegionIdx[0];

		if (m_Region[nIdx].Load(nX, nY))
		{
			m_Region[nIdx].m_nIndex = nIdx;
			m_Region[nIdx].Init(m_nRegionWidth, m_nRegionHeight);
			m_Region[nIdx].LoadObjectC(0, nX, nY, m_szMapPath);
		}
	}
	//设置焦点（人物）位置
	g_ScenePlace.SetFocusPosition(m_Region[nIdx].m_nRegionX, m_Region[nIdx].m_nRegionY, 0);

	m_ClientRegionIdx[0] = nIdx;

	for (int i = 0; i < 8; i++)
	{//链接相邻近的 几个区域
		int nConIdx;
		nConIdx = FindRegion(m_Region[nIdx].m_nConRegionID[i]);

		if (nConIdx < 0)
		{
			nConIdx = FindFreeRegion(nX, nY);
			_ASSERT(nConIdx >= 0);

			if (m_Region[nConIdx].Load(nX + nXOff[i], nY + nYOff[i]))
			{
				m_Region[nConIdx].m_nIndex = nConIdx;
				m_Region[nConIdx].Init(m_nRegionWidth, m_nRegionHeight);
				m_Region[nConIdx].LoadObjectC(0, nX + nXOff[i], nY + nYOff[i], m_szMapPath);
			}
			else
			{
				m_Region[nConIdx].m_nIndex = -1;
				m_Region[nConIdx].m_RegionID = -1;
				nConIdx = -1;
			}
		}
		m_ClientRegionIdx[i + 1] = nConIdx;
		m_Region[nIdx].m_nConnectRegion[i] = nConIdx;
		m_Region[nIdx].m_pConRegion[i] = &m_Region[nConIdx];
	}

	if (m_Region[nIdx].m_nConnectRegion[0] >= 0)
	{
		m_Region[m_Region[nIdx].m_nConnectRegion[0]].m_nConnectRegion[0] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[0]].m_nConnectRegion[1] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[0]].m_nConnectRegion[7] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[0]].m_nConnectRegion[4] = nIdx;
		m_Region[m_Region[nIdx].m_nConnectRegion[0]].m_nConnectRegion[2] = m_Region[nIdx].m_nConnectRegion[1];
		m_Region[m_Region[nIdx].m_nConnectRegion[0]].m_nConnectRegion[3] = m_Region[nIdx].m_nConnectRegion[2];
		m_Region[m_Region[nIdx].m_nConnectRegion[0]].m_nConnectRegion[5] = m_Region[nIdx].m_nConnectRegion[6];
		m_Region[m_Region[nIdx].m_nConnectRegion[0]].m_nConnectRegion[6] = m_Region[nIdx].m_nConnectRegion[7];
	
		m_Region[m_Region[nIdx].m_nConnectRegion[0]].m_pConRegion[0] = NULL;
		m_Region[m_Region[nIdx].m_nConnectRegion[0]].m_pConRegion[1] = NULL;
		m_Region[m_Region[nIdx].m_nConnectRegion[0]].m_pConRegion[7] = NULL;
		m_Region[m_Region[nIdx].m_nConnectRegion[0]].m_pConRegion[4] = &m_Region[nIdx];
		m_Region[m_Region[nIdx].m_nConnectRegion[0]].m_pConRegion[2] = m_Region[nIdx].m_pConRegion[1];
		m_Region[m_Region[nIdx].m_nConnectRegion[0]].m_pConRegion[3] = m_Region[nIdx].m_pConRegion[2];
		m_Region[m_Region[nIdx].m_nConnectRegion[0]].m_pConRegion[5] = m_Region[nIdx].m_pConRegion[6];
		m_Region[m_Region[nIdx].m_nConnectRegion[0]].m_pConRegion[6] = m_Region[nIdx].m_pConRegion[7];
	}
	m_Region[m_Region[nIdx].m_nConnectRegion[0]].m_pConRegion[2] = m_Region[nIdx].m_pConRegion[1];

	if (m_Region[nIdx].m_nConnectRegion[1] >= 0)
	{
		m_Region[m_Region[nIdx].m_nConnectRegion[1]].m_nConnectRegion[0] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[1]].m_nConnectRegion[1] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[1]].m_nConnectRegion[7] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[1]].m_nConnectRegion[4] = m_Region[nIdx].m_nConnectRegion[2];
		m_Region[m_Region[nIdx].m_nConnectRegion[1]].m_nConnectRegion[2] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[1]].m_nConnectRegion[3] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[1]].m_nConnectRegion[5] = nIdx;
		m_Region[m_Region[nIdx].m_nConnectRegion[1]].m_nConnectRegion[6] = m_Region[nIdx].m_nConnectRegion[0];
	    
		m_Region[m_Region[nIdx].m_nConnectRegion[1]].m_pConRegion[0] = NULL;
		m_Region[m_Region[nIdx].m_nConnectRegion[1]].m_pConRegion[1] = NULL;
		m_Region[m_Region[nIdx].m_nConnectRegion[1]].m_pConRegion[7] = NULL;
		m_Region[m_Region[nIdx].m_nConnectRegion[1]].m_pConRegion[4] = m_Region[nIdx].m_pConRegion[2];
		m_Region[m_Region[nIdx].m_nConnectRegion[1]].m_pConRegion[2] = NULL;
		m_Region[m_Region[nIdx].m_nConnectRegion[1]].m_pConRegion[3] = NULL;
		m_Region[m_Region[nIdx].m_nConnectRegion[1]].m_pConRegion[5] = &m_Region[nIdx];
		m_Region[m_Region[nIdx].m_nConnectRegion[1]].m_pConRegion[6] = m_Region[nIdx].m_pConRegion[0];
	}

	if (m_Region[nIdx].m_nConnectRegion[2] >= 0)
	{
		m_Region[m_Region[nIdx].m_nConnectRegion[2]].m_nConnectRegion[0] = m_Region[nIdx].m_nConnectRegion[1];
		m_Region[m_Region[nIdx].m_nConnectRegion[2]].m_nConnectRegion[1] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[2]].m_nConnectRegion[7] = m_Region[nIdx].m_nConnectRegion[0];
		m_Region[m_Region[nIdx].m_nConnectRegion[2]].m_nConnectRegion[4] = m_Region[nIdx].m_nConnectRegion[3];
		m_Region[m_Region[nIdx].m_nConnectRegion[2]].m_nConnectRegion[2] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[2]].m_nConnectRegion[3] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[2]].m_nConnectRegion[5] = m_Region[nIdx].m_nConnectRegion[4];
		m_Region[m_Region[nIdx].m_nConnectRegion[2]].m_nConnectRegion[6] = nIdx;

		m_Region[m_Region[nIdx].m_nConnectRegion[2]].m_pConRegion[0] = m_Region[nIdx].m_pConRegion[1];
		m_Region[m_Region[nIdx].m_nConnectRegion[2]].m_pConRegion[1] = NULL;
		m_Region[m_Region[nIdx].m_nConnectRegion[2]].m_pConRegion[7] = m_Region[nIdx].m_pConRegion[0];
		m_Region[m_Region[nIdx].m_nConnectRegion[2]].m_pConRegion[4] = m_Region[nIdx].m_pConRegion[3];
		m_Region[m_Region[nIdx].m_nConnectRegion[2]].m_pConRegion[2] = NULL;
		m_Region[m_Region[nIdx].m_nConnectRegion[2]].m_pConRegion[3] = NULL;
		m_Region[m_Region[nIdx].m_nConnectRegion[2]].m_pConRegion[5] = m_Region[nIdx].m_pConRegion[4];
		m_Region[m_Region[nIdx].m_nConnectRegion[2]].m_pConRegion[6] = &m_Region[nIdx];
	}

	if (m_Region[nIdx].m_nConnectRegion[3] >= 0)
	{
		m_Region[m_Region[nIdx].m_nConnectRegion[3]].m_nConnectRegion[0] = m_Region[nIdx].m_nConnectRegion[2];
		m_Region[m_Region[nIdx].m_nConnectRegion[3]].m_nConnectRegion[1] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[3]].m_nConnectRegion[2] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[3]].m_nConnectRegion[3] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[3]].m_nConnectRegion[4] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[3]].m_nConnectRegion[5] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[3]].m_nConnectRegion[6] = m_Region[nIdx].m_nConnectRegion[4];
		m_Region[m_Region[nIdx].m_nConnectRegion[3]].m_nConnectRegion[7] = nIdx;

		m_Region[m_Region[nIdx].m_nConnectRegion[3]].m_pConRegion[0] = m_Region[nIdx].m_pConRegion[2];
		m_Region[m_Region[nIdx].m_nConnectRegion[3]].m_pConRegion[1] = NULL;
		m_Region[m_Region[nIdx].m_nConnectRegion[3]].m_pConRegion[2] = NULL;
		m_Region[m_Region[nIdx].m_nConnectRegion[3]].m_pConRegion[3] = NULL;
		m_Region[m_Region[nIdx].m_nConnectRegion[3]].m_pConRegion[4] = NULL;
		m_Region[m_Region[nIdx].m_nConnectRegion[3]].m_pConRegion[5] = NULL;
		m_Region[m_Region[nIdx].m_nConnectRegion[3]].m_pConRegion[6] = m_Region[nIdx].m_pConRegion[4];
		m_Region[m_Region[nIdx].m_nConnectRegion[3]].m_pConRegion[7] = &m_Region[nIdx];
	}

	if (m_Region[nIdx].m_nConnectRegion[4] >= 0)
	{
		m_Region[m_Region[nIdx].m_nConnectRegion[4]].m_nConnectRegion[0] = nIdx;
		m_Region[m_Region[nIdx].m_nConnectRegion[4]].m_nConnectRegion[1] = m_Region[nIdx].m_nConnectRegion[2];
		m_Region[m_Region[nIdx].m_nConnectRegion[4]].m_nConnectRegion[2] = m_Region[nIdx].m_nConnectRegion[3];
		m_Region[m_Region[nIdx].m_nConnectRegion[4]].m_nConnectRegion[3] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[4]].m_nConnectRegion[4] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[4]].m_nConnectRegion[5] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[4]].m_nConnectRegion[6] = m_Region[nIdx].m_nConnectRegion[5];
		m_Region[m_Region[nIdx].m_nConnectRegion[4]].m_nConnectRegion[7] = m_Region[nIdx].m_nConnectRegion[6];
	
		m_Region[m_Region[nIdx].m_nConnectRegion[4]].m_pConRegion[0] = &m_Region[nIdx];
		m_Region[m_Region[nIdx].m_nConnectRegion[4]].m_pConRegion[1] = m_Region[nIdx].m_pConRegion[2];
		m_Region[m_Region[nIdx].m_nConnectRegion[4]].m_pConRegion[2] = m_Region[nIdx].m_pConRegion[3];
		m_Region[m_Region[nIdx].m_nConnectRegion[4]].m_pConRegion[3] = NULL;
		m_Region[m_Region[nIdx].m_nConnectRegion[4]].m_pConRegion[4] = NULL;
		m_Region[m_Region[nIdx].m_nConnectRegion[4]].m_pConRegion[5] = NULL;
		m_Region[m_Region[nIdx].m_nConnectRegion[4]].m_pConRegion[6] = m_Region[nIdx].m_pConRegion[5];
		m_Region[m_Region[nIdx].m_nConnectRegion[4]].m_pConRegion[7] = m_Region[nIdx].m_pConRegion[6];
	}

	if (m_Region[nIdx].m_nConnectRegion[5] >= 0)
	{
		m_Region[m_Region[nIdx].m_nConnectRegion[5]].m_nConnectRegion[0] = m_Region[nIdx].m_nConnectRegion[6];
		m_Region[m_Region[nIdx].m_nConnectRegion[5]].m_nConnectRegion[1] = nIdx;
		m_Region[m_Region[nIdx].m_nConnectRegion[5]].m_nConnectRegion[2] = m_Region[nIdx].m_nConnectRegion[4];
		m_Region[m_Region[nIdx].m_nConnectRegion[5]].m_nConnectRegion[3] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[5]].m_nConnectRegion[4] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[5]].m_nConnectRegion[5] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[5]].m_nConnectRegion[6] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[5]].m_nConnectRegion[7] = -1;

		m_Region[m_Region[nIdx].m_nConnectRegion[5]].m_pConRegion[0] = m_Region[nIdx].m_pConRegion[6];
		m_Region[m_Region[nIdx].m_nConnectRegion[5]].m_pConRegion[1] = &m_Region[nIdx];
		m_Region[m_Region[nIdx].m_nConnectRegion[5]].m_pConRegion[2] = m_Region[nIdx].m_pConRegion[4];
		m_Region[m_Region[nIdx].m_nConnectRegion[5]].m_pConRegion[3] = NULL;
		m_Region[m_Region[nIdx].m_nConnectRegion[5]].m_pConRegion[4] = NULL;
		m_Region[m_Region[nIdx].m_nConnectRegion[5]].m_pConRegion[5] = NULL;
		m_Region[m_Region[nIdx].m_nConnectRegion[5]].m_pConRegion[6] = NULL;
		m_Region[m_Region[nIdx].m_nConnectRegion[5]].m_pConRegion[7] = NULL;
	}

	if (m_Region[nIdx].m_nConnectRegion[6] >= 0)
	{
		m_Region[m_Region[nIdx].m_nConnectRegion[6]].m_nConnectRegion[0] = m_Region[nIdx].m_nConnectRegion[7];
		m_Region[m_Region[nIdx].m_nConnectRegion[6]].m_nConnectRegion[1] = m_Region[nIdx].m_nConnectRegion[0];
		m_Region[m_Region[nIdx].m_nConnectRegion[6]].m_nConnectRegion[2] = nIdx;
		m_Region[m_Region[nIdx].m_nConnectRegion[6]].m_nConnectRegion[3] = m_Region[nIdx].m_nConnectRegion[4];
		m_Region[m_Region[nIdx].m_nConnectRegion[6]].m_nConnectRegion[4] = m_Region[nIdx].m_nConnectRegion[5];
		m_Region[m_Region[nIdx].m_nConnectRegion[6]].m_nConnectRegion[5] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[6]].m_nConnectRegion[6] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[6]].m_nConnectRegion[7] = -1;

		m_Region[m_Region[nIdx].m_nConnectRegion[6]].m_pConRegion[0] = m_Region[nIdx].m_pConRegion[7];
		m_Region[m_Region[nIdx].m_nConnectRegion[6]].m_pConRegion[1] = m_Region[nIdx].m_pConRegion[0];
		m_Region[m_Region[nIdx].m_nConnectRegion[6]].m_pConRegion[2] = &m_Region[nIdx];
		m_Region[m_Region[nIdx].m_nConnectRegion[6]].m_pConRegion[3] = m_Region[nIdx].m_pConRegion[4];
		m_Region[m_Region[nIdx].m_nConnectRegion[6]].m_pConRegion[4] = m_Region[nIdx].m_pConRegion[5];
		m_Region[m_Region[nIdx].m_nConnectRegion[6]].m_pConRegion[5] = NULL;
		m_Region[m_Region[nIdx].m_nConnectRegion[6]].m_pConRegion[6] = NULL;
		m_Region[m_Region[nIdx].m_nConnectRegion[6]].m_pConRegion[7] = NULL;
	}
	
	if (m_Region[nIdx].m_nConnectRegion[7] >= 0)
	{
		m_Region[m_Region[nIdx].m_nConnectRegion[7]].m_nConnectRegion[0] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[7]].m_nConnectRegion[1] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[7]].m_nConnectRegion[2] = m_Region[nIdx].m_nConnectRegion[0];
		m_Region[m_Region[nIdx].m_nConnectRegion[7]].m_nConnectRegion[3] = nIdx;
		m_Region[m_Region[nIdx].m_nConnectRegion[7]].m_nConnectRegion[4] = m_Region[nIdx].m_nConnectRegion[6];
		m_Region[m_Region[nIdx].m_nConnectRegion[7]].m_nConnectRegion[5] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[7]].m_nConnectRegion[6] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[7]].m_nConnectRegion[7] = -1;

		m_Region[m_Region[nIdx].m_nConnectRegion[7]].m_pConRegion[0] = NULL;
		m_Region[m_Region[nIdx].m_nConnectRegion[7]].m_pConRegion[1] = NULL;
		m_Region[m_Region[nIdx].m_nConnectRegion[7]].m_pConRegion[2] = m_Region[nIdx].m_pConRegion[0];
		m_Region[m_Region[nIdx].m_nConnectRegion[7]].m_pConRegion[3] = &m_Region[nIdx];
		m_Region[m_Region[nIdx].m_nConnectRegion[7]].m_pConRegion[4] = m_Region[nIdx].m_pConRegion[6];
		m_Region[m_Region[nIdx].m_nConnectRegion[7]].m_pConRegion[5] = NULL;
		m_Region[m_Region[nIdx].m_nConnectRegion[7]].m_pConRegion[6] = NULL;
		m_Region[m_Region[nIdx].m_nConnectRegion[7]].m_pConRegion[7] = NULL;
	}

	return TRUE;
}
#endif

void KSubWorld::LoadTrap()
{

}

void KSubWorld::MessageLoop()
{
	KWorldMsgNode	Msg;
	while(m_WorldMessage.Get(&Msg))
	{
		try
		{
		   ProcessMsg(&Msg);
		}
		catch (...)
		{
#ifdef _SERVER
		   printf("--MessageLoop--\n");
#endif
		}

	}
}

void KSubWorld::ProcessMsg(KWorldMsgNode *pMsg)
{
	switch(pMsg->m_dwMsgType)
	{
	case GWM_NPC_DEL:
//#ifndef _SERVER
		if  (pMsg->m_nParam[0]<=0 || pMsg->m_nParam[0]>=MAX_NPC)
			break;

		if (Npc[pMsg->m_nParam[0]].m_RegionIndex >= 0)
		{
			int nIdx = pMsg->m_nParam[0];

			if (nIdx<=0)
				break;

			int nSubWorld = Npc[nIdx].m_SubWorldIndex;
			int nRegion = Npc[nIdx].m_RegionIndex;

			if (nSubWorld>=0 && nRegion>=0)
			{
				SubWorld[nSubWorld].m_Region[nRegion].RemoveNpc(nIdx);
				SubWorld[nSubWorld].m_Region[nRegion].DecRef(Npc[nIdx].m_MapX, Npc[nIdx].m_MapY,obj_npc);
				//SubWorld[nSubWorld].m_Region[nRegion].DecNpcRef(Npc[nIdx].m_MapX, Npc[nIdx].m_MapY);
			}
			
		}
		NpcSet.Remove(pMsg->m_nParam[0]);
/*#else
		{
			int nIdx = pMsg->m_nParam[0];
			int nSubWorld = Npc[nIdx].m_SubWorldIndex;
			int nRegion = Npc[nIdx].m_RegionIndex;
			if  (nRegion>=0)
			{
			   SubWorld[nSubWorld].m_Region[nRegion].RemoveNpc(nIdx);
			   SubWorld[nSubWorld].m_Region[nRegion].DecRef(Npc[nIdx].m_MapX, Npc[nIdx].m_MapY, obj_npc);
			}

			NpcSet.Remove(nIdx);
		}
#endif */
		break;
	//case GWM_NPC_CHANGE_REGION:
	//	NpcChangeRegion(pMsg->m_nParam[0], pMsg->m_nParam[1], pMsg->m_nParam[2]);
	//	break;
	case GWM_OBJ_DEL:

		if  (pMsg->m_nParam[0]<=0 || pMsg->m_nParam[0]>=MAX_OBJECT)
			break;
//#ifndef _SERVER
		if (Object[pMsg->m_nParam[0]].m_nRegionIdx >= 0)
			SubWorld[Object[pMsg->m_nParam[0]].m_nSubWorldID].m_Region[Object[pMsg->m_nParam[0]].m_nRegionIdx].RemoveObj(pMsg->m_nParam[0]);
		
		ObjSet.Remove(pMsg->m_nParam[0]);
/*#else
		if  (Object[pMsg->m_nParam[0]].m_nRegionIdx>=0)
		{
		   SubWorld[Object[pMsg->m_nParam[0]].m_nSubWorldID].m_Region[Object[pMsg->m_nParam[0]].m_nRegionIdx].RemoveObj(pMsg->m_nParam[0]);
		}
		ObjSet.Remove(pMsg->m_nParam[0]);
#endif */
		break;
	case GWM_OBJ_CHANGE_REGION:
		ObjChangeRegion(pMsg->m_nParam[0], pMsg->m_nParam[1], pMsg->m_nParam[2]);
		break;
	case GWM_MISSLE_CHANGE_REGION:
		MissleChangeRegion(pMsg->m_nParam[0], pMsg->m_nParam[1], pMsg->m_nParam[2]);
		break;
	case GWM_MISSLE_DEL:
		if  (pMsg->m_nParam[0]<=0 || pMsg->m_nParam[0]>=MAX_MISSLE)
			break;
//#ifndef _SERVER
		if (Missle[pMsg->m_nParam[0]].m_nRegionId >= 0)
		{
			SubWorld[Missle[pMsg->m_nParam[0]].m_nSubWorldId].m_Region[Missle[pMsg->m_nParam[0]].m_nRegionId].RemoveMissle(pMsg->m_nParam[0]);
	//		g_DebugLog("[Missle]Missle%dDEL", pMsg->m_nParam[0]);
			MissleSet.Remove(pMsg->m_nParam[0]);
		}
/*#else
		if (Missle[pMsg->m_nParam[0]].m_nRegionId >= 0)
		{
		   SubWorld[Missle[pMsg->m_nParam[0]].m_nSubWorldId].m_Region[Missle[pMsg->m_nParam[0]].m_nRegionId].RemoveMissle(pMsg->m_nParam[0]);
		}
		   MissleSet.Remove(pMsg->m_nParam[0]);
#endif*/
		break;
	//case GWM_PLAYER_CHANGE_REGION:
	//	PlayerChangeRegion(pMsg->m_nParam[0], pMsg->m_nParam[1], pMsg->m_nParam[2]);
	//	break;
	default:
		break;
	}	
}

#ifdef _SERVER
void KSubWorld::NpcChangeRegion(int nSrcRnidx, int nDesRnIdx, int nIdx)
{
	if (nIdx <= 0 || nIdx >= MAX_NPC)
		return;

	Npc[nIdx].SetActiveFlag(TRUE);  //设定不执行 循环处理

	if (nSrcRnidx == -1)
	{
	//	_ASSERT(nDesRnIdx >= 0);
		if (nDesRnIdx >= 0)
		{
			m_Region[nDesRnIdx].AddNpc(nIdx);
			Npc[nIdx].m_RegionIndex = nDesRnIdx;
		}
		return;
	}
	else if (nSrcRnidx == VOID_REGION)
	{//那些开始重生的NPC
		Npc[nIdx].m_Node.Remove();
		Npc[nIdx].m_Node.Release();
	}
	else if (nSrcRnidx >= 0)
	{
		SubWorld[Npc[nIdx].m_SubWorldIndex].m_Region[nSrcRnidx].RemoveNpc(nIdx);
		if (nDesRnIdx != VOID_REGION)	// 不是加入到死亡重生链表
			Npc[nIdx].m_RegionIndex = -1;
	}

	if (nDesRnIdx >= 0)
	{
		m_Region[nDesRnIdx].AddNpc(nIdx);
		Npc[nIdx].m_RegionIndex = nDesRnIdx;
	}
	else if (nDesRnIdx == VOID_REGION)
	{//那些死亡的 不在区域内的NPC
		m_NoneRegionNpcList.AddTail(&Npc[nIdx].m_Node);		//加入不在区域列表 以便重生
		Npc[nIdx].m_Node.AddRef();	//数量增加
	}
}
#endif

#ifndef _SERVER
void KSubWorld::NpcChangeRegion(int nSrcRnidx, int nDesRnIdx, int nIdx)
{
	int		nSrc, nDest;

	if (nSrcRnidx == -1)
	{
		nDest = SubWorld[0].FindRegion(nDesRnIdx);
		if (nDest < 0)
			return;
		m_Region[nDest].AddNpc(nIdx);
		Npc[nIdx].m_dwRegionID = m_Region[nDest].m_RegionID;
		Npc[nIdx].m_RegionIndex = nDest;
		return;
	}

	nSrc = SubWorld[0].FindRegion(nSrcRnidx);
	if (nSrc >= 0)
		SubWorld[0].m_Region[nSrc].RemoveNpc(nIdx);

	nDest = SubWorld[0].FindRegion(nDesRnIdx);

	KIndexNode *pNode = &Npc[nIdx].m_Node;
	if (nDest >= 0)
	{
		m_Region[nDest].AddNpc(nIdx);
		
		if (Player[CLIENT_PLAYER_INDEX].m_nIndex == nIdx)
			LoadMapC(m_SubWorldID, m_Region[nDest].m_RegionID);
		Npc[nIdx].m_dwRegionID = m_Region[nDest].m_RegionID;
		Npc[nIdx].m_RegionIndex = nDest;
	}
	else if (Player[CLIENT_PLAYER_INDEX].m_nIndex == nIdx)
	{
		LoadMapC(m_SubWorldID, nDesRnIdx);
		nDest = SubWorld[0].FindRegion(nDesRnIdx);
		if (nDest >= 0)
		{
			m_Region[nDest].AddNpc(nIdx);
			Npc[nIdx].m_dwRegionID = m_Region[nDest].m_RegionID;
			Npc[nIdx].m_RegionIndex = nDest;
		}
	}
}
#endif

void KSubWorld::ObjChangeRegion(int nSrcRnidx, int nDesRnIdx, int nIdx)
{
//#ifdef _SERVER
//	_ASSERT(nSrcRnidx >= 0);
//#endif

	if (nIdx<=0 || nIdx>=MAX_OBJECT)
	   return;

	if (nDesRnIdx == -1 && nSrcRnidx>=0)
	{
		SubWorld[Object[nIdx].m_nSubWorldID].m_Region[nSrcRnidx].RemoveObj(nIdx);
//		ObjSet.Remove(nIdx);
		return;
	}

	if (nSrcRnidx == -1 && nDesRnIdx>=0)
	{
		m_Region[nDesRnIdx].AddObj(nIdx);
		return;
	}

	/*KRegion::VEC_LIST::iterator it = m_Region[nSrcRnidx].GetObjNode(nIdx);
	if (it != m_Region[nSrcRnidx].m_ObjList.end())
	{
		m_Region[nSrcRnidx].m_ObjList.erase(it);
		m_Region[nDesRnIdx].m_ObjList.push_back(nIdx);
	}
	*/
	KIndexNode* pNode = (KIndexNode *)m_Region[nSrcRnidx].GetObjNode(nIdx);
	if (pNode)
	{
		pNode->Remove();
		if (nDesRnIdx>=0)
		   m_Region[nDesRnIdx].m_ObjList.AddTail(pNode);
	}
}

void KSubWorld::MissleChangeRegion(int nSrcRnidx, int nDesRnIdx, int nIdx)
{
	if (nIdx <= 0 || nIdx >= MAX_MISSLE)
		return;

	if (nSrcRnidx == -1 && nDesRnIdx>=0)
	{
		m_Region[nDesRnIdx].AddMissle(nIdx);
		return;
	}
	if (nSrcRnidx>=0 && Missle[nIdx].m_nSubWorldId>=0 && Missle[nIdx].m_nSubWorldId<MAX_SUBWORLD)
	   SubWorld[Missle[nIdx].m_nSubWorldId].m_Region[nSrcRnidx].RemoveMissle(nIdx);

	if (nDesRnIdx == -1)
	{
		MissleSet.Remove(nIdx);
		return;
	}

	if (nDesRnIdx>=0)
	   m_Region[nDesRnIdx].AddMissle(nIdx);
}

#ifdef _SERVER
void KSubWorld::PlayerChangeRegion(int nSrcRnidx, int nDesRnIdx, int nIdx)
{
	if (nIdx <= 0 || nIdx >= MAX_PLAYER)
		return;

	if (nSrcRnidx < 0)
		return;

	RemovePlayer(nSrcRnidx, nIdx);
	
	if (nDesRnIdx == -1)
	{
		//PlayerSet.PrepareRemove(Player[nIdx].m_nNetConnectIdx);
		printf("Player change region to not exist..., kill it\n");
		g_pServer->ShutdownClient(Player[nIdx].m_nNetConnectIdx);
		return;
	}

	if (nDesRnIdx >= 0)
	{
		AddPlayer(nDesRnIdx, nIdx);
	}
/* 因为RemovePlayer和AddPlayer已经做了从链表中清除节点，修改周边九个REGION的引用计数的操作了
	KIndexNode* pNode = &Player[nIdx].m_Node;
	if (pNode->m_Ref > 0)
	{
		pNode->Remove();
		pNode->Release();
	}
	m_Region[nSrcRnidx].m_nActive--;
	if (m_Region[nSrcRnidx].m_nActive < 0)
		m_Region[nSrcRnidx].m_nActive = 0;
	
	for (int i = 0; i < 8; i++)
	{
		if (m_Region[nSrcRnidx].m_nConnectRegion[i] < 0)
			continue;
		m_Region[m_Region[nSrcRnidx].m_nConnectRegion[i]].m_nActive--;
		if (m_Region[m_Region[nSrcRnidx].m_nConnectRegion[i]].m_nActive < 0)
			m_Region[m_Region[nSrcRnidx].m_nConnectRegion[i]].m_nActive = 0;
	}

	if (pNode->m_Ref == 0)
	{
		m_Region[nDesRnIdx].m_PlayerList.AddTail(pNode);
		pNode->AddRef();
	}

	m_Region[nDesRnIdx].m_nActive++;
	for (i = 0; i < 8; i++)
	{
		if (m_Region[nDesRnIdx].m_nConnectRegion[i] < 0)
			continue;
		m_Region[m_Region[nDesRnIdx].m_nConnectRegion[i]].m_nActive++;
		if (m_Region[m_Region[nDesRnIdx].m_nConnectRegion[i]].m_nActive < 0)
			m_Region[m_Region[nDesRnIdx].m_nConnectRegion[i]].m_nActive = 0;
	}
*/
}
#endif

void KSubWorld::GetMps(int *nX, int *nY, int nSpeed, int nDir, int nMaxDir /* = 64 */)
{
	/*if (nDir < 0)
		nDir = MaxMissleDir +nDir;
	
	if (nDir >= MaxMissleDir)
		nDir -= MaxMissleDir;*/
	int n;
	if (nDir < 0 )
	{
		if (nDir>=-MaxMissleDir)
			nDir = MaxMissleDir + nDir;
		else
		{//-129;
			n=-nDir%MaxMissleDir;//		 -1
			nDir = MaxMissleDir-n-1;
		}
		//nWantDir = MaxMissleDir+ nWantDir;
	}
	
	if (nDir >= MaxMissleDir)
	{
		n=nDir/MaxMissleDir;
		nDir -= MaxMissleDir*n;
	} 

	*nX += ((g_DirCos(nDir, nMaxDir) * nSpeed) >> 10);
	*nY += ((g_DirSin(nDir, nMaxDir) * nSpeed) >> 10);
}
//服务器端发送同步数据到客户端-服务器端--地图世界数据
#ifdef _SERVER
BOOL KSubWorld::SendSyncData(int nIdx, int nClient)
{
	WORLD_SYNC	WorldSync;
	WorldSync.ProtocolType = (BYTE)s2c_syncworld;
	WorldSync.SRegion      = m_Region[Npc[nIdx].m_RegionIndex].m_RegionID;
	WorldSync.Weather      = m_nWeather;
	WorldSync.Frame        = m_dwCurrentTime;  //同步时间帧
	WorldSync.SubWorld     = m_SubWorldID;     //地图ID
	WorldSync.m_GameStat   = m_GuziGameStat;
	char nMapType[32]={0},mMapType[32]={0};
	WorldSync.m_IsShowLoop = m_IsShowLoop;

    sprintf(nMapType,"%d_MapType",m_SubWorldID);
    g_NpcMapDropRate.GetString("List",nMapType,"无",mMapType,sizeof(mMapType));
	sprintf(WorldSync.WarMaster,"%s",nWorldMapInfo[STR_MAP_MASTER].c_str());   //城主
    if (strstr(mMapType,"City"))
	{	
		sprintf(WorldSync.WarTong,"%s",nWorldMapInfo[STR_MAP_ZTONGNAME].c_str());  //占领帮派
		sprintf(WorldSync.WarGongTong,"%s",nWorldMapInfo[STR_MAP_TGONGNAME].c_str());  //攻方
		sprintf(WorldSync.WarShouTong,"%s",nWorldMapInfo[STR_MAP_TSHOUNAME].c_str());  //
		
		WorldSync.WarIsWho=m_WarIsWho;
   
		WorldSync.WPKFlag=m_nWPKFlag;   //Pk模式
		WorldSync.mShuiType = m_ShuiShouType;

		WorldSync.IsWarCity=m_IswarCity;

		WorldSync.WarCityManey=m_WarCityManey;//城市建设资金
		WorldSync.WarCityJB=m_WarCityJB;
		WorldSync.WarCityGX=m_WarCityGX;
    }
	if (SUCCEEDED(g_pServer->PackDataToClient(nClient, (BYTE*)&WorldSync, sizeof(WORLD_SYNC))))   //发送到客户端
		return TRUE;
	else
	{
		printf("player Packing 世界地图同步world sync data failed...\n");
		return FALSE;
	}
}
#endif

#ifndef _SERVER
void KSubWorld::LoadCell()
{
}
#endif

#ifdef TOOLVERSION
int CORE_API g_ScreenX  = 0;
int CORE_API g_ScreenY  = 0;
#endif

#ifndef _SERVER
void KSubWorld::Paint()
{
	int nIdx = Player[CLIENT_PLAYER_INDEX].m_nIndex;

	int nX, nY;

	NewMap2Mps(Npc[nIdx].m_RegionIndex, Npc[nIdx].m_MapX, Npc[nIdx].m_MapY, Npc[nIdx].m_OffX, Npc[nIdx].m_OffY, &nX, &nY);
	
//	m_nScreenX = nX - 400;
//	m_nScreenY = nY - 300;

#ifdef TOOLVERSION
	g_ScreenX = nX;
	g_ScreenY = nY;
#endif
/*
	for (int i = 0; i < m_nTotalRegion; i++)
	{
		m_Region[i].Paint();
	}*/
}

void KSubWorld::Mps2Screen(int *Rx, int *Ry)
{

}

void KSubWorld::Screen2Mps(int *Rx, int *Ry)
{

}
#endif

#ifdef _SERVER
int KSubWorld::GetRegionIndex(int nRegionID)
{
	int nRet;
	short nX = (short)LOWORD(nRegionID) - m_nRegionBeginX;
	short nY = (short)HIWORD(nRegionID) - m_nRegionBeginY;

	if (nX < 0 || nY < 0 || nX >= m_nWorldRegionWidth || nY >= m_nWorldRegionHeight)
		return -1;

	nRet = nY * m_nWorldRegionWidth + nX;
	return nRet;
}
#endif

void KSubWorld::Close()
{
	if (m_SubWorldID == -1)
		return;

	for (int i = 0; i < m_nTotalRegion; i++)
	{
		m_Region[i].Close();
	}
	m_WorldMessage.Clear();
	m_nIndex = -1;
	m_SubWorldID = -1;
}

#ifdef _SERVER
void KSubWorld::LoadObject(char* szPath, char* szFileName)
{
	if (!szFileName || !szFileName[0])
		return;

	KIniFile IniFile;
	// 加载世界中的动态物件（Npc、Object、Trap）
	g_SetFilePath(szPath);
	int nObjNumber = 0;
	if (IniFile.Load(szFileName))
	{
		IniFile.GetInteger("MAIN", "elementnum", 0, &nObjNumber);
	}

	int nLength = strlen(szFileName);
	// the four bytes this line overwrites are the dot and the extension, and `nLength' is
	// the name's own measured length: a shorter name has no extension byte, so the write
	// lands before the buffer the caller handed over - and the path built on the following
	// lines would name a file that has no data half anyway.
	if (nLength < 4)
		return;
	szFileName[nLength - 4] = 0;
	strcat(szPath, "\\");
	strcat(szPath, szFileName);

	char	szSection[32]={0};
	char	szDataFile[32]={0};

	for (int i = 0; i < nObjNumber; i++)
	{
		float	fPos[3];
		int		nPos[3];

		g_SetFilePath(szPath);
		sprintf(szSection, "%d", i);
		IniFile.GetFloat3(szSection, "groundoffset", fPos);
		nPos[0] = (int)(fPos[0] * 32);
		nPos[1] = (int)(fPos[1] * 32);
		nPos[2] = (int)fPos[2];

		szDataFile[0] = 0;
		IniFile.GetString(szSection, "event", "", szDataFile, sizeof(szDataFile));

		IniFile.Clear()	;

		strcat(szDataFile, "_data.ini");
		KIniFile IniChange;
		int nType = 0, nLevel = 0;
		char	szName[32]={0};
		
		if (!IniChange.Load(szDataFile))
			continue;
		
		IniChange.GetInteger("MAIN", "Type", 0, &nType);
		IniChange.GetInteger("MAIN", "Level", 4, &nLevel);
		IniChange.GetString("MAIN", "mapedit_templatesection", "", szName, sizeof(szName));
		
		switch(nType)
		{
		case kind_normal:		// 普通战斗npc   kind =0
			{
			/*	int nFindNo = g_NpcSetting.FindRow(szName);
				if (nFindNo == -1)
					continue;
				else
					nFindNo = nFindNo - 2;
				
				int nSettingInfo = MAKELONG(nLevel, nFindNo);
				NpcSet.AddServerNpcA(nSettingInfo, m_nIndex, nPos[0], nPos[1],0,0,0,0,0,0);	*/
			}
			break;
		case kind_player:
			break;
		case kind_partner:
			break;
		case kind_dialoger:		// 普通非战斗npc
			break;
		case kind_bird:			// 客户端only
			break;
		case kind_mouse:		// 客户端only
			break;
		default:
			break;
		}

		IniChange.Clear();
	}
}
#endif

bool KSubWorld::AddPlayer(int nRegion, int nIdx)
{
	if (nRegion < 0 || nRegion >= m_nTotalRegion)
	{
		printf("---错误ARegion:(%d) Player(%d) TotalRegion(%d)---\n", nRegion, nIdx,m_nTotalRegion);
		return false;
	}

	if (m_Region[nRegion].AddPlayer(nIdx))
	{
		m_Region[nRegion].m_nActive++;

		for (int i = 0; i < 8; i++)
		{
			int nConRegion = m_Region[nRegion].m_nConnectRegion[i];//9
			if (nConRegion == -1)
				continue;
			
			m_Region[nConRegion].m_nActive++;  //激活的区域加1
		}
	}
	else
	{
		printf("---错误BRegion:(%d) Player(%d) TotalRegion(%d)---\n", nRegion, nIdx,m_nTotalRegion);
		return false;
	}
	return true;
}

void KSubWorld::RemovePlayer(int nRegion, int nIdx)
{
	if (nRegion < 0 || nRegion >= m_nTotalRegion)
		return;

	if (m_Region[nRegion].RemovePlayer(nIdx))
	{	
		m_Region[nRegion].m_nActive--;		

		if (m_Region[nRegion].m_nActive < 0)
		{
			//_ASSERT(0);
			m_Region[nRegion].m_nActive = 0;
		}
		
		for (int i = 0; i < 8; i++)
		{
			int nConRegion = m_Region[nRegion].m_nConnectRegion[i];
			if (nConRegion == -1)
				continue;
			
			m_Region[nConRegion].m_nActive--;

			if (m_Region[nConRegion].m_nActive < 0)
			{
				//_ASSERT(0);
				m_Region[nConRegion].m_nActive = 0;
			}
		}
	}
}



void KSubWorld::GetFreePos(POINT& pos)
{
	POINT	posLocal = pos;
	POINT	posTemp;
	int nLayer = 1;

	//if (CanPutObj(posLocal,nModel)) //如果 NPC所在的地方能掉东西 就 返回
	//	return;

    int xPos=1,yPos=1,nCheckNum=512,nIsCheckNpcPos=0,nModel=0,nIsCheckNpc=0;

    if (nIsCheckNpcPos==1) //如果 NPC所在的地方能掉东西 就 返回
	{//检测NPC点
	   if (CanPutObj(posLocal,nModel,nIsCheckNpc))
		   return;
	}

	while(1)
	{
		for (int i = 0; i <= nLayer; i++)
		{
         	posTemp.y = posLocal.y;
			posTemp.x = posLocal.x+(i+1)*xPos;      //正右边

			if (CanPutObj(posTemp,nModel,nIsCheckNpc))
			{
				pos = posTemp;
				return;
			}
			else
			{
			}
     
	        posTemp.y = posLocal.y+(i+1)*yPos;
			posTemp.x = posLocal.x;                  //正下边 
			if (CanPutObj(posTemp,nModel,nIsCheckNpc))
			{
				pos = posTemp;
				return;
			}
			else
			{

			}

			posTemp.y = posLocal.y;
			posTemp.x = posLocal.x-(i+1)*xPos;       //正左边 

			if (CanPutObj(posTemp,nModel,nIsCheckNpc))
			{
				pos = posTemp;
				return;
			}
			else
			{

			}

			posTemp.y = posLocal.y-(i+1)*yPos;
			posTemp.x = posLocal.x;                  //正上边 
			if (CanPutObj(posTemp,nModel,nIsCheckNpc))
			{
				pos = posTemp;
				return;
			}
			else
			{

			}
			posTemp.y = posLocal.y + (i+1) * yPos;           //像数坐标
			posTemp.x = posLocal.x + (i+1) * xPos;           //往右下方向(相隔的距离) (nLayer - i)

			if (CanPutObj(posTemp,nModel,nIsCheckNpc))
			{
				pos = posTemp;
				return;
			}
			else
			{
				
			}

			posTemp.y = posLocal.y + (i+1) * yPos;
			posTemp.x = posLocal.x - (i+1) * xPos;            //往左下方向
			if (CanPutObj(posTemp,nModel,nIsCheckNpc))
			{
				pos = posTemp;
				return;
			}
			else
			{
				
			}

			posTemp.y = posLocal.y - (i+1) * yPos;             //往右上方向
			posTemp.x = posLocal.x + (i+1) * xPos;
			if (CanPutObj(posTemp,nModel))
			{
				pos = posTemp;
				return;
			}
			else
			{
				
			}

			posTemp.y = posLocal.y - (i+1) * yPos;
			posTemp.x = posLocal.x - (i+1) * xPos;             //往左上方向
			if (CanPutObj(posTemp,nModel,nIsCheckNpc))
			{
				pos = posTemp;
				return;
			}
			else
			{
				
			}

		}

		nLayer++;

		if (nLayer >= nCheckNum)
			break;
	}

	return;
}


VOID KSubWorld::GetFreeObjPos(POINT& pos)
{
	POINT	posLocal = pos;
	POINT	posTemp;
	INT nLayer = 1;
	
	int nCheckNum=32,nIsCheckNpcPos=0,nModel=0,nIsCheckNpc=0;

	nCheckNum = _gameGlobalSet.SubWorldInfo.nCheckNum;
	nIsCheckNpcPos = _gameGlobalSet.SubWorldInfo.nIsCheckNpcPos;
	nModel = _gameGlobalSet.SubWorldInfo.nModel;
	nIsCheckNpc =_gameGlobalSet.SubWorldInfo.nIsCheckNpc;

	if (nIsCheckNpcPos==1) //如果 NPC所在的地方能掉东西 就 返回
	{//检测NPC点
	    if (CanPutObj(posLocal,nModel,nIsCheckNpc))
		    return;
	}
	
	while(1)
	{
		for (INT i = 0; i <= nLayer; i++)
		{
			posTemp.y = posLocal.y + i * 32;
			posTemp.x = posLocal.x + (nLayer - i) * 32;
			if (CanPutObj(posTemp,nModel,nIsCheckNpc))
			{
				pos = posTemp;
				return;
			}
			posTemp.y = posLocal.y + i * 32;
			posTemp.x = posLocal.x - (nLayer - i) * 32;
			if (CanPutObj(posTemp,nModel,nIsCheckNpc))
			{
				pos = posTemp;
				return;
			}
			posTemp.y = posLocal.y - i * 32;
			posTemp.x = posLocal.x + (nLayer - i) * 32;
			if (CanPutObj(posTemp,nModel,nIsCheckNpc))
			{
				pos = posTemp;
				return;
			}
			posTemp.y = posLocal.y - i * 32;
			posTemp.x = posLocal.x - (nLayer - i) * 32;
			if (CanPutObj(posTemp,nModel,nIsCheckNpc))
			{
				pos = posTemp;
				return;
			}
		}
		nLayer++;
		if (nLayer >= 10)
			break;
	}
	return;
}

//掉下物品距离调节
void KSubWorld::GetFreeNewObjPos(POINT& pos)
{
	POINT	posLocal = pos;
	POINT	posTemp;
	int nLayer = 1;

	//if (CanPutObj(posLocal,nModel)) //如果 NPC所在的地方能掉东西 就 返回
	//	return;

    int xPos=16,yPos=16,nCheckNum=32,nIsCheckNpcPos=0,nModel=0,nIsCheckNpc=0;

	xPos =_gameGlobalSet.SubWorldInfo.xPos;
	yPos = _gameGlobalSet.SubWorldInfo.yPos;
    nCheckNum = _gameGlobalSet.SubWorldInfo.nCheckNum;
	nIsCheckNpcPos = _gameGlobalSet.SubWorldInfo.nIsCheckNpcPos;
	nModel = _gameGlobalSet.SubWorldInfo.nModel;
	nIsCheckNpc =_gameGlobalSet.SubWorldInfo.nIsCheckNpc;

    //   g_GameSetTing.GetInteger2("Item","ItemDropPos",&xPos,&yPos); // 偏移
	   //g_GameSetTing.GetInteger2("Item","ItemDropCheckNum",&nCheckNum,&nIsCheckNpcPos); // 查找的范围
	   //g_GameSetTing.GetInteger2("Item","ItemDropModel",&nModel,&nIsCheckNpc);
       
    if (nIsCheckNpcPos==1) //如果 NPC所在的地方能掉东西 就 返回
	{//检测NPC点
	   if (CanPutObj(posLocal,nModel,nIsCheckNpc))
		   return;
	}

	while(1)
	{
		for (int i = 0; i <= nLayer; i++)
		{
         	posTemp.y = posLocal.y;
			posTemp.x = posLocal.x+(i+1)*xPos;      //正右边

			if (CanPutObj(posTemp,nModel,nIsCheckNpc))
			{
				pos = posTemp;
				return;
			}
     
	        posTemp.y = posLocal.y+(i+1)*yPos;
			posTemp.x = posLocal.x;                  //正下边 
			if (CanPutObj(posTemp,nModel,nIsCheckNpc))
			{
				pos = posTemp;
				return;
			}

			posTemp.y = posLocal.y;
			posTemp.x = posLocal.x-(i+1)*xPos;       //正左边 

			if (CanPutObj(posTemp,nModel,nIsCheckNpc))
			{
				pos = posTemp;
				return;
			}

			posTemp.y = posLocal.y-(i+1)*yPos;
			posTemp.x = posLocal.x;                  //正上边 
			if (CanPutObj(posTemp,nModel,nIsCheckNpc))
			{
				pos = posTemp;
				return;
			}

			posTemp.y = posLocal.y + (i+1) * yPos;           //像数坐标
			posTemp.x = posLocal.x + (i+1) * xPos;           //往右下方向(相隔的距离) (nLayer - i)

			if (CanPutObj(posTemp,nModel,nIsCheckNpc))
			{
				pos = posTemp;
				return;
			}

			posTemp.y = posLocal.y + (i+1) * yPos;
			posTemp.x = posLocal.x - (i+1) * xPos;            //往左下方向
			if (CanPutObj(posTemp,nModel,nIsCheckNpc))
			{
				pos = posTemp;
				return;
			}

			posTemp.y = posLocal.y - (i+1) * yPos;             //往右上方向
			posTemp.x = posLocal.x + (i+1) * xPos;
			if (CanPutObj(posTemp,nModel))
			{
				pos = posTemp;
				return;
			}

			posTemp.y = posLocal.y - (i+1) * yPos;
			posTemp.x = posLocal.x - (i+1) * xPos;             //往左上方向
			if (CanPutObj(posTemp,nModel,nIsCheckNpc))
			{
				pos = posTemp;
				return;
			}
		}

		nLayer++;

		if (nLayer >= nCheckNum)
			break;
	}

	return;
}

BOOL KSubWorld::CanPutObj(POINT pos,int nModle,BOOL nIsCheckNpc)
{
	int nRegion, nMapX, nMapY, nOffX, nOffY,nIsShowInfo=0,nIsShowHave=0;
	    Mps2Map(pos.x,pos.y,&nRegion,&nMapX,&nMapY,&nOffX,&nOffY);
   
#ifdef _SERVER
   //g_GameSetTing.GetInteger2("Item","IsShowInfo",&nIsShowInfo,&nIsShowHave);
   //if (nIsShowInfo)
   //   printf("----[检测地上空位置]原像素:X=%d,Y=%d,区域:%d,格子:X=%d,Y=%d,偏移:XOFF=%d,YOFF=%d----\n",pos.x,pos.y,nRegion,nMapX,nMapY,nOffX,nOffY);
#endif

if (nModle==0)
{
	if (nRegion >= 0 \
		&& !m_Region[nRegion].GetNewBarrier(nMapX,nMapY,nOffX,nOffY,nIsCheckNpc)\
		&& ! m_Region[nRegion].GetRef(nMapX,nMapY,obj_object))  //m_Region[nRegion].HasObj(nMapX,nMapY))//
		{
#ifdef _SERVER
   //if (nIsShowHave)
   //   printf("----[找到地上空位置]原像素:X=%d,Y=%d,区域:%d,格子:X=%d,Y=%d,偏移:XOFF=%d,YOFF=%d----\n",pos.x,pos.y,nRegion,nMapX,nMapY,nOffX,nOffY);
#endif
           	return TRUE;
		}
}
else 
{
	if (nRegion >= 0 \
		&& !m_Region[nRegion].GetBarrierNewMin(nMapX,nMapY,nOffX,nOffY,nIsCheckNpc)\
		&& ! m_Region[nRegion].GetRef(nMapX,nMapY,obj_object))  //m_Region[nRegion].HasObj(nMapX,nMapY))//
		{
#ifdef _SERVER
   //if (nIsShowHave)
     // printf("----[找到地上空位置]原像素:X=%d,Y=%d,区域:%d,格子:X=%d,Y=%d,偏移:XOFF=%d,YOFF=%d----\n",pos.x,pos.y,nRegion,nMapX,nMapY,nOffX,nOffY);
#endif
            return TRUE;
		}
}
	return FALSE;
}

#ifdef _SERVER
/*
void KSubWorld::BroadCast(void* pBuffer, DWORD dwSize, int &nMaxCount, int nOX, int nOY)
{
#define	MAX_SYNC_RANGE	25  //同步的范围
	KIndexNode *pNode = NULL;
	
	pNode = (KIndexNode *)m_PlayerList.GetHead();
	while(pNode && nMaxCount > 0)
	{
		_ASSERT(pNode->m_nIndex > 0 && pNode->m_nIndex < MAX_PLAYER);
		int nPlayerIndex = pNode->m_nIndex;
		int nNpcIndex = Player[nPlayerIndex].m_nIndex;
		int nTX = Npc[nNpcIndex].m_MapX;
		int nTY = Npc[nNpcIndex].m_MapY;
		int nDX = nTX - nOX;
		int nDY = nTY - nOY;
		if (Player[pNode->m_nIndex].m_nNetConnectIdx >= 0 
			&& (nDX * nDX + nDY * nDY) <= MAX_SYNC_RANGE * MAX_SYNC_RANGE
			&& Player[pNode->m_nIndex].m_bSleepMode == FALSE)
			g_pServer->PackDataToClient(Player[pNode->m_nIndex].m_nNetConnectIdx, (BYTE*)pBuffer, dwSize);
		nMaxCount--;
		pNode = (KIndexNode *)pNode->GetNext();
	}
}

*/
void KSubWorld::BroadCasts(const char* pBuffer, size_t uSize)
{
	int nIdx = PlayerSet.GetFirstPlayer();
	while(nIdx)
	{
		if (Player[nIdx].m_nNetConnectIdx >= 0 && Player[nIdx].m_nIndex > 0 && Npc[Player[nIdx].m_nIndex].m_SubWorldIndex == m_nIndex)
		{
			g_pServer->PackDataToClient(Player[nIdx].m_nNetConnectIdx, pBuffer, uSize);
		}
		nIdx = PlayerSet.GetNextPlayer();
	}
}
//复活所有NPC
int	KSubWorld::RevivalAllNpc()
{
	KIndexNode * pNode		= NULL;
	size_t	ulCount = 0;
	for (int i = 0; i < m_nTotalRegion; i++)
	{
		KRegion * pCurRegion = &m_Region[i];
		pNode = (KIndexNode *)pCurRegion->m_NpcList.GetHead();
		
		while(pNode)
		{
			int nNpcIdx = pNode->m_nIndex;
			if (!Npc[nNpcIdx].IsPlayer() && Npc[nNpcIdx].m_Kind!=kind_dialoger)
			{
				if (Npc[nNpcIdx].m_Doing == do_revive)
			           Npc[nNpcIdx].m_Doing = do_none;
				Npc[nNpcIdx].ExecuteRevive(); //复活
			}
			pNode = (KIndexNode *)pNode->GetNext();	
		}
	}
	
	pNode = (KIndexNode*)m_NoneRegionNpcList.GetHead();
	
	while(pNode)
	{
		int nNpcIdx = pNode->m_nIndex;
		if (!Npc[nNpcIdx].IsPlayer() && Npc[nNpcIdx].m_Kind!=kind_dialoger)
		{
			Npc[nNpcIdx].m_Frames.nTotalFrame = 1;
			Npc[nNpcIdx].m_Frames.nCurrentFrame = 0;
			ulCount ++;
		}
		pNode = (KIndexNode *)pNode->GetNext();	
	}
	
	return ulCount;
}

void KSubWorld::RemoveNpc(int nIdx)
{
	if (nIdx <= 0 || nIdx >= MAX_NPC)
		return;
	
	//_ASSERT(Npc[nIdx].m_Node.m_Ref > 0);
	
	//if (Npc[nIdx].m_Node.m_Ref > 0)
	//{
		Npc[nIdx].m_Node.Remove();
		Npc[nIdx].m_Node.Release();
		Npc[nIdx].m_Node.m_Ref=0;
	//}
#ifndef _SERVER
	Npc[nIdx].RemoveRes();
#endif
}
//删除该地图上某类NPC
int	KSubWorld::DelTypeNpcInWro(int nNpcSettings)
{
	int nCount=0;
	for (int i = 0; i < m_nTotalRegion;i++)  
	{	
		nCount +=m_Region[i].DelAllNpc(nNpcSettings);   
	}

	KIndexNode* pNode = (KIndexNode *)m_NoneRegionNpcList.GetHead();
	KIndexNode *pTmpNode = NULL;
	while(pNode)
	{//重生列表的	npc
		pTmpNode = (KIndexNode *)pNode->GetNext();
		int  nNpcIdx =	pNode->m_nIndex;
		if  (nNpcIdx>0 && Npc[nNpcIdx].m_Kind!=kind_player)
		{
			//if (Npc[nNpcIdx].m_RegionIndex>=0)
			{
			    //m_Region[Npc[nNpcIdx].m_RegionIndex].RemoveNpc(nNpcIdx);
				Npc[nNpcIdx].m_Node.Remove();
		        Npc[nNpcIdx].m_Node.Release();
			}
		    NpcSet.Remove(nNpcIdx);
			nCount++;
		}

		pNode=pTmpNode;
	}
	
    return nCount;
}

//玩家强制性移出地图
int	KSubWorld::MovePlayerOutMap(int nFigthstate,int nDesPos,int nIsComp,int nPunish,int nClearScript)
{
	KIndexNode * pNode = NULL;
	int	ulCount = 0;
	int ulCountA= 0; 

	KTabFile nPosFile;
	char nPosDir[128];
	     sprintf(nPosDir,"\\settings\\mapmovepos\\movepos_%d.txt",nDesPos);

	if (!nPosFile.Load(nPosDir))
	{
		printf("-目标地图坐标不存在,移动失败:%s!- \n",nPosDir);
		return 0;
	}
	 //g_MoveMapPos
	int nRoWs=nPosFile.GetHeight();

	for (int i = 0; i < m_nTotalRegion; i++)  //9个区域查找？
	{
		KRegion * pCurRegion = &m_Region[i];
		
		pNode = (KIndexNode *)pCurRegion->m_NpcList.GetHead();
		
		while(pNode)
		{
			int nNpcIdx = pNode->m_nIndex;
			if (nNpcIdx > 0  && Npc[nNpcIdx].m_Kind==kind_player)
			{
				int nCurRows=GetRandomNumber(1,nRoWs);
				int nMapidx,nXpos,nYpos;
				   	nPosFile.GetInteger(nCurRows,1,0,&nMapidx);
					nPosFile.GetInteger(nCurRows,2,0,&nXpos);
					nPosFile.GetInteger(nCurRows,3,0,&nYpos);
					Npc[nNpcIdx].ChangeWorld(nMapidx, nXpos,nYpos);

					Npc[nNpcIdx].SetFightMode(nFigthstate);
					if (nPunish<0 || nPunish>7)
						nPunish=0;  //NPC杀死模式
					Npc[nNpcIdx].SetPunishModel(nPunish);

					if  (nIsComp>0)
					{
						Npc[nNpcIdx].RestoreCurrentCamp();
					}

					if (nClearScript>0)
					{
					    Player[Npc[nNpcIdx].GetPlayerIdx()].m_dwDeathScriptId=0;
						Player[Npc[nNpcIdx].GetPlayerIdx()].m_dwDamageScriptId=0;
					}

				    ulCount ++;
			}
			pNode = (KIndexNode *)pNode->GetNext();	
		}
	}
	
	pNode = (KIndexNode*)m_NoneRegionNpcList.GetHead();
	
	while(pNode)
	{
		int nNpcIdx = pNode->m_nIndex;
		if (nNpcIdx > 0  && Npc[nNpcIdx].m_Kind==kind_player)
		{	
			int nCurRows=GetRandomNumber(1,nRoWs);
			int nMapidx,nXpos,nYpos;
			nPosFile.GetInteger(nCurRows,1,0,&nMapidx);
			nPosFile.GetInteger(nCurRows,2,0,&nXpos);
			nPosFile.GetInteger(nCurRows,3,0,&nYpos);
			Npc[nNpcIdx].ChangeWorld(nMapidx, nXpos,nYpos);
			Npc[nNpcIdx].SetFightMode(nFigthstate);

			if (nPunish<0 || nPunish>7)
				nPunish=0;  //NPC杀死模式
			Npc[nNpcIdx].SetPunishModel(nPunish);

			if  (nIsComp>0)
			{
				Npc[nNpcIdx].RestoreCurrentCamp();
			}

			if (nClearScript>0)
			{
			    Player[Npc[nNpcIdx].GetPlayerIdx()].m_dwDeathScriptId=0;
				Player[Npc[nNpcIdx].GetPlayerIdx()].m_dwDamageScriptId=0;
			}

			ulCountA ++;
		}
		pNode = (KIndexNode *)pNode->GetNext();	
	}
	
	nPosFile.Clear();

	return ulCount+ulCountA;

}

//获取某个地图上的NPC数量
int	KSubWorld::GetNpcCount(int nNpcKind)
{
   	KIndexNode * pNode = NULL;
	int	ulCount = 0;
	int ulCountA= 0; 
	for (int i = 0; i < m_nTotalRegion; i++)  //9个区域查找？
	{
		KRegion * pCurRegion = &m_Region[i];
		
		pNode = (KIndexNode *)pCurRegion->m_NpcList.GetHead();
		
		while(pNode)
		{
			int nNpcIdx = pNode->m_nIndex;
			if (nNpcIdx > 0  && Npc[nNpcIdx].m_Kind==nNpcKind)
			{
				ulCount ++;
			}
			pNode = (KIndexNode *)pNode->GetNext();	
		}
	}

	pNode = (KIndexNode*)m_NoneRegionNpcList.GetHead();

	while(pNode)
	{
		int nNpcIdx = pNode->m_nIndex;
		if (nNpcIdx > 0  && Npc[nNpcIdx].m_Kind==nNpcKind)
		{	
			ulCountA ++;
		}
		pNode = (KIndexNode *)pNode->GetNext();	
	}	
	return ulCount+ulCountA;

}

//删除该地图上NPC
int	KSubWorld::DelAllNpcInWroWithName(char *nName,int nType)
{
	if (nName==NULL)
		return 0;

	int nCount=0;
	for (int i = 0; i < m_nTotalRegion; i++)  
	{	
		nCount +=m_Region[i].DelAllNpcWithName(nName,nType);   
	}
	
	KIndexNode* pNode = (KIndexNode *)m_NoneRegionNpcList.GetHead();
	KIndexNode *pTmpNode = NULL;
	while(pNode)
	{//重生列表的	npc
		pTmpNode = (KIndexNode *)pNode->GetNext();
		int  nNpcIdx =	pNode->m_nIndex;
		if  (nType==0 && nNpcIdx>0 && Npc[nNpcIdx].m_Kind!=kind_player)
		{
			if (strstr(Npc[nNpcIdx].Name,nName))
			{
				Npc[nNpcIdx].m_Node.Remove();
				Npc[nNpcIdx].m_Node.Release();

				NpcSet.Remove(nNpcIdx);
			}

			nCount++;
		}
		else if (nType==1 && nNpcIdx>0 && Npc[nNpcIdx].m_Kind!=kind_player)
		{
			//char nstrName[64]={0};
	        //     Npc[nNpcIdx].GetstrInfo(STR_GUISHU_NAME,nstrName);
			unsigned int nTempDwid=g_FileName2Id(nName);
			if (Npc[nNpcIdx].m_RegionIndex>=0 && Npc[nNpcIdx].m_IsRevive==nTempDwid)
			{
				Npc[nNpcIdx].m_Node.Remove();
				Npc[nNpcIdx].m_Node.Release();
			}
			
			if (Npc[nNpcIdx].m_IsRevive==nTempDwid)
				NpcSet.Remove(nNpcIdx);

			nCount++;
		}
		
		pNode=pTmpNode;
	}
	
    return nCount;
	
}

//删除该地图上NPC
int	KSubWorld::DelAllNpcInWro()
{
	int nCount=0;
	for (int i = 0; i < m_nTotalRegion; i++)  
	{	
		nCount +=m_Region[i].DelAllNpc();   
	}

	KIndexNode* pNode = (KIndexNode *)m_NoneRegionNpcList.GetHead();
	KIndexNode *pTmpNode = NULL;
	while(pNode)
	{//重生列表的	npc
		pTmpNode = (KIndexNode *)pNode->GetNext();
		int  nNpcIdx =	pNode->m_nIndex;
		if  (nNpcIdx>0 && nNpcIdx < MAX_NPC && Npc[nNpcIdx].m_Kind!=kind_player)
		{
		//	if (Npc[nNpcIdx].m_RegionIndex>=0)
			{
			    //m_Region[Npc[nNpcIdx].m_RegionIndex].RemoveNpc(nNpcIdx);
				Npc[nNpcIdx].m_Node.Remove();
		        Npc[nNpcIdx].m_Node.Release();
			}
		    NpcSet.Remove(nNpcIdx);
			nCount++;
		}

		pNode=pTmpNode;
	}

    return nCount;
	
}
//用于隐藏NPC
int KSubWorld::FindNpcFromName(const char * szName)
{
	if (!szName || !szName[0])
		return 0;
	
	KIndexNode * pNode		= NULL;

	int nResult = 0;
	
	for (int i = 0; i < m_nTotalRegion; i++)
	{
		KRegion * pCurRegion = &m_Region[i];
		pNode = (KIndexNode *)pCurRegion->m_NpcList.GetHead();
		
		while(pNode)
		{
			int nNpcIdx = pNode->m_nIndex;
			if (strstr(Npc[nNpcIdx].Name, szName))	//!strcmp
			{
				nResult = nNpcIdx;
				return nResult;
			}
			pNode = (KIndexNode *)pNode->GetNext();	
		}
	}	
	
	pNode = (KIndexNode*)m_NoneRegionNpcList.GetHead();	//重生列表
	
	while(pNode)
	{
		int nNpcIdx = pNode->m_nIndex;
		if (!strcmp(Npc[nNpcIdx].Name, szName))
		{
			return nNpcIdx;
		}
		pNode = (KIndexNode *)pNode->GetNext();	
	}
	
	return nResult;
}
#endif

KRegionSearcher KSubWorld::GetRegionSearcher(KRegionSearcher::enumType eSearchType, INT nRange, INT nSearcherIdx, INT nRelation, INT nMapX, INT nMapY)
{
	if (nMapX < 0 && nMapY < 0)
	{
		if (Npc[nSearcherIdx].m_RegionIndex >= 0)
		{
			nMapX	= m_Region[Npc[nSearcherIdx].m_RegionIndex].m_nRegionX / m_nCellWidth + Npc[nSearcherIdx].m_MapX;
			nMapY	= m_Region[Npc[nSearcherIdx].m_RegionIndex].m_nRegionY / m_nCellHeight + Npc[nSearcherIdx].m_MapY;
		}
		else
		{
			_ASSERT(FALSE);
			nMapX	= 0;
			nMapY	= 0;
		}
	}
	return KRegionSearcher(eSearchType, this, nMapX, nMapY, nRange, nSearcherIdx, nRelation);
}

KRegionSearcher KSubWorld::GetRegionSearcher(KRegionSearcher::enumType eSearchType, INT nRange, INT nSearcherIdx, NPC_RELATION sRelation, INT nMapX, INT nMapY)
{
	if (nMapX < 0 && nMapY < 0)
	{
		if (Npc[nSearcherIdx].m_RegionIndex >= 0)
		{
			nMapX	= m_Region[Npc[nSearcherIdx].m_RegionIndex].m_nRegionX / m_nCellWidth + Npc[nSearcherIdx].m_MapX;
			nMapY	= m_Region[Npc[nSearcherIdx].m_RegionIndex].m_nRegionY / m_nCellHeight + Npc[nSearcherIdx].m_MapY;
		}
		else
		{
			_ASSERT(FALSE);
			nMapX	= 0;
			nMapY	= 0;
		}
	}
	return KRegionSearcher(eSearchType, this, nMapX, nMapY, nRange, nSearcherIdx, sRelation);
}

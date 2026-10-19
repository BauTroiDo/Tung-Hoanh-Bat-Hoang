/* -------------------------------------------------------------------------
//	文件名		：	kpathfinder.h
//	创建者		：	liupeng,luobaohang
//	创建时间	：	2007-10-5
//	功能描述	：	封装自动寻路类
//					
// -----------------------------------------------------------------------*/
#ifndef __KPATHFINDER_H__
#define __KPATHFINDER_H__

#ifndef _SERVER
//////////////////////////////////////////////////////////////////////////
// 地图上的点
#include <vector>

const BYTE	MapPoint_CanWalk =		1;					// 是否可走
const BYTE	MapPoint_CanPass =		1 << 1;				// 是否通行（非障碍点）
const BYTE	MapPoint_InOpenTable =	1 << 2;				// 是否处于open表中
const BYTE	MapPoint_InCloseTable = 1 << 3;				// 是否处于close表中
const BYTE	MapPoint_IsBusPoint =	1 << 4;				// 是否是站点
const BYTE	MapPoint_OnPath		=	1 << 5;				// 是否是到目标点的路径上的点
const BYTE	MapPoint_InSearchCloseTable = 1 << 6;		// 是否是搜索点的close表
const BYTE	MapPoint_Test = 1 << 7;		//

//#define USE_MAX_OBSTACLE

struct FindPathNode
{
	INT		x;
	INT		y;
};

//-------------------------------------------------------------------------------

const INT		notfinished = 0;
const INT		NearObstacle = 10;
const INT		MaxListNum = 0XFFFF;		// 最大的
const INT		RegionWidth = 16;
const INT		RegionHeight = 32;
const INT		notStarted = 0, found = 1, nonexistent = 2;
const INT		unwalkable = 0, walkable = 1;
const INT		cnFactorRadius = 16;			// 现在只用4位存储,所以勿超过16
const INT		cFactorBase = 32;// cnFactorRadius * 2; // 惩罚基数，越大惩罚作用越小

#define Point_Gap			200
#define Edge_MaxLen			400
#define Edge_Search_Max		10000000

#define KD_GEN_PATH_FILE // 生成寻路文件定义此宏
#define KD_MAX_PATH_POINT 32

class KOpenTable;

class KMapPoint
{
public:
#ifdef KD_GEN_PATH_FILE
	KMapPoint(): m_nFlag(0), m_nDir(0), m_nBusPoint(-1), m_nX(-1), m_nY(-1), m_btFactor(cFactorBase), m_nBusDis(-1), m_nBlock(0)
#else
	KMapPoint(): m_nFlag(0), m_nBusPoint(-1), m_nX(-1), m_nY(-1), m_btFactor(255)
#endif
	{}
	~KMapPoint()
	{}
#ifdef KD_GEN_PATH_FILE
	BOOL		CanWalk()				{ return m_nFlag & MapPoint_CanWalk; }
	VOID		SetCanWalk(BOOL b = TRUE)		{
		if (b)
			m_nFlag |= MapPoint_CanWalk;
		else
			m_nFlag &= ~MapPoint_CanWalk;
	}	
#else
	BOOL		CanWalk()				{ return m_btFactor != 255; }
#endif
	BOOL		CanPass()				{ return m_nFlag & MapPoint_CanPass; }
	VOID		SetCanPass(BOOL b = TRUE)
	{
		if (b)
			m_nFlag |= MapPoint_CanPass;
		else
			m_nFlag &= ~MapPoint_CanPass;
	}
	BOOL		InOpen()				{return m_nFlag & MapPoint_InOpenTable;}
	VOID		PutInOpen()				{
		m_nFlag &= ~MapPoint_InCloseTable;
		m_nFlag |= MapPoint_InOpenTable;
	}
	VOID		RemoveInOpen()			{m_nFlag &= ~MapPoint_InOpenTable;}

	BOOL		InClose()				{return m_nFlag & MapPoint_InCloseTable;}
	VOID		PutInClose()			{
		m_nFlag &= ~MapPoint_InOpenTable;
		m_nFlag |= MapPoint_InCloseTable;
	}
	VOID		RemoveInClose()			{m_nFlag &= ~MapPoint_InCloseTable;}

	BOOL		InSearchClose()				{return m_nFlag & MapPoint_InSearchCloseTable;}
	VOID		PutInSearchClose()			{
		m_nFlag &= ~MapPoint_InOpenTable;
		m_nFlag |= MapPoint_InSearchCloseTable;
	}
	VOID		RemoveInSearchClose()			{m_nFlag &= ~MapPoint_InSearchCloseTable;}
	BOOL		InTest() { return m_nFlag & MapPoint_Test; }
	VOID		PutInTest() { m_nFlag |= MapPoint_Test; }
	VOID		SetBusPoint(INT n)		{m_nBusPoint = n;}
	INT			GetBusPoint()			{return m_nBusPoint;}

	VOID		PutInPath()				{m_nFlag |= MapPoint_OnPath;}
	BOOL		OnPath()				{return m_nFlag & MapPoint_OnPath;}

	VOID		SetNext(INT x, INT y)	{m_nX = x, m_nY = y;}
	VOID		GetNext(INT& x, INT& y)	{x = m_nX, y = m_nY;}

	VOID		SetFactor(BYTE f)		{m_btFactor = f;}
	BYTE		GetFactor()				{return m_btFactor;}

	VOID		Reset()
	{
		m_nFlag &= ~(MapPoint_InOpenTable | MapPoint_InCloseTable);
	}

	VOID		ResetSearch()
	{
		m_nFlag &= ~(MapPoint_InOpenTable | MapPoint_InCloseTable | MapPoint_OnPath);
		m_nX = -1;
		m_nY = -1;
	}

#ifdef KD_GEN_PATH_FILE
	VOID		EnableDir(BYTE dir)		{m_nDir |= (1 << dir);}
	BOOL		IsGoodDir(BYTE dir)		{return m_nDir & (1 << dir);}
	VOID		SetBlock(INT n)			{m_nBlock = n;}
	INT			GetBlock()				{return m_nBlock;}
	VOID		SetBusDis(INT nDis)		{m_nBusDis = nDis;}
	INT			GetBusDis()				{return m_nBusDis;}
	VOID		PutInBusPoints()		{m_nFlag |= MapPoint_IsBusPoint;}
	BOOL		IsBusPoint()			{return m_nFlag & MapPoint_IsBusPoint;}
#endif

private:
	INT			m_nBusPoint;//SH					// 最近的站点编号(从1开始)
	BYTE		m_btFactor;	//BY					// 该点的惩罚值
	BYTE		m_nFlag;
	INT			m_nX;								// 路径上的下一个点坐标x
	INT			m_nY;								// 路径上的下一个点坐标y
#ifdef KD_GEN_PATH_FILE
	BYTE		m_nDir;								// 该点的可行方向
	INT			m_nBlock;							// 连通块区域编号(从1开始)
	INT			m_nBusDis;							// 到最近的BusPoint的距离
#endif
	friend class KPathFinder;
};
//////////////////////////////////////////////////////////////////////////
// 顶点表
class KBusPointList
{
public:
	KBusPointList()	{}
	~KBusPointList() {}

	INT AddVector(INT x, INT y)
	{
		m_vecX.push_back(x);
		m_vecY.push_back(y);
		return m_vecY.size();
	}

	INT	GetSize()	{return m_vecX.size();}
	VOID GetPoint(INT i, INT& x, INT& y)
	{
		x = m_vecX[i];
		y = m_vecY[i];
	}

	INT	GetIndex(INT x, INT y)
	{
		for (INT i = 0; i < (INT)m_vecX.size(); i++)
		{
			if (x == m_vecX[i] && y == m_vecY[i])
				return i+1;
		}
		return -1;
	}
	BOOL SaveToBuf(LPBYTE& pBuf)
	{
		*(SHORT*)pBuf = (SHORT)m_vecX.size();
		pBuf += sizeof(SHORT);
		memcpy(pBuf, &m_vecX[0], sizeof(SHORT) * m_vecX.size());
		pBuf += sizeof(SHORT) * m_vecX.size();
		memcpy(pBuf, &m_vecY[0], sizeof(SHORT) * m_vecY.size());
		pBuf += sizeof(SHORT) * m_vecY.size();
		return TRUE;
	}
	BOOL LoadFromBuf(LPBYTE& pBuf)
	{
		INT nSize = *(SHORT*)pBuf;
		pBuf += sizeof(SHORT);
		m_vecX.resize(nSize);
		m_vecY.resize(nSize);
		memcpy(&m_vecX[0], pBuf, sizeof(SHORT) * m_vecX.size());
		pBuf += sizeof(SHORT) * m_vecX.size();
		memcpy(&m_vecY[0], pBuf, sizeof(SHORT) * m_vecY.size());
		pBuf += sizeof(SHORT) * m_vecY.size();
		return TRUE;
	}
private:
	std::vector<SHORT>		m_vecX;
	std::vector<SHORT>		m_vecY;
};

//////////////////////////////////////////////////////////////////////////
// 边的记录, 从开始到结束的点的序列, 包含起始点和结束点
class KEdgePath
{
public:
	KEdgePath() : m_nCurPos(0)	{ }
#ifdef KD_GEN_PATH_FILE
	KEdgePath(INT nCost): m_nCost(nCost)	 {}
#endif
	~KEdgePath()	{}

	KEdgePath(const KEdgePath& path)
	{
#ifdef KD_GEN_PATH_FILE
		m_nCost = path.m_nCost;
#endif
		m_vecX = path.m_vecX;
		m_vecY = path.m_vecY;
	}
	VOID operator=(KEdgePath& path)
	{
#ifdef KD_GEN_PATH_FILE	
		m_nCost = path.m_nCost;
#endif
		m_vecX = path.m_vecX;
		m_vecY = path.m_vecY;
	}

	VOID AddPoint(INT x, INT y)
	{
		m_vecX.push_back(x);
		m_vecY.push_back(y);
	}

	// 将path颠倒过来
	VOID Reserve()
	{
		INT n = m_vecX.size() / 2;
		INT nSize = m_vecX.size();
		for (INT i = 0; i < n; i++)
		{
			INT temp = m_vecX[i];
			m_vecX[i] = m_vecX[nSize - i - 1];
			m_vecX[nSize - i - 1] = temp;
			
			temp = m_vecY[i];
			m_vecY[i] = m_vecY[nSize - i - 1];
			m_vecY[nSize - i - 1] = temp;
		}
	}

	INT		GetSize()	{return m_vecX.size();}

	INT		GetLeft()	{return m_vecX.size() - m_nCurPos;}

	BOOL	GetPoint(INT i, INT& x, INT& y)
	{
		if (i >= (INT)m_vecX.size())
			return FALSE;
		x = m_vecX[i];
		y = m_vecY[i];
		return TRUE;
	}
	BOOL SaveToBuf(LPBYTE& pBuf)
	{
		*(SHORT*)pBuf = (SHORT)m_vecX.size();
		pBuf += sizeof(SHORT);
		memcpy(pBuf, &m_vecX[0], sizeof(SHORT) * m_vecX.size());
		pBuf += sizeof(SHORT) * m_vecX.size();
		memcpy(pBuf, &m_vecY[0], sizeof(SHORT) * m_vecY.size());
		pBuf += sizeof(SHORT) * m_vecY.size();
		return TRUE;
	}
	BOOL LoadFromBuf(LPBYTE& pBuf)
	{
		INT nSize = *(SHORT*)pBuf;
		pBuf += sizeof(SHORT);
		m_vecX.resize(nSize);
		m_vecY.resize(nSize);
		memcpy(&m_vecX[0], pBuf, sizeof(SHORT) * m_vecX.size());
		pBuf += sizeof(SHORT) * m_vecX.size();
		memcpy(&m_vecY[0], pBuf, sizeof(SHORT) * m_vecY.size());
		pBuf += sizeof(SHORT) * m_vecY.size();
		return TRUE;
	}
	VOID Reset()
	{
		m_nCurPos = 0;
		m_vecX.erase(m_vecX.begin(), m_vecX.end());
		m_vecY.erase(m_vecY.begin(), m_vecY.end());
	}
	BOOL GetNextPoint(INT& x, INT& y, INT nStep)
	{
		if (m_nCurPos >= (INT)m_vecX.size())
			return FALSE;
		if (nStep <= 0)
			nStep = 1;
		m_nCurPos += nStep - 1;
		if (m_nCurPos >= (INT)m_vecX.size())
			m_nCurPos = m_vecX.size() - 1;
		x = m_vecX[m_nCurPos];
		y = m_vecY[m_nCurPos];
		m_nCurPos++;
		return TRUE;
	}
	BOOL GetCurPos() { return m_nCurPos; }
	VOID SetCurPos(INT nPos) { m_nCurPos = nPos; }
#ifdef KD_GEN_PATH_FILE
	INT	GetLen()	{return m_nCost;}
#endif
private:
	INT			m_nCurPos;
	std::vector<SHORT>	m_vecX;
	std::vector<SHORT>	m_vecY;
#ifdef KD_GEN_PATH_FILE
	INT			m_nCost;
#endif
};

//////////////////////////////////////////////////////////////////////////
// 边表
class KEdgeList
{
public:
	KEdgeList();
	~KEdgeList();
	INT		GetSize() { return m_nVecs; }
	INT		Init(INT nVects);
	BOOL	AddEdge(INT i ,INT j, KEdgePath* pPath);
	BOOL	HasEdge(INT i, INT j);
	BOOL	GetEdgePath(INT i, INT j, KEdgePath& path);
	INT		GetEdgeCnt()	{return m_Paths.size();}
	KEdgePath*	GetEdge(INT i) {return m_Paths[i];}

	BOOL	CreateMinMap();
	BOOL	GetMinNext(INT i, INT j, KEdgePath& path);
	BOOL	LoadFromBuf(LPBYTE pBuf);
	BOOL	SaveToBuf(LPBYTE& pBuf);
	INT		GetMinBusPoint(INT i, INT j)
	{
		return m_PathMap[i*m_nVecs+j];
	}
private:
	INT		GetDistance(INT i, INT j);
	VOID	Clear();

private:
	INT						m_nVecs;
	SHORT*					m_Graph;   // c，x到y点的最短路径的下一点
	SHORT*					m_PathMap;	//c，x到y点路径
	std::vector<KEdgePath*>		m_Paths;	// c
};
//////////////////////////////////////////////////////////////////////////
class KPathFinder
{
public:
	/***************游戏寻路相关接口************/
	// 快速寻路
	BOOL InitFindPath(INT tx, INT ty, INT ex, INT ey);
	BOOL IsValid() { return m_bValid; }
	// 初始化寻路目标
	BOOL StartSearch(INT tx, INT ty);	// 现在已不用这个
	BOOL Search(INT nCurX, INT nCurY)	// 现在已不用这个
	{
		return Search(nCurX, nCurY, m_FindPath);
	}
	// 获取下一点
	BOOL GetNextPoint(INT& x, INT& y, INT nStep);
	// 获取下一个直达点
	BOOL GetNextStraightPoint(DOUBLE dCurX, DOUBLE dCurY, INT& x, INT& y);
	// 以障碍文件和寻路文件初始化
	BOOL PathFindInit(char * szPathFile);
	INT GetLeftPoint() { return m_FindPath.GetLeft(); }
public:
	/****************寻路文件生成相关接口*******************/
	// 获取点（并赋值）
	KMapPoint** GetMapPoint() { return m_map; }
	// 计算地图的寻路数据, aPoint[]指示可达区域
	BOOL	GenSearchInfo(const POINT aPoint[], INT nPointCount);
	// 保存为寻路文件
	BOOL SaveSearchInfoFile(LPCSTR szSearchInfoFile);

public:
	// 初始化地图区域
	BOOL Init(const RECT& rc);
	BOOL GetMapLeftTop(INT& nLeft, INT& nTop)
	{
		nLeft = m_rc.left * RegionWidth; nTop = m_rc.top * RegionHeight;
		return TRUE;
	}

	BOOL SearchPath(INT nOrgX, INT nOrgY, INT nDesX, INT nDesY)
	{
		return SearchPath(nOrgX, nOrgY, nDesX, nDesY, m_FindPath);
	}
	BOOL		Search(INT x, INT y, KEdgePath& path);
	BOOL		SearchPath(INT nOrgX, INT nOrgY, INT nDesX, INT nDesY, KEdgePath& path);
	BOOL		LoadFromBuf(LPBYTE& pBuf);
	INT		GetBusPointCnt()				{return m_vec.GetSize();}
	VOID	GetBusPoint(INT i, INT& x, INT& y) {m_vec.GetPoint(i, x, y);}
	
	// 根据点找到最近的buspoint
	INT		GetBusPointIndex(INT x, INT y)	{
		if (!m_map[x][y].CanWalk())
			return -1;
		INT nRes = m_map[x][y].GetBusPoint();
		return nRes;
	}
	INT		GetMapWidth()					{return m_w;}
	INT		GetMapHeight()					{return m_h;}
/*	BOOL CalcBusPointsStep();
	BOOL	CalcEdgeStep();*/
	INT		GetEdgeCnt()					{return m_Edges.GetEdgeCnt();}
	KEdgePath*	GetEdge(INT i)				{return m_Edges.GetEdge(i);}	
	KEdgeList*	GetMinMap()					{return &m_Edges;}
	KPathFinder();
	~KPathFinder();
#ifdef KD_GEN_PATH_FILE
	// 将aPoint[]的点所在区域设为可走
	BOOL SetMapBlockByPoint(const POINT aPoint[], INT nPointCount);
	INT		GetTotalBlocks()				{return m_nBlocks;}
	// 设置该区域可走
	BOOL	SetMapBlock(INT nBlock);
	BOOL	SaveToBuf(LPBYTE& pBuf);
protected:
	// 计算点的可行方向
	BOOL CalcVectors();
	// 计算连通区域
	BOOL CalcConnectedBlocks();
	// 计算非障碍区域点的权值
	BOOL CalcFactors();
	// 描点
	BOOL CalcBusPoints();
	// 边算法
	BOOL CalcEdges();
	// 最短路径算法
	BOOL CalcMinPath();
	BYTE	GetMinFactorPoint(INT& i, INT& j);
	VOID	ExpandBusPoint(INT i, INT j, INT factor, KOpenTable& openTable);
	VOID	CalcEdge(INT x, INT y);
	VOID	ExtractPath(INT x1, INT y1, INT x2, INT y2, INT nCost);
	BOOL	SaveMapPoint(LPBYTE& pBuf);
#endif
public:
	BOOL	SearchNextPath(INT x1, INT y1, INT x2, INT y2, KEdgePath& path);
	KMapPoint**	 CopyMap();
	VOID	ExpandTargetPoint(INT x, INT y);
	BOOL	CreateMap(INT w, INT h);
	VOID	ClearMap(KMapPoint**& tmap);
	BOOL	LoadMapPoint(LPBYTE& pBuf);
	// 检查两点间线段是否无障碍，传入的是精确坐标
	BOOL	CheckIfStraightPath(DOUBLE ex, DOUBLE ey, DOUBLE tx, DOUBLE ty);
private:
	// 搜索目标点周围的8格是否可走，可走则修正该点
	BOOL	RepairPos(INT& nX, INT& nY);
private:
	BOOL m_bValid;
	// 地图宽高
	char			m_mapname[MAX_PATH];
	INT				m_w;
	INT				m_h;
	RECT			m_rc;
	// 地图信息扩展(包括点的连通块属性, 最近BusPoint, 到BusPoint的下一个点, 其他的一些Flag属性)
	KMapPoint**		m_map;
	KMapPoint*		m_mapData; //存

	KMapPoint**		m_searchmap;
	KMapPoint*		m_searchmapData;

	// 顶点表
	KBusPointList	m_vec;	//存
	// 边表
	KEdgeList		m_Edges; //存
	// 最短路径表
	INT				m_nBlocks; //存

	INT				m_ex, m_ey;
	INT				m_tx;
	INT				m_ty;
	// 找到的路径
	KEdgePath		m_FindPath;
};
extern KPathFinder g_PathFinder;

#endif // #ifndef _SERVER
#endif // __KPATHFINDER_H__

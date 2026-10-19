/* -------------------------------------------------------------------------
//	文件名		：	jx_path_finder.h
//	创建者		：	liupeng
//	创建时间	：	2007-5-17
//	功能描述	：	封装自动寻路类
//					
// -----------------------------------------------------------------------*/

#ifndef __JX_PATH_FINDER_H__
#define __JX_PATH_FINDER_H__
#ifndef _SERVER
#include <algorithm>
/*
enum Obstacle_Type			// 物件地面格子障碍类型
{
	Obstacle_Empty = 0,		// 空，无障碍
	Obstacle_Full,			// 完整障碍
	Obstacle_LT,			// 左上区域为障碍
	Obstacle_RT,			// 右上区域为障碍
	Obstacle_LB,			// 左下区域为障碍
	Obstacle_RB,			// 右下区域为障碍
	Obstacle_Type_Num,		// 障碍类型数
};

enum Obstacle_Kind
{
	Obstacle_NULL = 0,		// 没有障碍
	Obstacle_Normal,		// 一般障碍
	Obstacle_Fly,			// 障碍但是可以飞过
	Obstacle_Jump,			// 障碍但是可以跳过
	Obstacle_JumpFly,		// 障碍但是可以跳过并且飞过
	Obstacle_Kind_Num,		// 障碍种类数
};
*/
struct FindPathNode
{
	INT		x;
	INT		y;
};

//-------------------------------------------------------------------------------

CONST INT		notfinished = 0;
CONST INT		NearObstacle = 10;
CONST INT		MaxListNum = 0XFFFF;		// 最大的
CONST INT		RegionWidth = 16;
CONST INT		RegionHeight = 32;
CONST INT		notStarted = 0, found = 1, nonexistent = 2;
CONST INT		unwalkable = 0, walkable = 1;

#pragma pack(push, 1)
class OpenNode
{
public:
	WORD		x;						// open 节点 x 位置
	WORD		y;						// open 节点 y 位置
	WORD		fcost;					// open 节点到起始点的评估
	WORD		hcost;					// open 节点到目标点的评估值
};

struct PathNode
{
	BYTE		canwoalk;				// 是否是障碍
	BYTE		pathflag;				// 路径标志
	INT			gconst;					// 到起始点的评估值
	WORD		parent_x;				// 从当前点出发到起始点最短路径中的下一个点 x 位置
	WORD		parent_y;				// 从当前点出发到起始点最短路径中的下一个点 y 位置
	FLOAT		factor;					// 选择当前点时的权值(靠近障碍的点的权值较大,路径通过该点所用花费就比较大)
	WORD		pad_0;					// 4字节对齐边界填充
};
#pragma pack(pop)

class OpenNodePtr
{
public:
	OpenNodePtr(): m_pNode(NULL) {}
	OpenNodePtr(OpenNode* pNode): m_pNode(pNode) {}
	VOID operator=(OpenNode* pNode)
	{
		m_pNode = pNode;
	}
	bool operator<(CONST OpenNodePtr& p)
	{
		return m_pNode->fcost > p.m_pNode->fcost;
	}
	OpenNode*	operator->()
	{
		return m_pNode;
	}
	OpenNode*	GetPtr()
	{
		return m_pNode;
	}
	OpenNode*	detch()
	{
		OpenNode* res = m_pNode;
		m_pNode = NULL;
		return res;
	}
private:
	OpenNode*		m_pNode;
};

class OpenTable
{
public:
	OpenTable(): m_pOpenTable(NULL), m_nSize(0), m_nCurSize(0)
	{
	}
	~OpenTable()
	{
		if (m_pOpenTable)
			delete [] m_pOpenTable;
		m_pOpenTable = NULL;
		m_nSize = 0;
		m_nCurSize = 0;
	}

	bool			empty()			{ return m_nCurSize == 0; }
	INT				GetSize()		{ return m_nCurSize; }
	OpenNode*		GetNode(INT i)  { return m_pOpenTable[i].GetPtr(); }

	VOID			Reset(INT nSize)
	{
		if (m_pOpenTable)
		{
			delete [] m_pOpenTable;
			m_pOpenTable = NULL;
		}
		if (nSize)
		{
			m_nSize = nSize;
			m_pOpenTable = new OpenNodePtr[m_nSize];
		}
		m_nCurSize = 0;
	}

	OpenNode*		PopNode()
	{
		if (empty())
			return NULL;
		std::pop_heap(&m_pOpenTable[0], &m_pOpenTable[m_nCurSize]);
		m_nCurSize--;
		return m_pOpenTable[m_nCurSize].detch();
	}
	VOID			PushNode(OpenNode* pNode)
	{
		ASSERT(m_nCurSize < m_nSize);
		if (!m_nCurSize)
		{
			m_pOpenTable[0] = pNode;
			m_nCurSize++;
			return;
		}
		else
		{
			m_pOpenTable[m_nCurSize] = pNode;
			m_nCurSize++;
			std::push_heap(&m_pOpenTable[0], &m_pOpenTable[m_nCurSize]);
		}
	}
	VOID			MakeHeap()
	{
		if (empty()) return;
		std::make_heap(&m_pOpenTable[0], &m_pOpenTable[m_nCurSize]);
	}

private:
	// 所有的open节点,按照fcos大小排列,使用堆算法排序
	OpenNodePtr*	m_pOpenTable;
	INT				m_nSize;
	INT				m_nCurSize;
};

//------------------------------------------------------------------------
CONST WORD			cs_CloseFlagStart = 10;
CONST WORD			cs_OpenFlagStart = cs_CloseFlagStart - 1;

class KJXPathFinder
{
public:

	KJXPathFinder(BOOL bFactor = TRUE);
	~KJXPathFinder();

	enum KE_NEXTSTEP_RESULT
	{
		emKNEXTSTEP_RESULT_SUCCESS,//表示成功找到下一个点
		emKNEXTSTEP_RESULT_NOANYWAY,//表示无法找到下一个点
		emKNEXTSTEP_RESULT_ARRIVAL,//表示已经到达目的地
	};

	// 每到一个新的地图，都会重新初始化，加载该地图的obstacle.bin障碍信息文件
	BOOL				Init(LPCSTR szFileName);

	// 计算到目标点的轨迹(返回值：TRUE表示可以走到, FALSE表示不可以走到)
	BOOL				FindPath(FindPathNode& start, FindPathNode& target, BOOL bFactor = TRUE);

	//根据当前位置now，确定下一步走到哪里并返回值存入 nextstep中去
		//注意：now位置是由服务器定时同步过来的玩家当前位置。
		//由于服务器算法和客户端的偏差，或者玩家可能中途移动位置，所以now的位置不一定会在上次寻路时设定好的轨迹当中
		//需要根据now决定下一步移动的位置。并且有可能会出现，now位置到最终目标点很难再走到的情况，这时需要处理好
	KE_NEXTSTEP_RESULT	GetNextStep(CONST FindPathNode& now, FindPathNode & nextstep, INT steplen);

	// 获取地图的宽度和高度
	INT					GetMapWidth()			{ return m_nMapWidth; }
	INT					GetMapHeight()			{ return m_nMapHeight; }

	INT					CanWalk(INT x, INT y)	{ return m_map[x][y].canwoalk == walkable; }
	FLOAT				GetFactor(INT x, INT y)	{ return m_map[x][y].factor; }

	INT					GetPath(FindPathNode& now, FindPathNode*& pPath);
	BOOL				IsReady()
	{
		return m_bReady;
	};
	RECT*	GetLeftTop()
	{
		return &m_LeftTop;
	}

private:
	KE_NEXTSTEP_RESULT	GetNextStep(CONST FindPathNode& now, FindPathNode & nextstep);
	BOOL				CreateMap(INT w, INT h);
	VOID				ClearMap();
	VOID				ClearOpenTable();
	BOOL				LoadMap(LPBYTE pBuf, INT nSize);
	VOID				ResetMap();
	VOID				AddOpenNode(INT x, INT y, WORD fcost, WORD hcost, INT px, INT py);
	VOID				FixOpenTable(CONST FindPathNode& now);
	VOID				AddCloseNode(OpenNode* pNode, INT tx, INT ty);
	VOID				CalcFactors();
private:
	// 目的地坐标
	FindPathNode		m_target;				
	// 地图大小
	INT					m_nMapWidth;
	INT					m_nMapHeight;

	// A* 算法的辅助数据结构

	// 当前到目标路径的close点标志
	WORD				m_sCloseFlag;
	// 当前到目标路径的open点标志
	WORD				m_sOpenFlag;
	// 当前节点的open表
	OpenTable			m_OpenTable;
	// 地图信息
	PathNode**			m_map;
	// 是否使用权值进行寻路
	BOOL				m_bHasFactor;

	BOOL				m_bReady;
	RECT				m_LeftTop;
};

extern KJXPathFinder g_JXPathFinder;
#endif
#endif // __JX_PATH_FINDER_H__

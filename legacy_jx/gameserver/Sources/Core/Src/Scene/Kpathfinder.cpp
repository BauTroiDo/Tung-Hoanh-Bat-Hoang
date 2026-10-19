/* -------------------------------------------------------------------------
//	文件名		：	kpathfinder.cpp
//	创建者		：	liupeng,luobaohang
//	创建时间	：	2007-10-5
//	功能描述	：	封装自动寻路类
//					
// -----------------------------------------------------------------------*/
#include "KCore.h"
#ifndef _SERVER
#include <stack>
#include <math.h>
#include <fstream>
#include <algorithm>
#include "ksimplebuffer.h"
#include "Kpathfinder.h"
#include "KPlayer.h"
#define SAFE_DELETE(x)	try{ if( (x) != NULL ) { delete (x); (x) = NULL; } } catch(...) {}

KPathFinder g_PathFinder;


struct WPOINT
{
	WORD x;
	WORD y;
	WPOINT(WORD x0, WORD y0) : x(x0), y(y0) {}
};

inline BYTE CalcFactor(INT dx, INT dy)
{
	// 采用简单计算的方式
	_ASSERT(abs(dx) < cnFactorRadius && abs(dy) < cnFactorRadius);
	_ASSERT(abs(dx) + abs(dy) > 0);
	INT dis = (INT)(sqrt((DOUBLE)dx * dx + dy * dy) + 0.5);
	if (cnFactorRadius - dis <= 0)
		return cFactorBase;
	BYTE res =  (BYTE)(cnFactorRadius - dis + cFactorBase);
	return res;
}

class OpenNode
{
public:
	WORD		x;						// open 节点 x 位置
	WORD		y;						// open 节点 y 位置
	INT			ncost;					// open 节点到起始点的评估
	bool operator < (const OpenNode& node)const { return ncost > node.ncost; }
};

class KOpenTable
{
public:
	BOOL			empty()			{ return m_vecNode.empty(); }
	INT				GetSize()		{ return m_vecNode.size(); }

	const OpenNode& TopNode()
	{
		return *m_vecNode.begin();
	}

	BOOL PopNode(OpenNode& node)
	{
		if (m_vecNode.empty())
			return FALSE;
		std::pop_heap(m_vecNode.begin(), m_vecNode.end());
		node = m_vecNode.back();
		m_vecNode.pop_back();
		return TRUE;
	}

	BOOL FixCost(WORD x, WORD y, INT nCost)
	{
		std::vector<OpenNode>::iterator it = m_vecNode.begin();
		for (; it != m_vecNode.end(); ++it)
		{
			if (it->x == x && it->y == y && it->ncost > nCost)
			{
				it->ncost = nCost;
				std::make_heap(m_vecNode.begin(), m_vecNode.end());
				return TRUE;
			}
		}
		return FALSE;
	}

	VOID PushNode(const OpenNode& node)
	{
		m_vecNode.push_back(node);
		std::push_heap(m_vecNode.begin(), m_vecNode.end());
	}
	// 所有的open节点,按照fcos大小排列,使用堆算法排序
	std::vector<OpenNode> m_vecNode;
};

// 测试使用的辅助数据结构
static KOpenTable		m_openTable;
static INT				m_nFactor = 255;
static INT				m_nCurVec = -1;

static POINT Dir2Vec[8] = 
{
	-1, -1,
	0, -1,
	1, -1,
	-1, 0, 
	1, 0,
	-1, 1, 
	0, 1,
	1, 1
};

static INT	Vec2Dir[3][3] = 
{
	{0, 1, 2},
	{3, -1, 4},
	{5, 6, 7}
};

inline VOID Dir_Vec(INT dir, INT x, INT y, INT& x1, INT& y1)
{
	_ASSERT(dir >= 0 && dir < 8);
	x1 = x + Dir2Vec[dir].x;
	y1 = y + Dir2Vec[dir].y;
}

inline INT Vec_Dir(INT dx, INT dy)
{
	dx += 1;
	dy += 1;
	_ASSERT(dx >= 0 && dx < 3 && dy >= 0 && dy < 3);
	return Vec2Dir[dx][dy];
}

//////////////////////////////////////////////////////////////////////////
// 地图初始化类
KPathFinder::KPathFinder(): m_map(NULL), m_mapData(NULL), m_searchmap(NULL), m_searchmapData(NULL), 
						m_w(0), m_h(0), m_nBlocks(0), m_tx(-1), m_ty(-1), m_ex(0), m_ey(0)
{
	m_bValid = FALSE;
}

KPathFinder::~KPathFinder()
{
	if (m_map)
		delete[] m_map;
	if (m_mapData)
		delete[] m_mapData;
	if (m_searchmap)
		delete[] m_searchmap;
	if (m_searchmapData)
		delete[] m_searchmapData;
}

BOOL KPathFinder::Init( const RECT& rc ) 
{
	m_rc = rc;
	INT w = (rc.right - rc.left + 1) * RegionWidth;	  //这个地图的 共有  多少个区域 的宽度
	INT h = (rc.bottom - rc.top + 1) * RegionHeight;  //这个地图的 共有  多少个区域 的高度
	return CreateMap(w, h);	                          //地图的大小
}

BOOL KPathFinder::PathFindInit( char * szPathFile ) 
{
	if (!szPathFile)
		return FALSE;

	m_bValid = FALSE;
#ifdef KD_MAP_FIND_TEST
	std::ifstream fin(szPathFile, std::ios::binary);

	fin.seekg(0, std::ifstream::end);
	INT nSize = fin.tellg();
	if (nSize <= 0)
		return FALSE;
	BYTE* const pBuf = new BYTE[nSize];
	BYTE* pLoadBuf = pBuf;
	fin.seekg(0, std::ifstream::beg);
	fin.read((CHAR*)pBuf, nSize);

	if (fin.bad())
	{
		delete[] pBuf;
		return FALSE;
	}

	BOOL bRet = LoadFromBuf(pLoadBuf);
	delete[] pBuf;
	if (bRet)
		m_bValid = TRUE;
	return bRet;
#else
	KFile file;
	if (!file.Open(szPathFile))
		return FALSE;

	INT nSize = file.Size();

	if (nSize == -1)
		return FALSE;

	KSimpleBuffer cBuffer;
	LPBYTE pBuf = cBuffer.GetBuffer(nSize);	   //分配一个内存
	if (!file.Read((CHAR*)pBuf, nSize))
		return FALSE;
	if (!LoadFromBuf(pBuf))
		return FALSE;

	m_bValid = TRUE;
	return TRUE;
#endif
}

BOOL KPathFinder::RepairPos(INT& nX, INT& nY)
{
	if (nX < 0 || nY < 0 || nX >= m_w || nY >= m_h)
		return FALSE;

	if (!m_map[nX][nY].CanWalk())
	{
		// 执行半格障碍修正，搜索障碍周围的8格是否可走
		int ex1 = nX - 1, ey1 = nY - 1;
		int ex2 = nX + 1, ey2 = nY + 1;
		if (ex1 < 0)
			ex1 = 0;
		if (ey1 < 0)
			ey1 = 0;
		if (ex2 >= m_w)
			ex2 = m_w - 1;
		if (ey2 >= m_h)
			ey2 = m_h - 1;
		for (nX = ex1; nX <= ex2; nX++)
		{
			for (nY = ey1; nY <= ey2; nY++)
			{
				if (m_map[nX][nY].CanWalk())
					return TRUE;
			}
		}
		return FALSE;
	}
	return TRUE;
}

BOOL KPathFinder::InitFindPath( INT tx, INT ty, INT ex, INT ey ) 
{
	if (!m_bValid)
		return FALSE;
	// 对目标点和起点执行半格修正，防止为半格障碍时不能寻路
	if (!RepairPos(tx, ty))
		return FALSE;
	if (!RepairPos(ex, ey))
		return FALSE;
	m_FindPath.Reset();
	m_ex = ex, m_ey = ey;
	m_tx = tx, m_ty = ty;
	if (!SearchNextPath(m_ex, m_ey, m_tx, m_ty, m_FindPath))
		return FALSE;
	while (m_FindPath.GetSize() < KD_MAX_PATH_POINT)
	{
		INT nex, ney;
		if (!m_FindPath.GetPoint(m_FindPath.GetSize() - 1, nex, ney))
			break;
		if (!SearchNextPath(nex, ney, m_tx, m_ty, m_FindPath))
			break;
	}
	return m_FindPath.GetSize() != 0;
}

BOOL KPathFinder::GetNextPoint( INT& x, INT& y, INT nStep ) 
{
	if (nStep <= 0)
	{
		_ASSERT(FALSE);
		return FALSE;
	}
	return m_FindPath.GetNextPoint(x, y, nStep);
}

BOOL KPathFinder::GetNextStraightPoint( DOUBLE dCurX, DOUBLE dCurY, INT& x, INT& y ) 
{
	if (!m_bValid)
		return FALSE;
	const static INT anCheckPos[] = { 32, 16, 8, 4, 2 };
	if (m_FindPath.GetLeft() < anCheckPos[sizeof(anCheckPos) / sizeof(anCheckPos[0]) -1 ])
	{
		if (m_FindPath.GetLeft() > 4)
		{
			// 检查最后一点是否能直达
			INT nDesX, nDesY;
			if (m_FindPath.GetPoint(m_FindPath.GetSize() - 1, nDesX, nDesY))
			{
				if (CheckIfStraightPath(dCurX, dCurY, nDesX, nDesY))
				{
					m_FindPath.SetCurPos(m_FindPath.GetSize());
					x = nDesX;
					y = nDesY;
					return TRUE;
				}
			}
		}
	}
	else
	{
		// 检查这几个跨度能否直接到达
		INT nCurPos = m_FindPath.GetCurPos();
		for (INT i = 0; i < sizeof(anCheckPos) / sizeof(anCheckPos[0]); i++)
		{
			INT nDesX, nDesY;
			if (!m_FindPath.GetPoint(nCurPos + anCheckPos[i] - 1, nDesX, nDesY))
				continue;
			if (CheckIfStraightPath(dCurX, dCurY, nDesX, nDesY))
			{
				m_FindPath.SetCurPos(nCurPos + anCheckPos[i]);
				x = nDesX;
				y = nDesY;
				return TRUE;
			}
		}
	}
	return m_FindPath.GetNextPoint(x, y, 1);
}

// 初始化搜索到tx, ty的地图
BOOL KPathFinder::StartSearch(INT tx, INT ty)
{
	if (!m_bValid)
		return FALSE;
	INT i, j;
	if (tx < 0 || ty < 0 || tx >= m_w || ty >= m_h)
		return FALSE;
	if (!m_map[tx][ty].CanWalk())
		return FALSE;
	// 去掉所有的标志
	INT nTBusPoint = m_map[tx][ty].GetBusPoint() - 1;
	if (nTBusPoint < 0)
		return FALSE;
	for (i = 0; i < m_w; i++)
	{
		for (j = 0; j < m_h; j++)
		{
			m_map[i][j].ResetSearch();
		}
	}
	
	// 使用路径初始化地图
	INT nCnt = m_Edges.GetSize();
	for (i = 0; i < nCnt; i++)
	{
		if (i != nTBusPoint)
		{
			KEdgePath path;
			if (m_Edges.GetMinNext(i, nTBusPoint, path))
			{
				INT x, y;
				path.GetPoint(0, x, y);
				m_map[x][y].PutInPath();
				for (j = 1; j < path.GetSize(); j++)
				{
					INT x1, y1;
					path.GetPoint(j, x1, y1);
					m_map[x][y].PutInPath();
					m_map[x][y].SetNext(x1, y1);
					x = x1;
					y = y1;
				}
			}
		}
	}
	// 搜索目标点周围的区域
	ExpandTargetPoint(tx, ty);

	m_tx = tx;
	m_ty = ty;
	return TRUE;
}

BOOL KPathFinder::SearchPath( INT nOrgX, INT nOrgY, INT nDesX, INT nDesY, KEdgePath& sPath ) 
{
	if (nDesX < 0 || nDesY < 0 || nDesX >= m_w || nDesY >= m_h)
		return FALSE;
	if (!m_map[nDesX][nDesY].CanWalk())
		return FALSE;
	INT nDesBusPoint = m_map[nDesX][nDesY].GetBusPoint() - 1;
	INT nOrgBusPoint = m_map[nOrgX][nOrgY].GetBusPoint() - 1;
	// 去掉所有的标志
	for (INT i = 0; i < m_w; i++)
	{
		for (INT j = 0; j < m_h; j++)
		{
			m_map[i][j].ResetSearch();
		}
	}
	// 使用路径初始化地图
	INT nCnt = m_Edges.GetSize();
	KEdgePath path;
	while (nOrgBusPoint != nDesBusPoint)
	{
		if (m_Edges.GetMinNext(nOrgBusPoint, nDesBusPoint, path))
		{
			INT x, y;
			path.GetPoint(0, x, y);
			m_map[x][y].PutInPath();
			for (INT j = 1; j < path.GetSize(); j++)
			{
				INT x1, y1;
				path.GetPoint(j, x1, y1);
				m_map[x][y].PutInPath();
				m_map[x][y].SetNext(x1, y1);
				x = x1;
				y = y1;
			}
			nOrgBusPoint = m_map[x][y].GetBusPoint() - 1;
		}
	}
	// 搜索目标点周围的区域
	ExpandTargetPoint(nDesX, nDesY);
	m_tx = nDesX;
	m_ty = nDesY;
	return Search(nOrgX, nOrgY, sPath);
}

// 搜索原点
BOOL KPathFinder::Search(INT ex, INT ey, KEdgePath& path)
{
	if (ex < 0 || ey < 0 || ex >= m_w || ey >= m_h)
		return FALSE;
	if (!m_bValid)
		return FALSE;
	if (!m_map[ex][ey].CanWalk())
	{
		// 执行半格障碍修正，搜索障碍周围的8格是否可走
		int ex1 = ex - 1, ey1 = ey - 1;
		int ex2 = ex + 1, ey2 = ey + 1;
		if (ex1 < 0)
			ex1 = 0;
		if (ey1 < 0)
			ey1 = 0;
		if (ex2 >= m_w)
			ex2 = m_w - 1;
		if (ey2 >= m_h)
			ey2 = m_h - 1;
		for (ex = ex1; ex <= ex2; ex++)
			for (ey = ey1; ey <= ey2; ey++)
			{
				if (m_map[ex][ey].CanWalk())
					goto _Search_Go_On;
			}
		_ASSERT(FALSE);
		return FALSE;
	}
_Search_Go_On:
	m_FindPath.Reset();
	_ASSERT(ex >= 0 && ey >= 0);
	// 拷贝一份地图
	KMapPoint**	smap = NULL;
	OpenNode cNode;
	INT factor = 0;
	BOOL bMeet = FALSE;
	KOpenTable sPointOpenTable;
	//INT bspx, bspy;
	INT nBusPoint;
	INT nSCount = 0;
	DWORD time1 = ::GetTickCount(), time2 = 0, time3 = 0, time4 = 0, time5 = 0;
	smap = CopyMap();
	if (m_map[ex][ey].OnPath())
		goto FIDN_PATH;

	time2 = ::GetTickCount();
	cNode.x = ex;
	cNode.y = ey;
	cNode.ncost = 0;
	sPointOpenTable.PushNode(cNode);

	nBusPoint = smap[ex][ey].GetBusPoint();
	//m_vec.GetPoint(nBusPoint - 1, bspx, bspy);
	factor = cnFactorRadius;//smap[bspx][bspy].GetFactor();

	while (!bMeet && sPointOpenTable.PopNode(cNode))
	{
		++nSCount;
		INT curX = cNode.x;
		INT curY = cNode.y;
		// Point_Gap是搜索范围的经验值，太小可能有些离buspoint较远的点搜不到，太大影响效率
		if (cNode.ncost > (cnFactorRadius + cFactorBase) * Point_Gap)
		{
			sPointOpenTable.PushNode(cNode);
			break;
		}
		else
		{
			smap[curX][curY].PutInSearchClose();
			{
			INT x = curX, y = curY;
			//表示起始点的搜索过的范围和终点的搜索范围重叠了, 
			//一旦发生这种重叠, 表示已经可用找到一条路径了, 不需要再浪费时间去搜索了
			if (m_map[x][y].InClose())		// 处理中间相会
			{
				if (ex != x || ey != y)
				{	
					INT px, py;				// 翻转路径
					INT nx, ny;
					m_map[x][y].GetNext(nx, ny);
					smap[x][y].GetNext(px, py);

					INT bx = nx, by = ny;
					nx = x, ny = y;
					while (x != m_tx || y != m_ty)
					{
						_ASSERT(abs(bx - x) <= 1 && abs(by - y) <= 1);
						_ASSERT(m_map[bx][by].OnPath());
						smap[x][y].SetNext(bx, by);	
						x = bx, y = by;
						m_map[x][y].GetNext(bx, by);
					}

					x = px, y = py;
					smap[x][y].GetNext(px, py);
					while (true)
					{
						_ASSERT(abs(nx - x) <= 1 && abs(ny - y) <= 1);
						smap[x][y].SetNext(nx, ny);
						smap[x][y].PutInPath();
						if (x == ex && y == ey)
							break;
						nx = x, ny = y;
						x = px, y = py;
						_ASSERT(x != -1 && y != -1);
						smap[x][y].GetNext(px, py);
					}
				}
				bMeet = TRUE;
				break;
			}
			}
			for (INT y = curY - 1; y <= curY + 1; y++)
			for (INT x = curX - 1; x <= curX + 1; x++)
			{
				BOOL corner = TRUE;
				INT addCost = cNode.ncost;

				if (x < 0 || x >= m_w || y < 0 || y >= m_h)
					continue;
				if (!smap[x][y].CanWalk() || smap[x][y].InSearchClose())
					continue;

				if (abs(x-curX) == 1 && abs(y-curY) == 1)
					addCost += 14 * (smap[curX][curY].GetFactor() + smap[x][y].GetFactor()) / 2;
				else
					addCost += 10 * (smap[curX][curY].GetFactor() + smap[x][y].GetFactor()) / 2;	

				if (!smap[x][y].InOpen())
				{
					OpenNode cNodeNew;
					cNodeNew.x = x;
					cNodeNew.y = y;
					cNodeNew.ncost = addCost;
					smap[x][y].PutInOpen();
					sPointOpenTable.PushNode(cNodeNew);
					smap[x][y].SetNext(curX, curY);
				}
				else
				{
					if (sPointOpenTable.FixCost(x, y, addCost))
						smap[x][y].SetNext(curX, curY);;
				}
			}
		}
	}
	time3 = ::GetTickCount();
	if (!bMeet)
	{
		INT nCnt = 0;
		while (sPointOpenTable.PopNode(cNode))
		{
			INT x = cNode.x;
			INT y = cNode.y;
			if (m_map[x][y].OnPath())		// 找到一条路径, 对它进行反向
			{
				INT px, py;						// 翻转路径
				INT nx, ny;
				m_map[x][y].GetNext(nx, ny);
				_ASSERT(nx != -1 && ny != -1);
				if (!smap[nx][ny].InSearchClose())	// 向外的路径
				{
					smap[x][y].GetNext(px, py);

					INT bx = nx, by = ny;
					nx = x, ny = y;
					while (x != m_tx || y != m_ty)
					{
						_ASSERT(abs(bx - x) <= 1 && abs(by - y) <= 1);
						_ASSERT(m_map[bx][by].OnPath());
						if (smap[bx][by].InSearchClose())
						{
							goto AGAIN;				// 碰到了回绕的路径
						}
						smap[x][y].PutInSearchClose();
						smap[x][y].SetNext(bx, by);	

						x = bx, y = by;
						m_map[x][y].GetNext(bx, by);
					}

					x = px, y = py;
					smap[x][y].GetNext(px, py);
					while (true)
					{
						_ASSERT(abs(nx - x) <= 1 && abs(ny - y) <= 1);
						smap[x][y].SetNext(nx, ny);
						smap[x][y].PutInPath();
						if (x == ex && y == ey)
							break;
						nx = x, ny = y;
						x = px, y = py;
						_ASSERT(x != -1 && y != -1);
						smap[x][y].GetNext(px, py);
					}
					break;
				}
			}
AGAIN:	;
		}
	}

	time4 = ::GetTickCount();
FIDN_PATH:
	// 提取路径
	if (!smap[ex][ey].OnPath())
	{
		_ASSERT(FALSE);
		return FALSE;
	}
	path.AddPoint(ex, ey);
	while (ex != m_tx || ey != m_ty)
	{
		INT nx, ny;
		if (smap[ex][ey].InTest())
		{
			_ASSERT(FALSE);
			return FALSE;
		}
		smap[ex][ey].GetNext(nx, ny);
		smap[ex][ey].PutInTest();
		if (nx < 0 || ny < 0)
			return FALSE;
		ex = nx;
		ey = ny;
		path.AddPoint(ex, ey);
	}

	time5 = ::GetTickCount();
	return TRUE;
}

BOOL	KPathFinder::CreateMap(INT w, INT h)
{
	if (m_map)
	{
		delete[] m_map;
		m_map = NULL;
	}
	if (m_mapData)
	{
		delete[] m_mapData;
		m_mapData = NULL;
	}
	if (m_searchmap)
	{
		delete[] m_searchmap;
		m_searchmap = NULL;
	}
	if (m_searchmapData)
	{
		delete [] m_searchmapData;
		m_searchmapData = NULL;
	}

	_ASSERT((sizeof(KMapPoint) % 4) == 0);

	m_map = new KMapPoint*[w];          //新建一个 宽度 的内存 
	if (!m_map)
		return FALSE;	

	m_mapData = new KMapPoint[w * h];	//新建一个 总面积的 内存
	if (!m_mapData)
	{
		delete m_map;
		m_map = NULL;
		return FALSE;
	}

	KMapPoint* pStart = m_mapData;

	for (INT i = 0; i < w; i++)
	{
		m_map[i] = pStart; //横跨一个地图高度的线
		pStart += h;
	}
	_ASSERT(pStart == m_mapData + w * h);

	m_w = w;       //整个地图的 宽度 
	m_h = h;	   //整个地图的 高度
	return TRUE;
}

BOOL KPathFinder::LoadMapPoint( LPBYTE& pBuf ) 
{	
	{for (INT i = 0; i < m_w*m_h; i++)
	{//历遍每个点
		INT nBusPoint = *(SHORT*)pBuf;
		pBuf += sizeof(SHORT);
		if (nBusPoint < 0)
		{
			i += - nBusPoint - 1;
			continue;
		}
		m_mapData[i].m_nBusPoint = (SHORT)(nBusPoint >> 4);///*16
		m_mapData[i].m_btFactor = cFactorBase + (nBusPoint & 0xf); // 32 +  (nBusPoint & 0xf)
	}}
	return TRUE;
}

#ifdef KD_GEN_PATH_FILE

BOOL KPathFinder::SaveMapPoint( LPBYTE& pBuf ) 
{
	*(RECT*)pBuf = m_rc;
	pBuf += sizeof(RECT);
	INT nSkip = 0;
	{for (INT i = 0; i < m_w*m_h; i++)
	{
		INT nDir = m_mapData[i].m_nDir;
		if (!m_mapData[i].CanWalk())
		{
			nSkip--;
			if (nSkip <= -32767)
			{
				*(SHORT*)pBuf = (SHORT)nSkip;
				pBuf += sizeof(SHORT);
				nSkip = 0;
			}
			continue;
		}
		else if (m_mapData[i].m_nBusPoint <= 0)
		{
			_ASSERT(FALSE);
			nSkip--;
			if (nSkip <= -32767)
			{
				*(SHORT*)pBuf = (SHORT)nSkip;
				pBuf += sizeof(SHORT);
				nSkip = 0;
			}
			continue;
		}
		else if (nSkip < 0)
		{
			*(SHORT*)pBuf = (SHORT)nSkip;
			pBuf += sizeof(SHORT);
			nSkip = 0;
		}
		SHORT sStore = (SHORT)(m_mapData[i].m_nBusPoint << 4);
		INT nFactor = m_mapData[i].m_btFactor - cFactorBase;
		if (nFactor >= 16)
		{
			_ASSERT(FALSE);
			nFactor = 15;
		}
		sStore |= (nFactor & 0xf);
		*(SHORT*)pBuf = sStore;
		pBuf += sizeof(SHORT);
	}}
	if (nSkip < 0)
	{
		*(SHORT*)pBuf = (SHORT)nSkip;
		pBuf += sizeof(SHORT);
	}
	return TRUE;
}

BOOL KPathFinder::SetMapBlockByPoint(const POINT aPoint[], INT nPointCount)
{
	if (nPointCount <= 0)
	{
		//AfxMessageBox("Lack of map signpost!");
		return FALSE;
	}
	for (int i = 0; i < nPointCount; i++)
	{
		int nX = aPoint[i].x - m_rc.left * RegionWidth;
		int nY = aPoint[i].y - m_rc.top * RegionHeight;
		if (nX < 0 || nX >= m_w || nY < 0 || nY >= m_h || m_map[nX][nY].GetBlock() <= 0)
		{
			_ASSERT(FALSE);
			//AfxMessageBox("Illegal map signpost!");
			return FALSE;			
		}
		SetMapBlock(m_map[nX][nY].GetBlock());
	}
	return TRUE;
}

BOOL KPathFinder::SetMapBlock(INT nBlock)
{
	for (INT i = 0 ; i < m_w; i++)
		for (INT j = 0; j < m_h; j++)
			if (m_map[i][j].CanPass() && m_map[i][j].GetBlock() == nBlock)
				m_map[i][j].SetCanWalk();
	return TRUE;
}

BOOL	KPathFinder::GenSearchInfo(const POINT aPoint[], INT nPointCount)
{
	if (!CalcVectors())
		return FALSE;

	if (!CalcConnectedBlocks())
		return FALSE;

	if (!SetMapBlockByPoint(aPoint, nPointCount))
		return FALSE;	

	if (!CalcFactors())
		return FALSE;

	if (!CalcBusPoints())
		return FALSE;


	if (!CalcEdges())
		return FALSE;

	if (!CalcMinPath())
		return FALSE;

	return TRUE;
}

BOOL KPathFinder::CalcVectors()
{
	// 处理方向，左、右下三个边的边缘格先不处理
	{for (INT i = 1; i < m_w - 1; i++)
	{
		for (INT j = 0; j < m_h - 1; j++)
		{
			if (!m_map[i][j].CanPass())
				continue;
			// 按次序判断右，右下，下，左下4个方向
			if (m_map[i+1][j].CanPass())
			{
				m_map[i][j].EnableDir(4);	// 右
				m_map[i+1][j].EnableDir(3); // 左
			}
			if (m_map[i+1][j+1].CanPass())
			{
				m_map[i][j].EnableDir(7); // 右下
				m_map[i+1][j+1].EnableDir(0); // 左上
			}
			if (m_map[i][j+1].CanPass())
			{
				m_map[i][j].EnableDir(6); // 下
				m_map[i][j+1].EnableDir(1); // 上
			}
			if (m_map[i-1][j+1].CanPass())
			{
				m_map[i][j].EnableDir(5); // 左下
				m_map[i-1][j+1].EnableDir(2); // 右上
			}
		}
	}}
	// 处理左、右下三个边边缘格
	// 左竖边，只判断右，右下和下方向
	{for (INT j = 0; j < m_h - 1; j++)
	{
		if (!m_map[0][j].CanPass())
			continue;
		if (m_map[1][j].CanPass())
		{
			m_map[0][j].EnableDir(4);	// 右
			m_map[1][j].EnableDir(3); // 左
		}
		if (m_map[1][j+1].CanPass())
		{
			m_map[0][j].EnableDir(7); // 右下
			m_map[1][j+1].EnableDir(0); // 左上
		}
		if (m_map[0][j+1].CanPass())
		{
			m_map[0][j].EnableDir(6); // 下
			m_map[0][j+1].EnableDir(1); // 上
		}
	}}
	INT rEdge = m_w - 1;
	// 右竖边，只判断下和下左方向
	{for (INT j = 0; j < m_h - 1; j++)
	{
		if (!m_map[rEdge][j].CanPass())
			continue;
		if (m_map[rEdge][j+1].CanPass())
		{
			m_map[rEdge][j].EnableDir(6); // 下
			m_map[rEdge][j+1].EnableDir(1); // 上
		}
		if (m_map[rEdge-1][j].CanPass())
		{
			m_map[rEdge][j].EnableDir(5); // 左下
			m_map[rEdge-1][j+1].EnableDir(2); // 右上
		}
	}}
	INT dEdge = m_h - 1;
	// 下横边，只判断右方向，右下角的点不判断
	{for (INT i = 0; i < m_w - 1; i++)
	{
		if (!m_map[i][dEdge].CanPass())
			continue;
		if (m_map[i+1][dEdge].CanPass())
		{
			m_map[i][dEdge].EnableDir(4);	// 右
			m_map[i+1][dEdge].EnableDir(3); // 左
		}
	}}
	return TRUE;
}

// 计算连通块区域
BOOL KPathFinder::CalcConnectedBlocks()
{
	INT nBlock = 1;
	INT i, j;
	while (true)
	{
		// 从全部的点中选择没有设置连通块的点
		INT curX = -1, curY = -1;
		for (i = 0; i < m_w; i++)
			for (j = 0; j < m_h; j++)
				if (m_map[i][j].CanPass() && m_map[i][j].GetBlock() == 0)
				{
					curX = i, curY = j;
					goto find;
				}
find:
				if (curX == -1 && curY == -1)
					break;
				// 扩展当前的点
				std::stack<INT>	StackX;
				std::stack<INT>	StackY;
				StackX.push(curX);
				StackY.push(curY);
				while (!StackX.empty())
				{
					curX = StackX.top(), StackX.pop();
					curY = StackY.top(), StackY.pop();
					m_map[curX][curY].SetBlock(nBlock);

					for (INT i = 0; i < 8; i++)
					{
						INT x, y;
						if (!m_map[curX][curY].IsGoodDir(i))
							continue;
						Dir_Vec(i, curX, curY, x, y);

						//ASSERT(m_map[x][y].GetBlock() == 0 || m_map[x][y].GetBlock() == nBlock);
						if (m_map[x][y].GetBlock() == 0 || m_map[x][y].GetBlock() == nBlock)
						{
							StackX.push(x);
							StackY.push(y);
						}
					}
				}

				nBlock++;
	}
	m_nBlocks = nBlock;
	return TRUE;
}

#endif

#define KD_NOFACTOR_DIS_SQUARE 200	// 距离平方多近时不计算惩罚值

BOOL KPathFinder::SearchNextPath( INT x1, INT y1, INT x2, INT y2, KEdgePath& path ) 
{
	if (x1 < 0 || y1 < 0 || x1 >= m_w || y1 >= m_h || 
		x2 < 0 || y2 < 0 || x2 >= m_w || y2 >= m_h)
		return FALSE;
	if (x1 == x2 && y1 == y2)
		return FALSE;
	INT nBus1 = m_map[x1][y1].GetBusPoint();
	INT nBus2 = m_map[x2][y2].GetBusPoint();
	if (nBus1 <= 0 || nBus2 <= 0)
		return FALSE;
	nBus2 = m_Edges.GetMinBusPoint(nBus1 - 1, nBus2 - 1) + 1;
	if (nBus2 <= 0)
		return FALSE;
	BOOL bCalcFactor = (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2) > KD_NOFACTOR_DIS_SQUARE;
	KOpenTable tgPointOpenTable;
	std::vector<KMapPoint* > vecClosePoint;
	std::vector<WPOINT> vecPoint;
	OpenNode cNode;
	cNode.x = x1;
	cNode.y = y1;
	cNode.ncost = 0;
	tgPointOpenTable.PushNode(cNode);
	INT nFactor = m_map[x1][y1].GetFactor();
	BOOL bRet = FALSE;
	INT nCnt = 0;
	INT nBus2X, nBus2y;
	m_vec.GetPoint(nBus2 - 1, nBus2X, nBus2y);
	// 属于同一个BusPoint, 至目标点
	if (nBus1 == nBus2)
	{
		nBus2X = x2, nBus2y = y2;
	}
	DOUBLE a, b, dis;
	INT nDx12 = x1 - nBus2X, nDy12 = y1 - nBus2y;
	INT nSDis12 = nDx12 * nDx12 + nDy12 * nDy12;
	if (y1 == nBus2y)
		a = 0, b = 100, dis = 100;
	else if (x1 == nBus2X)
		a = 100, b = 0, dis = 100;
	else
	{
		a = 100, b = (DOUBLE)(nDx12) * 100 / (nDy12);
		dis = sqrt(a * a + b * b);
	}
	while (tgPointOpenTable.PopNode(cNode))
	{
		nCnt++;
		/*if (cNode.ncost > nFactor * Point_Gap * 2)			// 使用比点范围大
		{
			TRACE1("Cnt Not Find: %d\n", nCnt);
			return FALSE;
		}*/
		INT curX = cNode.x;
		INT curY = cNode.y;
		vecClosePoint.push_back(&m_map[curX][curY]);
		m_map[curX][curY].PutInClose();
		if (nBus1 == nBus2) // 同一个BusPoint则判断是否到目标点
		{
			if (curX == x2 && curY == y2)
			{
				int nX, nY;
				while (curX != x1 || curY != y1)
				{
					vecPoint.push_back(WPOINT(curX, curY));
					m_map[curX][curY].GetNext(nX, nY);
					curX = nX, curY = nY;
				}
				bRet = TRUE;
				break;
			}
		}
		else if (m_map[curX][curY].GetBusPoint() == nBus2)
		{
			int nX, nY;
			while (curX != x1 || curY != y1)
			{
				vecPoint.push_back(WPOINT(curX, curY));
				m_map[curX][curY].GetNext(nX, nY);
				curX = nX, curY = nY;
			}
			bRet = TRUE;
			break;
		}
		for (INT y = curY - 1; y <= curY + 1; y++)
			for (INT x = curX - 1; x <= curX + 1; x++)
			{
				if (x < 0 || x >= m_w || y < 0 || y >= m_h)
					continue;
				if (!m_map[x][y].CanWalk() || m_map[x][y].InClose())
					continue;
				INT addCost = m_map[x][y].GetFactor() - cFactorBase - (cnFactorRadius - 5);
				// 把惩罚值范围缩小到5
				if (addCost < 0)
					addCost = 0;
				// x, y到线段(x1, y1), (nBus2X, nBus2Y)的距离
				INT nDis;
				/*INT nDx01 = x - x1, nDy01 = y - y1;
				INT nDx02 = x - nBus2X, nDy02 = y - nBus2y;	
				INT nSDis01 = nDx01 * nDx01 + nDy01 * nDy01;
				INT nSDis02 = nDx02 * nDx02 + nDy02 * nDy02;
				if (nSDis01 + nSDis12 < nSDis02)
					nDis = 100 * sqrt(nSDis01);
				else if (nSDis02 + nSDis12 < nSDis01)
					nDis = 100 * sqrt(nSDis02);
				else*/ //(只求到直线距离)
					nDis = (INT)(100 * fabs(a * (nBus2X - x) - (nBus2y - y) * b) / dis);
				// 计算与到目标点距离的加成
				nDis += (INT)(sqrt((DOUBLE)(x - nBus2X) * (x - nBus2X) + (nBus2y - y) * (nBus2y - y)) * 100);
				if (bCalcFactor)
					addCost = addCost * 200 + nDis;
				else
					addCost = nDis;

				if (!m_map[x][y].InOpen())
				{
					OpenNode cNodeNew;
					cNodeNew.x = x;
					cNodeNew.y = y;
					cNodeNew.ncost = addCost;
					m_map[x][y].PutInOpen();
					tgPointOpenTable.PushNode(cNodeNew);
					m_map[x][y].SetNext(curX, curY);
				}
				else
				{
					if (tgPointOpenTable.FixCost(x, y, addCost))
						m_map[x][y].SetNext(curX, curY);
				}
			}
	}
	std::vector<WPOINT>::reverse_iterator itPoint = vecPoint.rbegin();
	for (; itPoint != vecPoint.rend(); ++itPoint)
	{
		path.AddPoint(itPoint->x, itPoint->y);
	}

	std::vector<OpenNode>::iterator it = tgPointOpenTable.m_vecNode.begin();
	for (; it != tgPointOpenTable.m_vecNode.end(); ++it)
	{
		m_map[it->x][it->y].Reset();
	}
	std::vector<KMapPoint* >::iterator it2 = vecClosePoint.begin();
	for (; it2 != vecClosePoint.end(); ++it2)
	{
		(*it2)->Reset();
	}
	return bRet;
}

BOOL KPathFinder::LoadFromBuf( LPBYTE& pBuf ) 
{
	RECT rc = *(RECT*)pBuf;
	pBuf += sizeof(RECT); //+上这个偏移

	if (!Init(rc))	 //初始化分配一块 地图大小的内存
		return FALSE;

	BOOL bRet = LoadMapPoint(pBuf);
	bRet &= m_vec.LoadFromBuf(pBuf);  //增加
	bRet &= m_Edges.LoadFromBuf(pBuf);
	return bRet;
}


VOID KPathFinder::ExpandTargetPoint(INT ex, INT ey)
{
	if (ex < 0 || ey < 0 || ex >= m_w || ey >= m_h)
		return;

	KOpenTable tgPointOpenTable;
	OpenNode cNode;
	cNode.x = ex;
	cNode.y = ey;
	cNode.ncost = 0;
	tgPointOpenTable.PushNode(cNode);
	INT nFactor = m_map[ex][ey].GetFactor();

	while (tgPointOpenTable.PopNode(cNode))
	{
		INT curX = cNode.x;
		INT curY = cNode.y;
		if (cNode.ncost > nFactor * Point_Gap)			// 使用比点范围大
		{
			tgPointOpenTable.PushNode(cNode);
			break;
		}
		else
		{
			m_map[curX][curY].PutInClose();
			m_map[curX][curY].PutInPath();

			for (INT y = curY - 1; y <= curY + 1; y++)
				for (INT x = curX - 1; x <= curX + 1; x++)
				{
					if (x < 0 || x >= m_w || y < 0 || y >= m_h)
						continue;
					if (!m_map[x][y].CanWalk() || m_map[x][y].InClose())
						continue;
					INT addCost = cNode.ncost;
					if (abs(x-curX) == 1 && abs(y-curY) == 1)
						addCost += 14 * (m_map[curX][curY].GetFactor() + m_map[x][y].GetFactor()) / 2;
					else
						addCost += 10 * (m_map[curX][curY].GetFactor() + m_map[x][y].GetFactor()) / 2;	

					if (!m_map[x][y].InOpen())
					{
						OpenNode cNodeNew;
						cNodeNew.x = x;
						cNodeNew.y = y;
						cNodeNew.ncost = addCost;
						m_map[x][y].PutInOpen();
						tgPointOpenTable.PushNode(cNodeNew);
						m_map[x][y].SetNext(curX, curY);
					}
					else
					{
						if (tgPointOpenTable.FixCost(x, y, addCost))
							m_map[x][y].SetNext(curX, curY);
					}
				}
		}
	}

	INT nCnt = 0;
	std::vector<OpenNode>::iterator it = tgPointOpenTable.m_vecNode.begin();
	for (; it != tgPointOpenTable.m_vecNode.end(); ++it)
	{
		m_map[it->x][it->y].RemoveInOpen();
		if (m_map[it->x][it->y].OnPath())
		{
			INT nx, ny;
			m_map[it->x][it->y].GetNext(nx, ny);
			_ASSERT(nx != -1 && ny != -1);
			if (m_map[nx][ny].InClose())
				nCnt++;
		}
	}
	//ASSERT(nCnt > 0);
}


KMapPoint**	 KPathFinder::CopyMap()
{
	if (!m_searchmap)
	{
		_ASSERT(!m_searchmapData);
		m_searchmap = new KMapPoint*[m_w];
		if (!m_searchmap)
			return NULL;
		memset(m_searchmap, 0, sizeof(KMapPoint*) * m_w);

		m_searchmapData = new KMapPoint[m_w * m_h];
		KMapPoint* pStart = m_searchmapData;
		for (INT i = 0; i < m_w; i++)
		{
			m_searchmap[i] = pStart;
			pStart += m_h;
		}
		_ASSERT(pStart == m_searchmapData + m_w * m_h);
	}
	memcpy(m_searchmapData, m_mapData, sizeof(KMapPoint) * m_w * m_h);
	return m_searchmap;
}

// 判断两点间是否直达。
// 基本算法是以变化大的方向为主增量，每次主增量变化一格，去判断每点的坐标是否障碍。如果改成
// 分别以两个方向作为主增量判断（而不是只选择其中变化大的一个），就是完全判断
BOOL KPathFinder::CheckIfStraightPath( DOUBLE ex, DOUBLE ey, DOUBLE tx, DOUBLE ty ) 
{
	if (ex < 0 || ey < 0 || ex >= m_w || ey >= m_h || 
		tx < 0 || ty < 0 || tx >= m_w || ty >= m_h)
		return FALSE;
	if (floor(ex) == floor(tx) && floor(ey) == floor(ty))
		return FALSE;  // 当作是FALSE因为寻路不能原地不动
	DOUBLE dx = tx - ex, dy = ty - ey;
	if (dx == 0 && dy == 0)
		return FALSE;
	// 现已改成完全判断
	//if (fabs(dx) > fabs(dy)) // 以x方向为主增量判断
	{
		// 主增量变化一格时次增量的变化
		DOUBLE dyMove = dy / dx;
		// 每移动一次时x的坐标
		DOUBLE exNext = dx > 0 ? ceil(ex) : floor(ex);
		// 每移动一次时y的坐标
		DOUBLE eyNext = ey + (exNext - ex) * dyMove;
		if (exNext < 0 || eyNext < 0 || exNext >= m_w || eyNext >= m_h)
			return FALSE;
		if (!m_map[(INT)exNext][(INT)eyNext].CanWalk())
			return FALSE;
		// 以下x每次移动一格，y移动相应大小
		for (INT i = 0; i < INT(fabs(dx)); i++)
		{
			if (dx > 0)
			{
				exNext += 1;
				eyNext += dyMove;
			}
			else
			{
				exNext -= 1;
				eyNext -= dyMove;
			}
			if (exNext < 0 || eyNext < 0 || exNext >= m_w || eyNext >= m_h)
				return FALSE;
			if (!m_map[(INT)exNext][(INT)eyNext].CanWalk())
				return FALSE;
		}
	}
	//else // fabs(dx) <= fabs(dy)且不为0 // 以y方向为主增量判断
	{
		DOUBLE dxMove = dx / dy;
		// 每移动一次时y的坐标
		DOUBLE eyNext = dy > 0 ? ceil(ey) : floor(ey);
		// 每移动一次时x的坐标
		DOUBLE exNext = ex + (eyNext - ey) * dxMove;
		if (exNext < 0 || eyNext < 0 || exNext >= m_w || eyNext >= m_h)
			return FALSE;
		if (!m_map[(INT)exNext][(INT)eyNext].CanWalk())
			return FALSE;
		// 以下y每次移动一格，x移动相应大小
		for (INT i = 0; i < INT(fabs(dy)); i++)
		{
			if (dy > 0)
			{
				eyNext += 1;
				exNext += dxMove;
			}
			else
			{
				eyNext -= 1;
				exNext -= dxMove;
			}
			if (exNext < 0 || eyNext < 0 || exNext >= m_w || eyNext >= m_h)
				return FALSE;
			if (!m_map[(INT)exNext][(INT)eyNext].CanWalk())
				return FALSE;
		}
	}
	return TRUE;
}

#ifdef KD_GEN_PATH_FILE
// 使用查表提高速度
BYTE	FastCalcFactor(INT dx, INT dy)
{
	static BYTE s_FactorTable[cnFactorRadius][cnFactorRadius] = {0};
	static BOOL s_bInit = FALSE;

	_ASSERT(abs(dx) < cnFactorRadius && abs(dy) < cnFactorRadius);
	_ASSERT(abs(dx) + abs(dy) > 0);

	if (!s_bInit)
	{
		for (INT i = 0; i < cnFactorRadius; i++)
			for (INT j = 0; j < cnFactorRadius; j++)
			{
				if ( i != 0 || j != 0)
				{
					s_FactorTable[i][j] = CalcFactor(i, j);
				}
			}
			s_bInit = TRUE;
	}
	return s_FactorTable[abs(dx)][abs(dy)];
}

BOOL KPathFinder::CalcFactors()
{
	// 先计算边界的影响
	{ for (INT a = 0; a < cnFactorRadius-1; a++)
	{
		BYTE btFactor = FastCalcFactor(a+1, 0);
		// 左边和右边
		INT i = 0;
		for (i = 0; i < m_h; i++)
		{
			if (m_map[a][i].CanWalk() && btFactor > m_map[a][i].GetFactor())
				m_map[a][i].SetFactor(btFactor);
			if (m_map[m_w-a-1][i].CanWalk() && btFactor > m_map[m_w-a-1][i].GetFactor())
				m_map[m_w-a-1][i].SetFactor(btFactor);
		}
		// 上边和下边
		for (i = 0; i < m_w; i++)
		{
			if (m_map[i][a].CanWalk() && btFactor > m_map[i][a].GetFactor())
				m_map[i][a].SetFactor(btFactor);
			if (m_map[i][m_h-a-1].CanWalk() && btFactor > m_map[i][m_h-a-1].GetFactor())
				m_map[i][m_h-a-1].SetFactor(btFactor);
		}
	}}

	// 每个障碍点计算它的覆盖范围
	for (INT i = 0; i < m_w; i++)
	{
		for (INT j = 0; j < m_h; j++)
		{
			if (!m_map[i][j].CanWalk())
			{
				// 裁剪掉上下左右都有障碍点的障碍点
				BOOL bCanCalc = FALSE;
				if (i - 1 >= 0 && m_map[i-1][j].CanWalk())		// 左
					bCanCalc = TRUE;
				else if (i + 1 < m_w && m_map[i+1][j].CanWalk()) // 右
					bCanCalc = TRUE;
				else if (j - 1 >= 0 && m_map[i][j-1].CanWalk())		// 上
					bCanCalc = TRUE;
				else if (j + 1 < m_h && m_map[i][j+1].CanWalk()) // 下
					bCanCalc = TRUE;
				if (bCanCalc)
				{
					// 在一个正方形边界中寻找作用点
					INT x = i - cnFactorRadius + 1;
					INT y = j - cnFactorRadius + 1;

					for (INT a = x; a < x + cnFactorRadius*2-1; a++)
						for (INT b = y; b < y + cnFactorRadius*2-1; b++)
							if (a >= 0 && a < m_w && b >= 0 && b < m_h)
							{
								if (m_map[a][b].CanWalk())
								{
									BYTE btFactor = FastCalcFactor(a-i, b - j);
									if (btFactor > m_map[a][b].GetFactor())
										m_map[a][b].SetFactor(btFactor);
								}
							}
				}
			}
		}
	}
	return TRUE;
}


// 计算标记点
BOOL KPathFinder::CalcBusPoints()
{
	// 从opentable中和当前的地图可以进行扩展的点中选择factor最小的点进行扩展
	KOpenTable openTable;

	INT factor = 255;
	INT nMinX = 0, nMinY = 0;
	// 从地图中搜索factor最小的点

	do 
	{
		factor = GetMinFactorPoint(nMinX, nMinY);
		if (factor != 255)
			ExpandBusPoint(nMinX, nMinY, factor, openTable);
		while (!openTable.empty() && openTable.TopNode().ncost <= factor)
		{
			OpenNode cNode;
			openTable.PopNode(cNode);
			if (!m_map[cNode.x][cNode.y].InClose())
				ExpandBusPoint(cNode.x, cNode.y, factor, openTable);
		}
	}
	while(factor != 255);
	return TRUE;
}

BYTE KPathFinder::GetMinFactorPoint(INT& nMinX, INT& nMinY)
{
	INT i, j;
	BYTE btFactor = 255;
	for (i = 0; i < m_w; i++)
		for (j = 0; j < m_h; j++)
		{
			if (m_map[i][j].CanWalk() && !m_map[i][j].InOpen() && !m_map[i][j].InClose())
			{
				if (m_map[i][j].GetFactor() < btFactor)
				{
					btFactor = m_map[i][j].GetFactor();
					nMinX = i;
					nMinY = j;
				}
			}
		}
		return btFactor;
}

VOID KPathFinder::ExpandBusPoint(INT ex, INT ey, INT factor, KOpenTable& openTable)
{
	if (ex < 0 || ey < 0 || ex >= m_w || ey >= m_h)
		return;

	INT nBusPoint = m_vec.AddVector(ex, ey);

	KOpenTable busPointOpenTable;
	OpenNode cNode;
	cNode.x = ex;
	cNode.y = ey;
	cNode.ncost = 0;
	m_map[ex][ey].SetBusPoint(nBusPoint);
	m_map[ex][ey].PutInBusPoints();
	busPointOpenTable.PushNode(cNode);

	while (busPointOpenTable.PopNode(cNode))
	{
		if (cNode.ncost > factor * Point_Gap)
		{
			busPointOpenTable.PushNode(cNode);
			break;
		}
		else
		{
			INT curX = cNode.x;
			INT curY = cNode.y;
			m_map[curX][curY].PutInClose();

			for (INT i = 0; i < 8; i++)
			{
				INT x, y;
				if (!m_map[curX][curY].IsGoodDir(i))
					continue;
				Dir_Vec(i, curX, curY, x, y);

				if (m_map[x][y].InClose() && (m_map[x][y].GetBusPoint() == nBusPoint))
					continue;

				INT addCost = cNode.ncost;
				if (abs(x-curX) == 1 && abs(y-curY) == 1)
					addCost += 14 * (m_map[curX][curY].GetFactor() + m_map[x][y].GetFactor()) / 2;
				else
					addCost += 10 * (m_map[curX][curY].GetFactor() + m_map[x][y].GetFactor()) / 2;

				if (m_map[x][y].GetBusPoint() != nBusPoint)
				{
					OpenNode cNodeNew;
					cNodeNew.x = x;
					cNodeNew.y = y;
					cNodeNew.ncost = addCost;
					m_map[x][y].SetBusPoint(nBusPoint);
					if (!m_map[x][y].InOpen() && !m_map[x][y].InClose())
						m_map[x][y].PutInOpen();
					busPointOpenTable.PushNode(cNodeNew);
				}
				else
				{
					// 对已经存在的点进行修正
					busPointOpenTable.FixCost(x, y, addCost);
				}
			}
		}
	}
	std::vector<OpenNode>::iterator it = busPointOpenTable.m_vecNode.begin();
	// 将未被扩展的点添加到下一次扩展的区域中
	for (; it != busPointOpenTable.m_vecNode.end(); ++it)
	{
		BOOL bFind = FALSE;
		std::vector<OpenNode>::iterator it2 = openTable.m_vecNode.begin();
		for (; it2 != openTable.m_vecNode.end(); ++it2)
		{
			if (it2->x == it->x && it2->y == it->y)
			{
				bFind = TRUE;
				break;
			}
		}
		if (!bFind && m_map[it->x][it->y].InOpen())
		{
			OpenNode cOpenNode;
			cOpenNode.x = it->x;
			cOpenNode.y = it->y;
			cOpenNode.ncost = m_map[it->x][it->y].GetFactor();
			openTable.PushNode(cOpenNode);
		}
	}
}

BOOL KPathFinder::CalcEdges()
{
	// 初始化地图
	INT i, j;
	for (i = 0; i < m_w; i++)
	{
		for (j = 0; j < m_h; j++)
		{
			m_map[i][j].Reset();
			if (!m_map[i][j].IsBusPoint())
			{
				m_map[i][j].SetBusPoint(0);
				m_map[i][j].SetBusDis(10000000);
			}
			else
			{
				INT nBusPoint = m_vec.GetIndex(i, j);
				_ASSERT(nBusPoint != -1);
				m_map[i][j].SetBusPoint(nBusPoint);
				m_map[i][j].SetBusDis(0);
			}
		}
	}

	m_Edges.Init(m_vec.GetSize());

	for (INT k = 0; k < m_vec.GetSize(); k++)
	{
		INT x,y;
		m_vec.GetPoint(k, x, y);
		CalcEdge(x, y);

		for (i = 0; i < m_w; i++)
		{
			for (j = 0; j < m_h; j++)
			{
				m_map[i][j].Reset();
				m_map[i][j].SetNext(-1, -1);
			}
		}
	}
	return TRUE;
}

// 计算点x, y所的邻接边
VOID KPathFinder::CalcEdge(INT ex, INT ey)
{
	if (ex < 0 || ey < 0 || ex >= m_w || ey >= m_h)
		return;

	// 去掉标志
	KOpenTable busPointOpenTable;
	OpenNode cNode;
	cNode.x = ex;
	cNode.y = ey;
	cNode.ncost = 0;
	busPointOpenTable.PushNode(cNode);
	m_map[ex][ey].SetBusDis(0);
	INT nFactor = m_map[ex][ey].GetFactor();

	while (busPointOpenTable.PopNode(cNode))
	{
		INT curX = cNode.x;
		INT curY = cNode.y;
		if (cNode.ncost > nFactor * Edge_MaxLen)			// 使用比点范围大
		{
			busPointOpenTable.PushNode(cNode);
			break;
		}
		else
		{
			m_map[curX][curY].PutInClose();
			// 提取Path
			if (m_map[curX][curY].IsBusPoint())
			{
				if (curX != ex || curY != ey)
				{
					if (m_map[curX][curY].GetBusPoint() != m_map[ex][ey].GetBusPoint())
						ExtractPath(curX, curY, ex, ey, cNode.ncost);
				}
			}

			for (INT y = curY - 1; y <= curY + 1; y++)
			{
				for (INT x = curX - 1; x <= curX + 1; x++)
				{
					if (x < 0 || x >= m_w || y < 0 || y >= m_h)
						continue;
					if (!m_map[x][y].CanWalk() || m_map[x][y].InClose())
						continue;

					INT addCost = cNode.ncost;
					if (abs(x-curX) == 1 && abs(y-curY) == 1)
						addCost += 14 * (m_map[curX][curY].GetFactor() + m_map[x][y].GetFactor()) / 2;
					else
						addCost += 10 * (m_map[curX][curY].GetFactor() + m_map[x][y].GetFactor()) / 2;	

					if (m_map[x][y].GetBusDis() > addCost)
					{
						m_map[x][y].SetBusPoint(m_map[ex][ey].GetBusPoint());
						m_map[x][y].SetBusDis(addCost);
					}

					if (!m_map[x][y].InOpen())
					{
						OpenNode cNodeNew;
						cNodeNew.x = x;
						cNodeNew.y = y;
						cNodeNew.ncost = addCost;
						m_map[x][y].PutInOpen();
						busPointOpenTable.PushNode(cNodeNew);
						m_map[x][y].SetNext(curX, curY);
					}
					else
					{
						// 对已经存在的点进行修正
						if (busPointOpenTable.FixCost(x, y, addCost))
							m_map[x][y].SetNext(curX, curY);
					}
				}
			}
		}
	}
}

VOID KPathFinder::ExtractPath(INT x1, INT y1, INT x2, INT y2, INT nCost)
{
	if (x1 < 0 || x2 < 0 || y1 < 0 || y2 < 0 || x1 >= m_w || x2 >= m_w || y1 >= m_h || y2 >= m_h)
		return;

	_ASSERT(m_map[x1][y1].IsBusPoint() && m_map[x2][y2].IsBusPoint());
	INT p1 = m_map[x1][y1].GetBusPoint();
	INT p2 = m_map[x2][y2].GetBusPoint();
	_ASSERT(p1 == m_vec.GetIndex(x1, y1));
	_ASSERT(p2 == m_vec.GetIndex(x2, y2));

	if (!m_Edges.HasEdge(p1-1, p2-1))
	{
		KEdgePath* pPath = new KEdgePath(nCost);
		pPath->AddPoint(x1, y1);
		INT nCross = 0;
		while (true)
		{
			INT tx1, ty1;
			m_map[x1][y1].GetNext(tx1, ty1);
			_ASSERT(abs(tx1 - x1) <= 1 && abs(ty1-y1) <= 1);	
			x1 = tx1, y1 = ty1;
			if (m_map[x1][y1].IsBusPoint() && (x1 != x2 || y1 != y2))		// 已经属于道上的
			{
				delete pPath;
				return;
			}
			_ASSERT(x1 != -1 && y1 != -1);
			pPath->AddPoint(x1, y1);
			if (x1 == -1 || y1 == -1)
			{
				delete pPath;
				return;
			}
			if (x1 == x2 && y1 == y2)
				break;
		}
		if (m_Edges.AddEdge(p1-1, p2-1, pPath))
		{
			for (INT i = 0; i < pPath->GetSize(); i++)
			{
				INT x, y;
				pPath->GetPoint(i, x, y);
				m_map[x][y].PutInPath();
			}
		}
	}
}

BOOL KPathFinder::CalcMinPath()
{
	if (m_Edges.CreateMinMap())
		return TRUE;
	return FALSE;
}

BOOL KPathFinder::SaveToBuf( LPBYTE& pBuf ) 
{
	BOOL bRet = SaveMapPoint(pBuf);
	bRet &= m_vec.SaveToBuf(pBuf);
	bRet &= m_Edges.SaveToBuf(pBuf);
	return bRet;
}
//保存寻路的信息
BOOL KPathFinder::SaveSearchInfoFile( LPCSTR szSearchInfoFile ) 
{
	BYTE* const pBuf = new BYTE[10*1024*1024];
	BYTE* pSaveBuf = pBuf;
	if (!SaveToBuf(pSaveBuf))
		return FALSE;
	std::ofstream fout(szSearchInfoFile, std::ios::binary);
	UINT uSize = pSaveBuf - pBuf;
	fout.write((CHAR*)pBuf, uSize);
	delete[] pBuf;
	return TRUE;
}
#endif

//////////////////////////////////////////////////////////////////////////
// 边表
KEdgeList::KEdgeList(): m_nVecs(0), m_Graph(NULL), m_PathMap(NULL)
{
}
KEdgeList::~KEdgeList()
{
	Clear();
}

VOID	KEdgeList::Clear()
{
	if (m_Graph)
	{
		delete[] m_Graph;
	}
	if (m_PathMap)
	{
		delete[] m_PathMap;
	}

	for (INT i = 0; i < (INT)m_Paths.size(); i++)
		delete m_Paths[i];
}

BOOL KEdgeList::GetMinNext(INT i, INT j, KEdgePath& path)
{
	_ASSERT(i >= 0 && i < m_nVecs && j >= 0 && j < m_nVecs);
	INT k = m_PathMap[i*m_nVecs+j];
	if (k == -1)
		return FALSE;
	return GetEdgePath(i, k, path);
}

INT	KEdgeList::Init(INT nVects)
{
	_ASSERT(nVects > 0);
	m_nVecs = nVects;
	m_Graph = new SHORT[nVects * nVects];
	m_PathMap = new SHORT[nVects * nVects];
	memset(m_Graph, 0xff, sizeof(SHORT) * nVects * nVects);
	memset(m_PathMap, 0xff, sizeof(SHORT) * nVects * nVects);
	return TRUE;
}

BOOL KEdgeList::GetEdgePath(INT i, INT j, KEdgePath& path)
{
	if (i == j)
		return FALSE;
	if (!HasEdge(i, j))
		return FALSE;
	path = *m_Paths[m_Graph[i*m_nVecs + j]];
	if (i > j)
		path.Reserve();
	return TRUE;
}

BOOL KEdgeList::AddEdge(INT i ,INT j, KEdgePath* pPath)
{
	_ASSERT(m_Graph && (i != j));
	if (!HasEdge(i, j))
	{
		if ( i > j )
			pPath->Reserve();
		m_Paths.push_back(pPath);
		m_Graph[i*m_nVecs + j] = m_Graph[j*m_nVecs + i] = m_Paths.size() - 1;
		return TRUE;
	}
	return FALSE;
}
BOOL KEdgeList::HasEdge(INT i, INT j)
{
	_ASSERT(m_Graph);
	return m_Graph[i*m_nVecs + j] != -1;
}

BOOL KEdgeList::LoadFromBuf( LPBYTE pBuf ) 
{
	m_nVecs = *(INT*)pBuf;
	pBuf += sizeof(INT);
	//m_Graph = new SHORT[m_nVecs * m_nVecs];
	SAFE_DELETE(m_PathMap);
	m_PathMap = new SHORT[m_nVecs * m_nVecs];
	//memcpy(m_Graph, pBuf, sizeof(SHORT) * m_nVecs * m_nVecs);
	//pBuf += sizeof(SHORT) * m_nVecs * m_nVecs;
	memcpy(m_PathMap, pBuf, sizeof(SHORT) * m_nVecs * m_nVecs);
	pBuf += sizeof(SHORT) * m_nVecs * m_nVecs;
	//INT nSize = *(INT*)pBuf;
	//pBuf += sizeof(INT);
	//m_Paths.resize(nSize);
	//{for (INT i = 0; i < nSize; i++)
	//{
	//	m_Paths[i] = new KEdgePath;
	//	m_Paths[i]->LoadFromBuf(pBuf);
	//}}
	return TRUE;
}

#ifdef KD_GEN_PATH_FILE

BOOL KEdgeList::SaveToBuf( LPBYTE& pBuf ) 
{
	*(INT*)pBuf = m_nVecs;
	pBuf += sizeof(INT);
	//memcpy(pBuf, m_Graph, sizeof(SHORT) * m_nVecs * m_nVecs);
	//pBuf += sizeof(SHORT) * m_nVecs * m_nVecs;
	memcpy(pBuf, m_PathMap, sizeof(SHORT) * m_nVecs * m_nVecs);
	pBuf += sizeof(SHORT) * m_nVecs * m_nVecs;
	//*(INT*)pBuf = m_Paths.size();
	//pBuf += sizeof(INT);
	//{for (INT i = 0; i < (INT)m_Paths.size(); i++)
	//{
	//	m_Paths[i]->SaveToBuf(pBuf);
	//}}
	return TRUE;
}

INT	KEdgeList::GetDistance(INT i, INT j)
{
	if (i == j)
		return 0;
	if (m_Graph[i*m_nVecs + j] != -1)
		return m_Paths[m_Graph[i*m_nVecs + j]]->GetLen();
	return -1;
}

VOID SearchPath(INT** PathMap, std::vector<INT>& PathIJ, INT i, INT j)
{
	if (i == j)
		return;
	if (PathMap[i][j] == j)
		PathIJ.push_back(j);
	else
	{
		_ASSERT(PathMap[i][j] != -1);
		SearchPath(PathMap, PathIJ, i, PathMap[i][j]);
		SearchPath(PathMap, PathIJ, PathMap[i][j], j);
	}
}

BOOL KEdgeList::CreateMinMap()
{
	INT n = m_nVecs;
	INT i, j, k;
	INT**  DisMap = NULL;		// 距离矩阵
	DisMap = new INT*[n];
	INT**  PathMap = NULL;		// 位置矩阵
	PathMap = new INT*[n];

	// 初始化边表和距离矩阵
	for (i = 0; i < n; i++)
	{
		DisMap[i] = new INT[n];
		PathMap[i] = new INT[n];
		for (j = 0; j < n; j++)
		{
			INT dis = GetDistance(i, j);
			DisMap[i][j] = dis;
			if (dis != -1)
				PathMap[i][j] = j;
			else
				PathMap[i][j] = -1;
		}
		DisMap[i][i] = 0;
		PathMap[i][i] = i;
	}

	// washile 算法求最短路径
	for (k = 0; k < n; k++)
	{
		for (i = 0; i < n; i++)
		{
			if (i != k)
			{
				for (j = 0; j < n; j++)
				{
					if( j != k && j != i)
					{
						if ((DisMap[i][k] != -1.0f) && (DisMap[k][j] != -1.0f))
						{
							INT nDis = DisMap[i][k] + DisMap[k][j];
							if ((DisMap[i][j] == -1.0f) || (nDis < DisMap[i][j]))
							{
								DisMap[i][j] = nDis;
								DisMap[j][i] = nDis;
								PathMap[i][j] = k;
								PathMap[j][i] = k;
							}
						}
					}
				}
			}
		}
	}
	
	for (i = 0; i < n; i++)
	{
		for (j = 0; j < n; j++)
		{
			_ASSERT(PathMap[i][j] >= -1 && PathMap[i][j] < n);
		}
	}
	// 修正路径
	for (i = 0; i < n; i++)
	{
		m_PathMap[i*n+i] = i;
	}

	for (i = 0; i < n; i++)
	{
		for (j = 0; j < n; j++)
		{
			if ((i == j) || (m_PathMap[i*n+j] != -1))
				continue;

			if (PathMap[i][j] == -1)
			{
				// ASSERT((PathIJ.size() >= 2) && "图形不联通!");
				continue;
			}
			std::vector<INT> PathIJ;
			PathIJ.push_back(i);
			SearchPath(PathMap, PathIJ, i, j);
			
			for (INT x = 0; x < (INT)PathIJ.size()-1; x++)
			{
				for (INT y = x+1; y < (INT)PathIJ.size(); y++)
				{
					INT i1 = PathIJ[x];
					INT j1 = PathIJ[y];
					_ASSERT(i1 < n && j1 < n);
					m_PathMap[i1*m_nVecs+j1] = PathIJ[x+1];
					m_PathMap[j1*m_nVecs+i1] = PathIJ[y-1];
				}
			}
		}
	}
	// 释放内存
	for (i = 0; i < n; i++)
	{
		delete[] DisMap[i];
		delete []PathMap[i];
	}
	delete[] DisMap;
	delete[] PathMap;

	return TRUE;
}

#endif

#endif // #ifndef _SERVER

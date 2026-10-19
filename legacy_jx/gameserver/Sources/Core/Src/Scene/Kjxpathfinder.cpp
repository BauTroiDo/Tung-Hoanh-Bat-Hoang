/* -------------------------------------------------------------------------
//	文件名		：	jx_path_finder.cpp
//	创建者		：	liupeng
//	创建时间	：	2007-5-17
//	功能描述	：	封装自动寻路类
//					
// -----------------------------------------------------------------------*/

#include "KCore.h"
#include <math.h>
#ifndef _SERVER
#include "Kjxpathfinder.h"
#include "ksimplebuffer.h"
#include "ScenePlaceMapC.h"

KJXPathFinder g_JXPathFinder;


#define USE_MAX_OBSTACLE 0

KJXPathFinder::KJXPathFinder(BOOL bFactor): m_nMapWidth(0), m_nMapHeight(0), m_sCloseFlag(cs_CloseFlagStart),
								m_sOpenFlag(cs_OpenFlagStart), m_map(NULL),
								m_bHasFactor(bFactor)
{
}
KJXPathFinder::~KJXPathFinder()
{
	ClearMap();
}

BOOL KJXPathFinder::CreateMap(INT w, INT h)
{
	ClearMap();
	_ASSERT((sizeof(PathNode) % 4) == 0);

	m_map = new PathNode*[w];
	memset(m_map, 0, sizeof(PathNode*) * w);
	if (!m_map)
		return FALSE;

	for (INT i = 0; i < w; i++)
	{
		m_map[i] = new PathNode[h];
		if (!m_map[i])
		{
			ClearMap();
			return FALSE;
		}
		memset(m_map[i], 0, sizeof(PathNode) * h);
	}
	
	m_nMapWidth = w;
	m_nMapHeight = h;

	m_OpenTable.Reset(w * h);
	return TRUE;
}

VOID KJXPathFinder::ClearMap()
{
	for (INT i = 0; i < m_nMapWidth; i++)
		delete[] m_map[i];
	delete[] m_map;
	m_map = NULL;
	m_nMapWidth = 0;
	m_nMapHeight = 0;

	m_sCloseFlag = cs_CloseFlagStart;
	m_sOpenFlag = cs_OpenFlagStart;

	for (INT a = 0; a < m_OpenTable.GetSize(); a++)
		delete m_OpenTable.GetNode(a);

	m_OpenTable.Reset(0);
}

BOOL KJXPathFinder::Init(char*  szFileName)
{
	m_bReady = FALSE;
	KFile file;
	if (!file.Open(szFileName))
		return FALSE;

	INT nSize = file.Size();
	if (nSize == -1)
		return FALSE;

	KSimpleBuffer cBuffer;
	LPBYTE pBuf = cBuffer.GetBuffer(nSize);
	if (!file.Read((CHAR*)pBuf, nSize))
		return FALSE;

	if (!LoadMap(pBuf, nSize))
		return FALSE;

	m_target.x = m_target.y = 0;
	m_bReady = TRUE;
	return TRUE;
}

BOOL KJXPathFinder::LoadMap(LPBYTE pBuf, INT nSize)
{
	RECT rc = *((RECT*)pBuf);
	m_LeftTop.left = rc.left * 16;
	m_LeftTop.top = rc.top * 32;
	m_LeftTop.right = rc.right * 16;
	m_LeftTop.bottom = rc.bottom * 32;
	m_vec.Reset();
	pBuf += sizeof(rc);
	nSize -= sizeof(rc);

	INT w = (rc.right - rc.left + 1) * 16;//dat文件
	INT h = (rc.bottom - rc.top + 1) * 32;//文件夹
	
	if (!CreateMap(w, h))
		return FALSE;

	_ASSERT(w * h == nSize);

	int m_nHeight,m_nWidth;
	m_nWidth  = rc.right - rc.left + 1;
	m_nHeight = rc.bottom - rc.top + 1; 
	//m_vec.AddVector();

	KScenePlaceMapC nMapsBuf;
		  nMapsBuf.GetMapBarrierBuf(m_ppbtBarrier);	//获取 这个区域的 障碍数据 有 512个点
	int x,y;
	  for (y=0;y<m_nHeight;y++)	   //历遍所有文件夹
		  for (x=0;x<m_nWidth;x++) //i*j 个区域	dat文件	历遍所有dat文件编号
		  {   
			//KRegion::LoadLittleMapData(x+m_MapRect.left,y+m_MapRect.top,SubWorld[0].m_szMapPath,GetBarrierBuf(x,y));
			//int nRegidx =(m_DestRegIdx_Y - m_MapRect.top) * m_nMapWidth +(m_DestRegIdx_X - m_MapRect.left);
			  for (int i = 0; i < REGION_GRID_HEIGHT; ++i)
			  {//512个点
				  for (int j = 0; j < REGION_GRID_WIDTH; ++j)
				  { 
					  if (m_ppbtBarrier)
					  {	
						  m_vec.AddVector(m_ppbtBarrier[y*m_nWidth+x][i * REGION_GRID_WIDTH + j]);
					  } 
				  }	   
			  } 
		  
		  }
	//	  0 512

	//nIdx(第几个区域) = (h/32 - rc.top)* （rc.right - rc.left + 1） +(w/16 - rc.left)
			  


	for ( y = 0; y < h; y++)	//历遍每个点
	for ( x = 0; x < w; x++)
	{
		//m_ppbtBarrier[y/32+rc.top][x/16];
		INT obstacle = *pBuf++>>4;
		m_map[x][y].factor = 1.0f;
#if USE_MAX_OBSTACLE
		if (obstacle == 0)
			m_map[x][y].canwoalk = walkable;
#else
		if (obstacle == 0 || (2<=obstacle && obstacle<=5)) //不记录完整障碍？
			m_map[x][y].canwoalk = walkable;  //记录障碍点
#endif
	}

	if (m_bHasFactor)
		CalcFactors();
	return TRUE;

}

// 计算点的权值，根据点离障碍点的距离决定他的权值。

// 为计算权值，是考虑每个障碍点去计算周围的非障碍点的值，这样比逐个直接计算非障碍点
// 的权值有效率的多。

// 基本假设就是如果一个障碍点在当前障碍点的左边，那么就不需要计算当前障碍点
// 对它左边的点的影响，因为它左边的一个障碍点对它们的影响肯定要大于当前障碍点
// 其他可以进行加速计算的情况就依此类推了。最终的效果就是需要计算的就是靠近边界的
// 障碍点需要计算。只是这样做仍然有冗余的情况出现。

// 提高效率的做法是查看一个障碍点周围的8个点，针对各种不同的组合情况进行裁剪需要
// 计算的点，原则上可以进行更加细致的裁剪，但是如果考虑更大的范围就会使复杂性大大
// 增加

// 开始的时候所有的点的factor都一样为最小值1，当发现一个障碍点对某个点有作用的时候
// 如果发现对它影响大于factor就用计算所得的值替换它。

// 障碍点的影响半径
CONST INT	cnFactorRadius = 15;

// dx，dy为点的距离半径,为提高速度采用查表的方式来计算
inline FLOAT CalcFactor(INT dx, INT dy)
{
	// 采用简单计算的方式
	_ASSERT(abs(dx) < cnFactorRadius && abs(dy) < cnFactorRadius);
	_ASSERT(abs(dx) + abs(dy) > 0);
	FLOAT dis = sqrt((FLOAT)(dx * dx + dy * dy));
	_ASSERT(dis >= 1.0f);
	if ((FLOAT)cnFactorRadius - dis <= 0)
		return 1.0f;
	FLOAT res = 2.0f * ((FLOAT)cnFactorRadius - dis) / (FLOAT)cnFactorRadius + 1.0f;
	return res;
}

VOID KJXPathFinder::CalcFactors()
{
	// 先采用简单的方式计算
	// 先计算边界的影响
	{
	for (INT a = 0; a < cnFactorRadius-1; a++)
	{
		FLOAT factor = CalcFactor(a+1, 0);
		// 左边和右边
		INT i = 0;
		for (i = 0; i < m_nMapHeight; i++)
		{
			if (m_map[a][i].canwoalk && factor > m_map[a][i].factor)
				m_map[a][i].factor = factor;
			if (m_map[m_nMapWidth-a-1][i].canwoalk && factor > m_map[m_nMapWidth-a-1][i].factor)
				m_map[m_nMapWidth-a-1][i].factor = factor;
		}
		// 上边和下边
		for (i = 0; i < m_nMapWidth; i++)
		{
			if (m_map[i][a].canwoalk && factor > m_map[i][a].factor)
				m_map[i][a].factor = factor;
			if (m_map[i][m_nMapHeight-a-1].canwoalk && factor > m_map[i][m_nMapHeight-a-1].factor)
				m_map[i][m_nMapHeight-a-1].factor = factor;
		}
	}
	}

	// 每个障碍点计算它的覆盖范围
	for (INT i = 0; i < m_nMapWidth; i++)
	{
		for (INT j = 0; j < m_nMapHeight; j++)
		{
			if (!m_map[i][j].canwoalk)
			{
				// 裁剪掉上下左右都有障碍点的障碍点
				BOOL bCanCalc = FALSE;
				if (i - 1 >= 0 && m_map[i-1][j].canwoalk)		// 左
					bCanCalc = TRUE;
				else if (i + 1 < m_nMapWidth && m_map[i+1][j].canwoalk) // 右
					bCanCalc = TRUE;
				else if (j - 1 >= 0 && m_map[i][j-1].canwoalk)		// 上
					bCanCalc = TRUE;
				else if (j + 1 < m_nMapHeight && m_map[i][j+1].canwoalk) // 下
					bCanCalc = TRUE;
				if (bCanCalc)
				{
					// 在一个正方形边界中寻找作用点
					INT x = i - cnFactorRadius + 1;
					INT y = j - cnFactorRadius + 1;

					for (INT a = x; a < x + cnFactorRadius*2-1; a++)
					for (INT b = y; b < y + cnFactorRadius*2-1; b++)
					if (a >= 0 && a < m_nMapWidth && b >= 0 && b < m_nMapHeight)
					{
						if (m_map[a][b].canwoalk)
						{
							FLOAT factor = CalcFactor(a-i, b - j);
							if (factor > m_map[a][b].factor)
								m_map[a][b].factor = factor;
						}
					}
				}
			}
		}
	}
}

BOOL KJXPathFinder::FindPath(FindPathNode& start, FindPathNode& target, BOOL bFactor)
{
	if (!m_bReady)
		return FALSE;

	if (target.x == m_target.x && target.y == m_target.y)
	{
		// 目标点相同, 直接找到下一个点
		FindPathNode next;
		return GetNextStep(start, next) != emKNEXTSTEP_RESULT_NOANYWAY;
	}
	else
	{
		if (target.x < 0 || target.x >= m_nMapWidth || target.y < 0 || target.y >= m_nMapHeight)
			return emKNEXTSTEP_RESULT_NOANYWAY;

		m_target.x = target.x;
		m_target.y = target.y;
		m_bHasFactor = bFactor;

		// 擦除以前的信息
		ResetMap();
		// 将第一个点的信息放入open表
		WORD hcost = 10*(abs(start.x - target.x) + abs(start.y - target.y));
		AddOpenNode(m_target.x, m_target.y, hcost, hcost, -1, -1);
		
		// 查找下一个的
		FindPathNode next;
		return GetNextStep(start, next) != emKNEXTSTEP_RESULT_NOANYWAY;
	}
	return TRUE;
}
KJXPathFinder::KE_NEXTSTEP_RESULT 	
KJXPathFinder::GetNextStep(CONST FindPathNode& now, FindPathNode & nextstep, INT steplen)
{
	if (!m_bReady)
		return emKNEXTSTEP_RESULT_NOANYWAY;

	_ASSERT(steplen > 0);
	KE_NEXTSTEP_RESULT  res = GetNextStep(now, nextstep);
	if (res == emKNEXTSTEP_RESULT_NOANYWAY || res == emKNEXTSTEP_RESULT_ARRIVAL)
		return res;
	else
	{
		// res == emKNEXTSTEP_RESULT_SUCCESS
		_ASSERT(m_map[now.x][now.y].pathflag == m_sCloseFlag || 
		m_map[now.x][now.y].pathflag == m_sOpenFlag );
		INT x = now.x, y = now.y;
		while (!(x == m_target.x && y == m_target.y))
		{
			INT tempx = m_map[x][y].parent_x;
			INT tempy = m_map[x][y].parent_y;
			x = tempx;
			y = tempy;
			steplen--;
			if (steplen == 0)
			{
				nextstep.x = x;
				nextstep.y = y;
				return res;
			}
		}
		nextstep.x = m_target.x;
		nextstep.y = m_target.y;
		return res;
	}
}
KJXPathFinder::KE_NEXTSTEP_RESULT 
KJXPathFinder::GetNextStep(CONST FindPathNode& now, FindPathNode & nextstep)
{
	if (!m_bReady)
		return emKNEXTSTEP_RESULT_NOANYWAY;
	// 判断是否需要进行搜索
	if (now.x < 0 || now.x >= m_nMapWidth || now.y < 0 || now.y >= m_nMapHeight)
		return emKNEXTSTEP_RESULT_NOANYWAY;

	if (m_map[m_target.x][m_target.y].canwoalk != walkable)
		return emKNEXTSTEP_RESULT_NOANYWAY;
	if (m_map[now.x][now.y].canwoalk != walkable)
		return emKNEXTSTEP_RESULT_NOANYWAY;

	if (now.x == m_target.x && now.y == m_target.y)
		return emKNEXTSTEP_RESULT_ARRIVAL;

	// 已经在open表或者close表中
	if ((m_map[now.x][now.y].pathflag == m_sOpenFlag) ||
		(m_map[now.x][now.y].pathflag == m_sCloseFlag))
	{
		nextstep.x = m_map[now.x][now.y].parent_x;
		nextstep.y = m_map[now.x][now.y].parent_y;
		return emKNEXTSTEP_RESULT_SUCCESS;
	}
	
	// 需要进行搜索
	// 修正open表中的cost值使得它的值以新的目标为取值目标
	FixOpenTable(now);
	while (!m_OpenTable.empty())
	{
		OpenNode* pNode = m_OpenTable.PopNode();
		AddCloseNode(pNode, now.x, now.y);
		delete pNode;
		
		if ((m_map[now.x][now.y].pathflag == m_sOpenFlag))
		{
			nextstep.x = m_map[now.x][now.y].parent_x;
			nextstep.y = m_map[now.x][now.y].parent_y;	
			return emKNEXTSTEP_RESULT_SUCCESS;
		}
	}
	return emKNEXTSTEP_RESULT_NOANYWAY;
}

VOID KJXPathFinder::ClearOpenTable()
{
	for (INT a = 0; a < m_OpenTable.GetSize(); a++)
		delete m_OpenTable.GetNode(a);
	m_OpenTable.Reset(0);
}
VOID KJXPathFinder::ResetMap()
{
	ClearOpenTable();
	m_OpenTable.Reset(m_nMapHeight * m_nMapWidth);
	for (INT y = 0; y < m_nMapHeight; y++)
	for (INT x = 0; x < m_nMapWidth; x++)
	{
		PathNode& node = m_map[x][y];
		node.gconst = 0;
		node.parent_x = 0;
		node.parent_y = 0;
		node.pathflag = 0;
	}
	
}

VOID KJXPathFinder::AddOpenNode(INT x, INT y, WORD fcost, WORD hcost, INT px, INT py)
{
	// 放入open表中的必须是既不在open表也不在close表中的节点
	// ASSERT(m_map[x][y].pathflag == 0 && m_map[x][y].canwoalk == walkable);
	if (m_map[x][y].pathflag != 0 || m_map[x][y].canwoalk != walkable)
	{
		return;
	}
	OpenNode* pNode = new OpenNode;
	pNode->x = x;
	pNode->y = y;
	pNode->fcost = fcost;
	pNode->hcost = hcost;
	m_OpenTable.PushNode(pNode);

	m_map[x][y].gconst = fcost - hcost;
	m_map[x][y].parent_x = px;
	m_map[x][y].parent_y = py;
	m_map[x][y].pathflag = (BYTE)m_sOpenFlag;
}

// 将一个open表中的节点放入
VOID KJXPathFinder::AddCloseNode(OpenNode* pNode, INT tx, INT ty)
{
	// 放入close表的必须是
	_ASSERT(m_map[pNode->x][pNode->y].pathflag == m_sOpenFlag);
	m_map[pNode->x][pNode->y].pathflag = (BYTE)m_sCloseFlag;

	// 查找和该open节点相邻的节点,如果不是open或close表中的节点,则放入open表中
	// 如果是open表中的节点,比较新的close节点和
	for (INT y = pNode->y - 1; y <= pNode->y + 1; y++)
	for (INT x = pNode->x - 1; x <= pNode->x + 1; x++)
	{
		if (x < 0 || x >= m_nMapWidth || y < 0 || y >= m_nMapHeight)
			continue;
		if (m_map[x][y].canwoalk != walkable)
			continue;
		if (m_map[x][y].pathflag == m_sCloseFlag)
			continue;
		// 判断是否可以从当前点走到扩展点
		WORD corner = walkable;
		if (x == pNode->x-1)
		{
			if (y == pNode->y-1)
			{				
				if (m_map[pNode->x-1][pNode->y].canwoalk == unwalkable
				 || m_map[pNode->x][pNode->y-1].canwoalk == unwalkable)
					corner = unwalkable;
			}
			else if (y == pNode->y+1)
			{
				if (m_map[pNode->x][pNode->y+1].canwoalk == unwalkable
				 || m_map[pNode->x-1][pNode->y].canwoalk == unwalkable) 
					corner = unwalkable; 
			}
		}
		else if (x == pNode->x+1)
		{
			if (y == pNode->x-1)
			{
				if (m_map[pNode->x][pNode->y-1].canwoalk == unwalkable 
				 || m_map[pNode->x+1][pNode->y].canwoalk == unwalkable) 
					corner = unwalkable;
			}
			else if (y == pNode->x+1)
			{
				if (m_map[pNode->x+1][pNode->y].canwoalk == unwalkable 
				 || m_map[pNode->x][pNode->y+1].canwoalk == unwalkable)
					corner = unwalkable; 
			}
		}
		if (corner != walkable)
			continue;

		// 计算扩展点到open节点的花销
		_ASSERT(abs(x-pNode->x) + abs(y-pNode->y) != 0);
		FLOAT addCost = 0;
		if (abs(x-pNode->x) == 1 && abs(y-pNode->y) == 1)
		{
			if (m_bHasFactor)
				addCost = 14.0f * m_map[pNode->x][pNode->y].factor;
			else
				addCost = 14.0f;
		}
		else
		{
			if (m_bHasFactor)
				addCost = 10.0f * m_map[pNode->x][pNode->y].factor;
			else
				addCost = 10.0f;
		}

		// 计算当前扩展点的gcost
		WORD gcost = m_map[pNode->x][pNode->y].gconst + (WORD)addCost;

		if (m_map[x][y].pathflag != m_sOpenFlag)
		{
			// 添加当前扩展节点到open表
			WORD hcost = 10*(abs(x - tx) + abs(y - ty));
			AddOpenNode(x, y, gcost+hcost, hcost, pNode->x, pNode->y);
		}
		else
		{
			// 修改已经是open表中的节点
			if (gcost < m_map[x][y].gconst)
			{
				m_map[x][y].gconst = gcost;
				m_map[x][y].parent_x = pNode->x;
				m_map[x][y].parent_y = pNode->y;
				for (INT i = 0; i < m_OpenTable.GetSize(); i++)
				{
					OpenNode* pOpenNode = m_OpenTable.GetNode(i);
					if (pOpenNode->x == x && pOpenNode->y == y)
					{
						pOpenNode->fcost = gcost + pOpenNode->hcost;
						m_OpenTable.MakeHeap();	
						break;
					}
				}
			}
		}
	}	
}
// 根据新的目标位置修正open表中的取值
VOID KJXPathFinder::FixOpenTable(CONST FindPathNode& now)
{
	for (INT a = 0; a < m_OpenTable.GetSize(); a++)
	{
		OpenNode* pNode = m_OpenTable.GetNode(a);
		WORD oldfcost = pNode->fcost;
		WORD oldhcost = pNode->hcost;
		_ASSERT(oldfcost - oldhcost == m_map[pNode->x][pNode->y].gconst);
		pNode->hcost = 10*(abs(pNode->x - now.x) + abs(pNode->y - now.y));
		pNode->fcost = pNode->hcost + (oldfcost - oldhcost);
	}
	m_OpenTable.MakeHeap();
}

// 
INT	KJXPathFinder::GetPath(FindPathNode& now, FindPathNode*& pPath)
{
	if (!m_bReady)
		return 0;

	_ASSERT(m_map[now.x][now.y].pathflag == m_sCloseFlag || 
		   m_map[now.x][now.y].pathflag == m_sOpenFlag );
	INT x = now.x, y = now.y;
	INT nLen = 0;
	while (!(x == m_target.x && y == m_target.y))
	{
		nLen ++;
		INT tempx = m_map[x][y].parent_x;
		INT tempy = m_map[x][y].parent_y;
		x = tempx;
		y = tempy;
	}
	if (nLen)
	{
		x = now.x;
		y = now.y;
		pPath = new FindPathNode[nLen];
		FindPathNode* pCurNode = pPath;
		INT nTestLen = 0;
		while (!(x == m_target.x && y == m_target.y))
		{
			pCurNode->x = x;
			pCurNode->y = y;
			pCurNode++;
			nTestLen++;
			INT tempx = m_map[x][y].parent_x;
			INT tempy = m_map[x][y].parent_y;
			x = tempx;
			y = tempy;
		}
		_ASSERT(nTestLen == nLen);
	}
	return nLen;
}
#endif

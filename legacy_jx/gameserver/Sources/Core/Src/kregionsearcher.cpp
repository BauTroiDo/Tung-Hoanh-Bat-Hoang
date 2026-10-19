/* -------------------------------------------------------------------------
//	文件名		：	kregionsearcher.cpp
//	创建者		：	zhangfan
//	创建时间	：	2007-4-26 15:55:19
//	功能描述	：	
//
// -----------------------------------------------------------------------*/

#include "KCore.h"
#include "kregionsearcher.h"
#include "KSubworld.h"
#include "KRegion.h"
#include "KNpc.h"
#include "KPlayer.h"
#include "KNpcSet.h"
#include "KObjSet.h"
#include "KMissle.h"
#include "KMath.h"


// -------------------------------------------------------------------------

inline BOOL KRegionSearcher::IsInRange(INT nMapX, INT nMapY) CONST
{
	//对于左上和右下有不同的处理，以避免区域过小造成的范围跳动过大
	INT nDx2	= (nMapX - m_nCurX) << 1;	// * 2
	INT nDy2	= (nMapY - m_nCurY) << 1;	// * 2
	return (nDx2 >= -m_nRange && nDx2 < m_nRange && nDy2 >= -m_nRange && nDy2 < m_nRange);
}

BOOL KRegionSearcher::FirstRegion()
{
	m_pRegion	= NULL;
	if (!m_pSubWorld)
	{
		_ASSERT(FALSE);
		return FALSE;
	}
	// GS、Client采用不同的方法找到左上角第一个Region
#ifdef _SERVER
	INT nRx, nRy;	// 起始Region编号
	nRx	= (m_nCellX - m_nRange/2) / m_pSubWorld->m_nRegionWidth;
	nRy	= (m_nCellY - m_nRange/2) / m_pSubWorld->m_nRegionHeight;
	if ((nRx -= m_pSubWorld->m_nRegionBeginX) < 0)
		nRx	= 0;
	if ((nRy -= m_pSubWorld->m_nRegionBeginY) < 0)
		nRy	= 0;
	if (nRx < m_pSubWorld->m_nWorldRegionWidth && nRy < m_pSubWorld->m_nWorldRegionHeight)
	{
		INT nRegionIdx	= nRy * m_pSubWorld->m_nWorldRegionWidth + nRx;
		if (nRegionIdx < m_pSubWorld->m_nTotalRegion)
			m_pRegion	= &m_pSubWorld->m_Region[nRy * m_pSubWorld->m_nWorldRegionWidth + nRx];
		else
			_ASSERT(FALSE);	// SubWorld本身就有问题？还是这里算法有问题。
	}

#else
	INT nX, nY;	// 起始像素坐标，向左上偏移一个Region
	nX	= (m_nCellX - m_nRange/2 - m_pSubWorld->m_nRegionWidth) * m_pSubWorld->m_nCellWidth;
	nY	= (m_nCellY - m_nRange/2 - m_pSubWorld->m_nRegionHeight) * m_pSubWorld->m_nCellHeight;
	KRegion* pRegion;
	for (INT i = 0; i < MAX_REGION; i++)	// 遍历客户端全部9个Region
	{
		pRegion	= &m_pSubWorld->m_Region[m_pSubWorld->m_ClientRegionIdx[i]];
		// 该Region至少有一部分落在起始点之后
		if (pRegion->m_nRegionX > nX && pRegion->m_nRegionY > nY)
		{
			// 该Region更靠前
			if (!m_pRegion || pRegion->m_RegionID < m_pRegion->m_RegionID)
				m_pRegion	= pRegion;
		}
	}

#endif

	if (!m_pRegion || !m_pcSearcherNpc)
	{
		_ASSERT(FALSE);	// 左上角已经超出地图右侧或下侧
		return FALSE;
	}

	m_nCurX	= m_nCellX - m_pRegion->m_nRegionX / m_pSubWorld->m_nCellWidth;
	m_nCurY	= m_nCellY - m_pRegion->m_nRegionY / m_pSubWorld->m_nCellHeight;

	if (m_nCurX * 2 + m_nRange <= 0 || m_nCurY * 2 + m_nRange <= 0)
	{
		_ASSERT(FALSE);	// 右下角已经超出地图左侧或上侧
		m_pRegion	= NULL;
		return FALSE;
	}

	return TRUE;
}

BOOL KRegionSearcher::NextRegion()
{
	// 临近Region是这样定义的：
	// 3  4  5
	// 2  *  6
	// 1  0  7
	if (m_bSearchRight)
	{
		// 尝试向右
		if (m_nCurX * 2 + m_nRange > (m_pSubWorld->m_nRegionWidth * 2) && m_pRegion->m_pConRegion[6])
		{
			m_pRegion	= m_pRegion->m_pConRegion[6];
			m_nCurX		-= m_pSubWorld->m_nRegionWidth;
			return TRUE;
		}
	}
	else
	{
		// 尝试向左
		if (m_nCurX * 2 - m_nRange < -1 * 2 && m_pRegion->m_pConRegion[2])
		{
			m_pRegion	= m_pRegion->m_pConRegion[2];
			m_nCurX		+= m_pSubWorld->m_nRegionWidth;
			return TRUE;
		}
	}
	// 尝试向下
	if (m_nCurY * 2 + m_nRange > (m_pSubWorld->m_nRegionHeight * 2) && m_pRegion->m_pConRegion[0])
	{
		m_pRegion		= m_pRegion->m_pConRegion[0];
		m_bSearchRight	= !m_bSearchRight;
		m_nCurY			-= m_pSubWorld->m_nRegionHeight;
		return TRUE;
	}
	m_pRegion	= NULL;
	return FALSE;
}

KRegionSearcher::KRegionSearcher(enumType eSearchType, KSubWorld* pSubWorld, INT nMapX, INT nMapY, INT nRange, INT nSearcherIdx, NPC_RELATION sRelation)
{
	if (pSubWorld && nMapX > 0 && nMapY > 0 && nSearcherIdx <= 0 || nSearcherIdx >= MAX_NPC || nRange <= 0)
	{
		_ASSERT(FALSE);
		return;
	}

	m_pSubWorld		= pSubWorld;
	m_nCellX		= nMapX;
	m_nCellY		= nMapY;
	m_nRange		= nRange;
	m_pcSearcherNpc	= NpcSet.GetNpcByIdx(nSearcherIdx);
	m_eSearchType	= eSearchType;
	m_sRelation		= sRelation;

	m_bSearchRight	= TRUE;
	m_pRegion		= NULL;
	m_pNode			= NULL;
	m_nCurIndex		= 0;

	if (!FirstRegion())
		return;

	switch(eSearchType)
	{
	case enum_npc:
		m_pNode	= m_pRegion->m_NpcList.GetHead();
		break;
	case enum_player:
		m_pNode	= m_pRegion->m_PlayerList.GetHead();
		break;
	case enum_obj:
		//m_pNode	= m_pRegion->m_ObjList.GetHead();
		m_nCurIndex = 0;
		break;
	case enum_missile:
		m_pNode	= m_pRegion->m_MissleList.GetHead();
		break;
	case enum_cell:
		break;
	default:
		_ASSERT(FALSE);
	}
}

KNpc* KRegionSearcher::GetNextNpc()
{
	_ASSERT(m_eSearchType == enum_npc);
	if (!m_pRegion || !m_pcSearcherNpc)
		return NULL;

	for (;;)
	{
		while (!m_pNode)
		{
			if (!NextRegion())
				return NULL;
			m_pNode	= m_pRegion->m_NpcList.GetHead();
		}

		KNpc* pNpc	= NpcSet.GetNpcByIdx(((KIndexNode*)m_pNode)->m_nIndex);
		if (!pNpc)
			continue;

		m_pNode		= m_pNode->GetNext();
		
		if (!IsInRange(pNpc->m_MapX, pNpc->m_MapY))
			continue;
		//if (!KNpcRelation::CheckRelationSet(*m_pcSearcherNpc, *pNpc, m_sRelation))
		if (m_sRelation != NpcSet.GetRelation(m_pcSearcherNpc->m_Index,pNpc->m_Index))
			continue;
		return pNpc;
	}	
}

KPlayer* KRegionSearcher::GetNextPlayer()
{
	_ASSERT(m_eSearchType == enum_player);
	if (!m_pRegion || !m_pcSearcherNpc)
		return NULL;

	for (;;)
	{
		while (!m_pNode)
		{
			if (!NextRegion())
				return NULL;
			m_pNode	= m_pRegion->m_PlayerList.GetHead();
		}

		KPlayer* pPlayer	= &Player[((KIndexNode*)m_pNode)->m_nIndex];
		m_pNode		= m_pNode->GetNext();
		if (pPlayer->m_nIndex <= 0)
			continue;

		KNpc* pNpc	= NpcSet.GetNpcByIdx(pPlayer->m_nIndex);
		if (!pNpc)
			continue;

		if (!IsInRange(pNpc->m_MapX, pNpc->m_MapY))
			continue;
		//if (!KNpcRelation::CheckRelationSet(*m_pcSearcherNpc, *pNpc, m_sRelation))
		if (m_sRelation != NpcSet.GetRelation(m_pcSearcherNpc->m_Index,pNpc->m_Index))
			continue;
		return pPlayer;
	}	
}

/*KObj* KRegionSearcher::GetNextObj()
{
	_ASSERT(m_eSearchType == enum_obj);
	if (!m_pRegion || !m_pcSearcherNpc)
		return NULL;

	for (;;)
	{
		while ((UINT)m_nCurIndex >= m_pRegion->m_ObjList.size())
		{
			m_nCurIndex = 0;
			if (!NextRegion())
				return NULL;
		}

		KObj& rcObj	= Object[m_pRegion->m_ObjList[m_nCurIndex]];
		++m_nCurIndex;
		if (!IsInRange(rcObj.m_nMapX, rcObj.m_nMapY))
			continue;
		// Obj暂时不处理关系判断
		return &rcObj;
	}	
}
*/
KMissle* KRegionSearcher::GetNextMissile()
{
	_ASSERT(m_eSearchType == enum_missile);
	if (!m_pRegion || !m_pcSearcherNpc)
		return NULL;

	for (;;)
	{
		while (!m_pNode)
		{
			if (!NextRegion())
				return NULL;
			m_pNode	= m_pRegion->m_MissleList.GetHead();
		}

		KMissle& rcMissile	= Missle[((KIndexNode*)m_pNode)->m_nIndex];
		m_pNode		= m_pNode->GetNext();
		if (rcMissile.m_nLauncher <= 0)
			continue;

		KNpc* pNpc	= NpcSet.GetNpcByIdx(rcMissile.m_nLauncher);
		if (!pNpc)
			continue;

		if (!IsInRange(rcMissile.m_nCurrentMapX, rcMissile.m_nCurrentMapY))
			continue;
		//if (!KNpcRelation::CheckRelationSet(*m_pcSearcherNpc, *pNpc, m_sRelation))
		if (m_sRelation != NpcSet.GetRelation(m_pcSearcherNpc->m_Index,pNpc->m_Index))
			continue;
		return &rcMissile;
	}	
}

KNpc* KRegionSearcher::GetNextNpcRound()
{
	_ASSERT(m_eSearchType == enum_npc);
	if (!m_pRegion || !m_pcSearcherNpc)
		return NULL;

	for (;;)
	{
		while (!m_pNode)
		{
			if (!NextRegion())
				return NULL;
			m_pNode	= m_pRegion->m_NpcList.GetHead();
		}

		KNpc* pNpc	= NpcSet.GetNpcByIdx(((KIndexNode*)m_pNode)->m_nIndex);
		if (!pNpc)
			continue;

		m_pNode		= m_pNode->GetNext();
		if (g_GetDisSquare(pNpc->m_MapX, pNpc->m_MapY, m_nCurX, m_nCurY) * 4 > m_nRange * m_nRange)
			continue;
		//if (!KNpcRelation::CheckRelationSet(*m_pcSearcherNpc, *pNpc, m_sRelation))
		if (m_sRelation != NpcSet.GetRelation(m_pcSearcherNpc->m_Index,pNpc->m_Index))
			continue;
		return pNpc;
	}	
}

#ifndef _SERVER
	#ifdef SWORDONLINE_SHOW_DBUG_INFO
std::vector<POINT> KRegionSearcher::GetValidCell_Round()
{
	std::vector<POINT> vRet;
	if (!m_pRegion)
	{
		return vRet;
	}

	while (1)
	{
		INT nCellX = m_pRegion->m_nRegionX;

		for (INT nWidth = 0; nWidth < m_pRegion->m_nWidth; ++nWidth, nCellX += defLOGIC_CELL_WIDTH)
		{
			INT nCellY = m_pRegion->m_nRegionY;
			for (INT nHeight = 0; nHeight < m_pRegion->m_nHeight; ++nHeight, nCellY += defLOGIC_CELL_HEIGHT)
			{
				INT nRegion, nMapX, nMapY, nOffX, nOffY;
				SubWorld[0].Mps2Map(nCellX, nCellY, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);

			
				if (!(g_GetDisSquare(nMapX, nMapY, m_nCurX, m_nCurY) * 4 > m_nRange * m_nRange))
				{
					POINT point;
					point.x = nCellX;
					point.y = nCellY;
					vRet.push_back(point);
				}
			}
		}

		if (!NextRegion())
		{
			return vRet;
		}
	}
}

std::vector<POINT> KRegionSearcher::GetValidCell_Square()
{
	std::vector<POINT> vRet;
	if (!m_pRegion)
	{
		return vRet;
	}

	while (1)
	{
		INT nCellX = m_pRegion->m_nRegionX;

		for (INT nWidth = 0; nWidth < m_pRegion->m_nWidth; ++nWidth, nCellX += defLOGIC_CELL_WIDTH)
		{
			INT nCellY = m_pRegion->m_nRegionY;
			for (INT nHeight = 0; nHeight < m_pRegion->m_nHeight; ++nHeight, nCellY += defLOGIC_CELL_HEIGHT)
			{
				INT nRegion, nMapX, nMapY, nOffX, nOffY;
				SubWorld[0].Mps2Map(nCellX, nCellY, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);

				if (IsInRange(nMapX, nMapY))
				{
					POINT point;
					point.x = nCellX;
					point.y = nCellY;
					vRet.push_back(point);
				}
			}
		}

		if (!NextRegion())
		{
			return vRet;
		}
	}
}
	#endif
#endif

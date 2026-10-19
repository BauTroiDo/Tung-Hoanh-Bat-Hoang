/* -------------------------------------------------------------------------
//	文件名		：	kregionsearcher.h
//	创建者		：	fanzai
//	创建时间	：	2007-4-26 16:04:35
//	功能描述	：	用于在附近Region中遍历Npc、Player、Missile...的类
//
// -----------------------------------------------------------------------*/
#ifndef __KREGIONSEARCHER_H__
#define __KREGIONSEARCHER_H__

// -------------------------------------------------------------------------

//#include "KMissle.h"
#include <vector>

class KNpc;
class KPlayer;
class KMissle;
class KObj;
class KSubWorld;
class KRegion;

class KRegionSearcher
{
public:
	enum enumType
	{
		enum_npc,
		enum_player,
		enum_obj,
		enum_missile,
		enum_cell,
	};
	
public:
	// 启动搜索后，请不要对Searcher进行删除Npc等操作。
	KRegionSearcher(enumType eSearchType, KSubWorld* pSubWorld, INT nMapX, INT nMapY,
					INT nRange, INT nSearcherIdx, NPC_RELATION sRelation);
	// 旧版接口
	KRegionSearcher(enumType eSearchType, KSubWorld* pSubWorld, INT nMapX, INT nMapY,
					INT nRange, INT nSearcherIdx, INT nRelation)
	{
		new (this) KRegionSearcher(eSearchType, pSubWorld, nMapX, nMapY, nRange,
						nSearcherIdx, nRelation);
	}

	KNpc* GetNextNpc();			// 在方形区域中得到一个Npc。可以安全的删除/移动该返回的Npc，但是不要删除/移动当前Region中的其他Npc（主要是别动链表中下一个Npc）
	KPlayer* GetNextPlayer();	// 在方形区域中得到一个Player。一样，可以安全的……，但是不要……
	//KObj* GetNextObj();			// 在方形区域中得到一个Obj。还是……
	KMissle* GetNextMissile();	// 在方形区域中得到一个Missile。又是……
	KNpc* GetNextNpcRound();	// 在圆形区域中得到一个Npc。……

#ifndef _SERVER
	#ifdef SWORDONLINE_SHOW_DBUG_INFO
		std::vector<POINT> GetValidCell_Round();
		std::vector<POINT> GetValidCell_Square();
	#endif
#endif
protected:
	BOOL FirstRegion();	// 找到左上角第一个Region
	BOOL NextRegion();	// 按照S型路线找到下一个Region

	BOOL IsInRange(INT nMapX, INT nMapY) CONST;

protected:
	INT			m_nCellX, m_nCellY;	// 搜索中心位置（以格子为单位）
	INT			m_nRange;			// 要搜索的范围（以格子为单位）
	KNpc*		m_pcSearcherNpc;	// 搜索者的Npc指针，用于判断Npc关系、是否可见等
	BOOL		m_bSearchRight;		// 正在向右遍历Region（对Region进行S型遍历）
	INT 		m_nCurX, m_nCurY;	// 相对于当前Region，搜索中心的位置（以格子为单位，以当前Region零点为零点）
	enumType	m_eSearchType;	// 搜索类型
	KSubWorld*	m_pSubWorld;	// 正在遍历的SubWorld
	KRegion*	m_pRegion;		// 正在遍历的Region
	KNode*		m_pNode;		// 要遍历的下一个节点
	INT			m_nCurIndex;	// 要遍历的下一个索引
	NPC_RELATION	m_sRelation;	// 要遍历的Npc/Player与搜索者的关系
};

// -------------------------------------------------------------------------

#endif /* __KREGIONSEARCHER_H__ */

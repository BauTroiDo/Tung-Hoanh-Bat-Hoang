/* -------------------------------------------------------------------------
//	文件名		：	kitemroom.h
//	创建者		：	xiaoyufei
//	创建时间	：	2007-7-2 11:14:09
//	功能描述	：	
//
// -----------------------------------------------------------------------*/

#ifndef __KITEMROOM_H__
#define __KITEMROOM_H__

//#include "kitemdatadef.h"
#define	KD_ITEM_INVALID_TYPE			0			// 无效的装备类别号
#define	KD_ITEM_MAX_STACK_COUNT			1			// 默认的物品叠放最大值
#define KD_ITEM_NO_SERIES_LIMIT			-1			// 无五行限制
#define KD_ITEM_NO_LEVEL_LIMIT			0			// 无级别限制
#define KD_ITEM_NO_BIND_LIMIT			-1			// 无绑定限制
#define KD_ITEM_NO_TIMEOUT_LIMIT		-1			// 无 物品过期时间 限制
#define KD_ITEM_INVALID_SUITEID			0			// 无效的套装号
#define	KD_ITEM_MIN_LEVEL				1
#define KD_ITEM_MAX_EQUIP_LEVEL			10			// 装备的最大级别
#define	KD_ITEM_MAX_EQUIP_ENHANCE		16			// 装备最多可以强化16次
#define KD_ITEM_ABRADE_SECOND			5			// 每5秒做一次掉耐久操作
#define KD_ITEM_ABRADE_PRE_TIME			2			// 每次掉2点		
#define KD_ITEM_MAX_RANDATTRIB_LEVEL	(KD_ITEM_MAX_EQUIP_LEVEL + KD_ITEM_MAX_EQUIP_ENHANCE)	// 随机属性最大级别

/*#define KD_ISEQUIP(g)				((g) >= item_equip_general && (g) <= item_equip_green)	// 根据道具类型判断是否装备道具
#define KD_ISSUITE(g)				((g) == item_equip_gold || (g) == item_equip_green)		// 根据道具类型判断是否套装装备
#define KD_ISWEAPON(d)				((d) == equip_meleeweapon || (d) == equip_rangeweapon)	// 根据道具细类判断是否武器（不检查大类）
#define KD_ISORNAMENT(nPos)			((nPos) >= emEQUIPPOS_AMULET && (nPos) <= emEQUIPPOS_PENDANT)	// 根据装备位置判断是否为首饰
#define KD_ISEQUIP_ROOM(nRoom)		((nRoom) == emROOM_EQUIP ||	(nRoom) == emROOM_EQUIPEX )			// 是否能是装备空间
*/
struct KITEM_INDEX								// 标识配置表中的一个物品（一行）
{
	INT		nGenre;				// 8Bits
	INT		nDetailType;		// 12Bits
	INT		nParticular;		// 12Bits
	INT		nLevel;

	bool operator < (CONST KITEM_INDEX &sIndex) CONST
	{
		if (nGenre != sIndex.nGenre)
			return	nGenre < sIndex.nGenre;
		if (nDetailType != sIndex.nDetailType)
			return	nDetailType < sIndex.nDetailType;
		if (nParticular != sIndex.nParticular)
			return	nParticular < sIndex.nParticular;
		return	nLevel < sIndex.nLevel;
	}

	bool operator == (CONST KITEM_INDEX &sIndex) CONST
	{
		return	nGenre == sIndex.nGenre && nDetailType == sIndex.nDetailType &&
			nParticular == sIndex.nParticular && nLevel == sIndex.nLevel;
	}

	bool operator != (CONST KITEM_INDEX &sIndex) CONST
	{
		return !operator == (sIndex);
	}
};

struct KPos
{
	INT nX;
	INT nY;
};

class KItem;

//---------------------------------------------------------------------------------
// KItemRoom

class KItemRoom
{

public:

	class KFinder;

	KItemRoom();
    ~KItemRoom();
	KItemRoom(const KItemRoom &rhl);
	KItemRoom& operator=(const KItemRoom &rhl);

	BOOL	Init(INT nMaxWidth, INT nMaxHeight, INT nWidth = -1, INT nHeight = -1);
	VOID	Release(VOID);
	VOID	Clear(VOID);															// 清除空间里的道具
	INT		GetWidth(VOID) CONST		{ return m_nWidth; }						// 获得可用宽度
	INT		GetHeight(VOID) CONST		{ return m_nHeight; }						// 获得可用高度
	INT		GetCapability(VOID) CONST	{ return m_nWidth * m_nHeight; }			// 获得容量
	INT		CountUsed(VOID) CONST;													// 计算已被占用的格子数
	INT		CountFree(VOID) CONST		{ return GetCapability() - CountUsed(); }	// 计算当前剩余可用格子数

	INT		GetCell(INT nX = 0, INT nY = 0) CONST;									// 获得指定格子（越界也返回0）
	BOOL	SetCell(INT nIdx, INT nX = 0, INT nY = 0);								// 设置指定格子
	BOOL	Resize(INT nWidth, INT nHeight);										// 改变当前可用空间的大小
	BOOL	CanResize(INT m_nWidth, INT m_nHeight) CONST;							// 是否可以改变可用空间大小
	BOOL	IsPickable(INT nIdx = 0, INT nX = 0, INT nY = 0) CONST;					// 判断是否能将物品取出
	BOOL	IsDropable(INT nX = 0, INT nY = 0) CONST;								// 判断是否能将物品放入
	BOOL	PickItem(INT nIdx, INT nX = 0, INT nY = 0);								// 将物品从空间的指定位置中取出
	BOOL	DropItem(INT nIdx, INT nX = 0, INT nY = 0);								// 将物品放入空间的指定位置
	BOOL	PickItemDirectly(INT nIdx, INT nX = 0, INT nY = 0);						// 同PickItem，但是将最大空间作为边界，慎用
	BOOL	DropItemDirectly(INT nIdx, INT nX = 0, INT nY = 0);						// 同DropItem，但是将最大空间作为边界，慎用
	BOOL	GetFreeCell(INT& rnX, INT& rnY) CONST;									// 获得一个空闲格子的坐标
	BOOL	FindItem(INT nIndex, LPINT pnX = NULL, LPINT pnY = NULL) CONST;			// 看是否拥有一个道具，并输出坐标
	KFinder	GetFinder(VOID) CONST;													// 返回一个指向自己的Finder
	VOID	ClearCell(INT nX = 0, INT nY = 0);										// 清除某个格子

protected:

	LPINT	Cell(INT nX, INT nY) CONST;
	LPINT	CellDirectly(INT nX, INT nY) CONST;

	LPINT	m_pnArray;
	INT		m_nWidth;			// 实际宽度
	INT		m_nHeight;			// 实际高度
	INT		m_nMaxWidth;		// 最大宽度，始终分配空间
	INT		m_nMaxHeight;		// 最大高度，始终分配空间

};

//---------------------------------------------------------------------------------
// KItemRoom::KFinder

class KItemRoom::KFinder	// 注意：当宿主空间大小改变时当前查找位置失效，应当重新Begin
{

public:

	KFinder() : m_nPos(0), m_nLast(0), m_pcRoom(NULL) {}
	explicit KFinder(CONST KItemRoom& rcRoom) { Attach(rcRoom); }

	INT		Index(VOID) CONST { return m_nLast; }				// 获得上次查找的结果（道具索引）
	KPos	Pos(VOID) CONST;									// 获得上次查找结果的位置（返回坐标为负表示还没找到过任何结果）
	BOOL	Begin(INT nX = 0, INT nY = 0);						// 准备指定位置开始查找
	VOID	Attach(CONST KItemRoom& rcRoom);					// 附着另一个空间

	template<typename KCondT>
	BOOL	Next(CONST KCondT& rfnCond);						// 按指定条件查找下一个

protected:

	INT					m_nPos;									// 下一个查找位置
	INT					m_nLast;								// 最后一次找到的结果
	CONST KItemRoom*	m_pcRoom;								// 宿主空间

};

template<typename KCondT>
BOOL KItemRoom::KFinder::Next(CONST KCondT& rfnCond)
{
	if (!m_pcRoom->GetWidth() || !m_pcRoom->GetHeight())
		return	FALSE;

	INT  nPos = m_nPos;
	INT  nIdx = 0;

	do
	{
		INT nX = nPos % m_pcRoom->GetWidth();
		INT nY = nPos / m_pcRoom->GetWidth();
		LPINT pnCell = m_pcRoom->Cell(nX, nY);
		if (!pnCell)
			return	FALSE;
		nIdx = *pnCell;
		++nPos;
	}
	while (!rfnCond(nIdx));

	m_nPos  = nPos;
	m_nLast = nIdx;
	return	TRUE;
}

//---------------------------------------------------------------------------------
// KRoomFindCondition

namespace KRoomFindCondition
{
	// 查找条件：普通函数

	bool Any(INT nIdx);							// 永真（遍历）
	bool Free(INT nIdx);						// 是否空闲
	bool Used(INT nIdx);						// 是否被占用
	bool Equip(INT nIdx);						// 是否装备

	// 查找条件：仿函数

	class SameIndex								// 是否与指定道具索引相同
	{
	public:
		explicit SameIndex(INT nIndex) : m_nIndex(nIndex) {}
		explicit SameIndex(CONST KItem& rcItem);
		bool operator () (INT nIdx) CONST { return nIdx == m_nIndex; }
	protected:
		INT m_nIndex;
	};

	class SameItem								// 是否与指定道具具有相同的类型
	{
	public:
		explicit SameItem(CONST KITEM_INDEX& rsIndex) : m_sIndex(rsIndex) {}
		explicit SameItem(CONST KItem& rcItem);
		explicit SameItem(
			INT nGenre		= KD_ITEM_INVALID_TYPE,
			INT nDetail		= KD_ITEM_INVALID_TYPE,
			INT nParticular	= KD_ITEM_INVALID_TYPE, 
			INT nLevel		= KD_ITEM_NO_LEVEL_LIMIT
		);
		bool operator () (INT nIdx) CONST;
	protected:
		KITEM_INDEX m_sIndex;
	};

	class SameIndexSeries : public SameItem		// 相同的类型和五行
	{
	public:
		explicit SameIndexSeries(CONST KITEM_INDEX& rsIndex, INT nSeries = KD_ITEM_NO_SERIES_LIMIT, INT nBind = KD_ITEM_NO_BIND_LIMIT)
			: SameItem(rsIndex), m_nSeries(nSeries), m_nBind(nBind) {}
		explicit SameIndexSeries(
			INT nGenre		= KD_ITEM_INVALID_TYPE,
			INT nDetail		= KD_ITEM_INVALID_TYPE,
			INT nParticular	= KD_ITEM_INVALID_TYPE, 
			INT nLevel		= KD_ITEM_NO_LEVEL_LIMIT,
			INT nSeries		= KD_ITEM_NO_SERIES_LIMIT,
			INT nBind		= KD_ITEM_NO_BIND_LIMIT
		);
		bool operator () (INT nIdx) CONST;
	protected:
		INT	m_nSeries;
		INT m_nBind;
	};

	class SameClass								// Class是否相同
	{
	public:
		SameClass(LPCSTR pszClass) : m_pszClass(pszClass) {}
		bool operator () (INT nIdx) CONST;
	protected:
		LPCSTR m_pszClass;
	};

	class StackWith : public SameIndexSeries	// 是否可以与之叠加
	{
	public:
		StackWith(CONST KItem& rcItem);
		StackWith(CONST KITEM_INDEX& rsIndex, INT nSeries = KD_ITEM_NO_SERIES_LIMIT, BOOL bBind = FALSE)
			: SameIndexSeries(rsIndex, nSeries, bBind) {}
		bool operator () (INT nIdx) CONST;
	};

}

// -------------------------------------------------------------------------

#endif /* __KITEMROOM_H__ */

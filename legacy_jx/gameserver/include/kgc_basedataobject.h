/* -------------------------------------------------------------------------
//	文件名		：	kgc_basedataobject.h
//	创建者		：	luobaohang
//	创建时间	：	2006-12-25 9:28:44
//	功能描述	：	底层数据对象
//
// -----------------------------------------------------------------------*/
#ifndef __KGC_BASEDATAOBJECT_H__
#define __KGC_BASEDATAOBJECT_H__

#include "kbufpool.h"
#include "kgc_basedatadef.h"
#include "kgc_datapoolmanager.h"
#include "utilities.h"
#ifdef KG_GC_SERVER
#include "kgc_dbmanager.h"
#endif

//////////////////////////////////////////////////////////////////////////

class KBranchDesc
{
public:
	//功能:添加一个分支表描述，在子游戏模块Init时调用
	//nKey:表的Key（识别标志，一个Branch的各个表都应有不同的Key）
	//eType:表的类型
	//nFlag:表的属性
	//szName:表的名字，用于调试查看。如果是一级表，表名将用于数据库存储的字段值
	//返回:所增加的分支表的分支结构描述指针
	KBranchDesc *AddBranchTableDesc(INT nKey, KE_TABLE_TYPE eType, INT Flag, LPCSTR szName);

	//功能:添加一个数据表描述，在子游戏模块Init时调用
	VOID		AddDataTableDesc(INT nKey, KE_TABLE_TYPE eType, INT Flag, LPCSTR szName);
	TABLE_DESC *GetTableDesc(INT nKey)
	{
		std::map<INT, TABLE_DESC*>::iterator it = m_mpData.find(nKey);
		if (it == m_mpData.end())
			return NULL;
		return it->second;
	}
	TABLE_DESC *m_pOwner;	//属于哪一个（分支）表描述
	std::map<INT, TABLE_DESC*> m_mpData;
};

class KBranch : public KMapTable<INT, INT>
{
public:
	using KMapTable<INT, INT>::m_mpData;
	using KMapTable<INT, INT>::m_pDesc;
	typedef std::map<INT,INT>::iterator KIT_MAP_INTINT;
	template<typename KeyT, typename ValueT>
	inline _KGC_PTableObject GetTable(INT nKey)
	{
		KIT_MAP_INTINT it = m_mpData.find(nKey);
		if (it == m_mpData.end())
			return NULL;
		return g_DataPoolManager.GetTable<KeyT, ValueT>(it->second);
	}
	//template<typename TableT>
	/*inline _KGC_PTableObject GetTable(INT nKey)
	{
		KIT_MAP_INTINT it = m_mpData.find(nKey);
		if (it == m_mpData.end())
			return NULL;
		return g_DataPoolManager.GetTable<typename TableT::KeyT, typename TableT::ValueT>(it->second);
	}*/
};

template <typename KeyT, typename ParamT>
class KBaseBranchTable : public KMapTable<KeyT, INT>
{
	typedef KMapTable<KeyT, INT> super_class;
public:
	using super_class::m_mpData;
	using super_class::m_pDesc;
	//增加一个Branch，返回Branch ID
	INT		AddBranch(ParamT Key);
	BOOL	DelBranch(ParamT Key);
};

class KIntBranchTable : public KBaseBranchTable<INT, INT>
{
public:
	typedef KBaseBranchTable<INT, INT> SuperT;
	using SuperT::m_mpData;
	using SuperT::m_pDesc;
	typedef INT KeyT;
	typedef INT ValueT;
};

class KStrBranchTable : public KBaseBranchTable<STR_TYPE, LPCSTR>
{
public:
	typedef KBaseBranchTable<STR_TYPE, LPCSTR> SuperT;
	using SuperT::m_mpData;
	using SuperT::m_pDesc;
	typedef STR_TYPE KeyT;
	typedef INT ValueT;
};

template<typename KeyT, typename ParamT>
class KBaseIntTable : public KMapTable<KeyT, INT>
{
	typedef typename std::map<KeyT,INT>::value_type	VALUETYPE;
	typedef typename std::map<KeyT,INT>::iterator	ITERATOR;
	typedef KMapTable<KeyT, INT>					super_class;
public:
	using super_class::m_mpData;
	using super_class::m_pDesc;
	INT		GetValue(ParamT Key)
	{
		typename std::map<KeyT,INT>::iterator it = super_class::m_mpData.find(Key);
		if (it == super_class::m_mpData.end())
			return 0;
		return it->second;
	}
	BOOL	SetValue(ParamT Key, INT nValue)
	{
		if (nValue == 0)
			return DelValue(Key);
		ITERATOR it = super_class::m_mpData.find(Key);
		if (it == super_class::m_mpData.end())
		{
			if (nValue == 0)
				return TRUE;
			return AddValue(Key, nValue);
		}
		it->second = nValue;
		KD_DBPUSH_SAVE4(this, Key, nValue, emKOPER_MODIFY_DATA);
		return TRUE;
	}
	BOOL	AppendValue(ParamT Key, INT nAdd)
	{
		ITERATOR it = super_class::m_mpData.find(Key);
		if (it == super_class::m_mpData.end())
			return AddValue(Key, nAdd);
		it->second += nAdd;
		KD_DBPUSH_SAVE4(this, Key, it->second, emKOPER_MODIFY_DATA);
		return TRUE;
	}
	BOOL	AddValue(ParamT Key, INT nValue)
	{
		if (!super_class::m_mpData.insert(VALUETYPE(Key, nValue)).second)
			return FALSE;
		KD_DBPUSH_SAVE4(this, Key, nValue, emKOPER_ADD_DATA);
		return TRUE;
	}
	BOOL	DelValue(ParamT Key)
	{
		if (super_class::m_mpData.erase(Key) <= 0)
			return TRUE;
		KD_DBPUSH_SAVE4(this, Key, 0, emKOPER_DEL_DATA);
		return TRUE;
	}
	INT		GetSize()
	{
		return (INT)super_class::m_mpData.size();
	}
};

class KMapIntIntTable : public KBaseIntTable<INT, INT>
{
public:
	typedef KBaseIntTable<INT, INT> SuperT;
	typedef INT KeyT;
	typedef INT ValueT;
};
class KMapStrIntTable : public KBaseIntTable<STR_TYPE, LPCSTR>
{
public:
	typedef KBaseIntTable<STR_TYPE, LPCSTR> SuperT;
	typedef STR_TYPE KeyT;
	typedef INT ValueT;
};

template<typename KeyT, typename ParamT>
class KBaseBufTable : public KMapTable<KeyT, DATA_DESC>
{
	typedef typename std::map<KeyT,DATA_DESC>::value_type	VALUETYPE;
	typedef typename std::map<KeyT,DATA_DESC>::iterator		ITERATOR;
	typedef std::pair<ITERATOR, bool>	PAIR_INSERT;
	typedef KMapTable<KeyT, DATA_DESC>	super_class;
public:
	KTPBuf	GetBuf(ParamT Key)
	{
		ITERATOR it = super_class::m_mpData.find(Key);
		if (it == super_class::m_mpData.end())
			return NULL;
		return g_DataPoolManager.GetBufData(it->second);
	}

	INT		GetBufSize(ParamT Key)
	{
		ITERATOR it = super_class::m_mpData.find(Key);
		if (it == super_class::m_mpData.end())
		{
			KGC_Error(Key);
			return -1;
		}
		return it->second.m_DataSize;
	}
	//eBufType表示要预留的缓冲区大小
	//如果eReserverSize < nSize，则由nSize + 1计算要预留的大小（多留一个字节）
	BOOL	AddBuf(ParamT Key, LPCVOID pBuf, size_t nSize, KE_BUFFER_TYPE eReserverSize = BUF_0)
	{
		if ((size_t)eReserverSize < nSize)
			eReserverSize = GetPropReserveBufSize((INT)nSize + 1);	//多留一个字节
		KGC_CONFIRM_RET_FALSE(eReserverSize != BUF_0);
		DATA_DESC ds;
		ds.m_BufType = eReserverSize;
		CHAR *pRet = g_DataPoolManager.NewBufData(eReserverSize, &ds.m_nBufIndex);
		memcpy(pRet, pBuf, nSize);
		pRet[nSize] = 0;	//多留的字节填0，增加安全性（比如buf误当string使用时）
		ds.m_DataSize = (unsigned)nSize;
		if (!super_class::m_mpData.insert(VALUETYPE(Key, ds)).second)
		{
			ASSERT(FALSE);
			g_DataPoolManager.FreeBufData(ds);
			return FALSE;
		}
		KD_DBPUSH_SAVE5(this, Key, pBuf, (INT)nSize, emKOPER_ADD_DATA);
		return TRUE;
	}
	BOOL	SetBuf(ParamT Key, LPCVOID pBuf, size_t nSize)
	{
		ITERATOR it = super_class::m_mpData.find(Key);
		//若此Key不存在则变为add
		if (it == super_class::m_mpData.end())
		{		
			return AddBuf(Key, pBuf, nSize);
		}
		CHAR *pBufData;
		if ((size_t)it->second.m_BufType < nSize + 1)
		{
			KE_BUFFER_TYPE eBuf = GetPropReserveBufSize(nSize + 1);
			KGC_CONFIRM_RET_FALSE(eBuf != BUF_0);
			g_DataPoolManager.FreeBufData(it->second);
			it->second.m_BufType = eBuf;
			pBufData = g_DataPoolManager.NewBufData(it->second.m_BufType, &it->second.m_nBufIndex);
		}
		else
		{
			pBufData = g_DataPoolManager.GetBufData(it->second);
		}
		KGC_CONFIRM_RET_FALSE(pBufData);
		if (pBufData != pBuf)
			memcpy(pBufData, pBuf, nSize);
		pBufData[nSize] = 0; //多留的字节填0，增加安全性（比如buf误当string使用时）
		it->second.m_DataSize = (unsigned)nSize;
		KD_DBPUSH_SAVE5(this, Key, pBuf, (INT)nSize, emKOPER_MODIFY_DATA);
		return TRUE;
	}
	BOOL	DelBuf(ParamT Key)
	{
		//从数据池删除对象
		ITERATOR it = super_class::m_mpData.find(Key);
		if (it == super_class::m_mpData.end())
		{
			//ASSERT(FALSE);
			return FALSE;
		}
		g_DataPoolManager.FreeBufData(it->second);
		super_class::m_mpData.erase(it);
		KD_DBPUSH_SAVE4(this, Key, 0, emKOPER_DEL_DATA);
		return TRUE;
	}

};

class KMapIntBufTable : public KBaseBufTable<INT, INT>
{
public:
	typedef KBaseBufTable<INT, INT> SuperT;
	typedef INT KeyT;
	typedef DATA_DESC ValueT;
};

class KMapStrBufTable : public KBaseBufTable<STR_TYPE, LPCSTR>
{
public:
	typedef KBaseBufTable<STR_TYPE, LPCSTR> SuperT;
	typedef STR_TYPE KeyT;
	typedef DATA_DESC ValueT;
};

KBaseTable *GCNewTable(TABLE_DESC *pTabDesc, INT &TableID);

// -------------------------------------------------------------------------

#endif /* __KGC_BASEDATAOBJECT_H__ */

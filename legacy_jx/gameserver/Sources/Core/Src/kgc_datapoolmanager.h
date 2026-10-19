/* -------------------------------------------------------------------------
//	文件名		：	kgc_datapoolmanager.h
//	创建者		：	luobaohang
//	创建时间	：	2006-12-4 16:14:10
//	功能描述	：	数据池管理
//
// -----------------------------------------------------------------------*/
#ifndef __KGC_DATAPOOLMANAGER_H__
#define __KGC_DATAPOOLMANAGER_H__
//#include "kgc_server_client_differ.h"
#include "kgc_basedatadef.h"
#include "kbufpool.h"

/*#ifdef _MSC_VER
#if _MSC_VER < 1300
(kgc_datapoolmanager.h)：Using MSVC6(or below) May Cause Problems for Limitations in Compatibility With C++ Standard;
#endif
#endif	*/
// -------------------------------------------------------------------------
//自动执行LPVOID 到底层数据对象指针的转换
class KBranch;
class KBranchDesc;
class KIntBranchTable;
class KStrBranchTable;
class KMapIntIntTable;
class KMapStrIntTable;
class KMapIntBufTable;
class KMapStrBufTable;
#define _KD_CHANGETO_PTYPE(tname) operator tname *()CONST { return reinterpret_cast<tname *>(m_pObject); }
class _KGC_PTableObject
{
public:
	_KGC_PTableObject(LPVOID p) : m_pObject(p) {}
	operator bool() { return m_pObject != NULL; }
	_KD_CHANGETO_PTYPE(KBaseTable)
	_KD_CHANGETO_PTYPE(KBranch)
	_KD_CHANGETO_PTYPE(KBranchDesc)
	_KD_CHANGETO_PTYPE(KIntBranchTable)
	_KD_CHANGETO_PTYPE(KStrBranchTable)
	_KD_CHANGETO_PTYPE(KMapIntIntTable)
	_KD_CHANGETO_PTYPE(KMapStrIntTable)
	_KD_CHANGETO_PTYPE(KMapIntBufTable)
	_KD_CHANGETO_PTYPE(KMapStrBufTable)
private:
	LPVOID m_pObject;
};

/************************************************************************************
*所有底层数据对象的申请和释放，ID管理
*所有底层数据对象ID都由Server端生成，然后同步到Client，以使Server和Client的ID保持一致
*所以使用KIntQueue，令接口形式操持一致
*KIntQueue在Server端ID是存入，Client端为读出
*************************************************************************************/
class KGC_DataPoolManager : public KBufPool
{
public:
	//初始化对象池
	BOOL InitObjectPool();
	template<typename KeyT, typename ValueT>
	inline _KGC_PTableObject NewTable(LPINT pnTabID);
	template<typename KeyT, typename ValueT>
	inline _KGC_PTableObject GetTable(INT nTabID);
	template<typename KeyT, typename ValueT>
	inline VOID DelTable(INT nTabID);
	BOOL ReportMemoryUse();
protected:
	//整形表与映射表
	KIncObjPool2<KMapIntInt> m_mpIntIntPool;
	KIncObjPool2<KMapIntData> m_mpIntDataPool;
	KIncObjPool2<KMapStrInt> m_mpStrIntPool;
	KIncObjPool2<KMapStrData> m_mpStrDataPool;
};

// -------------------------------------------------------------------------
extern KGC_DataPoolManager g_DataPoolManager;

//////////////////////////对象分配/////////////////////////////////////
/*#define KD_COMPLETE_TYPE_POOL(_keyt, _valuet, _object_pool) \
template<> \
inline _KGC_PTableObject KGC_DataPoolManager::NewTable<_keyt, _valuet>(LPINT TabID) \
{ return _object_pool.New(TabID); } \
template<> \
inline _KGC_PTableObject KGC_DataPoolManager::GetTable<_keyt, _valuet>(INT nTabID) \
{ return _object_pool.GetObject(nTabID); } \
template<> \
inline VOID KGC_DataPoolManager::DelTable<_keyt, _valuet>(INT nTabID) \
{ _object_pool.Free(nTabID); }

KD_COMPLETE_TYPE_POOL(INT, INT, m_mpIntIntPool)
KD_COMPLETE_TYPE_POOL(INT, DATA_DESC, m_mpIntDataPool)
KD_COMPLETE_TYPE_POOL(STR_TYPE, INT, m_mpStrIntPool)
KD_COMPLETE_TYPE_POOL(STR_TYPE, DATA_DESC, m_mpStrDataPool)	*/
//////////////////////////////////////////////////////////////////////////

#endif /* __KGC_DATAPOOLMANAGER_H__ */

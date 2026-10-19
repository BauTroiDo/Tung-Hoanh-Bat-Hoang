/* -------------------------------------------------------------------------
//	文件名		：	kgc_basedatadef.h
//	创建者		：	luobaohang
//	创建时间	：	2007-3-9 19:55:38
//	功能描述	：	
//
// -----------------------------------------------------------------------*/
#ifndef __KGC_BASEDATADEF_H__
#define __KGC_BASEDATADEF_H__
#include <map>
#include <string>
#include "kbufpool.h"
// -------------------------------------------------------------------------

class KGC_STDString : public std::string
{
	typedef std::string SuperT;
public:
	KGC_STDString(LPCSTR s) : SuperT(s) {}
	operator LPCSTR()CONST { return c_str(); }
};
typedef KGC_STDString STR_TYPE;
typedef LPSTR KTPBuf;
// -------------------------------------------------------------------------
//表的类型
enum KE_TABLE_TYPE
{
	emKTABTYPE_NONE,
	emKTABTYPE_INT_BRANCH,	//以INT为key的分支表
	emKTABTYPE_STR_BRANCH,	//以str为key的分支表
	emKTABTYPE_INT_INT,		//整型表
	emKTABTYPE_INT_BUF,		//buf型表
	emKTABTYPE_STR_INT,
	emKTABTYPE_STR_BUF,
};

//表的属性
enum KE_TABLE_FLAG
{
	//如果一个BranchTable不包含同步或保存属性,则其下的表均不能包含此属性
	//值为2的n次方
	emKTAB_FLAG_NONE = 0x0,
	emKTAB_FLAG_COMMON = 0x01,		//共有数据，GS启动连接到GC时会自动同步到GS，但数据改变时不会实时自动同步
	emKTAB_FLAG_GLOBALSAVE = 0x02,	//自动存取，Gamecenter启动或数据改变时都会自动存取，全局GC内存盘
	emKTAB_FLAG_NORMALSAVE = 0x04,	//自动存取，Gamecenter启动或数据改变时都会自动存取，普通GC内存盘
};

#define KD_AWAYS_SAVE_FLAG	 (emKTAB_FLAG_GLOBALSAVE | emKTAB_FLAG_NORMALSAVE)
#define KD_SYNC_SAVE_FLAG	(emKTAB_FLAG_COMMON | KD_AWAYS_SAVE_FLAG)

class KBranchDesc;
//表的描述结构
struct TABLE_DESC
{
	INT	m_eTabType;				//表的类型(KE_TABLE_TYPE)
	INT m_Flag;					//表的属性(第一位,是否同步,第二位,是否存储)
	//如果不是分支表则为空
	KBranchDesc	*m_pBranchDesc; //分支表的分支描述（分支表包含）
	CHAR szName[32];			//表名，将用于数据库中逻辑表名字段的值（永久不可改变）
};

struct KCmpStr : public std::binary_function<LPCSTR, LPCSTR, bool>
{
	bool operator()(LPCSTR psz1, LPCSTR psz2) CONST
	{ 
		return strcmp(psz1, psz2) < 0;
	}
};

struct KBaseTable
{
	TABLE_DESC *m_pDesc;	//描述表结构的指针
	INT m_nDBTabID;			//数据库中的tabid字段值
	INT	m_nDataVersion;		//数据的版本号
};

template <typename KeyT, typename ValueT, typename CmpT = std::less<KeyT> >
struct KMapTable : public KBaseTable
{
	std::map<KeyT, ValueT, CmpT> m_mpData;
	VOID KOP_ReUse() { m_pDesc = 0; m_nDBTabID = 0; m_nDataVersion = 0; }
	VOID KOP_UnUse() { m_mpData.clear(); }
};
//特化下面两函数使用使用对象池时可调用重用和释放函数
template<typename KeyT, typename ValueT, typename CmpT> inline
VOID KOP_ReUse(KMapTable<KeyT, ValueT, CmpT> &widget) { widget.KOP_ReUse(); }
template<typename KeyT, typename ValueT, typename CmpT> inline
VOID KOP_UnUse(KMapTable<KeyT, ValueT, CmpT> &widget) { widget.KOP_UnUse(); }

//////////////////////////基本的底层数据对象类/////////////////////////////
struct KMapIntInt : public KMapTable<INT, INT> {};

template<> inline
VOID KOP_ReUse(KMapIntInt &widget) { widget.KOP_ReUse(); }
template<> inline
VOID KOP_UnUse(KMapIntInt &widget) { widget.KOP_UnUse(); }

struct KMapIntData : public KMapTable<INT, DATA_DESC> {};
template<> inline
VOID KOP_ReUse(KMapIntData &widget) { widget.KOP_ReUse(); }
template<> inline
VOID KOP_UnUse(KMapIntData &widget) { widget.KOP_UnUse(); }

struct KMapStrInt : public KMapTable<STR_TYPE, INT> {};
template<> inline
VOID KOP_ReUse(KMapStrInt &widget) { widget.KOP_ReUse(); }
template<> inline
VOID KOP_UnUse(KMapStrInt &widget) { widget.KOP_UnUse(); }

struct KMapStrData : public KMapTable<STR_TYPE, DATA_DESC> {};
template<> inline
VOID KOP_ReUse(KMapStrData &widget) { widget.KOP_ReUse(); }
template<> inline
VOID KOP_UnUse(KMapStrData &widget) { widget.KOP_UnUse(); }


struct KTableDef 
{
	INT nKey;				//Table的Key，识别是分支内哪一个Table，永久不可改变
	KE_TABLE_TYPE eType;	//表的类型
	INT Flag;				//表的同步、存储等标志
	LPCSTR szDBName;		//表在数据库的表名，永久不可改变
};
// -------------------------------------------------------------------------

#endif /* __KGC_BASEDATADEF_H__ */

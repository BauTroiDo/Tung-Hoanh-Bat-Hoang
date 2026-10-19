/* -------------------------------------------------------------------------
//	文件名		：	kgc_dbmanager.h
//	创建者		：	luobaohang
//	创建时间	：	2007-3-5 20:59:29
//	功能描述	：	
//
// -----------------------------------------------------------------------*/
#ifndef __KGC_DBMANAGER_H__
#define __KGC_DBMANAGER_H__
#include <queue>
#include "kbufpool.h"
#include "kgc_basedatadef.h"
#include "kgc_mysql_database.h"
#ifdef KG_GC_SERVER

enum KE_OPER_TYPE
{
	emKOPER_ADD_DATA,	//增加一条数据
	emKOPER_DEL_DATA,	//删除一条数据
	emKOPER_MODIFY_DATA, //修改数据
	emKOPER_DEL_TABLE,	//删除一个表（逻辑表，非数据库表）
};
enum KE_COZONE_STEP
{
	emKCOZONE_STEP_NONE,
	emKCOZONE_STEP_INIT,
	emKCOZONE_STEP_DBSUB,
	emKCOZONE_STEP_DBSUBCLEAR,
	emKCOZONE_STEP_DBMAIN,
	emKCOZONE_STEP_COZONELOAD,
	emKCOZONE_STEP_COZONELOADED,
};
//关系数据表存盘结构
struct KSaveData
{
	KE_OPER_TYPE	eOpt;		//操作类型
	INT nDBTableID;
	INT nDataVer;
	DATA_DESC key;
	LPVOID pKeyBuf;
	DATA_DESC data;
	LPVOID pDataBuf;
	LPCSTR szTableName;	//表名
};


// -------------------------------------------------------------------------
class KBranch;
class KBranchTable;
class IKDBThreadCallback;
class IKMainThreadCallback;
class KGC_DBManager
{
	struct KDB_CALLBACK
	{
		IKDBThreadCallback*		pCallBack;
		LPVOID					pParam;
		INT						nParam;
	};
	struct KMAIN_CALLBACK
	{
		IKMainThreadCallback*	pCallBack;
		LPVOID					pParam;
		INT						nParam;
	};
	//用Queue，保证先push的先处理
	typedef std::queue<KSaveData>		QUE_SAVE_FIELD;
	typedef std::deque<KDB_CALLBACK>	QUE_DBTHREAD_CB;
	typedef std::queue<KMAIN_CALLBACK>	QUE_MAINTHREAD_CB;
public:
	KGC_DBManager();
	//涉及与数据库线程的同步操作使用同一线程锁，不要另外定义锁
	//一般在IKDBThreadCallback中使用，但尽量避免使用
	BOOL	DBThreadLock() { return m_ThreadLock.Lock(); }
	BOOL	DBThreadUnlock() { return m_ThreadLock.Unlock(); }
	BOOL	InitDBManager();
	BOOL	UnInit();
	//数据库线程启动
	BOOL	StratUp();
	BOOL	PushSave(KBaseTable *pTable, INT nKey, LPCVOID pData, INT nSize, KE_OPER_TYPE eOpt);
	BOOL	PushSave(KBaseTable *pTable, INT nKey, DATA_DESC data, KE_OPER_TYPE eOpt);
	BOOL	PushSave(KBaseTable *pTable, INT nKey, INT nValue, KE_OPER_TYPE eOpt);
	BOOL	PushSave(KBaseTable *pTable, LPCSTR szKey, LPCVOID pData, INT nSize, KE_OPER_TYPE eOpt);
	BOOL	PushSave(KBaseTable *pTable, LPCSTR szKey, DATA_DESC data, KE_OPER_TYPE eOpt);
	BOOL	PushSave(KBaseTable *pTable, LPCSTR szKey, INT nValue, KE_OPER_TYPE eOpt);
	BOOL	CreateRDSTable(TABLE_DESC* pTabDesc);	//创建通用关系数据库表
	BOOL	LoadTableDataFromDB(KBaseTable *pTable, KE_TABLE_TYPE eTabType);
	BOOL	GetSaveQuit() { return m_bSafeQuit; }
	//加入数据库事务
	BOOL	PushDBAffair(IKDBThreadCallback *pAffair, INT nParam = 0, LPVOID pParam = NULL, BOOL bPriority = FALSE);
	//数据库事务的主线程结果回调
	BOOL	PushMainThreadCallBack(IKMainThreadCallback *pCallback, INT nParam = 0, LPVOID pParam = NULL);
	BOOL	MainThreadActive();	//主线程Active
	//允许外部自执行数据库存取查询事务（通过IKDBThreadCallback）
	KGC_MySqlDatabase &GetDatabase() { return m_Database; }
	INT		GetDbAffairCounter() { return m_nDbAffairCounter; }
	INT		GetSaveAffairCounter() { return m_nSaveAffairCounter; }
	DWORD	GetSaveCheckFlag() { return m_dwSaveCheckFlag; }
protected:
	BOOL	GetRuningState()	{ return m_bRuningState; }
	VOID	ReSetRuningState()	{ while (m_bRuningState) m_bRuningState = FALSE; }
	BOOL	LoadDBShareData();
	BOOL	PushSave(KSaveData& cSaveData);
private:
	BOOL AddItemToTable(KBaseTable *pTable, KE_TABLE_TYPE eTabType,CHAR *pItemKey, INT nKeySize, CHAR *pItemData, INT nDataSize);
	BOOL _GetBranchDataFromDB(KBranch *pBranchData, LPVOID pData, INT nDataSize, LPINT &pIBuf);
	//Called by Thread (_DBRoute)
	VOID	DBRoute();
	//Called by Thread (_DBRoute)
	BOOL	SaveLoop();
	BOOL	Save(KSaveData& SaveData);
	BOOL	AffairLoop();
	//存取线程
	static VOID _DBRoute(LPVOID pvParam);
protected:
	BOOL	m_bRuningState;
	BOOL	m_bSafeQuit;	//是否能安全退出（数据已存盘完毕）
	BOOL	m_bStratup;
	KThread	m_SaveThread;
	QUE_SAVE_FIELD	m_queSaveData;	//准备保存的队列
	QUE_MAINTHREAD_CB m_queMainThreadCallback; //主线程事务回调队列
	QUE_DBTHREAD_CB	m_queDBThreadCallback; //数据库线线程事务队列
	KBufPool m_DBBufPool;
	KMutex	m_ThreadLock;	//两线程间尽量不要用多个锁，否则容易互锁
	BOOL m_bInited;
	KGC_MySqlDatabase m_Database;
	INT m_nDbAffairCounter;
	INT m_nSaveAffairCounter;
	DWORD m_dwSaveCheckFlag;		// 数据表是否应存盘检查的标志位 全局为 emKTAB_FLAG_GLOBALSAVE, 非全局为 emKTAB_FLAG_NORMALSAVE
public:
	BOOL CoZoneInit(LPCSTR szGcDbMain, LPCSTR szGoddessDbMain, LPCSTR szGcDbSub, LPCSTR szGoddessDbSub, BOOL bDelTempDb);
	BOOL CoZoneUnInit();
	BOOL CoZoneStepClear();
	BOOL CoZoneStepLoadDbMain();
	BOOL CoZoneStepLoadDbSub();
	BOOL CoZoneStepLoadDbSubEnd();

	INT GetCoZoneStep() {return m_eCoZoneStep; };

	LPCSTR GetCoZoneMainGcDBName() { return m_szCoZoneDbGcMain; }
	LPCSTR GetCoZoneSubGcDBName() { return m_szCoZoneDbGcSub; }
protected:
	KE_COZONE_STEP m_eCoZoneStep;
	CHAR m_szCoZoneDbGcMain[64];
	CHAR m_szCoZoneDbGoddessMain[64];
	CHAR m_szCoZoneDbGcSub[64];
	CHAR m_szCoZoneDbGoddessSub[64];
	BOOL m_bDelTempDB;
};
extern KGC_DBManager g_DataDBManager;
//当前的数据版本，每启动Gamecenter一次数据版本加1
//数据版本ID的作用是与内存的TableID一起唯一标识一个新增的表ID
extern INT g_nCurDataVersion;
// -------------------------------------------------------------------------
#endif //#ifdef KG_GC_SERVER
#endif /* __KGC_DBMANAGER_H__ */

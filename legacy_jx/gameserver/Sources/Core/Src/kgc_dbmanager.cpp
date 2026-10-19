/* -------------------------------------------------------------------------
//	文件名		：	kgc_dbmanager.cpp
//	创建者		：	luobaoahang
//	创建时间	：	2007-3-6 11:02:58
//	功能描述	：	
//
// -----------------------------------------------------------------------*/

//#include "stdafx.h"
#include "KCore.h"

#include "mysql/mysql.h"
#include "utilities.h"
#include "kgc_config.h"
#include "kgc_datapoolmanager.h"
#include "kgc_basedataobject.h"
#include "kgc_dbdef.h"
#include "kgc_dbcallback.h"
#include "kgc_dbmanager.h"
#include "kgc_control.h"


#ifdef KG_GC_SERVER
#define  KD_MAX_QUERY_FIELD	10
// -------------------------------------------------------------------------
KGC_DBManager g_DataDBManager;

//注意KD_DBFIELD_GROUP虽然增加纪录时有加这个字段，但操作数据时是不判断Group字段的，
//也就是说这个字段是备用的
//注意多线程冲突问题
//Key和Data字段类型将在创建表及数据库操作时根据Key和Data实际类型修正
static INT s_TabIDData[2];
static KDBFieldDesc s_CommonDataTableFieldDesc[] = 
{
	{ KD_DBFIELD_TABID, emK_FIELDTYPE_TINYBLOB, emK_FIELD_FLAG_NORMAL, sizeof(INT)*2, s_TabIDData, sizeof(INT)*2}, 
	{ KD_DBFIELD_ITEMKEY, emK_FIELDTYPE_TINYBLOB, emK_FIELD_FLAG_NORMAL, KD_ITEMKEY_MAX_SIZE }, 
	{ KD_DBFIELD_DATA, emK_FIELDTYPE_BLOB, emK_FIELD_FLAG_NORMAL, KD_MEDIUM_FIELD_MAX_DATA_SIZE }, 
	{ KD_DBFIELD_GROUP, emK_FIELDTYPE_VARCHAR, emK_FIELD_FLAG_NORMAL, KD_DBNAME_MAX_SIZE, (LPVOID)KD_DATA_GROUP, sizeof(KD_DATA_GROUP)},
};
static KDBFieldDesc	&s_TabIDField = s_CommonDataTableFieldDesc[0];
static KDBFieldDesc	&s_KeyField = s_CommonDataTableFieldDesc[1];
static KDBFieldDesc	&s_DataField = s_CommonDataTableFieldDesc[2];
static KDBFieldDesc	&s_GroupField = s_CommonDataTableFieldDesc[3];
CONST static INT csCommonTableFieldNum  = sizeof(s_CommonDataTableFieldDesc) / sizeof(s_CommonDataTableFieldDesc[0]);

KDBFieldDesc s_ShareTableFieldDesc[] = 
{
	{ KD_DBFIELD_DATAVER, emK_FIELDTYPE_TINYBLOB, emK_FIELD_FLAG_NOTNULL, sizeof(INT)}, 
};
CONST static INT csShareTableFieldNum  = sizeof(s_ShareTableFieldDesc) / sizeof(s_ShareTableFieldDesc[0]);
// -------------------------------------------------------------------------

KGC_DBManager::KGC_DBManager()
{
	m_bRuningState = m_bInited = m_bStratup = FALSE; 
	m_nDbAffairCounter = m_nSaveAffairCounter = 0;
	m_bSafeQuit = TRUE;
	m_eCoZoneStep = emKCOZONE_STEP_NONE;
	m_dwSaveCheckFlag = 0;
}

BOOL KGC_DBManager::InitDBManager() 
{
	KGC_CONFIRM_RET_FALSE(!m_bInited);
	IKGC_Config *piConfig = GetGCConfig();
	m_bInited = TRUE;
	m_bRuningState = TRUE;
	m_bSafeQuit = TRUE;
	m_dwSaveCheckFlag = piConfig->GetGlobalAgentMode() == emKGAMECENTER_SERVER_MODE ? emKTAB_FLAG_GLOBALSAVE : emKTAB_FLAG_NORMALSAVE;
	m_DBBufPool.InitBufPool();
	CONST DB_SERVER_PARAM &ServerParam = piConfig->DBServerParam();
	 // ----------------- connect to database  ----------------- 
	INT nRetCode = m_Database.DBConnect(
		ServerParam.szDBIP,
		ServerParam.nDBPort,
		ServerParam.szUserName,
		ServerParam.szPassword,
		NULL);
	KGC_CONFIRM_RET_FALSE(nRetCode);
	LPCSTR szCurDatabase = ServerParam.szDatabase;
	if (emKCOZONE_STEP_INIT == m_eCoZoneStep)
	{
		szCurDatabase = m_szCoZoneDbGcSub;
		m_eCoZoneStep = emKCOZONE_STEP_DBSUB;
	}
	else
	{
		nRetCode = m_Database.CreateDatabase(ServerParam.szDatabase, emKDBCREATE_IF_NOT_EXIST);
		KGC_CONFIRM_RET_FALSE(nRetCode);
	}
	KGC_CONFIRM_RET_FALSE(szCurDatabase);
	nRetCode = m_Database.SetCurrentDatabase(szCurDatabase);
	KGC_CONFIRM_RET_FALSE(nRetCode);
	KG_Printl("Create DBShare Table");
	CHAR sqlBuf[2048];
	KGC_MySqlQueryStream sqlStream = m_Database.CreateQueryStream(sqlBuf);
#if defined _DEBUG && defined WIN32
	//KG_Printl("WAINING______________________ Now Drop DBShare Table");
	////用于调试时清数据（不用时注掉）
	//sqlStream.AddRemoveTable(KD_DBTBL_SHARE);
	//m_Database.DoQuery(sqlStream);
	//sqlStream.Reset();
#endif
	KDBTableDesc shareTableDesc = {
		KD_DBTBL_SHARE, csShareTableFieldNum, s_ShareTableFieldDesc
	};
	sqlStream.AddCreateTable(emKTABLE_CREATE_IF_NOT_EXIST, shareTableDesc);
	nRetCode = m_Database.DoQuery(sqlStream);
	KGC_CONFIRM_RET_FALSE(nRetCode);
	KG_Printl("LoadDBShare...");
	if (!LoadDBShareData())
	{
		KG_Printl("!!!!!!!!!!!!!!!!!\nLoad DB Share Data Failed!!\n");
		return FALSE;
	}
	return TRUE;
}

BOOL KGC_DBManager::StratUp()
{
	m_bStratup = TRUE;
	if (emKCOZONE_STEP_NONE == m_eCoZoneStep)
	{
		m_SaveThread.Create(_DBRoute, this);
		m_bSafeQuit = FALSE;
	}
	else
	{
		m_bSafeQuit = TRUE;
	}
	return TRUE;
}

BOOL KGC_DBManager::PushSave( KBaseTable *pTable, INT nKey, LPCVOID pData, INT nSize, KE_OPER_TYPE eOpt ) 
{
	if (emKCOZONE_STEP_DBSUBCLEAR == m_eCoZoneStep)
		return TRUE;
	if(!pData || nSize <= 0)
	{
		ASSERT(FALSE);
		return PushSave(pTable, nKey, 0, eOpt);
	}
	KSaveData SaveData;
	SaveData.eOpt = eOpt;
	SaveData.nDBTableID = pTable->m_nDBTabID;
	SaveData.nDataVer = pTable->m_nDataVersion;
	SaveData.szTableName = pTable->m_pDesc->szName;
	SaveData.key.m_BufType = BUF_0;
	SaveData.key.m_nBufIndex = nKey;
	SaveData.data.m_BufType = GetPropReserveBufSize(nSize);
	SaveData.data.m_DataSize = nSize;
	m_ThreadLock.Lock();
	SaveData.pDataBuf = m_DBBufPool.NewBufData(SaveData.data.m_BufType, &SaveData.data.m_nBufIndex);
	m_ThreadLock.Unlock();
	KGC_CONFIRM_RET_FALSE(SaveData.pDataBuf);
	memcpy(SaveData.pDataBuf, pData, nSize);
	PushSave(SaveData);
	return TRUE;
}

BOOL KGC_DBManager::PushSave( KBaseTable *pTable, INT nKey, DATA_DESC data, KE_OPER_TYPE eOpt ) 
{
	if (emKCOZONE_STEP_DBSUBCLEAR == m_eCoZoneStep)
		return TRUE;
	KSaveData SaveData;
	SaveData.eOpt = eOpt;
	SaveData.nDBTableID = pTable->m_nDBTabID;
	SaveData.nDataVer = pTable->m_nDataVersion;
	SaveData.szTableName = pTable->m_pDesc->szName;
	SaveData.key.m_BufType = BUF_0;
	SaveData.key.m_nBufIndex = nKey;
	SaveData.data.m_BufType = data.m_BufType;
	SaveData.data.m_DataSize = data.m_DataSize;
	m_ThreadLock.Lock();
	SaveData.pDataBuf = m_DBBufPool.NewBufData(SaveData.data.m_BufType, &SaveData.data.m_nBufIndex);
	m_ThreadLock.Unlock();
	CHAR *pData = m_DBBufPool.GetBufData(data);
	KGC_CONFIRM_RET_FALSE(SaveData.pDataBuf && pData);
	memcpy(SaveData.pDataBuf, pData, data.m_DataSize);
	PushSave(SaveData);
	return TRUE;
}

BOOL KGC_DBManager::PushSave( KBaseTable *pTable, INT nKey, INT nValue, KE_OPER_TYPE eOpt ) 
{
	if (emKCOZONE_STEP_DBSUBCLEAR == m_eCoZoneStep)
		return TRUE;
	KSaveData SaveData;
	SaveData.eOpt = eOpt;
	SaveData.nDBTableID = pTable->m_nDBTabID;
	SaveData.nDataVer = pTable->m_nDataVersion;
	SaveData.szTableName = pTable->m_pDesc->szName;
	SaveData.key.m_BufType = BUF_0;
	SaveData.key.m_nBufIndex = nKey;
	SaveData.data.m_BufType = BUF_0;
	SaveData.data.m_nBufIndex = nValue;
	PushSave(SaveData);
	return TRUE;
}

BOOL KGC_DBManager::PushSave( KBaseTable *pTable, LPCSTR szKey, LPCVOID pData, INT nSize, KE_OPER_TYPE eOpt ) 
{
	if (emKCOZONE_STEP_DBSUBCLEAR == m_eCoZoneStep)
		return TRUE;
	if(!pData || nSize <=0)
	{
		ASSERT(FALSE);
		return PushSave(pTable, szKey, 0, eOpt);
	}
	INT nKeyLen = (INT)strlen(szKey) + 1;
	KSaveData SaveData;
	SaveData.eOpt = eOpt;
	SaveData.nDBTableID = pTable->m_nDBTabID;
	SaveData.nDataVer = pTable->m_nDataVersion;
	SaveData.szTableName = pTable->m_pDesc->szName;
	SaveData.key.m_BufType = GetPropReserveBufSize(nKeyLen);
	SaveData.key.m_DataSize = nKeyLen;
	SaveData.data.m_BufType = GetPropReserveBufSize(nSize);
	SaveData.data.m_DataSize = nSize;
	m_ThreadLock.Lock();
	SaveData.pKeyBuf = m_DBBufPool.NewBufData(SaveData.key.m_BufType, &SaveData.key.m_nBufIndex);
	SaveData.pDataBuf = m_DBBufPool.NewBufData(SaveData.data.m_BufType, &SaveData.data.m_nBufIndex);
	m_ThreadLock.Unlock();
	KGC_CONFIRM_RET_FALSE(SaveData.pKeyBuf)
	KGC_CONFIRM_RET_FALSE(SaveData.pDataBuf)
	memcpy(SaveData.pKeyBuf, szKey, nKeyLen);
	memcpy(SaveData.pDataBuf, pData, nSize);
	PushSave(SaveData);
	return TRUE;
}

BOOL KGC_DBManager::PushSave( KBaseTable *pTable, LPCSTR szKey, DATA_DESC data, KE_OPER_TYPE eOpt ) 
{
	if (emKCOZONE_STEP_DBSUBCLEAR == m_eCoZoneStep)
		return TRUE;
	INT nKeyLen = (INT)strlen(szKey) + 1;
	KSaveData SaveData;
	SaveData.eOpt = eOpt;
	SaveData.nDBTableID = pTable->m_nDBTabID;
	SaveData.nDataVer = pTable->m_nDataVersion;
	SaveData.szTableName = pTable->m_pDesc->szName;
	SaveData.key.m_BufType = GetPropReserveBufSize(nKeyLen);
	SaveData.key.m_DataSize = nKeyLen;
	SaveData.data.m_BufType = data.m_BufType;
	SaveData.data.m_DataSize = data.m_DataSize;
	m_ThreadLock.Lock();
	SaveData.pKeyBuf= m_DBBufPool.NewBufData(SaveData.key.m_BufType, &SaveData.key.m_nBufIndex);
	SaveData.pDataBuf= m_DBBufPool.NewBufData(SaveData.data.m_BufType, &SaveData.data.m_nBufIndex);
	CHAR *pData = m_DBBufPool.GetBufData(data);
	m_ThreadLock.Unlock();
	KGC_CONFIRM_RET_FALSE(SaveData.pKeyBuf)
	KGC_CONFIRM_RET_FALSE(SaveData.pDataBuf && pData);
	memcpy(SaveData.pKeyBuf, szKey, nKeyLen);
	memcpy(SaveData.pDataBuf, pData, data.m_DataSize);
	PushSave(SaveData);
	return TRUE;
}

BOOL KGC_DBManager::PushSave( KBaseTable *pTable, LPCSTR szKey, INT nValue, KE_OPER_TYPE eOpt ) 
{
	if (emKCOZONE_STEP_DBSUBCLEAR == m_eCoZoneStep)
		return TRUE;
	INT nKeyLen = (INT)strlen(szKey) + 1;
	KSaveData SaveData;
	SaveData.eOpt = eOpt;
	SaveData.nDBTableID = pTable->m_nDBTabID;
	SaveData.nDataVer = pTable->m_nDataVersion;
	SaveData.szTableName = pTable->m_pDesc->szName;
	SaveData.key.m_BufType = GetPropReserveBufSize(nKeyLen);
	SaveData.key.m_DataSize = nKeyLen;
	m_ThreadLock.Lock();
	SaveData.pKeyBuf= m_DBBufPool.NewBufData(SaveData.key.m_BufType, &SaveData.key.m_nBufIndex);
	m_ThreadLock.Unlock();
	KGC_CONFIRM_RET_FALSE(SaveData.pKeyBuf);
	memcpy(SaveData.pKeyBuf, szKey, nKeyLen);
	SaveData.data.m_BufType = BUF_0;
	SaveData.data.m_nBufIndex = nValue;
	PushSave(SaveData);
	return TRUE;
}

BOOL KGC_DBManager::PushSave( KSaveData& cSaveData ) 
{
	if (emKCOZONE_STEP_DBSUBCLEAR == m_eCoZoneStep)
		return TRUE;
	if (m_bSafeQuit || !m_bStratup || emKCOZONE_STEP_NONE != m_eCoZoneStep)	// 如果数据库线程已退出或未启动
	{
		return Save(cSaveData);
	}
	++m_nSaveAffairCounter;
	m_ThreadLock.Lock();
	m_queSaveData.push(cSaveData);
	m_ThreadLock.Unlock();
	return TRUE;
}

BOOL KGC_DBManager::LoadDBShareData() 
{
	BOOL bRet;
	CHAR szQueryBuf[2048];
	KGC_MySqlQueryStream queryStream =  m_Database.CreateQueryStream(szQueryBuf);
	queryStream.AddSelectQuery(1, s_ShareTableFieldDesc, KD_DBTBL_SHARE);
	bRet = m_Database.DoQuery(queryStream);
	KGC_CONFIRM_RET_FALSE(bRet);
	KGC_MySQLResult rs = m_Database.GetQueryResult();
	KGC_CONFIRM_RET_FALSE(rs.IsValid());
	if(rs.GetFieldCount() != 1)
	{
		ASSERT(FALSE);
		rs.Release();
		return FALSE;
	}
	INT nItemCount = rs.GetRowCount();
	if (nItemCount > 1)
	{
		ASSERT(FALSE);	//应该只有一项
	}
	if (nItemCount > 0)	//表不是空的
	{
		KGC_MySQLRow curRow = rs.GetNextRow();
		if (curRow.IsValid())
		{
			//每次启动DataVersion加1
			g_nCurDataVersion = (*(LPINT)curRow.aFieldData[0]) + 1;
		}
		else
		{
			ASSERT(FALSE);
		}
	}
	else
	{
		//以前没存过则为1
		g_nCurDataVersion = 1;
		queryStream.Reset();
		s_ShareTableFieldDesc[0].pvData = &g_nCurDataVersion;
		s_ShareTableFieldDesc[0].uDataSize = sizeof(g_nCurDataVersion);
		queryStream.AddInsertRecord(1, s_ShareTableFieldDesc, KD_DBTBL_SHARE);
		bRet = m_Database.DoQuery(queryStream);
		if(!bRet)
		{
			rs.Release();
			ASSERT(FALSE);
			return FALSE;
		}
	}
	rs.Release();
	KG_Printl("Current Data Version : " << g_nCurDataVersion);
	//g_nCurDataVersion值加1后再存入
	queryStream.Reset();
	s_ShareTableFieldDesc[0].pvData = &g_nCurDataVersion;
	s_ShareTableFieldDesc[0].uDataSize = sizeof(g_nCurDataVersion);
	queryStream.AddUpdateRecord(1, s_ShareTableFieldDesc, KD_DBTBL_SHARE);
	bRet = m_Database.DoQuery(queryStream);
	KGC_CONFIRM_RET_FALSE(bRet);
	return TRUE;
}
BOOL KGC_DBManager::SaveLoop()
{
	m_ThreadLock.Lock();
	//没有待存数据，返回
	if (m_queSaveData.empty())
	{
		m_ThreadLock.Unlock();
		return FALSE;
	};
	KSaveData SaveData = m_queSaveData.front();
	m_queSaveData.pop();
	m_ThreadLock.Unlock();
	return Save(SaveData);
}

BOOL KGC_DBManager::Save(KSaveData& SaveData)
{
	BOOL bRet = FALSE;
	//数据库操作时要等待网络数据库回应，因此先解Lock，防止主线程被卡较长时间
	KE_OPER_TYPE eOpt = SaveData.eOpt;

	//KD_MEDIUM_FIELD_MAX_DATA_SIZE * 2是因为Data可能作mysql_real_escape_string
	//use about 15K stack size
	CHAR sqlBuf[KD_MEDIUM_FIELD_MAX_DATA_SIZE * 2 + 2048];
	KGC_MySqlQueryStream sqlStream = m_Database.CreateQueryStream(sqlBuf);
	//数据据库中的表ID由DBTableID（表第一次生成时的内存ID）和数据版本ID结合组成
	s_TabIDData[0] = SaveData.nDBTableID;
	s_TabIDData[1] = SaveData.nDataVer;
	//Key
	if (SaveData.key.m_BufType == BUF_0)
	{
		s_KeyField.uDataSize = sizeof(INT);
		s_KeyField.eType = emK_FIELDTYPE_TINYBLOB;
		s_KeyField.pvData = &SaveData.key.m_nBufIndex;
		s_KeyField.uMaxSize = sizeof(INT);
	}
	else
	{
		if (SaveData.key.m_DataSize > KD_ITEMKEY_MAX_SIZE)
		{
			ASSERT(FALSE);
			goto KExit;
		}
		s_KeyField.uDataSize = SaveData.key.m_DataSize;
		s_KeyField.eType = emK_FIELDTYPE_VARCHAR;
		s_KeyField.pvData = SaveData.pKeyBuf;
		s_KeyField.uMaxSize = KD_ITEMKEY_MAX_SIZE;
	}
	//Data
	if (SaveData.data.m_BufType == BUF_0)
	{
		s_DataField.uDataSize = sizeof(INT);
		s_DataField.eType = emK_FIELDTYPE_TINYBLOB;
		s_DataField.pvData = &SaveData.data.m_nBufIndex;
		s_DataField.uMaxSize = sizeof(INT);
	}
	else
	{
		unsigned uSaveDataSize = SaveData.data.m_DataSize;
		if (uSaveDataSize > KD_MEDIUM_FIELD_MAX_DATA_SIZE)
		{
			ASSERT(FALSE);
			goto KExit;
		}
		s_DataField.uDataSize = uSaveDataSize;
		s_DataField.eType = emK_FIELDTYPE_BLOB;
		s_DataField.pvData = SaveData.pDataBuf;
		s_DataField.uMaxSize = KD_MEDIUM_FIELD_MAX_DATA_SIZE;
	}
	switch(eOpt)
	{
	case emKOPER_ADD_DATA:
		{
			sqlStream.AddInsertRecord(csCommonTableFieldNum, s_CommonDataTableFieldDesc, SaveData.szTableName);	
		}
		break;
	case emKOPER_DEL_DATA:
		{
			sqlStream.AddDeleteRecord(SaveData.szTableName);
			sqlStream.AddFieldCondition(emK_DBSQL_WHERE, emKOPERATOR_EQUAL, s_TabIDField);
			sqlStream.AddFieldCondition(emK_DBSQL_AND, emKOPERATOR_EQUAL, s_KeyField);
		}
		break;
	case emKOPER_MODIFY_DATA:
		{
			sqlStream.AddUpdateRecord(1, &s_DataField, SaveData.szTableName);
			sqlStream.AddFieldCondition(emK_DBSQL_WHERE, emKOPERATOR_EQUAL, s_TabIDField);
			sqlStream.AddFieldCondition(emK_DBSQL_AND, emKOPERATOR_EQUAL, s_KeyField);
		}
		break;
	case emKOPER_DEL_TABLE:
		{
			sqlStream.AddDeleteRecord(SaveData.szTableName);
			sqlStream.AddFieldCondition(emK_DBSQL_WHERE, emKOPERATOR_EQUAL, s_TabIDField);
		}
		break;
	default:
		KGC_Error("Invalid Opt Type");
		goto KExit;
	}
	bRet = m_Database.DoQuery(sqlStream);
	if(!bRet)
		KGC_Error("Query Error");
KExit:
	//因为要操作m_DBBufPool，所以要Lock
	m_ThreadLock.Lock();
	if (SaveData.key.m_BufType != BUF_0)
		m_DBBufPool.FreeBufData(SaveData.key);
	if (SaveData.data.m_BufType != BUF_0)
		m_DBBufPool.FreeBufData(SaveData.data);
	m_ThreadLock.Unlock();
	return TRUE;
}

BOOL KGC_DBManager::AffairLoop() 
{
extern UINT GCGetCurElapse();
	m_ThreadLock.Lock();
	//注意return前要Unlock
	if (!m_queDBThreadCallback.empty())
	{
		//每128个任务的界点输出调试信息
		if ((m_queDBThreadCallback.size() & 127) == 0)
			KG_Printl("DB Affair Count: " << m_queDBThreadCallback.size() << " at " << GCGetCurElapse());
		KDB_CALLBACK sCallBack = m_queDBThreadCallback.front();
		IKDBThreadCallback *pCallback = sCallBack.pCallBack;
		m_queDBThreadCallback.pop_front();
		m_ThreadLock.Unlock();	//先Unlock再执行Callback是肯定的
		pCallback->DBThreadCallback(sCallBack.nParam, sCallBack.pParam);
		pCallback->Release();
		return TRUE;
	}
	m_ThreadLock.Unlock();
	return FALSE;
}

VOID KGC_DBManager::DBRoute() 
{
	while (GetRuningState())
	{
		if (!(SaveLoop() | AffairLoop()))
			KGThread_Sleep(1);
	}
	//线程结束，继续完成存取操作
	while (SaveLoop() || AffairLoop())
	{
		continue;
	}
	//安全退出
	do 
	{
		m_bSafeQuit = TRUE;
	}while(!m_bSafeQuit);
}

VOID KGC_DBManager::_DBRoute( LPVOID pvParam ) 
{
	reinterpret_cast<KGC_DBManager *>(pvParam)->DBRoute();
}

BOOL KGC_DBManager::PushDBAffair(IKDBThreadCallback *pAffair, INT nParam, LPVOID pParam, BOOL bPriority) 
{
	++m_nDbAffairCounter;
	if (m_bSafeQuit || !m_bStratup || emKCOZONE_STEP_NONE != m_eCoZoneStep)	// 如果数据库线程已退出
	{
		pAffair->DBThreadCallback(nParam, pParam);
		pAffair->Release();
		return TRUE;
	}
	KDB_CALLBACK sCallBack;
	sCallBack.pCallBack = pAffair;
	sCallBack.nParam = nParam;
	sCallBack.pParam = pParam;
	m_ThreadLock.Lock();
	if (bPriority)
		m_queDBThreadCallback.push_front(sCallBack);
	else
		m_queDBThreadCallback.push_back(sCallBack);
	m_ThreadLock.Unlock();
	return TRUE;
}

BOOL KGC_DBManager::PushMainThreadCallBack(IKMainThreadCallback *pCallback, INT nParam, LPVOID pParam)
{
	// 若UnInit已调用，直接调
	if (!m_bInited)
	{
		pCallback->MainThreadCallback(nParam, pParam);
		pCallback->Release();
		return TRUE;
	}
	m_ThreadLock.Lock();
	KMAIN_CALLBACK sCallBack;
	sCallBack.pCallBack = pCallback;
	sCallBack.nParam = nParam;
	sCallBack.pParam = pParam;
	m_queMainThreadCallback.push(sCallBack);
	m_ThreadLock.Unlock();
	return TRUE;
}

BOOL KGC_DBManager::UnInit() 
{
	if (!m_bInited)
		return TRUE;
	ReSetRuningState();

	// 要等待线程退出
	while (!m_bSafeQuit)
	{
		if (!MainThreadActive())
			KGThread_Sleep(5);
	}
	m_ThreadLock.Lock();
	// 完成callback队列
	while (!m_queMainThreadCallback.empty())
	{
		KMAIN_CALLBACK sCallBack = m_queMainThreadCallback.front();
		m_queMainThreadCallback.pop();
		m_ThreadLock.Unlock();
		IKMainThreadCallback *piCallback = sCallBack.pCallBack;
		piCallback->MainThreadCallback(sCallBack.nParam, sCallBack.pParam);
		piCallback->Release();
		m_ThreadLock.Lock();
	}
	m_ThreadLock.Unlock();
	m_bInited = FALSE;
	//	m_Database.ShutDown(); UnInit后仍有可能存盘
	KG_Printl("Save Thread Exit...!");
	return TRUE;
}

//创建通用关系数据库表
BOOL KGC_DBManager::CreateRDSTable( TABLE_DESC* pTabDesc ) 
{
	if (!(pTabDesc->m_Flag & GetSaveCheckFlag()))
		return TRUE;
	KG_Printl("Create RDS Table: " << pTabDesc->szName);
	CHAR sqlBuf[2048];
	KGC_MySqlQueryStream sqlStream = m_Database.CreateQueryStream(sqlBuf);
#if defined _DEBUG && defined WIN32
	//KG_Printl("WAINING______________________ Now Drop Data Table " << pTabDesc->szName);
	////用于调试时清数据（不用时注掉）
	//sqlStream.AddRemoveTable(pTabDesc->szName);
	//m_Database.DoQuery(sqlStream);
	//sqlStream.Reset();
#endif
	KDBTableDesc kCommonDataTableDesc = {
		pTabDesc->szName, 
		csCommonTableFieldNum, 
		s_CommonDataTableFieldDesc
	};
	//校正Key和Data字段类型大小
	switch(pTabDesc->m_eTabType)
	{
	case emKTABTYPE_INT_BRANCH:
	case emKTABTYPE_INT_INT:
	case emKTABTYPE_INT_BUF:
		//不要用INTEGER类型，一点也不爽。TINYBLOB有索引后查询一样快的
		s_KeyField.eType = emK_FIELDTYPE_TINYBLOB;
		s_KeyField.uMaxSize = sizeof(INT);
		break;
	case emKTABTYPE_STR_BRANCH:
	case emKTABTYPE_STR_INT:
	case emKTABTYPE_STR_BUF:
		s_KeyField.eType = emK_FIELDTYPE_VARCHAR;
		s_KeyField.uMaxSize = KD_ITEMKEY_MAX_SIZE;
		break;
	default:
		ASSERT(FALSE);
	}
	switch(pTabDesc->m_eTabType)
	{
	case emKTABTYPE_INT_BRANCH:
	case emKTABTYPE_STR_BRANCH:
	case emKTABTYPE_INT_INT:
	case emKTABTYPE_STR_INT:
		s_DataField.eType = emK_FIELDTYPE_TINYBLOB;
		s_DataField.uMaxSize = sizeof(INT);
		break;
	case emKTABTYPE_INT_BUF:
	case emKTABTYPE_STR_BUF:
		s_DataField.eType = emK_FIELDTYPE_BLOB;
		s_DataField.uMaxSize = KD_MEDIUM_FIELD_MAX_DATA_SIZE;
		break;
	default:
		ASSERT(FALSE);
	}
	sqlStream.AddCreateTable(emKTABLE_CREATE_IF_NOT_EXIST, kCommonDataTableDesc);
	//为TableID和Key创建复合索引
	KDBIndexDesc kMulColIndex[] = {
		{ s_TabIDField.szFieldName, s_TabIDField.uMaxSize },
		{ s_KeyField.szFieldName, s_KeyField.uMaxSize },
	};
	sqlStream.AddCreateTableMultiFieldIndex(emKFIELD_INDEX_COMMON, 
		sizeof(kMulColIndex) / sizeof(kMulColIndex[0]), kMulColIndex);
	INT nRetCode = m_Database.DoQuery(sqlStream);
	KGC_CONFIRM_RET_FALSE(nRetCode);
	return TRUE;
}

BOOL KGC_DBManager::LoadTableDataFromDB( KBaseTable *pTable, KE_TABLE_TYPE eTabType ) 
{
	//注意此函数会递归调用
	//获取一个Item的key（字段）和数据（字段）
	static KDBFieldDesc	QueryField[] = { { s_KeyField.szFieldName }, { s_DataField.szFieldName }};
	static INT TabIDData[2];
	static KDBFieldDesc	TabIDField = 
	{ s_TabIDField.szFieldName, s_TabIDField.eType, s_TabIDField.nFlag, s_TabIDField.uMaxSize, TabIDData, sizeof(INT)*2 };
	TabIDData[0] = pTable->m_nDBTabID;
	TabIDData[1] = pTable->m_nDataVersion;
	//由于函数会递归调用，所以不用栈内存，否则很容易栈溢出！因为不会多线程调用所以使用static内存没问题
	static CHAR sqlBuf[1024];
	KGC_MySqlQueryStream sqlStream = m_Database.CreateQueryStream(sqlBuf);
	if (emKCOZONE_STEP_COZONELOAD == m_eCoZoneStep)
	{
		// 加载子服表数据，DataVersion要减回去
		if (TabIDData[1] > 0)
			TabIDData[1] -= g_nCurDataVersion;
		CHAR szTable[256];
		LPCSTR szCoZoneDatabase = m_szCoZoneDbGcSub;
		KGC_CONFIRM_RET_FALSE(szCoZoneDatabase && szCoZoneDatabase[0]);
		sprintf(szTable, "%s.%s", szCoZoneDatabase, pTable->m_pDesc->szName);
		sqlStream.AddSelectQuery(sizeof(QueryField) / sizeof(QueryField[0]), QueryField, szTable);
	}
	else
	{
		sqlStream.AddSelectQuery(sizeof(QueryField) / sizeof(QueryField[0]), QueryField, pTable->m_pDesc->szName);
	}
	sqlStream.AddFieldCondition(emK_DBSQL_WHERE, emKOPERATOR_EQUAL, TabIDField);
	sqlStream.AddFieldCondition(emK_DBSQL_AND, emKOPERATOR_EQUAL, s_GroupField);
	BOOL bRet = m_Database.DoQuery(sqlStream);
	if (!bRet)
	{
		KGC_Error("Query Data Error While Loading Data From DB!");
		return FALSE;
	}
	KGC_MySQLResult rs = m_Database.GetQueryResult();
	if (!rs.IsValid())
	{
		KGC_Error("Get Query Result Error While Loading Data From DB!");
		return FALSE;
	}
	INT nItemCount = rs.GetRowCount();
	if (nItemCount < 0)
	{
		rs.Release();
		return TRUE;
	}
	for (INT i = 0; i < nItemCount; i++)
	{
		KGC_MySQLRow row = rs.GetNextRow();
		KGC_CONFIRM_CONTINUE(row.IsValid());
		AddItemToTable(pTable, eTabType, row.aFieldData[0], row.aFieldLenth[0],
			row.aFieldData[1], row.aFieldLenth[1]);
	}
	rs.Release();
	return TRUE;
}

//nKeySize不包含字符串结尾符
BOOL KGC_DBManager::AddItemToTable( KBaseTable *pTable, KE_TABLE_TYPE eTabType,
								   CHAR *pItemKey, INT nKeySize, CHAR *pItemData, INT nDataSize ) 
{
	switch(eTabType)
	{
	case emKTABTYPE_INT_BRANCH:
		{
			KIntBranchTable *pBranchTable = reinterpret_cast<KIntBranchTable*>(pTable);
			if (emKCOZONE_STEP_COZONELOAD == m_eCoZoneStep &&
				pBranchTable->m_mpData.find(*(LPINT)pItemKey) != pBranchTable->m_mpData.end())
				return TRUE;
			INT nTableID;
			KBranch *pBranch = g_DataPoolManager.NewTable<INT, INT>(&nTableID);
			KGC_CONFIRM_RET_FALSE(pBranch);
			pBranchTable->m_mpData[*(LPINT)pItemKey] = nTableID;
			//Branch的Desc即是表的desc
			pBranch->m_pDesc = pBranchTable->m_pDesc;
			INT IBuf[128];	//记录新的Branch数据，如果新增了表则要更新数据库的Branch数据
			LPINT pIBuf = IBuf;
			//创建这个Branch，返回TRUE表示新增了表，Branch数据要更新
			if (_GetBranchDataFromDB(pBranch, pItemData, nDataSize, pIBuf) ||
				emKCOZONE_STEP_COZONELOAD == m_eCoZoneStep)
			{
				PushSave(pBranchTable, *(LPINT)pItemKey, IBuf, (INT)(sizeof(INT) * (pIBuf - IBuf)),
					emKCOZONE_STEP_COZONELOAD == m_eCoZoneStep ? emKOPER_ADD_DATA : emKOPER_MODIFY_DATA);
			}
		}
		break;
	case emKTABTYPE_STR_BRANCH:
		{
			KStrBranchTable *pBranchTable = reinterpret_cast<KStrBranchTable*>(pTable);
			if (emKCOZONE_STEP_COZONELOAD == m_eCoZoneStep &&
				pBranchTable->m_mpData.find((LPCSTR)pItemKey) != pBranchTable->m_mpData.end())
				return TRUE;
			INT nTableID;
			KBranch *pBranch = g_DataPoolManager.NewTable<INT, INT>(&nTableID);
			KGC_CONFIRM_RET_FALSE(pBranch);
			pBranchTable->m_mpData[(LPCSTR)pItemKey] = nTableID;
			//Branch的Desc即是表的desc
			pBranch->m_pDesc = pBranchTable->m_pDesc;
			INT IBuf[128];	//记录新的Branch数据，如果新增了表则要更新数据库的数据
			LPINT pIBuf = IBuf;
			//创建这个Branch，返回TRUE表示新增了表，Branch数据要更新
			if (_GetBranchDataFromDB(pBranch, pItemData, nDataSize, pIBuf) ||
				emKCOZONE_STEP_COZONELOAD == m_eCoZoneStep)
			{
				PushSave(pBranchTable, (LPCSTR)pItemKey, IBuf, (INT)(sizeof(INT) * (pIBuf - IBuf)),
					emKCOZONE_STEP_COZONELOAD == m_eCoZoneStep ? emKOPER_ADD_DATA : emKOPER_MODIFY_DATA);
			}
		}
		break;
	case emKTABTYPE_INT_INT:
		{
			KMapIntIntTable *pDataTable = reinterpret_cast<KMapIntIntTable*>(pTable);
			if (emKCOZONE_STEP_COZONELOAD == m_eCoZoneStep &&
				pDataTable->m_mpData.find(*(LPINT)pItemKey) != pDataTable->m_mpData.end())
				return TRUE;
			pDataTable->m_mpData[*(LPINT)pItemKey] = *(LPINT)pItemData;
			if (emKCOZONE_STEP_COZONELOAD == m_eCoZoneStep)
			{
				PushSave(pDataTable, *(LPINT)pItemKey, pItemData, sizeof(INT), emKOPER_ADD_DATA);
			}
		}
		break;
	case emKTABTYPE_STR_INT:
		{
			KMapStrIntTable *pDataTable = reinterpret_cast<KMapStrIntTable*>(pTable);
			if (emKCOZONE_STEP_COZONELOAD == m_eCoZoneStep &&
				pDataTable->m_mpData.find((LPCSTR)pItemKey) != pDataTable->m_mpData.end())
				return TRUE;
			pDataTable->m_mpData[(LPCSTR)pItemKey] = *(LPINT)pItemData;
			if (emKCOZONE_STEP_COZONELOAD == m_eCoZoneStep)
			{
				PushSave(pDataTable, (LPCSTR)pItemKey, pItemData, sizeof(INT), emKOPER_ADD_DATA);
			}
		}
		break;
	case emKTABTYPE_INT_BUF:
		{
			KMapIntBufTable *pDataTable = reinterpret_cast<KMapIntBufTable*>(pTable);
			if (emKCOZONE_STEP_COZONELOAD == m_eCoZoneStep &&
				pDataTable->m_mpData.find(*(LPINT)pItemKey) != pDataTable->m_mpData.end())
				return TRUE;
			DATA_DESC desc;
			desc.m_BufType = GetPropReserveBufSize(nDataSize + 1);
			desc.m_DataSize = nDataSize;
			CHAR *pBuf = g_DataPoolManager.NewBufData(desc.m_BufType, &desc.m_nBufIndex);
			KGC_CONFIRM_RET_FALSE(pBuf);
			memcpy(pBuf, pItemData, desc.m_DataSize);
			pBuf[desc.m_DataSize] = 0;
			pDataTable->m_mpData[*(LPINT)pItemKey] = desc;
			if (emKCOZONE_STEP_COZONELOAD == m_eCoZoneStep)
			{
				PushSave(pDataTable, *(LPINT)pItemKey, pItemData, nDataSize, emKOPER_ADD_DATA);
			}
		}
		break;
	case emKTABTYPE_STR_BUF:
		{
			KMapStrBufTable *pDataTable = reinterpret_cast<KMapStrBufTable*>(pTable);
			if (emKCOZONE_STEP_COZONELOAD == m_eCoZoneStep &&
				pDataTable->m_mpData.find((LPCSTR)pItemKey) != pDataTable->m_mpData.end())
				return TRUE;
			DATA_DESC desc;
			desc.m_BufType = GetPropReserveBufSize(nDataSize + 1);
			desc.m_DataSize = nDataSize;
			CHAR *pBuf = g_DataPoolManager.NewBufData(desc.m_BufType, &desc.m_nBufIndex);
			KGC_CONFIRM_RET_FALSE(pBuf);
			memcpy(pBuf, pItemData, desc.m_DataSize);
			pBuf[desc.m_DataSize] = 0;
			pDataTable->m_mpData[(LPCSTR)pItemKey] = desc;
			if (emKCOZONE_STEP_COZONELOAD == m_eCoZoneStep)
			{
				PushSave(pDataTable, (LPCSTR)pItemKey, pItemData, nDataSize, emKOPER_ADD_DATA);
			}
		}
		break;
	default:
		ASSERT(FALSE);
	}
	return TRUE;
}

BOOL KGC_DBManager::_GetBranchDataFromDB( KBranch *pBranchData, LPVOID pData, INT nDataSize, LPINT &pIBuf) 
{
	KBranchDesc *pBranchDescData = pBranchData->m_pDesc->m_pBranchDesc;
	LPINT pIntData = (LPINT)pData;
	//读取分支的表数
	INT nTableNum = nDataSize / (sizeof(INT) * 3);
	std::map<INT, std::pair<INT, INT> > mapBranchData;
	while (nTableNum-- > 0)
	{
		INT nKey = *pIntData++;
		INT nDBTableID = *pIntData++;
		INT nDataVerID = *pIntData++;
		if (emKCOZONE_STEP_COZONELOAD == m_eCoZoneStep)
			nDataVerID += g_nCurDataVersion;
		mapBranchData[nKey] = std::pair<INT, INT>(nDBTableID, nDataVerID); 
	}
	/*创建分支的各个表*/
	BOOL	bModify = FALSE;
	std::map<INT, TABLE_DESC*>::iterator itDesc;
	for (itDesc = pBranchDescData->m_mpData.begin(); itDesc != pBranchDescData->m_mpData.end(); ++itDesc)
	{
		INT nTableID;
		KBaseTable *pNewTable = GCNewTable(itDesc->second, nTableID);
		if (!pNewTable)
		{
			ASSERT(FALSE);
			continue;
		}
		pNewTable->m_pDesc = itDesc->second;
		pBranchData->m_mpData[itDesc->first] = nTableID;
		std::map<INT, std::pair<INT, INT> >::iterator itFind = mapBranchData.find(itDesc->first);
		//在数据库的数据中找不到这个表的key，证明新增了表
		if (itFind == mapBranchData.end())
		{
			pNewTable->m_nDBTabID = nTableID;
			pNewTable->m_nDataVersion =	g_nCurDataVersion;
			bModify = TRUE;	//有新增的表，就要更新数据库中每一个Branch的Data
		}
		else
		{
			//使用数据库的TableID和DataVersionID
			pNewTable->m_nDBTabID = itFind->second.first;
			pNewTable->m_nDataVersion = itFind->second.second;
			//从数据库加载这个表数据
			//如果当前没有Save属性而数据库有这个表数据，表明是原来Save的表改成不Save
			if (pNewTable->m_pDesc->m_Flag & GetSaveCheckFlag())
				LoadTableDataFromDB(pNewTable, (KE_TABLE_TYPE)itDesc->second->m_eTabType);
		}
		//记录新的Branch的数据
		*pIBuf++ = itDesc->first;
		*pIBuf++ = pNewTable->m_nDBTabID;
		*pIBuf++ = pNewTable->m_nDataVersion;
	}
	return bModify;
}

BOOL KGC_DBManager::MainThreadActive() 
{
	m_ThreadLock.Lock();
	//注意return前要Unlock
	if (!m_queMainThreadCallback.empty())
	{
		KMAIN_CALLBACK sCallBack = m_queMainThreadCallback.front();
		m_queMainThreadCallback.pop();
		m_ThreadLock.Unlock();
		IKMainThreadCallback *piCallback = sCallBack.pCallBack;
		piCallback->MainThreadCallback(sCallBack.nParam, sCallBack.pParam);
		piCallback->Release();
		return TRUE;
	}
	m_ThreadLock.Unlock();
	return FALSE;
}

BOOL KGC_DBManager::CoZoneInit( LPCSTR szGcDbMain, LPCSTR szGoddessDbMain,  LPCSTR szGcDbSub, LPCSTR szGoddessDbSub, BOOL bDelTempDb)
{
	 m_eCoZoneStep = emKCOZONE_STEP_INIT;
	 g_StrCpyLen(m_szCoZoneDbGcMain, szGcDbMain, sizeof(m_szCoZoneDbGcMain));
	 g_StrCpyLen(m_szCoZoneDbGoddessMain, szGoddessDbMain, sizeof(m_szCoZoneDbGoddessMain));
	 g_StrCpyLen(m_szCoZoneDbGcSub, szGcDbSub, sizeof(m_szCoZoneDbGcSub));
	 g_StrCpyLen(m_szCoZoneDbGoddessSub, szGoddessDbSub, sizeof(m_szCoZoneDbGoddessSub));
	 m_bDelTempDB = bDelTempDb;
	 return TRUE;
}

BOOL KGC_DBManager::CoZoneStepClear()
{
	m_eCoZoneStep = emKCOZONE_STEP_DBSUBCLEAR;
	return TRUE;
}

BOOL KGC_DBManager::CoZoneStepLoadDbMain()
{
	INT nRetCode = m_Database.SetCurrentDatabase(m_szCoZoneDbGcMain);
	KGC_CONFIRM_RET_FALSE(nRetCode);
	LoadDBShareData();	// 加载版本号
	m_eCoZoneStep = emKCOZONE_STEP_DBMAIN;
	return TRUE;
}

BOOL KGC_DBManager::CoZoneStepLoadDbSub()
{
	m_eCoZoneStep = emKCOZONE_STEP_COZONELOAD;
	return TRUE;
}

BOOL KGC_DBManager::CoZoneStepLoadDbSubEnd()
{
	m_eCoZoneStep = emKCOZONE_STEP_COZONELOADED;
	return TRUE;
}

BOOL KGC_DBManager::CoZoneUnInit()
{
	CHAR szQuery[2048];
	KG_Printl("Combining Role DB " << m_szCoZoneDbGoddessSub << " to " << m_szCoZoneDbGoddessMain);
	UINT uLen = sprintf(szQuery, "insert into %s.role select * from %s.role",
		m_szCoZoneDbGoddessMain, m_szCoZoneDbGoddessSub);
	BOOL bRet = m_Database.DoQuery(szQuery, uLen);
	if (m_bDelTempDB == TRUE)
	{
		KG_Printl("Droping Temp DB.."<< m_szCoZoneDbGoddessSub << "\t" << m_szCoZoneDbGcSub);
		CHAR szDropDbTmp[1024];
		sprintf(szDropDbTmp, "drop database %s", m_szCoZoneDbGcSub);
		m_Database.DoQuery(szDropDbTmp, (DWORD)strlen(szDropDbTmp));
		sprintf(szDropDbTmp, "drop database %s", m_szCoZoneDbGoddessSub);
		m_Database.DoQuery(szDropDbTmp, (DWORD)strlen(szDropDbTmp));
	}
	return bRet;
}
#endif


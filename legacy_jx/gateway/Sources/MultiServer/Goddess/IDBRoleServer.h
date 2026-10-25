/********************************************************************
	created:	2003/06/03
	file base:	IDBRoleServer
	file ext:	h
	author:		liupeng
	
	purpose:	
*********************************************************************/
#ifndef __INCLUDE_IDBROLESERVER_H__
#define __INCLUDE_IDBROLESERVER_H__

//char mAccountName[32];

#include "S3DBInterface.h"
#include "LogFile.h"
#include <sqlite3.h>
#include <time.h>


enum enumITEM_Type
{
	ITEM_KIND_SHENG,
	ITEM_KIND_ZIMU,
	ITEM_KIND_CHUWU,
	ITEM_KIND_KUOYI,
	ITEM_KIND_KUOER,
	ITEM_KIND_KUOSAN,
	ITEM_KIND_ALL,
	ITEM_KIND_COUNT,
};


enum enumdateType
{
	DATA_KIND_DB,
	DATA_KIND_SQL,
};

unsigned int FileNameDB(const char* pszFileName);
int copySqldataToDB();
int copyDBdataToSql();
int copySqldataTobakSql();

int InitDBInterface( size_t nMaxRoleCount,int dataType=DATA_KIND_DB );

void ReleaseDBInterface();

void *GetRoleInfo( char * pRoleBuffer, char * strUser, int &nBufLen ,char *pAccName=NULL,UINT nThsiServer=1, sqlite3_int64* roleRevision=NULL);
void *GetRoleInfoForGM(int nInfoID, char * pRoleBuffer, char * strUser, int &nBufLen);
void SetRoleInfoForGM(int nInfoID, char * pRoleBuffer, char * strUser, int nBufLen);

int	SaveRoleInfo( char * pRoleBuffer, const char * strUser, BOOL bAutoInsertWhenNoExistUser );

int CheckRoleName(char *pName); //检测角色信息

int GetRoleListOfAccount( char * szAccountName, S3DBI_RoleBaseInfo * RoleBaseList, int nMaxCount,UINT nNameLen,UINT nThisServer);

bool DeleteRole( const char * strUser );

char* GetAccountByUser(char * strUser);//通过用户名查找帐户

bool TestGetData(char *nPname,char *nDataVal,int nType=0);

bool TestAddData(char *nPname,char *nBuffer);

bool StartMergeReg(int nSourceRegm,int nTargetReg,char *nNameStr,int nDelLevel=0);

bool ClearRoleData(bool nIsAll,int nReg);

bool ClearRoleSkillData(char *nScrRoleName);

bool DelRoleData(char *nRoelName,bool nIsAcc);

int ChangeRoleName(char *nScrRoleName,char *nTarRoleName);

int ChanGeRoleReg(char *nNameStrRole,int nTargetReg);

int AddMoneyRoleCount(int nMoneyKind,int nIsOnLine,char *nMoneyStrRole,int nMoneyCount);

int  ChangeRoleInfo(char *nNameStrRole,int nMapidx);

int bakFindandAddRoleToDb(char *nRoleName,char *nBakPath);

bool MoveRoleLevel(int nMoveLevel);

bool getItemData(char *accName,char *roleName,int shen,int zimu,int chuwu,int kuoyi,int kuoer,int kuosan,int all);

//------------------------------------------------------------------------
bool RemoveLogFiel();
//数据库备份与数据统计 Add By Fellow At 2003.08.14
bool StartBackupTimer(int aTime);		        //开始运行备份线程
bool StopBackupTimer();							//结束运行备份线程
bool SuspendBackupTimer();						//挂起线程
bool ResumeBackupTimer();						//继续运行线程
bool IsBackupThreadWorking();					//线程是否正在运行
bool IsBackupWorking();							//是否在备份
bool DoManualBackup();							//手工备份

bool GetGameStat(TGAME_STAT_DATA* aStatData);   //取得用户统计数据
//------------------------------------------------------------------------
void AddOutputString (HWND hListCtrl, char* aStr);//添加操作输出文字
void AddRoleOutputString(HWND hListCtrl, char* aStr);//添加操作输出文字
void gTraceLogFile(LPCTSTR pBuffer, long nLen);
int    gTimeStamp(char* sDest, char* pTail);




// Trade Pair Atomic Save
int SaveTradePairAtomic(unsigned __int64 uTxId, const char* szRoleA, void* pDataA, int nLenA, unsigned int dwCrcA, const char* szRoleB, void* pDataB, int nLenB, unsigned int dwCrcB, unsigned int uGenA = 0, unsigned int uGenB = 0);

#endif // __INCLUDE_IDBROLESERVER_H__
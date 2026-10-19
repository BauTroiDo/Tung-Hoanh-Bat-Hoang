#include "stdafx.h"
#include "IDBRoleServer.h"
#include "DBTable.h"

#include "mysql_db.h"
#include "sqlitel_db.h"

#include "DBBackup.h"
#include "KGmProtocol.h"
#include "CRC32.h"
#include <stddef.h>
#include <errno.h>

#include "Utils.h"
#include "Inifile.h"
#include "tstring.h"
/*
using OnlineGameLib::Win32::CIniFile;
using OnlineGameLib::Win32::_tstring;
using OnlineGameLib::Win32::GetAppFullPath;
*/
#include <iostream>
#include <strstream>

using namespace std;

#include "Macro.h"

static ZDBTable *db_table = NULL;

static mysql_db *mysql_table = NULL;

static sqlitel_db *sqlitel_table = NULL;

static size_t nMaxRoleCount_InAccount = 3;  //角色数量
static CDBBackup::TStatData GameStatData;   //游戏统计数据（服务器初始化时填入）
static CDBBackup* DBBackup = NULL;

static HANDLE hDeadLockThread = NULL;		//检查数据库有没有死锁的线程
static HANDLE hRemoveLogThread = NULL;	    //删除没用日志文件的线程


HWND hRoleNamekList_skill= NULL;
HWND hBakRoleListOutput= NULL;
HWND hListOutput = NULL;	                //输出list
HWND hRoleErrOutput = NULL;
HWND hForbitRoleName= NULL;
HWND mainHwnd= NULL;

size_t   g_dataType  = DATA_KIND_DB;

CLogFile gs_LogFile;


static inline void sLogNewProgram(char *nStrInfo)
{
	SYSTEMTIME systm;
	::GetLocalTime(&systm);
	TCHAR buff[256];
	int len = 0;
	len = sprintf(buff, "***** Goddees %s (@ %04d/%02d/%02d %02d:%02d:%02d.%03d) >>>>>\r\n",
		nStrInfo,systm.wYear, systm.wMonth, systm.wDay, systm.wHour, systm.wMinute, systm.wSecond, systm.wMilliseconds);

	gs_LogFile.TraceLog(buff, len * sizeof(TCHAR));
}

int get_account(DB *db, const DBT *pkey, const DBT *pdata, DBT *ikey) 
{
	//给定一个完整的buffer，得到account作为索引
	memset(ikey,0,sizeof(DBT));
	if (pdata->size < offsetof(TRoleData, pBuffer))
		return EINVAL;
	TRoleData *pRoleData = (TRoleData *)pdata->data;

	//TRoleData *pRoleData = (TRoleData *)pdata->get_data();

	ikey->data = pRoleData->BaseInfo.caccname;	            //KEY 
	ikey->size = strlen(pRoleData->BaseInfo.caccname) + 1;	//KEY值
    //ikey->set_data(pRoleData->BaseInfo.caccname);
    //ikey->set_size(strlen(pRoleData->BaseInfo.caccname) + 1);

	return 0;
}

//==========检查数据库有没有死锁的线程 Add By Fellow 2003.9.10==============
DWORD WINAPI DeadlockProc(LPVOID lpParameter) {
	while(!db_table->bStop)
	{
		Sleep(5*1000);	//5秒钟一次
		db_table->deadlock();
	}
	return 0;
}
//==========删除没用日志文件的线程 Add By Fellow 2003.9.10==============
DWORD WINAPI RemoveLogProc(LPVOID lpParameter) 
{
	while(!db_table->bStop)
	{
		db_table->removeLog();
		Sleep(30*60*1000);			//1小时一次
	} 
	return 0;
}


unsigned int FileNameDB(const char* pszFileName)
{
		unsigned int len = strlen(pszFileName);
		const char *end=pszFileName+len;
		unsigned int hash;

		for (hash = 0; pszFileName < end; pszFileName++)
		{
			hash *= 16777619;
			hash ^= (unsigned int)(unsigned char)toupper(*pszFileName);
		}
		return (hash);

	/*_ASSERT(pszFileName && pszFileName[0]);
	unsigned long id = 0;
	const char *ptr = pszFileName;
	int index = 0;
	while(*ptr)
	{
	if(*ptr >= 'A' && *ptr <= 'Z') 
	id = (id + (++index) * (*ptr + 'a' - 'A')) % 0x8000000b * 0xffffffef;
	else 
	id = (id + (++index) * (*ptr)) % 0x8000000b * 0xffffffef;
	ptr++;
	}
	return (id ^ 0x12345678);*/
}

bool getItemData(char *accName,char *roleName,int shen,int zimu,int chuwu,int kuoyi,int kuoer,int kuosan,int all)
{
	if (sqlitel_table)
		sqlitel_table->findRoleITemInSql(accName,roleName,shen,zimu,chuwu,kuoyi,kuoer,kuosan,all);
	return true;
}

int copySqldataTobakSql()
{
	if (sqlitel_table)
	     sqlitel_table->copyDataToBaktable();

	return 1;
}

int copySqldataToDB()
{
	if (db_table && sqlitel_table)
		sqlitel_table->copyDataToDB(db_table);

	return 1;
}


int copyDBdataToSql()
{  //hBakRoleListOutput  AddRoleOutputString
	char outStr[218];
	if (db_table && sqlitel_table)
	{
		ZCursor *copycursor = db_table->first();
		while(copycursor)
		{	
			TRoleData* pRoleData = (TRoleData*)copycursor->data;
			if(pRoleData)
			{
				unsigned int nkey=FileNameDB(pRoleData->BaseInfo.szName);
				//sqlitel_table->deleteBlobData(pRoleData->BaseInfo.szName,nkey,true);
				//sqlitel_table->InsertBlobData((void *)copycursor->data,copycursor->size,pRoleData->BaseInfo.szName,nkey);
			    sqlitel_table->updateBlobData((void *)copycursor->data,copycursor->size,pRoleData->BaseInfo.szName,nkey,true);

				sprintf(outStr,"-拷贝:%s:%s成功",pRoleData->BaseInfo.caccname,pRoleData->BaseInfo.szName);
				AddRoleOutputString(hBakRoleListOutput,outStr);
			}

			if(!db_table->next(copycursor))
				break;
		} 

		return true;
	}
	AddRoleOutputString(hBakRoleListOutput,"db或sql数据库没有打开,不能copy!");
	return false;
}



//初始化 数据环境
int InitDBInterface(size_t nMaxRoleCount,int dataType)
{
	nMaxRoleCount_InAccount = nMaxRoleCount;
	char nTempstr[128];
	if  (dataType==DATA_KIND_SQL)
	{
		sqlitel_table = new sqlitel_db();
		if (sqlitel_table)
		{
			if  (sqlitel_table->Open("database",false))
			{
				//写BLOB数据
				//string sql = "create table if not exists table_role(id int not null auto_increment, name varchar(32) not null, edata mediumblob,ekey int,primary key (id) )";		
				if (sqlitel_table->creatTable("role_Blob"))
			   {
				  // ::MessageBox(NULL, "mysql 创建表role_Blob成功", "suss", MB_OK|MB_ICONERROR);
				   sprintf (nTempstr,"------mysql 创建表role_Blob成功------");
				   AddRoleOutputString(hListOutput,nTempstr);
		    	}

				if (sqlitel_table->creatTable_bak("role_Blob"))
				{
					// ::MessageBox(NULL, "mysql 创建表role_Blob成功", "suss", MB_OK|MB_ICONERROR);
					sprintf (nTempstr,"------mysql 创建备份表role_Blob成功------");
					AddRoleOutputString(hListOutput,nTempstr);
				}
				
				//sqlitel_table->deleteBlobData("xtent001",123456);
				//char* aBuffer = new char[128*1024]; //64
				//TRoleData *pRoleData= (TRoleData *)aBuffer;
				//int size;
				//sprintf(pRoleData->BaseInfo.caccname,"xtnet001");
				//sprintf(pRoleData->BaseInfo.szName,"测试数据");
				//pRoleData->BaseInfo.dwId=1234568;
				//pRoleData->nFightSkillCount=123;

				//pRoleData->nFightSkillCount=123;   //战斗技能数量
				//pRoleData->nLiveSkillCount=456;  //生活技能
				//pRoleData->nTaskCount=789;         //任务数量
				//pRoleData->nItemCount =102;         //物品数量
				////memcpy(aBuffer,&pRoleData,sizeof(pRoleData));
				//sqlitel_table->InsertBlobData(pRoleData,128*1024,"xtent001",123456);

				////UPDATE mytable SET myblobcolumn = x'ffeedd' WHERE your_where_clause;

				//char* aBuffe_2r = new char[128*1024]; //64
				//TRoleData *pRoleData_2= (TRoleData *)aBuffe_2r;
				//sprintf(pRoleData_2->BaseInfo.caccname,"xtnet002");
				//sprintf(pRoleData_2->BaseInfo.szName,"测试数据2");
				//pRoleData_2->BaseInfo.dwId=1234568;
				//pRoleData_2->nFightSkillCount=123;
				//pRoleData_2->nFightSkillCount=123;   //战斗技能数量
				//pRoleData_2->nLiveSkillCount=456;  //生活技能
				//pRoleData_2->nTaskCount=789;         //任务数量
				//pRoleData_2->nItemCount =102;         //物品数量
				////memcpy(aBuffer,&pRoleData,sizeof(pRoleData));
				//sqlitel_table->InsertBlobData(pRoleData_2,128*1024,"xtent002",123456);
				////读取数据
				//sqlitel_table->SelectBlobData("xtent002",123456);

				 sprintf (nTempstr,"------mysql 创建数据库成功------");
				 AddRoleOutputString(hListOutput,nTempstr);
				//::MessageBox(NULL, "mysql 创建数据库成功", "suss", MB_OK|MB_ICONERROR);
			}

		}

		//mysql_table =new mysql_db();
  //      if  (mysql_table)
		//{
		//	//mysql_table->init();
		//	bool isSuss=false;
		//	if (mysql_table->mysql_open("localhost","root","1234","jxsj_goddess",3306))
		//	{
		//			//创建一张表
		//			string sql = "create table if not exists table_role(id int not null auto_increment, name varchar(32) not null, edata mediumblob,ekey int,primary key (id) )";
		//			if ( -1 == mysql_table->mysql_noResult_query(sql.c_str()))
		//			{

		//			}

		//			//往表中插入2条数据
		//			/*sql = "insert into table_test (name,age) values('newsoul','400')";
		//			mysql_table->InsertRecord(sql.c_str());
		//			sql = "insert into table_test (name,age) values('百度','500')";
		//			mysql_table->InsertRecord(sql.c_str());*/

		//			//#define INSERT_QUERY  "INSERT INTO table_role(name,edata) VALUES('测试数据', ?)" 
		//			sql="INSERT INTO table_role(name,edata,ekey) VALUES('测试数据...', ?,112233)" ;
		//			char testStr[128];
		//			sprintf(testStr,"%s","1111111111111111111111111111111111111111111111");

		//			char* aBuffer = new char[128*1024]; //64
		//			TRoleData pRoleData;
		//			ZeroMemory(&pRoleData,sizeof(pRoleData));
		//			int size;
		//		//	GetRoleInfo(aBuffer, strUser, size);
		//
		//		    
		//			sprintf(pRoleData.BaseInfo.caccname,"xtnet001");
		//			sprintf(pRoleData.BaseInfo.szName,"测试数据");
		//			pRoleData.BaseInfo.dwId=0;
		//			pRoleData.nFightSkillCount=123;
		//			
		//			pRoleData.nFightSkillCount=123;   //战斗技能数量
		//			pRoleData.nLiveSkillCount=456;  //生活技能
		//			pRoleData.nTaskCount=789;         //任务数量
		//			pRoleData.nItemCount =102;         //物品数量
		//			memcpy(aBuffer,&pRoleData,sizeof(pRoleData));

		//			//int nLen=mysql_table->EncodeString(aBuffer_N,aBuffer,sizeof(pRoleData));

		//			if (!mysql_table->addBlobData(NULL,sql.c_str(),aBuffer,sizeof(pRoleData)))
		//			{
		//				if (aBuffer)
		//				{
		//					delete [] aBuffer;
		//					aBuffer=NULL;
		//				}
		//				::MessageBox(NULL, "mysql 插入数据失败", "suss", MB_OK|MB_ICONERROR);
		//			}

		//			//vector<string> vResults;
		//			//	sql = "select * from table_role where ekey=112233";
	

		//			if (aBuffer)
		//			{
		//				delete [] aBuffer;
		//				aBuffer=NULL;
		//			}
		//		char nstr[64];
		//		sprintf(nstr,"size(%d)",sizeof(TRoleData));		
		//		::MessageBox(NULL, "mysql 链接成功",nstr, MB_OK|MB_ICONERROR);
		//	}
		//	else
		//		::MessageBox(NULL, "mysql 链接失败", "error", MB_OK|MB_ICONERROR);
		//}
		//else
		//	::MessageBox(NULL, "mysql null", "error", MB_OK|MB_ICONERROR);

	//   return true;
	}




	db_table = new ZDBTable("database","roledb");  //新建内存

	if  (db_table==NULL)
		return -5;
	
	if (db_table->addIndex(get_account)==-1)   //增加二级数据
		return -6;

	int nReg=0;
	    nReg =db_table->open();

	if (nReg==1)
	{
		DWORD dwThreadId, dwThrdParam = 1;

        //死锁检测
		/*hDeadLockThread = CreateThread(
			NULL,				// no security attributes 
			0,					// use default stack size  
			DeadlockProc,		// thread function 
			&dwThrdParam,		// argument to thread function 
			0,					// use default creation flags 
			&dwThreadId);		// returns the thread identifier 

		if(!hDeadLockThread)
		{
			//创建线程失败,暂时没有处理
		}
	   */
		
		hRemoveLogThread = CreateThread(
			NULL,				// no security attributes 
			0,					// use default stack size  
			RemoveLogProc,		// thread function 
			&dwThrdParam,		// argument to thread function 
			0,					// use default creation flags 
			&dwThreadId);		// returns the thread identifier 
	
		if(!hRemoveLogThread)
		{
			//创建线程失败,暂时没有处理
			//::MessageBox( NULL, "hRemoveLogThread is failed!", "Warning", MB_OK | MB_ICONSTOP );
			//ReleaseDBInterface();
			//return -2;	    
		} 
			
		return 1;
	}
	
	return nReg;
}

void ReleaseDBInterface()		//释放数据库引擎
{
	if (db_table)
	{
		db_table->commit();
/*

		if(!hDeadLockThread)   //解除锁死线程创建失败
			TerminateThread(hDeadLockThread, 0);
*/
		if(!hRemoveLogThread)
			TerminateThread(hRemoveLogThread, 0);

		db_table->removeLog();	  //清楚日志
		
		StopBackupTimer();//停止备份线程
	
		db_table->close();

		//delete db_table;
		//db_table=NULL;
	}

	if (mysql_table)
	{
		mysql_table->close();
		delete mysql_table;
		mysql_table=NULL;
	}

	if  (sqlitel_table)
	{
		sqlitel_table->Close();
		delete sqlitel_table;
		sqlitel_table=NULL;
	}
}

//[wxb 2003-7-23]
void SetRoleInfoForGM(int nInfoID, char* pRoleBuffer, char* strUser, int nBufLen)
{
	char* aBuffer = new char[128*1024]; //64
	TRoleData* pRoleData = NULL;
	// GetRoleInfo measures its copy against the room this argument arrives holding, so
	// the buffer's own width is what goes in.
	int size = 128 * 1024;

	GetRoleInfo(aBuffer, strUser, size);
	if(size)
	{
		pRoleData = (TRoleData*)aBuffer;
		switch (nInfoID)
		{
		case gm_role_entergame_position:
			ASSERT(nBufLen == sizeof(GM_ROLE_DATA_SUB_ENTER_POS));
			memcpy(&(pRoleData->BaseInfo.ientergameid), pRoleBuffer, sizeof(GM_ROLE_DATA_SUB_ENTER_POS));
			break;
		default:
			ASSERT(0);
			return;
			break;
		}
		SaveRoleInfo(aBuffer, strUser, false);
	}

	if (aBuffer)
	{
      delete [] aBuffer;
	  aBuffer=NULL;
	}
}

//[wxb 2003-7-22]
void *GetRoleInfoForGM(int nInfoID, char * pRoleBuffer, char * strUser, int &nBufLen)
{
	int size = 0;
	ZCursor *cursor = db_table->search(strUser, strlen( strUser ) + 1);
//	char *buffer = db_table->search( strUser, strlen( strUser ) + 1, size );
	if ( cursor )
	{
		TRoleData* pRole = (TRoleData*)cursor->data;
		if (cursor->size < (int)offsetof(TRoleData, pBuffer))
		{
			nBufLen = 0;
			db_table->closeCursor(cursor);
			return pRoleBuffer;
		}
		switch(nInfoID) {
		case gm_role_entergame_position:
			nBufLen = sizeof(pRole->BaseInfo.ientergameid) + sizeof(pRole->BaseInfo.ientergamex) + sizeof(pRole->BaseInfo.ientergamey);
			memcpy( pRoleBuffer, &(pRole->BaseInfo.ientergameid), nBufLen );
			break;
		default:
			ASSERT(0);
			nBufLen = 0;
			break;
		}

		db_table->closeCursor(cursor);
	}
	else
	{
		nBufLen = 0;
	}

	return pRoleBuffer;
}

void *GetRoleInfo( char * pRoleBuffer, char * strUser, int &nBufLen, char *pAccName,UINT nThsiServer)
{

	int size = 0;
	char aStr[128]={0};

	if  (g_dataType==DATA_KIND_SQL)
	{
		unsigned int nKey=FileNameDB(strUser);
		nBufLen = sqlitel_table->SelectBlobData(pRoleBuffer,strUser,nKey);
		if  (nBufLen<=0)
		{
			nBufLen=0;
			snprintf(aStr, sizeof(aStr),"-------------------[SQL]获取[%s]数据失败,key:%u-------------------------",strUser,nKey);  //角色
			AddOutputString(hListOutput,aStr);
			return pRoleBuffer;
		}

		TRoleData *pRoleData = (TRoleData *)pRoleBuffer;  //检测账号
		if (!g_StrCmp(pAccName,pRoleData->BaseInfo.caccname))
		{
			nBufLen = 0;
			snprintf(aStr, sizeof(aStr),"--[SQL]帐号恶意登录:源账号:%s,客户端改:%s,角色:%s 断开链接--",pRoleData->BaseInfo.caccname,pAccName,strUser);  //角色
			AddOutputString(hListOutput,aStr);
			gTraceLogFile(aStr,strlen(aStr));
			return pRoleBuffer;
		} 

		if (pRoleData->BaseInfo.iRoleParm6==0  && pRoleData->BaseInfo.isWaiGua!=nThsiServer)
		{//不是跨服的 这个区的 不能登陆,跨服的不检测 是哪个区的
			nBufLen = 0;
			snprintf(aStr, sizeof(aStr),"--[SQL]区恶意登录:源账号:%s,客户端改:%s,角色:%s 断开链接--",pRoleData->BaseInfo.caccname,pAccName,strUser);  //角色
			AddOutputString(hListOutput,aStr);
			gTraceLogFile(aStr,strlen(aStr));
			return pRoleBuffer;
		}
		snprintf(aStr, sizeof(aStr),"--[SQL]获取数据成功:账号:%s,   角色:%s    等级:%d--",pRoleData->BaseInfo.caccname,pRoleData->BaseInfo.szName,pRoleData->BaseInfo.ifightlevel);  //角色
		AddOutputString(hListOutput,aStr);
		return pRoleBuffer;
	}


	ZCursor *cursor = db_table->search(strUser,strlen(strUser) + 1);  //主数据库 查找数据库角色资料
//	char *buffer = db_table->search( strUser, strlen( strUser ) + 1, size );
	if (cursor)
	{
		// cursor->size is the record the database holds and pRoleBuffer is somebody else's:
		// a 128K new[] here, the tail of a node's CBuffer in ClientNode.  The caller's room is
		// what nBufLen arrives stating, and a record larger than it is refused - the login
		// packet this copies into has no more bytes behind it than the node was given.
		if (cursor->size < (int)offsetof(TRoleData, pBuffer) || cursor->size > nBufLen)
		{
			nBufLen = 0;
			db_table->closeCursor(cursor);
			return pRoleBuffer;
		}
		nBufLen = size;
		memcpy(pRoleBuffer,cursor->data,cursor->size);  //赋值角色数据包
		nBufLen = cursor->size;
		db_table->closeCursor(cursor);
		
		TRoleData *pRoleData = (TRoleData *)pRoleBuffer;  //检测账号
		if (!g_StrCmp(pAccName,pRoleData->BaseInfo.caccname))
		{
			nBufLen = 0;
			snprintf(aStr, sizeof(aStr),"--帐号恶意登录:数据原:%s,客户端改:%s,角色:%s 断开链接--",pRoleData->BaseInfo.caccname,pAccName,strUser);  //角色
			AddOutputString(hListOutput,aStr);
			gTraceLogFile(aStr,strlen(aStr));
		} 

		if (pRoleData->BaseInfo.iRoleParm6==0  && pRoleData->BaseInfo.isWaiGua!=nThsiServer)
		{//不是跨服的 这个区的 不能登陆,跨服的不检测 是哪个区的
			nBufLen = 0;
			snprintf(aStr, sizeof(aStr),"--区恶意登录:数据原:%s,客户端改:%s,角色:%s 断开链接--",pRoleData->BaseInfo.caccname,pAccName,strUser);  //角色
			AddOutputString(hListOutput,aStr);
			gTraceLogFile(aStr,strlen(aStr));
		}
		
	}
	else
	{
		    nBufLen = 0;
	}
	return pRoleBuffer;
}

int  CheckRoleName(char *pName)
{
		/*
		 * Role of same name only  //检测角色名唯一性
		 */
	    if  (g_dataType==DATA_KIND_SQL)
	    {

	     }

	    if (!db_table)
			return 0;

		ZCursor *user_cursor = db_table->search(pName, strlen( pName ) + 1 );
//		char *user_data = db_table->search( pName, strlen( pName ) + 1, size );
		if ( user_cursor )
		{
			db_table->closeCursor(user_cursor);

			return 1;
		}
		return 0;
}

//const char file_rolelog[] = "goddes_log\\goddess.log";

//保存角色的信息，如果数据库不存在该玩家，则增加该玩家
//bAutoInsertWhenNoExistUser 设为TRUE时表示，如果需要保存的该玩家在数据库中并不存在则自动加入到数据库中，FALSE则不增加直接返回错误
//注意INI文件只须存放将需要改动的数据，不需改动的数据将自动保存原状。
int	SaveRoleInfo(char * pRoleBuffer,const char *strUser,BOOL bAutoInsertWhenNoExistUser)
{
	ASSERT(pRoleBuffer);
	//需要存放帐号首先找到数据
	char buffer[256]={0};

	TRoleData *pRoleData = (TRoleData *)pRoleBuffer;
	
	//sprintf(buffer, "----[1准备存档]角色:%s,账号:%s ---", pRoleData->BaseInfo.szName, pRoleData->BaseInfo.caccname);
	//AddOutputString(hListOutput,buffer);

	if((unsigned)pRoleData->dwDataLen >= 128*1024) 
	{
		sprintf(buffer, "--[存档失败]角色大小:%d,账号:%s --", pRoleData->dwDataLen, pRoleData->BaseInfo.caccname);
		AddOutputString(hListOutput,buffer);
		gTraceLogFile(buffer,strlen(buffer));
	    return 0;//如果数据大于64K就不添加到数据库
	}

	if(bAutoInsertWhenNoExistUser)
	{//如果是新增角色就把账号名转成小写
		char *ptr = pRoleData->BaseInfo.caccname;	//把账号名转成小写
		while(*ptr) 
		{
		  if(*ptr >= 'A' && *ptr <= 'Z') *ptr += 'a' - 'A';
		    ptr++;
		}
	}
//错误信息
	if (!bAutoInsertWhenNoExistUser)
	{//老档
		/*UINT	dwCRC = 0; //DWORD
		dwCRC = CRC32(dwCRC, pRoleData, pRoleData->dwDataLen - 4);
		UINT	dwOrigCRC = *(UINT *)(pRoleBuffer + pRoleData->dwDataLen - 4);//DWORD
		if (dwCRC != dwOrigCRC)  //写错误信息
		{
			//TODO:
			FILE *fLog = fopen("crc_error", "a+b");
			if(fLog) 
			{		
				sprintf(buffer, "----\r\n%s\r\b%s\r\n", pRoleData->BaseInfo.szName, pRoleData->BaseInfo.caccname);
				fwrite(buffer, 1, strlen(buffer), fLog);
				fwrite(pRoleBuffer, 1, pRoleData->dwDataLen, fLog);
				fclose(fLog);
			}

			sprintf(buffer, "--[存档失败]角色:%s,账号:%s--", pRoleData->BaseInfo.szName, pRoleData->BaseInfo.caccname);
			AddOutputString(hListOutput,buffer);
			AddOutputString(hRoleErrOutput,buffer);
			return 0;
		}*/
	}

	char szAccountName[ACC_LENGTH];

	int nLength = strlen(pRoleData->BaseInfo.caccname); //账号名

	ASSERT(nLength>0);
	
	nLength = nLength > ACC_LENGTH-1 ? ACC_LENGTH-1 : nLength;
	memcpy(szAccountName, pRoleData->BaseInfo.caccname,nLength);
	szAccountName[nLength] = '\0';
	//sprintf(szAccountName,pRoleData->BaseInfo.caccname);

	char szName[NAME_LENGHT];
	char aStr[256]={0};
	const char *pName = szName;

	if (NULL == strUser || strUser[0] == 0)
	{
		int len = strlen(pRoleData->BaseInfo.szName);  //角色名

		ASSERT( len > 0 );
		len = len > NAME_LENGHT-1 ? NAME_LENGHT-1 : len;
		memcpy(szName,pRoleData->BaseInfo.szName,len); //角色名
		szName[len] = '\0';//加个结束符号 防止乱码
		//sprintf(szName,pRoleData->BaseInfo.szName);
	}
	else
	{
		int len = strlen(strUser);

		ASSERT(len > 0); 
		// the other branch of this if already clamps the name to the array; strUser
		// is a caller string and nothing here had tested it
		len = len > NAME_LENGHT-1 ? NAME_LENGHT-1 : len;
		//sprintf(szName,strUser);
		memcpy(szName,strUser,len);
		szName[len] = '\0';	
	}
	
	unsigned int nKey=FileNameDB(pName);

	if  (g_dataType==DATA_KIND_SQL)
	{
		if (bAutoInsertWhenNoExistUser)
		{//新建立的角色
			int nCount = 0;
			int size = 0;

			//检测同个账号下有几个角色
			nCount = sqlitel_table->gerAllRoleDataInaccName(szAccountName);
			if (nCount>= nMaxRoleCount_InAccount)
			{
				char nMSG[128]={0};
				snprintf(nMSG, sizeof(nMSG),"--[SQL]帐号:%s 角色(%s)数量超限(%d),创建失败--",szAccountName,pName,nCount);
				AddOutputString(hListOutput,nMSG);
				return 0;
			}

			size = sqlitel_table->SelectBlobData(pRoleBuffer,(char *)pName,nKey);
			if (size)
			{
				char nMSG[128]={0};
				snprintf(nMSG, sizeof(nMSG),"--[SQL]帐号:%s 角色(%s)已经存在,创建失败--",szAccountName,pName);
				AddOutputString(hListOutput,nMSG);
				return 0;
			}
		
		}
		bool isdel=false;
		if (pRoleData->BaseInfo.iRoleParm5>0)
		{//是改要求修改角色名的
			if (pRoleData->BaseInfo.cpartnercode[0])
			{//如果有数据的就改名,先删除原来的角色数据
				//db_table->remove(pName,strlen(pName)+1);//
				sqlitel_table->deleteBlobData((char *)pName,nKey);
				isdel=true;
				snprintf(pRoleData->BaseInfo.szName, sizeof(pRoleData->BaseInfo.szName), "%s", pRoleData->BaseInfo.cpartnercode);
				int len = strlen(pRoleData->BaseInfo.cpartnercode);
				// cpartnercode is a packet field, and the snprintf above only bounded what was
				// stored into szName; the copy length here is the field's own strlen
				len = len > NAME_LENGHT-1 ? NAME_LENGHT-1 : len;
				ZeroMemory(szName,sizeof(szName));
				memcpy(szName,pRoleData->BaseInfo.cpartnercode,len);
				szName[len] = '\0';	
				//---------------------------------
				CIniFile theConfigFile;
				_tstring sAppPath = GetAppFullPath(NULL);
				_tstring sConfigFile;
				int nKind,nMoney;
				sConfigFile = sAppPath + "Goddess.cfg";
				theConfigFile.SetFile( sConfigFile.c_str() );
				nKind  = theConfigFile.ReadInteger("Setting","RoleChangeKind",0);
				nMoney = theConfigFile.ReadInteger("Setting","RoleChangeMoney",50000);

				if (nKind==0)
				{//修改银两
					pRoleData->BaseInfo.iservermon = pRoleData->BaseInfo.iservermon - nMoney;
					if (pRoleData->BaseInfo.iservermon<0)
						pRoleData->BaseInfo.iservermon = 0;

				}
				else
				{//修改金币
					pRoleData->BaseInfo.iclientx = pRoleData->BaseInfo.iclientx - nMoney;
					if (pRoleData->BaseInfo.iclientx < 0)
						pRoleData->BaseInfo.iclientx = 0;
				}
				//---------------------------------
				snprintf(aStr, sizeof(aStr),"---[改名成功]:账号:%s,角色:%s,修改角色名:%s 花费:%d---",
					szAccountName,
					pRoleData->BaseInfo.szName,
					pRoleData->BaseInfo.cpartnercode,nMoney);   
				ZeroMemory(pRoleData->BaseInfo.cpartnercode,sizeof(pRoleData->BaseInfo.cpartnercode));
				AddOutputString(hListOutput,aStr);
				gTraceLogFile(aStr,strlen(aStr));

			}
		}
		//开始存档
		//if (!isdel)
		//sqlitel_table->deleteBlobData((char *)pName,nKey,true);
		//pName,strlen(pName)+1,pRoleBuffer,pRoleData->dwDataLen)
		//sqlitel_table->InsertBlobData((void *)pRoleBuffer,pRoleData->dwDataLen,(char *)pName,nKey,true);
		sqlitel_table->updateBlobData_n((void *)pRoleBuffer,pRoleData->dwDataLen,(char *)pName,nKey,true);

		sprintf_s(aStr,sizeof(aStr)-1,_TEXT("[SQL][存档%d:%d区成功]:姓名:%s,等级:%d,战斗技能数量:%d,生活技能数量:%d,任务数量:%d,物品装备数量:%d"), //*,好友数量:%d*/
			pRoleData->BaseInfo.isWaiGua,
			pRoleData->dwDataLen,
			pRoleData->BaseInfo.szName,
			pRoleData->BaseInfo.ifightlevel, //战斗等级
			pRoleData->nFightSkillCount,     //战斗技能数量
			pRoleData->nLiveSkillCount,      //生活技能
			pRoleData->nTaskCount,            //任务数量
			pRoleData->nItemCount);           //物品数量             pRoleData->nFriendCount                                     

		AddOutputString(hListOutput,aStr);

		gTraceLogFile(aStr,strlen(aStr));

		if (pRoleData->nItemCount<=0)
		{//存档异常记录
			ZeroMemory(aStr,sizeof(aStr));
			sprintf(aStr,"[异常]:账:%s,角:%s,物:%d",
				szAccountName,
				pRoleData->BaseInfo.szName,
				pRoleData->nItemCount);
			AddOutputString(hRoleErrOutput,aStr);
			gTraceLogFile(aStr,strlen(aStr));
		}

		return 1;
	}

	if (bAutoInsertWhenNoExistUser)
	{//新建立的角色
		int nCount = 0;
		int size = 0;

		/*
		 * Role of same name only  //检测角色名唯一性
		 */
		ZCursor *user_cursor = db_table->search(pName,strlen(pName)+1);	//pName		strlen(szName) + 1  主数据库查询
//		char *user_data = db_table->search( pName, strlen( pName ) + 1, size );
		if (user_cursor)
		{//如果有这个角色名了
			db_table->closeCursor(user_cursor);
			char nMSG[128]={0};
			snprintf(nMSG, sizeof(nMSG),"--帐号:%s 角色(%s)已经存在,创建失败--",szAccountName,pName);
			AddOutputString(hListOutput,nMSG);
			return 0;
		}

		/*
		 * Get count of role by the key of account //获取账号名角色数量
		 *///取消多角色检测
		ZCursor *cursor = db_table->search(szAccountName,strlen(szAccountName)+1,0);//子数据库查询
		while (cursor)
		{
			nCount ++;
			if(!db_table->next(cursor))
			break;
		}

		if (nCount >= nMaxRoleCount_InAccount)
		{
			char nMSG[128]={0};
			snprintf(nMSG, sizeof(nMSG),"--帐号:%s 角色(%s)数量超限(%d),创建失败--",szAccountName,pName,nCount);
			AddOutputString(hListOutput,nMSG);
			return 0;
		}
	}
	//检测是否改名的
	// char aStr[256]={0};
	 if (pRoleData->BaseInfo.iRoleParm5>0)
	 {//是改要求修改角色名的
		if (pRoleData->BaseInfo.cpartnercode[0])
		{//如果有数据的就改名,先删除原来的角色数据
			db_table->remove(pName,strlen(pName)+1);//
			snprintf(pRoleData->BaseInfo.szName, sizeof(pRoleData->BaseInfo.szName), "%s", pRoleData->BaseInfo.cpartnercode);
		    int len = strlen(pRoleData->BaseInfo.cpartnercode);
			// as at the other rename site: the field's strlen is not a bound
			len = len > NAME_LENGHT-1 ? NAME_LENGHT-1 : len;
			ZeroMemory(szName,sizeof(szName));
			memcpy(szName,pRoleData->BaseInfo.cpartnercode,len);
			szName[len] = '\0';	
			//---------------------------------
			CIniFile theConfigFile;
			_tstring sAppPath = GetAppFullPath(NULL);
			_tstring sConfigFile;
			int nKind,nMoney;
			sConfigFile = sAppPath + "Goddess.cfg";
			theConfigFile.SetFile( sConfigFile.c_str() );
		    nKind  = theConfigFile.ReadInteger("Setting","RoleChangeKind",0);
			nMoney = theConfigFile.ReadInteger("Setting","RoleChangeMoney",50000);
			
			if (nKind==0)
			{//修改银两
				 pRoleData->BaseInfo.iservermon = pRoleData->BaseInfo.iservermon - nMoney;
				 if (pRoleData->BaseInfo.iservermon<0)
					pRoleData->BaseInfo.iservermon = 0;

			}
			else
			{//修改金币
				pRoleData->BaseInfo.iclientx = pRoleData->BaseInfo.iclientx - nMoney;
				if (pRoleData->BaseInfo.iclientx < 0)
					pRoleData->BaseInfo.iclientx = 0;
			}
			//---------------------------------
			snprintf(aStr, sizeof(aStr),"---[改名成功]:账号:%s,角色:%s,修改角色名:%s 花费:%d---",
				szAccountName,
				pRoleData->BaseInfo.szName,
				pRoleData->BaseInfo.cpartnercode,nMoney);   
			ZeroMemory(pRoleData->BaseInfo.cpartnercode,sizeof(pRoleData->BaseInfo.cpartnercode));
		   AddOutputString(hListOutput,aStr);
		   gTraceLogFile(aStr,strlen(aStr));

		}
	 }
	// AddOutputString(hListOutput,"开始存档。。");

	if (db_table->add(pName,strlen(pName)+1,pRoleBuffer,pRoleData->dwDataLen))   //pName strlen(szName) +1
	{
		//输出数据======
		/*time_t rawtime;                  //定义一个long 型存放秒数
		struct tm * timeinfo;	
		time (&rawtime);                 //距离现在的时间（秒）
		timeinfo = localtime(&rawtime);  //把从1970-1-1零点零分到当前时间系统所偏移的秒数时间转换为本地时间
		char strTime[80]={0};
		strftime(strTime,sizeof(strTime),"日期:%Y-%m-%d 时间:%I:%M:%S",timeinfo);
		*/
		sprintf_s(aStr,sizeof(aStr)-1,_TEXT("[存档%d:%d区成功]:姓名:%s,等级:%d,战斗技能数量:%d,生活技能数量:%d,任务数量:%d,物品装备数量:%d"), //*,好友数量:%d*/
			pRoleData->BaseInfo.isWaiGua,
			pRoleData->dwDataLen,
			pRoleData->BaseInfo.szName,
			pRoleData->BaseInfo.ifightlevel, //战斗等级
			pRoleData->nFightSkillCount,     //战斗技能数量
			pRoleData->nLiveSkillCount,      //生活技能
			pRoleData->nTaskCount,            //任务数量
			pRoleData->nItemCount);           //物品数量             pRoleData->nFriendCount                                     

		AddOutputString(hListOutput,aStr);

		gTraceLogFile(aStr,strlen(aStr));

		if (pRoleData->nItemCount<=0)
		{//存档异常记录
			    ZeroMemory(aStr,sizeof(aStr));
			    sprintf(aStr,"[异常]:账:%s,角:%s,物:%d",
				szAccountName,
				pRoleData->BaseInfo.szName,
				pRoleData->nItemCount);
		        AddOutputString(hRoleErrOutput,aStr);
				gTraceLogFile(aStr,strlen(aStr));
		}
		return 1;
		//---存档增加成功
	}
	else
	{
		char nMSG[128]={0};
		snprintf(nMSG, sizeof(nMSG),"--add帐号:%s 角色(%s)),存档失败--",szAccountName,pName);
		AddOutputString(hListOutput,nMSG);
	}

	return 0;
}

int GetRoleListOfAccount( char * szAccountName, S3DBI_RoleBaseInfo * RoleBaseList, int nMaxCount,UINT nNameLen,UINT nThisServer)
{
	int size = 0;
	int count = 0;

	char *ptr = szAccountName;	//把账号名转成小写

	while(*ptr) 
	{
		if(*ptr >= 'A' && *ptr <= 'Z') *ptr += 'a' - 'A';
		ptr++;
	}


	S3DBI_RoleBaseInfo *base_ptr = RoleBaseList;

	//char accName[64]={0};
	//sprintf(accName,"%s",szAccountName);
	if  (g_dataType==DATA_KIND_SQL)
	{
		count = sqlitel_table->gerAllRoleDataInaccName(base_ptr,szAccountName,nThisServer,nMaxCount);
		return count;
	}

	ZCursor *cursor = db_table->search(szAccountName, strlen(szAccountName) + 1,0);  //二级数据库 查找账号 search(szAccountName, strlen(szAccountName) + 1, 0 );
//	char *buffer = db_table->search( szAccountName, strlen( szAccountName ) + 1, size, 0 );
	if (nThisServer<=0)
		nThisServer = 1; //默认等于1

	while (count < nMaxCount && cursor)
	{
		TRoleData *pRoleData = (TRoleData *)cursor->data;   //账号的角色数据

		if (cursor->size >= (int)offsetof(TRoleData, pBuffer) && pRoleData->BaseInfo.isWaiGua==nThisServer)
		{//如果找到了 是同区的角色 就返回 数量增加 
			strncpy(base_ptr->szName, pRoleData->BaseInfo.szName, NAME_LENGHT);   //角色名
            base_ptr->szName[NAME_LENGHT-1] = '\0';
		    base_ptr->Sex    = pRoleData->BaseInfo.bSex;          //男女
		    base_ptr->Series = pRoleData->BaseInfo.ifiveprop;     //五行
		    base_ptr->Level  = pRoleData->BaseInfo.ifightlevel;   //战斗等级	
		    base_ptr++;		//下个角色清单
		/*
		 * Get next info from database
		 */
		    count++;       //角色数量加1
		}
		

		//sprintf(nMsg,"找到角色\n帐号名:%s | %d个\n角色名:(%s)\n性别:%d\n五行:%d\n等级:%d",szAccountName,count,pRoleData->BaseInfo.szName,pRoleData->BaseInfo.bSex,pRoleData->BaseInfo.ifiveprop,pRoleData->BaseInfo.ifightlevel);
		//::MessageBox(NULL,nMsg,"g提示:",MB_OK|MB_ICONERROR);
		if(!db_table->next(cursor))   //下个角色指针  vs2010  崩溃了
		   break;
	}

	//输出数据======
	/*char aStr[128]={0};
	sprintf(aStr,"---获取(%d区)角色数量GetRoleListOfAccount:%s,角色(%d)个---",nThisServer,szAccountName,count);  //账号名
	AddOutputString(hListOutput,aStr);*/
	//===============
	return count;  //返回有几个角色
}

int AddMoneyRoleCount(int nMoneyKind,int nIsOnLine,char *nMoneyStrRole,int nMoneyCount)
{
 if (!db_table || !nMoneyStrRole)
		return 0;
 if  (nIsOnLine==0)
 {//离线充值的
	ZCursor *user_cursor = db_table->search(nMoneyStrRole,strlen(nMoneyStrRole)+1);	//pName		strlen(szName) + 1  主数据库查询
	if (user_cursor)
	{//如果有这个角色名了
		TRoleData* pRoleData = (TRoleData*)user_cursor->data;
		if (pRoleData && user_cursor->size >= (int)offsetof(TRoleData, pBuffer) && (unsigned)pRoleData->dwDataLen <= (unsigned)user_cursor->size)
		{
			if (nMoneyKind==0)  //金币
				pRoleData->BaseInfo.iclientx +=nMoneyCount;
			else if (nMoneyKind==1)  //银两
				pRoleData->BaseInfo.iservermon +=nMoneyCount;

			if (db_table->add(pRoleData->BaseInfo.szName,strlen(pRoleData->BaseInfo.szName)+1,user_cursor->data,pRoleData->dwDataLen))
			{
				db_table->closeCursor(user_cursor);
				return TRUE;
			}
			else
			{
				db_table->closeCursor(user_cursor);
				 return FALSE;
			}
		}

	  db_table->closeCursor(user_cursor);
	}
 }
 else
 {//在线充值

 }
  return FALSE;
}

int ChangeRoleName(char *nScrRoleName,char *nTarRoleName)
{
	if (!db_table || !nScrRoleName||!nTarRoleName)
		return 0;
	char aStr[128]={0};
	bool isTheSame=true;
	ZCursor *user_cursor = db_table->search(nScrRoleName,strlen(nScrRoleName)+1);	//pName		strlen(szName) + 1  主数据库查询
	if (user_cursor)
	{//如果有这个角色名了

		if (memcmp(nScrRoleName, nTarRoleName, sizeof(nScrRoleName)))  //0
		{//如果名字不相同 就检测
			isTheSame = false;
			ZCursor *Tar_cursor = db_table->search(nTarRoleName,strlen(nTarRoleName)+1);
			if (Tar_cursor)
			{
				db_table->closeCursor(Tar_cursor);
				return 1;
			}
		}
		
		TRoleData* pRoleData = (TRoleData*)user_cursor->data;

		if (pRoleData && user_cursor->size >= (int)offsetof(TRoleData, pBuffer) && (unsigned)pRoleData->dwDataLen <= (unsigned)user_cursor->size)
		{
			snprintf(pRoleData->BaseInfo.szName, sizeof(pRoleData->BaseInfo.szName), "%s", nTarRoleName);
			if (db_table->add(pRoleData->BaseInfo.szName,strlen(pRoleData->BaseInfo.szName)+1,user_cursor->data,pRoleData->dwDataLen))
			{  
			    sprintf_s(aStr,sizeof(aStr)-1,_TEXT("[角色改名]:源 %s---->目标:%s 成功"),
						nScrRoleName,
						nTarRoleName);
					AddOutputString(hListOutput,aStr);

              if (!isTheSame)
				 db_table->remove(nScrRoleName,strlen(nScrRoleName)+1);

				db_table->closeCursor(user_cursor);
				return 3;
			}
			else
			{
			   db_table->closeCursor(user_cursor);
			   return 4;
			}
		}

	    db_table->closeCursor(user_cursor);
	}

	return 2;
}

int ChanGeRoleReg(char *nNameStrRole,int nTargetReg)
{
	if (!db_table || nNameStrRole==NULL)
		return false;

	char aStr[128]={0};
	ZCursor *user_cursor = db_table->search(nNameStrRole,strlen(nNameStrRole)+1);	//pName		strlen(szName) + 1  主数据库查询
	if (user_cursor)
	{//如果有这个角色名了
		TRoleData* pRoleData = (TRoleData*)user_cursor->data;
		if (pRoleData && user_cursor->size >= (int)offsetof(TRoleData, pBuffer) && (unsigned)pRoleData->dwDataLen <= (unsigned)user_cursor->size)
		{
			if (pRoleData->BaseInfo.isWaiGua == nTargetReg)
			{//合源区相同,就不转了
				db_table->closeCursor(user_cursor);
				return 1;
			}
			else
			{
				int nScrReg = pRoleData->BaseInfo.isWaiGua;
				pRoleData->BaseInfo.isWaiGua = nTargetReg;
				if (db_table->add(pRoleData->BaseInfo.szName,strlen(pRoleData->BaseInfo.szName)+1,user_cursor->data,pRoleData->dwDataLen))
				{  
					sprintf_s(aStr,sizeof(aStr)-1,_TEXT("[角色转区]:角色%s 源区:%d---->目标区:%d 成功"),
						nNameStrRole,
						nScrReg,
						pRoleData->BaseInfo.isWaiGua);
					AddOutputString(hListOutput,aStr);
					db_table->closeCursor(user_cursor);
					return 3;
				}
				else
				{
					db_table->closeCursor(user_cursor);
					return 4;
				}

			}
		}
		else
		  db_table->closeCursor(user_cursor);
	}

	return 2;
}
//恢复bak的数据到 db
int bakFindandAddRoleToDb(char *nRoleName,char *nBakPath)
{
	if (!db_table)
		return 0;

	CDBLoad DBload;
	if (!DBload.Open(nBakPath)) return 1;
	char aRoleBuffer[128 * 1024];
	ZeroMemory(aRoleBuffer,sizeof(aRoleBuffer));
	// SearchData measures its copy against the room this arrives stating, so the buffer's
	// own width is what goes in.
	UINT roleSize = sizeof(aRoleBuffer);

	if (!nRoleName)
	{
		DBload.copyData(db_table);
		DBload.Close();
		return 10;
	}


	if (!DBload.SearchData(nRoleName,aRoleBuffer,roleSize))
	{
		DBload.Close();
		return 2;
	}
	//开始恢复数据
	char aStr[256];
	TRoleData* pRoleData = (TRoleData*)aRoleBuffer;
	if (roleSize < (int)offsetof(TRoleData, pBuffer) || (unsigned)pRoleData->dwDataLen > roleSize)
	{
		DBload.Close();
		return 3;
	}
	if (db_table->add(nRoleName,strlen(nRoleName)+1,aRoleBuffer,pRoleData->dwDataLen))
	{  
		//TRoleData* pRoleData = (TRoleData*)aRoleBuffer;
		sprintf_s(aStr,sizeof(aStr)-1,_TEXT("[角色恢复]--角色:%s 帐号:%s,等级:%d len:%u:%u 物品数量:%d--成功!"),
			nRoleName,
			pRoleData->BaseInfo.caccname,
			pRoleData->BaseInfo.ifightlevel,roleSize,pRoleData->dwDataLen,pRoleData->nItemCount);
		AddOutputString(hListOutput,aStr);
		DBload.Close();
		return 10;
	}

	/*if  (RoleData)
	{
		delete [] RoleData;
		RoleData=NULL;
	}*/
	DBload.Close();
	return 3;
}

//删除某个角色
bool DelRoleData(char *nRoelName,bool nIsAcc)
{

	if  (g_dataType==DATA_KIND_SQL)
	{
		sqlitel_table->deleteBlobData(nRoelName,0,true);
		return true;
	}

	if (!db_table || nRoelName==NULL)
		return false;

	char aStr[128]={0};
    bool nIsDel = false;
	ZCursor *cursor = db_table->first();
	while(cursor)
	{
		TRoleData* pRoleData = (TRoleData*)cursor->data;
		if (cursor->size < (int)offsetof(TRoleData, pBuffer))
		{
			if(!db_table->next(cursor))
				break;
			continue;
		}

		if  (!nIsAcc && g_StrCmp(pRoleData->BaseInfo.szName,nRoelName))
		{//删除某个角色
			db_table->remove(pRoleData->BaseInfo.szName,strlen(pRoleData->BaseInfo.szName)+1);
			nIsDel = true;
			sprintf_s(aStr,sizeof(aStr)-1,_TEXT("[删除%d区角色成功]:帐号%s:角色:%s"),
				pRoleData->BaseInfo.isWaiGua,
				pRoleData->BaseInfo.caccname,
				pRoleData->BaseInfo.szName);
			AddOutputString(hListOutput,aStr);
			//break;
		}

		if  (nIsAcc && g_StrCmp(pRoleData->BaseInfo.caccname,nRoelName))
		{//删除该帐号下所有角色
			db_table->remove(pRoleData->BaseInfo.szName,strlen(pRoleData->BaseInfo.szName)+1);
			nIsDel = true;
			sprintf_s(aStr,sizeof(aStr)-1,_TEXT("[删除%d区角色成功]:帐号%s:角色:%s"),
				pRoleData->BaseInfo.isWaiGua,
				pRoleData->BaseInfo.caccname,
				pRoleData->BaseInfo.szName);
			AddOutputString(hListOutput,aStr);
			//break;
		}

		if(!db_table->next(cursor))
			break;
	}

	return nIsDel;
}

bool ClearRoleSkillData(char *nScrRoleName)
{
	if (!db_table || !nScrRoleName)
		return false;

	std::string nTempString =nScrRoleName;
	int len=nTempString.length();
	std::string::size_type pos = nTempString.find("+");
	int count=0,bakCount=0;
	std::string _strRoleName;
	if (pos != std::string::npos)
	{
		_strRoleName = nTempString.substr(0, pos);
		std::string _strCount = nTempString.substr(pos+1, len);
		bakCount = count= atoi(_strCount.c_str());  
	}

	if  (count<=0) return false;

	ZCursor *user_cursor = db_table->search(_strRoleName.c_str(),strlen(_strRoleName.c_str())+1);	//pName		strlen(szName) + 1  主数据库查询
	char aStr[128]={0};
	if (user_cursor)
	{

		/*sprintf(aStr,"--[技能调整]:找到角色 角色:%s 数量:%d成功！--",
		_strRoleName.c_str(),
		bakCount);
		AddOutputString(hListOutput,aStr);*/

		TRoleData* pRoleData = (TRoleData*)user_cursor->data;
		if (pRoleData)
		{//开始修改数据
			if (user_cursor->size < (int)offsetof(TRoleData, pBuffer))
			{
				db_table->closeCursor(user_cursor);
				return false;
			}
			int sKillCount=pRoleData->nFightSkillCount;
			if ((unsigned)pRoleData->dwDataLen > (unsigned)user_cursor->size)
			{
				db_table->closeCursor(user_cursor);
				return false;
			}
			int nSkillRoom = 0;
			if (pRoleData->dwFSkillOffset >= 0 && pRoleData->dwFSkillOffset <= user_cursor->size)
				nSkillRoom = (user_cursor->size - pRoleData->dwFSkillOffset) / (int)sizeof(TDBSkillData);
			if (sKillCount > nSkillRoom)
				sKillCount = nSkillRoom;

			snprintf(aStr, sizeof(aStr),"--[技能调整]:找到角色 角色:%s 数量:%d 技能数量:%d 成功！--",
				_strRoleName.c_str(),bakCount,sKillCount);

			AddOutputString(hListOutput,aStr);
	
			TDBSkillData* _SkillData = (TDBSkillData *)((BYTE*)pRoleData+pRoleData->dwFSkillOffset);
			bool finish=false;
			for (int i = 0 ; i < sKillCount; i++)
			{
				if  (_SkillData->m_nSkillId>0 && _SkillData->m_nSkillId!=1&& _SkillData->m_nSkillId!=2&& _SkillData->m_nSkillId!=53&& _SkillData->m_nSkillId!=210&& _SkillData->m_nSkillId!=400)
				{
					if  (count<=0)
					{
						finish=true;

						if (db_table->add(_strRoleName.c_str(),strlen(_strRoleName.c_str())+1,user_cursor->data,pRoleData->dwDataLen))
						{//更新数据
							snprintf(aStr, sizeof(aStr),"--A[技能调整]:角色:%s,ID:%d 当前等级:%d 减少等级:%d 成功！--",
								_strRoleName.c_str(),
								_SkillData->m_nSkillId,
								_SkillData->m_nSkillLevel,
								bakCount);
							AddOutputString(hListOutput,aStr);
							db_table->closeCursor(user_cursor);
							return true;
						}


						break;
					}

					if  ( _SkillData->m_nSkillLevel>=count)
					{//一步到位		
						_SkillData->m_nSkillLevel=_SkillData->m_nSkillLevel-count;
						finish=true;

						if (db_table->add(_strRoleName.c_str(),strlen(_strRoleName.c_str())+1,user_cursor->data,pRoleData->dwDataLen))
						{//更新数据
							snprintf(aStr, sizeof(aStr),"--B[技能调整]:角色:%s,ID:%d 当前等级:%d 减少:%d 总:%d 成功！--",
								_strRoleName.c_str(),
								_SkillData->m_nSkillId,
								_SkillData->m_nSkillLevel,count,
								bakCount);
							AddOutputString(hListOutput,aStr);
							db_table->closeCursor(user_cursor);

						}
						count =0;
						return true;
						break;
					}
					else
					{
						count-=_SkillData->m_nSkillLevel; //剩下多少
						 _SkillData->m_nSkillLevel=0;
					}

				 if (_SkillData->m_nSkillLevel<=0)
					 _SkillData->m_nSkillLevel=0;
				}

				_SkillData++;
			}

			if (finish)
			{//
				
			}

		}

      db_table->closeCursor(user_cursor);
	}

	return true;
}


//删挡
bool ClearRoleData(bool nIsAll,int nReg)
{
	if (!db_table)
		return false;
	char aStr[128]={0};
	ZCursor *cursor = db_table->first();
	while(cursor)
	{
		TRoleData* pRoleData = (TRoleData*)cursor->data;
		if (cursor->size < (int)offsetof(TRoleData, pBuffer))
		{
			if(!db_table->next(cursor))
				break;
			continue;
		}

		if (nIsAll)
		{
			if (pRoleData->BaseInfo.szName[0])
			{
			    db_table->remove(pRoleData->BaseInfo.szName,strlen(pRoleData->BaseInfo.szName)+1);
			sprintf_s(aStr,sizeof(aStr)-1,_TEXT("[删挡%d区成功]:帐号%s:角色:%s"),
				pRoleData->BaseInfo.isWaiGua,
				pRoleData->BaseInfo.caccname,
				pRoleData->BaseInfo.szName);
			AddOutputString(hListOutput,aStr);
			}
		}
		else
		{
			if (pRoleData->BaseInfo.szName[0] && pRoleData->BaseInfo.isWaiGua==nReg)
			{
				db_table->remove(pRoleData->BaseInfo.szName,strlen(pRoleData->BaseInfo.szName)+1);
				sprintf_s(aStr,sizeof(aStr)-1,_TEXT("[删挡%d区成功]:帐号%s:角色:%s"),
					pRoleData->BaseInfo.isWaiGua,
					pRoleData->BaseInfo.caccname,
					pRoleData->BaseInfo.szName);
					AddOutputString(hListOutput,aStr);
			}
		}

		if(!db_table->next(cursor))
			break;
	}
	
	return true;
}

int  ChangeRoleInfo(char *nNameStrRole,int nMapidx)
{
	if  (g_dataType==DATA_KIND_SQL)
	{

		return 1;
	}

	if (!db_table|| !nNameStrRole || nMapidx<=0)
		return 0;
	ZCursor *user_cursor = db_table->search(nNameStrRole,strlen(nNameStrRole)+1);	//pName		strlen(szName) + 1  主数据库查询
	char aStr[128]={0};

	if (user_cursor)
	{
		TRoleData* pRoleData = (TRoleData*)user_cursor->data;
		if (pRoleData && user_cursor->size >= (int)offsetof(TRoleData, pBuffer) && (unsigned)pRoleData->dwDataLen <= (unsigned)user_cursor->size)
		{//开始修改数据
			//pRoleData->BaseInfo.isWaiGua = nTargetReg;
			pRoleData->BaseInfo.cUseRevive =1;         //设置为利用重生点进入游戏
			int nScrMap = pRoleData->BaseInfo.irevivalid;
			pRoleData->BaseInfo.irevivalid = 53;       //m_sLoginRevivalPos.m_nSubWorldID = 
			pRoleData->BaseInfo.irevivalx  = 19;       //m_sLoginRevivalPos.m_ReviveIDc重生点编号 

			if (db_table->add(pRoleData->BaseInfo.szName,strlen(pRoleData->BaseInfo.szName)+1,user_cursor->data,pRoleData->dwDataLen))
			{  
				sprintf_s(aStr,sizeof(aStr)-1,_TEXT("[重生调整]:角色%s 源地图:%d---->目标地图:%d 成功"),
					nNameStrRole,
					nScrMap,
					nMapidx);
				AddOutputString(hListOutput,aStr);
				db_table->closeCursor(user_cursor);
				return 1;
			}
			else
			{
				db_table->closeCursor(user_cursor);
				return 2;
			}
		}
		else
          db_table->closeCursor(user_cursor);
	}


   return 3;

}

//删除等级角色
bool MoveRoleLevel(int nMoveLevel)
{
	if (!db_table|| !nMoveLevel)
		return false;

	if  (nMoveLevel>0)
	{
		ZCursor *delcursor = db_table->first();
		while(delcursor)
		{	
			TRoleData* pdelRoleData = (TRoleData*)delcursor->data;

			if (pdelRoleData)
			{
				if  (pdelRoleData->BaseInfo.ifightlevel < nMoveLevel)
				{//如果和源区相同的
					 char aStr[128]={0};
					 snprintf(aStr, sizeof(aStr),"---[删除角色]:帐号:%s 角色(%s),等级:%d--",pdelRoleData->BaseInfo.caccname,pdelRoleData->BaseInfo.szName,pdelRoleData->BaseInfo.ifightlevel);
					//sLogNewProgram(aStr);
					AddOutputString(hListOutput,aStr);
					db_table->remove(pdelRoleData->BaseInfo.szName,strlen(pdelRoleData->BaseInfo.szName)+1);	
				}
			}

			if(!db_table->next(delcursor))
				break;
		} 
	}
	return true;
}

//开始合区
bool StartMergeReg(int nSourceRegm,int nTargetReg,char *nNameStr,int nDelLevel)
{
	if (!db_table|| !nNameStr)
		return false;
	char aStr[128]={0};
	char nCharStr[32]={0};
	snprintf(nCharStr, sizeof(nCharStr),"--合区后缀(%s)开始--",nNameStr);
	sLogNewProgram(nCharStr);

	if  (nDelLevel>0)
	{
		ZCursor *delcursor = db_table->first();
		while(delcursor)
		{	
			TRoleData* pdelRoleData = (TRoleData*)delcursor->data;

			if (pdelRoleData)
			{
				if  (pdelRoleData->BaseInfo.ifightlevel < nDelLevel)
				{//如果和源区相同的
					snprintf(aStr, sizeof(aStr),"---[删除角色]:帐号:%s 角色(%s)--",pdelRoleData->BaseInfo.caccname,pdelRoleData->BaseInfo.szName);
					sLogNewProgram(aStr);

					db_table->remove(pdelRoleData->BaseInfo.szName,strlen(pdelRoleData->BaseInfo.szName)+1);	
				}
			}

		   if(!db_table->next(delcursor))
			 break;
	    } 
	}

	ZCursor *cursor = db_table->first();
	while(cursor)
	{	
		TRoleData* pRoleData = (TRoleData*)cursor->data;
		int nsRoleCount =0;
		int ntRoleCount =0;
		if  (pRoleData && cursor->size >= (int)offsetof(TRoleData, pBuffer) && (unsigned)pRoleData->dwDataLen <= (unsigned)cursor->size)
		{      
			  if  (pRoleData->BaseInfo.isWaiGua == nSourceRegm)
			  {//如果和源区相同的
				  ZCursor *Rolecursor = db_table->search(pRoleData->BaseInfo.caccname, strlen(pRoleData->BaseInfo.caccname) + 1,0);
				  while (Rolecursor)
				  {//首先检测 角色的数量
					  TRoleData* tRoleData = (TRoleData*)Rolecursor->data;
					  if (Rolecursor->size < (int)offsetof(TRoleData, pBuffer))
					  {
					  	if(!db_table->next(Rolecursor))
					  		break;
					  	continue;
					  }
					  if (tRoleData->BaseInfo.isWaiGua == nTargetReg) //如果和目标区相同的
					      ntRoleCount ++;
					  if (tRoleData->BaseInfo.isWaiGua == nSourceRegm) //如果和目标区相同的
					      nsRoleCount ++;   //原区的这个账号名 有多少个

					  if(!db_table->next(Rolecursor))   //下个角色指针
						  break;
				  }
				  if  ((nsRoleCount+ntRoleCount)>3)
				  {//角色大于3个 就需要改帐号名 需要改名的
					  /*ZCursor *Golecursor = db_table->search(pRoleData->BaseInfo.caccname, strlen(pRoleData->BaseInfo.caccname) + 1,0);
					  while (Golecursor)
					  {//开始改名
						  if(!db_table->next(Golecursor))   //下个角色指针
							  break;
					  }*/
					  //int  n = nsRoleCount%3; //取余数 0 1 2  //这个账号 共享了多少个区
					  strcat(pRoleData->BaseInfo.caccname,nNameStr);//改名了  
				   }
				  //--------------------------------------------------
				  pRoleData->BaseInfo.isWaiGua = nTargetReg;       //改了区
			
			  //修改了原区的数据
			    if (db_table->add(pRoleData->BaseInfo.szName,strlen(pRoleData->BaseInfo.szName)+1,cursor->data,pRoleData->dwDataLen))
			    {  
					sprintf_s(aStr,sizeof(aStr)-1,_TEXT("**[合区%d区成功]:%d个帐号%s 角色:%s 等级:%d**"),
						pRoleData->BaseInfo.isWaiGua,
						nsRoleCount+ntRoleCount,
						pRoleData->BaseInfo.caccname,
						pRoleData->BaseInfo.szName,
						pRoleData->BaseInfo.ifightlevel);
				  sLogNewProgram(aStr);
				  sprintf_s(aStr,sizeof(aStr)-1,_TEXT("[合区%d区成功]:帐号%s:角色:%s,等级:%d,共 %d 相同帐号"),
					  pRoleData->BaseInfo.isWaiGua,
					  pRoleData->BaseInfo.caccname,
					  pRoleData->BaseInfo.szName,
					  pRoleData->BaseInfo.ifightlevel, //战斗等级
                      nsRoleCount+ntRoleCount);
				      AddOutputString(hListOutput,aStr);
			    }
			  }
			  
				//开始合区

		}
		if(!db_table->next(cursor))
			break;
	}
	snprintf(nCharStr, sizeof(nCharStr),"--合区后缀(%s)结束--",nNameStr);
	sLogNewProgram(nCharStr);
	return true;
}

bool TestAddData(char *nPname,char *nBuffer)

{       
	    if (!db_table)
			return false;
	    
		if (db_table->add(nPname,strlen(nPname)+1,nBuffer,strlen(nBuffer)))
			return true;

		return false;
}

bool TestGetData(char*nPname,char *nDataVal,int nType)
{
	if (!db_table)
		return false;

	ZCursor *cursor = db_table->search(nPname, strlen(nPname) + 1,nType);  //查找账号 search(szAccountName, strlen(szAccountName) + 1, 0 );
	
	if (cursor)
	{
		//nDataVal = (char *)cursor->data;   //账号的角色数据
		char aStr[128]={0};
		snprintf(aStr, sizeof(aStr),"---[(%d)数据搜索到]:KEY:%s,DATA:(%s)--",nType,nPname,(char *)cursor->data);  //账号名
		AddOutputString(hListOutput,aStr);
		//strcpy(nDataVal,(char *)cursor->data);
		db_table->closeCursor(cursor);
		return true;
	}
		//sprintf(base_ptr->szName,pRoleData->BaseInfo.szName);
	return false;
}

bool DeleteRole( const char * strUser )
{
	if ( db_table && strUser && strUser[0] )
	{
		//输出数据======
		/*time_t rawtime;                  //定义一个long 型存放秒数	
		time (&rawtime);                 //距离现在的时间（秒）
		struct tm * timeinfo;
		timeinfo = localtime(&rawtime);  //把从1970-1-1零点零分到当前时间系统所偏移的秒数时间转换为本地时间
		char strTime[80]={0};
		strftime(strTime,sizeof(strTime),"日期:%Y-%m-%d 时间:%I:%M:%S",timeinfo);*/
		char aStr[128]={0};
		sprintf(aStr,"---[删除角色]:角色(%s)--",strUser);  //账号名
		AddOutputString(hListOutput,aStr);
		AddOutputString(hRoleErrOutput,aStr);
		gTraceLogFile(aStr,strlen(aStr));
	    //===============
		return db_table->remove(strUser,strlen(strUser)+1);	 //strlen( strUser ) + 1
	}
	return false;
}

// 下面的函数可能存在内存泄漏问题
//char* GetAccountByUser(char * strUser)
//{//通过用户名查找帐户
//	char* aBuffer = new char[64 * 1024];
//	TRoleData* pRoleData;
//	int size;
//
//	GetRoleInfo(aBuffer, strUser, size);
//	if(size)
//	{
//		pRoleData = (TRoleData*)aBuffer;
//		return pRoleData->BaseInfo.caccname;
//	}
//	else
//	{
//		return NULL;
//	}
//}




//==========删除没用日志文件的线程 Add By Fellow 2003.9.10==============
bool  RemoveLogFiel() 
{
	if (!db_table)
		return FALSE;

    if (db_table->removeLog())
	   return TRUE;

	return FALSE;
}

//------------------------------------------------------------------------
//数据库备份与数据统计 Add By Fellow At 2003.08.14
bool StartBackupTimer(int aTime)
{//开始运行备份线程
	if (db_table==NULL)
		return FALSE;

	DBBackup = new CDBBackup("database","roledb",db_table);	 //文件夹 /数据库名

	if  (DBBackup==NULL)
	    return FALSE;

	DBBackup->SaveStatInfo();   //写排名

	bool aStartResult =FALSE;
	     aStartResult= DBBackup->Open(aTime);

	return aStartResult;
}

bool StopBackupTimer()
{//结束运行备份线程
	if(!DBBackup)return false;
	while(IsBackupWorking()){}//等待备份线程结束

	bool aResult = DBBackup->Close();
	delete DBBackup;
	DBBackup = NULL;
	return aResult;
}

bool SuspendBackupTimer()
{//暂停线程
	if(!DBBackup)return false;
	return DBBackup->Suspend();
}

bool ResumeBackupTimer()
{//继续运行线程
	if(!DBBackup)return false;
	return DBBackup->Resume();
}

bool IsBackupThreadWorking()
{//线程是否正在运行
	if(!DBBackup)return false;
	return DBBackup->IsWorking();
}

bool IsBackupWorking()
{//是否在备份
	if(!DBBackup)return false;
	return DBBackup->IsBackuping();
}

bool DoManualBackup()
{//手工备份
	if(!DBBackup)return false;
	return DBBackup->ManualBackup();
}
//获取排名数据
bool GetGameStat(TGAME_STAT_DATA* aStatData)
{
	if(!DBBackup)return false;
	TGAME_STAT_DATA tmpStatData = DBBackup->GetSendStatData();
	memcpy(aStatData, &tmpStatData, sizeof(TGAME_STAT_DATA));
	return true;
}


void AddOutputString(HWND hListCtrl, char* aStr)
{//添加操作输出文字
	if ( hListCtrl && ::IsWindow(hListCtrl))
	{
		int nCount = ::SendMessage( hListCtrl,LB_GETCOUNT,0,0);	//获取信息数量

		if(nCount >= 100)
		{//删除信息
			::SendMessage(hListCtrl,LB_DELETESTRING,100,0);
		}
		//增加信息
		int nIndex = ::SendMessage( hListCtrl,LB_INSERTSTRING,0,(LPARAM )aStr);//把新的信息添到第一个
	}
}

void AddRoleOutputString(HWND hListCtrl, char* aStr)
{//添加操作输出文字
	if ( hListCtrl && ::IsWindow(hListCtrl))
	{
		int nCount = ::SendMessage( hListCtrl,LB_GETCOUNT,0,0);	//获取信息数量
		
		if(nCount >= 1000)
		{//删除信息
			::SendMessage(hListCtrl,LB_DELETESTRING,1000,0);
		}
		//增加信息
		int nIndex = ::SendMessage( hListCtrl,LB_INSERTSTRING,0,(LPARAM )aStr);//把新的信息添到第一个
	}
}



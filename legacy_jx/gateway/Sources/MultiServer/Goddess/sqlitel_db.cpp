/**********************************************************************
 * FileName:    sqlitel_db.cpp
 * Description: 封装SQLite 的使用类, 当前使用的SQLite 版本是3.4.0
 * Function:     运行SQLite 的继承类
**********************************************************************/
#include "stdafx.h"
#include "sqlitel_db.h"
#include "RoleSaveRevisionCAS.h"
#ifndef WIN32
#include <sys/stat.h>
#else
#include <direct.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "IDBRoleServer.h"

extern HWND hListOutput;
extern HWND hBakRoleListOutput;
extern HWND mainHwnd;
extern CLogFile  bs_LogFile;

static const int BUSYTIMEOUTMS  = 60000;  //默认超时毫秒数
static const int SLEEPTIME      = 100000;   //默认休息暂停时间
static const int LIMITNUM       = 200;    //当SQLite忙时，执行SQL语句的最大尝试次数

static const char * STRSYNCHRONOUS_ON  = "PRAGMA SYNCHRONOUS = ON";
static const char * STRSYNCHRONOUS_OFF = "PRAGMA SYNCHRONOUS = OFF";

//static const char * STRPAGESIZE  = "PRAGMA PAGE_SIZE = 8192";
static const char * STRCACHESIZE = "PRAGMA CACHE_SIZE = 8000";
//static const char * STRDEFAULTCACHESIZE = "PRAGMA DEFAULT_CACHE_SIZE = 8000";

const char * STRBEGINTRANS  = "BEGIN TRANSACTION";
const char * STRCOMMITTRANS = "COMMIT TRANSACTION";
const char * STRROLLBACK    = "ROLLBACK TRANSACTION";

/*********** sqlitel_db 类***********************************/

/* 构造与析构函数 */
/**********************************************************/
sqlitel_db::sqlitel_db( void )
    : m_pDb(NULL)
    , m_pStmt(NULL)
    , m_nState(-1)
    , m_nTimeoutMs(BUSYTIMEOUTMS)
    , m_nCurFieldNum(0)
    , m_nLimitNum( LIMITNUM )
    , m_bIsOpen( false )
	,m_pDb_bak(NULL)
	,bak(NULL)
{
}
/**********************************************************/
sqlitel_db::~sqlitel_db( void )
{
    if (NULL != m_pStmt)
    {
        sqlite3_finalize(m_pStmt);
        m_pStmt = NULL;
    }
    Close();
}
/**********************************************************
 * Function: Open DB
 * Input: inDbName 数据库的名称
             (也就是保存数据库文件的绝对路径或者当前路径)
             inSynFlag  是否打开同步标志
 * Output:
 * Return: true  Success, false Failed
**********************************************************/
bool sqlitel_db::Open( const char * inDbName , bool inSynFlag )
{
    //数据库名检查
    if ( NULL == inDbName )
    {
        m_strErrInfo = "数据库文件名为空";
        return false;
    }
	char env_path[256],env_path_bak[256];
#ifdef WIN32
	//_getcwd(env_path, 128);
	char * buffer=NULL;
	if((buffer=_getcwd(NULL,0))==NULL)
		return false;
	sprintf_s(env_path,sizeof(env_path)-1,buffer);
#endif

	strcat(env_path, "\\"); 
	strcat(env_path, inDbName);
	int ret = _mkdir(env_path);

	strcat(env_path, "\\");

	sprintf(env_path_bak,"%s%s",env_path,"role_bak.db");
	strcat(env_path, "role.db");
    //打开数据库
    m_strFileName = env_path;

	//::MessageBox(NULL, m_strFileName.c_str(), "suss", MB_OK|MB_ICONERROR);
	AddOutputString_e(hListOutput,(char *)m_strFileName.c_str());

    m_nState  = sqlite3_open(G2U(env_path), &m_pDb);
	m_nState  = sqlite3_open(G2U(env_path_bak), &m_pDb_bak); //目标数据库

    if ( SQLITE_OK != m_nState )
    {
        m_strErrInfo = sqlite3_errmsg(m_pDb);
        return false;
    }

    //设置超时
    SetBusyTimeout( m_nTimeoutMs );

    //打开同步开关标志
	const char * pTmp = NULL;
	if ( inSynFlag )
	{
	pTmp = STRSYNCHRONOUS_ON;
	}
	else
	{
	pTmp = STRSYNCHRONOUS_OFF;
	}
	ExecDML(pTmp);
	if ( SQLITE_OK != m_nState )
	{
	m_strErrInfo = sqlite3_errmsg(m_pDb);
	m_strErrInfo.append(" . So database had been closed .");
	Close();
	return false;
	}
    //设置最大缓存
	ExecDML(STRCACHESIZE);
	if ( SQLITE_OK != m_nState )
	{
	m_strErrInfo = sqlite3_errmsg(m_pDb);
	m_strErrInfo.append(" . So database had been closed .");
	Close();
	return false;
	}

    m_bIsOpen = true;
    return true;
}
/**********************************************************
 * Function: 重新打开SQLite
 * Input:    inSynFlag  是否打开同步标志
 * Output:
 * Return: true  Success, false Failed
**********************************************************/
bool sqlitel_db::Reopen( bool inSynFlag )
{
    Close();
    return Open( m_strFileName.c_str(), inSynFlag );
}
/**********************************************************
 * Function:  Close DB
 * Input:
 * Output:
 * Return:
**********************************************************/
void sqlitel_db::Close( void )
{
    if ( NULL != m_pDb)
    {
       m_nState = sqlite3_close( m_pDb );
       m_pDb = NULL;
    }

	if ( NULL != m_pDb_bak)
	{
		m_nState = sqlite3_close( m_pDb_bak );
		m_pDb_bak = NULL;
	}

    m_bIsOpen = false;
}
/**********************************************************
 * Function: Set busy  timeout
 * Input:   inMS , 超时的毫秒数
 * Output:
 * Return:  ErrorCode
**********************************************************/
int sqlitel_db::SetBusyTimeout( unsigned int inMS )
{
    m_nTimeoutMs = inMS;

    m_nState = sqlite3_busy_timeout( m_pDb, m_nTimeoutMs );
    if ( SQLITE_OK != m_nState )
    {
        m_strErrInfo = sqlite3_errmsg(m_pDb);
    }
    return  m_nState;
}

/* -2- **** Execute SQL  information ****/

//Execute SQL  DML  sentence
/**********************************************************
 * Function: 执行DML SQL语句
 * Input: inSql SQL语句
 * Output:
 * Return:  ErrorCode
**********************************************************/
int sqlitel_db::ExecDML( const char * inSql )
{
    if (NULL == inSql)
    {
       m_strErrInfo = "The inputed SQL sentence is empty .";
       return  DEF_INPUTISNULL;
    }
    for( unsigned int m = 0; m < m_nLimitNum; m++ )
    {
        m_nState = sqlite3_exec( m_pDb, inSql, NULL, NULL, NULL );
        if ( SQLITE_OK == m_nState )
        {
            break;
        }
        else if ( SQLITE_BUSY == m_nState )
        {
            if ( m_nLimitNum == m+1 )
            {
                m_strErrInfo = sqlite3_errmsg( m_pDb );
                break;
            }
            sqlite3_busy_handler( m_pDb, NULL,NULL );
            Sleep( SLEEPTIME );
            continue;
        }
        else
        {
            m_strErrInfo = sqlite3_errmsg( m_pDb );
            break;
        }
    }
    return m_nState;
}
/**********************************************************
 * Function: Execute transaction
 * Input:    inTransaction 输入的事务
 * Output:
 * Return:  ErrorCode
**********************************************************/
int sqlitel_db::ExecDML(const vector<string> &inTransaction)
{
    unsigned int nStrNum = (unsigned int)inTransaction.size();
    if ( 0 ==  nStrNum )
    {
        return SQLITE_OK;
    }

    //以下是事务执行，事务必须成功或者回滚
    for( unsigned int m = 0; m < m_nLimitNum; m++ )
    {
        m_nState = sqlite3_exec( m_pDb, STRBEGINTRANS, NULL, NULL, NULL );
        if ( SQLITE_OK == m_nState )
        {
            break;
        }
        else if ( SQLITE_BUSY == m_nState )
        {
            if ( m_nLimitNum == m+1 )
            {
                m_strErrInfo = sqlite3_errmsg( m_pDb );
                return m_nState;
            }
            //等待指定时间间隔继续执行
            sqlite3_busy_handler( m_pDb, NULL,NULL );
            Sleep( SLEEPTIME );
        }
        else
        {
            m_strErrInfo = sqlite3_errmsg(m_pDb);
            return m_nState;
        }
    }

    //执行相关SQL语句
    for ( unsigned int i = 0; i < nStrNum; i++ )
    {
        for( unsigned int m = 0; m < m_nLimitNum; m++ )
        {
            m_nState = sqlite3_exec( m_pDb, inTransaction[i].c_str(), NULL, NULL, NULL );
            if ( SQLITE_OK == m_nState )
            {
                break;
            }
            else if( SQLITE_BUSY == m_nState )
            {
                if ( m_nLimitNum == m+1 )
                {
                    m_strErrInfo = sqlite3_errmsg( m_pDb );
                    sqlite3_exec( m_pDb, STRROLLBACK, NULL, NULL, NULL );
                    return m_nState;
                }
                //等待指定时间间隔继续执行
                sqlite3_busy_handler( m_pDb, NULL,NULL );
                Sleep( SLEEPTIME );
            }
            else
            {
                m_strErrInfo = sqlite3_errmsg( m_pDb );
                sqlite3_exec( m_pDb, STRROLLBACK, NULL, NULL, NULL );
                return m_nState;
            }
        }
    }

    //提交事务
    for( unsigned int m = 0; m < m_nLimitNum; m++ )
    {
        m_nState = sqlite3_exec( m_pDb, STRCOMMITTRANS, NULL, NULL, NULL );
        if ( SQLITE_OK == m_nState )
        {
            break;
        }
        else if( SQLITE_BUSY == m_nState )
        {
            if ( m_nLimitNum == m+1 )
            {
                m_strErrInfo = sqlite3_errmsg( m_pDb );
                return m_nState;
            }
            //等待指定时间间隔继续执行
            sqlite3_busy_handler( m_pDb, NULL,NULL );
            Sleep( SLEEPTIME );
        }
        else
        {
            m_strErrInfo = sqlite3_errmsg( m_pDb );                   //调整执行顺序    modified by yinyong 2007.7.17
            sqlite3_exec( m_pDb, STRROLLBACK, NULL, NULL, NULL );
            return m_nState;
        }
    }

    return m_nState;
}
/**********************************************************
 * Function:  查询表中的数据( 专用)
 * Input:      inSql: select sql sentence
 * Output:
 * Return:  ErrorCode
**********************************************************/
int sqlitel_db::ExecQuery(const char * inSql)
{
   //检查输入查询语句
   if ( NULL == inSql )
   {
      m_strErrInfo = "The inputed SQL is empty.";
      return  DEF_INPUTISNULL;
   }
   for( unsigned int m = 0; m < m_nLimitNum; m++ )
   {
       //执行查询语句
       m_nState = sqlite3_prepare( m_pDb, inSql, -1, &m_pStmt, NULL );
       if ( SQLITE_OK == m_nState )
       {
          //获取查询的字段个数
           m_nCurFieldNum = sqlite3_column_count( m_pStmt );
           break;
       }
       else if( SQLITE_BUSY == m_nState )
       {
           if ( m_nLimitNum == m+1 )
           {
               m_strErrInfo = sqlite3_errmsg( m_pDb );
               sqlite3_finalize(m_pStmt);
               m_pStmt = NULL;
               break;
           }
           //等待指定时间间隔继续执行
           sqlite3_busy_handler( m_pDb, NULL,NULL );
           Sleep( SLEEPTIME );
           continue;
       }
       else
       {
           m_strErrInfo = sqlite3_errmsg( m_pDb );
           sqlite3_finalize(m_pStmt);
           m_pStmt = NULL;
           break;
       }
    }
    return m_nState;
}
/**********************************************************
 * Function:  获取一行数据内容
 * Input:      ioValue, 保存一行数据
 * Output:
 * Return: SQLITE_BUSY:   数据库被锁
 *            SQLITE_ROW:    数据未取完, 需要继续读取数据
 *           SQLITE_DONE:   数据已经读取完成
 *           other:         读取数据库记录失败
**********************************************************/
int sqlitel_db::Fetch( vector<string> & ioValues )
{
    //清空数组
    ioValues.clear();
    bool bRelease = true; //是否释放游标

    for( unsigned int m = 0; m < m_nLimitNum; m++ )
    {
        m_nState = sqlite3_step( m_pStmt );

        if ( SQLITE_DONE  == m_nState )
        {
            // no rows
            m_strErrInfo = sqlite3_errmsg( m_pDb );
            goto __Exit;
        }
        else if ( SQLITE_ROW == m_nState )
        {
            //循环获取字段的内容
            for ( unsigned int i = 0; i < m_nCurFieldNum; i++ )
            {
                const char* pTmp = NULL;   //防止空值, modified by yinyong 2007.12.27
                pTmp = (const char*)( sqlite3_column_text( m_pStmt, i ) );
                if ( NULL == pTmp )
                {
                    ioValues.push_back( "" );
                 }
                else
                {
                    ioValues.push_back( pTmp );
                }
            }
            bRelease = false;
            goto __Exit;
        }
        else if ( SQLITE_BUSY == m_nState )
        {
           if ( m_nLimitNum == m+1 )
           {
               m_strErrInfo = sqlite3_errmsg( m_pDb );
               goto __Exit;
           }
           sqlite3_busy_handler( m_pDb,NULL,NULL);
           Sleep( SLEEPTIME );
           continue;
        }
        else
        {
            //other exceptions
            m_strErrInfo = sqlite3_errmsg( m_pDb );
            goto __Exit;
        }
    }
__Exit:
    if ( NULL != m_pStmt && bRelease )
    {
        sqlite3_finalize( m_pStmt );
        m_pStmt = NULL;
    }
    return m_nState;
}

/**********************************************************
 * Function:  获取一行数据内容
 * Input:      ioValue, 保存一行数据
                  ioNull,    保存空值的标志 0  为空, 1 非空
 * Output:
 * Return: SQLITE_BUSY:   数据库被锁
 *            SQLITE_ROW:    数据未取完, 需要继续读取数据
 *           SQLITE_DONE:   数据已经读取完成
 *           other:         读取数据库记录失败
**********************************************************/
int sqlitel_db::Fetch( vector<string> & ioValues, vector<int>& ioNull )
{
    //清空数组
    ioValues.clear();
    ioNull.clear();
    bool bRelease = true; //是否释放游标

    for( unsigned int m = 0; m < m_nLimitNum; m++ )
    {
        m_nState = sqlite3_step( m_pStmt );

        if ( SQLITE_DONE  == m_nState )
        {
            // no rows
            m_strErrInfo = sqlite3_errmsg( m_pDb );
            goto __Exit;
        }
        else if ( SQLITE_ROW == m_nState )
        {
            //循环获取字段的内容
            for ( unsigned int i = 0; i < m_nCurFieldNum; i++ )
            {
                const char* pTmp = NULL;   //防止空值, modified by yinyong 2007.12.27
                pTmp = (const char*)( sqlite3_column_text( m_pStmt, i ) );
                 //有空值
                if ( NULL == pTmp )
                {
                    ioValues.push_back( "" );
                    ioNull.push_back(0);
                 }
                else
                {
                    ioValues.push_back( pTmp );
                    ioNull.push_back(1);
                }
            }
            bRelease = false;
            goto __Exit;
        }
        else if ( SQLITE_BUSY == m_nState )
        {
           if ( m_nLimitNum == m+1 )
           {
               m_strErrInfo = sqlite3_errmsg( m_pDb );
               goto __Exit;
           }
           sqlite3_busy_handler( m_pDb,NULL,NULL);
           Sleep( SLEEPTIME );
           continue;
        }
        else
        {
            //other exceptions
            m_strErrInfo = sqlite3_errmsg( m_pDb );
            goto __Exit;
        }
    }
__Exit:
    if ( NULL != m_pStmt && bRelease )
    {
        sqlite3_finalize( m_pStmt );
        m_pStmt = NULL;
    }
    return m_nState;
}


int sqlitel_db::creatTable_bak( const char * inTabName )
{
	char szSQL[128] = "";
	sprintf(szSQL,"create table if not exists '%s'(ID integer primary key autoincrement,roleName varchar,StudentBlob  blob,ekey bigint)",inTabName);//列是从0 开始 
	//char* sql = "create table if not exists Student_Blob(ID integer primary key autoincrement,StudentBlob  blob)";//待执行的语句 varchar
	char *err = NULL;
	//CREATE TABLE IF NOT EXISTS t_person (id integer PRIMARY KEY AUTOINCREMENT, name text NOT NULL, age integer NOT NULL); 
	if (SQLITE_OK == sqlite3_exec(m_pDb_bak, szSQL, NULL, NULL, &err)) {// 执行 SQL 语句
		printf("createTable succeed!\n");
		sprintf (szSQL,"----createTable_bak succeed=%s------",inTabName);
		AddOutputString_e(hListOutput,szSQL);
		//sqlite3_close(pDataBase);//关闭数据库
		return 1;
	}else{
		printf("err:%s",err);//如果失败打印错误码
		sqlite3_free(err);
		return 0;
	}
}

int sqlitel_db::creatTable( const char * inTabName )
{
	char szSQL[128] = "";
	sprintf(szSQL,"create table if not exists '%s'(ID integer primary key autoincrement,roleName varchar,StudentBlob  blob,ekey bigint)",inTabName);//列是从0 开始 
	//char* sql = "create table if not exists Student_Blob(ID integer primary key autoincrement,StudentBlob  blob)";//待执行的语句 varchar
	char *err = NULL;
	//CREATE TABLE IF NOT EXISTS t_person (id integer PRIMARY KEY AUTOINCREMENT, name text NOT NULL, age integer NOT NULL); 
	if (SQLITE_OK == sqlite3_exec(m_pDb, szSQL, NULL, NULL, &err)) {// 执行 SQL 语句
		printf("createTable succeed!\n");
		sprintf (szSQL,"----createTable succeed=%s------",inTabName);
		AddOutputString_e(hListOutput,szSQL);
		//sqlite3_close(pDataBase);//关闭数据库
		return 1;
	}else{
		printf("err:%s",err);//如果失败打印错误码
		sqlite3_free(err);
		return 0;
	}
}
/**********************************************************
 * Function: Table is or not exist
 * Input:  inTabName: Table's name
 * Output:
 * Return:  ErrorCode
**********************************************************/
int sqlitel_db::TableExists( const char * inTabName )
{
    char szSQL[128] = "";
    sprintf( szSQL,"select count(*) from sqlite_master where type='table' and name='%s'",inTabName );

    for( unsigned int m = 0; m < m_nLimitNum; m++ )
    {
        m_nState =  sqlite3_prepare( m_pDb, szSQL, -1, &m_pStmt, NULL );
        if ( SQLITE_OK == m_nState )
        {
            break;
        }
        else if( SQLITE_BUSY == m_nState )
        {
           if ( m_nLimitNum == m+1 )
           {
               m_strErrInfo = sqlite3_errmsg( m_pDb );
               goto __Exit;
           }
           //等待指定时间间隔继续执行
           sqlite3_busy_handler( m_pDb, NULL,NULL );
           Sleep( SLEEPTIME );
           continue;
        }
        else
        {
           m_strErrInfo = sqlite3_errmsg( m_pDb );
           goto __Exit;
        }
    }

    for( unsigned int m = 0; m < m_nLimitNum; m++ )
    {
        m_nState = sqlite3_step( m_pStmt );
        if ( SQLITE_ROW == m_nState )
        {
            if ( sqlite3_column_int( m_pStmt, 0 ) > 0 )
            {
                m_nState = DEF_TABLEEXIST;
            }
            else
            {
                m_nState = DEF_TABLENOTEXIST;
            }
            break;
        }
        else if ( SQLITE_BUSY == m_nState )
        {
            if ( m_nLimitNum == m+1 )
            {
                m_strErrInfo = sqlite3_errmsg( m_pDb );
                goto __Exit;
            }
            sqlite3_busy_handler( m_pDb, NULL,NULL );
            Sleep( SLEEPTIME );
        }
        else
        {
            m_strErrInfo = sqlite3_errmsg( m_pDb );
            goto __Exit;
        }
    }

__Exit:
    if ( NULL != m_pStmt )
    {
        sqlite3_finalize( m_pStmt );
        m_pStmt = NULL;
    }
    return m_nState;
}
/**********************************************************
 * Function: convert error code to strings
 * Input:
 * Output:
 * Return:  ErrorCode
**********************************************************/
const char* sqlitel_db::ErrCodeAsString(int inErrCode)
{
  switch (inErrCode)
  {
         case SQLITE_OK           : return "SQLITE_OK";
         case SQLITE_ERROR        : return "SQLITE_ERROR";
         case SQLITE_INTERNAL     : return "SQLITE_INTERNAL";
         case SQLITE_PERM         : return "SQLITE_PERM";
         case SQLITE_ABORT        : return "SQLITE_ABORT";
         case SQLITE_BUSY         : return "SQLITE_BUSY";
         case SQLITE_LOCKED       : return "SQLITE_LOCKED";
         case SQLITE_NOMEM        : return "SQLITE_NOMEM";
         case SQLITE_READONLY     : return "SQLITE_READONLY";
         case SQLITE_INTERRUPT    : return "SQLITE_INTERRUPT";
         case SQLITE_IOERR        : return "SQLITE_IOERR";
         case SQLITE_CORRUPT      : return "SQLITE_CORRUPT";
         case SQLITE_NOTFOUND     : return "SQLITE_NOTFOUND";
         case SQLITE_FULL         : return "SQLITE_FULL";
         case SQLITE_CANTOPEN     : return "SQLITE_CANTOPEN";
         case SQLITE_PROTOCOL     : return "SQLITE_PROTOCOL";
         case SQLITE_EMPTY        : return "SQLITE_EMPTY";
         case SQLITE_SCHEMA       : return "SQLITE_SCHEMA";
         case SQLITE_TOOBIG       : return "SQLITE_TOOBIG";
         case SQLITE_CONSTRAINT   : return "SQLITE_CONSTRAINT";
         case SQLITE_MISMATCH     : return "SQLITE_MISMATCH";
         case SQLITE_MISUSE       : return "SQLITE_MISUSE";
         case SQLITE_NOLFS        : return "SQLITE_NOLFS";
         case SQLITE_AUTH         : return "SQLITE_AUTH";
         case SQLITE_FORMAT       : return "SQLITE_FORMAT";
         case SQLITE_RANGE        : return "SQLITE_RANGE";
         case SQLITE_ROW          : return "SQLITE_ROW";
         case SQLITE_DONE         : return "SQLITE_DONE";

         case DEF_INPUTISNULL     : return "Input string is NULL";

         default                  : return "UNKNOWN_ERROR";
  }
}
/**********************************************************
 * Function: 默认释放类成员m_pStmt
 * Input:
 * Output:
 * Return:
**********************************************************/
void sqlitel_db::FreeStmt( void )
{
    if ( NULL != m_pStmt )
    {
        sqlite3_finalize( m_pStmt );
        m_pStmt = NULL;
    }
}


int sqlitel_db::gerAllRoleDataInaccName(char *accName)
{
	if (!accName /*|| !pRoleBuffer*/) return 0;
	int count=0;
	char *zErrMsg = 0;
	char sql[128];
	sprintf_s(sql,"select * from role_Blob"); //roleName='%s' and 
	//读取数据
	sqlite3_prepare(m_pDb,sql,strlen(sql),&m_pStmt, 0);
	int result = sqlite3_step(m_pStmt);
	int id = 0, len = 0;
	char nTempstr[128];
	while (result == SQLITE_ROW)                                 
	{
		id= sqlite3_column_int(m_pStmt, 0);
		const void * pReadBolbData = sqlite3_column_blob(m_pStmt, 2);
		if (pReadBolbData)
		{//查找到了就飞出去了
			TRoleData *pRoleData = (TRoleData *)pReadBolbData;   //账号的角色数据
			if (g_StrCmp(pRoleData->BaseInfo.caccname,accName))
			     count++;
		}
		result = sqlite3_step(m_pStmt);
	}
	sqlite3_finalize(m_pStmt);

	return count;
}


//获取某个角色得数据  根据key 或者 名字
int sqlitel_db::gerAllRoleDataInaccName(S3DBI_RoleBaseInfo * ptempBuffer,char *accName,int isThisSerVer,int roleMaxCount)
{
	if (!accName /*|| !pRoleBuffer*/) return 0;
	int count=0;
	char *zErrMsg = 0;
	char sql[128];
	sprintf_s(sql,"select * from role_Blob"); //roleName='%s' and 
	//读取数据
	sqlite3_prepare(m_pDb,sql,strlen(sql),&m_pStmt, 0);
	int result = sqlite3_step(m_pStmt);
	int id = 0, len = 0;
	char nTempstr[128];
	while (result == SQLITE_ROW)                                 
	{
		id= sqlite3_column_int(m_pStmt, 0);
		const void * pReadBolbData = sqlite3_column_blob(m_pStmt, 2);
		if (pReadBolbData)
		{//查找到了就飞出去了
	        if  (count >= roleMaxCount)
				break;

			TRoleData *pRoleData = (TRoleData *)pReadBolbData;   //账号的角色数据
			if (pRoleData->BaseInfo.isWaiGua==isThisSerVer&&g_StrCmp(pRoleData->BaseInfo.caccname,accName))
			{//如果找到了 是同区的角色 就返回 数量增加 
				strncpy(ptempBuffer->szName, pRoleData->BaseInfo.szName, NAME_LENGHT);   //角色名
				ptempBuffer->szName[NAME_LENGHT-1] = '\0';
				ptempBuffer->Sex    = pRoleData->BaseInfo.bSex;          //男女
				ptempBuffer->Series = pRoleData->BaseInfo.ifiveprop;     //五行
				ptempBuffer->Level  = pRoleData->BaseInfo.ifightlevel;   //战斗等级	
				ptempBuffer++;		//下个角色清单
				count++;       //角色数量加1
				///TRoleData *pRoleData = (TRoleData *)pReadBolbData;  //检测账号
				len = sqlite3_column_bytes(m_pStmt,2);   
				sprintf (nTempstr,"------查询到记录,记录编号:%d.账号=%s,角色=%s,总长度:%d------",count,accName,pRoleData->BaseInfo.szName,len);
				AddOutputString_e(hListOutput,nTempstr);
			}// end if
		}
		result = sqlite3_step(m_pStmt);
	}
	sqlite3_finalize(m_pStmt);

	return count;
}


//获取某个角色得数据  根据key 或者 名字
int sqlitel_db::SelectBlobData(void * pRoleBuffer,char *roleName,unsigned int nkey)
{
	if (!roleName /*|| !pRoleBuffer*/) return 0;

	char *zErrMsg = 0;
    char sql[1000];
	sprintf_s(sql,"select * from role_Blob where roleName='%s'",G2U(roleName)); //roleName='%s' and  ekey='%u'
	//读取数据
	sqlite3_prepare(m_pDb,sql,strlen(sql),&m_pStmt, 0);
	int result = sqlite3_step(m_pStmt);
	int id = 0, len = 0;
	char nTempstr[128];
	while (result == SQLITE_ROW)                                 
	{
		id= sqlite3_column_int(m_pStmt, 0);
		const void * pReadBolbData = sqlite3_column_blob(m_pStmt, 2);
		if (pReadBolbData)
		{//查找到了就飞出去了
			len = sqlite3_column_bytes(m_pStmt,2);   
			//	memcpy(pRoleBuffer,cursor->data,cursor->size);  //赋值角色数据包
			if (pRoleBuffer)
			{
				memcpy(pRoleBuffer,pReadBolbData,len);         // sizeof(tempStudent)    
				sprintf (nTempstr,"------查询到一条记录,记录编号:%d.角色=%s,key=%u,总长度:%d------",id,roleName,nkey,len);
				AddOutputString_e(hListOutput,nTempstr);
			}
			break;
		}

		result = sqlite3_step(m_pStmt);
	}
	sqlite3_finalize(m_pStmt);                                        
	return len;
}


//相当是查询了
bool sqlitel_db::deleteBlobData(char *roleName,unsigned int nkey,bool show)
{
	bool bRet = false;
	int rc;
	char sql[3000];
	char *pErrMsg = 0;
	sprintf_s(sql, "delete from role_Blob where roleName='%s'",G2U(roleName)); //roleName='%s' and  ekey=%u
	rc = sqlite3_exec(m_pDb, sql, NULL, 0, &pErrMsg);
	if (rc != SQLITE_OK)
	{//说明没有这个数据
		//cout << "操作发生失败，失败原因:" << pErrMsg << endl;;
		sqlite3_free(pErrMsg);
		bRet = false;
	}
	else
	{//如果执行成功 就说明删除了
		if (show)
		{
			char nTempstr[128];
			sprintf (nTempstr,"------删除所有记录:角色=%s,key=%u------",roleName,nkey);
			AddOutputString_e(hListOutput,nTempstr);
		}
		bRet = true;
	}
	return bRet;
}

void sqlitel_db::findRoleITemInSql(char *accName,char *roleName,int shen,int zimu,int chuwu,int kuoyi,int kuoer,int kuosan,int all)
{
	if (!accName || !roleName) true;
	char sql[256],nTempstr[128];
	char *zErrMsg=NULL;
	sprintf_s(sql,"select * from role_Blob where roleName='%s'",G2U(roleName)); //roleName='%s' and  ekey='%u'
	//读取数据
	sqlite3_prepare(m_pDb,sql,strlen(sql),&m_pStmt, 0);
	int id=0,len=0,itemCount=0,nSkillCount=0;
	while (sqlite3_step(m_pStmt) == SQLITE_ROW)                                 
	{
		id= sqlite3_column_int(m_pStmt, 0);
		const void * pReadBolbData = sqlite3_column_blob(m_pStmt, 2);
		if (pReadBolbData)
		{//查找到了就飞出去了
			len = sqlite3_column_bytes(m_pStmt,2);   
            TRoleData *nTempData=(TRoleData *)pReadBolbData;
			itemCount=nTempData->nItemCount;
			nSkillCount=nTempData->nFightSkillCount;
			//战斗技能
			//int  dwFSkillOffset = (BYTE * )nTempData->pBuffer -  (BYTE*)pReadBolbData;
			//int j=0;
			//TDBSkillData *pSkillData		= (TDBSkillData *)((BYTE*)pReadBolbData + dwFSkillOffset);
			//for (j = 0 ;j < nSkillCount; ++j, ++pSkillData)
			//{
			//}
			////生活技能
			//int dwLSkillOffset		= (BYTE *)pSkillData -  (BYTE*)pReadBolbData;
			//pSkillData				= (TDBSkillData *)((BYTE*)pReadBolbData + dwLSkillOffset);
			//nSkillCount = 0;
			//for j = 0 ;j < nSkillCount; ++j, ++pSkillData)
			//{

			//}
			////任务
			//int dwTaskOffset			= (BYTE*)pSkillData - (BYTE*)pReadBolbData;

			TDBItemData * pItemData = (TDBItemData *)((BYTE*)pReadBolbData+nTempData->dwItemOffset);
			if  (!pItemData)
			{
				len = 0;
				break;
			}
			sprintf (nTempstr,"-角色=%s,itemCount=%d,长度:%d",roleName,itemCount,len);
			AddOutputString_e(hBakRoleListOutput,nTempstr);
			for (int i=0;i<itemCount;++i,++pItemData)
			{
				//if (shen)
				switch(pItemData->ilocal) //目标容器
				{
				case pos_hand:	// 手上
					break;
				case pos_equip:			// 装备着的

					sprintf (nTempstr,"pos_equip-item[%d]=%d,X=%d,Y:%d",i,pItemData->ilocal,pItemData->ix,pItemData->iy);
					AddOutputString_e(hBakRoleListOutput,nTempstr);
					sprintf (nTempstr,"item[%d],class=%d,detail:%d",i,pItemData->iequipclasscode,pItemData->idetailtype);
					AddOutputString_e(hBakRoleListOutput,nTempstr);
					break;
				case pos_equiproom:		// 道具栏  包袱
					break;
				case pos_repositoryroom:	// 贮物箱
					break;
			   case pos_exbox1room:		// mo rong ruong 1  扩展箱
				   break;
				case pos_exbox2room:		// mo rong ruong 2
					break;
				case pos_exbox3room:		// mo rong ruong 3
					break;
				case pos_equiproomex:	// 同伴包袱
					break;
				case pos_traderoom:		// 交易栏
					break;
				case pos_trade1:		// 交易过程中对方的交易栏
					break;
				case pos_immediacy:		// 快捷栏物品
					break;
				case pos_give:           // 给予界面
					break;
			    case pos_dazao:         // 打造框界面
					break;
				case pos_cailiao:       // 材料栏
					break;
				case pos_xiangqi:       // 象棋栏
					break;
				default:
					sprintf (nTempstr,"All-item[%d]=%d,X=%d,Y:%d",i,pItemData->ilocal,pItemData->ix,pItemData->iy);
					AddOutputString_e(hBakRoleListOutput,nTempstr);
					break;
				}
				//if  (pItemData->ilocal>=pos_hand && pItemData->ilocal<=pos_xiangqi)
				//{
				//	if (pItemData->iequipclasscode==item_equip && pItemData->idetailtype==5)
				//	{
				//		sprintf (nTempstr,"共有-item[%d]=%d,X=%d,Y:%d",i,pItemData->ilocal,pItemData->ix,pItemData->iy);
				//		AddOutputString_e(hBakRoleListOutput,nTempstr);
				//		//sprintf (nTempstr,"[%d]账号-%s-角色:%s",i,nTempData->BaseInfo.caccname,nTempData->BaseInfo.szName);
				//		//AddOutputString_e(hBakRoleListOutput,nTempstr);		
				//	}		
				//}
				//sprintf (nTempstr,"-item[%d]=%d,X=%d,Y:%d",i,pItemData->ilocal,pItemData->ix,pItemData->iy);
			
			}
			sprintf (nTempstr,"--------------------------------------------");
			AddOutputString_e(hBakRoleListOutput,nTempstr);
			break;
		}
	}

	if  (!len)
	{
		sprintf (nTempstr,"-查询:角色=%s,没有该角色信息--",roleName);
		AddOutputString_e(hBakRoleListOutput,nTempstr);
	}

	sqlite3_finalize(m_pStmt);
}

void sqlitel_db::copyDataToDB(ZDBTable *db_table)
{
	char sql[256],nTempstr[128];
	char *zErrMsg=NULL;
	if (db_table &&  m_pDb)
	{
		//开始复制数据
		sprintf_s(sql, "select * from role_Blob");
		int idx=0,len=0,count=0;
		if  (SQLITE_OK== sqlite3_prepare(m_pDb, sql, strlen(sql), &m_pStmt, 0))                 
		{
			while (sqlite3_step(m_pStmt) == SQLITE_ROW)
			{
				idx = sqlite3_column_int(m_pStmt, 0); 
				const void * pReadBolbData = sqlite3_column_blob(m_pStmt, 2);
				if (pReadBolbData)
				{
					len = sqlite3_column_bytes(m_pStmt,2); 
					TRoleData *nTempData=(TRoleData *)pReadBolbData;
					if (db_table->add(nTempData->BaseInfo.szName,strlen(nTempData->BaseInfo.szName)+1,(char *)pReadBolbData,len)) 
					{				
						sprintf (nTempstr,"------[SQL]拷贝[%d]到DB数据库表成功:%s:%s------",count,nTempData->BaseInfo.caccname,nTempData->BaseInfo.szName);
						AddOutputString_e(hListOutput,nTempstr);
						count++;
					}	
				}
			}
		}
		sqlite3_finalize(m_pStmt);
	}
}

void sqlitel_db::copyDataToBaktable()
{
	//bak = sqlite3_backup_init(m_pDb_bak, "role_bak", m_pDb, "role"); //特别注意，源数据库名和目的数据库名都是固定的字符串“main”
 //   if (bak)
	//{
	//	sqlite3_backup_step(bak, -1);
	//    sqlite3_backup_finish(bak); 
 //    }  
	//delete from role_Blob
	 char sql[256],nTempstr[128];
	 char *zErrMsg=NULL;
	if (m_pDb_bak && m_pDb)
	{
		sprintf(sql,"%s","delete from role_Blob");
		if (SQLITE_OK ==  sqlite3_exec(m_pDb_bak, sql, NULL, 0, &zErrMsg))
		{
			//设置一些 id归零  DELETE FROM sqlite_sequence;	
			sprintf (nTempstr,"------[SQL]删除备份数据role_Blob库表所有记录 成功------");
			AddOutputString_e(hListOutput,nTempstr);
			sprintf(sql,"%s","delete from sqlite_sequence");
			if (SQLITE_OK ==  sqlite3_exec(m_pDb_bak, sql, NULL, 0, &zErrMsg))
			{
				sprintf (nTempstr,"------[SQL]清空备份数据库sqlite_sequence表所有记录 成功------");
				AddOutputString_e(hListOutput,nTempstr);
			}
		}
		//
		if (zErrMsg)
			sqlite3_free(zErrMsg);
		//开始复制数据
		sprintf_s(sql, "select * from role_Blob");
		int idx=0,len=0,count=0;
		if  (SQLITE_OK== sqlite3_prepare(m_pDb, sql, strlen(sql), &m_pStmt, 0))                 
		{
			while (sqlite3_step(m_pStmt) == SQLITE_ROW)
			{
				idx = sqlite3_column_int(m_pStmt, 0); 
				const void * pReadBolbData = sqlite3_column_blob(m_pStmt, 2);
				if (pReadBolbData)
				{//查找到了就飞出去了
					len = sqlite3_column_bytes(m_pStmt,2); 
					//------------------------------------------------------------------------//插入数据
					TRoleData *nTempData=(TRoleData *)pReadBolbData;
					sqlite3_stmt * m_Stmt=NULL;
					unsigned int nKey=FileNameDB(nTempData->BaseInfo.szName);
					sprintf_s(sql, "insert into role_Blob values(NULL,'%s',?,'%u');",G2U(nTempData->BaseInfo.szName),nKey);  //('%d','%s',?,0);
					sqlite3_prepare(m_pDb_bak, sql, strlen(sql), &m_Stmt, 0);                  
					{
						sqlite3_bind_blob(m_Stmt, 1,pReadBolbData, len, NULL);   
						sqlite3_step(m_Stmt);                                             
					}
					sqlite3_finalize(m_Stmt);
					count++;
					sprintf (nTempstr,"------[SQL]拷贝[%d]到备份数据库表成功:%s:%s------",count,nTempData->BaseInfo.caccname,nTempData->BaseInfo.szName);
					AddOutputString_e(hListOutput,nTempstr);
					//---------------------------------------------------------------------------
				}
			}
		}
		sqlite3_finalize(m_pStmt);
	}
}


//{
//	string str = "update database set x = :1 where a = A";
//	sqlite3_stmt* pVM;
//	int nRet = sqlite3_prepare(pDB, str.c_str(), -1, &pVM, &szTail);
//	nRet = sqlite3_bind_blob(pVM, 1,
//		(const void*)blobValue, nLen, SQLITE_TRANSIENT);
//	int nRet = sqlite3_step(mpVM);
//}

bool sqlitel_db::updateBlobData_n(void * roleData,int len,char *roleName,unsigned int nkey,bool show)
{
	if (!roleName || !roleData || len <= 0 || !m_pDb) return false;

	char nTempstr[256];
	// Stale save protection: only update if ekey is null or incoming nkey >= ekey.
	// If incoming nkey == 0, allow update for unversioned saves.
	const char* sql = "update role_Blob set StudentBlob = ?, ekey = ? where roleName = ? and (? = 0 or ekey is null or ekey <= ?);";
	sqlite3_stmt* pStmt = NULL;

	if (sqlite3_prepare_v2(m_pDb, sql, -1, &pStmt, NULL) != SQLITE_OK)
	{
		if (show)
		{
			sprintf_s(nTempstr, sizeof(nTempstr), "------[SQL] update prepare fail: role=%s, key=%u------", roleName, nkey);
			AddOutputString_e(hListOutput, nTempstr);
		}
		return false;
	}

	if (sqlite3_bind_blob(pStmt, 1, (const void*)roleData, len, SQLITE_TRANSIENT) != SQLITE_OK ||
	    sqlite3_bind_int64(pStmt, 2, (sqlite3_int64)nkey) != SQLITE_OK ||
	    sqlite3_bind_text(pStmt, 3, G2U(roleName), -1, SQLITE_TRANSIENT) != SQLITE_OK ||
	    sqlite3_bind_int64(pStmt, 4, (sqlite3_int64)nkey) != SQLITE_OK ||
	    sqlite3_bind_int64(pStmt, 5, (sqlite3_int64)nkey) != SQLITE_OK)
	{
		sqlite3_finalize(pStmt);
		if (show)
		{
			sprintf_s(nTempstr, sizeof(nTempstr), "------[SQL] update bind fail: role=%s, key=%u------", roleName, nkey);
			AddOutputString_e(hListOutput, nTempstr);
		}
		return false;
	}

	int nStep = sqlite3_step(pStmt);
	int nChanges = sqlite3_changes(m_pDb);
	sqlite3_finalize(pStmt);

	bool bSuccess = (nStep == SQLITE_DONE && nChanges > 0);

	if (show)
	{
		if (bSuccess)
			sprintf_s(nTempstr, sizeof(nTempstr), "------[SQL] update: role=%s, key=%u OK (changes=%d)------", roleName, nkey, nChanges);
		else
			sprintf_s(nTempstr, sizeof(nTempstr), "------[SQL] update: role=%s, key=%u FAIL (step=%d, changes=%d)------", roleName, nkey, nStep, nChanges);
		AddOutputString_e(hListOutput, nTempstr);
	}

	return bSuccess;
}


bool sqlitel_db::ensureTradeReceiptTable()
{
	if (!m_pDb) return false;
	const char* sql = "CREATE TABLE IF NOT EXISTS trade_receipts ("
	                  "tx_id BIGINT PRIMARY KEY, "
	                  "role_a VARCHAR(64), "
	                  "role_b VARCHAR(64), "
	                  "status INT, "
	                  "crc_a INT, "
	                  "crc_b INT, "
	                  "commit_time BIGINT);";
	char* zErrMsg = NULL;
	if (sqlite3_exec(m_pDb, sql, NULL, NULL, &zErrMsg) != SQLITE_OK)
	{
		if (zErrMsg) sqlite3_free(zErrMsg);
		return false;
	}
	// ekey stores the legacy FileNameDB(name) key, not a trade sequence.
	// This table is the durable per-role ordering fence across relog and restart.
	const char* fenceSql = "CREATE TABLE IF NOT EXISTS trade_role_fence ("
	    "role_name TEXT PRIMARY KEY, last_tx_id INTEGER NOT NULL);";
	zErrMsg = NULL;
	if (sqlite3_exec(m_pDb, fenceSql, NULL, NULL, &zErrMsg) != SQLITE_OK)
	{
		if (zErrMsg) sqlite3_free(zErrMsg);
		return false;
	}
	return JXRoleSaveRevision::Ensure(m_pDb);
}

bool sqlitel_db::isTradeTxCommitted(unsigned __int64 uTxId)
{
	if (!m_pDb) return false;
	ensureTradeReceiptTable();
	const char* sql = "SELECT status FROM trade_receipts WHERE tx_id = ?;";
	sqlite3_stmt* pStmt = NULL;
	if (sqlite3_prepare_v2(m_pDb, sql, -1, &pStmt, NULL) != SQLITE_OK)
		return false;

	sqlite3_bind_int64(pStmt, 1, (sqlite3_int64)uTxId);
	int nStep = sqlite3_step(pStmt);
	bool bCommitted = false;
	if (nStep == SQLITE_ROW)
	{
		int nStatus = sqlite3_column_int(pStmt, 0);
		if (nStatus == 1)
			bCommitted = true;
	}
	sqlite3_finalize(pStmt);
	return bCommitted;
}

int sqlitel_db::checkTradeReceiptMatch(unsigned __int64 uTxId, const char* szRoleA, unsigned int dwCrcA, const char* szRoleB, unsigned int dwCrcB)
{
	if (!m_pDb) return 0;
	ensureTradeReceiptTable();
	const char* sql = "SELECT status, crc_a, crc_b, role_a, role_b FROM trade_receipts WHERE tx_id = ?;";
	sqlite3_stmt* pStmt = NULL;
	if (sqlite3_prepare_v2(m_pDb, sql, -1, &pStmt, NULL) != SQLITE_OK)
		return 0;

	sqlite3_bind_int64(pStmt, 1, (sqlite3_int64)uTxId);
	int nStep = sqlite3_step(pStmt);
	int nMatchResult = 0;
	if (nStep == SQLITE_ROW)
	{
		int nStatus = sqlite3_column_int(pStmt, 0);
		unsigned int dbCrcA = (unsigned int)sqlite3_column_int64(pStmt, 1);
		unsigned int dbCrcB = (unsigned int)sqlite3_column_int64(pStmt, 2);
		const char* dbRoleA = (const char*)sqlite3_column_text(pStmt, 3);
		const char* dbRoleB = (const char*)sqlite3_column_text(pStmt, 4);

		if (nStatus == 1 &&
		    dbCrcA == dwCrcA && dbCrcB == dwCrcB &&
		    dbRoleA && szRoleA && _stricmp(dbRoleA, szRoleA) == 0 &&
		    dbRoleB && szRoleB && _stricmp(dbRoleB, szRoleB) == 0)
		{
			nMatchResult = 1; // Idempotent match
		}
		else
		{
			nMatchResult = -13; // Payload mismatch / duplicate TxID collision
		}
	}
	sqlite3_finalize(pStmt);
	return nMatchResult;
}

bool sqlitel_db::recordTradeReceipt_internal(unsigned __int64 uTxId, const char* szRoleA, unsigned int dwCrcA, const char* szRoleB, unsigned int dwCrcB, int status)
{
	if (!m_pDb) return false;
	const char* sql = "INSERT INTO trade_receipts (tx_id, role_a, role_b, status, crc_a, crc_b, commit_time) VALUES (?, ?, ?, ?, ?, ?, ?);";
	sqlite3_stmt* pStmt = NULL;
	if (sqlite3_prepare_v2(m_pDb, sql, -1, &pStmt, NULL) != SQLITE_OK)
		return false;

	FILETIME ft;
	GetSystemTimeAsFileTime(&ft);
	unsigned __int64 uNow = ((unsigned __int64)ft.dwHighDateTime << 32) | (unsigned __int64)ft.dwLowDateTime;

	sqlite3_bind_int64(pStmt, 1, (sqlite3_int64)uTxId);
	sqlite3_bind_text(pStmt, 2, szRoleA ? szRoleA : "", -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(pStmt, 3, szRoleB ? szRoleB : "", -1, SQLITE_TRANSIENT);
	sqlite3_bind_int(pStmt, 4, status);
	sqlite3_bind_int64(pStmt, 5, (sqlite3_int64)dwCrcA);
	sqlite3_bind_int64(pStmt, 6, (sqlite3_int64)dwCrcB);
	sqlite3_bind_int64(pStmt, 7, (sqlite3_int64)uNow);

	int nStep = sqlite3_step(pStmt);
	int nChanges = sqlite3_changes(m_pDb);
	sqlite3_finalize(pStmt);

	return (nStep == SQLITE_DONE && nChanges > 0);
}

// G2U returns a newly allocated buffer. Keep the database key UTF-8 without leaking it.
static std::string EncodeTradeRoleName(const char* role)
{
	char* converted = G2U(role);
	if (!converted) return std::string();
	std::string result(converted);
	delete[] converted;
	return result;
}

// SQLite login path: read role bytes and revision from the SAME transaction.
// The input name is in the same legacy encoding used by SaveRoleInfo.
// No partial role bytes/revision are returned on failure.
int sqlitel_db::SelectBlobDataWithRevision(void* output, int capacity, char* roleName,
    sqlite3_int64* revision)
{
    if (!output || capacity <= 0 || !roleName || !roleName[0] || !revision ||
        !ensureTradeReceiptTable()) return 0;
    const std::string name = EncodeTradeRoleName(roleName);
    if (name.empty()) return 0;
    std::vector<unsigned char> blob;
    sqlite3_int64 snap = -1;
    if (JXRoleSaveRevision::LoadSnapshot(m_pDb, name, &blob, &snap) != 1 ||
        blob.empty() || blob.size() > (size_t)capacity ||
        snap < 0 || blob.size() > 128 * 1024) return 0;
    memcpy(output, blob.data(), blob.size());
    *revision = snap;
    return (int)blob.size();
}

// This opt-in entrypoint is used only by a negotiated revisioned
// save packet; ordinary legacy saves still use updateBlobData_n.
int sqlitel_db::updateBlobData_versioned(void* data, int len, char* roleName,
    sqlite3_int64 expected, sqlite3_int64* next)
{
    if (!roleName || !roleName[0] || !ensureTradeReceiptTable()) return -14;
    const std::string utf8Role = EncodeTradeRoleName(roleName);
    if (utf8Role.empty()) return -14;
    return JXRoleSaveRevision::Save(m_pDb, utf8Role, data, len, expected, next);
}

// Returns 1 for a new TxID, -16 for a stale TxID, -14 for SQLite errors.
static int CheckTradeRoleOrder(sqlite3* db, const char* role, unsigned __int64 txId)
{
	sqlite3_stmt* stmt = NULL;
	const char* sql = "SELECT last_tx_id FROM trade_role_fence WHERE role_name = ?;";
	if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) return -14;
	const std::string utf8Role = EncodeTradeRoleName(role);
	if (sqlite3_bind_text(stmt, 1, utf8Role.c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK)
	{
		sqlite3_finalize(stmt);
		return -14;
	}
	const int result = sqlite3_step(stmt);
	int answer = -14;
	if (result == SQLITE_DONE)
	{
		// Upgrade path: an existing committed receipt may predate trade_role_fence.
		// Consult historical receipts only once per role (while the fence row is absent).
		sqlite3_stmt* previous = NULL;
		const char* historySql = "SELECT MAX(tx_id) FROM trade_receipts "
		    "WHERE status = 1 AND (role_a = ? COLLATE NOCASE OR role_b = ? COLLATE NOCASE);";
		if (sqlite3_prepare_v2(db, historySql, -1, &previous, NULL) == SQLITE_OK)
		{
			// Old trade_receipts stores the raw protocol/GBK role names.
			// Do not compare a UTF-8 database key against these legacy receipt bytes.
			if (sqlite3_bind_text(previous, 1, role, -1, SQLITE_TRANSIENT) == SQLITE_OK &&
			    sqlite3_bind_text(previous, 2, role, -1, SQLITE_TRANSIENT) == SQLITE_OK)
			{
				const int historyStep = sqlite3_step(previous);
				if (historyStep == SQLITE_ROW)
				{
					const sqlite3_int64 lastId =
					    (sqlite3_column_type(previous, 0) == SQLITE_NULL) ? 0 : sqlite3_column_int64(previous, 0);
					answer = (lastId >= 0 && (sqlite3_int64)txId > lastId) ? 1 : -16;
				}
			}
			sqlite3_finalize(previous);
		}
	}
	else if (result == SQLITE_ROW)
	{
		const sqlite3_int64 lastId = sqlite3_column_int64(stmt, 0);
		answer = (lastId >= 0 && (sqlite3_int64)txId > lastId) ? 1 : -16;
	}
	sqlite3_finalize(stmt);
	return answer;
}

// Atomic trade writes must NOT change the legacy ekey hash.
static bool WriteTradeRoleBlob(sqlite3* db, const char* role, const void* blob, int len)
{
	sqlite3_stmt* stmt = NULL;
	const char* sql = "UPDATE role_Blob SET StudentBlob = ? WHERE roleName = ?;";
	if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) return false;
	const std::string utf8Role = EncodeTradeRoleName(role);
	const bool bound = sqlite3_bind_blob(stmt, 1, blob, len, SQLITE_TRANSIENT) == SQLITE_OK &&
	    sqlite3_bind_text(stmt, 2, utf8Role.c_str(), -1, SQLITE_TRANSIENT) == SQLITE_OK;
	const int result = bound ? sqlite3_step(stmt) : SQLITE_ERROR;
	const int changed = sqlite3_changes(db);
	sqlite3_finalize(stmt);
	return result == SQLITE_DONE && changed == 1;
}

static bool SetTradeRoleOrder(sqlite3* db, const char* role, unsigned __int64 txId)
{
	sqlite3_stmt* stmt = NULL;
	const char* sql = "INSERT OR REPLACE INTO trade_role_fence(role_name,last_tx_id) VALUES(?,?);";
	if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) return false;
	const std::string utf8Role = EncodeTradeRoleName(role);
	const bool bound = sqlite3_bind_text(stmt, 1, utf8Role.c_str(), -1, SQLITE_TRANSIENT) == SQLITE_OK &&
	    sqlite3_bind_int64(stmt, 2, (sqlite3_int64)txId) == SQLITE_OK;
	const int result = bound ? sqlite3_step(stmt) : SQLITE_ERROR;
	sqlite3_finalize(stmt);
	return result == SQLITE_DONE;
}

int sqlitel_db::updateBlobData_atomic_dual_tx(
    unsigned __int64 uTxId,
    void * roleDataA, int lenA, char *roleNameA, unsigned int dwCrcA,
    void * roleDataB, int lenB, char *roleNameB, unsigned int dwCrcB,
    unsigned int nkeyA, unsigned int nkeyB, bool show)
{
	if (!m_pDb || !roleNameA || !roleNameB || !roleDataA || !roleDataB) return -10;
	// P0: never accept zero-generation trade packets or non-positive SQLite TxIDs.
	// TxID ordering is durable; in-session generations are only sanity markers.
	if (!uTxId || uTxId > 0x7FFFFFFFFFFFFFFFULL || !nkeyA || !nkeyB ||
	    !roleNameA[0] || !roleNameB[0] || _stricmp(roleNameA, roleNameB) == 0) return -15;

	if (!ensureTradeReceiptTable()) return -14;

	char* zErrMsg = NULL;
	if (sqlite3_exec(m_pDb, "BEGIN IMMEDIATE TRANSACTION;", NULL, NULL, &zErrMsg) != SQLITE_OK)
	{
		if (zErrMsg) sqlite3_free(zErrMsg);
		return -14;
	}

	// 1. Kiểm tra biên lai giao dịch BÊN TRONG IMMEDIATE TRANSACTION
	int nCheck = checkTradeReceiptMatch(uTxId, roleNameA, dwCrcA, roleNameB, dwCrcB);
	if (nCheck == 1)
	{
		// Never acknowledge a replay as durable success if SQLite could not commit.
		if (sqlite3_exec(m_pDb, "COMMIT TRANSACTION;", NULL, NULL, NULL) != SQLITE_OK)
		{
			sqlite3_exec(m_pDb, "ROLLBACK TRANSACTION;", NULL, NULL, NULL);
			return -14;
		}
		if (show)
		{
			char nTempstr[256];
			sprintf_s(nTempstr, sizeof(nTempstr), "------[SQL Dual Atomic]: Tx 0x%llX ALREADY COMMITTED (Idempotent success)------", uTxId);
			AddOutputString_e(hListOutput, nTempstr);
		}
		return 1;
	}
	else if (nCheck == -13)
	{
		sqlite3_exec(m_pDb, "ROLLBACK TRANSACTION;", NULL, NULL, NULL);
		if (show)
		{
			char nTempstr[256];
			sprintf_s(nTempstr, sizeof(nTempstr), "------[SQL Dual Atomic ERROR]: Tx 0x%llX COLLISION (Duplicate TxID with mismatched payload)------", uTxId);
			AddOutputString_e(hListOutput, nTempstr);
		}
		return -13;
	}

	// Reject a different older TxID before either player blob changes.
	// Same-TxID replay was already handled by the committed receipt above.
	const int orderA = CheckTradeRoleOrder(m_pDb, roleNameA, uTxId);
	const int orderB = (orderA == 1) ? CheckTradeRoleOrder(m_pDb, roleNameB, uTxId) : orderA;
	if (orderA != 1 || orderB != 1)
	{
		sqlite3_exec(m_pDb, "ROLLBACK TRANSACTION;", NULL, NULL, NULL);
		return (orderA == -16 || orderB == -16) ? -16 : -14;
	}

	bool bOkA = WriteTradeRoleBlob(m_pDb, roleNameA, roleDataA, lenA);
	bool bOkB = false;
	if (bOkA)
		bOkB = WriteTradeRoleBlob(m_pDb, roleNameB, roleDataB, lenB);

	bool bOkReceipt = false;
	bool bOkFenceA = false;
	bool bOkFenceB = false;
	if (bOkA && bOkB)
	{
		bOkReceipt = recordTradeReceipt_internal(uTxId, roleNameA, dwCrcA, roleNameB, dwCrcB, 1);
		if (bOkReceipt)
		{
			bOkFenceA = SetTradeRoleOrder(m_pDb, roleNameA, uTxId);
			if (bOkFenceA)
				bOkFenceB = SetTradeRoleOrder(m_pDb, roleNameB, uTxId);
		}
	}

	if (bOkA && bOkB && bOkReceipt && bOkFenceA && bOkFenceB &&
	    JXRoleSaveRevision::Advance(m_pDb, EncodeTradeRoleName(roleNameA)) &&
	    JXRoleSaveRevision::Advance(m_pDb, EncodeTradeRoleName(roleNameB)))
	{
		if (sqlite3_exec(m_pDb, "COMMIT TRANSACTION;", NULL, NULL, &zErrMsg) == SQLITE_OK)
		{
			if (show)
			{
				char nTempstr[256];
				sprintf_s(nTempstr, sizeof(nTempstr), "------[SQL Dual Atomic Commit with TxID 0x%llX]: roleA=%s, roleB=%s OK------", uTxId, roleNameA, roleNameB);
				AddOutputString_e(hListOutput, nTempstr);
			}
			return 1;
		}
		if (zErrMsg) sqlite3_free(zErrMsg);
	}

	sqlite3_exec(m_pDb, "ROLLBACK TRANSACTION;", NULL, NULL, NULL);
	if (show)
	{
		char nTempstr[256];
		sprintf_s(nTempstr, sizeof(nTempstr), "------[SQL Dual Atomic ROLLBACK with TxID 0x%llX]: roleA=%s (%s), roleB=%s (%s), receipt=(%s)------",
			uTxId, roleNameA, bOkA ? "OK" : "FAIL", roleNameB, bOkB ? "OK" : "FAIL", bOkReceipt ? "OK" : "FAIL");
		AddOutputString_e(hListOutput, nTempstr);
	}
	return -14;
}

bool sqlitel_db::updateBlobData_atomic_dual(
    void * roleDataA, int lenA, char *roleNameA,
    void * roleDataB, int lenB, char *roleNameB,
    unsigned int nkeyA, unsigned int nkeyB, bool show)
{
	if (!m_pDb || !roleNameA || !roleNameB || !roleDataA || !roleDataB) return false;

	char* zErrMsg = NULL;
	if (sqlite3_exec(m_pDb, "BEGIN IMMEDIATE TRANSACTION;", NULL, NULL, &zErrMsg) != SQLITE_OK)
	{
		if (zErrMsg) sqlite3_free(zErrMsg);
		return false;
	}

	bool bOkA = updateBlobData_n(roleDataA, lenA, roleNameA, nkeyA, show);
	bool bOkB = false;
	if (bOkA)
	{
		bOkB = updateBlobData_n(roleDataB, lenB, roleNameB, nkeyB, show);
	}

	if (bOkA && bOkB)
	{
		if (sqlite3_exec(m_pDb, "COMMIT TRANSACTION;", NULL, NULL, &zErrMsg) == SQLITE_OK)
		{
			if (show)
			{
				char nTempstr[256];
				sprintf_s(nTempstr, sizeof(nTempstr), "------[SQL Dual Atomic Commit]: roleA=%s, roleB=%s OK------", roleNameA, roleNameB);
				AddOutputString_e(hListOutput, nTempstr);
			}
			return true;
		}
		if (zErrMsg) sqlite3_free(zErrMsg);
	}

	// Rollback entire transaction if any update failed
	sqlite3_exec(m_pDb, "ROLLBACK TRANSACTION;", NULL, NULL, NULL);
	if (show)
	{
		char nTempstr[256];
		sprintf_s(nTempstr, sizeof(nTempstr), "------[SQL Dual Atomic ROLLBACK]: roleA=%s (%s), roleB=%s (%s)------",
			roleNameA, bOkA ? "OK" : "FAIL", roleNameB, bOkB ? "OK" : "FAIL");
		AddOutputString_e(hListOutput, nTempstr);
	}
	return false;
}


//更新数据  ('update byte set b = {}'.format(img))
bool sqlitel_db::updateBlobData(void * roleData,int len,char *roleName,unsigned int nkey,bool show)
 {//查询 获得id 删除 在原始id上插入数据 否则就新增加
	 if  (!roleName) return false;

	 char *zErrMsg = 0;
	 char sql[256];   
	 char nTempstr[128];
	sprintf_s(sql, "select ID from role_Blob where roleName='%s'",G2U(roleName));
	 int idx=0;
	if  (SQLITE_OK== sqlite3_prepare(m_pDb, sql, strlen(sql), &m_pStmt, 0))                 
	 {
		 while (sqlite3_step(m_pStmt) == SQLITE_ROW)
			 idx = sqlite3_column_int(m_pStmt, 0);                       
	 }
	 sqlite3_finalize(m_pStmt);

	 if (idx)
	 {//如果存在 就开始执行删除
		 sprintf_s(sql, "delete from role_Blob where roleName='%s'",G2U(roleName)); //roleName='%s' and  ekey=%u
		if (SQLITE_OK ==  sqlite3_exec(m_pDb, sql, NULL, 0, &zErrMsg))
		{
			sprintf (nTempstr,"------[SQL]删除记录:角色=%s,key=%u 成功------",roleName,nkey);
			AddOutputString_e(hListOutput,nTempstr);
		}

		if (zErrMsg)
		sqlite3_free(zErrMsg);

		//删除完毕 在源id位置上插入新数据
		 sprintf_s(sql, "insert into role_Blob values('%d','%s',?,'%u');",idx,G2U(roleName),nkey); 
	 }
	 else//否则就新增加id数据
		 sprintf_s(sql, "insert into role_Blob values(NULL,'%s',?,'%u');",G2U(roleName),nkey); 

	 sqlite3_prepare(m_pDb, sql, strlen(sql), &m_pStmt, 0);                  
	 {
		 sqlite3_bind_blob(m_pStmt, 1,roleData, len, NULL);   
		 sqlite3_step(m_pStmt);                                             
	 }
	 sqlite3_finalize(m_pStmt);
	 ///
	 if  (show)
	 {	 
		 sprintf (nTempstr,"------[SQL]更新记录:角色=%s,key=%u 成功------",roleName,nkey);
		 AddOutputString_e(hListOutput,nTempstr);
	 }

	 return true;
 }
//----------------------------------
//函数功能：向表中插入BLOB数据
//参数：    sqlite3 *db[IN]                               -- 数据库操作指针
//          const TestInfoStruct* info[IN]                -- 要插入的信息
//          const int recordId[IN]                        -- 记录ID
//返回值：  bool                                          -- 函数执行成功，则返回true；否则返回false
bool sqlitel_db::InsertBlobData(void * roleData,int len,char *roleName,unsigned int nkey,bool show)
{
	//"insert into blob_table values(rawtohex('里面放需要增加的BLOB'))";
	if  (!roleName) return false;
	char *zErrMsg = 0;
	char sql[1000];                                       
	sprintf_s(sql, "insert into role_Blob values(NULL,'%s',?,'%u');",G2U(roleName),nkey);  //('%d','%s',?,0);
	sqlite3_prepare(m_pDb, sql, strlen(sql), &m_pStmt, 0);                  
	{
		sqlite3_bind_blob(m_pStmt, 1,roleData, len, NULL);   
		sqlite3_step(m_pStmt);                                             
	}
	sqlite3_finalize(m_pStmt);

	if  (show)
	{
		char nTempstr[128];
		sprintf (nTempstr,"------[SQL]增加记录:角色=%s,key=%u 成功------",roleName,nkey);
		AddOutputString_e(hListOutput,nTempstr);
	}
	
	return true;
}
/**********************************************************/

void AddOutputString_e(HWND hListCtrl, char* aStr)
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

bool g_StrCmp(LPCSTR lpDest, LPCSTR lpSrc)
{
	register int nLen1, nLen2;
	nLen1 = strlen(lpDest);
	nLen2 = strlen(lpSrc);
	if (nLen1 != nLen2)
		return false;
	return (0 == memcmp(lpDest, lpSrc, nLen1));
}

//函数功能：将utf8字符转gb2312字符
//参数：    const char* utf8[IN]                   -- UTF8字符
//返回值：  char*                                  -- gb2312字符
char* U2G(const char* utf8)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len + 1];
	memset(wstr, 0, (len + 1) * sizeof(wchar_t));
	MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wstr, len);
	len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len + 1];
	memset(str, 0, len + 1);
	WideCharToMultiByte(CP_ACP, 0, wstr, -1, str, len, NULL, NULL);
	if (wstr) delete[] wstr;
	return str;
}

//unicode字符转utf8
//函数功能：将gb2312字符转换为utf8字符
//参数：    const char* gb2312[IN]                   -- gb2312字符
//返回值：  char*                                    -- UTF8字符
char* G2U(const char* gb2312)
{
	int len = MultiByteToWideChar(CP_ACP, 0, gb2312, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len + 1];
	memset(wstr, 0, (len + 1) * sizeof(wchar_t));
	MultiByteToWideChar(CP_ACP, 0, gb2312, -1, wstr, len);
	len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len + 1];
	memset(str, 0, len + 1);
	WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
	if (wstr) delete[] wstr;
	return str;
}



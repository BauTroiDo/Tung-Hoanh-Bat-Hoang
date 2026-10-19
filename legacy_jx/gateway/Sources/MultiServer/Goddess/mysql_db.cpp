#include "stdafx.h"
#include "mysql_db.h"
#ifndef WIN32
#include <sys/stat.h>
#else
#include <direct.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern HWND hListOutput;
extern HWND mainHwnd;

//数据库路径  和 名称

mysql_db::mysql_db()
{
	mysql_init(sqlCon);// mysql 初始化
	//::MessageBox(NULL, "mysql mysql_init成功", "suss", MB_OK|MB_ICONERROR);
	if  (&sqlCon)
	  stmt =   mysql_stmt_init(sqlCon); 
	else
	  stmt =NULL;
}

mysql_db::~mysql_db()
{
	mysql_close(sqlCon);// 关闭连接
}


void mysql_db::init()
{
	mysql_init(sqlCon);// mysql 初始化
}

void mysql_db::close()
{
		mysql_close(sqlCon);// 关闭连接
}


bool mysql_db::mysql_open(const char * host, const char * user, const char * password, const char * database, unsigned int port)
{
	char nvalue = 1;
	mysql_options(sqlCon, MYSQL_OPT_RECONNECT, (char *)&nvalue);// 断线自动重连
	mysql_options(sqlCon, MYSQL_SET_CHARSET_NAME, "gbk");// set name gbk 中文乱码问题
	//mysql_query(&sqlCon,"START TRANSACTION");                          // 开启事务， 如果没有开启事务，那么效率会变得非常低下！
	//mysql_query(&sqlCon,"COMMIT");   // 提交事务
	if (!mysql_real_connect(sqlCon, host, user, password, database, port, NULL, 0)){
		mysql_close(sqlCon);// 关闭连接
		return false;
	}
	return true;
}

//增加数据 
bool mysql_db::addBlobData(const char *roleName,const char *szSql,char *pData,int datalen)
{

	if  (/*roleName || */!szSql ||!pData || datalen<=0 ) return false;

	MYSQL_BIND bind[1]; 
	unsigned long length=datalen; 

	if (mysql_stmt_prepare(stmt, szSql, strlen(szSql))) 
	{ 
		/*fprintf(stderr, "\n mysql_stmt_prepare(), INSERT failed"); 
		fprintf(stderr, "\n %s", mysql_stmt_error(stmt)); */
		return false;
	} 
	memset(bind, 0, sizeof(bind)); 
	//bind[0].buffer_type= MYSQL_TYPE_STRING; 
	//bind[0].buffer_type = MYSQL_TYPE_LONG; 
	bind[0].buffer = pData; 
	//bind[0].buffer_type = MYSQL_TYPE_TINY; 
	bind[0].buffer_type = MYSQL_TYPE_MEDIUM_BLOB; 
	bind[0].length = &length; 
	bind[0].is_null= 0; 
	/* Bind the buffers */ 
	if (mysql_stmt_bind_param(stmt, bind)) 
	{ 
		/*fprintf(stderr, "\n param bind failed"); 
		fprintf(stderr, "\n %s", mysql_stmt_error(stmt));*/ 
		return false;
	} 

	///* Supply data in chunks to server */ 
	//if (mysql_stmt_send_long_data(stmt,0, pData, datalen)) 
	//{ 
	///*	fprintf(stderr, "\n send_long_data failed"); 
	//	fprintf(stderr, "\n %s", mysql_stmt_error(stmt)); */
	//	return false;
	//} 
	// pos += size; 
	///* Supply the next piece of data */ 
	//if (mysql_stmt_send_long_data(stmt,0, pData, datalen)) 
	//{ 
	//	/*fprintf(stderr, "\n send_long_data failed"); 
	//	fprintf(stderr, "\n %s", mysql_stmt_error(stmt)); */
	//	return false;
	//} 

	/* Now, execute the query */ 
	if (mysql_stmt_execute(stmt)) 
	{ 
		/*fprintf(stderr, "\n mysql_stmt_execute failed"); 
		fprintf(stderr, "\n %s", mysql_stmt_error(stmt)); */
		return false;
	} 


	return true;
}
//将一块二进制数据转换为mysql可识别的string数据。
//这样就直接可以通过mysql的sql语句insert，
//update来对blob数据进行更新和插入了，sql语句用法不变。

unsigned long mysql_db::EncodeString(char * pszDst, const char * pszSrc, int nLen)
{
	if (!pszDst) return 0;
	pszDst[0] = '\0';
	if  (!pszSrc || nLen <=0)  return 0;
	return mysql_real_escape_string(sqlCon, pszDst, pszSrc, nLen);
}

//bool mysql_db::SetBinaryField(std::string& strCondition,void* pDataIn,int nDataLen)
//{
//	if( ! mysql_stmt_prepare( m_pMySqlStmt, strCondition.c_str(), strCondition.length() ) ) 
//	{
//		memset(&m_MySqlBind,0,sizeof(MYSQL_BIND));
//		m_MySqlBind.buffer_type = MYSQL_TYPE_BLOB;
//		(*m_MySqlBind.length) = nDataLen;
//		memcpy(m_MySqlBind.buffer,pDataIn,nDataLen);
//		if(!mysql_stmt_bind_param(m_pMySqlStmt, (MYSQL_BIND *)&m_MySqlBind))
//		{
//			if(!mysql_stmt_execute(m_pMySqlStmt)) 
//			{
//				return true;
//			}
//		}
//	}
//	int nRes=GetDBErrorCode();
//	CLogOutStream errLog(crazy::ERROR_LEVEL,THIS_CLASS_NAME);
//	errLog<<"MySql Query Failed:\""<<strCondition<<"\" ,ErrorCode:"<<nRes<<crazy::logEnd;
//	return false;
//}



int mysql_db::mysql_noResult_query(const char * sql)
{
	if (mysql_query(sqlCon, sql) != 0){
		return -1;
	}
	//mysql_stmt_send_long_data
	return (int)mysql_affected_rows(sqlCon);
}

/*int mysql_db::mysql_select_query(const char * sql, map<int, vector<string>> & map_results)
{
	if (mysql_query(&sqlCon, sql) != 0){
		return -1;
	}

	if(!(m_pResult = mysql_use_result(&sqlCon))){
		return -1;
	}
	int i = 0;

	int count = mysql_num_fields(m_pResult);
	while(m_Row = mysql_fetch_row(m_pResult)){
		vector<string> vVal;
		for (int j = 0; j < count; j++){
			vVal.push_back(m_Row[j]);
		}
		map_results[i++] = vVal;
	}
	mysql_free_result(m_pResult);
	return i;
}*/
int mysql_db::mysql_select_query(const char * sql, map<int, vector<string>> & map_results)
{
    if (mysql_query(sqlCon, sql) != 0){
        return -1;
    }

    if(!(m_pResult = mysql_use_result(sqlCon))){
        return -1;
    }
    
    int i = 0;
    int count = mysql_num_fields(m_pResult);

    MYSQL_ROW m_Row;  // Khai báo đúng kiểu MYSQL_ROW

    // Sửa lại vòng lặp
    while ((m_Row = mysql_fetch_row(m_pResult)) != NULL){
        vector<string> vVal;
        for (int j = 0; j < count; j++){
            vVal.push_back(m_Row[j] ? m_Row[j] : "NULL");  // Kiểm tra NULL để tránh lỗi
        }
        map_results[i++] = vVal;
    }

    mysql_free_result(m_pResult);
    return i;
}

/*int mysql_db::mysql_select_SingleLine_query(const char * sql, vector<string> & v_results)
{
	if (mysql_query(&sqlCon, sql) != 0){
		return -1;
	}
	if(!(m_pResult = mysql_use_result(&sqlCon))){
		return -1;
	}
	int i = 0;

	int count = mysql_num_fields(m_pResult);
	while(m_Row = mysql_fetch_row(m_pResult)){
		for (int j = 0; j < count; j++){
			v_results.push_back(m_Row[j]);
		}
	}
	mysql_free_result(m_pResult);
	return i;
}*/

int mysql_db::mysql_select_SingleLine_query(const char * sql, vector<string> & v_results)
{
    if (mysql_query(sqlCon, sql) != 0){
        return -1;
    }

    if(!(m_pResult = mysql_use_result(sqlCon))){
        return -1;
    }

    int count = mysql_num_fields(m_pResult);

    MYSQL_ROW m_Row;  // Khai báo đúng kiểu MYSQL_ROW

    // Sửa lại vòng lặp
    if ((m_Row = mysql_fetch_row(m_pResult)) != NULL) {
        for (int j = 0; j < count; j++) {
            v_results.push_back(m_Row[j] ? m_Row[j] : "NULL");  // Kiểm tra NULL
        }
    }

    mysql_free_result(m_pResult);
    return v_results.size();  // Trả về số lượng phần tử trong v_results
}

string mysql_db::mysql_lasterror()
{
	return mysql_error(sqlCon);
}


 int mysql_db::createDB(const char *szSql)
 {
	 //	//创建一个数据库
	 //string sql = "create database if not exists database_test";

	 if ( -1 == mysql_noResult_query(szSql))
              return false;

	return true;
 }



MYSQL_RES* mysql_db::SelectRecord(const char *szSql)
{
	MYSQL *pMySql = sqlCon;
	if (pMySql == NULL)
		return NULL;
	if(mysql_query(pMySql,szSql))
		return NULL;
	MYSQL_RES *myquery = NULL;
	myquery = mysql_store_result(pMySql);
	return myquery;
}


bool mysql_db::InsertRecord(const char *szSql)
{
	bool bRet = false;
	MYSQL *pMySql = sqlCon;
	if (pMySql == NULL)
		return false;
	if(mysql_query(pMySql,szSql))
		bRet = true;

	return bRet;
}

bool  mysql_db::getfieldvalue(char * outValue,int field_num,MYSQL_FIELD * fields,MYSQL_ROW &row,char *fieldname)
{
	int i;
	for (i = 0;i < field_num;i++)
	{
		if (strcmp(fields[i].name,fieldname) == 0)
		{//如果列名相同

			if (row[i] != NULL)
			{
				strcpy(outValue,row[i]);
			}
			else
			{
				strcpy(outValue,"");
			}
			return true;
		}
	}
	return false;
}

#include "IDBRoleServer.h"

bool mysql_db::getRecord(const char *szSql)
{
	//if (mysql_query(&sqlCon, szSql) != 0){
	//	return false;
	//}

	//m_pResult = mysql_store_result(&sqlCon);
	//m_fields = mysql_fetch_fields(m_pResult);
 //   int  field_num = mysql_num_fields(m_pResult);
	//char* aBuffer = new char[128*1024]; //64
	//while (m_Row = mysql_fetch_row(m_pResult))
	//{
	//   getfieldvalue(aBuffer,field_num,m_fields,m_Row,"edata");
	//   AddOutputString_s(hListOutput,aBuffer);
	//}
	
	//char ntEMP[128];
	//for(int i = 0; i < field_num; i++)
	//{
 //      
	//	printf("Field %u is %s\n", i, m_fields[i].name);
	//	sprintf(ntEMP,"Field %u is %s", i, m_fields[i].name);
	//	AddOutputString_s(hListOutput,ntEMP);
	//}


	if(mysql_stmt_prepare(stmt, szSql, strlen(szSql)))
	{
		//fprintf(stderr, "mysql_stmt_prepare: %s\n", mysql_error(&sqlCon));
		return -1;
	}


	char name[20];
	//printf("id name: ");
	//scanf("%d %s", &id, name);
	const size_t nBufferLength = 128 * 1024;
	char* aBuffer = new char[nBufferLength];

	MYSQL_BIND params[1];
	memset(params, 0, sizeof(params));
	params[0].buffer_type = MYSQL_TYPE_MEDIUM_BLOB;
	params[0].buffer = aBuffer;
	params[0].buffer_length = nBufferLength;


	//mysql_stmt_bind_param(stmt, params);
	mysql_stmt_bind_result(stmt, params); //用于将结果集中的列与数据缓冲和长度缓冲关联（绑定）起来
	mysql_stmt_execute(stmt);                   //执行与语句句柄相关的预处理
	mysql_stmt_store_result(stmt);            //以便后续的mysql_stmt_fetch()调用能返回缓冲数据


	while(mysql_stmt_fetch(stmt) == 0) //返回结果集中的下一行
	{    
		//printf("%s\n", aBuffer);
		//AddOutputString_s(hListOutput,aBuffer);
		/*TRoleData *pRoleData = (TRoleData *)aBuffer;
		if  (pRoleData)
		AddOutputString_s(hListOutput,pRoleData->BaseInfo.szName);*/
	}

	delete[] aBuffer;
	return true;
}

bool mysql_db::UpdateRecord(const char *szSql)
 {
	bool bRet = false;
	MYSQL *pMySql = sqlCon;
	if (pMySql == NULL)
		return false;

	if(mysql_query(pMySql,szSql))
		bRet = true;

	return bRet;
}

bool mysql_db::DelRecord(const char *szSql)
{
	bool bRet = false;
	MYSQL *pMySql = sqlCon;
	if (pMySql == NULL)
		return false;

	if(mysql_query(pMySql,szSql))
		bRet = true;

	return bRet;
}
//

//mysql_db::mysql_db()
//{
//
//}
//
//mysql_db::~mysql_db()
//{
//	for (ITER_CONNECTION_HANDLE_LIST iter=m_lsBusyList.begin(); iter != m_lsBusyList.end(); iter++)
//		mysql_close((*iter));
//	for (ITER_CONNECTION_HANDLE_LIST iter=m_lsIdleList.begin(); iter != m_lsIdleList.end(); iter++)
//		mysql_close((*iter));
//	DeleteCriticalSection(&m_csList);
//}
//
//void mysql_db::mysql_db_Init(const char *host, const char *user, const char *password, const char *db, unsigned int port)
//{
//	strcpy(m_host,host);
//	strcpy(m_user,user);
//	strcpy(m_password, password);
//	strcpy(m_db, db);
//	m_port = port;
//	InitializeCriticalSection(&m_csList);
//}
//
//bool mysql_db::ConnectDB()
//{
////	同时打开CONNECTION_NUM个连接
//	try
//	{
//		for (int i=0; i<CONNECTION_NUM;i++)
//		{
//			MYSQL *pMySql = mysql_init((MYSQL*)NULL);
//			if (pMySql != NULL)
//			{
//				if (!mysql_real_connect(pMySql,m_host,m_user,m_password,m_db,m_port,NULL,0))
//				{
//					OutErrors(pMySql);
//					return false;
//				}
//				m_lsIdleList.push_back(pMySql);
//			}
//		}
//	}
//	catch (...)
//	{
//		return false;
//	}
//	return true;
//}
//
//MYSQL* mysql_db::GetIdleMySql()
//{
//	MYSQL* pMySql = NULL;
//	EnterCriticalSection(&m_csList);
//	if (m_lsIdleList.size() > 0)
//	{
//		pMySql = m_lsIdleList.front();
//		m_lsIdleList.pop_front();
//		m_lsBusyList.push_back(pMySql);
//	}
//	else
//	{
//		pMySql = NULL;
//	}
//	LeaveCriticalSection(&m_csList);
//	return pMySql;
//}
//
//void mysql_db::SetIdleMysql(MYSQL* pMySql)
//{
//	EnterCriticalSection(&m_csList);
//	m_lsBusyList.remove(pMySql);
//	m_lsIdleList.push_back(pMySql);
//	LeaveCriticalSection(&m_csList);
//}
//
//MYSQL_RES* mysql_db::SelectRecord(const char *szSql)
//{
//	MYSQL *pMySql = GetIdleMySql();
//	if (pMySql == NULL)
//		return NULL;
//	if(mysql_query(pMySql,szSql))
//		return NULL;
//	MYSQL_RES *myquery = NULL;
//	myquery = mysql_store_result(pMySql);
//	SetIdleMysql(pMySql);
//	return myquery;
//}
//
//bool mysql_db::InsertRecord(const char *szSql)
//{
//	bool bRet = false;
//	MYSQL *pMySql = GetIdleMySql();
//	if (pMySql == NULL)
//		return false;
//	if(mysql_query(pMySql,szSql))
//		bRet = true;
//	SetIdleMysql(pMySql);
//	return bRet;
//}
//
//bool mysql_db::UpdateRecord(const char *szSql)
//{
//	bool bRet = false;
//	MYSQL *pMySql = GetIdleMySql();
//	if (pMySql == NULL)
//	{
//		return false;
//	}
//	if(mysql_query(pMySql,szSql))
//	{
//		bRet = true;
//	}
//	SetIdleMysql(pMySql);
//	return bRet;
//}
//
//bool mysql_db::DelRecord(const char *szSql)
//{
//	bool bRet = false;
//	MYSQL *pMySql = GetIdleMySql();
//	if (pMySql == NULL)
//	{
//		return false;
//	}
//	if(mysql_query(pMySql,szSql))
//	{
//		bRet = true;
//	}
//	SetIdleMysql(pMySql);
//	return bRet;
//}
//
//bool mysql_db::SelectDB(const char *szDB)
//{
//	bool bRet = false;
//	MYSQL *pMySql = GetIdleMySql();
//	if (pMySql == NULL)
//	{
//		return false;
//	}
//	if (mysql_select_db(pMySql,szDB))
//		bRet = false;
//	else
//		bRet = true;
//	SetIdleMysql(pMySql);
//	return bRet;
//}
//
//my_ulonglong mysql_db::GetRowNum(MYSQL_RES *myquery)
//{
//	return mysql_num_rows(myquery);
//}
//
//MYSQL_ROW mysql_db::GetRecord(MYSQL_RES *myquery)
//{
//	m_row = mysql_fetch_row(myquery);
//	return m_row;
//}
//
//unsigned int mysql_db::GetFieldNum(MYSQL_RES *myquery)
//{
//	return mysql_num_fields(myquery);
//}
//
//void mysql_db::FreeRecord(MYSQL_RES *myquery)
//{
//	mysql_free_result(myquery);
//}
//
//int mysql_db::CreateDB(char *db)
//{
////return mysql_create_db(&m_mysql,db);
//	return 0;
//}
//
//void mysql_db::SeekData(MYSQL_RES *myquery, int offset)
//{
//	mysql_data_seek(myquery,offset);
//}
//
//char * mysql_db::OutErrors(MYSQL *pMySql)
//{
//	return NULL;//const_cast(mysql_error(pMySql));
//}
//
//BOOL mysql_db::IsEnd(MYSQL_RES *myquery)
//{
//	return mysql_eof(myquery);
//}
//
//char* mysql_db::GetFieldName(MYSQL_RES *myquery, int FieldNum)
//{
//	m_field = mysql_fetch_field_direct(myquery, FieldNum);
//	return m_field->name;
//}
//
//char * mysql_db::GetClientInfo()
//{
//	return (char *)mysql_get_client_info();
//}
//
//char* mysql_db::GetHostInfo()
//{
//	MYSQL *pMySql = GetIdleMySql();
//	if (pMySql == NULL)
//		return NULL;
//	return (char*)mysql_get_host_info(pMySql);
//}
//
//int mysql_db::GetProtocolInfo()
//{
//	int iRet = 0;
//	MYSQL *pMySql = GetIdleMySql();
//	if (pMySql == NULL)
//		return NULL;
//	iRet = mysql_get_proto_info(pMySql);
//	SetIdleMysql(pMySql);
//	return iRet;
//}
//
//char* mysql_db::GetServerInfo()
//{
//	static char szRet[1024];
//	MYSQL *pMySql = GetIdleMySql();
//	if (pMySql == NULL)
//		return NULL;
//	strcpy(szRet, mysql_get_server_info(pMySql));
//	SetIdleMysql(pMySql);
//	return szRet;
//}
//
//char* mysql_db::GetState()
//{
//	MYSQL *pMySql = GetIdleMySql();
//	if (pMySql == NULL)
//	{
//		return NULL;
//	}
//	static char szRet[1024];
//	strcpy(szRet,mysql_stat(pMySql));
//	SetIdleMysql(pMySql);
//	return szRet;
//}
//
//bool mysql_db::SetCharset()
//{
//	bool bRet = false;
//	char szSql[50];
//	strcpy(szSql, "set names gb2312");
//	MYSQL *pMySql = GetIdleMySql();
//	if (pMySql == NULL)
//	{
//		return false;
//	}
//	if (mysql_query(pMySql, szSql))
//		bRet = true;
//	SetIdleMysql(pMySql);
//	return bRet;
//}
//
////LOCK TABLES tbl1 READ, tbl2 WRITE
//
//bool mysql_db::LockTable(const char *TableName, const char *Priority)
//{
//	bool bRet = false;
//	char szSql[50];
//	sprintf(szSql, "LOCK TABLES %s %s", TableName, Priority);
//	MYSQL *pMySql = GetIdleMySql();
//	if (pMySql == NULL)
//	{
//		return false;
//	}
//	if (mysql_query(pMySql, szSql))
//		bRet = true;
//	SetIdleMysql(pMySql);
//	return bRet;
//}
//
//bool mysql_db::UnlockTable()
//{
//	bool bRet = false;
//	MYSQL *pMySql = GetIdleMySql();
//	if (pMySql == NULL)
//	{
//		return false;
//	}
//	if(mysql_query(pMySql,"UNLOCK TABLES"))
//		bRet = true;
//	SetIdleMysql(pMySql);
//	return bRet;
//}

//总结：
//	在针对大量数据的插入，更改等操作时，应该开启事务，待一连串的操作结束之后，
//  再提交事务，可提高程序执行效率。
//使用例子
//int main()
//{
//	mysql_db mydb;
//
//	//打开.. 连接
//	if (-1 == mydb.mysql_open("Localhost", "root", "123456", "electronicpolice", MYSQL_PORT))
//		cout << mydb.mysql_lasterror() << endl;
//
//	//创建一个数据库
//	string sql = "create database if not exists database_test";
//	if ( -1 == mydb.mysql_noResult_query(sql.c_str()))
//		cout << mydb.mysql_lasterror() << endl;
//
//	//use 库
//	sql = "use database_test";
//	if ( -1 == mydb.mysql_noResult_query(sql.c_str()))
//		cout << mydb.mysql_lasterror() << endl;
//
//	//创建一张表
//	sql = "create table if not exists table_test(id int not null auto_increment, name varchar(20) not null, age int not null ,primary key (id) )";
//	if ( -1 == mydb.mysql_noResult_query(sql.c_str()))
//		cout << mydb.mysql_lasterror() << endl;
//
//	//往表中插入2条数据
//	sql = "insert into table_test (name,age) values('newsoul','25')";
//	if ( -1 == mydb.mysql_noResult_query(sql.c_str()))
//		cout << mydb.mysql_lasterror() << endl;
//	sql = "insert into table_test (name,age) values('百度','100')";
//	if ( -1 == mydb.mysql_noResult_query(sql.c_str()))
//		cout << mydb.mysql_lasterror() << endl;
//
//
//	//mysql_select_SingleLine_query 演示
//	sql = "select * from table_test where id=1 ";
//	vector<string> vResults;
//	if (-1 == mydb.mysql_select_SingleLine_query(sql.c_str(),vResults))
//		cout << mydb.mysql_lasterror() << endl;
//
//	//遍历结果集
//	cout << "select * from table_test where id=1的结果: " << endl;
//	for (vector<string>::iterator it = vResults.begin(); it != vResults.end(); it++)
//		cout << (*it) << endl;
//
//
//	cout << endl << endl;
//	//mysql_select_query 演示
//	sql = "select * from table_test";
//
//	int i = 0;
//	while (i++ < 100){
//		map<int,vector<string>> map_res;
//		if (-1 == mydb.mysql_select_query(sql.c_str(),map_res))
//			cout << mydb.mysql_lasterror() << endl;
//
//
//		//遍历...
//		cout << "select * from table_test的结果:" << endl;
//		for (map<int,vector<string>>::iterator it = map_res.begin(); it != map_res.end(); it++){
//			cout << "第" << it->first+1<< "条 : ";
//			for (int i=0;i<it->second.size();i++)
//			{
//				cout << it->second[i] << "\t";
//			}
//			cout << endl;
//		}
//	}
//
//	system("pause");
//	return 0;
//}

void AddOutputString_s(HWND hListCtrl, char* aStr)
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



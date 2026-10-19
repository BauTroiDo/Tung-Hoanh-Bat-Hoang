#ifndef MYSQL_H
#define MYSQL_H
//定义支持复制、事务处理的，多索引记录的数据表类
#include "..\..\MySql\Include\mysql.h"
#include "malloc.h"
//#include <Winsock2.h>
//#include "mysql.h"
#include <string>
#include <vector>
//#include <strsafe.h>
#include <map>
//#include <stack>
//#include <list>
#include <queue>
using namespace std;  	


//typedef  std::list <MYSQL *> CONNECTION_HANDLE_LIST;
//typedef  CONNECTION_HANDLE_LIST::iterator ITER_CONNECTION_HANDLE_LIST;

#define CONNECTION_NUM 10 //同时打开的连接数

class mysql_db
{

public:
    mysql_db();
    ~mysql_db();
public:
	void  init();
	void  close();
    /*
        mysql_open()
        return : 1 OK 
                -1 失败  
     */
    bool mysql_open(const char * host, const char * user, const char * password, const char * database, unsigned int port );
    /*
        mysql_noResult_query();
        非select语句查询
        return >0 成功, 为受影响的行数
               -1 失败 
     */
    int mysql_noResult_query(const char * sql );
    /*
        mysql_select_query();
        有结果集的查询
        return >0 ok 返回结果集条数
               -1 失败
        map_results first = 行 second = values
        */
    int mysql_select_query(const char * sql, map<int,vector<string>> & map_results);
    /*
        mysql_select_SingleLine_query();
        只有一条数据 , 或者只有一个字段 N 条的查询. 直接调用vector即可
     */
    int mysql_select_SingleLine_query(const char * sql, vector<string> & v_results);
    /*
        mysql_lasterror();
        返回最近一次错误信息
     */
    string mysql_lasterror();

	int createDB(const char *szSql);//创建数据库，返回错误信息

	bool getRecord(const char *szSql);//更新记录
	bool UpdateRecord(const char *szSql);//更新记录
	bool InsertRecord(const char *szSql);//插入记录
	bool DelRecord(const char *szSql);//删除记录
	MYSQL_RES* SelectRecord(const char*szSql);//选择记录，返回结果集

	unsigned long EncodeString(char * pszDst, const char * pszSrc, int nLen);
	bool addBlobData(const char *roleName,const char *szSql,char *pData,int datalen);
private:
   bool getfieldvalue(char * value,int field_num,MYSQL_FIELD * fields,MYSQL_ROW &row,char *fieldname);


    MYSQL *sqlCon;
    MYSQL_RES *m_pResult;
    MYSQL_ROW  *m_Row;
	MYSQL_STMT *stmt;

	MYSQL_FIELD *m_fields;

};

void AddOutputString_s (HWND hListCtrl, char* aStr);//添加操作输出文字
//
//
//public:
//	
//	mysql_db();
//	virtual ~mysql_db();
//	void mysql_db_Init(const char *host, const char *user, const char *password, const char *db, unsigned int port=3306);
//	bool ConnectDB();//连接数据
//	MYSQL_RES* SelectRecord(const char*szSql);//选择记录，返回结果集
//	bool SelectDB(const char *szDB);//选择数据库
//	bool UpdateRecord(const char *szSql);//更新记录
//	bool InsertRecord(const char *szSql);//插入记录
//	bool DelRecord(const char *szSql);//删除记录
//	BOOL IsEnd(MYSQL_RES *myquery);//是否最后
//	void SeekData(MYSQL_RES *myquery, int offset);//查找指定数据
//	void FreeRecord(MYSQL_RES *myquery);//释放结果集
//	unsigned int GetFieldNum(MYSQL_RES *myquery);//得到字段数
//	MYSQL_ROW GetRecord(MYSQL_RES *myquery);//得到结果(一个记录)
//	my_ulonglong GetRowNum(MYSQL_RES *myquery);//得到记录数
//	char* OutErrors(MYSQL* pMySql);//输出错误信息
//	char* GetState();//服务器状态
//	char* GetServerInfo();//服务器信息
//	int     GetProtocolInfo();//协议信息
//	char* GetHostInfo();//主机信息
//	char* GetClientInfo();//客户机信息
//	char* GetFieldName(MYSQL_RES *myquery, int FieldNum);//字段名
//	bool LockTable(const char *TableName, const char *Priority);//对特定表加锁
//	bool UnlockTable();//解锁
//	bool SetCharset();
//	int CreateDB(char *db);//创建数据库，返回错误信息
//	//int DropDB(char *db);//删除数据库,返回错误信息
//	MYSQL* GetIdleMySql();//提取一个空闲句柄供使用
//	void SetIdleMysql(MYSQL* pMySql);//从使用队列中释放一个使用完毕的句柄，插入到空闲队列
//public:
//	//MYSQLm_mysql;//数据库连接句柄
//	MYSQL_ROW m_row;//记录集(单行)
//	MYSQL_FIELD *m_field;//字段信息(结构体)
//	//创建两个队列
//	CONNECTION_HANDLE_LIST m_lsBusyList;//正在使用的连接句柄
//	CONNECTION_HANDLE_LIST m_lsIdleList; //未使用的连接句柄
//	CRITICAL_SECTION m_csList;
//public:
//	char m_host[20];//主机
//	char m_user[20];//用户名
//	char m_password[20];//密码
//	char m_db[20];//数据库名
//	unsigned int m_port;//端口
//};
#endif

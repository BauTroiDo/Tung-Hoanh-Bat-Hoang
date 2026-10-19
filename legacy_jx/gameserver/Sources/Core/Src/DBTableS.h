#ifndef DBTABLES_H
#define DBTABLES_H
//定义支持复制、事务处理的，多索引记录的数据表类

//#include "db_cxx.h"
#include "db.h"
#include "malloc.h"

#define MAX_INDEX_S			16					//一个数据表最多的索引数目16
#define MAX_TABLE_NAME_S	64					    //最长的表名字

typedef int (*GetIndexFuncS)(DB *, const DBT *, const DBT *, DBT *);

typedef struct ZCursorS {
	bool bTravel;								//是否遍历
	DBC *dbcp;									//当前遍历使用的游标
	int index;									//当前的索引
	char *key;
	int key_size;
	char *data;									//返回的数据
	int size;									//数据的大小
}tagsZCursor;

class  ZDBTableS
{
	DB *primary_db;												//存放主键-数据的数据库
	DB *index_db[MAX_INDEX_S];									//存放索引-主键的数据库
	GetIndexFuncS get_index_funcs[MAX_INDEX_S];					//从数据中获得二级key的的函数列表
	bool is_index_unique[MAX_INDEX_S];							//索引是否唯一
	int index_number;											//二级索引数目
	char table_name[MAX_TABLE_NAME_S];
protected:
	char env_path[128];
	//DbEnv *dbenv;	
	DB_ENV *sdbenv;	//数据库环境
	ZCursorS *_search(bool bKey, const char *key_ptr, int key_size, int index);		//搜索指定记录
	bool _next(bool bKey, ZCursorS *cursor);															//下一个记录
public:
	bool bStop;
	ZDBTableS(const char *path, const char *name);			//环境目录和数据表的名字
	virtual ~ZDBTableS();
	
	int addIndex(GetIndexFuncS func, bool isUnique = false);	//增加索引项
	int open();												    //打开数据表
	void close();												//关闭数据表
	bool commit();												//基于事务的提交，目前使用自动提交，不使用

//基本记录操作
	bool add(const char *key_ptr, int key_size, const char *data_ptr, int data_size);
	bool remove(const char *key_ptr, int key_size, int index = -1);

	void closeCursor(ZCursorS *cursor) 
	{
		if(!cursor) return;
		/*if(cursor->bTravel) {
			free(cursor->key);
		}
		free(cursor->data);*/

		delete cursor;
		cursor=NULL;
	}
	ZCursorS *first() {											//遍历数据库，得到第一条记录	
		return _search(false, NULL, 0, -1);
	}
	ZCursorS *search(const char *key_ptr, int key_size, int index = -1){
		return _search(false, key_ptr, key_size, index);		//搜索指定记录
	}
	bool next(ZCursorS *cursor) {											//下一个记录
		return _next(false, cursor);
	}
	ZCursorS *search_key(const char *key_ptr, int key_size, int index = -1) {	//搜索指定记录，返回主键值
		return _search(true, key_ptr, key_size, index);
	}
	bool next_key(ZCursorS *cursor) {															//下一个记录，返回主键值
		return _next(true, cursor);
	}
//下面是一些维护性的操作
	void deadlock() {						//解除死锁
		sdbenv->lock_detect(sdbenv,0, DB_LOCK_DEFAULT, NULL);
	}
	BOOL removeLog();						//清除日志文件
};

#define MAX_RETRY_S	10                    //最大重试次数

//extern ZDBTableS* g_pDb;


#endif

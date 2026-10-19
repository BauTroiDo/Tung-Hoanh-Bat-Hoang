#include "stdafx.h"
#include "DBTable.h"
#ifndef WIN32
#include <sys/stat.h>
#else
#include <direct.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//数据库路径  和 名称
ZDBTable::ZDBTable(const char *path, const char *name) 
{
	memset(index_db, 0, sizeof(index_db));
	primary_db = NULL;
	#ifdef WIN32
	   //_getcwd(env_path, 128);
    char * buffer=NULL;
    if((buffer=_getcwd(NULL,0))==NULL)
		return;
	sprintf_s(env_path,sizeof(env_path)-1,buffer);
	#endif

	strcat_s(env_path, "\\"); 
	strcat_s(env_path, path);
	int ret = _mkdir(env_path);
    //gdbenv=NULL;
	//MessageBox(NULL,env_path,"TEST",MB_OK | MB_ICONSTOP );
	//gdbenv = new DB_ENV();
	//创建并设置数据库环境
	if(!db_env_create(&gdbenv,0)) 
	//if (gdbenv)
	{ 
		//(void)gdbenv->set_data_dir(gdbenv,"database");
		gdbenv->set_errpfx(gdbenv,"index_db");
		gdbenv->set_lg_regionmax(gdbenv,512*1024);         //512            区域的大小 用于存放名字
		gdbenv->set_lg_max(gdbenv,16*1024*1024);             //16M	          日志容量	 1G
		gdbenv->set_lg_bsize(gdbenv,2*1024*1024);            //2M             V S 2010会崩溃
		gdbenv->set_cachesize(gdbenv,0,64*1024*1024,1); //64M          缓存的大小
		
		if(!gdbenv->open(gdbenv,path,DB_CREATE|DB_INIT_LOG|DB_INIT_MPOOL|DB_INIT_TXN|DB_RECOVER|DB_THREAD|DB_PRIVATE,0))  //DB_INIT_LOCK 如果用这个则不能对数据删除操作
		{//open成功了 = 0;
			gdbenv->set_flags(gdbenv,DB_AUTO_COMMIT|DB_TXN_NOSYNC,1);
			index_number = 0;           //设置为0？
			strcpy_s(table_name, name);
			//MessageBox(NULL,env_path,table_name,MB_OK | MB_ICONSTOP );

			return;//成功了
		}
		if (gdbenv)
		  gdbenv->close(gdbenv,0);
	}
	gdbenv = NULL;
}

ZDBTable::~ZDBTable()
{
	//if(gdbenv) 
	//	gdbenv->close(0);

	if(!gdbenv)
		return;

	for(int index = 0; index < index_number; ++index) 
	{
		if (index_db[index]) index_db[index]->close(index_db[index],0);
		//delete index_db[index];
		index_db[index]=NULL;
	}

	if (primary_db)
	{
		primary_db->sync(primary_db,0);

		primary_db->close(primary_db,0);
		primary_db=NULL;
	}

	gdbenv->close(gdbenv,0);
	//delete gdbenv;
	gdbenv=NULL;
}

int ZDBTable::addIndex(GetIndexFunc func, bool isUnique) 
{
	if(!gdbenv) 
		return -1;

	if(index_number + 1 >= MAX_INDEX) 
		return index_number;

	get_index_funcs[index_number] = func;
	is_index_unique[index_number] = isUnique;

	return index_number++;
}

int ZDBTable::open() 
{
	if(!gdbenv) return -1;

	bStop = false;
	char index_table_name[MAX_TABLE_NAME];
	int index=0;
	int ret;
	primary_db=NULL;
	//primary_db= new Db(gdbenv, 0);
	if(!db_create(&primary_db,gdbenv,0)) //创建一个数据库
	//if (primary_db)
	{
		ret=primary_db->open(primary_db,NULL,table_name,NULL,DB_BTREE,DB_CREATE|DB_AUTO_COMMIT|DB_THREAD,0664);
		if(!ret)
		{	//打开主数据库
			for(index = 0; index < index_number; ++index) 
			{
				sprintf(index_table_name,"%s.%d",table_name,index);	 //roledb.0

				if(!db_create(&index_db[index], gdbenv, 0))
				{//如果创建不了,就不创建
					if(!is_index_unique[index])	//默认为 false
					{
						if(index_db[index]->set_flags(index_db[index],DB_DUP|DB_DUPSORT))//DB_NEXT_DUP
							break;
					}

					if(index_db[index]->open(index_db[index],NULL,index_table_name,NULL,DB_BTREE,DB_BTREE|DB_AUTO_COMMIT|DB_THREAD,0664))	//DB_BTREE| DB_AUTO_COMMIT | DB_THREAD
						break;

					if(primary_db->associate(primary_db,NULL,index_db[index],get_index_funcs[index],DB_AUTO_COMMIT))	   //DB_AUTO_COMMIT
					{
						index_db[index]->close(index_db[index],0);
		//				delete index_db[index];
						index_db[index]=NULL;
						break;
					}
				}
				else
					break;
			}

			if(index <= index_number) 
				return 1;										    //成功了
			else
			{
				while(--index>0)
				{
					(index_db[index])->close(index_db[index],0);                       //出错，关闭前面的索引表
			//		delete index_db[index];
					index_db[index]=NULL;
				}
			}

			primary_db->close(primary_db,0);
			//delete primary_db;
			primary_db=NULL;
			return -4;
		}
		else
		{
			//primary_db->err(primary_db, ret, "%s", table_name); 
			return -3;
		}
	}
	return -2;
}

void ZDBTable::close() 
{
	/*if(!gdbenv)
		return;
	for(int index = 0; index < index_number; ++index) 
	{
		index_db[index]->close(index_db[index],0);
		delete index_db[index];
		index_db[index]=NULL;
	}

	primary_db->close(primary_db,0);
	delete primary_db;
	primary_db=NULL;

	gdbenv->close(gdbenv,0);
	delete gdbenv;
	gdbenv=NULL;*/

	if(!gdbenv) return;
	if (primary_db) primary_db->close(primary_db, 0);

	for(int index = 0; index < index_number; index++) {
		if (index_db[index]) {
			index_db[index]->close(index_db[index], 0);
			index_db[index] = NULL;
		}
	}

	gdbenv->close(gdbenv,0);
	gdbenv=NULL;
}

bool ZDBTable::commit()
{
	return true;
}
//存档写入
bool ZDBTable::add(const char *key_ptr, int key_size, const char *data_ptr, int data_size) 
{
	if (!primary_db)
		return false;

	DBT data, key;
	memset(&key, 0, sizeof(DBT));
	memset(&data, 0, sizeof(DBT));
	
	key.data = (void *)key_ptr;    //角色名
	key.size = key_size;
	data.data = (void *)data_ptr;  //角色数据
	data.size = data_size; 

	int ret;
	int retry = 0;
	//gdbenv->lock_detect(gdbenv,NULL,DB_LOCK_DEFAULT,); //锁检测
	//gdbenv->lock_put(gdbenv, &lock);                   //释放锁
RETRY:
	ret = primary_db->put(primary_db,NULL,&key,&data,DB_AUTO_COMMIT);  // DB_AUTO_COMMIT		  //DB_NOOVERWRITE（存在就不写入了）

	if(ret == DB_LOCK_DEADLOCK && retry++ < MAX_RETRY) 
	{//如果是锁住的，强行回滚解锁
		if(bStop) 
			return false;

		goto RETRY;//循环写入
	}

	if(ret) 
		return false;
	else
	{
		/*if (primary_db)
			primary_db->sync(primary_db,0);*/
		return true;
	}
}

bool ZDBTable::remove(const char *key_ptr, int key_size, int index) {
	if (!primary_db)
		return false;

	DBT data, key;
	memset(&key, 0, sizeof(DBT));
	memset(&data, 0, sizeof(DBT));

	key.data = (void *)key_ptr;
	key.size = key_size;

	int ret;
	int retry = 0;
RETRY:
	ret = primary_db->del(primary_db,NULL, &key,DB_AUTO_COMMIT);	  //DB_AUTO_COMMIT
	if(ret == DB_LOCK_DEADLOCK && ++retry < MAX_RETRY)
	{//强行解锁
		if(bStop) return false;
		goto RETRY;
	}
	if(ret) return false;
	else
	{ 
		//if (primary_db)
		//   primary_db->sync(primary_db,0);

		return true;
	}
}

ZCursor *ZDBTable::_search(bool bisKey, const char *key_ptr, int key_size, int index) 
{
	DBT key, data, pkey;
	if(index < -1 || index >= index_number)
	  return NULL;

	memset(&key, 0, sizeof(key));
	memset(&data, 0, sizeof(data));
	memset(&pkey, 0, sizeof(pkey));

	key.flags = DB_DBT_MALLOC;
	data.flags = DB_DBT_MALLOC;
	pkey.flags = DB_DBT_MALLOC;
	key.data = (void *)key_ptr;
	key.size = key_size;

	DBC *dbcp=NULL;


	//char nMsg[128]={0};
	//sprintf_s(nMsg,sizeof(nMsg)-1,"搜索:%s",key_ptr);
    //::MessageBox(NULL,nMsg, "Warning", MB_OK | MB_ICONSTOP );

	if(!key_ptr || !key_size) //没有指定数据的
	{//没有设定索引值，要求遍历数据库  
		if(index == -1)
		{//主数据库搜索
			if (!primary_db || primary_db->cursor(primary_db,NULL, &dbcp, 0))
			{
				return NULL;
			}
			
			if (dbcp->c_get(dbcp,&key, &data,DB_FIRST))
			{
				dbcp->c_close(dbcp);
				return NULL;
			}
		}
		else
		{
			if(!index_db[index] || index_db[index]->cursor(index_db[index],NULL, &dbcp, 0))
			{
				return NULL;
			}
			
			if (dbcp->c_get(dbcp,&key, &data,DB_FIRST)) 
			{
				dbcp->c_close(dbcp);
				return NULL;
			}

		}	
	}
	else 
	{//指定关键字查询
		if(index == -1)
		{//主数据库搜索  
			if (!primary_db)
				return NULL;

			if(bisKey)
			{//主数据库
				if(primary_db->pget(primary_db,NULL, &key, &pkey, &data, 0))
					return NULL;
			}
			else
			{		
				if (primary_db->get(primary_db,NULL, &key, &data, 0))
					return NULL;
			}
		}
		else if (is_index_unique[index]) //默认为 false
		{//没有重复索引
			if (!index_db[index])
				return NULL;

			if(bisKey)
			{
				if(index_db[index]->pget(index_db[index],NULL, &key, &pkey, &data, 0))
					return NULL;
			}
			else 
			{
				if (index_db[index]->get(index_db[index],NULL, &key, &data, 0)) 
					return NULL;
			}
		}
		else
		{//打开游标	二级数据库  从index0 数据库搜索
			if (!index_db[index])
				return NULL;

			if (index_db[index]->cursor(index_db[index],NULL, &dbcp, 0))
			{
				return NULL;
			}

			if (bisKey)
			{
				if(dbcp->c_pget(dbcp,&key, &pkey, &data, DB_SET))
				{
					dbcp->c_close(dbcp);
					return NULL;
				}
			}
			else 
			{
				if(dbcp->c_get(dbcp,&key, &data, DB_SET)) 
				{
					dbcp->c_close(dbcp);
					return NULL;
				}
			}
		}
	}

	ZCursor *result = new ZCursor;

	if (!result)
		return NULL;

	result->index = 0;
	result->dbcp = dbcp;

	if (bisKey) 
	{
		result->data = (char *)pkey.data;
		result->size = pkey.size;
        pkey.data    = NULL;

	}
	else 
	{
		result->data = (char *)data.data;
		result->size = data.size;
        data.data    = NULL;
	}

	if (!key_ptr || !key_size) 
	{//如果没有关键字 就历遍
		result->key = (char *)key.data;
		result->key_size = key.size;
		result->bTravel = true; //厉遍
		key.data = NULL; 
	}
	else 
	{//不历遍
		result->bTravel = false;
	}
	//-----原来是注销的
	/*if(pkey.data) free(pkey.data);
	if(data.data) free(data.data);*/

	return result;
}


bool ZDBTable::_next(bool bisKey, ZCursor *cursor) 
{
	DBT key, data,pkey;

	memset(&key, 0, sizeof(key));
	memset(&data, 0, sizeof(data));
	memset(&pkey, 0, sizeof(pkey));
   
	key.flags = DB_DBT_MALLOC;
	data.flags = DB_DBT_MALLOC;
	pkey.flags = DB_DBT_MALLOC;

	if(!cursor || !cursor->dbcp) 
	   return false;
//-----原来是注销的
	//if (cursor->data)
	//{//有BUG？崩溃了 
	//   free(cursor->data); //释放原来的数据
	//   cursor->data = NULL;
	//}

	cursor->index++;

	//char nMsg[128]={0};
	//sprintf_s(nMsg,sizeof(nMsg)-1,"_next下一个记录搜索:%s",cursor->key);
	//::MessageBox(NULL,nMsg, "Warning", MB_OK | MB_ICONSTOP );

	if(cursor->bTravel) {
		//-----原来是注销的
		//free(cursor->key);
		//----------------------
		if(cursor->dbcp->c_get(cursor->dbcp,&key, &data, DB_NEXT)) {
			cursor->dbcp->c_close(cursor->dbcp);
			cursor->dbcp=NULL;
			delete cursor;
			cursor =NULL;
			return false;
		}
	}
	else {
		if(bisKey) {
			if(cursor->dbcp->c_pget(cursor->dbcp,&key, &pkey, &data, DB_NEXT_DUP)) {
				cursor->dbcp->c_close(cursor->dbcp);
				cursor->dbcp=NULL;
				delete cursor;
				cursor =NULL;
				return false;
			}
		}
		else {
			if(cursor->dbcp->c_get(cursor->dbcp,&key, &data, DB_NEXT_DUP)) {
				cursor->dbcp->c_close(cursor->dbcp);
				cursor->dbcp=NULL;
				delete cursor;
				cursor =NULL;
				return false;
			}
		}
	}

	if (bisKey) 
    {	
		cursor->data = (char *)pkey.data;
		cursor->size = pkey.size;
        pkey.data = NULL;
	}
	else {
		cursor->data = (char *)data.data;
		cursor->size = data.size;
        data.data = NULL;

	}
	
    if (cursor->bTravel) 
    {
		cursor->key = (char *)key.data;
		cursor->key_size = key.size;
        key.data = NULL;
	}
	
	//sprintf_s(nMsg,sizeof(nMsg)-1,"_nextB下一个记录搜索:%s",cursor->key);
	//::MessageBox(NULL,nMsg, "Warning", MB_OK | MB_ICONSTOP );
	//-----原来是注销的
	/*if (key.data)  free(key.data);
	if (pkey.data)   free(pkey.data);
	if (data.data)   free(data.data);*/
	
    return true;
}


BOOL ZDBTable::removeLog() 
{						//清除日志文件
	int ret;
	char **begin, **list;

	if (gdbenv==NULL)
		return FALSE;

	if((ret = gdbenv->txn_checkpoint(gdbenv,0,0,0)) != 0) 	        //
		return FALSE;

	//primary_db->db_checkpoint();

	if((ret = gdbenv->log_archive(gdbenv,&list,DB_ARCH_ABS))!= 0) 	//DB_ARCH_LOG(返回所有日志名,不管是否在使用的) DB_ARCH_ABS（返回绝对路径名（包括盘符号）(除了正在使用的)）
	{//归档日志	   DB_ARCH_REMOVE （自动删除没有用的日志 没有返回）
		return FALSE;
	}

	if (list!=NULL) 
	{
		for(begin = list; *list != NULL; ++list)
		{
			if((ret =::remove(*list))!= 0) //是否删除成功
			{
				free(begin);
				continue;
			}
		}
		//free(begin);
	}
	 return	TRUE;
	}

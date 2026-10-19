/*#include "stdafx.h"
#include "DBTable.h"
#ifndef WIN32
#include <sys/stat.h>
#else
#include <direct.h>
#endif

#include <stdio.h>
#include <string.h>*/

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

ZDBTable::ZDBTable(const char *path, const char *name) {
	memset(index_db, 0, sizeof(index_db));
	primary_db = NULL;
#ifdef WIN32
	//_getcwd(env_path, 128);
	char * buffer=NULL;
	if((buffer=_getcwd(NULL,0))==NULL)
		return;
	sprintf_s(env_path,sizeof(env_path)-1,buffer);
#endif

	strcat_s(env_path, "\\");  //VS 2010 不需要加双斜杠
	strcat_s(env_path, path);

#ifdef WIN32
	int ret = _mkdir(env_path);
#else
	int ret = mkdir(env_path, 0);
#endif

	dbcp = NULL;//初始化游标（by Fellow）
	
	if(!db_env_create(&sdbenv, 0)) {
			sdbenv->set_errpfx(sdbenv, "index_db");
			if(!sdbenv->open(sdbenv, path, DB_CREATE | DB_INIT_LOG | DB_INIT_MPOOL | DB_INIT_TXN | DB_RECOVER, 0)) {
				sdbenv->set_flags(sdbenv,DB_AUTO_COMMIT |DB_CDB_ALLDB| DB_TXN_NOSYNC,1);
				index_number = 0;
				strcpy_s(table_name, name);
				return;				//成功了
			}
		sdbenv->close(sdbenv, 0);
	}
	sdbenv = NULL;
}

ZDBTable::~ZDBTable() {
	if(sdbenv) sdbenv->close(sdbenv, 0);
}

int ZDBTable::addIndex(GetIndexFunc func, bool isUnique) {
	if(!sdbenv) return -1;
	if(index_number + 1 >= MAX_INDEX) return index_number;
	get_index_funcs[index_number] = func;
	is_index_unique[index_number] = isUnique;
	return index_number++;
}

bool ZDBTable::open() {
	if(!sdbenv) return false;
	char index_table_name[MAX_TABLE_NAME];
    int index;
	int ret;
	if(!db_create(&primary_db, sdbenv, 0)) {
		if(!primary_db->open(primary_db, NULL, table_name, NULL, DB_BTREE, DB_CREATE|DB_AUTO_COMMIT, 0664)) {	//打开主数据库
			for(index = 0; index < index_number; index++) {
				sprintf_s(index_table_name,sizeof(index_table_name)-1, "%s.%d", table_name, index);
				if(!db_create(&index_db[index], sdbenv, 0)) {
					if(!is_index_unique[index]) {
						if(index_db[index]->set_flags(index_db[index], DB_DUP | DB_DUPSORT)) break;
					}
					if(index_db[index]->open(index_db[index], NULL, index_table_name, NULL, DB_BTREE, DB_CREATE|DB_AUTO_COMMIT, 0664)) break;
					if(ret = primary_db->associate(primary_db, NULL, index_db[index], get_index_funcs[index], DB_AUTO_COMMIT)) {
						index_db[index]->close(index_db[index], 0);
						break;
					}
				}
				else break;
			}
			if(index == index_number) return true;										//成功了
			else while(--index>0) (index_db[index])->close(index_db[index], 0);				//出错，关闭前面的索引表
			primary_db->close(primary_db, 0);
		}
	}
	return false;
}

void ZDBTable::close() {
	if(!sdbenv) return;
	if (primary_db) primary_db->close(primary_db, 0);
	for(int index = 0; index < index_number; index++) {
		if (index_db[index]) {
			index_db[index]->close(index_db[index], 0);
			index_db[index] = NULL;
		}
	}
}

bool ZDBTable::commit() {
/*	if(!sdbenv) return false;
	int ret;
	DB_TXN *tid;
	if((ret = sdbenv->txn_begin(sdbenv, NULL, &tid, 0)) != 0) return false;
	if((ret = primary_db->sync(primary_db, 0)) == 0) {
		ret = tid->commit(tid, 0);
		if(!ret) return true;
	}
	else {
		tid->abort(tid);
		return false;
	}*/
	return true;
}

bool ZDBTable::add(const char *key_ptr, int key_size, const char *data_ptr, int data_size) {
	DBT data, key;
	memset(&key, 0, sizeof(DBT));
	memset(&data, 0, sizeof(DBT));
	key.data = (void *)key_ptr;
	key.size = key_size;
	data.data = (void *)data_ptr;
	data.size = data_size;

	if(primary_db && !primary_db->put(primary_db, NULL, &key, &data, DB_AUTO_COMMIT)) {
      //  primary_db->sync(primary_db,0);
		return true;
	}
	return false;
}

bool ZDBTable::remove(const char *key_ptr, int key_size, int index) {
	DBT data, key;
	memset(&key, 0, sizeof(DBT));
	memset(&data, 0, sizeof(DBT));
	key.data = (void *)key_ptr;
	key.size = key_size;
	if(primary_db && !primary_db->del(primary_db, NULL, &key, DB_AUTO_COMMIT)) {
		return true;
	}
	return false;
}

int ZDBTable::GetRecordCount()
{
	int nCount=0;
	DBT key, data, pkey;
	memset(&key, 0, sizeof(key));
	memset(&data, 0, sizeof(data));
	memset(&pkey, 0, sizeof(pkey));

	DBC *dbcpa=NULL;
	if(!dbcpa)
	{ 
		if (!primary_db || primary_db->cursor(primary_db,NULL,&dbcpa,0))
		{
			dbcpa = NULL;
			return false;
		}

		while(dbcpa->c_get(dbcpa,&key,&data,DB_NEXT)==0)
		{
			nCount++;
		}
	}

	dbcpa->c_close(dbcpa);
///	primary_db->sync(0);

	return	nCount;
}

char *ZDBTable::_search(bool bKey, const char *key_ptr, int key_size, int &size, int index) {
	dbcp = NULL;
	DBT key, data, pkey;
	if(index < -1 || index >= index_number) return NULL;
	memset(&key, 0, sizeof(key));
	memset(&data, 0, sizeof(data));
	memset(&pkey, 0, sizeof(pkey));
	key.data = (void *)key_ptr;
	key.size = key_size;
	if(index == -1) {									//主键搜索
		if(!primary_db || primary_db->get(primary_db, NULL, &key, &data, 0)) return NULL;
	}
	else if(is_index_unique[index]) {					//没有重复索引
		if(bKey) {
			if(!index_db[index] || index_db[index]->pget(index_db[index], NULL, &key, &pkey, &data, 0)) return NULL;
		}
		else {
			if(!index_db[index] || index_db[index]->get(index_db[index], NULL, &key, &data, 0)) return NULL;
		}
	}
	else {												//打开游标
		if(!index_db[index] || index_db[index]->cursor(index_db[index], NULL, &dbcp, 0)) {
			dbcp = NULL;
			return NULL;
		}
		if(bKey) {
			if(dbcp->c_pget(dbcp, &key, &pkey, &data, DB_SET)) {
				dbcp->c_close(dbcp);
				dbcp = NULL;
				return NULL;
			}
		}
		else {
			if(dbcp->c_get(dbcp, &key, &data, DB_SET)) {
				dbcp->c_close(dbcp);
				dbcp = NULL;
				return NULL;
			}
		}

	}
	char *result;
	if(bKey) {
		result = new char[pkey.size];
		memmove(result, pkey.data, pkey.size);
		size = pkey.size;
	}
	else {
		result = new char[data.size];
		memmove(result, data.data, data.size);
		size = data.size;
	}
	return result;
}

char *ZDBTable::_next(bool bKey, int &size) {
	if(!dbcp) return NULL;
	DBT key, data, pkey;
	memset(&key, 0, sizeof(key));
	memset(&data, 0, sizeof(data));
	memset(&pkey, 0, sizeof(pkey));
	if(bKey) {
		if(dbcp->c_pget(dbcp, &key, &pkey, &data, DB_NEXT_DUP)) {
			dbcp->c_close(dbcp);
			dbcp = NULL;
			return NULL;
		}
	}
	else {
		if(dbcp->c_get(dbcp, &key, &data, DB_NEXT_DUP)) {
			dbcp->c_close(dbcp);
			dbcp = NULL;
			return NULL;
		}
	}
	char *result;
	if(bKey) {
		result = new char[pkey.size];
		memmove(result, pkey.data, pkey.size);
		size = pkey.size;
	}
	else {
		result = new char[data.size];
		memmove(result, data.data, data.size);
		size = data.size;
	}
	return result;
}

char *ZDBTable::GetRecord(int &size, CursorPointer cpMode, int index )
{//取得按游标某一个数据
	if(!dbcp)
	{//如果数据库指针没有初始化，先初始化dbcp
		if(index < 0 || index >= index_number || !index_db[index] || index_db[index]->cursor(index_db[index], NULL, &dbcp, 0))
		{//初始化dbcp失败
			dbcp = NULL;
			return NULL;
		}
	}
	DBT key, data, pkey;
	memset(&key, 0, sizeof(key));
	memset(&data, 0, sizeof(data));
	memset(&pkey, 0, sizeof(pkey));
	
	if(dbcp->c_get(dbcp, &key, &data, cpMode)) {
			dbcp->c_close(dbcp);
			dbcp = NULL;
			return NULL;
		}

	char *result;
	result = new char[data.size];
	memmove(result, data.data, data.size);
	size = data.size;
	
	return result;
}


char *ZDBTable::GetRecord_key(int &size, CursorPointer cpMode, int index )
{
	if(!dbcp)
	{//如果数据库指针没有初始化，先初始化dbcp
		if(index < 0 || index >= index_number || !index_db[index] || index_db[index]->cursor(index_db[index], NULL, &dbcp, 0))
		{//初始化dbcp失败
			dbcp = NULL;
			return NULL;
		}
	}
	DBT key, data, pkey;
	memset(&key, 0, sizeof(key));
	memset(&data, 0, sizeof(data));
	memset(&pkey, 0, sizeof(pkey));
	
	if(dbcp->c_get(dbcp, &key, &data, cpMode)) {
			dbcp->c_close(dbcp);
			dbcp = NULL;
			return NULL;
		}

	char *result;
	result = new char[key.size];
	memmove(result, key.data, key.size);
	size = key.size;
	
	return result;
}

bool ZDBTable::GetRecordEx(char* aBuffer, int &size, int buf_capacity,
						   char* aKeyBuffer, int &keysize, int key_capacity,
						   CursorPointer cpMode, int index)
{//取得按游标某一个数据(新版函数)
	if(!dbcp)
	{//如果数据库指针没有初始化，先初始化dbcp
		if(index < 0 || index >= index_number || !index_db[index] || index_db[index]->cursor(index_db[index], NULL, &dbcp, 0))
		{//初始化dbcp失败
			dbcp = NULL;
			return false;
		}
	}
	DBT key, data, pkey;
	memset(&key, 0, sizeof(key));
	memset(&data, 0, sizeof(data));
	memset(&pkey, 0, sizeof(pkey));
	
	if(dbcp->c_get(dbcp, &key, &data, cpMode)) {
			dbcp->c_close(dbcp);
			dbcp = NULL;
			return false;
		}

	if (buf_capacity < 0 || key_capacity < 0
		|| (unsigned)data.size > (unsigned)buf_capacity
		|| (unsigned)key.size > (unsigned)key_capacity)
	{
		dbcp->c_close(dbcp);
		dbcp = NULL;
		size = 0;
		keysize = 0;
		return false;
	}
	memmove(aBuffer, data.data, data.size);
	size = data.size;
	
	memmove(aKeyBuffer, key.data, key.size);
	keysize = key.size;

	return true;
}

CDBTableReadOnly::CDBTableReadOnly(const char *path, const char *name) {
   #ifdef WIN32
		if (!getcwd(env_path, MAX_TABLE_NAME))
		{
			// nothing was written when the name did not fit; the two concatenations below still
			// build a directory inside the one the process is in
			env_path[0] = '.';
			env_path[1] = 0;
		}
   //#else
   #endif
		strcat(env_path, "\\");
		strcat(env_path, path);
   #ifdef WIN32
		int ret = mkdir(env_path);
   #else
        int ret = mkdir(env_path, 0);
   #endif
	dbcp = NULL;//初始化游标（by Fellow）

	primary_db = NULL;
	if(!db_env_create(&cdbenv, 0)) {
			cdbenv->set_errpfx(cdbenv, "index_db");
			if(!cdbenv->open(cdbenv, path, DB_USE_ENVIRON | DB_INIT_MPOOL, 0)) {
				strcpy_s(table_name, name);
				return;				//成功了
			}
		cdbenv->close(cdbenv, 0);
	}
	cdbenv = NULL;
}

CDBTableReadOnly::~CDBTableReadOnly() {
	if(cdbenv) cdbenv->close(cdbenv, 0);
}

bool CDBTableReadOnly::open() {
	if(!cdbenv) return false;
	if(!db_create(&primary_db, cdbenv, 0)) {
		if(!primary_db->open(primary_db, NULL, table_name, NULL, DB_BTREE, DB_RDONLY, 0664)) {	//打开主数据库
			return true;
		}
	}
	return false;
}	

void CDBTableReadOnly::close() {
	if(!cdbenv) return;
	if (primary_db) primary_db->close(primary_db, 0);
}

char *CDBTableReadOnly::_search(bool bKey, const char *key_ptr, int key_size, int &size) {
	dbcp = NULL;
	DBT key, data, pkey;
	memset(&key, 0, sizeof(key));
	memset(&data, 0, sizeof(data));
	memset(&pkey, 0, sizeof(pkey));
	key.data = (void *)key_ptr;
	key.size = key_size;
	if(!primary_db || primary_db->get(primary_db, NULL, &key, &data, 0)) return NULL;

	char *result;
	if(bKey) {
		result = new char[pkey.size];
		memmove(result, pkey.data, pkey.size);
		size = pkey.size;
	}
	else {
		result = new char[data.size];
		memmove(result, data.data, data.size);
		size = data.size;
	}
	return result;
}

char *CDBTableReadOnly::_next(bool bKey, int &size) {
	if(!dbcp) return NULL;
	DBT key, data, pkey;
	memset(&key, 0, sizeof(key));
	memset(&data, 0, sizeof(data));
	memset(&pkey, 0, sizeof(pkey));
	if(bKey) {
		if(dbcp->c_pget(dbcp, &key, &pkey, &data, DB_NEXT_DUP)) {
			dbcp->c_close(dbcp);
			dbcp = NULL;
			return NULL;
		}
	}
	else {
		if(dbcp->c_get(dbcp, &key, &data, DB_NEXT_DUP)) {
			dbcp->c_close(dbcp);
			dbcp = NULL;
			return NULL;
		}
	}
	char *result;
	if(bKey) {
		result = new char[pkey.size];
		memmove(result, pkey.data, pkey.size);
		size = pkey.size;
	}
	else {
		result = new char[data.size];
		memmove(result, data.data, data.size);
		size = data.size;
	}
	return result;
}

char *CDBTableReadOnly::GetRecord(int &size, CursorPointer cpMode)
{
	if(!dbcp)
	{//如果数据库指针没有初始化，先初始化dbcp
		if(!primary_db || primary_db->cursor(primary_db, NULL, &dbcp, 0))
		{//初始化dbcp失败
			dbcp = NULL;
			return NULL;
		}
	}
	DBT key, data, pkey;
	memset(&key, 0, sizeof(key));
	memset(&data, 0, sizeof(data));
	memset(&pkey, 0, sizeof(pkey));
	
	if(dbcp->c_get(dbcp, &key, &data, cpMode)) {
			dbcp->c_close(dbcp);
			dbcp = NULL;
			return NULL;
		}

	char *result;
	result = new char[data.size];
	memmove(result, data.data, data.size);
	size = data.size;
	
	return result;
}

char *CDBTableReadOnly::GetRecord_key(int &size, CursorPointer cpMode)
{
	if(!dbcp)
	{//如果数据库指针没有初始化，先初始化dbcp
		if(!primary_db || primary_db->cursor(primary_db, NULL, &dbcp, 0))
		{//初始化dbcp失败
			dbcp = NULL;
			return NULL;
		}
	}
	DBT key, data, pkey;
	memset(&key, 0, sizeof(key));
	memset(&data, 0, sizeof(data));
	memset(&pkey, 0, sizeof(pkey));
	
	if(dbcp->c_get(dbcp, &key, &data, cpMode)) {
			dbcp->c_close(dbcp);
			dbcp = NULL;
			return NULL;
		}

	char *result;
	result = new char[key.size];
	memmove(result, key.data, key.size);
	size = key.size;
	
	return result;
}

bool CDBTableReadOnly::GetRecordEx(char* aBuffer, int &size, int buf_capacity,
						   char* aKeyBuffer, int &keysize, int key_capacity,
						   CursorPointer cpMode)
{//取得按游标某一个数据(新版函数)
	if(!dbcp)
	{//如果数据库指针没有初始化，先初始化dbcp
		if(!primary_db || primary_db->cursor(primary_db, NULL, &dbcp, 0))
		{//初始化dbcp失败
			dbcp = NULL;
			return false;
		}
	}
	DBT key, data, pkey;
	memset(&key, 0, sizeof(key));
	memset(&data, 0, sizeof(data));
	memset(&pkey, 0, sizeof(pkey));
	
	if(dbcp->c_get(dbcp, &key, &data, cpMode)) {
			dbcp->c_close(dbcp);
			dbcp = NULL;
			return false;
		}

	if (buf_capacity < 0 || key_capacity < 0
		|| (unsigned)data.size > (unsigned)buf_capacity
		|| (unsigned)key.size > (unsigned)key_capacity)
	{
		dbcp->c_close(dbcp);
		dbcp = NULL;
		size = 0;
		keysize = 0;
		return false;
	}
	memmove(aBuffer, data.data, data.size);
	size = data.size;
	
	memmove(aKeyBuffer, key.data, key.size);
	keysize = key.size;

	return true;
}
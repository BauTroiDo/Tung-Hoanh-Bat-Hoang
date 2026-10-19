// DBDumpLoad.h: interface for the CDBDump class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DBDUMPLOAD_H__956A6AC3_33CD_44D2_B4B2_771DE290B32F__INCLUDED_)
#define AFX_DBDUMPLOAD_H__956A6AC3_33CD_44D2_B4B2_771DE290B32F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "S3DBInterface.h"
#include "DBTable.h"

class CDBDump  
{
private:
	char m_FilePath[MAX_PATH];							//文件名
	FILE* m_output;
	bool m_IsOpened;
	bool m_bRollbackFailed;   // a record the file could not erase is still in it
	//unsigned long offset;
public:
	CDBDump();
	virtual ~CDBDump(){};
	bool Open(char* aFilePath);	//打开包文件
	bool Close();								//保存包文件
	bool AddData(char* key, const UINT keysize, char *aData, const UINT size);		//添加数据

};

class CDBLoad
{
private:
	char m_FilePath[MAX_PATH];							//文件名
	FILE* m_output;
	bool m_IsOpened;
	int	    SavePlayerBaseInfoDB(BYTE * pRoleBuffer,char *bakRoleData);
	int		SavePlayerItemListDB(BYTE * pRoleBuffer, UINT room, char *bakRoleData, UINT bakroom);
	int		SavePlayerFightSkillListDB(BYTE * pRoleBuffer, UINT room, char *bakRoleData, UINT bakroom);
	int		SavePlayerLifeSkilllListDB(BYTE * pRoleBuffer, UINT room, char *bakRoleData, UINT bakroom);
	int		SavePlayerFriendListDB(BYTE * pRoleBuffer, UINT room, char *bakRoleData, UINT bakroom);
	int		SavePlayerTaskListDB(BYTE * pRoleBuffer, UINT room, char *bakRoleData, UINT bakroom);
	int		UpdateDBSkillListDB(BYTE *pSkillBuffer, UINT room, BYTE *bakSkillBuffer, UINT bakroom,
						  int count);
public:
	CDBLoad();
	virtual ~CDBLoad(){};
	bool Open(char* aFilePath);	//打开包文件
	bool Close();								//关闭包文件
	// the two UINT& arguments come back holding what the file said the record is, and the
	// two UINTs going in say what the caller's buffers hold - a count from the file is only
	// ever compared against a room this function was told about.
	bool ReadData(char* key, UINT keyroom, UINT& keysize, char *aData, UINT dataroom, UINT& size);		//读取数据
	bool GotoHead();                      //false when the rewind did not happen: do not scan
	bool SearchData(char* key, char *aData, UINT& size);		//搜索数据
	void copyData(ZDBTable *dbTable);
	int		UpdateDBPlayerDB(BYTE * pPlayerInfo, UINT room, char *bakRoleData, UINT bakroom);
};


void AddOutputString_db (HWND hListCtrl, char* aStr);//添加操作输出文字
#endif // !defined(AFX_DBDUMPLOAD_H__956A6AC3_33CD_44D2_B4B2_771DE290B32F__INCLUDED_)

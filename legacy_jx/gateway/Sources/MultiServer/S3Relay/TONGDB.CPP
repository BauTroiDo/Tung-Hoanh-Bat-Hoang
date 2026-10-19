// TongDB.cpp: implementation of the CTongDB class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Global.h"
//#include "db.h"
#include "../../../../header/db.h"
#include "DBTable.h"
#include "TongDB.h"
#include <errno.h>

CTongDB		g_cTongDB;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
int get_TongName(DB *db, const DBT *pkey, const DBT *pdata, DBT *ikey) 
{
	//给定一个完整的buffer，得到account作为索引
	memset( ikey, 0, sizeof(DBT));
	//TRoleData *pRoleData = (TRoleData *)pdata->data;
	//ikey->data = pRoleData->BaseInfo.caccname;
	//ikey->size = strlen( pRoleData->BaseInfo.caccname ) + 1;
	//TMemberStruct* pMemberStruct = (TMemberStruct*)pdata->get_data(); 
	//ikey->set_data(pMemberStruct->szTong);
    //ikey->set_size(strlen( pMemberStruct->szTong ) + 1);
	
	if (pdata->size < sizeof(TMemberStruct))
		return EINVAL;
	TMemberStruct* pMemberStruct = (TMemberStruct*)pdata->data;
	ikey->data = pMemberStruct->szTong;
	ikey->size = strlen( pMemberStruct->szTong ) + 1;

	return 0;
}

CTongDB::CTongDB()
{//初始化变量
	m_TongListCount = 0;
	memset(m_TongList,0,sizeof(TTongList) * MAX_TONG_COUNT);
}

bool CTongDB::Open()
{
	m_TongTable = new ZDBTable("TongDB","TongTable");		    //打开帮会表
	if(!m_TongTable->open())
		return false;

	m_MemberTable = new ZDBTable("TongMemberDB","MemberTable");	//帮众表
	m_MemberTable->addIndex(get_TongName);

	if(!m_MemberTable->open())
	{
		m_TongTable->close();
		return false;
	}
	m_MoneyTable = new ZDBTable("TongMoneyDB","MoneyTable");	//帮众表
	if(!m_MoneyTable->open())
	{

	}
/*	m_WarCityTable = new ZDBTable("WarCityDB","WarCityTable");
	if(!m_WarCityTable->open())
	{
			
	} */
	return true;
}

bool CTongDB::OpenNew()
{
	//DeleteFile("TongDB");  // 删除文件夹
	//DeleteFile("TongMemberDB");

	m_TongTable = new ZDBTable("TongDb","TongTable");		//打开帮会表
	if(!m_TongTable->open())
		return false;
	
	m_MemberTable = new ZDBTable("TongMemberDB","MemberTable");	//帮众表
	m_MemberTable->addIndex(get_TongName);
	if(!m_MemberTable->open())
	{
		m_TongTable->close();
		return false;
	}
	m_MoneyTable = new ZDBTable("TongMoneyDB","MoneyTable");	//帮众表
	if(!m_MoneyTable->open())
	{
		return false;
	}

/*	m_WarCityTable = new ZDBTable("WarCityDB","WarCityTable");
	if(!m_WarCityTable->open())
	{
		
	}*/
	return true;
}

void CTongDB::Close()
{
	m_TongTable->close();
	m_MemberTable->close();
	m_MoneyTable->close();
 /*
	m_WarCityTable->close(); 
	delete m_WarCityTable;
	m_WarCityTable=NULL;
  */
	delete m_TongTable;
	m_TongTable=NULL;
	delete m_MemberTable;
	m_MemberTable=NULL;
	delete m_MoneyTable;
	m_MoneyTable=NULL;
}

bool CTongDB::ChangeTong(CTongControl& aTongControl)
{//创建和改变帮派
	bool aResult = true;
	int aKeySize = strlen(aTongControl.m_szName) + 1;
	int aSize = sizeof(TTongStruct);
	TTongStruct aTongStruct;   
	memset(&aTongStruct, 0, aSize);
	//aTongStruct.nTempCamp=aTongControl.m_nTempCamp;
	aTongStruct.nCamp = aTongControl.m_nCamp;
	aTongStruct.dwMoney = aTongControl.m_dwMoney;                                  //帮会资金
	aTongStruct.nCredit = aTongControl.m_nCredit;                                  //帮会的贡献度
	aTongStruct.nLevel = aTongControl.m_nLevel;
	aTongStruct.nTongMap =aTongControl.m_nTongMap;

	aTongStruct.nAttackNum=aTongControl.m_nAttackNum;	                    // 参战场数
    aTongStruct.nWinNum=aTongControl.m_nWinNum;						    // 胜利场数
	aTongStruct.nLoseNum=aTongControl.m_nLoseNum;							// 输掉场数

	snprintf(aTongStruct.szName, sizeof(aTongStruct.szName), "%s", aTongControl.m_szName);	                           //帮会名
	strcpy(aTongStruct.szMasterTitle, aTongControl.m_szMasterTitle);               //统一的帮主称号
	strcpy(aTongStruct.szNormalTitle, aTongControl.m_szNormalTitle);               //统一的帮众称号
	strcpy(aTongStruct.m_szAnnounce, aTongControl.m_szAnnounce);
	strcpy(aTongStruct.szManTitle, aTongControl.m_szManTitle);
	strcpy(aTongStruct.szWomanTitle, aTongControl.m_szWomanTitle);

	int i;
	for(i=0;i<defTONG_MAX_DIRECTOR;++i)
	{
		strcpy(aTongStruct.szDirectorTitle[i], aTongControl.m_szDirectorTitle[i]);  //统一的长老称号
	}
	for(i=0;i<defTONG_MAX_MANAGER;++i)
	{
		strcpy(aTongStruct.szManagerTitle[i], aTongControl.m_szManagerTitle[i]);     //统一的队长称号
	}
	aResult = m_TongTable->add(aTongControl.m_szName, aKeySize,(char*)(&aTongStruct), aSize); //增加到数据库
	
	return aResult;
}

bool CTongDB::DelTong(char* aTongName)
{//删除帮派
	bool aResult = true;
	int aKeySize = strlen(aTongName) + 1;

	aResult = m_TongTable->remove(aTongName, aKeySize);
	if(!aResult) return aResult;

	aResult = m_MoneyTable->remove(aTongName, aKeySize);
	if(!aResult) return aResult;

	while(!m_MemberTable->remove(aTongName, aKeySize, 0))
		{}
	return true;
}

bool CTongDB::SearchTong(char* aTongName, CTongControl& aTongControl)
{//从数据库中取得帮派
	bool aResult = true;
	int aSize = sizeof(TTongStruct);
	int aKeySize = strlen(aTongName) + 1;
	char* aBuffer = m_TongTable->search(aTongName, aKeySize, aSize);
	if (!aBuffer) return false;	//找不到帮派
	if (aSize < (int)sizeof(TTongStruct))
	{
		delete[] aBuffer;
		aBuffer = NULL;
		return false;
	}

	TTongStruct* pTongStruct = (TTongStruct*)aBuffer;
	//aTongControl.m_nTempCamp=pTongStruct->nTempCamp;
	aTongControl.m_nCamp = pTongStruct->nCamp;
	aTongControl.m_nLevel = pTongStruct->nLevel;
	aTongControl.m_nTongMap = pTongStruct->nTongMap;
	aTongControl.m_nCredit = pTongStruct->nCredit;

	aTongControl.m_nAttackNum = pTongStruct->nAttackNum;	                    // 参战场数
	aTongControl.m_nWinNum = pTongStruct->nWinNum;						    // 胜利场数
	aTongControl.m_nLoseNum = pTongStruct->nLoseNum;							// 输掉场数

	snprintf(aTongControl.m_szName, sizeof(aTongControl.m_szName), "%s", pTongStruct->szName);
	strcpy(aTongControl.m_szPassword, pTongStruct->szPassword);
	strcpy(aTongControl.m_szMasterTitle, pTongStruct->szMasterTitle);
	strcpy(aTongControl.m_szNormalTitle, pTongStruct->szNormalTitle);
	strcpy(aTongControl.m_szManTitle, pTongStruct->szManTitle);
	strcpy(aTongControl.m_szWomanTitle, pTongStruct->szWomanTitle);
	strcpy(aTongControl.m_szAnnounce, pTongStruct->m_szAnnounce);
	memcpy(aTongControl.m_nDirectorPower, pTongStruct->m_nDirectorPower, sizeof(pTongStruct->m_nDirectorPower));

	int i;
	for (i = 0; i < defTONG_MAX_DIRECTOR; ++i)
	{
		strcpy(aTongControl.m_szDirectorTitle[i], pTongStruct->szDirectorTitle[i]);
	}
	for (i = 0; i < defTONG_MAX_MANAGER; ++i)
	{
		strcpy(aTongControl.m_szManagerTitle[i], pTongStruct->szManagerTitle[i]);
	}

	int aDirectorNum = 0;
	int aManagerNum = 0;
	int aMemberNum = 0;
	int aLenmengNum = 0;//联盟成员

	aBuffer = m_MemberTable->search(aTongName, aKeySize, aSize, 0);
	while (aBuffer)
	{
		TMemberStruct* pMemberStruct = (TMemberStruct*)aBuffer;
		if (aSize < (int)sizeof(TMemberStruct))
		{
			aBuffer = m_MemberTable->next(aSize);
			continue;
		}
		switch (pMemberStruct->MemberClass)
		{
		case enumTONG_FIGURE_MASTER:
			snprintf(aTongControl.m_szMasterName, sizeof(aTongControl.m_szMasterName), "%s", pMemberStruct->szName);
			aTongControl.m_dwMasterID = g_String2IdA(pMemberStruct->szName);
			aTongControl.m_nMasterPoint = pMemberStruct->nPoint;
			aTongControl.m_nMasterLevel = pMemberStruct->nLevel;
			break;

		case enumTONG_FIGURE_DIRECTOR:
			if (pMemberStruct->nTitleIndex < 0 || pMemberStruct->nTitleIndex >= defTONG_MAX_DIRECTOR)
				break;
			snprintf(aTongControl.m_szDirectorName[pMemberStruct->nTitleIndex], sizeof(aTongControl.m_szDirectorName[pMemberStruct->nTitleIndex]), "%s", pMemberStruct->szName);
			aTongControl.m_dwDirectorID[pMemberStruct->nTitleIndex] = g_String2IdA(pMemberStruct->szName);
			aTongControl.m_nDirectorPoint[pMemberStruct->nTitleIndex] = pMemberStruct->nPoint;
			aTongControl.m_nDirectorLevel[pMemberStruct->nTitleIndex] = pMemberStruct->nLevel;
			aDirectorNum++;
			break;

		case enumTONG_FIGURE_MANAGER:
			if (pMemberStruct->nTitleIndex < 0 || pMemberStruct->nTitleIndex >= defTONG_MAX_MANAGER)
				break;
			snprintf(aTongControl.m_szManagerName[pMemberStruct->nTitleIndex], sizeof(aTongControl.m_szManagerName[pMemberStruct->nTitleIndex]), "%s", pMemberStruct->szName);
			aTongControl.m_dwManagerID[pMemberStruct->nTitleIndex] = g_String2IdA(pMemberStruct->szName);
			aTongControl.m_nManagerPoint[pMemberStruct->nTitleIndex] = pMemberStruct->nPoint;
			aTongControl.m_nManagerLevel[pMemberStruct->nTitleIndex] = pMemberStruct->nLevel;
			aManagerNum++;
			break;

		case enumTONG_FIGURE_MEMBER:
			if (pMemberStruct->nTitleIndex < 0 || pMemberStruct->nTitleIndex >= aTongControl.m_nMemberPointSize)
				break;
			snprintf(aTongControl.m_psMember[pMemberStruct->nTitleIndex].m_szName, sizeof(aTongControl.m_psMember[pMemberStruct->nTitleIndex].m_szName), "%s", pMemberStruct->szName);
			aTongControl.m_psMember[pMemberStruct->nTitleIndex].m_dwNameID = g_String2IdA(pMemberStruct->szName);
			aTongControl.m_psMember[pMemberStruct->nTitleIndex].m_nPoint = pMemberStruct->nPoint;
			aTongControl.m_psMember[pMemberStruct->nTitleIndex].m_nLevel = pMemberStruct->nLevel;
			aMemberNum++;
			break;
		}
		aBuffer = m_MemberTable->next(aSize);
	}

	aTongControl.m_nDirectorNum = aDirectorNum;
	aTongControl.m_nManagerNum = aManagerNum;
	aTongControl.m_nMemberNum = aMemberNum;

	aSize = sizeof(TMoneyStruct);
	aBuffer = m_MoneyTable->search(aTongName, aKeySize, aSize);
	int dwMoney = 0;
	if (aBuffer && aSize >= (int)sizeof(TMoneyStruct))
	{
		TMoneyStruct* pMoneyStruct = (TMoneyStruct*)aBuffer;
		dwMoney = pMoneyStruct->m_dwMoney;
	}

	aTongControl.m_dwMoney = dwMoney;

	delete[] aBuffer;
	aBuffer = NULL;
	return aResult;
}

//帮派的数量
int CTongDB::GetTongCount()
{//取得门派数
	int aCount = 0;
	/*int aTongSize;
	int aTongKeySize;
	char aTongKeyBuffer[defTONG_STR_LENGTH];
	char aTongBuffer[sizeof(TTongStruct)];
    

	bool aResult = m_TongTable->GetRecordEx(aTongBuffer, aTongSize, aTongKeyBuffer, aTongKeySize,ZDBTable::cpFirst);

	while(aResult)
	{
		aCount++;
		aResult = m_TongTable->GetRecordEx(aTongBuffer, aTongSize, aTongKeyBuffer, aTongKeySize,ZDBTable::cpNext);
}*/
	
	return m_TongTable->GetRecordCount();
}

int CTongDB::GetTongList(TTongList* aTongList, int aMaxNum)
{//取得帮会列表
	int aCount = 0;
	int aTongSize;
	int aTongKeySize;
	char aTongKeyBuffer[defTONG_STR_LENGTH];
	char aTongBuffer[sizeof(TTongStruct)];
	bool aResult = m_TongTable->GetRecordEx(aTongBuffer, aTongSize, (int)sizeof(TTongStruct), aTongKeyBuffer, aTongKeySize, defTONG_STR_LENGTH,ZDBTable::cpFirst);
	while(aResult && aTongSize >= (int)sizeof(TTongStruct))
	{
		TTongStruct* pTong = (TTongStruct*)aTongBuffer;	   //帮会的整体数据
		snprintf(aTongList[aCount].szName, sizeof(aTongList[aCount].szName), "%s", pTong->szName);

		int aMemCount = 0;
		int aMemSize;
		char* aMemBuffer;
		aMemBuffer = m_MemberTable->search(aTongKeyBuffer, aTongKeySize, aMemSize, 0);
		while(aMemBuffer && aMemSize >= (int)sizeof(TMemberStruct))
		{
			TMemberStruct* pMember = (TMemberStruct*)aMemBuffer;  //帮会成员的数据
			if(aMemSize >= (int)sizeof(TMemberStruct) && pMember->MemberClass == enumTONG_FIGURE_MEMBER)
					aMemCount++;
			else if (aMemSize >= (int)sizeof(TMemberStruct) && pMember->MemberClass == enumTONG_FIGURE_MASTER)  //帮主名
				   snprintf(aTongList[aCount].m_szMasterName, sizeof(aTongList[aCount].m_szMasterName), "%s", pMember->szName);

			delete [] aMemBuffer;
			aMemBuffer=NULL;

			aMemBuffer = m_MemberTable->next(aMemSize);
		}

		aTongList[aCount].MemberCount = aMemCount;		          //帮会成员的数量

		aCount++;
		if( aCount>= aMaxNum )  //最大帮会数量
			break;
		aResult = m_TongTable->GetRecordEx(aTongBuffer, aTongSize, (int)sizeof(TTongStruct), aTongKeyBuffer, aTongKeySize, defTONG_STR_LENGTH,ZDBTable::cpNext);
	}

	return aCount;
}

bool CTongDB::ChangeMember(TMemberStruct aMember)
{//增加帮众 添加的到数据库
	return m_MemberTable->add(aMember.szName, strlen(aMember.szName) + 1, (char*)(&aMember), sizeof(TMemberStruct));
}


bool CTongDB::DelMember(char* aMemberName)
{//删除帮众
	int aKeySize = strlen(aMemberName) + 1;

	return m_MemberTable->remove(aMemberName, aKeySize);
}

bool CTongDB::ChangeMoney( TMoneyStruct aMoney )
{
	return m_MoneyTable->add(aMoney.szTong, strlen(aMoney.szTong) + 1, (char*)(&aMoney), sizeof(TMoneyStruct));
}

bool CTongDB::DelMoney( char* aTongName )
{
	int aKeySize = strlen(aTongName) + 1;
	
	return m_MoneyTable->remove(aTongName, aKeySize);	
}
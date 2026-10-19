#include "stdafx.h"
#include "resource.h"
#include "PlayerCreator.h"
#include "inifile.h"
#include "utils.h"
#include "tstring.h"
#include "Macro.h"
#include "../../../../header/GameDatadef.h"
#include <time.h>
#include "..\..\Engine\Src\KEngine.h"
#include <algorithm>
/*
using CIniFile;
using GetAppFullPath;
using _tstring;
using ToBool;
*/
extern CLogFile  bs_LogFile;
extern HWND hMianHwd;

CPlayerCreator::CPlayerCreator()
{
	for (int i = 0; i < MAX_PLAYERTYPE_VALUE; ++i)
	{
		m_pRoleData[i] = NULL;
	}
	
	Init();
}

CPlayerCreator::~CPlayerCreator()
{
	for (int i = 0; i < MAX_PLAYERTYPE_VALUE; ++i)
	{
		if (!m_pRoleData[i])
		{
			delete [] m_pRoleData[i];
			m_pRoleData[i] = NULL;
		}
	}

	stdMapID2RID::iterator it;	
	for (it = m_theMapID2RID.begin(); it != m_theMapID2RID.end(); ++it)
	{
		stdRevivalID &SL = (*it).second;	
		SL.clear();
	}
	
	m_theMapID2RID.erase( m_theMapID2RID.begin(), m_theMapID2RID.end() );
}

bool CPlayerCreator::Init()
{
	CIniFile theIniFile;

	_tstring sIniFilePathName;

	sIniFilePathName = GetAppFullPath(NULL);

	char	szFileName[MAX_PATH];

	for (int i = 0; i < MAX_PLAYERTYPE_VALUE; ++i)
	{
		sprintf(szFileName, PLAYERCREATOR_FILE, i);
		_tstring sIniFileName = sIniFilePathName + szFileName;

		if (!m_pRoleData[i])
			m_pRoleData[i] = new BYTE[MAX_NEWPLAYER_BUFFER];
		ZeroMemory(m_pRoleData[i], MAX_NEWPLAYER_BUFFER * sizeof(BYTE));
		GetRoleDataFromIni(m_pRoleData[i], sIniFileName.c_str());
	}
	
	_tstring sRevivalFileName = sIniFilePathName + REVIVALID_FILENAME;

	CIniFile	cFile;
	cFile.SetFile( sRevivalFileName.c_str() );  //重新赋值路劲给 m_szFileName[MAX_PATH]

	CIniFile::_VETSTR theIniVetstr;
	cFile.ReadSections( theIniVetstr );

	char szBuffer[64]={0};

	CIniFile::_VETSTR::iterator it;
	for (it = theIniVetstr.begin(); it != theIniVetstr.end(); ++it)
	{
		_tstring info = (*it);

		if (info.empty())
		{
			continue;
		}
		int nSection = atoi(info.c_str());  //char转int  地图id 为节 
		int nCount = cFile.ReadInteger(info.c_str(),"Count",0);

		if ( nCount > 0 )
		{
			stdRevivalID rid;
			
			for (int i=0; i<nCount;i++)
			{
				sprintf(szBuffer,"RevivalId%2.2d",i);  //两位数字
				
				int nID = cFile.ReadInteger(info.c_str(),szBuffer,0);
				
				rid.push_back(nID);
			}			
			m_theMapID2RID.insert( stdMapID2RID::value_type(nSection,rid)); // 加入内存 地图 出生点
		}
	}

	_tstring sutf8FileName = sIniFilePathName + PLAYERNAME_FILE;
	KTabFile codeFile;
	if (codeFile.Load((char*)sutf8FileName.c_str()))
	{
		int  nRows = codeFile.GetHeight() + 1;
		for (int i = 2; i < nRows; i++)
		{
			char temp[8];
			codeFile.GetString(i, "HEX", "", temp, sizeof(temp));
			m_UTF8Code.push_back(string(temp));
		}
		codeFile.Clear();
	}

	_tstring invalidNamePath = sIniFilePathName + PLAYERINVALIDNAME_FILE;
	KTabFile invNameFile;
	if (invNameFile.Load((char*)invalidNamePath.c_str()))
	{
		int  nRows = invNameFile.GetHeight() + 1;
		for (int i = 2; i < nRows; i++)
		{
			char temp[64];
			invNameFile.GetString(i, "NAME", "", temp, sizeof(temp));
			m_InvalidName.push_back(string(temp));
		}
		invNameFile.Clear();
	}

	return false;
}

unsigned CPlayerCreator::GetRevivalID( UINT nMapID, UINT nType /*= enumRandom*/ )
{
	stdMapID2RID::iterator it;
	
	if (m_theMapID2RID.end() != ( it = m_theMapID2RID.find(nMapID)))
	{
		stdRevivalID& sl = ( *it ).second;  //第二个参数？
		/*
		 * TODO : Don't get the server when it can't carry anyone
		 */
		if (!sl.empty())
		{
			int nCount = sl.size();
			ASSERT(nCount>0);
			int nIndex = rand() % nCount;  // 取随机值 出生点
			nIndex = ( nIndex >= 0 && nIndex < nCount ) ? nIndex : 0;
			return sl[nIndex];
		}
	}

	return 0;
}


const TRoleData* CPlayerCreator::GetRoleData(unsigned int &uDataLength, LPROLEPARAM lpParam,unsigned int nIsSerVer)
{
	int nIndex = lpParam->nSeries * ROLE_NO + lpParam->nSex;
	if (nIndex < 0 || nIndex >= MAX_PLAYERTYPE_VALUE)  //人物类型
	{
		uDataLength = 0;
		return NULL;
	}

	if (m_pRoleData[nIndex])
	{
		TRoleData *pData = ( TRoleData * )m_pRoleData[nIndex];

		ASSERT( pData );
        //以下是修改原来的基本数据
		strncpy(pData->BaseInfo.szName, lpParam->szName, sizeof(pData->BaseInfo.szName));
        pData->BaseInfo.szName[sizeof(pData->BaseInfo.szName) - 1] = '\0';

		strncpy(pData->BaseInfo.caccname, lpParam->szAccName,  sizeof(pData->BaseInfo.caccname));
        pData->BaseInfo.caccname[sizeof(pData->BaseInfo.caccname) - 1] = '\0';

		pData->BaseInfo.bSex = ( bool )( 0 != lpParam->nSex );
		pData->BaseInfo.ifiveprop = lpParam->nSeries;
        pData->BaseInfo.isWaiGua  = nIsSerVer; //赋值当前的区 数据 
		pData->BaseInfo.iRoleParm6 = 0;
		//开始获取时间 创建角色的时间
		time_t rawtime;
		struct tm * timeinfo;	
		time(&rawtime);
		/*timeinfo = localtime (&rawtime);  //获取本地时间
		timeinfo->tm_year+1900 =
		timeinfo->tm_mon + 1   = 
		timeinfo->tm_mday      = 
		timeinfo->tm_hour      =
		timeinfo->tm_min       =*/
		if ( BST_CHECKED == ::IsDlgButtonChecked( hMianHwd, IDC_CHECK_ROLESET ) )
		{//如果开启了统一设置
			UINT nValue=0;
			BOOL bTranslated=0;
			     nValue = ::GetDlgItemInt(hMianHwd, IDC_EDIT_ROLELEVEL, &bTranslated, FALSE );
            if  (nValue>0)
			{
				pData->BaseInfo.ifightlevel = nValue;
				pData->BaseInfo.ileftprop   = (nValue-1)*5;
				pData->BaseInfo.ileftfight  = nValue-1;
			}
			     nValue = ::GetDlgItemInt(hMianHwd, IDC_EDIT_ROLEMONEY, &bTranslated, FALSE );
			if  (nValue>0)
			     pData->BaseInfo.iservermon  = nValue;
			     nValue = ::GetDlgItemInt(hMianHwd, IDC_EDIT_ROLEXU, &bTranslated, FALSE );
			if  (nValue>0)
				 pData->BaseInfo.iclientx    = nValue;
		}

		pData->BaseInfo.iRoleParm8 = rawtime; //纪录秒数
		unsigned nNativeID = GetRevivalID(lpParam->nMapID);

		if (lpParam->nMapID<=0)
		{
			pData->BaseInfo.irevivalid=53;	                 //原重生点
		    pData->BaseInfo.irevivalx =19;
		    pData->BaseInfo.irevivaly =0;
		}
		else
		{
		    pData->BaseInfo.irevivalid = lpParam->nMapID;	 //原重生点
		    pData->BaseInfo.irevivalx  = nNativeID;
		    pData->BaseInfo.irevivaly  = 0;
		}
		uDataLength = pData->dwDataLen;
		return ( const TRoleData * )pData;
	}

	uDataLength = 0;
	return NULL;
}
//新建角色调用
bool CPlayerCreator::GetRoleDataFromIni(BYTE* pData, const char* szFileName)
{
	CIniFile	cFile;

	cFile.SetFile( szFileName );

	TRoleData*	pRoleData = (TRoleData *)pData;

	pRoleData->bBaseNeedUpdate = 1;
	pRoleData->dwVersion = 0;
	//玩家信息
	pRoleData->BaseInfo.ileftprop	= cFile.ReadInteger("ROLE", "ileftprop", 0);
	pRoleData->BaseInfo.ileftfight	= cFile.ReadInteger("ROLE", "ileftfight", 0);
	pRoleData->BaseInfo.ipower		= cFile.ReadInteger("ROLE", "ipower", 0);
	pRoleData->BaseInfo.iagility	= cFile.ReadInteger("ROLE", "iagility", 0);
	pRoleData->BaseInfo.iouter		= cFile.ReadInteger("ROLE", "iouter", 0);
	pRoleData->BaseInfo.iinside		= cFile.ReadInteger("ROLE", "iinside", 0);
	pRoleData->BaseInfo.iluck		= cFile.ReadInteger("ROLE", "iluck", 0);

	//玩家的显示信息	-- remark by spe because client ui display changed 2003/07/21
//	pRoleData->BaseInfo.ihelmres	= cFile.ReadInteger("ROLE", "ihelmres", 0);
//	pRoleData->BaseInfo.iarmorres	= cFile.ReadInteger("ROLE", "iarmorres", 0);
//	pRoleData->BaseInfo.iweaponres	= cFile.ReadInteger("ROLE", "iweaponres", 0);
	
	//玩家等级信息
	pRoleData->BaseInfo.fightexp	= cFile.ReadInteger("ROLE", "ifightexp", 0);
	pRoleData->BaseInfo.ifightlevel	= cFile.ReadInteger("ROLE", "ifightlevel", 1);
	pRoleData->BaseInfo.ileadlevel	= cFile.ReadInteger("ROLE", "ileadlevel", 0);
	pRoleData->BaseInfo.ileadexp	= cFile.ReadInteger("ROLE", "ileadexp", 0);

	//门派信息
	pRoleData->BaseInfo.nSect		= cFile.ReadInteger("ROLE", "isect", 0);
	pRoleData->BaseInfo.nFirstSect	= cFile.ReadInteger("ROLE", "ifirstsect", 0);
	pRoleData->BaseInfo.ijoincount	= cFile.ReadInteger("ROLE", "ijoincount", 0);

	//现金和贮物箱中的钱
	pRoleData->BaseInfo.iservermon		= cFile.ReadInteger("ROLE", "imoney", 0);
	pRoleData->BaseInfo.isavemoney	= cFile.ReadInteger("ROLE", "isavemoney", 0);
	//

	pRoleData->BaseInfo.ifiveprop	= cFile.ReadInteger("ROLE", "ifiveprop", 0);
	pRoleData->BaseInfo.iteam		= cFile.ReadInteger("ROLE", "iteam", 0);
	pRoleData->BaseInfo.bSex		= ( 0 != cFile.ReadInteger("ROLE", "bsex", 0) );
	pRoleData->BaseInfo.imaxlife	= cFile.ReadInteger("ROLE", "imaxlife", 0);
	pRoleData->BaseInfo.imaxstamina	= cFile.ReadInteger("ROLE", "imaxstamina", 0);
	pRoleData->BaseInfo.imaxinner	= cFile.ReadInteger("ROLE", "imaxinner", 0);
	pRoleData->BaseInfo.imaxnuqi	= cFile.ReadInteger("ROLE", "imaxnuqi", 0);
	pRoleData->BaseInfo.icurlife	= pRoleData->BaseInfo.imaxlife;
	pRoleData->BaseInfo.icurinner	= pRoleData->BaseInfo.imaxinner;
	pRoleData->BaseInfo.icurstamina	= pRoleData->BaseInfo.imaxstamina;
	//pRoleData->BaseInfo.icurnuqi	= 0; //怒气pRoleData->BaseInfo.imaxnuqi

	//登入点
	pRoleData->BaseInfo.irevivalid	= cFile.ReadInteger("ROLE", "irevivalid",0);
	pRoleData->BaseInfo.irevivalx	= cFile.ReadInteger("ROLE", "irevivalx",0);
	pRoleData->BaseInfo.irevivaly	= cFile.ReadInteger("ROLE", "irevivaly",0);
	
	pRoleData->BaseInfo.cUseRevive = 1; //默认从重生点进入游戏
	pRoleData->BaseInfo.cFightMode = 0;
	pRoleData->BaseInfo.cIsExchange = 0;
	pRoleData->BaseInfo.ientergameid = 0;
	pRoleData->BaseInfo.ientergamex = 0;
	pRoleData->BaseInfo.ientergamey = 0;

	//PK相关
	pRoleData->BaseInfo.cPkStatus = 0;
	pRoleData->BaseInfo.ipkvalue = 0;
	pRoleData->BaseInfo.ireputevalue = 0;
	pRoleData->BaseInfo.ifuyuanvalue = 0;
	pRoleData->BaseInfo.irebornvalue = 0;
	pRoleData->BaseInfo.ipassrole    = 0;   //锁频密码
	
	//帮会相关
	pRoleData->BaseInfo.dwTongID = 0;

	pRoleData->dwFSkillOffset = (BYTE * )pRoleData->pBuffer - pData;

//  Skill
	int nSkillCount = 0;
	char szSkillId[100]={0};
	char szSkillLevel[100]={0};
	nSkillCount						= cFile.ReadInteger(SECTION_FIGHTSKILLLIST, KEY_COUNT, 0);
	TDBSkillData *pSkillData		= (TDBSkillData *)(pData + pRoleData->dwFSkillOffset);
	for (int i = 0 ; i < nSkillCount; ++i, ++pSkillData)
	{
		int nSkillId = 0;
		int nSkillLevel = 0;
		sprintf(szSkillId, "%s%d", KEY_FIGHTSKILL, i+1);	
		sprintf(szSkillLevel, "%s%d", KEY_FIGHTSKILLLEVEL, i+1);
		pSkillData->m_nSkillId		= cFile.ReadInteger(SECTION_FIGHTSKILLLIST, szSkillId, 0);
		pSkillData->m_nSkillLevel	= cFile.ReadInteger(SECTION_FIGHTSKILLLIST, szSkillLevel, 0);
		pSkillData->m_nSkillExp	    = 0;
		pSkillData->m_nExpSkillPonit	 = 0;
	}
	pRoleData->nFightSkillCount		= nSkillCount;
	pRoleData->dwLSkillOffset		= (BYTE *)pSkillData - pData;

	nSkillCount						= cFile.ReadInteger(SECTION_LIFESKILLLIST, KEY_COUNT, 0);
	pSkillData						= (TDBSkillData *)(pData + pRoleData->dwLSkillOffset);
	INT32 i;
	for (i = 0 ; i < nSkillCount; ++i, ++pSkillData)
	{
		int nSkillId = 0;
		int nSkillLevel = 0;
		sprintf(szSkillId, "%s%d", KEY_FIGHTSKILL, i+1);	
		sprintf(szSkillLevel, "%s%d", KEY_FIGHTSKILLLEVEL, i+1);
		pSkillData->m_nSkillId		= cFile.ReadInteger(SECTION_FIGHTSKILLLIST, szSkillId, 0);
		pSkillData->m_nSkillLevel	= cFile.ReadInteger(SECTION_FIGHTSKILLLIST, szSkillLevel, 0);
		pSkillData->m_nSkillExp	    = 0;
		pSkillData->m_nExpSkillPonit	 = 0;
	}
	pRoleData->nLiveSkillCount		= nSkillCount;
	pRoleData->dwTaskOffset			= (BYTE*)pSkillData - pData;
	pRoleData->nTaskCount			= 0;
	pRoleData->dwItemOffset			= pRoleData->dwTaskOffset;
	
//Item
	TDBItemData * pItemData = (TDBItemData *)pSkillData;
	int nItemCount = 0;
	nItemCount						= cFile.ReadInteger(SECTION_ITEMLIST, KEY_COUNT, 0);
	
	char szSection[100]={0};
	
	for ( i = 0 ; i < nItemCount; ++i)
	{
		sprintf(szSection, "%s%d", SECTION_ITEM, i + 1 );
		//物件基本信息**************************************************************
		pItemData->iequipclasscode	= cFile.ReadInteger(szSection, "iequipclasscode", 0);
		pItemData->idetailtype		= cFile.ReadInteger(szSection, "idetailtype", 0);
		pItemData->iparticulartype	= cFile.ReadInteger(szSection, "iparticulartype", 0);
		pItemData->ilevel			= cFile.ReadInteger(szSection, "ilevel", 0);
		pItemData->iseries			= cFile.ReadInteger(szSection, "iseries", 0);
		pItemData->ix				= cFile.ReadInteger(szSection, "ix", 0);
		pItemData->iy				= cFile.ReadInteger(szSection, "iy", 0);
		pItemData->ilocal			= cFile.ReadInteger(szSection, "ilocal", 0);
		pItemData->iparam1			= cFile.ReadInteger(szSection, "iparam1", 0);
		pItemData->iparam2			= cFile.ReadInteger(szSection, "iparam2", 0);
		pItemData->iparam3			= cFile.ReadInteger(szSection, "iparam3", 0);
		pItemData->iparam4			= cFile.ReadInteger(szSection, "iparam4", 0);
		pItemData->iparam5			= cFile.ReadInteger(szSection, "iparam5", 0);
		pItemData->iparam6			= cFile.ReadInteger(szSection, "iparam6", 0);
		pItemData->iparamr1			= cFile.ReadInteger(szSection, "iparamr1", 0);//熔炼
		pItemData->iparamr2			= cFile.ReadInteger(szSection, "iparamr2", 0);
		pItemData->iparamr3			= cFile.ReadInteger(szSection, "iparamr3", 0);
		pItemData->iparamr4			= cFile.ReadInteger(szSection, "iparamr4", 0);
		pItemData->iparamr5			= cFile.ReadInteger(szSection, "iparamr5", 0);
		pItemData->iparamr6			= cFile.ReadInteger(szSection, "iparamr6", 0);
		pItemData->iequipversion	= cFile.ReadInteger(szSection, "iequipversion", 0);
		pItemData->irandseed		= cFile.ReadInteger(szSection, "irandseed", 0);
		pItemData->ilucky			= cFile.ReadInteger(szSection, "ilucky", 0);
		pItemData->idurability		= 0;
		pItemData->iidentify		= 0;

		++pItemData;
	}

	pRoleData->dwFriendOffset = (BYTE *)pItemData - pData;
	pRoleData->nItemCount = nItemCount;
	pRoleData->nFriendCount = 0;
	pRoleData->dwDataLen = (BYTE *)pItemData - pData;

	return true;
}

int CPlayerCreator::CountUTF8Character(unsigned char* bytes)
{
	int count = 0;
	for (int i = 0; bytes[i]; i++)
	{
		if ((bytes[i] & 0xC0) != 0x80)
		{
			count++;
		}
	}

	return count;
}

std::string CPlayerCreator::GetUTF8CharacterHex(unsigned char* bytes, int idx)
{
	char temp[4];
	sprintf(temp, "%.2x", bytes[idx]);
	std::string result(temp);
	for (int i = idx + 1; bytes[i]; i++)
	{
		if ((bytes[i] & 0xC0) != 0x80)
			break;
		else
		{
			sprintf(temp, "%.2x", bytes[i]);
			result = result + temp;
		}
	}

	return result;
}

bool CPlayerCreator::CheckNameValid(unsigned char* bytes)
{
	if (std::find(m_InvalidName.begin(), m_InvalidName.end(), (char*)bytes) != m_InvalidName.end())
		return false;

	for (int i = 0; bytes[i]; i++)
	{
		if ((bytes[i] & 0xC0) != 0x80)
		{
			string hex = GetUTF8CharacterHex(bytes, i);
			int dec = stoi(hex, 0, 16);
			if (!(dec >= 0x30 && dec <= 0x39) && !(dec >= 0x41 && dec <= 0x5a) && !(dec >= 0x61 && dec <= 0x7a)
				&& !(std::find(m_UTF8Code.begin(), m_UTF8Code.end(), hex) != m_UTF8Code.end()))
				return false;
		}
	}
	return true;
}


int CPlayerCreator::CountTCVN3Characters(const char* str) {
    return strlen(str);
}

bool CPlayerCreator::IsTCVN3NameValid(const char* str) {
    const char* invalidChars = "!@#$%^&*()=+[]{}|;:'\",.<>?/\\\t\n\r";
    while (*str) {
        unsigned char ch = *str;
        if (strchr(invalidChars, ch))
            return false;

        if (ch < 0x20 || ch == 0x7F) // cac ky tu dieu khien ASCII
            return false;

        str++;
    }
    return true;
}


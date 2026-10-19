// KTongControl.cpp: implementation of the CTongControl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Global.h"
#include "KTongControl.h"
#include "S3Relay.h"
#include "../../Engine/src/Text.h"

//#include "../../Engine/src/KIniFile.h"
//#include "../../Engine/src/KTabFile.h"

#define		defTONG_INIT_MEMBER_SIZE		100		// 成员内存初始化时的大小
#define		defTONG_MEMBER_SIZE_ADD			100		// 成员内存每次增加的大小

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//用于新创建帮会时的初始化帮会信息用到和 上线时加载的帮会信息
CTongControl::CTongControl(int nCamp, char* lpszPlayerName, char* lpszTongName)
{
	if (nCamp != camp_justice && nCamp != camp_evil && nCamp != camp_balance)
		return;
	if (!lpszPlayerName || !lpszPlayerName[0] || strlen(lpszPlayerName) >= NAME_LENGHT)
	{
		//sprintf(m_szMasterName, "%s", "帮主名有错");
		return;
	}
	if (!lpszTongName || !lpszTongName[0] || strlen(lpszTongName) >= NAME_LENGHT)
	{
		//sprintf(m_szMasterName, "%s", "帮派名有错");
		return;
	}

	m_nTempCamp = 0;
	//m_nCamp = 0;
	m_dwMoney = 0;
	m_nCredit = 0;
	m_nLevel = 1;
	m_nTongMap = 0;
	m_nAttackNum = 0;	                        // 参战场数
	m_nWinNum = 0;								// 胜利场数
	m_nLoseNum = 0;								// 输掉场数

	m_nDirectorNum = 0;
	m_nManagerNum = 0;
	m_nMemberNum = 0;
	m_nLenMengNum = 0;  //联盟
	//m_dwNameID = 0;
	//m_szName[0] = 0;
	m_szPassword[0] = 0;

	//m_nTempCamp = 0;                            // 帮会的临时阵营 （用于宣战）(红帮 绿帮)
	memset(m_szAttackName, 0, sizeof(m_szAttackName));
	sprintf(m_szAttackName, "AttackName");
	//m_szAttackName[defTONG_STR_LENGTH];		// 宣战帮会名
	m_AttackState = 0;                          // 宣战状态 0无宣战 1发起宣战放 2被宣战方
	m_AttackTime = 0;                           // 宣战的时长
	m_nDeathCount = 0;

	//m_szMasterTitle[0] = 0;
	strcpy(m_szMasterTitle, g_TongSetting.m_szMasterTitle);
	//rTRACE("**DEBUG, mastertitle: %s", g_TongSetting.m_szMasterTitle);
	//rTRACE("**DEBUG, mastertitle: %s", g_TongSetting.m_szManagerTitle);
	//rTRACE("**DEBUG, mastertitle: %s", g_TongSetting.m_szDirectorTitle);
	//rTRACE("**DEBUG, mastertitle: %s", g_TongSetting.m_szMemberTitle);

	memset(m_szDirectorTitle, 0, sizeof(m_szDirectorTitle));
	memset(m_szManagerTitle, 0, sizeof(m_szManagerTitle));

	for (int i = 0; i < defTONG_MAX_DIRECTOR; ++i)
	{
		snprintf(m_szDirectorTitle[i], sizeof(m_szDirectorTitle[i]), "%s", g_TongSetting.m_szDirectorTitle);
	}

	for (int i = 0; i < defTONG_MAX_MANAGER; ++i)
	{
		snprintf(m_szManagerTitle[i], sizeof(m_szManagerTitle[i]), "%s", g_TongSetting.m_szManagerTitle);
	}

	//m_szNormalTitle[0] = 0;
	snprintf(m_szNormalTitle, sizeof(m_szNormalTitle), "%s", g_TongSetting.m_szMemberTitle);

	sprintf(m_szManTitle, "");
	sprintf(m_szWomanTitle, "");
	sprintf(m_szAnnounce, "");

	memset(m_nDirectorPower, 0, sizeof(m_nDirectorPower));

	//m_dwMasterID = 0;

	//m_szMasterName[0] = 0;
	//memset(m_szMasterName, 0, sizeof(m_szMasterName));
	snprintf(m_szMasterName, sizeof(m_szMasterName), "%s", lpszPlayerName);
	memset(m_dwDirectorID, 0, sizeof(m_dwDirectorID));
	memset(m_szDirectorName, 0, sizeof(m_szDirectorName)); //长老

	memset(m_dwManagerID, 0, sizeof(m_dwManagerID));
	memset(m_szManagerName, 0, sizeof(m_szManagerName));  //队长

	m_psMember = (STONG_MEMBER*)new STONG_MEMBER[defTONG_INIT_MEMBER_SIZE];
	m_nMemberPointSize = defTONG_INIT_MEMBER_SIZE;
	memset(m_psMember, 0, sizeof(STONG_MEMBER) * m_nMemberPointSize);

	m_nCamp = nCamp;
	strcpy(m_szMasterName, lpszPlayerName);
	m_dwMasterID = g_String2IdA(lpszPlayerName); //修改过的原来是m_szMasterName
	snprintf(m_szName, sizeof(m_szName), "%s", lpszTongName);
	m_dwNameID = g_String2IdA(lpszTongName);   //修改过的原来是m_szName
}

CTongControl::CTongControl(TTongList sList)
{
	m_nCamp			= 0;
	m_dwMoney		= 0;
	m_nCredit		= 0;
	m_nLevel		= 1;
	m_nDirectorNum	= 0;
	m_nManagerNum	= 0;
	m_nTongMap      = 0;
	m_nTempCamp     = 0;  // 帮会的临时阵营 （用于宣战）(红帮 绿帮)
	m_nAttackNum    = 0;	                            // 参战场数
    m_nWinNum       = 0;								// 胜利场数
	m_nLoseNum      = 0;								// 输掉场数
	memset(m_szAttackName, 0, sizeof(m_szAttackName));
	//m_szAttackName[defTONG_STR_LENGTH];		// 宣战帮会名
	sprintf(m_szAttackName,"AttackName");
	m_AttackState   =0;                          // 宣战状态 0无宣战 1发起宣战放 2被宣战方
	m_AttackTime    =0;                           // 宣战的时长
	m_nDeathCount   =0;

	snprintf(m_szName, sizeof(m_szName), "%s", sList.szName);

	m_dwNameID		= g_String2IdA(m_szName);
	m_szPassword[0]	= 0;

	m_szMasterTitle[0] = 0;
    snprintf(m_szMasterTitle, sizeof(m_szMasterTitle), "%s", g_TongSetting.m_szMasterTitle);

	memset(m_szDirectorTitle, 0, sizeof(m_szDirectorTitle));
	memset(m_szManagerTitle, 0, sizeof(m_szManagerTitle));

	for (int i = 0; i < defTONG_MAX_DIRECTOR; ++i)
	{
		snprintf(m_szDirectorTitle[i], sizeof(m_szDirectorTitle[i]), "%s", g_TongSetting.m_szDirectorTitle);
	}

	for (int i = 0; i < defTONG_MAX_MANAGER; ++i)
	{
		snprintf(m_szManagerTitle[i], sizeof(m_szManagerTitle[i]), "%s", g_TongSetting.m_szManagerTitle);
	}
	
    m_szNormalTitle[0] = 0;
	snprintf(m_szNormalTitle, sizeof(m_szNormalTitle), "%s", g_TongSetting.m_szMemberTitle);

	m_dwMasterID = 0;
	snprintf(m_szMasterName, sizeof(m_szMasterName), "%s", sList.m_szMasterName);

	memset(m_dwDirectorID, 0, sizeof(m_dwDirectorID));
	memset(m_szDirectorName, 0, sizeof(m_szDirectorName));

	memset(m_dwManagerID, 0, sizeof(m_dwManagerID));
	memset(m_szManagerName, 0, sizeof(m_szManagerName));

	m_nMemberNum = sList.MemberCount;
	m_nMemberPointSize = m_nMemberNum + defTONG_INIT_MEMBER_SIZE;
	m_psMember = (STONG_MEMBER*)new STONG_MEMBER[m_nMemberPointSize];
	memset(m_psMember, 0, sizeof(STONG_MEMBER) * m_nMemberPointSize);
}

CTongControl::~CTongControl()
{
	if (m_psMember)
	{
		delete []m_psMember;
		m_psMember=NULL;
	}
}

UINT	CTongControl::AddMember(char* lpszPlayerName, int nLevel, int nPoint)
{
	// 这种情况不应该发生
	if (m_nMemberPointSize <= 0 || !m_psMember)
		return -1;

	if (!lpszPlayerName || strlen(lpszPlayerName) >= NAME_LENGHT)
		return -1;

	// 寻找空位
	int i;
	for (i = 0; i < m_nMemberPointSize; ++i)
	{
		if (m_psMember[i].m_dwNameID == 0)
			break;
	}

	// 没找着，满了，开辟一块更大的内存 - mo rong vung nho
	if (i >= m_nMemberPointSize)
	{
		i = m_nMemberPointSize;
		// 暂时备份一下
		STONG_MEMBER* pTemp;
		pTemp = (STONG_MEMBER*)new STONG_MEMBER[m_nMemberPointSize];
		memcpy(pTemp, m_psMember, sizeof(STONG_MEMBER) * m_nMemberPointSize);

		// 分配更大的内存
		delete[]m_psMember;
		m_psMember = NULL;
		m_psMember = (STONG_MEMBER*)new STONG_MEMBER[m_nMemberPointSize + defTONG_MEMBER_SIZE_ADD];
		memcpy(m_psMember, pTemp, sizeof(STONG_MEMBER) * m_nMemberPointSize);
		memset(&m_psMember[m_nMemberPointSize], 0, sizeof(STONG_MEMBER) * defTONG_MEMBER_SIZE_ADD);
		m_nMemberPointSize += defTONG_MEMBER_SIZE_ADD;

		delete[]pTemp;
		pTemp = NULL;
	}

	// 增加一个成员
	snprintf(m_psMember[i].m_szName, sizeof(m_psMember[i].m_szName), "%s", lpszPlayerName);
	m_psMember[i].m_dwNameID = g_String2IdA(lpszPlayerName);
	m_psMember[i].m_nLevel = nLevel;
	m_psMember[i].m_nPoint = nPoint;

	m_nMemberNum++;

	return i;
}

BOOL	CTongControl::ChangePassword(char *lpOld, char *lpNew)
{
	if (!lpOld || !lpNew)
		return FALSE;
	if (strlen(lpNew) >= NAME_LENGHT || strcmp(lpOld, m_szPassword) != 0)
		return FALSE;

	strcpy(m_szPassword, lpNew);

	return TRUE;
}

//更新宣战时间
BOOL CTongControl::UpAttackTime(char *lpszPlayerName)
{
	return TRUE;
}


BOOL CTongControl::GetTongAttackInfo(STONG_ATTACK_INFO_SYNC *pInfo)
{
	if (!pInfo)
		return FALSE;
	//int	i, j;
    //帮会数据赋值打包
	
	pInfo->m_AttackState=this->m_AttackState;
	pInfo->m_nTempCamp=this->m_nTempCamp;
	pInfo->m_AttackTime=this->m_AttackTime;
	pInfo->m_CurMapIdx=this->m_nTongMap;
	pInfo->m_nLevel=this->m_nLevel;
	pInfo->m_YDeathCount=this->m_nDeathCount;
	pInfo->m_nAttackNum=this->m_nAttackNum;
	pInfo->m_nWinNum=this->m_nWinNum;
    pInfo->m_nLoseNum=this->m_nLoseNum;
	strcpy(pInfo->m_szAttackName,this->m_szAttackName);  //对战的帮会名
	pInfo->m_wLength = sizeof(STONG_ATTACK_INFO_SYNC);
    return TRUE;
}

//同步客户端帮会主信息 --用于申请加入帮会用？
BOOL	CTongControl::GetTongHeadInfo(STONG_HEAD_INFO_SYNC *pInfo)
{
	if (!pInfo)
		return FALSE;

	int	i, j;
    //帮会数据赋值打包
	pInfo->ProtocolFamily = pf_tong;
	pInfo->ProtocolID     = enumS2C_TONG_HEAD_INFO;
	pInfo->m_dwMoney      = m_dwMoney;
	pInfo->m_nCredit      = m_nCredit;
	pInfo->m_btCamp       = m_nCamp;
	pInfo->m_btLevel      = m_nLevel;	    //帮会等级
	pInfo->m_btManagerNum = m_nManagerNum;  //队长数量
	pInfo->m_dwMemberNum  = m_nMemberNum;   //帮众数量

    snprintf(pInfo->m_szTongName, sizeof(pInfo->m_szTongName), "%s", m_szName);  //  帮会名
	pInfo->m_sMember[0].m_btFigure = enumTONG_FIGURE_MASTER;     //职位
	pInfo->m_sMember[0].m_btPos = 0;
	//strcpy(pInfo->m_sMember[0].m_szTitle, this->m_szMasterTitle);//帮主称号
	strcpy(pInfo->m_sMember[0].m_szTitle, g_TongSetting.m_szMasterTitle);
	snprintf(pInfo->m_sMember[0].m_szName, sizeof(pInfo->m_sMember[0].m_szName), "%s", this->m_szMasterName); //帮主
	strcpy(pInfo->m_szAnnounce, this->m_szAnnounce);

	pInfo->m_btDirectorNum = 0;               //长老数量

	for (i = 0, j = 1; i < defTONG_MAX_DIRECTOR; ++i)
	{
	  if (!m_szDirectorName[i][0])
			continue;
		pInfo->m_sMember[j].m_btFigure = enumTONG_FIGURE_DIRECTOR;
		pInfo->m_sMember[j].m_btPos = i;
		//strcpy(pInfo->m_sMember[j].m_szTitle, this->m_szDirectorTitle[i]);//长老称号
		strcpy(pInfo->m_sMember[j].m_szTitle, g_TongSetting.m_szDirectorTitle);
		snprintf(pInfo->m_sMember[j].m_szName, sizeof(pInfo->m_sMember[j].m_szName), "%s", this->m_szDirectorName[i]);  //长老名字
		++j;
		pInfo->m_btDirectorNum++;             //长老数量
	}

	pInfo->m_wLength = sizeof(STONG_HEAD_INFO_SYNC) - sizeof(pInfo->m_sMember) + sizeof(STONG_ONE_LEADER_INFO) * (1 + pInfo->m_btDirectorNum);

	return TRUE;
}

BOOL	CTongControl::GetTongManagerInfo(
			STONG_GET_MANAGER_INFO_COMMAND *pApply,
			STONG_MANAGER_INFO_SYNC *pInfo) // 队长信息
{
	if (!pApply || !pInfo)
		return FALSE;
	if (pApply->m_nParam3 <= 0)
		return FALSE;

	pInfo->ProtocolFamily	= pf_tong;
	pInfo->ProtocolID		= enumS2C_TONG_MANAGER_INFO;
	pInfo->m_dwParam		= pApply->m_dwParam;
	pInfo->m_dwMoney		= m_dwMoney;
	pInfo->m_nCredit		= m_nCredit;
	pInfo->m_btCamp			= m_nCamp;
	pInfo->m_btLevel		= m_nLevel;
	pInfo->m_btDirectorNum	= m_nDirectorNum;
	pInfo->m_btManagerNum	= m_nManagerNum;
	pInfo->m_dwMemberNum	= m_nMemberNum;
	pInfo->m_btStartNo		= pApply->m_nParam2;
	snprintf(pInfo->m_szTongName, sizeof(pInfo->m_szTongName), "%s", m_szName);

	int		nNeedNum, nStartNum, i, j;

	nStartNum = pApply->m_nParam2;
	nNeedNum = pApply->m_nParam3;
	if (nNeedNum > defTONG_ONE_PAGE_MAX_NUM)
		nNeedNum = defTONG_ONE_PAGE_MAX_NUM;

	pInfo->m_btCurNum = 0;
	// 找开始位置
	for (i = 0, j = 0; i < defTONG_MAX_MANAGER; ++i)
	{
		if (j >= nStartNum)
			break;
		if (m_szManagerName[i][0])
			++j;
	}
	//if (i >= defTONG_MAX_MANAGER || j < nStartNum)
		//return FALSE;

	// 开始记录数据
	for (; i < defTONG_MAX_MANAGER; ++i)
	{
		if (pInfo->m_btCurNum >= nNeedNum)
			break;
		if (!m_szManagerName[i][0])
			continue;

		pInfo->m_sMember[pInfo->m_btCurNum].m_btFigure = enumTONG_FIGURE_MANAGER;
		pInfo->m_sMember[pInfo->m_btCurNum].m_btPos = i;
		//strcpy(pInfo->m_sMember[pInfo->m_btCurNum].m_szTitle, m_szManagerTitle[i]);
		strcpy(pInfo->m_sMember[pInfo->m_btCurNum].m_szTitle, g_TongSetting.m_szManagerTitle);
		snprintf(pInfo->m_sMember[pInfo->m_btCurNum].m_szName, sizeof(pInfo->m_sMember[pInfo->m_btCurNum].m_szName), "%s", m_szManagerName[i]);

		pInfo->m_btCurNum++;
	}
	//if (pInfo->m_btCurNum == 0)
		//return FALSE;

	pInfo->m_wLength = sizeof(STONG_MANAGER_INFO_SYNC) - sizeof(STONG_ONE_LEADER_INFO) * (defTONG_ONE_PAGE_MAX_NUM - pInfo->m_btCurNum);

	return TRUE;
}


BOOL CTongControl::GetTongMemberInfo(STONG_GET_MEMBER_INFO_COMMAND* pApply, STONG_MEMBER_INFO_SYNC* pInfo)   //帮众信息
{//分段发送数据的
	if (!pApply || !pInfo)
		return FALSE;

	if (pApply->m_nParam3 <= 0)
		return FALSE;

	if (!m_psMember || m_nMemberPointSize <= 0)
		return FALSE;

	pInfo->ProtocolFamily = pf_tong;
	pInfo->ProtocolID = enumS2C_TONG_MEMBER_INFO;
	pInfo->m_dwParam = pApply->m_dwParam;
	pInfo->m_dwMoney = m_dwMoney;
	pInfo->m_nCredit = m_nCredit;
	pInfo->m_btCamp = m_nCamp;
	pInfo->m_btLevel = m_nLevel;
	pInfo->m_btDirectorNum = m_nDirectorNum;
	pInfo->m_btManagerNum = m_nManagerNum;
	pInfo->m_dwMemberNum = m_nMemberNum;
	pInfo->m_btStartNo = pApply->m_nParam2; //原来的开始的位置
	strcpy(pInfo->m_szTitle, m_szNormalTitle);
	snprintf(pInfo->m_szTongName, sizeof(pInfo->m_szTongName), "%s", m_szName);

	int i, j;

	int nStartNum = pApply->m_nParam2;   //开始的位置
	int nNeedNum = pApply->m_nParam3;   //请求的数量

	if (nNeedNum > defTONG_ONE_PAGE_MAX_NUM)     //最大请求数量
		nNeedNum = defTONG_ONE_PAGE_MAX_NUM;     //最大请求数量

	pInfo->m_btCurNum = 0;

	for (i = 0, j = 0; i < m_nMemberPointSize; ++i)
	{//过滤掉 请求位置前面的内容
		if (j >= nStartNum)
			break;
		if (m_psMember[i].m_szName[0])
			++j;
	}
	//if (i >= m_nMemberPointSize || j < nStartNum)  //如果超过了数量 或 数据比所请求的数量少的话 就 终止
		//return FALSE;

	// 开始记录所请求从开始位置后的数据
	for (; i < m_nMemberPointSize; ++i)
	{
		if (pInfo->m_btCurNum >= nNeedNum)
			break; //当前的数量 大于 请求的数量 则跳出
		if (!m_psMember[i].m_szName[0])
			continue; //名字为空的 继续下一个数据
		//记录从开始位置起 后的数据
		snprintf(pInfo->m_sMember[pInfo->m_btCurNum].m_szName, sizeof(pInfo->m_sMember[pInfo->m_btCurNum].m_szName), "%s", m_psMember[i].m_szName);

		pInfo->m_btCurNum++;
	}
	//if (pInfo->m_btCurNum == 0)
		//return FALSE;

	pInfo->m_wLength = sizeof(STONG_MEMBER_INFO_SYNC) - sizeof(STONG_ONE_MEMBER_INFO) * (defTONG_ONE_PAGE_MAX_NUM - pInfo->m_btCurNum);

	return TRUE;
}

#include <iterator>
#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>

void CTongControl::SortList(std::vector<std::pair<DWORD, UINT>> &mList)
{
	std::sort(mList.begin(), mList.end(), [](const std::pair<DWORD, UINT>& a, const std::pair<DWORD, UINT>& b) -> BOOL {
		return a.second > b.second;
	});
}

TONG_MEMBER_FIGURE CTongControl::GetMemberFigure(DWORD dwID, BYTE& nPos)
{
	if (dwID == m_dwMasterID)
	{
		nPos = 0;
		return enumTONG_FIGURE_MASTER;
	}
	for (int i = 0; i < defTONG_MAX_DIRECTOR; i++)
	{
		if (m_dwDirectorID[i] == dwID)
		{
			nPos = i;
			return enumTONG_FIGURE_DIRECTOR;
		}
	}

	for (int i = 0; i < defTONG_MAX_MANAGER; ++i)
	{
		if (m_dwManagerID[i] == dwID)
		{
			nPos = i;
			return enumTONG_FIGURE_MANAGER;
		}
	}

	for (int i = 0; i < m_nMemberPointSize; ++i)
	{
		if (m_psMember[i].m_dwNameID == dwID)
		{
			nPos = i;
			return enumTONG_FIGURE_MEMBER;
		}
	}

	return enumTONG_FIGURE_NUM;
}

BOOL CTongControl::GetTopPointControl(STONG_SEARCH_INFO_COMMAND* pSearch, STONG_SEARCH_INFO_SYNC* pSync)
{
	std::vector<std::pair<DWORD, UINT>> mList;
	mList.emplace_back(std::pair<DWORD, UINT>(m_dwMasterID, m_nMasterPoint));

	for (int i = 0; i < defTONG_MAX_DIRECTOR; ++i)
	{
		if(m_dwDirectorID[i])
			mList.emplace_back(std::pair<DWORD, UINT>(m_dwDirectorID[i], m_nDirectorPoint[i]));
	}

	for (int i = 0; i < defTONG_MAX_MANAGER; ++i)
	{
		if (m_dwManagerID[i])
			mList.emplace_back(std::pair<DWORD, UINT>(m_dwManagerID[i], m_nManagerPoint[i]));
	}

	for (int i = 0; i < m_nMemberPointSize; ++i)
	{
		if (m_psMember[i].m_dwNameID)
			mList.emplace_back(std::pair<DWORD, UINT>(m_psMember[i].m_dwNameID, m_psMember[i].m_nPoint));
	}

	SortList(mList);

	int i = 0, j = 0;
	pSync->m_nCurNum = 0;

	for (std::vector<std::pair<DWORD, UINT>>::iterator it = mList.begin(); it != mList.end(); ++it)
	{
		if (i >= pSearch->m_nStart && i < pSearch->m_nStart + pSearch->m_nNum)
		{
			pSync->m_sMember[j].m_nPoint = (*it).second;

			if (GetMemberFigure((*it).first, pSync->m_sMember[j].m_btPos) == enumTONG_FIGURE_MASTER)
			{
				pSync->m_sMember[j].m_btFigure = enumTONG_FIGURE_MASTER;
				snprintf(pSync->m_sMember[j].m_szName, sizeof(pSync->m_sMember[j].m_szName), "%s", m_szMasterName);
			}
			else if (GetMemberFigure((*it).first, pSync->m_sMember[j].m_btPos) == enumTONG_FIGURE_DIRECTOR)
			{
				pSync->m_sMember[j].m_btFigure = enumTONG_FIGURE_DIRECTOR;
				snprintf(pSync->m_sMember[j].m_szName, sizeof(pSync->m_sMember[j].m_szName), "%s", m_szDirectorName[pSync->m_sMember[j].m_btPos]);
			}
			else if (GetMemberFigure((*it).first, pSync->m_sMember[j].m_btPos) == enumTONG_FIGURE_MANAGER)
			{
				pSync->m_sMember[j].m_btFigure = enumTONG_FIGURE_MANAGER;
				snprintf(pSync->m_sMember[j].m_szName, sizeof(pSync->m_sMember[j].m_szName), "%s", m_szManagerName[pSync->m_sMember[j].m_btPos]);
			}
			else if (GetMemberFigure((*it).first, pSync->m_sMember[j].m_btPos) == enumTONG_FIGURE_MEMBER)
			{
				pSync->m_sMember[j].m_btFigure = enumTONG_FIGURE_MEMBER;
				snprintf(pSync->m_sMember[j].m_szName, sizeof(pSync->m_sMember[j].m_szName), "%s", m_psMember[pSync->m_sMember[j].m_btPos].m_szName);
			}
			pSync->m_nCurNum++;
			j++;
		}
		i++;
	}

	pSync->m_wLength = sizeof(STONG_SEARCH_INFO_SYNC) - sizeof(STONG_ONE_MEMBER_INFO) * (defTONG_ONE_PAGE_MAX_NUM - pSync->m_nCurNum);


	return TRUE;
}

BOOL CTongControl::GetTopLevelControl(STONG_SEARCH_INFO_COMMAND* pSearch, STONG_SEARCH_INFO_SYNC* pSync)
{
	std::vector<std::pair<DWORD, UINT>> mList;
	mList.emplace_back(std::pair<DWORD, UINT>(m_dwMasterID, m_nMasterLevel));

	for (int i = 0; i < defTONG_MAX_DIRECTOR; ++i)
	{
		if (m_dwDirectorID[i])
			mList.emplace_back(std::pair<DWORD, UINT>(m_dwDirectorID[i], m_nDirectorLevel[i]));
	}

	for (int i = 0; i < defTONG_MAX_MANAGER; ++i)
	{
		if (m_dwManagerID[i])
			mList.emplace_back(std::pair<DWORD, UINT>(m_dwManagerID[i], m_nManagerLevel[i]));
	}

	for (int i = 0; i < m_nMemberPointSize; ++i)
	{
		if (m_psMember[i].m_dwNameID)
			mList.emplace_back(std::pair<DWORD, UINT>(m_psMember[i].m_dwNameID, m_psMember[i].m_nLevel));
	}

	SortList(mList);

	int i = 0, j = 0;
	pSync->m_nCurNum = 0;

	for (std::vector<std::pair<DWORD, UINT>>::iterator it = mList.begin(); it != mList.end(); ++it)
	{
		if (i >= pSearch->m_nStart && i < pSearch->m_nStart + pSearch->m_nNum)
		{
			pSync->m_sMember[j].m_nPoint = (*it).second;

			if (GetMemberFigure((*it).first, pSync->m_sMember[j].m_btPos) == enumTONG_FIGURE_MASTER)
			{
				pSync->m_sMember[j].m_btFigure = enumTONG_FIGURE_MASTER;
				snprintf(pSync->m_sMember[j].m_szName, sizeof(pSync->m_sMember[j].m_szName), "%s", m_szMasterName);
			}
			else if (GetMemberFigure((*it).first, pSync->m_sMember[j].m_btPos) == enumTONG_FIGURE_DIRECTOR)
			{
				pSync->m_sMember[j].m_btFigure = enumTONG_FIGURE_DIRECTOR;
				snprintf(pSync->m_sMember[j].m_szName, sizeof(pSync->m_sMember[j].m_szName), "%s", m_szDirectorName[pSync->m_sMember[j].m_btPos]);
			}
			else if (GetMemberFigure((*it).first, pSync->m_sMember[j].m_btPos) == enumTONG_FIGURE_MANAGER)
			{
				pSync->m_sMember[j].m_btFigure = enumTONG_FIGURE_MANAGER;
				snprintf(pSync->m_sMember[j].m_szName, sizeof(pSync->m_sMember[j].m_szName), "%s", m_szManagerName[pSync->m_sMember[j].m_btPos]);
			}
			else if (GetMemberFigure((*it).first, pSync->m_sMember[j].m_btPos) == enumTONG_FIGURE_MEMBER)
			{
				pSync->m_sMember[j].m_btFigure = enumTONG_FIGURE_MEMBER;
				snprintf(pSync->m_sMember[j].m_szName, sizeof(pSync->m_sMember[j].m_szName), "%s", m_psMember[pSync->m_sMember[j].m_btPos].m_szName);
			}
			pSync->m_nCurNum++;
			j++;
		}
		i++;
	}

	pSync->m_wLength = sizeof(STONG_SEARCH_INFO_SYNC) - sizeof(STONG_ONE_MEMBER_INFO) * (defTONG_ONE_PAGE_MAX_NUM - pSync->m_nCurNum);

	return TRUE;
}

BOOL CTongControl::GetOnOffControl(STONG_SEARCH_INFO_COMMAND* pSearch, STONG_SEARCH_INFO_SYNC* pSync)
{
	std::vector<std::pair<DWORD, UINT>> mList;

	// check master online
	if (g_TongServer.FindPlayerByRole(NULL, std::_tstring(m_szMasterName), NULL, NULL, NULL, NULL))
	{
		mList.emplace_back(std::pair<DWORD, UINT>(m_dwMasterID, 1));
	}
	else
	{
		mList.emplace_back(std::pair<DWORD, UINT>(m_dwMasterID, 0));
	}

	// check director online
	for (int i = 0; i < defTONG_MAX_DIRECTOR; ++i)
	{
		if (m_dwDirectorID[i])
		{
			if (g_TongServer.FindPlayerByRole(NULL, std::_tstring(m_szDirectorName[i]), NULL, NULL, NULL, NULL))
			{
				mList.emplace_back(std::pair<DWORD, UINT>(m_dwDirectorID[i], 1));
			}
			else
			{
				mList.emplace_back(std::pair<DWORD, UINT>(m_dwDirectorID[i], 0));
			}
		}
	}

	// check manager online
	for (int i = 0; i < defTONG_MAX_MANAGER; ++i)
	{
		if (m_dwManagerID[i])
		{
			if (g_TongServer.FindPlayerByRole(NULL, std::_tstring(m_szManagerName[i]), NULL, NULL, NULL, NULL))
			{
				mList.emplace_back(std::pair<DWORD, UINT>(m_dwManagerID[i], 1));
			}
			else
			{
				mList.emplace_back(std::pair<DWORD, UINT>(m_dwManagerID[i], 0));
			}
		}
	}

	for (int i = 0; i < m_nMemberPointSize; ++i)
	{
		if (m_psMember[i].m_dwNameID)
		{
			if (g_TongServer.FindPlayerByRole(NULL, std::_tstring(m_psMember[i].m_szName), NULL, NULL, NULL, NULL))
			{
				mList.emplace_back(std::pair<DWORD, UINT>(m_psMember[i].m_dwNameID, 1));
			}
			else
			{
				mList.emplace_back(std::pair<DWORD, UINT>(m_psMember[i].m_dwNameID, 0));
			}
		}
	}

	SortList(mList);

	int i = 0, j = 0;
	pSync->m_nCurNum = 0;

	for (std::vector<std::pair<DWORD, UINT>>::iterator it = mList.begin(); it != mList.end(); ++it)
	{
		if (i >= pSearch->m_nStart && i < pSearch->m_nStart + pSearch->m_nNum)
		{
			pSync->m_sMember[j].m_nPoint = (*it).second;

			if (GetMemberFigure((*it).first, pSync->m_sMember[j].m_btPos) == enumTONG_FIGURE_MASTER)
			{
				pSync->m_sMember[j].m_btFigure = enumTONG_FIGURE_MASTER;
				snprintf(pSync->m_sMember[j].m_szName, sizeof(pSync->m_sMember[j].m_szName), "%s", m_szMasterName);
			}
			else if (GetMemberFigure((*it).first, pSync->m_sMember[j].m_btPos) == enumTONG_FIGURE_DIRECTOR)
			{
				pSync->m_sMember[j].m_btFigure = enumTONG_FIGURE_DIRECTOR;
				snprintf(pSync->m_sMember[j].m_szName, sizeof(pSync->m_sMember[j].m_szName), "%s", m_szDirectorName[pSync->m_sMember[j].m_btPos]);
			}
			else if (GetMemberFigure((*it).first, pSync->m_sMember[j].m_btPos) == enumTONG_FIGURE_MANAGER)
			{
				pSync->m_sMember[j].m_btFigure = enumTONG_FIGURE_MANAGER;
				snprintf(pSync->m_sMember[j].m_szName, sizeof(pSync->m_sMember[j].m_szName), "%s", m_szManagerName[pSync->m_sMember[j].m_btPos]);
			}
			else if (GetMemberFigure((*it).first, pSync->m_sMember[j].m_btPos) == enumTONG_FIGURE_MEMBER)
			{
				pSync->m_sMember[j].m_btFigure = enumTONG_FIGURE_MEMBER;
				snprintf(pSync->m_sMember[j].m_szName, sizeof(pSync->m_sMember[j].m_szName), "%s", m_psMember[pSync->m_sMember[j].m_btPos].m_szName);
			}
			pSync->m_nCurNum++;
			j++;
		}
		i++;
	}
	pSync->m_wLength = sizeof(STONG_SEARCH_INFO_SYNC) - sizeof(STONG_ONE_MEMBER_INFO) * (defTONG_ONE_PAGE_MAX_NUM - pSync->m_nCurNum);
	return TRUE;
}

//帮会任命
BOOL	CTongControl::Instate(STONG_INSTATE_COMMAND* pInstate, STONG_INSTATE_SYNC* pSync)
{
	if (!pInstate || !pSync)
		return FALSE;

	if (pInstate->m_btCurFigure == pInstate->m_btNewFigure && pInstate->m_btCurPos == pInstate->m_btNewPos)
		return FALSE;

	int		i, nOldPos, nNewPos;
	DWORD	dwNameID = 0;
	char	szName[NAME_LENGHT] = {0};

	snprintf(szName, sizeof(szName), "%s", pInstate->m_szName);
	dwNameID = g_String2IdA(szName);
	if (dwNameID == 0)
		return FALSE;

	pSync->ProtocolFamily = pf_tong;
	pSync->ProtocolID = enumS2C_TONG_INSTATE;
	pSync->m_btOldFigure = pInstate->m_btCurFigure;
	pSync->m_btOldPos = 0;
	pSync->m_btNewFigure = pInstate->m_btNewFigure;
	pSync->m_btNewPos = 0;
	pSync->m_dwParam = pInstate->m_dwParam;
	pSync->m_btSuccessFlag = 0;
	pSync->m_dwTongNameID = pInstate->m_dwTongNameID;
	pSync->m_szTitle[0] = 0;
	snprintf(pSync->m_szName, sizeof(pSync->m_szName), "%s", szName);

	TMemberStruct	sMember;
	ZeroMemory(&sMember, sizeof(sMember));
	sMember.MemberClass = (TONG_MEMBER_FIGURE)pInstate->m_btNewFigure;
	snprintf(sMember.szTong, sizeof(sMember.szTong), "%s", this->m_szName);
	snprintf(sMember.szName, sizeof(sMember.szName), "%s", szName);

	if (pInstate->m_btCurFigure == enumTONG_FIGURE_DIRECTOR)
	{
		for (i = 0; i < defTONG_MAX_DIRECTOR; ++i)
		{
			if (m_dwDirectorID[i] == dwNameID && strcmp(szName, m_szDirectorName[i]) == 0)
				break;
		}
		if (i >= defTONG_MAX_DIRECTOR)
			return FALSE;
		nOldPos = i;

		switch (pInstate->m_btNewFigure)
		{
		case enumTONG_FIGURE_MANAGER:
			for (i = 0; i < defTONG_MAX_MANAGER; ++i)
			{
				if (m_dwManagerID[i] == 0 && !m_szManagerName[i][0])
					break;
			}
			if (i >= defTONG_MAX_MANAGER)
				return FALSE;
			nNewPos = i;
			
			snprintf(m_szManagerName[nNewPos], sizeof(m_szManagerName[nNewPos]), "%s", szName);
			m_dwManagerID[nNewPos] = dwNameID;
			m_nManagerPoint[nNewPos] = m_nDirectorPoint[nOldPos];
			m_nManagerLevel[nNewPos] = m_nDirectorLevel[nOldPos];
			strcpy(m_szManagerTitle[nNewPos], g_TongSetting.m_szManagerTitle);
			
			sMember.nPoint = m_nManagerPoint[nNewPos];
			sMember.nLevel = m_nManagerLevel[nNewPos];

			ResetMember(nOldPos, enumTONG_FIGURE_DIRECTOR);
			//m_szDirectorName[nOldPos][0] = 0;
			//m_dwDirectorID[nOldPos] = 0;
			//m_nDirectorPoint[nOldPos] = 0;
			//m_nDirectorLevel[nOldPos] = 0;

			m_nDirectorNum--;
			m_nManagerNum++;
			strcpy(pSync->m_szTitle, m_szManagerTitle[nNewPos]);
			break;
		case enumTONG_FIGURE_MEMBER:
		{
			int idx = AddMember(szName, m_nDirectorLevel[nOldPos], m_nDirectorPoint[nOldPos]);
			if (idx < 0)
				return FALSE;

			nNewPos = idx;

			//m_psMember[nNewPos].m_nLevel = m_nDirectorLevel[nOldPos];
			//m_psMember[nNewPos].m_nPoint = m_nDirectorPoint[nOldPos];

			sMember.nPoint = m_psMember[nNewPos].m_nPoint;
			sMember.nLevel = m_psMember[nNewPos].m_nLevel;

			ResetMember(nOldPos, enumTONG_FIGURE_DIRECTOR);
			//m_szDirectorName[nOldPos][0] = 0;
			//m_dwDirectorID[nOldPos] = 0;
			//m_nDirectorPoint[nOldPos] = 0;
			//m_nDirectorLevel[nOldPos] = 0;
			
			m_nDirectorNum--;
			if(pInstate->m_nGender == 0 && m_szManTitle && m_szManTitle[0])
				strcpy(pSync->m_szTitle, m_szManTitle);			
			else if(pInstate->m_nGender == 1 && m_szWomanTitle && m_szWomanTitle[0])
				strcpy(pSync->m_szTitle, m_szWomanTitle);
			else
				strcpy(pSync->m_szTitle, m_szNormalTitle);
		}
			break;
		default:
			return FALSE;
		}
	}
	else if (pInstate->m_btCurFigure == enumTONG_FIGURE_MANAGER)
	{
		for (i = 0; i < defTONG_MAX_MANAGER; ++i)
		{
			if (m_dwManagerID[i] == dwNameID && strcmp(szName, m_szManagerName[i]) == 0)
				break;
		}
		if (i >= defTONG_MAX_MANAGER)
			return FALSE;

		nOldPos = i;

		switch (pInstate->m_btNewFigure)
		{
		case enumTONG_FIGURE_DIRECTOR:
			for (i = 0; i < defTONG_MAX_DIRECTOR; ++i)
			{
				if (m_dwDirectorID[i] == 0 && !m_szDirectorName[i][0])
					break;
			}
			if (i >= defTONG_MAX_DIRECTOR)
				return FALSE;

			nNewPos = i;

			snprintf(m_szDirectorName[nNewPos], sizeof(m_szDirectorName[nNewPos]), "%s", szName);
			m_dwDirectorID[nNewPos] = dwNameID;
			m_nDirectorPoint[nNewPos] = m_nManagerPoint[nOldPos];
			m_nDirectorLevel[nNewPos] = m_nManagerLevel[nOldPos];
			strcpy(m_szDirectorTitle[nNewPos], g_TongSetting.m_szDirectorTitle);
			ZeroMemory(&m_nDirectorPower[nNewPos], sizeof(m_nDirectorPower[nNewPos]));

			sMember.nPoint = m_nDirectorPoint[nNewPos];
			sMember.nLevel = m_nDirectorLevel[nNewPos];

			ResetMember(nOldPos, enumTONG_FIGURE_MANAGER);
			//m_szManagerName[nOldPos][0] = 0;
			//m_dwManagerID[nOldPos] = 0;
			//m_nManagerPoint[nOldPos] = 0;
			//m_nManagerLevel[nOldPos] = 0;
			
			m_nDirectorNum++;
			m_nManagerNum--;
			strcpy(pSync->m_szTitle, m_szDirectorTitle[nNewPos]);
			break;
		case enumTONG_FIGURE_MEMBER:
		{
			int idx = AddMember(szName, m_nManagerLevel[nOldPos], m_nManagerPoint[nOldPos]);
			if (idx < 0)
				return FALSE;

			nNewPos = idx;

			//m_psMember[nNewPos].m_nPoint = m_nManagerPoint[nOldPos];
			//m_psMember[nNewPos].m_nLevel = m_nManagerLevel[nOldPos];

			sMember.nPoint = m_psMember[nNewPos].m_nPoint;
			sMember.nLevel = m_psMember[nNewPos].m_nLevel;

			ResetMember(nOldPos, enumTONG_FIGURE_MANAGER);
			//m_szManagerName[nOldPos][0] = 0;
			//m_dwManagerID[nOldPos] = 0;
			//m_nManagerPoint[nOldPos] = 0;
			//m_nManagerLevel[nOldPos] = 0;

			m_nManagerNum--;
			if (pInstate->m_nGender == 0 && m_szManTitle && m_szManTitle[0])
				strcpy(pSync->m_szTitle, m_szManTitle);
			else if (pInstate->m_nGender == 1 && m_szWomanTitle && m_szWomanTitle[0])
				strcpy(pSync->m_szTitle, m_szWomanTitle);
			else
				strcpy(pSync->m_szTitle, m_szNormalTitle);
		}
			break;
		default:
			return FALSE;
		}
	}
	else if (pInstate->m_btCurFigure == enumTONG_FIGURE_MEMBER)
	{
		if (!m_psMember)
			return FALSE;

		for (i = 0; i < m_nMemberPointSize; ++i)
		{
			if (m_psMember[i].m_dwNameID == dwNameID && strcmp(szName, m_psMember[i].m_szName) == 0)
				break;
		}

		if (i >= m_nMemberPointSize)
			return FALSE;

		nOldPos = i;

		switch (pInstate->m_btNewFigure)
		{
		case enumTONG_FIGURE_DIRECTOR:
			for (i = 0; i < defTONG_MAX_DIRECTOR; ++i)
			{
				if (m_dwDirectorID[i] == 0 && !m_szDirectorName[i][0])
					break;
			}
			if (i >= defTONG_MAX_DIRECTOR)
				return FALSE;

			nNewPos = i;

			snprintf(m_szDirectorName[nNewPos], sizeof(m_szDirectorName[nNewPos]), "%s", szName);
			m_dwDirectorID[nNewPos] = dwNameID;
			m_nDirectorPoint[nNewPos] = this->m_psMember[nOldPos].m_nPoint;
			m_nDirectorLevel[nNewPos] = this->m_psMember[nOldPos].m_nLevel;
			strcpy(m_szDirectorTitle[nNewPos], g_TongSetting.m_szDirectorTitle);
			ZeroMemory(&m_nDirectorPower[nNewPos], sizeof(m_nDirectorPower[nNewPos]));

			sMember.nPoint = m_nDirectorPoint[nNewPos];
			sMember.nLevel = m_nDirectorLevel[nNewPos];

			ResetMember(nOldPos, enumTONG_FIGURE_MEMBER);
			//this->m_psMember[nOldPos].m_szName[0] = 0;
			//this->m_psMember[nOldPos].m_dwNameID = 0;
			//this->m_psMember[nOldPos].m_nLevel = 0;
			//this->m_psMember[nOldPos].m_nPoint = 0;

			m_nDirectorNum++;
			m_nMemberNum--;
			strcpy(pSync->m_szTitle, m_szDirectorTitle[nNewPos]);
			break;
		case enumTONG_FIGURE_MANAGER:  //帮众
			for (i = 0; i < defTONG_MAX_MANAGER; ++i)
			{
				if (m_dwManagerID[i] == 0 && !m_szManagerName[i][0])
					break;
			}
			if (i >= defTONG_MAX_MANAGER)
				return FALSE;
			nNewPos = i;

			snprintf(m_szManagerName[nNewPos], sizeof(m_szManagerName[nNewPos]), "%s", szName);
			m_dwManagerID[nNewPos] = dwNameID;
			m_nManagerPoint[nNewPos] = this->m_psMember[nOldPos].m_nPoint;
			m_nManagerLevel[nNewPos] = this->m_psMember[nOldPos].m_nLevel;
			strcpy(m_szManagerTitle[nNewPos], g_TongSetting.m_szDirectorTitle);

			sMember.nPoint = m_nManagerPoint[nNewPos];
			sMember.nLevel = m_nManagerLevel[nNewPos];

			ResetMember(nOldPos, enumTONG_FIGURE_MEMBER);
			//this->m_psMember[nOldPos].m_szName[0] = 0;
			//this->m_psMember[nOldPos].m_dwNameID = 0;
			//this->m_psMember[nOldPos].m_nLevel = 0;
			//this->m_psMember[nOldPos].m_nPoint = 0;

			m_nManagerNum++;
			m_nMemberNum--;
			strcpy(pSync->m_szTitle, m_szManagerTitle[nNewPos]);
			break;
		default:
			return FALSE;
		}
	}
	pSync->m_btSuccessFlag = 1;
	pSync->m_btOldPos = nOldPos;
	pSync->m_btNewPos = nNewPos;
	CNetConnectDup conndup;
	{ {
			//CNetConnectDup conndup;
			DWORD nameid = 0;
			unsigned long param = 0;
			if (g_TongServer.FindPlayerByRole(NULL, std::_tstring(szName), &conndup, NULL, &nameid, &param))
			{
				CNetConnectDup tongconndup = g_TongServer.FindTongConnectByIP(conndup.GetIP());
				if (tongconndup.IsValid())
				{
					STONG_BE_INSTATED_SYNC	sSync;

					sSync.ProtocolFamily = pf_tong;
					sSync.ProtocolID = enumS2C_TONG_BE_INSTATED;
					sSync.m_btFigure = pInstate->m_btNewFigure;
					sSync.m_btPos = nNewPos;
					sSync.m_dwParam = param;
					snprintf(sSync.m_szName, sizeof(sSync.m_szName), "%s", szName);
					strcpy(sSync.m_szTitle, pSync->m_szTitle);

					//rTRACE("DEBUG, Figure: %d\n", sSync.m_btFigure);
					//rTRACE("DEBUG, pos: %d\n", sSync.m_btPos);
					//rTRACE("DEBUG, dwparam: %d\n", sSync.m_dwParam);
					//rTRACE("DEBUG, name: %s\n", sSync.m_szName);
					//rTRACE("DEBUG, title: %s\n", sSync.m_szTitle);

					tongconndup.SendPackage((const void*)&sSync, sizeof(sSync));
				}
				else
				{
					//tong is not connect
				}

			}
			else
			{
				//not find
			}
		}}

	// 向帮会频道发消息，某人被任命
	char	szMsg[96];
	sprintf(szMsg, "\\O%d", m_dwNameID);

	DWORD channid = g_ChannelMgr.GetChannelID(szMsg, 0);
	if (channid != -1)
	{
		if (pSync->m_szTitle[0])
			snprintf(szMsg, sizeof(szMsg), "(%s)被任命为(%s)！", szName, pSync->m_szTitle);
		else
		{
			switch (pSync->m_btNewFigure)
			{
			case enumTONG_FIGURE_DIRECTOR:
				sprintf(szMsg, "(%s)被任命为长老！", szName);
				break;
			case enumTONG_FIGURE_MANAGER:
				sprintf(szMsg, "(%s)被任命为队长！", szName);
				break;
			case enumTONG_FIGURE_MEMBER:
				sprintf(szMsg, "(%s)被任命为普通帮众！", szName);
				break;
			}
		}
		g_ChannelMgr.SayOnChannel(channid, TRUE, std::string(), std::string("帮会"), std::string(szMsg), conndup.GetIP());
	}

	// 存盘，数据保存至数据库
	sMember.nTitleIndex = nNewPos;
	g_cTongDB.ChangeMember(sMember);

	return TRUE;
}

/*
BOOL	CTongControl::Instate(STONG_INSTATE_COMMAND *pInstate, STONG_INSTATE_SYNC *pSync)
{
	if (!pInstate || !pSync)
		return FALSE;
	if (pInstate->m_btCurFigure == pInstate->m_btNewFigure && pInstate->m_btCurPos == pInstate->m_btNewPos)
		return FALSE;

	char	szName[32];

	memcpy(szName, pInstate->m_szName, sizeof(pInstate->m_szName));
	szName[31] = 0;

	pSync->ProtocolFamily	= pf_tong;
	pSync->ProtocolID		= enumS2C_TONG_INSTATE;
	pSync->m_btOldFigure	= pInstate->m_btCurFigure;
	pSync->m_btOldPos		= pInstate->m_btCurPos;
	pSync->m_btNewFigure	= pInstate->m_btNewFigure;
	pSync->m_btNewPos		= pInstate->m_btNewPos;
	pSync->m_dwParam		= pInstate->m_dwParam;
	pSync->m_btSuccessFlag	= 0;
	pSync->m_dwTongNameID	= pInstate->m_dwTongNameID;
	pSync->m_szTitle[0]		= 0;
	strcpy(pSync->m_szName, szName);

	if (pInstate->m_btCurFigure == enumTONG_FIGURE_DIRECTOR)
	{
		if (pInstate->m_btCurPos >= defTONG_MAX_DIRECTOR)
			return FALSE;
		if (!m_szDirectorName[pInstate->m_btCurPos][0] ||
			strcmp(szName, m_szDirectorName[pInstate->m_btCurPos]) != 0)
			return FALSE;
		switch (pInstate->m_btNewFigure)
		{
		case enumTONG_FIGURE_DIRECTOR:
			if (pInstate->m_btNewPos >= defTONG_MAX_DIRECTOR)
				return FALSE;
			if (m_szDirectorName[pInstate->m_btNewPos][0])
				return FALSE;
			strcpy(m_szDirectorName[pInstate->m_btNewPos], szName);
			m_dwDirectorID[pInstate->m_btNewPos] = g_String2Id(szName);
			m_szDirectorName[pInstate->m_btCurPos][0] = 0;
			m_dwDirectorID[pInstate->m_btCurPos] = 0;
			strcpy(pSync->m_szTitle, m_szDirectorTitle[pInstate->m_btNewPos]);
			break;
		case enumTONG_FIGURE_MANAGER:
			if (pInstate->m_btNewPos >= defTONG_MAX_MANAGER)
				return FALSE;
			if (m_szManagerName[pInstate->m_btNewPos][0])
				return FALSE;
			strcpy(m_szManagerName[pInstate->m_btNewPos], szName);
			m_dwManagerID[pInstate->m_btNewPos] = g_String2Id(szName);
			m_szDirectorName[pInstate->m_btCurPos][0] = 0;
			m_dwDirectorID[pInstate->m_btCurPos] = 0;
			m_nDirectorNum--;
			m_nManagerNum++;
			strcpy(pSync->m_szTitle, m_szManagerTitle[pInstate->m_btNewPos]);
			break;
		case enumTONG_FIGURE_MEMBER:
			if (!AddMember(szName))
				return FALSE;
			m_szDirectorName[pInstate->m_btCurPos][0] = 0;
			m_dwDirectorID[pInstate->m_btCurPos] = 0;
			m_nDirectorNum--;
			strcpy(pSync->m_szTitle, m_szNormalTitle);
			break;
		default:
			return FALSE;
		}
	}
	else if (pInstate->m_btCurFigure == enumTONG_FIGURE_MANAGER)
	{
		if (pInstate->m_btCurPos >= defTONG_MAX_MANAGER)
			return FALSE;
		if (!m_szManagerName[pInstate->m_btCurPos][0] ||
			strcmp(szName, m_szManagerName[pInstate->m_btCurPos]) != 0)
			return FALSE;
		switch (pInstate->m_btNewFigure)
		{
		case enumTONG_FIGURE_DIRECTOR:
			if (pInstate->m_btNewPos >= defTONG_MAX_DIRECTOR)
				return FALSE;
			if (m_szDirectorName[pInstate->m_btNewPos][0])
				return FALSE;
			strcpy(m_szDirectorName[pInstate->m_btNewPos], szName);
			m_dwDirectorID[pInstate->m_btNewPos] = g_String2Id(szName);
			m_szManagerName[pInstate->m_btCurPos][0] = 0;
			m_dwManagerID[pInstate->m_btCurPos] = 0;
			m_nDirectorNum++;
			m_nManagerNum--;
			strcpy(pSync->m_szTitle, m_szDirectorTitle[pInstate->m_btNewPos]);
			break;
		case enumTONG_FIGURE_MANAGER:
			if (pInstate->m_btNewPos >= defTONG_MAX_MANAGER)
				return FALSE;
			if (m_szManagerName[pInstate->m_btNewPos][0])
				return FALSE;
			strcpy(m_szManagerName[pInstate->m_btNewPos], szName);
			m_dwManagerID[pInstate->m_btNewPos] = g_String2Id(szName);
			m_szManagerName[pInstate->m_btCurPos][0] = 0;
			m_dwManagerID[pInstate->m_btCurPos] = 0;
			strcpy(pSync->m_szTitle, m_szManagerTitle[pInstate->m_btNewPos]);
			break;
		case enumTONG_FIGURE_MEMBER:
			if (!AddMember(szName))
				return FALSE;
			m_szManagerName[pInstate->m_btCurPos][0] = 0;
			m_dwManagerID[pInstate->m_btCurPos] = 0;
			m_nManagerNum--;
			strcpy(pSync->m_szTitle, m_szNormalTitle);
			break;
		default:
			return FALSE;
		}
	}
	else if (pInstate->m_btCurFigure == enumTONG_FIGURE_MEMBER)
	{
		if (!m_psMember)
			return FALSE;
		int		nPos = -1;
		DWORD	dwNameID = g_String2Id(szName);
		if (dwNameID == 0)
			return FALSE;
		for (int i = 0; i < m_nMemberPointSize; ++i)
		{
			if (m_psMember[i].m_dwNameID != dwNameID)
				continue;
			nPos = i;
			break;
		}
		if (nPos < 0)
			return FALSE;

		switch (pInstate->m_btNewFigure)
		{
		case enumTONG_FIGURE_DIRECTOR:
			if (pInstate->m_btNewPos >= defTONG_MAX_DIRECTOR)
				return FALSE;
			if (m_szDirectorName[pInstate->m_btNewPos][0])
				return FALSE;
			strcpy(m_szDirectorName[pInstate->m_btNewPos], szName);
			m_dwDirectorID[pInstate->m_btNewPos] = g_String2Id(szName);
			this->m_psMember[nPos].m_szName[0] = 0;
			this->m_psMember[nPos].m_dwNameID = 0;
			m_nDirectorNum++;
			m_nMemberNum--;
			strcpy(pSync->m_szTitle, m_szDirectorTitle[pInstate->m_btNewPos]);
			break;
		case enumTONG_FIGURE_MANAGER:
			if (pInstate->m_btNewPos >= defTONG_MAX_MANAGER)
				return FALSE;
			if (m_szManagerName[pInstate->m_btNewPos][0])
				return FALSE;
			strcpy(m_szManagerName[pInstate->m_btNewPos], szName);
			m_dwManagerID[pInstate->m_btNewPos] = g_String2Id(szName);
			this->m_psMember[nPos].m_szName[0] = 0;
			this->m_psMember[nPos].m_dwNameID = 0;
			m_nManagerNum++;
			m_nMemberNum--;
			strcpy(pSync->m_szTitle, m_szManagerTitle[pInstate->m_btNewPos]);
			break;
		default:
			return FALSE;
		}
	}
	pSync->m_btSuccessFlag = 1;

	{{
		CNetConnectDup conndup;
		DWORD nameid = 0;
		unsigned long param = 0;
		if (g_HostServer.FindPlayerByRole(NULL, std::_tstring(szName), &conndup, NULL, &nameid, &param))
		{
			CNetConnectDup tongconndup = g_TongServer.FindTongConnectByIP(conndup.GetIP());
			if (tongconndup.IsValid())
			{
				STONG_BE_INSTATED_SYNC	sSync;

				sSync.ProtocolFamily = pf_tong;
				sSync.ProtocolID = enumS2C_TONG_BE_INSTATED;
				sSync.m_btFigure = pInstate->m_btNewFigure;
				sSync.m_btPos = pInstate->m_btNewPos;
				sSync.m_dwParam = param;
				strcpy(sSync.m_szName, szName);
				switch (pInstate->m_btNewFigure)
				{
				case enumTONG_FIGURE_DIRECTOR:
					strcpy(sSync.m_szTitle, m_szDirectorTitle[pInstate->m_btNewPos]);
					break;
				case enumTONG_FIGURE_MANAGER:
					strcpy(sSync.m_szTitle, m_szManagerTitle[pInstate->m_btNewPos]);
					break;
				case enumTONG_FIGURE_MEMBER:
					strcpy(sSync.m_szTitle, this->m_szNormalTitle);
					break;
				}

				tongconndup.SendPackage((const void *)&sSync, sizeof(sSync));
			}
			else
			{
				//tong is not connect
			}

		}
		else
		{
			//not find
		}
	}}

	// 向帮会频道发消息，某人被任命
	char	szMsg[96];
	sprintf(szMsg, "\\O%u", m_dwNameID);

	DWORD channid = g_ChannelMgr.GetChannelID(szMsg, 0);
	if (channid != -1)
	{
		if (pSync->m_szTitle[0])
			sprintf(szMsg, "%s被任命为%s！", szName, pSync->m_szTitle);
		else
		{
			switch (pSync->m_btNewFigure)
			{
			case enumTONG_FIGURE_DIRECTOR:
				sprintf(szMsg, "%s被任命为长老！", szName);
				break;
			case enumTONG_FIGURE_MANAGER:
				sprintf(szMsg, "%s被任命为队长！", szName);
				break;
			case enumTONG_FIGURE_MEMBER:
				sprintf(szMsg, "%s被任命为普通帮众！", szName);
				break;
			}
		}
		g_ChannelMgr.SayOnChannel(channid, TRUE, std::string(), std::string("公告"), std::string(szMsg));
	}


	// 存盘，数据保存至数据库
	TMemberStruct	sMember;
	sMember.MemberClass = (TONG_MEMBER_FIGURE)pInstate->m_btNewFigure;
	sMember.nTitleIndex = pInstate->m_btNewPos;
	strcpy(sMember.szTong, this->m_szName);
	strcpy(sMember.szName, szName);
	g_cTongDB.ChangeMember(sMember);

	return TRUE;
}
*/

BOOL	CTongControl::Kick(STONG_KICK_COMMAND *pKick, STONG_KICK_SYNC *pSync)
{
	if (!pKick || !pSync)
		return FALSE;

	char	szName[NAME_LENGHT];
	DWORD	dwNameID;
	int		nKickPos;
	// the two names are the same NAME_LENGHT array in the same header today; the
	// assert is what keeps a struct change from turning this copy into an overflow
	static_assert(sizeof(pKick->m_szName) <= sizeof(szName),
		"tong kick name does not fit szName");
	memcpy(szName, pKick->m_szName, sizeof(pKick->m_szName));
	szName[NAME_LENGHT - 1] = 0;
	dwNameID = g_String2IdA(szName);
	if (dwNameID == 0)
		return FALSE;

	pSync->ProtocolFamily	= pf_tong;
	pSync->ProtocolID		= enumS2C_TONG_KICK;
	pSync->m_btFigure		= pKick->m_btFigure;
	pSync->m_btPos			= pKick->m_btPos;
	pSync->m_dwParam		= pKick->m_dwParam;
	pSync->m_dwTongNameID	= pKick->m_dwTongNameID;
	pSync->m_btSuccessFlag	= 0;
	snprintf(pSync->m_szName, sizeof(pSync->m_szName), "%s", szName);

	if (pKick->m_btFigure == enumTONG_FIGURE_MANAGER)
	{
		if (pKick->m_btPos >= defTONG_MAX_MANAGER)
			return FALSE;
		INT32 i;
		for (i = 0; i < defTONG_MAX_MANAGER; ++i)
		{
			if (m_dwManagerID[i] == dwNameID && strcmp(m_szManagerName[i], szName) == 0)
				break;
		}
		if (i >= defTONG_MAX_MANAGER)
			return FALSE;
		nKickPos = i;
		this->m_szManagerName[i][0] = 0;
		this->m_dwManagerID[i] = 0;
		this->m_nManagerNum--;
	}
	else if (pKick->m_btFigure == enumTONG_FIGURE_MEMBER)
	{
		if (!m_psMember)
			return FALSE;

		int		nPos = -1;
		DWORD	dwNameID = g_String2IdA(szName);
		if (dwNameID == 0)
			return FALSE;

		for (int i = 0; i < m_nMemberPointSize; ++i)
		{
			if (m_psMember[i].m_dwNameID != dwNameID)
				continue;
			nPos = i;
			break;
		}
		if (nPos < 0)
			return FALSE;

		if (!m_psMember[nPos].m_szName[0] ||
			strcmp(m_psMember[nPos].m_szName, szName) != 0)
			return FALSE;
		m_psMember[nPos].m_szName[0] = 0;
		m_psMember[nPos].m_dwNameID = 0;
		m_nMemberNum--;
		nKickPos = 0;
	}
	else
	{
		return FALSE;
	}
	pSync->m_btSuccessFlag = 1;
	pSync->m_btPos = nKickPos;

	CNetConnectDup conndup;
	{{
		//CNetConnectDup conndup;
		DWORD nameid = 0;
		unsigned long param = 0;
		if (g_TongServer.FindPlayerByRole(NULL, std::_tstring(szName), &conndup, NULL, &nameid, &param))
		{
			CNetConnectDup tongconndup = g_TongServer.FindTongConnectByIP(conndup.GetIP());
			if (tongconndup.IsValid())
			{
				STONG_BE_KICKED_SYNC	sSync;

				sSync.ProtocolFamily = pf_tong;
				sSync.ProtocolID = enumS2C_TONG_BE_KICKED;
				sSync.m_btFigure = pKick->m_btFigure;
				sSync.m_btPos = nKickPos;
				sSync.m_dwParam = param;
				snprintf(sSync.m_szName, sizeof(sSync.m_szName), "%s", szName);

				tongconndup.SendPackage((const void *)&sSync, sizeof(sSync));
			}
			else
			{
				//tong is not connect
			}

		}
		else
		{
			//not find
		}
	}}

	// 存盘
	g_cTongDB.DelMember(szName);

	// 向帮会频道发消息，某人被踢出帮会
	char	szMsg[96];
	sprintf(szMsg, "\\O%d", m_dwNameID);

	DWORD channid = g_ChannelMgr.GetChannelID(szMsg, 0);
	if (channid != -1)
	{
		sprintf(szMsg, "(%s)被踢出帮会！", szName);
		g_ChannelMgr.SayOnChannel(channid, TRUE, std::string(), std::string("帮会"), std::string(szMsg),conndup.GetIP());
	}

	return TRUE;
}

BOOL	CTongControl::Leave(STONG_LEAVE_COMMAND *pLeave, STONG_LEAVE_SYNC *pSync)
{
	if (!pLeave || !pSync)
		return FALSE;

	char	szName[NAME_LENGHT];
	static_assert(sizeof(pLeave->m_szName) <= sizeof(szName),
		"tong leave name does not fit szName");
	memcpy(szName, pLeave->m_szName, sizeof(pLeave->m_szName));
	szName[NAME_LENGHT - 1] = 0;

	pSync->ProtocolFamily	= pf_tong;
	pSync->ProtocolID		= enumS2C_TONG_LEAVE;
	pSync->m_dwParam		= pLeave->m_dwParam;
	pSync->m_btSuccessFlag	= 0;
	snprintf(pSync->m_szName, sizeof(pSync->m_szName), "%s", szName);

	switch (pLeave->m_btFigure)
	{
	case enumTONG_FIGURE_MEMBER:
		{
			if (!m_psMember)
				break;
			int		nPos = -1;
			DWORD	dwNameID = g_String2IdA(szName);
			if (dwNameID == 0)
				return FALSE;
			for (int i = 0; i < m_nMemberPointSize; ++i)
			{
				if (m_psMember[i].m_dwNameID != dwNameID)
					continue;
				nPos = i;
				break;
			}
			if (nPos < 0)
				return FALSE;
			if (!m_psMember[nPos].m_szName[0] ||
				strcmp(m_psMember[nPos].m_szName, szName) != 0)
				return FALSE;

			m_psMember[nPos].m_szName[0] = 0;
			m_psMember[nPos].m_dwNameID = 0;
			m_nMemberNum--;
		}
		break;
	case enumTONG_FIGURE_MANAGER:
		{
			if (pLeave->m_btPos >= 0 && pLeave->m_btPos < defTONG_MAX_MANAGER)
			{
				if (strcmp(m_szManagerName[pLeave->m_btPos], szName) == 0)
				{
					m_szManagerName[pLeave->m_btPos][0] = 0;
					m_dwManagerID[pLeave->m_btPos] = 0;
					m_nManagerNum--;
					break;
				}
			}

			int		nPos = -1;
			DWORD	dwNameID = g_String2IdA(szName);
			if (dwNameID == 0)
				return FALSE;
			for (int i = 0; i < defTONG_MAX_MANAGER; ++i)
			{
				if (m_dwManagerID[i] != dwNameID)
					continue;
				nPos = i;
				break;
			}
			if (nPos < 0)
				return FALSE;
			if (!m_szManagerName[nPos][0] ||
				strcmp(szName, m_szManagerName[nPos]) != 0)
				return FALSE;
			m_szManagerName[nPos][0] = 0;
			m_dwManagerID[nPos] = 0;
			m_nManagerNum--;
		}
		break;
	case enumTONG_FIGURE_DIRECTOR:
		return FALSE;
	case enumTONG_FIGURE_MASTER:
		return FALSE;
	}
	pSync->m_btSuccessFlag = 1;

	// 存盘
	g_cTongDB.DelMember(szName);

	// 通过帮会频道发布消息：某某人叛帮
	char	szMsg[96];
	sprintf(szMsg, "\\O%d", m_dwNameID);

	DWORD channid = g_ChannelMgr.GetChannelID(szMsg, 0);
	if (channid != -1)
	{
		sprintf(szMsg, "(%s)叛出本帮！", szName);
		CNetConnectDup conndup;
		DWORD nameid = 0;
		unsigned long param = 0;
		if (g_TongServer.FindPlayerByRole(NULL, std::_tstring(szName), &conndup, NULL, &nameid, &param))
		{
			g_ChannelMgr.SayOnChannel(channid, TRUE, std::string(), std::string("公告"), std::string(szMsg),conndup.GetIP());
		}
	}

	return TRUE;
}

void CTongControl::ResetMember(int nPos, TONG_MEMBER_FIGURE nFigure)
{
	if (nFigure == enumTONG_FIGURE_DIRECTOR)
	{
		if (nPos < 0 || nPos >= defTONG_MAX_DIRECTOR)
			return;

		//m_szDirectorName[nPos][0] = 0;
		ZeroMemory(&m_szDirectorName[nPos], sizeof(m_szDirectorName[nPos]));
		m_dwDirectorID[nPos] = 0;
		m_nDirectorPoint[nPos] = 0;
		m_nDirectorLevel[nPos] = 0;
		ZeroMemory(&m_nDirectorPower[nPos], sizeof(m_nDirectorPower[nPos]));
	}
	else if (nFigure == enumTONG_FIGURE_MANAGER)
	{
		if (nPos < 0 || nPos >= defTONG_MAX_MANAGER)
			return;
		//m_szManagerName[nPos][0] = 0;
		ZeroMemory(&m_szManagerName[nPos], sizeof(m_szManagerName[nPos]));
		m_dwManagerID[nPos] = 0;
		m_nManagerPoint[nPos] = 0;
		m_nManagerLevel[nPos] = 0;
	}
	else if (nFigure == enumTONG_FIGURE_MEMBER)
	{
		if (nPos < 0 || nPos >= m_nMemberPointSize)
			return;
		//m_psMember[nPos].m_szName[0] = 0;
		ZeroMemory(&m_psMember[nPos], sizeof(m_psMember[nPos]));
		m_psMember[nPos].m_dwNameID = 0;
		m_psMember[nPos].m_nLevel = 0;
		m_psMember[nPos].m_nPoint = 0;
	}
}

BOOL	CTongControl::AcceptMaster(STONG_ACCEPT_MASTER_COMMAND *pAccept)
{
	if (!pAccept)
		return FALSE;

	if (!pAccept->m_szName || !pAccept->m_szName[0])
		return FALSE;

	char	szName[NAME_LENGHT];
	snprintf(szName, sizeof(szName), "%s", pAccept->m_szName);

	char szOldMaster[NAME_LENGHT];
	strcpy(szOldMaster, this->m_szMasterName);

	// 能力不够
	if (pAccept->m_btAcceptFalg == 0)
	{
		CNetConnectDup conndup;
		DWORD nameid = 0;
		unsigned long param = 0;
		if (g_TongServer.FindPlayerByRole(NULL, std::_tstring(m_szMasterName), &conndup, NULL, &nameid, &param))
		{
			CNetConnectDup tongconndup = g_TongServer.FindTongConnectByIP(conndup.GetIP());
			if (tongconndup.IsValid())
			{
				STONG_CHANGE_MASTER_FAIL_SYNC	sFail;
				sFail.ProtocolFamily	= pf_tong;
				sFail.ProtocolID		= enumS2C_TONG_CHANGE_MASTER_FAIL;
				sFail.m_dwParam			= param;
				sFail.m_btFailID		= 1;
				sFail.m_dwTongNameID	= pAccept->m_dwTongNameID;
				memcpy(sFail.m_szName, szName, sizeof(szName));
				tongconndup.SendPackage((const void *)&sFail, sizeof(sFail));
			}
			else
			{
				//tong is not connect
			}
		}
		else
		{
			//not find
		}

		return FALSE;
	}

	// AddMember returns UINT and reports failure as -1, so this test could not fire and a failed
	// Add was used below as slot 0xFFFFFFFF.  808 and 886 of this file read the same call as an
	// int and test it; only this line lost the sign that makes the guard mean what it says.
	int	nOldMasterPos = AddMember(m_szMasterName, m_nMasterLevel, m_nMasterPoint);
	if (nOldMasterPos < 0)
		return FALSE;

	TMemberStruct	sMember1;
	ZeroMemory(&sMember1, sizeof(sMember1));
	sMember1.MemberClass = enumTONG_FIGURE_MASTER;
	sMember1.nTitleIndex = 0;
	snprintf(sMember1.szTong, sizeof(sMember1.szTong), "%s", this->m_szName);
	snprintf(sMember1.szName, sizeof(sMember1.szName), "%s", szName);

	TMemberStruct	sMember2;
	ZeroMemory(&sMember2, sizeof(sMember2));
	sMember2.MemberClass = enumTONG_FIGURE_MEMBER;
	snprintf(sMember2.szTong, sizeof(sMember2.szTong), "%s", this->m_szName);
	snprintf(sMember2.szName, sizeof(sMember2.szName), "%s", m_szMasterName);
	sMember2.nLevel = m_nMasterLevel;
	sMember2.nPoint = m_nMasterPoint;
	sMember2.nTitleIndex = nOldMasterPos;

	DWORD	dwNameID;
	int		i;

	dwNameID = g_String2IdA(szName);
	if (dwNameID == 0)
		return FALSE;
	if (pAccept->m_btFigure == enumTONG_FIGURE_DIRECTOR)
	{
		for (i = 0; i < defTONG_MAX_DIRECTOR; ++i)
		{
			if (m_dwDirectorID[i] == dwNameID && strcmp(szName, m_szDirectorName[i]) == 0)
				break;
		}
		if (i >= defTONG_MAX_DIRECTOR)
			return FALSE;

		snprintf(this->m_szMasterName, sizeof(this->m_szMasterName), "%s", szName);
		this->m_dwMasterID = dwNameID;
		sMember1.nLevel = m_nDirectorLevel[i];
		sMember1.nPoint = m_nDirectorPoint[i];

		ResetMember(i, enumTONG_FIGURE_DIRECTOR);
		//m_szDirectorName[i][0] = 0;
		//m_dwDirectorID[i] = 0;
		//m_nDirectorPoint[i] = 0;
		//m_nDirectorLevel[i] = 0;

		this->m_nDirectorNum--;
	}
	else if (pAccept->m_btFigure == enumTONG_FIGURE_MANAGER)
	{
		for (i = 0; i < defTONG_MAX_MANAGER; ++i)
		{
			if (m_dwManagerID[i] == dwNameID && strcmp(szName, m_szManagerName[i]) == 0)
				break;
		}
		if (i >= defTONG_MAX_MANAGER)
			return FALSE;

		sMember1.nLevel = m_nManagerLevel[i];
		sMember1.nPoint = m_nManagerPoint[i];

		snprintf(this->m_szMasterName, sizeof(this->m_szMasterName), "%s", szName);
		this->m_dwMasterID = dwNameID;

		ResetMember(i, enumTONG_FIGURE_MANAGER);
		//m_szManagerName[i][0] = 0;
		//m_dwManagerID[i] = 0;
		//m_nManagerPoint[i] = 0;
		//m_nManagerLevel[i] = 0;

		this->m_nManagerNum--;
	}
	else if (pAccept->m_btFigure == enumTONG_FIGURE_MEMBER)
	{
		if (!m_psMember || m_nMemberPointSize <= 0)
			return FALSE;
		for (i = 0; i < this->m_nMemberPointSize; ++i)
		{
			if (m_psMember[i].m_dwNameID == dwNameID &&
				strcmp(szName, m_psMember[i].m_szName) == 0)
				break;
		}
		if (i >= m_nMemberPointSize)
			return FALSE;

		sMember1.nLevel = m_psMember[i].m_nLevel;
		sMember1.nPoint = m_psMember[i].m_nPoint;

		//if (!AddMember(m_szMasterName))
		//	return FALSE;
		snprintf(m_szMasterName, sizeof(m_szMasterName), "%s", szName); //帮主
		m_dwMasterID = dwNameID;

		ResetMember(i, enumTONG_FIGURE_MEMBER);
		//m_psMember[i].m_szName[0] = 0;
		//m_psMember[i].m_dwNameID = 0;
		//m_psMember[i].m_nLevel = 0;
		//m_psMember[i].m_nPoint = 0;

		m_nMemberNum--;
	}
	else
	{
		return FALSE;
	}

	//m_psMember[nOldMasterPos].m_nLevel = sMember2.nLevel;
	//m_psMember[nOldMasterPos].m_nPoint = sMember2.nPoint;

	m_nMasterPoint = sMember1.nPoint;
	m_nMasterLevel = sMember1.nLevel;
	strcpy(m_szMasterTitle, g_TongSetting.m_szMasterTitle);

	STONG_CHANGE_AS_SYNC	sChange;
	sChange.ProtocolFamily	= pf_tong;
	sChange.ProtocolID		= enumS2C_TONG_CHANGE_AS;
	sChange.m_dwTongNameID	= this->m_dwNameID;

	// 给老帮主发消息
	DWORD nameid = 0;
	unsigned long param = 0;
	CNetConnectDup conndup1;
	if (g_TongServer.FindPlayerByRole(NULL, std::_tstring(szOldMaster), &conndup1, NULL, &nameid, &param))
	{
		CNetConnectDup tongconndup = g_TongServer.FindTongConnectByIP(conndup1.GetIP());
		if (tongconndup.IsValid())
		{
			sChange.m_dwParam		= param;
			sChange.m_btFigure		= enumTONG_FIGURE_MEMBER;
			sChange.m_btPos			= 0;
			strcpy(sChange.m_szTitle, m_szNormalTitle);
			snprintf(sChange.m_szName, sizeof(sChange.m_szName), "%s", m_szMasterName);

			tongconndup.SendPackage((const void *)&sChange, sizeof(sChange));
		}
	}

	// 给新帮主发消息
	CNetConnectDup conndup2;
	if (g_TongServer.FindPlayerByRole(NULL, std::_tstring(szName), &conndup2, NULL, &nameid, &param))
	{
		CNetConnectDup tongconndup = g_TongServer.FindTongConnectByIP(conndup2.GetIP());
		if (tongconndup.IsValid())
		{
			sChange.m_dwParam		= param;
			sChange.m_btFigure		= enumTONG_FIGURE_MASTER;
			sChange.m_btPos			= 0;
			strcpy(sChange.m_szTitle, this->m_szMasterTitle);
			snprintf(sChange.m_szName, sizeof(sChange.m_szName), "%s", this->m_szMasterName);

			tongconndup.SendPackage((const void *)&sChange, sizeof(sChange));
		}
	}

	STONG_CHANGE_MASTER_SYNC	sMaster;
	sMaster.ProtocolFamily	= pf_tong;
	sMaster.ProtocolID		= enumS2C_TONG_CHANGE_AS;
	sMaster.m_dwTongNameID	= m_dwNameID;
	snprintf(sMaster.m_szName, sizeof(sMaster.m_szName), "%s", m_szMasterName);
	g_TongServer.BroadPackage((const void*)&sMaster, sizeof(sMaster));

	// 存盘
	// 新帮主
	//TMemberStruct	sMember1;
	//sMember1.MemberClass = enumTONG_FIGURE_MASTER;
	//sMember1.nTitleIndex = 0;
	//sMember1.nPoint		 = 
	//strcpy(sMember1.szTong, this->m_szName);
	//strcpy(sMember1.szName, szName);
	g_cTongDB.ChangeMember(sMember1);

	// 老帮主
	//TMemberStruct	sMember2;
	//sMember2.MemberClass = enumTONG_FIGURE_MEMBER;
	//sMember2.nTitleIndex = nOldMasterPos;
	//sMember2.nPoint = m_nMasterPoint;
	//sMember2.nLevel = m_nMasterLevel;
	//strcpy(sMember2.szTong, this->m_szName);
	//strcpy(sMember2.szName, szOldMaster);
	g_cTongDB.ChangeMember(sMember2);

	// 聊天频道发消息
	char	szMsg[96];
	sprintf(szMsg, "\\O%d", m_dwNameID);

	DWORD channid = g_ChannelMgr.GetChannelID(szMsg, 0);
	if (channid != -1)
	{
		snprintf(szMsg, sizeof(szMsg), "(%s)把帮主之位传给了(%s)！", szOldMaster, szName);
		g_ChannelMgr.SayOnChannel(channid, TRUE, std::string(), std::string("帮会"), std::string(szMsg),conndup2.GetIP());
	}

	return TRUE;
}

BOOL	CTongControl::GetLoginData(STONG_GET_LOGIN_DATA_COMMAND *pLogin, STONG_LOGIN_DATA_SYNC *pSync)
{
	if (!pLogin || !pSync)
		return FALSE;

	int		i;
	DWORD	dwNameID = g_String2IdA(pLogin->m_szName);
	if (dwNameID == 0)
		return FALSE;

	pSync->m_btCamp		= this->m_nCamp;
	snprintf(pSync->m_szTongName, sizeof(pSync->m_szTongName), "%s", this->m_szName);
	strcpy(pSync->m_szMaster, this->m_szMasterName);
	snprintf(pSync->m_szName, sizeof(pSync->m_szName), "%s", pLogin->m_szName);
	pSync->m_nMoney		= this->m_dwMoney;

	pSync->m_AttackState = this->m_AttackState;
	pSync->m_nTempCamp	 = this->m_nTempCamp;
	pSync->m_AttackTime = this->m_AttackTime;

	pSync->m_nDeathCount=this->m_nDeathCount;
	pSync->m_nLevel	    =this->m_nLevel;
	strcpy(pSync->m_szAttackName, this->m_szAttackName);

	if (this->m_dwMasterID == dwNameID)
	{
		pSync->m_btFigure	= enumTONG_FIGURE_MASTER;
		pSync->m_btPos		= 0;
		pSync->m_btFlag		= 1;
		strcpy(pSync->m_szTitle, this->m_szMasterTitle);
		return TRUE;
	}

	for (i = 0; i < defTONG_MAX_DIRECTOR; ++i)
	{
		if (this->m_dwDirectorID[i] == dwNameID)
		{
			pSync->m_btFigure	= enumTONG_FIGURE_DIRECTOR;
			pSync->m_btPos		= i;
			pSync->m_btFlag		= 1;
			strcpy(pSync->m_szTitle, this->m_szDirectorTitle[i]);
			return TRUE;
		}
	}

	for (i = 0; i < defTONG_MAX_MANAGER; ++i)
	{
		if (this->m_dwManagerID[i] == dwNameID)
		{
			pSync->m_btFigure	= enumTONG_FIGURE_MANAGER;
			pSync->m_btPos		= i;
			pSync->m_btFlag		= 1;
			strcpy(pSync->m_szTitle, this->m_szManagerTitle[i]);
			return TRUE;
		}
	}

	if (this->m_psMember)
	{
		for (i = 0; i < this->m_nMemberPointSize; ++i)
		{
			if (m_psMember[i].m_dwNameID == dwNameID)
			{
				pSync->m_btFigure	= enumTONG_FIGURE_MEMBER;
				pSync->m_btPos		= 0;
				pSync->m_btFlag		= 1;

				if(pLogin->m_nParam == 0 && m_szManTitle && m_szManTitle[0])
					strcpy(pSync->m_szTitle, this->m_szManTitle);
				else if(pLogin->m_nParam == 1 && m_szWomanTitle && m_szWomanTitle[0])
					strcpy(pSync->m_szTitle, this->m_szWomanTitle);
				else
					strcpy(pSync->m_szTitle, this->m_szNormalTitle);

				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL	CTongControl::SearchOne(char *lpszName, STONG_ONE_LEADER_INFO *pInfo)
{
	if (!lpszName || !lpszName[0] || !pInfo)
		return FALSE;

	int		i;
	DWORD	dwNameID = g_String2IdA(lpszName);
	if (dwNameID == 0)
		return FALSE;

	if (m_dwMasterID == dwNameID)
	{
		pInfo->m_btFigure	= enumTONG_FIGURE_MASTER;
		pInfo->m_btPos		= 0;
		strcpy(pInfo->m_szTitle, m_szMasterTitle);
		snprintf(pInfo->m_szName, sizeof(pInfo->m_szName), "%s", m_szMasterName);
		return TRUE;
	}

	for (i = 0; i < defTONG_MAX_DIRECTOR; ++i)
	{
		if (m_dwDirectorID[i] == dwNameID)
		{
			pInfo->m_btFigure	= enumTONG_FIGURE_DIRECTOR;
			pInfo->m_btPos		= i;
			strcpy(pInfo->m_szTitle, m_szDirectorTitle[i]);
			snprintf(pInfo->m_szName, sizeof(pInfo->m_szName), "%s", m_szDirectorName[i]);
			return TRUE;
		}
	}

	for (i = 0; i < defTONG_MAX_MANAGER; ++i)
	{
		if (m_dwManagerID[i] == dwNameID)
		{
			pInfo->m_btFigure	= enumTONG_FIGURE_MANAGER;
			pInfo->m_btPos		= i;
			strcpy(pInfo->m_szTitle, m_szManagerTitle[i]);
			snprintf(pInfo->m_szName, sizeof(pInfo->m_szName), "%s", m_szManagerName[i]);
			return TRUE;
		}
	}

	if (m_psMember)
	{
		for (i = 0; i < m_nMemberPointSize; ++i)
		{
			if (m_psMember[i].m_dwNameID == dwNameID)
			{
				pInfo->m_btFigure	= enumTONG_FIGURE_MEMBER;
				pInfo->m_btPos		= i;
				strcpy(pInfo->m_szTitle, m_szNormalTitle);
				snprintf(pInfo->m_szName, sizeof(pInfo->m_szName), "%s", m_psMember[i].m_szName);
				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL CTongControl::DBInstate( char *lpszPlayerName, BYTE nSite )
{
	if (!lpszPlayerName[0] || nSite > 4)
		return FALSE;

	int		i, nOldPos, nNewPos;
	DWORD	dwNameID;
	char	szName[NAME_LENGHT];

	// sizeof(lpszPlayerName) is the width of the pointer, not of the name: this copied
	// eight bytes of a name that can be longer and left szName unterminated unless the
	// caller's bytes happened to hold a NUL inside those eight.  Round 22's shape, in
	// the one copy the strcpy-family gates did not read.
	size_t nNameLen = strlen(lpszPlayerName);
	if (nNameLen > sizeof(szName) - 1)
		nNameLen = sizeof(szName) - 1;
	memcpy(szName, lpszPlayerName, nNameLen);
	szName[nNameLen] = 0;
	dwNameID = g_String2IdA(szName);
	if (dwNameID == 0)
		return FALSE;

	if (!m_psMember)
		return FALSE;
	int		nPos = -1;

	for (i = 0; i < m_nMemberPointSize; ++i)
	{
		if (m_psMember[i].m_dwNameID != dwNameID)
			continue;
		nPos = i;
		break;
	}
	if (nPos < 0)
		return FALSE;
	nOldPos = 0;

	switch (nSite)
	{
	case enumTONG_FIGURE_DIRECTOR:
		for (i = 0; i < defTONG_MAX_DIRECTOR; ++i)
		{
			if (m_dwDirectorID[i] == 0 && !m_szDirectorName[i][0])
				break;
		}
		if (i >= defTONG_MAX_DIRECTOR)
			return FALSE;
		nNewPos = i;

		snprintf(m_szDirectorName[nNewPos], sizeof(m_szDirectorName[nNewPos]), "%s", szName);
		m_dwDirectorID[nNewPos] = dwNameID;
		this->m_psMember[nPos].m_szName[0] = 0;
		this->m_psMember[nPos].m_dwNameID = 0;
		m_nDirectorNum++;
		m_nMemberNum--;
		break;
	case enumTONG_FIGURE_MANAGER:
		for (i = 0; i < defTONG_MAX_MANAGER; ++i)
		{
			if (m_dwManagerID[i] == 0 && !m_szManagerName[i][0])
				break;
		}
		if (i >= defTONG_MAX_MANAGER)
			return FALSE;
		nNewPos = i;

		snprintf(m_szManagerName[nNewPos], sizeof(m_szManagerName[nNewPos]), "%s", szName);
		m_dwManagerID[nNewPos] = dwNameID;
		this->m_psMember[nPos].m_szName[0] = 0;
		this->m_psMember[nPos].m_dwNameID = 0;
		m_nManagerNum++;
		m_nMemberNum--;
		break;
	default:
		return FALSE;
	}

	// 存盘，数据保存至数据库
	TMemberStruct	sMember;
	if (nSite == enumTONG_FIGURE_DIRECTOR)
	{
		sMember.MemberClass = enumTONG_FIGURE_DIRECTOR;
	} 
	else if (nSite == enumTONG_FIGURE_MANAGER)
	{
		sMember.MemberClass = enumTONG_FIGURE_MANAGER;
	}
	sMember.nTitleIndex = nNewPos;
	snprintf(sMember.szTong, sizeof(sMember.szTong), "%s", this->m_szName);
	snprintf(sMember.szName, sizeof(sMember.szName), "%s", szName);
	g_cTongDB.ChangeMember(sMember);
	return TRUE;
}

BOOL CTongControl::ChangeMoneyControl(STONG_MONEY_COMMAND* pMoney ,STONG_MONEY_SYNC *Sync)
{
	if (!pMoney)
		return FALSE;

	//rTRACE("DEBUG, pMoney->m_szName: %s", pMoney->m_szName);
	//rTRACE("DEBUG, pMoney->m_dwMoney: %d", pMoney->m_dwMoney);
	//rTRACE("DEBUG, pMoney->m_dwParam: %d", pMoney->m_dwParam);
	//rTRACE("DEBUG, pMoney->m_dwTongNameID: %d", pMoney->m_dwTongNameID);

	// 聊天频道发消息
	char	szMsg[96];
	sprintf(szMsg, "\\O%d",m_dwNameID);  //m_dwNameID
	
	DWORD channid = g_ChannelMgr.GetChannelID(szMsg, 0);

    //char msg[64];
	//sprintf(msg,"帮会ID:%d",channid);
    //MessageBox(0,msg,"提示:",MB_OK);

	Sync->ProtocolFamily = pf_tong;
	Sync->m_dwTongNameID = m_dwNameID;
	Sync->m_dwParam      = pMoney->m_dwParam;
	snprintf(Sync->m_szName, sizeof(Sync->m_szName), "%s", pMoney->m_szName);

	
	UINT nPoint = 0; // diem cong hien

	switch(pMoney->ProtocolID)
	{
	case enumC2S_TONG_MONEY_SAVE:
		m_dwMoney += pMoney->m_dwMoney;

		Sync->ProtocolID = enumS2C_TONG_MONEY_SAVE;
		Sync->m_nMoney   = pMoney->m_dwMoney;
		Sync->m_dwMoney  = m_dwMoney;

		if(pMoney->m_dwMoney >= g_TongSetting.m_nPointPerMoney)
			nPoint = pMoney->m_dwMoney / g_TongSetting.m_nPointPerMoney;

		if (channid != -1)
		{
			snprintf(szMsg, sizeof(szMsg), "(%s)向帮会资金注入了(%d)两!", pMoney->m_szName,pMoney->m_dwMoney);
			//g_ChannelMgr.SayOnChannel(channid, TRUE, std::string(), std::string("帮会"), std::string(szMsg),conndup.GetIP());
		}
		break;
	case enumC2S_TONG_MONEY_GET:
		if (m_dwMoney < pMoney->m_dwMoney)
			return FALSE;
		m_dwMoney -= pMoney->m_dwMoney;

		Sync->ProtocolID = enumS2C_TONG_MONEY_GET;
		Sync->m_nMoney = pMoney->m_dwMoney;
		Sync->m_dwMoney = m_dwMoney;
		if (channid != -1)
		{
			snprintf(szMsg, sizeof(szMsg), "(%s)取走了帮会资金(%d)两!", pMoney->m_szName,pMoney->m_dwMoney);
			//g_ChannelMgr.SayOnChannel(channid, TRUE, std::string(), std::string("帮会"), std::string(szMsg),conndup.GetIP());
		}
		break;
	case enumC2S_TONG_MONEY_SND:
		if (m_dwMoney < pMoney->m_dwMoney)
			return FALSE;
		m_dwMoney -= pMoney->m_dwMoney;

		Sync->ProtocolID = enumS2C_TONG_MONEY_SND;
		Sync->m_nMoney = pMoney->m_dwMoney;
		Sync->m_dwMoney = m_dwMoney;

		if (channid != -1)
		{
			snprintf(szMsg, sizeof(szMsg), "(%s)给在线的队长发放了帮会资金总计(%d)两!", pMoney->m_szName,pMoney->m_dwMoney);
			//g_ChannelMgr.SayOnChannel(channid, TRUE, std::string(), std::string("帮会"), std::string(szMsg),conndup.GetIP());
		}
		break;
	}


	CNetConnectDup conndup;
	DWORD nameid = 0;
	unsigned long param = 0;
	if (g_TongServer.FindPlayerByRole(NULL, std::_tstring(pMoney->m_szName), &conndup, NULL, &nameid, &param))
	{
		g_ChannelMgr.SayOnChannel(channid, TRUE, std::string(), std::string("帮会"), std::string(szMsg),conndup.GetIP());
	}

	TMoneyStruct tMoney;
	snprintf(tMoney.szTong, sizeof(tMoney.szTong), "%s", m_szName);
	tMoney.m_dwMoney = m_dwMoney;
	g_cTongDB.DelMoney(m_szName);  //改变数据信息
	g_cTongDB.ChangeMoney(tMoney);
	//g_cTongDB.DelTong(m_szName);
	//g_cTongDB.ChangeTong(*this);

	STONG_ONE_LEADER_INFO sInfo;
	SearchOne(pMoney->m_szName, &sInfo);

	//rTRACE("DEBUG, sInfo.m_btFigure: %d", sInfo.m_btFigure);
	//rTRACE("DEBUG, sInfo.m_btPos: %d", sInfo.m_btPos);
	//rTRACE("DEBUG, sInfo.m_btPos: %s", sInfo.m_szName);
	//rTRACE("DEBUG, sInfo.m_btPos: %s", sInfo.m_szTitle);

	if (sInfo.m_btFigure == enumTONG_FIGURE_MASTER)
		m_nMasterPoint += nPoint;
	else if (sInfo.m_btFigure == enumTONG_FIGURE_DIRECTOR)
		m_nDirectorPoint[sInfo.m_btPos] += nPoint;
	else if (sInfo.m_btFigure == enumTONG_FIGURE_MANAGER)
		m_nManagerPoint[sInfo.m_btPos] += nPoint;
	else if (sInfo.m_btFigure == enumTONG_FIGURE_MEMBER)
		m_psMember[sInfo.m_btPos].m_nPoint += nPoint;

	g_cTongDB.ChangeTong(*this);

	TMemberStruct	sMember;
	sMember.MemberClass = (TONG_MEMBER_FIGURE)sInfo.m_btFigure;
	sMember.nTitleIndex = sInfo.m_btPos;
	snprintf(sMember.szTong, sizeof(sMember.szTong), "%s", this->m_szName);
	snprintf(sMember.szName, sizeof(sMember.szName), "%s", sInfo.m_szName);
	sMember.nPoint = nPoint;
	g_cTongDB.ChangeMember(sMember);

	return TRUE;
}


BOOL CTongControl::AddMoneyControl(int nMoney)
{
	m_dwMoney += nMoney;		
	TMoneyStruct tMoney;
	snprintf(tMoney.szTong, sizeof(tMoney.szTong), "%s", m_szName);
	tMoney.m_dwMoney = m_dwMoney;
	g_cTongDB.DelMoney(m_szName);  //删除原来数据
	g_cTongDB.ChangeMoney(tMoney);
	return TRUE;
}

BOOL CTongControl::SetPower(STONG_POWER_COMMAND* pPower, STONG_POWER_SYNC* pSync)
{
	if (!pPower || !pSync)
		return FALSE;

	// the slot is the client's BYTE and the rows are the header's defTONG_MAX_DIRECTOR,
	// seven of them: every read below - the power row this function copies into, the
	// director name it echoes back, and the copy of that row into the sync - walks the
	// array with the same unbounded number.  A position the tong has no director for is
	// a command for a slot that does not exist, which is the answer FALSE already gives.
	if (pPower->m_btPos >= defTONG_MAX_DIRECTOR)
		return FALSE;

	int i = 0;
	for (i = 0; i < defTONG_MAX_DIRECTOR; ++i)
	{
		if (m_dwDirectorID[i] == pPower->m_dwNameID)
		{
			memcpy(m_nDirectorPower[pPower->m_btPos], pPower->m_btPower, sizeof(pPower->m_btPower));
			break;
		}
	}

	if (i >= defTONG_MAX_DIRECTOR)
		return FALSE;

	pSync->ProtocolFamily = pf_tong;
	pSync->ProtocolID = enumS2C_TONG_SET_POWER;
	pSync->m_dwTongNameID = pPower->m_dwTongNameID;
	strcpy(pSync->m_TargetName, m_szDirectorName[pPower->m_btPos]);
	memcpy(pSync->m_btPower, m_nDirectorPower[pPower->m_btPos], sizeof(pPower->m_btPower));
	pSync->m_dwParam = pPower->m_dwParam;
	pSync->m_bSetted = false;

	CNetConnectDup conndup;
	{ {
			DWORD nameid = 0;
			unsigned long param = 0;
			if (g_TongServer.FindPlayerByRole(NULL, pSync->m_TargetName, &conndup, NULL, &nameid, &param))
			{
				CNetConnectDup tongconndup = g_TongServer.FindTongConnectByIP(conndup.GetIP());
				if (tongconndup.IsValid())
				{
					STONG_POWER_SYNC	sSync;

					sSync.ProtocolFamily = pf_tong;
					sSync.ProtocolID = enumS2C_TONG_SET_POWER;
					sSync.m_dwTongNameID = pPower->m_dwTongNameID;
					strcpy(sSync.m_TargetName, m_szDirectorName[pPower->m_btPos]);
					memcpy(sSync.m_btPower, pPower->m_btPower, sizeof(pPower->m_btPower));
					sSync.m_dwParam = param;
					sSync.m_bSetted = true;

					tongconndup.SendPackage((const void*)&sSync, sizeof(sSync));
				}
				else
				{
					//tong is not connect
				}

			}
			else
			{
				//not find
			}
		}}

	return TRUE;
}

BOOL CTongControl::GetPower(STONG_GET_POWER_COMMAND* pPower, STONG_POWER_SYNC* pSync)
{
	if (!pPower || !pSync)
		return FALSE;

	pSync->ProtocolFamily = pf_tong;
	pSync->ProtocolID = enumS2C_TONG_GET_POWER;
	pSync->m_dwTongNameID = pPower->m_dwTongNameID;
	snprintf(pSync->m_TargetName, sizeof(pSync->m_TargetName), "%s", pPower->m_szName);
	pSync->m_dwParam = pPower->m_nPlayerIndex;

	if (strcmp(pPower->m_szName, m_szMasterName) == 0)
	{
		memset(pSync->m_btPower, 1, sizeof(pSync->m_btPower));
		return TRUE;
	}

	for (int i = 0; i < defTONG_MAX_DIRECTOR; ++i)
	{
		if (strcmp(pPower->m_szName, m_szDirectorName[i]) == 0)
		{
			memcpy(pSync->m_btPower, m_nDirectorPower[i], sizeof(pSync->m_btPower));
			return TRUE;
		}
	}

	memset(pSync->m_btPower, 0, sizeof(pSync->m_btPower));

	return TRUE;
}

BOOL CTongControl::UpdateLevel(STONG_UPDATE_LEVEL_COMMAND* pUpdate)
{
	TMemberStruct tMember;
	snprintf(tMember.szName, sizeof(tMember.szName), "%s", pUpdate->m_szName);
	snprintf(tMember.szTong, sizeof(tMember.szTong), "%s", m_szName);

	if (pUpdate->m_btFigure == enumTONG_FIGURE_MASTER)
	{
		if (strcmp(pUpdate->m_szName, m_szMasterName))
			return FALSE;

		m_nMasterLevel = pUpdate->m_nLevel;

		tMember.MemberClass = enumTONG_FIGURE_MASTER;
		tMember.nLevel = m_nMasterLevel;
		tMember.nPoint = m_nMasterPoint;
		tMember.nTitleIndex = 0;

	}
	else if (pUpdate->m_btFigure == enumTONG_FIGURE_DIRECTOR)
	{
		for (int i = 0; i < defTONG_MAX_DIRECTOR; ++i)
		{
			if (strcmp(pUpdate->m_szName, m_szDirectorName[i]) == 0)
			{
				m_nDirectorLevel[i] = pUpdate->m_nLevel;

				tMember.MemberClass = enumTONG_FIGURE_DIRECTOR;
				tMember.nLevel = m_nDirectorLevel[i];
				tMember.nPoint = m_nDirectorPoint[i];
				tMember.nTitleIndex = i;

				break;
			}
		}
	}
	else if (pUpdate->m_btFigure == enumTONG_FIGURE_MANAGER)
	{
		for (int i = 0; i < defTONG_MAX_MANAGER; ++i)
		{
			if (strcmp(pUpdate->m_szName, m_szManagerName[i]) == 0)
			{
				m_nManagerLevel[i] = pUpdate->m_nLevel;

				tMember.MemberClass = enumTONG_FIGURE_MANAGER;
				tMember.nLevel = m_nManagerLevel[i];
				tMember.nPoint = m_nManagerPoint[i];
				tMember.nTitleIndex = i;

				break;
			}
		}
	}
	else if (pUpdate->m_btFigure == enumTONG_FIGURE_MEMBER)
	{
		for (int i = 0; i < m_nMemberPointSize; ++i)
		{
			if (strcmp(pUpdate->m_szName, m_psMember[i].m_szName) == 0)
			{
				m_psMember[i].m_nLevel = pUpdate->m_nLevel;

				tMember.MemberClass = enumTONG_FIGURE_MEMBER;
				tMember.nLevel = m_psMember[i].m_nLevel;
				tMember.nPoint = m_psMember[i].m_nPoint;
				tMember.nTitleIndex = i;

				break;
			}
		}
	}

	g_cTongDB.ChangeMember(tMember);
	g_cTongDB.ChangeTong(*this);

	return TRUE;
}

#define  TONG_SET_FILE ""

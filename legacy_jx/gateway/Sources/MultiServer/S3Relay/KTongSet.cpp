// KTongSet.cpp: implementation of the CTongSet class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Global.h"
#include "TongDB.h"
#include "KTongSet.h"
#include "S3Relay.h"
//#include "TongConnect.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//CNetConnect TongConn;

TongSettings g_TongSetting;

#define  ATTACK_TIME 30*60

CTongSet::CTongSet()
{
	g_SetRootPath(NULL);
	g_SetFilePath("\\");

	KIniFile tongFile;
	if (tongFile.Load("\\settings\\tong\\tongsettings.ini"))
	{
		tongFile.GetString("TongSet", "MasterTitle", "", g_TongSetting.m_szMasterTitle, sizeof(g_TongSetting.m_szMasterTitle));
		tongFile.GetString("TongSet", "DirectorTitle", "", g_TongSetting.m_szDirectorTitle, sizeof(g_TongSetting.m_szDirectorTitle));
		tongFile.GetString("TongSet", "ManagerTitle", "", g_TongSetting.m_szManagerTitle, sizeof(g_TongSetting.m_szManagerTitle));
		tongFile.GetString("TongSet", "MemberTitle", "", g_TongSetting.m_szMemberTitle, sizeof(g_TongSetting.m_szMemberTitle));
		tongFile.GetInteger("TongSet", "PointPerMoney", 1000, (int*)&g_TongSetting.m_nPointPerMoney);
		tongFile.GetInteger("TongSet", "CampPrice", 1000000, (int*)&g_TongSetting.m_nCampPrice);
	}
	else
	{
		strcpy(g_TongSetting.m_szMasterTitle, "");
		strcpy(g_TongSetting.m_szDirectorTitle, "");
		strcpy(g_TongSetting.m_szManagerTitle, "");
		strcpy(g_TongSetting.m_szMemberTitle, "");
		g_TongSetting.m_nPointPerMoney = 1000;
		g_TongSetting.m_nCampPrice = 1000000;
	}
	tongFile.Clear();

	m_pcTong = NULL;
	m_nTongPointSize = 0;
	Init(); //清零帮会数据
}

CTongSet::~CTongSet()
{
	DeleteAll();
}

void	CTongSet::Init()
{
	DeleteAll();
	m_pcTong = (CTongControl**)new LPVOID[defTONG_SET_INIT_POINT_NUM];
	m_nTongPointSize = defTONG_SET_INIT_POINT_NUM;                     //帮会数量限制  (数组的字节数)
	for (int i = 0; i < m_nTongPointSize; ++i)
	{
		m_pcTong[i] = NULL;
	}
}

void	CTongSet::DeleteAll()
{
	if (m_pcTong)
	{
		for (int i = 0; i < m_nTongPointSize; ++i)
		{
			if (m_pcTong[i])
			{
				delete m_pcTong[i];
				m_pcTong[i] = NULL;
			}
		}
		delete []m_pcTong;
		m_pcTong = NULL;
	}
	m_nTongPointSize = 0;
}


void CTongSet::SynAttackStime(TCHAR *nTime)
{
	if (!m_pcTong)
		return;
	//将时间同步到  GS
	STONG_ATTACKSTIME_SENDBACK	sInfo;
	sInfo.ProtocolFamily = pf_alltong;
	sInfo.ProtocolID     = enumS3C_TONG_SERVER_TIEM;
	snprintf(sInfo.m_szTime, sizeof(sInfo.m_szTime), "%s", nTime);
	sInfo.m_wLength = sizeof(STONG_ATTACKSTIME_SENDBACK);
	//this->SendPackage((const void *)&sInfo, sInfo.m_wLength);
	g_TongServer.BroadPackage((const void*)&sInfo, sInfo.m_wLength);
}

void CTongSet::SynAttackInfo()
{//宣战结束
	if (!m_pcTong || m_nTongPointSize <= 0)
		return;
	int nBlueTongIdx=-1,nGreenTongIdx=-1;
	
	int nTongIdx=UpTongAttackTime(&nBlueTongIdx,&nGreenTongIdx);
	
	if(nTongIdx>=0)
	{//宣战结束
		char nTonName[NAME_LENGHT];
		if (GetTongName(nTongIdx,nTonName))
		{//获取信息发回		GS
			STONG_ATTACK_SENDBACK	sInfo;
			sInfo.ProtocolFamily = pf_alltong;
			sInfo.ProtocolID     = enumS2C_TONG_ATTACK_OVER;
			
			if (GetTongMapByIdx(nTongIdx))
				sInfo.m_dwParam      = 1;
			else
				sInfo.m_dwParam      = 0;
			
			sInfo.m_dwNpcID      = 0;
			snprintf(sInfo.m_szAttackName, sizeof(sInfo.m_szAttackName), "%s", nTonName);
			sInfo.m_wLength = sizeof(STONG_ATTACK_SENDBACK);
			//this->SendPackage((const void *)&sInfo, sInfo.m_wLength);
			g_TongServer.BroadPackage((const void*)&sInfo, sInfo.m_wLength);
		}
	}

}




void CTongSet::SynWarCityInfo()
{
	STONG_CITY_INFO_SYNC sInfo;
	sInfo.ProtocolFamily = pf_alltong;
	sInfo.ProtocolID     = enumS3C_TONG_SYNCITY_INFO;
	/*sInfo.m_WarCityCount=gGetPrivateProfileIntEx("AreaCount","count",file_warcfg,0);
	if (sInfo.m_WarCityCount>7)*/
		sInfo.m_WarCityCount=7;
   	for (int i=0;i<7;++i)
	{//城战信息
		char szSecton[32];
		sprintf(szSecton,"AreaName0%d",i);
		std::_tstring strVal;
		sInfo.snWarInfo[i].m_idx=gGetPrivateProfileIntEx(szSecton,key_idx,file_warcfg,0);
		sInfo.snWarInfo[i].m_mapidx=gGetPrivateProfileIntEx(szSecton,key_mapidx,file_warcfg,0);
		strVal = gGetPrivateProfileStringEx(szSecton, key_name, file_warcfg);
		snprintf(sInfo.snWarInfo[i].m_szMapName, sizeof(sInfo.snWarInfo[i].m_szMapName), "%s", strVal.c_str());

		strVal = gGetPrivateProfileStringEx(szSecton, key_tonname, file_warcfg);
		snprintf(sInfo.snWarInfo[i].m_Tongmaster, sizeof(sInfo.snWarInfo[i].m_Tongmaster), "%s", strVal.c_str());

		strVal = gGetPrivateProfileStringEx(szSecton, key_tongmaster, file_warcfg);
		snprintf(sInfo.snWarInfo[i].m_Mastername, sizeof(sInfo.snWarInfo[i].m_Mastername), "%s", strVal.c_str());

		sInfo.snWarInfo[i].m_levle=gGetPrivateProfileIntEx(szSecton,key_level,file_warcfg,0);
		sInfo.snWarInfo[i].m_shushou=gGetPrivateProfileIntEx(szSecton,key_shuis,file_warcfg,0);
	}
	sInfo.m_wLength=sizeof(STONG_CITY_INFO_SYNC);
	//g_TongConn.SendPackage((const void *)&sInfo, sInfo.m_wLength);
	g_TongServer.BroadPackage((const void*)&sInfo, sizeof(sInfo));
}

void CTongSet::SynTongWarInfo()
{

	if (!m_pcTong || m_nTongPointSize <= 0)
		return;

	for (int i = 0; i < m_nTongPointSize; ++i)
	{//有帮会的才同步
		if (m_pcTong[i] && m_pcTong[i]->m_dwNameID)
		{
			STONG_ATTACK_INFO_SYNC	sInfo;
			sInfo.ProtocolFamily = pf_alltong;
	        sInfo.ProtocolID     = enumS2C_TONG_ATTACK_GETBACK;
			if (!GetTongAttackInfo(i, &sInfo))
			{
				sInfo.m_AttackState=0;
				sInfo.m_nTempCamp=0;
				sInfo.m_AttackTime=0;
				sInfo.m_CurMapIdx =0;
				sInfo.m_nLevel =0;
				sInfo.m_YDeathCount=0;
				sprintf(sInfo.m_szAttackName,"暂无");  //  对战帮会名
				sInfo.m_nAttackNum=0;
				sInfo.m_nWinNum=0;
				sInfo.m_nLoseNum=0;
			}
			sInfo.m_DDeathCount=g_cTongSet.GetTongDeathCount(g_String2IdA(sInfo.m_szAttackName)); //对战方的死亡次数
			sInfo.m_dwParam = m_pcTong[i]->m_dwNameID;
			sInfo.m_dwNpcID = 0;
			sInfo.m_wLength=sizeof(STONG_ATTACK_INFO_SYNC);
			//g_TongConn.gGs->SendPackage((const void *)&sInfo, sInfo.m_wLength);
			g_TongServer.BroadPackage((const void*)&sInfo,sInfo.m_wLength);//发往 GS  需要区别是哪个GS 

		}
	}

	
}

int CTongSet::RandomSelTongmap()
{
   	if (!m_pcTong || m_nTongPointSize <= 0)
		return -1;
	int i,j;
	for (i = 0, j = 0; i < m_nTongPointSize; ++i)
	{
		if (m_pcTong[i] && m_pcTong[i]->m_dwNameID)
		{
			++j;  //有多少个帮会
		}
	}

   if (j>0)
   {//有帮会   
	  int nTonIdx=GetRandomNumberS3(0,j-1);
	  int nSel=-1;
	  for (i = 0, j = 0; i < m_nTongPointSize; ++i)
	  {
		  if (m_pcTong[i] && m_pcTong[i]->m_dwNameID && j++ == nTonIdx)
		  {
			  nSel = i;
			  break;
		  }
	  }
	  if (nSel >= 0)
	  {
		  m_pcTong[nSel]->m_nTongMap=gGetPrivateProfileIntEx("tongmap","idx",file_warcfg,47);
		  gWritePrivateProfileStringEx("tongmap","tongmaster",file_warcfg,m_pcTong[nSel]->m_szName);	//写入占领的帮会名
		  g_cTongDB.ChangeTong(*m_pcTong[nSel]);

		  return nSel;
	  }
   }   

	return -1;
}



int CTongSet::UpTongAttackTime(int *nBlueTongIdx,int *nGreenTongIdx)
{
	// 帮会模块出错
	if (!m_pcTong || m_nTongPointSize <= 0)
		return -1;

	int inMoney=gGetPrivateProfileIntEx("AttackSet","addmoney",file_warcfg,0);

	for (int i = 0; i < m_nTongPointSize; ++i)
	{
		//m_pcTong[i]->;
		//rTRACE("-------------更新所有帮会宣战时间数据-------------OK..");
		if (m_pcTong[i] && m_pcTong[i]->m_AttackState>0)
		{
		//	continue;//下一个帮会
		  if (m_pcTong[i]->m_AttackTime>0)
		  { 
			m_pcTong[i]->m_AttackTime--;	 //每秒执行一次
			if (m_pcTong[i]->m_AttackTime<=0)
			{
				rTRACE("-------------帮会(%s)宣战时间结束-------------OK..",m_pcTong[i]->m_szName);

				//开始计算 死亡次数 分出输赢
				if (m_pcTong[i]->m_AttackState==1)
				{//由攻方计算 蓝方
					int nDtongIdx;
					    nDtongIdx= GetTongIdxByNameId(g_String2IdA(m_pcTong[i]->m_szAttackName));
					if 	(nDtongIdx>-1)
					{//如果有对战的帮会存在 开始对比输赢 绿方 
						 *nBlueTongIdx=i;
						 *nGreenTongIdx=nDtongIdx;  //一对的

						if (m_pcTong[i]->m_nDeathCount>m_pcTong[nDtongIdx]->m_nDeathCount)
						{//守方
						   m_pcTong[i]->m_nLoseNum +=1;
						   m_pcTong[nDtongIdx]->m_nWinNum +=1;
						   m_pcTong[nDtongIdx]->m_nLevel +=1; //帮会等级加一
						   m_pcTong[nDtongIdx]->AddMoneyControl(inMoney); //帮会资金增加

						   if (m_pcTong[i]->m_nTongMap>0 && m_pcTong[nDtongIdx]->m_nTongMap<=0)
						   {//对方有帮会地图的,胜利方没有的
							   m_pcTong[nDtongIdx]->m_nTongMap=m_pcTong[i]->m_nTongMap;
							   m_pcTong[i]->m_nTongMap=0;  //抢过来了
							   gWritePrivateProfileStringEx("tongmap","tongmaster",file_warcfg,m_pcTong[nDtongIdx]->m_szName);	//写入占领的帮会名

						   }
						}
						else// if (m_pcTong[i]->m_nDeathCount<m_pcTong[nDtongIdx]->m_nDeathCount)
						{//攻方
							m_pcTong[nDtongIdx]->m_nLoseNum +=1;
						    m_pcTong[i]->m_nWinNum +=1;
							m_pcTong[i]->m_nLevel +=1;
							m_pcTong[i]->AddMoneyControl(inMoney);	//帮会资金增加

							if (m_pcTong[nDtongIdx]->m_nTongMap>0 && m_pcTong[i]->m_nTongMap<=0)
							{//对方有帮会地图的,胜利方没有的
								m_pcTong[i]->m_nTongMap=m_pcTong[nDtongIdx]->m_nTongMap;
								m_pcTong[nDtongIdx]->m_nTongMap=0;  //抢过来了
								//gWritePrivateProfileIntEx("tongmap","tongmaster",file_warcfg,m_pcTong[i]->m_nTongMap);
								gWritePrivateProfileStringEx("tongmap","tongmaster",file_warcfg,m_pcTong[i]->m_szName);	//写入占领的帮会名
							}

						}
						g_cTongDB.ChangeTong(*m_pcTong[i]);				//更新数据库
						g_cTongDB.ChangeTong(*m_pcTong[nDtongIdx]);	
					}	
				}
				m_pcTong[i]->m_AttackTime=0;
				m_pcTong[i]->m_AttackState=0;
				m_pcTong[i]->m_nTempCamp =0;
				return i;
			}
		  } 
		}
	}
	  return -1;
}



int		CTongSet::Create(int nCamp, char* lpszPlayerName, char* lpszTongName, int nPlayerLevel)
{
	// 帮会模块出错
	if (!m_pcTong || m_nTongPointSize <= 0)
		return enumTONG_CREATE_ERROR_ID10;

	// 名字字符串出错
	if (!lpszPlayerName || !lpszTongName)
		return enumTONG_CREATE_ERROR_ID11;

	// 名字字符串过长
	if (strlen(lpszTongName) >= defTONG_STR_LENGTH || strlen(lpszPlayerName) >= defTONG_STR_LENGTH)
		return enumTONG_CREATE_ERROR_ID12;

	int		i, nPos;
	DWORD	dwTongNameID, dwPlayerNameID;  //没有符号的整数型

	dwTongNameID = g_String2IdA(lpszTongName);
	dwPlayerNameID = g_String2IdA(lpszPlayerName);
	//rTRACE("<<<<<<<<<<<<<<:玩家(%s)创建一个帮派(%s)>>>>>>>>>>>>>>",lpszPlayerName,lpszTongName);
	// 名称检测，是否有同名的帮主或者同名的帮会

	for (i = 0; i < m_nTongPointSize; ++i)
	{
		if (m_pcTong[i] && (m_pcTong[i]->m_dwNameID == dwTongNameID || m_pcTong[i]->m_dwMasterID == dwPlayerNameID))
			break;
	}

	// 帮会同名错误
	if (i < m_nTongPointSize)
		return enumTONG_CREATE_ERROR_ID13;

	// 查找空位
	nPos = -1;
	for (i = 0; i < m_nTongPointSize; ++i)
	{
		if (!m_pcTong[i])
		{
			nPos = i;
			break;
		}
	}

	// 现有指针空间已满，分配新的更大的指针空间 - het slot, mo rong kich thuoc vung nho
	if (nPos < 0)
	{
		// 临时存储旧的指针
		CTongControl** pTemp;
		pTemp = (CTongControl**)new LPVOID[m_nTongPointSize];
		for (i = 0; i < m_nTongPointSize; ++i)
			pTemp[i] = m_pcTong[i];

		// 分配新的更大的指针空间，大小是原来的两倍
		delete[]m_pcTong;
		m_pcTong = NULL;
		m_pcTong = (CTongControl**)new LPVOID[m_nTongPointSize * 2];
		for (i = 0; i < m_nTongPointSize; ++i)
			m_pcTong[i] = pTemp[i];
		delete[]pTemp;
		pTemp = NULL;
		m_nTongPointSize *= 2;
		for (i = m_nTongPointSize / 2; i < m_nTongPointSize; ++i)
			m_pcTong[i] = NULL;
		nPos = m_nTongPointSize / 2;
	}

	// 产生一个新的帮会
	m_pcTong[nPos] = new CTongControl(nCamp, lpszPlayerName, lpszTongName); // tao bang
	
	// 产生失败
	if (m_pcTong[nPos]->m_dwNameID == 0)
	{
		delete m_pcTong[nPos];
		m_pcTong[nPos] = NULL;
		return enumTONG_CREATE_ERROR_ID14;  //产生帮会失败
	}

	m_pcTong[nPos]->m_nMasterLevel = nPlayerLevel;

	// 存盘，数据保存至数据库 - luu thanh vien(Bang chu)
	TMemberStruct	sMember;  //帮会保存入数据库的信息结构
	ZeroMemory(&sMember, sizeof(sMember));
	sMember.MemberClass = enumTONG_FIGURE_MASTER;
	sMember.nTitleIndex = 0; //第一个位置为帮主
	sMember.nPoint = 0;
	sMember.nLevel = nPlayerLevel;
	snprintf(sMember.szTong, sizeof(sMember.szTong), "%s", m_pcTong[nPos]->m_szName);
	snprintf(sMember.szName, sizeof(sMember.szName), "%s", m_pcTong[nPos]->m_szMasterName);

	try
	{
		g_cTongDB.ChangeTong(*m_pcTong[nPos]);//改变帮会设置
		g_cTongDB.ChangeMember(sMember);      //增加帮众
	}
	catch (...)
	{
		char	szMsg[96];
		sprintf(szMsg, "世界");

		DWORD channid = g_ChannelMgr.GetChannelID(szMsg, 0);
		if (channid != -1)
		{//如果有世界频道的
			sprintf(szMsg, "(%s)创建出错!!", m_pcTong[nPos]->m_szName);
			CNetConnectDup conndup;
			DWORD nameid = 0;
			unsigned long param = 0;
			if (g_TongServer.FindPlayerByRole(NULL, std::_tstring(lpszPlayerName), &conndup, NULL, &nameid, &param))
			{
				//g_ChannelMgr.SayOnChannel(channid, TRUE, std::string(), std::string("公告"), std::string(szMsg),conndup.GetID());
				g_ChannelMgr.SayOnChannel(channid, TRUE, std::string(), std::string("帮会"), std::string(szMsg), conndup.GetIP());
			}

		}
	}
	//rTRACE("<<<<<<<<<<<<<<:玩家(%s)创建一个帮派(%s)>>>>>>>>>>>>>>", lpszPlayerName, lpszTongName);
	rTRACE("<<<<<<<<<<<<<<Player %s Created Tong: %s>>>>>>>>>>>>>>", lpszPlayerName, lpszTongName);
	return 0;
}

//----------------------------------------------------------------------
//	功能：添加一个帮会成员，if return == 0 成功 else return error id
//----------------------------------------------------------------------
int		CTongSet::AddMember(char* lpszPlayerName, char* lpszTongName, int nPlayerLevel)
{
	if (!m_pcTong || m_nTongPointSize <= 0)
		return -1;

	if (!lpszPlayerName || !lpszPlayerName[0] || !lpszTongName || !lpszTongName[0])
		return -1;

	if (strlen(lpszTongName) >= defTONG_STR_LENGTH || strlen(lpszPlayerName) >= defTONG_STR_LENGTH)
		return -1;

	int		i;
	DWORD	dwTongNameID;

	dwTongNameID = g_String2IdA(lpszTongName);

	// 寻找帮会
	for (i = 0; i < m_nTongPointSize; ++i)
	{
		// 找到了
		if (m_pcTong[i] && (m_pcTong[i]->m_dwNameID == dwTongNameID))
		{
			// the callee is CTongControl::AddMember, which returns UINT and reports failure as -1;
			// held as a UINT that -1 is 0xFFFFFFFF and `idx < 0' cannot be true, so the else branch
			// below read m_psMember[idx] past the end of the member array instead of returning -1.
			int idx = m_pcTong[i]->AddMember(lpszPlayerName, nPlayerLevel, 0);
			if (idx < 0)
				return -1;
			else
			{
				// 存盘，数据保存至数据库
				TMemberStruct	sMember;
				ZeroMemory(&sMember, sizeof(sMember));
				sMember.MemberClass = enumTONG_FIGURE_MEMBER;
				sMember.nTitleIndex = idx;
				sMember.nPoint = m_pcTong[i]->m_psMember[idx].m_nPoint;
				sMember.nLevel = m_pcTong[i]->m_psMember[idx].m_nLevel;
				snprintf(sMember.szTong, sizeof(sMember.szTong), "%s", m_pcTong[i]->m_szName);
				snprintf(sMember.szName, sizeof(sMember.szName), "%s", lpszPlayerName);
				
				g_cTongDB.ChangeMember(sMember); // add 1 member vao data

				// 给帮会频道发消息 - thong bao
				char	szMsg[96];
				sprintf(szMsg, "\\O%d", m_pcTong[i]->m_dwNameID);
				DWORD channid = g_ChannelMgr.GetChannelID(szMsg, 0);
				if (channid != -1)
				{//有帮会频道的
					sprintf(szMsg, "(%s)加入本帮！", lpszPlayerName);
					CNetConnectDup conndup;
					DWORD nameid = 0;
					unsigned long param = 0;
					if (g_TongServer.FindPlayerByRole(NULL, std::_tstring(lpszPlayerName), &conndup, NULL, &nameid, &param))
					{
						//g_ChannelMgr.SayOnChannel(channid, TRUE, std::string(), std::string("公告"), std::string(szMsg),conndup.GetID());
						g_ChannelMgr.SayOnChannel(channid, TRUE, std::string(), std::string("帮会"), std::string(szMsg), conndup.GetIP());
					}

				}
				return i;  //返回帮会在数据库中的排序编号
			}
		}
	}

	return -1;
}



BOOL	CTongSet::GetTongNameById(DWORD dwTongNameID, char *lpszName)
{
	if (!lpszName)
		return FALSE;
	if (!m_pcTong || m_nTongPointSize <= 0)
		return FALSE;

	for (int i = 0; i < m_nTongPointSize; ++i)
	{
		// 找到了
		if (m_pcTong[i] && (m_pcTong[i]->m_dwNameID == dwTongNameID))
		{
		   snprintf(lpszName, sizeof(m_pcTong[i]->m_szName), "%s", m_pcTong[i]->m_szName);
		   return TRUE;
		}
	}

	return FALSE;
}

int   CTongSet::GetTongIdxByNameId(DWORD dwTongNameID)
{
	  	if (!m_pcTong || m_nTongPointSize <= 0)
			return -1;
		
		for (int i = 0; i < m_nTongPointSize; ++i)
		{
			// 找到了
			if (m_pcTong[i] && (m_pcTong[i]->m_dwNameID == dwTongNameID))
			{
				return i;
			}
		}
		
		return -1;
}

BOOL  CTongSet::GetTongName(int nTongIdx, char *lpszName)
{
	  	if (!lpszName)
			return FALSE;
		if (!m_pcTong || m_nTongPointSize <= 0)
			return FALSE;
		if (nTongIdx < 0 || nTongIdx >= m_nTongPointSize)
			return FALSE;
		if (!m_pcTong[nTongIdx])
			return FALSE;
		snprintf(lpszName, sizeof(m_pcTong[nTongIdx]->m_szName), "%s", m_pcTong[nTongIdx]->m_szName);
       	return TRUE;
}





BOOL   CTongSet::UpDataTongInfo(DWORD dwTongNameID)
{
	if (!m_pcTong || m_nTongPointSize <= 0)
		return FALSE;
	int nCount=0;
	for (int i = 0; i < m_nTongPointSize; ++i)
	{
		if (m_pcTong[i] && m_pcTong[i]->m_dwNameID==dwTongNameID)
		{
		   return g_cTongDB.ChangeTong(*m_pcTong[i]);//改变帮会设置
		}
	}

	return TRUE;
}



//----------------------------------------------------------------------
//	功能：获得帮会数量
//----------------------------------------------------------------------
int	CTongSet::GetTongCount()
{
	   if (!m_pcTong || m_nTongPointSize <= 0)
		return 0;
		int nCount=0;
		for (int i = 0; i < m_nTongPointSize; ++i)
		{
			if (m_pcTong[i] && m_pcTong[i]->m_dwNameID)
			{
				nCount++;
			}
		} 
	
	return nCount;
}


BOOL  CTongSet::SetTongWinNum(DWORD dwTongNameID,int nCount)	      //设置胜利次数
{
	if (!m_pcTong || m_nTongPointSize <= 0)
		return FALSE;
	for (int i = 0; i < m_nTongPointSize; ++i)
	{
		if (m_pcTong[i] && m_pcTong[i]->m_dwNameID==dwTongNameID)
		{
			m_pcTong[i]->m_nWinNum +=nCount;
			g_cTongDB.ChangeTong(*m_pcTong[i]); //不存档
			return TRUE;
		}
	}
	return FALSE;
}

BOOL  CTongSet::SetTongLoseNum(DWORD dwTongNameID,int nCount)	      //设置失败次数
{
	if (!m_pcTong || m_nTongPointSize <= 0)
		return FALSE;
	for (int i = 0; i < m_nTongPointSize; ++i)
	{
		if (m_pcTong[i] && m_pcTong[i]->m_dwNameID==dwTongNameID)
		{
			m_pcTong[i]->m_nLoseNum +=nCount;
			g_cTongDB.ChangeTong(*m_pcTong[i]); //不存档
			return TRUE;
		}
	}
	return FALSE;
}

BOOL	CTongSet::SetTongKillNum(DWORD dwTongNameID,int nCount)
{
	if (!m_pcTong || m_nTongPointSize <= 0)
		return FALSE;
	for (int i = 0; i < m_nTongPointSize; ++i)
	{
		if (m_pcTong[i] && m_pcTong[i]->m_dwNameID==dwTongNameID)
		{
			m_pcTong[i]->m_nDeathCount +=nCount;
			//g_cTongDB.ChangeTong(*m_pcTong[i]); //不存档
			return TRUE;
		}
	}
	return FALSE;
}

BOOL	CTongSet::SetTongLevel(DWORD dwTongNameID,int nLevel)	          //等级
{
	if (!m_pcTong || m_nTongPointSize <= 0)
		return FALSE;
	for (int i = 0; i < m_nTongPointSize; ++i)
	{
		if (m_pcTong[i] && m_pcTong[i]->m_dwNameID==dwTongNameID)
		{
			m_pcTong[i]->m_nLevel +=nLevel;
			g_cTongDB.ChangeTong(*m_pcTong[i]);

			STONG_CHANGE_DATA_SYNC sInfo;
			ZeroMemory(&sInfo, sizeof(sInfo));
			sInfo.ProtocolFamily = pf_alltong;
			sInfo.ProtocolID = enumS2C_TONG_CHANGE_DATA;
			sInfo.m_dwTongNameID = dwTongNameID;
			sInfo.m_btMsgId = enumC2S_TONG_SET_LEVEL;
			sInfo.m_btParam = m_pcTong[i]->m_nLevel;
			sInfo.m_wLength = sizeof(STONG_CHANGE_DATA_SYNC);

			g_TongServer.BroadPackage(&sInfo, sInfo.m_wLength);
			return TRUE;
		}
	}
	return FALSE;
}
BOOL	CTongSet::SetTongCredit(DWORD dwTongNameID,int nCredit)           //建设资金
{
	if (!m_pcTong || m_nTongPointSize <= 0)
		return FALSE;
	for (int i = 0; i < m_nTongPointSize; ++i)
	{
		if (m_pcTong[i] && m_pcTong[i]->m_dwNameID==dwTongNameID)
		{
			m_pcTong[i]->m_nCredit=nCredit;
			g_cTongDB.ChangeTong(*m_pcTong[i]);
			return TRUE;
		}
	}
	return FALSE;
}
BOOL	CTongSet::SetTongMap(DWORD dwTongNameID,int nMapIdx)	              //地图
{
	if (!m_pcTong || m_nTongPointSize <= 0)
		return FALSE;
	for (int i = 0; i < m_nTongPointSize; ++i)
	{
		if (m_pcTong[i] && m_pcTong[i]->m_dwNameID==dwTongNameID)
		{
			m_pcTong[i]->m_nTongMap=nMapIdx;
			//开始写文件
			gWritePrivateProfileStringEx("tongmap","tongmaster",file_warcfg,m_pcTong[i]->m_szName);	//写入占领的帮会名
			g_cTongDB.ChangeTong(*m_pcTong[i]);
			return TRUE;
		}
	}
	return FALSE;
}

BOOL	CTongSet::SetTongMasterTitle(DWORD dwTongNameID,char *nTitle)  //帮主称号
{
	if (!m_pcTong || m_nTongPointSize <= 0)
		return FALSE;

	for (int i = 0; i < m_nTongPointSize; ++i)
	{
		if (m_pcTong[i] && m_pcTong[i]->m_dwNameID==dwTongNameID)
		{
			strcpy(m_pcTong[i]->m_szMasterTitle,nTitle);
			g_cTongDB.ChangeTong(*m_pcTong[i]);
			return TRUE;
		}
	}
	return FALSE;
}
BOOL	CTongSet::SetTongDirectorTitle(DWORD dwTongNameID,char *nTitle, int nPos)//长老称号
{
	if (!m_pcTong || m_nTongPointSize <= 0)
		return FALSE;

	if (nPos < 0 || nPos >= defTONG_MAX_DIRECTOR)
		return FALSE;

	for (int i = 0; i < m_nTongPointSize; ++i)
	{
		if (m_pcTong[i] && m_pcTong[i]->m_dwNameID == dwTongNameID)
		{
			snprintf(m_pcTong[i]->m_szDirectorTitle[nPos], sizeof(m_pcTong[i]->m_szDirectorTitle[nPos]), "%s", nTitle);

			g_cTongDB.ChangeTong(*m_pcTong[i]);
			return TRUE;
		}

		//if (m_pcTong[i] && m_pcTong[i]->m_dwNameID==dwTongNameID)
		//{
		//	for (int j=0;j<defTONG_MAX_DIRECTOR;++j)
		//	  sprintf(m_pcTong[i]->m_szDirectorTitle[j],nTitle);

		//	g_cTongDB.ChangeTong(*m_pcTong[i]);
		//	return TRUE;
		//}
	}
	return FALSE;
}
BOOL	CTongSet::SetTongManagerTitle(DWORD dwTongNameID,char *nTitle, int nPos) //队长称号
{
	if (!m_pcTong || m_nTongPointSize <= 0)
		return FALSE;

	if (nPos < 0 || nPos >= defTONG_MAX_MANAGER)
		return FALSE;

	for (int i = 0; i < m_nTongPointSize; ++i)
	{
		if (m_pcTong[i] && m_pcTong[i]->m_dwNameID == dwTongNameID)
		{
			snprintf(m_pcTong[i]->m_szManagerTitle[nPos], sizeof(m_pcTong[i]->m_szManagerTitle[nPos]), "%s", nTitle);

			g_cTongDB.ChangeTong(*m_pcTong[i]);
			return TRUE;
		}

		//if (m_pcTong[i] && m_pcTong[i]->m_dwNameID==dwTongNameID)
		//{
		//	for (int j=0;j<defTONG_MAX_MANAGER;++j)
		//	  sprintf(m_pcTong[i]->m_szManagerTitle[j],nTitle);

		//	g_cTongDB.ChangeTong(*m_pcTong[i]);
		//	return TRUE;
		//}
	}
	return FALSE;
}

BOOL	CTongSet::SetTongNormalTitle(DWORD dwTongNameID, char* nTitle, int nGender)  //帮众称号
{
	if (!m_pcTong || m_nTongPointSize <= 0)
		return FALSE;

	for (int i = 0; i < m_nTongPointSize; ++i)
	{
		if (m_pcTong[i] && m_pcTong[i]->m_dwNameID == dwTongNameID)
		{
			if(nGender == 0)
				strcpy(m_pcTong[i]->m_szManTitle, nTitle);
			else
				strcpy(m_pcTong[i]->m_szWomanTitle, nTitle);

			g_cTongDB.ChangeTong(*m_pcTong[i]);

			STONG_CHANGE_DATA_SYNC sInfo;
			ZeroMemory(&sInfo, sizeof(sInfo));
			sInfo.ProtocolFamily = pf_alltong;
			sInfo.ProtocolID = enumS2C_TONG_CHANGE_DATA;
			sInfo.m_dwTongNameID = dwTongNameID;
			sInfo.m_btMsgId = enumC2S_TONG_SET_ZONGTITLE;
			sInfo.m_btParam = nGender;
			strcpy(sInfo.m_szParam, nTitle);
			sInfo.m_wLength = sizeof(STONG_CHANGE_DATA_SYNC);

			g_TongServer.BroadPackage(&sInfo, sInfo.m_wLength);

			return TRUE;
		}
	}
	return FALSE;
}

BOOL CTongSet::SetTongCam(DWORD dwTongNameID, int CamId)
{
	if (!m_pcTong || m_nTongPointSize <= 0)
		return FALSE;

	for (int i = 0; i < m_nTongPointSize; i++)
	{
		if (m_pcTong[i] && m_pcTong[i]->m_dwNameID == dwTongNameID)
		{
			m_pcTong[i]->m_nCamp = CamId;
			m_pcTong[i]->m_dwMoney -= g_TongSetting.m_nCampPrice;
			g_cTongDB.ChangeTong(*m_pcTong[i]);

			STONG_CHANGE_DATA_SYNC sInfo;
			ZeroMemory(&sInfo, sizeof(sInfo));
			sInfo.ProtocolFamily = pf_alltong;
			sInfo.ProtocolID = enumS2C_TONG_CHANGE_DATA;

			sInfo.m_dwTongNameID = dwTongNameID;
			sInfo.m_btMsgId = enumC2S_TONG_SET_CAM;
			sInfo.m_btParam = CamId;
			sInfo.m_wLength = sizeof(STONG_CHANGE_DATA_SYNC);

			g_TongServer.BroadPackage(&sInfo, sInfo.m_wLength);

			return TRUE;
		}
	}

	return FALSE;
}

BOOL CTongSet::ChangeAnnounce(DWORD dwTongNameID, CHAR* szAnnounce)
{
	if (!m_pcTong || m_nTongPointSize <= 0)
		return FALSE;

	for (int i = 0; i < m_nTongPointSize; i++)
	{
		if (m_pcTong[i] && m_pcTong[i]->m_dwNameID == dwTongNameID)
		{
			strcpy(m_pcTong[i]->m_szAnnounce, szAnnounce);
			g_cTongDB.ChangeTong(*m_pcTong[i]);

			STONG_ANNOUNCE_INFO_SYNC sInfo;
			ZeroMemory(&sInfo, sizeof(sInfo));
			sInfo.ProtocolFamily = pf_alltong;
			sInfo.ProtocolID = enumS2C_TONG_CHANGE_ANNOUNCE;
			sInfo.m_dwTongNameID = dwTongNameID;
			strcpy(sInfo.m_szAnnounce, szAnnounce);
			sInfo.m_wLength = sizeof(STONG_CHANGE_DATA_SYNC);

			g_TongServer.BroadPackage(&sInfo, sInfo.m_wLength);

			return TRUE;
		}
	}

	return FALSE;
}

int   CTongSet::GetTongDeathCount(DWORD dwTongNameID)
{
	if (!m_pcTong || m_nTongPointSize <= 0)
		return FALSE;
	for (int i = 0; i < m_nTongPointSize; ++i)
	{
		if (m_pcTong[i] && m_pcTong[i]->m_dwNameID==dwTongNameID)
		{
			return  m_pcTong[i]->m_nDeathCount;
		}
	}
	return FALSE;
}


int     CTongSet::GetAllMenberCount(DWORD dwTongNameID)	              //获取全部人数数量
{
	if (!m_pcTong || m_nTongPointSize <= 0)
		return FALSE;
	for (int i = 0; i < m_nTongPointSize; ++i)
	{
		if (m_pcTong[i] && m_pcTong[i]->m_dwNameID==dwTongNameID)
		{
			return  m_pcTong[i]->m_nDirectorNum+ m_pcTong[i]->m_nManagerNum+ m_pcTong[i]->m_nMemberNum+1;
		}
	}
	return FALSE;
}



int	CTongSet::GetTongMapByIdx(int nTongIdx)
{
	if (!m_pcTong || m_nTongPointSize <= 0)
		return 0;
	if (nTongIdx < 0 || nTongIdx >= m_nTongPointSize)
		return 0;
	
	if (m_pcTong[nTongIdx])
		return m_pcTong[nTongIdx]->m_nTongMap;
	
	return 0;
}
//----------------------------------------------------------------------
//	功能：获得帮会阵营
//----------------------------------------------------------------------
int		CTongSet::GetTongCamp(int nTongIdx)
{
	if (!m_pcTong || m_nTongPointSize <= 0)
		return -1;
	if (nTongIdx < 0 || nTongIdx >= m_nTongPointSize)
		return -1;

	if (m_pcTong[nTongIdx])
		return m_pcTong[nTongIdx]->m_nCamp;

	return -1;
}
//获取帮主名字
BOOL	CTongSet::GetMasterName(int nTongIdx, char *lpszName)
{
	if (!lpszName)
		return FALSE;
	if (!m_pcTong || m_nTongPointSize <= 0)
		return FALSE;
	if (nTongIdx < 0 || nTongIdx >= m_nTongPointSize)
		return FALSE;
	if (!m_pcTong[nTongIdx])
		return FALSE;
	strcpy(lpszName, m_pcTong[nTongIdx]->m_szMasterName);
	return TRUE;
}

BOOL CTongSet::GetMasterTitle(char* lpszTongName, char* lpszMasterTitle)
{
	if (!lpszTongName || !lpszMasterTitle)
		return FALSE;

	for (int i = 0; i < m_nTongPointSize; ++i)
	{
		if (m_pcTong[i] && m_pcTong[i]->m_dwNameID == g_String2IdA(lpszTongName))
		{
			strcpy(lpszMasterTitle, m_pcTong[i]->m_szMasterTitle);
			return TRUE;
		}
	}

	return FALSE;
}

//获取称号
BOOL	CTongSet::GetMemberTitle(int nTongIdx, char *lpszTitle, int nGender)
{
	if (!lpszTitle)
		return FALSE;
	if (!m_pcTong || m_nTongPointSize <= 0)
		return FALSE;
	if (nTongIdx < 0 || nTongIdx >= m_nTongPointSize)
		return FALSE;
	if (!m_pcTong[nTongIdx])
		return FALSE;

	if(nGender == 0 && m_pcTong[nTongIdx]->m_szManTitle && m_pcTong[nTongIdx]->m_szManTitle[0])
		strcpy(lpszTitle, m_pcTong[nTongIdx]->m_szManTitle);
	else if(nGender == 1 && m_pcTong[nTongIdx]->m_szWomanTitle && m_pcTong[nTongIdx]->m_szWomanTitle[0])
		strcpy(lpszTitle, m_pcTong[nTongIdx]->m_szWomanTitle);
	else
		strcpy(lpszTitle, m_pcTong[nTongIdx]->m_szNormalTitle);

	return TRUE;
}

//开始宣战
BOOL CTongSet::SetTongAttackInfo(DWORD dwYTongNameID,DWORD dwDTongNameID)
{
	if (!m_pcTong || m_nTongPointSize <= 0 || dwYTongNameID == 0 || dwDTongNameID==0)
		return FALSE;
	
	//寻找帮会
	int nDidx=-1,nYidx=-1;
	for (int i = 0; i < m_nTongPointSize; ++i)
	{
		if (m_pcTong[i])
		{
		   if (m_pcTong[i]->m_dwNameID == dwYTongNameID)
		   {//宣战方
			   nYidx=i;
		   }

		   if (m_pcTong[i]->m_dwNameID == dwDTongNameID)
		   {//被宣战方
			    nDidx=i;
		   }
		}
	}
	
	if (nYidx>=0 && nDidx>=0 && m_pcTong[nYidx]->m_AttackState==0 && m_pcTong[nDidx]->m_AttackState==0)
	{
		m_pcTong[nYidx]->m_AttackState=1;
	    m_pcTong[nYidx]->m_AttackTime=ATTACK_TIME;
		m_pcTong[nYidx]->m_nDeathCount=0;
		m_pcTong[nYidx]->m_nAttackNum +=1;
		g_cTongDB.ChangeTong(*m_pcTong[nYidx]);

		m_pcTong[nDidx]->m_AttackState=2;
		m_pcTong[nDidx]->m_AttackTime=ATTACK_TIME;
		m_pcTong[nDidx]->m_nDeathCount=0;
		m_pcTong[nDidx]->m_nAttackNum +=1;
	    g_cTongDB.ChangeTong(*m_pcTong[nDidx]);

		snprintf(m_pcTong[nYidx]->m_szAttackName, sizeof(m_pcTong[nYidx]->m_szAttackName), "%s", m_pcTong[nDidx]->m_szName);
		snprintf(m_pcTong[nDidx]->m_szAttackName, sizeof(m_pcTong[nDidx]->m_szAttackName), "%s", m_pcTong[nYidx]->m_szName);
	}
	else
	{
		if (nYidx>=0 && m_pcTong[nYidx]->m_AttackState==0)
		{
			m_pcTong[nYidx]->m_AttackState=0;
			m_pcTong[nYidx]->m_AttackTime=0;
			if (nDidx>=0 && m_pcTong[nDidx]) m_pcTong[nDidx]->m_nDeathCount=0;
		}

		if (nDidx>=0 && m_pcTong[nDidx]->m_AttackState==0)
		{
			m_pcTong[nDidx]->m_AttackState=0;
			m_pcTong[nDidx]->m_AttackTime=0;
			m_pcTong[nDidx]->m_nDeathCount=0;
		}

		return FALSE;
	}

	return TRUE;

}


BOOL CTongSet::GetTongAttackInfo(int dwTongindex, STONG_ATTACK_INFO_SYNC *pInfo)
{
	if (!m_pcTong || m_nTongPointSize <= 0)
		return FALSE;

	return m_pcTong[dwTongindex]->GetTongAttackInfo(pInfo);
 /*
	// 寻找帮会
	for (int i = 0; i < m_nTongPointSize; ++i)
	{
		if (m_pcTong[i] && m_pcTong[i]->m_dwNameID == dwTongNameID)
		{
			return m_pcTong[i]->GetTongAttackInfo(pInfo);
		}
	} */
	//return FALSE;
}

BOOL CTongSet::GetTongHeadInfo(DWORD dwTongNameID, STONG_HEAD_INFO_SYNC *pInfo)
{
	if (!m_pcTong || m_nTongPointSize <= 0 || dwTongNameID == 0)
		return FALSE;

	// 寻找帮会
	for (int i = 0; i < m_nTongPointSize; ++i)
	{
		if (m_pcTong[i] && m_pcTong[i]->m_dwNameID == dwTongNameID)
		{
			return m_pcTong[i]->GetTongHeadInfo(pInfo);
		}
	}

	return FALSE;
}

BOOL CTongSet::SearchInfo(STONG_SEARCH_INFO_COMMAND* pSearch, STONG_SEARCH_INFO_SYNC* pSync)
{
	if (!pSearch || !pSync)
		return FALSE;

	if (!m_pcTong)
		return FALSE;

	if (pSearch->m_nNum <= 0 || pSearch->m_nStart < 0)
		return FALSE;

	// m_nNum is the peer's, the page it writes into is the header's: the three builders
	// this door dispatches to run m_nStart..m_nStart+m_nNum over m_sMember[defTONG_ONE_PAGE_MAX_NUM],
	// so a count above the page depth is not a longer page but a write past the record, and the
	// wrapped (defTONG_ONE_PAGE_MAX_NUM - m_nCurNum) below then sizes the packet it sends back.
	// GetTongListInfo in this file clips its own nNeedNum the same way.
	if (pSearch->m_nNum > defTONG_ONE_PAGE_MAX_NUM)
		pSearch->m_nNum = defTONG_ONE_PAGE_MAX_NUM;

	for (int i = 0; i < m_nTongPointSize; ++i)
	{
		if (m_pcTong[i] && m_pcTong[i]->m_dwNameID == (DWORD)pSearch->m_dwTongNameID)
		{
			pSync->ProtocolFamily = pf_tong;
			pSync->ProtocolID = enumS2C_TONG_SEARCH_INFO;
			pSync->m_nType = pSearch->m_nType;
			pSync->m_nPlayerIndex = pSearch->m_nPlayerIndex;
			pSync->m_nStart = pSearch->m_nStart;
			if (pSearch->m_nType == 0) // search theo diem cong hien
				return m_pcTong[i]->GetTopPointControl(pSearch, pSync);
			else if (pSearch->m_nType == 1) // search theo cap do
				return m_pcTong[i]->GetTopLevelControl(pSearch, pSync);
			else if (pSearch->m_nType == 2) // search theo online/offline
				return m_pcTong[i]->GetOnOffControl(pSearch, pSync);
		}
	}

	return FALSE;
}

BOOL	CTongSet::GetTongManagerInfo(STONG_GET_MANAGER_INFO_COMMAND *pApply, STONG_MANAGER_INFO_SYNC *pInfo)
{
	if (!pApply || !pInfo)
		return FALSE;
	if (!m_pcTong)
		return FALSE;

	// 寻找帮会
	for (int i = 0; i < m_nTongPointSize; ++i)
	{
		if (m_pcTong[i] && m_pcTong[i]->m_dwNameID == (DWORD)pApply->m_nParam1)
		{
			return m_pcTong[i]->GetTongManagerInfo(pApply, pInfo);
		}
	}

	return FALSE;
}

BOOL	CTongSet::GetTongMemberInfo(STONG_GET_MEMBER_INFO_COMMAND *pApply, STONG_MEMBER_INFO_SYNC *pInfo)
{
	if (!pApply || !pInfo)
		return FALSE;
	if (!m_pcTong)
		return FALSE;

	// 寻找帮会
	for (int i = 0; i < m_nTongPointSize; ++i)
	{
		if (m_pcTong[i] && m_pcTong[i]->m_dwNameID == (DWORD)pApply->m_nParam1)
		{
			return m_pcTong[i]->GetTongMemberInfo(pApply, pInfo);
		}
	}

	return FALSE;
}

//帮会任命
BOOL	CTongSet::Instate(STONG_INSTATE_COMMAND *pInstate, STONG_INSTATE_SYNC *pSync)
{
	if (!pInstate || !pSync)
		return FALSE;
	if (!m_pcTong)
		return FALSE;

	// 寻找帮会
	for (int i = 0; i < m_nTongPointSize; ++i)
	{
		if (m_pcTong[i] && m_pcTong[i]->m_dwNameID == pInstate->m_dwTongNameID)
		{
			return m_pcTong[i]->Instate(pInstate, pSync);
		}
	}

	return FALSE;
}
//剔出帮会
BOOL	CTongSet::Kick(STONG_KICK_COMMAND *pKick, STONG_KICK_SYNC *pSync)
{
	if (!pKick || !pSync)
		return FALSE;
	if (!m_pcTong)
		return FALSE;

	// 寻找帮会
	for (int i = 0; i < m_nTongPointSize; ++i)
	{
		if (m_pcTong[i] && m_pcTong[i]->m_dwNameID == pKick->m_dwTongNameID)
		{
			return m_pcTong[i]->Kick(pKick, pSync);
		}
	}

	return FALSE;
}
//离开帮会
BOOL	CTongSet::Leave(STONG_LEAVE_COMMAND *pLeave, STONG_LEAVE_SYNC *pSync)
{
	if (!pLeave || !pSync)
		return FALSE;
	if (!m_pcTong)
		return FALSE;

	// 寻找帮会
	for (int i = 0; i < m_nTongPointSize; ++i)
	{
		if (m_pcTong[i] && m_pcTong[i]->m_dwNameID == pLeave->m_dwTongNameID)
		{
			return m_pcTong[i]->Leave(pLeave, pSync);
		}
	}

	return FALSE;
}

BOOL	CTongSet::AcceptMaster(STONG_ACCEPT_MASTER_COMMAND *pAccept)
{
	if (!pAccept)
		return FALSE;
	if (!m_pcTong)
		return FALSE;

	// 寻找帮会
	for (int i = 0; i < m_nTongPointSize; ++i)
	{
		if (m_pcTong[i] && m_pcTong[i]->m_dwNameID == pAccept->m_dwTongNameID)
		{
			return m_pcTong[i]->AcceptMaster(pAccept);
		}
	}

	return FALSE;
}

BOOL CTongSet::InitFromDB()
{
	int	nTongNum = g_cTongDB.GetTongCount();	//帮会数量
	if (nTongNum <= 0)
		return TRUE;

	rTRACE("------------ Tong Count: %d-------------",nTongNum);

	//CTongControl**	m_tmpTong = (CTongControl**)new LPVOID[nTongNum];

	//for (int i = 0; i < nTongNum; ++i)
	//{
		//m_tmpTong[i] = NULL;
	//}

	TTongList	*pList = new TTongList[nTongNum];
	memset(pList, 0, sizeof(TTongList) * nTongNum);
	int nGetNum = g_cTongDB.GetTongList(pList, nTongNum); //有多少个帮会
	if (nGetNum <= 0)
		return TRUE;
	//rTRACE("------------ nGetNum %d 个 -------------",nGetNum);

	//for (int i = 0; i < nGetNum; ++i)
	//{
	//	m_tmpTong[i] = new CTongControl(pList[i]);
	//	if (!m_tmpTong[i]->m_szName[0])
	//	{
	//		delete m_tmpTong[i];
	//		m_tmpTong[i] = NULL;
	//		continue;
	//	}

	//	rTRACE("------------ 第 %d 个(%s) 人数:%d 帮主名:%s -------------",i,m_tmpTong[i]->m_szName,pList[i].MemberCount,pList[i].m_szMasterName);
	//}

	//Init();

	g_cTongDB.Close();

	g_cTongDB.OpenNew();


	//for (i = 0; i < nGetNum; ++i)
	//{
	//	if (m_tmpTong[i] && m_tmpTong[i]->m_szName[0])
	//	{
	//		Create(m_tmpTong[i]->m_nCamp,m_tmpTong[i]->m_szMasterName,m_tmpTong[i]->m_szName);
	//		for (int j = 0; j < m_tmpTong[i]->m_nMemberPointSize;++j)
	//		{
	//			AddMember(m_tmpTong[i]->m_psMember[j].m_szName,m_tmpTong[i]->m_szName);
	//		}
	//	}
	//}
	for (int i = 0; i < nGetNum; ++i)
	{
		m_pcTong[i] = new CTongControl(pList[i]);
		if (!m_pcTong[i]->m_szName[0])
		{
			delete m_pcTong[i];
			m_pcTong[i] = NULL;
			continue;
		}
		rTRACE("------------ Loaded Tong - %d: %s, Member count: %d, Master name: %s -------------", i, m_pcTong[i]->m_szName, pList[i].MemberCount, pList[i].m_szMasterName);
	}
	for (int i = 0; i < nGetNum; ++i)
	{
		if (m_pcTong[i] && m_pcTong[i]->m_szName[0])
		{
			g_cTongDB.SearchTong(m_pcTong[i]->m_szName, *(m_pcTong[i])); //设置帮会基本信息
			for (int j = 0;j < m_pcTong[i]->m_nManagerNum;++j)
			{//队长
				m_pcTong[i]->DBInstate(m_pcTong[i]->m_szManagerName[j],1);
			}

			for (int j = 0;j < m_pcTong[i]->m_nDirectorNum;++j)
			{//长老
				m_pcTong[i]->DBInstate(m_pcTong[i]->m_szDirectorName[j],2);
			}
		}
	}
	//if (m_tmpTong)
	//{
	//	delete [] m_tmpTong;
	//	m_tmpTong=NULL;
	//}
	if (pList)
	{
		delete [] pList;
          pList=NULL;
	}

	return TRUE;
}

BOOL	CTongSet::GetLoginData(STONG_GET_LOGIN_DATA_COMMAND *pLogin, STONG_LOGIN_DATA_SYNC *pSync)
{
	if (!pLogin || !pSync)
		return FALSE;

	memset(pSync, 0, sizeof(STONG_LOGIN_DATA_SYNC));
	pSync->ProtocolFamily	= pf_tong;
	pSync->ProtocolID		= enumS2C_TONG_LOGIN_DATA;
	pSync->m_btFlag			= 0;
	pSync->m_dwParam		= pLogin->m_dwParam;

	if (!m_pcTong)
		return FALSE;

	// 寻找帮会
	for (int i = 0; i < m_nTongPointSize; ++i)
	{
		if (m_pcTong[i] && m_pcTong[i]->m_dwNameID == pLogin->m_dwTongNameID)
		{
			return m_pcTong[i]->GetLoginData(pLogin, pSync);
		}
	}

	return TRUE;
}
//查找一个帮会
BOOL	CTongSet::SearchOne(DWORD dwTongNameID, char *lpszName, STONG_ONE_LEADER_INFO *pInfo)
{
	if (!m_pcTong)
		return FALSE;
	if (dwTongNameID == 0 || !lpszName || !lpszName[0] || !pInfo)
		return FALSE;

	for (int i = 0; i < m_nTongPointSize; ++i)
	{
		if (m_pcTong[i] && m_pcTong[i]->m_dwNameID == dwTongNameID)
		{
			return m_pcTong[i]->SearchOne(lpszName, pInfo);
		}
	}

	return FALSE;
}

BOOL CTongSet::ChangeMoney( STONG_MONEY_COMMAND *pMoney,STONG_MONEY_SYNC *Sync)
{
	if (!pMoney)
		return FALSE;
	if (!m_pcTong)
		return FALSE;
	
	// 寻找帮会
	for (int i = 0; i < m_nTongPointSize; ++i)
	{
		if (m_pcTong[i] && m_pcTong[i]->m_dwNameID == pMoney->m_dwTongNameID)
		{//这里是相同的
			return m_pcTong[i]->ChangeMoneyControl(pMoney,Sync);
		}
	}
	
	return FALSE;
}

//获取所有帮会的信息
BOOL CTongSet::GetTongListInfo(STONG_GET_LIST_INFO_COMMAND *pApply, STONG_LIST_INFO_SYNC *pInfo)   //帮众信息
{//分段发送数据的
	rTRACE("------------ GetTongListInfo da chay quaaaaa ");
	if (!m_pcTong||!pApply || !pInfo)
	{
	rTRACE("------------ GetTongListInfo da chay quaaaaa 1 ");
		return FALSE;
	}

	if (pApply->m_nParam3 <= 0)
	{
		rTRACE("------------ GetTongListInfo da chay quaaaaa 2 m_nParam: %d-%d-%d",pApply->m_nParam1,pApply->m_nParam2,pApply->m_nParam3);
		return FALSE;
	}

	if (m_nTongPointSize <= 0)
	{
	rTRACE("------------ GetTongListInfo da chay quaaaaa 3 ");
		return FALSE;
	}

	rTRACE("------------ GetTongListInfo da chay quaaa 4 ");

	pInfo->ProtocolFamily	= pf_alltong;
	pInfo->ProtocolID		= enumS2C_TONG_ALLLIST_INFO;
	pInfo->m_dwParam		= pApply->m_dwParam;
	pInfo->m_btStartNo		= pApply->m_nParam2; //原来的开始的位置
	
	int	nNeedNum, nStartNum, i, j;
	
	nStartNum = pApply->m_nParam2;   //开始的位置
	nNeedNum  = pApply->m_nParam3;   //请求的数量
	
	if (nNeedNum > defTONG_ONE_PAGE_MAX_NUM)     //最大请求数量
		nNeedNum = defTONG_ONE_PAGE_MAX_NUM;     //最大请求数量
	
	pInfo->m_btCurNum = 0;
	
	for (i = 0, j = 0; i < m_nTongPointSize; ++i)
	{//过滤掉 请求位置前面的内容
		if (j >= nStartNum)
			break;
		if (m_pcTong[i] && m_pcTong[i]->m_dwNameID)
			++j;  //有多少个已经是已经是获取了数据的
	}

	if (i >= m_nTongPointSize || j < nStartNum)  //如果超过了数量 或 数据比所请求的数量少的话 就 终止
		return FALSE;
	
	// 开始记录所请求从开始位置后的数据
	for (; i < m_nTongPointSize; ++i)
	{
		if (pInfo->m_btCurNum >= nNeedNum)
			break; //当前的数量 大于 请求的数量 则跳出
		if (m_pcTong[i] && m_pcTong[i]->m_dwNameID)
		{
        //记录从开始位置起 后的数据
		   snprintf(pInfo->m_sTongList[pInfo->m_btCurNum].m_szName, sizeof(pInfo->m_sTongList[pInfo->m_btCurNum].m_szName), "%s", m_pcTong[i]->m_szName);
		   pInfo->m_btCurNum++;
		}
	}
	if (pInfo->m_btCurNum == 0)
		return FALSE;
	
	pInfo->m_wLength = sizeof(STONG_LIST_INFO_SYNC) - sizeof(STONG_ONE_MEMBER_INFO) * (defTONG_ONE_PAGE_MAX_NUM - pInfo->m_btCurNum);
	
	return TRUE;
}

BOOL CTongSet::SetPower(STONG_POWER_COMMAND* pPower, STONG_POWER_SYNC* pSync)
{
	if (!pPower || !pSync)
		return FALSE;
	if (!m_pcTong)
		return FALSE;

	for (int i = 0; i < m_nTongPointSize; ++i)
	{
		if (m_pcTong[i] && m_pcTong[i]->m_dwNameID == pPower->m_dwTongNameID)
		{
			return m_pcTong[i]->SetPower(pPower, pSync);
		}
	}
	return FALSE;
}

BOOL	CTongSet::GetPower(STONG_GET_POWER_COMMAND* pPower, STONG_POWER_SYNC* pSync)
{
	if (!pPower || !pSync)
		return FALSE;
	if (!m_pcTong)
		return FALSE;

	for (int i = 0; i < m_nTongPointSize; ++i)
	{
		if (m_pcTong[i] && m_pcTong[i]->m_dwNameID == pPower->m_dwTongNameID)
		{
			return m_pcTong[i]->GetPower(pPower, pSync);
		}
	}
	return FALSE;
}


BOOL CTongSet::UpdateLevel(STONG_UPDATE_LEVEL_COMMAND* pUpdate)
{
	if (!pUpdate)
		return FALSE;

	if (!m_pcTong)
		return FALSE;

	for (int i = 0; i < m_nTongPointSize; ++i)
	{
		if (m_pcTong[i] && m_pcTong[i]->m_dwNameID == pUpdate->m_dwTongNameID)
		{
			return m_pcTong[i]->UpdateLevel(pUpdate);
		}
	}
}

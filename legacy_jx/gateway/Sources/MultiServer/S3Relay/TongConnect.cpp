// TongConnect.cpp: implementation of the CTongConnect class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TongConnect.h"
#include "TongServer.h"
#include "S3Relay.h"
#include "Global.h"
#include "malloc.h"
#include <cmath>



//-------------------------- tong struct size ---------------------------
int	g_nTongPSSize[defTONG_PROTOCOL_SERVER_NUM] = 
{
	-1,											// enumC2S_TONG_CREATE
	-1,											// enumC2S_TONG_ADD_MEMBER
	sizeof(STONG_GET_TONG_HEAD_INFO_COMMAND),	// enumC2S_TONG_GET_HEAD_INFO
	sizeof(STONG_GET_MANAGER_INFO_COMMAND),		// enumC2S_TONG_GET_MANAGER_INFO
	sizeof(STONG_GET_MEMBER_INFO_COMMAND),		// enumC2S_TONG_GET_MEMBER_INFO
	sizeof(STONG_SEARCH_INFO_COMMAND),
	sizeof(STONG_INSTATE_COMMAND),				// enumC2S_TONG_INSTATE
	sizeof(STONG_KICK_COMMAND),					// enumC2S_TONG_KICK
	sizeof(STONG_LEAVE_COMMAND),				// enumC2S_TONG_LEAVE
	sizeof(STONG_CHANGE_MASTER_COMMAND),		// enumC2S_TONG_CHANGE_MASTER
	sizeof(STONG_ACCEPT_MASTER_COMMAND),		// enumC2S_TONG_ACCEPT_MASTER
	sizeof(STONG_GET_LOGIN_DATA_COMMAND),		// enumC2S_TONG_GET_LOGIN_DATA
	sizeof(STONG_MONEY_COMMAND),				// enumC2S_TONG_MONEY_SAVE
	sizeof(STONG_MONEY_COMMAND),				// enumC2S_TONG_MONEY_GET
	sizeof(STONG_MONEY_COMMAND),				// enumC2S_TONG_MONEY_SND
	sizeof(STONG_POWER_COMMAND),				// enumC2S_TONG_SET_POWER
	sizeof(STONG_GET_POWER_COMMAND),			// enumC2S_TONG_GET_POWER
	-1,											// enumC2S_TONG_MORE_INFO
	sizeof(STONG_UPDATE_LEVEL_COMMAND),			// enumC2S_TONG_UPDATE_LEVEL
};

//------------------------- tong struct size end ---------------------------


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTongConnect::CTongConnect(CTongServer* pTongServer, unsigned long id)
	: CNetConnect(pTongServer, id)
{
	 //gGs=this;
	 nTondNetIdx = id;
     nSerVerIndex = 1;

}

CTongConnect::~CTongConnect()
{
	 nSerVerIndex = 1;
}


void CTongConnect::OnClientConnectCreate()
{
	rTRACE("tong connect create: %08X",GetIP());// _ip2a(GetIP())
}

void CTongConnect::OnClientConnectClose()
{
	rTRACE("tong connect close: %08X", GetIP());
	//g_ChannelMgr.B_ClearPlayer_All(GetIP());
}

//接受GS数据包 线程
void CTongConnect::RecvPackage(const void* pData, size_t size)
{

//	isRunning = TRUE;

	EXTEND_HEADER* pHeader = (EXTEND_HEADER*)pData;

	if (pHeader->ProtocolFamily == pf_tong)
	{
 /*       char	szPlayerName[32], szTongName[32];
		STONG_CREATE_COMMAND	*pTongCreate = (STONG_CREATE_COMMAND*)pData;
		int  nCamp       = pTongCreate->m_btCamp;//&pTongCreate->m_szBuffer[0]
		
		memcpy(szPlayerName, &pTongCreate->m_szBuffer[0],pTongCreate->m_btPlayerNameLength);
		szTongName[pTongCreate->m_btTongNameLength] = 0;  //加个结束符号
		memcpy(szTongName, &pTongCreate->m_szBuffer[pTongCreate->m_btPlayerNameLength], pTongCreate->m_btTongNameLength);
			szPlayerName[pTongCreate->m_btPlayerNameLength] = 0;
*/		    
			int nReg=Proc0_Tong(pData, size);
		    rTRACE("------------ pf_tong    ----------- OK..:(%d) pData: %d size: %d",nReg,pData,size);
	}
	else if (pHeader->ProtocolFamily == pf_friend)
	{
		    Proc0_Friend(pData, size);
	        rTRACE("------------ pf_friend ------------ OK..: %08X pData: %d size: %d", GetIP(),pData,size);
	}
	else if (pHeader->ProtocolFamily == pf_alltong)
	{
		   Proc0_allTong(pData, size);
		   rTRACE("------------- pf_alltong ----------- OK..:  pData: %d size: %d",pData,size);
	}
	else if (pHeader->ProtocolFamily == pf_udataserveridx)
	{//更新当前服务器的索引
		Proc0_ServerCommunity(pData, size);
	}
	else
	{
		EXTEND_HEADER* pHeader = (EXTEND_HEADER*)pData;
		if (pHeader->ProtocolFamily == pf_normal)
		{
			Proc0_Normal(pData, size);
		}
	}
}

BOOL CTongConnect::PassToSomeone(DWORD ip, unsigned long id, DWORD nameid, const void* pData, size_t size)
{
	CNetConnectDup conndup;
	if (GetID() == ip)
		conndup = *this;
	else
	{
		conndup = g_TongServer.FindTongConnectByIP(ip);
		if (!conndup.IsValid())
			return FALSE;
	}

	size_t pckgsize = sizeof(EXTEND_PASSTOSOMEONE) + size;
	EXTEND_PASSTOSOMEONE* pEps = (EXTEND_PASSTOSOMEONE*)_alloca(pckgsize);
	pEps->ProtocolFamily = pf_extend;
	pEps->ProtocolID = extend_s2c_passtosomeone;
	pEps->nameid = nameid;
	pEps->lnID = id;
	pEps->datasize = size;
	memcpy(pEps + 1, pData, size);

	conndup.SendPackage(pEps, pckgsize);

	return TRUE;
}


void CTongConnect::proc_AllPlayer()
{//可以执行脚本 或 发送其他 包给GS ---客户端
 /*
	nS3loop++;
	//rTRACE("------S3循环-------: %d",nS3loop);
	if (nS3loop>18 && nS3loop%18==0)
	{
		try
		{
			g_cTongSet.SynTongWarInfo();		
			g_cTongSet.SynWarCityInfo();  //同步七城信息
			g_cTongSet.SynAttackInfo();
			rTRACE("--------------同步帮战七城,帮战信息数据成功----------------");
		}
		catch (...)
		{
			rTRACE("--------------同步帮战七城信息数据异常----------------");
		}
	}*/	
}

//帮会宣战循环 接收GS 发过来的信息
int CTongConnect::Proc0_allTong(const void* pData, size_t size)
{
	if (!pData)
		return FALSE;

	EXTEND_HEADER* pHeader = (EXTEND_HEADER*)pData;
	
	if (pHeader->ProtocolID >= enumC2S_TONG_NUM)  //  协议的数量
		return FALSE;

	switch (pHeader->ProtocolID)
	{
	case enumC2S_TONG_ATTACK:
		{//更新宣战时间	
			int nBlueTongIdx=-1,nGreenTongIdx=-1;

		    int nTongIdx=g_cTongSet.UpTongAttackTime(&nBlueTongIdx,&nGreenTongIdx);

			if(nTongIdx>=0)
			{//宣战结束
				char nTonName[NAME_LENGHT];
				if (g_cTongSet.GetTongName(nTongIdx,nTonName))
				{//获取信息发回		GS
					STONG_ATTACK_SENDBACK	sInfo;
					sInfo.ProtocolFamily = pf_alltong;
					sInfo.ProtocolID     = enumS2C_TONG_ATTACK_OVER;
					
					if (g_cTongSet.GetTongMapByIdx(nTongIdx))
					   sInfo.m_dwParam      = 1;
					else
					   sInfo.m_dwParam      = 0;

					   sInfo.m_dwNpcID      = 0;
					snprintf(sInfo.m_szAttackName, sizeof(sInfo.m_szAttackName), "%s", nTonName);
					sInfo.m_wLength = sizeof(STONG_ATTACK_SENDBACK);
					this->SendPackage((const void *)&sInfo, sInfo.m_wLength);			
				}
			}
		}
		break;
	case enumC2S_TONG_INFO_SET:
		{
			STONG_SET_TONG_INFO_COMMAND	 *pSend = (STONG_SET_TONG_INFO_COMMAND*)pData;
			switch (pSend->NextProtocolID)
			{
			case enumC2S_TONG_RANDOM_MAP:
				{//随机设置帮会地图
					if (g_cTongSet.RandomSelTongmap()==-1)
					{//设置不成功!....
						//rTRACE("--------------成功----------------");
						
					}
				}
				break;
			case  enumC2S_TONG_SET_ATTACKWIN:
			{//设置胜利
				if (g_cTongSet.SetTongWinNum(pSend->m_dwTongNameID,pSend->m_dwParam))
				{
						
				}	
			}
			break;
			case  enumC2S_TONG_SET_ATTACKLOSE:
			{//设置失败
				if (g_cTongSet.SetTongLoseNum(pSend->m_dwTongNameID,pSend->m_dwParam))
				{
					
				}
			}
			break;
			case enumC2S_TONG_SET_DEATHCOUNT:	
			{//设置死亡次数
				if (g_cTongSet.SetTongKillNum(pSend->m_dwTongNameID,pSend->m_dwParam))
				{
					
				}
			}
				break;
			case enumC2S_TONG_SET_LEVEL:
				{//设置等级
					if (g_cTongSet.SetTongLevel(pSend->m_dwTongNameID,pSend->m_dwParam))
					{

					}
				}
				break;
			case enumC2S_TONG_SET_MAP:
				{//设置帮会地图
					if (g_cTongSet.SetTongMap(pSend->m_dwTongNameID,pSend->m_dwParam))
					{
						
					}
				}
				break;
			case enumC2S_TONG_SET_CREDIT:	  //Credit
				{//设置建设资金
					if (g_cTongSet.SetTongCredit(pSend->m_dwTongNameID,pSend->m_dwParam))
					{
						
					}
				}
				break;
			case enumC2S_TONG_SET_BANGTITLE:
				{//设置帮主称号
					if (g_cTongSet.SetTongMasterTitle(pSend->m_dwTongNameID, pSend->m_title))
					{
						DWORD nameid = 0;
						unsigned long param = 0;
						CNetConnectDup conndup;
						if (g_TongServer.FindPlayerByRole(NULL, std::_tstring(pSend->m_szName), &conndup, NULL, &nameid, &param))
						{
							CNetConnectDup tongconndup = g_TongServer.FindTongConnectByIP(conndup.GetIP());
							if (tongconndup.IsValid())
							{
								STONG_CHANGE_TITLE_SYNC	sChange;

								sChange.ProtocolFamily = pf_alltong;
								sChange.ProtocolID = enumS2C_TONG_CHANGE_TITLE;
								sChange.m_dwParam = param;
								sChange.m_dwTongNameID = pSend->m_dwTongNameID;
								strcpy(sChange.m_szTitle, pSend->m_title);

								tongconndup.SendPackage((const void*)&sChange, sizeof(sChange));
							}
						}
					}
				}
				break;
			case enumC2S_TONG_SET_ZHANGTITLE:
				{//设置长老称号
					if (g_cTongSet.SetTongDirectorTitle(pSend->m_dwTongNameID,pSend->m_title, pSend->m_dwParam))
					{
						DWORD nameid = 0;
						unsigned long param = 0;
						CNetConnectDup conndup;
						if (g_TongServer.FindPlayerByRole(NULL, std::_tstring(pSend->m_szName), &conndup, NULL, &nameid, &param))
						{
							CNetConnectDup tongconndup = g_TongServer.FindTongConnectByIP(conndup.GetIP());
							if (tongconndup.IsValid())
							{
								STONG_CHANGE_TITLE_SYNC	sChange;

								sChange.ProtocolFamily = pf_alltong;
								sChange.ProtocolID = enumS2C_TONG_CHANGE_TITLE;
								sChange.m_dwParam = param;
								sChange.m_dwTongNameID = pSend->m_dwTongNameID;
								strcpy(sChange.m_szTitle, pSend->m_title);

								tongconndup.SendPackage((const void*)&sChange, sizeof(sChange));
							}
						}
					}
				}
				break;
			case enumC2S_TONG_SET_DUITITLE:	
				{//设置队长称号
					if (g_cTongSet.SetTongManagerTitle(pSend->m_dwTongNameID,pSend->m_title, pSend->m_dwParam))
					{
						DWORD nameid = 0;
						unsigned long param = 0;
						CNetConnectDup conndup;
						if (g_TongServer.FindPlayerByRole(NULL, std::_tstring(pSend->m_szName), &conndup, NULL, &nameid, &param))
						{
							CNetConnectDup tongconndup = g_TongServer.FindTongConnectByIP(conndup.GetIP());
							if (tongconndup.IsValid())
							{
								STONG_CHANGE_TITLE_SYNC	sChange;

								sChange.ProtocolFamily = pf_alltong;
								sChange.ProtocolID = enumS2C_TONG_CHANGE_TITLE;
								sChange.m_dwParam = param;
								sChange.m_dwTongNameID = pSend->m_dwTongNameID;
								strcpy(sChange.m_szTitle, pSend->m_title);

								tongconndup.SendPackage((const void*)&sChange, sizeof(sChange));
							}
						}
					}
				}
				break;
			case enumC2S_TONG_SET_ZONGTITLE:	
				{//设置帮众称号
					if (g_cTongSet.SetTongNormalTitle(pSend->m_dwTongNameID,pSend->m_title, pSend->m_dwParam))
					{
						
					}
				}
				break;
			case enumC2S_TONG_SET_CAM:
				{
					if (g_cTongSet.SetTongCam(pSend->m_dwTongNameID, pSend->m_dwParam))
					{

					}
				}
				break;
			default:
				break;
			} 
		}	
	break;
	case enumC2S_TONG_ATTACK_SEND_INFO:
		{
			STONG_GET_TONG_ATTACK_INFO_COMMAND	*pSend = (STONG_GET_TONG_ATTACK_INFO_COMMAND*)pData;
			STONG_ATTACK_SENDBACK	sInfo;
			sInfo.ProtocolFamily = pf_alltong;
			sInfo.ProtocolID     = enumS2C_TONG_ATTACK_SENDBACK;	
	        
			if (g_cTongSet.SetTongAttackInfo(pSend->m_dwTongNameID,pSend->m_dwParam))
			{//获取信息发回		GS
				sInfo.m_dwNpcID = pSend->m_dwNpcID;
				sInfo.m_dwParam = 1;				
			}
			else
			{//设置不成功
				sInfo.m_dwNpcID = pSend->m_dwNpcID;
				sInfo.m_dwParam = 0;
			}
			char nYTonName[NAME_LENGHT],nDTonName[NAME_LENGHT];

			if (g_cTongSet.GetTongNameById(pSend->m_dwTongNameID,nYTonName))
			{
				snprintf(sInfo.m_szAttackName, sizeof(sInfo.m_szAttackName), "%s", nYTonName);
			}

			if (g_cTongSet.GetTongNameById(pSend->m_dwParam,nDTonName))
			{
				snprintf(sInfo.m_szDAttackName, sizeof(sInfo.m_szDAttackName), "%s", nDTonName);

			}

			sInfo.m_wLength = sizeof(STONG_ATTACK_SENDBACK);
			this->SendPackage((const void *)&sInfo, sInfo.m_wLength);
		} 
		break;

	case enumC2S_TONG_ATTACK_GET_INFO:
	{
	/*	STONG_GET_TONG_ATTACK_INFO_COMMAND	*pGet = (STONG_GET_TONG_ATTACK_INFO_COMMAND*)pData;
		STONG_ATTACK_INFO_SYNC	sInfo;
		sInfo.ProtocolFamily = pf_alltong;
	    sInfo.ProtocolID     = enumS2C_TONG_ATTACK_GETBACK;
		if (!g_cTongSet.GetTongAttackInfo(pGet->m_dwTongNameID, &sInfo))
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
		 
		 sInfo.m_dwParam = pGet->m_dwParam;
		 sInfo.m_dwNpcID = pGet->m_dwNpcID;
			 sInfo.m_WarCityCount=gGetPrivateProfileIntEx("AreaCount","count",file_warcfg,0);
		 if (sInfo.m_WarCityCount>7)
			 sInfo.m_WarCityCount=7;
		 std::_tstring strVal;
		
	     for (int i=0;i<sInfo.m_WarCityCount;i++)
		 {//城战信息
			char szSecton[32];
			 sprintf(szSecton,"AreaName0%d",i);

		   sInfo.snWarInfo[i].m_idx=gGetPrivateProfileIntEx(szSecton,key_idx,file_warcfg,0);
		   sInfo.snWarInfo[i].m_mapidx=gGetPrivateProfileIntEx(szSecton,key_mapidx,file_warcfg,0);
		      strVal = gGetPrivateProfileStringEx(szSecton, key_name, file_warcfg);
		   sprintf(sInfo.snWarInfo[i].m_szMapName,strVal.c_str());
		      strVal = gGetPrivateProfileStringEx(szSecton, key_tonname, file_warcfg);
		   sprintf(sInfo.snWarInfo[i].m_Tongmaster,strVal.c_str());
			  strVal = gGetPrivateProfileStringEx(szSecton, key_tongmaster, file_warcfg);
		    sprintf(sInfo.snWarInfo[i].m_Mastername,strVal.c_str());
		   sInfo.snWarInfo[i].m_levle=gGetPrivateProfileIntEx(szSecton,key_level,file_warcfg,0);
		   sInfo.snWarInfo[i].m_shushou=gGetPrivateProfileIntEx(szSecton,key_shuis,file_warcfg,0);
		 } 

		 sInfo.m_wLength=sizeof(STONG_ATTACK_INFO_SYNC);
		 this->SendPackage((const void *)&sInfo, sInfo.m_wLength);*/			
	//	}
	} 
	break;
	case enumC2S_TONG_GET_LIST_INFO:
		{//-->GS 帮会列表
			STONG_GET_LIST_INFO_COMMAND	*pGet = (STONG_GET_LIST_INFO_COMMAND*)pData;
			STONG_LIST_INFO_SYNC	sInfo;
			//rTRACE("------------- enumC2S_TONG_GET_LIST_INFO ----------- OK:  m_nParam: %d-%d-%d",pGet->m_nParam1,pGet->m_nParam2,pGet->m_nParam3);
			if (g_cTongSet.GetTongListInfo(pGet, &sInfo))
			{
				sInfo.m_TongCount=g_cTongSet.GetTongCount();

				this->SendPackage((const void *)&sInfo, sInfo.m_wLength);
			 //rTRACE("------------- enumC2S_TONG_GET_LIST_INFO da send ----------- OK..");
			}
		}
	break;
	case enumC2S_TONG_CHANGE_ANNOUNCE:
	{
		STONG_CHANGE_ANNOUNCE_COMMAND* pChange = (STONG_CHANGE_ANNOUNCE_COMMAND*)pData;
		if (g_cTongSet.ChangeAnnounce(pChange->m_dwTongNameID, pChange->m_szAnnounce))
		{

		}
	}
	break;
	default:
		break;
	}
  return TRUE;
}
//-----------------------------------------------------------------------
//	功能：帮会协议处理，收到 game server 发来的与帮会有关的协议	 功能入口
//-----------------------------------------------------------------------
int CTongConnect::Proc0_Tong(const void* pData, size_t size)
{
	if (!pData)
		return 1;

	// 协议长度检测
	if (size < sizeof(EXTEND_HEADER))
		return 2;

	EXTEND_HEADER* pHeader = (EXTEND_HEADER*)pData;

	if (pHeader->ProtocolID >= enumC2S_TONG_NUM)  //  协议的数量
		return 3;
	if (g_nTongPSSize[pHeader->ProtocolID] < 0)
	{
		if (size <= sizeof(EXTEND_HEADER) + 2)
			return 4;
		WORD	wLength = *((WORD*)((BYTE*)pData + sizeof(EXTEND_HEADER)));
		if (wLength != size)
			return 5;
	}
	else if (g_nTongPSSize[pHeader->ProtocolID] != size)
	{
		return 6;
	}

	switch (pHeader->ProtocolID)
	{
	case enumC2S_TONG_CREATE:
		{
			char	szPlayerName[NAME_LENGHT], szTongName[NAME_LENGHT];
			STONG_CREATE_COMMAND	*pTongCreate = (STONG_CREATE_COMMAND*)pData;

			//rTRACE("DEBUG, pTongCreate->m_blevel: %d", pTongCreate->m_blevel);


			int		nCamp = pTongCreate->m_btCamp;//&pTongCreate->m_szBuffer[0]
			int nPlayerLevel = pTongCreate->m_blevel;

			// two BYTEs straight off the wire, each used as both a memcpy length and an
			// index into a NAME_LENGHT array - a 200 in either one runs 140 bytes past the
			// stack.  The terminator that was written before its own copy is written after
			// it now, so the two agree on the byte count that bounds them.
			int nPlayerNameLen = pTongCreate->m_btPlayerNameLength;
			int nTongNameLen = pTongCreate->m_btTongNameLength;
			// these two lengths are declared int (KTongProtocol.h:284-285), not the BYTE the m_bt
			// prefix promises: a negative one walks through the width test below and reaches
			// memcpy as a size_t of 18446744073709551615.  The add-member branch cannot carry a
			// negative length because its fields really are BYTEs; this one has to say it
			if (nPlayerNameLen < 0 || nTongNameLen < 0)
				return 6;
			if (nPlayerNameLen >= NAME_LENGHT || nTongNameLen >= NAME_LENGHT)
				return 6;
			if (nPlayerNameLen + nTongNameLen >= NAME_LENGHT)
				return 6;
			memcpy(szPlayerName, &pTongCreate->m_szBuffer[0], nPlayerNameLen);
			szPlayerName[nPlayerNameLen] = 0;
			memcpy(szTongName, &pTongCreate->m_szBuffer[nPlayerNameLen], nTongNameLen);
			szTongName[nTongNameLen] = 0;
			// 建立帮会
			int nSuccessID = g_cTongSet.Create(nCamp, szPlayerName, szTongName, nPlayerLevel);
			if (nSuccessID == 0)
			{
				STONG_CREATE_SUCCESS_SYNC	sSync;  //同步帮会信息结构发送给GS
				sSync.ProtocolFamily = pf_tong;
				sSync.ProtocolID = enumS2C_TONG_CREATE_SUCCESS;
				sSync.m_dwPlayerNameID = pTongCreate->m_dwPlayerNameID;//创建者名字 filename2id
				sSync.m_dwParam = pTongCreate->m_dwParam; //player index
				sSync.m_btCamp = pTongCreate->m_btCamp;
				g_cTongSet.GetMasterTitle(szTongName, sSync.m_szTitle);
				//rTRACE("******DEBUG, title: %s", sSync.m_szTitle);
				sSync.m_btTongNameLength = nTongNameLen;
				memcpy(sSync.m_szTongName, &pTongCreate->m_szBuffer[nPlayerNameLen], nTongNameLen);
				sSync.m_wLength = sizeof(STONG_CREATE_SUCCESS_SYNC) - sizeof(sSync.m_szTongName) + sSync.m_btTongNameLength;
			     this->SendPackage((const void *)&sSync, sSync.m_wLength);
			    return 0;
			}
			// 建立失败
			else
			{
				STONG_CREATE_FAIL_SYNC	sSync;
				sSync.ProtocolFamily = pf_tong;
				sSync.ProtocolID = enumS2C_TONG_CREATE_FAIL;
				sSync.m_dwPlayerNameID = pTongCreate->m_dwPlayerNameID;
				sSync.m_dwParam = pTongCreate->m_dwParam;
				sSync.m_btFailID = nSuccessID;
				this->SendPackage((const void *)&sSync, sizeof(STONG_CREATE_FAIL_SYNC));
			    return 7;
			}
		}
		break;

	// 添加成员
	case enumC2S_TONG_ADD_MEMBER:
		{
			char	szPlayerName[64], szTongName[64];
			STONG_ADD_MEMBER_COMMAND	*pAdd = (STONG_ADD_MEMBER_COMMAND*)pData;

			//rTRACE("DEBUG, pAdd->m_nLevel: %d", pAdd->m_nLevel);

			int nTongNameLen = pAdd->m_btTongNameLength;
			int nPlayerNameLen = pAdd->m_btPlayerNameLength;
			// the same two BYTE lengths as the create command above, against the same 64
			if (nTongNameLen >= 64 || nPlayerNameLen >= 64)
				return 6;
			if (nTongNameLen + nPlayerNameLen >= 64)
				return 6;
			memcpy(szTongName, &pAdd->m_szBuffer[0], nTongNameLen);
			szTongName[nTongNameLen] = 0;
			
			memcpy(szPlayerName, &pAdd->m_szBuffer[nTongNameLen], nPlayerNameLen);
			szPlayerName[nPlayerNameLen] = 0;

			int nRet = g_cTongSet.AddMember(szPlayerName, szTongName, pAdd->m_nLevel);
			
			// 成员添加成功
			if (nRet >= 0)
			{
				STONG_ADD_MEMBER_SUCCESS_SYNC	sSync;
				sSync.ProtocolFamily = pf_tong;
				sSync.ProtocolID = enumS2C_TONG_ADD_MEMBER_SUCCESS;
				sSync.m_dwPlayerNameID = pAdd->m_dwPlayerNameID;
				sSync.m_dwParam = pAdd->m_dwParam;
				sSync.m_btCamp = g_cTongSet.GetTongCamp(nRet);
				g_cTongSet.GetMasterName(nRet, sSync.m_szMasterName);
				strcpy(sSync.m_szTongName, szTongName);
				g_cTongSet.GetMemberTitle(nRet, sSync.m_szTitle, pAdd->m_gender);

				this->SendPackage((const void *)&sSync, sizeof(sSync));
		
			}
			// 成员添加不成功
			else
			{
				STONG_ADD_MEMBER_FAIL_SYNC	sSync;
				sSync.ProtocolFamily = pf_tong;
				sSync.ProtocolID = enumS2C_TONG_ADD_MEMBER_FAIL;
				sSync.m_dwPlayerNameID = pAdd->m_dwPlayerNameID;
				sSync.m_dwParam = pAdd->m_dwParam;
				sSync.m_btFailID = nRet;

				this->SendPackage((const void *)&sSync, sizeof(STONG_ADD_MEMBER_FAIL_SYNC));
			}
		}
		break;
	//接受 GS转发过来的 客户端请求信息 ---获得帮会信息
	case enumC2S_TONG_GET_HEAD_INFO:
		{//获取帮会的 帮主 长老 等消息
			STONG_GET_TONG_HEAD_INFO_COMMAND	*pGet = (STONG_GET_TONG_HEAD_INFO_COMMAND*)pData;
			STONG_HEAD_INFO_SYNC	sInfo;
			if (g_cTongSet.GetTongHeadInfo(pGet->m_dwTongNameID, &sInfo))
			{//获取信息发回		GS
				sInfo.m_dwParam = pGet->m_dwParam;
				//sInfo.m_dwNpcID = pGet->m_dwNpcID;
				this->SendPackage((const void *)&sInfo, sInfo.m_wLength);
			}
		}
		break;
	case enumC2S_TONG_GET_MANAGER_INFO:
		{//队长信息
			STONG_GET_MANAGER_INFO_COMMAND	*pGet = (STONG_GET_MANAGER_INFO_COMMAND*)pData;
			STONG_MANAGER_INFO_SYNC	sInfo;
			ZeroMemory(&sInfo, sizeof(sInfo));
			if (g_cTongSet.GetTongManagerInfo(pGet, &sInfo))
			{	//-->GS
				this->SendPackage((const void *)&sInfo, sInfo.m_wLength);  
			}
		}
		break;
	case enumC2S_TONG_GET_MEMBER_INFO:
		{//-->GS 帮众信息
			STONG_GET_MEMBER_INFO_COMMAND	*pGet = (STONG_GET_MEMBER_INFO_COMMAND*)pData;
			STONG_MEMBER_INFO_SYNC	sInfo;
			ZeroMemory(&sInfo, sizeof(sInfo));
			if (g_cTongSet.GetTongMemberInfo(pGet, &sInfo))
				this->SendPackage((const void *)&sInfo, sInfo.m_wLength);
		}
		break;
	case enumC2S_TONG_SEARCH_INFO:
		{
			STONG_SEARCH_INFO_COMMAND* pGet = (STONG_SEARCH_INFO_COMMAND*)pData;
			STONG_SEARCH_INFO_SYNC	sInfo;
			ZeroMemory(&sInfo, sizeof(sInfo));
			if (g_cTongSet.SearchInfo(pGet, &sInfo))
			{	//-->GS
				this->SendPackage((const void*)&sInfo, sInfo.m_wLength);
			}
		}
		break;
	case enumC2S_TONG_INSTATE:
		{//-->GS  ren
			STONG_INSTATE_COMMAND	*pInstate = (STONG_INSTATE_COMMAND*)pData;
			STONG_INSTATE_SYNC	sSync;
			ZeroMemory(&sSync, sizeof(sSync));
			if (g_cTongSet.Instate(pInstate, &sSync))
			{
				//rTRACE("DEBUG, m_dwParam: %d\n", sSync.m_dwParam);
				//rTRACE("DEBUG, m_dwTongNameID: %d\n", sSync.m_dwTongNameID);
				//rTRACE("DEBUG, m_btSuccessFlag: %d\n", sSync.m_btSuccessFlag);
				//rTRACE("DEBUG, m_btOldFigure: %d\n", sSync.m_btOldFigure);
				//rTRACE("DEBUG, m_btOldPos: %d\n", sSync.m_btOldPos);
				//rTRACE("DEBUG, m_btNewFigure: %d\n", sSync.m_btNewFigure);
				//rTRACE("DEBUG, m_btNewPos: %d\n", sSync.m_btNewPos);
				//rTRACE("DEBUG, m_szTitle: %s\n", sSync.m_szTitle);
				//rTRACE("DEBUG, m_szName: %s\n", sSync.m_szName);

				this->SendPackage((const void*)&sSync, sizeof(sSync));
			}
		}
		break;
	case enumC2S_TONG_KICK:
		{//-->GS
			STONG_KICK_COMMAND	*pKick = (STONG_KICK_COMMAND*)pData;
			STONG_KICK_SYNC	sKick;
			if(g_cTongSet.Kick(pKick, &sKick))
				this->SendPackage((const void *)&sKick, sizeof(sKick));
		}
		break;
	case enumC2S_TONG_LEAVE:
		{//-->GS
			STONG_LEAVE_COMMAND	*pLeave = (STONG_LEAVE_COMMAND*)pData;
			STONG_LEAVE_SYNC	sLeave;
			//sLeave.ProtocolID = 0;
			//g_cTongSet.Leave(pLeave, &sLeave);
			//if (sLeave.ProtocolID != 0)
			if(g_cTongSet.Leave(pLeave, &sLeave))
				this->SendPackage((const void *)&sLeave, sizeof(sLeave));
		}
		break;
	case enumC2S_TONG_MONEY_SAVE:
		{//-->GS
			STONG_MONEY_COMMAND *pMoney = (STONG_MONEY_COMMAND*)pData;
			STONG_MONEY_SYNC	sMoney;
			//g_cTongSet.ChangeMoney(pMoney,&sMoney);
			//if (sMoney.ProtocolID != 0)
			if(g_cTongSet.ChangeMoney(pMoney, &sMoney))
				this->SendPackage((const void *)&sMoney, sizeof(sMoney));
		}
		break;
	case enumC2S_TONG_MONEY_GET:
		{//-->GS
			STONG_MONEY_COMMAND *pMoney = (STONG_MONEY_COMMAND*)pData;
			STONG_MONEY_SYNC	sMoney;
			//g_cTongSet.ChangeMoney(pMoney,&sMoney);
			//if (sMoney.ProtocolID != 0)
			if(g_cTongSet.ChangeMoney(pMoney, &sMoney))
				this->SendPackage((const void *)&sMoney, sizeof(sMoney));
		}
		break;
	case enumC2S_TONG_MONEY_SND:
		{//-->GS
			STONG_MONEY_COMMAND *pMoney = (STONG_MONEY_COMMAND*)pData;
			STONG_MONEY_SYNC	sMoney;
			g_cTongSet.ChangeMoney(pMoney,&sMoney);
			if (sMoney.ProtocolID != 0)
				this->SendPackage((const void *)&sMoney, sizeof(sMoney));
		}
		break;
	case enumC2S_TONG_CHANGE_MASTER:
		{//-->GS
			STONG_CHANGE_MASTER_COMMAND	*pChange = (STONG_CHANGE_MASTER_COMMAND*)pData;

			char	szName[NAME_LENGHT];
			STONG_ONE_LEADER_INFO	sInfo;
			static_assert(sizeof(pChange->m_szName) <= sizeof(szName),
				"tong change-master name does not fit szName");
			memcpy(szName, pChange->m_szName, sizeof(pChange->m_szName));
			szName[NAME_LENGHT-1] = 0;

			if (!g_cTongSet.SearchOne(pChange->m_dwTongNameID, szName, &sInfo))
			{
				STONG_CHANGE_MASTER_FAIL_SYNC	sFail;
				sFail.ProtocolFamily	= pf_tong;
				sFail.ProtocolID		= enumS2C_TONG_CHANGE_MASTER_FAIL;
				sFail.m_dwParam			= pChange->m_dwParam;
				sFail.m_btFailID		= 2;
				sFail.m_dwTongNameID	= pChange->m_dwTongNameID;
				memcpy(sFail.m_szName, szName, sizeof(szName));
				this->SendPackage((const void *)&sFail, sizeof(sFail));
				break;
			}

		//	{{
				CNetConnectDup conndup;
				DWORD nameid = 0;
				unsigned long param = 0;
				if (g_TongServer.FindPlayerByRole(NULL, std::_tstring(szName), &conndup, NULL, &nameid, &param))
				{
					CNetConnectDup tongconndup = g_TongServer.FindTongConnectByIP(conndup.GetIP());
					if (tongconndup.IsValid())
					{
						STONG_CHECK_GET_MASTER_POWER_SYNC	sCheck;
						sCheck.ProtocolFamily	= pf_tong;
						sCheck.ProtocolID		= enumS2C_TONG_CHECK_CHANGE_MASTER_POWER;
						sCheck.m_dwTongNameID	= pChange->m_dwTongNameID;
						sCheck.m_btFigure		= pChange->m_btFigure;
						sCheck.m_btPos			= pChange->m_btPos;
						sCheck.m_dwParam		= param;
						memcpy(sCheck.m_szName, szName, sizeof(szName));
						
						tongconndup.SendPackage((const void *)&sCheck, sizeof(sCheck));
					}
					break;
				}

				// 不在线 not find 或者 tong is not connect 当作不在线
				STONG_CHANGE_MASTER_FAIL_SYNC	sFail;
				sFail.ProtocolFamily	= pf_tong;
				sFail.ProtocolID		= enumS2C_TONG_CHANGE_MASTER_FAIL;
				sFail.m_dwParam			= pChange->m_dwParam;
				sFail.m_btFailID		= 0;
				sFail.m_dwTongNameID	= pChange->m_dwTongNameID;
				memcpy(sFail.m_szName, szName, sizeof(szName));
				this->SendPackage((const void *)&sFail, sizeof(sFail));
		//	}}

		}
		break;

	case enumC2S_TONG_ACCEPT_MASTER:
		{
			STONG_ACCEPT_MASTER_COMMAND	*pAccept = (STONG_ACCEPT_MASTER_COMMAND*)pData;
			g_cTongSet.AcceptMaster(pAccept);
		}
		break;

	case enumC2S_TONG_GET_LOGIN_DATA:
		{
			STONG_GET_LOGIN_DATA_COMMAND	*pLogin = (STONG_GET_LOGIN_DATA_COMMAND*)pData;
			STONG_LOGIN_DATA_SYNC	sLogin;
			if (g_cTongSet.GetLoginData(pLogin, &sLogin))
				this->SendPackage((const void *)&sLogin, sizeof(sLogin));
		}
		break;
	case enumC2S_TONG_SET_POWER:
		{
			STONG_POWER_COMMAND* pPower = (STONG_POWER_COMMAND*)pData;
			STONG_POWER_SYNC	sSync;
			ZeroMemory(&sSync, sizeof(sSync));
			if (g_cTongSet.SetPower(pPower, &sSync))
				this->SendPackage((const void*)&sSync, sizeof(sSync));
		}
		break;
	case enumC2S_TONG_GET_POWER:
		{
			STONG_GET_POWER_COMMAND* pInstate = (STONG_GET_POWER_COMMAND*)pData;
			STONG_POWER_SYNC	sSync;
			ZeroMemory(&sSync, sizeof(sSync));
			if (g_cTongSet.GetPower(pInstate, &sSync))
				this->SendPackage((const void*)&sSync, sizeof(sSync));
		}
		break;
	case enumC2S_TONG_MORE_INFO:
	{

	}
	break;
	case enumC2S_TONG_UPDATE_LEVEL:
	{
		STONG_UPDATE_LEVEL_COMMAND* pUpdate = (STONG_UPDATE_LEVEL_COMMAND*)pData;
		g_cTongSet.UpdateLevel(pUpdate);
	}
	break;

	default:
		{
		return 8;
		break;
		}
	}
	return 9;
}

void CTongConnect::Proc0_Friend(const void* pData, size_t size)
{
	EXTEND_HEADER* pHeader = (EXTEND_HEADER*)pData;

	if (pHeader->ProtocolID == friend_c2c_askaddfriend)
	{
		Proc1_Friend_AskAddFriend(pData, size);
	}
	else if (pHeader->ProtocolID == friend_c2c_repaddfriend)
	{
		Proc1_Friend_RepAddFriend(pData, size);
	}
	else if (pHeader->ProtocolID == friend_c2s_groupfriend)
	{
		Proc1_Friend_GroupFriend(pData, size);
	}
	else if (pHeader->ProtocolID == friend_c2s_erasefriend)
	{
		Proc1_Friend_EraseFriend(pData, size);
	}
	else if (pHeader->ProtocolID == friend_c2s_asksyncfriendlist)
	{//获取同步朋友清单
		Proc1_Friend_AskSyncFriendList(pData, size);
	}
	else if (pHeader->ProtocolID == friend_c2s_associate)
	{
		Proc1_Friend_Associate(pData, size);
	}
	else if (pHeader->ProtocolID == friend_c2s_associatebevy)
	{
		Proc1_Friend_AssociateBevy(pData, size);
	}
}

void CTongConnect::Proc1_Friend_AskAddFriend(const void* pData, size_t size)
{
	ASK_ADDFRIEND_CMD* pAafCmd = (ASK_ADDFRIEND_CMD*)pData;
	tagPlusSrcInfo* pSrcInfo = (tagPlusSrcInfo*)((BYTE*)pData + size) - 1;

	if (!gIsLegalString(pAafCmd->dstrole, 1, NAME_LENGHT))
		return;

	std::_tstring srcrole;
	if (!g_TongServer.FindPlayerByIpParam(NULL, GetIP(), pSrcInfo->lnID, NULL, NULL, &srcrole, NULL))
		return;

	std::_tstring dstrole(pAafCmd->dstrole);

	if (_tstring_equal()(srcrole, dstrole))
		return;


	BOOL pass = FALSE;
	BOOL promise = FALSE;

	CNetConnectDup dsthostconndup;
	DWORD dstnameid = 0;
	unsigned long dstparam = 0;
	if (g_TongServer.FindPlayerByRole(NULL, dstrole, &dsthostconndup, NULL, &dstnameid, &dstparam))
	{
		if (g_FriendMgr.PlayerIsFriend(dstrole, srcrole))
		{
			g_FriendMgr.PlayerAddFriend(srcrole, dstrole);

			promise = TRUE;	//默许
		}
		else
		{
			if (g_FriendMgr.TrackAddFriend(srcrole, dstrole, pAafCmd->pckgid))
			{
				ASK_ADDFRIEND_SYNC AafSync;
				AafSync.ProtocolFamily = pf_friend;
				AafSync.ProtocolID = friend_c2c_askaddfriend;
				AafSync.pckgid = pAafCmd->pckgid;
				snprintf(AafSync.srcrole, sizeof(AafSync.srcrole), "%s", srcrole.c_str());

				if (PassToSomeone(dsthostconndup.GetIP(), dstparam, dstnameid, &AafSync, sizeof(ASK_ADDFRIEND_SYNC)))
					pass = TRUE;
				else
					g_FriendMgr.ExtractAddFriend(srcrole, dstrole, pAafCmd->pckgid);
			}
		}
	}

	if (!pass)
	{
		REP_ADDFRIEND_SYNC RafSync;
		RafSync.ProtocolFamily = pf_friend;
		RafSync.ProtocolID = friend_c2c_repaddfriend;
		RafSync.pckgid = pAafCmd->pckgid;
		strcpy(RafSync.srcrole, pAafCmd->dstrole);
		RafSync.answer = promise ? answerAgree : answerUnable;

		PassToSomeone(GetIP(), pSrcInfo->lnID, pSrcInfo->nameid, &RafSync, sizeof(REP_ADDFRIEND_SYNC));
	}
}

void CTongConnect::Proc1_Friend_RepAddFriend(const void* pData, size_t size)
{
	REP_ADDFRIEND_CMD* pRafCmd = (REP_ADDFRIEND_CMD*)pData;
	tagPlusSrcInfo* pSrcInfo = (tagPlusSrcInfo*)((BYTE*)pData + size) - 1;

	if (!gIsLegalString(pRafCmd->dstrole, 1, NAME_LENGHT))
		return;

	std::_tstring srcrole;
	if (!g_TongServer.FindPlayerByIpParam(NULL, GetIP(), pSrcInfo->lnID, NULL, NULL, &srcrole, NULL))
		return;

	std::_tstring dstrole(pRafCmd->dstrole);

	if (_tstring_equal()(srcrole, dstrole))
		return;


	if (!g_FriendMgr.ExtractAddFriend(dstrole, srcrole, pRafCmd->pckgid))
		return;


	if (pRafCmd->answer == answerAgree)
		g_FriendMgr.PlayerAddFriend(srcrole, dstrole);


	{{
	CNetConnectDup dsthostconndup;
	DWORD dstnameid = 0;
	unsigned long dstparam = 0;
	if (g_TongServer.FindPlayerByRole(NULL, dstrole, &dsthostconndup, NULL, &dstnameid, &dstparam))
	{
		REP_ADDFRIEND_SYNC RafSync;
		RafSync.ProtocolFamily = pf_friend;
		RafSync.ProtocolID = friend_c2c_repaddfriend;
		RafSync.pckgid = pRafCmd->pckgid;
		snprintf(RafSync.srcrole, sizeof(RafSync.srcrole), "%s", srcrole.c_str());
		RafSync.answer = pRafCmd->answer;

		PassToSomeone(dsthostconndup.GetIP(), dstparam, dstnameid, &RafSync, sizeof(REP_ADDFRIEND_SYNC));
	}
	else
	{
		FRIEND_STATE Fs;
		Fs.ProtocolFamily = pf_friend;
		Fs.ProtocolID = friend_s2c_friendstate;
		Fs.state = stateOffline;

		PassToSomeone(GetIP(), pSrcInfo->lnID, pSrcInfo->nameid, &Fs, sizeof(FRIEND_STATE));
	}
	}}
}

void CTongConnect::Proc1_Friend_GroupFriend(const void* pData, size_t size)
{
	GROUP_FRIEND* pGf = (GROUP_FRIEND*)pData;
	tagPlusSrcInfo* pSrcInfo = (tagPlusSrcInfo*)((BYTE*)pData + size) - 1;

	std::_tstring srcrole;
	if (!g_TongServer.FindPlayerByIpParam(NULL, GetIP(), pSrcInfo->lnID, NULL, NULL, &srcrole, NULL))
		return;

	char* pGroupTag = (char*)(pGf + 1);
	if (*pGroupTag != specGroup)
		return;	//error

	char* pGroup = (char*)(pGroupTag + 1);

on_nextgroup:
	if (!gIsLegalString(pGroup, 0, _GROUP_NAME_LEN))
		return;	//error

	std::_tstring group(pGroup);
	std::list<std::_tstring> blackList;

	for (char* pRoleTag = pGroup + strlen(pGroup) + 1; ; )
	{
		if (*pRoleTag == specOver)
			break;//return;	//success
		else if (*pRoleTag == specGroup)
		{
			pGroup = pRoleTag + 1;
			goto on_nextgroup;	//next group
		}
		else if (*pRoleTag == specRole)
		{
			char* pRole = pRoleTag + 1;

			if (!gIsLegalString(pRole, 1, NAME_LENGHT))
				break;  //return;	//error

			std::_tstring role(pRole);
			if (g_FriendMgr.SetFriendGroup(srcrole, role, group))
				blackList.push_back(role);

			pRoleTag = pRole + role.size() + 1;
			continue; 	//next role
		}
		else
			break; // return;	//error
	}

	if (!blackList.empty())
	{
		std::list<std::_tstring>::iterator it = blackList.begin();
		for (int i = 0; i < std::ceil(blackList.size() * 1.0f / defTONG_ONE_PAGE_MAX_NUM); i++)
		{
			FRIEND_SYNCBLACKLIST	sSync;
			ZeroMemory(&sSync, sizeof(sSync));
			sSync.ProtocolFamily = pf_friend;
			sSync.ProtocolID = friend_s2c_syncblacklist;

			int idx = 0;
			for (; it != blackList.end(); it++)
			{
				snprintf(sSync.m_szBlackList[idx], sizeof(sSync.m_szBlackList[idx]), "%s", (*it).c_str());
				sSync.m_szBlackList[idx][(*it).length()] = '\0';
				idx++;
				if (idx >= defTONG_ONE_PAGE_MAX_NUM)
					break;
			}

			CNetConnectDup conndup;
			{ {
					DWORD nameid = 0;
					unsigned long param = 0;
					if (g_TongServer.FindPlayerByRole(NULL, std::_tstring(srcrole), &conndup, NULL, &nameid, &param))
					{
						CNetConnectDup tongconndup = g_TongServer.FindTongConnectByIP(conndup.GetIP());
						if (tongconndup.IsValid())
						{
							sSync.m_nParam = param;
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
		}
	}
}

void CTongConnect::Proc1_Friend_EraseFriend(const void* pData, size_t size)
{
	ERASE_FRIEND* pEf = (ERASE_FRIEND*)pData;
	tagPlusSrcInfo* pSrcInfo = (tagPlusSrcInfo*)((BYTE*)pData + size) - 1;

	if (!gIsLegalString(pEf->friendrole, 1, NAME_LENGHT))
		return;

	std::_tstring srcrole;
	if (!g_TongServer.FindPlayerByIpParam(NULL, GetIP(), pSrcInfo->lnID, NULL, NULL, &srcrole, NULL))
		return;

	g_FriendMgr.PlayerDelFriend(srcrole, std::_tstring(pEf->friendrole));
}

void CTongConnect::Proc1_Friend_AskSyncFriendList(const void* pData, size_t size)
{
	ASK_SYNCFRIENDLIST* pAsfl = (ASK_SYNCFRIENDLIST*)pData;
	tagPlusSrcInfo* pSrcInfo = (tagPlusSrcInfo*)((BYTE*)pData + size) - 1;

	std::_tstring role;
	if (!g_TongServer.FindPlayerByIpParam(NULL, GetIP(), pSrcInfo->lnID, NULL, NULL, &role, NULL))
		return;

	g_FriendMgr.SomeoneSyncFriends(role, pSrcInfo->lnID, pSrcInfo->nameid, CNetConnectDup(*this), pAsfl->full, pAsfl->pckgid);
}

void CTongConnect::Proc1_Friend_Associate(const void* pData, size_t size)
{
	FRIEND_ASSOCIATE* pFa = (FRIEND_ASSOCIATE*)pData;

	char* szGroup = (char*)(pFa + 1);
	if (!gIsLegalString(szGroup, 0, _GROUP_NAME_LEN))
		return;

	std::_tstring group(szGroup);

	char* szRole1 = szGroup + group.size() + 1;
	if (!gIsLegalString(szRole1, 1, NAME_LENGHT))
		return;

	std::_tstring role1(szRole1);

	char* szRole2 = szRole1 + role1.size() + 1;
	if (!gIsLegalString(szRole2, 1, NAME_LENGHT))
		return;

	std::_tstring role2(szRole2);

	if (_tstring_equal()(role1, role2))
		return;


	{{
		g_FriendMgr.PlayerAssociate(role1, role2, group, pFa->bidir);
	}}



	//size_t basesize = sizeof(FRIEND_SYNCASSOCIATE) + group.size() + 1;
	//size_t role1size = role1.size() + 1;
	//size_t role2size = role2.size() + 1;
	//size_t maxsize = basesize + (pFa->bidir ? max(role1size, role2size) : role2size) + 1;

	//FRIEND_SYNCASSOCIATE* pFsa = (FRIEND_SYNCASSOCIATE*)_alloca(maxsize);
	//pFsa->ProtocolFamily = pf_friend;
	//pFsa->ProtocolID = friend_s2c_syncassociate;

	//char* szGroupSync = (char*)(pFsa + 1);
	//strcpy(szGroupSync, group.c_str());

	//char* szRoleSync = szGroupSync + group.size() + 1;


	//{{
	//CNetConnectDup conndup;
	//DWORD nameid = 0;
	//unsigned long param = 0;
	//if (g_HostServer.FindPlayerByRole(NULL, role1, &conndup, NULL, &nameid, &param))
	//{
	//	size_t pckgsize = basesize + role2size + 1;

	//	strcpy(szRoleSync, role2.c_str());
	//	szRoleSync[role2size] = 0;

	//	PassToSomeone(conndup.GetIP(), param, nameid, pFsa, pckgsize);
	//}
	//}}

	//if (pFa->bidir)
	//{
	//	CNetConnectDup conndup2;
	//	DWORD nameid2 = 0;
	//	unsigned long param2 = 0;
	//	if (g_HostServer.FindPlayerByRole(NULL, role2, &conndup2, NULL, &nameid2, &param2))
	//	{
	//		size_t pckgsize2 = basesize + role1size + 1;

	//		strcpy(szRoleSync, role1.c_str());
	//		szRoleSync[role1size] = 0;

	//		PassToSomeone(conndup2.GetIP(), param2, nameid2, pFsa, pckgsize2);
	//	}
	//}
}

void CTongConnect::Proc1_Friend_AssociateBevy(const void* pData, size_t size)
{
	FRIEND_ASSOCIATEBEVY* pFab = (FRIEND_ASSOCIATEBEVY*)pData;

	std::_tstring group;
	CFriendMgr::_BEVY bevy;

	size_t rolessize = 0;

	{{
	char* szGroup = (char*)(pFab + 1);
	if (!gIsLegalString(szGroup, 0, _GROUP_NAME_LEN))
		return;

	group.assign(szGroup);

	char* szRoles = szGroup + group.size() + 1;
	while (*szRoles)
	{
		if (!gIsLegalString(szRoles, 1, NAME_LENGHT))
			return;

		std::_tstring role(szRoles);
		bevy.push_back(role);

		szRoles += role.size() + 1;
		rolessize += role.size() + 1;
	}

	if (bevy.size() < 2)
		return;
	}}


	g_FriendMgr.PlayerAssociateBevy(bevy, group);


	{{
	size_t basesize = sizeof(FRIEND_SYNCASSOCIATE) + group.size() + 1;
	size_t maxsize = basesize + rolessize + 1;

	FRIEND_SYNCASSOCIATE* pFsa = (FRIEND_SYNCASSOCIATE*)_alloca(maxsize);
	pFsa->ProtocolFamily = pf_friend;
	pFsa->ProtocolID = friend_s2c_syncassociate;

	char* szGroupSync = (char*)(pFsa + 1);
	strcpy(szGroupSync, group.c_str());

	char* szRolesSync = szGroupSync + group.size() + 1;


	for (CFriendMgr::_BEVY::iterator itS = bevy.begin(); itS != bevy.end(); ++itS)
	{
		std::_tstring someone = *itS;

		CNetConnectDup conndup;
		DWORD nameid = 0;
		unsigned long param = 0;
		if (!g_HostServer.FindPlayerByRole(NULL, someone, &conndup, NULL, &nameid, &param))
			continue;

		size_t cursor = 0;
		for (CFriendMgr::_BEVY::iterator itD = bevy.begin(); itD != bevy.end(); ++itD)
		{
			if (itD == itS)
				continue;

			std::_tstring dst = *itD;

			strcpy(szRolesSync + cursor, dst.c_str());
			cursor += dst.size() + 1;
		}
		szRolesSync[cursor++] = 0;

		PassToSomeone(conndup.GetIP(), param, nameid, pFsa, basesize + cursor);
	}
	}}
}


void CTongConnect::Proc0_Normal(const void* pData, size_t size)
{
	EXTEND_HEADER* pHeader = (EXTEND_HEADER*)pData;

	if (pHeader->ProtocolID == c2s__entergamegkjyiruqine)
	{
		Proc1_Normal_EnterGame(pData, size);
	}
	else if (pHeader->ProtocolID == c2s_leavegame)
	{
		Proc1_Normal_LeaveGame(pData, size);
	}
}

//进入游戏调用 填充各个容器 帐号 角色结构等
void CTongConnect::Proc1_Normal_EnterGame(const void* pData, size_t size)
{
	tagEnterGame2* pEnterGame2 = (tagEnterGame2*)pData;

	std::_tstring strAcc = (char*)pEnterGame2->szAccountName;//进入游戏的帐号
	assert(!strAcc.empty());
	std::_tstring strRole = (char*)pEnterGame2->szCharacterName;//进入游戏的角色名
	assert(!strRole.empty());

	ACCINFO infoAcc;
	infoAcc.role = strRole;
	infoAcc.nameID = pEnterGame2->dwNameID;
	infoAcc.param = pEnterGame2->lnID;//客户端连接GS的网络唯一标示 多区会覆盖掉

	ROLEINFO infoRole;
	infoRole.acc = strAcc;
	infoRole.nameID = pEnterGame2->dwNameID;
	infoRole.param = pEnterGame2->lnID;//客户端连接GS的网络唯一标示 多区会覆盖掉

	PARAMINFO infoParam;
	infoParam.acc = strAcc;
	infoParam.role = strRole;
	infoParam.nameID = pEnterGame2->dwNameID;

	{{
		DUMMY_AUTOLOCKWRITE(m_lockPlayer);

		m_mapAcc[strAcc] = infoAcc;
		m_mapRole[strRole] = infoRole;
		m_mapParam[pEnterGame2->lnID] = infoParam;//有可能重复的 多区的时候 会覆盖掉 需要调整
	}}

	//rTRACE("DEBUG, GameStatus %d Enter Game", pEnterGame2->lnID);

	{{
		g_FriendMgr.SomeoneLogin(strRole, nTondNetIdx);//通知 这个 角色已经上线 
	}}

	rTRACE("Tong:player login: %s [%s] (%08X,%08X)", strAcc.c_str(), strRole.c_str(), GetIP(), pEnterGame2->lnID,pEnterGame2->nSelSerVer);
}
//离开游戏调用
void CTongConnect::Proc1_Normal_LeaveGame(const void* pData, size_t size)
{
	tagLeaveGame2* pLeaveGame = (tagLeaveGame2*)pData;


	std::_tstring strAcc = pLeaveGame->szAccountName;

	assert(!strAcc.empty());

	{{

		DUMMY_AUTOLOCKWRITE(m_lockPlayer);

		ACCMAP::iterator itAcc = m_mapAcc.find(strAcc);
		if (itAcc != m_mapAcc.end())
		{
			ACCINFO& rAccInfo = (*itAcc).second;

			//{{
			//	//ensure
			//	g_ChannelMgr.B_ClearPlayer(GetIP(), rAccInfo.param);
			//	g_FriendMgr.SomeoneLogout(rAccInfo.role);
			//}}
			//rTRACE("DEBUG, GameStatus %d Leave Game", m_mapAcc[pLeaveGame->szAccountName].param);


			m_mapRole.erase(rAccInfo.role);
			m_mapParam.erase(rAccInfo.param);

			m_mapAcc.erase(itAcc);
		}
	}}
	rTRACE("---Tong:(%u区)玩家账号(%s)退出游戏---",pLeaveGame->nSelServer, strAcc.c_str());
}

void CTongConnect::Proc0_ServerCommunity(const void* pData, size_t size)
{
	EXTEND_HEADER* pHeader = (EXTEND_HEADER*)pData;

	if (pHeader->ProtocolID == relay_s2c_udateserver)
	{//
		tagLeaveGame2 * pServerData = (tagLeaveGame2*)pData;
		//nSerVerIndex = pServerData->nSelServer;//属于哪个区的gs
		nSerVerIndex = (short)HIWORD(pServerData->nSelServer);
		nGsNumBer    =  LOWORD(pServerData->nSelServer);
		m_sTongServer[nSerVerIndex] = GetID();//保存 各个 服务器客户端的连接号 即是GS 的标示编号
		setSerRegIndex(nSerVerIndex);
		setGsNumIndex(nGsNumBer);
		rTRACE("---Tong:(%d)区服务器启动完毕:%08X----",nSerVerIndex,nGsNumBer);
	}
}

BOOL CTongConnect::FindPlayerByAcc(const std::_tstring& acc, std::_tstring* pRole, DWORD* pNameID, unsigned long* pParam, std::_tstring* pRealAcc)
{
	DUMMY_AUTOLOCKREAD(m_lockPlayer);

	ACCMAP::iterator it = m_mapAcc.find(acc);
	if (it == m_mapAcc.end())
		return FALSE;

	const ACCINFO& infoAcc = (*it).second;

	if (pRole != NULL)
		*pRole = infoAcc.role;
	if (pNameID != NULL)
		*pNameID = infoAcc.nameID;
	if (pParam != NULL)
		*pParam = infoAcc.param;

	if (pRealAcc != NULL)
		*pRealAcc = (*it).first;

	return TRUE;
}

BOOL CTongConnect::FindPlayerByRole(const std::_tstring& role, std::_tstring* pAcc, DWORD* pNameID, unsigned long* pParam, std::_tstring* pRealRole)
{
	DUMMY_AUTOLOCKREAD(m_lockPlayer);

	ROLEMAP::iterator it = m_mapRole.find(role);
	if (it == m_mapRole.end())
		return FALSE;

	const ROLEINFO& infoRole = (*it).second;

	if (pAcc != NULL)
		*pAcc = infoRole.acc;
	if (pNameID != NULL)
		*pNameID = infoRole.nameID;
	if (pParam != NULL)
		*pParam = infoRole.param;

	if (pRealRole != NULL)
		*pRealRole = (*it).first;

	return TRUE;
}

//这个角色 和 帐号 是否连接 每登陆一个玩家 都记录在案 
BOOL CTongConnect::FindPlayerByParam(unsigned long param, std::_tstring* pAcc, std::_tstring* pRole, DWORD* pNameID)
{
	DUMMY_AUTOLOCKREAD(m_lockPlayer);

	PARAMMAP::iterator it = m_mapParam.find(param);
	if (it == m_mapParam.end())
	{
		rTRACE("----发送(%08X区)消息CTongConnect::FindPlayerByIpParam失败A-----",param);
		return FALSE;
	}

	const PARAMINFO& infoParam = (*it).second;

	if (pAcc != NULL)
		*pAcc = infoParam.acc;
	if (pRole != NULL)
		*pRole = infoParam.role;
	if (pNameID != NULL)
		*pNameID = infoParam.nameID;

	return TRUE;
}

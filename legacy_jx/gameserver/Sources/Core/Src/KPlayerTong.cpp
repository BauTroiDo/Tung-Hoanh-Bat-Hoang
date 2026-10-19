//---------------------------------------------------------------------------
// Sword3 Engine (c) 2003 by Kingsoft
//
// File:	KPlayerTong.cpp
// Date:	2003.08.12
// Code:	边城浪子
// Desc:	KPlayerTong Class
//---------------------------------------------------------------------------

#include	"KCore.h"
#include	"KNpc.h"
#include	"KPlayer.h"
#include	"KPlayerSet.h"
#include	"KPlayerTong.h"
#include "../../Engine/src/Text.h"
//#include "../../Engine/src/KFilePath.h"
#include     "KFilePath.h"
#include     "KTabFile.h"
#include     "KTabFileCtrl.h"

#include "KProtocolDef.h"
#include "KProtocol.h"
#include "KRelayProtocol.h"
#include "KTongProtocol.h"

#ifndef _SERVER
#include	"CoreShell.h"
#endif

#define     TONG_DATA_FILE             "\\TongSet\\"
#define     TONG_SET_FILE             "\\TongSet\\TongSet.ini"
//#define		defTONG_NAME_LENGTH			8
#define		defFuncShowNormalMsg(str)		\
	{										\
		KSystemMessage	sMsg;				\
		sMsg.eType = SMT_NORMAL;			\
		sMsg.byConfirmType = SMCT_NONE;		\
		sMsg.byPriority = 0;				\
		sMsg.byParamSize = 0;				\
		snprintf(sMsg.szMessage, sizeof(sMsg.szMessage), "%s", str);		\
		sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));\
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);\
	}

//-------------------------------------------------------------------------
//	功能：初始化
//-------------------------------------------------------------------------
void	KPlayerTong::Init(int nPlayerIdx)
{
	nAttAckInfo.clear();
	m_nPlayerIndex = nPlayerIdx;

	ActiveLoop=0;

	Clear();
}

//-------------------------------------------------------------------------
//	功能：清空
//-------------------------------------------------------------------------
void	KPlayerTong::Clear()
{
	m_nFlag				= 0;
	m_nFigure			= enumTONG_FIGURE_MEMBER;
	m_nCamp				= 0;
	m_dwTongNameID		= 0;
	m_szName[0]			= 0;
	m_szTitle[0]		= 0;
	m_BMasterName[0]	= 0;
	m_nApplyTo			= 0;
	m_nMoney			= 0;
	m_AttackState       = 0;
	m_AttackCamp        = 0;
	m_nDeathCount       = 0;
    m_AttackTime        = 0;
    m_AttackName[0]     = 0;
	m_Mapid             = 0;
	m_CurRenShu			= 0;
	m_nLevel            = 1;

	m_nAttackNum		= 0;	                            // 参战场数
    m_nWinNum		    = 0;								// 胜利场数
    m_nLoseNum		    = 0;
    sprintf(m_AttackName,"暂无");
}

#ifndef _SERVER
//-------------------------------------------------------------------------
//	功能：申请创建帮会
//-------------------------------------------------------------------------
BOOL	KPlayerTong::ApplyCreateTong(int nCamp, char *lpszTongName)
{
	defFuncShowNormalMsg(MSG_TONG_APPLY_CREATE);
	// 帮会名问题
	if (!lpszTongName || !lpszTongName[0] || strlen(lpszTongName) > defTONG_NAME_MAX_LENGTH)
	{
		defFuncShowNormalMsg(MSG_TONG_CREATE_ERROR01);
		return FALSE;
	}
	// 帮会阵营问题
	if (nCamp != camp_justice && nCamp != camp_evil && nCamp != camp_balance)
	{
		defFuncShowNormalMsg(MSG_TONG_CREATE_ERROR06);
		return FALSE;
	}
	// 已经是帮会成员
	if (m_nFlag)
	{
		defFuncShowNormalMsg(MSG_TONG_CREATE_ERROR05);
		return FALSE;
	}
	// 自己的阵营问题
	if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentCamp != camp_free)
	{
		defFuncShowNormalMsg(MSG_TONG_CREATE_ERROR04);
		return FALSE;
	}
	// 等级问题
	if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Level < PlayerSet.m_sTongParam.m_nLevel)
	{
		defFuncShowNormalMsg(MSG_TONG_CREATE_ERROR07);
		return FALSE;
	}
	if ((int)Player[CLIENT_PLAYER_INDEX].m_dwLeadLevel < PlayerSet.m_sTongParam.m_nLeadLevel)
	{
		defFuncShowNormalMsg(MSG_TONG_CREATE_ERROR06);
		return FALSE;
	}
	// 交易过程中
	if (Player[CLIENT_PLAYER_INDEX].CheckTrading())
	{
		return FALSE;
	}
	// 钱问题
	if (Player[CLIENT_PLAYER_INDEX].m_ItemList.GetMoneyAmount() < PlayerSet.m_sTongParam.m_nMoney)
	{
		char	szBuf[80];
		sprintf(szBuf, MSG_TONG_CREATE_ERROR08, PlayerSet.m_sTongParam.m_nMoney);
		defFuncShowNormalMsg(szBuf);
		return FALSE;
	}
	// 组队不能建帮会
	if (Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag)
	{
		defFuncShowNormalMsg(MSG_TONG_CREATE_ERROR09);
		return FALSE;
	}

	TONG_APPLY_CREATE_COMMAND	sApply;
	sApply.ProtocolType = c2s_extendtong;
	sApply.m_wLength = sizeof(TONG_APPLY_CREATE_COMMAND) - 1;
	sApply.m_btMsgId = enumTONG_COMMAND_ID_APPLY_CREATE;
	sApply.m_btCamp = (BYTE)nCamp;
	snprintf(sApply.m_szName, sizeof(sApply.m_szName), "%s", lpszTongName);

	if (g_pClient)
		g_pClient->SendPackToServer(&sApply, sApply.m_wLength + 1);

	return TRUE;
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	功能：申请加入帮会
//-------------------------------------------------------------------------
BOOL	KPlayerTong::ApplyAddTong(DWORD dwNpcID)
{
	defFuncShowNormalMsg(MSG_TONG_APPLY_ADD);
	// 已经是帮会成员
	if (m_nFlag)
	{
		defFuncShowNormalMsg(MSG_TONG_APPLY_ADD_ERROR1);
		return FALSE;
	}
	// 自己的阵营问题
	if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentCamp != camp_free)
	{
		defFuncShowNormalMsg(MSG_TONG_APPLY_ADD_ERROR2);
		return FALSE;
	}
	// 交易过程中
	if (Player[CLIENT_PLAYER_INDEX].CheckTrading())
	{
		return FALSE;
	}
	// 组队不能建帮会
	if (Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag)
	{
		defFuncShowNormalMsg(MSG_TONG_APPLY_ADD_ERROR3);
		return FALSE;
	}

	TONG_APPLY_ADD_COMMAND	sApply;
	sApply.ProtocolType = c2s_extendtong;
	sApply.m_wLength = sizeof(TONG_APPLY_ADD_COMMAND) - 1;
	sApply.m_btMsgId = enumTONG_COMMAND_ID_APPLY_ADD;
	sApply.m_dwNpcID = dwNpcID;

	if (g_pClient)
		g_pClient->SendPackToServer(&sApply, sizeof(TONG_APPLY_ADD_COMMAND));

	return TRUE;
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	功能：判断创建帮会条件是否成立
//-------------------------------------------------------------------------
int		KPlayerTong::CheckCreateCondition(int nCamp, char *lpszTongName)
{
	// check dieu kien o lua
	//return 0;
	 
	if (Player[m_nPlayerIndex].m_nIndex <= 0)
		return enumTONG_CREATE_ERROR_ID1;

	// 交易过程中
	if (Player[m_nPlayerIndex].CheckTrading())
		return enumTONG_CREATE_ERROR_ID2;

	// 帮会名问题
	if (!lpszTongName || !lpszTongName[0] || strlen(lpszTongName) > NAME_LENGHT)
		return enumTONG_CREATE_ERROR_ID3;

	// 帮会阵营问题
	if (nCamp != camp_justice && nCamp != camp_evil && nCamp != camp_balance)
		return enumTONG_CREATE_ERROR_ID4;

	// 已经是帮会成员
	if (m_nFlag)
		return enumTONG_CREATE_ERROR_ID5;

	// 自己的阵营问题
	if (Npc[Player[m_nPlayerIndex].m_nIndex].m_CurrentCamp != camp_free ||
		Npc[Player[m_nPlayerIndex].m_nIndex].m_Camp != camp_free)
		return enumTONG_CREATE_ERROR_ID6;

	// 等级问题
	if (Npc[Player[m_nPlayerIndex].m_nIndex].m_Level < PlayerSet.m_sTongParam.m_nLevel || (int)Player[m_nPlayerIndex].m_dwLeadLevel < PlayerSet.m_sTongParam.m_nLeadLevel)
		return enumTONG_CREATE_ERROR_ID7;

	// 钱问题
	if (Player[m_nPlayerIndex].m_ItemList.GetMoneyAmount() < PlayerSet.m_sTongParam.m_nMoney)
		return enumTONG_CREATE_ERROR_ID8;
	
	// 组队不能建帮会
	if (Player[m_nPlayerIndex].m_cTeam.m_nFlag)
		return enumTONG_CREATE_ERROR_ID9;

	// check nhac vuong kiem
	if (!Player[m_nPlayerIndex].m_ItemList.GetItemCount(4, PlayerSet.m_sTongParam.m_nItemID, 0, -1, -1, 3))
		return enumTONG_CREATE_ERROR_ID11;

	return 0;
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	功能：得到relay通知，帮会创建成功，处理相应数据
//-------------------------------------------------------------------------
BOOL	KPlayerTong::Create(int nCamp, char* lpszTongName, char* lpszTitle)
{
	//	if (!CheckCreateCondition(nCamp, lpszTongName))
	//		return FALSE;

	m_nFlag = 1;
	m_nFigure = enumTONG_FIGURE_MASTER;
	m_nCamp = nCamp;
	m_AttackState = 0;
	//m_szTitle[0] = 0;
	strcpy(m_szTitle, lpszTitle);
	m_nMoney = 0;
	m_nLevel = 1;
	m_AttackCamp = 0;
	m_AttackTime = 0;
	m_AttackName[0] = 0;
	m_nDeathCount = 0;
	m_nAttackNum = 0;	                            // 参战场数
	m_nWinNum = 0;								// 胜利场数
	m_nLoseNum = 0;

	snprintf(m_szName, sizeof(m_szName), "%s", lpszTongName); //帮会名
	snprintf(m_BMasterName, sizeof(m_BMasterName), "%s", Npc[Player[m_nPlayerIndex].m_nIndex].Name);   //帮主名Npc[Player[m_nPlayerIndex].m_nIndex].Name
	//strcpy(m_szMasterName,Npc[Player[m_nPlayerIndex].m_nIndex].Name);  //帮主名Npc[Player[m_nPl
	m_dwTongNameID = g_FileName2Id(lpszTongName);                     //赋值给 帮会ID

	Npc[Player[m_nPlayerIndex].m_nIndex].m_Camp = m_nCamp;
	Npc[Player[m_nPlayerIndex].m_nIndex].m_CurrentCamp = m_nCamp;
	Player[m_nPlayerIndex].m_ItemList.DecMoney(PlayerSet.m_sTongParam.m_nMoney);
	Npc[Player[m_nPlayerIndex].m_nIndex].m_btRankId = this->m_nFigure + 74;
	//del nhac vuong kiem
	Player[m_nPlayerIndex].m_ItemList.RemoveItemCount(1, 4, PlayerSet.m_sTongParam.m_nItemID, 0, -1, -1, 3);
	/*if (Player[m_nPlayerIndex].Save())	                     //生成数据包
	{
		//Player[m_nPlayerIndex].m_uMustSave = SAVE_REQUEST;	 //增加东西立即存档
		if (g_pDataClient)
		{//存档
			Player[m_nPlayerIndex].SavePlayerData(false);
		}
	} */ 

	// 通知客户端
	TONG_CREATE_SYNC	sCreate;
	sCreate.ProtocolType = s2c_tongcreate;
	sCreate.m_btCamp = nCamp;
	if (strlen(lpszTongName) < sizeof(sCreate.m_szName))
		snprintf(sCreate.m_szName, sizeof(sCreate.m_szName), "%s", lpszTongName);
	else
	{
		memcpy(sCreate.m_szName, lpszTongName, sizeof(sCreate.m_szName) - 1);
		sCreate.m_szName[sizeof(sCreate.m_szName) - 1] = 0;
	}
	if (g_pServer)
		g_pServer->PackDataToClient(Player[m_nPlayerIndex].m_nNetConnectIdx, &sCreate, sizeof(TONG_CREATE_SYNC));
	
	//===================开始写帮会列表文件=====================
	//KIniFile nTongSet;
	//KFile nFile;

	//if (!g_FileExists(TONG_SET_FILE))//检测这个文件是否存在
	//{
	//	nFile.Create(TONG_SET_FILE);
	//}
	//nFile.Close();

	////ZeroMemory(nTongSetPath,sizeof(nTongSetPath));

	//if (!nTongSet.Load(TONG_SET_FILE))
	//{
	//	Player[m_nPlayerIndex].m_ItemList.msgshow("提示:TONG数据有误,请联系GM处理!");
	//	nTongSet.Clear();
	//	//return;
	//}

	//if (nTongSet.IsSectionExist(lpszTongName))
	//{//检测这个节名是否存在 

	//}
	//nTongSet.WriteString(lpszTongName, "QingXiang", "请选择倾向");
	//nTongSet.WriteString(lpszTongName, "HuoDong1", "请选择活动");
	//nTongSet.WriteString(lpszTongName, "HuoDong2", "请选择活动");
	//nTongSet.WriteString(lpszTongName, "HuoDong3", "请选择活动");
	//nTongSet.WriteString(lpszTongName, "HuoDong4", "请选择活动");
	//nTongSet.WriteString(lpszTongName, "JiYu", "请填写帮会寄语");
	//nTongSet.WriteInteger(lpszTongName, "ApplyLevel", 60);
	//nTongSet.WriteInteger(lpszTongName, "RefuseLevel", 50);
	//nTongSet.WriteString(lpszTongName, "GongGao", "");
	//nTongSet.Save(TONG_SET_FILE);
	////Player[m_nPlayerIndex].m_ItemList.msgshow("提示:<color=yellow>帮会招募<color>信息保存成功!");
	//nTongSet.Clear();
	//==========================================================
	
	return TRUE;
}
#endif
//客户端
#ifndef _SERVER
//-------------------------------------------------------------------------
//	功能：得到服务器通知创建帮会
//-------------------------------------------------------------------------
void	KPlayerTong::Create(TONG_CREATE_SYNC *psCreate)
{
	if (!psCreate)
		return;

	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Camp = psCreate->m_btCamp;
	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentCamp = psCreate->m_btCamp;

	m_nFlag			= 1;
	m_nFigure		= enumTONG_FIGURE_MASTER;
	m_nCamp			= psCreate->m_btCamp;
	m_AttackState   = 0;
	m_szTitle[0]	= 0;
	m_AttackCamp        = 0;
    m_AttackTime        = 0;
    m_AttackName[0]     = 0;
	m_nAttackNum		= 0;	                            // 参战场数
    m_nWinNum		    = 0;								// 胜利场数
    m_nLoseNum		    = 0;

	memset(m_szName, 0, sizeof(m_szName)); //帮会名
	memcpy(m_szName, psCreate->m_szName, sizeof(psCreate->m_szName));
	snprintf(m_BMasterName, sizeof(m_BMasterName), "%s", Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name);//Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name)
	//strcpy(m_szMasterName,Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name);//Npc[P
	m_dwTongNameID	= g_FileName2Id(psCreate->m_szName);

	// 界面通知帮会建立成功
	defFuncShowNormalMsg(MSG_TONG_CREATE_SUCCESS);

	// 通知帮会频道 更新基本信息
	CoreDataChanged(GDCNI_PLAYER_BASE_INFO, 0, 0);

}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	功能：头上是否需要顶找人标志 ---是否有招人的权限
//-------------------------------------------------------------------------
BOOL	KPlayerTong::GetOpenFlag()
{//招人标志打开 并且职位不是帮众（是队长或 长老）
	return (m_nFlag && m_nFigure != enumTONG_FIGURE_MEMBER);
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	功能：转发加入帮会申请给对方客户端
//-------------------------------------------------------------------------
BOOL	KPlayerTong::TransferAddApply(DWORD dwNpcID)
{
	// 已经是帮会成员
	if (m_nFlag) // nguoi xin da la thanh vien bang
		return FALSE;

	// 自己的阵营问题
	if (Npc[Player[m_nPlayerIndex].m_nIndex].m_CurrentCamp != camp_free ||
		Npc[Player[m_nPlayerIndex].m_nIndex].m_Camp != camp_free) // nguoi xin ko phai chu do
		return FALSE;

	// 交易过程中
	if (Player[m_nPlayerIndex].CheckTrading())
		return FALSE;

	// 组队不能加入帮会
	if (Player[m_nPlayerIndex].m_cTeam.m_nFlag)
		return FALSE;

	if (Npc[Player[m_nPlayerIndex].m_nIndex].m_Level < PlayerSet.m_sTongParam.m_nLevel) // ko du level
		return FALSE;

	int	nTarget = Player[m_nPlayerIndex].FindAroundPlayer(dwNpcID);
	if (nTarget == -1)
		return FALSE;

	//int nMaxCount;

	//nMaxCount = _gameGlobalSet.SubWorldInfo.TongMaxCount;
	////g_GameSetTing.GetInteger("TongSet","MaxCount",10,&nMaxCount); //	 帮会的最大人数限制

	//if (Player[nTarget].m_cTong.m_CurRenShu >= nMaxCount)
	//{
	//	Player[m_nPlayerIndex].m_ItemList.msgshow("提示:<color=yellow>人数已满,取消申请<color>!");
	//	return FALSE;
	//}

	if (!Player[nTarget].m_cTong.CheckAcceptAddApplyCondition())  //检测对方是否有这个权限（同意 拒绝加帮）
	{
		Player[m_nPlayerIndex].m_ItemList.msgshow("提示:<color=yellow>对方权限不够,取消申请<color>!");
		return FALSE;
	}

	//int nReg = 0;
	//nReg = CheckAcceptLevel(Player[nTarget].m_cTong.m_szName, Npc[Player[m_nPlayerIndex].m_nIndex].m_Level);

	//if (nReg == 0)
	//{
	//	Player[m_nPlayerIndex].m_ItemList.msgshow("提示:<color=yellow>等级不够!,取消申请<color>!");
	//	return FALSE;
	//}

	m_nApplyTo = nTarget;   //锁定某人的playeridx为：是否有同意，拒绝加入帮派的权限

	//通知客户端有人发来申请加入帮派的申请了 开始申请列表文件
	TONG_APPLY_ADD_SYNC	sAdd;
	sAdd.ProtocolType = s2c_extendtong;
	sAdd.m_btMsgId = enumTONG_SYNC_ID_TRANSFER_ADD_APPLY;
	sAdd.m_nPlayerIdx = m_nPlayerIndex;  //申请人的 plauerid
	sAdd.m_IsAuToAdd = 1;
	snprintf(sAdd.m_szName, sizeof(sAdd.m_szName), "%s", Npc[Player[m_nPlayerIndex].m_nIndex].Name); //申请人 名称
	sAdd.m_wLength = sizeof(TONG_APPLY_ADD_SYNC) - 1 - sizeof(sAdd.m_szName) + strlen(sAdd.m_szName);
	if (g_pServer)
		g_pServer->PackDataToClient(Player[nTarget].m_nNetConnectIdx, &sAdd, sAdd.m_wLength + 1);
	
	//=============开始写申请列表==============================
	//KTabFileCtrl nTong;
	//KFile nFile;
	//char nTongApplyPath[125];
	//sprintf(nTongApplyPath, "%s%s.txt", TONG_DATA_FILE, Player[nTarget].m_cTong.m_szName);
	//if (!g_FileExists(nTongApplyPath))
	//{
	//	char szCol[64];
	//	nFile.Create(nTongApplyPath);
	//	sprintf(szCol, "编号\t姓名\t等级\15\n");
	//	nFile.Write(szCol, sizeof(szCol));
	//	//nFile.Save(nTongApplyPath);
	//}
	//nFile.Close();

	//if (nTong.Load(nTongApplyPath))
	//{
	//	int nRows, nIsHave = 0;
	//	nRows = nTong.GetHeight();
	//	//检测名字是否存在的
	//	char nName[32];

	//	for (int i = 0; i < nRows; i++)
	//	{
	//		nTong.GetString(i, 2, "", nName, sizeof(nName));
	//		if (strstr(nName, Npc[Player[m_nPlayerIndex].m_nIndex].Name))
	//		{
	//			nIsHave = 1;
	//			break;
	//		}
	//	}
	//	///////////////////
	//	if (nIsHave == 0)
	//	{
	//		nTong.InsertAfter(nRows);
	//		nTong.WriteInteger(nRows, 1, nRows);                                     //行号
	//		nTong.WriteString(nRows, 2, Npc[Player[m_nPlayerIndex].m_nIndex].Name);     //名字
	//		nTong.WriteInteger(nRows, 3, Npc[Player[m_nPlayerIndex].m_nIndex].m_Level); //等级
	//		nTong.Save(nTongApplyPath);
	//	}
	//}
	//nTong.Clear();
	//=========================================================

	return TRUE;
}

BOOL KPlayerTong::TransferInviteApply(DWORD dwNpcID)
{
	int	nTarget = Player[m_nPlayerIndex].FindAroundPlayer(dwNpcID);
	//printf("DEBUG - Name: %s\n", Npc[Player[nTarget].m_nIndex].Name);
	if (nTarget == -1)
	{
		//printf("Not found!\n");
		return FALSE;
	}

	//int nReg = CheckAcceptLevel(Player[nTarget].m_cTong.m_szTongName, Npc[Player[m_nPlayerIndex].m_nIndex].m_Level);
	//if (nReg == 0)
	//{
	//	Player[m_nPlayerIndex].m_ItemList.msgshow((char*)strClientInfo["message3150"].c_str());
	//	return FALSE;
	//}

	if (Player[nTarget].m_cTong.m_nFlag) // dang trong bang hoi, ko the moi
	{
		//printf("Dang trong bang hoi!\n");
		return FALSE;
	}

	if (Npc[Player[nTarget].m_nIndex].m_CurrentCamp != camp_free || Npc[Player[nTarget].m_nIndex].m_Camp != camp_free) // ko phai chu do
	{
		//printf("Camp not free");
		return FALSE;

	}

	if (Npc[Player[nTarget].m_nIndex].m_Level < PlayerSet.m_sTongParam.m_nLevel) // ko du level
	{
		//printf("Thieu level");
		return FALSE;

	}

	if (Player[nTarget].CheckTrading()) // dang trading ko the moi
	{
		//printf("Dang trading");
		return FALSE;

	}

	if (Player[nTarget].m_cTeam.m_nFlag) // dang to doi, ko the moi
	{
		//printf("Dang to doi");
		return FALSE;
	}

	TONG_APPLY_INVITE_SYNC	sInvite;
	sInvite.ProtocolType = s2c_extendtong;
	sInvite.m_btMsgId = enumTONG_SYNC_ID_TRANSFER_ADD_INVITE;
	sInvite.m_nPlayerIdx = m_nPlayerIndex;  //申请人的 plauerid
	snprintf(sInvite.m_szName, sizeof(sInvite.m_szName), "%s", Npc[Player[m_nPlayerIndex].m_nIndex].Name); //申请人 名称
	sInvite.m_wLength = sizeof(TONG_APPLY_INVITE_SYNC) - 1 - sizeof(sInvite.m_szName) + strlen(sInvite.m_szName);
	if (g_pServer)
		g_pServer->PackDataToClient(Player[nTarget].m_nNetConnectIdx, &sInvite, sInvite.m_wLength + 1);

	return TRUE;
}

#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	功能：判断是否可以转发别人的加入帮会申请
//-------------------------------------------------------------------------
int  KPlayerTong::CheckAcceptLevel(char *nTongName,int nPlayerLevel)
{
	KIniFile nCheck;
	char nTongSetPath[128];
	int  nAcceptLevel,nAutoAddLevel;
	nCheck.Load(TONG_SET_FILE);
	if (!nCheck.IsSectionExist(nTongName))
	{
		nCheck.Clear();
		return 1;
	} 
	nCheck.GetInteger(nTongName,"RefuseLevel",0,&nAcceptLevel);  //拒绝的等级
	nCheck.GetInteger(nTongName,"ApplyLevel",0,&nAutoAddLevel);  //自动加入的等级
	if (nPlayerLevel>=nAcceptLevel && nPlayerLevel>nAutoAddLevel)
	{
		nCheck.Clear();
		return 2;
	}
	else if (nPlayerLevel>=nAcceptLevel)
	{
		nCheck.Clear();
		return 1;
	}

		nCheck.Clear();
		return 0;
}
//-------------------------------------------------------------------------
//	功能：判断是否可以转发别人的加入帮会申请
//-------------------------------------------------------------------------
BOOL	KPlayerTong::CheckAcceptAddApplyCondition()
{
	if (!m_nFlag || m_nFigure == enumTONG_FIGURE_MEMBER)
		return FALSE;

	return TRUE;
}
#endif

#ifndef _SERVER

void KPlayerTong::Active()
{
	
/*	ActiveLoop++;
	//if (!m_nFlag)
	//	return;
	if (m_nFigure == enumTONG_FIGURE_MASTER)
	{
		//defFuncShowNormalMsg(MSG_TONG_CANNOT_LEAVE1);
		//return FALSE;
	}
	if (m_nFigure == enumTONG_FIGURE_DIRECTOR)
	{
		//defFuncShowNormalMsg(MSG_TONG_CANNOT_LEAVE2);
		//return FALSE;
	}

   if (ActiveLoop>18 && ActiveLoop%18==0)
   {
	TONG_APPLY_GET_ATTACK	sAttackGet;
	sAttackGet.ProtocolType		= c2s_extendtong;
	sAttackGet.m_wLength		= sizeof(sAttackGet) - 1;
	sAttackGet.m_btMsgId		= enumTONG_COMMAND_ID_APPLY_ATTACK_GET;
	if (m_nFlag)
	   sAttackGet.m_dwTongNameID= Player[CLIENT_PLAYER_INDEX].m_cTong.m_dwTongNameID; //g_FileName2Id(this->m_szName);
	else
	   sAttackGet.m_dwTongNameID= 0;

	sAttackGet.m_NpcDwid        = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_dwID;
	if (g_pClient)
		g_pClient->SendPackToServer(&sAttackGet, sAttackGet.m_wLength + 1);
   }

   if (ActiveLoop>30000000)
	   ActiveLoop=0; */
}

void KPlayerTong::SendAttack(char *lpszTongName)
{	
	if (!m_nFlag)
	{
		defFuncShowNormalMsg("提示:只有 加入帮派 才可以发起帮会宣战!");
		return;
	}
	if (m_nFigure== enumTONG_FIGURE_MASTER || m_nFigure == enumTONG_FIGURE_DIRECTOR)
	{
		TONG_APPLY_SEND_ATTACK	sAttackSend;
		sAttackSend.ProtocolType	= c2s_extendtong;
		sAttackSend.m_wLength		= sizeof(sAttackSend) - 1;
		sAttackSend.m_btMsgId		= enumTONG_COMMAND_ID_APPLY_ATTACK_SEND;
		sAttackSend.m_dwYTongNameID	= Player[CLIENT_PLAYER_INDEX].m_cTong.m_dwTongNameID;        //g_FileName2Id(this->m_szName);
		sAttackSend.m_dwDTongNameID = g_FileName2Id(lpszTongName);
		sAttackSend.m_NpcDwid       = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_dwID;
		if (g_pClient)
			g_pClient->SendPackToServer(&sAttackSend, sAttackSend.m_wLength + 1);
	}
    else
	{
		defFuncShowNormalMsg("提示:只有 帮主 或 长老 才可以发起帮会宣战!");
	}
	return;
}

void    KPlayerTong::SetTongLevel(char *lpszTongName,int nLevel)
{

}

void    KPlayerTong::SetTongMap(char *lpszTongName,int nMap)
{	
	
}

void	KPlayerTong::SetTongCredit(char *lpszTongName,int nCredit)        //建设资金
{

}
void	KPlayerTong::SetTongMasterTitle(char *lpszTongName,char *nTitle)  //帮主称号
{

}
void	KPlayerTong::SetTongDirectorTitle(char *lpszTongName,char *nTitle)//长老称号
{

}
void	KPlayerTong::SetTongManagerTitle(char *lpszTongName,char *nTitle)//队长称号
{

}
void	KPlayerTong::SetTongNormalTitle(char *lpszTongName,char *nTitle)  //帮众称号
{

}

//-------------------------------------------------------------------------
//	功能：是否接受成员 bFlag == TRUE 接受 == FALSE 不接受
//-------------------------------------------------------------------------
void	KPlayerTong::AcceptMember(int nPlayerIdx, DWORD dwNameID, BOOL bFlag)
{
	if (nPlayerIdx <= 0)
		return;

	TONG_ACCEPT_MEMBER_COMMAND	sAccept;
	sAccept.ProtocolType	= c2s_extendtong;
	sAccept.m_wLength		= sizeof(TONG_ACCEPT_MEMBER_COMMAND) - 1;
	sAccept.m_btMsgId		= enumTONG_COMMAND_ID_ACCEPT_ADD;
	sAccept.m_nPlayerIdx	= nPlayerIdx;   // 申请的Playerid
	sAccept.m_dwNameID		= dwNameID;     // 申请人的名字ID
	sAccept.m_btFlag		= (bFlag != 0);

	if (g_pClient)
		g_pClient->SendPackToServer(&sAccept, sAccept.m_wLength + 1);
}
#endif

#ifdef _SERVER


void KPlayerTong::Active()
{//获取宣战信息
	if (!m_nFlag)
		return;
	if (m_nFigure == enumTONG_FIGURE_MASTER)
	{
		//defFuncShowNormalMsg(MSG_TONG_CANNOT_LEAVE1);
		//return FALSE;
	}
	if (m_nFigure == enumTONG_FIGURE_DIRECTOR)
	{
		//defFuncShowNormalMsg(MSG_TONG_CANNOT_LEAVE2);
		//return FALSE;
	}	

}

//-------------------------------------------------------------------------
//	功能：发消息通知拒绝某人申请  nPlayerIdx 申请人的id   m_nPlayerIndex为被申请的id
//-------------------------------------------------------------------------
void	KPlayerTong::SendRefuseMessage(int nPlayerIdx, DWORD dwNameID)
{
	if (nPlayerIdx <= 0 || nPlayerIdx >= MAX_PLAYER)
	{
		//printf("---Error0:拒绝加入帮会(%u)OK------\n", dwNameID);
		return;
	}
	if (Player[nPlayerIdx].m_nIndex <= 0 || Player[m_nPlayerIndex].m_nIndex >= MAX_NPC)
	{
		//printf("---Error1:拒绝加入帮会(%u)OK------\n", dwNameID);
		return;
	}
	//if (g_FileName2Id(Npc[Player[nPlayerIdx].m_nIndex].Name) != dwNameID)
	//{
		//printf("---Error2:拒绝加入帮会(%u:%s)OK------\n", dwNameID, Npc[Player[nPlayerIdx].m_nIndex].Name);
		//return;
	//}
	SHOW_MSG_SYNC	sMsg;
	sMsg.ProtocolType = s2c_msgshow;
	sMsg.m_wMsgID = enumMSG_ID_TONG_REFUSE_ADD;
	sMsg.m_lpBuf = 0;
	snprintf(sMsg.m_Mmsgs, sizeof(sMsg.m_Mmsgs), "%s", Npc[Player[m_nPlayerIndex].m_nIndex].Name);
	sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1;
	if (g_pServer)
		g_pServer->PackDataToClient(Player[nPlayerIdx].m_nNetConnectIdx, (BYTE*)&sMsg, sMsg.m_wLength + 1);

	//	goto exit;
	//=========开始删除申请列表==================================
	//exit:
	//KTabFileCtrl nTong;
	//KFile nFile;
	//char nTongApplyPath[125];
	//sprintf(nTongApplyPath, "%s%s.txt", TONG_DATA_FILE, Player[m_nPlayerIndex].m_cTong.m_szName);
	//if (!g_FileExists(nTongApplyPath))
	//{
	//	char szCol[64];
	//	nFile.Create(nTongApplyPath);
	//	sprintf(szCol, "编号\t姓名\t等级\15\n");
	//	nFile.Write(szCol, sizeof(szCol));
	//	//nFile.Save(nTongApplyPath);
	//}
	//nFile.Close();

	//if (nTong.Load(nTongApplyPath))
	//{
	//	char nName[32];
	//	int nRows;
	//	nRows = nTong.GetHeight();
	//	for (int i = 0; i < nRows; i++)
	//	{
	//		nTong.GetString(i, 2, "", nName, sizeof(nName));
	//		if (strstr(nName, Npc[Player[nPlayerIdx].m_nIndex].Name))
	//			nTong.Remove(i);
	//	}
	//	nTong.Save(nTongApplyPath);
	//}
	//nTong.Clear();
	//	 return;
	//===========================================================
}
#endif

void	KPlayerTong::GetTongName(char *lpszGetName)
{
	//if (!lpszGetName)
	//	return;
	if (!m_nFlag)
	{
		lpszGetName[0] = 0;
		return;
	}

	snprintf(lpszGetName, sizeof(m_szName), "%s", m_szName);
}

#ifdef _SERVER
//-------------------------------------------------------------------------
//	功能：判断别人加入自己帮会条件是否成立
//-------------------------------------------------------------------------
BOOL	KPlayerTong::CheckAddCondition(int nPlayerIdx)
{
	//--------------------- 自己的条件 -----------------------
	if (nPlayerIdx <= 0 || nPlayerIdx >= MAX_PLAYER)
		return FALSE;

	if (!m_nFlag || m_nFigure == enumTONG_FIGURE_MEMBER)
		return FALSE;

	//--------------------- 对方的条件 -----------------------
		// 是否申请了
		/*if (Player[nPlayerIdx].m_cTong.m_nApplyTo != this->m_nPlayerIndex)
			return FALSE;*/
			//检测是否在申请列表中
	//KTabFile nTong;
	//char nTongApplyPath[125];
	//sprintf(nTongApplyPath, "%s%s.txt", TONG_DATA_FILE, Player[this->m_nPlayerIndex].m_cTong.m_szName);
	//int nReg = 0;
	//if (nTong.Load(nTongApplyPath))
	//{
	//	int Rows = nTong.GetHeight();
	//	for (int i = 0; i < Rows; i++)
	//	{
	//		char nName[32];
	//		nTong.GetString(i, "姓名", "", nName, sizeof(nName));
	//		if (strstr(Npc[Player[nPlayerIdx].m_nIndex].Name, nName))
	//		{
	//			nReg = 1;
	//			nTong.Clear();
	//			break;
	//		}
	//	}
	//}
	//else
	//{
	//	nTong.Clear();
	//	return FALSE;
	//}
	//nTong.Clear();
	//if (nReg == 0)
	//	return FALSE;
	// 已经是帮会成员
	if (Player[nPlayerIdx].m_cTong.m_nFlag)
		return FALSE;

	// 自己的阵营问题
	if (Npc[Player[nPlayerIdx].m_nIndex].m_CurrentCamp != camp_free || Npc[Player[nPlayerIdx].m_nIndex].m_Camp != camp_free)
		return FALSE;

	// 交易过程中
	if (Player[nPlayerIdx].CheckTrading())
		return FALSE;

	// 组队不能加入帮会
	if (Player[nPlayerIdx].m_cTeam.m_nFlag)
		return FALSE;

	if (Npc[Player[nPlayerIdx].m_nIndex].m_Level < PlayerSet.m_sTongParam.m_nLevel) // ko du level
		return FALSE;

	return TRUE;
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	功能：加入帮会，成为普通帮众
//-------------------------------------------------------------------------
BOOL	KPlayerTong::AddTong(int nCamp, char* lpszTongName, char* lpszMasterName, char* lpszTitleName)
{
	if (!lpszTongName || !lpszMasterName || !lpszTitleName)
		return FALSE;
	// 帮会阵营问题
	if (nCamp != camp_justice && nCamp != camp_evil && nCamp != camp_balance)
		return FALSE;

	m_nFlag = 1;
	m_nFigure = enumTONG_FIGURE_MEMBER;
	m_nCamp = nCamp;
	snprintf(this->m_szName, sizeof(this->m_szName), "%s", lpszTongName);
	strcpy(this->m_BMasterName, lpszMasterName);
	strcpy(this->m_szTitle, lpszTitleName);
	m_dwTongNameID = g_FileName2Id(lpszTongName);

	Npc[Player[m_nPlayerIndex].m_nIndex].m_Camp = m_nCamp;
	Npc[Player[m_nPlayerIndex].m_nIndex].m_CurrentCamp = m_nCamp;
	Npc[Player[m_nPlayerIndex].m_nIndex].m_btRankId = this->m_nFigure + 74;

	Npc[Player[m_nPlayerIndex].m_nIndex].SetCurrentCamp(m_nCamp);
	Npc[Player[m_nPlayerIndex].m_nIndex].SetCamp(m_nCamp);
	/*if (Player[m_nPlayerIndex].Save())	                     //生成数据包
	{
		//Player[m_nPlayerIndex].m_uMustSave = SAVE_REQUEST;	 //增加东西立即存档

		if (g_pDataClient)
		{//存档
			Player[m_nPlayerIndex].SavePlayerData(false);
		}
	}*/
	// 通知客户端
	TONG_Add_SYNC	sAdd;
	sAdd.ProtocolType = s2c_extendtong;
	sAdd.m_wLength = sizeof(sAdd) - 1;
	sAdd.m_btMsgId = enumTONG_SYNC_ID_ADD;
	sAdd.m_btCamp = this->m_nCamp;
	strcpy(sAdd.m_szTongName, lpszTongName);
	strcpy(sAdd.m_szTitle, m_szTitle);
	strcpy(sAdd.m_szMaster, lpszMasterName);
	printf("-----帮会:%s 帮主:%s 增加成员:%s OK------!\n", this->m_szName, this->m_BMasterName, Npc[Player[m_nPlayerIndex].m_nIndex].Name);
	if (g_pServer)
		g_pServer->PackDataToClient(Player[m_nPlayerIndex].m_nNetConnectIdx, &sAdd, sAdd.m_wLength + 1);
	
	//================开始删除申请列表==========================
	//KTabFileCtrl nTong;
	//KFile nFile;
	//char nTongApplyPath[125];
	//sprintf(nTongApplyPath, "%s%s.txt", TONG_DATA_FILE, lpszTongName);
	//if (!g_FileExists(nTongApplyPath))
	//{
	//	char szCol[64];
	//	nFile.Create(nTongApplyPath);
	//	sprintf(szCol, "编号\t姓名\t等级\15\n");
	//	nFile.Write(szCol, sizeof(szCol));
	//	//nFile.Save(nTongApplyPath);
	//}
	//nFile.Close();

	//if (nTong.Load(nTongApplyPath))
	//{
	//	char nName[32];
	//	int nRows;
	//	nRows = nTong.GetHeight();
	//	for (int i = 0; i < nRows; i++)
	//	{
	//		nTong.GetString(i, 2, "", nName, sizeof(nName));
	//		if (strstr(nName, Npc[Player[m_nPlayerIndex].m_nIndex].Name))
	//			nTong.Remove(i);
	//	}
	//	nTong.Save(nTongApplyPath);
	//}
	//nTong.Clear();
	//==========================================================

	return TRUE;
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	功能：加入帮会，成为普通帮众
//-------------------------------------------------------------------------
BOOL	KPlayerTong::AddTong(int nCamp, char *lpszTongName, char *lpszTitle, char *lpszMaster)
{
	if (!lpszTongName || !lpszTongName[0] || strlen(lpszTongName) > defTONG_NAME_MAX_LENGTH)
		return FALSE;
	// 帮会阵营问题
	if (nCamp != camp_justice && nCamp != camp_evil && nCamp != camp_balance)
		return FALSE;

	m_nFlag		= 1;
	m_nFigure	= enumTONG_FIGURE_MEMBER;
	m_nCamp		= nCamp;
	snprintf(m_szName, sizeof(m_szName), "%s", lpszTongName);
	if (!lpszTitle)
		this->m_szTitle[0] = 0;
	else
		strcpy(m_szTitle, lpszTitle);
	if (!lpszMaster)
		this->m_BMasterName[0] = 0;
	else
		strcpy(m_BMasterName, lpszMaster);
	m_dwTongNameID	= g_FileName2Id(lpszTongName);

	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Camp = m_nCamp;
	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentCamp = m_nCamp;

	defFuncShowNormalMsg(MSG_TONG_ADD_SUCCESS);

	return TRUE;
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	功能：申请任命
//-------------------------------------------------------------------------
BOOL	KPlayerTong::ApplyInstate(int nCurFigure, int nCurPos, int nNewFigure, int nNewPos, char *lpszName)
{
	if (!lpszName)
		return FALSE;
	if (!m_nFlag)
		return FALSE;
	if (nCurFigure < 0 || nCurFigure >= enumTONG_FIGURE_NUM ||
		nNewFigure < 0 || nNewFigure >= enumTONG_FIGURE_NUM)
		return FALSE;
	if (nCurPos < 0 || nNewPos < 0)
		return FALSE;
	if (nCurFigure == nNewFigure && nCurPos == nNewPos)
		return FALSE;

	switch (m_nFigure)
	{
	case enumTONG_FIGURE_MEMBER:
		// 帮众没有任命权力
		return FALSE;
	case enumTONG_FIGURE_MANAGER:
		// 队长没有任命权力
		return FALSE;
	case enumTONG_FIGURE_DIRECTOR:
		// 长老只有对队长、帮众的任命权力
		if ((nCurFigure != enumTONG_FIGURE_MANAGER && nCurFigure != enumTONG_FIGURE_MEMBER) ||
			(nNewFigure != enumTONG_FIGURE_MANAGER && nNewFigure != enumTONG_FIGURE_MEMBER))
			return FALSE;
		if (nCurFigure == enumTONG_FIGURE_MANAGER && nCurPos >= defTONG_MAX_MANAGER)
			return FALSE;
		if (nNewFigure == enumTONG_FIGURE_MANAGER && nNewPos >= defTONG_MAX_MANAGER)
			return FALSE;
		break;
	case enumTONG_FIGURE_MASTER:
		// 帮主有对长老、队长、帮众的任命权力
		if (nCurFigure == enumTONG_FIGURE_MASTER || nNewFigure == enumTONG_FIGURE_MASTER)
			return FALSE;
		if (nCurFigure == enumTONG_FIGURE_MANAGER && nCurPos >= defTONG_MAX_MANAGER)
			return FALSE;
		if (nCurFigure == enumTONG_FIGURE_DIRECTOR && nCurPos >= defTONG_MAX_DIRECTOR)
			return FALSE;
		if (nNewFigure == enumTONG_FIGURE_MANAGER && nNewPos >= defTONG_MAX_MANAGER)
			return FALSE;
		if (nNewFigure == enumTONG_FIGURE_DIRECTOR && nNewPos >= defTONG_MAX_DIRECTOR)
			return FALSE;
		break;
	default:
		return FALSE;
	}

	TONG_APPLY_INSTATE_COMMAND	sApply;

	if (strlen(lpszName) >= sizeof(sApply.m_szName))
		return FALSE;
	sApply.ProtocolType   = c2s_extendtong;
	sApply.m_btMsgId      = enumTONG_COMMAND_ID_APPLY_INSTATE;
	sApply.m_dwTongNameID = g_FileName2Id(this->m_szName);
	sApply.m_btCurFigure  = nCurFigure;
	sApply.m_btCurPos     = nCurPos;
	sApply.m_btNewFigure  = nNewFigure;
	sApply.m_btNewPos     = nNewPos;
	memset(sApply.m_szName, 0, sizeof(sApply.m_szName));
	snprintf(sApply.m_szName, sizeof(sApply.m_szName), "%s", lpszName);
	sApply.m_wLength      = sizeof(sApply) - 1;

	if (g_pClient)
		g_pClient->SendPackToServer(&sApply, sApply.m_wLength + 1);

	return TRUE;
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	功能：申请踢人
//-------------------------------------------------------------------------
BOOL	KPlayerTong::ApplyKick(int nCurFigure, int nCurPos, char *lpszName)
{
	if (!lpszName || !lpszName[0] || strlen(lpszName) >= 32)
		return FALSE;
	if (!m_nFlag)
		return FALSE;

	if (m_nFigure != enumTONG_FIGURE_MASTER && m_nFigure != enumTONG_FIGURE_DIRECTOR)   //执行踢人的职位
	{
		Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("提示:不能进行踢人操作!");
		return FALSE;
	}

	if (nCurFigure != enumTONG_FIGURE_MANAGER && nCurFigure != enumTONG_FIGURE_MEMBER)  //被踢的人的职位
	{
		Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("提示:不能对长老进行操作 !");
		return FALSE;
	}

	if (nCurFigure == enumTONG_FIGURE_MANAGER /*&& (nCurPos < 0 || nCurPos >= defTONG_MAX_MANAGER)*/)
	{
        Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("提示:不能对队长进行操作 !");
		return FALSE;//被踢人的职位
	}

	TONG_APPLY_KICK_COMMAND	sKick;
	sKick.ProtocolType		= c2s_extendtong;
	sKick.m_wLength			= sizeof(sKick) - 1;
	sKick.m_btMsgId			= enumTONG_COMMAND_ID_APPLY_KICK;
	sKick.m_btFigure		= nCurFigure;
	sKick.m_btPos			= nCurPos;
	sKick.m_dwTongNameID	= g_FileName2Id(this->m_szName);
	snprintf(sKick.m_szName, sizeof(sKick.m_szName), "%s", lpszName);

	if (g_pClient)
		g_pClient->SendPackToServer(&sKick, sKick.m_wLength + 1);
	char msg[64];
	snprintf(msg, sizeof(msg),"提示:已将(%s),踢出帮会!",lpszName);
    Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(msg);
	return TRUE;
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	功能：申请传位
//-------------------------------------------------------------------------
BOOL	KPlayerTong::ApplyChangeMaster(int nCurFigure, int nPos, char *lpszName)
{
	if (!lpszName || !lpszName[0] || strlen(lpszName) >= 32)
		return FALSE;
	if (!m_nFlag)
		return FALSE;
	if (m_nFigure != enumTONG_FIGURE_MASTER)
		return FALSE;

	TONG_APPLY_CHANGE_MASTER_COMMAND	sChange;
	sChange.ProtocolType	= c2s_extendtong;
	sChange.m_wLength		= sizeof(sChange) - 1;
	sChange.m_btMsgId		= enumTONG_COMMAND_ID_APPLY_CHANGE_MASTER;
	sChange.m_dwTongNameID	= g_FileName2Id(this->m_szName);
	sChange.m_btFigure		= nCurFigure;
	sChange.m_btPos			= nPos;
	snprintf(sChange.m_szName, sizeof(sChange.m_szName), "%s", lpszName);
	if (g_pClient)
		g_pClient->SendPackToServer(&sChange, sChange.m_wLength + 1);

	return TRUE;
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	功能：申请离开帮会 叛帮
//-------------------------------------------------------------------------
BOOL	KPlayerTong::ApplyLeave()
{
	if (!m_nFlag)
		return FALSE;
	if (m_nFigure == enumTONG_FIGURE_MASTER)
	{
		defFuncShowNormalMsg(MSG_TONG_CANNOT_LEAVE1);
		return FALSE;
	}
	if (m_nFigure == enumTONG_FIGURE_DIRECTOR)
	{
		defFuncShowNormalMsg(MSG_TONG_CANNOT_LEAVE2);
		return FALSE;
	}

	TONG_APPLY_LEAVE_COMMAND	sLeave;
	sLeave.ProtocolType		= c2s_extendtong;
	sLeave.m_wLength		= sizeof(sLeave) - 1;
	sLeave.m_btMsgId		= enumTONG_COMMAND_ID_APPLY_LEAVE;
	sLeave.m_btFigure		= m_nFigure;
	sLeave.m_btPos			= 0;
	sLeave.m_dwTongNameID	= g_FileName2Id(this->m_szName);
	snprintf(sLeave.m_szName, sizeof(sLeave.m_szName), "%s", Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name);

	if (g_pClient)
		g_pClient->SendPackToServer(&sLeave, sLeave.m_wLength + 1);

	return TRUE;
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	功能：申请获得帮会信息
//-------------------------------------------------------------------------
BOOL	KPlayerTong::ApplyInfo(int nInfoID, int nParam1, int nParam2, int nParam3, char *lpszName/*=NULL*/)
{
	if (nInfoID < 0 || nInfoID >= enumTONG_APPLY_INFO_ID_NUM)
		return FALSE;

	TONG_APPLY_INFO_COMMAND	sInfo;
	sInfo.ProtocolType = c2s_extendtong;
	sInfo.m_btMsgId = enumTONG_COMMAND_ID_APPLY_INFO;
	sInfo.m_btInfoID = nInfoID;
	sInfo.m_nParam1 = nParam1;	  //TONGID
	sInfo.m_nParam2 = nParam2;	  //开始位置
	sInfo.m_nParam3 = nParam3;    //每次获取数量

	switch (nInfoID)
	{
	case enumTONG_APPLY_INFO_ID_SELF:  //自己查询自己的帮会信息
		sInfo.m_wLength = sizeof(sInfo) - 1 - sizeof(sInfo.m_szBuf);
		if (g_pClient)   //gs
			g_pClient->SendPackToServer(&sInfo, sInfo.m_wLength + 1);
		break;

	case enumTONG_APPLY_INFO_ID_MASTER: //帮主
		break;

	case enumTONG_APPLY_INFO_ID_DIRECTOR: //长老
		break;

	case enumTONG_APPLY_INFO_ID_MANAGER:  // 队长
		if (!m_nFlag)
			break;
		/*if (this->m_nFigure != enumTONG_FIGURE_MASTER &&
			this->m_nFigure != enumTONG_FIGURE_DIRECTOR)
			break;*///只能是帮主和长老才更新

		sInfo.m_wLength = sizeof(sInfo) - 1 - sizeof(sInfo.m_szBuf);
		if (g_pClient)
			g_pClient->SendPackToServer(&sInfo, sInfo.m_wLength + 1);

		break;
	case enumTONG_APPLY_INFO_ID_LIST:
		{
			sInfo.m_wLength = sizeof(sInfo) - 1 - sizeof(sInfo.m_szBuf);
			if (g_pClient)
			g_pClient->SendPackToServer(&sInfo, sInfo.m_wLength + 1);
		}
		break;
	case enumTONG_APPLY_INFO_ID_MEMBER:   //帮众
		if (!m_nFlag)
			break;
		/*if (this->m_nFigure != enumTONG_FIGURE_MASTER &&
			this->m_nFigure != enumTONG_FIGURE_DIRECTOR)
			break;*///只能是帮主和长老才更新
            
		sInfo.m_wLength = sizeof(sInfo) - 1 - sizeof(sInfo.m_szBuf);
		if (g_pClient)
			g_pClient->SendPackToServer(&sInfo, sInfo.m_wLength + 1);

		break;

	case enumTONG_APPLY_INFO_ID_ONE:  //某人帮会成员的信息
		if (!lpszName || !lpszName[0])
			break;
		if (strlen(lpszName) >= 32)
			break;
		strcpy(sInfo.m_szBuf, lpszName);
		sInfo.m_wLength = sizeof(sInfo) - 1 - sizeof(sInfo.m_szBuf) + strlen(lpszName);
		if (g_pClient)
			g_pClient->SendPackToServer(&sInfo, sInfo.m_wLength + 1);
		break;
	case enumTONG_APPLY_INFO_ID_TONG_HEAD:  //获取帮主，长老等信息
		if (nParam1 <= 0 || nParam1 >= MAX_NPC)
			break;
		if (nParam1 != Player[CLIENT_PLAYER_INDEX].m_nIndex && Npc[nParam1].m_nTongFlag == 0)
			break;   
		sInfo.m_nParam1 = Npc[nParam1].m_dwID;
		sInfo.m_wLength = sizeof(sInfo) - 1 - sizeof(sInfo.m_szBuf);
		if (g_pClient)
			g_pClient->SendPackToServer(&sInfo, sInfo.m_wLength + 1);
		break;
	}
	return TRUE;
}
#endif

UINT	KPlayerTong::GetTongNameID()
{
///	return (m_nFlag ? m_dwTongNameID : 0);
	return m_dwTongNameID;
//	if (!m_nFlag)
//		return 0;
//	return g_FileName2Id(m_szName);
}

//-------------------------------------------------------------------------
//	功能：是否有权力查询队长信息
//-------------------------------------------------------------------------
BOOL	KPlayerTong::CanGetManagerInfo(DWORD dwTongNameID)
{
	if (!m_nFlag)
	{
		//printf("DEBUG, chua gia nhap bang\n");
		return FALSE;
	}
	if (dwTongNameID != m_dwTongNameID)
	{
		//printf("DEBUG, chua gia nhap bang\n");
		return FALSE;
	}

	//if (m_nFigure == enumTONG_FIGURE_MASTER || m_nFigure == enumTONG_FIGURE_DIRECTOR)
	//	return TRUE;
	
	return TRUE;
}

//-------------------------------------------------------------------------
//	功能：是否有权力查询帮众信息
//-------------------------------------------------------------------------
BOOL	KPlayerTong::CanGetMemberInfo(DWORD dwTongNameID)
{
	if (!m_nFlag)
		return FALSE;
	if (dwTongNameID != g_FileName2Id(this->m_szName))
		return FALSE;
	//if (m_nFigure == enumTONG_FIGURE_MASTER || m_nFigure == enumTONG_FIGURE_DIRECTOR)
	//	return TRUE;
	return TRUE;
}
//服务器端 同步给客户端
#ifdef _SERVER
//-------------------------------------------------------------------------
//	功能：给客户端同步自己在帮会中的信息
//-------------------------------------------------------------------------
void KPlayerTong::SendSelfInfo()
{
	TONG_SELF_INFO_SYNC	sInfo;
	sInfo.ProtocolType = s2c_extendtong;
	sInfo.m_wLength = sizeof(sInfo) - 1;
	sInfo.m_btMsgId = enumTONG_SYNC_ID_SELF_INFO;
	sInfo.m_btJoinFlag = this->m_nFlag;
	sInfo.m_btFigure = this->m_nFigure;
	sInfo.m_btCamp = this->m_nCamp;
	sInfo.m_dwMoney = this->m_nMoney;
	sInfo.m_nAttackState= this->m_AttackState;
	sInfo.m_AttackCamp= this->m_AttackCamp;
	sInfo.m_AttackTime= this->m_AttackTime;
	sInfo.m_nDeathCount=this->m_nDeathCount;
	sInfo.m_nLevel=this->m_nLevel;
	sInfo.m_dwTongNameID = this->m_dwTongNameID;

	strcpy(sInfo.m_AttackName, this->m_AttackName);
	strcpy(sInfo.m_szMaster, this->m_BMasterName);//m_szMasterName//m_BMasterName
	strcpy(sInfo.m_szTitle, this->m_szTitle);
	snprintf(sInfo.m_szTongName, sizeof(sInfo.m_szTongName), "%s", this->m_szName);

	//printf("DEBUG, Npc[Player[m_nPlayerIndex].m_nIndex].Name: %s\n", Npc[Player[m_nPlayerIndex].m_nIndex].Name);
	//printf("DEBUG, sInfo.m_dwMoney: %d\n", sInfo.m_dwMoney);

	if (g_pServer)
		g_pServer->PackDataToClient(Player[this->m_nPlayerIndex].m_nNetConnectIdx, &sInfo, sInfo.m_wLength + 1);
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	功能：设定自身帮会信息
//-------------------------------------------------------------------------
void	KPlayerTong::SetSelfInfo(TONG_SELF_INFO_SYNC *pInfo)
{
	if (pInfo->m_btJoinFlag == 0)
	{//没有入帮的
		if (m_nFlag)
		{//客户端原来是 帮会成员
			KUiGameObjectWithName	sUi;
			snprintf(sUi.szName, sizeof(sUi.szName), "%s", Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name);
			sUi.nData = TONG_ACTION_LEAVE;
			sUi.nParam = m_nFigure;
			sUi.uParam = 0;
			sUi.szString[0] = 0;
			CoreDataChanged(GDCNI_TONG_ACTION_RESULT, (unsigned int)&sUi, 1);

			// 被踢出帮会
			defFuncShowNormalMsg("提示:您被踢出帮会!");
			Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Camp = camp_free;
			if (!Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag)
				Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentCamp = camp_free;
		}

		Clear();

		CoreDataChanged(GDCNI_PLAYER_BASE_INFO, 0, 0);

		return;
	}

	if (m_nFlag == 1 && m_nFigure != pInfo->m_btFigure)
	{	// 被任命
		defFuncShowNormalMsg("提示:您在帮派中职位发生变化!");
	}

	this->m_nFlag = 1;                   //客户端是否入帮
	this->m_nFigure = pInfo->m_btFigure; //客户端帮派的身份
	this->m_nCamp = pInfo->m_btCamp;     //客户端帮派的阵营
	this->m_nMoney = pInfo->m_dwMoney;   //客户单帮派的钱
	this->m_AttackState= pInfo->m_nAttackState;	  //帮会的宣战状态
	this->m_nDeathCount= pInfo->m_nDeathCount;
	this->m_nLevel=pInfo->m_nLevel;
	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Camp = m_nCamp;

	if (Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag == 0)
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentCamp = m_nCamp;

	memcpy(this->m_BMasterName, pInfo->m_szMaster, sizeof(pInfo->m_szMaster)); //重新设置了帮主的名字
	memcpy(this->m_szName, pInfo->m_szTongName, sizeof(pInfo->m_szTongName));  //帮会名
	memcpy(this->m_szTitle, pInfo->m_szTitle, sizeof(pInfo->m_szTitle));       //帮会中的职位
	m_dwTongNameID	= g_FileName2Id(pInfo->m_szTongName);                      //帮会的代码
    //更新界面信息
	CoreDataChanged(GDCNI_PLAYER_BASE_INFO, 0, 0);
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	功能：检测是否有任命权利
//-------------------------------------------------------------------------
BOOL	KPlayerTong::CheckInstatePower(TONG_APPLY_INSTATE_COMMAND* pApply)
{
	if (!pApply)
		return FALSE;

	if (pApply->m_wLength <= sizeof(TONG_APPLY_INSTATE_COMMAND) - 1 - sizeof(pApply->m_szName))
		return FALSE;

	if (!m_nFlag)
		return FALSE;

	if (pApply->m_dwTongNameID != g_FileName2Id(m_szName))
		return FALSE;

	if (m_nFigure != enumTONG_FIGURE_MASTER && m_nFigure != enumTONG_FIGURE_DIRECTOR) // bang chu, truong lao(dc giao quyen)
		return FALSE;

	if (pApply->m_btCurFigure == pApply->m_btNewFigure && pApply->m_btCurPos == pApply->m_btNewPos)
		return FALSE;

	if (strlen(pApply->m_szName) >= NAME_LENGHT)
		return FALSE;

	if (m_nFigure == enumTONG_FIGURE_MASTER)
	{
		if (pApply->m_btCurFigure == enumTONG_FIGURE_DIRECTOR)
		{
			if (pApply->m_btCurPos >= defTONG_MAX_DIRECTOR)
				return FALSE;
		}
		else if (pApply->m_btCurFigure == enumTONG_FIGURE_MANAGER)
		{
			if (pApply->m_btCurPos >= defTONG_MAX_MANAGER)
				return FALSE;
		}
		else if (pApply->m_btCurFigure == enumTONG_FIGURE_MEMBER)
		{

		}
		else
		{
			return FALSE;
		}

		if (pApply->m_btNewFigure == enumTONG_FIGURE_DIRECTOR)
		{
			if (pApply->m_btNewPos >= defTONG_MAX_DIRECTOR)
				return FALSE;
		}
		else if (pApply->m_btNewFigure == enumTONG_FIGURE_MANAGER)
		{
			if (pApply->m_btNewPos >= defTONG_MAX_MANAGER)
				return FALSE;
		}
		else if (pApply->m_btNewFigure == enumTONG_FIGURE_MEMBER)
		{
		}
		else
		{
			return FALSE;
		}
	}
	else // if (m_nFigure == enumTONG_FIGURE_DIRECTOR)
	{
		if (pApply->m_btCurFigure == enumTONG_FIGURE_MANAGER)
		{
			if (pApply->m_btCurPos >= defTONG_MAX_MANAGER)
				return FALSE;
		}
		else if (pApply->m_btCurFigure == enumTONG_FIGURE_MEMBER)
		{
		}
		else
		{
			return FALSE;
		}

		if (pApply->m_btNewFigure == enumTONG_FIGURE_MANAGER)
		{
			if (pApply->m_btNewPos >= defTONG_MAX_MANAGER)
				return FALSE;
		}
		else if (pApply->m_btNewFigure == enumTONG_FIGURE_MEMBER)
		{
		}
		else
		{
			return FALSE;
		}
	}

	return TRUE;
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	功能：检测是否有踢人权利
//-------------------------------------------------------------------------
BOOL	KPlayerTong::CheckKickPower(TONG_APPLY_KICK_COMMAND *pKick)
{
	if (!pKick)
		return FALSE;

	if (pKick->m_wLength + 1 != sizeof(TONG_APPLY_KICK_COMMAND))
		return FALSE;

	if (!m_nFlag || (m_nFigure != enumTONG_FIGURE_MASTER && m_nFigure != enumTONG_FIGURE_DIRECTOR))
		return FALSE;

	if (pKick->m_dwTongNameID != g_FileName2Id(m_szName))
		return FALSE;

	if (pKick->m_btFigure == enumTONG_FIGURE_MASTER)
		return FALSE;

	if (pKick->m_btFigure == enumTONG_FIGURE_MANAGER)
	{
		if (pKick->m_btPos < 0 || pKick->m_btPos >= defTONG_MAX_MANAGER)
			return FALSE;
	}

	if (pKick->m_btFigure == enumTONG_FIGURE_DIRECTOR)
	{
		if (pKick->m_btPos < 0 || pKick->m_btPos >= defTONG_MAX_DIRECTOR)
			return FALSE;
	}

	return TRUE;
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	功能：被任命
//-------------------------------------------------------------------------
void	KPlayerTong::BeInstated(STONG_SERVER_TO_CORE_BE_INSTATED *pSync)
{
	this->m_nFlag = 1;
	this->m_nFigure = pSync->m_btFigure;
	strcpy(m_szTitle, pSync->m_szTitle);

	Npc[Player[m_nPlayerIndex].m_nIndex].m_btRankId = this->m_nFigure + 74;

	SendSelfInfo();

    if (Player[m_nPlayerIndex].Save())	                     //生成数据包
	{
		//Player[m_nPlayerIndex].m_uMustSave = SAVE_REQUEST;	 //增加东西立即存档
		
		if (g_pDataClient)
		{//存档
			Player[m_nPlayerIndex].SavePlayerData(false);
		}
	} 
}   
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	功能：被踢出帮会
//-------------------------------------------------------------------------
void	KPlayerTong::BeKicked(STONG_SERVER_TO_CORE_BE_KICKED *pSync)
{
	Clear();

	Npc[Player[m_nPlayerIndex].m_nIndex].m_Camp = camp_free;
	if (!Player[this->m_nPlayerIndex].m_cTeam.m_nFlag)
		Npc[Player[m_nPlayerIndex].m_nIndex].m_CurrentCamp = camp_free;

	Npc[Player[m_nPlayerIndex].m_nIndex].m_btRankId = 72;
	SendSelfInfo();

	if (Player[m_nPlayerIndex].Save())	                     //生成数据包
	{
		//Player[m_nPlayerIndex].m_uMustSave = SAVE_REQUEST;	 //增加东西立即存档
		
		if (g_pDataClient)
		{//存档
			Player[m_nPlayerIndex].SavePlayerData(false);
		}
	} 

	SHOW_MSG_SYNC	sMsg;
	// 被踢出帮会
	sMsg.ProtocolType = s2c_msgshow;
	sMsg.m_wMsgID = enumMSG_ID_TONG_BE_KICK;
	sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1;
	g_pServer->PackDataToClient(Player[m_nPlayerIndex].m_nNetConnectIdx,(BYTE *)&sMsg, sMsg.m_wLength + 1);
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	功能：检测是否有离开权利
//-------------------------------------------------------------------------
BOOL	KPlayerTong::CheckLeavePower(TONG_APPLY_LEAVE_COMMAND *pLeave)
{
	if (!pLeave)
		return FALSE;

	if (pLeave->m_wLength + 1 != sizeof(TONG_APPLY_LEAVE_COMMAND))
		return FALSE;

	if (!m_nFlag || m_nFigure == enumTONG_FIGURE_MASTER || m_nFigure == enumTONG_FIGURE_DIRECTOR)
		return FALSE;

	if (pLeave->m_dwTongNameID != g_FileName2Id(m_szName))
		return FALSE;

	return TRUE;
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	功能：离开帮会
//-------------------------------------------------------------------------
void	KPlayerTong::Leave(STONG_SERVER_TO_CORE_LEAVE *pLeave)
{
	if (strcmp(pLeave->m_szName, Npc[Player[m_nPlayerIndex].m_nIndex].Name) != 0)
		return;

	if (pLeave->m_bSuccessFlag)
	{
		Clear();
		Npc[Player[m_nPlayerIndex].m_nIndex].m_Camp = camp_free;
		if (!Player[this->m_nPlayerIndex].m_cTeam.m_nFlag)
			Npc[Player[m_nPlayerIndex].m_nIndex].m_CurrentCamp = camp_free;
		SendSelfInfo();

		if (Player[m_nPlayerIndex].Save())	                     //生成数据包
		{   
		   //Player[m_nPlayerIndex].m_uMustSave = SAVE_REQUEST;	 //增加东西立即存档
		
		   if (g_pDataClient)
		   { //存档 
			  Player[m_nPlayerIndex].SavePlayerData(false);
		   } 			 
		}  
	   
		SHOW_MSG_SYNC	sMsg;
		// 离开帮会成功
		sMsg.ProtocolType = s2c_msgshow;
		sMsg.m_wMsgID = enumMSG_ID_TONG_LEAVE_SUCCESS;
		sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1;
		g_pServer->PackDataToClient(Player[m_nPlayerIndex].m_nNetConnectIdx,(BYTE *)&sMsg, sMsg.m_wLength + 1);
	}
	else
	{
		SendSelfInfo();

		SHOW_MSG_SYNC	sMsg;
		// 离开帮会失败
		sMsg.ProtocolType = s2c_msgshow;
		sMsg.m_wMsgID = enumMSG_ID_TONG_LEAVE_FAIL;
		sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1;
		g_pServer->PackDataToClient(Player[m_nPlayerIndex].m_nNetConnectIdx,(BYTE *)&sMsg, sMsg.m_wLength + 1);
	}
 
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	功能：检测是否有权利换帮主
//-------------------------------------------------------------------------
BOOL	KPlayerTong::CheckChangeMasterPower(TONG_APPLY_CHANGE_MASTER_COMMAND *pChange)
{
	if (!pChange)
		return FALSE;
	if (pChange->m_wLength + 1 != sizeof(TONG_APPLY_CHANGE_MASTER_COMMAND))
		return FALSE;
	if (!m_nFlag || m_nFigure != enumTONG_FIGURE_MASTER)
		return FALSE;
	if (pChange->m_dwTongNameID != g_FileName2Id(m_szName))
		return FALSE;

	if (strlen(pChange->m_szName) >= NAME_LENGHT)
		return FALSE;

	//char	szName[32];
	//memcpy(szName, pChange->m_szName, sizeof(pChange->m_szName));
	//szName[31] = 0;
	//if (strlen(szName) >= 31)
	//	return FALSE;

	return TRUE;
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	功能：检测是否有能力接受传位
//-------------------------------------------------------------------------
BOOL	KPlayerTong::CheckGetMasterPower(STONG_SERVER_TO_CORE_CHECK_GET_MASTER_POWER *pCheck)
{
	if (!pCheck)
		return FALSE;
	if (!m_nFlag || m_nFigure == enumTONG_FIGURE_MASTER)
		return FALSE;
	if (pCheck->m_dwTongNameID != g_FileName2Id(m_szName))
		return FALSE;
//	if (m_nFigure != pCheck->m_btFigure)
//		return FALSE;
	if (strcmp(pCheck->m_szName, Npc[Player[m_nPlayerIndex].m_nIndex].Name) != 0)
		return FALSE;
	// 等级问题
	if (Npc[Player[m_nPlayerIndex].m_nIndex].m_Level < PlayerSet.m_sTongParam.m_nLevel || 
		(int)Player[m_nPlayerIndex].m_dwLeadLevel < PlayerSet.m_sTongParam.m_nLeadLevel)
		return FALSE;

	return TRUE;
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	功能：传位导致身份改变
//-------------------------------------------------------------------------
void	KPlayerTong::ChangeAs(STONG_SERVER_TO_CORE_CHANGE_AS *pAs)
{
	if (!pAs)
		return;
	if (!m_nFlag)
		return;
	if (pAs->m_dwTongNameID != g_FileName2Id(m_szName))
		return;

	m_nFigure = pAs->m_btFigure;
	strcpy(m_szTitle, pAs->m_szTitle);
	snprintf(this->m_BMasterName, sizeof(this->m_BMasterName), "%s", pAs->m_szName);

	this->SendSelfInfo();

	SHOW_MSG_SYNC	sMsg;

	sMsg.ProtocolType = s2c_msgshow;
	if (m_nFigure == enumTONG_FIGURE_MASTER)
		sMsg.m_wMsgID = enumMSG_ID_TONG_CHANGE_AS_MASTER;
	else
		sMsg.m_wMsgID = enumMSG_ID_TONG_CHANGE_AS_MEMBER;
	sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1;
	g_pServer->PackDataToClient(Player[m_nPlayerIndex].m_nNetConnectIdx,(BYTE *)&sMsg, sMsg.m_wLength + 1);

	return;
}

void KPlayerTong::ChangeTitle(STONG_SERVER_TO_CORE_CHANGE_TITLE* pChange)
{
	if (!pChange)
		return;
	if (!m_nFlag)
		return;
	if (pChange->m_dwTongNameID != g_FileName2Id(m_szName))
		return;

	strcpy(m_szTitle, pChange->m_szTitle);

	//SendSelfInfo();
}

#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	功能：换帮主
//-------------------------------------------------------------------------
void	KPlayerTong::ChangeMaster(char *lpszMaster)
{
	if (!lpszMaster || !lpszMaster[0])
		return;
	if (strlen(lpszMaster) >= sizeof(this->m_BMasterName))
		return;
	strcpy(m_BMasterName, lpszMaster);

	this->SendSelfInfo();
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	功能：登陆时候获得帮会信息
//-------------------------------------------------------------------------
void	KPlayerTong::Login(STONG_SERVER_TO_CORE_LOGIN *pLogin)
{
	if (strcmp(Npc[Player[this->m_nPlayerIndex].m_nIndex].Name, pLogin->m_szName) != 0)
	{
		printf("----Error游戏登陆开始设置(%s)帮会信息:%s 角色错误----\n",Npc[Player[this->m_nPlayerIndex].m_nIndex].Name,pLogin->m_szName);
		return;
	}
	if (pLogin->m_nFlag == 0)
	{//没有入帮的
	    if (m_nFlag)  //原来是有帮会的
		{  
		  m_nFlag			= 0;

		  Npc[Player[m_nPlayerIndex].m_nIndex].m_Camp = camp_free;	         //实际等于红名
		  if (!Player[m_nPlayerIndex].m_cTeam.m_nFlag)
		  { //没有组队
			Npc[Player[m_nPlayerIndex].m_nIndex].m_CurrentCamp =camp_free;// m_nCamp;	  //临时阵营 等于帮会的阵营
		  }  
		}
		Clear();
	    this->SendSelfInfo();  //同步信息都客户端
		printf("----游戏登陆开始设置(%s)帮会信息:%d 没有帮会----\n",pLogin->m_szName,pLogin->m_nFlag);
		return;
	}

	//入帮了的
	m_nFlag			= 1;
	m_nMoney		= pLogin->m_nMoney;
	m_nFigure		= pLogin->m_nFigure;
	m_nCamp			= pLogin->m_nCamp;	//帮会阵营
	Npc[Player[m_nPlayerIndex].m_nIndex].m_Camp = m_nCamp;	  //实际阵营

	if (pLogin->m_AttackState==1)
	{//如果是 宣战方 (蓝)
		pLogin->m_nTempCamp=7;
	   m_nCamp      = pLogin->m_nTempCamp;
	}
	else if (pLogin->m_AttackState==2)
	{//如果是 被宣战方(绿)
		pLogin->m_nTempCamp=8;
		m_nCamp     = pLogin->m_nTempCamp;	
	}
	else
	{

	}

	m_AttackState   = pLogin->m_AttackState;
	m_AttackCamp   = pLogin->m_nTempCamp;
	m_AttackTime   = pLogin->m_AttackTime;
	m_nDeathCount  = pLogin->m_nDeathCount;
	m_nLevel	   = pLogin->m_nLevel;
	strcpy(m_AttackName, pLogin->m_szAttackName);
	m_dwTongNameID	= g_FileName2Id(pLogin->m_szTongName);
	m_nApplyTo		= 0;  //取消申请加入帮派状态！
	snprintf(m_szName, sizeof(m_szName), "%s", pLogin->m_szTongName);
	strcpy(m_szTitle, pLogin->m_szTitle);
	strcpy(m_BMasterName, pLogin->m_szMaster);

	if (!Player[m_nPlayerIndex].m_cTeam.m_nFlag)
		Npc[Player[m_nPlayerIndex].m_nIndex].m_CurrentCamp = m_nCamp;
	this->SendSelfInfo();  //同步自己的帮派信息
	 printf("----游戏登陆开始设置(%s)帮会信息:%d|%s ----\n",pLogin->m_szName,pLogin->m_nFlag,pLogin->m_szTongName);
}
#endif
//打开建帮UI
#ifndef _SERVER
void    KPlayerTong::OpenCreateInterface()
{
	CoreDataChanged(GDCNI_OPEN_TONG_CREATE_SHEET, 1, 0);
}
//帮户存钱
BOOL KPlayerTong::ApplySaveMoney(DWORD nMoney)
{
	if (!m_nFlag)
		return FALSE;
	if (nMoney <= 0)
		return FALSE;
	if (Player[CLIENT_PLAYER_INDEX].m_ItemList.GetsMon(room_equipment) < nMoney)
	{
		KSystemMessage	sMsg;
		sprintf(sMsg.szMessage, "提示:您身上没有那么多钱呢!");
		sMsg.eType = SMT_NORMAL;
		sMsg.byConfirmType = SMCT_NONE;
		sMsg.byPriority = 0;
		sMsg.byParamSize = 0;
		sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		return FALSE;
	}

	TONG_APPLY_SAVE_COMMAND	sSave;
	sSave.ProtocolType		= c2s_extendtong;
	sSave.m_wLength			= sizeof(sSave) - 1;
	sSave.m_btMsgId			= enumTONG_COMMAND_ID_APPLY_SAVE;
	sSave.m_dwTongNameID	= Player[CLIENT_PLAYER_INDEX].m_cTong.m_dwTongNameID;//g_FileName2Id(Player[CLIENT_PLAYER_INDEX].m_cTong.m_szName);  //this->m_szName
	sSave.m_dwMoney			= nMoney;
	snprintf(sSave.m_szName, sizeof(sSave.m_szName), "%s", Npc[Player[m_nPlayerIndex].m_nIndex].Name);
	if (g_pClient)
		g_pClient->SendPackToServer(&sSave, sSave.m_wLength + 1);
	
	return TRUE;
}
//帮会取钱
BOOL KPlayerTong::ApplyGetMoney(DWORD nMoney)
{
	if (!m_nFlag)
		return FALSE;
	if (nMoney <= 0)
		return FALSE;
	if (m_nFigure != enumTONG_FIGURE_MASTER && m_nFigure != enumTONG_FIGURE_DIRECTOR)
		return FALSE;
	if (this->m_nMoney < nMoney)
	{
		KSystemMessage	sMsg;
		sprintf(sMsg.szMessage, "提示:金钱不足!");
		sMsg.eType = SMT_NORMAL;
		sMsg.byConfirmType = SMCT_NONE;
		sMsg.byPriority = 0;
		sMsg.byParamSize = 0;
        sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		return FALSE;
	}

	TONG_APPLY_SAVE_COMMAND	sSave;
	sSave.ProtocolType		= c2s_extendtong;
	sSave.m_wLength			= sizeof(sSave) - 1;
	sSave.m_btMsgId			= enumTONG_COMMAND_ID_APPLY_GET;
	sSave.m_dwTongNameID	=  Player[CLIENT_PLAYER_INDEX].m_cTong.m_dwTongNameID;//g_FileName2Id(this->m_szName);
	sSave.m_dwMoney			= nMoney;
	snprintf(sSave.m_szName, sizeof(sSave.m_szName), "%s", Npc[Player[m_nPlayerIndex].m_nIndex].Name);
	if (g_pClient)
		g_pClient->SendPackToServer(&sSave, sSave.m_wLength + 1);
	
	return TRUE;
}

//客户端
void    KPlayerTong::SetAttAckCityInfo(BYTE *Msg)
{ 
	  CTONG_CITY_INFO_SYNC	*pAinfo = (CTONG_CITY_INFO_SYNC*)Msg;

	  for (int i =1;i<pAinfo->m_WarCityCount+1;++i)
	  {  

		  nAttAckInfo[i].m_idx = pAinfo->snWarInfo[i-1].m_idx; 
		  nAttAckInfo[i].m_levle=pAinfo->snWarInfo[i-1].m_levle;
		  nAttAckInfo[i].m_mapidx=pAinfo->snWarInfo[i-1].m_mapidx;
		  nAttAckInfo[i].m_shushou=pAinfo->snWarInfo[i-1].m_shushou;
		  nAttAckInfo[i].m_Mastername = pAinfo->snWarInfo[i-1].m_Mastername;
		  nAttAckInfo[i].m_szMapName = pAinfo->snWarInfo[i-1].m_szMapName; //地图名
		  nAttAckInfo[i].m_Tongmaster = pAinfo->snWarInfo[i-1].m_Tongmaster;

		/*AttAck[i].m_idx=pAinfo->snWarInfo[i].m_idx;
		AttAck[i].m_levle=pAinfo->snWarInfo[i].m_levle;
		AttAck[i].m_mapidx=pAinfo->snWarInfo[i].m_mapidx;
		AttAck[i].m_shushou=pAinfo->snWarInfo[i].m_shushou;
		sprintf(AttAck[i].m_Mastername,pAinfo->snWarInfo[i].m_Mastername);
		sprintf(AttAck[i].m_szMapName,pAinfo->snWarInfo[i].m_szMapName); //地图名
		sprintf(AttAck[i].m_Tongmaster,pAinfo->snWarInfo[i].m_Tongmaster);
		*/
		// Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(AttAck[i].m_Tongmaster);
		/*if (m_nFlag && strstr(m_szName,AttAck[i].m_Tongmaster)) //如果入帮了
		   //如果帮会相同,就是属于这个城市的成员
		  Npc[Player[m_nPlayerIndex].m_nIndex].m_IsWarCity=AttAck[i].m_mapidx;
		else
		  Npc[Player[m_nPlayerIndex].m_nIndex].m_IsWarCity=-1; */

	  }  	
}


void    KPlayerTong::SetAttAckInfo(BYTE *Msg)
{
	CTONG_ATTTACK_INFO_SYNC	*pAinfo = (CTONG_ATTTACK_INFO_SYNC*)Msg;
   	/*
	for (int i =0;i<pAinfo->m_WarCityCount;i++)
	{
		AttAck[i].m_idx=pAinfo->snWarInfo[i].m_idx;
		AttAck[i].m_levle=pAinfo->snWarInfo[i].m_levle;
		AttAck[i].m_mapidx=pAinfo->snWarInfo[i].m_mapidx;
		AttAck[i].m_shushou=pAinfo->snWarInfo[i].m_shushou;
		sprintf(AttAck[i].m_Mastername,pAinfo->snWarInfo[i].m_Mastername);
		sprintf(AttAck[i].m_szMapName,pAinfo->snWarInfo[i].m_szMapName);
		sprintf(AttAck[i].m_Tongmaster,pAinfo->snWarInfo[i].m_Tongmaster);
	}
	*/
	m_AttackState=pAinfo->m_AttackState;
	m_AttackCamp=pAinfo->m_nTempCamp;
	m_AttackTime=pAinfo->m_AttackTime;
	m_nLevel=pAinfo->m_nLevel;
	m_nDeathCount=pAinfo->m_YDeathCount;
	m_nDDeathCount=pAinfo->m_DDeathCount;
	m_Mapid= pAinfo->m_nMapidx;
	m_nAttackNum=pAinfo->m_nAttackNum;	                            // 参战场数
	m_nWinNum=pAinfo->m_nWinNum;								// 胜利场数
	m_nLoseNum=pAinfo->m_nLoseNum;
	snprintf(m_AttackName, sizeof(m_AttackName), "%s", pAinfo->m_szAttackName); //对战的帮会

	if (pAinfo->m_nMapidx>0)
	{//客户端 帮会地图的设置
		//sprintf(SubWorld[pAinfo->m_nMapidx].m_WarSubTongname,m_szName);
		SubWorld[pAinfo->m_nMapidx].nWorldMapInfo[STR_MAP_ZTONGNAME] = m_szName;
	}
}

//帮会发钱
BOOL KPlayerTong::ApplySndMoney(DWORD nMoney)
{
	if (!m_nFlag)
		return FALSE;
	if (nMoney <= 0)
		return FALSE;
	if (m_nFigure != enumTONG_FIGURE_MASTER && m_nFigure != enumTONG_FIGURE_DIRECTOR)
		return FALSE;
	if (this->m_nMoney < nMoney)
	{
		KSystemMessage	sMsg;
		sprintf(sMsg.szMessage, "提示:金钱不足!");
		sMsg.eType = SMT_NORMAL;
		sMsg.byConfirmType = SMCT_NONE;
		sMsg.byPriority = 0;
		sMsg.byParamSize = 0;
		sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		return FALSE;
	}
	
	TONG_APPLY_SAVE_COMMAND	sSave;
	sSave.ProtocolType		= c2s_extendtong;
	sSave.m_wLength			= sizeof(sSave) - 1;
	sSave.m_btMsgId			= enumTONG_COMMAND_ID_APPLY_SND;
	sSave.m_dwTongNameID	= Player[CLIENT_PLAYER_INDEX].m_cTong.m_dwTongNameID;//g_FileName2Id(this->m_szName);
	sSave.m_dwMoney			= nMoney;
	snprintf(sSave.m_szName, sizeof(sSave.m_szName), "%s", Npc[Player[m_nPlayerIndex].m_nIndex].Name);
	if (g_pClient)
		g_pClient->SendPackToServer(&sSave, sSave.m_wLength + 1);
	
	return TRUE;
}
#endif

#ifdef _SERVER

BOOL KPlayerTong::CheckMoneyPower( TONG_APPLY_SAVE_COMMAND *pSave )
{
	if (!pSave)
		return FALSE;

	if (pSave->m_wLength + 1 != sizeof(TONG_APPLY_SAVE_COMMAND))
		return FALSE;

	if (!m_nFlag)
		return FALSE;

	if (m_nFigure != enumTONG_FIGURE_MASTER && m_nFigure != enumTONG_FIGURE_DIRECTOR)
		return FALSE;

	if (pSave->m_dwTongNameID != g_FileName2Id(m_szName))
		return FALSE;

	if (m_nMoney < pSave->m_dwMoney)
		return FALSE;
	
	return TRUE;
}

BOOL KPlayerTong::CheckSaveMoneyPower(TONG_APPLY_SAVE_COMMAND* pSave)
{
	if (!pSave)
		return FALSE;

	if (pSave->m_wLength + 1 != sizeof(TONG_APPLY_SAVE_COMMAND))
		return FALSE;

	if (!m_nFlag)
		return FALSE;

	if (pSave->m_dwTongNameID != g_FileName2Id(m_szName))
		return FALSE;

	if (Player[m_nPlayerIndex].m_ItemList.GetEquipmentMoney() < pSave->m_dwMoney)
		return FALSE;

	return TRUE;
}

void KPlayerTong::ChangeMoney( DWORD dwMoney )
{
	m_nMoney = dwMoney;
	
	this->SendSelfInfo();
}



void KPlayerTong::SetAttAckInfo(const char * pMsg)
{
   STONG_ATTACK_INFO_SYNC	*pSync = (STONG_ATTACK_INFO_SYNC*)pMsg; //接收数据
  /*
   for (int i =0;i<pSync->m_WarCityCount;i++)
   {
	   AttAck[i].m_idx=pSync->snWarInfo[i].m_idx;
	   AttAck[i].m_levle=pSync->snWarInfo[i].m_levle;
	   AttAck[i].m_mapidx=pSync->snWarInfo[i].m_mapidx;
	   AttAck[i].m_shushou=pSync->snWarInfo[i].m_shushou;
	   sprintf(AttAck[i].m_Mastername,pSync->snWarInfo[i].m_Mastername);
	   sprintf(AttAck[i].m_szMapName,pSync->snWarInfo[i].m_szMapName);
	   sprintf(AttAck[i].m_Tongmaster,pSync->snWarInfo[i].m_Tongmaster);
	}*/
  
   snprintf(m_AttackName, sizeof(m_AttackName), "%s", pSync->m_szAttackName);
   m_nLevel	= pSync->m_nLevel;
   m_nDeathCount=pSync->m_YDeathCount;
   m_nDDeathCount=pSync->m_YDeathCount;
   m_nAttackNum=pSync->m_nAttackNum;	                    // 参战场数
   m_nWinNum=pSync->m_nWinNum;								// 胜利场数
   m_nLoseNum=pSync->m_nLoseNum;
   m_Mapid= pSync->m_CurMapIdx;	   						    // 帮会地图
   
   if (pSync->m_CurMapIdx>0)
   {
	  //sprintf(SubWorld[pSync->m_CurMapIdx].m_WarSubTongname,m_szName);
	   SubWorld[pSync->m_CurMapIdx].nWorldMapInfo[STR_MAP_ZTONGNAME] = m_szName;
   }
}


//服务器端
void    KPlayerTong::SetAttAckCityInfo(const char * pMsg)
{ 
	CTONG_CITY_INFO_SYNC	*pAinfo = (CTONG_CITY_INFO_SYNC*)pMsg;
	
	for (int i =1;i<pAinfo->m_WarCityCount+1;++i)
	{  
		/*AttAck[i].m_idx=pAinfo->snWarInfo[i].m_idx;
		AttAck[i].m_levle=pAinfo->snWarInfo[i].m_levle;
		AttAck[i].m_mapidx=pAinfo->snWarInfo[i].m_mapidx;
		AttAck[i].m_shushou=pAinfo->snWarInfo[i].m_shushou;
		sprintf(AttAck[i].m_Mastername,pAinfo->snWarInfo[i].m_Mastername);
		sprintf(AttAck[i].m_szMapName,pAinfo->snWarInfo[i].m_szMapName); //地图名
		sprintf(AttAck[i].m_Tongmaster,pAinfo->snWarInfo[i].m_Tongmaster);
		// Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(AttAck[i].m_Tongmaster);
		if (m_nFlag && strstr(m_szName,AttAck[i].m_Tongmaster)) //如果入帮了
			//如果帮会相同,就是属于这个城市的成员
			Npc[Player[m_nPlayerIndex].m_nIndex].m_IsWarCity=AttAck[i].m_mapidx;*/

		  nAttAckInfo[i].m_idx=pAinfo->snWarInfo[i-1].m_idx;
		  nAttAckInfo[i].m_levle=pAinfo->snWarInfo[i-1].m_levle;
		  nAttAckInfo[i].m_mapidx=pAinfo->snWarInfo[i-1].m_mapidx;
		  nAttAckInfo[i].m_shushou=pAinfo->snWarInfo[i-1].m_shushou;
		  nAttAckInfo[i].m_Mastername = pAinfo->snWarInfo[i-1].m_Mastername;
		  nAttAckInfo[i].m_szMapName = pAinfo->snWarInfo[i-1].m_szMapName; //地图名
		  nAttAckInfo[i].m_Tongmaster = pAinfo->snWarInfo[i-1].m_Tongmaster;
		  // Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(AttAck[i].m_Tongmaster);
		  if (m_nFlag && strstr(m_szName,pAinfo->snWarInfo[i-1].m_Tongmaster)) //如果入帮了
			  //如果帮会相同,就是属于这个城市的成员
			Npc[Player[m_nPlayerIndex].m_nIndex].m_IsWarCity=nAttAckInfo[i].m_mapidx;
	
	}
	
	/*		 char msg[64];
			 
			 sprintf(msg,"占帮编号:%d",Npc[Player[m_nPlayerIndex].m_nIndex].m_IsWarCity);
			 
			 Player[m_nPlayerIndex].m_ItemList.msgshow(msg); */

}


void KPlayerTong::ChangeData(void* data)
{
	STONG_CHANGE_DATA_SYNC* pSync = (STONG_CHANGE_DATA_SYNC*)data;

	//printf("%d-%s\n", pSync->m_btParam, pSync->m_szParam);
	//printf("%d-%d\n", Npc[Player[m_nPlayerIndex].m_nIndex].m_nSex, m_nFigure);

	if (pSync->m_dwTongNameID == m_dwTongNameID)
	{
		switch (pSync->m_btMsgId)
		{
			case enumC2S_TONG_SET_CAM:
			{
				m_nCamp = pSync->m_btParam;
				m_nMoney -= PlayerSet.m_sTongSetParam.m_nCampFee;
				if (Player[m_nPlayerIndex].m_cTask.GetClearVal(200) == 1)
					return;
				Npc[Player[m_nPlayerIndex].m_nIndex].m_Camp = m_nCamp;
				if (!Player[m_nPlayerIndex].m_cTeam.m_nFlag)
					Npc[Player[m_nPlayerIndex].m_nIndex].m_CurrentCamp = m_nCamp;
			}
			break;
			case enumC2S_TONG_SET_ZONGTITLE:
			{
				if (m_nFigure != enumTONG_FIGURE_MEMBER)
					return;
				if (Npc[Player[m_nPlayerIndex].m_nIndex].m_nSex == pSync->m_btParam)
					strcpy(m_szTitle, pSync->m_szParam);
				else
					return;
			}
			break;
			case enumC2S_TONG_SET_LEVEL:
			{
				m_nLevel = pSync->m_btParam;
			}
			break;
			case enumC2S_TONG_SET_MAP:
				break;
		}
	}

	SendSelfInfo();
}

BOOL	KPlayerTong::CheckPowerPower(TONG_APPLY_POWER_COMMAND* pApply)
{
	if (!pApply)
		return FALSE;
	//if (pApply->m_wLength <= sizeof(TONG_APPLY_POWER_COMMAND) - 1 - sizeof(pApply->m_szName))
	//	return FALSE;
	if (!m_nFlag || (m_nFigure != enumTONG_FIGURE_MASTER))
		return FALSE;

	if (strlen(pApply->m_szName) >= NAME_LENGHT)
		return FALSE;

	return TRUE;
}

void KPlayerTong::UpdateLevel(int nLevel)
{
	STONG_UPDATE_LEVEL_COMMAND	sTong;
	sTong.ProtocolFamily = pf_tong;
	sTong.ProtocolID = enumC2S_TONG_UPDATE_LEVEL;  
	sTong.m_dwTongNameID = m_dwTongNameID;
	sTong.m_wLength = sizeof(sTong);
	sTong.m_nLevel = nLevel;
	sTong.m_btFigure = m_nFigure;
	snprintf(sTong.m_szName, sizeof(sTong.m_szName), "%s", Npc[Player[m_nPlayerIndex].m_nIndex].Name);
	
	if (g_pTongClient)
		g_pTongClient->SendPackToServer((const void*)&sTong, sTong.m_wLength);
}

BOOL KPlayerTong::CheckSetCondition(STONG_SERVER_TO_CORE_CHECK_SET* sCheck)
{
	if (!sCheck)
		return FALSE;

	if (!m_nFlag)
		return FALSE;

	switch (sCheck->ProtocolType)
	{
	case enumTONG_COMMAND_ID_APPLY_CHANGE_ANNOUNCE:
	{
		if (m_nFigure != enumTONG_FIGURE_MASTER && m_nFigure != enumTONG_FIGURE_DIRECTOR)
			return FALSE;
	}
	break;
	case enumC2S_TONG_SET_CAM:
	{
		if (sCheck->m_inParam == m_nCamp)
			return FALSE;
		if (m_nFigure != enumTONG_FIGURE_MASTER && m_nFigure != enumTONG_FIGURE_DIRECTOR)
			return FALSE;
		if (m_nMoney < PlayerSet.m_sTongSetParam.m_nCampFee)
			return FALSE;
	}
	break;
	case enumC2S_TONG_SET_BANGTITLE:
	{
		if (m_nFigure != enumTONG_FIGURE_MASTER)
			return FALSE;
		if (strlen(sCheck->m_strParam1) >= NAME_LENGHT || strlen(sCheck->m_strParam2) >= NAME_LENGHT)
			return FALSE;
	}
	break;
	case enumC2S_TONG_SET_ZHANGTITLE:
	{
		if (m_nFigure != enumTONG_FIGURE_MASTER && m_nFigure != enumTONG_FIGURE_DIRECTOR)
			return FALSE;

		if (strlen(sCheck->m_strParam2) >= NAME_LENGHT || strlen(sCheck->m_strParam1) >= NAME_LENGHT)
			return FALSE;

		if (sCheck->m_inParam < 0 || sCheck->m_inParam >= defTONG_MAX_DIRECTOR) // npos
			return FALSE;
	}
	break;
	case enumC2S_TONG_SET_DUITITLE:
	{
		if (m_nFigure != enumTONG_FIGURE_MASTER && m_nFigure != enumTONG_FIGURE_DIRECTOR)
			return FALSE;

		if (strlen(sCheck->m_strParam2) >= NAME_LENGHT || strlen(sCheck->m_strParam1) >= NAME_LENGHT)
			return FALSE;

		if (sCheck->m_inParam < 0 || sCheck->m_inParam >= defTONG_MAX_MANAGER) // npos
			return FALSE;
	}
	break;
	case enumC2S_TONG_SET_ZONGTITLE:
	{
		if (m_nFigure != enumTONG_FIGURE_MASTER && m_nFigure != enumTONG_FIGURE_DIRECTOR)
			return FALSE;

		if (strlen(sCheck->m_strParam2) >= NAME_LENGHT)
			return FALSE;
	}
	break;
	default:
		break;
	}

	return TRUE;
}

#endif




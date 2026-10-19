#include "KCore.h"
#ifndef WM_MOUSEHOVER
#define WM_MOUSEHOVER 0x02A1
#endif
#include "KEngine.h"
#ifdef _SERVER
//#include "KNetServer.h"
//#include "../MultiServer/Heaven/Interface/iServer.h"
#include "time.h"
#else
//#include "KNetClient.h"
#include "../../../Headers/IClient.h"
#include "Scene/KScenePlaceC.h"
#include "KIme.h"
#include "KNpcAI.h"

#endif
#include "KNpc.h"
#include "KObj.h"
#include "KNpcSet.h"
#include "KSubWorld.h"
#include "KPlayer.h"
#include "Text.h"

#include "LuaFuns.h"
#include "KSortScript.h"
#include "KScriptValueSet.h"
#include "KObjSet.h"
#include "KSkills.h"
#include "KPlayerSet.h"
#include "KSubWorldSet.h"
#include "KFile.h"
#include "GameDataDef.h"
#include "KBuySell.h"
//#include "MyAssert.h"
#include "MsgGenreDef.h"
#include "KItemSet.h"
#include "KTaskFuns.h"
#include "Text.h"
#include "KSellItem.h"
//KSellItem	g_PSellItem;
#include "KForBitGua.h"
#include <map>
#include <fstream>
#ifdef _SERVER
#ifndef _STANDALONE
#include "../../../lib/S3DBInterface.h"
#else
#include "S3DBInterface.h"
#endif
#else
#include "CoreShell.h"
#endif

extern int LuaGetNpcTalk(Lua_State *L);
extern void V9_ReleaseAllSessions(int);

#define		defPLAYER_LOGIN_TIMEOUT			20 * 20   // 10 sec


#define		PLAYER_LEVEL_1_EXP				48
#define		PLAYER_LEVEL_ADD_ATTRIBUTE		5
#define		PLAYER_LEVEL_ADD_SKILL			1
#define		PLAYER_SHARE_EXP_DISTANCE		768
#define		PLAYER_TEAM_EXP_ADD				50

#define		MAX_APPLY_TEAM_TIME				500

#define		BASE_WALK_SPEED					5    //走速
#define		BASE_RUN_SPEED					10   //跑速

#define		BASE_ATTACK_SPEED				0       //基本的攻击速度
#define		BASE_CAST_SPEED					0       //基本的出招速度
#define		BASE_VISION_RADIUS				120     //
#define		BASE_HIT_RECOVER				6

#define		TOWN_PORTAL_TIME				1800    //回城时间


KPlayer	*Player = NULL;//Player[MAX_PLAYER];
int		g_nLastNetMsgLoop;

//-------------------------------------------------------------------------
//	功能：构造函数
//-------------------------------------------------------------------------
KPlayer::KPlayer()
{
	Release();
}

//-------------------------------------------------------------------------
//	功能：析构函数
//-------------------------------------------------------------------------
KPlayer::~KPlayer()
{
	Release();
}

//-------------------------------------------------------------------------
//	功能：清空  退出游戏时候
//-------------------------------------------------------------------------
void	KPlayer::Release()
{
	m_nChestPW = 0;
	m_isGm = 0;
	m_IsInfindPathState=false;
	m_LastDialogNpc = 0;
    m_nLeftSkillID = 0;
	m_nLeftListidx = 0;
	m_nLeftSkillLevel = 0;
	m_nRightSkillID = 0;
	m_nRightListidx = 0;
	m_nRightSkillLevel = 0;
	m_nLimitMaxLevel = 0;
	__forBitUseTownPortal =0;
	__forBitTrade = 0;
#ifndef _SERVER
	m_DebugMode=FALSE;
	m_RunStatus = 0;
	m_dwNextLevelLeadExp = 0;
	/*m_nLeftSkillID = 0;
	m_nLeftSkillLevel = 0;
	m_nRightSkillID = 0;
	m_nRightSkillLevel = 0;*/
	m_nSendMoveFrames = defMAX_PLAYER_SEND_MOVE_FRAME;
	m_MouseDown[0] = FALSE;
	m_MouseDown[1] = FALSE;
	m_nImageNpcID = 0;
	m_ItemLinkDwid =0;
	//ZeroMemory(szClienResult,sizeof(szClienResult));

	m_MissionData.Clear();
	memset(m_MissionRank, 0, sizeof(m_MissionRank));

	m_nCurPinDao=-1;
#endif
	m_GameLeftTime = 0;
	m_Isinvalid=FALSE;
	m_dwID = 0;
	m_nIndex = 0;
	m_nNetConnectIdx = -1;
	m_autoTickCount = 0;
	m_cMenuState.Release();
	m_cTrade.Release();
	m_nAttributePoint = 0;
	m_nSkillPoint = 0;

	m_nStrength = 0;
	m_nDexterity = 0;
	m_nVitality = 0;
	m_nEngergy = 0;
	m_nLucky = 0;
	m_nCurStrength = 0;
	m_nCurDexterity = 0;
	m_nCurVitality = 0;
	m_nCurEngergy = 0;
	m_nCurLucky = 0;
	m_nTempLucky_p=0;
	m_btChatSpecialChannel = 0;
	m_nUpExp = 0;
	
	m_nExp = 0;
	m_nNextLevelExp = PLAYER_LEVEL_1_EXP;
	m_bExchangeServer = FALSE;
	m_CUnlocked = 0;  //默认解锁
	
	m_dwLeadExp = 0;
	m_dwLeadLevel = 1;
	
	m_nPeapleIdx = 0;
	m_nObjectIdx = 0;
//	m_bWaitingPlayerFeedBack = false;
//	m_btTryExecuteScriptTimes = 0;	//
    m_nForbiddenFlag = 0; //默认是解除禁言
	
#ifdef _SERVER
	m_IsFirstLogin       = 1;
	m_TempEnTerMapIndx = 0;          // 客户端传过来的临时登陆地图    
	m_TempKeyId = 0;                 // 重生点keyiD
	m_bUseReviveIdWhenLogin = 0;
	m_dwDeathScriptId = 0;
	m_dwDamageScriptId = 0;
	m_nGiveCallBackScript = 0;
	m_nGiveCallBackNum = 0;
	__isMobileClent      =false;
	__MobileClentLanguage=1;
	m_sLoginRevivalPos.m_nSubWorldID = 0;
	m_sLoginRevivalPos.m_nMpsX = 0;
	m_sLoginRevivalPos.m_nMpsY = 0;
	
	m_sDeathRevivalPos.m_nSubWorldID = 0;
	m_sDeathRevivalPos.m_nMpsX = 0;
	m_sDeathRevivalPos.m_nMpsY = 0;
	
	m_sPortalPos.m_nSubWorldId = 0;  //返回点
	m_sPortalPos.m_nTime = 0;
	m_sPortalPos.m_nMpsX = 0;
	m_sPortalPos.m_nMpsY = 0;
	
	//m_pLastScriptCacheNode = NULL;
	m_dwLoginTime			= -1;
	//	m_uLastPingTime			= -1;
	m_bFinishLoading = FALSE;
	m_uMustSave  = SAVE_IDLE;
	m_bIsQuiting = FALSE;
	m_isInGame   =false;
	m_CurSerVer  = 1;
	m_TempSerVer = 0;
	m_TimerTask.SetOwner(this);
	m_bSleepMode = FALSE;
	m_nLastNetOperationTime = 0;
	m_nPrePayMoney = 0;
	m_nPrePayXu = 0;
	
	m_nRoomFull =0;
	//m_nForbiddenFlag = 0;
	
	m_nChangeExtPoint = 0;
	m_nChangeExtTime  = 0;
	m_nLeftTime = 0;
	m_nCreatRoleTime = 0;
	m_nPlayerTime = -1;
	m_nPlayerTitleTime = 0;
    m_nVipType = 0;
	m_nLiXian = 0;
	m_nForbFlag=0;
	//ZeroMemory(m_PlayerIpInfo, sizeof(m_PlayerIpInfo));
	//sprintf(m_PlayerIpInfo,"127.0.0.1");
	m_uNextSaveTime=0;
	m_ItemList.ClearRoom(room_give);
#endif
	m_nExtPoint = 0;
	__forbitEnmity = 0;
}
//-------------------------------------------------------------------------
//	功能：设定 m_nPlayerIndex (本函数只允许在PlayerSet的Init中调用)
//-------------------------------------------------------------------------
void	KPlayer::SetPlayerIndex(int nNo)
{
	if (nNo < 0)
		m_nPlayerIndex = 0;
	else
		m_nPlayerIndex = nNo;
}

//-------------------------------------------------------------------------
//	功能：获得本实例在 Player 数组中的位置
//-------------------------------------------------------------------------
int		KPlayer::GetPlayerIndex()
{
	return m_nPlayerIndex;
}

#define 	defMAX_SLEEP_TIME	10800     //休眠时间设置 // 3600
//-------------------------------------------------------------------------
//	功能：玩家每次游戏循环都需要处理的东西
//-------------------------------------------------------------------------
void	KPlayer::Active(int  _minLimitLevel, int  _maxLimitLevel)
{

#ifdef _SERVER
	if (m_nNetConnectIdx == -1 || m_bExchangeServer)
		return;

	/*if (m_nVipType==0)
	{
	  if (m_GameLeftTime>0)
	  { //包月
		//if (g_SubWorldSet.GetGameTime()>>4)
		if (g_SubWorldSet.GetGameTime()-(g_SubWorldSet.GetGameTime()>>4<<4)==0)
		{
		   m_GameLeftTime--;

		   if (m_GameLeftTime<=0)
			 m_Isinvalid = TRUE;
		   else
			 m_Isinvalid = FALSE;

		  if (m_GameLeftTime<=540)
		  {
			  char nMsg[64]={0};
			  sprintf(nMsg,"提示:<c=gyellow>在游戏中还可以在线 %d<c> 秒!",m_GameLeftTime>>4);
			  m_ItemList.msgshow(nMsg);
		  }
		}
	  }
	  else
		m_Isinvalid = TRUE;

	  if  (m_Isinvalid)
	  {//发送时间到的提示
		  g_pServer->ShutdownClient(m_nNetConnectIdx);
	  }

	}*/

	if (m_nPlayerTime > 0)
	{
		m_nPlayerTime--;
		//printf("m_nPlayerTime: %d\n",m_nPlayerTime);
		if (m_nPlayerTime==0)
		{//称号的时间到期了
			//UseToolSkills(); //移除称号属性
			for (int i=0;i<5;++i)
			{
				int nSkillD=Npc[m_nIndex].m_ExtSkill[i];
				if (nSkillD>0)
				{//有技能ID,需要先去除状态
					Npc[m_nIndex].m_ExtSkill[i]=0;
					Npc[m_nIndex].SetSkillLeftTime(nSkillD,0); //同步客户端删除状态				
				}
			}

			//Npc[m_nIndex].m_NpcTitle = 0;
			Npc[m_nIndex].m_CurNpcTitle = 0;
		}
	}
		
	//PlayerTittle
	if (m_nPlayerTitleTime > 0)
	{
		if (m_nPlayerTitleTime <= KSG_GetCurSec())
		{
			//printf("Xoa Danh hieu ne: %d %d\n", m_nPlayerTitleTime, KSG_GetCurSec());
			for (int i=0;i<5;++i)
			{
				int nSkillD = Npc[m_nIndex].m_ExtSkill[i];
				if (nSkillD > 0) 
				{//有技能ID,需要先去除状态
					Npc[m_nIndex].m_ExtSkill[i]=0;
					Npc[m_nIndex].SetSkillLeftTime(nSkillD,0); //同步客户端删除状态				
				}
			}
			Npc[m_nIndex].m_NpcTitle = 0;
			m_nPlayerTitleTime = 0; 
		}
	}

	//时间任务的处理
	m_TimerTask.Activate(g_PlayerTimerCallBackFun);

	this->SendCurNormalSyncData();     //无限循环同步生命 内力等最大值
	
	this->m_cChat.TimerAdd();
	
	//仇杀倒计时
	this->m_cPK.Active();	           //无限循环仇杀

	//this->m_cTong.Active();            //无限循环宣战
	
	if (Npc[m_nIndex].m_FightMode == 0 && m_bSleepMode == FALSE && g_SubWorldSet.GetGameTime() - m_nLastNetOperationTime > defMAX_SLEEP_TIME)
	{
		NPC_SLEEP_SYNC	SleepSync;
		SleepSync.ProtocolType = s2c_npcsleepmode;
		SleepSync.bSleep = 1;
		SleepSync.NpcID = Npc[m_nIndex].m_dwID;
		Npc[m_nIndex].SendDataToNearRegion(&SleepSync, sizeof(NPC_SLEEP_SYNC));
		m_bSleepMode = TRUE;
	}

	if  (_minLimitLevel>0 && _maxLimitLevel>0)
	{//该地图限制了等级 就执行飞出该地图
		if  (Npc[m_nIndex].m_Level<_minLimitLevel || Npc[m_nIndex].m_Level>_maxLimitLevel)
		{
			///mapPos pos;
			//Npc[m_nIndex].GetMpsPos(&pos);
		   ExecuteScript_Direct("\\script\\global\\global_move.lua", "global_move",true,"dsddd", m_nIndex,Npc[m_nIndex].Name,Npc[m_nIndex].m_SubWorldIndex,_minLimitLevel,_maxLimitLevel);
		}
	}


//
#else
	// 队伍申请人的处理
	if ( !m_cTeam.m_nFlag )
	{
		if (m_cTeam.m_nApplyCaptainID > 0)
		{
			if ( m_cTeam.m_dwApplyTimer == 0 )
			{
				m_cTeam.m_nApplyCaptainID = 0;	
			}
			else
			{
				m_cTeam.m_dwApplyTimer--;
				if ( !NpcSet.SearchID(m_cTeam.m_nApplyCaptainID) )
				{
					m_cTeam.m_nApplyCaptainID = 0;
					m_cTeam.m_dwApplyTimer = 0;
				}
			}
		}
	}
	// 队长的处理
	else if (m_cTeam.m_nFigure == TEAM_CAPTAIN)
	{
		int i;
		for (i = 0; i < MAX_TEAM_APPLY_LIST; ++i)
		{
			if (m_cTeam.m_sApplyList[i].m_dwNpcID > 0)
			{
				if (m_cTeam.m_sApplyList[i].m_dwTimer == 0)
				{
					m_cTeam.m_sApplyList[i].m_dwNpcID = 0;
					m_cTeam.UpdateInterface();
					m_cTeam.UpdateamUI(); //显示主界面队伍信息
				}
				else
				{
					m_cTeam.m_sApplyList[i].m_dwTimer--;
					if ( !Npc[this->m_nIndex].SearchAroundID(m_cTeam.m_sApplyList[i].m_dwNpcID) )
					{
						m_cTeam.m_sApplyList[i].m_dwNpcID = 0;
						m_cTeam.m_sApplyList[i].m_dwTimer = 0;
						m_cTeam.UpdateInterface();
						m_cTeam.UpdateamUI(); //显示主界面队伍信息
					}
				}
			}
		}
	}
	
	++m_nSendMoveFrames;
	
	// 仇杀倒计时
	this->m_cPK.Active();

	this->m_cTong.Active();            //无限循环宣战

	
#endif
}


void	KPlayer::ProcessMsg(KWorldMsgNode *lpMsg)
{
	switch (lpMsg->m_dwMsgType)
	{
	case GWM_PLAYER_SKILL:
		break;
	case GWM_PLAYER_RUNTO:
		break;
	case GWM_PLAYER_WALKTO:
		break;
	case GWM_PLAYER_JUMPTO:
		break;
	default:
		break;
	}	
}

#ifndef _SERVER
void KPlayer::ProcessInputMsg(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_MOUSEMOVE:   //鼠标离开
	case WM_MOUSEHOVER:  //鼠标经过
		if (wParam & MK_LBUTTON)
			OnButtonMove(LOWORD(lParam), HIWORD(lParam),
			(wParam & ~(MK_LBUTTON | MK_MBUTTON | MK_RBUTTON)), button_left);
		else if (wParam & MK_RBUTTON)
			OnButtonMove(LOWORD(lParam), HIWORD(lParam),
			(wParam & ~(MK_LBUTTON | MK_MBUTTON | MK_RBUTTON)), button_right);
		else
			OnMouseMove(LOWORD(lParam), HIWORD(lParam));
		break;
		
	case WM_LBUTTONUP:   //左键上
		OnButtonUp(LOWORD(lParam), HIWORD(lParam), button_left);
		break;
		
	case WM_LBUTTONDOWN: //左键按下
		OnButtonDown(LOWORD(lParam), HIWORD(lParam), 
			(wParam & ~(MK_LBUTTON | MK_MBUTTON | MK_RBUTTON)), button_left);
		break;
		
	case WM_RBUTTONUP:   //右键上
		OnButtonUp(LOWORD(lParam), HIWORD(lParam), button_right);
		break;	
		
	case WM_RBUTTONDOWN: //右键按下
		OnButtonDown(LOWORD(lParam), HIWORD(lParam),
			(wParam & ~(MK_LBUTTON | MK_MBUTTON | MK_RBUTTON)), button_right);
		break;
	}
}
//按下鼠标的时候
void KPlayer::OnButtonDown(int x,int y, int Key, MOUSE_BUTTON nButton)
{
	m_MouseDown[(int)nButton] = TRUE;

	FindSelectNpc(x, y, relation_all);	 //赋值锁定 NPC 
	FindSelectObject(x, y);	             //赋值锁定 物品
	//	Npc[m_nIndex].m_nPeopleIdx = m_nPeapleIdx;
	ProcessMouse(x, y, Key, nButton);
}

void KPlayer::OnButtonMove(int x,int y,int Key, MOUSE_BUTTON nButton)
{
	if (m_MouseDown[(int)nButton])
	{
		ProcessMouse(x, y, Key, nButton);
	}
}

void KPlayer::OnButtonUp(int x,int y,MOUSE_BUTTON nButton)
{
	m_MouseDown[(int)nButton] = 0;
}
//客户端鼠标经过显示的图标
void KPlayer::OnMouseMove(int x,int y)
{
	m_nPeapleIdx = 0;
	m_nObjectIdx = 0;
	FindSelectNpc(x, y, relation_all);
	FindSelectObject(x, y);

	if (m_nPeapleIdx) //m_nPeapleIdx
	{
		if (Npc[m_nPeapleIdx].m_Kind == kind_dialoger)
		{
			//char nMsg[64];
			//sprintf(nMsg,"提示: %s 打开面板..", Npc[m_nPeapleIdx].Name);
			//m_ItemList.ClientShowMsg(nMsg);
			CoreDataChanged(GDCNI_SWITCH_CURSOR, NULL, MOUSE_CURSOR_DIALOG);
		}
		else if (Npc[m_nPeapleIdx].m_Kind == kind_player && Npc[m_nPeapleIdx].m_BaiTan)
		{
			//char nMsg[64];
			//sprintf(nMsg,"提示: %s 摆摊中..", Npc[m_nPeapleIdx].Name);
			//m_ItemList.ClientShowMsg(nMsg);
			CoreDataChanged(GDCNI_SWITCH_CURSOR, NULL, MOUSE_CURSOR_DIALOG);  //设置鼠标指针图标
		}
		else if (NpcSet.GetRelation(m_nIndex, m_nPeapleIdx) == relation_enemy)
			CoreDataChanged(GDCNI_SWITCH_CURSOR, NULL, MOUSE_CURSOR_FIGHT);
		else
			CoreDataChanged(GDCNI_SWITCH_CURSOR, NULL, MOUSE_CURSOR_NORMAL);
	}
	else if (m_nObjectIdx)
	{
		if (Object[m_nObjectIdx].m_nKind == Obj_Kind_MapObj)
			CoreDataChanged(GDCNI_SWITCH_CURSOR, NULL, MOUSE_CURSOR_DIALOG);
		else if (Object[m_nObjectIdx].m_nKind == Obj_Kind_Item || Object[m_nObjectIdx].m_nKind == Obj_Kind_Money)
			CoreDataChanged(GDCNI_SWITCH_CURSOR, NULL, MOUSE_CURSOR_PICK);
		else if (Object[m_nObjectIdx].m_nKind == Obj_Kind_Prop)
			CoreDataChanged(GDCNI_SWITCH_CURSOR, NULL, MOUSE_CURSOR_USE);
	}
	else
		CoreDataChanged(GDCNI_SWITCH_CURSOR, NULL, MOUSE_CURSOR_NORMAL);
}
//鼠标行为 鼠标线程
void KPlayer::ProcessMouse(int x, int y, int Key, MOUSE_BUTTON nButton)
{

	if (CheckTrading())
		return;

	if (m_ItemList.m_Hand > 0)
	{
		if (nButton == button_left) //左键
		{
			ThrowAwayItem();  //丢弃物品 丢掉物品
			return;
		}
	}

	int nX = x;
	int nY = y;
	int nZ = 0;
	g_ScenePlace.ViewPortCoordToSpaceCoord(nX, nY, nZ);
	
	/*	if (Key & MK_SHIFT)
	{
	Npc[m_nIndex].SendCommand(do_jump, nX, nY);
	SendClientCmdJump(nX, nY);
	return;
}*/
	
	if (Npc[m_nIndex].IsCanInput())
	{

		int nIdx = 0;
		
		if (nButton == button_right)
		{
			nIdx = Npc[m_nIndex].m_SkillList.FindSame(m_nRightSkillID);
			//g_DebugLog("[skill]right");
		}
		else
		{
			nIdx = Npc[m_nIndex].m_SkillList.FindSame(m_nLeftSkillID);
			//g_DebugLog("[skill]left");
		}
		Npc[m_nIndex].SetActiveSkill(nIdx);
	}
	else
	{
		//g_DebugLog("[skill]return");
		Npc[m_nIndex].m_nPeopleIdx = 0;
		return;
	}
	
	if ((Key & MK_SHIFT) || (nButton == button_right))  //强制攻击
	{
		if (Npc[m_nIndex].m_ActiveSkillID > 0)
		{
			ISkill * pISkill = (KSkill *) g_SkillManager.GetSkill(Npc[m_nIndex].m_ActiveSkillID, 1);
			if (!pISkill) 
                return;
			
			if (pISkill->IsAura())
				return;
			int nAttackRange = pISkill->GetAttackRadius();
			
			int nTargetIdx = 0;
			//			m_nPeapleIdx = 0;
			//按照Object / Enemy / Ally 的优先级找到需要打的对象id
			
			
			if (pISkill->IsTargetAlly())
			{
				FindSelectNpc(x, y, relation_ally);
				if (m_nPeapleIdx)
				{
					nTargetIdx = m_nPeapleIdx;
				}
			}
			
			if (pISkill->IsTargetEnemy())
			{
				FindSelectNpc(x, y, relation_enemy);

				if (m_nPeapleIdx)
				{
					nTargetIdx = m_nPeapleIdx;
				}
			}
			
			if (pISkill->IsTargetObj())
			{
				FindSelectObject(x, y);
				if (m_nObjectIdx)
				{
					nTargetIdx = m_nObjectIdx;
				}
			}
			//如果技能必须指定对象，而当前位置无对象的话，直接退出
			if (pISkill->IsTargetOnly() && !nTargetIdx)
            {
				Npc[m_nIndex].m_nPeopleIdx = 0;
				m_nPeapleIdx = 0;
				return;
			}
			
			if (m_nIndex == nTargetIdx)
			{
				Npc[m_nIndex].m_nPeopleIdx = 0;
				m_nPeapleIdx = 0;
				return;
			}
		  /*
			if (Npc[m_nIndex].m_SkillList.GetNextCastTime(Npc[m_nIndex].m_ActiveSkillID)>0 && (Npc[m_nIndex].m_SkillList.GetNextCastTime(Npc[m_nIndex].m_ActiveSkillID)>SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_dwCurrentTime))
			{//还没解冻
				return;
			}
			
			if (Npc[m_nIndex].m_HorseType)
			{//骑马时间限制
				if (Npc[m_nIndex].m_SkillList.GetHorseNextCastTime(Npc[m_nIndex].m_ActiveSkillID)>0 && (Npc[m_nIndex].m_SkillList.GetHorseNextCastTime(Npc[m_nIndex].m_ActiveSkillID)>SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_dwCurrentTime))
				return;
			} */
		
			if ((!Npc[m_nIndex].m_SkillList.CanCast(Npc[m_nIndex].m_ActiveSkillID, SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_dwCurrentTime))
				||(!Npc[m_nIndex].Cost(pISkill->GetSkillCostType() , pISkill->GetSkillCost(&Npc[m_nIndex]), TRUE)))
			{
				Npc[m_nIndex].m_nPeopleIdx = 0;
				m_nPeapleIdx = 0;
				return ;
			}
			
			//无对象，直接发坐标
			if (!nTargetIdx)
			{
				Npc[m_nIndex].SendSerCommand(do_skill, Npc[m_nIndex].m_ActiveSkillID, nX, nY);		
				SendClientCmdSkill(Npc[m_nIndex].m_ActiveSkillID, nX, nY);
                return ;   //无对象直接退出 不发技能
			}
			else
			{
				if (pISkill->IsTargetOnly())
				{
					int distance = NpcSet.GetDistance(m_nIndex , nTargetIdx);
					if (distance > pISkill->GetAttackRadius())
					{
						m_nPeapleIdx = nTargetIdx;
						return ;
					}
				}
				// 
				if (m_nIndex == nTargetIdx && pISkill->GetSkillStyle() == SKILL_SS_Missles) 
					return ;
				Npc[m_nIndex].SendSerCommand(do_skill, Npc[m_nIndex].m_ActiveSkillID, -1, nTargetIdx);
				// Send to Server		
				SendClientCmdSkill(Npc[m_nIndex].m_ActiveSkillID, -1, Npc[nTargetIdx].m_dwID);
			}
		}

		Npc[m_nIndex].m_nPeopleIdx = 0;
		return;
	}
	
	if (Key == 0 )
	{//没有键盘键时
		int nRelation = NpcSet.GetRelation(m_nIndex, m_nPeapleIdx);
		if(nRelation == relation_enemy || nRelation == relation_dialog ||(Npc[m_nPeapleIdx].m_Kind==kind_player && Npc[m_nPeapleIdx].m_BaiTan==1))
		{
			Npc[m_nIndex].m_nPeopleIdx = m_nPeapleIdx;
		}
		
		if (m_nSendMoveFrames >= defMAX_PLAYER_SEND_MOVE_FRAME)
		{
			m_nPickObjectIdx = m_nObjectIdx;
			Npc[m_nIndex].m_nObjectIdx = m_nPickObjectIdx;

			if (!m_RunStatus)
			{
				Npc[m_nIndex].SendSerCommand(do_walk, nX, nY);
				// Send to Server
				SendClientCmdWalk(nX, nY);
			}
			else
			{
				Npc[m_nIndex].SendSerCommand(do_run, nX, nY);
				// Send to Server
				SendClientCmdRun(nX, nY);
			}
			m_nSendMoveFrames = 0;
		}
		return;
	}
	
	/*	if (Key & MK_ALT)
	{
	Npc[m_nIndex].SendCommand(do_jump, nX, nY);
	SendClientCmdJump(nX, nY);
	}*/
}

void KPlayer::Walk(int nDir, int nSpeed)
{
	int	nMapX = Npc[m_nIndex].m_MapX;
	int nMapY = Npc[m_nIndex].m_MapY;
	int	nOffX = Npc[m_nIndex].m_OffX;
	int	nOffY = Npc[m_nIndex].m_OffY;
	int	nSubWorld = Npc[m_nIndex].m_SubWorldIndex;
	int	nRegion = Npc[m_nIndex].m_RegionIndex;
	int	nX, nY;
	
	SubWorld[nSubWorld].NewMap2Mps(nRegion, nMapX, nMapY, nOffX, nOffY, &nX, &nY);

	SubWorld[nSubWorld].GetMps(&nX, &nY, nSpeed * 2, nDir);
	
	if (m_RunStatus)
	{
		Npc[m_nIndex].SendSerCommand(do_run, nX, nY);
		// Send to Server
		if ( !CheckTrading() )
			SendClientCmdRun(nX, nY);
	}
	else
	{
		Npc[m_nIndex].SendSerCommand(do_walk, nX, nY);
		// Send to Server
		if (!CheckTrading())
			SendClientCmdWalk(nX, nY);
	}
}


void KPlayer::TurnLeft()
{
	if (Npc[m_nIndex].m_Doing != do_stand &&
		Npc[m_nIndex].m_Doing != do_sit)
		return;
	
	if (Npc[m_nIndex].m_Dir > 8)
		Npc[m_nIndex].m_Dir -= 8;
	else
		Npc[m_nIndex].m_Dir = MAX_NPC_DIR - 1;
}

void KPlayer::TurnRight()
{
	if (Npc[m_nIndex].m_Doing != do_stand &&
		Npc[m_nIndex].m_Doing != do_sit)
		return;
	
	if (Npc[m_nIndex].m_Dir < MAX_NPC_DIR - 9)
		Npc[m_nIndex].m_Dir += 8;
	else
		Npc[m_nIndex].m_Dir = 0;
}

void KPlayer::TurnBack()
{
	if (Npc[m_nIndex].m_Doing != do_stand &&
		Npc[m_nIndex].m_Doing != do_sit)
		return;
	
	if (Npc[m_nIndex].m_Dir < MAX_NPC_DIR / 2)
		Npc[m_nIndex].m_Dir += MAX_NPC_DIR / 2;
	else
		Npc[m_nIndex].m_Dir -= MAX_NPC_DIR / 2;
}

void KPlayer::FindSelectNpc(int x, int y, int nRelation)
{
	int	nNpcIdx = 0;
	
	    nNpcIdx = NpcSet.SearchNpcAt(x, y, nRelation, 16);

	if  (nNpcIdx>0)
	{
		if (Npc[nNpcIdx].m_Hide.nTime > 0 && NpcSet.GetRelation(m_nIndex,nNpcIdx) == relation_enemy)
			m_nPeapleIdx = 0; //只对敌人隐身
		else
		{
			m_nPeapleIdx  = nNpcIdx;
			m_nImageNpcID = nNpcIdx;
		} 
	}
	else
		m_nPeapleIdx = 0;
}

void KPlayer::FindSelectObject(int x, int y)
{
	int	nObjIdx = 0;
	
	nObjIdx = ObjSet.SearchObjAt(x, y, 40);
	if (nObjIdx)
		m_nObjectIdx = nObjIdx;
	else
		m_nObjectIdx = 0;
}
// need change
int	KPlayer::NetCommandPlayerTalk(BYTE * pProtocol)
{
/*	DWORD	dwNpcID;
int		nSize;
dwNpcID = *(DWORD *) &pProtocol[1];
nSize = pProtocol[5];
int nIdx = NpcSet.SearchID(dwNpcID);

  if (ConformIdx(nIdx))
  {
		strcpy(Npc[nIdx].m_szChatBuffer, (char *)(pProtocol+6));
		Npc[nIdx].m_nCurChatTime = NPC_SHOW_CHAT_TIME_LENGTH;
		}
		
		  return (7 + nSize);	// need test
	*/
	return 0;
}


BOOL KPlayer::ConformIdx(int nIdx)
{
	if (nIdx == m_nIndex || nIdx == 0)
		return FALSE;
	return TRUE;
}

#endif

#ifdef _SERVER
BOOL KPlayer::IsWaitingRemove()
{
	if (!this || !m_dwID)
		return FALSE;

	return m_bIsQuiting;
}
//设置游戏为离开状态
void KPlayer::WaitForRemove()
{
	m_bIsQuiting = TRUE;
}

BOOL KPlayer::IsLoginTimeOut()
{
	if (m_nNetConnectIdx != -1)
		return FALSE;
	
	if (!m_dwID)
		return FALSE;
	
	if (-1 != m_dwLoginTime && g_SubWorldSet.GetGameTime() - m_dwLoginTime > defPLAYER_LOGIN_TIMEOUT)
	{//已经登录的
		return TRUE;
	}
	
	return FALSE;
}

//超时登陆的
void KPlayer::LoginTimeOut()
{
    const int nSessionSlot = m_Node.m_nIndex;
    if (Player && nSessionSlot > 0 && nSessionSlot < MAX_PLAYER &&
        this == &Player[nSessionSlot])
        V9_ReleaseAllSessions(nSessionSlot);
	// 通知聊天好友自己下线了
	m_cChat.OffLine(m_dwID);
	
	m_pStatusLoadPlayerInfo = NULL;
	
	Release();
}

BOOL KPlayer::Save(BOOL nIsShow)
{
	if ((m_nIndex <= 0 && m_dwID == 0) || m_nNetConnectIdx==-1)
       return FALSE;


 	/*
	  if (m_uMustSave != SAVE_IDLE)
		return FALSE;
	*/
	ZeroMemory(m_GaemSaveBuffer, sizeof(m_GaemSaveBuffer));

	TRoleData* pData = (TRoleData *)m_GaemSaveBuffer;
	
	if (UpdateDBPlayerInfo((BYTE *)pData,sizeof(m_GaemSaveBuffer),nIsShow) == -1)   //存档，更新数据存档包
		return FALSE;
		/*
		#ifdef _DEBUG
		KFile File;
		char szFileName[32];
		sprintf(szFileName, "d:\\%sSave.Buf", m_PlayerName);
		File.Create(szFileName);
		File.Write(pData, pData->dwDataLen);
		File.Close();
		#endif
	*/

	//_ASSERT(pData->dwDataLen < sizeof(m_SaveBuffer));
	
	if (pData->dwDataLen <= 0)
		return FALSE;
	
	//m_dwTempDataLen	  =  pData->dwDataLen;
	//m_uMustSave = SAVE_REQUEST;
	m_ulLastSaveTime = g_SubWorldSet.GetGameTime();
	return TRUE;
}

BOOL KPlayer::CanSave()
{
	if (m_nNetConnectIdx < 0)
		return FALSE;
	
	if (m_nIndex <= 0)
		return FALSE;
	
	if (m_bExchangeServer)
		return FALSE;
	
	if (m_bIsQuiting)
		return FALSE;

	if (m_nLiXian)
	   return FALSE;

	if (CheckTrading())
	{
		return FALSE;
	}
	//超时　或　已经断线　不存档
	if  (IsLoginTimeOut() || m_uMustSave == SAVE_REQUEST)
		return FALSE;
	
	return TRUE;
}

BOOL KPlayer::SendSyncData(int &nStep, unsigned int &nParam)
{	
	BOOL bRet = FALSE;
	switch(nStep)
	{
	case STEP_BASE_INFO:
		{	
			bRet = SubWorld[Npc[m_nIndex].m_SubWorldIndex].SendSyncData(m_nIndex,m_nNetConnectIdx);
			if (!bRet)
			{
				printf("SubWorld同步世界地图失败 Fail.\n");        //同步世界地图
				break;
			}
			bRet = Npc[m_nIndex].SendSyncData(m_nNetConnectIdx);  // 同步NPC帮会等基本信息
			if (!bRet)
			{
				printf("世界NPC基本信息同步失败 Fail.\n");
				break;
			}
			// 这个消息必须在同步世界NPC数据后做，使客户端能找到当前玩家在客户端的Npc索引
			CURPLAYER_SYNC	sSync;	// 同步当前玩家的自身独特信息给客户端（装备等）
			sSync.ProtocolType = (BYTE)s2c_synonlineplayer;
			sSync.m_dwID       = Npc[m_nIndex].m_dwID;
			sSync.m_btLevel    = (WORD)Npc[m_nIndex].m_Level;
			sSync.m_btSex      = Npc[m_nIndex].m_nSex;
			sSync.m_btKind     = Npc[m_nIndex].m_Kind;
			sSync.m_btSeries   = Npc[m_nIndex].m_Series;
			sSync.m_wLifeMax   = Npc[m_nIndex].m_LifeMax;
			sSync.m_wNuqiMax   = Npc[m_nIndex].m_NuqiMax;  //怒气值
			sSync.m_wStaminaMax = Npc[m_nIndex].m_StaminaMax;
			sSync.m_wManaMax    = Npc[m_nIndex].m_ManaMax;
			sSync.m_HeadImage   = Npc[m_nIndex].m_HeadImage;
			sSync.m_wAttributePoint = this->m_nAttributePoint;
			sSync.m_wSkillPoint     = this->m_nSkillPoint;
			sSync.m_wStrength       = this->m_nStrength;
			sSync.m_wDexterity      = this->m_nDexterity;
			sSync.m_wVitality       = this->m_nVitality;
			sSync.m_wEngergy        = this->m_nEngergy;
			sSync.m_wLucky          = this->m_nLucky;//10
			sSync.m_nExp            = this->m_nExp;
		//	sSync.m_SUpExp = this->m_nUpExp;//0
			sSync.m_dwLeadExp       = this->m_dwLeadExp;
		//	sSync.m_btCUnlocked = this->m_CUnlocked; //是否锁频
			sSync.m_btCurFaction    = this->m_cFaction.m_nCurFaction;
			sSync.m_btFirstFaction  = this->m_cFaction.m_nFirstAddFaction;
			sSync.m_nFactionAddTimes = this->m_cFaction.m_nAddTimes;
			sSync.m_wWorldStat       = (WORD)m_nWorldStat;   //世界排名
			sSync.m_wSectStat        = (WORD)m_nSectStat;     //门派排名
			sSync.m_nMoney1          = this->m_ItemList.GetsMon(room_equipment);
			sSync.m_nMoney2          = this->m_ItemList.GetsMon(room_repository);
			sSync.m_nXu              = this ->m_ItemList.GetxLient(room_equipment);
			//sSync.n_dwLoginTime = this ->m_dwLoginTime;
			printf("--CURPLAYER_SYNC size:%zu --\n",sizeof(CURPLAYER_SYNC));
			if (SUCCEEDED(g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sSync, sizeof(CURPLAYER_SYNC))))
			{
                char nmTong[32],nMapName[64]={0};		  
				sprintf(nmTong,"%d",m_nWorldStat); //世界排名
				snprintf(nMapName, sizeof(nMapName), "%s", SubWorld[Npc[m_nIndex].m_SubWorldIndex].nWorldMapInfo[STR_MAP_NAME].c_str());
                ExecuteScriptC("\\script\\system\\online.lua","OnLine",nmTong,nMapName,m_IsFirstLogin,0,0,0,0,true);  //执行上线脚本
				bRet = TRUE;
			}
			else 
			{
				printf("player Packing sync data failed...\n");
				bRet = FALSE;
				break;
			}
		}
		break;
	case STEP_FIGHT_SKILL_LIST:	
		{
		  bRet = TRUE; 
		}
		break;
	case STEP_LIFE_SKILL_LIST:
		  bRet = TRUE;
		break;
	case STEP_ITEM_LIST:
		{
		  try
		  { 
			  SendSyncData_Skill();     //同步技能数据
			  bRet = TRUE;
		  }
		  catch (...)
		  {
			  bRet = TRUE;
		  } 
		}
		break;
	case STEP_TASK_LIST:
		bRet = TRUE;
		break;	
	case STEP_FRIEND_LIST:
		bRet = TRUE;
		break; 
	case STEP_SYNC_END:
		  bRet = TRUE;
		break;
	}
	if (!bRet)
		printf("Load PlayerInfo Step %d Failed...\n", nStep);

	return bRet;
}
//同步技能数据到客户端
BOOL KPlayer::SendSyncData_Skill()
{
	SKILL_SEND_ALL_SYNC		sSkill;	
	sSkill.ProtocolType = s2c_synccurplayerskill;
	sSkill.m_wProtocolLong = 2;
	int i;
	int j;
	for (i = 0,j = 0; i < MAX_NPCSKILL; ++i)
	{
		sSkill.m_sAllSkill[i].SkillId = 0;
		sSkill.m_sAllSkill[i].SkillLevel = 0;

		if( Npc[m_nIndex].m_SkillList.m_Skills[i].SkillId <= 0)
			continue;

		if (Npc[m_nIndex].m_SkillList.m_Skills[i].SkillId>g_SkillManager.GetSkillCount())
			continue;

		sSkill.m_sAllSkill[j].SkillId    = Npc[m_nIndex].m_SkillList.m_Skills[i].SkillId;
		sSkill.m_sAllSkill[j].SkillLevel = Npc[m_nIndex].m_SkillList.m_Skills[i].SkillLevel;
		sSkill.m_sAllSkill[j].SkillAdd   = Npc[m_nIndex].m_SkillList.m_Skills[i].mAddPoint;
		sSkill.m_sAllSkill[j].CurSkillExp= Npc[m_nIndex].m_SkillList.m_Skills[i].nCurSkillExp; //当前的熟练度
		//---------------服务端开始增加被动技能状态--------------------------------------
		KSkill * pSkill =(KSkill *)g_SkillManager.GetSkill(Npc[m_nIndex].m_SkillList.m_Skills[i].SkillId, Npc[m_nIndex].m_SkillList.m_Skills[i].SkillLevel);
		if (pSkill) 
		{
		   int nReg=0;
		       nReg = pSkill->NewCanCastSkill(m_nIndex,-1,m_nIndex);
		   if (pSkill->GetSkillStyle() == SKILL_SS_PassivityNpcState && nReg==1)
		   {//如果该技能属于被动辅助技能时，无武器限制的 则设置Npc状态  
			   pSkill->Cast(m_nIndex, -1, m_nIndex,-1);
		   } 
           else if  (pSkill->GetSkillStyle() == SKILL_SS_PassivityNpcState && nReg<=0)
		   {//空手 状态 有武器限制的被动技能 被清除！
		       pSkill->Cast(m_nIndex, -1, m_nIndex,1);
		   }
		} 
		pSkill=NULL;
		//-------------------------------------------------------------------------------
		j++;
	}

	 Npc[m_nIndex].m_SkillList.ReEnChance();

	sSkill.m_wProtocolLong += sizeof(SKILL_SEND_ALL_SYNC_DATA) * j;
	
	if (SUCCEEDED(g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sSkill, sSkill.m_wProtocolLong + 1)))
	{
		//m_ItemList.msgshow("提示:同步技能列表成功!");
		return TRUE;
	}
	else
	{
		printf("-------[技能同步]player Packing skill sync data failed...--------\n");
		return FALSE;
	}
}
//服务器端 同步 人物数据--无限循环
void	KPlayer::SendCurNormalSyncData()
{
	CURPLAYER_NORMAL_SYNC	sSync;
	
	sSync.ProtocolType   = s2c_synccurplayernormal;
	sSync.m_shLife       = Npc[m_nIndex].m_CurrentLife;
	sSync.m_shStamina    = Npc[m_nIndex].m_CurrentStamina;
	sSync.m_shMana       = Npc[m_nIndex].m_CurrentMana;
	sSync.m_shLifeMax    = Npc[m_nIndex].m_CurrentLifeMax;
	sSync.m_shStaminaMax = Npc[m_nIndex].m_CurrentStaminaMax;
	sSync.m_shManaMax    = Npc[m_nIndex].m_CurrentManaMax;
	//sSync.m_shAngry = Npc[m_nIndex].m_CurrentNuQi;  //当前怒值
	sSync.m_shAngryMax   = Npc[m_nIndex].m_CurrentNuQiMax;
	sSync.m_nForbiddenFlag = m_nForbiddenFlag;  //同步禁言

	if (!m_cTeam.m_nFlag)
		sSync.m_btTeamData = 0;
	else
	{
		if (m_cTeam.m_nFigure == TEAM_CAPTAIN)
			sSync.m_btTeamData = 0x03;
		else
			sSync.m_btTeamData = 0x01;
	}
	
	g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sSync, sizeof(CURPLAYER_NORMAL_SYNC));
}

//退出游戏存档(取消掉了)
void KPlayer::QuitGame(int nType)
{
	int nWorldMapid,nIsNoReturn=0; 
	    nWorldMapid = SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_SubWorldID;
    //int nReg = NpcSet.CheckThisNpc(nWorldMapid,"MapID");
	char nValKey[32]={0};
		sprintf(nValKey,"%d_IsNoReturn",nWorldMapid);
		g_NpcMapDropRate.GetInteger("List",nValKey,0,&nIsNoReturn);

    if (nIsNoReturn)
	{
		SetLoginType(1);
        SetRevivalPos(53,19);
		Npc[m_nIndex].m_GameliveTime = -1;
		Npc[m_nIndex].m_liveType = 0;
		//Npc[m_nIndex].m_FightMode=0;
		Npc[m_nIndex].SetFightMode(FALSE);
	}
 
	//Save();
}

void KPlayer::BuyItem(BYTE* pProtocol)
{
	PLAYER_BUY_ITEM_COMMAND* pCommand = (PLAYER_BUY_ITEM_COMMAND *)pProtocol;

	if (pCommand->m_shuishou==-1 && pCommand->m_CityMenber==-1&& pCommand->m_MapID==-1)
	{//拍卖行,玩家增加东西
		//printf("----Dau gia:%d ----\n",pCommand->m_BuyIdx);
		/*if (m_ItemList.GetPlayerItemCount()>=MAX_ITEMLIST_CONUT)
		{
			m_ItemList.msgshow("Dau gia:<c=yellow>物品超限,请清理储物空间<c>!");
			return;
		}*/

		if (pCommand->m_BuyIdx>0 && pCommand->m_BuyIdx<=nMaxSellCount)
		{//玩家开始增加东西

			if (iItemDataInfo.count(pCommand->m_BuyIdx)<=0)
			{
				m_ItemList.msgshow("Dau gia:<c=blue>Vat pham nay khong ton tai<c>");
				return;
			}

			if (pCommand->m_ItemNum<1)
				pCommand->m_ItemNum =1;

			int nItemClass = iItemDataInfo[pCommand->m_BuyIdx].iequipgen;
			int nIsStack   = iItemDataInfo[pCommand->m_BuyIdx].iparamj1;//是否可以叠加
			UINT nIsSellOut = iItemDataInfo[pCommand->m_BuyIdx].isSellOut;
			UINT nStartTime= iItemDataInfo[pCommand->m_BuyIdx].addtime;
			UINT nEndTime  = iItemDataInfo[pCommand->m_BuyIdx].endtime;
			int nTimeOut   = iItemDataInfo[pCommand->m_BuyIdx].iparamj2;//是否过期

			if  (nTimeOut >=1)
			{
				m_ItemList.msgshow("Dau gia:<c=yellow>Mat hang nay da het han<c>!");
				return;
			}

			if  (nIsSellOut >=1)
			{
				m_ItemList.msgshow("Dau gia:<c=yellow>Mat hang nay da duoc ban<c>!");
				return;
			}
			char nDataKey[32]={0};
			//sprintf(nDataKey,"剑侠%04情缘",pCommand->m_BuyIdx);
			sprintf(nDataKey, "kiem%04hiep", pCommand->m_BuyIdx);
			if (nStartTime > 0 && nEndTime>0)
			{//有时间限制的
				time_t rawtime;                  //定义一个long 型存放秒数
				time (&rawtime);                 //距离现在的时间（秒）
				if  (rawtime>nEndTime)
				{
					iItemDataInfo[pCommand->m_BuyIdx].iparamj2 = 1;
					if (g_DbItemFile)
					{//写数据库
						ZCursorS *item_cursor = g_DbItemFile->search(nDataKey,strlen(nDataKey)+1);	//pName		strlen(szName) + 1  主数据库查询
						if (item_cursor)
						{
							TDBShopItemData* pItemData = (TDBShopItemData*)item_cursor->data;
							if (pItemData && item_cursor->size >= (int)sizeof(TDBShopItemData))
							{//开始修改数据
								pItemData->iparamj2 = 1;
								if (g_DbItemFile->add(nDataKey,strlen(nDataKey)+1,item_cursor->data,sizeof(TDBShopItemData)))   //pName strlen(szName) +1
								{
									printf("---[Dau gia] Cap nhat du lieu hang het han thanh cong---/n");
								}
							}
						  g_DbItemFile->closeCursor(item_cursor);
						}
					}
					m_ItemList.msgshow("Dau gia:<c=yellow>Mat hang nay da het han<c>!");
					return;
				}
			}

			if (nItemClass==0)
			{//如果是装备
				if (pCommand->m_ItemNum!=1)
				{
					pCommand->m_ItemNum = 1;
					m_ItemList.msgshow("Dau gia: Muc nay khong the xep chong len nhau, khoi phuc du lieu goc!");
				}
			}
			else
			{//不是装备
				if  (!nIsStack && pCommand->m_ItemNum!=1)
				{//如果不能叠加的　
						m_ItemList.msgshow("Dau gia: Mat hang nay khong the duoc mua ket hop!");
						return;
				}
			}
			int nItemCount   = pCommand->m_ItemNum;
			int nItemPiceVal = iItemDataInfo[pCommand->m_BuyIdx].iSellPrice * nItemCount;
			int nItemPriceKind = iItemDataInfo[pCommand->m_BuyIdx].iPriceKind;

            if (nItemPriceKind==moneyunit_money)  //银两
			{
				if (m_ItemList.GetEquipmentMoney() <nItemPiceVal)
				{
					m_ItemList.msgshow("Dau gia: khong du tien!");
					return;
				} 
			}
			else if (nItemPriceKind==moneyunit_moneya)
			{
				if (m_ItemList.GetEquipmentMoney() <nItemPiceVal)
				{
					m_ItemList.msgshow("Dau gia: khong du tien! ");
					return;
				}
			}
			else if (nItemPriceKind==moneyunit_xu)
			{
				if (m_ItemList.GetEquipmentXu() < nItemPiceVal)
				{
					m_ItemList.msgshow("Dau gia: khong du xu!");
					return;
				}
			}
			else if (nItemPriceKind==moneyunit_fuyuan)
			{
				if (m_cFuYuan.GetFuYuanValue() < nItemPiceVal)
				{
					m_ItemList.msgshow("Dau gia: khong du diem phuc duyen");
					return;
				}
			}
			else if (nItemPriceKind==moneyunit_repute)
			{
				if (m_cRepute.GetReputeValue() < nItemPiceVal)
				{
					m_ItemList.msgshow("Dau gia: khong du diem danh vong");
					return;
				}
			}
			else if (nItemPriceKind==moneyunit_expoint)
			{
				//if (GetExtPoint() < nItemPiceVal)
				if (m_cTask.GetSaveVal(2051) < nItemPiceVal)
				{ 
					m_ItemList.msgshow("Dau gia: khong du diem vinh du");
					return;
				} 
			}
			else if (nItemPriceKind==moneyunit_jifen)
			{
				if (m_cTask.GetSaveVal(192) < nItemPiceVal)
				{
					m_ItemList.msgshow("Dau gia: khong du diem tong kim");
					return;
				}
			}
			else
			{
				m_ItemList.msgshow("Dau gia: Sai loai tien te");
			    return;
			}
			//----------------------------------开始产生东西 或者 执行脚本
			int nStackNum = nItemCount;
			int nSeries   = iItemDataInfo[pCommand->m_BuyIdx].iseries;
			int nLevel    = iItemDataInfo[pCommand->m_BuyIdx].ilevel;
			int nLuck     = iItemDataInfo[pCommand->m_BuyIdx].ilucky;
			int nDetailType     = iItemDataInfo[pCommand->m_BuyIdx].idetailtype;
			int nParticularType = iItemDataInfo[pCommand->m_BuyIdx].iparticulartype;
			int nItemLevel[6],nRItemLevel[6];
			ZeroMemory(&nItemLevel,sizeof(nItemLevel));
			ZeroMemory(&nRItemLevel,sizeof(nRItemLevel));
			nItemLevel[0] = iItemDataInfo[pCommand->m_BuyIdx].iparam1;
			nItemLevel[1] = iItemDataInfo[pCommand->m_BuyIdx].iparam2;
			nItemLevel[2] = iItemDataInfo[pCommand->m_BuyIdx].iparam3;
			nItemLevel[3] = iItemDataInfo[pCommand->m_BuyIdx].iparam4;
			nItemLevel[4] = iItemDataInfo[pCommand->m_BuyIdx].iparam5;
			nItemLevel[5] = iItemDataInfo[pCommand->m_BuyIdx].iparam6;

			nRItemLevel[0] = iItemDataInfo[pCommand->m_BuyIdx].iparamr1;
			nRItemLevel[1] = iItemDataInfo[pCommand->m_BuyIdx].iparamr2;
			nRItemLevel[2] = iItemDataInfo[pCommand->m_BuyIdx].iparamr3;
			nRItemLevel[3] = iItemDataInfo[pCommand->m_BuyIdx].iparamr4;
			nRItemLevel[4] = iItemDataInfo[pCommand->m_BuyIdx].iparamr5;
			nRItemLevel[6] = iItemDataInfo[pCommand->m_BuyIdx].iparamr6;
			UINT nRandSeed = iItemDataInfo[pCommand->m_BuyIdx].irandseed;
            int  nEnChance = iItemDataInfo[pCommand->m_BuyIdx].ienchance;
			int  nPoint    = iItemDataInfo[pCommand->m_BuyIdx].ipoint;
			int  IsWhere   = iItemDataInfo[pCommand->m_BuyIdx].iswhere;
			int  nRongPoint= iItemDataInfo[pCommand->m_BuyIdx].iRongpiont;
			int  nGoldIdx  = iItemDataInfo[pCommand->m_BuyIdx].igoldid;
			int  nPlatina  = iItemDataInfo[pCommand->m_BuyIdx].isplatina;
			int nIndex = 0;

			if (nGoldIdx<=0)
			{//蓝装装
				 nIndex = ItemSet.AddOther(nItemClass, nSeries, nLevel, nLuck, nDetailType, nParticularType,nItemLevel, g_SubWorldSet.GetGameVersion(),nRandSeed,nItemCount,nEnChance,nPoint,\
				  0,0,0,0,0,nRongPoint,nRItemLevel,NULL,NULL,IsWhere,TRUE);
			}
			else
			{//其他装备　黄金装备
			  m_ItemList.msgshow("Dau gia: Loi nGoldIdx vui long bao GM");
			  return;
			}

			if (nIndex <= 0)
			{
				return ;
			}
			m_ItemList.msgshow("Dau gia:<c=yellow> Item loi vui long bao GM<c>!");
			//设置节点已经购买过
			 time_t nthistime;
			 time(&nthistime);
			 iItemDataInfo[pCommand->m_BuyIdx].isSellOut = nthistime;

			 if (g_DbItemFile)
			 {//写数据库
				 ZCursorS *item_cursor = g_DbItemFile->search(nDataKey,strlen(nDataKey)+1);	//pName		strlen(szName) + 1  主数据库查询
				 if (item_cursor)
				 {
					 TDBShopItemData* pItemData = (TDBShopItemData*)item_cursor->data;
					 if (pItemData && item_cursor->size >= (int)sizeof(TDBShopItemData))
					 {//开始修改数据
						 pItemData->isSellOut = nthistime;
						 if (g_DbItemFile->add(nDataKey,strlen(nDataKey)+1,item_cursor->data,sizeof(TDBShopItemData)))   //pName strlen(szName) +1
						 {
							 printf("---[Dau gia] Cap nhat thanh cong du lieu cac mat hang da ban---/n");
						 }
					 }
				   g_DbItemFile->closeCursor(item_cursor);
				 }
			 }
			//更新数据库数据　或者删除节点　或者　删除数据库
			 //暂时测试　无处理
			//----------------开始扣钱------------------------------
			 if (nItemPriceKind==moneyunit_money)  //银两
			 {
				Pay(nItemPiceVal);
			 }
			 else if (nItemPriceKind==moneyunit_moneya)
			 {
			   Pay(nItemPiceVal);
			 }
			 else if (nItemPriceKind==moneyunit_xu)
			 {
			   PayXu(nItemPiceVal);
			 }
			 else if (nItemPriceKind==moneyunit_fuyuan)
			 {
				m_cFuYuan.AddFuYuanValue(-nItemPiceVal);
			 }
			 else if (nItemPriceKind==moneyunit_repute)
			 {
				m_cRepute.AddReputeValue(-nItemPiceVal);
			 }
			 else if (nItemPriceKind==moneyunit_expoint)
			 {
				  //AddAndPayExtPoint(-nItemPiceVal);
				 int nVal = m_cTask.GetSaveVal(2051);
				 m_cTask.SetSaveVal(2051,nVal-nItemPiceVal);
			 }
			 else if (nItemPriceKind==moneyunit_jifen)
			 {
				 int nVal = m_cTask.GetSaveVal(192);
				 m_cTask.SetSaveVal(192,nVal-nItemPiceVal);
			 }
			 else
			 {
				 m_ItemList.msgshow("Dau gia: sai loai tien te");
				 return;
			 }
			//------------------------------------------------------
			/*if (Item[nIndex].GetGenre()!=0 && !Item[nIndex].GetIsBang() && Item[nIndex].GetTime()->bYear<=0)
			{//不是装备的话,包袱里是否有这个物品
				int nIdx=m_ItemList.IsHaveItem(Item[nIndex].GetGenre(),\
					Item[nIndex].GetDetailType(),\
					Item[nIndex].GetParticular(),\
					Item[nIndex].GetSeries(),\
					Item[nIndex].GetLevel(),pos_equiproom);

				if (nIdx>0 && Item[nIdx].IsStack() &&
					Item[nIndex].CanStack(nIdx) &&
					(Item[nIdx].GetStackNum()+nStackNum) <= MAX_STACKNUM) //是否可以叠加--手里和包袱里的是否相同
				{//包袱里的东西可以叠加
					//Item[nIdx].SetStackNum(Item[nIdx].GetStackNum()+StackNum);             //设置叠加数量
					//m_ItemList.SynPlayerItem(nIdx);                                        //同步到客户端
					//xep chong item mua
					printf("da mua item %d HaveItem: %d\n",Item[nIdx].GetGenre(),nIdx);
					nStackNum=Item[nIdx].GetStackNum()+nStackNum;
					m_ItemList.Remove(nIdx);//删除包袱的东西
					ItemSet.Remove(nIdx);
				}
			}*/
			//-------------------------------------------------------
			/*if (m_ItemList.GetPlayerItemCount()>=MAX_ITEMLIST_CONUT)
			{
				ItemSet.Remove(nIndex);
				m_ItemList.msgshow("Dau gia:<c=yellow>物品超限,请清理储物空间<c>!");
				return;
			}*/

			int	x, y;
			if (m_ItemList.CheckCanPlaceInEquipment(Item[nIndex].GetWidth(), Item[nIndex].GetHeight(), &x, &y))
			{//同步客户端（包里）
				m_ItemList.Add(nIndex,pos_equiproom,x,y,0,0,nStackNum);  //包袱增加装备 同步装备--客户端
			}
			else
			{
				m_ItemList.msgshow("Dau gia: Hanh trang dau roi");
				int	nIdx = m_ItemList.Hand();

				if (nIdx)
				{
					m_ItemList.Remove(nIdx);  //删除客户端的节点了
					ItemSet.Remove(nIdx);
				}

				//同步客户端（手里增加新的节点）
				m_ItemList.Remove(nIndex);  //删除客户端的节点了
				ItemSet.Remove(nIndex);
				return;
				//Player[nPlayerIndex].m_ItemList.Add(nIndex,pos_hand,0,0,0,0,nStackNum);
			}
		}


		return;
	}

    BuySell.Buy(m_nPlayerIndex, m_BuyInfo.m_nBuyIdx, pCommand->m_BuyIdx, pCommand->m_Place, pCommand->m_X, pCommand->m_Y,pCommand->m_shuishou,pCommand->m_CityMenber,pCommand->m_MapID,pCommand->m_ItemNum);
}

void KPlayer::SellItem(BYTE* pProtocol)
{
	PLAYER_SELL_ITEM_COMMAND* pCommand = (PLAYER_SELL_ITEM_COMMAND *)pProtocol;

	int nItemIndex = m_ItemList.SearchID(pCommand->m_ID);

	if  (nItemIndex<=0)
	{//如果不存在了 就删除客户端
		m_ItemList.msgshow("Loi: Vat pham thong tin khong ton tai, Vui long lien he GM!");
		ITEM_REMOVE_SYNC	sRemove;
		sRemove.ProtocolType = s2c_removeitem;
		sRemove.m_ID = pCommand->m_ID;
		sRemove.m_dx = 0;
		sRemove.m_dy = 0;
		sRemove.m_model = 0;
		sRemove.nPlace    =-1;
		g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sRemove, sizeof(ITEM_REMOVE_SYNC));
		char nItemInfo[128];
		snprintf(nItemInfo, sizeof(nItemInfo),"--------SellItem: Tai khoan: %s Name: %s, nghi ngo sao chep muc, muc bat thuong da bi xoa thanh cong!-----",m_AccoutName,m_PlayerName);
		g_LogFile.gTraceLogFile(nItemInfo,strlen(nItemInfo));
		return;
	}

	BuySell.Sell(m_nPlayerIndex, m_BuyInfo.m_nBuyIdx, nItemIndex,pCommand->m_ClinetKind);
}


void KPlayer::GopItemPlayer(BYTE* pProtocol)
{
	PLAYER_COMBINE_ITEM* pCommand = (PLAYER_COMBINE_ITEM *)pProtocol;

	int nItemIndex = m_ItemList.SearchID(pCommand->m_ID);

	if (nItemIndex <= 0 || nItemIndex >= MAX_ITEM)
		return;

    if (Item[nItemIndex].GetStackNum() == MAX_STACKNUM)
        return;

	m_ItemList.GopItem(nItemIndex, pos_equiproom);
}
 
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	功能：获取玩家进入游戏重生点位置
//-------------------------------------------------------------------------
void KPlayer::GetLoginRevivalPos(int *lpnSubWorld, int *lpnMpsX, int *lpnMpsY)
{
	*lpnSubWorld = m_sLoginRevivalPos.m_nSubWorldID;
	*lpnMpsX     = m_sLoginRevivalPos.m_nMpsX;
	*lpnMpsY     = m_sLoginRevivalPos.m_nMpsY;
}

//获取玩家的死亡后重生点
void KPlayer::GetDeathRevivalPos(int *lpnSubWorld, int *lpnMpsX, int *lpnMpsY)
{
	*lpnSubWorld = m_sDeathRevivalPos.m_nSubWorldID;
	*lpnMpsX     = m_sDeathRevivalPos.m_nMpsX;
	*lpnMpsY     = m_sDeathRevivalPos.m_nMpsY;
}

#endif

#ifdef _SERVER
void	KPlayer::SetTimer(DWORD nTime, int nTimerTaskId)					//时间任务脚本，开启计时器
{
	if (!nTime || !nTimerTaskId) return ;
	m_TimerTask.SetTimer(nTime, nTimerTaskId);
}

void	KPlayer::CloseTimer()							//关闭时间计时器
{
	m_TimerTask.CloseTimer();
}
#endif


#ifdef _SERVER
//------------------------------------------------------------------------------
//	功能：设定玩家重生点位置(地图 和 坐标必须存在)
//------------------------------------------------------------------------------
void	KPlayer::SetRevivalPos(int nSubWorld, int nReviveId)
{
	if (nSubWorld >= 0)  //如果小于0，表示沿用当前的
	{
        if (g_SubWorldSet.SearchWorld(nSubWorld)>0)  //如果存在这个地图
		    m_sLoginRevivalPos.m_nSubWorldID = nSubWorld;
		else
		{//如果不存在
               int mSubWorld=53,mReviveId=19;        //80县
			   //g_GameSetTing.GetInteger2("ServerConfig","PlayerRevPosId",&mSubWorld,&mReviveId);
			   mSubWorld =  _gameGlobalSet.PlayerInfo.mSubWorld;
			   mReviveId =  _gameGlobalSet.PlayerInfo.mReviveId;
               m_sLoginRevivalPos.m_nSubWorldID=mSubWorld;
               nReviveId=mReviveId;
		}
	}
	else
	{//当前地图
		m_sLoginRevivalPos.m_nSubWorldID = SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_SubWorldID; 
	}
//------------------检查是否存在这个地图的重生点-------------	
	KIniFile IniFile;

	g_SetFilePath("\\settings");
	
	IniFile.Load("RevivePos.ini");
	char szSection[32],szKeyName[16];
	
	snprintf(szSection, 32, "%d", m_sLoginRevivalPos.m_nSubWorldID);
	snprintf(szKeyName, sizeof(szKeyName), "%d", nReviveId);
	
	if(!IniFile.IsSectionExist(szSection))
	{//如果重生文件里米有这个地图的话 就设置默认的
		int mSubWorld=53,mReviveId=19;        //80县
		//g_GameSetTing.GetInteger2("ServerConfig","PlayerRevPosId",&mSubWorld,&mReviveId);

		mSubWorld =  _gameGlobalSet.PlayerInfo.mSubWorld;
		mReviveId =  _gameGlobalSet.PlayerInfo.mReviveId;

		m_sLoginRevivalPos.m_nSubWorldID=mSubWorld;
		nReviveId=mReviveId;
	}

    IniFile.Clear();
//------------------------------------------------------------
	POINT Pos;
	
	g_SubWorldSet.GetRevivalPosFromId(m_sLoginRevivalPos.m_nSubWorldID,nReviveId,&Pos);
	m_sLoginRevivalPos.m_ReviveID = nReviveId;
	m_sLoginRevivalPos.m_nMpsX    = Pos.x;
	m_sLoginRevivalPos.m_nMpsY    = Pos.y;	

	m_sDeathRevivalPos            = m_sLoginRevivalPos;  //死亡复活点（结构）
}

//------------------------------------------------------------------------------
//	功能：设定玩家临时死亡重生点位置
//------------------------------------------------------------------------------
void KPlayer::SetDeathRevivalPos(int nSubWorld,int nXpos,int nYpos)
{
/*	if (nSubWorld >= 0)  //如果小于0，表示沿用当前的
	{
        if (g_SubWorldSet.SearchWorld(nSubWorld)>0)  //如果存在这个地图
			m_sLoginRevivalPos.m_nSubWorldID = nSubWorld; = --maplist.ini 里的编号
		else
		{//如果不存在
			int mSubWorld=53,mReviveId=19;        //80县
			g_GameSetTing.GetInteger2("ServerConfig","PlayerRevPosId",&mSubWorld,&mReviveId);
			m_sLoginRevivalPos.m_nSubWorldID=mSubWorld;
			nReviveId=mReviveId;
		}
	}
	else
	{
		m_sLoginRevivalPos.m_nSubWorldID = SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_SubWorldID; 
	}
*/
	m_sDeathRevivalPos.m_nSubWorldID = nSubWorld;
    m_sDeathRevivalPos.m_nMpsX = nXpos;
    m_sDeathRevivalPos.m_nMpsY = nYpos;


	printf("----SetDeathRevivalPos:(%s):%d(%d,%d)----\n",m_PlayerName,m_sDeathRevivalPos.m_nSubWorldID,m_sDeathRevivalPos.m_nMpsX,m_sDeathRevivalPos.m_nMpsY);
}


#endif

#ifdef _SERVER
//------------------------------------------------------------------------------
//	功能：收到客户端查询某个npc组队信息的申请后，向这个客户端发送队伍信息
//------------------------------------------------------------------------------
void	KPlayer::S2CSendTeamInfo(BYTE* pProtocol)
{
	PLAYER_APPLY_TEAM_INFO	*pApplyTeamInfo = (PLAYER_APPLY_TEAM_INFO*)pProtocol;
	
	//------------------------------ 查询自身组队信息 -----------------------------
	if (pApplyTeamInfo->m_dwTarNpcID == Npc[m_nIndex].m_dwID)
	{
		SendSelfTeamInfo();
		return;
	}
	
	//--------------------------- 查询其他 npc 组队信息 -----------------------------
	PLAYER_SEND_TEAM_INFO	sTeamInfo;
	PLAYER_APPLY_TEAM_INFO_FALSE	sTeamInfoFalse;
	// 寻找相应 npc id 的 player 编号
	int		nPlayer, nRegionNo, i;
	nPlayer = SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_Region[Npc[m_nIndex].m_RegionIndex].FindPlayer(pApplyTeamInfo->m_dwTarNpcID);
	if ( nPlayer >= 0)
		goto SendMsg;
	for (i = 0; i < 8; ++i)
	{
		nRegionNo = SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_Region[Npc[m_nIndex].m_RegionIndex].m_nConnectRegion[i];
		if ( nRegionNo < 0)
			continue;
		nPlayer = SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_Region[nRegionNo].FindPlayer(pApplyTeamInfo->m_dwTarNpcID);
		if (nPlayer >= 0)
			break;
	}
	if (i >= 8)
		goto SendFalse;
	
SendMsg:
	// 寻找队伍
	KTeam	*pTeam;
	if ( !Player[nPlayer].m_cTeam.m_nFlag )
		goto SendFalse;
	pTeam = &g_Team[Player[nPlayer].m_cTeam.m_nID];
	if (pTeam->m_nCaptain < 0 || !pTeam->IsOpen())
		goto SendFalse;
	
	// 发送队伍消息
	sTeamInfo.ProtocolType = (BYTE)s2c_teaminfo;
	sTeamInfo.m_nCaptain = Npc[Player[pTeam->m_nCaptain].m_nIndex].m_dwID;
	for (i = 0; i < MAX_TEAM_MEMBER; ++i)
	{
		if (pTeam->m_nMember[i] < 0)
			sTeamInfo.m_nMember[i] = 0;
		else
			sTeamInfo.m_nMember[i] = Npc[Player[pTeam->m_nMember[i]].m_nIndex].m_dwID;
	}
	//	strcpy(sTeamInfo.m_szTeamName, pTeam->m_szName);
	
	// 发送
	g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sTeamInfo, sizeof(PLAYER_SEND_TEAM_INFO));
	return;
	
	// 发送失败通知
SendFalse:
	sTeamInfoFalse.ProtocolType = s2c_teamapplyinfofalse;
	g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sTeamInfoFalse, sizeof(PLAYER_APPLY_TEAM_INFO_FALSE));
	return;
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	功能：服务器向客户端发送队伍信息
//-------------------------------------------------------------------------
void	KPlayer::SendSelfTeamInfo()
{
	if (m_cTeam.m_nFlag && m_cTeam.m_nID >= 0)	// 发送队伍信息
	{
		int		nNpcIndex;
		PLAYER_SEND_SELF_TEAM_INFO	sSelfInfo;
		sSelfInfo.ProtocolType = s2c_teamselfinfo;
		sSelfInfo.nTeamServerID = m_cTeam.m_nID;
		sSelfInfo.m_dwLeadExp = m_dwLeadExp;
		sSelfInfo.m_btState = g_Team[m_cTeam.m_nID].m_nState;
		
		// 队长
		nNpcIndex = Player[g_Team[m_cTeam.m_nID].m_nCaptain].m_nIndex;
		sSelfInfo.m_dwNpcID[0] = Npc[nNpcIndex].m_dwID;
		sSelfInfo.m_btLevel[0] = (WORD)Npc[nNpcIndex].m_Level;
		snprintf(sSelfInfo.m_szNpcName[0], sizeof(sSelfInfo.m_szNpcName[0]), "%s", Npc[nNpcIndex].Name);
		// 队员
		for (int i = 0; i < MAX_TEAM_MEMBER;++i)
		{
			if (g_Team[m_cTeam.m_nID].m_nMember[i] >= 0)
			{
				nNpcIndex = Player[g_Team[m_cTeam.m_nID].m_nMember[i]].m_nIndex;
				sSelfInfo.m_dwNpcID[i + 1] = Npc[nNpcIndex].m_dwID;
				sSelfInfo.m_btLevel[i + 1] = (WORD)Npc[nNpcIndex].m_Level;
				snprintf(sSelfInfo.m_szNpcName[i + 1], sizeof(sSelfInfo.m_szNpcName[i + 1]), "%s", Npc[nNpcIndex].Name);
			}
			else
			{
				sSelfInfo.m_dwNpcID[i + 1] = 0;
				sSelfInfo.m_btLevel[i + 1] = 0;
			}
		}
		// 发送消息
		g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sSelfInfo, sizeof(PLAYER_SEND_SELF_TEAM_INFO));
	}
	else	// 发送离队信息
	{
		PLAYER_LEAVE_TEAM	sLeaveTeam;
		sLeaveTeam.ProtocolType = s2c_teamleave;
		sLeaveTeam.m_dwNpcID = Npc[m_nIndex].m_dwID;
		g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sLeaveTeam, sizeof(PLAYER_LEAVE_TEAM));
	}
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	功能：收到客户端请求创建一支队伍
//-------------------------------------------------------------------------
BOOL	KPlayer::CreateTeam(BYTE* pProtocol)
{
	return m_cTeam.CreateTeam(this->m_nPlayerIndex, (PLAYER_APPLY_CREATE_TEAM*)pProtocol);
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	功能：收到客户端请求开放、关闭本player队伍是否允许队员加入状态
//-------------------------------------------------------------------------
BOOL	KPlayer::SetTeamState(BYTE* pProtocol)
{
	if (this->CheckTrading())
		return FALSE;
	if ( !m_cTeam.m_nFlag || m_cTeam.m_nFigure != TEAM_CAPTAIN || m_cTeam.m_nID < 0)
	{
		SendSelfTeamInfo();
		return FALSE;
	}
	
	PLAYER_TEAM_OPEN_CLOSE	*pTeamState = (PLAYER_TEAM_OPEN_CLOSE*)pProtocol;
	if (pTeamState->m_btOpenClose == 0)
	{
		g_Team[m_cTeam.m_nID].SetTeamClose();
	}
	else
	{
		if (m_cMenuState.m_nState == PLAYER_MENU_STATE_TRADEOPEN)
			m_cMenuState.SetState(m_nPlayerIndex, PLAYER_MENU_STATE_NORMAL);
		g_Team[m_cTeam.m_nID].SetTeamOpen();
	}
	
	return TRUE;
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	功能：收到客户端请求加入一支队伍
//-------------------------------------------------------------------------
BOOL	KPlayer::S2CSendAddTeamInfo(BYTE* pProtocol)
{
	if (!m_cTeam.m_bCanTeamFlag)
	{
		SHOW_MSG_SYNC	sMsg;
		sMsg.ProtocolType = s2c_msgshow;
		sMsg.m_wMsgID = enumMSG_ID_CANNOT_ADD_TEAM;
		sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1;
		g_pServer->PackDataToClient(m_nNetConnectIdx,(BYTE *)&sMsg, sMsg.m_wLength + 1);
		return FALSE;
	}
	
	PLAYER_APPLY_ADD_TEAM	*pAddTeam = (PLAYER_APPLY_ADD_TEAM*)pProtocol;
	
	// 寻找队长 npc id 的 player 编号
	int		nPlayer, nRegionNo, i;
	nPlayer = SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_Region[Npc[m_nIndex].m_RegionIndex].FindPlayer(pAddTeam->m_dwTarNpcID);
	if ( nPlayer >= 0)
		goto SendMsg;
	for (i = 0; i < 8; ++i)
	{
		nRegionNo = SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_Region[Npc[m_nIndex].m_RegionIndex].m_nConnectRegion[i];
		if ( nRegionNo < 0)
			continue;
		nPlayer = SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_Region[nRegionNo].FindPlayer(pAddTeam->m_dwTarNpcID);
		if (nPlayer >= 0)
			break;
	}
	if (i >= 8)
		return FALSE;
	
SendMsg:
	if (Npc[Player[nPlayer].m_nIndex].m_Camp == camp_begin && Npc[m_nIndex].m_Camp != camp_begin)
		return FALSE;
	m_cTeam.m_nApplyCaptainID = pAddTeam->m_dwTarNpcID;
	PLAYER_APPLY_ADD_TEAM	sAddTeam;
	sAddTeam.ProtocolType = s2c_teamgetapply;
	sAddTeam.m_dwTarNpcID = Npc[m_nIndex].m_dwID;
	g_pServer->PackDataToClient(Player[nPlayer].m_nNetConnectIdx, (BYTE*)&sAddTeam, sizeof(PLAYER_APPLY_ADD_TEAM));
	return TRUE;
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	功能：队长通知服务器接受某个npc为队伍成员
//-------------------------------------------------------------------------
BOOL	KPlayer::AddTeamMember(BYTE* pProtocol)
{
	// 错误检测(是否组队、是否队长、队伍开放状态、队员数、统率力)
	if (m_cTeam.m_nID<0 || m_cTeam.m_nID >=MAX_PLAYER)
		return FALSE;

	if ( !m_cTeam.m_nFlag ||
		m_cTeam.m_nFigure != TEAM_CAPTAIN ||
		!g_Team[m_cTeam.m_nID].IsOpen() ||
		g_Team[m_cTeam.m_nID].m_nMemNum >= MAX_TEAM_MEMBER ||
		g_Team[m_cTeam.m_nID].m_nMemNum >= g_Team[m_cTeam.m_nID].CalcCaptainPower())
	{
		SendSelfTeamInfo();
		return FALSE;
	}
	
	// 从npc id查找player
	int		nPlayer, i;
	PLAYER_ACCEPT_TEAM_MEMBER	*pAccept = (PLAYER_ACCEPT_TEAM_MEMBER*)pProtocol;
	nPlayer = FindAroundPlayer(pAccept->m_dwNpcID);
	if (nPlayer < 0)
		return FALSE;
	// 对方正处于不能组队状态
	if (!Player[nPlayer].m_cTeam.m_bCanTeamFlag)
	{
		SHOW_MSG_SYNC	sMsg;
		sMsg.ProtocolType = s2c_msgshow;
		sMsg.m_wMsgID = enumMSG_ID_TARGET_CANNOT_ADD_TEAM;
		sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 ;
		g_pServer->PackDataToClient(m_nNetConnectIdx,(BYTE *)&sMsg, sMsg.m_wLength + 1);
		return FALSE;
	}
	// 已经在队伍中
	if (Player[nPlayer].m_cTeam.m_nFlag)
		return FALSE;
	if (Player[nPlayer].m_cTeam.m_nApplyCaptainID != Npc[this->m_nIndex].m_dwID)
		return FALSE;

	// 如果满员就不能再加了 改变队伍状态
	if (g_Team[m_cTeam.m_nID].m_nMemNum >= MAX_TEAM_MEMBER || g_Team[m_cTeam.m_nID].CheckFull())
	{
		g_Team[m_cTeam.m_nID].SetTeamClose();
		return FALSE;
	}

	// 队伍添加成员
	if (!g_Team[m_cTeam.m_nID].AddMember(nPlayer) )
		return FALSE;

	// 被接受队员队伍数据处理
	Player[nPlayer].m_cTeam.Release();
	Player[nPlayer].m_cTeam.m_nFlag = 1;
	Player[nPlayer].m_cTeam.m_nFigure = TEAM_MEMBER;
	Player[nPlayer].m_cTeam.m_nID = m_cTeam.m_nID;
	Npc[Player[nPlayer].m_nIndex].SetCurrentCamp(Npc[m_nIndex].m_Camp);
	
	// 给各个队员发消息
	PLAYER_TEAM_ADD_MEMBER	sAddMem;
	sAddMem.ProtocolType = s2c_teamaddmember;
	sAddMem.m_dwNpcID = Npc[Player[nPlayer].m_nIndex].m_dwID;
	sAddMem.m_btLevel = (WORD)Npc[Player[nPlayer].m_nIndex].m_Level;
	snprintf(sAddMem.m_szName, sizeof(sAddMem.m_szName), "%s", Npc[Player[nPlayer].m_nIndex].Name);
	
	// 给队长发消息
	g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sAddMem, sizeof(PLAYER_TEAM_ADD_MEMBER));
	
	// 给老队员发消息
	for (i = 0; i < MAX_TEAM_MEMBER; ++i)
	{
		if (g_Team[m_cTeam.m_nID].m_nMember[i] < 0 || g_Team[m_cTeam.m_nID].m_nMember[i] == nPlayer)
			continue;
		g_pServer->PackDataToClient(Player[g_Team[m_cTeam.m_nID].m_nMember[i]].m_nNetConnectIdx, (BYTE*)&sAddMem, sizeof(PLAYER_TEAM_ADD_MEMBER));
	}
	
	// ------------------------------------- 给新队员发消息 --------------------------
	int		nNpcIndex;
	PLAYER_SEND_SELF_TEAM_INFO	sSelfInfo;
	sSelfInfo.ProtocolType = s2c_teamselfinfo;
	sSelfInfo.nTeamServerID = m_cTeam.m_nID;
	sSelfInfo.m_dwLeadExp = Player[nPlayer].m_dwLeadExp;
	sSelfInfo.m_btState = g_Team[m_cTeam.m_nID].m_nState;
	// 队长数据
	nNpcIndex = m_nIndex;
	sSelfInfo.m_dwNpcID[0] = Npc[nNpcIndex].m_dwID;
	sSelfInfo.m_btLevel[0] = (WORD)Npc[nNpcIndex].m_Level;
	snprintf(sSelfInfo.m_szNpcName[0], sizeof(sSelfInfo.m_szNpcName[0]), "%s", Npc[nNpcIndex].Name);
	// 队员数据
	for (i = 0; i < MAX_TEAM_MEMBER; ++i)
	{
		if (g_Team[m_cTeam.m_nID].m_nMember[i] >= 0)
		{
			nNpcIndex = Player[g_Team[m_cTeam.m_nID].m_nMember[i]].m_nIndex;
			sSelfInfo.m_dwNpcID[i + 1] = Npc[nNpcIndex].m_dwID;
			sSelfInfo.m_btLevel[i + 1] = (WORD)Npc[nNpcIndex].m_Level;
			snprintf(sSelfInfo.m_szNpcName[i + 1], sizeof(sSelfInfo.m_szNpcName[i + 1]), "%s", Npc[nNpcIndex].Name);
		}
		else
		{
			sSelfInfo.m_dwNpcID[i + 1] = 0;
			sSelfInfo.m_btLevel[i + 1] = 0;
		}
	}
	// 发送消息
	g_pServer->PackDataToClient(Player[nPlayer].m_nNetConnectIdx, (BYTE*)&sSelfInfo, sizeof(PLAYER_SEND_SELF_TEAM_INFO));
	
	SHOW_MSG_SYNC	sMsg;
	sMsg.ProtocolType = s2c_msgshow;
	sMsg.m_wMsgID = enumMSG_ID_TEAM_SELF_ADD;
	sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1;
	sMsg.m_lpBuf = 0;
	g_pServer->PackDataToClient(Player[nPlayer].m_nNetConnectIdx,(BYTE *)&sMsg, sMsg.m_wLength + 1);
	
	// --------------------------------- 给新队员发消息 end --------------------------
	
	return TRUE;
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	功能：收到客户端队员通知离开队伍
//-------------------------------------------------------------------------
void	KPlayer::LeaveTeam(BYTE* pProtocol)
{
	if (!m_cTeam.m_nFlag)
		return;
	if (this->m_cTeam.m_nFigure == TEAM_CAPTAIN && g_Team[m_cTeam.m_nID].IsOpen())
	{//队长离开
		g_Team[m_cTeam.m_nID].SetTeamClose();
	}
	
	// 非队长离开队伍，发系统消息，队长离开队伍的系统消息处理在DeleteMember里面
	if (m_cTeam.m_nFigure != TEAM_CAPTAIN)
	{
		SHOW_MSG_SYNC	sMsg;
		sMsg.ProtocolType = s2c_msgshow;
		sMsg.m_wMsgID = enumMSG_ID_TEAM_LEAVE;
		sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1;
		sMsg.m_lpBuf = Npc[this->m_nIndex].m_dwID;
		
		g_pServer->PackDataToClient(Player[g_Team[m_cTeam.m_nID].m_nCaptain].m_nNetConnectIdx,(BYTE *)&sMsg, sMsg.m_wLength + 1);
		for (int i = 0; i < MAX_TEAM_MEMBER; ++i)
		{
			if (g_Team[m_cTeam.m_nID].m_nMember[i] > 0)
			{
				g_pServer->PackDataToClient(Player[g_Team[m_cTeam.m_nID].m_nMember[i]].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
			}
		}
		sMsg.m_lpBuf = 0;
	}
	
	// 服务器端队员离开的处理
	g_Team[m_cTeam.m_nID].DeleteMember(GetPlayerIndex());
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	功能：收到客户端队长通知踢出某个队员
//-------------------------------------------------------------------------
void	KPlayer::TeamKickOne(BYTE* pProtocol)
{
	if ( !m_cTeam.m_nFlag || m_cTeam.m_nFigure != TEAM_CAPTAIN)
		return;
	
	int		nMemNo;
	PLAYER_TEAM_KICK_MEMBER	*pKickOne = (PLAYER_TEAM_KICK_MEMBER*)pProtocol;
	nMemNo = g_Team[m_cTeam.m_nID].FindMemberID(pKickOne->m_dwNpcID);
	if (nMemNo < 0)
		return;
	int		nPlayerNo;
	// 服务器端队员离开的处理
	nPlayerNo = g_Team[m_cTeam.m_nID].m_nMember[nMemNo];
	g_Team[m_cTeam.m_nID].DeleteMember(nPlayerNo);
	
	// 发消息给客户端
	/*
	int		nLength;
	SHOW_MSG_SYNC	sMsg;
	nLength = strlen(Npc[Player[nPlayerNo].m_nIndex].Name);
	sMsg.ProtocolType = s2c_msgshow;
	sMsg.m_wMsgID = enumMSG_ID_TEAM_KICK_One;
	sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID) + nLength;
	sMsg.m_lpBuf = new BYTE[sMsg.m_wLength + 1];
	
	memcpy(sMsg.m_lpBuf, &sMsg, sizeof(SHOW_MSG_SYNC) - sizeof(LPVOID));
	memcpy((char*)sMsg.m_lpBuf + sizeof(SHOW_MSG_SYNC) - sizeof(LPVOID), Npc[Player[nPlayerNo].m_nIndex].Name, nLength);
	
	g_pServer->PackDataToClient(Player[nPlayerNo].m_nNetConnectIdx, sMsg.m_lpBuf, sMsg.m_wLength + 1);
	
	g_pServer->PackDataToClient(Player[g_Team[m_cTeam.m_nID].m_nCaptain].m_nNetConnectIdx, sMsg.m_lpBuf, sMsg.m_wLength + 1);
*/	
	char nMsg[64];

	snprintf(nMsg, sizeof(nMsg),"%s da roi khoi nhom!",Npc[Player[nPlayerNo].m_nIndex].Name);
	Player[nPlayerNo].m_ItemList.msgshow(nMsg);
	Player[g_Team[m_cTeam.m_nID].m_nCaptain].m_ItemList.msgshow(nMsg);

	for (int i = 0; i < MAX_TEAM_MEMBER; ++i)
	{
		if (g_Team[m_cTeam.m_nID].m_nMember[i] > 0)
			Player[g_Team[m_cTeam.m_nID].m_nMember[i]].m_ItemList.msgshow(nMsg);
			//g_pServer->PackDataToClient(Player[g_Team[m_cTeam.m_nID].m_nMember[i]].m_nNetConnectIdx, sMsg.m_lpBuf, sMsg.m_wLength + 1);
	}
	//sMsg.Release();
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	功能：收到客户端队长通知把队长身份交给某个队员
//-------------------------------------------------------------------------
void	KPlayer::TeamChangeCaptain(BYTE* pProtocol)
{
	if ( !m_cTeam.m_nFlag || m_cTeam.m_nFigure != TEAM_CAPTAIN)
		return;
	int		i, nMember, nPlayerNo, nIsOpen = 0;
	PLAYER_APPLY_TEAM_CHANGE_CAPTAIN	*pChange = (PLAYER_APPLY_TEAM_CHANGE_CAPTAIN*)pProtocol;
	nMember = g_Team[m_cTeam.m_nID].FindMemberID(pChange->m_dwNpcID);
	if (nMember < 0)
		return;
	if (Npc[Player[g_Team[m_cTeam.m_nID].m_nMember[nMember]].m_nIndex].m_Camp == camp_begin && Npc[this->m_nIndex].m_Camp != camp_begin)
	{
		// 队长移交失败：对方统帅力不够
		SHOW_MSG_SYNC	sMsg;
		sMsg.ProtocolType = s2c_msgshow;
		sMsg.m_wMsgID = enumMSG_ID_TEAM_CHANGE_CAPTAIN_FAIL2;
		sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1;
		sMsg.m_lpBuf = pChange->m_dwNpcID;
		g_pServer->PackDataToClient(m_nNetConnectIdx,(BYTE *)&sMsg, sMsg.m_wLength + 1);
		sMsg.m_lpBuf = 0;
		return;
	}
	if (g_Team[m_cTeam.m_nID].m_nMemNum > PlayerSet.m_cLeadExp.GetMemNumFromLevel(Player[g_Team[m_cTeam.m_nID].m_nMember[nMember]].m_dwLeadLevel))
	{
		// 队长移交失败：对方统帅力不够
		SHOW_MSG_SYNC	sMsg;
		sMsg.ProtocolType = s2c_msgshow;
		sMsg.m_wMsgID = enumMSG_ID_TEAM_CHANGE_CAPTAIN_FAIL;
		sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1;
		sMsg.m_lpBuf = pChange->m_dwNpcID;
		g_pServer->PackDataToClient(m_nNetConnectIdx,(BYTE *)&sMsg, sMsg.m_wLength + 1);
		sMsg.m_lpBuf = 0;
		return;
	}
	if (g_Team[m_cTeam.m_nID].IsOpen())
	{
		g_Team[m_cTeam.m_nID].SetTeamClose();
		nIsOpen = 1;
	}
	nPlayerNo = g_Team[m_cTeam.m_nID].m_nCaptain;
	g_Team[m_cTeam.m_nID].m_nCaptain = g_Team[m_cTeam.m_nID].m_nMember[nMember];
	g_Team[m_cTeam.m_nID].m_nMember[nMember] = nPlayerNo;
	m_cTeam.m_nFigure = TEAM_MEMBER;
	Player[g_Team[m_cTeam.m_nID].m_nCaptain].m_cTeam.m_nFigure = TEAM_CAPTAIN;
	// 改变阵营
	Npc[Player[g_Team[m_cTeam.m_nID].m_nCaptain].m_nIndex].RestoreCurrentCamp();
	for (i = 0; i < MAX_TEAM_MEMBER; ++i)
	{
		nPlayerNo = g_Team[m_cTeam.m_nID].m_nMember[i];
		if (nPlayerNo < 0)
			continue;
		Npc[Player[nPlayerNo].m_nIndex].SetCurrentCamp(Npc[Player[g_Team[m_cTeam.m_nID].m_nCaptain].m_nIndex].m_Camp);
	}
	
	// 给各个队员发消息
	PLAYER_TEAM_CHANGE_CAPTAIN	sChangeCaptain;
	sChangeCaptain.ProtocolType = s2c_teamchangecaptain;
	sChangeCaptain.m_dwCaptainID = pChange->m_dwNpcID;
	sChangeCaptain.m_dwMemberID = Npc[m_nIndex].m_dwID;
	// 给队长发消息
	g_pServer->PackDataToClient(Player[g_Team[m_cTeam.m_nID].m_nCaptain].m_nNetConnectIdx, (BYTE*)&sChangeCaptain, sizeof(PLAYER_TEAM_CHANGE_CAPTAIN));
	// 给队员发消息
	for (i = 0; i < MAX_TEAM_MEMBER; ++i)
	{
		nPlayerNo = g_Team[m_cTeam.m_nID].m_nMember[i];
		if (nPlayerNo < 0)
			continue;
		g_pServer->PackDataToClient(Player[nPlayerNo].m_nNetConnectIdx, (BYTE*)&sChangeCaptain, sizeof(PLAYER_TEAM_CHANGE_CAPTAIN));
	}
	
	if (nIsOpen)
	{
		g_Team[m_cTeam.m_nID].SetTeamOpen();
	}
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	功能：收到客户端队长请求解散队伍
//-------------------------------------------------------------------------
void	KPlayer::TeamDismiss(BYTE* pProtocol)
{
	if ( !m_cTeam.m_nFlag || m_cTeam.m_nFigure != TEAM_CAPTAIN)
		return;
	
	PLAYER_LEAVE_TEAM	sLeaveTeam;
	int		i;
	SHOW_MSG_SYNC	sMsg;
	
	// 队伍解散消息
	sMsg.ProtocolType = s2c_msgshow;
	sMsg.m_wMsgID = enumMSG_ID_TEAM_DISMISS;
	sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1;
	g_pServer->PackDataToClient(m_nNetConnectIdx,(BYTE *)&sMsg, sMsg.m_wLength + 1);
	
	// 给队长发消息
	sLeaveTeam.ProtocolType = s2c_teamleave;
	sLeaveTeam.m_dwNpcID = Npc[m_nIndex].m_dwID;
	g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sLeaveTeam, sizeof(PLAYER_LEAVE_TEAM));
	// 给队员发消息
	for (i = 0; i < MAX_TEAM_MEMBER; ++i)
	{
		g_pServer->PackDataToClient(Player[g_Team[m_cTeam.m_nID].m_nMember[i]].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
		
		sLeaveTeam.m_dwNpcID = Npc[Player[g_Team[m_cTeam.m_nID].m_nMember[i]].m_nIndex].m_dwID;
		g_pServer->PackDataToClient(Player[g_Team[m_cTeam.m_nID].m_nMember[i]].m_nNetConnectIdx, (BYTE*)&sLeaveTeam, sizeof(PLAYER_LEAVE_TEAM));
		// 队员身份改变
		Player[g_Team[m_cTeam.m_nID].m_nMember[i]].m_cTeam.m_nFlag = 0;
		Npc[Player[g_Team[m_cTeam.m_nID].m_nMember[i]].m_nIndex].RestoreCurrentCamp();;
	}
	// 队伍清空
	g_Team[m_cTeam.m_nID].Release();
	// 队长身份改变
	m_cTeam.m_nFlag = 0;
	Npc[m_nIndex].RestoreCurrentCamp();
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	功能：收到客户端请求设定PK状态
//-------------------------------------------------------------------------
void	KPlayer::SetPK(BYTE* pProtocol)
{
	PLAYER_SET_PK	*pPK = (PLAYER_SET_PK*)pProtocol;
	
	if (pPK->m_btPKFlag)
		Npc[m_nIndex].SetCurrentCamp(camp_free);
	else
		Npc[m_nIndex].RestoreCurrentCamp();
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	功能：收到客户端请求获得门派数据
//-------------------------------------------------------------------------
void	KPlayer::SendFactionData(BYTE* pProtocol)
{
	SendFactionData();
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	功能：向客户端发送门派数据
//-------------------------------------------------------------------------
void	KPlayer::SendFactionData()
{
	// 给客户端发消息
	PLAYER_FACTION_DATA	sData;
	sData.ProtocolType = s2c_playerfactiondata;
	sData.m_btCamp = Npc[m_nIndex].m_Camp;
	sData.m_btCurFaction = m_cFaction.m_nCurFaction;
	sData.m_btFirstFaction = m_cFaction.m_nFirstAddFaction;
	sData.m_nAddTimes = m_cFaction.m_nAddTimes;
	
	g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sData, sizeof(PLAYER_FACTION_DATA));
}
#endif

#ifdef _SERVER
// not end
void KPlayer::GetAboutPos(KMapPos *pMapPos)
{
	if (m_nIndex <= 0)
		return;
	
	if (Npc[m_nIndex].m_SubWorldIndex < 0)
		return;
	
	POINT Pos;
	int nX, nY,nMap;
	Npc[m_nIndex].GetMpsPos(&nX, &nY,&nMap);  //人物所站的位置
	Pos.x = nX;
	Pos.y = nY;
	
	SubWorld[Npc[m_nIndex].m_SubWorldIndex].GetFreeObjPos(Pos);
	
	pMapPos->nSubWorld = Npc[m_nIndex].m_SubWorldIndex;
	SubWorld[Npc[m_nIndex].m_SubWorldIndex].Mps2Map(
		Pos.x, 
		Pos.y, 
		&pMapPos->nRegion, 
		&pMapPos->nMapX, 
		&pMapPos->nMapY, 
		&pMapPos->nOffX, 
		&pMapPos->nOffY);
}


void	KPlayer::GetNewAboutPos(KMapPos *pMapPos)
{
POINT	Pos[8] = 
{
{0, 32}, {-32, 32}, {-32, 0}, {-32, -32},
{0, -32}, {32, -32}, {32, 0}, {32, 32},
};

  int nMpsX, nMpsY, nTmpX, nTmpY;
  int nR, nMapX, nMapY,nMaps, nOffX, nOffY;
  Npc[m_nIndex].GetMpsPos(&nMpsX, &nMpsY,&nMaps);
  	int i;
	for (i = 0; i < 8; ++i)
	{
	  nTmpX = nMpsX + Pos[i].x;
	  nTmpY = nMpsY + Pos[i].y;
	  if (SubWorld[Npc[m_nIndex].m_SubWorldIndex].GetBarrier(nTmpX, nTmpY)) //有障碍
	     continue;
	  SubWorld[Npc[m_nIndex].m_SubWorldIndex].Mps2Map(nTmpX, nTmpY, &nR, &nMapX, &nMapY, &nOffX, &nOffY);
	  if (nR == -1) //区域为空
	     continue;
	 if (SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_Region[nR].GetRef(nMapX, nMapY, obj_object))
	 // if (SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_Region[nR].HasObj(nMapX, nMapY))
	     continue;  //有东西
	  else
	     break;
	}
	
	  if (i == 8)
	  {
	    pMapPos->nSubWorld = Npc[m_nIndex].m_SubWorldIndex;
	    pMapPos->nRegion = Npc[m_nIndex].m_RegionIndex;
	    pMapPos->nMapX = Npc[m_nIndex].m_MapX;
	    pMapPos->nMapY = Npc[m_nIndex].m_MapY;
	    pMapPos->nOffX = Npc[m_nIndex].m_OffX;
	    pMapPos->nOffY = Npc[m_nIndex].m_OffY;
	  }
	  else
	  {
	    pMapPos->nSubWorld = Npc[m_nIndex].m_SubWorldIndex;
	    pMapPos->nRegion = nR;
	    pMapPos->nMapX = nMapX;
	    pMapPos->nMapY = nMapY;
	    pMapPos->nOffX = nOffX;
	    pMapPos->nOffY = nOffY;
	  }
}
#endif


#ifdef _SERVER
//-------------------------------------------------------------------------
//	功能：寻找玩家周围的某个指定npc id的player index
//-------------------------------------------------------------------------
int		KPlayer::FindAroundPlayer(DWORD dwNpcID)
{
	if (dwNpcID == 0)
		return -1;
	
	int	nPlayer, nRegionNo, i;
	    nPlayer = SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_Region[Npc[m_nIndex].m_RegionIndex].FindPlayer(dwNpcID);
	if ( nPlayer >= 0)
		return nPlayer;

	for (i = 0; i < 8; ++i)
	{
		nRegionNo = SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_Region[Npc[m_nIndex].m_RegionIndex].m_nConnectRegion[i];
		if ( nRegionNo < 0)
			continue;
		nPlayer = SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_Region[nRegionNo].FindPlayer(dwNpcID);
		if (nPlayer >= 0)
			return nPlayer;
	}
	
	return -1;
}

//-------------------------------------------------------------------------
//	功能：判断某玩家是否在周围
//-------------------------------------------------------------------------
BOOL	KPlayer::CheckPlayerAround(int nPlayerIdx)
{
	if (nPlayerIdx <= 0)
		return FALSE;
	if (SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_Region[Npc[m_nIndex].m_RegionIndex].CheckPlayerIn(nPlayerIdx))
		return TRUE;
	int		nRegionNo;
	for (int i = 0; i < 8; ++i)
	{
		nRegionNo = SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_Region[Npc[m_nIndex].m_RegionIndex].m_nConnectRegion[i];
		if ( nRegionNo < 0)
			continue;
		if (SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_Region[nRegionNo].CheckPlayerIn(nPlayerIdx))
			return TRUE;
	}
	
	return FALSE;
}

//-------------------------------------------------------------------------
//	功能：寻找玩家周围的某个指定npc dwid的npc index
//-------------------------------------------------------------------------
int		KPlayer::FindAroundNpc(DWORD dwNpcID)
{
	if (dwNpcID <= 0)
		return 0;

	if (Npc[m_nIndex].m_SubWorldIndex >= MAX_SUBWORLD || Npc[m_nIndex].m_SubWorldIndex < 0 || Npc[m_nIndex].m_RegionIndex >= SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_nTotalRegion || Npc[m_nIndex].m_RegionIndex < 0)
	{
		Npc[m_nIndex].m_SubWorldIndex = 0;
		Npc[m_nIndex].m_RegionIndex = 381;
		Npc[m_nIndex].m_MapX = 0;
		Npc[m_nIndex].m_MapY = 0;
		Npc[m_nIndex].m_OffX = 0;
		Npc[m_nIndex].m_OffY = 0;
		printf("Xay ra loi tim kiem npc, tu dong xua loi !\n");
	}

	int	nNpc, nRegionNo, i;
	    nNpc = SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_Region[Npc[m_nIndex].m_RegionIndex].SearchNpc(dwNpcID);
	if ( nNpc > 0 && nNpc<MAX_NPC)
		return nNpc;
	for (i = 0; i < 8; i++)
	{
		nRegionNo = SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_Region[Npc[m_nIndex].m_RegionIndex].m_nConnectRegion[i];
		if ( nRegionNo < 0)
			continue;
		nNpc = SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_Region[nRegionNo].SearchNpc(dwNpcID);
		if ( nNpc > 0 && nNpc<MAX_NPC)
			return nNpc;
	}
	
	return 0;
}

int		KPlayer::FindAroundNpcID(DWORD dwNpcID, int nDist)
{
	if (dwNpcID == 0)
		return 0;

	if (Npc[m_nIndex].m_SubWorldIndex >= MAX_SUBWORLD || Npc[m_nIndex].m_SubWorldIndex < 0 || Npc[m_nIndex].m_RegionIndex >= SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_nTotalRegion || Npc[m_nIndex].m_RegionIndex < 0)
	{
		Npc[m_nIndex].m_SubWorldIndex = 0;
		Npc[m_nIndex].m_RegionIndex = 381;
		Npc[m_nIndex].m_MapX = 0;
		Npc[m_nIndex].m_MapY = 0;
		Npc[m_nIndex].m_OffX = 0;
		Npc[m_nIndex].m_OffY = 0;
		printf("Xay ra loi tim kiem npc, tu dong xua loi !\n");
	}


	int		nNpc, nRegionNo, i;
	nNpc = SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_Region[Npc[m_nIndex].m_RegionIndex].SearchNpc(dwNpcID);
	if (nNpc > 0)
	{
		if (nDist <= 0 || (nDist >= NpcSet.GetDistance(m_nIndex, nNpc)))
			return nNpc;
	}
	for (i = 0; i < 8; i++)
	{
		nRegionNo = SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_Region[Npc[m_nIndex].m_RegionIndex].m_nConnectRegion[i];
		if ( nRegionNo < 0)
			continue;
		nNpc = SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_Region[nRegionNo].SearchNpc(dwNpcID);
		if (nNpc > 0)
		{
			if (nDist <= 0 || (nDist >= NpcSet.GetDistance(m_nIndex, nNpc)))
				return nNpc;
		}
	}
	
	return 0;
}

#endif

//-------------------------------------------------------------------------
//	功能：新玩家登陆时根据五行属性产生 力量 敏捷 活力 精力 四项数值   ----没有调用
//-------------------------------------------------------------------------
BOOL	KPlayer::NewPlayerGetBaseAttribute(int Series)
{
	if (Series < series_metal || Series > series_earth)
		return FALSE;
	
	Npc[m_nIndex].SetSeries(Series);
	
	m_nStrength = PlayerSet.m_cNewPlayerAttribute.m_nStrength[Series];
	m_nDexterity = PlayerSet.m_cNewPlayerAttribute.m_nDexterity[Series];
	m_nVitality = PlayerSet.m_cNewPlayerAttribute.m_nVitality[Series];
	m_nEngergy = PlayerSet.m_cNewPlayerAttribute.m_nEngergy[Series];
	m_nLucky = PlayerSet.m_cNewPlayerAttribute.m_nLucky[Series]; //幸运值
	m_nUpExp = 0;  //杀死敌人获得经验增加
	
	m_nCurStrength = m_nStrength;
	m_nCurDexterity = m_nDexterity;
	m_nCurVitality = m_nVitality;
	m_nCurEngergy = m_nEngergy;

	if  (m_nCurStrength<0) m_nCurStrength=0;
	if  (m_nCurDexterity<0) m_nCurDexterity=0;
	if  (m_nCurVitality<0) m_nCurVitality=0;
	if  (m_nCurEngergy<0) m_nCurEngergy=0;
	CalcCurLucky();
	
	m_cFaction.SetSeries(Series);
	
	return TRUE;
}

//-------------------------------------------------------------------------
//	功能：计算当前力量
//-------------------------------------------------------------------------
//void	KPlayer::CalcCurStrength()
//{	// 还需要考虑 装备、技能、状态 的影响
//	m_nCurStrength = m_nStrength;
//}

//-------------------------------------------------------------------------
//	功能：计算当前敏捷
//-------------------------------------------------------------------------
//void	KPlayer::CalcCurDexterity()
//{	// 还需要考虑 装备、技能、状态 的影响
//	m_nCurDexterity = m_nDexterity;
//}

//-------------------------------------------------------------------------
//	功能：计算当前活力
//-------------------------------------------------------------------------
//void	KPlayer::CalcCurVitality()
//{	// 还需要考虑 装备、技能、状态 的影响
//	m_nCurVitality = m_nVitality;
//}

//-------------------------------------------------------------------------
//	功能：计算当前精力
//-------------------------------------------------------------------------
//void	KPlayer::CalcCurEngergy()
//{	// 还需要考虑 装备、技能、状态 的影响
//	m_nCurEngergy = m_nEngergy;
//}

//-------------------------------------------------------------------------
//	功能：计算当前运气
//-------------------------------------------------------------------------
void	KPlayer::CalcCurLucky()
{	// 还需要考虑 装备、技能、状态 的影响
	m_nCurLucky = m_nLucky;
}

#ifdef _SERVER
void  KPlayer::SyncCurrentBaseAttriibute(int type,int attribute,int curAttribute)
{	
	PLAYER_ATTRIBUTE_SYNC	sSync;
	sSync.ProtocolType = s2c_playersyncattribute;
	sSync.m_btAttribute = type;
	sSync.m_nBasePoint = attribute;
	sSync.m_nCurPoint = curAttribute;
	sSync.m_nLeavePoint = m_nAttributePoint;
	g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sSync, sizeof(PLAYER_ATTRIBUTE_SYNC));
}
//-------------------------------------------------------------------------
//	功能：增加基本力量-----力量
//-------------------------------------------------------------------------
void	KPlayer::AddBaseStrength(int nData)
{
	if (nData > 0 && nData <= m_nAttributePoint)
	{
		m_nStrength += nData;

		if  (m_nStrength<0)
			m_nStrength=0;

		m_nAttributePoint -= nData;
		m_nCurStrength += nData;

		if (m_nCurStrength<0)
		   m_nCurStrength=0;

		UpdataCurData();
		SetNpcPhysicsDamage();  // 设置物理伤害
		// 把当前力量传给客户端
		PLAYER_ATTRIBUTE_SYNC	sSync;
		sSync.ProtocolType = s2c_playersyncattribute;
		sSync.m_btAttribute = 0;
		sSync.m_nBasePoint = m_nStrength;
		sSync.m_nCurPoint = m_nCurStrength;
		sSync.m_nLeavePoint = m_nAttributePoint;
		g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sSync, sizeof(PLAYER_ATTRIBUTE_SYNC));
	}
}
//力量
void	KPlayer::ResetBaseStrength(int nData)
{		
	m_nStrength = nData;	
	m_nCurStrength = nData;
	if  (m_nStrength<0) m_nStrength=0;
	if  (m_nCurStrength<0) m_nCurStrength=0;
	UpdataCurData();
	SetNpcPhysicsDamage();    //
	SyncCurrentBaseAttriibute(0,m_nStrength,m_nCurStrength);	
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	功能：增加基本敏捷----身法
//-------------------------------------------------------------------------
void	KPlayer::AddBaseDexterity(int nData)
{
	if (nData > 0 && nData <= m_nAttributePoint)
	{
		m_nDexterity += nData;
		m_nAttributePoint -= nData;
		m_nCurDexterity += nData;
		if  (m_nDexterity<0) m_nDexterity=0;
		if  (m_nCurDexterity<0) m_nCurDexterity=0;
		SetNpcAttackRating();    // 设置命中
		SetNpcDefence();         // 设置闪避
		UpdataCurData();
		SetNpcPhysicsDamage();   // 设置物理伤害
		
		// 把当前力量传给客户端
		PLAYER_ATTRIBUTE_SYNC	sSync;
		sSync.ProtocolType = s2c_playersyncattribute;
		sSync.m_btAttribute = 1;
		sSync.m_nBasePoint = m_nDexterity;
		sSync.m_nCurPoint = m_nCurDexterity;
		sSync.m_nLeavePoint = m_nAttributePoint;
		g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sSync, sizeof(PLAYER_ATTRIBUTE_SYNC));
	}
}
//外功
void	KPlayer::ResetBaseDexterity(int nData)
{
		m_nDexterity = nData;		
		m_nCurDexterity = nData;
		if  (m_nDexterity<0) m_nDexterity=0;
		if  (m_nCurDexterity<0) m_nCurDexterity=0;
		SetNpcAttackRating();
		SetNpcDefence();
		UpdataCurData();
		SetNpcPhysicsDamage();
		SyncCurrentBaseAttriibute(1,m_nDexterity,m_nCurDexterity);

}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	功能：增加基本活力  ----外功
//-------------------------------------------------------------------------
void	KPlayer::AddBaseVitality(int nData)
{
	if (nData > 0 && nData <= m_nAttributePoint)
	{
		m_nVitality += nData;
		m_nAttributePoint -= nData;
		m_nCurVitality += nData;
		if  (m_nVitality<0) m_nVitality=0;
		if  (m_nCurVitality<0) m_nCurVitality=0;
		Npc[m_nIndex].AddBaseLifeMax(PlayerSet.m_cLevelAdd.GetLifePerVitality(Npc[m_nIndex].m_Series) * nData);
		Npc[m_nIndex].AddBaseStaminaMax(PlayerSet.m_cLevelAdd.GetStaminaPerVitality(Npc[m_nIndex].m_Series) * nData);
		UpdataCurData();
		
		// 把当前力量传给客户端
		PLAYER_ATTRIBUTE_SYNC	sSync;
		sSync.ProtocolType = s2c_playersyncattribute;
		sSync.m_btAttribute = 2;
		sSync.m_nBasePoint = m_nVitality;
		sSync.m_nCurPoint = m_nCurVitality;
		sSync.m_nLeavePoint = m_nAttributePoint;
		g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sSync, sizeof(PLAYER_ATTRIBUTE_SYNC));
	}
}
//身法
void	KPlayer::ResetBaseVitality(int nData)
{	
	m_nVitality = nData;		
	m_nCurVitality = nData;
	if  (m_nVitality<0) m_nVitality=0;
	if  (m_nCurVitality<0) m_nCurVitality=0;
	Npc[m_nIndex].SetBaseLifeMax(PlayerSet.m_cLevelAdd.GetLifePerVitality(Npc[m_nIndex].m_Series) * nData);
	Npc[m_nIndex].SetBaseStaminaMax(PlayerSet.m_cLevelAdd.GetStaminaPerVitality(Npc[m_nIndex].m_Series) * nData);
	UpdataCurData();
	SyncCurrentBaseAttriibute(2,m_nVitality,m_nCurVitality);
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	功能：增加基本精力-----内功
//-------------------------------------------------------------------------
void	KPlayer::AddBaseEngergy(int nData)
{
	if (nData > 0 && nData <= m_nAttributePoint)
	{
		m_nEngergy += nData;
		m_nAttributePoint -= nData;
		m_nCurEngergy += nData;
		if  (m_nEngergy<0) m_nEngergy=0;
		if  (m_nCurEngergy<0) m_nCurEngergy=0;
		Npc[m_nIndex].AddBaseManaMax(PlayerSet.m_cLevelAdd.GetManaPerEnergy(Npc[m_nIndex].m_Series) * nData);
		UpdataCurData();
		SetNpcPhysicsDamage();   //设置物理伤害

		// 把当前内功传给客户端
		PLAYER_ATTRIBUTE_SYNC	sSync;
		sSync.ProtocolType = s2c_playersyncattribute;
		sSync.m_btAttribute = 3;
		sSync.m_nBasePoint = m_nEngergy;
		sSync.m_nCurPoint = m_nCurEngergy;
		sSync.m_nLeavePoint = m_nAttributePoint;
		g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sSync, sizeof(PLAYER_ATTRIBUTE_SYNC));
	}
}
//内功
void	KPlayer::ResetBaseEngergy(int nData)
{
	m_nEngergy = nData;		
	m_nCurEngergy = nData;
	if  (m_nEngergy<0) m_nEngergy=0;
	if  (m_nCurEngergy<0) m_nCurEngergy=0;
	Npc[m_nIndex].SetBaseManaMax(PlayerSet.m_cLevelAdd.GetManaPerEnergy(Npc[m_nIndex].m_Series) * nData);
	UpdataCurData();
    SetNpcPhysicsDamage();
	SyncCurrentBaseAttriibute(3,m_nEngergy,m_nCurEngergy);
}
#endif

//-------------------------------------------------------------------------
//	功能：改变当前力量(当 nData 小于 0 时，减少)
//-------------------------------------------------------------------------
void	KPlayer::ChangeCurStrength(int nData)
{

	m_nCurStrength +=nData;
	if  (m_nCurStrength<0) m_nCurStrength=0;
	SetNpcPhysicsDamage();  //物理攻击增加
	// 把当前力量传给客户端
}

//-------------------------------------------------------------------------
//	功能：改变当前敏捷(当 nData 小于 0 时，减少)
//-------------------------------------------------------------------------
void	KPlayer::ChangeCurDexterity(int nData)
{
	m_nCurDexterity += nData;

	//if (m_nCurDexterity < 0)
	//	m_nCurDexterity = 0;

	//int nRating = nData * 4;
	//Npc[m_nIndex].m_CurrentAttackRating += nRating;

	//int nDefence = nData / 4;
	//Npc[m_nIndex].m_CurrentDefend += nDefence;

	SetNpcPhysicsDamage();
}


//-------------------------------------------------------------------------
//	功能：改变当前活力(当 nData 小于 0 时，减少)  、//增加当前生命 和内力值
//-------------------------------------------------------------------------
void	KPlayer::ChangeCurVitality(int nData)	    //状态
{
/*	if (nData + m_nAttributePoint < 0)
return;
	*/
	m_nCurVitality += nData;  //外功增加多少点
	if  (m_nCurVitality<0) m_nCurVitality=0;
	Npc[m_nIndex].AddCurLifeMax(PlayerSet.m_cLevelAdd.GetLifePerVitality(Npc[m_nIndex].m_Series) * nData);
	Npc[m_nIndex].AddCurStaminaMax(PlayerSet.m_cLevelAdd.GetStaminaPerVitality(Npc[m_nIndex].m_Series) * nData);	//体力
	
}

//-------------------------------------------------------------------------
//	功能：改变当前精力(当 nData 小于 0 时，减少) 内功
//-------------------------------------------------------------------------
void	KPlayer::ChangeCurEngergy(int nData)
{
/*	if (m_nAttributePoint + nData < 0)
return;
	*/
	m_nCurEngergy += nData;
	if  (m_nCurEngergy<0) m_nCurEngergy=0;
	Npc[m_nIndex].AddCurManaMax(PlayerSet.m_cLevelAdd.GetManaPerEnergy(Npc[m_nIndex].m_Series) * nData);

	SetNpcPhysicsDamage();
}

//-------------------------------------------------------------------------
//	功能：增加基本运气
//-------------------------------------------------------------------------
void	KPlayer::AddBaseLucky(int nData)
{
	m_nLucky += nData;
	if  (m_nLucky<0) m_nLucky=0;
	CalcCurLucky();
}

//-------------------------------------------------------------------------
//	功能：由当前力量计算对应npc的物理伤害(PhysicsDamage)---根据武器的伤害为基点---换装是调用
//-------------------------------------------------------------------------
void	KPlayer::SetNpcPhysicsDamage()
{
/*	int		nMinDamage = 0, nMaxDamage = 0, mLeftskillidex = 0;
	mLeftskillidex = GetLeftSkill();                         //左键技能
	KSkill* pOrdinSkill = (KSkill*)g_SkillManager.GetSkill(mLeftskillidex, 1);
	if (!pOrdinSkill)
		return;

	if (pOrdinSkill->IsMagic())
	{// noi cong
		if (Npc[m_nIndex].m_Series == 1)
		{// he moc
			//if (m_cFaction.m_nCurFaction == 2)
			if (m_ItemList.GetWeaponType() == equip_rangeweapon)	//vu khi danh xa
			{ // duong mon
				nMinDamage += (6 * m_nCurEngergy);
				nMaxDamage += (6 * m_nCurEngergy);
			}
		}
		else
		{// he khac
			nMinDamage += m_nCurEngergy;
			nMaxDamage += m_nCurEngergy;
		} 
		Npc[m_nIndex].SetPhysicsDamage(nMinDamage, nMaxDamage);		
	}
	else
	{ // ngoai cong
		m_ItemList.GetWeaponDamage(&nMinDamage, &nMaxDamage); // STVL(min/max) cua vu khi
		if (m_ItemList.GetWeaponType() == equip_meleeweapon)	// vu khi danh gan
		{
			nMinDamage += m_nCurStrength / STRENGTH_SET_DAMAGE_VALUE;
			nMaxDamage += m_nCurStrength / STRENGTH_SET_DAMAGE_VALUE;
			Npc[m_nIndex].SetPhysicsDamage(nMinDamage, nMaxDamage);
		}
		//if (m_cFaction.m_nCurFaction == 2)
		else if (m_ItemList.GetWeaponType() == equip_rangeweapon)	//vu khi danh xa 
		{ // duong mon -> Than Phap / 5
			nMinDamage += m_nCurDexterity / DEXTERITY_SET_DAMAGE_VALUE;
			nMaxDamage += m_nCurDexterity / DEXTERITY_SET_DAMAGE_VALUE;
			Npc[m_nIndex].SetPhysicsDamage(nMinDamage, nMaxDamage);
		}
		else
			Npc[m_nIndex].SetPhysicsDamage(nMinDamage, nMaxDamage);
	}
*/

	int		nMinDamage, nMaxDamage;
	m_ItemList.GetWeaponDamage(&nMinDamage, &nMaxDamage);
	
	if (m_ItemList.GetWeaponType() == equip_meleeweapon)
	{
		nMinDamage += m_nCurStrength / STRENGTH_SET_DAMAGE_VALUE;
		nMaxDamage += m_nCurStrength / STRENGTH_SET_DAMAGE_VALUE;
		Npc[m_nIndex].SetPhysicsDamage(nMinDamage, nMaxDamage);
	}
	else if (m_ItemList.GetWeaponType() == equip_rangeweapon)
	{
		nMinDamage += m_nCurDexterity / DEXTERITY_SET_DAMAGE_VALUE;
		nMaxDamage += m_nCurDexterity / DEXTERITY_SET_DAMAGE_VALUE;
		Npc[m_nIndex].SetPhysicsDamage(nMinDamage, nMaxDamage);
	}
	else
	{
		Npc[m_nIndex].SetPhysicsDamage(nMinDamage, nMaxDamage);
	}

}

//-------------------------------------------------------------------------
//	功能：由当前敏捷计算对应npc的攻击命中率(AttackRating)
//-------------------------------------------------------------------------
void	KPlayer::SetNpcAttackRating()
{
	int		nRating;
	
	nRating = m_nDexterity * 4 - 28;
	Npc[m_nIndex].SetBaseAttackRating(nRating);
}

//-------------------------------------------------------------------------
//	功能：由当前敏捷计算对应npc的防御力
//-------------------------------------------------------------------------
void	KPlayer::SetNpcDefence()
{
	int		nDefence;
	
	nDefence = m_nDexterity / 4;
	Npc[m_nIndex].SetBaseDefence(nDefence);
}

/*
#ifdef _SERVER
//-------------------------------------------------------------------------
//	功能：由当前敏捷计算对应npc的行走速度
//-------------------------------------------------------------------------
void	KPlayer::SetNpcWalkSpeed()
{
int		nSpeed;
nSpeed = BASE_WALK_SPEED * (m_nCurDexterity + 320) / 320;
Npc[m_nIndex].SetBaseWalkSpeed(nSpeed);
}
#endif
*/

/*
#ifdef _SERVER
//-------------------------------------------------------------------------
//	功能：由当前敏捷计算对应npc的跑步速度
//-------------------------------------------------------------------------
void	KPlayer::SetNpcRunSpeed()
{
int		nSpeed;
nSpeed = BASE_RUN_SPEED * (m_nCurDexterity + 320) / 320;
Npc[m_nIndex].SetBaseRunSpeed(nSpeed);
}
#endif
*/


//-------------------------------------------------------------------------
//	功能：增加经验(原始数据，还未经过处理，需要处理组队的影响)
//-------------------------------------------------------------------------
#ifdef _SERVER
void	KPlayer::AddExp(int nExp, int nTarLevel,int nGoldType)
{
	if (nExp == 1)
		return;
	 
	if (!m_cTeam.m_nFlag)  //没有组队
	{
		AddSelfExp(nExp, nTarLevel,nGoldType);
		return;
	}
	 
	// 已经组队
	int	i, j, nShareFlag[MAX_TEAM_MEMBER + 1], nRegion, nTotalLevel = 0, nTotalPlayer = 0;
	memset(nShareFlag, 0, sizeof(nShareFlag));
	
	int		nMpsX, nMpsY, nNpcIdx, nSelfSubIdx, nSelfMpsX, nSelfMpsY;
	
	nSelfSubIdx = Npc[m_nIndex].m_SubWorldIndex;

	SubWorld[nSelfSubIdx].NewMap2Mps(\
		Npc[m_nIndex].m_RegionIndex,\
		Npc[m_nIndex].m_MapX,\
		Npc[m_nIndex].m_MapY,\
		Npc[m_nIndex].m_OffX,\
		Npc[m_nIndex].m_OffY,\
		&nSelfMpsX,\
		&nSelfMpsY);
	
	// 队长
	nNpcIdx = Player[g_Team[m_cTeam.m_nID].m_nCaptain].m_nIndex;
	if (nSelfSubIdx == Npc[nNpcIdx].m_SubWorldIndex)
	{
		SubWorld[Npc[nNpcIdx].m_SubWorldIndex].NewMap2Mps(\
			Npc[nNpcIdx].m_RegionIndex,\
			Npc[nNpcIdx].m_MapX,\
			Npc[nNpcIdx].m_MapY,\
			Npc[nNpcIdx].m_OffX,\
			Npc[nNpcIdx].m_OffY,\
			&nMpsX,\
			&nMpsY);
		if ( (nMpsX - nSelfMpsX) * (nMpsX - nSelfMpsX) + \
			(nMpsY - nSelfMpsY) * (nMpsY - nSelfMpsY) <= \
			PLAYER_SHARE_EXP_DISTANCE * PLAYER_SHARE_EXP_DISTANCE)
		{
			nTotalLevel += Npc[nNpcIdx].m_Level;
			nShareFlag[0] = 1;
			nTotalPlayer++;
			if (nTotalPlayer >= g_Team[m_cTeam.m_nID].m_nMemNum + 1)
				goto SharePlace;
		}
	}
	// 队员
	for (i = 0; i < MAX_TEAM_MEMBER; ++i)
	{
		if (g_Team[m_cTeam.m_nID].m_nMember[i] < 0)
			continue;
		nNpcIdx = Player[g_Team[m_cTeam.m_nID].m_nMember[i]].m_nIndex;
		if (nSelfSubIdx != Npc[nNpcIdx].m_SubWorldIndex)
			continue;
		SubWorld[Npc[nNpcIdx].m_SubWorldIndex].NewMap2Mps(\
			Npc[nNpcIdx].m_RegionIndex,\
			Npc[nNpcIdx].m_MapX,\
			Npc[nNpcIdx].m_MapY,\
			Npc[nNpcIdx].m_OffX,\
			Npc[nNpcIdx].m_OffY,\
			&nMpsX,\
			&nMpsY);
		if ( (nMpsX - nSelfMpsX) * (nMpsX - nSelfMpsX) +\
			(nMpsY - nSelfMpsY) * (nMpsY - nSelfMpsY) >\
			PLAYER_SHARE_EXP_DISTANCE * PLAYER_SHARE_EXP_DISTANCE)
			continue;
		nTotalLevel += Npc[nNpcIdx].m_Level;
		nShareFlag[i + 1] = 1;
		nTotalPlayer++;
		if (nTotalPlayer >= g_Team[m_cTeam.m_nID].m_nMemNum + 1)
			goto SharePlace;
	}
	
SharePlace:
	//_ASSERT(nTotalPlayer >= 1);
	//_ASSERT(nTotalLevel >= 1);

	if (nTotalPlayer < 1 || nTotalLevel < 1)
		return;
	if (nTotalPlayer == 1)
	{
		AddSelfExp(nExp, nTarLevel,nGoldType);
		return;
	}

	float k,nGetExp;
	k = 100 + nTotalPlayer * 15;	
 
	// 给队长分配经验
	if (nShareFlag[0])
	{
		nGetExp = (k * Npc[Player[g_Team[m_cTeam.m_nID].m_nCaptain].m_nIndex].m_Level)/nTotalLevel; //总等级

		if (nGetExp <=0)
			nGetExp = 1;
		 
		nGetExp = k * Npc[Player[g_Team[m_cTeam.m_nID].m_nCaptain].m_nIndex].m_Level / nTotalLevel;
		if (nGetExp <= 0)
			nGetExp = 1;
		Player[g_Team[m_cTeam.m_nID].m_nCaptain].AddSelfExp(nExp * nGetExp / 100, nTarLevel, nGoldType);

		// LeadExp = 怪物提供的经验值 * β * 分享者等级 / 分享者等级和；
		int		nLeadExp = nExp * k * PlayerSet.m_cLevelAdd.GetLeadExpShare(Npc[m_nIndex].m_Series) * (nTotalLevel - Npc[m_nIndex].m_Level) / (nTotalLevel * 10000);
		Player[g_Team[m_cTeam.m_nID].m_nCaptain].AddLeadExp(nLeadExp);

	}
	// 给队员分配经验
	for (i = 0; i < MAX_TEAM_MEMBER; ++i)
	{
		if (!nShareFlag[i + 1])
			continue;

		nGetExp = k * Npc[Player[g_Team[m_cTeam.m_nID].m_nMember[i]].m_nIndex].m_Level / nTotalLevel;
		if (nGetExp <= 0)
			nGetExp = 1;
		Player[g_Team[m_cTeam.m_nID].m_nMember[i]].AddSelfExp(nExp * nGetExp / 100, nTarLevel, nGoldType);

	}
}


/* 定义个类型，主要是为了方便返回商和余数 */
/*struct DIV_T
{
    // 商 
    int quot; 
    // 余数 
    int rem;
}; */

/* 
有符号数除法，负数的时候要加上一个偏置量即
x / (2^k) = (x < 0 ? (x + (1 << k) - 1) : x) >> k
这样的话就会向0取整，比如-5 / 2 = -2.5 = -2
*/
/*struct DIV_T xDiv(int a, int b)
{
    //符号位，这是一个常数，在编译时候就能计算出来
    enum {SIGN_BIT = 1 << ((sizeof(int) << 3) - 1)};
    //是否为负数 
    int neg = a & SIGN_BIT;
    // 偏置量，负数的话要加上偏置量 
    int bias = ((1 - !neg) << b) - (1 && neg);
	
    struct DIV_T tmp;
	
    tmp.quot = (a + bias) >> b;     //a除以b
    tmp.rem = a - (tmp.quot << b);
	
    return tmp;
	
}  */

//-------------------------------------------------------------------------
//	功能：增加经验(不需要再经过队伍分配的处理)
//  nExp 增加的经验  nTarLevel 被打死的npc的等级
//	详情参阅 sourcesafe策划库\document\System\战斗经验设定.doc
//-------------------------------------------------------------------------
void	KPlayer::AddSelfExp(int nExp, int nTarLevel,int nGoldType)
{
	if (nExp <= 0)
		return;

	if (Npc[m_nIndex].m_Doing == do_death || Npc[m_nIndex].m_Doing == do_revive)
		return;

	int	nGetExp = 0, nPlayerLevel = 0;

	nPlayerLevel = Npc[m_nIndex].m_Level;
	if (nPlayerLevel > 100) 
		nPlayerLevel = 100;

	int	nSubLevel = abs(nPlayerLevel - nTarLevel);
	int g_ExpRate;
	g_GameOption.GetInteger("GameOption", "ServerExp", 1, &g_ExpRate);
	nExp *= g_ExpRate + g_GlobalMissionArray.GetMissionValue(27);

	if ((Npc[m_nIndex].m_Level >= nTarLevel) && (nTarLevel >= 100))
	{
		nGetExp = nExp - nExp * nSubLevel / 200;
	}
	else
	{
		if (nSubLevel <= 5)
		{
			nGetExp = nExp;
		}
		else if (nSubLevel <= 15)
		{
			nGetExp = nExp * (295 - 19 * nSubLevel) / 200;
		}
		else
		{
			nGetExp = 1;
		}
	} 
	
	if (nGetExp <= 0) 
		nGetExp = 1;

	if(Npc[m_nIndex].m_CurrentUpExp)//check tien thao lo x2 
		nGetExp = nGetExp * 2;


	int g_ExpLimit;//gioi han exp
	g_GameOption.GetInteger("GameOption", "ServerExpLimit", 1, &g_ExpLimit);
	int nLeftExp = m_cTask.GetSaveVal(TASKVALUE_LIMIT_EXP);
	if (nLeftExp > g_ExpLimit)
		return;

	m_cTask.SetSaveVal(TASKVALUE_LIMIT_EXP, nLeftExp + nGetExp);
	  

	m_nExp += nGetExp;

	int g_Skill120ExpRate;
	g_GameOption.GetInteger("GameOption", "Skill120Rate", 0, &g_Skill120ExpRate);
	if(g_Skill120ExpRate)
		AddSkillExp(nGetExp * g_Skill120ExpRate / MAX_PERCENT);

	if (m_nExp > m_nNextLevelExp)	// 升级
	{
			LevelUp();
			return;
	}
	else if (m_nExp == m_nNextLevelExp)						// 同步经验
	{
		if (Npc[m_nIndex].m_Level < MAX_LEVEL)
		{
			LevelUp();
			return;
		}
	}


	PLAYER_EXP_SYNC	sSyncExp;
	sSyncExp.ProtocolType = (BYTE)s2c_playerexp;
	sSyncExp.m_nExp = m_nExp;
	g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sSyncExp, sizeof(PLAYER_EXP_SYNC));
}

//skill 12x
void KPlayer::AddSkillExp(int nExp)
{
	for (int i = 1; i < MAX_NPCSKILL; i++)
	{
		if (Npc[m_nIndex].m_SkillList.m_Skills[i].SkillLevel <= 0)
			continue;
		if (Npc[m_nIndex].m_SkillList.m_Skills[i].SkillLevel >= MAX_TRAIN_SKILLEXPLEVEL)
			continue;
		if (Npc[m_nIndex].m_SkillList.m_Skills[i].nCurSkillExp <= 0)
			continue;

		KSkill * pOrdinSkill = (KSkill *)g_SkillManager.GetSkill(Npc[m_nIndex].m_SkillList.m_Skills[i].SkillId, Npc[m_nIndex].m_SkillList.m_Skills[i].CurrentSkillLevel);
		if (pOrdinSkill) 
		{
			if (pOrdinSkill->GetIsExpSkill() && !pOrdinSkill->IsTargetEnemy())
			{
				if (Npc[m_nIndex].m_SkillList.IncreaseExp(i, nExp))
					this->UpdataCurData();

				PLAYER_SKILL_LEVEL_SYNC sSkill;
				sSkill.ProtocolType = s2c_playerskilllevel;
				sSkill.m_nSkillID = Npc[m_nIndex].m_SkillList.m_Skills[i].SkillId;
				sSkill.m_nSkillLevel = Npc[m_nIndex].m_SkillList.GetLevel(Npc[m_nIndex].m_SkillList.m_Skills[i].SkillId);
				sSkill.m_nSkillExp = Npc[m_nIndex].m_SkillList.GetCurSkillExp(Npc[m_nIndex].m_SkillList.m_Skills[i].SkillId);//muc do luyen
				sSkill.m_nLeavePoint = m_nSkillPoint;
				g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sSkill, sizeof(PLAYER_SKILL_LEVEL_SYNC));
			}
		}
	}	
}


void	KPlayer::setFindPathState(bool ver,bool synclient)
 {
	 m_IsInfindPathState = ver;

	 if (synclient)
	 {//是否同步到 客户端
		 PLAYER_FINDPATH_STATE sSync;
		 sSync.ProtocolType = s2c_findpath_state;
		 sSync.m_btstate = m_IsInfindPathState;
		 if (g_pServer)
			 g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sSync, sizeof(PLAYER_FINDPATH_STATE));
	 }
 }
#endif
//-------------------------------------------------------------------------
//	功能：直接增加经验值，不考虑其他因素
//-------------------------------------------------------------------------
void	KPlayer::DirectAddExp(int nExp)
{
	//kiem soat exp
	if (nExp == 0 || Npc[m_nIndex].m_Level >= MAX_LEVEL)
		return;
    int nMaxLevel=1000;

	 nMaxLevel  = _gameGlobalSet.PlayerInfo.nMaxLevel;
     //  g_GameSetTing.GetInteger("ServerConfig","PlayerMaxLevel",1000,&nMaxLevel);
     if (Npc[m_nIndex].m_Level >= (nMaxLevel+m_nLimitMaxLevel))
		 return;

	m_nExp += nExp;
    
#ifdef _SERVER
//	printf("-------[直接经验](%s)原始经验(%d)实际增加经验%d点-------\n", Npc[m_nIndex].Name,nExp,nExp);//nGetExp*m_nUpExp/100);

	if (m_nExp >= m_nNextLevelExp)	// 升级
		LevelUp();
	else								// 同步经验
	{
		PLAYER_EXP_SYNC	sSyncExp;
		sSyncExp.ProtocolType = (BYTE)s2c_playerexp;
		sSyncExp.m_nExp       = m_nExp;
		g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sSyncExp, sizeof(PLAYER_EXP_SYNC));
	}
#endif
}

//-------------------------------------------------------------------------
//	功能：玩家升一级
//-------------------------------------------------------------------------
void	KPlayer::LevelUp()
{
	m_nExp = 0;

	if (Npc[m_nIndex].m_Level >= MAX_LEVEL)
		return;

	Npc[m_nIndex].m_Level++;

	m_nNextLevelExp = PlayerSet.m_cLevelAdd.GetLevelExp(Npc[m_nIndex].m_Level);
	
	// -------------------------- 等级增加时各项属性增加 ----------------------
	// 增加未分配属性点
	m_nAttributePoint += PLAYER_LEVEL_ADD_ATTRIBUTE;   //一次增加5点属性
	// 增加未分配技能点
	m_nSkillPoint     += PLAYER_LEVEL_ADD_SKILL;       //一级增加1点技能点
	
	// 处理生命、体力、内力值（影响基本生命、体力、内力最大值的因素：等级、属性点，影响当前生命最大值的因素：装备、技能、药物（临时））
	LevelAddBaseLifeMax();
	LevelAddBaseStaminaMax();
	LevelAddBaseManaMax();
	// Npc[m_nIndex].ResetLifeReplenish();
	
	// 处理各种抗性的变化 火、冰、毒、电、物理  原始
	Npc[m_nIndex].m_FireResist				= PlayerSet.m_cLevelAdd.GetFireResist(Npc[m_nIndex].m_Series, Npc[m_nIndex].m_Level);
	Npc[m_nIndex].m_CurrentFireResist		= Npc[m_nIndex].m_FireResist;
	Npc[m_nIndex].m_ColdResist				= PlayerSet.m_cLevelAdd.GetColdResist(Npc[m_nIndex].m_Series, Npc[m_nIndex].m_Level);
	Npc[m_nIndex].m_CurrentColdResist		= Npc[m_nIndex].m_ColdResist;
	Npc[m_nIndex].m_PoisonResist			= PlayerSet.m_cLevelAdd.GetPoisonResist(Npc[m_nIndex].m_Series, Npc[m_nIndex].m_Level);
	Npc[m_nIndex].m_CurrentPoisonResist		= Npc[m_nIndex].m_PoisonResist;
	Npc[m_nIndex].m_LightResist				= PlayerSet.m_cLevelAdd.GetLightResist(Npc[m_nIndex].m_Series, Npc[m_nIndex].m_Level);
	Npc[m_nIndex].m_CurrentLightResist		= Npc[m_nIndex].m_LightResist;
	Npc[m_nIndex].m_PhysicsResist			= PlayerSet.m_cLevelAdd.GetPhysicsResist(Npc[m_nIndex].m_Series, Npc[m_nIndex].m_Level);
	Npc[m_nIndex].m_CurrentPhysicsResist	= Npc[m_nIndex].m_PhysicsResist;

	//Npc[m_nIndex].m_FireResistMax			= BASE_FIRE_RESIST_MAX;
	//Npc[m_nIndex].m_ColdResistMax			= BASE_COLD_RESIST_MAX;
	//Npc[m_nIndex].m_PoisonResistMax			= BASE_POISON_RESIST_MAX;
	//Npc[m_nIndex].m_LightResistMax			= BASE_LIGHT_RESIST_MAX;
	//Npc[m_nIndex].m_PhysicsResistMax		= BASE_PHYSICS_RESIST_MAX;
	
	int resistMax = 0;
	g_GameSetTing.GetInteger("SYSTEM", "BASE_FIRE_RESIST_MAX", BASE_FIRE_RESIST_MAX, &resistMax);
	Npc[m_nIndex].m_FireResistMax = resistMax;

	g_GameSetTing.GetInteger("SYSTEM", "BASE_COLD_RESIST_MAX", BASE_COLD_RESIST_MAX, &resistMax);
	Npc[m_nIndex].m_ColdResistMax = resistMax;

	g_GameSetTing.GetInteger("SYSTEM", "BASE_POISON_RESIST_MAX", BASE_POISON_RESIST_MAX, &resistMax);
	Npc[m_nIndex].m_PoisonResistMax = resistMax;

	g_GameSetTing.GetInteger("SYSTEM", "BASE_LIGHT_RESIST_MAX", BASE_LIGHT_RESIST_MAX, &resistMax);
	Npc[m_nIndex].m_LightResistMax = resistMax;

	g_GameSetTing.GetInteger("SYSTEM", "BASE_PHYSICS_RESIST_MAX", BASE_PHYSICS_RESIST_MAX, &resistMax);
	Npc[m_nIndex].m_PhysicsResistMax = resistMax;

/*
	Npc[m_nIndex].m_CurrentFireResistMax	= Npc[m_nIndex].m_FireResistMax;
	Npc[m_nIndex].m_CurrentColdResistMax	= Npc[m_nIndex].m_ColdResistMax;
	Npc[m_nIndex].m_CurrentPoisonResistMax	= Npc[m_nIndex].m_PoisonResistMax;
	Npc[m_nIndex].m_CurrentLightResistMax	= Npc[m_nIndex].m_LightResistMax;
	Npc[m_nIndex].m_CurrentPhysicsResistMax	= Npc[m_nIndex].m_PhysicsResistMax;
*/
	int nOldCurCamp = Npc[m_nIndex].m_CurrentCamp;
	// 根据装备、技能信息更新当前数据
	this->UpdataCurData();
	SetNpcPhysicsDamage();
	Npc[m_nIndex].m_CurrentCamp = nOldCurCamp;
	
	// 生命、体力、内力值加满
	Npc[m_nIndex].m_CurrentLife = Npc[m_nIndex].m_CurrentLifeMax;
	Npc[m_nIndex].m_CurrentStamina = Npc[m_nIndex].m_CurrentStaminaMax;
	Npc[m_nIndex].m_CurrentMana = Npc[m_nIndex].m_CurrentManaMax;
	
#ifdef _SERVER
	// 给客户端发送数据
	// 自己的数据
	PLAYER_LEVEL_UP_SYNC	sLevelUp;
	sLevelUp.ProtocolType		= s2c_playerlevelup;
	sLevelUp.m_btLevel			= (WORD)Npc[m_nIndex].m_Level;
	sLevelUp.m_nExp				= m_nExp;
	sLevelUp.m_nAttributePoint	= m_nAttributePoint;
	sLevelUp.m_nSkillPoint		= m_nSkillPoint;
	sLevelUp.m_nBaseLifeMax		= Npc[m_nIndex].m_LifeMax;
	sLevelUp.m_nBaseStaminaMax	= Npc[m_nIndex].m_StaminaMax;
	sLevelUp.m_nBaseManaMax		= Npc[m_nIndex].m_ManaMax;
	g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sLevelUp, sizeof(PLAYER_LEVEL_UP_SYNC));
	// 给队友发送等级数据
	if (m_cTeam.m_nFlag)
	{
		PLAYER_TEAMMATE_LEVEL_SYNC	sLevel;
		sLevel.ProtocolType = s2c_teammatelevel;
		sLevel.m_btLevel = (WORD)Npc[m_nIndex].m_Level;
		sLevel.m_dwTeammateID = Npc[m_nIndex].m_dwID;
		// 给队长发
		if (m_cTeam.m_nFigure != TEAM_CAPTAIN)
			g_pServer->PackDataToClient(Player[g_Team[m_cTeam.m_nID].m_nCaptain].m_nNetConnectIdx, (BYTE*)&sLevel, sizeof(PLAYER_TEAMMATE_LEVEL_SYNC));
		for (int i = 0; i < MAX_TEAM_MEMBER; ++i)
		{
			if (g_Team[m_cTeam.m_nID].m_nMember[i] >= 0 && g_Team[m_cTeam.m_nID].m_nMember[i] != GetPlayerIndex())
				g_pServer->PackDataToClient(Player[g_Team[m_cTeam.m_nID].m_nMember[i]].m_nNetConnectIdx, (BYTE*)&sLevel, sizeof(PLAYER_TEAMMATE_LEVEL_SYNC));
		}
	}

	m_cTong.UpdateLevel((WORD)Npc[m_nIndex].m_Level);

	ExecuteScriptC("\\script\\system\\player\\levelup.lua","LevelUp","","",0,0,0,0,0,FALSE);

#endif
}


void	KPlayer::SetLevel(int nLevel)
{
	if (nLevel <= 0 || nLevel > MAX_LEVEL)
		return;

	m_nExp = 0;
	#define		NEW_PLAYER_INI_FILE_NAME		"\\settings\\player\\NewPlayerIni%02d.ini"
	char cBaseFile[80];
	sprintf(cBaseFile, NEW_PLAYER_INI_FILE_NAME, Npc[m_nIndex].m_Series * 2 + Npc[m_nIndex].m_nSex);

	KIniFile BaseAttrib;
	if(!BaseAttrib.Load(cBaseFile)) return;

	int nBaseStr, nBaseDex, nBaseVit, nBaseEn;
	BaseAttrib.GetInteger("ROLE", "ipower", 0, &nBaseStr);
	BaseAttrib.GetInteger("ROLE", "iagility", 0, &nBaseDex);
	BaseAttrib.GetInteger("ROLE", "iouter", 0, &nBaseVit);
	BaseAttrib.GetInteger("ROLE", "iinside", 0, &nBaseEn);

	int nStr = m_nStrength - nBaseStr;
	int nDex = m_nDexterity - nBaseDex;
	int nVit = m_nVitality - nBaseVit;
	int nEn = m_nEngergy - nBaseEn;

	m_nStrength = nBaseStr;
	m_nDexterity = nBaseDex;
	m_nVitality = nBaseVit;
	m_nEngergy = nBaseEn;

	m_nCurStrength = m_nStrength;
	m_nCurDexterity = m_nDexterity;
	m_nCurVitality = m_nVitality;
	m_nCurEngergy = m_nEngergy;

	m_nNextLevelExp = PlayerSet.m_cLevelAdd.GetLevelExp(nLevel/*, Npc[m_nIndex].m_byTranslife*/);

	int nBaseLife, nBaseMana, nBaseStamina;
	BaseAttrib.GetInteger("ROLE", "imaxlife", 0, &nBaseLife);
	BaseAttrib.GetInteger("ROLE", "imaxinner", 0, &nBaseMana);
	BaseAttrib.GetInteger("ROLE", "imaxstamina", 0, &nBaseStamina);

	#define		PLAYER_PREFIX_LEVEL				1

	Npc[m_nIndex].m_LifeMax = nBaseLife + (nLevel - PLAYER_PREFIX_LEVEL) * PlayerSet.m_cLevelAdd.GetLifePerLevel(Npc[m_nIndex].m_Series);
	Npc[m_nIndex].m_ManaMax = nBaseMana + (nLevel - PLAYER_PREFIX_LEVEL) * PlayerSet.m_cLevelAdd.GetManaPerLevel(Npc[m_nIndex].m_Series);
	Npc[m_nIndex].m_StaminaMax = nBaseStamina + (nLevel - PLAYER_PREFIX_LEVEL) * PlayerSet.m_cLevelAdd.GetStaminaPerLevel(/*Npc[m_nIndex].m_nSex,*/ Npc[m_nIndex].m_Series);

	Npc[m_nIndex].m_CurrentLifeMax = Npc[m_nIndex].m_LifeMax;
	Npc[m_nIndex].m_CurrentManaMax = Npc[m_nIndex].m_ManaMax;
	Npc[m_nIndex].m_CurrentStaminaMax = Npc[m_nIndex].m_StaminaMax;

	m_nAttributePoint += nStr + nDex + nVit + nEn - (Npc[m_nIndex].m_Level - nLevel) * PLAYER_LEVEL_ADD_ATTRIBUTE;
	m_nSkillPoint += Npc[m_nIndex].m_SkillList.RollBackSkills(false) - (Npc[m_nIndex].m_Level - nLevel) * PLAYER_LEVEL_ADD_SKILL;
	SendSyncData_Skill();

	Npc[m_nIndex].m_FireResist				= PlayerSet.m_cLevelAdd.GetFireResist(Npc[m_nIndex].m_Series, nLevel);
	Npc[m_nIndex].m_CurrentFireResist		= Npc[m_nIndex].m_FireResist;
	Npc[m_nIndex].m_ColdResist				= PlayerSet.m_cLevelAdd.GetColdResist(Npc[m_nIndex].m_Series, nLevel);
	Npc[m_nIndex].m_CurrentColdResist		= Npc[m_nIndex].m_ColdResist;
	Npc[m_nIndex].m_PoisonResist			= PlayerSet.m_cLevelAdd.GetPoisonResist(Npc[m_nIndex].m_Series, nLevel);
	Npc[m_nIndex].m_CurrentPoisonResist		= Npc[m_nIndex].m_PoisonResist;
	Npc[m_nIndex].m_LightResist				= PlayerSet.m_cLevelAdd.GetLightResist(Npc[m_nIndex].m_Series, nLevel);
	Npc[m_nIndex].m_CurrentLightResist		= Npc[m_nIndex].m_LightResist;
	Npc[m_nIndex].m_PhysicsResist			= PlayerSet.m_cLevelAdd.GetPhysicsResist(Npc[m_nIndex].m_Series, nLevel);
	Npc[m_nIndex].m_CurrentPhysicsResist	= Npc[m_nIndex].m_PhysicsResist;
	Npc[m_nIndex].m_FireResistMax			= BASE_FIRE_RESIST_MAX;
	Npc[m_nIndex].m_ColdResistMax			= BASE_COLD_RESIST_MAX;
	Npc[m_nIndex].m_PoisonResistMax			= BASE_POISON_RESIST_MAX;
	Npc[m_nIndex].m_LightResistMax			= BASE_LIGHT_RESIST_MAX;
	Npc[m_nIndex].m_PhysicsResistMax		= BASE_PHYSICS_RESIST_MAX;
	Npc[m_nIndex].m_CurrentFireResistMax	= Npc[m_nIndex].m_FireResistMax;
	Npc[m_nIndex].m_CurrentColdResistMax	= Npc[m_nIndex].m_ColdResistMax;
	Npc[m_nIndex].m_CurrentPoisonResistMax	= Npc[m_nIndex].m_PoisonResistMax;
	Npc[m_nIndex].m_CurrentLightResistMax	= Npc[m_nIndex].m_LightResistMax;
	Npc[m_nIndex].m_CurrentPhysicsResistMax	= Npc[m_nIndex].m_PhysicsResistMax;

	int nOldCurCamp = Npc[m_nIndex].m_CurrentCamp;
	this->UpdataCurData();
	SetNpcPhysicsDamage();
	Npc[m_nIndex].SetCurrentCamp(nOldCurCamp);

	Npc[m_nIndex].m_CurrentLife = Npc[m_nIndex].m_CurrentLifeMax;
	Npc[m_nIndex].m_CurrentStamina = Npc[m_nIndex].m_CurrentStaminaMax;
	Npc[m_nIndex].m_CurrentMana = Npc[m_nIndex].m_CurrentManaMax;

	PLAYER_LEVEL_UP_SYNC	sLevelUp;
	sLevelUp.ProtocolType		= s2c_playerlevelup;
	sLevelUp.m_btLevel			= (DWORD)nLevel;
	sLevelUp.m_nExp				= m_nExp;
	sLevelUp.m_nAttributePoint	= m_nAttributePoint;
	sLevelUp.m_nSkillPoint		= m_nSkillPoint;
	sLevelUp.m_nBaseLifeMax		= Npc[m_nIndex].m_LifeMax;
	sLevelUp.m_nBaseStaminaMax	= Npc[m_nIndex].m_StaminaMax;
	sLevelUp.m_nBaseManaMax		= Npc[m_nIndex].m_ManaMax;
	g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sLevelUp, sizeof(PLAYER_LEVEL_UP_SYNC));

	Npc[m_nIndex].m_Level = nLevel;
	ExecuteScriptC("\\script\\system\\player\\levelup.lua", "LevelUp","","",0,0,0,0,0,FALSE);
}


// 同步客户端属性状态技能等数据
void	KPlayer::UpdataCurData(int nIsLoign,int nUnEquip) //更新自身的属性参数
{
	if (m_nIndex <= 0 || m_nIndex >= MAX_NPC)
		return;

	for (int j = 0 ; j< MAX_NPCSKILL;j++)
	{
		int nAdd = 	Npc[m_nIndex].m_SkillList.GetAddPoint(j); //获取该位置的额外技能点
		if (nAdd > 0)
		{
			nAdd = -nAdd;  //等于负的
			Npc[m_nIndex].m_SkillList.QeuipAddPoint(j,nAdd);             //设置该位置技能点为实际技能点
#ifdef _SERVER	
			Npc[m_nIndex].m_SkillList.IncreaseLevel(j,nAdd,FALSE,FALSE); //清楚额外技能等级被动状态
			int nSkillid= Npc[m_nIndex].m_SkillList.GetSkillId(j);
			if (nUnEquip && nSkillid>0)
			{
				PLAYER_SKILL_LEVEL_SYNC	sSkill;
				sSkill.ProtocolType  = s2c_playerskilllevel;
				sSkill.m_nSkillID    = nSkillid;
				sSkill.m_nSkillLevel = Npc[m_nIndex].m_SkillList.GetCurrentLevelByIdx(j);
				sSkill.m_nSkillExp = Npc[m_nIndex].m_SkillList.GetCurSkillExp(j);//muc do luyen
				sSkill.m_nLeavePoint = Player[Npc[m_nIndex].GetPlayerIdx()].m_nSkillPoint;
				sSkill.m_nAddPoint   = Npc[m_nIndex].m_SkillList.GetAddPoint(j);	//额外增加的技能点
				if (g_pServer)
					g_pServer->PackDataToClient(Player[Npc[m_nIndex].GetPlayerIdx()].m_nNetConnectIdx, (BYTE*)&sSkill, sizeof(PLAYER_SKILL_LEVEL_SYNC));
			}		
#endif
		}

		Npc[m_nIndex].m_SkillList.m_Skills[j].mAddPoint = 0;
		Npc[m_nIndex].m_SkillList.m_Skills[j].EnChance = 0;              // 技能的加成
		Npc[m_nIndex].m_SkillList.m_Skills[j].nTempEnChance = 0;
	}

	ZeroMemory(&Npc[m_nIndex].m_CurrentPhysicsMagicDamageP, sizeof(KMagicAttrib));
	ZeroMemory(&Npc[m_nIndex].m_CurrentPhysicsMagicDamageV, sizeof(KMagicAttrib));

	int nCurbei=0; //,nFomoidx=Npc[m_nIndex].GetCurFuMoIdx()
	
	/*if 	(nFomoidx>-1)
	{
		if (Npc[m_nIndex].GetCurFoMoSkllLevel(nFomoidx)>=5) //获取神将当前的等级
		{
			nCurbei=Npc[m_nIndex].GetCurFoMoSkllLevel(nFomoidx)/5;
		}
	} */

	Npc[m_nIndex].m_CurrentLifeMax		= Npc[m_nIndex].m_LifeMax*(100+/*m_cReBorn.GetReBornNum()**/m_cReBorn.GetReBornLifeMaxVal()+/*m_cVip.GetRoleVipNum()**/m_cVip.GetRoleVipLifeMaxVal()+nCurbei*50)/100;
	//;Npc[m_nIndex].TakeTrader(Npc[m_nIndex].m_LifeMax*(100+m_cReBorn.GetReBornNum()*50+nCurbei*50),100);
	//Npc[m_nIndex].m_LifeMax*(100+m_cReBorn.GetReBornNum()*50+nCurbei*50)/100;
	Npc[m_nIndex].m_CurrentManaMax		= Npc[m_nIndex].m_ManaMax*(100+/*m_cReBorn.GetReBornNum()**/m_cReBorn.GetReBornLifeMaxVal()+/*m_cVip.GetRoleVipNum()**/m_cVip.GetRoleVipLifeMaxVal()+nCurbei*50)/100;
	//Npc[m_nIndex].TakeTrader(Npc[m_nIndex].m_ManaMax*(100+m_cReBorn.GetReBornNum()*50+nCurbei*50),100);
	//Npc[m_nIndex].m_ManaMax*(100+m_cReBorn.GetReBornNum()*50+nCurbei*50)/100;
	Npc[m_nIndex].m_CurrentStaminaMax	= Npc[m_nIndex].m_StaminaMax;
	Npc[m_nIndex].m_CurrentAttackRating	= Npc[m_nIndex].m_AttackRating;
	Npc[m_nIndex].m_CurrentAttackSpeed	= Npc[m_nIndex].m_AttackSpeed;
	Npc[m_nIndex].m_CurrentCastSpeed	= Npc[m_nIndex].m_CastSpeed;
    Npc[m_nIndex].m_CurrentRunSpeed	    = Npc[m_nIndex].m_RunSpeed; 
	Npc[m_nIndex].m_CurrentJumpSpeed	= Npc[m_nIndex].m_JumpSpeed;
	Npc[m_nIndex].m_CurrentVisionRadius	= Npc[m_nIndex].m_VisionRadius;
	Npc[m_nIndex].m_CurrentWalkSpeed	= Npc[m_nIndex].m_WalkSpeed;
	Npc[m_nIndex].m_CurrentNuQiMax		= Npc[m_nIndex].m_NuqiMax;//怒气
	m_nCurStrength						= m_nStrength;
    if  (m_nCurStrength<0) m_nCurStrength=0;
	m_nCurDexterity						= m_nDexterity;
	 if  (m_nCurDexterity<0) m_nCurDexterity=0;
	m_nCurVitality						= m_nVitality;
	if  (m_nCurVitality<0) m_nCurVitality=0;
	m_nCurEngergy						= m_nEngergy;
	if  (m_nCurEngergy<0) m_nCurEngergy=0;
	m_nCurLucky							= m_nLucky;

	m_nTempLucky_p                      = 0;

	Npc[m_nIndex].m_TempFireResist = 0;	        // Npc的当前火抗性
	Npc[m_nIndex].m_TempColdResist = 0;	        // Npc的当前冰抗性
	Npc[m_nIndex].m_TempPoisonResist = 0;	    // Npc的当前毒抗性
	Npc[m_nIndex].m_TempLightResist = 0;	    // Npc的当前电抗性
	Npc[m_nIndex].m_TempPhysicsResist = 0;	    // Npc的当前物理抗性
	Npc[m_nIndex].m_CurrentHitNpcRecover = 0;
	Npc[m_nIndex].m_CurrentHitRank       = 0;

	Npc[m_nIndex].m_CurrentColdResist	= Npc[m_nIndex].m_ColdResist;  //原始的防御
	Npc[m_nIndex].m_CurrentDefend		= Npc[m_nIndex].m_Defend;
	Npc[m_nIndex].m_CurrentAddDexterity = 0;
	Npc[m_nIndex].m_CurrentFireResist	= Npc[m_nIndex].m_FireResist;
	Npc[m_nIndex].m_CurrentLifeReplenish= Npc[m_nIndex].m_LifeReplenish;
	Npc[m_nIndex].m_CurrentLifeReplenish_p	= 0;
	Npc[m_nIndex].m_CurrentManaReplenish_p  = 0;
	Npc[m_nIndex].m_CurrentHitRecover	= Npc[m_nIndex].m_HitRecover;  //Npc的受伤动作时间
	Npc[m_nIndex].m_CurrentLightResist	= Npc[m_nIndex].m_LightResist;
	Npc[m_nIndex].m_CurrentManaReplenish= Npc[m_nIndex].m_ManaReplenish;
	Npc[m_nIndex].m_CurrentPhysicsResist= Npc[m_nIndex].m_PhysicsResist;
	Npc[m_nIndex].m_CurrentPoisonResist	= Npc[m_nIndex].m_PoisonResist;
	Npc[m_nIndex].m_CurrentStaminaGain	= Npc[m_nIndex].m_StaminaGain;
	Npc[m_nIndex].m_CurrentStaminaLoss	= Npc[m_nIndex].m_StaminaLoss;

	//SetBaseSpeedAndRadius();
	Npc[m_nIndex].m_CurrentColdEnhance	= 0;

	Npc[m_nIndex].m_CurrentColdResistMax	= BASE_FANGYU_ALL_MAX+/*m_cReBorn.GetReBornNum()**/m_cReBorn.GetReBornFanYuMaxVal()+/*m_cVip.GetRoleVipNum()**/m_cVip.GetRoleVipFanYuMaxVal();//Npc[m_nIndex].m_ColdResistMax;
	Npc[m_nIndex].m_CurrentFireResistMax    = BASE_FANGYU_ALL_MAX+/*m_cReBorn.GetReBornNum()**/m_cReBorn.GetReBornFanYuMaxVal()+/*m_cVip.GetRoleVipNum()**/m_cVip.GetRoleVipFanYuMaxVal();
	Npc[m_nIndex].m_CurrentLightResistMax   = BASE_FANGYU_ALL_MAX+/*m_cReBorn.GetReBornNum()**/m_cReBorn.GetReBornFanYuMaxVal()+/*m_cVip.GetRoleVipNum()**/m_cVip.GetRoleVipFanYuMaxVal();//Npc[m_nIndex].m_LightResistMax;
	Npc[m_nIndex].m_CurrentPhysicsResistMax	= BASE_FANGYU_ALL_MAX+/*m_cReBorn.GetReBornNum()**/m_cReBorn.GetReBornFanYuMaxVal()+/*m_cVip.GetRoleVipNum()**/m_cVip.GetRoleVipFanYuMaxVal();//Npc[m_nIndex].m_PhysicsResistMax;
	Npc[m_nIndex].m_CurrentPoisonResistMax	= BASE_FANGYU_ALL_MAX+/*m_cReBorn.GetReBornNum()**/m_cReBorn.GetReBornFanYuMaxVal()+/*m_cVip.GetRoleVipNum()**/m_cVip.GetRoleVipFanYuMaxVal();//Npc[m_nIndex].m_PoisonResistMax;

	Npc[m_nIndex].m_CurrentDamage2Mana	= 0;
	Npc[m_nIndex].m_CurrentDamageReduce	= 0;
	Npc[m_nIndex].m_CurrentDeadlyStrike	= 0;
//	Npc[m_nIndex].m_CurrentElementDamageReduce = 0;
	Npc[m_nIndex].m_CurrentFireEnhance	= 0;
	
	Npc[m_nIndex].m_CurrentHandEnhance	= 0;
	Npc[m_nIndex].m_CurrentKnockBack	= 0;
	Npc[m_nIndex].m_CurrentLifeStolen	= 0;
//	ZeroMemory(&Npc[m_nIndex].m_WaiPhysicsDamage, sizeof(KMagicAttrib));
	Npc[m_nIndex].m_CurrentLightEnhance	= 0;
	Npc[m_nIndex].m_CurrentPoisonTime=0;

//	Npc[m_nIndex].m_CurrentManaPerEnemy	= 0;
	Npc[m_nIndex].m_CurrentManaStolen	= 0;
	Npc[m_nIndex].m_CurrentMeleeDmgRet	= 0;        // 近程反弹
	Npc[m_nIndex].m_CurrentHulueMeleeDmgRet=0;      // 忽略近程反弹多少点、
   	Npc[m_nIndex].m_CurrentHulueRangeDmgRet=0;      // 忽略远程程反弹多少点、
	Npc[m_nIndex].m_CurrentMeleeDmgRetPercent = 0;  // 近程反弹百分比
	Npc[m_nIndex].m_CurrentTempSpeed=0;

//	Npc[m_nIndex].m_CurrentPiercePercent	= 0;
	Npc[m_nIndex].m_CurrentPoisonEnhance	= 0;

	Npc[m_nIndex].m_CurrentRangeDmgRet	= 0;         //远程反弹
	Npc[m_nIndex].m_CurrentRangeDmgRetPercent	= 0; //远程反弹百分比
	Npc[m_nIndex].m_CurrentRangeEnhance	= 0;
	Npc[m_nIndex].m_CurrentSlowMissle	= 0;         //子弹减速
	Npc[m_nIndex].m_CurrentStaminaStolen	= 0;
	Npc[m_nIndex].m_CurrentAddPhysicsDamage = 0;      //外功普点
	m_nUpExp							= 0;
	Npc[m_nIndex].m_CurrentUpExp        = 0;
	Npc[m_nIndex].m_CurrentFreezeTimeReducePercent = 0;
	Npc[m_nIndex].m_CurrentPoisonTimeReducePercent = 0;
	Npc[m_nIndex].m_EnemyPoisonTimeReducePercent = 0;
	Npc[m_nIndex].m_CurrentStunTimeReducePercent = 0;
	Npc[m_nIndex].m_EnemyStunTimeReducePercent   = 0;
	Npc[m_nIndex].m_CurrentSerisesEnhance = 0;
	Npc[m_nIndex].m_CurrentSeriesAbate   = 0;
	Npc[m_nIndex].m_CurrentDamageReduce = 0;
	Npc[m_nIndex].m_CurrentdanggeRate = 0;  //档格
	Npc[m_nIndex].m_CurrentzhongjiRate = 0; //重击
	 Npc[m_nIndex].m_CurrentcjdanggeRate	= 0;               //拆解档格
	 Npc[m_nIndex].m_CurrentcjzhongjiRate	= 0;               //拆解重击
	 Npc[m_nIndex].m_Currentsorbdamage	= 0;                   //抵消伤害
	 Npc[m_nIndex].m_Currentsorbdamage_v=0;
	 Npc[m_nIndex].m_Currenadddamagev   = 0;
	 Npc[m_nIndex].m_Currenadddamagep   = 0;
	 Npc[m_nIndex].m_Currentpoisonres	= 0;                   //=忽略对方毒防:#d1-%
	 Npc[m_nIndex].m_Currentfireres	= 0;                       //=忽略对方火防:#d1-%
	 Npc[m_nIndex].m_Currentlightingres	= 0;                   //=忽略对方雷防:#d1-%
	 Npc[m_nIndex].m_Currentphysicsres	= 0;                   //=忽略对方普防:#d1-%
	 Npc[m_nIndex].m_Currentcoldres	= 0;                       //=忽略对方冰防:#d1-%
	 Npc[m_nIndex].m_Currentallres	= 0; 
	 Npc[m_nIndex].m_Currentnopkvalue	= 0;                   //=不增加PK值概率:#d1+%
	 Npc[m_nIndex].m_Currentbossdamage	= 0;                   //=对黄金boss攻击伤害<c=orange>#d1+%<c>
	 Npc[m_nIndex].m_Currentelementsenhance	= 0;               //=五行强化值：#d1-点。强化对相克五行的克制效果
     Npc[m_nIndex].m_Currentelementsresist	= 0;               //=五行弱化值：#d1-点。弱化受相克五行的克制效果
	 Npc[m_nIndex].m_Currentskillenhance=0;                    //技能的加成
	  //ZeroMemory(Npc[m_nIndex].m_CurrentSkillEnhance,sizeof(Npc[m_nIndex].m_CurrentSkillEnhance));

	 //std::_EnhanceInfo::iterator it;
	 /*typedef std::map<INT, CEnhanceInfo>::iterator it;
	 for( it = Npc[m_nIndex].nEnhanceInfo.begin(); it != Npc[m_nIndex].nEnhanceInfo.end(); ++it)
	 {
		 it->second.nSkillIdx = 0;
		 it->second.nEnhance = 0;
	 }*/
	 Npc[m_nIndex].nEnhanceInfo.clear();
	 Npc[m_nIndex].m_CurrentFullManaskillenhance=0;
	 Npc[m_nIndex].m_TempFullManaskillenhance = 0;
	 Npc[m_nIndex].m_CurrentautoReviverate=0;                  //复活概率
	 Npc[m_nIndex].m_CurrentAddmagicphysicsDamageP= 0;// 当前被动内普百分比
	 Npc[m_nIndex].m_CurrentPoisondamagereturnV=0;
	 Npc[m_nIndex].m_PoisonDamageReturn=0;
	 Npc[m_nIndex].m_Currentbaopoisondmax_p=0;
	 Npc[m_nIndex].m_ReturnSkill=0;
	 Npc[m_nIndex].m_IgnoreSkill=0;
	 Npc[m_nIndex].m_CurrentReturnResist=0;
	 Npc[m_nIndex].m_CurrentCreatnpcv=0;
	 Npc[m_nIndex].m_CurrentAllJiHuo=0; //是否全身激活
	 Npc[m_nIndex].m_CurrentCreatStatus=0;
	 Npc[m_nIndex].m_CurrentAttackRatingEnhancep=0;
	 Npc[m_nIndex].m_CurrentAttackRatingEnhancev=0;
	 Npc[m_nIndex].m_CurrentIgnorenAttacRating  =0;
	 Npc[m_nIndex].m_Me2metaldamage_p=0;              //=对金系伤害增加：#d1+%
	 Npc[m_nIndex].m_Metal2medamage_p=0;              //=减少来自金系的伤害：#d1-%
	 Npc[m_nIndex].m_Me2wooddamage_p=0;              //=对木系伤害增加：#d1+%
	 Npc[m_nIndex].m_Wood2medamage_p=0;              //=减少来自木系的伤害：#d1-%
	 Npc[m_nIndex].m_Me2waterdamage_p=0;              //=对水系伤害增加：#d1+%
	 Npc[m_nIndex].m_Water2medamage_p=0;              //=减少来自水系的伤害：#d1-%
	 Npc[m_nIndex].m_Me2firedamage_p=0;              //=对火系伤害增加：#d1+%
	 Npc[m_nIndex].m_Fire2medamage_p=0;              //=减少来自火系的伤害：#d1-%
	 Npc[m_nIndex].m_Me2earthdamage_p=0;              //=对土系伤害增加：#d1+%
	 Npc[m_nIndex].m_Earth2medamage_p=0;              //=减少来自土系的伤害：#d1-%
     Npc[m_nIndex].m_CurrentStunRank_p=0;
	 Npc[m_nIndex].m_Staticmagicshield_p=0;
	 Npc[m_nIndex].m_CurrentPiercePercent	= 0;

	Npc[m_nIndex].m_ManaShield			= 0;

	ZeroMemory(&Npc[m_nIndex].m_CurrentMeleeEnhance, sizeof(Npc[m_nIndex].m_CurrentMeleeEnhance));

	ZeroMemory(&Npc[m_nIndex].m_CurrentPoisonDamage, sizeof(KMagicAttrib));
	ZeroMemory(&Npc[m_nIndex].m_CurrentColdDamage, sizeof(KMagicAttrib));
	ZeroMemory(&Npc[m_nIndex].m_CurrentFireDamage, sizeof(KMagicAttrib));
	ZeroMemory(&Npc[m_nIndex].m_CurrentLightDamage, sizeof(KMagicAttrib));
	ZeroMemory(&Npc[m_nIndex].m_CurrentMeleeEnhance, sizeof(Npc[m_nIndex].m_CurrentMeleeEnhance));

	
	ZeroMemory(&Npc[m_nIndex].m_MagicPhysicsDamage, sizeof(KMagicAttrib));
	ZeroMemory(&Npc[m_nIndex].m_CurrentMagicColdDamage, sizeof(KMagicAttrib));
	ZeroMemory(&Npc[m_nIndex].m_CurrentMagicFireDamage, sizeof(KMagicAttrib));
	ZeroMemory(&Npc[m_nIndex].m_CurrentMagicLightDamage, sizeof(KMagicAttrib));
	ZeroMemory(&Npc[m_nIndex].m_CurrentMagicPoisonDamage, sizeof(KMagicAttrib));	
	

	ZeroMemory(&Npc[m_nIndex].m_AutoAttackSkill, sizeof(KMagicAttrib));
	ZeroMemory(&Npc[m_nIndex].m_AutoReplySkill, sizeof(KMagicAttrib));
	ZeroMemory(&Npc[m_nIndex].m_AutoDeathSkill, sizeof(KMagicAttrib));
	ZeroMemory(&Npc[m_nIndex].m_AutoRescueSkill, sizeof(KMagicAttrib));

	 ReCalcEquip();         //重新计算身上的装备
	 ReCalcState(nIsLoign); //重新计算技能的状态
}

void KPlayer::ReCalcEquip()
{
	_ASSERT(m_nIndex > 0 && m_nIndex < MAX_NPC);
	
	for (int i = 0; i < itempart_num; ++i)
	{
		int nIdx = m_ItemList.m_EquipItem[i];
		if (!nIdx)
			continue;

		if (Item[nIdx].GetIsCanUse()==0)  //不可用的
		    continue;

		int nActive = m_ItemList.GetEquipEnhance(i);  //获取激活的数量

		if (m_ItemList.m_bActiveSet)  //如果是黄金套装激活的 或者 全身激活
		{
			nActive = 3;              //默认的黄金明属性
		}
//除马匹
		if (i != itempart_horse || Npc[m_nIndex].m_bRideHorse)  //除掉马的部位 或 骑着马
		{
			int nActiveInfo[2];
			
			ZeroMemory(nActiveInfo,sizeof(nActiveInfo));
			
			if (Item[nIdx].IsBlue())
			{//如果是蓝装
				nActiveInfo[0] = 1;
				nActiveInfo[1] = Item[nIdx].GetIsWhere();
				
			} 
			else if  (Item[nIdx].IsPurple())
			{//如果是紫装
				nActiveInfo[0] = 2;
				nActiveInfo[1] = Item[nIdx].GetIsWhere();
			} 
			else if  (Item[nIdx].IsGold())
			{//如果是黄装
				nActiveInfo[0] = 3;
				nActiveInfo[1] = Item[nIdx].GetLevel();//等级
			} 
			else
			{
				nActiveInfo[0] = 0;
				nActiveInfo[1] = 0;
			}
			
			if (Item[nIdx].GetIsCanUse()==1)   //是可以使用的
			{     
			   Item[nIdx].ApplyMagicAttribToNPC(&Npc[m_nIndex], nActive,nActiveInfo);
			}
			else if (Item[nIdx].GetIsCanUse()==0 && Item[nIdx].GetGenre()==item_equip && Item[nIdx].GetDetailType()==equip_mask) 
			{//面具
               Item[nIdx].ApplyMagicAttribToNPC(&Npc[m_nIndex], nActive,nActiveInfo);
			}

		}
	}
}

void KPlayer::ReCalcState(int nIsLoign)
{
	_ASSERT(m_nIndex > 0 && m_nIndex < MAX_NPC);
	
	Npc[m_nIndex].ReCalcStateEffect();        //重置状态效果

	Npc[m_nIndex].m_SkillList.ReEnChance();   //重置加成
	
	if (!nIsLoign)
	   Npc[m_nIndex].NpcNewInitJinMaiVal();   //重置经脉状态
	       
	//Npc[m_nIndex].ReSkillEnhance(m_nLeftSkillID,1); //重置对攻击技能的加成!
}

//-------------------------------------------------------------------------
//	功能：增加统率力经验
//-------------------------------------------------------------------------
void	KPlayer::AddLeadExp(int nExp)
{
	if (m_dwLeadLevel >= MAX_LEAD_LEVEL)
		return;
	if (Npc[m_nIndex].m_Doing == do_death || Npc[m_nIndex].m_Doing == do_revive)
		return;
	m_dwLeadExp += nExp;
	if (m_dwLeadExp >= (DWORD)PlayerSet.m_cLeadExp.GetLevelExp(m_dwLeadLevel))
	{
		// 升级
		m_dwLeadLevel = PlayerSet.m_cLeadExp.GetLevel(m_dwLeadExp, m_dwLeadLevel);
#ifndef _SERVER
		m_dwNextLevelLeadExp = PlayerSet.m_cLeadExp.GetLevelExp(m_dwLeadLevel);
#endif
	}
	
#ifdef _SERVER
	PLAYER_LEAD_EXP_SYNC	sLeadExp;
	sLeadExp.ProtocolType = s2c_playersyncleadexp;
	sLeadExp.m_dwLeadExp = m_dwLeadExp;
	g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sLeadExp, sizeof(PLAYER_LEAD_EXP_SYNC));
#endif
	
}

//-------------------------------------------------------------------------
//	功能：原始玩家升一级加生命
//-------------------------------------------------------------------------
void KPlayer::LevelAddBaseLifeMax()
{
	Npc[m_nIndex].m_LifeMax += PlayerSet.m_cLevelAdd.GetLifePerLevel(Npc[m_nIndex].m_Series);
	Npc[m_nIndex].m_CurrentLifeMax = Npc[m_nIndex].m_LifeMax;
}

//-------------------------------------------------------------------------
//	功能：玩家升一级加体力
//-------------------------------------------------------------------------
void KPlayer::LevelAddBaseStaminaMax()
{
	Npc[m_nIndex].m_StaminaMax += PlayerSet.m_cLevelAdd.GetStaminaPerLevel(Npc[m_nIndex].m_Series);
	Npc[m_nIndex].m_CurrentStaminaMax = Npc[m_nIndex].m_StaminaMax;
}

//-------------------------------------------------------------------------
//	功能：玩家升一级加内力
//-------------------------------------------------------------------------
void KPlayer::LevelAddBaseManaMax()
{
	Npc[m_nIndex].m_ManaMax += PlayerSet.m_cLevelAdd.GetManaPerLevel(Npc[m_nIndex].m_Series);
	Npc[m_nIndex].m_CurrentManaMax = Npc[m_nIndex].m_ManaMax;
}

//-------------------------------------------------------------------------
//	功能：改变玩家阵营
//-------------------------------------------------------------------------
void	KPlayer::ChangePlayerCamp(int nCamp)
{
	if (nCamp < camp_begin || nCamp >= camp_num)
		return;
	Npc[m_nIndex].m_Camp = (NPCCAMP)nCamp;
}

#ifndef _SERVER
//-------------------------------------------------------------------------
//	功能：向服务器申请查询某个npc所在队伍的信息
//-------------------------------------------------------------------------
void	KPlayer::ApplyTeamInfo(DWORD dwNpcID)
{
	if (dwNpcID <= 0)
		return;
	PLAYER_APPLY_TEAM_INFO	sApplyInfo;
	sApplyInfo.ProtocolType = (BYTE)c2s_teamapplyinfo;
	sApplyInfo.m_dwTarNpcID = dwNpcID;
	if (g_pClient)
		g_pClient->SendPackToServer(&sApplyInfo, sizeof(PLAYER_APPLY_TEAM_INFO));
}

//-------------------------------------------------------------------------
//	功能：向服务器报告包袱不足了
//-------------------------------------------------------------------------
void	KPlayer::ApplyRoomInfo(int nIsFull)
{
	if (nIsFull < 0)
		return;
	PLAYER_NO_ROOM_INFO	sApplyInfo;
	sApplyInfo.ProtocolType = (BYTE)c2s_roomisfull;
	sApplyInfo.m_IsNoRoom = nIsFull;
	m_nRoomFull           = nIsFull;
	if (g_pClient)
		g_pClient->SendPackToServer(&sApplyInfo, sizeof(PLAYER_NO_ROOM_INFO));
}

#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	功能：向服务器申请查询玩家自身的队伍情况
//-------------------------------------------------------------------------
void	KPlayer::ApplySelfTeamInfo()
{
	ApplyTeamInfo(Npc[m_nIndex].m_dwID);
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	功能：玩家向服务器申请创建队伍
//-------------------------------------------------------------------------
BOOL	KPlayer::ApplyCreateTeam()//char *lpszTeamName)
{
	return m_cTeam.ApplyCreate();//lpszTeamName);
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	功能：队长向服务器申请开放、关闭队伍是否允许加入成员状态
//-------------------------------------------------------------------------
BOOL	KPlayer::ApplyTeamOpenClose(BOOL bFlag)
{
	if ( !m_cTeam.m_nFlag )					// 未组队
		return FALSE;
	if (m_cTeam.m_nFigure != TEAM_CAPTAIN)	// 不是队长
		return FALSE;
	if (this->CheckTrading())	// 处在交易状态
		return FALSE;
	
	if (bFlag)
	{
		if (PlayerSet.m_cLeadExp.GetMemNumFromLevel(m_dwLeadLevel) <= g_Team[0].m_nMemNum)	// 统帅力不够
			return FALSE;
	}
	
	PLAYER_TEAM_OPEN_CLOSE	sTeamState;
	sTeamState.ProtocolType = c2s_teamapplyopenclose;
	sTeamState.m_btOpenClose = bFlag;
	if (g_pClient)
		g_pClient->SendPackToServer(&sTeamState, sizeof(PLAYER_TEAM_OPEN_CLOSE));
	return TRUE;
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	功能：玩家向服务器申请加入某个队伍
//	参数：int nNpcIndex 目标队伍队长在客户端Npc数组中的位置
//-------------------------------------------------------------------------
void	KPlayer::ApplyAddTeam(int nNpcIndex)
{
	if (this->CheckTrading())
		return;
	if (m_cTeam.m_nFlag)					// 已经组队，不能申请加入别的队伍
		return;
	
	// 不同阵营现在可以组队，老手不能加入新人队伍，新人可以加入老手队伍
	if (Npc[nNpcIndex].m_Camp == camp_begin && Npc[m_nIndex].m_Camp != camp_begin)
		return;
	//	if (Npc[nNpcIndex].m_Camp != Npc[m_nIndex].m_Camp)	// 阵营不同
	//		return;
	
	// 判断目标队伍队长是不是处在队伍开放状态
	if (Npc[nNpcIndex].GetMenuState() != PLAYER_MENU_STATE_TEAMOPEN)
		return;
	
	m_cTeam.m_nApplyCaptainID = Npc[nNpcIndex].m_dwID;
	m_cTeam.m_dwApplyTimer = MAX_APPLY_TEAM_TIME;
	PLAYER_APPLY_ADD_TEAM	sAddTeam;
	sAddTeam.ProtocolType = c2s_teamapplyadd;
	sAddTeam.m_dwTarNpcID = Npc[nNpcIndex].m_dwID;
	if (g_pClient)
		g_pClient->SendPackToServer(&sAddTeam, sizeof(PLAYER_APPLY_ADD_TEAM));
	
	KSystemMessage	sMsg;
	sprintf(sMsg.szMessage, MSG_TEAM_APPLY_ADD_SELF_MSG, Npc[nNpcIndex].Name);
	sMsg.eType = SMT_NORMAL;
	sMsg.byConfirmType = SMCT_NONE;
	sMsg.byPriority = 0;
	sMsg.byParamSize = 0;
	sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
	CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	功能：玩家通知服务器接受某个npc为队伍成员
//-------------------------------------------------------------------------
void	KPlayer::AcceptTeamMember(DWORD dwNpcID)
{
	// 判断状态 （缺少对npc队伍开放状态的检测 not end）
	if ( !m_cTeam.m_nFlag || m_cTeam.m_nFigure != TEAM_CAPTAIN)
	{
		ApplySelfTeamInfo();
		return;
	}
	if (dwNpcID == 0)
		return;
	
	// 查找申请列表
	int		i;
	for (i = 0; i < MAX_TEAM_APPLY_LIST; ++i)
	{
		if (m_cTeam.m_sApplyList[i].m_dwNpcID == dwNpcID)
		{
			m_cTeam.m_sApplyList[i].m_dwNpcID = 0;
			m_cTeam.UpdateInterface();
			break;
		}
	}
	// 如果没找到
//	if (i >= MAX_TEAM_APPLY_LIST)
//		return;
	
	// 向服务器发申请
	PLAYER_ACCEPT_TEAM_MEMBER	sAcceptTeam;
	sAcceptTeam.ProtocolType = c2s_teamacceptmember;
	sAcceptTeam.m_dwNpcID = dwNpcID;
	if (g_pClient)
		g_pClient->SendPackToServer(&sAcceptTeam, sizeof(PLAYER_ACCEPT_TEAM_MEMBER));
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	功能：队长删除加入队伍申请列表中的某个npc
//-------------------------------------------------------------------------
void	KPlayer::TeamDropApplyOne(DWORD dwNpcID)
{
	if (dwNpcID == 0)
		return;
	// 查找申请列表
	int		i;
	for (i = 0; i < MAX_TEAM_APPLY_LIST; ++i)
	{
		if (m_cTeam.m_sApplyList[i].m_dwNpcID == dwNpcID)
		{
			m_cTeam.m_sApplyList[i].m_dwNpcID = 0;
			m_cTeam.m_sApplyList[i].m_dwTimer = 0;
			m_cTeam.m_sApplyList[i].m_nLevel = 0;
			m_cTeam.m_sApplyList[i].m_szName[0] = 0;
			break;
		}
	}
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	功能：通知服务器本玩家离开队伍
//-------------------------------------------------------------------------
void	KPlayer::LeaveTeam()
{
	PLAYER_APPLY_LEAVE_TEAM	sLeaveTeam;
	sLeaveTeam.ProtocolType = c2s_teamapplyleave;
	if (g_pClient)
		g_pClient->SendPackToServer(&sLeaveTeam, sizeof(PLAYER_APPLY_LEAVE_TEAM));
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	功能：队长通知服务器踢除某个队员
//-------------------------------------------------------------------------
void	KPlayer::TeamKickMember(DWORD dwNpcID)
{
	if ( !m_cTeam.m_nFlag || m_cTeam.m_nFigure != TEAM_CAPTAIN)
		return;
	
	PLAYER_TEAM_KICK_MEMBER	sKickOne;
	sKickOne.ProtocolType = c2s_teamapplykickmember;
	sKickOne.m_dwNpcID = dwNpcID;
	if (g_pClient)
		g_pClient->SendPackToServer(&sKickOne, sizeof(PLAYER_TEAM_KICK_MEMBER));
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	功能：队长向服务器申请把自己的队长身份交给别的队员
//-------------------------------------------------------------------------
void	KPlayer::ApplyTeamChangeCaptain(DWORD dwNpcID)
{
	if ( !m_cTeam.m_nFlag || m_cTeam.m_nFigure != TEAM_CAPTAIN)
		return;
	
	PLAYER_APPLY_TEAM_CHANGE_CAPTAIN	sTeamChange;
	sTeamChange.ProtocolType = c2s_teamapplychangecaptain;
	sTeamChange.m_dwNpcID = dwNpcID;
	if (g_pClient)
		g_pClient->SendPackToServer(&sTeamChange, sizeof(PLAYER_APPLY_TEAM_CHANGE_CAPTAIN));
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	功能：队长向服务器申请解散队伍
//-------------------------------------------------------------------------
void	KPlayer::ApplyTeamDismiss()
{
	if ( !m_cTeam.m_nFlag || m_cTeam.m_nFigure != TEAM_CAPTAIN)
		return;
	
	PLAYER_APPLY_TEAM_DISMISS	sTeamDismiss;
	sTeamDismiss.ProtocolType = c2s_teamapplydismiss;
	if (g_pClient)
		g_pClient->SendPackToServer(&sTeamDismiss, sizeof(PLAYER_APPLY_TEAM_DISMISS));
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	功能：玩家向服务器申请打开、关闭pk开关
//-------------------------------------------------------------------------
void	KPlayer::ApplySetPK(BOOL bPK)
{
	PLAYER_SET_PK	sPK;
	sPK.ProtocolType = c2s_playerapplysetpk;
	sPK.m_btPKFlag = bPK;
	if (g_pClient)
		g_pClient->SendPackToServer(&sPK, sizeof(PLAYER_SET_PK));
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	功能：玩家向服务器申请门派数据
//-------------------------------------------------------------------------
void	KPlayer::ApplyFactionData()
{
	PLAYER_APPLY_FACTION_DATA	sFaction;
	sFaction.ProtocolType = c2s_playerapplyfactiondata;
	if (g_pClient)
		g_pClient->SendPackToServer(&sFaction, sizeof(PLAYER_APPLY_FACTION_DATA));
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	功能：客户端发送聊天语句给服务器
//-------------------------------------------------------------------------
void	KPlayer::SendChat(KUiMsgParam *pMsg, char *lpszSentence)
{
	if (!lpszSentence || !pMsg || pMsg->nMsgLength <= 0)
		return;
	int nLength = pMsg->nMsgLength;
	if (nLength >= MAX_SENTENCE_LENGTH)
		nLength = MAX_SENTENCE_LENGTH - 1;
	
	if (pMsg->eGenre == MSG_G_CHAT)
	{
		Npc[m_nIndex].SetChatInfo(pMsg->szName, lpszSentence, nLength);  //处理自己聊天内容 赋值给 m_szChatBuffer
		//char m_szChatBuffer[256]={0};
		//Npc[m_nIndex].GetstrInfo(STR_CHATBUFF_CLIENT,m_szChatBuffer);
		m_cChat.SendSentence(pMsg,Npc[m_nIndex].m_szChatBuffer);        //发送给服务器
	}
	else if (pMsg->eGenre == MSG_G_CMD)	// not end
	{

	}
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	功能：队长向服务器申请增加四项属性中某一项的点数(0=Strength 1=Dexterity 2=Vitality 3=Engergy)
//-------------------------------------------------------------------------
void	KPlayer::ApplyAddBaseAttribute(int nAttribute, int nNo)
{
	if (nAttribute < 0 || nAttribute > 3)
		return;
	if (nNo <= 0 || nNo >= 255)
		return;
	PLAYER_ADD_BASE_ATTRIBUTE_COMMAND	sAdd;
	sAdd.ProtocolType = c2s_playeraddbaseattribute;
	sAdd.m_btAttribute = nAttribute;
	sAdd.m_nAddNo = nNo;
	if (g_pClient)
		g_pClient->SendPackToServer(&sAdd, sizeof(PLAYER_ADD_BASE_ATTRIBUTE_COMMAND));
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	功能：向服务器申请某个技能升级
//-------------------------------------------------------------------------
BOOL	KPlayer::ApplyAddSkillLevel(int nSkillID, int nAddPoint)
{
	int nListidx = Npc[m_nIndex].m_SkillList.FindSame(nSkillID);
	if (!nListidx) //查找是否有这个技能
		return FALSE;
	if (m_nSkillPoint < nAddPoint || nAddPoint + Npc[m_nIndex].m_SkillList.GetLevelByListidx(nListidx) >=MAX_SKILLLEVEL)
		return FALSE;
	
	PLAYER_ADD_SKILL_POINT_COMMAND	sSkill;
	sSkill.ProtocolType = c2s_playerapplyaddskillpoint;
	sSkill.m_nSkillID = nSkillID;
	sSkill.m_nAddPoint= nAddPoint; //增加的技能点
	
	if (g_pClient)
		g_pClient->SendPackToServer(&sSkill, sizeof(PLAYER_ADD_SKILL_POINT_COMMAND));
	
	return TRUE;
}
#endif
//客户端 
#ifndef _SERVER
//-------------------------------------------------------------------------
//	功能：向服务器申请使用某个物品（鼠标右键点击该物品，只能用于吃药）
//-------------------------------------------------------------------------
BOOL	KPlayer::ApplyUseItem(int nItemID, ItemPos SrcPos)
{
	if (this->CheckTrading())
		return FALSE;

	int nRet = m_ItemList.UseItem(nItemID,SrcPos.nX,SrcPos.nY);  //同意使用物品
	if (nRet == 0)
		return FALSE;
	
	if (nRet == REQUEST_EQUIP_ITEM)         //物品装备---右键换装-设置
	{//等于装备
	/*	PLAYER_EQUIP_ITEM_COMMAND sEquip;
		sEquip.m_nItemID = nItemID;
			int nDetail = Item[pInfo->Obj.uId].GetDetailType();
		    int nPartuil = Item[pInfo->Obj.uId].GetParticular();
        */
		/*KSystemMessage	sMsg;
		sMsg.byConfirmType = SMCT_NONE;
		sMsg.eType = SMT_NORMAL;
		sMsg.byParamSize = 0;
		sMsg.byPriority = 0;	
		strcpy(sMsg.szMessage, MSG_HUANZHUANG_NO_ROOM);
		sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);*/
     
	}
	else if (nRet == REQUEST_EAT_MEDICINE)  // 药品
	{
		PLAYER_EAT_ITEM_COMMAND	sEat;
		sEat.ProtocolType = c2s_playereatitem;
		sEat.m_nItemID    = nItemID;
		sEat.m_btPlace    = SrcPos.nPlace;
		sEat.m_btX        = SrcPos.nX;
		sEat.m_btY        = SrcPos.nY;
		if (g_pClient)
			g_pClient->SendPackToServer(&sEat, sizeof(PLAYER_EAT_ITEM_COMMAND));
	}
	
	return TRUE;
}

BOOL KPlayer::ApplyAutoMoveItem(int nItemID, ItemPos SrcPos,int destRoom)
{
	if (this->CheckTrading())
		return FALSE;

		PLAYER_AUTOMUVE_ITEM_COMMAND	sMoveItem;
		sMoveItem.ProtocolType = c2s_playerAutoMoveitem;
		sMoveItem.m_nItemID    = Item[nItemID].GetID();//nItemdwid;
		sMoveItem.m_btPlace    = SrcPos.nPlace;
		sMoveItem.m_btX        = SrcPos.nX;
		sMoveItem.m_btY        = SrcPos.nY;
        sMoveItem.m_destRoom   = destRoom;
		sMoveItem.m_ItemWidth  = Item[nItemID].GetWidth();
        sMoveItem.m_ItemHigth  = Item[nItemID].GetHeight();

		if (g_pClient)
			g_pClient->SendPackToServer(&sMoveItem, sizeof(PLAYER_AUTOMUVE_ITEM_COMMAND));

	return TRUE;
}

#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	功能：客户端鼠标点击obj检起某个物品，向服务器发消息
//-------------------------------------------------------------------------
void	KPlayer::PickUpObj(int nObjIndex)
{
	if (this->CheckTrading())
		return;
	if (nObjIndex <= 0)
		return;
	if (Object[nObjIndex].m_nKind != Obj_Kind_Item && Object[nObjIndex].m_nKind != Obj_Kind_Money)
		return;
	
	PLAYER_PICKUP_ITEM_COMMAND	sPickUp;
	if (Object[nObjIndex].m_nKind == Obj_Kind_Money)
	{//钱
		sPickUp.ProtocolType = c2s_playerpickupitem;
		sPickUp.m_nObjID = Object[nObjIndex].m_nID;
		sPickUp.m_btPosType = 0;
		sPickUp.m_btPosX = 0;
		sPickUp.m_btPosY = 0;

		 if (Npc[m_nIndex].m_AutoplayId==1)
		 {//挂机状态			
			if (m_Autoplay.nShiQuNum>m_Autoplay.nVal && m_Autoplay.nShiQuNum%m_Autoplay.nVal==0)
			{
                Object[nObjIndex].m_AttackerDwid=Npc[m_nIndex].m_dwID;
                Object[nObjIndex].m_IsHaveAttack=1;
			    Object[nObjIndex].m_AttackerTime=18*60*10;
				m_Autoplay.nShiQuNum=0;
				m_ItemList.ClientShowMsg("Tro giup: Kiem tra vat pham qua thoi gian, huy kiem tra!");
				return;
			}
			  m_Autoplay.nShiQuNum++;
		 }
		 else
		 {
		      m_Autoplay.nShiQuNum=0;
		 }
	}
	else
	{//其他的物品
		ItemPos	sItemPos;
		if (FALSE == m_ItemList.SearchPosition(Object[nObjIndex].m_nItemWidth, Object[nObjIndex].m_nItemHeight, &sItemPos))
		{
			if (FALSE == m_ItemList.SearchItemPosition(Object[nObjIndex].m_nItemWidth, Object[nObjIndex].m_nItemHeight, &sItemPos,room_equipmentex))
			{
				if (Npc[m_nIndex].m_AutoplayId==1)
				{ //挂机状态
					Object[nObjIndex].m_AttackerDwid=Npc[m_nIndex].m_dwID;
					Object[nObjIndex].m_IsHaveAttack=1;
					Object[nObjIndex].m_AttackerTime=18*60*10;
					//开始发送命令告诉服务器包袱不足了
					ApplyRoomInfo(1);
				}
				
				m_ItemList.ClientShowMsg("Tip: Chuyen luu tui m_ExItemId that bai, khong gian khong du!");
				return;
			}
			
			int nItemEX = Npc[m_nIndex].m_ExItemId; //字母袋是否开启
			
			if (nItemEX==0)
			{
				if (Npc[m_nIndex].m_AutoplayId==1)
				{ //挂机状态
					Object[nObjIndex].m_AttackerDwid=Npc[m_nIndex].m_dwID;
					Object[nObjIndex].m_IsHaveAttack=1;
					Object[nObjIndex].m_AttackerTime=18*60*10;
					//开始发送命令告诉服务器包袱不足了
					ApplyRoomInfo(1);
				}
				
				m_ItemList.ClientShowMsg("Tip: Chuyen luu tui m_ExItemId that bai, tui m_ExItemId chua duoc mo!");
				return;
			}	

		}
	
		 if (Npc[m_nIndex].m_AutoplayId==1)
		 {//挂机状态			
			if (m_Autoplay.nShiQuNum>m_Autoplay.nVal && m_Autoplay.nShiQuNum%m_Autoplay.nVal==0)
			{
                Object[nObjIndex].m_AttackerDwid=Npc[m_nIndex].m_dwID;
                Object[nObjIndex].m_IsHaveAttack=1;
			    Object[nObjIndex].m_AttackerTime=18*60*10;
				m_Autoplay.nShiQuNum=0;
				m_ItemList.ClientShowMsg("Auto: Kiem tra vat pham qua thoi gian, huy kiem tra");
				return;
			}
			  m_Autoplay.nShiQuNum++;
		 }
		 else
		 {
		      m_Autoplay.nShiQuNum=0;
		 }

/*		  char msg[64];
          sprintf(msg,"测试:禁止捡:%d,dwidx:%d",Object[nObjIndex].m_IsHaveAttack,Object[nObjIndex].m_AttackerDwid);
          m_ItemList.ClientShowMsg(msg);
*/
		sPickUp.ProtocolType = c2s_playerpickupitem;
		sPickUp.m_nObjID = Object[nObjIndex].m_nID;
		sPickUp.m_btPosType = sItemPos.nPlace;
		sPickUp.m_btPosX = sItemPos.nX;  //包袱容器的位置
		sPickUp.m_btPosY = sItemPos.nY;  //包袱容器的位置
	}
	
	if (g_pClient)
		g_pClient->SendPackToServer(&sPickUp, sizeof(PLAYER_PICKUP_ITEM_COMMAND));
}
#endif


#ifndef _SERVER
//-------------------------------------------------------------------------
//	功能：客户端鼠标点击obj检起某个钱，向服务器发消息
//-------------------------------------------------------------------------
void	KPlayer::PickUpMoney(int nObjIndex,int rid)
{
	if (this->CheckTrading())
		return;
	if (nObjIndex <= 0)
		return;
	if (Object[nObjIndex].m_nKind != Obj_Kind_Money)
	{
      Npc[rid].m_nObjectIdx=0;
	  return;
	}
	//m_ItemList.ClientShowMsg("测试怪物捡钱！！");
	PLAYER_PICKUP_ITEM_COMMAND	sPickUp; //对象
	if (Object[nObjIndex].m_nKind == Obj_Kind_Money) //掉在地上的东西类型为==钱
	{
		sPickUp.ProtocolType = c2s_playerpickupitem; //捡东西协议标识
		sPickUp.m_nObjID = Object[nObjIndex].m_nID;
		sPickUp.m_btPosType = 0;
		sPickUp.m_btPosX = 0;
		sPickUp.m_btPosY = 0;

		m_Autoplay.nShiQuNum=0;
	}
	
	if (g_pClient)
		g_pClient->SendPackToServer(&sPickUp, sizeof(PLAYER_PICKUP_ITEM_COMMAND));
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	功能：客户端鼠标点击obj检起某个装备，向服务器发消息
//-------------------------------------------------------------------------
void	KPlayer::PickUpItem(int nObjIndex,int rid)
{
	if (this->CheckTrading())
		return;
	if (nObjIndex <= 0)
		return;
	if (Object[nObjIndex].m_nKind != Obj_Kind_Item)
	{
		Npc[rid].m_nObjectIdx=0;
		return;
	}
	PLAYER_PICKUP_ITEM_COMMAND	sPickUp; //对象
	if (Object[nObjIndex].m_nKind == Obj_Kind_Item) //掉在地上的东西类型为==装备
	{
		ItemPos	sItemPos;

		if (FALSE == m_ItemList.SearchPosition(Object[nObjIndex].m_nItemWidth, Object[nObjIndex].m_nItemHeight, &sItemPos) )
		{//检测包袱
			if (FALSE == m_ItemList.SearchItemPosition(Object[nObjIndex].m_nItemWidth, Object[nObjIndex].m_nItemHeight, &sItemPos,room_equipmentex))
			{
				if (Npc[m_nIndex].m_AutoplayId==1)
				{ //挂机状态
					Object[nObjIndex].m_AttackerDwid=Npc[m_nIndex].m_dwID;
					Object[nObjIndex].m_IsHaveAttack=1;
					Object[nObjIndex].m_AttackerTime=18*60*10;
					//开始发送命令告诉服务器包袱不足了
					ApplyRoomInfo(1);
				}

				m_ItemList.ClientShowMsg("提示:转存子母袋失败,空间不足！");
				return;
			}

			int nItemEX = Npc[m_nIndex].m_ExItemId; //字母袋是否开启

          	if (nItemEX==0)
			{
				if (Npc[m_nIndex].m_AutoplayId==1)
				{ //挂机状态
					Object[nObjIndex].m_AttackerDwid=Npc[m_nIndex].m_dwID;
					Object[nObjIndex].m_IsHaveAttack=1;
					Object[nObjIndex].m_AttackerTime=18*60*10;
					//开始发送命令告诉服务器包袱不足了
					ApplyRoomInfo(1);
				}

				m_ItemList.ClientShowMsg("提示:转存子母袋失败,子母袋还未开启！");
				return;
			}	
		}

         if (Npc[m_nIndex].m_AutoplayId==1)
		 {//挂机状态			
			if (m_Autoplay.nShiQuNum>m_Autoplay.nVal && m_Autoplay.nShiQuNum%m_Autoplay.nVal==0)
			{
                Object[nObjIndex].m_AttackerDwid=Npc[m_nIndex].m_dwID;
                Object[nObjIndex].m_IsHaveAttack=1;
			    Object[nObjIndex].m_AttackerTime=18*60*10;
				m_Autoplay.nShiQuNum=0;
				m_ItemList.ClientShowMsg("<c=yellow>助手<c>:捡取物品超时,取消捡取!");
				return;
			}
			  m_Autoplay.nShiQuNum++;
		 }
		 else
		 {
		      m_Autoplay.nShiQuNum=0;
		 }

		sPickUp.ProtocolType = c2s_playerpickupitem;
		sPickUp.m_nObjID = Object[nObjIndex].m_nID;
		sPickUp.m_btPosType = sItemPos.nPlace;  //目标容器
		sPickUp.m_btPosX = sItemPos.nX;         //目标位置
		sPickUp.m_btPosY = sItemPos.nY;
	}
	
	if (g_pClient)
		g_pClient->SendPackToServer(&sPickUp, sizeof(PLAYER_PICKUP_ITEM_COMMAND));
}
#endif


#ifndef _SERVER
//-------------------------------------------------------------------------
//	功能：客户端鼠标点击obj，向服务器发消息   执行脚本
//-------------------------------------------------------------------------
void	KPlayer::ObjMouseClick(int nObjIndex)
{
	if (this->CheckTrading())
		return;
	if (nObjIndex <= 0)
		return;
	if (Object[nObjIndex].m_nKind != Obj_Kind_Box && Object[nObjIndex].m_nKind != Obj_Kind_Prop)
		return;

	if (Object[nObjIndex].m_nRegionIdx<0)
		return;

	SendObjMouseClick(Object[nObjIndex].m_nID, SubWorld[0].m_Region[Object[nObjIndex].m_nRegionIdx].m_RegionID);
}
#endif

#ifndef _SERVER
// DownPos 是面板上的物品的当前坐标，UpPos 必须是手上物品放到面板上的坐标
void	KPlayer::MoveItem(ItemPos DownPos, ItemPos UpPos,int nIsComp)
{
	if (this->CheckTrading() && DownPos.nPlace == pos_traderoom)  //交易栏
	{
		if (this->m_cTrade.m_nTradeLock)
			return;
		
		// 任务物品的限制
		//int	nHandIdx = m_ItemList.Hand();

	/*	if (nHandIdx && Item[nHandIdx].GetGenre() == item_task ||(Item[nHandIdx].GetTime()->bYear>0 && Item[nHandIdx].GetIsWhere()==0) || Item[nHandIdx].GetIsBang())
		{
			KSystemMessage	sMsg;
			sprintf(sMsg.szMessage, MSG_TRADE_TASK_ITEM);//禁止任务交易
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
			return;
		}*/
	}
	
	if (!CheckTrading() && DownPos.nPlace == pos_immediacy)//单对象容器检测  目标容器检测
	{
		int nHandIdx = m_ItemList.Hand();
		if (nHandIdx)
		{
			if (m_ItemList.m_Room[room_immediacy].CheckSameDetailType(Item[nHandIdx].GetGenre(), Item[nHandIdx].GetDetailType(),Item[nHandIdx].GetParticular()))
			{
				KSystemMessage	sMsg;
				sMsg.eType = SMT_NORMAL;
				sMsg.byConfirmType = SMCT_NONE;
				sMsg.byPriority = 0;
				sMsg.byParamSize = 0;
				snprintf(sMsg.szMessage, sizeof(sMsg.szMessage), "%s", MSG_ITEM_SAME_DETAIL_IN_IMMEDIATE);
				sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				return;
			}
		}
	}
	else if (!CheckTrading() && DownPos.nPlace == pos_dazao)  //单容器打造栏已经有同样物品了
	{
		int nHandIdx = m_ItemList.Hand();
		if (nHandIdx)
		{//单容器材料检测开始
			//g_CompEquip.
			if (nIsComp==-1) //提炼面板
			{
				//m_ItemList.ClientShowMsg("升级玄晶矿石材料检测");
				//return;
			 }
			else if (nIsComp==-2) //提炼面板
			{
				//m_ItemList.ClientShowMsg("升级属性矿石材料检测");
				//return;
			 }
		    else if (nIsComp==1) //提炼面板
			 {
				 //m_ItemList.ClientShowMsg("提炼玄晶矿石材料检测");
				 //return;
			 }
			else if (nIsComp==2) //提取面板
			{
				 int nGenre=Item[nHandIdx].GetGenre();                //类别
                 int nPart=Item[nHandIdx].GetDetailType();            //详细类别
				 int nParticular= Item[nHandIdx].GetParticular();    //具体类别

				 /*if (DownPos.nX==1) //装备框大框
				 {
                     int nPartTyep[15],nReg=0;

					 ZeroMemory(nPartTyep,sizeof(nPartTyep));

					 g_CompEquip.GetInt15s(2,"Position_2",nPartTyep);

                     for (int i=0;i<14;++i)
					 {//检测可放置的装备的部位
						 if (nPartTyep[i]==nPart)
						 {
                              nReg=1;
						      break;
						 }
					 } 

					 if (!Item[nHandIdx].IsBlue())
					 {
				       m_ItemList.ClientShowMsg("提示:材料不对,请放蓝装!");
					   ZeroMemory(nPartTyep,sizeof(nPartTyep));
				        return;
					 }
					 if (nGenre!=0 || nReg==0)
					 {
						 m_ItemList.ClientShowMsg("提示:蓝装类型不对,请重新放置!");
						 ZeroMemory(nPartTyep,sizeof(nPartTyep));
						 return;
					 }

				 }
				 if (DownPos.nX==3) //小框1
				 {
                      
                     int nPartTyep[3];
					 ZeroMemory(nPartTyep,sizeof(nPartTyep));
                     g_CompEquip.GetInt3s(4,"Distill",nPartTyep);
					 if (nGenre!=nPartTyep[0] || nPart!=nPartTyep[1] || nParticular!=nPartTyep[2])
					 {
					     m_ItemList.ClientShowMsg("提示:材料不对,请放玄晶矿石!");
						 ZeroMemory(nPartTyep,sizeof(nPartTyep));
						  return;
					 }
					
				 }
				 if (DownPos.nX==4) //小框2
				 {
					int nPartTyep[2];
					ZeroMemory(nPartTyep,sizeof(nPartTyep));
					g_CompEquip.GetInt3s(5,"Distill",nPartTyep);
					if (nGenre!=2 || nPart!=1 || nParticular>nPartTyep[1] || nParticular<nPartTyep[0])
					{
						m_ItemList.ClientShowMsg("提示:材料不对,请放属性原矿!");
						ZeroMemory(nPartTyep,sizeof(nPartTyep));
						return;
					}
				 }*/
              //   return;
			 }
			 else if (nIsComp==3) //打造紫装
			 {
				 //m_ItemList.ClientShowMsg("打造材料检测");
				 //return;
			 }
			 else if (nIsComp==4) //镶套紫装
			 {
				 //m_ItemList.ClientShowMsg("镶套材料检测");
				 //return;
			 }
			 else if (nIsComp==5) //图谱
			 {
				 //m_ItemList.ClientShowMsg("图谱材料检测");
				 //return;
			 }
			 else if (nIsComp==12) //神将
			 {
				 //m_ItemList.ClientShowMsg("神将面板");

			   if (DownPos.nX>=53 && DownPos.nX<=77)
			   {
				 char nMsg[64];  
				 if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_TongBanNum)
				 {//已经召唤了宠物的	 
					 int nNpcIdx=0;
					     nNpcIdx =NpcSet.SearchID(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_TongBanNum);
					 if (nNpcIdx>0)
					 {
					 }
					 else
					 {
						 sprintf(nMsg,"<c=yellow>神将<c>:同伴没有召唤,不能装备!");
						 m_ItemList.ClientShowMsg(nMsg);
						 return;
					 }
				 }
				 else
				 {
					 sprintf(nMsg,"<c=yellow>神将<c>:同伴没有召唤,不能装备!");
					 m_ItemList.ClientShowMsg(nMsg);
					 return;
				 }
			   }
			 } 
		}
	}
	else if (!CheckTrading() && DownPos.nPlace == pos_cailiao)  //检测 材料栏
	{
		int nHandIdx = m_ItemList.Hand();
		if (nHandIdx)
		{//多对象容器材料检测开始,在服务端进行检查
			//g_CompEquip.
			/*int nGenre=Item[nHandIdx].GetGenre();                //类别
			int nPart=Item[nHandIdx].GetDetailType();            //详细类别
			int nParticular= Item[nHandIdx].GetParticular();     //具体类别
            */
             if (nIsComp==1) //提炼
			 {
				// m_ItemList.ClientShowMsg("提炼材料检测");
				// return;
			 }
			 else if (nIsComp==2) //提取
			 {
				// m_ItemList.ClientShowMsg("提取材料检测");
				// return;
			 }
			 else if (nIsComp==3) //打造紫装
			 {
				 //m_ItemList.ClientShowMsg("打造材料检测");
				 //return;
			 }
			 else if (nIsComp==4) //镶套紫装
			 {
				 //m_ItemList.ClientShowMsg("镶套材料检测");
				//return;
			 }
			 else if (nIsComp==5) //图谱
			 {
				 //m_ItemList.ClientShowMsg("图谱材料检测");
				 //return;
			 }
			 else if (nIsComp==8) //融合
			 {
				 //m_ItemList.ClientShowMsg("提示:面板关闭中...");
				 //return;
			 }
/*
			if (m_ItemList.m_Room[room_cailiao].CheckSameDetailType(Item[nHandIdx].GetGenre(), Item[nHandIdx].GetDetailType()))
			{
				KSystemMessage	sMsg;
				sMsg.eType = SMT_NORMAL;
				sMsg.byConfirmType = SMCT_NONE;
				sMsg.byPriority = 0;
				sMsg.byParamSize = 0;
				sprintf(sMsg.szMessage, "%s(%d)", "提示:<c=yellow>材料栏已经有同类型的物品了!<c>",nIsComp);
				sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				return;
			}
			*/
		}
	}
	
	SendClientCmdMoveItem(&DownPos,&UpPos,nIsComp); //DownPos 目标容器
}
#endif
#ifndef _SERVER
// DownPos 是面板上的物品的当前坐标，UpPos 必须是手上物品放到面板上的坐标
void	KPlayer::MoveSkill(ItemPos DownPos, ItemPos UpPos)
{
	if (!CheckTrading() && DownPos.nPlace == pos_immediacy)
	{
		int nHandIdx = m_ItemList.Hand();
		if (nHandIdx)
		{
			if (m_ItemList.m_Room[room_immediacy].CheckSameDetailType(Item[nHandIdx].GetGenre(), Item[nHandIdx].GetDetailType(),Item[nHandIdx].GetParticular()))
			{
				KSystemMessage	sMsg;
				sMsg.eType = SMT_NORMAL;
				sMsg.byConfirmType = SMCT_NONE;
				sMsg.byPriority = 0;
				sMsg.byParamSize = 0;
				sprintf(sMsg.szMessage, "提示：快捷栏已经有同样的技能啦！");
				sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				return;
			}
		}
	}
	
//	SendClientCmdMoveItem(&DownPos, &UpPos);
}
#endif
//丢弃物品 丢掉物品
#ifndef _SERVER
int	KPlayer::ThrowAwayItem()
{
	if (this->CheckTrading())
	{
	    Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("提示:正在交易中,不能丢弃物品!");
		return 0;
	}

	if (!m_ItemList.Hand())
	{

	    //Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("物品为空！！！！");
		return 0;
	}
/*	else
	{
	   char msg[64];
	   sprintf(msg,"物品索引:%d..",m_ItemList.Hand());
	   Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(msg);
	}*/
	int	nIdx =m_ItemList.Hand();

	if (/*Item[nIdx].GetTime()->bYear>0 || */Item[nIdx].GetIsBang())
	{
       int nReg=0;
	       g_GameSetTing.GetInteger("SYSTEM","ThrowAwayItem",1,&nReg);	//设置是否启用丢弃限制

     if (!nReg)
	 { 
		KSystemMessage	sMsg;
		sprintf(sMsg.szMessage,"警告:<c=green>(限时/绑定)装备不能丢弃!<c>");
		sMsg.eType = SMT_NORMAL;
		sMsg.byConfirmType = SMCT_NONE;
		sMsg.byPriority = 0;
		sMsg.byParamSize = 0;
		sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
	    return 0;
	 } 
	}

	PLAYER_THROW_AWAY_ITEM_COMMAND	sThrow;
	
  sThrow.ProtocolType = c2s_playerthrowawayitem;
  sThrow.m_Kind=0;

  if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_AutoplayId==1)
  {
    sThrow.m_ItemDwidx=Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_dwID;
	sThrow.m_IsAuToFenJie=m_Autoplay.nIsAuToFenJie;
  }
  else
  {
    sThrow.m_ItemDwidx=0;
	sThrow.m_IsAuToFenJie=0;
  }
	if (g_pClient)
		g_pClient->SendPackToServer(&sThrow, sizeof(PLAYER_THROW_AWAY_ITEM_COMMAND));
	
	return 1;
}


int	KPlayer::AutoAwayItem(DWORD nItmeDwIdx)
{

	PLAYER_THROW_AWAY_ITEM_COMMAND	sThrow;
	
	sThrow.ProtocolType = c2s_playerthrowawayitem;
	sThrow.m_Kind=1;
    sThrow.m_ItemDwidx=nItmeDwIdx;
	sThrow.m_IsAuToFenJie=m_Autoplay.nIsAuToFenJie;
	if (g_pClient)
		g_pClient->SendPackToServer(&sThrow, sizeof(PLAYER_THROW_AWAY_ITEM_COMMAND));
	
	return 1;
}

#endif

#ifndef _SERVER
void	KPlayer::ChatAddFriend(int nPlayerIdx)
{
	CHAT_ADD_FRIEND_COMMAND	sAdd;
	sAdd.ProtocolType = c2s_chataddfriend;
	sAdd.m_nTargetPlayerIdx = nPlayerIdx;
	if (g_pClient)
		g_pClient->SendPackToServer(&sAdd, sizeof(CHAT_ADD_FRIEND_COMMAND));
}
#endif

#ifndef _SERVER
void	KPlayer::ChatRefuseFriend(int nPlayerIdx)
{
	CHAT_REFUSE_FRIEND_COMMAND	sRefuse;
	sRefuse.ProtocolType = c2s_chatrefusefriend;
	sRefuse.m_nTargetPlayerIdx = nPlayerIdx;
	if (g_pClient)
		g_pClient->SendPackToServer(&sRefuse, sizeof(CHAT_REFUSE_FRIEND_COMMAND));
}
#endif

#ifndef _SERVER
void	KPlayer::TradeApplyOpen(char *lpszSentence, int nLength)
{
	if (this->CheckTrading())
		return;
	if (!lpszSentence)
		nLength = 0;
	TRADE_APPLY_OPEN_COMMAND	sOpen;
	sOpen.ProtocolType = c2s_tradeapplystateopen;
	if (nLength >= MAX_SENTENCE_LENGTH)
		nLength = MAX_SENTENCE_LENGTH - 1;
	sOpen.m_wLength = sizeof(TRADE_APPLY_OPEN_COMMAND) - 1 - sizeof(sOpen.m_szSentence) + nLength;
	memset(m_cMenuState.m_szSentence, 0, sizeof(m_cMenuState.m_szSentence));
	if (lpszSentence)
		memcpy(m_cMenuState.m_szSentence, lpszSentence, nLength);
	memset(sOpen.m_szSentence, 0, sizeof(sOpen.m_szSentence));
	memcpy(sOpen.m_szSentence, m_cMenuState.m_szSentence, nLength);
	if (g_pClient)
		g_pClient->SendPackToServer(&sOpen, sOpen.m_wLength + 1);
}
#endif

#ifndef _SERVER
void	KPlayer::TradeApplyClose()
{
	TRADE_APPLY_CLOSE_COMMAND	sApply;
	sApply.ProtocolType = c2s_tradeapplystateclose;
	if (g_pClient)
		g_pClient->SendPackToServer(&sApply, sizeof(TRADE_APPLY_CLOSE_COMMAND));
}
#endif

#ifndef _SERVER
void	KPlayer::TradeApplyStart(int nNpcIdx)
{
	if (nNpcIdx < 0 || nNpcIdx >= MAX_NPC)
		return;
	TRADE_APPLY_START_COMMAND	sStart;
	sStart.ProtocolType = c2s_tradeapplystart;
	sStart.m_dwID = Npc[nNpcIdx].m_dwID;
	if (g_pClient)
		g_pClient->SendPackToServer(&sStart, sizeof(TRADE_APPLY_START_COMMAND));
	
	KSystemMessage	sMsg;
	sprintf(sMsg.szMessage, MSG_TRADE_SEND_APPLY, Npc[nNpcIdx].Name);
	sMsg.eType = SMT_NORMAL;
	sMsg.byConfirmType = SMCT_NONE;
	sMsg.byPriority = 0;
	sMsg.byParamSize = 0;
	sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
	CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
}
#endif

#ifndef _SERVER
BOOL	KPlayer::TradeMoveMoney(int nMoney)
{
	if (!CheckTrading())
		return FALSE;
	if (this->m_cTrade.m_nTradeLock)
		return FALSE;
	// 钱数量错误
	if (nMoney < 0 || nMoney > m_ItemList.GetEquipmentMoney())
		return FALSE;
	
	TRADE_MOVE_MONEY_COMMAND	sMoney;
	
	sMoney.ProtocolType = c2s_trademovemoney;
	sMoney.m_nMoney = nMoney;
	if (g_pClient)
		g_pClient->SendPackToServer(&sMoney, sizeof(TRADE_MOVE_MONEY_COMMAND));
	
	return TRUE;
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	功能：交易确定或取消
//	if nDecision == 0 推出交易  if nDecision == 1 确定交易  if nDecision == 2 取消交易确定
//-------------------------------------------------------------------------
void	KPlayer::TradeDecision(int nDecision)
{
	if ( !CheckTrading() )
		return;
	TRADE_DECISION_COMMAND	sDecision;
	sDecision.ProtocolType = c2s_tradedecision;
	if (nDecision == 1)
	{
		sDecision.m_btDecision = 1;
	}
	else if (nDecision == 0)
	{
		sDecision.m_btDecision = 0;
	}
	else if (nDecision == 2)
	{
		sDecision.m_btDecision = 4;
	}
	if (g_pClient)
		g_pClient->SendPackToServer(&sDecision, sizeof(TRADE_DECISION_COMMAND));
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	功能：交易锁定或取消锁定
//-------------------------------------------------------------------------
void	KPlayer::TradeApplyLock(int nLockOrNot)
{
	if ( !CheckTrading() )
		return;
	TRADE_DECISION_COMMAND	sDecision;
	sDecision.ProtocolType = c2s_tradedecision;
	if (nLockOrNot)
	{
		sDecision.m_btDecision = 2;
	}
	else
	{
		sDecision.m_btDecision = 3;
	}
	if (g_pClient)
		g_pClient->SendPackToServer(&sDecision, sizeof(TRADE_DECISION_COMMAND));
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	功能：收到服务器通知有人申请加入队伍
//-------------------------------------------------------------------------
void	KPlayer::s2cApplyAddTeam(BYTE* pProtocol)
{
	// 状态检查 (还需要检查 npc 的队伍开放状态 not end)
	if ( !m_cTeam.m_nFlag || m_cTeam.m_nFigure != TEAM_CAPTAIN)
	{
		ApplySelfTeamInfo();
		return;
	}
	
	// 寻找 npc
	PLAYER_APPLY_ADD_TEAM	*pAddTeam = (PLAYER_APPLY_ADD_TEAM*)pProtocol;
	int nNpcNo = NpcSet.SearchID(pAddTeam->m_dwTarNpcID);
	if (nNpcNo == 0)
		return;
	
	int i, nFreeListNo;
	// 如果已经存在，更新
	for (i = 0; i < MAX_TEAM_APPLY_LIST; ++i)
	{
		if (m_cTeam.m_sApplyList[i].m_dwNpcID == pAddTeam->m_dwTarNpcID)
		{
			m_cTeam.m_sApplyList[i].m_nLevel = Npc[nNpcNo].m_Level;
			m_cTeam.m_sApplyList[i].m_dwTimer = MAX_APPLY_TEAM_TIME;
			snprintf(m_cTeam.m_sApplyList[i].m_szName, sizeof(m_cTeam.m_sApplyList[i].m_szName), "%s", Npc[nNpcNo].Name);
			m_cTeam.UpdateInterface();
			m_cTeam.UpdateamUI(); //显示主界面队伍信息
			return;
		}
	}
	// 寻找申请人list空位
	for (i = 0; i < MAX_TEAM_APPLY_LIST; ++i)
	{
		if (m_cTeam.m_sApplyList[i].m_dwNpcID == 0)
		{
			nFreeListNo = i;
			break;
		}
	}
	if (i >= MAX_TEAM_APPLY_LIST)	// 没找到申请人空位
		return;
	
	m_cTeam.m_sApplyList[nFreeListNo].m_dwNpcID = pAddTeam->m_dwTarNpcID;
	m_cTeam.m_sApplyList[nFreeListNo].m_nLevel = Npc[nNpcNo].m_Level;
	m_cTeam.m_sApplyList[nFreeListNo].m_dwTimer = MAX_APPLY_TEAM_TIME;
	snprintf(m_cTeam.m_sApplyList[nFreeListNo].m_szName, sizeof(m_cTeam.m_sApplyList[nFreeListNo].m_szName), "%s", Npc[nNpcNo].Name);
	
	m_cTeam.UpdateInterface();
	m_cTeam.UpdateamUI(); //显示主界面队伍信息
//-------------------------------------------------//自动应答
    //pPlayer = (KUiPlayerItem*)(&m_pHandlingMsg[1]);
			//g_pCoreShell->TeamOperation(TEAM_OI_APPLY_RESPONSE,(unsigned int)pPlayer, (nSelAction == 0));	//nSelAction=0：答应, nSelAction=1：拒绝
   if (Player[CLIENT_PLAYER_INDEX].m_Autoplay.nCteam.nIsHuiDa==1 && Player[CLIENT_PLAYER_INDEX].m_Autoplay.nCteam.nIsDuiZhang==1)
   {
      Player[CLIENT_PLAYER_INDEX].AcceptTeamMember(pAddTeam->m_dwTarNpcID);
	  return;
   }
//-------------------------------------------------

	KSystemMessage	sMsg;
	sprintf(sMsg.szMessage, MSG_TEAM_APPLY_ADD, Npc[nNpcNo].Name);
	sMsg.eType = SMT_TEAM;
	sMsg.byConfirmType = SMCT_UI_TEAM_APPLY;
	sMsg.byPriority = 3;   
	sMsg.byParamSize = sizeof(KUiPlayerItem);
    sMsg.nMsgLen=TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
	KUiPlayerItem	player;
	snprintf(player.Name, sizeof(player.Name), "%s", Npc[nNpcNo].Name);
	player.nIndex = 0;
	player.uId = pAddTeam->m_dwTarNpcID;
	
	CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, (int)&player);
	
	return;
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	功能：加入门派
//-------------------------------------------------------------------------
BOOL	KPlayer::AddFaction(char *lpszFactionName)
{
	if ( !m_cFaction.AddFaction(Npc[m_nIndex].m_Series, lpszFactionName))
		return FALSE;
	
	// 改变阵营
	Npc[m_nIndex].SetCamp(m_cFaction.GetGurFactionCamp());
	
	// 给客户端发消息
	SendFactionData();
	
	return TRUE;
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	功能：加入门派
//-------------------------------------------------------------------------
BOOL	KPlayer::AddFaction(int nFactionID)
{
	if ( !m_cFaction.AddFaction(Npc[m_nIndex].m_Series, nFactionID) )
		return FALSE;
	
	// 改变阵营
	Npc[m_nIndex].SetCamp(m_cFaction.GetGurFactionCamp());
	
	// 给客户端发消息
	SendFactionData();
	
	return TRUE;
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	功能：离开门派
//-------------------------------------------------------------------------
BOOL	KPlayer::LeaveCurFaction()
{
	m_cFaction.LeaveFaction();
	
	// 改变阵营
	Npc[m_nIndex].SetCamp(camp_free);
	
	// 给客户端发消息
	PLAYER_LEAVE_FACTION	sLeave;
	sLeave.ProtocolType = s2c_playerleavefaction;
	g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sLeave, sizeof(PLAYER_LEAVE_FACTION));
	
	return TRUE;
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	功能：开放当前门派某个等级的技能
//-------------------------------------------------------------------------
BOOL	KPlayer::CurFactionOpenSkill(int nLevel)
{
	if ( !m_cFaction.OpenCurSkillLevel(nLevel, &Npc[m_nIndex].m_SkillList) )
		return FALSE;
	
	PLAYER_FACTION_SKILL_LEVEL	sLevel;
	sLevel.ProtocolType = s2c_playerfactionskilllevel;
	sLevel.m_btCurFactionID = m_cFaction.m_nCurFaction;
	sLevel.m_btLevel = nLevel;
	g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sLevel, sizeof(PLAYER_FACTION_SKILL_LEVEL));
	
	return TRUE;
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	功能：清空任务临时变量
//-------------------------------------------------------------------------
void	KPlayer::TaskClearTempVal()
{
	this->m_cTask.ClearTempVar();
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	功能：得到任务完成情况
//-------------------------------------------------------------------------
int		KPlayer::TaskGetSaveVal(int nNo)
{
	return this->m_cTask.GetSaveVal(nNo);
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	功能：设定任务完成情况
//-------------------------------------------------------------------------
void	KPlayer::TaskSetSaveVal(int nNo, BOOL bFlag)
{
	this->m_cTask.SetSaveVal(nNo, bFlag);
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	功能：得到任务临时过程控制变量值
//-------------------------------------------------------------------------
int		KPlayer::TaskGetClearVal(int nNo)
{
	return this->m_cTask.GetClearVal(nNo);
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	功能：设定任务临时过程控制变量值
//-------------------------------------------------------------------------
void	KPlayer::TaskSetClearVal(int nNo, int nVal)
{
	this->m_cTask.SetClearVal(nNo, nVal);
}
#endif

//设置快捷技能 --默认
#ifndef _SERVER
void	KPlayer::SetDefaultImmedSkill()  
{
	//获取玩家左右键技能为默认物理技能
	int nDetailType = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetWeaponType();
	int nParticularType = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetWeaponParticular();
	
	//近身武器
	if (nDetailType == 0)
	{
		Player[CLIENT_PLAYER_INDEX].SetLeftSkill(g_nMeleeWeaponSkill[nParticularType]);
		Player[CLIENT_PLAYER_INDEX].SetRightSkill(g_nMeleeWeaponSkill[nParticularType]);
	}//远程武器
	else if (nDetailType == 1)
	{
		Player[CLIENT_PLAYER_INDEX].SetLeftSkill(g_nRangeWeaponSkill[nParticularType]);
		Player[CLIENT_PLAYER_INDEX].SetRightSkill(g_nRangeWeaponSkill[nParticularType]);
	}//空手
	else if (nDetailType == -1)
	{
		Player[CLIENT_PLAYER_INDEX].SetLeftSkill(g_nHandSkill);
		Player[CLIENT_PLAYER_INDEX].SetRightSkill(g_nHandSkill);
	}

}
//设置快捷栏拾取技能
void	KPlayer::SetShiquSkill(int nSkillID)
{

	if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetCurrentLevel(nSkillID) <= 0) 
		return;

	KUiGameObject Info;
	Info.uGenre = CGOG_SKILL_SHORTCUT;
	Info.uId = nSkillID;
	CoreDataChanged(GDCNI_PLAYER_IMMED_ITEMSKILL, (unsigned int)&Info, -1);

}

void KPlayer::SetLeftSkill(int nSkillID,BOOL nIsNoLogin)
{
	if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetCurrentLevel(nSkillID) < 0) 
		return;

    Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].ReSkillEnhance(m_nLeftSkillID,0); //先减去上一次左键技能加成

	//写内挂技能编号
	if (nIsNoLogin)
	{
		 Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("<c=gyellow>助手<c>:保存自动挂机技能成功!");
		 KIniFile nAutoConfig;
		
		 char nConfig[128]={0};
					   
		 sprintf(nConfig,"\\FsAi\\配置\\%s_配置.ini",Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name);
					   
		 if  (nAutoConfig.Load(nConfig))
		 { 
			 nAutoConfig.WriteInteger("AAAA","LeftSkill_0",nSkillID); 
		 }
		 else
		 {
			 Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("<c=gyellow>助手<c>:配置不存在!");

		 }
		 nAutoConfig.Save(nConfig);
		 nAutoConfig.Clear();
	}

	m_nLeftSkillID = nSkillID;	
	m_nLeftListidx = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.FindSame(m_nLeftSkillID);	

	SKILL_LEFT_SYNC	sLeftSkill;
	sLeftSkill.ProtocolType = c2s_skillsync;
	sLeftSkill.m_nLeftskill = nSkillID;
    sLeftSkill.m_Type=0;

	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].ReSkillEnhance(nSkillID,1);
	
	if (g_pClient)
		g_pClient->SendPackToServer(&sLeftSkill, sizeof(SKILL_LEFT_SYNC));
    
	//界面处理
	KUiGameObject Info;
	Info.uGenre = CGOG_SKILL_SHORTCUT;
	Info.uId = m_nLeftSkillID;
	CoreDataChanged(GDCNI_PLAYER_IMMED_ITEMSKILL, (unsigned int)&Info, -1);
}


void	KPlayer::SetRightSkill(int nSkillID)
{
	if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetCurrentLevel(nSkillID) < 0) 
		return;

	m_nRightSkillID = nSkillID;
	m_nRightListidx = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.FindSame(m_nRightSkillID);
	// 检查是否是光环技能
	ISkill * pOrdinSkill = g_SkillManager.GetSkill(nSkillID, 1);

	if (!pOrdinSkill) 
        return;
	
	if (pOrdinSkill->IsAura())
	{//如果是光环技能 就立即使用
		Npc[m_nIndex].SetAuraSkill(nSkillID);
	}
	else
	{//否则，设置为不是光环技能
		Npc[m_nIndex].SetAuraSkill(0);
	}
     
	KUiGameObject Info;
	Info.uGenre = CGOG_SKILL_SHORTCUT;
	Info.uId = m_nRightSkillID;
	CoreDataChanged(GDCNI_PLAYER_IMMED_ITEMSKILL, (unsigned int)&Info, -2);
}
//换装 更新空手武器限制技能数据
void KPlayer::UpdateWeaponSkill()
{
	if (m_nLeftSkillID > 0)
	{
		ISkill * pISkill = g_SkillManager.GetSkill(m_nLeftSkillID, 1);
		if (!pISkill) 
            return;
		
		if (m_nLeftSkillID==1 || m_nLeftSkillID==2 || m_nLeftSkillID==53)
		{//设置左键技能 id
			SetLeftSkill(Npc[m_nIndex].GetCurActiveWeaponSkill());
		}
		
	}
	if (m_nRightSkillID > 0)
	{//设置右键技能 id
		ISkill * pISkill = (KSkill *) g_SkillManager.GetSkill(m_nRightSkillID, 1);
		if (!pISkill) 
            return;
		
		//if (pISkill->IsPhysical()) //外功系设置
		if (m_nRightSkillID==1 || m_nRightSkillID==2 || m_nRightSkillID==53)
		{
			SetRightSkill(Npc[m_nIndex].GetCurActiveWeaponSkill());
		}
	}
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	功能：设定当前聊天频道
//-------------------------------------------------------------------------
void	KPlayer::SetChatCurChannel(int nChannelNo)
{
	m_cChat.SetCurChannel(nChannelNo);
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	功能：邀请加入队伍
//-------------------------------------------------------------------------
void	KPlayer::TeamInviteAdd(DWORD dwNpcID)
{
	m_cTeam.InviteAdd(dwNpcID);
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	功能：收到客户端发来的聊天语句
//-------------------------------------------------------------------------
void	KPlayer::ServerSendChat(BYTE* pProtocol)
{
	m_cChat.ServerSendChat(m_nPlayerIndex, pProtocol);
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	功能：收到客户端要求增加基本属性点(0=Strength 1=Dexterity 2=Vitality 3=Engergy)
//-------------------------------------------------------------------------
void	KPlayer::AddBaseAttribute(BYTE* pProtocol)
{
	PLAYER_ADD_BASE_ATTRIBUTE_COMMAND	*pAdd = (PLAYER_ADD_BASE_ATTRIBUTE_COMMAND*)pProtocol;
	
	//执行一个判断脚本 来是否往下执行
	int reg=ExecuteScript_Direct("\\script\\global\\global_setting.lua","main_attributeSet",true,"dd",pAdd->m_btAttribute,pAdd->m_nAddNo);
	if  (reg<=0)
	{
		char szMsg[128];
		sprintf(szMsg, "<c=cyan>Khong the tang them!<c>");
		int nLength=0; 
		nLength = TEncodeText(szMsg, strlen(szMsg));
		KPlayerChat::SendSystemInfo(1, m_nPlayerIndex, "Tiem Nang", (char *)szMsg, nLength);
		return;
	}
	
	switch (pAdd->m_btAttribute)
	{
	case ATTRIBUTE_STRENGTH:
		AddBaseStrength(pAdd->m_nAddNo);  //力量
		break;
	case ATTRIBUTE_DEXTERITY:
		AddBaseDexterity(pAdd->m_nAddNo);  //身法
		break;
	case ATTRIBUTE_VITALITY:
		AddBaseVitality(pAdd->m_nAddNo);   //外功
		break;
	case ATTRIBUTE_ENGERGY:
		AddBaseEngergy(pAdd->m_nAddNo);    //  内功
		break;
	}
}
//重置基本属性
void	KPlayer::ResetBaseAttribute(BYTE* pProtocol)
{
	PLAYER_ADD_BASE_ATTRIBUTE_COMMAND	*pAdd = (PLAYER_ADD_BASE_ATTRIBUTE_COMMAND*)pProtocol;
	switch (pAdd->m_btAttribute)
	{
	case ATTRIBUTE_STRENGTH:  //力量
		ResetBaseStrength(pAdd->m_nAddNo);
		break;
	case ATTRIBUTE_DEXTERITY: //外功
		ResetBaseDexterity(pAdd->m_nAddNo);
		break;
	case ATTRIBUTE_VITALITY:  //身法
		ResetBaseVitality(pAdd->m_nAddNo);
		break;
	case ATTRIBUTE_ENGERGY:   //内力
		ResetBaseEngergy(pAdd->m_nAddNo);
		break;
	}
}
#endif

#define MAX_ORDINSKILL_LEVEL_ALWAYS  20//不包括其它情况对技能等级的变动之外的，一般最大技能等级

#ifdef _SERVER
//-------------------------------------------------------------------------
//	功能：收到客户端要求增加某个技能的点数   升级技能
//-------------------------------------------------------------------------
void	KPlayer::AddSkillPoint(BYTE* pProtocol)
{
	PLAYER_ADD_SKILL_POINT_COMMAND	*pAdd = (PLAYER_ADD_SKILL_POINT_COMMAND*)pProtocol;
	
	int nSkillIndex = Npc[m_nIndex].m_SkillList.FindSame(pAdd->m_nSkillID);

	if (nSkillIndex == 0)
		return;

	int nSkillLevel = Npc[m_nIndex].m_SkillList.GetSkillIdxLevel(nSkillIndex);  //获取技能当前的等级
	
	if (m_nSkillPoint >= pAdd->m_nAddPoint && nSkillLevel + pAdd->m_nAddPoint <= MAX_SKILLLEVEL)
	{
		ISkill * pSkill = NULL;
		if (nSkillLevel == 0)
			pSkill =  g_SkillManager.GetSkill(pAdd->m_nSkillID, 1);  //获取技能的基本数据
		else
			pSkill =  g_SkillManager.GetSkill(pAdd->m_nSkillID, nSkillLevel);

		if (pSkill)
		{
			char szMsg[125]={0};
			//执行一个判断脚本 来是否往下执行
			int reg=ExecuteScript_Direct("\\script\\Global\\global_setting.lua","main_Skillset",true,"ddddsd",pAdd->m_nSkillID,nSkillLevel,nSkillIndex,pAdd->m_nAddPoint,pSkill->GetSkillName(),pSkill->GetSkillStyle());
			if  (reg<=0)
			{
				snprintf(szMsg, sizeof(szMsg), "<c=green>%s<c>khong phu hop nang cap dieu kien, khong the nang cap!", pSkill->GetSkillName());
				int nLength=0; 
				nLength = TEncodeText(szMsg, strlen(szMsg));
				KPlayerChat::SendSystemInfo(1, m_nPlayerIndex, strCoreInfo[MESSAGE_SYSTEM_ANNOUCE_HEAD].c_str(), (char *)szMsg, nLength);
				return;
			}
			//有对应的技能升级脚本,优先执行脚本
			if (pSkill->GetSkillLevelUpScriptId())
			{
				ExecuteScriptB(pSkill->GetSkillLevelUpScriptId(), "main", pAdd->m_nAddPoint,"空",FALSE);
				nSkillLevel = Npc[m_nIndex].m_SkillList.GetSkillIdxLevel(nSkillIndex);
			}
			else
			{
				switch(pSkill->GetSkillStyle())
				{
				  case SKILL_SS_Missles:			            //	子弹类		本技能用于发送子弹类
				  case SKILL_SS_Melee:
				  case SKILL_SS_InitiativeNpcState:	    //	主动类		本技能用于改变当前Npc的主动状态
				  case SKILL_SS_PassivityNpcState:		//	被动类		本技能用于改变Npc的被动状态
					{
						int nWantToBeLevel = nSkillLevel + pAdd->m_nAddPoint;
						
						//Limit2
						int nLevelSkillMax = g_SkillManager.GetSkillMaxLevel(pAdd->m_nSkillID);
						int nLevelSkillMaxAdd = m_cReBorn.GetReBornSkillMaxLevel();
						int AddPointSkill = Npc[m_nIndex].m_SkillList.GetAddPoint(nSkillIndex) ;
						//reborn skill
						int mReg=0;					
		                    mReg=Npc[m_nIndex].m_SkillList.CheckNoSkill("NoCanXiSui",pAdd->m_nSkillID);
						if (mReg)
						{
							   sprintf(szMsg, "<c=green>%s<c> khong du dieu kien de tang!", pSkill->GetSkillName());
							   int nLength=0; 
							   nLength = TEncodeText(szMsg, strlen(szMsg));
							   KPlayerChat::SendSystemInfo(1, m_nPlayerIndex, strCoreInfo[MESSAGE_SYSTEM_ANNOUCE_HEAD].c_str(), (char *)szMsg, nLength);
							   return;
						 }

						if (nWantToBeLevel >= MAX_SKILLLEVEL) 
							return;

						if (nWantToBeLevel - Npc[m_nIndex].m_SkillList.GetAddPoint(nSkillIndex) > g_SkillManager.GetSkillMaxLevel(pAdd->m_nSkillID)+/*m_cReBorn.GetReBornNum()**/m_cReBorn.GetReBornSkillMaxLevel()+/*m_cVip.GetRoleVipNum()**/m_cVip.GetRoleVipSkillMaxLevel())
						{
							sprintf(szMsg, strCoreInfo[G_STR_ADD_SKILL_POINT4].c_str(), pSkill->GetSkillName());
							int nLength=0; 
							nLength = TEncodeText(szMsg, strlen(szMsg));
							KPlayerChat::SendSystemInfo(1, m_nPlayerIndex, strCoreInfo[MESSAGE_SYSTEM_ANNOUCE_HEAD].c_str(), (char *)szMsg, nLength);
							return;  
						}

						//printf("nWantToBeLevel: %d nLevelSkillMax: %d nLevelSkillMaxAdd: %d AddPointSkill: %d Level hien tai: %d\n",nWantToBeLevel,nLevelSkillMax,nLevelSkillMaxAdd,AddPointSkill,nSkillLevel);
						//减去额外的技能点
						//if ((nWantToBeLevel - Npc[m_nIndex].m_SkillList.GetAddPoint(nSkillIndex)) <= g_SkillManager.GetSkillMaxLevel(pAdd->m_nSkillID)+/*m_cReBorn.GetReBornNum()**/m_cReBorn.GetReBornSkillMaxLevel()+/*m_cVip.GetRoleVipNum()**/m_cVip.GetRoleVipSkillMaxLevel() &&  Npc[m_nIndex].m_Level >= ((KSkill*)pSkill)->GetSkillReqLevel())
						if (nLevelSkillMax > 1 && nWantToBeLevel <= (nLevelSkillMax + nLevelSkillMaxAdd + AddPointSkill ) && nWantToBeLevel <= Npc[m_nIndex].m_Level - ((KSkill*)pSkill)->GetSkillReqLevel() + 1)
						{
							if (!Npc[m_nIndex].m_SkillList.IncreaseLevel(nSkillIndex, pAdd->m_nAddPoint,FALSE,TRUE))
							   return;
			
							m_nSkillPoint -= pAdd->m_nAddPoint; //剩余技能点减少
							nSkillLevel   += pAdd->m_nAddPoint; //技能等级增加
						}
						else 
						{
							if (nWantToBeLevel >= nLevelSkillMax)
							{
								int nLength=0; 
								if (pSkill->GetIsExpSkill())
								{
									sprintf(szMsg, strCoreInfo[G_STR_ADD_SKILL_POINT1].c_str(), pSkill->GetSkillName());
									nLength = TEncodeText(szMsg, strlen(szMsg));
									KPlayerChat::SendSystemInfo(1, m_nPlayerIndex, strCoreInfo[MESSAGE_SYSTEM_ANNOUCE_HEAD].c_str(),(char *)szMsg,nLength);
									return;
								}
								else
								{
									sprintf(szMsg, strCoreInfo[G_STR_ADD_SKILL_POINT4].c_str(), pSkill->GetSkillName());
									nLength = TEncodeText(szMsg, strlen(szMsg));
									KPlayerChat::SendSystemInfo(1, m_nPlayerIndex, strCoreInfo[MESSAGE_SYSTEM_ANNOUCE_HEAD].c_str(),(char *)szMsg,nLength);
									return;
								}
							}
							else 
							{	//check dieu kien 2 k can coi do 
								if (nWantToBeLevel <= g_SkillManager.GetSkillMaxLevel(pAdd->m_nSkillID) && nWantToBeLevel <= Npc[m_nIndex].m_Level - ((KSkill*)pSkill)->GetSkillReqLevel() + 1 + AddPointSkill)
								{
									if (!Npc[m_nIndex].m_SkillList.IncreaseLevel(nSkillIndex, pAdd->m_nAddPoint,FALSE,TRUE))
									   return;
			
									m_nSkillPoint -= pAdd->m_nAddPoint; //剩余技能点减少
									nSkillLevel   += pAdd->m_nAddPoint; //技能等级增加
								}
								else
								{ 
									sprintf(szMsg, strCoreInfo[G_STR_ADD_SKILL_POINT5].c_str(), (((KSkill *)pSkill)->GetSkillReqLevel() -1) +  pAdd->m_nAddPoint + nSkillLevel - AddPointSkill , ((KSkill*)pSkill)->GetSkillName());
									int nLength=0; 
									nLength = TEncodeText(szMsg, strlen(szMsg));
									KPlayerChat::SendSystemInfo(1, m_nPlayerIndex, strCoreInfo[MESSAGE_SYSTEM_ANNOUCE_HEAD].c_str(), (char *)szMsg,nLength);
									return;
								}
							}
						}	
							sprintf(szMsg, strCoreInfo[G_STR_ADD_SKILL_POINT2].c_str(), pSkill->GetSkillName(),nWantToBeLevel);
							int nLength=0; 
							nLength = TEncodeText(szMsg, strlen(szMsg));
							KPlayerChat::SendSystemInfo(1, m_nPlayerIndex, strCoreInfo[MESSAGE_SYSTEM_ANNOUCE_HEAD].c_str(), (char *)szMsg, nLength);	
					}
					break;
				case SKILL_SS_Thief:  //偷窃类技能
					{
						return;
					}break;
					
				}	
			}

			UpdataCurData();  //更新数据
			// 把当前此技能的点数和剩余技能点发给客户端
			PLAYER_SKILL_LEVEL_SYNC	sSkill;
			sSkill.ProtocolType  = s2c_playerskilllevel;
			sSkill.m_nSkillID    = pAdd->m_nSkillID;
			sSkill.m_nSkillLevel = Npc[m_nIndex].m_SkillList.GetCurrentLevelByIdx(nSkillIndex);
			sSkill.m_nSkillExp = Npc[m_nIndex].m_SkillList.GetCurSkillExp(nSkillIndex);//muc do luyen
			sSkill.m_nLeavePoint = m_nSkillPoint;
			sSkill.m_nAddPoint   = Npc[m_nIndex].m_SkillList.GetAddPoint(nSkillIndex);
			//printf("DEBUG, %d-%d-%d\n", sSkill.m_nSkillLevel, sSkill.m_nLeavePoint, sSkill.m_nAddPoint);
			g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sSkill, sizeof(PLAYER_SKILL_LEVEL_SYNC));
		}
	}
}

void	KPlayer::IncSkillExp(int nSkillId, int nAddExp)
{	
	int		nSkillIndex, nSkillLevel;
	
	nSkillIndex = Npc[m_nIndex].m_SkillList.FindSame(nSkillId);
	if (nSkillIndex <= 0)
		return;

	nSkillLevel = Npc[m_nIndex].m_SkillList.GetLevel(nSkillId);
	if (nSkillLevel >= MAX_SKILLLEVEL)
		return;

	ISkill * pSkill = NULL;
	if (nSkillLevel <= 0)
	{
		pSkill =  g_SkillManager.GetSkill(nSkillId, 1);
	}
	else
	{
		pSkill =  g_SkillManager.GetSkill(nSkillId, nSkillLevel);
	}
	
	if (pSkill)
	{
		if  (Npc[m_nIndex].m_SkillList.IncreaseExp(nSkillIndex, nAddExp))
			UpdataCurData();
		
		PLAYER_SKILL_LEVEL_SYNC	sSkill;
		sSkill.ProtocolType = s2c_playerskilllevel;
		sSkill.m_nSkillID = nSkillId;
		sSkill.m_nSkillLevel = Npc[m_nIndex].m_SkillList.GetLevel(nSkillId);
		sSkill.m_nSkillExp = Npc[m_nIndex].m_SkillList.GetCurSkillExp(nSkillId);
		sSkill.m_nLeavePoint = m_nSkillPoint;
		g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sSkill, sizeof(PLAYER_SKILL_LEVEL_SYNC));
	}
}



#endif
//地上物品 与 身上物品公用一个索引
#ifdef _SERVER
//-------------------------------------------------------------------------
//	功能：服务器端拣物品  执行脚本
//-------------------------------------------------------------------------
BOOL	KPlayer::ServerPickUpItem(BYTE* pProtocol)
{
	PLAYER_PICKUP_ITEM_COMMAND	*pPickUp = (PLAYER_PICKUP_ITEM_COMMAND*)pProtocol;
	
	int	nObjIndex, nNpcX, nNpcY, nObjX, nObjY;
	    nObjIndex = ObjSet.FindID(pPickUp->m_nObjID);
	if (nObjIndex == 0)
		return FALSE;
	
	if (Object[nObjIndex].m_nBelong > 0)
	{//别人的东西
		if (!m_cTeam.m_nFlag)
		{//不是组队的
			if (Object[nObjIndex].m_nBelong != m_nPlayerIndex)
			{//不是自己的东西
				SHOW_MSG_SYNC	sMsg;
				sMsg.ProtocolType = s2c_msgshow;

				if (Object[nObjIndex].m_nKind == Obj_Kind_Money)
					sMsg.m_wMsgID = enumMSG_ID_MONEY_CANNOT_PICKUP;	//不能拾取别人的银两
				else
					sMsg.m_wMsgID = enumMSG_ID_OBJ_CANNOT_PICKUP; //不能拾取别人的物品

				sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1;
				g_pServer->PackDataToClient(m_nNetConnectIdx,(BYTE *)&sMsg, sMsg.m_wLength + 1);

				return FALSE;
			}
		}
		else
		{//组队的
			if (Object[nObjIndex].m_nKind == Obj_Kind_Money)
			{
				if (!g_Team[m_cTeam.m_nID].CheckIn(Object[nObjIndex].m_nBelong))
				{
					SHOW_MSG_SYNC	sMsg;
					sMsg.ProtocolType = s2c_msgshow;
					sMsg.m_wMsgID = enumMSG_ID_MONEY_CANNOT_PICKUP;
					sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1;
					g_pServer->PackDataToClient(m_nNetConnectIdx,(BYTE *)&sMsg, sMsg.m_wLength + 1);
					return FALSE;
				}
			}
			else if (Object[nObjIndex].m_nKind == Obj_Kind_Item)
			{
				if (Object[nObjIndex].m_nItemDataID <= 0 || Object[nObjIndex].m_nItemDataID >= MAX_ITEM)
					return FALSE;

				if (!g_Team[m_cTeam.m_nID].CheckIn(Object[nObjIndex].m_nBelong))
				{//不能捡起别人的东西
					SHOW_MSG_SYNC	sMsg;
					sMsg.ProtocolType = s2c_msgshow;
					sMsg.m_wMsgID = enumMSG_ID_OBJ_CANNOT_PICKUP;
					sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1;
					g_pServer->PackDataToClient(m_nNetConnectIdx,(BYTE *)&sMsg, sMsg.m_wLength + 1);

                    Object[nObjIndex].m_IsHaveAttack=1;

					return FALSE;
				}
			}
			else
				return FALSE;
		}
	}
	else if (Object[nObjIndex].m_nBelong == -2)
	{//系统设置不允许捡的东西
		    m_ItemList.msgshow("Vat pham nay bi he thong cam!");
			printf("Vat pham nay bi he thong cam!");
			return FALSE;
	}

	// 判断距离
	if (Object[nObjIndex].m_nSubWorldID != Npc[m_nIndex].m_SubWorldIndex)
		return FALSE;
	SubWorld[Object[nObjIndex].m_nSubWorldID].NewMap2Mps(
		Object[nObjIndex].m_nRegionIdx,
		Object[nObjIndex].m_nMapX,
		Object[nObjIndex].m_nMapY,
		Object[nObjIndex].m_nOffX,
		Object[nObjIndex].m_nOffY,
		&nObjX,
		&nObjY);
	SubWorld[Npc[m_nIndex].m_SubWorldIndex].NewMap2Mps(
		Npc[m_nIndex].m_RegionIndex,
		Npc[m_nIndex].m_MapX,
		Npc[m_nIndex].m_MapY,
		Npc[m_nIndex].m_OffX,
		Npc[m_nIndex].m_OffY,
		&nNpcX,
		&nNpcY);
	//nhat vat pham qua xa
/*	if (PLAYER_PICKUP_SERVER_DISTANCE < (nNpcX - nObjX) * (nNpcX - nObjX) + (nNpcY - nObjY) * (nNpcY - nObjY))
	{ //捡东西距离检测
		SHOW_MSG_SYNC	sMsg;
		sMsg.ProtocolType = s2c_msgshow;
		sMsg.m_wMsgID = enumMSG_ID_OBJ_TOO_FAR;
		sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1;
		g_pServer->PackDataToClient(m_nNetConnectIdx,(BYTE *)&sMsg, sMsg.m_wLength + 1);
		return FALSE;
	}
*/
	int mReg=0;
	char nMapName[64]={0};
		//mReg=NpcSet.CheckForBit(nItemInfo,"GuiShuItem",3); //是否可以改名的东西
	snprintf(nMapName, sizeof(nMapName), "%s", SubWorld[Npc[m_nIndex].m_SubWorldIndex].nWorldMapInfo[STR_MAP_NAME].c_str());
	if (Item[Object[nObjIndex].m_nItemDataID].GetGenre()!=0 && Item[Object[nObjIndex].m_nItemDataID].GetIsUse()/* && (strcmpi(Item[Object[nObjIndex].m_nItemDataID].GetWonName(),"系统") == 0)*/)  //如果是立即使用
	{//不是装备 是立即使用物品 物品的脚本
		int nRegn=1;
		nRegn = Object[nObjIndex].ExecScriptFiled(m_nPlayerIndex,\
		      Item[Object[nObjIndex].m_nItemDataID].GetScript(),\
		                                           "PickUpItem",\
		                        Object[nObjIndex].m_nItemDataID,\
		                                         m_nPlayerIndex,\
	                                                   nMapName,\
		     Item[Object[nObjIndex].m_nItemDataID].GetWonName(),\
			                                                0);
		if (!nRegn)
		{//删除了物品  
			m_ItemList.Remove(Object[nObjIndex].m_nItemDataID,0,0,0,FALSE);
			ItemSet.Remove(Object[nObjIndex].m_nItemDataID);
		    Object[nObjIndex].m_nItemDataID = 0;
			Object[nObjIndex].m_nItemWidth  = 0;
			Object[nObjIndex].m_nItemHeight = 0;

			Object[nObjIndex].m_IsHaveAttack=0;
			Object[nObjIndex].m_AttackerDwid=0;
            Object[nObjIndex].m_AttackerTime=0;

			Object[nObjIndex].Remove(FALSE);
			//ObjSet.Remove(nObjIndex);
			//ObjSet.resetLeftTime(nObjIndex,0);
			return TRUE;
		}   
	}
	else if (Object[nObjIndex].m_nKind!=Obj_Kind_Money)
	{//不是钱的东西 执行全局捡东西脚本
		//char m_gGoldScritp[200];
		//sprintf(m_gGoldScritp,"\\script\\Global\\goldpickup.lua");
	    Object[nObjIndex].ExecScriptFiled(m_nPlayerIndex,\
	                  "\\script\\global\\goldpickup.lua",\
			                                "PickUpItem",\
			             Object[nObjIndex].m_nItemDataID,\
			                              m_nPlayerIndex,\
                                                nMapName,\
	  Item[Object[nObjIndex].m_nItemDataID].GetWonName(),\
	                                                 0);
	}

	switch (Object[nObjIndex].m_nKind)
	{
	case Obj_Kind_Item:				// 掉在地上的装备
		{//捡东西 捡装备
			//---------------------------------------------------//检测包袱空间
			int xPos=0,yPos=0,n_TempRoom=room_equipment;
			switch(pPickUp->m_btPosType)
			{
			case pos_equiproom:
				n_TempRoom = room_equipment;
				break;
			case pos_equiproomex:
				n_TempRoom = room_equipmentex;
				break;
			default:
				break;
			}

			if (!m_ItemList.CheckCanPlaceInEquipment(Item[Object[nObjIndex].m_nItemDataID].GetWidth(),Item[Object[nObjIndex].m_nItemDataID].GetHeight(),&xPos,&yPos,n_TempRoom))
			{//另外查询服务器空间
				m_ItemList.msgshow("khoang trong khong du!");
				return FALSE;
			}

			if (Object[nObjIndex].m_nItemDataID <= 0 || Object[nObjIndex].m_nItemDataID >= MAX_ITEM)
			{
				m_ItemList.msgshow("Item loi!");
				return FALSE;
			}
			//xep chong
			m_nRoomFull=0;	        //设置包袱为空闲
			int StackNum =Item[Object[nObjIndex].m_nItemDataID].GetStackNum();

            if (StackNum<=0)
                StackNum=1;
            //---------------------------------------------------开始检测包袱是否有这个物品 是否能叠加
             /*if  (Item[Object[nObjIndex].m_nItemDataID].GetGenre()!=0 &&
				 !Item[Object[nObjIndex].m_nItemDataID].GetIsBang() &&
				 Item[Object[nObjIndex].m_nItemDataID].GetTime()->bYear<=0) 
			 {//不是装备的话,包袱里是否有这个物品  是否可以自动叠加
				 int nIdx=m_ItemList.IsHaveItem(Item[Object[nObjIndex].m_nItemDataID].GetGenre(),\
					                                      Item[Object[nObjIndex].m_nItemDataID].GetDetailType(),\
														  Item[Object[nObjIndex].m_nItemDataID].GetParticular(),\
														  Item[Object[nObjIndex].m_nItemDataID].GetSeries(),\
													      Item[Object[nObjIndex].m_nItemDataID].GetLevel(),pPickUp->m_btPosType);
               
				if (nIdx>0 && Item[nIdx].IsStack() &&
					Item[Object[nObjIndex].m_nItemDataID].CanStack(nIdx) &&
					(Item[nIdx].GetStackNum()+StackNum) <= MAX_STACKNUM
					) //是否可以叠加--手里和包袱里的是否相同

				{//包袱里的东西可以叠加  
                    //StackNum=Item[nIdx].GetStackNum()+StackNum;
					printf("nhat item 1%d , HaveItem: %d\n",Item[nIdx].GetGenre(),nIdx);
					//m_ItemList.Remove(nIdx,0,0,0,FALSE);
					//ItemSet.Remove(nIdx);
				}
			}*/
			//---------------------------------------------------//服务器端产生东西
			int nItemIdx = m_ItemList.Add(Object[nObjIndex].m_nItemDataID,pPickUp->m_btPosType,xPos,yPos,0,0,StackNum);//pPickUp->m_btPosX 
			
			if (nItemIdx <= 0)  //大于所有容器的物品
				return FALSE;

			// 去掉Object[nObjIndex]与道具的关联。避免ItemSet的Remove被Object的Remove调用
			Object[nObjIndex].m_nItemDataID = 0;
			Object[nObjIndex].m_nItemWidth  = 0;
			Object[nObjIndex].m_nItemHeight = 0;

            Object[nObjIndex].m_IsHaveAttack=0;
			Object[nObjIndex].m_AttackerDwid=0;
            Object[nObjIndex].m_AttackerTime=0;

			//ObjSet.Remove(nObjIndex);
			Object[nObjIndex].Remove(FALSE);   //删除地上的东西	
			//ObjSet.resetLeftTime(nObjIndex,0);
		}
		break;
	case Obj_Kind_Money:			// 掉在地上的钱
		if (!Earn(Object[nObjIndex].m_nMoneyNum) )  //增加钱
			return FALSE;
		Object[nObjIndex].SyncRemove(TRUE);

		if (Object[nObjIndex].m_nRegionIdx >= 0)
			SubWorld[Object[nObjIndex].m_nSubWorldID].m_Region[Object[nObjIndex].m_nRegionIdx].RemoveObj(nObjIndex);
		ObjSet.Remove(nObjIndex);
		//ObjSet.resetLeftTime(nObjIndex,0);
		break;
	}
	  
		return TRUE;
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	功能：收到客户端要求使用某个物品(鼠标右键点击)
//-------------------------------------------------------------------------
void	KPlayer::EatItem(BYTE* pProtocol)
{
	PLAYER_EAT_ITEM_COMMAND	*pEat = (PLAYER_EAT_ITEM_COMMAND*)pProtocol;
	
	m_ItemList.EatMecidineC(pEat->m_btPlace, pEat->m_btX, pEat->m_btY);
}

void	KPlayer::AutoItem(BYTE* pProtocol)
{
	PLAYER_AUTOMUVE_ITEM_COMMAND	*pMove = (PLAYER_AUTOMUVE_ITEM_COMMAND*)pProtocol;
	char nItemInfo[128];
	int nItemIdx=m_ItemList.SearchID(pMove->m_nItemID);
	//这里处理一下是否有相同的物品
	if (nItemIdx<=0 || nItemIdx>=MAX_ITEM)
	{
#ifdef _SERVER
		//客户端存在这个异常的物品 需要删除掉.
		ITEM_REMOVE_SYNC	sRemove;
		sRemove.ProtocolType = s2c_removeitem;
		sRemove.m_ID         = pMove->m_nItemID;
		sRemove.nPlace       =-1;
		sRemove.m_dx         = 0;
		sRemove.m_dy         = 0;
		sRemove.m_model   = 0;
		g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sRemove, sizeof(ITEM_REMOVE_SYNC));
		snprintf(nItemInfo, sizeof(nItemInfo),"----AutoItem:Acc:%s Name:%s co dau hieu bat thuong check Player::AutoItem!-----",m_AccoutName,m_PlayerName);
		g_LogFile.gTraceLogFile(nItemInfo,strlen(nItemInfo));
#endif
		return;
	}
	
	int nListIdx = m_ItemList.FindSame(nItemIdx);
	if (nListIdx <=0)
	{
		m_ItemList.msgshow("Item A khong ton tai, vui long bao gm!");
		snprintf(nItemInfo, sizeof(nItemInfo),"----Item A Acc:%s Name:%s co dau hieu bat thuong check Player::AutoItem!-----",m_AccoutName,m_PlayerName);
#ifdef _SERVER
		//客户端存在这个异常的物品 需要删除掉.
		ITEM_REMOVE_SYNC	sRemove;
		sRemove.ProtocolType = s2c_removeitem;
		sRemove.m_ID         = pMove->m_nItemID;
		sRemove.nPlace       =-1;
		sRemove.m_dx         = 0;
		sRemove.m_dy         = 0;
		sRemove.m_model   = 0;
		g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sRemove, sizeof(ITEM_REMOVE_SYNC));

		snprintf(nItemInfo, sizeof(nItemInfo),"----AutoItem:账号:%s角色:%s,涉嫌复制物品:%s[+%d],删除异常物品成功A!-----",m_AccoutName,m_PlayerName,Item[nItemIdx].GetName(),Item[nItemIdx].GetEnChance());
		g_LogFile.gTraceLogFile(nItemInfo,strlen(nItemInfo));

		if (m_ItemList.Remove(nItemIdx,0,0,0,FALSE))
			ItemSet.Remove(nItemIdx);

#endif
		return;
	}


	if (m_ItemList.m_Items[nListIdx].nPlace!= pMove->m_btPlace ||
		m_ItemList.m_Items[nListIdx].nX!= pMove->m_btX ||
		m_ItemList.m_Items[nListIdx].nY!= pMove->m_btY)
	{

	printf("Loi nPlace: %d nX: %d nY: %d - nPlace: %d nX: %d nY: %d\n",
		   m_ItemList.m_Items[nListIdx].nPlace,
		   m_ItemList.m_Items[nListIdx].nX, 
		   m_ItemList.m_Items[nListIdx].nY,
		   pMove->m_btPlace,
		   pMove->m_btX,
		   pMove->m_btY); 

		m_ItemList.msgshow("Thao tac qua nhanh!"); 
/*
#ifdef _SERVER
		ITEM_REMOVE_SYNC	sRemove;
		sRemove.ProtocolType = s2c_removeitem;
		sRemove.m_ID         =pMove->m_nItemID;
		sRemove.nPlace       =pMove->m_btPlace;
		sRemove.m_dx         =pMove->m_btX;
		sRemove.m_dy         =pMove->m_btY;
		sRemove.m_model   = 0;
		g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sRemove, sizeof(ITEM_REMOVE_SYNC));

		sprintf(nItemInfo,"----AutoItem:账号:%s角色:%s,涉嫌复制物品:%s[+%d],删除异常物品成功B!-----",m_AccoutName,m_PlayerName,Item[nItemIdx].GetName(),Item[nItemIdx].GetEnChance());
		g_LogFile.gTraceLogFile(nItemInfo,strlen(nItemInfo));

		if (m_ItemList.Remove(nItemIdx,0,0,0,FALSE))
			ItemSet.Remove(nItemIdx);
#endif
*/
		return;
	}

	m_ItemList.serAutoMoveItem(nItemIdx, pMove->m_btPlace, pMove->m_btX, pMove->m_btY,pMove->m_destRoom);
}

#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	功能：收到客户端要求使用某个物品(鼠标右键点击)
//-------------------------------------------------------------------------
//void	KPlayer::UseItem(BYTE* pProtocol)
//{
//	PLAYER_USE_ITEM_COMMAND	*pItem = (PLAYER_USE_ITEM_COMMAND*)pProtocol;
//}
#endif

#ifdef _SERVER
int	KPlayer::ServerMoveItem(BYTE* pProtocol)
{
	PLAYER_MOVE_ITEM_COMMAND	*pMove = (PLAYER_MOVE_ITEM_COMMAND*)pProtocol;
	ItemPos	DownPos,UpPos;
	int IsMianBan;

	DownPos.nPlace = pMove->m_btDownPos;
	DownPos.nX     = pMove->m_btDownX;
	DownPos.nY     = pMove->m_btDownY;
    DownPos.nDIsRongqi=pMove->m_DownRongqi;
	UpPos.nPlace   = pMove->m_btUpPos;
	UpPos.nX       = pMove->m_btUpX;
	UpPos.nY       = pMove->m_btUpY;
	UpPos.nSIsRongqi= pMove->m_UpRongqi;
	IsMianBan      = pMove->m_IsComp;
	
	if (this->CheckTrading() && DownPos.nPlace == pos_traderoom)
	{
		if (this->m_cTrade.m_nTradeLock)
			return -3;
	}
	//服务器端开始交换物品
	//printf("-------物品交还中--------- \n");
	m_ItemList.ExchangeItem(&DownPos, &UpPos,IsMianBan);
	
	//完成物品交换就解锁
	BYTE byFinished = s2c_itemexchangefinish;
	if (g_pServer)
		g_pServer->PackDataToClient(m_nNetConnectIdx,&byFinished, sizeof(BYTE));

	return TRUE;
}
#endif
//服务器检测人物自己丢弃物品时的归属
#ifdef _SERVER

BOOL KPlayer::CheckGmName()
{
  int nGmRows=g_GMSetting.GetHeight()+1;
  int nIsGM=FALSE;
   for (int i=1;i<nGmRows;++i)
   { 
	  char nGmName[ACC_LENGTH];
      g_GMSetting.GetString(i,1,"无",nGmName,sizeof(nGmName));
      if (stricmp(m_AccoutName,nGmName) == 0) //Player[nIndex].m_AccoutName
	  { 
		nIsGM=TRUE;
	    break;
	  } 
   }

  return nIsGM;

}

BOOL KPlayer::CheckCanTradeName()
{
	KTabFile nSname;
	 nSname.Load("\\settings\\tradename.txt");
	int nGmRows=nSname.GetHeight()+1;
	int nIsGM=FALSE;
	for (int i=1;i<nGmRows;++i)
	{ 
		char nGmName[ACC_LENGTH];
		nSname.GetString(i,1,"无",nGmName,sizeof(nGmName));
		if (stricmp(m_AccoutName,nGmName) == 0) //Player[nIndex].m_AccoutName
		{ 
			nIsGM=TRUE;
			break;
		} 
	}
	 nSname.Clear()	;
	return nIsGM;
	
}



void	KPlayer::ServerThrowAwayItem(BYTE* pProtocol)
{
	PLAYER_THROW_AWAY_ITEM_COMMAND	*pThrow = (PLAYER_THROW_AWAY_ITEM_COMMAND*)pProtocol;
	if (NULL==pThrow) return;
	
	KMapPos		sMapPos;

 if (pThrow->m_Kind==0)
 {

	if (!this->m_ItemList.Hand())
	{
	    m_ItemList.msgshow("Xoa Item may chu khong thanh cong vui long bao GM!");
		printf("Loi  1	ServerThrowAwayItem ");
		return;
	}
	
	int nItemIdx = m_ItemList.m_Hand;

	if  (nItemIdx<=0 || nItemIdx>=MAX_ITEM) 
	{
		m_ItemList.msgshow("Loi item vui long bao GM!!!!!!!!1123");
		printf("Loi 2	ServerThrowAwayItem ");
		return;
	}
	if (Item[nItemIdx].GetLockItem()->IsLockItem() && !CheckGmName())
	{
       int nReg=0;
	   nReg =  _gameGlobalSet.PlayerInfo.isThrowAwayItem;
	      // g_GameSetTing.GetInteger("SYSTEM","ThrowAwayItem",1,&nReg);	//设置是否启用丢弃限制
       if (!nReg)
	   {    
	      char msg[80];
		  snprintf(msg, 80, "<c=green>Nham bao ve tai san cho ban Vat pham nay khong duoc tuy y vut bo!<c>");
		  m_ItemList.msgshow(msg);
	      return;
	   } 	   
	}

	GetAboutPos(&sMapPos);

	if (m_ItemList.Remove(nItemIdx,0,0,0,FALSE))
	{//删除手里的东西成功的话 ，就表示丢弃成功！
		int		nObj;
		KObjItemInfo	sInfo;
		sInfo.m_nItemID     = nItemIdx;
		sInfo.m_nItemWidth  = Item[nItemIdx].GetWidth();
		sInfo.m_nItemHeight = Item[nItemIdx].GetHeight();
		sInfo.m_nMoneyNum   = 0;
		snprintf(sInfo.m_szName, sizeof(sInfo.m_szName), "%s", Item[nItemIdx].m_CommonAttrib.szItemName);
		sInfo.m_nColorID    = 0;
		sInfo.m_nMovieFlag  = 1;
		sInfo.m_nSoundFlag  = 1;
		if (pThrow->m_ItemDwidx)
		    sInfo.m_sHaveAttack=1;  
		else
		    sInfo.m_sHaveAttack=0;                     //是否设置为攻击无效了
	    //sInfo.m_AttackerDwid=0;                      //上次攻击着的DWID
		nObj = ObjSet.Add(Item[nItemIdx].GetObjIdx(),sMapPos,sInfo);  //同步客户端地上东西

		if (nObj >= 0)
		{
			int mprgType=0;
			//ZeroMemory(mItemType,sizeof(mItemType));
            //ZeroMemory(mItemBelong,sizeof(mItemBelong));
			//mItemBelong[0] = Item[nItemIdx].GetGenre();
            //mItemBelong[1] = Item[nItemIdx].GetDetailType();  //具体
            //mItemBelong[2] = Item[nItemIdx].GetParticular();  //详细
			   mprgType=CheckThisItem(Item[nItemIdx].GetGenre(),"IsPickType");  //禁止捡起的类型
            //mprgBelong = NpcSet.CheckForBit(mItemBelong,"ItemBelong",3);  //是否有归属者

			if (mprgType==1)  //有大类在禁止列表
			{//系统禁止的大类
				 Object[nObj].SetEntireBelong(-2);
			}
            else
			{//如果不是这个大类 就检测个人的 
			   //if (mprgBelong==1)
			   //{//有具体的东西在禁止列表
               //Object[nObj].SetEntireBelong(this->m_nPlayerIndex);
			   //}
			   //else
			   //设置所有人都可以捡
				Object[nObj].SetItemBelong(-1);
			}
		}
	}
	else
	{
	// 通知客户端买进的东西放不下
	   m_ItemList.msgshow("Xoa muc khong thanh cong!");
	}
  }
  else if (pThrow->m_Kind==1)
  {//外挂丢东西 和　手游客户端丢东西
    GetAboutPos(&sMapPos); 
	char nItemInfo[128];

	int nItemIdx = m_ItemList.SearchID(pThrow->m_ItemDwidx);
	int inidx    = m_ItemList.SearchItemsIdx(pThrow->m_ItemDwidx);

	if  (nItemIdx<=0 || nItemIdx>=MAX_ITEM)
	{//物品不存在
	   m_ItemList.msgshow("Loi vui long bao GM 2133333!");
#ifdef _SERVER
	   ITEM_REMOVE_SYNC	sRemove;
	   sRemove.ProtocolType = s2c_removeitem;
	   sRemove.m_ID         = pThrow->m_ItemDwidx;
	   sRemove.nPlace       =-1;
	   sRemove.m_dx         = 0;
	   sRemove.m_dy         = 0;
	   sRemove.m_model   = 0;
	   g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sRemove, sizeof(ITEM_REMOVE_SYNC));
	   snprintf(nItemInfo, sizeof(nItemInfo),"----ServerThrowAwayItem:账号:%s角色:%s,涉嫌复制物品,删除异常物品成功!C-----",m_AccoutName,m_PlayerName);
	   g_LogFile.gTraceLogFile(nItemInfo,strlen(nItemInfo));
#endif
	   return;
	}

	int inPlace=m_ItemList.m_Items[inidx].nPlace;

	if  (inPlace<=0 || inPlace>=pos_count) 
	{//位置不对，有这个物品
		m_ItemList.msgshow("Loi vui long bao GM ma loi 3333131313");
		//同步客户端删除客户端物品
#ifdef _SERVER
		ITEM_REMOVE_SYNC	sRemove;
		sRemove.ProtocolType = s2c_removeitem;
		sRemove.m_ID         = pThrow->m_ItemDwidx;	
		sRemove.nPlace       =-1;
		sRemove.m_dx         = 0;
		sRemove.m_dy         = 0;
		sRemove.m_model   = 0;
		g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sRemove, sizeof(ITEM_REMOVE_SYNC));

		snprintf(nItemInfo, sizeof(nItemInfo),"----ServerThrowAwayItem:账号:%s角色:%s,涉嫌复制物品:%s[+%d],删除异常物品成功!A-----",m_AccoutName,m_PlayerName,Item[nItemIdx].GetName(),Item[nItemIdx].GetEnChance());
		g_LogFile.gTraceLogFile(nItemInfo,strlen(nItemInfo));

		if (m_ItemList.Remove(nItemIdx,0,0,0,FALSE))
			ItemSet.Remove(nItemIdx);
#endif
		return;
	}

	if (pThrow->m_nPlace!=inPlace || pThrow->m_X!=m_ItemList.m_Items[inidx].nX || pThrow->m_Y!=m_ItemList.m_Items[inidx].nY)
	{//有这个物品 但是位置信息不对
		m_ItemList.msgshow("Loi vui long bao GM ma loi 5555555");
		//同步客户端删除客户端物品
#ifdef _SERVER
		ITEM_REMOVE_SYNC	sRemove;
		sRemove.ProtocolType = s2c_removeitem;
		sRemove.m_ID         = pThrow->m_ItemDwidx;
		sRemove.nPlace       =pThrow->m_nPlace;
		sRemove.m_dx         =pThrow->m_X;
		sRemove.m_dy         =pThrow->m_Y;
		sRemove.m_model   =0;
		g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sRemove, sizeof(ITEM_REMOVE_SYNC));

		snprintf(nItemInfo, sizeof(nItemInfo),"----ServerThrowAwayItem:账号:%s角色:%s,涉嫌复制物品:%s[+%d],删除异常物品成功!B-----",m_AccoutName,m_PlayerName,Item[nItemIdx].GetName(),Item[nItemIdx].GetEnChance());
		g_LogFile.gTraceLogFile(nItemInfo,strlen(nItemInfo));

		if (m_ItemList.Remove(nItemIdx,0,0,0,FALSE))
			   ItemSet.Remove(nItemIdx);

#endif
		return;
	}
	    //m_ItemList.m_Items[inidx].nPlace;//所在容器的类型
        //nItemidx  = m_ItemList.SearchItemsIdx(nItemDwID);
	if (m_ItemList.Remove(nItemIdx,0,0,0,FALSE))
	{
       int nLevel=Item[nItemIdx].GetLevel();
       int nYlevel=1; 
	  if (pThrow->m_IsAuToFenJie==1 && Item[nItemIdx].GetGenre()==item_equip && (Item[nItemIdx].IsBlue()||Item[nItemIdx].IsPurple()||Item[nItemIdx].GetGoldId()>0) &&  nLevel>=_gameGlobalSet.gameItemInfo.nInFenJie[0])
	  {//自动分解的话
			
			int nIndex = ItemSet.AddOther(item_task,0,1,0,_gameGlobalSet.gameItemInfo.nInFenJie[2],0,0,0,0,GetRandomNumber(1,nLevel),0,0,0,0,0,0,0);
	
	        if (nIndex <= 0) 
		       return;
			//xep chong item
           int nStackNum =Item[nIndex].GetStackNum();
	       if (nStackNum<=0)
	          nStackNum=1;
//---------------------------------------------------开始检测包袱是否有这个物品 是否能叠加
            /* if (Item[nIndex].GetGenre()!=0 && !Item[nIndex].GetIsBang() && Item[nIndex].GetTime()->bYear<=0)
			 {//不是装备的话,包袱里是否有这个物品
				 int nIdx=m_ItemList.IsHaveItem(Item[nIndex].GetGenre(),\
					                                      Item[nIndex].GetDetailType(),\
														  Item[nIndex].GetParticular(),\
														  Item[nIndex].GetSeries(),\
													      Item[nIndex].GetLevel(),inPlace);
               
				if (nIdx>0 && Item[nIdx].IsStack() &&
					Item[nIndex].CanStack(nIdx) &&
					(Item[nIdx].GetStackNum()+nStackNum) <= MAX_STACKNUM) //是否可以叠加--手里和包袱里的是否相同
				{//包袱里的东西可以叠加		
					printf("throw item %d , HaveItem: %d\n",Item[nIdx].GetGenre(),nIdx);
                    //nStackNum=Item[nIdx].GetStackNum()+nStackNum;
					m_ItemList.Remove(nIdx,0,0,0,FALSE);
					ItemSet.Remove(nIdx);			
				}
			}*/
			 
	       int x, y,nRoomType=room_equipment;

		   if (inPlace==pos_equiproom)	//包袱
			   nRoomType=room_equipment;

		   if(inPlace==pos_equiproomex)	//字母袋
 			   nRoomType=room_equipmentex;
		  
	       if (m_ItemList.CheckCanPlaceInEquipment(Item[nIndex].GetWidth(),Item[nIndex].GetHeight(),&x,&y,nRoomType))
	       {
		       m_ItemList.Add(nIndex,inPlace,x,y,0,0,nStackNum,TRUE);
			   DirectAddExp(nLevel*_gameGlobalSet.gameItemInfo.nInFenJie[1]);
			   char msg[64]={0};
			   sprintf(msg,"Phan huy thanh cong");
			   m_ItemList.msgshow(msg);
	       }
		   else
		   { 
		       m_ItemList.Remove(nIndex,0,0,0,FALSE);
			   ItemSet.Remove(nIndex);
			   if (nRoomType==room_equipment)
                  m_ItemList.msgshow("Khong du cho chua hanh trang, qua trinh huy khong thanh cong");
			   if (nRoomType==room_equipmentex)
                  m_ItemList.msgshow("Tui phu khong du cho qua tinh huy khong thanh cong");
		   }

		   ItemSet.Remove(nItemIdx);
	     
	  }
	  else
	  {//如果没有开启分解　就丢地上　或　直接删除
		int nIsOpenClearItem =1;
		   // g_GameSetTing.GetInteger("Item","IsOpenClearItem",1,&nIsOpenClearItem);
		nIsOpenClearItem = _gameGlobalSet.SubWorldInfo.nIsOpenClearItem;
		if  (nIsOpenClearItem)
		{
			ItemSet.Remove(nItemIdx);
			return;
		}

        if (!Npc[m_nIndex].m_AutoplayId)
		{
			snprintf(nItemInfo, sizeof(nItemInfo),"----[正常]ServerThrowAwayItem:账号:%s角色:%s,手动丢弃物品:%s[+%d],-----",m_AccoutName,m_PlayerName,Item[nItemIdx].GetName(),Item[nItemIdx].GetEnChance());
			g_LogFile.gTraceLogFile(nItemInfo,strlen(nItemInfo));
		}
		

		int		nObj;
		KObjItemInfo	sInfo;
		sInfo.m_nItemID     = nItemIdx;
		sInfo.m_nItemWidth  = Item[nItemIdx].GetWidth();
		sInfo.m_nItemHeight = Item[nItemIdx].GetHeight();
		sInfo.m_nMoneyNum   = 0;
		snprintf(sInfo.m_szName, sizeof(sInfo.m_szName), "%s", Item[nItemIdx].m_CommonAttrib.szItemName);
		sInfo.m_nColorID    = 0;
		sInfo.m_nMovieFlag  = 1;
		sInfo.m_nSoundFlag  = 1;
		if (pThrow->m_isAttack)
			sInfo.m_sHaveAttack = 1;                      //是否设置为攻击无效了
		else
			sInfo.m_sHaveAttack = 0; 
	    //sInfo.m_AttackerDwid=Npc[m_nIndex].m_dwID;   //上次攻击着的DWID
		nObj = ObjSet.Add(Item[nItemIdx].GetObjIdx(),sMapPos,sInfo);  //同步客户端地上东西
        
		if (nObj >= 0)
		{
			int mprgType=0;
			   mprgType=CheckThisItem(Item[nItemIdx].GetGenre(),"IsPickType");  //禁止捡起的类型
			if (mprgType==1)  //有大类在禁止列表
			{//系统禁止的大类
				 Object[nObj].SetEntireBelong(-2);
			}
            else
			{//如果不是这个大类 就检测个人的 
			   //设置所有人都可以捡
				Object[nObj].SetItemBelong(-1); 
			}
		}
		else
		   ItemSet.Remove(nItemIdx);
	  }
	}
	else// 通知客户端买进的东西放不下
		m_ItemList.msgshow("Xoa Item khong thanh cong!");
  }
}
#endif

BOOL KPlayer::CheckThisItem(int nSetings,char *nKey)
{
     
	int nRow = g_ForbitMap.GetHeight()+1,nReg=FALSE;
	
	for (int i=2;i<nRow;++i)
	{
		int nSkilid =0;
		g_ForbitMap.GetInteger(i,nKey,0,&nSkilid);
		if (nSkilid==nSetings)
		{
			nReg =TRUE;
			break;
		}
	}
	return nReg;   
}



#ifdef _SERVER
//-------------------------------------------------------------------------
//	功能：收到客户端消息设定聊天订阅频道
//-------------------------------------------------------------------------
void	KPlayer::ChatSetTakeChannel(BYTE* pProtocol)
{
	CHAT_SET_CHANNEL_COMMAND	*pChannel = (CHAT_SET_CHANNEL_COMMAND*)pProtocol;
	this->m_cChat.SetTakeChannel(pChannel->m_dwTakeChannel);
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	功能：收到客户端请求转发聊天添加好友信息
//-------------------------------------------------------------------------
void	KPlayer::ChatTransmitApplyAddFriend(BYTE* pProtocol)
{
	CHAT_APPLY_ADD_FRIEND_COMMAND	*pAdd = (CHAT_APPLY_ADD_FRIEND_COMMAND*)pProtocol;
	int	nPlayerIdx = FindAroundPlayer(pAdd->m_dwTargetNpcID);
	if (nPlayerIdx < 0)
		return;
	
	m_cChat.m_nAddFriendList[m_cChat.m_nListPos++] = nPlayerIdx;
	m_cChat.m_nListPos %= CHAT_APPLY_ADD_FRIEND_LIST;
	
	CHAT_APPLY_ADD_FRIEND_SYNC	sAdd;
	sAdd.ProtocolType = s2c_chatapplyaddfriend;
	sAdd.m_nSrcPlayerIdx = m_nPlayerIndex;
	snprintf(sAdd.m_szSourceName, sizeof(sAdd.m_szSourceName), "%s", Npc[m_nIndex].Name);
	//memset(sAdd.m_szInfo, 0, sizeof(sAdd.m_szInfo));
	//memcpy(sAdd.m_szInfo, pAdd->m_szInfo, pAdd->m_wLength + 1 + sizeof(pAdd->m_szInfo) - sizeof(CHAT_APPLY_ADD_FRIEND_COMMAND));
	g_pServer->PackDataToClient(Player[nPlayerIdx].m_nNetConnectIdx, (BYTE*)&sAdd, sizeof(CHAT_APPLY_ADD_FRIEND_SYNC));
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	功能：收到客户端消息添加聊天好友
//-------------------------------------------------------------------------
BOOL	KPlayer::ChatAddFriend(BYTE* pProtocol)
{
	CHAT_ADD_FRIEND_COMMAND	*pAdd = (CHAT_ADD_FRIEND_COMMAND*)pProtocol;
	
	if (pAdd->m_nTargetPlayerIdx < 0 || pAdd->m_nTargetPlayerIdx >= MAX_PLAYER)
		goto AddFalse;
	if (Player[pAdd->m_nTargetPlayerIdx].m_nIndex <= 0 || Player[pAdd->m_nTargetPlayerIdx].m_nIndex >= MAX_NPC)
		goto AddFalse;

	int		i;
	for (i = 0; i < CHAT_APPLY_ADD_FRIEND_LIST; ++i)
	{
		if (m_nPlayerIndex == Player[pAdd->m_nTargetPlayerIdx].m_cChat.m_nAddFriendList[i])
			break;
	}
	if (i >= CHAT_APPLY_ADD_FRIEND_LIST)
		goto AddFalse;
	
	m_cChat.AddFriendData(m_nPlayerIndex, pAdd->m_nTargetPlayerIdx);
	Player[pAdd->m_nTargetPlayerIdx].m_cChat.AddFriendData(pAdd->m_nTargetPlayerIdx, m_nPlayerIndex);
	
	return TRUE;
	
AddFalse:
	CHAT_ADD_FRIEND_FAIL_SYNC	sFail;
	sFail.ProtocolType = s2c_chataddfriendfail;
	sFail.m_nTargetPlayerIdx = pAdd->m_nTargetPlayerIdx;
	g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sFail, sizeof(CHAT_ADD_FRIEND_FAIL_SYNC));
	
	return FALSE;
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	功能：收到客户端消息拒绝添加聊天好友
//-------------------------------------------------------------------------
void	KPlayer::ChatRefuseFriend(BYTE* pProtocol)
{
	CHAT_REFUSE_FRIEND_COMMAND	*pRefuse = (CHAT_REFUSE_FRIEND_COMMAND*)pProtocol;
	
	if (pRefuse->m_nTargetPlayerIdx < 0 || pRefuse->m_nTargetPlayerIdx >= MAX_PLAYER)
		return;
	if (Player[pRefuse->m_nTargetPlayerIdx].m_nIndex == 0)
		return;
	for (int i = 0; i < CHAT_APPLY_ADD_FRIEND_LIST; ++i)
	{
		if (m_nPlayerIndex == Player[pRefuse->m_nTargetPlayerIdx].m_cChat.m_nAddFriendList[i])
		{
			CHAT_REFUSE_FRIEND_SYNC	sRefuse;
			sRefuse.ProtocolType = s2c_chatrefusefriend;
			snprintf(sRefuse.m_szName, sizeof(sRefuse.m_szName), "%s", Npc[m_nIndex].Name);
			sRefuse.m_wLength = sizeof(CHAT_REFUSE_FRIEND_SYNC) - 1 - sizeof(sRefuse.m_szName) + strlen(sRefuse.m_szName);
			
			g_pServer->PackDataToClient(Player[pRefuse->m_nTargetPlayerIdx].m_nNetConnectIdx, (BYTE*)&sRefuse, sRefuse.m_wLength + 1);
			return;
		}
	}
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	功能：玩家登录时收到客户端请求重新发送所有聊天好友数据（带名字）
//-------------------------------------------------------------------------
void	KPlayer::ChatResendAllFriend(BYTE* pProtocol)
{
	if ( m_cChat.m_nSyncState != 1)
		return;
	m_cChat.m_nSyncState = 2;
	m_cChat.m_pStateSendPos = (CChatFriend*)m_cChat.m_cFriendTeam[0].m_cEveryOne.GetHead();
	if ( !m_cChat.m_pStateSendPos )
		m_cChat.m_nSyncState = 0;
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	功能：玩家登录时收到客户端请求发送某个聊天好友数据（带名字）
//-------------------------------------------------------------------------
void	KPlayer::ChatSendOneFriendData(BYTE* pProtocol)
{
	CHAT_APPLY_SEND_ONE_FRIEND_NAME_COMMAND	*pApply = (CHAT_APPLY_SEND_ONE_FRIEND_NAME_COMMAND*)pProtocol;
	
	m_cChat.ResendOneFriendData(pApply->m_dwID, m_nPlayerIndex);
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	功能：收到客户端申请删除某个聊天好友
//-------------------------------------------------------------------------
void	KPlayer::ChatDeleteFriend(BYTE* pProtocol)
{
	CHAT_DELETE_FRIEND_COMMAND	*pDelete = (CHAT_DELETE_FRIEND_COMMAND*)pProtocol;
	
	m_cChat.DeleteFriend(pDelete->m_dwID, m_dwID);
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	功能：收到客户端申请再次删除某个聊天好友
//-------------------------------------------------------------------------
void	KPlayer::ChatRedeleteFriend(BYTE* pProtocol)
{
	CHAT_REDELETE_FRIEND_COMMAND	*pDelete = (CHAT_REDELETE_FRIEND_COMMAND*)pProtocol;
	
	this->m_cChat.RedeleteFriend(pDelete->m_dwID, m_nPlayerIndex);
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	功能：收到客户端申请进入待交易状态
//-------------------------------------------------------------------------
void	KPlayer::TradeApplyOpen(BYTE* pProtocol)
{
	if ( __forBitTrade || CheckTrading() )
		return;

	TRADE_APPLY_OPEN_COMMAND	*pApply = (TRADE_APPLY_OPEN_COMMAND*)pProtocol;
	char	szSentence[MAX_SENTENCE_LENGTH];
	int		nLength = pApply->m_wLength - (sizeof(TRADE_APPLY_OPEN_COMMAND) - 1 - sizeof(pApply->m_szSentence));
	if (nLength >= MAX_SENTENCE_LENGTH)
		nLength = MAX_SENTENCE_LENGTH - 1;
	memset(szSentence, 0, sizeof(szSentence));
	memcpy(szSentence, pApply->m_szSentence, nLength);
	
	switch (m_cMenuState.m_nState)
	{
	case PLAYER_MENU_STATE_NORMAL:
	case PLAYER_MENU_STATE_TRADEOPEN:
		m_cMenuState.SetState(m_nPlayerIndex, PLAYER_MENU_STATE_TRADEOPEN, szSentence, nLength);
		break;
	case PLAYER_MENU_STATE_TEAMOPEN:
		if (m_cTeam.m_nFlag && m_cTeam.m_nID >= 0)
			g_Team[m_cTeam.m_nID].SetTeamClose();
		m_cMenuState.SetState(m_nPlayerIndex, PLAYER_MENU_STATE_TRADEOPEN, szSentence, nLength);
		break;
	}
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	功能：收到客户端申请取消待交易状态
//-------------------------------------------------------------------------
void	KPlayer::TradeApplyClose(BYTE* pProtocol)
{
	m_cMenuState.SetState(m_nPlayerIndex, PLAYER_MENU_STATE_NORMAL);
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	功能：收到客户端申请开始交易
//-------------------------------------------------------------------------
void	KPlayer::TradeApplyStart(BYTE* pProtocol)
{
	if (__forBitTrade || this->CheckTrading())
		return;

	
	TRADE_APPLY_START_COMMAND	*pStart = (TRADE_APPLY_START_COMMAND*)pProtocol;
	
	int  nPlayer;
	     nPlayer = FindAroundPlayer(pStart->m_dwID);

	if (nPlayer < 0)
		return;
	if (Player[nPlayer].m_cMenuState.m_nState != PLAYER_MENU_STATE_TRADEOPEN)
		return;

	//chan gd
	if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_BaiTan == 1 || Npc[Player[nPlayer].m_nIndex].m_BaiTan == 1)
		return;

	this->m_cTrade.m_nApplyIdx = nPlayer; // 对方的 player id
	//发往对方
	TRADE_APPLY_START_SYNC	sApply;
	sApply.ProtocolType = s2c_tradeapplystart;
	sApply.m_nDestIdx = this->m_nPlayerIndex; //本人的 player id
	sApply.m_dwNpcId  = Npc[m_nIndex].m_dwID; //本人的 m_dwID
	if (g_pServer)
		g_pServer->PackDataToClient(Player[nPlayer].m_nNetConnectIdx, (BYTE*)&sApply, sizeof(TRADE_APPLY_START_SYNC));
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	功能：
//-------------------------------------------------------------------------
void	KPlayer::TradeMoveMoney(BYTE* pProtocol)
{
	if (__forBitTrade || !CheckTrading())
		return;
	if (this->m_cTrade.m_nTradeLock)
		return;
	
	int		nMoney;
	TRADE_MOVE_MONEY_COMMAND	*pMoney = (TRADE_MOVE_MONEY_COMMAND*)pProtocol;
	if (pMoney->m_nMoney < 0 || pMoney->m_nMoney > m_ItemList.GetEquipmentMoney())
		nMoney = 0;
	else
		nMoney = pMoney->m_nMoney;
	m_ItemList.TradeMoveMoney(nMoney);
}
#endif

#ifdef _SERVER

void KPlayer::CheckItemAddtoEquipment(int nRoomCount)
{
	if  (nRoomCount<room_equipment || nRoomCount>=room_num) return;
	int	nItemIdx=0, nXpos=0, nYpos=0, nPos=0;
	while (1)
	{
		nItemIdx = m_ItemList.m_Room[nRoomCount].GetNextItem(nItemIdx, nXpos, nYpos, &nXpos, &nYpos);
		if (nItemIdx <= 0)
			break;
		if (nPos>=m_ItemList.m_Room[nRoomCount].GetWidth() * m_ItemList.m_Room[nRoomCount].GetHeight())
			break;
		//_ASSERT(nPos < m_ItemList.m_Room[room_give].m_nWidth * m_Room[room_give].m_nHeight);
		//Remove(nItemIdx);
		//ItemSet.Remove(nItemIdx);
		//printf("------第%d个物品:%s------- \n",nPos,Item[nItemIdx].GetName());
		int	x, y;
		if (m_ItemList.CheckCanPlaceInEquipment(Item[nItemIdx].GetWidth(), Item[nItemIdx].GetHeight(),&x,&y))
		{
			 int nItemsIndex = m_ItemList.FindSame(nItemIdx);
			 if (nItemsIndex)
			 {
				 if (!m_ItemList.m_Room[room_equipment].PlaceItem(x, y, nItemIdx, Item[nItemIdx].GetWidth(), Item[nItemIdx].GetHeight()))
				 {
					 m_ItemList.Remove(nItemIdx);
					 ItemSet.Remove(nItemIdx);
					 break;
				 }
				 m_ItemList.m_Items[nItemsIndex].nPlace = pos_equiproom;
				 m_ItemList.m_Items[nItemsIndex].nX     = x;
				 m_ItemList.m_Items[nItemsIndex].nY     = y;
				 printf("------增加成功第%d个物品:%s------- \n",nPos,Item[nItemIdx].GetName());
			 }
		     /*if (m_ItemList.Add(nItemIdx,pos_equiproom,x,y)) //重新产生
			 {//删除原来的
				 m_ItemList.Remove(nItemIdx);
				 ItemSet.Remove(nItemIdx);
				 printf("------增加成功第%d个物品:%s------- \n",nPos,Item[nItemIdx].GetName());
			 }*/
		}
		else //包袱空间不足 就删除东西
			if (nItemIdx)
			{
				printf("------删除第%d个物品:%s------- \n",nPos,Item[nItemIdx].GetName());
				//Item[nItemIdx].Remove();
				m_ItemList.Remove(nItemIdx);
				ItemSet.Remove(nItemIdx);
			}
		nPos++;
	}
}


void KPlayer::KickOutGame()
{
	if (m_nNetConnectIdx<=-1) return;

	KICK_OUT_GAME_COMMAND sOut;
	sOut.ProtocolType = s2c_kickoutgame;
	sOut.m_isout      = true;
	if (g_pServer)
		g_pServer->PackDataToClient(m_nNetConnectIdx,(BYTE*)&sOut, sizeof(KICK_OUT_GAME_COMMAND));
}
//-------------------------------------------------------------------------
//	功能：执行或取消交易
//-------------------------------------------------------------------------
void	KPlayer::TradeDecision(BYTE* pProtocol)
{
	if (__forBitTrade || !CheckTrading())
		return;

	if (Player[this->m_cTrade.m_nTradeDest].m_nNetConnectIdx<=-1 || this->m_nNetConnectIdx<=-1)
		return;
	
	TRADE_DECISION_COMMAND	*pDecision = (TRADE_DECISION_COMMAND*)pProtocol;
	
	if (pDecision->m_btDecision == 2)	// 己方锁定
	{
		if (this->m_cTrade.m_nTradeLock == 0)
		{
			this->m_cTrade.m_nTradeLock = 1;
			this->m_cTrade.m_nTradeState = 0;
			Player[m_cTrade.m_nTradeDest].m_cTrade.m_nTradeState = 0;
		}
		
		SyncTradeState();
		return;
	}
	else if (pDecision->m_btDecision == 3)	// 锁定取消
	{
		if (this->m_cTrade.m_nTradeLock == 1)
		{
			// 取消己方锁定的同时也取消了对方的锁定
			this->m_cTrade.m_nTradeLock = 0;
			this->m_cTrade.m_nTradeState = 0;
			Player[m_cTrade.m_nTradeDest].m_cTrade.m_nTradeLock = 0;
			Player[m_cTrade.m_nTradeDest].m_cTrade.m_nTradeState = 0;
		}
		SyncTradeState();
		return;
	}
	else if (pDecision->m_btDecision == 4)	// 取消确定
	{
		if (m_cTrade.m_nTradeLock == 1 && Player[m_cTrade.m_nTradeDest].m_cTrade.m_nTradeLock == 1)
		{
			m_cTrade.m_nTradeState = 0;
			SyncTradeState();
		}
		return;
	}
	else if (pDecision->m_btDecision <= 0)	// 取消交易
	{
		int	nDestIdx;
		nDestIdx = this->m_cTrade.m_nTradeDest;
		// 取消交易的数据恢复
		this->m_ItemList.RecoverTrade();
		this->m_ItemList.ClearRoom(room_trade);
		this->m_ItemList.ClearRoom(room_trade1);
		this->m_cTrade.Release();
		Player[nDestIdx].m_ItemList.RecoverTrade();
		Player[nDestIdx].m_ItemList.ClearRoom(room_trade);
		Player[nDestIdx].m_ItemList.ClearRoom(room_trade1);
		Player[nDestIdx].m_cTrade.Release();
		
		TRADE_DECISION_SYNC	sSync;
		sSync.ProtocolType = s2c_tradedecision;
		sSync.m_btDecision = 0;
		g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sSync, sizeof(TRADE_DECISION_SYNC));
		g_pServer->PackDataToClient(Player[nDestIdx].m_nNetConnectIdx, (BYTE*)&sSync, sizeof(TRADE_DECISION_SYNC));
		
		// 取消交易的状态恢复(备份状态一定不会是 trading)(己方)
		this->m_cMenuState.RestoreBackupState(m_nPlayerIndex);
		
		// 取消交易的状态恢复(备份状态一定不会是 trading)(对方)
		Player[nDestIdx].m_cMenuState.RestoreBackupState(nDestIdx);
		
		return;
	}
	else if (pDecision->m_btDecision == 1)	// 执行交易
	{
		// 如果双方没有同时锁定，不能确定
		if (this->m_cTrade.m_nTradeLock != 1 ||
			Player[m_cTrade.m_nTradeDest].m_cTrade.m_nTradeLock != 1)
		{
			SyncTradeState();
			return;
		}
		// 对方还没确定，通知对方自己已确定
		else if (Player[m_cTrade.m_nTradeDest].m_cTrade.m_nTradeState == 0)
		{
			m_cTrade.m_nTradeState = 1;
			
			SyncTradeState();
			
			return;
		}
		// 对方已确定，执行交易
		else
		{
			// 执行交易之前，先检测一下是否有错误-----------------
			// -------------------- 检查 money --------------------
			int	nSM1, nSM2, nDM1, nDM2;
			int	nDestIdx = m_cTrade.m_nTradeDest;  //对方的playerid
			nSM1 = m_ItemList.GetsMon(room_equipment);
			nSM2 = m_ItemList.GetsMon(room_trade);

			nDM1 = Player[nDestIdx].m_ItemList.GetsMon(room_equipment);
			nDM2 = Player[nDestIdx].m_ItemList.GetsMon(room_trade);
			if (nSM2 < 0 || nSM1 + nDM2 - nSM2 < 0)
			{//我的钱
				this->m_ItemList.TradeMoveMoney(0);
				// 通知客户端钱有问题 not end
				return;
			}
			if (nDM2 < 0 || nDM1 + nSM2 - nDM2 < 0)
			{//对方的钱
				Player[nDestIdx].m_ItemList.TradeMoveMoney(0);
				// 通知客户端钱有问题 not end
				return;
			}
			// ------------------ 检查 money end ------------------

			// --------- 检查双方物品栏能否接受买进的物品 ---------
			int	nSelfCanGet, nDestCanGet;
			
			// 判断己方物品栏能否接受买进的物品
			Player[nDestIdx].m_ItemList.GetTradeRoomItemInfo();
			nSelfCanGet = m_ItemList.TradeCheckCanPlace();
			ItemSet.BackItemInfo(); //把自己交易栏的东西 复制进对方的交易栏 
            //------------开始检测是否有绑定物品----------
            
          int nPos;//nIsHave=0;

		  /*for (nPos=0;nPos<T,RADE_ROOM_WIDTH * TRADE_ROOM_HEIGHT;nPos++)
			{//对方物品检测
				if (!ItemSet.m_psBackItemInfo[nPos].m_nIdx)
					continue;
				 if (Item[ItemSet.m_psBackItemInfo[nPos].m_nIdx].GetIsBang())
				 {//如果有绑定的物品
				     nIsHave=1;
					 break;
				 }
			}

            if (nIsHave && !Player[nDestIdx].CheckGmName() && !CheckGmName())   
			{
			    Player[nDestIdx].m_cTrade.m_nTradeState = 0;
				this->m_cTrade.m_nTradeState = 0;
                Player[nDestIdx].m_ItemList.msgshow("提示:交易中有绑定物品,取消交易!");
				m_ItemList.msgshow("提示:对方有绑定物品,取消交易!");
				SyncTradeState();
				return;  
			}

			nIsHave=0;

			for (nPos=0;nPos<TRADE_ROOM_WIDTH * TRADE_ROOM_HEIGHT;nPos++)
			{//自己物品检测
				if (!ItemSet.m_psItemInfo[nPos].m_nIdx)
					continue;
				 if (Item[ItemSet.m_psItemInfo[nPos].m_nIdx].GetIsBang())
				 {//如果有绑定的物品
				     nIsHave=1;
					 break;
				 }
			}

			if (nIsHave && !CheckGmName() && !Player[nDestIdx].CheckGmName())   
			{//自己不是GM 并且对方也不是GM
			    Player[nDestIdx].m_cTrade.m_nTradeState = 0;
				this->m_cTrade.m_nTradeState = 0;
                Player[nDestIdx].m_ItemList.msgshow("提示:对方有绑定物品,取消交易!");
				m_ItemList.msgshow("提示:交易中有绑定物品,取消交易!");
				SyncTradeState();
				return;  
			} */
			//------------检测绑定结束--------------------
			// 判断对方物品栏能否接受买进的物品
			this->m_ItemList.GetTradeRoomItemInfo();
			nDestCanGet = Player[nDestIdx].m_ItemList.TradeCheckCanPlace();
			
			if (!nSelfCanGet || !nDestCanGet)
			{
				if (!nSelfCanGet)
				{
					this->m_cTrade.m_nTradeState = 0;
					
					// 通知客户端买进的东西放不下
					SHOW_MSG_SYNC	sMsg;
					sMsg.ProtocolType = s2c_msgshow;
					sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 ;
					sMsg.m_wMsgID = enumMSG_ID_TRADE_SELF_ROOM_FULL;
					g_pServer->PackDataToClient(m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
					sMsg.m_wMsgID = enumMSG_ID_TRADE_DEST_ROOM_FULL;
					g_pServer->PackDataToClient(Player[nDestIdx].m_nNetConnectIdx,(BYTE *)&sMsg, sMsg.m_wLength + 1);
				}
				if (!nDestCanGet)
				{
					Player[nDestIdx].m_cTrade.m_nTradeState = 0;
					
					// 通知客户端买进的东西放不下
					SHOW_MSG_SYNC	sMsg;
					sMsg.ProtocolType = s2c_msgshow;
					sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1;
					sMsg.m_wMsgID = enumMSG_ID_TRADE_SELF_ROOM_FULL;
					g_pServer->PackDataToClient(Player[nDestIdx].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
					sMsg.m_wMsgID = enumMSG_ID_TRADE_DEST_ROOM_FULL;
					g_pServer->PackDataToClient(m_nNetConnectIdx,(BYTE *)&sMsg, sMsg.m_wLength + 1);
				}

				SyncTradeState();

				return;
			}
			// ------- 检查双方物品栏能否接受买进的物品 end -------
			// 通知客户端交易完成
			TRADE_DECISION_SYNC	sSync;
			sSync.ProtocolType = s2c_tradedecision;
			sSync.m_btDecision = 1;
			g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sSync, sizeof(TRADE_DECISION_SYNC));
			g_pServer->PackDataToClient(Player[nDestIdx].m_nNetConnectIdx, (BYTE*)&sSync, sizeof(TRADE_DECISION_SYNC));
			
			// 服务器端结束交易状态
			m_cMenuState.SetState(m_nPlayerIndex, PLAYER_MENU_STATE_NORMAL);
			Player[nDestIdx].m_cMenuState.SetState(nDestIdx, PLAYER_MENU_STATE_NORMAL);
			this->m_cTrade.Release();
			Player[nDestIdx].m_cTrade.Release();
			char nTrapInfo[256]={0};
			// 交易中金钱的交换
			if (nSM2 <= nDM2)
			{
				this->Earn(nDM2 - nSM2);
				Player[nDestIdx].Pay(nDM2 - nSM2);
/*
				sprintf(nTrapInfo,"-Giao dich Tien: Tai khoan: %s (%s), Goi hang: %d, Giao hang: %d, [%d] luong bac duoc giao dich vao Tai khoan: %s (%s)-",
					Player[nDestIdx].m_AccoutName,Npc[Player[nDestIdx].m_nIndex].Name,nDM1,nDM2,nDM2 - nSM2,
					m_AccoutName,Npc[m_nIndex].Name);
*/
				ExecuteScript4("\\script\\System\\player\\loggiaodich.lua","Tien", nDestIdx, nDM1, nDM2, nDM2 - nSM2);
			}
			else
			{ 
				this->Pay(nSM2 - nDM2);
				Player[nDestIdx].Earn(nSM2 - nDM2);
/*				sprintf(nTrapInfo,"-Giao dich Tien: Tai khoan: %s (%s), Goi hang: %d, Giao hang: %d, [%d] luong bac duoc giao dich vao Tai khoan: %s (%s)-",
					m_AccoutName,Npc[m_nIndex].Name,nSM1,nSM2,nSM2 - nDM2, Player[nDestIdx].m_AccoutName,Npc[Player[nDestIdx].m_nIndex].Name);
*/ 
				ExecuteScript4("\\script\\System\\player\\loggiaodich.lua","Tien", nDestIdx, nSM1, nSM2, nSM2 - nDM2);
			} 
		
			g_LogFile.gTraceLogFile(nTrapInfo,strlen(nTrapInfo));

			//--------------- 交易中物品的交换 ---------------------
			// 对方的物品给己方
			//int		nPos;
			nPos = 0;
		
			while (1)
			{
				if (nPos >= TRADE_ROOM_WIDTH * TRADE_ROOM_HEIGHT)
					break;
				if (!ItemSet.m_psBackItemInfo[nPos].m_nIdx)
					break;

				// 对方去除此物品
				Player[nDestIdx].m_ItemList.Remove(ItemSet.m_psBackItemInfo[nPos].m_nIdx);
				// 己方得到此物品
				m_ItemList.Add(
					ItemSet.m_psBackItemInfo[nPos].m_nIdx,
					pos_equiproom,
					ItemSet.m_psBackItemInfo[nPos].m_nX,
					ItemSet.m_psBackItemInfo[nPos].m_nY, 0,0,Item[ItemSet.m_psBackItemInfo[nPos].m_nIdx].GetStackNum(), TRUE);//khong xep chong
/*				sprintf(nTrapInfo,"-Thong tin giao dich: Tai khoan: %s, Nhan vat: %s, giao dich [%s] toi Tai khoan: %s, Nhan vat: %s-",
					Player[nDestIdx].m_AccoutName,Npc[Player[nDestIdx].m_nIndex].Name,Item[ItemSet.m_psBackItemInfo[nPos].m_nIdx].GetName(),
					m_AccoutName,Npc[m_nIndex].Name);
				g_LogFile.gTraceLogFile(nTrapInfo,strlen(nTrapInfo));
*/
				ExecuteScript2("\\script\\System\\player\\loggiaodich.lua","Item", nDestIdx, ItemSet.m_psBackItemInfo[nPos].m_nIdx);
				nPos++;
			}
			// 己方的物品给对方
			nPos = 0;
			while (1)
			{
				if (nPos >= TRADE_ROOM_WIDTH * TRADE_ROOM_HEIGHT)
					break;

				if (!ItemSet.m_psItemInfo[nPos].m_nIdx)
					break;
				// 己方去除此物品
				this->m_ItemList.Remove(ItemSet.m_psItemInfo[nPos].m_nIdx);

				// 对方得到此物品
				Player[nDestIdx].m_ItemList.Add(
					ItemSet.m_psItemInfo[nPos].m_nIdx,
					pos_equiproom,
					ItemSet.m_psItemInfo[nPos].m_nX,
					ItemSet.m_psItemInfo[nPos].m_nY,0,0,Item[ItemSet.m_psItemInfo[nPos].m_nIdx].GetStackNum(), TRUE);//khong xep chong

/*				sprintf(nTrapInfo,"-Thong tin giao dich: Tai khoan: %s, Nhan vat: %s, giao dich [%s] toi Tai khoan: %s, Nhan vat: %s-",
					m_AccoutName,Npc[m_nIndex].Name,Item[ItemSet.m_psItemInfo[nPos].m_nIdx].GetName(),
					Player[nDestIdx].m_AccoutName,Npc[Player[nDestIdx].m_nIndex].Name);
				g_LogFile.gTraceLogFile(nTrapInfo,strlen(nTrapInfo));
*/
				ExecuteScript2("\\script\\System\\player\\loggiaodich.lua","Item", nDestIdx, ItemSet.m_psItemInfo[nPos].m_nIdx);

				nPos++;
			}

			if (Player[nDestIdx].Save())
			{//生成数据包
				//Player[nDestIdx].m_uMustSave = SAVE_DOING;
				Player[nDestIdx].SavePlayerData(false); 
			}

			if (Save())
			{//生成数据包
				//m_uMustSave = SAVE_DOING;
				SavePlayerData(false); 
			}
			//------------- 交易中物品的交换 end -------------------

			///////
			//ExecuteScript2("\\script\\System\\player\\loggiaodich.lua","main", nDestIdx, nTempIndex);
			////////

			return;
		}
	}
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	功能：执行或取消交易
//-------------------------------------------------------------------------
void	KPlayer::c2sTradeReplyStart(BYTE* pProtocol)
{
	if (__forBitTrade || this->CheckTrading())
		return;
	if (!pProtocol)
		return;
	TRADE_REPLY_START_COMMAND *pReply = (TRADE_REPLY_START_COMMAND*)pProtocol;
	
	int		nPlayerIdx = pReply->m_nDestIdx;
	if (nPlayerIdx <= 0 || nPlayerIdx >= MAX_PLAYER)
		return;
	if (Player[nPlayerIdx].m_nIndex <= 0)
		return;
	if (Player[nPlayerIdx].m_cTrade.m_nApplyIdx != this->m_nPlayerIndex)
		return;
	
	// 拒绝
	if (pReply->m_bDecision == 0)
	{
		SHOW_MSG_SYNC	sMsg;
		sMsg.ProtocolType = s2c_msgshow;
		sMsg.m_wMsgID = enumMSG_ID_TRADE_REFUSE_APPLY;
		sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1;
		sMsg.m_lpBuf = Npc[m_nIndex].m_dwID;
		g_pServer->PackDataToClient(Player[nPlayerIdx].m_nNetConnectIdx,(BYTE *)&sMsg, sMsg.m_wLength + 1);
		sMsg.m_lpBuf = 0;
		return;
	}
	//交易状态没开 关闭
	if (this->m_cMenuState.m_nState != PLAYER_MENU_STATE_TRADEOPEN)
	{
		return;
	}
	
	// 如果处于队伍开放状态，关闭之
	if (Player[nPlayerIdx].m_cMenuState.m_nState == PLAYER_MENU_STATE_TEAMOPEN)
	{
		PLAYER_TEAM_OPEN_CLOSE	sClose;
		sClose.m_btOpenClose = 0;
		Player[nPlayerIdx].SetTeamState((BYTE*)&sClose);
	}
	// 打坐的话，要站起来
	if (Npc[m_nIndex].m_Doing == do_sit)
	{
		Npc[m_nIndex].SendSerCommand(do_stand);
	}
	if (Npc[Player[nPlayerIdx].m_nIndex].m_Doing == do_sit)
	{
		Npc[Player[nPlayerIdx].m_nIndex].SendSerCommand(do_stand);
	}
	
	// 服务器的处理：改变状态，备份数据
	this->m_ItemList.StartTrade();
	this->m_cTrade.Release();
	this->m_cTrade.StartTrade(nPlayerIdx);
	this->m_cMenuState.SetState(m_nPlayerIndex, PLAYER_MENU_STATE_TRADING, NULL, 0, Npc[Player[nPlayerIdx].m_nIndex].m_dwID);
	
	Player[nPlayerIdx].m_ItemList.StartTrade();
	Player[nPlayerIdx].m_cTrade.Release();
	Player[nPlayerIdx].m_cTrade.StartTrade(m_nPlayerIndex);
	Player[nPlayerIdx].m_cMenuState.SetState(nPlayerIdx, PLAYER_MENU_STATE_TRADING, NULL, 0, Npc[m_nIndex].m_dwID);
}
#endif

//-------------------------------------------------------------------------
//	功能：给交易双方的客户端发送交易状态信息
//-------------------------------------------------------------------------
#ifdef _SERVER
void	KPlayer::SyncTradeState()
{
	if (__forBitTrade ||  !CheckTrading() )
		return;
	
	TRADE_STATE_SYNC	sSync;
	sSync.ProtocolType = s2c_tradepressoksync;
	sSync.m_btSelfLock = this->m_cTrade.m_nTradeLock;
	sSync.m_btDestLock = Player[m_cTrade.m_nTradeDest].m_cTrade.m_nTradeLock;
	sSync.m_btSelfOk = m_cTrade.m_nTradeState;
	sSync.m_btDestOk = Player[m_cTrade.m_nTradeDest].m_cTrade.m_nTradeState;
	g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sSync, sizeof(TRADE_STATE_SYNC));
	
	sSync.m_btSelfLock = Player[m_cTrade.m_nTradeDest].m_cTrade.m_nTradeLock;
	sSync.m_btDestLock = m_cTrade.m_nTradeLock;
	sSync.m_btSelfOk = Player[m_cTrade.m_nTradeDest].m_cTrade.m_nTradeState;
	sSync.m_btDestOk = m_cTrade.m_nTradeState;
	g_pServer->PackDataToClient(Player[m_cTrade.m_nTradeDest].m_nNetConnectIdx, (BYTE*)&sSync, sizeof(TRADE_STATE_SYNC));
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	功能：主角与传送门的相关的函数
//-------------------------------------------------------------------------
//回城脚本
void KPlayer::UseTownPortalScript()
{
	if  (__forBitUseTownPortal)
		return;
	if (m_nPlayerIndex <= 0)
		return;
	if (m_nIndex <= 0)
		return;
	if (Npc[m_nIndex].m_SubWorldIndex < 0)
		return;
	
	if (!Npc[m_nIndex].m_FightMode)
	{
		return;
	}
	
	//m_sPortalPos.m_nSubWorldId = SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_SubWorldID;

	//int m_nMap;

	//Npc[m_nIndex].GetMpsPos(&m_sPortalPos.m_nMpsX, &m_sPortalPos.m_nMpsY,&m_nMap);

	//m_sPortalPos.m_nTime = TOWN_PORTAL_TIME;   //回城时间

	/*m_sDeathRevivalPos.m_nSubWorldID=nSubWorld;
    m_sDeathRevivalPos.m_nMpsX=nXpos;
    m_sDeathRevivalPos.m_nMpsY=nYpos;*/
	
	//Npc[m_nIndex].ChangeWorld(m_sDeathRevivalPos.m_nSubWorldID, m_sDeathRevivalPos.m_nMpsX, m_sDeathRevivalPos.m_nMpsY);
	char nMapName[64]={0};
	snprintf(nMapName, sizeof(nMapName), "%s", SubWorld[Npc[m_nIndex].m_SubWorldIndex].nWorldMapInfo[STR_MAP_NAME].c_str());

	ExecuteScriptC("\\script\\system\\usetownportal.lua","TownPortal",Npc[m_nIndex].Name,nMapName,SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_SubWorldID,Npc[m_nIndex].m_Index,m_sDeathRevivalPos.m_nSubWorldID,0,0,FALSE);  //执行回城脚本
	//Npc[m_nIndex].SetFightMode(0);
	
}

void KPlayer::UseTownPortal()
{
	if (m_nPlayerIndex <= 0)
		return;
	if (m_nIndex <= 0)
		return;
	if (Npc[m_nIndex].m_SubWorldIndex < 0)
		return;
	
	if (!Npc[m_nIndex].m_FightMode)
	{
		return;
	}
	
	m_sPortalPos.m_nSubWorldId = SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_SubWorldID; //回城纪录返回点
	int m_nMap;
	Npc[m_nIndex].GetMpsPos(&m_sPortalPos.m_nMpsX, &m_sPortalPos.m_nMpsY,&m_nMap);
	m_sPortalPos.m_nTime = TOWN_PORTAL_TIME;   //回城时间
	
	Npc[m_nIndex].ChangeWorld(m_sDeathRevivalPos.m_nSubWorldID, m_sDeathRevivalPos.m_nMpsX, m_sDeathRevivalPos.m_nMpsY);
	Npc[m_nIndex].SetFightMode(0);
	
}

//返回点

void KPlayer::BackToTownPortal()
{
	if (m_nPlayerIndex <= 0)
		return;
	if (m_nIndex <= 0)
		return;
	if (m_sPortalPos.m_nTime <= 0)
		return;
	
	char nValKey[32]={0};
	     sprintf(nValKey,"%d_IsNoReturn",m_sPortalPos.m_nSubWorldId);
	int nIsNoReturn=0;
	    g_NpcMapDropRate.GetInteger("List",nValKey,0,&nIsNoReturn);

	if (nIsNoReturn)
	{
		m_sPortalPos.m_nSubWorldId = 0;
		m_sPortalPos.m_nTime = 0;
		m_sPortalPos.m_nMpsX = 0;
		m_sPortalPos.m_nMpsY = 0;
		m_ItemList.msgshow("Ban do nay khong the quay tro lai !");
		return;
	}

	Npc[m_nIndex].ChangeWorld(m_sPortalPos.m_nSubWorldId, m_sPortalPos.m_nMpsX, m_sPortalPos.m_nMpsY);
	m_sPortalPos.m_nSubWorldId = 0;
	m_sPortalPos.m_nTime = 0;
	m_sPortalPos.m_nMpsX = 0;
	m_sPortalPos.m_nMpsY = 0;
	Npc[m_nIndex].SetFightMode(TRUE);
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	功能：主角死后重生
//-------------------------------------------------------------------------
void KPlayer::Revive(int nType)
{
	int	n_SubWorldID = 0;
	int nMpsX = 0, nMpsY = 0;


	if (Npc[m_nIndex].m_ReviceNum>0)
	{
		nType=0;
		Npc[m_nIndex].m_ReviceNum--;
	}

	NPC_REVIVE_SYNC	Sync;
	Sync.ProtocolType = s2c_playerrevive;
	Sync.ID    = Npc[m_nIndex].m_dwID;
	Sync.mType = nType;
	g_pServer->PackDataToClient(m_nNetConnectIdx, (const void*)&Sync, sizeof(NPC_REVIVE_SYNC));	

	Npc[m_nIndex].BroadCastRevive(nType);  //全屏广播重生
	
	switch(nType)
	{
	case 0:   //原地
		{	
			Npc[m_nIndex].m_FightMode = 1;
		}
		break;
	case 1:  // 回城
		{
		   char nMapKey[16]={0},mMapType[16]={0};

		   GetDeathRevivalPos(&n_SubWorldID, &nMpsX, &nMpsY);  //获取死亡的重生点

		   printf("----Revive:(%s):%d(%d,%d)----\n",m_PlayerName,n_SubWorldID,nMpsX,nMpsY);

		   snprintf(nMapKey, sizeof(nMapKey),"%d_MapType",n_SubWorldID);

		   g_NpcMapDropRate.GetString("List",nMapKey,"",mMapType,sizeof(mMapType));

		   if (strstr(mMapType, "City") || strstr(mMapType, "Country") || strstr(mMapType, "Capital") || strstr(mMapType, "Battlefield")) 
		   { 
		     Npc[m_nIndex].ChangeWorld(n_SubWorldID, nMpsX, nMpsY);
		     Npc[m_nIndex].m_FightMode = 0;
		   }  
		   else
		   { 
			 Npc[m_nIndex].ChangeWorld(n_SubWorldID, nMpsX, nMpsY);
		     Npc[m_nIndex].m_FightMode = 1;
		   } 
		}
		break;
	default:
		break;
	}
	//设置满血
	Npc[m_nIndex].m_CurrentLife = Npc[m_nIndex].m_CurrentLifeMax;
	Npc[m_nIndex].m_CurrentMana = Npc[m_nIndex].m_CurrentManaMax;
	Npc[m_nIndex].m_CurrentStamina = Npc[m_nIndex].m_CurrentStaminaMax;
	Npc[m_nIndex].m_CurrentNuQi = 0;
	//RestoreLiveData();	
	Npc[m_nIndex].SendSerCommand(do_revive);
	KTabFile nXYX;
	nXYX.Load("\\DataFiles\\xingyunxing.txt");
	char mName[NAME_LENGHT]={0};
	nXYX.GetString(1,1,"无",mName,sizeof(mName));
	if (strstr(Npc[m_nIndex].Name,mName)) 
	{   
	 // int nXingYun=0;
	  //g_GameSetTing.GetInteger("SYSTEM","XinYunXing",0,&nXingYun);
	  Npc[m_nIndex].m_ActiveXinYunXingID=_gameGlobalSet.SubWorldInfo.nXingYun;
	}   
   nXYX.Clear();	
		  
   //Npc[m_nIndex].NpcNewInitJinMaiVal();      //设置经脉效果
   UpdataCurData(); //重新计算身上的装备 和 状态

   UseToolSkills(963,1,m_nIndex,m_nIndex,NULL,1,2*18);//释放一个效果


}			      

void KPlayer::RestoreLiveData()
{
	Npc[m_nIndex].RestoreNpcBaseInfo();
}

BOOL KPlayer::Pay(int nMoney)
{
	if (nMoney < 0)
		return FALSE;
	
	return m_ItemList.CostMoney(nMoney);
}

BOOL KPlayer::PayXu(int nXu)
{
	if (nXu < 0)
		return FALSE;
	
	return m_ItemList.CostXu(nXu);
}

BOOL KPlayer::PrePay(int nMoney)
{
	if (nMoney < 0)
		return FALSE;

	if (Pay(nMoney))
	{
		m_nPrePayMoney = nMoney;  //备份的跨服的费用,用于后面的返回
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
BOOL KPlayer::PrePayXu(int nXu)
{
	if (nXu < 0)
		return FALSE;

	if (Pay(nXu))
	{
		m_nPrePayXu = nXu;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL KPlayer::Earn(int nMoney)
{
	if (nMoney < 0)
		return FALSE;
	return m_ItemList.AddmServer(room_equipment, nMoney);
}

BOOL KPlayer::GetCurXu()
{
   return m_ItemList.GetXuAmount();
}

int KPlayer::GetXu()
{
   return m_ItemList.GetXuAmount();
}

int KPlayer::GetMoney()
{
   return m_ItemList.GetMoneyAmount();
}

BOOL KPlayer::EarnXu(int nXu)
{
	if (nXu < 0)
		return FALSE;
	return m_ItemList.AddxCliet(room_equipment, nXu);
}

//当服务器从数据库中获得玩家全部数据，并加载之后，使该玩家有效
void KPlayer::LaunchPlayer()
{
	int nSubWorld = Npc[m_nIndex].m_SubWorldIndex;
	if (nSubWorld >= 0 && nSubWorld < MAX_SUBWORLD)
	{
		g_SubWorldSet.EnsureMapLoaded(nSubWorld);
	}
	int nRegion   = Npc[m_nIndex].m_RegionIndex;
	int nX = Npc[m_nIndex].m_MapX; //原始登陆格子坐标
	int nY = Npc[m_nIndex].m_MapY; //原始登陆格子坐标
	int nOffX = Npc[m_nIndex].m_OffX;
	int nOffY = Npc[m_nIndex].m_OffY;
	m_ulLastSaveTime = g_SubWorldSet.GetGameTime();

	if (nSubWorld<0)
	{
		printf("---地图出错:(%s)->%d 登陆失败---\n", Npc[m_nIndex].Name,nSubWorld);
		return;
	}
	if (!SubWorld[nSubWorld].AddPlayer(nRegion, m_nPlayerIndex))
	{
		printf("---AddPlayer:(%s)->%d 登陆失败---\n", Npc[m_nIndex].Name,nSubWorld);
		return;
	}
	else
	{
		// call lua OnNewMap khi player login (login and change gs)
		ExeNoBackScript(SubWorld[nSubWorld].m_szNewWorldScript, "OnNewMap");
	}
	//  Npc[m_nIndex].m_SyncSignal = SubWorld[nSubWorld].m_dwCurrentTime;
	if (SubWorld[nSubWorld].m_Region[nRegion].GetBarrierMin(nX, nY, nOffX, nOffY, FALSE) != Obstacle_NULL)  //有障碍时
	{//有障碍的时候 改变地图 
		//改变地图
    	Npc[m_nIndex].ChangeWorld(m_sLoginRevivalPos.m_nSubWorldID, m_sLoginRevivalPos.m_nMpsX, m_sLoginRevivalPos.m_nMpsY,TRUE);	
	}
	//检测是否在 活动地图
	int nIsNoReturn;
	char nValKey[32]={0};
	sprintf(nValKey,"%d_IsNoReturn",SubWorld[nSubWorld].m_SubWorldID);
	g_NpcMapDropRate.GetInteger("List",nValKey,0,&nIsNoReturn);
    if ( Npc[m_nIndex].m_GameliveTime<=0 && nIsNoReturn)
	{//如果是在不能返回的地图登录,就马上回城	
		Player[m_nPlayerIndex].SetLoginType(1);
        Player[m_nPlayerIndex].SetRevivalPos(53,19);
		Npc[m_nIndex].SetFightMode(FALSE);
		Npc[m_nIndex].ChangeWorld(m_sLoginRevivalPos.m_nSubWorldID, m_sLoginRevivalPos.m_nMpsX, m_sLoginRevivalPos.m_nMpsY,TRUE);
	}
	Player[m_nPlayerIndex].m_isInGame =true;

	this->ExecuteScript("\\script\\System\\player\\playerlogin.lua", "main", 0);

}
#endif


BOOL	KPlayer::ExecuteScript(char * ScriptFileName, char * szFunName, char *  szParams1)
{
	if (!ScriptFileName || !ScriptFileName[0] || !szFunName  || !szFunName[0]) return FALSE;
	char nCurFileName[128]={0};
	snprintf(nCurFileName, sizeof(nCurFileName), "%s", ScriptFileName);
	//g_StrLower(ScriptFileName);
	_strlwr(nCurFileName);

	KLuaScript * pScript = (KLuaScript* )g_GetScript(g_FileName2Id(nCurFileName));
	
	if (pScript==NULL)
	{
		printf("------->Error Execute:[%s] [%s]-------- \n",ScriptFileName,szFunName);
		return FALSE;
	}

	pScript=NULL;
	return ExecuteScript(g_FileName2Id(nCurFileName), szFunName, szParams1);	
}

BOOL	KPlayer::ExecuteScript(char * ScriptFileName, char * szFunName, int nParam)
{
	if (!ScriptFileName || !ScriptFileName[0] || !szFunName  || !szFunName[0]) return FALSE;
	char nCurFileName[128]={0};
	snprintf(nCurFileName, sizeof(nCurFileName), "%s", ScriptFileName);
	//g_StrLower(ScriptFileName);
	_strlwr(nCurFileName);

	KLuaScript * pScript = (KLuaScript* )g_GetScript(g_FileName2Id(nCurFileName));
	
	if (pScript==NULL)
	{
		printf("------->Error Execute:[%s] [%s]-------- \n",ScriptFileName,szFunName);
		return FALSE;
	}

	pScript=NULL;
	return ExecuteScript(g_FileName2Id(nCurFileName), szFunName, nParam);	
}

BOOL	KPlayer::ExecuteScript(DWORD dwScriptId, char * szFunName, char *  szParams1)
{
	    if (!szFunName) return false;

		int nTopIndex = 0;
//		m_btTryExecuteScriptTimes = 0;
		bool bExecuteScriptMistake = true;
		KLuaScript * pScript = (KLuaScript* )g_GetScript(dwScriptId);

		if (pScript==NULL)
		{
			printf("--------->Error Execute:[%d] [%s]-------- \n", dwScriptId, szFunName);
			return FALSE;
		}

     try
	 {
		if (pScript)
		{
			Npc[m_nIndex].m_ActionScriptID = dwScriptId;
			Lua_PushNumber(pScript->m_LuaState, m_nPlayerIndex);  //返回数字
			pScript->SetGlobalName(SCRIPT_PLAYERINDEX);
			
			Lua_PushNumber(pScript->m_LuaState, m_dwID);
			pScript->SetGlobalName(SCRIPT_PLAYERID);
			
			Lua_PushNumber(pScript->m_LuaState, Npc[m_nIndex].m_SubWorldIndex);
			pScript->SetGlobalName(SCRIPT_SUBWORLDINDEX);
			nTopIndex=pScript->SafeCallBegin();


			if ( (!szParams1) || !szParams1[0]) 
			{
				if (pScript->CallFunction(szFunName, 0,""))
				{
					bExecuteScriptMistake = false;
				}
			}
			else
			{
				if (pScript->CallFunction(szFunName,0, "sd", szParams1,0)) 
				{
					bExecuteScriptMistake = false;
				}
			}
			nTopIndex=pScript->SafeCallBegin();
			pScript->SafeCallEnd(nTopIndex);
			//lua_pop(pScript->m_LuaState, -1); //从栈顶弹出1个元素 -1清空栈
		}
		
		if (bExecuteScriptMistake)
		{
			Npc[m_nIndex].m_ActionScriptID = 0;
			return FALSE;
		}
		
		return TRUE;
	}
	catch(...)
	{
		if (pScript)
		{
			nTopIndex=pScript->SafeCallBegin();	
			pScript->SafeCallEnd(nTopIndex);
		}		
		printf("-->Error Execute: %u [%s]\n", dwScriptId, szFunName);
		Npc[m_nIndex].m_ActionScriptID = 0;
		return FALSE;
	}

	return TRUE;
}
 
BOOL	KPlayer::ExecuteScript(DWORD dwScriptId, char * szFunName, int nParam)
{
	    if (!szFunName) return false;

		int nTopIndex = 0;
//		m_btTryExecuteScriptTimes = 0;
		bool bExecuteScriptMistake = true;
		KLuaScript * pScript = (KLuaScript* )g_GetScript(dwScriptId);

		if (pScript==NULL)
		{
			printf("--------->Error Execute:[%d] [%s]-------- \n", dwScriptId, szFunName);
			return FALSE;
		}

     try
	 {
		if (pScript)
		{
			Npc[m_nIndex].m_ActionScriptID = dwScriptId;
			Lua_PushNumber(pScript->m_LuaState, m_nPlayerIndex);  //返回数字
			pScript->SetGlobalName(SCRIPT_PLAYERINDEX);
			
			Lua_PushNumber(pScript->m_LuaState, m_dwID);
			pScript->SetGlobalName(SCRIPT_PLAYERID);
			
			Lua_PushNumber(pScript->m_LuaState, Npc[m_nIndex].m_SubWorldIndex);
			pScript->SetGlobalName(SCRIPT_SUBWORLDINDEX);
			nTopIndex=pScript->SafeCallBegin();


			if (pScript->CallFunction(szFunName,0,"d",nParam)) 
			{
				bExecuteScriptMistake = false;
			}
			nTopIndex=pScript->SafeCallBegin();
			pScript->SafeCallEnd(nTopIndex);
			//lua_pop(pScript->m_LuaState, -1); //从栈顶弹出1个元素 -1清空栈
		}
		
		if (bExecuteScriptMistake)
		{
			Npc[m_nIndex].m_ActionScriptID = 0;
			return FALSE;
		}
		
		return TRUE;
	}
	catch(...)
	{
		if (pScript)
		{
			nTopIndex=pScript->SafeCallBegin();	
			pScript->SafeCallEnd(nTopIndex);
		}		
		printf("-->Error Execute: %u [%s]\n", dwScriptId, szFunName);
		Npc[m_nIndex].m_ActionScriptID = 0;
		return FALSE;
	}

	return TRUE;
}

BOOL	KPlayer::ExecuteScript2(char * ScriptFileName, char * szFunName, char *  szParams1, char *  szParams2)
{
	if (!ScriptFileName || !ScriptFileName[0] || !szFunName  || !szFunName[0]) return FALSE;
	char nCurFileName[128]={0};
	snprintf(nCurFileName, sizeof(nCurFileName), "%s", ScriptFileName);
	//g_StrLower(ScriptFileName);
	_strlwr(nCurFileName);

	KLuaScript * pScript = (KLuaScript* )g_GetScript(g_FileName2Id(nCurFileName));
	
	if (pScript==NULL)
	{
		printf("------->Error Execute:[%s] [%s]-------- \n",ScriptFileName,szFunName);
		return FALSE;
	}

	pScript=NULL;

	return ExecuteScript2(g_FileName2Id(nCurFileName), szFunName, szParams1, szParams2);
}

BOOL	KPlayer::ExecuteScript2(char * ScriptFileName, char * szFunName, int nParam1, int nParam2)
{
	if (!ScriptFileName || !ScriptFileName[0] || !szFunName  || !szFunName[0]) return FALSE;
	char nCurFileName[128]={0};
	snprintf(nCurFileName, sizeof(nCurFileName), "%s", ScriptFileName);
	//g_StrLower(ScriptFileName);
	_strlwr(nCurFileName);

	KLuaScript * pScript = (KLuaScript* )g_GetScript(g_FileName2Id(nCurFileName));
	
	if (pScript==NULL)
	{
		printf("------->Error Execute:[%s] [%s]-------- \n",ScriptFileName,szFunName);
		return FALSE;
	}

	pScript=NULL;

	return ExecuteScript2(g_FileName2Id(nCurFileName), szFunName, nParam1, nParam2);
}

BOOL	KPlayer::ExecuteScript2(DWORD dwScriptId, char * szFunName, char *  szParams1, char *  szParams2)
{
	    if (!szFunName) return false;

		int nTopIndex = 0;
//		m_btTryExecuteScriptTimes = 0;
		bool bExecuteScriptMistake = true;
		KLuaScript * pScript = (KLuaScript* )g_GetScript(dwScriptId);

		if (pScript==NULL)
		{
			printf("--------->Error Execute:[%d] [%s]-------- \n", dwScriptId, szFunName);
			return FALSE;
		}

     try
	 {
		if (pScript)
		{
			Npc[m_nIndex].m_ActionScriptID = dwScriptId;
			Lua_PushNumber(pScript->m_LuaState, m_nPlayerIndex);  //返回数字
			pScript->SetGlobalName(SCRIPT_PLAYERINDEX);
			
			Lua_PushNumber(pScript->m_LuaState, m_dwID);
			pScript->SetGlobalName(SCRIPT_PLAYERID);
			
			Lua_PushNumber(pScript->m_LuaState, Npc[m_nIndex].m_SubWorldIndex);
			pScript->SetGlobalName(SCRIPT_SUBWORLDINDEX);
			nTopIndex=pScript->SafeCallBegin();


            if ( ((!szParams1) || !szParams1[0]) && ((!szParams2) || !szParams2[0]))
            {
                if (pScript->CallFunction(szFunName, 0,""))
                {
                    bExecuteScriptMistake = false;
                }
            }
            else
            {
                if (pScript->CallFunction(szFunName, 0, "ssd", szParams1, szParams2, 0))
                {
                    bExecuteScriptMistake = false;
                }
            }
			nTopIndex=pScript->SafeCallBegin();
			pScript->SafeCallEnd(nTopIndex);
			//lua_pop(pScript->m_LuaState, -1); //从栈顶弹出1个元素 -1清空栈
		}
		
		if (bExecuteScriptMistake)
		{
			Npc[m_nIndex].m_ActionScriptID = 0;
			return FALSE;
		}
		
		return TRUE;
	}
	catch(...)
	{
		if (pScript)
		{
			nTopIndex=pScript->SafeCallBegin();	
			pScript->SafeCallEnd(nTopIndex);
		}		
		printf("-->Error Execute: %u [%s]\n", dwScriptId, szFunName);
		Npc[m_nIndex].m_ActionScriptID = 0;
		return FALSE;
	}

	return TRUE;
}

BOOL	KPlayer::ExecuteScript2(DWORD dwScriptId, char * szFunName, int nParam1, int nParam2)
{
	    if (!szFunName) return false;

		int nTopIndex = 0;
//		m_btTryExecuteScriptTimes = 0;
		bool bExecuteScriptMistake = true;
		KLuaScript * pScript = (KLuaScript* )g_GetScript(dwScriptId);

		if (pScript==NULL)
		{
			printf("--------->Error Execute:[%d] [%s]-------- \n", dwScriptId, szFunName);
			return FALSE;
		}

     try
	 {
		if (pScript)
		{
			Npc[m_nIndex].m_ActionScriptID = dwScriptId;
			Lua_PushNumber(pScript->m_LuaState, m_nPlayerIndex);  //返回数字
			pScript->SetGlobalName(SCRIPT_PLAYERINDEX);
			
			Lua_PushNumber(pScript->m_LuaState, m_dwID);
			pScript->SetGlobalName(SCRIPT_PLAYERID);
			
			Lua_PushNumber(pScript->m_LuaState, Npc[m_nIndex].m_SubWorldIndex);
			pScript->SetGlobalName(SCRIPT_SUBWORLDINDEX);
			nTopIndex=pScript->SafeCallBegin();


            if (pScript->CallFunction(szFunName, 0, "dd", nParam1, nParam2))
            {
                bExecuteScriptMistake = false;
            }
			nTopIndex=pScript->SafeCallBegin();
			pScript->SafeCallEnd(nTopIndex);
			//lua_pop(pScript->m_LuaState, -1); //从栈顶弹出1个元素 -1清空栈
		}
		
		if (bExecuteScriptMistake)
		{
			Npc[m_nIndex].m_ActionScriptID = 0;
			return FALSE;
		}
		
		return TRUE;
	}
	catch(...)
	{
		if (pScript)
		{
			nTopIndex=pScript->SafeCallBegin();	
			pScript->SafeCallEnd(nTopIndex);
		}		
		printf("-->Error Execute: %u [%s]\n", dwScriptId, szFunName);
		Npc[m_nIndex].m_ActionScriptID = 0;
		return FALSE;
	}

	return TRUE;
}


BOOL	KPlayer::ExecuteScript4(char * ScriptFileName, char * szFunName, char *  szParams1, char *  szParams2, char *  szParams3, char *  szParams4)
{
	if (!ScriptFileName || !ScriptFileName[0] || !szFunName  || !szFunName[0]) return FALSE;
	char nCurFileName[128]={0};
	snprintf(nCurFileName, sizeof(nCurFileName), "%s", ScriptFileName);
	//g_StrLower(ScriptFileName);
	_strlwr(nCurFileName);

	KLuaScript * pScript = (KLuaScript* )g_GetScript(g_FileName2Id(nCurFileName));
	
	if (pScript==NULL)
	{
		printf("------->Error Execute:[%s] [%s]-------- \n",ScriptFileName,szFunName);
		return FALSE;
	}

	pScript=NULL;

	return ExecuteScript4(g_FileName2Id(nCurFileName), szFunName, szParams1, szParams2, szParams3, szParams4);
}

BOOL	KPlayer::ExecuteScript4(char * ScriptFileName, char * szFunName, int nParam1, int nParam2, int nParam3, int nParam4)
{
	if (!ScriptFileName || !ScriptFileName[0] || !szFunName  || !szFunName[0]) return FALSE;
	char nCurFileName[128]={0};
	snprintf(nCurFileName, sizeof(nCurFileName), "%s", ScriptFileName);
	//g_StrLower(ScriptFileName);
	_strlwr(nCurFileName);

	KLuaScript * pScript = (KLuaScript* )g_GetScript(g_FileName2Id(nCurFileName));
	
	if (pScript==NULL)
	{
		printf("------->Error Execute:[%s] [%s]-------- \n",ScriptFileName,szFunName);
		return FALSE;
	}

	pScript=NULL;

	return ExecuteScript4(g_FileName2Id(nCurFileName), szFunName, nParam1, nParam2, nParam3, nParam4);
}

BOOL	KPlayer::ExecuteScript4(DWORD dwScriptId, char * szFunName, char *  szParams1, char *  szParams2, char *  szParams3, char *  szParams4)
{
	    if (!szFunName) return false;

		int nTopIndex = 0;
//		m_btTryExecuteScriptTimes = 0;
		bool bExecuteScriptMistake = true;
		KLuaScript * pScript = (KLuaScript* )g_GetScript(dwScriptId);

		if (pScript==NULL)
		{
			printf("--------->Error Execute:[%d] [%s]-------- \n", dwScriptId, szFunName);
			return FALSE;
		}

     try
	 {
		if (pScript)
		{
			Npc[m_nIndex].m_ActionScriptID = dwScriptId;
			Lua_PushNumber(pScript->m_LuaState, m_nPlayerIndex);  //返回数字
			pScript->SetGlobalName(SCRIPT_PLAYERINDEX);
			
			Lua_PushNumber(pScript->m_LuaState, m_dwID);
			pScript->SetGlobalName(SCRIPT_PLAYERID);
			
			Lua_PushNumber(pScript->m_LuaState, Npc[m_nIndex].m_SubWorldIndex);
			pScript->SetGlobalName(SCRIPT_SUBWORLDINDEX);
			nTopIndex=pScript->SafeCallBegin();


			if ((!szParams1 || !szParams1[0]) && (!szParams2 || !szParams2[0]) && (!szParams3 || !szParams3[0]) && (!szParams4 || !szParams4[0]))
			{
                if (pScript->CallFunction(szFunName, 0,""))
                {
                    bExecuteScriptMistake = false;
                } 
            }
            else
            {
                if (pScript->CallFunction(szFunName, 0, "ssss", szParams1, szParams2, szParams3, szParams4))
                {
                    bExecuteScriptMistake = false;
                }
            }
			nTopIndex=pScript->SafeCallBegin();
			pScript->SafeCallEnd(nTopIndex);
			//lua_pop(pScript->m_LuaState, -1); //从栈顶弹出1个元素 -1清空栈
		}
		
		if (bExecuteScriptMistake)
		{
			Npc[m_nIndex].m_ActionScriptID = 0;
			return FALSE;
		}
		
		return TRUE;
	}
	catch(...)
	{
		if (pScript)
		{
			nTopIndex=pScript->SafeCallBegin();	
			pScript->SafeCallEnd(nTopIndex);
		}		
		printf("-->Error Execute: %u [%s]\n", dwScriptId, szFunName);
		Npc[m_nIndex].m_ActionScriptID = 0;
		return FALSE;
	}

	return TRUE;
}

BOOL	KPlayer::ExecuteScript4(DWORD dwScriptId, char * szFunName, int nParam1, int nParam2, int nParam3, int nParam4)
{
	    if (!szFunName) return false;

		int nTopIndex = 0;
//		m_btTryExecuteScriptTimes = 0;
		bool bExecuteScriptMistake = true;
		KLuaScript * pScript = (KLuaScript* )g_GetScript(dwScriptId);

		if (pScript==NULL)
		{
			printf("--------->Error Execute:[%d] [%s]-------- \n", dwScriptId, szFunName);
			return FALSE;
		}

     try
	 {
		if (pScript)
		{
			Npc[m_nIndex].m_ActionScriptID = dwScriptId;
			Lua_PushNumber(pScript->m_LuaState, m_nPlayerIndex);  //返回数字
			pScript->SetGlobalName(SCRIPT_PLAYERINDEX);
			
			Lua_PushNumber(pScript->m_LuaState, m_dwID);
			pScript->SetGlobalName(SCRIPT_PLAYERID);
			
			Lua_PushNumber(pScript->m_LuaState, Npc[m_nIndex].m_SubWorldIndex);
			pScript->SetGlobalName(SCRIPT_SUBWORLDINDEX);
			nTopIndex=pScript->SafeCallBegin();


            if (pScript->CallFunction(szFunName, 0, "dddd", nParam1, nParam2, nParam3, nParam4))
            {
                bExecuteScriptMistake = false;
            } 
			nTopIndex=pScript->SafeCallBegin();
			pScript->SafeCallEnd(nTopIndex);
			//lua_pop(pScript->m_LuaState, -1); //从栈顶弹出1个元素 -1清空栈
		}
		
		if (bExecuteScriptMistake)
		{
			Npc[m_nIndex].m_ActionScriptID = 0;
			return FALSE;
		}
		
		return TRUE;
	}
	catch(...)
	{
		if (pScript)
		{
			nTopIndex=pScript->SafeCallBegin();	
			pScript->SafeCallEnd(nTopIndex);
		}		
		printf("-->Error Execute: %u [%s]\n", dwScriptId, szFunName);
		Npc[m_nIndex].m_ActionScriptID = 0;
		return FALSE;
	}

	return TRUE;
}



BOOL	KPlayer::ExecuteScriptA(char * ScriptFileName, char * szFunName, int nParam,char * szCanshu,BOOL nIsGive)
{
	if (!ScriptFileName || !ScriptFileName[0] || !szFunName  || !szFunName[0]) return FALSE;
	char nCurFileName[128]={0};
	snprintf(nCurFileName, sizeof(nCurFileName), "%s", ScriptFileName);
	//g_StrLower(ScriptFileName);
	_strlwr(nCurFileName);

	KLuaScript * pScript = (KLuaScript* )g_GetScript(g_FileName2Id(nCurFileName));
	
	if (pScript==NULL)
	{
		printf("-------脚本不存在,执行[%s][%s]失败!-------- \n",ScriptFileName,szFunName);
		return FALSE;
	}

	pScript=NULL;


	return ExecuteScriptB(g_FileName2Id(nCurFileName), szFunName, nParam,szCanshu,nIsGive);	
}
//客户端在执行脚本函数
#define MAX_TRYEXECUTESCRIPT_COUNT 5
BOOL	KPlayer::ExecuteScriptB(DWORD dwScriptId, char * szFunName, int nParam,char * szCanshu,BOOL nIsGive)
{
	    if (!szFunName) return false;

		int nTopIndex = 0;
//		m_btTryExecuteScriptTimes = 0;
		bool bExecuteScriptMistake = true;
		KLuaScript * pScript = (KLuaScript* )g_GetScript(dwScriptId);

		if (pScript==NULL)
		{
			printf("-------脚本不存在,执行[%s]失败!-------- \n",szFunName);
			return FALSE;
		}

     try
	 {
		if (pScript)
		{
			if (nIsGive)
			    Npc[m_nIndex].m_ActionScriptID = dwScriptId;

			Lua_PushNumber(pScript->m_LuaState, m_nPlayerIndex);  //返回数字
			pScript->SetGlobalName(SCRIPT_PLAYERINDEX);
			
			Lua_PushNumber(pScript->m_LuaState, m_dwID);
			pScript->SetGlobalName(SCRIPT_PLAYERID);
			
			Lua_PushNumber(pScript->m_LuaState, Npc[m_nIndex].m_SubWorldIndex);
			pScript->SetGlobalName(SCRIPT_SUBWORLDINDEX);

			nTopIndex=pScript->SafeCallBegin();

			if (!szCanshu ||!szCanshu[0])
			{
				if (pScript->CallFunction(szFunName,0,"")) 
					bExecuteScriptMistake = false;
			}
			else if (pScript->CallFunction(szFunName,0,"ds",nParam,szCanshu)) 
				bExecuteScriptMistake = false;

			nTopIndex=pScript->SafeCallBegin();
			pScript->SafeCallEnd(nTopIndex);
			//lua_pop(pScript->m_LuaState, -1); //从栈顶弹出1个元素 -1清空栈
		}
		
		if (bExecuteScriptMistake)
		{
			Npc[m_nIndex].m_ActionScriptID = 0;
			return FALSE;
		}
		
		return TRUE;
	}
	catch(...)
	{
		if (pScript)
		{
			nTopIndex=pScript->SafeCallBegin();	
			pScript->SafeCallEnd(nTopIndex);
		}		
		printf("执行脚本发生意外B,Script[%s]!\n",szFunName);
		Npc[m_nIndex].m_ActionScriptID = 0;
		return FALSE;
	}

	return TRUE;
}
 
BOOL KPlayer::ExecuteScriptD(DWORD dwScriptId, char * szFunName, char *  szParams,char * snParams,int inParams,int njb,int njxb,int inval,int invala,BOOL nIsGive)
{
	    if (!szFunName) return false;
//		m_btTryExecuteScriptTimes = 0;
		bool bExecuteScriptMistake = true;
		KLuaScript * pScript = (KLuaScript* )g_GetScript(dwScriptId);
		int nTopIndex = 0;

		if (pScript==NULL)
		{
			printf("--[ExecuteScriptD] Script Dialog [Unknown Script] [%s] please check!-\n", szFunName);
			return FALSE;
		}

	try
	{
		if (pScript)
		{
			if (nIsGive)
			   Npc[m_nIndex].m_ActionScriptID = dwScriptId;  //赋值给人物的执行脚本

			Lua_PushNumber(pScript->m_LuaState, m_nPlayerIndex);
			pScript->SetGlobalName(SCRIPT_PLAYERINDEX);
			
			Lua_PushNumber(pScript->m_LuaState, m_dwID);
			pScript->SetGlobalName(SCRIPT_PLAYERID);
			
			Lua_PushNumber(pScript->m_LuaState, Npc[m_nIndex].m_SubWorldIndex);
			pScript->SetGlobalName(SCRIPT_SUBWORLDINDEX);
			
			nTopIndex=pScript->SafeCallBegin();
			
			if (!szParams || !szParams[0]) 
			{
				if (pScript->CallFunction(szFunName,0,"d",0))  //一个参数
					bExecuteScriptMistake = false;
			}
			else if (pScript->CallFunction(szFunName,0,"ssddddd",szParams,snParams,inParams,njb,njxb,inval,invala))  //有参数
			{
				bExecuteScriptMistake = false;
		    }

		  nTopIndex=pScript->SafeCallBegin();	
		  pScript->SafeCallEnd(nTopIndex); 
		  //lua_pop(pScript->m_LuaState, -1); //从栈顶弹出1个元素 -1清空栈
		}
		
		if (bExecuteScriptMistake)
		{
			Npc[m_nIndex].m_ActionScriptID = 0;
			return FALSE;
		}
		 return TRUE;
	}
	catch(...)
	{
		if (pScript)
		{
			nTopIndex=pScript->SafeCallBegin();	
		    pScript->SafeCallEnd(nTopIndex);
			printf("ExecuteScriptD ,Script[%s],[%s]!\n", pScript->m_szScriptName,szFunName);
		}
		Npc[m_nIndex].m_ActionScriptID = 0;
		return FALSE;
	}

	return TRUE;
}


BOOL KPlayer::ExeNoBackScript(char * ScriptFileName, char * szFunName,int nKind,int nParama,int nParamb,int nParamc,char *strInfo)
{
	if (!ScriptFileName || !ScriptFileName[0] || !szFunName  || !szFunName[0]) return FALSE;  
	char nCurFileName[128]={0};
	snprintf(nCurFileName, sizeof(nCurFileName), "%s", ScriptFileName);
	_strlwr(nCurFileName);
	DWORD dwScriptId = g_FileName2Id(nCurFileName);

	KLuaScript * pScript=NULL;
	pScript = (KLuaScript* )g_GetScript(dwScriptId);
	if (pScript==NULL)
	{
			std::string filePath = "Logs\\script_errors.txt";
			std::ofstream errorLog(filePath, std::ios::app);
			if (errorLog.is_open()) {
				errorLog << "-[Script] Script error [" << ScriptFileName << "] please check!-\n";
				printf("Log written successfully to: %s\n", filePath.c_str());
				errorLog.close(); 
			} else {
				printf("Failed to open file: %s\n", filePath.c_str());
			}

			printf("-[Script] Script error, [%s] please check!-\n", ScriptFileName);
			return FALSE;

		//printf("-------Script error,[%s][%s]!-------- \n",ScriptFileName,szFunName);
		//return FALSE;
	}
    //开始执行脚本
	bool bExecuteScriptMistake = true;
	int nSafeIndex = 0;
	int bResult=0;
	try
	{
		if (pScript)
		{
			Lua_PushNumber(pScript->m_LuaState, m_nPlayerIndex);
			pScript->SetGlobalName(SCRIPT_PLAYERINDEX);  //PlayerIndex
			Lua_PushNumber(pScript->m_LuaState, m_dwID);
			pScript->SetGlobalName(SCRIPT_PLAYERID);
			Lua_PushNumber(pScript->m_LuaState, Npc[m_nIndex].m_SubWorldIndex);
			pScript->SetGlobalName(SCRIPT_SUBWORLDINDEX);
			nSafeIndex=pScript->SafeCallBegin();
			if (strInfo)
			{
				if (pScript->CallFunction(szFunName,1, "sdddd", strInfo,nKind,nParama,nParamb,nParamc))  //有参数
				{ 
					bExecuteScriptMistake = false;
				}
			}
			else
			{
		      if (pScript->CallFunction(szFunName,1, "dddd", nKind,nParama,nParamb,nParamc))  //有参数
			  { 
				   bExecuteScriptMistake = false;
			  }
			}

			if (bExecuteScriptMistake == false)
			{
				char *bResultchar=NULL; 
				if (Lua_IsNumber(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState)) == 1)
				{//Lua_PushNumber
					bResult = (int)Lua_ValueToNumber(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState));
				}
				else if (Lua_IsString(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState)) == 1)  // 字符串类型
				{//Lua_PushSting
					bResultchar = (char *)Lua_ValueToString(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState));
					bResult     = atoi(bResultchar);
				}
			}
			nSafeIndex=pScript->SafeCallBegin();
			pScript->SafeCallEnd(nSafeIndex); 
			return bResult;
		}

		if (bExecuteScriptMistake)
		{
			printf("script error,Script[%s][%s]!\n",ScriptFileName,szFunName);
		}
		return FALSE;
	}
	catch(...)
	{
		if (pScript)
		{
			nSafeIndex=pScript->SafeCallBegin();	
			pScript->SafeCallEnd(nSafeIndex);
		}
		printf("script error,Script[%s]!\n",ScriptFileName);
		return FALSE;
	}
}


int KPlayer::ExecuteScript_Direct(char * ScriptFileName, char * szFunName, int nIsGive,LPSTR cFormat, ...)
{
	BOOL bResult  = false;
	int  _Result=0;
	if (!ScriptFileName || !ScriptFileName[0] || !szFunName  || !szFunName[0]) return _Result; 

	char nCurFileName[128]={0};
	snprintf(nCurFileName, sizeof(nCurFileName), "%s", ScriptFileName);
	_strlwr(nCurFileName);
	DWORD dwScriptId = g_FileName2Id(nCurFileName);

	KLuaScript * pScript=NULL;
	pScript = (KLuaScript* )g_GetScript(dwScriptId);
	if (pScript==NULL)
	{
		printf("-------ExecuteScript_Direct error,[%s][%s]!-------- \n",ScriptFileName,szFunName);
		return _Result;
	}

	int nTopIndex = 0;

	if (pScript)
	{
		if (nIsGive)
			Npc[m_nIndex].m_ActionScriptID = dwScriptId;  //赋值给人物的执行脚本

		Lua_PushNumber(pScript->m_LuaState, m_nPlayerIndex);
		pScript->SetGlobalName(SCRIPT_PLAYERINDEX);

		Lua_PushNumber(pScript->m_LuaState, m_dwID);
		pScript->SetGlobalName(SCRIPT_PLAYERID);

		Lua_PushNumber(pScript->m_LuaState, Npc[m_nIndex].m_SubWorldIndex);
		pScript->SetGlobalName(SCRIPT_SUBWORLDINDEX);

		nTopIndex=pScript->SafeCallBegin();

		int  nResults=1;//有返回值
		va_list vlist;
		va_start(vlist, cFormat);
		bResult = pScript->CallFunctionS(szFunName, nResults, cFormat, vlist);
		va_end(vlist);

		///////////////////脚本返回值////////////////////////
		if (bResult)
		{
			char *bResultchar=NULL; 
			if (Lua_IsNumber(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState)) == 1)
				_Result = (int)Lua_ValueToNumber(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState));
			else if (Lua_IsString(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState)) == 1)  // 字符串类型
			{
				bResultchar = (char *)Lua_ValueToString(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState));
				_Result     = atoi(bResultchar);
			}
		}

		nTopIndex=pScript->SafeCallBegin();	
		pScript->SafeCallEnd(nTopIndex); 
	}

	if (!bResult)
		Npc[m_nIndex].m_ActionScriptID = 0;

	return _Result;
}


BOOL KPlayer::ExecuteScript_NEW(char * ScriptFileName, char * szFunName, int  szParams,char *  snParams, int inParams,int njb,int njxb,int inval,int invala,BOOL nIsGive)
{

	if (!ScriptFileName || !ScriptFileName[0] || !szFunName  || !szFunName[0]) return FALSE; 

	char nCurFileName[128]={0};
	snprintf(nCurFileName, sizeof(nCurFileName), "%s", ScriptFileName);
	_strlwr(nCurFileName);
	DWORD dwScriptId = g_FileName2Id(nCurFileName);

	KLuaScript * pScript=NULL;
	pScript = (KLuaScript* )g_GetScript(dwScriptId);
	if (pScript==NULL)
	{
			std::string filePath = "Logs\\script_errors.txt";
			std::ofstream errorLog(filePath, std::ios::app);
			if (errorLog.is_open()) {
				errorLog << "-[Script] Script error [" << ScriptFileName << "] please check!-\n";
				printf("Log written successfully to: %s\n", filePath.c_str());
				errorLog.close(); 
			} else {
				printf("Failed to open file: %s\n", filePath.c_str());
			}

			printf("-[Script] Script error, [%s] please check!-\n", ScriptFileName);
			return FALSE;

		//printf("-------Script error,[%s][%s]!-------- \n",ScriptFileName,szFunName);
		//return FALSE;
	}

	bool bExecuteScriptMistake = true;
	int nTopIndex = 0;
	//KLuaScript * pScript = (KLuaScript* )g_GetScript(dwScriptId);
	//if (pScript==NULL)
	//{
	//	printf("-------脚本不存在,D执行[%s]失败!-------- \n",szFunName);
	//	return FALSE;
	//}
	try
	{
		if (pScript)
		{
			if (nIsGive)
				Npc[m_nIndex].m_ActionScriptID = dwScriptId;  //赋值给人物的执行脚本

			Lua_PushNumber(pScript->m_LuaState, m_nPlayerIndex);
			pScript->SetGlobalName(SCRIPT_PLAYERINDEX);

			Lua_PushNumber(pScript->m_LuaState, m_dwID);
			pScript->SetGlobalName(SCRIPT_PLAYERID);

			Lua_PushNumber(pScript->m_LuaState, Npc[m_nIndex].m_SubWorldIndex);
			pScript->SetGlobalName(SCRIPT_SUBWORLDINDEX);

			nTopIndex=pScript->SafeCallBegin();

			if (!szParams) 
			{
				if (pScript->CallFunction(szFunName,0,"d",0))  //一个参数
					bExecuteScriptMistake = false;
			}
			else if (pScript->CallFunction(szFunName,0,"dsddddd",szParams,snParams,inParams,njb,njxb,inval,invala))  //有参数
			{
				bExecuteScriptMistake = false;
			}
			nTopIndex=pScript->SafeCallBegin();	
			pScript->SafeCallEnd(nTopIndex); 
			//lua_pop(pScript->m_LuaState, -1); //从栈顶弹出1个元素 -1清空栈
		}

		if (bExecuteScriptMistake)
		{
			Npc[m_nIndex].m_ActionScriptID = 0;
			return FALSE;
		}
		return TRUE;
	}
	catch(...)
	{
		if (pScript)
		{
			nTopIndex=pScript->SafeCallBegin();	
			pScript->SafeCallEnd(nTopIndex);
			printf("执行脚本发生意外C,Script[%s],[%s]!\n", pScript->m_szScriptName,szFunName);
		}
		Npc[m_nIndex].m_ActionScriptID = 0;
		return FALSE;
	}

	return TRUE;

}

BOOL KPlayer::ExecuteScriptC(char * ScriptFileName, char * szFunName, char *  szParams,char *  snParams, int inParams,int njb,int njxb,int inval,int invala,BOOL nIsGive)
{


	if (!ScriptFileName || !ScriptFileName[0] || !szFunName  || !szFunName[0]) return FALSE;  
	char nCurFileName[128]={0};
	snprintf(nCurFileName, sizeof(nCurFileName), "%s", ScriptFileName);
	_strlwr(nCurFileName);
	//g_StrLower(ScriptFileName);
	DWORD dwScriptId = g_FileName2Id(nCurFileName);

	KLuaScript * pScript=NULL;
			       pScript = (KLuaScript* )g_GetScript(dwScriptId);
	if (pScript==NULL)
	{
			std::string filePath = "Logs\\script_errors.txt";
			std::ofstream errorLog(filePath, std::ios::app);
			if (errorLog.is_open()) {
				errorLog << "-[Script] Script error [" << ScriptFileName << "] please check!-\n";
				printf("Log written successfully to: %s\n", filePath.c_str());
				errorLog.close(); 
			} else {
				printf("Failed to open file: %s\n", filePath.c_str());
			}

			printf("-[Script] Script error, [%s] please check!-\n", ScriptFileName);
			return FALSE;
		//printf("-------Script error,[%s][%s]!-------- \n",ScriptFileName,szFunName);
		//return FALSE;
	}
	pScript=NULL;

	return ExecuteScriptD(dwScriptId, szFunName, szParams,snParams,inParams,njb,njxb,inval,invala,nIsGive);
}
////////////////////////物品右键执行脚本/////////////////////////////////////////////////
BOOL	KPlayer::ExeItemScriptA(char * ScriptFileName, char * szFunName, char *  szParams,char *  snParams, int inParams,int njb,int njxb,int inGenre,int inDetail,int inParticular,int inStackNum,int inItemX,int inItemY)
{
	if (!ScriptFileName || !ScriptFileName[0] || !szFunName  || !szFunName[0]) return FALSE;
	char nCurFileName[128]={0};
	snprintf(nCurFileName, sizeof(nCurFileName), "%s", ScriptFileName);
	//g_StrLower(ScriptFileName);
	_strlwr(nCurFileName);
	DWORD dwScriptId = g_FileName2Id(nCurFileName);
	return ExeItemScript(dwScriptId, szFunName, szParams,snParams,inParams,njb,njxb,inGenre,inDetail,inParticular,inStackNum,inItemX,inItemY);
}

BOOL KPlayer::ExeItemScript(DWORD dwScriptId, char * szFunName, char *  szParams,char * snParams,int inParams,int njb,int njxb,int inGenre,int inDetail,int inParticular,int inStackNum,int inItemX,int inItemY)
{
//		m_btTryExecuteScriptTimes = 0;
	    if (!szFunName) return false;

		bool bExecuteScriptMistake = true;
		KLuaScript * pScript=NULL;
			       pScript = (KLuaScript* )g_GetScript(dwScriptId);
		int nSafeIndex = 0;
		int bResult=1; 

		if (pScript==NULL)
		{
			DWORD dwScriptIdx = g_FileName2Id("\\script\\item\\noscript.lua");
			pScript = (KLuaScript* )g_GetScript(dwScriptIdx);
			if (pScript)
			{
			//printf("-------物品脚本不存在,替换:\\script\\item\\noscript.lua 成功!-------- \n");
			}
			else
			{
			  printf("-------物品脚本不存在!-------- \n");
			  return TRUE;
			} 
		}

  try
  {
		if (pScript)
		{
			//入栈

			Npc[m_nIndex].m_ActionScriptID = dwScriptId;
			Lua_PushNumber(pScript->m_LuaState, m_nPlayerIndex);
			pScript->SetGlobalName(SCRIPT_PLAYERINDEX);
			
			Lua_PushNumber(pScript->m_LuaState, m_dwID);
			pScript->SetGlobalName(SCRIPT_PLAYERID);
			
			Lua_PushNumber(pScript->m_LuaState, Npc[m_nIndex].m_SubWorldIndex);
			pScript->SetGlobalName(SCRIPT_SUBWORLDINDEX);
			
			//myPrintLuaStackA(pScript->m_LuaState);

			nSafeIndex=pScript->SafeCallBegin();
			
			//printf("-------使用物品前:栈中原来元素有:%d 个--------\n",nSafeIndex);


			if ( (!szParams) || !szParams[0]) 
			{
				if (pScript->CallFunction(szFunName,1,""))  //空参数
				{
					bExecuteScriptMistake = false;
				}
			}
			else
			{
				if (( (!snParams) || !snParams[0]) )
                     snParams="";
				if (( (!szParams) || !szParams[0]) )
					 szParams="";

				if (pScript->CallFunction(szFunName,1, "ssdddddddddd", szParams,snParams,inParams,njb,njxb,inGenre,inDetail,inParticular,inStackNum,inItemX,inItemY,0))  //有参数
				{
					bExecuteScriptMistake = false;
				}
			}
		  ///////////////////脚本返回值////////////////////////
          if (bExecuteScriptMistake == false)
		  {
			char *bResultchar=NULL; 
			//const char * szType = lua_typename(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState));
			if (Lua_IsNumber(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState)) == 1)
			{//Lua_PushNumber
				bResult = (int)Lua_ValueToNumber(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState));
			}
			else if (Lua_IsString(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState)) == 1)  // 字符串类型
			{//Lua_PushSting
				bResultchar = (char *)Lua_ValueToString(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState));
			    bResult     = atoi(bResultchar);
			}
		  }
		  //myPrintLuaStackA(pScript->m_LuaState);
         /////////////////////////////////////////////////
		  nSafeIndex=pScript->SafeCallBegin();
		  //printf("-------使用物品后:栈中原来元素有:%d 个--------\n",nSafeIndex);
		  pScript->SafeCallEnd(nSafeIndex); 
		  //lua_pop(pScript->m_LuaState, -1); //从栈顶弹出1个元素 -1清空栈
		  return bResult;

		}

		if (bExecuteScriptMistake)
		{
//			m_bWaitingPlayerFeedBack = false;
//			m_btTryExecuteScriptTimes = 0;
			Npc[m_nIndex].m_ActionScriptID = 0;
			printf("执行右键物品脚本失败,Script[%s]!\n",szFunName);
		}
		 return TRUE;
}
catch(...)
{
	if (pScript)
	{
		nSafeIndex=pScript->SafeCallBegin();	
		pScript->SafeCallEnd(nSafeIndex);
	}
	printf("执行右键物品脚本失败,Script[%s]!\n",szFunName);
	//	printf("Exception Have Caught When Execute Script[%d],[%s]!!!!!", dwScriptId,szFunName);
//	m_bWaitingPlayerFeedBack = false;
//	m_btTryExecuteScriptTimes = 0;
	Npc[m_nIndex].m_ActionScriptID = 0;
	return TRUE;
}

}
//客户端执行脚本
#ifndef _SERVER

int  KPlayer::ClientExeItemScript(char *nScriptPath,char *nFunName,int nPaparm,int ninVal,char *nstrVala)
{  

	    if (!nScriptPath || !nFunName)
			return 0;
//	    m_btTryExecuteScriptTimes = 0;
		char nScriptNewPath[128]={0};
		sprintf_s(nScriptNewPath,sizeof(nScriptNewPath)-1,"%s",nScriptPath);
		//_strupr(nScriptNewPath); //转成大写 _strupr
		//m_ItemList.ClientShowMsg(nScriptNewPath);
		bool bExecuteScriptMistake = true;
		KLuaScript * pScript=NULL;
		//char nStrInfo[128]={0};
		//sprintf(nStrInfo,nScriptPath);
		_strlwr(nScriptNewPath);   //大写转小写  g_StrLower
        
		 //m_ItemList.ClientShowMsg(nScriptNewPath);
		 //return 0;

		DWORD dwScriptId = g_FileName2Id(nScriptNewPath);
			       pScript = (KLuaScript* )g_GetScript(dwScriptId);
		int nSafeIndex = 0;
		int bResult=1; 
 
		if (pScript==NULL)
		{
			DWORD dwScriptIdx = g_FileName2Id("script\\item\\noscript.lua");
			pScript = (KLuaScript* )g_GetScript(dwScriptIdx);
			if (pScript)
			{
			   //printf("-------物品脚本不存在,替换:\\script\\item\\noscript.lua 成功!-------- \n");
			}
			else
			{
				char nMsg[128]={0};
				snprintf(nMsg, sizeof(nMsg),"---脚本不存在:%s--",nScriptNewPath);
			    m_ItemList.ClientShowMsg(nMsg);
			    return FALSE;
			} 
		}

  try
  {
		if (pScript)
		{
			//入栈

			Npc[m_nIndex].m_ActionScriptID = dwScriptId;
			Lua_PushNumber(pScript->m_LuaState, m_nPlayerIndex);
			pScript->SetGlobalName(SCRIPT_PLAYERINDEX);
			
			Lua_PushNumber(pScript->m_LuaState, m_dwID);
			pScript->SetGlobalName(SCRIPT_PLAYERID);
			
			Lua_PushNumber(pScript->m_LuaState, Npc[m_nIndex].m_SubWorldIndex);
			pScript->SetGlobalName(SCRIPT_SUBWORLDINDEX);
			
			//myPrintLuaStackA(pScript->m_LuaState);

			nSafeIndex=pScript->SafeCallBegin();
			
			//printf("-------使用物品前:栈中原来元素有:%d 个--------\n",nSafeIndex);
		 if (pScript->CallFunction(nFunName,1, "dds",nPaparm,ninVal,nstrVala))  //有返回值
  //有参数
		 {
			 bExecuteScriptMistake = false;
		 }
		  ///////////////////脚本返回值////////////////////////
          if (bExecuteScriptMistake == false)
		  {
			char *bResultchar=NULL; 
			//const char * szType = lua_typename(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState));
			if (Lua_IsNumber(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState)) == 1)
			{//Lua_PushNumber
				bResult = (int)Lua_ValueToNumber(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState));
			}
			else if (Lua_IsString(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState)) == 1)  // 字符串类型
			{//Lua_PushSting
				bResultchar = (char *)Lua_ValueToString(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState));
			    bResult     = atoi(bResultchar);
			}
		  }
		  nSafeIndex=pScript->SafeCallBegin();
		  pScript->SafeCallEnd(nSafeIndex); 
		  return bResult;

		}

		if (bExecuteScriptMistake)
		{
//			m_bWaitingPlayerFeedBack = false;
//			m_btTryExecuteScriptTimes = 0;
			Npc[m_nIndex].m_ActionScriptID = 0;
			m_ItemList.ClientShowMsg("-------脚本错误!-------- ");
		}
		 return TRUE;
}
catch(...)
{
	if (pScript)
	{
		nSafeIndex=pScript->SafeCallBegin();	
		pScript->SafeCallEnd(nSafeIndex);
	}
	m_ItemList.ClientShowMsg("-------脚本错误!-------- ");
//	m_bWaitingPlayerFeedBack = false;
//	m_btTryExecuteScriptTimes = 0;
	Npc[m_nIndex].m_ActionScriptID = 0;
	return TRUE;
}

	   /* ZeroMemory(szClienResult,sizeof(szClienResult));
	    KLuaScript nClentLua;
	    nClentLua.Init();                                                    //初始化 注册标准函数库
		nClentLua.RegisterFunctions(GameScriptFuns,g_GetGameScriptFunNum()); //把C脚本函数注册到脚本中

		if (nClentLua.Load(nScriptPath))
		{
			//m_ItemList.ClientShowMsg("脚本执行成功C");
			sprintf(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_ScriptPicPath,nScriptPath);
			//Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_ActionScriptID =g_FileName2Id(nScriptPath);
			int nSafeIndex = 0;
			nSafeIndex=nClentLua.SafeCallBegin();
			//执行这个脚本的函数				
			nClentLua.CallFunction(nFunName,1, "dds",nPaparm,ninVal,nstrVala);  //有返回值
			const char * szType = lua_typename(nClentLua.m_LuaState, Lua_GetTopIndex(nClentLua.m_LuaState));
			if (Lua_IsNumber(nClentLua.m_LuaState, Lua_GetTopIndex(nClentLua.m_LuaState)) == 1)
			{//Lua_PushNumber
				int nResult = (int)Lua_ValueToNumber(nClentLua.m_LuaState, Lua_GetTopIndex(nClentLua.m_LuaState));
				sprintf(szClienResult, "%d", nResult);  //int类型
			}
			else if (Lua_IsString(nClentLua.m_LuaState, Lua_GetTopIndex(nClentLua.m_LuaState)) ==1)//字符串类型
			{
				sprintf(szClienResult, (char *)Lua_ValueToString(nClentLua.m_LuaState, Lua_GetTopIndex(nClentLua.m_LuaState)));
			}
			else
				sprintf(szClienResult,"<c=Red>脚本数据错误GC,请联系GM处理!<c>");  
			    //m_ItemList.ClientShowMsg("执行脚本成功GC!");
			    nSafeIndex=nClentLua.SafeCallBegin();
			    nClentLua.SafeCallEnd(nSafeIndex);
		}
		else
		{
		  ZeroMemory(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_ScriptPicPath,sizeof(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_ScriptPicPath));
		  //Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_ActionScriptID=0;
		  m_ItemList.ClientShowMsg("执行脚本失败GC,请联系GM处理!");
		}
		//m_ItemList.ClientShowMsg("脚本执行成功");
		nClentLua.Exit();
		return atoi(szClienResult);*/ 
}

#endif


//显示栈信息的函数
void KPlayer::myPrintLuaStackA(lua_State *L)
{
    int stackTop=lua_gettop(L);//获取栈顶的索引值
    int index,t;
    printf("--栈顶(v)(%d)--\n",stackTop);
    //显示栈中的元素
    for(index=stackTop;index>0;--index)
	{  t=lua_type(L,index);
	    printf("(i:%d) %s(%s)\n",index,lua_typename(L,t),lua_tostring(L,index));	     
	}
    printf("--栈底--\n");
}
/////////////////////////////////////////////////////////////////
//GM执行脚本
BOOL	KPlayer::DoScript(char * ScriptCommand)
{

	return TRUE;
/*	if (NULL == ScriptCommand) return FALSE;
	KLuaScript *Script = new KLuaScript;
	Script->Init();
	Script->RegisterFunctions(GameScriptFuns, g_GetGameScriptFunNum());
	
	//GM Standand Script Functions 
//	Script->Load("\\script\\system\\startserver--.lua.lua");  //GM执行开始游戏脚本
	
	
	Lua_PushNumber(Script->m_LuaState, m_nPlayerIndex);
	Script->SetGlobalName(SCRIPT_PLAYERINDEX);
	Lua_PushNumber(Script->m_LuaState, m_dwID);
	Script->SetGlobalName(SCRIPT_PLAYERID);	
	Lua_PushNumber(Script->m_LuaState, Npc[m_nIndex].m_SubWorldIndex);
	Script->SetGlobalName(SCRIPT_SUBWORLDINDEX);
	
	if (Script->LoadBuffer((PBYTE)ScriptCommand, strlen(ScriptCommand))) 
	{
		BOOL bResult = Script->ExecuteCode();	
		delete Script; //Question!
         Script=NULL;
		return bResult;
	}
	delete Script; //Question!
	Script=NULL;
	return FALSE;*/
}


void	KPlayer::DoScriptAction(PLAYER_SCRIPTACTION_SYNC * pUIInfo) //要求显示某个UI界面
{
	if (!pUIInfo) return;
	
	//服务器端脚本时
	if (pUIInfo->m_bParam2 >= 1)
	{
#ifdef _SERVER
		pUIInfo->ProtocolType = (BYTE)s2c_scriptaction;
		pUIInfo->m_wProtocolLong = sizeof(PLAYER_SCRIPTACTION_SYNC) - MAX_SCIRPTACTION_BUFFERNUM + pUIInfo->m_nBufferLen - 1;
		g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)pUIInfo, pUIInfo->m_wProtocolLong + 1 );
#else
	}
	else
	{//客户端脚本要求显示脚本 直接运行
		OnScriptAction((PLAYER_SCRIPTACTION_SYNC *)pUIInfo);
#endif
	}
	
}

//服务器端获知玩家界面UI选择了某项后，处理执行脚本函数~~
void KPlayer::ProcessPlayerSelectFromUI(BYTE* pProtocol)			// 处理当玩家从选择菜单选择某项时的操作	
{
	PLAYER_SELECTUI_COMMAND * pSelUI = (PLAYER_SELECTUI_COMMAND*) pProtocol;
//	m_bWaitingPlayerFeedBack = false;
	//如果返回负数，表示退出该脚本执行环境
	if (pSelUI->nSelectIndex < 0) 
		m_nAvailableAnswerNum = 0;

	if (m_nAvailableAnswerNum > pSelUI->nSelectIndex)
	{
		if (m_szTaskAnswerFun[pSelUI->nSelectIndex][0])
		{
			//g_SetFilePath("\\script");
			if (m_nIndex)
			{//服务器端执行
			   #ifdef _SERVER 
				  ExecuteScriptB(Npc[m_nIndex].m_ActionScriptID,m_szTaskAnswerFun[pSelUI->nSelectIndex],pSelUI->nSelectIndex,m_szTaskAnswerCanshu[pSelUI->nSelectIndex]);//     
			   #else
				  
			      //m_ItemList.ClientShowMsg(Npc[m_nIndex].m_ScriptPicPath);
				  //m_ItemList.ClientShowMsg(m_szTaskAnswerFun[pSelUI->nSelectIndex]);
				  char nstrPath[256]={0};
				  Npc[m_nIndex].GetstrInfo(STR_SCRIPTTATH_CLIENT,nstrPath,sizeof(nstrPath));
				  ClientExeItemScript(nstrPath,m_szTaskAnswerFun[pSelUI->nSelectIndex],pSelUI->nSelectIndex,0,m_szTaskAnswerCanshu[pSelUI->nSelectIndex]) ;
			   	  
			   #endif
			}
			/*else
			{//客户端执行
			     Player[CLIENT_PLAYER_INDEX].ClientExeItemScript("\\Ui\\Fs_NewTaskWindow.lua","newtaskrequestinfo",0,0,"") ;
			} */
		}
	}
}
//客户端
#ifndef _SERVER
//玩家在界面交互后，选择了某项后，向服务器端发送
void	KPlayer::OnSelectFromUI(PLAYER_SELECTUI_COMMAND * pSelectUI, UIInfo eUIInfo)	//当玩家从选择框中选择某项后，将向服务器发送			
{
	if (!pSelectUI) return;
	
	switch(eUIInfo)
	{
	case UI_SELECTDIALOG:
		{
			if (g_bUISelIntelActiveWithServer)
			{//是否是服务器执行
				pSelectUI->ProtocolType = (BYTE)c2s_playerselui;
				if (g_pClient)
					g_pClient->SendPackToServer((BYTE*)pSelectUI, sizeof(PLAYER_SELECTUI_COMMAND));
			}
			else
			{//客户端执行
				ProcessPlayerSelectFromUI((BYTE *)pSelectUI);// 处理当玩家从选择菜单选择某项时的操作	
			}
		}
		break;
	case UI_TALKDIALOG:
		{
			if (g_bUISpeakActiveWithServer)
			{//是否是服务器执行
				pSelectUI->ProtocolType = (BYTE)c2s_playerselui;
				if (g_pClient)
					g_pClient->SendPackToServer((BYTE*)pSelectUI, sizeof(PLAYER_SELECTUI_COMMAND));
			}
			else
			{//客户端执行
				ProcessPlayerSelectFromUI((BYTE *)pSelectUI);			// 处理当玩家从选择菜单选择某项时的操作	
			}
			
		}break;
	}
}
#endif

//-------------------------------------------------------------------------
//	功能：获得通知某好友上线了
//-------------------------------------------------------------------------
void	KPlayer::ChatFriendOnLine(DWORD dwID, int nFriendIdx)
{
	if (nFriendIdx <= 0)
		return;
	for (int i = 0; i < MAX_FRIEND_TEAM; ++i)
	{
		if (m_cChat.m_cFriendTeam[i].m_nFriendNo == 0)
			continue;
		CChatFriend	*pFriend;
		pFriend = (CChatFriend*)m_cChat.m_cFriendTeam[i].m_cEveryOne.GetHead();
		while (pFriend)
		{
			if (pFriend->m_dwID == dwID)
			{
				pFriend->m_nPlayerIdx = nFriendIdx;
#ifdef _SERVER
				CHAT_FRIEND_ONLINE_SYNC	sFriend;
				sFriend.ProtocolType = s2c_chatfriendonline;
				sFriend.m_dwID = dwID;
				sFriend.m_nPlayerIdx = nFriendIdx;
				g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sFriend, sizeof(CHAT_FRIEND_ONLINE_SYNC));
#else
				// 通知界面有好友上线
				KUiPlayerItem	sPlayer;
				snprintf(sPlayer.Name, sizeof(sPlayer.Name), "%s", pFriend->m_szName);
				sPlayer.uId = dwID;
				sPlayer.nIndex = nFriendIdx;
				sPlayer.nData = CHAT_S_ONLINE;

				CoreDataChanged(GDCNI_CHAT_FRIEND_STATUS, (unsigned int)&sPlayer, i);
				
				KSystemMessage	sMsg;
				sprintf(sMsg.szMessage, MSG_CHAT_FRIEND_ONLINE, pFriend->m_szName);
				sMsg.eType = SMT_NORMAL;
				sMsg.byConfirmType = SMCT_NONE;
				sMsg.byPriority = 0;
				sMsg.byParamSize = 0;
				sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
#endif
				return;
			}
			pFriend = (CChatFriend*)pFriend->GetNext();
		}
	}
#ifdef _SERVER
	// 如果没找到，说明本方已经删除了对方，必须通知对方删除
	Player[nFriendIdx].m_cChat.DeleteFriendData(m_dwID, nFriendIdx);
#endif
}

// 获得当前门派名称
void	KPlayer::GetFactionName(char *lpszName, int nSize)
{
	this->m_cFaction.GetCurFactionName(lpszName);
	return;
}
int	KPlayer::GetFactionCamp()
{
	return this->m_cFaction.GetGurFactionCamp();
}

// 获得当前门派名称
int	KPlayer::GetFactionNo()
{
	return this->m_cFaction.GetCurFactionNo();
}
#ifdef _SERVER
void	KPlayer::S2CExecuteScript(char * ScriptName, char * szParam = NULL)
{
	if (!ScriptName || (!ScriptName[0])) return; 
	
	PLAYER_SCRIPTACTION_SYNC  ScriptAction;
	ScriptAction.m_nOperateType = SCRIPTACTION_EXESCRIPT;
	ScriptAction.ProtocolType = s2c_scriptaction;
	char * script = NULL;
	if (szParam == NULL || szParam[0] == 0) 
	{
		ScriptAction.m_nBufferLen = strlen(ScriptName) + 1 ;
		strcpy(ScriptAction.m_pContent, ScriptName);
	}
	else
	{
		ScriptAction.m_nBufferLen = strlen(ScriptName) + 2 + strlen(szParam);
		sprintf(ScriptAction.m_pContent, "%s|%s", ScriptName, szParam);
	}
	ScriptAction.m_wProtocolLong = sizeof(PLAYER_SCRIPTACTION_SYNC) - 300 + ScriptAction.m_nBufferLen - 1;
	g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&ScriptAction, sizeof(PLAYER_SCRIPTACTION_SYNC) - 300 + ScriptAction.m_nBufferLen);	
	
}
#endif

#ifndef _SERVER
void	KPlayer::OnScriptAction(PLAYER_SCRIPTACTION_SYNC * pMsg)
{
	PLAYER_SCRIPTACTION_SYNC * pScriptAction = (PLAYER_SCRIPTACTION_SYNC *)pMsg;

	char szString[1024]={0};
	
	switch(pScriptAction->m_nOperateType)
	{
	case SCRIPTACTION_UISHOW:
		{
			switch(pScriptAction->m_bUIId)
			{
			case UI_SELECTDIALOG://通知客户端显示选择窗口
				{
					/*if (Player[CLIENT_PLAYER_INDEX].m_nCheckWaiGua==1)
					{//开始检测外挂
						////////////////////检测外挂程序/////////////////////////
						//int nIsOpenForBit=0,;  //默认开启封挂
						int nMun=0,wgRows=g_ForbitWaiGua.GetHeight()+1;

						if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nSleepFlag == 0)  
						{   
							for (int i=2;i<wgRows;++i)
							{  
								char nKey[16],nReg=0;
								g_ForbitWaiGua.GetString(i,"GuaNameKey","无",nKey,sizeof(nKey));
								   nReg= g_ForBitGua.GetProcessList(nKey,wgRows,i);//CheckWaiGua(nKey);
								if (nReg>=1)
								{ 	
									Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("警告!<c=yellow>发现活动<c=green>辅助<c>,请关闭!");
									break;
								}
								else if (nReg<0)  //提权失败
								{
									Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("检测:<c=yellow>数据出错,请联系GM");
									break;
								}
								else  if (nReg==0)
								{
									nMun++;
								}
								g_ForBitGua.Colse();
							}
						}

						if (nMun>=wgRows-2)
						{  							
							Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("检测:<c=yellow>恭喜!您通过了<c=green>安全<c>检测!");
						}
						else
						  break;	
					} 
					*/
					KUiQuestionAndAnswer	*pQuest = NULL;
					KUiNpcSpr                *pImage = NULL;
					if (pScriptAction->m_nBufferLen <= 0) break;
					
					if (pScriptAction->m_bOptionNum <= 0)
						pQuest = (KUiQuestionAndAnswer *)malloc(sizeof(KUiQuestionAndAnswer));
					else
						pQuest = (KUiQuestionAndAnswer *)malloc(sizeof(KUiQuestionAndAnswer) + sizeof(KUiAnswer) * (pScriptAction->m_bOptionNum - 1));
					
					if (pQuest==NULL)
					{
						//Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("内存分配失败A"); 
						break;
					}

					
					pImage = (KUiNpcSpr *)malloc(sizeof(KUiNpcSpr));

					if (pImage==NULL)
					{
						//Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("内存分配失败B"); 
						break;
					}
					//Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("<c=gyellow>正常！<c>");
					//break;

					//char strContent[1024];
					char *strContent=NULL;
					      strContent=(char *)malloc(1024);
				    if (strContent==NULL)
					{
					    break;
					}		 
					char * pAnswer = NULL;
					pQuest->AnswerCount = 0;

					//Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg( pScriptAction->m_pContent);

					//主信息为字符串
					if (pScriptAction->m_bParam1 == 0)
					{
						g_StrCpyLen(strContent, pScriptAction->m_pContent, pScriptAction->m_nBufferLen + 1); //公告内容
						
						pAnswer = strstr(strContent, "|");   //获取函数名 取后面的值

						if (!pAnswer)
						{
							pScriptAction->m_bOptionNum = 0;
							pQuest->AnswerCount = 0;
						}
						else
						{
							*pAnswer++ = 0; //过滤 字符串 “|”后面的内容  ---截取出选项内容
						}
						
						g_StrCpyLen(pQuest->Question, strContent, sizeof(pQuest->Question));  //公告内容
						pQuest->QuestionLen = TEncodeText(pQuest->Question, strlen(pQuest->Question));
					}
					//主信息为数字标识
					else 
					{
						g_StrCpyLen(pQuest->Question, g_GetStringRes(*(int *)pScriptAction->m_pContent, szString, sizeof(szString)), sizeof(pQuest->Question));
						pQuest->QuestionLen = TEncodeText(pQuest->Question, strlen(pQuest->Question));
						
						g_StrCpyLen(strContent, pScriptAction->m_pContent + sizeof(int), pScriptAction->m_nBufferLen - sizeof(int) + 1);
						pAnswer = strContent + 1;
					}
					// 以下为选项的设置
					//Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(strContent);

					for (int i = 0; i < pScriptAction->m_bOptionNum; ++i)   //选项的个数
					{
						char * pNewAnswer = strstr(pAnswer, "|");   //找到有这个符号的位置
                               //pNewAnswer =strstr(pNewAnswer, "/");

						if (pNewAnswer)   //函数名
						{
							*pNewAnswer = 0;
							strcpy(pQuest->Answer[i].AnswerText, pAnswer);  // 函数名

							//Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(pQuest->Answer[i].AnswerText);

							pQuest->Answer[i].AnswerLen = -1;
                            pAnswer = pNewAnswer+1;
						}
						else
						{
							strcpy(pQuest->Answer[i].AnswerText, pAnswer);
							//Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(pQuest->Answer[i].AnswerText);
							pQuest->Answer[i].AnswerLen = -1;
                            pQuest->AnswerCount = i + 1;	
		     				break;
						}
					}//end for

					if (pScriptAction->m_bParam2>=1)
					   g_bUISelIntelActiveWithServer =TRUE;

					//g_bUISelIntelActiveWithServer = pScriptAction->m_bParam2;
					g_bUISelLastSelCount = pQuest->AnswerCount;
                    /*******************************************Code by thienthanden2*******************************************************/ 
                    char szBuffer[128]={0}; 
					if  (pScriptAction->m_szSprPath[0] && strstr(pScriptAction->m_szSprPath,".spr"))
					{
						strcpy(pImage->ImageFile, pScriptAction->m_szSprPath);

						pImage->MaxFrame = 1;
						goto Next;
					}
					else if (!pScriptAction->m_szSprPath[0] && m_nImageNpcID)  //spr 设置 NPC索引
                    { 

                      for (int i = 0; i < 16; ++i) 
					  { Npc[m_nImageNpcID].GetNpcRes()->m_pcResNode->GetFileName(i, 3, 0, "", szBuffer, sizeof(szBuffer)); 
                        if (szBuffer[0]) 
                            { 
                            strcpy(pImage->ImageFile, szBuffer); 
                            pImage->MaxFrame = (Npc[m_nImageNpcID].GetNpcRes()->m_pcResNode->GetTotalFrames(i, 3, 0, 16))/ 
                            (Npc[m_nImageNpcID].GetNpcRes()->m_pcResNode->GetTotalDirs(i, 3, 0, 16)); goto Next; 
                            } 
					  } 
                      for (int j = 0; j < 16; ++j) 
					  {  
                         Npc[m_nImageNpcID].GetNpcRes()->m_pcResNode->GetFileName(j, 0, 0, "", szBuffer, sizeof(szBuffer)); 
                        if (szBuffer[0]) 
                            { 
                              strcpy(pImage->ImageFile, szBuffer); 
                              pImage->MaxFrame = (Npc[m_nImageNpcID].GetNpcRes()->m_pcResNode->GetTotalFrames(j, 0, 0, 16))/ 
                              (Npc[m_nImageNpcID].GetNpcRes()->m_pcResNode->GetTotalDirs(j, 0, 0, 16)); goto Next; 
                            } 
					  } 
                     
                    } 
                    Next: 
                    if (pScriptAction->m_Select == 1 /*&& m_nImageNpcID*/) 
                          CoreDataChanged(GDCNI_QUESTION_CHOOSE,(unsigned int) pQuest,(int)pImage,pScriptAction->m_bParam2); 
                    else
						  CoreDataChanged(GDCNI_QUESTION_CHOOSE,(unsigned int) pQuest, 0); 

                    free(pImage); 
                    pImage = NULL; 
					
                    free(pQuest); 
                    pQuest = NULL;
					
					free(strContent); 
                    strContent = NULL;
                } 
                break;  
				
			case UI_TALKDIALOG: 
				{//talk
					BOOL bUsingSpeakId = pScriptAction->m_bParam1;
					int	 nSentenceCount = pScriptAction->m_bOptionNum;
					if (nSentenceCount <= 0) return ;
					KUiInformationParam *pSpeakList  = new KUiInformationParam[nSentenceCount];
					memset(pSpeakList, 0, sizeof(KUiInformationParam) * nSentenceCount);
					
					char * pAnswer = new char [pScriptAction->m_nBufferLen + 1];

					if (pAnswer==NULL)
					{
						//Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("内存分配失败A"); 
						break;
					}

					char * pBackupAnswer = pAnswer;
					g_StrCpyLen(pAnswer, pScriptAction->m_pContent, pScriptAction->m_nBufferLen + 1);
					
					int nCount = 0;
					for (int i = 0; i < pScriptAction->m_bOptionNum; ++i)
					{
						char * pNewAnswer = strstr(pAnswer, "|");
						
						if (pNewAnswer)
						{
							*pNewAnswer = 0;
							if (!bUsingSpeakId)
							{
								strcpy(pSpeakList[i].sInformation, pAnswer);
								
							}
							else
							{
								strcpy(pSpeakList[i].sInformation, g_GetStringRes(atoi(pAnswer), szString, sizeof(szString)));
							}
							
							
							if (i < pScriptAction->m_bOptionNum - 1)
								strcpy(pSpeakList[i].sConfirmText, "Kplayer so 1");
							else 
							{
								strcpy(pSpeakList[i].sConfirmText, "Kplayer so 2");
								if (pScriptAction->m_nParam == 1)						
									pSpeakList[i].bNeedConfirmNotify = TRUE;
								
							}
							pSpeakList[i].nInforLen = TEncodeText(pSpeakList[i].sInformation, strlen(pSpeakList[i].sInformation));
							pAnswer = pNewAnswer + 1;
						}
						else
						{
							if (!bUsingSpeakId)
							{
								strcpy(pSpeakList[i].sInformation, pAnswer);
							}
							else
							{
								strcpy(pSpeakList[i].sInformation, g_GetStringRes(atoi(pAnswer), szString ,sizeof(szString)));
							}
							
							strcpy(pSpeakList[i].sConfirmText, "Kplayer so 3");
							
							if (pScriptAction->m_nParam == 1)						
								pSpeakList[i].bNeedConfirmNotify = TRUE;
							
							pSpeakList[i].nInforLen = TEncodeText(pSpeakList[i].sInformation, strlen(pSpeakList[i].sInformation));
							nCount++;
							break;
						}
						nCount ++;
					} //END FOR

					if (pBackupAnswer)
					{
						delete []pBackupAnswer;
						pBackupAnswer = NULL;
					}

					if (pScriptAction->m_bParam2>=1)
					   g_bUISpeakActiveWithServer =TRUE;

					//g_bUISpeakActiveWithServer = pScriptAction->m_bParam2;
					//客户端调出窗口
					CoreDataChanged(GDCNI_SPEAK_WORDS,(unsigned int)pSpeakList, nCount);

					if (pSpeakList)
					{
						delete pSpeakList;	
						pSpeakList = NULL;
					}
				}
				break;
			case UI_NOTEINFO:   //任务记录窗口
				{
					if (pScriptAction->m_nBufferLen <= 0) 
						break;
					KMissionRecord Record;
					//主信息为字符串
					if (pScriptAction->m_bParam1 == 0)
					{
						g_StrCpyLen(Record.sContent, pScriptAction->m_pContent,  pScriptAction->m_nBufferLen - sizeof(int) - 1);
						Record.uValue = *(unsigned int*) (pScriptAction->m_pContent + pScriptAction->m_nBufferLen - sizeof(int));
					}
					else
					{
						g_GetStringRes(*(int *)pScriptAction->m_pContent, Record.sContent ,sizeof(Record.sContent));
						Record.uValue = *(unsigned int* ) (Record.sContent + sizeof(int));
					}
					
					
					Record.nContentLen = TEncodeText(Record.sContent, strlen(Record.sContent));
					
					CoreDataChanged(GDCNI_MISSION_RECORD, (unsigned long)&Record, 0);
					
				}
				break;
			case UI_MSGINFO:  //左向右的跳出信息 宋金，攻城等
				{
					if (pScriptAction->m_nBufferLen <= 0) 
						break;
					
					char strContent[1024];
					//主信息为字符串
					if (pScriptAction->m_bParam1 == 0)
					{
						g_StrCpyLen(strContent, pScriptAction->m_pContent,  pScriptAction->m_nBufferLen + 1);
					}
					else
					{
						g_GetStringRes(*(int *)pScriptAction->m_pContent, strContent ,sizeof(strContent));
					}
					
					KSystemMessage	sMsg;
					sMsg.eType = SMT_PLAYER;
					sMsg.byConfirmType = SMCT_MSG_BOX;
					sMsg.byPriority = 0;
					sMsg.byParamSize = 0;
					g_StrCpyLen(sMsg.szMessage, strContent, sizeof(sMsg.szMessage));
					sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage)); //后来加的
					CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
					//CoreDataChanged(GDCNI_MISSION_RECORD, (unsigned int)strContent, strlen(strContent));
					
				}break;
				
			case UI_NEWSINFO:  //滚动公告消息
				{
					if (pScriptAction->m_nBufferLen <= 0) 
						break;
					switch(pScriptAction->m_bOptionNum) //OPtionNum代表是哪一种News类型
					{
					case NEWSMESSAGE_NORMAL:  //显示就消亡
						{
							
							KNewsMessage News;
							News.nType = NEWSMESSAGE_NORMAL;
							
							char strContent[1024];
							//主信息为字符串
							if (pScriptAction->m_bParam1 == 0)
							{
								g_StrCpyLen(strContent, pScriptAction->m_pContent,  pScriptAction->m_nBufferLen + 1);
							}
							else
							{
								g_GetStringRes(*(int *)pScriptAction->m_pContent, strContent ,sizeof(strContent));
							}
							
							g_StrCpyLen(News.sMsg,  strContent, sizeof(News.sMsg));

							News.nMsgLen = TEncodeText(News.sMsg, strlen(News.sMsg));
							CoreDataChanged(GDCNI_NEWS_MESSAGE, (unsigned int)&News, 0);
						}
						break;
					case NEWSMESSAGE_SHUIJI:  //系统随机 显示就消亡
						{							
							KNewsMessage News;
							News.nType = NEWSMESSAGE_SHUIJI;
							
							char strContent[512];
							//主信息为字符串
							if (pScriptAction->m_bParam1 == 0)
							{//服务器端数字 信息id
								g_StrCpyLen(strContent, pScriptAction->m_pContent,  pScriptAction->m_nBufferLen + 1);
							}
							else
							{//字符串
								g_GetStringRes(*(int *)pScriptAction->m_pContent, strContent ,sizeof(strContent));
							}
							
							g_StrCpyLen(News.sMsg,strContent, sizeof(News.sMsg));
							News.nMsgLen = TEncodeText(News.sMsg, strlen(News.sMsg));
							CoreDataChanged(GDCNI_SHUIJI_MESSAGE, (unsigned int)&News, 0);
						}
						break;
						//STRING|STRINGID + TIME(INT)
					case NEWSMESSAGE_COUNTING:  //计时消息，时间到就消亡
						{
							KNewsMessage News;
							News.nType = pScriptAction->m_bOptionNum;
							
							char strContent[1024];
							int nTime = 0;
							//主信息为字符串
							if (pScriptAction->m_bParam1 == 0)
							{
								g_StrCpyLen(strContent, pScriptAction->m_pContent,  pScriptAction->m_nBufferLen - sizeof(int) + 1);
							}
							else
							{
								g_GetStringRes(*(int *)pScriptAction->m_pContent, strContent ,sizeof(strContent));
							}
							
							g_StrCpyLen(News.sMsg,  strContent, sizeof(News.sMsg));
							News.nMsgLen = TEncodeText(News.sMsg, strlen(News.sMsg));
							SYSTEMTIME systime;
							
							memset(&systime, 0, sizeof(SYSTEMTIME));
							systime.wSecond = *(int *)((pScriptAction->m_pContent + pScriptAction->m_nBufferLen - sizeof(int)));
							
							CoreDataChanged(GDCNI_NEWS_MESSAGE, (unsigned int)&News, (unsigned int)&systime);
							
						}break;

					case NEWSMESSAGE_TIMEEND:  //定时消息
						{
							KNewsMessage News;
							News.nType = pScriptAction->m_bOptionNum;
							
							char strContent[1024];
							int nTime = 0;
							//主信息为字符串
							if (pScriptAction->m_bParam1 == 0)
							{
								g_StrCpyLen(strContent, pScriptAction->m_pContent,  pScriptAction->m_nBufferLen - sizeof(SYSTEMTIME) + 1);
							}
							else
							{
								g_GetStringRes(*(int *)pScriptAction->m_pContent, strContent ,sizeof(strContent));
							}
							
							g_StrCpyLen(News.sMsg,  strContent, sizeof(News.sMsg));
							News.nMsgLen = TEncodeText(News.sMsg, strlen(News.sMsg));
							SYSTEMTIME systime;
							systime = *(SYSTEMTIME*)((pScriptAction->m_pContent + pScriptAction->m_nBufferLen - sizeof(SYSTEMTIME)));
							CoreDataChanged(GDCNI_NEWS_MESSAGE, (unsigned int)&News, (unsigned int)&systime);
							
						}break;
					
					}
					
				}break;
			case UI_PLAYMUSIC:
				{
					char szMusicFile[MAX_PATH];
					memcpy(szMusicFile, pScriptAction->m_pContent, pScriptAction->m_nBufferLen);
					szMusicFile[pScriptAction->m_nBufferLen] = 0;
					g_SubWorldSet.m_cMusic.ScriptPlay(szMusicFile);
				}break;
			
			case UI_OPENTONGUI:		  //打开建帮UI		
				{
					Player[CLIENT_PLAYER_INDEX].m_cTong.OpenCreateInterface();
					break;
				}
			}
	} break;
	case SCRIPTACTION_EXESCRIPT://要求客户端调用某个脚本
		{
			if (pScriptAction->m_nBufferLen <= 0 ) break;
			char szScriptInfo[1024];
			g_StrCpyLen(szScriptInfo, pScriptAction->m_pContent,pScriptAction->m_nBufferLen + 1);
			char * pDivPos = strstr(szScriptInfo, "/");  // 斜杠后面的内容 则是脚本参数名
			if (pDivPos)	
				*pDivPos++ = 0; 
			if (pDivPos)
				ExecuteScriptC(szScriptInfo, "OnCall", pDivPos,"");
			else
				ExecuteScriptC(szScriptInfo, "OnCall", "","");
		}
		break;
	}
	
	
}
#endif

void  KPlayer::ReSkillEnhance()
{
	int nActiveSkillID;
	
#ifdef _SERVER
	nActiveSkillID=m_nLeftSkillID;
#else
	nActiveSkillID=Player[CLIENT_PLAYER_INDEX].GetLeftSkill();	
#endif
	
	if (Npc[m_nIndex].m_Currentskillenhance!= 0)
	{   
		int nListIndex = Npc[m_nIndex].m_SkillList.FindSame(nActiveSkillID);
		if (nListIndex)                 //查找激活的当前攻击技能ID
		{
			if (Npc[m_nIndex].m_SkillList.GetCurrentLevelByIdx(nListIndex) > 0)  //等级大于0的才 增加
			{
				Npc[m_nIndex].m_SkillList.AddEnChance(nListIndex,Npc[m_nIndex].m_Currentskillenhance);
			}			
		}
	}
	
//#ifdef _SERVER
	//char msg[64];
	//sprintf(msg,"PS攻击加成:%d",Npc[m_nIndex].m_Currentskillenhance);
	//m_ItemList.msgshow(msg);
//#else
	//char msg[64];
	//sprintf(msg,"PC攻击加成:%d",Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Currentskillenhance);
	//Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(msg);	
//#endif

}

#ifdef _SERVER

void  KPlayer::SetCurLeftSkillID(int nSillID)
{
	Npc[m_nIndex].ReSkillEnhance(m_nLeftSkillID,0); //减去上一次技能的加成

    m_nLeftSkillID    = nSillID;
	m_nLeftListidx    = Npc[m_nIndex].m_SkillList.FindSame(m_nLeftSkillID);       //左键攻击技能skill[m_nLeftListidx]索引
	m_nLeftSkillLevel = Npc[m_nIndex].m_SkillList.GetCurrentLevelByIdx(m_nLeftListidx);//当前的等级

	Npc[m_nIndex].ReSkillEnhance(nSillID,1);        //设置当前攻击技能加成
}

/*int KPlayer::AddTempTaskValue(void* pData)
{
	//memcpy(m_cTask.nClear, pData, sizeof(int) * MAX_TEMP_TASK);
//	memcpy(m_cTask.m_TaskTemplate, pData, sizeof(int) * MAX_TEMP_TASK);
	return 1;
} */

//客户端请求与某个Npc对话  对话NPC执行脚本
//服务器版本
void	KPlayer::DialogNpc(BYTE * pProtocol)
{
	//if (m_nAvailableAnswerNum > 0) return ;
	PLAYER_DIALOG_NPC_COMMAND * pDialogNpc = (PLAYER_DIALOG_NPC_COMMAND*) pProtocol;
	int nIdx = 0;
	if (pDialogNpc->nNpcId < 0) return;
	   nIdx = FindAroundNpc(pDialogNpc->nNpcId); //NpcSet.SearchID(pDialogNpc->nNpcId);  //
	if (nIdx > 0) 
	{
		m_LastDialogNpc = nIdx;
		int distance = NpcSet.GetDistance(nIdx, m_nIndex);
		// 小于对话半径就开始对话
		if ((Npc[nIdx].m_Kind == kind_dialoger) || (NpcSet.GetRelation(m_nIndex, nIdx) == relation_none))
		{
			if (distance <= Npc[nIdx].m_DialogRadius * 2)	// 放大server对话半径
			{
				char nMgs[256]={0};
				//if (Npc[nIdx].GetstrInfo(STR_ACTION_SCRIPT,nMgs))
				//if (Npc[nIdx].m_ActionScriptID)
			//	{
					char nNpcSetings[8];
					snprintf(nNpcSetings, sizeof(nNpcSetings),"%d",Npc[nIdx].m_NpcSettingIdx);
					ExecuteScriptD(Npc[nIdx].m_ActionScriptID, "main",Npc[nIdx].Name,nNpcSetings,nIdx,Npc[nIdx].m_dwID,m_nIndex,0,0,TRUE);
			//	}
			}
		}
	}
}
#endif

#ifndef _SERVER
void	KPlayer::s2cTradeChangeState(BYTE* pMsg)
{
	TRADE_CHANGE_STATE_SYNC	*pTrade = (TRADE_CHANGE_STATE_SYNC*)pMsg;
	
	
	switch (m_cMenuState.m_nState)
	{
	case PLAYER_MENU_STATE_NORMAL:		// 当前客户端 NORMAL
	case PLAYER_MENU_STATE_TRADEOPEN:	// 当前客户端 TRADEOPEN
		if (pTrade->m_btState == 0)
		{
			m_cMenuState.SetState(PLAYER_MENU_STATE_NORMAL);
			CoreDataChanged(GDCNI_TRADE_OPER_DATA, 0, 0);
		}
		else if (pTrade->m_btState == 1)
		{
			m_cMenuState.SetState(PLAYER_MENU_STATE_TRADEOPEN);
			CoreDataChanged(GDCNI_TRADE_OPER_DATA, 0, 0);
		}
		else if (pTrade->m_btState == 2)
		{
			m_cMenuState.SetState(PLAYER_MENU_STATE_TRADING);
			m_cTrade.StartTrade(pTrade->m_dwNpcID);
			m_ItemList.StartTrade();
			
			// 通知界面进入交易界面
			KUiPlayerItem	sTradePlayer;
			int	nIdx;
			
			nIdx = NpcSet.SearchID(pTrade->m_dwNpcID);
			if (nIdx > 0)
				snprintf(sTradePlayer.Name, sizeof(sTradePlayer.Name), "%s", Npc[nIdx].Name);
			else
				sTradePlayer.Name[0] = 0;
			sTradePlayer.nIndex = 0;
			sTradePlayer.uId = 0;
			sTradePlayer.nData = 0;
			CoreDataChanged(GDCNI_TRADE_START, (unsigned int)(&sTradePlayer), 0);
			
			if (Npc[m_nIndex].m_Doing == do_sit)
			{
				Npc[m_nIndex].SendSerCommand(do_stand);
			}
		}
		break;
	case PLAYER_MENU_STATE_TEAMOPEN:	// 当前客户端 TEAMOPEN
		g_Team[0].SetTeamClose();
		if (pTrade->m_btState == 0)
		{
			m_cMenuState.SetState(PLAYER_MENU_STATE_NORMAL);
			CoreDataChanged(GDCNI_TRADE_OPER_DATA, 0, 0);
		}
		else if (pTrade->m_btState == 1)
		{
			m_cMenuState.SetState(PLAYER_MENU_STATE_TRADEOPEN);
			CoreDataChanged(GDCNI_TRADE_OPER_DATA, 0, 0);
		}
		else if (pTrade->m_btState == 2)
		{
			m_cMenuState.SetState(PLAYER_MENU_STATE_TRADING);
			m_cTrade.StartTrade(pTrade->m_dwNpcID);
			m_ItemList.StartTrade();
			
			// 通知界面进入交易界面
			KUiPlayerItem	sTradePlayer;
			int	nIdx;
			
			nIdx = NpcSet.SearchID(pTrade->m_dwNpcID);
			if (nIdx > 0)
				snprintf(sTradePlayer.Name, sizeof(sTradePlayer.Name), "%s", Npc[nIdx].Name);
			else
				sTradePlayer.Name[0] = 0;
			sTradePlayer.Name[0] = 0;
			sTradePlayer.nIndex = 0;
			sTradePlayer.uId = 0;
			sTradePlayer.nData = 0;
			CoreDataChanged(GDCNI_TRADE_START, (unsigned int)(&sTradePlayer), 0);
			
			if (Npc[m_nIndex].m_Doing == do_sit)
			{
				Npc[m_nIndex].SendSerCommand(do_stand);
			}
		}
		break;
	case PLAYER_MENU_STATE_TRADING:		// 当前客户端 TRADING
		// 当客户端处于 TRADING 时，不应该收到此协议
		if (pTrade->m_btState == 0)
		{
			m_cMenuState.SetState(PLAYER_MENU_STATE_NORMAL);
			CoreDataChanged(GDCNI_TRADE_OPER_DATA, 0, 0);
		}
		else if (pTrade->m_btState == 1)
		{
			m_cMenuState.SetState(PLAYER_MENU_STATE_TRADEOPEN);
			CoreDataChanged(GDCNI_TRADE_OPER_DATA, 0, 0);
		}
		else if (pTrade->m_btState == 2)	// 客户端此时不应该在这个状态，客户端已出错
		{
			m_cMenuState.m_nState = PLAYER_MENU_STATE_TRADING;
			m_cMenuState.m_nTradeDest = pTrade->m_dwNpcID;
			m_cMenuState.m_nTradeState = 0;
			m_cMenuState.m_nTradeDestState = 0;
			this->m_ItemList.RecoverTrade();
			this->m_ItemList.BackupTrade();
			this->m_ItemList.ClearRoom(room_trade);
			this->m_ItemList.ClearRoom(room_trade1);
			
			// 通知界面进入交易界面
			KUiPlayerItem	sTradePlayer;
			int	nIdx;
			
			nIdx = NpcSet.SearchID(pTrade->m_dwNpcID);
			if (nIdx > 0)
				snprintf(sTradePlayer.Name, sizeof(sTradePlayer.Name), "%s", Npc[nIdx].Name);
			else
				sTradePlayer.Name[0] = 0;
			sTradePlayer.Name[0] = 0;
			sTradePlayer.nIndex = 0;
			sTradePlayer.uId = 0;
			sTradePlayer.nData = 0;
			CoreDataChanged(GDCNI_TRADE_START, (unsigned int)(&sTradePlayer), 0);
			
			if (Npc[m_nIndex].m_Doing == do_sit)
			{
				Npc[m_nIndex].SendSerCommand(do_stand);
			}
		}
		break;
	}
}
#endif
//等级提升
#ifndef _SERVER
void	KPlayer::s2cLevelUp(BYTE* pMsg)
{
	PLAYER_LEVEL_UP_SYNC	*pLevel = (PLAYER_LEVEL_UP_SYNC*)pMsg;

	if ((WORD)Npc[m_nIndex].m_Level < pLevel->m_btLevel && m_nExp < PlayerSet.m_cLevelAdd.GetLevelExp(Npc[m_nIndex].m_Level))
	{
		KSystemMessage	sMsg;
		sprintf(sMsg.szMessage, MSG_GET_EXP, PlayerSet.m_cLevelAdd.GetLevelExp(Npc[m_nIndex].m_Level) - m_nExp);
		sMsg.eType = SMT_NORMAL;
		sMsg.byConfirmType = SMCT_NONE;
		sMsg.byPriority = 0;
		sMsg.byParamSize = 0;
		sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
	}
	
	m_nExp = 0;
	Npc[m_nIndex].m_Level = (WORD)pLevel->m_btLevel;
	m_nNextLevelExp = PlayerSet.m_cLevelAdd.GetLevelExp(Npc[m_nIndex].m_Level);
	
	// -------------------------- 等级增加时各项属性同步 ----------------------
	// 同步未分配属性点
	if (pLevel->m_nAttributePoint > m_nAttributePoint)
	{
		KSystemMessage Msg;
		sprintf(Msg.szMessage, MSG_GET_ATTRIBUTE_POINT, pLevel->m_nAttributePoint - m_nAttributePoint);
		Msg.eType = SMT_PLAYER;
		Msg.byConfirmType = SMCT_UI_ATTRIBUTE;
		Msg.byPriority = 3;
		Msg.byParamSize = 0;
		Msg.nMsgLen = TEncodeText(Msg.szMessage, strlen(Msg.szMessage));
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&Msg, NULL);
	}
	m_nAttributePoint = pLevel->m_nAttributePoint;
	
	// 同步未分配技能点
	if (m_nSkillPoint < pLevel->m_nSkillPoint)
	{
		KSystemMessage Msg;
		sprintf(Msg.szMessage, MSG_GET_SKILL_POINT, pLevel->m_nSkillPoint - m_nSkillPoint);
		Msg.eType = SMT_PLAYER;
		Msg.byConfirmType = SMCT_UI_SKILLS;
		Msg.byPriority = 3;
		Msg.byParamSize = 0;
		Msg.nMsgLen = TEncodeText(Msg.szMessage, strlen(Msg.szMessage));
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&Msg, NULL);
	}
	m_nSkillPoint = pLevel->m_nSkillPoint;
	
	// 处理生命、体力、内力值（影响基本生命、体力、内力最大值的因素：等级、属性点，影响当前生命最大值的因素：装备、技能、药物（临时））
	Npc[m_nIndex].m_LifeMax           = pLevel->m_nBaseLifeMax;
	Npc[m_nIndex].m_StaminaMax        = pLevel->m_nBaseStaminaMax;
	Npc[m_nIndex].m_ManaMax           = pLevel->m_nBaseManaMax;
	//	Npc[m_nIndex].ResetLifeReplenish();
	Npc[m_nIndex].m_CurrentLifeMax    = Npc[m_nIndex].m_LifeMax;
	Npc[m_nIndex].m_CurrentStaminaMax = Npc[m_nIndex].m_StaminaMax;
	Npc[m_nIndex].m_CurrentManaMax    = Npc[m_nIndex].m_ManaMax;
	Npc[m_nIndex].m_CurrentNuQiMax    = Npc[m_nIndex].m_NuqiMax;  //怒气
	
	// 处理各种抗性的变化 火、冰、毒、电、物理
	Npc[m_nIndex].m_FireResist				= PlayerSet.m_cLevelAdd.GetFireResist(Npc[m_nIndex].m_Series, Npc[m_nIndex].m_Level);
	Npc[m_nIndex].m_CurrentFireResist		= Npc[m_nIndex].m_FireResist;
	Npc[m_nIndex].m_ColdResist				= PlayerSet.m_cLevelAdd.GetColdResist(Npc[m_nIndex].m_Series, Npc[m_nIndex].m_Level);
	Npc[m_nIndex].m_CurrentColdResist		= Npc[m_nIndex].m_ColdResist;
	Npc[m_nIndex].m_PoisonResist			= PlayerSet.m_cLevelAdd.GetPoisonResist(Npc[m_nIndex].m_Series, Npc[m_nIndex].m_Level);
	Npc[m_nIndex].m_CurrentPoisonResist		= Npc[m_nIndex].m_PoisonResist;
	Npc[m_nIndex].m_LightResist				= PlayerSet.m_cLevelAdd.GetLightResist(Npc[m_nIndex].m_Series, Npc[m_nIndex].m_Level);
	Npc[m_nIndex].m_CurrentLightResist		= Npc[m_nIndex].m_LightResist;
	Npc[m_nIndex].m_PhysicsResist			= PlayerSet.m_cLevelAdd.GetPhysicsResist(Npc[m_nIndex].m_Series, Npc[m_nIndex].m_Level);
	Npc[m_nIndex].m_CurrentPhysicsResist	= Npc[m_nIndex].m_PhysicsResist;
	Npc[m_nIndex].m_FireResistMax			= BASE_FIRE_RESIST_MAX;
	Npc[m_nIndex].m_ColdResistMax			= BASE_COLD_RESIST_MAX;
	Npc[m_nIndex].m_PoisonResistMax			= BASE_POISON_RESIST_MAX;
	Npc[m_nIndex].m_LightResistMax			= BASE_LIGHT_RESIST_MAX;
	Npc[m_nIndex].m_PhysicsResistMax		= BASE_PHYSICS_RESIST_MAX;
	/*
	Npc[m_nIndex].m_CurrentFireResistMax	= Npc[m_nIndex].m_FireResistMax;
	Npc[m_nIndex].m_CurrentColdResistMax	= Npc[m_nIndex].m_ColdResistMax;
	Npc[m_nIndex].m_CurrentPoisonResistMax	= Npc[m_nIndex].m_PoisonResistMax;
	Npc[m_nIndex].m_CurrentLightResistMax	= Npc[m_nIndex].m_LightResistMax;
	Npc[m_nIndex].m_CurrentPhysicsResistMax	= Npc[m_nIndex].m_PhysicsResistMax;
*/	
	int nOldCurCamp = Npc[m_nIndex].m_CurrentCamp;
	// 根据装备、技能信息更新当前数据
	this->UpdataCurData();
	SetNpcPhysicsDamage();
	Npc[m_nIndex].m_CurrentCamp    = nOldCurCamp;
	
	// 生命、体力、内力值加满
	Npc[m_nIndex].m_CurrentLife    = Npc[m_nIndex].m_CurrentLifeMax;
	Npc[m_nIndex].m_CurrentStamina = Npc[m_nIndex].m_CurrentStaminaMax;
	Npc[m_nIndex].m_CurrentMana    = Npc[m_nIndex].m_CurrentManaMax;
	
	// 客户端队伍中自己的等级加一
	if (this->m_cTeam.m_nFlag)
	{
		if (m_cTeam.m_nFigure == TEAM_CAPTAIN)
		{//队长
			g_Team[0].m_nMemLevel[0] = Npc[m_nIndex].m_Level;
		}
		else
		{
			for (int i = 0; i < MAX_TEAM_MEMBER; ++i)
			{
				if ((DWORD)g_Team[0].m_nMember[i] == Npc[m_nIndex].m_dwID)
				{
					g_Team[0].m_nMemLevel[i] = Npc[m_nIndex].m_Level;
					break;
				}
			}
		}
	}
	
	KSystemMessage Msg;
				
	Msg.byConfirmType = SMCT_CLICK;
	Msg.byParamSize = 0;
	Msg.byPriority = 1;
	Msg.eType = SMT_PLAYER;
	sprintf(Msg.szMessage, MSG_LEVEL_UP, Npc[m_nIndex].m_Level);
	Msg.nMsgLen = TEncodeText(Msg.szMessage, strlen(Msg.szMessage));
	CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&Msg, NULL);
	
	Npc[this->m_nIndex].SetInstantSpr(enumINSTANT_STATE_LEVELUP);  //释放升级瞬间特效
}
#endif

#ifndef _SERVER
void	KPlayer::s2cGetCurAttribute(BYTE* pMsg)
{
	PLAYER_ATTRIBUTE_SYNC	*pAttribute = (PLAYER_ATTRIBUTE_SYNC*)pMsg;
	m_nAttributePoint = pAttribute->m_nLeavePoint;
	int nData;
	switch (pAttribute->m_btAttribute)
	{
	case ATTRIBUTE_STRENGTH://力量
		nData = pAttribute->m_nBasePoint - m_nStrength;
		m_nStrength = pAttribute->m_nBasePoint;
		m_nCurStrength = pAttribute->m_nCurPoint;
		UpdataCurData();
		SetNpcPhysicsDamage();
		break;
	case ATTRIBUTE_DEXTERITY://身法
		nData = pAttribute->m_nBasePoint - m_nDexterity;
		m_nDexterity = pAttribute->m_nBasePoint;
		SetNpcAttackRating();
		SetNpcDefence();
		UpdataCurData();
		SetNpcPhysicsDamage();
		break;
	case ATTRIBUTE_VITALITY://外功
		nData = pAttribute->m_nBasePoint - m_nVitality;
		m_nVitality = pAttribute->m_nBasePoint;
		Npc[m_nIndex].AddBaseLifeMax(PlayerSet.m_cLevelAdd.GetLifePerVitality(Npc[m_nIndex].m_Series) * nData);
		Npc[m_nIndex].AddBaseStaminaMax(PlayerSet.m_cLevelAdd.GetStaminaPerVitality(Npc[m_nIndex].m_Series) * nData);
		UpdataCurData();
		break;
	case ATTRIBUTE_ENGERGY: //内功
		nData = pAttribute->m_nBasePoint - m_nEngergy;
		m_nEngergy = pAttribute->m_nBasePoint;
		Npc[m_nIndex].AddBaseManaMax(PlayerSet.m_cLevelAdd.GetManaPerEnergy(Npc[m_nIndex].m_Series) * nData);
		UpdataCurData();
		SetNpcPhysicsDamage();
		break;
	}
	//CoreDataChanged(GDCNI_PLAYER_RT_ATTRIBUTE, 0, 0);
	//CoreDataChanged(GDCNI_PLAYER_NEW_ATTRIBUTE, 0, 0);
}
#endif

#ifndef _SERVER
void	KPlayer::s2cSetExp(int nExp)
{
	if (nExp > m_nExp)
	{
		KSystemMessage	sMsg;
		sprintf(sMsg.szMessage, MSG_GET_EXP, nExp - m_nExp);
		sMsg.eType = SMT_NORMAL;
		sMsg.byConfirmType = SMCT_NONE;
		sMsg.byPriority = 0;
		sMsg.byParamSize = 0;
		sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
	}
	else if (nExp < m_nExp)
	{
		KSystemMessage	sMsg;
		sprintf(sMsg.szMessage, "提示:您损失了(%d)点经验值。", m_nExp - nExp);
		sMsg.eType = SMT_NORMAL;
		sMsg.byConfirmType = SMCT_NONE;
		sMsg.byPriority = 0;
		sMsg.byParamSize = 0;
		sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
	}
	
	this->m_nExp = nExp; //经验赋值
}
#endif

#ifndef _SERVER
void	KPlayer::s2cSyncMoney(BYTE* pMsg)
{
	PLAYER_FS_MONEY_SYNC	*pMoney = (PLAYER_FS_MONEY_SYNC*)pMsg;
	
	if ( CheckTrading() )
	{
		if (pMoney->m_nMoney1 >= pMoney->m_nMoney3)
		{
			m_ItemList.SetClient(pMoney->m_nMoney1 - pMoney->m_nMoney3, pMoney->m_nMoney2, pMoney->m_nMoney3);
		}
		else
		{
			m_ItemList.SetClient(0, pMoney->m_nMoney2 + pMoney->m_nMoney1 - pMoney->m_nMoney3, pMoney->m_nMoney3);
		}
		m_cTrade.m_nTradeState = 0;
		m_cTrade.m_nTradeDestState = 0;
		m_cTrade.m_nBackEquipMoney = pMoney->m_nMoney1;
		m_cTrade.m_nBackRepositoryMoney = pMoney->m_nMoney2;
	}
	else
	{
		// 捡钱一定是到room_equipment
		int nMoney1 = m_ItemList.GetsMon(room_equipment);
		if (pMoney->m_nMoney1 - nMoney1 > 0)
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_EARN_MONEY, pMoney->m_nMoney1 - nMoney1);
			sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		m_ItemList.SetClient(pMoney->m_nMoney1, pMoney->m_nMoney2, pMoney->m_nMoney3);
	}
	m_Autoplay.nShiQuNum=0;
}
void	KPlayer::s2cSyncXu(BYTE* pMsg)
{
		PLAYER_FS_XU_SYNC	*pXu = (PLAYER_FS_XU_SYNC*)pMsg;
		int nXu = m_ItemList.GetxLient(room_equipment);
		if (pXu->m_nXu - nXu > 0)
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_EARN_XU, pXu->m_nXu - nXu);
			sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		m_ItemList.SetServer(pXu->m_nXu);//设置客户端金币数量
}
#endif

#ifndef _SERVER
//---------------------------------------------------------------------
//	功能：收到服务器通知有人申请交易
//---------------------------------------------------------------------
void	KPlayer::s2cTradeApplyStart(BYTE* pMsg)
{
	if (!pMsg)
		return;
	TRADE_APPLY_START_SYNC	*pApply = (TRADE_APPLY_START_SYNC*)pMsg;
	int		nNpcIdx;
	nNpcIdx = NpcSet.SearchID(pApply->m_dwNpcId);
	if (nNpcIdx == 0)
		return;
	
	KSystemMessage	sMsg;
	KUiPlayerItem	sPlayer;
	
	snprintf(sPlayer.Name, sizeof(sPlayer.Name), "%s", Npc[nNpcIdx].Name);
	sPlayer.nIndex = pApply->m_nDestIdx;
	sPlayer.uId = pApply->m_dwNpcId;
	sPlayer.nData = 0;
	
	sprintf(sMsg.szMessage, MSG_TRADE_GET_APPLY, Npc[nNpcIdx].Name);
	sMsg.eType = SMT_SYSTEM;
	sMsg.byConfirmType = SMCT_UI_TRADE;
	sMsg.byPriority = 3;
	sMsg.byParamSize = sizeof(KUiPlayerItem);
	sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
	CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, (int)&sPlayer);
}
#endif

#ifndef _SERVER
void	KPlayer::s2cTradeMoneySync(BYTE* pMsg)
{
	TRADE_MONEY_SYNC	*pMoney = (TRADE_MONEY_SYNC*)pMsg;
	
	m_ItemList.SetRoomMoney(room_trade1, pMoney->m_nMoney);
	
	// 通知界面
	KUiObjAtRegion	sMoney;
	sMoney.Obj.uGenre = CGOG_MONEY;
	sMoney.Obj.uId = pMoney->m_nMoney;
	CoreDataChanged(GDCNI_TRADE_DESIRE_ITEM, (unsigned int)&sMoney, 0);
}
#endif

#ifndef _SERVER
//---------------------------------------------------------------------
//	功能：收到服务器通知交易完成或取消
//---------------------------------------------------------------------
void	KPlayer::s2cTradeDecision(BYTE* pMsg)
{
	TRADE_DECISION_SYNC	*pSync = (TRADE_DECISION_SYNC*)pMsg;
	if (pSync->m_btDecision == 1)	// 完成交易
	{
		KSystemMessage	sMsg;
		sprintf(sMsg.szMessage, MSG_TRADE_SUCCESS, m_cTrade.m_szDestName);
		sMsg.eType = SMT_NORMAL;
		sMsg.byConfirmType = SMCT_NONE;
		sMsg.byPriority = 0;
		sMsg.byParamSize = 0;
		sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		
		// 交给ItemList.Remove来清除 modify by spe 03/06/14
		//m_ItemList.ClearRoom(room_trade);
		m_ItemList.RemoveAllInOneRoom(room_trade1);
		//m_ItemList.ClearRoom(room_trade1);
		m_ItemList.ClearRoom(room_tradeback);
		this->m_cTrade.Release();
		m_cMenuState.SetState(PLAYER_MENU_STATE_NORMAL);
		
		// 通知界面交易结束
		CoreDataChanged(GDCNI_TRADE_END, 0, 0);
		
		m_ItemList.MenuSetMouseItem();
	}
	else if (pSync->m_btDecision == 0)	// 取消交易
	{
		if ( !CheckTrading() )
			return;
		
		KSystemMessage	sMsg;
		sprintf(sMsg.szMessage, MSG_TRADE_FAIL, m_cTrade.m_szDestName);
		sMsg.eType = SMT_NORMAL;
		sMsg.byConfirmType = SMCT_NONE;
		sMsg.byPriority = 0;
		sMsg.byParamSize = 0;
		sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		
		// 取消交易的数据恢复
		m_ItemList.RecoverTrade();
		m_ItemList.SetClient(m_cTrade.m_nBackEquipMoney, m_cTrade.m_nBackRepositoryMoney, 0);
		m_ItemList.ClearRoom(room_trade);
		m_ItemList.RemoveAllInOneRoom(room_trade1);
		//		m_ItemList.ClearRoom(room_trade1);
		m_ItemList.ClearRoom(room_tradeback);
		m_cTrade.Release();
		m_cMenuState.SetState(PLAYER_MENU_STATE_NORMAL);
		
		// 通知界面交易结束
		CoreDataChanged(GDCNI_TRADE_END, 0, 0);
		// 更新物品面版
		CoreDataChanged(GDCNI_CONTAINER_OBJECT_CHANGED, UOC_ITEM_TAKE_WITH, 0);
		
		m_ItemList.MenuSetMouseItem();
	}
}
#endif
//客户端  上线玩家参数属性
#ifndef _SERVER
void	KPlayer::SyncCurPlayer(BYTE* pMsg)
{
	//m_ItemList.RemoveAll();  --原来有的
	
	CURPLAYER_SYNC* PlaySync = (CURPLAYER_SYNC *)pMsg;
	this->m_nIndex = NpcSet.SearchID(PlaySync->m_dwID);
	this->m_dwID = g_FileName2Id(Npc[m_nIndex].Name); //稀哈码
	
	Npc[m_nIndex].m_Kind = kind_player;
	Npc[m_nIndex].m_Level = (WORD)PlaySync->m_btLevel;
	Npc[m_nIndex].m_nSex = PlaySync->m_btSex;
	Npc[m_nIndex].m_Series  = PlaySync->m_btSeries;
	Npc[m_nIndex].SetPlayerIdx(CLIENT_PLAYER_INDEX); //=1

	m_nAttributePoint = PlaySync->m_wAttributePoint;
	m_nSkillPoint = PlaySync->m_wSkillPoint;
	m_nStrength = PlaySync->m_wStrength;
	m_nDexterity = PlaySync->m_wDexterity;
	m_nVitality = PlaySync->m_wVitality;
	m_nEngergy = PlaySync->m_wEngergy;
	m_nLucky = PlaySync->m_wLucky;
//	m_nUpExp = PlaySync->m_SUpExp;
	m_nCurStrength = m_nStrength;
	m_nCurDexterity = m_nDexterity;
	m_nCurVitality = m_nVitality;
	m_nCurEngergy = m_nEngergy;
	m_nCurLucky = m_nLucky;
	SetFirstDamage();   //设置基本伤害
	SetBaseAttackRating();
	SetBaseDefence();
	Npc[m_nIndex].m_ActionScriptID = 0;
	Npc[m_nIndex].m_TrapScriptID = 0;
	m_nExp = PlaySync->m_nExp;
	m_nNextLevelExp = PlayerSet.m_cLevelAdd.GetLevelExp(PlaySync->m_btLevel);
	m_dwLeadExp = PlaySync->m_dwLeadExp;
   // m_dwLoginTime = PlaySync->n_dwLoginTime;  //当前登陆帧数
	//m_CUnlocked = PlaySync->m_btCUnlocked;
	m_dwLeadLevel = PlayerSet.m_cLeadExp.GetLevel(m_dwLeadExp);
	m_dwNextLevelLeadExp = PlayerSet.m_cLeadExp.GetLevelExp(m_dwLeadLevel);
	
	m_cFaction.m_nCurFaction = (char)PlaySync->m_btCurFaction;
	m_cFaction.m_nFirstAddFaction = (char)PlaySync->m_btFirstFaction;
	m_cFaction.m_nAddTimes = PlaySync->m_nFactionAddTimes;
	
	m_nWorldStat = (int)PlaySync->m_wWorldStat;//世界排名
	m_nSectStat = (int)PlaySync->m_wSectStat; //门派中的排名

	m_ItemList.Init(CLIENT_PLAYER_INDEX);
	m_ItemList.SetClient(PlaySync->m_nMoney1, PlaySync->m_nMoney2, 0);
	m_ItemList.SetServer(PlaySync->m_nXu);
	Npc[m_nIndex].m_LifeMax = PlaySync->m_wLifeMax;
	Npc[m_nIndex].m_ManaMax = PlaySync->m_wManaMax;
	Npc[m_nIndex].m_NuqiMax = PlaySync->m_wNuqiMax;  //怒气
	Npc[m_nIndex].m_StaminaMax = PlaySync->m_wStaminaMax;

	Npc[m_nIndex].m_LifeReplenish = PLAYER_LIFE_REPLENISH;
	Npc[m_nIndex].m_ManaReplenish = PLAYER_MANA_REPLENISH;
	Npc[m_nIndex].m_StaminaGain = PLAYER_STAMINA_GAIN;
	Npc[m_nIndex].m_StaminaLoss = PLAYER_STAMINA_LOSS;
	
	SetBaseResistData();
	SetBaseSpeedAndRadius();
	
	Npc[m_nIndex].RestoreNpcBaseInfo();
	
	m_BuyInfo.Clear();
	m_cMenuState.Release();
	m_cChat.Release();
	memset(m_szTaskAnswerFun, 0, sizeof(m_szTaskAnswerFun));
	memset(m_szTaskAnswerCanshu, 0, sizeof(m_szTaskAnswerCanshu));
	m_nAvailableAnswerNum = 0;
//	m_nCheckWaiGua=0;
	
	Npc[m_nIndex].m_Experience = 0;
	//Npc[m_nIndex].ClearstrInfo(STR_CHATBUFF_CLIENT);
	memset(Npc[m_nIndex].m_szChatBuffer, 0, sizeof(Npc[m_nIndex].m_szChatBuffer));
	Npc[m_nIndex].m_nCurChatTime = 0;
	
	m_RunStatus = 1;
	m_nLeftSkillID = 0;
	m_nLeftSkillLevel = 0;
	m_nRightSkillID = 0;
	m_nRightSkillLevel = 0;
	m_nPeapleIdx = 0;
	m_nObjectIdx = 0;
	m_MouseDown[0] = 0;
	m_MouseDown[1] = 0;
	
	Npc[m_nIndex].m_SyncSignal = 0;
	//Npc[m_nIndex].RestoreLiveData();
	//SubWorld[0].AddPlayer(Npc[m_nIndex].m_RegionIndex, 0);//m_Region[Npc[m_nIndex].m_RegionIndex].AddPlayer(0);// m_WorldMessage.Send(GWM_PLAYER_ADD, Npc[m_nIndex].m_RegionIndex, 0);
}
#endif

BOOL KPlayer::CheckTrading()
{
	return (m_cMenuState.m_nState == PLAYER_MENU_STATE_TRADING);
}
//设置基本伤害
void	KPlayer::SetFirstDamage()
{
	int nDamageBase;
	
	nDamageBase = m_nCurStrength / STRENGTH_SET_DAMAGE_VALUE + 1;   //力量除于 6 等 基本伤害
	
	Npc[m_nIndex].m_PhysicsDamage.nValue[0] = nDamageBase;
	Npc[m_nIndex].m_PhysicsDamage.nValue[2] = nDamageBase;
	Npc[m_nIndex].m_PhysicsDamage.nValue[1] = 0;

//    Npc[m_nIndex].m_WaiPhysicsDamage.nValue[0] = 0;
  //  Npc[m_nIndex].m_WaiPhysicsDamage.nValue[1] = 0;
	//Npc[m_nIndex].m_WaiPhysicsDamage.nValue[2] = 0;
	
	Npc[m_nIndex].m_CurrentFireDamage.nValue[0] = 0;
	Npc[m_nIndex].m_CurrentFireDamage.nValue[1] = 0;
	Npc[m_nIndex].m_CurrentFireDamage.nValue[2] = 0;
	
	Npc[m_nIndex].m_CurrentColdDamage.nValue[0] = 0;
	Npc[m_nIndex].m_CurrentColdDamage.nValue[1] = 0;
	Npc[m_nIndex].m_CurrentColdDamage.nValue[2] = 0;
	
	Npc[m_nIndex].m_CurrentLightDamage.nValue[0] = 0;
	Npc[m_nIndex].m_CurrentLightDamage.nValue[1] = 0;
	Npc[m_nIndex].m_CurrentLightDamage.nValue[2] = 0;
	
	Npc[m_nIndex].m_CurrentPoisonDamage.nValue[0] = 0;
	Npc[m_nIndex].m_CurrentPoisonDamage.nValue[1] = 0;
	Npc[m_nIndex].m_CurrentPoisonDamage.nValue[2] = 0;

	Npc[m_nIndex].m_CurrentPhysicsMagicDamageV.nValue[0] = 0;
	Npc[m_nIndex].m_CurrentPhysicsMagicDamageV.nValue[2] = 0;
	Npc[m_nIndex].m_CurrentPhysicsMagicDamageV.nValue[1] = 0;

	Npc[m_nIndex].m_CurrentPhysicsMagicDamageP.nValue[0] = 0;
	Npc[m_nIndex].m_CurrentPhysicsMagicDamageP.nValue[2] = 0;
	Npc[m_nIndex].m_CurrentPhysicsMagicDamageP.nValue[1] = 0;
	
	Npc[m_nIndex].m_CurrentMagicFireDamage.nValue[0] = 0;
	Npc[m_nIndex].m_CurrentMagicFireDamage.nValue[1] = 0;
	Npc[m_nIndex].m_CurrentMagicFireDamage.nValue[2] = 0;
	
	Npc[m_nIndex].m_CurrentMagicColdDamage.nValue[0] = 0;
	Npc[m_nIndex].m_CurrentMagicColdDamage.nValue[1] = 0;
	Npc[m_nIndex].m_CurrentMagicColdDamage.nValue[2] = 0;
	
	Npc[m_nIndex].m_CurrentMagicLightDamage.nValue[0] = 0;
	Npc[m_nIndex].m_CurrentMagicLightDamage.nValue[1] = 0;
	Npc[m_nIndex].m_CurrentMagicLightDamage.nValue[2] = 0;
	
	Npc[m_nIndex].m_CurrentMagicPoisonDamage.nValue[0] = 0;
	Npc[m_nIndex].m_CurrentMagicPoisonDamage.nValue[1] = 0;
	Npc[m_nIndex].m_CurrentMagicPoisonDamage.nValue[2] = 0;
	

	Npc[m_nIndex].m_AutoReplySkill.nValue[0] = 0;
	Npc[m_nIndex].m_AutoReplySkill.nValue[1] = 0;
	Npc[m_nIndex].m_AutoReplySkill.nValue[2] = 0;

	Npc[m_nIndex].m_AutoAttackSkill.nValue[0] = 0;
	Npc[m_nIndex].m_AutoAttackSkill.nValue[1] = 0;
	Npc[m_nIndex].m_AutoAttackSkill.nValue[2] = 0;

	Npc[m_nIndex].m_AutoDeathSkill.nValue[0] = 0;
	Npc[m_nIndex].m_AutoDeathSkill.nValue[1] = 0;
	Npc[m_nIndex].m_AutoDeathSkill.nValue[2] = 0;

	Npc[m_nIndex].m_AutoRescueSkill.nValue[0] = 0;
	Npc[m_nIndex].m_AutoRescueSkill.nValue[1] = 0;
	Npc[m_nIndex].m_AutoRescueSkill.nValue[2] = 0;

}

void	KPlayer::SetBaseAttackRating()
{
	Npc[m_nIndex].m_AttackRating = m_nDexterity * 4 - 28;
}

void	KPlayer::SetBaseDefence()
{
	Npc[m_nIndex].m_Defend = m_nDexterity >> 2;
}
//原始的五防
void	KPlayer::SetBaseResistData()
{
	Npc[m_nIndex].m_FireResist			= PlayerSet.m_cLevelAdd.GetFireResist(Npc[m_nIndex].m_Series, Npc[m_nIndex].m_Level);
	Npc[m_nIndex].m_ColdResist			= PlayerSet.m_cLevelAdd.GetColdResist(Npc[m_nIndex].m_Series, Npc[m_nIndex].m_Level);
	Npc[m_nIndex].m_PoisonResist		= PlayerSet.m_cLevelAdd.GetPoisonResist(Npc[m_nIndex].m_Series, Npc[m_nIndex].m_Level);
	Npc[m_nIndex].m_LightResist			= PlayerSet.m_cLevelAdd.GetLightResist(Npc[m_nIndex].m_Series, Npc[m_nIndex].m_Level);
	Npc[m_nIndex].m_PhysicsResist		= PlayerSet.m_cLevelAdd.GetPhysicsResist(Npc[m_nIndex].m_Series, Npc[m_nIndex].m_Level);
	//Npc[m_nIndex].m_FireResistMax		= BASE_FIRE_RESIST_MAX;
	//Npc[m_nIndex].m_ColdResistMax		= BASE_COLD_RESIST_MAX;
	//Npc[m_nIndex].m_PoisonResistMax		= BASE_POISON_RESIST_MAX;
	//Npc[m_nIndex].m_LightResistMax		= BASE_LIGHT_RESIST_MAX;
	//Npc[m_nIndex].m_PhysicsResistMax	= BASE_PHYSICS_RESIST_MAX;
	int resistMax = 0;
	g_GameSetTing.GetInteger("SYSTEM", "BASE_FIRE_RESIST_MAX", BASE_FIRE_RESIST_MAX, &resistMax);
	Npc[m_nIndex].m_FireResistMax = resistMax;

	g_GameSetTing.GetInteger("SYSTEM", "BASE_COLD_RESIST_MAX", BASE_COLD_RESIST_MAX, &resistMax);
	Npc[m_nIndex].m_ColdResistMax = resistMax;

	g_GameSetTing.GetInteger("SYSTEM", "BASE_POISON_RESIST_MAX", BASE_POISON_RESIST_MAX, &resistMax);
	Npc[m_nIndex].m_PoisonResistMax = resistMax;

	g_GameSetTing.GetInteger("SYSTEM", "BASE_LIGHT_RESIST_MAX", BASE_LIGHT_RESIST_MAX, &resistMax);
	Npc[m_nIndex].m_LightResistMax = resistMax;

	g_GameSetTing.GetInteger("SYSTEM", "BASE_PHYSICS_RESIST_MAX", BASE_PHYSICS_RESIST_MAX, &resistMax);
	Npc[m_nIndex].m_PhysicsResistMax = resistMax;
}

void	KPlayer::SetBaseSpeedAndRadius()
{
	Npc[m_nIndex].m_WalkSpeed    = BASE_WALK_SPEED;
	Npc[m_nIndex].m_RunSpeed     = BASE_RUN_SPEED;
	Npc[m_nIndex].m_AttackSpeed  = BASE_ATTACK_SPEED;
	Npc[m_nIndex].m_CastSpeed    = BASE_CAST_SPEED;
	Npc[m_nIndex].m_VisionRadius = BASE_VISION_RADIUS;
	Npc[m_nIndex].m_HitRecover   = BASE_HIT_RECOVER;
}

#ifndef _SERVER
//客户端版本   对话NPC 执行脚本
void KPlayer::DialogNpc(int nIndex,BOOL nIsServer)
{
	if (nIndex > 0 && Npc[nIndex].m_Index > 0)
	{
		//char nMgs[256]={0};
		//if (Npc[nIndex].GetstrInfo(STR_ACTION_SCRIPT,nMgs))
		if (!nIsServer)
		{
			ExecuteScriptD(Npc[nIndex].m_ActionScriptID,"main",Npc[nIndex].Name,"",nIndex,Npc[nIndex].m_dwID,0,0,0,TRUE);
		}
		else
		{
			PLAYER_DIALOG_NPC_COMMAND DialogNpcCmd;
			DialogNpcCmd.nNpcId = Npc[nIndex].m_dwID;
			DialogNpcCmd.ProtocolType = c2s_dialognpc;		
			if (g_pClient)
				g_pClient->SendPackToServer(&DialogNpcCmd, sizeof(PLAYER_DIALOG_NPC_COMMAND));
			
		}
	}
}
//playerShop
void KPlayer::OpenBaiTan(DWORD nIndwId)
{
	g_cSellItem.ApplyViewItem(nIndwId);
}

#endif

#ifndef _SERVER
void KPlayer::CheckObject(int nIdx)
{
/*	enum	// 物件类型
{
Obj_Kind_MapObj = 0,		// 地图物件，主要用于地图动画
Obj_Kind_Body,				// npc 的尸体
Obj_Kind_Box,				// 宝箱
Obj_Kind_Item,				// 掉在地上的装备
Obj_Kind_Money,				// 掉在地上的钱
Obj_Kind_LoopSound,			// 循环音效
Obj_Kind_RandSound,			// 随机音效
Obj_Kind_Light,				// 光源（3D模式中发光的东西）
Obj_Kind_Door,				// 门类
Obj_Kind_Trap,				// 陷阱
Obj_Kind_Prop,				// 小道具，可重生 储物箱
Obj_Kind_Num,				// 物件的种类数
};*/
	
	switch(Object[nIdx].m_nKind)
	{
//////原来没有的/////////////
	case Obj_Kind_Item:
		PickUpObj(nIdx);
		Npc[m_nIndex].m_nObjectIdx = 0;
		break;
	case Obj_Kind_Money:
		PickUpObj(nIdx);
		Npc[m_nIndex].m_nObjectIdx = 0;
		break;
	case Obj_Kind_Box:
	case Obj_Kind_Door:
	case Obj_Kind_Trap:  //陷阱类 
	case Obj_Kind_Prop:	 //储物箱等
		this->ObjMouseClick(nIdx);
		//Npc[m_nIndex].m_nIsOver=TRUE;
		Npc[m_nIndex].m_nObjectIdx = 0;
		break;
	default:
		break;
	}
	m_nObjectIdx = 0;
	m_nPickObjectIdx = 0;
}
#endif

#ifndef _SERVER
void KPlayer::DrawSelectInfo()   //选择人物后处理 的显示
{
	if (m_nIndex <= 0)
		return;

	if (m_nPeapleIdx)
	{//npc
		if (Npc[m_nPeapleIdx].m_Kind == kind_player) // 玩家
		{
			if (!NpcSet.CheckShowName())
			{
				Npc[m_nPeapleIdx].PaintInfo(Npc[m_nPeapleIdx].GetNpcPate(), true);
			}
		}
		else if (Npc[m_nPeapleIdx].m_Kind == kind_dialoger)  //对话者
		{
			if (!NpcSet.CheckShowName())
			{
			    Npc[m_nPeapleIdx].PaintInfo(Npc[m_nPeapleIdx].GetNpcPate(), true);
			}
		}
		else if (Npc[m_nPeapleIdx].m_Kind == kind_partner)  //同伴类
		{
			if (!NpcSet.CheckShowName())
			{
				Npc[m_nPeapleIdx].PaintInfo(Npc[m_nPeapleIdx].GetNpcPate(), true);
			}
		}
		else   //动物冒血处理
		{
			Npc[m_nPeapleIdx].DrawBlood();   //绘制血条  冒血处理
		}

		return;
	}
	if (m_nObjectIdx)
	{//物品
		if (!ObjSet.CheckShowName())
			Object[m_nObjectIdx].DrawInfo();
		//	Object[m_nObjectIdx].DrawBorder();
		return;
	}
}
#endif
//获取人物最大和最小最终伤害
//#ifndef _SERVER
void KPlayer::GetEchoDamage(int* nMin, int* nMax, int* nAr, int nType)
{
	int nSkillId;
	*nMin = 0;
	*nMax = 0;
	
	if (m_nIndex <= 0 || m_nIndex >= MAX_NPC || nType < 0 || nType > 1 || NULL == nMin || NULL == nMax)
		return;
	//左右技能选择

	nSkillId = (0 == nType)?m_nLeftSkillID:m_nRightSkillID;
	int nListidx = Npc[m_nIndex].m_SkillList.FindSame(nSkillId);
	int nLevel = Npc[m_nIndex].m_SkillList.GetCurrentLevelByIdx(nListidx);
	
	if (nSkillId <= 0 ||nLevel <= 0 || nLevel >= MAX_SKILLLEVEL)
		return;
	
	KMagicAttrib*	pMagicData = NULL;
	
	KSkill * pOrdinSkill = (KSkill *) g_SkillManager.GetSkill(nSkillId, nLevel);
	if (!pOrdinSkill) 
        return ;

	switch(pOrdinSkill->GetSkillStyle())
	{
		case SKILL_SS_Missles:
		case SKILL_SS_Melee:
		case SKILL_SS_InitiativeNpcState:
		case SKILL_SS_PassivityNpcState:
			{
			}
			break;
		default:
			return;
	}

	BOOL	bIsPhysical = pOrdinSkill->IsPhysical();          //物理攻击
	BOOL	bIsMagic = pOrdinSkill->IsMagic();                //1内功系 或0外攻系
	BOOL	bIsSkillMagic = pOrdinSkill->IsSkillMagic();
	BOOL	bIsSkillPhysical = pOrdinSkill->IsSkillPhysical();

	// lay % ho tro skill
	int nSkillEnhance = 0;
	if(nSkillId > 0 && nSkillId < MAX_SKILL)
	{
		nSkillEnhance = Npc[m_nIndex].m_SkillList.GetEnChance(nListidx);;
	}

	pMagicData = pOrdinSkill->GetDamageAttribs();     //攻击技能数据

	if (!pMagicData)
		return;

	int nMinNpcDamage = Npc[m_nIndex].m_PhysicsDamage.nValue[0] + Npc[m_nIndex].m_CurrentAddPhysicsDamage;
	int nMaxNpcDamage = Npc[m_nIndex].m_PhysicsDamage.nValue[2] + Npc[m_nIndex].m_CurrentAddPhysicsDamage;
	// Skip attackrating
	*nAr = Npc[m_nIndex].m_CurrentAttackRating;
	if(nType == 0)
	{
		if (magic_attackrating_p == pMagicData->nAttribType)
		{
			*nAr += Npc[m_nIndex].m_AttackRating * pMagicData->nValue[0] / 100;
		}
	}
	// Skip ignoredefense
	pMagicData++;
	// Calc physics damage
	pMagicData++;
	if (magic_physicsenhance_p == pMagicData->nAttribType)
	{
		*nMin += nMinNpcDamage * (100 + pMagicData->nValue[0]) / 100;
		*nMax += nMaxNpcDamage * (100 + pMagicData->nValue[0]) / 100;
		*nMin += nMinNpcDamage * (pMagicData->nValue[0] * nSkillEnhance / 100) / 100;
		*nMax += nMaxNpcDamage * (pMagicData->nValue[0] * nSkillEnhance / 100) / 100;
		
		int nEnhance;
		if (equip_meleeweapon == m_ItemList.GetWeaponType())
		{
			nEnhance = Npc[m_nIndex].m_CurrentMeleeEnhance[m_ItemList.GetWeaponParticular()];
		}
		else if (equip_rangeweapon == m_ItemList.GetWeaponType())
		{
			nEnhance = Npc[m_nIndex].m_CurrentRangeEnhance;
		}
		else 
		{
			nEnhance = Npc[m_nIndex].m_CurrentHandEnhance;
		}
		*nMin += nMinNpcDamage * nEnhance / 100;
		*nMax += nMaxNpcDamage * nEnhance / 100;
	}
	else if (magic_physicsdamage_v == pMagicData->nAttribType)
	{
		*nMin += pMagicData->nValue[0] * (100 + nSkillEnhance) / 100;
		*nMax += pMagicData->nValue[2] * (100 + nSkillEnhance) / 100;
		// Luc danh vat ly noi
		if (!bIsPhysical)
		{
			*nMin += Npc[m_nIndex].m_MagicPhysicsDamage.nValue[0];
			*nMax += Npc[m_nIndex].m_MagicPhysicsDamage.nValue[2];
		}
	}
	pMagicData++;
	// Tinh luc danh bang
	if (magic_colddamage_v == pMagicData->nAttribType)
	{
		*nMin += pMagicData->nValue[0] * (100 + nSkillEnhance) / 100;
		*nMax += pMagicData->nValue[2] * (100 + nSkillEnhance) / 100;
		// Luc danh bang noi
		if (!bIsPhysical)
		{
			*nMin += Npc[m_nIndex].m_CurrentMagicColdDamage.nValue[0];
			*nMax += Npc[m_nIndex].m_CurrentMagicColdDamage.nValue[2];
		}
	}
	// Luc danh bang ngoai
	if (bIsPhysical)
	{
		*nMin += Npc[m_nIndex].m_CurrentColdDamage.nValue[0];
		*nMax += Npc[m_nIndex].m_CurrentColdDamage.nValue[2];
	}
	// Tinh luc danh hoa
	pMagicData++;
	if (magic_firedamage_v == pMagicData->nAttribType)
	{
		/* Fix by kinnox;
		*nMin += pMagicData->nValue[0] * (100 + nSkillEnhance) / 100;
		*nMax += pMagicData->nValue[2] * (100 + nSkillEnhance) / 100;
		// Luc danh hoa noi
		if (!bIsPhysical)
		{
			*nMin += Npc[m_nIndex].m_CurrentMagicFireDamage.nValue[0];
			*nMax += Npc[m_nIndex].m_CurrentMagicFireDamage.nValue[2];
		}*/

		*nMin += pMagicData->nValue[0]*(100+nSkillEnhance)/100 + pMagicData->nValue[0]*(100+nSkillEnhance)/100 * Npc[m_nIndex].m_CurrentFireEnhance/100;
		*nMax += pMagicData->nValue[2]*(100+nSkillEnhance)/100 + pMagicData->nValue[2]*(100+nSkillEnhance)/100 * Npc[m_nIndex].m_CurrentFireEnhance/100;
		if (!bIsPhysical)
	     {
			 *nMin += Npc[m_nIndex].m_CurrentMagicFireDamage.nValue[0] + Npc[m_nIndex].m_CurrentMagicFireDamage.nValue[0] * Npc[m_nIndex].m_CurrentFireEnhance/100;
			 *nMax += Npc[m_nIndex].m_CurrentMagicFireDamage.nValue[2] + Npc[m_nIndex].m_CurrentMagicFireDamage.nValue[2] * Npc[m_nIndex].m_CurrentFireEnhance/100;
	     }

	}
	// Luc danh hoa ngoai
	if (bIsPhysical)
	{
		/* Fix by kinnox;			
		*nMin += Npc[m_nIndex].m_CurrentFireDamage.nValue[0];
		*nMax += Npc[m_nIndex].m_CurrentFireDamage.nValue[2];*/
		*nMin += Npc[m_nIndex].m_CurrentFireDamage.nValue[0] + Npc[m_nIndex].m_CurrentFireDamage.nValue[0] * Npc[m_nIndex].m_CurrentFireEnhance/100;
		*nMax += Npc[m_nIndex].m_CurrentFireDamage.nValue[2] + Npc[m_nIndex].m_CurrentFireDamage.nValue[2] * Npc[m_nIndex].m_CurrentFireEnhance/100;
	}
	// Tinh luc danh loi
	pMagicData++;
	if (magic_lightingdamage_v == pMagicData->nAttribType)
	{
		/* Fix by kinnox;	
		*nMin += pMagicData->nValue[0] * (100 + nSkillEnhance) / 100;
		*nMax += pMagicData->nValue[2] * (100 + nSkillEnhance) / 100;
		// Luc danh loi noi
		if (!bIsPhysical)
		{
			*nMin += Npc[m_nIndex].m_CurrentMagicLightDamage.nValue[0];
			*nMax += Npc[m_nIndex].m_CurrentMagicLightDamage.nValue[2];
		}*/
		*nMin += pMagicData->nValue[0]*(100+nSkillEnhance)/100 + (pMagicData->nValue[2]*(100+nSkillEnhance)/100 - pMagicData->nValue[0]*(100+nSkillEnhance)/100) * Npc[m_nIndex].m_CurrentLightEnhance / 100;
		*nMax += pMagicData->nValue[2]*(100+nSkillEnhance)/100;

		if (!bIsPhysical)
	     {
		 *nMin += Npc[m_nIndex].m_CurrentMagicLightDamage.nValue[0];
		 *nMax += Npc[m_nIndex].m_CurrentMagicLightDamage.nValue[2];
	     }
	}

	// Luc danh loi ngoai
	if (bIsPhysical)
	{
		*nMin += Npc[m_nIndex].m_CurrentLightDamage.nValue[0];
		*nMax += Npc[m_nIndex].m_CurrentLightDamage.nValue[2];
	}
	// Tinh luc danh doc
	pMagicData++;
	if (magic_poisondamage_v == pMagicData->nAttribType)
	{
		if (pMagicData->nValue[2] > 0)
		{
			int nPoisonDamage = (pMagicData->nValue[0] * pMagicData->nValue[1] / pMagicData->nValue[2]);
			nPoisonDamage = nPoisonDamage * (100 + nSkillEnhance) / 100;
			*nMin += nPoisonDamage;
			*nMax += nPoisonDamage;
		}
		// Luc danh doc noi
		if (!bIsPhysical)
		{
			if (Npc[m_nIndex].m_CurrentMagicPoisonDamage.nValue[2] > 0)
			{
				int nPoisonDamage = (Npc[m_nIndex].m_CurrentMagicPoisonDamage.nValue[0]
					* Npc[m_nIndex].m_CurrentMagicPoisonDamage.nValue[1] 
					/ Npc[m_nIndex].m_CurrentMagicPoisonDamage.nValue[2]);
				*nMin += nPoisonDamage;
				*nMax += nPoisonDamage;
			}
		}
	}
	// Luc danh doc ngoai
	if (bIsPhysical)
	{
		if (Npc[m_nIndex].m_CurrentPoisonDamage.nValue[2] > 0)
		{
			int nPoisonDamage = (Npc[m_nIndex].m_CurrentPoisonDamage.nValue[0]
				* Npc[m_nIndex].m_CurrentPoisonDamage.nValue[1] 
				/ Npc[m_nIndex].m_CurrentPoisonDamage.nValue[2]);
			*nMin += nPoisonDamage;
			*nMax += nPoisonDamage;
		}
	}
	pMagicData++;
	// Calc magic damage
	if (magic_magicdamage_v == pMagicData->nAttribType)
	{
		*nMin += pMagicData->nValue[0];
		*nMax += pMagicData->nValue[2];
	}
}

//#endif

//换装是调用函数设置物理伤害
void KPlayer::SetNpcDamageAttrib()
{
	SetNpcPhysicsDamage();
}
//设置跨服的 目标地图点
#ifdef _SERVER
void KPlayer::TobeExchangeServer(DWORD dwMapID, int nX, int nY,BOOL isChange)
{
	if (!m_dwID || m_nNetConnectIdx == -1)
		return;
	
	m_sExchangePos.m_dwMapID = dwMapID;
	m_sExchangePos.m_nX = nX;
	m_sExchangePos.m_nY = nY;
	m_bExchangeServer = isChange;
}
#endif

/*
#ifdef _SERVER
void KPlayer::UpdateEnterGamePos(DWORD dwSubWorldID, int nX, int nY, int nFightMode)
{
	TRoleData* pData = (TRoleData *)&m_SaveBuffer;
	pData->BaseInfo.cFightMode = nFightMode;
	pData->BaseInfo.cUseRevive = 0;
	pData->BaseInfo.ientergameid = dwSubWorldID;
	pData->BaseInfo.ientergamex = nX;
	pData->BaseInfo.ientergamey = nY;
}
#endif
*/

#ifdef _SERVER
BOOL KPlayer::IsExchangingServer()
{
	return m_bExchangeServer;
}
#endif
//服务器端 打开
#ifdef _SERVER

void KPlayer::Market(int nId,int shopID) 
{ 
	BuySell.OpenSMarket(nId, shopID);  //买卖ID
}
#endif


#ifdef _SERVER
void KPlayer::String(int nbutton)
	{//stringbox
	    //char scriptName[128]={0}; 
		//sprintf(scriptName,"\\script\\system\\input.lua");
		//ExecuteScriptA(scriptName,"main",nbutton,"空"); 
		ExeNoBackScript("\\script\\system\\input.lua","main",nbutton,0,0,0);
	}
#endif

//挂机
#ifdef _SERVER
void KPlayer::SAutoplay(int nbutton,int nBtyeVal,int nBtyeVala)
{
   if (nbutton==0 || nbutton==1)
   {
      Npc[m_nIndex].m_AutoplayId=(BYTE)nbutton;  //挂机

	  m_Autoplay.nIsNoBlue          = EGetByte(nBtyeVal,1);  //是否过滤蓝怪
	  m_Autoplay.nIsOpenPicjkup     = EGetByte(nBtyeVal,2);  //是否开启拾取
	  m_Autoplay.nAutoMoney         = EGetByte(nBtyeVal,3);
	  m_Autoplay.nAutoEques         = EGetByte(nBtyeVal,4);

	  m_Autoplay.nAutoMoneyAndEques = EGetByte(nBtyeVala,1);
	  m_Autoplay.nAutoTuPu          = EGetByte(nBtyeVala,2);
	  m_Autoplay.nAutoTool          = EGetByte(nBtyeVala,3);
	  /*
	  AutoPlayCmd.ByteVal=ESetByte(AutoPlayCmd.ByteVal,3,Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoMoney);//钱
	  AutoPlayCmd.ByteVal=ESetByte(AutoPlayCmd.ByteVal,4,Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoEques);//装备

	  AutoPlayCmd.ByteVala=ESetByte(AutoPlayCmd.ByteVala,1,Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoMoneyAndEques);//钱和装备
	  AutoPlayCmd.ByteVala=ESetByte(AutoPlayCmd.ByteVala,2,Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoTuPu);//图谱
	  AutoPlayCmd.ByteVala=ESetByte(AutoPlayCmd.ByteVala,3,Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoTool);//道具
	  
	  */
	  //m_cTask.GetTaskByte(nBtyeVal,2);
   }				   

    char scriptName[128]={0}; 
	sprintf(scriptName,"%s","\\script\\system\\autoplayer.lua");
	ExecuteScriptA(scriptName,"main",nbutton,"无",FALSE); //99编号以后为抽奖所用
}
#endif

#ifdef _SERVER
void KPlayer::script(int nId,int nXuHao) 
{ 
	char scriptName[255];
	if (nId == 1)
	{	 
		sprintf(scriptName,"\\script\\laotou\\quest_exp.lua");  //datau
		ExecuteScriptA(scriptName,"main",0,"",FALSE); 
	}
	else if (nId == 2)
	{
		sprintf(scriptName,"\\script\\laotou\\quest_random.lua");
		ExecuteScriptA(scriptName,"main",0,"",FALSE); 	
	}
	else if (nId == 3)
	{
		sprintf(scriptName,"\\script\\laotou\\quest_money.lua");
		ExecuteScriptA(scriptName,"main",0,"",FALSE); 	
	}
	
	else if (nId == 4)
	{
		sprintf(scriptName,"\\script\\laotou\\quest_lucky.lua");
		ExecuteScriptA(scriptName,"main",0,"",FALSE); 	
	}
	else if (nId == 5)
	{
		sprintf(scriptName,"\\script\\laotou\\quest_item.lua");
		ExecuteScriptA(scriptName,"main",0,"",FALSE); 	
	}
	else if (nId == 6)
	{
		sprintf(scriptName,"\\script\\laotou\\quest_point.lua");
		ExecuteScriptA(scriptName,"main",0,"",FALSE); 	
	}
	else if (nId == 10)  //为股子所用
	{
		sprintf(scriptName,"\\script\\system\\guzi_xiazhu.lua");
		ExecuteScriptA(scriptName,"main",nXuHao,"空"); 	
	}
	
}
#endif


#ifdef _SERVER
void KPlayer::pkvalue(int nId) 
{ 
	if (nId == 1)
	{
		char scriptName[255]; 
		sprintf(scriptName,"\\script\\special\\trangthai.lua");  //datau
		ExecuteScriptA(scriptName,"main",1,"空"); 
	}
	else if (nId == 2)
	{
		char scriptName[255]; 
		sprintf(scriptName,"\\script\\special\\trangthai.lua");
		ExecuteScriptA(scriptName,"main",2,"空"); 	
	}
	
	else if (nId == 3)
	{
		char scriptName[255]; 
		sprintf(scriptName,"\\script\\special\\trangthai.lua");
		ExecuteScriptA(scriptName,"main",3,"空"); 	
	}	
	
}
#endif


#ifdef _SERVER
void	KPlayer::SetChestLock(BOOL unlockFlag)   //检查是否锁住屏幕
{
	if (unlockFlag)
	{
		m_CUnlocked = 1;
		/*char szMsg[100];
		sprintf(szMsg, "<c=cyan>您已经解锁,可以操作界面了!<c>");
		int nLength=0; 
		nLength = TEncodeText(szMsg, strlen(szMsg));
		KPlayerChat::SendSystemInfo(1, m_nPlayerIndex, "屏锁:", (char *) szMsg, nLength);*/
	}
	else
	{
		m_CUnlocked = 0;
		/*char szMsg[100];
		sprintf(szMsg, "<c=cyan>屏幕已经锁定,禁止操作部分界面!<c>");
		int nLength=0; 
		nLength = TEncodeText(szMsg, strlen(szMsg));
		KPlayerChat::SendSystemInfo(1, m_nPlayerIndex, "屏锁:", (char *) szMsg, nLength );*/
	}

	PLAYER_CHESTPW_SYNC	pChestSync;
	pChestSync.ProtocolType = s2c_chestpwsync;
	pChestSync.m_Unlocked = m_CUnlocked;
	pChestSync.m_Created = m_nChestPW > 0 ? 1 : 0;
	if (g_pServer)
		g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&pChestSync, sizeof(PLAYER_CHESTPW_SYNC));
}
#endif

#ifdef _SERVER
BOOL	KPlayer::CheckChestPW(int iPassword) //检查屏锁密码
{
	if (iPassword == m_nChestPW)
	{
		// Mat khau ruong chinh xac
		//char szMsg[100];
		//sprintf(szMsg, "<c=cyan>输入密码正确,解锁！<c>");
		//int nLength=0; 
		//nLength = TEncodeText(szMsg, strlen(szMsg));
		//KPlayerChat::SendSystemInfo(1, m_nPlayerIndex, "屏锁:", (char *) szMsg, nLength);
		return TRUE;
	}
	else
	{
		// Mat khau ruong SAI
		//char szMsg[100];
		//sprintf(szMsg, "<c=cyan>输入密码不正确,请检查!<c>");
		//int nLength=0; 
		//nLength = TEncodeText(szMsg, strlen(szMsg));
		//KPlayerChat::SendSystemInfo(1, m_nPlayerIndex, "屏锁:", (char *) szMsg, nLength);
		return FALSE;
	}
}
#endif

#ifdef _SERVER
void	KPlayer::SetChestPW(int newPW)
{
	m_nChestPW = newPW;
	
	// Thong bao cho Client
	//char szMsg[100];
	//sprintf(szMsg, "<c=cyan>设置屏锁密码完成!<c>");
	//int nLength=0; 
		//nLength = TEncodeText(szMsg, strlen(szMsg));
	//KPlayerChat::SendSystemInfo(1, m_nPlayerIndex, "屏锁:", (char *) szMsg, nLength);
	
	PLAYER_CHESTPW_SYNC	pChestSync;
	pChestSync.ProtocolType = s2c_chestpwsync;
	pChestSync.m_Unlocked = m_CUnlocked;
	pChestSync.m_Created = m_nChestPW > 0 ? 1 : 0;
	if (g_pServer)
		g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&pChestSync, sizeof(PLAYER_CHESTPW_SYNC));
}
#endif

#ifdef _SERVER
void	KPlayer::SetNewPW(int resetPW) //设置新的屏锁密码
{
	if (resetPW <= 0)
		resetPW = 0;

	m_nChestPW = resetPW;
	m_CUnlocked = 0;
	
	// Thong bao cho Client
	//char szMsg[100];
	//sprintf(szMsg, "<c=cyan>重置屏锁密码完成!<c>");
	//int nLength=0; 
		//nLength = TEncodeText(szMsg, strlen(szMsg));
	//KPlayerChat::SendSystemInfo(1, m_nPlayerIndex, "屏锁:", (char *) szMsg,nLength);

	PLAYER_CHESTPW_SYNC	pChestSync;
	pChestSync.ProtocolType = s2c_chestpwsync;
	pChestSync.m_Unlocked = m_CUnlocked;
	pChestSync.m_Created = m_nChestPW > 0 ? 1 : 0;
	if (g_pServer)
		g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&pChestSync, sizeof(PLAYER_CHESTPW_SYNC));
}


#endif

#ifdef _SERVER
void KPlayer::SetLastNetOperationTime(DWORD nTime)
{
	if (m_nNetConnectIdx == -1 || m_dwID == 0 || m_nIndex <= 0)
		return;
	
	m_nLastNetOperationTime = nTime;

	if (m_bSleepMode)
	{
		m_bSleepMode = FALSE;
		NPC_SLEEP_SYNC	SleepSync;
		SleepSync.ProtocolType = s2c_npcsleepmode;
		SleepSync.bSleep = m_bSleepMode;
		SleepSync.NpcID = Npc[m_nIndex].m_dwID;
		Npc[m_nIndex].SendDataToNearRegion(&SleepSync, sizeof(NPC_SLEEP_SYNC));
	}
}
#endif

#ifdef _SERVER
BOOL	KPlayer::CreateTong(int nCamp, char *lpszTongName, char* lpszTitle)
{//帮会创建成功
	return this->m_cTong.Create(nCamp, lpszTongName, lpszTitle);
}
#endif

#ifdef _SERVER
void KPlayer::RepairItem(DWORD dwItemID)
{
	int nIdx  = m_ItemList.SearchID(dwItemID);
	int nCost = Item[nIdx].GetRepairPrice();

	int  nCount=0,nReg=0;

	if (nCost<=0/* && Item[nIdx].GetDurability() > 3*/)
	{
		m_ItemList.msgshow("Trang bi nay khong can sua chua");
		return;
	}

    g_GameSetTing.GetInteger("ForbitRepairSet","Count",0,&nCount);

    if (nCount>0)
	{ 
		for (int i=0;i<nCount;++i)
		{
			char nKey[16],nItemName[80];
			snprintf(nKey, sizeof(nKey),"ItemName_%d",i);
            g_GameSetTing.GetString("ForbitRepairSet",nKey,"",nItemName,sizeof(nItemName));
			if (strstr(Item[nIdx].GetName(),nItemName))
			{
				nReg=1;
				break;
			} 
		}
	} 
    
	if (nReg>=1)
	{
		m_ItemList.msgshow("Hien tai trang bi nay khong can sua chua");
		return;
	}

	int nMaxDur = Item[nIdx].GetMaxDurability();
	int nDur    = Item[nIdx].GetDurability();

	if (Pay(nCost) && m_nNetConnectIdx >= 0)
	{////修理磨损警告
		Item[nIdx].SetDurability(nMaxDur);
        Item[nIdx].SetIsCanUse(1);
		ITEM_DURABILITY_CHANGE	IDC;
		IDC.ProtocolType = s2c_itemdurabilitychange;
		IDC.dwItemID = dwItemID;
		IDC.nChange  = nMaxDur - nDur;

		if (m_ItemList.CheckIsOutTime(nIdx)) //是辨识装备 并且是过期的
           IDC.nCanUse  = 0;
		else
           IDC.nCanUse  = 1;

		if (g_pServer)
			g_pServer->PackDataToClient(m_nNetConnectIdx, &IDC, sizeof(ITEM_DURABILITY_CHANGE));
	}
}
#endif
//拆分物品
//xep chong BreakItem
#ifdef _SERVER
void KPlayer::BreakItem(DWORD dwItemID, int nNum) 
{
	int nIdx   = m_ItemList.SearchID(dwItemID);
	//检测包袱是否有足够的空间
	/*int ncount = m_ItemList.GetFreePositionCount(Item[nIdx].GetWidth(),Item[nIdx].GetHeight(),room_equipment);
	
	if (ncount<nNum)
	{
		m_ItemList.msgshow("Loi: ncount<nNum");
		return;
	}*/

	ItemPos	sItemPosCheck;
	if (!m_ItemList.SearchPosition(Item[nIdx].GetWidth(),Item[nIdx].GetHeight(), &sItemPosCheck) )
	{
		printf("Khong con cho trong de phan tach, loi !\n");
		return;
	}


	if (!Item[nIdx].IsStack() || nNum >= Item[nIdx].GetStackNum() || nNum < 1)
	    return;

	if (Item[nIdx].GetStackNum() > nNum)
	{
		int nIndex = ItemSet.AddOther(Item[nIdx].GetGenre(),\
									Item[nIdx].GetSeries(), \
									Item[nIdx].GetLevel(), \
									10, \
									Item[nIdx].GetDetailType(), \
									Item[nIdx].GetParticular(),\
									NULL, g_SubWorldSet.GetGameVersion(),\
									0,\
									1);		
		//xep chong item

		int	x, y;
		if (m_ItemList.CheckCanPlaceInEquipment(Item[nIndex].GetWidth(), Item[nIndex].GetHeight(), &x, &y))
		{
			Item[nIdx].SetStackNum(Item[nIdx].GetStackNum() - nNum);

			Item[nIndex].LockItem(Item[nIdx].GetLockItem()->nState);
			//Item[nIndex].GetLockItem()->dwLockTime = Item[nIdx].GetLockItem()->dwLockTime;

			Item[nIndex].SetBang(Item[nIdx].GetIsBang());
			Item[nIndex].SetTime(
				Item[nIdx].GetTime()->bYear,
				Item[nIdx].GetTime()->bMonth,
				Item[nIdx].GetTime()->bDay,
				Item[nIdx].GetTime()->bHour,
				Item[nIdx].GetTime()->bMin);
			m_ItemList.SynPlayerItem(nIdx); 

			m_ItemList.Add(nIndex,pos_equiproom,x,y,0,0,nNum,true);  
		}
		else
		{
			m_ItemList.msgshow("Loiiiiiiiiiiiiiiiii");
			int	nIdx = m_ItemList.Hand();
			if (nIdx)
			{
				m_ItemList.Remove(nIdx);
				ItemSet.Remove(nIdx);
			} 
			m_ItemList.Remove(nIndex);
			ItemSet.Remove(nIndex);
		}
	}

}
#endif


void KPlayer::SetExtGameTime(DWORD nPoint)
{
	m_GameLeftTime       = nPoint;
}

UINT KPlayer::GetExtPoint()
{
	return m_nExtPoint;  //扩展点 会员充值
}

BOOL KPlayer::PayExtPoint(int nPoint)
{
	if (m_nExtPoint < nPoint)
		return FALSE;

	m_nExtPoint -= nPoint;
	m_nChangeExtPoint += nPoint;
	return TRUE;
}


#ifdef _SERVER

void KPlayer::SetExtPoint(DWORD nPoint, LONG nChangePoint)
{
	m_nExtPoint       = nPoint;
	m_nChangeExtPoint = nChangePoint;
}

void KPlayer::SetLeftTineAndVipType(DWORD nLeftTime,int nVipType,LONG nChangeExtTime)
{
	if (m_nLeftTime>=0) //剩余时间
	    m_nLeftTime = nLeftTime;

	m_nVipType  = nVipType;	 //会员类型
	m_nChangeExtTime = nChangeExtTime;	//变动的时间
}

//要改变的时间
long KPlayer::GetExtTimeChanged()
{
	return m_nChangeExtTime;
}

//恢复原来称号=========
void  KPlayer::ResSetNpcTitle()
{
	int i,nSkillD,nSkillevel;

	if  (Npc[m_nIndex].m_NpcTitle<=0)
		return;

	Npc[m_nIndex].m_CurNpcTitle=0;

	m_nPlayerTime  = -1;

	for (i=0;i<5;++i)
	{
		nSkillD=Npc[m_nIndex].m_ExtSkill[i];
		if (nSkillD>0)
		{//有技能ID,需要先去除状态
			Npc[m_nIndex].m_ExtSkill[i]=0;
			Npc[m_nIndex].SetSkillLeftTime(nSkillD,0); //同步客户端删除状态			
		}
	}
	
	nSkillD=0;nSkillevel=0;
	
	for (i=0;i<5;++i)
	{
		char nKeyVal[48];
		sprintf(nKeyVal,"ExtSkill%d",i+1);
		g_PlayerTitle.GetInteger(Npc[m_nIndex].m_NpcTitle+1,nKeyVal,0,&nSkillD);
		snprintf(nKeyVal, 48, "ExtSkillLevel%d",i+1);
		g_PlayerTitle.GetInteger(Npc[m_nIndex].m_NpcTitle+1,nKeyVal,0,&nSkillevel);
		
		if ((nSkillD<=0 && nSkillevel<=0) || nSkillevel>=MAX_SKILLLEVEL)
		{
			Npc[m_nIndex].m_ExtSkill[i]=0;
			continue;
		}
		
		Npc[m_nIndex].m_ExtSkill[i]=nSkillD;
		
		UseToolSkills(nSkillD,nSkillevel,m_nIndex,m_nIndex,NULL,1,-1);
	}
}

void KPlayer::UseToolSkills(int sKillID,int sLevel,int nNpcIdx,int nDesNpcIdx,KMagicAttrib *pData,BOOL nIfMagic,int nTime,int nBeiVal)
{										   
	      KSkill *pSkill = (KSkill*)g_SkillManager.GetSkill(sKillID,sLevel);

		  if (nIfMagic)
		  {	   
			  if (pSkill)
			  { 
				  if (pSkill->GetSkillStyle()!=SKILL_SS_Missles)	////0为子弹 2 为主动 3 为被动
				  { //使用非子弹 类技能  （包括被动 主动技能状态）
					  //if (pSkill->GetSkillStyle()==SKILL_SS_PassivityNpcState)
					  //   nTime=-1;
					  // printf("执行技能%s{%d)等级(%d)--\n",pSkill->GetSkillName(),sKillID,sLevel);
					   pSkill->CastToolStateSkill(nNpcIdx,0,0,nTime);  //是否
				  } 
				  else
				  {//就是子弹技能	0;	//攻击技能
					 if(pSkill->Cast(nNpcIdx, -1,nDesNpcIdx,nTime,SKILL_SLT_Npc,nBeiVal))//,20,SKILL_SLT_Npc
                         pSkill->CastSkill(nNpcIdx,sKillID,-1,Npc[nNpcIdx].m_dwID,sLevel,nTime,nBeiVal);  
				  } 
			  }	  
		  }
		  else
			Npc[nNpcIdx].SetToolStateSkillEffect(nNpcIdx,sKillID,sLevel,pData,0,nTime);  //设置技能光环效果	
}

void KPlayer::SetPlayerTitleSkills()
{										   
	int i,nSkillD,nSkillevel;

	if  (Npc[m_nIndex].m_NpcTitle<=0)
		return;

	Npc[m_nIndex].m_CurNpcTitle=0;

	for (i=0;i<5;++i)
	{
		nSkillD=Npc[m_nIndex].m_ExtSkill[i];
		if (nSkillD>0)
		{//有技能ID,需要先去除状态
			Npc[m_nIndex].m_ExtSkill[i]=0;
			Npc[m_nIndex].SetSkillLeftTime(nSkillD,0); //同步客户端删除状态			
		}
	}
	
	nSkillD=0;nSkillevel=0;
	
	for (i=0;i<5;++i)
	{
		char nKeyVal[48];
		sprintf(nKeyVal,"ExtSkill%d",i+1);
		g_PlayerTitle.GetInteger(Npc[m_nIndex].m_NpcTitle+1,nKeyVal,0,&nSkillD);
		snprintf(nKeyVal, 48, "ExtSkillLevel%d",i+1);
		g_PlayerTitle.GetInteger(Npc[m_nIndex].m_NpcTitle+1,nKeyVal,0,&nSkillevel);
		
		if ((nSkillD<=0 && nSkillevel<=0) || nSkillevel>=MAX_SKILLLEVEL)
		{
			Npc[m_nIndex].m_ExtSkill[i]=0;
			continue;
		}
		
		Npc[m_nIndex].m_ExtSkill[i]=nSkillD;
		int doitime = TimeToHour(m_nPlayerTitleTime,KSG_GetCurSec());
		//printf("doitime: %d m_nPlayerTitleTime: %d KSG: %d\n",doitime,m_nPlayerTitleTime,KSG_GetCurSec());
		UseToolSkills(nSkillD,nSkillevel,m_nIndex,m_nIndex,NULL,1, doitime * 64800);
	}
} 

int KPlayer::TimeToHour(int time1, int time2)
{
    int y1 = time1 / 1000000;
    int m1 = (time1 / 10000) % 100;
    int d1 = (time1 / 100) % 100;
    int h1 = time1 % 100;

    int y2 = time2 / 1000000;
    int m2 = (time2 / 10000) % 100;
    int d2 = (time2 / 100) % 100;
    int h2 = time2 % 100;

    struct tm t1 = {0}, t2 = {0};
    t1.tm_year = y1 - 1900;
    t1.tm_mon  = m1 - 1;
    t1.tm_mday = d1;
    t1.tm_hour = h1;

    t2.tm_year = y2 - 1900;
    t2.tm_mon  = m2 - 1;
    t2.tm_mday = d2;
    t2.tm_hour = h2;

    time_t timeVal1 = mktime(&t1);
    time_t timeVal2 = mktime(&t2);

    int diffInSeconds = difftime(timeVal1, timeVal2);
    int diffInHours = diffInSeconds / 3600;

    return diffInHours;
}


UINT  KPlayer::GetLeftTime()
{
    return	m_nLeftTime;
}

void KPlayer::SendMSGroup()
{
	// 给客户端发消息
	PLAYER_MISSION_DATA	sData;
	sData.ProtocolType = s2c_playermissiondata;
	sData.m_nMissionGroup = Npc[m_nIndex].m_nMissionGroup;
	if (g_pServer && m_nNetConnectIdx != -1)
		g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sData, sizeof(PLAYER_MISSION_DATA));
}

void KPlayer::SendMSRank(TMissionLadderSelfInfo* SelfData, TMissionLadderInfo* RankData)
/*{
	int i;
	PLAYER_MISSION_RANKDATA sData;
	sData.ProtocolType = s2c_syncrankdata;
	sData.SelfData = *SelfData;

	for(i =0;i<MISSION_STATNUM;i++)
	{
		//memcpy(&sData.MissionRank[i], &RankData[i], sizeof(RankData[i]));
		memcpy(&sData.MissionRank[i], &RankData[i], sizeof(RankData[i]));
		///printf("--包大小 ：%u --\n",sizeof(PLAYER_MISSION_RANKDATA)); //1209	 //1024
	}

	if (g_pServer && m_nNetConnectIdx != -1)
		g_pServer->PackDataToClient(m_nNetConnectIdx, &sData, sizeof(PLAYER_MISSION_RANKDATA));	
}*/
{
    if (!SelfData || !RankData) {
        printf("SendMSRank Error: Invalid parameters (SelfData or RankData is NULL).\n");
        return;
    }

    PLAYER_MISSION_RANKDATA sData;
    sData.ProtocolType = s2c_syncrankdata;

    // Sao chep data ca nhan
    sData.SelfData = *SelfData;

    for (int i = 0; i < MISSION_STATNUM; i++) {
        // gui tung phan tu trong mang MissionRank
        sData.MissionRank = RankData[i];
        sData.m_nIndex = i;

        //printf("-- Sending MissionRank[%d] --\n", i);

        if (g_pServer && m_nNetConnectIdx != -1) {
            g_pServer->PackDataToClient(m_nNetConnectIdx, &sData, sizeof(PLAYER_MISSION_RANKDATA));
        }
		else 
		{
            printf("SendMSRank Error: Server is NULL or invalid connection index.\n");
        }
    }
}


//同步玩家的某种类型 给客户端
void   KPlayer::GetTypeData(int nType)
{

	if (nType==1)
	{
	  

	   int ngIsOpen,ngType,ngYinL,ngJinBi,ngBeiLv,ngDianShu;

	   g_GameSetTing.GetInteger("GuZiGameSet","GoldMissID_1",0,&ngIsOpen);
	   g_GameSetTing.GetInteger("GuZiGameSet","GoldMissID_2",0,&ngType);
	   g_GameSetTing.GetInteger("GuZiGameSet","GoldMissID_3",0,&ngYinL);
	   g_GameSetTing.GetInteger("GuZiGameSet","GoldMissID_4",0,&ngJinBi);
	   g_GameSetTing.GetInteger("GuZiGameSet","GoldMissID_5",0,&ngBeiLv);
	   g_GameSetTing.GetInteger("GuZiGameSet","GoldMissID_6",0,&ngDianShu);

	   PLAYER_SYNCALLBACK_GUZI nGuZi;
	   nGuZi.ProtocolType = s2c_plyerguzicallback;
	   nGuZi.m_nIsOpen        = g_GlobalMissionArray.GetMissionValue(ngIsOpen);
	   nGuZi.m_Type		      =	g_GlobalMissionArray.GetMissionValue(ngType);
	   nGuZi.m_nYinL		  =	g_GlobalMissionArray.GetMissionValue(ngYinL);
	   nGuZi.m_nJinBi		  =	g_GlobalMissionArray.GetMissionValue(ngJinBi);
	   nGuZi.m_nDianShu		  =	g_GlobalMissionArray.GetMissionValue(ngDianShu);
	   nGuZi.m_Beilv		  =	g_GlobalMissionArray.GetMissionValue(ngBeiLv);
		   
	   if (g_pServer)
	      g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&nGuZi, sizeof(PLAYER_SYNCALLBACK_GUZI));
	}

}




int  KPlayer::GetVipType()
{
	
    return	m_nVipType;

}

BOOL KPlayer::AddAndPayExtTime(long nTime)
{
	m_nLeftTime += nTime;
	if  (m_nLeftTime < 0)
	{
		m_nLeftTime = 0;
	}
	m_nChangeExtTime  += nTime;    //需要改变的是多少
	return TRUE;
}

BOOL KPlayer::AddAndPayExtPoint(long nPoint)
{

	m_nExtPoint       += nPoint;

   if  (m_nExtPoint < 0)
   {
       m_nExtPoint = 0;
   }

	m_nChangeExtPoint += nPoint;    //需要改变的是多少
	return TRUE;
}

//获取要改变的扩展点
long KPlayer::GetExtPointChanged()
{
	return m_nChangeExtPoint;
}

int KPlayer::ComItem( int nIdx1,int nIdx2,int nIdx3 ,int nKind)
{//提炼
	int nReg=0;
	char nMapName[64]={0};
	snprintf(nMapName, sizeof(nMapName), "%s", SubWorld[Npc[m_nIndex].m_SubWorldIndex].nWorldMapInfo[STR_MAP_NAME].c_str());
	nReg=ExeItemScriptA("\\script\\system\\orecomp.lua","maincompore",Npc[m_nIndex].Name,nMapName,nKind,nIdx1,nIdx2,nIdx3,0,0,0,0,0);
	//Item[nIdx1].GetBaseMagic(); //属性的行数
	return nReg;
}

//打造紫装 和 图谱
int KPlayer::ComDiyItem( int nId1,int nId2,int nId3,int nKind,int nMin )  //打造紫装装备 nMin为辅助物品额外数据
{
	int nIndex;
//开始提取属性石头
	if (nKind == 1)  
	{
		int nReg=0;
		char nMapName[64]={0};
		snprintf(nMapName, sizeof(nMapName), "%s", SubWorld[Npc[m_nIndex].m_SubWorldIndex].nWorldMapInfo[STR_MAP_NAME].c_str());

		nReg=ExeItemScriptA("\\script\\system\\itemcomp.lua","maincompitem",Npc[m_nIndex].Name,nMapName,nKind,nId1,nId2,nId3,nMin,0,0,0,0);
		
		return nReg;

	}
// 打造紫装孔
	else if (nKind == 2)  // 打造紫装
	{
		int nReg=0;
		char nMapName[64]={0};
		snprintf(nMapName, sizeof(nMapName), "%s", SubWorld[Npc[m_nIndex].m_SubWorldIndex].nWorldMapInfo[STR_MAP_NAME].c_str());

		nReg=ExeItemScriptA("\\script\\system\\itemcomp.lua","maincompitem",Npc[m_nIndex].Name,nMapName,nKind,nId1,nId2,nId3,nMin,0,0,0,0);
		
		return nReg;

	}
//  纹钢黄金
	else if (nKind == 4)  
	{
		int nPlace = Item[nId3].GetParticular() - 200; //详细类别

		nMin = nMin / 2;
		nMin += Item[nId2].GetLevel() + Item[nId3].GetLevel() - 9;  //属性矿石 和 神秘
		
		int nMax = nMin + Item[nId2].GetLevel();

		if (nMin <= 0)
		{
			nMin = 1;
		}
		
		if (::GetRandomNumber(nMin,nMax) < 3) 
		{
			m_ItemList.Remove(nId2);
			ItemSet.Remove(nId2);
			m_ItemList.Remove(nId3);
			ItemSet.Remove(nId3);
			return 0;					
		}
		
		int nSerise = Item[nId1].GetSeries();
		int nRoint = Item[nId1].IsRong();
		
		int nItemClass		= Item[nId1].GetGenre(); //道具种类
		int nDetailType		= Item[nId1].GetDetailType();
		int nParticularType	= Item[nId1].GetParticular();
		int nLevel			= Item[nId1].GetLevel();
		int nSeries			= Item[nId1].GetSeries();
		int nLuck			= Item[nId1].GetItemParam()->nLuck;

		int nItemLevel[2];
		
		KTabFile tabMagic;
		tabMagic.Load("\\Settings\\item\\magicattriblevel_rong.txt"); //熔炼的属性列表
		
		int nRow;
		char szTmp[8];
		snprintf(szTmp, sizeof(szTmp),"%d",Item[nId3].GetBaseMagic()); //属性矿石
		nRow = tabMagic.FindRow(szTmp); //找到魔法属性的这行
/*		for (int l = 0;l < tabMagic.GetHeight();l++)
		{
			tabMagic.GetInteger(l,1,0,&szTmp);
			if (szTmp == Item[nId3].GetBaseMagic())
			{
				nRow = l;
				break;
			}
		}*/

		int nAdd = ::GetRandomNumber(nMin,nMax) + ((Item[nId3].GetLevel() + Item[nId2].GetLevel())/2);
		if (nLuck < 0) nLuck = 0;
		
		nLuck += nMin ;
		if (nLuck > 10) nLuck = 10;
		if (nAdd > 10) nAdd = 10;
		int nMinRow; 
		tabMagic.GetInteger(nRow,"MAGATTRLVL_START",0,&nMinRow);
		nMinRow +=  nAdd;
		tabMagic.Clear();

		tabMagic.Load(TABFILE_MAGICATTRIB_PATH); //这个要修改
		int nMagic;
		tabMagic.GetInteger(nMinRow,Item[nId1].GetDetailType() + 15,0,&nMagic);//具体类别+13？得到对应的 装备类型 入 头 或靴子 等
		if (nMagic <= 0)//是否可以熔炼的
			return 0;
		tabMagic.Clear();
		for (int k = 0;k < 2;++k)
		{
			if (nPlace > k)
			{
				if (Item[nId1].GetRongAttribType(k) <= 0 || Item[nId1].GetRongAttribType(k) == Item[nId3].GetBaseMagic())
				{
					return 0;
				}
			}
			else if (nPlace == k &&  Item[nId1].GetRongAttribType(k) <= 0)
			{
				nItemLevel[k] = nMinRow;
				continue;
			}
			else if (nPlace == k && Item[nId1].GetRongAttribType(k) > 0)
			{
				return 0;
			}
			nItemLevel[k] = Item[nId1].GetItemParam()->nGeneratorLevel[k];
		}
		
		int nStackNum = 1;
		int nEnChance = Item[nId1].GetEnChance();
		
		// 产生装备
		nIndex = ItemSet.AddOther(nItemClass,nSeries,nLevel,nLuck,nDetailType,nParticularType,nItemLevel,0,0,1,0,0,nRoint);
	}
// 镶套紫装 
	else if (nKind == 3)   
	{   
        int nReg=0;
		char nMapName[64]={0};
		snprintf(nMapName, sizeof(nMapName), "%s", SubWorld[Npc[m_nIndex].m_SubWorldIndex].nWorldMapInfo[STR_MAP_NAME].c_str());
            nReg=ExeItemScriptA("\\script\\system\\itemcomp.lua","maincompitem",Npc[m_nIndex].Name,nMapName,nKind,nId1,nId2,nId3,nMin,0,0,0,0);

        return nReg;

    }     
//熔炼镶套 蓝装 紫装 黄装
   else if (nKind == 6)   
	{
		nMin = nMin / 2;
		nMin += Item[nId2].GetLevel() + Item[nId3].GetLevel() - 9;
		
		int nMax = nMin + Item[nId2].GetLevel();

		if (nMin <= 0)
		{
			nMin = 1;
		}
		
		if (::GetRandomNumber(nMin,nMax) < 3) 
		{
			m_ItemList.Remove(nId2);  //移除装备
			ItemSet.Remove(nId3);
			m_ItemList.Remove(nId3);
			ItemSet.Remove(nId3);
		//	CoreDataChanged(GDCNI_CONTAINER_OBJECT_CHANGED, UOC_ITEM_TAKE_WITH, 0);
			return 0;					
		}
		
		int nSerise = Item[nId1].GetSeries();
		int nRongmun = Item[nId1].IsRong();
		int npiont   =  Item[nId1].IsPurple();
		//int nBlue    =  Item[nId1].IsBlue();
		
		int nItemClass		= Item[nId1].GetGenre();
		int nDetailType		= Item[nId1].GetDetailType();
		int nParticularType	= Item[nId1].GetParticular();
		int nLevel			= Item[nId1].GetLevel();
		int nSeries			= Item[nId1].GetSeries();
		//int nRongmun        = Item[nId1].GetRongNum();//取属性数量值
		int nGoldID        = Item[nId1].GetGoldId();
		int nEnChance       = Item[nId1].GetEnChance();   //         m_CommonAttrib.nEnChance

		UINT nRandomID      = Item[nId1].GetItemParam()->uRandomSeed; // 随机种子
		int nVersionID      = Item[nId1].GetItemParam()->nVersion;
		int nLuck			= Item[nId1].GetItemParam()->nLuck;

        int nItemLevel[6]; 
		int nPlace;
        //确定属性石头类别
		if  (nGoldID>0)  
        nPlace = Item[nId3].GetParticular() - 200;  //属性矿石在 装备TXT中的位置-起始位置  黄装
        else if(npiont>0)
        nPlace = Item[nId3].GetParticular() - 200;  //属性矿石在 装备TXT中的位置-起始位置  紫装
        else
        nPlace = Item[nId3].GetParticular() - 200;  //属性矿石在 装备TXT中的位置-起始位置  蓝装
        //同步魔法属性
		for (int j = 0; j < 6; ++j)
		nItemLevel[j] = Item[nId1].m_GeneratorParam.nGeneratorLevel[j];
		//可溶链的 装备位置
		KTabFile tabMagic;
		tabMagic.Load(TABFILE_MAGICALEVEL_PATH);

		int nRow;
		char szTmp[8];
		snprintf(szTmp, sizeof(szTmp),"%d",Item[nId3].GetBaseMagic()); //获取魔法石的基本属性
		nRow = tabMagic.FindRow(szTmp);
/*		for (int l = 0;l < tabMagic.GetHeight();l++)
		{
			tabMagic.GetInteger(l,1,0,&szTmp);
			if (szTmp == Item[nId3].GetBaseMagic())
			{
				nRow = l;
				break;
			}
		}*/
         //产生值得计算
		int nAdd = ::GetRandomNumber(nMin,nMax) + ((Item[nId3].GetLevel() + Item[nId2].GetLevel())/2);
		if (nLuck < 0) nLuck = 0;
		
		nLuck += nMin ;
		if (nLuck > 10) nLuck = 10;
		if (nAdd > 10) nAdd = 10;
		int nMinRow; 
		tabMagic.GetInteger(nRow,"MAGATTRLVL_START",0,&nMinRow);//最小+随机值
		nMinRow +=  nAdd;
		tabMagic.Clear();
		tabMagic.Load(TABFILE_MAGICATTRIB_PATH);
		int nMagic;
		tabMagic.GetInteger(nMinRow,Item[nId1].GetDetailType() + 15,0,&nMagic);

		if (nMagic <= 0) //可出现该属性的装备类型
		   return 0;
		tabMagic.Clear();
		int nrItemLevel[6];

		/*/已经镶套了的相同属性的不能镶套
		for (int p = 0;p < 6;p++)
		{
          if (Item[nId1].GetAttribType(p) == Item[nId3].GetBaseMagic())  
               return 0; 
		  else if(Item[nId1].GetRongAttribType(p) == Item[nId3].GetBaseMagic())
               return 0; 
		}
        *///同步熔炼属性值 和 判断 属性石头的位置
		for (int k = 0;k < 6;++k)
		{
			if (nPlace > k)  //  位置和属性石头上的不相同不能镶套
			{
				if (Item[nId1].GetRongAttribType(k) <= 0 || Item[nId1].GetRongAttribType(k) == Item[nId3].GetBaseMagic())
				{
					return 0;  //有相同的 属性 镶套不了  要按顺序来镶套 否则失败
				}
			}
			else if (nPlace == k && Item[nId1].GetRongAttribType(k) <= 0)  //位置相同 并且没有属性的空位置
			{
				nrItemLevel[k] = nMinRow;   //设定最小值
				continue;
			}
			else if (nPlace == k && Item[nId1].GetRongAttribType(k) > 0)  //位置相同 但有属性
			{
				return 0;
			}
			nrItemLevel[k] = Item[nId1].m_GeneratorParam.nRGeneratorLevel[k];
		}
		
		int nStackNum = 1;
  
		nIndex = ItemSet.AddRong(nGoldID,nSeries,nLevel,nLuck,nDetailType,nParticularType,nItemLevel,nrItemLevel,NULL,nVersionID,nRandomID,1,nEnChance,nRongmun,0,0,NULL,0,0,0,0,0,0,TRUE);	
	}
// 打造 纹钢 打孔
    else if (nKind == 5)  
	{
		int nMax = Item[nId2].GetLevel() + 2;  //根据等级+2 确定打孔的最大数量
		int nMin = Item[nId2].GetLevel();
		
		if (nMax > 6) nMax = 6;
		if (nMin > 6) nMax = 6;
		
		if (nMax < 1) nMax = 1;
		if (nMin < 1) nMax = 1;
		
		//int nRoint = ::GetRandomNumber(nMin,nMax); //取随机数量--孔
		
		int nItemClass		= Item[nId1].GetGenre();
		int nDetailType		= Item[nId1].GetDetailType();
		int nParticularType	= Item[nId1].GetParticular();
		int nLevel			= Item[nId1].GetLevel();
		int nSeries			= Item[nId1].GetSeries();
		int nRongmun        = Item[nId1].GetRongNum();    //取属性数量值
		int nGoldmID        = Item[nId1].GetGoldId();
		int nEnChance       = Item[nId1].GetEnChance();   //m_CommonAttrib.nEnChance

		UINT nRandomID       = Item[nId1].GetItemParam()->uRandomSeed; // 随机种子
		int nVersionID      = Item[nId1].GetItemParam()->nVersion;
		int nLuck		    = Item[nId1].GetItemParam()->nLuck;

        int nItemLevel[6];   
		for (int j = 0; j < 6; ++j)//同步魔法属性
		nItemLevel[j] = Item[nId1].m_GeneratorParam.nGeneratorLevel[j];

			if (nRongmun<=0)  //改为可以熔炼的数量
			{
               //defFuncShowNormalMsgA("提示:nGoldmun 数据出错");
				return 0;
			}
        int nrItemLevel[6];  //熔炼
		   nrItemLevel[0] = 0;
		   nrItemLevel[1] = 0;
		   nrItemLevel[2] = 0;
		   nrItemLevel[3] = 0;
		   nrItemLevel[4] = 0;
		   nrItemLevel[5] = 0;	
		   nIndex = ItemSet.AddRong(nGoldmID,nSeries,nLevel,nLuck,nDetailType,nParticularType,nItemLevel,nrItemLevel,NULL,nVersionID,nRandomID,1,nEnChance,nRongmun,0,0,NULL,0,0,0,0,0,0,TRUE);	
	}

	if (nId1>0)
	{
	  m_ItemList.Remove(nId1);
	  ItemSet.Remove(nId1);
	}
	if (nId2>0)
	{
	  m_ItemList.Remove(nId2);
	  ItemSet.Remove(nId2);
	}
	if (nId3>0)
	{

	  m_ItemList.Remove(nId3);  
	  ItemSet.Remove(nId3);
	}
	if (nIndex <= 0)
	{						
		return 0;  //合成失败
	}

	/*if (m_ItemList.GetPlayerItemCount()>=MAX_ITEMLIST_CONUT)
	{
		ItemSet.Remove(nIndex);
		m_ItemList.msgshow("提示:<c=yellow>物品超限,请清理储物空间<c>!");
		return 0;
	}*/

	int	x, y;
	if (m_ItemList.CheckCanPlaceInEquipment(Item[nIndex].GetWidth(), Item[nIndex].GetHeight(), &x, &y)) //判断 包袱 的空间
	{
		m_ItemList.Add(nIndex, pos_equiproom, x, y); //包袱增加装备
	}
	else
	{
		m_ItemList.msgshow("Hanh trang day!");
		int	nIdx = m_ItemList.Hand();
		
		if (nIdx)
		{//手里有东西
			m_ItemList.Remove(nIdx);  //删除客户端的节点了
			ItemSet.Remove(nIdx);
		} 
		//同步客户端（手里增加新的节点）
		m_ItemList.Remove(nIndex);  //删除客户端的节点了
		ItemSet.Remove(nIndex);
		return 0;
	}
	return 1;  //成功
}


//获取摆摊物品的数量
void KPlayer::SendSellItemCount( int nIndex )
{
	if (nIndex <= 0 || nIndex >= MAX_PLAYER || Player[nIndex].m_nIndex <= 0)
		return;

	PLAYER_GET_COUNT pGetCount;
	pGetCount.ProtocolType = s2c_playergetcount;
	pGetCount.dwId = Npc[m_nIndex].m_dwID;

	int nCount = 0;
	for (int i = 0;i < MAX_PLAYER_ITEM;i++)
	{
		int nIdx = m_ItemList.m_Items[i].nIdx;
        if (nIdx <= 0 || Item[nIdx].GetLockItem()->IsLockItem() || Item[nIdx].GetTradePrice() <= 0 || m_ItemList.m_Items[i].nPlace != pos_equiproom)
			continue;

		nCount++;
	}

	pGetCount.nCount = nCount;

	if (nCount <= 0)
	{	//ban het do
		Npc[m_nIndex].m_BaiTan = 0; //设置结束摆摊状态
	}

	//printf("SendSellItemCount: Name: %s nCount: %d \n",Npc[m_nIndex].Name,nCount);
	g_pServer->PackDataToClient(Player[nIndex].m_nNetConnectIdx, (BYTE*)&pGetCount, sizeof(pGetCount));
}
//获取摆摊物品的数量
int KPlayer::GetTradeCount()
{
	int nCount = 0;
	for (int i = 0;i < MAX_PLAYER_ITEM;i++)
	{
		int nIdx = m_ItemList.m_Items[i].nIdx;
		if (nIdx <= 0 || Item[nIdx].GetLockItem()->IsLockItem() || Item[nIdx].GetTradePrice() <= 0 || m_ItemList.m_Items[i].nPlace != pos_equiproom)
		{
			continue;
		}
		nCount++;
	}

	return nCount;
}

#include <algorithm>  // cho std::min, std::max
#undef min
#undef max
//gia ban item
void KPlayer::SendSellItemInfo( int nTargetPlayer, int nPrcess, BOOL bUpdate)
{
	if (Player[nTargetPlayer].CheckTrading()) return;

    if (nTargetPlayer <= 0 || nTargetPlayer >= MAX_PLAYER || Player[nTargetPlayer].m_nIndex <= 0) return;

    // 1. Tap hop toan bo item thaa dieu kien vao mot vector tam
    std::vector<SViewSellItemInfo> allItems;
    allItems.reserve(MAX_PLAYER_ITEM);

    for (int i = 0; i < MAX_PLAYER_ITEM; i++)
    {
        int nIdx = m_ItemList.m_Items[i].nIdx;
        if (nIdx <= 0 || Item[nIdx].GetLockItem()->IsLockItem() || Item[nIdx].GetTradePrice() <= 0 || m_ItemList.m_Items[i].nPlace != pos_equiproom)
            continue;

        SViewSellItemInfo info;
        ZeroMemory(&info, sizeof(info));

        // Fill info tu item
        info.m_uPrice    = Item[nIdx].GetTradePrice();//m_ItemList.m_Items[i].nPrice;
        info.m_btX       = m_ItemList.m_Items[i].nX;
        info.m_btY       = m_ItemList.m_Items[i].nY;
        info.m_nIdx      = m_ItemList.m_Items[i].nIdx;
        info.m_Model     = m_ItemList.m_Items[i].nModel;
        info.m_Point     = Item[nIdx].GetPoint();
        info.m_Rongpiont = Item[nIdx].IsRong();
        info.m_EnChance  = Item[nIdx].GetEnChance();
        info.m_GoldId    = Item[nIdx].GetGoldId();
        info.m_IsBaiJin  = Item[nIdx].GetIsPlatima();
        info.m_StackNum  = Item[nIdx].GetStackNum();
        info.m_ID        = Item[nIdx].GetID();
        info.m_Genre     = Item[nIdx].GetGenre();
        info.m_Detail    = Item[nIdx].GetDetailType();
        info.m_Particur  = Item[nIdx].GetParticular();
        info.m_Series    = Item[nIdx].GetSeries();
        info.m_Level     = Item[nIdx].GetLevel();
        info.m_Luck      = Item[nIdx].m_GeneratorParam.nLuck;
        info.m_Time      = *Item[nIdx].GetTime();
        info.m_IsWhere   = Item[nIdx].GetIsWhere();
        info.m_IsCanUse  = Item[nIdx].GetIsCanUse();
        info.m_RandomSeed= Item[nIdx].m_GeneratorParam.uRandomSeed;
        info.m_Version   = Item[nIdx].m_GeneratorParam.nVersion;
        for (int j = 0; j < 6; j++)
        {
            info.m_MagicLevel[j]   = Item[nIdx].m_GeneratorParam.nGeneratorLevel[j];
            info.m_RongMagicLev[j] = Item[nIdx].m_GeneratorParam.nRGeneratorLevel[j];
        }

        allItems.push_back(info);
    }

	if (allItems.empty())
	{
		Npc[m_nIndex].m_BaiTan = 0;
		//Npc[m_nIndex].SetstrInfo(STR_SHOP_NAME,"Х b竛 h誸 !");
		//Npc[Player[m_nPlayerIndex].m_nIndex].SetstrInfo(STR_SHOP_NAME, "Х b竛 h誸 !");
		//Npc[m_nIndex].SetstrInfo(STR_SHOP_NAME, "Da Ban Het Do!");
		//Player[nTargetPlayer].m_ItemList.msgshow("<c=green>Nguoi choi nay da ban het do roi!<c>");
		//printf("da ban het do return;  %s - %s - %s   \n",Npc[Player[nTargetPlayer].m_nIndex].Name,Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name,Npc[Player[m_nPlayerIndex].m_nIndex].Name);
		return;
	}

    // 2. Chia nho va gui 
    const int MAX_PER_PACKET = 6;
    int total = (int)allItems.size();
    int sent  = 0;
    int packetIndex = 0;

    while (sent < total)
    {
        int chunkCount = std::min(MAX_PER_PACKET, total - sent);

        PLAYER_TRADE_ITEM_SYNC packet;
		packet.ProtocolType = s2c_viewsellitem;//cap nhat item va open shop
        packet.m_dwNpcID    = Npc[m_nIndex].m_dwID;
        packet.nItemCount   = chunkCount;

        // copy chunk
        for (int k = 0; k < chunkCount; k++) 
            packet.m_sInfo[k] = allItems[sent + k];

        int packetSize = sizeof(BYTE) + sizeof(UINT) + sizeof(int)
                       + sizeof(SViewSellItemInfo) * chunkCount;

        //printf("SendSellItemInfo: ProtocolType: %d packet #%d, size=%d, items [%d..%d]\n",packet.ProtocolType,
        //       packetIndex++, packetSize, sent, sent + chunkCount - 1);

        g_pServer->PackDataToClient(Player[nTargetPlayer].m_nNetConnectIdx, (BYTE*)&packet, packetSize);

        sent += chunkCount;
    }
}
//-------------------------------------------------------------------------
//	功能：发送自己装备在身上的装备信息给别人看
//-------------------------------------------------------------------------
void	KPlayer::SendEquipItemInfo(int nTargetPlayer, int nPrcess, BOOL bUpdate) //nTargetPlayer 要查看装备的人
{
	if (nTargetPlayer <= 0 || nTargetPlayer >= MAX_PLAYER || Player[nTargetPlayer].m_nIndex <= 0)
		return;
	
	int	nIdx, i ,j= 0;

	SViewItemInfo	*pInfo;
	VIEW_EQUIP_SYNC	sView;

	memset(sView.m_sInfo, 0, sizeof(sView.m_sInfo));

	sView.ProtocolType = s2c_viewequip;

	sView.m_dwNpcID = Npc[m_nIndex].m_dwID; // m_nIndex 为被查看装备的人索引
	
	for (i = 0; i < itempart_num ; i++)     // 装备位置itempart_num
	{
	    nIdx  = m_ItemList.m_EquipItem[i];

	    pInfo = &sView.m_sInfo[i];

		if (nIdx <= 0)
		{//如果这个部位为空 就继续下一个部位
			memset(pInfo, 0, sizeof(*pInfo));
			continue;
		}
		pInfo->m_Point        = Item[nIdx].GetPoint();
		pInfo->m_Rongpiont    = Item[nIdx].IsRong(); //熔炼
		pInfo->m_EnChance     = Item[nIdx].GetEnChance();
		pInfo->m_GoldId       = Item[nIdx].GetGoldId();
		//pInfo->m_StackNum     = Item[nIdx].GetStackNum();
		pInfo->m_IsBaiJin     = Item[nIdx].GetIsPlatima(); 
		pInfo->m_ID           = Item[nIdx].GetID();
		pInfo->m_Genre        = Item[nIdx].GetGenre();
		pInfo->m_Detail       = Item[nIdx].GetDetailType();
		pInfo->m_Particur     = Item[nIdx].GetParticular();
		pInfo->m_Series       = Item[nIdx].GetSeries();
		pInfo->m_Level        = Item[nIdx].GetLevel();
		pInfo->m_Luck         = Item[nIdx].m_GeneratorParam.nLuck;
		//pInfo->m_btPlace      = m_Items[i].nPlace; //同步目标容器
		//pInfo->m_btX          = m_Items[i].nX;     //同步位置
		//pInfo->m_btY          = m_Items[i].nY;
		pInfo->m_Time.bYear   = Item[nIdx].GetTime()->bYear;
		pInfo->m_Time.bMonth  = Item[nIdx].GetTime()->bMonth;
		pInfo->m_Time.bDay    = Item[nIdx].GetTime()->bDay;
		pInfo->m_Time.bHour   = Item[nIdx].GetTime()->bHour;
		pInfo->m_Time.bMin    = Item[nIdx].GetTime()->bMin;
		//pInfo->m_IsBang       = Item[nIdx].GetIsBang();      //是否绑定
		//pInfo->m_IsKuaiJie    = Item[nIdx].GetIsKuaiJie();   //是否快捷栏
		//pInfo->m_IsMagic      = Item[nIdx].GetIsMagic();     //是否有魔法属性
		//pInfo->m_SkillType    = Item[nIdx].GetSkillType();
		//pInfo->m_MagicID      = Item[nIdx].GetMagicID();
		//pInfo->m_IsWhere      = Item[nIdx].GetIsWhere();
		pInfo->m_IsCanUse   = Item[nIdx].GetIsCanUse();
		pInfo->m_RandomSeed = Item[nIdx].m_GeneratorParam.uRandomSeed;
		pInfo->m_Version    = Item[nIdx].m_GeneratorParam.nVersion;
		//pInfo->m_Durability = Item[nIdx].GetDurability();
		//sprintf(pInfo->m_WonName,Item[nIdx].GetWonName());  //所有者	
		
		for (j = 0; j < 6; j++)
		{
			pInfo->m_MagicLevel[j]=Item[nIdx].m_GeneratorParam.nGeneratorLevel[j]; 
			//pInfo->m_RongMagicLev[j]=Item[nIdx].m_GeneratorParam.nRGeneratorLevel[j];
			//pInfo->m_BaoMagicLev[j] = Item[nIdx].m_GeneratorParam.nBGeneratorLevel[j];
		}

		//for (j = 0; j < 7; ++j)
		//{
        //    pInfo->m_btJMagicLevel[j]= Item[nIdx].m_GeneratorParam.nJGeneratorLevel[j];
		//}
	}
	//向要查看装备的人 发送数据
	g_pServer->PackDataToClient(Player[nTargetPlayer].m_nNetConnectIdx,(BYTE*)&sView, sizeof(sView));	//sView
}

int KPlayer::SendLianItemInfo(const void *nData)
{
	if  (!nData) return false;
	tagItemAction* pInfo = (tagItemAction *)nData;
	//nGmRoleName 需要获取数据的人  nRoleName为发起链接的人
	//printf("---SendLianItemInfo ITEM操作:%s %s %u ----\n",pInfo->nGmRoleName,pInfo->nRoleName,pInfo->nId);

	int	nIdx,nItemidx,i;
	VIEW_LIAN_ITEM_SYNC	 sLView;  //协议
    memset(&sLView, 0, sizeof(sLView));//清空结构体;
	sLView.ProtocolType = c2s_item_synaction_result;
	nIdx      = m_ItemList.SearchID(pInfo->nId);
	nItemidx  = m_ItemList.SearchItemsIdx(pInfo->nId);

	if (nIdx <= 0 || nItemidx<=0 /*|| m_ItemList.m_Items[nItemidx].nLianJieFlg==0 || m_ItemList.m_Items[nItemidx].nPlace != pos_equiproom*/)
	{ 
		return FALSE;
	}

	sLView.m_LianJieFlg     = 1;//m_ItemList.m_Items[nItemidx].nLianJieFlg;
	/*sLView.m_btX            = m_ItemList.m_Items[nItemidx].nX;
	sLView.m_btY            = m_ItemList.m_Items[nItemidx].nY;
	sLView.m_nIdx           = m_ItemList.m_Items[nItemidx].nIdx;*/
	
	sLView.m_Point        = Item[nIdx].GetPoint();
	sLView.m_Rongpiont    = Item[nIdx].IsRong(); //熔炼
	sLView.m_EnChance     = Item[nIdx].GetEnChance();
	sLView.m_GoldId       = Item[nIdx].GetGoldId();
	sLView.m_IsBaijin     = Item[nIdx].GetIsPlatima();
	sLView.m_UseMap		  = Item[nIdx].GetUseMapIdx();
	sLView.m_UseKind	  = Item[nIdx].GetUseKind();
	sLView.m_StackNum     = Item[nIdx].GetStackNum();
	sLView.m_ID           = Item[nIdx].GetID();
	sLView.m_Genre        = Item[nIdx].GetGenre();
	sLView.m_Detail       = Item[nIdx].GetDetailType();
	sLView.m_Particur     = Item[nIdx].GetParticular();
	sLView.m_Series       = Item[nIdx].GetSeries();
	sLView.m_Level        = Item[nIdx].GetLevel();
	sLView.m_Luck         = Item[nIdx].m_GeneratorParam.nLuck;
	sLView.m_Time.bYear   = Item[nIdx].GetTime()->bYear;
	sLView.m_Time.bMonth  = Item[nIdx].GetTime()->bMonth;
	sLView.m_Time.bDay    = Item[nIdx].GetTime()->bDay;
	sLView.m_Time.bHour   = Item[nIdx].GetTime()->bHour;
	sLView.m_Time.bMin    = Item[nIdx].GetTime()->bMin;
	sLView.m_IsBang       = Item[nIdx].GetIsBang();   //是否绑定
	sLView.m_IsKuaiJie    = Item[nIdx].GetIsKuaiJie();   //是否快捷栏
	sLView.m_IsMagic      = Item[nIdx].GetIsMagic();   //是否有魔法属性
	sLView.m_SkillType    = Item[nIdx].GetSkillType();
	sLView.m_MagicID      = Item[nIdx].GetMagicID();
	sLView.m_IsWhere      = Item[nIdx].GetIsWhere();
	sLView.m_IsCanUse     = Item[nIdx].GetIsCanUse();
	snprintf(sLView.m_WonName, sizeof(sLView.m_WonName), "%s", pInfo->nGmRoleName);  //所有者
		
	int j;
	for (j = 0; j < 6; j++)//同步魔法属性
	{
		sLView.m_MagicLevel[j] = Item[nIdx].m_GeneratorParam.nGeneratorLevel[j]; 
		sLView.m_RongMagicLev[j] = Item[nIdx].m_GeneratorParam.nRGeneratorLevel[j];
	}
		
	for (j = 0; j < 7; j++)
	{
		sLView.m_JBLevel[j]      = Item[nIdx].m_GeneratorParam.nJGeneratorLevel[j];
	}
		
	sLView.m_RandomSeed = Item[nIdx].m_GeneratorParam.uRandomSeed;
	sLView.m_Version    = Item[nIdx].m_GeneratorParam.nVersion;
	sLView.m_Durability = Item[nIdx].GetDurability();
	ZeroMemory(sLView.m_ItmeInfo,sizeof(sLView.m_ItmeInfo));
	if (Item[nIdx].GetSItmeInfo() && strstr(Item[nIdx].GetSItmeInfo(),".lua"))  //strrchr
	{
		KLuaScript * pScript = (KLuaScript*)g_GetScript(Item[nIdx].GetSItmeInfo());//g_FileName2Id(
		if (pScript)
		{
			int nSafeIndex = 0;
			nSafeIndex=pScript->SafeCallBegin();
			//执行这个脚本的函数
			try
			{//说明
				pScript->CallFunction("GetItemDescription",1,"sdddddds", Item[nIdx].GetName(),Item[nIdx].GetItemBaseMagic(0),Item[nIdx].GetItemBaseMagic(1),Item[nIdx].GetItemBaseMagic(2),Item[nIdx].GetItemBaseMagic(3),Item[nIdx].GetItemBaseMagic(4),Item[nIdx].GetItemBaseMagic(5),"");
				const char * szType = lua_typename(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState));
				if (Lua_IsNumber(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState)) == 1)
				{ //Lua_PushNumber
					int nResult = (int)Lua_ValueToNumber(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState));
					_snprintf(sLView.m_ItmeInfo,sizeof(sLView.m_ItmeInfo)-1, "%d", nResult);  // int 类型
				   	//sprintf(sLView.m_ItmeInfo,"%d",nResult);
				} 
				else if (Lua_IsString(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState)) == 1)  // 字符串类型
				{  
					snprintf(sLView.m_ItmeInfo, sizeof(sLView.m_ItmeInfo), "%s", (char *)Lua_ValueToString(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState)));
				   	//sprintf(sLView.m_ItmeInfo,(char *)Lua_ValueToString(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState)));
				}   
				else
					//sprintf(sLView.m_ItmeInfo,"<c=Red>数据错误,请联系GM处理!<c>");
					_snprintf(sLView.m_ItmeInfo,sizeof(sLView.m_ItmeInfo)-1,"<c=Red>数据错误,请联系GM处理!<c>");
			}  
			catch (...)
			{
				_snprintf(sLView.m_ItmeInfo,sizeof(sLView.m_ItmeInfo)-1,"<c=Red>数据错误,请联系GM处理!<c>");
				//sprintf(sLView.m_ItmeInfo,"<c=Red>数据错误,请联系GM处理!<c>");
			}

			nSafeIndex=pScript->SafeCallBegin();
			pScript->SafeCallEnd(nSafeIndex);
		}
		else
		{
			_snprintf(sLView.m_ItmeInfo,sizeof(sLView.m_ItmeInfo)-1,"暂无数据");
			//sprintf(sLView.m_ItmeInfo,"暂无数据");
		}	
	}

	if (g_pGateWayClient)
		g_pGateWayClient->SendPackToServer((BYTE*)&sLView, sizeof(VIEW_LIAN_ITEM_SYNC));
    

	return true;
}

//链接  服务器
int KPlayer::SendLianItemInfo(int nTargetPlayer, int nItemDwID,int nPrcess, BOOL bUpdate,char *nShopName)  //nTargetPlayer  查看装备的索引
{
	if (nTargetPlayer <= 0 || nTargetPlayer >= MAX_PLAYER || Player[nTargetPlayer].m_nIndex <= 0)
		return FALSE;
	
	int	nIdx,nItemidx,i;
	VIEW_LIAN_ITEM_SYNC	 sLView;  //协议
    memset(&sLView, 0, sizeof(sLView));//清空结构体;
	sLView.ProtocolType = s2c_viewlianitem;
	nIdx      = m_ItemList.SearchID(nItemDwID);
	nItemidx  = m_ItemList.SearchItemsIdx(nItemDwID);

	if (nIdx <= 0 || nItemidx<=0 /*|| m_ItemList.m_Items[nItemidx].nLianJieFlg==0 || m_ItemList.m_Items[nItemidx].nPlace != pos_equiproom*/)
	{ 
		return FALSE;
	}

	sLView.m_LianJieFlg     = 1;//m_ItemList.m_Items[nItemidx].nLianJieFlg;
	/*sLView.m_btX            = m_ItemList.m_Items[nItemidx].nX;
	sLView.m_btY            = m_ItemList.m_Items[nItemidx].nY;
	sLView.m_nIdx           = m_ItemList.m_Items[nItemidx].nIdx;*/
	
	sLView.m_Point        = Item[nIdx].GetPoint();
	sLView.m_Rongpiont    = Item[nIdx].IsRong(); //熔炼
	sLView.m_EnChance     = Item[nIdx].GetEnChance();
	sLView.m_GoldId       = Item[nIdx].GetGoldId();
	sLView.m_IsBaijin     = Item[nIdx].GetIsPlatima();
	sLView.m_UseMap		  = Item[nIdx].GetUseMapIdx();
	sLView.m_UseKind	  = Item[nIdx].GetUseKind();
	sLView.m_StackNum     = Item[nIdx].GetStackNum();
	sLView.m_ID           = Item[nIdx].GetID();
	sLView.m_Genre        = Item[nIdx].GetGenre();
	sLView.m_Detail       = Item[nIdx].GetDetailType();
	sLView.m_Particur     = Item[nIdx].GetParticular();
	sLView.m_Series       = Item[nIdx].GetSeries();
	sLView.m_Level        = Item[nIdx].GetLevel();
	sLView.m_Luck         = Item[nIdx].m_GeneratorParam.nLuck;
	sLView.m_Time.bYear   = Item[nIdx].GetTime()->bYear;
	sLView.m_Time.bMonth  = Item[nIdx].GetTime()->bMonth;
	sLView.m_Time.bDay    = Item[nIdx].GetTime()->bDay;
	sLView.m_Time.bHour   = Item[nIdx].GetTime()->bHour;
	sLView.m_Time.bMin    = Item[nIdx].GetTime()->bMin;
	sLView.m_IsBang       = Item[nIdx].GetIsBang();   //是否绑定
	sLView.m_IsKuaiJie    = Item[nIdx].GetIsKuaiJie();   //是否快捷栏
	sLView.m_IsMagic      = Item[nIdx].GetIsMagic();   //是否有魔法属性
	sLView.m_SkillType    = Item[nIdx].GetSkillType();
	sLView.m_MagicID      = Item[nIdx].GetMagicID();
	sLView.m_IsWhere      = Item[nIdx].GetIsWhere();
	sLView.m_IsCanUse     = Item[nIdx].GetIsCanUse();
	snprintf(sLView.m_WonName, sizeof(sLView.m_WonName), "%s", Item[nIdx].GetWonName());  //所有者
		
	int j;
	for (j = 0; j < 6; j++)//同步魔法属性
	{
		sLView.m_MagicLevel[j] = Item[nIdx].m_GeneratorParam.nGeneratorLevel[j]; 
		sLView.m_RongMagicLev[j] = Item[nIdx].m_GeneratorParam.nRGeneratorLevel[j];
	}
		
	for (j = 0; j < 7; j++)
	{
		sLView.m_JBLevel[j]      = Item[nIdx].m_GeneratorParam.nJGeneratorLevel[j];
	}
		
	sLView.m_RandomSeed = Item[nIdx].m_GeneratorParam.uRandomSeed;
	sLView.m_Version    = Item[nIdx].m_GeneratorParam.nVersion;
	sLView.m_Durability = Item[nIdx].GetDurability();
	ZeroMemory(sLView.m_ItmeInfo,sizeof(sLView.m_ItmeInfo));
	if (Item[nIdx].GetSItmeInfo() && strstr(Item[nIdx].GetSItmeInfo(),".lua"))  //strrchr
	{
		KLuaScript * pScript = (KLuaScript*)g_GetScript(Item[nIdx].GetSItmeInfo());//g_FileName2Id(
		if (pScript)
		{
			int nSafeIndex = 0;
			nSafeIndex=pScript->SafeCallBegin();
			//执行这个脚本的函数
			try
			{//说明
				pScript->CallFunction("GetItemDescription",1,"sdddddds", Item[nIdx].GetName(),Item[nIdx].GetItemBaseMagic(0),Item[nIdx].GetItemBaseMagic(1),Item[nIdx].GetItemBaseMagic(2),Item[nIdx].GetItemBaseMagic(3),Item[nIdx].GetItemBaseMagic(4),Item[nIdx].GetItemBaseMagic(5),"");
				const char * szType = lua_typename(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState));
				if (Lua_IsNumber(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState)) == 1)
				{ //Lua_PushNumber
					int nResult = (int)Lua_ValueToNumber(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState));
					_snprintf(sLView.m_ItmeInfo,sizeof(sLView.m_ItmeInfo)-1, "%d", nResult);  // int 类型
				   	//sprintf(sLView.m_ItmeInfo,"%d",nResult);
				} 
				else if (Lua_IsString(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState)) == 1)  // 字符串类型
				{  
					snprintf(sLView.m_ItmeInfo, sizeof(sLView.m_ItmeInfo), "%s", (char *)Lua_ValueToString(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState)));
				   	//sprintf(sLView.m_ItmeInfo,(char *)Lua_ValueToString(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState)));
				}   
				else
					//sprintf(sLView.m_ItmeInfo,"<c=Red>数据错误,请联系GM处理!<c>");
					_snprintf(sLView.m_ItmeInfo,sizeof(sLView.m_ItmeInfo)-1,"<c=Red>数据错误,请联系GM处理!<c>");
			}  
			catch (...)
			{
				_snprintf(sLView.m_ItmeInfo,sizeof(sLView.m_ItmeInfo)-1,"<c=Red>数据错误,请联系GM处理!<c>");
				//sprintf(sLView.m_ItmeInfo,"<c=Red>数据错误,请联系GM处理!<c>");
			}

			nSafeIndex=pScript->SafeCallBegin();
			pScript->SafeCallEnd(nSafeIndex);
		}
		else
		{
			_snprintf(sLView.m_ItmeInfo,sizeof(sLView.m_ItmeInfo)-1,"暂无数据");
			//sprintf(sLView.m_ItmeInfo,"暂无数据");
		}	
	}

	if (g_pServer)
		g_pServer->PackDataToClient(Player[nTargetPlayer].m_nNetConnectIdx, (BYTE*)&sLView, sizeof(VIEW_LIAN_ITEM_SYNC));
    
	return TRUE;
}

void KPlayer::CreateRefItem(BYTE* pProtocol)
{
	PLAYER_CREATE_REF_ITEM_COMMAND* pCreateRefCmd = (PLAYER_CREATE_REF_ITEM_COMMAND*)pProtocol;
	int nItemID = m_ItemList.SearchID(pCreateRefCmd->ndwID);
	if (!nItemID)
	{
		printf("DEBUG, Khong the dua len phim(ko co vat pham trong hanh trang)\n");
		return;
	}

	m_ItemList.PlaceItemToRef(pCreateRefCmd->nIndex, nItemID);
}

void KPlayer::RemoveRefItem(BYTE* pProtocol)
{
	PLAYER_REMOVE_REF_ITEM_COMMAND* pRemoveRefCmd = (PLAYER_REMOVE_REF_ITEM_COMMAND*)pProtocol;
	m_ItemList.RemoveItemFromRef(pRemoveRefCmd->nIndex);
}

void KPlayer::TradeStart(BYTE* pProtocol)
{
	KLuaScript* pScript = (KLuaScript*)g_GetScript("\\script\\system\\player\\script_protocol.lua");
	if (pScript)
	{
		Lua_PushNumber(pScript->m_LuaState, m_nPlayerIndex);
		pScript->SetGlobalName(SCRIPT_PLAYERINDEX);
		
		Lua_PushNumber(pScript->m_LuaState, m_dwID);
		pScript->SetGlobalName(SCRIPT_PLAYERID);
		
		Lua_PushNumber(pScript->m_LuaState, Npc[Player[m_nPlayerIndex].m_nIndex].m_SubWorldIndex);
		pScript->SetGlobalName(SCRIPT_SUBWORLDINDEX);

		int nTopIndex = 0;
		int nReturn = 0;
		nTopIndex = pScript->SafeCallBegin();
		pScript->CallFunction("PermitTrade", 1, "");
		nTopIndex = Lua_GetTopIndex(pScript->m_LuaState);
		nReturn = (int) Lua_ValueToNumber(pScript->m_LuaState, nTopIndex);
		pScript->SafeCallEnd(nTopIndex);

		if(!nReturn) return;
	}

		PLAYER_START_TRADE *pST = (PLAYER_START_TRADE *)pProtocol;

		if (pST->m_bSet != 0 && GetTradeCount())
		{
			Npc[Player[m_nPlayerIndex].m_nIndex].m_BaiTan = 1;
			//char nstrName[32]={0};
		    Npc[Player[m_nPlayerIndex].m_nIndex].SetstrInfo(STR_SHOP_NAME,pST->m_Name);
			//strcpy(Npc[Player[nIndex].m_nIndex].ShopName,pST->m_Name);
		}
		else
			Npc[Player[m_nPlayerIndex].m_nIndex].m_BaiTan = 0;
}



#endif

// 物品相应的操作：
//     获得物品：1、从地上拣物品 2、脚本控制直接给 3、交易 4、player 之间赠
//               送(通过交易实现)
//         客户端鼠标点在物件上，然后客户端通过物件得出物品id、应该出现在装备栏或者
//         物品栏的位置或者跟随鼠标的计算，把计算结果发给服务器；服务器收到后首先判
//         断玩家与物品的位置关系，然后检查客户端的计算结果是否正确，然后进行相应的
//         处理，把处理结果发给客户端
//     物品的位置调整：客户端处理好物品来源位置、鼠标上物品目的位置，然后发给服务器，
//     服务器确认后通知客户端最终确定的操作（如果成功，把协议原样发回去）；如果有数
//     值等其他变化，另外通知客户端；
//     使用物品：1、吃药 2、装备（鼠标右键点击）
//         客户端向服务器端申请吃什么位置的药，同时客户端的相应数值先作相应变化，服
//         务器收到客户端申请后，处理完相应数据，通过player同步的方式通知客户端数据
//         的变化；
//         装备：鼠标右键点在一个装备上，自动处理装备上、卸下的位置信息，发给服务器，
//         服务器处理完后原样发回来；

//         交易的处理还没有

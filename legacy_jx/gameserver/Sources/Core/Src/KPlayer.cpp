#include "KCore.h"
#include "KPlayerTradeJournal.h"
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
#include "KPlayerAntiAddict.h"
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

#define		BASE_WALK_SPEED					5    //
#define		BASE_RUN_SPEED					10   //

#define		BASE_ATTACK_SPEED				0       //Ĺٶ
#define		BASE_CAST_SPEED					0       //ĳٶ
#define		BASE_VISION_RADIUS				120     //
#define		BASE_HIT_RECOVER				6

#define		TOWN_PORTAL_TIME				1800    //سʱ


KPlayer	*Player = NULL;//Player[MAX_PLAYER];

//=============================================================================
// V9 Anti-Addiction Implementation & Safe Cancel Trade
//=============================================================================
KPlayerAntiAddict g_PlayerAntiAddict[MAX_PLAYER];

void AntiAddict_OnPlayerActive(int nPlayerIndex)
{
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
        return;
    if (Player[nPlayerIndex].m_nNetConnectIdx < 0)
        return;

    g_PlayerAntiAddict[nPlayerIndex].dwOnlineTime++;
    g_PlayerAntiAddict[nPlayerIndex].dwTiredTime++;

    DWORD dwCurDegree = (DWORD)g_PlayerAntiAddict[nPlayerIndex].GetDegree();
    if (dwCurDegree != g_PlayerAntiAddict[nPlayerIndex].dwLastDegree)
    {
        g_PlayerAntiAddict[nPlayerIndex].dwLastDegree = dwCurDegree;
        AntiAddict_SendSyncPacket(nPlayerIndex);
    }
}

void AntiAddict_OnPlayerRelease(int nPlayerIndex)
{
    if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER)
    {
        g_PlayerAntiAddict[nPlayerIndex].Clear();
    }
}

BOOL AntiAddict_SendSyncPacket(int nPlayerIndex)
{
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
        return FALSE;
    if (!g_pServer || Player[nPlayerIndex].m_nNetConnectIdx < 0)
        return FALSE;

    BYTE pPacket[16];
    memset(pPacket, 0, sizeof(pPacket));
    pPacket[0] = 0xbd;
    *(DWORD*)(pPacket + 1) = g_PlayerAntiAddict[nPlayerIndex].dwOnlineTime;
    *(DWORD*)(pPacket + 5) = g_PlayerAntiAddict[nPlayerIndex].dwTiredTime;
    *(DWORD*)(pPacket + 9) = g_PlayerAntiAddict[nPlayerIndex].dwRelaxTime;

    return g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, (BYTE*)pPacket, 13);
}

void KPlayer::CancelTradeSafe()
{
    // 1. Validate both player indices and existence
    if (m_nPlayerIndex <= 0 || m_nPlayerIndex >= MAX_PLAYER)
        return;
    if (m_cTrade.m_nIsTrading != 1)
        return;

    int nDestIdx = m_cTrade.m_nTradeDest;
    if (nDestIdx <= 0 || nDestIdx >= MAX_PLAYER || nDestIdx == m_nPlayerIndex)
        return;

    // 2. Strict bidirectional link validation: Partner must also be trading with this player
    if (Player[nDestIdx].m_cTrade.m_nIsTrading != 1 || Player[nDestIdx].m_cTrade.m_nTradeDest != m_nPlayerIndex)
        return;

    // 3. Trade session validation: Both must belong to the exact same active session (session 0 is strictly forbidden)
    DWORD dwSession = m_cTrade.m_dwTradeSessionId;
    if (dwSession == 0 || Player[nDestIdx].m_cTrade.m_dwTradeSessionId != dwSession)
        return;

    // 4. Pre-condition rollback validation: Both must possess valid rollback snapshots
    // If either party lacks a valid snapshot, abort without touching either inventory
    if (!this->m_ItemList.HasValidTradeSnapshot(dwSession) ||
        !Player[nDestIdx].m_ItemList.HasValidTradeSnapshot(dwSession))
    {
        return;
    }

    // 5. ATOMIC STATE TRANSITION: Mark both trading inactive BEFORE ANY inventory modification
    m_cTrade.m_nIsTrading = 0;
    m_cTrade.m_nTradeState = 0;
    Player[nDestIdx].m_cTrade.m_nIsTrading = 0;
    Player[nDestIdx].m_cTrade.m_nTradeState = 0;

    // 6. Transactional rollback of both parties
    this->m_ItemList.RecoverTrade(dwSession);
    this->m_ItemList.ClearRoom(room_trade);
    this->m_ItemList.ClearRoom(room_trade1);
    this->m_cTrade.Release();
    this->m_cMenuState.RestoreBackupState(m_nPlayerIndex);

    Player[nDestIdx].m_ItemList.RecoverTrade(dwSession);
    Player[nDestIdx].m_ItemList.ClearRoom(room_trade);
    Player[nDestIdx].m_ItemList.ClearRoom(room_trade1);
    Player[nDestIdx].m_cTrade.Release();
    Player[nDestIdx].m_cMenuState.RestoreBackupState(nDestIdx);

    // 7. Synchronize cancellation to both clients
    TRADE_DECISION_SYNC sSync;
    sSync.ProtocolType = s2c_tradedecision;
    sSync.m_btDecision = 0;
    if (g_pServer)
    {
        if (m_nNetConnectIdx >= 0)
            g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sSync, sizeof(TRADE_DECISION_SYNC));
        if (Player[nDestIdx].m_nNetConnectIdx >= 0)
            g_pServer->PackDataToClient(Player[nDestIdx].m_nNetConnectIdx, (BYTE*)&sSync, sizeof(TRADE_DECISION_SYNC));
    }
}

int		g_nLastNetMsgLoop;

//-------------------------------------------------------------------------
//	ܣ�?
//-------------------------------------------------------------------------
KPlayer::KPlayer()
{
	m_nPlayerIndex = 0;
	Release();
}

//-------------------------------------------------------------------------
//	ܣ
//-------------------------------------------------------------------------
KPlayer::~KPlayer()
{
	Release();
}

//-------------------------------------------------------------------------
//	ܣ  ˳Ϸʱ
//-------------------------------------------------------------------------
void	KPlayer::Release()
{
#ifdef _SERVER
	if (m_nPlayerIndex > 0 && m_nPlayerIndex < MAX_PLAYER)
	{
		AntiAddict_OnPlayerRelease(m_nPlayerIndex);
	}
#endif
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
	m_CUnlocked = 0;  //ĬϽ
	
	m_dwLeadExp = 0;
	m_dwLeadLevel = 1;
	
	m_nPeapleIdx = 0;
	m_nObjectIdx = 0;
//	m_bWaitingPlayerFeedBack = false;
//	m_btTryExecuteScriptTimes = 0;	//
    m_nForbiddenFlag = 0; //Ĭǽ
	
#ifdef _SERVER
	m_IsFirstLogin       = 1;
	m_TempEnTerMapIndx = 0;          // ͻ˴ʱ½ͼ    
	m_TempKeyId = 0;                 // keyiD
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
	
	m_sPortalPos.m_nSubWorldId = 0;  //ص
	m_sPortalPos.m_nTime = 0;
	m_sPortalPos.m_nMpsX = 0;
	m_sPortalPos.m_nMpsY = 0;
	
	//m_pLastScriptCacheNode = NULL;
	m_dwLoginTime			= -1;
	//	m_uLastPingTime			= -1;
	m_bFinishLoading = FALSE;
	// The database-owned trade order is keyed by TxID, not this in-memory counter.
	m_uSaveGeneration = 0;
	m_uRoleSaveRevision = 0;
	m_bRoleSaveRevisionLoaded = false;
	m_bRoleRevisionProtocolSeen = false;
	m_bRoleSavePending = false;
	m_uRoleSavePendingNonce = 0;
	m_uRoleSavePendingExpected = 0;
	memset(m_szRoleSavePendingName,0,sizeof(m_szRoleSavePendingName));
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
//	ܣ�?m_nPlayerIndex (ֻPlayerSetInitе)
//-------------------------------------------------------------------------
void	KPlayer::SetPlayerIndex(int nNo)
{
	if (nNo < 0)
		m_nPlayerIndex = 0;
	else
		m_nPlayerIndex = nNo;
}

//-------------------------------------------------------------------------
//	ܣñʵ Player еλ
//-------------------------------------------------------------------------
int		KPlayer::GetPlayerIndex()
{
	return m_nPlayerIndex;
}

#define 	defMAX_SLEEP_TIME	10800     //ʱ // 3600
//-------------------------------------------------------------------------
//	ܣÿϷѭҪĶ
//-------------------------------------------------------------------------
void	KPlayer::Active(int  _minLimitLevel, int  _maxLimitLevel)
{

#ifdef _SERVER
	if (m_nNetConnectIdx == -1 || m_bExchangeServer)
		return;

	if ((g_SubWorldSet.GetGameTime() % 18) == 0)
	{
		AntiAddict_OnPlayerActive(m_nPlayerIndex);
	}

	/*if (m_nVipType==0)
	{
	  if (m_GameLeftTime>0)
	  { //
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
			  sprintf(nMsg,"ʾ:<c=gyellow>Ϸл %d<c> !",m_GameLeftTime>>4);
			  m_ItemList.msgshow(nMsg);
		  }
		}
	  }
	  else
		m_Isinvalid = TRUE;

	  if  (m_Isinvalid)
	  {//ʱ䵽�?
		  g_pServer->ShutdownClient(m_nNetConnectIdx);
	  }

	}*/

	if (m_nPlayerTime > 0)
	{
		m_nPlayerTime--;
		//printf("m_nPlayerTime: %d\n",m_nPlayerTime);
		if (m_nPlayerTime==0)
		{//ƺŵʱ�?
			//UseToolSkills(); //Ƴƺ
			for (int i=0;i<5;++i)
			{
				int nSkillD=Npc[m_nIndex].m_ExtSkill[i];
				if (nSkillD>0)
				{//мID,Ҫȥ״̬
					Npc[m_nIndex].m_ExtSkill[i]=0;
					Npc[m_nIndex].SetSkillLeftTime(nSkillD,0); //ͬͻɾ״̬				
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
				{//мID,Ҫȥ״̬
					Npc[m_nIndex].m_ExtSkill[i]=0;
					Npc[m_nIndex].SetSkillLeftTime(nSkillD,0); //ͬͻɾ״̬				
				}
			}
			Npc[m_nIndex].m_NpcTitle = 0;
			m_nPlayerTitleTime = 0; 
		}
	}

	//ʱĴ
	m_TimerTask.Activate(g_PlayerTimerCallBackFun);

	this->SendCurNormalSyncData();     //ѭͬ ֵ
	
	this->m_cChat.TimerAdd();
	
	//ɱʱ
	this->m_cPK.Active();	           //ѭɱ

	//this->m_cTong.Active();            //ѭս
	
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
	{//õͼ˵ȼ ִзɳõͼ
		if  (Npc[m_nIndex].m_Level<_minLimitLevel || Npc[m_nIndex].m_Level>_maxLimitLevel)
		{
			///mapPos pos;
			//Npc[m_nIndex].GetMpsPos(&pos);
		   ExecuteScript_Direct("\\script\\global\\global_move.lua", "global_move",true,"dsddd", m_nIndex,Npc[m_nIndex].Name,Npc[m_nIndex].m_SubWorldIndex,_minLimitLevel,_maxLimitLevel);
		}
	}


//
#else
	// ˵Ĵ
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
	// ӳĴ
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
					m_cTeam.UpdateamUI(); //ʾϢ
				}
				else
				{
					m_cTeam.m_sApplyList[i].m_dwTimer--;
					if ( !Npc[this->m_nIndex].SearchAroundID(m_cTeam.m_sApplyList[i].m_dwNpcID) )
					{
						m_cTeam.m_sApplyList[i].m_dwNpcID = 0;
						m_cTeam.m_sApplyList[i].m_dwTimer = 0;
						m_cTeam.UpdateInterface();
						m_cTeam.UpdateamUI(); //ʾϢ
					}
				}
			}
		}
	}
	
	++m_nSendMoveFrames;
	
	// ɱʱ
	this->m_cPK.Active();

	this->m_cTong.Active();            //ѭս

	
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
	case WM_MOUSEMOVE:   //�?
	case WM_MOUSEHOVER:  //�?
		if (wParam & MK_LBUTTON)
			OnButtonMove(LOWORD(lParam), HIWORD(lParam),
			(wParam & ~(MK_LBUTTON | MK_MBUTTON | MK_RBUTTON)), button_left);
		else if (wParam & MK_RBUTTON)
			OnButtonMove(LOWORD(lParam), HIWORD(lParam),
			(wParam & ~(MK_LBUTTON | MK_MBUTTON | MK_RBUTTON)), button_right);
		else
			OnMouseMove(LOWORD(lParam), HIWORD(lParam));
		break;
		
	case WM_LBUTTONUP:   //
		OnButtonUp(LOWORD(lParam), HIWORD(lParam), button_left);
		break;
		
	case WM_LBUTTONDOWN: //
		OnButtonDown(LOWORD(lParam), HIWORD(lParam), 
			(wParam & ~(MK_LBUTTON | MK_MBUTTON | MK_RBUTTON)), button_left);
		break;
		
	case WM_RBUTTONUP:   //Ҽ
		OnButtonUp(LOWORD(lParam), HIWORD(lParam), button_right);
		break;	
		
	case WM_RBUTTONDOWN: //Ҽ
		OnButtonDown(LOWORD(lParam), HIWORD(lParam),
			(wParam & ~(MK_LBUTTON | MK_MBUTTON | MK_RBUTTON)), button_right);
		break;
	}
}
//ʱ
void KPlayer::OnButtonDown(int x,int y, int Key, MOUSE_BUTTON nButton)
{
	m_MouseDown[(int)nButton] = TRUE;

	FindSelectNpc(x, y, relation_all);	 //ֵ NPC 
	FindSelectObject(x, y);	             //ֵ Ʒ
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
//ͻ�ʾ?
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
			//sprintf(nMsg,"ʾ: %s ..", Npc[m_nPeapleIdx].Name);
			//m_ItemList.ClientShowMsg(nMsg);
			CoreDataChanged(GDCNI_SWITCH_CURSOR, NULL, MOUSE_CURSOR_DIALOG);
		}
		else if (Npc[m_nPeapleIdx].m_Kind == kind_player && Npc[m_nPeapleIdx].m_BaiTan)
		{
			//char nMsg[64];
			//sprintf(nMsg,"ʾ: %s ̯..", Npc[m_nPeapleIdx].Name);
			//m_ItemList.ClientShowMsg(nMsg);
			CoreDataChanged(GDCNI_SWITCH_CURSOR, NULL, MOUSE_CURSOR_DIALOG);  //ָͼ
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
//Ϊ ߳
void KPlayer::ProcessMouse(int x, int y, int Key, MOUSE_BUTTON nButton)
{

	if (CheckTrading())
		return;

	if (m_ItemList.m_Hand > 0)
	{
		if (nButton == button_left) //
		{
			ThrowAwayItem();  //Ʒ Ʒ
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
	
	if ((Key & MK_SHIFT) || (nButton == button_right))  //ǿƹ
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
			//Object / Enemy / Ally ȼҵҪĶid
			
			
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
			//ָܱ�ǰλ޶Ļֱ�
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
			{//û�?
				return;
			}
			
			if (Npc[m_nIndex].m_HorseType)
			{//ʱ
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
			
			//޶ֱӷ
			if (!nTargetIdx)
			{
				Npc[m_nIndex].SendSerCommand(do_skill, Npc[m_nIndex].m_ActiveSkillID, nX, nY);		
				SendClientCmdSkill(Npc[m_nIndex].m_ActiveSkillID, nX, nY);
                return ;   //޶ֱ˳ 
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
	{//ûм̼ʱ
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
			m_nPeapleIdx = 0; //ֻԵ
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
//ϷΪ�״?
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
	{//Ѿ¼
		return TRUE;
	}
	
	return FALSE;
}

//ʱ½
void KPlayer::LoginTimeOut()
{
    const int nSessionSlot = m_Node.m_nIndex;
    if (Player && nSessionSlot > 0 && nSessionSlot < MAX_PLAYER &&
        this == &Player[nSessionSlot])
        V9_ReleaseAllSessions(nSessionSlot);
	// ֪ͨԼ
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
	
	if (UpdateDBPlayerInfo((BYTE *)pData,sizeof(m_GaemSaveBuffer),nIsShow) == -1)   //浵ݴ浵
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
	//ʱѾߡ�?
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
				printf("SubWorldͬͼʧ Fail.\n");        //ͬͼ
				break;
			}
			bRet = Npc[m_nIndex].SendSyncData(m_nNetConnectIdx);  // ͬNPCȻϢ
			if (!bRet)
			{
				printf("NPCϢͬʧ Fail.\n");
				break;
			}
			// ϢͬNPCݺʹͻҵǰڿͻ˵Npc
			CURPLAYER_SYNC	sSync;	// ͬǰҵϢͻˣװȣ
			sSync.ProtocolType = (BYTE)s2c_synonlineplayer;
			sSync.m_dwID       = Npc[m_nIndex].m_dwID;
			sSync.m_btLevel    = (WORD)Npc[m_nIndex].m_Level;
			sSync.m_btSex      = Npc[m_nIndex].m_nSex;
			sSync.m_btKind     = Npc[m_nIndex].m_Kind;
			sSync.m_btSeries   = Npc[m_nIndex].m_Series;
			sSync.m_wLifeMax   = Npc[m_nIndex].m_LifeMax;
			sSync.m_wNuqiMax   = Npc[m_nIndex].m_NuqiMax;  //ŭֵ
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
		//	sSync.m_btCUnlocked = this->m_CUnlocked; //ǷƵ
			sSync.m_btCurFaction    = this->m_cFaction.m_nCurFaction;
			sSync.m_btFirstFaction  = this->m_cFaction.m_nFirstAddFaction;
			sSync.m_nFactionAddTimes = this->m_cFaction.m_nAddTimes;
			sSync.m_wWorldStat       = (WORD)m_nWorldStat;   //
			sSync.m_wSectStat        = (WORD)m_nSectStat;     //
			sSync.m_nMoney1          = this->m_ItemList.GetsMon(room_equipment);
			sSync.m_nMoney2          = this->m_ItemList.GetsMon(room_repository);
			sSync.m_nXu              = this ->m_ItemList.GetxLient(room_equipment);
			//sSync.n_dwLoginTime = this ->m_dwLoginTime;
			printf("--CURPLAYER_SYNC size:%zu --\n",sizeof(CURPLAYER_SYNC));
			if (SUCCEEDED(g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sSync, sizeof(CURPLAYER_SYNC))))
			{
                char nmTong[32],nMapName[64]={0};		  
				sprintf(nmTong,"%d",m_nWorldStat); //
				snprintf(nMapName, sizeof(nMapName), "%s", SubWorld[Npc[m_nIndex].m_SubWorldIndex].nWorldMapInfo[STR_MAP_NAME].c_str());
                ExecuteScriptC("\\script\\system\\online.lua","OnLine",nmTong,nMapName,m_IsFirstLogin,0,0,0,0,true);  //ִ߽ű
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
			  SendSyncData_Skill();     //ͬ
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
//ͬݵͻ
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
		sSkill.m_sAllSkill[j].CurSkillExp= Npc[m_nIndex].m_SkillList.m_Skills[i].nCurSkillExp; //ǰ
		//---------------˿ʼӱ״̬--------------------------------------
		KSkill * pSkill =(KSkill *)g_SkillManager.GetSkill(Npc[m_nIndex].m_SkillList.m_Skills[i].SkillId, Npc[m_nIndex].m_SkillList.m_Skills[i].SkillLevel);
		if (pSkill) 
		{
		   int nReg=0;
		       nReg = pSkill->NewCanCastSkill(m_nIndex,-1,m_nIndex);
		   if (pSkill->GetSkillStyle() == SKILL_SS_PassivityNpcState && nReg==1)
		   {//üڱʱƵ Npc״̬  
			   pSkill->Cast(m_nIndex, -1, m_nIndex,-1);
		   } 
           else if  (pSkill->GetSkillStyle() == SKILL_SS_PassivityNpcState && nReg<=0)
		   {// ״̬ Ƶı 
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
		//m_ItemList.msgshow("ʾ:ͬбɹ!");
		return TRUE;
	}
	else
	{
		printf("-------[ͬ]player Packing skill sync data failed...--------\n");
		return FALSE;
	}
}
// ͬ --ѭ
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
	//sSync.m_shAngry = Npc[m_nIndex].m_CurrentNuQi;  //ǰŭֵ
	sSync.m_shAngryMax   = Npc[m_nIndex].m_CurrentNuQiMax;
	sSync.m_nForbiddenFlag = m_nForbiddenFlag;  //ͬ

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

//˳Ϸ�?ȡ)
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
	{//,Ӷ
		//printf("----Dau gia:%d ----\n",pCommand->m_BuyIdx);
		/*if (m_ItemList.GetPlayerItemCount()>=MAX_ITEMLIST_CONUT)
		{
			m_ItemList.msgshow("Dau gia:<c=yellow>Ʒ,ռ<c>!");
			return;
		}*/

		if (pCommand->m_BuyIdx>0 && pCommand->m_BuyIdx<=nMaxSellCount)
		{//ҿʼӶ

			if (iItemDataInfo.count(pCommand->m_BuyIdx)<=0)
			{
				m_ItemList.msgshow("Dau gia:<c=blue>Vat pham nay khong ton tai<c>");
				return;
			}

			if (pCommand->m_ItemNum<1)
				pCommand->m_ItemNum =1;

			int nItemClass = iItemDataInfo[pCommand->m_BuyIdx].iequipgen;
			int nIsStack   = iItemDataInfo[pCommand->m_BuyIdx].iparamj1;//ǷԵ
			UINT nIsSellOut = iItemDataInfo[pCommand->m_BuyIdx].isSellOut;
			UINT nStartTime= iItemDataInfo[pCommand->m_BuyIdx].addtime;
			UINT nEndTime  = iItemDataInfo[pCommand->m_BuyIdx].endtime;
			int nTimeOut   = iItemDataInfo[pCommand->m_BuyIdx].iparamj2;//Ƿ

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
			//sprintf(nDataKey,"%04Ե",pCommand->m_BuyIdx);
			sprintf(nDataKey, "kiem%04hiep", pCommand->m_BuyIdx);
			if (nStartTime > 0 && nEndTime>0)
			{//ʱƵ
				time_t rawtime;                  //һlong ʹ
				time (&rawtime);                 //ڵʱ��
				if  (rawtime>nEndTime)
				{
					iItemDataInfo[pCommand->m_BuyIdx].iparamj2 = 1;
					if (g_DbItemFile)
					{//дݿ
						ZCursorS *item_cursor = g_DbItemFile->search(nDataKey,strlen(nDataKey)+1);	//pName		strlen(szName) + 1  ݿѯ
						if (item_cursor)
						{
							TDBShopItemData* pItemData = (TDBShopItemData*)item_cursor->data;
							if (pItemData && item_cursor->size >= (int)sizeof(TDBShopItemData))
							{//ʼ޸
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
			{//װ
				if (pCommand->m_ItemNum!=1)
				{
					pCommand->m_ItemNum = 1;
					m_ItemList.msgshow("Dau gia: Muc nay khong the xep chong len nhau, khoi phuc du lieu goc!");
				}
			}
			else
			{//װ
				if  (!nIsStack && pCommand->m_ItemNum!=1)
				{//ܵӵġ
						m_ItemList.msgshow("Dau gia: Mat hang nay khong the duoc mua ket hop!");
						return;
				}
			}
			int nItemCount   = pCommand->m_ItemNum;
			int nItemPiceVal = iItemDataInfo[pCommand->m_BuyIdx].iSellPrice * nItemCount;
			int nItemPriceKind = iItemDataInfo[pCommand->m_BuyIdx].iPriceKind;

            if (nItemPriceKind==moneyunit_money)  //
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
			//----------------------------------ʼ  ִнű
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
			{//װװ
				 nIndex = ItemSet.AddOther(nItemClass, nSeries, nLevel, nLuck, nDetailType, nParticularType,nItemLevel, g_SubWorldSet.GetGameVersion(),nRandSeed,nItemCount,nEnChance,nPoint,\
				  0,0,0,0,0,nRongPoint,nRItemLevel,NULL,NULL,IsWhere,TRUE);
			}
			else
			{//װƽװ
			  m_ItemList.msgshow("Dau gia: Loi nGoldIdx vui long bao GM");
			  return;
			}

			if (nIndex <= 0)
			{
				return ;
			}
			m_ItemList.msgshow("Dau gia:<c=yellow> Item loi vui long bao GM<c>!");
			//ýڵѾ
			 time_t nthistime;
			 time(&nthistime);
			 iItemDataInfo[pCommand->m_BuyIdx].isSellOut = nthistime;

			 if (g_DbItemFile)
			 {//дݿ
				 ZCursorS *item_cursor = g_DbItemFile->search(nDataKey,strlen(nDataKey)+1);	//pName		strlen(szName) + 1  ݿѯ
				 if (item_cursor)
				 {
					 TDBShopItemData* pItemData = (TDBShopItemData*)item_cursor->data;
					 if (pItemData && item_cursor->size >= (int)sizeof(TDBShopItemData))
					 {//ʼ޸
						 pItemData->isSellOut = nthistime;
						 if (g_DbItemFile->add(nDataKey,strlen(nDataKey)+1,item_cursor->data,sizeof(TDBShopItemData)))   //pName strlen(szName) +1
						 {
							 printf("---[Dau gia] Cap nhat thanh cong du lieu cac mat hang da ban---/n");
						 }
					 }
				   g_DbItemFile->closeCursor(item_cursor);
				 }
			 }
			//ݿݡɾڵ㡡ߡɾ�?
			 //ʱԡ޴
			//----------------ʼǮ------------------------------
			 if (nItemPriceKind==moneyunit_money)  //
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
			{//װĻ,ǷƷ
				int nIdx=m_ItemList.IsHaveItem(Item[nIndex].GetGenre(),\
					Item[nIndex].GetDetailType(),\
					Item[nIndex].GetParticular(),\
					Item[nIndex].GetSeries(),\
					Item[nIndex].GetLevel(),pos_equiproom);

				if (nIdx>0 && Item[nIdx].IsStack() &&
					Item[nIndex].CanStack(nIdx) &&
					(Item[nIdx].GetStackNum()+nStackNum) <= MAX_STACKNUM) //ǷԵ--ͰǷͬ
				{//ĶԵ
					//Item[nIdx].SetStackNum(Item[nIdx].GetStackNum()+StackNum);             //õ
					//m_ItemList.SynPlayerItem(nIdx);                                        //ͬͻ
					//xep chong item mua
					printf("da mua item %d HaveItem: %d\n",Item[nIdx].GetGenre(),nIdx);
					nStackNum=Item[nIdx].GetStackNum()+nStackNum;
					m_ItemList.Remove(nIdx);//ɾĶ
					ItemSet.Remove(nIdx);
				}
			}*/
			//-------------------------------------------------------
			/*if (m_ItemList.GetPlayerItemCount()>=MAX_ITEMLIST_CONUT)
			{
				ItemSet.Remove(nIndex);
				m_ItemList.msgshow("Dau gia:<c=yellow>Ʒ,ռ<c>!");
				return;
			}*/

			int	x, y;
			if (m_ItemList.CheckCanPlaceInEquipment(Item[nIndex].GetWidth(), Item[nIndex].GetHeight(), &x, &y))
			{//ͬͻˣ�?
				m_ItemList.Add(nIndex,pos_equiproom,x,y,0,0,nStackNum);  //װ ͬװ--ͻ
			}
			else
			{
				m_ItemList.msgshow("Dau gia: Hanh trang dau roi");
				int	nIdx = m_ItemList.Hand();

				if (nIdx)
				{
					m_ItemList.Remove(nIdx);  //ɾͻ˵Ľڵ
					ItemSet.Remove(nIdx);
				}

				//ͬͻˣµĽڵ�?
				m_ItemList.Remove(nIndex);  //ɾͻ˵Ľڵ
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
	{// ɾͻ
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
//	ܣȡҽϷλ
//-------------------------------------------------------------------------
void KPlayer::GetLoginRevivalPos(int *lpnSubWorld, int *lpnMpsX, int *lpnMpsY)
{
	*lpnSubWorld = m_sLoginRevivalPos.m_nSubWorldID;
	*lpnMpsX     = m_sLoginRevivalPos.m_nMpsX;
	*lpnMpsY     = m_sLoginRevivalPos.m_nMpsY;
}

//ȡҵ
void KPlayer::GetDeathRevivalPos(int *lpnSubWorld, int *lpnMpsX, int *lpnMpsY)
{
	*lpnSubWorld = m_sDeathRevivalPos.m_nSubWorldID;
	*lpnMpsX     = m_sDeathRevivalPos.m_nMpsX;
	*lpnMpsY     = m_sDeathRevivalPos.m_nMpsY;
}

#endif

#ifdef _SERVER
void	KPlayer::SetTimer(DWORD nTime, int nTimerTaskId)					//ʱűʱ
{
	if (!nTime || !nTimerTaskId) return ;
	m_TimerTask.SetTimer(nTime, nTimerTaskId);
}

void	KPlayer::CloseTimer()							//رʱʱ
{
	m_TimerTask.CloseTimer();
}
#endif


#ifdef _SERVER
//------------------------------------------------------------------------------
//	ܣ趨�?ͼ  )
//------------------------------------------------------------------------------
void	KPlayer::SetRevivalPos(int nSubWorld, int nReviveId)
{
	if (nSubWorld >= 0)  //С0ʾõǰ
	{
        if (g_SubWorldSet.SearchWorld(nSubWorld)>0)  //ͼ
		    m_sLoginRevivalPos.m_nSubWorldID = nSubWorld;
		else
		{//
               int mSubWorld=53,mReviveId=19;        //80
			   //g_GameSetTing.GetInteger2("ServerConfig","PlayerRevPosId",&mSubWorld,&mReviveId);
			   mSubWorld =  _gameGlobalSet.PlayerInfo.mSubWorld;
			   mReviveId =  _gameGlobalSet.PlayerInfo.mReviveId;
               m_sLoginRevivalPos.m_nSubWorldID=mSubWorld;
               nReviveId=mReviveId;
		}
	}
	else
	{//ǰͼ
		m_sLoginRevivalPos.m_nSubWorldID = SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_SubWorldID; 
	}
//------------------Ƿͼ-------------	
	KIniFile IniFile;

	g_SetFilePath("\\settings");
	
	IniFile.Load("RevivePos.ini");
	char szSection[32],szKeyName[16];
	
	snprintf(szSection, 32, "%d", m_sLoginRevivalPos.m_nSubWorldID);
	snprintf(szKeyName, sizeof(szKeyName), "%d", nReviveId);
	
	if(!IniFile.IsSectionExist(szSection))
	{//ļͼĻ Ĭϵ
		int mSubWorld=53,mReviveId=19;        //80
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

	m_sDeathRevivalPos            = m_sLoginRevivalPos;  //㣨ṹ
}

//------------------------------------------------------------------------------
//	ܣ趨ʱ�?
//------------------------------------------------------------------------------
void KPlayer::SetDeathRevivalPos(int nSubWorld,int nXpos,int nYpos)
{
/*	if (nSubWorld >= 0)  //С0ʾõǰ
	{
        if (g_SubWorldSet.SearchWorld(nSubWorld)>0)  //ͼ
			m_sLoginRevivalPos.m_nSubWorldID = nSubWorld; = --maplist.ini ı
		else
		{//
			int mSubWorld=53,mReviveId=19;        //80
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
//	ܣյͻ˲ѯĳnpcϢͻ˷ͶϢ
//------------------------------------------------------------------------------
void	KPlayer::S2CSendTeamInfo(BYTE* pProtocol)
{
	PLAYER_APPLY_TEAM_INFO	*pApplyTeamInfo = (PLAYER_APPLY_TEAM_INFO*)pProtocol;
	
	//------------------------------ ѯϢ -----------------------------
	if (pApplyTeamInfo->m_dwTarNpcID == Npc[m_nIndex].m_dwID)
	{
		SendSelfTeamInfo();
		return;
	}
	
	//--------------------------- ѯ npc Ϣ -----------------------------
	PLAYER_SEND_TEAM_INFO	sTeamInfo;
	PLAYER_APPLY_TEAM_INFO_FALSE	sTeamInfoFalse;
	// ѰӦ npc id  player 
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
	// ѰҶ
	KTeam	*pTeam;
	if ( !Player[nPlayer].m_cTeam.m_nFlag )
		goto SendFalse;
	pTeam = &g_Team[Player[nPlayer].m_cTeam.m_nID];
	if (pTeam->m_nCaptain < 0 || !pTeam->IsOpen())
		goto SendFalse;
	
	// ͶϢ
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
	
	// 
	g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sTeamInfo, sizeof(PLAYER_SEND_TEAM_INFO));
	return;
	
	// ʧ֪ͨ
SendFalse:
	sTeamInfoFalse.ProtocolType = s2c_teamapplyinfofalse;
	g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sTeamInfoFalse, sizeof(PLAYER_APPLY_TEAM_INFO_FALSE));
	return;
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	ܣͻ˷ͶϢ
//-------------------------------------------------------------------------
void	KPlayer::SendSelfTeamInfo()
{
	if (m_cTeam.m_nFlag && m_cTeam.m_nID >= 0)	// ͶϢ
	{
		int		nNpcIndex;
		PLAYER_SEND_SELF_TEAM_INFO	sSelfInfo;
		sSelfInfo.ProtocolType = s2c_teamselfinfo;
		sSelfInfo.nTeamServerID = m_cTeam.m_nID;
		sSelfInfo.m_dwLeadExp = m_dwLeadExp;
		sSelfInfo.m_btState = g_Team[m_cTeam.m_nID].m_nState;
		
		// ӳ
		nNpcIndex = Player[g_Team[m_cTeam.m_nID].m_nCaptain].m_nIndex;
		sSelfInfo.m_dwNpcID[0] = Npc[nNpcIndex].m_dwID;
		sSelfInfo.m_btLevel[0] = (WORD)Npc[nNpcIndex].m_Level;
		snprintf(sSelfInfo.m_szNpcName[0], sizeof(sSelfInfo.m_szNpcName[0]), "%s", Npc[nNpcIndex].Name);
		// Ա
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
		// Ϣ
		g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sSelfInfo, sizeof(PLAYER_SEND_SELF_TEAM_INFO));
	}
	else	// Ϣ
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
//	ܣյͻ󴴽һ֧
//-------------------------------------------------------------------------
BOOL	KPlayer::CreateTeam(BYTE* pProtocol)
{
	return m_cTeam.CreateTeam(this->m_nPlayerIndex, (PLAYER_APPLY_CREATE_TEAM*)pProtocol);
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	ܣյͻ�šرձplayerǷԱ״̬
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
//	ܣյͻһ֧
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
	
	// ѰҶӳ npc id  player 
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
//	ܣӳ֪ͨĳnpcΪԱ
//-------------------------------------------------------------------------
BOOL	KPlayer::AddTeamMember(BYTE* pProtocol)
{
	// (ǷӡǷӳ�״̬Ա?
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
	
	// npc idplayer
	int		nPlayer, i;
	PLAYER_ACCEPT_TEAM_MEMBER	*pAccept = (PLAYER_ACCEPT_TEAM_MEMBER*)pProtocol;
	nPlayer = FindAroundPlayer(pAccept->m_dwNpcID);
	if (nPlayer < 0)
		return FALSE;
	// Էڲ״̬
	if (!Player[nPlayer].m_cTeam.m_bCanTeamFlag)
	{
		SHOW_MSG_SYNC	sMsg;
		sMsg.ProtocolType = s2c_msgshow;
		sMsg.m_wMsgID = enumMSG_ID_TARGET_CANNOT_ADD_TEAM;
		sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 ;
		g_pServer->PackDataToClient(m_nNetConnectIdx,(BYTE *)&sMsg, sMsg.m_wLength + 1);
		return FALSE;
	}
	// Ѿڶ
	if (Player[nPlayer].m_cTeam.m_nFlag)
		return FALSE;
	if (Player[nPlayer].m_cTeam.m_nApplyCaptainID != Npc[this->m_nIndex].m_dwID)
		return FALSE;

	// ԱͲټ ı״̬
	if (g_Team[m_cTeam.m_nID].m_nMemNum >= MAX_TEAM_MEMBER || g_Team[m_cTeam.m_nID].CheckFull())
	{
		g_Team[m_cTeam.m_nID].SetTeamClose();
		return FALSE;
	}

	// ӳԱ
	if (!g_Team[m_cTeam.m_nID].AddMember(nPlayer) )
		return FALSE;

	// ܶԱݴ
	Player[nPlayer].m_cTeam.Release();
	Player[nPlayer].m_cTeam.m_nFlag = 1;
	Player[nPlayer].m_cTeam.m_nFigure = TEAM_MEMBER;
	Player[nPlayer].m_cTeam.m_nID = m_cTeam.m_nID;
	Npc[Player[nPlayer].m_nIndex].SetCurrentCamp(Npc[m_nIndex].m_Camp);
	
	// ԱϢ
	PLAYER_TEAM_ADD_MEMBER	sAddMem;
	sAddMem.ProtocolType = s2c_teamaddmember;
	sAddMem.m_dwNpcID = Npc[Player[nPlayer].m_nIndex].m_dwID;
	sAddMem.m_btLevel = (WORD)Npc[Player[nPlayer].m_nIndex].m_Level;
	snprintf(sAddMem.m_szName, sizeof(sAddMem.m_szName), "%s", Npc[Player[nPlayer].m_nIndex].Name);
	
	// ӳϢ
	g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sAddMem, sizeof(PLAYER_TEAM_ADD_MEMBER));
	
	// ϶ԱϢ
	for (i = 0; i < MAX_TEAM_MEMBER; ++i)
	{
		if (g_Team[m_cTeam.m_nID].m_nMember[i] < 0 || g_Team[m_cTeam.m_nID].m_nMember[i] == nPlayer)
			continue;
		g_pServer->PackDataToClient(Player[g_Team[m_cTeam.m_nID].m_nMember[i]].m_nNetConnectIdx, (BYTE*)&sAddMem, sizeof(PLAYER_TEAM_ADD_MEMBER));
	}
	
	// ------------------------------------- ¶ԱϢ --------------------------
	int		nNpcIndex;
	PLAYER_SEND_SELF_TEAM_INFO	sSelfInfo;
	sSelfInfo.ProtocolType = s2c_teamselfinfo;
	sSelfInfo.nTeamServerID = m_cTeam.m_nID;
	sSelfInfo.m_dwLeadExp = Player[nPlayer].m_dwLeadExp;
	sSelfInfo.m_btState = g_Team[m_cTeam.m_nID].m_nState;
	// ӳ
	nNpcIndex = m_nIndex;
	sSelfInfo.m_dwNpcID[0] = Npc[nNpcIndex].m_dwID;
	sSelfInfo.m_btLevel[0] = (WORD)Npc[nNpcIndex].m_Level;
	snprintf(sSelfInfo.m_szNpcName[0], sizeof(sSelfInfo.m_szNpcName[0]), "%s", Npc[nNpcIndex].Name);
	// Ա
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
	// Ϣ
	g_pServer->PackDataToClient(Player[nPlayer].m_nNetConnectIdx, (BYTE*)&sSelfInfo, sizeof(PLAYER_SEND_SELF_TEAM_INFO));
	
	SHOW_MSG_SYNC	sMsg;
	sMsg.ProtocolType = s2c_msgshow;
	sMsg.m_wMsgID = enumMSG_ID_TEAM_SELF_ADD;
	sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1;
	sMsg.m_lpBuf = 0;
	g_pServer->PackDataToClient(Player[nPlayer].m_nNetConnectIdx,(BYTE *)&sMsg, sMsg.m_wLength + 1);
	
	// --------------------------------- ¶ԱϢ end --------------------------
	
	return TRUE;
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	ܣյͻ˶Ա֪ͨ�?
//-------------------------------------------------------------------------
void	KPlayer::LeaveTeam(BYTE* pProtocol)
{
	if (!m_cTeam.m_nFlag)
		return;
	if (this->m_cTeam.m_nFigure == TEAM_CAPTAIN && g_Team[m_cTeam.m_nID].IsOpen())
	{//ӳ�?
		g_Team[m_cTeam.m_nID].SetTeamClose();
	}
	
	// Ƕӳ��ϵͳϢӳ�ϵͳϢDeleteMember
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
	
	// ˶Ա�?
	g_Team[m_cTeam.m_nID].DeleteMember(GetPlayerIndex());
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	ܣյͻ˶ӳ֪ͨ߳ĳԱ
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
	// ˶Ա�?
	nPlayerNo = g_Team[m_cTeam.m_nID].m_nMember[nMemNo];
	g_Team[m_cTeam.m_nID].DeleteMember(nPlayerNo);
	
	// Ϣͻ
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
//	ܣյͻ˶ӳ֪ͨѶӳݽĳԱ
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
		// ӳƽʧܣԷͳ˧
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
		// ӳƽʧܣԷͳ˧
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
	// ıӪ
	Npc[Player[g_Team[m_cTeam.m_nID].m_nCaptain].m_nIndex].RestoreCurrentCamp();
	for (i = 0; i < MAX_TEAM_MEMBER; ++i)
	{
		nPlayerNo = g_Team[m_cTeam.m_nID].m_nMember[i];
		if (nPlayerNo < 0)
			continue;
		Npc[Player[nPlayerNo].m_nIndex].SetCurrentCamp(Npc[Player[g_Team[m_cTeam.m_nID].m_nCaptain].m_nIndex].m_Camp);
	}
	
	// ԱϢ
	PLAYER_TEAM_CHANGE_CAPTAIN	sChangeCaptain;
	sChangeCaptain.ProtocolType = s2c_teamchangecaptain;
	sChangeCaptain.m_dwCaptainID = pChange->m_dwNpcID;
	sChangeCaptain.m_dwMemberID = Npc[m_nIndex].m_dwID;
	// ӳϢ
	g_pServer->PackDataToClient(Player[g_Team[m_cTeam.m_nID].m_nCaptain].m_nNetConnectIdx, (BYTE*)&sChangeCaptain, sizeof(PLAYER_TEAM_CHANGE_CAPTAIN));
	// ԱϢ
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
//	ܣյͻ˶ӳɢ
//-------------------------------------------------------------------------
void	KPlayer::TeamDismiss(BYTE* pProtocol)
{
	if ( !m_cTeam.m_nFlag || m_cTeam.m_nFigure != TEAM_CAPTAIN)
		return;
	
	PLAYER_LEAVE_TEAM	sLeaveTeam;
	int		i;
	SHOW_MSG_SYNC	sMsg;
	
	// ɢϢ
	sMsg.ProtocolType = s2c_msgshow;
	sMsg.m_wMsgID = enumMSG_ID_TEAM_DISMISS;
	sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1;
	g_pServer->PackDataToClient(m_nNetConnectIdx,(BYTE *)&sMsg, sMsg.m_wLength + 1);
	
	// ӳϢ
	sLeaveTeam.ProtocolType = s2c_teamleave;
	sLeaveTeam.m_dwNpcID = Npc[m_nIndex].m_dwID;
	g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sLeaveTeam, sizeof(PLAYER_LEAVE_TEAM));
	// ԱϢ
	for (i = 0; i < MAX_TEAM_MEMBER; ++i)
	{
		g_pServer->PackDataToClient(Player[g_Team[m_cTeam.m_nID].m_nMember[i]].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
		
		sLeaveTeam.m_dwNpcID = Npc[Player[g_Team[m_cTeam.m_nID].m_nMember[i]].m_nIndex].m_dwID;
		g_pServer->PackDataToClient(Player[g_Team[m_cTeam.m_nID].m_nMember[i]].m_nNetConnectIdx, (BYTE*)&sLeaveTeam, sizeof(PLAYER_LEAVE_TEAM));
		// Աݸı
		Player[g_Team[m_cTeam.m_nID].m_nMember[i]].m_cTeam.m_nFlag = 0;
		Npc[Player[g_Team[m_cTeam.m_nID].m_nMember[i]].m_nIndex].RestoreCurrentCamp();;
	}
	// 
	g_Team[m_cTeam.m_nID].Release();
	// ӳݸı
	m_cTeam.m_nFlag = 0;
	Npc[m_nIndex].RestoreCurrentCamp();
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	ܣյͻ趨PK״̬
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
//	ܣյͻ
//-------------------------------------------------------------------------
void	KPlayer::SendFactionData(BYTE* pProtocol)
{
	SendFactionData();
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	ܣͻ˷
//-------------------------------------------------------------------------
void	KPlayer::SendFactionData()
{
	// ͻ˷Ϣ
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
	Npc[m_nIndex].GetMpsPos(&nX, &nY,&nMap);  //վλ
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
	  if (SubWorld[Npc[m_nIndex].m_SubWorldIndex].GetBarrier(nTmpX, nTmpY)) //ϰ
	     continue;
	  SubWorld[Npc[m_nIndex].m_SubWorldIndex].Mps2Map(nTmpX, nTmpY, &nR, &nMapX, &nMapY, &nOffX, &nOffY);
	  if (nR == -1) //Ϊ
	     continue;
	 if (SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_Region[nR].GetRef(nMapX, nMapY, obj_object))
	 // if (SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_Region[nR].HasObj(nMapX, nMapY))
	     continue;  //ж
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
//	ܣѰΧĳָnpc idplayer index
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
//	ܣжĳǷΧ
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
//	ܣѰΧĳָnpc dwidnpc index
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
//	ܣҵ½ʱԲ     ֵ   ----ûе
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
	m_nLucky = PlayerSet.m_cNewPlayerAttribute.m_nLucky[Series]; //ֵ
	m_nUpExp = 0;  //ɱ˻þ
	
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
//	ܣ㵱�?
//-------------------------------------------------------------------------
//void	KPlayer::CalcCurStrength()
//{	// Ҫ װܡ״̬ Ӱ
//	m_nCurStrength = m_nStrength;
//}

//-------------------------------------------------------------------------
//	ܣ㵱�?
//-------------------------------------------------------------------------
//void	KPlayer::CalcCurDexterity()
//{	// Ҫ װܡ״̬ Ӱ
//	m_nCurDexterity = m_nDexterity;
//}

//-------------------------------------------------------------------------
//	ܣ㵱�?
//-------------------------------------------------------------------------
//void	KPlayer::CalcCurVitality()
//{	// Ҫ װܡ״̬ Ӱ
//	m_nCurVitality = m_nVitality;
//}

//-------------------------------------------------------------------------
//	ܣ㵱�?
//-------------------------------------------------------------------------
//void	KPlayer::CalcCurEngergy()
//{	// Ҫ װܡ״̬ Ӱ
//	m_nCurEngergy = m_nEngergy;
//}

//-------------------------------------------------------------------------
//	ܣ㵱�?
//-------------------------------------------------------------------------
void	KPlayer::CalcCurLucky()
{	// Ҫ װܡ״̬ Ӱ
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
//	ܣӻ-----
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
		SetNpcPhysicsDamage();  // ˺
		// ѵǰͻ
		PLAYER_ATTRIBUTE_SYNC	sSync;
		sSync.ProtocolType = s2c_playersyncattribute;
		sSync.m_btAttribute = 0;
		sSync.m_nBasePoint = m_nStrength;
		sSync.m_nCurPoint = m_nCurStrength;
		sSync.m_nLeavePoint = m_nAttributePoint;
		g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sSync, sizeof(PLAYER_ATTRIBUTE_SYNC));
	}
}
//
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
//	ܣӻ----
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
		SetNpcAttackRating();    // 
		SetNpcDefence();         // 
		UpdataCurData();
		SetNpcPhysicsDamage();   // ˺
		
		// ѵǰͻ
		PLAYER_ATTRIBUTE_SYNC	sSync;
		sSync.ProtocolType = s2c_playersyncattribute;
		sSync.m_btAttribute = 1;
		sSync.m_nBasePoint = m_nDexterity;
		sSync.m_nCurPoint = m_nCurDexterity;
		sSync.m_nLeavePoint = m_nAttributePoint;
		g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sSync, sizeof(PLAYER_ATTRIBUTE_SYNC));
	}
}
//�?
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
//	ܣӻ  ----�?
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
		
		// ѵǰͻ
		PLAYER_ATTRIBUTE_SYNC	sSync;
		sSync.ProtocolType = s2c_playersyncattribute;
		sSync.m_btAttribute = 2;
		sSync.m_nBasePoint = m_nVitality;
		sSync.m_nCurPoint = m_nCurVitality;
		sSync.m_nLeavePoint = m_nAttributePoint;
		g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sSync, sizeof(PLAYER_ATTRIBUTE_SYNC));
	}
}
//
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
//	ܣӻ-----ڹ
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
		SetNpcPhysicsDamage();   //˺

		// ѵǰڹͻ
		PLAYER_ATTRIBUTE_SYNC	sSync;
		sSync.ProtocolType = s2c_playersyncattribute;
		sSync.m_btAttribute = 3;
		sSync.m_nBasePoint = m_nEngergy;
		sSync.m_nCurPoint = m_nCurEngergy;
		sSync.m_nLeavePoint = m_nAttributePoint;
		g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sSync, sizeof(PLAYER_ATTRIBUTE_SYNC));
	}
}
//ڹ
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
//	ܣı䵱�? nData С 0 ʱ)
//-------------------------------------------------------------------------
void	KPlayer::ChangeCurStrength(int nData)
{

	m_nCurStrength +=nData;
	if  (m_nCurStrength<0) m_nCurStrength=0;
	SetNpcPhysicsDamage();  //
	// ѵǰͻ
}

//-------------------------------------------------------------------------
//	ܣı䵱�? nData С 0 ʱ)
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
//	ܣı䵱�? nData С 0 ʱ)  //ӵǰ ֵ
//-------------------------------------------------------------------------
void	KPlayer::ChangeCurVitality(int nData)	    //״̬
{
/*	if (nData + m_nAttributePoint < 0)
return;
	*/
	m_nCurVitality += nData;  //⹦Ӷ�?
	if  (m_nCurVitality<0) m_nCurVitality=0;
	Npc[m_nIndex].AddCurLifeMax(PlayerSet.m_cLevelAdd.GetLifePerVitality(Npc[m_nIndex].m_Series) * nData);
	Npc[m_nIndex].AddCurStaminaMax(PlayerSet.m_cLevelAdd.GetStaminaPerVitality(Npc[m_nIndex].m_Series) * nData);	//
	
}

//-------------------------------------------------------------------------
//	ܣı䵱�? nData С 0 ʱ) ڹ
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
//	ܣӻ
//-------------------------------------------------------------------------
void	KPlayer::AddBaseLucky(int nData)
{
	m_nLucky += nData;
	if  (m_nLucky<0) m_nLucky=0;
	CalcCurLucky();
}

//-------------------------------------------------------------------------
//	ܣɵǰӦnpc˺(PhysicsDamage)---˺Ϊ---װǵ
//-------------------------------------------------------------------------
void	KPlayer::SetNpcPhysicsDamage()
{
/*	int		nMinDamage = 0, nMaxDamage = 0, mLeftskillidex = 0;
	mLeftskillidex = GetLeftSkill();                         //
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
//	ܣɵǰݼӦnpcĹ(AttackRating)
//-------------------------------------------------------------------------
void	KPlayer::SetNpcAttackRating()
{
	int		nRating;
	
	nRating = m_nDexterity * 4 - 28;
	Npc[m_nIndex].SetBaseAttackRating(nRating);
}

//-------------------------------------------------------------------------
//	ܣɵǰݼӦnpcķ
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
//	ܣɵǰݼӦnpcٶ
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
//	ܣɵǰݼӦnpcܲٶ
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
//	ܣӾ(ԭʼݣδҪӵӰ)
//-------------------------------------------------------------------------
#ifdef _SERVER
void	KPlayer::AddExp(int nExp, int nTarLevel,int nGoldType)
{
	if (nExp == 1)
		return;
	 
	if (!m_cTeam.m_nFlag)  //û
	{
		AddSelfExp(nExp, nTarLevel,nGoldType);
		return;
	}
	 
	// Ѿ
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
	
	// ӳ
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
	// Ա
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
 
	// ӳ�?
	if (nShareFlag[0])
	{
		nGetExp = (k * Npc[Player[g_Team[m_cTeam.m_nID].m_nCaptain].m_nIndex].m_Level)/nTotalLevel; //ܵȼ

		if (nGetExp <=0)
			nGetExp = 1;
		 
		nGetExp = k * Npc[Player[g_Team[m_cTeam.m_nID].m_nCaptain].m_nIndex].m_Level / nTotalLevel;
		if (nGetExp <= 0)
			nGetExp = 1;
		Player[g_Team[m_cTeam.m_nID].m_nCaptain].AddSelfExp(nExp * nGetExp / 100, nTarLevel, nGoldType);

		// LeadExp = ṩľ�?*  * ߵȼ / ߵȼͣ
		int		nLeadExp = nExp * k * PlayerSet.m_cLevelAdd.GetLeadExpShare(Npc[m_nIndex].m_Series) * (nTotalLevel - Npc[m_nIndex].m_Level) / (nTotalLevel * 10000);
		Player[g_Team[m_cTeam.m_nID].m_nCaptain].AddLeadExp(nLeadExp);

	}
	// Ա�?
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


/* ͣҪΪ˷㷵�?*/
/*struct DIV_T
{
    //  
    int quot; 
    //  
    int rem;
}; */

/* 
зʱҪһƫ
x / (2^k) = (x < 0 ? (x + (1 << k) - 1) : x) >> k
Ļͻ0ȡ-5 / 2 = -2.5 = -2
*/
/*struct DIV_T xDiv(int a, int b)
{
    //λһڱʱܼ
    enum {SIGN_BIT = 1 << ((sizeof(int) << 3) - 1)};
    //ǷΪ 
    int neg = a & SIGN_BIT;
    // ƫĻҪƫ 
    int bias = ((1 - !neg) << b) - (1 && neg);
	
    struct DIV_T tmp;
	
    tmp.quot = (a + bias) >> b;     //ab
    tmp.rem = a - (tmp.quot << b);
	
    return tmp;
	
}  */

//-------------------------------------------------------------------------
//	ܣӾ(ҪپĴ)
//  nExp ӵľ  nTarLevel npcĵȼ
//	 sourcesafe߻\document\System\ս�?doc
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

	if (m_nExp > m_nNextLevelExp)	// 
	{
			LevelUp();
			return;
	}
	else if (m_nExp == m_nNextLevelExp)						// ͬ
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
	 {//Ƿͬ ͻ
		 PLAYER_FINDPATH_STATE sSync;
		 sSync.ProtocolType = s2c_findpath_state;
		 sSync.m_btstate = m_IsInfindPathState;
		 if (g_pServer)
			 g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sSync, sizeof(PLAYER_FINDPATH_STATE));
	 }
 }
#endif
//-------------------------------------------------------------------------
//	ܣֱӾֵ
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
//	printf("-------[ֱӾ](%s)ԭʼ(%d)ʵӾ%d-------\n", Npc[m_nIndex].Name,nExp,nExp);//nGetExp*m_nUpExp/100);

	if (m_nExp >= m_nNextLevelExp)	// 
		LevelUp();
	else								// ͬ
	{
		PLAYER_EXP_SYNC	sSyncExp;
		sSyncExp.ProtocolType = (BYTE)s2c_playerexp;
		sSyncExp.m_nExp       = m_nExp;
		g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sSyncExp, sizeof(PLAYER_EXP_SYNC));
	}
#endif
}

//-------------------------------------------------------------------------
//	ܣһ
//-------------------------------------------------------------------------
void	KPlayer::LevelUp()
{
	m_nExp = 0;

	if (Npc[m_nIndex].m_Level >= MAX_LEVEL)
		return;

	Npc[m_nIndex].m_Level++;

	m_nNextLevelExp = PlayerSet.m_cLevelAdd.GetLevelExp(Npc[m_nIndex].m_Level);
	
	// -------------------------- ȼʱ ----------------------
	// δԵ
	m_nAttributePoint += PLAYER_LEVEL_ADD_ATTRIBUTE;   //һ5
	// δ似�?
	m_nSkillPoint     += PLAYER_LEVEL_ADD_SKILL;       //һ1㼼�?
	
	// ֵӰֵأȼԵ�Ӱ쵱ǰֵأװܡҩ?
	LevelAddBaseLifeMax();
	LevelAddBaseStaminaMax();
	LevelAddBaseManaMax();
	// Npc[m_nIndex].ResetLifeReplenish();
	
	// ֿԵı�?𡢱�? ԭʼ
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
	// װϢµǰ
	this->UpdataCurData();
	SetNpcPhysicsDamage();
	Npc[m_nIndex].m_CurrentCamp = nOldCurCamp;
	
	// ֵ
	Npc[m_nIndex].m_CurrentLife = Npc[m_nIndex].m_CurrentLifeMax;
	Npc[m_nIndex].m_CurrentStamina = Npc[m_nIndex].m_CurrentStaminaMax;
	Npc[m_nIndex].m_CurrentMana = Npc[m_nIndex].m_CurrentManaMax;
	
#ifdef _SERVER
	// ͻ˷
	// Լ
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
	// ѷ͵ȼ
	if (m_cTeam.m_nFlag)
	{
		PLAYER_TEAMMATE_LEVEL_SYNC	sLevel;
		sLevel.ProtocolType = s2c_teammatelevel;
		sLevel.m_btLevel = (WORD)Npc[m_nIndex].m_Level;
		sLevel.m_dwTeammateID = Npc[m_nIndex].m_dwID;
		// ӳ
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


// ͬͻ״̬ܵ
void	KPlayer::UpdataCurData(int nIsLoign,int nUnEquip) //Բ
{
	if (m_nIndex <= 0 || m_nIndex >= MAX_NPC)
		return;

	for (int j = 0 ; j< MAX_NPCSKILL;j++)
	{
		int nAdd = 	Npc[m_nIndex].m_SkillList.GetAddPoint(j); //ȡλõĶ⼼�?
		if (nAdd > 0)
		{
			nAdd = -nAdd;  //ڸ
			Npc[m_nIndex].m_SkillList.QeuipAddPoint(j,nAdd);             //øλüܵΪʵʼܵ
#ifdef _SERVER	
			Npc[m_nIndex].m_SkillList.IncreaseLevel(j,nAdd,FALSE,FALSE); //⼼ܵȼ״�?
			int nSkillid= Npc[m_nIndex].m_SkillList.GetSkillId(j);
			if (nUnEquip && nSkillid>0)
			{
				PLAYER_SKILL_LEVEL_SYNC	sSkill;
				sSkill.ProtocolType  = s2c_playerskilllevel;
				sSkill.m_nSkillID    = nSkillid;
				sSkill.m_nSkillLevel = Npc[m_nIndex].m_SkillList.GetCurrentLevelByIdx(j);
				sSkill.m_nSkillExp = Npc[m_nIndex].m_SkillList.GetCurSkillExp(j);//muc do luyen
				sSkill.m_nLeavePoint = Player[Npc[m_nIndex].GetPlayerIdx()].m_nSkillPoint;
				sSkill.m_nAddPoint   = Npc[m_nIndex].m_SkillList.GetAddPoint(j);	//ӵļܵ
				if (g_pServer)
					g_pServer->PackDataToClient(Player[Npc[m_nIndex].GetPlayerIdx()].m_nNetConnectIdx, (BYTE*)&sSkill, sizeof(PLAYER_SKILL_LEVEL_SYNC));
			}		
#endif
		}

		Npc[m_nIndex].m_SkillList.m_Skills[j].mAddPoint = 0;
		Npc[m_nIndex].m_SkillList.m_Skills[j].EnChance = 0;              // ܵļӳ
		Npc[m_nIndex].m_SkillList.m_Skills[j].nTempEnChance = 0;
	}

	ZeroMemory(&Npc[m_nIndex].m_CurrentPhysicsMagicDamageP, sizeof(KMagicAttrib));
	ZeroMemory(&Npc[m_nIndex].m_CurrentPhysicsMagicDamageV, sizeof(KMagicAttrib));

	int nCurbei=0; //,nFomoidx=Npc[m_nIndex].GetCurFuMoIdx()
	
	/*if 	(nFomoidx>-1)
	{
		if (Npc[m_nIndex].GetCurFoMoSkllLevel(nFomoidx)>=5) //ȡ�ǰĵ�
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
	Npc[m_nIndex].m_CurrentNuQiMax		= Npc[m_nIndex].m_NuqiMax;//ŭ
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

	Npc[m_nIndex].m_TempFireResist = 0;	        // Npcĵǰ
	Npc[m_nIndex].m_TempColdResist = 0;	        // Npcĵǰ
	Npc[m_nIndex].m_TempPoisonResist = 0;	    // Npcĵǰ
	Npc[m_nIndex].m_TempLightResist = 0;	    // Npcĵǰ�?
	Npc[m_nIndex].m_TempPhysicsResist = 0;	    // Npcĵǰ
	Npc[m_nIndex].m_CurrentHitNpcRecover = 0;
	Npc[m_nIndex].m_CurrentHitRank       = 0;

	Npc[m_nIndex].m_CurrentColdResist	= Npc[m_nIndex].m_ColdResist;  //ԭʼķ
	Npc[m_nIndex].m_CurrentDefend		= Npc[m_nIndex].m_Defend;
	Npc[m_nIndex].m_CurrentAddDexterity = 0;
	Npc[m_nIndex].m_CurrentFireResist	= Npc[m_nIndex].m_FireResist;
	Npc[m_nIndex].m_CurrentLifeReplenish= Npc[m_nIndex].m_LifeReplenish;
	Npc[m_nIndex].m_CurrentLifeReplenish_p	= 0;
	Npc[m_nIndex].m_CurrentManaReplenish_p  = 0;
	Npc[m_nIndex].m_CurrentHitRecover	= Npc[m_nIndex].m_HitRecover;  //Npc˶ʱ
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
	Npc[m_nIndex].m_CurrentMeleeDmgRet	= 0;        // ̷
	Npc[m_nIndex].m_CurrentHulueMeleeDmgRet=0;      // Խ̷ٵ�?
   	Npc[m_nIndex].m_CurrentHulueRangeDmgRet=0;      // Զ̷̳ٵ�?
	Npc[m_nIndex].m_CurrentMeleeDmgRetPercent = 0;  // ̷ٷֱ
	Npc[m_nIndex].m_CurrentTempSpeed=0;

//	Npc[m_nIndex].m_CurrentPiercePercent	= 0;
	Npc[m_nIndex].m_CurrentPoisonEnhance	= 0;

	Npc[m_nIndex].m_CurrentRangeDmgRet	= 0;         //Զ̷
	Npc[m_nIndex].m_CurrentRangeDmgRetPercent	= 0; //Զ̷ٷֱ
	Npc[m_nIndex].m_CurrentRangeEnhance	= 0;
	Npc[m_nIndex].m_CurrentSlowMissle	= 0;         //ӵ
	Npc[m_nIndex].m_CurrentStaminaStolen	= 0;
	Npc[m_nIndex].m_CurrentAddPhysicsDamage = 0;      //⹦�?
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
	Npc[m_nIndex].m_CurrentdanggeRate = 0;  //
	Npc[m_nIndex].m_CurrentzhongjiRate = 0; //ػ
	 Npc[m_nIndex].m_CurrentcjdanggeRate	= 0;               //�?
	 Npc[m_nIndex].m_CurrentcjzhongjiRate	= 0;               //ػ
	 Npc[m_nIndex].m_Currentsorbdamage	= 0;                   //˺
	 Npc[m_nIndex].m_Currentsorbdamage_v=0;
	 Npc[m_nIndex].m_Currenadddamagev   = 0;
	 Npc[m_nIndex].m_Currenadddamagep   = 0;
	 Npc[m_nIndex].m_Currentpoisonres	= 0;                   //=ԶԷ:#d1-%
	 Npc[m_nIndex].m_Currentfireres	= 0;                       //=ԶԷ:#d1-%
	 Npc[m_nIndex].m_Currentlightingres	= 0;                   //=ԶԷ׷:#d1-%
	 Npc[m_nIndex].m_Currentphysicsres	= 0;                   //=ԶԷշ:#d1-%
	 Npc[m_nIndex].m_Currentcoldres	= 0;                       //=ԶԷ:#d1-%
	 Npc[m_nIndex].m_Currentallres	= 0; 
	 Npc[m_nIndex].m_Currentnopkvalue	= 0;                   //=PKֵ:#d1+%
	 Npc[m_nIndex].m_Currentbossdamage	= 0;                   //=Իƽboss˺<c=orange>#d1+%<c>
	 Npc[m_nIndex].m_Currentelementsenhance	= 0;               //=ǿֵ#d1-㡣ǿеĿ�?
     Npc[m_nIndex].m_Currentelementsresist	= 0;               //=ֵ#d1-㡣еĿ�?
	 Npc[m_nIndex].m_Currentskillenhance=0;                    //ܵļӳ
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
	 Npc[m_nIndex].m_CurrentautoReviverate=0;                  //
	 Npc[m_nIndex].m_CurrentAddmagicphysicsDamageP= 0;// ǰհٷֱ
	 Npc[m_nIndex].m_CurrentPoisondamagereturnV=0;
	 Npc[m_nIndex].m_PoisonDamageReturn=0;
	 Npc[m_nIndex].m_Currentbaopoisondmax_p=0;
	 Npc[m_nIndex].m_ReturnSkill=0;
	 Npc[m_nIndex].m_IgnoreSkill=0;
	 Npc[m_nIndex].m_CurrentReturnResist=0;
	 Npc[m_nIndex].m_CurrentCreatnpcv=0;
	 Npc[m_nIndex].m_CurrentAllJiHuo=0; //Ƿȫ
	 Npc[m_nIndex].m_CurrentCreatStatus=0;
	 Npc[m_nIndex].m_CurrentAttackRatingEnhancep=0;
	 Npc[m_nIndex].m_CurrentAttackRatingEnhancev=0;
	 Npc[m_nIndex].m_CurrentIgnorenAttacRating  =0;
	 Npc[m_nIndex].m_Me2metaldamage_p=0;              //=Խϵ˺ӣ#d1+%
	 Npc[m_nIndex].m_Metal2medamage_p=0;              //=Խϵ˺#d1-%
	 Npc[m_nIndex].m_Me2wooddamage_p=0;              //=ľϵ˺ӣ#d1+%
	 Npc[m_nIndex].m_Wood2medamage_p=0;              //=ľϵ˺#d1-%
	 Npc[m_nIndex].m_Me2waterdamage_p=0;              //=ˮϵ˺ӣ#d1+%
	 Npc[m_nIndex].m_Water2medamage_p=0;              //=ˮϵ˺#d1-%
	 Npc[m_nIndex].m_Me2firedamage_p=0;              //=Իϵ˺ӣ#d1+%
	 Npc[m_nIndex].m_Fire2medamage_p=0;              //=Իϵ˺#d1-%
	 Npc[m_nIndex].m_Me2earthdamage_p=0;              //=ϵ˺ӣ#d1+%
	 Npc[m_nIndex].m_Earth2medamage_p=0;              //=ϵ˺#d1-%
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

	 ReCalcEquip();         //¼ϵװ
	 ReCalcState(nIsLoign); //¼㼼ܵ״�?
}

void KPlayer::ReCalcEquip()
{
	_ASSERT(m_nIndex > 0 && m_nIndex < MAX_NPC);
	
	for (int i = 0; i < itempart_num; ++i)
	{
		int nIdx = m_ItemList.m_EquipItem[i];
		if (!nIdx)
			continue;

		if (Item[nIdx].GetIsCanUse()==0)  //õ
		    continue;

		int nActive = m_ItemList.GetEquipEnhance(i);  //ȡ

		if (m_ItemList.m_bActiveSet)  //ǻƽװ  ȫ
		{
			nActive = 3;              //ĬϵĻƽ
		}
//ƥ
		if (i != itempart_horse || Npc[m_nIndex].m_bRideHorse)  //Ĳλ  
		{
			int nActiveInfo[2];
			
			ZeroMemory(nActiveInfo,sizeof(nActiveInfo));
			
			if (Item[nIdx].IsBlue())
			{//װ
				nActiveInfo[0] = 1;
				nActiveInfo[1] = Item[nIdx].GetIsWhere();
				
			} 
			else if  (Item[nIdx].IsPurple())
			{//װ
				nActiveInfo[0] = 2;
				nActiveInfo[1] = Item[nIdx].GetIsWhere();
			} 
			else if  (Item[nIdx].IsGold())
			{//ǻװ
				nActiveInfo[0] = 3;
				nActiveInfo[1] = Item[nIdx].GetLevel();//ȼ
			} 
			else
			{
				nActiveInfo[0] = 0;
				nActiveInfo[1] = 0;
			}
			
			if (Item[nIdx].GetIsCanUse()==1)   //ǿʹõ
			{     
			   Item[nIdx].ApplyMagicAttribToNPC(&Npc[m_nIndex], nActive,nActiveInfo);
			}
			else if (Item[nIdx].GetIsCanUse()==0 && Item[nIdx].GetGenre()==item_equip && Item[nIdx].GetDetailType()==equip_mask) 
			{//
               Item[nIdx].ApplyMagicAttribToNPC(&Npc[m_nIndex], nActive,nActiveInfo);
			}

		}
	}
}

void KPlayer::ReCalcState(int nIsLoign)
{
	_ASSERT(m_nIndex > 0 && m_nIndex < MAX_NPC);
	
	Npc[m_nIndex].ReCalcStateEffect();        //״̬Ч

	Npc[m_nIndex].m_SkillList.ReEnChance();   //üӳ
	
	if (!nIsLoign)
	   Npc[m_nIndex].NpcNewInitJinMaiVal();   //þ״̬
	       
	//Npc[m_nIndex].ReSkillEnhance(m_nLeftSkillID,1); //öԹܵļӳ!
}

//-------------------------------------------------------------------------
//	ܣͳ
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
		// 
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
//	ܣԭʼһ
//-------------------------------------------------------------------------
void KPlayer::LevelAddBaseLifeMax()
{
	Npc[m_nIndex].m_LifeMax += PlayerSet.m_cLevelAdd.GetLifePerLevel(Npc[m_nIndex].m_Series);
	Npc[m_nIndex].m_CurrentLifeMax = Npc[m_nIndex].m_LifeMax;
}

//-------------------------------------------------------------------------
//	ܣһ
//-------------------------------------------------------------------------
void KPlayer::LevelAddBaseStaminaMax()
{
	Npc[m_nIndex].m_StaminaMax += PlayerSet.m_cLevelAdd.GetStaminaPerLevel(Npc[m_nIndex].m_Series);
	Npc[m_nIndex].m_CurrentStaminaMax = Npc[m_nIndex].m_StaminaMax;
}

//-------------------------------------------------------------------------
//	ܣһ
//-------------------------------------------------------------------------
void KPlayer::LevelAddBaseManaMax()
{
	Npc[m_nIndex].m_ManaMax += PlayerSet.m_cLevelAdd.GetManaPerLevel(Npc[m_nIndex].m_Series);
	Npc[m_nIndex].m_CurrentManaMax = Npc[m_nIndex].m_ManaMax;
}

//-------------------------------------------------------------------------
//	ܣıӪ
//-------------------------------------------------------------------------
void	KPlayer::ChangePlayerCamp(int nCamp)
{
	if (nCamp < camp_begin || nCamp >= camp_num)
		return;
	Npc[m_nIndex].m_Camp = (NPCCAMP)nCamp;
}

#ifndef _SERVER
//-------------------------------------------------------------------------
//	ܣѯĳnpcڶϢ
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
//	ܣ
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
//	ܣѯĶ
//-------------------------------------------------------------------------
void	KPlayer::ApplySelfTeamInfo()
{
	ApplyTeamInfo(Npc[m_nIndex].m_dwID);
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	ܣ�?
//-------------------------------------------------------------------------
BOOL	KPlayer::ApplyCreateTeam()//char *lpszTeamName)
{
	return m_cTeam.ApplyCreate();//lpszTeamName);
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	ܣӳ�šرնǷԱ״?
//-------------------------------------------------------------------------
BOOL	KPlayer::ApplyTeamOpenClose(BOOL bFlag)
{
	if ( !m_cTeam.m_nFlag )					// δ
		return FALSE;
	if (m_cTeam.m_nFigure != TEAM_CAPTAIN)	// Ƕӳ
		return FALSE;
	if (this->CheckTrading())	// ڽ״̬
		return FALSE;
	
	if (bFlag)
	{
		if (PlayerSet.m_cLeadExp.GetMemNumFromLevel(m_dwLeadLevel) <= g_Team[0].m_nMemNum)	// ͳ˧
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
//	ܣĳ
//	int nNpcIndex ĿӳڿͻNpcеλ
//-------------------------------------------------------------------------
void	KPlayer::ApplyAddTeam(int nNpcIndex)
{
	if (this->CheckTrading())
		return;
	if (m_cTeam.m_nFlag)					// ѾӣĶ
		return;
	
	// ͬӪڿӣֲܼ˶�˿Լ?
	if (Npc[nNpcIndex].m_Camp == camp_begin && Npc[m_nIndex].m_Camp != camp_begin)
		return;
	//	if (Npc[nNpcIndex].m_Camp != Npc[m_nIndex].m_Camp)	// Ӫͬ
	//		return;
	
	// жĿӳǲǴڶ�״?
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
//	ܣ֪ͨĳnpcΪԱ
//-------------------------------------------------------------------------
void	KPlayer::AcceptTeamMember(DWORD dwNpcID)
{
	// ж״̬ ȱٶnpc�״̬?not end
	if ( !m_cTeam.m_nFlag || m_cTeam.m_nFigure != TEAM_CAPTAIN)
	{
		ApplySelfTeamInfo();
		return;
	}
	if (dwNpcID == 0)
		return;
	
	// б
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
	// ûҵ
//	if (i >= MAX_TEAM_APPLY_LIST)
//		return;
	
	// 
	PLAYER_ACCEPT_TEAM_MEMBER	sAcceptTeam;
	sAcceptTeam.ProtocolType = c2s_teamacceptmember;
	sAcceptTeam.m_dwNpcID = dwNpcID;
	if (g_pClient)
		g_pClient->SendPackToServer(&sAcceptTeam, sizeof(PLAYER_ACCEPT_TEAM_MEMBER));
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	ܣӳɾбеĳnpc
//-------------------------------------------------------------------------
void	KPlayer::TeamDropApplyOne(DWORD dwNpcID)
{
	if (dwNpcID == 0)
		return;
	// б
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
//	ܣ֪ͨ�?
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
//	ܣӳ֪ͨ߳ĳԱ
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
//	ܣӳԼĶӳݽĶԱ
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
//	ܣӳɢ
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
//	ܣ�رpk
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
//	ܣ
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
//	ܣͻ˷
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
		Npc[m_nIndex].SetChatInfo(pMsg->szName, lpszSentence, nLength);  //Լ ֵ m_szChatBuffer
		//char m_szChatBuffer[256]={0};
		//Npc[m_nIndex].GetstrInfo(STR_CHATBUFF_CLIENT,m_szChatBuffer);
		m_cChat.SendSentence(pMsg,Npc[m_nIndex].m_szChatBuffer);        //͸
	}
	else if (pMsg->eGenre == MSG_G_CMD)	// not end
	{

	}
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	ܣӳĳһĵ(0=Strength 1=Dexterity 2=Vitality 3=Engergy)
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
//	ܣĳ
//-------------------------------------------------------------------------
BOOL	KPlayer::ApplyAddSkillLevel(int nSkillID, int nAddPoint)
{
	int nListidx = Npc[m_nIndex].m_SkillList.FindSame(nSkillID);
	if (!nListidx) //Ƿ
		return FALSE;
	if (m_nSkillPoint < nAddPoint || nAddPoint + Npc[m_nIndex].m_SkillList.GetLevelByListidx(nListidx) >=MAX_SKILLLEVEL)
		return FALSE;
	
	PLAYER_ADD_SKILL_POINT_COMMAND	sSkill;
	sSkill.ProtocolType = c2s_playerapplyaddskillpoint;
	sSkill.m_nSkillID = nSkillID;
	sSkill.m_nAddPoint= nAddPoint; //ӵļܵ
	
	if (g_pClient)
		g_pClient->SendPackToServer(&sSkill, sizeof(PLAYER_ADD_SKILL_POINT_COMMAND));
	
	return TRUE;
}
#endif
//ͻ 
#ifndef _SERVER
//-------------------------------------------------------------------------
//	ܣʹĳƷҼƷֻڳҩ
//-------------------------------------------------------------------------
BOOL	KPlayer::ApplyUseItem(int nItemID, ItemPos SrcPos)
{
	if (this->CheckTrading())
		return FALSE;

	int nRet = m_ItemList.UseItem(nItemID,SrcPos.nX,SrcPos.nY);  //ͬʹƷ
	if (nRet == 0)
		return FALSE;
	
	if (nRet == REQUEST_EQUIP_ITEM)         //Ʒװ---Ҽװ-
	{//װ
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
	else if (nRet == REQUEST_EAT_MEDICINE)  // ҩƷ
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
//	ܣͻobjĳƷϢ
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
	{//Ǯ
		sPickUp.ProtocolType = c2s_playerpickupitem;
		sPickUp.m_nObjID = Object[nObjIndex].m_nID;
		sPickUp.m_btPosType = 0;
		sPickUp.m_btPosX = 0;
		sPickUp.m_btPosY = 0;

		 if (Npc[m_nIndex].m_AutoplayId==1)
		 {//һ״̬			
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
	{//Ʒ
		ItemPos	sItemPos;
		if (FALSE == m_ItemList.SearchPosition(Object[nObjIndex].m_nItemWidth, Object[nObjIndex].m_nItemHeight, &sItemPos))
		{
			if (FALSE == m_ItemList.SearchItemPosition(Object[nObjIndex].m_nItemWidth, Object[nObjIndex].m_nItemHeight, &sItemPos,room_equipmentex))
			{
				if (Npc[m_nIndex].m_AutoplayId==1)
				{ //һ״̬
					Object[nObjIndex].m_AttackerDwid=Npc[m_nIndex].m_dwID;
					Object[nObjIndex].m_IsHaveAttack=1;
					Object[nObjIndex].m_AttackerTime=18*60*10;
					//ʼ߷
					ApplyRoomInfo(1);
				}
				
				m_ItemList.ClientShowMsg("Tip: Chuyen luu tui m_ExItemId that bai, khong gian khong du!");
				return;
			}
			
			int nItemEX = Npc[m_nIndex].m_ExItemId; //ĸǷ
			
			if (nItemEX==0)
			{
				if (Npc[m_nIndex].m_AutoplayId==1)
				{ //һ״̬
					Object[nObjIndex].m_AttackerDwid=Npc[m_nIndex].m_dwID;
					Object[nObjIndex].m_IsHaveAttack=1;
					Object[nObjIndex].m_AttackerTime=18*60*10;
					//ʼ߷
					ApplyRoomInfo(1);
				}
				
				m_ItemList.ClientShowMsg("Tip: Chuyen luu tui m_ExItemId that bai, tui m_ExItemId chua duoc mo!");
				return;
			}	

		}
	
		 if (Npc[m_nIndex].m_AutoplayId==1)
		 {//һ״̬			
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
          sprintf(msg,":ֹ:%d,dwidx:%d",Object[nObjIndex].m_IsHaveAttack,Object[nObjIndex].m_AttackerDwid);
          m_ItemList.ClientShowMsg(msg);
*/
		sPickUp.ProtocolType = c2s_playerpickupitem;
		sPickUp.m_nObjID = Object[nObjIndex].m_nID;
		sPickUp.m_btPosType = sItemPos.nPlace;
		sPickUp.m_btPosX = sItemPos.nX;  //λ
		sPickUp.m_btPosY = sItemPos.nY;  //λ
	}
	
	if (g_pClient)
		g_pClient->SendPackToServer(&sPickUp, sizeof(PLAYER_PICKUP_ITEM_COMMAND));
}
#endif


#ifndef _SERVER
//-------------------------------------------------------------------------
//	ܣͻobjĳǮϢ
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
	//m_ItemList.ClientShowMsg("ԹǮ");
	PLAYER_PICKUP_ITEM_COMMAND	sPickUp; //
	if (Object[nObjIndex].m_nKind == Obj_Kind_Money) //ڵϵĶΪ==Ǯ
	{
		sPickUp.ProtocolType = c2s_playerpickupitem; //Эʶ
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
//	ܣͻobjĳװϢ
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
	PLAYER_PICKUP_ITEM_COMMAND	sPickUp; //
	if (Object[nObjIndex].m_nKind == Obj_Kind_Item) //ڵϵĶΪ==װ
	{
		ItemPos	sItemPos;

		if (FALSE == m_ItemList.SearchPosition(Object[nObjIndex].m_nItemWidth, Object[nObjIndex].m_nItemHeight, &sItemPos) )
		{//
			if (FALSE == m_ItemList.SearchItemPosition(Object[nObjIndex].m_nItemWidth, Object[nObjIndex].m_nItemHeight, &sItemPos,room_equipmentex))
			{
				if (Npc[m_nIndex].m_AutoplayId==1)
				{ //һ״̬
					Object[nObjIndex].m_AttackerDwid=Npc[m_nIndex].m_dwID;
					Object[nObjIndex].m_IsHaveAttack=1;
					Object[nObjIndex].m_AttackerTime=18*60*10;
					//ʼ߷
					ApplyRoomInfo(1);
				}

				m_ItemList.ClientShowMsg("ʾ:תĸʧ,ռ䲻㣡");
				return;
			}

			int nItemEX = Npc[m_nIndex].m_ExItemId; //ĸǷ

          	if (nItemEX==0)
			{
				if (Npc[m_nIndex].m_AutoplayId==1)
				{ //һ״̬
					Object[nObjIndex].m_AttackerDwid=Npc[m_nIndex].m_dwID;
					Object[nObjIndex].m_IsHaveAttack=1;
					Object[nObjIndex].m_AttackerTime=18*60*10;
					//ʼ߷
					ApplyRoomInfo(1);
				}

				m_ItemList.ClientShowMsg("ʾ:תĸʧ,ĸδ");
				return;
			}	
		}

         if (Npc[m_nIndex].m_AutoplayId==1)
		 {//һ״̬			
			if (m_Autoplay.nShiQuNum>m_Autoplay.nVal && m_Autoplay.nShiQuNum%m_Autoplay.nVal==0)
			{
                Object[nObjIndex].m_AttackerDwid=Npc[m_nIndex].m_dwID;
                Object[nObjIndex].m_IsHaveAttack=1;
			    Object[nObjIndex].m_AttackerTime=18*60*10;
				m_Autoplay.nShiQuNum=0;
				m_ItemList.ClientShowMsg("<c=yellow><c>:ȡƷʱ,ȡȡ!");
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
		sPickUp.m_btPosType = sItemPos.nPlace;  //Ŀ
		sPickUp.m_btPosX = sItemPos.nX;         //Ŀλ
		sPickUp.m_btPosY = sItemPos.nY;
	}
	
	if (g_pClient)
		g_pClient->SendPackToServer(&sPickUp, sizeof(PLAYER_PICKUP_ITEM_COMMAND));
}
#endif


#ifndef _SERVER
//-------------------------------------------------------------------------
//	ܣͻobjϢ   ִнű
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
// DownPos ϵƷĵǰ꣬UpPos Ʒŵϵ
void	KPlayer::MoveItem(ItemPos DownPos, ItemPos UpPos,int nIsComp)
{
	if (this->CheckTrading() && DownPos.nPlace == pos_traderoom)  //
	{
		if (this->m_cTrade.m_nTradeLock)
			return;
		
		// Ʒ
		//int	nHandIdx = m_ItemList.Hand();

	/*	if (nHandIdx && Item[nHandIdx].GetGenre() == item_task ||(Item[nHandIdx].GetTime()->bYear>0 && Item[nHandIdx].GetIsWhere()==0) || Item[nHandIdx].GetIsBang())
		{
			KSystemMessage	sMsg;
			sprintf(sMsg.szMessage, MSG_TRADE_TASK_ITEM);//ֹ
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
			return;
		}*/
	}
	
	if (!CheckTrading() && DownPos.nPlace == pos_immediacy)//  Ŀ
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
	else if (!CheckTrading() && DownPos.nPlace == pos_dazao)  //ѾͬƷ
	{
		int nHandIdx = m_ItemList.Hand();
		if (nHandIdx)
		{//ϼ�?
			//g_CompEquip.
			if (nIsComp==-1) //
			{
				//m_ItemList.ClientShowMsg("ʯϼ");
				//return;
			 }
			else if (nIsComp==-2) //
			{
				//m_ItemList.ClientShowMsg("Կʯϼ");
				//return;
			 }
		    else if (nIsComp==1) //
			 {
				 //m_ItemList.ClientShowMsg("ʯϼ");
				 //return;
			 }
			else if (nIsComp==2) //ȡ
			{
				 int nGenre=Item[nHandIdx].GetGenre();                //
                 int nPart=Item[nHandIdx].GetDetailType();            //ϸ
				 int nParticular= Item[nHandIdx].GetParticular();    //

				 /*if (DownPos.nX==1) //װ
				 {
                     int nPartTyep[15],nReg=0;

					 ZeroMemory(nPartTyep,sizeof(nPartTyep));

					 g_CompEquip.GetInt15s(2,"Position_2",nPartTyep);

                     for (int i=0;i<14;++i)
					 {//ɷõװĲλ
						 if (nPartTyep[i]==nPart)
						 {
                              nReg=1;
						      break;
						 }
					 } 

					 if (!Item[nHandIdx].IsBlue())
					 {
				       m_ItemList.ClientShowMsg("ʾ:ϲ,װ!");
					   ZeroMemory(nPartTyep,sizeof(nPartTyep));
				        return;
					 }
					 if (nGenre!=0 || nReg==0)
					 {
						 m_ItemList.ClientShowMsg("ʾ:װͲ,·!");
						 ZeroMemory(nPartTyep,sizeof(nPartTyep));
						 return;
					 }

				 }
				 if (DownPos.nX==3) //С1
				 {
                      
                     int nPartTyep[3];
					 ZeroMemory(nPartTyep,sizeof(nPartTyep));
                     g_CompEquip.GetInt3s(4,"Distill",nPartTyep);
					 if (nGenre!=nPartTyep[0] || nPart!=nPartTyep[1] || nParticular!=nPartTyep[2])
					 {
					     m_ItemList.ClientShowMsg("ʾ:ϲ,ʯ!");
						 ZeroMemory(nPartTyep,sizeof(nPartTyep));
						  return;
					 }
					
				 }
				 if (DownPos.nX==4) //С2
				 {
					int nPartTyep[2];
					ZeroMemory(nPartTyep,sizeof(nPartTyep));
					g_CompEquip.GetInt3s(5,"Distill",nPartTyep);
					if (nGenre!=2 || nPart!=1 || nParticular>nPartTyep[1] || nParticular<nPartTyep[0])
					{
						m_ItemList.ClientShowMsg("ʾ:ϲ,ԭ!");
						ZeroMemory(nPartTyep,sizeof(nPartTyep));
						return;
					}
				 }*/
              //   return;
			 }
			 else if (nIsComp==3) //װ
			 {
				 //m_ItemList.ClientShowMsg("ϼ");
				 //return;
			 }
			 else if (nIsComp==4) //װ
			 {
				 //m_ItemList.ClientShowMsg("ײϼ");
				 //return;
			 }
			 else if (nIsComp==5) //ͼ
			 {
				 //m_ItemList.ClientShowMsg("ͼײϼ");
				 //return;
			 }
			 else if (nIsComp==12) //
			 {
				 //m_ItemList.ClientShowMsg("");

			   if (DownPos.nX>=53 && DownPos.nX<=77)
			   {
				 char nMsg[64];  
				 if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_TongBanNum)
				 {//Ѿٻ˳	 
					 int nNpcIdx=0;
					     nNpcIdx =NpcSet.SearchID(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_TongBanNum);
					 if (nNpcIdx>0)
					 {
					 }
					 else
					 {
						 sprintf(nMsg,"<c=yellow><c>:ͬûٻ,װ!");
						 m_ItemList.ClientShowMsg(nMsg);
						 return;
					 }
				 }
				 else
				 {
					 sprintf(nMsg,"<c=yellow><c>:ͬûٻ,װ!");
					 m_ItemList.ClientShowMsg(nMsg);
					 return;
				 }
			   }
			 } 
		}
	}
	else if (!CheckTrading() && DownPos.nPlace == pos_cailiao)  // 
	{
		int nHandIdx = m_ItemList.Hand();
		if (nHandIdx)
		{//ϼ�?ڷ˽м
			//g_CompEquip.
			/*int nGenre=Item[nHandIdx].GetGenre();                //
			int nPart=Item[nHandIdx].GetDetailType();            //ϸ
			int nParticular= Item[nHandIdx].GetParticular();     //
            */
             if (nIsComp==1) //
			 {
				// m_ItemList.ClientShowMsg("ϼ");
				// return;
			 }
			 else if (nIsComp==2) //ȡ
			 {
				// m_ItemList.ClientShowMsg("ȡϼ");
				// return;
			 }
			 else if (nIsComp==3) //װ
			 {
				 //m_ItemList.ClientShowMsg("ϼ");
				 //return;
			 }
			 else if (nIsComp==4) //װ
			 {
				 //m_ItemList.ClientShowMsg("ײϼ");
				//return;
			 }
			 else if (nIsComp==5) //ͼ
			 {
				 //m_ItemList.ClientShowMsg("ͼײϼ");
				 //return;
			 }
			 else if (nIsComp==8) //ں
			 {
				 //m_ItemList.ClientShowMsg("ʾ:ر...");
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
				sprintf(sMsg.szMessage, "%s(%d)", "ʾ:<c=yellow>Ѿͬ͵Ʒ!<c>",nIsComp);
				sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				return;
			}
			*/
		}
	}
	
	SendClientCmdMoveItem(&DownPos,&UpPos,nIsComp); //DownPos Ŀ
}
#endif
#ifndef _SERVER
// DownPos ϵƷĵǰ꣬UpPos Ʒŵϵ
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
				sprintf(sMsg.szMessage, "ʾѾͬļ");
				sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				return;
			}
		}
	}
	
//	SendClientCmdMoveItem(&DownPos, &UpPos);
}
#endif
//Ʒ Ʒ
#ifndef _SERVER
int	KPlayer::ThrowAwayItem()
{
	if (this->CheckTrading())
	{
	    Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("ʾ:ڽ,ܶƷ!");
		return 0;
	}

	if (!m_ItemList.Hand())
	{

	    //Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("ƷΪգ");
		return 0;
	}
/*	else
	{
	   char msg[64];
	   sprintf(msg,"Ʒ:%d..",m_ItemList.Hand());
	   Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(msg);
	}*/
	int	nIdx =m_ItemList.Hand();

	if (/*Item[nIdx].GetTime()->bYear>0 || */Item[nIdx].GetIsBang())
	{
       int nReg=0;
	       g_GameSetTing.GetInteger("SYSTEM","ThrowAwayItem",1,&nReg);	//Ƿö

     if (!nReg)
	 { 
		KSystemMessage	sMsg;
		sprintf(sMsg.szMessage,":<c=green>(ʱ/)װܶ!<c>");
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
	// Ǯ
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
//	ܣȷȡ
//	if nDecision == 0 Ƴ  if nDecision == 1 ȷ  if nDecision == 2 ȡȷ
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
//	ܣȡ
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
//	ܣյ֪ͨ
//-------------------------------------------------------------------------
void	KPlayer::s2cApplyAddTeam(BYTE* pProtocol)
{
	// ״̬ (Ҫ npc Ķ�״?not end)
	if ( !m_cTeam.m_nFlag || m_cTeam.m_nFigure != TEAM_CAPTAIN)
	{
		ApplySelfTeamInfo();
		return;
	}
	
	// Ѱ npc
	PLAYER_APPLY_ADD_TEAM	*pAddTeam = (PLAYER_APPLY_ADD_TEAM*)pProtocol;
	int nNpcNo = NpcSet.SearchID(pAddTeam->m_dwTarNpcID);
	if (nNpcNo == 0)
		return;
	
	int i, nFreeListNo;
	// Ѿڣ
	for (i = 0; i < MAX_TEAM_APPLY_LIST; ++i)
	{
		if (m_cTeam.m_sApplyList[i].m_dwNpcID == pAddTeam->m_dwTarNpcID)
		{
			m_cTeam.m_sApplyList[i].m_nLevel = Npc[nNpcNo].m_Level;
			m_cTeam.m_sApplyList[i].m_dwTimer = MAX_APPLY_TEAM_TIME;
			snprintf(m_cTeam.m_sApplyList[i].m_szName, sizeof(m_cTeam.m_sApplyList[i].m_szName), "%s", Npc[nNpcNo].Name);
			m_cTeam.UpdateInterface();
			m_cTeam.UpdateamUI(); //ʾϢ
			return;
		}
	}
	// Ѱlistλ
	for (i = 0; i < MAX_TEAM_APPLY_LIST; ++i)
	{
		if (m_cTeam.m_sApplyList[i].m_dwNpcID == 0)
		{
			nFreeListNo = i;
			break;
		}
	}
	if (i >= MAX_TEAM_APPLY_LIST)	// ûҵ˿λ
		return;
	
	m_cTeam.m_sApplyList[nFreeListNo].m_dwNpcID = pAddTeam->m_dwTarNpcID;
	m_cTeam.m_sApplyList[nFreeListNo].m_nLevel = Npc[nNpcNo].m_Level;
	m_cTeam.m_sApplyList[nFreeListNo].m_dwTimer = MAX_APPLY_TEAM_TIME;
	snprintf(m_cTeam.m_sApplyList[nFreeListNo].m_szName, sizeof(m_cTeam.m_sApplyList[nFreeListNo].m_szName), "%s", Npc[nNpcNo].Name);
	
	m_cTeam.UpdateInterface();
	m_cTeam.UpdateamUI(); //ʾϢ
//-------------------------------------------------//ԶӦ
    //pPlayer = (KUiPlayerItem*)(&m_pHandlingMsg[1]);
			//g_pCoreShell->TeamOperation(TEAM_OI_APPLY_RESPONSE,(unsigned int)pPlayer, (nSelAction == 0));	//nSelAction=0Ӧ, nSelAction=1ܾ
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
//	ܣ
//-------------------------------------------------------------------------
BOOL	KPlayer::AddFaction(char *lpszFactionName)
{
	if ( !m_cFaction.AddFaction(Npc[m_nIndex].m_Series, lpszFactionName))
		return FALSE;
	
	// ıӪ
	Npc[m_nIndex].SetCamp(m_cFaction.GetGurFactionCamp());
	
	// ͻ˷Ϣ
	SendFactionData();
	
	return TRUE;
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	ܣ
//-------------------------------------------------------------------------
BOOL	KPlayer::AddFaction(int nFactionID)
{
	if ( !m_cFaction.AddFaction(Npc[m_nIndex].m_Series, nFactionID) )
		return FALSE;
	
	// ıӪ
	Npc[m_nIndex].SetCamp(m_cFaction.GetGurFactionCamp());
	
	// ͻ˷Ϣ
	SendFactionData();
	
	return TRUE;
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	ܣ�?
//-------------------------------------------------------------------------
BOOL	KPlayer::LeaveCurFaction()
{
	m_cFaction.LeaveFaction();
	
	// ıӪ
	Npc[m_nIndex].SetCamp(camp_free);
	
	// ͻ˷Ϣ
	PLAYER_LEAVE_FACTION	sLeave;
	sLeave.ProtocolType = s2c_playerleavefaction;
	g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sLeave, sizeof(PLAYER_LEAVE_FACTION));
	
	return TRUE;
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	ܣŵǰĳȼļ
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
//	ܣʱ
//-------------------------------------------------------------------------
void	KPlayer::TaskClearTempVal()
{
	this->m_cTask.ClearTempVar();
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	ܣõ
//-------------------------------------------------------------------------
int		KPlayer::TaskGetSaveVal(int nNo)
{
	return this->m_cTask.GetSaveVal(nNo);
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	ܣ�?
//-------------------------------------------------------------------------
void	KPlayer::TaskSetSaveVal(int nNo, BOOL bFlag)
{
	this->m_cTask.SetSaveVal(nNo, bFlag);
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	ܣõʱ̿Ʊֵ
//-------------------------------------------------------------------------
int		KPlayer::TaskGetClearVal(int nNo)
{
	return this->m_cTask.GetClearVal(nNo);
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	ܣ趨ʱ̿Ʊ�?
//-------------------------------------------------------------------------
void	KPlayer::TaskSetClearVal(int nNo, int nVal)
{
	this->m_cTask.SetClearVal(nNo, nVal);
}
#endif

//ÿݼ --Ĭ
#ifndef _SERVER
void	KPlayer::SetDefaultImmedSkill()  
{
	//ȡҼΪĬ
	int nDetailType = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetWeaponType();
	int nParticularType = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetWeaponParticular();
	
	//
	if (nDetailType == 0)
	{
		Player[CLIENT_PLAYER_INDEX].SetLeftSkill(g_nMeleeWeaponSkill[nParticularType]);
		Player[CLIENT_PLAYER_INDEX].SetRightSkill(g_nMeleeWeaponSkill[nParticularType]);
	}//Զ
	else if (nDetailType == 1)
	{
		Player[CLIENT_PLAYER_INDEX].SetLeftSkill(g_nRangeWeaponSkill[nParticularType]);
		Player[CLIENT_PLAYER_INDEX].SetRightSkill(g_nRangeWeaponSkill[nParticularType]);
	}//
	else if (nDetailType == -1)
	{
		Player[CLIENT_PLAYER_INDEX].SetLeftSkill(g_nHandSkill);
		Player[CLIENT_PLAYER_INDEX].SetRightSkill(g_nHandSkill);
	}

}
//ÿʰȡ
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

    Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].ReSkillEnhance(m_nLeftSkillID,0); //ȼȥһܼӳ

	//дڹҼܱ
	if (nIsNoLogin)
	{
		 Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("<c=gyellow><c>:Զһܳɹ!");
		 KIniFile nAutoConfig;
		
		 char nConfig[128]={0};
					   
		 sprintf(nConfig,"\\FsAi\\\\%s_.ini",Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name);
					   
		 if  (nAutoConfig.Load(nConfig))
		 { 
			 nAutoConfig.WriteInteger("AAAA","LeftSkill_0",nSkillID); 
		 }
		 else
		 {
			 Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("<c=gyellow><c>:ò!");

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
    
	//�?
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
	// Ƿǹ�?
	ISkill * pOrdinSkill = g_SkillManager.GetSkill(nSkillID, 1);

	if (!pOrdinSkill) 
        return;
	
	if (pOrdinSkill->IsAura())
	{//ǹ�?ʹ
		Npc[m_nIndex].SetAuraSkill(nSkillID);
	}
	else
	{//Ϊǹ�?
		Npc[m_nIndex].SetAuraSkill(0);
	}
     
	KUiGameObject Info;
	Info.uGenre = CGOG_SKILL_SHORTCUT;
	Info.uId = m_nRightSkillID;
	CoreDataChanged(GDCNI_PLAYER_IMMED_ITEMSKILL, (unsigned int)&Info, -2);
}
//װ ¿Ƽ
void KPlayer::UpdateWeaponSkill()
{
	if (m_nLeftSkillID > 0)
	{
		ISkill * pISkill = g_SkillManager.GetSkill(m_nLeftSkillID, 1);
		if (!pISkill) 
            return;
		
		if (m_nLeftSkillID==1 || m_nLeftSkillID==2 || m_nLeftSkillID==53)
		{// id
			SetLeftSkill(Npc[m_nIndex].GetCurActiveWeaponSkill());
		}
		
	}
	if (m_nRightSkillID > 0)
	{//Ҽ id
		ISkill * pISkill = (KSkill *) g_SkillManager.GetSkill(m_nRightSkillID, 1);
		if (!pISkill) 
            return;
		
		//if (pISkill->IsPhysical()) //⹦�?
		if (m_nRightSkillID==1 || m_nRightSkillID==2 || m_nRightSkillID==53)
		{
			SetRightSkill(Npc[m_nIndex].GetCurActiveWeaponSkill());
		}
	}
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	ܣ趨ǰ�?
//-------------------------------------------------------------------------
void	KPlayer::SetChatCurChannel(int nChannelNo)
{
	m_cChat.SetCurChannel(nChannelNo);
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	ܣ
//-------------------------------------------------------------------------
void	KPlayer::TeamInviteAdd(DWORD dwNpcID)
{
	m_cTeam.InviteAdd(dwNpcID);
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	ܣյͻ˷
//-------------------------------------------------------------------------
void	KPlayer::ServerSendChat(BYTE* pProtocol)
{
	m_cChat.ServerSendChat(m_nPlayerIndex, pProtocol);
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	ܣյͻҪӻԵ(0=Strength 1=Dexterity 2=Vitality 3=Engergy)
//-------------------------------------------------------------------------
void	KPlayer::AddBaseAttribute(BYTE* pProtocol)
{
	PLAYER_ADD_BASE_ATTRIBUTE_COMMAND	*pAdd = (PLAYER_ADD_BASE_ATTRIBUTE_COMMAND*)pProtocol;
	
	//ִһжϽű Ƿִ
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
		AddBaseStrength(pAdd->m_nAddNo);  //
		break;
	case ATTRIBUTE_DEXTERITY:
		AddBaseDexterity(pAdd->m_nAddNo);  //
		break;
	case ATTRIBUTE_VITALITY:
		AddBaseVitality(pAdd->m_nAddNo);   //�?
		break;
	case ATTRIBUTE_ENGERGY:
		AddBaseEngergy(pAdd->m_nAddNo);    //  ڹ
		break;
	}
}
//û
void	KPlayer::ResetBaseAttribute(BYTE* pProtocol)
{
	PLAYER_ADD_BASE_ATTRIBUTE_COMMAND	*pAdd = (PLAYER_ADD_BASE_ATTRIBUTE_COMMAND*)pProtocol;
	switch (pAdd->m_btAttribute)
	{
	case ATTRIBUTE_STRENGTH:  //
		ResetBaseStrength(pAdd->m_nAddNo);
		break;
	case ATTRIBUTE_DEXTERITY: //�?
		ResetBaseDexterity(pAdd->m_nAddNo);
		break;
	case ATTRIBUTE_VITALITY:  //
		ResetBaseVitality(pAdd->m_nAddNo);
		break;
	case ATTRIBUTE_ENGERGY:   //
		ResetBaseEngergy(pAdd->m_nAddNo);
		break;
	}
}
#endif

#define MAX_ORDINSKILL_LEVEL_ALWAYS  20//Լܵȼı�֮ģһܵ?


#ifdef _SERVER
//-------------------------------------------------------------------------
//	ܣյͻҪĳܵĵ   
//-------------------------------------------------------------------------
void	KPlayer::AddSkillPoint(BYTE* pProtocol)
{
	PLAYER_ADD_SKILL_POINT_COMMAND	*pAdd = (PLAYER_ADD_SKILL_POINT_COMMAND*)pProtocol;
	
	int nSkillIndex = Npc[m_nIndex].m_SkillList.FindSame(pAdd->m_nSkillID);

	if (nSkillIndex == 0)
		return;

	int nSkillLevel = Npc[m_nIndex].m_SkillList.GetSkillIdxLevel(nSkillIndex);  //ȡܵǰĵȼ
	
	if (m_nSkillPoint >= pAdd->m_nAddPoint && nSkillLevel + pAdd->m_nAddPoint <= MAX_SKILLLEVEL)
	{
		ISkill * pSkill = NULL;
		if (nSkillLevel == 0)
			pSkill =  g_SkillManager.GetSkill(pAdd->m_nSkillID, 1);  //ȡܵĻ
		else
			pSkill =  g_SkillManager.GetSkill(pAdd->m_nSkillID, nSkillLevel);

		if (pSkill)
		{
			char szMsg[125]={0};
			//ִһжϽű Ƿִ
			int reg=ExecuteScript_Direct("\\script\\Global\\global_setting.lua","main_Skillset",true,"ddddsd",pAdd->m_nSkillID,nSkillLevel,nSkillIndex,pAdd->m_nAddPoint,pSkill->GetSkillName(),pSkill->GetSkillStyle());
			if  (reg<=0)
			{
				snprintf(szMsg, sizeof(szMsg), "<c=green>%s<c>khong phu hop nang cap dieu kien, khong the nang cap!", pSkill->GetSkillName());
				int nLength=0; 
				nLength = TEncodeText(szMsg, strlen(szMsg));
				KPlayerChat::SendSystemInfo(1, m_nPlayerIndex, strCoreInfo[MESSAGE_SYSTEM_ANNOUCE_HEAD].c_str(), (char *)szMsg, nLength);
				return;
			}
			//жӦļű,ִнű
			if (pSkill->GetSkillLevelUpScriptId())
			{
				ExecuteScriptB(pSkill->GetSkillLevelUpScriptId(), "main", pAdd->m_nAddPoint,"",FALSE);
				nSkillLevel = Npc[m_nIndex].m_SkillList.GetSkillIdxLevel(nSkillIndex);
			}
			else
			{
				switch(pSkill->GetSkillStyle())
				{
				  case SKILL_SS_Missles:			            //	ӵ		ڷӵ
				  case SKILL_SS_Melee:
				  case SKILL_SS_InitiativeNpcState:	    //			ڸı䵱ǰNpc״̬
				  case SKILL_SS_PassivityNpcState:		//			ڸıNpcı״̬
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
						//ȥļܵ
						//if ((nWantToBeLevel - Npc[m_nIndex].m_SkillList.GetAddPoint(nSkillIndex)) <= g_SkillManager.GetSkillMaxLevel(pAdd->m_nSkillID)+/*m_cReBorn.GetReBornNum()**/m_cReBorn.GetReBornSkillMaxLevel()+/*m_cVip.GetRoleVipNum()**/m_cVip.GetRoleVipSkillMaxLevel() &&  Npc[m_nIndex].m_Level >= ((KSkill*)pSkill)->GetSkillReqLevel())
						if (nLevelSkillMax > 1 && nWantToBeLevel <= (nLevelSkillMax + nLevelSkillMaxAdd + AddPointSkill ) && nWantToBeLevel <= Npc[m_nIndex].m_Level - ((KSkill*)pSkill)->GetSkillReqLevel() + 1)
						{
							if (!Npc[m_nIndex].m_SkillList.IncreaseLevel(nSkillIndex, pAdd->m_nAddPoint,FALSE,TRUE))
							   return;
			
							m_nSkillPoint -= pAdd->m_nAddPoint; //ʣ༼�?
							nSkillLevel   += pAdd->m_nAddPoint; //ܵȼ
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
			
									m_nSkillPoint -= pAdd->m_nAddPoint; //ʣ༼�?
									nSkillLevel   += pAdd->m_nAddPoint; //ܵȼ
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
				case SKILL_SS_Thief:  //͵�?
					{
						return;
					}break;
					
				}	
			}

			UpdataCurData();  //
			// ѵǰ˼ܵĵʣ༼ܵ㷢ͻ
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
//Ʒ  Ʒһ
#ifdef _SERVER
//-------------------------------------------------------------------------
//	ܣ˼Ʒ  ִнű
//-------------------------------------------------------------------------
BOOL	KPlayer::ServerPickUpItem(BYTE* pProtocol)
{
	PLAYER_PICKUP_ITEM_COMMAND	*pPickUp = (PLAYER_PICKUP_ITEM_COMMAND*)pProtocol;
	
	int	nObjIndex, nNpcX, nNpcY, nObjX, nObjY;
	    nObjIndex = ObjSet.FindID(pPickUp->m_nObjID);
	if (nObjIndex == 0)
		return FALSE;
	
	if (Object[nObjIndex].m_nBelong > 0)
	{//˵Ķ
		if (!m_cTeam.m_nFlag)
		{//ӵ
			if (Object[nObjIndex].m_nBelong != m_nPlayerIndex)
			{//ԼĶ
				SHOW_MSG_SYNC	sMsg;
				sMsg.ProtocolType = s2c_msgshow;

				if (Object[nObjIndex].m_nKind == Obj_Kind_Money)
					sMsg.m_wMsgID = enumMSG_ID_MONEY_CANNOT_PICKUP;	//ʰȡ˵
				else
					sMsg.m_wMsgID = enumMSG_ID_OBJ_CANNOT_PICKUP; //ʰȡ˵Ʒ

				sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1;
				g_pServer->PackDataToClient(m_nNetConnectIdx,(BYTE *)&sMsg, sMsg.m_wLength + 1);

				return FALSE;
			}
		}
		else
		{//ӵ
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
				{//ܼ˵Ķ
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
	{//ϵͳòĶ
		    m_ItemList.msgshow("Vat pham nay bi he thong cam!");
			printf("Vat pham nay bi he thong cam!");
			return FALSE;
	}

	// жϾ
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
	{ //
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
		//mReg=NpcSet.CheckForBit(nItemInfo,"GuiShuItem",3); //ǷԸĶ
	snprintf(nMapName, sizeof(nMapName), "%s", SubWorld[Npc[m_nIndex].m_SubWorldIndex].nWorldMapInfo[STR_MAP_NAME].c_str());
	if (Item[Object[nObjIndex].m_nItemDataID].GetGenre()!=0 && Item[Object[nObjIndex].m_nItemDataID].GetIsUse()/* && (strcmpi(Item[Object[nObjIndex].m_nItemDataID].GetWonName(),"ϵͳ") == 0)*/)  //ʹ
	{//װ ʹƷ ƷĽű
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
		{//ɾƷ  
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
	{//ǮĶ ִȫּű
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
	case Obj_Kind_Item:				// ڵϵװ
		{// װ
			//---------------------------------------------------//ռ
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
			{//ѯռ
				m_ItemList.msgshow("khoang trong khong du!");
				return FALSE;
			}

			if (Object[nObjIndex].m_nItemDataID <= 0 || Object[nObjIndex].m_nItemDataID >= MAX_ITEM)
			{
				m_ItemList.msgshow("Item loi!");
				return FALSE;
			}
			//xep chong
			m_nRoomFull=0;	        //ðΪ
			int StackNum =Item[Object[nObjIndex].m_nItemDataID].GetStackNum();

            if (StackNum<=0)
                StackNum=1;
            //---------------------------------------------------ʼǷƷ Ƿܵ
             /*if  (Item[Object[nObjIndex].m_nItemDataID].GetGenre()!=0 &&
				 !Item[Object[nObjIndex].m_nItemDataID].GetIsBang() &&
				 Item[Object[nObjIndex].m_nItemDataID].GetTime()->bYear<=0) 
			 {//װĻ,ǷƷ  ǷԶ
				 int nIdx=m_ItemList.IsHaveItem(Item[Object[nObjIndex].m_nItemDataID].GetGenre(),\
					                                      Item[Object[nObjIndex].m_nItemDataID].GetDetailType(),\
														  Item[Object[nObjIndex].m_nItemDataID].GetParticular(),\
														  Item[Object[nObjIndex].m_nItemDataID].GetSeries(),\
													      Item[Object[nObjIndex].m_nItemDataID].GetLevel(),pPickUp->m_btPosType);
               
				if (nIdx>0 && Item[nIdx].IsStack() &&
					Item[Object[nObjIndex].m_nItemDataID].CanStack(nIdx) &&
					(Item[nIdx].GetStackNum()+StackNum) <= MAX_STACKNUM
					) //ǷԵ--ͰǷͬ

				{//ĶԵ  
                    //StackNum=Item[nIdx].GetStackNum()+StackNum;
					printf("nhat item 1%d , HaveItem: %d\n",Item[nIdx].GetGenre(),nIdx);
					//m_ItemList.Remove(nIdx,0,0,0,FALSE);
					//ItemSet.Remove(nIdx);
				}
			}*/
			//---------------------------------------------------//˲
			int nItemIdx = m_ItemList.Add(Object[nObjIndex].m_nItemDataID,pPickUp->m_btPosType,xPos,yPos,0,0,StackNum);//pPickUp->m_btPosX 
			
			if (nItemIdx <= 0)  //Ʒ
				return FALSE;

			// ȥObject[nObjIndex]ߵĹItemSetRemoveObjectRemove
			Object[nObjIndex].m_nItemDataID = 0;
			Object[nObjIndex].m_nItemWidth  = 0;
			Object[nObjIndex].m_nItemHeight = 0;

            Object[nObjIndex].m_IsHaveAttack=0;
			Object[nObjIndex].m_AttackerDwid=0;
            Object[nObjIndex].m_AttackerTime=0;

			//ObjSet.Remove(nObjIndex);
			Object[nObjIndex].Remove(FALSE);   //ɾϵĶ	
			//ObjSet.resetLeftTime(nObjIndex,0);
		}
		break;
	case Obj_Kind_Money:			// ڵϵǮ
		if (!Earn(Object[nObjIndex].m_nMoneyNum) )  //Ǯ
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
//	ܣյͻҪʹĳƷ(Ҽ)
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
	//ﴦһǷ�?
	if (nItemIdx<=0 || nItemIdx>=MAX_ITEM)
	{
#ifdef _SERVER
		//ͻ˴쳣�?Ҫɾ.
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
		//ͻ˴쳣�?Ҫɾ.
		ITEM_REMOVE_SYNC	sRemove;
		sRemove.ProtocolType = s2c_removeitem;
		sRemove.m_ID         = pMove->m_nItemID;
		sRemove.nPlace       =-1;
		sRemove.m_dx         = 0;
		sRemove.m_dy         = 0;
		sRemove.m_model   = 0;
		g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sRemove, sizeof(ITEM_REMOVE_SYNC));

		snprintf(nItemInfo, sizeof(nItemInfo),"----AutoItem:˺:%sɫ:%s,ӸƷ:%s[+%d],ɾ쳣ƷɹA!-----",m_AccoutName,m_PlayerName,Item[nItemIdx].GetName(),Item[nItemIdx].GetEnChance());
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

		sprintf(nItemInfo,"----AutoItem:˺:%sɫ:%s,ӸƷ:%s[+%d],ɾ쳣ƷɹB!-----",m_AccoutName,m_PlayerName,Item[nItemIdx].GetName(),Item[nItemIdx].GetEnChance());
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
//	ܣյͻҪʹĳƷ(Ҽ)
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
	//˿ʼƷ
	//printf("-------Ʒ--------- \n");
	m_ItemList.ExchangeItem(&DownPos, &UpPos,IsMianBan);
	
	//Ʒͽ
	BYTE byFinished = s2c_itemexchangefinish;
	if (g_pServer)
		g_pServer->PackDataToClient(m_nNetConnectIdx,&byFinished, sizeof(BYTE));

	return TRUE;
}
#endif
//ԼƷʱĹ
#ifdef _SERVER

BOOL KPlayer::CheckGmName()
{
  int nGmRows=g_GMSetting.GetHeight()+1;
  int nIsGM=FALSE;
   for (int i=1;i<nGmRows;++i)
   { 
	  char nGmName[ACC_LENGTH];
      g_GMSetting.GetString(i,1,"",nGmName,sizeof(nGmName));
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
		nSname.GetString(i,1,"",nGmName,sizeof(nGmName));
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
	      // g_GameSetTing.GetInteger("SYSTEM","ThrowAwayItem",1,&nReg);	//Ƿö
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
	{//ɾĶɹĻ ͱʾɹ
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
		    sInfo.m_sHaveAttack=0;                     //ǷΪЧ
	    //sInfo.m_AttackerDwid=0;                      //ϴιŵDWID
		nObj = ObjSet.Add(Item[nItemIdx].GetObjIdx(),sMapPos,sInfo);  //ͬͻ˵϶

		if (nObj >= 0)
		{
			int mprgType=0;
			//ZeroMemory(mItemType,sizeof(mItemType));
            //ZeroMemory(mItemBelong,sizeof(mItemBelong));
			//mItemBelong[0] = Item[nItemIdx].GetGenre();
            //mItemBelong[1] = Item[nItemIdx].GetDetailType();  //
            //mItemBelong[2] = Item[nItemIdx].GetParticular();  //ϸ
			   mprgType=CheckThisItem(Item[nItemIdx].GetGenre(),"IsPickType");  //ֹ
            //mprgBelong = NpcSet.CheckForBit(mItemBelong,"ItemBelong",3);  //Ƿй

			if (mprgType==1)  //дڽֹб
			{//ϵͳֹĴ
				 Object[nObj].SetEntireBelong(-2);
			}
            else
			{// ͼ˵ 
			   //if (mprgBelong==1)
			   //{//оĶڽֹб
               //Object[nObj].SetEntireBelong(this->m_nPlayerIndex);
			   //}
			   //else
			   //˶Լ
				Object[nObj].SetItemBelong(-1);
			}
		}
	}
	else
	{
	// ֪ͨͻĶŲ
	   m_ItemList.msgshow("Xoa muc khong thanh cong!");
	}
  }
  else if (pThrow->m_Kind==1)
  {//Ҷ ͡οͻ˶
    GetAboutPos(&sMapPos); 
	char nItemInfo[128];

	int nItemIdx = m_ItemList.SearchID(pThrow->m_ItemDwidx);
	int inidx    = m_ItemList.SearchItemsIdx(pThrow->m_ItemDwidx);

	if  (nItemIdx<=0 || nItemIdx>=MAX_ITEM)
	{//Ʒ
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
	   snprintf(nItemInfo, sizeof(nItemInfo),"----ServerThrowAwayItem:˺:%sɫ:%s,ӸƷ,ɾ쳣Ʒ�?C-----",m_AccoutName,m_PlayerName);
	   g_LogFile.gTraceLogFile(nItemInfo,strlen(nItemInfo));
#endif
	   return;
	}

	int inPlace=m_ItemList.m_Items[inidx].nPlace;

	if  (inPlace<=0 || inPlace>=pos_count) 
	{//λòԣƷ
		m_ItemList.msgshow("Loi vui long bao GM ma loi 3333131313");
		//ͬͻɾͻƷ
#ifdef _SERVER
		ITEM_REMOVE_SYNC	sRemove;
		sRemove.ProtocolType = s2c_removeitem;
		sRemove.m_ID         = pThrow->m_ItemDwidx;	
		sRemove.nPlace       =-1;
		sRemove.m_dx         = 0;
		sRemove.m_dy         = 0;
		sRemove.m_model   = 0;
		g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sRemove, sizeof(ITEM_REMOVE_SYNC));

		snprintf(nItemInfo, sizeof(nItemInfo),"----ServerThrowAwayItem:˺:%sɫ:%s,ӸƷ:%s[+%d],ɾ쳣Ʒ�?A-----",m_AccoutName,m_PlayerName,Item[nItemIdx].GetName(),Item[nItemIdx].GetEnChance());
		g_LogFile.gTraceLogFile(nItemInfo,strlen(nItemInfo));

		if (m_ItemList.Remove(nItemIdx,0,0,0,FALSE))
			ItemSet.Remove(nItemIdx);
#endif
		return;
	}

	if (pThrow->m_nPlace!=inPlace || pThrow->m_X!=m_ItemList.m_Items[inidx].nX || pThrow->m_Y!=m_ItemList.m_Items[inidx].nY)
	{//Ʒ λϢ
		m_ItemList.msgshow("Loi vui long bao GM ma loi 5555555");
		//ͬͻɾͻƷ
#ifdef _SERVER
		ITEM_REMOVE_SYNC	sRemove;
		sRemove.ProtocolType = s2c_removeitem;
		sRemove.m_ID         = pThrow->m_ItemDwidx;
		sRemove.nPlace       =pThrow->m_nPlace;
		sRemove.m_dx         =pThrow->m_X;
		sRemove.m_dy         =pThrow->m_Y;
		sRemove.m_model   =0;
		g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sRemove, sizeof(ITEM_REMOVE_SYNC));

		snprintf(nItemInfo, sizeof(nItemInfo),"----ServerThrowAwayItem:˺:%sɫ:%s,ӸƷ:%s[+%d],ɾ쳣Ʒ�?B-----",m_AccoutName,m_PlayerName,Item[nItemIdx].GetName(),Item[nItemIdx].GetEnChance());
		g_LogFile.gTraceLogFile(nItemInfo,strlen(nItemInfo));

		if (m_ItemList.Remove(nItemIdx,0,0,0,FALSE))
			   ItemSet.Remove(nItemIdx);

#endif
		return;
	}
	    //m_ItemList.m_Items[inidx].nPlace;//
        //nItemidx  = m_ItemList.SearchItemsIdx(nItemDwID);
	if (m_ItemList.Remove(nItemIdx,0,0,0,FALSE))
	{
       int nLevel=Item[nItemIdx].GetLevel();
       int nYlevel=1; 
	  if (pThrow->m_IsAuToFenJie==1 && Item[nItemIdx].GetGenre()==item_equip && (Item[nItemIdx].IsBlue()||Item[nItemIdx].IsPurple()||Item[nItemIdx].GetGoldId()>0) &&  nLevel>=_gameGlobalSet.gameItemInfo.nInFenJie[0])
	  {//ԶֽĻ
			
			int nIndex = ItemSet.AddOther(item_task,0,1,0,_gameGlobalSet.gameItemInfo.nInFenJie[2],0,0,0,0,GetRandomNumber(1,nLevel),0,0,0,0,0,0,0);
	
	        if (nIndex <= 0) 
		       return;
			//xep chong item
           int nStackNum =Item[nIndex].GetStackNum();
	       if (nStackNum<=0)
	          nStackNum=1;
//---------------------------------------------------ʼǷƷ Ƿܵ
            /* if (Item[nIndex].GetGenre()!=0 && !Item[nIndex].GetIsBang() && Item[nIndex].GetTime()->bYear<=0)
			 {//װĻ,ǷƷ
				 int nIdx=m_ItemList.IsHaveItem(Item[nIndex].GetGenre(),\
					                                      Item[nIndex].GetDetailType(),\
														  Item[nIndex].GetParticular(),\
														  Item[nIndex].GetSeries(),\
													      Item[nIndex].GetLevel(),inPlace);
               
				if (nIdx>0 && Item[nIdx].IsStack() &&
					Item[nIndex].CanStack(nIdx) &&
					(Item[nIdx].GetStackNum()+nStackNum) <= MAX_STACKNUM) //ǷԵ--ͰǷͬ
				{//ĶԵ		
					printf("throw item %d , HaveItem: %d\n",Item[nIdx].GetGenre(),nIdx);
                    //nStackNum=Item[nIdx].GetStackNum()+nStackNum;
					m_ItemList.Remove(nIdx,0,0,0,FALSE);
					ItemSet.Remove(nIdx);			
				}
			}*/
			 
	       int x, y,nRoomType=room_equipment;

		   if (inPlace==pos_equiproom)	//
			   nRoomType=room_equipment;

		   if(inPlace==pos_equiproomex)	//ĸ
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
	  {//ûпֽ⡡Ͷϡֱ�?
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
			snprintf(nItemInfo, sizeof(nItemInfo),"----[]ServerThrowAwayItem:˺:%sɫ:%s,ֶƷ:%s[+%d],-----",m_AccoutName,m_PlayerName,Item[nItemIdx].GetName(),Item[nItemIdx].GetEnChance());
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
			sInfo.m_sHaveAttack = 1;                      //ǷΪЧ
		else
			sInfo.m_sHaveAttack = 0; 
	    //sInfo.m_AttackerDwid=Npc[m_nIndex].m_dwID;   //ϴιŵDWID
		nObj = ObjSet.Add(Item[nItemIdx].GetObjIdx(),sMapPos,sInfo);  //ͬͻ˵϶
        
		if (nObj >= 0)
		{
			int mprgType=0;
			   mprgType=CheckThisItem(Item[nItemIdx].GetGenre(),"IsPickType");  //ֹ
			if (mprgType==1)  //дڽֹб
			{//ϵͳֹĴ
				 Object[nObj].SetEntireBelong(-2);
			}
            else
			{// ͼ˵ 
			   //˶Լ
				Object[nObj].SetItemBelong(-1); 
			}
		}
		else
		   ItemSet.Remove(nItemIdx);
	  }
	}
	else// ֪ͨͻĶŲ
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
//	ܣյͻϢ�충�
//-------------------------------------------------------------------------
void	KPlayer::ChatSetTakeChannel(BYTE* pProtocol)
{
	CHAT_SET_CHANNEL_COMMAND	*pChannel = (CHAT_SET_CHANNEL_COMMAND*)pProtocol;
	this->m_cChat.SetTakeChannel(pChannel->m_dwTakeChannel);
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	ܣյͻתӺϢ
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
//	ܣյͻϢ
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
//	ܣյͻϢܾ
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
//	ܣҵ¼ʱյͻ·ݣ֣
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
//	ܣҵ¼ʱյͻĳݣ֣
//-------------------------------------------------------------------------
void	KPlayer::ChatSendOneFriendData(BYTE* pProtocol)
{
	CHAT_APPLY_SEND_ONE_FRIEND_NAME_COMMAND	*pApply = (CHAT_APPLY_SEND_ONE_FRIEND_NAME_COMMAND*)pProtocol;
	
	m_cChat.ResendOneFriendData(pApply->m_dwID, m_nPlayerIndex);
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	ܣյͻɾĳ
//-------------------------------------------------------------------------
void	KPlayer::ChatDeleteFriend(BYTE* pProtocol)
{
	CHAT_DELETE_FRIEND_COMMAND	*pDelete = (CHAT_DELETE_FRIEND_COMMAND*)pProtocol;
	
	m_cChat.DeleteFriend(pDelete->m_dwID, m_dwID);
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	ܣյͻٴɾĳ
//-------------------------------------------------------------------------
void	KPlayer::ChatRedeleteFriend(BYTE* pProtocol)
{
	CHAT_REDELETE_FRIEND_COMMAND	*pDelete = (CHAT_REDELETE_FRIEND_COMMAND*)pProtocol;
	
	this->m_cChat.RedeleteFriend(pDelete->m_dwID, m_nPlayerIndex);
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	ܣյͻ״̬
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
//	ܣյͻȡ״̬
//-------------------------------------------------------------------------
void	KPlayer::TradeApplyClose(BYTE* pProtocol)
{
	m_cMenuState.SetState(m_nPlayerIndex, PLAYER_MENU_STATE_NORMAL);
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	ܣյͻ�?
//-------------------------------------------------------------------------
void	KPlayer::TradeApplyStart(BYTE* pProtocol)
{
	if (__forBitTrade || this->CheckTrading())
		return;
	if (KPlayerTradeJournal::Instance().IsPlayerLocked(this->m_PlayerName))
	{
		printf("[TradeApplyStart Blocked]: Player '%s' is locked by Trade Journal Recovery.\n", this->m_PlayerName);
		return;
	}

	
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

	this->m_cTrade.m_nApplyIdx = nPlayer; // Է player id
	//Է
	TRADE_APPLY_START_SYNC	sApply;
	sApply.ProtocolType = s2c_tradeapplystart;
	sApply.m_nDestIdx = this->m_nPlayerIndex; //˵ player id
	sApply.m_dwNpcId  = Npc[m_nIndex].m_dwID; //˵ m_dwID
	if (g_pServer)
		g_pServer->PackDataToClient(Player[nPlayer].m_nNetConnectIdx, (BYTE*)&sApply, sizeof(TRADE_APPLY_START_SYNC));
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	ܣ
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
		//printf("------%dƷ:%s------- \n",nPos,Item[nItemIdx].GetName());
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
				 printf("------ӳɹ%dƷ:%s------- \n",nPos,Item[nItemIdx].GetName());
			 }
		     /*if (m_ItemList.Add(nItemIdx,pos_equiproom,x,y)) //²
			 {//ɾԭ
				 m_ItemList.Remove(nItemIdx);
				 ItemSet.Remove(nItemIdx);
				 printf("------ӳɹ%dƷ:%s------- \n",nPos,Item[nItemIdx].GetName());
			 }*/
		}
		else //ռ�?ɾ
			if (nItemIdx)
			{
				printf("------ɾ%dƷ:%s------- \n",nPos,Item[nItemIdx].GetName());
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
//	ܣִлȡ
//-------------------------------------------------------------------------
void	KPlayer::TradeDecision(BYTE* pProtocol)
{
	if (__forBitTrade || !CheckTrading())
		return;
	if (KPlayerTradeJournal::Instance().IsPlayerLocked(this->m_PlayerName))
	{
		printf("[TradeDecision Blocked]: Player '%s' is locked by Trade Journal Recovery.\n", this->m_PlayerName);
		return;
	}

	if (Player[this->m_cTrade.m_nTradeDest].m_nNetConnectIdx<=-1 || this->m_nNetConnectIdx<=-1)
		return;
	
	TRADE_DECISION_COMMAND	*pDecision = (TRADE_DECISION_COMMAND*)pProtocol;
	
	if (pDecision->m_btDecision == 2)	// 
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
	else if (pDecision->m_btDecision == 3)	// ȡ
	{
		if (this->m_cTrade.m_nTradeLock == 1)
		{
			// ȡͬʱҲȡ˶Է
			this->m_cTrade.m_nTradeLock = 0;
			this->m_cTrade.m_nTradeState = 0;
			Player[m_cTrade.m_nTradeDest].m_cTrade.m_nTradeLock = 0;
			Player[m_cTrade.m_nTradeDest].m_cTrade.m_nTradeState = 0;
		}
		SyncTradeState();
		return;
	}
	else if (pDecision->m_btDecision == 4)	// ȡȷ
	{
		if (m_cTrade.m_nTradeLock == 1 && Player[m_cTrade.m_nTradeDest].m_cTrade.m_nTradeLock == 1)
		{
			m_cTrade.m_nTradeState = 0;
			SyncTradeState();
		}
		return;
	}
	else if (pDecision->m_btDecision <= 0)
	{
		CancelTradeSafe();
		return;
	}
	else if (pDecision->m_btDecision == 1)	// ִн
	{
		// ˫ûͬʱȷ
		if (this->m_cTrade.m_nTradeLock != 1 ||
			Player[m_cTrade.m_nTradeDest].m_cTrade.m_nTradeLock != 1)
		{
			SyncTradeState();
			return;
		}
		// Էûȷ֪ͨԷԼȷ
		else if (Player[m_cTrade.m_nTradeDest].m_cTrade.m_nTradeState == 0)
		{
			m_cTrade.m_nTradeState = 1;
			
			SyncTradeState();
			
			return;
		}
		// Էȷִн
		else
		{
			// ִн֮ǰȼһǷд-----------------
			// --------------------  money --------------------
			int	nDestIdx = m_cTrade.m_nTradeDest;
			if (nDestIdx <= 0 || nDestIdx >= MAX_PLAYER || nDestIdx == m_nPlayerIndex)
			{
				CancelTradeSafe();
				return;
			}
			// Round 4 P0: 1. Pre-commit session and snapshot validation
			DWORD dwSelfSession = m_cTrade.m_dwTradeSessionId;
			DWORD dwDestSession = Player[nDestIdx].m_cTrade.m_dwTradeSessionId;
			if (dwSelfSession == 0 || dwDestSession == 0 || dwSelfSession != dwDestSession)
			{
				CancelTradeSafe();
				return;
			}
			if (!this->m_ItemList.HasValidTradeSnapshot(dwSelfSession) ||
				!Player[nDestIdx].m_ItemList.HasValidTradeSnapshot(dwDestSession))
			{
				CancelTradeSafe();
				return;
			}
			// Round 4 P0: Checked 64-bit arithmetic on money (overflow & balance protection)
			long long llSM1 = (long long)m_ItemList.GetsMon(room_equipment);
			long long llSM2 = (long long)m_ItemList.GetsMon(room_trade);
			long long llDM1 = (long long)Player[nDestIdx].m_ItemList.GetsMon(room_equipment);
			long long llDM2 = (long long)Player[nDestIdx].m_ItemList.GetsMon(room_trade);
			if (llSM1 < 0 || llSM2 < 0 || llDM1 < 0 || llDM2 < 0 || llSM2 > llSM1 || llDM2 > llDM1)
			{
				CancelTradeSafe();
				return;
			}
			long long llNewSelfMoney = llSM1 - llSM2 + llDM2;
			long long llNewDestMoney = llDM1 - llDM2 + llSM2;
			if (llNewSelfMoney < 0 || llNewSelfMoney > 2000000000LL ||
				llNewDestMoney < 0 || llNewDestMoney > 2000000000LL)
			{
				CancelTradeSafe();
				return;
			}
			// Round 4 P0: Room placement check
			Player[nDestIdx].m_ItemList.GetTradeRoomItemInfo();
			int nSelfCanGet = m_ItemList.TradeCheckCanPlace();
			ItemSet.BackItemInfo();
			this->m_ItemList.GetTradeRoomItemInfo();
			int nDestCanGet = Player[nDestIdx].m_ItemList.TradeCheckCanPlace();
			if (!nSelfCanGet || !nDestCanGet)
			{
				this->m_cTrade.m_nTradeState = 0;
				Player[nDestIdx].m_cTrade.m_nTradeState = 0;
				SHOW_MSG_SYNC sMsg;
				sMsg.ProtocolType = s2c_msgshow;
				sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1;
				sMsg.m_wMsgID = !nSelfCanGet ? enumMSG_ID_TRADE_SELF_ROOM_FULL : enumMSG_ID_TRADE_DEST_ROOM_FULL;
				if (m_nNetConnectIdx >= 0) g_pServer->PackDataToClient(m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
				if (Player[nDestIdx].m_nNetConnectIdx >= 0) g_pServer->PackDataToClient(Player[nDestIdx].m_nNetConnectIdx, (BYTE*)&sMsg, sMsg.m_wLength + 1);
				SyncTradeState();
				return;
			}
			// Round 4 P0: Bound and locked item protection
			for (int nP = 0; nP < TRADE_ROOM_WIDTH * TRADE_ROOM_HEIGHT; ++nP)
			{
				int nBackIdx = ItemSet.m_psBackItemInfo[nP].m_nIdx;
				if (nBackIdx > 0 && nBackIdx < MAX_ITEM && Item[nBackIdx].GetIsBang())
				{
					CancelTradeSafe();
					return;
				}
				int nSelfIdx = ItemSet.m_psItemInfo[nP].m_nIdx;
				if (nSelfIdx > 0 && nSelfIdx < MAX_ITEM && Item[nSelfIdx].GetIsBang())
				{
					CancelTradeSafe();
					return;
				}
			}
			// Final P0: Pre-trade WAL snapshot serialization for durable transaction journal
			this->Save();
			Player[nDestIdx].Save();
			uint32_t dwPreLenSelf = ((TRoleData*)m_GaemSaveBuffer)->dwDataLen;
			uint32_t dwPreLenDest = ((TRoleData*)Player[nDestIdx].m_GaemSaveBuffer)->dwDataLen;
			this->m_uSaveGeneration++;
			Player[nDestIdx].m_uSaveGeneration++;
			uint64_t uTxId = KPlayerTradeJournal::Instance().BeginTransaction(
				m_nPlayerIndex, m_PlayerName, m_GaemSaveBuffer, dwPreLenSelf,
				nDestIdx, Player[nDestIdx].m_PlayerName, Player[nDestIdx].m_GaemSaveBuffer, dwPreLenDest,
				this->m_uSaveGeneration, Player[nDestIdx].m_uSaveGeneration);
			if (uTxId == 0)
			{
				this->m_ItemList.RecoverTrade(dwSelfSession);
				Player[nDestIdx].m_ItemList.RecoverTrade(dwDestSession);
				this->m_ItemList.ClearRoom(room_trade);
				Player[nDestIdx].m_ItemList.ClearRoom(room_trade);
				this->m_cTrade.Release();
				this->m_cMenuState.RestoreBackupState(m_nPlayerIndex);
				Player[nDestIdx].m_cTrade.Release();
				Player[nDestIdx].m_cMenuState.RestoreBackupState(nDestIdx);
				TRADE_DECISION_SYNC sCancelSync;
				sCancelSync.ProtocolType = s2c_tradedecision;
				sCancelSync.m_btDecision = 0;
				if (m_nNetConnectIdx >= 0)
					g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sCancelSync, sizeof(TRADE_DECISION_SYNC));
				if (Player[nDestIdx].m_nNetConnectIdx >= 0)
					g_pServer->PackDataToClient(Player[nDestIdx].m_nNetConnectIdx, (BYTE*)&sCancelSync, sizeof(TRADE_DECISION_SYNC));
				return;
			}
			// 2. In-memory money exchange
			if (llSM2 <= llDM2)
			{
				this->Earn((int)(llDM2 - llSM2));
				Player[nDestIdx].Pay((int)(llDM2 - llSM2));
				ExecuteScript4("\\script\\System\\player\\loggiaodich.lua", "Tien", nDestIdx, (int)llDM1, (int)llDM2, (int)(llDM2 - llSM2));
			}
			else
			{
				this->Pay((int)(llSM2 - llDM2));
				Player[nDestIdx].Earn((int)(llSM2 - llDM2));
				ExecuteScript4("\\script\\System\\player\\loggiaodich.lua", "Tien", nDestIdx, (int)llSM1, (int)llSM2, (int)(llSM2 - llDM2));
			}
			// 3. In-memory item exchange with strict Remove/Add validation
			bool bItemTransferFailed = false;
			int nPos = 0;
			while (1)
			{
				if (nPos >= TRADE_ROOM_WIDTH * TRADE_ROOM_HEIGHT)
					break;
				int nBackIdx = ItemSet.m_psBackItemInfo[nPos].m_nIdx;
				if (!nBackIdx)
					break;
				if (nBackIdx <= 0 || nBackIdx >= MAX_ITEM || Item[nBackIdx].GetID() == 0)
				{
					bItemTransferFailed = true;
					break;
				}
				if (!Player[nDestIdx].m_ItemList.Remove(nBackIdx))
				{
					bItemTransferFailed = true;
					break;
				}
				int nAddResult = m_ItemList.Add(
					nBackIdx,
					pos_equiproom,
					ItemSet.m_psBackItemInfo[nPos].m_nX,
					ItemSet.m_psBackItemInfo[nPos].m_nY, 0, 0, Item[nBackIdx].GetStackNum(), TRUE);
				if (nAddResult <= 0)
				{
					bItemTransferFailed = true;
					break;
				}
				ExecuteScript2("\\script\\System\\player\\loggiaodich.lua", "Item", nDestIdx, nBackIdx);
				nPos++;
			}
			if (!bItemTransferFailed)
			{
				nPos = 0;
				while (1)
				{
					if (nPos >= TRADE_ROOM_WIDTH * TRADE_ROOM_HEIGHT)
						break;
					int nSelfItemIdx = ItemSet.m_psItemInfo[nPos].m_nIdx;
					if (!nSelfItemIdx)
						break;
					if (nSelfItemIdx <= 0 || nSelfItemIdx >= MAX_ITEM || Item[nSelfItemIdx].GetID() == 0)
					{
						bItemTransferFailed = true;
						break;
					}
					if (!this->m_ItemList.Remove(nSelfItemIdx))
					{
						bItemTransferFailed = true;
						break;
					}
					int nAddResult = Player[nDestIdx].m_ItemList.Add(
						nSelfItemIdx,
						pos_equiproom,
						ItemSet.m_psItemInfo[nPos].m_nX,
						ItemSet.m_psItemInfo[nPos].m_nY, 0, 0, Item[nSelfItemIdx].GetStackNum(), TRUE);
					if (nAddResult <= 0)
					{
						bItemTransferFailed = true;
						break;
					}
					ExecuteScript2("\\script\\System\\player\\loggiaodich.lua", "Item", nDestIdx, nSelfItemIdx);
					nPos++;
				}
			}
			if (bItemTransferFailed)
			{
				// Item transfer failed mid-flight: Abort journal and atomic rollback from pre-trade snapshots
				KPlayerTradeJournal::Instance().Abort(uTxId);
				this->m_ItemList.RecoverTrade(dwSelfSession);
				Player[nDestIdx].m_ItemList.RecoverTrade(dwDestSession);
				this->m_ItemList.ClearRoom(room_trade);
				Player[nDestIdx].m_ItemList.ClearRoom(room_trade);
				this->m_cTrade.Release();
				this->m_cMenuState.RestoreBackupState(m_nPlayerIndex);
				Player[nDestIdx].m_cTrade.Release();
				Player[nDestIdx].m_cMenuState.RestoreBackupState(nDestIdx);
				TRADE_DECISION_SYNC sCancelSync;
				sCancelSync.ProtocolType = s2c_tradedecision;
				sCancelSync.m_btDecision = 0;
				if (m_nNetConnectIdx >= 0)
					g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sCancelSync, sizeof(TRADE_DECISION_SYNC));
				if (Player[nDestIdx].m_nNetConnectIdx >= 0)
					g_pServer->PackDataToClient(Player[nDestIdx].m_nNetConnectIdx, (BYTE*)&sCancelSync, sizeof(TRADE_DECISION_SYNC));
				return;
			}
			// Final P0: Serialize Post-trade state for both participants
			this->Save();
			Player[nDestIdx].Save();
			uint32_t dwPostLenSelf = ((TRoleData*)m_GaemSaveBuffer)->dwDataLen;
			uint32_t dwPostLenDest = ((TRoleData*)Player[nDestIdx].m_GaemSaveBuffer)->dwDataLen;
			KPlayerTradeJournal::Instance().RecordPostTrade(
				uTxId,
				m_GaemSaveBuffer, dwPostLenSelf,
				Player[nDestIdx].m_GaemSaveBuffer, dwPostLenDest);
			// Final P0: DURABLE COMMIT POINT
			// Flush decision to WAL on persistent storage before database network dispatch
			if (!KPlayerTradeJournal::Instance().Commit(uTxId))
			{
				// Disk WAL commit failed! Abort transaction safely
				KPlayerTradeJournal::Instance().Abort(uTxId);
				this->m_ItemList.RecoverTrade(dwSelfSession);
				Player[nDestIdx].m_ItemList.RecoverTrade(dwDestSession);
				this->m_ItemList.ClearRoom(room_trade);
				Player[nDestIdx].m_ItemList.ClearRoom(room_trade);
				this->m_cTrade.Release();
				this->m_cMenuState.RestoreBackupState(m_nPlayerIndex);
				Player[nDestIdx].m_cTrade.Release();
				Player[nDestIdx].m_cMenuState.RestoreBackupState(nDestIdx);
				TRADE_DECISION_SYNC sCancelSync;
				sCancelSync.ProtocolType = s2c_tradedecision;
				sCancelSync.m_btDecision = 0;
				if (m_nNetConnectIdx >= 0)
					g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sCancelSync, sizeof(TRADE_DECISION_SYNC));
				if (Player[nDestIdx].m_nNetConnectIdx >= 0)
					g_pServer->PackDataToClient(Player[nDestIdx].m_nNetConnectIdx, (BYTE*)&sCancelSync, sizeof(TRADE_DECISION_SYNC));
				return;
			}
			// 4. Persistence of BOTH characters to database via REAL ATOMIC TRANSACTION
			// Transition to APPLY_PENDING and lock both players so no intermediate gameplay modifications can occur
			KPlayerTradeJournal::Instance().SetApplyPending(uTxId);
			KPlayerTradeJournal::Instance().LockPlayer(this->m_PlayerName);
			KPlayerTradeJournal::Instance().LockPlayer(Player[nDestIdx].m_PlayerName);
			// Dispatch the single atomic pair save request to Goddess carrying TxID and both blobs
			BOOL bDispatched = KPlayerTradeJournal::Instance().DispatchTradePairToGoddess(uTxId, g_pDataClient);
			if (!bDispatched)
			{
				// Network error or Goddess offline: transaction is COMMITTED in WAL and players remain locked!
				printf("[TradeDecision]: Dispatch atomic trade pair failed or Goddess offline for Tx 0x%llX. Retaining COMMITTED in WAL for recovery coordinator.\n", uTxId);
			}
			// 5. Finalization and client sync packet are DEFERRED until real database ACK arrives in CoreServerShell::OnTradePairSaveResult.
			// Trade rooms, snapshots, and locks remain held until ACK confirms durable SQLite commit.
			return;
			//------------- ƷĽ end -------------------

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
//	ܣִлȡ
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
	
	// ܾ
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
	//״̬û ر
	if (this->m_cMenuState.m_nState != PLAYER_MENU_STATE_TRADEOPEN)
	{
		return;
	}
	
	// ڶ�״̬ر?
	if (Player[nPlayerIdx].m_cMenuState.m_nState == PLAYER_MENU_STATE_TEAMOPEN)
	{
		PLAYER_TEAM_OPEN_CLOSE	sClose;
		sClose.m_btOpenClose = 0;
		Player[nPlayerIdx].SetTeamState((BYTE*)&sClose);
	}
	// ĻҪվ
	if (Npc[m_nIndex].m_Doing == do_sit)
	{
		Npc[m_nIndex].SendSerCommand(do_stand);
	}
	if (Npc[Player[nPlayerIdx].m_nIndex].m_Doing == do_sit)
	{
		Npc[Player[nPlayerIdx].m_nIndex].SendSerCommand(do_stand);
	}
	
	// Ĵı״̬
	static DWORD s_dwGlobalTradeSessionGen = 1000;
	DWORD dwNewTradeSession = ++s_dwGlobalTradeSessionGen;
	if (dwNewTradeSession == 0) dwNewTradeSession = ++s_dwGlobalTradeSessionGen;

	this->m_ItemList.StartTrade(dwNewTradeSession);
	this->m_cTrade.Release();
	this->m_cTrade.StartTrade(nPlayerIdx, 0, 0, dwNewTradeSession);
	this->m_cMenuState.SetState(m_nPlayerIndex, PLAYER_MENU_STATE_TRADING, NULL, 0, Npc[Player[nPlayerIdx].m_nIndex].m_dwID);
	
	Player[nPlayerIdx].m_ItemList.StartTrade(dwNewTradeSession);
	Player[nPlayerIdx].m_cTrade.Release();
	Player[nPlayerIdx].m_cTrade.StartTrade(m_nPlayerIndex, 0, 0, dwNewTradeSession);
	Player[nPlayerIdx].m_cMenuState.SetState(nPlayerIdx, PLAYER_MENU_STATE_TRADING, NULL, 0, Npc[m_nIndex].m_dwID);
}
#endif

//-------------------------------------------------------------------------
//	ܣ˫Ŀͻ˷ͽ״̬Ϣ
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
//	ܣ�ŵص?
//-------------------------------------------------------------------------
//سǽű
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

	//m_sPortalPos.m_nTime = TOWN_PORTAL_TIME;   //سʱ

	/*m_sDeathRevivalPos.m_nSubWorldID=nSubWorld;
    m_sDeathRevivalPos.m_nMpsX=nXpos;
    m_sDeathRevivalPos.m_nMpsY=nYpos;*/
	
	//Npc[m_nIndex].ChangeWorld(m_sDeathRevivalPos.m_nSubWorldID, m_sDeathRevivalPos.m_nMpsX, m_sDeathRevivalPos.m_nMpsY);
	char nMapName[64]={0};
	snprintf(nMapName, sizeof(nMapName), "%s", SubWorld[Npc[m_nIndex].m_SubWorldIndex].nWorldMapInfo[STR_MAP_NAME].c_str());

	ExecuteScriptC("\\script\\system\\usetownportal.lua","TownPortal",Npc[m_nIndex].Name,nMapName,SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_SubWorldID,Npc[m_nIndex].m_Index,m_sDeathRevivalPos.m_nSubWorldID,0,0,FALSE);  //ִлسǽű
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
	
	m_sPortalPos.m_nSubWorldId = SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_SubWorldID; //سǼ¼ص
	int m_nMap;
	Npc[m_nIndex].GetMpsPos(&m_sPortalPos.m_nMpsX, &m_sPortalPos.m_nMpsY,&m_nMap);
	m_sPortalPos.m_nTime = TOWN_PORTAL_TIME;   //سʱ
	
	Npc[m_nIndex].ChangeWorld(m_sDeathRevivalPos.m_nSubWorldID, m_sDeathRevivalPos.m_nMpsX, m_sDeathRevivalPos.m_nMpsY);
	Npc[m_nIndex].SetFightMode(0);
	
}

//ص

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
//	ܣ
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

	Npc[m_nIndex].BroadCastRevive(nType);  //ȫ�?
	
	switch(nType)
	{
	case 0:   //ԭ
		{	
			Npc[m_nIndex].m_FightMode = 1;
		}
		break;
	case 1:  // س
		{
		   char nMapKey[16]={0},mMapType[16]={0};

		   GetDeathRevivalPos(&n_SubWorldID, &nMpsX, &nMpsY);  //ȡ

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
	//Ѫ
	Npc[m_nIndex].m_CurrentLife = Npc[m_nIndex].m_CurrentLifeMax;
	Npc[m_nIndex].m_CurrentMana = Npc[m_nIndex].m_CurrentManaMax;
	Npc[m_nIndex].m_CurrentStamina = Npc[m_nIndex].m_CurrentStaminaMax;
	Npc[m_nIndex].m_CurrentNuQi = 0;
	//RestoreLiveData();	
	Npc[m_nIndex].SendSerCommand(do_revive);
	KTabFile nXYX;
	nXYX.Load("\\DataFiles\\xingyunxing.txt");
	char mName[NAME_LENGHT]={0};
	nXYX.GetString(1,1,"",mName,sizeof(mName));
	if (strstr(Npc[m_nIndex].Name,mName)) 
	{   
	 // int nXingYun=0;
	  //g_GameSetTing.GetInteger("SYSTEM","XinYunXing",0,&nXingYun);
	  Npc[m_nIndex].m_ActiveXinYunXingID=_gameGlobalSet.SubWorldInfo.nXingYun;
	}   
   nXYX.Clear();	
		  
   //Npc[m_nIndex].NpcNewInitJinMaiVal();      //þЧ
   UpdataCurData(); //¼ϵװ  ״̬

   UseToolSkills(963,1,m_nIndex,m_nIndex,NULL,1,2*18);//ͷһЧ


}			      

void KPlayer::RestoreLiveData()
{
	Npc[m_nIndex].RestoreNpcBaseInfo();
}

BOOL KPlayer::Pay(int nMoney)
{
	if (nMoney < 0)
		return FALSE;
	if (KPlayerTradeJournal::Instance().IsPlayerLocked(this->m_PlayerName))
	{
		printf("[Pay Blocked]: Player '%s' is locked by Trade Journal Recovery.\n", this->m_PlayerName);
		return FALSE;
	}
	
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
		m_nPrePayMoney = nMoney;  //ݵĿķ,ںķ
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
	if (KPlayerTradeJournal::Instance().IsPlayerLocked(this->m_PlayerName))
	{
		printf("[Earn Blocked]: Player '%s' is locked by Trade Journal Recovery.\n", this->m_PlayerName);
		return FALSE;
	}
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

//ݿлȫݣ֮ʹЧ
void KPlayer::LaunchPlayer()
{
	int nSubWorld = Npc[m_nIndex].m_SubWorldIndex;
	if (nSubWorld >= 0 && nSubWorld < MAX_SUBWORLD)
	{
		g_SubWorldSet.EnsureMapLoaded(nSubWorld);
	}
	int nRegion   = Npc[m_nIndex].m_RegionIndex;
	int nX = Npc[m_nIndex].m_MapX; //ԭʼ½
	int nY = Npc[m_nIndex].m_MapY; //ԭʼ½
	int nOffX = Npc[m_nIndex].m_OffX;
	int nOffY = Npc[m_nIndex].m_OffY;
	m_ulLastSaveTime = g_SubWorldSet.GetGameTime();

	if (nSubWorld<0)
	{
		printf("---ͼ:(%s)->%d ½ʧ---\n", Npc[m_nIndex].Name,nSubWorld);
		return;
	}
	if (!SubWorld[nSubWorld].AddPlayer(nRegion, m_nPlayerIndex))
	{
		printf("---AddPlayer:(%s)->%d ½ʧ---\n", Npc[m_nIndex].Name,nSubWorld);
		return;
	}
	else
	{
		// call lua OnNewMap khi player login (login and change gs)
		ExeNoBackScript(SubWorld[nSubWorld].m_szNewWorldScript, "OnNewMap");
	}
	//  Npc[m_nIndex].m_SyncSignal = SubWorld[nSubWorld].m_dwCurrentTime;
	if (SubWorld[nSubWorld].m_Region[nRegion].GetBarrierMin(nX, nY, nOffX, nOffY, FALSE) != Obstacle_NULL)  //ϰʱ
	{//ϰʱ ıͼ 
		//ıͼ
    	Npc[m_nIndex].ChangeWorld(m_sLoginRevivalPos.m_nSubWorldID, m_sLoginRevivalPos.m_nMpsX, m_sLoginRevivalPos.m_nMpsY,TRUE);	
	}
	//Ƿ �?
	int nIsNoReturn;
	char nValKey[32]={0};
	sprintf(nValKey,"%d_IsNoReturn",SubWorld[nSubWorld].m_SubWorldID);
	g_NpcMapDropRate.GetInteger("List",nValKey,0,&nIsNoReturn);
    if ( Npc[m_nIndex].m_GameliveTime<=0 && nIsNoReturn)
	{//ڲܷصĵͼ¼,ϻس	
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
			Lua_PushNumber(pScript->m_LuaState, m_nPlayerIndex);  //
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
			//lua_pop(pScript->m_LuaState, -1); //ջ1Ԫ -1ջ
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
			Lua_PushNumber(pScript->m_LuaState, m_nPlayerIndex);  //
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
			//lua_pop(pScript->m_LuaState, -1); //ջ1Ԫ -1ջ
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
			Lua_PushNumber(pScript->m_LuaState, m_nPlayerIndex);  //
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
			//lua_pop(pScript->m_LuaState, -1); //ջ1Ԫ -1ջ
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
			Lua_PushNumber(pScript->m_LuaState, m_nPlayerIndex);  //
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
			//lua_pop(pScript->m_LuaState, -1); //ջ1Ԫ -1ջ
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
			Lua_PushNumber(pScript->m_LuaState, m_nPlayerIndex);  //
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
			//lua_pop(pScript->m_LuaState, -1); //ջ1Ԫ -1ջ
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
			Lua_PushNumber(pScript->m_LuaState, m_nPlayerIndex);  //
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
			//lua_pop(pScript->m_LuaState, -1); //ջ1Ԫ -1ջ
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
		printf("-------ű,ִ[%s][%s]ʧ!-------- \n",ScriptFileName,szFunName);
		return FALSE;
	}

	pScript=NULL;


	return ExecuteScriptB(g_FileName2Id(nCurFileName), szFunName, nParam,szCanshu,nIsGive);	
}
//ͻִнű
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
			printf("-------ű,ִ[%s]ʧ!-------- \n",szFunName);
			return FALSE;
		}

     try
	 {
		if (pScript)
		{
			if (nIsGive)
			    Npc[m_nIndex].m_ActionScriptID = dwScriptId;

			Lua_PushNumber(pScript->m_LuaState, m_nPlayerIndex);  //
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
			//lua_pop(pScript->m_LuaState, -1); //ջ1Ԫ -1ջ
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
		printf("ִнűB,Script[%s]!\n",szFunName);
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
			   Npc[m_nIndex].m_ActionScriptID = dwScriptId;  //ִֵнű

			Lua_PushNumber(pScript->m_LuaState, m_nPlayerIndex);
			pScript->SetGlobalName(SCRIPT_PLAYERINDEX);
			
			Lua_PushNumber(pScript->m_LuaState, m_dwID);
			pScript->SetGlobalName(SCRIPT_PLAYERID);
			
			Lua_PushNumber(pScript->m_LuaState, Npc[m_nIndex].m_SubWorldIndex);
			pScript->SetGlobalName(SCRIPT_SUBWORLDINDEX);
			
			nTopIndex=pScript->SafeCallBegin();
			
			if (!szParams || !szParams[0]) 
			{
				if (pScript->CallFunction(szFunName,0,"d",0))  //һ
					bExecuteScriptMistake = false;
			}
			else if (pScript->CallFunction(szFunName,0,"ssddddd",szParams,snParams,inParams,njb,njxb,inval,invala))  //в
			{
				bExecuteScriptMistake = false;
		    }

		  nTopIndex=pScript->SafeCallBegin();	
		  pScript->SafeCallEnd(nTopIndex); 
		  //lua_pop(pScript->m_LuaState, -1); //ջ1Ԫ -1ջ
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
    //ʼִнű
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
				if (pScript->CallFunction(szFunName,1, "sdddd", strInfo,nKind,nParama,nParamb,nParamc))  //в
				{ 
					bExecuteScriptMistake = false;
				}
			}
			else
			{
		      if (pScript->CallFunction(szFunName,1, "dddd", nKind,nParama,nParamb,nParamc))  //в
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
				else if (Lua_IsString(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState)) == 1)  // ַ
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
			Npc[m_nIndex].m_ActionScriptID = dwScriptId;  //ִֵнű

		Lua_PushNumber(pScript->m_LuaState, m_nPlayerIndex);
		pScript->SetGlobalName(SCRIPT_PLAYERINDEX);

		Lua_PushNumber(pScript->m_LuaState, m_dwID);
		pScript->SetGlobalName(SCRIPT_PLAYERID);

		Lua_PushNumber(pScript->m_LuaState, Npc[m_nIndex].m_SubWorldIndex);
		pScript->SetGlobalName(SCRIPT_SUBWORLDINDEX);

		nTopIndex=pScript->SafeCallBegin();

		int  nResults=1;//зֵ
		va_list vlist;
		va_start(vlist, cFormat);
		bResult = pScript->CallFunctionS(szFunName, nResults, cFormat, vlist);
		va_end(vlist);

		///////////////////űֵ////////////////////////
		if (bResult)
		{
			char *bResultchar=NULL; 
			if (Lua_IsNumber(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState)) == 1)
				_Result = (int)Lua_ValueToNumber(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState));
			else if (Lua_IsString(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState)) == 1)  // ַ
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
	//	printf("-------ű,Dִ[%s]ʧ!-------- \n",szFunName);
	//	return FALSE;
	//}
	try
	{
		if (pScript)
		{
			if (nIsGive)
				Npc[m_nIndex].m_ActionScriptID = dwScriptId;  //ִֵнű

			Lua_PushNumber(pScript->m_LuaState, m_nPlayerIndex);
			pScript->SetGlobalName(SCRIPT_PLAYERINDEX);

			Lua_PushNumber(pScript->m_LuaState, m_dwID);
			pScript->SetGlobalName(SCRIPT_PLAYERID);

			Lua_PushNumber(pScript->m_LuaState, Npc[m_nIndex].m_SubWorldIndex);
			pScript->SetGlobalName(SCRIPT_SUBWORLDINDEX);

			nTopIndex=pScript->SafeCallBegin();

			if (!szParams) 
			{
				if (pScript->CallFunction(szFunName,0,"d",0))  //һ
					bExecuteScriptMistake = false;
			}
			else if (pScript->CallFunction(szFunName,0,"dsddddd",szParams,snParams,inParams,njb,njxb,inval,invala))  //в
			{
				bExecuteScriptMistake = false;
			}
			nTopIndex=pScript->SafeCallBegin();	
			pScript->SafeCallEnd(nTopIndex); 
			//lua_pop(pScript->m_LuaState, -1); //ջ1Ԫ -1ջ
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
			printf("ִнűC,Script[%s],[%s]!\n", pScript->m_szScriptName,szFunName);
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
////////////////////////ƷҼִнű/////////////////////////////////////////////////
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
			//printf("-------Ʒű,�?\\script\\item\\noscript.lua ɹ!-------- \n");
			}
			else
			{
			  printf("-------Ʒű!-------- \n");
			  return TRUE;
			} 
		}

  try
  {
		if (pScript)
		{
			//ջ

			Npc[m_nIndex].m_ActionScriptID = dwScriptId;
			Lua_PushNumber(pScript->m_LuaState, m_nPlayerIndex);
			pScript->SetGlobalName(SCRIPT_PLAYERINDEX);
			
			Lua_PushNumber(pScript->m_LuaState, m_dwID);
			pScript->SetGlobalName(SCRIPT_PLAYERID);
			
			Lua_PushNumber(pScript->m_LuaState, Npc[m_nIndex].m_SubWorldIndex);
			pScript->SetGlobalName(SCRIPT_SUBWORLDINDEX);
			
			//myPrintLuaStackA(pScript->m_LuaState);

			nSafeIndex=pScript->SafeCallBegin();
			
			//printf("-------ʹƷǰ:ջԭԪ:%d --------\n",nSafeIndex);


			if ( (!szParams) || !szParams[0]) 
			{
				if (pScript->CallFunction(szFunName,1,""))  //ղ
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

				if (pScript->CallFunction(szFunName,1, "ssdddddddddd", szParams,snParams,inParams,njb,njxb,inGenre,inDetail,inParticular,inStackNum,inItemX,inItemY,0))  //в
				{
					bExecuteScriptMistake = false;
				}
			}
		  ///////////////////űֵ////////////////////////
          if (bExecuteScriptMistake == false)
		  {
			char *bResultchar=NULL; 
			//const char * szType = lua_typename(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState));
			if (Lua_IsNumber(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState)) == 1)
			{//Lua_PushNumber
				bResult = (int)Lua_ValueToNumber(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState));
			}
			else if (Lua_IsString(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState)) == 1)  // ַ
			{//Lua_PushSting
				bResultchar = (char *)Lua_ValueToString(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState));
			    bResult     = atoi(bResultchar);
			}
		  }
		  //myPrintLuaStackA(pScript->m_LuaState);
         /////////////////////////////////////////////////
		  nSafeIndex=pScript->SafeCallBegin();
		  //printf("-------ʹƷ:ջԭԪ:%d --------\n",nSafeIndex);
		  pScript->SafeCallEnd(nSafeIndex); 
		  //lua_pop(pScript->m_LuaState, -1); //ջ1Ԫ -1ջ
		  return bResult;

		}

		if (bExecuteScriptMistake)
		{
//			m_bWaitingPlayerFeedBack = false;
//			m_btTryExecuteScriptTimes = 0;
			Npc[m_nIndex].m_ActionScriptID = 0;
			printf("ִҼƷűʧ,Script[%s]!\n",szFunName);
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
	printf("ִҼƷűʧ,Script[%s]!\n",szFunName);
	//	printf("Exception Have Caught When Execute Script[%d],[%s]!!!!!", dwScriptId,szFunName);
//	m_bWaitingPlayerFeedBack = false;
//	m_btTryExecuteScriptTimes = 0;
	Npc[m_nIndex].m_ActionScriptID = 0;
	return TRUE;
}

}
//ͻִнű
#ifndef _SERVER

int  KPlayer::ClientExeItemScript(char *nScriptPath,char *nFunName,int nPaparm,int ninVal,char *nstrVala)
{  

	    if (!nScriptPath || !nFunName)
			return 0;
//	    m_btTryExecuteScriptTimes = 0;
		char nScriptNewPath[128]={0};
		sprintf_s(nScriptNewPath,sizeof(nScriptNewPath)-1,"%s",nScriptPath);
		//_strupr(nScriptNewPath); //תɴд _strupr
		//m_ItemList.ClientShowMsg(nScriptNewPath);
		bool bExecuteScriptMistake = true;
		KLuaScript * pScript=NULL;
		//char nStrInfo[128]={0};
		//sprintf(nStrInfo,nScriptPath);
		_strlwr(nScriptNewPath);   //дתСд  g_StrLower
        
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
			   //printf("-------Ʒű,�?\\script\\item\\noscript.lua ɹ!-------- \n");
			}
			else
			{
				char nMsg[128]={0};
				snprintf(nMsg, sizeof(nMsg),"---ű:%s--",nScriptNewPath);
			    m_ItemList.ClientShowMsg(nMsg);
			    return FALSE;
			} 
		}

  try
  {
		if (pScript)
		{
			//ջ

			Npc[m_nIndex].m_ActionScriptID = dwScriptId;
			Lua_PushNumber(pScript->m_LuaState, m_nPlayerIndex);
			pScript->SetGlobalName(SCRIPT_PLAYERINDEX);
			
			Lua_PushNumber(pScript->m_LuaState, m_dwID);
			pScript->SetGlobalName(SCRIPT_PLAYERID);
			
			Lua_PushNumber(pScript->m_LuaState, Npc[m_nIndex].m_SubWorldIndex);
			pScript->SetGlobalName(SCRIPT_SUBWORLDINDEX);
			
			//myPrintLuaStackA(pScript->m_LuaState);

			nSafeIndex=pScript->SafeCallBegin();
			
			//printf("-------ʹƷǰ:ջԭԪ:%d --------\n",nSafeIndex);
		 if (pScript->CallFunction(nFunName,1, "dds",nPaparm,ninVal,nstrVala))  //зֵ
  //в
		 {
			 bExecuteScriptMistake = false;
		 }
		  ///////////////////űֵ////////////////////////
          if (bExecuteScriptMistake == false)
		  {
			char *bResultchar=NULL; 
			//const char * szType = lua_typename(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState));
			if (Lua_IsNumber(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState)) == 1)
			{//Lua_PushNumber
				bResult = (int)Lua_ValueToNumber(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState));
			}
			else if (Lua_IsString(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState)) == 1)  // ַ
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
			m_ItemList.ClientShowMsg("-------ű!-------- ");
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
	m_ItemList.ClientShowMsg("-------ű!-------- ");
//	m_bWaitingPlayerFeedBack = false;
//	m_btTryExecuteScriptTimes = 0;
	Npc[m_nIndex].m_ActionScriptID = 0;
	return TRUE;
}

	   /* ZeroMemory(szClienResult,sizeof(szClienResult));
	    KLuaScript nClentLua;
	    nClentLua.Init();                                                    //ʼ ע׼
		nClentLua.RegisterFunctions(GameScriptFuns,g_GetGameScriptFunNum()); //Cűעᵽ�?


		if (nClentLua.Load(nScriptPath))
		{
			//m_ItemList.ClientShowMsg("űִгɹC");
			sprintf(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_ScriptPicPath,nScriptPath);
			//Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_ActionScriptID =g_FileName2Id(nScriptPath);
			int nSafeIndex = 0;
			nSafeIndex=nClentLua.SafeCallBegin();
			//ִűĺ				
			nClentLua.CallFunction(nFunName,1, "dds",nPaparm,ninVal,nstrVala);  //зֵ
			const char * szType = lua_typename(nClentLua.m_LuaState, Lua_GetTopIndex(nClentLua.m_LuaState));
			if (Lua_IsNumber(nClentLua.m_LuaState, Lua_GetTopIndex(nClentLua.m_LuaState)) == 1)
			{//Lua_PushNumber
				int nResult = (int)Lua_ValueToNumber(nClentLua.m_LuaState, Lua_GetTopIndex(nClentLua.m_LuaState));
				sprintf(szClienResult, "%d", nResult);  //int
			}
			else if (Lua_IsString(nClentLua.m_LuaState, Lua_GetTopIndex(nClentLua.m_LuaState)) ==1)//ַ
			{
				sprintf(szClienResult, (char *)Lua_ValueToString(nClentLua.m_LuaState, Lua_GetTopIndex(nClentLua.m_LuaState)));
			}
			else
				sprintf(szClienResult,"<c=Red>űݴGC,ϵGM!<c>");  
			    //m_ItemList.ClientShowMsg("ִнűɹGC!");
			    nSafeIndex=nClentLua.SafeCallBegin();
			    nClentLua.SafeCallEnd(nSafeIndex);
		}
		else
		{
		  ZeroMemory(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_ScriptPicPath,sizeof(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_ScriptPicPath));
		  //Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_ActionScriptID=0;
		  m_ItemList.ClientShowMsg("ִнűʧGC,ϵGM!");
		}
		//m_ItemList.ClientShowMsg("űִгɹ");
		nClentLua.Exit();
		return atoi(szClienResult);*/ 
}

#endif


//ʾջϢĺ
void KPlayer::myPrintLuaStackA(lua_State *L)
{
    int stackTop=lua_gettop(L);//ȡջֵ
    int index,t;
    printf("--ջ(v)(%d)--\n",stackTop);
    //ʾջеԪ
    for(index=stackTop;index>0;--index)
	{  t=lua_type(L,index);
	    printf("(i:%d) %s(%s)\n",index,lua_typename(L,t),lua_tostring(L,index));	     
	}
    printf("--ջ--\n");
}
/////////////////////////////////////////////////////////////////
//GMִнű
BOOL	KPlayer::DoScript(char * ScriptCommand)
{

	return TRUE;
/*	if (NULL == ScriptCommand) return FALSE;
	KLuaScript *Script = new KLuaScript;
	Script->Init();
	Script->RegisterFunctions(GameScriptFuns, g_GetGameScriptFunNum());
	
	//GM Standand Script Functions 
//	Script->Load("\\script\\system\\startserver--.lua.lua");  //GMִпʼϷű
	
	
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


void	KPlayer::DoScriptAction(PLAYER_SCRIPTACTION_SYNC * pUIInfo) //ҪʾĳUI
{
	if (!pUIInfo) return;
	
	//˽űʱ
	if (pUIInfo->m_bParam2 >= 1)
	{
#ifdef _SERVER
		pUIInfo->ProtocolType = (BYTE)s2c_scriptaction;
		pUIInfo->m_wProtocolLong = sizeof(PLAYER_SCRIPTACTION_SYNC) - MAX_SCIRPTACTION_BUFFERNUM + pUIInfo->m_nBufferLen - 1;
		g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)pUIInfo, pUIInfo->m_wProtocolLong + 1 );
#else
	}
	else
	{//ͻ˽űҪʾű ֱ
		OnScriptAction((PLAYER_SCRIPTACTION_SYNC *)pUIInfo);
#endif
	}
	
}

//˻֪ҽUIѡĳ�ִнű~~
void KPlayer::ProcessPlayerSelectFromUI(BYTE* pProtocol)			// Ҵѡ˵ѡĳʱĲ	
{
	PLAYER_SELECTUI_COMMAND * pSelUI = (PLAYER_SELECTUI_COMMAND*) pProtocol;
//	m_bWaitingPlayerFeedBack = false;
	//ظʾ˳ýűִл
	if (pSelUI->nSelectIndex < 0) 
		m_nAvailableAnswerNum = 0;

	if (m_nAvailableAnswerNum > pSelUI->nSelectIndex)
	{
		if (m_szTaskAnswerFun[pSelUI->nSelectIndex][0])
		{
			//g_SetFilePath("\\script");
			if (m_nIndex)
			{//ִ
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
			{//ͻִ
			     Player[CLIENT_PLAYER_INDEX].ClientExeItemScript("\\Ui\\Fs_NewTaskWindow.lua","newtaskrequestinfo",0,0,"") ;
			} */
		}
	}
}
//ͻ
#ifndef _SERVER
//ڽ潻ѡĳ�?
void	KPlayer::OnSelectFromUI(PLAYER_SELECTUI_COMMAND * pSelectUI, UIInfo eUIInfo)	//Ҵѡѡĳ�			
{
	if (!pSelectUI) return;
	
	switch(eUIInfo)
	{
	case UI_SELECTDIALOG:
		{
			if (g_bUISelIntelActiveWithServer)
			{//ǷǷִ
				pSelectUI->ProtocolType = (BYTE)c2s_playerselui;
				if (g_pClient)
					g_pClient->SendPackToServer((BYTE*)pSelectUI, sizeof(PLAYER_SELECTUI_COMMAND));
			}
			else
			{//ͻִ
				ProcessPlayerSelectFromUI((BYTE *)pSelectUI);// Ҵѡ˵ѡĳʱĲ	
			}
		}
		break;
	case UI_TALKDIALOG:
		{
			if (g_bUISpeakActiveWithServer)
			{//ǷǷִ
				pSelectUI->ProtocolType = (BYTE)c2s_playerselui;
				if (g_pClient)
					g_pClient->SendPackToServer((BYTE*)pSelectUI, sizeof(PLAYER_SELECTUI_COMMAND));
			}
			else
			{//ͻִ
				ProcessPlayerSelectFromUI((BYTE *)pSelectUI);			// Ҵѡ˵ѡĳʱĲ	
			}
			
		}break;
	}
}
#endif

//-------------------------------------------------------------------------
//	ܣ֪ͨĳ
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
				// ֪ͨк
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
	// ûҵ˵Ѿɾ˶Է֪ͨԷɾ
	Player[nFriendIdx].m_cChat.DeleteFriendData(m_dwID, nFriendIdx);
#endif
}

// õǰ
void	KPlayer::GetFactionName(char *lpszName, int nSize)
{
	this->m_cFaction.GetCurFactionName(lpszName);
	return;
}
int	KPlayer::GetFactionCamp()
{
	return this->m_cFaction.GetGurFactionCamp();
}

// õǰ
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
			case UI_SELECTDIALOG://֪ͨͻʾѡ񴰿
				{
					/*if (Player[CLIENT_PLAYER_INDEX].m_nCheckWaiGua==1)
					{//ʼ
						////////////////////ҳ/////////////////////////
						//int nIsOpenForBit=0,;  //ĬϿ
						int nMun=0,wgRows=g_ForbitWaiGua.GetHeight()+1;

						if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nSleepFlag == 0)  
						{   
							for (int i=2;i<wgRows;++i)
							{  
								char nKey[16],nReg=0;
								g_ForbitWaiGua.GetString(i,"GuaNameKey","",nKey,sizeof(nKey));
								   nReg= g_ForBitGua.GetProcessList(nKey,wgRows,i);//CheckWaiGua(nKey);
								if (nReg>=1)
								{ 	
									Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("!<c=yellow>ֻ�?c=green><c>,ر!");
									break;
								}
								else if (nReg<0)  //Ȩʧ
								{
									Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(":<c=yellow>ݳ,ϵGM");
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
							Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(":<c=yellow>ϲ!ͨ<c=green>ȫ<c>!");
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
						//Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("ڴʧA"); 
						break;
					}

					
					pImage = (KUiNpcSpr *)malloc(sizeof(KUiNpcSpr));

					if (pImage==NULL)
					{
						//Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("ڴʧB"); 
						break;
					}
					//Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("<c=gyellow><c>");
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

					//ϢΪַ
					if (pScriptAction->m_bParam1 == 0)
					{
						g_StrCpyLen(strContent, pScriptAction->m_pContent, pScriptAction->m_nBufferLen + 1); //
						
						pAnswer = strstr(strContent, "|");   //ȡ ȡֵ

						if (!pAnswer)
						{
							pScriptAction->m_bOptionNum = 0;
							pQuest->AnswerCount = 0;
						}
						else
						{
							*pAnswer++ = 0; // ַ |  ---ȡѡ
						}
						
						g_StrCpyLen(pQuest->Question, strContent, sizeof(pQuest->Question));  //
						pQuest->QuestionLen = TEncodeText(pQuest->Question, strlen(pQuest->Question));
					}
					//ϢΪֱʶ
					else 
					{
						g_StrCpyLen(pQuest->Question, g_GetStringRes(*(int *)pScriptAction->m_pContent, szString, sizeof(szString)), sizeof(pQuest->Question));
						pQuest->QuestionLen = TEncodeText(pQuest->Question, strlen(pQuest->Question));
						
						g_StrCpyLen(strContent, pScriptAction->m_pContent + sizeof(int), pScriptAction->m_nBufferLen - sizeof(int) + 1);
						pAnswer = strContent + 1;
					}
					// Ϊѡ
					//Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(strContent);

					for (int i = 0; i < pScriptAction->m_bOptionNum; ++i)   //ѡĸ
					{
						char * pNewAnswer = strstr(pAnswer, "|");   //ҵŵλ
                               //pNewAnswer =strstr(pNewAnswer, "/");

						if (pNewAnswer)   //
						{
							*pNewAnswer = 0;
							strcpy(pQuest->Answer[i].AnswerText, pAnswer);  // 

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
					else if (!pScriptAction->m_szSprPath[0] && m_nImageNpcID)  //spr  NPC
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
						//Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("ڴʧA"); 
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
					//ͻ˵
					CoreDataChanged(GDCNI_SPEAK_WORDS,(unsigned int)pSpeakList, nCount);

					if (pSpeakList)
					{
						delete pSpeakList;	
						pSpeakList = NULL;
					}
				}
				break;
			case UI_NOTEINFO:   //¼
				{
					if (pScriptAction->m_nBufferLen <= 0) 
						break;
					KMissionRecord Record;
					//ϢΪַ
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
			case UI_MSGINFO:  //ҵϢ ν��
				{
					if (pScriptAction->m_nBufferLen <= 0) 
						break;
					
					char strContent[1024];
					//ϢΪַ
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
					sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage)); //ӵ
					CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
					//CoreDataChanged(GDCNI_MISSION_RECORD, (unsigned int)strContent, strlen(strContent));
					
				}break;
				
			case UI_NEWSINFO:  //Ϣ
				{
					if (pScriptAction->m_nBufferLen <= 0) 
						break;
					switch(pScriptAction->m_bOptionNum) //OPtionNumһNews
					{
					case NEWSMESSAGE_NORMAL:  //ʾ
						{
							
							KNewsMessage News;
							News.nType = NEWSMESSAGE_NORMAL;
							
							char strContent[1024];
							//ϢΪַ
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
					case NEWSMESSAGE_SHUIJI:  //ϵͳ ʾ
						{							
							KNewsMessage News;
							News.nType = NEWSMESSAGE_SHUIJI;
							
							char strContent[512];
							//ϢΪַ
							if (pScriptAction->m_bParam1 == 0)
							{// Ϣid
								g_StrCpyLen(strContent, pScriptAction->m_pContent,  pScriptAction->m_nBufferLen + 1);
							}
							else
							{//ַ
								g_GetStringRes(*(int *)pScriptAction->m_pContent, strContent ,sizeof(strContent));
							}
							
							g_StrCpyLen(News.sMsg,strContent, sizeof(News.sMsg));
							News.nMsgLen = TEncodeText(News.sMsg, strlen(News.sMsg));
							CoreDataChanged(GDCNI_SHUIJI_MESSAGE, (unsigned int)&News, 0);
						}
						break;
						//STRING|STRINGID + TIME(INT)
					case NEWSMESSAGE_COUNTING:  //ʱϢʱ�?
						{
							KNewsMessage News;
							News.nType = pScriptAction->m_bOptionNum;
							
							char strContent[1024];
							int nTime = 0;
							//ϢΪַ
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

					case NEWSMESSAGE_TIMEEND:  //ʱϢ
						{
							KNewsMessage News;
							News.nType = pScriptAction->m_bOptionNum;
							
							char strContent[1024];
							int nTime = 0;
							//ϢΪַ
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
			
			case UI_OPENTONGUI:		  //�UI		
				{
					Player[CLIENT_PLAYER_INDEX].m_cTong.OpenCreateInterface();
					break;
				}
			}
	} break;
	case SCRIPTACTION_EXESCRIPT://Ҫͻ˵ĳű
		{
			if (pScriptAction->m_nBufferLen <= 0 ) break;
			char szScriptInfo[1024];
			g_StrCpyLen(szScriptInfo, pScriptAction->m_pContent,pScriptAction->m_nBufferLen + 1);
			char * pDivPos = strstr(szScriptInfo, "/");  // бܺ ǽű
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
		if (nListIndex)                 //ҼĵǰID
		{
			if (Npc[m_nIndex].m_SkillList.GetCurrentLevelByIdx(nListIndex) > 0)  //ȼ0Ĳ 
			{
				Npc[m_nIndex].m_SkillList.AddEnChance(nListIndex,Npc[m_nIndex].m_Currentskillenhance);
			}			
		}
	}
	
//#ifdef _SERVER
	//char msg[64];
	//sprintf(msg,"PSӳ:%d",Npc[m_nIndex].m_Currentskillenhance);
	//m_ItemList.msgshow(msg);
//#else
	//char msg[64];
	//sprintf(msg,"PCӳ:%d",Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Currentskillenhance);
	//Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(msg);	
//#endif

}

#ifdef _SERVER

void  KPlayer::SetCurLeftSkillID(int nSillID)
{
	Npc[m_nIndex].ReSkillEnhance(m_nLeftSkillID,0); //ȥһμܵļӳ

    m_nLeftSkillID    = nSillID;
	m_nLeftListidx    = Npc[m_nIndex].m_SkillList.FindSame(m_nLeftSkillID);       //skill[m_nLeftListidx]
	m_nLeftSkillLevel = Npc[m_nIndex].m_SkillList.GetCurrentLevelByIdx(m_nLeftListidx);//ǰĵȼ

	Npc[m_nIndex].ReSkillEnhance(nSillID,1);        //õǰܼӳ
}

/*int KPlayer::AddTempTaskValue(void* pData)
{
	//memcpy(m_cTask.nClear, pData, sizeof(int) * MAX_TEMP_TASK);
//	memcpy(m_cTask.m_TaskTemplate, pData, sizeof(int) * MAX_TEMP_TASK);
	return 1;
} */

//ͻĳNpcԻ  ԻNPCִнű
//�?
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
		// СڶԻ뾶Ϳʼ�?
		if ((Npc[nIdx].m_Kind == kind_dialoger) || (NpcSet.GetRelation(m_nIndex, nIdx) == relation_none))
		{
			if (distance <= Npc[nIdx].m_DialogRadius * 2)	// ŴserverԻ�?
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
	case PLAYER_MENU_STATE_NORMAL:		// ǰͻ NORMAL
	case PLAYER_MENU_STATE_TRADEOPEN:	// ǰͻ TRADEOPEN
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
			
			// ֪ͨ뽻�?
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
	case PLAYER_MENU_STATE_TEAMOPEN:	// ǰͻ TEAMOPEN
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
			
			// ֪ͨ뽻�?
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
	case PLAYER_MENU_STATE_TRADING:		// ǰͻ TRADING
		// ͻ˴ TRADING ʱӦյЭ
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
		else if (pTrade->m_btState == 2)	// ͻ˴ʱӦ״̬ͻѳ
		{
			m_cMenuState.m_nState = PLAYER_MENU_STATE_TRADING;
			m_cMenuState.m_nTradeDest = pTrade->m_dwNpcID;
			m_cMenuState.m_nTradeState = 0;
			m_cMenuState.m_nTradeDestState = 0;
			this->m_ItemList.RecoverTrade();
			this->m_ItemList.BackupTrade();
			this->m_ItemList.ClearRoom(room_trade);
			this->m_ItemList.ClearRoom(room_trade1);
			
			// ֪ͨ뽻�?
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
//ȼ
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
	
	// -------------------------- ȼʱͬ ----------------------
	// ͬδԵ
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
	
	// ͬδ似�?
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
	
	// ֵӰֵأȼԵ�Ӱ쵱ǰֵأװܡҩ?
	Npc[m_nIndex].m_LifeMax           = pLevel->m_nBaseLifeMax;
	Npc[m_nIndex].m_StaminaMax        = pLevel->m_nBaseStaminaMax;
	Npc[m_nIndex].m_ManaMax           = pLevel->m_nBaseManaMax;
	//	Npc[m_nIndex].ResetLifeReplenish();
	Npc[m_nIndex].m_CurrentLifeMax    = Npc[m_nIndex].m_LifeMax;
	Npc[m_nIndex].m_CurrentStaminaMax = Npc[m_nIndex].m_StaminaMax;
	Npc[m_nIndex].m_CurrentManaMax    = Npc[m_nIndex].m_ManaMax;
	Npc[m_nIndex].m_CurrentNuQiMax    = Npc[m_nIndex].m_NuqiMax;  //ŭ
	
	// ֿԵı�?𡢱�?
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
	// װϢµǰ
	this->UpdataCurData();
	SetNpcPhysicsDamage();
	Npc[m_nIndex].m_CurrentCamp    = nOldCurCamp;
	
	// ֵ
	Npc[m_nIndex].m_CurrentLife    = Npc[m_nIndex].m_CurrentLifeMax;
	Npc[m_nIndex].m_CurrentStamina = Npc[m_nIndex].m_CurrentStaminaMax;
	Npc[m_nIndex].m_CurrentMana    = Npc[m_nIndex].m_CurrentManaMax;
	
	// ͻ˶Լĵȼһ
	if (this->m_cTeam.m_nFlag)
	{
		if (m_cTeam.m_nFigure == TEAM_CAPTAIN)
		{//ӳ
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
	
	Npc[this->m_nIndex].SetInstantSpr(enumINSTANT_STATE_LEVELUP);  //ͷ˲Ч
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
	case ATTRIBUTE_STRENGTH://
		nData = pAttribute->m_nBasePoint - m_nStrength;
		m_nStrength = pAttribute->m_nBasePoint;
		m_nCurStrength = pAttribute->m_nCurPoint;
		UpdataCurData();
		SetNpcPhysicsDamage();
		break;
	case ATTRIBUTE_DEXTERITY://
		nData = pAttribute->m_nBasePoint - m_nDexterity;
		m_nDexterity = pAttribute->m_nBasePoint;
		SetNpcAttackRating();
		SetNpcDefence();
		UpdataCurData();
		SetNpcPhysicsDamage();
		break;
	case ATTRIBUTE_VITALITY://�?
		nData = pAttribute->m_nBasePoint - m_nVitality;
		m_nVitality = pAttribute->m_nBasePoint;
		Npc[m_nIndex].AddBaseLifeMax(PlayerSet.m_cLevelAdd.GetLifePerVitality(Npc[m_nIndex].m_Series) * nData);
		Npc[m_nIndex].AddBaseStaminaMax(PlayerSet.m_cLevelAdd.GetStaminaPerVitality(Npc[m_nIndex].m_Series) * nData);
		UpdataCurData();
		break;
	case ATTRIBUTE_ENGERGY: //ڹ
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
		sprintf(sMsg.szMessage, "ʾ:ʧ(%d)�?, m_nExp - nExp);
		sMsg.eType = SMT_NORMAL;
		sMsg.byConfirmType = SMCT_NONE;
		sMsg.byPriority = 0;
		sMsg.byParamSize = 0;
		sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
	}
	
	this->m_nExp = nExp; //鸳�?
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
		// Ǯһǵroom_equipment
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
		m_ItemList.SetServer(pXu->m_nXu);//ÿͻ˽
}
#endif

#ifndef _SERVER
//---------------------------------------------------------------------
//	ܣյ֪ͨ�?
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
	
	// ֪ͨ
	KUiObjAtRegion	sMoney;
	sMoney.Obj.uGenre = CGOG_MONEY;
	sMoney.Obj.uId = pMoney->m_nMoney;
	CoreDataChanged(GDCNI_TRADE_DESIRE_ITEM, (unsigned int)&sMoney, 0);
}
#endif

#ifndef _SERVER
//---------------------------------------------------------------------
//	ܣյ֪ͨɻȡ
//---------------------------------------------------------------------
void	KPlayer::s2cTradeDecision(BYTE* pMsg)
{
	TRADE_DECISION_SYNC	*pSync = (TRADE_DECISION_SYNC*)pMsg;
	if (pSync->m_btDecision == 1)	// ɽ
	{
		KSystemMessage	sMsg;
		sprintf(sMsg.szMessage, MSG_TRADE_SUCCESS, m_cTrade.m_szDestName);
		sMsg.eType = SMT_NORMAL;
		sMsg.byConfirmType = SMCT_NONE;
		sMsg.byPriority = 0;
		sMsg.byParamSize = 0;
		sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		
		// ItemList.Remove modify by spe 03/06/14
		//m_ItemList.ClearRoom(room_trade);
		m_ItemList.RemoveAllInOneRoom(room_trade1);
		//m_ItemList.ClearRoom(room_trade1);
		m_ItemList.ClearRoom(room_tradeback);
		this->m_cTrade.Release();
		m_cMenuState.SetState(PLAYER_MENU_STATE_NORMAL);
		
		// ֪ͨ潻�?
		CoreDataChanged(GDCNI_TRADE_END, 0, 0);
		
		m_ItemList.MenuSetMouseItem();
	}
	else if (pSync->m_btDecision == 0)	// ȡ
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
		
		// ȡ׵ݻָ
		m_ItemList.RecoverTrade();
		m_ItemList.SetClient(m_cTrade.m_nBackEquipMoney, m_cTrade.m_nBackRepositoryMoney, 0);
		m_ItemList.ClearRoom(room_trade);
		m_ItemList.RemoveAllInOneRoom(room_trade1);
		//		m_ItemList.ClearRoom(room_trade1);
		m_ItemList.ClearRoom(room_tradeback);
		m_cTrade.Release();
		m_cMenuState.SetState(PLAYER_MENU_STATE_NORMAL);
		
		// ֪ͨ潻�?
		CoreDataChanged(GDCNI_TRADE_END, 0, 0);
		// Ʒ
		CoreDataChanged(GDCNI_CONTAINER_OBJECT_CHANGED, UOC_ITEM_TAKE_WITH, 0);
		
		m_ItemList.MenuSetMouseItem();
	}
}
#endif
//ͻ  Ҳ
#ifndef _SERVER
void	KPlayer::SyncCurPlayer(BYTE* pMsg)
{
	//m_ItemList.RemoveAll();  --ԭе
	
	CURPLAYER_SYNC* PlaySync = (CURPLAYER_SYNC *)pMsg;
	this->m_nIndex = NpcSet.SearchID(PlaySync->m_dwID);
	this->m_dwID = g_FileName2Id(Npc[m_nIndex].Name); //ϡ
	
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
	SetFirstDamage();   //û˺
	SetBaseAttackRating();
	SetBaseDefence();
	Npc[m_nIndex].m_ActionScriptID = 0;
	Npc[m_nIndex].m_TrapScriptID = 0;
	m_nExp = PlaySync->m_nExp;
	m_nNextLevelExp = PlayerSet.m_cLevelAdd.GetLevelExp(PlaySync->m_btLevel);
	m_dwLeadExp = PlaySync->m_dwLeadExp;
   // m_dwLoginTime = PlaySync->n_dwLoginTime;  //ǰ½֡
	//m_CUnlocked = PlaySync->m_btCUnlocked;
	m_dwLeadLevel = PlayerSet.m_cLeadExp.GetLevel(m_dwLeadExp);
	m_dwNextLevelLeadExp = PlayerSet.m_cLeadExp.GetLevelExp(m_dwLeadLevel);
	
	m_cFaction.m_nCurFaction = (char)PlaySync->m_btCurFaction;
	m_cFaction.m_nFirstAddFaction = (char)PlaySync->m_btFirstFaction;
	m_cFaction.m_nAddTimes = PlaySync->m_nFactionAddTimes;
	
	m_nWorldStat = (int)PlaySync->m_wWorldStat;//
	m_nSectStat = (int)PlaySync->m_wSectStat; //е

	m_ItemList.Init(CLIENT_PLAYER_INDEX);
	m_ItemList.SetClient(PlaySync->m_nMoney1, PlaySync->m_nMoney2, 0);
	m_ItemList.SetServer(PlaySync->m_nXu);
	Npc[m_nIndex].m_LifeMax = PlaySync->m_wLifeMax;
	Npc[m_nIndex].m_ManaMax = PlaySync->m_wManaMax;
	Npc[m_nIndex].m_NuqiMax = PlaySync->m_wNuqiMax;  //ŭ
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
//û˺
void	KPlayer::SetFirstDamage()
{
	int nDamageBase;
	
	nDamageBase = m_nCurStrength / STRENGTH_SET_DAMAGE_VALUE + 1;   // 6  ˺
	
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
//ԭʼ
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
//ͻ˰�?  ԻNPC ִнű
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
/*	enum	// 
{
Obj_Kind_MapObj = 0,		// ͼҪڵͼ
Obj_Kind_Body,				// npc ʬ
Obj_Kind_Box,				// 
Obj_Kind_Item,				// ڵϵװ
Obj_Kind_Money,				// ڵϵǮ
Obj_Kind_LoopSound,			// ѭЧ
Obj_Kind_RandSound,			// Ч
Obj_Kind_Light,				// Դ3DģʽзĶ
Obj_Kind_Door,				// 
Obj_Kind_Trap,				// 
Obj_Kind_Prop,				// Сߣ 
Obj_Kind_Num,				// 
};*/
	
	switch(Object[nIdx].m_nKind)
	{
//////ԭûе/////////////
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
	case Obj_Kind_Trap:  // 
	case Obj_Kind_Prop:	 //
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
void KPlayer::DrawSelectInfo()   //ѡ ʾ
{
	if (m_nIndex <= 0)
		return;

	if (m_nPeapleIdx)
	{//npc
		if (Npc[m_nPeapleIdx].m_Kind == kind_player) // 
		{
			if (!NpcSet.CheckShowName())
			{
				Npc[m_nPeapleIdx].PaintInfo(Npc[m_nPeapleIdx].GetNpcPate(), true);
			}
		}
		else if (Npc[m_nPeapleIdx].m_Kind == kind_dialoger)  //Ի
		{
			if (!NpcSet.CheckShowName())
			{
			    Npc[m_nPeapleIdx].PaintInfo(Npc[m_nPeapleIdx].GetNpcPate(), true);
			}
		}
		else if (Npc[m_nPeapleIdx].m_Kind == kind_partner)  //ͬ
		{
			if (!NpcSet.CheckShowName())
			{
				Npc[m_nPeapleIdx].PaintInfo(Npc[m_nPeapleIdx].GetNpcPate(), true);
			}
		}
		else   //ðѪ
		{
			Npc[m_nPeapleIdx].DrawBlood();   //Ѫ  ðѪ
		}

		return;
	}
	if (m_nObjectIdx)
	{//Ʒ
		if (!ObjSet.CheckShowName())
			Object[m_nObjectIdx].DrawInfo();
		//	Object[m_nObjectIdx].DrawBorder();
		return;
	}
}
#endif
//ȡС˺
//#ifndef _SERVER
void KPlayer::GetEchoDamage(int* nMin, int* nMax, int* nAr, int nType)
{
	int nSkillId;
	*nMin = 0;
	*nMax = 0;
	
	if (m_nIndex <= 0 || m_nIndex >= MAX_NPC || nType < 0 || nType > 1 || NULL == nMin || NULL == nMax)
		return;
	//Ҽѡ

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

	BOOL	bIsPhysical = pOrdinSkill->IsPhysical();          //
	BOOL	bIsMagic = pOrdinSkill->IsMagic();                //1ڹϵ 0⹥�?
	BOOL	bIsSkillMagic = pOrdinSkill->IsSkillMagic();
	BOOL	bIsSkillPhysical = pOrdinSkill->IsSkillPhysical();

	// lay % ho tro skill
	int nSkillEnhance = 0;
	if(nSkillId > 0 && nSkillId < MAX_SKILL)
	{
		nSkillEnhance = Npc[m_nIndex].m_SkillList.GetEnChance(nListidx);;
	}

	pMagicData = pOrdinSkill->GetDamageAttribs();     //

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

//װǵú˺
void KPlayer::SetNpcDamageAttrib()
{
	SetNpcPhysicsDamage();
}
//ÿ Ŀͼ
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
// 
#ifdef _SERVER

void KPlayer::Market(int nId,int shopID) 
{ 
	BuySell.OpenSMarket(nId, shopID);  //ID
}
#endif


#ifdef _SERVER
void KPlayer::String(int nbutton)
	{//stringbox
	    //char scriptName[128]={0}; 
		//sprintf(scriptName,"\\script\\system\\input.lua");
		//ExecuteScriptA(scriptName,"main",nbutton,""); 
		ExeNoBackScript("\\script\\system\\input.lua","main",nbutton,0,0,0);
	}
#endif

//һ
#ifdef _SERVER
void KPlayer::SAutoplay(int nbutton,int nBtyeVal,int nBtyeVala)
{
   if (nbutton==0 || nbutton==1)
   {
      Npc[m_nIndex].m_AutoplayId=(BYTE)nbutton;  //һ

	  m_Autoplay.nIsNoBlue          = EGetByte(nBtyeVal,1);  //Ƿ
	  m_Autoplay.nIsOpenPicjkup     = EGetByte(nBtyeVal,2);  //Ƿʰȡ
	  m_Autoplay.nAutoMoney         = EGetByte(nBtyeVal,3);
	  m_Autoplay.nAutoEques         = EGetByte(nBtyeVal,4);

	  m_Autoplay.nAutoMoneyAndEques = EGetByte(nBtyeVala,1);
	  m_Autoplay.nAutoTuPu          = EGetByte(nBtyeVala,2);
	  m_Autoplay.nAutoTool          = EGetByte(nBtyeVala,3);
	  /*
	  AutoPlayCmd.ByteVal=ESetByte(AutoPlayCmd.ByteVal,3,Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoMoney);//Ǯ
	  AutoPlayCmd.ByteVal=ESetByte(AutoPlayCmd.ByteVal,4,Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoEques);//װ

	  AutoPlayCmd.ByteVala=ESetByte(AutoPlayCmd.ByteVala,1,Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoMoneyAndEques);//Ǯװ
	  AutoPlayCmd.ByteVala=ESetByte(AutoPlayCmd.ByteVala,2,Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoTuPu);//ͼ
	  AutoPlayCmd.ByteVala=ESetByte(AutoPlayCmd.ByteVala,3,Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoTool);//
	  
	  */
	  //m_cTask.GetTaskByte(nBtyeVal,2);
   }				   

    char scriptName[128]={0}; 
	sprintf(scriptName,"%s","\\script\\system\\autoplayer.lua");
	ExecuteScriptA(scriptName,"main",nbutton,"",FALSE); //99ԺΪ�?
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
	else if (nId == 10)  //Ϊ
	{
		sprintf(scriptName,"\\script\\system\\guzi_xiazhu.lua");
		ExecuteScriptA(scriptName,"main",nXuHao,""); 	
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
		ExecuteScriptA(scriptName,"main",1,""); 
	}
	else if (nId == 2)
	{
		char scriptName[255]; 
		sprintf(scriptName,"\\script\\special\\trangthai.lua");
		ExecuteScriptA(scriptName,"main",2,""); 	
	}
	
	else if (nId == 3)
	{
		char scriptName[255]; 
		sprintf(scriptName,"\\script\\special\\trangthai.lua");
		ExecuteScriptA(scriptName,"main",3,""); 	
	}	
	
}
#endif


#ifdef _SERVER
void	KPlayer::SetChestLock(BOOL unlockFlag)   //ǷסĻ
{
	if (unlockFlag)
	{
		m_CUnlocked = 1;
		/*char szMsg[100];
		sprintf(szMsg, "<c=cyan>Ѿ,Բ!<c>");
		int nLength=0; 
		nLength = TEncodeText(szMsg, strlen(szMsg));
		KPlayerChat::SendSystemInfo(1, m_nPlayerIndex, ":", (char *) szMsg, nLength);*/
	}
	else
	{
		m_CUnlocked = 0;
		/*char szMsg[100];
		sprintf(szMsg, "<c=cyan>ĻѾ,ֹֽ!<c>");
		int nLength=0; 
		nLength = TEncodeText(szMsg, strlen(szMsg));
		KPlayerChat::SendSystemInfo(1, m_nPlayerIndex, ":", (char *) szMsg, nLength );*/
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
BOOL	KPlayer::CheckChestPW(int iPassword) //
{
	if (iPassword == m_nChestPW)
	{
		// Mat khau ruong chinh xac
		//char szMsg[100];
		//sprintf(szMsg, "<c=cyan>ȷ,<c>");
		//int nLength=0; 
		//nLength = TEncodeText(szMsg, strlen(szMsg));
		//KPlayerChat::SendSystemInfo(1, m_nPlayerIndex, ":", (char *) szMsg, nLength);
		return TRUE;
	}
	else
	{
		// Mat khau ruong SAI
		//char szMsg[100];
		//sprintf(szMsg, "<c=cyan>벻�?!<c>");
		//int nLength=0; 
		//nLength = TEncodeText(szMsg, strlen(szMsg));
		//KPlayerChat::SendSystemInfo(1, m_nPlayerIndex, ":", (char *) szMsg, nLength);
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
	//sprintf(szMsg, "<c=cyan>!<c>");
	//int nLength=0; 
		//nLength = TEncodeText(szMsg, strlen(szMsg));
	//KPlayerChat::SendSystemInfo(1, m_nPlayerIndex, ":", (char *) szMsg, nLength);
	
	PLAYER_CHESTPW_SYNC	pChestSync;
	pChestSync.ProtocolType = s2c_chestpwsync;
	pChestSync.m_Unlocked = m_CUnlocked;
	pChestSync.m_Created = m_nChestPW > 0 ? 1 : 0;
	if (g_pServer)
		g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&pChestSync, sizeof(PLAYER_CHESTPW_SYNC));
}
#endif

#ifdef _SERVER
void	KPlayer::SetNewPW(int resetPW) //µ
{
	if (resetPW <= 0)
		resetPW = 0;

	m_nChestPW = resetPW;
	m_CUnlocked = 0;
	
	// Thong bao cho Client
	//char szMsg[100];
	//sprintf(szMsg, "<c=cyan>!<c>");
	//int nLength=0; 
		//nLength = TEncodeText(szMsg, strlen(szMsg));
	//KPlayerChat::SendSystemInfo(1, m_nPlayerIndex, ":", (char *) szMsg,nLength);

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
{//ᴴ�?
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
	{////ĥ�
		Item[nIdx].SetDurability(nMaxDur);
        Item[nIdx].SetIsCanUse(1);
		ITEM_DURABILITY_CHANGE	IDC;
		IDC.ProtocolType = s2c_itemdurabilitychange;
		IDC.dwItemID = dwItemID;
		IDC.nChange  = nMaxDur - nDur;

		if (m_ItemList.CheckIsOutTime(nIdx)) //Ǳʶװ ǹڵ
           IDC.nCanUse  = 0;
		else
           IDC.nCanUse  = 1;

		if (g_pServer)
			g_pServer->PackDataToClient(m_nNetConnectIdx, &IDC, sizeof(ITEM_DURABILITY_CHANGE));
	}
}
#endif
//Ʒ
//xep chong BreakItem
#ifdef _SERVER
void KPlayer::BreakItem(DWORD dwItemID, int nNum) 
{
	int nIdx   = m_ItemList.SearchID(dwItemID);
	//Ƿ㹻Ŀ�?
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
	return m_nExtPoint;  //չ Աֵ
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
	if (m_nLeftTime>=0) //ʣʱ
	    m_nLeftTime = nLeftTime;

	m_nVipType  = nVipType;	 //Ա
	m_nChangeExtTime = nChangeExtTime;	//�?
}

//Ҫıʱ
long KPlayer::GetExtTimeChanged()
{
	return m_nChangeExtTime;
}

//ָԭƺ=========
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
		{//мID,Ҫȥ״̬
			Npc[m_nIndex].m_ExtSkill[i]=0;
			Npc[m_nIndex].SetSkillLeftTime(nSkillD,0); //ͬͻɾ״̬			
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
				  if (pSkill->GetSkillStyle()!=SKILL_SS_Missles)	////0Ϊӵ 2 Ϊ 3 Ϊ
				  { //ʹ÷ӵ �?  ״̬
					  //if (pSkill->GetSkillStyle()==SKILL_SS_PassivityNpcState)
					  //   nTime=-1;
					  // printf("ִм%s{%d)ȼ(%d)--\n",pSkill->GetSkillName(),sKillID,sLevel);
					   pSkill->CastToolStateSkill(nNpcIdx,0,0,nTime);  //Ƿ
				  } 
				  else
				  {//ӵ	0;	//
					 if(pSkill->Cast(nNpcIdx, -1,nDesNpcIdx,nTime,SKILL_SLT_Npc,nBeiVal))//,20,SKILL_SLT_Npc
                         pSkill->CastSkill(nNpcIdx,sKillID,-1,Npc[nNpcIdx].m_dwID,sLevel,nTime,nBeiVal);  
				  } 
			  }	  
		  }
		  else
			Npc[nNpcIdx].SetToolStateSkillEffect(nNpcIdx,sKillID,sLevel,pData,0,nTime);  //üܹ⻷�?
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
		{//мID,Ҫȥ״̬
			Npc[m_nIndex].m_ExtSkill[i]=0;
			Npc[m_nIndex].SetSkillLeftTime(nSkillD,0); //ͬͻɾ״̬			
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
	// ͻ˷Ϣ
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
		///printf("--С %u --\n",sizeof(PLAYER_MISSION_RANKDATA)); //1209	 //1024
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


//ͬҵĳ ͻ
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
	m_nChangeExtTime  += nTime;    //ҪıǶ
	return TRUE;
}

BOOL KPlayer::AddAndPayExtPoint(long nPoint)
{

	m_nExtPoint       += nPoint;

   if  (m_nExtPoint < 0)
   {
       m_nExtPoint = 0;
   }

	m_nChangeExtPoint += nPoint;    //ҪıǶ
	return TRUE;
}

//ȡҪıչ
long KPlayer::GetExtPointChanged()
{
	return m_nChangeExtPoint;
}

int KPlayer::ComItem( int nIdx1,int nIdx2,int nIdx3 ,int nKind)
{//
	int nReg=0;
	char nMapName[64]={0};
	snprintf(nMapName, sizeof(nMapName), "%s", SubWorld[Npc[m_nIndex].m_SubWorldIndex].nWorldMapInfo[STR_MAP_NAME].c_str());
	nReg=ExeItemScriptA("\\script\\system\\orecomp.lua","maincompore",Npc[m_nIndex].Name,nMapName,nKind,nIdx1,nIdx2,nIdx3,0,0,0,0,0);
	//Item[nIdx1].GetBaseMagic(); //Ե
	return nReg;
}

//װ  ͼ
int KPlayer::ComDiyItem( int nId1,int nId2,int nId3,int nKind,int nMin )  //װװ nMinΪƷ
{
	int nIndex;
//ʼȡʯͷ
	if (nKind == 1)  
	{
		int nReg=0;
		char nMapName[64]={0};
		snprintf(nMapName, sizeof(nMapName), "%s", SubWorld[Npc[m_nIndex].m_SubWorldIndex].nWorldMapInfo[STR_MAP_NAME].c_str());

		nReg=ExeItemScriptA("\\script\\system\\itemcomp.lua","maincompitem",Npc[m_nIndex].Name,nMapName,nKind,nId1,nId2,nId3,nMin,0,0,0,0);
		
		return nReg;

	}
// װ
	else if (nKind == 2)  // װ
	{
		int nReg=0;
		char nMapName[64]={0};
		snprintf(nMapName, sizeof(nMapName), "%s", SubWorld[Npc[m_nIndex].m_SubWorldIndex].nWorldMapInfo[STR_MAP_NAME].c_str());

		nReg=ExeItemScriptA("\\script\\system\\itemcomp.lua","maincompitem",Npc[m_nIndex].Name,nMapName,nKind,nId1,nId2,nId3,nMin,0,0,0,0);
		
		return nReg;

	}
//  Ƹֻƽ
	else if (nKind == 4)  
	{
		int nPlace = Item[nId3].GetParticular() - 200; //ϸ

		nMin = nMin / 2;
		nMin += Item[nId2].GetLevel() + Item[nId3].GetLevel() - 9;  //Կʯ  
		
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
		
		int nItemClass		= Item[nId1].GetGenre(); //
		int nDetailType		= Item[nId1].GetDetailType();
		int nParticularType	= Item[nId1].GetParticular();
		int nLevel			= Item[nId1].GetLevel();
		int nSeries			= Item[nId1].GetSeries();
		int nLuck			= Item[nId1].GetItemParam()->nLuck;

		int nItemLevel[2];
		
		KTabFile tabMagic;
		tabMagic.Load("\\Settings\\item\\magicattriblevel_rong.txt"); //б
		
		int nRow;
		char szTmp[8];
		snprintf(szTmp, sizeof(szTmp),"%d",Item[nId3].GetBaseMagic()); //Կʯ
		nRow = tabMagic.FindRow(szTmp); //ҵħԵ
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

		tabMagic.Load(TABFILE_MAGICATTRIB_PATH); //Ҫ޸
		int nMagic;
		tabMagic.GetInteger(nMinRow,Item[nId1].GetDetailType() + 15,0,&nMagic);//+13õӦ װ  ͷ ѥ 
		if (nMagic <= 0)//Ƿ
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
		
		// װ
		nIndex = ItemSet.AddOther(nItemClass,nSeries,nLevel,nLuck,nDetailType,nParticularType,nItemLevel,0,0,1,0,0,nRoint);
	}
// װ 
	else if (nKind == 3)   
	{   
        int nReg=0;
		char nMapName[64]={0};
		snprintf(nMapName, sizeof(nMapName), "%s", SubWorld[Npc[m_nIndex].m_SubWorldIndex].nWorldMapInfo[STR_MAP_NAME].c_str());
            nReg=ExeItemScriptA("\\script\\system\\itemcomp.lua","maincompitem",Npc[m_nIndex].Name,nMapName,nKind,nId1,nId2,nId3,nMin,0,0,0,0);

        return nReg;

    }     
// װ װ װ
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
			m_ItemList.Remove(nId2);  //Ƴװ
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
		//int nRongmun        = Item[nId1].GetRongNum();//ȡֵ
		int nGoldID        = Item[nId1].GetGoldId();
		int nEnChance       = Item[nId1].GetEnChance();   //         m_CommonAttrib.nEnChance

		UINT nRandomID      = Item[nId1].GetItemParam()->uRandomSeed; // 
		int nVersionID      = Item[nId1].GetItemParam()->nVersion;
		int nLuck			= Item[nId1].GetItemParam()->nLuck;

        int nItemLevel[6]; 
		int nPlace;
        //ȷʯͷ
		if  (nGoldID>0)  
        nPlace = Item[nId3].GetParticular() - 200;  //Կʯ װTXTеλ-ʼλ  װ
        else if(npiont>0)
        nPlace = Item[nId3].GetParticular() - 200;  //Կʯ װTXTеλ-ʼλ  װ
        else
        nPlace = Item[nId3].GetParticular() - 200;  //Կʯ װTXTеλ-ʼλ  װ
        //ͬħ
		for (int j = 0; j < 6; ++j)
		nItemLevel[j] = Item[nId1].m_GeneratorParam.nGeneratorLevel[j];
		// װλ
		KTabFile tabMagic;
		tabMagic.Load(TABFILE_MAGICALEVEL_PATH);

		int nRow;
		char szTmp[8];
		snprintf(szTmp, sizeof(szTmp),"%d",Item[nId3].GetBaseMagic()); //ȡħʯĻ
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
         //ֵü
		int nAdd = ::GetRandomNumber(nMin,nMax) + ((Item[nId3].GetLevel() + Item[nId2].GetLevel())/2);
		if (nLuck < 0) nLuck = 0;
		
		nLuck += nMin ;
		if (nLuck > 10) nLuck = 10;
		if (nAdd > 10) nAdd = 10;
		int nMinRow; 
		tabMagic.GetInteger(nRow,"MAGATTRLVL_START",0,&nMinRow);//С+ֵ
		nMinRow +=  nAdd;
		tabMagic.Clear();
		tabMagic.Load(TABFILE_MAGICATTRIB_PATH);
		int nMagic;
		tabMagic.GetInteger(nMinRow,Item[nId1].GetDetailType() + 15,0,&nMagic);

		if (nMagic <= 0) //ɳָԵװ
		   return 0;
		tabMagic.Clear();
		int nrItemLevel[6];

		/*/Ѿ˵ͬԵĲ
		for (int p = 0;p < 6;p++)
		{
          if (Item[nId1].GetAttribType(p) == Item[nId3].GetBaseMagic())  
               return 0; 
		  else if(Item[nId1].GetRongAttribType(p) == Item[nId3].GetBaseMagic())
               return 0; 
		}
        *///ֵͬ  ж ʯͷλ
		for (int k = 0;k < 6;++k)
		{
			if (nPlace > k)  //  λúʯͷϵĲͬ
			{
				if (Item[nId1].GetRongAttribType(k) <= 0 || Item[nId1].GetRongAttribType(k) == Item[nId3].GetBaseMagic())
				{
					return 0;  //ͬ  ײ  Ҫ˳ ʧ
				}
			}
			else if (nPlace == k && Item[nId1].GetRongAttribType(k) <= 0)  //λͬ ûԵĿλ
			{
				nrItemLevel[k] = nMinRow;   //趨С�?
				continue;
			}
			else if (nPlace == k && Item[nId1].GetRongAttribType(k) > 0)  //λͬ 
			{
				return 0;
			}
			nrItemLevel[k] = Item[nId1].m_GeneratorParam.nRGeneratorLevel[k];
		}
		
		int nStackNum = 1;
  
		nIndex = ItemSet.AddRong(nGoldID,nSeries,nLevel,nLuck,nDetailType,nParticularType,nItemLevel,nrItemLevel,NULL,nVersionID,nRandomID,1,nEnChance,nRongmun,0,0,NULL,0,0,0,0,0,0,TRUE);	
	}
//  Ƹ 
    else if (nKind == 5)  
	{
		int nMax = Item[nId2].GetLevel() + 2;  //ݵȼ+2 ȷ׵
		int nMin = Item[nId2].GetLevel();
		
		if (nMax > 6) nMax = 6;
		if (nMin > 6) nMax = 6;
		
		if (nMax < 1) nMax = 1;
		if (nMin < 1) nMax = 1;
		
		//int nRoint = ::GetRandomNumber(nMin,nMax); //ȡ--
		
		int nItemClass		= Item[nId1].GetGenre();
		int nDetailType		= Item[nId1].GetDetailType();
		int nParticularType	= Item[nId1].GetParticular();
		int nLevel			= Item[nId1].GetLevel();
		int nSeries			= Item[nId1].GetSeries();
		int nRongmun        = Item[nId1].GetRongNum();    //ȡֵ
		int nGoldmID        = Item[nId1].GetGoldId();
		int nEnChance       = Item[nId1].GetEnChance();   //m_CommonAttrib.nEnChance

		UINT nRandomID       = Item[nId1].GetItemParam()->uRandomSeed; // 
		int nVersionID      = Item[nId1].GetItemParam()->nVersion;
		int nLuck		    = Item[nId1].GetItemParam()->nLuck;

        int nItemLevel[6];   
		for (int j = 0; j < 6; ++j)//ͬħ
		nItemLevel[j] = Item[nId1].m_GeneratorParam.nGeneratorLevel[j];

			if (nRongmun<=0)  //Ϊ
			{
               //defFuncShowNormalMsgA("ʾ:nGoldmun ݳ");
				return 0;
			}
        int nrItemLevel[6];  //
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
		return 0;  //ϳʧ
	}

	/*if (m_ItemList.GetPlayerItemCount()>=MAX_ITEMLIST_CONUT)
	{
		ItemSet.Remove(nIndex);
		m_ItemList.msgshow("ʾ:<c=yellow>Ʒ,ռ<c>!");
		return 0;
	}*/

	int	x, y;
	if (m_ItemList.CheckCanPlaceInEquipment(Item[nIndex].GetWidth(), Item[nIndex].GetHeight(), &x, &y)) //ж  Ŀռ
	{
		m_ItemList.Add(nIndex, pos_equiproom, x, y); //װ
	}
	else
	{
		m_ItemList.msgshow("Hanh trang day!");
		int	nIdx = m_ItemList.Hand();
		
		if (nIdx)
		{//ж
			m_ItemList.Remove(nIdx);  //ɾͻ˵Ľڵ
			ItemSet.Remove(nIdx);
		} 
		//ͬͻˣµĽڵ�?
		m_ItemList.Remove(nIndex);  //ɾͻ˵Ľڵ
		ItemSet.Remove(nIndex);
		return 0;
	}
	return 1;  //ɹ
}


//ȡ̯Ʒ
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
		Npc[m_nIndex].m_BaiTan = 0; //ý̯״̬
	}

	//printf("SendSellItemCount: Name: %s nCount: %d \n",Npc[m_nIndex].Name,nCount);
	g_pServer->PackDataToClient(Player[nIndex].m_nNetConnectIdx, (BYTE*)&pGetCount, sizeof(pGetCount));
}
//ȡ̯Ʒ
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
		//Npc[m_nIndex].SetstrInfo(STR_SHOP_NAME," bn ht !");
		//Npc[Player[m_nPlayerIndex].m_nIndex].SetstrInfo(STR_SHOP_NAME, " bn ht !");
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
//	ܣԼװϵװϢ˿
//-------------------------------------------------------------------------
void	KPlayer::SendEquipItemInfo(int nTargetPlayer, int nPrcess, BOOL bUpdate) //nTargetPlayer Ҫ鿴�?
{
	if (nTargetPlayer <= 0 || nTargetPlayer >= MAX_PLAYER || Player[nTargetPlayer].m_nIndex <= 0)
		return;
	
	int	nIdx, i ,j= 0;

	SViewItemInfo	*pInfo;
	VIEW_EQUIP_SYNC	sView;

	memset(sView.m_sInfo, 0, sizeof(sView.m_sInfo));

	sView.ProtocolType = s2c_viewequip;

	sView.m_dwNpcID = Npc[m_nIndex].m_dwID; // m_nIndex Ϊ鿴�?
	
	for (i = 0; i < itempart_num ; i++)     // װλitempart_num
	{
	    nIdx  = m_ItemList.m_EquipItem[i];

	    pInfo = &sView.m_sInfo[i];

		if (nIdx <= 0)
		{//λΪ ͼһλ
			memset(pInfo, 0, sizeof(*pInfo));
			continue;
		}
		pInfo->m_Point        = Item[nIdx].GetPoint();
		pInfo->m_Rongpiont    = Item[nIdx].IsRong(); //
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
		//pInfo->m_btPlace      = m_Items[i].nPlace; //ͬĿ
		//pInfo->m_btX          = m_Items[i].nX;     //ͬλ
		//pInfo->m_btY          = m_Items[i].nY;
		pInfo->m_Time.bYear   = Item[nIdx].GetTime()->bYear;
		pInfo->m_Time.bMonth  = Item[nIdx].GetTime()->bMonth;
		pInfo->m_Time.bDay    = Item[nIdx].GetTime()->bDay;
		pInfo->m_Time.bHour   = Item[nIdx].GetTime()->bHour;
		pInfo->m_Time.bMin    = Item[nIdx].GetTime()->bMin;
		//pInfo->m_IsBang       = Item[nIdx].GetIsBang();      //Ƿ
		//pInfo->m_IsKuaiJie    = Item[nIdx].GetIsKuaiJie();   //Ƿ
		//pInfo->m_IsMagic      = Item[nIdx].GetIsMagic();     //Ƿħ
		//pInfo->m_SkillType    = Item[nIdx].GetSkillType();
		//pInfo->m_MagicID      = Item[nIdx].GetMagicID();
		//pInfo->m_IsWhere      = Item[nIdx].GetIsWhere();
		pInfo->m_IsCanUse   = Item[nIdx].GetIsCanUse();
		pInfo->m_RandomSeed = Item[nIdx].m_GeneratorParam.uRandomSeed;
		pInfo->m_Version    = Item[nIdx].m_GeneratorParam.nVersion;
		//pInfo->m_Durability = Item[nIdx].GetDurability();
		//sprintf(pInfo->m_WonName,Item[nIdx].GetWonName());  //	
		
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
	//Ҫ鿴�?
	g_pServer->PackDataToClient(Player[nTargetPlayer].m_nNetConnectIdx,(BYTE*)&sView, sizeof(sView));	//sView
}

int KPlayer::SendLianItemInfo(const void *nData)
{
	if  (!nData) return false;
	tagItemAction* pInfo = (tagItemAction *)nData;
	//nGmRoleName Ҫȡݵ  nRoleNameΪӵ
	//printf("---SendLianItemInfo ITEM:%s %s %u ----\n",pInfo->nGmRoleName,pInfo->nRoleName,pInfo->nId);

	int	nIdx,nItemidx,i;
	VIEW_LIAN_ITEM_SYNC	 sLView;  //Э
    memset(&sLView, 0, sizeof(sLView));//ս�?
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
	sLView.m_Rongpiont    = Item[nIdx].IsRong(); //
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
	sLView.m_IsBang       = Item[nIdx].GetIsBang();   //Ƿ
	sLView.m_IsKuaiJie    = Item[nIdx].GetIsKuaiJie();   //Ƿ
	sLView.m_IsMagic      = Item[nIdx].GetIsMagic();   //Ƿħ
	sLView.m_SkillType    = Item[nIdx].GetSkillType();
	sLView.m_MagicID      = Item[nIdx].GetMagicID();
	sLView.m_IsWhere      = Item[nIdx].GetIsWhere();
	sLView.m_IsCanUse     = Item[nIdx].GetIsCanUse();
	snprintf(sLView.m_WonName, sizeof(sLView.m_WonName), "%s", pInfo->nGmRoleName);  //
		
	int j;
	for (j = 0; j < 6; j++)//ͬħ
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
			//ִűĺ
			try
			{//˵
				pScript->CallFunction("GetItemDescription",1,"sdddddds", Item[nIdx].GetName(),Item[nIdx].GetItemBaseMagic(0),Item[nIdx].GetItemBaseMagic(1),Item[nIdx].GetItemBaseMagic(2),Item[nIdx].GetItemBaseMagic(3),Item[nIdx].GetItemBaseMagic(4),Item[nIdx].GetItemBaseMagic(5),"");
				const char * szType = lua_typename(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState));
				if (Lua_IsNumber(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState)) == 1)
				{ //Lua_PushNumber
					int nResult = (int)Lua_ValueToNumber(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState));
					_snprintf(sLView.m_ItmeInfo,sizeof(sLView.m_ItmeInfo)-1, "%d", nResult);  // int 
				   	//sprintf(sLView.m_ItmeInfo,"%d",nResult);
				} 
				else if (Lua_IsString(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState)) == 1)  // ַ
				{  
					snprintf(sLView.m_ItmeInfo, sizeof(sLView.m_ItmeInfo), "%s", (char *)Lua_ValueToString(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState)));
				   	//sprintf(sLView.m_ItmeInfo,(char *)Lua_ValueToString(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState)));
				}   
				else
					//sprintf(sLView.m_ItmeInfo,"<c=Red>ݴ,ϵGM!<c>");
					_snprintf(sLView.m_ItmeInfo,sizeof(sLView.m_ItmeInfo)-1,"<c=Red>ݴ,ϵGM!<c>");
			}  
			catch (...)
			{
				_snprintf(sLView.m_ItmeInfo,sizeof(sLView.m_ItmeInfo)-1,"<c=Red>ݴ,ϵGM!<c>");
				//sprintf(sLView.m_ItmeInfo,"<c=Red>ݴ,ϵGM!<c>");
			}

			nSafeIndex=pScript->SafeCallBegin();
			pScript->SafeCallEnd(nSafeIndex);
		}
		else
		{
			_snprintf(sLView.m_ItmeInfo,sizeof(sLView.m_ItmeInfo)-1,"");
			//sprintf(sLView.m_ItmeInfo,"");
		}	
	}

	if (g_pGateWayClient)
		g_pGateWayClient->SendPackToServer((BYTE*)&sLView, sizeof(VIEW_LIAN_ITEM_SYNC));
    

	return true;
}

//  
int KPlayer::SendLianItemInfo(int nTargetPlayer, int nItemDwID,int nPrcess, BOOL bUpdate,char *nShopName)  //nTargetPlayer  鿴�?
{
	if (nTargetPlayer <= 0 || nTargetPlayer >= MAX_PLAYER || Player[nTargetPlayer].m_nIndex <= 0)
		return FALSE;
	
	int	nIdx,nItemidx,i;
	VIEW_LIAN_ITEM_SYNC	 sLView;  //Э
    memset(&sLView, 0, sizeof(sLView));//ս�?
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
	sLView.m_Rongpiont    = Item[nIdx].IsRong(); //
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
	sLView.m_IsBang       = Item[nIdx].GetIsBang();   //Ƿ
	sLView.m_IsKuaiJie    = Item[nIdx].GetIsKuaiJie();   //Ƿ
	sLView.m_IsMagic      = Item[nIdx].GetIsMagic();   //Ƿħ
	sLView.m_SkillType    = Item[nIdx].GetSkillType();
	sLView.m_MagicID      = Item[nIdx].GetMagicID();
	sLView.m_IsWhere      = Item[nIdx].GetIsWhere();
	sLView.m_IsCanUse     = Item[nIdx].GetIsCanUse();
	snprintf(sLView.m_WonName, sizeof(sLView.m_WonName), "%s", Item[nIdx].GetWonName());  //
		
	int j;
	for (j = 0; j < 6; j++)//ͬħ
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
			//ִűĺ
			try
			{//˵
				pScript->CallFunction("GetItemDescription",1,"sdddddds", Item[nIdx].GetName(),Item[nIdx].GetItemBaseMagic(0),Item[nIdx].GetItemBaseMagic(1),Item[nIdx].GetItemBaseMagic(2),Item[nIdx].GetItemBaseMagic(3),Item[nIdx].GetItemBaseMagic(4),Item[nIdx].GetItemBaseMagic(5),"");
				const char * szType = lua_typename(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState));
				if (Lua_IsNumber(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState)) == 1)
				{ //Lua_PushNumber
					int nResult = (int)Lua_ValueToNumber(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState));
					_snprintf(sLView.m_ItmeInfo,sizeof(sLView.m_ItmeInfo)-1, "%d", nResult);  // int 
				   	//sprintf(sLView.m_ItmeInfo,"%d",nResult);
				} 
				else if (Lua_IsString(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState)) == 1)  // ַ
				{  
					snprintf(sLView.m_ItmeInfo, sizeof(sLView.m_ItmeInfo), "%s", (char *)Lua_ValueToString(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState)));
				   	//sprintf(sLView.m_ItmeInfo,(char *)Lua_ValueToString(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState)));
				}   
				else
					//sprintf(sLView.m_ItmeInfo,"<c=Red>ݴ,ϵGM!<c>");
					_snprintf(sLView.m_ItmeInfo,sizeof(sLView.m_ItmeInfo)-1,"<c=Red>ݴ,ϵGM!<c>");
			}  
			catch (...)
			{
				_snprintf(sLView.m_ItmeInfo,sizeof(sLView.m_ItmeInfo)-1,"<c=Red>ݴ,ϵGM!<c>");
				//sprintf(sLView.m_ItmeInfo,"<c=Red>ݴ,ϵGM!<c>");
			}

			nSafeIndex=pScript->SafeCallBegin();
			pScript->SafeCallEnd(nSafeIndex);
		}
		else
		{
			_snprintf(sLView.m_ItmeInfo,sizeof(sLView.m_ItmeInfo)-1,"");
			//sprintf(sLView.m_ItmeInfo,"");
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

// ƷӦĲ
//     Ʒ1ӵϼƷ 2űֱӸ 3 4player ֮
//               (ͨʵ)
//         ͻϣȻͻͨóƷidӦóװ
//         Ʒλû߸ļ�Ѽ?
//         ƷλùϵȻͻ˵ļǷȷȻӦ
//         Ѵͻ
//     Ʒλõͻ˴ƷԴλáƷĿλãȻ󷢸
//     ȷϺ֪ͨͻȷĲɹЭԭȥ
//     ֵ�֪ͨͻ?
//     ʹƷ1ҩ 2װҼ
//         ͻʲôλõҩͬʱͻ˵ӦֵӦ�?
//         յͻ��ݣͨplayerͬķʽ֪ͨͻ
//         ı�?
//         װҼһװϣԶװϡжµλϢ
//         ԭ

//         ׵Ĵû

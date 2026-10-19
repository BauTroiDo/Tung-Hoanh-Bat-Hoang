/* -------------------------------------------------------------------------
//	文件名		：	kautoai.cpp
//	创建者		：	luobaohang
//	创建时间	：	2007-11-3 12:25:22
//	功能描述	：	自动挂机AI
//
// -----------------------------------------------------------------------*/
#ifndef _SERVER

#include "kcore.h"
#include <iostream>
#include <fstream>
//#include "lua/lua.h"
#include "knpc.h"
#include "kplayer.h"
#include "scene/obstacledef.h"
#include "knpcfindpath.h"
#include "kobj.h"
#include "knpc.h"
#include "kautoai.h"


using namespace std;

AutoAi g_cAutoAi;

class KSelfNpcFindPath : public KNpcFindPath
{
public:
	KSelfNpcFindPath() { m_NpcIdx = Player[CLIENT_PLAYER_INDEX].m_nIndex; }
};

//目标循环（用于AI分析）//lbhtest
#define AI_BLACK_COUNT			2				//黑名单的最大个数
#define AI_TIMES_ADDBLACK			15			//多少次攻击无效后加入黑名单
#define AI_TIMES_BLACK_CLOSE		5			//开始攻击后多少次不掉血就加黑名单，需小于AI_TIMES_ADDBLACK
#define AI_TIME_BLACK_LAST			(1000*3)	//黑名单有效时间
typedef struct tagTargetNode 
{
	tagTargetNode()
	{
		m_IDToHit = 0;
	}
	INT	m_IDToHit;
	INT	m_nAddTime; //加入黑名单的时间
}TargetNode;

class TargetCircle
{
public:
	TargetCircle()
	{
		m_nCurrentIndex = 0;
	}
	BOOL IsDoIt(INT nID);
	//查找是否在黑名单中
	INT FindInBlackList(INT nID)
	{
		if (nID > 0)
			nID = (INT)Npc[nID].m_dwID;
		INT i;
		INT nTick = KSysService::GetTickCount() ;
		for(i = 0; i < AI_BLACK_COUNT; i++)
		{
			if (nID == m_Nodes[i].m_IDToHit)
			{
				if (nTick - m_Nodes[i].m_nAddTime > AI_TIME_BLACK_LAST)
				{
					m_Nodes[i].m_IDToHit = 0;
					return -1;
				}
				return i;
			}
		}
		return -1;
	}
	VOID ClearCurrent()
	{
		m_nCurTargetId = 0;
	};
	//获取黑名单
	VOID GetBlackList(INT *pInt, INT &nCount)
	{
		nCount = m_nCurrentIndex;
		for (INT i = 0; i < AI_BLACK_COUNT; i++)
			pInt[i] = m_Nodes[i].m_IDToHit;
	}
	//清除黑名单
	VOID ClearBlackList()
	{
		m_nCurrentIndex = 0;
		for (INT i = 0; i < AI_BLACK_COUNT; i++)
		{
			m_Nodes[i].m_IDToHit = 0;
		}
		m_BlackTimes = 0;
	}
	VOID SetBlackClose() 
	{ 
		if (m_BlackTimes < AI_TIMES_ADDBLACK - AI_TIMES_BLACK_CLOSE)
			m_BlackTimes = AI_TIMES_ADDBLACK - AI_TIMES_BLACK_CLOSE; 
	}
protected:
	TargetNode	m_Nodes[AI_BLACK_COUNT];
	INT			m_nCurrentIndex; //黑名单索引
	INT			m_nCurTargetId; //当前目标ID
	INT			m_nCurTargetLife; //当前目标血量
	INT			m_BlackTimes; //无效攻击累计
private:
	VOID NewTarget(INT nID);
	//加入到黑名单
	VOID AddToBlackList(INT nID)
	{
		if (nID > 0)
			nID = (INT)Npc[nID].m_dwID;
		if (m_nCurrentIndex >= AI_BLACK_COUNT)
		{
			m_nCurrentIndex = 0;
		}
		m_Nodes[m_nCurrentIndex].m_IDToHit = nID;
		m_Nodes[m_nCurrentIndex].m_nAddTime = KSysService::GetTickCount();
		m_nCurrentIndex++;
	}
};
VOID TargetCircle::NewTarget(INT nID)
{
	m_nCurTargetId = nID;
	m_nCurTargetLife = Npc[nID].m_CurrentLife;
	m_BlackTimes = 0;
}
BOOL TargetCircle::IsDoIt(INT nID)
{
	if (nID <= 0)
		return FALSE;
	if (FindInBlackList(nID) != -1) //在黑名单中
		return FALSE;
	if (Npc[nID].GetDoing() == do_death ||
		Npc[nID].GetDoing() == do_revive ||
		Npc[nID].m_dwID <= 0)
		return FALSE;
	if (nID == m_nCurTargetId) //如果是当前目标
	{
		//规定时间内打目标不伤血
		if (m_BlackTimes > AI_TIMES_ADDBLACK)
		{
			if (Npc[nID].m_Kind != kind_player && Npc[nID].m_Kind != kind_partner) //玩家没有黑名单，仅重新寻怪
				AddToBlackList(nID); //加入到黑名单
			else
				m_nCurTargetId = 0;
			return FALSE;
		}
		else
		{
			if (m_nCurTargetLife > Npc[nID].m_CurrentLife)
			{
				m_BlackTimes = AI_TIMES_ADDBLACK - AI_TIMES_BLACK_CLOSE;
			}
			else
			{
				if (g_cAutoAi.m_bIsChangeCurTarget)
				{
					m_BlackTimes++;
				}
			}
			m_nCurTargetLife = Npc[nID].m_CurrentLife;
			return TRUE;
		}
	}
	else
	{
		NewTarget(nID);
		return TRUE;
	}
}

TargetCircle g_AutoAiTarget;

// ---------KAutoMove----------------------------------------------------------------
INT KAutoMove::AutoMove() 
{
	if (m_CurDir < 0)
		m_CurDir = KSysService::Rand() % 8;
	INT nDis;
	if ((nDis = FindPathAndMove(m_CurDir)) <= 0)
	{	
		//变成反方向
		m_CurDir = (m_CurDir + 4) & 0x7;
		return FindPathAndMove(m_CurDir);
	}
	return nDis;
}

INT KAutoMove::AutoMoveTo( INT nX, INT nY ) 
{
	INT nOrgX, nOrgY;
	INT nPlayer = Player[CLIENT_PLAYER_INDEX].m_nIndex;
	KNpc *pNpc = &Npc[nPlayer];
	pNpc->GetMpsPos(&nOrgX, &nOrgY);
	INT nDis = g_GetDisSquare(nOrgX, nOrgY, nX, nY);
	//距离接近，直接走
	if (nDis <= 200 * 200)
	{
		pNpc->ClientGotoPos(nX, nY);
		return -2;	//-2表示已经到达
	}
	INT nDir = g_GetDirIndex(nOrgX, nOrgY, nX, nY);
	nDir = KSelfNpcFindPath().Dir64To8(nDir);
	//若以前没走过
	if (m_CurDir < 0)
		m_CurDir = nDir;
	//和当前方向合成（否则易出现在两点来回走的情况）
	INT nCDir = (nDir + m_CurDir) / 2;
	//角度大于180度，取反角度合成
	if (abs(nDir - m_CurDir) > 4)
		nCDir = (nCDir + 4) & 0x7;
	m_CurDir = nCDir;
	//如果这个方向不能走，则走随机方向
	if ((nDis = FindPathAndMove(m_CurDir, MIN((INT)sqrt((DOUBLE)nDis), (INT)DIS_MOVE))) <= 0)
	{
		Reset();
		return AutoMove();
	}
	return nDis;
}

INT KAutoMove::FindPathAndMove( INT &nDir8, INT minStep /*= -1*/ ) 
{
	if (minStep < 0)
		minStep = DIS_MOVE;
	INT nPlayer = Player[CLIENT_PLAYER_INDEX].m_nIndex;
	KNpc *pNpc = &Npc[nPlayer];
	KSelfNpcFindPath path;
	INT nOrgX, nOrgY;
	pNpc->GetMpsPos(&nOrgX, &nOrgY);
	INT nDir64 = path.Dir8To64(nDir8 & 0x7);
	INT i, x, y;
	INT nSpeed = pNpc->m_CurrentRunSpeed;
	INT nLastDis = CheckDirectMoveDis(pNpc, nDir8, nSpeed);
	INT nMaxDis = nLastDis;
	i = 1;
	INT nGoodDir = nDir8;
	while(nMaxDis < minStep && i < 3) 
	{
		INT nDis = CheckDirectMoveDis(pNpc, nDir8 + i, nSpeed);
		if (nDis > nMaxDis)
		{
			nMaxDis = nDis;
			nGoodDir = (nDir8 + i + 8) & 0x7;
		}
		i = i > 0 ? -i : 1-i;
	}
	if (nMaxDis == 0)
	{	
		//各个方向都走不通
		return 0;
	}
	nDir8 = nGoodDir;
	nDir64 = path.Dir8To64(nDir8 & 0x7);
	x = (g_DirCos( nDir64, 64 ) * nMaxDis) >> 10;
	y = (g_DirSin( nDir64, 64 ) * nMaxDis) >> 10;
	pNpc->ClientGotoPos(nOrgX + x, nOrgY + y);
	return nMaxDis;
}

INT KAutoMove::CheckDirectMoveDis( KNpc *pNpc, INT nDir8, INT nSpeed ) 
{
	KSelfNpcFindPath path;
	INT nDir64 = path.Dir8To64((nDir8 + 8) & 0x7);
	INT nDx = g_DirCos( nDir64, 64 ) * nSpeed;
	INT nDy = g_DirSin( nDir64, 64 ) * nSpeed;
	INT nDis = 0;
	INT i;
	for (i = 1; i <= DIS_MOVE / nSpeed; i++)
	{
		if (path.CheckBarrier(nDx * i, nDy * i) != Obstacle_NULL)
			return nDis;
		nDis += nSpeed;
	}
	return DIS_MOVE;
}
// --------AutoAi-----------------------------------------------------------------
AutoAi::AutoAi()
{
	memset(&m_AiBFlag, 0, sizeof(m_AiBFlag));
	m_bLock = FALSE;
	m_nActiveRadius = 0;
	m_nVisionRadius = 0;
	m_ItemPickIndex = 0;
	m_eAiState = AI_FREE;
	m_eCurCommon = NO_COMMON;
	m_nNowAttackX = m_nNowAttackY = m_nLastAttackX = m_nLastAttackY = 0;
	m_nLastRunNormalTime = 0;
	m_ErrorNum = 0;
	m_nNormalSleep = 0;
	m_bLastFaultFlag = FALSE;
	st_drug1pos = st_drug2pos = st_groundpick = 0;
	m_nNormalRunCounter = 0;
	m_bShopOpen = FALSE;
	m_nAiState = 0;
	m_nAttackRadius = 1000;
	st_pickitemvalue = 2000;
	m_nDropItemPos = 0;
	m_bIsChangeCurTarget = TRUE;
	ZeroMemory(m_aDropItemId, sizeof(m_aDropItemId));
}

VOID AutoAi::AddDrapItem( INT nItem, INT nx, INT ny ) 
{
	if (m_ItemPickIndex >= ITEM_PICK_NUM) m_ItemPickIndex = 0;
	m_aryItemPickList[m_ItemPickIndex].nItem = nItem;
	m_aryItemPickList[m_ItemPickIndex].nX = nx;
	m_aryItemPickList[m_ItemPickIndex].nY = ny;
	m_ItemPickIndex++;
}

VOID AutoAi::SetTruePos( INT x, INT y ) 
{
	m_nRealX = x;
	m_nRealY = y;
}

BOOL AutoAi::AutoRepairItem() 
{
	return FALSE;
}

BOOL AutoAi::RunNormal() 
{
	if (m_bLock)
		return TRUE;
	m_nNormalRunCounter++;
	if (m_nNormalSleep > 0)
	{
		m_nNormalSleep--;
		return TRUE;
	}
#define NORMAL_SLEEP 3
	INT nIdx = Player[CLIENT_PLAYER_INDEX].m_nIndex;
	INT nCurTime = SubWorld[0].m_dwCurrentTime;
	if (m_nAiState)
	{
		if (nCurTime > m_nLastRunNormalTime && g_pMainScript)  
		{
			m_nLastRunNormalTime = nCurTime + NORMAL_SLEEP;
			KScriptFunctionCallHelper sfcHelper(*g_pMainScript);
			sfcHelper.SetMe(&Player[CLIENT_PLAYER_INDEX]);
			g_pMainScript->CallTableFunction("AutoAi", "AI_Normal", 0, "ddddd", 
				Npc[nIdx].m_FightMode, Npc[nIdx].m_CurrentLife, Npc[nIdx].m_CurrentMana, 
				Npc[nIdx].m_CurrentLifeMax, Npc[nIdx].m_CurrentManaMax);
		}
	}

	// 有可能调脚本时此值变为非0
	return m_nNormalSleep;
}

BOOL AutoAi::RunAutoAttack()
{
#define RUN_AWAY_TIME 18
	if (!m_nAiState || m_nNormalSleep > 0)
		return FALSE;
	INT nPlayerNpcIdx = Player[CLIENT_PLAYER_INDEX].m_nIndex;
	KNpc* pNpc = &Npc[Npc[nPlayerNpcIdx].m_cAi.m_nPeopleIdx];
	BOOL bScriptAIContinue = TRUE;
	if ( Npc[nPlayerNpcIdx].m_FightMode)  
	{
		INT nPopeleIdx = Npc[nPlayerNpcIdx].m_cAi.m_nPeopleIdx;
		if (nPopeleIdx && !g_AutoAiTarget.IsDoIt(nPopeleIdx))
		{
			if (!InEyeshot(nPopeleIdx))
			{
				g_AutoAiTarget.ClearBlackList();
				m_nRunAwayNpc = 0;
			}
			else if (nPopeleIdx == m_nRunAwayNpc)
			{
				INT nCurX, nCurY;
				Npc[nPopeleIdx].GetMpsPos(&nCurX, &nCurY);
				if (nCurX != m_nRunAwayNpcX && nCurY != m_nRunAwayNpcY)
				{
					g_AutoAiTarget.ClearBlackList();
					m_nRunAwayNpc = 0;
					return FALSE;
				}
				else
				{
					Flee(nPopeleIdx);
					return TRUE;
				}
			}
			else if (Npc[nPopeleIdx].GetDoing() == do_skill || Npc[nPopeleIdx].m_CurrentLife != Npc[nPopeleIdx].m_CurrentLifeMax)
			{
				m_nRunAwayNpc = nPopeleIdx;
				Npc[nPopeleIdx].GetMpsPos(&m_nRunAwayNpcX, &m_nRunAwayNpcY);
				Flee(nPopeleIdx);
				return TRUE;
			}
			//引怪
			m_nNormalSleep += RUN_AWAY_TIME;
			Npc[nPlayerNpcIdx].m_cAi.m_nPeopleIdx = 0;
			RandomMove();
			return TRUE;
		}
		if (PlayerCanAttack(nPopeleIdx))
		{
			/*if (KSysService::Engine_Random(100) > 80) //且战且退
			{
			NpcAI.Flee(Npc[nPlayerNpcIdx].m_cAi.m_nPeopleIdx);
			Npc[nPlayerNpcIdx].m_NextAITime = nCurTime + RUN_AWAY_TIME;
			return;
			}
			else*/
			// 修正攻击距离
			if (m_nAttackRadius < Npc[nPlayerNpcIdx].m_CurrentAttackRadius)
				Npc[nPlayerNpcIdx].m_CurrentAttackRadius = m_nAttackRadius;
			g_pMainScript->CallTableFunction("AutoAi", "ChangeSkill", 0, "");
			return FALSE;
		}
		else
		{
			Npc[nPlayerNpcIdx].m_cAi.m_nPeopleIdx = 0;
			bScriptAIContinue = FALSE;
		}
	}
	else
	{
		bScriptAIContinue = FALSE;
	}
	if (!bScriptAIContinue)
	{
		if (Npc[nPlayerNpcIdx].m_cAi.m_nObjectIdx > 0)
		{
			return FALSE;
		}
		while(!m_vecItemToPick.empty())
		{
			INT nObj = m_vecItemToPick.back();
			m_vecItemToPick.pop_back();
			if (nObj > 0 && Object[nObj].m_nID > 0)
			{
				INT nX, nY;
				Object[nObj].GetMpsPos(&nX, &nY);
				Npc[nPlayerNpcIdx].ClientGotoPos(nX, nY);
				Npc[nPlayerNpcIdx].m_cAi.m_nObjectIdx = nObj;
				return FALSE;
			}				
		}

		if (Npc[nPlayerNpcIdx].GetLastDamageNpcIdx() > 0)
		{
			if (PlayerCanAttack(Npc[nPlayerNpcIdx].GetLastDamageNpcIdx()))
			{
				Npc[nPlayerNpcIdx].m_cAi.m_nPeopleIdx = Npc[nPlayerNpcIdx].GetLastDamageNpcIdx();
				return FALSE;
			}
			else
			{
				Npc[nPlayerNpcIdx].SetLastDamageNpcIdx(0);
			}
		}

		KScriptFunctionCallHelper sfcHelper(*g_pMainScript);
		sfcHelper.SetMe(&Player[CLIENT_PLAYER_INDEX]);
		if (!g_pMainScript->CallTableFunction("AutoAi", "AutoFight", 0, ""))
			return FALSE;
	}
	return TRUE;
}

BOOL AutoAi::CheckIfValueItem( INT nItem ) 
{
	BOOL nResult = 0;
	std::vector<std::string>::iterator itName;
	std::string sName = Item[nItem].GetName();
	for (itName = m_vecItemPickName.begin(); itName != m_vecItemPickName.end(); ++itName)
	{
		if (sName.find(*itName) != std::string::npos) //如果在可捡物品表中
		{
			return TRUE;
		}
	}
	for (itName = m_vecAvoidPickName.begin(); itName != m_vecAvoidPickName.end(); ++itName)
	{
		if (sName.find(*itName) != std::string::npos) //如果在排除表中
		{
			return FALSE;
		}
	}
	KScriptFunctionCallHelper sfcHelper(*g_pMainScript);
	sfcHelper.SetIt(&Item[nItem]);
	sfcHelper.SetMe(&Player[CLIENT_PLAYER_INDEX]);
	if (g_pMainScript->CallTableFunction("AutoAi", "CheckItemPickable", 1, ""))
	{
		INT nReturn = g_pMainScript->GetInt(-1);
		// 如果脚本返回值为1表示可捡。
		if (nReturn == 1)
			return TRUE;
	}
	// 从脚本判断
	return FALSE;
}

VOID AutoAi::ReturnCity() 
{
	// 该协议现在废的
	PLAYER_AUTOATTACKNPC AttackNpc;
	ZeroMemory(&AttackNpc, sizeof(AttackNpc));
	AttackNpc.m_btProtocolType = c2s_autoattacknpc;
	AttackNpc.m_btChildProtocolType = PLAYER_AUTOATTACKNPC::type_useitem;
	if (g_pCoreClient)
		g_pCoreClient->SendData((LPBYTE)&AttackNpc, sizeof(AttackNpc));
}

static CHAR * _GetWord(CHAR *buf, const CHAR *&szSentence)
{
#define _IS_SPACE(A) ((A) == 0 || (A) == ' ' || (A) == '\n' || (A) == ',')
	CHAR *szRet = buf;
	while (_IS_SPACE(*szSentence))
	{
		if (*szSentence == '\0')
			break ;
		szSentence++;
	}
	while (!_IS_SPACE(*szSentence))
	{
		*buf++ = *szSentence++;
	}
	*buf = 0;
	return szRet;
}

VOID AutoAi::AutoAiItemDrop( INT nItem, INT nX, INT nY )
{
	if (Object[nItem].m_nKind == Obj_Kind_Item || Object[nItem].m_nKind == Obj_Kind_Money)
	{
		INT nIX, nIY;
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetMpsPos(&nIX, &nIY);
		if (Object[nItem].m_nKind == Obj_Kind_Money)
		{
		}
		else if (Object[nItem].m_nKind == Obj_Kind_Item)	
		{
			/*			if (Object[nItem].m_nColorID == 2 || Object[nItem].m_nColorID == 4) //黄金或紫
			{
			return;
			}
			if (std::binary_search(m_vecItemPickType.begin(),  
			m_vecItemPickType.end(), Object[nItem].m_nDataID)) //如果在可捡物品表中
			{
			Player[CLIENT_PLAYER_INDEX].PickUpObj(nItem);
			return;
			}

			std::vector<std::string>::iterator itName;
			std::string sName = Object[nItem].m_szName;
			for (itName = m_vecItemPickName.begin(); itName != m_vecItemPickName.end(); ++itName)
			{
			if (sName.find(*itName) != std::string::npos) //如果在可捡物品表中
			{
			Player[CLIENT_PLAYER_INDEX].PickUpObj(nItem);
			return;
			}
			}*/
			for (INT i = 0; i < countof(m_aDropItemId); ++i)
			{
				if (m_aDropItemId[i] == Object[nItem].m_nItemDataID)
					return;
			}
			std::vector<std::string>::iterator itName;
			std::string sName = Object[nItem].m_szName;
			for (itName = m_vecAvoidPickName.begin(); itName != m_vecAvoidPickName.end(); ++itName)
			{
				if (sName.find(*itName) != std::string::npos) //如果在排除表中
				{
					return;
				}
			}
		}
		else
		{
			return;
		}
		INT nDisSquare = g_GetDisSquare(nX, nY, nIX, nIY);
		if (nDisSquare < PLAYER_PICKUP_CLIENT_DISTANCE * PLAYER_PICKUP_CLIENT_DISTANCE && !m_bLock)
		{
			Player[CLIENT_PLAYER_INDEX].m_cPlayerItem.PickUpObj(nItem);
		}
		if (m_nAiState && nDisSquare < 300000)
		{
			m_vecItemToPick.push_back(nItem);
		}
	}
}

VOID AutoAi::RandomMove() 
{
	KSelfNpcFindPath path;
	INT nPlayer = Player[CLIENT_PLAYER_INDEX].m_nIndex;
	INT ndx, ndy;
	INT nTimes = 100;
	while (nTimes--)
	{
		ndx = 200 - 400 * KSysService::Engine_Random(1000) / 1000;
		ndy = (KSysService::Engine_Random(100) > 50) ? (INT)sqrt((DOUBLE)200*200 - ndx*ndx) : -(INT)sqrt((DOUBLE)200*200 - ndx*ndx);
		if (path.RealCheckBarrier(ndx, ndy) == Obstacle_NULL)
			break;
	}
	INT nx, ny;
	Npc[nPlayer].GetMpsPos(&nx, &ny);
	Npc[nPlayer].ClientGotoPos(nx + ndx, ny + ndy);
}

BOOL AutoAi::CheckIfCanDirectMove( INT nx, INT ny ) 
{
	KSelfNpcFindPath path;
	INT nPlayer = Player[CLIENT_PLAYER_INDEX].m_nIndex;
	INT nMoveSpeed = SubWorld[0].m_nCellWidth;	//设移动速度为一格子
	INT nOrgx, nOrgy;
	Npc[nPlayer].GetMpsPos(&nOrgx, &nOrgy);
	INT ndx = nx - nOrgx;
	INT ndy = ny - nOrgy;
	//需要移动多少步
	DOUBLE nSteps = sqrt((DOUBLE)ndx*ndx + ndy*ndy) / nMoveSpeed;
	if (nSteps < 1)	//相距不足一步，中间不可能有障碍
		return TRUE;
	//获取x、y方向每步位移
	DOUBLE noffsetx = ndx / nSteps;
	DOUBLE noffsety = ndy / nSteps;
	for (INT i = 1; i <= INT(nSteps); i++)
	{
		INT nX = INT(noffsetx * i);
		INT nY = INT(noffsety * i);
		if (path.RealCheckBarrier(nX, nY) != Obstacle_NULL)
			return FALSE;
	}
	return TRUE;
}

VOID AutoAi::OnSee( KActor *pActor, TEvent event, UINT lParam, UINT wParam ) 
{
	INT nPlayer = Player[CLIENT_PLAYER_INDEX].m_nIndex;
	switch(event)
	{
	case SUBWORLD_ADD_ITEM:
		{
			std::pair<INT, INT> *pPos = (std::pair<INT, INT> *)wParam;
			AutoAiItemDrop(lParam, pPos->first, pPos->second);
		}
		break;
	case SUBWORLD_NPC_PKCHANGE:
		{
			INT nNpcIndex = (INT)lParam;
			if (nNpcIndex <= 0 || nNpcIndex > MAX_NPC)
				break;
			//if(m_AiBFlag.killer_warning && nNpcIndex != nPlayer && Npc[nNpcIndex].m_nPKFlag == 2 && Npc[nNpcIndex].m_FightMode)
			//	Npc[nPlayer].SetChatInfo("警告：", KILLER_WARNING, strlen(KILLER_WARNING));
			if (m_eAiState == AI_RUNSCRIPT && m_eCurCommon == AUTO_ATTACK)
			{
				m_nNormalSleep = 1800;
				ReturnCity();
			}
		}
		break;
	case SUBWORLD_NEW_NPC:
		{
			INT nGoldType = Npc[lParam].m_cGold.GetGoldType();
			if (nGoldType > 0)
			{
				//if (nGoldType <= g_cNpcMgr.m_cGoldTemplate.m_nEffectTypeNum)
				//	Npc[nPlayer].SetChatInfo("", "<color=blue>发现蓝怪！");
				//else
				//	Npc[nPlayer].SetChatInfo("", "<color=gold>发现金怪！");
			}
		}
		break;
	case PLAYER_ADDED_ITEM:
		if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_FightMode && m_AiBFlag.auto_pick && lParam > 0)
		{
			KLISTITEM *pPlayerItem = Player[CLIENT_PLAYER_INDEX].m_cPlayerItem.GetItem();
			if (pPlayerItem && !CheckIfValueItem(pPlayerItem[lParam].nIdx))
			{
				switch (pPlayerItem[lParam].sPos.eRoom)
				{
				case emROOM_MAINBAG:
				case emROOM_EXTBAG1:
				case emROOM_EXTBAG2:
				case emROOM_EXTBAG3:
					Player[CLIENT_PLAYER_INDEX].m_cPlayerItem.ThrowAwayItem(Item[pPlayerItem[lParam].nIdx].GetID());
					m_aDropItemId[m_nDropItemPos++ % countof(m_aDropItemId)] = Item[pPlayerItem[lParam].nIdx].GetID();
					break;
				}
			}
		}
		else	//自动合玄晶
		{
			//DoAutoCompound();
		}
		break;
	default:
		break;
	}
}

BOOL AutoAi::ProcessHandCommand( const CHAR* szCommon, INT lParam, INT wParam ) 
{
#define STR_EQUAL(A, B) (strcmp(A, B) == 0)
	HAND_COMMOND cmd = NO_COMMON;
	if STR_EQUAL(szCommon, "auto_pick") 
		cmd = AUTO_PICK;
	else if STR_EQUAL(szCommon, "auto_drug") 
		cmd = AUTO_DRUG;
	else if STR_EQUAL(szCommon, "auto_fight")
		cmd = AUTO_ATTACK;
	else if STR_EQUAL(szCommon, "goto")
		cmd = AUTO_GOTO;
	else if STR_EQUAL(szCommon, "storage")
		cmd = AUTO_STOREAGE;
	return ProcessHandCommand(cmd, lParam, wParam);
}

BOOL AutoAi::ProcessHandCommand( HAND_COMMOND cmd, INT lParam /*= 0*/, INT wParam /*= 0*/ ) 
{
	INT nNpcIdx = Player[CLIENT_PLAYER_INDEX].m_nIndex;
	if (nNpcIdx <= 0 || Npc[nNpcIdx].m_dwID <= 0)
		return FALSE;
	switch(cmd)
	{
	case AUTO_ATTACK:
		{
			InitAutoAttack(lParam);
		}
		break;
	case AUTO_PICK:
		{
			switch(lParam)
			{
			case 0:
				m_AiBFlag.auto_pick = 0;
				break;
			case 1:
				m_AiBFlag.auto_pick = 1;
				break;
			default:
				m_AiBFlag.auto_pick = !m_AiBFlag.auto_pick;
				break;
			}
			if (m_AiBFlag.auto_pick)
			{
				Watch(&SubWorld[0], SUBWORLD_ADD_ITEM);
				Watch(&Player[CLIENT_PLAYER_INDEX], PLAYER_ADDED_ITEM);
			}
			else
			{
				UnWatch(&SubWorld[0], SUBWORLD_ADD_ITEM);
				UnWatch(&Player[CLIENT_PLAYER_INDEX], PLAYER_ADDED_ITEM);
			}
		}
		break;
	case AUTO_DRUG:
		{
			switch(lParam)
			{
			case 0:
				m_AiBFlag.auto_drug = 0;
				break;
			case 1:
				m_AiBFlag.auto_drug = 1;
				break;
			default:
				m_AiBFlag.auto_drug = !m_AiBFlag.auto_drug;
				break;
			}
			m_AiBFlag.auto_drug = !m_AiBFlag.auto_drug;
		}
		break;
	case DEATH_PROTECT:
		{
			switch(lParam)
			{
			case 0:
				m_AiBFlag.death_protect = 0;
				break;
			case 1:
				m_AiBFlag.death_protect = 1;
				break;
			default:
				m_AiBFlag.death_protect = !m_AiBFlag.death_protect;
				break;
			}
			m_AiBFlag.death_protect = !m_AiBFlag.death_protect;
		}
		break;
	case RELOAD_SCRIPT:
		Init();
		break;
	default:
		break;
	}
	return TRUE;
}

VOID AutoAi::ProcessScriptControl( UINT uKey ) 
{
	KScriptFunctionCallHelper sfcHelper(*g_pMainScript);
	sfcHelper.SetMe(&Player[CLIENT_PLAYER_INDEX]);
	VERIFY(g_pMainScript->CallTableFunction("AutoAi", "ProcessControl", 0, "u", uKey));
}

VOID AutoAi::InitAutoAttack( INT nMode ) 
{
	BOOL bActive = TRUE;
	switch(nMode)
	{
	case 0:
		bActive = FALSE;
		break;
	case 1:
		if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_FightMode != 1)
		{
			gSysMsg2Player(MSG_FIGHTMODE_AUTOFIGHT);
			return;
		}
		bActive = TRUE;
		break;
	default:
		bActive = m_nAiState ? FALSE : TRUE;
		break;
	}
	m_nAiState = bActive;
	m_nAttackRadius = 1000;
	if (bActive)
	{
		INT nNpcIdx = Player[CLIENT_PLAYER_INDEX].m_nIndex;
		INT nx, ny;
		Npc[nNpcIdx].GetMpsPos(&nx, &ny);
		Npc[nNpcIdx].m_OriginX = nx;
		Npc[nNpcIdx].m_OriginY = ny;
		Npc[nNpcIdx].m_cAi.m_nActiveRadius = m_nActiveRadius;
		Npc[nNpcIdx].m_cAi.m_nVisionRadius = m_nVisionRadius;
		g_AutoAiTarget.ClearCurrent();
		g_AutoAiTarget.ClearBlackList();
		m_vecItemToPick.erase(m_vecItemToPick.begin(), m_vecItemToPick.end());
		KScriptFunctionCallHelper sfcHelper(*g_pMainScript);
		sfcHelper.SetMe(&Player[CLIENT_PLAYER_INDEX]);
		m_nDropItemPos = 0;
		ZeroMemory(m_aDropItemId, sizeof(m_aDropItemId));
		//Watch(&Player[CLIENT_PLAYER_INDEX], PLAYER_DIE);
	}
	else
	{		
		//UnWatch(&Player[CLIENT_PLAYER_INDEX], PLAYER_DIE);
		//UnWatch(SubWorld[0], SUBWORLD_NPC_PKCHANGE);
	}

	VERIFY(g_pMainScript->CallTableFunction("AutoAi", "AI_InitAttack", 0, "d", bActive));
}

INT AutoAi::PlayerFindAim() 
{
	INT nPlayerNpcIdx = Player[CLIENT_PLAYER_INDEX].m_nIndex;
	if (m_nActiveRadius <= 0)
		return 0;	
	//不要每次都索引
	INT nPlayerX = Npc[nPlayerNpcIdx].m_MapX, nPlayerY = Npc[nPlayerNpcIdx].m_MapY, 
		nPlayerRegion = Npc[nPlayerNpcIdx].m_RegionIndex;
	if (nPlayerRegion < 0 ||  Npc[nPlayerNpcIdx].m_RegionIndex < 0)
		return 0;
	INT nSubWorld = Npc[nPlayerNpcIdx].m_SubWorldIndex;
	INT nxRange = m_nActiveRadius / SubWorld[nSubWorld].m_nCellWidth + 1;
	INT nyRange = m_nActiveRadius / SubWorld[nSubWorld].m_nCellHeight + 1;
	INT nRangeDis = ((nxRange + nyRange)<<2) + abs(nxRange - nyRange); 
	// 检查活动范围内的格子里的敌对玩家和NPC
	std::map<INT, INT> mpPlayerfound, mpNpcFound;
	KIndexNode *pNode;
	INT nStep = 0;
	//从中间region开始
	INT nRegion = nPlayerRegion;	//搜索是以Player为中心搜索
	INT nXOffset = 0, nYOffset = 0;	//X和Y的偏移(切换region时用)
_tagPlayerFind1:
	pNode = (KIndexNode *)SubWorld[nSubWorld].m_Region[nRegion].m_NpcList.GetHead();
	for(; pNode != NULL; pNode = (KIndexNode *)pNode->GetNext())
	{
		if (pNode->m_nIndex == nPlayerNpcIdx)
			continue;
		//对比到Player间距
		INT xDis = abs(Npc[pNode->m_nIndex].m_MapX - nPlayerX - nXOffset);
		INT yDis = abs(Npc[pNode->m_nIndex].m_MapY - nPlayerY - nYOffset);

		INT nDis = ((xDis + yDis) << 2) + abs(xDis - yDis);
		if (nDis < nRangeDis)	//如在范围之内
		{
			if (Npc[pNode->m_nIndex].m_Kind == kind_normal) //普通npc，enemy在后面再判断
			{
				mpNpcFound.insert(std::pair<INT, INT>(nDis, pNode->m_nIndex));
			}
			else	//否则视为player
			{
				
				if (KNpcRelation::CheckRelationSet(Npc[nPlayerNpcIdx], Npc[pNode->m_nIndex], relation_enemy)) //玩家在这里判断enemy
					mpPlayerfound.insert(std::pair<INT, INT>(nDis, pNode->m_nIndex));
			}
		}
	}
_tagPlayerFind2:
	switch(nStep++)		//此循环搜索周围八个region
	{
	case 0:	//左
		if (nPlayerX - nxRange < 0)
		{
			nXOffset = SubWorld[nSubWorld].m_nRegionWidth;	//坐标右移
			nRegion = SubWorld[nSubWorld].m_Region[nPlayerRegion].m_nConnectRegion[2];
			if (nRegion >= 0)
				goto _tagPlayerFind1;
		}
		goto _tagPlayerFind2;
	case 1: //右
		if (nPlayerX + nxRange > SubWorld[nSubWorld].m_nRegionWidth)
		{
			nXOffset = -SubWorld[nSubWorld].m_nRegionWidth; //坐标左移
			nRegion = SubWorld[nSubWorld].m_Region[nPlayerRegion].m_nConnectRegion[6];
			if (nRegion >= 0)
				goto _tagPlayerFind1;
		}
		goto _tagPlayerFind2;
	case 2:	//上
		if (nPlayerY - nyRange < 0)
		{
			nXOffset = 0;
			nYOffset = SubWorld[nSubWorld].m_nRegionHeight; //坐标下移
			nRegion = SubWorld[nSubWorld].m_Region[nPlayerRegion].m_nConnectRegion[4];
			if (nRegion >= 0)
				goto _tagPlayerFind1;
		}
		goto _tagPlayerFind2;
	case 3: //下
		if (nPlayerY + nyRange > SubWorld[nSubWorld].m_nRegionHeight)
		{
			nXOffset = 0;
			nYOffset = -SubWorld[nSubWorld].m_nRegionHeight; //坐标上移
			nRegion = SubWorld[nSubWorld].m_Region[nPlayerRegion].m_nConnectRegion[0];
			if (nRegion >= 0)
				goto _tagPlayerFind1;
		}
		goto _tagPlayerFind2;
	case 4: //左上(近似)
		if (((nPlayerX + nPlayerY + 2)<<2) + abs(nPlayerX-nPlayerY) < nRangeDis) //左上region右下角的点到原点间距在范围内？
		{
			nXOffset = SubWorld[nSubWorld].m_nRegionWidth;	//坐标右移
			nYOffset = SubWorld[nSubWorld].m_nRegionHeight; //坐标下移
			nRegion = SubWorld[nSubWorld].m_Region[nPlayerRegion].m_nConnectRegion[3];
			if (nRegion >= 0)
				goto _tagPlayerFind1;
		}
		goto _tagPlayerFind2;
	case 5: //左下(近似)
		if (((nPlayerX + SubWorld[nSubWorld].m_nRegionHeight - nPlayerY + 2)<<2) + 
			abs(nPlayerX - SubWorld[nSubWorld].m_nRegionHeight + nPlayerY) < nRangeDis) //左下region右上角的点到原点间距...
		{
			nXOffset = SubWorld[nSubWorld].m_nRegionWidth;	//坐标右移
			nYOffset = -SubWorld[nSubWorld].m_nRegionHeight; //坐标上移
			nRegion = SubWorld[nSubWorld].m_Region[nPlayerRegion].m_nConnectRegion[1];
			if (nRegion >= 0)
				goto _tagPlayerFind1;
		}
		goto _tagPlayerFind2;
	case 6: //右上(近似)
		if (((SubWorld[nSubWorld].m_nRegionWidth - nPlayerX + nPlayerY + 2)<<2) + 
			abs(nPlayerX - SubWorld[nSubWorld].m_nRegionWidth + nPlayerY) < nRangeDis) //右上region左下角的点到原点间距...
		{
			nXOffset = -SubWorld[nSubWorld].m_nRegionWidth; //坐标左移
			nYOffset = +SubWorld[nSubWorld].m_nRegionHeight; //坐标下移
			nRegion = SubWorld[nSubWorld].m_Region[nPlayerRegion].m_nConnectRegion[5];
			if (nRegion >= 0)
				goto _tagPlayerFind1;
		}
		goto _tagPlayerFind2;
	case 7: //右下(近似)
		if (((SubWorld[nSubWorld].m_nRegionWidth - nPlayerX + SubWorld[nSubWorld].m_nRegionHeight - nPlayerY + 2)<<2) + 
			abs(nPlayerX - nPlayerY - SubWorld[nSubWorld].m_nRegionWidth + SubWorld[nSubWorld].m_nRegionHeight) < nRangeDis) //...
		{
			nXOffset = -SubWorld[nSubWorld].m_nRegionWidth; //坐标左移
			nYOffset = -SubWorld[nSubWorld].m_nRegionHeight; //坐标上移
			nRegion = SubWorld[nSubWorld].m_Region[nPlayerRegion].m_nConnectRegion[7];
			if (nRegion >= 0)
				goto _tagPlayerFind1;
		}
		goto _tagPlayerFind2;		
	default:
		break;
	}
	//返回最近的敌人(并检查黑名单)
	//玩家优先处理
	std::map<INT, INT>::iterator it;
	/*	for (it = mpPlayerfound.begin(); it != mpPlayerfound.end(); ++it)
	{
	if (pFightPartner->m_Target.IsDoIt(it->second))
	return it->second;
	}*/
	for (it = mpNpcFound.begin(); it != mpNpcFound.end(); ++it)
	{
		if (KNpcRelation::CheckRelationSet(Npc[nPlayerNpcIdx], Npc[it->second], relation_enemy) && 
			g_AutoAiTarget.IsDoIt(it->second))
		{
			if (!m_vecNpcOnlyAttack.empty())
			{
				std::vector<std::string>::iterator itName;
				std::string sName = Npc[it->second].GetName();
				for (itName = m_vecNpcOnlyAttack.begin(); itName != m_vecNpcOnlyAttack.end(); ++itName)
				{
					if (sName == *itName)
						return it->second;
				}
				return 0;
			}
			if (!m_vecNpcAvoidAttack.empty())
			{
				std::vector<std::string>::iterator itName;
				std::string sName = Npc[it->second].GetName();
				BOOL bFind = FALSE;
				for (itName = m_vecNpcAvoidAttack.begin(); itName != m_vecNpcAvoidAttack.end(); ++itName)
				{
					if (sName == *itName)
					{
						bFind = TRUE;
						break;
					}
				}
				if (bFind)
					continue;
			}
			INT nx, ny;
			Npc[it->second].GetMpsPos(&nx, &ny);
			if (CheckIfCanDirectMove(nx, ny))
				return it->second;
		}
	}
	return 0;
}

BOOL AutoAi::PlayerCanAttack( INT nNpcIdx ) 
{
	if (nNpcIdx <= 0)
		return FALSE;
	if (Npc[nNpcIdx].GetDoing() == do_death ||
		Npc[nNpcIdx].GetDoing() == do_revive ||
		Npc[nNpcIdx].m_dwID <= 0)
		return FALSE;
	INT nSelfIdx = Player[CLIENT_PLAYER_INDEX].m_nIndex;
	if (!InEyeshot(nNpcIdx) || !KNpcRelation::CheckRelationSet(Npc[nSelfIdx], Npc[nNpcIdx], relation_enemy))
		return FALSE;
	return TRUE;
}

VOID AutoAi::InitGameReload( INT nStep ) 
{
	switch(nStep)
	{
	case 0:
		m_bLock = TRUE;
		break;
	case 1:
		{
			INT nIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;
			Npc[nIndex].m_cAi.m_nActiveRadius = m_nActiveRadius;
			Npc[nIndex].m_cAi.m_nVisionRadius = m_nVisionRadius;
			m_bLock = FALSE;
			if (m_eAiState == AI_RUNSCRIPT && m_eCurCommon != AUTO_SLEEP)
			{
				//ReDommon();
			}
		}
		break;
	default:
		break;
	}
}

BOOL AutoAi::InEyeshot(INT nIdx) 
{
	INT nDisSquare = Npc[nIdx].GetDistanceSquare(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex]);

	if (nDisSquare < 0 || g_Square(m_nVisionRadius) <= nDisSquare)
		return FALSE;
	return TRUE;
}


static VOID LoadStrList(std::vector<std::string> &vecList, const CHAR *szName)
{
	const CHAR *pStart;
	const CHAR *pFind = pStart = NULL;
	CHAR buf[255];
	INT nPreIndex = g_pMainScript->GetTopIndex();
	INT nTopIndex = nPreIndex;
	g_pMainScript->GetGlobal("AutoAi");
	INT nTableIndex = ++nTopIndex;

	g_pMainScript->GetTableField(nTableIndex, szName);
	pStart = g_pMainScript->GetStr(++nTableIndex);
	if (!pStart)
	{
		g_pMainScript->SetTopIndex(nPreIndex);
		return;
	}
	pFind = pStart;
	INT iSearch = 0;
	for (; *pFind != 0; pFind++)
	{
		if (*pFind == ';')
		{
			memcpy(buf, pStart + iSearch, pFind - pStart - iSearch);
			buf[pFind - pStart - iSearch] = 0;
			if (buf[0] != '\0')
				vecList.push_back(buf);
			iSearch = pFind - pStart + 1;
		}
	}
	g_pMainScript->SetTopIndex(nPreIndex);
};

BOOL AutoAi::Init() 
{	
	INT nPreIndex = g_pMainScript->GetTopIndex();
	//g_pMainScript->PopStack(nPreIndex);
	//VERIFY(g_pMainScript->DoFile(KDF_AUTOAI_SCTRPT));
	INT nTopIndex = nPreIndex;
	g_pMainScript->GetGlobal("AutoAi");
	INT nTableIndex = ++nTopIndex;
	g_pMainScript->GetTableField(nTableIndex, "ACTIVE_RADIUS");
	m_nActiveRadius = g_pMainScript->GetInt(++nTopIndex);
	g_pMainScript->GetTableField(nTableIndex, "VISION_RADIUS");
	m_nVisionRadius = g_pMainScript->GetInt(++nTopIndex);
	g_pMainScript->GetTableField(nTableIndex, "DRUG1_POS");
	st_drug1pos = g_pMainScript->GetInt(++nTopIndex);
	g_pMainScript->GetTableField(nTableIndex, "DRUG2_POS");
	st_drug2pos = g_pMainScript->GetInt(++nTopIndex);
	g_pMainScript->GetTableField(nTableIndex, "DRUG_PRO");
	st_drugpro = g_pMainScript->GetInt(++nTopIndex);
	g_pMainScript->GetTableField(nTableIndex, "DRUG_FREE_ROOM");
	st_freeroom = g_pMainScript->GetInt(++nTopIndex);
	g_pMainScript->GetTableField(nTableIndex, "MONEY_LEFT");
	st_moneyleft = g_pMainScript->GetInt(++nTopIndex);
	g_pMainScript->GetTableField(nTableIndex, "GROUND_PICK");
	st_groundpick = g_pMainScript->GetInt(++nTopIndex);
	g_pMainScript->GetTableField(nTableIndex, "PICK_ITEM_VALUE");
	st_pickitemvalue = g_pMainScript->GetInt(++nTopIndex);

	m_vecItemPickType.clear();
	m_vecItemPickName.clear();
	m_vecNpcOnlyAttack.clear();
	m_vecAvoidPickName.clear();

	g_pMainScript->SetTopIndex(nPreIndex);

	LoadStrList(m_vecAvoidPickName, "ITEM_NOPICK_LIST");
	//	LoadStrList(g_pMainScript->GetLuaState(), m_vecItemPickType, "ITEM_PICK_TYPE");		
	LoadStrList(m_vecItemPickName, "ITEM_PICK_LIST");
	LoadStrList(m_vecNpcOnlyAttack, "NPC_ONLY_ATTACK");
	LoadStrList(m_vecNpcAvoidAttack, "NPC_AVOID_ATTACK");

	Watch(&SubWorld[0], SUBWORLD_NPC_PKCHANGE);
	Watch(&Player[CLIENT_PLAYER_INDEX], PLAYER_KILL_NPC);
	Watch(&SubWorld[0], SUBWORLD_NEW_NPC);

	m_nLastRunNormalTime = 0;
	return TRUE;
}

VOID AutoAi::OnAttack( INT nNpcIdx ) 
{
	g_AutoAiTarget.SetBlackClose();
	Npc[nNpcIdx].GetMpsPos(&m_nNowAttackX, &m_nNowAttackY);
	return;
}

BOOL AutoAi::InitGameLoad( INT nStep ) 
{
	switch(nStep)
	{
	case 0:
		m_bLock = TRUE;
		break;
	case 1:
		{
			INT nIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;
			m_nActiveRadius = m_nActiveRadius;
			m_nVisionRadius = m_nVisionRadius;
			m_bLock = FALSE;
			if (m_eAiState == AI_RUNSCRIPT && m_eCurCommon != AUTO_SLEEP)
			{
				//ReDommon();
			}
		}
		break;
	default:
		break;
	}
	m_nLastRunNormalTime = 0;
	return TRUE;
}

#define FLEE_SPACE 400
BOOL AutoAi::Flee( INT nNpcIdx ) 
{
	// 即使逃跑失败也延迟1秒
	m_nNormalSleep += RUN_AWAY_TIME;
	if (nNpcIdx <= 0 || nNpcIdx > MAX_NPC || !InEyeshot(nNpcIdx))
		return FALSE;
	double x1, y1, x2, y2;
	INT xPos, yPos;
	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetMpsPos(&xPos, &yPos);
	x1 = xPos, y1 = yPos;
	Npc[nNpcIdx].GetMpsPos(&xPos, &yPos);
	x2 = xPos, y2 = yPos;
	//取和的分量，无须取平方分量
	double nDx = x1 - x2, nDy = y1 - y2;
	double nD = fabs(nDx) + fabs(nDy);
	if (nD == 0)
		return FALSE;
	x1 += FLEE_SPACE * nDx / nD;
	y1 += FLEE_SPACE * nDy / nD;
	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].ClientGotoPos((INT)x1, (INT)y1);
	return TRUE;
}

// 拾取最近的一件物品
BOOL AutoAi::PickAroundItem() 
{
	INT nItemIndex = -1;
	INT nPlayerNpcIdx = Player[CLIENT_PLAYER_INDEX].m_nIndex;
	//不要每次都索引
	INT nPlayerX = Npc[nPlayerNpcIdx].m_MapX, nPlayerY = Npc[nPlayerNpcIdx].m_MapY, 
		nPlayerRegion = Npc[nPlayerNpcIdx].m_RegionIndex;
	if (nPlayerRegion < 0 ||  Npc[nPlayerNpcIdx].m_RegionIndex < 0)
		return 0;
	INT nSubWorld = Npc[nPlayerNpcIdx].m_SubWorldIndex;
	INT nxRange = PLAYER_PICKUP_CLIENT_DISTANCE / SubWorld[nSubWorld].m_nCellWidth + 1;
	INT nyRange = PLAYER_PICKUP_CLIENT_DISTANCE / SubWorld[nSubWorld].m_nCellHeight + 1;
	INT nRangeDis = ((nxRange + nyRange)<<2) + abs(nxRange - nyRange);
	KRegion::VEC_LIST::iterator it;
	INT nStep = 0;
	//从中间region开始
	INT nRegion = nPlayerRegion;	//搜索是以Player为中心搜索
	INT nXOffset = 0, nYOffset = 0;	//X和Y的偏移(切换region时用)
_tagPlayerFind1:
	it = SubWorld[nSubWorld].m_Region[nRegion].m_ObjList.begin();
	for(; it != SubWorld[nSubWorld].m_Region[nRegion].m_ObjList.end(); ++it)
	{
		INT nItem = *it;
		if (nItem >= 0 && Object[nItem].m_nKind == Obj_Kind_Item || Object[nItem].m_nKind == Obj_Kind_Money)
		{
			//对比到Player间距
			INT xDis = abs(Object[nItem].m_nMapX - nPlayerX - nXOffset);
			INT yDis = abs(Object[nItem].m_nMapY - nPlayerY - nYOffset);

			INT nDis = ((xDis + yDis) << 2) + abs(xDis - yDis);
			if (nDis < nRangeDis)	//如比先前的小
			{
				nRangeDis = nDis;
				nItemIndex = nItem;
			}
		}
	}
_tagPlayerFind2:
	switch(nStep++)		//此循环搜索周围八个region
	{
	case 0:	//左
		if (nPlayerX - nxRange < 0)
		{
			nXOffset = SubWorld[nSubWorld].m_nRegionWidth;	//坐标右移
			nRegion = SubWorld[nSubWorld].m_Region[nPlayerRegion].m_nConnectRegion[2];
			if (nRegion >= 0)
				goto _tagPlayerFind1;
		}
		goto _tagPlayerFind2;
	case 1: //右
		if (nPlayerX + nxRange > SubWorld[nSubWorld].m_nRegionWidth)
		{
			nXOffset = -SubWorld[nSubWorld].m_nRegionWidth; //坐标左移
			nRegion = SubWorld[nSubWorld].m_Region[nPlayerRegion].m_nConnectRegion[6];
			if (nRegion >= 0)
				goto _tagPlayerFind1;
		}
		goto _tagPlayerFind2;
	case 2:	//上
		if (nPlayerY - nyRange < 0)
		{
			nXOffset = 0;
			nYOffset = SubWorld[nSubWorld].m_nRegionHeight; //坐标下移
			nRegion = SubWorld[nSubWorld].m_Region[nPlayerRegion].m_nConnectRegion[4];
			if (nRegion >= 0)
				goto _tagPlayerFind1;
		}
		goto _tagPlayerFind2;
	case 3: //下
		if (nPlayerY + nyRange > SubWorld[nSubWorld].m_nRegionHeight)
		{
			nXOffset = 0;
			nYOffset = -SubWorld[nSubWorld].m_nRegionHeight; //坐标上移
			nRegion = SubWorld[nSubWorld].m_Region[nPlayerRegion].m_nConnectRegion[0];
			if (nRegion >= 0)
				goto _tagPlayerFind1;
		}
		goto _tagPlayerFind2;
	case 4: //左上(近似)
		if (((nPlayerX + nPlayerY + 2)<<2) + abs(nPlayerX-nPlayerY) < nRangeDis) //左上region右下角的点到原点间距在范围内？
		{
			nXOffset = SubWorld[nSubWorld].m_nRegionWidth;	//坐标右移
			nYOffset = SubWorld[nSubWorld].m_nRegionHeight; //坐标下移
			nRegion = SubWorld[nSubWorld].m_Region[nPlayerRegion].m_nConnectRegion[3];
			if (nRegion >= 0)
				goto _tagPlayerFind1;
		}
		goto _tagPlayerFind2;
	case 5: //左下(近似)
		if (((nPlayerX + SubWorld[nSubWorld].m_nRegionHeight - nPlayerY + 2)<<2) + 
			abs(nPlayerX - SubWorld[nSubWorld].m_nRegionHeight + nPlayerY) < nRangeDis) //左下region右上角的点到原点间距...
		{
			nXOffset = SubWorld[nSubWorld].m_nRegionWidth;	//坐标右移
			nYOffset = -SubWorld[nSubWorld].m_nRegionHeight; //坐标上移
			nRegion = SubWorld[nSubWorld].m_Region[nPlayerRegion].m_nConnectRegion[1];
			if (nRegion >= 0)
				goto _tagPlayerFind1;
		}
		goto _tagPlayerFind2;
	case 6: //右上(近似)
		if (((SubWorld[nSubWorld].m_nRegionWidth - nPlayerX + nPlayerY + 2)<<2) + 
			abs(nPlayerX - SubWorld[nSubWorld].m_nRegionWidth + nPlayerY) < nRangeDis) //右上region左下角的点到原点间距...
		{
			nXOffset = -SubWorld[nSubWorld].m_nRegionWidth; //坐标左移
			nYOffset = +SubWorld[nSubWorld].m_nRegionHeight; //坐标下移
			nRegion = SubWorld[nSubWorld].m_Region[nPlayerRegion].m_nConnectRegion[5];
			if (nRegion >= 0)
				goto _tagPlayerFind1;
		}
		goto _tagPlayerFind2;
	case 7: //右下(近似)
		if (((SubWorld[nSubWorld].m_nRegionWidth - nPlayerX + SubWorld[nSubWorld].m_nRegionHeight - nPlayerY + 2)<<2) + 
			abs(nPlayerX - nPlayerY - SubWorld[nSubWorld].m_nRegionWidth + SubWorld[nSubWorld].m_nRegionHeight) < nRangeDis) //...
		{
			nXOffset = -SubWorld[nSubWorld].m_nRegionWidth; //坐标左移
			nYOffset = -SubWorld[nSubWorld].m_nRegionHeight; //坐标上移
			nRegion = SubWorld[nSubWorld].m_Region[nPlayerRegion].m_nConnectRegion[7];
			if (nRegion >= 0)
				goto _tagPlayerFind1;
		}
		goto _tagPlayerFind2;		
	default:
		break;
	}
	if (nItemIndex > 0)
		Player[CLIENT_PLAYER_INDEX].m_cPlayerItem.PickUpObj(nItemIndex);
	return nItemIndex > 0;
}

#endif // #ifndef _SERVER

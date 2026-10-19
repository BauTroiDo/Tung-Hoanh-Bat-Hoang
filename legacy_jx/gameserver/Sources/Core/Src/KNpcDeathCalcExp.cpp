//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KNpcDeathCalcExp.cpp
// Date:	2003.07.21
// Code:	边城浪子
// Desc:	KNpcDeathCalcExp Class   经验分配
//---------------------------------------------------------------------------

#include	"KCore.h"
#include	"KNpc.h"
#include	"KPlayerDef.h"
#include	"KPlayer.h"
#include	"KNpcDeathCalcExp.h"

void	KNpcDeathCalcExp::Init(int nNpcIdx)
{
	m_nNpcIdx = (nNpcIdx > 0 ? nNpcIdx : 0);
	memset(m_sCalcInfo, 0, sizeof(m_sCalcInfo));
	ZeroMemory(&m_sCalcInfo,sizeof(m_sCalcInfo));
}

void	KNpcDeathCalcExp::Active()
{
	//if (Npc[m_nNpcIdx].m_Kind != kind_normal)
	//	return;

	//for (int i = 0; i < defMAX_CALC_EXP_NUM; i++)
	//{
	//	if (m_sCalcInfo[i].m_nAttackIdx <= 0 || m_sCalcInfo[i].m_nAttackIdx>=MAX_PLAYER)
	//		continue;

	//	int nDistance = KNpcSet::GetDistanceSquare(m_nNpcIdx, Player[m_sCalcInfo[i].m_nAttackIdx].m_nIndex);
	//	int nSerDistance = 400;
	//	//g_GameSetTing.GetInteger("SkillsIcon","ExpDisDistance",400,&nSerDistance);
	//	nSerDistance = _gameGlobalSet.SubWorldInfo.nSerDistance;

	//	if  (nDistance>nSerDistance)
	//	{//如果两者的距离范围 大于 800 马上清零
	//		m_sCalcInfo[i].m_nTime = 0;
	//		m_sCalcInfo[i].m_nAttackIdx = 0;     //经验所属者
	//		m_sCalcInfo[i].m_nTotalDamage = 1;   //经验消失
	//		continue;
	//	}

	//	if  (m_sCalcInfo[i].m_nTime>0)
	//	    m_sCalcInfo[i].m_nTime--;               //保留的时间

	//	if (m_sCalcInfo[i].m_nTime <= 0)
	//	{
	//		m_sCalcInfo[i].m_nTime = 0;
	//		m_sCalcInfo[i].m_nAttackIdx = 0;     //经验所属者
	//		m_sCalcInfo[i].m_nTotalDamage = 1;   //经验消失
	//	}
	//}

	if (Npc[m_nNpcIdx].m_Kind != kind_normal)
		return;

	for (int i = 0; i < defMAX_CALC_EXP_NUM; i++)
	{
		if (m_sCalcInfo[i].m_nAttackIdx <= 0)
			continue;

		m_sCalcInfo[i].m_nTime--;
		if (m_sCalcInfo[i].m_nTime <= 0)
		{
			m_sCalcInfo[i].m_nTime = 0;
			m_sCalcInfo[i].m_nAttackIdx = 0;
			m_sCalcInfo[i].m_nTotalDamage = 1;
		}
	}


}

//增加经验
#ifdef _SERVER
void	KNpcDeathCalcExp::AddDamage(int nPlayerIdx, int nDamage)
{
	if (Npc[m_nNpcIdx].m_Kind != kind_normal)  //只有才有能增加经验
		return;

	if (nPlayerIdx <= 0 || nPlayerIdx >= MAX_PLAYER || Player[nPlayerIdx].m_nIndex <= 0)
		return;
	if (Player[nPlayerIdx].m_cTeam.m_nFlag)
	{//队长 如果队长不在了 就没有经验了！
		nPlayerIdx = g_Team[Player[nPlayerIdx].m_cTeam.m_nID].m_nCaptain;
		if (nPlayerIdx <= 0 || nPlayerIdx >= MAX_PLAYER || Player[nPlayerIdx].m_nIndex <= 0)
			return;
	}
	int		i;
	for (i = 0; i < defMAX_CALC_EXP_NUM; i++)  //最大分配经验的数量限制？
	{
		if (m_sCalcInfo[i].m_nAttackIdx == nPlayerIdx)
		{//是同一个人打的 就累积伤害 经验
			this->m_sCalcInfo[i].m_nTotalDamage += nDamage;     //累积当前伤害

			if (this->m_sCalcInfo[i].m_nTotalDamage<=0)
				this->m_sCalcInfo[i].m_nTotalDamage =1;

			this->m_sCalcInfo[i].m_nTime = defMAX_CALC_EXP_TIME;//经验消除时间
			return;
		}
	}

	for (i = 0; i < defMAX_CALC_EXP_NUM; i++)
	{
		if (m_sCalcInfo[i].m_nAttackIdx == 0)
			break;
	}
    //如果没有空位就返回 没有经验 不设置了
	if (i >= defMAX_CALC_EXP_NUM)  //保证有三次的攻击者是相同的
		return;

	m_sCalcInfo[i].m_nAttackIdx   = nPlayerIdx;
	m_sCalcInfo[i].m_nTotalDamage = nDamage;

	if (m_sCalcInfo[i].m_nTotalDamage<=0)
       m_sCalcInfo[i].m_nTotalDamage =1;

	m_sCalcInfo[i].m_nTime        = defMAX_CALC_EXP_TIME;   //经验消除时间
}
//分配经验   经验分配
int		KNpcDeathCalcExp::CalcExp()
{
	if (Npc[m_nNpcIdx].m_CurrentLifeMax <= 0)
		return 0;

	int i, j, nDamage = 0, nMaxPlayer = 0;

    if (Npc[m_nNpcIdx].m_Experience<=0 && Npc[m_nNpcIdx].m_Kind!=kind_player)
	   Npc[m_nNpcIdx].m_Experience=1; 
	
	int  nExpSale=1;

    // g_GameSetTing.GetInteger("ServerConfig","ExpRate",1,&nExpSale);
	nExpSale = _gameGlobalSet.SubWorldInfo.nExpSale;

	for (i = 0; i < defMAX_CALC_EXP_NUM; i++)
	{
		if (m_sCalcInfo[i].m_nAttackIdx <= 0)
			continue;

		// 如果组队 
		if (Player[m_sCalcInfo[i].m_nAttackIdx].m_cTeam.m_nFlag && Player[m_sCalcInfo[i].m_nAttackIdx].m_cTeam.m_nID >= 0)
		{
			int	nTeam, nPlayer, nDistance, nMinDistance;

			nPlayer = 0;
			nMinDistance = PLAYER_SHARE_EXP_DISTANCE * PLAYER_SHARE_EXP_DISTANCE;
			nTeam = Player[m_sCalcInfo[i].m_nAttackIdx].m_cTeam.m_nID;
			//队长
			nDistance = KNpcSet::GetDistanceSquare(m_nNpcIdx, Player[g_Team[nTeam].m_nCaptain].m_nIndex);

			if (nDistance >= 0 && nDistance < nMinDistance)
			{
				nMinDistance = nDistance;
				nPlayer = g_Team[nTeam].m_nCaptain;
			}
			//队员
			for (j = 0; j < MAX_TEAM_MEMBER; j++)
			{
				if (g_Team[nTeam].m_nMember[j] <= 0)
					continue;

				nDistance = KNpcSet::GetDistanceSquare(m_nNpcIdx, Player[g_Team[nTeam].m_nMember[j]].m_nIndex);
				if (nDistance >= 0 && nDistance < nMinDistance)
				{
					nMinDistance = nDistance;
					nPlayer = g_Team[nTeam].m_nMember[j];
				}
			}

			if (nPlayer > 0)
			{
				if (m_sCalcInfo[i].m_nTotalDamage > nDamage)
				{//得到伤害最大的那个人
					nDamage = m_sCalcInfo[i].m_nTotalDamage;
					nMaxPlayer = nPlayer;
				}
				//玩家增加经验
                if (m_sCalcInfo[i].m_nTotalDamage<=0)
                   m_sCalcInfo[i].m_nTotalDamage=1;
             //if (Npc[Player[nPlayer].m_nIndex].m_Level<200)
				if (Npc[m_nNpcIdx].IsJinYan<=0)
				   Npc[m_nNpcIdx].IsJinYan=1;

				if(SubWorld[Npc[m_nNpcIdx].m_SubWorldIndex].m_nNoDropItemKind!=NO_DROP_EXP)
				   Player[nPlayer].AddExp(Npc[m_nNpcIdx].m_Experience*Npc[m_nNpcIdx].IsJinYan*nExpSale, Npc[m_nNpcIdx].m_Level,Npc[m_nNpcIdx].m_cGold.GetGoldType());
			}
		}
		// 如果没有组队
		else
		{
			int nDistance = KNpcSet::GetDistanceSquare(m_nNpcIdx, Player[m_sCalcInfo[i].m_nAttackIdx].m_nIndex);
			if (nDistance >= PLAYER_SHARE_EXP_DISTANCE * PLAYER_SHARE_EXP_DISTANCE)
				continue;

			if (m_sCalcInfo[i].m_nTotalDamage > nDamage)
			{//三次 取最大那次的经验
				nDamage = m_sCalcInfo[i].m_nTotalDamage;
				nMaxPlayer = m_sCalcInfo[i].m_nAttackIdx;
			}

			if (m_sCalcInfo[i].m_nTotalDamage<=0)
                m_sCalcInfo[i].m_nTotalDamage=1;
			if (Npc[m_nNpcIdx].IsJinYan<=0)
				Npc[m_nNpcIdx].IsJinYan=1;

		//	(m_sCalcInfo[i].m_nTotalDamage+Npc[m_nNpcIdx].m_CurrentLifeMax)* Npc[m_nNpcIdx].m_Experience*nJinYan*Npc[m_nNpcIdx].IsJinYan*nExpSale/(Npc[Player[m_sCalcInfo[i].m_nAttackIdx].m_nIndex].m_Level*Npc[m_nNpcIdx].m_Level)
	    //  if (Npc[Player[m_sCalcInfo[i].m_nAttackIdx].m_nIndex].m_Level<200)

		  if (SubWorld[Npc[m_nNpcIdx].m_SubWorldIndex].m_nNoDropItemKind!=NO_DROP_EXP)
			Player[m_sCalcInfo[i].m_nAttackIdx].AddExp(Npc[m_nNpcIdx].m_Experience*Npc[m_nNpcIdx].IsJinYan*nExpSale,Npc[m_nNpcIdx].m_Level,Npc[m_nNpcIdx].m_cGold.GetGoldType());//Npc[m_nNpcIdx].m_CurrentLifeMax
		}
	}//endif

	Clear();   //打死后才清空

	return nMaxPlayer;
}

#endif

void KNpcDeathCalcExp::Clear()
{
	memset(m_sCalcInfo, 0, sizeof(m_sCalcInfo));
	ZeroMemory(&m_sCalcInfo,sizeof(m_sCalcInfo));
}




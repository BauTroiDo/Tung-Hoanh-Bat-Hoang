#include "KEngine.h"
#include "KCore.h"
#include "SkillDef.h"
#include "KSkillList.h"
#ifndef _STANDALONE
#include "../../../lib/S3DBInterface.h"
#include "crtdbg.h"
#else
#include "S3DBInterface.h"
#endif
#include "KSkills.h"
#include "KPlayer.h"

#ifndef _SERVER
#include "cOREsHELL.H"
#endif

KSkillList::KSkillList()
{
	ZeroMemory(m_Skills, sizeof(NPCSKILL) * MAX_NPCSKILL);
	m_nNpcIndex = 0;
}

KSkillList::~KSkillList()
{
}


//static int g_CallCount = 0;

int	KSkillList::FindSame(int nSkillID)
{
//    g_CallCount++;

	if (!nSkillID)
		return 0;

	if (nSkillID>g_SkillManager.GetSkillCount())
		return 0;

	for (int i=1; i<MAX_NPCSKILL; ++i)
	{
		if (m_Skills[i].SkillId == nSkillID)
		{
			return i;
		}
	}
	return 0;	
}

int KSkillList::FindFree()
{
	for (int i = 1; i < MAX_NPCSKILL; ++i)
	{
		if (!m_Skills[i].SkillId)
		{//返回空位置
			return i;
		}
	}
	return 0;	
}
//如果是NPC被动技能 即立即使用技能
#ifdef _SERVER
BOOL KSkillList::SetNpcSkill(int nSkillNo, int nSkillId, int nSkillLevel)
{
	if (nSkillNo <= 0 || nSkillLevel <= 0 || nSkillId <= 0)
		return FALSE;

	if (nSkillNo >= MAX_NPCSKILL) 
		return FALSE;

	if (nSkillId>g_SkillManager.GetSkillCount())
		return FALSE;

	
	m_Skills[nSkillNo].SkillId           = nSkillId;
	m_Skills[nSkillNo].CurrentSkillLevel = nSkillLevel;
	m_Skills[nSkillNo].SkillLevel        = nSkillLevel;
	m_Skills[nSkillNo].MaxTimes          = 0;
	m_Skills[nSkillNo].RemainTimes       = 0;

	m_Skills[nSkillNo].nSkillStyle       = -1;
	m_Skills[nSkillNo].NextCastTime      = 180;
	m_Skills[nSkillNo].NextHorseCastTime = 180;
	m_Skills[nSkillNo].nEquiptLimited    = -2;

//	_ASSERT(nSkillId > 0 && nSkillLevel > 0);

	KSkill * pOrdinSkill = (KSkill *) g_SkillManager.GetSkill(nSkillId, nSkillLevel);
	if (!pOrdinSkill) 
        return FALSE;

	m_Skills[nSkillNo].NextCastTime      = pOrdinSkill->GetDelayPerCast();
	m_Skills[nSkillNo].NextHorseCastTime = pOrdinSkill->GetHorsePerCast();
	m_Skills[nSkillNo].nSkillStyle       = pOrdinSkill->GetSkillStyle();
    m_Skills[nSkillNo].nEquiptLimited    = pOrdinSkill->GetEquiptLimited();
    //pOrdinSkill->CanCastSkill(m_nNpcIndex, -1, m_nNpcIndex);//技能限制检测
	//如果是被动技能 即立即使用技能
    if (pOrdinSkill->GetSkillStyle() == SKILL_SS_PassivityNpcState)
	{
		pOrdinSkill->Cast(m_nNpcIndex, -1, m_nNpcIndex,-1);
	}

	ReEnChance();

	if (pOrdinSkill->IsAura())
	{
		return TRUE;
	}

	return FALSE;
}

void KSkillList::RemoveNpcSkill(int nSkillNo)
{
	if (nSkillNo <= 0 )
		return ;
	if (nSkillNo >= MAX_NPCSKILL) 
		return ;
	
	m_Skills[nSkillNo].SkillId = 0;
	m_Skills[nSkillNo].CurrentSkillLevel = 0;
	m_Skills[nSkillNo].SkillLevel = 0;
	m_Skills[nSkillNo].MaxTimes = 0;
	m_Skills[nSkillNo].RemainTimes = 0;
	m_Skills[nSkillNo].NextCastTime      = 0;
	m_Skills[nSkillNo].NextHorseCastTime = 0;
	m_Skills[nSkillNo].nSkillStyle       = -1;
    m_Skills[nSkillNo].nEquiptLimited    = 0;
	m_Skills[nSkillNo].nExpSkill = 0;
	m_Skills[nSkillNo].nCurSkillExp = 0;
	m_Skills[nSkillNo].nExpSkillPoint=0;
}

#endif

int KSkillList::GetCount()
{
	int nCount = 0;
	
	for (int i = 1; i < MAX_NPCSKILL; ++i)
	{
		if (m_Skills[i].SkillId)
		{
			nCount++;
		}
	}
	return nCount;
}

#ifndef _SERVER
void KSkillList::SetSkillLevel(int nId/*技能的ID号*/, int nLevel)
{
	int i = FindSame(nId);
	
	if (!i)
	{
		Add(nId, nLevel);
		return;		
	}
	else
		//m_Skills[i].SkillLevel = nLevel;
		SetLevel(i, nLevel);
}
#endif

#ifndef _SERVER
BOOL KSkillList::SetLevel(int nIndex/*技能列表的索引号*/, int nLevel)
{
	if (nIndex <= 0 || nIndex >= MAX_NPCSKILL)
		return FALSE;
	if (nLevel < 0 || nLevel >= MAX_SKILLLEVEL)
		return FALSE;
	
	int nInc = nLevel - m_Skills[nIndex].SkillLevel;

	//if 	(nInc==0)
	IncreaseLevel(nIndex, nInc);
	//else
	//	IncreaseLevel(nIndex, nInc,FALSE,TRUE);

	return TRUE; 
}
#endif
//从脚本中解析最新等级技能的数据  
BOOL KSkillList::IncreaseLevel(int nIdx, int nLvl, int Qeuip,BOOL nIsClearExp)
{
	if (nIdx <= 0 || nIdx >= MAX_NPCSKILL)
		return FALSE;

	if (m_Skills[nIdx].SkillId>g_SkillManager.GetSkillCount())
		return FALSE;

	m_Skills[nIdx].SkillLevel +=nLvl;

	if (m_Skills[nIdx].SkillLevel>=MAX_SKILLLEVEL)
		m_Skills[nIdx].SkillLevel=MAX_SKILLLEVEL-1;

	if (m_Skills[nIdx].SkillLevel<0)
		m_Skills[nIdx].SkillLevel=0;

	m_Skills[nIdx].CurrentSkillLevel +=nLvl;

	if (m_Skills[nIdx].CurrentSkillLevel>=MAX_SKILLLEVEL)
		m_Skills[nIdx].CurrentSkillLevel=MAX_SKILLLEVEL-1;
	
	if (m_Skills[nIdx].CurrentSkillLevel<0)
		m_Skills[nIdx].CurrentSkillLevel=0;

	/*if (nLvl<0 && !Qeuip)  //如果增加的等级是小于0的话 就清除状态  是换装的话
	{//卸载装备，进行数据处理
		KSkill * pCurinSkill = (KSkill *)g_SkillManager.GetSkill(m_Skills[nIdx].SkillId, m_Skills[nIdx].CurrentSkillLevel);
		//从脚本中解析最新等级技能的数据
		if (pCurinSkill) //如果有这个技能的数据
		{
			if (pCurinSkill->GetSkillStyle() == SKILL_SS_PassivityNpcState) //是否立即使用技能
			{//如果是被动技能 清除状态
				
				//pCurinSkill->Cast(m_nNpcIndex, -1, m_nNpcIndex,-1);           //设置被动状态的在NPC上的效果
				Npc[m_nNpcIndex].SetSkillLeftTime(m_Skills[nIdx].SkillId,0);
			}
			pCurinSkill=NULL;
		}
	}
   */
	KSkill * pOrdinSkill = (KSkill *)g_SkillManager.GetSkill(m_Skills[nIdx].SkillId, m_Skills[nIdx].CurrentSkillLevel);
	//从脚本中解析最新等级技能的数据
	if (!pOrdinSkill) //如果没有这个技能的数据
        return FALSE;

    m_Skills[nIdx].NextCastTime      = pOrdinSkill->GetDelayPerCast();
	m_Skills[nIdx].NextHorseCastTime = pOrdinSkill->GetHorsePerCast();
	m_Skills[nIdx].nSkillStyle       = pOrdinSkill->GetSkillStyle();
    m_Skills[nIdx].nEquiptLimited    = pOrdinSkill->GetEquiptLimited();
	m_Skills[nIdx].nIsExpSkill       = pOrdinSkill->GetIsExpSkill();        //是否是熟练度技能
	m_Skills[nIdx].nExpSkill         = pOrdinSkill->GetSkillExp();          //该位置的总熟练度
    if (nIsClearExp)	
	   m_Skills[nIdx].nCurSkillExp      = 0;       //当前熟练度 存档

    int nReg=0;
        nReg = pOrdinSkill->NewCanCastSkill(m_nNpcIndex,-1,m_nNpcIndex);       //技能限制检测

	if (pOrdinSkill->GetSkillStyle() == SKILL_SS_PassivityNpcState && nReg==1 && !Qeuip) //是否立即使用技能 不是换装的就 执行技能
	{//如果是被动技能并且与当前武器相符合的 立即使用技能
		pOrdinSkill->Cast(m_nNpcIndex, -1, m_nNpcIndex,-1); //设置被动状态的在NPC上的效果
	}
	//原来没有的
	else if (pOrdinSkill->GetSkillStyle() == SKILL_SS_PassivityNpcState && nReg <= 0 && !Qeuip)
	{//与当前武器不符合的被动状态 被清空
         pOrdinSkill->Cast(m_nNpcIndex, -1, m_nNpcIndex,1);
	}

	pOrdinSkill=NULL;

	ReEnChance(); //重新设置加成效果

	return TRUE;
}
//train ky nang skills
BOOL KSkillList::IncreaseExp(int nIdx, int nExp)
{
	if (nIdx <= 0 || nIdx >= MAX_NPCSKILL)
		return FALSE;

	if (m_Skills[nIdx].SkillLevel <= 0)
		return FALSE;

	if (m_Skills[nIdx].nExpSkill <= 0)//exp yeu cau
		return FALSE;

	m_Skills[nIdx].nCurSkillExp += nExp;
	//char szTemp[128];
	//sprintf(szTemp, "Skill ID: %d | Level: %d | EXP: %d/%d ", nIdx, m_Skills[nIdx].SkillLevel, m_Skills[nIdx].nCurSkillExp, m_Skills[nIdx].nExpSkill);
	//printf("%s\n", szTemp);

	if (m_Skills[nIdx].nCurSkillExp >= m_Skills[nIdx].nExpSkill)
	{
		return IncreaseLevel(nIdx, 1,0,1);
	}
	return FALSE;
}


int	KSkillList::SetSkillLevelDirectlyUsingId(unsigned long ulSkillId, unsigned long ulLevel)/*请慎用该函数，因为目前未加任何被动技能降级对数值的影响*/
{
	if (ulSkillId >= MAX_SKILL || ulSkillId == 0 ||  ulLevel >= MAX_SKILLLEVEL)
		return 0;

	if (ulSkillId>g_SkillManager.GetSkillCount())
		return FALSE;

	unsigned long  ulSkillIdx = FindSame(ulSkillId);
	if (ulSkillIdx)
	{
		if (m_Skills[ulSkillIdx].SkillId)
		{
			m_Skills[ulSkillIdx].SkillLevel = ulLevel;
			m_Skills[ulSkillIdx].CurrentSkillLevel = ulLevel;
			
			ISkill * pSkill =  g_SkillManager.GetSkill(m_Skills[ulSkillIdx].SkillId, m_Skills[ulSkillIdx].CurrentSkillLevel);
			if (!pSkill) 
				return 0 ;
			
			m_Skills[ulSkillIdx].NextCastTime      = pSkill->GetDelayPerCast();
			m_Skills[ulSkillIdx].NextHorseCastTime = pSkill->GetHorsePerCast();
			m_Skills[ulSkillIdx].nSkillStyle       = pSkill->GetSkillStyle();
			m_Skills[ulSkillIdx].nEquiptLimited    = pSkill->GetEquiptLimited();
			
			if (pSkill->GetSkillStyle() == SKILL_SS_PassivityNpcState)
			{
				((KSkill*)pSkill)->Cast(m_nNpcIndex, -1, m_nNpcIndex,-1);
			} 
			ReEnChance();
			return 1;
		}	
	}
	return 0;
	}

int	KSkillList::SetSkillLevelDirectlyUsingIndex(unsigned long ulSkillIdx, unsigned long ulLevel)/*请慎用该函数，因为目前未加任何被动技能降级对数值的影响*/
{
	if (ulSkillIdx >= MAX_NPCSKILL ||  ulSkillIdx == 0 || ulLevel >= MAX_SKILLLEVEL)
		return 0;
	
	if (m_Skills[ulSkillIdx].SkillId>g_SkillManager.GetSkillCount())
		return 0;

	if (m_Skills[ulSkillIdx].SkillId)
	{
		m_Skills[ulSkillIdx].SkillLevel = ulLevel;
		m_Skills[ulSkillIdx].CurrentSkillLevel = ulLevel;
		
		ISkill * pSkill =  g_SkillManager.GetSkill(m_Skills[ulSkillIdx].SkillId, m_Skills[ulSkillIdx].CurrentSkillLevel);
		if (!pSkill) 
			return 0 ;
		
		m_Skills[ulSkillIdx].NextCastTime      = pSkill->GetDelayPerCast();
		m_Skills[ulSkillIdx].NextHorseCastTime = pSkill->GetHorsePerCast();
		m_Skills[ulSkillIdx].nSkillStyle       = pSkill->GetSkillStyle();
		m_Skills[ulSkillIdx].nEquiptLimited    = pSkill->GetEquiptLimited();
		
		if (pSkill->GetSkillStyle() == SKILL_SS_PassivityNpcState)
		{
			((KSkill*)pSkill)->Cast(m_nNpcIndex, -1, m_nNpcIndex,-1);
		}

		ReEnChance();
		
		return 1;
	}	
	return 0;
}

int KSkillList::Remove(int nSkillID)
{
	if (nSkillID <= 0 || nSkillID >= MAX_SKILL)  //技能数量限制
		return FALSE;
	int i = FindSame(nSkillID);
	if (i)
	{
		m_Skills[i].CurrentSkillLevel = 0;
		m_Skills[i].SkillId = 0;
		m_Skills[i].SkillLevel = 0;
		m_Skills[i].mAddPoint = 0;
		m_Skills[i].EnChance = 0;   //后来添加的
		m_Skills[i].nTempEnChance = 0;
		m_Skills[i].nSkillStyle = -1;
		m_Skills[i].NextCastTime = 0;
		m_Skills[i].NextHorseCastTime = 0;
		m_Skills[i].nEquiptLimited = 0;
		m_Skills[i].nExpSkill=0;     //当前熟练度经验
		m_Skills[i].nCurSkillExp=0;
		m_Skills[i].nExpSkillPoint=0;
		return i;

	}
	return FALSE;
	
}
//移除全部技能
void KSkillList::RemoveAllSkill()
{
//	int i = FindSame(nSkillID);
	for (int i=0;i<MAX_NPCSKILL;++i)
	{
      if (m_Skills[i].SkillId>0 &&m_Skills[i].SkillId!=1 &&m_Skills[i].SkillId!=2 && m_Skills[i].SkillId!=53 &&m_Skills[i].SkillId!=400&&m_Skills[i].SkillId!=210)  //!CheckNoSkill("NoCanXiSui",m_Skills[i].SkillId)
	  {
		   m_Skills[i].CurrentSkillLevel = 0;
		   m_Skills[i].SkillId = 0;
		   m_Skills[i].SkillLevel = 0;
		   m_Skills[i].mAddPoint = 0;
		   m_Skills[i].EnChance = 0;   //后来添加的
		   m_Skills[i].nTempEnChance = 0;
		   m_Skills[i].nSkillStyle = -1;
		   m_Skills[i].NextCastTime = 0;
		   m_Skills[i].NextHorseCastTime = 0;
		   m_Skills[i].nEquiptLimited = 0;
		   m_Skills[i].nExpSkill=0;
		   m_Skills[i].nCurSkillExp=0;
		   m_Skills[i].nExpSkillPoint=0;
	  }
	}	
}

//上线添加技能 和 使用技能调用的函数   //客户端  服务器端 共调用
int KSkillList::Add(int nSkillID, int nLevel, int nMaxTimes, int RemainTimes, int nAdd,int nCurExpSkill,int nExpSkillPonit)
{
	int i;	
	if (nSkillID <= 0 || nLevel < 0)
		return 0;

	if (nSkillID>g_SkillManager.GetSkillCount())
		return 0;

	if (nLevel>=MAX_SKILLLEVEL)
		nLevel=	MAX_SKILLLEVEL-1;
	int nTempLevel=0;
//--------------------已经有这个技能-------------------------
	i = FindSame(nSkillID);

	if (i)
	{//如果有找到这个技能的列表位置--已经有这个技能了
		m_Skills[i].SkillId = nSkillID;
		if (nLevel > m_Skills[i].SkillLevel)
		{//等级大于原来的等级
			int nInc = nLevel - m_Skills[i].SkillLevel;	 //额外的技能点
			IncreaseLevel(i,nInc,FALSE,TRUE);//重新加载技能位置信息
		}
		else
		{
			if (nSkillID==1 || nSkillID==2 || nSkillID==53)
			{
		        m_Skills[i].SkillLevel = 1;
		        m_Skills[i].CurrentSkillLevel = 1;
			}
			else
			{
				m_Skills[i].SkillLevel = nLevel;
		        m_Skills[i].CurrentSkillLevel = nLevel;
			}
		}
		
		m_Skills[i].MaxTimes = 0;
		m_Skills[i].RemainTimes = 0;
		m_Skills[i].mAddPoint = nAdd;
		m_Skills[i].EnChance = 0;
		m_Skills[i].nTempEnChance = 0;
        m_Skills[i].nCurSkillExp=0;
		m_Skills[i].nExpSkillPoint=0;
		
		nTempLevel = m_Skills[i].CurrentSkillLevel;

        if  (nTempLevel<=0)
			nTempLevel = 1;

		if (nTempLevel>0)
		{
			KSkill * pSkill =(KSkill *) g_SkillManager.GetSkill(nSkillID,nTempLevel); //m_Skills[i].CurrentSkillLevel
			
			if (!pSkill) 
				return 0;

			m_Skills[i].NextCastTime      = pSkill->GetDelayPerCast();
			m_Skills[i].NextHorseCastTime = pSkill->GetHorsePerCast();
			m_Skills[i].nSkillStyle       = pSkill->GetSkillStyle();
            m_Skills[i].nEquiptLimited    = pSkill->GetEquiptLimited();

			if (m_Skills[i].CurrentSkillLevel<=0)
				m_Skills[i].nExpSkill         = 0;
			else
                m_Skills[i].nExpSkill         = pSkill->GetSkillExp();

			m_Skills[i].nIsExpSkill       = pSkill->GetIsExpSkill();  //是否 熟练度技能
			m_Skills[i].nCurSkillExp      = nCurExpSkill;
			m_Skills[i].nExpSkillPoint    = nExpSkillPonit; //练起来的 技能点

			//如果该技能属于被动辅助技能时，则设置Npc状态
			if (m_Skills[i].CurrentSkillLevel>0)
			{
			  int nReg=0;
                  nReg = pSkill->NewCanCastSkill(m_nNpcIndex, -1, m_nNpcIndex);
			  if (pSkill->GetSkillStyle() == SKILL_SS_PassivityNpcState && nReg==1)
			  {//带武器 和 无武器限制的 执行状态
				  pSkill->Cast(m_nNpcIndex, -1, m_nNpcIndex,-1);
			  }
			  else if (pSkill->GetSkillStyle() == SKILL_SS_PassivityNpcState && nReg<=0)
			  {//清除 与当前武器不相符的被动技能状态效果
                  pSkill->Cast(m_nNpcIndex, -1, m_nNpcIndex,1);
			  }
			  ReEnChance();
			}

			pSkill=NULL;
		}
		return i;
	}
//----------------------新添加技能---------------------------------
	i = FindFree();
	if (i)
	{//另外查找新位置并设置该位置的技能信息
		m_Skills[i].SkillId = nSkillID;
		if (nSkillID==1 || nSkillID==2 || nSkillID==53)
		{
			m_Skills[i].SkillLevel = 1;
			m_Skills[i].CurrentSkillLevel = 1;
		}
		else
		{
			m_Skills[i].SkillLevel = nLevel;
			m_Skills[i].CurrentSkillLevel = nLevel;
		}
		m_Skills[i].MaxTimes = 0;
		m_Skills[i].RemainTimes = 0;
		m_Skills[i].mAddPoint = nAdd;
		m_Skills[i].EnChance = 0;
		m_Skills[i].nTempEnChance = 0;
		m_Skills[i].nSkillStyle = -1;
		m_Skills[i].NextCastTime = 180;
        m_Skills[i].NextHorseCastTime = 180;
		m_Skills[i].nEquiptLimited = -2;
		m_Skills[i].nCurSkillExp=0;
		m_Skills[i].nExpSkillPoint=0; 

		nTempLevel = m_Skills[i].CurrentSkillLevel;

		if  (nTempLevel<=0)
			nTempLevel = 1;

		if (nTempLevel > 0)
		{
			KSkill * pSkill = (KSkill*) g_SkillManager.GetSkill(nSkillID, nTempLevel); //nLevel
			
			if (!pSkill) 
				return 0;

		    m_Skills[i].NextCastTime      = pSkill->GetDelayPerCast();
			m_Skills[i].NextHorseCastTime = pSkill->GetHorsePerCast();
			m_Skills[i].nSkillStyle       = pSkill->GetSkillStyle();
            m_Skills[i].nEquiptLimited    = pSkill->GetEquiptLimited();
			if (m_Skills[i].CurrentSkillLevel<=0)
				m_Skills[i].nExpSkill         = 0;
			else
			    m_Skills[i].nExpSkill     = pSkill->GetSkillExp();

			m_Skills[i].nIsExpSkill       = pSkill->GetIsExpSkill();
			m_Skills[i].nCurSkillExp      = nCurExpSkill;
			m_Skills[i].nExpSkillPoint    = nExpSkillPonit; //练起来的 技能点

           if (m_Skills[i].CurrentSkillLevel>0)
		   {
              int nReg=0;
                  nReg = pSkill->NewCanCastSkill(m_nNpcIndex, -1, m_nNpcIndex);
			  if (pSkill->GetSkillStyle() == SKILL_SS_PassivityNpcState && nReg==1)
			  {
				  pSkill->Cast(m_nNpcIndex, -1, m_nNpcIndex,-1);
			  }
			  else if (pSkill->GetSkillStyle() == SKILL_SS_PassivityNpcState && nReg<=0)
			  {//清除与当前武器不相符合的被动状态
				  pSkill->Cast(m_nNpcIndex, -1, m_nNpcIndex,1);
			  }
			  ReEnChance();
		   }
			pSkill=NULL;
		}

		return i;
	}
	
	return 0;
}
//-----------------------------------------
int  KSkillList::AddSkillExpByListidx(int nListidx,int nCurExp,int nType)
{
	if  (nListidx<=0 && nListidx>=MAX_NPCSKILL) return 0;

	if (m_Skills[nListidx].SkillLevel < MAX_SKILLLEVEL)
	{ 
		if (nType==1)
			m_Skills[nListidx].nCurSkillExp +=nCurExp;
		else
			m_Skills[nListidx].nCurSkillExp=nCurExp;

		if (m_Skills[nListidx].nCurSkillExp>=m_Skills[nListidx].nExpSkill)
			m_Skills[nListidx].nCurSkillExp=m_Skills[nListidx].nExpSkill;

		return 1;
	} 

	return 0;

}
int  KSkillList::GetCurSkillExpByListidx(int nListidx)
{
	if  (nListidx<=0 && nListidx>=MAX_NPCSKILL) return 0;

	if (m_Skills[nListidx].SkillLevel < MAX_SKILLLEVEL)
		return m_Skills[nListidx].nCurSkillExp;

	return 0;
}
int  KSkillList::GetSkillExpByListidx(int nListidx)
{
	if  (nListidx<=0 && nListidx>=MAX_NPCSKILL) return 0;

	if (m_Skills[nListidx].SkillLevel < MAX_SKILLLEVEL)	
		return m_Skills[nListidx].nExpSkill;

	return 0;
}
void  KSkillList::SetSkillExpByListidx(int nListidx,int nExp)
{
	if  (nListidx<=0 && nListidx>=MAX_NPCSKILL) return;

	if (m_Skills[nListidx].SkillLevel < MAX_SKILLLEVEL)
		m_Skills[nListidx].nExpSkill=nExp;
	else
		m_Skills[nListidx].nExpSkill=0;

}

int   KSkillList::IsSkillExpByListidx(int nListidx)
{
	if  (nListidx<=0 && nListidx>=MAX_NPCSKILL) return 0;
	if (m_Skills[nListidx].SkillLevel < MAX_SKILLLEVEL)
		return m_Skills[nListidx].nIsExpSkill;
	return 0;
}

int   KSkillList::GetSkillLevelByListidx(int nListidx)
{
	if  (nListidx<=0 && nListidx>=MAX_NPCSKILL) return 0;
	if (m_Skills[nListidx].SkillLevel < MAX_SKILLLEVEL)
		return m_Skills[nListidx].SkillLevel;
	return 0;
}

//-----------------------------------------
//总熟练度
int KSkillList::GetSkillExp(int nSkillID)
{
	int i;
	
	if (!nSkillID)
		return 0;
	
	i = FindSame(nSkillID);
	if (i)
	{
		//_ASSERT(m_Skills[i].SkillLevel < MAX_SKILLLEVEL);
		if (m_Skills[i].SkillLevel < MAX_SKILLLEVEL)	
		    return m_Skills[i].nExpSkill;
	}
	
	return 0;
}
//设置总熟练度
void KSkillList::SetSkillExp(int nSkillID,int nExp)
{
	int i;
	
	if (!nSkillID)
		return;
	
	i = FindSame(nSkillID);
	if (i)
	{
		//_ASSERT(m_Skills[i].SkillLevel < MAX_SKILLLEVEL);
		if (m_Skills[i].SkillLevel < MAX_SKILLLEVEL)
		   m_Skills[i].nExpSkill=nExp;
	}
}

//当前熟练度
int KSkillList::GetCurSkillExp(int nSkillID)
{
	int i;
	
	if (!nSkillID)
		return 0; 
	
	i = FindSame(nSkillID);
	if (i)
	{
		//_ASSERT(m_Skills[i].SkillLevel < MAX_SKILLLEVEL);
		if (m_Skills[i].SkillLevel < MAX_SKILLLEVEL)
		   return m_Skills[i].nCurSkillExp;
	}
	
	return 0;
}

//是否熟练度技能
int KSkillList::IsSkillExp(int nSkillID)
{
	int i;
	
	if (!nSkillID)
		return 0;
	
	i = FindSame(nSkillID);
	if (i)
	{
		//_ASSERT(m_Skills[i].SkillLevel < MAX_SKILLLEVEL);
		if (m_Skills[i].SkillLevel < MAX_SKILLLEVEL)
		    return m_Skills[i].nIsExpSkill;
	}
	
	return 0;
}


int KSkillList::AddSkillExp(int nSkillID,int nCurExp,int nType)
{
	int i;
	
	if (!nSkillID)
		return 0;
	
	i = FindSame(nSkillID);
	if (i)
	{
	//	_ASSERT(m_Skills[i].SkillLevel < MAX_SKILLLEVEL);
	  if (m_Skills[i].SkillLevel < MAX_SKILLLEVEL)
	  { 
        if (nType==1)
            m_Skills[i].nCurSkillExp +=nCurExp;
		else
            m_Skills[i].nCurSkillExp=nCurExp;

		if (m_Skills[i].nCurSkillExp >= m_Skills[i].nExpSkill)
				m_Skills[i].nCurSkillExp = m_Skills[i].nExpSkill;

		return 1;
	  } 
	}
	
	return 0;
}


int KSkillList::GetLevelByListidx(int nListidx)
{

	if (nListidx<=0 && nListidx>=MAX_NPCSKILL) return 0;

		if (m_Skills[nListidx].SkillLevel < MAX_SKILLLEVEL)
			return m_Skills[nListidx].SkillLevel;

	return 0;
}

int KSkillList::GetLevel(int nSkillID)
{
	int i;
	
	if (!nSkillID)
		return 0;
	
	i = FindSame(nSkillID);
	if (i)
	{
		//_ASSERT(m_Skills[i].SkillLevel < MAX_SKILLLEVEL);
		if (m_Skills[i].SkillLevel < MAX_SKILLLEVEL)
		   return m_Skills[i].SkillLevel;
	}

	return 0;
}

void KSkillList::AddExpSkillPonitByListidxx(int nListidx, int nLevel,bool nKind)
{
	if (nListidx<=0 && nListidx>=MAX_NPCSKILL) return;

	if (m_Skills[nListidx].SkillLevel < MAX_SKILLLEVEL)
	{
		if (nKind)
			m_Skills[nListidx].nExpSkillPoint +=nLevel;
		else
			m_Skills[nListidx].nExpSkillPoint =nLevel;

		if (m_Skills[nListidx].nExpSkillPoint<0)
			m_Skills[nListidx].nExpSkillPoint = 0;
	}
}


BOOL KSkillList::SetSLevelByListidx(int nListidx, int nLevel,BOOL nKind)
{
	    if (nListidx<=0 && nListidx>=MAX_NPCSKILL) return FALSE;

		if (m_Skills[nListidx].SkillLevel < MAX_SKILLLEVEL)
		{

			if (nKind)
			{
				m_Skills[nListidx].SkillLevel +=nLevel;
				m_Skills[nListidx].CurrentSkillLevel +=nLevel;
			}
			else
			{
				m_Skills[nListidx].SkillLevel =nLevel;
				m_Skills[nListidx].CurrentSkillLevel =nLevel;
			}

			if (m_Skills[nListidx].SkillLevel<0)
				m_Skills[nListidx].SkillLevel = 0;
			if (m_Skills[nListidx].CurrentSkillLevel<0)
				m_Skills[nListidx].CurrentSkillLevel = 0;

			return TRUE;
	    }

	return FALSE;
}

BOOL KSkillList::SetSLevel(int nSkillID, int nLevel,BOOL nKind)
{
	int i;
	
	if (!nSkillID)
		return FALSE;
	
	i = FindSame(nSkillID);
	if (i)
	{
		//_ASSERT(m_Skills[i].SkillLevel < MAX_SKILLLEVEL);
		if (m_Skills[i].SkillLevel < MAX_SKILLLEVEL)
		{

		  if (nKind)
		  {
		      m_Skills[i].SkillLevel +=nLevel;
		      m_Skills[i].CurrentSkillLevel +=nLevel;
		  }
		  else
		  {
			  m_Skills[i].SkillLevel =nLevel;
		      m_Skills[i].CurrentSkillLevel =nLevel;
		  }

		   if (m_Skills[i].SkillLevel<0)
			   m_Skills[i].SkillLevel = 0;
		   if (m_Skills[i].CurrentSkillLevel<0)
			   m_Skills[i].CurrentSkillLevel = 0;

		   return TRUE;
		}
	}
	
	return FALSE;
}

int KSkillList::GetCurrentLevel(int nSkillID)
{
	int i;
	
	if (!nSkillID)
		return 0;
	
	i = FindSame(nSkillID);

	if (i)
	{
		return m_Skills[i].CurrentSkillLevel;
	}

	return 0;
}

int KSkillList::GetCurrentLevelByIdx(int nListIdx)
{
	if (nListIdx > 0 && nListIdx < MAX_NPCSKILL)
		return m_Skills[nListIdx].CurrentSkillLevel;

	return 0;
}

//是否可以发技能 发技能的 时间间隔
BOOL KSkillList::CanCastByIndex(int nSkListIndex, DWORD dwTime)
{

	if (nSkListIndex<=0 && nSkListIndex>=MAX_NPCSKILL) return false;

	if (m_Skills[nSkListIndex].CurrentSkillLevel <= 0 || m_Skills[nSkListIndex].NextCastTime > dwTime)  //时间间隔大于现在的时间
		return FALSE;

	return TRUE;
}

char KSkillList::GetPointAdd(int nSkillID)
{
	int i;
	
	if (!nSkillID)
		return 0;
	
	i = FindSame(nSkillID);

	if (i)
	{
		//return m_Skills[i].PointAdd;
		return 0;
	}

	return 0;
}

//是否可以发技能 发技能的 时间间隔
BOOL KSkillList::CanCast(int nSkillID, DWORD dwTime)
{
	
	if (!nSkillID)
		return FALSE;
	
	int i = FindSame(nSkillID);
	if (!i)
	{
		return FALSE;
	}

	if (m_Skills[i].CurrentSkillLevel <= 0 || m_Skills[i].NextCastTime > dwTime)  //时间间隔大于现在的时间
		return FALSE;
	
	return TRUE;
}

//设置骑马技能解冻时间
void	KSkillList::SetHorseNextCastTimeByIndex(int nSkListIndex, DWORD dwTime)
{
	if (nSkListIndex<=0 && nSkListIndex>=MAX_NPCSKILL) return;	
	m_Skills[nSkListIndex].NextHorseCastTime = dwTime;
}
//下次发技能时间
void KSkillList::SetNextCastTimeByIndex(int nSkListIndex, DWORD dwTime)
{
	if (nSkListIndex<=0 && nSkListIndex>=MAX_NPCSKILL) return;
	m_Skills[nSkListIndex].NextCastTime = dwTime;
}	

//设置骑马技能解冻时间
void	KSkillList::SetHorseNextCastTime(int nSkillID, DWORD dwTime)
{
	if (!nSkillID)
		return;	
	int i = FindSame(nSkillID);
	if (!i)
		return;	
	m_Skills[i].NextHorseCastTime = dwTime;
}
//下次发技能时间
void KSkillList::SetNextCastTime(int nSkillID, DWORD dwTime)
{
//	m_Skills[nSkillID].NextCastTime = dwTime;
//	return;
	if (!nSkillID)
		return;
	
	int i = FindSame(nSkillID);
	if (!i)
		return;
	
	m_Skills[i].NextCastTime = dwTime;
}	


int KSkillList::GetHorseNextCastTimeByIndex(int nSkListIndex)
{
	if (nSkListIndex<=0 || nSkListIndex>=MAX_NPCSKILL)
		return -1;

	return	m_Skills[nSkListIndex].NextHorseCastTime;
}

int KSkillList::GetHorseNextCastTime(int nSkillID)
{
	if (!nSkillID)
		return -1;
	
	int i = FindSame(nSkillID);
	if (!i)
		return -1;
	
    return	m_Skills[i].NextHorseCastTime;
}

int KSkillList::GetNextCastTimeByIndex(int nSkListIndex)
{
	//	m_Skills[nSkillID].NextCastTime = dwTime;
	//	return;
	if (nSkListIndex<=0 || nSkListIndex>=MAX_NPCSKILL)
		return -1;

	return	m_Skills[nSkListIndex].NextCastTime;
}

int KSkillList::GetNextCastTime(int nSkillID)
{
	//	m_Skills[nSkillID].NextCastTime = dwTime;
	//	return;
	if (!nSkillID)
		return -1;
	
	int i = FindSame(nSkillID);
	if (!i)
		return -1;	
    return	m_Skills[i].NextCastTime;
}
//重新计算技能加成
void KSkillList::ReEnChance()
{
	int i;
	for(i = 0;i < MAX_NPCSKILL;++i)
	{//清空加成
		if (m_Skills[i].SkillId <= 0)
			continue;
		m_Skills[i].EnChance = 0;
		m_Skills[i].nTempEnChance = 0;
	}

	for(i = 0;i < MAX_NPCSKILL;++i)
	{
		if (m_Skills[i].SkillId <= 0 || m_Skills[i].SkillId==1 || m_Skills[i].SkillId==2 || m_Skills[i].SkillId==53)
			continue;
		int nSkillId    = m_Skills[i].SkillId;
		int nSkillLevel = m_Skills[i].CurrentSkillLevel;

		if (nSkillLevel <= 0)
			continue;
		KSkill * pOrdinSkill = (KSkill *) g_SkillManager.GetSkill(nSkillId, nSkillLevel);
        //设置加成效果
		if (!pOrdinSkill)
			continue;
		pOrdinSkill->EnChanceSkill(m_nNpcIndex);
		//执行完后开始备份 
		m_Skills[i].nTempEnChance = m_Skills[i].EnChance;
		//printf("----(%d)技能:%s 原始加成:%d -----\n",i,pOrdinSkill->GetSkillName(),m_Skills[i].nTempEnChance);
	}
}

//--------------------------客户端技能数据-----------------------
#ifndef _SERVER
int	KSkillList::GetSkillSortList(KUiSkillData * pSkillList)
{
	//Ques
	if (!pSkillList)
		return 0;

	memset(pSkillList, 0, sizeof(KUiSkillData) * 50);
	int nCount = 0;
	ISkill* pSkill = NULL;
	KSkill * pOrdinSkill = NULL;
	for (int i = 1; i < MAX_NPCSKILL; ++i)
	{
		if (m_Skills[i].SkillId)
		{
			_ASSERT(m_Skills[i].SkillLevel >= 0);
			
			if (!m_Skills[i].SkillLevel)
			{
				pSkill =  g_SkillManager.GetSkill(m_Skills[i].SkillId, 1);
			}
			else 
			{
				pSkill =  g_SkillManager.GetSkill(m_Skills[i].SkillId, m_Skills[i].SkillLevel);
			}
			
			if (!pSkill)
				continue;
			eSkillStyle eStyle = (eSkillStyle)pSkill->GetSkillStyle();
			
			switch(eStyle)
			{
			case SKILL_SS_Missles:			        //	子弹类		本技能用于发送子弹类
			case SKILL_SS_Melee:
			case SKILL_SS_InitiativeNpcState:	    //	主动类		本技能用于改变当前Npc的主动状态
			case SKILL_SS_PassivityNpcState:		//	被动类		本技能用于改变Npc的被动状态
				{
					/*
					pOrdinSkill = (KSkill * ) pSkill;
					if (pOrdinSkill->IsPhysical())
						continue;//下一个技能
					*/
					if (m_Skills[i].SkillId==1 || m_Skills[i].SkillId==2 || m_Skills[i].SkillId==53)
						continue;//下一个技能
				}
				break;
			case SKILL_SS_Thief:					//	偷窃类
				{
					
				}
				break;
			default:
				if (m_Skills[i].SkillId==1 || m_Skills[i].SkillId==2 || m_Skills[i].SkillId==53)
					continue;//下一个技能
			}
			
            KUiSkillData * pSkill = pSkillList + nCount;
			pSkill->uGenre = CGOG_SKILL_FIGHT;
			pSkill->uId = m_Skills[i].SkillId;
			pSkill->nLevel = m_Skills[i].SkillLevel;
			if ((++nCount) == 50)  //技能树数量限制
			{
				printf("Npc的技能数量超过了限制50！！！，请检查!!!\n");			
				break;
			}
		}
	}
	return nCount;
}

int KSkillList::GetLeftSkillSortList(KUiSkillData* pSkillList)
{
	if (!pSkillList) return 0;
	memset(pSkillList, 0, sizeof(KUiSkillData) * 65);
	int nCount = 1;
	
	pSkillList->uGenre = CGOG_SKILL_SHORTCUT;
	pSkillList->uId = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetCurActiveWeaponSkill();//武器空手技能ID!
	pSkillList->nData = 0;
	
	ISkill * pISkill = NULL;
	KSkill * pOrdinSkill = NULL;
	for (int i = 1; i < MAX_NPCSKILL; ++i)
	{
		if (m_Skills[i].SkillId && m_Skills[i].SkillLevel > 0)
		{
			_ASSERT(m_Skills[i].SkillId > 0);
			pISkill = g_SkillManager.GetSkill(m_Skills[i].SkillId, m_Skills[i].SkillLevel);
			if (!pISkill)
				continue;
			
			eSkillStyle eStyle = (eSkillStyle)pISkill->GetSkillStyle();
			switch(eStyle)
			{
			case SKILL_SS_Missles:			//	子弹类		本技能用于发送子弹类
			case SKILL_SS_Melee:
			case SKILL_SS_InitiativeNpcState:	//	主动类		本技能用于改变当前Npc的主动状态
			case SKILL_SS_PassivityNpcState:		//	被动类		本技能用于改变Npc的被动状态
				{
					if (m_Skills[i].SkillId==1 || m_Skills[i].SkillId==2 || m_Skills[i].SkillId==53) 
                        continue;

					pOrdinSkill = (KSkill * ) pISkill;
					if (pOrdinSkill->GetSkillLRInfo() == BothSkill || pOrdinSkill->GetSkillLRInfo() == leftOnlySkill)
					{
					}
					else
					   continue;
					
				}
				break;				
			case SKILL_SS_Thief:
				{
					continue;
				}
				break;
			}		
			KUiSkillData * pSkill = pSkillList + nCount;
			pSkill->uGenre = CGOG_SKILL_SHORTCUT;
			pSkill->uId = m_Skills[i].SkillId;
			pSkill->nData = nCount / 8;
			nCount ++;
			if (nCount >= 65)
				break;
			
		}
	}
	return nCount;
}

int KSkillList::GetRightSkillSortList(KUiSkillData* pSkillList)
{
	if (!pSkillList) return 0;
	memset(pSkillList, 0, sizeof(KUiSkillData) * 65);
	
	int nCount = 1;
	pSkillList->uGenre = CGOG_SKILL_SHORTCUT;
	//pSkillList->uId = (unsigned int)-1;//物理技能置前!
	pSkillList->uId = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetCurActiveWeaponSkill();//物理技能置前!;	 
	pSkillList->nData = 0;
	ISkill * pISkill = NULL;
	KSkill * pOrdinSkill = NULL;
	
	for (int i = 1; i < MAX_NPCSKILL; ++i)
	{
		if (m_Skills[i].SkillId && m_Skills[i].SkillLevel > 0)
		{
			//_ASSERT(m_Skills[i].SkillId > 0);
			pISkill  =  g_SkillManager.GetSkill(m_Skills[i].SkillId, m_Skills[i].SkillLevel);
			if(!pISkill)
				continue;

			eSkillStyle eStyle = ( eSkillStyle ) pISkill->GetSkillStyle();
			
			switch(eStyle)
			{
			case SKILL_SS_Missles:			//	子弹类		本技能用于发送子弹类
			case SKILL_SS_Melee:
			case SKILL_SS_InitiativeNpcState:	//	主动类		本技能用于改变当前Npc的主动状态
			case SKILL_SS_PassivityNpcState:		//	被动类		本技能用于改变Npc的被动状态
				{
					if (m_Skills[i].SkillId==1 || m_Skills[i].SkillId==2 || m_Skills[i].SkillId==53) 
                        continue;
					pOrdinSkill = (KSkill * ) pISkill;
					if (pOrdinSkill->GetSkillLRInfo() == BothSkill || pOrdinSkill->GetSkillLRInfo() == RightOnlySkill)
					{	
					}
					else 
					    continue;  
				}
				break;		
			case SKILL_SS_Thief:
				{			
				}
				break;
			}
			KUiSkillData * pSkill = pSkillList + nCount;
			pSkill->uGenre = CGOG_SKILL_SHORTCUT;
			pSkill->uId = m_Skills[i].SkillId;
			pSkill->nData = nCount / 8;
			nCount ++;
			if (nCount >= 65)
				break;
			
		}
	}
	return nCount;
}

//获得技能在技能界面的位置
int KSkillList::GetSkillPosition(int nSkillId)
{
	if (nSkillId <= 0) 
		return -1;
	KSkill * pOrdinSkill = NULL;
	
	ISkill * pISkill = NULL;
	int nCount = 0;
	for (int i = 1; i < MAX_NPCSKILL; ++i)
	{
		//_ASSERT(m_Skills[i].SkillLevel >= 0);

		if (m_Skills[i].SkillId<= 0)
			continue;
		
		if (m_Skills[i].SkillLevel == 0)
		{//等级为0时的 ，设置默认等级1的数据
			pISkill = g_SkillManager.GetSkill(m_Skills[i].SkillId, 1);
		}
		else
		{
			pISkill =  g_SkillManager.GetSkill(m_Skills[i].SkillId, m_Skills[i].SkillLevel);
		}

		if (!pISkill) 
        	continue ;

		eSkillStyle eStyle = ( eSkillStyle ) pISkill->GetSkillStyle();
		switch(eStyle)
		{
		case SKILL_SS_Missles:			//	子弹类		本技能用于发送子弹类
		case SKILL_SS_Melee:
		case SKILL_SS_InitiativeNpcState:	//	主动类		本技能用于改变当前Npc的主动状态
		case SKILL_SS_PassivityNpcState:		//	被动类		本技能用于改变Npc的被动状态
		/*	{
				pOrdinSkill = (KSkill * ) pISkill;
				if ((m_Skills[i].SkillId<=0) )//|| (pOrdinSkill->IsPhysical())
				{
					continue;
				}
			
			break;
			*/
		case SKILL_SS_Thief:
			break;
		}	
		
		if (m_Skills[i].SkillId == nSkillId)
		{
			nCount=i;
			return nCount;
		}
		//nCount ++;
		
	}
	return nCount;//nCount = i-1;
}

//设置额外的技能点
void KSkillList::SetAddPoint( int nId, int nPoint )
{
	int i = FindSame(nId);

	if (i > 0 && i < MAX_NPCSKILL)
	{
		m_Skills[i].mAddPoint = nPoint;
	}
}
//客户端调用
void KSkillList::SetEnChance( int nId, int nEnChance )
{
	int i = FindSame(nId);
	
	if (i > 0 && i < MAX_NPCSKILL)
	{
		m_Skills[i].EnChance = nEnChance;
		//m_Skills[i].nTempEnChance = nEnChance;
	}
}

void KSkillList::AddCEnChance( int nId, int nEnChance )
{
	int i = FindSame(nId);
	
	if (i > 0 && i < MAX_NPCSKILL)
	{
		m_Skills[i].EnChance+= nEnChance;
	}
}
#endif

//存档调用 生成技能包
#ifdef _SERVER
int	KSkillList::UpdateDBSkillList(BYTE * pSkillBuffer)
{
	if (!pSkillBuffer)
		return -1;
	
	int nCount = 0,i;

//	TDBSkillData * sSkillData = (TDBSkillData *) pSkillBuffer;

/*	for (i = 0 ; i < MAX_NPCSKILL; ++i)
	{//存档前全部清空
		sSkillData->m_nSkillId = 0;
		sSkillData->m_nSkillExp= 0;
		sSkillData->m_nSkillLevel =0;
		sSkillData++;
	} */

	TDBSkillData * pSkillData = (TDBSkillData *)pSkillBuffer;

	//for (i = 0 ; i < 4; i++)
	//{//保证前面三个一定是新手技能
	//	if  (i==0)
	//	{
	//		pSkillData->m_nSkillId = 0;
	//		pSkillData->m_nSkillExp= 0;
	//		pSkillData->m_nSkillLevel =0;
	//		pSkillData++;
	//		continue;
	//	}

	//	if  (i==1)
	//	{
	//		pSkillData->m_nSkillId = 53;
	//		pSkillData->m_nSkillExp= 0;
	//		pSkillData->m_nSkillLevel =1;
	//		pSkillData++;
	//		continue;

	//	}
	//	if  (i==2)
	//	{
	//		pSkillData->m_nSkillId = 1;
	//		pSkillData->m_nSkillExp= 0;
	//		pSkillData->m_nSkillLevel =1;
	//		pSkillData++;
	//		continue;
	//	}
	//	if  (i==3)
	//	{
	//		pSkillData->m_nSkillId = 2;
	//		pSkillData->m_nSkillExp= 0;
	//		pSkillData->m_nSkillLevel =1;
	//		pSkillData++;
	//		continue;
	//	}
	//}


	for (i = 0 ; i < MAX_NPCSKILL; i++)
	{
		if (m_Skills[i].SkillId > 0)
		{
			    if (m_Skills[i].SkillId>g_SkillManager.GetSkillCount())
				{
				   m_Skills[i].SkillId=0;
				   m_Skills[i].SkillLevel =0;
				   m_Skills[i].nCurSkillExp=0;
				   m_Skills[i].nExpSkillPoint=0;

				   pSkillData->m_nSkillId = 0;
                   pSkillData->m_nSkillExp= 0;
				   pSkillData->m_nSkillLevel =0;
				   pSkillData->m_nExpSkillPonit=0;
		           continue;
				}

				/*if (m_Skills[i].SkillId==53 || m_Skills[i].SkillId==1 || m_Skills[i].SkillId==2)
				continue;*/

				if (m_Skills[i].SkillLevel>=MAX_SKILLLEVEL)
					m_Skills[i].SkillLevel =MAX_SKILLLEVEL-1;

				pSkillData->m_nSkillId = m_Skills[i].SkillId;
                pSkillData->m_nSkillExp= m_Skills[i].nCurSkillExp;          //该技能当前的熟练度
				pSkillData->m_nExpSkillPonit=m_Skills[i].nExpSkillPoint;//修炼出来的技能点

				if (m_Skills[i].mAddPoint > 0)   //该技能的额外的技能点大于0的
					pSkillData->m_nSkillLevel = m_Skills[i].SkillLevel - m_Skills[i].mAddPoint;	
				else
					pSkillData->m_nSkillLevel = m_Skills[i].SkillLevel;

				nCount++;    //实际的技能数
				pSkillData++;
		}
		else
		{
			m_Skills[i].SkillId=0;
			m_Skills[i].SkillLevel =0;
			m_Skills[i].nCurSkillExp=0;
			m_Skills[i].nExpSkillPoint=0;

			pSkillData->m_nSkillId = 0;
			pSkillData->m_nSkillExp= 0;
			pSkillData->m_nSkillLevel =0;
			pSkillData->m_nExpSkillPonit=0;
		}
	}
	return nCount;
}
//洗点调用
int	KSkillList::ClearSkillLevel(int bNoClearExpLevel)
{
	int nTotalSkillPoint = 0;

	for (int i = 0 ; i < MAX_NPCSKILL; ++i)
	{
		if (m_Skills[i].SkillId > 0 && m_Skills[i].SkillId != 1 && m_Skills[i].SkillId !=2 && m_Skills[i].SkillId != 210 && m_Skills[i].SkillId != 400)
		{
			ISkill * pSkill = g_SkillManager.GetSkill(m_Skills[i].SkillId, 1);

			if (((KSkill*)pSkill)->GetSkillReqLevel() == 80 || ((KSkill*)pSkill)->GetSkillReqLevel() == 120)
				continue;

			if (pSkill)
			{
                int nReg=0;
				    nReg=CheckNoSkill("NoCanXiSui",m_Skills[i].SkillId);
				  if (nReg)
					continue;

				if (bNoClearExpLevel && m_Skills[i].nIsExpSkill == 1 && m_Skills[i].nExpSkill > 0) //熟练度技能不参与洗点
				{//保留熟练度
					nTotalSkillPoint += m_Skills[i].SkillLevel-m_Skills[i].nExpSkillPoint-m_Skills[i].mAddPoint; //该位置实际技能的技能点			
					m_Skills[i].SkillLevel = m_Skills[i].nExpSkillPoint;
					m_Skills[i].CurrentSkillLevel = m_Skills[i].nExpSkillPoint;
				}
				else
				{//非保留熟练度

					nTotalSkillPoint += m_Skills[i].SkillLevel - m_Skills[i].nExpSkillPoint - m_Skills[i].mAddPoint; //该位置实际技能的技能点
					m_Skills[i].SkillLevel =0;
					m_Skills[i].CurrentSkillLevel =0;
					m_Skills[i].nExpSkillPoint=0;
				}

				m_Skills[i].mAddPoint = 0;
				m_Skills[i].EnChance = 0;
				m_Skills[i].nTempEnChance = 0;
				m_Skills[i].nSkillStyle = -1;
				m_Skills[i].NextCastTime = 0;
				m_Skills[i].NextHorseCastTime = 0;
	 	        m_Skills[i].nEquiptLimited = 0;
				m_Skills[i].nExpSkill = 0;
				m_Skills[i].nCurSkillExp = 0;
			}
		}
	}
	//printf(".......洗点测试: %d.......\n",nTotalSkillPoint);
	return nTotalSkillPoint;   //
}

#endif

int	KSkillList::CheckNoSkill(char *nKey,int nSkillidx)
{
	int nRow = g_ForbitMap.GetHeight()+1,nReg=0;
	
	for (int i=2;i<nRow;++i)
	{
		int nSkilid =0;
		g_ForbitMap.GetInteger(i,nKey,0,&nSkilid);
		if (nSkilid==nSkillidx)
		{
			nReg =1;
			break;
		}
	}
	return nReg;     
}

int		KSkillList::RollBackSkills(bool bRbAll)
{
	int nTotalSkillPoint = 0;

	for (int i = 0 ; i < MAX_NPCSKILL; i ++)
	{
		if (m_Skills[i].SkillId > 0)
		{
			ISkill * pSkill = g_SkillManager.GetSkill(m_Skills[i].SkillId, 1);
			if (pSkill)
			{
				if (pSkill->IsBase()) continue;
				if (!bRbAll && pSkill->GetIsExpSkill()) continue;
				if(!pSkill->GetIsExpSkill())
					nTotalSkillPoint += m_Skills[i].SkillLevel;
				m_Skills[i].SkillLevel = 0;
				//m_Skills[i].AddLevel = m_nAllSkillV;
				m_Skills[i].CurrentSkillLevel = 0;
			}
		}
	}
	return nTotalSkillPoint;
}

//fix skill
int KSkillList::GetAddSkillDamage(int nSkillID) 
{ 
    int nAddP = 0; 

    if (nSkillID <= 0 || nSkillID >= MAX_SKILL) 
        return nAddP; 
     
    for (int i = 1; i < MAX_NPCSKILL; i++) 
    { 
		if (m_Skills[i].SkillLevel)
		{
			KSkill * pSkill = (KSkill *) g_SkillManager.GetSkill(m_Skills[i].SkillId, m_Skills[i].CurrentSkillLevel); 
			if (pSkill)  
			{ 
				KMagicAttrib* pMagicData = NULL; 
				//pMagicData = pSkill->GetAddSkillDamage();     
				for (int j =0 ; j < 9 ; j ++) 
				{ 
					if (!pMagicData[j].nAttribType) continue; 
					if ( pMagicData[j].nValue[0] == nSkillID) 
					{ 
						nAddP += pMagicData[j].nValue[2]; 
					} 
				} 
			}
		}
    } 
    return nAddP; 
}  

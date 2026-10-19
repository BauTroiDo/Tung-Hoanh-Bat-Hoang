//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KNpcGold.cpp
// Date:	2003.07.23
// Code:	边城浪子
// Desc:	KNpcGold Class
//---------------------------------------------------------------------------
#include	"KCore.h"
#include	"KNpcTemplate.h"
#include	"KNpc.h"
#include	"KNpcGold.h"

#define		defNPC_GOLD_SKILL_NO		5
#define		defNPC_GOLD_RATE			20

#ifdef _SERVER
extern KLuaScript		*g_pNpcLevelScript;
#endif

void	KNpcGold::Init(int nIdx)
{
	this->m_nNpcIdx				= nIdx;
	this->m_nIsGold				= 0;
	this->m_nIsGolding			= 0;
	this->m_nGoldType			= 0;

	this->m_dwSkill5ID			= 0;
	this->m_nSkill5Level		= 0;
	this->m_nFireResist			= 0;
	this->m_nFireResistMax		= 0;
	this->m_nColdResist			= 0;
	this->m_nColdResistMax		= 0;
	this->m_nLightingResist		= 0;
	this->m_nLightingResistMax	= 0;
	this->m_nPoisonResist		= 0;
	this->m_nPoisonResistMax	= 0;
	this->m_nPhycicsResist		= 0;
	this->m_nPhycicsResistMax	= 0;
	////////////////////////////////
    /*this->m_nAiMode	= 0;
    this->m_nAiParam1	= 0;
    this->m_nAiParam2	= 0;
    this->m_nAiParam3	= 0;
    this->m_nAiParam4= 0;
    this->m_nAiParam5= 0;
    this->m_nAiParam6= 0;
    this->m_nAiParam7= 0;
    this->m_nAiParam8= 0;
    this->m_nAiParam9= 0;
    this->m_nAiParam10= 0;
    this->m_nAiMaxTime= 0;
    this->m_nPhysicalDamageBase= 0;
    this->m_nPhysicalMagicBase= 0;
    this->m_nPoisonDamageBase= 0;
    this->m_nPoisonMagicBase= 0;
    this->m_nColdDamageBase= 0;
    this->m_nColdMagicBase= 0;
    this->m_nFireDamageBase= 0;
    this->m_nFireMagicBase= 0;
    this->m_nLightingDamageBase= 0;
    this->m_nLightingMagicBase= 0;
	*/
    //ZeroMemory(this->m_nDeathScript,sizeof(this->m_nDeathScript));
    //ZeroMemory(this->m_nDropRate,sizeof(this->m_nDropRate));
    //this->m_nRate= 0;
}

//---------------------------------------------------------------------------
//	功能：设定是否为黄金怪物
//---------------------------------------------------------------------------
void	KNpcGold::SetGoldType(BOOL bFlag)
{
	this->m_nIsGold = (bFlag != 0 ? 1 : 0);
}

//---------------------------------------------------------------------------
//	功能：设定类型为黄金怪物，同时备份相应数据
//---------------------------------------------------------------------------
void	KNpcGold::SetGoldTypeAndBackData()
{
	m_nIsGold = 1;
	m_nIsGolding = 0;
	m_nGoldType = 0;

	m_nFireResist			= Npc[m_nNpcIdx].m_CurrentFireResist;
	m_nFireResistMax		= Npc[m_nNpcIdx].m_CurrentFireResistMax;
	m_nColdResist			= Npc[m_nNpcIdx].m_CurrentColdResist;
	m_nColdResistMax		= Npc[m_nNpcIdx].m_CurrentColdResistMax;
	m_nLightingResist		= Npc[m_nNpcIdx].m_CurrentLightResist;
	m_nLightingResistMax	= Npc[m_nNpcIdx].m_CurrentLightResistMax;
	m_nPoisonResist			= Npc[m_nNpcIdx].m_CurrentPoisonResist;
	m_nPoisonResistMax		= Npc[m_nNpcIdx].m_CurrentPoisonResistMax;
	m_nPhycicsResist		= Npc[m_nNpcIdx].m_CurrentPhysicsResist;
	m_nPhycicsResistMax		= Npc[m_nNpcIdx].m_CurrentPhysicsResistMax;
}
//---------------------------------------------------------------------------
//	功能：设定类型为黄金怪物，同时备份相应数据
//---------------------------------------------------------------------------
void	KNpcGold::SetBossTypeAndBackData(int nBoss)
{
	m_nIsGold = 1;
	m_nIsGolding = 1;
	m_nGoldType = nBoss;
	
	m_nFireResist			= Npc[m_nNpcIdx].m_CurrentFireResist;
	m_nFireResistMax		= Npc[m_nNpcIdx].m_CurrentFireResistMax;
	m_nColdResist			= Npc[m_nNpcIdx].m_CurrentColdResist;
	m_nColdResistMax		= Npc[m_nNpcIdx].m_CurrentColdResistMax;
	m_nLightingResist		= Npc[m_nNpcIdx].m_CurrentLightResist;
	m_nLightingResistMax	= Npc[m_nNpcIdx].m_CurrentLightResistMax;
	m_nPoisonResist			= Npc[m_nNpcIdx].m_CurrentPoisonResist;
	m_nPoisonResistMax		= Npc[m_nNpcIdx].m_CurrentPoisonResistMax;
	m_nPhycicsResist		= Npc[m_nNpcIdx].m_CurrentPhysicsResist;
	m_nPhycicsResistMax		= Npc[m_nNpcIdx].m_CurrentPhysicsResistMax;
}

//---------------------------------------------------------------------------
//	功能：恢复旧数据，变成普通npc
//---------------------------------------------------------------------------
void	KNpcGold::RecoverBackData()
{
	if (/*!m_nIsGold || */!m_nIsGolding || m_nGoldType >defMAX_NPC_GOLD_TYEP)
		return;

	m_nIsGolding = 0;

	// 用备份数据覆盖
	Npc[m_nNpcIdx].m_nReducedDamge				= 0;
	Npc[m_nNpcIdx].m_CurrentFireResist			= m_nFireResist;	   //原始数据
	Npc[m_nNpcIdx].m_CurrentFireResistMax		= m_nFireResistMax;
	Npc[m_nNpcIdx].m_CurrentColdResist			= m_nColdResist;
	Npc[m_nNpcIdx].m_CurrentColdResistMax		= m_nColdResistMax;
	Npc[m_nNpcIdx].m_CurrentLightResist			= m_nLightingResist;
	Npc[m_nNpcIdx].m_CurrentLightResistMax		= m_nLightingResistMax;
	Npc[m_nNpcIdx].m_CurrentPoisonResist		= m_nPoisonResist;
	Npc[m_nNpcIdx].m_CurrentPoisonResistMax		= m_nPoisonResistMax;
	Npc[m_nNpcIdx].m_CurrentPhysicsResist		= m_nPhycicsResist;
	Npc[m_nNpcIdx].m_CurrentPhysicsResistMax	= m_nPhycicsResistMax;
#ifdef _SERVER
	Npc[m_nNpcIdx].m_SkillList.RemoveNpcSkill(defNPC_GOLD_SKILL_NO);
	Npc[m_nNpcIdx].SetAuraSkill(0);   //设置技能为空 
#endif

	if (m_nGoldType < 0 || m_nGoldType >= NpcSet.m_cGoldTemplate.m_nEffectTypeNum)
		return;

	KNpcGoldTemplateInfo	*pInfo = &NpcSet.m_cGoldTemplate.m_sInfo[m_nGoldType];

	// 按比例缩小
	//Npc[m_nNpcIdx].m_Experience					/= pInfo->m_nExp;
	if  (pInfo->m_nLife>0)
	    Npc[m_nNpcIdx].m_CurrentLifeMax				/= pInfo->m_nLife;
	else
		Npc[m_nNpcIdx].m_CurrentLifeMax = 100;

	if (pInfo->m_nExp>0)
		Npc[m_nNpcIdx].m_Experience	/= pInfo->m_nExp;
	else
		Npc[m_nNpcIdx].m_Experience   = 1;


	if (pInfo->m_nLifeReplenish>0)
	    Npc[m_nNpcIdx].m_CurrentLifeReplenish	/= pInfo->m_nLifeReplenish;
	else
		Npc[m_nNpcIdx].m_CurrentLifeReplenish   = 0;

	if (pInfo->m_nAttackRating>0)
	    Npc[m_nNpcIdx].m_CurrentAttackRating		/= pInfo->m_nAttackRating;
	else
		Npc[m_nNpcIdx].m_CurrentAttackRating    = 0;

	if (pInfo->m_nDefense>0)
        Npc[m_nNpcIdx].m_CurrentDefend				/= pInfo->m_nDefense;
	else
		Npc[m_nNpcIdx].m_CurrentDefend          = 0;
   
	if (pInfo->m_nMinDamage>0)
	   Npc[m_nNpcIdx].m_PhysicsDamage.nValue[0]	/= pInfo->m_nMinDamage;	  //基本伤害
	else
	   Npc[m_nNpcIdx].m_PhysicsDamage.nValue[0] = 0;
	if (pInfo->m_nMaxDamage>0)
	   Npc[m_nNpcIdx].m_PhysicsDamage.nValue[2]	/= pInfo->m_nMaxDamage;	  //基本伤害--不包括技能
	else
	   Npc[m_nNpcIdx].m_PhysicsDamage.nValue[2] = 5;

	// 扣点数
	Npc[m_nNpcIdx].m_CurrentTreasure	-= pInfo->m_nTreasure;   //掉落装备的数量
	Npc[m_nNpcIdx].m_CurrentWalkSpeed	-= pInfo->m_nWalkSpeed;
	Npc[m_nNpcIdx].m_CurrentRunSpeed	-= pInfo->m_nRunSpeed;   //
	Npc[m_nNpcIdx].m_CurrentAttackSpeed	-= pInfo->m_nAttackSpeed;
	Npc[m_nNpcIdx].m_CurrentCastSpeed	-= pInfo->m_nCastSpeed;
}

//---------------------------------------------------------------------------
//	功能：按一定概率变成黄金怪物---蓝怪几率产生设置-
//---------------------------------------------------------------------------
void KNpcGold::RandChangeGold(BOOL bGold,int BossType,int nSubWorld)  //默认 5 和 flase
{
	//if (SubWorld[nSubWorld].m_SubWorldID==38)
	//printf("--BOSS类型B：%d 地图:%d--\n",BossType,SubWorld[nSubWorld].m_SubWorldID);

    if (BossType<0)
	{
		m_nIsGold		= 0;
		m_nIsGolding	= 0;
 #ifdef _SERVER
		int inskillid=0,nSkillLevel=1;
		if (inskillid=Npc[m_nNpcIdx].m_SkillList.GetSkillId(5))
		{
			nSkillLevel=Npc[m_nNpcIdx].m_SkillList.GetLevel(inskillid);
			
			if (nSkillLevel<=0)
				nSkillLevel=1;
			
			if (Npc[m_nNpcIdx].m_SkillList.SetNpcSkill(5,inskillid,nSkillLevel))
				Npc[m_nNpcIdx].SetAuraSkill(inskillid); //设置为光环技能

		}

		if (inskillid=Npc[m_nNpcIdx].m_SkillList.GetSkillId(6))
		{
			nSkillLevel=Npc[m_nNpcIdx].m_SkillList.GetLevel(inskillid);
			
			if (nSkillLevel<=0)
				nSkillLevel=1;
			
			Npc[m_nNpcIdx].m_SkillList.SetNpcSkill(6,inskillid,nSkillLevel);
		}
#endif
        return;
	}
    else if (BossType>0)
	{ 
      m_nGoldType =BossType;	
	  if (NpcSet.m_cGoldTemplate.m_nEffectTypeNum <= 0)
		return;
	} 
    else
	{  
	  if (/*m_nIsGold || */this->m_nIsGolding==1)
		return;

	  //if (SubWorld[nSubWorld].m_SubWorldID==38)
	  //	  printf("--BOSS类型A：%d--\n",BossType);

      int npMapGoldRate=0,nMapAutoGoldNpcRank;
	  char nMapGoldRate[32]={0};
	  snprintf(nMapGoldRate, sizeof(nMapGoldRate),"%d_AutoGoldenNpc",SubWorld[nSubWorld].m_SubWorldID);  //38_AutoGoldenNpc
      g_NpcMapDropRate.GetInteger("List",nMapGoldRate,0,&npMapGoldRate);//生成加强怪物的概率

	  nMapAutoGoldNpcRank = _gameGlobalSet.SubWorldInfo.nMapAutoGoldNpcRank;
	  //g_GameSetTing.GetInteger("SYSTEM","MapAutoGoldNpcRank",0,&nMapAutoGoldNpcRank);

	  if (npMapGoldRate>=0 && g_Random(npMapGoldRate) < g_Random(nMapAutoGoldNpcRank))
	  {//普通怪物   
#ifdef _SERVER
		if (Npc[m_nNpcIdx].m_Kind==kind_normal)
		{ 
			//if (SubWorld[nSubWorld].m_SubWorldID==38)
			//	printf("--BOSS概率B：%d--\n",npMapGoldRate);

		  char nMapIdx[32]={0},nNormalDropFile[128]={0};
		  snprintf(nMapIdx, sizeof(nMapIdx),"%d_NormalDropRate",SubWorld[nSubWorld].m_SubWorldID);
		  g_NpcMapDropRate.GetString("List",nMapIdx,"",nNormalDropFile,sizeof(nNormalDropFile));
		  int nSet=0;
		      nSet=CheckThisNpc(Npc[m_nNpcIdx].m_NpcSettingIdx,"NpcsetID"); //检查是否在禁止列表中

		  if (!nSet)
		  {
		     if (nNormalDropFile[0])
			 {//普通怪

				  KNpcTemplate* pNpcTemp = NULL;
				  if (pNpcTemp = NpcSet.GetTemplate(Npc[m_nNpcIdx].m_NpcSettingIdx, 0))
					  pNpcTemp->InitDropRate(m_nNpcIdx,nNormalDropFile);	
				  else if (pNpcTemp = NpcSet.GetTemplate(Npc[m_nNpcIdx].m_NpcSettingIdx, Npc[m_nNpcIdx].m_Level))
					  pNpcTemp->InitDropRate(m_nNpcIdx,nNormalDropFile);	

			  
			  /*if (g_pNpcTemplate[Npc[m_nNpcIdx].m_NpcSettingIdx][0])
	              g_pNpcTemplate[Npc[m_nNpcIdx].m_NpcSettingIdx][0]->InitDropRate(m_nNpcIdx,nNormalDropFile);  //白明爆率修改	
			  else if (g_pNpcTemplate[Npc[m_nNpcIdx].m_NpcSettingIdx][Npc[m_nNpcIdx].m_Level])
				  g_pNpcTemplate[Npc[m_nNpcIdx].m_NpcSettingIdx][Npc[m_nNpcIdx].m_Level]->InitDropRate(m_nNpcIdx,nNormalDropFile);  //白明爆率修改	
			 	  */
			 }
			 //else
			 //  Npc[m_nNpcIdx].m_pDropRate=NULL;
		  }
		  //else
		  //{
			//  Npc[m_nNpcIdx].m_pDropRate=NULL;
		  //}

		  int inskillid=0,nSkillLevel=1;

		  if (inskillid=Npc[m_nNpcIdx].m_SkillList.GetSkillId(defNPC_GOLD_SKILL_NO))
		  {
			  nSkillLevel=Npc[m_nNpcIdx].m_SkillList.GetLevel(inskillid);
			  if (nSkillLevel<=0)
			     nSkillLevel=1;

			  if (Npc[m_nNpcIdx].m_SkillList.SetNpcSkill(defNPC_GOLD_SKILL_NO,inskillid,nSkillLevel))
				  Npc[m_nNpcIdx].SetAuraSkill(inskillid); //设置为光环技能
		  }

		  if (inskillid=Npc[m_nNpcIdx].m_SkillList.GetSkillId(6))
		  {
			  nSkillLevel=Npc[m_nNpcIdx].m_SkillList.GetLevel(inskillid);
			  if (nSkillLevel<=0)
				  nSkillLevel=1;
			  
			  Npc[m_nNpcIdx].m_SkillList.SetNpcSkill(6,inskillid,nSkillLevel);
		  }


		}  
#endif
		return;
	  }  

	if (NpcSet.m_cGoldTemplate.m_nEffectTypeNum <= 0)
		return;
//原来没有的  蓝黄怪产生类型
//fix quai khi addnpc khi khoi tao 1 dang boss blue duy nhat
   if (bGold==1)
   { 
	   char nMapGoldType[32]={0};
       int  npMapGoldType=0;
       sprintf(nMapGoldType,"%d_GoldenType"   ,SubWorld[nSubWorld].m_SubWorldID); 
       g_NpcMapDropRate.GetInteger("List",nMapGoldType,4,&npMapGoldType);
	  //if (SubWorld[nSubWorld].m_SubWorldID==38)
	  //    printf("--BOSS概率A：%d--\n",npMapGoldRate);
       if (npMapGoldRate<0 || npMapGoldRate>1)
	   {  
         if (g_Random(npMapGoldRate) >= g_Random(npMapGoldRate))
	   	 // m_nGoldType = GetRandomNumber(1,npMapGoldType); //绿 蓝 黄 红
			m_nGoldType = GetRandomNumber(2,4); //蓝怪
         else
			m_nGoldType = GetRandomNumber(2,4); //蓝怪
			//m_nGoldType = GetRandomNumber(1,4); //蓝怪
	   } 
	   else
		   return;
	}
    else
       return;
	}
////////////////////////////////////////////////////以下是脚本刷怪
      m_nIsGolding = 1;

    KNpcGoldTemplateInfo	*pInfo              = &NpcSet.m_cGoldTemplate.m_sInfo[m_nGoldType];
	// 用模板数据直接辅值
	Npc[m_nNpcIdx].m_CurrentFireResist			= pInfo->m_nFireResist;
	Npc[m_nNpcIdx].m_CurrentFireResistMax		= pInfo->m_nFireResistMax;
	Npc[m_nNpcIdx].m_CurrentColdResist			= pInfo->m_nColdResist;
	Npc[m_nNpcIdx].m_CurrentColdResistMax		= pInfo->m_nColdResistMax;
	Npc[m_nNpcIdx].m_CurrentLightResist			= pInfo->m_nLightingResist;
	Npc[m_nNpcIdx].m_CurrentLightResistMax		= pInfo->m_nLightingResistMax;
	Npc[m_nNpcIdx].m_CurrentPoisonResist		= pInfo->m_nPoisonResist;
	Npc[m_nNpcIdx].m_CurrentPoisonResistMax		= pInfo->m_nPoisonResistMax;
	Npc[m_nNpcIdx].m_CurrentPhysicsResist		= pInfo->m_nPhycicsResist;
    Npc[m_nNpcIdx].m_CurrentPhysicsResistMax	= pInfo->m_nPhycicsResistMax;

#ifdef _SERVER
	
	if (pInfo->m_dwSkill5ID && pInfo->m_szSkill5Level)  //第5个技能
	{//设置第5个技能
		//KNpcTemplate nNpcScriptData;
		//int nSkillLevel = 2;
		if (pInfo->m_szSkill5Level>63)
			pInfo->m_szSkill5Level = 63;
        //nSkillLevel = GetGoldNpcLevelDataFromScript(g_pNpcLevelScript, "Level5", Npc[m_nNpcIdx].m_Level, pInfo->m_szSkill5Level,Npc[m_nNpcIdx].m_Series);
		//printf("[------蓝怪信息:等级:%d,类型:%d,技能ID:%d,技能倍率:%s,五行:%d------]\n",Npc[m_nNpcIdx].m_Level,m_nGoldType,pInfo->m_dwSkill5ID,pInfo->m_szSkill5Level,Npc[m_nNpcIdx].m_Series);
		if (Npc[m_nNpcIdx].m_SkillList.SetNpcSkill(defNPC_GOLD_SKILL_NO,pInfo->m_dwSkill5ID,pInfo->m_szSkill5Level))
			Npc[m_nNpcIdx].SetAuraSkill(pInfo->m_dwSkill5ID); //设置为光环技能	
	}

	int inskillid=0,nSkillLevel=1;

	if (inskillid=Npc[m_nNpcIdx].m_SkillList.GetSkillId(6))
	{
		nSkillLevel=Npc[m_nNpcIdx].m_SkillList.GetLevel(inskillid);

		if (nSkillLevel<=0)
			nSkillLevel=1;
		
		Npc[m_nNpcIdx].m_SkillList.SetNpcSkill(6,inskillid,nSkillLevel);
	}

#endif
	// 按比例放大
	Npc[m_nNpcIdx].m_Experience					*= pInfo->m_nExp;   //模板经验比例
	Npc[m_nNpcIdx].m_CurrentLifeMax				*= pInfo->m_nLife;  //模板倍数
	Npc[m_nNpcIdx].m_CurrentLifeReplenish		*= pInfo->m_nLifeReplenish;  //生命恢复速度
	Npc[m_nNpcIdx].m_CurrentAttackRating		*= pInfo->m_nAttackRating;  //命中
	Npc[m_nNpcIdx].m_CurrentDefend				*= pInfo->m_nDefense;   //闪避点
	Npc[m_nNpcIdx].m_PhysicsDamage.nValue[0]	*= pInfo->m_nMinDamage; //最小物理伤害
	Npc[m_nNpcIdx].m_PhysicsDamage.nValue[2]	*= pInfo->m_nMaxDamage; //最大物理伤害
	// 加点数
	Npc[m_nNpcIdx].m_CurrentTreasure	+= pInfo->m_nTreasure;   //掉落装备的数量
	Npc[m_nNpcIdx].m_CurrentWalkSpeed	+= pInfo->m_nWalkSpeed;
	Npc[m_nNpcIdx].m_CurrentRunSpeed	+= pInfo->m_nRunSpeed;
	Npc[m_nNpcIdx].m_CurrentAttackSpeed	+= pInfo->m_nAttackSpeed;  //外功速度
	Npc[m_nNpcIdx].m_CurrentCastSpeed	+= pInfo->m_nCastSpeed;    //内攻速
    //设置满血
	Npc[m_nNpcIdx].m_CurrentLife = Npc[m_nNpcIdx].m_CurrentLifeMax;
#ifdef _SERVER
	if (Npc[m_nNpcIdx].m_Kind==kind_normal)
	{//蓝怪 或 黄金怪的爆
		char nGDropFile[128]={0},nMapidB[32]={0};
		  ZeroMemory(nGDropFile,sizeof(nGDropFile));
		  ZeroMemory(nMapidB,sizeof(nMapidB));
		  snprintf(nMapidB, sizeof(nMapidB),"%d_GoldenDropRate",SubWorld[nSubWorld].m_SubWorldID);
		  g_NpcMapDropRate.GetString("List",nMapidB,"",nGDropFile,sizeof(nGDropFile));
		  int nSet=0;
		      nSet=CheckThisNpc(Npc[m_nNpcIdx].m_NpcSettingIdx,"NpcsetID"); //检查是否在禁止列表中
		  if (!nSet)
		  {
		     if (this->GetGoldType()>1 && this->GetGoldType()<=16 && nGDropFile[0])
			 {	 
			    
				 /*if (g_pNpcTemplate[Npc[m_nNpcIdx].m_NpcSettingIdx][0])
			      g_pNpcTemplate[Npc[m_nNpcIdx].m_NpcSettingIdx][0]->InitDropRate(m_nNpcIdx,nGDropFile);  //蓝怪爆率修改
		        else if (g_pNpcTemplate[Npc[m_nNpcIdx].m_NpcSettingIdx][Npc[m_nNpcIdx].m_Level])
				  g_pNpcTemplate[Npc[m_nNpcIdx].m_NpcSettingIdx][Npc[m_nNpcIdx].m_Level]->InitDropRate(m_nNpcIdx,nGDropFile);	  
				 */
			 
				 KNpcTemplate* pNpcTemp = NULL;
				 if (pNpcTemp = NpcSet.GetTemplate(Npc[m_nNpcIdx].m_NpcSettingIdx, 0))
					  pNpcTemp->InitDropRate(m_nNpcIdx,nGDropFile);	
				 else if (pNpcTemp = NpcSet.GetTemplate(Npc[m_nNpcIdx].m_NpcSettingIdx, Npc[m_nNpcIdx].m_Level))
					  pNpcTemp->InitDropRate(m_nNpcIdx,nGDropFile);
			 
			 }
			 //else
			//	Npc[m_nNpcIdx].m_pDropRate=NULL;
		  }
		 // else
		  //{
			//  Npc[m_nNpcIdx].m_pDropRate=NULL;
		  //}
	}
#endif

#ifdef _SERVER
	// 向周围九屏广播
	NPC_GOLD_CHANGE_SYNC	sSync;
	sSync.ProtocolType = s2c_npcgoldchange;
	sSync.m_dwNpcID = Npc[this->m_nNpcIdx].m_dwID;
	sSync.m_wGoldFlag = this->GetGoldType();
	Npc[m_nNpcIdx].SendDataToNearRegion((LPVOID)&sSync, sizeof(NPC_GOLD_CHANGE_SYNC));
#endif
}

//读取脚本 从脚本获取返回值
int KNpcGold::GetGoldNpcLevelDataFromScript(KLuaScript * pScript, char * szDataName, int nLevel, char * szParam,int nSeries)
{
	int nTopIndex = 0;
	int nReturn = 0;
	if (pScript==NULL || szParam == NULL|| szParam[0] == 0 /*|| strlen(szParam) < 3*/)
	{
		return 0;
	}
	nTopIndex=pScript->SafeCallBegin();
	pScript->CallFunction("GetNpcLevelData",1, "ddss",nSeries,nLevel,szDataName,szParam);   // 函数命令
	nTopIndex = Lua_GetTopIndex(pScript->m_LuaState);
	nReturn = (int) Lua_ValueToNumber(pScript->m_LuaState, nTopIndex);  //返回数字

	nTopIndex=pScript->SafeCallBegin();
	pScript->SafeCallEnd(nTopIndex);
	return nReturn;
}


//---------------------------------------------------------------------------
//	功能：获得当前黄金类型，0 非黄金怪
//---------------------------------------------------------------------------
int		KNpcGold::GetGoldType()
{
	if (/*!m_nIsGold || */!m_nIsGolding)
		return 0;
	return m_nGoldType;//m_nGoldType+1
}


void	KNpcGold::SetGoldCurrentType(int nType,int nSubWorld)
{
	if (nType <= 0 || nType > 16)
	{
		m_nIsGold		= 0;
		m_nIsGolding	= 0;
		return;
	}
	RecoverBackData();  //恢复基本数据
	m_nIsGold		= 1;
	m_nIsGolding	= 1;
	m_nGoldType		= nType;

    KNpcGoldTemplateInfo	*pInfo              = &NpcSet.m_cGoldTemplate.m_sInfo[m_nGoldType];
	// 用模板数据直接辅值
	Npc[m_nNpcIdx].m_nReducedDamge				 = 30;
	Npc[m_nNpcIdx].m_CurrentFireResist			= pInfo->m_nFireResist;
	Npc[m_nNpcIdx].m_CurrentFireResistMax		= pInfo->m_nFireResistMax;
	Npc[m_nNpcIdx].m_CurrentColdResist			= pInfo->m_nColdResist;
	Npc[m_nNpcIdx].m_CurrentColdResistMax		= pInfo->m_nColdResistMax;
	Npc[m_nNpcIdx].m_CurrentLightResist			= pInfo->m_nLightingResist;
	Npc[m_nNpcIdx].m_CurrentLightResistMax		= pInfo->m_nLightingResistMax;
	Npc[m_nNpcIdx].m_CurrentPoisonResist		= pInfo->m_nPoisonResist;
	Npc[m_nNpcIdx].m_CurrentPoisonResistMax		= pInfo->m_nPoisonResistMax;
	Npc[m_nNpcIdx].m_CurrentPhysicsResist		= pInfo->m_nPhycicsResist;
    Npc[m_nNpcIdx].m_CurrentPhysicsResistMax	= pInfo->m_nPhycicsResistMax;
	
#ifdef _SERVER
	
	if (pInfo->m_dwSkill5ID && pInfo->m_szSkill5Level)  //第5个技能
	{//设置第5个技能
		//KNpcTemplate nNpcScriptData;
		//int nSkillLevel = 2;
		if (pInfo->m_szSkill5Level>63)
			pInfo->m_szSkill5Level = 63;
        // nSkillLevel = GetGoldNpcLevelDataFromScript(g_pNpcLevelScript, "Level5", Npc[m_nNpcIdx].m_Level, pInfo->m_szSkill5Level,Npc[m_nNpcIdx].m_Series);
		//printf("[------蓝怪信息:等级:%d,类型:%d,技能ID:%d,技能倍率:%s,五行:%d------]\n",Npc[m_nNpcIdx].m_Level,m_nGoldType,pInfo->m_dwSkill5ID,pInfo->m_szSkill5Level,Npc[m_nNpcIdx].m_Series);
		if(Npc[m_nNpcIdx].m_SkillList.SetNpcSkill(defNPC_GOLD_SKILL_NO,pInfo->m_dwSkill5ID,pInfo->m_szSkill5Level))
		    Npc[m_nNpcIdx].SetAuraSkill(pInfo->m_dwSkill5ID); //光环技能
        //printf("[-------------------------------------------------------------------]\n");
		
	}
#endif
	// 按比例放大
	Npc[m_nNpcIdx].m_Experience					*= pInfo->m_nExp;   //模板经验比例
	Npc[m_nNpcIdx].m_CurrentLifeMax				*= pInfo->m_nLife;  //模板倍数
	Npc[m_nNpcIdx].m_CurrentLifeReplenish		*= pInfo->m_nLifeReplenish;  //生命恢复速度
	Npc[m_nNpcIdx].m_CurrentAttackRating		*= pInfo->m_nAttackRating;  //命中
	Npc[m_nNpcIdx].m_CurrentDefend				*= pInfo->m_nDefense;   //闪避点
	Npc[m_nNpcIdx].m_PhysicsDamage.nValue[0]	*= pInfo->m_nMinDamage; //最小物理伤害
	Npc[m_nNpcIdx].m_PhysicsDamage.nValue[2]	*= pInfo->m_nMaxDamage; //最大物理伤害
	
	// 加点数
	Npc[m_nNpcIdx].m_CurrentTreasure	+= pInfo->m_nTreasure;   //掉落装备的数量
	Npc[m_nNpcIdx].m_CurrentWalkSpeed	+= pInfo->m_nWalkSpeed;
	Npc[m_nNpcIdx].m_CurrentRunSpeed	+= pInfo->m_nRunSpeed;
	Npc[m_nNpcIdx].m_CurrentAttackSpeed	+= pInfo->m_nAttackSpeed;  //外功速度
	Npc[m_nNpcIdx].m_CurrentCastSpeed	+= pInfo->m_nCastSpeed;    //内攻速
    //设置满血
	//if (Npc[m_nNpcIdx].m_CurrentLifeMax>15000000)
	//   Npc[m_nNpcIdx].m_CurrentLifeMax = 15000000;

	Npc[m_nNpcIdx].m_CurrentLife = Npc[m_nNpcIdx].m_CurrentLifeMax;
#ifdef _SERVER
	if (Npc[m_nNpcIdx].m_Kind==kind_normal)
	{
	        //蓝怪 或黄金怪的爆
		char nGDropFile[128],nMapidB[32];
			ZeroMemory(nGDropFile,128);
			ZeroMemory(nMapidB,32);
			snprintf(nMapidB, sizeof(nMapidB),"%d_GoldenDropRate",SubWorld[nSubWorld].m_SubWorldID);
			g_NpcMapDropRate.GetString("List",nMapidB,"",nGDropFile,sizeof(nGDropFile));
			int nSet=0;
			    nSet=CheckThisNpc(Npc[m_nNpcIdx].m_NpcSettingIdx,"NpcsetID"); //检查是否在禁止列表中
			if (!nSet)
			{
			   if (this->GetGoldType()>1 && this->GetGoldType()<=16 && nGDropFile[0])
			   {  
				  /*if (g_pNpcTemplate[Npc[m_nNpcIdx].m_NpcSettingIdx][0])
				     g_pNpcTemplate[Npc[m_nNpcIdx].m_NpcSettingIdx][0]->InitDropRate(m_nNpcIdx,nGDropFile);  //蓝怪爆率修改	  
				  else  if (g_pNpcTemplate[Npc[m_nNpcIdx].m_NpcSettingIdx][Npc[m_nNpcIdx].m_Level])
					 g_pNpcTemplate[Npc[m_nNpcIdx].m_NpcSettingIdx][Npc[m_nNpcIdx].m_Level]->InitDropRate(m_nNpcIdx,nGDropFile);  //蓝怪爆率修改
				   */
				   KNpcTemplate* pNpcTemp = NULL;
				   if (pNpcTemp = NpcSet.GetTemplate(Npc[m_nNpcIdx].m_NpcSettingIdx, 0))
					   pNpcTemp->InitDropRate(m_nNpcIdx,nGDropFile);	
				   else if (pNpcTemp = NpcSet.GetTemplate(Npc[m_nNpcIdx].m_NpcSettingIdx, Npc[m_nNpcIdx].m_Level))
					   pNpcTemp->InitDropRate(m_nNpcIdx,nGDropFile);
			   }
			}
			//else
			//  Npc[m_nNpcIdx].m_pDropRate=NULL;
	}
#endif

#ifdef _SERVER
	// 向周围九屏广播
	NPC_GOLD_CHANGE_SYNC	sSync;
	sSync.ProtocolType = s2c_npcgoldchange;
	sSync.m_dwNpcID = Npc[this->m_nNpcIdx].m_dwID;
	sSync.m_wGoldFlag = m_nGoldType; //m_nGoldType + 1
	Npc[m_nNpcIdx].SendDataToNearRegion((LPVOID)&sSync, sizeof(NPC_GOLD_CHANGE_SYNC));
#endif
}

#ifndef _SERVER
void	KNpcGold::ClientClearState()
{
	m_nIsGold		= 0;
	m_nIsGolding	= 0;
}
#endif

#ifdef _SERVER

BOOL KNpcGold::CheckThisNpc(int nSetings,char *nKey)
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

#endif

//------------------------------------------------------------------
KNpcGoldTemplate::KNpcGoldTemplate()
{
	memset(this->m_sInfo, 0, sizeof(this->m_sInfo));
	this->m_nEffectTypeNum = 0;
}
//蓝怪的模板
BOOL	KNpcGoldTemplate::Init()
{
	KTabFile	cFile;

//	g_SetFilePath("\\");
	if (!cFile.Load(NPC_GOLD_TEMPLATE_FILE))
		return FALSE;

//	char	szTemp[80];
	int     nSkillID=0,nRows=0,i;
            nRows=cFile.GetHeight()-1;
	for (i = 0; i <nRows; ++i)
	{
        if (i>=defMAX_NPC_GOLD_TYEP)  
			break;
		cFile.GetInteger(i + 2, "Exp", 1, &m_sInfo[i+1].m_nExp);
		cFile.GetInteger(i + 2, "Life", 1, &m_sInfo[i+1].m_nLife);
		cFile.GetInteger(i + 2, "LifeReplenish", 1, &m_sInfo[i+1].m_nLifeReplenish);
		cFile.GetInteger(i + 2, "AttackRating", 1, &m_sInfo[i+1].m_nAttackRating);
		cFile.GetInteger(i + 2, "Defense", 1, &m_sInfo[i+1].m_nDefense);
		cFile.GetInteger(i + 2, "MinDamage", 1, &m_sInfo[i+1].m_nMinDamage);
		cFile.GetInteger(i + 2, "MaxDamage", 1, &m_sInfo[i+1].m_nMaxDamage);

		cFile.GetInteger(i + 2, "Treasure", 0, &m_sInfo[i+1].m_nTreasure);
		cFile.GetInteger(i + 2, "WalkSpeed", 0, &m_sInfo[i+1].m_nWalkSpeed);
		cFile.GetInteger(i + 2, "RunSpeed", 0, &m_sInfo[i+1].m_nRunSpeed);
		cFile.GetInteger(i + 2, "AttackSpeed", 0, &m_sInfo[i+1].m_nAttackSpeed);
		cFile.GetInteger(i + 2, "CastSpeed", 0, &m_sInfo[i+1].m_nCastSpeed);

	//	cFile.GetString(i + 2, 14, "", szTemp, sizeof(szTemp));
	//	m_sInfo[i].m_dwSkill5ID = KNpcTemplate::SkillString2Id(szTemp);
        cFile.GetInteger(i + 2, "SkillID",0, &m_sInfo[i+1].m_dwSkill5ID);
		cFile.GetInteger(i + 2, "SkillLevel",5,&m_sInfo[i+1].m_szSkill5Level);
		cFile.GetInteger(i + 2, "FireResist", 0, &m_sInfo[i+1].m_nFireResist);
		cFile.GetInteger(i + 2, "FireResistMax", 0, &m_sInfo[i+1].m_nFireResistMax);
		cFile.GetInteger(i + 2, "ColdResist", 0, &m_sInfo[i+1].m_nColdResist);
		cFile.GetInteger(i + 2, "ColdResistMax", 0, &m_sInfo[i+1].m_nColdResistMax);
		cFile.GetInteger(i + 2, "LightingResist", 0, &m_sInfo[i+1].m_nLightingResist);
		cFile.GetInteger(i + 2, "LightingResistMax", 0, &m_sInfo[i+1].m_nLightingResistMax);
		cFile.GetInteger(i + 2, "PoisonResist", 0, &m_sInfo[i+1].m_nPoisonResist);
		cFile.GetInteger(i + 2, "PoisonResistMax", 0, &m_sInfo[i+1].m_nPoisonResistMax);
		cFile.GetInteger(i + 2, "PhycicsResist", 0, &m_sInfo[i+1].m_nPhycicsResist);
		cFile.GetInteger(i + 2, "PhycicsResistMax", 0, &m_sInfo[i+1].m_nPhycicsResistMax);
		/*cFile.GetInteger(i + 2, "AiMode", 0, &m_sInfo[i+1].m_nAiMode);
		cFile.GetInteger(i + 2, "AiParam1", 0, &m_sInfo[i+1].m_nAiParam1);
		cFile.GetInteger(i + 2, "AiParam2", 0, &m_sInfo[i+1].m_nAiParam2);
		cFile.GetInteger(i + 2, "AiParam3", 0, &m_sInfo[i+1].m_nAiParam3);
		cFile.GetInteger(i + 2, "AiParam4", 0, &m_sInfo[i+1].m_nAiParam4);
		cFile.GetInteger(i + 2, "AiParam5", 0, &m_sInfo[i+1].m_nAiParam5);
		cFile.GetInteger(i + 2, "AiParam6", 0, &m_sInfo[i+1].m_nAiParam6);
		cFile.GetInteger(i + 2, "AiParam7", 0, &m_sInfo[i+1].m_nAiParam7);
		cFile.GetInteger(i + 2, "AiParam8", 0, &m_sInfo[i+1].m_nAiParam8);
		cFile.GetInteger(i + 2, "AiParam9", 0, &m_sInfo[i+1].m_nAiParam9);
		cFile.GetInteger(i + 2, "AiParam10", 0, &m_sInfo[i+1].m_nAiParam10);
		cFile.GetInteger(i + 2, "AiMaxTime", 0, &m_sInfo[i+1].m_nAiMaxTime);
		cFile.GetInteger(i + 2, "PhysicalDamageBase", 0, &m_sInfo[i+1].m_nPhysicalDamageBase);
		cFile.GetInteger(i + 2, "PhysicalMagicBase", 0, &m_sInfo[i+1].m_nPhysicalMagicBase);
		cFile.GetInteger(i + 2, "PoisonDamageBase", 0, &m_sInfo[i+1].m_nPoisonDamageBase);
		cFile.GetInteger(i + 2, "PoisonMagicBase", 0, &m_sInfo[i+1].m_nPoisonMagicBase);
		cFile.GetInteger(i + 2, "ColdDamageBase", 0, &m_sInfo[i+1].m_nColdDamageBase);
		cFile.GetInteger(i + 2, "ColdMagicBase", 0, &m_sInfo[i+1].m_nColdMagicBase);
		cFile.GetInteger(i + 2, "FireDamageBase", 0, &m_sInfo[i+1].m_nFireDamageBase);
		cFile.GetInteger(i + 2, "FireMagicBase", 0, &m_sInfo[i+1].m_nFireMagicBase);
		cFile.GetInteger(i + 2, "LightingDamageBase", 0, &m_sInfo[i+1].m_nLightingDamageBase);
		cFile.GetInteger(i + 2, "LightingMagicBase", 0, &m_sInfo[i+1].m_nLightingMagicBase);
		*/
		//cFile.GetString(i + 2, 48, "", m_sInfo[i+1].m_nDeathScript, sizeof(m_sInfo[i+1].m_nDeathScript)); //死亡脚本
        //cFile.GetString(i + 2, 49, "", m_sInfo[i+1].m_nDropRate,sizeof(m_sInfo[i+1].m_nDropRate));        //爆率
	    //cFile.GetInteger(i + 2, 50, 0, &m_sInfo[i+1].m_nRate);                                            //概率
		
	}
	this->m_nEffectTypeNum = i;                                                                             //模板 黄金怪物的类型数量 
	cFile.Clear();
    //printf(" KNpcGoldTemplate::Init(%d) OK!!!..\n",this->m_nEffectTypeNum);
	return TRUE;
}
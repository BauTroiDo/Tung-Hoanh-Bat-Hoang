#include "KCore.h"
#include <math.h>
#include "KNpc.h"
#include "KSubWorld.h"
#include "KRegion.h"
#include "GameDataDef.h"
#include "KNpcSet.h"
#include "KPlayer.h"
#include "KNpcTemplate.h"  //原来没有的
#ifndef _SERVER
#include "CoreShell.h"
#include "Scene/KScenePlaceC.h"
#include "../../Represent/iRepresent/iRepresentshell.h"
#include "KOption.h"
#ifndef TOOLVERSION
#include "../../../Headers/IClient.h"
#endif
#endif
//#include "Scene/SceneDataDef.h"
#include "KMath.h"



//#define	ENCHANT_SETTING_PATH	"settings\\npc"
//#define	ENCHANT_NORMAL_FILE		"normalunique.txt"
//#define	ENCHANT_SPECIAL_FILE	"speicalunique.txt"
extern KLuaScript		*g_pNpcLevelScript;

KNpcSet	NpcSet;

KNpcSet::KNpcSet()
{
	m_dwIDCreator = 1; //默认是1000起
	Release();
}

BOOL KNpcSet::Release(void)
{
	/*for (_KMapTemplate::iterator it = m_mapTemplate.begin(); it != m_mapTemplate.end(); ++it)
	{
		//SAFE_DELETE(it->second);
		if (it->second)
		{
			delete it->second;
			it->second = NULL;
		}
	}*/
	m_mapTemplate.clear();
	return TRUE;
}

// 黄金怪物设定文件的初始化没有放在这里，直接放在 core 的初始化里面
void KNpcSet::Init()
{
    GenRelationTable();  //初始化 敌友关系

	m_FreeIdxNpcSet.Init(MAX_NPC);  //MAX_NPC 分配内存
	m_UseIdxNpcSet.Init(MAX_NPC);
	int i;
	// 开始时所有的数组元素都为空   初始化内存数组
	for (i = MAX_NPC - 1; i > 0; i--) // 除0外
	{
		m_FreeIdxNpcSet.Insert(i);
		Npc[i].m_Node.m_nIndex = i;     //节点的索引
#ifdef _SERVER
	    Npc[i].m_cDeathCalcExp.Init(i); //清空经验
#endif
		Npc[i].m_cGold.Init(i);         //黄金怪物初始化
	}

	//if (!m_cTabFile.Load(NPC_SETTING_FILE))
	//	return;
	
	/*INT nNpcTemplateHeight = g_NpcSetting.GetHeight();
	INT nNpcTemplateId;
	ZeroStruct(m_anTemplateRowId);
	for (INT nRowId	= 2; nRowId <= nNpcTemplateHeight; ++nRowId)
	{
		//m_cTabFile.GetInteger(nRowId, "Id", 0, &nNpcTemplateId);
		nNpcTemplateId =  nRowId-1;
		if (nNpcTemplateId <= 0 || nNpcTemplateId > MAX_NPCSTYLE)
		{
			_ASSERT(FALSE);
			continue;
		} 
		
		if (nNpcTemplateId > 0 && nNpcTemplateId <= MAX_NPCSTYLE)
			m_anTemplateRowId[nNpcTemplateId]	= nRowId; //行号
	} */

	LoadPlayerBaseValue(PLAYER_BASE_VALUE);

#ifdef _SERVER
	KIniFile	cPKIni;
//	g_SetFilePath("\\");
	if (cPKIni.Load(PLAYER_PK_RATE_FILE))
	{
		cPKIni.GetInteger("PK", "rate", 20, &m_nPKDamageRate);
		cPKIni.GetInteger("PK", "FactionPKFaction", 1, &m_nFactionPKFactionAddPKValue);
		cPKIni.GetInteger("PK", "KillerPKFaction", 1, &m_nKillerPKFactionAddPKValue);
		cPKIni.GetInteger("PK", "EnmityPK", 2, &m_nEnmityAddPKValue);
		cPKIni.GetInteger("PK", "BeKilled", -2, &m_nBeKilledAddPKValue);
		//cPKIni.GetInteger("PK", "LevelDistance", 25, &m_nLevelDistance);
	}
	else
	{
		m_nPKDamageRate = 20;
		m_nFactionPKFactionAddPKValue = 1;
		m_nKillerPKFactionAddPKValue = 1;
		m_nEnmityAddPKValue = 2;
		m_nBeKilledAddPKValue = -2;
		//m_nLevelDistance = 25;
	}
	cPKIni.Clear();
#endif

#ifndef _SERVER
	m_nShowPateFlag = PATE_CHAT;
	ZeroMemory(m_RequestNpc, sizeof(m_RequestNpc));
	m_RequestFreeIdx.Init(MAX_NPC_REQUEST);  //最大的NPC数量请求
	m_RequestUseIdx.Init(MAX_NPC_REQUEST);

	for (i = MAX_NPC_REQUEST - 1; i > 0; i--)
	{
		m_RequestFreeIdx.Insert(i);
	}
#endif
	//printf(" KNpcSet::Init(%d)(%d)(%d) OK..!!!!\n",MAX_NPC,m_FreeIdxNpcSet.GetCount(),m_FreeIdxNpcSet.GetNext(0));
	
	g_GameOption.GetFloat("QUAI", "tglc", 0, &m_nTglc);
	g_GameOption.GetFloat("QUAI", "tgtd", 0, &m_nTgtd);
	g_GameOption.GetFloat("QUAI", "tgc", 0, &m_nTgc);
}

void KNpcSet::LoadPlayerBaseValue(LPSTR szFile)
{
	KIniFile	File;

	File.Load(szFile);

	File.GetInteger("Common", "HurtFrame", 12, &m_cPlayerBaseValue.nHurtFrame);
	File.GetInteger("Common", "RunSpeed", 10, &m_cPlayerBaseValue.nRunSpeed);
	File.GetInteger("Common", "WalkSpeed", 5, &m_cPlayerBaseValue.nWalkSpeed);
	File.GetInteger("Common", "AttackFrame", 20, &m_cPlayerBaseValue.nAttackFrame);
#ifndef _SERVER
	File.GetInteger("Male", "WalkFrame", 15, &m_cPlayerBaseValue.nWalkFrame[0]);
	File.GetInteger("Female", "WalkFrame", 15, &m_cPlayerBaseValue.nWalkFrame[1]);
	File.GetInteger("Male", "RunFrame", 15, &m_cPlayerBaseValue.nRunFrame[0]);
	File.GetInteger("Female", "RunFrame", 15, &m_cPlayerBaseValue.nRunFrame[1]);
	File.GetInteger("Male", "StandFrame", 15, &m_cPlayerBaseValue.nStandFrame[0]);
	File.GetInteger("Female", "StandFrame", 15, &m_cPlayerBaseValue.nStandFrame[1]);
#endif
	File.Clear();
}
//是否是这个NPC
BOOL KNpcSet::IsNpcExist(int nIdx, DWORD dwId)
{
	if (Npc[nIdx].m_dwID == dwId)
		return TRUE;
	else
		return FALSE;
}
BOOL	KNpcSet::IsIdxValid(INT nIdx){ 
	return nIdx > 0 && nIdx < MAX_NPC; 
}
// 获取双方的敌友关系
NPC_RELATION KNpcSet::GenOneRelation(NPCKIND Kind1, NPCKIND Kind2, NPCCAMP Camp1, NPCCAMP Camp2)
{
	// 路人NPC没有战斗关系
	if (Kind1 == kind_dialoger || Kind2 == kind_dialoger)
		return relation_dialog;
	//人与同伴 同伴与同伴 同伴与动物
	if ((Kind1 == kind_partner || Kind1 == kind_player) && Kind2 == kind_normal && Camp1!=Camp2)
		//Npc[Kind1].Partner._npcDwid
		return relation_enemy;//人与动物 同伴与动物  阵营不同的
	if (Kind1 == kind_normal && (Kind2 == kind_partner || Kind2 == kind_player) && Camp1!=Camp2)
		return relation_enemy;//动物与同伴 动物与人 阵营不同的
	if ((Kind1 == kind_normal &&  Kind2 == kind_partner) || (Kind1 == kind_partner &&  Kind2 == kind_normal))
		return relation_enemy;//同伴与动物  或  动物与同伴 
	if ((Kind1 == kind_partner &&  Kind2 == kind_partner) && Camp1!=Camp2)
		return relation_enemy;//两个同伴  阵营不同的
	
	if (Kind1 >= kind_bird || Kind2 >= kind_bird)
		return relation_none;

	// Khong duoc danh NPC thuong neu no thuoc camp_event
	if ((Kind1 == kind_player && Kind2 == kind_normal && Camp2 == camp_event) ||
		(Kind2 == kind_player && Kind1 == kind_normal && Camp1 == camp_event))
		return relation_none;

	// 路人阵营没有战斗关系
	if (Camp1 == camp_event || Camp2 == camp_event)
		return relation_none;

	// 新手和动物还是战斗关系
	if ((Camp1 == camp_begin && Camp2 == camp_animal)
		||(Camp1 == camp_animal && Camp2 == camp_begin))
		return relation_enemy;

	// 只要有一个新手，就不存在战斗关系(是同盟关系，大家帮新手)
	if (Camp1 == camp_begin || Camp2 == camp_begin)
		return relation_ally;

	// 两个都是玩家
	if (Kind1 == kind_player && Kind2 == kind_player)
	{
		// 只要有一个玩家开了PK，就一定存在战斗关系
		if (Camp1 == camp_free || Camp2 == camp_free)
		{
			return relation_enemy;
		}
	}
	// 同阵营为伙伴关系
	if (Camp1 == Camp2)
		return relation_ally;

	// 其他情况为战斗关系
	return relation_enemy;

	//// 路人NPC没有战斗关系
	//if (Kind1 == kind_dialoger || Kind2 == kind_dialoger)  //任意一方为 对话NPC 都是 对话关系
	//	return relation_dialog;
	//if ((Kind1 == kind_partner|| Kind1 == kind_player) && Kind2 == kind_normal && Camp1!=Camp2)
	//	return relation_enemy;
	//if (Kind1 == kind_normal && (Kind2 == kind_partner || Kind2 == kind_player) && Camp1!=Camp2)
	//	return relation_enemy;
 //   if ((Kind1 == kind_normal &&  Kind2 == kind_partner) || (Kind1 == kind_partner &&  Kind2 == kind_normal))
	//    return relation_enemy;
 //   if ((Kind1 == kind_partner &&  Kind2 == kind_partner) && Camp1!=Camp2)
	//    return relation_enemy;

	//// 路人阵营没有战斗关系
	//if (Camp1 == camp_event || Camp2 == camp_event)
	//	return relation_none;
	//
	//// 新手和动物还是战斗关系
	//if ((Camp1 == camp_begin && Camp2 == camp_animal)
	//	||(Camp1 == camp_animal && Camp2 == camp_begin))
	//	return relation_enemy;

	//// 只要有一个新手，就不存在战斗关系(是同盟关系，大家帮新手)
	//if (Camp1 == camp_begin || Camp2 == camp_begin)
	//	return relation_ally;
	//
 //   // 两个都是玩家
	//if (Kind1 == kind_player && Kind2 == kind_player)
	//{
	//	if (Camp1 == camp_free || Camp2 == camp_free)  //杀手阵营
	//	{
	//		return relation_enemy;
	//	}
	//}

	//// 同阵营为伙伴关系
	//if (Camp1 == Camp2) //同阵营为伙伴关系
	//	return relation_ally;

	//// 其他情况为战斗关系
	//return relation_enemy;


}


 //Add by Freeway Chen in 2003.7.14
int KNpcSet::GenRelationTable()
{
	int nKind1 = 0;
	int nKind2 = 0;
	int nCamp1 = 0;
	int nCamp2 = 0;

	for (nKind1 = 0; nKind1 < kind_num; ++nKind1)
	{
		for (nKind2 = 0; nKind2 < kind_num; ++nKind2)
		{
			for (nCamp1 = 0; nCamp1 < camp_num; ++nCamp1)
			{
				for (nCamp2 = 0; nCamp2 < camp_num; ++nCamp2)
				{
					m_RelationTable[nKind1][nKind2][nCamp1][nCamp2] = GenOneRelation(
						(NPCKIND)nKind1, 
						(NPCKIND)nKind2, 
						(NPCCAMP)nCamp1, 
						(NPCCAMP)nCamp2
						);
				}
			}
		}

	}

	return true;

} 


int	KNpcSet::SearchByTongName(LPSTR szName)
{

	return 0;

}


//dwID 名字查找 返回索引
int	 KNpcSet::SearchID(DWORD dwID)
{
	int nIdx = 0;
	while (1)
	{
		nIdx = m_UseIdxNpcSet.GetNext(nIdx);
		if (nIdx == 0)
			break;
		if (Npc[nIdx].m_dwID == dwID)  //唯一的标示
			return nIdx;
	}
	return 0;
}

#ifndef _SERVER
//---------------------------------------------------------------------------
//	功能：查找某个ClientID的npc是否存在
//---------------------------------------------------------------------------
int		KNpcSet::SearchClientID(KClientNpcID sClientID)
{
	int nIdx = 0;
	while (1)
	{
		nIdx = m_UseIdxNpcSet.GetNext(nIdx);
		if (nIdx == 0)
			break;
		if (Npc[nIdx].m_sClientNpcID.m_dwRegionID == sClientID.m_dwRegionID &&
			Npc[nIdx].m_sClientNpcID.m_nNo == sClientID.m_nNo)
			return nIdx;
	}

	return 0;
}
#endif

//名字查找 返回索引
int KNpcSet::SearchName(LPSTR szName,int _kind)
{
	if (!szName) return 0;
	int nlen2 = strlen(szName);

    int nIdx = 0;
    while (1)
    {
        nIdx = m_UseIdxNpcSet.GetNext(nIdx);
        if (nIdx == 0)
            break;
		/*int nlen1 =strlen(Npc[nIdx].Name);
		MessageBox(G2U(Npc[nIdx].Name).c_str(), "SearchName");
		if (nlen1 == nlen2 && strstr(Npc[nIdx].Name, szName))*/
		if (g_StrCmp(Npc[nIdx].Name, szName))
		{
			if (_kind == -1)
				return nIdx;
			else if (Npc[nIdx].m_Kind == _kind)
				return nIdx;
		}
		
    }
    return 0;
}

int KNpcSet::SearchGSdwIdInMap(DWORD dwID,int inMapidx)
{
	int nIdx = 0,nCount=0;
	while (1)
	{
		nIdx = m_UseIdxNpcSet.GetNext(nIdx);
		if (nIdx == 0)
			break;

		if (Npc[nIdx].m_Kind==kind_normal && Npc[nIdx].m_GuiShuDwid ==dwID  && SubWorld[Npc[nIdx].m_SubWorldIndex].m_SubWorldID==inMapidx)
			nCount ++;
	}
	return nCount;
}

//名字查找 返回索引
int KNpcSet::SearchNameInMap(LPSTR szName,int inMapidx,int nType)
{

	int nIdx = 0,nCount=0;
	while (1)
	{
		nIdx = m_UseIdxNpcSet.GetNext(nIdx);
		if (nIdx == 0)
			break;
		if (g_StrCmp(Npc[nIdx].Name,szName) && SubWorld[Npc[nIdx].m_SubWorldIndex].m_SubWorldID==inMapidx)
		{
		  	nCount ++;
		    if (nType==1)
			   return nIdx;
		}
			//return nIdx;
	}
	return nCount;
}


//获取NPC setings
int KNpcSet::SearchNpcSettingID(LPSTR szName)
{
	int nIdx = 0;
	while (1)
	{
		nIdx = m_UseIdxNpcSet.GetNext(nIdx);
		if (nIdx == 0)
			break;
		if (g_StrCmp(Npc[nIdx].Name,szName))
			return Npc[nIdx].m_NpcSettingIdx;
	}
	return 0;
}

int KNpcSet::DelNpcInNpcSet(int nNpcSettings)
{
	int nIdx = 0;
	int nCount=0;
	while (1)
	{
		nIdx = m_UseIdxNpcSet.GetNext(nIdx);
		if (nIdx == 0)
			break;
		if (Npc[nIdx].m_NpcSettingIdx==nNpcSettings)
		{
             Remove(nIdx);
			 Npc[nIdx].m_Node.Remove();
			 nCount++;
			//return Npc[nIdx].m_NpcSettingIdx;
		}
	}
	return nCount;
}

int  KNpcSet::SearchGSdwId(DWORD dwID)
{
	int nIdx = 0,nCount=0;
	while (1)
	{
		nIdx = m_UseIdxNpcSet.GetNext(nIdx);
		if (nIdx == 0)
			break;
		if (Npc[nIdx].m_GuiShuDwid==dwID)
			return nIdx;
	}
	return nIdx;
}
//查归属名
int KNpcSet::SearchGSName(LPSTR szName)
{
	int nIdx = 0,nCount=0;
	while (1)
	{
		nIdx = m_UseIdxNpcSet.GetNext(nIdx);
		if (nIdx == 0)
			break;
		char nstrName[64]={0};
		Npc[nIdx].GetstrInfo(STR_GUISHU_NAME,nstrName,sizeof(nstrName));
		if (g_StrCmp(nstrName,szName))
			//nCount ++;
			return nIdx;
	}
	return nIdx;
}

//dwID 名字查找
int KNpcSet::SearchNameID(DWORD dwID)
{
	int nIdx = 0;
	DWORD	dwNameID = 0;
	while(1)
	{
		nIdx = m_UseIdxNpcSet.GetNext(nIdx);
		if (nIdx == 0)
			break;
		if (Npc[nIdx].m_Kind != kind_player)   //查找玩家ID  不是玩家的就不查找了
			continue;
		dwNameID = g_FileName2Id(Npc[nIdx].Name);
		if (dwID == dwNameID)
			return nIdx;
	}
	return 0;
}


int KNpcSet::FindFreeNpcSet()
{
	return m_FreeIdxNpcSet.GetNext(0);
}

#ifndef _SERVER
//---------------------------------------------------------------------------
//	功能：添加一个客户端npc（需要设定ClientNpcID）
//---------------------------------------------------------------------------
int		KNpcSet::AddClientNpc(int nTemplateID, int nRegionX, int nRegionY, int nMpsX, int nMpsY, int nNo)
{
	int		nNpcNo, nNpcSettingIdxInfo, nMapX, nMapY, nOffX, nOffY, nRegion;

	nNpcSettingIdxInfo = MAKELONG(1, nTemplateID);
	SubWorld[0].Mps2Map(nMpsX, nMpsY, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);
	if (nRegion < 0)
		return 0;

	nNpcNo = this->AddServerNpcB(nNpcSettingIdxInfo, 0, nRegion, nMapX, nMapY, nOffX, nOffY);

	if (nNpcNo > 0)
	{
		Npc[nNpcNo].m_sClientNpcID.m_dwRegionID = MAKELONG(nRegionX, nRegionY);
		Npc[nNpcNo].m_sClientNpcID.m_nNo = nNo;
		Npc[nNpcNo].m_RegionIndex = nRegion;
		Npc[nNpcNo].m_dwRegionID = SubWorld[0].m_Region[nRegion].m_RegionID;
		Npc[nNpcNo].m_bClientOnly = TRUE;
		Npc[nNpcNo].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
		SubWorld[0].m_Region[nRegion].DecNpcRef(Npc[nNpcNo].m_MapX, Npc[nNpcNo].m_MapY);
	}

	return nNpcNo;
}
#endif
//地图刷NPC
int KNpcSet::AddMapNpc(int nSubWorld, void* pNpcInfo,KMapList* pMapListInfo)
{//nSubWorld 为 索引 
	if (NULL==pNpcInfo || NULL==pMapListInfo) return false;

	KSPNpc*	pKSNpcInfo = (KSPNpc *)pNpcInfo;

	int nMpsX = pKSNpcInfo->nPositionX;
	int nMpsY = pKSNpcInfo->nPositionY;
	int	 nNpcSettingIdxInfo = MAKELONG(pKSNpcInfo->nLevel, pKSNpcInfo->nTemplateID);
	int nRet = AddServerNpcA(nNpcSettingIdxInfo, nSubWorld, nMpsX, nMpsY,0,0,0,0,0,0);
	if (nRet)
	{
		Npc[nRet].m_TrapScriptID = 0;

		g_StrCpyLen(Npc[nRet].Name,pKSNpcInfo->szName, sizeof(Npc[nRet].Name)); //按照字节数 复制名字
		//printf("////////////////%s --- nGameVersion %d\n", Npc[nRet].Name, nGameVersion);
		char nTempName[64];
		ZeroMemory(nTempName,sizeof(nTempName));
		//g_NpcChangeName.GetString(pKSNpcInfo->nTemplateID+2,2,"",nTempName,sizeof(nTempName));

		char nKey[64];
		if (nGameVersion!=1 && Npc[nRet].Name[0])
		{//不是中文版本就替换名字
			g_NpcChangeName.GetStringForCoding(Npc[nRet].Name,2,"Name_Error",nTempName,sizeof(nTempName),0,7);
//			printf("///*********************%s ------------ %s\n", Npc[nRet].Name, nTempName);
			if (nTempName[0])  
			{
				g_StrCpyLen(Npc[nRet].Name, nTempName, sizeof(Npc[nRet].Name));  //sizeof(Npc[nRet]._Name
				//printf("///*********************%s\n", Npc[nRet].Name);
			}
		}

		//修改NPC的为地图默认数据
		/*Npc[nRet].m_Camp        = pKSNpcInfo->cCamp;    //阵营
		  Npc[nRet].m_CurrentCamp = pKSNpcInfo->cCamp;    //阵营
		*/

        /*if (SubWorld[nSubWorld].m_SubWorldID==42)
		{
			printf("--地图%d,刷怪:%s 成功---\n",SubWorld[nSubWorld].m_SubWorldID,Npc[nRet].Name);
		}*/

		Npc[nRet].m_Series      = (BYTE)pKSNpcInfo->cSeries;  //五行
		if (pKSNpcInfo->shKind >= kind_normal && pKSNpcInfo->shKind < kind_num)
		   Npc[nRet].m_Kind     = pKSNpcInfo->shKind;     //种类

		if (pKSNpcInfo->cCamp >= camp_begin && pKSNpcInfo->cCamp < camp_num)
		{
			Npc[nRet].m_Camp        = pKSNpcInfo->cCamp;    //阵营
			Npc[nRet].m_CurrentCamp = pKSNpcInfo->cCamp;    //阵营
		}

		char nMapNpc[32]={0};
		int  nIsCreat=0;
		ZeroMemory(nMapNpc,sizeof(nMapNpc));
		sprintf(nMapNpc,"%d_IsScript",SubWorld[nSubWorld].m_SubWorldID);
		g_NpcMapDropRate.GetInteger("List",nMapNpc,0,&nIsCreat);
		Npc[nRet].IsExeGoldScript = nIsCreat; 
		sprintf(nMapNpc,"%d_IsCreat",SubWorld[nSubWorld].m_SubWorldID);
		g_NpcMapDropRate.GetInteger("List",nMapNpc,0,&nIsCreat);//默认执行全局脚本
		Npc[nRet].IsCreatBoss     = nIsCreat; 

		// 如果是黄金怪物，备份数据
		if (pKSNpcInfo->bSpecialNpc) //特殊NPC
			Npc[nRet].m_cGold.SetGoldTypeAndBackData();
		else
			Npc[nRet].m_cGold.SetGoldType(FALSE);

#ifdef _SERVER //开始加载爆率
		if (Npc[nRet].m_Kind==kind_normal)
		{
			int nSet=0;
			    nSet=CheckThisNpc(Npc[nRet].m_NpcSettingIdx,"NpcsetID"); //检查是否在禁止列表中

			if (!nSet)
			{
				KNpcTemplate* pNpcTemp = NULL;
				if (pNpcTemp = NpcSet.GetTemplate(Npc[nRet].m_NpcSettingIdx, 0))
				{
					if (Npc[nRet].m_cGold.GetGoldType()>1 && Npc[nRet].m_cGold.GetGoldType()<=4 && pKSNpcInfo->nGDropFile[0])
						pNpcTemp->InitDropRate(nRet,pKSNpcInfo->nGDropFile);  //蓝怪绿怪爆率修改
					else if (Npc[nRet].m_cGold.GetGoldType()==0 && pKSNpcInfo->nNDropFile[0])
						pNpcTemp->InitDropRate(nRet,pKSNpcInfo->nNDropFile);  //白怪爆率修改	  
				}
				else if (pNpcTemp = NpcSet.GetTemplate(Npc[nRet].m_NpcSettingIdx, Npc[nRet].m_Level))
				{
					if (Npc[nRet].m_cGold.GetGoldType()>1 && Npc[nRet].m_cGold.GetGoldType()<=4 && pKSNpcInfo->nGDropFile[0])
						pNpcTemp->InitDropRate(nRet,pKSNpcInfo->nGDropFile);  //蓝怪绿怪爆率修改
					else if (Npc[nRet].m_cGold.GetGoldType()==0 && pKSNpcInfo->nNDropFile[0])
						pNpcTemp->InitDropRate(nRet,pKSNpcInfo->nNDropFile);  //白怪爆率修改	  
				} 

			   //--------------------开始转变 加强怪物
			   int npMapGoldRate=0;
			   char nMapGoldRate[32]={0};
			   snprintf(nMapGoldRate, sizeof(nMapGoldRate),"%d_AutoGoldenNpc",SubWorld[nSubWorld].m_SubWorldID);  //38_AutoGoldenNpc
			   g_NpcMapDropRate.GetInteger("List",nMapGoldRate,0,&npMapGoldRate);//生成加强怪物的概率
			   
			   if (npMapGoldRate<0 &&  Npc[nRet].m_cGold.GetGoldType()<=0)
				   Npc[nRet].m_cGold.RandChangeGold(1,0,nSubWorld);
			   //
			  char pMapIdx[32]={0},nReviveFile[128]={0};//phuc sinh quai
			  ZeroMemory(nReviveFile,sizeof(nReviveFile));
			  snprintf(pMapIdx, sizeof(pMapIdx),"%d_NpcReviveScript",SubWorld[nSubWorld].m_SubWorldID); 
			  g_NpcMapDropRate.GetString("List",pMapIdx,"",nReviveFile,sizeof(nReviveFile));
			  Npc[nRet].NpcExecuteScriptA(nReviveFile,"OnRevive", Npc[nRet].Name, "nNum", SubWorld[nSubWorld].m_SubWorldID, Npc[nRet].m_NpcSettingIdx, Npc[nRet].m_Index);

			}
			else
				Npc[nRet].m_pDropRate=NULL;

			//开始调整防御
		  if (pMapListInfo->nNpcDefense>0){
			Npc[nRet].m_CurrentFireResist    = pMapListInfo->nNpcDefense;	        // Npc的当前火抗性
			Npc[nRet].m_CurrentColdResist    = pMapListInfo->nNpcDefense;         // Npc的当前冰抗性
			Npc[nRet].m_CurrentPoisonResist  = pMapListInfo->nNpcDefense;	        // Npc的当前毒抗性
			Npc[nRet].m_CurrentLightResist   = pMapListInfo->nNpcDefense;	        // Npc的当前电抗性
			Npc[nRet].m_CurrentPhysicsResist = pMapListInfo->nNpcDefense;	        // Npc的当前物理抗性

			Npc[nRet].m_CurrentFireResistMax    = MAX_RESIST;	// Npc的当前最大火抗性
			Npc[nRet].m_CurrentColdResistMax    = MAX_RESIST;	// Npc的当前最大冰抗性
			Npc[nRet].m_CurrentPoisonResistMax  = MAX_RESIST;	// Npc的当前最大毒抗性
			Npc[nRet].m_CurrentLightResistMax   = MAX_RESIST;	// Npc的当前最大电抗性
			Npc[nRet].m_CurrentPhysicsResistMax = MAX_RESIST;	// Npc的当前最大物理抗性

			Npc[nRet].m_FireResist    = pMapListInfo->nNpcDefense;		   //原始的五防
			Npc[nRet].m_ColdResist    = pMapListInfo->nNpcDefense;
			Npc[nRet].m_PoisonResist  = pMapListInfo->nNpcDefense;
			Npc[nRet].m_LightResist   = pMapListInfo->nNpcDefense;
			Npc[nRet].m_PhysicsResist = pMapListInfo->nNpcDefense;

			if (Npc[nRet].m_CurrentFireResist>MAX_RESIST)
				Npc[nRet].m_CurrentFireResist=MAX_RESIST;
			if (Npc[nRet].m_CurrentColdResist>MAX_RESIST)
				Npc[nRet].m_CurrentColdResist=MAX_RESIST;
			if (Npc[nRet].m_CurrentPoisonResist>MAX_RESIST)
				Npc[nRet].m_CurrentPoisonResist=MAX_RESIST;
			if (Npc[nRet].m_CurrentLightResist>MAX_RESIST)
				Npc[nRet].m_CurrentLightResist=MAX_RESIST;
			if (Npc[nRet].m_CurrentPhysicsResist>MAX_RESIST)
				Npc[nRet].m_CurrentPhysicsResist=MAX_RESIST;

			if (Npc[nRet].m_CurrentFireResistMax>MAX_RESIST)
				Npc[nRet].m_CurrentFireResistMax=MAX_RESIST;
			if (Npc[nRet].m_CurrentColdResistMax>MAX_RESIST)
				Npc[nRet].m_CurrentColdResistMax=MAX_RESIST;
			if (Npc[nRet].m_CurrentPoisonResistMax>MAX_RESIST)
				Npc[nRet].m_CurrentPoisonResistMax=MAX_RESIST;
			if (Npc[nRet].m_CurrentLightResistMax>MAX_RESIST)
				Npc[nRet].m_CurrentLightResistMax=MAX_RESIST;
			if (Npc[nRet].m_CurrentPhysicsResistMax>MAX_RESIST)
				Npc[nRet].m_CurrentPhysicsResistMax=MAX_RESIST;
		  }
		  //设置生命
		  if (pMapListInfo->nNpcMaxLife>0){
			  Npc[nRet].m_CurrentLife    = pMapListInfo->nNpcMaxLife;			// Npc的当前生命
			  Npc[nRet].m_CurrentLifeMax = pMapListInfo->nNpcMaxLife;		// Npc的当前生命最大值
		  }
		  //设置经验倍率
		  if (pMapListInfo->nExpSale>0)
			   Npc[nRet].IsJinYan=pMapListInfo->nExpSale; //m_Experience

		}

		if (Npc[nRet].m_Kind==kind_dialoger)
		   Npc[nRet].m_CurrentLifeReplenish = 200000000;
	
#endif
		if (pKSNpcInfo->szScript[0]) //脚本不等空
		{  
		    int zVal=CheckThisNpc(Npc[nRet].m_NpcSettingIdx,"NoScript");//检查是否在禁止列表中
		    if (zVal)//在禁止列表中，就不加载脚本
			{
			   Npc[nRet].m_ActionScriptID = 0;
			   return nRet; 
			}

			char nMsg[256]={0};
			/*
			if (pKSNpcInfo->szScript[0] == '.')
				g_StrCpyLen(Npc[nRet].ActionScript, &pKSNpcInfo->szScript[1], sizeof(Npc[nRet].ActionScript));
			else
				g_StrCpyLen(Npc[nRet].ActionScript, pKSNpcInfo->szScript, sizeof(Npc[nRet].ActionScript));
			*/
			if (pKSNpcInfo->szScript[0] == '.')
				g_StrCpyLen(nMsg, &pKSNpcInfo->szScript[1], sizeof(nMsg));
			else
				g_StrCpyLen(nMsg, pKSNpcInfo->szScript, sizeof(nMsg));
			 // 保持小写，保证脚本对应关系

			_strlwr(nMsg);
	
			DWORD dwScriptId = g_FileName2Id(nMsg);
			/*KLuaScript * pScript=NULL;
			pScript = (KLuaScript* )g_GetScript(dwScriptId);

			if (pScript)
			{*/
				Npc[nRet].SetstrInfo(STR_ACTION_SCRIPT,nMsg);
				Npc[nRet].m_ActionScriptID =dwScriptId;
				/*	}
				else
				Npc[nRet].m_ActionScriptID =0;*/
			
		}
		else
			Npc[nRet].m_ActionScriptID = 0;
	}
	else
	{//产生怪物失败
		/*if (SubWorld[nSubWorld].m_SubWorldID==53)
		{
			printf("--地图%d,刷怪:%s 成功---\n",SubWorld[nSubWorld].m_SubWorldID,Npc[nRet].Name);
		}*/
    }
	return nRet;
}
//脚本刷NPC和 玩家的增加（addNPC）
int KNpcSet::AddServerNpcA(int nNpcSettingIdxInfo, int nSubWorld, int nMpsX, int nMpsY,int nBoss,int nRodom,int nOffwminx,int nOffwmaxx,int nOffhminy,int nOffhmaxy)
{
	int nRegion, nmMapX, nmMapY, nOffX, nOffY;
	if (nSubWorld < 0 || nSubWorld >=MAX_SUBWORLD)
		return 0;

	g_SubWorldSet.EnsureMapLoaded(nSubWorld);

    SubWorld[nSubWorld].Mps2Map(nMpsX, nMpsY, &nRegion, &nmMapX, &nmMapY, &nOffX, &nOffY);
	
	//POINT	ptLocal;
 //   ptLocal.x = nMpsX;
 //   ptLocal.y = nMpsY;
//	SubWorld[nSubWorld].GetFreeObjPos(ptLocal);           //获取周围没有障碍物的坐标(位置矫正)
	//SubWorld[nSubWorld].Mps2Map(ptLocal.x, ptLocal.y,&nRegion, &nmMapX, &nmMapY,&nOffX, &nOffY);
	
	if (nRegion < 0)
	{
	/*	SubWorld[nSubWorld].Mps2Map(nMpsX,nMpsY,&nRegion, &nmMapX, &nmMapY,&nOffX, &nOffY);
		if  (nRegion<0)
		{*/
			printf(" -------------AddServerNpcA(World: %d)(X:%d Y:%d)--error-------------- \n",SubWorld[nSubWorld].m_SubWorldID,nMpsX,nMpsY);
			return 0;
	////	}
	}

	int nIndexID=0;
        nIndexID=AddServerNpcB(nNpcSettingIdxInfo, nSubWorld, nRegion, nmMapX, nmMapY, nOffX, nOffY,nBoss);	
	
	if (nIndexID >0)
	{
	  // 如果是黄金怪物
	  if (nBoss>0 && Npc[nIndexID].m_Kind==kind_normal) //特殊NPC  g_NpcMapDropRate
	  {//if(!IsPlayer() && m_Kind!=kind_dialoger && m_Kind!=kind_partner)
		Npc[nIndexID].m_cGold.SetGoldCurrentType(nBoss,nSubWorld);
		//Npc[nIndexID].m_cGold.RandChangeGold(0,0,nBoss,nSubWorld);  //同步怪物类型 
	  }



//-----------------------------------------------------------------------------------------
#ifndef _SERVER  //普通NPC 和特殊NPC 通用
	  Npc[nIndexID].m_DataRes.Init(Npc[nIndexID].szNpcTypeName, &g_NpcResList);  //外观初始化
	  Npc[nIndexID].m_DataRes.SetAction(Npc[nIndexID].m_ClientDoing);            //设定外观行为对接
	  Npc[nIndexID].m_DataRes.SetRideHorse(Npc[nIndexID].m_bRideHorse);          //设定是否骑马接帧数
	  Npc[nIndexID].m_DataRes.SetArmor(Npc[nIndexID].m_ArmorType);               //衣服
	  Npc[nIndexID].m_DataRes.SetHelm(Npc[nIndexID].m_HelmType);                 //头部
	  Npc[nIndexID].m_DataRes.SetHorse(Npc[nIndexID].m_HorseType);               //马位置
	  Npc[nIndexID].m_DataRes.SetWeapon(Npc[nIndexID].m_WeaponType);             //武器
	  if (Npc[nIndexID].m_Kind==kind_player)
	  {  
		  //if  (Npc[nIndexID].m_PifengType>0)
	           Npc[nIndexID].m_DataRes.SetPifeng(Npc[nIndexID].m_PifengType);             //披风
		  //if  (Npc[nIndexID].m_ChiBangType>0)
	           Npc[nIndexID].m_DataRes.SetChiBang(Npc[nIndexID].m_ChiBangType);
	  }
#endif
//-----------------------------------------------------------------------------------------	
	}
	return nIndexID;	
}
//刷押镖NPC
int KNpcSet::Addyabiao(int nNpcSettingIdxInfo, int nSubWorld, int nMpsX, int nMpsY, IN int nSeries, IN int nComp,IN int nRindexid,int nBoss)
{
	int nRegion, nMapX, nMapY, nOffX, nOffY;
	if (nSubWorld < 0 || nSubWorld >= MAX_SUBWORLD)//MAX_SUBWORLD
		return 0;

	SubWorld[nSubWorld].Mps2Map(nMpsX, nMpsY, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);
	if (nRegion < 0)
		return 0;

	if  (nRindexid<=0 || nRindexid>=MAX_NPC || !Npc[nRindexid].IsPlayer()) return 0;

	int indexid = AddServerNpcB(nNpcSettingIdxInfo, nSubWorld, nRegion, nMapX, nMapY, nOffX, nOffY,-1);
	
	if (indexid >0 && nRindexid>0 )
	{   
		Npc[indexid].m_Camp       = Npc[nRindexid].m_Camp;
		Npc[indexid].m_CurrentCamp= Npc[nRindexid].m_Camp;    //阵营
		Npc[indexid].SetCamp(Npc[nRindexid].m_Camp);

		Npc[indexid].m_btRankFFId = 88;


		Npc[indexid].m_IsRevive   = g_FileName2Id(Npc[nRindexid].Name);
		Npc[indexid].m_GuiShuDwid = Npc[nRindexid].m_dwID;
		Npc[indexid].m_Kind       = kind_partner;

		Npc[indexid].m_bClientOnly = FALSE;
		char nPCname[NAME_LENGHT];
		ZeroMemory(nPCname, NAME_LENGHT);
		ZeroMemory(Npc[indexid].Name, NAME_LENGHT); // 清零
        g_StrCpyLen(nPCname,Npc[nRindexid].Name,sizeof(Npc[nRindexid].Name));
		strcat(nPCname,"的(镖车)");
        snprintf(Npc[indexid].Name, sizeof(Npc[indexid].Name),"%s",nPCname);

		Npc[indexid].Partner._Id = 88;
		Npc[indexid].Partner._npcDwid = Npc[nRindexid].m_dwID;
		Npc[indexid].Partner._npcIndex= nRindexid;
		Npc[indexid].Partner._roleNameDwid = Npc[indexid].m_IsRevive;		
		sprintf(Npc[indexid].Partner._roleName,"%s",Npc[nRindexid].Name);
		Npc[indexid].Partner._lockMap=0;
		Npc[indexid].Partner._damageRatio=1;  //伤害的倍率

		return indexid;
	}
	else
	{
	    return 0;
	}

}

//刷同伴NPC
int KNpcSet::AddTongBan(int nNpcSettingIdxInfo, int nSubWorld, int nMpsX, int nMpsY, IN int nSeries, IN int nComp,IN int nRindexid,int nBoss)
{
	int nRegion, nMapX, nMapY, nOffX, nOffY;
	if (nSubWorld < 0 || nSubWorld >= MAX_SUBWORLD)//MAX_SUBWORLD
		return 0;
	SubWorld[nSubWorld].Mps2Map(nMpsX, nMpsY, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);
	if (nRegion < 0)
		return 0;

	if  (nRindexid<=0 || nRindexid>=MAX_NPC || !Npc[nRindexid].IsPlayer()) return 0;

	int indexid = AddServerNpcB(nNpcSettingIdxInfo, nSubWorld, nRegion, nMapX, nMapY, nOffX, nOffY,-1);
	
	if (indexid >0 && nRindexid>0 )
	{   
		Npc[indexid].m_Camp            = Npc[nRindexid].m_Camp;
		Npc[indexid].m_CurrentCamp= Npc[nRindexid].m_Camp;
		Npc[indexid].SetCamp(Npc[nRindexid].m_Camp);

		Npc[indexid].m_btRankFFId = 99;
		Npc[indexid].m_GuiShuDwid = Npc[nRindexid].m_dwID;
		Npc[indexid].m_IsRevive   = g_FileName2Id(Npc[nRindexid].Name);  // 名字的转换码


		Npc[indexid].m_Kind       = kind_partner;
		Npc[indexid].m_bClientOnly = FALSE;

		Npc[indexid].Partner._Id = 99;
		Npc[indexid].Partner._npcDwid = Npc[nRindexid].m_dwID;
		Npc[indexid].Partner._npcIndex= nRindexid;
		Npc[indexid].Partner._roleNameDwid = Npc[indexid].m_IsRevive;		
		sprintf(Npc[indexid].Partner._roleName,"%s",Npc[nRindexid].Name);
		Npc[indexid].Partner._lockMap=0;
		Npc[indexid].Partner._damageRatio=1;  //伤害的倍率

		return indexid;
	}
	else
	{
		return 0;
	}
	
}
//刷怪
int KNpcSet::AddServerNpcB(int nNpcSettingIdxInfo, int nSubWorld, int nRegion, int nMapX, int nMapY, int nOffX /* = 0 */, int nOffY,int nBoss)
{
	int nNpcSettingIdx = (short)HIWORD(nNpcSettingIdxInfo);// >> 7; //除于128

    if  (nNpcSettingIdx>g_NpcSetting.GetHeight()-2) //NPC.txt 的行数
	{
		printf(" -------------(超出范围)增加NPC失败-------------- \n");
	    return 0;
	}

	int i = FindFreeNpcSet();
	if (i <= 0)
	{//查找空闲节点
		printf(" -------------NPC已经满员(刷怪失败) OK------------ \n");
	    return 0;
	}
#ifndef _SERVER
	Npc[i].m_sClientNpcID.m_dwRegionID = 0;
	Npc[i].m_sClientNpcID.m_nNo = -1;
	Npc[i].Remove();                       //初始化 删除外观？
#endif
	//int nNpcSettingIdx = (short)HIWORD(nNpcSettingIdxInfo);// >> 7; //除于128
	/*if (SubWorld[nSubWorld].m_SubWorldID==42)
	{
		printf("--地图%d,Npc[%d].Load():---\n",SubWorld[nSubWorld].m_SubWorldID,i);
	}*/

	int nLevel         = LOWORD(nNpcSettingIdxInfo);// & 0x7f; 
	Npc[i].Remove();
	Npc[i].m_Node.m_nIndex = i;
	Npc[i].m_cDeathCalcExp.Init(i);
	Npc[i].m_cGold.Init(i);
	Npc[i].m_Index= i; //节点的索引
	Npc[i].m_SkillList.m_nNpcIndex = i;
	Npc[i].Load(nNpcSettingIdx, nLevel,nSubWorld,nBoss);  //加载NPC信息等  人物类型 等
	Npc[i].m_SubWorldIndex = nSubWorld;
	Npc[i].m_RegionIndex   = nRegion;

	//van tieu
	ZeroMemory(Npc[i].m_NpcParam, sizeof(Npc[i].m_NpcParam));
	Npc[i].m_bNpcFollowFindPath = FALSE;
	Npc[i].m_nNpcTimeout = 0;
	Npc[i].m_uFindPathTime = 0;
	Npc[i].m_uFindPathMaxTime = 0;
	Npc[i].m_uLastFindPathTime = 0;

#ifndef _SERVER
	if (nRegion >= 0 && nRegion < 9)
		Npc[i].m_dwRegionID = SubWorld[nSubWorld].m_Region[nRegion].m_RegionID;
#endif
	Npc[i].m_MapX = nMapX;
	Npc[i].m_MapY = nMapY;
	Npc[i].m_OffX = nOffX;
	Npc[i].m_OffY = nOffY;

	SubWorld[nSubWorld].NewMap2Mps(nRegion, nMapX, nMapY, nOffX, nOffY, &Npc[i].m_OriginX, &Npc[i].m_OriginY);

#ifdef _SERVER
	SetID(i);  //i为index值设定NPC的DWid(全局)= 数量 第几个NPC
	Npc[i].m_cDeathCalcExp.Clear();     //清空经验
#endif
	// 修改可用与使用表
	m_FreeIdxNpcSet.Remove(i); //可用表删除一个
	m_UseIdxNpcSet.Insert(i);  //已用表增加一个
	SubWorld[nSubWorld].m_Region[nRegion].AddNpc(i);//增加节点m_WorldMessage.Send(GWM_NPC_ADD, nRegion, i);
	SubWorld[nSubWorld].m_Region[nRegion].AddRef(nMapX, nMapY, obj_npc);  //NPC 出现的位置
	//SubWorld[nSubWorld].m_Region[nRegion].AddNpcRef(nMapX,nMapY);

#ifndef _SERVER
	Npc[i].m_dwRegionID = SubWorld[nSubWorld].m_Region[nRegion].m_RegionID;
//---------------------------------------------------------------------------
	Npc[i].m_DataRes.Init(Npc[i].szNpcTypeName, &g_NpcResList);  //外观初始化
	Npc[i].m_DataRes.SetAction(Npc[i].m_ClientDoing);            //设定外观行为对接Npc[i].m_ClientDoing
	Npc[i].m_DataRes.SetRideHorse(Npc[i].m_bRideHorse);          //设定是否骑马接帧数
	Npc[i].m_DataRes.SetArmor(Npc[i].m_ArmorType);               //衣服
	Npc[i].m_DataRes.SetHelm(Npc[i].m_HelmType);                 //头部
	Npc[i].m_DataRes.SetHorse(Npc[i].m_HorseType);               //马位置
	Npc[i].m_DataRes.SetWeapon(Npc[i].m_WeaponType);             //武器
  if (Npc[i].m_Kind==kind_player)
  {  
    //if (Npc[i].m_PifengType>0)
	//   Npc[i].m_DataRes.SetPifeng(Npc[i].m_PifengType);             //披风
	  if (Npc[i].m_ChiBangType>0)
	    Npc[i].m_DataRes.SetChiBang(Npc[i].m_ChiBangType);           //翅膀
  }  
//---------------------------------------------------------------------------
#endif
	return i;
}

void KNpcSet::Remove(int nIdx,BOOL isNeedSyn)
{
	if (nIdx <= 0 || nIdx >= MAX_NPC)
		return;
#ifdef _SERVER
 if (isNeedSyn)
 {
	NPC_REMOVE_SYNC	NetCommand;

	NetCommand.ProtocolType = (BYTE)s2c_npcremove;
	NetCommand.ID = Npc[nIdx].m_dwID;
	NetCommand.Rv = TRUE;
	int nSubWorld = Npc[nIdx].m_SubWorldIndex;
	int nRegion = Npc[nIdx].m_RegionIndex;
	
	if (nSubWorld >= 0 && nSubWorld <MAX_SUBWORLD && nRegion >= 0 && nRegion<SubWorld[nSubWorld].m_nTotalRegion)
	{	
		POINT	POff[8] = 
		{
			{0, 32},
			{-16, 32},
			{-16, 0},
			{-16, -32},
			{0, -32},
			{16, -32},
			{16, 0},
			{16, 32},
		};
		int nMaxCount = MAX_BROADCAST_COUNT;
		SubWorld[nSubWorld].m_Region[nRegion].BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, Npc[nIdx].m_MapX, Npc[nIdx].m_MapY);
		
		int nConRegion;
		for (int i = 0; i < 8; ++i)
		{
			nConRegion = SubWorld[nSubWorld].m_Region[nRegion].m_nConnectRegion[i];
			if  (nConRegion<0 || nConRegion>SubWorld[nSubWorld].m_nTotalRegion)
				continue;
			SubWorld[nSubWorld].m_Region[nConRegion].BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, Npc[nIdx].m_MapX - POff[i].x, Npc[nIdx].m_MapY - POff[i].y);
		}	
	}
}
#endif
	Npc[nIdx].ClearNpcState();
	Npc[nIdx].m_SkillList.Clear();
	Npc[nIdx].Remove(); ////外观删除
	m_FreeIdxNpcSet.Insert(nIdx);
	m_UseIdxNpcSet.Remove(nIdx);
}

void KNpcSet::RemoveAll()
{
	int nIdx = m_UseIdxNpcSet.GetNext(0);
	int nIdx1 = 0;
	while(nIdx)
	{
		nIdx1 = m_UseIdxNpcSet.GetNext(nIdx);
		Npc[nIdx].ClearNpcState();
		Npc[nIdx].m_SkillList.Clear();
		Npc[nIdx].Remove();
		m_FreeIdxNpcSet.Insert(nIdx);
		m_UseIdxNpcSet.Remove(nIdx);
		nIdx = nIdx1;
	}
}

#ifndef _SERVER
//---------------------------------------------------------------------------
//	功能：从npc数组中寻找属于某个region的 client npc ，添加进去
//---------------------------------------------------------------------------
void	KNpcSet::InsertNpcToRegion(int nRegionIdx)
{
	if (nRegionIdx < 0 || nRegionIdx >= MAX_REGION)
		return;
	int nIdx = 0;
	while (1)
	{
		nIdx = m_UseIdxNpcSet.GetNext(nIdx);
		if (nIdx == 0)
			break;

		if (Npc[nIdx].m_sClientNpcID.m_dwRegionID > 0 && Npc[nIdx].m_dwRegionID == (DWORD)SubWorld[0].m_Region[nRegionIdx].m_RegionID)
		{
			SubWorld[0].m_Region[nRegionIdx].AddNpc(nIdx);
			Npc[nIdx].m_RegionIndex = nRegionIdx;
			Npc[nIdx].m_dwRegionID = SubWorld[0].m_Region[nRegionIdx].m_RegionID;
			Npc[nIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
			Npc[nIdx].SendSerCommand(do_stand);
		}
	}
}
#endif


void KNpcSet::SetID(int m_nIndex)
{
	if (m_nIndex <= 0 || m_nIndex >= MAX_NPC)
		return;
	Npc[m_nIndex].m_dwID = m_dwIDCreator;
	m_dwIDCreator++; //设定一个后自加一次
}

int KNpcSet::GetDistance(int nIdx1, int nIdx2) //获取场景的距离
{
	int	nRet = 0;
	if (Npc[nIdx1].m_SubWorldIndex != Npc[nIdx2].m_SubWorldIndex)
		return -1;

	if (Npc[nIdx1].m_RegionIndex == Npc[nIdx2].m_RegionIndex)
	{
		int XOff = (Npc[nIdx1].m_MapX - Npc[nIdx2].m_MapX) * SubWorld[Npc[nIdx1].m_SubWorldIndex].m_nCellWidth;
		XOff += ((Npc[nIdx1].m_OffX - Npc[nIdx2].m_OffX) >> 10);

		int YOff = (Npc[nIdx1].m_MapY - Npc[nIdx2].m_MapY) * SubWorld[Npc[nIdx1].m_SubWorldIndex].m_nCellHeight;
		YOff +=((Npc[nIdx1].m_OffY - Npc[nIdx2].m_OffY) >> 10);

		nRet = (int)sqrt(double(XOff * XOff) + YOff * YOff);
	}
	else
	{
		int X1, Y1;
		SubWorld[Npc[nIdx1].m_SubWorldIndex].NewMap2Mps(Npc[nIdx1].m_RegionIndex, 
			Npc[nIdx1].m_MapX,
			Npc[nIdx1].m_MapY,
			Npc[nIdx1].m_OffX,
			Npc[nIdx1].m_OffY,
			&X1,
			&Y1);
		int X2, Y2;
		SubWorld[Npc[nIdx2].m_SubWorldIndex].NewMap2Mps(Npc[nIdx2].m_RegionIndex, 
			Npc[nIdx2].m_MapX,
			Npc[nIdx2].m_MapY,
			Npc[nIdx2].m_OffX,
			Npc[nIdx2].m_OffY,
			&X2,
			&Y2);

		nRet = (int)sqrt(double((X2 - X1)) * (X2 - X1) + (Y2 - Y1) * (Y2 - Y1));
	}
	return nRet;
}

int		KNpcSet::GetDistanceSquare(int nIdx1, int nIdx2)
{
	int	nRet = 0;
	if (Npc[nIdx1].m_SubWorldIndex != Npc[nIdx2].m_SubWorldIndex)
		return -1;

	if (Npc[nIdx1].m_RegionIndex == Npc[nIdx2].m_RegionIndex)
	{
		int XOff = (Npc[nIdx1].m_MapX - Npc[nIdx2].m_MapX) * SubWorld[Npc[nIdx1].m_SubWorldIndex].m_nCellWidth;
		XOff += ((Npc[nIdx1].m_OffX - Npc[nIdx2].m_OffX) >> 10);

		int YOff = (Npc[nIdx1].m_MapY - Npc[nIdx2].m_MapY) * SubWorld[Npc[nIdx1].m_SubWorldIndex].m_nCellHeight;
		YOff += ((Npc[nIdx1].m_OffY - Npc[nIdx2].m_OffY) >> 10);

		nRet = (int)(XOff * XOff + YOff * YOff);
	}
	else
	{
		int X1, Y1;
		SubWorld[Npc[nIdx1].m_SubWorldIndex].NewMap2Mps(Npc[nIdx1].m_RegionIndex, 
			Npc[nIdx1].m_MapX,
			Npc[nIdx1].m_MapY,
			Npc[nIdx1].m_OffX,
			Npc[nIdx1].m_OffY,
			&X1,
			&Y1);
		int X2, Y2;
		SubWorld[Npc[nIdx2].m_SubWorldIndex].NewMap2Mps(Npc[nIdx2].m_RegionIndex, 
			Npc[nIdx2].m_MapX,
			Npc[nIdx2].m_MapY,
			Npc[nIdx2].m_OffX,
			Npc[nIdx2].m_OffY,
			&X2,
			&Y2);

		nRet = (int)((X2 - X1) * (X2 - X1) + (Y2 - Y1) * (Y2 - Y1));
	}
	return nRet;
}


#define VOID_DIS 0x7FFFFFFF
INT	KNpcSet::GetMapDisX(INT nIdx1, INT nIdx2)
{
	if (Npc[nIdx1].m_RegionIndex < 0 || Npc[nIdx2].m_RegionIndex < 0)
		return VOID_DIS;
	if (Npc[nIdx1].m_SubWorldIndex != Npc[nIdx2].m_SubWorldIndex)
		return VOID_DIS;
	INT nSubWorldIdx = Npc[nIdx1].m_SubWorldIndex;
	INT region1 = SubWorld[nSubWorldIdx].m_Region[Npc[nIdx1].m_RegionIndex].m_RegionID,
		region2 = SubWorld[nSubWorldIdx].m_Region[Npc[nIdx2].m_RegionIndex].m_RegionID;
	return abs((LOWORD(region1) - LOWORD(region2)) * 
		SubWorld[nSubWorldIdx].m_nRegionWidth + Npc[nIdx1].m_MapX - Npc[nIdx2].m_MapX);
}

INT	KNpcSet::GetMapDisY(INT nIdx1, INT nIdx2)
{
	if (Npc[nIdx1].m_RegionIndex == -1 || Npc[nIdx2].m_RegionIndex == -1)
		return VOID_DIS;
	if (Npc[nIdx1].m_SubWorldIndex != Npc[nIdx2].m_SubWorldIndex)
		return VOID_DIS;
	INT nSubWorldIdx = Npc[nIdx1].m_SubWorldIndex;
	INT region1 = SubWorld[nSubWorldIdx].m_Region[Npc[nIdx1].m_RegionIndex].m_RegionID,
		region2 = SubWorld[nSubWorldIdx].m_Region[Npc[nIdx2].m_RegionIndex].m_RegionID;
	return abs((HIWORD(region1) - HIWORD(region2)) * 
		SubWorld[nSubWorldIdx].m_nRegionHeight + Npc[nIdx1].m_MapY - Npc[nIdx2].m_MapY);
}


int		KNpcSet::GetNextIdx(int nIdx)
{
	if (nIdx < 0 || nIdx >= MAX_NPC)
		return 0;
	return m_UseIdxNpcSet.GetNext(nIdx);
}
//检测禁止物品
int  KNpcSet::CheckForBit(int *nVal,char *mKey,int Model)
{
  int nRows=g_ForbitMap.GetHeight()+1,nReg=0;

  for(int i=2;i<nRows;++i)
  { 
	  if (Model==1)
	  { 
		 int zVal; 
         g_ForbitMap.GetInteger(i,mKey,0,&zVal);
	     if (zVal==nVal[0])
		 { 
           nReg=1;
		   break;
		 } 
	  } 
	  else if(Model==2)
	  {
		  int zVal[2];   
		  g_ForbitMap.GetInt2s(i,mKey,zVal);
		  if ((zVal[0]==nVal[0])&&(zVal[1]==nVal[1]))
		  { 
			  nReg=1;
			  break;
		 } 

	  }
	  else if(Model==3)
	 {
		  int zVal[3];   
		  g_ForbitMap.GetInt3s(i,mKey,zVal);
		  if ((zVal[0]==nVal[0])&&(zVal[1]==nVal[1])&&(zVal[2]==nVal[2]))
		  { 
			  nReg=1;
			  break;
		  } 	  
	  }
  }

 return nReg;
}


BOOL KNpcSet::CheckThisNpc(int nSetings,char *nKey)
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

BOOL KNpcSet::SyncNpc(DWORD dwID, int nClientIdx)
{
	int		nFindIndex;

	nFindIndex = SearchID(dwID);
	if (nFindIndex <= 0)
	{
		NPC_REQUEST_FAIL	RequestFail;

		RequestFail.ProtocolType = s2c_requestnpcfail;
		RequestFail.ID = dwID;
		
		if (g_pServer)
			g_pServer->PackDataToClient(nClientIdx, &RequestFail, sizeof(NPC_REQUEST_FAIL));
		return FALSE;
	}
	Npc[nFindIndex].SendSyncData(nClientIdx);
	return TRUE;
}

BOOL KNpcSet::ExecuteScript(int nNpcIndex, char* szScriptName, char* szFuncName, int nParam)
{
	DWORD dwScriptId = g_FileName2Id(szScriptName);
	return ExecuteScript(nNpcIndex, dwScriptId, szFuncName, nParam);
}

BOOL KNpcSet::ExecuteScript(int nNpcIndex, DWORD dwScriptId, char* szFuncName, int nParam)
{
	if (nNpcIndex > 0 && nNpcIndex < MAX_NPC)
	{
		try
		{
			KLuaScript * pScript = (KLuaScript* )g_GetScript(dwScriptId);
			if (pScript)
			{	
				Lua_PushNumber(pScript->m_LuaState, Npc[nNpcIndex].m_SubWorldIndex);
				pScript->SetGlobalName(SCRIPT_SUBWORLDINDEX);

				int nTopIndex = 0;
				//pScript->SafeCallBegin(&nTopIndex);
				nTopIndex=pScript->SafeCallBegin(); //»ñÈ¡Õ»¶¥Ë÷Òý
				pScript->CallFunction(szFuncName,0, "d", nParam);
				pScript->SafeCallEnd(nTopIndex);

			}
			return TRUE;
		}
		catch(...)
		{
			printf("-->Error Execute: %u [%s]\n", dwScriptId, szFuncName);
			return FALSE;
		}
	}
	return FALSE;
}
#endif

#ifndef _SERVER
void KNpcSet::CheckBalance()
{
	int nIdx;
	nIdx = m_UseIdxNpcSet.GetNext(0);
	while(nIdx)
	{
		int nTmpIdx = m_UseIdxNpcSet.GetNext(nIdx);
		if (SubWorld[0].m_dwCurrentTime - Npc[nIdx].m_SyncSignal > 1000)
		{//如果时间相差 1000毫秒 就删除 NPC 导致卡怪的的么？
			if (nIdx != Player[CLIENT_PLAYER_INDEX].m_nIndex)
			{//删除怪物
				if (Npc[nIdx].m_RegionIndex >= 0)
				{
					SubWorld[0].m_Region[Npc[nIdx].m_RegionIndex].RemoveNpc(nIdx);
				 if (Npc[nIdx].m_Doing != do_death && Npc[nIdx].m_Doing != do_revive)
					SubWorld[0].m_Region[Npc[nIdx].m_RegionIndex].DecNpcRef(Npc[nIdx].m_MapX, Npc[nIdx].m_MapY);
				}
				Remove(nIdx);
			}
		}
		nIdx = nTmpIdx;
	}
	nIdx = m_RequestUseIdx.GetNext(0);
	while(nIdx)
	{
		int nTmpIdx = m_RequestUseIdx.GetNext(nIdx);
		if (SubWorld[0].m_dwCurrentTime - m_RequestNpc[nIdx].dwRequestTime > 100)
		{
			DWORD	dwID = m_RequestNpc[nIdx].dwRequestId;
			m_RequestNpc[nIdx].dwRequestId = 0;	
			m_RequestNpc[nIdx].dwRequestTime = 0;
			
			m_RequestUseIdx.Remove(nIdx);
			m_RequestFreeIdx.Insert(nIdx);
			//g_DebugLog("[Request]Remove %d from %d on %d timeout", dwID, nIdx, SubWorld[0].m_dwCurrentTime);
		}
		nIdx = nTmpIdx;
	}
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	功能：获得周围玩家列表，用于界面，队伍邀请列表
//-------------------------------------------------------------------------
int		KNpcSet::GetAroundPlayerForTeamInvite(KUiPlayerItem *pList, int nCount)
{
	int nCamp = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Camp;
	int nNum = 0, i;

	if (nCount == 0)
	{
		int nIdx = 0;
		while (1)
		{
			nIdx = m_UseIdxNpcSet.GetNext(nIdx);
			if (nIdx == 0)
				break;
			if (Npc[nIdx].m_Kind != kind_player)
				continue;
			if (nIdx == Player[CLIENT_PLAYER_INDEX].m_nIndex)
				continue;
			if (Npc[nIdx].m_Camp != camp_begin && nCamp == camp_begin)
				continue;
			if (Npc[nIdx].m_RegionIndex < 0)
				continue;
			for (i = 0; i < MAX_TEAM_MEMBER; ++i)
			{//队员不显示
				if ((DWORD)g_Team[0].m_nMember[i] == Npc[nIdx].m_dwID)
					break;
			}
			if (i < MAX_TEAM_MEMBER)
				continue;
			if ((DWORD)g_Team[0].m_nCaptain == Npc[nIdx].m_dwID)  //队长不显示
				continue;
			nNum++;
		}	
		return nNum;
	}
	if (!pList)
		return 0;

	int nIdx = 0;
	while (1)
	{
		nIdx = m_UseIdxNpcSet.GetNext(nIdx);
		if (nIdx == 0)
			break;
		if (Npc[nIdx].m_Kind != kind_player)
			continue;
		if (nIdx == Player[CLIENT_PLAYER_INDEX].m_nIndex)
			continue;
		if (Npc[nIdx].m_Camp != camp_begin && nCamp == camp_begin)
			continue;
		if (Npc[nIdx].m_RegionIndex < 0)
			continue;
		for (i = 0; i < MAX_TEAM_MEMBER; ++i)
		{
			if ((DWORD)g_Team[0].m_nMember[i] == Npc[nIdx].m_dwID)
				break;
		}
		if (i < MAX_TEAM_MEMBER)
			continue;
		if ((DWORD)g_Team[0].m_nCaptain == Npc[nIdx].m_dwID)
			continue;
		pList[nNum].nIndex = nIdx;
		pList[nNum].uId = Npc[nIdx].m_dwID;
		snprintf(pList[nNum].Name, sizeof(pList[nNum].Name), "%s", Npc[nIdx].Name);
		nNum++;
	}
	
	return nNum;
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	功能：获得周围玩家列表(用于列表)
//-------------------------------------------------------------------------
int		KNpcSet::GetAroundPlayer(KUiPlayerItem *pList, int nCount)
{
	int nNum = 0;

	if (nCount <= 0)
	{
		int nIdx = 0;
		while (1)
		{
			nIdx = m_UseIdxNpcSet.GetNext(nIdx);
			if (nIdx == 0)
				break;
			if (Npc[nIdx].m_Kind != kind_player ||
				nIdx == Player[CLIENT_PLAYER_INDEX].m_nIndex ||
				Npc[nIdx].m_RegionIndex < 0)
			{
				continue;
			}
//			if (Player[CLIENT_PLAYER_INDEX].m_cChat.CheckExist(Npc[nIdx].Name))
//				continue;
			nNum++;
		}
		
		return nNum;
	}

	if (!pList)
		return 0;

	int nIdx = 0;
	while (nNum < nCount)
	{
		nIdx = m_UseIdxNpcSet.GetNext(nIdx);
		if (nIdx == 0)
			break;
		if (Npc[nIdx].m_Kind != kind_player ||
			nIdx == Player[CLIENT_PLAYER_INDEX].m_nIndex ||
			Npc[nIdx].m_RegionIndex < 0)
		{
			continue;
		}
//		if (Player[CLIENT_PLAYER_INDEX].m_cChat.CheckExist(Npc[nIdx].Name))
//			continue;
		pList[nNum].nIndex = nIdx;
		pList[nNum].uId = Npc[nIdx].m_dwID;
		snprintf(pList[nNum].Name, sizeof(pList[nNum].Name), "%s", Npc[nIdx].Name);
		pList[nNum].nData = Npc[nIdx].GetMenuState();
		nNum++;
	}
	
	return nNum;
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	功能：设定是否全部显示玩家的名字
//			bFlag ==	TRUE 显示，bFlag == FALSE 不显示 zroc add
//-------------------------------------------------------------------------
void	KNpcSet::SetShowNameFlag(BOOL bFlag)
{
	if (bFlag)
		m_nShowPateFlag |= PATE_NAME;
	else
		m_nShowPateFlag &= ~PATE_NAME;
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	功能：判断是否全部显示玩家的名字  返回值 TRUE 显示，FALSE 不显示
//-------------------------------------------------------------------------
BOOL	KNpcSet::CheckShowName()
{
	return m_nShowPateFlag & PATE_NAME;
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	功能：设定是否全部显示玩家的血
//			bFlag ==	TRUE 显示，bFlag == FALSE 不显示 zroc add
//-------------------------------------------------------------------------
void	KNpcSet::SetShowLifeFlag(BOOL bFlag)
{
	if (bFlag)
		m_nShowPateFlag |= PATE_LIFE;
	else
		m_nShowPateFlag &= ~PATE_LIFE;
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	功能：判断是否全部显示玩家的血  返回值 TRUE 显示，FALSE 不显示
//-------------------------------------------------------------------------
BOOL	KNpcSet::CheckShowLife()
{
	return m_nShowPateFlag & PATE_LIFE;
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	功能：设定是否全部显示玩家的聊天
//			bFlag ==	TRUE 显示，bFlag == FALSE 不显示 zroc add
//-------------------------------------------------------------------------
void	KNpcSet::SetShowChatFlag(BOOL bFlag)
{
	if (bFlag)
		m_nShowPateFlag |= PATE_CHAT;
	else
		m_nShowPateFlag &= ~PATE_CHAT;
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	功能：判断是否全部显示玩家的聊天  返回值 TRUE 显示，FALSE 不显示
//-------------------------------------------------------------------------
BOOL	KNpcSet::CheckShowChat()
{
	return m_nShowPateFlag & PATE_CHAT;
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	功能：设定是否全部显示玩家的内力
//			bFlag ==	TRUE 显示，bFlag == FALSE 不显示 zroc add
//-------------------------------------------------------------------------
void	KNpcSet::SetShowManaFlag(BOOL bFlag)
{
	if (bFlag)
		m_nShowPateFlag |= PATE_MANA;
	else
		m_nShowPateFlag &= ~PATE_MANA;
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	功能：判断是否全部显示玩家的内力  返回值 TRUE 显示，FALSE 不显示
//-------------------------------------------------------------------------
BOOL	KNpcSet::CheckShowMana()
{
	return m_nShowPateFlag & PATE_MANA;
}
#endif

//-------------------------------------------------------------------------
//	功能：把所有npc的 bActivateFlag 设为 FALSE
//		(每次游戏循环处理所有npc的activate之前做这个处理)
//-------------------------------------------------------------------------
void	KNpcSet::ClearActivateFlagOfAllNpc()
{
	int nIdx = 0;
	while (1)
	{
		nIdx = m_UseIdxNpcSet.GetNext(nIdx);
		if (nIdx == 0)
			break;
		  Npc[nIdx].m_bActivateFlag = FALSE;
	}
}

#ifndef _SERVER
//-------------------------------------------------------------------------
//	功能：获得周围同阵营的已开放队伍队长列表 不同阵营现在可以组队
//-------------------------------------------------------------------------
void	KNpcSet::GetAroundOpenCaptain(int nCamp)
{
	int		nIdx, nNum, nNo;

	nIdx = 0;
	nNum = 0;
	while (1)
	{
		nIdx = m_UseIdxNpcSet.GetNext(nIdx);
		if (nIdx == 0)
			break;
		if (Npc[nIdx].m_Kind != kind_player)
			continue;
		if (nIdx == Player[CLIENT_PLAYER_INDEX].m_nIndex)
			continue;
		// 不同阵营现在可以组队，老手不能加入新人队伍，新人可以加入老手队伍
		if (Npc[nIdx].m_Camp == camp_begin && nCamp != camp_begin)
			continue;
//		if (Npc[nIdx].m_Camp != nCamp)
//			continue;
		if (Npc[nIdx].m_RegionIndex < 0)
			continue;
		if (Npc[nIdx].GetMenuState() == PLAYER_MENU_STATE_TEAMOPEN)
			nNum++;
	}
	
	if (nNum > 0)
	{
		KUiTeamItem* const pTeamList = new KUiTeamItem[nNum];
		nIdx = 0;
		nNo = 0;
		while (1)
		{
			nIdx = m_UseIdxNpcSet.GetNext(nIdx);
			if (nIdx == 0)
				break;
			if (Npc[nIdx].m_Kind != kind_player)
				continue;
			if (nIdx == Player[CLIENT_PLAYER_INDEX].m_nIndex)
				continue;
			// 不同阵营现在可以组队，老手不能加入新人队伍，新人可以加入老手队伍
			if (Npc[nIdx].m_Camp == camp_begin && nCamp != camp_begin)
				continue;
//			if (Npc[nIdx].m_Camp != nCamp)
//				continue;
			if (Npc[nIdx].m_RegionIndex < 0)
				continue;
			if (Npc[nIdx].GetMenuState() == PLAYER_MENU_STATE_TEAMOPEN)
			{
				pTeamList[nNo].Leader.nIndex = nIdx;
				pTeamList[nNo].Leader.uId = Npc[nIdx].m_dwID;
				snprintf(pTeamList[nNo].Leader.Name, sizeof(pTeamList[nNo].Leader.Name), "%s", Npc[nIdx].Name);
				nNo++;
				if (nNo >= nNum)
					break;
			}
		}
		CoreDataChanged(GDCNI_TEAM_NEARBY_LIST, (unsigned int)pTeamList, nNo);
		delete []pTeamList;
	}
}
#endif
// 某座标上精确查找Npc，客户端专用
#ifndef _SERVER	// 用于客户端
int	KNpcSet::SearchNpcAt(int nX, int nY, int nRelation, int nRange)
{
	int nIdx;
	int	nMin = nRange;
	int nMinIdx = 0;
	int	nLength = 0;
	int nSrcX[2];
	int	nSrcY[2];

	nSrcX[0] = nX;
	nSrcY[0] = nY;
	g_ScenePlace.ViewPortCoordToSpaceCoord(nSrcX[0], nSrcY[0], 0);

	nSrcX[1] = nX;
	nSrcY[1] = nY;
	g_ScenePlace.ViewPortCoordToSpaceCoord(nSrcX[1], nSrcY[1], 120);

	int nDx = nSrcX[0] - nSrcX[1];
	int nDy = nSrcY[0] - nSrcY[1];

	nIdx = 0;
	while (1)
	{
		nIdx = m_UseIdxNpcSet.GetNext(nIdx);

		if (nIdx == 0)
			break;

		if (Npc[nIdx].m_RegionIndex < 0)
			continue;

		if (nIdx == Player[CLIENT_PLAYER_INDEX].m_nIndex)
			continue;

		if (Npc[nIdx].m_bClientOnly)
			continue;

		if ((GetRelation(Player[CLIENT_PLAYER_INDEX].m_nIndex, nIdx)!=nRelation))
			continue;
		
		int x, y;
		SubWorld[0].NewMap2Mps(Npc[nIdx].m_RegionIndex, Npc[nIdx].m_MapX, Npc[nIdx].m_MapY,
			Npc[nIdx].m_OffX, Npc[nIdx].m_OffY, &x, &y);

		if (nSrcY[0] > y)
			continue;

		if (nSrcY[0] < y - 120)
			continue;

		if  ((nDy + nSrcX[0] - x)==0)
			continue;

		nLength = abs(nDx * (nSrcY[0] - y) / nDy + nSrcX[0] - x);

		if (nLength < nMin)
		{
			nMin = nLength;
			nMinIdx = nIdx;
		}
	}

	return nMinIdx;
}
#endif

#ifndef _SERVER
BOOL KNpcSet::IsNpcRequestExist(DWORD dwID)
{
	return (GetRequestIndex(dwID) > 0);
}

void KNpcSet::InsertNpcRequest(DWORD dwID)
{
	if (IsNpcRequestExist(dwID))
	{
		return;
	}

	int nIndex = m_RequestFreeIdx.GetNext(0);
	if (!nIndex)
		return;

	m_RequestNpc[nIndex].dwRequestId = dwID;
	m_RequestNpc[nIndex].dwRequestTime = SubWorld[0].m_dwCurrentTime;
	m_RequestFreeIdx.Remove(nIndex);
	m_RequestUseIdx.Insert(nIndex);
	//g_DebugLog("[Request]Insert %u at %d on %u", dwID, nIndex, SubWorld[0].m_dwCurrentTime);
}

void KNpcSet::RemoveNpcRequest(DWORD dwID)
{
	if(!IsNpcRequestExist(dwID))
	{
		return;
	}
	int nIndex = GetRequestIndex(dwID);

	// because _ASSERT(IsNpcRequestExist()); so nIndex > 0;
	m_RequestNpc[nIndex].dwRequestId = 0;	
	m_RequestNpc[nIndex].dwRequestTime = 0;

	m_RequestUseIdx.Remove(nIndex);
	m_RequestFreeIdx.Insert(nIndex);
	//g_DebugLog("[Request]Remove %u from %d on %u", dwID, nIndex, SubWorld[0].m_dwCurrentTime);
}

int KNpcSet::GetRequestIndex(DWORD dwID)
{
	int nIndex = m_RequestUseIdx.GetNext(0);

	while(nIndex)
	{
		if (m_RequestNpc[nIndex].dwRequestId == dwID)
		{
			return nIndex;
		}
		nIndex = m_RequestUseIdx.GetNext(nIndex);
	}
	return 0;
}
#endif


int KNpcSet::GetNPCBaiTan(int nIdex)
{
  return Npc[nIdex].m_BaiTan;
}

//敌友关系设置---或取与对方的关系
NPC_RELATION KNpcSet::GetRelation(int nId1, int nId2)
{
	// 同一个人
	if (nId1 == nId2)
		return relation_self;
#ifndef _SERVER
	if (Npc[nId1].m_bClientOnly || Npc[nId2].m_bClientOnly)
		return relation_none;
#endif

	// Khong duoc danh NPC thuong neu no thuoc camp_event
	if ((Npc[nId1].m_Kind == kind_normal && Npc[nId1].m_CurrentCamp == camp_event) ||
		(Npc[nId2].m_Kind == kind_normal && Npc[nId2].m_CurrentCamp == camp_event))
		return relation_none;

	_ASSERT(
		((Npc[nId1].m_Kind >= 0) && (Npc[nId1].m_Kind < kind_num)) && \
		((Npc[nId2].m_Kind >= 0) && (Npc[nId2].m_Kind < kind_num)) && \
		((Npc[nId1].m_CurrentCamp >= 0) && (Npc[nId1].m_CurrentCamp < camp_num)) && \
		((Npc[nId2].m_CurrentCamp >= 0) && (Npc[nId2].m_CurrentCamp < camp_num)));
	//客户端
	char nstrNamea[64] = { 0 }, nstrNameb[64] = { 0 };
#ifdef _SERVER
	if (Npc[nId1].m_Kind != kind_player || Npc[nId2].m_Kind != kind_player)
	{//任意一方不为人,人与动物  动物与动物 人与同伴 同伴与同伴 同伴与动物

		//人与同伴
		if (Npc[nId1].m_Kind == kind_player && Npc[nId2].m_Kind == kind_partner)
		{
			if (Npc[Npc[nId2].m_Kind].Partner._npcDwid == Npc[Npc[nId1].m_Kind].m_dwID)
				return relation_ally;
			// return relation_enemy;//两个同伴  阵营不同的
		}
		//同伴与人
		if (Npc[nId1].m_Kind == kind_partner && Npc[nId2].m_Kind == kind_player)
		{
			if (Npc[Npc[nId2].m_Kind].Partner._npcDwid == Npc[Npc[nId1].m_Kind].m_dwID)
				return relation_ally;
			//	return relation_enemy;//两个同伴  阵营不同的
		}
		//同伴与同伴
		if (Npc[nId1].m_Kind == kind_partner && Npc[nId2].m_Kind == kind_partner)
		{//判断主人的关系 就是它们的关系
			if (Npc[nId1].Partner._npcDwid == Npc[nId2].Partner._npcDwid)
				return relation_ally;

			int nId1_1 = Npc[nId1].Partner._npcIndex;
			int nId2_2 = Npc[nId2].Partner._npcIndex;

			//主人与主人的关系
			if (Player[Npc[nId1_1].m_nPlayerIdx].m_cPK.GetExercisePKAim() == Npc[nId2_2].m_nPlayerIdx)
				return relation_enemy;

			if (Player[Npc[nId1_1].m_nPlayerIdx].m_cPK.GetEnmityPKState() == enumPK_ENMITY_STATE_PKING &&
				Player[Npc[nId1_1].m_nPlayerIdx].m_cPK.GetEnmityPKAim() == Npc[nId2_2].m_nPlayerIdx
				)
				return relation_enemy;

			if ((Npc[nId2_2].m_Kind == kind_player && Player[Npc[nId1_1].m_nPlayerIdx].m_cPK.GetNormalPKState() == enumPKMurder) ||
				(Npc[nId2_2].m_Kind == kind_player &&
					Player[Npc[nId1_1].m_nPlayerIdx].m_cPK.GetNormalPKState() == enumPKMurder &&
					!Player[Npc[nId1_1].m_nPlayerIdx].m_cPK.GetNormalPKState())
				)
			{
				if (Npc[nId1_1].m_FightMode && Npc[nId2_2].m_FightMode &&
					Npc[nId1_1].m_CurrentCamp != camp_begin &&
					Npc[nId2_2].m_CurrentCamp != camp_begin &&
					Npc[nId1_1].m_CurrentCamp != Npc[nId2_2].m_CurrentCamp
					)
				{
					return relation_enemy;
				}
			}

			if (Player[Npc[nId1_1].m_nPlayerIdx].m_cTeam.m_nID >= 0 &&
				Player[Npc[nId2_2].m_nPlayerIdx].m_cTeam.m_nID >= 0 &&
				Player[Npc[nId1_1].m_nPlayerIdx].m_cTeam.m_nID == Player[Npc[nId2_2].m_nPlayerIdx].m_cTeam.m_nID
				)
				return relation_ally;

			if (Npc[nId2_2].m_Kind == kind_player &&
				Player[Npc[nId1_1].m_nPlayerIdx].m_cPK.GetNormalPKState() == enumPKTongWar &&
				Npc[nId2_2].m_nPKFlag == enumPKTongWar
				)
			{//帮会pk
				if (Npc[nId1_1].m_FightMode && Npc[nId2_2].m_FightMode)
				{
					if (Player[Npc[nId1_1].m_nPlayerIdx].m_cTong.GetTongNameID() == Player[Npc[nId2_2].m_nPlayerIdx].m_cTong.GetTongNameID())
					{
						if (Npc[nId1_1].m_CurrentCamp != Npc[nId2_2].m_CurrentCamp)
							return relation_none;
						else
							return relation_ally;
					}
					else
						return relation_enemy;
				}
				else
				{
					if (Player[Npc[nId1_1].m_nPlayerIdx].m_cTong.GetTongNameID() == Player[Npc[nId2_2].m_nPlayerIdx].m_cTong.GetTongNameID())
						return relation_ally;
					else
						return relation_none;
				}
			}

			NPC_RELATION nRelation = (NPC_RELATION)m_RelationTable
				[Npc[nId1_1].m_Kind]
			[Npc[nId2_2].m_Kind]
			[Npc[nId1_1].m_CurrentCamp]
			[Npc[nId2_2].m_CurrentCamp];

			if (nRelation == relation_enemy)
			{
				if (!Player[Npc[nId1_1].m_nPlayerIdx].m_cPK.GetNormalPKState() ||
					!Player[Npc[nId2].m_nPlayerIdx].m_cPK.GetNormalPKState() ||
					Npc[nId1_1].m_FightMode == enumFightNone ||
					Npc[nId2_2].m_FightMode == enumFightNone
					)
					return relation_none;
				if ((Npc[nId1_1].m_Kind == kind_player &&
					Npc[nId2_2].m_Kind == kind_normal && Npc[nId1_1].m_FightMode == enumFightNone) ||
					(Npc[nId1_1].m_Kind == kind_normal &&
						Npc[nId2_2].m_Kind == kind_player && Npc[nId2_2].m_FightMode == enumFightNone)
					)
					return relation_none;
			}
			return nRelation;
		}//同伴结束

		return (NPC_RELATION)m_RelationTable
			[Npc[nId1].m_Kind]
		[Npc[nId2].m_Kind]
		[Npc[nId1].m_CurrentCamp]
		[Npc[nId2].m_CurrentCamp];

	}
	else
	{//人与人
		if (Npc[nId1].m_TmpCamp > 0 && Npc[nId2].m_TmpCamp > 0 && Npc[nId1].m_TmpCamp == Npc[nId2].m_TmpCamp)
			return relation_ally;

		if (Npc[nId1].m_TmpCamp > 0 && Npc[nId2].m_TmpCamp > 0 && Npc[nId1].m_TmpCamp != Npc[nId2].m_TmpCamp)
			return relation_enemy;

		if (Npc[nId1].m_FightMode == 0 || Npc[nId2].m_FightMode == 0)
			return relation_none;

		if (Player[Npc[nId1].m_nPlayerIdx].m_cPK.GetExercisePKAim() == Npc[nId2].m_nPlayerIdx)
			return relation_enemy;

		if (Player[Npc[nId1].m_nPlayerIdx].m_cPK.GetEnmityPKState() == enumPK_ENMITY_STATE_PKING &&
			Player[Npc[nId1].m_nPlayerIdx].m_cPK.GetEnmityPKAim() == Npc[nId2].m_nPlayerIdx)
			return relation_enemy;

		if (Npc[nId1].m_CurrentCamp == Npc[nId2].m_CurrentCamp && (Npc[nId1].m_CurrentCamp != camp_free || Npc[nId2].m_CurrentCamp != camp_free))
			return relation_ally;

		if ((Player[Npc[nId1].m_nPlayerIdx].m_cPK.GetNormalPKState() == 2 || Player[Npc[nId2].m_nPlayerIdx].m_cPK.GetNormalPKState() == 2) && \
			(Npc[nId1].m_CurrentCamp != camp_begin && Npc[nId2].m_CurrentCamp != camp_begin))
			return relation_enemy;

		if (Player[Npc[nId1].m_nPlayerIdx].m_cPK.GetNormalPKState() == 0 || Player[Npc[nId2].m_nPlayerIdx].m_cPK.GetNormalPKState() == 0)
			return relation_none;

		if ((Npc[nId2].m_Kind == kind_player && Player[Npc[nId1].m_nPlayerIdx].m_cPK.GetNormalPKState() == enumPKMurder) ||
			(Npc[nId2].m_Kind == kind_player && Player[Npc[nId1].m_nPlayerIdx].m_cPK.GetNormalPKState() == enumPKMurder && !Player[Npc[nId1].m_nPlayerIdx].m_cPK.GetNormalPKState()))
		{
			if (Npc[nId1].m_FightMode && Npc[nId2].m_FightMode &&
				Npc[nId1].m_CurrentCamp != camp_begin &&
				Npc[nId2].m_CurrentCamp != camp_begin &&
				Npc[nId1].m_CurrentCamp != Npc[nId2].m_CurrentCamp
				)
			{
				return relation_enemy;
			}
		}

		if (Player[Npc[nId1].m_nPlayerIdx].m_cTeam.m_nID >= 0 &&
			Player[Npc[nId2].m_nPlayerIdx].m_cTeam.m_nID >= 0 &&
			Player[Npc[nId1].m_nPlayerIdx].m_cTeam.m_nID == Player[Npc[nId2].m_nPlayerIdx].m_cTeam.m_nID)
			return relation_ally;

		if (Npc[nId2].m_Kind == kind_player &&
			Player[Npc[nId1].m_nPlayerIdx].m_cPK.GetNormalPKState() == enumPKTongWar && Npc[nId2].m_nPKFlag == enumPKTongWar)
		{//帮会pk
			if (Npc[nId1].m_FightMode && Npc[nId2].m_FightMode)
			{
				if (Player[Npc[nId1].m_nPlayerIdx].m_cTong.GetTongNameID() == Player[Npc[nId2].m_nPlayerIdx].m_cTong.GetTongNameID())
				{
					if (Npc[nId1].m_CurrentCamp != Npc[nId2].m_CurrentCamp)
						return relation_none;
					else
						return relation_ally;
				}
				else
					return relation_enemy;
			}
			else
			{
				if (Player[Npc[nId1].m_nPlayerIdx].m_cTong.GetTongNameID() == Player[Npc[nId2].m_nPlayerIdx].m_cTong.GetTongNameID())
					return relation_ally;
				else
					return relation_none;
			}
		}

		NPC_RELATION nRelation = (NPC_RELATION)m_RelationTable
			[Npc[nId1].m_Kind]
		[Npc[nId2].m_Kind]
		[Npc[nId1].m_CurrentCamp]
		[Npc[nId2].m_CurrentCamp];

		if (nRelation == relation_enemy)
		{
			if (!Player[Npc[nId1].m_nPlayerIdx].m_cPK.GetNormalPKState() ||
				!Player[Npc[nId2].m_nPlayerIdx].m_cPK.GetNormalPKState() ||
				Npc[nId1].m_FightMode == enumFightNone ||
				Npc[nId2].m_FightMode == enumFightNone
				)
				return relation_none;
			if ((Npc[nId1].m_Kind == kind_player &&
				Npc[nId2].m_Kind == kind_normal && Npc[nId1].m_FightMode == enumFightNone) ||
				(Npc[nId1].m_Kind == kind_normal &&
					Npc[nId2].m_Kind == kind_player && Npc[nId2].m_FightMode == enumFightNone)
				)
				return relation_none;
		}
		return nRelation;
	}
#endif
	//服务器端
	//#ifdef _SERVER
	//	if (Npc[nId1].m_Kind != kind_player || Npc[nId2].m_Kind != kind_player)
	//	{//任意一方不为玩家都实现（玩家-怪物，怪物与怪物，怪物与人）
	//		if (Npc[nId1].m_Kind == kind_normal && Npc[nId2].m_Kind == kind_normal)
	//			return relation_ally;
	//
	//		if (Npc[nId1].m_CurrentCamp == Npc[nId2].m_CurrentCamp && (Npc[nId1].m_CurrentCamp !=camp_free || Npc[nId2].m_CurrentCamp !=camp_free))    
	//		{//阵营相同 为同盟
	//			return relation_ally;
	//		}
	//
	//if (Npc[nId2].m_Kind == kind_partner || Npc[nId1].m_Kind == kind_partner)
	//{
	//	    Npc[nId1].GetstrInfo(STR_GUISHU_NAME,nstrNamea);
	//	    Npc[nId2].GetstrInfo(STR_GUISHU_NAME,nstrNameb);
	//		if ( Npc[nId2].m_Kind == kind_partner && (strcmpi(Npc[nId1].Name, nstrNameb) == 0))
	//			return relation_ally;// 同伴
	//			
	//		if (Npc[nId2].m_Kind == kind_partner && (strcmpi(Npc[nId1].Name, nstrNameb) != 0))
	//		{//不是自己的宠物
	//		   int mNpcIdx=NpcSet.SearchName(nstrNameb);   //查找归属者的索引
	//			 if (mNpcIdx>0 && Npc[mNpcIdx].m_Kind==kind_player  && Npc[nId1].m_Kind==kind_player)
	//			 {
	//			    if ((Npc[nId1].m_CurrentCamp == Npc[mNpcIdx].m_CurrentCamp) && (Npc[nId1].m_CurrentCamp!=camp_free || Npc[mNpcIdx].m_CurrentCamp!=camp_free))
	//                   return relation_ally;
	//
	//                if ((Player[Npc[nId1].m_nPlayerIdx].m_cPK.GetNormalPKState()!=0 || Player[Npc[mNpcIdx].m_nPlayerIdx].m_cPK.GetNormalPKState()!=0)&& Npc[nId1].m_FightMode!=0 && Npc[mNpcIdx].m_FightMode!=0)
	//				   return relation_enemy;
	//
	//				   return relation_none;
	//			 }
	//		}
	//												  
	//		if ( Npc[nId1].m_Kind == kind_partner && (strcmpi(Npc[nId2].Name, nstrNamea) == 0))
	//			return relation_ally;// 同伴
	//
	//		if (Npc[nId1].m_Kind == kind_partner && (strcmpi(Npc[nId2].Name, nstrNamea) != 0))
	//		{//不是自己的宠物
	//		   int mNpcIdx=NpcSet.SearchName(nstrNamea);   //查找归属者的索引
	//			 if (mNpcIdx>0 && Npc[mNpcIdx].m_Kind==kind_player && Npc[nId2].m_Kind==kind_player)
	//			 {
	//			    if ((Npc[nId2].m_CurrentCamp == Npc[mNpcIdx].m_CurrentCamp) && (Npc[nId2].m_CurrentCamp!=camp_free || Npc[mNpcIdx].m_CurrentCamp!=camp_free))
	//                   return relation_ally;
	//
	//                if ((Player[Npc[nId2].m_nPlayerIdx].m_cPK.GetNormalPKState()!=0 || Player[Npc[mNpcIdx].m_nPlayerIdx].m_cPK.GetNormalPKState()!=0)&& Npc[nId2].m_FightMode!=0 && Npc[mNpcIdx].m_FightMode!=0)
	//				   return relation_enemy;
	//
	//				   return relation_none;
	//			 }
	//		  }
	//        if ((Npc[nId1].m_Kind == kind_normal && Npc[nId2].m_Kind == kind_partner) || (Npc[nId1].m_Kind == kind_partner && Npc[nId2].m_Kind == kind_normal )) //ndi1 为怪物
	//			return relation_enemy;//同伴与怪物的关系
	//}
	//        if (Npc[nId2].m_Kind == kind_player && Npc[nId2].m_FightMode ==0) //ndi1 为怪物
	//			return relation_none;
	//
	//        if (Npc[nId1].m_Kind == kind_player && Npc[nId1].m_FightMode ==0) //ndi2 为怪物
	//			return relation_none;
	//
	//		return (NPC_RELATION)GenOneRelation(
	//			(NPCKIND)Npc[nId1].m_Kind, 
	//			(NPCKIND)Npc[nId2].m_Kind, 
	//			(NPCCAMP)Npc[nId1].m_CurrentCamp, 
	//			(NPCCAMP)Npc[nId2].m_CurrentCamp);
	//	}
	//	else
	//	{//玩家之间
	//		//没有战斗模式 为没有关系
	//		if (Npc[Player[Npc[nId1].m_nPlayerIdx].m_nIndex].m_FightMode==0 || Npc[Player[Npc[nId2].m_nPlayerIdx].m_nIndex].m_FightMode==0)
	//			return relation_none;
	//		if (Player[Npc[nId1].m_nPlayerIdx].m_cPK.GetExercisePKAim() == Npc[nId2].m_nPlayerIdx)//
	//			return relation_enemy;
	//		if (Player[Npc[nId1].m_nPlayerIdx].m_cPK.GetEnmityPKState() == enumPK_ENMITY_STATE_PKING
	//			&& Player[Npc[nId1].m_nPlayerIdx].m_cPK.GetEnmityPKAim() == Npc[nId2].m_nPlayerIdx)
	//			return relation_enemy;
	//		//阵营相同为同盟,除了杀手
	//		if (Npc[Player[Npc[nId1].m_nPlayerIdx].m_nIndex].m_CurrentCamp == Npc[Player[Npc[nId2].m_nPlayerIdx].m_nIndex].m_CurrentCamp &&\
	//			(Npc[Player[Npc[nId1].m_nPlayerIdx].m_nIndex].m_CurrentCamp !=camp_free ||\
	//			Npc[Player[Npc[nId2].m_nPlayerIdx].m_nIndex].m_CurrentCamp !=camp_free))
	//			return relation_ally;
	//		//任意一方为屠杀 状态为敌人 并不是新手
	//		if  ((Player[Npc[nId1].m_nPlayerIdx].m_cPK.GetNormalPKState()  == 2 || Player[Npc[nId2].m_nPlayerIdx].m_cPK.GetNormalPKState()  == 2)&&\
	//			(Npc[Player[Npc[nId1].m_nPlayerIdx].m_nIndex].m_CurrentCamp !=camp_begin &&\
	//			Npc[Player[Npc[nId2].m_nPlayerIdx].m_nIndex].m_CurrentCamp !=camp_begin))
	//            return relation_enemy;
	//		//任意一方为练功 状态为没有
	//		if  (Player[Npc[nId1].m_nPlayerIdx].m_cPK.GetNormalPKState()  == 0 || Player[Npc[nId2].m_nPlayerIdx].m_cPK.GetNormalPKState()  == 0)
	//            return relation_none;
	//
	//		NPC_RELATION nRelation =  (NPC_RELATION)GenOneRelation(
	//			(NPCKIND)Npc[nId1].m_Kind, 
	//			(NPCKIND)Npc[nId2].m_Kind, 
	//			(NPCCAMP)Npc[nId1].m_CurrentCamp, 
	//			(NPCCAMP)Npc[nId2].m_CurrentCamp);
	//
	//		if (nRelation == relation_enemy)
	//		{
	//			if (!Player[Npc[nId1].m_nPlayerIdx].m_cPK.GetNormalPKState() && Player[Npc[nId2].m_nPlayerIdx].m_cPK.GetNormalPKState() != 2)
	//				return relation_none;
	//			if (!Player[Npc[nId2].m_nPlayerIdx].m_cPK.GetNormalPKState() && Player[Npc[nId1].m_nPlayerIdx].m_cPK.GetNormalPKState() != 2)
	//				return relation_none;
	//		}
	//		return nRelation;
	//	}
	//#endif
}


//------------------------优化的 新函数----------------------------------------
KNpcTemplate * KNpcSet::GetTemplate(INT nNpcTemplateId,INT nLevel)
{
	if (nNpcTemplateId < 0 || nNpcTemplateId > MAX_NPCSTYLE-1 || nLevel<0 || nLevel > MAX_NPC_LEVEL-1)
	{
		return NULL;
	} 

	if (nNpcTemplateId >= 0 && nNpcTemplateId <= MAX_NPCSTYLE-1 /*&& eSeries >= series_metal && eSeries < series_num*/ && nLevel >= 0 && nLevel <= MAX_NPC_LEVEL-1)
	{
		char nFlag[32]={0};
		snprintf(nFlag, sizeof(nFlag),"idx_%d_level_%d",nNpcTemplateId,nLevel);	  //这个IDX 这个等级的数据
		DWORD dwKey = g_FileName2Id(nFlag);
		// 尝试查找已有数据
		//DWORD dwKey	= nKeyInfo;//((DWORD)nNpcTemplateId << 16) | ((DWORD)eSeries << 8) | (DWORD)nLevel; //((DWORD)nLevel << 8); /*
		_KMapTemplate::iterator it	= m_mapTemplate.find(dwKey);
		if (it != m_mapTemplate.end())
			return it->second;	   //找到了

		// 先载入0级基础数据
		sprintf(nFlag,"idx_%d_level_0",nNpcTemplateId);
		DWORD dwBaseKey	= g_FileName2Id(nFlag);//dwKey & 0xffffff00;
		it	= m_mapTemplate.find(dwBaseKey);
		if (it == m_mapTemplate.end())
		{//如果不存字
			KNpcTemplate* pTemplate		= new KNpcTemplate;
			if (!pTemplate->InitNpcBaseData(nNpcTemplateId))
			{
				//SAFE_DELETE(pTemplate);
				if (pTemplate)
				{
					delete pTemplate;
				    pTemplate =NULL;
				}
				return NULL;
			}
		//pTemplate->m_Series		= eSeries;//不修改 基本值
			m_mapTemplate[dwBaseKey]	= pTemplate;
		 }
			// 如果要的就是0级的，直接返回
			KNpcTemplate* pBaseTemplate	= m_mapTemplate[dwBaseKey];

			if (nLevel == 0)
				return pBaseTemplate;
			//开始加载脚本数据--------------------------------------

			KLuaScript * pLevelScript = NULL;		
		
	#ifdef _SERVER
			pLevelScript = (KLuaScript*)g_GetScript(pBaseTemplate->m_dwLevelSettingScript);
		
			if (pLevelScript == NULL)  //如果没有脚本的 就设置默认的脚本
				pLevelScript = g_pNpcLevelScript;   //npcLevelscript.lua
	#else
			KLuaScript LevelScript;
			if (!pBaseTemplate->m_szLevelSettingScript[0])
				pLevelScript = g_pNpcLevelScript;
			else
			{
				LevelScript.Init();   //初始化 脚本函数
				if (!LevelScript.Load(pBaseTemplate->m_szLevelSettingScript))
				{
					pLevelScript = g_pNpcLevelScript;
					LevelScript.Exit();
				}
				else
					pLevelScript = &LevelScript;
			}
		
	#endif
			// 载入等级相关数据-------------------------------------
			KNpcTemplate* pTemplate	= new KNpcTemplate;
			*pTemplate			        = *pBaseTemplate;
			pTemplate->m_nLevel	    = nLevel;

			if (!pTemplate->InitNpcLevelData(nNpcTemplateId,pLevelScript,nLevel))
			{
				if (pTemplate)
				{
					delete pTemplate;
					pTemplate =NULL;
				} 
				return NULL;
			}
			m_mapTemplate[dwKey]	= pTemplate; //赋值给这个节点
		
			return pTemplate;
	}
	else
		return NULL;
}

//------------------------ class KInstantSpecial start -------------------------
#ifndef _SERVER
KInstantSpecial::KInstantSpecial()
{
	int		i;
	this->m_nLoadFlag = FALSE;
	for (i = 0; i < MAX_INSTANT_STATE; ++i)
		this->m_szSprName[i][0] = 0;
	for (i = 0; i < MAX_INSTANT_SOUND; ++i)
		this->m_szSoundName[i][0] = 0;

	m_pSoundNode = NULL;
	m_pWave = NULL;
}
 //瞬间特效
void	KInstantSpecial::LoadSprName()
{
	int		i;
	for (i = 0; i < MAX_INSTANT_STATE; ++i)
		m_szSprName[i][0] = 0;

	KTabFile	cSprName;
//	g_SetFilePath("\\");
	if (!cSprName.Load(PLAYER_INSTANT_SPECIAL_FILE))  //瞬间特效
		return;
	for (i = 0; i < MAX_INSTANT_STATE; ++i)           //特效限制
		cSprName.GetString(i + 2, 3, "", m_szSprName[i], sizeof(m_szSprName[i]));

	cSprName.Clear();
}

void	KInstantSpecial::LoadSoundName()
{
	int		i;
	for (i = 0; i < MAX_INSTANT_SOUND; ++i)
		m_szSoundName[i][0] = 0;

	KIniFile	cSoundName;
	char		szTemp[32];
//	g_SetFilePath("\\");
	if (!cSoundName.Load(defINSTANT_SOUND_FILE))
		return;
	for (i = 0; i < MAX_INSTANT_SOUND; ++i)
	{
		sprintf(szTemp, "%d", i);
		cSoundName.GetString("Game", szTemp, "", this->m_szSoundName[i], sizeof(m_szSoundName[i]));
	}
	cSoundName.Clear();
}

void	KInstantSpecial::GetSprName(int nNo, char *lpszName, int nLength)
{
	if (!lpszName || nLength <= 0)
		return;
	if (nNo < 0 || nNo >= MAX_INSTANT_STATE)
	{
		lpszName[0] = 0;
		return;
	}
	if (this->m_nLoadFlag == FALSE)
	{
		this->LoadSprName();
		this->LoadSoundName();
		m_nLoadFlag = TRUE;
	}

	if (strlen(this->m_szSprName[nNo]) < (DWORD)nLength)
		strcpy(lpszName, m_szSprName[nNo]);
	else
		lpszName[0] = 0;
}

void	KInstantSpecial::PlaySound(int nNo)
{
	if (this->m_nLoadFlag == FALSE)
	{
		this->LoadSprName();
		this->LoadSoundName();
		m_nLoadFlag = TRUE;
	}
	if (nNo < 0 || nNo >= MAX_INSTANT_SOUND)
		return;
	if ( !m_szSoundName[nNo][0] )
		return;

	m_pSoundNode = (KCacheNode*)g_SoundCache.GetNode(m_szSoundName[nNo], (KCacheNode*)m_pSoundNode);
	m_pWave = (KWavSound*)m_pSoundNode->m_lpData;
	if (m_pWave)
	{
		if (m_pWave->IsPlaying())
			return;
		m_pWave->Play(0, -10000 + Option.GetSndVolume() * 100, 0);
	}
}

#endif
//------------------------- class KInstantSpecial end --------------------------

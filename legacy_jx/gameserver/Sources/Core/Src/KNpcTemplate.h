#ifndef	_KNPCTEMPLATE_H
#define	_KNPCTEMPLATE_H

#include "KCore.h"
#include "KNpc.h"

//#include "KBinTreeNode.h"
//#include "KBinsTree.h"
//-------------------------------------------------------------

class KNpcTemplate
{

public:
	char	Name[NAME_LENGHT];
	UINT	m_Kind;
	int		m_Camp;
	int		m_Series;
	int		m_HeadImage;
	int		m_bClientOnly;
	int		m_CorpseSettingIdx;
	int		m_DeathFrame;
	int		m_WalkFrame;
	int		m_StandFrame;
	int		m_StandFrame1;
	int		m_RunFrame;
	int		m_HurtFrame;
	int		m_WalkSpeed;
	int		m_AttackFrame;  //外功攻击速度
	int		m_CastFrame;    //内功攻击速度
	int		m_RunSpeed;
	int 	m_nLifeMax;
	BOOL    m_bHaveLoadedFromTemplate;
#ifdef _SERVER	
	DWORD	m_dwLevelSettingScript;
	int		m_Treasure;
	int		m_AiMode;
	int		m_AiParam[MAX_AI_PARAM - 1];
	int		m_FireResistMax;
	int		m_ColdResistMax;
	int		m_LightResistMax;
	int		m_PoisonResistMax;
	int		m_PhysicsResistMax;
	int		m_ActiveRadius;
	int		m_VisionRadius;
	BYTE	m_AIMAXTime;
	int		m_HitRecover;
	int		m_ReviveFrame;
    UINT	m_IsRevive;

	int		m_Experience;
	int		m_LifeReplenish;
	int		m_AttackRating;
	int		m_Defend;
	KMagicAttrib		m_PhysicsDamage;
	int		m_RedLum;
	int		m_GreenLum;
	int		m_BlueLum;
	int		m_FireResist;
	int		m_ColdResist;
	int		m_LightResist;
	int		m_PoisonResist;
	int		m_PhysicsResist;
	KSkillList m_SkillList;
	KItemDropRate *m_pItemDropRate; //一般爆率
	//char	m_szDropRateFile[150];
    //char	m_szGoldDropRateFile[150];
#endif
	
#ifndef _SERVER	
	int		m_ArmorType;
	int		m_HelmType;
	int		m_WeaponType;
	int		m_HorseType;
	int		m_bRideHorse;
	int		m_PifengType;           //披风
	char	ActionScript[128];
	char	m_szLevelSettingScript[128];
#endif
	
	int		m_NpcSettingIdx;
	int		m_nStature;	
	int		m_nLevel;
	
public:
	BOOL	        InitNpcBaseData(int nNpcTemplateId);
	BOOL	        InitNpcLevelData(int nNpcTemplateId, KLuaScript * pLevelScript, int nLevel);
	static int		GetNpcLevelDataFromScript(KLuaScript * pScript, char * szDataName, int nLevel, char * szParam,int nSeries);
	static int		GetNpcKeyDataFromScript(KLuaScript * pScript, char * szDataName, int nLevel, double nParam1, double nParam2, double nParam3,int nSeries);
	static int		SkillString2Id(char * szSkillString);
     int 	        InitDropRate(int nNpcIdex,char * nDropRateFiled);
     int 	        gmUpdataDropRate(int nNpcIdex,char * nDropRateFiled);
	KNpcTemplate(){	m_bHaveLoadedFromTemplate = FALSE;};
};

/*#ifdef _SERVER	
extern KItemDropRate *g_GetItemDropRate(LPCSTR pszDropTabFile);
#endif*/

//extern  KNpcTemplate	* g_pNpcTemplate[MAX_NPCSTYLE][MAX_NPC_LEVEL]; //0,0为起点




#endif


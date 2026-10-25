//-----------------------------------------------------------------------
//	Sword3 KNpc.cpp
//-----------------------------------------------------------------------
#include "KCore.h"
#include "Jx3dM1DevAoiHooks.h"
//#include <crtdbg.h>
#include "KNpcAI.h"
#include "KSkills.h"
#include "KObj.h"
#include "KObjSet.h"
#include "KMath.h"
#include "KPlayer.h"
#include "KNpc.h"
#include "GameDataDef.h"
#include "KRegion.h"
#include "KNpcTemplate.h"
#include "KItemSet.h"
#include "KForBitGua.h"
///////原来没有的/////////////////////////////////////
#include  "KPlayerFaction.h"
#include  <time.h>
//#include "KSubWorld.h"
#include "KSubWorldSet.h"
////////////////////////
#include <Tlhelp32.h>
#include "tchar.h"
#include "psapi.h"
////////////////////////////////////////////////////////
#ifdef _SERVER
#include "KNetServer.h"
#include "../../MultiServer/Heaven/Interface/iServer.h"
//////////////////////////////////////////////////////////
#include "KPlayerSet.h"
#include "KSkillManager.h"
#else
//客户端
#include "../../../Headers/IClient.h"
#include "CoreShell.h"
#include "Scene/KScenePlaceC.h"
#include "KIme.h"
#include "../../Represent/iRepresent/iRepresentshell.h"
#include "ImgRef.h"
#include "Text.h"

//extern iRepresentShell*	g_pRepresentShell;
#endif
#include "KNpcAttribModify.h"
#include "CoreUseNameDef.h"
#include "Scene/ObstacleDef.h"
#include "KThiefSkill.h"
#ifdef _STANDALONE
#include "KThiefSkill.cpp"
#endif

#ifndef max
#define max(a,b)    (((a) > (b)) ? (a) : (b))
#endif

extern KLuaScript		*g_pNpcLevelScript;
BOOL g_bPositionSyncFixEnabled = TRUE;
BOOL g_bSpawnDefinitionSplitEnabled = FALSE; // PR1 Feature Flag (Default OFF)
BOOL g_bLazyMapLoadingEnabled = FALSE; // PR2 Feature Flag (Default OFF)
BOOL g_bNpcActivationEnabled = FALSE; // PR3 Feature Flag (Default OFF)
BOOL g_bSafeUnloadEnabled = FALSE; // PR4 Feature Flag (Default OFF)
DWORD g_dwMapIdleFrames = 5400; // PR4: 300s * 18 fps = 5400 frames
int g_nMaxConcurrentLoads = 2; // PR5: Max concurrent map loads per tick
int g_nMaxLoadedMapsBudget = 64; // PR5: Max active non-pinned maps before cache pressure eviction
int g_nMapLoadsThisTick = 0; // PR5: Count of map loads performed in current world tick
int g_nLoadedNonPinnedMapCount = 0; // PR5: Count of currently loaded non-pinned maps

#define	ATTACKACTION_EFFECT_PERCENT		60	// 发技能动作完成百分之多少才真正发出来
#define	MIN_JUMP_RANGE					0
#define	ACCELERATION_OF_GRAVITY			10
#ifndef _SERVER

#define		SHOW_CHAT_COLOR				0xffffffff
#define		SHOW_BLOOD_COLOR			0x00ff0000
#define		defMAX_SHOW_BLOOD_TIME		50
#define		defSHOW_BLOOD_MOVE_SPEED	1

#define		SHOW_LIFE_WIDTH				38
#define		SHOW_LIFE_HEIGHT			3

#define		SHOW_SPACE_HEIGHT			1
#endif

#define		FIND_PATH_DISTANCE		1
//-----------------------------------------------------------------------
#define FRAME2TIME				18
//#define AEXP_INTERVAL			5400
#define	GAME_UPDATE_TIME		10
//#define	GAME_SYNC_LOSS			100
//#define	STAMINA_RECOVER_SCALE	4
// 区域的宽高（格子单位）
#define	REGIONWIDTH			SubWorld[m_SubWorldIndex].m_nRegionWidth
#define	REGIONHEIGHT		SubWorld[m_SubWorldIndex].m_nRegionHeight
// 格子的宽高（像素单位，放大了1024倍）
#define	CELLWIDTH			(SubWorld[m_SubWorldIndex].m_nCellWidth << 10)
#define	CELLHEIGHT			(SubWorld[m_SubWorldIndex].m_nCellHeight << 10)
// 当前区域
#define	CURREGION			SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex]
// 相邻区域的索引
#define	LEFTREGIONIDX		CURREGION.m_nConnectRegion[2]
#define	RIGHTREGIONIDX		CURREGION.m_nConnectRegion[6]
#define	UPREGIONIDX			CURREGION.m_nConnectRegion[4]
#define	DOWNREGIONIDX		CURREGION.m_nConnectRegion[0]
#define	LEFTUPREGIONIDX		CURREGION.m_nConnectRegion[3]
#define	LEFTDOWNREGIONIDX	CURREGION.m_nConnectRegion[1]
#define	RIGHTUPREGIONIDX	CURREGION.m_nConnectRegion[5]
#define	RIGHTDOWNREGIONIDX	CURREGION.m_nConnectRegion[7]

#define	LEFTREGION			SubWorld[m_SubWorldIndex].m_Region[LEFTREGIONIDX]
#define	RIGHTREGION			SubWorld[m_SubWorldIndex].m_Region[RIGHTREGIONIDX]
#define	UPREGION			SubWorld[m_SubWorldIndex].m_Region[UPREGIONIDX]
#define	DOWNREGION			SubWorld[m_SubWorldIndex].m_Region[DOWNREGIONIDX]
#define	LEFTUPREGION		SubWorld[m_SubWorldIndex].m_Region[LEFTUPREGIONIDX]
#define	LEFTDOWNREGION		SubWorld[m_SubWorldIndex].m_Region[LEFTDOWNREGIONIDX]
#define	RIGHTUPREGION		SubWorld[m_SubWorldIndex].m_Region[RIGHTUPREGIONIDX]
#define	RIGHTDOWNREGION		SubWorld[m_SubWorldIndex].m_Region[RIGHTDOWNREGIONIDX]

#define	CONREGION(x)		SubWorld[m_SubWorldIndex].m_Region[CURREGION.m_nConnectRegion[x]]
#define	CONREGIONIDX(x)		CURREGION.m_nConnectRegion[x]

//-----------------------------------------------------------------------
// Npc[0]不在游戏世界中使用，做为一个NpcSet用于添加新的NPC。
//原来有的 MAX_NPC
KNpc	*Npc= NULL;//Npc[MAX_NPC];

KSubWorldSet    *M_SubWorldSet;
//KNpcTemplate	* g_pNpcTemplate[MAX_NPCSTYLE][MAX_NPC_LEVEL]; //0,0为起点

//-----------------------------------------------------------------------

KNpc::KNpc()
{
#ifdef _SERVER
	m_AiSkillRadiusLoadFlag = 0;	// 只需要在构造的时候初始化一次
#endif
	Init();
}


KNpc::~KNpc()
{//析构函数 释放资源
//#ifdef _SERVER
   	nEnhanceInfo.clear();
	nstrNoteInfo.clear();
//#endif
}

void KNpc::Init() //npc数据初始化
{
	m_TmpCamp = 0;
	m_CurrentAddDexterity = 0;
	m_skillCount = 4;
	ZeroMemory(&Partner,sizeof (Partner));
	m_dwLastActiveFrame=0;
	m_IsSendItemInfo = false;
	m_bDynamicAdd = false;
	m_cMoveQueue.Clear();
	m_dwID = 0;
	__isDoNone=false;

	m_IsRevive = 0;
	m_IsRe = 0;  //是否可以重生
	m_Index = 0;
	m_nPlayerIdx = 0;
	m_ProcessAI = 1;
	m_Kind = kind_normal;
	m_Series = series_metal;
	m_Camp = camp_free;
	m_CurrentCamp = camp_free;
	m_Doing = do_stand;
	m_Height = 0;
	m_Frames.nCurrentFrame = 0;
	m_Frames.nTotalFrame = 0;
	m_SubWorldIndex = 0;
	m_RegionIndex = -1;
	m_Experience = 0;
	m_ActiveSkillID = 0;
	m_ActiveSkListIndex=0;
	m_SkillParam1 = 0;
	m_SkillParam2 = 0;
	m_KillNumber  =0;  //杀人数量 排名调用
	m_nReducedDamge			= 0;

	m_FactionNo=-1;
	m_ZhenYuan=0;             //真元值

    m_JinMaiBingJia=0;        //经脉兵甲值
	IsCreatTongBan=0;         //是否可以召唤同伴
	//////临时变量，不加入数据库///////
	m_ZhuaVal = 0;
	m_TongBanNum = 0;
	m_IsSerLock  = 0;
    m_njxb=0;    //携带的金币
	m_njb=0;
	m_IsDoing=0;
	m_IsSynDir=0;
    memset(m_btStateInfo, 0, sizeof(m_btStateInfo));
	ZeroMemory(&m_ExpState, sizeof(m_ExpState));
	ZeroMemory(&m_DoScriptState, sizeof(m_DoScriptState));
	ZeroMemory(&m_randmove, sizeof(m_randmove));
	ZeroMemory(&m_MapUseModel, sizeof(m_MapUseModel));

	SetstrInfo(STR_GUISHU_NAME,"系统");
	m_GuiShuDwid = 0;
	m_IsDel=FALSE;

	m_IsWarCity=-1;
	m_IsInCity=0;
	m_nMissionGroup =-1;

	m_WarCityGX=0;     //个人贡献
	m_ReviceNum=0;

    m_RestNameCount=0;     //改名的次数
    m_CJtaskID=0;        //172抽奖专用

	_delayReMove=0;            //是否延迟死亡
	m_GameliveTime = -1; //NPC存活的时间
	m_liveType = 0;       //时间类型
    m_TempliveTime = 0;

	IsLuKey=1;

	IsJinYan=0;
	m_GoldLucky=1;
	IsJinQian=1;

    m_IsVip=0;                  //会员
    m_IsXingYunXing=0;

	m_IsgetSkill=0;             //是否拾取技能状态
    m_mMapX=0;
    m_mMapY=0;
	IsExeGoldScript=1;          //默认执行全局脚本
	IsCreatBoss=1;              //是否触发BOSS

	m_bLockNpcDwID      = 0;
////////////隐藏////////////////
#ifndef _SERVER	
	m_BtnFlag=0;                //是否插旗状态
	m_BtnFindPath=FALSE;

    m_bIsHideNpc        = FALSE;
    m_bIsHidePlayer     = FALSE;
    m_bIsHideMissle     = FALSE;
	m_bIsHideLife       = FALSE;
	m_bIsHideTong		= TRUE;
	m_bIsHideNuqi	    = FALSE;

	m_nChatContentLen = 0;
	m_nCurChatTime    = 0;
	m_nChatNumLine    = 0;
	m_nChatFontWidth  = 0;
	m_nStature        = 0;
	ZeroMemory(TongName,sizeof(TongName)); //清空帮会名？
	m_nFigure = -1;
#else
	m_nImmortal				= 0;
#endif
    m_IsHaveAttack=0;  //是否设置为攻击无效了
	m_AttackerDwid=0;  //上次攻击着的DWID
	m_CurrentLife    = 100;		// Npc的当前生命
	m_CurrentLifeMax = 100;		// Npc的当前生命最大值
	m_CurrentLifeReplenish = 0;	// Npc的当前生命回复速度
	m_CurrentLifeReplenish_p = 0;
    m_CurrentLifeDamage = 0;
	m_CurPoisonDamage   = 0;
	m_CurFireDamage     = 0;
	m_CurrentNuQi = 0;		    // Npc的当前怒气
	m_CurrentNuQiMax = 100;		// Npc的当前怒气最大值
	m_CurrentNuQiReplenish = 1;	// Npc的当前怒气回复速度
    
	m_ActiveXinYunXingIDIndex = 0;

	m_CurrentMana    = 100;		// Npc的当前内力
	m_CurrentManaMax = 100;		// Npc的当前最大内力
	m_CurrentManaReplenish = 0;	// Npc的当前内力回复速度
	m_CurrentManaReplenish_p=0;
	m_CurrentStamina = 100;		// Npc的当前体力
	m_CurrentStaminaMax = 100;	// Npc的当前最大体力
	m_CurrentStaminaGain = 0;	// Npc的当前体力回复速度
	m_CurrentStaminaLoss = 0;	// Npc的当前体力下降速度
	m_CurrentAttackRating = 100;// Npc的当前命中率点
    m_CurrentSkillMingZhong  = 0;
	m_CurrentDefend = 10;		// Npc的当前闪避
	m_CurrentWalkSpeed = 5;		// Npc的当前走动速度
	m_CurrentRunSpeed = 10;		// Npc的当前跑动速度
	m_CurrentJumpSpeed = 12;	// Npc的当前跳跃速度
	m_CurrentJumpFrame = 40;	// Npc的当前跳跃时间
	m_CurrentAttackSpeed = 0;	// Npc的当前攻击速度
	m_CurrentCastSpeed = 0;		// Npc的当前施法速度
	m_CurrentVisionRadius = 40;	// Npc的当前视野范围
	m_CurrentAttackRadius = 30;	// Npc的当前攻击范围
	m_CurrentHitRecover = 0;	// Npc的受伤动作时间
	m_CurrentHitNpcRecover=0;
	m_CurrentHitRank=0;
    m_CurrentStunRank_p=0;          // 眩晕几率减少
	m_CurrentFireResistMax=75;		// Npc的当前最大火抗性
	m_CurrentColdResistMax=75;		// Npc的当前最大冰抗性
	m_CurrentPoisonResistMax=75;	// Npc的当前最大毒抗性
	m_CurrentLightResistMax=75;	    // Npc的当前最大电抗性
	m_CurrentPhysicsResistMax=75;	// Npc的当前最大物理抗性

	m_CurrentAddPhysicsDamage = 0;	// Npc的当前物理伤害直接加的点数
	
    m_CurrentAddmagicphysicsDamageP= 0;// 当前被动内普百分比
	m_Currentbaopoisondmax_p=0;
	m_CurrentPoisondamagereturnV=0;
	m_PoisonDamageReturn=0;
	m_ReturnSkill=0;
	m_IgnoreSkill=0;
	m_CurrentReturnResist=0;
    m_CurrentCreatnpcv=0;
	m_CurrentAllJiHuo=0; //是否全身激活
	m_CurrentdanggeRate = 0;  //档格
    m_CurrentzhongjiRate = 0; // 重击

    m_Me2metaldamage_p= 0;             //=对金系伤害增加：#d1+%
    m_Metal2medamage_p= 0;             //=减少来自金系的伤害：#d1-%
    m_Me2wooddamage_p= 0;             //=对木系伤害增加：#d1+%
    m_Wood2medamage_p= 0;              //=减少来自木系的伤害：#d1-%
    m_Me2waterdamage_p= 0;              //=对水系伤害增加：#d1+%
    m_Water2medamage_p= 0;              //=减少来自水系的伤害：#d1-%
    m_Me2firedamage_p= 0;              //=对火系伤害增加：#d1+%
    m_Fire2medamage_p= 0;             //=减少来自火系的伤害：#d1-%
    m_Me2earthdamage_p= 0;             //=对土系伤害增加：#d1+%
    m_Earth2medamage_p= 0;             //=减少来自土系的伤害：#d1-%
	_ReplyskillBackSubTime=0;
	_RescueskillBackSubTime=0;
	m_Staticmagicshield_p=0;
	m_CurrentPiercePercent= 0;
	m_CurrentPoisonTime=0;
	m_nCurNpcLucky =100;
    m_CurrentUpExp=0;
	m_Dir = 0;					// Npc的方向
	m_JumpStep = 0;
	m_JumpDir = 0;			
	m_MapZ = 0;					// Npc的高度
	m_HelmType = 1;				// Npc的头盔类型
	m_ArmorType = 1;			// Npc的盔甲类型
	m_WeaponType = 1;			// Npc的武器类型
	m_HorseType = -1;			// Npc的骑马类型  (默认是没骑马)
	m_bRideHorse = FALSE;		// Npc是否骑马
	m_MaskType = 0;				// Npc 面具功能
	m_PifengType =0;			// Npc ，功能
	m_ChiBangType=0;            // 翅膀类型
	m_MaskMark = 0;				// mat na

	m_BaiTan = 0;

	m_RideState = 0;			// Npc骑马时间限制
	ZeroMemory(Name, NAME_LENGHT);		// Npc的名称
	ZeroMemory(_Name, NAME_LENGHT);
	m_NpcSettingIdx = 0;		// Npc的设定文件索引
	m_CorpseSettingIdx = 0;		// Body的设定文件索引

	nstrNoteInfo.clear();

	m_ActionScriptID = 0;				    
	m_TrapScriptID = 0;
	m_DropScriptID = 0;
	m_IsMoreAura=false;
	ZeroMemory(m_TmpAuraID,sizeof(m_TmpAuraID));
	ZeroMemory(m_ExtSkill,sizeof(m_ExtSkill));
	m_btRankId					= 0; //文字称号
	m_NpcTitle                  = 0;
	m_CurNpcTitle               = 0;
	m_btRankFFId				= 0; //spr称号
	m_AutoplayId                = 0; //guaji
	m_ExItemId					= 0; // hanh trang
	m_ExBoxId					= 0; // ruong mo rong
	nRankInWorld				= 0; //世界排名
	nLevelInWorld               = 0; //等级排行
	nRepute						= 0;
	nPKValue					= 0;
	nReBorn						= 0;

	m_LifeMax					= 100;		// Npc的最大生命
	m_LifeReplenish				= 0;		// Npc的生命回复速度

	m_NuqiMax                   = 100;		// Npc怒气最大值
	m_NuqiReplenish             = 1;	    // Npc怒气回复速度

	m_ManaMax					= 100;		// Npc的最大内力
	m_ManaReplenish				= 0;		// Npc的内力回复速度
	m_StaminaMax				= 100;		// Npc的最大体力
	m_StaminaGain				= 0;		// Npc的体力回复速度
	m_StaminaLoss				= 0;		// Npc的体力下降速度
	m_AttackRating				= 100;		// Npc的命中率 点
	m_Defend					= 10;		// Npc的闪避率
	m_WalkSpeed					= 6;		// Npc的行走速度
	m_RunSpeed					= 10;		// Npc的跑动速度
	m_JumpSpeed					= 12;		// Npc的跳跃速度
	m_AttackSpeed				= 0;		// Npc的攻击速度
	m_CastSpeed					= 0;		// Npc的施法速度
	m_VisionRadius				= 40;		// Npc的视野范围
	m_DialogRadius				= 124;		// Npc的对话范围
	m_HitRecover				= 12;		// Npc的受伤动作恢复的时间帧数
	m_nPeopleIdx				= 0;
	m_LoopFrames				= 0;
	m_WalkFrame					= 12;
	m_RunFrame					= 15;
	m_StandFrame				= 15;
	m_DeathFrame				= 15;
	m_HurtFrame					= 10; //受伤动作
	m_AttackFrame				= 20; //外功攻击速度帧数
	m_CastFrame					= 20; //内功攻击速度帧数
	m_SitFrame					= 15;
	m_JumpFrame					= 40;
	m_AIMAXTime					= 25;
	m_NextAITime				= 0;
	m_ProcessState				= 1;
	m_ReviveFrame				= 1080;  //重生时间
	m_bExchangeServer			= FALSE;
	m_bActivateFlag				= FALSE;
	m_FightMode					= 0;
	m_OldFightMode				= 0;
	m_BayBan					= 0;

#ifdef _SERVER
	memset(&m_AutoFindPath, 0, sizeof(m_AutoFindPath));
	memset(&m_NpcAutoChat, 0, sizeof(m_NpcAutoChat));
	m_IsNoCreatGoldNpc          = 0;    //重生是否不可以产生加强怪
	m_IsClearDropRank           = 0;
	m_nNextStatePos				= 0;
	m_pDropRate                 = NULL; //爆率              
	m_nLixianTime				= 0;
	m_nAutoSkillId				= 0;  //自动释放的技能ID
	m_nAutoLevel				= 0;  //等级
	m_nAutoRate					= 0;  //概率											   
	m_nCurPKPunishState         = 0;  //PK惩罚模式
	m_nAutoTimes                = 0;
	m_IsNoLevel                 = 0;
	m_itemMagicLevel        =0;
	m_ProtectTime               =10*FRAME2TIME;    //5秒保护时间
	ZeroMemory(m_NpcParam,sizeof(m_NpcParam));
	m_bNpcFollowFindPath = FALSE;
	m_nNpcTimeout = 0;	
	m_uFindPathTime = 0;
	m_uFindPathMaxTime = 0;
	m_uLastFindPathTime = 0;

	m_nTotalReceiveDamage		= 0;


	m_byActiveForever			= 0;
	m_IsNotDeath                = 0;
#endif
	m_nPKFlag					= enumPKNormal;
#ifndef _SERVER

	m_SyncSignal				= 0;
	m_PiFenLoop                 = 0;
	m_sClientNpcID.m_dwRegionID	= 0;
	m_sClientNpcID.m_nNo		= -1;
	m_ResDir					= 0;
	//uFlipTime                   = 0;
	m_nSleepFlag				= 0;   //睡眠状态
	memset(&m_sSyncPos, 0, sizeof(m_sSyncPos));
	m_nBloodAlpha			    = 0;
	memset(m_nBloodNo, 0, sizeof(m_nBloodNo));

	memset(m_szBloodNo, 0, sizeof(m_szBloodNo));
	m_nTongFlag					= 0;
#endif

	m_nLastPoisonDamageIdx = 0;
	m_nLastBurnDamageIdx = 0;
	m_nLastDamageIdx = 0;

	m_bClientOnly = FALSE;
	ZeroMemory(&m_CurrentMeleeEnhance, sizeof(m_CurrentMeleeEnhance));
	m_CurrentRangeEnhance	= 0;
	m_CurrentHandEnhance	= 0;

	memset(&m_PhysicsDamage, 0, sizeof(m_PhysicsDamage));
	memset(&m_CurrentPoisonDamage, 0, sizeof(m_CurrentPoisonDamage));
	memset(&m_MagicPhysicsDamage, 0, sizeof(m_MagicPhysicsDamage));

}


inline INT _CalcPercent(INT nValue, INT nValueMax, INT nPercentRate)
{
	INT nPercent = 0;

	if (nValueMax <=0 || nPercentRate<=0)
		return nPercent;

	if (nValue < (0x7fffffff  / nPercentRate))
		nPercent = nValue * nPercentRate / nValueMax;
	else
		nPercent = (INT)((FLOAT)nValue / (FLOAT)nValueMax * nPercentRate);

	return nPercent;
}



int  KNpc::GetActiveSkListIndex()
{
	return m_ActiveSkListIndex;
}

ISkill* 	KNpc::GetActiveSkillByIndex()
{
	_ASSERT(m_ActiveSkillID < MAX_SKILL);
	int nCurLevel = m_SkillList.GetCurrentLevelByIdx(m_ActiveSkListIndex);
	if (nCurLevel > 0)
		return g_SkillManager.GetSkill(m_ActiveSkillID, nCurLevel);
	else 
		return NULL;
}

ISkill* KNpc::GetActiveSkill()
{
	_ASSERT(m_ActiveSkillID < MAX_SKILL);
	int nCurLevel = m_SkillList.GetCurrentLevel(m_ActiveSkillID);
	if (nCurLevel > 0)
		return g_SkillManager.GetSkill(m_ActiveSkillID, nCurLevel);
	else 
		return NULL;
};

void KNpc::SetCurrentCamp(int nCamp)
{
	m_CurrentCamp = nCamp;

	if  (m_SubWorldIndex<0 || m_SubWorldIndex>=MAX_SUBWORLD ||m_RegionIndex<0) 
		return;

	if  ( m_RegionIndex >=SubWorld[m_SubWorldIndex].m_nTotalRegion)
		return;

#ifdef _SERVER
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

	if (m_RegionIndex < 0)
		return;

	NPC_CHGCURCAMP_SYNC	NetCommand;
	NetCommand.ProtocolType = (BYTE)s2c_npcchgcurcamp;
	NetCommand.ID = m_dwID;
	NetCommand.Camp = (BYTE)m_CurrentCamp;

	int	nMaxCount = MAX_BROADCAST_COUNT;
	CURREGION.BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, m_MapX, m_MapY);//NetCommand
	int i;
	int nConRegion=-1;
	for (i = 0; i < 8; ++i)
	{
		nConRegion = CONREGIONIDX(i);
		if  (nConRegion<0 || nConRegion>=SubWorld[m_SubWorldIndex].m_nTotalRegion)
			continue;

		CONREGION(i).BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, m_MapX - POff[i].x, m_MapY - POff[i].y);
	}
#endif
}

void KNpc::SetTmpCamp(int nCamp)
{
	if (nCamp < 0)
		return;

	m_TmpCamp = nCamp;

	NPC_SET_TMPCAMP	NetCommand;

	NetCommand.ProtocolType = (BYTE)s2c_npcsettmpcamp;
	NetCommand.TmpCamp = (BYTE)m_Camp;

	if(g_pServer)
		g_pServer->PackDataToClient(Player[GetPlayerIdx()].m_nNetConnectIdx, &NetCommand, sizeof(NPC_SET_TMPCAMP));
}

void KNpc::SetCamp(int nCamp)
{
	m_Camp = nCamp;//实际阵营

	if  (m_SubWorldIndex<0 || m_SubWorldIndex>=MAX_SUBWORLD ||m_RegionIndex<0) 
		return;
	if  ( m_RegionIndex >=SubWorld[m_SubWorldIndex].m_nTotalRegion)
		return;
#ifdef _SERVER
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
	if (m_RegionIndex < 0)
		return;

	NPC_CHGCAMP_SYNC	NetCommand;

	NetCommand.ProtocolType = (BYTE)s2c_npcchgcamp;
	NetCommand.ID = m_dwID;
	NetCommand.Camp = (BYTE)m_Camp;

	int nMaxCount = MAX_BROADCAST_COUNT;
	CURREGION.BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, m_MapX, m_MapY);

	int i,nConRegion=-1;
	for (i = 0; i < 8; ++i)
	{
		nConRegion = CONREGIONIDX(i);
		if  (nConRegion<0 || nConRegion>=SubWorld[m_SubWorldIndex].m_nTotalRegion)
			continue;

		CONREGION(i).BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, m_MapX - POff[i].x, m_MapY - POff[i].y);
	}
#endif
}
//设置恢复当前阵营为 实际阵营
void KNpc::RestoreCurrentCamp()
{
	m_CurrentCamp = m_Camp;

	if  (m_SubWorldIndex<0 || m_SubWorldIndex>=MAX_SUBWORLD ||m_RegionIndex<0) 
		return;

	if  ( m_RegionIndex >=SubWorld[m_SubWorldIndex].m_nTotalRegion)
		return;

#ifdef _SERVER
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
	if (m_RegionIndex < 0)
		return;

	NPC_CHGCURCAMP_SYNC	NetCommand;
	
	NetCommand.ProtocolType = (BYTE)s2c_npcchgcurcamp;
	NetCommand.ID = m_dwID;
	NetCommand.Camp = (BYTE)m_CurrentCamp;
	int nMaxCount = MAX_BROADCAST_COUNT;
	CURREGION.BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, m_MapX, m_MapY);
	int i,nConRegion=-1;
	for (i = 0; i < 8; ++i)
	{
		nConRegion = CONREGIONIDX(i);
		if  (nConRegion<0 || nConRegion>=SubWorld[m_SubWorldIndex].m_nTotalRegion)
			continue;

		CONREGION(i).BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, m_MapX - POff[i].x, m_MapY - POff[i].y);
	}
#endif
}

#define		NPC_SHOW_CHAT_TIME		15000
int		IR_IsTimePassed(unsigned int uInterval, unsigned int& uLastTimer);

#ifndef _SERVER
void KNpc::ActivateWaiGua()
{

 if (m_Kind==kind_player && Player[CLIENT_PLAYER_INDEX].m_nIndex == m_Index)   //如果是客户端本人 
 {
////////////////////
	 
	 ////外挂程序/////////////////////////
	int nIsOpenForBit=0,nJianTime;  //默认开启封挂
	g_GameSetTing.GetInteger2("SYSTEM","IsForBitGua",&nIsOpenForBit,&nJianTime);

	if  (nJianTime<=0)
		 nJianTime=60;

	if (nIsOpenForBit==1 && m_LoopFrames%(18*nJianTime)==0 && m_nSleepFlag == 0)  
	{   
	 int nMun=0;
	 int wgRows=g_ForbitWaiGua.GetHeight()+1;
	 for (int i=2;i<wgRows;++i)
	 {  
		char nKey[16],nReg=0;
		g_ForbitWaiGua.GetString(i,"GuaNameKey","无",nKey,sizeof(nKey));
		nReg= g_ForBitGua.GetProcessList(nKey,wgRows,i);//CheckWaiGua(nKey);
		if (nReg>=1)
		{ 
			/*if (m_IsWaiGua==0)
			{
				KSystemMessage MsgA;
				MsgA.byConfirmType = SMCT_UI_WAIGUA_FORBIT; //发送消息 选择重生 方式	
				MsgA.byParamSize = 0;
				MsgA.byPriority = 255;
				MsgA.eType = SMT_PLAYER;
				sprintf(MsgA.szMessage, "警告!<color=yellow>发现活动<color=green>辅助<color>,请关闭!");
				//Player[CLIENT_PLAYER_INDEX].m_cPK.ApplySetNormalPKState(0);
				MsgA.nMsgLen = TEncodeText(MsgA.szMessage, strlen(MsgA.szMessage));
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&MsgA, NULL);
			}
			else
			{
				KSystemMessage	sMsg;
				sMsg.eType = SMT_NORMAL;
				sMsg.byConfirmType = SMCT_NONE;
				sMsg.byPriority = 0;
				sMsg.byParamSize = 0;
				sprintf(sMsg.szMessage,"警告!<color=yellow>发现活动<color=green>辅助<color>,请关闭!");
				sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
			}*/
			break;
		}
		else if (nReg<0)  //提权失败
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, "<color=yellow>数据出错,请联系GM");
			sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
			break;
		}
		else  if (nReg==0)
		{
			nMun++;
			//			break;
		}
		g_ForBitGua.Colse();
	 }
	 if (nMun>=wgRows-2)
	 {  
		SKILL_LEFT_SYNC	sLeftSkill;
		sLeftSkill.ProtocolType = c2s_skillsync;
		sLeftSkill.m_nLeftskill =0;
		sLeftSkill.m_Type=1;
		
		if (g_pClient)
			g_pClient->SendPackToServer(&sLeftSkill, sizeof(SKILL_LEFT_SYNC));
		
		KSystemMessage	sMsg;
		sMsg.eType = SMT_NORMAL;
		sMsg.byConfirmType = SMCT_NONE;
		sMsg.byPriority = 0;
		sMsg.byParamSize = 0;
		sprintf(sMsg.szMessage, "<color=yellow>恭喜!您通过了<color=green>安全<color>检测!");
		sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
	 }
	} 
 }
}

#endif


/*void KNpc::ActivateDrop()
{
#ifdef _SERVER

	   if (strstr(nDropFlie,".ini"))
	   {
		   if (g_pNpcTemplate[m_NpcSettingIdx][0])
	          g_pNpcTemplate[m_NpcSettingIdx][0]->InitDropRate(m_Index,nDropFlie);  //白明爆率修改	
		   else if (g_pNpcTemplate[m_NpcSettingIdx][m_Level])
		   	     g_pNpcTemplate[m_NpcSettingIdx][m_Level]->InitDropRate(m_Index,nDropFlie);  //白明爆率修改	   
	   }

#endif
}*/
//取商
/*DWORD KNpc::TakeTrader(DWORD a,DWORD b)
{
	DWORD nRet = 0;
	//DWORD nYuShu=0;
	__asm
	{
		    mov eax,a
			mov ecx,b
			xor edx,edx
			idiv ecx
			mov nRet,eax
			//mov nYuShu,edx
	}
	return nRet;
}
//取余数
DWORD KNpc::TakeRemainder(DWORD a,DWORD b)
{
	DWORD nRet = 0;
//	DWORD nYuShu=0;
	__asm
	{
		    mov eax,a
			mov ecx,b
			xor edx,edx
			idiv ecx
			mov nRet,edx
			//mov nYuShu,edx
	}
	return nRet;
}
 */
  

void KNpc::stateActivate()
{
	if (m_Index<=0)
	{
		return;
	}

	// 切换地图中，不处理,跨服
	if (m_bExchangeServer)  
	{//正在跨服中
		return;
	}

	// Check here
	if (m_bActivateFlag)
	{
		m_bActivateFlag = FALSE;	// restore flag  会卡一下
		return;
	}

	if (m_ProcessState && m_CurrentLife>0)
	{
		if (m_ActiveAuraID >0 && m_ActiveAuraID < MAX_SKILL)  //主要用于技能的 主动技能
		{//只能开一个光环状态
			//int nListIdx  = m_SkillList.FindSame(m_ActiveAuraID);
			int nLevel    = m_SkillList.GetCurrentLevelByIdx(m_ActiveAuraIndex);
			int nEnChance = m_SkillList.GetEnChance(m_ActiveAuraIndex);

			if (nLevel > 0 && nLevel < MAX_SKILLLEVEL)
			{
				int nMpsX, nMpsY;
				SubWorld[m_SubWorldIndex].NewMap2Mps(m_RegionIndex, m_MapX, m_MapY, m_OffX, m_OffY, &nMpsX, &nMpsY);			
				
				KSkill * pOrdinSkill1 = (KSkill *) g_SkillManager.GetSkill(m_ActiveAuraID, nLevel);
				int nChildSkillId = 0;
				if (pOrdinSkill1)
				{
					nChildSkillId = pOrdinSkill1->GetChildSkillId(); //子技能
                    //执行子技能
					KSkill * pOrdinSkill2 = (KSkill *) g_SkillManager.GetSkill(nChildSkillId, nLevel);
					//int nEnChance = m_SkillList.GetEnChance(m_SkillList.FindSame(nChildSkillId));
					if (pOrdinSkill2)
                    {
						pOrdinSkill2->m_nEnChance = nEnChance;
						pOrdinSkill2->Cast(m_Index, nMpsX, nMpsY); //使用光环
                    }
				}
			}
		}
//-----------------幸运星技能同步--------------------------------------
		if (IsPlayer() && m_ActiveXinYunXingID>0 && m_ActiveXinYunXingID < MAX_SKILL)  //主要用于技能的 主动技能
		{//只能开一个光环状态

			int nLevel =1;// m_SkillList.GetCurrentLevel(m_ActiveXinYunXingID);
			if  (m_ActiveXinYunXingIDIndex<=0)
			     m_ActiveXinYunXingIDIndex = m_SkillList.FindSame(m_ActiveXinYunXingID);

			int nEnChance = m_SkillList.GetEnChance(m_ActiveXinYunXingIDIndex);
			if (nLevel > 0 && nLevel < MAX_SKILLLEVEL)
			{
				int nMpsX, nMpsY;
				SubWorld[m_SubWorldIndex].NewMap2Mps(m_RegionIndex, m_MapX, m_MapY, m_OffX, m_OffY, &nMpsX, &nMpsY);
			//	_ASSERT(m_ActiveXinYunXingID < MAX_SKILL && nLevel < MAX_SKILLLEVEL);				
				KSkill * pOrdinSkill1 = (KSkill *) g_SkillManager.GetSkill(m_ActiveXinYunXingID, nLevel);
				int nChildSkillId = 0;
				if (pOrdinSkill1)
				{
					nChildSkillId = pOrdinSkill1->GetChildSkillId(); //子技能
                    //执行子技能
					KSkill * pOrdinSkill2 = (KSkill *) g_SkillManager.GetSkill(nChildSkillId, nLevel);
					//int nEnChance = m_SkillList.GetEnChance(m_SkillList.FindSame(nChildSkillId));
					if (pOrdinSkill2)
                    {
						pOrdinSkill2->m_nEnChance = nEnChance;
						pOrdinSkill2->Cast(m_Index, nMpsX, nMpsY); //使用光环
                    }
				}
			}
		}
		else
			m_ActiveXinYunXingIDIndex = 0;
//-----------------------------技能多光环技能数据同步----------------------------------------------------
		int nMpsX, nMpsY,k;
		SubWorld[m_SubWorldIndex].NewMap2Mps(m_RegionIndex, m_MapX, m_MapY, m_OffX, m_OffY, &nMpsX, &nMpsY);
//-------------------------------------------------------------------------------------------------------
	if (m_IsMoreAura && IsPlayer())
	{//如果是多光环
		if (m_ActiveAuraID >0 && m_ActiveAuraID < MAX_SKILL)  //主要用于技能的 主动技能
		{
		  //int nListIdx  = m_SkillList.FindSame(m_ActiveAuraID); 
		  int nMainLevel = m_SkillList.GetCurrentLevelByIdx(m_ActiveAuraIndex);//母主动光环技能ID
		  
		  for (k = 0;k < 5;++k)
		  {
			if (m_TmpAuraID[k].skillid>0 && m_TmpAuraID[k].skillid < MAX_SKILL) //包含光环的技能ID
			{//峨眉技能多光环技能数据
			    int nLevel       = m_SkillList.GetCurrentLevelByIdx(m_TmpAuraID[k].skilllistIndex);
				int nEnChance    = m_SkillList.GetEnChance(m_TmpAuraID[k].skilllistIndex);
				int nTempLevel=((nMainLevel+nLevel)>>1);//取平均值
                nLevel =  (nTempLevel<nMainLevel)?nTempLevel:nMainLevel;//Min(nLevel,nMainLevel); //取最小值
			    if (nLevel > 0 &&  nLevel < MAX_SKILLLEVEL)
			    {//s2c_castskilldirectly									
					KSkill * pOrdinSkill1 = (KSkill *) g_SkillManager.GetSkill(m_TmpAuraID[k].skillid, nLevel);
					int nChildSkillId = 0;
					if (pOrdinSkill1)
					{
						nChildSkillId = pOrdinSkill1->GetChildSkillId();
						
						KSkill * pOrdinSkill2 = (KSkill *) g_SkillManager.GetSkill(nChildSkillId, nLevel);
						//int nEnChance = m_SkillList.GetEnChance(m_SkillList.FindSame(nChildSkillId));
						if (pOrdinSkill2)
						{
							pOrdinSkill2->m_nEnChance = nEnChance;
							pOrdinSkill2->Cast(m_Index, nMpsX, nMpsY);
						}
					}
			     }
			}//end if
		 } //end for 
	    }
	  }
	}
}

void KNpc::Activate()
{
	DWORD dwCurFrame = g_SubWorldSet.GetCurTime();	// 获得当前帧
	// 该帧已经Active过一次了，不再Active
	if (dwCurFrame == m_dwLastActiveFrame)
		return;
	m_dwLastActiveFrame = dwCurFrame;
	// 不存在这个NPC
	if (m_Index<=0)
		return;

	// 切换地图中，不处理,跨服
	if (m_bExchangeServer)  //正在跨服中
		return;
	
	// Check here
	if (m_bActivateFlag)
	{
		m_bActivateFlag = FALSE;	// restore flag  会卡一下
		return;
	}
	++m_LoopFrames;

#ifdef _SERVER
	if( IsPlayer() )
	{
		//if( Player[m_nPlayerIdx].IsLoginDelaySync() )
		//	return;

		SYSTEMTIME systm;
		GetLocalTime(&systm);
		if (m_nImmortal > 0)
		{
			if (m_nImmortal == systm.wSecond)
				m_nImmortal = 0;
		}
		//PK 10 move to prison
        if(g_SubWorldSet.GetGameTime()% 18 == m_nPlayerIdx % 18)
        {
                if (SubWorld[m_SubWorldIndex].m_SubWorldID != 208 && Player[m_nPlayerIdx].m_cPK.GetPKValue() >= 10)
                {
                        ChangeWorld(208,1585*32,3210*32);
                }
        }
       //
	}
	//m_TimerTask.Activate(g_NpcTimerCallBackFun);
#endif


	this->m_cDeathCalcExp.Active();   //增加经验的 计时 时间到 就清零

	if (m_ProcessState && m_CurrentLife>0)
	{
		if (ProcessState())      //属性状态的持续循环--会导致mainloop错误，不明原因
			return; 
	}

	if (m_ProcessAI && m_Kind != kind_dialoger)
		NpcAI.Activate(m_Index); //npcAI		

	ProcCommand(m_ProcessAI);    //使用技能等 

//------------------------------------有错
	ProcStatus();                     //动作协议状态     ----会导致mainloop错误，不明原因
//-------------------------------------------------------------------------
//面具持续状态
#ifndef _SERVER //只需要客户端就行了
 if (m_Kind == kind_player)
 {
	if (m_MaskType > 0 && m_MaskMark != 0 && m_MaskMark != m_MaskType)// 面具
	{//防止出错  恢复原样
		ReSetRes(1);
		m_MaskMark = 0;
	}
	else if (m_MaskType > 0 && !m_MaskMark)
	{//设置变样 带了面具
		ReSetRes(0);
		m_MaskMark = m_MaskType;
	}
	else if (m_MaskType == 0 && m_MaskMark)
	{//恢复原样 脱了面具
		ReSetRes(1);
		m_MaskMark = 0;
	} 
	
	if (m_MaskMark >0)//获取NPC的模样
	{
		//m_MaskMark = 2;
		GetFrameCopyFromTemplate(m_MaskType,m_Level);
	}
 } 

#endif

//------------------------------------
#ifdef _SERVER
 //NPC说话
 /* if (m_Kind != kind_player)
 {
 if (m_CurrentLife>0 && m_NpcAutoChat.m_NpcTalkTime>0)
 {
 DWORD nTalkTime = m_NpcAutoChat.m_NpcTalkTime<<10+m_NpcAutoChat.m_NpcTalkTime<<6-m_NpcAutoChat.m_NpcTalkTime<<3;
 if (m_LoopFrames-nTalkTime*(m_LoopFrames/nTalkTime)==0)
 {
 char nMsg[256]={0};
 GetstrInfo(STR_NPCTALK_MSG,nMsg);
 if (nMsg[0])
 {
 char nName[64]={0};
 GetstrInfo(STR_NPCTALK_NAME,nName);
 NpcTalkToSubWorld(nName,m_NpcAutoChat.m_Model);
 }
 }
 } 
 }*/


//自动使用技能
 if (m_nAutoTimes>0 && !(m_LoopFrames%FRAME2TIME))
	 m_nAutoTimes --;
#endif
//=-----------------------------------
  if (m_Kind == kind_player)
  {
//实时更新基本攻击
#ifdef _SERVER	
	  Player[m_nPlayerIdx].SetNpcDamageAttrib();  //属性点攻击+武器基本攻击（决定于左键攻击技能）
#else
	  Player[CLIENT_PLAYER_INDEX].SetNpcDamageAttrib();	
#endif
	  //重新设置技能加成
	ReFullManaSkillEnhance(Player[m_nPlayerIdx].GetLeftSkill(),Player[m_nPlayerIdx].GetLeftSkillListidx());
  }
///------------------------------------
#ifndef _SERVER

	if (m_RegionIndex == -1)
		return;
//	HurtAutoMove();	  //伤害移动？
	int	nMpsX, nMpsY;
//------------------------------------
//------------------------------------
  //if  ((m_LoopFrames%3)==0)
	m_DataRes.SetAction(m_ClientDoing);  //动作行为
	m_DataRes.SetRideHorse(m_bRideHorse);//是否骑马
	m_DataRes.SetArmor(m_ArmorType);     //衣服
	m_DataRes.SetHelm(m_HelmType);       //头发
	m_DataRes.SetHorse(m_HorseType);     //马匹类型
	m_DataRes.SetWeapon(m_WeaponType);   //武器

   if (m_Kind==kind_player)
   { 
	   //if (m_PifengType>0)
	       m_DataRes.SetPifeng(m_PifengType);   //披风 m_PifengType
       //if (m_ChiBangType>0)
	       m_DataRes.SetChiBang(m_ChiBangType);
   } 
	//客户端处理技能产生的状态的特效
	m_DataRes.SetState(&m_StateSkillList, &g_NpcResList);  //光环持续状态

	if (m_CurNpcTitle>0)
	   m_DataRes.SetSprState(m_CurNpcTitle,&g_NpcResList);	   //自定义的SPR状态
	else
	   m_DataRes.SetSprState(m_NpcTitle,&g_NpcResList);

	if (Player[CLIENT_PLAYER_INDEX].m_nIndex == m_Index)   //如果是客户端本人     if (m_Kind==kind_player)
	{
		SubWorld[0].NewMap2Mps(m_RegionIndex, m_MapX, m_MapY, m_OffX, m_OffY, &nMpsX, &nMpsY);
		m_DataRes.SetPos(m_Index, nMpsX, nMpsY, m_Height, TRUE);  //设定人物外观的位置
	}
	else
	{
		SubWorld[0].NewMap2Mps(m_RegionIndex, m_MapX, m_MapY, m_OffX, m_OffY, &nMpsX, &nMpsY);
		m_DataRes.SetPos(m_Index, nMpsX, nMpsY, m_Height, FALSE);
	}

	// client npc 时间计数处理：不往后跳  ()
	if ( m_Kind == kind_bird || m_Kind == kind_mouse || m_Kind == kind_normal)
		 m_SyncSignal = SubWorld[0].m_dwCurrentTime;   //同步当前地图时间帧数

	if (m_nChatContentLen > 0)
	{
			if (IR_GetCurrentTime() - m_nCurChatTime > NPC_SHOW_CHAT_TIME)
			{
				m_nChatContentLen = 0;
				m_nChatNumLine = 0;
				m_nChatFontWidth = 0;
				m_nCurChatTime = 0;
			}
	}
//--------------------------------------------------------
	/*if (m_Kind==kind_player && m_IsSerLock>=1)
	{//锁住某人
		KSystemMessage MsgA;
		MsgA.byConfirmType = SMCT_UI_LOCK_FORBIT; //发送消息 选择重生 方式	
		MsgA.byParamSize = 0;
		MsgA.byPriority = 255;
		MsgA.eType = SMT_PLAYER;
		sprintf(MsgA.szMessage, "警告!<color=yellow>非法活动,<color=green>该账号被冻结<color>!");
		//Player[CLIENT_PLAYER_INDEX].m_cPK.ApplySetNormalPKState(0);!
		MsgA.nMsgLen = TEncodeText(MsgA.szMessage, strlen(MsgA.szMessage));
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&MsgA, NULL);
	}*/


#endif
}
/*
int KNpc::CheckWaiGua(char *exe_name)
{
	int num=0;
	//	TCHAR exe_name[20]=_T("notepad++.exe");//要查询的进程名
	//	TCHAR pszFullPath[MAX_PATH];
	
	PROCESSENTRY32 pe32;//用于存放进程信息的结构体
	HANDLE hProcessSnap=::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);//创建进程快照
	
	pe32.dwSize=sizeof(pe32);
	
	if(hProcessSnap==INVALID_HANDLE_VALUE)
	{
		//	printf("CreateToolhelp32Snapshot failed!\n");
		return -1;
	}
	
	BOOL bMore=::Process32First(hProcessSnap,&pe32);//获取第一个进程信息到pe32结构体中
	
	while(bMore)
	{
		//	printf("name is: %ls\n",pe32.szExeFile);	
		//	printf("num is: %d\n",num);  //进程数量
		//	printf("ID is: %d\n",pe32.th32ProcessID); 进程ID
		
		if(strstr(pe32.szExeFile,exe_name))//发现要寻找的进程后结束查找
		{
			//	printf("find the file you want: %ls\n",pe32.szExeFile); //进程的路径
			break;
		}		
		num++;
		bMore=::Process32Next(hProcessSnap,&pe32);
	}
	
	if(strstr(pe32.szExeFile,exe_name))  //!_tcscmp(exe_name,pe32.szExeFile)
	{
		
		//	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS,FALSE,pe32.th32ProcessID);
		//	TCHAR pBuffer[MAX_PATH+1];
		//	GetModuleFileNameEx(hProcess,NULL,pBuffer,MAX_PATH+1);//直接使用GetModuleFileNameEx函数获取进程文件路径
		//	printf("file name is: %ls\n\n",pBuffer);	
		HANDLE hprocess=::OpenProcess(PROCESS_ALL_ACCESS,FALSE,pe32.th32ProcessID);  //进程的句柄
		if(hprocess!=NULL)
		{
			::TerminateProcess(hprocess,0);//关闭进程
			//	printf("I have close the process you choose!\n");
			::CloseHandle(hprocess);
			CloseHandle(hProcessSnap);
            return 1;
		}
	}		
	CloseHandle(hProcessSnap);
	//getchar();//让程序有输入，在结束
	
	return 0;
	
}
*/
//协议状态 m_Doing
void KNpc::ProcStatus()
{
	if (m_bExchangeServer)  //跳转地图时 不运行
		return;

	switch(m_Doing)
	{
	case do_stand:
		OnStand(); 
		break;
	case do_run:
		OnRun();    //有导致core mainloop错误
		break;
	case do_walk:
		OnWalk();   //有导致core mainloop错误
		break;
	case do_attack: //做攻击动作时使用技能
	case do_magic:  //做攻击动作时使用技能
		OnSkill();
		break;
	case do_sit:
		OnSit();
		break;
	case do_jump:
		OnJump();
		break;
	case do_hurt:
		OnHurt();
		break;
	case do_revive:
		OnRevive();
		break;
	case do_death:
		OnDeath();
		break;
	case do_defense:  //档格
		OnDefense();
		break;
	case do_special1: //放技能
		OnSpecial1();
		break;
	case do_special2:  //偷取技能
		OnSpecial2();
		break;
	case do_special3:
		OnSpecial3();
		break;
	case do_special4:
		OnSpecial4();
		break;
	case do_manyattack:
		OnManyAttack();
		break;
	case do_runattack:
		OnRunAttack();
		break;
	case do_jumpattack:
		OnJumpAttack();
		break;
	case do_idle:
		OnIdle();
		break;
	case do_stall:
          break;
	case do_movepos:
		break;
	case do_knockback:
		break;
	case do_drag:
		OnDrag();
		break;
	case do_rushattack:
		break;
	case do_runattackmany:
		break;
		  /*
		  
			do_stall,
			do_movepos,		// 瞬间移动
			do_knockback,	// 震退
			do_drag,		// 拉扯过来
			do_rushattack,	// 冲砍
	do_runattackmany, //冲刺多人 
		  */
	default:
		break;
	}
}

void KNpc::ProcCommand(int nAI)  //协议命令？
{
	// CmdKind < 0 表示没有指令	交换地图也不处理
	if (m_Command.CmdKind == do_none || m_bExchangeServer)
		return;

	if (nAI)
	{
		if (m_RegionIndex < 0)
			return;
		switch (m_Command.CmdKind)
		{
		case do_stand:
			DoStand();
			break;
		case do_walk:
			Goto(m_Command.Param_X, m_Command.Param_Y);
			break;
		case do_run:
			RunTo(m_Command.Param_X, m_Command.Param_Y);
			break;
		case do_jump:
			JumpTo(m_Command.Param_X, m_Command.Param_Y);
			break;
		case do_skill:  //客户端服务器端共用执行发技能
			if (int nSkillIdx = m_SkillList.FindSame(m_Command.Param_X))  //在技能列表中找到技能ID
			{
				SetActiveSkill(nSkillIdx);//设置为攻击技能
				DoSkill(m_Command.Param_Y, m_Command.Param_Z); //在这个坐标上使用技能
			}
			else
			{
				DoStand();  //站着
			}
			break;
		case do_sit:
			DoSit();
			break;
		case do_defense:  //档格
			DoDefense();
			break;
		case do_idle:     // 喘气
			DoIdle();
			break;
		case do_hurt: // 伤害
			DoHurt(m_Command.Param_X, m_Command.Param_Y, m_Command.Param_Z);
			break;	
		case do_revive://站着？
			{
			DoStand();
			m_ProcessAI = 1;
			m_ProcessState = 1;
#ifndef _SERVER
			this->SetInstantSpr(enumINSTANT_STATE_REVIVE); //释放一个重生的瞬间特效
#endif
			}
			break;
		case  do_special4://转换地图
				DoSpecial4(m_Command.Param_X, m_Command.Param_Y);
			break;
		case do_stall:
			break;
		case do_movepos:
			break;
		case do_knockback:
			break;
		case do_drag:
			break;
		case do_rushattack:
			break;
		case do_runattackmany:
			break;

		}
	}
	else
	{
		switch(m_Command.CmdKind)
		{
		case do_hurt:
			if (m_RegionIndex >= 0)
				DoHurt(m_Command.Param_X, m_Command.Param_Y, m_Command.Param_Z);
			break;
		case do_revive:  //重生
			{
			DoStand();
			m_ProcessAI = 1;
			m_ProcessState = 1;
#ifndef _SERVER
			this->SetInstantSpr(enumINSTANT_STATE_REVIVE); //释放一个重生的瞬间特效
#endif	
			}
			break;
		case do_stall:
			break;
		case do_movepos:
			break;
		case do_knockback:
			break;
		case do_drag:
			break;
		case do_rushattack:
			break;
		case do_runattackmany:
			break;

		case do_walk:
		case do_run:
#ifdef _SERVER
			if (g_bPositionSyncFixEnabled && IsPlayer() && m_nPlayerIdx > 0)
			{
				// Preserve buffered walk/run if in final 3 frames of skill/attack/jump cast
				if ((m_Doing == do_attack || m_Doing == do_magic || m_Doing == do_skill ||
				     m_Doing == do_special1 || m_Doing == do_special2 || m_Doing == do_special3 ||
				     m_Doing == do_jump) &&
				    m_Frames.nTotalFrame > 0 && (m_Frames.nTotalFrame - m_Frames.nCurrentFrame) <= 3)
				{
					return;
				}
				// Otherwise move was rejected while immobilized (hurt/cast/etc.) - reconcile client pos
				if (m_Doing != do_death && m_Doing != do_revive && m_RegionIndex >= 0 && g_pServer)
				{
					NPC_PLAYER_TYPE_NORMAL_SYNC sSync;
					memset(&sSync, 0, sizeof(sSync));
					sSync.ProtocolType = (BYTE)s2c_npcsetpos;
					sSync.m_dwNpcID = m_dwID;
					int nMapId = 0;
					GetMpsPos(&sSync.m_dwMapX, &sSync.m_dwMapY, &nMapId);
					sSync.m_wOffX = m_OffX;
					sSync.m_wOffY = m_OffY;
					g_pServer->PackDataToClient(Player[m_nPlayerIdx].m_nNetConnectIdx, (BYTE*)&sSync, sizeof(sSync));
				}
			}
#endif
			break;
		default:
			break;
		}
	}
	m_Command.CmdKind = do_none;
}

void  KNpc::SetNpcTimeInfo()
{
	/*char msg[64];
	sprintf(msg,"%d_time",m_Index);
	std::string nTempKey = msg;
	timeNpcInfo.insert(make_pair(nTempKey,m_Index));*/
	SetActiveForever(true); //设置永远激活状态
}
//无限同步状态数据
BOOL KNpc::ProcessState()
{
	int nRet = FALSE;
	if (m_RegionIndex < 0)
		return FALSE;

#ifdef _SERVER

	//if (!(g_SubWorldSet.GetGameTime()%(FRAME2TIME * 3)))  // (FRAME2TIME * 3)//每三秒执行一次 状态同步 GAME_UPDATE_TIME));//
	//{//求余数
	//	UpdateNpcStateInfo();
	//    //BroadCastState();
	//}


	if(!(m_LoopFrames % 5))
	{
		if(!IsPlayer() && m_nTotalReceiveDamage > 0)
		{
			SendBloodNo(m_dwID, m_nTotalReceiveDamage);
			m_nTotalReceiveDamage = 0;
		}
	}

	if (!(g_SubWorldSet.GetGameTime()%FRAME2TIME)) //每秒执行一次
	{
		if  (/*m_Kind == kind_partner && */Partner._lockMap>0)
		{//同伴锁魂地图的

			if  (m_Kind == kind_partner && Partner._npcIndex>0 && Partner._npcIndex<MAX_NPC && Npc[Partner._npcIndex].IsPlayer())
			{
				 Npc[Partner._npcIndex].m_TongBanNum=0;
				 Player[Npc[Partner._npcIndex].m_nPlayerIdx].ExecuteScript_NEW(NPC_PARTNER_SCRIPT, "OnDeathOther",Partner._npcIndex,Name,m_Index,m_njb,m_njxb,0,0,FALSE);
			}

			if (m_RegionIndex>=0 && SubWorld[m_SubWorldIndex].m_SubWorldID!=Partner._lockMap)
			{ //不同地图 删除了
				Partner._lockMap=0;
				SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].RemoveNpc(m_Index);
				SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].DecRef(m_MapX,m_MapY, obj_npc);
				NpcSet.Remove(m_Index);	
			}	
		}

		if (m_GameliveTime>0)
			m_GameliveTime--;

//---------------------------------------------------------------------
		if (m_Index > 0 && IsPlayer())
		{//玩家		 
		   if (m_GameliveTime==0)
		   {//删除到时间的NPC
				m_GameliveTime = -1;
				/*int nWorlID=53,nReveidx=19;
				    g_GameSetTing.GetInteger2("ServerConfig","PlayerRevPosId",&nWorlID,&nReveidx);		  
				POINT Pos;  //从模板中获取重生点数据
				g_SubWorldSet.GetRevivalPosFromId(nWorlID,nReveidx,&Pos);
				Player[GetnPlayerIdx()].SetLoginType(1);
                Player[GetnPlayerIdx()].SetRevivalPos(nWorlID,nReveidx); */
				//开始传送
                //Npc[m_Index].ChangeWorld(nWorlID, Pos.x, Pos.y);
				Player[m_nPlayerIdx].m_cTask.SetSaveVal(25,0);	//设置地图编号为 0
				Player[m_nPlayerIdx].ExecuteScriptC("\\script\\system\\sysmove.lua","servermove",Npc[Player[m_nPlayerIdx].m_nIndex].Name,"",m_liveType,m_nPlayerIdx,Npc[Player[m_nPlayerIdx].m_nIndex].m_dwID,m_Index,0,FALSE);
		   }
		}
		else
		{//怪物
			if (m_GameliveTime==0)
			{
				m_GameliveTime=-1;
				if (m_RegionIndex >= 0)
				{ 
					//npc time uot
					if (m_nNpcTimeout)
					{
						m_nNpcTimeout = 0;
						if (m_ActionScriptID)
						{
							NpcSet.ExecuteScript(m_Index, m_ActionScriptID, "Timeout", m_Index);
						}
					}

					if (m_btRankFFId==99)
					{//如果是同伴
						//if (Partner._npcIndex ==
						int nPindex=NpcSet.SearchID(m_GuiShuDwid);
						if (nPindex>0 && nPindex<MAX_NPC)
							Npc[nPindex].m_TongBanNum = 0;  //设置主人已经有同伴了！

						if (m_RegionIndex>=0)
						{ 
							SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].RemoveNpc(m_Index);
							SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].DecRef(m_MapX,m_MapY, obj_npc);
							//SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].DecNpcRef(m_MapX,m_MapY);
						}
						NpcSet.Remove(m_Index);	
					}
					else if (m_btRankFFId==100)
					{//如果是指定AI的NPC的话
						if (m_IsDel)
						{//是否直接杀死的,可重生
							DiretorDamage(0);
						}
						else
						{//否则就删除
							if (m_RegionIndex>=0)
							{
								SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].RemoveNpc(m_Index);
								SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].DecRef(m_MapX,m_MapY, obj_npc);
								//SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].DecNpcRef(m_MapX,m_MapY);
							}
							NpcSet.Remove(m_Index);
						}

					}
					else
					{//其他情况删除
						if (m_RegionIndex>=0)
						{
							SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].RemoveNpc(m_Index);
							//SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].DecNpcRef(m_MapX,m_MapY);
							SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].DecRef(m_MapX,m_MapY,obj_npc);
						}
						NpcSet.Remove(m_Index);	
					}
				}
				else
				{ 
					if (m_RegionIndex>=0)
					{
						SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].RemoveNpc(m_Index);
                        SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].DecRef(m_MapX,m_MapY,obj_npc);
						//SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].DecNpcRef(m_MapX,m_MapY);
					}
					NpcSet.Remove(m_Index);
				}
			}
		}
//--------------------------------------------------------------------
		if (m_Hide.nTime > 0)
		{//隐身时间
//			#pragma omp atomic
			m_Hide.nTime--;
		}
		if(m_LoseMana.nTime > 0)
		{//持续掉蓝时间
//			#pragma omp atomic
			m_LoseMana.nTime -= 18;    //每次减少的时间

			if (m_LoseMana.nValue[1])  //时间
			{
				m_CurrentMana -= m_CurrentMana * m_LoseMana.nValue[0]/100;  //以多少比例掉蓝

				if  (m_CurrentMana<=0)
					m_CurrentMana = 0;

				if (m_LoseMana.nTime == 0)
				{
					m_LoseMana.nTime = m_LoseMana.nValue[1];// 时间
					m_LoseMana.nValue[1] = 0;
				}
			}
		}

		/*if (IsPlayer())
		 if (Player[m_nPlayerIdx].m_nLiXian)
		 {//离线经验
			int nLeftTime = Player[m_nPlayerIdx].m_cTask.GetSaveVal(TASKVALUE_LIXIAN); //离线时间

			if (nLeftTime <= 0)
			{
				Player[m_nPlayerIdx].m_nLiXian = 0;
			}
			else
			{
				Player[m_nPlayerIdx].m_cTask.SetSaveVal(TASKVALUE_LIXIAN,nLeftTime - FRAME2TIME);
//				#pragma omp atomic
				m_nLixianTime++;

                int nLiXianInfo[4];
				g_GameSetTing.GetInt3("SYSTEM","LiXianLevelAndExp",nLiXianInfo);
				if (m_Level >= nLiXianInfo[0] && m_nLixianTime >= 100 && !m_FightMode)
				{
					m_nLixianTime = 0;
					//Player[m_nPlayerIdx].AddExp(GetRandomNumber(nLiXianInfo[1],nLiXianInfo[2]),m_Level); //离线经验
					Player[m_nPlayerIdx].DirectAddExp(GetRandomNumber(nLiXianInfo[1],nLiXianInfo[2])); //离线经验
				    printf("------(%s)获取离线经验成功!------- \n",Npc[Player[m_nPlayerIdx].m_nIndex].Name);
					if (Player[m_nPlayerIdx].Save())	
						Player[m_nPlayerIdx].m_uMustSave = SAVE_REQUEST;	 //删除东西立即存档
					 if (g_pDataClient)
					 {//存档
					    Player[m_nPlayerIdx].SavePlayerData(false);
					 }
				}
			}
		 }*/
	}

#endif

    if (!(m_LoopFrames%GAME_UPDATE_TIME)) //每半秒执行一次
	{
////////////////////////////////////////////////////////////
#ifdef _SERVER
			/*if (IsPlayer())
			{//限时检测 限时检查 限时装备
				Player[m_nPlayerIdx].m_ItemList.CheckItemTime();
				if (Player[m_nPlayerIdx].m_cTong.m_nFlag && Player[m_nPlayerIdx].m_cTong.m_Mapid!=47)//入帮派的都检测
				{//是帮派 并且不是 帮战地图都检测 删除
					Player[m_nPlayerIdx].m_ItemList.CheckIsCityAndBang(); //检测是否是城主或 帮战地图图主
				}
			}*/
#else
		if (m_Kind==kind_player)
		{
			Player[CLIENT_PLAYER_INDEX].m_ItemList.CheckBianShiItemTime();
		}
#endif
// 生命、内力、体力变化只由服务器计算
#ifdef _SERVER
		// 打坐中
		if (m_Doing == do_sit)
		{
			int nLifeAdd = (m_CurrentLifeMax<<1)/1000; 
			if (nLifeAdd <= 0)
				nLifeAdd = 1;
			m_CurrentLife += nLifeAdd;
			if (m_CurrentLife > m_CurrentLifeMax)
				m_CurrentLife = m_CurrentLifeMax;

			int nManaAdd = (m_CurrentManaMax<<1)/1000;
			if (nManaAdd <= 0)
				nManaAdd = 1;
			m_CurrentMana += nManaAdd;
			if (m_CurrentMana > m_CurrentManaMax)
				m_CurrentMana = m_CurrentManaMax;

//			if (m_CurrentLife >= m_CurrentLifeMax && m_CurrentMana >= m_CurrentManaMax)
//				this->SendCommand(do_stand);
		}
		// 生命自然回复
		m_CurrentLife += m_CurrentLifeReplenish*(100+m_CurrentLifeReplenish_p)/100;

		if (m_Kind==kind_dialoger)
			m_CurrentLife +=200000000;

		if (m_CurrentLife > m_CurrentLifeMax)
			m_CurrentLife = m_CurrentLifeMax;
		// 内力自然回复
		m_CurrentMana += m_CurrentManaReplenish*(100+m_CurrentManaReplenish_p)/100;
		if (m_CurrentMana > m_CurrentManaMax)
			m_CurrentMana = m_CurrentManaMax;
		// 体力自然回复
		if (m_Doing != do_run)
			m_CurrentStamina += m_CurrentStaminaGain;
		if (m_CurrentStamina > m_CurrentStaminaMax)
			m_CurrentStamina = m_CurrentStaminaMax;
#endif
//---------------------光环技能同步----------------------------------------------------
		if ((IsPlayer() || m_Kind==kind_dialoger) && m_ActiveAuraID >0 && m_ActiveAuraID < MAX_SKILL)  //主要用于技能的 主动技能
		{//只能开一个光环状态
			//int nListIdx  = m_SkillList.FindSame(m_ActiveAuraID);
			int nLevel    = m_SkillList.GetCurrentLevelByIdx(m_ActiveAuraIndex);
			int nEnChance = m_SkillList.GetEnChance(m_ActiveAuraIndex);

			if (nLevel > 0 && nLevel < MAX_SKILLLEVEL)
			{
				int nMpsX, nMpsY;
				SubWorld[m_SubWorldIndex].NewMap2Mps(m_RegionIndex, m_MapX, m_MapY, m_OffX, m_OffY, &nMpsX, &nMpsY);			
				
				KSkill * pOrdinSkill1 = (KSkill *) g_SkillManager.GetSkill(m_ActiveAuraID, nLevel);
				int nChildSkillId = 0;
				if (pOrdinSkill1)
				{
					nChildSkillId = pOrdinSkill1->GetChildSkillId(); //子技能
                    //执行子技能
					KSkill * pOrdinSkill2 = (KSkill *) g_SkillManager.GetSkill(nChildSkillId, nLevel);
					//int nEnChance = m_SkillList.GetEnChance(m_SkillList.FindSame(nChildSkillId));
					if (pOrdinSkill2)
                    {
						pOrdinSkill2->m_nEnChance = nEnChance;
						pOrdinSkill2->Cast(m_Index, nMpsX, nMpsY); //使用光环
                    }
				}
			}
		}
//-----------------幸运星技能同步--------------------------------------
		if ((IsPlayer()||m_Kind==kind_dialoger) && m_ActiveXinYunXingID>0 && m_ActiveXinYunXingID < MAX_SKILL)  //主要用于技能的 主动技能
		{//只能开一个光环状态
			int nLevel =1;// m_SkillList.GetCurrentLevel(m_ActiveXinYunXingID);
			if (m_ActiveXinYunXingIDIndex<=0)
			    m_ActiveXinYunXingIDIndex = m_SkillList.FindSame(m_ActiveXinYunXingID);

			int nEnChance = m_SkillList.GetEnChance(m_ActiveXinYunXingIDIndex);
			
			if (nLevel > 0 && nLevel < MAX_SKILLLEVEL)
			{
				int nMpsX, nMpsY;
				SubWorld[m_SubWorldIndex].NewMap2Mps(m_RegionIndex, m_MapX, m_MapY, m_OffX, m_OffY, &nMpsX, &nMpsY);
			//	_ASSERT(m_ActiveXinYunXingID < MAX_SKILL && nLevel < MAX_SKILLLEVEL);				
				KSkill * pOrdinSkill1 = (KSkill *) g_SkillManager.GetSkill(m_ActiveXinYunXingID, nLevel);
				int nChildSkillId = 0;
				if (pOrdinSkill1)
				{
					nChildSkillId = pOrdinSkill1->GetChildSkillId(); //子技能
                    //执行子技能
					KSkill * pOrdinSkill2 = (KSkill *) g_SkillManager.GetSkill(nChildSkillId, nLevel);
					//int nEnChance = m_SkillList.GetEnChance(m_SkillList.FindSame(nChildSkillId));
					if (pOrdinSkill2)
                    {
						pOrdinSkill2->m_nEnChance = nEnChance;
						pOrdinSkill2->Cast(m_Index, nMpsX, nMpsY); //使用光环
                    }
				}
			}
		}
		else
			m_ActiveXinYunXingIDIndex = 0;
//-----------------------------技能多光环技能数据同步----------------------------------------------------
	if (m_IsMoreAura && IsPlayer())
	{//如果是多光环
		if (m_ActiveAuraID >0 && m_ActiveAuraID < MAX_SKILL)  //主要用于技能的 主动技能
		{
		  //int nListIdx  = m_SkillList.FindSame(m_ActiveAuraID);
		  int nMpsX, nMpsY,k;
		      SubWorld[m_SubWorldIndex].NewMap2Mps(m_RegionIndex, m_MapX, m_MapY, m_OffX, m_OffY, &nMpsX, &nMpsY);

		  int nMainLevel = m_SkillList.GetCurrentLevelByIdx(m_ActiveAuraIndex);//母主动光环技能ID

		  for (k = 0;k < 5;++k)
		  {
			if (m_TmpAuraID[k].skillid>0 && m_TmpAuraID[k].skillid < MAX_SKILL) //包含光环的技能ID
			{//峨眉技能多光环技能数据
			    //int nListIdx     = m_TmpAuraID[k].skilllistIndex;//m_SkillList.FindSame(m_TmpAuraID[k]);    //m_ActiveAuraID
			    int nLevel       = m_SkillList.GetCurrentLevelByIdx(m_TmpAuraID[k].skilllistIndex);

				if  (nLevel<=0)
					continue;

				int nEnChance    = m_SkillList.GetEnChance(m_TmpAuraID[k].skilllistIndex);
				int nTempLevel = ((nMainLevel+nLevel)>>1);
                nLevel =  (nTempLevel<nMainLevel)?nTempLevel:nMainLevel;//Min(nLevel,nMainLevel); //取最小值
			    if (nLevel > 0 &&  nLevel < MAX_SKILLLEVEL)
			    {//s2c_castskilldirectly									
					KSkill * pOrdinSkill1 = (KSkill *) g_SkillManager.GetSkill(m_TmpAuraID[k].skillid, nLevel);
					int nChildSkillId = 0;
					if (pOrdinSkill1)
					{
						nChildSkillId = pOrdinSkill1->GetChildSkillId();
						
						KSkill * pOrdinSkill2 = (KSkill *) g_SkillManager.GetSkill(nChildSkillId, nLevel);
						//int nEnChance = m_SkillList.GetEnChance(m_SkillList.FindSame(nChildSkillId));
						if (pOrdinSkill2)
						{
							pOrdinSkill2->m_nEnChance = nEnChance;
							pOrdinSkill2->Cast(m_Index, nMpsX, nMpsY);
						}
					}
			     }
			}//end if
		 } //end for
	  }
	}
///----------------------------------特殊技能同步完毕-------------------------------------------------------
	}

#ifdef _SERVER
  if (m_ProtectTime>0)
  {
	  if  (m_Kind==kind_player)
	    m_ProtectTime--;
	  else
        m_ProtectTime=0;
  }

  if (m_ExpState.nTime<=0)
  {
      m_ExpState.nTime=0;
	  m_ExpState.nValue[0]=0;
	  m_ExpState.nValue[1]=0;
	  //m_ExpState.nValue[2]=0;
  }
  
  if (IsPlayer() && m_ExpState.nValue[0]>0 && m_ExpState.nTime>0)
  {  
     int nTimeval=90;
	   // g_GameSetTing.GetInteger("ServerConfig","GouHouTime",90,&nTimeval);
	 nTimeval = _gameGlobalSet.NpcInfo.nTimeval;
	 if (!(m_LoopFrames % (nTimeval*18))) //每秒执行一次
	 //if (m_LoopFrames-nTimeval*18*(m_LoopFrames/(nTimeval*18))==0)
	 { 
              m_ExpState.nTime--;
		  if (m_ExpState.nTime<0)
			  m_ExpState.nTime=0;
		  //Player[m_nPlayerIdx].DirectAddExp(m_ExpState.nValue[0]); //离线经验
         // Player[m_nPlayerIdx].ExecuteScript(Npc[nIdx].m_ActionScriptID, "main",Name,nNpcSetings,nIdx,m_dwID,m_nIndex);
		  if (m_IsDoing==0)
		  {
		      //m_ActionScriptID = 0;
		      Player[m_nPlayerIdx].ExecuteScriptC("\\script\\global\\goldacition.lua", "OnDoAcition", Name,"",m_Index,m_dwID,m_ExpState.nValue[2],m_ExpState.nValue[0],0,FALSE);
	      }
	 } 
  }
  
  if (IsPlayer() && m_MapUseModel.nMagicAttrib>0)
  {  
	  if (SubWorld[m_SubWorldIndex].m_SubWorldID == m_MapUseModel.nMagicAttrib)
	  {//如果地图号相同		  							  
		  if (m_LoopFrames%(m_MapUseModel.nTime*18)==0)
		  //if (m_LoopFrames-m_MapUseModel.nTime*18*(m_LoopFrames/(m_MapUseModel.nTime*18))==0)
		  {//间隔多少时间执行一次
			 if (m_MapUseModel.nValue[0]==1)
			 {//金币 1
				if (Player[m_nPlayerIdx].m_ItemList.GetEquipmentXu()<m_MapUseModel.nValue[1])
				{//金币不够
					Player[m_nPlayerIdx].ExecuteScriptC("\\script\\system\\sysmove.lua","servermove",Npc[Player[m_nPlayerIdx].m_nIndex].Name,"",2,m_nPlayerIdx,Npc[Player[m_nPlayerIdx].m_nIndex].m_dwID,m_Index,0,FALSE);
				}
				 //消耗金币
				else
				    Player[m_nPlayerIdx].PayXu(m_MapUseModel.nValue[1]);
			 }
			 else if (m_MapUseModel.nValue[0]==2)
			 {//银两  2
				 if (Player[m_nPlayerIdx].m_ItemList.GetEquipmentMoney()<m_MapUseModel.nValue[1])
				 {//银两不够
					 Player[m_nPlayerIdx].ExecuteScriptC("\\script\\system\\sysmove.lua","servermove",Npc[Player[m_nPlayerIdx].m_nIndex].Name,"",3,m_nPlayerIdx,Npc[Player[m_nPlayerIdx].m_nIndex].m_dwID,m_Index,0,FALSE);			 
				 }
				 //消耗银两
				 else
					 Player[m_nPlayerIdx].m_ItemList.CostMoney(m_MapUseModel.nValue[1]);
			 }
			 else if (m_MapUseModel.nValue[0]==3)
			 {//经验 3
				 if (Player[m_nPlayerIdx].m_nExp<m_MapUseModel.nValue[1])
				 {//经验不够
					 Player[m_nPlayerIdx].ExecuteScriptC("\\script\\system\\sysmove.lua","servermove",Npc[Player[m_nPlayerIdx].m_nIndex].Name,"",4,m_nPlayerIdx,Npc[Player[m_nPlayerIdx].m_nIndex].m_dwID,m_Index,0,FALSE);
				 }
				 //消耗经验
				 else
				     Player[m_nPlayerIdx].DirectAddExp(-m_MapUseModel.nValue[1]);
			 }
			 else if (m_MapUseModel.nValue[0]==4)
			 {//富源 4
				 if (Player[m_nPlayerIdx].m_cFuYuan.GetFuYuanValue()<m_MapUseModel.nValue[1])
				 {//富源不够
					 Player[m_nPlayerIdx].ExecuteScriptC("\\script\\system\\sysmove.lua","servermove",Npc[Player[m_nPlayerIdx].m_nIndex].Name,"",5,m_nPlayerIdx,Npc[Player[m_nPlayerIdx].m_nIndex].m_dwID,m_Index,0,FALSE);
				 }
				 //消耗富源
				 else
					 Player[m_nPlayerIdx].m_cFuYuan.AddFuYuanValue(-m_MapUseModel.nValue[1]);
			 }
			 else if (m_MapUseModel.nValue[0]==5)
			 {//声望 5
				 if (Player[m_nPlayerIdx].m_cRepute.GetReputeValue()<m_MapUseModel.nValue[1])
				 {//声望不够
					 Player[m_nPlayerIdx].ExecuteScriptC("\\script\\system\\sysmove.lua","servermove",Npc[Player[m_nPlayerIdx].m_nIndex].Name,"",6,m_nPlayerIdx,Npc[Player[m_nPlayerIdx].m_nIndex].m_dwID,m_Index,0,FALSE);
				 }
				 //消耗声望
				 else
					 Player[m_nPlayerIdx].m_cRepute.AddReputeValue(-m_MapUseModel.nValue[1]);
			 }
		  }
	  }
  } 


   /* if (m_AutoRescueSkill.nTime)
	{
		m_AutoRescueSkill.nTime--;
	}
	else
	{
		m_AutoRescueSkill.nValue[0] = 0;
		m_AutoRescueSkill.nValue[1] = 0;
	}*/
	// 物理护盾
	if (m_PhysicsArmor.nTime)
	{
		m_PhysicsArmor.nTime--;
	}
	else
	{
		m_PhysicsArmor.nValue[0] = 0;
	}
	// 冰护盾
	if (m_ColdArmor.nTime)
	{
		m_ColdArmor.nTime--;
	}
	else
	{
		m_ColdArmor.nValue[0] = 0;
	}
	// 火护盾
	if (m_FireArmor.nTime)
	{
		m_FireArmor.nTime--;
	}
	else
	{
		m_FireArmor.nValue[0] = 0;
	}
	// 毒护盾
	if (m_PoisonArmor.nTime)
	{
		m_PoisonArmor.nTime--;
	}
	else
	{
		m_PoisonArmor.nValue[0] = 0;
	}
	// 电护盾
	if (m_LightArmor.nTime)
	{
		m_LightArmor.nTime--;
	}
	else
	{
		m_LightArmor.nValue[0] = 0;
	}
	// 魔法盾--内力吸收伤害
	/*if (m_ManaShield.nTime)
	{
	m_ManaShield.nTime--;
	}
	else
	{
	m_ManaShield.nValue[0] = 0;
	}*/
//	m_PowerState;			// 怒火状态
	 //中毒状态

	// 混乱状态
	if (m_randmove.nTime > 0)
	{
		//m_ProcessAI	= 0;
		if (!(g_SubWorldSet.GetGameTime() % GAME_UPDATE_TIME))
		{
			int nDesX, nDesY, nMapid;
			GetMpsPos(&nDesX, &nDesY, &nMapid);
			int	nOffX, nOffY;
			nOffX = g_Random(150+m_Index%150);
			nOffY = g_Random(150+m_Index%150);
			if (nOffX & 1)
				nOffX = - nOffX;
			if (nOffY & 1)
				nOffY = - nOffY;
			SendSerCommand(do_walk, nDesX + nOffX, nDesY + nOffY);
		}
		m_randmove.nTime--;
		//if(m_randmove.nTime == 0)
		//m_ProcessAI	= 1;
	}



	if (m_PoisonState.nTime <= 0)
	{
		m_PoisonState.nTime = 0;
		m_CurrentPoisonTime = 0;
		m_nLastPoisonDamageIdx = 0;
		m_CurPoisonDamage = 0;
		if (!m_PoisonList.empty())
			m_PoisonList.clear();
	}

	if (m_IgnoreNegativeState.nTime > 0)//hoa giai sat thuong vo dang
	{	
		m_PoisonState.nTime = 0;
		m_FreezeState.nTime = 0;
		m_StunState.nTime = 0;
		//m_Poison2DecManaState.nTime = 0;
		//m_RandMoveState.nTime = 0;
		m_IgnoreNegativeState.nTime--;
	}
/*	
	if (m_PoisonState.nTime > 0)
	{ 
	//	printf("DEBUG Dame - rut doc: %d time: %d\n", m_CurPoisonDamage, m_PoisonState.nTime);
		if( (NpcSet.GetRelation(m_nLastPoisonDamageIdx, this->m_Index) & relation_enemy) && (this->m_CurrentLife > 0) )
		{
			m_PoisonState.nTime--;
			if (m_PoisonState.nValue[1] == 0)
			{
				m_PoisonState.nValue[1] = 1;
			}
			if (!(m_PoisonState.nTime % m_PoisonState.nValue[1]))
			{	
				int nPoisonDamage = 0;
				nPoisonDamage = CalcDamage(m_nLastPoisonDamageIdx, m_PoisonList.front(), m_PoisonList.front(), 0, damage_poison, FALSE, FALSE, TRUE);

				if(nPoisonDamage > 0 && !IsPlayer())
				{
					Npc[m_nLastPoisonDamageIdx].SendBloodNo(m_dwID, nPoisonDamage);
				}
			}
		}
		else
		{ 
			this->m_PoisonState.nTime = 0;
		}
	}
*/

	if (m_PoisonState.nTime > 0)
	{
		m_PoisonState.nTime--;

		if (m_PoisonState.nValue[0] <= 0)
			m_PoisonState.nTime = 0;

		if (m_PoisonState.nValue[1] <= 0) 
			m_PoisonState.nValue[1] = 1;

		if (!(m_PoisonState.nTime % m_PoisonState.nValue[1]) && m_nLastPoisonDamageIdx > 0)
		{//是五毒才暴血
			if (!m_PoisonList.empty())
			{
				m_CurPoisonDamage += CalcDamage(m_nLastPoisonDamageIdx, m_PoisonList.front(), m_PoisonList.front(), 0, damage_poison, FALSE, FALSE, TRUE);
				//printf("DEBUG Dame - rut doc: %d time: %d\n", m_CurPoisonDamage, m_PoisonState.nTime);
				m_PoisonList.pop_front();
				m_CurrentLife -= m_CurPoisonDamage;
				 
				if (m_CurrentLife<=0)
				{ 
					m_CurrentLife = 0;
					int nMode = DeathCalcPKValue(m_nLastPoisonDamageIdx);
					DoDeath(nMode, m_nLastPoisonDamageIdx);
					if (m_Kind == kind_player)
						Player[m_nPlayerIdx].m_cPK.CloseAll();
				}
			}
			else
			{
				m_CurPoisonDamage = 0;
			}
		}
	}





	// 冰冻状态
	if (m_FreezeState.nTime > 0)
	{
		m_FreezeState.nTime--;
		if (m_FreezeState.nTime & 1)  //为奇数时不额能动
		{//冰冻状态不能动了
			nRet = TRUE;
		}
	}

	if  (m_BurnState.nTime <= 0)
	{
		m_CurFireDamage      = 0;
		m_nLastBurnDamageIdx = 0;
	}
	// 燃烧状态 --火伤害
	if (m_BurnState.nTime > 0)
	{
		m_BurnState.nTime--;

		if (m_BurnState.nValue[1] == 0)
		{
			m_BurnState.nValue[1] = 1;
		}

		if (m_nLastBurnDamageIdx && !(m_BurnState.nTime % m_BurnState.nValue[1]))
		{
			//实时同步计算伤害值
			m_CurFireDamage = CalcDamage(m_nLastBurnDamageIdx,0, m_BurnState.nValue[0], m_BurnState.nValue[0], damage_fire, TRUE, FALSE);
		    m_CurrentLife -= m_CurFireDamage;   //被攻击者

			//printf("Debug: m_CurFireDamage: %d",m_CurFireDamage);
			if (m_CurrentLife<=0)      //被攻击者
			{
				m_CurrentLife=0;
				int nMode = DeathCalcPKValue(m_nLastBurnDamageIdx); //攻击者 惩罚模式
				DoDeath(nMode,m_nLastBurnDamageIdx);                //没有血了 死亡
				//if 死的是玩家，关闭他的仇杀和切磋状态
				if (m_Kind == kind_player)
					Player[m_nPlayerIdx].m_cPK.CloseAll();
			}  
		}
	}
	//  迷惑状态
	if (m_ConfuseState.nTime > 0)
	{
		m_ConfuseState.nTime--;
		if (m_ConfuseState.nTime <= 0)
		{//还原阵营
			m_CurrentCamp = m_Camp;
		}
	}
	// 眩晕状态
	if (m_StunState.nTime > 0)
	{//一直不能动
		m_StunState.nTime--;
		nRet = TRUE;
	}
	// 被抓捕状态 --不能动
	if (m_ZhuaState.nTime > 0 && m_Kind==kind_normal)
	{
		m_ZhuaState.nTime--;
		m_ZhuaVal++;
		//nRet = TRUE;
	}
	else if (m_ZhuaState.nTime <= 0 && m_Kind==kind_normal)
	{
         m_ZhuaVal =0;
		 //ZeroMemory(m_GuishuName,sizeof(m_GuishuName));
		 //sprintf(m_GuishuName,"系统");
	}
	// 补血状态
	if (m_LifeState.nTime > 0)
	{
		m_LifeState.nTime--;

		if (!(m_LifeState.nTime % GAME_UPDATE_TIME))
		//if (m_LifeState.nTime - (m_LifeState.nTime>>3<<3)==0)
		{
			m_CurrentLife += m_LifeState.nValue[0];
			if (m_CurrentLife > m_CurrentLifeMax)
				m_CurrentLife = m_CurrentLifeMax;
		}
	}
	// 补MANA状态
	if (m_ManaState.nTime > 0)
	{
		m_ManaState.nTime--;
		if (!(m_ManaState.nTime % GAME_UPDATE_TIME))
		//if (m_ManaState.nTime - (m_ManaState.nTime>>3<<3)==0)
		{
			m_CurrentMana += m_ManaState.nValue[0];
			if (m_CurrentMana > m_CurrentManaMax)
			{
				m_CurrentMana = m_CurrentManaMax;
			}
		}
	}
	// 醉酒状态
	if (m_DrunkState.nTime > 0)
	{
		m_DrunkState.nTime--;
	} 
   //骑马状态时间//
	if (m_RideState > 0)
	{
		m_RideState--;
	}
#endif

#ifndef _SERVER
	// 怒气半秒自然回复
	//if (!(m_LoopFrames % GAME_UPDATE_TIME))
	if (m_LoopFrames-(m_LoopFrames>>3<<3)==0)
	{
	    m_CurrentNuQi += m_CurrentNuQiReplenish;
	  if (m_CurrentNuQi > m_CurrentNuQiMax)
		 m_CurrentNuQi = m_CurrentNuQiMax;		        // 怒气半秒自然回复

//	    m_CurrentFuMoVal += m_CurrentFuMoValReplenish; 
//	  if (m_CurrentFuMoVal > m_CurrentFuMoValMax)
//		  m_CurrentFuMoVal = m_CurrentFuMoValMax;		//附魔半秒自然回复
	}

	//if (!(m_LoopFrames % (18*30)))
	if  (m_LoopFrames-18*30*(m_LoopFrames/(18*30))==0)
	{
	    m_IsHaveAttack=0;  //是否设置为攻击无效了
	    m_AttackerDwid=0;  //上次攻击着的DWID
	}

/*
	int					m_CurrentFuMoVal;		// Npc的当前附魔值
	int					m_CurrentFuMoValReplenish;	// Npc的当前附魔值回复速度
    int	                m_FuMoTimesVal;		    // Npc的当前附魔持续时间值（间隔值）
	int					m_CurrentFuMoValMax;    // Npc的当前附魔最大值
*/

	bool bAdjustColorId = false;

	if (m_FreezeState.nTime > 0)
	{//冰冻
		m_DataRes.SetAdjustColorId(KNpcRes::adjustcolor_freeze); //
		bAdjustColorId = true;

		if (SubWorld[0].m_dwCurrentTime & 1)
			nRet = TRUE;
	}
	
	if (m_StunState.nTime > 0)
	{//眩晕
		m_DataRes.SetAdjustColorId(KNpcRes::adjustcolor_stun);
		bAdjustColorId = true;
		nRet = TRUE;
	}

	//if (m_PoisonState.nTime > 0)
	if (m_PoisonState.nTime>0)
	{//毒
	//	m_PoisonState.nTime--;
		m_DataRes.SetAdjustColorId(KNpcRes::adjustcolor_poison);
		bAdjustColorId = true;
	}
	// 怒满状态(战斗模式才上色)
/*	if (m_CurrentNuQi >= m_CurrentNuQiMax && m_FightMode==1)
	{//怒去
		m_DataRes.SetAdjustColorId(KNpcRes::adjustcolor_red);
		bAdjustColorId = true;
	}
	*/
	//被抓捕状态上色
    /*if (m_ZhuaState.nTime > 0)
	{
		m_DataRes.SetAdjustColorId(KNpcRes::adjustcolor_confuse);
		//nRet = TRUE;
		bAdjustColorId = true;
	}*/ 
	// 燃烧状态
	if (m_BurnState.nTime > 0)
	{
	//	m_BurnState.nTime--;
		m_DataRes.SetAdjustColorId(KNpcRes::adjustcolor_burn);
		bAdjustColorId = true;
	}
	// 混乱状态
	if (m_ConfuseState.nTime > 0)
	{
	//	m_ConfuseState.nTime--;
		m_DataRes.SetAdjustColorId(KNpcRes::adjustcolor_confuse);
		bAdjustColorId = true;
	}

	if (!bAdjustColorId)
		m_DataRes.SetAdjustColorId(KNpcRes::adjustcolor_physics);

#endif

//状态的移除 和 同步=========================================
	KStateNode* pNode=NULL;
	            pNode = (KStateNode *)m_StateSkillList.GetHead();
	while(pNode)
	{
		KStateNode* pTempNode = pNode;
        pNode = (KStateNode *)pNode->GetNext();
		if (pTempNode->m_LeftTime <= -1)	
		{//被动技能 无时间限制的不会被清除！
			continue;
		}
	
		if (pTempNode->m_LeftTime == 0 && pTempNode->m_IsClientState==0)    //第几个状态时间 服务器端
		{//移除技能状态 有时间限制的
			int i;
			for (i = 0; i < MAX_SKILL_STATE; ++i)
			{//重新计算技能状态属性各项值
				if (pTempNode->m_State[i].nAttribType) //所有状态中的第几个属性
				{//移除属性值
					KMagicAttrib nMagicAttrib;   
					nMagicAttrib.nAttribType = pTempNode->m_State[i].nAttribType;
					nMagicAttrib.nValue[0]   = pTempNode->m_State[i].nValue[0];
					nMagicAttrib.nValue[1]   = pTempNode->m_State[i].nValue[1];
					nMagicAttrib.nValue[2]   = pTempNode->m_State[i].nValue[2];
                    ModifyAttrib(m_Index, &nMagicAttrib);
				}
			}
			//ReCalcStateEffect();
			if (pTempNode != NULL)
			{
			   pTempNode->Remove();
			   delete pTempNode;
			   pTempNode = NULL;
			}
#ifdef _SERVER
			UpdateNpcStateInfo();
			//BroadCastState();         //更新状态同步给周围玩家
#endif
			continue;
		}
		else
		{//时间减少
			pTempNode->m_LeftTime--;
			//画图标
			//======
		}
	}
///===========================================================
	return nRet;
}

int KNpc::GetSkillLeftTime(int nSkillId)
{
    KStateNode* pNode;
	    pNode = (KStateNode *)m_StateSkillList.GetHead(); //第一个节点的 技能
	//KStateNode *mNode =(KStateNode *)m_StateSkillList.GetStatusNode(5);  //第5个节点的技能
	int nMunTime=-1;

	while(pNode)
	{
		if (pNode->m_SkillID == nSkillId)
		{//ID相同 如果有这个技能
			nMunTime= pNode->m_LeftTime;    
			break;
		}
		pNode = (KStateNode *)pNode->GetNext();
	}

    return nMunTime;
}


int KNpc::SetSkillLeftTime(int nSkillId,int nTime)
{
    KStateNode* pNode;
	pNode = (KStateNode *)m_StateSkillList.GetHead(); //第一个节点的 技能
	//KStateNode *mNode =(KStateNode *)m_StateSkillList.GetStatusNode(5);  //第5个节点的技能
	int nReut=0;
	while(pNode)
	{
		if (pNode->m_SkillID == nSkillId)
		{//ID相同
			nReut =pNode->m_LeftTime;
			pNode->m_LeftTime=nTime;
			//----------------------
			break;
		}
		pNode = (KStateNode *)pNode->GetNext();
	}

#ifdef _SERVER
if  (IsPlayer())
{
    PLAYER_ONESTATE_SYNC mStateSync;
	mStateSync.ProtocolType	= (BYTE)s2c_synconestate;
	mStateSync.ID			= m_dwID;
	mStateSync.m_Skillid	= nSkillId;
	mStateSync.m_Time       = nTime;
	if (g_pServer)
	  g_pServer->PackDataToClient(Player[GetPlayerIdx()].m_nNetConnectIdx, &mStateSync, sizeof(PLAYER_ONESTATE_SYNC));
}
#endif

	return nReut;

}


void KNpc::ClearOneSkillState(int nSkillId)
{
#ifdef _SERVER

    if (nSkillId<=0)
		return;

	KStateNode * pNode = (KStateNode*)m_StateSkillList.GetHead();
	KStateNode * pTempNode = NULL;	
	while(pNode)
	{
		pTempNode = pNode;
		pNode = (KStateNode*) pNode->GetNext();
		if (pNode->m_SkillID == nSkillId)
		{
		    int i;
			for (i = 0; i < MAX_SKILL_STATE; ++i)
			{//重新计算技能状态属性各项值
				if (pTempNode->m_State[i].nAttribType) //所有状态中的第几个属性
				{//移除属性值
					KMagicAttrib nMagicAttrib;   
					nMagicAttrib.nAttribType = pTempNode->m_State[i].nAttribType;
					nMagicAttrib.nValue[0]   = pTempNode->m_State[i].nValue[0];
					nMagicAttrib.nValue[1]   = pTempNode->m_State[i].nValue[1];
					nMagicAttrib.nValue[2]   = pTempNode->m_State[i].nValue[2];
                    ModifyAttrib(m_Index, &nMagicAttrib);
				}
			}

			if (pTempNode != NULL)
			{
			   pTempNode->Remove();
			   delete pTempNode;
			   pTempNode = NULL;
			}
		    break;
		}
	}

	UpdateNpcStateInfo();
	//BroadCastState();         //更新状态同步给周围玩家

	/*PLAYER_ONESTATE_SYNC mStateSync;
	mStateSync.ProtocolType	= (BYTE)s2c_synconestate;
	mStateSync.ID			= m_dwID;
	mStateSync.m_Skillid	= nSkillId;
	if (g_pServer)
	  g_pServer->PackDataToClient(Player[GetPlayerIdx()].m_nNetConnectIdx, &mStateSync, sizeof(PLAYER_ONESTATE_SYNC));
	*/
#endif 

return;
}

//玩家死亡 Npc死亡？
void KNpc::DoDeath(int nMode,int nLastDamageIdx)
{
	if  (m_SubWorldIndex<0 || m_SubWorldIndex>=MAX_SUBWORLD ||m_RegionIndex<0) 
		return;

	if  ( m_RegionIndex >=SubWorld[m_SubWorldIndex].m_nTotalRegion)
		return;

	if (m_Doing == do_death)
		return;

	if (nLastDamageIdx<=0 || nLastDamageIdx>=MAX_NPC)
	{

		if (IsActiveForever())
			SetActiveForever(FALSE);//删除了

		if (m_Kind == kind_normal)
		{	
			if (!_delayReMove && m_GameliveTime>0)//时间没有到 就给杀死的
				m_GameliveTime = 0;//设置给系统杀死*/
		}
		goto NEXT;
		return;
	}
//服务器端
	if (IsPlayer() && !m_FightMode)	// 城镇内不会死亡（有效）
	{
		m_CurrentLife = 1;  
		return;
	}

	if (IsPlayer())//（有效）
	{
		Player[m_nPlayerIdx].m_ItemList.SetMaskLock(FALSE);// 死亡者
		int nIdx = Player[m_nPlayerIdx].m_ItemList.GetEquipment(itempart_mask); //
		m_MaskType = Item[nIdx].GetBaseMagic();
	}

#ifndef _SERVER

	m_DataRes.SetBlur(FALSE);	 //清除残影

	if (this->m_Kind == kind_normal || this->m_Kind == kind_partner)  //所有都冒血处理
		this->SetBlood(this->m_CurrentLife);  //冒血数字显示为当前生命this->m_CurrentLife
#endif


#ifdef _SERVER
	int nPlayer = 0;
	// 杀死玩家不得经验
	if (m_Kind == kind_normal)//if (m_Kind!= kind_player && m_Kind!= kind_partner)  //不是玩家 
	{
		nPlayer = m_cDeathCalcExp.CalcExp();   //分配经验  返回 攻击者NPC索引  经验增加
		
		if (IsActiveForever())
			SetActiveForever(FALSE);//删除了

		if (!_delayReMove && m_GameliveTime>0)  //时间没有到 就给杀死的
			m_GameliveTime =0;//设置给系统杀死 不能重生
	}

	if (m_Kind == kind_normal)
	{
		if (m_DropScriptID && nPlayer)
		{
			Player[nPlayer].ExecuteScript2(m_DropScriptID, "DropRate", m_Index, nPlayer);
		}
	}
	//bat auto khong roi do
	if (m_Kind==kind_normal && m_cGold.GetGoldType()>0 && m_cGold.GetGoldType()<5)
	{//被攻击者是黄金怪物
		if (Npc[nLastDamageIdx].m_Kind==kind_player && Npc[nLastDamageIdx].m_AutoplayId == 1)
		{  //攻击者是人并在挂机中 挂机中的 	
			int nPlaidx=0;
			nPlaidx=Npc[nLastDamageIdx].GetPlayerIdx();	
			if (Player[nPlaidx].m_Autoplay.nIsNoBlue==0)//如果没有开启了 秒杀蓝怪 就掉东西
				DeathPunish(nMode,nPlayer,nLastDamageIdx);
		}
		else
		{//没有挂机
		   DeathPunish(nMode,nPlayer,nLastDamageIdx);
		}
	}
	else
	 DeathPunish(nMode,nPlayer,nLastDamageIdx);                    //死亡惩罚 = 掉下物品 --掉经验-掉钱

NEXT:
	NPC_DEATH_SYNC	NetCommandA;
	NetCommandA.ProtocolType =(BYTE)s2c_npcdeath;          //发送死亡协议到客户端(BYTE)
	NetCommandA.ID = m_dwID;                               //死亡者的dwid--节点第几个NPC标示
    snprintf(NetCommandA.nDeathName, sizeof(NetCommandA.nDeathName), "%s", Name);

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

	int nMaxCount = MAX_BROADCAST_COUNT;//;MAX_PLAYER    CURREGION MAX_BROADCAST_COUNT 
    CURREGION.BroadCast(&NetCommandA, sizeof(NPC_DEATH_SYNC), nMaxCount, m_MapX, m_MapY);
	int i,nConRegion=-1;
	for (i = 0; i < 8; ++i)
	{
		nConRegion = CONREGIONIDX(i);
		if  (nConRegion<0 || nConRegion>=SubWorld[m_SubWorldIndex].m_nTotalRegion)
			continue;
		CONREGION(i).BroadCast(&NetCommandA, sizeof(NPC_DEATH_SYNC), nMaxCount, m_MapX - POff[i].x, m_MapY - POff[i].y);
	}

#endif
//要等分配经验完毕
	m_bLockNpcDwID       = 0;
	m_Doing              = do_death;      //指向调用 ondeath 函数
	m_ProcessAI	         = 0;
	m_ProcessState       = 0;
	m_Frames.nTotalFrame = m_DeathFrame;  //定格在死亡帧数	
	m_Height = 0;                         //高度为零
	m_CurrentLifeDamage  = 0;
#ifndef _SERVER
	m_ClientDoing = cdo_death;

	if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nPeopleIdx == m_Index)
	{
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nPeopleIdx = 0;
	}

	this->m_cGold.ClientClearState();  //删除黄金怪状态

	if (IsPlayer())
	{// 自动离队
		if (Player[m_nPlayerIdx].m_cTeam.m_nFlag)
		{
			Player[m_nPlayerIdx].LeaveTeam();
		}
	}

	//CoreDataChanged(GDCNI_OPEN_JINDUTIAO,0,0);  //死亡关闭进度条
#endif
}

//玩家 NPC死亡脚本  无限循环死亡状态
//int                 IsExeGoldScript;          //是否执行全局死亡脚本
//int                 IsCreatBoss;
void KNpc::OnDeath()
{
	if (WaitForFrame())
	{
		m_Frames.nCurrentFrame = m_Frames.nTotalFrame - 1;		// 保证不会有重回第一帧的情况m_Frames.nTotalFrame - 1
		//如果当间死者为主角，则看有没有死亡脚本，有则运行。
		char nMgs[256]={0},nstrName[64]={0};
		if (IsPlayer())
		{
			char szNpcIndex[30]={0};
			sprintf(szNpcIndex, "%d", m_nLastDamageIdx);

			if (Player[m_nPlayerIdx].m_dwDeathScriptId)
				Player[m_nPlayerIdx].ExecuteScriptD(Player[m_nPlayerIdx].m_dwDeathScriptId, "OnDeath", szNpcIndex,Name,m_Index,m_dwID,m_nPlayerIdx,0,0,TRUE);
			else
			{//否则执行全局脚本
				g_SubWorldSet.SubExecuteScript("\\script\\global\\golddeath.lua","OnPlayerDeath",szNpcIndex,Name,m_Index,m_dwID,m_nPlayerIdx);
			}
		}
		//如果死者是Npc，而打死他的是主角，并有死亡脚本则运行脚本
		else  if (Npc[m_nLastDamageIdx].IsPlayer())
		{//是玩家 打死怪物 或 同伴
			int nIdx = Npc[m_nLastDamageIdx].m_nPlayerIdx;  //玩家的索引

			if (nIdx)
			{
				char szNpcIndex[30];
				sprintf(szNpcIndex, "%d", m_nLastDamageIdx);
				// 执行战斗npc死亡脚本
				if  (m_Kind == kind_partner)
				{//同伴 类型 死亡
					int npIdx  = NpcSet.SearchID(m_GuiShuDwid);//全服务器npc查找 消耗性能比较高
                        npIdx   = Npc[npIdx].m_nPlayerIdx;
				   if (m_btRankFFId==88)
				   {//镖车死亡  
					  if (npIdx)//主人在线
						   Player[npIdx].ExecuteScript_NEW(NPC_PARTNER_SCRIPT, "OnDeath",m_nLastDamageIdx,Name,m_Index,m_njb,m_njxb,1,0,FALSE); 

					  //劫镖者执行
					  Player[nIdx].ExecuteScript_NEW(NPC_PARTNER_SCRIPT, "OnDeathTar",m_nLastDamageIdx,Name,m_Index,m_njb,m_njxb,npIdx,0,FALSE);
				   }  
				   else
				   {//同伴死亡
					   if (npIdx)//主人在线
						 Player[npIdx].ExecuteScript_NEW(NPC_PARTNER_SCRIPT, "OnDeath",m_nLastDamageIdx,Name,m_Index,m_njb,m_njxb,0,0,FALSE);
				   }
				}  
				else if (m_Kind!=kind_dialoger)
				{//怪物死亡
					g_SubWorldSet.SubExecuteScript("\\script\\global\\golddeath.lua","OnGoldDeath",szNpcIndex,Name,m_Index,m_dwID,nIdx);
		
					if (GetstrInfo(STR_ACTION_SCRIPT,nMgs,sizeof(nMgs)))
						Player[nIdx].ExecuteScript_NEW(nMgs, "OnDeath",m_nLastDamageIdx,Name,m_Index,m_njb,m_njxb,0,0,FALSE);
				}
			}
		}
		else if (Npc[m_nLastDamageIdx].m_Kind == kind_partner)
		{//是同伴 打死怪物 或 同伴
			int nIndx = NpcSet.SearchID(Npc[m_nLastDamageIdx].m_GuiShuDwid);//NpcSet.SearchName(nstrName); //PlayerSet.FindPlayerID(Npc[m_nLastDamageIdx].m_GuiShuDwid)(是PLAYER 的 dwid)
            INT nIdx  = Npc[nIndx].m_nPlayerIdx;
		    if (nIdx)
			{ //主人的索引
              char szNpcIndex[30];
			  sprintf(szNpcIndex, "%d", m_nLastDamageIdx);
			  Player[nIdx].ExecuteScript_NEW(NPC_PARTNER_SCRIPT, "OnDeath",m_nLastDamageIdx,Name,m_Index,m_njb,m_njxb,0,0,FALSE);
			} 
		}
        else if  (m_Kind == kind_partner)
		{//同伴死亡
			int  npIdx  =  NpcSet.SearchID(m_GuiShuDwid);//NpcSet.SearchName(nstrName);
                  npIdx  =  Npc[npIdx].m_nPlayerIdx;
			if (npIdx)
			{  //主人的索引
				char szNpcIndex[30];
				sprintf(szNpcIndex, "%d", m_nLastDamageIdx);
					Player[npIdx].ExecuteScript_NEW(NPC_PARTNER_SCRIPT, "OnDeath",m_nLastDamageIdx,Name,m_Index,m_njb,m_njxb,0,0,FALSE);
			}
		}
		else
		{//其他情况,动物杀手动物,执行固定脚本
			char szNpcIndex[30];
			sprintf(szNpcIndex, "%d", m_nLastDamageIdx);
		    if (GetstrInfo(STR_ACTION_SCRIPT,nMgs,sizeof(nMgs)))
			    g_SubWorldSet.SubExecuteScript(nMgs,"OnDeath",szNpcIndex,Name,m_Index,m_dwID,m_njxb);
		}

		if (m_Index <= 0)	// 本Npc在死亡脚本里面被删了
		   return;

		if (!IsPlayer())
		{
			if (Partner._damageRatio>1)
			{
				 m_PhysicsDamage.nValue[0]	/=Partner._damageRatio;	  //基本伤害
				 m_PhysicsDamage.nValue[2]	/=Partner._damageRatio;	  //基本伤害
			}

			this->m_cGold.RecoverBackData();   //黄金怪物死亡后 恢复 Npc 基本数据
		}
		// 重生点
		if (m_Kind != kind_partner)//战斗Npc时
		{//怪物与人
			if (this->m_cGold.GetGoldType() <= 4) //不是黄金怪物就复活 1-4 5-11 12-16
				DoRevive();  //开始重生	  已经删除了节点 加入重生列表
		  
			/*if (this->m_Kind == kind_normal)
				NpcSet.ExecuteScript(m_Index, m_ActionScriptID, "DeathSelf", m_Index);*/


			if (this->m_cGold.GetGoldType() > 4)  // 黄金怪物节点  不能重生
			{
				if (m_SubWorldIndex>=0 && m_RegionIndex >= 0)
				{
					SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].RemoveNpc(m_Index);
				   SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].DecRef(m_MapX, m_MapY,obj_npc);
				}
				NpcSet.Remove(m_Index);//同不到客户端删除节点
			}
		}
		else
		{//同伴类直接不能重生
			if (m_SubWorldIndex>=0 && m_RegionIndex >= 0)
			{
			  SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].RemoveNpc(m_Index);
			 SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].DecRef(m_MapX,m_MapY,obj_npc);
			}
			NpcSet.Remove(m_Index);
		}
	}
	else
	{
//		printf("[DEATH] 死亡帧数错误WaitForFrame FALSE\n");
	}
}

void KNpc::DoDefense() //档格
{
	m_ProcessAI = 0;
}

void KNpc::OnDefense()  //档格
{

}

void KNpc::DoIdle()
{
	if (m_Doing == do_idle)
		return;
	m_Doing = do_idle;
}

void KNpc::OnIdle()
{

}

#ifdef _SERVER
BOOL KNpc::DoDrag(INT nTotalFrame, INT nDragSpeend, INT nParam1, INT nParam2)
{
	if (m_RegionIndex < 0 || m_Doing == do_death)
		return FALSE;
	if (m_Doing == do_drag)
		return FALSE;

	//if (!m_nLastCollisionNpcIdx)
	//	return FALSE;
	
	// 计算拉扯终止点
	INT nMyX, nMyY,nMap;
	GetMpsPos(&nMyX, &nMyY,&nMap);
	
	INT nOpX, nOpY,nOpMap;
	if (nParam1 == -1)
	{
		Npc[nParam2].GetMpsPos(&nOpX, &nOpY,&nOpMap);
	}
	else
	{
		nOpX = nParam1;
		nOpY = nParam2;
	}
	INT nDragDistance = nDragSpeend * nTotalFrame;
	
	if (nOpX != nMyX || nOpY != nMyY)	// 两人不在同一点
		m_Dir	= g_GetDirIndex(nOpX, nOpY, nMyX, nMyY);
	INT nTargetX = nOpX;
	INT nTargetY = nOpY;
	
	
	if(!TestMovePos(nTargetX, nTargetY, nDragDistance, MIN(nDragSpeend, (defLOGIC_CELL_WIDTH-1)), TRUE))
		return FALSE;
	
	m_DesX	= nTargetX;
	m_DesY	= nTargetY;
	
	//SetDoing(do_drag);
	m_Doing = do_drag;
	m_ProcessAI = 0;
	//m_ProcessState = 0;
	
	//ClearProcessAI();
	
	m_Frames.SetFrame((nTotalFrame * nDragSpeend)>>5);
	
	return TRUE;
}
#else
VOID KNpc::DoDrag(INT nDragFrame, INT nDesX, INT nDesY)
{
	if (m_RegionIndex < 0 || m_Doing == do_death)
		return;
	if (m_Doing == do_drag)
		return;
	
	INT nMyX, nMyY,nMap;
	GetMpsPos(&nMyX, &nMyY,&nMap);
	if (nDesX != nMyX || nDesY != nMyY)
		m_Dir	= g_GetDirIndex(nDesX, nDesY, nMyX, nMyY);
	
	m_Doing = do_drag;
	
	//ClearProcessAI();
	m_ProcessAI = 0;
	//m_ProcessState = 0;
	
	m_ClientDoing = cdo_hurt;
	
	m_Frames.SetFrame(nDragFrame);
	
	m_DesX	= nDesX;
	m_DesY	= nDesY;
}
#endif

//拉扯过来
VOID KNpc::OnDrag()
{
	if (m_RegionIndex < 0)
		return;
	
	INT nX, nY,nMap;
	GetMpsPos(&nX, &nY,&nMap);
	
	INT nRestSteps	= m_Frames.nTotalFrame - m_Frames.nCurrentFrame;
	if (nRestSteps <= 0)
		nRestSteps	= 1;
	
	INT nMoveX	= ((m_DesX - nX) << 10) / nRestSteps;
	INT nMoveY  = ((m_DesY - nY) << 10) / nRestSteps;
	
	MovePos(nMoveX, nMoveY);
	
	if (WaitForFrame())
	{
		DoStand();
		//SetProcessAI();
		m_ProcessAI = 1;
	}
}

// -------------------------------------------------------------------------
// 函数		: KNpc::MovePos
// 功能		: 移动一定偏移量
// 返回值	: VOID 
// 参数		: INT nMoveX, nMoveY	Npc在格子中的偏移坐标（放大了1024倍）
// 作者		: FanZai
// 附注		: 来自ServeMove与ServeJump，合并重复代码
// -------------------------------------------------------------------------
#define CORRECT_SYNC_RANGE 30 

VOID KNpc::MovePos(INT nMoveX, INT nMoveY)
{
	INT nOldRegion = m_RegionIndex;
	INT nOldMapX = m_MapX;
	INT nOldMapY = m_MapY;
	INT nOldOffX = m_OffX;
	INT nOldOffY = m_OffY;

	m_OffX += nMoveX;
	m_OffY += nMoveY;

	if (!m_bClientOnly)
		CURREGION.DecRef(m_MapX, m_MapY,obj_npc);
		//CURREGION.DecNpcRef(m_MapX, m_MapY);

	INT nCellWidth	= (SubWorld[m_SubWorldIndex].m_nCellWidth << 10);
	INT nCellHeight	= (SubWorld[m_SubWorldIndex].m_nCellHeight << 10);

	if (nCellWidth <= 0 || nCellHeight <= 0)
	{
		_ASSERT(FALSE);
		return;
	}

	while (m_OffX < 0)
	{
		m_MapX--;
		m_OffX += nCellWidth;
	}
	while (m_OffX >= nCellWidth)
	{
		m_MapX++;
		m_OffX -= nCellWidth;
	}

	while (m_OffY < 0)
	{
		m_MapY--;
		m_OffY += nCellHeight;
	}
	while (m_OffY >= nCellHeight)
	{
		m_MapY ++;
		m_OffY -= nCellHeight;
	}
	
	if (m_MapX < 0)
	{
		m_RegionIndex = LEFTREGIONIDX;
		m_MapX += REGIONWIDTH;
	}
	else if (m_MapX >= REGIONWIDTH)
	{
		m_RegionIndex = RIGHTREGIONIDX;
		m_MapX -= REGIONWIDTH;
	}
	
	if (m_RegionIndex >= 0)
	{
		if (m_MapY < 0)
		{
			m_RegionIndex = UPREGIONIDX;
			m_MapY += REGIONHEIGHT;
		}
		else if (m_MapY >= REGIONHEIGHT)
		{
			m_RegionIndex = DOWNREGIONIDX;
			m_MapY -= REGIONHEIGHT;
		}
		if (!m_bClientOnly && m_RegionIndex >= 0)
			//CURREGION.AddNpcRef(m_MapX,m_MapY);
			CURREGION.AddRef(m_MapX, m_MapY,obj_npc);
	}

	if (m_RegionIndex < 0)	// 不可能移动到-1 Region，如果出现这种情况，恢复原坐标
	{
#ifdef _SERVER
		m_RegionIndex = nOldRegion;
		m_MapX = nOldMapX;
		m_MapY = nOldMapY;
		m_OffX = nOldOffX;
		m_OffY = nOldOffY;
		//CURREGION.AddNpcRef(m_MapX, m_MapY);
		CURREGION.AddRef(m_MapX, m_MapY,obj_npc);
		return;
#else
		SubWorld[0].m_Region[nOldRegion].RemoveNpc(m_Index);
		m_RegionIndex = -1;
		return;
#endif

	}
	
	_ASSERT(m_OffX < nCellWidth && m_OffY < nCellHeight);
#ifndef _SERVER
	//快到同步边缘时同步一下坐标
	if (!IsPlayer() && (NpcSet.GetMapDisX(m_Index, Player[CLIENT_PLAYER_INDEX].m_nIndex) >= CORRECT_SYNC_RANGE ||
		NpcSet.GetMapDisY(m_Index, Player[CLIENT_PLAYER_INDEX].m_nIndex) >= CORRECT_SYNC_RANGE ))
	{
//		ForceSyncPos(TRUE);
	}
#endif

	if (nOldRegion != m_RegionIndex)
	{
#ifdef _SERVER
		SubWorld[m_SubWorldIndex].NpcChangeRegion(nOldRegion, m_RegionIndex, m_Index);
		if (IsPlayer())
		{
			SubWorld[m_SubWorldIndex].PlayerChangeRegion(nOldRegion, m_RegionIndex, m_nPlayerIdx);
			// zhengyuhua 移动不需要扣耐久了
			//if (m_nPlayerIdx > 0)
			//	Player[m_nPlayerIdx].m_cPlayerItem.Abrade();
		}
#else
		SubWorld[0].NpcChangeRegion(SubWorld[0].m_Region[nOldRegion].m_RegionID, SubWorld[0].m_Region[m_RegionIndex].m_RegionID, m_Index);
		m_dwRegionID = SubWorld[0].m_Region[m_RegionIndex].m_RegionID;
#endif
	}
    JX3D_M1_DEV_AOI_EMIT_MOVE(m_Index);
}


//做受伤动作 //hieu ung giat
void KNpc::DoHurt(int nHurtFrames /*= 0*/, int nX /*= 0*/, int nY /*= 0*/,int nHurtI) 
{
	if  (m_SubWorldIndex<0 || m_SubWorldIndex>=MAX_SUBWORLD ||m_RegionIndex<0) 
		return;
	if  ( m_RegionIndex >=SubWorld[m_SubWorldIndex].m_nTotalRegion)
		return; 

#ifdef _SERVER
	if (m_nImmortal > 0) 
		return;
#endif
	if (m_Doing == do_hurt || m_Doing == do_death || m_Doing == do_revive || m_Doing == do_runattack)
	return;

	//_ASSERT(m_RegionIndex >= 0);
#ifndef _SERVER
	m_DataRes.SetBlur(FALSE);
#endif
	if (m_RegionIndex < 0)
		return;
		
	if (m_IgnoreNegativeState.nValue[1] > 1)
		return;

#ifdef _SERVER

		int giam_tho_thuong = 0;
		if (m_CurrentHitRecover <= 80)
		{
			giam_tho_thuong = (m_CurrentHitRecover/10) * 10;
		}
		else
		{
			giam_tho_thuong = 80;
		}
		if (!g_RandPercent(nHurtI/2+50-giam_tho_thuong/2))
			return;
#endif
	m_Doing = do_hurt;
	m_ProcessAI	= 0;

#ifdef _SERVER
	//m_Frames.nTotalFrame = m_HurtFrame * (100 - m_CurrentHitRecover) / 100;
	m_Frames.nTotalFrame = 18 * m_HurtFrame * nHurtI *(100 - giam_tho_thuong)/ 100000;
#else
	m_ClientDoing = cdo_hurt;
	m_Frames.nTotalFrame = m_HurtFrame * (100 - m_CurrentHitRecover) / 100;;
	m_nHurtDesX = nX;
	m_nHurtDesY = nY;
	if (m_Height > 0)
	{
		m_nHurtHeight = m_Height;
	}
	else
	{
		m_nHurtHeight = 0;
	}
#endif
	if (m_Frames.nTotalFrame == 0)
		m_Frames.nTotalFrame = 1;
	m_Frames.nCurrentFrame = 0;

#ifdef _SERVER
	NPC_HURT_SYNC	NetCommand;
	NetCommand.ProtocolType = (BYTE)s2c_npchurt;
	NetCommand.ID = m_dwID;
	NetCommand.nFrames = m_Frames.nTotalFrame;
	int NetMap=0;
	GetMpsPos(&NetCommand.nX, &NetCommand.nY,&NetMap);  //当前坐标

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
	CURREGION.BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, m_MapX, m_MapY);
	int i, nConRegion = -1;
	for (i = 0; i < 8; i++)
	{
		nConRegion = CONREGIONIDX(i);
		if (nConRegion < 0 || nConRegion >= SubWorld[m_SubWorldIndex].m_nTotalRegion)
			continue;
		CONREGION(i).BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, m_MapX - POff[i].x, m_MapY - POff[i].y);
	}
#endif
}

void KNpc::OnHurt()
{
	if (m_RegionIndex < 0)
		return;

	int nX, nY,nMap;
	GetMpsPos(&nX, &nY,&nMap);
#ifdef _SERVER
	m_Height = 0;
#endif
#ifndef _SERVER

	_ASSERT(m_Frames.nTotalFrame>0);

	m_Height = m_nHurtHeight * (m_Frames.nTotalFrame - m_Frames.nCurrentFrame - 1) / m_Frames.nTotalFrame;
	nX = nX + (m_nHurtDesX - nX) * m_Frames.nCurrentFrame / m_Frames.nTotalFrame;
	nY = nY + (m_nHurtDesY - nY) * m_Frames.nCurrentFrame / m_Frames.nTotalFrame;

	int nOldRegion = m_RegionIndex;
	//SetPos(nX, nY);
	CURREGION.DecNpcRef(m_MapX, m_MapY);

	int nRegion, nMapX, nMapY, nOffX, nOffY;
	nRegion = -1;
	nMapX = nMapY = nOffX = nOffY = 0;
	SubWorld[m_SubWorldIndex].Mps2Map(nX, nY, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);
	if (nRegion == -1)
	{
		SubWorld[0].m_Region[nOldRegion].RemoveNpc(m_Index);
		m_dwRegionID = 0;
	}
	else if (nOldRegion != nRegion || nRegion < 0)
	{
		m_RegionIndex = nRegion;
		m_MapX = nMapX;
		m_MapY = nMapY;
		m_OffX = nOffX;
		m_OffY = nOffY;
		SubWorld[0].NpcChangeRegion(SubWorld[0].m_Region[nOldRegion].m_RegionID, SubWorld[0].m_Region[m_RegionIndex].m_RegionID, m_Index);
		m_dwRegionID = SubWorld[0].m_Region[m_RegionIndex].m_RegionID;
	}
	if (nRegion >= 0)
		CURREGION.AddNpcRef(m_MapX,m_MapY);
		//CURREGION.AddRef(m_MapX, m_MapY, obj_npc);
	//SubWorld[m_SubWorldIndex].m_Region[nRegion].AddNpcRef(m_MapX,m_MapY);
#endif

	if (WaitForFrame())   //等待的帧数
	{
	//	g_DebugLog("[DEATH]On Hurt Finished");                 //受到伤害的动作
	   if (m_Kind==kind_player)
	   {
		 #ifdef _SERVER
		   Player[GetnPlayerIdx()].UpdataCurData();//UpdataCurData();                 //更新自身的属性参数
		 #else
           Player[CLIENT_PLAYER_INDEX].UpdataCurData();
         #endif
	   }
		DoStand();
		m_ProcessAI = 1; //完成受伤动作
	}
}

void KNpc::DoSpecial1()
{
	 DoBlurAttack();
}

void KNpc::OnSpecial1()
{
	if (WaitForFrame() &&m_Frames.nTotalFrame != 0)
	{
#ifndef _SERVER
		m_DataRes.SetBlur(FALSE);
#endif
		DoStand();
		m_ProcessAI = 1;	
	}
	else if (IsReachFrame(ATTACKACTION_EFFECT_PERCENT))
	{
		KSkill * pSkill = (KSkill*)GetActiveSkill();
		if (pSkill)
		{//m_ActiveSkListIndex
			int nEnChance = m_SkillList.GetEnChance(m_ActiveSkListIndex); //m_SkillList.FindSame(pSkill->GetSkillId())
			pSkill->m_nEnChance = nEnChance;
			int nChildSkill = pSkill->GetChildSkillId();

			int nChildSkillLevel = pSkill->m_ulLevel;
			
			if (nChildSkill > 0)
			{
				KSkill * pChildSkill = (KSkill*)g_SkillManager.GetSkill(nChildSkill, nChildSkillLevel);
				if (pChildSkill)
				{
					pChildSkill->m_nEnChance = nEnChance;
					pChildSkill->Cast(m_Index, m_SkillParam1, m_SkillParam2);
				}
			}
		}

        if (m_Kind==kind_player)
            NpcFuMoCastSkll(m_Index, m_SkillParam1, m_SkillParam2);

		if (m_Frames.nTotalFrame == 0)
		{
			m_ProcessAI = 1;
		}
	}
}

void KNpc::DoSpecial2()
{
}

void KNpc::OnSpecial2()
{

	if (WaitForFrame() &&m_Frames.nTotalFrame != 0)
	{
#ifndef _SERVER
		m_DataRes.SetBlur(FALSE);
#endif
		DoStand();
		m_ProcessAI = 1;	
	}
	else if (IsReachFrame(ATTACKACTION_EFFECT_PERCENT))
	{
		ISkill * pSkill = GetActiveSkill();
		eSkillStyle eStyle = (eSkillStyle)pSkill->GetSkillStyle();
		switch(eStyle)
		{
		case SKILL_SS_Thief:
			{
				((KThiefSkill*)pSkill)->OnSkill(this);  //偷取技能
			}
			break;
		} 
		
		if (m_Frames.nTotalFrame == 0)
		{
			m_ProcessAI = 1;
		}
	}
	
}

void KNpc::DoSpecial3()
{
}

void KNpc::OnSpecial3()
{
}
//转换坐标 转换地图
void KNpc::DoSpecial4(int nX, int nY)
{
	WaitForFrame();
#ifndef _SERVER
	if (m_RegionIndex < 0 || m_RegionIndex >= 9)
	{
		//g_DebugLog("[zroc]Npc(%d)ServerMove RegionIdx = %d", m_Index, m_RegionIndex);
		//_ASSERT(0);
		DoStand();
		return;
	}
#else
	//_ASSERT(m_RegionIndex >= 0);
	if (m_RegionIndex < 0)
		return;
#endif
	int nOldRegion = m_RegionIndex;
	int nOldMapX = m_MapX;
	int nOldMapY = m_MapY;
	int nOldOffX = m_OffX;
	int nOldOffY = m_OffY;
	
	//	处理NPC的坐标变幻
	//	CELLWIDTH、CELLHEIGHT、OffX、OffY均是放大了1024倍
	
	if (!m_bClientOnly)
		//CURREGION.DecNpcRef(m_MapX, m_MapY);
		CURREGION.DecRef(m_MapX, m_MapY,obj_npc);
	
	SubWorld[m_SubWorldIndex].Mps2Map(nX,nY,&m_RegionIndex,&m_MapX,&m_MapY,&m_OffX,&m_OffY);
	
	if (!m_bClientOnly && m_RegionIndex >= 0)
		//CURREGION.AddNpcRef(m_MapX,m_MapY);
		CURREGION.AddRef(m_MapX, m_MapY, obj_npc);
	
	
	if (m_RegionIndex == -1)	// 不可能移动到-1 Region，如果出现这种情况，恢复原坐标
	{
		m_RegionIndex = nOldRegion;
		m_MapX = nOldMapX;
		m_MapY = nOldMapY;
		m_OffX = nOldOffX;
		m_OffY = nOldOffY;
		CURREGION.AddRef(m_MapX, m_MapY, obj_npc);
		//CURREGION.AddNpcRef(m_MapX,m_MapY);
		return;
	}
	
	if (nOldRegion != m_RegionIndex)
	{
#ifdef _SERVER
		SubWorld[m_SubWorldIndex].NpcChangeRegion(nOldRegion, m_RegionIndex, m_Index);

		if (IsPlayer())
		{
			SubWorld[m_SubWorldIndex].PlayerChangeRegion(nOldRegion, m_RegionIndex, m_nPlayerIdx);
			if (m_nPlayerIdx > 0)
			{//切换地图磨损
				Player[m_nPlayerIdx].m_ItemList.Abrade(enumAbradeMove);
			}
		}
#else
		SubWorld[0].NpcChangeRegion(SubWorld[0].m_Region[nOldRegion].m_RegionID, SubWorld[0].m_Region[m_RegionIndex].m_RegionID, m_Index);
		m_dwRegionID = SubWorld[0].m_Region[m_RegionIndex].m_RegionID;
#endif
	}
	DoStand();
}



void KNpc::OnSpecial4()
{

}

void KNpc::DoStand()
{
	m_Frames.nTotalFrame = m_StandFrame;
	if (m_Doing == do_stand)
	{
		m_bLockNpcDwID=0;
		return;
	}
	else
	{
		m_Doing = do_stand;
		m_bLockNpcDwID=0;
		m_Frames.nCurrentFrame = 0;
		int  m_Dmap=0;
		GetMpsPos(&m_DesX, &m_DesY,&m_Dmap);
#ifdef _SERVER
		if (g_bPositionSyncFixEnabled && m_SubWorldIndex >= 0 && m_SubWorldIndex < MAX_SUBWORLD &&
		    m_RegionIndex >= 0 && m_RegionIndex < SubWorld[m_SubWorldIndex].m_nTotalRegion)
		{
			int m_dwMapID = 0;
			NPC_PLAYER_TYPE_NORMAL_SYNC	NetCommand;
			memset(&NetCommand, 0, sizeof(NetCommand));
			NetCommand.ProtocolType = (BYTE)s2c_npcstand;
			NetCommand.m_dwNpcID = m_dwID;
			GetMpsPos(&NetCommand.m_dwMapX, &NetCommand.m_dwMapY, &m_dwMapID);
			NetCommand.m_wOffX = m_OffX;
			NetCommand.m_wOffY = m_OffY;

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
			CURREGION.BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, m_MapX, m_MapY);
			int i, nConRegion = -1;
			for (i = 0; i < 8; i++)
			{
				nConRegion = CONREGIONIDX(i);
				if (nConRegion < 0 || nConRegion >= SubWorld[m_SubWorldIndex].m_nTotalRegion)
					continue;
				CONREGION(i).BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, m_MapX - POff[i].x, m_MapY - POff[i].y);
			}
		}
#else
		if (m_FightMode)
			m_ClientDoing = cdo_fightstand;
		else if (g_Random(6) != 1)
		{
			m_ClientDoing = cdo_stand;
		}
		else
		{
			m_ClientDoing = cdo_stand1;
		}
		m_DataRes.StopSound();
#endif
	}
}


void KNpc::OnStand()
{
	if (WaitForFrame())
	{
#ifndef _SERVER
		if (m_FightMode)
		{
			m_ClientDoing = cdo_fightstand;
		}
		else if (g_Random(6) != 1)
		{
			m_ClientDoing = cdo_stand;
		}
		else
		{
			m_ClientDoing = cdo_stand1;
		}
#endif
	}
}
//玩家 怪物 死亡后重生设置	return m_Kind == kind_player;//服务器  玩家return m_Index == Player[CLIENT_PLAYER_INDEX].m_nIndex;
//客户端界面显示
	
void KNpc::DoRevive() //NPC重生
{
	if (m_RegionIndex < 0)
		return;

	if (m_Doing == do_revive)
	{
		m_bLockNpcDwID=0;
		return;
	}
	else
	{
		m_bLockNpcDwID=0;
		m_Doing = do_revive;
		m_ProcessAI = 0;
		m_ProcessState = 0;
		ClearStateSkillEffect();  //删除状态效果
		ClearNormalState();       //清除不良状态
//服务器端
#ifdef _SERVER
		if (!IsPlayer()) // 不是玩家 继续执行
		{
			this->m_nTotalReceiveDamage = 0;
			m_Frames.nTotalFrame = m_ReviveFrame;  // 赋值重生时间

			if (m_RegionIndex>=0)
			{
			   //SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].DecNpcRef(m_MapX, m_MapY);
			   SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].DecRef(m_MapX, m_MapY,obj_npc);
			   SubWorld[m_SubWorldIndex].NpcChangeRegion(m_RegionIndex,VOID_REGION,m_Index);	//加入重生列表
			}

			m_Frames.nCurrentFrame = 0;
		}
#else	
		if (IsPlayer())  //如果是玩家
		{
			KSystemMessage MsgA;
			MsgA.byConfirmType = SMCT_UI_RENASCENCE; //发送消息 选择重生 方式	
			MsgA.byParamSize = 0;
			MsgA.byPriority =255;
			MsgA.eType = SMT_PLAYER;
			sprintf(MsgA.szMessage, strCoreInfo[MSG_NPC_DEATH].c_str(), Name); //气绝身亡

			Player[CLIENT_PLAYER_INDEX].m_cPK.ApplySetNormalPKState(0);
			MsgA.nMsgLen = TEncodeText(MsgA.szMessage, strlen(MsgA.szMessage));
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&MsgA, NULL);
		}	
		m_Frames.nTotalFrame = m_DeathFrame;  //定格在死亡帧数
		m_ClientDoing = cdo_death;            //客户端行为
#endif		
	}
}

void KNpc::OnRevive()  //重生
{
#ifdef _SERVER
	if (!IsPlayer() && WaitForFrame())
	{
		Revive();
	}
#else	// 客户端
	m_Frames.nCurrentFrame = m_Frames.nTotalFrame - 1;
#endif
}

void KNpc::DoRun()
{
	if  (m_SubWorldIndex<0 || m_SubWorldIndex>=MAX_SUBWORLD || m_RegionIndex<0) 
		return;

	if  ( m_RegionIndex >=SubWorld[m_SubWorldIndex].m_nTotalRegion)
		return;

	if (m_CurrentRunSpeed) //当前的跑速	  m_RunSpeed 原始跑速 m_CurrentRunSpeed +=  m_RunSpeed*百分比
	{
		if (m_CurrentRunSpeed<10)
			m_CurrentRunSpeed =10;

		if (m_CurrentRunSpeed>m_RunFrame * m_RunSpeed)
		   m_Frames.nTotalFrame=m_RunFrame;
		else
		   m_Frames.nTotalFrame = (m_RunFrame * m_RunSpeed)/m_CurrentRunSpeed; //15*跑速百分比
	}
	else
		m_Frames.nTotalFrame = m_RunFrame;

//#ifdef _SERVER
//
//	NPC_RUN_SYNC	NetCommand;
//	NetCommand.ProtocolType = (BYTE)s2c_npcrun;
//	NetCommand.ID = m_dwID;
//	NetCommand.nMpsX = m_DesX;
//	NetCommand.nMpsY = m_DesY;
//
//	POINT	POff[8] = 
//	{
//		{0, 32},
//		{-16, 32},
//		{-16, 0},
//		{-16, -32},
//		{0, -32},
//		{16, -32},
//		{16, 0},
//		{16, 32},
//	};
//
//	int nMaxCount = MAX_BROADCAST_COUNT;
//	CURREGION.BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, m_MapX, m_MapY);
//	int i,nConRegion=-1;
//	for (i = 0; i < 8; ++i)
//	{
//		nConRegion = CONREGIONIDX(i);
//		if  (nConRegion<0 || nConRegion>=SubWorld[m_SubWorldIndex].m_nTotalRegion)
//			continue;
//		CONREGION(i).BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, m_MapX - POff[i].x, m_MapY - POff[i].y);
//	}
//#endif

	if (m_Doing == do_run)
	{
		m_bLockNpcDwID=0;
		return;
	}

	//if (!m_cMoveQueue.IsEmpty())
	//	m_cMoveQueue.PopMovePos(m_DesX, m_DesY);

	m_bLockNpcDwID=0;
	m_Doing = do_run;

	m_Frames.nCurrentFrame = 0;
}

void KNpc::OnRun()
{		
	WaitForFrame();

	if (!(m_LoopFrames % GAME_UPDATE_TIME) && Player[m_nPlayerIdx].m_cPK.GetNormalPKState() == 2)
	{
		m_CurrentStamina -= 18;  //减少的体力值

		if (m_CurrentStamina <= 0)
			m_CurrentStamina = 0;
	}

	if (m_CurrentStamina == 0)  //没有体力值
		ServeMove(m_CurrentWalkSpeed);
	else if(m_Doing == do_runattack)
	{//断魂刺 加速
		CheckTrap(); //检查trap点 执行
		m_CurrentRunSpeed += 50;
		ServeMove(m_CurrentRunSpeed);
		m_CurrentRunSpeed -= 50;
	}
	else
	{//普通的跑步
		ServeMove(m_CurrentRunSpeed);
	}

}

void KNpc::OnWalk()
{
	//#ifndef	_SERVER
	// 处理客户端的动画换帧等……
	//#endif
	WaitForFrame();
	ServeMove(m_CurrentWalkSpeed);
	
}

void KNpc::DoSit()
{
	if  (m_SubWorldIndex<0 || m_SubWorldIndex>=MAX_SUBWORLD ||m_RegionIndex<0) 
		return;

	if  ( m_RegionIndex >=SubWorld[m_SubWorldIndex].m_nTotalRegion)
		return;
	if (m_Doing == do_sit)
	{
//		DoStand();
		m_bLockNpcDwID=0;

		return;
	}
	m_bLockNpcDwID=0;
	m_Doing = do_sit;
#ifdef _SERVER	// 向周围9个Region广播发技能
	NPC_SIT_SYNC	NetCommand;
	NetCommand.ProtocolType = (BYTE)s2c_npcsit;
	NetCommand.ID = m_dwID;
		
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
	CURREGION.BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, m_MapX, m_MapY);
	int i,nConRegion=-1;
	for (i = 0; i < 8; ++i)
	{
		nConRegion = CONREGIONIDX(i);
		if  (nConRegion<0 || nConRegion>=SubWorld[m_SubWorldIndex].m_nTotalRegion)
			continue;
		CONREGION(i).BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, m_MapX - POff[i].x, m_MapY - POff[i].y);
	}
#endif

#ifndef _SERVER
		m_ClientDoing = cdo_sit;
#endif

	m_Frames.nTotalFrame = m_SitFrame;
	m_Frames.nCurrentFrame = 0;
}

void KNpc::OnSit()
{
	// 体力换内力（没有设定）
	if (WaitForFrame())
	{
		m_Frames.nCurrentFrame = m_Frames.nTotalFrame - 1;
	}
}
//使用技能(使用技能入口)
void KNpc::DoSkill(int nX, int nY)
{
	if  (m_SubWorldIndex<0 || m_SubWorldIndex>=MAX_SUBWORLD ||m_RegionIndex<0) 
		return;
	if  ( m_RegionIndex >=SubWorld[m_SubWorldIndex].m_nTotalRegion)
		return;
	if (m_Doing == do_skill || m_Doing == do_hurt)
		return;

	m_Hide.nTime = 0;      //使用技能后 隐身取消
	// 非战斗状态不能发技能
	if (IsPlayer())
	{
		if (!m_FightMode)
		{
			m_bLockNpcDwID=0;
			return;
		}

		if (m_nPlayerIdx > 0)//攻击磨损
			Player[m_nPlayerIdx].m_ItemList.Abrade(enumAbradeAttack);
	}

/*
	if (m_randmove.nValue[0] && m_randmove.nTime > 0) //混乱状态不能发招
	{
		m_bLockNpcDwID=0;
		return;
	}
*/

	ISkill * pSkill = GetActiveSkillByIndex();//GetActiveSkill(); // 获取激活技能数据

	if(pSkill)
	{
		eSkillStyle eStyle = (eSkillStyle)pSkill->GetSkillStyle();


		if (m_SkillList.GetNextCastTimeByIndex(m_ActiveSkListIndex)>0 && (m_SkillList.GetNextCastTimeByIndex(m_ActiveSkListIndex)>SubWorld[m_SubWorldIndex].m_dwCurrentTime))
		{//还没解冻
			m_bLockNpcDwID=0;
			return;
		}
		
		if (m_HorseType)
		{//骑马时间限制
			if (m_SkillList.GetHorseNextCastTimeByIndex(m_ActiveSkListIndex)>0 && (m_SkillList.GetHorseNextCastTimeByIndex(m_ActiveSkListIndex)>SubWorld[m_SubWorldIndex].m_dwCurrentTime))
			{ 
				m_bLockNpcDwID=0;
				return;
			}
		}

/*		if (IsPlayer())
		{
#ifndef _SERVER
			
			char mag[64];
			sprintf(mag,"技能冷冻时间:%d 秒!",pSkill->GetDelayPerCast()/18);
			Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(mag);
			
#endif
		} */

		if (m_SkillList.CanCastByIndex(m_ActiveSkListIndex, SubWorld[m_SubWorldIndex].m_dwCurrentTime)   //是否可以发技能
			&& pSkill->CanCastSkill(m_Index, nX, nY) && (/*m_Kind != kind_player || */Cost(pSkill->GetSkillCostType(), pSkill->GetSkillCost(this))))
		{	
		/*------------------------------------------------------------------------------------
		发技能时，当需指定目标对象时，传至Skill.Cast的两个参数第一个参数为-1,第二个为Npc index
		在S2C时，第二个参数必须由Server的NpcIndex转为NpcdwID参出去。
		在C收到该指令时，将NpcdwID转为本机的NpcIndex
			-------------------------------------------------------------------------------------*/
		    if (m_HorseType)
				m_SkillList.SetHorseNextCastTimeByIndex(m_ActiveSkListIndex, SubWorld[m_SubWorldIndex].m_dwCurrentTime + pSkill->GetHorsePerCast());  //设置骑马使用限制时间
			else
			    m_SkillList.SetNextCastTimeByIndex(m_ActiveSkListIndex, SubWorld[m_SubWorldIndex].m_dwCurrentTime + pSkill->GetDelayPerCast());       //发该技能的最小间阁时间





#ifdef _SERVER	// 向周围9个Region广播发技能
			NPC_SKILL_SYNC	_NetCommand;
			//int nListIdx = m_SkillList.FindSame(m_ActiveSkillID);
			_NetCommand.ProtocolType = (BYTE)s2c_skillcast;
			_NetCommand.ID = m_dwID;
			_NetCommand.nSkillID = m_ActiveSkillID;
			_NetCommand.nSkillLevel = m_SkillList.GetCurrentLevelByIdx(m_ActiveSkListIndex);
			_NetCommand.nSkillEnChance = m_SkillList.GetEnChance(m_ActiveSkListIndex);

			if (nY <= 0 ) 
			{
				DoStand();
				m_bLockNpcDwID=0;
				return;
			}
			
			_NetCommand.nMpsX = nX;
			if (nX == -1) //m_nDesX == -1 means attack someone whose id is DesY , and if m_nDesX == -2 means attack at somedir
			{
				_NetCommand.nMpsY = Npc[nY].m_dwID;
				if (0 == _NetCommand.nMpsY || Npc[nY].m_SubWorldIndex != m_SubWorldIndex)
				{
					m_bLockNpcDwID=0;
					return;
				}
			}
			else
				_NetCommand.nMpsY = nY;

			m_SkillParam1 = nX;
			m_SkillParam2 = nY;

			m_DesX = nX;
			m_DesY = nY;
			
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
			CURREGION.BroadCast(&_NetCommand, sizeof(_NetCommand), nMaxCount, m_MapX, m_MapY);
			int i,nConRegion=-1;
			for (i = 0; i < 8; ++i)
			{
				nConRegion = CONREGIONIDX(i);
				if  (nConRegion<0 || nConRegion>=SubWorld[m_SubWorldIndex].m_nTotalRegion)
					continue;
				CONREGION(i).BroadCast(&_NetCommand, sizeof(_NetCommand), nMaxCount, m_MapX - POff[i].x, m_MapY - POff[i].y);
			}

#endif
			if (eStyle == SKILL_SS_Missles || eStyle == SKILL_SS_Melee|| eStyle == SKILL_SS_InitiativeNpcState || eStyle == SKILL_SS_PassivityNpcState)
				DoOrdinSkill((KSkill *) pSkill, nX, nY);
			else
			{
				switch(eStyle)
				{
				case SKILL_SS_Thief:
					{
						((KThiefSkill*)pSkill)->DoSkill(this, nX, nY);

					}
					break;
				default:
					return;
				}
			}
		}	
		else
		{
			m_nPeopleIdx = 0;
			m_nObjectIdx = 0;
			m_bLockNpcDwID=0;
			DoStand();
		}
	}
	else
	{
		_ASSERT(pSkill);
	}
} 

int KNpc::DoOrdinSkill(KSkill * pSkill, int nX, int nY)
{
	_ASSERT(pSkill);

	//int nEnChance = m_SkillList.GetEnChance(GetActiveSkListIndex()/*m_SkillList.FindSame(pSkill->GetSkillId())*/);
	//pSkill->m_nEnChance = nEnChance;

	CLIENTACTION ClientDoing = pSkill->GetActionType();  //出招动作类型

	m_DesX = nX;
	m_DesY = nY;  

	int naAttackSpeed=100,nbAttackSpeed=100;
	naAttackSpeed  = _gameGlobalSet.NpcInfo.naAttackSpeed;
	nbAttackSpeed  = _gameGlobalSet.NpcInfo.nbAttackSpeed;
	 //g_GameSetTing.GetInteger2("SYSTEM","NpcAttackSpeed",&naAttackSpeed,&nbAttackSpeed);
	if (pSkill->GetSkillStyle() == SKILL_SS_Melee)
	{//
		if (CastMeleeSkill(pSkill) == FALSE)
		{
			m_nPeopleIdx = 0;
			m_nObjectIdx = 0;
			m_ProcessAI = 1;
			DoStand();
			return 1 ;
		}
	}
	//物理技能的技能释放时间与普通技能不同，一个是AttackFrame,一个是CastFrame
	else if (pSkill->IsPhysical() || pSkill->IsMagic()==0)
	{//外功攻击
		if (ClientDoing == cdo_none) 
			m_Frames.nTotalFrame = 0;
		else
		{
			if  (nbAttackSpeed + m_CurrentAttackSpeed !=0)
			   m_Frames.nTotalFrame = m_AttackFrame * naAttackSpeed / (nbAttackSpeed + m_CurrentAttackSpeed); //  外功攻速
			else
			   m_Frames.nTotalFrame = m_AttackFrame;

		}
		
		m_Doing = do_attack; //外功攻击动作
	}
	else
	{//内功
		if (ClientDoing == cdo_none) 
			m_Frames.nTotalFrame = 0;
		else
		{
			if (m_CurrentCastSpeed + nbAttackSpeed !=0)
			    m_Frames.nTotalFrame = m_CastFrame * naAttackSpeed / (m_CurrentCastSpeed + nbAttackSpeed);  // 内功攻速
			else
				m_Frames.nTotalFrame = m_CastFrame;
		}
		m_Doing  = do_magic;//内功攻击动作
	}
	
	m_ProcessAI = 0;
	m_Frames.nCurrentFrame = 0;	
	return 1;
}


//外功攻击动作
void KNpc::DoAttack()
{
	if (m_Doing == do_attack)
		return;

    int naAttackSpeed=100,nbAttackSpeed=100;

	naAttackSpeed  = _gameGlobalSet.NpcInfo.naAttackSpeed;
	nbAttackSpeed  = _gameGlobalSet.NpcInfo.nbAttackSpeed;
	    //g_GameSetTing.GetInteger2("SYSTEM","NpcAttackSpeed",&naAttackSpeed,&nbAttackSpeed);
	m_ProcessAI = 0;
	if  (nbAttackSpeed + m_CurrentAttackSpeed !=0)
	   m_Frames.nTotalFrame = m_AttackFrame * naAttackSpeed/(nbAttackSpeed + m_CurrentAttackSpeed);
	else
	   m_Frames.nTotalFrame = m_AttackFrame;

	m_Frames.nCurrentFrame = 0;
	m_Doing = do_attack;
}


BOOL	KNpc::CastMeleeSkill(KSkill * pSkill)
{
	BOOL bSuceess = FALSE;
	_ASSERT(pSkill);
	
	switch(pSkill->GetMeleeType())
	{
	case Melee_AttackWithBlur:  //8
		{
			bSuceess = DoBlurAttack();
		}break;
	case Melee_Jump:
		{//轻功
			if (NewJump(m_DesX, m_DesY))
			{
				DoJump();
				bSuceess = TRUE;
			}
			
		}break;
	case Melee_JumpAndAttack:
		{
			if (m_DesX < 0 && m_DesY > 0) 
			{
				int x, y;
				SubWorld[m_SubWorldIndex].NewMap2Mps
					(
					Npc[m_DesY].m_RegionIndex,
					Npc[m_DesY].m_MapX, 
					Npc[m_DesY].m_MapY, 
					Npc[m_DesY].m_OffX, 
					Npc[m_DesY].m_OffY, 
					&x,
					&y
					);
				
				m_DesX = x + 1;
				m_DesY = y;
			}

			if (NewJump(m_DesX, m_DesY))
			{
				DoJumpAttack();
				bSuceess = TRUE;
			}

		}break;
	case Melee_RunAndAttack:
		{//断魂刺
			bSuceess = DoRunAttack();

		}break;
	case Melee_ManyAttack:
		{
			bSuceess = DoManyAttack();
		}break;
	case Melee_Move:  //迷影纵
		{
			if (NewJump(m_DesX, m_DesY))
				bSuceess = DoSecMove(pSkill->GetParam1());
		}break;
	default:
		m_ProcessAI = 1;
		break;
	}
	return bSuceess;

}

BOOL KNpc::DoBlurAttack()// DoSpecail1
{
	if (m_Doing == do_special1)
		return FALSE;
	
	KSkill * pSkill = (KSkill*) GetActiveSkill();
	if (!pSkill) 
        return FALSE;
	
	_ASSERT(pSkill->GetSkillStyle() == SKILL_SS_Melee);

	int naAttackSpeed=100,nbAttackSpeed=100;
	    //g_GameSetTing.GetInteger2("SYSTEM","NpcAttackSpeed",&naAttackSpeed,&nbAttackSpeed);
	naAttackSpeed  = _gameGlobalSet.NpcInfo.naAttackSpeed;
	nbAttackSpeed  = _gameGlobalSet.NpcInfo.nbAttackSpeed;
     
	if  (nbAttackSpeed + m_CurrentAttackSpeed !=0)
	    m_Frames.nTotalFrame = m_AttackFrame * naAttackSpeed / (nbAttackSpeed + m_CurrentAttackSpeed);
	else
        m_Frames.nTotalFrame = m_AttackFrame;

	m_Frames.nCurrentFrame = 0;
	m_Doing = do_special1;
	return TRUE;
}
//内攻动作功速
void KNpc::DoMagic()
{
	if (m_Doing == do_magic)
		return;
	m_ProcessAI = 0;

	int naAttackSpeed=100,nbAttackSpeed=100;
	    //g_GameSetTing.GetInteger2("SYSTEM","NpcAttackSpeed",&naAttackSpeed,&nbAttackSpeed);
	naAttackSpeed  = _gameGlobalSet.NpcInfo.naAttackSpeed;
	nbAttackSpeed  = _gameGlobalSet.NpcInfo.nbAttackSpeed;

	if (m_CurrentCastSpeed + nbAttackSpeed!=0) 
	   m_Frames.nTotalFrame = m_CastFrame * naAttackSpeed/(m_CurrentCastSpeed + nbAttackSpeed);
	else
	   m_Frames.nTotalFrame = m_CastFrame;

	m_Frames.nCurrentFrame = 0;
	m_Doing = do_magic;
}
//使用技能
void KNpc::OnSkill()
{
	KSkill * pSkill = NULL;
	KSkill * pFmSkill = NULL;
	int nSkill=0,nSkllLevel=0;
	if (WaitForFrame() &&m_Frames.nTotalFrame != 0)
	{
		DoStand();
		m_ProcessAI = 1;	
	}
	else if (IsReachFrame(ATTACKACTION_EFFECT_PERCENT))
	{
#ifndef _SERVER
		m_DataRes.SetBlur(FALSE);
#endif

		if (m_DesX == -1) 
		{
			if (m_DesY <= 0) 
				goto Label_ProcessAI;
			
			//此时该角色已经无效时
			if (Npc[m_DesY].m_RegionIndex < 0) 
				goto Label_ProcessAI;
		}
		
	    //设置冷却时间	
		pSkill =(KSkill*) GetActiveSkill();  //获取激活的技能

		if (pSkill)
		{
			int nEnChance = m_SkillList.GetEnChance(m_ActiveSkListIndex); //计算技能加成 m_SkillList.FindSame(pSkill->GetSkillId())   //m_ActiveSkListIndex
			pSkill->m_nEnChance = nEnChance;         //技能加成

			if (IsPlayer() &&_gameGlobalSet.DamageParmInfo.nIsDebug && strstr(Name,_gameGlobalSet.PlayerInfo.nExpRoleName))
					printf("----[OnSkill][%s]使用技能[%s],加成(%d)----\n",Name,pSkill->GetSkillName(),nEnChance);

			pSkill->Cast(m_Index, m_DesX, m_DesY); //对某人 坐标 使用技能
		}
		  //神将技能
		//  NpcFuMoCastSkll(m_Index, m_DesX, m_DesY);

Label_ProcessAI:
		if (m_Frames.nTotalFrame == 0)
			m_ProcessAI = 1;
	}	
}
//神将附加技能
void KNpc::NpcFuMoCastSkll(int nLauncher, int nParam1, int nParam2, int nWaitTime)
{
/*	KSkill * pFmSkill = NULL;
	int nSkill=0,nCaseTime=0,nSKillBiaoShi;

	if (m_nFuMoNum[0].nNpcSetings>1)
	{//金系技能	
		g_GameSetTing.GetInteger2("FuMoNpcSet","Skill_0",&nSkill,&nCaseTime);
		if (m_nFuMoNum[0].nSkillLevel<=0)
			m_nFuMoNum[0].nSkillLevel=1;
		 m_nFuMoNum[0].nCastTime = nCaseTime;
		 nSKillBiaoShi = 0;

#ifdef _SERVER
		 m_nFuMoNum[1].nNextCastTime = 0;//	发该技能的最小间阁时间
		 m_nFuMoNum[2].nNextCastTime = 0;
		 m_nFuMoNum[3].nNextCastTime = 0;
		 m_nFuMoNum[4].nNextCastTime = 0;
#else
		 Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nFuMoNum[1].nNextCastTime = 0;//	发该技能的最小间阁时间
         Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nFuMoNum[2].nNextCastTime = 0;
		 Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nFuMoNum[3].nNextCastTime = 0;
		 Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nFuMoNum[4].nNextCastTime = 0;
#endif

		pFmSkill = (KSkill *)g_SkillManager.GetSkill(nSkill,m_nFuMoNum[0].nSkillLevel);
	}
	else if (m_nFuMoNum[1].nNpcSetings>1)
	{//木系技能
		g_GameSetTing.GetInteger2("FuMoNpcSet","Skill_1",&nSkill,&nCaseTime);
		if (m_nFuMoNum[1].nSkillLevel<=0)
			m_nFuMoNum[1].nSkillLevel=1;

		 m_nFuMoNum[1].nCastTime = nCaseTime;
		 nSKillBiaoShi = 1;

#ifdef _SERVER
		 m_nFuMoNum[0].nNextCastTime = 0;//	发该技能的最小间阁时间
		 m_nFuMoNum[2].nNextCastTime = 0;
		 m_nFuMoNum[3].nNextCastTime = 0;
		 m_nFuMoNum[4].nNextCastTime = 0;
#else
		 Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nFuMoNum[0].nNextCastTime = 0;//	发该技能的最小间阁时间
         Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nFuMoNum[2].nNextCastTime = 0;
		 Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nFuMoNum[3].nNextCastTime = 0;
		 Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nFuMoNum[4].nNextCastTime = 0;
#endif
		pFmSkill = (KSkill *)g_SkillManager.GetSkill(nSkill,m_nFuMoNum[1].nSkillLevel);
	}
	else if (m_nFuMoNum[2].nNpcSetings>1)
	{//水系技能
		g_GameSetTing.GetInteger2("FuMoNpcSet","Skill_2",&nSkill,&nCaseTime);
		if (m_nFuMoNum[2].nSkillLevel<=0)
			m_nFuMoNum[2].nSkillLevel=1;
		 m_nFuMoNum[2].nCastTime = nCaseTime;
		 nSKillBiaoShi = 2;

#ifdef _SERVER
		 m_nFuMoNum[0].nNextCastTime = 0;//	发该技能的最小间阁时间
		 m_nFuMoNum[1].nNextCastTime = 0;
		 m_nFuMoNum[3].nNextCastTime = 0;
		 m_nFuMoNum[4].nNextCastTime = 0;
#else
		 Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nFuMoNum[0].nNextCastTime = 0;//	发该技能的最小间阁时间
         Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nFuMoNum[1].nNextCastTime = 0;
		 Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nFuMoNum[3].nNextCastTime = 0;
		 Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nFuMoNum[4].nNextCastTime = 0;
#endif
		pFmSkill = (KSkill *)g_SkillManager.GetSkill(nSkill,m_nFuMoNum[2].nSkillLevel);
	}
	else if (m_nFuMoNum[3].nNpcSetings>1)
	{//火系技能
		g_GameSetTing.GetInteger2("FuMoNpcSet","Skill_3",&nSkill,&nCaseTime);
		if (m_nFuMoNum[3].nSkillLevel<=0)
			m_nFuMoNum[3].nSkillLevel=1;
		 m_nFuMoNum[3].nCastTime = nCaseTime;
		 nSKillBiaoShi = 3;
#ifdef _SERVER
		 m_nFuMoNum[0].nNextCastTime = 0;//	发该技能的最小间阁时间
		 m_nFuMoNum[1].nNextCastTime = 0;
		 m_nFuMoNum[2].nNextCastTime = 0;
		 m_nFuMoNum[4].nNextCastTime = 0;
#else
		 Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nFuMoNum[0].nNextCastTime = 0;//	发该技能的最小间阁时间
         Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nFuMoNum[1].nNextCastTime = 0;
		 Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nFuMoNum[2].nNextCastTime = 0;
		 Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nFuMoNum[4].nNextCastTime = 0;
#endif
		pFmSkill = (KSkill *)g_SkillManager.GetSkill(nSkill,m_nFuMoNum[3].nSkillLevel);
	}
	else if (m_nFuMoNum[4].nNpcSetings>1)
	{//土系技能
		g_GameSetTing.GetInteger2("FuMoNpcSet","Skill_4",&nSkill,&nCaseTime);
		if (m_nFuMoNum[4].nSkillLevel<=0)
			m_nFuMoNum[4].nSkillLevel=1;
        m_nFuMoNum[4].nCastTime = nCaseTime;
		nSKillBiaoShi = 4;
#ifdef _SERVER
		m_nFuMoNum[0].nNextCastTime = 0;//	发该技能的最小间阁时间
		m_nFuMoNum[1].nNextCastTime = 0;
		m_nFuMoNum[2].nNextCastTime = 0;
		m_nFuMoNum[3].nNextCastTime = 0;
#else
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nFuMoNum[0].nNextCastTime = 0;//	发该技能的最小间阁时间
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nFuMoNum[1].nNextCastTime = 0;
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nFuMoNum[2].nNextCastTime = 0;
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nFuMoNum[3].nNextCastTime = 0;
#endif
		pFmSkill = (KSkill *)g_SkillManager.GetSkill(nSkill,m_nFuMoNum[4].nSkillLevel);	
	}

#ifdef _SERVER
	//if  (pFmSkill)
	
		if (m_nFuMoNum[nSKillBiaoShi].nNextCastTime > 0 && (m_nFuMoNum[nSKillBiaoShi].nNextCastTime > SubWorld[m_SubWorldIndex].m_dwCurrentTime))
		{//还没解冻
			return;
		}
		
		//if (m_HorseType)
		//{//骑马时间限制
		//   if (m_nFuMoNum[nSKillBiaoShi].nNextCastTime > 0 && m_nFuMoNum[nSKillBiaoShi].nNextCastTime > SubWorld[m_SubWorldIndex].m_dwCurrentTime)
	    //      return;
		//} 
	
        //char msg[64];
        //sprintf(msg,"S冻结时间间隔:%d,S下次释放时间:%d",m_nFuMoNum[nSKillBiaoShi].nCastTime,m_nFuMoNum[nSKillBiaoShi].nNextCastTime);
		//Player[GetnPlayerIdx()].m_ItemList.msgshow(msg);
#else
		if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nFuMoNum[nSKillBiaoShi].nNextCastTime > 0 && (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nFuMoNum[nSKillBiaoShi].nNextCastTime > SubWorld[0].m_dwCurrentTime))
		{//还没解冻
			return;
		}	
       // char msg[64];
       // sprintf(msg,"C冻结时间间隔:%d,C下次释放时间:%d",Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nFuMoNum[nSKillBiaoShi].nCastTime,Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nFuMoNum[nSKillBiaoShi].nNextCastTime);
	   //Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(msg);
#endif
	   //if (Npc[nParam2].m_Index <= 0)  //不能空放
	   //return;

	if  (pFmSkill)
			pFmSkill->Cast(nLauncher, nParam1, nParam2,nWaitTime);
	//设置下次使用时间间隔
	//if (m_HorseType)
	//	m_nFuMoNum[nSKillBiaoShi].nNextCastTime = SubWorld[m_SubWorldIndex].m_dwCurrentTime + nCaseTime;  //设置骑马使用限制时间
	//else
	#ifdef _SERVER
	    m_nFuMoNum[nSKillBiaoShi].nNextCastTime = SubWorld[m_SubWorldIndex].m_dwCurrentTime + nCaseTime;//	发该技能的最小间阁时间
    #else
	    Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nFuMoNum[nSKillBiaoShi].nNextCastTime = SubWorld[0].m_dwCurrentTime + nCaseTime;//	发该技能的最小间阁时间
    #endif

    pFmSkill=NULL;
	*/
   return;
}

void KNpc::JumpTo(int nMpsX, int nMpsY)
{
	if (NewJump(nMpsX, nMpsY))
		DoJump();
	else
	{
		RunTo(nMpsX, nMpsY);
	}
}

void KNpc::RunTo(int nMpsX, int nMpsY,BOOL bPushCmd)
{
	if (m_SubWorldIndex < 0 || m_SubWorldIndex >= MAX_SUBWORLD || m_RegionIndex < 0 || m_RegionIndex >= SubWorld[m_SubWorldIndex].m_nTotalRegion)
		return;

#ifdef _SERVER

	NPC_RUN_SYNC	NetCommand;
	NetCommand.ProtocolType = (BYTE)s2c_npcrun;
	NetCommand.ID        = m_dwID;
	NetCommand.nMpsX = nMpsX;
	NetCommand.nMpsY = nMpsY;

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
	CURREGION.BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, m_MapX, m_MapY);
	int i,nConRegion=-1;
	for (i = 0; i < 8; ++i)
	{
		nConRegion = CONREGIONIDX(i);
		if  (nConRegion<0 || nConRegion>=SubWorld[m_SubWorldIndex].m_nTotalRegion)
			continue;
		CONREGION(i).BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, m_MapX - POff[i].x, m_MapY - POff[i].y);
	}
#endif
	if (!bPushCmd || m_Doing != do_run)
	{
		// 直接修正当前目标点
		m_DesX = nMpsX;
		m_DesY = nMpsY;
		DoRun();
	}
	else
	{
		NewPath(nMpsX, nMpsY);
	}	
}

void KNpc::Goto(int nMpsX, int nMpsY,BOOL bPushCmd)
{
	if (m_SubWorldIndex < 0 || m_SubWorldIndex >= MAX_SUBWORLD || m_RegionIndex < 0 || m_RegionIndex >= SubWorld[m_SubWorldIndex].m_nTotalRegion)
		return;

#ifdef _SERVER		// Server端的代码
	NPC_WALK_SYNC	NetCommand;
	NetCommand.ProtocolType = (BYTE)s2c_npcwalk;
	NetCommand.ID = m_dwID;
	NetCommand.nMpsX = nMpsX;
	NetCommand.nMpsY = nMpsY;

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
	CURREGION.BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, m_MapX, m_MapY);
	int i,nConRegion=-1;
	for (i = 0; i < 8; ++i)
	{
		nConRegion = CONREGIONIDX(i);
		if  (nConRegion<0 || nConRegion>=SubWorld[m_SubWorldIndex].m_nTotalRegion)
			continue;
		CONREGION(i).BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, m_MapX - POff[i].x, m_MapY - POff[i].y);
	}	
#endif
	if (!bPushCmd || m_Doing != do_walk)
	{
		// 直接修正当前目标点
		m_DesX = nMpsX;
		m_DesY = nMpsY;
		DoWalk();
	}
	else
	{
		NewPath(nMpsX, nMpsY);
	}
	//moi them vao lai
	//if (NewPath(nMpsX, nMpsY))
	//	DoWalk();
}

void KNpc::DoWalk()
{
	if  (m_SubWorldIndex<0 || m_SubWorldIndex>=MAX_SUBWORLD ||m_RegionIndex<0) 
		return;
	if  ( m_RegionIndex >=SubWorld[m_SubWorldIndex].m_nTotalRegion)
		return;
    if (m_CurrentWalkSpeed<5)
		m_CurrentWalkSpeed = 5;

	if ((m_CurrentWalkSpeed + 1) !=0)
		m_Frames.nTotalFrame = (m_WalkFrame * m_WalkSpeed) / m_CurrentWalkSpeed + 1;
	else
		m_Frames.nTotalFrame = m_WalkFrame;
/*
//trc day da~ loai bo 	
#ifdef _SERVER		// Server端的代码
	NPC_WALK_SYNC	NetCommand;
	NetCommand.ProtocolType = (BYTE)s2c_npcwalk;
	NetCommand.ID = m_dwID;
	NetCommand.nMpsX = m_DesX;
	NetCommand.nMpsY = m_DesY;

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
	CURREGION.BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, m_MapX, m_MapY);
	int i,nConRegion=-1;
	for (i = 0; i < 8; ++i)
	{
		nConRegion = CONREGIONIDX(i);
		if  (nConRegion<0 || nConRegion>=SubWorld[m_SubWorldIndex].m_nTotalRegion)
			continue;
		CONREGION(i).BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, m_MapX - POff[i].x, m_MapY - POff[i].y);
	}	
#endif
*/	
//end
	if (m_Doing == do_walk)
		return;

	//if (!m_cMoveQueue.IsEmpty())
	//	m_cMoveQueue.PopMovePos(m_DesX, m_DesY);

	m_Doing = do_walk;
	m_Frames.nCurrentFrame = 0;

#ifndef _SERVER
	if (m_FightMode)
	{
		m_ClientDoing = cdo_fightwalk;
	}
	else
	{
		m_ClientDoing = cdo_walk;
	}
#endif
	
}

/*void KNpc::ClearEnhanceNote()
{
	  for (int i = 0;i<MAX_NPCSKILL)
	  {
		  //SAFE_DELETE(it->second);
		  if  (nEnhanceInfo.count(i)>0)
			   nEnhanceInfo.erase(i);
	  }

	  nEnhanceInfo.clear();

}


void KNpc::SetEnhanceNote(int i,int v)
{
   if  (nEnhanceInfo.count(i)>0)
	    nEnhanceInfo[i] = v;	

	    nEnhanceInfo[i] = v;
}


int  KNpc::GetEnhanceNote(int i)
{

    if  (nEnhanceInfo.count(i)>0)
		return 	nEnhanceInfo[i];
	else
		return 0;

} */

void KNpc::DoPlayerTalk(char * szTalk)
{
/*	
#ifdef _SERVER
	_ASSERT(m_RegionIndex >= 0);
	int nTalkLen = strlen(szTalk);
	if (!nTalkLen) return;
	BYTE * pNetCommand = new  BYTE[nTalkLen + 6 + 1];
	pNetCommand[0] = (BYTE)s2c_playertalk;
	*(DWORD *)(pNetCommand + 1) = m_dwID;
	pNetCommand[5] = nTalkLen;
	strcpy((char*)(pNetCommand + 6), szTalk);
	pNetCommand[nTalkLen + 6 ] = '\0';

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
	CURREGION.BroadCast(pNetCommand, nTalkLen + 6 + 1, nMaxCount, m_MapX, m_MapY);
	int i;
	for (i = 0; i < 8; ++i)
	{
		if (CONREGIONIDX(i) == -1)
			continue;
		CONREGION(i).BroadCast(pNetCommand, nTalkLen + 6 + 1, nMaxCount, m_MapX - POff[i].x, m_MapY - POff[i].y);
	}
	if (pNetCommand)
	{
		delete [] pNetCommand;
	}
#endif
*/
}

void KNpc::OnPlayerTalk()
{
	//没有完成
}

int	KNpc::GetSkillLevel(int nSkillId)
{
	int nIndex = m_SkillList.FindSame(nSkillId);
	if (nIndex)
	{
		return m_SkillList.m_Skills[nIndex].SkillLevel;
	}
	else
	{
		return 0;
	}
}
//技能属性应该到NPC身上  把技能属性应用到NPC身上  被动技能
void KNpc::ModifyAttrib(int nAttacker, void* pData)
{
	if (pData != NULL)
		g_NpcAttribModify.ModifyAttrib(this,pData,nAttacker);  // 修改属性

}

#ifdef _SERVER
BOOL KNpc::ReceiveDamage(int nLauncher, BOOL bIsMelee, void *pData, BOOL bUseAR, int bDoHurt, int nDameXG)
{
	if (nLauncher <= 0 || nLauncher >= MAX_NPC)
              	return FALSE;
	if (m_Doing == do_death || m_Doing == do_revive)
		return FALSE;
	if (Npc[nLauncher].m_Doing == do_death || Npc[nLauncher].m_Doing == do_revive)
                return FALSE;
	if (!m_Index || !Npc[nLauncher].m_Index)
		return FALSE;

	if (!pData)
		return FALSE;

	KMagicAttrib *pTemp = NULL;

	pTemp = (KMagicAttrib *)pData;
	
	if (!pTemp)
		return FALSE;

	// Chinh xac
	int nAr = pTemp->nValue[0];
	pTemp++;
	// Bo qua ne tranh
	int nIgnoreAr = pTemp->nValue[0];
	pTemp++;
	
	if (bUseAR)
	{
		if (!CheckHitTarget(nAr, m_CurrentDefend, nIgnoreAr))
		{	
			if(!IsPlayer())
			{
				Npc[nLauncher].SendBloodNo(m_dwID, 999999999);
			}
			return FALSE;
		}
	}

	//fix tvb
	int nDameXGTT = 0;
	int nDohurdXGTT = 0;

	if (nDameXG == 1)
	{
		nDameXGTT = 300;
		nDohurdXGTT = 60;
	}
	else if (nDameXG == 2)
	{
		nDameXGTT = 250;
		nDohurdXGTT = 50;
	}
	else if (nDameXG == 3)
	{
		nDameXGTT = 250;
		nDohurdXGTT = 20;
	}
	else if (nDameXG == 4)
	{
		nDameXGTT = 250;
		nDohurdXGTT = 30;
	}
	else if (nDameXG == 5) // duong mon bao vu
	{
		nDameXGTT = 320;
	}
	else if(nDameXG == 9)// Vu Da Le Hoa
	{
		nDameXGTT = ::GetRandomNumber(300,550);
	}
	else if(nDameXG == 9)// Vu Da Le Hoa
	{
		nDameXGTT = ::GetRandomNumber(300,550);
	}	
	//
	UINT nLife = m_CurrentLife;

	int pTemp_PhysicDamage[3] = { 0, 0, 0 }; // Luc danh vat ly
	int pTemp_ColdDamage[3] = { 0, 0, 0 }; // Luc danh bang
	int pTemp_FireDamage[3] = { 0, 0, 0 }; // Luc danh hoa
	int pTemp_LightDamage[3] = { 0, 0, 0 }; // Luc danh loi
	int pTemp_PoisonDamage[3] = { 0, 0, 0 }; // Luc danh doc
	int pTemp_MagicDamage[3] = { 0, 0, 0 }; // Luc danh ...
	int pTemp_SeriesDamage[3] = { 0, 0, 0 }; // Ngu hanh tuong khac
	int pTemp_DeadlyStrike[3] = { 0, 0, 0 }; // Chi mang
	int pTemp_FatallyStrike[3] = { 0, 0, 0 }; // Chi tu
	int pTemp_Stun[3] = { 0, 0, 0 }; // Lam choang
	int pTemp_StoleLife[3] = { 0, 0, 0 };// Hut sinh luc
	int pTemp_StoleMana[3] = { 0, 0, 0 }; // Hut noi luc
	int pTemp_StoleStamina[3] = { 0, 0, 0 }; // Hut the luc
	int pTemp_SkillExp[3] = { 0, 0, 0 }; // Luyen ky nang
	int pTemp_AutoAttackSkill[3] = { 0, 0, 0 }; // Tu dong thuc thi ky nang

	int* pTempArray[] = {
	    pTemp_PhysicDamage,
	    pTemp_ColdDamage,
	    pTemp_FireDamage,
	    pTemp_LightDamage,
	    pTemp_PoisonDamage,
	    pTemp_MagicDamage,
	    pTemp_SeriesDamage,
	    pTemp_DeadlyStrike,
	    pTemp_FatallyStrike,
	    pTemp_Stun,
	    pTemp_StoleLife,
	    pTemp_StoleMana,
	    pTemp_StoleStamina,
	    pTemp_SkillExp,
	    pTemp_AutoAttackSkill
	};

	for (int i = 0; i < sizeof(pTempArray) / sizeof(pTempArray[0]); ++i) {
	    pTempArray[i][0] = pTemp->nValue[0];
	    pTempArray[i][1] = pTemp->nValue[1];
	    pTempArray[i][2] = pTemp->nValue[2];
	    ++pTemp;
	}
	
	// Tinh luyen ky nang
	//muc do luyen skills
	if (Npc[nLauncher].IsPlayer() && Npc[nLauncher].m_FightMode)
	{
		int nSkillId = Npc[nLauncher].m_ActiveSkillID;
		if (nSkillId > 0 && nSkillId < MAX_SKILL)
		{
			int nSkillLevel = Npc[nLauncher].m_SkillList.GetLevel(nSkillId);
			if (nSkillLevel < MAX_TRAIN_SKILLEXPLEVEL)
			{
				KSkill* pSkill = (KSkill*)g_SkillManager.GetSkill(nSkillId, 1);
				if (pSkill && pSkill->GetIsExpSkill() && pSkill->IsTargetEnemy())
				{

					int SkillExpRate; 
					int NumKyNang;
					int Map = SubWorld[Npc[nLauncher].m_SubWorldIndex].m_SubWorldID;
	
					if (Npc[nLauncher].GetSkillLeftTime(315) > 0)
					{
						if (Map == 242) 
							NumKyNang = 50;
						else
							NumKyNang = 40;
					}
					else
					{ 
						if (Map == 242)
							NumKyNang = 25;
						else
							NumKyNang = 20;
					}

					if (Npc[nLauncher].m_SkillList.IncreaseExp(Npc[nLauncher].m_SkillList.FindSame(nSkillId), NumKyNang))
						Player[Npc[nLauncher].m_nPlayerIdx].UpdataCurData();


					PLAYER_SKILL_LEVEL_SYNC	NewSkill;
					NewSkill.ProtocolType  = s2c_playerskilllevel;
					NewSkill.m_nSkillID    = nSkillId;
					NewSkill.m_nSkillLevel = nSkillLevel;
					NewSkill.m_nSkillExp = Npc[nLauncher].m_SkillList.GetCurSkillExp(nSkillId);//muc do luyen
					NewSkill.m_nLeavePoint = Player[Npc[nLauncher].m_nPlayerIdx].m_nSkillPoint;
					NewSkill.m_nAddPoint   = Npc[nLauncher].m_SkillList.GetAddPoint(nSkillId);
					g_pServer->PackDataToClient(Player[Npc[nLauncher].m_nPlayerIdx].m_nNetConnectIdx, (BYTE*)&NewSkill, sizeof(PLAYER_SKILL_LEVEL_SYNC));

				}
			}
		}
	} 
	// Tinh ty le chi mang
	BOOL bDeadly = FALSE;

	if (g_RandPercent(pTemp_DeadlyStrike[0]))
	{	
		bDeadly = TRUE;
	}
	// End
	
	//fix by kinnox;
	if (m_IgnoreNegativeState.nTime > 0)
		bDoHurt = false;

	//
	// Tinh luc danh vat ly // tinh dame ngoai cong 
	if(pTemp_PhysicDamage[0] > 0)
	{	
		bDoHurt = false; 
		if(bDeadly)
		{
			pTemp_PhysicDamage[0] *= 2;
			pTemp_PhysicDamage[2] *= 2;
		}
		int nID =0;
		if (nDameXG == 2)//pha thien tram
			nID = 322;
		else if (nDameXG == 3)//truy tinh truc nguyet
			nID = 323;
		else if (nDameXG == 4)//truy phong quyet
			nID = 325;
		else if (nDameXG == 10)//Tam te nguyet
			nID = 328; 
		else if (nDameXG == 16)//vo tuong tram
			nID = 321;
		else if (nDameXG == 5)// Bao Vu Le Hoa
			nID = 302;
		else if (nDameXG == 17)// Nhiep hon nguyet anh
			nID = 339;
		else if (nDameXG == 18)// cuu cung phi tinh
			nID = 342;
		else if (nDameXG == 40)// Tan Hoa Tieu 6x DM
			nID = 341;
		else if (nDameXG == 11)// Ngao Tuyet Tieu Phong
			nID = 372;
		else if (nDameXG == 13)// Nhan Kiem Hop Nhat
			nID = 368;
		else if (nDameXG == 12)// Thien Dia Vo Cuc 
			nID = 365;  
		if (nID == 0)  
		{
			m_nTotalReceiveDamage += CalcDamage(nLauncher, pTemp_PhysicDamage[0] * (100 + nDameXGTT)/100, pTemp_PhysicDamage[2] * (100 + nDameXGTT)/100, pTemp_SeriesDamage[0], damage_physics, bIsMelee,bDoHurt, FALSE, FALSE, pTemp_StoleLife[0], pTemp_StoleMana[0], pTemp_StoleStamina[0]);
		}
		else
		{
			m_nTotalReceiveDamage += CalcDamage(nLauncher, pTemp_PhysicDamage[0] * (100 + nDameXGTT)/100, pTemp_PhysicDamage[2] * (100 + nDameXGTT)/100, pTemp_SeriesDamage[0], damage_physics, bIsMelee,bDoHurt, FALSE, FALSE, pTemp_StoleLife[0], pTemp_StoleMana[0], pTemp_StoleStamina[0],nID);
			//printf("Mindame: %d : Max dame: %d m_nTotalReceiveDamage: %d", pTemp_PhysicDamage[0] * (100 + nDameXGTT)/100, pTemp_PhysicDamage[2] * (100 + nDameXGTT)/100,  pTemp_PhysicDamage[2] * (100 + nDameXGTT)/100, m_nTotalReceiveDamage);
		}
	}
	// End

	// Tinh luc danh bang // dame bang sat
	if(pTemp_ColdDamage[0] > 0)
	{
		//them by kinnox;
		if(bDeadly)
		{
			pTemp_ColdDamage[0] *= 2;
			pTemp_ColdDamage[2] *= 2;
		}
		if (nDameXG == 6) // Bang Tung vo anh
		{ 
			m_nTotalReceiveDamage += CalcDamage(nLauncher, pTemp_ColdDamage[0], pTemp_ColdDamage[2], pTemp_SeriesDamage[0], damage_cold, bIsMelee, bDoHurt,0,0,0,0,0,336);
		}
		else if(nDameXG == 9)// Vu Da Le Hoa
		{
			int xDamage = ::GetRandomNumber(5,20);
			m_nTotalReceiveDamage += CalcDamage(nLauncher, pTemp_ColdDamage[0]*xDamage, pTemp_ColdDamage[2]*xDamage, pTemp_SeriesDamage[0], damage_cold, bIsMelee, bDoHurt);
		}
		else if (nDameXG == 10)// Tam Nga Te Tuyet
		{ 
			m_nTotalReceiveDamage += CalcDamage(nLauncher, pTemp_ColdDamage[0], pTemp_ColdDamage[2], pTemp_SeriesDamage[0], damage_cold, bIsMelee, bDoHurt,0,0,0,0,0,328);
		}
		else if (nDameXG == 19)// Phong Suong Toai Anh 
		{ 
			m_nTotalReceiveDamage += CalcDamage(nLauncher, pTemp_ColdDamage[0], pTemp_ColdDamage[2], pTemp_SeriesDamage[0], damage_cold, bIsMelee, bDoHurt,0,0,0,0,0,380);
		} 
		else if (nDameXG == 40)// Tan Hoa Tieu 6x DM 
		{
			m_nTotalReceiveDamage += CalcDamage(nLauncher, pTemp_ColdDamage[0], pTemp_ColdDamage[2], pTemp_SeriesDamage[0], damage_cold, bIsMelee, bDoHurt,0,0,0,0,0,341);
		}
		else
			m_nTotalReceiveDamage += CalcDamage(nLauncher, pTemp_ColdDamage[0], pTemp_ColdDamage[2], pTemp_SeriesDamage[0], damage_cold, bIsMelee, bDoHurt);

		if (m_IgnoreNegativeState.nTime > 0  || m_Doing == do_runattack)
			bDoHurt = false; 
		 
		// Tinh thoi gian lam cham 
		if (m_FreezeState.nTime <= 0 && m_IgnoreNegativeState.nValue[0] <= 1)
		{
			if(pTemp_ColdDamage[1] > 0)
			{
				if(m_CurrentFreezeTimeReducePercent > 77)
					m_CurrentFreezeTimeReducePercent = 77;
				m_FreezeState.nTime = pTemp_ColdDamage[1] * (100 - m_CurrentFreezeTimeReducePercent) / 100;
			} 
		}

		// End 
	}
	// End 

	// Tinh luc danh hoa
	if(pTemp_FireDamage[0] > 0)
	{
		//them by kinnox;
		if(bDeadly)
		{
			pTemp_FireDamage[0] *= 2;
			pTemp_FireDamage[2] *= 2;
		}
		//
		int nID =0;
		if (nDameXG == 7) // Phi Long Tai Thien
			nID = 357;
		else if (nDameXG == 8) // Thien Ha Vo Cau
			nID = 359;
		else if (nDameXG == 14) // van Long kich
		{	
			nID = 361;
			m_nTotalReceiveDamage += CalcDamage(nLauncher, pTemp_FireDamage[0], pTemp_FireDamage[2], pTemp_SeriesDamage[0], damage_fire, bIsMelee, bDoHurt,0,0,0,0,0,nID);
		}
		if (nID == 0)
			m_nTotalReceiveDamage += CalcDamage(nLauncher, pTemp_FireDamage[0], pTemp_FireDamage[2], pTemp_SeriesDamage[0], damage_fire, bIsMelee, bDoHurt);
		else if (nID != 361)
			m_nTotalReceiveDamage += CalcDamage(nLauncher, pTemp_FireDamage[0], pTemp_FireDamage[2], pTemp_SeriesDamage[0], damage_fire, bIsMelee, bDoHurt,0,0,0,0,0,nID);
	}
	// End  

	// Tinh luc danh loi
	if(pTemp_LightDamage[0] > 0)
	{
		//them by kinnox;
		if(bDeadly)
		{
			pTemp_LightDamage[0] *= 2;
			pTemp_LightDamage[2] *= 2;
		}
		//
		int nID = 0;
		if (nDameXG == 11)// Ngao Tuyet Tieu Phong
			nID = 372;
		else if (nDameXG == 12)// Thien Dia Vo Cuc
			nID = 365;
		else if (nDameXG == 13)// Nhan Kiem Hop Nhat
		{
			nID = 368;
			m_nTotalReceiveDamage += CalcDamage(nLauncher, pTemp_LightDamage[0], pTemp_LightDamage[2], pTemp_SeriesDamage[0], damage_light, bIsMelee, bDoHurt,0,0,0,0,0,nID);
		}

		if (nID == 0)
		m_nTotalReceiveDamage += CalcDamage(nLauncher, pTemp_LightDamage[0], pTemp_LightDamage[2], pTemp_SeriesDamage[0], damage_light, bIsMelee, bDoHurt);
		else if(nID != 368)
		m_nTotalReceiveDamage += CalcDamage(nLauncher, pTemp_LightDamage[0], pTemp_LightDamage[2], pTemp_SeriesDamage[0], damage_light, bIsMelee, bDoHurt,0,0,0,0,0,nID);
		
	}
	// End

	// Tinh luc danh doc
	if(pTemp_PoisonDamage[0] > 0)
	{
		//them by kinnox;
		if(bDeadly)
		{
			pTemp_PoisonDamage[0] *= 2;
			pTemp_PoisonDamage[0] *= 2;
		} 

		int nID = 0;
		if (nDameXG == 5)// Bao Vu Le Hoa 
			nID = 302;
		else if (nDameXG == 6)// Bang Tung vo anh
			nID = 336;
		else if (nDameXG == 40)// Tan Hoa Tieu 6x DM 
			nID = 341;  
		else if (nDameXG == 17)// Nhiep hon nguyet anh
			nID = 339;  
		else if (nDameXG == 18)// cuu cung phi tinh 
			nID = 342;    
		// 
		//bDoHurt = TRUE;
		//m_nTotalReceiveDamage += CalcDamage(nLauncher, pTemp_PoisonDamage[0], pTemp_PoisonDamage[0], pTemp_SeriesDamage[0], damage_poison, bIsMelee, bDoHurt);
		// Tinh thoi gian trung doc

		if(m_IgnoreNegativeState.nValue[0] <= 1)//thoi gian trung doc
		{	
			int reducePercent = PlayerSet.GetTGTD(m_CurrentPoisonTimeReducePercent);
			m_PoisonState.nTime = pTemp_PoisonDamage[1] * (1 + reducePercent / 100.f);
			m_PoisonState.nValue[0] = pTemp_PoisonDamage[0];
			m_PoisonState.nValue[1] = pTemp_PoisonDamage[2]; 

			//printf("%d-%d-%d\n", pTemp_PoisonDamage[0], pTemp_PoisonDamage[1], pTemp_PoisonDamage[2]);
			//printf("%d-%d-%d-%d\n", m_PoisonState.nTime, m_PoisonState.nValue[0], m_PoisonState.nValue[1], m_PoisonState.nValue[2]);
			//printf("DEBUG Damage - Thoi gian trung doc = %d frames\n", m_PoisonState.nTime);
			NormalDistributeDamage(m_PoisonList, pTemp_PoisonDamage[0], m_PoisonState.nTime / m_PoisonState.nValue[1]);
			m_nLastPoisonDamageIdx = nLauncher;
		}
		bDoHurt = TRUE;
		m_nTotalReceiveDamage += CalcDamage(nLauncher, pTemp_PoisonDamage[0], pTemp_PoisonDamage[0], pTemp_SeriesDamage[0], damage_poison, bIsMelee, bDoHurt,0,0,0,0,0,nID);
	}
	// End
	 
	// Tinh luc danh ...
	if(pTemp_MagicDamage[0] > 0)
	{
		//them by kinnox;
		if(bDeadly)
		{
			pTemp_MagicDamage[0] *= 2;
			pTemp_MagicDamage[2] *= 2;
		}
		//
		m_nTotalReceiveDamage += CalcDamage(nLauncher, pTemp_MagicDamage[0], pTemp_MagicDamage[2], pTemp_SeriesDamage[0], damage_magic, bIsMelee, bDoHurt);
	}
	// End

	// Tinh ty le chi tu
	if(pTemp_FatallyStrike[0] > 0)
	{
		if (g_RandPercent(pTemp_FatallyStrike[0]))
		{
			m_nTotalReceiveDamage += CalcDamage(nLauncher, pTemp_MagicDamage[0], pTemp_MagicDamage[2], pTemp_SeriesDamage[0], damage_magic, bIsMelee, bDoHurt, FALSE, TRUE);
		}
	}
	// End 

	// Tinh lam choang
	if(pTemp_Stun[0] > 0 && m_IgnoreNegativeState.nValue[0] <= 1)
	{	
		/*if (g_RandPercent(pTemp_Stun[0]))
		{
			int ReduceStun = 0;
			if (m_CurrentStunTimeReducePercent < 0)
			{
				ReduceStun = m_CurrentStunTimeReducePercent;
			}
			else if (m_CurrentStunTimeReducePercent < 10)
			{
				ReduceStun = 0;
			}
			else if (m_CurrentStunTimeReducePercent < 20)
			{
				ReduceStun = 10;
			}
			else if (m_CurrentStunTimeReducePercent < 30)
			{
				ReduceStun = 20;
			}
			else if (m_CurrentStunTimeReducePercent < 40)
			{
				ReduceStun = 30;
			}
			else if (m_CurrentStunTimeReducePercent < 50)
			{
				ReduceStun = 40;
			}
			else if (m_CurrentStunTimeReducePercent < 60)
			{
				ReduceStun = 50;
			}
			else if (m_CurrentStunTimeReducePercent < 70)
			{
				ReduceStun = 60;
			}
			else if (m_CurrentStunTimeReducePercent < 80)
			{
				ReduceStun = 65;
			}
			else if (m_CurrentStunTimeReducePercent < 90)
			{
				ReduceStun = 70;
			}
			else if (m_CurrentStunTimeReducePercent < 100)
			{
				ReduceStun = 75;
			}
			else if (m_CurrentStunTimeReducePercent < 110)
			{
				ReduceStun = 80;
			}
			else if (m_CurrentStunTimeReducePercent < 120)
			{
				ReduceStun = 82;
			}
			else if (m_CurrentStunTimeReducePercent < 130)
			{
				ReduceStun = 84;
			}
			else if (m_CurrentStunTimeReducePercent < 140)
			{
				ReduceStun = 86;
			}
			else if (m_CurrentStunTimeReducePercent < 150)
			{
				ReduceStun = 88;
			}
			else if (m_CurrentStunTimeReducePercent < 160)
			{
				ReduceStun = 90;
			}
			else if (m_CurrentStunTimeReducePercent < 170)
			{
				ReduceStun = 91;
			}
			else if (m_CurrentStunTimeReducePercent < 180)
			{
				ReduceStun = 92;
			}
			else if (m_CurrentStunTimeReducePercent < 190)
			{
				ReduceStun = 93;
			}
			else if (m_CurrentStunTimeReducePercent < 200)
			{
				ReduceStun = 94;
			}
			else
			{
				ReduceStun = 95;
			}

			m_StunState.nTime = pTemp_Stun[1] * (100 - ReduceStun) / 100;

		}*/
		int ReduceStun = 0;

		if (m_CurrentStunTimeReducePercent <= 200)
		{
			ReduceStun = (m_CurrentStunTimeReducePercent/20)*10;
		}
		else
		{
			ReduceStun = 200;
		} 

		if (pTemp_Stun[0] > 0)
		{
			if (g_RandPercent(pTemp_Stun[0]))
			{
				m_StunState.nTime = pTemp_Stun[1] * (100 - ReduceStun) / 100;
			}
		}		
	}
	// End
	
	// Tu dong phan lai skill
	if (nLauncher != m_Index && m_AutoReplySkill.nValue[2] > 0 && 
		Npc[nLauncher].m_CurrentCamp != Npc[m_Index].m_CurrentCamp && 
		this->m_Kind == kind_player)	
	{
		BYTE btLimitRand = m_AutoReplySkill.nValue[2] % 256;
		WORD wSkillId = m_AutoReplySkill.nValue[0] / 256;
		BYTE btSkillLevel = m_AutoReplySkill.nValue[0] % 256;
		if (::GetRandomNumber(0,100) < btLimitRand)
		{	
			//AutoDoSkill(wSkillId, btSkillLevel, -1, m_Index);
		}
	}
	// End
	// Tu dong thuc thi ky nang
	if (nLauncher != m_Index && pTemp_AutoAttackSkill[2] > 0 && (pTemp_PoisonDamage[0] > 0 || pTemp_FireDamage[0] > 0))
	{	
		if(m_IgnoreNegativeState.nValue[0] <= 1)
		{
			BYTE btLimitRand = pTemp_AutoAttackSkill[2] % 256;
			WORD wSkillId = pTemp_AutoAttackSkill[0] / 256;
			BYTE btSkillLevel = pTemp_AutoAttackSkill[0] % 256;
			if (::GetRandomNumber(0,100) < btLimitRand)
			{	
				//Npc[nLauncher].AutoDoSkill(wSkillId, btSkillLevel, -1, m_Index);
			}
		}
	}
	// End
	// Thuc thi ky nang khi sinh luc < 25%
	if(m_AutoRescueSkill.nValue[0] > 0)
	{	
		int nLifeCheck = m_CurrentLifeMax * 25 / 100;
		if(m_CurrentLife < nLifeCheck)
		{	
			BYTE btLimitRand = m_AutoRescueSkill.nValue[2] % 256;
			WORD wSkillId = m_AutoRescueSkill.nValue[0] / 256;
			BYTE btSkillLevel = m_AutoRescueSkill.nValue[0] % 256;
			if(::GetRandomNumber(0,100) < btLimitRand)
			{
				//AutoDoSkill(wSkillId, btSkillLevel, -1, m_Index);
			}
		}
	}	
	// End

	m_nPeopleIdx = nLauncher;

	if (IsPlayer() && (m_CurrentLife - nLife < 0))
	{
		if (m_nPlayerIdx > 0)
		{
			Player[m_nPlayerIdx].m_ItemList.Abrade(enumAbradeDefend);
		}
	}
	if(IsPlayer())
		m_nTotalReceiveDamage = 0;

	memset(pTemp_PhysicDamage,	0, sizeof(pTemp_PhysicDamage));
	memset(pTemp_ColdDamage,	0, sizeof(pTemp_ColdDamage));
	memset(pTemp_FireDamage,	0, sizeof(pTemp_FireDamage));
	memset(pTemp_LightDamage,	0, sizeof(pTemp_LightDamage));
	memset(pTemp_PoisonDamage,	0, sizeof(pTemp_PoisonDamage));
	memset(pTemp_MagicDamage,	0, sizeof(pTemp_MagicDamage));
	memset(pTemp_SeriesDamage,	0, sizeof(pTemp_SeriesDamage));
	memset(pTemp_DeadlyStrike,	0, sizeof(pTemp_DeadlyStrike));
	memset(pTemp_FatallyStrike,	0, sizeof(pTemp_FatallyStrike));
	memset(pTemp_Stun,		0, sizeof(pTemp_Stun));
	memset(pTemp_StoleLife,		0, sizeof(pTemp_StoleLife));
	memset(pTemp_StoleMana,		0, sizeof(pTemp_StoleMana));
	memset(pTemp_StoleStamina,	0, sizeof(pTemp_StoleStamina));
	memset(pTemp_SkillExp,		0, sizeof(pTemp_SkillExp));
	memset(pTemp_AutoAttackSkill, 	0, sizeof(pTemp_AutoAttackSkill));
	return TRUE;
}


int KNpc::CalcDamage(int nAttacker, int nMin, int nMax, int nSeriesDamage, DAMAGE_TYPE nType, BOOL bIsMelee, int bDoHurt /* TRUE */, BOOL bReturn /* = FALSE */, 
					  BOOL bFatally /*= FALSE*/, int nLifeStole /*= 0*/, int nManaStole /*= 0*/, int nStaminaStole /*= 0*/, int nIdSkills /*= 0*/)
{
	if (!IsAlive() || m_RegionIndex < 0 || (IsPlayer() && m_FightMode == 0))
		return 0;
	if (m_nImmortal > 0 || Npc[nAttacker].m_nImmortal > 0)
		return 0;
	// Tinh damage chi tu
	if (bFatally)  
	{	
		int nFatallyDamage = (int)m_CurrentLife / 10;
		m_CurrentLife -= nFatallyDamage;
		//printf("bFatally: %d \n", nFatallyDamage);
		return nFatallyDamage;
		//printf("bFatally: %d \n", nFatallyDamage);
		//return 0;
	}

	if (nMin + nMax <= 0)
		return 0;

	int	nRes = 0;
	int nDamageRange = nMax - nMin;
	int nDamage = 0;
	if (nDamageRange < 0) 
	{
		nDamage = nMax + g_Random(-nDamageRange);
	}
	else
	{
		nDamage = nMin + g_Random(nMax - nMin);
	}
	// Tinh % ngu hanh tuong khac
	int nSeriesPercent = 0;
	switch(Npc[nAttacker].m_Series)
	{
		case 0:
			switch(m_Series)
			{	
				case 1:
					nSeriesPercent -= nSeriesDamage;
					break;
				default:
					break;
			}
			break;
		case 1:
			switch(m_Series)
			{	
				case 4:
					nSeriesPercent -= nSeriesDamage;
					break;
				default:
					break;
			}
			break;
		case 2:
			switch(m_Series)
			{	
				case 3:
					nSeriesPercent -= nSeriesDamage;
					break;
				default:
					break;
			}
			break;
		case 3:
			switch(m_Series)
			{	
				case 0:
					nSeriesPercent -= nSeriesDamage;
					break;
				default:
					break;
			}
			break;
		case 4:
			switch(m_Series)
			{	
				case 2:
					nSeriesPercent -= nSeriesDamage;
					break;
				default:
					break;
			}
			break;
		default:
			break;
	}
	// Tinh khang ngu hanh
	nRes = nSeriesPercent;
	// Tinh khang theo loai damage
	switch(nType)
	{
	case damage_physics:
		nRes += m_CurrentPhysicsResist;
		if (nRes > m_CurrentPhysicsResistMax)
		{
			nRes = m_CurrentPhysicsResistMax;
		}
		else if(nRes < -m_CurrentPhysicsResistMax)
		{
			nRes = -m_CurrentPhysicsResistMax;
		}
		if (nRes > MAX_RESIST)
		{
			nRes = MAX_RESIST;
		}

		if (bIsMelee)
		{
			nMax = m_CurrentMeleeDmgRetPercent;
		}
		else
		{
			nMax = m_CurrentRangeDmgRetPercent;
		}
		break;
	case damage_cold:
		nRes += m_CurrentColdResist;
		if (nRes > m_CurrentColdResistMax)
		{
			nRes = m_CurrentColdResistMax;
		}
		else if(nRes < -m_CurrentColdResistMax)
		{
			nRes = -m_CurrentColdResistMax;
		}
		if (nRes > MAX_RESIST)
		{
			nRes = MAX_RESIST;
		}

		nMax = m_CurrentRangeDmgRetPercent;
		break;
	case damage_fire:
		nRes += m_CurrentFireResist;
		if (nRes > m_CurrentFireResistMax)
		{
			nRes = m_CurrentFireResistMax;
		}
		else if(nRes < -m_CurrentFireResistMax)
		{
			nRes = -m_CurrentFireResistMax;
		}
		if (nRes > MAX_RESIST)
		{
			nRes = MAX_RESIST;
		}

		nMax = m_CurrentRangeDmgRetPercent;
		break;
	case damage_light:
		nRes += m_CurrentLightResist;
		if (nRes > m_CurrentLightResistMax)
		{
			nRes = m_CurrentLightResistMax;
		}
		else if(nRes < -m_CurrentLightResistMax)
		{
			nRes = -m_CurrentLightResistMax;
		}
		if (nRes > MAX_RESIST)
		{
			nRes = MAX_RESIST;
		}
		
		nMax = m_CurrentRangeDmgRetPercent;
		break;
	case damage_poison:
		nRes += m_CurrentPoisonResist;
		if (nRes > m_CurrentPoisonResistMax)
		{
			nRes = m_CurrentPoisonResistMax;
		}
		else if(nRes < -m_CurrentPoisonResistMax)
		{
			nRes = -m_CurrentPoisonResistMax;
		}
		if (nRes > MAX_RESIST)
		{
			nRes = MAX_RESIST;
		}
		
		nMax = m_CurrentRangeDmgRetPercent;
		m_nLastPoisonDamageIdx = nAttacker;
		break;
	case damage_magic:
		nRes = 0;
		break;
	default:
		nRes = 0;
		break;
	}

	if (nDamage < 0)
		return 0;


	nDamage = nDamage * (100 - nRes) / 100;
	// giam sat thuong cua player vao npc
	if (this->m_nReducedDamge > 0 && Npc[nAttacker].m_Kind == kind_player && this->m_Kind != kind_player)
	{
		if (this->m_nReducedDamge > 100)
			this->m_nReducedDamge = 99;
		nDamage -= (nDamage * m_nReducedDamge) / 100;
	}

	// Tinh luc sat thuong khi PK giua 2 player
	if (this->m_Kind == kind_player && Npc[nAttacker].m_Kind == kind_player && bReturn == FALSE)
	{
		if (nIdSkills > 0)
		{
			if (nIdSkills == 302)// bao vu le hoa 
			{
				nDamage = (nDamage/6) * NpcSet.m_nPKDamageRate / 100; 
			}    
			else if (nIdSkills == 339)// Nhiep hon nguyet anh
			{
				nDamage = (nDamage/8) * NpcSet.m_nPKDamageRate / 100; 
			}  
			else if (nIdSkills == 442)// cuu cung phi tinh
			{  
				nDamage = (nDamage/8) * NpcSet.m_nPKDamageRate / 100; 
			}  
			else if (nIdSkills == 336)// Bang Tung vo anh
			{
				nDamage = (nDamage/3) * NpcSet.m_nPKDamageRate / 100; 
			}
			else if (nIdSkills == 357)// Phi Long Tai Thien
			{
				nDamage = (nDamage) * NpcSet.m_nPKDamageRate / 100;
			}
			else if (nIdSkills == 359)// Thien Ha Vo Cau
			{
				nDamage = (nDamage*3.5) * NpcSet.m_nPKDamageRate / 100;
			}
			else if (nIdSkills == 362)// Thien Ngoai Luu Tinh
			{
				nDamage = (nDamage*8) * NpcSet.m_nPKDamageRate / 100;
			}
			else if (nIdSkills == 361)// Van Long Kich
			{
				nDamage = (nDamage/2.5) * NpcSet.m_nPKDamageRate / 100;
			}
			else if (nIdSkills == 322)// Pha Thien Tram 
			{
				nDamage = (nDamage*40) * NpcSet.m_nPKDamageRate / 100; 
			} 
			else if (nIdSkills == 323)// truy tinh truc nguyet
			{
				nDamage = (nDamage*40) * NpcSet.m_nPKDamageRate / 100; 
			}
			else if (nIdSkills == 325)// truy phong quyet
			{
				nDamage = (nDamage*40) * NpcSet.m_nPKDamageRate / 100;
			} 
			else if (nIdSkills == 328)// Tam Nga Te Tuyet  
			{
				nDamage = (nDamage/18) * NpcSet.m_nPKDamageRate / 100;  
			}
			else if (nIdSkills == 380)// Phong Suong Toai Anh
			{
				nDamage = (nDamage) * NpcSet.m_nPKDamageRate / 100;  
			}
			else if (nIdSkills == 372)// Ngao Tuyet Tieu Phong 
			{
				nDamage = (nDamage/6) * NpcSet.m_nPKDamageRate / 100; 
			} 
			else if (nIdSkills == 375)// Loi dong cuu thien  
			{
				nDamage = (nDamage/3) * NpcSet.m_nPKDamageRate / 100;
			}
			else if (nIdSkills == 365)// Thien Dia Vo Cuc
			{ 
				nDamage = (nDamage/18) * NpcSet.m_nPKDamageRate / 100; 
			} 
			else if (nIdSkills == 368)// Nhan Kiem hop nhat
			{ 
				nDamage = (nDamage/10) * NpcSet.m_nPKDamageRate / 100; 
			}
			else if (nIdSkills == 318)// dat ma do giang 
			{ 
				nDamage = (nDamage/6) * NpcSet.m_nPKDamageRate / 100; 
			}  
			else if (nIdSkills == 319)// hoanh tao thien quan
			{  
				nDamage = (nDamage/6) * NpcSet.m_nPKDamageRate / 100; 
			} 
			else if (nIdSkills == 321)// vo tuong tram   
			{       
				nDamage = (nDamage/8) * NpcSet.m_nPKDamageRate / 100; 
			} 
			else if (nIdSkills == 341)// Tan Hoa Tieu 6x DM
			{       
				nDamage = (nDamage/20) * NpcSet.m_nPKDamageRate / 100; 
			} 
			else
				nDamage = nDamage/2 * NpcSet.m_nPKDamageRate / 100;
		} 
		else
		{
			nDamage = nDamage/2 * NpcSet.m_nPKDamageRate / 100;
		}
	}
	
	// Giam sat thuong Npc xuong
	if (this->m_Kind == kind_player && Npc[nAttacker].m_Kind != kind_player && bReturn == FALSE)
	{	
		if (Npc[nAttacker].m_cGold.GetGoldType() != 1)
		{
			nDamage = (nDamage * 50) / 100;
		}
	}
/*
	////////////////////DIEU CHINH SAT THUONG CAC HE
	// Tang sat thuong cua he kim 40%
	if (Npc[nAttacker].m_Series == 0 && Npc[nAttacker].m_Kind == kind_player)
	{
		nDamage = nDamage + (nDamage * 40) / 100; 
	}
	// Tang sat thuong cua he moc 
	if (Npc[nAttacker].m_Series == 1 && Npc[nAttacker].m_Kind == kind_player)
	{
		nDamage = nDamage*2;
	}
	// Giam sat thuong cua he thuy 60%
	if (Npc[nAttacker].m_Series == 2 && Npc[nAttacker].m_Kind == kind_player)
	{
		nDamage = (nDamage * 60) / 100; 
	}
	// Tang sat thuong cua he hoa 60%
	if (Npc[nAttacker].m_Series == 3 && Npc[nAttacker].m_Kind == kind_player)
	{
		nDamage = nDamage + (nDamage * 60) / 100; 
	}
	// Tang sat thuong cua he tho 50%
	if (Npc[nAttacker].m_Series == 4 && Npc[nAttacker].m_Kind == kind_player)
	{
		nDamage = nDamage + (nDamage * 50) / 100; 
	}
*/
	 
	////////////////////DIEU CHINH SAT THUONG CAC HE
	// Giam sat thuong cua he moc 60%
	if (Npc[nAttacker].m_Series == 0 && Npc[nAttacker].m_Kind == kind_player)
	{
		nDamage = (nDamage * 60) / 100; 
	}
	//  Giam sat thuong cua he moc 
	if (Npc[nAttacker].m_Series == 1 && Npc[nAttacker].m_Kind == kind_player)
	{
		nDamage = (nDamage * 60) / 100; 
	}
	// Giam sat thuong cua he thuy 60%
	if (Npc[nAttacker].m_Series == 2 && Npc[nAttacker].m_Kind == kind_player)
	{
		nDamage = (nDamage * 60) / 100; 
	}
	//  Giam sat thuong cua he hoa 60%
	if (Npc[nAttacker].m_Series == 3 && Npc[nAttacker].m_Kind == kind_player)
	{
		nDamage = (nDamage * 60) / 100; 
	}
	// Giam sat thuong cua he tho 60%
	if (Npc[nAttacker].m_Series == 4 && Npc[nAttacker].m_Kind == kind_player)
	{
		nDamage = (nDamage * 60) / 100; 
	}

	 
	// Tinh ty le hut
	if (m_CurrentLife > 0) 
	{	
		if(!IsPlayer())
		{		
			int nStole = 0;
			// Hut sinh luc
			if(nDamage > this->m_CurrentLifeMax)
			{
				nStole = m_CurrentLifeMax * nLifeStole / 100;
			}
			else 
			{
				nStole = nDamage * nLifeStole / 100;
			}
			
			if ((Npc[nAttacker].m_CurrentLifeMax - Npc[nAttacker].m_CurrentLife) > nStole)
			{	
				Npc[nAttacker].m_CurrentLife += nStole;
			}
			else
			{
				Npc[nAttacker].m_CurrentLife = Npc[nAttacker].m_CurrentLifeMax;
			}
			// Hut noi luc
			if(nDamage > this->m_CurrentLifeMax)
			{
				nStole = m_CurrentLifeMax * nManaStole / 100;
			}
			else
			{
				nStole = nDamage * nManaStole / 100;
			}

			if ((Npc[nAttacker].m_CurrentManaMax - Npc[nAttacker].m_CurrentMana) > nStole)
			{
				Npc[nAttacker].m_CurrentMana += nStole;
			}
			else
			{
				Npc[nAttacker].m_CurrentMana = Npc[nAttacker].m_CurrentManaMax;
			}
			// Hut the luc
			if(nDamage > this->m_CurrentLifeMax)
			{
				nStole = m_CurrentLifeMax * nStaminaStole / 100;
			}
			else
			{
				nStole = nDamage * nStaminaStole / 100;
			}

			if ((Npc[nAttacker].m_CurrentStaminaMax - Npc[nAttacker].m_CurrentStamina) > nStole)
			{
				Npc[nAttacker].m_CurrentStamina += nStole;
			}
			else
			{
				Npc[nAttacker].m_CurrentStamina = Npc[nAttacker].m_CurrentStaminaMax;
			}
		}
	}
	// Tinh phan don sat thuong
	if (nAttacker > 0 && bReturn == FALSE && Npc[nAttacker].m_Index != m_Index)
	{
		if (bIsMelee)
		{	
			if (nType != damage_poison)
			{
				nMin = m_CurrentMeleeDmgRet;
				nMin += nDamage * nMax / 100;
				
				if(nMin > 0)
				{
					//printf("Pahn don sat thuong	0	\n");
					// Noi luc ho than phan don %
					if (Npc[nAttacker].m_ManaShield > 0)
					{	
						int nManaDamageA = nMin * Npc[nAttacker].m_ManaShield / 100;
						if (nManaDamageA < Npc[nAttacker].m_CurrentMana)
						{
							Npc[nAttacker].m_CurrentMana -= nManaDamageA;
							nMin -= nManaDamageA;
						}
						else
						{
							nMin -= Npc[nAttacker].m_CurrentMana;
							Npc[nAttacker].m_CurrentMana = 0;       
						}
							//printf("Phan don sat thuong	1	\n");
					}
					
					// Khang phan don sat thuong
					if(Npc[nAttacker].m_CurrentReturnResist > 0)
					{	
						int nResistDamage = nMin * Npc[nAttacker].m_CurrentReturnResist / 100;
						nMin -= nResistDamage;
						//printf("Phan don sat thuong 2\n");
					}

					int nReturnDamage = 0;
					nReturnDamage = Npc[nAttacker].CalcDamage(m_Index, nMin, nMin, 0, damage_magic, FALSE, FALSE, TRUE);
					if(nReturnDamage > 0 && !Npc[nAttacker].IsPlayer())
					{
						SendBloodNo(Npc[nAttacker].m_dwID, nReturnDamage);
						//printf("Phan don sat thuong 3\n");
					}
				}
			}
		
		}
		// Phan don tam xa
		else
		{	
			if (nType != damage_poison)
			{	
				nMin = m_CurrentRangeDmgRet;
				nMin += nDamage * nMax / 100;
				
				if(nMin > 0)
				{
					//printf("Phan don sat thuong 4: %d - %d\n" , Npc[nAttacker].m_CurrentReturnResist,m_CurrentRangeDmgRet);
					// Noi luc ho than %
					if (Npc[nAttacker].m_ManaShield > 0)
					{	
						int nManaDamageA = nMin * Npc[nAttacker].m_ManaShield / 100;
						if (nManaDamageA < Npc[nAttacker].m_CurrentMana)
						{ 
							Npc[nAttacker].m_CurrentMana -= nManaDamageA;
							nMin -= nManaDamageA;
						}
						else
						{
							nMin -= Npc[nAttacker].m_CurrentMana;
							Npc[nAttacker].m_CurrentMana = 0;       
						}
					}
					// Khang phan don sat thuong
					if(Npc[nAttacker].m_CurrentReturnResist > 0)
					{	
						int nResistDamage = nMin * Npc[nAttacker].m_CurrentReturnResist / 100;
						nMin -= nResistDamage;
						//printf("Phan don sat thuong 5: %d - %d\n" , Npc[nAttacker].m_CurrentReturnResist,m_CurrentRangeDmgRet);
					}
					
					int nReturnDamage = 0;
					nReturnDamage = Npc[nAttacker].CalcDamage(m_Index, nMin, nMin, 0, damage_magic, FALSE, FALSE, TRUE);
					if(nReturnDamage > 0 && !Npc[nAttacker].IsPlayer())
					{
						SendBloodNo(Npc[nAttacker].m_dwID, nReturnDamage); 
						//printf("Phan don sat thuong 6: %d - %d\n" , Npc[nAttacker].m_CurrentReturnResist,m_CurrentRangeDmgRet);
					}
				}
			}	
		}
	}
	
	// Phan don doc sat
	if (nAttacker > 0 && m_PoisonDamageReturn > 0)
	{	
		if (nType == damage_poison)
		{
			nMin = 0;
			nMin += nDamage * m_PoisonDamageReturn / 100;

			int nReturnDamage = 0;
			nReturnDamage = Npc[nAttacker].CalcDamage(m_Index, nMin, nMin, 0, damage_magic, FALSE, FALSE, TRUE);
			if(nReturnDamage > 0 && !Npc[nAttacker].IsPlayer())
			{
				SendBloodNo(Npc[nAttacker].m_dwID, nReturnDamage);
			} 
			//printf("Pahn don sat thuong 7\n");
		}
	}


	// Noi luc ho than %
	int nManaDamage = 0;
	if (m_ManaShield > 0)
	{	
		nManaDamage = nDamage * m_ManaShield / 100;
		m_CurrentMana -= nManaDamage;
		if (m_CurrentMana < 0)
		{
			nDamage -= m_CurrentMana; 
			m_CurrentMana = 0;
		}
		else
		{
			nDamage -= nManaDamage;
		}
	}

	// Noi luc ho than diem
	if(m_CurrentDamageReduce > 0)
	{	
		int nStartDamage = 0, nMagicShield = 0;

		nStartDamage = nDamage * 40 / 100;

		nMagicShield = m_CurrentDamageReduce / 5;
		if (this->m_Kind == kind_player && Npc[nAttacker].m_Kind == kind_player && bReturn == FALSE)
			nMagicShield = nMagicShield * NpcSet.m_nPKDamageRate / 100;
		
		if(nMagicShield > nStartDamage) 
			nMagicShield = nStartDamage;
		
		if (m_CurrentDamageReduce > 0)
			nMagicShield = m_CurrentManaMax * m_CurrentDamageReduce / 100;

		if(nDamage > nMagicShield)
		{
			nDamage -= nMagicShield;
		}
		else
		{
			nDamage = 0;
		}
	}

/*
	if (m_CurrentDamageReduce > 0)
	{
		int Noi_luc_ho_than_diem = m_CurrentDamageReduce / 5;
		if (Noi_luc_ho_than_diem > nDamage)
		{
			if (m_CurrentMana > nDamage)
			{
				m_CurrentMana -= nDamage;
			}
		}
		else
		{
			nDamage -= Noi_luc_ho_than_diem;

			if (m_CurrentMana > nDamage)
			{
				m_CurrentMana -= nDamage;
			} 
		}
	}
*/
	m_nLastDamageIdx = nAttacker;

	// Tinh Exp thong qua damage
	int nCalcExpDamage = 0;
	if(Npc[nAttacker].m_Kind == kind_player && Npc[nAttacker].m_nPlayerIdx > 0)
	{	
		if(this->m_Kind == kind_normal)
		{
			if(nDamage >= (int)m_CurrentLife)
				nCalcExpDamage = (int)m_CurrentLife;
			else
				nCalcExpDamage = nDamage;
			this->m_cDeathCalcExp.AddDamage(Npc[nAttacker].m_nPlayerIdx, nCalcExpDamage);
		}
		/*if(this->m_Kind == kind_player)
		{
			if(Player[m_nPlayerIdx].TaskGetSaveVal(TV_CALCDAMAGE) > 0)
			{
				nCalcExpDamage = nDamage + nManaDamage;
				this->m_cDeathCalcExp.AddDamage(Npc[nAttacker].m_nPlayerIdx, nCalcExpDamage);
			}
		}*/
	}
/*	//giam dame doc
	if (this->m_Kind == kind_player && Npc[nAttacker].m_Kind == kind_player && m_PoisonState.nTime > 0)//nguoi 
	{
		nDamage = nDamage/3;
	} 
	if (this->m_Kind == kind_player && Npc[nAttacker].m_Kind != kind_player && m_PoisonState.nTime > 0)//quai 
	{
		nDamage = nDamage/2; //quai
	}
*/
	if(nDamage < 0)
		nDamage = 1;

	if(nDamage > (int)m_CurrentLife)
		nDamage = (int)m_CurrentLife;

	m_CurrentLife -= nDamage;
	 
	//printf("m_CurrentLife: %d nDamage: %d nIdSkills: %d, bDoHurt: %d\n", m_CurrentLife, nDamage, nIdSkills,bDoHurt);
	  
	if (nDamage > 0)
	{
		if(IsPlayer()) //Time Box
		{
			Player[m_nPlayerIdx].m_ItemList.Abrade(enumAbradeDefend);//#m礽 m遪 ph遪g th?khi b?tr髇g damage
			/*if(Player[m_nPlayerIdx].m_dwTimeBoxId >0)//#產ng h竔 qu?ho芻 產ng l祄 g? TimeBox PaceBar b?tr髇g damage
			{
				Player[m_nPlayerIdx].m_dwTimeBoxId = 0;
				S2C_TIME_BOX NetCommand;					 //#sync client ng総 TimeBox
				NetCommand.ProtocolType = s2c_timebox;
				strcpy(NetCommand.Value, "");
				NetCommand.Value1 = -1;
				strcpy(NetCommand.Value2, "");
				if(g_pServer && Player[m_nPlayerIdx].m_nNetConnectIdx != -1)
					g_pServer->PackDataToClient(Player[m_nPlayerIdx].m_nNetConnectIdx,&NetCommand,sizeof(S2C_TIME_BOX));
			}
			*/
		}
		m_CurrentMana += m_CurrentDamage2Mana * nDamage / 100;
		if (m_CurrentMana > m_CurrentManaMax)
		{
			m_CurrentMana = m_CurrentManaMax;
		}
		if (bDoHurt) // boom lag vi tri ASSERT thong bao lien tuc;
		{
			DoHurt(0,0,0,bDoHurt);
			//printf("bDoHurt: %d TGPH: %d HitRank: %d\n", bDoHurt, Npc[nAttacker].m_CurrentHitNpcRecover, Npc[nAttacker].m_CurrentHitRank);
		}
	}

	if (m_CurrentLife <= 0)
	{	
		if (m_AutoDeathSkill.nValue[0] > 0)
		{	
			//WORD wSkillId = m_AutoDeathSkill.nValue[0] / 256;
			//BYTE btSkillLevel = m_AutoDeathSkill.nValue[0] % 256;
			//AutoDoSkill(wSkillId, btSkillLevel, -1, m_Index);
		}
		int nMode = DeathCalcPKValue(nAttacker);
		DoDeath(nMode,nAttacker);

		if (m_Kind == kind_player)
			Player[m_nPlayerIdx].m_cPK.CloseAll();
	} 

	return nDamage; 
}
 
void KNpc::AddPoisonState(int nLauncher, int nDamage, int nDamage1, int nTime, int nCycle)
{	
	if(!nDamage || !nTime)
		return; 
//*/	  
	if (m_PoisonState.nTime == 0)
	{
		m_PoisonState.nTime		= nTime;
		m_PoisonState.nValue[0]	= nDamage;
		m_PoisonState.nValue[1]	= nCycle;
	}
	else
	{  
		int d, t, c;
		d	= m_PoisonState.nValue[0];
		t	= m_PoisonState.nTime;
		c	= m_PoisonState.nValue[1];
		if (c > 0 && nCycle > 0 && d > 0 && nDamage > 0) 
		{
			printf("nDamage3: %d, TGTD: %d nDamage2: %d\n", nDamage, nTime,nCycle);
			m_PoisonState.nTime	= (int)((1.0 * t * d * nCycle + 1.0 * nTime * nDamage * c) /(1.0 * d * nCycle + 1.0 * nDamage * c));
			
			int nDamageMax	= 123456;//123456;
			if( IsPlayer() )
			   nDamageMax = 60; //550 phe 5doc

			int nAddDamage; 
			if (nDamageMax > 0)
			{
				nAddDamage = nDamageMax * nCycle * 2 - d * nCycle / c; //khuc nay11
				if (nDamage < nAddDamage)
					nAddDamage = nDamage;
			}					
			else
			{
				nAddDamage = nDamage;
			}
			if (nAddDamage > 0)
			{
				m_PoisonState.nValue[0]	= ((c + nCycle) * d / c + (c + nCycle) * nAddDamage / nCycle) / 2;
				m_PoisonState.nValue[1]	= (c + nCycle) / 2;
			}
		}
	}

/*
	int giam_trung_doc = nTime;
	int tong_doc_sat0 = nDamage;
	int tong_doc_sat1 = nDamage1;
	int tong_doc_sat2 = nCycle;
	if (m_PoisonState.nTime == 0) //htkl
	{
		m_PoisonState.nTime = tong_doc_sat1 * (100 - giam_trung_doc) / 100;
		m_PoisonState.nValue[0] = tong_doc_sat0 * (100 - giam_trung_doc/2)/100;
		m_PoisonState.nValue[1] = tong_doc_sat2;
	}
	else
	{
		int d1, d2, t1, t2, c1, c2, d3,t3,c3,e;
		d1 = m_PoisonState.nValue[0];
		d2 = tong_doc_sat0 * (100 - giam_trung_doc)/100;
		t1 = m_PoisonState.nTime;
		t2 = tong_doc_sat1 * (100 - giam_trung_doc) / 100;
		c1 = m_PoisonState.nValue[1];
		c2 = tong_doc_sat2;
		if (c1 > 0 && c2 > 0 && d1 > 0 && d2 > 0 && t1 > 0 && t2 > 0) 
		{
			c3 = (c1 + c2)/2;
			t3 = max(t1,t2);
			d3 = (d1 * t1 / c1 + d2 * t2 / c2) * c3 / t3 ;

			m_PoisonState.nValue[0] = d3;
			m_PoisonState.nTime = min(t3 + t2/3, t2 * 4);
			m_PoisonState.nValue[1] = c3;
			 
			int NpcPoisonDamageMax=0,PlayerPoisonDamageMax=0,PoisonDamageMax=0; //合成毒伤害的最大值	
			g_GameSetTing.GetInteger("ServerConfig","NpcPoisonDamageMax",50,&NpcPoisonDamageMax);
			g_GameSetTing.GetInteger("ServerConfig","PlayerPoisonDamageMax",5,&PlayerPoisonDamageMax);


			if (this->m_Kind == kind_player && Npc[nLauncher].m_Kind == kind_player)//nguoi
			{ 
				PoisonDamageMax = PlayerPoisonDamageMax;
				//printf("dame nguoi\n");
			} 
			else//quai
			{
				PoisonDamageMax = NpcPoisonDamageMax;
				//printf("dame quai\n"); 
			} 

			if (m_PoisonState.nValue[0] >= PoisonDamageMax)
		        m_PoisonState.nValue[0]  = PoisonDamageMax;
			 
		}
	}


/* //tinh kiem master
	int giam_trung_doc = nTime;
	int tong_doc_sat0 = nDamage;
	int tong_doc_sat1 = 18;
	int tong_doc_sat2 = nCycle;

		if (m_PoisonState.nTime == 0)
		{
			m_PoisonState.nTime = tong_doc_sat1 - (tong_doc_sat1 * giam_trung_doc / MAX_PERCENT);
			m_PoisonState.nValue[0] = tong_doc_sat0;
			m_PoisonState.nValue[1] = tong_doc_sat2;
		}
		else
		{
			int d1, d2, t1, t2, c1, c2;
			d1 = m_PoisonState.nValue[0];
			d2 = tong_doc_sat0;
			t1 = m_PoisonState.nTime;
			t2 = tong_doc_sat1 - (tong_doc_sat1 * giam_trung_doc / MAX_PERCENT);
			c1 = m_PoisonState.nValue[1];
			c2 = tong_doc_sat2;
			if (c1 > 0 && c2 > 0 && d1 > 0 && d2 > 0) 
			{
				m_PoisonState.nValue[0] = ((c1 + c2) * d1 / c1 + (c1 + c2) * d2 / c2) / 2;
				m_PoisonState.nTime = (t1 * d1 * c2 + t2 * d2 * c1) / (d1 * c2 + d2 * c1);
				m_PoisonState.nValue[1] = (c1 + c2) / 2;
			}
		}
*/


	m_nLastPoisonDamageIdx	= nLauncher;
}

#endif

//主动技能攻击伤害   收到伤害
#ifdef _SERVER

BOOL KNpc::DiretorDamage(int nLauncher,int nType,int nDamage,int nNum,BOOL bIsMelee,int bDoHurt)  //nSkillId 为攻击者者使用的技能ID
{//直接的伤害
	 int nCurLifeDamage=0;

	 /*if (m_IsNotDeath>=1)
	 {//无敌NPC
		 return FALSE;
	 }*/

     if (nType==0 && nLauncher)
	 { 
        for (int i=0;i<nNum;++i)   //伤害多少次
            nCurLifeDamage += CalcDamage(nLauncher,0, nDamage, nDamage, damage_magic, bIsMelee, bDoHurt);	
		
		  m_CurrentLife -= nCurLifeDamage;   //被攻击者
		  //printf("Debug: DiretorDamage: %d",nCurLifeDamage);
	 } 
     else
          m_CurrentLife=0;      //直接杀死

	 m_nPeopleIdx = nLauncher;      //(被攻击者)锁定 发技能者

	 //Npc[nLauncher].m_nPeopleIdx=m_Index;
	 if  (nLauncher)
	     Npc[nLauncher].m_bLockNpcDwID=m_dwID;

	 m_nLastDamageIdx = nLauncher;  //用于执行脚本的

	//printf("Debug: m_nLastDamageIdx: %d",m_nLastDamageIdx);

	 if (m_CurrentLife<=0)      //被攻击者
	 {
		 if  (nLauncher)
		 {
			 Npc[nLauncher].m_nPeopleIdx=0;
			 Npc[nLauncher].m_bLockNpcDwID=0;
		 }
		  m_nPeopleIdx = 0;
		  
		  m_CurrentLife=0;
		  int nMode = DeathCalcPKValue(nLauncher); //攻击者 惩罚模式
		  DoDeath(nMode,nLauncher);                //没有血了 死亡
		   //if 死的是玩家，关闭他的仇杀和切磋状态
		  if (m_Kind == kind_player)
			Player[m_nPlayerIdx].m_cPK.CloseAll();
	 } 

	return TRUE;
}
#endif

//设置立即技能状态效果
void KNpc::SetImmediatelySkillEffect(int nLauncher, void *pData, int nDataNum)
{
	if (!pData || !nDataNum)
		return;

	KMagicAttrib*	pTemp = (KMagicAttrib *)pData;
	//_ASSERT(nDataNum <= MAX_SKILL_STATE);
	if (nDataNum >= MAX_MISSLE_DAMAGEATTRIB)
		nDataNum = MAX_MISSLE_DAMAGEATTRIB;

	for (int i = 0; i < nDataNum; ++i)
	{
		ModifyAttrib(nLauncher, pTemp);//改变属性-技能加成
		++pTemp;
	}
}
//dame vu khi
//技能子弹属性数据  --被动子弹伤害计算
void KNpc::AppendSkillEffect(int uId, int nIsMaigc ,BOOL bIsPhysical, BOOL bIsMelee, void *pSrcData, void *pDesData, int nSkillEnhance)
{
	if (!pSrcData || !pDesData) {
	    return;
	}

	if (uId <= 0 || uId >= MAX_SKILL) {
    	return;
	}
		// Lay % ma skill dc ho tro
		//int nSkillEnhance = 0;

		int nMinDamage,nMaxDamage,nTempVal=0;
	
		nMinDamage = m_PhysicsDamage.nValue[0] + m_CurrentAddPhysicsDamage; //基本伤害+铺点伤害
		nMaxDamage = m_PhysicsDamage.nValue[2] + m_CurrentAddPhysicsDamage;
//===========================================================================
	KMagicAttrib* pTemp = (KMagicAttrib *)pSrcData;   //脚本解析出来的技能数据
	KMagicAttrib* pDes  = (KMagicAttrib *)pDesData;   //通过计算后的数据
	// Tinh do chinh xac
	//1
	if (pTemp->nAttribType == magic_attackrating_p)
	{
		pDes->nAttribType = magic_attackrating_v;
		pDes->nValue[0] = m_CurrentAttackRating + m_AttackRating * pTemp->nValue[0] / 100;
	}
	else
	{
		pDes->nAttribType = magic_attackrating_v;
		pDes->nValue[0] = m_CurrentAttackRating;
	}
	// Tinh bo qua ne tranh
	pTemp++;
	pDes++;//2
	if (pTemp->nAttribType == magic_ignoredefense_p)
	{
		pDes->nAttribType = magic_ignoredefense_p;
		pDes->nValue[0] = pTemp->nValue[0];
	}
	// Tinh luc danh vat ly
	pTemp++;
	pDes++;//3
	if (pTemp->nAttribType == magic_physicsenhance_p)
	{
		pDes->nAttribType = magic_physicsdamage_v;
		pDes->nValue[0] = nMinDamage * (100 + pTemp->nValue[0]) / 100;
		pDes->nValue[2] = nMaxDamage * (100 + pTemp->nValue[0]) / 100;
		if (IsPlayer())
		{
			if (Player[m_nPlayerIdx].m_ItemList.GetWeaponType() == equip_meleeweapon)
			{
				pDes->nValue[0] += nMinDamage * m_CurrentMeleeEnhance[Player[m_nPlayerIdx].m_ItemList.GetWeaponParticular()] / 100;
				pDes->nValue[2] += nMaxDamage * m_CurrentMeleeEnhance[Player[m_nPlayerIdx].m_ItemList.GetWeaponParticular()] / 100;
			}
			else if (Player[m_nPlayerIdx].m_ItemList.GetWeaponType() == equip_rangeweapon)
			{
				pDes->nValue[0] += nMinDamage * m_CurrentRangeEnhance / 100;
				pDes->nValue[2] += nMaxDamage * m_CurrentRangeEnhance / 100;
			}
			else
			{
				pDes->nValue[0] += nMinDamage * m_CurrentHandEnhance / 100;
				pDes->nValue[2] += nMaxDamage * m_CurrentHandEnhance / 100;
			}
		}
	}
	else if (pTemp->nAttribType == magic_physicsdamage_v)
	{
		pDes->nAttribType = magic_physicsdamage_v;
		pDes->nValue[0] = pTemp->nValue[0] * (100 + nSkillEnhance) / 100;
		pDes->nValue[2] = pTemp->nValue[2] * (100 + nSkillEnhance) / 100;
		// Luc danh vat ly noi cong
		if (!bIsPhysical)
		{
			pDes->nValue[0] += m_MagicPhysicsDamage.nValue[0];
			pDes->nValue[2] += m_MagicPhysicsDamage.nValue[2];
		}
	}
	// Tinh luc danh bang
	pTemp++;
	pDes++;//4
	if (pTemp->nAttribType == magic_colddamage_v)
	{
		pDes->nAttribType = magic_colddamage_v;
		pDes->nValue[0] = pTemp->nValue[0];
		pDes->nValue[1] = this->m_CurrentColdEnhance + pTemp->nValue[1];
		pDes->nValue[2] = pTemp->nValue[2];
		
		if(nSkillEnhance > 0)
		{
			pDes->nValue[0] += pTemp->nValue[0] * nSkillEnhance / 100;
			pDes->nValue[2] += pTemp->nValue[2] * nSkillEnhance / 100;
		}

		// Luc danh bang noi cong
		if (!bIsPhysical)
		{
			pDes->nValue[0] += m_CurrentMagicColdDamage.nValue[0];
			pDes->nValue[2] += m_CurrentMagicColdDamage.nValue[2];
	   }
	}
	//Luc danh bang ngoai cong
	if (bIsPhysical)
	{	
		const int nTempMin = pDes->nValue[0] + m_CurrentColdDamage.nValue[0];
		pDes->nValue[0] = nTempMin;

		if(nTempMin)
		{
			int nTempTime = this->m_CurrentColdEnhance + this->m_CurrentColdDamage.nValue[1];
			if(nTempTime < pDes->nValue[1])
				nTempTime = pDes->nValue[1];
			pDes->nValue[1] = nTempTime;
		}
		pDes->nValue[2] += this->m_CurrentColdDamage.nValue[2];
		//g_DebugLog("Bang sat: %d",pDes->nValue[2]);
	}
	// Tinh luc danh hoa
	pTemp++;
	pDes++;//5
	if (pTemp->nAttribType == magic_firedamage_v)
	{
		pDes->nAttribType = magic_firedamage_v;
		// pDes->nValue[0] = pTemp->nValue[0] * (100 + nSkillEnhance) / 100;
		// pDes->nValue[2] = pTemp->nValue[2] * (100 + nSkillEnhance) / 100;
		pDes->nValue[0] = pTemp->nValue[0]*(100+nSkillEnhance)/100 + pTemp->nValue[0]*(100+nSkillEnhance)/100 * m_CurrentFireEnhance/100;
		pDes->nValue[2] = pTemp->nValue[2]*(100+nSkillEnhance)/100 + pTemp->nValue[2]*(100+nSkillEnhance)/100 * m_CurrentFireEnhance/100;
		//printf("bIsPhysical: %d m_CurrentMagicFireDamage.nValue[0]: %d m_CurrentMagicFireDamage.nValue[2]: %d\n",bIsPhysical,m_CurrentMagicFireDamage.nValue[0],m_CurrentMagicFireDamage.nValue[2]);
		// Luc danh hoa noi cong
		if (!bIsPhysical)
		{
			/*pDes->nValue[0] += m_CurrentMagicFireDamage.nValue[0];
			pDes->nValue[2] += m_CurrentMagicFireDamage.nValue[2];*/
		    pDes->nValue[0] += m_CurrentMagicFireDamage.nValue[0] + m_CurrentMagicFireDamage.nValue[0] * m_CurrentFireEnhance /100;
      		pDes->nValue[2] += m_CurrentMagicFireDamage.nValue[2] + m_CurrentMagicFireDamage.nValue[2] * m_CurrentFireEnhance /100;
		}
		//printf("!bIsPhysical: %d m_CurrentMagicFireDamage.nValue[0]: %d m_CurrentMagicFireDamage.nValue[2]: %d\n",bIsPhysical,m_CurrentMagicFireDamage.nValue[0],m_CurrentMagicFireDamage.nValue[2]);
	}
	// Luc danh hoa ngoai cong
	if (bIsPhysical)
	{
/*		pDes->nValue[0] += m_CurrentFireDamage.nValue[0];
		pDes->nValue[2] += m_CurrentFireDamage.nValue[2];*/
		pDes->nValue[0] += m_CurrentFireDamage.nValue[0] + m_CurrentFireDamage.nValue[0] * m_CurrentFireEnhance /100;
		pDes->nValue[2] += m_CurrentFireDamage.nValue[2] + m_CurrentFireDamage.nValue[2] * m_CurrentFireEnhance /100;	
		//printf("bIsPhysical: %d m_CurrentMagicFireDamage.nValue[0]: %d m_CurrentMagicFireDamage.nValue[2]: %d\n",bIsPhysical,m_CurrentMagicFireDamage.nValue[0],m_CurrentMagicFireDamage.nValue[2]);
	}
	// Tinh luc danh loi
	pTemp++;
	pDes++;
	if (pTemp->nAttribType == magic_lightingdamage_v)
	{
		pDes->nAttribType = magic_lightingdamage_v;
/*		pDes->nValue[0] = pTemp->nValue[0] + (pTemp->nValue[2] - pTemp->nValue[0]) * m_CurrentLightEnhance / 100;
		pDes->nValue[0] = pDes->nValue[0] * (100 + nSkillEnhance) / 100;
		pDes->nValue[2] = pTemp->nValue[2] * (100 + nSkillEnhance) / 100;*/
		pDes->nValue[0] = pTemp->nValue[0] *(100 + nSkillEnhance)/100 + (pTemp->nValue[2]*(100+nSkillEnhance)/100 - pTemp->nValue[0]*(100+nSkillEnhance)/100) * m_CurrentLightEnhance / 100;
		pDes->nValue[2] = pTemp->nValue[2] *(100 + nSkillEnhance) / 100;
		// Luc danh loi noi cong
		if (!bIsPhysical)
		{
			pDes->nValue[0] += m_CurrentMagicLightDamage.nValue[0];
			pDes->nValue[2] += m_CurrentMagicLightDamage.nValue[2];
		}
	}
	// Luc danh loi ngoai cong
	if (bIsPhysical)
	{
		pDes->nValue[0] += m_CurrentLightDamage.nValue[0];
		pDes->nValue[2] += m_CurrentLightDamage.nValue[2];
	}
	// Tinh luc danh doc
	pTemp++;
	pDes++;
	if (pTemp->nAttribType == magic_poisondamage_v)
	{
		pDes->nAttribType = magic_poisondamage_v;

		pDes->nValue[0] = pTemp->nValue[0];
		pDes->nValue[1] = pTemp->nValue[1];
		pDes->nValue[2] = pTemp->nValue[2]; // nhim fix;
		if(pDes->nValue[2] <= 0)
			pDes->nValue[2] = 1;
		
		if(nSkillEnhance > 0)
		{
			pDes->nValue[0] += pDes->nValue[0] * nSkillEnhance / 100;
		}
		// Luc danh doc noi cong
		if (!bIsPhysical) 
		{
			if(m_CurrentMagicPoisonDamage.nValue[0] > 0)
			{
				KMagicAttrib cAttrib;
				cAttrib.nAttribType = magic_poisondamage_v;
				cAttrib.nValue[0] = m_CurrentMagicPoisonDamage.nValue[0];
				cAttrib.nValue[1] = m_CurrentMagicPoisonDamage.nValue[1];
				if(cAttrib.nValue[1] <= 0)
					cAttrib.nValue[1] = 60;
				cAttrib.nValue[2] = m_CurrentMagicPoisonDamage.nValue[2];
				if(cAttrib.nValue[2] <= 0)
					cAttrib.nValue[2] = 1;

				 g_NpcAttribModify.MixPoisonDamage(pDes, &cAttrib);
					//printf("Doc sat - Noi: [0]: %d [1]: %d [2]: %d\n",pDes->nValue[0],pDes->nValue[1],pDes->nValue[2]);		 
			}
		}
	}
	if (bIsPhysical) 
	{
		if(m_CurrentPoisonDamage.nValue[0] > 0)
		{
			g_NpcAttribModify.MixPoisonDamage(pDes, &this->m_CurrentPoisonDamage);
			//printf("Doc sat - Ngoai: [0]: %d [1]: %d [2]: %d\n",pDes->nValue[0],pDes->nValue[1],pDes->nValue[2]);
		}
	}
	// Tinh luc danh ngu hanh
	pTemp++;
	pDes++;
	if (pTemp->nAttribType == magic_magicdamage_v)
	{
		pDes->nAttribType = magic_magicdamage_v;
		pDes->nValue[0] = pTemp->nValue[0]*(100+ nSkillEnhance)/100; //fix dame ngu hanh by kinnox;
		pDes->nValue[2] = pTemp->nValue[2]*(100+ nSkillEnhance)/100;  //fix dame ngu hanh by kinnox;
	}
	// Tinh ngu hanh tuong khac
	pTemp++;
	pDes++;
	if (pTemp->nAttribType == magic_seriesdamage_p)
	{
		pDes->nAttribType = magic_seriesdamage_p;
		pDes->nValue[0] = pTemp->nValue[0];
		pDes->nValue[1] = pTemp->nValue[1];
		pDes->nValue[2] = pTemp->nValue[2];
	}
	// Tinh luc danh chi mang
	pTemp++;
	pDes++;
	if (pTemp->nAttribType == magic_deadlystrike_p)
	{
		pDes->nAttribType = magic_deadlystrike_p;
		pDes->nValue[0] = pTemp->nValue[0];
	}
	// Chi mang o ngoai cong
	if (bIsPhysical)
	{
		pDes->nValue[0] = pDes->nValue[0] + (pDes->nValue[0] * m_CurrentDeadlyStrike / 100);
	}
	// Tinh luc danh chi tu
	pTemp++;
	pDes++;
	if (pTemp->nAttribType == magic_fatallystrike_p)
	{
		pDes->nAttribType = magic_fatallystrike_p;
		pDes->nValue[0] = pTemp->nValue[0];
	}
	// Tinh lam choang
	pTemp++;
	pDes++;
	if (pTemp->nAttribType == magic_stun_p)
	{
		pDes->nAttribType = magic_stun_p;
		pDes->nValue[0] = pTemp->nValue[0];
		pDes->nValue[1] = pTemp->nValue[1];
		pDes->nValue[2] = pTemp->nValue[2];
	}
	// Tinh hut sinh luc
	pTemp++;
	pDes++;
	
	if (pTemp->nAttribType == magic_steallife_p)
	{
		pDes->nAttribType = magic_steallife_p;
		pDes->nValue[0] = pTemp->nValue[0];
	}
	// Hut sinh luc o ngoai cong
	if (bIsPhysical)
	{
		pDes->nValue[0] += m_CurrentLifeStolen;
	}
	// Tinh hut noi luc
	pTemp++;
	pDes++;
	if (pTemp->nAttribType == magic_stealmana_p)
	{
		pDes->nAttribType = magic_stealmana_p;
		pDes->nValue[0] = pTemp->nValue[0];
	}
	// Hut noi luc o ngoai cong
	if (bIsPhysical)
	{
		pDes->nValue[0] += m_CurrentManaStolen;
	}
	// Tinh hut the luc
	pTemp++;
	pDes++;
	if (pTemp->nAttribType == magic_stealstamina_p)
	{
		pDes->nAttribType = magic_stealstamina_p;
		pDes->nValue[0] = pTemp->nValue[0];
	}
	// Hut noi luc o ngoai cong
	if (bIsPhysical)
	{
		pDes->nValue[0] += m_CurrentStaminaStolen;
	}
	// Tinh luyen ky nang
	pTemp++;
	pDes++;
	if (pTemp->nAttribType == magic_skill_skillexp_v)
	{	
		int nPointAdd = m_SkillList.GetPointAdd(uId);
		if(nPointAdd > 0)
		{
			int nLevel = m_SkillList.GetCurrentLevel(uId);
			if(nLevel <= 0)
				nLevel = 1;

			KSkill * pOrdinSkill = (KSkill *) g_SkillManager.GetSkill(uId, nLevel);
			KMagicAttrib *DamageAttribs = pOrdinSkill->GetDamageAttribs();
			for (int i  = 0; i < MAX_MISSLE_DAMAGEATTRIB; i ++)
			{
				if (!(DamageAttribs + i)->nAttribType) continue;
				if ((DamageAttribs + i)->nAttribType == magic_skill_skillexp_v)
				{
					pDes->nAttribType = magic_skill_skillexp_v;
					pDes->nValue[0] = (DamageAttribs + i)->nValue[0];
					pDes->nValue[1] = (DamageAttribs + i)->nValue[1];
					pDes->nValue[2] = (DamageAttribs + i)->nValue[2];
					break;
				}
			}
		}
		else
		{
			pDes->nAttribType = magic_skill_skillexp_v;
			pDes->nValue[0] = pTemp->nValue[0];
			pDes->nValue[1] = pTemp->nValue[1];
			pDes->nValue[2] = pTemp->nValue[2];
		}
	}
	// Tu dong xuat skill
	pTemp++;
	pDes++;
	if(m_AutoAttackSkill.nValue[2] > 0)
	{
		pDes->nAttribType = magic_autoattackskill;
		pDes->nValue[0] = m_AutoAttackSkill.nValue[0];
		pDes->nValue[1] = m_AutoAttackSkill.nValue[1];
		pDes->nValue[2] = m_AutoAttackSkill.nValue[2];
	}
}



void KNpc::ServeMove(int MoveSpeed)  // 服务器移动
{
	if (m_Doing != do_walk && m_Doing != do_run && m_Doing != do_hurt && m_Doing != do_runattack)
		return;

	//int nPlayerIndex=0;
    //if (m_Kind==kind_player)
	//  nPlayerIndex=GetnPlayerIdx();

	if (MoveSpeed <= 0)
	{
		DoStand(); //站着
		return;
	}
	if (MoveSpeed >= SubWorld[m_SubWorldIndex].m_nCellWidth)//移动的速度太快的
		MoveSpeed = SubWorld[m_SubWorldIndex].m_nCellWidth - 1;	//31   15*

	if (m_RegionIndex < 0)
	{
		DoStand(); //站着
		return;
	}

	int x, y;

	SubWorld[m_SubWorldIndex].NewMap2Mps(m_RegionIndex,m_MapX,m_MapY,0,0,&x,&y); //区域格子转换成像素坐标

	x = (x << 10) + m_OffX;
	y = (y << 10) + m_OffY;

	int nRet = m_PathFinder.GetDir(x,y,m_Dir,m_DesX,m_DesY,MoveSpeed,&m_Dir);  //调用寻路函数

	if(nRet == 1)
	{
		if (m_Dir >= MaxMissleDir)
			m_Dir=m_Dir%MaxMissleDir;

		x = g_DirCos(m_Dir,MaxMissleDir)*MoveSpeed*FIND_PATH_DISTANCE;	  //下个要走的点
		y = g_DirSin(m_Dir,MaxMissleDir)*MoveSpeed*FIND_PATH_DISTANCE;	  //下个要走的点
	}
	else
	{
		BOOL bStoppedByBarrier = m_PathFinder.CheckDistance(x >> 10, y >> 10, m_DesX, m_DesY, MoveSpeed);
		DoStand();
#ifdef _SERVER
		if (g_bPositionSyncFixEnabled && bStoppedByBarrier && m_RegionIndex >= 0)
		{
			NPC_PLAYER_TYPE_NORMAL_SYNC sSync;
			memset(&sSync, 0, sizeof(sSync));
			sSync.ProtocolType = (BYTE)s2c_npcsetpos;
			sSync.m_dwNpcID = m_dwID;
			int nMapId = 0;
			GetMpsPos(&sSync.m_dwMapX, &sSync.m_dwMapY, &nMapId);
			sSync.m_wOffX = m_OffX;
			sSync.m_wOffY = m_OffY;
			SendDataToNearRegion(&sSync, sizeof(sSync));
			if (IsPlayer() && m_nPlayerIdx > 0 && g_pServer)
				g_pServer->PackDataToClient(Player[m_nPlayerIdx].m_nNetConnectIdx, (BYTE*)&sSync, sizeof(sSync));
		}
#endif
		return;
	}

	int nOldRegion = m_RegionIndex;
	int nOldMapX = m_MapX;
	int nOldMapY = m_MapY;
	int nOldOffX = m_OffX;
	int nOldOffY = m_OffY;

	m_OffX +=x;
	m_OffY +=y;
//	处理NPC的坐标变幻
//	CELLWIDTH、CELLHEIGHT、OffX、OffY均是放大了1024倍

	if (!m_bClientOnly)
		CURREGION.DecRef(m_MapX, m_MapY,obj_npc);
		//CURREGION.DecNpcRef(m_MapX, m_MapY);
	
	if (m_OffX < 0)
	{
		m_MapX--;
		m_OffX += CELLWIDTH;
	}
	else if (m_OffX > CELLWIDTH)
	{
		m_MapX++;
		m_OffX -= CELLWIDTH;
	}

	if (m_OffY < 0)
	{
		m_MapY--;
		m_OffY += CELLHEIGHT;
	}
	else if (m_OffY > CELLHEIGHT)
	{
		m_MapY++;
		m_OffY -= CELLHEIGHT;
	}
	
	if (m_MapX < 0)
	{
		m_RegionIndex = LEFTREGIONIDX;
		m_MapX += REGIONWIDTH;
	}
	else if (m_MapX >= REGIONWIDTH)
	{
		m_RegionIndex = RIGHTREGIONIDX;
		m_MapX -= REGIONWIDTH;
	}

	if (m_RegionIndex >= 0)
	{
		if (m_MapY < 0)
		{
			m_RegionIndex = UPREGIONIDX;
			m_MapY += REGIONHEIGHT;
		}
		else if (m_MapY >= REGIONHEIGHT)
		{
			m_RegionIndex = DOWNREGIONIDX;
			m_MapY -= REGIONHEIGHT;
		}

		if (!m_bClientOnly && m_RegionIndex >= 0)
			CURREGION.AddRef(m_MapX,m_MapY,obj_npc);
			//CURREGION.AddNpcRef(m_MapX,m_MapY);
	}

	if (m_RegionIndex == -1)	// 不可能移动到-1 Region，如果出现这种情况，恢复原坐标
	{
		m_RegionIndex = nOldRegion;
		m_MapX = nOldMapX;
		m_MapY = nOldMapY;
		m_OffX = nOldOffX;
		m_OffY = nOldOffY;
		//CURREGION.AddNpcRef(m_MapX,m_MapY);
		CURREGION.AddRef(m_MapX,m_MapY,obj_npc);
		return;
	}

	if (nOldRegion != m_RegionIndex)
	{
		SubWorld[m_SubWorldIndex].NpcChangeRegion(nOldRegion, m_RegionIndex, m_Index);
		if (IsPlayer())
		{//转换地图时
			SubWorld[m_SubWorldIndex].PlayerChangeRegion(nOldRegion,m_RegionIndex,m_nPlayerIdx);

			if (m_nPlayerIdx > 0)
				Player[m_nPlayerIdx].m_ItemList.Abrade(enumAbradeMove);  //移动磨损
		}

	}
    // These movement paths commit coordinates without calling MovePos.
    if (nOldRegion != m_RegionIndex || nOldMapX != m_MapX || nOldMapY != m_MapY ||
        nOldOffX != m_OffX || nOldOffY != m_OffY)
        JX3D_M1_DEV_AOI_EMIT_MOVE(m_Index);
}

void KNpc::ServeJump(int nSpeed)
{
	//_ASSERT(m_RegionIndex >= 0);
	if (m_RegionIndex < 0)
		return;

	if (!(m_Doing == do_jump || m_Doing == do_jumpattack))
		return;
	
	if (nSpeed <= 0)
		return;

	if (nSpeed >= SubWorld[m_SubWorldIndex].m_nCellWidth)
	{
		nSpeed = SubWorld[m_SubWorldIndex].m_nCellWidth - 1;
	}

	m_OffX += g_DirCos(m_JumpDir, 64) * nSpeed;
	m_OffY += g_DirSin(m_JumpDir, 64) * nSpeed;

	// s = vt - a * t * t / 2
	m_Height = (m_JumpFirstSpeed * m_Frames.nCurrentFrame - ACCELERATION_OF_GRAVITY * m_Frames.nCurrentFrame * m_Frames.nCurrentFrame / 2) / 8;
	if (m_Height < 0)
		m_Height = 0;

	int nOldRegion = m_RegionIndex;
	int nOldMapX = m_MapX;
	int nOldMapY = m_MapY;
	int nOldOffX = m_OffX;
	int nOldOffY = m_OffY;
	//CURREGION.DecNpcRef(m_MapX, m_MapY);
    CURREGION.DecRef(m_MapX, m_MapY,obj_npc);
	
	if (m_OffX < 0)
	{
		m_MapX--;
		m_OffX += CELLWIDTH;
	}
	else if (m_OffX > CELLWIDTH)
	{
		m_MapX++;
		m_OffX -= CELLWIDTH;
	}
	
	if (m_OffY < 0)
	{
		m_MapY--;
		m_OffY += CELLHEIGHT;
	}
	else if (m_OffY > CELLHEIGHT)
	{
		m_MapY++;
		m_OffY -= CELLHEIGHT;
	}
	
	if (m_MapX < 0)
	{
		m_RegionIndex = LEFTREGIONIDX;
		m_MapX += REGIONWIDTH;
	}
	else if (m_MapX >= REGIONWIDTH)
	{
		m_RegionIndex = RIGHTREGIONIDX;
		m_MapX -= REGIONWIDTH;
	}
	
	if (m_RegionIndex >= 0)
	{
		if (m_MapY < 0)
		{
			m_RegionIndex = UPREGIONIDX;
			m_MapY += REGIONHEIGHT;
		}
		else if (m_MapY >= REGIONHEIGHT)
		{
			m_RegionIndex = DOWNREGIONIDX;
			m_MapY -= REGIONHEIGHT;
		}
		if (m_RegionIndex >= 0)
			CURREGION.AddRef(m_MapX, m_MapY,obj_npc);
			//CURREGION.AddNpcRef(m_MapX, m_MapY);
	}

	if (m_RegionIndex == -1)	// 不可能移动到-1 Region，如果出现这种情况，恢复原坐标
	{
		m_RegionIndex = nOldRegion;
		m_MapX = nOldMapX;
		m_MapY = nOldMapY;
		m_OffX = nOldOffX;
		m_OffY = nOldOffY;
		CURREGION.AddRef(m_MapX, m_MapY,obj_npc);
		//CURREGION.AddNpcRef(m_MapX, m_MapY);
		return;
	}

	if (nOldRegion != m_RegionIndex)
	{
#ifdef _SERVER
		SubWorld[m_SubWorldIndex].NpcChangeRegion(nOldRegion, m_RegionIndex, m_Index);
		if (IsPlayer())
		{
			SubWorld[m_SubWorldIndex].PlayerChangeRegion(nOldRegion, m_RegionIndex, m_nPlayerIdx);
			if (m_nPlayerIdx > 0)
			{//跳跃磨损
				Player[m_nPlayerIdx].m_ItemList.Abrade(enumAbradeMove);//移动磨损
			}
		}
#else
		if (m_RegionIndex >= 0)
		{
			SubWorld[0].NpcChangeRegion(SubWorld[0].m_Region[nOldRegion].m_RegionID, SubWorld[0].m_Region[m_RegionIndex].m_RegionID, m_Index);
			m_dwRegionID = SubWorld[0].m_Region[m_RegionIndex].m_RegionID;
		}
#endif
	}		
    // These movement paths commit coordinates without calling MovePos.
    if (nOldRegion != m_RegionIndex || nOldMapX != m_MapX || nOldMapY != m_MapY ||
        nOldOffX != m_OffX || nOldOffY != m_OffY)
        JX3D_M1_DEV_AOI_EMIT_MOVE(m_Index);
}

void KNpc::SendSerCommand(NPCCMD cmd,int x,int y, int z)
{

	if (m_randmove.nTime > 0)
	{
		if (cmd == do_run || 
			cmd == do_runattack || 
			cmd == do_jump || 
			cmd == do_jumpattack || 
			cmd == do_skill || 
			cmd == do_magic)
		{
			return;
		}
	}

	m_Command.CmdKind = cmd;
	m_Command.Param_X = x;
	m_Command.Param_Y = y;
	m_Command.Param_Z = z;
}

BOOL KNpc::NewPath(int nMpsX, int nMpsY)
{
	/*m_DesX = nMpsX;
	m_DesY = nMpsY;
	return TRUE;*/
	//m_cMoveQueue.PushMovePos(nMpsX, nMpsY);
	m_DesX = nMpsX;//moi them vao lai
	m_DesY = nMpsY;
	return TRUE;
}

BOOL KNpc::NewJump(int nMpsX, int nMpsY)
{
	_ASSERT(m_CurrentJumpSpeed > 0);
	if (m_CurrentJumpSpeed <= 0)
		return FALSE;
	
	int nX, nY,nMap;
	    GetMpsPos(&nX, &nY,&nMap);
														
		if (nX == nMpsX && nY == nMpsY)	//如果NPC坐标与 目标相同就 不计算了
		return FALSE;

	int nDir = g_GetDirIndex(nX, nY, nMpsX, nMpsY);	 //获取两个点之间的方向
	int	nMaxLength = m_CurrentJumpSpeed * m_CurrentJumpFrame;
	int	nWantLength = g_GetDistance(nX, nY, nMpsX, nMpsY); //两个点的距离

	if (m_Dir >= MaxMissleDir)
		m_Dir -= MaxMissleDir;

	int	nSin = g_DirSin(nDir, MaxMissleDir);
	int	nCos = g_DirCos(nDir, MaxMissleDir);

	if (nWantLength > nMaxLength)
	{
		m_DesX = nX + ((nMaxLength * nCos) >> 10);
		m_DesY = nY + ((nMaxLength * nSin) >> 10);
		nWantLength = nMaxLength;
	}
	else if (nWantLength <= MIN_JUMP_RANGE)
	{
		m_DesX = nMpsX;
		m_DesY = nMpsY;
		return FALSE;
	}
	
	m_JumpStep = nWantLength / m_CurrentJumpSpeed;	  //要挑多少步

	int nTestX = 0;
	int nTestY = 0;
	int nSuccessStep = 0;

	for (int i = 1; i < m_JumpStep + 1; ++i)
	{
		nTestX = nX + ((m_CurrentJumpSpeed * nCos * i) >> 10);
		nTestY = nY + ((m_CurrentJumpSpeed * nSin * i) >> 10);
		int nBarrier = SubWorld[m_SubWorldIndex].GetBarrier(nTestX, nTestY); //这个点是否有障碍
		if (Obstacle_NULL == nBarrier)
		{
			nSuccessStep = i;
		}
		if (Obstacle_Normal == nBarrier || Obstacle_Fly == nBarrier)
		{
			if (nSuccessStep <= MIN_JUMP_RANGE / m_CurrentJumpSpeed)
			{
				return FALSE;
			}
			m_DesX = nX + ((m_CurrentJumpSpeed * nCos * nSuccessStep) >> 10);
			m_DesY = nY + ((m_CurrentJumpSpeed * nSin * nSuccessStep) >> 10);
			m_JumpStep = nSuccessStep;
			break;
		}

		int	nTrap = SubWorld[m_SubWorldIndex].GetTrap(nTestX, nTestY);

		if (nTrap)
		{//有Trap
			if (i <= MIN_JUMP_RANGE / m_CurrentJumpSpeed)
			{
				return FALSE;
			}
			m_DesX = nX + ((m_CurrentJumpSpeed * nCos * i) >> 10);
			m_DesY = nY + ((m_CurrentJumpSpeed * nSin * i) >> 10);
			m_JumpStep = i;
			break;
		}
	}
	m_JumpDir = nDir;
	return TRUE;
}

/*void KNpc::SelfDamage(int nDamage)
{
	m_CurrentLife -= nDamage;

	if (m_CurrentLife<=0)
	{
		m_CurrentLife = 1;
	}
} */
//提示内力不足 生命不足 体力不足设置
BOOL KNpc::Cost(NPCATTRIB nType, int nCost, BOOL bOnlyCheckCanCast)
{
	if (!IsPlayer())
		return TRUE;

	bool _IsNoCost= false;
	if  (m_SubWorldIndex>=0 && m_SubWorldIndex<MAX_SUBWORLD)
		_IsNoCost= SubWorld[m_SubWorldIndex]._IsNoCost;

	if (_IsNoCost)  //不消耗
		return TRUE;
	
	__int64 *pSource = NULL;

	switch(nType)
	{
	case attrib_mana:
		pSource = &m_CurrentMana;
		break;
	case attrib_life:
		pSource = &m_CurrentLife;
		break;
	case attrib_stamina:
		pSource = &m_CurrentStamina;
		break;
	default:
		break;
	}

	if (pSource)
	{
		if (*pSource - nCost<=0)
		{
#ifndef _SERVER
			KSystemMessage Msg;

			Msg.byConfirmType = SMCT_NONE;
			Msg.byParamSize = 0;
			Msg.byPriority = 1;
			Msg.eType = SMT_NORMAL;

			switch(nType)
			{
			case attrib_mana:
				g_StrCpyLen(Msg.szMessage, MSG_NPC_NO_MANA, sizeof(Msg.szMessage));
				break;
			case attrib_life:
				g_StrCpyLen(Msg.szMessage, MSG_NPC_NO_LIFE, sizeof(Msg.szMessage));
				break;
			case attrib_stamina:
				g_StrCpyLen(Msg.szMessage, MSG_NPC_NO_STAMINA, sizeof(Msg.szMessage));
				break;
			default:
				break;
			}
			Msg.nMsgLen = TEncodeText(Msg.szMessage, strlen(Msg.szMessage));
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&Msg, NULL);
#endif
			return FALSE;
		}
		else
		{
			if (!bOnlyCheckCanCast)
				*pSource -= nCost;
			return TRUE;
		}
	}
	return FALSE;
}

void KNpc::DoJump()
{
	if  (m_SubWorldIndex<0 || m_SubWorldIndex>=MAX_SUBWORLD ||m_RegionIndex<0) 
		return;
	if  ( m_RegionIndex >=SubWorld[m_SubWorldIndex].m_nTotalRegion)
		return;
	if (m_Doing == do_jump)
		return;
	
	m_Doing = do_jump;
	m_Dir = m_JumpDir;
	m_ProcessAI	= 0;
	m_JumpFirstSpeed = ACCELERATION_OF_GRAVITY * (m_JumpStep - 1) / 2 ;
#ifdef _SERVER	// 向周围9个Region广播发技能
	NPC_JUMP_SYNC	NetCommand;
	NetCommand.ProtocolType = (BYTE)s2c_npcjump;
	NetCommand.ID = m_dwID;
	NetCommand.nMpsX = m_DesX;
	NetCommand.nMpsY = m_DesY;
		
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
	CURREGION.BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, m_MapX, m_MapY);
	int i,nConRegion=-1;
	for (i = 0; i < 8; ++i)
	{
		nConRegion = CONREGIONIDX(i);
		if  (nConRegion<0 || nConRegion>=SubWorld[m_SubWorldIndex].m_nTotalRegion)
			continue;
		CONREGION(i).BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, m_MapX - POff[i].x, m_MapY - POff[i].y);
	}
#endif

#ifndef _SERVER
		m_ClientDoing = cdo_jump;
#endif
	m_Frames.nTotalFrame = m_JumpStep;
	m_Frames.nCurrentFrame = 0;	
}

BOOL KNpc::OnJump()
{
	ServeJump(m_CurrentJumpSpeed);
	if (WaitForFrame())
	{
		DoStand();
		m_ProcessAI	= 1;
		return FALSE;
	}
	return TRUE;
}

BOOL KNpc::WaitForFrame()
{
	m_Frames.nCurrentFrame ++;
	if (m_Frames.nCurrentFrame < m_Frames.nTotalFrame)
		return FALSE;
	m_Frames.nCurrentFrame = 0;
	return TRUE;
}

BOOL KNpc::IsReachFrame(int nPercent)
{
	if (m_Frames.nCurrentFrame == m_Frames.nTotalFrame * nPercent / 100)
	{
		return TRUE;
	}
	return FALSE;
}
//刷怪
//客户端从网络得到的NpcSettingIdx是包含高16位Npc的模板号与低16位为等级
void KNpc::Load(int nNpcSettingIdx, int nLevel,int nSubWorld,int nBoss)
{
	m_PathFinder.Init(m_Index);

	if (nLevel <= 0) 
	{
		nLevel = 1;
	}

	//特殊NPC 玩家
	if (nNpcSettingIdx == PLAYER_MALE_NPCTEMPLATEID || nNpcSettingIdx == PLAYER_FEMALE_NPCTEMPLATEID)
	{
		m_NpcSettingIdx = nNpcSettingIdx;
		m_Level = nLevel;
#ifndef _SERVER
		if (nNpcSettingIdx == PLAYER_MALE_NPCTEMPLATEID)
		{
			strcpy(szNpcTypeName, "男主角");
			m_StandFrame = NpcSet.GetPlayerStandFrame(TRUE);
			m_WalkFrame = NpcSet.GetPlayerWalkFrame(TRUE);
			m_RunFrame = NpcSet.GetPlayerRunFrame(TRUE);
		}
		else
		{
			strcpy(szNpcTypeName, "女主角");
			m_StandFrame = NpcSet.GetPlayerStandFrame(FALSE);
			m_WalkFrame = NpcSet.GetPlayerWalkFrame(FALSE);
			m_RunFrame = NpcSet.GetPlayerRunFrame(FALSE);
		}
#endif
		//		TODO: Load Player Data;
		m_WalkSpeed   = NpcSet.GetPlayerWalkSpeed();
		m_RunSpeed    = NpcSet.GetPlayerRunSpeed();
		m_AttackFrame = NpcSet.GetPlayerAttackFrame();
		m_HurtFrame	  = NpcSet.GetPlayerHurtFrame();
	}
	else
	{ //普通npc 怪物
		/*if (SubWorld[nSubWorld].m_SubWorldID==42)
		{
			printf("--地图%d,GetNpcCopyFromTemplate(%d,%d):---\n",SubWorld[nSubWorld].m_SubWorldID,nNpcSettingIdx,nLevel);
		}*/
		GetNpcCopyFromTemplate(nNpcSettingIdx, nLevel);  //拷贝 NPCs.TXT 里面的基本信息！！

#ifndef _SERVER	
		g_NpcSetting.GetString(nNpcSettingIdx + 2, "NpcResType", "enemy003", szNpcTypeName, sizeof(szNpcTypeName));
		if (!szNpcTypeName[0])
		{ //人物类型表 npcres
			g_NpcKindFile.GetString(2, "人物名称", "enemy003", szNpcTypeName, sizeof(szNpcTypeName));//如果没找到，用第一个npc代替
		}
		g_NpcSetting.GetInteger(nNpcSettingIdx + 2, "AIMode", 12, &m_AiMode);
		g_NpcSetting.GetInteger(nNpcSettingIdx + 2, "AIParam1", 12, &m_AiParam[0]);
		g_NpcSetting.GetInteger(nNpcSettingIdx + 2, "AIParam2", 12, &m_AiParam[1]);
		g_NpcSetting.GetInteger(nNpcSettingIdx + 2, "AIParam3", 12, &m_AiParam[2]);
		g_NpcSetting.GetInteger(nNpcSettingIdx + 2, "AIParam4", 12, &m_AiParam[3]);
		g_NpcSetting.GetInteger(nNpcSettingIdx + 2, "AIParam5", 12, &m_AiParam[4]);
		g_NpcSetting.GetInteger(nNpcSettingIdx + 2, "AIParam6", 12, &m_AiParam[5]);
		g_NpcSetting.GetInteger(nNpcSettingIdx + 2, "AIParam7", 12, &m_AiParam[6]);
		g_NpcSetting.GetInteger(nNpcSettingIdx + 2, "AIParam8", 12, &m_AiParam[7]);
		g_NpcSetting.GetInteger(nNpcSettingIdx + 2, "AIParam9", 12, &m_AiParam[8]);

		g_NpcSetting.GetInteger(nNpcSettingIdx + 2, "ActiveRadius", 12, &m_ActiveRadius); //活动范围
		g_NpcSetting.GetInteger(nNpcSettingIdx + 2, "ClientOnly", 0, &m_bClientOnly);
		int m_nIsRevive;
		g_NpcSetting.GetInteger(nNpcSettingIdx + 2, "IsRevive", 0, &m_nIsRevive);
	    m_IsRevive = (DWORD)m_nIsRevive;
        char nMapNpc[32]={0};
		int  nIsCreat=0;
		ZeroMemory(nMapNpc,sizeof(nMapNpc));
		sprintf(nMapNpc,"%d_IsScript",SubWorld[nSubWorld].m_SubWorldID);
		g_NpcMapDropRate.GetInteger("List",nMapNpc,0,&nIsCreat);
		IsExeGoldScript = nIsCreat; 
		sprintf(nMapNpc,"%d_IsCreat",SubWorld[nSubWorld].m_SubWorldID);
		g_NpcMapDropRate.GetInteger("List",nMapNpc,0,&nIsCreat);//默认执行全局脚本
	    IsCreatBoss     = nIsCreat; 
		// 飞行类，11，12，17，用AiParam[6]保存策划设定高度
		// add by flying
		if (m_AiMode == 11 || m_AiMode == 12 || m_AiMode == 17)
			m_AiParam[6] = m_AiMode;
#endif
	}

#ifndef _SERVER  //普通NPC 和特殊NPC 通用
	m_DataRes.Init(szNpcTypeName, &g_NpcResList);  //外观初始化
	m_DataRes.SetAction(m_ClientDoing);  //设定外观行为对接
	m_DataRes.SetRideHorse(m_bRideHorse); //设定是否骑马接帧数
	m_DataRes.SetArmor(m_ArmorType); //衣服
	m_DataRes.SetHelm(m_HelmType); //头部
	m_DataRes.SetHorse(m_HorseType); //马位置
	m_DataRes.SetWeapon(m_WeaponType); //武器
  if (m_Kind==kind_player)
  { 
	  //if (m_PifengType>0)
         m_DataRes.SetPifeng(m_PifengType); //披风
	  //if (m_ChiBangType>0)
         m_DataRes.SetChiBang(m_ChiBangType);//翅膀
  }
#endif
	m_CurrentCamp = m_Camp;
#ifdef _SERVER
	//出现蓝装 率怪的概率
	if(m_Kind==kind_normal)
	{//是动物
		m_cGold.RandChangeGold(1,nBoss,nSubWorld);  //有一定概率刷出 蓝怪以上的怪物  0为 取消产生蓝怪

		/*if (CheckMaps("IsThisMap",SubWorld[nSubWorld].m_SubWorldID))
		{//执行地图全局脚本,设置重生NPC方案
			//M_SubWorldSet->SubExecuteScript("\\script\\Global\\goldrevive.lua","ReviveMain",Name,"",SubWorld[nSubWorld].m_SubWorldID,m_NpcSettingIdx,m_Index);
		      NpcExecuteScriptA("\\script\\global\\goldrevive.lua","ReviveMain",Name,Name,SubWorld[nSubWorld].m_SubWorldID,m_NpcSettingIdx,m_Index);
		}*/ 
	}
#endif
}

void KNpc::GetMpsPos(mapPos *pos)
{
	if (!pos) return;

	if (m_SubWorldIndex<0 || m_SubWorldIndex>=MAX_SUBWORLD)
	{
		pos->nMap=m_SubWorldIndex;
		pos->nMpsX=0;
		pos->nMpsY=0;
		return;
	}

	INT X=0,Y=0;
	SubWorld[m_SubWorldIndex].NewMap2Mps(m_RegionIndex,m_MapX,m_MapY,m_OffX,m_OffY,&X,&Y); //格子转像素
	pos->nMap=m_SubWorldIndex;
	pos->nMpsX = X;
	pos->nMpsY = Y;
	return;
}

void KNpc::GetMpsPos(int *pPosX, int *pPosY,int *nMapid)
{
	if (m_SubWorldIndex<0 || m_SubWorldIndex>=MAX_SUBWORLD)
	{
		*nMapid=m_SubWorldIndex;
		return;
	}
#ifdef _SERVER
	SubWorld[m_SubWorldIndex].NewMap2Mps(m_RegionIndex,m_MapX,m_MapY,m_OffX,m_OffY,pPosX,pPosY); //格子转像素
	*nMapid=m_SubWorldIndex;
#else
	SubWorld[m_SubWorldIndex].NewMap2Mps(m_RegionIndex,m_MapX,m_MapY,m_OffX,m_OffY,pPosX,pPosY);
    *nMapid=m_SubWorldIndex;
//	KSubWorld::Map2Mps(C_REGION_X(m_RegionIndex), C_REGION_Y(m_RegionIndex), m_MapX, m_MapY, m_OffX, m_OffY, pPosX, pPosY);
//	int nRegion, nMapX, nMapY, nOffX, nOffY;
//	SubWorld[m_SubWorldIndex].Mps2Map(nX, nY, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY); //像素坐标 转化格子坐标
#endif
}

//改变攻击距离
BOOL	KNpc::ChangeSkillAttackRadius(int nSkillIdx,int nDis)
{
	if (nSkillIdx <= 0 || nSkillIdx >= MAX_NPCSKILL)
		return FALSE;

	if (!IsPlayer())
		return FALSE;

	if (!(m_SkillList.m_Skills[nSkillIdx].SkillId && m_SkillList.m_Skills[nSkillIdx].CurrentSkillLevel))
		return FALSE;
	int nLevel = m_SkillList.m_Skills[nSkillIdx].SkillLevel;

	int nChanSkillID= m_SkillList.m_Skills[nSkillIdx].SkillId;

	if  (nChanSkillID >=MAX_SKILL && nLevel >=MAX_SKILLLEVEL && nLevel <=0)
		return FALSE;

	ISkill * pISkill =  g_SkillManager.GetSkill(nChanSkillID, nLevel);

	if (pISkill)
	{
		m_CurrentAttackRadius = pISkill->ChangeAttackRadius(nDis); //得到技能的攻击范围
#ifndef _SERVER//客户端发送协议同步给服务器
		SendClientRunScript(1,nSkillIdx,nDis,nChanSkillID);
#endif
	}
	return TRUE;
}

//设置为激活技能
BOOL	KNpc::SetActiveSkill(int nSkillIdx)
{
	if (nSkillIdx <= 0 || nSkillIdx >= MAX_NPCSKILL)
		return FALSE;

	if (!(m_SkillList.m_Skills[nSkillIdx].SkillId && m_SkillList.m_Skills[nSkillIdx].CurrentSkillLevel))
		return FALSE;

	m_ActiveSkillID = m_SkillList.m_Skills[nSkillIdx].SkillId;
	m_ActiveSkListIndex =nSkillIdx; 
	int nLevel = m_SkillList.m_Skills[nSkillIdx].SkillLevel;
	//_ASSERT(m_ActiveSkillID < MAX_SKILL && nLevel < MAX_SKILLLEVEL && nLevel > 0);
	if  (m_ActiveSkillID >=MAX_SKILL && nLevel >=MAX_SKILLLEVEL && nLevel <=0)
		return FALSE;

	ISkill * pISkill =  g_SkillManager.GetSkill(m_ActiveSkillID, nLevel);

	if (pISkill)
    {
		m_CurrentAttackRadius = pISkill->GetAttackRadius(); //得到技能的攻击范围
    }
	return TRUE;
}
//获取第几个技能的ID
int	KNpc::GetSkillID(int nSkillIdx)
{
	if (nSkillIdx <= 0 || nSkillIdx >= MAX_NPCSKILL)
		return FALSE;
	
	if (!(m_SkillList.m_Skills[nSkillIdx].SkillId && m_SkillList.m_Skills[nSkillIdx].CurrentSkillLevel))
		return FALSE;
	return m_SkillList.m_Skills[nSkillIdx].SkillId;
}

// 同步客户端属性状态技能等数据
void KNpc::UpdataNpcCurData(int nWonerIndex,int nEquipPlace) //更新自身的属性参数
{

/*	for (int j = 0 ; j< MAX_NPCSKILL;j++)
	{
		int nAdd = 	Npc[m_nIndex].m_SkillList.GetAddPoint(j);
		if (nAdd > 0)
		{
			nAdd = -nAdd;
#ifdef _SERVER	
			Npc[m_nIndex].m_SkillList.IncreaseLevel(j,nAdd); //更新清除技能被动状态
#endif
			Npc[m_nIndex].m_SkillList.QeuipAddPoint(j,nAdd);
		}

		Npc[m_nIndex].m_SkillList.m_Skills[j].mAddPoint = 0;
		Npc[m_nIndex].m_SkillList.m_Skills[j].EnChance = 0;  // 技能的加成
	}*/

	ZeroMemory(&m_CurrentPhysicsMagicDamageP, sizeof(KMagicAttrib));
	ZeroMemory(&m_CurrentPhysicsMagicDamageV, sizeof(KMagicAttrib));

/*	int nFomoidx=Npc[m_nIndex].GetCurFuMoIdx(),nCurbei=0;
	
	if 	(nFomoidx>-1)
	{
		if (Npc[m_nIndex].GetCurFoMoSkllLevel(nFomoidx)>=5) //获取神将当前的等级
		{
			nCurbei=Npc[m_nIndex].GetCurFoMoSkllLevel(nFomoidx)/5;
		}
	}*/

	m_CurrentLifeMax		= m_LifeMax;
	m_CurrentManaMax		= m_ManaMax;
	m_CurrentStaminaMax	= m_StaminaMax;
	m_CurrentAttackRating	= m_AttackRating;
	m_CurrentAttackSpeed	= m_AttackSpeed;
	m_CurrentCastSpeed	= m_CastSpeed;
    m_CurrentRunSpeed	    = m_RunSpeed; 
	m_CurrentJumpSpeed	= m_JumpSpeed;
	m_CurrentVisionRadius	= m_VisionRadius;
	m_CurrentWalkSpeed	= m_WalkSpeed;
	m_CurrentNuQiMax		= 0;//怒气
	m_CurrentDefend		= m_Defend;
	m_CurrentAddDexterity = 0;
	m_CurrentLifeReplenish= m_LifeReplenish;
	m_CurrentHitRecover	= m_HitRecover;  //Npc的受伤动作时间
	m_CurrentManaReplenish= m_ManaReplenish;

	m_TempFireResist = 0;	        // Npc的当前火抗性
	m_TempColdResist = 0;	            // Npc的当前冰抗性
	m_TempPoisonResist = 0;	        // Npc的当前毒抗性
	m_TempLightResist = 0;	        // Npc的当前电抗性
	m_TempPhysicsResist = 0;	    // Npc的当前物理抗性

	m_CurrentLightResist	= BASE_NPC_RESIST_MAX; //m_LightResist;
	m_CurrentPhysicsResist  = BASE_NPC_RESIST_MAX; //m_PhysicsResist;
	m_CurrentPoisonResist	= BASE_NPC_RESIST_MAX; //m_PoisonResist;
	m_CurrentColdResist	    = BASE_NPC_RESIST_MAX; //m_ColdResist;
	m_CurrentFireResist	    = BASE_NPC_RESIST_MAX; //m_FireResist;
	m_nCurNpcLucky          = BASE_NPC_RESIST_MAX;

	m_CurrentStaminaGain	= m_StaminaGain;
	m_CurrentStaminaLoss	= m_StaminaLoss;
	//SetBaseSpeedAndRadius();
	ZeroMemory(&m_CurrentMagicColdDamage, sizeof(KMagicAttrib));
	ZeroMemory(&m_CurrentColdDamage, sizeof(KMagicAttrib));
	m_CurrentColdEnhance	= 0;

	m_CurrentColdResistMax	= MAX_RESIST;//m_ColdResistMax;
	m_CurrentFireResistMax  = MAX_RESIST;
	m_CurrentLightResistMax = MAX_RESIST;//m_LightResistMax;
	m_CurrentPhysicsResistMax = MAX_RESIST;//m_PhysicsResistMax;
	m_CurrentPoisonResistMax  = MAX_RESIST;//m_PoisonResistMax;

	m_CurrentDamage2Mana	= 0;
	m_CurrentDamageReduce	= 0;
	m_CurrentDeadlyStrike	= 0;
//	m_CurrentElementDamageReduce = 0;
	ZeroMemory(&m_CurrentMagicFireDamage, sizeof(KMagicAttrib));
	ZeroMemory(&m_CurrentFireDamage, sizeof(KMagicAttrib));
	m_CurrentFireEnhance	= 0;
	
	m_CurrentHandEnhance	= 0;
	m_CurrentKnockBack	= 0;
	m_CurrentLifeStolen	= 0;
	ZeroMemory(&m_CurrentMagicLightDamage, sizeof(KMagicAttrib));
	ZeroMemory(&m_CurrentLightDamage, sizeof(KMagicAttrib));
//	ZeroMemory(&m_WaiPhysicsDamage, sizeof(KMagicAttrib));
	m_CurrentLightEnhance	= 0;
	m_CurrentPoisonTime=0;

//	m_CurrentManaPerEnemy	= 0;
	m_CurrentManaStolen	= 0;
	m_CurrentMeleeDmgRet	= 0;        // 近程反弹
	m_CurrentHulueMeleeDmgRet=0;      // 忽略近程反弹多少点、
   	m_CurrentHulueRangeDmgRet=0;      // 忽略远程程反弹多少点、
	m_CurrentMeleeDmgRetPercent = 0;  // 近程反弹百分比
	m_CurrentTempSpeed=0;
	ZeroMemory(&m_CurrentMeleeEnhance, sizeof(m_CurrentMeleeEnhance));

//	m_CurrentPiercePercent	= 0;
	ZeroMemory(&m_CurrentMagicPoisonDamage, sizeof(KMagicAttrib));
	ZeroMemory(&m_CurrentPoisonDamage, sizeof(KMagicAttrib));
	m_CurrentPoisonEnhance	= 0;

	m_CurrentRangeDmgRet	= 0;         //远程反弹
	m_CurrentRangeDmgRetPercent	= 0; //远程反弹百分比
	m_CurrentRangeEnhance	= 0;
	m_CurrentSlowMissle	= 0;         //子弹减速
	m_CurrentStaminaStolen	= 0;
	m_CurrentAddPhysicsDamage = 0;      //外功普点
	m_CurrentUpExp        = 0;
	m_CurrentFreezeTimeReducePercent = 0;
	m_CurrentPoisonTimeReducePercent = 0;
	m_EnemyPoisonTimeReducePercent   = 0;
	m_CurrentStunTimeReducePercent = 0;
	m_EnemyStunTimeReducePercent   = 0;
	m_CurrentSerisesEnhance = 0;
	m_CurrentSeriesAbate   = 0;
	m_CurrentDamageReduce = 0;
	m_CurrentdanggeRate = 0;  //档格
	m_CurrentzhongjiRate = 0; //重击
	 m_CurrentcjdanggeRate	= 0;               //拆解档格
	 m_CurrentcjzhongjiRate	= 0;               //拆解重击
	 m_Currentsorbdamage	= 0;                   //抵消伤害
	 m_Currentsorbdamage_v=0;
	 m_Currenadddamagev   = 0;
	 m_Currenadddamagep   = 0;
	 m_Currentpoisonres	= 0;                   //=忽略对方毒防:#d1-%
	 m_Currentfireres	= 0;                       //=忽略对方火防:#d1-%
	 m_Currentlightingres	= 0;                   //=忽略对方雷防:#d1-%
	 m_Currentphysicsres	= 0;                   //=忽略对方普防:#d1-%
	 m_Currentcoldres	= 0;                       //=忽略对方冰防:#d1-%
	 m_Currentallres	= 0; 
	 m_Currentnopkvalue	= 0;                   //=不增加PK值概率:#d1+%
	 m_Currentbossdamage	= 0;                   //=对黄金boss攻击伤害<color=orange>#d1+%<color>
	 m_Currentelementsenhance	= 0;               //=五行强化值：#d1-点。强化对相克五行的克制效果
     m_Currentelementsresist	= 0;               //=五行弱化值：#d1-点。弱化受相克五行的克制效果
	 m_Currentskillenhance=0;                    //技能的加成
//	  ZeroMemory(m_CurrentSkillEnhance,sizeof(m_CurrentSkillEnhance));
	/* _EnhanceInfo::iterator it;
	 for( it = nEnhanceInfo.begin(); it != nEnhanceInfo.end(); ++it)
	 {
		 it->second.nSkillIdx = 0;
		 it->second.nEnhance = 0;
	 }*/
	 nEnhanceInfo.clear();

	 m_CurrentFullManaskillenhance=0;
	 m_TempFullManaskillenhance = 0;
	 m_CurrentautoReviverate=0;                  //复活概率


	 m_CurrentAddmagicphysicsDamageP= 0;// 当前被动内普百分比
	 m_CurrentPoisondamagereturnV=0;
	 m_PoisonDamageReturn=0;
	 m_Currentbaopoisondmax_p=0;
	 m_ReturnSkill=0;
	 m_IgnoreSkill=0;
	 m_CurrentReturnResist=0;
	 m_CurrentCreatnpcv=0;
	 m_CurrentAllJiHuo=0; //是否全身激活
	 m_CurrentCreatStatus=0;
	 m_CurrentAttackRatingEnhancep=0;
	 m_CurrentAttackRatingEnhancev=0;
	 m_CurrentIgnorenAttacRating  =0;
	 m_Me2metaldamage_p=0;              //=对金系伤害增加：#d1+%
	 m_Metal2medamage_p=0;              //=减少来自金系的伤害：#d1-%
	 m_Me2wooddamage_p=0;              //=对木系伤害增加：#d1+%
	 m_Wood2medamage_p=0;              //=减少来自木系的伤害：#d1-%
	 m_Me2waterdamage_p=0;              //=对水系伤害增加：#d1+%
	 m_Water2medamage_p=0;              //=减少来自水系的伤害：#d1-%
	 m_Me2firedamage_p=0;              //=对火系伤害增加：#d1+%
	 m_Fire2medamage_p=0;              //=减少来自火系的伤害：#d1-%
	 m_Me2earthdamage_p=0;              //=对土系伤害增加：#d1+%
	 m_Earth2medamage_p=0;              //=减少来自土系的伤害：#d1-%
     m_CurrentStunRank_p=0;
	 m_Staticmagicshield_p=0;
	 m_CurrentPiercePercent	= 0;
	 ReCalcNpcEquip(nWonerIndex,nEquipPlace); //重新计算身上的装备
	 //ReCalcState(nIsLoign); //重新计算技能的状态
}

void KNpc::ReCalcNpcEquip(int nWonerIndex,int nEquipPlace)
{
	
  if (nWonerIndex<=0)	//如果主人不存在的话
		return;

  if (nEquipPlace==-1)
  {
	  int nNsta,nNstb,nNstc,nNstd,nNste;
	      g_GameSetTing.GetInteger("FuMoNpcSet","Mo_0",0,&nNsta);
	      g_GameSetTing.GetInteger("FuMoNpcSet","Mo_1",0,&nNstb);
	      g_GameSetTing.GetInteger("FuMoNpcSet","Mo_2",0,&nNstc);
	      g_GameSetTing.GetInteger("FuMoNpcSet","Mo_3",0,&nNstd);
	      g_GameSetTing.GetInteger("FuMoNpcSet","Mo_4",0,&nNste);
	  
	  if (m_NpcSettingIdx ==nNsta)
	  {//金
		 nEquipPlace=53;
	  }
	  else if (m_NpcSettingIdx ==nNstb)
	  {//木
		  nEquipPlace=58;
	  }
	  else if (m_NpcSettingIdx ==nNstc)
	  {//水
		  nEquipPlace=63;
	  }
	  else if (m_NpcSettingIdx ==nNstd)
	  {//火
		  nEquipPlace=68;
	  }
	  else if (m_NpcSettingIdx ==nNste)
	  {//土
		  nEquipPlace=73;
	  }
  }
 //金53-57 木58-62 水63-67 火68-72 土73-77
  int i;
  if (53<=nEquipPlace && nEquipPlace<=57)
  {//金
	for (i = 53; i < 58; ++i)
	{
		if (!UpMagicAttrib(i,nWonerIndex))
			  continue;
	}
  }
  else if (58<=nEquipPlace && nEquipPlace<=62)
  {//木
	  for (i = 58; i < 63; ++i)
	  {
		  if (!UpMagicAttrib(i,nWonerIndex))
			  continue;
	  }
  }
  else if (63<=nEquipPlace && nEquipPlace<=67)
  {//水
	  for (i = 63; i < 68; ++i)
	  {
		  if (!UpMagicAttrib(i,nWonerIndex))
			  continue;
	  }
  }
  else if (68<=nEquipPlace && nEquipPlace<=72)
  {//火
	  for (i = 68; i < 73; ++i)
	  {
		  if (!UpMagicAttrib(i,nWonerIndex))
			  continue;
	  }
  }
  else if (73<=nEquipPlace && nEquipPlace<=77)
  {//土
	  for (i = 73; i < 78; ++i)
	  {
		  if (!UpMagicAttrib(i,nWonerIndex))
			  continue;
	  }
  }

}

BOOL KNpc::UpMagicAttrib(int i,int nWonerIndex)
{
	int nIdx =Player[nWonerIndex].m_ItemList.GetItemBox(i);
	
	if (!nIdx)
	   return FALSE;
	
	int nActive = 3;//m_ItemList.GetEquipEnhance(i);  //获取激活的数量
	
	//除马匹
	//	if (i != itempart_horse || Npc[m_nIndex].m_bRideHorse)  //除掉马的部位 或 骑着马
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
			Item[nIdx].ApplyMagicAttribToNPC(&Npc[m_Index], nActive,nActiveInfo);
		}
		else if (Item[nIdx].GetIsCanUse()==0 && Item[nIdx].GetGenre()==item_equip && Item[nIdx].GetDetailType()==equip_mask) 
		{//面具
			Item[nIdx].ApplyMagicAttribToNPC(&Npc[m_Index], nActive,nActiveInfo);
		}
		
		}
	return TRUE;
}

//设置为光环技能
//vong sang skill
void KNpc::SetAuraSkill(int nSkillID)
{
	int nCurLevel = 0;

	if (nSkillID>g_SkillManager.GetSkillCount())
	{
		m_ActiveAuraID = 0;
		m_ActiveAuraIndex =0;
		return;
	}

	if (nSkillID <= 0 || nSkillID >= MAX_SKILL) 
    {
        nSkillID = 0;
    }
	else
	{
		nCurLevel = m_SkillList.GetCurrentLevel(nSkillID);
		if (nCurLevel <= 0) 
            nSkillID = 0;
		else
		{
			_ASSERT(nSkillID < MAX_SKILL && nCurLevel < MAX_SKILLLEVEL);
			
			KSkill * pOrdinSkill = (KSkill *)g_SkillManager.GetSkill(nSkillID, nCurLevel);
            if (!pOrdinSkill || !pOrdinSkill->IsAura())
				nSkillID  = 0;
		}
	}

	m_ActiveAuraID    = nSkillID;
	m_ActiveAuraIndex = m_SkillList.FindSame(m_ActiveAuraID);
/*#ifdef _SERVER
	if (m_ActiveAuraID)
	{
		int nStateSpecialId = 0;  //状态的ID
		KSkill * pOrdinSkill = (KSkill *) g_SkillManager.GetSkill(nSkillID, nCurLevel);
		
		if (pOrdinSkill)
        {
			nStateSpecialId = pOrdinSkill->GetStateSpecailId();  //获取光环状态的ID号
        }
		
		if (nStateSpecialId) m_btStateInfo[m_nNextStatePos] = nStateSpecialId;
		if ((++m_nNextStatePos) >= MAX_NPC_RECORDER_STATE) 
			m_nNextStatePos = 0;

        UpdateNpcStateInfo();	 //原来没有
	//	BroadCastState();        //同步光环
	}
#endif*/

#ifndef _SERVER
	SKILL_CHANGEAURASKILL_COMMAND ChangeAuraMsg;
	ChangeAuraMsg.ProtocolType = c2s_changeauraskill;
	ChangeAuraMsg.m_nAuraSkill = m_ActiveAuraID;
	if (g_pClient)
		g_pClient->SendPackToServer(&ChangeAuraMsg, sizeof(SKILL_CHANGEAURASKILL_COMMAND));
#else
	UpdateNpcStateInfo();
#endif

}

BOOL KNpc::SetPlayerIdx(int nIdx)
{
	if (nIdx <= 0 || nIdx >= MAX_PLAYER)
		return FALSE;

	if (m_Kind != kind_player)
		return FALSE;

	m_nPlayerIdx = nIdx;
	return TRUE;
}

// -------------------------------------------------------------------------
// 函数		: KNpc::TestMovePos
// 功能		: 测试是否能够直线到达目标点
// 返回值	: BOOL 是否能到达
// 参数		: INT& nMpsX, INT& nMpsY	传入目标点地图坐标；	返回实际可以到达的坐标
// 参数		: INT& nLength				传入允许最大长度；		返回实际长度
// 参数		: INT  nSpeed				传入移动速度
// 参数		: BOOL bCanJumpOver			传入是否可以越过一部分障碍（可能是Npc之类的）
// 作者		: FanZai
// 附注		: 从旧的NewJump()修改而来
// -------------------------------------------------------------------------

BOOL KNpc::TestMovePos(IN OUT INT& nMpsX, IN OUT INT& nMpsY, IN OUT INT& nLength, INT nSpeed, BOOL bCanJumpOver)
{
	if (nMpsX < 0 || nMpsY < 0)
	{
		return FALSE;
	}
	if (nSpeed <= 0 || nSpeed > defLOGIC_CELL_WIDTH)
	{
		//_ASSERT(FALSE);	// 怎么会这样呢？
		return FALSE;
	}
	
	if (nLength <= 0)
	{
		//_ASSERT(FALSE);	// 允许最大长度0，不能移动
		return FALSE;
	}
	
	INT nX, nY,nMap;
	GetMpsPos(&nX, &nY,&nMap);
	
	INT	nTargetX	= nMpsX - nX;
	INT	nTargetY	= nMpsY - nY;	// 目标点偏移坐标
	
	INT	nTargetLength =				// 目标点偏移距离
		(INT)sqrt((FLOAT)(nTargetX * nTargetX + nTargetY * nTargetY));
	
	if (!nTargetLength)				// 距离度是0后面就要出错了
		return FALSE;
	
	INT	nDx	= (nTargetX * nSpeed << 10) / nTargetLength;
	INT	nDy	= (nTargetY * nSpeed << 10) / nTargetLength;	// 每步偏移坐标（放大1024倍）
	
	if (nTargetLength > nLength)
		nTargetLength = nLength;	// 实际跳跃长度
	
	INT	nBarrierTestCounts	= nTargetLength / nSpeed;		// 要测试的步数
	
	INT	nSuccessCounts = 0;			// 成功的步数（只算可以停的点。可以越过、但不可以停的点不算）
	
	INT nTestEX = nX << 10;
	INT nTestEY = nY << 10;			// 当前测试点放大1024倍坐标
	INT nTestX, nTestY;				// 当前测试点地图坐标
	Obstacle_Kind eTestBarrier;		// 当前测试点障碍信息
	
	for (INT i = 1; i <= nBarrierTestCounts; i++)
	{
		nTestX	= (nTestEX += nDx) >> 10;
		nTestY	= (nTestEY += nDy) >> 10;
#ifdef _SERVER
		if (SubWorld[m_SubWorldIndex].GetTrap(nTestX, nTestY))	// 有Trap点就直接停，避免越过Trap点
		{
			nSuccessCounts = i;
			break;
		}
#endif
		eTestBarrier	= (Obstacle_Kind)SubWorld[m_SubWorldIndex].GetBarrier(nTestX, nTestY);
		switch (eTestBarrier)
		{
		case Obstacle_NULL:
			nSuccessCounts = i;
			continue;
		case Obstacle_Normal:
		case Obstacle_Fly:
			break;
		case Obstacle_Jump:
		case Obstacle_JumpFly:
			if (bCanJumpOver)
				continue;
			else
				break;
		case 0xff:
			_ASSERT(FALSE);	// 坐标超出地图范围？！
			break;
		default:
			_ASSERT(FALSE);	// 有新障碍类型了？！
			return FALSE;
		}
		break;
	}
	nLength	= nSuccessCounts * nSpeed;
	nMpsX	= nX + ((nDx * nSuccessCounts) >> 10);
	nMpsY	= nY + ((nDy * nSuccessCounts) >> 10);
	
	return TRUE;
}

int	KNpc::CheckMaps(char *nKey,int nMapIdx)
{
	int nRow = g_ForbitMap.GetHeight()+1,nReg=0;
	
	for (int i=2;i<nRow;++i)
	{
		int nMaps =0;
		g_ForbitMap.GetInteger(i,nKey,0,&nMaps);
		if (nMapIdx==nMaps)
		{
			nReg =1;
			break;
		}
	}
	return nReg;     
}


int	KNpc::CheckAllItem(int nKeyCol,int nGen,int nDetail,int nPart)
{
	int nRow = g_ForbitMap.GetHeight()+1,nReg=0;
	
	for (int i=2;i<nRow;++i)
	{
		int nMaps[3];
		nMaps[0]=0;
		nMaps[1]=0;
		nMaps[2]=0;

		g_ForbitMap.GetInt3(i,nKeyCol,nMaps);

		if (nMaps[2]==-1)
			continue;

		if (nMaps[0]==nGen && nMaps[1]==nDetail && nMaps[2]==nPart)
		{
			nReg =1;
			break;
		}
	}
	return nReg;     
}

//服务器端--上线同步数据库基本属性信息
#ifdef _SERVER

BOOL KNpc::NpcTalkToSubWorld(char *TieleTxt,int nModel)
{
	char nMsg[256]={0};
	     GetstrInfo(STR_NPCTALK_MSG,nMsg,sizeof(nMsg));
	if (nMsg[0])
	{
	  //g_StrCpyLen(nMsg,m_NpcAutoChat.m_NpcTalkMsg, sizeof(m_NpcAutoChat.m_NpcTalkMsg));
      int nleg=strlen(nMsg);	//TEncodeText(nMsg, strlen(nMsg));
	      //nleg=TFilterEncodedText(nMsg, strlen(nMsg));
	  char * szMsg = nMsg;
	  char nNpcSname[64]={0};
	  if (nModel==0 && szMsg)
      {//公告 
		 nleg = TEncodeText(nMsg, strlen(nMsg));

		 if (TieleTxt)
		   sprintf(nNpcSname,"%s",TieleTxt);
         else
		   sprintf(nNpcSname,"%s",Name);
		
		KPlayerChat::SendSystemInfo(0,0,nNpcSname,(char *)szMsg,nleg);  //strlen(szMsg) 	
	  }
	  else if (szMsg)
	  {//头顶显示 局部聊天
		  //char* szMsg = (char*)Lua_ValueToString(L,2);
		  //int nLen= TEncodeText(szMsg, strlen(szMsg));
		  if(nleg >= MAX_MESSAGE_LENGTH)
			  szMsg[nleg-1]='\0';
		  NPC_CHAT_SYNC	command;
		  command.ProtocolType = s2c_npctalk;
		  command.ID = m_dwID;
		  //command.nTime=(int) Lua_ValueToNumber(L,3);  //分钟
		  //strcpy(command.szMsg, szMsg);
		  snprintf(command.szMsg, sizeof(command.szMsg), "%s", szMsg);
		  command.nMsgLen=nleg;
		  SendDataToNearRegion(&command, sizeof(NPC_CHAT_SYNC));
	  }

	  return TRUE;
	}
	return FALSE;
}

BOOL KNpc::SendSyncData(int nClient,bool bBroadCast) //同步客户端 怪物NPC
{
	BOOL	bRet = FALSE;
	NPC_SYNC	NpcSync;

	NpcSync.ProtocolType		= (BYTE)s2c_syncnpc;
	NpcSync.m_btKind			= (BYTE)m_Kind;
	NpcSync.Camp				= (BYTE)m_Camp;
	NpcSync.CurrentCamp			= (BYTE)m_CurrentCamp;
	NpcSync.m_bySeries			= (BYTE)m_Series;
	NpcSync.m_Doing				= (BYTE)m_Doing; //同步行为状态SPR
	 //m_CurrentLifeMax;
	/*
	if (m_CurrentLifeMax > 0)
		NpcSync.LifePerCent		= (BYTE)((m_CurrentLife<<7)/m_CurrentLifeMax);//(BYTE)( Question只改了这部分，其它的也需要spe修改
	else
		NpcSync.LifePerCent		= 0;*/

	if (m_CurrentLife>0 && m_CurrentLifeMax > 0)
	{
		//NpcSync.nLifeShang =TakeTrader(m_CurrentLifeMax,m_CurrentLife);
	   // NpcSync.nLifeYu =m_CurrentLifeMax-m_CurrentLife;//TakeRemainder(m_CurrentLifeMax,m_CurrentLife);
		NpcSync.nLifeDamagePerCent	= _CalcPercent(m_CurrentLife, m_CurrentLifeMax, 1000);
	}
	else
	   NpcSync.nLifeDamagePerCent = 0;

	if (this->IsPlayer()) //如果是玩家 同步状态
	{
		NpcSync.m_btMenuState	= Player[this->m_nPlayerIdx].m_cMenuState.m_nState;
	    NpcSync.NpcEnchant	    = 0;
	}
	else
	{
		NpcSync.NpcEnchant		= (WORD)this->m_cGold.GetGoldType(); //boss 类型
		NpcSync.m_btMenuState	= 0;
	}

	int Nmap=0;
	GetMpsPos((int *)&NpcSync.MapX, (int *)&NpcSync.MapY,&Nmap);
	NpcSync.ID					= m_dwID; //数量 第几个怪物
	NpcSync.NpcSettingIdx		= MAKELONG(m_Level, m_NpcSettingIdx);

	NpcSync.Dangge	= m_CurrentdanggeRate;  //档格
    NpcSync.Zhongji = m_CurrentzhongjiRate; //重击
	snprintf(NpcSync.m_szName, sizeof(NpcSync.m_szName),"%s",Name);              //同步名字
	NpcSync.m_wLength			= sizeof(NPC_SYNC) - 1 - sizeof(NpcSync.m_szName) + strlen(NpcSync.m_szName);


	if (bBroadCast)
	{
		this->SendDataToNearRegion((BYTE*)&NpcSync, NpcSync.m_wLength + 1);
	}
	else
	{
		if (SUCCEEDED(g_pServer->PackDataToClient(nClient, (BYTE*)&NpcSync, NpcSync.m_wLength + 1)))
		{
			//printf("Packing sync data ok...\n");
			bRet = TRUE;
		}
		else
		{
			printf("Packing sync data failed...\n");
			return FALSE;
		}
	}

	if (IsPlayer())  //同步客户端 玩家
	{
		PLAYER_SYNC	PlayerSync;
		PlayerSync.ProtocolType		= (BYTE)s2c_syncplayer; //s2c_synccurplayer
		PlayerSync.ID				= m_dwID;
		PlayerSync.TeamServerID		= Player[m_nPlayerIdx].m_cTeam.m_nFlag ? (int)Player[m_nPlayerIdx].m_cTeam.m_nID:-1;
		PlayerSync.ArmorType		= m_ArmorType;
		PlayerSync.HelmType			= m_HelmType;
		PlayerSync.MaskType			= m_MaskType;// 面具
	    PlayerSync.PifengType		= m_PifengType; 
		PlayerSync.mCJtaskID        = m_CJtaskID;//抽奖专用
		PlayerSync.mFaction=Player[m_nPlayerIdx].m_cFaction.m_nCurFaction;  //门派

		if (Player[m_nPlayerIdx].m_cTong.m_BMasterName[0]) //帮主名
		{
			PlayerSync.m_bFigure    = Player[m_nPlayerIdx].m_cTong.GetFigure();
			Player[m_nPlayerIdx].m_cTong.GetTongName(PlayerSync.TongName); //同步帮会名
			PlayerSync.TongNameID	= Player[m_nPlayerIdx].m_cTong.GetTongNameID();
			strcpy(PlayerSync.TongTitle, Player[m_nPlayerIdx].m_cTong.m_szTitle);
		}
		else
		{//没有帮派的
			PlayerSync.TongNameID	= 0;
			PlayerSync.m_bFigure	= -1;
			/*if (m_IsVip>=1)
			strcpy(PlayerSync.TongName,"荣●誉");
			else */if (Player[m_nPlayerIdx].m_cFaction.m_nCurFaction<0)
				strcpy(PlayerSync.TongName,"十年一剑,经典涅槃");
            else if (Player[m_nPlayerIdx].m_cFaction.m_nCurFaction==0) 
				strcpy(PlayerSync.TongName,"少林派");
			else if (Player[m_nPlayerIdx].m_cFaction.m_nCurFaction==1) 
				strcpy(PlayerSync.TongName,"天王帮");
			else if (Player[m_nPlayerIdx].m_cFaction.m_nCurFaction==2) 
				strcpy(PlayerSync.TongName,"唐门教");
			else if (Player[m_nPlayerIdx].m_cFaction.m_nCurFaction==3) 
				strcpy(PlayerSync.TongName,"五毒教");
			else if (Player[m_nPlayerIdx].m_cFaction.m_nCurFaction==4) 
				strcpy(PlayerSync.TongName,"峨嵋派");
			else if (Player[m_nPlayerIdx].m_cFaction.m_nCurFaction==5) 
				strcpy(PlayerSync.TongName,"翠烟门");
			else if (Player[m_nPlayerIdx].m_cFaction.m_nCurFaction==6) 
				strcpy(PlayerSync.TongName,"丐帮门");
			else if (Player[m_nPlayerIdx].m_cFaction.m_nCurFaction==7) 
				strcpy(PlayerSync.TongName,"天忍教");
			else if (Player[m_nPlayerIdx].m_cFaction.m_nCurFaction==8) 
				strcpy(PlayerSync.TongName,"武当派");
			else if (Player[m_nPlayerIdx].m_cFaction.m_nCurFaction==9) 
				strcpy(PlayerSync.TongName,"昆仑派");
			else if (Player[m_nPlayerIdx].m_cFaction.m_nCurFaction==10) 
				strcpy(PlayerSync.TongName,"华山派");
			else
				strcpy(PlayerSync.TongName,"十年一剑,经典涅槃");
		}
		                                         
		if(m_bRideHorse)
		{
			PlayerSync.HorseType		= m_HorseType;
		}
		else
		{
			PlayerSync.HorseType        = -1;  //下马 或无马 状态
		}

		PlayerSync.m_bBaiTan = m_BaiTan;

		PlayerSync.RunSpeed			= m_CurrentRunSpeed;
		PlayerSync.WalkSpeed		= m_CurrentWalkSpeed;
		PlayerSync.WeaponType		= (BYTE)m_WeaponType;
		PlayerSync.RankID			= m_btRankId;
		PlayerSync.RankFFID			= (BYTE)m_btRankFFId;
		PlayerSync.ExItemID			= (BYTE)m_ExItemId; // hanh trang
		PlayerSync.ExBoxID			= (BYTE)m_ExBoxId; // ruong mo rong
		PlayerSync.RankInWorld		= Player[m_nPlayerIdx].m_nWorldStat;
		PlayerSync.Repute			= Player[m_nPlayerIdx].m_cRepute.GetReputeValue();
		PlayerSync.PKValue		= Player[m_nPlayerIdx].m_cPK.GetPKValue();
		PlayerSync.ReBorn			= Player[m_nPlayerIdx].m_cReBorn.GetReBornCount();
		PlayerSync.m_btSomeFlag		= 0;
		if (!Player[m_nPlayerIdx].m_cPK.GetNormalPKState())
			PlayerSync.m_btSomeFlag |= 0x01; 
		if (Npc[Player[m_nPlayerIdx].m_nIndex].m_FightMode)
			PlayerSync.m_btSomeFlag |= 0x02;
		if (Player[m_nPlayerIdx].m_bSleepMode)
			PlayerSync.m_btSomeFlag |= 0x04;
		if (Player[m_nPlayerIdx].m_cPK.GetNormalPKState() == 1)
			PlayerSync.m_btSomeFlag |= 0x10;

		if (bBroadCast)
		{
			this->SendDataToNearRegion((BYTE*)&PlayerSync, sizeof(PLAYER_SYNC));
		}
		else
		{
			if (SUCCEEDED(g_pServer->PackDataToClient(nClient, (BYTE*)&PlayerSync, sizeof(PLAYER_SYNC))))
			{
				bRet = TRUE;
			}
			else
			{
				printf("Packing player sync data failed...\n");
				return FALSE;
			}
		}
	}
	return bRet;
}

void KNpc::SetNpcCishu(int cishu)
{
    m_CJtaskID=cishu;
}


void KNpc::NormalMapSync()
{
	if (m_Doing == do_revive || m_Doing == do_death || !m_Index || m_RegionIndex < 0)
		return;

	if  (m_SubWorldIndex<0 || m_SubWorldIndex>=MAX_SUBWORLD) 
		return;

	if  ( m_RegionIndex >=SubWorld[m_SubWorldIndex].m_nTotalRegion)
		return;

	if (IsPlayer())
	{
		PLAYER_NORMALMAP_SYNC mPlayerSync;
		mPlayerSync.ProtocolType	= (BYTE)s2c_syncplayermap;
		mPlayerSync.ID			= m_dwID;
		mPlayerSync.m_IsInCity = m_IsInCity; // Player[m_nPlayerIdx].m_nChestPW;

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
		
		int nMaxCount = MAX_BROADCAST_COUNT,j;
		CURREGION.BroadCast(&mPlayerSync, sizeof(PLAYER_NORMALMAP_SYNC), nMaxCount, m_MapX, m_MapY);  // 区域小范围同步
		for (j = 0; j < 8; ++j)
		{
			int nConRegion = CURREGION.m_nConnectRegion[j];
			if  (nConRegion<0 || nConRegion>=SubWorld[m_SubWorldIndex].m_nTotalRegion)
					continue;

			SubWorld[m_SubWorldIndex].m_Region[nConRegion].BroadCast((BYTE*)&mPlayerSync, sizeof(PLAYER_NORMALMAP_SYNC), nMaxCount, m_MapX - POff[j].x, m_MapY - POff[j].y);
		}
	}
}

// 服务器端--客户端实时数据的同步--无限循环的--实时更新同步数据--包含临时变量
void KNpc::NormalSync()
{

	if (m_Doing == do_revive || m_Doing == do_death || !m_Index || m_RegionIndex < 0)
		return;

	if  (m_SubWorldIndex<0 || m_SubWorldIndex>=MAX_SUBWORLD) 
		return;
	if  ( m_RegionIndex >=SubWorld[m_SubWorldIndex].m_nTotalRegion)
		return;
	NPC_NORMAL_SYNC NpcSync;

	int nMpsX, nMpsY,nMap;

	GetMpsPos(&nMpsX,&nMpsY,&nMap);

	NpcSync.ProtocolType = (BYTE)s2c_syncnpcmin;  //怪物

	NpcSync.ID = m_dwID;

	NpcSync.nCurrentLifeMax	= m_CurrentLifeMax;

	if (m_CurrentLifeMax == 0) 
		return;

	if (m_CurrentLife > 0)                        //为什么要左移 放大数据？
	{
	 //   //NpcSync.LifePerCent        =(BYTE)((m_CurrentLife<<7)/m_CurrentLifeMax);//(BYTE)
		//if (m_CurrentLifeMax > 0)
		//{
			NpcSync.nLifeYu= 1;//m_CurrentLifeMax-m_CurrentLife;
		//	if (m_CurrentLife<0)
		//		m_CurrentLife=0;
		//}
		//NpcSync.nLifeDamagePerCent = 0;//m_CurrentLifeDamage+m_CurPoisonDamage;//冒血处理
		NpcSync.nLifeDamagePerCent	= _CalcPercent(m_CurrentLife, m_CurrentLifeMax, 1000);
		//NpcSync.btCurrentManaPerCent	= _CalcPercent(m_CurrentMana, m_CurrentManaMax, 1000);

	}
	else
	{   m_CurrentLifeDamage        = 0;
		NpcSync.nLifeDamagePerCent = 0;
		//NpcSync.LifePerCent        = 0;
		m_CurrentLife              = 0;
	}

	memcpy(NpcSync.StateInfo, m_btStateInfo ,sizeof(BYTE) * MAX_SKILL_STATE);

	NpcSync.MapX           = nMpsX;
	NpcSync.MapY           = nMpsY;

	NpcSync.m_wOffX     = m_OffX;
	NpcSync.m_wOffY     = m_OffY;

	NpcSync.ASpeed	    = m_CurrentAttackSpeed;    //外功攻速
	NpcSync.CSpeed	    = m_CurrentCastSpeed;      //内功攻速
    NpcSync.nAttackFrame =	m_AttackFrame;         //外功攻速帧数
	NpcSync.nCastFrame   =  m_CastFrame;           //内功攻速帧数
    NpcSync.nMaskType	= m_MaskType;              // mat na
	NpcSync.Dangge	        = m_CurrentdanggeRate;     //档格
    NpcSync.Zhongji         = m_CurrentzhongjiRate;

	//char nstrName[64]={0};
	//if (m_Kind != kind_player)
    //   GetstrInfo(STR_GUISHU_NAME,nstrName);

	//sprintf(NpcSync.GuishuName,"%s",nstrName); //归属
	snprintf(NpcSync.nName, sizeof(NpcSync.nName),"%s",Name);              //同步名字
	//sprintf(NpcSync._clientName,"%s",_Name);
	NpcSync.nNpcTitle	= m_NpcTitle;
	NpcSync.nCurNpcTitle= m_CurNpcTitle;
	//实时同步当前地图的 倍率
	if (IsPlayer())
	{
       NpcSync.AIsJinYan   = IsJinYan  = SubWorld[m_SubWorldIndex].m_IsJinYan;                //人物活动经验
	   NpcSync.AIsLukey    = IsLuKey   = SubWorld[m_SubWorldIndex].m_IsLuKey;                 //人物活动幸运倍数
	   NpcSync.AIsJinQian  = IsJinQian = SubWorld[m_SubWorldIndex].m_IsJinQian;			   //人物的经验倍率
	   NpcSync.mFactionNo  = Player[m_nPlayerIdx].m_cTask.m_FactionNo;
	}
	else
	{
		NpcSync.AIsJinYan   = IsJinYan;                //怪物活动经验
		NpcSync.AIsLukey    = IsLuKey;                 //怪物活动幸运倍数
		NpcSync.AIsJinQian  = IsJinQian;			    //怪物的经验倍率
		NpcSync.mFactionNo  = m_FactionNo;
	}
	   NpcSync.nExpPer     = m_Experience;	            //怪物的经验基础倍率

  //  m_GoldLucky = g_GlobalMissionArray.GetMissionValue(28);
	if  (m_GoldLucky<=0)
         m_GoldLucky=1;
	 //if (SubWorld[m_SubWorldIndex].m_nWPKFlag > -1)
	NpcSync.gsLukey    = m_GoldLucky;
	//NpcSync.IsTuiGuang  = m_IsTuiGuang;
	NpcSync.RankFFID      = m_btRankFFId;
    //NpcSync.IsVip         = m_IsVip;               //会员
    NpcSync.IsXingYunXing = m_IsXingYunXing;       //幸运星

	NpcSync.mliveTime     = m_GameliveTime;
	NpcSync.nGuiShuDwid   = m_GuiShuDwid;          //主人的dwid
    m_IsXingYunXing=0;
	//sprintf(NpcSync.ItmeInfo,m_ItmeInfo);        //物品说明
	NpcSync.Camp          = m_CurrentCamp;         //当前阵营
	NpcSync.nZhuaVal      = m_ZhuaVal;
	NpcSync.nTongBanNum   = m_TongBanNum;

    if (!IsPlayer() && m_IsSynDir>=1)
	{
        NpcSync.nIsSynDir = m_IsSynDir;
	    NpcSync.nDir      = m_Dir; //Npc方向
	}

	NpcSync.nState = 0;

	if (m_FreezeState.nTime > 0)
		NpcSync.nState |= STATE_FREEZE;  //1   2的0次方
	//逐位或运算符逐位或运算符（|）比较两个操作数对应的二进制位，当两个二进制位均为0时，该位的结果取0，否则取1。
	if (m_PoisonState.nTime > 0)
		NpcSync.nState |= STATE_POISON;  //2   2的1次方
	if (m_ConfuseState.nTime > 0)
		NpcSync.nState |= STATE_CONFUSE; //4   2的2次方
	if (m_StunState.nTime > 0)
		NpcSync.nState |= STATE_STUN;    //8   2的3次方
	if (m_Hide.nTime > 0)
		NpcSync.nState |= STATE_HIDE;    //16  2的4次方

	if (m_IgnoreNegativeState.nTime > 0)
		NpcSync.nState |= STATE_IGNORENEGATIVE;

/*    if (m_randmove.nTime > 0)
	    NpcSync.nIsMove = 1;
	else
	    NpcSync.nIsMove = 0;
*/
	if (m_IsRe)
		NpcSync.nReForm	= m_ReviveFrame;
	else
	    NpcSync.nReForm	=0; 

	//if (m_ZhuaState.nTime > 0)
	//	NpcSync.State |= STATE_ZHUA; //抓捕状态

	NpcSync.Doing = (BYTE)m_Doing;
        
	// 由于同步数据每次同步的很少，所以上限就是MAX_PLAYER
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

	int nMaxCount =MAX_PLAYER;// MAX_BROADCAST_COUNT;
	CURREGION.BroadCast(&NpcSync, sizeof(NPC_NORMAL_SYNC), nMaxCount, m_MapX, m_MapY);
	int j;
	for (j = 0; j < 8; ++j)
	{
		int nConRegion = CURREGION.m_nConnectRegion[j];
		if  (nConRegion<0 || nConRegion>=SubWorld[m_SubWorldIndex].m_nTotalRegion)
			continue;

		SubWorld[m_SubWorldIndex].m_Region[nConRegion].BroadCast((BYTE*)&NpcSync, sizeof(NPC_NORMAL_SYNC), nMaxCount, m_MapX - POff[j].x, m_MapY - POff[j].y);
	}

	m_CurrentLifeDamage=0;

	if (IsPlayer())  //玩家
	{
		PLAYER_NORMAL_SYNC PlayerSync;
		PlayerSync.ProtocolType	= (BYTE)s2c_syncplayermin;
		PlayerSync.ID			= m_dwID;

		if (m_bLockNpcDwID>0)
		{
			//m_bLockNpcDwID=Npc[m_nPeopleIdx].m_dwID;
			PlayerSync.m_LockNpcDwID=m_bLockNpcDwID; 	//被锁住NPC的索引
		}
		else
		{
			m_bLockNpcDwID=0;
			PlayerSync.m_LockNpcDwID=0; 
		}
		
		//PlayerSync.AttackSpeed= m_CurrentAttackSpeed;
		//PlayerSync.CastSpeed	= m_CurrentCastSpeed;
		PlayerSync.nCurGsSerIdx     = (BYTE)nCurSerIdx; //当前GS的编号

		if (Player[m_nPlayerIdx].m_CurSerVer!=nCurSerIdx)
		    PlayerSync.nIsTranse    = (BYTE)nCurSerIdx;
		else
			PlayerSync.nIsTranse    = FALSE;

		PlayerSync.RunSpeed		= m_CurrentRunSpeed;
		PlayerSync.WalkSpeed	= m_CurrentWalkSpeed;
		PlayerSync.HelmType		= m_HelmType;
		PlayerSync.ArmorType	= m_ArmorType;
		PlayerSync.WeaponType	= m_WeaponType;
		//PlayerSync.MaskType	= m_MaskType;       // mat na
		PlayerSync.m_IsInCity	= m_IsInCity ;//= Player[m_nPlayerIdx].m_nChestPW;
		PlayerSync.mAttackState = Player[m_nPlayerIdx].m_cTong.m_AttackState;
        PlayerSync.PifengType	= m_PifengType;     //披风
		PlayerSync.nChiBangType = m_ChiBangType;
		PlayerSync.mCJtaskID    = m_CJtaskID;       //抽奖专用
		/*PlayerSync.GuziYajing =m_GuziYajing;      //押金
		PlayerSync.GuziDianshu  =m_GuziDianshu;     //点数
		PlayerSync.GuziZhuang   =m_GuziZhuang;      //是否庄家
		PlayerSync.GuziMenber   =m_GuziMenber;      //是否正式下注者
		PlayerSync.IsShuai      =m_IsShuai;         //是否已经甩了
        PlayerSync.ZuoWeihao    =m_ZuoWeihao;       //座位号
		PlayerSync.IsTuiGuang   =m_IsTuiGuang;      //推广员
        PlayerSync.IsVip        =m_IsVip;
		*/

		PlayerSync.mIsCreatTongBan=IsCreatTongBan;  //是否有同伴
		//PlayerSync.mIsWaiGua    =m_IsWaiGua;        //是否使用外挂
		//PlayerSync.mIsCheckWaiGua	= Player[m_nPlayerIdx].m_nCheckWaiGua;
		PlayerSync.msPlayerIdx  = Player[m_nPlayerIdx].GetPlayerIndex();
		PlayerSync.mFaction=Player[m_nPlayerIdx].m_cFaction.m_nCurFaction;  //门派

		PlayerSync.nIsSerLock = m_IsSerLock; //是否被锁住
		PlayerSync.nVipType   = (BYTE)Player[m_nPlayerIdx].GetVipType();
		//char nstrName[64]={0}; //自定意称号
		//     Npc[Player[m_nPlayerIdx].m_nIndex].GetstrInfo(STR_CHEGNHAO_NAME,nstrName);
        //strcpy(PlayerSync.mChenHaoName,nstrName);

		if (m_MapUseModel.nMagicAttrib>0)
		   PlayerSync.nIsOpenMapType=m_MapUseModel.nValue[0];
		else
		   PlayerSync.nIsOpenMapType=0;

		//PlayerSync.IsTuiGuang   =m_IsTuiGuang;      //推广员

        PlayerSync.IsVip        = Player[m_nPlayerIdx].m_cVip.GetRoleVipLevel();//m_IsVip;

		if (Player[m_nPlayerIdx].m_cTong.m_BMasterName[0])
		{//有帮派的
			PlayerSync.m_bFigure  = Player[m_nPlayerIdx].m_cTong.GetFigure();
			PlayerSync.mIsWarCity = m_IsWarCity;
			Player[m_nPlayerIdx].m_cTong.GetTongName(PlayerSync.TongName);
			strcpy(PlayerSync.TongTitle, Player[m_nPlayerIdx].m_cTong.m_szTitle);
		}
		else
		{//没有帮派的	
			PlayerSync.m_bFigure		= -1;
			m_IsWarCity=-1;

			char nstrName[64]={0};//自定义称号
			Npc[Player[m_nPlayerIdx].m_nIndex].GetstrInfo(STR_CHEGNHAO_NAME,nstrName,sizeof(nstrName));

			if (nstrName[0])
				strcpy(PlayerSync.TongName,nstrName);
			else
			{
				/*if (m_IsVip>=1)
				strcpy(PlayerSync.TongName,"荣●誉");
				else*/ if (Player[m_nPlayerIdx].m_cFaction.m_nCurFaction<0)
					strcpy(PlayerSync.TongName,"十年一剑,经典涅槃");
				else if (Player[m_nPlayerIdx].m_cFaction.m_nCurFaction==0) 
					strcpy(PlayerSync.TongName,"少林派");
				else if (Player[m_nPlayerIdx].m_cFaction.m_nCurFaction==1) 
					strcpy(PlayerSync.TongName,"天王帮");
				else if (Player[m_nPlayerIdx].m_cFaction.m_nCurFaction==2) 
					strcpy(PlayerSync.TongName,"唐门教");
				else if (Player[m_nPlayerIdx].m_cFaction.m_nCurFaction==3) 
					strcpy(PlayerSync.TongName,"五毒教");
				else if (Player[m_nPlayerIdx].m_cFaction.m_nCurFaction==4) 
					strcpy(PlayerSync.TongName,"峨嵋派");
				else if (Player[m_nPlayerIdx].m_cFaction.m_nCurFaction==5) 
					strcpy(PlayerSync.TongName,"翠烟门");
				else if (Player[m_nPlayerIdx].m_cFaction.m_nCurFaction==6) 
					strcpy(PlayerSync.TongName,"丐帮门");
				else if (Player[m_nPlayerIdx].m_cFaction.m_nCurFaction==7) 
					strcpy(PlayerSync.TongName,"天忍教");
				else if (Player[m_nPlayerIdx].m_cFaction.m_nCurFaction==8) 
					strcpy(PlayerSync.TongName,"武当派");
				else if (Player[m_nPlayerIdx].m_cFaction.m_nCurFaction==9) 
					strcpy(PlayerSync.TongName,"昆仑派");
				else if (Player[m_nPlayerIdx].m_cFaction.m_nCurFaction==10) 
					strcpy(PlayerSync.TongName,"华山派");
				else
					strcpy(PlayerSync.TongName,"十年一剑,经典涅槃");
			}
		}

		//实时更新骑马
		if(m_bRideHorse)
			PlayerSync.HorseType		= m_HorseType;
		else
			PlayerSync.HorseType        = -1;

        //其他数据同步
		PlayerSync.RankID			= m_btRankId;

		PlayerSync.RankFFID			= (BYTE)m_btRankFFId;
		PlayerSync.AutoplayId		= (BYTE)m_AutoplayId; //挂机
		PlayerSync.ExItemID			= (BYTE)m_ExItemId; // hanh trang
		PlayerSync.ExBoxID			= (BYTE)m_ExBoxId;  // ruong mo rong  

		PlayerSync.RankInWorld		= Player[m_nPlayerIdx].m_nWorldStat;

		PlayerSync.Repute			    = Player[m_nPlayerIdx].m_cRepute.GetReputeValue();
		
		PlayerSync.PKValue          = MAKELONG(Player[m_nPlayerIdx].m_isGm,Player[m_nPlayerIdx].m_cPK.GetPKValue());

		//PlayerSync.PKValue			= Player[m_nPlayerIdx].m_cPK.GetPKValue();  //pk值
		
		
		PlayerSync.ReBorn			    = Player[m_nPlayerIdx].m_cReBorn.GetReBornCount();  //转生的次数
	//	PlayerSync.CUnlocked		= (BYTE)Player[m_nPlayerIdx].m_CUnlocked; //是否锁屏
		PlayerSync.m_bBaiTan       = m_BaiTan;
		if (m_BaiTan)
			GetstrInfo(STR_SHOP_NAME,PlayerSync.ShopName,sizeof(PlayerSync.ShopName)); //摆摊的名称

		//if (SubWorld[m_SubWorldIndex].m_nWPKFlag > -1 && Npc[Player[m_nPlayerIdx].m_nIndex].m_ProtectTime==0 && Player[m_nPlayerIdx].m_cPK.GetNormalPKState()!=SubWorld[m_SubWorldIndex].m_nWPKFlag)          //设置地图的	PK	模式
  //          Player[m_nPlayerIdx].m_cPK.SetNormalPKState(SubWorld[m_SubWorldIndex].m_nWPKFlag); //设置PK模式

		//if (SubWorld[m_SubWorldIndex].m_nWcomp >-1 && Npc[Player[m_nPlayerIdx].m_nIndex].m_CurrentCamp!=SubWorld[m_SubWorldIndex].m_nWcomp)	            //改变临时阵营
		//	Npc[Player[m_nPlayerIdx].m_nIndex].SetCurrentCamp(SubWorld[m_SubWorldIndex].m_nWcomp);

		PlayerSync.m_btSomeFlag	= 0;

		if (!Player[m_nPlayerIdx].m_cPK.GetNormalPKState())	    //0
			PlayerSync.m_btSomeFlag |= 0x01;

		if (m_FightMode)
			PlayerSync.m_btSomeFlag |= 0x02; //战斗模式

		if (Npc[Player[m_nPlayerIdx].m_nIndex].m_ProtectTime==0 && SubWorld[m_SubWorldIndex].m_nWFight ==1)   //设置战斗模式
			m_FightMode=1;
		if (SubWorld[m_SubWorldIndex].m_nWFight ==0)
			m_FightMode=0;

		if (Player[m_nPlayerIdx].m_bSleepMode)
			PlayerSync.m_btSomeFlag |= 0x04;

		if (Player[m_nPlayerIdx].m_cPK.GetNormalPKState() == 1)  //pk状态
			PlayerSync.m_btSomeFlag |= 0x10;

		if (Player[m_nPlayerIdx].m_cTong.GetOpenFlag())         //是否入帮并且职位不是帮众（帮主 长老 队长）
			PlayerSync.m_btSomeFlag |= 0x08;

            PlayerSync.mJinMaiBingJia = m_JinMaiBingJia;
            PlayerSync.mZhenYuan      = m_ZhenYuan;

		int nMaxCount = MAX_PLAYER;//MAX_BROADCAST_COUNT;
		CURREGION.BroadCast(&PlayerSync, sizeof(PLAYER_NORMAL_SYNC), nMaxCount, m_MapX, m_MapY);  // 区域小范围同步
		
		for (j = 0; j < 8; ++j)
		{
			int nConRegion = CURREGION.m_nConnectRegion[j];
		    if  (nConRegion<0 || nConRegion>=SubWorld[m_SubWorldIndex].m_nTotalRegion)
				continue;

			SubWorld[m_SubWorldIndex].m_Region[nConRegion].BroadCast((BYTE*)&PlayerSync, sizeof(PLAYER_NORMAL_SYNC), nMaxCount, m_MapX - POff[j].x, m_MapY - POff[j].y);
		}

		NPC_PLAYER_TYPE_NORMAL_SYNC	sSync;
		sSync.ProtocolType = s2c_syncnpcminplayer;
		sSync.m_dwNpcID = m_dwID;

		sSync.m_GameLeftTiem = Player[m_nPlayerIdx].m_GameLeftTime;
		
		sSync.m_dwMapX = nMpsX;
		sSync.m_dwMapY = nMpsY;
		sSync.m_wOffX = m_OffX;
		sSync.m_wOffY = m_OffY;
		//sSync.m_byDoing = this->m_Doing;
		g_pServer->PackDataToClient(Player[m_nPlayerIdx].m_nNetConnectIdx, (BYTE*)&sSync, sizeof(sSync));
 
	}
}
//广播同步重生
void KNpc::BroadCastRevive(int nType)
{
	//if (!IsPlayer())  
	//	return;
	if  (m_SubWorldIndex<0 || m_SubWorldIndex>=MAX_SUBWORLD) return;

	if (m_RegionIndex < 0)
		return;

	if  ( m_RegionIndex >=SubWorld[m_SubWorldIndex].m_nTotalRegion)
		return;

	NPC_REVIVE_SYNC	NpcReviveSync;
	NpcReviveSync.ProtocolType = s2c_playerrevive;
	NpcReviveSync.ID           = m_dwID;
	NpcReviveSync.mType        = nType;

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
	CURREGION.BroadCast((BYTE*)&NpcReviveSync, sizeof(NPC_REVIVE_SYNC), nMaxCount, m_MapX, m_MapY);
	int j;
	for (j = 0; j < 8; ++j)
	{
		int nConRegion = CURREGION.m_nConnectRegion[j];
		if  (nConRegion<0 || nConRegion>=SubWorld[m_SubWorldIndex].m_nTotalRegion)
		    continue;
		SubWorld[m_SubWorldIndex].m_Region[nConRegion].BroadCast((BYTE*)&NpcReviveSync, sizeof(NPC_REVIVE_SYNC), nMaxCount, m_MapX - POff[j].x, m_MapY - POff[j].y);
	}
}

int	KNpc::GetPlayerIdx()
{
	if (m_Kind != kind_player)
		return 0;
	return m_nPlayerIdx;
}
#endif

#ifndef _SERVER
#include "scene/KScenePlaceC.h"
//绘制名字 血条 等
int KNpc::PaintInfo(int nHeightOffset, bool bSelect, int nFontSize, DWORD dwBorderColor)  //绘制信息
{
	int nMpsX, nMpsY,nmMap;
	    GetMpsPos(&nMpsX, &nMpsY,&nmMap);
	///nFontSize=12;
	DWORD	dwColor;
	int nHeightOff = nHeightOffset + nFontSize + 1;
	int  nHigthOff = 0;
	
	if (relation_enemy == NpcSet.GetRelation(m_Index, Player[CLIENT_PLAYER_INDEX].m_nIndex) &&
		(m_Kind == kind_player || m_Kind == kind_partner) && m_Hide.nTime > 0 )//和玩家有敌对关系并且隐藏时间大于的 不显示
	{
		return 0;
	}

	if (m_Kind == kind_player) //玩家阵营颜色显示
	{
		switch(m_CurrentCamp)
		{
		case camp_begin: 
			dwColor = 0xffffffff;   //新手
			break;
		case camp_justice:
			dwColor = 0xff000000 | (255 << 16) | (168 << 8) | 94;
			break;
		case camp_evil:
			dwColor = 0xff000000 | (255 << 16) | (146 << 8) | 255;
			break;
		case camp_balance:
			dwColor = 0xff000000 | (85 << 16) | (255 << 8) | 145;
			break;
		case camp_free:
			dwColor = 0xff000000 | (255 << 16);  //杀手
			break;
		case camp_blue:
			dwColor = TGetColor("74,74,255");
			break;
		case camp_green:
			dwColor = TGetColor("0,249,0");  //杀手
			break;
		default:
			dwColor = 0xffff00ff;
			break;
		}
//		char  xinxi[128];
//      sprintf(xinxi,"[CQ:%d,SH:%d,PE:%d,LU:%d,PF:%d,JY:%d,JQ:%d,生命:%d,JN:%d,dwID:%d,TYPE:%d]",m_BtnFlag,m_WarShuishou,m_nPeopleIdx,IsLuKey,m_PifengType,IsJinYan,IsJinQian,m_CurrentLife,m_ActiveSkillID,m_dwID,this->m_cGold.GetGoldType());  
//		g_pRepresent->OutputText(nFontSize, xinxi, KRF_ZERO_END, nMpsX - nFontSize * g_StrLen(xinxi)/4, nMpsY-24, dwColor, 0, nHeightOff, dwBorderColor);
//		nHeightOffset += nFontSize + 1;
	
		char	szString[128]={0};		
        sprintf(szString,"%s",Name); 

		if (m_IsInCity==1)
		{
           strcat(szString, "☆凤");
		   //strcat(szString, m_ScriptPicPath);
		}
		else if (m_IsInCity==11)
		{
			strcat(szString, "☆都");
			//strcat(szString, m_ScriptPicPath);
		}
		else if (m_IsInCity==162)
		{
			strcat(szString, "☆理");
			//strcat(szString, m_ScriptPicPath);
		}
		else if (m_IsInCity==37)
		{
			strcat(szString, "☆京");
			//strcat(szString, m_ScriptPicPath);
		}
		else if (m_IsInCity==78)
		{
			strcat(szString, "☆阳");
			//strcat(szString, m_ScriptPicPath);
		}
		else if (m_IsInCity==80)
		{
			strcat(szString, "☆州");
			//strcat(szString, m_ScriptPicPath);
		}
		else if (m_IsInCity==176)
		{
			strcat(szString, "☆安");
			//strcat(szString, m_ScriptPicPath);
		}
		/*else
		{	char nMsg[32];
		    sprintf(nMsg,"锁定:%d",m_nPeopleIdx);
			strcat(szString, nMsg);	 //m_WeaponType
		}*/

		//if (m_FreezeState.nTime || m_PoisonState.nTime || m_ConfuseState.nTime || m_StunState.nTime || m_Hide.nTime || m_randmove.nTime)
		 if (m_FreezeState.nTime || m_PoisonState.nTime || m_ConfuseState.nTime || m_StunState.nTime || m_Hide.nTime || m_randmove.nTime)
		{
			strcat(szString, "(");
			if (m_FreezeState.nTime)
				strcat(szString, "冰");
			//if (m_PoisonState.nTime)
			if (m_PoisonState.nTime)
				strcat(szString, "毒");
			if (m_ConfuseState.nTime)
				strcat(szString, "乱");
			if (m_StunState.nTime)
				strcat(szString, "晕");
			if (m_randmove.nTime)
				strcat(szString, "恐");
			if (m_Hide.nTime)
			   strcat(szString, "隐");
			strcat(szString, ")");
		}

      if ( m_PifengType>0)	// 披风称号
		g_pRepresent->OutputText(nFontSize, szString, KRF_ZERO_END, nMpsX +20- nFontSize * g_StrLen(Name)/4, nMpsY-10, dwColor, 0, nHeightOff, dwBorderColor); //+100
      else
        g_pRepresent->OutputText(nFontSize, szString, KRF_ZERO_END, nMpsX- nFontSize * g_StrLen(Name)/4, nMpsY-10, dwColor, 0, nHeightOff, dwBorderColor); //+100   		
 

	   nMpsY=nMpsY-10;

       nHeightOffset += nFontSize + 1;

       char Livename[40]={0}; 

       /*if (m_GameliveTime>0)
	   { 
		  dwColor=TGetColor("255,255,255");
		  dwBorderColor=TGetColor("0,0,0");
          int nHour=0,nMin=0,nSec=0;
		  
		  nHour = m_GameliveTime/3600;//TakeTrader(m_GameliveTime,3600);  //小时
		  
		  nMin  = (m_GameliveTime-nHour*3600)/60;//TakeTrader(m_GameliveTime-nHour*3600,60); //剩余的秒大于60秒，就计算分
		  
		  if (nMin>0)
		  {
			  nSec  = (m_GameliveTime-nHour*3600-nMin*60);  //秒
			  
		  }
		  else
			  nSec  = m_GameliveTime-nHour*3600;
		  	  
		  wsprintf(Livename,"(剩余时间:%d小时%d分%d秒)",nHour,nMin,nSec);
		  g_pRepresent->OutputText(nFontSize,Livename, KRF_ZERO_END, nMpsX - nFontSize * g_StrLen(Name)/4-50, nMpsY+150, dwColor, 0, nHeightOff, dwBorderColor);
		  nHeightOffset += nFontSize + 1;
	   }
	   else	*/
	 /* if (m_MapUseModel.nTime>0)
	   {
		   dwColor=TGetColor("255,255,255");
		   dwBorderColor=TGetColor("0,0,0");
		   if (m_MapUseModel.nTime==1)
			   wsprintf(Livename,"(剩余金币:%d个)",Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipmentXu());
		   else	if (m_MapUseModel.nTime==2)
			   wsprintf(Livename,"(剩余银两:%d两)",Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipmentMoney());
		   else	if (m_MapUseModel.nTime==3)
			   wsprintf(Livename,"(剩余经验:%d点)",Player[CLIENT_PLAYER_INDEX].m_nExp);
		   else	if (m_MapUseModel.nTime==4)
			   wsprintf(Livename,"(剩余富源:%d点)",Player[CLIENT_PLAYER_INDEX].m_cFuYuan.GetFuYuanValue());
		   else	if (m_MapUseModel.nTime==5)
			   wsprintf(Livename,"(剩余声望:%d点)",Player[CLIENT_PLAYER_INDEX].m_cRepute.GetReputeValue());
		   
		   g_pRepresent->OutputText(nFontSize,Livename, KRF_ZERO_END, nMpsX - nFontSize * g_StrLen(Name)/4-25, nMpsY+150, dwColor, 0, nHeightOff, dwBorderColor);
		   nHeightOffset += nFontSize + 1;
	   }
	   else
	   { 
		    ZeroMemory(Livename,sizeof(Livename));
	   }*/ 

		//------------------------------------------
		if ( m_PifengType>0)	// 披风称号
		{	
			nHeightOffset +=8;
		    if (m_Doing == do_sit && !m_MaskType)
				nHeightOffset +=6;
			else if	(m_MaskType)
				nHeightOffset +=-6;
			
			PaintPifeng(m_PifengType,nMpsX -40- nFontSize * g_StrLen(Name)/4,nMpsY-GetNpcPate()-nHeightOffset,7);	//显示spr称号  nMpsX-30	
		}
		//-------------------------------------------
      //  int bb= Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_ExBoxId;
		
	 	if(m_btRankFFId > 0 && m_btRankFFId!=88 && m_btRankFFId!=99)	//显示spr称号
		{	
			PaintHonor(m_btRankFFId,nMpsX,nMpsY,6);	//显示spr称号			
		}
		 char *nPath=NULL;
		if (m_AutoplayId ==1)
		{
			//this->SetFrameSpr("\\spr\\Ui3\\剑侠助手\\state_autofight.spr",0, 120,GetNpcPate(),100);
			nPath="\\spr\\Ui3\\剑侠助手\\state_autofight.spr";
			SetClientSpr(nPath,nMpsX, nMpsY-GetNpcPate()-nHeightOff*1.8,0,0);
		}
		//转生的光环
		if (nReBorn>0)
		{
			nPath="\\spr\\skill\\others\\重击.spr";
			SetClientSpr(nPath,nMpsX - nFontSize * g_StrLen(Name)/4+30, nMpsY-GetNpcPate()+80,0,6);
		}
		//神将的状态 --红月
		/*if (m_nFuMoNum[0].nNpcSetings >0 || m_nFuMoNum[1].nNpcSetings >0||m_nFuMoNum[2].nNpcSetings >0||m_nFuMoNum[3].nNpcSetings >0||m_nFuMoNum[4].nNpcSetings >0)
		{	
			nPath="\\spr\\skill\\zhongqiu\\zq_sl_003.spr";
			SetClientSpr(nPath,nMpsX,nMpsY-GetNpcPate(),0,7);	 //-nHeightOff
		}  */

		if (TongName[0])  //帮派 显示帮派称号
		{
			char szTong[64];
			//dwColor=GetColor("255,255,255");
            //dwBorderColor=GetColor("255,255,0");
			dwBorderColor=TGetColor("0,0,0");

            strcpy(szTong,TongName);

			switch(m_nFigure)  //帮会职位
			{
			case enumTONG_FIGURE_MEMBER://帮众
				{
                if (m_IsWarCity==78)
                {
				   ZeroMemory(szTong,sizeof(szTong));
				   strcpy(szTong,"襄阳·");
		           strcat(szTong,TongName);
				}
				else if (m_IsWarCity==1)
				{
					ZeroMemory(szTong,sizeof(szTong));
					strcpy(szTong,"凤翔·");
		            strcat(szTong,TongName);
				}
				else if (m_IsWarCity==162)
				{
					ZeroMemory(szTong,sizeof(szTong));
					strcpy(szTong,"大理·");
					strcat(szTong,TongName);
				}
				else if (m_IsWarCity==11)
				{
					ZeroMemory(szTong,sizeof(szTong));
					strcpy(szTong,"成都·");
					strcat(szTong,TongName);
				}
				else if (m_IsWarCity==53)
				{
					ZeroMemory(szTong,sizeof(szTong));
					strcpy(szTong,"巴陵·");
					strcat(szTong,TongName);
				}
				else if (m_IsWarCity==176)
				{
					ZeroMemory(szTong,sizeof(szTong));
					strcpy(szTong,"临安·");
					strcat(szTong,TongName);
				}
				else if (m_IsWarCity==80)
				{
					ZeroMemory(szTong,sizeof(szTong));
					strcpy(szTong,"扬州·");
					strcat(szTong,TongName);
				}
				else if (m_IsWarCity==28)
				{
					ZeroMemory(szTong,sizeof(szTong));
					strcpy(szTong,"汴京·");
					strcat(szTong,TongName);
				}
			/*	if (m_WarFucheng==1)
                    strcat(szTong,"(副城主)");
				else if (m_WarZuoHu==2)
					strcat(szTong,"(左护法)");
				else if (m_WarYouHu==3)
					strcat(szTong,"(右护法)");
				else if (m_WarTaishi==4)
					strcat(szTong,"(太史令)");
				else if (m_WarZhongShu==5)
					strcat(szTong,"(中书令)");
				else if (m_WarShangShu==6)
					strcat(szTong,"(尚书令)");
				else if (m_WarMiShu==7)
					strcat(szTong,"(秘书令)");
				else if (m_WarTaiLe==8)
					strcat(szTong,"(太乐令)");
                else */
				strcat(szTong,"(帮众)");

				//dwColor=GetColor("255,255,0");
                //dwBorderColor=GetColor("255,0,0");
				dwBorderColor=TGetColor("0,0,0");
				}
				break;
			case enumTONG_FIGURE_MANAGER:  // 队长
				{
				 if (m_IsWarCity==78)
				 {
					ZeroMemory(szTong,sizeof(szTong));
					strcpy(szTong,"襄阳·");
					strcat(szTong,TongName);
				 }
				 else if (m_IsWarCity==1)
				 {
					ZeroMemory(szTong,sizeof(szTong));
					strcpy(szTong,"凤翔·");
					strcat(szTong,TongName);
				 }
				 else if (m_IsWarCity==162)
				 { 
				   ZeroMemory(szTong,sizeof(szTong));
				   strcpy(szTong,"大理·");
				   strcat(szTong,TongName);
				 } 
				 else if(m_IsWarCity==11)
				 { 
				  ZeroMemory(szTong,sizeof(szTong));
				  strcpy(szTong,"成都·");
				  strcat(szTong,TongName);
				 } 
				 else if (m_IsWarCity==53)
				 {   
				  ZeroMemory(szTong,sizeof(szTong));
				  strcpy(szTong,"巴陵·");
				  strcat(szTong,TongName);
				 } 
				 else if (m_IsWarCity==176)
				 {  
				  ZeroMemory(szTong,sizeof(szTong));
				  strcpy(szTong,"临安·");
				  strcat(szTong,TongName);
				 } 
			     else if (m_IsWarCity==80)
				 { 
				  ZeroMemory(szTong,sizeof(szTong));
				  strcpy(szTong,"扬州·");
				  strcat(szTong,TongName);
				 }
				 else if (m_IsWarCity==28)
				 { 
				  ZeroMemory(szTong,sizeof(szTong));
				  strcpy(szTong,"汴京·");
				  strcat(szTong,TongName);
				 }
				/*if (m_WarFucheng==1)
                    strcat(szTong,"(副城主)");
				else if (m_WarZuoHu==2)
					strcat(szTong,"(左护法)");
				else if (m_WarYouHu==3)
					strcat(szTong,"(右护法)");
				else if (m_WarTaishi==4)
					strcat(szTong,"(太史令)");
				else if (m_WarZhongShu==5)
					strcat(szTong,"(中书令)");
				else if (m_WarShangShu==6)
					strcat(szTong,"(尚书令)");
				else if (m_WarMiShu==7)
					strcat(szTong,"(秘书令)");
				else if (m_WarTaiLe==8)
					strcat(szTong,"(太乐令)");
                else */
				strcat(szTong,"(队长)");

				//dwColor=GetColor("255,255,0");
                //dwBorderColor=GetColor("255,0,0");
				dwBorderColor=TGetColor("0,0,0");
				}
				break;
			case enumTONG_FIGURE_DIRECTOR:  // 长老
				{
					if (m_IsWarCity==78)
					{
						ZeroMemory(szTong,sizeof(szTong));
						strcpy(szTong,"襄阳·");
						strcat(szTong,TongName);
					}
					else if (m_IsWarCity==1)
					{
						ZeroMemory(szTong,sizeof(szTong));
						strcpy(szTong,"凤翔·");
						strcat(szTong,TongName);
					}
					else if (m_IsWarCity==162)
					{ 
						ZeroMemory(szTong,sizeof(szTong));
						strcpy(szTong,"大理·");
						strcat(szTong,TongName);
					} 
					else if(m_IsWarCity==11)
					{ 
						ZeroMemory(szTong,sizeof(szTong));
						strcpy(szTong,"成都·");
						strcat(szTong,TongName);
					} 
					else if (m_IsWarCity==53)
					{   
						ZeroMemory(szTong,sizeof(szTong));
						strcpy(szTong,"巴陵·");
						strcat(szTong,TongName);
					} 
					else if (m_IsWarCity==176)
					{  
						ZeroMemory(szTong,sizeof(szTong));
						strcpy(szTong,"临安·");
						strcat(szTong,TongName);
					} 
					else if (m_IsWarCity==80)
					{ 
						ZeroMemory(szTong,sizeof(szTong));
						strcpy(szTong,"扬州·");
						strcat(szTong,TongName);
					}
					else if (m_IsWarCity==28)
					{ 
						ZeroMemory(szTong,sizeof(szTong));
						strcpy(szTong,"汴京·");
						strcat(szTong,TongName);
					}
				/*if (m_WarFucheng==1)
                    strcat(szTong,"(副城主)");
				else if (m_WarZuoHu==2)
					strcat(szTong,"(左护法)");
				else if (m_WarYouHu==3)
					strcat(szTong,"(右护法)");
				else if (m_WarTaishi==4)
					strcat(szTong,"(太史令)");
				else if (m_WarZhongShu==5)
					strcat(szTong,"(中书令)");
				else if (m_WarShangShu==6)
					strcat(szTong,"(尚书令)");
				else if (m_WarMiShu==7)
					strcat(szTong,"(秘书令)");
				else if (m_WarTaiLe==8)
					strcat(szTong,"(太乐令)");
                else */
				     strcat(szTong,"(长老)");
				//dwColor=GetColor("255,255,0");
                //dwBorderColor=GetColor("255,0,0");
				dwBorderColor=TGetColor("0,0,0");
				}
				break;
			case enumTONG_FIGURE_MASTER:  // 帮主
				{
                if (m_IsWarCity==78)
				{
					ZeroMemory(szTong,sizeof(szTong));
					strcpy(szTong,"襄阳·");
				 	strcat(szTong,TongName);
					strcat(szTong,"(城主)");
				}  
                else if (m_IsWarCity==1)
				{ 
	             ZeroMemory(szTong,sizeof(szTong));
	             strcpy(szTong,"凤翔·");
	             strcat(szTong,TongName);
				 strcat(szTong,"(城主)");
				} 
                else if (m_IsWarCity==162)
				{  
	             ZeroMemory(szTong,sizeof(szTong));
	             strcpy(szTong,"大理·");
	             strcat(szTong,TongName);
				 strcat(szTong,"(城主)");
				}  
                else if(m_IsWarCity==11)
				{  
	             ZeroMemory(szTong,sizeof(szTong));
	             strcpy(szTong,"成都·");
	             strcat(szTong,TongName);
				 strcat(szTong,"(城主)");
				}  
                else if (m_IsWarCity==53)
				{    
	             ZeroMemory(szTong,sizeof(szTong));
	             strcpy(szTong,"巴陵·");
	             strcat(szTong,TongName);
				 strcat(szTong,"(城主)");
				}  
                else if (m_IsWarCity==176)
				{   
	            ZeroMemory(szTong,sizeof(szTong));
	             strcpy(szTong,"临安·");
	             strcat(szTong,TongName);
				 strcat(szTong,"(城主)");
				}  
                else if (m_IsWarCity==80)
				{  
	             ZeroMemory(szTong,sizeof(szTong));
	             strcpy(szTong,"扬州·");
	             strcat(szTong,TongName);
				 strcat(szTong,"(城主)");
				} 
                else if (m_IsWarCity==28)
				{  
	             ZeroMemory(szTong,sizeof(szTong));
	             strcpy(szTong,"汴京·");
	             strcat(szTong,TongName);
				 strcat(szTong,"(城主)");
				}
				else
				{
				  strcat(szTong,"(帮主)");
				}
				//dwColor=GetColor("255,255,0");
                //dwBorderColor=GetColor("255,0,0");
				dwBorderColor=TGetColor("0,0,0");
				}
				break;
			default:
				dwColor=TGetColor("255,255,255");
				break;
			}

			if (!m_bIsHideTong)
			{
			   g_pRepresent->OutputText(nFontSize, szTong, KRF_ZERO_END, nMpsX - nFontSize * g_StrLen(szTong) / 4, nMpsY-24, dwColor, 0, nHeightOff, dwBorderColor);
			   nHeightOffset += nFontSize + 1;
			}
			else
			{
				ZeroMemory(TongName,sizeof(TongName));
			}
		}

		char PlayerRank[32]={0};

		if  (m_CurNpcTitle>0)
		{
			int nVal=0;
			char nClorVal[32]={0};
			dwColor = 0xffff00ff;
			dwBorderColor=TGetColor("0,0,0");
			g_PlayerTitle.GetString(m_CurNpcTitle+1,"Color","",nClorVal,sizeof(nClorVal));
			if (strstr(nClorVal,","))
				dwColor=TGetColor(nClorVal);
			else
				dwColor=atoi(nClorVal);
			
			if (dwColor<=0)
				dwColor = 0xffff00ff;
			
			g_PlayerTitle.GetString(m_CurNpcTitle+1,"TitleName","",PlayerRank,sizeof(PlayerRank));
			
			int nYY = nMpsY;

			if (TongName[0])
			{
				nYY -= 50;
			}
			else
				nYY -= 26;
			
			g_pRepresent->OutputText(nFontSize, PlayerRank, KRF_ZERO_END, nMpsX - nFontSize * g_StrLen(PlayerRank) / 4, nYY, dwColor,0, nHeightOff, dwBorderColor);
			nHeightOffset += nFontSize + 1;
		}
		else if  (m_NpcTitle>0)
		{

			int nVal=0;
			char nClorVal[32]={0};
			dwColor = 0xffff00ff;
			dwBorderColor=TGetColor("0,0,0");
			g_PlayerTitle.GetString(m_NpcTitle+1,"Color","",nClorVal,sizeof(nClorVal));
			if (strstr(nClorVal,","))
				dwColor=TGetColor(nClorVal);
			else
				dwColor=atoi(nClorVal);
			
			if (dwColor<=0)
				dwColor = 0xffff00ff;

			g_PlayerTitle.GetString(m_NpcTitle+1,"TitleName","",PlayerRank,sizeof(PlayerRank));

			int nYY = nMpsY;
			if (TongName[0])
			{
				nYY -= 50;
			}
			else
				nYY -= 26;
			//测试调试
			/*char nMsg[16]={0};
			sprintf(nMsg,"：%d:%d",m_ChiBangType,m_Camp);
			strcat(PlayerRank,nMsg);*/
		
			g_pRepresent->OutputText(nFontSize, PlayerRank, KRF_ZERO_END, nMpsX - nFontSize * g_StrLen(PlayerRank) / 4, nYY, dwColor,0, nHeightOff, dwBorderColor);
			nHeightOffset += nFontSize + 1;

			/*if(m_NpcTitle>0 && m_btRankFFId <=0)//没有属性称号的时候就显示
			{	
				int nStateID=0;
				g_PlayerTitle.GetInteger(m_NpcTitle+1,"SpeicalGraphic",0,&nStateID);
				if (nStateID>0)
				{
					//int sprMpsX, sprMpsY,sprmMap;
					//GetMpsPos(&sprMpsX, &sprMpsY,&sprmMap);
					
					char nSpePath[128]={0},ncPart[32]={0};
					cTabFile.GetString(nStateID+1,"文件名","",nSpePath,sizeof(nSpePath));
					cTabFile.GetString(nStateID+1,"类型","",ncPart,sizeof(ncPart));
					if (nSpePath)
						m_DataRes.SetClientSpr(nSpePath,nMpsX,nMpsY,0,0,ncPart,nStateID);
				}
				
			}*/ 
	
		}
		else if (m_btRankId>0) //显示老的称号
		{
			//char szRankId[5];
			//itoa(m_btRankId, szRankId, 10); //int 转 char
			g_RankTabSetting.GetString(m_btRankId, "RANKSTR", "", PlayerRank, sizeof(PlayerRank));
			dwColor = 0xffff00ff;
			dwBorderColor=TGetColor("0,0,0");
			int nYY = nMpsY;

			if (TongName[0])
			{
				nYY -= 50;
			}
			else
				nYY -= 26;


			g_pRepresent->OutputText(nFontSize, PlayerRank, KRF_ZERO_END, nMpsX - nFontSize * g_StrLen(PlayerRank) / 4, nYY, dwColor,0, nHeightOff, dwBorderColor);
			nHeightOffset += nFontSize + 1;
		} 
		else
		{
			sprintf(PlayerRank,"剑侠情缘");
			dwColor = TGetColor("15,249,68");//0xffff00ff;
			dwBorderColor=TGetColor("0,0,0");
			int nYY = nMpsY;
			if (TongName[0])
			{
				nYY -= 50;
			}
			else
				nYY -= 26;

			g_pRepresent->OutputText(nFontSize, PlayerRank, KRF_ZERO_END, nMpsX - nFontSize * g_StrLen(PlayerRank) / 4, nYY, dwColor,0, nHeightOff, dwBorderColor);
			nHeightOffset += nFontSize + 1;
		}

		if (m_MaskType > 0)	//带面具的时候才显示
		{
			if (m_bRideHorse)  //如何骑马则 绘制窗口
			{
				char* sOut = "\\Spr\\UI3\\Npc\\ActionIcon_RideHorse.spr";//Spr
				SetClientSpr(sOut,nMpsX + (nFontSize * g_StrLen(Name) / 4),nMpsY-nHeightOff*1.8,0,1);
			}
			else if (m_Doing == do_sit)
			{
				char* sOut = "\\Spr\\UI3\\Npc\\ActionIcon_Sit.spr";
				SetClientSpr(sOut,nMpsX + (nFontSize * g_StrLen(Name) / 4),nMpsY-nHeightOff*1.7,0,2);
				
			}
		}
		if (m_BaiTan) //摆摊
		{
			int	nMpsX, nMpsY,nMmap;
			    GetMpsPos(&nMpsX, &nMpsY,&nMmap);
				char nstrName[32]={0};
		        GetstrInfo(STR_SHOP_NAME,nstrName,sizeof(nstrName));
			int nWid = nFontSize * g_StrLen(nstrName)/2 + 10;
			int nHei = nFontSize + 12;
			char*sOutm = "\\Spr\\Ui3\\摆摊\\摆摊头顶条－中.spr";
			KRUImage RUIconImageR;
			RUIconImageR.nType = ISI_T_SPR;
			RUIconImageR.Color.Color_b.a = 255;
			RUIconImageR.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
			RUIconImageR.uImage = 0;
			RUIconImageR.nISPosition = IMAGE_IS_POSITION_INIT;
			RUIconImageR.bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;
			strcpy(RUIconImageR.szImage, sOutm);
			RUIconImageR.oPosition.nX = nMpsX - nWid / 2;
			RUIconImageR.oPosition.nY = nMpsY - 35;
			RUIconImageR.oPosition.nZ = nHeightOffset;
			RUIconImageR.nFrame = 0;
			
			for (int i = 0; i < nWid;++i)
			{
				RUIconImageR.oPosition.nX = nMpsX - nWid / 2 + i;
				g_pRepresent->DrawPrimitives(1, &RUIconImageR, RU_T_IMAGE, FALSE);
			}

			if (PlayerRank[0] || TongName[0])
			{
				dwColor = 0x00c3a94e;
				dwBorderColor=TGetColor("0,0,0");

				if (TongName[0] && PlayerRank[0]) //显示 摆摊广告  和 是称号 为空的话
					g_pRepresent->OutputText(nFontSize, nstrName, KRF_ZERO_END, nMpsX - nFontSize * g_StrLen(nstrName) / 4, nMpsY - 76, dwColor, 0, nHeightOff, dwBorderColor);
				else if (TongName[0]||PlayerRank[0])
					g_pRepresent->OutputText(nFontSize, nstrName, KRF_ZERO_END, nMpsX - nFontSize * g_StrLen(nstrName) / 4, nMpsY - 50, dwColor, 0, nHeightOff, dwBorderColor);
			
			}
			else
			{
				dwColor = 0x00c3a94e;
				dwBorderColor=TGetColor("0,0,0");
				g_pRepresent->OutputText(nFontSize, nstrName, KRF_ZERO_END, nMpsX - nFontSize * g_StrLen(nstrName) / 4, nMpsY - 34, dwColor, 0, nHeightOff, dwBorderColor);
			}

			char*   sOut = "\\Spr\\Ui3\\摆摊\\摆摊头顶条－左.spr";
			SetClientSpr(sOut,nMpsX - nWid / 2 - 10,nMpsY+6,nHeightOffset + 23,4);
		            sOut = "\\Spr\\Ui3\\摆摊\\摆摊头顶条－右.spr";
			SetClientSpr(sOut,nMpsX + nWid / 2,nMpsY+6,nHeightOffset + 23,3);

			nHeightOffset += nFontSize + 1;
			//----------------------------------------------------------------------------------------------------------------------------

		}
	}
	else if (m_Kind == kind_dialoger)  //对话NPC
	{ 
        char outname[64]="";           //怪物名称

		if (m_GameliveTime>0)
		{
			dwColor=TGetColor("251,84,4");
            dwBorderColor=TGetColor("0,0,0");

			int nHour=0,nMin=0,nSec=0;
			
			nHour = m_GameliveTime/3600;//TakeTrader(m_GameliveTime,3600);  //小时
		  
		    nMin  = (m_GameliveTime-nHour*3600)/60;//TakeTrader(m_GameliveTime-nHour*3600,60); //剩余的秒大于60秒，就计算分
			
			if (nMin>0)
			{
				nSec  = (m_GameliveTime-nHour*3600-nMin*60);  //秒
				
			}
			else
				nSec  = m_GameliveTime-nHour*3600;
			char nstrName[64]={0};
		    GetstrInfo(STR_GUISHU_NAME,nstrName,sizeof(nstrName));

			snprintf(outname, sizeof(outname), "%s", nstrName);
			g_pRepresent->OutputText(nFontSize,outname, KRF_ZERO_END, nMpsX - nFontSize * g_StrLen(outname)/4, nMpsY-57, dwColor, 0, nHeightOff, dwBorderColor);
			nHeightOffset += nFontSize + 1;

			snprintf(outname, sizeof(outname),"(存活时间:%d小时%d分%d秒)",nHour,nMin,nSec);
			g_pRepresent->OutputText(nFontSize,outname, KRF_ZERO_END, nMpsX - nFontSize * g_StrLen(outname)/4, nMpsY-30, dwColor, 0, nHeightOff, dwBorderColor);
			nHeightOffset += nFontSize + 1;
		}

		   dwColor=TGetColor("255,255,255");
           dwBorderColor=TGetColor("0,0,0");
	       snprintf(outname, sizeof(outname),"%s",Name);//m_NpcSettingIdx           
	       g_pRepresent->OutputText(nFontSize,outname, KRF_ZERO_END, nMpsX - nFontSize * g_StrLen(outname)/4, nMpsY-5, dwColor, 0, nHeightOff, dwBorderColor);
	 	   nHeightOffset += nFontSize + 1;
	}
	else if (m_Kind == kind_partner)  //
	{   
		char outname[64]=""; //怪物名称
		if (m_btRankFFId==99) 
		{//宠物  
			dwColor=TGetColor("255,255,255");
			dwBorderColor=TGetColor("0,0,0");	

			if (m_GameliveTime>0)
			{//限时同伴
                int nHour=0,nMin=0,nSec=0;

                //nHour = TakeTrader(m_GameliveTime,3600);  //小时
		        //nMin  = TakeTrader(m_GameliveTime-nHour*3600,60); //剩余的秒大于60秒，就计算分
				nHour = m_GameliveTime/3600;//TakeTrader(m_GameliveTime,3600);  //小时	
				nMin  = (m_GameliveTime-nHour*3600)/60;//TakeTrader(m_GameliveTime-nHour*3600,60); //剩余的秒大于60秒，就计算分


					if (nMin>0)
					{
                         nSec  = (m_GameliveTime-nHour*3600-nMin*60);  //秒

					}
					else
                         nSec  = m_GameliveTime-nHour*3600;
			
				
                snprintf(outname, sizeof(outname),"(存活时间:%d小时%d分%d秒)",nHour,nMin,nSec);
			    g_pRepresent->OutputText(nFontSize,outname, KRF_ZERO_END, nMpsX - nFontSize * g_StrLen(outname)/4, nMpsY-30, dwColor, 0, nHeightOff, dwBorderColor);
		        nHeightOffset += nFontSize + 1;

				char nFileName[128];
				sprintf(nFileName,"\\spr\\skill\\others\\助攻王.spr");
				SetClientSpr(nFileName,nMpsX - (nFontSize * g_StrLen(outname)/4+10),nMpsY-nHeightOff*2.7,0,7);

			}

            dwColor=TGetColor("237,157,24");
			dwBorderColor=TGetColor("0,0,0");
			snprintf(outname, sizeof(outname),"%s",Name);
		    strcat(outname,"(宠物)");
		   //strcat(outname,m_GuishuName);  //同伴归属
		   //sprintf(outnamea,"[阵营:%d,name:%s,M_dwID:%d,M_Index:%d,PeopleId:%d]",m_Camp);             
			g_pRepresent->OutputText(nFontSize,outname, KRF_ZERO_END, nMpsX - nFontSize * g_StrLen(outname)/4, nMpsY-6, dwColor, 0, nHeightOff, dwBorderColor);
		    nHeightOffset += nFontSize + 1;

			//PaintHonor(30,nMpsX,nMpsY,6);	//显示spr称号
			
		}
		else
		{//其他	镖车
			dwColor      = TGetColor("234,289,11");
			dwBorderColor= TGetColor("0,0,0");
			if (m_GameliveTime>0)
			{	
                int nHour=0,nMin=0,nSec=0;

                //nHour = TakeTrader(m_GameliveTime,3600);  //小时
		        //nMin  = TakeTrader(m_GameliveTime-nHour*3600,60); //剩余的秒大于60秒，就计算分
				nHour = m_GameliveTime/3600;//TakeTrader(m_GameliveTime,3600);  //小时	
				nMin  = (m_GameliveTime-nHour*3600)/60;//TakeTrader(m_GameliveTime-nHour*3600,60); //剩余的秒大于60秒，就计算分

				
				if (nMin>0)
				{
					nSec  = (m_GameliveTime-nHour*3600-nMin*60);  //秒
					
				}
				else
                    nSec  = m_GameliveTime-nHour*3600;

                snprintf(outname, sizeof(outname),"(存活时间:%d小时%d分%d秒)",nHour,nMin,nSec);
				g_pRepresent->OutputText(nFontSize,outname, KRF_ZERO_END, nMpsX - nFontSize * g_StrLen(outname)/4, nMpsY-30, dwColor, 0, nHeightOff, dwBorderColor);
				nHeightOffset += nFontSize + 1;
			}

			snprintf(outname, sizeof(outname),"%s",Name);
		    g_pRepresent->OutputText(nFontSize,outname, KRF_ZERO_END, nMpsX - nFontSize * g_StrLen(outname)/4, nMpsY-6, dwColor, 0, nHeightOff, dwBorderColor);
		    nHeightOffset += nFontSize + 1;
		}

		   
	}
    else if (bSelect) //是否 
	{//怪物	
		
		char outname[64]={0}; //怪物名称
		char outnamea[64]={0}; //怪物名称
		char* series;

		  if (m_GameliveTime>0)
		  {//限时NPC
			    dwColor=TGetColor("255,255,255");
			    int nHour=0,nMin=0,nSec=0;

                //nHour = TakeTrader(m_GameliveTime,3600);  //小时
		        //nMin  = TakeTrader(m_GameliveTime-nHour*3600,60); //剩余的秒大于60秒，就计算分
				nHour = m_GameliveTime/3600;//TakeTrader(m_GameliveTime,3600);  //小时	
				nMin  = (m_GameliveTime-nHour*3600)/60;//TakeTrader(m_GameliveTime-nHour*3600,60); //剩余的秒大于60秒，就计算分

				
				if (nMin>0)
				{
					nSec  = (m_GameliveTime-nHour*3600-nMin*60);  //秒
					
				}
				else
                    nSec  = m_GameliveTime-nHour*3600;


            snprintf(outname, sizeof(outname),"(存活时间:%d小时%d分%d秒)",nHour,nMin,nSec);
			g_pRepresent->OutputText(nFontSize,outname, KRF_ZERO_END, nMpsX - nFontSize * g_StrLen(outname)/4, nMpsY-30, dwColor, 0, nHeightOff, dwBorderColor);
			nHeightOffset += nFontSize + 1;

			/*wsprintf(outname,"归属:%s",m_GuishuName);
			g_pRepresent->OutputText(nFontSize,outname, KRF_ZERO_END, nMpsX - nFontSize * g_StrLen(outname)/4, nMpsY+100, dwColor, 0, nHeightOff, dwBorderColor);
			nHeightOffset += nFontSize + 1;*/

		  }

		  int nType = this->m_cGold.GetGoldType();  //获取是否黄金怪物！0为非黄金
		  int nVal=0;
		      g_NpcSetting.GetInteger(m_NpcSettingIdx+2, "AuraSkillId", 0, &nVal);

		        if (nType == 0 && nVal==0)
					dwColor = 0xffffffff;           //白色名字怪物
				else if (nType ==1) 
					dwColor = 0xff00ff00;	        // 绿怪  爆钱
				else if (nType >=2 && nType <= 4)
					dwColor=TGetColor("100,100,255");//浅蓝100,100,255 深蓝 0,0,160
				else if (nType >=5 && nType <= 11)
                    dwColor=TGetColor("234,189,11"); //GetColor(" 255,255,0");
				else if (nType >=12 && nType <= 16)
					dwColor=TGetColor("255,62,62");	//   深红"210,0,0"
				else 
				{
				   if (nVal>0)
				   {
					  dwColor=TGetColor("234,189,11");
				   }

				   //g_NpcSetting.GetInteger(m_NpcSettingIdx+2, "PasstSkillId", 0, &nVal)
				}

           if (m_ZhuaVal>0)
		   {//被抓捕状态
			   dwColor=TGetColor("255,217,78");
			   dwBorderColor=TGetColor("0,0,0");
               snprintf(outname, sizeof(outname),"%s(被抓捕中..)",Name);
		   }
		   else
		   {
                 snprintf(outname, sizeof(outname),"%s",Name); 
		   }
           
           if (m_FreezeState.nTime || m_PoisonState.nTime/*m_PoisonState.nTime*/ || m_ConfuseState.nTime || m_StunState.nTime || m_randmove.nTime)
           {
		       strcat(outname, "(");
			if (m_FreezeState.nTime)
				strcat(outname, "冰");
			//if (m_PoisonState.nTime)
			if  (m_PoisonState.nTime)
			   strcat(outname, "毒");
			if (m_ConfuseState.nTime)
				strcat(outname, "乱");
			if (m_StunState.nTime)
				strcat(outname, "晕");
			if (m_randmove.nTime)
			    strcat(outname, "恐");
			strcat(outname, ")");
		   }

		      snprintf(outnamea, sizeof(outnamea),"%s,Lv:%d",outname,m_Level);  // m_NpcSettingIdx      
		   g_pRepresent->OutputText(nFontSize,outnamea, KRF_ZERO_END, nMpsX - nFontSize * g_StrLen(outname)/4, nMpsY-6, dwColor, 0, nHeightOff, dwBorderColor);
		   nHeightOffset += nFontSize + 1;
		  

		switch(m_Series) 
		{
		case 0:
			series= "\\Spr\\Ui3\\Npc\\series0.spr";
			break;
		case 1:
			series= "\\Spr\\Ui3\\Npc\\series1.spr";
			break;
		case 2:
			series= "\\Spr\\Ui3\\Npc\\series2.spr";
			break;
		case 3:
			series= "\\Spr\\Ui3\\Npc\\series3.spr";
			break;
		case 4:
			series= "\\Spr\\Ui3\\Npc\\series4.spr";
			break;
		}

		if  (m_Series >= 0  && m_Series<= 4)
		   SetClientSpr(series,nMpsX - (nFontSize * g_StrLen(outname)/4+20),nMpsY-nHeightOff*1.8+1,0,5);
		 
/*
		KRUImage RUIconImage;
		RUIconImage.nType = ISI_T_SPR;
		RUIconImage.Color.Color_b.a = 255;
		RUIconImage.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
		RUIconImage.uImage = 0;
		RUIconImage.nISPosition = IMAGE_IS_POSITION_INIT;
		RUIconImage.bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;
		strcpy(RUIconImage.szImage, series);
		RUIconImage.oPosition.nX = nMpsX - (nFontSize * g_StrLen(outname)/4+20); //图片的位置
		RUIconImage.oPosition.nY = nMpsY-nHeightOff*1.75;
	//	RUIconImage.oPosition.nY = nMpsY;
		RUIconImage.oPosition.nZ = 0;
		RUIconImage.nFrame = 0;
		g_pRepresent->DrawPrimitives(1, &RUIconImage, RU_T_IMAGE, FALSE);
*/

	  
		dwColor=TGetColor("255,255,255");
		dwBorderColor=TGetColor("0,0,0");
		ZeroMemory(outnamea,sizeof(outnamea));

		if (!m_bIsHideLife && !Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_bIsHideLife)
		{ 
		   sprintf(outnamea,"%d/%d",m_CurrentLife,m_CurrentLifeMax);
		} 
		else
		  ZeroMemory(outnamea,sizeof(outnamea));

		g_pRepresent->OutputText(nFontSize,outnamea,KRF_ZERO_END,nMpsX-nFontSize*g_StrLen(outnamea)/4, nMpsY+30, dwColor, 0, nHeightOff, dwBorderColor);
		nHeightOffset += nFontSize + 1;
	 
	}
/*	
#ifdef SWORDONLINE_SHOW_DBUG_INFO
	if (Player[CLIENT_PLAYER_INDEX].m_DebugMode)
	{
		char szNameID[50];
		sprintf(szNameID,"[%d]", m_dwID);
		g_pRepresent->OutputText(nFontSize, szNameID, KRF_ZERO_END, nMpsX, nMpsY + 20, 0xfff0fff0, 0, m_Height);
	}
	
	if (Player[CLIENT_PLAYER_INDEX].m_nIndex == m_Index && Player[CLIENT_PLAYER_INDEX].m_DebugMode)
	{
		char	szMsg[256];
		int nCount[9];
		for (int i = 0; i < 9; ++i)
			nCount[i] = 0;
		if (LEFTUPREGIONIDX >= 0)
			nCount[0] = LEFTUPREGION.m_NpcList.GetNodeCount();
		if (UPREGIONIDX >= 0)
			nCount[1] = UPREGION.m_NpcList.GetNodeCount();
		if (RIGHTUPREGIONIDX >= 0)
			nCount[2] = RIGHTUPREGION.m_NpcList.GetNodeCount();
		if (LEFTREGIONIDX >= 0)
			nCount[3] = LEFTREGION.m_NpcList.GetNodeCount();
		if (m_RegionIndex >= 0)
			nCount[4] = CURREGION.m_NpcList.GetNodeCount();
		if (RIGHTREGIONIDX >= 0)
			nCount[5] = RIGHTREGION.m_NpcList.GetNodeCount();
		if (LEFTDOWNREGIONIDX >= 0)
			nCount[6] = LEFTDOWNREGION.m_NpcList.GetNodeCount();
		if (DOWNREGIONIDX >= 0)
			nCount[7] = DOWNREGION.m_NpcList.GetNodeCount();
		if (RIGHTDOWNREGIONIDX >= 0)
			nCount[8] = RIGHTDOWNREGION.m_NpcList.GetNodeCount();
		
		int nPosX, nPosY;
		GetMpsPos(&nPosX, &nPosY);
		sprintf(szMsg,
			"NpcID:%d  Life:%d\nRegionIndex:%d Pos:%d,%d\nPlayerNumber:%d\n"
			"NpcNumber:\n%02d,%02d,%02d\n%02d,%02d,%02d\n%02d,%02d,%02d",
			m_dwID,
			m_CurrentLife,			
			m_RegionIndex,
			m_MapX,
			m_MapY,
			CURREGION.m_PlayerList.GetNodeCount(),
			nCount[0], nCount[1], nCount[2],
			nCount[3], nCount[4], nCount[5],
			nCount[6], nCount[7], nCount[8]			
			);
		
		g_pRepresent->OutputText(14, szMsg, -1, 320, 40, 0xffffffff);

	}
#endif
	*/	   
	

	return nHeightOffset;
}
#ifndef _SERVER

void KNpc::SetClientSpr(char *nSprPath,int nxLeft,int nyTop,int nzPos,int i,char *ncPart)
{
				KRUImage RUIconImage;
				RUIconImage.nType = ISI_T_SPR;
				RUIconImage.Color.Color_b.a = 255;
				RUIconImage.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
				RUIconImage.uImage = 0;
				RUIconImage.nISPosition = IMAGE_IS_POSITION_INIT;
				RUIconImage.bRenderFlag =RUIMAGE_RENDER_FLAG_REF_SPOT;

				strcpy(RUIconImage.szImage, nSprPath);  //改变坐姿

				if (ncPart)
				{
					if (strstr(ncPart,"Head")) //头部
					{
						RUIconImage.oPosition.nX = nxLeft;
						RUIconImage.oPosition.nY = nyTop-10;
					}
					else if (strstr(ncPart,"Body"))	//中部
					{
						RUIconImage.oPosition.nX = nxLeft;
						RUIconImage.oPosition.nY = nyTop;
					}
					else
					{//脚步
						RUIconImage.oPosition.nX = nxLeft;
						RUIconImage.oPosition.nY = nyTop;
					}
				    
				}
				else
				{
					RUIconImage.oPosition.nX = nxLeft;
					RUIconImage.oPosition.nY = nyTop;
				}
				RUIconImage.oPosition.nZ = nzPos; 

				/*
				KRUImage RUIconImageR;
				RUIconImageR.nType = ISI_T_SPR;
				RUIconImageR.Color.Color_b.a = 255;
				RUIconImageR.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
				RUIconImageR.uImage = 0;
				RUIconImageR.nISPosition = IMAGE_IS_POSITION_INIT;
				RUIconImageR.bRenderFlag = 0;
				strcpy(RUIconImageR.szImage,TaskBg);  //高亮图
				RUIconImageR.oPosition.nX = nYx;//nMpsX - nWid / 2;
				RUIconImageR.oPosition.nY = nYy;//nMpsY - 35;
				RUIconImageR.oPosition.nZ = 0;
				RUIconImageR.nFrame = 0;
				g_pRepresentShell->DrawPrimitives(1,&RUIconImageR, RU_T_IMAGE,TRUE);

				*/
				KImageParam	 imgParam; 
				imgParam.nNumFrames = 0;
				g_pRepresent->GetImageParam(nSprPath, &imgParam, ISI_T_SPR);
				
				if (imgParam.nNumFrames > 0 && m_DurFrame[i] < imgParam.nNumFrames)
				{			
					IR_NextFrame(m_DurFrame[i], imgParam.nNumFrames,imgParam.nInterval,uFlipTime[i]);
					RUIconImage.nFrame =m_DurFrame[i];
				}
				else
				{
					m_DurFrame[i]=0;
					RUIconImage.nFrame =0;		
				}	
				g_pRepresent->DrawPrimitives(1, &RUIconImage, RU_T_IMAGE, FALSE);	 //RU_T_IMAGE
}
#endif
//--------------------------------------------------------------------------
//	功能：把字符串表示的颜色信息转为数值表示
//--------------------------------------------------------------------------
/*unsigned int KNpc::GetColor(LPCTSTR pString)
{
	if (pString == NULL)
		return 0;
	
	unsigned int Color = 0xFF000000;  //黑色
	
	char Buf[16] = "";
	int  i = 0;
	int  n = 0;
	while (pString[i] != ',')
	{
		if (pString[i] == 0 || n >= 15)
			return Color;
		Buf[n++] = pString[i++];
	}
	
	Buf[n] = 0;
	Color += ((atoi(Buf) & 0xFF) << 16);
	n = 0;
	i++;
	while (pString[i] != ',')
	{
		if (pString[i] == 0 || n >= 15)
			return Color;
		Buf[n++] = pString[i++];
	}
	Buf[n] = 0;
	Color += ((atoi(Buf) & 0xFF) << 8);
	n = 0;
	i++;
	while (pString[i] != 0)
	{
		if (n >= 15)
			return Color;
		Buf[n++] = pString[i++];
	}
	Buf[n] = 0;
	Color += (atoi(Buf) & 0xFF);
	return Color;
}
//转换成字符串 颜色
const char* KNpc::GetColorString(unsigned int nColor)
{
	static char szColor[12];
	KRColor c;
	c.Color_dw = nColor;
	sprintf(szColor, "%d,%d,%d", c.Color_b.r, c.Color_b.g, c.Color_b.b);
	szColor[11] = 0;
	
	return szColor;
}
*/
//是玩家头顶信息显现
int	KNpc::PaintChat(int nHeightOffset)
{
	//if (m_Kind != kind_player)  //是玩家头顶才显现
	//	return nHeightOffset;

	if (m_nChatContentLen <= 0)
		return nHeightOffset;
	if (m_nChatNumLine <= 0)
		return nHeightOffset;

	int nFontSize = 12;
	int					nWidth, nHeight;
	int					nMpsX, nMpsY,nMap;
	KRUShadow			sShadow;
	KOutputTextParam	sParam;
	sParam.BorderColor = 0XFF000000; //字体边缘

	sParam.nNumLine = m_nChatNumLine;

	nWidth = m_nChatFontWidth * nFontSize / 2;//
	nHeight = sParam.nNumLine * (nFontSize + 1);

	nWidth += 10;	//为了好看
	nHeight += 5;	//为了好看


	GetMpsPos(&nMpsX, &nMpsY,&nMap);
	sParam.nX = nMpsX - nWidth / 2;
	sParam.nY = nMpsY;
	sParam.nZ = nHeightOffset + nHeight;
	sParam.Color = SHOW_CHAT_COLOR;
	sParam.nSkipLine = 0;
	sParam.nVertAlign = 0;

	sShadow.oPosition.nX = sParam.nX;
	sShadow.oPosition.nX -= 3;	//为了好看
	sShadow.oPosition.nY = sParam.nY;
	sShadow.oPosition.nZ = sParam.nZ;
	sShadow.oEndPos.nX = sParam.nX + nWidth;
	sShadow.oEndPos.nX += 2;	//为了好看
	sShadow.oEndPos.nY = sParam.nY;
	sShadow.oEndPos.nZ = sParam.nZ - nHeight;
	//sShadow.Color.Color_dw = 0x00FFFF00;
	sShadow.Color.Color_dw =0x14000000; // 0XFF000000
	sParam.bPicPackInSingleLine = true;

	g_pRepresent->OutputRichText(nFontSize, &sParam, m_szChatBuffer, m_nChatContentLen, nWidth);

	return sParam.nZ;
}

#include "../../Engine/Src/Text.h"
//自己聊天头顶显示
int	KNpc::SetChatInfo(const char* Name, const char* pMsgBuff, unsigned short nMsgLength)
{
	int nFontSize = 12;
	int nMaxStrLen;

    if (m_Kind == kind_player)
       nMaxStrLen = 32;	// 显示的长度
    else
       nMaxStrLen = 64;	// 显示的长度

	char szChatBuffer[MAX_SENTENCE_LENGTH];

	memset(szChatBuffer, 0, sizeof(szChatBuffer));
  //  m_nChatContentLen = TEncodeText(szChatBuffer, strlen(szChatBuffer));
	
	if (nMsgLength)
	{
		int nOffset = 0;
		if (pMsgBuff[0] != KTC_TAB) //空格
		{
			szChatBuffer[nOffset] = (char)KTC_COLOR;
			nOffset++;
			szChatBuffer[nOffset] = (char)0xFF;	  //三个颜色
			nOffset++;
			szChatBuffer[nOffset] = (char)0xFF;
			nOffset++;
			szChatBuffer[nOffset] = (char)0x00;
			nOffset++;
			strncpy(szChatBuffer + nOffset, Name, 32);
			(szChatBuffer + nOffset)[(32) - 1] = '\0';
			// strncpy wrote 32 bytes at most, so this advance may not claim more than
			// that: a longer name would move the cursor past the bytes that are there
			nOffset += ((int)strlen(Name) < 32 ? (int)strlen(Name) : 32);
			szChatBuffer[nOffset] = ':';
			nOffset++;
			szChatBuffer[nOffset] = (char)KTC_COLOR_RESTORE;
			nOffset++;
		}
		else
		{
			pMsgBuff ++;
			nMsgLength --;
		}

		if (nMsgLength)
		{
			// pMsgBuff is the caller's payload and nMsgLength its count, so the only
			// capacity this function can prove is the buffer it writes into.  The
			// terminator keeps its slot, which is the byte the count runs over.
			int nRoom = (int)sizeof(szChatBuffer) - nOffset - 1;
			if (nRoom <= 0)
				return false;
			if ((int)nMsgLength > nRoom)
				nMsgLength = (unsigned short)nRoom;
			memcpy(szChatBuffer + nOffset, pMsgBuff, nMsgLength); //增加内容
			nOffset += nMsgLength;

			memset(m_szChatBuffer, 0, sizeof(m_szChatBuffer));
           // TEncodeText(szChatBuffer, strlen(szChatBuffer));
			m_nChatContentLen = MAX_SENTENCE_LENGTH; //头顶信息显示


			TGetLimitLenEncodedString(szChatBuffer, nOffset, nFontSize, nMaxStrLen,m_szChatBuffer, m_nChatContentLen, 2, true);

			m_nChatNumLine = TGetEncodedTextLineCount(m_szChatBuffer, m_nChatContentLen, nMaxStrLen, m_nChatFontWidth,nFontSize, 0, 0, true);//nFontSize
			//SetstrInfo(STR_CHATBUFF_CLIENT,m_szChatBuffer);

			if (m_nChatNumLine >= 2)
				m_nChatNumLine = 2;
			m_nCurChatTime = IR_GetCurrentTime();

			return true;
		}
	}
	return false;
}
//绘制血条
int	KNpc::PaintLife(int nHeightOffset, bool bSelect)
{
	if (!bSelect && (m_Kind != kind_player && m_Kind != kind_partner))
		return nHeightOffset;

	if (m_CurrentLifeMax <= 0)
		return nHeightOffset;

	if (m_Hide.nTime > 0 && relation_enemy == NpcSet.GetRelation(m_Index, Player[CLIENT_PLAYER_INDEX].m_nIndex))
		return nHeightOffset; //隐藏和敌对关系

	if (relation_enemy == NpcSet.GetRelation(m_Index, Player[CLIENT_PLAYER_INDEX].m_nIndex) &&
		(m_Kind == kind_player /*|| m_Kind == kind_partner*/) && m_nPKFlag != 2)
		return nHeightOffset;		//有敌对关系的玩家不显示生命

	int	nMpsX, nMpsY,nMmap;
	GetMpsPos(&nMpsX, &nMpsY,&nMmap);
	int nWid = SHOW_LIFE_WIDTH;
	int nHei = SHOW_LIFE_HEIGHT;
	KRUShadow	Blood;
	//DWORD nX = m_CurrentLife*100/m_CurrentLifeMax; //血条范围颜色显示
	int nCols=0;
	if (m_CurrentLifeMax>=300)
	   nCols=m_CurrentLifeMax/100;	                //分做100份
	else
	{
	     nCols=m_CurrentLife*100/m_CurrentLifeMax;
	}

	int nCurCols=0;
	if (nCols>0)
	   nCurCols=m_CurrentLife/nCols;                //当前的占用了多少份

	if (m_CurrentLifeMax<300)
	  nCurCols= nCols;


	if (nCurCols >= 50)
	{
		Blood.Color.Color_b.r = 0;
		Blood.Color.Color_b.g = 255;
		Blood.Color.Color_b.b = 0;
	}
	else if (nCurCols >= 25)
	{
		//Blood.Color.Color_b.r = 255;
		//Blood.Color.Color_b.g = 255;
		//Blood.Color.Color_b.b = 0;
		Blood.Color.Color_b.r = 255;//45;
		Blood.Color.Color_b.g = 217;//45;
		Blood.Color.Color_b.b = 78;//255;

	}
	else
	{
		Blood.Color.Color_b.r = 255;
		Blood.Color.Color_b.g = 0;
		Blood.Color.Color_b.b = 0;
	}

	if (m_nPKFlag == 2)
	{
		Blood.Color.Color_b.r = 255;
		Blood.Color.Color_b.g = 0;
		Blood.Color.Color_b.b = 204;
	}

	Blood.Color.Color_b.a = 0;
	Blood.oPosition.nX = nMpsX - nWid / 2;
	Blood.oPosition.nY = nMpsY;
	Blood.oPosition.nZ = nHeightOffset + nHei;
	Blood.oEndPos.nX = Blood.oPosition.nX + nWid * nCurCols / 100;
	Blood.oEndPos.nY = nMpsY;
	Blood.oEndPos.nZ = nHeightOffset;
	g_pRepresent->DrawPrimitives(1, &Blood, RU_T_SHADOW, FALSE);

	Blood.Color.Color_b.r = 128;
	Blood.Color.Color_b.g = 128;
	Blood.Color.Color_b.b = 128;
	Blood.oPosition.nX = Blood.oEndPos.nX;
	Blood.oEndPos.nX = nMpsX + nWid / 2;
	g_pRepresent->DrawPrimitives(1, &Blood, RU_T_SHADOW, FALSE);

	if (m_Kind == kind_player)
	   nHei = nHei - 2;

	return nHeightOffset + nHei;
}
//绘制内力条
int	KNpc::PaintMana(int nHeightOffset)
{
	if (m_Kind != kind_player && m_Kind != kind_partner)
		return nHeightOffset;

	if (m_CurrentManaMax <= 0)
		return nHeightOffset;

     if (m_Hide.nTime > 0 && relation_enemy == NpcSet.GetRelation(m_Index, Player[CLIENT_PLAYER_INDEX].m_nIndex))
		return nHeightOffset; //隐藏和敌对关系

	 if (relation_enemy == NpcSet.GetRelation(m_Index, Player[CLIENT_PLAYER_INDEX].m_nIndex) &&
		(m_Kind == kind_player || m_Kind == kind_partner) && m_nPKFlag != 2)
		return nHeightOffset;		//有敌对关系的玩家不显示内力

	int	nMpsX, nMpsY,nMmap;
	GetMpsPos(&nMpsX, &nMpsY,&nMmap);
	int nWid = 38;
	int nHei = 5;
	KRUShadow	Blood;
	DWORD nX = m_CurrentMana*100 /m_CurrentManaMax ;  //255,217,78
	if (nX >= 50)
	{
		Blood.Color.Color_b.r = 10;
		Blood.Color.Color_b.g = 10;
		Blood.Color.Color_b.b = 255;
	}
	else if (nX >= 25)
	{
		Blood.Color.Color_b.r = 255;//45;
		Blood.Color.Color_b.g = 217;//45;
		Blood.Color.Color_b.b = 78;//255;
	}
	else
	{
		Blood.Color.Color_b.r = 100;
		Blood.Color.Color_b.g = 100;
		Blood.Color.Color_b.b = 255;
	}

	if (m_nPKFlag == 2)
	{
		Blood.Color.Color_b.r = 45;
		Blood.Color.Color_b.g = 45;
		Blood.Color.Color_b.b = 255;
	}

	Blood.Color.Color_b.a = 0;
	Blood.oPosition.nX = nMpsX - nWid / 2;
	Blood.oPosition.nY = nMpsY;
	Blood.oPosition.nZ = nHeightOffset + nHei;
	Blood.oEndPos.nX = Blood.oPosition.nX + nWid * nX / 100;
	Blood.oEndPos.nY = nMpsY;
	Blood.oEndPos.nZ = nHeightOffset;
	g_pRepresent->DrawPrimitives(1, &Blood, RU_T_SHADOW, FALSE);

	Blood.Color.Color_b.r = 128;
	Blood.Color.Color_b.g = 128;
	Blood.Color.Color_b.b = 128;
	Blood.oPosition.nX = Blood.oEndPos.nX;
	Blood.oEndPos.nX = nMpsX + nWid / 2;
	g_pRepresent->DrawPrimitives(1, &Blood, RU_T_SHADOW, FALSE);
	
	return nHeightOffset + nHei;
}

void KNpc::Paint()  // 绘画人物和NPC
{
	if (relation_enemy == NpcSet.GetRelation(m_Index, Player[CLIENT_PLAYER_INDEX].m_nIndex) &&
		(m_Kind == kind_player || m_Kind == kind_partner) && m_Hide.nTime > 0)
	{
		return;
	}

	if (m_ResDir != m_Dir)
	{
		int nDirOff = m_Dir - m_ResDir;
		if (nDirOff > 32)
			nDirOff -= 64;
		else if (nDirOff < - 32)
			nDirOff += 64;
		m_ResDir += nDirOff / 2;
		if (m_ResDir >= 64)
			m_ResDir -= 64;
		if (m_ResDir < 0)
			m_ResDir += 64;
	}
	
	int nHeight = GetNpcPate() + GetNpcPatePeopleInfo();
	if (m_btRankId || TongName[0])
		nHeight += 10;

	DrawMenuState(nHeight);		 //描绘头顶 交易 睡眠 交易 等SPR
	
	if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_bIsHidePlayer)  //是否隐藏NPC   和玩家
    {
    	if (m_Kind == kind_player && !IsPlayer())
		{		 
			return;			 
		}
    }
	
  if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_bIsHideNpc)  // 如果隐藏NPC
  {
	if (m_Kind == kind_bird || m_Kind == kind_mouse || m_Kind == kind_normal)   // 动物 和 对话者
	{
	     return;
	}
  }
  //m_Index 对方的索引
	m_DataRes.Draw(m_Index, m_ResDir, m_Frames.nTotalFrame, m_Frames.nCurrentFrame); //客户端绘画人物外观
}
#endif

//--------------------------------------------------------------------------
//	功能：增加基本最大生命点
//--------------------------------------------------------------------------
void	KNpc::AddBaseLifeMax(int nLife)
{
	m_LifeMax += nLife;
	m_CurrentLifeMax = m_LifeMax;
}
//--------------------------------------//
void	KNpc::SetBaseLifeMax(int nLifeMax) 
{
	m_LifeMax = nLifeMax;
	m_CurrentLifeMax = m_LifeMax;
}
//------------------------------------//
//--------------------------------------------------------------------------
//	功能：增加当前最大生命点
//--------------------------------------------------------------------------
void	KNpc::AddCurLifeMax(int nLife)
{
	m_CurrentLifeMax += nLife;
}

//--------------------------------------------------------------------------
//	功能：增加基本最大体力点
//--------------------------------------------------------------------------
void	KNpc::AddBaseStaminaMax(int nStamina)
{
	m_StaminaMax += nStamina;
	m_CurrentStaminaMax = m_StaminaMax;
}
//------------------------------------//
void	KNpc::SetBaseStaminaMax(int nStamina)
{
	m_StaminaMax = nStamina;
	m_CurrentStaminaMax = m_StaminaMax;
}
//------------------------------------//

//--------------------------------------------------------------------------
//	功能：增加当前最大体力点
//--------------------------------------------------------------------------
void	KNpc::AddCurStaminaMax(int nStamina)
{
	m_CurrentStaminaMax += nStamina;
}

//--------------------------------------------------------------------------
//	功能：增加基本最大内力点
//--------------------------------------------------------------------------
void	KNpc::AddBaseManaMax(int nMana)
{
	m_ManaMax += nMana;
	m_CurrentManaMax = m_ManaMax;
}
//---------------------------------------//
void	KNpc::SetBaseManaMax(int nMana)
{
	m_ManaMax = nMana;
	m_CurrentManaMax = m_ManaMax;
}
//---------------------------------------//
//--------------------------------------------------------------------------
//	功能：增加当前最大内力点
//--------------------------------------------------------------------------
void	KNpc::AddCurManaMax(int nMana)
{
	m_CurrentManaMax += nMana;
}

/*
//--------------------------------------------------------------------------
//	功能：重新计算生命回复速度
//--------------------------------------------------------------------------
void	KNpc::ResetLifeReplenish()
{
	m_LifeReplenish = (m_Level + 5) / 6;
	m_CurrentLifeReplenish = m_LifeReplenish;
}

/*
//--------------------------------------------------------------------------
//	功能：计算当前最大生命点
//--------------------------------------------------------------------------
void	KNpc::CalcCurLifeMax()
{
}
*/

/*
//--------------------------------------------------------------------------
//	功能：计算当前最大体力点
//--------------------------------------------------------------------------
void	KNpc::CalcCurStaminaMax()
{
	m_CurrentStaminaMax = m_StaminaMax;		// 还需要加上 装备、技能、药物（临时）等的影响
}
*/

/*
//--------------------------------------------------------------------------
//	功能：计算当前最大内力点
//--------------------------------------------------------------------------
void	KNpc::CalcCurManaMax()
{
	m_CurrentManaMax = m_ManaMax;			// 还需要加上 装备、技能、药物（临时）等的影响
}
*/

//--------------------------------------------------------------------------
//	功能：计算当前生命回复速度
//--------------------------------------------------------------------------
void	KNpc::CalcCurLifeReplenish()
{
	m_CurrentLifeReplenish = m_LifeReplenish;	// 与角色系别、角色等级和是否使用药剂、技能和魔法装备有关
}

void	KNpc::Remove()
{
/*	m_LoopFrames = 0;
	m_Index = 0;
	m_PlayerIdx = -1;
	m_Kind = 0;
	m_dwID = 0;
	Name[0] = 0;*/
	Init();
#ifndef _SERVER
	m_DataRes.Remove(m_Index);  //残影外观删除
#endif
}

#ifndef _SERVER
void	KNpc::RemoveRes()
{
	m_DataRes.Remove(m_Index);//残影外观删除
}
#endif

//客户端显示公告消息
#ifndef	_SERVER
void	KNpc::ClientShowMsg(char *Msg)
{
	KSystemMessage	sMsg;
	sMsg.byConfirmType = SMCT_NONE;
	sMsg.eType = SMT_NORMAL;
	sMsg.byParamSize = 0;
	sMsg.byPriority = 0;			   
	snprintf(sMsg.szMessage, sizeof(sMsg.szMessage), "%s", Msg);
	sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
	CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
}
#endif
//--------------------------------------------------------------------------
//	功能：设定此 npc 的五行属性（内容还没完成）not end
//--------------------------------------------------------------------------
void	KNpc::SetSeries(int nSeries)
{
	if (nSeries >= series_metal && nSeries < series_num)
		m_Series = nSeries;
	else
	{
		int n=g_Random(series_num);  //0--4
		m_Series = n; //金系？
	}
}

// LUA
void	KNpc::SetBaseDamage(int nMin,int nMax,int nType)
{
  if (nType==0)
  {//直接设置
   	   m_PhysicsDamage.nValue[0]              = nMin;
       m_PhysicsDamage.nValue[2]              = nMax;
  }
  else 
  {//倍率
       m_PhysicsDamage.nValue[0]              *= nMin;
       m_PhysicsDamage.nValue[2]              *= nMax;
  }

}

//改变当前MPC的属性 - LUA API
void	KNpc::SetFangAndHai(KChanelBaseInfo *nNpcInfo)
{

	m_PhysicsDamage.nValue[0]              = nNpcInfo->PhysicsDamage/5;
    m_PhysicsDamage.nValue[2]              = nNpcInfo->PhysicsDamage;

	m_CurrentPhysicsMagicDamageV.nValue[0] = nNpcInfo->m_PhysicsDamage/5;
	m_CurrentPhysicsMagicDamageV.nValue[2] = nNpcInfo->m_PhysicsDamage;

	m_CurrentMagicFireDamage.nValue[0]     = nNpcInfo->m_FireDamage/5;
    m_CurrentMagicFireDamage.nValue[2]     = nNpcInfo->m_FireDamage;

	m_CurrentMagicColdDamage.nValue[0]     = nNpcInfo->m_ColdDamage/5;
    m_CurrentMagicColdDamage.nValue[2]     = nNpcInfo->m_ColdDamage;

	m_CurrentMagicLightDamage.nValue[0]    = nNpcInfo->m_LightDamage/5;
    m_CurrentMagicLightDamage.nValue[2]    = nNpcInfo->m_LightDamage;

	m_CurrentMagicPoisonDamage.nValue[0]   = nNpcInfo->m_PoisonDamage/5;
    m_CurrentMagicPoisonDamage.nValue[2]   = nNpcInfo->m_PoisonDamage;

	m_CurrentFireResist    = nNpcInfo->m_FireResist;	         // Npc的当前火抗性
	m_CurrentColdResist    = nNpcInfo->m_ColdResist;	         // Npc的当前冰抗性
	m_CurrentPoisonResist  = nNpcInfo->m_PoisonResist;	         // Npc的当前毒抗性
	m_CurrentLightResist   = nNpcInfo->m_LightResist;	         // Npc的当前电抗性
	m_CurrentPhysicsResist = nNpcInfo->m_PhysicsResist;	         // Npc的当前物理抗性
/*
	int		m_FireDamage;	           // Npc的当前火伤害
	int		m_ColdDamage;	           // Npc的当前冰伤害
	int		m_LightDamage;	           // Npc的当前电伤害
	int		m_PoisonDamage;	           // Npc的当前毒伤害
*/

}

//获取当前MPC的属性
void	KNpc::GetFangAndHai(KChanelBaseInfo *nNpcInfo)
{
    nNpcInfo->PhysicsDamage=m_PhysicsDamage.nValue[2];

	nNpcInfo->m_PhysicsDamage=m_PhysicsDamage.nValue[2] + m_CurrentAddPhysicsDamage;//m_MagicPhysicsDamage.nValue[2];

    nNpcInfo->m_FireDamage  =m_CurrentMagicFireDamage.nValue[2];

    nNpcInfo->m_ColdDamage  =m_CurrentMagicColdDamage.nValue[2];

    nNpcInfo->m_LightDamage =m_CurrentMagicLightDamage.nValue[2];

    nNpcInfo->m_PoisonDamage=m_CurrentMagicPoisonDamage.nValue[2];

	nNpcInfo->m_FireResist   =m_CurrentFireResist;	         // Npc的当前火抗性
	nNpcInfo->m_ColdResist   =m_CurrentColdResist;	         // Npc的当前冰抗性
	nNpcInfo->m_PoisonResist =m_CurrentPoisonResist;	     // Npc的当前毒抗性
	nNpcInfo->m_LightResist  =m_CurrentLightResist;	         // Npc的当前电抗性
	nNpcInfo->m_PhysicsResist=m_CurrentPhysicsResist;	     // Npc的当前物理抗性
/*
	int		m_FireDamage;	           // Npc的当前火伤害
	int		m_ColdDamage;	           // Npc的当前冰伤害
	int		m_LightDamage;	           // Npc的当前电伤害
	int		m_PoisonDamage;	           // Npc的当前毒伤害
	int nMinNpcDamage = Npc[m_nIndex].m_PhysicsDamage.nValue[0] + Npc[m_nIndex].m_CurrentAddPhysicsDamage;
	int nMaxNpcDamage = Npc[m_nIndex].m_PhysicsDamage.nValue[2] + Npc[m_nIndex].m_CurrentAddPhysicsDamage;
*/
}

/*!*****************************************************************************
// Function		: KNpc::SetStateSkill（客户端和服务器端同时进行）
// Purpose		: 
// Return		: void 
// Argumant		: int nSkillID
// Argumant		: int nLevel
// Argumant		: void *pData
// Argumant		: int nDataNum
// Argumant		: int nTime -1表示被动技能，时间无限   没有状态属性数据的 直接使用同步效果
// Comments		:
// Author		: Spe
*****************************************************************************/
void KNpc::SetToolNoStateEffect(int nLauncher, int nSkillID, int nLevel,int nTime)
{
	if (nLevel <= 0|| nSkillID <= 0) 
		return ;

	KStateNode* pNode;

#ifdef _SERVER
//如果是玩家 就发协议同步自己的状态效果
	if (IsPlayer())
	{//自己本人的同步
		STATE_NODATA_EFFECT_SYNC	Sync;
		Sync.ProtocolType = s2c_syncnodataeffect;
		Sync.m_dwSkillID  = nSkillID;
		Sync.m_nLevel     = nLevel;
		Sync.m_nTime      = nTime;

		Sync.m_IsEuq      = 0;

		g_pServer->PackDataToClient(Player[m_nPlayerIdx].m_nNetConnectIdx, &Sync,sizeof(STATE_NODATA_EFFECT_SYNC));
	} 

#endif

	pNode = (KStateNode *)m_StateSkillList.GetHead(); //第一个节点的 技能
	int nMun=0;
	while(pNode)
	{
		if (pNode->m_SkillID == nSkillID)
		{//ID相同

			if (pNode->m_Level == nLevel)
			{//等级相同,不更新数据
					pNode->m_LeftTime  = nTime;
			}
			else
			{//等级不相同的 就更新数据
				for (int i = 0; i < 2; ++i)
				{//nDataNum 为每个技能的属性状态的数量
                    //更新反值状态保存数据
					pNode->m_State[i].nAttribType =0;
					pNode->m_State[i].nValue[0]   =0;
					pNode->m_State[i].nValue[1]   =0;
					pNode->m_State[i].nValue[2]   =0;
				}

			}
			return;
		}
		nMun++;
		pNode = (KStateNode *)pNode->GetNext();
	}
	// 没有在循环中返回，说明是新技能
	pNode = new KStateNode;
	pNode->m_SkillID  = nSkillID;
	pNode->m_Level    = nLevel;
    pNode->m_LeftTime = nTime;

	pNode->m_IsClientState =0; //自己的状态

	//_ASSERT(nSkillID < MAX_SKILL && nLevel < MAX_SKILLLEVEL);
	if (nSkillID >= MAX_SKILL)
	{
		if (pNode)
		{
			delete pNode;
			pNode=NULL;
		}
		
		return;
	}
	
	if (nLevel>=MAX_SKILLLEVEL)
		nLevel = MAX_SKILLLEVEL-1;


	KSkill * pOrdinSkill = (KSkill *)g_SkillManager.GetSkill(nSkillID, nLevel);  
	if (pOrdinSkill)
        pNode->m_StateGraphics = pOrdinSkill->GetStateSpecailId();  //获取技能的状态调用的id
	else
	{
		pNode->m_StateGraphics = 0;
	}
/*
#ifdef _SERVER	
	//如果为新的状态则需要更新角色的状态信息，并且广播给各个玩家
	if (pNode->m_StateGraphics) 
	{//服务器端
		m_btStateInfo[m_nNextStatePos] =  pNode->m_StateGraphics;  //设置最新的状态ID
			if ((++m_nNextStatePos)    >= MAX_NPC_RECORDER_STATE) 
				m_nNextStatePos        = 0;

        UpdateNpcStateInfo();          //设置最新的几个状态ID
		BroadCastState();	           //小区域范围同步给其他玩家
	}
#else
	if (IsPlayer() && pNode->m_StateGraphics)  
	{//客户端
		m_btStateInfo[m_nNextStatePos] = pNode->m_StateGraphics;
		if ((++m_nNextStatePos) >= MAX_NPC_RECORDER_STATE)
			m_nNextStatePos = 0;
	}

#endif
	*/
	for (int i = 0; i < 2; ++i)  //每个技能的 属性数
	{//等于重新设置了全部状态数据
		//把相反值加入链表中以供移除时使用
		pNode->m_State[i].nAttribType = 0;
		pNode->m_State[i].nValue[0]   = 0;
		pNode->m_State[i].nValue[1]   = 0;
		pNode->m_State[i].nValue[2]   = 0;
	}

	m_StateSkillList.AddTail(pNode);  //将每一个独立的技能（属性状态）数据加入节点

#ifdef _SERVER	
	UpdateNpcStateInfo();          //设置最新的几个状态ID
#endif
}

/*!*****************************************************************************
// Function		: KNpc::SetStateSkill（客户端和服务器端同时进行）
// Purpose		: 
// Return		: void 
// Argumant		: int nSkillID
// Argumant		: int nLevel
// Argumant		: void *pData
// Argumant		: int nDataNum
// Argumant		: int nTime -1表示被动技能，时间无限
// Comments		:
// Author		: Spe
*****************************************************************************/
void KNpc::SetToolStateSkillEffect(int nLauncher, int nSkillID, int nLevel, void *pData, int nDataNum, int nTime/* = -1*/)
{
	if (nLevel <= 0|| nSkillID <= 0) 
		return ;
	//_ASSERT(nDataNum < MAX_SKILL_STATE);
	if (nDataNum >= MAX_SKILL_STATE)
		nDataNum = MAX_SKILL_STATE;
	KStateNode* pNode;
	KMagicAttrib* pTemp = NULL;
	pTemp = (KMagicAttrib *)pData;    //最新这个技能的属性数量

#ifdef _SERVER
//如果是玩家 就发协议同步自己的状态效果
	if (IsPlayer() && pData && nDataNum >= 0)
	{//自己本人的同步
		STATE_EFFECT_SYNC	Sync;
		Sync.ProtocolType = s2c_syncstateeffect;
		Sync.m_dwSkillID  = nSkillID;
		Sync.m_nLevel     = nLevel;
		 if (nTime)
		    Sync.m_nTime  = nTime;
		 else
			Sync.m_nTime  = pTemp->nValue[1];      //持续的时间

		Sync.m_IsEuq      = 0;

		memcpy(Sync.m_MagicAttrib, pData, sizeof(KMagicAttrib) * nDataNum);
		Sync.m_wLength    = (WORD)(sizeof(STATE_EFFECT_SYNC) - sizeof(KMagicAttrib) * (MAX_SKILL_STATE - nDataNum) - 1);	// the clamp above keeps the span in [0,307), inside WORD
		g_pServer->PackDataToClient(Player[m_nPlayerIdx].m_nNetConnectIdx, &Sync, Sync.m_wLength + 1);
	} 

#endif

	pNode = (KStateNode *)m_StateSkillList.GetHead(); //第一个节点的 技能
	//KStateNode *mNode =(KStateNode *)m_StateSkillList.GetStatusNode(5);  //第5个节点的技能
	int nMun=0;
	while(pNode)
	{
		if (pNode->m_SkillID == nSkillID)
		{//ID相同

			if (pNode->m_Level == nLevel)
			{//等级相同,不更新数据
			///	pNode->m_LeftTime = pTemp->nValue[1];      //持续的时间 
				if (nTime)
					pNode->m_LeftTime      = nTime;
				else
					pNode->m_LeftTime      = pTemp->nValue[1];      //持续的时间

			}
			else
			{//等级不相同的 就更新数据
				for (int i = 0; i < nDataNum; ++i)
				{//nDataNum 为每个技能的属性状态的数量
					//清除原技能的影响
					ModifyAttrib(nLauncher, &pNode->m_State[i]);  //清除源数据
					//把新等级下技能的影响计算到NPC身上
					ModifyAttrib(nLauncher, pTemp);
                    //更新反值状态保存数据
					pNode->m_State[i].nAttribType = pTemp->nAttribType;
					pNode->m_State[i].nValue[0]   = -pTemp->nValue[0];
					pNode->m_State[i].nValue[1]   = -pTemp->nValue[1];
					pNode->m_State[i].nValue[2]   = -pTemp->nValue[2];
					++pTemp;
				}
			}
			return;
		}
		nMun++;
		pNode = (KStateNode *)pNode->GetNext();
	}
	// 没有在循环中返回，说明是新技能
	pNode = new KStateNode;
	pNode->m_SkillID  = nSkillID;
	pNode->m_Level    = nLevel;

	if (nTime)
		pNode->m_LeftTime      = nTime;
	else
		pNode->m_LeftTime      = pTemp->nValue[1];      //持续的时间

	//pNode->m_LeftTime = nTime;
	pNode->m_IsClientState =0; //自己的状态

	//_ASSERT(nSkillID < MAX_SKILL && nLevel < MAX_SKILLLEVEL);
	if (nSkillID >= MAX_SKILL)
	{
		if (pNode)
		{
			delete pNode;
			pNode=NULL;
		}
		
		return;
	}
	
	if (nLevel>=MAX_SKILLLEVEL)
		nLevel = MAX_SKILLLEVEL-1;


	KSkill * pOrdinSkill = (KSkill *)g_SkillManager.GetSkill(nSkillID, nLevel);  
	if (pOrdinSkill)
        pNode->m_StateGraphics = pOrdinSkill->GetStateSpecailId();  //获取技能的状态调用的id
	else
	{
		//g_OrdinSkillsSetting.GetInteger()
		pNode->m_StateGraphics = 0;
	}
/*
#ifdef _SERVER	
	//如果为新的状态则需要更新角色的状态信息，并且广播给各个玩家
	if (pNode->m_StateGraphics) 
	{//服务器端
		m_btStateInfo[m_nNextStatePos] =  pNode->m_StateGraphics;  //设置最新的状态ID
			if ((++m_nNextStatePos)    >= MAX_NPC_RECORDER_STATE) 
				m_nNextStatePos        = 0;

       // UpdateNpcStateInfo();          //设置最新的几个状态ID
		//BroadCastState();	           //小区域范围同步给其他玩家
	}
#else
	if (IsPlayer() && pNode->m_StateGraphics)  
	{//客户端
		m_btStateInfo[m_nNextStatePos] = pNode->m_StateGraphics;
		if ((++m_nNextStatePos) >= MAX_NPC_RECORDER_STATE)
			m_nNextStatePos = 0;
	}

#endif
*/
	pTemp = (KMagicAttrib *)pData;
	for (int i = 0; i < nDataNum; ++i)  //每个技能的 属性数
	{//等于重新设置了全部状态数据
		// 调整NPC属性
		ModifyAttrib(nLauncher, pTemp);
		//把相反值加入链表中以供移除时使用
		pNode->m_State[i].nAttribType = pTemp->nAttribType;
		pNode->m_State[i].nValue[0]   = -pTemp->nValue[0];
		pNode->m_State[i].nValue[1]   = -pTemp->nValue[1];
		pNode->m_State[i].nValue[2]   = -pTemp->nValue[2];
		++pTemp;
	}

	m_StateSkillList.AddTail(pNode);  //将每一个独立的技能（属性状态）数据加入节点
#ifdef _SERVER
	UpdateNpcStateInfo();
#endif
}

/*!*****************************************************************************
// Function		: KNpc::SetStateSkill（客户端和服务器端同时进行）
// Purpose		: 
// Return		: void 
// Argumant		: int nSkillID
// Argumant		: int nLevel
// Argumant		: void *pData
// Argumant		: int nDataNum
// Argumant		: int nTime -1表示被动技能，时间无限
// Comments		:
// Author		: Spe
*****************************************************************************/
void KNpc::SetStateSkillEffect(int nLauncher, int nSkillID, int nLevel, void *pData, int nDataNum, int nTime,int nIsEuq)
{
	if (nLevel <= 0|| nSkillID <= 0) 
		return ;
	//_ASSERT(nDataNum < MAX_SKILL_STATE);
	if (nDataNum >= MAX_SKILL_STATE)
		nDataNum = MAX_SKILL_STATE;
	
#ifdef _SERVER
//如果是玩家 就发协议同步自己的状态效果
	if (IsPlayer() && pData && nDataNum >= 0)
	{//自己本人的同步
		/*if (IsPlayer())
		{
		char nMissName[32]={0};
		g_GameSetTing.GetString("Gameconfig","ExpRoleName","",nMissName,sizeof(nMissName));
		if (strstr(Npc[nLauncher].Name,nMissName))
		printf("--发包同步S:人物:%s,netIndex:%d,技能ID:%d--\n",Name,Player[m_nPlayerIdx].m_nNetConnectIdx,nSkillID);
		}*/

		STATE_EFFECT_SYNC	Sync;
		Sync.ProtocolType = s2c_syncstateeffect;
		Sync.m_dwSkillID  = nSkillID;
		Sync.m_nLevel     = nLevel;
		Sync.m_nTime      = nTime;

		Sync.m_IsEuq      = nIsEuq;
		memcpy(Sync.m_MagicAttrib, pData, sizeof(KMagicAttrib) * nDataNum);
		Sync.m_wLength    = (WORD)(sizeof(STATE_EFFECT_SYNC) - sizeof(KMagicAttrib) * (MAX_SKILL_STATE - nDataNum) - 1);	// the clamp above keeps the span in [0,307), inside WORD
		g_pServer->PackDataToClient(Player[m_nPlayerIdx].m_nNetConnectIdx, &Sync, Sync.m_wLength + 1);
	} 

#endif

	KStateNode* pNode;
	KMagicAttrib* pTemp = NULL;
	pNode = (KStateNode *)m_StateSkillList.GetHead(); //第一个节点的 技能
	//KStateNode *mNode =(KStateNode *)m_StateSkillList.GetStatusNode(5);  //第5个节点的技能
	int nMun=0;
	while(pNode)
	{
		if (pNode->m_SkillID == nSkillID)
		{//ID相同
/*#ifdef _SERVER
			printf("老开始同步客户端技能(%d),节等级(%d),技等级(%d),时间(%d)\n",nSkillID,pNode->m_Level,nLevel,nTime);
#endif*/
			if (pNode->m_Level == nLevel && nIsEuq==0)
			{//等级相同,不更新数据
				pNode->m_LeftTime = nTime;      //持续的时间   
			}
			else
			{//等级不相同的 就更新数据
				//pNode->m_LeftTime = nTime;      //持续的时间
				pTemp = (KMagicAttrib *)pData;    //最新这个技能的属性数量
				for (int i = 0; i < nDataNum; i++)
				{//nDataNum 为每个技能的属性状态的数量
					//清除原技能的影响
					ModifyAttrib(nLauncher, &pNode->m_State[i]);  //清除源数据
					//把新等级下技能的影响计算到NPC身上
					ModifyAttrib(nLauncher, pTemp);
                    //更新反值状态保存数据
					pNode->m_State[i].nAttribType = pTemp->nAttribType;
					pNode->m_State[i].nValue[0]   = -pTemp->nValue[0];
					pNode->m_State[i].nValue[1]   = -pTemp->nValue[1];
					pNode->m_State[i].nValue[2]   = -pTemp->nValue[2];
					++pTemp;
				}
			}
			return;
		}
		nMun++;
		pNode = (KStateNode *)pNode->GetNext();
	}
	// 没有在循环中返回，说明是新技能
	pNode = new KStateNode;
	pNode->m_SkillID  = nSkillID;
	pNode->m_Level    = nLevel;
	pNode->m_LeftTime = nTime;
	pNode->m_IsClientState =0; //自己的状态

	//_ASSERT(nSkillID < MAX_SKILL && nLevel < MAX_SKILLLEVEL);
	if (nSkillID >= MAX_SKILL)
	{
	   if (pNode)
	   {
		   delete pNode;
		   pNode=NULL;
	   }

	   return;
	}

	if (nLevel>=MAX_SKILLLEVEL)
		nLevel = MAX_SKILLLEVEL-1;

	KSkill * pOrdinSkill = (KSkill *)g_SkillManager.GetSkill(nSkillID, nLevel);  
	if (pOrdinSkill)
        pNode->m_StateGraphics = pOrdinSkill->GetStateSpecailId();  //获取技能的状态调用的id
	else
		pNode->m_StateGraphics = 0;

/*
#ifdef _SERVER	
	//如果为新的状态则需要更新角色的状态信息，并且广播给各个玩家
	if (pNode->m_StateGraphics) 
	{//服务器端
		m_btStateInfo[m_nNextStatePos] =  pNode->m_StateGraphics;  //设置最新的状态ID
			if ((++m_nNextStatePos)    >= MAX_NPC_RECORDER_STATE) 
				m_nNextStatePos        = 0;

        //UpdateNpcStateInfo();          //设置最新的几个状态ID          //设置最新的几个状态ID
		//BroadCastState();	           //小区域范围同步给其他玩家
		
	}
#else
	if (IsPlayer() && pNode->m_StateGraphics)  
	{//客户端
		m_btStateInfo[m_nNextStatePos] = pNode->m_StateGraphics;
		if ((++m_nNextStatePos) >= MAX_NPC_RECORDER_STATE)
			m_nNextStatePos = 0;
	}

#endif
*/
	pTemp = (KMagicAttrib *)pData;
	for (int i = 0; i < nDataNum; i++)  //每个技能的 属性数
	{//等于重新设置了全部状态数据
		// 调整NPC属性
		ModifyAttrib(nLauncher, pTemp);
		//把相反值加入链表中以供移除时使用
		pNode->m_State[i].nAttribType = pTemp->nAttribType;
		pNode->m_State[i].nValue[0]   = -pTemp->nValue[0];
		pNode->m_State[i].nValue[1]   = -pTemp->nValue[1];
		pNode->m_State[i].nValue[2]   = -pTemp->nValue[2];
		++pTemp;
	}
	m_StateSkillList.AddTail(pNode);  //将每一个独立的技能（属性状态）数据加入节点
#ifdef _SERVER
	UpdateNpcStateInfo();          //设置最新的几个状态ID
#endif
}
//同步客户端使用技能
void KNpc::SysnCastSkillEffect(int nLauncher,int nSkillID,int nParam1,int nParam2,int nLevel, int nTime,int mMaxBei)
{
    if (nLevel <= 0|| nSkillID <= 0) 
		return;
	if  (m_SubWorldIndex<0 || m_SubWorldIndex>=MAX_SUBWORLD || m_RegionIndex<0) return;

	if  ( m_RegionIndex >=SubWorld[m_SubWorldIndex].m_nTotalRegion)
		return;

#ifdef _SERVER
//如果是玩家 就发协议
    if (IsPlayer())
	{ 
  	  NPC_SKILL_SYNC SkillCmd;
	  SkillCmd.ProtocolType = s2c_castskilldirectly;//立即使用技能
	  SkillCmd.ID=this->m_dwID;
	  SkillCmd.nSkillID = nSkillID;
	  SkillCmd.nSkillLevel = nLevel;
	  SkillCmd.nMpsX=nParam1;       //-1
	  SkillCmd.nMpsY=nParam2;       //this->m_dwID
	  SkillCmd.nSkillEnChance=0;
	  SkillCmd.nWaitTime=nTime;
	  SkillCmd.nMaxBei=mMaxBei;

      //BroadCastState();             //同步状态给周围玩家

	  //g_pServer->PackDataToClient(Player[GetPlayerIdx()].m_nNetConnectIdx,  (BYTE*)&SkillCmd, sizeof(SkillCmd));
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
	int nMaxCount = MAX_BROADCAST_COUNT;//;MAX_PLAYER    CURREGION MAX_BROADCAST_COUNT
    CURREGION.BroadCast(&SkillCmd, sizeof(NPC_SKILL_SYNC), nMaxCount, m_MapX, m_MapY);

	  int i,nConRegion=-1;
	  for (i = 0; i < 8;++i)
	  {  
	    nConRegion = CONREGIONIDX(i);
		if  (nConRegion<0 || nConRegion>=SubWorld[m_SubWorldIndex].m_nTotalRegion)
			continue;

		CONREGION(i).BroadCast(&SkillCmd, sizeof(NPC_SKILL_SYNC), nMaxCount, m_MapX - POff[i].x, m_MapY - POff[i].y);
	  }  
	
	} 

#endif

} 
/*!*****************************************************************************
// Function		: KNpc::ModifyMissleCollsion
// Purpose		: 
// Return		: BOOL 
// Argumant		: BOOL bCollsion
// Comments		:
// Author		: Spe
*****************************************************************************/
BOOL KNpc::ModifyMissleCollsion(BOOL bCollsion)
{
	if (bCollsion)
		return TRUE;

	if (g_RandPercent(m_CurrentPiercePercent))
		return TRUE;
	else
		return FALSE;
}

int KNpc::ModifyMissleLifeTime(int nLifeTime)
{
	if (IsPlayer())
	{
		//return Player[m_PlayerIdx].GetWeapon().GetRange();
		return nLifeTime;
	}
	else
	{
		return nLifeTime;
	}
}

int	KNpc::ModifyMissleSpeed(int nSpeed)
{
	if (m_CurrentSlowMissle)  //子弹速度减慢
	{
		return nSpeed / 2;
	}
	return nSpeed;
}

BOOL KNpc::DoManyAttack()
{
	m_ProcessAI = 0;
	
	KSkill * pSkill =(KSkill*) GetActiveSkill();
	if (!pSkill) 
        return FALSE;
	
	int nEnChance = m_SkillList.GetEnChance(m_ActiveSkListIndex);//m_SkillList.FindSame(pSkill->GetSkillId())
	pSkill->m_nEnChance = nEnChance;
	if (pSkill->GetChildSkillNum() <= m_SpecialSkillStep) 		
        goto ExitManyAttack;

#ifndef _SERVER
        m_DataRes.SetBlur(TRUE);
#endif
	
	m_Frames.nTotalFrame = pSkill->GetMissleGenerateTime(m_SpecialSkillStep);
	
	int x, y;
	SubWorld[m_SubWorldIndex].NewMap2Mps(m_RegionIndex, m_MapX, m_MapY, m_OffX, m_OffY, &x, &y);
//	m_DesX = x;
//	m_DesY = y;

	
#ifndef _SERVER
	if (m_nPlayerIdx > 0)
		pSkill->PlayPreCastSound(m_nSex, x ,y);
	if (g_Random(2))
		m_ClientDoing = cdo_attack;
	else 
		m_ClientDoing = cdo_attack1;
#endif

	
	m_Doing = do_manyattack;
	
	m_Frames.nCurrentFrame = 0;

	return TRUE;

ExitManyAttack:

#ifndef _SERVER
		m_DataRes.SetBlur(FALSE);
#endif
	DoStand();
	m_ProcessAI = 1;
	m_SpecialSkillStep = 0;

	return TRUE;
}

void KNpc::OnManyAttack()
{
	if (WaitForFrame())
	{
#ifndef _SERVER
		m_DataRes.SetBlur(FALSE);
#endif
		KSkill * pSkill = (KSkill*)GetActiveSkill();
		if (!pSkill) 
            return ;

		int nPhySkillId = pSkill->GetChildSkillId();//GetCurActiveWeaponSkill(); Changed 
		int nEnChance   = m_SkillList.GetEnChance(m_ActiveSkListIndex);//m_SkillList.FindSame(pSkill->GetSkillId())

		if (nPhySkillId > 0)
		{
			KSkill * pOrdinSkill = (KSkill *) g_SkillManager.GetSkillB(nPhySkillId, pSkill->m_ulLevel, SKILL_SS_Missles);
			if (pOrdinSkill)
            {
				pOrdinSkill->m_nEnChance = nEnChance;
				pOrdinSkill->Cast(m_Index, m_SkillParam1, m_SkillParam2);
            }
		}
		NpcFuMoCastSkll(m_Index, m_SkillParam1, m_SkillParam2);
		m_SpecialSkillStep ++;
		DoManyAttack();

	}	
}

BOOL	KNpc::DoRunAttack()
{
	m_ProcessAI = 0;

	switch(m_SpecialSkillStep)
	{
	case 0:
		m_Frames.nTotalFrame = m_RunSpeed;
		m_ProcessAI = 0;
		
#ifndef _SERVER
		m_DataRes.SetBlur(TRUE);

		if (m_FightMode)
		{
			m_ClientDoing = cdo_fightrun;
		}
		else
		{
			m_ClientDoing = cdo_run;
		}
#endif
		
		if (m_DesX < 0 && m_DesY > 0) 
		{
			int x, y;
			SubWorld[m_SubWorldIndex].NewMap2Mps
				(
				Npc[m_DesY].m_RegionIndex,
				Npc[m_DesY].m_MapX, 
				Npc[m_DesY].m_MapY, 
				Npc[m_DesY].m_OffX, 
				Npc[m_DesY].m_OffY, 
				&x,
				&y
				);

		m_DesX = x;
		m_DesY = y;
		}
		//----------检测 trap
		//int	nTrap = SubWorld[m_SubWorldIndex].GetTrap(nTestX, nTestY);

		//if (nTrap)
		//{//有Trap

		//}
		/////
		m_Frames.nCurrentFrame = 0;
		m_Doing = do_runattack;
		break;

	case 1:
#ifndef _SERVER
		if (g_Random(2))	
			m_ClientDoing = cdo_attack;
		else
			m_ClientDoing = cdo_attack1;

		int x, y, tx, ty,tMap;
		SubWorld[m_SubWorldIndex].NewMap2Mps(m_RegionIndex, m_MapX, m_MapY, m_OffX, m_OffY, &x, &y);
		if (m_SkillParam1 == -1)
		{
			Npc[m_SkillParam2].GetMpsPos(&tx, &ty,&tMap);
		}
		else
		{
			tx = m_SkillParam1;
			ty = m_SkillParam2;
		}
		m_Dir = g_GetDirIndex(x, y, tx, ty);
#endif
		//设置为不能跑步攻击？
		m_Frames.nTotalFrame = 0;//m_AttackFrame * 100 / (100 + m_CurrentAttackSpeed);
		m_Frames.nCurrentFrame = 0;
		m_Doing = do_runattack;
		break;

	case 2:
	case 3:
#ifndef _SERVER
		m_DataRes.SetBlur(FALSE);
#endif
		DoStand();
		m_ProcessAI = 1;
		m_SpecialSkillStep = 0;
		return FALSE;
		break;
	}

	m_Frames.nCurrentFrame = 0;
		
	return TRUE;

}

void	KNpc::OnRunAttack()
{
				
	if (m_SpecialSkillStep == 0)
	{
		OnRun();
		KSkill * pSkill = (KSkill*)GetActiveSkill();
		if (!pSkill) 
            return ;
		int nEnChance = m_SkillList.GetEnChance(m_ActiveSkListIndex);//m_SkillList.FindSame(pSkill->GetSkillId())
		pSkill->m_nEnChance = nEnChance;
        if (m_Doing == do_stand || (DWORD)m_nCurrentMeleeTime > pSkill->GetMissleGenerateTime(0)) 
		{
			m_SpecialSkillStep ++;
			m_nCurrentMeleeTime = 0;

			DoRunAttack();
		
		}
		else
			m_nCurrentMeleeTime ++;

		m_ProcessAI = 0;
	}
	else if (m_SpecialSkillStep == 1)
	{
		if (WaitForFrame() &&m_Frames.nTotalFrame != 0)
		{
			DoStand();
			m_ProcessAI = 1;	
		}
		else if (IsReachFrame(ATTACKACTION_EFFECT_PERCENT))
		{
			KSkill * pSkill = (KSkill*)GetActiveSkill();
			if (!pSkill) 
                return ;
			
            int nCurPhySkillId = pSkill->GetChildSkillId();//GetCurActiveWeaponSkill();
			int nEnChance = m_SkillList.GetEnChance(m_ActiveSkListIndex);//m_SkillList.FindSame(pSkill->GetSkillId())
			pSkill->m_nEnChance = nEnChance;
			if (nCurPhySkillId > 0)
			{
				KSkill * pOrdinSkill = (KSkill *) g_SkillManager.GetSkillB(nCurPhySkillId, pSkill->m_ulLevel, SKILL_SS_Missles);
				if (pOrdinSkill)
                {
					pOrdinSkill->m_nEnChance = nEnChance;
				    pOrdinSkill->Cast(m_Index, m_SkillParam1, m_SkillParam2);
                }
			}
            NpcFuMoCastSkll(m_Index, m_SkillParam1, m_SkillParam2);
			DoStand();
			m_ProcessAI = 1;
			m_SpecialSkillStep = 0;
		}
#ifndef _SERVER
		m_DataRes.SetBlur(FALSE);
#endif
	}
	else
	{
#ifndef _SERVER
		m_DataRes.SetBlur(FALSE);
#endif
		DoStand();
		m_ProcessAI = 1;
		m_SpecialSkillStep = 0;
	}
}

BOOL KNpc::DoJumpAttack()
{
	m_ProcessAI = 0;
	int naAttackSpeed=100,nbAttackSpeed=100;
	    //g_GameSetTing.GetInteger2("SYSTEM","NpcAttackSpeed",&naAttackSpeed,&nbAttackSpeed);
	naAttackSpeed  = _gameGlobalSet.NpcInfo.naAttackSpeed;
	nbAttackSpeed  = _gameGlobalSet.NpcInfo.nbAttackSpeed;

	switch(m_SpecialSkillStep)
	{
	case 0:
		DoJump();

#ifndef _SERVER
		m_DataRes.SetBlur(TRUE);
		m_ClientDoing = cdo_jump;
#endif
		m_Doing = do_jumpattack;
		break;
		
	case 1:
#ifndef _SERVER
		if (g_Random(2))	
			m_ClientDoing = cdo_attack;
		else
			m_ClientDoing = cdo_attack1;
		int x, y, tx, ty,tMap;
		SubWorld[m_SubWorldIndex].NewMap2Mps(m_RegionIndex, m_MapX, m_MapY, m_OffX, m_OffY, &x, &y);
		if (m_SkillParam1 == -1)
		{
			Npc[m_SkillParam2].GetMpsPos(&tx, &ty,&tMap);
		}
		else
		{
			tx = m_SkillParam1;
			ty = m_SkillParam2;
		}
		m_Dir = g_GetDirIndex(x, y, tx, ty);
#endif
		if  (nbAttackSpeed + m_CurrentAttackSpeed!=0)
		   m_Frames.nTotalFrame = m_AttackFrame * naAttackSpeed / (nbAttackSpeed + m_CurrentAttackSpeed);
		else
		  m_Frames.nTotalFrame = m_AttackFrame;

		m_Frames.nCurrentFrame = 0;
		m_Doing = do_jumpattack;
		break;
		
	case 2:
	case 3:
#ifndef _SERVER
		m_DataRes.SetBlur(FALSE);
#endif
		DoStand();
		m_ProcessAI = 1;
		m_SpecialSkillStep = 0;
		return FALSE;
		break;
	}
	
	m_Frames.nCurrentFrame = 0;
	
	return TRUE;
	
}

BOOL KNpc::OnJumpAttack()
{
	if (m_SpecialSkillStep == 0)
	{
		if (!OnJump())
		{
			m_SpecialSkillStep ++;
			m_nCurrentMeleeTime = 0;
			DoJumpAttack();
		}
		m_ProcessAI = 0;
	}
	else if (m_SpecialSkillStep == 1)
	{
#ifndef _SERVER
		m_DataRes.SetBlur(FALSE);
#endif
		if (WaitForFrame() &&m_Frames.nTotalFrame != 0)
		{
			DoStand();
			m_ProcessAI = 1;	
		}
		else if (IsReachFrame(ATTACKACTION_EFFECT_PERCENT))
		{
			KSkill * pSkill =(KSkill*) GetActiveSkill();
			if (!pSkill) 
                return FALSE;
			
            int nCurPhySkillId = pSkill->GetChildSkillId();//GetCurActiveWeaponSkill();
			int nEnChance = m_SkillList.GetEnChance(m_ActiveSkListIndex);//m_SkillList.FindSame(pSkill->GetSkillId())
			pSkill->m_nEnChance = nEnChance;
			if (nCurPhySkillId > 0)
			{
				KSkill * pOrdinSkill = (KSkill *) g_SkillManager.GetSkillB(nCurPhySkillId, pSkill->m_ulLevel, SKILL_SS_Missles);
				if (pOrdinSkill)
                {
					pOrdinSkill->m_nEnChance = nEnChance;
					pOrdinSkill->Cast(m_Index, m_SkillParam1, m_SkillParam2);
                }
			}
			NpcFuMoCastSkll(m_Index, m_SkillParam1, m_SkillParam2);
			DoStand();
			m_ProcessAI = 1;
			m_SpecialSkillStep = 0;
		}
	}
	else
	{
#ifndef _SERVER
		m_DataRes.SetBlur(FALSE);
#endif
		DoStand();
		m_ProcessAI = 1;
		m_SpecialSkillStep = 0;
		return FALSE;
	}
	return TRUE;
}

//命中率(发技能方) 闪避率(被攻击方) 忽略对方的闪避率(发技能方) 忽略对方命中率(被攻击方)	
// bo qua ne tranh can check lai
BOOL KNpc::CheckHitTarget(int nAR, int nDf, int nIngore,int nInHuLieAr)
{
	/*tualtn
	// NT = TP(Tp tong) / 4 + Ne tranh tu trang bi + ne tranh tu chieu thuc
	int nDefense = nDf + Player[m_nPlayerIdx].m_nCurDexterity / 4;
	// Ti le danh trung
	int nRating = nAR / ((nAR + nDefense) * (0.5f - nIngore / 100.f) / 100.f);
	//if(m_Kind != kind_player)
	//printf("DEBUG Damage - TLDT = %d\n", nRating);
	if (GetRandomNumber(0, 100) <= nRating)
		return TRUE;

	return FALSE;
	//tualtn*/ 


	//HTKL
	int nDefense = nDf * (100 - nIngore) / 100;
	int nPercent = 0;
	//Question nAr+ nDefense  == 0 ,error!so.....,must modify
	if ((nAR + nDefense) == 0) 
		nPercent = 50;
	else
		nPercent = nAR * 100 / (nAR + nDefense);

	if (nPercent > MAX_HIT_PERCENT)
		nPercent = MAX_HIT_PERCENT;

	if (nPercent < MIN_HIT_PERCENT)
		nPercent = MIN_HIT_PERCENT;

	BOOL bRet = g_RandPercent(nPercent);
	//g_DebugLog("[数值]AttackRating %d : Defense %d: RandomPercent (%d, %d)", nAR, nDf, nPercent, bRet);
	return bRet;


/*
	//TINH KIEM MASTER
	int nDefense = 0;
	if (nIngore < 100)
		nDefense = nDf * (MAX_PERCENT - nIngore) / MAX_PERCENT;
	int nPercent = 0;
	if (nAR < 0)
		return FALSE;

	if (nDf < 0)
		nPercent = 100;
	else if ((nAR + nDefense) == 0) 
		nPercent = 50;
	else
		nPercent = nAR * MAX_PERCENT / (nAR + nDefense);

	if (nPercent > MAX_HIT_PERCENT + 4)
		nPercent = MAX_HIT_PERCENT; 

	if (nPercent < 40)
		nPercent = 40;

	BOOL bRet = g_RandPercent(nPercent);
	return bRet;
*/



	//int nDefense = nDf * (100 - nIngore) / 100;  //敌人的闪避率
	//int bRet = 0;

	//int nCurAr = nAR - nInHuLieAr;  //忽略对方命中点数

	//if (nCurAr <= 0) // 攻击方剩余的命中
	//{//如果当前攻击方的命中为 小于 0 或 等于0时,设置默认命中率为 10
	//	nCurAr = MIN_HIT_PERCENT;
	//}

	//if (nDefense <= 0)// (被攻击方的剩余闪避率)
	//{//如果敌人的闪避率小于 或 等与0  必中
	//	bRet = 1;
	//	return bRet;
	//}
	//else
	//{//否则对比  命中率 与 敌人的 闪避的  随机值
	//	if (g_Random(nCurAr) >= g_Random(nDefense))
	//	{//打中了
	//		bRet = 1;
	//	}
	//}

	//return bRet;
}

void KNpc::GetNpcCopyFromTemplate(int nNpcTemplateId, int nLevel)
{
	if (nNpcTemplateId < 0 || nLevel < 1 ) 
		return ;

	KNpcTemplate* pNpcTemp = NULL;
	if (pNpcTemp = NpcSet.GetTemplate(nNpcTemplateId, nLevel))
	//if (g_pNpcTemplate[nNpcTemplateId][nLevel]) //数据有效则拷贝，否则重新生成
		LoadDataFromTemplate(pNpcTemp);//(nNpcTemplateId, nLevel);

	/*else
	{
		if (!g_pNpcTemplate[nNpcTemplateId][0])
		{//大类基本数据,没有等级之分
			g_pNpcTemplate[nNpcTemplateId][0] = new KNpcTemplate;
			g_pNpcTemplate[nNpcTemplateId][0]->InitNpcBaseData(nNpcTemplateId);  //初始化基本数据和爆率文件
			g_pNpcTemplate[nNpcTemplateId][0]->m_NpcSettingIdx = nNpcTemplateId;
			g_pNpcTemplate[nNpcTemplateId][0]->m_bHaveLoadedFromTemplate = TRUE;
		}
	

//然后加载脚本等级数据		
KLuaScript * pLevelScript = NULL;		

#ifdef _SERVER //g_pNpcTemplate[nNpcTemplateId][0]
			pLevelScript = (KLuaScript*)g_GetScript(g_pNpcTemplate[nNpcTemplateId][0]->m_dwLevelSettingScript);
		
		if (pLevelScript == NULL)  //如果没有脚本的 就设置默认的脚本
			pLevelScript = g_pNpcLevelScript;   //npcLevelscript.lua
#else
		KLuaScript LevelScript;
		if (!g_pNpcTemplate[nNpcTemplateId][0]->m_szLevelSettingScript[0])
			pLevelScript = g_pNpcLevelScript;
		else
		{
			    LevelScript.Init();   //初始化 脚本函数
			if (!LevelScript.Load(g_pNpcTemplate[nNpcTemplateId][0]->m_szLevelSettingScript))
			{
				//g_DebugLog ("[error]致命错误,无法正确读取%s", g_pNpcTemplate[nNpcTemplateId][0]->m_szLevelSettingScript);
				//_ASSERT(0);
				pLevelScript = g_pNpcLevelScript;
				LevelScript.Exit();
			}
			else
				pLevelScript = &LevelScript;
		}

#endif
		g_pNpcTemplate[nNpcTemplateId][nLevel] = new KNpcTemplate; //重新生成当前等级下的 NPC数据
		*g_pNpcTemplate[nNpcTemplateId][nLevel] = *g_pNpcTemplate[nNpcTemplateId][0]; //把基本数据赋值
		g_pNpcTemplate[nNpcTemplateId][nLevel]->m_nLevel = nLevel; //改变等级
		//加载改等级下的脚本数据 
		g_pNpcTemplate[nNpcTemplateId][nLevel]->InitNpcLevelData(nNpcTemplateId, pLevelScript, nLevel);
		g_pNpcTemplate[nNpcTemplateId][nLevel]->m_bHaveLoadedFromTemplate = TRUE;  //设置已经加载了 模板NPC数据
		LoadDataFromTemplate(nNpcTemplateId,nLevel);       //加载更新后的数据基本数据
	} */

}
//从NPC模板导入数据
void	KNpc::LoadDataFromTemplate(VOID * nNpcTemp)//(int nNpcTemplateId, int nLevel)  //从NPC模板导入数据
{
	/*if (nNpcTemplateId < 0 || nNpcTemplateId > MAX_NPCSTYLE-1 || nLevel<0 || nLevel > MAX_NPC_LEVEL-1)
	{
		return ;
	} */
	
	//KNpcTemplate* pNpcTemp = NULL;
	//if (pNpcTemp = NpcSet.GetTemplate(nNpcTemplateId, nLevel))

	KNpcTemplate * pNpcTemp = (KNpcTemplate *)nNpcTemp;//NpcSet.GetTemplate(nNpcTemplateId, nLevel);//
	//KNpcTemplate * pNpcTemp = g_pNpcTemplate[nNpcTemplateId][nLevel];

	if (!pNpcTemp)
		return;

	//strcpy(Name,pNpcTemp->Name);
	snprintf(Name, sizeof(Name), "%s", pNpcTemp->Name);
	m_Kind   = pNpcTemp->m_Kind;
	m_Camp   = pNpcTemp->m_Camp;
	m_CurrentCamp  = pNpcTemp->m_Camp;

	if (pNpcTemp->m_Series<0 && pNpcTemp->m_Series>4)
	   m_Series = GetRandomNumber(0,4);
	else
	   m_Series = (BYTE)pNpcTemp->m_Series;

	m_Level  = pNpcTemp->m_nLevel;

	if  (m_Level<=0)
		m_Level =1;

	m_HeadImage   =	pNpcTemp->m_HeadImage;
	m_bClientOnly = pNpcTemp->m_bClientOnly;   //是否只是客户端的NPC
	m_CorpseSettingIdx = pNpcTemp->m_CorpseSettingIdx;
	m_DeathFrame =	pNpcTemp->m_DeathFrame;
	m_WalkFrame  =	pNpcTemp->m_WalkFrame;
	m_RunFrame   =	pNpcTemp->m_RunFrame;
	m_HurtFrame   =	pNpcTemp->m_HurtFrame;
	m_WalkSpeed   = pNpcTemp->m_WalkSpeed;
	m_AttackFrame =	pNpcTemp->m_AttackFrame;  //外攻攻击速度
	m_CastFrame   = pNpcTemp->m_CastFrame;    //内攻攻击速度
	m_RunSpeed =	pNpcTemp->m_RunSpeed;
	m_StandFrame =  pNpcTemp->m_StandFrame;
	m_StandFrame1 = pNpcTemp->m_StandFrame1;
	m_NpcSettingIdx = pNpcTemp->m_NpcSettingIdx;
	m_nStature		= pNpcTemp->m_nStature;

#ifdef _SERVER	
	m_Treasure		= pNpcTemp->m_Treasure;   //Npc掉落装备的数量
	m_SkillList		= pNpcTemp->m_SkillList;
	m_AiMode		= pNpcTemp->m_AiMode;
	m_AiAddLifeTime	= 0;
	m_pDropRate		= pNpcTemp->m_pItemDropRate;      //爆率赋值
    //m_pGoldRate		= pNpcTemp->m_pGoldItemDropRate;  //爆率赋值
	if (!m_AiSkillRadiusLoadFlag)
	{
		m_AiSkillRadiusLoadFlag = 1;
		int i;
		for (i = 0; i < MAX_AI_PARAM - 1; ++i)  //9
			m_AiParam[i] =	pNpcTemp->m_AiParam[i];

		int		nMaxRadius = 0, nTempRadius;
		KSkill	*pSkill;
		for (i = 1; i < MAX_NPC_USE_SKILL + 1; ++i)   // NPC技能数量
		{
			pSkill = (KSkill*)g_SkillManager.GetSkill(m_SkillList.m_Skills[i].SkillId, m_SkillList.m_Skills[i].SkillLevel);
			if (!pSkill)
				continue;
			nTempRadius = pSkill->GetAttackRadius();
			if (nTempRadius > nMaxRadius)
				nMaxRadius = nTempRadius;
		}
		m_AiParam[MAX_AI_PARAM - 1] = nMaxRadius * nMaxRadius;
	}

	m_FireResistMax			= pNpcTemp->m_FireResistMax;
	m_ColdResistMax			= pNpcTemp->m_ColdResistMax;
	m_LightResistMax		= pNpcTemp->m_LightResistMax;
	m_PoisonResistMax		= pNpcTemp->m_PoisonResistMax;
	m_PhysicsResistMax		= pNpcTemp->m_PhysicsResistMax;
	m_ActiveRadius			= pNpcTemp->m_ActiveRadius;
	m_VisionRadius			= pNpcTemp->m_VisionRadius;
	m_AIMAXTime				= pNpcTemp->m_AIMAXTime;
	m_HitRecover			= pNpcTemp->m_HitRecover;
	m_ReviveFrame			= pNpcTemp->m_ReviveFrame;
	m_IsRevive              = pNpcTemp->m_IsRevive;  //是否可以重生
	m_Experience			= pNpcTemp->m_Experience;
	m_CurrentExperience		= m_Experience;
	m_LifeMax				= pNpcTemp->m_nLifeMax;   //脚本的最大生命值
	m_LifeReplenish			= pNpcTemp->m_LifeReplenish;
	m_AttackRating			= pNpcTemp->m_AttackRating;
	m_Defend				= pNpcTemp->m_Defend;
	m_PhysicsDamage			= pNpcTemp->m_PhysicsDamage;
	m_RedLum				= pNpcTemp->m_RedLum;
	m_GreenLum				= pNpcTemp->m_GreenLum;
	m_BlueLum				= pNpcTemp->m_BlueLum;
	m_FireResist			= pNpcTemp->m_FireResist;
	m_ColdResist			= pNpcTemp->m_ColdResist;
	m_LightResist			= pNpcTemp->m_LightResist;
	m_PoisonResist			= pNpcTemp->m_PoisonResist;
	m_PhysicsResist			= pNpcTemp->m_PhysicsResist;
#else
	m_LifeMax				= pNpcTemp->m_nLifeMax;
	m_ArmorType				= pNpcTemp->m_ArmorType;
	m_HelmType				= pNpcTemp->m_HelmType;
//	m_PifengType		    = pNpcTemp->m_PifengType;
	m_WeaponType			= pNpcTemp->m_WeaponType;
	m_HorseType				= pNpcTemp->m_HorseType;
	m_bRideHorse			= pNpcTemp->m_bRideHorse;
	//strcpy(ActionScript, pNpcTemp->ActionScript);  //行为脚本赋值
	SetstrInfo(STR_ACTION_SCRIPT,pNpcTemp->ActionScript);
#endif
	
	/*for (int j  = 0; j < 4; j ++)
		m_SkillList.Add()
	int		m_nSkillID[4];
	int		m_nSkillLevel[4];*/

	//BOOL	m_bHaveLoadedFromTemplate;
	//int		m_nNpcTemplateId;
	
	RestoreNpcBaseInfo();  //设置NPC基本信息
	
}

//-----------------------------------------------------------------------
//	功能：设定物理攻击的最大最小值 not end 需要考虑调用的地方
//-----------------------------------------------------------------------
void	KNpc::SetPhysicsDamage(int nMinDamage, int nMaxDamage)
{
	m_PhysicsDamage.nValue[0] = nMinDamage;
	m_PhysicsDamage.nValue[2] = nMaxDamage;
}

//-----------------------------------------------------------------------
//	功能：设定攻击命中率
//-----------------------------------------------------------------------
void	KNpc::SetBaseAttackRating(int nAttackRating)
{
	m_AttackRating = nAttackRating;
	// 此处还需要加上装备、技能的影响，计算出当前值
	m_CurrentAttackRating += m_AttackRating;
}

//-----------------------------------------------------------------------
//	功能：设定防御力
//-----------------------------------------------------------------------
void	KNpc::SetBaseDefence(int nDefence)
{
	m_Defend = nDefence;
	// 此处还需要加上装备、技能的影响，计算出当前值
	m_CurrentDefend = m_Defend;
}

/*
//-----------------------------------------------------------------------
//	功能：设定行走速度
//-----------------------------------------------------------------------
void	KNpc::SetBaseWalkSpeed(int nSpeed)
{
	m_WalkSpeed = nSpeed;
	// 此处还需要加上装备、技能的影响，计算出当前值 (not end)
	m_CurrentWalkSpeed = m_WalkSpeed;
}
*/

/*
//-----------------------------------------------------------------------
//	功能：设定跑步速度
//-----------------------------------------------------------------------
void	KNpc::SetBaseRunSpeed(int nSpeed)
{
	m_RunSpeed = nSpeed;
	// 此处还需要加上装备、技能的影响，计算出当前值 (not end)
	m_CurrentRunSpeed = m_RunSpeed;
}
*/

#ifdef _SERVER

void KNpc::ScriptDropItem(int nBelongPlayer,int nLastDamageIdx,int nItemNum,int nSeries)
{
	if (nBelongPlayer > 0 && m_pDropRate) // 是否有爆率  是否有 攻击者 并且 被攻击者 有爆率在身
	{
		int nItemCountNum=0,nLuckyRate=0;
		int nLuckBeiShu = Npc[Player[nBelongPlayer].m_nIndex].IsLuKey;    //玩家活动幸运倍数

		int nGoldLuck = g_GlobalMissionArray.GetMissionValue(28);

		if (nGoldLuck<=0)
            nGoldLuck=1;

        //int nLuck       = Player[nBelongPlayer].m_nCurLucky*nLuckBeiShu*nGoldLuck;  //攻击者的幸运值
		//int nSysZcount  = 2;
		//    nSysZcount  = m_pDropRate->nCount;   //总数量
        int nSysBcount  = 0;
		   // nSysBcount  = m_pDropRate->nTypeNum; //要掉下的数量

		//g_GameSetTing.GetInteger("ServerConfig","LuckyRate",100,&nLuckyRate);
		nLuckyRate = _gameGlobalSet.NpcInfo.nLuckyRate;
		nItemCountNum = nItemNum;//m_pDropRate->nTypeNum;//GetItemParm(ITEM_PARM_TYPENUM);

		/*if (m_CurrentTreasure >0)
		{
		    nItemCountNum = m_CurrentTreasure;
		}*/

		if (m_pDropRate->nCount<=0)
		   return;
       //钱只掉一次

       int nisOpenAutoPlay = Npc[Player[nBelongPlayer].m_nIndex].m_AutoplayId;
	   //g_GameSetTing.GetInteger("ServerConfig","GoldMoneyRate",100,&nSysBcount);
	   nSysBcount = _gameGlobalSet.NpcInfo.nGoldMoneyRate;
	   if (/*g_RandPercent(nSysBcount) &&*/ g_RandPercent(m_pDropRate->nMoneyRate))//GetItemParm(ITEM_PARM_MONEYRATE)))         //随机取爆率范围数 return ((int)g_Random(100) < nPercent);
	   {  /*
		   if (nisOpenAutoPlay>0)
		   {
			   if  (Player[nBelongPlayer].m_Autoplay.nAutoMoney>0 || Player[nBelongPlayer].m_Autoplay.nAutoMoneyAndEques>0)
			         LoseMoney(nBelongPlayer,m_pDropRate->nMoneyNum);//钱
		   }
		   else*/
		     LoseMoney(nBelongPlayer,m_pDropRate->nMoneyNum);//钱
	   } 
//----------------------------------------------------------时间段设置
	   	int nIsCloseAllItem=0,nIsBreak=0;
        if (/*nBelongPlayer>0 && */nisOpenAutoPlay>0)
		{//挂机的
			int nIsCloseGuaItem=0,nTimeStart=0,nTimeEnd=9,nIsOpenFullForbit=0;
			//g_GameSetTing.GetInteger("ServerConfig","IsOpenFullForbit",0,&nIsOpenFullForbit);
			nIsOpenFullForbit = _gameGlobalSet.NpcInfo.nIsOpenFullForbit;

			if (Player[nBelongPlayer].m_Autoplay.nIsOpenPicjkup!=101)
			{
				//Player[nBelongPlayer].m_ItemList.msgshow("<color=gyellow>助手<color>:请开启自动拾取!!");
				nIsBreak=1;
				return;
			}

			if (nIsOpenFullForbit>=1 && Player[nBelongPlayer].m_nRoomFull==1)	  //包袱满了 不产生物品
			{
				nIsBreak=1;
				return;
			}
			nIsCloseGuaItem = _gameGlobalSet.NpcInfo.nIsCloseGuaItem;
			nIsCloseAllItem = _gameGlobalSet.NpcInfo.nIsCloseAllItem;
			nTimeStart      = _gameGlobalSet.NpcInfo.nTimeStart;
			nTimeEnd        = _gameGlobalSet.NpcInfo.nTimeEnd;
           
			if (nIsCloseAllItem>=1)
			{
                return;
			}

			time_t rawtime;      //定义一个long 型存放秒数
			struct tm * timeinfo;	
			time(&rawtime);     //距离现在的时间（秒）
			timeinfo = localtime(&rawtime);  //把从1970-1-1零点零分到当前时间系统所偏移的秒数时间转换为本地时间
			int	bYear=0,bMonth=0,bDay=0,bHour=0,bMin=0;			
			bYear   = timeinfo->tm_year+1900;
			bMonth  = timeinfo->tm_mon+1;
			bDay    = timeinfo->tm_mday;
			bHour   = timeinfo->tm_hour;
	        bMin    = timeinfo->tm_min;
            
            if (nIsCloseGuaItem>=1 && bHour>=nTimeStart && bHour<nTimeEnd)
			{
				nIsBreak=1;
				return;
			}
		}

		if (nItemCountNum<=0)
		    nItemCountNum = 3;

		intDropInfo.clear();

		for (int i = 0; i < nItemCountNum; ++i) 
		{  			
            if (nIsCloseAllItem>=1)
				return;
            if (nIsBreak>=1)
				return;
			LoseSingleItem(nBelongPlayer,nLuckyRate,nLastDamageIdx);   //NPC掉下装备 m_nLastDamageIdx	
		}

		//if (g_RandPercent(m_pDropRate->nMoneyRate));//GetItemParm(ITEM_PARM_MONEYRATE)))  //随机取爆率范围数 return ((int)g_Random(100) < nPercent);
		//{   
		//	if (nisOpenAutoPlay>0)
		//	{
		//		if  (Player[nBelongPlayer].m_Autoplay.nAutoMoney>0 || Player[nBelongPlayer].m_Autoplay.nAutoMoneyAndEques>0)
		//			LoseMoney(nBelongPlayer);  //钱
		//	}
		//	else
		//	        LoseMoney(nBelongPlayer);  //钱
		//}   
	}

}
//  死亡处罚--NPC掉下装备
void KNpc::DeathPunish(int nMode, int nBelongPlayer,int nLastDamageIdx)
{
	#define	LOSE_EXP_SCALE	10

	if (IsPlayer())
	{//玩家死亡
		int nExpMin=0,nExpMax=0,nGoldExpMin=0,nGoldExpMax=0,nSubExp=0,nDeathLevelpro;

		 nExpMin  = _gameGlobalSet.NpcInfo.nExpMin;
		 nExpMax  = _gameGlobalSet.NpcInfo.nExpMax;
		 nGoldExpMin  = _gameGlobalSet.NpcInfo.nGoldExpMin;
		 nGoldExpMax  = _gameGlobalSet.NpcInfo.nGoldExpMax;
		 nSubExp  = _gameGlobalSet.NpcInfo.nSubExp;
		 nDeathLevelpro  = _gameGlobalSet.NpcInfo.nDeathLevelpro;

		int nTempState =SubWorld[m_SubWorldIndex].m_nCurPKPunishState;

		if  (nTempState>=0 && nTempState<DEATH_MODE_NUM)
			nMode = nTempState;

		switch (nMode)
		{
		case DEATH_MODE_NPC_KILL:
			{//npc kill
				/*nSubExp=0;
				if (Player[m_nPlayerIdx].m_nExp > 0)
				{//当前的经验

					if (m_Level <= 10)
						nSubExp = Player[m_nPlayerIdx].m_nExp/50;//TakeTrader(Player[m_nPlayerIdx].m_nExp,50);	 //PlayerSet.m_cLevelAdd.GetLevelExp(m_Level)
					else
						nSubExp = GetRandomNumber(nGoldExpMin*m_Level,nGoldExpMax*m_Level);//TakeTrader(Player[m_nPlayerIdx].m_nExp,25);  	 //4分之一

					if (nSubExp>0)
						Player[m_nPlayerIdx].DirectAddExp(-nSubExp);
					else  
					{
						if (nSubExp==0)
							Player[m_nPlayerIdx].DirectAddExp(-Player[m_nPlayerIdx].m_nExp);
						else
							Player[m_nPlayerIdx].DirectAddExp(nSubExp);//Player[m_nPlayerIdx].m_nExp
					}
				}
				else
				{//当前经验为零时,调用全局死亡经验
					if (m_Level > nDeathLevelpro)
						Player[m_nPlayerIdx].DirectAddExp(-GetRandomNumber(nExpMin,nExpMax));
				}*/


				// 经验减少
				//if (Player[m_nPlayerIdx].m_nExp > 0)
				//{
					int nSubExp;
					if (m_Level <= 10)
						nSubExp = (PlayerSet.m_cLevelAdd.GetLevelExp(m_Level)  / MAX_PERCENT) * 2;
					else
						nSubExp = (PlayerSet.m_cLevelAdd.GetLevelExp(m_Level)  / MAX_PERCENT) * 4;

					Player[m_nPlayerIdx].DirectAddExp( -nSubExp );
				//}
				// 钱减少
				int nMoney = (Player[m_nPlayerIdx].m_ItemList.GetEquipmentMoney())/2;//TakeTrader(Player[m_nPlayerIdx].m_ItemList.GetEquipmentMoney(),2);  //减少一半的钱
				if (nMoney > 0)
				{
					Player[m_nPlayerIdx].m_ItemList.CostMoney(nMoney);
					// 损失金钱消息
					SHOW_MSG_SYNC	sMsg;
					sMsg.ProtocolType =s2c_msgshow;
					sMsg.m_wMsgID = enumMSG_ID_DEC_MONEY;
					sMsg.m_lpBuf = nMoney;
					sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1;
					g_pServer->PackDataToClient(Player[m_nPlayerIdx].m_nNetConnectIdx,(BYTE *)&sMsg, sMsg.m_wLength + 1);//
					sMsg.m_lpBuf = 0;

					//if (TakeTrader(nMoney,2) > 0)
					if ((nMoney/2) > 0)
						PlayerDeadCreateMoneyObj(nMoney/2);  //生成1/4 的钱
				}
			}
			break;
		case DEATH_MODE_PLAYER_NO_PUNISH:
			break;
		case DEATH_MODE_EXP_PUNISH://chua ro
			{//经验惩罚
				nSubExp=0;
				if (Player[m_nPlayerIdx].m_nExp > 0)
				{//当前的经验

					if (m_Level <= 10)
						nSubExp = (Player[m_nPlayerIdx].m_nExp)/50;//TakeTrader(Player[m_nPlayerIdx].m_nExp,50);
					else
						nSubExp = GetRandomNumber(nGoldExpMin*m_Level,nGoldExpMax*m_Level);//TakeTrader(Player[m_nPlayerIdx].m_nExp,25);  

					if (nSubExp>0)
						Player[m_nPlayerIdx].DirectAddExp(-nSubExp);
					else
					{
						if (nSubExp==0)
							Player[m_nPlayerIdx].DirectAddExp(-Player[m_nPlayerIdx].m_nExp);
						else
							Player[m_nPlayerIdx].DirectAddExp(nSubExp);//Player[m_nPlayerIdx].m_nExp
					}

				} 
				else
				{//当前经验为零时,调用全局死亡经验
					nSubExp= GetRandomNumber(nExpMin,nExpMax);
					Player[m_nPlayerIdx].DirectAddExp(-nSubExp);
				}
				if (Npc[nLastDamageIdx].m_Kind==kind_player)
				{//全局惩罚脚本
					int nTarPlayer=0;
					char szNpcIndex[30];

					if (nLastDamageIdx>0)
					{
						sprintf(szNpcIndex, "%d", nLastDamageIdx);
						nTarPlayer = Npc[nLastDamageIdx].m_nPlayerIdx;  //Npc[m_nLastDamageIdx].m_nPlayerIdx
					} 
					if (nTarPlayer>0)    //是否执行全局脚本
						Player[nTarPlayer].ExecuteScript_NEW("\\script\\global\\goldpunish.lua", "nGoldPunish", nLastDamageIdx,Name,m_Index,m_dwID,nSubExp);
						//Player[nTarPlayer].ExecuteScriptC("\\script\\global\\goldpunish.lua", "nGoldPunish", szNpcIndex,Name,m_Index,m_dwID,nSubExp);
				}
			}
			break;
		case DEATH_MODE_MONEY_PUNISH:
			{
				//银两惩罚
				int nMoney = Player[m_nPlayerIdx].m_ItemList.GetEquipmentMoney()/2;//TakeTrader(Player[m_nPlayerIdx].m_ItemList.GetEquipmentMoney(),2);  //减少一半的钱
				if (nMoney > 0)
				{
					Player[m_nPlayerIdx].m_ItemList.CostMoney(nMoney);
					// 损失金钱消息
					char msg[64];
					sprintf(msg,"提示:您损失了 %d 银两!",nMoney);
					Player[m_nPlayerIdx].m_ItemList.msgshow(msg);

					if ((nMoney/2) > 0)
						PlayerDeadCreateMoneyObj(nMoney/2);  //生成1/4 的钱

				}
				if (Npc[nLastDamageIdx].m_Kind==kind_player)
				{//全局惩罚脚本
					int nTarPlayer=0;
					char szNpcIndex[30];
					if (nLastDamageIdx>0)
					{
						sprintf(szNpcIndex, "%d", nLastDamageIdx);
						nTarPlayer = Npc[nLastDamageIdx].m_nPlayerIdx;  //Npc[m_nLastDamageIdx].m_nPlayerIdx
					} 
					if (nTarPlayer>0)    //是否执行全局脚本
						Player[nTarPlayer].ExecuteScript_NEW("\\script\\global\\goldpunish.lua", "nGoldPunish", nLastDamageIdx,Name,m_Index,m_dwID,nMoney);
						//Player[nTarPlayer].ExecuteScriptC("\\script\\global\\goldpunish.lua", "nGoldPunish", szNpcIndex,Name,m_Index,m_dwID,nMoney);
				}
			}
			break;
		case DEATH_MODE_EQUIP_PUNISH:
			{//装备惩罚
				int  nLostRate;
				nLostRate=g_Random(100)+Player[m_nPlayerIdx].m_nCurLucky;
				// 丢失物品
				Player[m_nPlayerIdx].m_ItemList.AutoLoseItemFromEquipmentRoom(nLostRate);

				// 丢失穿在身上的装备
				if (g_Random(100) > g_Random(100)+Player[m_nPlayerIdx].m_nCurLucky)
				{//幸运越多就越难掉下
					Player[m_nPlayerIdx].m_ItemList.AutoLoseEquip();
				}
			}
			break;
		case DEATH_MODE_JINBI_PUNISH:
			{
				//金币惩罚
				int nMoney = (Player[m_nPlayerIdx].m_ItemList.GetEquipmentXu())/10;//TakeTrader(Player[m_nPlayerIdx].m_ItemList.GetEquipmentXu(),10);  //10分之一
				if (nMoney > 0)
				{
					Player[m_nPlayerIdx].m_ItemList.CostXu(nMoney);
					// 损失金钱消息
					char msg[64];
					sprintf(msg,"提示:您损失了 %d 金币!",nMoney);
					Player[m_nPlayerIdx].m_ItemList.msgshow(msg);
				}

				if (Npc[nLastDamageIdx].m_Kind==kind_player)
				{//全局惩罚脚本
					int nTarPlayer=0;
					char szNpcIndex[30];
					if (nLastDamageIdx>0)
					{
						sprintf(szNpcIndex, "%d", nLastDamageIdx);
						nTarPlayer = Npc[nLastDamageIdx].m_nPlayerIdx;  //Npc[m_nLastDamageIdx].m_nPlayerIdx
					} 
					if (nTarPlayer>0)    //是否执行全局脚本
						Player[nTarPlayer].ExecuteScript_NEW("\\script\\global\\goldpunish.lua", "nGoldPunish", nLastDamageIdx,Name,m_Index,m_dwID,nMoney);
						//Player[nTarPlayer].ExecuteScriptC("\\script\\global\\goldpunish.lua", "nGoldPunish", szNpcIndex,Name,m_Index,m_dwID,nMoney);
				}
			}
			break;
		case DEATH_MODE_PKBATTLE_PUNISH:
			{
				//经验减少 ,掉装备  国战
				nSubExp=0;
				if (Player[m_nPlayerIdx].m_nExp > 0)
				{

					if (m_Level <= 10)
						nSubExp = Player[m_nPlayerIdx].m_nExp/50;//TakeTrader(Player[m_nPlayerIdx].m_nExp,50);
					else
						nSubExp = GetRandomNumber(nGoldExpMin*m_Level,nGoldExpMax*m_Level);//TakeTrader(Player[m_nPlayerIdx].m_nExp,25);  

					if (nSubExp>0)
						Player[m_nPlayerIdx].DirectAddExp(-nSubExp);
					else
					{
						if (nSubExp==0)
							Player[m_nPlayerIdx].DirectAddExp(-Player[m_nPlayerIdx].m_nExp);
						else
							Player[m_nPlayerIdx].DirectAddExp(nSubExp);//Player[m_nPlayerIdx].m_nExp
					}
				}
				else
				{//当前经验为零时,调用全局死亡经验
					if (m_Level > nDeathLevelpro)
						Player[m_nPlayerIdx].DirectAddExp(-GetRandomNumber(nExpMin,nExpMax));
				}

				// 钱减少
				int nMoney = Player[m_nPlayerIdx].m_ItemList.GetEquipmentMoney()/2;//TakeTrader(Player[m_nPlayerIdx].m_ItemList.GetEquipmentMoney(),2);  //减少一半的钱
				if (nMoney > 0)
				{
					Player[m_nPlayerIdx].m_ItemList.CostMoney(nMoney);
					// 损失金钱消息
					SHOW_MSG_SYNC	sMsg;
					sMsg.ProtocolType =s2c_msgshow;
					sMsg.m_wMsgID = enumMSG_ID_DEC_MONEY;
					sMsg.m_lpBuf = nMoney;
					sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1;
					g_pServer->PackDataToClient(Player[m_nPlayerIdx].m_nNetConnectIdx,(BYTE *)&sMsg, sMsg.m_wLength + 1);//
					sMsg.m_lpBuf = 0;

					if (nMoney/2 > 0)
						PlayerDeadCreateMoneyObj(nMoney/2);  //生成1/4 的钱
				}

				int  nLostRate;
				nLostRate=g_Random(100)+Player[m_nPlayerIdx].m_nCurLucky;
				// 丢失物品
				Player[m_nPlayerIdx].m_ItemList.AutoLoseItemFromEquipmentRoom(nLostRate);

				// 丢失穿在身上的装备
				if (g_Random(100) > g_Random(100)+Player[m_nPlayerIdx].m_nCurLucky)
				{
					Player[m_nPlayerIdx].m_ItemList.AutoLoseEquip();
				}
			}
			break;
		default:
			{
				int	nPKValue;
				nPKValue = Player[this->m_nPlayerIdx].m_cPK.GetPKValue();
				if (nPKValue < 0)
					nPKValue = 0;
				if (nPKValue > MAX_DEATH_PUNISH_PK_VALUE)
					nPKValue = MAX_DEATH_PUNISH_PK_VALUE;

				// 经验减少
				//int	nLevelExp = PlayerSet.m_cLevelAdd.GetLevelExp(m_Level);  //获取当前等级的经验
				//    Player[m_nPlayerIdx].DirectAddExp(-(nLevelExp/PlayerSet.m_sPKPunishParam[nPKValue].m_nExp));

				nSubExp=0;
				if (Player[m_nPlayerIdx].m_nExp > 0)
				{
					if (m_Level <= 10)
						nSubExp = (PlayerSet.m_cLevelAdd.GetLevelExp(m_Level))/50;//TakeTrader(PlayerSet.m_cLevelAdd.GetLevelExp(m_Level),50);
					else
						nSubExp = GetRandomNumber(nGoldExpMin*m_Level,nGoldExpMax*m_Level);//TakeTrader(PlayerSet.m_cLevelAdd.GetLevelExp(m_Level),25);  

					if (nSubExp>0)
						Player[m_nPlayerIdx].DirectAddExp(-nSubExp);
					else
					{
						if (nSubExp==0)
							Player[m_nPlayerIdx].DirectAddExp(-Player[m_nPlayerIdx].m_nExp);
						else
							Player[m_nPlayerIdx].DirectAddExp(nSubExp);//Player[m_nPlayerIdx].m_nExp
					}
				}
				else
				{//当前经验为零时,调用全局死亡经验
					Player[m_nPlayerIdx].DirectAddExp(-GetRandomNumber(nExpMin,nExpMax));
				} 

				// 钱减少
				int nMoney = Player[m_nPlayerIdx].m_ItemList.GetEquipmentMoney()/50;//TakeTrader(Player[m_nPlayerIdx].m_ItemList.GetEquipmentMoney(),50);

				if (nMoney > 0)
				{
					Player[m_nPlayerIdx].m_ItemList.CostMoney(nMoney);
					// 损失金钱消息
					SHOW_MSG_SYNC	sMsg;
					sMsg.ProtocolType = s2c_msgshow;
					sMsg.m_wMsgID = enumMSG_ID_DEC_MONEY;
					sMsg.m_lpBuf = nMoney;
					sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1;
					g_pServer->PackDataToClient(Player[m_nPlayerIdx].m_nNetConnectIdx,(BYTE *)&sMsg, sMsg.m_wLength + 1);
					sMsg.m_lpBuf = 0;

					if (nMoney/2 > 0)
						PlayerDeadCreateMoneyObj(nMoney/2);
				}
				int sPKValue=0;
				//g_GameSetTing.GetInteger("ServerConfig","PKValue",5,&sPKValue);
				// 丢失物品GetPKValue()
				if (nPKValue >= sPKValue)
				{//PK值大于5才掉下装备
					Player[m_nPlayerIdx].m_ItemList.AutoLoseItemFromEquipmentRoom(PlayerSet.m_sPKPunishParam[nPKValue].m_nItem);
					// 丢失穿在身上的装备
					if (g_Random(100) < PlayerSet.m_sPKPunishParam[nPKValue].m_nEquip)  //按照PK值的 惩罚概率
					{
						//Player[m_nPlayerIdx].m_ItemList.AutoLoseEquip();
					}
				}
				//被人杀死 PK值减少一点
				Player[m_nPlayerIdx].m_cPK.AddPKValue(NpcSet.m_nBeKilledAddPKValue); //增加PK值  m_Currentnopkvalue

				if (nLastDamageIdx)
				{
					if (Npc[nLastDamageIdx].IsPlayer())
						KPlayerChat::MakeEnemy(Name,Npc[nLastDamageIdx].Name);  //拉为仇人
				}
			}
			break;
		}
	}
//怪物死亡
	else if (nBelongPlayer > 0 && m_pDropRate) // 是否有爆率  是否有 攻击者 并且 被攻击者 有爆率在身
	{
		int nItemCountNum=0,nLuckyRate=0;

		int nLuckBeiShu = Npc[Player[nBelongPlayer].m_nIndex].IsLuKey;    //玩家活动幸运倍数

		int nGoldLuck=g_GlobalMissionArray.GetMissionValue(28);

		if (nGoldLuck<=0)
            nGoldLuck=1;

		nLuckyRate = _gameGlobalSet.NpcInfo.nLuckyRate;
		//g_GameSetTing.GetInteger("ServerConfig","LuckyRate",200,&nLuckyRate);
		nItemCountNum = m_pDropRate->nTypeNum;//GetItemParm(ITEM_PARM_TYPENUM);

		if (m_CurrentTreasure >0)
		    nItemCountNum = m_CurrentTreasure;

		if (m_pDropRate->nCount <= 0)
			return;


		if (nItemCountNum > m_pDropRate->nCount)
			nItemCountNum = m_pDropRate->nCount;

       //钱只掉一次
       int nisOpenAutoPlay = Npc[Player[nBelongPlayer].m_nIndex].m_AutoplayId;

/*	   if (/*g_RandPercent(_gameGlobalSet.NpcInfo.nGoldMoneyRate) && g_RandPercent(m_pDropRate->nMoneyRate))//GetItemParm(ITEM_PARM_MONEYRATE)))         //随机取爆率范围数 return ((int)g_Random(100) < nPercent);
	   {	//bat auto khong roi tien
		   /*if (nisOpenAutoPlay>0)
		   {
			   /*if  (Player[nBelongPlayer].m_Autoplay.nAutoMoney>0 || Player[nBelongPlayer].m_Autoplay.nAutoMoneyAndEques>0)
			   {
			         LoseMoney(nBelongPlayer,m_pDropRate->nMoneyNum);//钱
					 printf("Money1: idx:%d  %d -    %d-%d\n",nBelongPlayer, m_pDropRate->nMoneyNum, Player[nBelongPlayer].m_Autoplay.nAutoMoneyAndEques, Player[nBelongPlayer].m_Autoplay.nAutoMoney);	   
			   }
		   }
		   else
		   {
				LoseMoney(nBelongPlayer,m_pDropRate->nMoneyNum);//钱
		   }

			LoseMoney(nBelongPlayer,m_pDropRate->nMoneyNum);//钱
	   } */

	   

//----------------------------------------------------------时间段设置
	   	int nIsCloseAllItem=0,nIsBreak=0;
        if (/*nBelongPlayer>0 && */nisOpenAutoPlay>0)
		{//挂机的
			int nIsCloseGuaItem=0,nTimeStart=0,nTimeEnd=9,nIsOpenFullForbit=0;
			nIsOpenFullForbit = _gameGlobalSet.NpcInfo.nIsOpenFullForbit;

			if (Player[nBelongPlayer].m_Autoplay.nIsOpenPicjkup!=101)
			{
				nIsBreak=1;
				return;
			}

			if (nIsOpenFullForbit>=1 && Player[nBelongPlayer].m_nRoomFull==1)	  //包袱满了 不产生物品
			{
				nIsBreak=1;
				return;
			}

			nIsCloseGuaItem = _gameGlobalSet.NpcInfo.nIsCloseGuaItem;
			nIsCloseAllItem = _gameGlobalSet.NpcInfo.nIsCloseAllItem;
			nTimeStart      = _gameGlobalSet.NpcInfo.nTimeStart;
			nTimeEnd        = _gameGlobalSet.NpcInfo.nTimeEnd;

			if (nIsCloseAllItem>=1)
                return;

			time_t rawtime;      //定义一个long 型存放秒数
			struct tm * timeinfo;	
			time(&rawtime);     //距离现在的时间（秒）
			timeinfo = localtime(&rawtime);  //把从1970-1-1零点零分到当前时间系统所偏移的秒数时间转换为本地时间
			int	bYear=0,bMonth=0,bDay=0,bHour=0,bMin=0;			
			bYear   = timeinfo->tm_year+1900;
			bMonth  = timeinfo->tm_mon+1;
			bDay    = timeinfo->tm_mday;
			bHour   = timeinfo->tm_hour;
	        bMin    = timeinfo->tm_min;
            
            if (nIsCloseGuaItem>=1 && bHour>=nTimeStart && bHour<nTimeEnd)
			{
				nIsBreak=1;
				return;
			}
		}

		//bool      _IsNoLostItem;    //不掉装备
		//bool      _IsNoLostMoney;   //不掉银两
	
		if  (m_SubWorldIndex>=0 && m_SubWorldIndex<MAX_SUBWORLD)
		{
				bool _IsNoLostItem= false;
				_IsNoLostItem= SubWorld[m_SubWorldIndex]._IsNoLostItem;

				if (_IsNoLostItem)
					return;
		}

		if (nItemCountNum<=0)
		    nItemCountNum = 3;

		int nCurIdx = 0;

		intDropInfo.clear();
		//loai bo drop item khi dung dropItem scripts "DropRate"
		if  (m_IsClearDropRank)
		{
			 m_pDropRate=NULL;
			 return;
		}

		//roi tien
		for (int i = 0; i < nItemCountNum; i++)
		{
			if (g_RandPercent(m_pDropRate->nMoneyRate))
			{
				LoseMoney(nBelongPlayer, m_pDropRate->nMoneyNum);//钱
			} 
		}
		//roi trang bi
		for (int i = 0; i < nItemCountNum; ++i) 
		{  			
            if (nIsCloseAllItem>=1)
				return;
            if (nIsBreak>=1)
				return;

			LoseSingleItem(nBelongPlayer,nLuckyRate,nLastDamageIdx);   //NPC掉下装备 m_nLastDamageIdx	
		}
	
	}
}

// 玩家死的时候掉出来的钱生成一个object
void	KNpc::PlayerDeadCreateMoneyObj(int nMoneyNum)
{
	int		nX, nY,nMap;
	POINT	ptLocal;
	KMapPos	Pos;

	GetMpsPos(&nX, &nY,&nMap);
	ptLocal.x = nX;
	ptLocal.y = nY;
	SubWorld[m_SubWorldIndex].GetFreeObjPos(ptLocal);

	Pos.nSubWorld = m_SubWorldIndex;
	SubWorld[m_SubWorldIndex].Mps2Map(ptLocal.x, ptLocal.y, 
		&Pos.nRegion, &Pos.nMapX, &Pos.nMapY, 
		&Pos.nOffX, &Pos.nOffY);
	
	int nObjIdx = ObjSet.AddMoneyObj(Pos, nMoneyNum);
	if (nObjIdx > 0 && nObjIdx < MAX_OBJECT)
	{
		Object[nObjIdx].SetItemBelong(-1);
	}
}
//掉下的钱数
void KNpc::LoseMoney(int nBelongPlayer,int nMoneyNum)
{
	if  (m_SubWorldIndex<0 ||  m_SubWorldIndex>=MAX_SUBWORLD)
	   return;

	bool _IsNoLostMoney= false;
	_IsNoLostMoney= SubWorld[m_SubWorldIndex]._IsNoLostMoney;

	if (_IsNoLostMoney)
		return;

	if (SubWorld[m_SubWorldIndex].m_nNoDropItemKind==NO_DROP_MONEY)
		return;

	if  (nBelongPlayer<=0)
		return ;

	int nX, nY,nMap;
	POINT	ptLocal;
	KMapPos	Pos;
    int  nJinQian=Npc[Player[nBelongPlayer].m_nIndex].IsJinQian;

	int nMoney=0;
/*	nMoney = Npc[Player[nBelongPlayer].m_nIndex].m_Level* m_pDropRate->nMoneyScale*nJinQian;	//GetItemParm(ITEM_PARM_MONEYSCALE)
	
	printf("LoseMoney1: nMoney: %d MoneyScale: %d nJinQian: %d\n",nMoney,m_pDropRate->nMoneyScale,nJinQian);
    if (nMoney>=10000)
        nMoney=5000+GetRandomNumber(1,5000);
	//TakeTrader(nMoneyRate[1],2)+GetRandomNumber(1,TakeTrader(nMoneyRate[1],2));

	if (nMoneyNum > 0) 
		nMoney=GetRandomNumber(nMoneyNum/2,nMoneyNum);

	if (nMoney <= 0)
		nMoney = Npc[Player[nBelongPlayer].m_nIndex].m_Level*10;
*/
	if (nMoneyNum<=0) 
		nMoneyNum=1;

	nMoney = nMoneyNum;

	if (nMoney <= 0)
		nMoney = 1;

	//如果是手机用户直接入包袱
	if (Player[nBelongPlayer].getMobileClient())
	{
		Player[nBelongPlayer].Earn(GetRandomNumber(nMoney/2,nMoney)); //增加钱
		return ;
	}

	//printf("LoseMoney2: tien roi nMoney: %d \n",nMoney);

	GetMpsPos(&nX, &nY,&nMap);
	ptLocal.x = nX;
	ptLocal.y = nY;
	SubWorld[m_SubWorldIndex].GetFreeObjPos(ptLocal);
	
	Pos.nSubWorld = m_SubWorldIndex;
	SubWorld[m_SubWorldIndex].Mps2Map(ptLocal.x, ptLocal.y, 
		&Pos.nRegion, &Pos.nMapX, &Pos.nMapY, 
		&Pos.nOffX, &Pos.nOffY);
	
	int nObjIdx = ObjSet.AddMoneyObj(Pos, nMoney);
	if (nObjIdx > 0 && nObjIdx < MAX_OBJECT)
	{
		if (nBelongPlayer > 0)
			Object[nObjIdx].SetItemBelong(nBelongPlayer);
		else
			Object[nObjIdx].SetItemBelong(-1);
	}

}

//随机掉下某个装备
int  KNpc::DropOneItem(int nBelongPlayer,int nItemGenre,int nDetailType,int nParticulType,\
					   int nItemLevel, int nItemSeries,int nLuckey,int nRate,int nStackNum,\
					   int nMgCount,int nMgLevel,int IsBang,int IsHour,int IsQuality)
{
 //   
//	int nGoldLuck=g_GlobalMissionArray.GetMissionValue(28);//全服幸运
//	if (nGoldLuck<=0)
//        nGoldLuck=1;
//
	//printf("DropOneItem:  m_nCurLucky: %d IsLuKey: %d\n", Player[nBelongPlayer].m_nCurLucky, Npc[Player[nBelongPlayer].m_nIndex].IsLuKey);


	if (nItemSeries<=-1 || nItemSeries>=5)
	      nItemSeries   = GetRandomNumber(0,4);//m_Series;
	
//==============爆的就是这个物品了==================
	time_t rawtime;
	struct tm * timeinfo;	
	time (&rawtime);
	timeinfo = localtime (&rawtime);
	int bYear,bMonth,bDay,bHour,bMin,pnMagicLevel[6],nSLevel=0;
	ZeroMemory(pnMagicLevel,sizeof(pnMagicLevel));

	if (IsHour>0)
	{
      bYear  =timeinfo->tm_year+1900;
      bMonth =timeinfo->tm_mon+1;
      bDay   =timeinfo->tm_mday;
      bHour  =timeinfo->tm_hour+IsHour;
	  bMin   =timeinfo->tm_min;
	}
	else
	{
		bYear  =0;
		bMonth =0;
		bDay   =0;
		bHour  =0;
    	bMin   =0;
	}

	if (nMgCount<=0)
		nMgCount = GetRandomNumber(1,6);

	if (nStackNum<=0)
	   nStackNum=1;

	if (nMgCount > 6)
		nMgCount = 6;

	for (int j = 0; j < nMgCount; ++j)    // 掉下的属性数量
	{
		if (!nItemGenre) 
		{//是装备
			if (nMgLevel<=0)
			    nSLevel=GetRandomNumber(1,10);
			else
				nSLevel=nMgLevel;
				pnMagicLevel[j] =nSLevel;  //魔法属性的类型 魔法属性里面的行数  出现  属性的品数 1-10范围
		}
		else
		{//为白装  爆其他的 	 
			pnMagicLevel[j] = 0;  //空属性
		}
	}
/*
	if (nMgLevel >= 1)
	{
		for (int i = 0; i < nMgCount; i++)
			pnMagicLevel[i] = nMgLevel;
	}
	else
	{
		for (int i = 0; i < 5; i ++)
			pnMagicLevel[i + 1] = pnMagicLevel[0];
	}
*/

//--------------------------------------------------------------------
	if (nItemLevel<=0)
        nItemLevel = 1;

	int nItemIdx=0;

    if (IsQuality==0)
		nItemIdx = ItemSet.AddOther(nItemGenre,nItemSeries,nItemLevel,nLuckey,\
		                  nDetailType, nParticulType, pnMagicLevel,\
		                  g_SubWorldSet.GetGameVersion(),0,GetRandomNumber(1,nStackNum),\
		                  0,0,bYear,bMonth,bDay,bHour,bMin,0,\
                            NULL,NULL,NULL,0);
	else if(IsQuality==1) //黄装的产生
	    nItemIdx = ItemSet.AddGold(nDetailType,pnMagicLevel,nItemSeries,0,0,\
		                   bYear,bMonth,bDay,bHour,bMin,NULL,NULL,nItemLevel,nLuckey);

	if (nItemIdx <= 0)
		return 0;

    if (IsBang)
        Item[nItemIdx].SetBang(IsBang);

	int		nX, nY,nMap;
	POINT	ptLocal;
	KMapPos	Pos;

	GetMpsPos(&nX, &nY,&nMap);                         // 怪物的坐标=转换后的坐标

	ptLocal.x = nX;
	ptLocal.y = nY;

	SubWorld[m_SubWorldIndex].GetFreeObjPos(ptLocal);  //获取能掉下东西的坐标
	
	Pos.nSubWorld = m_SubWorldIndex;
	SubWorld[m_SubWorldIndex].Mps2Map(ptLocal.x, ptLocal.y,\
		              &Pos.nRegion, &Pos.nMapX, &Pos.nMapY,\
		                           &Pos.nOffX, &Pos.nOffY);

	int nObj;
	//////掉下装备保存在KObjItemInfo//////结构体中的参数，以便调用
	KObjItemInfo sInfo;
	memset(&sInfo, 0, sizeof(sInfo));
	sInfo.m_nItemID     = nItemIdx;                   //物品索引
	sInfo.m_nItemWidth  = Item[nItemIdx].GetWidth();
	sInfo.m_nItemHeight = Item[nItemIdx].GetHeight();
	sInfo.m_nMoneyNum   = 0;
	snprintf(sInfo.m_szName, sizeof(sInfo.m_szName), "%s", Item[nItemIdx].GetName()); //物品的名称
	sInfo.m_nColorID    = 0;                      //颜色
	sInfo.m_nMovieFlag  = 1;
	sInfo.m_nSoundFlag  = 1;
    sInfo.m_sHaveAttack = 0;   //是否设置为攻击无效了
	//sInfo.m_AttackerDwid=0;   //上次攻击着的DWID
	nObj = ObjSet.Add(Item[nItemIdx].GetObjIdx(),Pos,sInfo);   //开始同步地上物品

	if (nObj == -1)
	{
		ItemSet.Remove(nItemIdx);
		return 0;
	}
	else
	{
		if (nBelongPlayer > 0)
		{
		   
              Object[nObj].SetItemBelong(nBelongPlayer);
		} 
		else
		{
			  Object[nObj].SetItemBelong(-1);
		}
	}

		return nItemIdx;

}
//Drop Mac Dinh item .ini
//NPC掉出装备   NPC掉装备  NPC掉下装备 玩家PK掉下装备
/*void KNpc::LoseSingleItem(int nBelongPlayer,int LuckyRate,int LastDamageIdx,int IsWhere,int Series)
{
	//if (!m_pDropRate //|| m_IsClearDropRank!=0//)  //如果没有爆率 就不掉出
	if (!m_pDropRate)
		return;

	if (SubWorld[m_SubWorldIndex].m_nNoDropItemKind==NO_DROP_ITEM)
		return;

	if (m_pDropRate->nMaxItemLevelScale <= 0 || m_pDropRate->nMinItemLevelScale <= 0)
		return;

	int nRand = g_Random(m_pDropRate->nMaxRandRate);  //总爆率  取余数

	int nCheckRand  = 0;	// 累加概率，确认是否落在区间内
    //int nIsThisItem =-1;
	int i;
	for (i = 0; i < m_pDropRate->nCount; ++i)   //循环掉装备  装备的总数量
	{
		if (intDropInfo[i+1]==1)
		{//已经爆过的
			//printf("-已经爆过(%d)不能重复爆,取下一个 \n",i);
			continue;
		}

		if (nRand >= nCheckRand && nRand < nCheckRand + m_pDropRate->pItemParam[i].nRate)
		{//这个数大于等于上个物品的累计爆率并小于当时这个物品的累加爆率
			//nIsThisItem = i;
			break;
		}

		nCheckRand += m_pDropRate->pItemParam[i].nRate;   //每个物品爆率加起来累加
	}

	if (i >= m_pDropRate->nCount)  //没找到合适物品爆 就返回了
		return;	

	//==============爆的就是这个物品了=================
	int nGenre, nLuck=0, nDetail, nParticular, nLevel=1, pnMagicLevel[6],nQuality,nwMinItemLevel,nwMaxItemLevel,nIsBang=0,nHour=0,bYear=0,bMonth=0,bDay=0,bHour=0,bMin=0,n_playerLevel;  //Quality=1是黄装 2为白金，3 为紫装 等
	int pnRMagicLevel[6],nIsNoBian=0,nMaxStackNum=1;
	ZeroMemory(pnRMagicLevel,sizeof(pnRMagicLevel));
	nGenre         = m_pDropRate->pItemParam[i].nGenre;
	nDetail        = m_pDropRate->pItemParam[i].nDetailType;
	nParticular    = m_pDropRate->pItemParam[i].nParticulType;

	if 	(nGenre<0)
		return;

	//---------------------开始挂机判断-----------------------------------
	int _isOpenAutoPlay = Npc[Player[nBelongPlayer].m_nIndex].m_AutoplayId;

	if  (_isOpenAutoPlay>0)
	{
		if  (Player[nBelongPlayer].m_Autoplay.nAutoTool<=0)
		{//不掉道具
			if (nGenre==item_mine || nGenre ==item_task)
				return;
		}

		if  (Player[nBelongPlayer].m_Autoplay.nAutoEques<=0 && Player[nBelongPlayer].m_Autoplay.nAutoMoneyAndEques<=0)
		{//不掉装备
			if (nGenre==item_equip)
				return;
		}

		if  (Player[nBelongPlayer].m_Autoplay.nAutoTuPu<=0)
		{//不爆图志
			if ((nGenre==item_mine && nParticular==206) ||(nGenre==item_mine && nParticular==213)|| (nGenre==item_task && nDetail==82))
				return;
		}
	}
    //---------------------------------------------------------------------
	intDropInfo[i+1]=1;           //记录已经爆过的索引号 

	int nGoldLuck=g_GlobalMissionArray.GetMissionValue(28);//全服幸运
	if (nGoldLuck<=0)
        nGoldLuck=1;

	if (nBelongPlayer>0)
	    nLuck= Player[nBelongPlayer].m_nCurLucky*Npc[Player[nBelongPlayer].m_nIndex].IsLuKey*nGoldLuck;  //玩家的幸运值

	if  (Series<0 || Series>4)
	      Series   = GetRandomNumber(0,4);//m_Series;
	
    n_playerLevel       = Npc[Player[nBelongPlayer].m_nIndex].m_Level; //玩家等级
	nQuality       = m_pDropRate->pItemParam[i].nQuality;         //是否黄金 或紫装
    nwMinItemLevel = m_pDropRate->pItemParam[i].nMinItemLevel;    //物品部分的等级
    nwMaxItemLevel = m_pDropRate->pItemParam[i].nMaxItemLevel;    //物品的等级
    nIsBang        = m_pDropRate->pItemParam[i].nIsBang;
    nHour          = m_pDropRate->pItemParam[i].nHour;
    nIsNoBian      = m_pDropRate->pItemParam[i].nIsNoBian;
    nMaxStackNum   = m_pDropRate->pItemParam[i].nStackNum;
//==============爆的就是这个物品了==================
	time_t rawtime;
	struct tm * timeinfo;	
	time (&rawtime);
	timeinfo = localtime (&rawtime);

	if (nHour>0)
	{
      bYear  =timeinfo->tm_year+1900;
      bMonth =timeinfo->tm_mon+1;
      bDay   =timeinfo->tm_mday;
      bHour  =timeinfo->tm_hour+nHour;
	  bMin   =timeinfo->tm_min;
	}
	else
	{
		bYear  =0;
		bMonth =0;
		bDay   =0;
		bHour  =0;
    	bMin   =0;
	}

    //int nMaxLevel,nMinLevel,mCount=0,mAllLuck=nLuck//+n_Wlevel+m_Level//,nBossType=4,nIsNoRandomGoald=0;
    int nMaxLevel,nMinLevel,mCount=0,mAllLuck=nLuck,nBossType=4,nIsNoRandomGoald=0;
	nIsNoRandomGoald = m_pDropRate->pItemParam[i].nIsNoRandGold;

	BOOL bSkip = FALSE;

	int nSerLucky = 0;
	nSerLucky = nLuck*IsLuKey;//NPC的幸运

	if (nSerLucky>2000000000)
		nSerLucky = 2000000000;

	int j;
	//printf("----爆的就是这个物品(%d,%d,%d)---\n",nGenre,nDetail,nParticular);
	for (j = 0; j <6; ++j)    // 掉下的属性数量
	{
		if (!bSkip)
		{//不等Ture 就执行掉 GetItemParm(ITEM_PARM_MAGRATE)
			 //if (g_Random(m_pDropRate->nMagicRate + nSerLucky)// nLuck)// >= g_Random(LuckyRate))   // 取余数 幸运+蓝装的爆率  魔法属性的爆率
			 if (g_Random(m_pDropRate->nMagicRate + nSerLucky) >= g_Random(LuckyRate))   // 取余数 幸运+蓝装的爆率  魔法属性的爆率
			 {
				 if (nwMinItemLevel>0 && nwMaxItemLevel>0)
				 {//物品指定的等级	 
					 nLevel = GetRandomNumber(nwMinItemLevel,nwMaxItemLevel);  
				 }
				 else
				 {//NPC等级 / 等级倍率 ==物品的等级
					 nMaxLevel = m_Level/m_pDropRate->nMaxItemLevelScale;//GetItemParm(ITEM_PARM_MAXLEVELSCALE);
					 nMinLevel = m_Level/m_pDropRate->nMinItemLevelScale;//GetItemParm(ITEM_PARM_MINLEVELSCALE);
					 
					 if (nMaxLevel > m_pDropRate->nMaxItemLevel)  //全部等级参数
						 nMaxLevel = m_pDropRate->nMaxItemLevel;
					 
					 if (nMinLevel < m_pDropRate->nMinItemLevel)
						 nMinLevel = m_pDropRate->nMinItemLevel;
					 
					 if (nMaxLevel < m_pDropRate->nMinItemLevel)
						 nMaxLevel = m_pDropRate->nMinItemLevel;
					 
					 if (nMinLevel > m_pDropRate->nMaxItemLevel)
						 nMinLevel = m_pDropRate->nMaxItemLevel;
					 
					 if (nMaxLevel < nMinLevel)
					 {
						 int nTemp;
						 nTemp     = nMinLevel;
						 nMinLevel = nMaxLevel;
						 nMaxLevel = nTemp;
					 }

					 nLevel = g_Random(nMaxLevel - nMinLevel) + nMinLevel; //*随机种子 然互取 取余数 装备等级
				 } 

				  if (nLevel>=10)
					 nLevel = 10;
			      if (nLevel<=0)
					 nLevel = 1;

				  int  _curLuck=GetRandomNumber(nLuck/2,nLuck*1.5);
				  int  _nMagicLevel = _curLuck/_gameGlobalSet.NpcInfo.nLuckyRange;//

				 if (m_IsNoLevel<=0)
				 {//如果不检测等级
					 int nCha=0;
				          nCha=abs(m_Level-n_playerLevel);//等级差

					 if (nCha<GetRandomNumber(0,100)) //对比的等级//物品的品数，否则就是白装
					 {
						 if  (_nMagicLevel<=0)
							 _nMagicLevel =GetRandomNumber(1,_gameGlobalSet.NpcInfo.nLevelRangeCount);
						 if  (_nMagicLevel>10)
							 _nMagicLevel =GetRandomNumber(7,10);
					 }
					 else
						  _nMagicLevel =GetRandomNumber(1,_gameGlobalSet.NpcInfo.nLevelRangeCount);
				  }
				  else
				  {//如果不检测等级的差距 			  
					  if  (_nMagicLevel<=0)
						   _nMagicLevel =GetRandomNumber(1,_gameGlobalSet.NpcInfo.nLevelRangeCount);
                      if  (_nMagicLevel>10)
						   _nMagicLevel =GetRandomNumber(7,10);
				  }
				 
				 if  (m_itemMagicLevel>0)
					 _nMagicLevel = m_itemMagicLevel;

				 pnMagicLevel[j] = _nMagicLevel;  //魔法属性的类型 魔法属性里面的行数  出现  属性的品数 1-10范围

				 if (m_Kind == kind_normal)
				 {
					 if (nGenre==item_equip && strstr(Npc[Player[nBelongPlayer].m_nIndex].Name,_gameGlobalSet.PlayerInfo.nExpRoleName))
						 printf("--玩家:%s,幸运:%d,取值幸运:%d[爆率:属性%d,品质:%d,忽略等级:%d]--\n",Npc[Player[nBelongPlayer].m_nIndex].Name,nLuck,_curLuck,j,_nMagicLevel,m_IsNoLevel);
				 }

				 if (_nMagicLevel==0)
					bSkip = TRUE;
			 }
			 else
			 {//概率不在这个范围的 不再掉下有属性的
				     pnMagicLevel[j] = 0; // 空装备
				     bSkip = TRUE;        // 不再掉下装备了
			 }
		}
		else
		{//为白装  爆其他的 
			if (nwMinItemLevel>0 && nwMaxItemLevel>0)
			{//物品指定的等级	 
				nLevel = GetRandomNumber(nwMinItemLevel,nwMaxItemLevel);  
			}
			pnMagicLevel[j] = 0;  //空属性
		}
	}

	int nItemIdx=0,nRpiont=0,nMagicNum=0; 

	//-------蓝装nLevel物品的产生影响装备的外观等级---辨识属性设置----	
	ZeroMemory(pnRMagicLevel,sizeof(pnRMagicLevel));

	///g_GameSetTing.GetInteger("ServerConfig","MagicNum",0,&nMagicNum);
	nMagicNum = _gameGlobalSet.NpcInfo.nMagicNum;

	if (nGenre==item_fusion)
	{//如果是纹纲 
        nParticular +=nLevel-1; //详细类别
	}

    if (nGenre==item_equip && m_pDropRate->nIsBianShi && pnMagicLevel[nMagicNum] && nIsNoBian!=1)
	{//有多少个属性以上的才可以爆辨识装备
		//g_GameSetTing.GetInt5("SYSTEM","BianShiLuckyCount",_gameGlobalSet.NpcInfo.BianLuckItem);

		if (nLuck>=_gameGlobalSet.NpcInfo.BianLuckItem[0] && nLuck <=_gameGlobalSet.NpcInfo.BianLuckItem[1])
			nRpiont = GetRandomNumber(0,1);
		else if (nLuck>_gameGlobalSet.NpcInfo.BianLuckItem[1] && nLuck <=_gameGlobalSet.NpcInfo.BianLuckItem[2])
			nRpiont = GetRandomNumber(0,2);
		else if  (nLuck>_gameGlobalSet.NpcInfo.BianLuckItem[2] && nLuck <=_gameGlobalSet.NpcInfo.BianLuckItem[3])
			nRpiont = GetRandomNumber(0,3);
		else if (nLuck>_gameGlobalSet.NpcInfo.BianLuckItem[3]&& nLuck <=_gameGlobalSet.NpcInfo.BianLuckItem[4])
			nRpiont = GetRandomNumber(0,4);
		else if (nLuck>_gameGlobalSet.NpcInfo.BianLuckItem[4])
			nRpiont = GetRandomNumber(0,6);
		else
            nRpiont = 0;

        if (nRpiont>0)
		{ //是否产生辨识属性
		  for (j=0;j<nRpiont;++j)
		  { //熔炼属性赋值 == 行数
			  pnRMagicLevel[j] = 0;//isHave; //设定行数			
		  } 
		 
		  IsWhere = -1;                      //设置未辨识

          int nTime = 1;
		  //g_GameSetTing.GetInteger("ServerConfig","BianShiTime",1,&nTime);
		    nTime =  _gameGlobalSet.NpcInfo.nBianTime;
          if (pnRMagicLevel[0])
		  {  
            bYear  =timeinfo->tm_year+1900;
			bMonth =timeinfo->tm_mon+1;
			bDay   =timeinfo->tm_mday;
			bHour  =timeinfo->tm_hour+nTime;   //默认有一个小时来辨识
	        bMin   =timeinfo->tm_min;
		  } 
		}
        else
		{  
            nRpiont = 0;
		}   

	}   

	//_gameGlobalSet.NpcInfo.nLuckItem[0]=0;
	//_gameGlobalSet.NpcInfo.nLuckItem[1]=0;
	//_gameGlobalSet.NpcInfo.nLuckItem[2]=0;
	//_gameGlobalSet.NpcInfo.nLuckItem[3]=0;
	//_gameGlobalSet.NpcInfo.nLuckItem[4]=0;
//--------------------------------------------------------------------
	if (nLevel<=0)
        nLevel = 1;
	//int nForBitItem[3];
	//g_GameSetTing.GetInt3("ServerConfig","AutoPlayForBitItem",nForBitItem);

	if (nBelongPlayer>0 && Npc[Player[nBelongPlayer].m_nIndex].m_AutoplayId>0 && _gameGlobalSet.NpcInfo.nForBitItem[0] && timeinfo->tm_hour>=_gameGlobalSet.NpcInfo.nForBitItem[1] && timeinfo->tm_hour<_gameGlobalSet.NpcInfo.nForBitItem[2])
	{//挂机的 开启并且是这时间段的 开始检测 不能掉落的物品
		if (CheckAllItem(3,nGenre,nDetail,nParticular))
			return;
	}

	//nLuck  nSerLucky
    if (nQuality==0)
		nItemIdx = ItemSet.AddOther(nGenre,Series,nLevel,nLuck,\
		                  nDetail, nParticular, pnMagicLevel,\
		                  g_SubWorldSet.GetGameVersion(),0,GetRandomNumber(1,nMaxStackNum),\
		            0,0,bYear,bMonth,bDay,bHour,bMin,nRpiont,\
                            pnRMagicLevel,NULL,NULL,IsWhere);
	else if(nQuality>0) //黄装的产生
	{
		if (nIsNoRandomGoald<=0)
		  nItemIdx = ItemSet.AddRondomGold(nQuality,pnMagicLevel,Series,0,0,bYear,bMonth,bDay,bHour,bMin,NULL,nLevel,nLuck,nDetail);
		else
          nItemIdx = ItemSet.AddGold(nQuality,pnMagicLevel,Series,0,0,bYear,bMonth,bDay,bHour,bMin,NULL,NULL,nLevel,nLuck);
	}  
	//char msg[80];
	//sprintf(msg,"S测试:%s(%d,%d,%d,%d)",Item[nItemIdx].GetName(),nGenre,nDetail,nParticular,nSLevel);
    //Player[nBelongPlayer].m_ItemList.msgshow(msg);//m_PlayerIdx

	if (nItemIdx <= 0)
		return;

    if (nIsBang)
          Item[nItemIdx].SetBang(nIsBang);

	if  (SubWorld[m_SubWorldIndex].m_suoHun)
	{
		Item[nItemIdx].SetUseKind(0);//锁魂地图
		Item[nItemIdx].SetUseMapIdx(SubWorld[m_SubWorldIndex].m_SubWorldID);
	}

	int		nX, nY,nMap;
	POINT	ptLocal;
	KMapPos	Pos;

	GetMpsPos(&nX, &nY,&nMap);                         // 怪物的坐标=转换后的坐标

	ptLocal.x = nX;
	ptLocal.y = nY;

	SubWorld[m_SubWorldIndex].GetFreeObjPos(ptLocal);  //获取能掉下东西的坐标
	
	Pos.nSubWorld = m_SubWorldIndex;
	SubWorld[m_SubWorldIndex].Mps2Map(ptLocal.x, ptLocal.y,\
		              &Pos.nRegion, &Pos.nMapX, &Pos.nMapY,\
		                           &Pos.nOffX, &Pos.nOffY);

	int nObj;
	//////掉下装备保存在KObjItemInfo//////结构体中的参数，以便调用
	KObjItemInfo sInfo;
	memset(&sInfo, 0, sizeof(sInfo));
	sInfo.m_nItemID     = nItemIdx;                   //物品索引
	sInfo.m_nItemWidth  = Item[nItemIdx].GetWidth();
	sInfo.m_nItemHeight = Item[nItemIdx].GetHeight();
	sInfo.m_nMoneyNum   = 0;
	strcpy(sInfo.m_szName, Item[nItemIdx].GetName()); //物品的名称
	sInfo.m_nColorID    = 0;                      //颜色
	sInfo.m_nMovieFlag  = 1;
	sInfo.m_nSoundFlag  = 1;
    sInfo.m_sHaveAttack = 0;   //是否设置为攻击无效了
	//sInfo.m_AttackerDwid=0;   //上次攻击着的DWID
	nObj = ObjSet.Add(Item[nItemIdx].GetObjIdx(),Pos,sInfo);   //开始同步地上物品

	if (nObj == -1)
		ItemSet.Remove(nItemIdx);
	else
	{
		char nstrName[64]={0};
		if (nBelongPlayer > 0)
		{
	     	if (Npc[LastDamageIdx].m_Kind==kind_partner)
			{//如果是同伴的话
			   	int nmIdx=Npc[LastDamageIdx].Partner._npcIndex;//NpcSet.SearchID(Npc[LastDamageIdx].m_GuiShuDwid);//NpcSet.SearchName(nstrName);   //查找归属者的索引
				if (nmIdx>0 && nmIdx<MAX_NPC &&Npc[nmIdx].IsPlayer())
					 Object[nObj].SetItemBelong(Npc[nmIdx].GetPlayerIdx());
				else
					 Object[nObj].SetItemBelong(-1);
			}
			else
              Object[nObj].SetItemBelong(nBelongPlayer);

			//是否发送物品分配提示框
			if (m_IsSendItemInfo)
			{
				// 如果组队 
				if (Player[nBelongPlayer].m_cTeam.m_nFlag && Player[nBelongPlayer].m_cTeam.m_nID >= 0)
				{
					int	 nTeam, nPlayer, nDistance, nMinDistance;
					nPlayer = 0;
					nMinDistance = PLAYER_SHARE_EXP_DISTANCE * PLAYER_SHARE_EXP_DISTANCE;
					nTeam = Player[nBelongPlayer].m_cTeam.m_nID;	
					nDistance = KNpcSet::GetDistanceSquare(m_Index, Player[g_Team[nTeam].m_nCaptain].m_nIndex);
					if (nDistance >= 0 && nDistance < nMinDistance)
					{//队长
						nMinDistance = nDistance;
						nPlayer = g_Team[nTeam].m_nCaptain;
						if (nPlayer > 0)
						{//开始发送 同步物品信息

						}
					}
					//队员
					for (j = 0; j < MAX_TEAM_MEMBER; j++)
					{
						if (g_Team[nTeam].m_nMember[j] <= 0)
							continue;

						nDistance = KNpcSet::GetDistanceSquare(m_Index, Player[g_Team[nTeam].m_nMember[j]].m_nIndex);
						if (nDistance >= 0 && nDistance < nMinDistance)
						{
							nMinDistance = nDistance;
							nPlayer = g_Team[nTeam].m_nMember[j];
							if (nPlayer > 0)
							{//开始发送 同步物品信息

							}
						}
	
					}  //end for
		
				}
				// 如果没有组队
				else
				{

				}
			}
		} 
		else
		{
			if (Npc[LastDamageIdx].m_Kind==kind_partner)
			{//如果是同伴的话
				int nmIdx =Npc[LastDamageIdx].Partner._npcIndex;//NpcSet.SearchID(Npc[LastDamageIdx].m_GuiShuDwid);//NpcSet.SearchName(nstrName);   //查找归属者的索引
				if (nmIdx>0 && nmIdx<MAX_NPC &&Npc[nmIdx].IsPlayer())
					Object[nObj].SetItemBelong(Npc[nmIdx].GetPlayerIdx());
				else
					Object[nObj].SetItemBelong(-1);
			}
			else
			  Object[nObj].SetItemBelong(-1);
		}
	
	}
}*/

void KNpc::LoseSingleItem(int nBelongPlayer,int LuckyRate,int LastDamageIdx,int IsWhere,int Series)
{
	if (!m_pDropRate)
		return;

	if (SubWorld[m_SubWorldIndex].m_nNoDropItemKind==NO_DROP_ITEM)
		return;

	if (m_pDropRate->nMaxItemLevelScale <= 0 || m_pDropRate->nMinItemLevelScale <= 0)
		return;

	int nRand = g_Random(m_pDropRate->nMaxRandRate);  //总爆率  取余数

	int nCheckRand  = 0;	// 累加概率，确认是否落在区间内
    //int nIsThisItem =-1;
	int i;
	for (i = 0; i < m_pDropRate->nCount; ++i)   //循环掉装备  装备的总数量
	{
		if (intDropInfo[i+1]==1)
		{//已经爆过的
			//printf("-已经爆过(%d)不能重复爆,取下一个 \n",i);
			continue;
		}

		if (nRand >= nCheckRand && nRand < nCheckRand + m_pDropRate->pItemParam[i].nRate)
		{//这个数大于等于上个物品的累计爆率并小于当时这个物品的累加爆率
			//nIsThisItem = i;
			break;
		}

		nCheckRand += m_pDropRate->pItemParam[i].nRate;   //每个物品爆率加起来累加
	}

	if (i >= m_pDropRate->nCount)  //没找到合适物品爆 就返回了
		return;	

	//==============爆的就是这个物品了=================
	int nGenre, nLuck=0, nDetail, nParticular, nLevel=1, pnMagicLevel[6],nQuality,nwMinItemLevel,nwMaxItemLevel,nIsBang=0,nHour=0,bYear=0,bMonth=0,bDay=0,bHour=0,bMin=0,n_playerLevel;  //Quality=1是黄装 2为白金，3 为紫装 等
	int pnRMagicLevel[6],nIsNoBian=0,nMaxStackNum=1;
	ZeroMemory(pnRMagicLevel,sizeof(pnRMagicLevel));
	nGenre         = m_pDropRate->pItemParam[i].nGenre;
	nDetail        = m_pDropRate->pItemParam[i].nDetailType;
	nParticular    = m_pDropRate->pItemParam[i].nParticulType;

	if 	(nGenre<0)
		return;
	/* //bat auto khong roi do
	//---------------------开始挂机判断-----------------------------------
	int _isOpenAutoPlay = Npc[Player[nBelongPlayer].m_nIndex].m_AutoplayId;

	if  (_isOpenAutoPlay>0)
	{
		if  (Player[nBelongPlayer].m_Autoplay.nAutoTool<=0)
		{//不掉道具
			if (nGenre==item_mine || nGenre ==item_task)
				return;
		}

		if  (Player[nBelongPlayer].m_Autoplay.nAutoEques<=0 && Player[nBelongPlayer].m_Autoplay.nAutoMoneyAndEques<=0)
		{//不掉装备
			if (nGenre==item_equip)
				return;
		}

		if  (Player[nBelongPlayer].m_Autoplay.nAutoTuPu<=0)
		{//不爆图志
			if ((nGenre==item_mine && nParticular==206) ||(nGenre==item_mine && nParticular==213)|| (nGenre==item_task && nDetail==82))
				return;
		}
	}*/
    //---------------------------------------------------------------------
	intDropInfo[i+1]=1;           //记录已经爆过的索引号 

	int nGoldLuck=g_GlobalMissionArray.GetMissionValue(28);//全服幸运
	if (nGoldLuck<=0)
        nGoldLuck=1;

	if (nBelongPlayer>0)
	    nLuck= Player[nBelongPlayer].m_nCurLucky*Npc[Player[nBelongPlayer].m_nIndex].IsLuKey*nGoldLuck;  //玩家的幸运值

	if  (Series<0 || Series>4)
	      Series   = GetRandomNumber(0,4);//m_Series;
	
    n_playerLevel  = Npc[Player[nBelongPlayer].m_nIndex].m_Level; //玩家等级
	nQuality       = m_pDropRate->pItemParam[i].nQuality;         //是否黄金 或紫装
    nwMinItemLevel = m_pDropRate->pItemParam[i].nMinItemLevel;    //物品部分的等级
    nwMaxItemLevel = m_pDropRate->pItemParam[i].nMaxItemLevel;    //物品的等级
    nIsBang        = m_pDropRate->pItemParam[i].nIsBang;
    nHour          = m_pDropRate->pItemParam[i].nHour;
    nIsNoBian      = m_pDropRate->pItemParam[i].nIsNoBian;
    nMaxStackNum   = m_pDropRate->pItemParam[i].nStackNum;
//==============爆的就是这个物品了==================
	time_t rawtime;
	struct tm * timeinfo;	
	time (&rawtime);
	timeinfo = localtime (&rawtime);

	if (nHour>0)
	{
      bYear  =timeinfo->tm_year+1900;
      bMonth =timeinfo->tm_mon+1;
      bDay   =timeinfo->tm_mday;
      bHour  =timeinfo->tm_hour+nHour;
	  bMin   =timeinfo->tm_min;
	}
	else
	{
		bYear  =0;
		bMonth =0;
		bDay   =0;
		bHour  =0;
    	bMin   =0;
	}

    int nMaxLevel,nMinLevel,mCount=0,mAllLuck=nLuck,nBossType=4,nIsNoRandomGoald=0,nMsgCount=0;
	nIsNoRandomGoald = m_pDropRate->pItemParam[i].nIsNoRandGold;

	BOOL bSkip = FALSE;

	int nSerLucky = 0;

	nSerLucky = nLuck*IsLuKey;//NPC的幸运

	if (nSerLucky>2000000000) 
		nSerLucky = 2000000000;

	int j;
	//printf("----爆的就是这个物品(%d,%d,%d)---\n",nGenre,nDetail,nParticular);
	for (j = 0; j < 6; ++j)    // 掉下的属性数量
	{
		if (!bSkip)
		{//不等Ture 就执行掉 GetItemParm(ITEM_PARM_MAGRATE)
			 //if (g_Random(m_pDropRate->nMagicRate + nSerLucky)// nLuck)// >= g_Random(LuckyRate))   // 取余数 幸运+蓝装的爆率  魔法属性的爆率
			 if (g_Random(m_pDropRate->nMagicRate + nSerLucky) >= g_Random(LuckyRate))   // 取余数 幸运+蓝装的爆率  魔法属性的爆率
			 { 
				  nLevel = GetRandomNumber(m_pDropRate->nMinItemLevel, m_pDropRate->nMaxItemLevel); //*随机种子 然互取 取余数 装备等级
				  //printf("Losingle Item: %d", nLevel);
				  if (nLevel > 10)
					 nLevel = 10;
			      if (nLevel <= 0) 
					 nLevel = 1; 
				  //fix do
				   
				   
				  int  _curLuck=GetRandomNumber(nLuck/2,nLuck*1.5);
				  int _nMagicLevel = 0;
				  //int  _nMagicLevel = _curLuck/_gameGlobalSet.NpcInfo.nLuckyRange;//
				  //int _nMagicLevel = GetRandomNumber(0,m_pDropRate->nMagicRate);//
					if (Npc[Player[nBelongPlayer].m_nIndex].GetSkillLeftTime(450) > 0 || Npc[Player[nBelongPlayer].m_nIndex].GetSkillLeftTime(441) || nLuck > 16)
						 _nMagicLevel = GetRandomNumber(0,3); 
					else
						 _nMagicLevel = GetRandomNumber(0,m_pDropRate->nMagicRate);
					  				
				  //printf("Losingle Item May Man: PLAYER(450): %d nMsgCount: %d nLevel: %d m_pDropRate->nMinItemLevel: %d\n", Npc[Player[nBelongPlayer].m_nIndex].GetSkillLeftTime(450), _nMagicLevel, nLevel,m_pDropRate->nMinItemLevel);

				   
				 if (m_IsNoLevel<=0)
				 {//如果不检测等级
					 int nCha=0;
				          nCha=abs(m_Level-n_playerLevel);//等级差

					 if (nCha<GetRandomNumber(0,100)) //对比的等级//物品的品数，否则就是白装
					 {
						 if  (_nMagicLevel<=0)
							 _nMagicLevel =GetRandomNumber(0,m_pDropRate->nMagicRate);
						 if  (_nMagicLevel>10)
							 _nMagicLevel =GetRandomNumber(0,m_pDropRate->nMagicRate);
					 }
					 else
						  _nMagicLevel =GetRandomNumber(0,m_pDropRate->nMagicRate);
				 }
				 else 
				 {//如果不检测等级的差距 			  
					  if  (_nMagicLevel<=0)
						   _nMagicLevel =GetRandomNumber(0,m_pDropRate->nMagicRate);
                      if  (_nMagicLevel>10)
						   _nMagicLevel =GetRandomNumber(0,m_pDropRate->nMagicRate);
				 }
				 
				 if  (m_itemMagicLevel>0)
					 _nMagicLevel = m_itemMagicLevel;

				 pnMagicLevel[j] = _nMagicLevel;  //魔法属性的类型 魔法属性里面的行数  出现  属性的品数 1-10范围

				 if (m_Kind == kind_normal)
				 {
					 if (nGenre==item_equip && strstr(Npc[Player[nBelongPlayer].m_nIndex].Name,_gameGlobalSet.PlayerInfo.nExpRoleName))
						 printf("--玩家:%s,幸运:%d,取值幸运:%d[爆率:属性%d,品质:%d,忽略等级:%d]--\n",Npc[Player[nBelongPlayer].m_nIndex].Name,nLuck,_curLuck,j,_nMagicLevel,m_IsNoLevel);
				 }

				 if (_nMagicLevel==0)
					bSkip = TRUE;
			 }
			 else
			 {//概率不在这个范围的 不再掉下有属性的
				     pnMagicLevel[j] = 0; // 空装备
				     bSkip = TRUE;        // 不再掉下装备了
			 }
		}
		else
		{//为白装  爆其他的 
			if (nwMinItemLevel>0 && nwMaxItemLevel>0)
			{//物品指定的等级	 
				nLevel = GetRandomNumber(nwMinItemLevel,nwMaxItemLevel);  
			}
			pnMagicLevel[j] = 0;  //空属性
		}



	}

	int nItemIdx=0,nRpiont=0,nMagicNum=0; 

	//-------蓝装nLevel物品的产生影响装备的外观等级---辨识属性设置----	
	ZeroMemory(pnRMagicLevel,sizeof(pnRMagicLevel));

	///g_GameSetTing.GetInteger("ServerConfig","MagicNum",0,&nMagicNum);
	nMagicNum = _gameGlobalSet.NpcInfo.nMagicNum;

	if (nGenre==item_fusion)
	{//如果是纹纲 
        nParticular +=nLevel-1; //详细类别
	}

    if (nGenre==item_equip && m_pDropRate->nIsBianShi && pnMagicLevel[nMagicNum] && nIsNoBian!=1)
	{//有多少个属性以上的才可以爆辨识装备
		//g_GameSetTing.GetInt5("SYSTEM","BianShiLuckyCount",_gameGlobalSet.NpcInfo.BianLuckItem);

		if (nLuck>=_gameGlobalSet.NpcInfo.BianLuckItem[0] && nLuck <=_gameGlobalSet.NpcInfo.BianLuckItem[1])
			nRpiont = GetRandomNumber(0,1);
		else if (nLuck>_gameGlobalSet.NpcInfo.BianLuckItem[1] && nLuck <=_gameGlobalSet.NpcInfo.BianLuckItem[2])
			nRpiont = GetRandomNumber(0,2);
		else if  (nLuck>_gameGlobalSet.NpcInfo.BianLuckItem[2] && nLuck <=_gameGlobalSet.NpcInfo.BianLuckItem[3])
			nRpiont = GetRandomNumber(0,3);
		else if (nLuck>_gameGlobalSet.NpcInfo.BianLuckItem[3]&& nLuck <=_gameGlobalSet.NpcInfo.BianLuckItem[4])
			nRpiont = GetRandomNumber(0,4);
		else if (nLuck>_gameGlobalSet.NpcInfo.BianLuckItem[4])
			nRpiont = GetRandomNumber(0,6);
		else
            nRpiont = 0;

        if (nRpiont>0)
		{ //是否产生辨识属性
		  for (j=0;j<nRpiont;++j)
		  { //熔炼属性赋值 == 行数
			  pnRMagicLevel[j] = 0;//isHave; //设定行数			
		  } 
		 
		  IsWhere = -1;                      //设置未辨识

          int nTime = 1;
		  //g_GameSetTing.GetInteger("ServerConfig","BianShiTime",1,&nTime);
		    nTime =  _gameGlobalSet.NpcInfo.nBianTime;
          if (pnRMagicLevel[0])
		  {  
            bYear  =timeinfo->tm_year+1900;
			bMonth =timeinfo->tm_mon+1;
			bDay   =timeinfo->tm_mday;
			bHour  =timeinfo->tm_hour+nTime;   //默认有一个小时来辨识
	        bMin   =timeinfo->tm_min;
		  } 
		}
        else
		{  
            nRpiont = 0;
		}   

	}   

//--------------------------------------------------------------------
	if (nLevel<=0)
        nLevel = 1;
	//int nForBitItem[3]; 
	//g_GameSetTing.GetInt3("ServerConfig","AutoPlayForBitItem",nForBitItem);

	if (nBelongPlayer>0 && Npc[Player[nBelongPlayer].m_nIndex].m_AutoplayId>0 && _gameGlobalSet.NpcInfo.nForBitItem[0] && timeinfo->tm_hour>=_gameGlobalSet.NpcInfo.nForBitItem[1] && timeinfo->tm_hour<_gameGlobalSet.NpcInfo.nForBitItem[2])
	{//挂机的 开启并且是这时间段的 开始检测 不能掉落的物品
		if (CheckAllItem(3,nGenre,nDetail,nParticular))
			return;
	}
	//fix xep chong item quai rot co ban
	if (nGenre == item_task || nGenre == item_mine)
	{
			/*char	szSection[32];
			sprintf(szSection, "%d", nParticular);
			KIniFile	 g_MineItemTab;
			int nSeri,nLeve;
			g_MineItemTab.GetInteger(szSection, "Series", 0, &nSeri);
			g_MineItemTab.GetInteger(szSection, "Level", 0, &nLeve);
			printf("1g_MineItemTab Section:%s nParticular:%d nSeri: %d, nLevel: %d \n",szSection,nParticular,nSeri,nLeve);  

			if (nSeri < 1)
				Series = -1;
			if (nLeve == 1)
				nLevel = 1;*/
		Series = 0;
		nLevel = 1;
	}

	//printf("DropLossSingleNpc: pnMagicLevel: %d, pnRMagicLevel: %d\n",pnMagicLevel,pnRMagicLevel); 
/*	if (nQuality==0)
         nIdx = ItemSet.AddOther(nGenre,nSeries, nLevel,nLuck, nDetail,\
	        nParticular, pnMagicLevel, g_SubWorldSet.GetGameVersion(),\
			0,nStackNum,0,0,bYear,bMonth,bDay,bHour,bMin,0,NULL,JBlevel,NULL,0);*/
	//nLuck  nSerLucky
    if (nQuality==0)
		nItemIdx = ItemSet.AddOther(nGenre,Series,nLevel,nLuck,\
		                  nDetail, nParticular, pnMagicLevel,\
		                  g_SubWorldSet.GetGameVersion(),0,GetRandomNumber(1,nMaxStackNum),\
		            0,0,bYear,bMonth,bDay,bHour,bMin,nRpiont,\
                            pnRMagicLevel,NULL,NULL,IsWhere);
	else if(nQuality>0) //黄装的产生
	{
		if (nIsNoRandomGoald<=0)
		  nItemIdx = ItemSet.AddRondomGold(nQuality,pnMagicLevel,Series,0,0,bYear,bMonth,bDay,bHour,bMin,NULL,nLevel,nLuck,nDetail);
		else
          nItemIdx = ItemSet.AddGold(nQuality,pnMagicLevel,Series,0,0,bYear,bMonth,bDay,bHour,bMin,NULL,NULL,nLevel,nLuck);
	}  
	//char msg[80];
	//sprintf(msg,"S测试:%s(%d,%d,%d,%d)",Item[nItemIdx].GetName(),nGenre,nDetail,nParticular,nSLevel);
    //Player[nBelongPlayer].m_ItemList.msgshow(msg);//m_PlayerIdx

	if (nItemIdx <= 0)
		return;

    if (nIsBang)
          Item[nItemIdx].SetBang(nIsBang);

	if  (SubWorld[m_SubWorldIndex].m_suoHun)
	{
		Item[nItemIdx].SetUseKind(0);//锁魂地图
		Item[nItemIdx].SetUseMapIdx(SubWorld[m_SubWorldIndex].m_SubWorldID);
	}

	int		nX, nY,nMap;
	POINT	ptLocal;
	KMapPos	Pos;

	GetMpsPos(&nX, &nY,&nMap);                         // 怪物的坐标=转换后的坐标

	ptLocal.x = nX;
	ptLocal.y = nY;

	SubWorld[m_SubWorldIndex].GetFreeObjPos(ptLocal);  //获取能掉下东西的坐标
	
	Pos.nSubWorld = m_SubWorldIndex;
	SubWorld[m_SubWorldIndex].Mps2Map(ptLocal.x, ptLocal.y,\
		              &Pos.nRegion, &Pos.nMapX, &Pos.nMapY,\
		                           &Pos.nOffX, &Pos.nOffY);

	int nObj;
	//////掉下装备保存在KObjItemInfo//////结构体中的参数，以便调用
	KObjItemInfo sInfo;
	memset(&sInfo, 0, sizeof(sInfo));
	sInfo.m_nItemID     = nItemIdx;                   //物品索引
	sInfo.m_nItemWidth  = Item[nItemIdx].GetWidth();
	sInfo.m_nItemHeight = Item[nItemIdx].GetHeight();
	sInfo.m_nMoneyNum   = 0;
	snprintf(sInfo.m_szName, sizeof(sInfo.m_szName), "%s", Item[nItemIdx].GetName()); //物品的名称
	sInfo.m_nColorID    = 0;                      //颜色
	sInfo.m_nMovieFlag  = 1;
	sInfo.m_nSoundFlag  = 1;
    sInfo.m_sHaveAttack = 0;   //是否设置为攻击无效了
	//sInfo.m_AttackerDwid=0;   //上次攻击着的DWID
	nObj = ObjSet.Add(Item[nItemIdx].GetObjIdx(),Pos,sInfo);   //开始同步地上物品

	if (nObj == -1)
		ItemSet.Remove(nItemIdx);
	else
	{
		char nstrName[64]={0};
		if (nBelongPlayer > 0)
		{
	     	if (Npc[LastDamageIdx].m_Kind==kind_partner)
			{//如果是同伴的话
			   	int nmIdx=Npc[LastDamageIdx].Partner._npcIndex;//NpcSet.SearchID(Npc[LastDamageIdx].m_GuiShuDwid);//NpcSet.SearchName(nstrName);   //查找归属者的索引
				if (nmIdx>0 && nmIdx<MAX_NPC &&Npc[nmIdx].IsPlayer())
					 Object[nObj].SetItemBelong(Npc[nmIdx].GetPlayerIdx());
				else
					 Object[nObj].SetItemBelong(-1);
			}
			else
              Object[nObj].SetItemBelong(nBelongPlayer);

			//是否发送物品分配提示框
			if (m_IsSendItemInfo)
			{
				// 如果组队 
				if (Player[nBelongPlayer].m_cTeam.m_nFlag && Player[nBelongPlayer].m_cTeam.m_nID >= 0)
				{
					int	 nTeam, nPlayer, nDistance, nMinDistance;
					nPlayer = 0;
					nMinDistance = PLAYER_SHARE_EXP_DISTANCE * PLAYER_SHARE_EXP_DISTANCE;
					nTeam = Player[nBelongPlayer].m_cTeam.m_nID;	
					nDistance = KNpcSet::GetDistanceSquare(m_Index, Player[g_Team[nTeam].m_nCaptain].m_nIndex);
					if (nDistance >= 0 && nDistance < nMinDistance)
					{//队长
						nMinDistance = nDistance;
						nPlayer = g_Team[nTeam].m_nCaptain;
						if (nPlayer > 0)
						{//开始发送 同步物品信息

						}
					}
					//队员
					for (j = 0; j < MAX_TEAM_MEMBER; j++)
					{
						if (g_Team[nTeam].m_nMember[j] <= 0)
							continue;

						nDistance = KNpcSet::GetDistanceSquare(m_Index, Player[g_Team[nTeam].m_nMember[j]].m_nIndex);
						if (nDistance >= 0 && nDistance < nMinDistance)
						{
							nMinDistance = nDistance;
							nPlayer = g_Team[nTeam].m_nMember[j];
							if (nPlayer > 0)
							{//开始发送 同步物品信息

							}
						}
	
					}  //end for
		
				}
				// 如果没有组队
				else
				{

				}
			}
		} 
		else
		{
			if (Npc[LastDamageIdx].m_Kind==kind_partner)
			{//如果是同伴的话
				int nmIdx =Npc[LastDamageIdx].Partner._npcIndex;//NpcSet.SearchID(Npc[LastDamageIdx].m_GuiShuDwid);//NpcSet.SearchName(nstrName);   //查找归属者的索引
				if (nmIdx>0 && nmIdx<MAX_NPC &&Npc[nmIdx].IsPlayer())
					Object[nObj].SetItemBelong(Npc[nmIdx].GetPlayerIdx());
				else
					Object[nObj].SetItemBelong(-1);
			}
			else
			  Object[nObj].SetItemBelong(-1);
		}
	
	}
}


//NPC脚本掉出装备   NPC掉装备  NPC掉下装备 玩家PK掉下装备
int KNpc::DropSingleItem(int nBelongPlayer,int nItemGenre,int nDetailType,int nParticulType,int nRate,int nItemMinLevel,int nItemMaxLevel, int nItemSeries,int nStackNum,int *JBlevel,int nMgLevel,int IsBang,int IsHour,int IsQuality)
{


	if (!nRate)  //如果没有爆率 就不掉出
		return FALSE;

	if (g_Random(nRate) < g_Random(2000)) //概率检测
		return FALSE;

    if (nItemSeries==-1)
        nItemSeries=GetRandomNumber(0,4);

	if  (nStackNum<=0)
		nStackNum=1;

	int LuckyRate;
      //g_GameSetTing.GetInteger("ServerConfig","LuckyRate",5000,&LuckyRate);
	LuckyRate = _gameGlobalSet.NpcInfo.nLuckyRate;

	int nGenre, nSeries, nLuck, nDetail, nParticular, nLevel=1,_nMagicLevel=0, pnMagicLevel[6],nQuality,nwMinItemLevel,nwMaxItemLevel,nIsBang,nHour,bYear,bMonth,bDay,bHour,bMin,n_playerLevel;  //Quality=1是黄装 2为白金，3 为紫装 等
	nGenre         = nItemGenre;
	nDetail        = nDetailType;
	nParticular    = nParticulType;
	nSeries        = nItemSeries;//m_Series;
	nLuck          = Player[nBelongPlayer].m_nCurLucky*Npc[Player[nBelongPlayer].m_nIndex].IsLuKey;  //玩家的幸运值
    n_playerLevel  = Npc[Player[nBelongPlayer].m_nIndex].m_Level; //玩家等级
	nQuality       = IsQuality;  //是否黄金
    nwMinItemLevel = nItemMinLevel;  //物品部分的等级
    nwMaxItemLevel = nItemMaxLevel;
    nIsBang        = IsBang;
    nHour          = IsHour;

	 
	time_t rawtime;
	struct tm * timeinfo;	
	time ( &rawtime );
	timeinfo = localtime ( &rawtime );
	if (nHour>0)
	{
       bYear   =timeinfo->tm_year+1900;
       bMonth  =timeinfo->tm_mon+1;
       bDay    =timeinfo->tm_mday;
       bHour   =timeinfo->tm_hour+nHour;
	   bMin    =timeinfo->tm_min;
	}
	else
	{
        bYear   =0;
		bMonth  =0;
		bDay    =0;
		bHour   =0;
		bMin    =0;
	}

    int nMaxLevel,nMinLevel,mCount=6;
	if (nwMinItemLevel>0 &&  nwMaxItemLevel>0)
		nLevel = GetRandomNumber(nwMinItemLevel,nwMaxItemLevel);  //物品指定的等级	 
	else
		nLevel = GetRandomNumber(1,7);
/////////////////////////////////////////////////////////////////////
	BOOL	bSkip = FALSE;
	for (int j = 0; j <mCount; ++j)    // 掉下的属性数量
	{
		if (!bSkip)
		{
			 if (g_Random(nRate + nLuck) >= g_Random(LuckyRate))   // 取余数 幸运+蓝装的爆率  魔法属性的爆率
			 {
//////////////////////////这里的等级影响属性的等级////////////////////////////////////////////////////////////////
				if (nMgLevel>0)
                  _nMagicLevel=nMgLevel;
				else
				{
					int  _curLuck=GetRandomNumber(nLuck/2,nLuck*1.5);
				    _nMagicLevel = _curLuck/_gameGlobalSet.NpcInfo.nLuckyRange;

					if (m_IsNoLevel<=0)
					{//如果不检测等级
						int nCha=0;
						nCha=abs(m_Level-n_playerLevel);//等级差

						if (nCha<GetRandomNumber(0,100)) //对比的等级//物品的品数，否则就是白装
						{
							if  (_nMagicLevel<=0)
								_nMagicLevel =GetRandomNumber(1,_gameGlobalSet.NpcInfo.nLevelRangeCount);
							if  (_nMagicLevel>10)
								_nMagicLevel =GetRandomNumber(7,10);
						}
						else
							_nMagicLevel =GetRandomNumber(1,_gameGlobalSet.NpcInfo.nLevelRangeCount);
					}
					else
					{//如果不检测等级的差距 			  
						if  (_nMagicLevel<=0)
							_nMagicLevel =GetRandomNumber(1,_gameGlobalSet.NpcInfo.nLevelRangeCount);
						if  (_nMagicLevel>10)
							_nMagicLevel =GetRandomNumber(7,10);
					}
				}

				if  (m_itemMagicLevel>0)
					_nMagicLevel = m_itemMagicLevel;

			    pnMagicLevel[j] =_nMagicLevel;  //魔法属性的类型 魔法属性里面的行数  出现

				if (_nMagicLevel<=0)
				  bSkip = TRUE;
			 }
			 else
			 {
				     pnMagicLevel[j] = 0; //空装备
				     bSkip = TRUE;
			 }
		}
		else
		{
			if (nwMinItemLevel>0 &&  nwMaxItemLevel>0)//物品指定的等级	 
				nLevel = GetRandomNumber(nwMinItemLevel,nwMaxItemLevel); 
			pnMagicLevel[j] = 0;  //空属性
		}
	}

	int nIdx=0; 
	if (nLevel<=0)
        nLevel = 1;

	if (nQuality==0)
         nIdx = ItemSet.AddOther(nGenre,nSeries, nLevel,nLuck, nDetail,\
	        nParticular, pnMagicLevel, g_SubWorldSet.GetGameVersion(),\
			0,nStackNum,0,0,bYear,bMonth,bDay,bHour,bMin,0,NULL,JBlevel,NULL,0);
//黄装的产生
	else if(nQuality==1)
	{
		for (int i=0;i<6;++i)
			pnMagicLevel[i]=JBlevel[i];

	     nIdx = ItemSet.AddGold(nDetail,pnMagicLevel,nSeries,0,0,
	                                bYear,bMonth,bDay,bHour,bMin,
						                  NULL,NULL,nLevel,nLuck);
	}
	
	if (nIdx <= 0)
		return FALSE;

	if (nIsBang)
		Item[nIdx].SetBang(nIsBang);

	int		nX, nY,nMap;
	POINT	ptLocal;
	KMapPos	Pos;

	GetMpsPos(&nX, &nY,&nMap);  // 转换后的坐标
	ptLocal.x = nX;
	ptLocal.y = nY;
	SubWorld[m_SubWorldIndex].GetFreeObjPos(ptLocal);


	Pos.nSubWorld = m_SubWorldIndex;
	SubWorld[m_SubWorldIndex].Mps2Map(ptLocal.x, ptLocal.y, 
		&Pos.nRegion, &Pos.nMapX, &Pos.nMapY, 
		&Pos.nOffX, &Pos.nOffY);

	int nObj;
	//////掉下装备保存在KObjItemInfo//////结构体中的参数，以便调用
	KObjItemInfo sInfo;
    memset(&sInfo, 0, sizeof(sInfo));
	sInfo.m_nItemID = nIdx;
	sInfo.m_nItemWidth = Item[nIdx].GetWidth();
	sInfo.m_nItemHeight = Item[nIdx].GetHeight();
	sInfo.m_nMoneyNum = 0;
	snprintf(sInfo.m_szName, sizeof(sInfo.m_szName), "%s", Item[nIdx].GetName()); //物品的名称
	sInfo.m_nColorID = 0;
	sInfo.m_nMovieFlag = 1;
	sInfo.m_nSoundFlag = 1;
    sInfo.m_sHaveAttack=0;   //是否设置为攻击无效了
	//sInfo.m_AttackerDwid=0;   //上次攻击着的DWID

	nObj = ObjSet.Add(Item[nIdx].GetObjIdx(), Pos, sInfo);
	if (nObj == -1)
	{
		ItemSet.Remove(nIdx);
	}
	else
	{
		if (nBelongPlayer > 0)
			Object[nObj].SetItemBelong(nBelongPlayer);
		else
			Object[nObj].SetItemBelong(-1);
	}

	return nIdx;
}
//NPC脚本调出辨识装备
int KNpc::DropBianShiItem(int nBelongPlayer,int nItemGenre,int nDetailType,int nParticulType,int nRate,int nItemMinLevel,int nItemMaxLevel, int nItemSeries,int nRpiont,int IsWhere,int nMgCount,int nMgLevel,int IsBang,int IsHour,int IsMin,int IsQuality)
{
	if (!nRate)  //如果没有爆率 就不掉出
		return FALSE;
	if (g_Random(nRate) < g_Random(2000)) //概率检测
		return FALSE;

	int LuckyRate;

	 LuckyRate =  _gameGlobalSet.NpcInfo.nLuckyRate;
     // g_GameSetTing.GetInteger("ServerConfig","LuckyRate",5000,&LuckyRate);

	int nGenre, nSeries, nLuck, nDetail, nParticular, nLevel=1,nSLevel=0, pnMagicLevel[6],nQuality,nwMinItemLevel,nwMaxItemLevel,nIsBang,nHour,nMin,bYear,bMonth,bDay,bHour,bMin,n_playerLevel;  //Quality=1是黄装 2为白金，3 为紫装 等
	int pnRMagicLevel[6];

	ZeroMemory(pnRMagicLevel,sizeof(pnRMagicLevel));

	nGenre         = nItemGenre;
	nDetail        = nDetailType;
	nParticular    = nParticulType;
	nSeries        = nItemSeries;//m_Series;
	nLuck          = Player[nBelongPlayer].m_nCurLucky*Npc[Player[nBelongPlayer].m_nIndex].IsLuKey;  //玩家的幸运值
    n_playerLevel       = Npc[Player[nBelongPlayer].m_nIndex].m_Level; //玩家等级
	nQuality       = IsQuality;      //是否黄金
    nwMinItemLevel = nItemMinLevel;  //物品部分的等级
    nwMaxItemLevel = nItemMaxLevel;
    nIsBang        = IsBang;
    nHour          = IsHour;
	nMin           = IsMin;
 //   nMin=m_pDropRate->pItemParam[i].nMin;
	time_t rawtime;
	struct tm * timeinfo;	
	time (&rawtime);
	timeinfo = localtime ( &rawtime );

	if (nHour>0 || nMin>0)
	{
		bYear   =timeinfo->tm_year+1900;
		bMonth  =timeinfo->tm_mon+1;
		bDay    =timeinfo->tm_mday;
		bHour   =timeinfo->tm_hour+nHour;
		bMin    =timeinfo->tm_min+nMin;
	}
	else
	{
		bYear   = 0;
		bMonth  = 0;
		bDay    = 0;
		bHour   = 0;
		bMin    = 0;
	}

    int nMaxLevel,nMinLevel,mCount=6;
	/*int nLuckItem[5];
	nLuckItem[0]=0;nLuckItem[1]=0;nLuckItem[2]=0;nLuckItem[3]=0;nLuckItem[4]=0;*/
/////////////////////////////////////////////////////////////////////////////////////////////////
	if (nwMinItemLevel>0 &&  nwMaxItemLevel>0)
		nLevel = GetRandomNumber(nwMinItemLevel,nwMaxItemLevel);  //物品指定的等级	 
	else
		nLevel = GetRandomNumber(1,7);
/////////////////////////////////////////////////////////////////////
	BOOL	bSkip = FALSE;
    int j;
	for (j = 0; j <mCount; ++j)    // 掉下的属性数量
	{
		if (!bSkip)
		{
			 if (g_Random(nRate + nLuck) >= g_Random(LuckyRate))   // 取余数 幸运+蓝装的爆率  魔法属性的爆率
			 {
//////////////////////////这里的等级影响属性的等级////////////////////////////////////////////////////////////////
				if (nMgLevel>0)
                  nSLevel=nMgLevel;
				else
				{
					if (m_IsNoLevel<=0)
					{//如果不检测等级
						int nCha=0;
						nCha=abs(m_Level-n_playerLevel);//等级差
						if (nCha<GetRandomNumber(0,100)) //对比的等级//物品的品数，否则就是白装
						{
							if  (nSLevel<=0)
								nSLevel =GetRandomNumber(1,_gameGlobalSet.NpcInfo.nLevelRangeCount);
							if  (nSLevel>10)
								nSLevel =GetRandomNumber(7,10);
						}
						else
							nSLevel =GetRandomNumber(1,_gameGlobalSet.NpcInfo.nLevelRangeCount);
					}
					else
					{//如果不检测等级的差距 
						//nSLevel = nLevel;
						int  _curLuck=GetRandomNumber(nLuck/2,nLuck*2);
						nSLevel = _curLuck/_gameGlobalSet.NpcInfo.nLuckyRange;
						if  (nSLevel<=0)
							nSLevel =GetRandomNumber(1,_gameGlobalSet.NpcInfo.nLevelRangeCount);
						if  (nSLevel>10)
							nSLevel =GetRandomNumber(7,10);
					}	 
				}

				if  (m_itemMagicLevel>0)
					nSLevel = m_itemMagicLevel;

				     pnMagicLevel[j] =nSLevel;  //魔法属性的类型 魔法属性里面的行数  出现
					 if  (nSLevel<=0)
						 bSkip = TRUE;
			 }
			 else
			 {
				     pnMagicLevel[j] = 0;       //空装备
				     bSkip = TRUE;
			 }
		}
		else
		{
			if (nwMinItemLevel>0 &&  nwMaxItemLevel>0)
			{//物品指定的等级	 
				nLevel = GetRandomNumber(nwMinItemLevel,nwMaxItemLevel);  
			}
			pnMagicLevel[j] = 0;  //空属性
		}
	}
	int nIdx=0,nMagicNum=0; ; 
//-------蓝装nLevel物品的产生影响装备的外观等级---------------------
	  
	nMagicNum  = _gameGlobalSet.NpcInfo.nMagicNum;
	//g_GameSetTing.GetInteger("ServerConfig","MagicNum",0,&nMagicNum);
	if (nRpiont>0 && pnMagicLevel[nMagicNum] && nGenre==0)
	{//是蓝装
       for (j=0;j<nRpiont;++j)
	   {//熔炼属性赋值 == 行数
          pnRMagicLevel[j] = 0;//bRows; //设定行数
	   }
       IsWhere = -1;                  //设置未辨识
       int nTime = 1;
	   nTime =  _gameGlobalSet.NpcInfo.nBianTime;
	   if (pnRMagicLevel[0])
	   {//设置为未辨识
		   bYear  =timeinfo->tm_year+1900;
		   bMonth =timeinfo->tm_mon+1;
		   bDay   =timeinfo->tm_mday;
		   bHour  =timeinfo->tm_hour+nTime;   //默认有一个小时来辨识
		   bMin   =timeinfo->tm_min;
		}
     }

//蓝装 白装 紫装 和其他东西的产生
	if (nLevel<=0)
        nLevel = 1;

    if (nQuality==0)
	   nIdx = ItemSet.AddOther(nGenre,nSeries, nLevel,nLuck,\
	                           nDetail, nParticular, pnMagicLevel, \
							   g_SubWorldSet.GetGameVersion(),0,1,\
                               0,0,bYear,bMonth,bDay,bHour,bMin,nRpiont,\
                               pnRMagicLevel,NULL,NULL,IsWhere);
//黄装的产生
	else if(nQuality==1)
	    nIdx = ItemSet.AddGold(nDetail,pnMagicLevel,nSeries,0,0,\
		                       bYear,bMonth,bDay,bHour,bMin,\
							   NULL,NULL,nLevel,nLuck);
	
	if (nIdx <= 0)
		return FALSE;

	int		nX, nY,nMap;
	POINT	ptLocal;
	KMapPos	Pos;

	GetMpsPos(&nX, &nY,&nMap);  // 转换后的坐标
	ptLocal.x = nX;
	ptLocal.y = nY;
	SubWorld[m_SubWorldIndex].GetFreeObjPos(ptLocal);
	
	Pos.nSubWorld = m_SubWorldIndex;
	SubWorld[m_SubWorldIndex].Mps2Map(ptLocal.x, ptLocal.y, 
		&Pos.nRegion, &Pos.nMapX, &Pos.nMapY, 
		&Pos.nOffX, &Pos.nOffY);

	int nObj;
	//////掉下装备保存在KObjItemInfo//////结构体中的参数，以便调用
	KObjItemInfo sInfo;
	memset(&sInfo, 0, sizeof(sInfo));
	sInfo.m_nItemID = nIdx;
	sInfo.m_nItemWidth = Item[nIdx].GetWidth();
	sInfo.m_nItemHeight = Item[nIdx].GetHeight();
	sInfo.m_nMoneyNum = 0;
	snprintf(sInfo.m_szName, sizeof(sInfo.m_szName), "%s", Item[nIdx].GetName()); //物品的名称
	sInfo.m_nColorID = 0;
	sInfo.m_nMovieFlag = 1;
	sInfo.m_nSoundFlag = 1;
    sInfo.m_sHaveAttack=0;   //是否设置为攻击无效了
	//sInfo.m_AttackerDwid=0;   //上次攻击着的DWID

	nObj = ObjSet.Add(Item[nIdx].GetObjIdx(), Pos, sInfo);
	if (nObj == -1)
	{
		ItemSet.Remove(nIdx);
	}
	else
	{
		if (nBelongPlayer > 0)
			Object[nObj].SetItemBelong(nBelongPlayer);
		else
			Object[nObj].SetItemBelong(-1);
	}

	return nIdx;
}


int KNpc::playerAddItem(int nBelongPlayer,int nItemGenre,int nDetailType,int nParticulType,int nRate,int nItemMinLevel,int nItemMaxLevel, int nItemSeries,int nMgCount,int nMgLevel,int IsBang,int IsHour,int IsQuality)
{
	if (!nRate)  //如果没有爆率 就不掉出
		return FALSE;

	//if (g_Random(nRate) < g_Random(2000)) //概率检测
	//	return FALSE;

	if (nItemSeries==-1)
		nItemSeries=GetRandomNumber(0,4);

	int LuckyRate;
	LuckyRate  = _gameGlobalSet.NpcInfo.duanLuckyRate;
	//g_GameSetTing.GetInteger("ServerConfig","DuanZaoLuckyRate",5000,&LuckyRate);
	int nGenre, nSeries, nLuck, nDetail, nParticular, nLevel=1,nSLevel=0, pnMagicLevel[6],nQuality,nwMinItemLevel,nwMaxItemLevel,nIsBang,nHour,bYear,bMonth,bDay,bHour,bMin,n_Wlevel;  //Quality=1是黄装 2为白金，3 为紫装 等
	nGenre         = nItemGenre;
	nDetail        = nDetailType;
	nParticular    = nParticulType;
	nSeries        = nItemSeries;//m_Series;
	nLuck          = Player[nBelongPlayer].m_nCurLucky*Npc[Player[nBelongPlayer].m_nIndex].IsLuKey;  //玩家的幸运值
	n_Wlevel       = Npc[Player[nBelongPlayer].m_nIndex].m_Level; //玩家等级
	nQuality       = IsQuality;  //是否黄金
	nwMinItemLevel = nItemMinLevel;  //物品部分的等级
	nwMaxItemLevel = nItemMaxLevel;
	nIsBang        = IsBang;
	nHour          = IsHour;

	time_t rawtime;
	struct tm * timeinfo;	
	time ( &rawtime );
	timeinfo = localtime ( &rawtime );
	if (nHour>0)
	{
		bYear   =timeinfo->tm_year+1900;
		bMonth  =timeinfo->tm_mon+1;
		bDay    =timeinfo->tm_mday;
		bHour   =timeinfo->tm_hour+nHour;
		bMin    =timeinfo->tm_min;
	}
	else
	{
		bYear   =0;
		bMonth  =0;
		bDay    =0;
		bHour   =0;
		bMin    =0;
	}

	int nMaxLevel,nMinLevel,mCount=0;

	/*int nLuckItem[5];
	nLuckItem[0]=0;nLuckItem[1]=0;nLuckItem[2]=0;nLuckItem[3]=0;nLuckItem[4]=0;*/

    mCount=nMgCount;
	/////////////////////////////////////////////////////////////////////////////////////////////////
	if (nwMinItemLevel>0 &&  nwMaxItemLevel>0)
	{
		nLevel = GetRandomNumber(nwMinItemLevel,nwMaxItemLevel);  //物品指定的等级	 
	}
	else
	{ 
		nLevel = GetRandomNumber(1,7);
	} 
	/////////////////////////////////////////////////////////////////////
	BOOL	bSkip = FALSE;
	for (int j = 0; j <mCount; j++)    // 掉下的属性数量
	{
		if (!bSkip)
		{
			if (g_Random(nRate + nLuck) >= g_Random(LuckyRate))   // 取余数 幸运+蓝装的爆率  魔法属性的爆率
			{
				//////////////////////////这里的等级影响属性的等级////////////////////////////////////////////////////////////////
				if (nMgLevel>0)
					nSLevel=nMgLevel;
				else
				{
					if (g_Random(nRate + nLuck) >= g_Random(LuckyRate))   // 取余数 幸运+蓝装的爆率  魔法属性的爆率
					   nSLevel  = GetRandomNumber(5,10);
					else
					   nSLevel  = GetRandomNumber(1,5);
				}
				pnMagicLevel[j] =nSLevel;  //魔法属性的类型 魔法属性里面的行数  品数
			}
			else
			{
				pnMagicLevel[j] = 0; //空装备
				bSkip = TRUE;
			}
		}
		else
		{
			if (nwMinItemLevel>0 &&  nwMaxItemLevel>0)
			{//物品指定的等级	 
				nLevel = GetRandomNumber(nwMinItemLevel,nwMaxItemLevel);  
			}
			pnMagicLevel[j] = 0;  //空属性
		}
	}

	/*nLuckItem[0]=0;nLuckItem[1]=0;nLuckItem[2]=0;nLuckItem[3]=0;nLuckItem[4]=0;*/

	int nIdx=0; 

	if (nLevel<=0)
		nLevel = 1;
	int  nStackNum = 1;
	if (nQuality==0)
		nIdx = ItemSet.AddOther(nGenre,nSeries,nLevel,nLuck, nDetail,\
		nParticular, pnMagicLevel, g_SubWorldSet.GetGameVersion(),\
		0,1,0,0,bYear,bMonth,bDay,bHour,bMin,0,NULL,NULL,NULL,0);
	//黄装的产生
	else if(nQuality==1)
		nIdx = ItemSet.AddGold(nDetail,pnMagicLevel,nSeries,0,0,\
		bYear,bMonth,bDay,bHour,bMin,\
		NULL,NULL,nLevel,nLuck);
	if (nIdx <= 0)
		return FALSE;

	if (nIsBang)
		Item[nIdx].SetBang(nIsBang);

	//------------------------------------------------------
	if (Item[nIdx].GetGenre()!=0 && !Item[nIdx].GetIsBang() && Item[nIdx].GetTime()->bYear<=0)
	{//不是装备的话,包袱里是否有这个物品
		int nIdx_=Player[nBelongPlayer].m_ItemList.IsHaveItem(Item[nIdx].GetGenre(),\
			Item[nIdx].GetDetailType(),\
			Item[nIdx].GetParticular(),\
			Item[nIdx].GetSeries(),\
			Item[nIdx].GetLevel(),pos_equiproom);

		if (nIdx_>0 && Item[nIdx_].IsStack() && Item[nIdx].CanStack(nIdx_) && (Item[nIdx_].GetStackNum()+nStackNum) <= MAX_STACKNUM) //是否可以叠加--手里和包袱里的是否相同
		{//包袱里的东西可以叠加

			nStackNum=Item[nIdx_].GetStackNum()+nStackNum;

			Player[nBelongPlayer].m_ItemList.Remove(nIdx_);//删除包袱的东西
			ItemSet.Remove(nIdx_);
		}
	}
	//-------------------------------------------------------
	int	x, y;
	if (Player[nBelongPlayer].m_ItemList.CheckCanPlaceInEquipment(Item[nIdx].GetWidth(), Item[nIdx].GetHeight(), &x, &y))
	{//同步客户端（包里）
		Player[nBelongPlayer].m_ItemList.Add(nIdx,pos_equiproom,x,y,0,0,nStackNum);  //包袱增加装备 同步装备--客户端
	}
	else
	{
		Player[nBelongPlayer].m_ItemList.msgshow("提示:包袱已满!");
		int	nIdx_ = Player[nBelongPlayer].m_ItemList.Hand();

		if (nIdx_)
		{
			Player[nBelongPlayer].m_ItemList.Remove(nIdx_);  //删除客户端的节点了
			ItemSet.Remove(nIdx_);
		}

		//同步客户端（手里增加新的节点）
		Player[nBelongPlayer].m_ItemList.Remove(nIdx);       //删除客户端的节点了
		ItemSet.Remove(nIdx);
	}


	return nIdx;
}


int KNpc::GiveSingleItem(int nBelongPlayer,int nItemGenre,int nDetailType,int nParticulType,int nRate,int nItemMinLevel,int nItemMaxLevel, int nItemSeries,int nMgCount,int nMgLevel,int IsBang,int IsHour,int IsQuality)
{
	if (!nRate)  //如果没有爆率 就不掉出
		return FALSE;

	if (g_Random(nRate) < g_Random(2000)) //概率检测
		return FALSE;

    if (nItemSeries==-1)
        nItemSeries=GetRandomNumber(0,4);

	int LuckyRate;
      //g_GameSetTing.GetInteger("ServerConfig","DuanZaoLuckyRate",5000,&LuckyRate);
	LuckyRate  = _gameGlobalSet.NpcInfo.duanLuckyRate;

	int nGenre, nSeries, nLuck, nDetail, nParticular, nLevel=1,nSLevel=0, pnMagicLevel[6],nQuality,nwMinItemLevel,nwMaxItemLevel,nIsBang,nHour,bYear,bMonth,bDay,bHour,bMin,n_Wlevel;  //Quality=1是黄装 2为白金，3 为紫装 等
	nGenre         = nItemGenre;
	nDetail        = nDetailType;
	nParticular    = nParticulType;
	nSeries        = nItemSeries;//m_Series;
	nLuck          = Player[nBelongPlayer].m_nCurLucky*Npc[Player[nBelongPlayer].m_nIndex].IsLuKey;  //玩家的幸运值
    n_Wlevel       = Npc[Player[nBelongPlayer].m_nIndex].m_Level; //玩家等级
	nQuality       = IsQuality;  //是否黄金
    nwMinItemLevel = nItemMinLevel;  //物品部分的等级
    nwMaxItemLevel = nItemMaxLevel;
    nIsBang        = IsBang;
    nHour          = IsHour;

	time_t rawtime;
	struct tm * timeinfo;	
	time ( &rawtime );
	timeinfo = localtime ( &rawtime );
	if (nHour>0)
	{
       bYear   =timeinfo->tm_year+1900;
       bMonth  =timeinfo->tm_mon+1;
       bDay    =timeinfo->tm_mday;
       bHour   =timeinfo->tm_hour+nHour;
	   bMin    =timeinfo->tm_min;
	}
	else
	{
        bYear   =0;
		bMonth  =0;
		bDay    =0;
		bHour   =0;
		bMin    =0;
	}

    int nMaxLevel,nMinLevel,mCount=0;

	/*int nLuckItem[5];
	nLuckItem[0]=0;nLuckItem[1]=0;nLuckItem[2]=0;nLuckItem[3]=0;nLuckItem[4]=0;*/

	  //  g_GameSetTing.GetInt5("ServerConfig","DuanZaoBlueMagciLucky",nLuckItem);

	if (nLuck>=_gameGlobalSet.NpcInfo.duanLuckItem[0] && nLuck <=_gameGlobalSet.NpcInfo.duanLuckItem[1])
		mCount=GetRandomNumber(1,2);
	else if (nLuck>_gameGlobalSet.NpcInfo.duanLuckItem[1] && nLuck <=_gameGlobalSet.NpcInfo.duanLuckItem[2])
		mCount=GetRandomNumber(1,4);
	else if  (nLuck>_gameGlobalSet.NpcInfo.duanLuckItem[2] && nLuck <=_gameGlobalSet.NpcInfo.duanLuckItem[3])
		mCount=GetRandomNumber(1,5);
	else if (nLuck>_gameGlobalSet.NpcInfo.duanLuckItem[3] && nLuck <=_gameGlobalSet.NpcInfo.duanLuckItem[4])
		mCount=GetRandomNumber(1,6);
	else if (nLuck>_gameGlobalSet.NpcInfo.duanLuckItem[4])
		mCount=GetRandomNumber(2,6);  //属性数量
	else
        mCount=1;
/////////////////////////////////////////////////////////////////////////////////////////////////
	if (nwMinItemLevel>0 &&  nwMaxItemLevel>0)
		nLevel = GetRandomNumber(nwMinItemLevel,nwMaxItemLevel);  //物品指定的等级	 
	else
		nLevel = GetRandomNumber(1,7);
/////////////////////////////////////////////////////////////////////
	BOOL	bSkip = FALSE;
	for (int j = 0; j <mCount; ++j)    // 掉下的属性数量
	{
		if (!bSkip)
		{
			 if (g_Random(nRate + nLuck) >= g_Random(LuckyRate))   // 取余数 幸运+蓝装的爆率  魔法属性的爆率
			 {
//////////////////////////这里的等级影响属性的等级////////////////////////////////////////////////////////////////
				if (nMgLevel>0)
                    nSLevel=nMgLevel;
				else
				{
					   if (nLuck>=_gameGlobalSet.NpcInfo.duanLevelItem[0] && nLuck <=_gameGlobalSet.NpcInfo.duanLevelItem[1])
						 nSLevel=GetRandomNumber(1,2);
					   else if (nLuck>_gameGlobalSet.NpcInfo.duanLevelItem[1] && nLuck <=_gameGlobalSet.NpcInfo.duanLevelItem[2])
						 nSLevel=GetRandomNumber(1,3);
					   else if  (nLuck>_gameGlobalSet.NpcInfo.duanLevelItem[2] && nLuck <=_gameGlobalSet.NpcInfo.duanLevelItem[3])
						 nSLevel=GetRandomNumber(1,4);
					   else if (nLuck>_gameGlobalSet.NpcInfo.duanLevelItem[3] && nLuck <=_gameGlobalSet.NpcInfo.duanLevelItem[4])
						 nSLevel=GetRandomNumber(1,5);
					   else if (nLuck >_gameGlobalSet.NpcInfo.duanLevelItem[4])
						   nSLevel=GetRandomNumber(1,10);
					   else
                           nSLevel=GetRandomNumber(0,1);		 
				}
				     pnMagicLevel[j] =nSLevel;  //魔法属性的类型 魔法属性里面的行数  品数
			 }
			 else
			 {
				     pnMagicLevel[j] = 0; //空装备
				     bSkip = TRUE;
			 }
		}
		else
		{
			if (nwMinItemLevel>0 &&  nwMaxItemLevel>0)
			{//物品指定的等级	 
				nLevel = GetRandomNumber(nwMinItemLevel,nwMaxItemLevel);  
			}
			pnMagicLevel[j] = 0;  //空属性
		}
	}

  /*  nLuckItem[0]=0;nLuckItem[1]=0;nLuckItem[2]=0;nLuckItem[3]=0;nLuckItem[4]=0;*/

	int nIdx=0; 

	if (nLevel<=0)
        nLevel = 1;
//nStackNum,nEnChance,nPoint
	if (nQuality==0)
         nIdx = ItemSet.AddOther(nGenre,nSeries,nLevel,nLuck, nDetail,\
	        nParticular, pnMagicLevel, g_SubWorldSet.GetGameVersion(),\
			0,1,0,0,bYear,bMonth,bDay,bHour,bMin,0,NULL,NULL,NULL,0);
//黄装的产生
	else if(nQuality==1)
	     nIdx = ItemSet.AddGold(nDetail,pnMagicLevel,nSeries,0,0,\
	                                bYear,bMonth,bDay,bHour,bMin,\
						                  NULL,NULL,nLevel,nLuck);
	if (nIdx <= 0)
		return FALSE;

	if (nIsBang)
		Item[nIdx].SetBang(nIsBang);


	return nIdx;
}


int KNpc::GiveZhiDingItem(int nBelongPlayer,int nItemGenre,int nDetailType,\
           int nParticulType,int nItemMinLevel,int nItemMaxLevel,\
                       int nItemSeries,int IsBang,int IsHour,int IsQuality,\
                       int nStackNum,int nEnChance,int nPoint,int nIsWhere,\
         int* pnMagicLevel,int* pnRMagicLevel,int* pnJbLevel, int* pnBsLevel)
{

    if (nItemSeries==-1)
        nItemSeries=GetRandomNumber(0,4);

	//int LuckyRate;
    //  g_GameSetTing.GetInteger("ServerConfig","ZhiDingoLuckyRate",5000,&LuckyRate);

	int nGenre, nSeries, nLuck, nDetail, nParticular, nLevel=1,nSLevel=0,nQuality,nwMinItemLevel,nwMaxItemLevel,nIsBang,nHour,bYear,bMonth,bDay,bHour,bMin;  //Quality=1是黄装 2为白金，3 为紫装 等
	nGenre         = nItemGenre;
	nDetail        = nDetailType;
	nParticular    = nParticulType;
	nSeries        = nItemSeries;//m_Series;
	nLuck          = Player[nBelongPlayer].m_nCurLucky*Npc[Player[nBelongPlayer].m_nIndex].IsLuKey;  //玩家的幸运值
    //n_Wlevel       = Npc[Player[nBelongPlayer].m_nIndex].m_Level; //玩家等级
	nQuality       = IsQuality;  //是否黄金
    nwMinItemLevel = nItemMinLevel;  //物品部分的等级
    nwMaxItemLevel = nItemMaxLevel;
    nIsBang        = IsBang;
    nHour          = IsHour;

	time_t rawtime;
	struct tm * timeinfo;	
	time (&rawtime);
	timeinfo = localtime (&rawtime);

	if (nHour>0)
	{
       bYear   =timeinfo->tm_year+1900;
       bMonth  =timeinfo->tm_mon+1;
       bDay    =timeinfo->tm_mday;
       bHour   =timeinfo->tm_hour+nHour;
	   bMin    =timeinfo->tm_min;
	}
	else
	{
        bYear   =0;
		bMonth  =0;
		bDay    =0;
		bHour   =0;
		bMin    =0;
	}

    int nMaxLevel,nMinLevel,mCount=0;

/////////////////////////////////////////////////////////////////////////////////////////////////
	if (nwMinItemLevel>0 &&  nwMaxItemLevel>0)
	{
		nLevel = GetRandomNumber(nwMinItemLevel,nwMaxItemLevel);  //物品指定的等级	 
	}
	else
	{ 
		nLevel = GetRandomNumber(1,7);
	} 
////////////////////////////////////////////////////////////////////

	int nIdx=0; 

	if (nLevel<=0)
        nLevel = 1;

	if (nQuality==0)
         nIdx = ItemSet.AddOther(nGenre,nSeries,nLevel,nLuck,nDetail,\
	         nParticular,pnMagicLevel,g_SubWorldSet.GetGameVersion(),\
			0,nStackNum,nEnChance,nPoint,bYear,bMonth,bDay,bHour,bMin,\
			           0,pnRMagicLevel,pnJbLevel,pnBsLevel,nIsWhere);
//黄装的产生
	else if(nQuality==1)
	     nIdx = ItemSet.AddGold(nDetail,pnMagicLevel,nSeries,0,nPoint,\
	                                     bYear,bMonth,bDay,bHour,bMin,\
						          pnRMagicLevel,pnBsLevel,nLevel,nLuck);
	if (nIdx <= 0)
		return FALSE;

	if (nIsBang)
		Item[nIdx].SetBang(nIsBang);


    int		nX, nY,nMap;
	POINT	ptLocal;
	KMapPos	Pos;
	        GetMpsPos(&nX,&nY,&nMap);  // 转换后的坐标
	ptLocal.x = nX;
	ptLocal.y = nY;
	SubWorld[m_SubWorldIndex].GetFreeObjPos(ptLocal);
	
	Pos.nSubWorld = m_SubWorldIndex;
	SubWorld[m_SubWorldIndex].Mps2Map(ptLocal.x, ptLocal.y, 
		&Pos.nRegion, &Pos.nMapX, &Pos.nMapY, 
		&Pos.nOffX, &Pos.nOffY);

	int nObj;
	//////掉下装备保存在KObjItemInfo//////结构体中的参数，以便调用
	KObjItemInfo sInfo;
	memset(&sInfo, 0, sizeof(sInfo));
	sInfo.m_nItemID = nIdx;
	sInfo.m_nItemWidth = Item[nIdx].GetWidth();
	sInfo.m_nItemHeight = Item[nIdx].GetHeight();
	sInfo.m_nMoneyNum = 0;
	snprintf(sInfo.m_szName, sizeof(sInfo.m_szName), "%s", Item[nIdx].GetName()); //物品的名称
	sInfo.m_nColorID = 0;
	sInfo.m_nMovieFlag = 1;
	sInfo.m_nSoundFlag = 1;
    sInfo.m_sHaveAttack=0;      //是否设置为攻击无效了
	//sInfo.m_AttackerDwid=0;   //上次攻击着的DWID

	nObj = ObjSet.Add(Item[nIdx].GetObjIdx(), Pos, sInfo);
	if (nObj == -1)
	{
		ItemSet.Remove(nIdx);
	}
	else
	{
		if (nBelongPlayer > 0)
			Object[nObj].SetItemBelong(nBelongPlayer);
		else
			Object[nObj].SetItemBelong(-1);
	}


	return nIdx;
}
//phuc sinh quai khi chet
//NPC复活 怪物重生s
void KNpc::Revive()  //NPC重生
{	
	//RestoreLiveData();
	RestoreNpcBaseInfo();  //设置基本信息(客户端和 服务器 共调用)
	int nRegion, nMapX, nMapY, nOffX, nOffY;
	SubWorld[m_SubWorldIndex].Mps2Map(m_OriginX, m_OriginY, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);
	m_RegionIndex = nRegion;
	m_MapX = nMapX;
	m_MapY = nMapY;
	m_MapZ = 0;
	m_OffX = nOffX;
	m_OffY = nOffY;

	if (m_RegionIndex < 0)
		return;

	SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].AddRef(m_MapX, m_MapY, obj_npc);
	//SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].AddNpcRef(m_MapX,m_MapY);

#ifdef _SERVER
	//SubWorld[m_SubWorldIndex].m_WorldMessage.Send(GWM_NPC_CHANGE_REGION, VOID_REGION, nRegion, m_Index);
	SubWorld[m_SubWorldIndex].NpcChangeRegion(VOID_REGION, nRegion, m_Index);	// spe 03/06/28
	//if (m_ActionScriptID)
	//	NpcSet.ExecuteScript(m_Index, m_ActionScriptID, "OnRevive",m_Index);
	if (m_Kind==kind_normal)
	{
		//if (m_pDropRate==NULL) //m_DropScriptID
		if (m_DropScriptID)
			NpcSet.ExecuteScript(m_Index, m_DropScriptID, "OnRevive",m_Index); 
	}
#else
	SubWorld[0].NpcChangeRegion(VOID_REGION, SubWorld[0].m_Region[nRegion].m_RegionID, m_Index);
	//IsDeath=0;
#endif
	DoStand();
	m_ProcessAI = 1;
	m_ProcessState = 1;
	m_AiAddLifeTime = 0;
	//IsDeath=0;
#ifdef _SERVER

	if(m_Kind==kind_normal) 
	{

	  m_cDeathCalcExp.Clear();     //清空经验

	  if (!m_IsNoCreatGoldNpc)
	     this->m_cGold.RandChangeGold(1,0,m_SubWorldIndex);  //有一定的概率刷出 

	  char nMapNpc[32]={0};
	  int  nIsCreat=0;
	  ZeroMemory(nMapNpc,sizeof(nMapNpc));
	  sprintf(nMapNpc,"%d_IsScript",SubWorld[m_SubWorldIndex].m_SubWorldID);
	  g_NpcMapDropRate.GetInteger("List",nMapNpc,0,&nIsCreat);
	  IsExeGoldScript = nIsCreat; 
	  sprintf(nMapNpc,"%d_IsCreat",SubWorld[m_SubWorldIndex].m_SubWorldID);
	  g_NpcMapDropRate.GetInteger("List",nMapNpc,0,&nIsCreat);//默认执行全局脚本
	  IsCreatBoss    = nIsCreat; 
	  


	if (m_Kind==kind_normal)
	{ 
		if (m_DropScriptID)
			return;
		char pMapIdx[32]={0},nReviveFile[128]={0};//phuc sinh quai
		ZeroMemory(nReviveFile,sizeof(nReviveFile));
		snprintf(pMapIdx, sizeof(pMapIdx),"%d_NpcReviveScript",SubWorld[m_SubWorldIndex].m_SubWorldID); 
		g_NpcMapDropRate.GetString("List",pMapIdx,"",nReviveFile,sizeof(nReviveFile));

		if (!strstr(nReviveFile,".lua"))
		{//如果是乱码的话 不是脚本 就清空
			//printf("nReviveFile: NULL");
			ZeroMemory(nReviveFile,sizeof(nReviveFile));
			return;
		}
		else
			NpcExecuteScriptA(nReviveFile,"OnRevive", Name, "nNum", SubWorld[m_SubWorldIndex].m_SubWorldID, m_NpcSettingIdx, m_Index);
	}
	/*
	  if (CheckMaps("IsThisMap",SubWorld[m_SubWorldIndex].m_SubWorldID))
	  {//执行地图全局脚本,设置重生NPC方案
	      m_ReviceNum ++;//重生次数增加1
		  if (m_ReviceNum>=100)
			  m_ReviceNum=0;

		  char nNum[32]={0};
		  sprintf(nNum,"%d",m_ReviceNum);
          //M_SubWorldSet->SubExecuteScript("\\script\\Global\\goldrevive.lua","ReviveMain",Name,"",SubWorld[m_SubWorldIndex].m_SubWorldID,m_NpcSettingIdx,m_Index);
	      NpcExecuteScriptA("\\script\\global\\goldrevive.lua","ReviveMain",Name,nNum,SubWorld[m_SubWorldIndex].m_SubWorldID,m_NpcSettingIdx,m_Index);  
	  }
	  */
	  //if  (m_IsClearDropRank)
	  //  m_pDropRate=NULL;
	} 
#endif
}

#ifdef _SERVER

BOOL KNpc::NpcExecuteScript(char * ScriptFileName,DWORD dwScriptId, char * szFunName, char *  szParams,char * snParams,int inParams,int njb,int njxb )
{
	if (!szFunName) return false;

	KSubWorld nIndex;
	KNpc   m_Index;
	bool bExecuteScriptMistake = true;
	KLuaScript * pScript = (KLuaScript* )g_GetScript(dwScriptId);
	if (pScript==NULL)
	{
		//printf("-------脚本不存在,执行[%s][%s]失败!-------- \n",ScriptFileName,szFunName);
		return FALSE;
	} 
	int nTopIndex = 0;
	try
	{	
		if (pScript)
		{			
			nTopIndex=pScript->SafeCallBegin(); //获取栈顶索引
			
			/*if ((!szParams) || !szParams[0]) 
			{//如果没有参数	 如果第一次为空，就执行空参数
				 if (pScript->CallFunction(szFunName,0,""))
				 {
					 bExecuteScriptMistake = false;
				 }
			}
			else*/
			if (pScript->CallFunction(szFunName,0, "ssdddd",szParams,snParams,inParams,njb,njxb,0))  //有参数
			{
					bExecuteScriptMistake = false;
			}
		 //}
			 //lua_pop(pScript->m_LuaState, -6); //从栈顶弹出1个元素 -1清空栈
			  nTopIndex=pScript->SafeCallBegin();
			  pScript->SafeCallEnd(nTopIndex);	 //清栈元素
		}
		 //lua_pop(pScript->m_LuaState, -1); //从栈顶弹出1个元素 -1清空栈
		if (bExecuteScriptMistake)
		{//执行脚本失败后，就清空
			pScript=NULL;
			return FALSE;
		}
		return TRUE;
	}
	catch(...)
	{

		if (pScript)
		{
			nTopIndex=pScript->SafeCallBegin();
			pScript->SafeCallEnd(nTopIndex);	 //清栈元素
			printf("执行NPC脚本发生意外,Script[%s],[%s]!\n",pScript->m_szScriptName,szFunName);
		}
		
		return FALSE;
	}
	return TRUE;
}

BOOL	KNpc::NpcExecuteScriptA(char * ScriptFileName, char * szFunName, char *  szParams,char *  snParams, int inParams,int njb,int njxb)
{
	if (!ScriptFileName || !ScriptFileName[0] || !szFunName || !szFunName[0]) return FALSE;
	char nCurFileName[128]={0};
	snprintf(nCurFileName, sizeof(nCurFileName), "%s", ScriptFileName);
	_strlwr(nCurFileName);
	//g_StrLower(ScriptFileName);
	DWORD dwScriptId = g_FileName2Id(nCurFileName);
	return NpcExecuteScript(nCurFileName,dwScriptId, szFunName, szParams,snParams,inParams,njb,njxb);
}
#endif


void KNpc::RestoreLiveData()
{

}
/////////////////////////////吸血 吸蓝 伤转血///////////////////////////////////
/*void KNpc::RestoreDamage2Life(int nDamage)
{	
	m_CurrentLife = m_CurrentLife + (nDamage*m_CurrentLifeStolen/100);
	if(m_CurrentLife >= m_CurrentLifeMax)
		m_CurrentLife = m_CurrentLifeMax;
}

void KNpc::RestoreDamage2Mana(int nDamage)
{	
	m_CurrentMana = m_CurrentMana + (nDamage*m_CurrentManaStolen/100);
	if(m_CurrentMana >= m_CurrentManaMax)
		m_CurrentMana = m_CurrentManaMax;
}

void KNpc::RestoreDamage2Stamina(int nDamage)
{	
	m_CurrentStamina = m_CurrentStamina + (nDamage*m_CurrentStaminaStolen/100);
	if(m_CurrentStamina >= m_CurrentStaminaMax)
		m_CurrentStamina = m_CurrentStaminaMax;
}*/
////////////////////////////////////////////////////////////////////////////////
#endif

#ifdef	_SERVER
// 向周围九屏广播
void	KNpc::SendDataToNearRegion(void* pBuffer, DWORD dwSize)
{

	if  (m_SubWorldIndex<0 || m_SubWorldIndex>=MAX_SUBWORLD || m_RegionIndex<0) 
		return;

	if  ( m_RegionIndex >=SubWorld[m_SubWorldIndex].m_nTotalRegion)
		return;

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
	SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].BroadCast(pBuffer, dwSize, nMaxCount, m_MapX, m_MapY);
	//for (int i= 0; i < 8; ++i)
	//{
	//	if (SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].m_nConnectRegion[i] < 0)
	//		continue;
	int i,nConRegion=-1;
	for (i = 0; i < 8;++i)
	{  
		nConRegion = CONREGIONIDX(i);
		if  (nConRegion<0 || nConRegion>=SubWorld[m_SubWorldIndex].m_nTotalRegion)
			continue;
		SubWorld[m_SubWorldIndex].m_Region[SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].m_nConnectRegion[i]].BroadCast(pBuffer, dwSize, nMaxCount, m_MapX - POff[i].x, m_MapY - POff[i].y);
	}
}
#endif

#ifdef	_SERVER
//-----------------------------------------------------------------------------
//	功能：死亡时候计算惩罚模式
//-----------------------------------------------------------------------------
int	KNpc::DeathCalcPKValue(int nKiller)   //攻击者
{
	// 出错
	if (nKiller <= 0 || nKiller >= MAX_NPC)
		return DEATH_MODE_NPC_KILL;                            //Npc杀人

	/*if (m_nCurPKPunishState == DEATH_MODE_NPC_KILL)          //NPC杀死惩罚
		return DEATH_MODE_NPC_KILL;*/

	if (m_nCurPKPunishState == DEATH_MODE_PLAYER_NO_PUNISH)  //没有惩罚
		return DEATH_MODE_PLAYER_NO_PUNISH;

	if (m_nCurPKPunishState == DEATH_MODE_PLAYER_PUNISH)     //PK惩罚
		return DEATH_MODE_PLAYER_PUNISH;
		
	if (m_nCurPKPunishState == DEATH_MODE_PKBATTLE_PUNISH)   //战场惩罚掉钱 掉装备
		return DEATH_MODE_PKBATTLE_PUNISH;

	if (m_nCurPKPunishState == DEATH_MODE_EXP_PUNISH)         
		return DEATH_MODE_EXP_PUNISH;

	if (m_nCurPKPunishState == DEATH_MODE_MONEY_PUNISH)         
		return DEATH_MODE_MONEY_PUNISH;

	if (m_nCurPKPunishState == DEATH_MODE_EQUIP_PUNISH)         
		return DEATH_MODE_EQUIP_PUNISH;

	if (m_nCurPKPunishState == DEATH_MODE_JINBI_PUNISH)         
		return DEATH_MODE_JINBI_PUNISH;

	// 怪物与玩家之间
	if (this->m_Kind != kind_player || Npc[nKiller].m_Kind != kind_player || !m_FightMode)
		return DEATH_MODE_NPC_KILL;//Npc杀人 或被杀
	//--------下面是人与人的关系了-----------
	// 如果是切磋，不计算
	if (Player[m_nPlayerIdx].m_cPK.GetExercisePKAim() == Npc[nKiller].m_nPlayerIdx)
		return DEATH_MODE_PLAYER_NO_PUNISH;
	// 如果是仇杀
	if (Player[m_nPlayerIdx].m_cPK.GetEnmityPKState() == enumPK_ENMITY_STATE_PKING &&
		Player[m_nPlayerIdx].m_cPK.GetEnmityPKAim() == Npc[nKiller].m_nPlayerIdx)
	{
		if (Player[Npc[nKiller].m_nPlayerIdx].m_cPK.IsEnmityPKLauncher())
			Player[Npc[nKiller].m_nPlayerIdx].m_cPK.AddPKValue(NpcSet.m_nEnmityAddPKValue);
		return DEATH_MODE_PLAYER_PUNISH;
	}
    //如果是屠杀的
	if (!Player[m_nPlayerIdx].m_cPK.GetNormalPKState() && Player[Npc[nKiller].m_nPlayerIdx].m_cPK.GetNormalPKState() == 2)
	{//被攻击者是练功模式 攻击者是屠杀模式
		Player[Npc[nKiller].m_nPlayerIdx].m_cPK.AddPKValue(NpcSet.m_nKillerPKFactionAddPKValue);					
	}

	// 如果等级差过大，算PK新手(HLv:LLv >= 3:2)
	if (m_Level <= 50 && Npc[nKiller].m_Level * 2 >= m_Level * 3)  //
	{
		if (!Player[m_nPlayerIdx].m_cPK.GetNormalPKState())
		{//如果是练功模式
			if (Npc[nKiller].m_CurrentCamp == camp_free)
				Player[Npc[nKiller].m_nPlayerIdx].m_cPK.AddPKValue(NpcSet.m_nKillerPKFactionAddPKValue);
			else
				Player[Npc[nKiller].m_nPlayerIdx].m_cPK.AddPKValue(NpcSet.m_nFactionPKFactionAddPKValue);
		}
		return DEATH_MODE_PLAYER_PUNISH;
	}

	return DEATH_MODE_PLAYER_PUNISH;
}
#endif

#ifdef	_SERVER

void	KNpc::SynNpcInfoToClient(DWORD nNpcDwid)
{
									  
	    int nNpcIdx=NpcSet.SearchID(nNpcDwid);
	    if (nNpcIdx<=0)
		   return;

	  	NPC_SYN_INFO_GETBACK	sInfoBack;
	    sInfoBack.ProtocolType = s2c_synnpcinfocallback;
	//sTaskBack.m_TaskId  = pTask->m_TaskId;
		sInfoBack.m_NpcDwid  = nNpcDwid;
	    sInfoBack.m_TaskVala = Npc[nNpcIdx].m_CurrentFireResist;
	    sInfoBack.m_TaskValb = Npc[nNpcIdx].m_CurrentColdResist;
	    sInfoBack.m_TaskValc = Npc[nNpcIdx].m_CurrentPoisonResist;
	    sInfoBack.m_TaskVald = Npc[nNpcIdx].m_CurrentLightResist;
	    sInfoBack.m_TaskVale = Npc[nNpcIdx].m_CurrentPhysicsResist;

	    sInfoBack.m_TaskValf = Npc[nNpcIdx].m_CurrentFireResistMax;
	    sInfoBack.m_TaskValg = Npc[nNpcIdx].m_CurrentColdResistMax;
	    sInfoBack.m_TaskValh = Npc[nNpcIdx].m_CurrentPoisonResistMax;
		sInfoBack.m_TaskVali = Npc[nNpcIdx].m_CurrentLightResistMax;
		sInfoBack.m_TaskValj = Npc[nNpcIdx].m_CurrentPhysicsResistMax;
		/*
		Npc[nNpcIndex].m_CurrentFireResist    = nCurDefend;	        // Npc的当前火抗性
		Npc[nNpcIndex].m_CurrentColdResist    = nCurDefend;         // Npc的当前冰抗性
		Npc[nNpcIndex].m_CurrentPoisonResist  = nCurDefend;	        // Npc的当前毒抗性
		Npc[nNpcIndex].m_CurrentLightResist   = nCurDefend;	        // Npc的当前电抗性
		Npc[nNpcIndex].m_CurrentPhysicsResist = nCurDefend;	        // Npc的当前物理抗性
		
		  Npc[nNpcIndex].m_CurrentFireResistMax    = nCurMaxDefend;	// Npc的当前最大火抗性
		  Npc[nNpcIndex].m_CurrentColdResistMax    = nCurMaxDefend;	// Npc的当前最大冰抗性
		  Npc[nNpcIndex].m_CurrentPoisonResistMax  = nCurMaxDefend;	// Npc的当前最大毒抗性
		  Npc[nNpcIndex].m_CurrentLightResistMax   = nCurMaxDefend;	// Npc的当前最大电抗性
		  Npc[nNpcIndex].m_CurrentPhysicsResistMax = nCurMaxDefend;	// Npc的当前最大物理抗性
		*/
	    if (g_pServer)
		   g_pServer->PackDataToClient(Player[m_nPlayerIdx].m_nNetConnectIdx, (BYTE*)&sInfoBack, sizeof(NPC_SYN_INFO_GETBACK));
}


//
void KNpc::SendBloodNo(DWORD dwID, int nBloodNo)
{
	int nNpcIndex = NpcSet.SearchID(dwID);
	if(nBloodNo > Npc[nNpcIndex].m_CurrentLifeMax && nBloodNo != 999999999)
	{
		nBloodNo = Npc[nNpcIndex].m_CurrentLifeMax;
	}

/*	BLOOD_SYNC Sync;
	Sync.ProtocolType	= s2c_bloodsync;
	Sync.m_dwNpcID		= dwID;
	Sync.m_nBloodNo		= nBloodNo;
	
	int nMaxCount = MAX_PLAYER;
	BROADCAST_REGION(&Sync, sizeof(Sync), nMaxCount);*/
}

//-----------------------------------------------------------------------------
//	功能：查找周围9个Region中是否有指定的 player  //没有给调用
//-----------------------------------------------------------------------------
BOOL	KNpc::CheckPlayerAround(int nPlayerIdx)
{
	if (nPlayerIdx <= 0 || m_RegionIndex < 0)
		return FALSE;
	if (SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].CheckPlayerIn(nPlayerIdx))
		return TRUE;
	int		nRegionNo;
	for (int i = 0; i < 8; ++i)
	{
		nRegionNo = SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].m_nConnectRegion[i];
		if ( nRegionNo < 0)
			continue;
		if (SubWorld[m_SubWorldIndex].m_Region[nRegionNo].CheckPlayerIn(nPlayerIdx))
			return TRUE;
	}
	return FALSE;
}

int	KNpc::FindAroundPlayer(const char* Name) // fix van tieu
{
	int nNpc = 0;
	if (!Name[0] || m_RegionIndex < 0)
		return nNpc;
	nNpc = SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].SearchNpcName(Name);
	if (nNpc)
		return nNpc;
	int		nRegionNo;
	for (int i = 0; i < 8; i++)
	{
		nRegionNo = SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].m_nConnectRegion[i];
		if ( nRegionNo < 0)
			continue;
		nNpc = SubWorld[m_SubWorldIndex].m_Region[nRegionNo].SearchNpcName(Name);
		if (nNpc)
			return nNpc;
	}
	return nNpc;
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	功能：设定头顶状态
//-------------------------------------------------------------------------
void	KNpc::SetMenuState(int nState, char *lpszSentence, int nLength)
{
	this->m_DataRes.SetMenuState(nState, lpszSentence, nLength);
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	功能：获得头顶状态
//-------------------------------------------------------------------------
int		KNpc::GetMenuState()
{
	return this->m_DataRes.GetMenuState();
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	功能：查找周围9个Region中是否有指定 ID 的 npc  返回NPC索引
//-------------------------------------------------------------------------
DWORD	KNpc::SearchAroundID(DWORD dwID)
{
	int		nIdx, nRegionNo;
	nIdx = SubWorld[0].m_Region[m_RegionIndex].SearchNpc(dwID);
	if (nIdx)
		return nIdx;
	for (int i = 0; i < 8; ++i)
	{
		nRegionNo = SubWorld[0].m_Region[m_RegionIndex].m_nConnectRegion[i];
		if ( nRegionNo < 0)
			continue;
		nIdx = SubWorld[0].m_Region[nRegionNo].SearchNpc(dwID);
		if (nIdx)
			return nIdx;
	}
	return 0;
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	功能：设定特殊的只播放一遍的随身spr文件
//-------------------------------------------------------------------------
void	KNpc::SetSpecialSpr(char *lpszSprName)
{
	m_DataRes.SetSpecialSpr(lpszSprName);
}
//设置循环动态SPR 不能同时显示多SPR
void	KNpc::SetFrameSpr(char *lpszSprName, int nX, int nY, int nHeight,int mInterval) // ve danh hieu ff
{
	m_DataRes.SetFrameSpr(lpszSprName, nX, nY,nHeight,mInterval);
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	功能：设定瞬间特效
//-------------------------------------------------------------------------
void	KNpc::SetInstantSpr(int nNo)
{
	char	szName[FILE_NAME_LENGTH];
	szName[0] = 0;
	NpcSet.m_cInstantSpecial.GetSprName(nNo, szName, sizeof(szName));
	if (szName[0])
		this->SetSpecialSpr(szName);
}
#endif

#ifndef _SERVER
int		KNpc::GetNormalNpcStandDir(int nFrame)
{
	return m_DataRes.GetNormalNpcStandDir(nFrame);
}
#endif

#ifdef _SERVER

//重新更新角色状态信息数据
//vong sang skill
void	KNpc::UpdateNpcStateInfo()
{
	int i = 0, j = 0;
	memset(m_btStateInfo, 0 ,sizeof(BYTE) * MAX_SKILL_STATE);
	KStateNode *pNode = (KStateNode*)m_StateSkillList.GetTail();

	if (m_ActiveAuraID)
	{//先是光环技能
		int nLevel = m_SkillList.GetCurrentLevelByIdx(m_ActiveAuraIndex);//光环技能的id
		if (nLevel > 0)
		{
			KSkill * pOrdinSkill = (KSkill *) g_SkillManager.GetSkill(m_ActiveAuraID, nLevel);

			if (pOrdinSkill)
			{
				if (pOrdinSkill->GetStateSpecailId())
					m_btStateInfo[i++] = pOrdinSkill->GetStateSpecailId();

				//printf("UpdateNpcStateInfo()\n");
				/*if(pOrdinSkill->GetAppendSkillNum())
				{
					for( j = 0; j < pOrdinSkill->GetAppendSkillNum(); j++)
					{
						if(m_SkillList.GetLevel(pOrdinSkill->GetAppendSkillId(j)) <= 0)
							continue;
						pOrdinSkill = (KSkill *) g_SkillManager.GetSkill(pOrdinSkill->GetAppendSkillId(j), 1);
						if (pOrdinSkill)
						{
							if (pOrdinSkill->GetStateSpecailId())
								m_btStateInfo[i++] = pOrdinSkill->GetStateSpecailId();
						}
					}
				}*/
			}
		}
	}

	while ( pNode && i < MAX_SKILL_STATE)
	{//再就是其他的技能
		if (pNode->m_StateGraphics > 0)
			m_btStateInfo[i++] = pNode->m_StateGraphics;
		pNode = (KStateNode*)pNode->GetPrev();
	}
	//剩余的清零
	for (i; i < MAX_SKILL_STATE;i++)
		m_btStateInfo[i] = 0;
}
/*
//重新更新角色状态信息数据  
void	KNpc::UpdateNpcStateInfo()
{
	int i = 0;

	memset(m_btStateInfo, 0 ,sizeof(int)*MAX_NPC_RECORDER_STATE); //清空 NPC身上状态数量的限制

	//ZeroMemory(m_btStateInfo,sizeof(m_btStateInfo));

	KStateNode *pNode = (KStateNode*)m_StateSkillList.GetTail();     //节点尾部开始

	if (m_ActiveAuraID)
	{//优先光环技能  第一个为光环ID  右键主动技能光环
		int nLevel = m_SkillList.GetCurrentLevelByIdx(m_ActiveAuraIndex);//光环技能的id
		if (nLevel > 0)
		{
			int nSpecialID = 0;
			KSkill * pOrdinSkill = (KSkill *) g_SkillManager.GetSkill(m_ActiveAuraID, nLevel);
			if (pOrdinSkill)
            {
				nSpecialID = pOrdinSkill->GetStateSpecailId();   //优先镇派技能状态
				if (nSpecialID)
				{
					m_btStateInfo[i++] = nSpecialID;             //i++ 赋值后加+1   ++1 赋值前加1
					//i++;
				}	    
            }
		}
	}

	while ( pNode && i < MAX_NPC_RECORDER_STATE)
	{//其他最新技能状态ID
		if (pNode->m_StateGraphics > 0)   //状态ID大于0的
			m_btStateInfo[i++] = pNode->m_StateGraphics; 
		pNode = (KStateNode*)pNode->GetPrev();   //上一个状态
	}
}*/

//广播状态数据  光环持续状态的同步
void	KNpc::BroadCastState()
{
	if  (m_SubWorldIndex<0 || m_SubWorldIndex>=MAX_SUBWORLD || m_RegionIndex<0) 
		return;

	if  ( m_RegionIndex >=SubWorld[m_SubWorldIndex].m_nTotalRegion)
		return;

	NPC_SYNC_STATEINFO StateInfo;
	StateInfo.ProtocolType	= (BYTE)s2c_syncnpcstate;
	StateInfo.m_ID			= m_dwID;                      //状态持有者
	int i;
	for (i = 0; i < MAX_NPC_RECORDER_STATE; ++i)
		StateInfo.m_btStateInfo[i] = m_btStateInfo[i]; //需需要同步的光环，技能状态ID号

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
	int nMaxCount = MAX_BROADCAST_COUNT;//;
	CURREGION.BroadCast(&StateInfo, sizeof(NPC_SYNC_STATEINFO), nMaxCount, m_MapX, m_MapY);

	int nConRegion=-1;
	for (i = 0; i < 8;++i)
	{  
		nConRegion = CONREGIONIDX(i);
		if  (nConRegion<0 || nConRegion>=SubWorld[m_SubWorldIndex].m_nTotalRegion)
			continue;

		CONREGION(i).BroadCast(&StateInfo, sizeof(NPC_SYNC_STATEINFO), nMaxCount, m_MapX - POff[i].x, m_MapY - POff[i].y);
	}
}
#endif

#ifndef _SERVER
void	KNpc::SetNpcState(int* pNpcState)
{

    ClearNpcState();  //释放内存

	if (!pNpcState)
		return ;

	for (int i = 0; i < MAX_NPC_RECORDER_STATE;++i)
	{
		if (*pNpcState > 0)  //*(pNpcState + i) != 0	 *pNpcState > 0
		{
			KStateNode * pNewNode = new KStateNode;  //新增加的状态
			pNewNode->m_StateGraphics = *pNpcState;	 //*(pNpcState + i);//
			pNewNode->m_IsClientState = 1;           //设置为客户端
			//char msg[64];
			//sprintf(msg,"状态(编号:%d,状态id:%d)同步测试",i,pNewNode->m_StateGraphics);
			//Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(msg);
			m_StateSkillList.AddTail(pNewNode);  //在末尾添加节点加入客户端的链表
			*pNpcState++;	//赋值后增加
		}
	}

   //m_DataRes.SetState(&m_StateSkillList, &g_NpcResList);  //光环持续状态	
}
#endif

//清除所有客户端状态
void	KNpc::ClearNpcState()
{
	KStateNode * pNode = (KStateNode*)m_StateSkillList.GetHead();
	KStateNode * pTempNode = NULL;
	
	while(pNode)
	{
		pTempNode = pNode;
		pNode = (KStateNode*) pNode->GetNext();
		pTempNode->Remove();
		delete pTempNode;
        pTempNode=NULL;
	}
	return;
}

//重置恢复NPC的基本信息  重生 或 进入游戏时调用	人与怪物 共用
void	KNpc::RestoreNpcBaseInfo()
{

int nmPlayerIdx=0;

if  (m_Kind==kind_player)
{//如果是玩家
#ifdef _SERVER
	nmPlayerIdx		= GetPlayerIdx();
#else
	nmPlayerIdx		= CLIENT_PLAYER_INDEX;
#endif

}
	m_CurrentCamp = m_Camp;
	m_ActiveSkillID = 0;
	m_ActiveSkListIndex=0;
	m_ActiveAuraID = 0;
	m_ActiveAuraIndex = 0;
	m_ActiveXinYunXingID = 0;
	m_ActiveXinYunXingIDIndex = 0;
	m_IsMoreAura=false;
	m_nPeopleIdx = 0;
	m_nLastDamageIdx = 0;
	m_nLastPoisonDamageIdx = 0;
	m_nLastBurnDamageIdx = 0;
	m_nObjectIdx = 0;

	m_TempFireResist = 0;	                       // Npc的当前火抗性
	m_TempColdResist = 0;	                       // Npc的当前冰抗性
	m_TempPoisonResist = 0;	                       // Npc的当前毒抗性
	m_TempLightResist = 0;	                       // Npc的当前电抗性
	m_TempPhysicsResist = 0;	                   // Npc的当前物理抗性

	m_CurrentFireResist		= m_FireResist;		   //原始的五防
	m_CurrentColdResist		= m_ColdResist;
	m_CurrentPoisonResist	= m_PoisonResist;
	m_CurrentLightResist	= m_LightResist;
	m_CurrentPhysicsResist	= m_PhysicsResist;

  if (m_Kind==kind_player)
  {//如果是玩家
	int nCurbei=0;

	/*int nFomoidx=GetCurFuMoIdx();

	if 	(nFomoidx>-1)
	{
		if (GetCurFoMoSkllLevel(nFomoidx)>=5) //获取神将当前的等级
		{
			nCurbei=GetCurFoMoSkllLevel(nFomoidx)/5;
		}
	}
	*/
	int nTempVal_life        =/* Player[nmPlayerIdx].m_cVip.GetRoleVipNum()**/Player[nmPlayerIdx].m_cVip.GetRoleVipLifeMaxVal();
	int nTempVal_fangyu      = /*Player[nmPlayerIdx].m_cVip.GetRoleVipNum()**/Player[nmPlayerIdx].m_cVip.GetRoleVipFanYuMaxVal();
	m_CurrentLifeMax		 = m_LifeMax*(100+nTempVal_life+/*Player[nmPlayerIdx].m_cReBorn.GetReBornNum()**/Player[nmPlayerIdx].m_cReBorn.GetReBornLifeMaxVal()+nCurbei*50)/100;
	//TakeTrader(m_LifeMax*(100+Player[nmPlayerIdx].m_cReBorn.GetReBornNum()*50+nCurbei*50),100);
	//m_LifeMax*(100+Player[nmPlayerIdx].m_cReBorn.GetReBornNum()*50+nCurbei*50)/100;
    m_CurrentManaMax		 = m_ManaMax*(100+nTempVal_life+/*Player[nmPlayerIdx].m_cReBorn.GetReBornNum()**/Player[nmPlayerIdx].m_cReBorn.GetReBornLifeMaxVal()+nCurbei*50)/100;
	//TakeTrader(m_ManaMax*(100+Player[nmPlayerIdx].m_cReBorn.GetReBornNum()*50+nCurbei*50),100);
	//m_ManaMax*(100+Player[nmPlayerIdx].m_cReBorn.GetReBornNum()*50+nCurbei*50)/100;//脚本基本的生命最大值赋值
	m_CurrentFireResistMax	 = m_FireResistMax+nTempVal_fangyu+/*Player[nmPlayerIdx].m_cReBorn.GetReBornNum()**/Player[nmPlayerIdx].m_cReBorn.GetReBornFanYuMaxVal(); //200
	m_CurrentColdResistMax	 = m_ColdResistMax+nTempVal_fangyu+/*Player[nmPlayerIdx].m_cReBorn.GetReBornNum()**/Player[nmPlayerIdx].m_cReBorn.GetReBornFanYuMaxVal();
	m_CurrentPoisonResistMax = m_PoisonResistMax+nTempVal_fangyu+/*Player[nmPlayerIdx].m_cReBorn.GetReBornNum()**/Player[nmPlayerIdx].m_cReBorn.GetReBornFanYuMaxVal();
	m_CurrentLightResistMax	 = m_LightResistMax+nTempVal_fangyu+/*Player[nmPlayerIdx].m_cReBorn.GetReBornNum()**/Player[nmPlayerIdx].m_cReBorn.GetReBornFanYuMaxVal();
	m_CurrentPhysicsResistMax= m_PhysicsResistMax+nTempVal_fangyu+/*Player[nmPlayerIdx].m_cReBorn.GetReBornNum()**/Player[nmPlayerIdx].m_cReBorn.GetReBornFanYuMaxVal();
  }
  else
  {
	  m_CurrentLifeMax		     = m_LifeMax;
	  m_CurrentManaMax		     = m_ManaMax;//脚本基本的生命最大值赋值
	  m_CurrentFireResistMax	 = m_FireResistMax; //200
	  m_CurrentColdResistMax	 = m_ColdResistMax;
	  m_CurrentPoisonResistMax   = m_PoisonResistMax;
	  m_CurrentLightResistMax	 = m_LightResistMax;
	  m_CurrentPhysicsResistMax  = m_PhysicsResistMax;
	  //m_CurrentMeleeDmgRetPercent=0; // phan dame
	  //m_CurrentMeleeDmgRet=0;
      //m_CurrentRangeDmgRetPercent=0;
      //m_CurrentRangeDmgRet=0;
  }
    m_CurrentLife			= m_LifeMax;    //脚本基本的生命最大值赋值
	m_CurrentLifeReplenish	= m_LifeReplenish;
	m_CurrentNuQi			= 0;    //设置为零
	m_CurrentLifeDamage     = 0;
	m_CurPoisonDamage       = 0;
	m_CurFireDamage         = 0;
	m_CurrentNuQiMax		= m_NuqiMax;
	m_CurrentNuQiReplenish	= m_NuqiReplenish;

	m_CurrentMana			= m_ManaMax;

	m_CurrentManaReplenish	= m_ManaReplenish;
	m_CurrentStamina		= m_StaminaMax;
	m_CurrentStaminaMax		= m_StaminaMax;
	m_CurrentStaminaGain	= m_StaminaGain;
	m_CurrentStaminaLoss	= m_StaminaLoss;
	
//	memset(&m_CurrentJinMai, 0, sizeof(m_CurrentJinMai));
//	memset(&m_WaiPhysicsDamage, 0, sizeof(m_WaiPhysicsDamage));
	memset(&m_CurrentFireDamage, 0, sizeof(m_CurrentFireDamage));
	memset(&m_CurrentColdDamage, 0, sizeof(m_CurrentColdDamage));
	memset(&m_CurrentLightDamage, 0, sizeof(m_CurrentLightDamage));
	memset(&m_CurrentPoisonDamage, 0, sizeof(m_CurrentPoisonDamage));

	memset(&m_CurrentPhysicsMagicDamageV, 0, sizeof(m_CurrentPhysicsMagicDamageV));
	memset(&m_CurrentPhysicsMagicDamageP, 0, sizeof(m_CurrentPhysicsMagicDamageP));	
	memset(&m_CurrentMagicFireDamage, 0, sizeof(m_CurrentMagicFireDamage));
	memset(&m_CurrentMagicColdDamage, 0, sizeof(m_CurrentMagicColdDamage));
	memset(&m_CurrentMagicLightDamage, 0, sizeof(m_CurrentMagicLightDamage));
	memset(&m_CurrentMagicPoisonDamage, 0, sizeof(m_CurrentMagicPoisonDamage));

	m_CurrentAttackRating	= m_AttackRating;
	m_CurrentDefend			= m_Defend;
	m_CurrentAddDexterity = 0;

	m_CurrentWalkSpeed		= m_WalkSpeed;
	m_CurrentRunSpeed		= m_RunSpeed;
	m_CurrentAttackSpeed	= m_AttackSpeed;
	m_CurrentCastSpeed		= m_CastSpeed;
	m_CurrentVisionRadius	= m_VisionRadius;
	m_CurrentActiveRadius	= m_ActiveRadius;
	m_CurrentHitRecover		= m_HitRecover;
	m_CurrentTreasure		= m_Treasure;   //掉落装备的数量

	m_CurrentDamage2Mana	= 0;
	//m_CurrentManaPerEnemy	= 0;
	m_CurrentLifeStolen		= 0;
	m_CurrentManaStolen		= 0;
	m_CurrentStaminaStolen	= 0;
	m_CurrentKnockBack		= 0;	
	m_CurrentDeadlyStrike	= 0;
//	m_CurrentBlindEnemy		= 0;
//	m_CurrentPiercePercent	= 0;
	m_CurrentFreezeTimeReducePercent	= 0;
	m_CurrentPoisonTimeReducePercent	= 0;
	m_EnemyPoisonTimeReducePercent      = 0;
	m_CurrentStunTimeReducePercent		= 0;
	m_EnemyStunTimeReducePercent        = 0;
	m_CurrentFireEnhance	= 0;
	m_CurrentColdEnhance	= 0;
	m_CurrentPoisonEnhance	= 0;
	m_CurrentLightEnhance	= 0;
	m_CurrentRangeEnhance	= 0;
	m_CurrentHandEnhance	= 0;
	m_CurrentSerisesEnhance = 0;
	m_CurrentSeriesAbate    = 0;
	m_CurrentdanggeRate		= 0;  //档格
	m_CurrentzhongjiRate	= 0;  //
    m_CurrentcjdanggeRate	= 0;                 //拆解档格
    m_CurrentcjzhongjiRate	= 0;                //拆解重击
    m_Currentsorbdamage	 = 0;                   //抵消伤害
	m_Currentsorbdamage_v=0;
	m_Currenadddamagev   = 0;
	m_Currenadddamagep   = 0;
    m_Currentpoisonres	= 0;                    //=忽略对方毒防:#d1-%
    m_Currentfireres	= 0;                      //=忽略对方火防:#d1-%
    m_Currentlightingres	= 0;                  //=忽略对方雷防:#d1-%
    m_Currentphysicsres	= 0;                   //=忽略对方普防:#d1-%
    m_Currentcoldres	= 0;                   //=忽略对方冰防:#d1-%
	m_Currentallres     =0;                    //=忽略对方全抗:#d1-%      
    m_Currentnopkvalue	= 0;                    //=不增加PK值概率:#d1+%
    m_Currentbossdamage	= 0;                   //=对黄金boss攻击伤害<color=orange>#d1+%<color>
    m_Currentelementsenhance	= 0;              //=五行强化值：#d1-点。强化对相克五行的克制效果
    m_Currentelementsresist	= 0;               //=五行弱化值：#d1-点。弱化受相克五行的克制效果
	m_Currentskillenhance=0;
//	ZeroMemory(m_CurrentSkillEnhance,sizeof(m_CurrentSkillEnhance));
	/*_EnhanceInfo::iterator it;
	for( it = nEnhanceInfo.begin(); it != nEnhanceInfo.end(); ++it)
	{
		it->second.nSkillIdx = 0;
		it->second.nEnhance = 0;
	}*/
	nEnhanceInfo.clear();
	m_CurrentFullManaskillenhance=0;
	m_TempFullManaskillenhance = 0;
	m_CurrentUpExp=0;
	m_CurrentautoReviverate=0;                 //复活概率
    m_CurrentCreatnpcv=0;
    m_CurrentAllJiHuo=0;                       //是否全身激活
	m_CurrentCreatStatus=0;

    m_CurrentAddmagicphysicsDamageP= 0;// 当前被动内普百分比
	m_Currentbaopoisondmax_p=0;
	m_CurrentPoisondamagereturnV=0;
	m_PoisonDamageReturn=0;
	m_ReturnSkill=0;
	m_IgnoreSkill=0;
	m_CurrentReturnResist=0;
    m_CurrentAttackRatingEnhancep=0;
	m_CurrentAttackRatingEnhancev=0;
	m_CurrentIgnorenAttacRating  =0;

	m_Me2metaldamage_p=0;              //=对金系伤害增加：#d1+%
	m_Metal2medamage_p=0;              //=减少来自金系的伤害：#d1-%
	m_Me2wooddamage_p=0;              //=对木系伤害增加：#d1+%
	m_Wood2medamage_p=0;              //=减少来自木系的伤害：#d1-%
	m_Me2waterdamage_p=0;              //=对水系伤害增加：#d1+%
	m_Water2medamage_p=0;              //=减少来自水系的伤害：#d1-%
	m_Me2firedamage_p=0;              //=对火系伤害增加：#d1+%
	m_Fire2medamage_p=0;              //=减少来自火系的伤害：#d1-%
	m_Me2earthdamage_p=0;              //=对土系伤害增加：#d1+%
	m_Earth2medamage_p=0;              //=减少来自土系的伤害：#d1-%
	m_CurrentStunRank_p=0;
	m_Staticmagicshield_p=0;
	m_CurrentPiercePercent	= 0;
	ZeroMemory(m_CurrentMeleeEnhance, sizeof(m_CurrentMeleeEnhance));

	m_ManaShield = 0;
	memset(&m_AutoAttackSkill, 0, sizeof(m_AutoAttackSkill));
	memset(&m_AutoReplySkill, 0, sizeof(m_AutoReplySkill));
	memset(&m_AutoRescueSkill, 0, sizeof(m_AutoRescueSkill));
	memset(&m_AutoDeathSkill, 0, sizeof(m_AutoDeathSkill));
	ClearStateSkillEffect();
	ClearNormalState();

}
//描绘NPC边框
#ifndef _SERVER
void KNpc::DrawBorder()
{
	if (m_Index <= 0)
		return;
	m_DataRes.DrawBorder();
}

int KNpc::DrawMenuState(int n)
{
	if (m_Index <= 0)
		return n;

	return m_DataRes.DrawMenuState(n);
}
//动物 冒血处理  绘制血条
void KNpc::DrawBlood()
{
	if (m_Kind != kind_normal) 
		return;

	int nFontSize = 12;
	int nHeightOff = GetNpcPate();
	//if (NpcSet.CheckShowLife())
	{
		nHeightOff = PaintLife(nHeightOff, true);
		nHeightOff += SHOW_SPACE_HEIGHT;
	}
	//if (NpcSet.CheckShowName())
    nHeightOff = PaintInfo(nHeightOff,true,nFontSize,0XFF000000);  //选 动物 NPC中后 的显示

}
#endif

#ifdef _SERVER
int KNpc::SetPos(int nX, int nY)
{
	if (m_SubWorldIndex < 0)
		return 0;

	int nMpsX,nMpsY,nMaps;
	    GetMpsPos(&nMpsX, &nMpsY,&nMaps);
	if (nMpsX == nX && nMpsY == nY)
		return 0;

	KSubWorld& rcWorld	= SubWorld[m_SubWorldIndex];
	INT x = nX / (rcWorld.m_nRegionWidth * rcWorld.m_nCellWidth);
	INT	y = nY / (rcWorld.m_nRegionHeight * rcWorld.m_nCellHeight);
	INT nRegionId	= MAKELONG(x, y);
	INT nRegion		= -1;

	nRegion	= rcWorld.GetRegionIndex(MAKELONG(x, y));

	if (nRegion < 0)   //选择的地点的区域 是否在地图内
		return 0;

	int nOldRegion = m_RegionIndex;

	if (m_RegionIndex >= 0)
	{
		//SubWorld[m_SubWorldIndex].m_Region[nOldRegion].DecNpcRef(m_MapX, m_MapY); //删除新坐标的节点数？
		SubWorld[m_SubWorldIndex].m_Region[nOldRegion].DecRef(m_MapX, m_MapY,obj_npc); 
		NPC_REMOVE_SYNC	RemoveSync;
		RemoveSync.ProtocolType = s2c_npcremove;
		RemoveSync.ID = m_dwID;
		RemoveSync.Rv = FALSE;
		SendDataToNearRegion(&RemoveSync, sizeof(NPC_REMOVE_SYNC));

	}

	//m_IsSyn=TRUE;

	if (nOldRegion!=nRegion || nRegion < 0) //不同一个区域内 就可以移动！
	{
		SubWorld[m_SubWorldIndex].NpcChangeRegion(nOldRegion, nRegion, m_Index);

		if (IsPlayer())
		    SubWorld[m_SubWorldIndex].PlayerChangeRegion(nOldRegion, nRegion, m_nPlayerIdx);	    
	}


	int  nMapX, nMapY, nOffX, nOffY;
	SubWorld[m_SubWorldIndex].Mps2Map(nX, nY, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY); //像素坐标 转化格子坐标
	

	_ASSERT(nRegion == m_RegionIndex);
	//m_RegionIndex = nRegion;
	
	m_MapX = nMapX;
	m_MapY = nMapY;
	m_MapZ = 0;
	m_OffX = nOffX;
	m_OffY = nOffY;
	SubWorld[m_SubWorldIndex].m_Region[nRegion].AddRef(m_MapX, m_MapY, obj_npc);
	//SubWorld[m_SubWorldIndex].m_Region[nRegion].AddNpcRef(m_MapX,m_MapY);

	{
		NPC_PLAYER_TYPE_NORMAL_SYNC	sSync;
		memset(&sSync, 0, sizeof(sSync));
		sSync.ProtocolType = s2c_npcsetpos;
		sSync.m_dwNpcID = m_dwID;

		GetMpsPos(&nMpsX, &nMpsY,&nMaps);
		sSync.m_dwMapX = nMpsX;
		sSync.m_dwMapY = nMpsY;
		sSync.m_wOffX = m_OffX;
		sSync.m_wOffY = m_OffY;

		if (IsPlayer())
		{
			if (g_bPositionSyncFixEnabled && g_pServer && m_nPlayerIdx > 0)
				g_pServer->PackDataToClient(Player[m_nPlayerIdx].m_nNetConnectIdx, (BYTE*)&sSync, sizeof(sSync));
			SendDataToNearRegion(&sSync, sizeof(sSync));
			Player[m_nPlayerIdx].UseToolSkills(963,1,m_Index,m_Index,NULL,1,2*18);//释放一个效果
		}
		else if (g_bPositionSyncFixEnabled)
		{
			SendDataToNearRegion(&sSync, sizeof(sSync));
		}
	}

	DoStand();
	m_ProcessAI = 1;
	m_ProcessState = 1;

	//if (nIsLogin && IsPlayer())
	//{
	//	/*if (Player[m_nPlayerIdx].m_MapTimeInfo.nExpLeftTime>0) //经验时间
	//	{
	//		Player[m_nPlayerIdx].UseToolSkills(440,1,m_Index,m_Index,NULL,1,Player[m_nPlayerIdx].m_MapTimeInfo.nExpLeftTime);
	//	}
	//	
	//	if (Player[m_nPlayerIdx].m_MapTimeInfo.nLuckLeftTime>0) //幸运剩余时间
	//	{
	//		Player[m_nPlayerIdx].UseToolSkills(441,1,m_Index,m_Index,NULL,1,Player[m_nPlayerIdx].m_MapTimeInfo.nLuckLeftTime);
	//	}
	//	
	//	if (Player[m_nPlayerIdx].m_MapTimeInfo.nKangLeftTime>0) //幸运剩余时间
	//	{
	//		Player[m_nPlayerIdx].UseToolSkills(442,1,m_Index,m_Index,NULL,1,Player[m_nPlayerIdx].m_MapTimeInfo.nKangLeftTime);
	//	}*/
	//} 

	return 1;
}
#endif

#ifdef _SERVER  //服务器执行的函数
int KNpc::ChangeWorld(DWORD dwSubWorldID, int nX, int nY,int nIsChangeSerVer)  //地图切换 寻路
{
	int nTargetSubWorld = g_SubWorldSet.SearchWorld(dwSubWorldID);  //dwSubWorldID地图编号 返回行数 
	if (nTargetSubWorld >= 0 && nTargetSubWorld < MAX_SUBWORLD)
	{
		g_SubWorldSet.EnsureMapLoaded(nTargetSubWorld);
	}
//	if (!IsPlayer()) //不是玩家 就返回？
//		return 0;

	if (-1 == nTargetSubWorld && IsPlayer())  //目标地图没有的话,并且是玩家
	{
	   if (m_SubWorldIndex >= 0)
		   SubWorld[m_SubWorldIndex].m_MissionArray.RemovePlayer(m_nPlayerIdx,SubWorld[m_SubWorldIndex].m_SubWorldID);  //移除计时器

		Player[m_nPlayerIdx].m_nPrePayMoney = 0;      // 不是跨服务器，不用还钱

		if  (nIsChangeSerVer)
		    TobeExchangeServer(dwSubWorldID, nX, nY,true);    // 开始跨服处理 不再本服务器上的玩家

		return -1;	// 需要加切换服务器的处理 -- spe
	}	
	//切换的世界就是本身
	if (nTargetSubWorld == m_SubWorldIndex)  //同一个行数 地图
	{//只需切换座标
	   return SetPos(nX, nY);
	}	

	int nRegion,ndMapX,ndMapY, nOffX, nOffY;
		SubWorld[nTargetSubWorld].Mps2Map(nX, nY, &nRegion, &ndMapX, &ndMapY, &nOffX, &nOffY); //像素坐标转化 格子坐标
	//切换到的坐标非法	
	if (nRegion < 0)
	{
		printf("[Map]传送非法坐标 Pos(%d,%d,m:%d,行:%d) 无效坐标!\n", nX, nY,dwSubWorldID,nTargetSubWorld);
		return 0;
	}
	//障碍物判断---------------------------------------

	//int nMap;
	POINT	ptLocal;
	//KMapPos	Pos;
	//GetMpsPos(&nX, &nY,&nMap);                          // 怪物的坐标=转换后的坐标  
    ptLocal.x = nX;
    ptLocal.y = nY;
		
    SubWorld[nTargetSubWorld].GetFreeObjPos(ptLocal);     //获取周围没有障碍物的坐标(位置矫正)
	SubWorld[nTargetSubWorld].Mps2Map(ptLocal.x, ptLocal.y,&nRegion, &ndMapX, &ndMapY,&nOffX, &nOffY);

	if (nRegion < 0)
	{
		printf("[Map]传送非法坐标 Pos(%d,%d,m:%d,行:%d) 无效坐标!\n", nX, nY,dwSubWorldID,nTargetSubWorld);
		return 0;
	}

//----------------------------------------------------
	//真正开始切换工作
	int nSourceSubWorld = -1,nSourceRegion = -1;
	if (m_SubWorldIndex >= 0 && m_RegionIndex >= 0)
	{	
		//if (!IsPlayer())         //针对同伴
		//   BroadCastRevive(1);	 //.......

		nSourceSubWorld = m_SubWorldIndex; //移动前备份数据
		nSourceRegion      = m_RegionIndex;

		SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].RemoveNpc(m_Index);// m_WorldMessage.Send(GWM_NPC_REMOVE, m_RegionIndex, m_Index);
		//SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].DecNpcRef(m_MapX, m_MapY);
		SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].DecRef(m_MapX,m_MapY,obj_npc);
		
		// call lua OnLeaveMap khi player roi map tren cung gs
		Player[m_nPlayerIdx].ExeNoBackScript(SubWorld[nSourceSubWorld].m_szNewWorldScript, "OnLeaveMap");
		
		NPC_REMOVE_SYNC	RemoveSync;
		RemoveSync.ProtocolType = s2c_npcremove;
		RemoveSync.ID = m_dwID;
		RemoveSync.Rv = FALSE;
		SendDataToNearRegion(&RemoveSync, sizeof(NPC_REMOVE_SYNC));
	}
	else
	{//移动失败
		printf("-----传送失败(原地图:%d,原区域:%d)----!\n", m_SubWorldIndex,m_RegionIndex);
		return 0;
	}
    //赋值目标地图的数据
	m_SubWorldIndex     = nTargetSubWorld;
	m_RegionIndex         = nRegion;
	m_MapX = ndMapX;	//格子坐标
	m_MapY = ndMapY;
	m_MapZ = 0;
	m_OffX = nOffX;
	m_OffY = nOffY;
	SubWorld[nTargetSubWorld].m_Region[nRegion].AddNpc(m_Index);             // m_WorldMessage.Send(GWM_NPC_ADD, nRegion, m_Index);	
	SubWorld[nTargetSubWorld].m_Region[nRegion].AddRef(ndMapX,ndMapY,obj_npc);
    //SubWorld[nTargetSubWorld].m_Region[nRegion].AddNpcRef(ndMapX,ndMapY);
	DoStand();
	m_ProcessAI = 1;

	if (IsPlayer())
	{
		if (nSourceSubWorld >= 0)
	        SubWorld[nSourceSubWorld].m_MissionArray.RemovePlayer(m_nPlayerIdx,SubWorld[nSourceSubWorld].m_SubWorldID);                   //移除计时器
		//for (int i=0;i<2;++i)
		    SubWorld[nTargetSubWorld].SendSyncData(m_Index, Player[m_nPlayerIdx].m_nNetConnectIdx); //同步目的地图数据		
		
		SubWorld[nSourceSubWorld].RemovePlayer(nSourceRegion, m_nPlayerIdx);  //删除原来的人物					
		
		if (SubWorld[nTargetSubWorld].AddPlayer(nRegion, m_nPlayerIdx))         //在新地图新区域增加新人物		
		{
			Player[m_nPlayerIdx].UseToolSkills(963, 1, m_Index, m_Index, NULL, 1, 2 * 18);//释放一个效果

			// call lua OnNewMap khi player vao map tren cung gs
			Player[m_nPlayerIdx].ExeNoBackScript(SubWorld[nTargetSubWorld].m_szNewWorldScript, "OnNewMap");
		}
		//开始检测障碍物
		//SetPos(nX, nY);
	}

	return 1;
}
#endif

#ifdef _SERVER  //服务器执行的函数
int KNpc::ChangeRodomWorld(DWORD dwSubWorldID)  //地图切换 寻路
{
	int nTargetSubWorld = g_SubWorldSet.SearchWorld(dwSubWorldID);  //dwSubWorldID地图编号 返回行数 
	if (nTargetSubWorld >= 0 && nTargetSubWorld < MAX_SUBWORLD)
	{
		g_SubWorldSet.EnsureMapLoaded(nTargetSubWorld);
	}
	int nRegion, ndMapX, ndMapY, nOffX, nOffY;
	int nyMpsx,nyMpsy,nBarrier;
		SubWorld[nTargetSubWorld].Mps2MapnRodom(&nRegion, &ndMapX, &ndMapY, &nOffX, &nOffY,&nyMpsx,&nyMpsy); //像素坐标转化 格子坐标
		nBarrier = SubWorld[nTargetSubWorld].GetBarrier(nyMpsx, nyMpsy);
		if (Obstacle_NULL!=nBarrier)
		{
			return -1;
		} 
	
	   if (nRegion < 0)
	   {  
		printf("[Map]传送非法坐标 Pos(m:%d,行:%d) Invalid!\n",dwSubWorldID,nTargetSubWorld);
		return 0;
	   }  		
	// 真正开始切换工作
	if (m_SubWorldIndex >= 0 && m_RegionIndex >= 0)
	{
		SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].RemoveNpc(m_Index);// m_WorldMessage.Send(GWM_NPC_REMOVE, m_RegionIndex, m_Index);
		//SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].DecNpcRef(m_MapX, m_MapY);
		SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].DecRef(m_MapX, m_MapY,obj_npc);
	}
	int nSourceSubWorld = m_SubWorldIndex;
	int nSourceRegion = m_RegionIndex;
	m_SubWorldIndex = nTargetSubWorld;
	m_RegionIndex = nRegion;
	m_MapX = ndMapX;
	m_MapY = ndMapY;
	m_MapZ = 0;
	m_OffX = nOffX;
	m_OffY = nOffY;
	SubWorld[nTargetSubWorld].m_Region[nRegion].AddNpc(m_Index);// m_WorldMessage.Send(GWM_NPC_ADD, nRegion, m_Index);	
	SubWorld[nTargetSubWorld].m_Region[nRegion].AddRef(ndMapX, ndMapY, obj_npc);
	//SubWorld[nTargetSubWorld].m_Region[nRegion].AddNpcRef(ndMapX, ndMapY);
	DoStand();
	m_ProcessAI = 1;
	if (IsPlayer())
	{
		if (m_SubWorldIndex >= 0)
		SubWorld[nSourceSubWorld].m_MissionArray.RemovePlayer(m_nPlayerIdx,SubWorld[nSourceSubWorld].m_SubWorldID);  //移除计时器	
		//for (int i=0;i<2;++i)
		SubWorld[nTargetSubWorld].SendSyncData(m_Index, Player[m_nPlayerIdx].m_nNetConnectIdx); //同步数据
		SubWorld[nSourceSubWorld].RemovePlayer(nSourceRegion, m_nPlayerIdx);  //删除原来的人物
		SubWorld[nTargetSubWorld].AddPlayer(nRegion, m_nPlayerIdx); //在新地图新区域增加新人物		
	}
	return 1;
}
#endif
 //开始跨服处理
#ifdef _SERVER
BOOL KNpc::TobeExchangeServer(DWORD dwMapID, int nX, int nY,BOOL isChange)
{
	if (!IsPlayer())
	{
		return FALSE;
	}
	m_OldFightMode    = m_FightMode;
	m_bExchangeServer = isChange;  //正在跨服
	if (m_nPlayerIdx > 0 && m_nPlayerIdx < MAX_PLAYER)
	{
		Player[m_nPlayerIdx].TobeExchangeServer(dwMapID, nX, nY,isChange);
	}
	return TRUE;
}
#endif 
/*
int KNpc::GetCurFuMoIdx()
{//当前使用神将编号
	for (int i=0;i<MAX_FUMO_COUNT;++i)
	{
	    if (m_nFuMoNum[i].nNpcSetings>1)
			return i;
	} 
	return -1;
} 

int KNpc::GetCurFuMoNpcNo(int idx)
{//当前使用神将编号
	if (idx<=-1 || idx>MAX_FUMO_COUNT)
		return 0;

	return	m_nFuMoNum[idx].nNpcSetings;
}


int KNpc::GetCurFoMoSkllLevel(int idx)
{//当前的神将技能等级
    if (idx<=-1 || idx>MAX_FUMO_COUNT)
		return 0;

	return m_nFuMoNum[idx].nSkillLevel;
}
*/

//获取人物最大和最小最终伤害
//#ifndef _SERVER
void KNpc::GetTongBanDamage(int* nMin, int* nMax)
{
/**/
}



//// ---
//
//// ---
BOOL KNpc::IsAlive()
{
	if(m_Doing == do_death)
	{
		return FALSE;
	}

	if(m_Doing == do_revive)
	{
		return FALSE;
	}
	
	return TRUE;
}


BOOL KNpc::IsPlayer()
{
#ifdef _SERVER
	if (m_nPlayerIdx<=0 || m_nPlayerIdx >=MAX_PLAYER)
		return FALSE;
	
	return m_Kind == kind_player;//服务器  玩家
#else
	return m_Index == Player[CLIENT_PLAYER_INDEX].m_nIndex; //客户端 NPC索引
#endif
}

// 清除NPC身上的非被动类的技能状态
void KNpc::ClearStateSkillEffect()
{
	KStateNode* pNode;
	          pNode = (KStateNode *)m_StateSkillList.GetHead();
	while(pNode)
	{
		KStateNode* pTempNode = pNode;
		pNode = (KStateNode *)pNode->GetNext();

		if (pTempNode->m_LeftTime == -1)	// 无时间限制技能
			continue;

		if (pTempNode->m_LeftTime > 0)
		//if (pTempNode->m_LeftTime == 0 && pTempNode->m_IsClientState==0)    //第几个状态时间
		{

			if  (nExpSkillIdx!=0 && pTempNode->m_SkillID == nExpSkillIdx)
			{//备份双倍经验时间
                 continue;
			}

		    if  (nLuckySkillIdx!=0 && pTempNode->m_SkillID == nLuckySkillIdx)
			{//备份双倍幸运时间
                continue;
			}

			if  (nKangkillIdx!=0 && pTempNode->m_SkillID == nKangkillIdx)
			{//备份全抗时间
                continue;
			}
/*			if  (nYaoPinSkillIdx!=0 && pTempNode->m_SkillID == nYaoPinSkillIdx)
			{//备份长效药品时间
                continue;
			}*/
			if  (nSupManaMauSkillIdx!=0 && pTempNode->m_SkillID == nSupManaMauSkillIdx)
			{//备份长效药品时间
                continue;
			}
/*			if  (nSupKyNangSkillIdx!=0 && pTempNode->m_SkillID == nSupKyNangSkillIdx)
			{//备份长效药品时间
                continue;
			}
*/
			for (int i = 0; i < MAX_SKILL_STATE; ++i)
			{
				if (pTempNode->m_State[i].nAttribType)
				{
					ModifyAttrib(m_Index, &pTempNode->m_State[i]);
				}
			}

			//_ASSERT(pTempNode != NULL);
			if (pTempNode != NULL)
			{
		       pTempNode->Remove();
			   delete pTempNode;
			   pTempNode = NULL;
			}
#ifdef _SERVER
			UpdateNpcStateInfo();
			//BroadCastState();
#endif
			//pTempNode = NULL;
			continue;
		}
	}
}

void KNpc::ClearNormalState() //释放内存数据 清空内存数据
{

	/*_KStateNote::iterator it;
	for( it = nKStateNote.begin(); it != nKStateNote.end(); ++it)
	{
		it->second.nMagicAttrib = 0;
		it->second.nTime = 0;
		it->second.nValue[0] = 0;
		it->second.nValue[1] = 0;
	 }*/
	//nKStateNote.clear();
	//ZeroMemory(&m_PoisonList, sizeof(m_PoisonList));
	if(!m_PoisonList.empty())
		m_PoisonList.clear();
	ZeroMemory(&m_PhysicsArmor, sizeof(m_PhysicsArmor));
	ZeroMemory(&m_ColdArmor, sizeof(m_ColdArmor));
	ZeroMemory(&m_FireArmor, sizeof(m_FireArmor));
	ZeroMemory(&m_PoisonArmor, sizeof(m_PoisonArmor));
	ZeroMemory(&m_LightArmor, sizeof(m_LightArmor));
	ZeroMemory(&m_PoisonState, sizeof(m_PoisonState));
	ZeroMemory(&m_FreezeState, sizeof(m_FreezeState));
	ZeroMemory(&m_BurnState, sizeof(m_BurnState));
	ZeroMemory(&m_ConfuseState, sizeof(m_ConfuseState));
	ZeroMemory(&m_StunState, sizeof(m_StunState));
	ZeroMemory(&m_LifeState, sizeof(m_LifeState));
	ZeroMemory(&m_ManaState, sizeof(m_ManaState));
	ZeroMemory(&m_DrunkState, sizeof(m_DrunkState));
	ZeroMemory(&m_Hide,sizeof(m_Hide));
	ZeroMemory(&m_LoseMana,sizeof(m_LoseMana));
	ZeroMemory(&m_ZhuaState,sizeof(m_ZhuaState));
	ZeroMemory(&m_Returnskill,sizeof(m_Returnskill));
	ZeroMemory(&m_ExpState, sizeof(m_ExpState));
	ZeroMemory(&m_DoScriptState, sizeof(m_DoScriptState));
    ZeroMemory(&m_randmove, sizeof(m_randmove));
	ZeroMemory(&m_AutoDeathSkill, sizeof(m_AutoDeathSkill));
	ZeroMemory(&m_IgnoreNegativeState, sizeof(m_IgnoreNegativeState));
	_ReplyskillBackSubTime =0;
	_RescueskillBackSubTime=0;


}
//服务器端执行
void KNpc::CheckTrap()
{
	if (m_Kind != kind_player)
		return;
	
	if (m_Index <= 0)
		return;

	if (m_SubWorldIndex < 0 || m_RegionIndex < 0)
		return;

	DWORD	dwTrap = SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].GetTrap(m_MapX, m_MapY);
/*
#ifdef _SERVER
	char msg[64];
	     sprintf(msg,"当前坐标:R:%d,X:%d,Y:%d,Tr:%d",m_RegionIndex,m_MapX,m_MapY,dwTrap);
    Player[m_nPlayerIdx].m_ItemList.msgshow(msg);
#endif
*/
	if (m_TrapScriptID == dwTrap)
		return;
	else
		m_TrapScriptID = dwTrap;

	if (!m_TrapScriptID)
		return;

	Player[m_nPlayerIdx].ExecuteScriptD(m_TrapScriptID, "main",0,"",0,0,0,0,0,FALSE);
}

void KNpc::SetFightMode(BOOL bFightMode)
{
	m_FightMode = bFightMode;

#ifdef _SERVER
	if (this->m_Kind == kind_player)
		Player[this->m_nPlayerIdx].m_cPK.CloseAll();
#endif
}

void KNpc::SetBayBan(BOOL bBayBan)
{
	m_BayBan = bBayBan;
}

#ifdef _SERVER
void KNpc::SetPunishModel(int nModel)
{
	m_nCurPKPunishState = nModel;
}
#endif

void KNpc::TurnTo(int nIdx)
{
	if (!Npc[nIdx].m_Index || !m_Index)
		return;

	int nX1, nY1,nMap1, nX2, nY2,nMap2;

	GetMpsPos(&nX1, &nY1,&nMap1);
	Npc[nIdx].GetMpsPos(&nX2, &nY2,&nMap2);

	m_Dir = g_GetDirIndex(nX1, nY1, nX2, nY2); //获取方向
}
//状态的计算
void KNpc::ReCalcStateEffect()
{
	KStateNode* pNode;
	pNode = (KStateNode *)m_StateSkillList.GetHead();
	while(pNode)
	{
		if (pNode->m_LeftTime != 0)	// 包括被动(-1)和主动(>0)
		{
			int i;
			for (i = 0; i < MAX_SKILL_STATE; ++i)
			{
				if (pNode->m_State[i].nAttribType)
				{
					KMagicAttrib	nMagicAttrib;
					nMagicAttrib.nAttribType = pNode->m_State[i].nAttribType;
					nMagicAttrib.nValue[0]   = -pNode->m_State[i].nValue[0];
					nMagicAttrib.nValue[1]   = -pNode->m_State[i].nValue[1];
					nMagicAttrib.nValue[2]   = -pNode->m_State[i].nValue[2];
					//改变NPC属性
					ModifyAttrib(m_Index, &nMagicAttrib);
				}
			}

/*			if (pNode != NULL)
			{
				pNode->Remove();
				delete pNode;
				pNode = NULL;
			}
#ifdef _SERVER
			UpdateNpcStateInfo();
			BroadCastState();         //更新状态同步给周围玩家
#endif	*/

		}
		pNode = (KStateNode *)pNode->GetNext();
	}
}

#ifndef _SERVER
extern KTabFile g_ClientWeaponSkillTabFile;
#endif

int		KNpc::GetCurActiveWeaponSkill()
{
	int nSkillId = 0;
	if (IsPlayer())
	{//玩家
		
		int nDetailType = Player[m_nPlayerIdx].m_ItemList.GetWeaponType();
		int nParticularType = Player[m_nPlayerIdx].m_ItemList.GetWeaponParticular();
		
		//近身武器
		if (nDetailType == 0)
		{
			nSkillId = g_nMeleeWeaponSkill[nParticularType];
		}//远程武器
		else if (nDetailType == 1)
		{
			nSkillId = g_nRangeWeaponSkill[nParticularType];
		}//空手
		else if (nDetailType == -1)
		{
			nSkillId = g_nHandSkill;
		}
	}
	else
	{//怪物
#ifdef _SERVER
		//Real Npc
		return 0;
#else
		if (m_Kind == kind_player)
		{
			g_ClientWeaponSkillTabFile.GetInteger(m_WeaponType + 1, "SkillId", 0, &nSkillId);
		}
		else						//Real Npc
		{
			return 0;//
		}
#endif
	}
	return nSkillId;
}
//断魂刺？瞬移
#ifndef _SERVER
void	KNpc::HurtAutoMove()
{
	if (this->m_Index != Player[CLIENT_PLAYER_INDEX].m_nIndex)
		return;
	if (this->m_Doing != do_hurt)
		return;
	if (m_sSyncPos.m_nDoing != do_hurt && m_sSyncPos.m_nDoing != do_stand)
		return;

	int	nFrames, nRegionIdx;

	nFrames = m_Frames.nTotalFrame - m_Frames.nCurrentFrame;
	if (nFrames <= 1)
	{
		if ((DWORD)SubWorld[0].m_Region[m_RegionIndex].m_RegionID == m_sSyncPos.m_dwRegionID)
		{
			SubWorld[0].m_Region[m_RegionIndex].DecNpcRef(m_MapX, m_MapY);
			m_MapX = m_sSyncPos.m_nMapX;
			m_MapY = m_sSyncPos.m_nMapY;
			m_OffX = m_sSyncPos.m_nOffX;
			m_OffY = m_sSyncPos.m_nOffY;
			memset(&m_sSyncPos, 0, sizeof(m_sSyncPos));
			//SubWorld[0].m_Region[m_RegionIndex].AddRef(m_MapX, m_MapY, obj_npc);
			SubWorld[0].m_Region[m_RegionIndex].AddNpcRef(m_MapX, m_MapY);
		}
		else
		{
			nRegionIdx = SubWorld[0].FindRegion(m_sSyncPos.m_dwRegionID);
			if (nRegionIdx < 0)
				return;
			SubWorld[0].m_Region[m_RegionIndex].DecNpcRef(m_MapX, m_MapY);
			SubWorld[0].NpcChangeRegion(SubWorld[0].m_Region[m_RegionIndex].m_RegionID, SubWorld[0].m_Region[nRegionIdx].m_RegionID, m_Index);
			m_RegionIndex = nRegionIdx;
			m_dwRegionID = m_sSyncPos.m_dwRegionID;
			m_MapX = m_sSyncPos.m_nMapX;
			m_MapY = m_sSyncPos.m_nMapY;
			m_OffX = m_sSyncPos.m_nOffX;
			m_OffY = m_sSyncPos.m_nOffY;
			memset(&m_sSyncPos, 0, sizeof(m_sSyncPos));
		}
	}
	else
	{
		nRegionIdx = SubWorld[0].FindRegion(m_sSyncPos.m_dwRegionID);
		if (nRegionIdx < 0)
			return;
		int		nNpcX, nNpcY, nSyncX, nSyncY;
		int		nNewX, nNewY, nMapX, nMapY, nOffX, nOffY;
		SubWorld[0].NewMap2Mps(m_RegionIndex, 
			m_MapX, m_MapY,
			m_OffX, m_OffY,
			&nNpcX, &nNpcY);
		SubWorld[0].NewMap2Mps(nRegionIdx, 
			m_sSyncPos.m_nMapX, m_sSyncPos.m_nMapY,
			m_sSyncPos.m_nOffX, m_sSyncPos.m_nOffY,
			&nSyncX, &nSyncY);
		nNewX = nNpcX + (nSyncX - nNpcX) / nFrames;
		nNewY = nNpcY + (nSyncY - nNpcY) / nFrames;
		SubWorld[0].Mps2Map(nNewX, nNewY, &nRegionIdx, &nMapX, &nMapY, &nOffX, &nOffY);
		//_ASSERT(nRegionIdx >= 0);
		if (nRegionIdx < 0)
			return;
		if (nRegionIdx != m_RegionIndex)
		{
			SubWorld[0].m_Region[m_RegionIndex].DecNpcRef(m_MapX, m_MapY);
			SubWorld[0].NpcChangeRegion(SubWorld[0].m_Region[m_RegionIndex].m_RegionID, SubWorld[0].m_Region[nRegionIdx].m_RegionID, m_Index);
			m_RegionIndex = nRegionIdx;
			m_dwRegionID = m_sSyncPos.m_dwRegionID;
			m_MapX = nMapX;
			m_MapY = nMapY;
			m_OffX = nOffX;
			m_OffY = nOffY;
		}
		else
		{
			SubWorld[0].m_Region[m_RegionIndex].DecNpcRef(m_MapX, m_MapY);
			m_MapX = nMapX;
			m_MapY = nMapY;
			m_OffX = nOffX;
			m_OffY = nOffY;
			//SubWorld[0].m_Region[m_RegionIndex].AddRef(m_MapX, m_MapY, obj_npc);
			SubWorld[0].m_Region[m_RegionIndex].AddNpcRef(m_MapX, m_MapY);
		}
	}
}

#endif
//客户端
#ifndef _SERVER
void KNpc::ProcNetCommand(NPCCMD cmd, int x /* = 0 */, int y /* = 0 */, int z /* = 0 */)
{
	switch (cmd)
	{
	case do_death:   
		{
		DoDeath();
		SetInstantSpr(enumINSTANT_STATE_REVIVE); //释放一个死亡的瞬间特效
		}
		break;
	case do_hurt:
		DoHurt(x, y, z);
		break;
	case do_revive:  //站着？
		{
		DoStand();
		m_ProcessAI    = 1;
		m_ProcessState = 1;
		//IsDeath=0;
		SetInstantSpr(enumINSTANT_STATE_REVIVE); //释放一个重生的瞬间特效	
		}
		break;
	case do_stand:
		{
		DoStand();
		m_ProcessAI = 1;
		m_ProcessState = 1;
		}
	    break;
	default:
		break;
	}
}
#endif
/*
#ifndef _SERVER
void	KNpc::ClearBlood(int i)
{
	m_nBloodNo[i]		= 0;
	m_nBloodAlpha[i]	= 0;
	m_nBloodTime[i]		= 0;
	m_szBloodNo[i][0]	= 0;
}
#endif


#ifndef _SERVER
void	KNpc::SetBlood(int nNo)
{
	if (nNo <= 0)
		return;
	for (int i = 0;i < defMAX_SHOW_BLOOD_NUM;i++)
	{
		if (!m_szBloodNo[i][0])
			break;
	}
	if (i == defMAX_SHOW_BLOOD_NUM) i = 0;
	m_nBloodNo[i]		= nNo;
	m_nBloodAlpha[i]	= 0;
	m_nBloodTime[i]		= defMAX_SHOW_BLOOD_TIME;
	sprintf(m_szBloodNo[i], "%d", nNo);
}
#endif

#ifndef _SERVER
int	KNpc::PaintBlood(int nHeightOffset)
{
	for (int i = 0;i < defMAX_SHOW_BLOOD_NUM;i++)
	{
		if (!m_szBloodNo[i][0])
			continue;
		
		int	nHeightOff = nHeightOffset + (defMAX_SHOW_BLOOD_TIME - m_nBloodTime[i]) * defSHOW_BLOOD_MOVE_SPEED;
		int nFontSize = 14;
		DWORD	dwColor = SHOW_BLOOD_COLOR | (m_nBloodAlpha[i] << 24);
		int		nMpsX, nMpsY,nMapID;
		GetMpsPos(&nMpsX,&nMpsY,&nMapID);
		g_pRepresent->OutputText(nFontSize, m_szBloodNo[i], KRF_ZERO_END, nMpsX - nFontSize * g_StrLen(m_szBloodNo[i]) / 4, nMpsY, dwColor, 0, nHeightOff);
		
		m_nBloodTime[i]--;
		if (m_nBloodTime[i] <= 0)
		{
			ClearBlood(i);
			continue;
		}
		m_nBloodAlpha[i]++;
		if (m_nBloodAlpha[i] > 31)
			m_nBloodAlpha[i] = 31;
	}
	
	return nHeightOffset;
}
#endif
*/

#ifndef _SERVER
void	KNpc::SetBlood(int nNo)
{	
		if (nNo <= 0)
			return;

		BOOL flag = FALSE;
		int i = 0;
		for (i; i< defMAX_SHOW_BLOOD_NUM;++i)
		{
			 if (m_nBloodNo[i][0] == 0) 
			{//空伤害值
				 flag = TRUE;
				 //m_nBloodNo[i][0] = nNo;                   //赋值给 绘制生命数值？
			     //m_nBloodNo[i][1] = defMAX_SHOW_BLOOD_TIME;//间隔时间 还是距离/?	
				 break;
			}
		}
		
		if (flag == TRUE)
		{//有空的就显示这个
			m_nBloodNo[i][0] = nNo;                   //赋值给 绘制生命数值？
			m_nBloodNo[i][1] = defMAX_SHOW_BLOOD_TIME;//间隔时间 还是距离/?	
		}
		else
		{//否则 就显示在第一个
			 m_nBloodNo[0][0] = nNo;
			 m_nBloodNo[0][1] = defMAX_SHOW_BLOOD_TIME;
		}
}
#endif 
//冒血数字显示
#ifndef _SERVER


int	KNpc::PaintBlood(int nHeightOffset)
{	
	 BOOL _flag = FALSE;

	 for (int j = 0; j< defMAX_SHOW_BLOOD_NUM; ++j)
	 {
		 if (m_nBloodNo[j][0] > 0)
		{
			_flag = TRUE;
			break;
		}
	 }
 
	 if (_flag == FALSE )
	 {
		return nHeightOffset;
	 }

	int nFontSize = 16;
	DWORD dwColor = SHOW_BLOOD_COLOR | (m_nBloodAlpha << 24); //颜色
	 int  nMpsX, nMpsY,nMmap;
	      GetMpsPos(&nMpsX, &nMpsY,&nMmap);
	int nHeightOff = nHeightOffset;

	for (int i = 0; i < defMAX_SHOW_BLOOD_NUM; ++i)
	{
		if (m_nBloodNo[i][0] > 0)
		{//血量大于0
			 nHeightOff = nHeightOffset + (defMAX_SHOW_BLOOD_TIME - m_nBloodNo[i][1]) * defSHOW_BLOOD_MOVE_SPEED;
			 sprintf(m_szBloodNo, "%d",m_nBloodNo[i][0]); //冒血数字显示
			 int n;
			 if (g_pRepresent->GetRepMoDel()==2)	 //2D m_ISrepresent2
			    n=1000;
			  else
				n=50;				 //3D

			 while(n>0)
			 {	
				 g_pRepresent->OutputText(nFontSize, m_szBloodNo, KRF_ZERO_END, nMpsX - nFontSize * g_StrLen(m_szBloodNo)/4, nMpsY, dwColor,0, nHeightOff,0XFF000000);
				 --n;
			 }
			 m_nBloodNo[i][1]--; //时间减1

			 if (m_nBloodNo[i][1] <=0)
			 {//清空
				 m_nBloodNo[i][0] = 0;
				 m_nBloodNo[i][1] = 0;
				 m_nBloodAlpha	= 0;
				 m_szBloodNo[0]	= 0;
			 }
		 }

		m_nBloodAlpha++;
		if (m_nBloodAlpha > 31)
		m_nBloodAlpha = 31;
	}

	return nHeightOff;
}

//要求同步NPC信息
void KNpc::ApplySynNpcInfo(DWORD nNpcDwid)
{
	NPC_SYN_INFO	sApplyInfo;
	sApplyInfo.ProtocolType = (BYTE)c2s_synnpcinfo;
	sApplyInfo.nNpcDwid=nNpcDwid;
	if (g_pClient)
		g_pClient->SendPackToServer(&sApplyInfo, sizeof(NPC_SYN_INFO));
}


int	KNpc::PaintNewBlood(int nHeightOffset,int Val)
{	
	int nFontSize = 12;
	//DWORD dwColor = SHOW_BLOOD_COLOR | (m_nBloodAlpha << 24); //颜色
	//DWORD dwColor = SHOW_BLOOD_COLOR;
     DWORD	dwColor=TGetColor("255,255,255");
	 int  nMpsX, nMpsY,nMmap;
	      GetMpsPos(&nMpsX, &nMpsY,&nMmap);
	 char nNewBlood[32];
	int nHeightOff = nHeightOffset;
			 sprintf(nNewBlood, "%d",Val); //冒血数字显示 
			 g_pRepresent->OutputText(nFontSize, nNewBlood, KRF_ZERO_END, nMpsX - nFontSize * g_StrLen(nNewBlood) / 4, nMpsY, dwColor, 0, nHeightOff,0XFF000000);
			 
	//m_nBloodAlpha++;
	//if (m_nBloodAlpha > 31)
	//	m_nBloodAlpha = 31;

	return nHeightOff;
}
#endif
//高度调整
#ifndef _SERVER
int	KNpc::GetNpcPate()
{
	int nHeight = m_Height + m_nStature;

	if (m_Kind == kind_player)
	{
		if (m_nSex)
			nHeight += 90;	//女
		else
			nHeight += 90;	//男

		if (m_MaskType == 0)
		{//没带面具的
			if (m_Doing == do_sit && MulDiv(10, m_Frames.nCurrentFrame, m_Frames.nTotalFrame) >= 8)
				nHeight -= MulDiv(30, m_Frames.nCurrentFrame, m_Frames.nTotalFrame);
			
			if (m_bRideHorse)
				nHeight += 20;	//骑马
		}
		else
		{//带面具
			nHeight += 20;
		}
		
	}
	
	return nHeight;
}
#endif
//生命条显示
#ifndef _SERVER
int	KNpc::GetNpcPatePeopleInfo()
{
	int nFontSize = 18;
	if (m_nChatContentLen > 0 && m_nChatNumLine > 0)
		return m_nChatNumLine * (nFontSize + 1);

	int nHeight = 0;
	if (NpcSet.CheckShowLife())
	{
		if (m_Kind == kind_player || m_Kind == kind_partner)
		{
			if (m_CurrentLifeMax > 0 &&\
				(relation_enemy == NpcSet.GetRelation(m_Index, Player[CLIENT_PLAYER_INDEX].m_nIndex))
				)
				nHeight += SHOW_LIFE_HEIGHT;
		}
	}
	if (NpcSet.CheckShowName())
	{
		if (nHeight != 0)
			nHeight += SHOW_SPACE_HEIGHT;//好看

		if (m_Kind == kind_player || m_Kind == kind_dialoger)
			nHeight += nFontSize + 1;
	}
	return nHeight;
}
#endif

void KNpc::GetFrameCopyFromTemplate(int nNpcTemplateId, int nLevel)
{
	if (nNpcTemplateId < 0 || nLevel < 1 ) 
		return ;
	KNpcTemplate* pNpcTemp = NULL;
	if (pNpcTemp = NpcSet.GetTemplate(nNpcTemplateId, nLevel))
	//if (g_pNpcTemplate[nNpcTemplateId][nLevel])        //数据有效则拷贝，否则重新生成
		LoadFrameFromTemplate(pNpcTemp);//(nNpcTemplateId, nLevel); //从模板获取数据
	/*else
	{
		if (!g_pNpcTemplate[nNpcTemplateId][0])
		{
			g_pNpcTemplate[nNpcTemplateId][0] = new KNpcTemplate;
			g_pNpcTemplate[nNpcTemplateId][0]->InitNpcBaseData(nNpcTemplateId);  //NPC基本
			g_pNpcTemplate[nNpcTemplateId][0]->m_NpcSettingIdx = nNpcTemplateId;
			g_pNpcTemplate[nNpcTemplateId][0]->m_bHaveLoadedFromTemplate = TRUE;
		}

		KLuaScript * pLevelScript = NULL;		
		
#ifdef _SERVER	 //g_pNpcTemplate[nNpcTemplateId][0]
		pLevelScript = (KLuaScript*)g_GetScript(
			g_pNpcTemplate[nNpcTemplateId][0]->m_dwLevelSettingScript
			);
		
		if (pLevelScript == NULL)
			pLevelScript = g_pNpcLevelScript;
#else
		KLuaScript LevelScript;
		if (!g_pNpcTemplate[nNpcTemplateId][0]->m_szLevelSettingScript[0])
			pLevelScript = g_pNpcLevelScript;
		else
		{
			LevelScript.Init();
			if (!LevelScript.Load(g_pNpcTemplate[nNpcTemplateId][0]->m_szLevelSettingScript))
			{
				//g_DebugLog ("[error]致命错误,无法正确读取[%s]", g_pNpcTemplate[nNpcTemplateId][0]->m_szLevelSettingScript);
				//_ASSERT(0);
				pLevelScript = g_pNpcLevelScript;
				LevelScript.Exit();
			}
			else
				pLevelScript = &LevelScript;
		}
		
#endif
		g_pNpcTemplate[nNpcTemplateId][nLevel] = new KNpcTemplate;
		*g_pNpcTemplate[nNpcTemplateId][nLevel] = *g_pNpcTemplate[nNpcTemplateId][0];
		g_pNpcTemplate[nNpcTemplateId][nLevel]->m_nLevel = nLevel;
		g_pNpcTemplate[nNpcTemplateId][nLevel]->InitNpcLevelData(nNpcTemplateId, pLevelScript, nLevel);
		g_pNpcTemplate[nNpcTemplateId][nLevel]->m_bHaveLoadedFromTemplate = TRUE;
		LoadFrameFromTemplate(nNpcTemplateId,nLevel);
	   }
	 */

}
//面具变形
void	KNpc::ReSetRes(int nMark)
{
	
#ifndef _SERVER
	char	szNpcTypeName[32];
#endif
	if (nMark == 1)
	{//恢复原来的样子
	  if (m_NpcSettingIdx == PLAYER_MALE_NPCTEMPLATEID || m_NpcSettingIdx == PLAYER_FEMALE_NPCTEMPLATEID)
	  { //如果是人物的话
#ifndef _SERVER
		if (m_NpcSettingIdx == PLAYER_MALE_NPCTEMPLATEID)
		{
			strcpy(szNpcTypeName, "男主角");
			/*strcpy(szNpcTypeName, "ani018");*/
			m_StandFrame = NpcSet.GetPlayerStandFrame(TRUE);
			m_WalkFrame  = NpcSet.GetPlayerWalkFrame(TRUE);
			m_RunFrame   = NpcSet.GetPlayerRunFrame(TRUE);
		}
		else
		{
			strcpy(szNpcTypeName, "女主角");
			m_StandFrame = NpcSet.GetPlayerStandFrame(FALSE);
			m_WalkFrame  = NpcSet.GetPlayerWalkFrame(FALSE);
			m_RunFrame   = NpcSet.GetPlayerRunFrame(FALSE);
		}
#endif
		m_WalkSpeed   = NpcSet.GetPlayerWalkSpeed();
		m_RunSpeed    = NpcSet.GetPlayerRunSpeed();
		m_AttackFrame = NpcSet.GetPlayerAttackFrame();
		m_HurtFrame	  = NpcSet.GetPlayerHurtFrame();

	  }
	  else
	  {//普通npc 怪物
			GetNpcCopyFromTemplate(m_NpcSettingIdx,m_Level);  //拷贝 NPCs.TXT 里面的基本信息！！			
#ifndef _SERVER	
			g_NpcSetting.GetString(m_NpcSettingIdx + 2, "NpcResType", "enemy003", szNpcTypeName, sizeof(szNpcTypeName));
			if (!szNpcTypeName[0])
			{ //人物类型表 npcres
				g_NpcKindFile.GetString(2, "人物名称", "enemy003", szNpcTypeName, sizeof(szNpcTypeName));//如果没找到，用第一个npc代替
			} 
#endif
	  }
	}
	else
	{//否则就变形
		GetFrameCopyFromTemplate(m_MaskType,m_Level);	//获取NPC的样子 数据 =m_MaskType
#ifndef _SERVER	
		g_NpcSetting.GetString(m_MaskType + 2, "NpcResType", "", szNpcTypeName, sizeof(szNpcTypeName));	// 面具
		
		if (!szNpcTypeName[0])
		{
			g_NpcKindFile.GetString(4, "人物名称", "enemy003", szNpcTypeName, sizeof(szNpcTypeName));//如果没找到，用第一个npc代替
		}
#endif
	}

#ifndef _SERVER
	m_DataRes.Remove(m_Index);               //清除残影
	m_DataRes.Init(szNpcTypeName, &g_NpcResList);
	m_DataRes.SetAction(m_ClientDoing);
	m_DataRes.SetRideHorse(m_bRideHorse);
	m_DataRes.SetArmor(m_ArmorType);
	m_DataRes.SetHelm(m_HelmType);
	m_DataRes.SetHorse(m_HorseType);
	m_DataRes.SetWeapon(m_WeaponType);
	//if (m_PifengType>0)
	m_DataRes.SetPifeng(m_PifengType);
	//if (m_ChiBangType>0)
	m_DataRes.SetChiBang(m_ChiBangType);
#endif

}
//---------------

/*
//	
void	KNpc::AutoDoSkill(int nSkillId, int nLevel, int nX, int nY)
{	
	if(m_AutoReplySkill.nValue[2] > 0)
	{	
		if(nSkillId == 20)
		{
			if(!m_SkillList.CanCast(nSkillId, SubWorld[m_SubWorldIndex].m_dwCurrentTime))
				return;
		}
	}

	if(m_AutoAttackSkill.nValue[2] > 0)
	{	
		if(nSkillId == 719)
		{
			if(!m_SkillList.CanCast(nSkillId - 8, SubWorld[m_SubWorldIndex].m_dwCurrentTime))
				return;
		}
		else if(nSkillId == 720)
		{
			if(!m_SkillList.CanCast(nSkillId - 6, SubWorld[m_SubWorldIndex].m_dwCurrentTime))
				return;
		}
	}

	if(m_AutoRescueSkill.nValue[0] > 0)
	{
		if(nSkillId == 721)
		{
			if(!m_SkillList.CanCast(nSkillId - 4, SubWorld[m_SubWorldIndex].m_dwCurrentTime))
				return;
		}
	}

	KSkill * pSkill = (KSkill *)g_SkillManager.GetSkill(nSkillId, nLevel);
	if(!pSkill)
		return;
	
	eSkillStyle eStyle = (eSkillStyle)pSkill->GetSkillStyle();

#ifdef _SERVER
	NPC_SKILL_SYNC	NetCommand;
	
	NetCommand.ProtocolType = s2c_skillcast;
	NetCommand.ID = m_dwID;
	NetCommand.nSkillID = nSkillId;
	NetCommand.nSkillLevel = nLevel;

	if (nY <= 0 ) 
	{
		return;
	}
	
	NetCommand.nMpsX = nX;
	if (nX == -1)
	{
		NetCommand.nMpsY = Npc[nY].m_dwID;
		if (0 == NetCommand.nMpsY || Npc[nY].m_SubWorldIndex != m_SubWorldIndex)
			return;
	}
	else
	{
		NetCommand.nMpsY = nY;
	}

	
	NetCommand.m_bAuto	= TRUE;
	

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
	CURREGION.BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, m_MapX, m_MapY);
	int i, nConRegion = -1;
	for (i = 0; i < 8; i++)
	{
		nConRegion = CONREGIONIDX(i);
		if (nConRegion < 0 || nConRegion >= SubWorld[m_SubWorldIndex].m_nTotalRegion)
			continue;
		CONREGION(i).BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, m_MapX - POff[i].x, m_MapY - POff[i].y);
	}
	NetCommand.ProtocolType = s2c_playerautodoskill;
	g_pServer->PackDataToClient(Player[m_nPlayerIdx].m_nNetConnectIdx, (NPC_SKILL_SYNC*)&NetCommand, sizeof(NetCommand));	

#endif
	if (eStyle == SKILL_SS_Missles 
		|| eStyle == SKILL_SS_Melee 
		|| eStyle == SKILL_SS_InitiativeNpcState 
		|| eStyle == SKILL_SS_PassivityNpcState)
	{
#ifndef _SERVER
		m_DataRes.StopSound();
		int x, y, tx, ty;
		SubWorld[m_SubWorldIndex].Map2Mps(m_RegionIndex, m_MapX, m_MapY, m_OffX, m_OffY, &x, &y);
		
		if (nY < 0)
			return;
		
		if (nX < 0)
		{
			if (nX != -1) 
				return;
			
			if (nY >= MAX_NPC || Npc[nY].m_dwID == 0 || Npc[nY].m_SubWorldIndex != m_SubWorldIndex)
				return;
			Npc[nY].GetMpsPos(&tx, &ty);
		}
		else
		{
			tx = nX;
			ty = nY;
		}
		m_Dir = g_GetDirIndex(x, y, tx, ty);
		if (pSkill->GetPreCastEffectFile()[0])
			m_DataRes.SetSpecialSpr((char *)pSkill->GetPreCastEffectFile());
		
		if (IsPlayer())
			pSkill->PlayPreCastSound(m_nSex,x, y);
		
		if (pSkill->IsNeedShadow())		
			m_DataRes.SetBlur(TRUE);
		else
			m_DataRes.SetBlur(FALSE);
#endif
		if (pSkill->GetSkillStyle() == SKILL_SS_Melee)
		{
			CastMeleeSkill(pSkill);
		}
		else
		{	
			if (m_AutoReplySkill.nValue[2] > 0)
			{	
				pSkill->Cast(m_Index, nX, nY);
			
				USHORT nDelay = 0;
				nDelay = m_AutoReplySkill.nValue[2] / 256;
				
				if(nSkillId == 20)
				{
					m_SkillList.SetNextCastTime(nSkillId, SubWorld[m_SubWorldIndex].m_dwCurrentTime + nDelay);
					m_SkillList.SetMaxCastTime(nSkillId, nDelay);
				}
				return;
			}

			if (m_AutoAttackSkill.nValue[2] > 0)
			{	
				pSkill->Cast(m_Index, nX, nY);
								
				USHORT nDelay = 0;
				nDelay = m_AutoAttackSkill.nValue[2] / 256;
				
				if(nSkillId == 719)
				{
					m_SkillList.SetNextCastTime(nSkillId - 8, SubWorld[m_SubWorldIndex].m_dwCurrentTime + nDelay);
					m_SkillList.SetMaxCastTime(nSkillId - 8, nDelay);
				}
				else if(nSkillId == 720)
				{
					m_SkillList.SetNextCastTime(nSkillId - 6, SubWorld[m_SubWorldIndex].m_dwCurrentTime + nDelay);
					m_SkillList.SetMaxCastTime(nSkillId - 6, nDelay);
				}
				return;
			}

			if (m_AutoRescueSkill.nValue[2] > 0)
			{	
				pSkill->Cast(m_Index, nX, nY);
				
				WORD wDelay = 0;
				wDelay = m_AutoRescueSkill.nValue[2] / 256;
				
				if(nSkillId == 721)
				{
					m_SkillList.SetNextCastTime(nSkillId - 4, SubWorld[m_SubWorldIndex].m_dwCurrentTime + wDelay);
					m_SkillList.SetMaxCastTime(nSkillId - 4, wDelay);
				} 
				return;
			}
			pSkill->Cast(m_Index, nX, nY);
		}
	}
}
*/

void	KNpc::LoadFrameFromTemplate(VOID* nNpcTemp)//(int nNpcTemplateId, int nLevel)
{
	/*if (nNpcTemplateId < 0 )
	{
		return ;
	}
	
	KNpcTemplate* pNpcTemp = NULL;
	if (pNpcTemp = NpcSet.GetTemplate(nNpcTemplateId, nLevel))
   */
	//KNpcTemplate * pNpcTemp = g_pNpcTemplate[nNpcTemplateId][nLevel];	  //NpcSet.GetTemplate(nNpcTemplateId, nLevel);
	KNpcTemplate* pNpcTemp =NULL;
	              pNpcTemp  =  (KNpcTemplate *)nNpcTemp;

	if (!pNpcTemp)
		return ;

	m_HeadImage        =	pNpcTemp->m_HeadImage;
	m_CorpseSettingIdx =	pNpcTemp->m_CorpseSettingIdx;
	m_DeathFrame       =	pNpcTemp->m_DeathFrame;
	m_WalkFrame        =	pNpcTemp->m_WalkFrame;
	m_RunFrame         =	pNpcTemp->m_RunFrame;
	m_HurtFrame        =	pNpcTemp->m_HurtFrame;
	//m_AttackFrame      =	pNpcTemp->m_AttackFrame;
	//m_CastFrame        =	pNpcTemp->m_CastFrame;
	m_StandFrame       =    pNpcTemp->m_StandFrame;
	m_StandFrame1      =    pNpcTemp->m_StandFrame1;
#ifndef _SERVER
	m_ArmorType				= pNpcTemp->m_ArmorType;
	m_HelmType				= pNpcTemp->m_HelmType;
	m_WeaponType			= pNpcTemp->m_WeaponType;
 // m_PifengType		    = pNpcTemp->m_PifengType; //披风
//	m_HorseType				= pNpcTemp->m_HorseType;
//	m_bRideHorse			= pNpcTemp->m_bRideHorse;
#endif

	
}
//移动Npc 
BOOL KNpc::DoSecMove(int nDistance)
{
	WaitForFrame();
	if (nDistance <= 0)
		return FALSE;

#ifndef _SERVER
	if (m_RegionIndex < 0 || m_RegionIndex >= 9)
	{
		//g_DebugLog("[zroc]Npc(%d)ServerMove RegionIdx = %d", m_Index, m_RegionIndex);
		_ASSERT(0);
		DoStand();
		return FALSE;
	}
#else
	//_ASSERT(m_RegionIndex >= 0);
	if  (m_SubWorldIndex<0 || m_SubWorldIndex>=MAX_SUBWORLD) return false;

	if (m_RegionIndex < 0)
		return FALSE;

	if  ( m_RegionIndex >=SubWorld[m_SubWorldIndex].m_nTotalRegion)
		return FALSE;
#endif
	int x, y;

	SubWorld[m_SubWorldIndex].NewMap2Mps(m_RegionIndex, m_MapX, m_MapY, m_OffX, m_OffY, &x, &y);
	int nDis = SubWorld[m_SubWorldIndex].GetDistance(x,y,m_DesX,m_DesY);

	if (nDis > nDistance)
	{
		if (m_JumpDir >= MaxMissleDir)
			m_JumpDir -= MaxMissleDir;

		x = m_DesX - (g_DirCos(m_JumpDir, MaxMissleDir) * (nDis - nDistance) >> 10);
		y = m_DesY - (g_DirSin(m_JumpDir, MaxMissleDir) * (nDis - nDistance) >> 10);
	}
	else
	{
		x = m_DesX;
		y = m_DesY;
	}

	int nOldRegion = m_RegionIndex;
	int nOldMapX = m_MapX;
	int nOldMapY = m_MapY;
	int nOldOffX = m_OffX;
	int nOldOffY = m_OffY;
	
	//	处理NPC的坐标变幻
	//	CELLWIDTH、CELLHEIGHT、OffX、OffY均是放大了1024倍
	
	if (!m_bClientOnly)
	 // CURREGION.DecNpcRef(m_MapX, m_MapY);
      CURREGION.DecRef(m_MapX, m_MapY,obj_npc);

	SubWorld[m_SubWorldIndex].Mps2Map(x,y,&m_RegionIndex,&m_MapX,&m_MapY,&m_OffX,&m_OffY);

	if (!m_bClientOnly && m_RegionIndex >= 0)
		//CURREGION.AddNpcRef(m_MapX,m_MapY);
		CURREGION.AddRef(m_MapX, m_MapY, obj_npc);
	
	
	if (m_RegionIndex == -1)	// 不可能移动到-1 Region，如果出现这种情况，恢复原坐标
	{
		m_RegionIndex = nOldRegion;
		m_MapX = nOldMapX;
		m_MapY = nOldMapY;
		m_OffX = nOldOffX;
		m_OffY = nOldOffY;
		CURREGION.AddRef(m_MapX, m_MapY, obj_npc);
        //CURREGION.AddNpcRef(m_MapX,m_MapY);
		return FALSE;
	}
	
	if (nOldRegion != m_RegionIndex)
	{
#ifdef _SERVER
		SubWorld[m_SubWorldIndex].NpcChangeRegion(nOldRegion, m_RegionIndex, m_Index);
		if (IsPlayer())
		{
			SubWorld[m_SubWorldIndex].PlayerChangeRegion(nOldRegion, m_RegionIndex, m_nPlayerIdx);
		}
#else
		SubWorld[0].NpcChangeRegion(SubWorld[0].m_Region[nOldRegion].m_RegionID, SubWorld[0].m_Region[m_RegionIndex].m_RegionID, m_Index);
		m_dwRegionID = SubWorld[0].m_Region[m_RegionIndex].m_RegionID;
#endif
	}

#ifdef _SERVER	// 向周围9个Region广播发技能
	NPC_JUMP_SYNC	NetCommand;
	NetCommand.ProtocolType = (BYTE)s2c_npcsecmove;
	NetCommand.ID = m_dwID;
	NetCommand.nMpsX = m_DesX;
	NetCommand.nMpsY = m_DesY;
	
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
	CURREGION.BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, m_MapX, m_MapY);
	int i,nConRegion=-1;
	for (i = 0; i < 8;++i)
	{  
		nConRegion = CONREGIONIDX(i);
		if  (nConRegion<0 || nConRegion>=SubWorld[m_SubWorldIndex].m_nTotalRegion)
			continue;

		CONREGION(i).BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, m_MapX - POff[i].x, m_MapY - POff[i].y);
	}
#endif

	return FALSE;
}

//============================
void KNpc::SetRankFF(int nRankFF)
{	
	m_btRankFFId = nRankFF;
	if (m_SubWorldIndex<0 || m_SubWorldIndex>=MAX_SUBWORLD ||m_RegionIndex<0) 
		return;
	if  ( m_RegionIndex >=SubWorld[m_SubWorldIndex].m_nTotalRegion)
		return ;
#ifdef _SERVER
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
	RANKFF_SYNC	NetCommand;
	NetCommand.ProtocolType = BYTE(s2c_rankname);
	NetCommand.ID = m_dwID;
	NetCommand.RankFF = m_btRankFFId;
	if (m_RegionIndex < 0)
		return;
	int nMaxCount = MAX_BROADCAST_COUNT;
	CURREGION.BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, m_MapX, m_MapY);
	int i,nConRegion=-1;
	for (i = 0; i < 8;++i)
	{  
		nConRegion = CONREGIONIDX(i);
		if  (nConRegion<0 || nConRegion>=SubWorld[m_SubWorldIndex].m_nTotalRegion)
			continue;
		CONREGION(i).BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, m_MapX - POff[i].x, m_MapY - POff[i].y);
	}

#endif

}

#ifndef _SERVER
//spr状态称号==========
void KNpc::PaintHonor(int nbtRankFFId,int nMpsX,int nMpsY,int i)
{
	KTabFile nBank;
   nBank.Load("\\Settings\\HonorSetting.txt");
   int nRows=nBank.GetHeight(); //行数
   int nLies=nBank.GetWidth();  //列数
	char szImageName[128], nLuaField[64];
	int n_X, n_Y,mInterval=100;
	ZeroMemory(szImageName,strlen(szImageName));
    ZeroMemory(nLuaField,strlen(nLuaField));
     if (nbtRankFFId<=nRows)
	 {
          nBank.GetString(nbtRankFFId+1,"HONORLINK","",nLuaField,sizeof(nLuaField)); 
		  sprintf(szImageName,"\\spr\\skill\\others\\%s",nLuaField);
		  nBank.GetInteger(nbtRankFFId+1,"HONORX",10,&n_X);
		  nBank.GetInteger(nbtRankFFId+1,"HONORY",10,&n_Y);
		  nBank.GetInteger(nbtRankFFId+1,"nInterval",100,&mInterval);  
	 }
	 else
	 {//宠物
		 sprintf(szImageName,"\\spr\\skill\\others\\助攻王.spr");

	 }
	 this->SetFrameSpr(szImageName, n_X, n_Y+6,GetNpcPate(),mInterval);
	 //SetClientSpr(szImageName,nMpsX+n_X,nMpsY+n_Y,0,i);
	 ZeroMemory(szImageName,strlen(szImageName));
     ZeroMemory(nLuaField,strlen(nLuaField));
     nBank.Clear();
}
//披风=================
void KNpc::PaintPifeng(int m_PifengType,int nMpsX,int nMpsY,int i)
{	

	//m_PiFenLoop++;

	KIniFile nBank;
	nBank.Load("\\Ui\\npcbobo.ini");
	int nCount=0,nTime=20;
	char szImageName[128]={0},nDir[16]={0};
	      nBank.GetInteger("FortuneRank","count",11,&nCount);
		  nBank.GetInteger("FortuneRank","nTime",20,&nTime);
		 if (m_PifengType>nCount)
				 m_PifengType=nCount;
		 /*if (m_PiFenLoop%18*nTime==0)
		 {
            m_PifengType=GetRandomNumber(0,nCount);
			if  (m_PiFenLoop>50000)
			     m_PiFenLoop=0;
		 }*/
        snprintf(nDir, sizeof(nDir),"Spr_%d",m_PifengType);
		nBank.GetString("FortuneRank",nDir,"",szImageName,sizeof(szImageName)); 		
	    SetClientSpr(szImageName,nMpsX,nMpsY,0,i); //szImageName
	    nBank.Clear();
}

#endif

//#ifdef _SERVER
//经脉初始化(死亡是初始化)
int KNpc::NpcNewInitJinMaiVal()
{

    int nPlayeridx=0;

    #ifdef _SERVER
        nPlayeridx=m_nPlayerIdx;
	#else
        nPlayeridx=CLIENT_PLAYER_INDEX;
	#endif

    KTabFile nMagFile;
	  if(!nMagFile.Load(TABFILE_MAGICAGOLD_PATH)) //"\\Settings\\item\\004\\GoldMagic.txt"
	  {
	     #ifdef _SERVER
				  
                  Player[nPlayeridx].m_ItemList.msgshow("加载文件失败S!");
	     #else
				  Player[nPlayeridx].m_ItemList.ClientShowMsg("加载文件失败C!");
		 #endif

		return 0;
	  }
	//int nLeftTime = Player[nPlayeridx].m_cTask.GetSaveVal(TASKVALUE_LIXIAN); //离线时间


	BYTE nDuMaiStaus=EGetByte(Player[nPlayeridx].m_cTask.GetSaveVal(3501),1); //筋脉大类 督脉 0 表示关闭中 1 表示进行中 2 表示完成了
	BYTE nDuMaiRow=EGetByte(Player[nPlayeridx].m_cTask.GetSaveVal(3501),2); //开始的行数
	//BYTE nDuMaiCurXue=Player[nPlayeridx].m_cTask.GetTaskByte(Player[nPlayeridx].m_cTask.GetSaveVal(3501),3); //当前穴道
		 //Player[nPlayeridx].m_cTask.GetTaskByte(Player[nPlayeridx].m_cTask.GetSaveVal(3501),4);  
	BYTE nRenMaiStaus=EGetByte(Player[nPlayeridx].m_cTask.GetSaveVal(3502),1);
	BYTE nRenMaiRow=EGetByte(Player[nPlayeridx].m_cTask.GetSaveVal(3502),2);
	//BYTE nRenMaiCurXue=Player[nPlayeridx].m_cTask.GetTaskByte(Player[nPlayeridx].m_cTask.GetSaveVal(3502),3);

	BYTE nChoMaiStaus=EGetByte(Player[nPlayeridx].m_cTask.GetSaveVal(3503),1);
	BYTE nChoMaiRow=EGetByte(Player[nPlayeridx].m_cTask.GetSaveVal(3503),2);
	//BYTE nChoMaiCurXue=Player[nPlayeridx].m_cTask.GetTaskByte(Player[nPlayeridx].m_cTask.GetSaveVal(3503),3);

	BYTE nDaiMaiStaus=EGetByte(Player[nPlayeridx].m_cTask.GetSaveVal(3504),1);
	BYTE nDaiMaiRow=EGetByte(Player[nPlayeridx].m_cTask.GetSaveVal(3504),2);
	//BYTE nDaiMaiCurXue=Player[nPlayeridx].m_cTask.GetTaskByte(Player[nPlayeridx].m_cTask.GetSaveVal(3504),3);

	BYTE nYinWMaiStaus=EGetByte(Player[nPlayeridx].m_cTask.GetSaveVal(3505),1);
	BYTE nYinWMaiRow=EGetByte(Player[nPlayeridx].m_cTask.GetSaveVal(3505),2);
	//BYTE nYinWMaiCurXue=Player[nPlayeridx].m_cTask.GetTaskByte(Player[nPlayeridx].m_cTask.GetSaveVal(3505),3);

	BYTE nYanWMaiStaus=EGetByte(Player[nPlayeridx].m_cTask.GetSaveVal(3506),1);
	BYTE nYanWMaiRow=EGetByte(Player[nPlayeridx].m_cTask.GetSaveVal(3506),2);
	//BYTE nYanWMaiCurXue=Player[nPlayeridx].m_cTask.GetTaskByte(Player[nPlayeridx].m_cTask.GetSaveVal(3506),3);

	BYTE nYinQMaiStaus=EGetByte(Player[nPlayeridx].m_cTask.GetSaveVal(3507),1);
	BYTE nYinQMaiRow=EGetByte(Player[nPlayeridx].m_cTask.GetSaveVal(3507),2);
	//BYTE nYinQMaiCurXue=Player[nPlayeridx].m_cTask.GetTaskByte(Player[nPlayeridx].m_cTask.GetSaveVal(3507),3);
	//BYTE nYinQMaiStaus=Player[nPlayeridx].m_cTask.GetTaskByte(Player[nPlayeridx].m_cTask.GetSaveVal(3507),4);

	BYTE nYanQMaiStaus=EGetByte(Player[nPlayeridx].m_cTask.GetSaveVal(3508),1);
	BYTE nYanQMaiRow=EGetByte(Player[nPlayeridx].m_cTask.GetSaveVal(3508),2);
	//BYTE nYanQMaiCurXue=Player[nPlayeridx].m_cTask.GetTaskByte(Player[nPlayeridx].m_cTask.GetSaveVal(3508),3);


	int nNpcIdx = m_Index;
    int nAllStaus=0;
	if (nNpcIdx>0)
	{	
		if (nDuMaiStaus==2&&  //丹田完成
			nRenMaiStaus==2&&    
			nChoMaiStaus==2&&
			nDaiMaiStaus==2&&
			nYinWMaiStaus==2&&
			nYanWMaiStaus==2&&
			nYinQMaiStaus==2&&
			nYanQMaiStaus==2
			)
		{//丹田全部经脉大类完成处理  激活全抗 和 技能加成
            /*  int nMacLevel[2];
			g_FsJinMai.GetInt2(114,6,nMacLevel);
			UseSkills(nMacLevel[0],nMacLevel[1],nNpcIdx);*/ //设置技能光环效果
			nAllStaus=1;
		}
      //以下是处理各个经脉的分脉状态
	 KMagicAttrib m_CurrentJinMai;	    // Npc的当前经脉数据
	 ZeroStruct(m_CurrentJinMai);

     for (int i=1;i<9;++i)
	 {
		//char mMagcid[16];
		//int mMgLevel=0,nIfMagic=1,nTime=-1;
		if (nAllStaus==1)
		{//大类全部完成处理 分支状态
		    int nRows=g_FsJinMai.GetHeight();
            for (int k=1;k<nRows;++k)  //13个分脉（行数）
			{   
			    /*if (k>nDuMaiCurXue)
				    break;*/
                int nMacidx;
                
				g_FsJinMai.GetInteger(k+1,7,0,&nMacidx);
                int mAttribType,mValue;
				nMagFile.GetInteger(nMacidx+1,5,0,&mAttribType) ;
				nMagFile.GetInteger(nMacidx+1,6,0,&mValue) ;
				//UseSkills(nMacLevel[0],nMacLevel[1],nNpcIdx); //设置技能光环效果
				
			  //const  KItemNormalAttrib* pAttrib;
		      //pAttrib = &(m_aryMagicAttrib[i]);
              /*
			    	KMagicAttrib* pTemp = NULL;
		            pTemp = (KMagicAttrib *)pData;  //最新等级数据
			  */
				m_CurrentJinMai.nAttribType=mAttribType;
                m_CurrentJinMai.nValue[0]=mValue;
                m_CurrentJinMai.nValue[1]=-1;
				m_CurrentJinMai.nValue[2]=0;
                // pAttrib=&(m_CurrentJinMai);
		       if (-1 != m_CurrentJinMai.nAttribType)
			   {//将属性应用再NPC身上
					ModifyAttrib(nNpcIdx, &m_CurrentJinMai);
			   }	
			} 
			//UseSkills(533,1,nNpcIdx); //设置技能光环效果
		}
		else 	
		{//部分完成
		 //int nMacLevel[2];//nMacLevel[0]=技能ID,nMacLevel[1]=技能等级
			if (i==1 && nDuMaiStaus>0) //督脉状态为进行中
			{
			   
			  for (int k=1;k<17;++k)  //13个分脉（行数）
			  {   
			    if (k>nDuMaiRow)
				    break;

                int nMacidx;

				g_FsJinMai.GetInteger(k+1,7,0,&nMacidx);
                int mAttribType=0,mValue=0;
				nMagFile.GetInteger(nMacidx+1,"属性调整类别",0,&mAttribType) ;
				nMagFile.GetInteger(nMacidx+1,"参数1最小值",0,&mValue) ;
				//UseSkills(nMacLevel[0],nMacLevel[1],nNpcIdx); //设置技能光环效果
				
                //char msg[64];

			    /*KItemNormalAttrib* pAttrib=NULL;
		        //pAttrib = &(m_aryMagicAttrib[i]);
				pAttrib->nAttribType=mAttribType;
                pAttrib->nValue[0]=mValue;
                pAttrib->nValue[1]=-1;
				pAttrib->nValue[2]=0;

		        if (-1 != pAttrib->nAttribType)
	            {//将属性应用再NPC身上
					ModifyAttrib(nNpcIdx, (void *)pAttrib);
	            }*/
				m_CurrentJinMai.nAttribType=mAttribType;
                m_CurrentJinMai.nValue[0]=mValue;
                m_CurrentJinMai.nValue[1]=-1;
				m_CurrentJinMai.nValue[2]=0;

                // pAttrib=&(m_CurrentJinMai);

		        if (m_CurrentJinMai.nAttribType)
				{//将属性应用再NPC身上
					ModifyAttrib(nNpcIdx, &m_CurrentJinMai);
				}
                /*#ifdef _SERVER
				  sprintf(msg,"督脉(%d)状态S:%d,穴道:%d,类型:%d,值:%d||%d",nMacidx,nDuMaiStaus,nDuMaiRow,mAttribType,mValue,m_CurrentJinMai.nValue[0]);
                  Player[nPlayeridx].m_ItemList.msgshow(msg);
	            #else
				  sprintf(msg,"督脉(%d)状态C:%d,穴道:%d,类型:%d,值:%d||%d",nMacidx,nDuMaiStaus,nDuMaiRow,mAttribType,mValue,m_CurrentJinMai.nValue[0]);
				  Player[nPlayeridx].m_ItemList.ClientShowMsg(msg);
				#endif*/
					
			  } 
			    
			//	UseSkills(533,1,nNpcIdx); //设置技能光环效果
			}
			else if (i==2 && nRenMaiStaus>0) //任脉分脉	
			{
			  for (int k=1;k<17;++k)  //13个分脉（行数）
			  {   
			    if (k>nRenMaiRow)
				    break;

                int nMacidx;

				g_FsJinMai.GetInteger(k+17,7,0,&nMacidx);
                int mAttribType,mValue;
				nMagFile.GetInteger(nMacidx+1,5,0,&mAttribType) ;
				nMagFile.GetInteger(nMacidx+1,6,0,&mValue) ;
				//UseSkills(nMacLevel[0],nMacLevel[1],nNpcIdx); //设置技能光环效果
				
			    m_CurrentJinMai.nAttribType=mAttribType;
                m_CurrentJinMai.nValue[0]=mValue;
                m_CurrentJinMai.nValue[1]=-1;
				m_CurrentJinMai.nValue[2]=0;

                // pAttrib=&(m_CurrentJinMai);

		        if (m_CurrentJinMai.nAttribType)
				{//将属性应用再NPC身上
					ModifyAttrib(nNpcIdx, &m_CurrentJinMai);
				}
			  	
			  } 			
				//UseSkills(533,1,nNpcIdx); //设置技能光环效果
			}
			else if (i==3 && nChoMaiStaus>0) //冲脉分脉
			{
			  for (int k=1;k<17;++k)  //13个分脉（行数）
			  {   
			    if (k>nChoMaiRow)
				    break;

                int nMacidx;

				g_FsJinMai.GetInteger(k+33,7,0,&nMacidx);
                int mAttribType,mValue;
				nMagFile.GetInteger(nMacidx+1,5,0,&mAttribType) ;
				nMagFile.GetInteger(nMacidx+1,6,0,&mValue) ;
				//UseSkills(nMacLevel[0],nMacLevel[1],nNpcIdx); //设置技能光环效果
				
			    m_CurrentJinMai.nAttribType=mAttribType;
                m_CurrentJinMai.nValue[0]=mValue;
                m_CurrentJinMai.nValue[1]=-1;
				m_CurrentJinMai.nValue[2]=0;

                // pAttrib=&(m_CurrentJinMai);

		        if (m_CurrentJinMai.nAttribType)
				{//将属性应用再NPC身上
					ModifyAttrib(nNpcIdx, &m_CurrentJinMai);
				}	
			  } 							
				//UseSkills(533,1,nNpcIdx); //设置技能光环效果
			}	
 	
			else if (i==4 && nDaiMaiStaus>0) //带脉分脉	
			{
			  for (int k=1;k<17;++k)  //13个分脉（行数）
			  {   
			    if (k>nDaiMaiRow)
				    break;

                int nMacidx;

				g_FsJinMai.GetInteger(k+49,7,0,&nMacidx);
                int mAttribType,mValue;
				nMagFile.GetInteger(nMacidx+1,5,0,&mAttribType) ;
				nMagFile.GetInteger(nMacidx+1,6,0,&mValue) ;
				//UseSkills(nMacLevel[0],nMacLevel[1],nNpcIdx); //设置技能光环效果
				
			    m_CurrentJinMai.nAttribType=mAttribType;
                m_CurrentJinMai.nValue[0]=mValue;
                m_CurrentJinMai.nValue[1]=-1;
				m_CurrentJinMai.nValue[2]=0;

                // pAttrib=&(m_CurrentJinMai);

		        if (m_CurrentJinMai.nAttribType)
				{//将属性应用再NPC身上
					ModifyAttrib(nNpcIdx, &m_CurrentJinMai);
				}	
			  } 											
				//UseSkills(533,1,nNpcIdx); //设置技能光环效果
			}
			else if (i==5 && nYinWMaiStaus>0) //阴维分脉	
			{
			  for (int k=1;k<17;++k)  //13个分脉（行数）
			  {   
			    if (k>nYinWMaiRow)
				    break;

                int nMacidx;

				g_FsJinMai.GetInteger(k+65,7,0,&nMacidx);
                int mAttribType,mValue;
				nMagFile.GetInteger(nMacidx+1,5,0,&mAttribType) ;
				nMagFile.GetInteger(nMacidx+1,6,0,&mValue) ;
				//UseSkills(nMacLevel[0],nMacLevel[1],nNpcIdx); //设置技能光环效果
				
			    m_CurrentJinMai.nAttribType=mAttribType;
                m_CurrentJinMai.nValue[0]=mValue;
                m_CurrentJinMai.nValue[1]=-1;
				m_CurrentJinMai.nValue[2]=0;

                // pAttrib=&(m_CurrentJinMai);

		        if (m_CurrentJinMai.nAttribType)
				{ //将属性应用再NPC身上
					ModifyAttrib(nNpcIdx, &m_CurrentJinMai);
				}	 
			  } 
			  //UseSkills(533,1,nNpcIdx); //设置技能光环效果												
			}
			else if (i==6 && nYanWMaiStaus>0) //阳维分脉	
			{
			  for (int k=1;k<17;++k)  //13个分脉（行数）
			  {   
			    if (k>nYanWMaiRow)
				    break;

                int nMacidx;

				g_FsJinMai.GetInteger(k+81,7,0,&nMacidx);
                int mAttribType,mValue;
				nMagFile.GetInteger(nMacidx+1,5,0,&mAttribType) ;
				nMagFile.GetInteger(nMacidx+1,6,0,&mValue) ;
				//UseSkills(nMacLevel[0],nMacLevel[1],nNpcIdx); //设置技能光环效果
				
			    m_CurrentJinMai.nAttribType=mAttribType;
                m_CurrentJinMai.nValue[0]=mValue;
                m_CurrentJinMai.nValue[1]=-1;
				m_CurrentJinMai.nValue[2]=0;

                // pAttrib=&(m_CurrentJinMai);

		        if (m_CurrentJinMai.nAttribType)
				{ //将属性应用再NPC身上
					ModifyAttrib(nNpcIdx, &m_CurrentJinMai);
				}	 
			  } 										
				//UseSkills(533,1,nNpcIdx); //设置技能光环效果
			} 
			else if (i==7 && nYinQMaiStaus>0) //阴跷分脉	
			{
			  for (int k=1;k<17;++k)  //13个分脉（行数）
			  {   
			    if (k>nYinQMaiRow)
				    break;

                int nMacidx;

				g_FsJinMai.GetInteger(k+97,7,0,&nMacidx);
                int mAttribType,mValue;
				nMagFile.GetInteger(nMacidx+1,5,0,&mAttribType) ;
				nMagFile.GetInteger(nMacidx+1,6,0,&mValue) ;
				//UseSkills(nMacLevel[0],nMacLevel[1],nNpcIdx); //设置技能光环效果
				
			   m_CurrentJinMai.nAttribType=mAttribType;
                m_CurrentJinMai.nValue[0]=mValue;
                m_CurrentJinMai.nValue[1]=-1;
				m_CurrentJinMai.nValue[2]=0;

                // pAttrib=&(m_CurrentJinMai);

		        if (m_CurrentJinMai.nAttribType)
				{ //将属性应用再NPC身上
					ModifyAttrib(nNpcIdx, &m_CurrentJinMai);
				}	 
			  } 
			  //UseSkills(533,1,nNpcIdx); //设置技能光环效果												
			} 
			else if (i==8 && nYanQMaiStaus>0) //阳跷分脉	
			{
			  for (int k=1;k<17;++k)  //13个分脉（行数）
			  {   
			    if (k>nYanQMaiRow)
				    break;

                int nMacidx;

				g_FsJinMai.GetInteger(k+113,7,0,&nMacidx);
                int mAttribType,mValue;
				nMagFile.GetInteger(nMacidx+1,5,0,&mAttribType) ;
				nMagFile.GetInteger(nMacidx+1,6,0,&mValue) ;
				//UseSkills(nMacLevel[0],nMacLevel[1],nNpcIdx); //设置技能光环效果
				
			    m_CurrentJinMai.nAttribType=mAttribType;
                m_CurrentJinMai.nValue[0]=mValue;
                m_CurrentJinMai.nValue[1]=-1;
				m_CurrentJinMai.nValue[2]=0;

                // pAttrib=&(m_CurrentJinMai);

		        if (m_CurrentJinMai.nAttribType)
				{ //将属性应用再NPC身上
					ModifyAttrib(nNpcIdx, &m_CurrentJinMai);
				}	 
			  } 										
				//UseSkills(533,1,nNpcIdx); //设置技能光环效果
			} 				
		}
	 }
	} 
	else
	{
	  nMagFile.Clear();
      return 0;
	}

	nMagFile.Clear();
    return 1;
}

void KNpc::UseSkills(int sKillID,int sLevel,int nNpcIdx,KMagicAttrib *pData,BOOL nIfMagic,int nTime)
{										   
	      KSkill *pSkill = (KSkill*)g_SkillManager.GetSkill(sKillID,sLevel);
		  if (nIfMagic)
		  {	   if (pSkill)
			      pSkill->CastStateSkill(nNpcIdx,0,0,nTime);  //是否
		  }
		  else
		     Npc[nNpcIdx].SetStateSkillEffect(nNpcIdx,sKillID,sLevel,pData,0,nTime);  //设置技能光环效果	
}

//#endif


//重置满内里对攻击技能的加成
void KNpc::ReFullManaSkillEnhance(int nLeftSkill,int nLeftListidx)
{
	int nActiveSkillID,nCurSkillEnchance=0;
	
	nActiveSkillID = nLeftSkill;

#ifdef _SERVER
	if (m_CurrentMana>=m_CurrentManaMax) //满内力对攻击的加成
	    nCurSkillEnchance = m_Currentskillenhance+m_CurrentFullManaskillenhance;//随状态的增减
	else
        nCurSkillEnchance = m_Currentskillenhance;
#else
//客户端需要换装才能修复，刚登陆时造成的加成数据误差
	if (IsPlayer())
	{
     if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentManaMax <= Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentMana)
		nCurSkillEnchance = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Currentskillenhance+Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentFullManaskillenhance;
	 else
        nCurSkillEnchance = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Currentskillenhance;
	}
#endif
	
	if (nCurSkillEnchance!= 0)
	{
		int nListIndex = 0;
		if  (nLeftListidx>0)
			nListIndex  = nLeftListidx;
		else
		 /* m_nLeftListidx =  */nListIndex = m_SkillList.FindSame(nActiveSkillID);

		if (nListIndex)                 //查找激活的当前攻击技能ID
		{
			int nEnchance = m_SkillList.GetTempEnChance(nListIndex);  //获取原始的技能加成 

			if (m_SkillList.GetCurrentLevelByIdx(nListIndex) > 0)  //等级大于0的才 增加
			{
				//printf("----左键技能:%d 原始加成:%d -----\n",nListIndex,nEnchance);
			    m_SkillList.SetEnChance(nListIndex,nEnchance+nCurSkillEnchance);
				//printf("----左键技能: 总加成:%d，另外:%d -----\n",nEnchance+nCurSkillEnchance,nCurSkillEnchance);
			}			
		}
	}

}
//重置攻击技能的加成
void KNpc::ReSkillEnhance(int nLeftSkill,int nIsAdd,int nLeftListidx)
{
	int nActiveSkillID,nCurSkillEnchance=0;
	
	nActiveSkillID = nLeftSkill;

#ifdef _SERVER
	if (m_CurrentMana>=m_CurrentManaMax) //满内力对攻击的加成
	    nCurSkillEnchance = m_Currentskillenhance+m_CurrentFullManaskillenhance;//随状态的增减
	else
        nCurSkillEnchance = m_Currentskillenhance;
#else
//客户端需要换装才能修复，刚登陆时造成的加成数据误差
	if (IsPlayer())
	{
     if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentManaMax <= Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentMana)
		nCurSkillEnchance = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Currentskillenhance+Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentFullManaskillenhance;
	 else
        nCurSkillEnchance = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Currentskillenhance;
	}
#endif
	
	if (nCurSkillEnchance!= 0)
	{
		int nListIndex = 0;
		if  (nLeftListidx>0)
			nListIndex  = nLeftListidx;
		else
		    nListIndex = m_SkillList.FindSame(nActiveSkillID);

		if (nListIndex)                 //查找激活的当前攻击技能ID
		{
			if (m_SkillList.GetCurrentLevelByIdx(nListIndex) > 0)  //等级大于0的才 增加
			{
				if (nIsAdd==0)
				    m_SkillList.AddEnChance(nListIndex,-nCurSkillEnchance);
				else if (nIsAdd > 0)
					m_SkillList.AddEnChance(nListIndex,nCurSkillEnchance);
			}			
		}
	}
/*
#ifdef _SERVER
	char msg[64];
	sprintf(msg,"S攻击加成:%d",nCurSkillEnchance);
	Player[m_nsPlayerIdx].m_ItemList.msgshow(msg);
#else
	char msg[64];
	sprintf(msg,"C攻击加成:%d",nCurSkillEnchance);
	Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(msg);	
#endif
*/
}

//重置某技能的加成
void KNpc::ReWhereSkillEnhance(int nActiveSkillID,int nCurSkillEnchance,int nIsAdd)
{

/*
#ifdef _SERVER
				char Msg[64];
				sprintf(Msg,"S:对某技能(%d)(%d)加成成功",nActiveSkillID,nCurSkillEnchance);
				Player[GetPlayerIdx()].m_ItemList.msgshow(Msg);	
#else
				char Msg[64];
				sprintf(Msg,"C:对某技能(%d)(%d)加成成功",nActiveSkillID,nCurSkillEnchance);
				Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(Msg);
#endif;
*/				
	if (nCurSkillEnchance!= 0)
	{   
		int nListIndex = m_SkillList.FindSame(nActiveSkillID);

		if (nListIndex)                 //查找激活的当前攻击技能ID
		{
		  if (m_SkillList.GetCurrentLevelByIdx(nListIndex) > 0)    //等级大于0的才 增加
		  {
			 if (nIsAdd==0)
				m_SkillList.AddEnChance(nListIndex,-nCurSkillEnchance);
			 else if (nIsAdd > 0)
				m_SkillList.AddEnChance(nListIndex,nCurSkillEnchance);
		  }	 		
		}
	}
}

void KNpc::NewSetNpcEnChance()
{

	m_SkillList.ReEnChance();                  //重置技能数据的加成
#ifdef _SERVER	
	ReSkillEnhance(Player[m_nPlayerIdx].GetLeftSkill(),1);	 //对攻击技能的加成 或满内力对攻击的加成
#else
	ReSkillEnhance(Player[CLIENT_PLAYER_INDEX].GetLeftSkill(),1);	
#endif
	_EnhanceInfo::iterator it;
	for( it = nEnhanceInfo.begin(); it != nEnhanceInfo.end(); ++it)
	{
		if (it->second.nSkillIdx>0)
			ReWhereSkillEnhance(it->second.nSkillIdx,it->second.nEnhance,1);	 		
	}
}

//脚本调用
VOID KNpc::SetActiveForever(BOOL bIsActiveForever)
{
	BOOL bIsActive = (bIsActiveForever != 0);	// bIsActiveForever 可能不是0 or 1,转换成0 or 1
	//KG_PROCESS_SUCCESS(IsActiveForever() == bIsActive);
	if  (IsActiveForever() == bIsActive)  //已经加过 或 减少过 就取消了
		goto Exit1;
	
	if (bIsActiveForever)
		SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].AddNearByNpc(1);
	else
		SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].AddNearByNpc(-1);

	m_byActiveForever = (BYTE)bIsActive;

	//printf("----SetActiveForever :%s,%d,%d----\n",Name,bIsActiveForever,m_byActiveForever);

Exit1:
	return ;
} 

/*
	Phan phoi dame rut: Tong = nTotal, rut n lan -> chia ra rut sao cho tong bang nTotal
*/
void KNpc::NormalDistributeDamage(list<int>& arr, int nTotal, int n)
{
	if (n <= 0)
		n = 1;
	list<int> tmpList;
	int nAverage = nTotal / n;
	int nNear = nAverage * 0.3f;
	int nTemp = 0;
	for (int i = 0; i < n - 1; i++)
	{
		int r = GetRandomNumber(nAverage - nNear, nAverage + nNear);
		tmpList.push_back(r);
		nTemp += r;
	}
	tmpList.push_back(nTotal - nTemp);
	list<int>::iterator arrEnd = arr.end();
	for (list<int>::iterator it = tmpList.begin(), it_arr = arr.begin(); it != tmpList.end(); it++)
	{
		if (it_arr != arrEnd)
		{
			*it_arr += *it;
			it_arr++;
		}
		else
		{
			arr.push_back(*it);
		}
	}
}
/*******************************************************************************
// FileName			:	KMissle.cpp
// FileAuthor		:	RomanDou
// FileCreateDate	:	2002-6-10 15:32:22
// FileDescription	:	
// Revision Count	:	
*******************************************************************************/

#include "KCore.h"
#include "KMissle.h"
#include "KSubWorld.h"
#include "KSubWorldSet.h"
#include "KRegion.h"
#include "KNpc.h"
#include "KNpcSet.h"
#include "KMath.h"
#include <math.h>
#include "KSkillSpecial.h"
//#include "myassert.h"
#ifndef _SERVER
#include "../../Represent/iRepresent/iRepresentshell.h"
#include "Scene\KScenePlaceC.h"
#include "ImgRef.h"
#endif
#include "Scene/ObstacleDef.h"
#include "KPlayer.h"
#include "KMissleSet.h"

#ifdef _STANDALONE
#include "KSG_StringProcess.h"
#else
#include "../../Engine/Src/KSG_StringProcess.h"
#endif

/*TCollisionMatrix g_CollisionMatrix[64] =
{
	{0,	0, -1, 1, 0, 1, 1, 1}, // 0--------
	{0,	0, -1, 1, 0, 1, 1, 1}, // 1
	{0,	0, -1, 1, 0, 1, 1, 1}, // 2
	{0,	0, -1, 1, 0, 1, 1, 1}, // 3
	
	{0,	0, -1, 0, -1, 1, 0, 1}, // 4
	{0,	0, -1, 0, -1, 1, 0, 1}, // 5
	{0,	0, -1, 0, -1, 1, 0, 1}, // 6
	{0,	0, -1, 0, -1, 1, 0, 1}, // 7
	{0,	0, -1, 0, -1, 1, 0, 1}, // 8--------
	{0,	0, -1, 0, -1, 1, 0, 1}, // 9
	{0,	0, -1, 0, -1, 1, 0, 1}, // 10
	{0,	0, -1, 0, -1, 1, 0, 1}, // 11
	
	{0,	0, -1, -1, -1, 0, -1, 1}, // 12
	{0,	0, -1, -1, -1, 0, -1, 1}, // 13
	{0,	0, -1, -1, -1, 0, -1, 1}, // 14
	{0,	0, -1, -1, -1, 0, -1, 1}, // 15
	{0,	0, -1, -1, -1, 0, -1, 1}, // 16--------
	{0,	0, -1, -1, -1, 0, -1, 1}, // 17
	{0,	0, -1, -1, -1, 0, -1, 1}, // 18
	{0,	0, -1, -1, -1, 0, -1, 1}, // 19
	
	{0,	0, 0, -1, -1, -1, -1, 0}, // 20
	{0,	0, 0, -1, -1, -1, -1, 0}, // 21
	{0,	0, 0, -1, -1, -1, -1, 0}, // 22
	{0,	0, 0, -1, -1, -1, -1, 0}, // 23
	{0,	0, 0, -1, -1, -1, -1, 0}, // 24--------
	{0,	0, 0, -1, -1, -1, -1, 0}, // 25
	{0,	0, 0, -1, -1, -1, -1, 0}, // 26
	{0,	0, 0, -1, -1, -1, -1, 0}, // 27
	
	{0,	0, -1, -1, 0, -1, 1, -1}, // 28
	{0,	0, -1, -1, 0, -1, 1, -1}, // 29
	{0,	0, -1, -1, 0, -1, 1, -1}, // 30
	{0,	0, -1, -1, 0, -1, 1, -1}, // 31
	{0,	0, -1, -1, 0, -1, 1, -1}, // 32--------
	{0,	0, -1, -1, 0, -1, 1, -1}, // 33
	{0,	0, -1, -1, 0, -1, 1, -1}, // 34
	{0,	0, -1, -1, 0, -1, 1, -1}, // 35
	
	{0,	0, 0, -1, 1, -1, 1, 0}, // 36
	{0,	0, 0, -1, 1, -1, 1, 0}, // 37
	{0,	0, 0, -1, 1, -1, 1, 0}, // 38
	{0,	0, 0, -1, 1, -1, 1, 0}, // 39
	{0,	0, 0, -1, 1, -1, 1, 0}, // 40--------
	{0,	0, 0, -1, 1, -1, 1, 0}, // 41
	{0,	0, 0, -1, 1, -1, 1, 0}, // 42
	{0,	0, 0, -1, 1, -1, 1, 0}, // 43
	
	{0,	0, 1, -1, 1, 0, 1, 1}, // 44
	{0,	0, 1, -1, 1, 0, 1, 1}, // 45
	{0,	0, 1, -1, 1, 0, 1, 1}, // 46
	{0,	0, 1, -1, 1, 0, 1, 1}, // 47
	{0,	0, 1, -1, 1, 0, 1, 1}, // 48--------
	{0,	0, 1, -1, 1, 0, 1, 1}, // 49
	{0,	0, 1, -1, 1, 0, 1, 1}, // 50
	{0,	0, 1, -1, 1, 0, 1, 1}, // 51
	
	{0,	0, 1, 0, 1, 1, 0, 1}, // 52
	{0,	0, 1, 0, 1, 1, 0, 1}, // 53
	{0,	0, 1, 0, 1, 1, 0, 1}, // 54
	{0,	0, 1, 0, 1, 1, 0, 1}, // 55
	{0,	0, 1, 0, 1, 1, 0, 1}, // 56---------
	{0,	0, 1, 0, 1, 1, 0, 1}, // 57
	{0,	0, 1, 0, 1, 1, 0, 1}, // 58
	{0,	0, 1, 0, 1, 1, 0, 1}, // 59
	
	{0,	0, -1, 1, 0, 1, 1, 1}, // 60
	{0,	0, -1, 1, 0, 1, 1, 1}, // 61
	{0,	0, -1, 1, 0, 1, 1, 1}, // 62
	{0,	0, -1, 1, 0, 1, 1, 1}, // 63
};
*/
KMissle g_MisslesLib[MAX_MISSLESTYLE];// *g_MisslesLib;//

//每个格子的像素长宽
#define CellWidth		(SubWorld[m_nSubWorldId].m_nCellWidth << 10)
#define CellHeight		(SubWorld[m_nSubWorldId].m_nCellHeight << 10)

//每个region格点长宽
#define RegionWidth		(SubWorld[m_nSubWorldId].m_nRegionWidth)
#define RegionHeight	(SubWorld[m_nSubWorldId].m_nRegionHeight)

#define CurRegion		SubWorld[m_nSubWorldId].m_Region[m_nRegionId]
#define CurSubWorld		SubWorld[m_nSubWorldId]

#define LeftRegion(nRegionId)	SubWorld[m_nSubWorldId].m_Region[nRegionId].m_nConnectRegion[2]
#define RightRegion(nRegionId)		SubWorld[m_nSubWorldId].m_Region[nRegionId].m_nConnectRegion[6]
#define UpRegion(nRegionId)		SubWorld[m_nSubWorldId].m_Region[nRegionId].m_nConnectRegion[4]
#define DownRegion(nRegionId)		SubWorld[m_nSubWorldId].m_Region[nRegionId].m_nConnectRegion[0]

//随机移动魔法的左右偏移表
//int g_nRandMissleTab[100] = {0};

CORE_API KMissle *Missle = NULL;//Missle[MAX_MISSLE];

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
KMissle::KMissle()
{
	m_nMissleId = -1;
	m_nCollideOrVanishTime = 0;
	m_ulDamageInterval = 0;
	m_nTempParam1 = 0;
	m_nTempParam2 = 0;
	m_nFirstReclaimTime = 0;
	m_nEndReclaimTime = 0;
	m_nRate=0;                //  子弹击中的概率
	m_nHitCount=0; 
	m_nLastDoCollisionIdx=0;
#ifdef _SERVER
	m_pMagicAttribsData = NULL;
	m_ulNextCalDamageTime = 0;
	m_bDrag               = 0;                //  是否有拉扯状态
	m_nIsActivate         = 0;
	m_btRandomParam = 0;
#else
	m_bFollowNpcWhenCollid = 1;
	m_bRemoving	= FALSE;
	m_btRedLum = m_btGreenLum = m_btBlueLum = 0xff;
	m_usLightRadius = 50;	//灯光范围
	m_SceneID=0;
#endif
}

void KMissle::Release()
{
///#pragma	message(ATTENTION("子弹消亡时，需更新发送者使用该技能时的当前使用次数，使之减一"))
#ifndef _SERVER	
   if (m_SceneID)
   {
	 g_ScenePlace.RemoveObject(CGOG_MISSLE, m_nMissleId,m_SceneID);	 //
	 m_SceneID=0;
   }

	//SubWorld[m_nSubWorldId].m_WorldMessage.Send(GWM_MISSLE_DEL, m_nMissleId);

	m_MissleRes.Clear();
	m_nMissleId = -1;
	m_nFollowNpcIdx = 0;
#endif
	m_nIsActivate =0;
	m_nRate=0;                // 子弹击中的概率
	m_nHitCount=0; 
	m_nLastDoCollisionIdx=0;
#ifdef _SERVER
	if (m_pMagicAttribsData)
	{
		//printf("--子弹消亡(%d)--\n",m_pMagicAttribsData->GetRef());
		if (m_pMagicAttribsData->DelRef() <= 0)
		{
		    m_pMagicAttribsData->Remove();
			delete m_pMagicAttribsData;
		}
	}
	 m_pMagicAttribsData = NULL;
#endif
}

KMissle::~KMissle()
{
	
}
/*!*****************************************************************************
// Function		: KMissle::GetInfoFromTabFile
// Purpose		: 获得TabFile有关子弹的基本信息
// Return		: BOOL 
// Argumant		: int nMissleId
// Comments		:
// Author		: RomanDou
*****************************************************************************/
BOOL KMissle::GetInfoFromTabFile(int nMissleId)
{
	if (nMissleId <= 0 ) return FALSE;
	KITabFile * pITabFile = &g_MisslesSetting;
	return GetInfoFromTabFile(pITabFile, nMissleId);
}

BOOL KMissle::GetInfoFromTabFile(KITabFile * pMisslesSetting, int nMissleId)
{
	if (nMissleId <= 0 ) return FALSE;
	m_nMissleId		= nMissleId;
	int nRow = nMissleId;

	pMisslesSetting->GetString(nRow, "MissleName",		   "", m_szMissleName,sizeof(m_szMissleName), TRUE);

	int nHeightOld ;
	pMisslesSetting->GetInteger(nRow, "MissleHeight",		0, &nHeightOld, TRUE);
	m_nHeight = nHeightOld << 10; //扩大 1024倍率
	
	pMisslesSetting->GetInteger(nRow, "LifeTime",			0, &m_nLifeTime, TRUE);
	pMisslesSetting->GetInteger(nRow, "Speed",				0, &m_nSpeed, TRUE);   //子弹速度
	pMisslesSetting->GetInteger(nRow, "ResponseSkill",		0, &m_nSkillId, TRUE);
	pMisslesSetting->GetInteger(nRow, "CollidRange",		0, &m_nCollideRange, TRUE);	//碰撞范围
	pMisslesSetting->GetInteger(nRow, "ColVanish",			0, &m_bCollideVanish, TRUE);//碰撞就消亡
	pMisslesSetting->GetInteger(nRow, "CanColFriend",		0, &m_bCollideFriend, TRUE);
	pMisslesSetting->GetInteger(nRow, "CanSlow",			0, &m_bCanSlow, TRUE);
	pMisslesSetting->GetInteger(nRow, "IsRangeDmg",		    0, &m_bRangeDamage, TRUE);   //是否有伤害范围
	pMisslesSetting->GetInteger(nRow, "DmgRange",			0, &m_nDamageRange, TRUE);
	pMisslesSetting->GetInteger(nRow, "MoveKind",			0, (int*)&m_eMoveKind, TRUE);
	pMisslesSetting->GetInteger(nRow, "FollowKind",		0, (int*)&m_eFollowKind, TRUE);
	pMisslesSetting->GetInteger(nRow, "Zacc",				0,(int*)&m_nZAcceleration, TRUE); //Z加速
	pMisslesSetting->GetInteger(nRow, "Zspeed",				0,(int*)&m_nHeightSpeed, TRUE);
	pMisslesSetting->GetInteger(nRow, "Param1",			0, &m_nParam1, TRUE);
	pMisslesSetting->GetInteger(nRow, "Param2",			0, &m_nParam2, TRUE);
	pMisslesSetting->GetInteger(nRow, "Param3",			0, &m_nParam3, TRUE);
	
	BOOL bAutoExplode = 0;

	m_nRate=0;
	m_nHitCount=0;

	pMisslesSetting->GetInteger(nRow, "AutoExplode",    0, (int*)&bAutoExplode, TRUE);
	m_bAutoExplode = bAutoExplode;
	
	pMisslesSetting->GetInteger(nRow, "DmgInterval",	0, (int*)&m_ulDamageInterval, TRUE);
	
#ifndef _SERVER	
	char AnimFileCol[64]={0};
	char SndFileCol[64]={0};
	char AnimFileInfoCol[100]={0};
	char szAnimFileInfo[100]={0};

	const char *pcszTemp = NULL;

	pMisslesSetting->GetInteger(nRow, "RedLum",	    255, (int*)&m_btRedLum, TRUE);
	pMisslesSetting->GetInteger(nRow, "GreenLum",	255, (int*)&m_btGreenLum, TRUE);
	pMisslesSetting->GetInteger(nRow, "BlueLum",	255, (int*)&m_btBlueLum, TRUE);

	int nLightRadius = 0;
	pMisslesSetting->GetInteger(nRow, "LightRadius", 50, (int*)&nLightRadius, TRUE);
	m_usLightRadius = nLightRadius;

	pMisslesSetting->GetInteger(nRow, "MultiShow",		0, &m_bMultiShow, TRUE);

	for (int i  = 0; i < MAX_MISSLE_STATUS; ++i) //子弹状态
	{
		sprintf(AnimFileCol, "AnimFile%d", i + 1);
		sprintf(SndFileCol,  "SndFile%d", i + 1);
		sprintf(AnimFileInfoCol, "AnimFileInfo%d", i + 1);

		pMisslesSetting->GetString(nRow, AnimFileCol,			"", m_MissleRes.m_MissleRes[i].AnimFileName, sizeof(m_MissleRes.m_MissleRes[i].AnimFileName), TRUE);
		pMisslesSetting->GetString(nRow, SndFileCol,			"", m_MissleRes.m_MissleRes[i].SndFileName,sizeof(m_MissleRes.m_MissleRes[i].SndFileName), TRUE);
		pMisslesSetting->GetString(nRow, AnimFileInfoCol,		"", szAnimFileInfo, sizeof(szAnimFileInfo), TRUE);

		pcszTemp = szAnimFileInfo;

		m_MissleRes.m_MissleRes[i].nTotalFrame = KSG_StringGetInt(&pcszTemp, 100);
		KSG_StringSkipSymbol(&pcszTemp, ',');
		m_MissleRes.m_MissleRes[i].nDir = KSG_StringGetInt(&pcszTemp, 16);
		KSG_StringSkipSymbol(&pcszTemp, ',');
		m_MissleRes.m_MissleRes[i].nInterval = KSG_StringGetInt(&pcszTemp, 1);

		sprintf(AnimFileCol, "AnimFileB%d", i + 1);
		sprintf(SndFileCol,  "SndFileB%d", i + 1);
		sprintf(AnimFileInfoCol, "AnimFileInfoB%d", i + 1);

		pMisslesSetting->GetString(nRow, AnimFileCol,			"", m_MissleRes.m_MissleRes[i + MAX_MISSLE_STATUS].AnimFileName,sizeof(m_MissleRes.m_MissleRes[i + MAX_MISSLE_STATUS].AnimFileName), TRUE);
		pMisslesSetting->GetString(nRow, SndFileCol,			"", m_MissleRes.m_MissleRes[i + MAX_MISSLE_STATUS].SndFileName, sizeof(m_MissleRes.m_MissleRes[i + MAX_MISSLE_STATUS].SndFileName), TRUE);
		pMisslesSetting->GetString(nRow, AnimFileInfoCol,		"", szAnimFileInfo, sizeof(szAnimFileInfo), TRUE);

		pcszTemp = szAnimFileInfo;
		m_MissleRes.m_MissleRes[i + MAX_MISSLE_STATUS].nTotalFrame = KSG_StringGetInt(&pcszTemp, 100);
		KSG_StringSkipSymbol(&pcszTemp, ',');
		m_MissleRes.m_MissleRes[i + MAX_MISSLE_STATUS].nDir = KSG_StringGetInt(&pcszTemp, 16);
		KSG_StringSkipSymbol(&pcszTemp, ',');
		m_MissleRes.m_MissleRes[i + MAX_MISSLE_STATUS].nInterval = KSG_StringGetInt(&pcszTemp, 1);

		//sscanf(szAnimFileInfo, "%d,%d,%d", 
		//	&m_MissleRes.m_MissleRes[i + MAX_MISSLE_STATUS].nTotalFrame,
		//	&m_MissleRes.m_MissleRes[i + MAX_MISSLE_STATUS].nDir,
		//	&m_MissleRes.m_MissleRes[i + MAX_MISSLE_STATUS].nInterval
		//);

	}
	int ninVal = 0;
	pMisslesSetting->GetInteger(nRow, "LoopPlay",0,&ninVal,TRUE);
	m_MissleRes.m_bLoopAnim = ninVal;
	pMisslesSetting->GetInteger(nRow, "SubLoop",0,&ninVal, TRUE);
	m_MissleRes.m_bSubLoop = ninVal;
	pMisslesSetting->GetInteger(nRow, "SubStart",0,&ninVal, TRUE);
	m_MissleRes.m_nSubStart=ninVal;
	pMisslesSetting->GetInteger(nRow, "SubStop",0,&ninVal, TRUE);
	m_MissleRes.m_nSubStop =ninVal;
	pMisslesSetting->GetInteger(nRow, "ColFollowTarget",0,(int *)&m_bFollowNpcWhenCollid, TRUE);
#endif
	return TRUE;
}

BOOL KMissle::Init( int nLauncher, int nMissleId, int nXFactor, int nYFactor, int nLevel)
{
#ifndef _SERVER
	m_MissleRes.Init();
#endif
	return	TRUE;
}

/*!*****************************************************************************
// Function		: KMissle::Activate
// Purpose		: 
// Return		: void 
// Comments		:
// Author		: RomanDou
*****************************************************************************/
//技能子弹设置 无限循环的子弹碰撞伤害效果
int KMissle::Activate()   
{	
	 
	if (m_nMissleId <= 0 || m_nRegionId < 0 || m_nIsActivate<=0)
		return  0;

	if (m_nLauncher <= 0 || m_nLauncher>=MAX_NPC)
	{
		DoVanish();
		return 0;
	}
	
	//子弹的主人已经离开，So 子弹消亡 Npc[m_nLauncher].IsMatch(m_dwLauncherId)
	if (Npc[m_nLauncher].m_dwID!=m_dwLauncherId || Npc[m_nLauncher].m_SubWorldIndex != m_nSubWorldId || Npc[m_nLauncher].m_RegionIndex < 0)
	{
		DoVanish();
		return 0;	
	}
	
	//跟踪的目标人物已经不在该地图上时，自动清空
	if (m_nFollowNpcIdx > 0  && m_nFollowNpcIdx<MAX_NPC)
	{
	//	if (!Npc[m_nFollowNpcIdx].IsMatch(m_dwFollowNpcID) )
	//		m_nFollowNpcIdx = 0;

		if (Npc[m_nFollowNpcIdx].m_SubWorldIndex != m_nSubWorldId)
			m_nFollowNpcIdx = 0;  //被子弹跟踪的 NPC索引

		if  (Npc[m_nFollowNpcIdx].m_Doing==do_death || Npc[m_nFollowNpcIdx].m_Doing==do_revive)
			m_nFollowNpcIdx = 0;  //被子弹跟踪的 NPC索引
		
		//else if (!this->m_eFollowKind)
		//{//如果两个的距离 大于30 就设置为零
		//	int nSrcMpsX = 0, nSrcMpsY = 0, nDesMpsX = 0, nDesMpsY = 0, nDmap, nDistance = 0;
		//	GetMpsPos(&nSrcMpsX, &nSrcMpsY);
		//	Npc[m_nFollowNpcIdx].GetMpsPos(&nDesMpsX, &nDesMpsY, &nDmap);
		//	nDistance = SubWorld[m_nSubWorldId].GetDistance(nSrcMpsX, nSrcMpsY, nDesMpsX, nDesMpsY);
		//	if (nDistance > _gameGlobalSet.SubWorldInfo.nSkillStateKeepRange)
		//		m_nFollowNpcIdx = 0;  //被子弹跟踪的 NPC索引
		//}
	}
	
	eMissleStatus eLastStatus = m_eMissleStatus;
	
	//如果当前状态是子弹生命正常结束正准备消亡状态时，而不是消亡中或者已碰撞中
	if (m_nCurrentLife >= m_nLifeTime && m_eMissleStatus != MS_DoVanish && m_eMissleStatus != MS_DoCollision)
	{//飞行当中 时间大于存活时间就消亡
		if (m_bAutoExplode)
		{// 消亡前是否再次碰撞
			ProcessCollision();//处理碰撞

			if (m_bCollideEvent)	
			{
				if (m_nSkillId<MAX_SKILL && m_nSkillId>0 && m_nLevel>0 && m_nLevel<MAX_SKILLLEVEL)
				{
					KSkill * pOrdinSkill = (KSkill *)g_SkillManager.GetSkill(m_nSkillId, m_nLevel);
					if (pOrdinSkill)
						pOrdinSkill->Collidsion(this);
				}
			}
		}
		//消亡了
		DoVanish();
#ifdef _SERVER
         m_nCurrentLife++;
   //InterlockedIncrement(&m_nCurrentLife);
         return 1;
#endif				
	}
	//--------------
	if (m_nCurrentLife == m_nStartLifeTime && m_eMissleStatus != MS_DoVanish)	
	{
		if (PrePareFly())
		{
#ifndef _SERVER
			int nSrcX2 = 0 ;
			int nSrcY2 = 0 ;
			SubWorld[0].NewMap2Mps(m_nRegionId, m_nCurrentMapX, m_nCurrentMapY,m_nXOffset, m_nYOffset, &nSrcX2, &nSrcY2);
			m_MissleRes.PlaySound(MS_DoFly, nSrcX2, nSrcY2, 0);
			//CreateSpecialEffect(MS_DoFly, nSrcX2, nSrcY2, m_nCurrentMapZ); //创建飞行效果
#endif
			DoFly();	//设置飞行状态
		}
		else
			DoVanish();
	}
	//--------
	switch(m_eMissleStatus)
	{
	case MS_DoWait:
		{
			//OnWait();
		}
		break;
	case MS_DoFly:
		{
			OnFly();

			if (m_bFlyEvent)
			{
		    	if (m_nFlyEventTime==0)
				{
					DoVanish();
					break;
				}

				if ((m_nCurrentLife - m_nStartLifeTime)%m_nFlyEventTime == 0 )
				{
					//_ASSERT(m_nSkillId < MAX_SKILL && m_nLevel < MAX_SKILLLEVEL);
					//if (m_nLevel  <= 0 ) return 0;

                    if (m_nSkillId >=MAX_SKILL || m_nLevel >=MAX_SKILLLEVEL || m_nLevel <= 0)
						return 0;

					KSkill * pOrdinSkill = (KSkill *) g_SkillManager.GetSkill(m_nSkillId , m_nLevel);
					if (pOrdinSkill)
					{
						pOrdinSkill->FlyEvent(this);
					}
				}
			}
		}
		break;
	case MS_DoCollision:
		{
			OnCollision();
		}
		break;
	case MS_DoVanish:
		{
			OnVanish();
		}
		break;
	}
	
#ifndef _SERVER
	//子弹未消亡掉 绘画 有可能导致使用玫瑰花 客户端 崩溃
	if (m_nMissleId > 0 && m_eMissleStatus != MS_DoVanish)
	{
		int nSrcX;
		int nSrcY;
		
		SubWorld[0].NewMap2Mps(m_nRegionId, m_nCurrentMapX, m_nCurrentMapY,m_nXOffset, m_nYOffset, &nSrcX, &nSrcY);
		
		if (m_usLightRadius && m_eMissleStatus != MS_DoWait)//绘画子弹
		{//有光照范围的
			g_ScenePlace.MoveObject(CGOG_MISSLE, m_nMissleId, nSrcX, nSrcY, m_nCurrentMapZ, m_SceneID, IPOT_RL_OBJECT | IPOT_RL_LIGHT_PROP );
		}
		else
			g_ScenePlace.MoveObject(CGOG_MISSLE, m_nMissleId, nSrcX, nSrcY, m_nCurrentMapZ, m_SceneID, IPOT_RL_OBJECT);
	} 
	
#endif
	m_nCurrentLife ++;
	//InterlockedIncrement(&m_nCurrentLife);
	return 1;
}

/*!*****************************************************************************
// Function		: KMissle::OnWait
// Purpose		: 
// Return		: void 
// Comments		:
// Author		: RomanDou
********************************************************************************/
void KMissle::OnWait()
{
	return;
}
/*!*****************************************************************************
// Function		: KMissle::OnCollision
// Purpose		: 
// Return		: void 
// Comments		:
// Author		: RomanDou
*****************************************************************************/
void KMissle::OnCollision()
{
	return;	
}

// 1表示正常碰撞到物体，0表示未碰撞到任何物体, -1表示落地
int KMissle::FsCheckCollision()
{
	if  (m_nLauncher<=0 || m_nLauncher>=MAX_NPC || m_nSubWorldId<0 || m_nSubWorldId>=MAX_SUBWORLD)
		return FALSE;

/*#ifdef _SERVER
	int nCurIdx=0;
	g_GameSetTing.GetInteger("Gameconfig","checkSkillId",-1,&nCurIdx);
	if (Npc[m_nLauncher].IsPlayer() && m_nSkillId==nCurIdx)
		printf("---------检测子弹飞行(%s)使用技能:%d --------\n",Npc[m_nLauncher].Name,m_nSkillId);
#endif
	*/
	
#ifdef TOOLVERSION
	return FALSE;
#endif
	
	if (m_nCurrentMapZ <= MISSLE_MIN_COLLISION_ZHEIGHT) 
		return -1;
	
	//子弹在高于一定高度时，不处理越界碰撞问题
	if (m_nCurrentMapZ > MISSLE_MAX_COLLISION_ZHEIGHT) 
		return 0;
	
	if (m_nRegionId < 0) 
		return -1;

	int nAbsX = 0;
	int nAbsY = 0;
	int nCellWidth  = CellWidth;
	int nCellHeight = CellHeight;
	//_ASSERT(nCellWidth > 0 && nCellHeight > 0);
	if  (nCellWidth<=0 || nCellHeight<=0)
		return FALSE;

	int nRMx = 0;
	int nRMy = 0;
	int nSearchRegion = 0;
	int nNpcIdx = 0;
	int nDX = 0;
	int nDY = 0;
	int nNpcOffsetX = 0;
	int nNpcOffsetY = 0;
	BOOL bCollision = FALSE;
	
	int nColRegion = m_nRegionId;
	int nColMapX = m_nCurrentMapX;	   //子弹的当前格子坐标 X:0-16 Y:0-32
	int nColMapY = m_nCurrentMapY;
		
	if (m_nCollideRange==1) //碰撞范围
	{//单个攻击

		/*if (m_bNeedReclaim && m_nCurrentLife >= m_nFirstReclaimTime && m_nCurrentLife <= m_nEndReclaimTime)
		{
		if (m_nCurrentLife == m_nEndReclaimTime) 
		m_bNeedReclaim = FALSE;

		nNpcIdx = 	CheckNearestCollision();
		}
		else
		{
		nNpcIdx = SubWorld[m_nSubWorldId].m_Region[nColRegion].FindNpc(nColMapX, nColMapY, m_nLauncher, m_eRelation);
		}*/

		//if (!this->m_eFollowKind /*&& this->m_eMoveKind == MISSLE_MMK_Line*/ && this->m_nFollowNpcIdx>0)
		//	nNpcIdx=this->m_nFollowNpcIdx; //技能锁定的 npc索引
		//else
		nNpcIdx=CheckNearestCollision();

		if (nNpcIdx > 0 && nNpcIdx<MAX_NPC)
		{
			//if (!m_bRangeDamage)//在目标Npc处碰撞
			if (m_nDamageRange == 1)//在目标Npc处碰撞
				ProcessCollision(m_nLauncher, Npc[nNpcIdx].m_RegionIndex , Npc[nNpcIdx].m_MapX, Npc[nNpcIdx].m_MapY, m_nDamageRange , m_eRelation,0/*nNpcIdx*/);
			else
				ProcessCollision();//在子弹位置处理碰撞

			if(m_nLastDoCollisionIdx == nNpcIdx)
			{
				//to do
			}
			else
			{
				if(this->m_eMoveKind == MISSLE_MMK_Line)
					m_nLastDoCollisionIdx = nNpcIdx;

				DoCollision();//子弹作碰撞后的效果
			}
			//DoCollision();//子弹作碰撞后的效果
			return 1;
		}
	}
	else
	{//群攻
		for (int i = -m_nCollideRange; i <= m_nCollideRange; i ++)
			for (int j = -m_nCollideRange; j <= m_nCollideRange; j ++)
			{
				if (!GetOffsetAxis(m_nSubWorldId, m_nRegionId, m_nCurrentMapX, m_nCurrentMapY, i , j , nSearchRegion, nRMx, nRMy))
					continue;

				if  (nSearchRegion<0)
				    continue;

				nNpcIdx = SubWorld[m_nSubWorldId].m_Region[nSearchRegion].FindNpc(nRMx, nRMy, m_nLauncher, m_eRelation);
				if (nNpcIdx > 0)
				{
					//if (!m_bRangeDamage)//在目标Npc处碰撞
					//	ProcessCollision(m_nLauncher, Npc[nNpcIdx].m_RegionIndex , Npc[nNpcIdx].m_MapX, Npc[nNpcIdx].m_MapY, m_nDamageRange , m_eRelation,nNpcIdx);
					//else
					ProcessCollision();//在子弹位置处理碰撞

					DoCollision();            //子弹作碰撞后的效果
					return 1;
				}
			}
	}
	return 0;
}

inline UINT	KMissle::GetCurrentSubWorldTime()
{
	return SubWorld[m_nSubWorldId].m_dwCurrentTime;
}
//技能子弹设置
void KMissle::OnFly()
{
	if (m_nLauncher <= 0 || m_nLauncher>=MAX_NPC)
	{
		DoVanish();
		return ;
	}

	if (m_nInteruptTypeWhenMove)
	{
		//当发送者位置移动了，不仅正从do_wait状态到do_fly状态的新子弹被消失掉
		//而且已进入dofly状态的旧的所属子弹也要强制消失掉
		if (m_nInteruptTypeWhenMove == Interupt_EndOldMissleLifeWhenMove)
		{
			int nPX, nPY,nMp;
			Npc[m_nLauncher].GetMpsPos(&nPX, &nPY,&nMp);
			if (nPX != m_nLauncherSrcPX || nPY != m_nLauncherSrcPY)
			{
				
#ifndef _SERVER 
				int nSrcX2 = 0 ;
				int nSrcY2 = 0 ;
				SubWorld[0].NewMap2Mps(m_nRegionId, m_nCurrentMapX, m_nCurrentMapY,m_nXOffset, m_nYOffset, &nSrcX2, &nSrcY2);
				CreateSpecialEffect(MS_DoVanish, nSrcX2, nSrcY2, m_nCurrentMapZ);
#endif
				
				DoVanish();
				return ;
			}
		}
	}
	
//飞行检测当前位置是否有障碍
	if (TestBarrier()) 
	{//有障碍物
#ifndef _SERVER 
		int nSrcX3 = 0 ;
		int nSrcY3 = 0 ;
		SubWorld[0].NewMap2Mps(m_nRegionId, m_nCurrentMapX, m_nCurrentMapY,m_nXOffset, m_nYOffset, &nSrcX3, &nSrcY3);
		CreateSpecialEffect(MS_DoVanish, nSrcX3, nSrcY3, m_nCurrentMapZ);
#endif
		DoVanish(); //子弹消亡  
		return;
	}


	if (m_nSubWorldId<0 || m_nSubWorldId>=MAX_SUBWORLD)
	{
		DoVanish(); //子弹消亡  
		return;
	}
	
	int nDOffsetX = 0; //子弹的飞行偏移i?
	int nDOffsetY = 0;

	if (m_nSpeed>32)
		m_nSpeed=32;
	 
	ZAxisMove();   //Z轴加速

	if (FsCheckCollision() == -1) 
	{//子弹落地了
		if (m_bAutoExplode)
		{//是否再次	碰撞
			ProcessCollision();//处理碰撞
		}			  

#ifndef _SERVER 
		int nSrcX4 = 0 ;
		int nSrcY4 = 0 ;
		SubWorld[0].NewMap2Mps(m_nRegionId, m_nCurrentMapX, m_nCurrentMapY,m_nXOffset, m_nYOffset, &nSrcX4, &nSrcY4);
		CreateSpecialEffect(MS_DoVanish, nSrcX4, nSrcY4, m_nCurrentMapZ);
#endif
		DoVanish();
		return;
	}

	//如果不能飞了就不飞了，不往下执行代码了
	if (m_eMissleStatus != MS_DoFly)
		return;
	
	switch(this->m_eMoveKind) //移动类型
	{
	case	MISSLE_MMK_Stand:							//	原地
		    break;
	case	MISSLE_MMK_Parabola:					//	抛物线
	case	MISSLE_MMK_Line:							//	直线飞行
		{
			if(this->m_nFollowNpcIdx > 0 && this->m_nFollowNpcIdx<MAX_NPC && this->m_eFollowKind)  //丐帮技能
			{//跟随攻击
				int nDistance = 0;
				int nSrcMpsX = 0;
				int nSrcMpsY = 0;
				int nDesMpsX = 0;
				int nDesMpsY = 0;

				if (m_nTempParam1 ++ >= 8)
				{
					m_nTempParam1 = 0;
					int nDmap=0;

					GetMpsPos(&nSrcMpsX,&nSrcMpsY);
					Npc[m_nFollowNpcIdx].GetMpsPos(&nDesMpsX,&nDesMpsY,&nDmap);
					nDistance = SubWorld[m_nSubWorldId].GetDistance(nSrcMpsX, nSrcMpsY, nDesMpsX, nDesMpsY);

					if (nDistance != 0)
					{
						int nXFactor = ((nDesMpsX - nSrcMpsX ) << 10) / nDistance;
						int nYFactor = ((nDesMpsY - nSrcMpsY ) << 10) / nDistance;
						m_nDirIndex		= g_GetDirIndex(nSrcMpsX, nSrcMpsY, nDesMpsX, nDesMpsY);
						m_nDir			= g_DirIndex2Dir(m_nDirIndex, MaxMissleDir);
						m_nXFactor = nXFactor;
						m_nYFactor = nYFactor;
					}
				}
			}

			nDOffsetX	 = (m_nXFactor * m_nSpeed);
			nDOffsetY	 = (m_nYFactor * m_nSpeed);
		}break;
	case MISSLE_MMK_RollBack: 
		{
			if (!m_nTempParam1)	
			{
				if (m_nTempParam2 <= m_nCurrentLife)
				{
					m_nXFactor = -m_nXFactor;
					m_nYFactor = -m_nYFactor;
					m_nTempParam1 = 1;
					m_nDir = m_nDir - (MaxMissleDir>>1);
					if (m_nDir < 0) m_nDir += MaxMissleDir;
				}
			}

			nDOffsetX = (m_nSpeed * m_nXFactor);
			nDOffsetY = (m_nSpeed * m_nYFactor);
		}break;
		//按照设计方案，随机飞行无法达到客服两端的同步
	case	MISSLE_MMK_Random:							//	随机飞行（暗黑二女巫的Charged Bolt）
		{
			
		}break;
		//参数一表示顺时针还是逆时针转动
		//参数二表示固定原心还是围饶发动者
		//dx = SinA * R
		//dy = Ctg(90-A/2).R = SinA*SinA / (1 + CosA) * R
	case	MISSLE_MMK_Circle:							//	环行飞行（围绕在身边，暗黑二刺客的集气） 百毒穿心
		{
			int nPreAngle = m_nAngle - 1;
			if (nPreAngle < 0) nPreAngle = MaxMissleDir - 1;
			   m_nDir = m_nAngle + (MaxMissleDir>>2);
			if (m_nDir >= MaxMissleDir) m_nDir = m_nDir - MaxMissleDir;
			//int dx = (m_nSpeed + 250)  * (g_DirCos(m_nAngle,MaxMissleDir) - g_DirCos(nPreAngle,MaxMissleDir)) ;
			//int dy = (m_nSpeed + 50)  * (g_DirSin(m_nAngle,MaxMissleDir) - g_DirSin(nPreAngle, MaxMissleDir)) ; 
			int dx = (m_nSpeed + 30)  * (g_DirCos(m_nAngle,MaxMissleDir) - g_DirCos(nPreAngle,MaxMissleDir)) ;
			int dy = (m_nSpeed + 30)  * (g_DirSin(m_nAngle,MaxMissleDir) - g_DirSin(nPreAngle, MaxMissleDir)) ; 
			
			if (m_nParam2) //原地转
			{
				nDOffsetX = dx;
				nDOffsetY = dy;
			}
			else			// 围绕着发送者转
			{
				int nOldRegion = m_nRegionId;
				//CurRegion.DecRef(m_nCurrentMapX, m_nCurrentMapY, obj_missle);
				m_nRegionId		= Npc[m_nLauncher].m_RegionIndex;
				m_nCurrentMapX	= Npc[m_nLauncher].m_MapX;
				m_nCurrentMapY	= Npc[m_nLauncher].m_MapY;
				//m_nXOffset		= Npc[m_nLauncher].m_OffX;
				//m_nYOffset		= Npc[m_nLauncher].m_OffY;
				m_nXOffset		= Npc[m_nLauncher].m_OffX + 30  * (g_DirCos(m_nAngle,MaxMissleDir) + g_DirCos(nPreAngle, MaxMissleDir));
				m_nYOffset		= Npc[m_nLauncher].m_OffY + 30  * (g_DirSin(m_nAngle,MaxMissleDir) + g_DirSin(nPreAngle,MaxMissleDir));

				//CurRegion.AddRef(m_nCurrentMapX, m_nCurrentMapY, obj_missle);
				
				if (nOldRegion != m_nRegionId)
				{
					//SubWorld[m_nSubWorldId].m_WorldMessage.NewSend(GWM_MISSLE_CHANGE_REGION,nOldRegion,m_nRegionId,m_nMissleId);
					SubWorld[m_nSubWorldId].MissleChangeRegion(nOldRegion, m_nRegionId, m_nMissleId);
				}
				nDOffsetX = dx;
				nDOffsetY = dy;
				//CurRegion.DecRef(m_nCurrentMapX, m_nCurrentMapY, obj_missle);
			}
			
			//顺时针还是逆时针
			if (m_nParam1)
			{
				m_nAngle ++;
				if (m_nAngle >= MaxMissleDir)
					m_nAngle = 0;
			}
			else
			{
				m_nAngle --;
				if (m_nAngle < 0 )
					m_nAngle = MaxMissleDir - 1;
			}
			
		}
		break; 
		//参数一表示顺时针还是逆时针转动
		//参数二表示固定原心还是围饶发动者
	case	MISSLE_MMK_Helix:							//	阿基米德螺旋线（暗黑二游侠的Bless Hammer）
		{
			INT nPreAngle = m_nAngle - 1;
			if (nPreAngle < 0) 
			{
				nPreAngle = MaxMissleDir -1;
			}
			m_nDir = m_nAngle + (MaxMissleDir>>2);
			if (m_nDir >= MaxMissleDir) m_nDir = m_nDir - MaxMissleDir;
			
			INT dx = (m_nSpeed + m_nCurrentLife + 50)  * (g_DirCos(m_nAngle,MaxMissleDir) - g_DirCos(nPreAngle, MaxMissleDir)) ;
			INT dy = (m_nSpeed + m_nCurrentLife + 50)  * (g_DirSin(m_nAngle,MaxMissleDir) - g_DirSin(nPreAngle,MaxMissleDir)) ; 
			
			if (m_nParam2)  //原地转
			{
				nDOffsetX = dx;
				nDOffsetY = dy;
			}
			else			// 围绕着发送者转
			{
				INT nOldRegion = m_nRegionId;
				//				CurRegion.DecRef(m_nCurrentMapX, m_nCurrentMapY, obj_missile);
				m_nRegionId		= Npc[m_nLauncher].m_RegionIndex;
				m_nCurrentMapX	= Npc[m_nLauncher].m_MapX;
				m_nCurrentMapY	= Npc[m_nLauncher].m_MapY;
				m_nXOffset		= Npc[m_nLauncher].m_OffX;
				m_nYOffset		= Npc[m_nLauncher].m_OffY;
				//m_nXOffset		= Npc[m_nLauncher].m_OffX + 50  * (g_DirCos(m_nAngle,MaxMissleDir) + g_DirCos(nPreAngle, MaxMissleDir));
				//m_nYOffset		= Npc[m_nLauncher].m_OffY + 50  * (g_DirSin(m_nAngle,MaxMissleDir) + g_DirSin(nPreAngle,MaxMissleDir));

				
				if (nOldRegion != m_nRegionId)
				{
					//SubWorld[m_nSubWorldId].m_WorldMessage.Send(GWM_MISSILE_CHANGE_REGION, nOldRegion, m_nRegionId, m_nMissileIdx);
					SubWorld[m_nSubWorldId].MissleChangeRegion(nOldRegion, m_nRegionId, m_nMissleId);
				}  
				nDOffsetX = dx;
				nDOffsetY = dy;
			}
			
			if (m_nParam1)
			{
				m_nAngle ++;
				if (m_nAngle >= MaxMissleDir)
					m_nAngle = 0;
			}
			else
			{
				m_nAngle --;
				if (m_nAngle < 0 )
					m_nAngle = MaxMissleDir - 1;
			}
		}
		break; 
	case	MISSLE_MMK_Follow:							//	跟踪目标飞行
		{
			if(this->m_nFollowNpcIdx > 0 && this->m_nFollowNpcIdx<MAX_NPC)
			{
				int nDistance = 0;
				int nSrcMpsX = 0;
				int nSrcMpsY = 0;
				int nDesMpsX = 0;
				int nDesMpsY = 0;

				if (m_nTempParam1 ++ >= 8)
				{
					m_nTempParam1 = 0;
					int nDmap=0;

					GetMpsPos(&nSrcMpsX,&nSrcMpsY);
					Npc[m_nFollowNpcIdx].GetMpsPos(&nDesMpsX,&nDesMpsY,&nDmap);
					nDistance = SubWorld[m_nSubWorldId].GetDistance(nSrcMpsX, nSrcMpsY, nDesMpsX, nDesMpsY);

					if (nDistance != 0)
					{
						int nXFactor = ((nDesMpsX - nSrcMpsX ) << 10) / nDistance;
						int nYFactor = ((nDesMpsY - nSrcMpsY ) << 10) / nDistance;
						m_nDirIndex		= g_GetDirIndex(nSrcMpsX, nSrcMpsY, nDesMpsX, nDesMpsY);
						m_nDir			= g_DirIndex2Dir(m_nDirIndex, MaxMissleDir);
						m_nXFactor = nXFactor;
						m_nYFactor = nYFactor;
					}
				}
			}
			nDOffsetX	 = (m_nXFactor * m_nSpeed);
			nDOffsetY	 = (m_nYFactor * m_nSpeed);
		}break;
	case	MISSLE_MMK_Motion:							//	玩家动作类
		{
			
		}break;
		
	case MISSLE_MMK_SingleLine:						//	必中的单一直线飞行魔法
		{
			//单一必中类子弹，类式于传奇以及其它的同类网络游戏中的基本直线魔法			
#ifdef _SERVER
			
#else
			int x = m_nXOffset;
			int y = m_nYOffset;
			int dx = (m_nSpeed * m_nXFactor);
			int dy = (m_nSpeed * m_nYFactor);
			nDOffsetX	=  dx;//* m_nCurrentLife;
			nDOffsetY	=  dy;//* m_nCurrentLife;
			
#endif
		}
		break;
	default:
		_ASSERT(0);
		
	}
	
	//子弹改变了的偏移
	if (CheckBeyondRegion(nDOffsetX, nDOffsetY))
	{
		/*if (FsCheckCollision() == -1) 
		{//子弹落地了
			if (m_bAutoExplode)
			{//是否再次	碰撞
				ProcessCollision();//处理碰撞
			}			  

#ifndef _SERVER 
			int nSrcX4 = 0 ;
			int nSrcY4 = 0 ;
			SubWorld[0].NewMap2Mps(m_nRegionId, m_nCurrentMapX, m_nCurrentMapY,m_nXOffset, m_nYOffset, &nSrcX4, &nSrcY4);
			CreateSpecialEffect(MS_DoVanish, nSrcX4, nSrcY4, m_nCurrentMapZ);
#endif
			DoVanish();
			return;
		}*/
	}
	else//如果子弹飞行过程中进入了一个无效的Region则子弹自动消亡
	{
		DoVanish();
#ifdef _SERVER
		/*char nMissName[32]={0};
		g_GameSetTing.GetString("Gameconfig","ExpRoleName","",nMissName,sizeof(nMissName));*/
		/*if (Npc[m_nLauncher].IsPlayer() && strstr(Npc[m_nLauncher].Name,_gameGlobalSet.PlayerInfo.nExpRoleName))
		printf("--无效区域子弹消亡 F:%d 偏移:offX:%d,offY:%d,CurMapX:%d,CurMapY:%d,P:%d,N:%s--\n",m_nFollowNpcIdx,nDOffsetX,nDOffsetY,m_nCurrentMapX,m_nCurrentMapY,Npc[m_nLauncher].m_nPeopleIdx,Npc[m_nLauncher].Name);*/
#endif
	}
}
/*!*****************************************************************************
// Function		: KMissle::OnVanish
// Purpose		: 
// Return		: void 
// Comments		:
// Author		: RomanDou
*****************************************************************************/
void KMissle::OnVanish()
{
	/*if (m_nMissleId>0 && m_nMissleId<MAX_MISSLE)
	{
	   //SubWorld[m_nSubWorldId].m_WorldMessage.NewSend(GWM_MISSLE_DEL, m_nMissleId);
		if (Missle[m_nMissleId].m_nSubWorldId>=0 && Missle[m_nMissleId].m_nRegionId >= 0)
			SubWorld[Missle[m_nMissleId].m_nSubWorldId].m_Region[Missle[m_nMissleId].m_nRegionId].RemoveMissle(m_nMissleId);
        	MissleSet.Remove(m_nMissleId);
	} */
}

#ifndef _SERVER
//绘画子弹
void KMissle::Paint()
{//绘画玫瑰花崩溃
	if (m_nMissleId <= 0 ) 
		return;

	int nSrcX;
	int nSrcY;


	SubWorld[0].NewMap2Mps(m_nRegionId, m_nCurrentMapX, m_nCurrentMapY,m_nXOffset, m_nYOffset, &nSrcX, &nSrcY);

	if (!m_nZAcceleration)
	{
		m_MissleRes.Draw(m_eMissleStatus, nSrcX, nSrcY, m_nCurrentMapZ, m_nDir,m_nLifeTime - m_nStartLifeTime,  m_nCurrentLife - m_nStartLifeTime );
	}
	else
	{
		int nDirIndex = g_GetDirIndex(0,0,m_nXFactor, m_nYFactor);
		int nDir = g_DirIndex2Dir(nDirIndex, MaxMissleDir);

		m_MissleRes.Draw(m_eMissleStatus, nSrcX, nSrcY, m_nCurrentMapZ, nDir,m_nLifeTime - m_nStartLifeTime,  m_nCurrentLife - m_nStartLifeTime );
	}


	//对于客户端，直到子弹及其产生的效果全部播放完才终止并删除掉!
	if (m_MissleRes.m_bHaveEnd && (m_MissleRes.SpecialMovieIsAllEnd()))	//表头已经没有了 才删除
	{
		if (m_nMissleId>0 && m_nMissleId<MAX_MISSLE)
		{
		   //SubWorld[m_nSubWorldId].m_WorldMessage.NewSend(GWM_MISSLE_DEL, m_nMissleId);
			if (Missle[m_nMissleId].m_nSubWorldId>=0 && Missle[m_nMissleId].m_nRegionId >= 0)
				SubWorld[Missle[m_nMissleId].m_nSubWorldId].m_Region[Missle[m_nMissleId].m_nRegionId].RemoveMissle(m_nMissleId);
			
			MissleSet.Remove(m_nMissleId);
		}
	}
}
#endif

#define REGION_PIXEL_WIDTH	512
#define REGION_PIXEL_HEIGHT	1024

//BOOL	KMissle::CheckBeyondRegion(int nDOffsetX, int nDOffsetY)
//{
//	if (m_nRegionId < 0)
//		return FALSE;
//	//未动
//	if (nDOffsetX == 0 && nDOffsetY == 0) return TRUE;
//
//	if (abs(nDOffsetX) >= (REGION_PIXEL_WIDTH << 10))
//		return FALSE;
//	
//	if (abs(nDOffsetY) >= (REGION_PIXEL_HEIGHT << 10))
//		return FALSE;
//
//	int nOldRegion = m_nRegionId;
//	int nNewXOffset = m_nXOffset + nDOffsetX;
//	int nNewYOffset = m_nYOffset + nDOffsetY;
//	int nNewMapX = m_nCurrentMapX;
//	int nNewMapY = m_nCurrentMapY;
//	int nNewRegion = m_nRegionId;
//
//	DWORD nRegionWidth = RegionWidth;
//	DWORD nRegionHeight = RegionHeight;
//
//	/*_ASSERT(abs(nNewXOffset) <= CellWidth * 2);
//	_ASSERT(abs(nNewYOffset) <= CellHeight * 2);*/
//	if (abs(nNewXOffset) > CellWidth * 2)
//		return FALSE;
//
//	if (abs(nNewYOffset) > CellHeight * 2)
//		return FALSE;
//
//	//	处理NPC的坐标变幻
//	//	CELLWIDTH、CELLHEIGHT、OffX、OffY均是放大了1024倍
//
//	if (nNewXOffset < 0)
//	{
//		nNewMapX--;
//		nNewXOffset += CellWidth;
//	}
//	else if (nNewXOffset > CellWidth)
//	{
//		nNewMapX++;
//		nNewXOffset -= CellWidth;
//	}
//
//	if (nNewYOffset < 0)
//	{
//		nNewMapY--;
//		nNewYOffset += CellHeight;
//	}
//	else if (nNewYOffset > CellHeight)
//	{
//		nNewMapY++;
//		nNewYOffset -= CellHeight;
//	}
//
//	if (nNewMapX < 0)
//	{
//		nNewRegion = LeftRegion(m_nRegionId);
//		nNewMapX += nRegionWidth;
//	}
//	else if ((DWORD)nNewMapX >= nRegionWidth)
//	{
//		nNewRegion = RightRegion(m_nRegionId);
//		nNewMapX -= nRegionWidth;
//	}
//
//	if (nNewRegion < 0)
//	{
//		return FALSE;
//	}
//
//	if (nNewMapY < 0)
//	{
//		nNewRegion = UpRegion(nNewRegion);
//		nNewMapY += nRegionHeight;
//	}
//	else if (nNewMapY >= RegionHeight)
//	{
//		nNewRegion = DownRegion(nNewRegion);
//		nNewMapY -= nRegionHeight;
//	}
//
//	//下一个位置为不合法位置，则消亡
//	if (nNewRegion < 0)
//		return FALSE;
//	else
//	{
//		m_nRegionId = nNewRegion;
//		m_nCurrentMapX = nNewMapX;
//		m_nCurrentMapY = nNewMapY;
//		m_nXOffset = nNewXOffset;
//		m_nYOffset = nNewYOffset;
//
//		if (nOldRegion != m_nRegionId)
//			SubWorld[m_nSubWorldId].MissleChangeRegion(nOldRegion, m_nRegionId, m_nMissleId);
//	}
//	return TRUE;
//}


BOOL	KMissle::CheckBeyondRegion(int nDOffsetX, int nDOffsetY)
{
	if (m_nRegionId < 0) 
		return FALSE;
	//未动
	if (nDOffsetX == 0 && nDOffsetY == 0) return TRUE;

	if (abs(nDOffsetX) > CellWidth) 
	{
		return FALSE;
	}

	if (abs(nDOffsetY) > CellHeight) 
	{
		return FALSE;
	}

	int nOldRegion		= m_nRegionId;
	int nNewXOffset		= m_nXOffset + nDOffsetX;
	int nNewYOffset		= m_nYOffset + nDOffsetY;
	int nNewMapX		= m_nCurrentMapX;
	int nNewMapY		= m_nCurrentMapY;
	int nNewRegion		= m_nRegionId;
	
	DWORD nRegionWidth  = RegionWidth;
	DWORD nRegionHeight = RegionHeight;
	
	_ASSERT(abs(nNewXOffset) <= CellWidth * 2);
	_ASSERT(abs(nNewYOffset) <= CellHeight * 2);
	
	//	处理NPC的坐标变幻
	//	CELLWIDTH、CELLHEIGHT、OffX、OffY均是放大了1024倍
	
	if (nNewXOffset < 0)
	{
		nNewMapX--;
		nNewXOffset += CellWidth;
	}
	else if (nNewXOffset > CellWidth)
	{
		nNewMapX++;
		nNewXOffset -= CellWidth;
	}
	
	if (nNewYOffset < 0)
	{
		nNewMapY--;
		nNewYOffset += CellHeight;
	}
	else if (nNewYOffset > CellHeight)
	{
		nNewMapY++;
		nNewYOffset -= CellHeight;
	}
	
	if (nNewMapX < 0)
	{
		nNewRegion = LeftRegion(m_nRegionId);
		nNewMapX += nRegionWidth;
	}
	else if ((DWORD)nNewMapX >= nRegionWidth)
	{
		nNewRegion = RightRegion(m_nRegionId);
		nNewMapX -= nRegionWidth;
	}

	if (nNewRegion < 0) 
	{
		return FALSE; 
	}
	
	if (nNewMapY < 0)
	{
		nNewRegion = UpRegion(nNewRegion);
		nNewMapY += nRegionHeight;
	}
	else if (nNewMapY >= RegionHeight)
	{
		nNewRegion = DownRegion(nNewRegion);
		nNewMapY -= nRegionHeight;
	}
	
	//下一个位置为不合法位置，则消亡
	if (nNewRegion < 0) 
	{
		return FALSE; 
	}
	else
	{
		m_nRegionId	   = nNewRegion;
		m_nCurrentMapX = nNewMapX;
		m_nCurrentMapY = nNewMapY;
		m_nXOffset	   = nNewXOffset;
		m_nYOffset	   = nNewYOffset;

		if (nOldRegion != m_nRegionId)
			SubWorld[m_nSubWorldId].MissleChangeRegion(nOldRegion, m_nRegionId, m_nMissleId);

	}
	return TRUE;
}

KMissle&	KMissle::operator=(KMissle& Missle)
{
	Missle.m_nTempParam1	=	0;
	Missle.m_nTempParam2	=	0;
	Missle.m_nDesMapX			=	0;
	Missle.m_nDesMapY			=	0;
	Missle.m_nDesRegion		=	0;
	Missle.m_bNeedReclaim	=	FALSE;
	Missle.m_nFirstReclaimTime = 0;
	Missle.m_nEndReclaimTime = 0;
	//memset(Missle.m_NeedReclaimPos, 0, sizeof(m_NeedReclaimPos));

	Missle.m_bCanSlow		=	m_bCanSlow;
	Missle.m_bCollideEvent	=	m_bCollideEvent;
	Missle.m_bCollideFriend =	m_bCollideFriend;
	Missle.m_bCollideVanish	=	m_bCollideVanish;
	Missle.m_bRangeDamage	=	m_bRangeDamage;
	Missle.m_eFollowKind	=	m_eFollowKind;
	Missle.m_eMoveKind		=	m_eMoveKind;
	Missle.m_nAction		=	m_nAction;
	Missle.m_nAngle			=	m_nAngle;
	Missle.m_nCollideRange	=	m_nCollideRange;
	Missle.m_nCurrentLife	=	0;
	Missle.m_nDamageRange	=	m_nDamageRange;
	Missle.m_nHeight		=	m_nHeight;
	Missle.m_nLifeTime		=	m_nLifeTime;
	Missle.m_nSpeed			=   m_nSpeed;
	Missle.m_nParam1		=	m_nParam1;
	Missle.m_nParam2		=	m_nParam2;
	Missle.m_nParam3		=	m_nParam3;
	Missle.m_nCurrentMapZ	=   (m_nHeight >> 10);
	Missle.m_bFlyEvent		=	m_bFlyEvent;
	Missle.m_nFlyEventTime  =	m_nFlyEventTime;
	Missle.m_nZAcceleration =	m_nZAcceleration;
	Missle.m_nHeightSpeed	=	m_nHeightSpeed;
	Missle.m_bAutoExplode	=	m_bAutoExplode;
	Missle.m_ulDamageInterval = m_ulDamageInterval;
	strcpy(Missle.m_szMissleName	,	m_szMissleName);
	
#ifndef  _SERVER
	strcpy(Missle.m_szMissleName	,	m_szMissleName);
	Missle.m_bMultiShow		=  m_bMultiShow;
	Missle.m_MissleRes.m_bLoopAnim = m_MissleRes.m_bLoopAnim;
	Missle.m_MissleRes.m_bHaveEnd = FALSE;
	Missle.m_btRedLum		= m_btRedLum;
	Missle.m_btGreenLum		= m_btGreenLum;
	Missle.m_btBlueLum		= m_btBlueLum;
	Missle.m_usLightRadius	= m_usLightRadius;
	int nOffset = 0;
	
	//如果是相同的子弹可以以不同方式显示时，则随机产生
	if (m_bMultiShow)		
	{
		if (g_Random(2) == 0)
		{
			nOffset = 0;
		}
		else
			nOffset = MAX_MISSLE_STATUS;
	}
	
	for (int t = 0; t < MAX_MISSLE_STATUS ; t++)
	{
		strcpy(Missle.m_MissleRes.m_MissleRes[t].AnimFileName,m_MissleRes.m_MissleRes[t + nOffset].AnimFileName);
		
		Missle.m_MissleRes.m_MissleRes[t].nTotalFrame = m_MissleRes.m_MissleRes[t + nOffset].nTotalFrame;
		Missle.m_MissleRes.m_MissleRes[t].nDir = m_MissleRes.m_MissleRes[t + nOffset].nDir;
		Missle.m_MissleRes.m_MissleRes[t].nInterval = m_MissleRes.m_MissleRes[t + nOffset].nInterval;
		
		strcpy(Missle.m_MissleRes.m_MissleRes[t].SndFileName,m_MissleRes.m_MissleRes[t + nOffset].SndFileName);
	}
	Missle.m_MissleRes.m_bSubLoop = m_MissleRes.m_bSubLoop;
	Missle.m_MissleRes.m_nSubStart = m_MissleRes.m_nSubStart;
	Missle.m_MissleRes.m_nSubStop = m_MissleRes.m_nSubStop;
#endif	
	
	return (Missle);
}

/*!*****************************************************************************
// Function		: KMissle::ProcessDamage
// Purpose		: 
// Return		: BOOL 
// Argumant		: int nNpcId
// Comments		:
// Author		: RomanDou   无限持续的子弹伤害  子弹技能--伤害攻击技能
*****************************************************************************/
BOOL KMissle::ProcessDamage(int nNpcId,int nRata)
{
	BOOL bCalDamage = FALSE;

	if (nRata>0)
	{
		if  (nRata>100)
			 nRata =0;
		if ((100-nRata)>=g_Random(100))
			bCalDamage=TRUE;
	}
	else
	   bCalDamage=TRUE;

	if (!bCalDamage)
		return FALSE;

	if  (m_nLauncher<=0 || m_nLauncher>=MAX_NPC)
		return FALSE;

	if  (nNpcId<=0 || nNpcId>=MAX_NPC)
		return FALSE;
	
	//_ASSERT (Npc[m_nLauncher].IsMatch(m_dwLauncherId));  //发技能的索引

	if (m_dwLauncherId!=Npc[m_nLauncher].m_dwID)
		return FALSE;

	//设置同伴打人无伤
	if (Npc[m_nLauncher].m_Kind==kind_partner && Npc[nNpcId].IsPlayer())
		return false;
	
	if (m_pMagicAttribsData) 
	{//收到伤害 就设置光环效果

//fix tvb
		int nDameXG = 0;
		if (m_nSkillId == 322)//pha thien tram
		{
			if ((m_nDir >= 10 && m_nDir < 25) || (m_nDir >= 45 && m_nDir < 60))
			{
				nDameXG = 2;
			}
		}
		else if (m_nSkillId == 323)//truy tinh truc nguyet
		{
			if ((m_nDir >= 10 && m_nDir < 25) || (m_nDir >= 45 && m_nDir < 60))
			{
				nDameXG = 2;
			}
		}
		else if (m_nSkillId == 325)//truy phong quyet
		{
			if ((m_nDir >= 10 && m_nDir < 20) || (m_nDir >= 45 && m_nDir < 55))
			{
				nDameXG = 3;
			}
			else if (m_nDir == 60)
			{
				nDameXG = 4;
			}
		}
		else if (m_nSkillId == 302)// Bao Vu Le Hoa
		{
			nDameXG = 5;
		}
		else if (m_nSkillId == 336)// Bang Tung vo anh
		{
			nDameXG = 6;
		}
		else if (m_nSkillId == 357)// phi long tai thien
		{
			nDameXG = 7;
		}
		else if (m_nSkillId == 359)// thien ha vo cau
		{
			nDameXG = 8;
		}		
		else if (m_nSkillId == 105)// Vu Da Le Hoa
		{
			nDameXG = 9;
		}
		else if (m_nSkillId == 328)// Tam Nga Te Tuyet
		{
			nDameXG = 10;
		}
		else if (m_nSkillId == 372)// Ngao Tuyet Tieu Phong
		{
			nDameXG = 11;
		} 
		else if (m_nSkillId == 365)// Thien Dia Vo Cuc
		{
			nDameXG = 12;
		}
		else if (m_nSkillId == 368)// Nhan Kiem Hop Nhat
		{
			nDameXG = 13;
		}
		else if (m_nSkillId == 361)// Van Long Kich
		{
			nDameXG = 14;
		}
		else if (m_nSkillId == 318)// dat ma do giang  
		{
			nDameXG = 15;
		}
		else if (m_nSkillId == 319)// hoanh tao thien quan
		{ 
			nDameXG = 16;
		}
		else if (m_nSkillId == 321)// vo tuong tram
		{
			nDameXG = 16;
		} 
		else if (m_nSkillId == 339)// Nhiep hon nguyet anh
		{
			nDameXG = 17;
		} 
		else if (m_nSkillId == 342)// cuu cung phi tinh
		{
			nDameXG = 18; 
		} 
		else if (m_nSkillId == 380)// Phong Suong Toai Anh
		{
			nDameXG = 19; 
		} 
		else if (m_nSkillId == 341)// Tan Hoa Tieu 6x DM
		{
			nDameXG = 40;
		}

//		printf("Missles: m_nSkillId: %d m_bDoHurt: %d\n", m_nSkillId, m_bDoHurt);
//
		if(m_nRate > 0 && m_nRate > m_btRandomParam)
			return FALSE;
		 

		if(Npc[nNpcId].m_ReturnSkill > 0 || Npc[nNpcId].m_IgnoreSkill > 0)
		{
			KSkill * pOrdinSkill = (KSkill *)g_SkillManager.GetSkill(m_nSkillId, m_nLevel);
			if(!pOrdinSkill)
				return FALSE;
			if(pOrdinSkill->IsTargetEnemy())
			{
				if (pOrdinSkill->GetChildSkillId() == BUFF_SKILL_MISSLE_ID_1 ||
					pOrdinSkill->GetChildSkillId() == BUFF_SKILL_MISSLE_ID_2) 
				{	
					if(Npc[nNpcId].m_ReturnSkill > 0 && m_btRandomParam < Npc[nNpcId].m_ReturnSkill)
					{
						if (Npc[m_nLauncher].ReceiveDamage(nNpcId, m_bIsMelee, m_pMagicAttribsData->m_pDamageMagicAttribs, m_bUseAttackRating, m_bDoHurt))
						{
							//Npc[nNpcId].AutoDoSkill(725,1, -1, nNpcId);//skill dai thua nhu lai chu 12x
							if (m_pMagicAttribsData->m_nStateMagicAttribsNum > 0)
								Npc[m_nLauncher].SetStateSkillEffect(nNpcId, m_nSkillId, m_nLevel, m_pMagicAttribsData->m_pStateMagicAttribs, m_pMagicAttribsData->m_nStateMagicAttribsNum, m_pMagicAttribsData->m_pStateMagicAttribs[0].nValue[1]);
							
							if (m_pMagicAttribsData->m_nImmediateMagicAttribsNum > 0)
								Npc[m_nLauncher].SetImmediatelySkillEffect(nNpcId, m_pMagicAttribsData->m_pImmediateAttribs, m_pMagicAttribsData->m_nImmediateMagicAttribsNum);
						}
					}

					if(Npc[nNpcId].m_IgnoreSkill > 0 && m_btRandomParam < Npc[nNpcId].m_IgnoreSkill)
					{	
						int i = 0;
						for(i = 0; i < m_pMagicAttribsData->m_nStateMagicAttribsNum; i++)
						{
							if(m_pMagicAttribsData->m_pStateMagicAttribs[i].nAttribType != magic_poisonres_p &&
								m_pMagicAttribsData->m_pStateMagicAttribs[i].nAttribType != magic_fireres_p &&
								m_pMagicAttribsData->m_pStateMagicAttribs[i].nAttribType != magic_lightingres_p &&
								m_pMagicAttribsData->m_pStateMagicAttribs[i].nAttribType != magic_physicsres_p &&
								m_pMagicAttribsData->m_pStateMagicAttribs[i].nAttribType != magic_coldres_p &&
								m_pMagicAttribsData->m_pStateMagicAttribs[i].nAttribType != magic_allres_p)
								continue;
							if(m_pMagicAttribsData->m_pStateMagicAttribs[i].nValue[0] < 0)
							{	
								//Npc[nNpcId].AutoDoSkill(724,1, -1, nNpcId);//dao hu thien 12x
								return FALSE;
							}
						}
					}
				}
			}
			if (Npc[nNpcId].ReceiveDamage(m_nLauncher, m_bIsMelee, m_pMagicAttribsData->m_pDamageMagicAttribs, m_bUseAttackRating, m_bDoHurt))
			{
				if (m_pMagicAttribsData->m_nStateMagicAttribsNum > 0)
					Npc[nNpcId].SetStateSkillEffect(m_nLauncher, m_nSkillId, m_nLevel, m_pMagicAttribsData->m_pStateMagicAttribs, m_pMagicAttribsData->m_nStateMagicAttribsNum, m_pMagicAttribsData->m_pStateMagicAttribs[0].nValue[1]);
				
				if (m_pMagicAttribsData->m_nImmediateMagicAttribsNum > 0)
					Npc[nNpcId].SetImmediatelySkillEffect(m_nLauncher, m_pMagicAttribsData->m_pImmediateAttribs, m_pMagicAttribsData->m_nImmediateMagicAttribsNum);
			}
			
		}
		else  
		{	
			//printf("22Missles: m_nSkillId: %d m_bDoHurt: %d nDameXG: %d\n", m_nSkillId, m_bDoHurt, nDameXG);
			if (Npc[nNpcId].ReceiveDamage(m_nLauncher, m_bIsMelee, m_pMagicAttribsData->m_pDamageMagicAttribs, m_bUseAttackRating, m_bDoHurt, nDameXG))//fixTVB
			{
				if (m_pMagicAttribsData->m_nStateMagicAttribsNum > 0)
					Npc[nNpcId].SetStateSkillEffect(m_nLauncher, m_nSkillId, m_nLevel, m_pMagicAttribsData->m_pStateMagicAttribs, m_pMagicAttribsData->m_nStateMagicAttribsNum, m_pMagicAttribsData->m_pStateMagicAttribs[0].nValue[1]);
				
				if (m_pMagicAttribsData->m_nImmediateMagicAttribsNum > 0)
					Npc[nNpcId].SetImmediatelySkillEffect(m_nLauncher, m_pMagicAttribsData->m_pImmediateAttribs, m_pMagicAttribsData->m_nImmediateMagicAttribsNum);
			}
			else
			{
				Npc[nNpcId].SetStateSkillEffect(m_nLauncher, m_nSkillId, m_nLevel, 0, 0, 18);
			} 
		}



/*
		    if (m_pMagicAttribsData->m_nDamageMagicAttribsNum > 0)   //这个技能是攻击伤害技
			{
				//printf("Debug: ReceiveDamage1: %d-%d-%d-%d-[bUseAR: %d]-%d-%d-%d-%d-%d\n",m_nLauncher,m_nMissleSeries, m_bIsMelee, m_pMagicAttribsData->m_pDamageMagicAttribs, m_bUseAttackRating, m_bDoHurt,m_nEnChance,m_nShangBei,m_nSkillId,m_nIsMagic);
                Npc[nNpcId].ReceiveDamage(m_nLauncher,m_nMissleSeries, m_bIsMelee, m_pMagicAttribsData->m_pDamageMagicAttribs, m_bUseAttackRating, m_bDoHurt,m_nEnChance,m_nShangBei,m_nSkillId,m_nIsMagic);
			}
			if (m_pMagicAttribsData->m_nStateMagicAttribsNum > 0)        //将状态属性应用到NPC身上
			{
				if (m_eRelation&relation_enemy)
				{//是对敌人有效得状态 就是诅咒了
					if (Npc[nNpcId].m_ReturnSkill > 0 && g_RandPercent(Npc[nNpcId].m_ReturnSkill)) //状态反弹了
						Npc[m_nLauncher].SetStateSkillEffect(nNpcId, m_nSkillId, m_nLevel, m_pMagicAttribsData->m_pStateMagicAttribs, m_pMagicAttribsData->m_nStateMagicAttribsNum, m_pMagicAttribsData->m_pStateMagicAttribs[0].nValue[1]);
					else
					{
						if (Npc[nNpcId].m_IgnoreSkill > 0 && g_RandPercent(Npc[nNpcId].m_IgnoreSkill)) 
						{//

						}
						else 
					       Npc[nNpcId].SetStateSkillEffect(m_nLauncher, m_nSkillId, m_nLevel, m_pMagicAttribsData->m_pStateMagicAttribs, m_pMagicAttribsData->m_nStateMagicAttribsNum, m_pMagicAttribsData->m_pStateMagicAttribs[0].nValue[1]);
					}
				
				}
				else//其他得状态
				   Npc[nNpcId].SetStateSkillEffect(m_nLauncher, m_nSkillId, m_nLevel, m_pMagicAttribsData->m_pStateMagicAttribs, m_pMagicAttribsData->m_nStateMagicAttribsNum, m_pMagicAttribsData->m_pStateMagicAttribs[0].nValue[1]);
			}
		    //if (m_pMagicAttribsData->m_nImmediateMagicAttribsNum > 0) //将状态属性应用到NPC身上
			//	Npc[nNpcId].SetImmediatelySkillEffect(m_nLauncher, m_pMagicAttribsData->m_pImmediateAttribs, m_pMagicAttribsData->m_nImmediateMagicAttribsNum);
	   return TRUE;
	}
*/



	/*if (Npc[m_nLauncher].IsPlayer())
	{
		char nMissName[32]={0};
		g_GameSetTing.GetString("Gameconfig","ExpRoleName","",nMissName,sizeof(nMissName));
		if (strstr(Npc[m_nLauncher].Name,nMissName))
		    printf("--ProcessDamage:%s 索引:%d,N:%s,R:%d--\n",Npc[nNpcId].Name,nNpcId,Npc[m_nLauncher].Name,m_nRate);
	}*/

	}
	return FALSE;
}
//子弹消亡
void KMissle::DoVanish()
{
	if (m_eMissleStatus == MS_DoVanish) return ;
	
#ifndef _SERVER
	m_MissleRes.m_bHaveEnd = TRUE;	//设置客户端 已经播放完毕
	m_nCollideOrVanishTime = m_nCurrentLife;
#endif
	if (m_bVanishedEvent)	
	{//子弹消亡时 释放的结束时的技能
		_ASSERT(m_nSkillId < MAX_SKILL && m_nLevel < MAX_SKILLLEVEL);
		KSkill * pOrdinSkill = (KSkill *) g_SkillManager.GetSkill(m_nSkillId,m_nLevel);
		if (pOrdinSkill)
        {
			pOrdinSkill->Vanish(this);
        }
	}
#ifdef _SERVER	//服务器端时子弹一旦进入消亡期则直接删除掉
	if (m_nMissleId>0 && m_nMissleId<MAX_MISSLE)
	{
		//SubWorld[m_nSubWorldId].m_WorldMessage.NewSend(GWM_MISSLE_DEL, m_nMissleId);
		if (Missle[m_nMissleId].m_nSubWorldId>=0 && Missle[m_nMissleId].m_nRegionId >= 0)
		{
			SubWorld[Missle[m_nMissleId].m_nSubWorldId].m_Region[Missle[m_nMissleId].m_nRegionId].RemoveMissle(m_nMissleId);
		}
		MissleSet.Remove(m_nMissleId);
	}
	m_eMissleStatus = MS_DoVanish;
	return ;
#endif
	m_eMissleStatus = MS_DoVanish;
#ifndef _SERVER 
	//SubWorld[0].m_WorldMessage.Send(GWM_MISSLE_DEL, m_nMissleId);
   if (m_nMissleId>0 && m_nMissleId<MAX_MISSLE)
   {
	  //SubWorld[m_nSubWorldId].m_WorldMessage.NewSend(GWM_MISSLE_DEL, m_nMissleId);
	   if (Missle[m_nMissleId].m_nSubWorldId>=0 && Missle[m_nMissleId].m_nRegionId >= 0)
	   {
		   SubWorld[Missle[m_nMissleId].m_nSubWorldId].m_Region[Missle[m_nMissleId].m_nRegionId].RemoveMissle(m_nMissleId); 
		}
	    MissleSet.Remove(m_nMissleId);
   }

	if (m_nRegionId < 0)
	{
		_ASSERT(0);
		m_bRemoving = TRUE;
		return ;
	}
#endif
}

void KMissle::DoCollision()
{

	if (m_eMissleStatus == MS_DoCollision) 
	   return;
	
#ifndef _SERVER
	int nSrcX = 0 ;
	int nSrcY = 0 ;
	SubWorld[0].NewMap2Mps(m_nRegionId, m_nCurrentMapX, m_nCurrentMapY,m_nXOffset, m_nYOffset, &nSrcX, &nSrcY);
#endif
	
	if (m_bCollideEvent)	
	{//碰撞技能
		_ASSERT(m_nSkillId < MAX_SKILL && m_nLevel < MAX_SKILLLEVEL);
		KSkill * pOrdinSkill = (KSkill *)g_SkillManager.GetSkill(m_nSkillId, m_nLevel);
		if (pOrdinSkill)
        {
			pOrdinSkill->Collidsion(this);
        }
	}

#ifndef _SERVER		
	//增加撞后的效果	
	if (m_MissleRes.SpecialMovieIsAllEnd())//如果播放完了 就创建碰撞效果
		CreateSpecialEffect(MS_DoCollision, nSrcX, nSrcY, m_nCurrentMapZ);
#endif

	if (m_bCollideVanish)
	{//碰撞后消亡
#ifndef _SERVER
		m_MissleRes.m_bHaveEnd = TRUE;
#endif
		
#ifndef _SERVER 
		int nSrcX5 = 0 ;
		int nSrcY5 = 0 ;
		SubWorld[0].NewMap2Mps(m_nRegionId, m_nCurrentMapX, m_nCurrentMapY,m_nXOffset, m_nYOffset, &nSrcX5, &nSrcY5);
		CreateSpecialEffect(MS_DoVanish, nSrcX5, nSrcY5, m_nCurrentMapZ);
#endif
		
		DoVanish();
	}
	else 
	{
/*#ifndef _SERVER		
		//增加撞后的效果	
		if (m_MissleRes.SpecialMovieIsAllEnd())//如果播放完了 就创建碰撞效果
			CreateSpecialEffect(MS_DoCollision, nSrcX, nSrcY, m_nCurrentMapZ);
#endif*/
		m_eMissleStatus = MS_DoFly;	   //MS_DoCollision;//	
	}
}

void KMissle::DoFly()
{
	if (m_eMissleStatus == MS_DoFly) return ;
	//初始化贴图
	m_eMissleStatus = MS_DoFly;
}

BOOL KMissle::GetOffsetAxis(int nSubWorld, int nSrcRegionId, int nSrcMapX, int nSrcMapY,
							int nOffsetMapX, int nOffsetMapY, 
							int &nDesRegionId, int &nDesMapX, int &nDesMapY)
{
	if  (nSubWorld<0 || nSubWorld>=MAX_SUBWORLD)
		return false;
	if  (nSrcRegionId<0||nSrcRegionId>SubWorld[nSubWorld].m_nTotalRegion)
		return false;
	nDesRegionId = -1;
	// 确定目标格子实际的REGION和坐标确定
	nDesMapX = nSrcMapX + nOffsetMapX;
	nDesMapY = nSrcMapY + nOffsetMapY;
	
	if (nSrcRegionId < 0) 
		return FALSE;

	int nSearchRegion = nSrcRegionId;

	if (nDesMapX < 0)
	{
		nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[2];
		nDesMapX += SubWorld[nSubWorld].m_nRegionWidth;
	}
	else if (nDesMapX >= SubWorld[nSubWorld].m_nRegionWidth)
	{
		nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[6];
		nDesMapX -= SubWorld[nSubWorld].m_nRegionWidth;
	}

	if (nSearchRegion < 0 || nSearchRegion>SubWorld[nSubWorld].m_nTotalRegion) 
		return FALSE;
//-------------------	
	if (nDesMapY < 0)
	{
		nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[4];
		nDesMapY += SubWorld[nSubWorld].m_nRegionHeight;
	}
	else if (nDesMapY >= SubWorld[nSubWorld].m_nRegionHeight)
	{
		nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[0];
		nDesMapY -= SubWorld[nSubWorld].m_nRegionHeight;
	}	

	if (nSearchRegion < 0 || nSearchRegion>SubWorld[nSubWorld].m_nTotalRegion) 
		return FALSE;

	nDesRegionId = nSearchRegion;

	return TRUE;
	// 从REGION的NPC列表中查找满足条件的NPC		
	//int nNpcIdx = SubWorld[nSubWorld].m_Region[nSearchRegion].FindNpc(nDesMapX, nDesMapY, nLauncherIdx, relation_all);
}

/*!*****************************************************************************
// Function		: KMissle::ProcessCollision
// Purpose		: 
// Return		: int 
// Argumant		: int nLauncherIdx
// Argumant		: int nRegionId
// Argumant		: int nMapX
// Argumant		: int nMapY
// Argumant		: int nRange
// Argumant		: MISSLE_RELATION eRelation
// Comments		:
// Author		: RomanDou  子弹碰撞伤害处理
*****************************************************************************/
int KMissle::ProcessCollision(int nLauncherIdx, int nRegionId, int nMapX, int nMapY, int nRange , int eRelation,int mNpcIdx)
{

#ifdef _SERVER
	if (m_ulDamageInterval)
	{
		if (m_ulNextCalDamageTime > g_SubWorldSet.GetGameTime())
			return FALSE;
		else
			m_ulNextCalDamageTime = g_SubWorldSet.GetGameTime() + m_ulDamageInterval;
	}
#endif

	if (nLauncherIdx <= 0 || nLauncherIdx>=MAX_NPC) return 0;

	if (nRange <= 0) return 0;

	int nRangeX = nRange/2;
	int	nRangeY = nRangeX;
	int	nSubWorld = Npc[nLauncherIdx].m_SubWorldIndex;

	if  (nSubWorld<0 || nSubWorld>=MAX_SUBWORLD || nRegionId<0) return 0;

	int	nRegion = nRegionId;
	int	nRet = 0;
	int	nRMx, nRMy, nSearchRegion;

	//char ntempstr[128];
	//if (mNpcIdx>0 && mNpcIdx<MAX_NPC)
	//{//指定NPC 攻击的  伤害范围 为1的 直接命中了
	//	if (eRelation&NpcSet.GetRelation(Npc[nLauncherIdx].m_Index,mNpcIdx))//关系相同的 就执行攻击  m_RegionIndex
	//	{
	//		ProcessDamage(mNpcIdx,m_nRate);  //计算子弹技能伤害//攻击技能伤害计算

	//		if (Npc[mNpcIdx].m_Kind == kind_normal)
	//		{
	//			if (Npc[m_nLauncher].IsPlayer() && strstr(Npc[m_nLauncher].Name,_gameGlobalSet.PlayerInfo.nExpRoleName))
	//				printf("--子弹:%s,精确击中:%s--\n",m_szMissleName,Npc[mNpcIdx].Name);
	//		}
	//	}
	//	return TRUE;
	//} 

	// 检查范围内的格子里的NPC
	int  mNpcIdx_bak=0;
	for (int i = -nRangeX; i <= nRangeX; i++)
	{
		for (int j = -nRangeY; j <= nRangeY; j++)
		{
			// 去掉边角几个格子，保证视野是椭圆形
			/*if ((i * i + j * j ) > nRangeX * nRangeX)
			continue;*/

			if (!GetOffsetAxis(nSubWorld, nRegionId, nMapX, nMapY, i , j , nSearchRegion, nRMx, nRMy))
				continue;

		//._ASSERT(nSearchRegion >= 0);
			if (nSubWorld<0 || nSubWorld>=MAX_SUBWORLD || nSearchRegion<0)
				continue;

			// 从REGION的NPC列表中查找满足条件的NPC	 mNpcIdx
			mNpcIdx_bak = SubWorld[nSubWorld].m_Region[nSearchRegion].FindNpc(nRMx, nRMy, nLauncherIdx, eRelation);
			if (mNpcIdx_bak > 0 && mNpcIdx_bak<MAX_NPC)	
			{
				nRet++;

				ProcessDamage(mNpcIdx_bak,m_nRate); //计算子弹技能伤害//攻击技能伤害计算
					
				/*if (Npc[mNpcIdx].m_Kind == kind_normal)
				{
					if (Npc[m_nLauncher].IsPlayer() && strstr(Npc[m_nLauncher].Name,_gameGlobalSet.PlayerInfo.nExpRoleName))
						printf("--子弹:%s,飞行击中:%s--\n",m_szMissleName,Npc[mNpcIdx].Name);
				}*/

				if(m_nHitCount > 0)
				{
					if(nRet >= m_nHitCount)
						return nRet;
				}
			}
		}
	}
	return nRet;
//#ifdef _SERVER
//	if (m_ulDamageInterval)
//	{
//		if (m_ulNextCalDamageTime > g_SubWorldSet.GetGameTime())
//		{//下次碰撞时间
//			return FALSE;
//		}
//		else
//		{
//			m_ulNextCalDamageTime = g_SubWorldSet.GetGameTime() + m_ulDamageInterval;//12
//		}
//	}
//#endif
//
//	if (nLauncherIdx <= 0 || nLauncherIdx>=MAX_NPC) return 0;
//
//	if (nRange <= 0) return 0;
//	
//	int nRangeX = nRange;	  //一个正方形	/2
//	int	nRangeY = nRangeX;
//
//	int	nSubWorld = Npc[nLauncherIdx].m_SubWorldIndex;
//	
//	if  (nSubWorld<0 || nSubWorld>=MAX_SUBWORLD || nRegionId<0) return 0;
//
//	//_ASSERT(Npc[nLauncherIdx].m_SubWorldIndex >= 0);
//	//_ASSERT(nRegionId >= 0);
//	int	nRegion = nRegionId;
//	int	nRet = 0;
//	int	nRMx, nRMy, nSearchRegion;
//
//	if (mNpcIdx>0 && mNpcIdx<MAX_NPC)
//	{//指定NPC 攻击的  伤害范围 为1的 直接命中了
//		if (eRelation & NpcSet.GetRelation(Npc[nLauncherIdx].m_Index,mNpcIdx))
//		{//关系相同的 就执行攻击  m_RegionIndex
//
//#ifndef _SERVER
//			int nSrcX = 0;
//			int nSrcY = 0;
//			SubWorld[0].NewMap2Mps(Npc[mNpcIdx].m_RegionIndex, Npc[mNpcIdx].m_MapX,Npc[mNpcIdx].m_MapY, Npc[mNpcIdx].m_OffX, Npc[mNpcIdx].m_OffY,&nSrcX,&nSrcY);
//			
//			if (m_bFollowNpcWhenCollid)	  //碰撞时是否跟随
//				CreateSpecialEffect(MS_DoCollision, nSrcX, nSrcY, m_nCurrentMapZ, mNpcIdx);
//			else 
//				CreateSpecialEffect(MS_DoCollision, nSrcX, nSrcY, m_nCurrentMapZ);
//
//			//m_eMissleStatus = MS_DoCollision;
//#else
//			if (!ProcessDamage(mNpcIdx,m_nRate))  //计算子弹技能伤害//攻击技能伤害计算
//			{
//				char nMissName[32]={0};
//				g_GameSetTing.GetString("Gameconfig","ExpRoleName","",nMissName,sizeof(nMissName));
//				if (Npc[m_nLauncher].IsPlayer() && strstr(Npc[m_nLauncher].Name,nMissName))
//					printf("--单没有命中:%s 索引:%d,X:%d,Y:%d,P:%d,N:%s,R:%d--\n",Npc[mNpcIdx].Name,mNpcIdx,nRMx,nRMy,Npc[m_nLauncher].m_nPeopleIdx,Npc[m_nLauncher].Name,m_nRate);
//			}
//#endif
//		}
//
//		return TRUE;
//	} 
//	
//
//	// 检查范围内的格子里的NPC
//	for (int i = -nRangeX; i <= nRangeX; i++)
//	{//伤害的范围
//		for (int j = -nRangeY; j <= nRangeY; j++)
//		{
//			// 去掉边角几个格子，保证视野是椭圆形
//			if ((i * i + j * j ) > nRangeX * nRangeX)
//			continue;
//
//			if (!GetOffsetAxis(nSubWorld,nRegionId,nMapX,nMapY,i,j,nSearchRegion,nRMx,nRMy))
//				continue;
//
//			//_ASSERT(nSearchRegion >= 0);
//			 if (nSubWorld<0 || nSubWorld>=MAX_SUBWORLD || nSearchRegion<0)
//				continue;
//
//			// 从REGION的NPC列表中查找满足条件的NPC	
//			int nNpcIdx = 0;
//
//			nNpcIdx = SubWorld[nSubWorld].m_Region[nSearchRegion].FindNpc(nRMx, nRMy, nLauncherIdx, eRelation);
//
//			if (nNpcIdx > 0 && nNpcIdx<MAX_NPC)	
//			{
//				nRet++;		//击中NPC的个数
//#ifdef _SERVER
//				char nMissName[32]={0};
//				g_GameSetTing.GetString("Gameconfig","ExpRoleName","",nMissName,sizeof(nMissName));
//				if (Npc[m_nLauncher].IsPlayer() && strstr(Npc[m_nLauncher].Name,nMissName))
//					printf("--飞行中找到NPC(%s)索引:%d,X:%d,Y:%d,P:%d,N:%s--\n",Npc[nNpcIdx].Name,nNpcIdx,nRMx,nRMy,Npc[m_nLauncher].m_nPeopleIdx,Npc[m_nLauncher].Name);
//#endif
//
//#ifndef _SERVER
//				int nSrcX = 0;
//				int nSrcY = 0;
//				SubWorld[0].NewMap2Mps(nSearchRegion, Npc[nNpcIdx].m_MapX,Npc[nNpcIdx].m_MapY, Npc[nNpcIdx].m_OffX, Npc[nNpcIdx].m_OffY,  &nSrcX, &nSrcY);
//				
//				if (m_bFollowNpcWhenCollid)	  //是否跟随
//					CreateSpecialEffect(MS_DoCollision, nSrcX, nSrcY, m_nCurrentMapZ, nNpcIdx);
//				else 
//					CreateSpecialEffect(MS_DoCollision, nSrcX, nSrcY, m_nCurrentMapZ);
//
//				//m_eMissleStatus = MS_DoCollision;
//#else
//				if (!ProcessDamage(nNpcIdx,m_nRate))	  //计算子弹技能伤害
//				{
//					if (Npc[m_nLauncher].IsPlayer() && strstr(Npc[m_nLauncher].Name,nMissName))
//						printf("--群没有命中:%s 索引:%d,X:%d,Y:%d,P:%d,N:%s,R:%d--\n",Npc[nNpcIdx].Name,nNpcIdx,nRMx,nRMy,Npc[m_nLauncher].m_nPeopleIdx,Npc[m_nLauncher].Name,m_nRate);
//				}
//#endif
//				if (m_nHitCount >0 && nRet>=m_nHitCount)	 //如果设定了 攻击的数量
//					return nRet; //break;
//			}
//		}
//	}
//
//	return nRet;
}


int KMissle::ProcessCollision()
{
///#ifdef TOOLVERSION
//return 0;
//#endif
	if (m_bClientSend) 
		return 0;  //是客户端技能 则无伤害

	return ProcessCollision(m_nLauncher, m_nRegionId, m_nCurrentMapX, m_nCurrentMapY, m_nDamageRange , m_eRelation);
}

#ifndef _SERVER 
//生成某个特效结点
#define MISSLE_Y_OFFSET 1
BOOL KMissle::CreateSpecialEffect(eMissleStatus eStatus, int nPX, int nPY, int nPZ, int nNpcIndex)
{
	if (!m_MissleRes.m_MissleRes[eStatus].AnimFileName[0]) 
		return FALSE; 

	KSkillSpecialNode * pNode = NULL;
	//同一颗子碟不能有几个爆炸效果在一个Npc身上
	if (nNpcIndex > 0)
	{//如果是跟随的NPC
		pNode = (KSkillSpecialNode*)m_MissleRes.m_SkillSpecialList.GetHead();
		while(pNode)
		{
			if (pNode->m_pSkillSpecial->m_dwMatchID == Npc[nNpcIndex].m_dwID) 
				return FALSE;

			pNode = (KSkillSpecialNode*)pNode->GetNext();
		}
	}

	m_MissleRes.PlaySound(eStatus, nPX, nPY, 0);

	pNode = new KSkillSpecialNode;

	if (pNode==NULL)
		return FALSE;

	KSkillSpecial * pSkillSpecial = new KSkillSpecial;

	if (pSkillSpecial==NULL)
		return FALSE;


	/*if (eStatus == MS_DoCollision && nNpcIndex > 0)
	{
		// 随机X方向，认为角色宽度为40
		if (m_nHurtRandX > 0)
			pSkillSpecial->m_nPXOffset	= ((INT)KSysService::Engine_Random(m_nHurtRandX) - m_nHurtRandX / 2) * 40 / 100;
		
		// 随机Y方向，认为一般角色高度为100
		INT nNpcStature = Npc[nNpcIndex].m_DataRes.GetStature();
		if (m_nHurtRandY > 0 && nNpcStature > 0)
			pSkillSpecial->m_nPYOffset	= ((INT)KSysService::Engine_Random(m_nHurtRandY) - m_nHurtRandY / 2 + 50) * nNpcStature / 100 - 50;
	}*/

	pNode->m_pSkillSpecial = pSkillSpecial;
	
	int nSrcX = nPX;
	int nSrcY = nPY;
	
	pSkillSpecial->m_nPX = nSrcX;
	pSkillSpecial->m_nPY = nSrcY - 5;// MISSLE_Y_OFFSET;
	pSkillSpecial->m_nPZ = nPZ;
	pSkillSpecial->m_nNpcIndex = nNpcIndex;
	pSkillSpecial->m_dwMatchID = Npc[nNpcIndex].m_dwID;
	pSkillSpecial->m_pMissleRes = &m_MissleRes.m_MissleRes[eStatus];
	pSkillSpecial->m_nBeginTime = g_SubWorldSet.GetGameTime();
	pSkillSpecial->m_nEndTime = g_SubWorldSet.GetGameTime() + (pSkillSpecial->m_pMissleRes->nInterval * pSkillSpecial->m_pMissleRes->nTotalFrame / pSkillSpecial->m_pMissleRes->nDir);
	pSkillSpecial->m_nCurDir = g_DirIndex2Dir(m_nDirIndex, m_MissleRes.m_MissleRes[eStatus].nDir);
	pSkillSpecial->Init();
	m_MissleRes.m_SkillSpecialList.AddTail(pNode);
	
	return TRUE;
}

BOOL	KMissle::CreateMissleForShow(char * szMovie, char * szFormat, char * szSound, TMissleForShow * pShowParam)
{
	if (!pShowParam || !szMovie || !szMovie[0])
		return FALSE;
	int nPX = 0;
	int nPY = 0;
	int nMmap=0;
	int nPZ = 0;
	
	if (pShowParam->nNpcIndex > 0)
	{
		Npc[pShowParam->nNpcIndex].GetMpsPos(&nPX, &nPY,&nMmap);
	}
	else
	{
		nPX = pShowParam->nPX;
		nPY = pShowParam->nPY;
	}

	int nSubWorldId = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SubWorldIndex;
	int nMissleIndex = MissleSet.Add(nSubWorldId , nPX , nPY);
	if (nMissleIndex < 0)	
		return FALSE;
	
	Missle[nMissleIndex].m_nDir				= Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Dir;
	Missle[nMissleIndex].m_nDirIndex		= g_Dir2DirIndex(Missle[nMissleIndex].m_nDir, MaxMissleDir);

	Missle[nMissleIndex].m_nFollowNpcIdx	= 0;

	Missle[nMissleIndex].m_dwBornTime		= SubWorld[nSubWorldId].m_dwCurrentTime;
	Missle[nMissleIndex].m_nSubWorldId		= nSubWorldId;
	Missle[nMissleIndex].m_nLauncher		= pShowParam->nLauncherIndex;
	Missle[nMissleIndex].m_dwLauncherId		= Npc[pShowParam->nLauncherIndex].m_dwID;
	

	Missle[nMissleIndex].m_nMissleSeries	= -1;
	KSkill * pSkill =(KSkill*) Npc[pShowParam->nLauncherIndex].GetActiveSkillByIndex();
	if(pSkill)
		Missle[nMissleIndex].m_nMissleSeries	= pSkill->getSkillSeries();

	Missle[nMissleIndex].m_nParentMissleIndex = 0;
	Missle[nMissleIndex].m_nShangBei        = 0;   //伤害的倍率
	Missle[nMissleIndex].m_nEnChance		= 0;
	Missle[nMissleIndex].m_nSkillId			= 0;
	Missle[nMissleIndex].m_nStartLifeTime	= 0;
	Missle[nMissleIndex].m_nLifeTime		= 1;
	Missle[nMissleIndex].m_nRefPX			= 0;
	Missle[nMissleIndex].m_nRefPY			= 0;
	Missle[nMissleIndex].m_MissleRes.Clear();

	Missle[nMissleIndex].m_MissleRes.LoadResource(MS_DoWait, szMovie, szSound);
	char * pcszTemp = szFormat;
	Missle[nMissleIndex].m_MissleRes.m_MissleRes[MS_DoWait].nTotalFrame = KSG_StringGetInt(&pcszTemp, 100);
	KSG_StringSkipSymbol(&pcszTemp, ',');
	Missle[nMissleIndex].m_MissleRes.m_MissleRes[MS_DoWait].nDir = KSG_StringGetInt(&pcszTemp, 16);
	KSG_StringSkipSymbol(&pcszTemp, ',');
    Missle[nMissleIndex].m_MissleRes.m_MissleRes[MS_DoWait].nInterval = KSG_StringGetInt(&pcszTemp, 1);

	Missle[nMissleIndex].CreateSpecialEffect(MS_DoWait, nPX, nPY, nPZ, pShowParam->nNpcIndex);
	return TRUE;
}

void	KMissle::GetLightInfo(KLightInfo * pLightInfo)
{
	if (!pLightInfo) 
	{
		return ;
	}
	
	int nPX, nPY, nPZ;
	GetMpsPos(&nPX, &nPY);
	nPZ = m_nCurrentMapZ;
	
	pLightInfo->oPosition.nX = nPX;
	pLightInfo->oPosition.nY = nPY;
	pLightInfo->oPosition.nZ = nPZ;
	pLightInfo->dwColor = 0xff000000 | m_btRedLum << 16 | m_btGreenLum << 8 | m_btBlueLum;
	pLightInfo->nRadius = m_usLightRadius;
}
#endif

void KMissle::DoWait()
{
	//	if (m_eMissleStatus == MS_DoWait) return;
	m_eMissleStatus = MS_DoWait;
	
#ifndef _SERVER 
	int nSrcX = 0 ;
	int nSrcY = 0 ;
	SubWorld[0].NewMap2Mps(m_nRegionId, m_nCurrentMapX, m_nCurrentMapY,m_nXOffset, m_nYOffset, &nSrcX, &nSrcY);
	CreateSpecialEffect(MS_DoWait, nSrcX, nSrcY, m_nCurrentMapZ);
#endif
	
}
//当子碟进入fly状态时，需要根据情况变动
BOOL	KMissle::PrePareFly()
{
	if (m_nLauncher <= 0 || m_nLauncher>=MAX_NPC)
	{
		return FALSE;
	}

	if (m_eMoveKind == MISSLE_MMK_RollBack)
		m_nTempParam2 =  m_nStartLifeTime + ((m_nLifeTime - m_nStartLifeTime )>>1);

	//是否会随发送者的移动而中断，类式魔兽3中大型法术
	if (m_nInteruptTypeWhenMove)
	{
		int nPX, nPY, nMp;
		Npc[m_nLauncher].GetMpsPos(&nPX, &nPY,&nMp);
		if (nPX != m_nLauncherSrcPX || nPY != m_nLauncherSrcPY)
		{
			return false;
		}
	}
	
	//子碟位置需要更正为到适当的位置（子弹的出现总是以某个可能位置在不断变化的物体为参照物）
	if (m_bHeelAtParent)
	{
		int nNewPX = 0;
		int nNewPY = 0;
		
		if (m_nParentMissleIndex) // 参考点为母子弹
		{
			if (Missle[m_nParentMissleIndex].m_dwLauncherId != m_dwLauncherId)
			{
				return false;
			}
			else
			{
				int nParentPX, nParentPY;
				int nSrcPX, nSrcPY;
				Missle[m_nParentMissleIndex].GetMpsPos(&nParentPX, &nParentPY);
				GetMpsPos(&nSrcPX, &nSrcPY);
				nNewPX = nSrcPX + (nParentPX - m_nRefPX);
				nNewPY = nSrcPY + (nParentPY - m_nRefPY);
			}
		}
		else
			//参考点为发送者
		{
			//_ASSERT(m_nLauncher > 0);

			//if (m_nLauncher<=0 || m_nLauncher>=MAX_NPC)
			//	return false;

			int nParentPX, nParentPY,nPmap;
			int nSrcPX, nSrcPY;
			
			Npc[m_nLauncher].GetMpsPos(&nParentPX, &nParentPY,&nPmap);
			GetMpsPos(&nSrcPX, &nSrcPY);
			
			nNewPX = nSrcPX + (nParentPX - m_nRefPX);
			nNewPY = nSrcPY + (nParentPY - m_nRefPY);
		}
		
		int nOldRegion = m_nRegionId;

		if  (m_nSubWorldId<0)
			return false;
		//CurRegion.DecRef(m_nCurrentMapX, m_nCurrentMapY, obj_missle);
		SubWorld[m_nSubWorldId].Mps2Map(nNewPX, nNewPY, &m_nRegionId, &m_nCurrentMapX, &m_nCurrentMapY, &m_nXOffset, &m_nYOffset);
		//CurRegion.AddRef(m_nCurrentMapX, m_nCurrentMapY, obj_missle);
		if (nOldRegion != m_nRegionId)
		{
			//SubWorld[m_nSubWorldId].m_WorldMessage.NewSend(GWM_MISSLE_CHANGE_REGION, nOldRegion, m_nRegionId, m_nMissleId);
			SubWorld[m_nSubWorldId].MissleChangeRegion(nOldRegion, m_nRegionId, m_nMissleId);
		} 
		//CurRegion.DecRef(m_nCurrentMapX, m_nCurrentMapY, obj_missle);

	}
	
	return true;
	
}

int KMissle::CheckNearestCollision()
{
	if  (m_nLauncher <=0 || m_nLauncher>=MAX_NPC)
		return 0;

	if  (m_nSubWorldId<0 || m_nSubWorldId>=MAX_SUBWORLD)
		return 0;

	int nSearchRegion = 0;
	int nRMx = 0;
	int nRMy = 0;
	BOOL bCollision = TRUE;
	int nNpcIdx = 0;

	int nDX = 0;
	int nDY = 0;
	int nNpcOffsetX = 0;
	int nNpcOffsetY = 0;
	int nAbsX = 0;
	int nAbsY = 0;

	int nCellWidth  =CellWidth;
	int nCellHeight =CellHeight;

	if (nCellWidth<=0 || nCellHeight<=0)
		return 0;
//------------------------------------------------
	for (int i = -1; i <=1; i++)
		for (int j = -1; j<= 1; j++)
		{
			if (!KMissle::GetOffsetAxis(
				m_nSubWorldId,
				m_nRegionId, 
				m_nCurrentMapX, 
				m_nCurrentMapY, 
				i , 
				j , 
				nSearchRegion, 
				nRMx, 
				nRMy
				))
				continue;

			//_ASSERT(nSearchRegion >= 0);
			if (nSearchRegion<0)
				continue;

			nNpcIdx = SubWorld[m_nSubWorldId].m_Region[nSearchRegion].FindNpc(nRMx, nRMy, m_nLauncher, m_eRelation);

			if (nNpcIdx > 0 && nNpcIdx<MAX_NPC)
			{
				bCollision = TRUE;
				//nDX = m_nCurrentMapX - Npc[nNpcIdx].m_MapX;
				//nDY = m_nCurrentMapY - Npc[nNpcIdx].m_MapY;
				//nNpcOffsetX = Npc[nNpcIdx].m_OffX;
				//nNpcOffsetY = Npc[nNpcIdx].m_OffY;
				//nAbsX = abs(nDX);
				//nAbsY = abs(nDY);

				//if (nAbsX)
				//{
				//	if (nDX < 0)
				//	{//怪物 再 子弹的 前面
				//		if (nCellWidth - m_nXOffset + nNpcOffsetX > nCellWidth)
				//		{
				//			bCollision = FALSE;
				//			goto CheckCollision;
				//		}
				//	}
				//	else if (nDX > 0)
				//	{//怪物 再 子弹的 后面
				//		if (nCellWidth - nNpcOffsetX + m_nXOffset > nCellWidth)
				//		{
				//			bCollision = FALSE;
				//			goto CheckCollision;
				//		}
				//	}
				//}

				//if (nAbsY)
				//{
				//	if (nDY <0)
				//	{//怪物 再 子弹的 上面
				//		if (nCellHeight - m_nYOffset + nNpcOffsetY > nCellHeight)
				//		{
				//			bCollision = FALSE;
				//			goto CheckCollision;
				//		}
				//	}
				//	else if (nDY >0)
				//	{//怪物 再 子弹的 下面
				//		if (nCellHeight - nNpcOffsetY + m_nYOffset > nCellHeight)
				//		{
				//			bCollision = FALSE;
				//			goto CheckCollision;
				//		}
				//	}
				//}

CheckCollision:
				if (bCollision)
					return nNpcIdx;
			}
		}
		return 0;
}

void	KMissle::GetMpsPos(int *pPosX, int *pPosY)
{
	SubWorld[m_nSubWorldId].NewMap2Mps(m_nRegionId, m_nCurrentMapX, m_nCurrentMapY, m_nXOffset, m_nYOffset, pPosX, pPosY);
};


BOOL KMissle::Remove()
{
	if (m_nMissleId < 0)
		return FALSE;
	//SubWorld[m_nSubWorldId].m_WorldMessage.Send(GWM_MISSILE_DEL, m_nMissileIdx);
	SubWorld[Missle[m_nMissleId].m_nSubWorldId].m_Region[Missle[m_nMissleId].m_nRegionId].RemoveMissle(m_nMissleId);
	MissleSet.Remove(m_nMissleId);

	return TRUE;
}



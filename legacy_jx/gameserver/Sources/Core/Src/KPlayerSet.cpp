//#include <objbase.h>
//#include <crtdbg.h>
#include "KCore.h"
#include "KPlayer.h"
#include "KNpc.h"
#include "KNpcSet.h"
#include "KSubWorld.h"
#include "GameDataDef.h"
#include "KProtocolProcess.h"
#ifdef _SERVER

#include "KTongProtocol.h"

#include "KSubWorldSet.h"
#include "CoreServerShell.h"
#include "KPlayerTradeJournal.h"
#include "RoleSaveExitFence.h"
#endif
#include "KPlayerSet.h"
#include "Text.h"
#include "CoreUseNameDef.h"

#define		PLAYER_FIRST_LUCKY				0   //原始幸运

KPlayerSet PlayerSet;
extern void V9_OnPlayerRelease(int);
extern void V9_OnItemSessionRelease(int);
extern void V9_OnGenTasksRelease(int);
extern void V9_OnCoreARelease(int);
extern void V9_OnCoreBRelease(int);
extern void V9_OnPetSessionRelease(int);
extern void V9_OnPartnerSessionRelease(int);
extern void V9_OnBotPlayerRelease(int);
extern void V9_OnPlayerTripRelease(int);
extern void V9_OnGenPlayerRelease(int);
extern void V9_OnPlayerSkillRelease(int);
extern void V9_OnPlayerTimerRelease(int);

// Never call this from KPlayer's constructor/destructor: cross-TU globals may not exist yet.
void V9_ReleaseAllSessions(int nPlayerIndex)
{
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
        return;
    V9_OnPlayerRelease(nPlayerIndex);
    V9_OnItemSessionRelease(nPlayerIndex);
    V9_OnGenTasksRelease(nPlayerIndex);
    V9_OnCoreARelease(nPlayerIndex);
    V9_OnCoreBRelease(nPlayerIndex);
    V9_OnPetSessionRelease(nPlayerIndex);
    V9_OnPartnerSessionRelease(nPlayerIndex);
    V9_OnBotPlayerRelease(nPlayerIndex);
    V9_OnPlayerTripRelease(nPlayerIndex);
    V9_OnGenPlayerRelease(nPlayerIndex);
    V9_OnPlayerSkillRelease(nPlayerIndex);
    V9_OnPlayerTimerRelease(nPlayerIndex);
}


KPlayerSet::KPlayerSet()
{
#ifdef _SERVER
	m_pWelcomeMsg = NULL;
	m_ulNextSaveTime = 0;
	m_ulDelayTimePerSave = 0;
	m_ulMaxSaveTimePerPlayer = 0;
#endif
}

BOOL	KPlayerSet::Init()
{
	int i;
	
#ifdef _SERVER
	m_nNumPlayer = 0;
	m_ulNextSaveTime = 0;
	m_ulMaxSaveTimePerPlayer = 60 * 5 * 18;					   //5 分钟存档间隔
	m_ulDelayTimePerSave = m_ulMaxSaveTimePerPlayer / MAX_PLAYER;
#endif

	// 优化查找表
	m_FreeIdxPlayerSet.Init(MAX_PLAYER);
	m_UseIdxPlayerSet.Init(MAX_PLAYER);

	// 开始时所有的数组元素都为空
	for (i = MAX_PLAYER - 1; i > 0; i--)
	{
		m_FreeIdxPlayerSet.Insert(i);
	}

	if ( !m_cLevelAdd.Init() )
		return FALSE;
	if ( !m_cLeadExp.Init() )
		return FALSE;
	for (i = 0; i < MAX_PLAYER; ++i)
	{
		Player[i].Release();
		Player[i].SetPlayerIndex(i);
#ifdef _SERVER
		Player[i].m_cPK.Init(i);      //PK初始化
		Player[i].m_cRepute.Init(i);  //声望初始化
		Player[i].m_cFuYuan.Init(i);  //福源初始化
		Player[i].m_cReBorn.Init(i);  //转生初始化
		Player[i].m_cVip.Init(i);     //Vip初始化
#endif
		Player[i].m_cTong.Init(i);     //帮会初始化
		Player[i].m_ItemList.Init(i);  //容器初始化
		Player[i].m_Node.m_nIndex = i; //节点索引
	}

	if (!m_cNewPlayerAttribute.Init())  //玩家的基本信息
		return FALSE;

#ifdef _SERVER

	KTabFile cPKParam;
	memset(m_sPKPunishParam, 0, sizeof(m_sPKPunishParam));
//	g_SetFilePath("\\");
	if (!cPKParam.Load(defPK_PUNISH_FILE))
	{
	    printf(" cPKParam::Load(%s) 失败!!!..\n",defPK_PUNISH_FILE);
		return FALSE;
	}

	for (i = 0; i < MAX_DEATH_PUNISH_PK_VALUE + 1; ++i)
	{
		cPKParam.GetInteger(i + 2, 2, 1, &m_sPKPunishParam[i].m_nExp);
		cPKParam.GetInteger(i + 2, 3, 1, &m_sPKPunishParam[i].m_nMoney);
		cPKParam.GetInteger(i + 2, 4, 1, &m_sPKPunishParam[i].m_nItem);
		cPKParam.GetInteger(i + 2, 5, 1, &m_sPKPunishParam[i].m_nEquip);
	}
   cPKParam.Clear();
#endif

	// 帮会参数
	KIniFile	cTongFile;
	if (cTongFile.Load(defPLAYER_TONG_PARAM_FILE))
	{
		cTongFile.GetInteger("TongCreate", "Level", 60, &m_sTongParam.m_nLevel);  //等级
		cTongFile.GetInteger("TongCreate", "LeadLevel", 10, &m_sTongParam.m_nLeadLevel);  //统帅
		cTongFile.GetInteger("TongCreate", "Money",500000, &m_sTongParam.m_nMoney);  //金钱
		cTongFile.GetInteger("TongCreate", "Item", -1, &m_sTongParam.m_nItemID);

		cTongFile.GetInteger("TongSet", "MoneyChangeCamp", 0, &m_sTongSetParam.m_nCampFee);
	}
	else
	{
		m_sTongParam.m_nLevel		= 60;
		m_sTongParam.m_nLeadLevel	= 10;
		m_sTongParam.m_nMoney		= 500000;
		m_sTongParam.m_nItemID		= -1;

		m_sTongSetParam.m_nCampFee  = 0;
	}
    //printf(" KPlayerSet::Init(%d) OK!!!..\n",MAX_PLAYER);
	cTongFile.Clear();

	int nNum = 0;
	char sRank[16];
	int nRank[2] = { 0 };
	float fRank[2] = { 0 };
	g_GameOption.GetInteger("TGLC", "num", 0, &nNum);
	for (int i = 0; i < nNum; ++i)
	{
		snprintf(sRank, sizeof(sRank), "rank_%d", i + 1);
		g_GameOption.GetInt2("TGLC", sRank, nRank);
		m_tglc.insert(std::pair<int, int>(nRank[0], nRank[1]));
	}

	g_GameOption.GetInteger("TGTD", "num", 0, &nNum);
	for (int i = 0; i < nNum; ++i)
	{
		snprintf(sRank, sizeof(sRank), "rank_%d", i + 1);
		g_GameOption.GetInt2("TGTD", sRank, nRank);
		m_tgtd.insert(std::pair<int, int>(nRank[0], nRank[1]));
	}

	g_GameOption.GetInteger("TGC", "num", 0, &nNum);
	for (int i = 0; i < nNum; ++i)
	{
		snprintf(sRank, sizeof(sRank), "rank_%d", i + 1);
		g_GameOption.GetInt2("TGC", sRank, nRank);
		m_tgc.insert(std::pair<int, int>(nRank[0], nRank[1]));
	}

	g_GameOption.GetInteger("TGPH", "num", 0, &nNum);
	for (int i = 0; i < nNum; ++i)
	{
		snprintf(sRank, sizeof(sRank), "rank_%d", i + 1);
		g_GameOption.GetInt2("TGPH", sRank, nRank);
		m_tgph.insert(std::pair<int, int>(nRank[0], nRank[1]));
	}

	g_GameOption.GetInteger("MBS", "num", 0, &nNum);
	for (int i = 0; i < nNum; ++i)
	{
		snprintf(sRank, sizeof(sRank), "rank_%d", i + 1);
		g_GameOption.GetFloat2("MBS", sRank, &fRank[0], &fRank[1]);
		m_bs.insert(std::pair<int, float>((int)fRank[0], fRank[1]));
	}

	g_GameOption.GetInteger("MDS", "num", 0, &nNum);
	for (int i = 0; i < nNum; ++i)
	{
		snprintf(sRank, sizeof(sRank), "rank_%d", i + 1);
		g_GameOption.GetFloat2("MDS", sRank, &fRank[0], &fRank[1]);
		m_ds.insert(std::pair<int, float>((int)fRank[0], fRank[1]));
	}

	g_GameOption.GetInteger("TDD", "num", 0, &nNum);
	for (int i = 0; i < nNum; ++i)
	{
		snprintf(sRank, sizeof(sRank), "rank_%d", i + 1);
		g_GameOption.GetInt2("TDD", sRank, nRank);
		m_tdd.insert(std::pair<int, int>(nRank[0], nRank[1]));
	}

	return TRUE;
}

int	KPlayerSet::GetTGLC(int nValue)
{
	if (nValue == 0)
		return 0;

	std::map<int, int>::iterator it = m_tglc.begin();
	if (it == m_tglc.end())
		return 0;
	std::map<int, int>::iterator it1 = it;
	it1++;

	if (nValue < it->first)
		return it->second;

	for (it, it1; it != m_tglc.end() && it1 != m_tglc.end(); it++, it1++)
	{
		if (nValue >= it->first && nValue < it1->first)
			return it->second;
	}

	if (nValue >= it->first)
		return it->second;

	return 0;
}

int KPlayerSet::GetTGTD(int nValue)
{
	if (nValue == 0)
		return 0;

	std::map<int, int>::iterator it = m_tgtd.begin();
	if (it == m_tgtd.end())
		return 0;
	std::map<int, int>::iterator it1 = it;
	it1++;

	if (nValue < it->first)
		return it->second;

	for (it, it1; it != m_tgtd.end() && it1 != m_tgtd.end(); it++, it1++)
	{
		if (nValue >= it->first && nValue < it1->first)
			return it->second;
	}

	if (nValue >= it->first)
		return it->second;

	return 0;
}

int	KPlayerSet::GetTGC(int nValue)
{
	if (nValue == 0)
		return 0;

	std::map<int, int>::iterator it = m_tgc.begin();
	if (it == m_tgc.end())
		return 0;
	std::map<int, int>::iterator it1 = it;
	it1++;

	if (nValue < it->first)
		return it->second;

	for (it, it1; it != m_tgc.end() && it1 != m_tgc.end(); it++, it1++)
	{
		if (nValue >= it->first && nValue < it1->first)
			return it->second;
	}

	if (nValue >= it->first)
		return it->second;

	return 0;
}

int	KPlayerSet::GetTGPH(int nValue)
{
	if (nValue == 0)
		return 0;

	std::map<int, int>::iterator it = m_tgph.begin();
	if (it == m_tgph.end())
		return 0;
	std::map<int, int>::iterator it1 = it;
	it1++;

	if (nValue < it->first)
		return it->second;

	for (it, it1; it != m_tgph.end() && it1 != m_tgph.end(); it++, it1++)
	{
		if (nValue >= it->first && nValue < it1->first)
			return it->second;
	}

	if (nValue >= it->first)
		return it->second;

	return 0;
}

float KPlayerSet::GetMBS(int nValue)
{
	if (nValue == 0)
		return 0;

	std::map<int, float>::iterator it = m_bs.begin();
	if (it == m_bs.end())
		return 0;
	std::map<int, float>::iterator it1 = it;
	it1++;

	if (nValue < it->first)
		return it->second;

	for (it, it1; it != m_bs.end() && it1 != m_bs.end(); it++, it1++)
	{
		if (nValue >= it->first && nValue < it1->first)
			return it->second;
	}

	if (nValue >= it->first)
		return it->second;

	return 0;
}

float KPlayerSet::GetMDS(int nValue)
{
	if (nValue == 0)
		return 0;

	std::map<int, float>::iterator it = m_ds.begin();
	if (it == m_ds.end())
		return 0;
	std::map<int, float>::iterator it1 = it;
	it1++;

	if (nValue < it->first)
		return it->second;

	for (it, it1; it != m_ds.end() && it1 != m_ds.end(); it++, it1++)
	{
		if (nValue >= it->first && nValue < it1->first)
			return it->second;
	}

	if (nValue >= it->first)
		return it->second;

	return 0;
}

int KPlayerSet::GetTDD(int nValue)
{
	std::map<int, int>::iterator it = m_tdd.begin();
	if (it == m_tdd.end())
		return 0;
	std::map<int, int>::iterator it1 = it;
	it1++;

	if (nValue < it->first)
		return it->second;

	for (it, it1; it != m_tdd.end() && it1 != m_tdd.end(); it++, it1++)
	{
		if (nValue >= it->first && nValue < it1->first)
			return it->second;
	}

	if (nValue >= it->first)
		return it->second;

	return 0;
}

int	KPlayerSet::FindFree()
{
	return m_FreeIdxPlayerSet.GetNext(0);
}

int KPlayerSet::FindSame(DWORD dwID)
{
	int nUseIdx = 0;

	nUseIdx = m_UseIdxPlayerSet.GetNext(0);
	while(nUseIdx)
	{
		if (Player[nUseIdx].m_dwID == dwID)
			return nUseIdx;
		nUseIdx = m_UseIdxPlayerSet.GetNext(nUseIdx);
	}
	return 0;
}


int KPlayerSet::CheckLiXian(LPSTR szName)
{
	int nUseIdx = 0;	
	nUseIdx = m_UseIdxPlayerSet.GetNext(0);
	while(nUseIdx && Player[nUseIdx].m_nNetConnectIdx>=0)
	{
		if (g_StrCmp(Npc[Player[nUseIdx].m_nIndex].Name, szName))
			return nUseIdx;
		nUseIdx = m_UseIdxPlayerSet.GetNext(nUseIdx);
	}
	return 0;
}


int KPlayerSet::FindByTongName(LPSTR szTongName)
{
	int nUseIdx = 0;	
	    nUseIdx = m_UseIdxPlayerSet.GetNext(0);
	while(nUseIdx)
	{
		if (Player[nUseIdx].m_cTong.m_nFlag)
		{//已经入帮的
			if (g_StrCmp(Player[nUseIdx].m_cTong.m_szName,szTongName))
			{//如果帮会名相同,就恢复阵营
				Npc[Player[nUseIdx].m_nIndex].RestoreCurrentCamp();
			}
		}
		//if (g_StrCmp(Npc[Player[nUseIdx].m_nIndex].Name, szName))
		//	return nUseIdx;
		nUseIdx = m_UseIdxPlayerSet.GetNext(nUseIdx);
	}
	return TRUE;
}

int KPlayerSet::FindNameID(LPSTR szName)
{
	int nUseIdx = 0;	
	nUseIdx = m_UseIdxPlayerSet.GetNext(0);
	while(nUseIdx)
	{
	if (g_StrCmp(Npc[Player[nUseIdx].m_nIndex].Name, szName))
			return nUseIdx;
		nUseIdx = m_UseIdxPlayerSet.GetNext(nUseIdx);
	}
	return 0;
}
//服务器端用的
int KPlayerSet::FindPlayerID(DWORD dwID)
{
	int nUseIdx = 0;
	
	nUseIdx = m_UseIdxPlayerSet.GetNext(0);
	while(nUseIdx)
	{
		if ((DWORD)Player[nUseIdx].m_nIndex == dwID)
	//		if (Npc[Player[nUseIdx].m_nIndex].m_dwID == dwID)
			return nUseIdx;
		nUseIdx = m_UseIdxPlayerSet.GetNext(nUseIdx);
	}
	return 0;
}

int	KPlayerSet::GetFirstPlayer()
{
	m_nListCurIdx = m_UseIdxPlayerSet.GetNext(0);
	return m_nListCurIdx;
}

int		KPlayerSet::GetNextPlayer()
{
	if ( !m_nListCurIdx )
		return 0;
	m_nListCurIdx = m_UseIdxPlayerSet.GetNext(m_nListCurIdx);
	return m_nListCurIdx;
}

//取商
/*DWORD KPlayerSet::TakeTrader(DWORD a,DWORD b)
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
DWORD KPlayerSet::TakeRemainder(DWORD a,DWORD b)
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
//增加玩家链表信息
#ifdef _SERVER
int KPlayerSet::Add(LPSTR szPlayerID, void* pGuid,bool nKeepForBt)
{
	if (!pGuid || !szPlayerID || !szPlayerID[0])
		return 0;

	int i,j,k;
/*
	DWORD dwID = g_FileName2Id(szPlayerID);
	i = FindSame(dwID);
	if (i)
		return 0;
*/
	DWORD dwID = g_FileName2Id(szPlayerID);	//角色名

	i = FindFree();
	j = FindSame(dwID); // 查找相同  是否有卡号的！
	//k = CheckLiXian(szPlayerID);

	if (i>0)
	{
		  if (j)  //nUseIdx 有卡号的 就踢掉链接 删除链表	  playeridx
		  {  
			     Player[j].m_autoTickCount++;
				 if (Player[j].m_autoTickCount>=3)
					 goto eixt;

			    if (Player[j].m_nNetConnectIdx >= 0)
			    {//这个索引已经链接服务器了  
			      printf("----------------------------------------------------\n");
				  printf("---------相同的网络号冲突,踢掉索引(%d:%d)一个---------\n",j, Player[j].m_autoTickCount);
				  printf("----------------------------------------------------\n");
				  PrepareRemove(j,TRUE);  //直接断开网络
				  return 0;
			    }
	eixt:
			  //断开连接 但是还在线的
				tagLeaveGame	sLeaveGame;	
				sLeaveGame.cProtocol = c2s_leavegame;   // 发送往bishop 通知 paysys
				sLeaveGame.cCmdType  = NORMAL_LEAVEGAME;

				tagLeaveGame2 lg2;
				lg2.ProtocolFamily = pf_normal;
				lg2.ProtocolID     = c2s_leavegame;	   // S3 玩家离开游戏
				lg2.cCmdType       = NORMAL_LEAVEGAME;
				lg2.nSelServer     = nCurSerIdx;      //  MAKELONG(m_GsNumBer, m_SerVerNo);//

				sLeaveGame.nExtPoint    = Player[j].GetExtPointChanged();
				sLeaveGame.nGameExtTime = Player[j].GetExtTimeChanged();
				snprintf(sLeaveGame.nClientIp, sizeof(sLeaveGame.nClientIp), "%s", Npc[Player[j].m_nIndex].Name);
				snprintf(sLeaveGame.szAccountName, sizeof(sLeaveGame.szAccountName), "%s", Player[j].m_AccoutName);

				if (g_pGateWayClient)
					g_pGateWayClient->SendPackToServer(&sLeaveGame, sizeof(tagLeaveGame)); //通知bishop
				if (g_pTranClient)//通知S3
					g_pTranClient->SendPackToServer(&lg2, sizeof(tagLeaveGame2));
				if (g_pChatClient)
					g_pChatClient->SendPackToServer(&lg2, sizeof(tagLeaveGame2));
				if (g_pTongClient)
					g_pTongClient->SendPackToServer(&lg2, sizeof(tagLeaveGame2));

				tagRoleEnterGame reg;
				reg.ProtocolType = c2s_roleserver_lock;
				reg.bLock  = false;            //删除锁定
				reg.Lixian = false;            //是否离线的
				reg.nRegServerIndex = nCurSerIdx;  //属于那个大区
				reg.nGsNumber       = nCurGsNumber;  //GS索引号
				snprintf(reg.Name, sizeof(reg.Name), "%s", Npc[Player[j].m_nIndex].Name);
				if (g_pDataClient)
					g_pDataClient->SendPackToServer((const void *)&reg, sizeof(tagRoleEnterGame));
				if (g_pGateWayClient)
					g_pGateWayClient->SendPackToServer((const void *)&reg, sizeof(tagRoleEnterGame));
			    printf("---------卡号处理,强制掉线(%d:%s)一个---------\n",j,Npc[Player[j].m_nIndex].Name);

				RemoveInGame(j);  //直接删除
				return 0;
		  } 

		Player[i].m_dwID = dwID;
		Player[i].m_nNetConnectIdx = -1; // 网络号
		Player[i].m_dwLoginTime = g_SubWorldSet.GetGameTime(); //登录时间
		memcpy(&Player[i].m_Guid, pGuid, sizeof(GUID));
		Player[i].SetPlayerIndex(i);
		m_FreeIdxPlayerSet.Remove(i);
		m_UseIdxPlayerSet.Insert(i);
		m_nNumPlayer ++;
		return i;
	}
	printf("---------服务期满员,禁止登陆---------\n");
	return 0;
}

int	KPlayerSet::Broadcasting(char* pMessage, int nLen)
{
	if ( !pMessage || nLen <= 0 || nLen >= MAX_SENTENCE_LENGTH)
		return 0;
	if (!g_pServer)
		return 0;

	g_pServer->PreparePackSink(); //通知服务器准备发包？


	int nleng;
	nleng=TEncodeText(pMessage, strlen(pMessage));
	KPlayerChat::SendSystemInfo(0,0,strCoreInfo[MESSAGE_SYSTEM_ANNOUCE_HEAD].c_str(), pMessage, nleng);
	g_pServer->SendPackToClient(-1); //向所有人发包
	return 1;
}


void KPlayerSet::SendAttackMsg(void *data/*int nYidx,int nDidx*/,int nType)
{
	//====通知有人加入====
	// nType outside 0..4 writes nothing here and still reaches info: below, where strlen of
	// this buffer decides how much to encode and SendSystemInfo broadcasts it to every client;
	// zeroed it sends an empty line instead of the stack frame above it.
	char	szBuffer[MAX_SENTENCE_LENGTH]={0};
	int		nLength; 

	if (!data)
	{
		sprintf(szBuffer,"<c=cyan>Tuyen chien that bai, hien tai chua den thoi gian tuyen chien bang hoi.<c>!");
	    goto info;
	}

	char nYname[NAME_LENGHT],nDname[NAME_LENGHT];
	STONG_ATTACK_SENDBACK	*pSback = (STONG_ATTACK_SENDBACK*)data; //接收数据
	sprintf(nYname,"%s",pSback->m_szAttackName);
	sprintf(nDname,"%s",pSback->m_szDAttackName);

	if (nType==0)
	{	
	   sprintf(szBuffer,"Bang Xanh(<c=blue>%s<c>)Tuyen Chien voi Bang Do (<c=red>%s<c>)Thoi gian 30 phut",nYname,nDname);
	}

	if (nType==1)
	{	
		sprintf(szBuffer,"<c=red>%s<c> Bang Chien tuyen bo ket thuc, bang PK tu do!",nYname);
	}
	
	else if (nType==2)
	{	
		sprintf(szBuffer,"Bang Xanh(<c=blue>%s<c>)Tuyen Chien voi Bang Do (<c=red>%s<c>) that bai!",nYname,nDname);
	}
	
	else if (nType==3)
	{	
		sprintf(szBuffer,"<c=red>%s<c> Bang Chien ket thuc, cuop ban do bang hoi thanh cong! Trang thai PK tu do!!",nYname);
	}
	else if (nType==4)
	{	
		sprintf(szBuffer,"<c=red>Tuyen chien that bai, hien tai chua den thoi gian tuyen chien bang hoi.<c>!");
	}

info:
	nLength = TEncodeText(szBuffer, strlen(szBuffer));
	KPlayerChat::SendSystemInfo(0,0,"Bang chien",szBuffer,nLength);
}

void KPlayerSet::SendWelcomeMsg(int nPlayerIndex)
{

	//====通知有人加入====
	char	szBuffer[MAX_SENTENCE_LENGTH];
	int		nLength;
	static int nMsg = 0;
	const char* Msgs[3] =
	{"Toi %s day, cac nguoi hay den ma bai phuc.",
	"%s buoc tren linh ba vi bo, nhe nhang tien den.",
	"Mot tia sang loe len, %s tu tren troi giang xuong."};

/*
//	int				    m_nWorldStat;           // 排名
//	int				    m_nSectStat;            // 帮会排名
//	char				m_WarTongNamea[32];     //占领帮会
//  char				m_WarMaster[32];        //城主
//	int				    m_WarShuishou;          //税收
//	int                 m_Warzhi;               //物价指数
//	int                 m_IsWarCity;  
//	m_cTong
     if (Player[nPlayerIndex].m_cTong.m_nFlag)
	 { 
		 if (strstr(Player[nPlayerIndex].m_WarMaster,Player[nPlayerIndex].m_PlayerName))
		 {//城主
			 sprintf(szBuffer, Msgs[0], Player[nPlayerIndex].m_PlayerName);
		 }
	 } 
*/ 


	nMsg=GetRandomNumber(0,2);
	memset(szBuffer, 0, sizeof(szBuffer));
	sprintf(szBuffer,Msgs[nMsg],Player[nPlayerIndex].m_PlayerName);
	//nMsg++;
	//nMsg %= 3;
	nLength = TEncodeText(szBuffer, strlen(szBuffer));
//	KPlayerChat::SendSystemInfo(0,0,"Thong Bao",szBuffer,nLength); //tam tat thong tbao


	//====发送欢迎消息==================
	/*if (m_pWelcomeMsg && nPlayerIndex >0)
	{
		int	*pAMsg = (int*)m_pWelcomeMsg;
		int nCount =  *pAMsg;
		pAMsg ++;
		for (int i = 0; i < nCount; i++)
		{
			int	nLen = *pAMsg;
			KPlayerChat::SendSystemInfo(1, nPlayerIndex, "游戏管家", (((char*)pAMsg) + sizeof(int)), nLen);
			pAMsg = (int*)(((char*)pAMsg) + nLen + sizeof(int));
		}
	}*/
	
}
//重载欢迎语
void KPlayerSet::ReloadWelcomeMsg(int nPlayerIndex)
{
	//return;  //取消了？
	/*    欢迎信息文本文件格式要求：
	每行为一条信息。  Msg:信息内容	
	控制信息内容的长度，使每条信息经过编码後长度不大于MAX_SENTENCE_LENGTH
	*/
	if (m_pWelcomeMsg)
	{
		free (m_pWelcomeMsg);
		m_pWelcomeMsg = NULL;
	}
	KFile		File;
	char		Head[8];
	if (File.Open("\\Msg\\WelcomeMsg.txt") == FALSE)
		return;

	int nLen = File.Size();
	    m_pWelcomeMsg = malloc(nLen + sizeof(int));
	if (m_pWelcomeMsg == NULL)
		return;

	*(int*)m_pWelcomeMsg = 0;
	char* pSearchBegin = (char*)m_pWelcomeMsg + sizeof(int);
	int* pLineHeader = (int*)pSearchBegin;
	File.Read(pLineHeader, nLen);
	File.Close();

	char* pMsgHeader;
	while(pMsgHeader = (char*)memchr(pSearchBegin, ':', nLen))
	{
		pMsgHeader++;	//跳过那个冒号
		nLen -= pMsgHeader -pSearchBegin; //冒号后的剩余长度
		if (nLen <= 0)
			break;
		int	nSkipLen = (int)(pMsgHeader - (char*)pLineHeader - sizeof(int));
		pSearchBegin = pMsgHeader;
		if (nSkipLen < 0)
			continue;
		if (nSkipLen > 0)
			memset(&pLineHeader[1], 0, nSkipLen);
		char* pMsgTail = (char*)memchr(pSearchBegin, 0x0d, nLen);	//查找行结尾
		int	nMsgLen;
		if (pMsgTail)
		{
			*pMsgTail = 0;
			pMsgTail++;
			if ((pMsgTail < pSearchBegin + nLen) && *pMsgTail == 0x0a)
			{
				*pMsgTail = 0;
				pMsgTail++;
			}
			nMsgLen = pMsgTail - pSearchBegin;
			pSearchBegin = pMsgTail;
		}
		else
		{
			nMsgLen = nLen;
		}
		nLen -= nMsgLen;
		nMsgLen = TEncodeText((char*)&pLineHeader[1], nMsgLen + nSkipLen);		
		if (nMsgLen > 0)
		{
			*pLineHeader = nMsgLen;
			pLineHeader = (int*)(((char*)pLineHeader) + sizeof(int) + (*pLineHeader));
			(*(int*)m_pWelcomeMsg) ++;
		}
		if (pMsgTail == NULL || nLen <= 0)
			break;
	}

    SendWelcomeMsg(nPlayerIndex);
	 
	//if ((int*)m_pWelcomeMsg == 0)
	{
		free(m_pWelcomeMsg);
		m_pWelcomeMsg = NULL;
	}
}


int	 KPlayerSet::CheckMaps(int nIndex,BOOL nIsShow ,BOOL isOnline)
{
	//g_DebugLog("SERVER:Player[%s] has been removed!", Npc[Player[nIndex].m_nIndex].Name);
	// 通知聊天好友自己下线了
	if (Player[nIndex].m_nLiXian == 0 && Npc[Player[nIndex].m_nIndex].m_BaiTan)
		Npc[Player[nIndex].m_nIndex].m_BaiTan = 0;

	if (!isOnline)
		return FALSE;

	Player[nIndex].ExecuteScript("\\script\\System\\player\\playerlogout.lua", "OnLogout", 0);

	Player[nIndex].m_cChat.OffLine(Player[nIndex].m_dwID);
	// 如果组队，离开队伍
	PLAYER_APPLY_LEAVE_TEAM	sLeaveTeam;
	sLeaveTeam.ProtocolType = c2s_teamapplyleave;
	Player[nIndex].LeaveTeam((BYTE*)&sLeaveTeam);
	// 如果交易正在进行，取消交易
	TRADE_DECISION_COMMAND	sTrade;
	sTrade.ProtocolType = c2s_tradedecision;
	sTrade.m_btDecision = 0;
	Player[nIndex].TradeDecision((BYTE*)&sTrade);
	// PK状态取消
	Player[nIndex].m_cPK.CloseAll();

	//--------------------------------------------
	int nWorldMapid,nIsNoReturn=0; 
	   nWorldMapid = SubWorld[Npc[Player[nIndex].m_nIndex].m_SubWorldIndex].m_SubWorldID;
	char nValKey[32]={0};
	sprintf(nValKey,"%d_IsNoReturn",nWorldMapid);
	g_NpcMapDropRate.GetInteger("List",nValKey,0,&nIsNoReturn);
	
    if (nIsNoReturn)
	{	
		Player[nIndex].SetLoginType(1);
        Player[nIndex].SetRevivalPos(53,19);
		Npc[Player[nIndex].m_nIndex].SetFightMode(FALSE);
		//Npc[Player[nIndex].m_nIndex].m_liveTime = -1;
	}

	if (nIsShow)
	{
	   printf("[Leave the game: MapId %d]: ---Whether offline: %d Name [%s] left the game. Whether to set respawn 80 nIsNoReturn (%d)!---\n",nWorldMapid,Player[nIndex].m_nLiXian,Npc[Player[nIndex].m_nIndex].Name,nIsNoReturn);
	}

 	return nIsNoReturn;
}

void KPlayerSet::__PrepareRemove(int nIndex)
{
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
#ifdef _SERVER
    if (JXRoleSaveExitFence::BlocksRelease(
        Player[nIndex].m_bRoleRevisionProtocolSeen,
        Player[nIndex].m_bRoleSaveRevisionLoaded,
        Player[nIndex].m_bRoleSavePending)) return;
#endif
	CheckMaps(nIndex,FALSE,FALSE);
	Player[nIndex].WaitForRemove();    //设置离开游戏状态
}

void KPlayerSet::mandatoryShutDownClient(int nIndex)
{
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
}

//gm操作将人直接踢出网关 和 游戏
void KPlayerSet::gm_PrepareRemove(char *accName,char *roleName)
{
	if  (!accName || !roleName) return ;

	int  nIndex = FindNameID(accName);
#ifdef _SERVER
    if (nIndex > 0 && nIndex < MAX_PLAYER &&
        JXRoleSaveExitFence::BlocksRelease(
            Player[nIndex].m_bRoleRevisionProtocolSeen,
            Player[nIndex].m_bRoleSaveRevisionLoaded,
            Player[nIndex].m_bRoleSavePending)) return;
#endif

	if  (nIndex>0 && nIndex<MAX_PLAYER)
	     CheckMaps(nIndex,FALSE,true);

	printf("[卡号处理]:---------玩家[%s],角色:[%s]!启动强行删除---------\n",accName,roleName);
	printf("[卡号处理]:---------玩家[%s],角色:[%s]!启动强行删除---------\n",accName,roleName);

	tagLeaveGame	sLeaveGame;	
	sLeaveGame.cProtocol = c2s_leavegame;   // 发送往bishop 通知 paysys
	sLeaveGame.cCmdType  = NORMAL_LEAVEGAME;

	tagLeaveGame2 lg2;
	lg2.ProtocolFamily = pf_normal;
	lg2.ProtocolID     = c2s_leavegame;	   // S3 玩家离开游戏
	lg2.cCmdType       = NORMAL_LEAVEGAME;
	lg2.nSelServer     = nCurSerIdx;      //  MAKELONG(m_GsNumBer, m_SerVerNo);//

	if  (nIndex>0 && nIndex<MAX_PLAYER)
	{
		sLeaveGame.nExtPoint    = Player[nIndex].GetExtPointChanged();
		sLeaveGame.nGameExtTime = Player[nIndex].GetExtTimeChanged();
		snprintf(sLeaveGame.szAccountName, sizeof(sLeaveGame.szAccountName), "%s", Player[nIndex].m_AccoutName);	
	}
	else
	{
		sLeaveGame.nExtPoint    =0;
		sLeaveGame.nGameExtTime =0;
		snprintf(sLeaveGame.szAccountName, sizeof(sLeaveGame.szAccountName), "%s", accName);	
	}
	snprintf(sLeaveGame.nClientIp, sizeof(sLeaveGame.nClientIp), "%s", roleName);

	if (g_pGateWayClient)
		g_pGateWayClient->SendPackToServer(&sLeaveGame, sizeof(tagLeaveGame)); //通知bishop
	if (g_pTranClient)//通知S3
		g_pTranClient->SendPackToServer(&lg2, sizeof(tagLeaveGame2));
	if (g_pChatClient)
		g_pChatClient->SendPackToServer(&lg2, sizeof(tagLeaveGame2));
	if (g_pTongClient)
		g_pTongClient->SendPackToServer(&lg2, sizeof(tagLeaveGame2));

	tagRoleEnterGame reg;
	reg.ProtocolType = c2s_roleserver_lock;
	reg.bLock  = false;            //删除锁定
	reg.Lixian = false;            //是否离线的
	reg.nRegServerIndex = nCurSerIdx;  //属于那个大区
	reg.nGsNumber       = nCurGsNumber;  //GS索引号
	snprintf(reg.Name, sizeof(reg.Name), "%s", roleName);

	if (g_pDataClient)
		g_pDataClient->SendPackToServer((const void *)&reg, sizeof(tagRoleEnterGame));
	if (g_pGateWayClient)
		g_pGateWayClient->SendPackToServer((const void *)&reg, sizeof(tagRoleEnterGame));

	if  (nIndex>0 && nIndex<MAX_PLAYER)
	     RemoveInGame(nIndex);

	return;
}

//离开游戏 或 关闭游戏时候调用
void KPlayerSet::PrepareRemove(int nIndex,BOOL isOnline)
{
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
#ifdef _SERVER
    if (JXRoleSaveExitFence::BlocksRelease(
        Player[nIndex].m_bRoleRevisionProtocolSeen,
        Player[nIndex].m_bRoleSaveRevisionLoaded,
        Player[nIndex].m_bRoleSavePending)) return;
#endif

    CheckMaps(nIndex,FALSE,isOnline);

	if (Player[nIndex].m_autoTickCount>=3)
	{
		printf("[Card number processing failed]:--Name [%s], NetConnectIdx: (%d)--\n",Npc[Player[nIndex].m_nIndex].Name,Player[nIndex].m_nNetConnectIdx);		
		tagLeaveGame	sLeaveGame;	
		sLeaveGame.cProtocol = c2s_leavegame;   // 发送往bishop 通知 paysys
		sLeaveGame.cCmdType  = NORMAL_LEAVEGAME;

		tagLeaveGame2 lg2;
		lg2.ProtocolFamily = pf_normal;
		lg2.ProtocolID     = c2s_leavegame;	   // S3 玩家离开游戏
		lg2.cCmdType       = NORMAL_LEAVEGAME;
		lg2.nSelServer     = nCurSerIdx;      //  MAKELONG(m_GsNumBer, m_SerVerNo);//

		sLeaveGame.nExtPoint    = Player[nIndex].GetExtPointChanged();
		sLeaveGame.nGameExtTime = Player[nIndex].GetExtTimeChanged();
		snprintf(sLeaveGame.nClientIp, sizeof(sLeaveGame.nClientIp), "%s", Npc[Player[nIndex].m_nIndex].Name);
		snprintf(sLeaveGame.szAccountName, sizeof(sLeaveGame.szAccountName), "%s", Player[nIndex].m_AccoutName);

		if (g_pGateWayClient)
			g_pGateWayClient->SendPackToServer(&sLeaveGame, sizeof(tagLeaveGame)); //通知bishop
		if (g_pTranClient)//通知S3
			g_pTranClient->SendPackToServer(&lg2, sizeof(tagLeaveGame2));
		if (g_pChatClient)
			g_pChatClient->SendPackToServer(&lg2, sizeof(tagLeaveGame2));
		if (g_pTongClient)
			g_pTongClient->SendPackToServer(&lg2, sizeof(tagLeaveGame2));

		tagRoleEnterGame reg;
		reg.ProtocolType = c2s_roleserver_lock;
		reg.bLock  = false;            //删除锁定
		reg.Lixian = false;            //是否离线的
		reg.nRegServerIndex = nCurSerIdx;  //属于那个大区
		reg.nGsNumber       = nCurGsNumber;  //GS索引号
		snprintf(reg.Name, sizeof(reg.Name), "%s", Npc[Player[nIndex].m_nIndex].Name);
		if (g_pDataClient)
			g_pDataClient->SendPackToServer((const void *)&reg, sizeof(tagRoleEnterGame));
		if (g_pGateWayClient)
			g_pGateWayClient->SendPackToServer((const void *)&reg, sizeof(tagRoleEnterGame));

		RemoveInGame(nIndex);

       return;
	}

	if  (isOnline)
	{//清空实际档案,如果已经是卡号的,则复制备份当
		if (g_pServer)
		{//通知玩家掉线
			if (FAILED(g_pServer->ShutdownClient(Player[nIndex].m_nNetConnectIdx)))
			{//如果断开失败 就直接删除
				printf("[Card number processing failed]:--Name [%s], NetConnectIdx: (%d)--\n",Npc[Player[nIndex].m_nIndex].Name,Player[nIndex].m_nNetConnectIdx);		
				tagLeaveGame	sLeaveGame;	
				sLeaveGame.cProtocol = c2s_leavegame;   // 发送往bishop 通知 paysys
				sLeaveGame.cCmdType  = NORMAL_LEAVEGAME;

				tagLeaveGame2 lg2;
				lg2.ProtocolFamily = pf_normal;
				lg2.ProtocolID     = c2s_leavegame;	   // S3 玩家离开游戏
				lg2.cCmdType       = NORMAL_LEAVEGAME;
				lg2.nSelServer     = nCurSerIdx;      //  MAKELONG(m_GsNumBer, m_SerVerNo);//

				sLeaveGame.nExtPoint    = Player[nIndex].GetExtPointChanged();
				sLeaveGame.nGameExtTime = Player[nIndex].GetExtTimeChanged();
				snprintf(sLeaveGame.nClientIp, sizeof(sLeaveGame.nClientIp), "%s", Npc[Player[nIndex].m_nIndex].Name);
				snprintf(sLeaveGame.szAccountName, sizeof(sLeaveGame.szAccountName), "%s", Player[nIndex].m_AccoutName);

				if (g_pGateWayClient)
					g_pGateWayClient->SendPackToServer(&sLeaveGame, sizeof(tagLeaveGame)); //通知bishop
				if (g_pTranClient)//通知S3
					g_pTranClient->SendPackToServer(&lg2, sizeof(tagLeaveGame2));
				if (g_pChatClient)
					g_pChatClient->SendPackToServer(&lg2, sizeof(tagLeaveGame2));
				if (g_pTongClient)
					g_pTongClient->SendPackToServer(&lg2, sizeof(tagLeaveGame2));

				tagRoleEnterGame reg;
				reg.ProtocolType = c2s_roleserver_lock;
				reg.bLock  = false;            //删除锁定
				reg.Lixian = false;            //是否离线的
				reg.nRegServerIndex = nCurSerIdx;  //属于那个大区
				reg.nGsNumber       = nCurGsNumber;  //GS索引号
				snprintf(reg.Name, sizeof(reg.Name), "%s", Npc[Player[nIndex].m_nIndex].Name);
				if (g_pDataClient)
					g_pDataClient->SendPackToServer((const void *)&reg, sizeof(tagRoleEnterGame));
				if (g_pGateWayClient)
					g_pGateWayClient->SendPackToServer((const void *)&reg, sizeof(tagRoleEnterGame));
					
				RemoveInGame(nIndex);
			}
		}
		return;
	}
    
	Player[nIndex].WaitForRemove();    //设置离开游戏状态
}

//登陆失败的
void KPlayerSet::PrepareLoginFailed(int nIndex)
{
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;

	//g_DebugLog("SERVER:Player[%s] has been removed!", Npc[Player[nIndex].m_nIndex].Name);
	// 通知聊天好友自己下线了
	Player[nIndex].m_cChat.OffLine(Player[nIndex].m_dwID);
	// 如果组队，离开队伍
	PLAYER_APPLY_LEAVE_TEAM	sLeaveTeam;
	sLeaveTeam.ProtocolType = c2s_teamapplyleave;
	Player[nIndex].LeaveTeam((BYTE*)&sLeaveTeam);
	// 如果交易正在进行，取消交易
	TRADE_DECISION_COMMAND	sTrade;
	sTrade.ProtocolType = c2s_tradedecision;
	sTrade.m_btDecision = 0;
	Player[nIndex].TradeDecision((BYTE*)&sTrade);
	// PK状态取消
	Player[nIndex].m_cPK.CloseAll();
	// 帮会状态处理
	Player[nIndex].m_cTong.Clear();

	int nWorldMapid,nIsNoReturn=0; 
	    nWorldMapid = SubWorld[Npc[Player[nIndex].m_nIndex].m_SubWorldIndex].m_SubWorldID;
	
	char nValKey[32]={0};
		sprintf(nValKey,"%d_IsNoReturn",nWorldMapid);
		g_NpcMapDropRate.GetInteger("List",nValKey,0,&nIsNoReturn);

    //int nReg = NpcSet.CheckThisNpc(nWorldMapid,"MapID");
	///NpcSet.CheckForBit(nWorldMapid,"MapID",1);
    if (nIsNoReturn)
	{
		Player[nIndex].SetLoginType(1);
        Player[nIndex].SetRevivalPos(53,19);
		Npc[nIndex].SetFightMode(FALSE);
		//Npc[Player[nIndex].m_nIndex].m_liveTime = -1;
	}	
    printf("[登陆失败]:------玩家[%s]离开游戏-!------\n", Npc[Player[nIndex].m_nIndex].Name);
    // 利用TimeOut来清Player
	Player[nIndex].m_nNetConnectIdx = -1;
	Player[nIndex].m_dwLoginTime = 0;
}
//玩家开始跨服处理
void KPlayerSet::PrepareExchange(int i)
{
	if (!Player[i].m_bExchangeServer || !Npc[Player[i].m_nIndex].m_bExchangeServer)
		return;

	// 清除PK状态
	Player[i].m_cPK.CloseAll();

	// 离队
	PLAYER_APPLY_LEAVE_TEAM	sLeaveTeam;
	sLeaveTeam.ProtocolType = c2s_teamapplyleave;
	Player[i].LeaveTeam((BYTE*)&sLeaveTeam);

	// 如果交易正在进行，取消交易
	TRADE_DECISION_COMMAND	sTrade;
	sTrade.ProtocolType = c2s_tradedecision;
	sTrade.m_btDecision = 0;
	Player[i].TradeDecision((BYTE*)&sTrade);

	if (Player[i].Save())//生成数据包
	{
		Player[i].m_uMustSave = SAVE_DOING;
		Player[i].SavePlayerData(true);
	}

//	Player[i].UpdateEnterGamePos(Player[i].m_sExchangePos.m_dwMapID,
//		Player[i].m_sExchangePos.m_nX,
//		Player[i].m_sExchangePos.m_nY,
//		Npc[Player[i].m_nIndex].m_FightMode);
}
//卡号处理
void KPlayerSet::RemoveInGame(int nIndex)
{
		if (Player[nIndex].m_nIndex > 0)	// have npc
		{
			int nRegion   = Npc[Player[nIndex].m_nIndex].m_RegionIndex;
			int nSubWorld = Npc[Player[nIndex].m_nIndex].m_SubWorldIndex;

			if (nSubWorld >= 0 && nRegion >= 0)
			{
				SubWorld[nSubWorld].m_MissionArray.RemovePlayer(nIndex,SubWorld[nSubWorld].m_SubWorldID);
				SubWorld[nSubWorld].RemovePlayer(nRegion, nIndex);

				SubWorld[nSubWorld].m_Region[nRegion].RemoveNpc(Player[nIndex].m_nIndex);
				//SubWorld[nSubWorld].m_Region[nRegion].DecNpcRef(Npc[Player[nIndex].m_nIndex].m_MapX, Npc[Player[nIndex].m_nIndex].m_MapY);
				SubWorld[nSubWorld].m_Region[nRegion].DecRef(Npc[Player[nIndex].m_nIndex].m_MapX, Npc[Player[nIndex].m_nIndex].m_MapY,obj_npc);
			}	
			NpcSet.Remove(Player[nIndex].m_nIndex);
		}

		// 帮会状态处理
		Player[nIndex].m_cTong.Clear();
		Player[nIndex].m_ItemList.RemoveAll();    //删除物品不生成存档包
		Player[nIndex].m_dwID           = 0;      //数据归零
		Player[nIndex].m_nIndex         = 0;
		Player[nIndex].m_nNetConnectIdx = -1;     //
		V9_ReleaseAllSessions(nIndex);
		Player[nIndex].Release();
		m_FreeIdxPlayerSet.Insert(nIndex);        //删除链表
		m_UseIdxPlayerSet.Remove(nIndex);         
		m_nNumPlayer --;                          //人数减一
}
//某个索引玩家离开游戏时GS调用
void KPlayerSet::RemoveQuiting(int nIndex)
{
    if (nIndex <= 0 || nIndex >= MAX_PLAYER) return;
#ifdef _SERVER
    if (JXRoleSaveExitFence::BlocksRelease(
        Player[nIndex].m_bRoleRevisionProtocolSeen,
        Player[nIndex].m_bRoleSaveRevisionLoaded,
        Player[nIndex].m_bRoleSavePending)) return;
#endif
	if (Player[nIndex].m_nNetConnectIdx == -1 || Player[nIndex].m_dwID == 0 || Player[nIndex].m_nLiXian)
		return;

	if (Player[nIndex].IsWaitingRemove())
	{//是离开状态
		if (Player[nIndex].m_nIndex > 0)	// have npc
		{
			printf("------RemoveQuiting: (%s:%s)------- \n",Player[nIndex].m_AccoutName,Player[nIndex].m_PlayerName);
			int nRegion   = Npc[Player[nIndex].m_nIndex].m_RegionIndex;
			int nSubWorld = Npc[Player[nIndex].m_nIndex].m_SubWorldIndex;
			
			if (nSubWorld >= 0 && nRegion >= 0)
			{
				SubWorld[nSubWorld].m_MissionArray.RemovePlayer(nIndex,SubWorld[nSubWorld].m_SubWorldID);
				SubWorld[nSubWorld].RemovePlayer(nRegion, nIndex);

				SubWorld[nSubWorld].m_Region[nRegion].RemoveNpc(Player[nIndex].m_nIndex);
				//SubWorld[nSubWorld].m_Region[nRegion].DecNpcRef(Npc[Player[nIndex].m_nIndex].m_MapX, Npc[Player[nIndex].m_nIndex].m_MapY);
				SubWorld[nSubWorld].m_Region[nRegion].DecRef(Npc[Player[nIndex].m_nIndex].m_MapX, Npc[Player[nIndex].m_nIndex].m_MapY,obj_npc);

				// call OnLeaveMap khi player thoat game
				Player[nIndex].ExeNoBackScript(SubWorld[nSubWorld].m_szNewWorldScript, "OnLeaveMap");
			}	
			NpcSet.Remove(Player[nIndex].m_nIndex);
		}
		
		// 帮会状态处理
		Player[nIndex].m_cTong.Clear();

		Player[nIndex].m_ItemList.RemoveAll();    //删除物品不生成存档包

		Player[nIndex].m_dwID           = 0;      //数据归零
		Player[nIndex].m_nIndex         = 0;
		Player[nIndex].m_nNetConnectIdx = -1;
		V9_ReleaseAllSessions(nIndex);
		Player[nIndex].Release();
		
		m_FreeIdxPlayerSet.Insert(nIndex);        //删除链表
		m_UseIdxPlayerSet.Remove(nIndex);
		m_nNumPlayer --;                          //人数减一
	}
}	
//处理登陆超时
void KPlayerSet::RemoveLoginTimeOut(int nIndex)
{
	if (Player[nIndex].IsLoginTimeOut())
	{
		if (Player[nIndex].m_nIndex > 0)
		{
			int nRegion = Npc[Player[nIndex].m_nIndex].m_RegionIndex;
			int nSubWorld = Npc[Player[nIndex].m_nIndex].m_SubWorldIndex;
				
			if (nSubWorld >= 0 && nRegion >= 0)
			{
				SubWorld[nSubWorld].m_Region[nRegion].RemoveNpc(Player[nIndex].m_nIndex);
				//SubWorld[nSubWorld].m_Region[nRegion].DecNpcRef(Npc[Player[nIndex].m_nIndex].m_MapX, Npc[Player[nIndex].m_nIndex].m_MapY);
				SubWorld[nSubWorld].m_Region[nRegion].DecRef(Npc[Player[nIndex].m_nIndex].m_MapX, Npc[Player[nIndex].m_nIndex].m_MapY,obj_npc);
			}
			NpcSet.Remove(Player[nIndex].m_nIndex);
		}	
		
		Player[nIndex].m_ItemList.RemoveAll();
		Player[nIndex].m_dwID = 0;
		Player[nIndex].m_nIndex = 0;
		Player[nIndex].m_nNetConnectIdx = -1;
		V9_ReleaseAllSessions(nIndex);
		Player[nIndex].LoginTimeOut();      //通知好友下线了
		m_FreeIdxPlayerSet.Insert(nIndex);
		m_UseIdxPlayerSet.Remove(nIndex);
		m_nNumPlayer --;
	}
}
//处理转换地图时
void KPlayerSet::RemoveExchanging(int nIndex)
{//等于掉线
	if (Player[nIndex].m_nNetConnectIdx == -1)
		return;

	//if (Player[nIndex].IsExchangingServer())
	{//是正在跨服的 
		//if (Player[nIndex].Save())	
		//	Player[nIndex].m_uMustSave = SAVE_REQUEST;	 //删除东西立即存档

		int nRegion = Npc[Player[nIndex].m_nIndex].m_RegionIndex;
		int nSubWorld = Npc[Player[nIndex].m_nIndex].m_SubWorldIndex;
		
		if (nSubWorld >= 0 && nRegion >= 0)
		{
			SubWorld[nSubWorld].m_MissionArray.RemovePlayer(nIndex,SubWorld[nSubWorld].m_SubWorldID);
			SubWorld[nSubWorld].RemovePlayer(nRegion, nIndex);
			SubWorld[nSubWorld].m_Region[nRegion].RemoveNpc(Player[nIndex].m_nIndex);
			//SubWorld[nSubWorld].m_Region[nRegion].DecNpcRef(Npc[Player[nIndex].m_nIndex].m_MapX, Npc[Player[nIndex].m_nIndex].m_MapY);
			SubWorld[nSubWorld].m_Region[nRegion].DecRef(Npc[Player[nIndex].m_nIndex].m_MapX, Npc[Player[nIndex].m_nIndex].m_MapY,obj_npc);

			// call lua OnLeaveMap khi player chuyen gs
			Player[nIndex].ExeNoBackScript(SubWorld[nSubWorld].m_szNewWorldScript, "OnLeaveMap");
		}
		
		if (Player[nIndex].m_nIndex > 0)
			NpcSet.Remove(Player[nIndex].m_nIndex);

		Player[nIndex].m_ItemList.RemoveAll();
		Player[nIndex].m_dwID = 0;
		Player[nIndex].m_nIndex = 0;
		Player[nIndex].m_nNetConnectIdx = -1;
		V9_ReleaseAllSessions(nIndex);
		Player[nIndex].Release();
		
		m_FreeIdxPlayerSet.Insert(nIndex);
		m_UseIdxPlayerSet.Remove(nIndex);
		m_nNumPlayer --;
	}
}
#endif

//根据连接号 查找 playerid
int KPlayerSet::FindClient(int nClient)
{
	int i;
	i = m_UseIdxPlayerSet.GetNext(0);
	while(i)
	{
		if (Player[i].m_nNetConnectIdx == nClient)
		{
			return i;
		}
		i = m_UseIdxPlayerSet.GetNext(i);
	}
	return 0;
}

//服务器端接受客户端发来的网路协议包
#ifdef _SERVER
void KPlayerSet::ProcessClientMessage(int nIndex, const char* pChar, int nSize)
{
	if (nIndex <= 0 && nIndex >= MAX_PLAYER)
		return;

	int i = Player[nIndex].m_nNetConnectIdx;

	if (i >= 0)
	{
//		_ASSERT(g_CoreServerShell.CheckProtocolSize(pChar, nSize));
		if (Player[nIndex].m_dwID && !Player[nIndex].m_bExchangeServer)
			g_ProtocolProcess.ProcessNetMsg(nIndex, (BYTE*)pChar);
	}
}
#endif

//---------------------------------------------------------------------------
//	功能：构造函数
//---------------------------------------------------------------------------
KLevelAdd::KLevelAdd()
{
	memset(m_nLevelExp, 0, sizeof(int) * MAX_LEVEL);
	memset(m_nLifePerLevel, 0, sizeof(int) * series_num);
	memset(m_nManaPerLevel, 0, sizeof(int) * series_num);
	memset(m_nStaminaPerLevel, 0, sizeof(int) * series_num);
	memset(m_nLifePerVitality, 0, sizeof(int) * series_num);
	memset(m_nStaminaPerVitality, 0, sizeof(int) * series_num);
	memset(m_nManaPerEnergy, 0, sizeof(int) * series_num);
}

//---------------------------------------------------------------------------
//	功能：初始化
//---------------------------------------------------------------------------
BOOL	KLevelAdd::Init()
{
	int			i;
	KTabFile	LevelExp;
	if ( !LevelExp.Load(PLAYER_LEVEL_EXP_FILE) )
		return FALSE;
	for (i = 0; i < MAX_LEVEL; ++i)
	{
		LevelExp.GetInteger(i + 2, 2, 0, &m_nLevelExp[i]);
	}
	LevelExp.Clear();

	KTabFile	LevelAdd;
	if ( !LevelAdd.Load(PLAYER_LEVEL_ADD_FILE) )
		return FALSE;
	for (i = 0; i < series_num; ++i)
	{
		LevelAdd.GetInteger(i + 2, 2, 0, &m_nLifePerLevel[i]);
		LevelAdd.GetInteger(i + 2, 3, 0, &m_nStaminaPerLevel[i]);
		LevelAdd.GetInteger(i + 2, 4, 0, &m_nManaPerLevel[i]);
		LevelAdd.GetInteger(i + 2, 5, 0, &m_nLifePerVitality[i]);
		LevelAdd.GetInteger(i + 2, 6, 0, &m_nStaminaPerVitality[i]);
		LevelAdd.GetInteger(i + 2, 7, 0, &m_nManaPerEnergy[i]);
		LevelAdd.GetInteger(i + 2, 8, 0, &m_nLeadExpShare[i]);
		LevelAdd.GetInteger(i + 2, 9, 0, &m_nFireResistPerLevel[i]);
		LevelAdd.GetInteger(i + 2, 10, 0, &m_nColdResistPerLevel[i]);
		LevelAdd.GetInteger(i + 2, 11, 0, &m_nPoisonResistPerLevel[i]);
		LevelAdd.GetInteger(i + 2, 12, 0, &m_nLightResistPerLevel[i]);
		LevelAdd.GetInteger(i + 2, 13, 0, &m_nPhysicsResistPerLevel[i]);
	}
	LevelAdd.Clear();
	return TRUE;
}

//---------------------------------------------------------------------------
//	功能：获得某等级的升级经验
//	参数：nLevel  目标等级
//---------------------------------------------------------------------------
int	KLevelAdd::GetLevelExp(int nLevel)
{
	if (nLevel < 1 || nLevel > MAX_LEVEL)
		return 0;
	return m_nLevelExp[nLevel - 1];
}

//---------------------------------------------------------------------------
//	功能：获得每个系升级加生命点
//---------------------------------------------------------------------------
int		KLevelAdd::GetLifePerLevel(int nSeries)
{
	if (nSeries < 0 || nSeries >= series_num)
		return 0;
	return m_nLifePerLevel[nSeries];
}

//---------------------------------------------------------------------------
//	功能：获得每个系升级加体力点
//---------------------------------------------------------------------------
int		KLevelAdd::GetStaminaPerLevel(int nSeries)
{
	if (nSeries < 0 || nSeries >= series_num)
		return 0;
	return m_nStaminaPerLevel[nSeries];
}

//---------------------------------------------------------------------------
//	功能：获得每个系升级加内力点
//---------------------------------------------------------------------------
int		KLevelAdd::GetManaPerLevel(int nSeries)
{
	if (nSeries < 0 || nSeries >= series_num)
		return 0;
	return m_nManaPerLevel[nSeries];
}

//---------------------------------------------------------------------------
//	功能：每个系活力点增加一点后生命点增长
//---------------------------------------------------------------------------
int		KLevelAdd::GetLifePerVitality(int nSeries)
{
	if (nSeries < 0 || nSeries >= series_num)
		return 0;
	return m_nLifePerVitality[nSeries];
}

//---------------------------------------------------------------------------
//	功能：每个系活力点增加一点后体力点增长
//---------------------------------------------------------------------------
int		KLevelAdd::GetStaminaPerVitality(int nSeries)
{
	if (nSeries < 0 || nSeries >= series_num)
		return 0;
	return m_nStaminaPerVitality[nSeries];
}

//---------------------------------------------------------------------------
//	功能：每个系精力点增加一点后内力点增长
//---------------------------------------------------------------------------
int		KLevelAdd::GetManaPerEnergy(int nSeries)
{
	if (nSeries < 0 || nSeries >= series_num)
		return 0;
	return m_nManaPerEnergy[nSeries];
}

//---------------------------------------------------------------------------
//	功能：每个系精力点增加一点后内力点增长
//---------------------------------------------------------------------------
int		KLevelAdd::GetLeadExpShare(int nSeries)
{
	if (nSeries < 0 || nSeries >= series_num)
		return 0;
	return m_nLeadExpShare[nSeries];
}

//---------------------------------------------------------------------------
//	功能：每个系某个等级的基本火抗性
//---------------------------------------------------------------------------
int		KLevelAdd::GetFireResist(int nSeries, int nLevel)
{
	if (nSeries < 0 || nSeries >= series_num)
		return 0;
	if (nLevel <= 0 || nLevel >= MAX_LEVEL)
		return 0;
	return (m_nFireResistPerLevel[nSeries] * nLevel / 100);
}

//---------------------------------------------------------------------------
//	功能：每个系某个等级的基本冰抗性
//---------------------------------------------------------------------------
int		KLevelAdd::GetColdResist(int nSeries, int nLevel)
{
	if (nSeries < 0 || nSeries >= series_num)
		return 0;
	if (nLevel <= 0 || nLevel >= MAX_LEVEL)
		return 0;
	return (m_nColdResistPerLevel[nSeries] * nLevel / 100);
}

//---------------------------------------------------------------------------
//	功能：每个系某个等级的基本毒抗性
//---------------------------------------------------------------------------
int		KLevelAdd::GetPoisonResist(int nSeries, int nLevel)
{
	if (nSeries < 0 || nSeries >= series_num)
		return 0;
	if (nLevel <= 0 || nLevel >= MAX_LEVEL)
		return 0;
	return (m_nPoisonResistPerLevel[nSeries] * nLevel / 100);
}

//---------------------------------------------------------------------------
//	功能：每个系某个等级的基本电抗性
//---------------------------------------------------------------------------
int		KLevelAdd::GetLightResist(int nSeries, int nLevel)
{
	if (nSeries < 0 || nSeries >= series_num)
		return 0;
	if (nLevel <= 0 || nLevel >= MAX_LEVEL)
		return 0;
	return (m_nLightResistPerLevel[nSeries] * nLevel / 100);
}

//---------------------------------------------------------------------------
//	功能：每个系某个等级的基本物理抗性
//---------------------------------------------------------------------------
int		KLevelAdd::GetPhysicsResist(int nSeries, int nLevel)
{
	if (nSeries < 0 || nSeries >= series_num)
		return 0;
	if (nLevel <= 0 || nLevel >= MAX_LEVEL)
		return 0;
	return (m_nPhysicsResistPerLevel[nSeries] * nLevel / 100);
}

//---------------------------------------------------------------------------
//	功能：构造函数
//---------------------------------------------------------------------------
KTeamLeadExp::KTeamLeadExp()
{
	for (int i = 0; i < MAX_LEAD_LEVEL; ++i)
	{
		m_sPerLevel[i].m_dwExp = 0;
		m_sPerLevel[i].m_dwMemNum = 1;
	}
}

//---------------------------------------------------------------------------
//	功能：初始化，读取统率力数据
//---------------------------------------------------------------------------
BOOL	KTeamLeadExp::Init()
{
	int			i;
	KTabFile	LevelExp;

	if ( !LevelExp.Load(PLAYER_LEVEL_LEAD_EXP_FILE) )
		return FALSE;
	for (i = 0; i < MAX_LEAD_LEVEL; ++i)
	{
		LevelExp.GetInteger(i + 2, 2, 0, (int*)&m_sPerLevel[i].m_dwExp);
		LevelExp.GetInteger(i + 2, 3, 1, (int*)&m_sPerLevel[i].m_dwMemNum);  //能最组队的人数
	}
	LevelExp.Clear();
	return TRUE;
}

//---------------------------------------------------------------------------
//	功能：传入经验获得等级数
//---------------------------------------------------------------------------
int		KTeamLeadExp::GetLevel(DWORD dwExp, int nCurLeadLevel)
{
	if (dwExp <= 0)
		return 1;
	if (nCurLeadLevel > 0)
	{
		if (nCurLeadLevel >= MAX_LEAD_LEVEL)
			return MAX_LEAD_LEVEL;
		if (dwExp < m_sPerLevel[nCurLeadLevel - 1].m_dwExp)
			return nCurLeadLevel;
		if (dwExp < m_sPerLevel[nCurLeadLevel].m_dwExp)
			return nCurLeadLevel + 1;
	}
	for (int i = 0; i < MAX_LEAD_LEVEL; ++i)
	{
		if (dwExp < m_sPerLevel[i].m_dwExp)
		{
			return i + 1;
		}
	}
	return MAX_LEAD_LEVEL;
}

//---------------------------------------------------------------------------
//	功能：传入经验获得可带队员数
//---------------------------------------------------------------------------
int		KTeamLeadExp::GetMemNumFromExp(DWORD dwExp)
{
	int nGetLevel;
	nGetLevel = GetLevel(dwExp);
	return m_sPerLevel[nGetLevel - 1].m_dwMemNum;
}

//---------------------------------------------------------------------------
//	功能：传入等级获得可带队员数
//---------------------------------------------------------------------------
int		KTeamLeadExp::GetMemNumFromLevel(int nLevel)
{
	if (1 <= nLevel && nLevel <= MAX_LEAD_LEVEL)
		return m_sPerLevel[nLevel - 1].m_dwMemNum;
	return 1;
}

//---------------------------------------------------------------------------
//	功能：传入等级获得升级所需经验值
//---------------------------------------------------------------------------
int		KTeamLeadExp::GetLevelExp(int nLevel)
{
	if (1 <= nLevel && nLevel <= MAX_LEAD_LEVEL)
		return this->m_sPerLevel[nLevel - 1].m_dwExp;
	return 0;
}

#ifdef _SERVER
void	KPlayerSet::Activate()
{
}
#endif

KNewPlayerAttribute::KNewPlayerAttribute()
{
	memset(m_nStrength, 0, sizeof(m_nStrength));
	memset(m_nDexterity, 0, sizeof(m_nDexterity));
	memset(m_nVitality, 0, sizeof(m_nVitality));
	memset(m_nEngergy, 0, sizeof(m_nEngergy));
	memset(m_nLucky, 0, sizeof(m_nLucky));
}

BOOL	KNewPlayerAttribute::Init()
{
	char		szSeries[5][16] = {"metal", "wood", "water", "fire", "earth"};
	KIniFile	AttributeFile;

	if ( !AttributeFile.Load(BASE_ATTRIBUTE_FILE_NAME) )
		return FALSE;

	for (int i = 0; i < series_num; ++i)
	{
		AttributeFile.GetInteger(szSeries[i], "Strength", 0, &m_nStrength[i]);
		AttributeFile.GetInteger(szSeries[i], "Dexterity", 0, &m_nDexterity[i]);
		AttributeFile.GetInteger(szSeries[i], "Vitality", 0, &m_nVitality[i]);
		AttributeFile.GetInteger(szSeries[i], "Engergy", 0, &m_nEngergy[i]);
		AttributeFile.GetInteger(szSeries[i], "Lucky", PLAYER_FIRST_LUCKY, &m_nLucky[i]);
	}
	AttributeFile.Clear();
	return TRUE;
}

#ifdef _SERVER
BOOL	KPlayerSet::GetPlayerName(int nIndex, char* szName)
{
//	int i = FindClient(nClient);

	int i = nIndex;

	if (!szName)
		return FALSE;

	if (i <= 0 || i >= MAX_PLAYER)
	{
		szName[0] = 0;
		return FALSE;
	}
	snprintf(szName, sizeof(Player[i].m_PlayerName), "%s", Player[i].m_PlayerName);
	return TRUE;
}
#endif
//玩家账号名
#ifdef _SERVER
BOOL	KPlayerSet::GetPlayerAccount(int i, char* szName)
{
//	int i = FindClient(nClient);
	if (!szName)
		return FALSE;

	if (i <= 0 || i >= MAX_PLAYER)
	{
		szName[0] = 0;
		return FALSE;
	}

	snprintf(szName, sizeof(Player[i].m_AccoutName), "%s", Player[i].m_AccoutName);
	return TRUE;
}
#endif

//设置网络连接号 和 存档时间
#ifdef _SERVER
//强行跨服
void	KPlayerSet::ForceTransWorld(DWORD nSubWorlIdx,int nXpos,int nYpos,int ntarServerIdx,int nLevel)
{
	int nUseIdx = m_UseIdxPlayerSet.GetPrev(0);
	while(nUseIdx)
	{
		if (Player[nUseIdx].m_nNetConnectIdx != -1)
		{//链接服务器的
			if (g_pServer /*&& Player[nUseIdx].m_CurSerVer==nCurSerIdx*/ && Npc[Player[nUseIdx].m_nIndex].m_Level>=nLevel) 
			{
				Player[nUseIdx].m_TempSerVer = ntarServerIdx;
				//nResult =1;
				if (Npc[Player[nUseIdx].m_nIndex].TobeExchangeServer(nSubWorlIdx,nXpos*32,nYpos*32,TRUE))
				{
					  g_pServer->ShutdownClient(Player[nUseIdx].m_nNetConnectIdx);
				}
			}
		}
		nUseIdx = m_UseIdxPlayerSet.GetPrev(nUseIdx);
	}
}



//踢掉本区的所有玩家
void	KPlayerSet::KickOutPlayerOutGame()
{
	int nUseIdx = m_UseIdxPlayerSet.GetPrev(0);

	while(nUseIdx)
	{
		if (Player[nUseIdx].m_nNetConnectIdx != -1)
		{//链接服务器的
				if (Player[nUseIdx].getMobileClient())
				{
					printf("GM Kick out player one from Mobile.\n");
					Player[nUseIdx].KickOutGame();
				}
				else
					g_pServer->ShutdownClient(Player[nUseIdx].m_nNetConnectIdx);
		}

		nUseIdx = m_UseIdxPlayerSet.GetPrev(nUseIdx);
	}
}

//踢掉不属于本区的所有跨服玩家
void	KPlayerSet::KickOutPlayerThisGame()
{
	int nUseIdx = m_UseIdxPlayerSet.GetPrev(0);

	while(nUseIdx)
	{
		if (Player[nUseIdx].m_nNetConnectIdx != -1)
		{//链接服务器的
			if (g_pServer /*&& Player[nUseIdx].m_CurSerVer!=nCurSerIdx*/) 
			{
				if (Player[nUseIdx].getMobileClient())
				{
					printf("GM Kick out player one from Mobile.\n");
					Player[nUseIdx].KickOutGame();
				}
				else
			      g_pServer->ShutdownClient(Player[nUseIdx].m_nNetConnectIdx);
			}
		}

		nUseIdx = m_UseIdxPlayerSet.GetPrev(nUseIdx);
	}
}

void	KPlayerSet::KickOutPlayerWithLevel(int nLevel)
{//踢掉所有等级大于某等级的玩家 
	int nUseIdx = m_UseIdxPlayerSet.GetPrev(0);

	while(nUseIdx)
	{
		if (Player[nUseIdx].m_nNetConnectIdx != -1)
		{//链接服务器的
			if (g_pServer /*&& Player[nUseIdx].m_CurSerVer==nCurSerIdx*/ && Npc[Player[nUseIdx].m_nIndex].m_Level>=nLevel) 
			{
				if (Player[nUseIdx].getMobileClient())
				{
					printf("GM Kick out player one from Mobile.\n");
					Player[nUseIdx].KickOutGame();
				}
				else
				    g_pServer->ShutdownClient(Player[nUseIdx].m_nNetConnectIdx);
			}
		}

		nUseIdx = m_UseIdxPlayerSet.GetPrev(nUseIdx);
	}
}

int		KPlayerSet::AttachPlayer(const unsigned long lnID, GUID* pGuid,int model)
{
	if (/*lnID >= MAX_PLAYER || */NULL == pGuid)
		return 0;

	int nUseIdx = m_UseIdxPlayerSet.GetPrev(0);

	while(nUseIdx)
	{
		if (Player[nUseIdx].m_nNetConnectIdx == -1)
		{//没有链接服务器的
			if (0 == memcmp(&Player[nUseIdx].m_Guid, pGuid, sizeof(GUID)))
			{
				if (KPlayerTradeJournal::Instance().IsPlayerLocked(Player[nUseIdx].m_PlayerName))
				{
					printf("[AttachPlayer BLOCKED]: Player '%s' is locked by Trade Journal Recovery. Attach REJECTED.\n", Player[nUseIdx].m_PlayerName);
					return 0;
				}
				Player[nUseIdx].m_nNetConnectIdx = lnID;  //网络号
				Player[nUseIdx].m_ulLastSaveTime = g_SubWorldSet.m_nLoopRate;
///				Player[nUseIdx].m_dwLoginTime = -1;
				return nUseIdx;
			}
		}
		nUseIdx = m_UseIdxPlayerSet.GetPrev(nUseIdx);
	}

	return 0;
}
#endif

#ifdef _SERVER
int		KPlayerSet::GetPlayerIndexByGuid(GUID* pGuid)
{
	int nUseIdx = m_UseIdxPlayerSet.GetNext(0);
	while(nUseIdx)
	{
		if (0 == memcmp(&Player[nUseIdx].m_Guid, pGuid, sizeof(GUID)))
		{
			if (Player[nUseIdx].m_nNetConnectIdx != -1)
			{
				return nUseIdx;
			}
			else
			{
			//g_DebugLog("[error]Find Guid to a disconnect player");
				return 0;
			}
		}
		nUseIdx = m_UseIdxPlayerSet.GetNext(nUseIdx);
	}
	return 0;
}
#endif
//游戏中定时存档 玩家自动存档
#ifdef _SERVER
void KPlayerSet::AutoSave()
{
	unsigned long uTime = g_SubWorldSet.GetGameTime();	//服务器运行的 循环数
	if (uTime >= m_ulNextSaveTime)//服务器的下次存档时间
	{
		int nUseIdx = m_UseIdxPlayerSet.GetNext(0); 
		while(nUseIdx)
		{
			if (Player[nUseIdx].CanSave()  && uTime - Player[nUseIdx].m_ulLastSaveTime >= (60 * 5 * 18))//1phut save data 1 lan
			{//个人存档时间
				if (Player[nUseIdx].Save())
				{//生成数据包  
					//printf("--AutoSaveData() Name: (%s) OKE--\n", Npc[Player[nUseIdx].m_nIndex].Name); 
					m_ulNextSaveTime += m_ulDelayTimePerSave;
					Player[nUseIdx].SavePlayerData(false);
					break;  
				}
			} 
			nUseIdx = m_UseIdxPlayerSet.GetNext(nUseIdx);
		}
	}
}
#endif	

#ifndef KPlayerSetH
#define	KPlayerSetH

#include "KLinkArray.h"

#define		MAX_LEVEL					500		// 玩家最大等级
#define		MAX_SERIES					5
#define		MAX_LEAD_LEVEL				100		// 最大统率力等级

typedef struct
{
	int		m_nExp;
	int		m_nMoney;
	int		m_nItem;
	int		m_nEquip;
} KPK_DEATH_PUNISH_PARAM;

typedef struct
{
	int		m_nLevel;
	int		m_nLeadLevel;
	int		m_nMoney;
	int		m_nItemID;
} STONG_CREATE_PARAM;

typedef struct
{
	int		m_nCampFee;
} STONG_SET_PARAM;


class KLevelAdd
{
private:
	int		m_nLevelExp[MAX_LEVEL];				    // 升级经验
	int		m_nLifePerLevel[MAX_SERIES];			// 每个系升级加生命点
	int		m_nStaminaPerLevel[MAX_SERIES];			// 每个系升级加体力点
	int		m_nManaPerLevel[MAX_SERIES];			// 每个系升级加内力点
	int		m_nLifePerVitality[MAX_SERIES];			// 每个系活力点增加一点后生命点增长
	int		m_nStaminaPerVitality[MAX_SERIES];		// 每个系活力点增加一点后体力点增长
	int		m_nManaPerEnergy[MAX_SERIES];			// 每个系精力点增加一点后内力点增长
	int		m_nLeadExpShare[MAX_SERIES];			// 每个系获得战斗经验时分配统率力经验的参数
	int		m_nFireResistPerLevel[MAX_SERIES];		// 每个系升级加火抗性
	int		m_nColdResistPerLevel[MAX_SERIES];		// 每个系升级加冰抗性
	int		m_nPoisonResistPerLevel[MAX_SERIES];	// 每个系升级加毒抗性
	int		m_nLightResistPerLevel[MAX_SERIES];		// 每个系升级加电抗性
	int		m_nPhysicsResistPerLevel[MAX_SERIES];	// 每个系升级加物理抗性
public:
	KLevelAdd();
	BOOL		Init();
	int			GetLevelExp(int nLevel);
	int			GetLifePerLevel(int nSeries);
	int			GetStaminaPerLevel(int nSeries);
	int			GetManaPerLevel(int nSeries);
	int			GetLifePerVitality(int nSeries);
	int			GetStaminaPerVitality(int nSeries);
	int			GetManaPerEnergy(int nSeries);
	int			GetLeadExpShare(int nSeries);
	int			GetFireResist(int nSeries, int nLevel);
	int			GetColdResist(int nSeries, int nLevel);
	int			GetPoisonResist(int nSeries, int nLevel);
	int			GetLightResist(int nSeries, int nLevel);
	int			GetPhysicsResist(int nSeries, int nLevel);
};


class KTeamLeadExp
{
public:
	struct LEADLEVEL
	{
		UINT	m_dwExp;								// 统率力升级经验
		UINT	m_dwMemNum;								// 统率等级决定能统率的队员人数
	}m_sPerLevel[MAX_LEAD_LEVEL];
public:
	KTeamLeadExp();
	BOOL		Init();									// 初始化
	int			GetLevel(DWORD dwExp, int nCurLeadLevel = 0);// 传入经验获得等级数 (如果传入当前等级，可以加快搜索速度)
	int			GetMemNumFromExp(DWORD dwExp);			// 传入经验获得可带队员数
	int			GetMemNumFromLevel(int nLevel);			// 传入等级获得可带队员数
	int			GetLevelExp(int nLevel);				// 传入等级获得升级所需经验值
};

class KNewPlayerAttribute
{
public:
	int			m_nStrength[series_num];
	int			m_nDexterity[series_num];
	int			m_nVitality[series_num];
	int			m_nEngergy[series_num];
	int			m_nLucky[series_num];
public:
	KNewPlayerAttribute();
	BOOL		Init();
};

class KPlayerSet
{
private:	// 用于优化查找速度
	KLinkArray		m_FreeIdxPlayerSet;				//	可用表
	KLinkArray		m_UseIdxPlayerSet;				//	已用表
	int				m_nListCurIdx;			// 用于 GetFirstPlayer 和 GetNextPlayer
#ifdef _SERVER
	unsigned long	m_ulNextSaveTime;
	unsigned long	m_ulDelayTimePerSave;	//1mins
	unsigned long	m_ulMaxSaveTimePerPlayer; //30mins
#endif
public:
	KLevelAdd				m_cLevelAdd;
	KTeamLeadExp			m_cLeadExp;
	KNewPlayerAttribute		m_cNewPlayerAttribute;
	STONG_CREATE_PARAM		m_sTongParam;
	STONG_SET_PARAM			m_sTongSetParam;

#ifdef _SERVER
	KPK_DEATH_PUNISH_PARAM	m_sPKPunishParam[MAX_DEATH_PUNISH_PK_VALUE + 1];	// PK惩罚参数
#endif

	std::map<int, int> m_tglc;	// moc thoi gian lam cham
	std::map<int, int> m_tgtd;  // moc thoi gian trung doc
	std::map<int, int> m_tgc;	// moc thoi gian choang
	std::map<int, int> m_tgph;	// moc thoi gian phuc hoi
	std::map<int, float> m_bs;	// moc bang sat
	std::map<int, float> m_ds;	// moc doc sat
	std::map<int, int> m_tdd;   // moc toc do danh

public:
	int		GetTGLC(int nValue);
	int		GetTGTD(int nValue);
	int		GetTGC(int nValue);
	int		GetTGPH(int nValue);
	float	GetMBS(int nValue);
	float	GetMDS(int nValue);
	int		GetTDD(int nValue);

public:
	KPlayerSet();
	BOOL	Init();
	int		FindSame(DWORD dwID);
	int     FindNameID(LPSTR szName);  //服务器端用
	int     FindByTongName(LPSTR szTongName);  //服务器端用
	int     CheckLiXian(LPSTR szName);
	int     FindClient(int nClient);   //根据连接号 查找玩家Playerid
	int		FindPlayerID(DWORD dwID);
	int		GetFirstPlayer();				// 遍历所有玩家第一步
	int		GetNextPlayer();				// 遍历所有玩家下一步(这支函数必须在上一支调用之后才能调用)
	int		GetOnlinePlayerCount() { return m_UseIdxPlayerSet.GetCount(); }
//	DWORD   TakeTrader(DWORD a,DWORD b);//求商
//	DWORD   TakeRemainder(DWORD a,DWORD b);//取于

//	int		FindClient(int nClient);

#ifdef	_SERVER
	void	Activate();
	int		Add(LPSTR szPlayerID, void* pGuid,bool nKeepForBt=true);
	void	PrepareRemove(int nIndex,BOOL isOnline=FALSE);
	void gm_PrepareRemove(char *accName,char *roleName);
	void    mandatoryShutDownClient(int nIndex); //强制性 关闭客户端,用于卡号处理
	void	__PrepareRemove(int nIndex);
	int	    CheckMaps(int nIndex,BOOL nIsShow=FALSE,BOOL isOnline=TRUE);
	void	PrepareExchange(int nIndex);
	void	PrepareLoginFailed(int nIndex);
	void	RemoveQuiting(int nIndex);
	void    RemoveInGame(int nIndex);
	void	RemoveLoginTimeOut(int nIndex);
	void	RemoveExchanging(int nIndex);
    void	AutoSave();
	void	ProcessClientMessage(int nClient, const char* pChar, int nSize);
	int		GetPlayerNumber() { return m_nNumPlayer; } //玩家数量
	BOOL	GetPlayerName(int nClient, char* szName);  //玩家姓名
	BOOL	GetPlayerAccount(int nClient, char* szName);//玩家账号
	int		Broadcasting(char* pMessage, int nLen);
	void	SetSaveBufferPtr(void* pData);              //设置存档的 数据包大小
	int		AttachPlayer(const unsigned long lnID, GUID* pGuid,int model=0);   //GS调用 获取玩家的 链表号 即：playerid
	void	KickOutPlayerThisGame();
	void	KickOutPlayerOutGame();
	//强行跨服
	void	ForceTransWorld(DWORD nSubWorlIdx,int nXpos,int nYpos,int ntarServerIdx,int nLevel);
	void	KickOutPlayerWithLevel(int nLevel);
	int		GetPlayerIndexByGuid(GUID* pGuid);
#endif

private:
	int		FindFree();

#ifdef	_SERVER
	//welcome message deliver
public:
	void	ReloadWelcomeMsg(int nPlayerIndex=0);
    void	SendWelcomeMsg(int nPlayerIndex=0);
	void    SendAttackMsg(void *data/*int nYidx,int nDidx*/,int nType=0);
private:
	//void	SendWelcomeMsg(int nPlayerIndex=0);
	void*	m_pWelcomeMsg;

private:
	int		m_nNumPlayer;
#endif
};

extern KPlayerSet PlayerSet;
#endif //KPlayerSetH

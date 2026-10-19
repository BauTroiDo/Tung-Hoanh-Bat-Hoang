#ifndef KPlayerH
#define	KPlayerH
//#include <objbase.h>
#include "KWorldMsg.h"
#include "KMessage.h"
#include "KInventory.h"
#include "KPlayerFaction.h"
#include "KPlayerChat.h"
#include "KPlayerTask.h"
#include "KPlayerMenuState.h"
#include "KPlayerTrade.h"
#include "KPlayerTeam.h"
#include "KPlayerPK.h"
#include "KPlayerRepute.h"
#include "KPlayerFuYuan.h"
#include "KPlayerReBorn.h"
#include "KPlayerVip.h"
#include "KPlayerTong.h"
#include "KItemList.h"
#include "KNpc.h"
#include "KSkills.h"

#include "KPlayerDef.h"

/*#ifdef _SERVER
#define	MAX_PLAYER		500   //玩家人数限制 1200 256
#else
#define	MAX_PLAYER		2    //Index 0 reserved 500
#endif*/

#define		MAX_ANSWERNUM					16 //
#define		PLAYER_LIFE_REPLENISH			0
#define		PLAYER_MANA_REPLENISH			0
#define		PLAYER_STAMINA_GAIN				1
#define		PLAYER_STAMINA_LOSS				4

#define		STRENGTH_SET_DAMAGE_VALUE		5
#define		DEXTERITY_SET_DAMAGE_VALUE		5

//#define		MAX_AVENGE_NUM					4

#define		BASE_FIRE_RESIST_MAX			150//275  //火防最大值
#define		BASE_COLD_RESIST_MAX			150//     //冰防最大值
#define		BASE_POISON_RESIST_MAX			150//     //毒防最大值
#define		BASE_LIGHT_RESIST_MAX			150//     //雷防最大值
#define		BASE_PHYSICS_RESIST_MAX			150//     //普防最大值
#define		BASE_FANGYU_ALL_MAX			    75        //原始五防御
#define		defPLAYER_SAVE_TIMEOUT			18*60*5   // 存档时间


enum UIInfo //脚本通知显示的界面类型
{
	UI_SELECTDIALOG,
	UI_TRADEDIALOG,
	UI_TALKDIALOG,
	UI_NOTEINFO,
	UI_MSGINFO,      //自右向左冒出来的信息
	UI_NEWSINFO,     //滚动新闻
	UI_PLAYMUSIC,
	UI_OPENTONGUI,
};

// 重生点位置信息
typedef struct PLAYER_REVIVAL_POS_DATA
{
	int				m_nSubWorldID;		// 重生点地图
	int				m_ReviveID;			// 重生点索引
	int				m_nMpsX;			// 重生点地图位置 x
	int				m_nMpsY;			// 重生点地图位置 y
} PLAYER_REVIVAL_POS;

typedef struct
{
	int				m_nSubWorldId;		// 传送门世界ID
	int				m_nTime;			// 传送门保持时间
	int				m_nMpsX;
	int				m_nMpsY;
} PLAYER_TOWNPORTAL_POS;

typedef struct 
{
	UINT			m_dwMapID;
	int				m_nX;
	int				m_nY;
} PLAYER_EXCHANGE_POS;


struct KplayerTimeInfo
{
	  int nMapLetTime;
	  int nExpLeftTime;
	  int nLuckLeftTime;
	  int nKangLeftTime;
	  int nYaoLeftTime;
	  int nSupManaMauLeftTime;
	  int nSupKyNangLeftTime;
	  int nNhatKyLeftTime;
};

//玩家老虎机结构 
struct KplayerGuzi
{
	BOOL nIsOpen;
	int  nTpye;
	int  nYingL;
	int  nJinBi;
	int  nDianShu;
	int  nBeiLv;
};




//玩家挂机结构
#ifdef _SERVER
struct KplayerAuto
{
	int  nIsNoBlue;
	int  nIsOpenPicjkup;
	int  nAutoMoney;   //钱
	int  nAutoEques;   //装备
	int  nAutoMoneyAndEques;
	int  nAutoTuPu;    //图谱
	BYTE nAutoTool;   //道具
};

#endif

#ifndef _SERVER

struct KplayerAuto
{
    struct	Kteam
	{
		BYTE	nIsHuiDa;
		BYTE    nIsDuiZhang;
		BYTE    nIsDuiYuan;
	}; //组队的

	struct	Kskill
	{
		BYTE	nIsOpen;
		int     nsKillId;
		int     nCostTime;
		int     nsKillRows;
		int     nsFactionTaskVal;
		//int     nSkilloop;
	};

	struct	KLRskill
	{
		BYTE	nIsOpen;
		int     nsLeftKillId;
		int     nsRightKillId;
		int     nCostTime;
		int     nsKillRows;
		int     nsFactionTaskVal;
		int     nSkilloop;
	};
	int   nSkilloop;
	DWORD nLocKnpcDw;
    int nCurXpos;
	int nCurYpos;
	int nOldIndex;   //卡怪的NPC的索引
	int nHideNpc;
	int nHidePlayer;
	int nHideMiss;
	int nAutoMoney;
	int nAutoEques;
	int nAutoMoneyAndEques;
	BYTE nShuXingNum; //属性数量
	int nAutoTuPu;
	BYTE nAutoTool;
	BYTE nAutoOther;  //检测装备名字
	BYTE nIsAuToFenJie;
	BOOL nIsYaBiao;
	BYTE nCurNum;
	BYTE nIsHaveDo;
	BYTE nIsJiaoBen;  //是否录制脚本
	BYTE nAutoModel;
	BYTE nCurStaus;  //当前的状态
	BYTE nIsOpenZiMu;//是否自动放进子母袋
	BYTE nIsAutoSay; //是否自动发言
	BYTE nIsAutoNuQi;
	BYTE nIsAutoUseItem;//自动使用物品
	BYTE nIsShowPath;   //是否显示路线
	BYTE nIsNoBlue;     //是否不攻击蓝绿怪
	BYTE nIsNoYellow;   //是否不攻击黄怪
	BYTE nIsNoRed;      //是否不攻击红怪
	BOOL nIsFollow;     //是否跟随队长
	BOOL nIsFell;
	int  nAttackNum;
    Kteam nCteam;
	int  nNpcCurlife;//备份的NPC生命
	int  nShiQuNum;
	int  nVal;
	int  nAutoTime;  //取消捡取的时间 延时多少秒后 取消捡取 或攻击
	int  ngMoneyLoop;
	int  ngItemLoop;
	int  ngTuzhiLoop;
	int  ngToolLoop;
	int  ngAttackLoop;
	Kskill  nAutoSkill[4];
	KLRskill nAutoLRskill;
	BOOL nIsReTurn;//是否正在回城
	BOOL nIsUseSkill;
	int  nNpcDis;  //寻怪范围
};
#endif

class KIniFile;

#ifdef TOOLVERSION
class CORE_API KPlayer
#else
class KPlayer
#endif
{
private:
	int				m_nLeftSkillID;
	int             m_nLeftListidx;
	int				m_nLeftSkillLevel;
	int				m_nRightSkillID;
	int             m_nRightListidx;
	int				m_nRightSkillLevel;
	int             m_nLimitMaxLevel;
	int             m_LastDialogNpc;
#ifndef _SERVER
	BOOL			m_MouseDown[2];
#endif
	UINT			m_nExtPoint;				// 活动点数

#ifdef _SERVER
	PLAYER_REVIVAL_POS		m_sLoginRevivalPos;	// 登入重生点位置（会存盘）
	PLAYER_REVIVAL_POS		m_sDeathRevivalPos;	// 死亡重生点（默认为登入重生点，不存盘）
	PLAYER_TOWNPORTAL_POS	m_sPortalPos;		// 记录返回点传送门位置
	BOOL			m_bUseReviveIdWhenLogin;

	INT			   m_nChangeExtPoint;			// 变动的点数
	INT            m_nChangeExtTime;            // 变动的时间
    UINT	       m_nLeftTime;
	
//  int	            m_nVipType;
#endif
	int				m_nPhysicsSkillID;		    //当前玩家的物理攻击技能
	int				m_nPeapleIdx;
	int				m_nObjectIdx;
	int				m_nPickObjectIdx;
	int				m_nPlayerIndex;				// 本实例在 Player 数组中的位置
	bool         m_IsInfindPathState;      //是否正在寻路中

public:

	bool getFindPathState() {
		return m_IsInfindPathState;
	}

	void setFindPathState(bool ver,bool synclient=false);

	UINT            m_GameLeftTime;
	BOOL            m_Isinvalid;
	int             m_nRoomFull;   //包袱是否满了
	int           __forbitEnmity;                  //是否禁止仇杀
#ifdef _SERVER
	unsigned  int   m_TempEnTerMapIndx;          // 客户端传过来的临时登陆地图    
	unsigned  int   m_TempKeyId;                 // 重生点keyiD
	int             m_IsFirstLogin;              // 是否第一次登陆
	PLAYER_EXCHANGE_POS		m_sExchangePos;
	KTimerTaskFun	m_TimerTask;
	UINT            m_CurSerVer;               //当前角色所在的第几区 
	UINT            m_TempSerVer;              //当前临时转服的区记录 
	BOOL			m_bIsQuiting;
	bool            m_isInGame;
	UINT			m_uMustSave;
	UINT			m_ulLastSaveTime;          //玩家退出游戏时，服务器的循环帧数
	UINT			m_uNextSaveTime;           //下次存档时间
	UINT  			m_dwLoginTime;             //玩家的登陆时，服务器的循环帧数
//	DWORD			m_uLastPingTime;
	char			m_AccoutName[ACC_LENGTH];	        //账号名
	//char			m_PlayerIpInfo[64];	        //账号的Ip信息
	void*			m_pStatusLoadPlayerInfo;	//加载玩家信息时用
	BYTE*			m_pCurStatusOffset;			//二进制时，记录读到指针位置了
	BOOL			m_bFinishLoading;			//完成加载
	BYTE			m_GaemSaveBuffer[128*1024];	//BYTE	m_GaemSaveBuffer[128*1024];	//存档调用-保存缓冲 64越大越增加内存的使用量
	UINT			m_nLastNetOperationTime;	//最后一次网络操作时间
	BOOL			m_bSleepMode;				//
	KList			m_PlayerWayPointList;		//经过的地点列表
	KList			m_PlayerStationList;        //进过的城市列表
	UINT			m_nViewEquipTime;			//最后一次察看他人装备的时间
	int				m_nPrePayMoney;
	int				m_nPrePayXu;
	unsigned int    m_nCreatRoleTime;           //创建角色的时间
	int  	        m_nPlayerTime;              //新称号存活的时间
	int				m_nPlayerTitleTime;
//-----------------------登录设置---------------
	/*int             m_nGameStatus;       //游戏状态
	int             m_nExchangeStatus;   //转服状态
	int             m_nReplyPingTime;	 //收ping时间
	int             m_nSendPingTime;     //发ping时间
	int             m_nNetStatus;        //网络状态
	*/
//----------------------------------------------
/*	m_pGameStatus[lnID].nGameStatus = enumPlayerBegin;
	m_pGameStatus[lnID].nPlayerIndex = 0;
	m_pGameStatus[lnID].nExchangeStatus = enumExchangeBegin;
	m_pGameStatus[lnID].nReplyPingTime = 0;
	m_pGameStatus[lnID].nSendPingTime = 0;	
*/
	unsigned int BonusType[48];

/*	enum
	{
		FF_CHAT = 0x01,
	};
	int				m_nForbiddenFlag;			// 禁止发言标志*/
	BYTE			m_nLiXian;					// 离线挂机标志
	BYTE			m_nForbFlag;                // 是否可以解开禁言了
	KplayerAuto		m_Autoplay;					// 玩家任务系统(变量)

#endif

#ifndef _SERVER
	int             m_IsTranse;                 //是否跨服
	int             m_CurGsSerIdx;              //当前服务器
    int             m_nImageNpcID;              // id npc SPR
	int				m_RunStatus;				// 是跑还是走
	DWORD			m_dwNextLevelLeadExp;		// 统率力下一级经验值
	int				m_nSendMoveFrames;			// 用于控制客户端向服务器发送移动(走或跑)协议的频率，使之不能发送大量的移动协议，减小带宽压力
	//char            szClienResult[256];         // 脚本返回值全局变量
	KTabFile nPos,nEquipItem,nToolItem,nYaBiao;
	KIniFile	m_FileDesc;
	KTabFile nCheckName,nGuaSkill;

	DWORD           m_ItemLinkDwid;
	TMissionLadderSelfInfo m_MissionData;
	TMissionLadderInfo m_MissionRank[MISSION_STATNUM];

	int             m_ExpTime;                  //  当前领取经验时间
	int             m_YinLiangTime;             //  当前领取银两时间
	int             m_JinBiTime;                //  当前领取金币时间
	int             m_ExpPiontTime;             //  当前领取点卡时间
	KplayerAuto		m_Autoplay;					// 玩家任务系统(变量)
	int				m_DebugMode;			    //调试模式

	int             m_nCurPinDao;  //临时频道
#endif

	enum
	{
		FF_CHAT = 0x01,
	};
	BYTE			m_nForbiddenFlag;			// 禁止发言标志
	KIndexNode		m_Node;
	GUID			m_Guid;                     //网络序号
	BOOL			m_bExchangeServer;          //是否跨服状态

	UINT			m_dwID;						// 玩家的32位ID
	int				m_nIndex;					// 玩家的Npc编号
	int				m_nNetConnectIdx;			// 第几个网络连接
	int				m_autoTickCount;			//自动踢人几次 还踢不掉就强制下线
	KItemList		m_ItemList;					// 玩家的装备列表
	BuySellInfo		m_BuyInfo;					// 进行的交易列表
	//NewBuySellInfo	m_NewBuyInfo;				// 进行的交易列表
	KPlayerMenuState	m_cMenuState;			// 是否处于交易或队伍开放状态
	KTrade			m_cTrade;					// 交易模块
	int				m_nAttributePoint;			// 未分配属性点
	int				m_nSkillPoint;				// 未分配技能点
	int 			m_nChestPW;		            //密码 已经给 城市地图 使用
	int				m_nStrength;				// 玩家的基本力量（决定基本伤害）
	int				m_nDexterity;				// 玩家的基本敏捷（决定命中、体力）
	int				m_nVitality;				// 玩家的基本活力（决定生命、体力）
	int				m_nEngergy;					// 玩家的基本精力（决定内力）
	int				m_nLucky;					// 玩家的基本运气（影响得到装备的好坏）
	BYTE			m_btChatSpecialChannel;		
	int				m_nUpExp;                   // 杀死敌人获得经验增加百分比

	int				m_nCurStrength;				// 玩家的当前力量（决定基本伤害）   力量
	int				m_nCurDexterity;			// 玩家的当前敏捷（决定命中、体力） 身法
	int				m_nCurVitality;				// 玩家的当前活力（决定生命、体力） 外功
	int				m_nCurEngergy;				// 玩家的当前精力（决定内力）       内功
	int				m_nCurLucky;				// 玩家的当前运气（影响得到装备的好坏）
	int				m_nTempLucky_p;				// 记录玩家的临时装备的总幸运百分比
    BYTE	        m_nVipType;                 // 会员的类型
//	int             m_nZhonji;                  // 当前重击
//	int             m_nDangge;                  // 当前档格

	int 			m_nExp;						// 当前经验值(当前等级在npc身上)
	int				m_nNextLevelExp;			// 下一级经验值
	BYTE			m_CUnlocked;                // 客户端是否解屏 屏幕

	UINT			m_dwLeadExp;				// 统率力经验值
	UINT			m_dwLeadLevel;				// 统率力等级
	char			m_PlayerName[NAME_LENGHT];           // 玩家的角色名字
	//char			m_PeiouName[32];            // 配偶名字

	KPlayerTeam		m_cTeam;					// 玩家的组队信息
	KPlayerFaction	m_cFaction;					// 玩家的门派信息

	KPlayerChat		m_cChat;                    // 玩家头顶说话显示

	KPlayerTask		m_cTask;					// 玩家任务系统(变量)
    
	KplayerGuzi		m_GuziGame;	                // 玩家的老虎机系统

	KplayerTimeInfo m_MapTimeInfo;			    // 幸运草 长效药存档

	KPlayerPK		m_cPK;						// PK关系处理
	
	KPlayerRepute	m_cRepute;					//声望

	KPlayerFuYuan	m_cFuYuan;					//福源

	KPlayerReBorn	m_cReBorn;					//转生

	KPlayerVip	    m_cVip;					    //会员

	KPlayerTong		m_cTong;					// 自己的帮会信息
	
	DWORD			m_dwDeathScriptId;			// 死亡脚本
	DWORD			m_dwDamageScriptId;			
	char			m_szTaskAnswerCanshu[MAX_ANSWERNUM][32];  //玩家 所带的 脚本参数
	char			m_szTaskAnswerFun[MAX_ANSWERNUM][32];
	int				m_nAvailableAnswerNum;                    //当前选择界面下，最大回答数。
//	bool			m_bWaitingPlayerFeedBack;	              //当前是否正等待玩家在客户端的反馈。该状态下，当前脚本不置空.类式对话选择情况
//	DWORD			m_dwOutOfDateFeedBackTime;	              //当前等待玩家与服务器脚本交互的最大限定时刻，到了这个游戏时间如果玩家仍未反馈则视作废。
//	BYTE			m_btTryExecuteScriptTimes;	              //
    //BYTE            m_nCheckWaiGua;
	//char			m_CurScriptName[128];
	int				m_nWorldStat;    // 排名
	int				m_nSectStat;     // 帮会排名
	int            m_isGm;
	DWORD			m_nGiveCallBackScript;
	BYTE			m_nGiveCallBackNum;

public:
	KPlayer();
	~KPlayer();
	int            getLastDialogNpc(){return m_LastDialogNpc;}
	void			SetPlayerIndex(int nNo);					// 设定 m_nPlayerIndex
	void			GetAboutPos(KMapPos *pMapPos);			    // 获得玩家附近一个空位置
	void			GetNewAboutPos(KMapPos *pMapPos);
	int				GetPlayerIndex();							// 获得本实例在 Player 数组中的位置
	UINT			GetPlayerID(){return m_dwID;};
	void			GetFactionName(char *lpszName, int nSize);	// 获得当前门派名称 not end
	int				GetFactionCamp();
	int				GetFactionNo();
	void			ChatFriendOnLine(DWORD dwID, int nFriendIdx);// 获得通知某好友上线了

	BOOL			ExecuteScript(char * ScriptFileName, char * szFunName, int nParam = 0);
	BOOL			ExecuteScript(char * ScriptFileName, char * szFunName, char * szParams);
	BOOL			ExecuteScript(DWORD dwScriptId, char * szFunName, char *  szParams);
	BOOL			ExecuteScript(DWORD dwScriptId,  char * szFunName, int nParam);

	BOOL			ExecuteScript2(char * ScriptFileName, char * szFunName, int nParam1 = 0, int nParam2 = 0);
	BOOL			ExecuteScript2(char * ScriptFileName, char * szFunName, char * szParams1, char * szParams2);
	BOOL			ExecuteScript2(DWORD dwScriptId, char * szFunName, char *  szParams1, char *  szParams2);
	BOOL			ExecuteScript2(DWORD dwScriptId,  char * szFunName, int nParam1, int nParam2);

	BOOL			ExecuteScript4(char * ScriptFileName, char * szFunName, int nParam1 = 0, int nParam2 = 0, int nParam3 = 0, int nParam4 = 0);
	BOOL			ExecuteScript4(char * ScriptFileName, char * szFunName, char * szParams1, char * szParams2, char * nParam3, char * szParams4);
	BOOL			ExecuteScript4(DWORD dwScriptId, char * szFunName, char *  szParams1, char * szParams2, char * szParams3, char * szParams4);
	BOOL			ExecuteScript4(DWORD dwScriptId,  char * szFunName, int nParam1, int nParam2, int nParam3 = 0, int nParam4 = 0);

	BOOL			ExecuteScriptA(char * ScriptFileName, char * szFunName, int nParam = 0,char * szCanshu=NULL,BOOL nIsGive=TRUE); //NPC对话框调用
	BOOL			ExeNoBackScript(char * ScriptFileName, char * szFunName,int nKind = 0,int nParama = 0,int nParamb = 0,int nParamc = 0,char *strInfo=NULL);
	BOOL			ExecuteScriptC(char * ScriptFileName, char * szFunName, char * szParams=NULL,char * snParams=NULL,int inParam = 0,int njb=0,int njxb=0,int inval=0,int invala=0,BOOL nIsGive=TRUE);
	
	BOOL			ExecuteScript_NEW(char * ScriptFileName, char * szFunName, int szParams=0,char * snParams=NULL,int inParam = 0,int njb=0,int njxb=0,int inval=0,int invala=0,BOOL nIsGive=TRUE);
	
	int             ExecuteScript_Direct(char * ScriptFileName, char * szFunName,int nIsGive,LPSTR cFormat, ...);

	BOOL			ExecuteScriptD(DWORD dwScriptId, char * szFunName, char *  szParams=NULL,char * snParams=NULL,int inParam = 0,int njb=0,int njxb=0,int inval=0,int invala=0,BOOL nIsGive=FALSE);
	BOOL			ExecuteScriptB(DWORD dwScriptId,  char * szFunName, int nParam=0,char * szCanshu=NULL,BOOL nIsGive=TRUE);
	BOOL			DoScript(char * ScriptCommand);				//执行某个脚本指令
	BOOL			ExeItemScriptA(char * ScriptFileName, char * szFunName, char * szParams=NULL,char * snParams=NULL,int inParam = 0,int njb=0,int njxb=0,int inGenre=0,int inDetail=0,int inParticular=0,int inStackNum=0,int inItemX=0,int inItemY=0);
	BOOL			ExeItemScript(DWORD dwScriptId, char * szFunName, char *  szParams=NULL,char * snParams=NULL,int inParam = 0,int njb=0,int njxb=0,int inGenre=0,int inDetail=0,int inParticular=0,int inStackNum=0,int inItemX=0,int inItemY=0);
    void            myPrintLuaStackA(lua_State *L);  //调试
	void			ChangeCurStrength(int nData);			// 改变当前力量(当 nData 小于 0 时，减少)
	void			ChangeCurDexterity(int nData);			// 改变当前敏捷(当 nData 小于 0 时，减少)
	void			ChangeCurVitality(int nData);			// 改变当前活力(当 nData 小于 0 时，减少)
	void			ChangeCurEngergy(int nData);			// 改变当前精力(当 nData 小于 0 时，减少)
//	BOOL			ExecuteScriptC(char * ScriptFileName);
	void			Release();
	void			Active(int  _minLimitLevel, int  _maxLimitLevel);								// 玩家每次游戏循环都需要处理的东西
	void			ProcessMsg(KWorldMsgNode *lpMsg);		// 处理世界消息，转为NPC命令
    void            ReSkillEnhance();
	LPSTR			GetPlayerName() { return m_PlayerName; };
	int             GetLimitMaxLevel(){return m_nLimitMaxLevel;};
	void            SetLimitMaxLevel(int ver){m_nLimitMaxLevel=ver;};
	BOOL			NewPlayerGetBaseAttribute(int Series);	// 新玩家登陆时根据五行属性产生 力量 敏捷 活力 精力 四项数值
	void			AddBaseLucky(int nData);				// 增加基本运气
#ifdef _SERVER

	void            CheckItemAddtoEquipment(int nRoomCount);
//	void			AddExp(int nExp, int nTarLevel);		// 增加经验(原始数据，还未经过处理)
	void	        AddExp(int nExp, int nTarLevel,int nGoldType);
	void			AddSelfExp(int nExp, int nTarLevel,int nGoldType);	// 增加经验(不需要再经过队伍分配的处理，但需要考虑被砍死npc的等级)
	void			AddSkillExp(int nExp);
#endif
	void			DirectAddExp(int nExp);					// 直接增加经验值，不考虑其他因素
	void			LevelUp();								// 升一级
	void			SetLevel(int nLevel);
	void			AddLeadExp(int nExp);					// 增加统率力经验
	void			UpdataCurData(int nIsLoign=0,int nUnEquip=0);
	void			ReCalcEquip();		    // 重新计算身上的装备
	void			ReCalcState(int nIsLoign=0);							// 重新计算身上的状态
	BOOL            CheckThisItem(int nSetings,char *nKey);
	void			ChangePlayerCamp(int nCamp);			// 改变玩家阵营
	void 			Revive(int nType);						// 重生	
	void			GetEchoDamage(int* nMin, int* nMax, int* nAr, int nType);// 获取界面需要显示的伤害值
	BOOL			CheckTrading();
	void			SetFirstDamage();   //设置基本伤害
	void			SetBaseAttackRating();
	void			SetBaseDefence();
	void			SetBaseResistData();
	void			SetBaseSpeedAndRadius();
	int				GetPhysicsSkillId(){return m_nPhysicsSkillID;};
	void			SetPhysicsSkillId(int nPhysicsSkillID)
	{
		if (nPhysicsSkillID <= 0) return ;
		ISkill * pISkill =  g_SkillManager.GetSkill(nPhysicsSkillID, 1);
		if (!pISkill) 
            return ;
		
        if (nPhysicsSkillID > 0 && pISkill->IsPhysical())
			m_nPhysicsSkillID = nPhysicsSkillID;
	};
	int				GetLeftSkill(){return m_nLeftSkillID;};
	int				GetLeftSkillListidx(){return m_nLeftListidx;};
	int				GetRightSkill(){return m_nRightSkillID;};
	int			    GetRightSkillListidx(){return m_nRightListidx;};

	void            SetExtGameTime(DWORD nPoint);
#ifndef _SERVER
	int				GetTargetNpc() { return m_nPeapleIdx; };
	int				GetTargetObj() { return m_nObjectIdx; };
	void			SetTargetNpc(int n) { m_nPeapleIdx = n; };
	void			SetTargetObj(int n) { m_nObjectIdx = n; };
	void			FindSelectNpc(int x, int y, int nRelation);
	void			FindSelectObject(int x, int y);
	void			Walk(int nDir, int nSpeed);	
	void			TurnLeft();
	void			TurnRight();
	void			TurnBack();
	void			DrawSelectInfo();
	BOOL			ConformIdx(int nIdx);	
//	void			GetEchoDamage(int* nMin, int* nMax, int nType);// 获取界面需要显示的伤害值
	void			ProcessInputMsg(UINT uMsg, WPARAM wParam, LPARAM lParam);// 处理键盘鼠标消息
	void			RecvSyncData();								// 接收同步数据
    BOOL            ClientExeItemScript(char *nScriptPath,char *nFunName,int nPaparm,int ninVal,char *nstrVala);
	void			ApplyTeamInfo(DWORD dwNpcID);				// 向服务器申请查询某个npc所在队伍的信息
	void	        ApplyRoomInfo(int nIsFull);
	void			ApplySelfTeamInfo();						// 向服务器申请查询玩家自身的队伍情况
	BOOL			ApplyCreateTeam();//char *lpszTeamName);		// 玩家向服务器申请创建队伍
	BOOL			ApplyTeamOpenClose(BOOL bFlag);				// 队长向服务器申请开放、关闭队伍是否允许加入成员状态
	void			ApplyAddTeam(int nNpcIndex);				// 玩家向服务器申请加入某个队伍
	void			AcceptTeamMember(DWORD dwNpcID);			// 玩家通知服务器接受某个npc为队伍成员
	void			TeamDropApplyOne(DWORD dwNpcID);			// 队长删除加入队伍申请列表中的某个npc
	void			LeaveTeam();								// 通知服务器本玩家离开队伍
	void			TeamKickMember(DWORD dwNpcID);				// 队长通知服务器踢除某个队员
	void			ApplyTeamChangeCaptain(DWORD dwNpcID);		// 队长向服务器申请把自己的队长身份交给别的队员
	void			ApplyTeamDismiss();							// 队长向服务器申请解散队伍
	void			ApplySetPK(BOOL bPK);						// 玩家向服务器申请打开、关闭pk开关
	void			ApplyFactionData();							// 玩家向服务器申请门派数据
	void			SendChat(KUiMsgParam *pMsg, char *lpszSentence);// 客户端发送聊天语句给服务器
	void			ApplyAddBaseAttribute(int nAttribute, int nNo);// 队长向服务器申请增加四项属性中某一项的点数(0=Strength 1=Dexterity 2=Vitality 3=Engergy)
	BOOL			ApplyAddSkillLevel(int nSkillID, int nAddPoint);// 向服务器申请某个技能升级
	BOOL			ApplyUseItem(int nItemID, ItemPos SrcPos);	// 向服务器申请使用某个物品（鼠标右键点击该物品）
	BOOL            ApplyAutoMoveItem(int nItemID, ItemPos SrcPos,int destPlace);
	void	        PickUpItem(int nObjIndex,int rid);
    void	        PickUpMoney(int nObjIndex,int rid);
	void			PickUpObj(int nObjIndex);					// 客户端鼠标点击obj检起某个物品，向服务器发消息
	void			ObjMouseClick(int nObjIndex);				// 客户端鼠标点击obj，向服务器发消息
	void			MoveItem(ItemPos DownPos, ItemPos UpPos,int nIsComp=0);	// DownPos 不能是手，UpPos 必须是手
	void			MoveSkill(ItemPos DownPos, ItemPos UpPos);
	int				ThrowAwayItem();
	int	            AutoAwayItem(DWORD nItmeDwIdx);  //---------------外挂丢东西-------------------
	void			ChatAddFriend(int nPlayerIdx);				// 客户端通过别人的添加聊天好友的申请
	void			ChatRefuseFriend(int nPlayerIdx);			// 客户端拒绝别人的添加聊天好友的申请
	void			TradeApplyOpen(char *lpszSentence, int nLength);
	void			TradeApplyClose();
	void			TradeApplyStart(int nNpcIdx);
	BOOL			TradeMoveMoney(int nMoney);					// 交易时输入自己的钱	
	void			TradeDecision(int nDecision);				// if nDecision == 0 推出交易  if nDecision == 1 确定交易  if nDecision == 2 取消交易确定
	void			TradeApplyLock(int nLockOrNot);				// 交易锁定或取消锁定

	void			SetChatCurChannel(int nChannelNo);			// 设定当前聊天频道
	void			TeamInviteAdd(DWORD dwNpcID);				// 邀请加入队伍

	void			SetLeftSkill(int nSkillID,BOOL nIsNoLogin=FALSE);
	void			SetRightSkill(int nSkillID);
	void	        SetShiquSkill(int nSkillID);
	void			UpdateWeaponSkill();
//	int				GetLeftSkill(){return m_nLeftSkillID;};
//	int				GetRightSkill(){return m_nRightSkillID;};
	void			SetDefaultImmedSkill();
	void			s2cApplyAddTeam(BYTE* pProtocol);			// 收到服务器通知有人申请加入队伍
	void			s2cTradeChangeState(BYTE* pMsg);
	void			s2cTradeMoneySync(BYTE* pMsg);
	void			s2cTradeDecision(BYTE* pMsg);				// 收到服务器通知交易完成或取消
	void			SyncCurPlayer(BYTE* pMsg);
	void			s2cLevelUp(BYTE* pMsg);
	void			s2cGetCurAttribute(BYTE* pMsg);
	void			s2cSetExp(int nExp);
	void			s2cSyncMoney(BYTE* pMsg);
	void			s2cSyncXu(BYTE* pMsg);
	void			s2cTradeApplyStart(BYTE* pMsg);
	void			CheckObject(int nIdx);
#endif
	UINT  			GetExtPoint();
	BOOL			PayExtPoint(int nPoint);
#ifdef _SERVER
	void			SetExtPoint(DWORD nPoint, LONG nChangePoint);
	void            SetLeftTineAndVipType(DWORD nLeftTime,int nVipType,LONG nChangeExtTime);

	UINT           GetLeftTime();
	void            GetTypeData(int nType);

	void			SendMSGroup();
	void			SendMSRank(TMissionLadderSelfInfo* SelfData, TMissionLadderInfo* RankData);


	int             GetVipType();
	BOOL			AddAndPayExtPoint(long nPoint);
	BOOL			AddAndPayExtTime(long nTime);
	long			GetExtPointChanged();
	long			GetExtTimeChanged();
	void            UseToolSkills(int sKillID,int sLevel,int nNpcIdx,int nDesNpcIdx=0,KMagicAttrib *pData=NULL,BOOL nIfMagic=1,int nTime=0,int nBeiVal=1);
	void            ResSetNpcTitle();
	void            SetPlayerTitleSkills();
	int				TimeToHour(int time1, int time2);
	void			RepairItem(DWORD dwItemID);
	void			BreakItem(DWORD dwItemID, int nNum);
	int				ComItem(int nIdx1,int nIdx2,int nIdx3,int nKind);
	int				ComDiyItem(int nId1,int nId2,int nId3,int nKind,int nMin = 0);
	void			SetCurLeftSkillID(int nSillID);
	BOOL			PrePay(int nMoney);
	BOOL			PrePayXu(int nXu);
	void			SetLastNetOperationTime(DWORD nTime);
	int				FindAroundPlayer(DWORD dwNpcID);		// 寻找玩家周围的某个指定npc id的player index
	int				FindAroundNpc(DWORD dwNpcID);			// 寻找玩家周围的某个指定npc id的npc index
	int				FindAroundNpcID(DWORD dwNpcID, int nDist =0);
	BOOL			CheckPlayerAround(int nPlayerIdx);		// 判断某玩家是否在周围
	BOOL			IsExchangingServer();
	void			TobeExchangeServer(DWORD dwMapID, int nX, int nY,BOOL isChange);
//	void			UpdateEnterGamePos(DWORD dwSubWorldID, int nX, int nY, int nFightMode);
	BOOL			IsWaitingRemove();
	BOOL			IsLoginTimeOut();
	void			WaitForRemove();
	void			LoginTimeOut();
	void			UseTownPortal();
	void			UseTownPortalScript();
	void			BackToTownPortal();
	void			GetLoginRevivalPos(int *lpnSubWorld, int *lpnMpsX, int *lpnMpsY);		// 获取玩家登入重生点位置
	void			GetDeathRevivalPos(int *lpnSubWorld, int *lpnMpsX, int *lpnMpsY);		// 获取玩家死亡重生点位置
	void			SetRevivalPos(int nSubWorld,int nRevalId);	    //设定玩家进入游戏重生点ID
	void            SetDeathRevivalPos(int nSubWorld,int nXpos,int nYpos);//设置玩家的临时死亡复活点
	BOOL			Save(BOOL nIsShow=FALSE);									// 保存玩家数据
	BOOL			CanSave();
	void			ProcessUser();
	BOOL			SendSyncData(int &nStep, unsigned int &nParam);	// 发送同步数据
	BOOL			SendSyncData_Skill();					// 发送同步数据 - 技能
	void			SendCurNormalSyncData();				// 发送平时给自己的同步数据
	void			SetChatForbiddenFlag(int nFlag)
	{//设置禁言  
	        m_nForbiddenFlag = nFlag;
	};

	BOOL			AddFaction(char *lpszFactionName);		// 加入门派
	BOOL			AddFaction(int nFactionID);				// 加入门派
	BOOL			LeaveCurFaction();						// 离开门派
	BOOL			CurFactionOpenSkill(int nLevel);		// 开放当前门派某个等级的技能

	void			TaskClearTempVal();						// 清空任务临时变量
	int				TaskGetSaveVal(int nNo);				// 得到任务完成情况
	void			TaskSetSaveVal(int nNo, BOOL bFlag);	// 设定任务完成情况
	int				TaskGetClearVal(int nNo);				// 得到任务临时过程控制变量值
	void			TaskSetClearVal(int nNo, int nVal);		// 设定任务临时过程控制变量值

	//void			SetTimeTaskTime(DWORD time){m_dwTimeTaskTime = time;};
	void			BuyItem(BYTE* pProtocol);
	void			SellItem(BYTE* pProtocol);
	void			GopItemPlayer(BYTE* pProtocol);
	void			QuitGame(int nQuitType);				// 退出游戏
	void			S2CSendTeamInfo(BYTE* pProtocol);		// 收到客户端查询某个npc组队信息的申请后，向这个客户端发送队伍信息
	void			SendSelfTeamInfo();						// 服务器向客户端发送队伍信息
	BOOL			CreateTeam(BYTE* pProtocol);			// 收到客户端请求创建一支队伍
	BOOL			SetTeamState(BYTE* pProtocol);			// 收到客户端请求开放、关闭本player队伍是否允许队员加入状态
	BOOL			S2CSendAddTeamInfo(BYTE* pProtocol);	// 收到客户端请求加入一支队伍
	BOOL			AddTeamMember(BYTE* pProtocol);			// 队长通知服务器接受某个npc为队伍成员
	void			LeaveTeam(BYTE* pProtocol);				// 收到客户端队员通知离开队伍
	void			TeamKickOne(BYTE* pProtocol);			// 收到客户端队长通知踢出某个队员
	void			TeamChangeCaptain(BYTE* pProtocol);		// 收到客户端队长通知把队长身份交给某个队员
	void			TeamDismiss(BYTE* pProtocol);			// 收到客户端队长请求解散队伍
	void			SetPK(BYTE* pProtocol);					// 收到客户端请求设定PK状态
	void			SendFactionData(BYTE* pProtocol);		// 收到客户端请求获得门派数据
	void			ServerSendChat(BYTE* pProtocol);		// 收到客户端发来的聊天语句
	void			AddBaseAttribute(BYTE* pProtocol);		// 收到客户端要求增加基本属性点(0=Strength 1=Dexterity 2=Vitality 3=Engergy)
	void			ResetBaseAttribute(BYTE* pProtocol);	// ham moi
	void			IncSkillExp(int nSkillId, int nAddExp); // ham moi
	void			AddSkillPoint(BYTE* pProtocol);			// 收到客户端要求增加某个技能的点数
	BOOL			ServerPickUpItem(BYTE* pProtocol);		// 收到客户端消息鼠标点击某个obj拣起装备或金钱
	void			EatItem(BYTE* pProtocol);				// 收到客户端消息吃药
	void			AutoItem(BYTE* pProtocol);				// 收到客户端消息吃药
	int 			ServerMoveItem(BYTE* pProtocol);		// 收到客户端消息移动物品
	void			ServerThrowAwayItem(BYTE* pProtocol);	// 收到客户端消息丢弃物品
	BOOL            CheckGmName();                          // 检测是否是GM账号
	BOOL            CheckCanTradeName();
	void			ChatSetTakeChannel(BYTE* pProtocol);	// 收到客户端消息设定聊天订阅频道
	void			ChatTransmitApplyAddFriend(BYTE* pProtocol);// 收到客户端请求转发聊天添加好友信息
	BOOL			ChatAddFriend(BYTE* pProtocol);			// 收到客户端消息添加聊天好友
	void			ChatRefuseFriend(BYTE* pProtocol);		// 收到客户端消息拒绝添加聊天好友
	void			ChatResendAllFriend(BYTE* pProtocol);
	void			ChatSendOneFriendData(BYTE* pProtocol);
	void			ChatDeleteFriend(BYTE* pProtocol);		// 收到客户端申请删除某个聊天好友
	void			ChatRedeleteFriend(BYTE* pProtocol);	// 收到客户端申请再次删除某个聊天好友
	void			TradeApplyOpen(BYTE* pProtocol);		// 收到客户端申请进入待交易状态
	void			TradeApplyClose(BYTE* pProtocol);		// 收到客户端申请取消待交易状态
	void			TradeApplyStart(BYTE* pProtocol);		// 收到客户端申请开始交易
	void			TradeMoveMoney(BYTE* pProtocol);		// 收到客户端申请交易中money的改变
	void			TradeDecision(BYTE* pProtocol);			// 收到客户端申请交易确定或取消
	void            KickOutGame();                          // 通知玩家退出游戏
	void			c2sTradeReplyStart(BYTE* pProtocol);
	void			SyncTradeState();						// 给交易双方的客户端发送交易状态信息
	void			CreateRefItem(BYTE* pProtocol);
	void			RemoveRefItem(BYTE* pProtocol);

	void			TradeStart(BYTE* pProtocol);



	PLAYER_REVIVAL_POS* GetDeathRevivalPos() 
	{
		return &m_sDeathRevivalPos;
	};
	BOOL			IsUseReviveIdWhenLogin() { return m_bUseReviveIdWhenLogin; };
	void			SetLoginType(BOOL bUseReviveId) { m_bUseReviveIdWhenLogin = bUseReviveId; };

	void			SetChestLock(BOOL unlockFlag);
	void            setMobileClient(bool Ver){__isMobileClent = Ver;};            
	bool            getMobileClient(){return __isMobileClent;}

	void            setMobileClientLanguage(bool Ver){__MobileClentLanguage = Ver;};            
	int             getMobileClientLanguage(){return __MobileClentLanguage;}

	BOOL			CheckChestPW(int iPassword);
	void			SetChestPW(int newPW);
	void			SetNewPW(int resetPW);
	//int             GetIsCitY(){return m_nChestPW;};
	void			Market(int nId,int shopID);
	void			String(int nbutton); //stringbox
	void	        SAutoplay(int nbutton,int nBtyeVal,int nBtyeVala); //挂机
	void			script(int nId,int nXuHao);
	void			pkvalue(int nId);	
	BOOL			CreateTong(int nCamp, char *lpszTongName, char* lpszTitle);
    void			SendEquipItemInfo(int nTargetPlayer, int nPrcess = 0, BOOL bUpdate = FALSE);// 发送自己装备在身上的装备信息给别人看
	void			SendSellItemInfo(int nTargetPlayer, int nPrcess, BOOL bUpdate = FALSE);	// 发送自己设置的出售信息给别人看
	void			SendSellItemCount(int nIndex);
	int				GetTradeCount();
	int 			SendLianItemInfo(int nTargetPlayer, int nItemDwID,int nPrcess=0, BOOL bUpdate= FALSE,char *nShopName=NULL);	// 发送链接信息给别人看
	int 			SendLianItemInfo(const void *nData);
	

#endif

private:
//	void			CalcCurStrength();						// 计算当前力量
//	void			CalcCurDexterity();						// 计算当前敏捷
//	void			CalcCurVitality();						// 计算当前活力
//	void			CalcCurEngergy();						// 计算当前精力
	void			CalcCurLucky();							// 计算当前运气
	void			LevelAddBaseLifeMax();					// 等级升一级后增加最大生命点
	void			LevelAddBaseManaMax();					// 等级升一级后增加最大内力点
	void			LevelAddBaseStaminaMax();				// 等级升一级后增加最大体力点

	void			SendFactionData();						// 向客户端发送门派数据

	void			SyncCurrentBaseAttriibute(int type,int attribute,int curAttribute);	// ham moi
// V9 port: LuaAddStrg/AddDex/AddVit/AddEng call these four directly
public:
	void			AddBaseStrength(int nData);				// 增加基本力量  ---力量
	void			AddBaseDexterity(int nData);			// 增加基本敏捷  ---身法
	void			AddBaseVitality(int nData);				// 增加基本活力---外功
	void			AddBaseEngergy(int nData);				// 增加基本精力---内功
private:
	
	void			ResetBaseStrength(int nData);			// 增加基本力量
	void			ResetBaseDexterity(int nData);			// 增加基本敏捷
	void			ResetBaseVitality(int nData);			// 增加基本活力
	void			ResetBaseEngergy(int nData);			// 增加基本精力
	
	void			SetNpcPhysicsDamage();					// 由当前力量计算对应npc的物理伤害(PhysicsDamage)
	void			SetNpcAttackRating();					// 由当前敏捷计算对应npc的攻击命中率(AttackRating)
	void			SetNpcDefence();						// 由当前敏捷计算对应npc的防御力
//	void			SetNpcWalkSpeed();						// 由当前敏捷计算对应npc的行走速度
//	void			SetNpcRunSpeed();						// 由当前敏捷计算对应npc的跑步速度

#ifndef _SERVER
	void			ProcessMouse(int x, int y, int Key, MOUSE_BUTTON nButton);
	void			OnButtonUp(int x,int y,MOUSE_BUTTON nButton);				// 处理鼠标键抬起
	void			OnButtonDown(int x,int y,int Key,MOUSE_BUTTON nButton);		// 处理鼠标键按下
	void			OnButtonMove(int x,int y,int Key,MOUSE_BUTTON nButton);		// 处理鼠标键按下后移动
	void			OnMouseMove(int x,int y);									// 处理鼠标移动
	int				NetCommandPlayerTalk(BYTE* pProtocol);

	//Question:为单机测试版使用
	friend		    int LuaInitStandAloneGame(Lua_State * L);
#endif


// 豆豆的东西
private:
	void			S2CExecuteScript(char * ScriptName, char * szParam);

#ifdef _SERVER
//数据库模块函数-----------------
private:
	bool            __isMobileClent;
	int             __MobileClentLanguage;
	int            __forBitTrade;
	int            __forBitUseTownPortal;
	int				LoadPlayerBaseInfo(BYTE * pRoleBuffer, BYTE * &pRoleBaseBuffer, unsigned int &nParam );
	int				LoadPlayerItemList(BYTE * pRoleBuffer, BYTE * &pItemBuffer, unsigned int &nParam );
	int				LoadPlayerFightSkillList(BYTE * pRoleBuffer, BYTE * &pFightBuffer, unsigned int &nParam);
	int				LoadPlayerLifeSkilllList(BYTE * pRoleBuffer, BYTE * &pSkillBuffer, unsigned int &nParam);
	int				LoadPlayerFriendList(BYTE * pRoleBuffer, BYTE * &pFriendBuffer, unsigned int &nParam);
	int				LoadPlayerTaskList(BYTE * pRoleBuffer, BYTE * &pTaskBuffer, unsigned int &nParam);
	int				SavePlayerBaseInfo(BYTE * pRoleBuffer);
	int				SavePlayerItemList(BYTE * pRoleBuffer,DWORD dwBufferLen);
	int				SavePlayerFightSkillList(BYTE * pRoleBuffer);
	int				SavePlayerLifeSkilllList(BYTE * pRoleBuffer);
	int				SavePlayerFriendList(BYTE * pRoleBuffer,DWORD dwBufferLen,BOOL nIsShow=FALSE);
	int				SavePlayerTaskList(BYTE * pRoleBuffer);
#endif

public:
	void          setForBitUseTownPortal(int ver){__forBitUseTownPortal =ver;}
	int             getForBitUseTownPortal(){return __forBitUseTownPortal;}
	void          setforBitTrade(int ver){__forBitTrade =ver;}
	void			SetNpcDamageAttrib();
	void			DoScriptAction(PLAYER_SCRIPTACTION_SYNC * pUIInfo); //通知该客户端显示某个UI界面
	void			ProcessPlayerSelectFromUI(BYTE* pProtocol);			// 处理当玩家从选择菜单选择某项时的操作
#ifndef _SERVER
	void			DialogNpc(int nIndex,BOOL nIsServer=TRUE);
	void            OpenBaiTan(DWORD nIndwId);
	void			OnSelectFromUI(PLAYER_SELECTUI_COMMAND * pSelectUI, UIInfo eUIInfo);//当玩家从选择框中选择某项后，将向服务器发送			
	void			OnScriptAction(PLAYER_SCRIPTACTION_SYNC * );
#endif
#ifdef _SERVER
	void			RestoreLiveData();						//重生后恢复玩家的基本数据
	void			SetTimer(DWORD nTime, int nTimeTaskId);//时间任务脚本，开启计时器
	void			CloseTimer();							//关闭时间计时器
	BOOL            SavePlayerData(bool bUnLock);
	int				AddDBPlayer(char * szPlayerName, int sex, DWORD * pdwID );
	int				LoadDBPlayerInfo(BYTE * pPlayerInfo,  int &nStep, unsigned int &nParam);
	BOOL			GetNewPlayerFromIni(KIniFile * pIniFile, BYTE * pRoleBuffer);
	int				UpdateDBPlayerInfo(BYTE * pPlayerInfo,DWORD dwBufferLen,BOOL nIsShow=FALSE);  //存档
	int				DeletePlayer(char * szPlayerName = NULL);//注意：本函数是清除玩家帐号！！！，不能乱用
	void			LaunchPlayer();
	BOOL			Pay(int nMoney);
	BOOL			PayXu(int nXu);
	BOOL			Earn(int nMoney);
	BOOL			EarnXu(int nXu);
	BOOL            GetCurXu();
	int				GetXu();
	int				GetMoney();

	void			DialogNpc(BYTE * pProtocol);
	int             SetReBornByte(int nIntValue,int nByteNumber,int nByteValue);
    int             GetReBornByte(int nIntValue,int nByteNumber);
//	int				AddTempTaskValue(void* pData);
#endif
};

#ifdef TOOLVERSION
  extern CORE_API KPlayer	*Player;//Player[MAX_PLAYER];
#else
  extern KPlayer	*Player;//Player[MAX_PLAYER];
#endif

#endif //KPlayerH


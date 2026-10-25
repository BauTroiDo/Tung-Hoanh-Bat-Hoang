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
#define	MAX_PLAYER		500   // 1200 256
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

#define		BASE_FIRE_RESIST_MAX			150//275  //ֵ
#define		BASE_COLD_RESIST_MAX			150//     //ֵ
#define		BASE_POISON_RESIST_MAX			150//     //ֵ
#define		BASE_LIGHT_RESIST_MAX			150//     //׷ֵ
#define		BASE_PHYSICS_RESIST_MAX			150//     //շֵ
#define		BASE_FANGYU_ALL_MAX			    75        //ԭʼ
#define		defPLAYER_SAVE_TIMEOUT			18*60*5   // 浵ʱ


enum UIInfo //ű֪ͨʾĽ
{
	UI_SELECTDIALOG,
	UI_TRADEDIALOG,
	UI_TALKDIALOG,
	UI_NOTEINFO,
	UI_MSGINFO,      //ðϢ
	UI_NEWSINFO,     //
	UI_PLAYMUSIC,
	UI_OPENTONGUI,
};

// λϢ
typedef struct PLAYER_REVIVAL_POS_DATA
{
	int				m_nSubWorldID;		// ͼ
	int				m_ReviveID;			// 
	int				m_nMpsX;			// ͼλ x
	int				m_nMpsY;			// ͼλ y
} PLAYER_REVIVAL_POS;

typedef struct
{
	int				m_nSubWorldId;		// ID
	int				m_nTime;			// űʱ
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

//ϻṹ 
struct KplayerGuzi
{
	BOOL nIsOpen;
	int  nTpye;
	int  nYingL;
	int  nJinBi;
	int  nDianShu;
	int  nBeiLv;
};




//ҹһṹ
#ifdef _SERVER
struct KplayerAuto
{
	int  nIsNoBlue;
	int  nIsOpenPicjkup;
	int  nAutoMoney;   //Ǯ
	int  nAutoEques;   //װ
	int  nAutoMoneyAndEques;
	int  nAutoTuPu;    //ͼ
	BYTE nAutoTool;   //
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
	}; //ӵ

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
	int nOldIndex;   //ֵNPC
	int nHideNpc;
	int nHidePlayer;
	int nHideMiss;
	int nAutoMoney;
	int nAutoEques;
	int nAutoMoneyAndEques;
	BYTE nShuXingNum; //
	int nAutoTuPu;
	BYTE nAutoTool;
	BYTE nAutoOther;  //װ
	BYTE nIsAuToFenJie;
	BOOL nIsYaBiao;
	BYTE nCurNum;
	BYTE nIsHaveDo;
	BYTE nIsJiaoBen;  //Ƿ¼ƽű
	BYTE nAutoModel;
	BYTE nCurStaus;  //ǰ״̬
	BYTE nIsOpenZiMu;//ǷԶŽĸ
	BYTE nIsAutoSay; //ǷԶ
	BYTE nIsAutoNuQi;
	BYTE nIsAutoUseItem;//ԶʹƷ
	BYTE nIsShowPath;   //Ƿʾ·
	BYTE nIsNoBlue;     //Ƿ񲻹̹
	BYTE nIsNoYellow;   //Ƿ񲻹ƹ
	BYTE nIsNoRed;      //Ƿ񲻹
	BOOL nIsFollow;     //Ƿӳ
	BOOL nIsFell;
	int  nAttackNum;
    Kteam nCteam;
	int  nNpcCurlife;//ݵNPC
	int  nShiQuNum;
	int  nVal;
	int  nAutoTime;  //ȡȡʱ ʱ ȡȡ 򹥻
	int  ngMoneyLoop;
	int  ngItemLoop;
	int  ngTuzhiLoop;
	int  ngToolLoop;
	int  ngAttackLoop;
	Kskill  nAutoSkill[4];
	KLRskill nAutoLRskill;
	BOOL nIsReTurn;//Ƿڻس
	BOOL nIsUseSkill;
	int  nNpcDis;  //ѰַΧ
};
#endif

class KIniFile;

class CORE_API KPlayer
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
	UINT			m_nExtPoint;				// 

#ifdef _SERVER
	PLAYER_REVIVAL_POS		m_sLoginRevivalPos;	// λạ̃
	PLAYER_REVIVAL_POS		m_sDeathRevivalPos;	// 㣨ĬΪ㣬̣
	PLAYER_TOWNPORTAL_POS	m_sPortalPos;		// ¼ص㴫λ
	BOOL			m_bUseReviveIdWhenLogin;

	INT			   m_nChangeExtPoint;			// 䶯ĵ
	INT            m_nChangeExtTime;            // 䶯ʱ
    UINT	       m_nLeftTime;
	
//  int	            m_nVipType;
#endif
	int				m_nPhysicsSkillID;		    //ǰҵ
	int				m_nPeapleIdx;
	int				m_nObjectIdx;
	int				m_nPickObjectIdx;
	int				m_nPlayerIndex;				// ʵ Player еλ
	bool         m_IsInfindPathState;      //ǷѰ·

public:

	bool getFindPathState() {
		return m_IsInfindPathState;
	}

	void setFindPathState(bool ver,bool synclient=false);

	UINT            m_GameLeftTime;
	BOOL            m_Isinvalid;
	int             m_nRoomFull;   //Ƿ
	int           __forbitEnmity;                  //Ƿֹɱ
#ifdef _SERVER
	unsigned  int   m_TempEnTerMapIndx;          // ͻ˴ʱ½ͼ    
	unsigned  int   m_TempKeyId;                 // keyiD
	int             m_IsFirstLogin;              // Ƿһε½
	PLAYER_EXCHANGE_POS		m_sExchangePos;
	KTimerTaskFun	m_TimerTask;
	UINT            m_CurSerVer;               //ǰɫڵĵڼ 
	UINT            m_TempSerVer;              //ǰʱת¼ 
	BOOL			m_bIsQuiting;
	bool            m_isInGame;
	UINT			m_uSaveGeneration;
	unsigned __int64 m_uRoleSaveRevision; // SQLite revision bound to login session
	bool m_bRoleSaveRevisionLoaded;
	UINT			m_uMustSave;
	UINT			m_ulLastSaveTime;          //˳Ϸʱѭ֡
	UINT			m_uNextSaveTime;           //´δ浵ʱ
	UINT  			m_dwLoginTime;             //ҵĵ½ʱѭ֡
//	DWORD			m_uLastPingTime;
	char			m_AccoutName[ACC_LENGTH];	        //˺
	//char			m_PlayerIpInfo[64];	        //˺ŵIpϢ
	void*			m_pStatusLoadPlayerInfo;	//Ϣʱ
	BYTE*			m_pCurStatusOffset;			//ʱ¼ָλ
	BOOL			m_bFinishLoading;			//ɼ
	BYTE			m_GaemSaveBuffer[128*1024];	//BYTE	m_GaemSaveBuffer[128*1024];	//浵-滺 64ԽԽڴʹ
	UINT			m_nLastNetOperationTime;	//һʱ
	BOOL			m_bSleepMode;				//
	KList			m_PlayerWayPointList;		//ĵصб
	KList			m_PlayerStationList;        //ĳб
	UINT			m_nViewEquipTime;			//һβ쿴װʱ
	int				m_nPrePayMoney;
	int				m_nPrePayXu;
	unsigned int    m_nCreatRoleTime;           //ɫʱ
	int  	        m_nPlayerTime;              //³ƺŴʱ
	int				m_nPlayerTitleTime;
//-----------------------¼---------------
	/*int             m_nGameStatus;       //Ϸ״̬
	int             m_nExchangeStatus;   //ת״̬
	int             m_nReplyPingTime;	 //pingʱ
	int             m_nSendPingTime;     //pingʱ
	int             m_nNetStatus;        //״̬
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
	int				m_nForbiddenFlag;			// ֹԱ־*/
	BYTE			m_nLiXian;					// ߹һ־
	BYTE			m_nForbFlag;                // ǷԽ⿪
	KplayerAuto		m_Autoplay;					// ϵͳ()

#endif

#ifndef _SERVER
	int             m_IsTranse;                 //Ƿ
	int             m_CurGsSerIdx;              //ǰ
    int             m_nImageNpcID;              // id npc SPR
	int				m_RunStatus;				// ܻ
	DWORD			m_dwNextLevelLeadExp;		// ͳһֵ
	int				m_nSendMoveFrames;			// ڿƿͻƶ(߻)ЭƵʣʹܷ֮ʹƶЭ飬Сѹ
	//char            szClienResult[256];         // űֵȫֱ
	KTabFile nPos,nEquipItem,nToolItem,nYaBiao;
	KIniFile	m_FileDesc;
	KTabFile nCheckName,nGuaSkill;

	DWORD           m_ItemLinkDwid;
	TMissionLadderSelfInfo m_MissionData;
	TMissionLadderInfo m_MissionRank[MISSION_STATNUM];

	int             m_ExpTime;                  //  ǰȡʱ
	int             m_YinLiangTime;             //  ǰȡʱ
	int             m_JinBiTime;                //  ǰȡʱ
	int             m_ExpPiontTime;             //  ǰȡ㿨ʱ
	KplayerAuto		m_Autoplay;					// ϵͳ()
	int				m_DebugMode;			    //ģʽ

	int             m_nCurPinDao;  //ʱƵ
#endif

	enum
	{
		FF_CHAT = 0x01,
	};
	BYTE			m_nForbiddenFlag;			// ֹԱ־
	KIndexNode		m_Node;
	GUID			m_Guid;                     //
	BOOL			m_bExchangeServer;          //Ƿ״̬

	UINT			m_dwID;						// ҵ32λID
	int				m_nIndex;					// ҵNpc
	int				m_nNetConnectIdx;			// ڼ
	int				m_autoTickCount;			//Զ˼ ߲ǿ
	KItemList		m_ItemList;					// ҵװб
	BuySellInfo		m_BuyInfo;					// еĽб
	//NewBuySellInfo	m_NewBuyInfo;				// еĽб
	KPlayerMenuState	m_cMenuState;			// Ƿڽ׻鿪״̬
	KTrade			m_cTrade;					// ģ
	int				m_nAttributePoint;			// δԵ
	int				m_nSkillPoint;				// δ似ܵ
	int 			m_nChestPW;		            // Ѿ еͼ ʹ
	int				m_nStrength;				// ҵĻ˺
	int				m_nDexterity;				// ҵĻݣС
	int				m_nVitality;				// ҵĻ
	int				m_nEngergy;					// ҵĻ
	int				m_nLucky;					// ҵĻӰõװĺû
	BYTE			m_btChatSpecialChannel;		
	int				m_nUpExp;                   // ɱ˻þӰٷֱ

	int				m_nCurStrength;				// ҵĵǰ˺   
	int				m_nCurDexterity;			// ҵĵǰݣС 
	int				m_nCurVitality;				// ҵĵǰ ⹦
	int				m_nCurEngergy;				// ҵĵǰ       ڹ
	int				m_nCurLucky;				// ҵĵǰӰõװĺû
	int				m_nTempLucky_p;				// ¼ҵʱװ˰ٷֱ
    BYTE	        m_nVipType;                 // Ա
//	int             m_nZhonji;                  // ǰػ
//	int             m_nDangge;                  // ǰ

	int 			m_nExp;						// ǰֵ(ǰȼnpc)
	int				m_nNextLevelExp;			// һֵ
	BYTE			m_CUnlocked;                // ͻǷ Ļ

	UINT			m_dwLeadExp;				// ͳֵ
	UINT			m_dwLeadLevel;				// ͳȼ
	char			m_PlayerName[NAME_LENGHT];           // ҵĽɫ
	//char			m_PeiouName[32];            // ż

	KPlayerTeam		m_cTeam;					// ҵϢ
	KPlayerFaction	m_cFaction;					// ҵϢ

	KPlayerChat		m_cChat;                    // ͷ˵ʾ

	KPlayerTask		m_cTask;					// ϵͳ()
    
	KplayerGuzi		m_GuziGame;	                // ҵϻϵͳ

	KplayerTimeInfo m_MapTimeInfo;			    // ˲ Чҩ浵

	KPlayerPK		m_cPK;						// PKϵ
	
	KPlayerRepute	m_cRepute;					//

	KPlayerFuYuan	m_cFuYuan;					//Դ

	KPlayerReBorn	m_cReBorn;					//ת

	KPlayerVip	    m_cVip;					    //Ա

	KPlayerTong		m_cTong;					// ԼİϢ
	
	DWORD			m_dwDeathScriptId;			// ű
	DWORD			m_dwDamageScriptId;			
	char			m_szTaskAnswerCanshu[MAX_ANSWERNUM][32];  //  ű
	char			m_szTaskAnswerFun[MAX_ANSWERNUM][32];
	int				m_nAvailableAnswerNum;                    //ǰѡ£ش
//	bool			m_bWaitingPlayerFeedBack;	              //ǰǷȴڿͻ˵ķ״̬£ǰűÿ.ʽԻѡ
//	DWORD			m_dwOutOfDateFeedBackTime;	              //ǰȴű޶ʱ̣Ϸʱδϡ
//	BYTE			m_btTryExecuteScriptTimes;	              //
    //BYTE            m_nCheckWaiGua;
	//char			m_CurScriptName[128];
	int				m_nWorldStat;    // 
	int				m_nSectStat;     // 
	int            m_isGm;
	DWORD			m_nGiveCallBackScript;
	BYTE			m_nGiveCallBackNum;

public:
	KPlayer();
	~KPlayer();
	int            getLastDialogNpc(){return m_LastDialogNpc;}
	void			SetPlayerIndex(int nNo);					// 趨 m_nPlayerIndex
	void			GetAboutPos(KMapPos *pMapPos);			    // Ҹһλ
	void			GetNewAboutPos(KMapPos *pMapPos);
	int				GetPlayerIndex();							// ñʵ Player еλ
	UINT			GetPlayerID(){return m_dwID;};
	void			GetFactionName(char *lpszName, int nSize);	// õǰ not end
	int				GetFactionCamp();
	int				GetFactionNo();
	void			ChatFriendOnLine(DWORD dwID, int nFriendIdx);// ֪ͨĳ

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

	BOOL			ExecuteScriptA(char * ScriptFileName, char * szFunName, int nParam = 0,char * szCanshu=NULL,BOOL nIsGive=TRUE); //NPCԻ
	BOOL			ExeNoBackScript(char * ScriptFileName, char * szFunName,int nKind = 0,int nParama = 0,int nParamb = 0,int nParamc = 0,char *strInfo=NULL);
	BOOL			ExecuteScriptC(char * ScriptFileName, char * szFunName, char * szParams=NULL,char * snParams=NULL,int inParam = 0,int njb=0,int njxb=0,int inval=0,int invala=0,BOOL nIsGive=TRUE);
	
	BOOL			ExecuteScript_NEW(char * ScriptFileName, char * szFunName, int szParams=0,char * snParams=NULL,int inParam = 0,int njb=0,int njxb=0,int inval=0,int invala=0,BOOL nIsGive=TRUE);
	
	int             ExecuteScript_Direct(char * ScriptFileName, char * szFunName,int nIsGive,LPSTR cFormat, ...);

	BOOL			ExecuteScriptD(DWORD dwScriptId, char * szFunName, char *  szParams=NULL,char * snParams=NULL,int inParam = 0,int njb=0,int njxb=0,int inval=0,int invala=0,BOOL nIsGive=FALSE);
	BOOL			ExecuteScriptB(DWORD dwScriptId,  char * szFunName, int nParam=0,char * szCanshu=NULL,BOOL nIsGive=TRUE);
	BOOL			DoScript(char * ScriptCommand);				//ִĳűָ
	BOOL			ExeItemScriptA(char * ScriptFileName, char * szFunName, char * szParams=NULL,char * snParams=NULL,int inParam = 0,int njb=0,int njxb=0,int inGenre=0,int inDetail=0,int inParticular=0,int inStackNum=0,int inItemX=0,int inItemY=0);
	BOOL			ExeItemScript(DWORD dwScriptId, char * szFunName, char *  szParams=NULL,char * snParams=NULL,int inParam = 0,int njb=0,int njxb=0,int inGenre=0,int inDetail=0,int inParticular=0,int inStackNum=0,int inItemX=0,int inItemY=0);
    void            myPrintLuaStackA(lua_State *L);  //
	void			ChangeCurStrength(int nData);			// ı䵱ǰ( nData С 0 ʱ)
	void			ChangeCurDexterity(int nData);			// ı䵱ǰ( nData С 0 ʱ)
	void			ChangeCurVitality(int nData);			// ı䵱ǰ( nData С 0 ʱ)
	void			ChangeCurEngergy(int nData);			// ı䵱ǰ( nData С 0 ʱ)
//	BOOL			ExecuteScriptC(char * ScriptFileName);
	void			Release();
	void			Active(int  _minLimitLevel, int  _maxLimitLevel);								// ÿϷѭҪĶ
	void			ProcessMsg(KWorldMsgNode *lpMsg);		// ϢתΪNPC
    void            ReSkillEnhance();
	LPSTR			GetPlayerName() { return m_PlayerName; };
	int             GetLimitMaxLevel(){return m_nLimitMaxLevel;};
	void            SetLimitMaxLevel(int ver){m_nLimitMaxLevel=ver;};
	BOOL			NewPlayerGetBaseAttribute(int Series);	// ҵ½ʱԲ     ֵ
	void			AddBaseLucky(int nData);				// ӻ
#ifdef _SERVER

	void            CheckItemAddtoEquipment(int nRoomCount);
//	void			AddExp(int nExp, int nTarLevel);		// Ӿ(ԭʼݣδ)
	void	        AddExp(int nExp, int nTarLevel,int nGoldType);
	void			AddSelfExp(int nExp, int nTarLevel,int nGoldType);	// Ӿ(ҪپĴҪǱnpcĵȼ)
	void			AddSkillExp(int nExp);
#endif
	void			DirectAddExp(int nExp);					// ֱӾֵ
	void			LevelUp();								// һ
	void			SetLevel(int nLevel);
	void			AddLeadExp(int nExp);					// ͳ
	void			UpdataCurData(int nIsLoign=0,int nUnEquip=0);
	void			ReCalcEquip();		    // ¼ϵװ
	void			ReCalcState(int nIsLoign=0);							// ¼ϵ״̬
	BOOL            CheckThisItem(int nSetings,char *nKey);
	void			ChangePlayerCamp(int nCamp);			// ıӪ
	void 			Revive(int nType);						// 	
	void			GetEchoDamage(int* nMin, int* nMax, int* nAr, int nType);// ȡҪʾ˺ֵ
	BOOL			CheckTrading();
	void			SetFirstDamage();   //û˺
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
//	void			GetEchoDamage(int* nMin, int* nMax, int nType);// ȡҪʾ˺ֵ
	void			ProcessInputMsg(UINT uMsg, WPARAM wParam, LPARAM lParam);// Ϣ
	void			RecvSyncData();								// ͬ
    BOOL            ClientExeItemScript(char *nScriptPath,char *nFunName,int nPaparm,int ninVal,char *nstrVala);
	void			ApplyTeamInfo(DWORD dwNpcID);				// ѯĳnpcڶϢ
	void	        ApplyRoomInfo(int nIsFull);
	void			ApplySelfTeamInfo();						// ѯĶ
	BOOL			ApplyCreateTeam();//char *lpszTeamName);		// 봴
	BOOL			ApplyTeamOpenClose(BOOL bFlag);				// ӳ뿪šرնǷԱ״̬
	void			ApplyAddTeam(int nNpcIndex);				// ĳ
	void			AcceptTeamMember(DWORD dwNpcID);			// ֪ͨĳnpcΪԱ
	void			TeamDropApplyOne(DWORD dwNpcID);			// ӳɾбеĳnpc
	void			LeaveTeam();								// ֪ͨ뿪
	void			TeamKickMember(DWORD dwNpcID);				// ӳ֪ͨ߳ĳԱ
	void			ApplyTeamChangeCaptain(DWORD dwNpcID);		// ӳԼĶӳݽĶԱ
	void			ApplyTeamDismiss();							// ӳɢ
	void			ApplySetPK(BOOL bPK);						// 򿪡رpk
	void			ApplyFactionData();							// 
	void			SendChat(KUiMsgParam *pMsg, char *lpszSentence);// ͻ˷
	void			ApplyAddBaseAttribute(int nAttribute, int nNo);// ӳĳһĵ(0=Strength 1=Dexterity 2=Vitality 3=Engergy)
	BOOL			ApplyAddSkillLevel(int nSkillID, int nAddPoint);// ĳ
	BOOL			ApplyUseItem(int nItemID, ItemPos SrcPos);	// ʹĳƷҼƷ
	BOOL            ApplyAutoMoveItem(int nItemID, ItemPos SrcPos,int destPlace);
	void	        PickUpItem(int nObjIndex,int rid);
    void	        PickUpMoney(int nObjIndex,int rid);
	void			PickUpObj(int nObjIndex);					// ͻobjĳƷϢ
	void			ObjMouseClick(int nObjIndex);				// ͻobjϢ
	void			MoveItem(ItemPos DownPos, ItemPos UpPos,int nIsComp=0);	// DownPos ֣UpPos 
	void			MoveSkill(ItemPos DownPos, ItemPos UpPos);
	int				ThrowAwayItem();
	int	            AutoAwayItem(DWORD nItmeDwIdx);  //---------------Ҷ-------------------
	void			ChatAddFriend(int nPlayerIdx);				// ͻͨ˵ѵ
	void			ChatRefuseFriend(int nPlayerIdx);			// ͻ˾ܾ˵ѵ
	void			TradeApplyOpen(char *lpszSentence, int nLength);
	void			TradeApplyClose();
	void			TradeApplyStart(int nNpcIdx);
	BOOL			TradeMoveMoney(int nMoney);					// ʱԼǮ	
	void			TradeDecision(int nDecision);				// if nDecision == 0 Ƴ  if nDecision == 1 ȷ  if nDecision == 2 ȡȷ
	void			TradeApplyLock(int nLockOrNot);				// ȡ

	void			SetChatCurChannel(int nChannelNo);			// 趨ǰƵ
	void			TeamInviteAdd(DWORD dwNpcID);				// 

	void			SetLeftSkill(int nSkillID,BOOL nIsNoLogin=FALSE);
	void			SetRightSkill(int nSkillID);
	void	        SetShiquSkill(int nSkillID);
	void			UpdateWeaponSkill();
//	int				GetLeftSkill(){return m_nLeftSkillID;};
//	int				GetRightSkill(){return m_nRightSkillID;};
	void			SetDefaultImmedSkill();
	void			s2cApplyAddTeam(BYTE* pProtocol);			// յ֪ͨ
	void			s2cTradeChangeState(BYTE* pMsg);
	void			s2cTradeMoneySync(BYTE* pMsg);
	void			s2cTradeDecision(BYTE* pMsg);				// յ֪ͨɻȡ
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
	int				FindAroundPlayer(DWORD dwNpcID);		// ѰΧĳָnpc idplayer index
	int				FindAroundNpc(DWORD dwNpcID);			// ѰΧĳָnpc idnpc index
	int				FindAroundNpcID(DWORD dwNpcID, int nDist =0);
	BOOL			CheckPlayerAround(int nPlayerIdx);		// жĳǷΧ
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
	void			GetLoginRevivalPos(int *lpnSubWorld, int *lpnMpsX, int *lpnMpsY);		// ȡҵλ
	void			GetDeathRevivalPos(int *lpnSubWorld, int *lpnMpsX, int *lpnMpsY);		// ȡλ
	void			SetRevivalPos(int nSubWorld,int nRevalId);	    //趨ҽϷID
	void            SetDeathRevivalPos(int nSubWorld,int nXpos,int nYpos);//ҵʱ
	BOOL			Save(BOOL nIsShow=FALSE);									// 
	BOOL			CanSave();
	void			ProcessUser();
	BOOL			SendSyncData(int &nStep, unsigned int &nParam);	// ͬ
	BOOL			SendSyncData_Skill();					// ͬ - 
	void			SendCurNormalSyncData();				// ƽʱԼͬ
	void			SetChatForbiddenFlag(int nFlag)
	{//ý  
	        m_nForbiddenFlag = nFlag;
	};

	BOOL			AddFaction(char *lpszFactionName);		// 
	BOOL			AddFaction(int nFactionID);				// 
	BOOL			LeaveCurFaction();						// 뿪
	BOOL			CurFactionOpenSkill(int nLevel);		// ŵǰĳȼļ

	void			TaskClearTempVal();						// ʱ
	int				TaskGetSaveVal(int nNo);				// õ
	void			TaskSetSaveVal(int nNo, BOOL bFlag);	// 趨
	int				TaskGetClearVal(int nNo);				// õʱ̿Ʊֵ
	void			TaskSetClearVal(int nNo, int nVal);		// 趨ʱ̿Ʊֵ

	//void			SetTimeTaskTime(DWORD time){m_dwTimeTaskTime = time;};
	void			BuyItem(BYTE* pProtocol);
	void			SellItem(BYTE* pProtocol);
	void			GopItemPlayer(BYTE* pProtocol);
	void			QuitGame(int nQuitType);				// ˳Ϸ
	void			S2CSendTeamInfo(BYTE* pProtocol);		// յͻ˲ѯĳnpcϢͻ˷ͶϢ
	void			SendSelfTeamInfo();						// ͻ˷ͶϢ
	BOOL			CreateTeam(BYTE* pProtocol);			// յͻ󴴽һ֧
	BOOL			SetTeamState(BYTE* pProtocol);			// յͻ󿪷šرձplayerǷԱ״̬
	BOOL			S2CSendAddTeamInfo(BYTE* pProtocol);	// յͻһ֧
	BOOL			AddTeamMember(BYTE* pProtocol);			// ӳ֪ͨĳnpcΪԱ
	void			LeaveTeam(BYTE* pProtocol);				// յͻ˶Ա֪ͨ뿪
	void			TeamKickOne(BYTE* pProtocol);			// յͻ˶ӳ֪ͨ߳ĳԱ
	void			TeamChangeCaptain(BYTE* pProtocol);		// յͻ˶ӳ֪ͨѶӳݽĳԱ
	void			TeamDismiss(BYTE* pProtocol);			// յͻ˶ӳɢ
	void			SetPK(BYTE* pProtocol);					// յͻ趨PK״̬
	void			SendFactionData(BYTE* pProtocol);		// յͻ
	void			ServerSendChat(BYTE* pProtocol);		// յͻ˷
	void			AddBaseAttribute(BYTE* pProtocol);		// յͻҪӻԵ(0=Strength 1=Dexterity 2=Vitality 3=Engergy)
	void			ResetBaseAttribute(BYTE* pProtocol);	// ham moi
	void			IncSkillExp(int nSkillId, int nAddExp); // ham moi
	void			AddSkillPoint(BYTE* pProtocol);			// յͻҪĳܵĵ
	BOOL			ServerPickUpItem(BYTE* pProtocol);		// յͻϢĳobjװǮ
	void			EatItem(BYTE* pProtocol);				// յͻϢҩ
	void			AutoItem(BYTE* pProtocol);				// յͻϢҩ
	int 			ServerMoveItem(BYTE* pProtocol);		// յͻϢƶƷ
	void			ServerThrowAwayItem(BYTE* pProtocol);	// յͻϢƷ
	BOOL            CheckGmName();                          // ǷGM˺
	BOOL            CheckCanTradeName();
	void			ChatSetTakeChannel(BYTE* pProtocol);	// յͻϢ趨충Ƶ
	void			ChatTransmitApplyAddFriend(BYTE* pProtocol);// յͻתӺϢ
	BOOL			ChatAddFriend(BYTE* pProtocol);			// յͻϢ
	void			ChatRefuseFriend(BYTE* pProtocol);		// յͻϢܾ
	void			ChatResendAllFriend(BYTE* pProtocol);
	void			ChatSendOneFriendData(BYTE* pProtocol);
	void			ChatDeleteFriend(BYTE* pProtocol);		// յͻɾĳ
	void			ChatRedeleteFriend(BYTE* pProtocol);	// յͻٴɾĳ
	void			TradeApplyOpen(BYTE* pProtocol);		// յͻ״̬
	void			TradeApplyClose(BYTE* pProtocol);
	void			CancelTradeSafe(); // V9 Atomic Cancel Trade		// յͻȡ״̬
	void			TradeApplyStart(BYTE* pProtocol);		// յͻ뿪ʼ
	void			TradeMoveMoney(BYTE* pProtocol);		// յͻ뽻moneyĸı
	void			TradeDecision(BYTE* pProtocol);			// յͻ뽻ȷȡ
	void            KickOutGame();                          // ֪ͨ˳Ϸ
	void			c2sTradeReplyStart(BYTE* pProtocol);
	void			SyncTradeState();						// ˫Ŀͻ˷ͽ״̬Ϣ
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
	void	        SAutoplay(int nbutton,int nBtyeVal,int nBtyeVala); //һ
	void			script(int nId,int nXuHao);
	void			pkvalue(int nId);	
	BOOL			CreateTong(int nCamp, char *lpszTongName, char* lpszTitle);
    void			SendEquipItemInfo(int nTargetPlayer, int nPrcess = 0, BOOL bUpdate = FALSE);// ԼװϵװϢ˿
	void			SendSellItemInfo(int nTargetPlayer, int nPrcess, BOOL bUpdate = FALSE);	// ԼõĳϢ˿
	void			SendSellItemCount(int nIndex);
	int				GetTradeCount();
	int 			SendLianItemInfo(int nTargetPlayer, int nItemDwID,int nPrcess=0, BOOL bUpdate= FALSE,char *nShopName=NULL);	// Ϣ˿
	int 			SendLianItemInfo(const void *nData);
	

#endif

private:
//	void			CalcCurStrength();						// 㵱ǰ
//	void			CalcCurDexterity();						// 㵱ǰ
//	void			CalcCurVitality();						// 㵱ǰ
//	void			CalcCurEngergy();						// 㵱ǰ
	void			CalcCurLucky();							// 㵱ǰ
	void			LevelAddBaseLifeMax();					// ȼһ
	void			LevelAddBaseManaMax();					// ȼһ
	void			LevelAddBaseStaminaMax();				// ȼһ

	void			SendFactionData();						// ͻ˷

	void			SyncCurrentBaseAttriibute(int type,int attribute,int curAttribute);	// ham moi
// V9 port: LuaAddStrg/AddDex/AddVit/AddEng call these four directly
public:
	void			AddBaseStrength(int nData);				// ӻ  ---
	void			AddBaseDexterity(int nData);			// ӻ  ---
	void			AddBaseVitality(int nData);				// ӻ---⹦
	void			AddBaseEngergy(int nData);				// ӻ---ڹ
private:
	
	void			ResetBaseStrength(int nData);			// ӻ
	void			ResetBaseDexterity(int nData);			// ӻ
	void			ResetBaseVitality(int nData);			// ӻ
	void			ResetBaseEngergy(int nData);			// ӻ
	
	void			SetNpcPhysicsDamage();					// ɵǰӦnpc˺(PhysicsDamage)
	void			SetNpcAttackRating();					// ɵǰݼӦnpcĹ(AttackRating)
	void			SetNpcDefence();						// ɵǰݼӦnpcķ
//	void			SetNpcWalkSpeed();						// ɵǰݼӦnpcٶ
//	void			SetNpcRunSpeed();						// ɵǰݼӦnpcܲٶ

#ifndef _SERVER
	void			ProcessMouse(int x, int y, int Key, MOUSE_BUTTON nButton);
	void			OnButtonUp(int x,int y,MOUSE_BUTTON nButton);				// ̧
	void			OnButtonDown(int x,int y,int Key,MOUSE_BUTTON nButton);		// 
	void			OnButtonMove(int x,int y,int Key,MOUSE_BUTTON nButton);		// ºƶ
	void			OnMouseMove(int x,int y);									// ƶ
	int				NetCommandPlayerTalk(BYTE* pProtocol);

	//Question:Ϊ԰ʹ
	friend		    int LuaInitStandAloneGame(Lua_State * L);
#endif


// Ķ
private:
	void			S2CExecuteScript(char * ScriptName, char * szParam);

#ifdef _SERVER
//ݿģ麯-----------------
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
	void			DoScriptAction(PLAYER_SCRIPTACTION_SYNC * pUIInfo); //֪ͨÿͻʾĳUI
	void			ProcessPlayerSelectFromUI(BYTE* pProtocol);			// Ҵѡ˵ѡĳʱĲ
#ifndef _SERVER
	void			DialogNpc(int nIndex,BOOL nIsServer=TRUE);
	void            OpenBaiTan(DWORD nIndwId);
	void			OnSelectFromUI(PLAYER_SELECTUI_COMMAND * pSelectUI, UIInfo eUIInfo);//Ҵѡѡĳ󣬽			
	void			OnScriptAction(PLAYER_SCRIPTACTION_SYNC * );
#endif
#ifdef _SERVER
	void			RestoreLiveData();						//ָҵĻ
	void			SetTimer(DWORD nTime, int nTimeTaskId);//ʱűʱ
	void			CloseTimer();							//رʱʱ
	BOOL            SavePlayerData(bool bUnLock);
	int				AddDBPlayer(char * szPlayerName, int sex, DWORD * pdwID );
	int				LoadDBPlayerInfo(BYTE * pPlayerInfo,  int &nStep, unsigned int &nParam);
	BOOL			GetNewPlayerFromIni(KIniFile * pIniFile, BYTE * pRoleBuffer);
	int				UpdateDBPlayerInfo(BYTE * pPlayerInfo,DWORD dwBufferLen,BOOL nIsShow=FALSE);  //浵
	int				DeletePlayer(char * szPlayerName = NULL);//ע⣺ʺţ
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

extern CORE_API KPlayer	*Player;

#endif //KPlayerH


#ifndef	KGAMEGLOBALSET
#define	KGAMEGLOBALSET

class KGameglobalSet
{
	struct _damageParm
	{
		int	nIsDebug;
		int	nIsFreeze;
		int	nIsPoison;
		int	nIsLight;
		int nIsFire;
		int nFreezeRank;
		int nFreezeTime;
		int nPoisonRank;
		int nPoisonTime;
		int nStunRank;
		int nXiMoSkill;
		int nIsOpenPuDian;
		int nIsOpenPuBai;
		int _isOpenZhiMing;
		int nDanGeRate;
		int nStateSkill;
		int NpcPoisonDamageMax,PlayerPoisonDamageMax; //合成毒伤害的最大
	    int nIsXiBlood,nIsXiBlue,nShowDamageInfo;
	};


	struct _calcDamageParm
	{
		int nVerSale;
		int nIsDebug,nMinResist,nMaxResist,nStateSkill,nIsOpenPk;
		int nZhongJiRate,nbrokendefense,_isOpen;
		int sDoHurt[3];	
	};

	struct _worldParm
	{
		 int xPos,yPos,nCheckNum,nIsCheckNpcPos,nModel,nIsCheckNpc,nIsOpenClearItem;
		 int nMinDropRate,nMaxDropRate,TongMaxCount,nSerDistance;
		 int nExpSale,nMapAutoGoldNpcRank,nXingYun,_mapRefModel;
		 int nCityLevel,nWorldLevel,nIsClearAllSkill,nIsClearAllFriend,nIsClearAllTask,nSkillStateKeepRange;
	};

	struct _npcParm
	{
		int nTimeval,sPKValue,nAttackRadius,nLuckyRange,nLevelRangeCount;
		int naAttackSpeed,nbAttackSpeed;
		int nLuckyRate,nGoldMoneyRate,duanLuckyRate;
		int nIsOpenFullForbit,nBianTime,nMagicNum;
		int nIsCloseGuaItem,nIsCloseAllItem,nTimeStart,nTimeEnd;
		int nExpMin,nExpMax,nGoldExpMin,nGoldExpMax,nSubExp,nDeathLevelpro;
		int nLuckItem[5],BianLuckItem[5],nForBitItem[3],duanLuckItem[5],duanLevelItem[5];
	};

	struct _playerParm
	{
	   char nExpRoleName[NAME_LENGHT];
	   int mSubWorld,mReviveId,isThrowAwayItem;        //80县
	   int nGoldRate,nMaxLevel;
	   int nNpcChaLevel,nExpForBitLvel,nIsOpenExpLevel;
	   int nShowDamageInfo;
	   int nGoldExpInfo[3],nNoGoldExpInfo[3];
	};

	struct _itemParm
	{
		int  nItemUseLevel;
		int  nGoldAllActive,nEquipAllActive;
		int  nInFenJie[3];
		int  nshowinfo,_loadItemMagic;
	};

	struct _skillsetParm
	{
		int nSeriesTrimMax;
	    //五行印属性公式参数
		int nSeriesTrimParam1;
		int nSeriesTrimParam2;
		int nSeriesTrimParam3;
		int nSeriesTrimParam4;
		int nSeriesTrimParam5;
	};
	
public:
	KGameglobalSet();
	void			Init();
	virtual         BOOL Release(void);
    _damageParm     DamageParmInfo;
	_calcDamageParm CalcDamageInfo;
	_worldParm      SubWorldInfo;
	_npcParm        NpcInfo;
	_playerParm     PlayerInfo;
	typedef std::/*unordered_*/map<std::string,float> _ExpNoteInfo;
	_ExpNoteInfo nGoldExpInfo;
	_ExpNoteInfo nNoGoldExpInfo;
	_itemParm    gameItemInfo;
	_skillsetParm skillsetInfo;
	int gameLanguageVersion; //1 CN 2 VN
};

extern KGameglobalSet _gameGlobalSet;
#endif
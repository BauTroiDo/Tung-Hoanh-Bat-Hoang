#include "KCore.h"
//#include "KGameglobalSet.h"
KGameglobalSet	_gameGlobalSet;

KGameglobalSet::KGameglobalSet()
{
	Release();
}

BOOL KGameglobalSet::Release(void)
{
	memset(&DamageParmInfo,0,sizeof(DamageParmInfo));
    memset(&CalcDamageInfo,0,sizeof(CalcDamageInfo));
	memset(&SubWorldInfo,0,sizeof(SubWorldInfo));
	SubWorldInfo.xPos=16;
	SubWorldInfo.yPos=16;
	SubWorldInfo.nCheckNum=32;

	memset(&NpcInfo,0,sizeof(NpcInfo));
	NpcInfo.naAttackSpeed = 100;
	NpcInfo.nbAttackSpeed = 100;
	NpcInfo.nTimeval      = 90;
	NpcInfo.nTimeEnd      = 9;

	memset(&PlayerInfo,0,sizeof(PlayerInfo));
	PlayerInfo.mSubWorld=53;
	PlayerInfo.mReviveId=19;        //80县

	nGoldExpInfo.clear();
	nNoGoldExpInfo.clear();

	memset(&gameItemInfo,0,sizeof(gameItemInfo));
	return TRUE;
}

// 黄金怪物设定文件的初始化没有放在这里，直接放在 core 的初始化里面
void KGameglobalSet::Init()
{
	g_GameSetTing.GetInteger2("ServerConfig","DanGeRate",&DamageParmInfo.nDanGeRate,&DamageParmInfo.nStateSkill);
	g_GameSetTing.GetInteger("ServerConfig","IsDeBugDamage",0,&DamageParmInfo.nIsDebug);
	g_GameSetTing.GetInteger("ServerConfig","nIsFreeze",0,&DamageParmInfo.nIsFreeze);
	g_GameSetTing.GetInteger("ServerConfig","nIsPoison",0,&DamageParmInfo.nIsPoison);
	g_GameSetTing.GetInteger("ServerConfig","nIsLight",0,&DamageParmInfo.nIsLight);
	g_GameSetTing.GetInteger("ServerConfig","nIsFire",0,&DamageParmInfo.nIsFire);
	g_GameSetTing.GetInteger2("ServerConfig","FreezeRankAndTime",&DamageParmInfo.nFreezeRank,&DamageParmInfo.nFreezeTime);
	g_GameSetTing.GetInteger2("ServerConfig","PoisonRankAndTime",&DamageParmInfo.nPoisonRank,&DamageParmInfo.nPoisonTime);
	g_GameSetTing.GetInteger("ServerConfig","NpcStunRank",0,&DamageParmInfo.nStunRank);
	g_GameSetTing.GetInteger("SYSTEM","XiMoXingSkill",711,&DamageParmInfo.nXiMoSkill);
	g_GameSetTing.GetInteger2("SYSTEM","IsOpenPuAttack",&DamageParmInfo.nIsOpenPuDian,&DamageParmInfo.nIsOpenPuBai);
	g_GameSetTing.GetInteger("Gameconfig","IsOpenZhiMing",0,&DamageParmInfo._isOpenZhiMing);

	g_GameSetTing.GetInteger("Gameconfig","showItemInfo",1,&gameItemInfo.nshowinfo);

	g_GameSetTing.GetInteger("Gameconfig","attackRadius",512,&NpcInfo.nAttackRadius);
	

	g_GameSetTing.GetInteger("ServerConfig","NpcPoisonDamageMax",0,&DamageParmInfo.NpcPoisonDamageMax);
	g_GameSetTing.GetInteger("ServerConfig","PlayerPoisonDamageMax",0,&DamageParmInfo.PlayerPoisonDamageMax);

	//双吸设置 可以设置内外攻 吸血 吸内-----------------------------------------------------
	g_GameSetTing.GetInteger2("ServerConfig","IsBloodAndBlue",&DamageParmInfo.nIsXiBlood,&DamageParmInfo.nIsXiBlue);
	g_GameSetTing.GetInteger("ServerConfig","ShowDamageInfo",0,&DamageParmInfo.nShowDamageInfo);

	g_GameSetTing.GetInteger("Gameconfig","ZhiMingVer",10,&CalcDamageInfo.nVerSale);

	
	g_GameSetTing.GetInteger("ServerConfig","IsDeBug",0,&CalcDamageInfo.nIsDebug);
	g_GameSetTing.GetInteger("ServerConfig","IsOpenPk",0,&CalcDamageInfo.nIsOpenPk);
	//g_GameSetTing.GetInteger2("ServerConfig","IsBloodAndBlue",&nIsXiBlood,&nIsXiBlue);
	g_GameSetTing.GetInteger2("ServerConfig","MinAndMaxResist",&CalcDamageInfo.nMinResist,&CalcDamageInfo.nMaxResist);

	g_GameSetTing.GetInteger2("ServerConfig","ZhongJiRate",&CalcDamageInfo.nZhongJiRate,&CalcDamageInfo.nStateSkill);
	g_GameSetTing.GetInt3("SYSTEM","IsDoHurt",CalcDamageInfo.sDoHurt);
	g_GameSetTing.GetInteger2("ServerConfig","brokendefense",&CalcDamageInfo._isOpen,&CalcDamageInfo.nbrokendefense);


	g_GameSetTing.GetInteger2("Item","ItemDropPos",&SubWorldInfo.xPos,&SubWorldInfo.yPos); // 偏移
	g_GameSetTing.GetInteger2("Item","ItemDropCheckNum",&SubWorldInfo.nCheckNum,&SubWorldInfo.nIsCheckNpcPos); // 查找的范围
	g_GameSetTing.GetInteger2("Item","ItemDropModel",&SubWorldInfo.nModel,&SubWorldInfo.nIsCheckNpc);
	g_GameSetTing.GetInteger("Item","IsOpenClearItem",1,&SubWorldInfo.nIsOpenClearItem);
	g_GameSetTing.GetInteger("Item","loadItemMagic",MAGIC_LOAD_KIND_LEVEL,&_gameGlobalSet.gameItemInfo._loadItemMagic);
	

	g_GameSetTing.GetInteger("SYSTEM","XinYunXing",0,&SubWorldInfo.nXingYun);
	g_GameSetTing.GetInteger2("SYSTEM","ItemDropRate",&SubWorldInfo.nMinDropRate,&SubWorldInfo.nMaxDropRate);


	g_GameSetTing.GetInteger("ServerConfig","GouHouTime",90,&NpcInfo.nTimeval);
	g_GameSetTing.GetInteger2("SYSTEM","NpcAttackSpeed",&NpcInfo.naAttackSpeed,&NpcInfo.nbAttackSpeed);
	g_GameSetTing.GetInteger("ServerConfig","LuckyRate",100,&NpcInfo.nLuckyRate);
	//g_GameSetTing.GetInteger("ServerConfig","GoldMoneyRate",100,&NpcInfo.nGoldMoneyRate);
	g_GameSetTing.GetInteger("ServerConfig","IsOpenFullForbit",0,&NpcInfo.nIsOpenFullForbit);
	g_GameSetTing.GetInteger2("ServerConfig","IsCloseItem",&NpcInfo.nIsCloseGuaItem,&NpcInfo.nIsCloseAllItem);
	g_GameSetTing.GetInteger2("ServerConfig","TimeCloseItem",&NpcInfo.nTimeStart,&NpcInfo.nTimeEnd);
	
	g_GameSetTing.GetInteger("SYSTEM","GoldMoneyRate",100,&NpcInfo.nGoldMoneyRate);
	g_GameSetTing.GetInteger2("SYSTEM","DeathExpData",&NpcInfo.nExpMin,&NpcInfo.nExpMax);
	g_GameSetTing.GetInteger("SYSTEM","DeathLevelpro",10,&NpcInfo.nDeathLevelpro);
	g_GameSetTing.GetInteger2("SYSTEM","GoldDeathExpData",&NpcInfo.nGoldExpMin,&NpcInfo.nGoldExpMax);

	g_GameSetTing.GetInteger("ServerConfig","PKValue",5,&NpcInfo.sPKValue);
	g_GameSetTing.GetInt5("SYSTEM","BlueMagciLuckyLevel",NpcInfo.nLuckItem);

	g_GameSetTing.GetInteger2("SYSTEM","MagciLuckyRange",&NpcInfo.nLuckyRange,&NpcInfo.nLevelRangeCount);

    g_GameSetTing.GetInt5("SYSTEM","BianShiLuckyCount",NpcInfo.BianLuckItem);
	g_GameSetTing.GetInteger("ServerConfig","BianShiTime",1,&NpcInfo.nBianTime);
	g_GameSetTing.GetInt3("ServerConfig","AutoPlayForBitItem",NpcInfo.nForBitItem);
	g_GameSetTing.GetInteger("ServerConfig","MagicNum",0,&NpcInfo.nMagicNum);
	g_GameSetTing.GetInteger("ServerConfig","DuanZaoLuckyRate",5000,&NpcInfo.duanLuckyRate);

	g_GameSetTing.GetInt5("ServerConfig","DuanZaoBlueMagciLucky",NpcInfo.duanLuckItem);
	g_GameSetTing.GetInt5("ServerConfig","DuanZaoMagciLuckyLevel",NpcInfo.duanLevelItem);

	g_GameSetTing.GetInteger("TongSet","MaxCount",10,&SubWorldInfo.TongMaxCount); //	 帮会的最大人数限制
	g_GameSetTing.GetInteger("SkillsIcon","ExpDisDistance",400,&SubWorldInfo.nSerDistance);
	g_GameSetTing.GetInteger("ServerConfig","ExpRate",1,&SubWorldInfo.nExpSale);
	g_GameSetTing.GetInteger("SYSTEM","MapAutoGoldNpcRank",0,&SubWorldInfo.nMapAutoGoldNpcRank);
	g_GameSetTing.GetInteger2("SYSTEM","ForBitChat",&SubWorldInfo.nCityLevel,&SubWorldInfo.nWorldLevel);
	g_GameSetTing.GetInteger("SYSTEM","IsClearAllTask",0,&SubWorldInfo.nIsClearAllTask);
	g_GameSetTing.GetInteger("SYSTEM","IsClearAllFriend",0,&SubWorldInfo.nIsClearAllFriend);
	g_GameSetTing.GetInteger("SYSTEM","IsClearAllSkill",0,&SubWorldInfo.nIsClearAllSkill);

	g_GameSetTing.GetInteger2("ServerConfig","PlayerRevPosId",&PlayerInfo.mSubWorld,&PlayerInfo.mReviveId);
    g_GameSetTing.GetInteger("ServerConfig","GlobalExpRate",0,&PlayerInfo.nGoldRate);
	g_GameSetTing.GetInteger("ServerConfig","PlayerMaxLevel",1000,&PlayerInfo.nMaxLevel);

	 g_GameSetTing.GetInteger("ServerConfig","nChaLevel",20,&PlayerInfo.nNpcChaLevel);
	 g_GameSetTing.GetInteger("ServerConfig","ExpForBitMaxLvel",5000,&PlayerInfo.nExpForBitLvel);
	 g_GameSetTing.GetInteger("ServerConfig","IsOpenExpLevel",0,&PlayerInfo.nIsOpenExpLevel);
	 g_GameSetTing.GetString("Gameconfig","ExpRoleName","",PlayerInfo.nExpRoleName,sizeof(PlayerInfo.nExpRoleName));
	 g_GameSetTing.GetInteger("Gameconfig","SkillStateKeepRange",30,&SubWorldInfo.nSkillStateKeepRange);
	 g_GameSetTing.GetInteger("Gameconfig","mapRefModel",GAME_MAP_REF_NOR,&SubWorldInfo._mapRefModel);
	  
	 
	 g_GameSetTing.GetInteger("ServerConfig","ShowDamageInfo",0,&PlayerInfo.nShowDamageInfo);

	 float  ExpSle;
     g_GameSetTing.GetFloat("ExpLevelSale","ExpLevel50",1,&ExpSle);
	 nNoGoldExpInfo["ExpLevel50"] = ExpSle;
	 g_GameSetTing.GetFloat("ExpLevelSale","ExpLevel70",1,&ExpSle);
	 nNoGoldExpInfo["ExpLevel70"] = ExpSle;
     g_GameSetTing.GetFloat("ExpLevelSale","ExpLevel80",1,&ExpSle);
	 nNoGoldExpInfo["ExpLevel80"] = ExpSle;
     g_GameSetTing.GetFloat("ExpLevelSale","ExpLevel90",1,&ExpSle);
	 nNoGoldExpInfo["ExpLevel90"] = ExpSle;
	 g_GameSetTing.GetFloat("ExpLevelSale","ExpLevel110",1,&ExpSle);
	 nNoGoldExpInfo["ExpLevel110"] = ExpSle;
	 g_GameSetTing.GetFloat("ExpLevelSale","ExpLevel130",1,&ExpSle);
	 nNoGoldExpInfo["ExpLevel130"] = ExpSle;
	 g_GameSetTing.GetFloat("ExpLevelSale","ExpLevel150",1,&ExpSle);
	 nNoGoldExpInfo["ExpLevel150"] = ExpSle;
	 g_GameSetTing.GetFloat("ExpLevelSale","ExpLevel170",1,&ExpSle);
	 nNoGoldExpInfo["ExpLevel170"] = ExpSle;
	 g_GameSetTing.GetFloat("ExpLevelSale","ExpLevel190",1,&ExpSle);
	 nNoGoldExpInfo["ExpLevel190"] = ExpSle;
	 g_GameSetTing.GetFloat("ExpLevelSale","ExpLevel200",1,&ExpSle);
	 nNoGoldExpInfo["ExpLevel200"] = ExpSle;
     g_GameSetTing.GetFloat("ExpLevelSale","ExpLevel",1,&ExpSle);
	 nNoGoldExpInfo["ExpLevel"] = ExpSle;
	 

	 g_GameSetTing.GetFloat("GoldExpLevelSale","ExpLevel50",1,&ExpSle);
	 nGoldExpInfo["ExpLevel50"] = ExpSle;
	 g_GameSetTing.GetFloat("GoldExpLevelSale","ExpLevel70",1,&ExpSle);
	 nGoldExpInfo["ExpLevel70"] = ExpSle;
	 g_GameSetTing.GetFloat("GoldExpLevelSale","ExpLevel80",1,&ExpSle);
	 nGoldExpInfo["ExpLevel80"] = ExpSle;
	 g_GameSetTing.GetFloat("GoldExpLevelSale","ExpLevel90",1,&ExpSle);
	 nGoldExpInfo["ExpLevel90"] = ExpSle;
	 g_GameSetTing.GetFloat("GoldExpLevelSale","ExpLevel110",1,&ExpSle);
	 nGoldExpInfo["ExpLevel110"] = ExpSle;
	 g_GameSetTing.GetFloat("GoldExpLevelSale","ExpLevel130",1,&ExpSle);
	 nGoldExpInfo["ExpLevel130"] = ExpSle;
	 g_GameSetTing.GetFloat("GoldExpLevelSale","ExpLevel150",1,&ExpSle);
	 nGoldExpInfo["ExpLevel150"] = ExpSle;
	 g_GameSetTing.GetFloat("GoldExpLevelSale","ExpLevel170",1,&ExpSle);
	 nGoldExpInfo["ExpLevel170"] = ExpSle;
	 g_GameSetTing.GetFloat("GoldExpLevelSale","ExpLevel190",1,&ExpSle);
	 nGoldExpInfo["ExpLevel190"] = ExpSle;
	 g_GameSetTing.GetFloat("GoldExpLevelSale","ExpLevel200",1,&ExpSle);
	 nGoldExpInfo["ExpLevel200"] = ExpSle;
	 g_GameSetTing.GetFloat("GoldExpLevelSale","ExpLevel",1,&ExpSle);
	 nGoldExpInfo["ExpLevel"] = ExpSle;
	

	 g_GameSetTing.GetInt3("ExpLevelSale","ExpOther",PlayerInfo.nNoGoldExpInfo);
	 g_GameSetTing.GetInt3("GoldExpLevelSale","ExpOther",PlayerInfo.nGoldExpInfo);
	  g_GameSetTing.GetInteger("SYSTEM","ThrowAwayItem",1,&PlayerInfo.isThrowAwayItem);	
	  g_GameSetTing.GetInt3("SYSTEM","FenJieInfo",gameItemInfo.nInFenJie);

	  g_GameSetTing.GetInteger("SYSTEM","ItemUseLevel",1,&gameItemInfo.nItemUseLevel);
	  g_GameSetTing.GetInteger("SYSTEM","GoldAllActive",itempart_shiping,&gameItemInfo.nGoldAllActive);
	  g_GameSetTing.GetInteger("SYSTEM","EquipAllActive",itempart_yinjian,&gameItemInfo.nEquipAllActive);

	  g_GameSetTing.GetInteger("SKILLSET","SeriesTrimMax",95,&skillsetInfo.nSeriesTrimMax);
	  g_GameSetTing.GetInteger("SKILLSET","SeriesTrimParam1",1,&skillsetInfo.nSeriesTrimParam1);
	  g_GameSetTing.GetInteger("SKILLSET","SeriesTrimParam2",1,&skillsetInfo.nSeriesTrimParam2);
	  g_GameSetTing.GetInteger("SKILLSET","SeriesTrimParam3",1,&skillsetInfo.nSeriesTrimParam3);
	  g_GameSetTing.GetInteger("SKILLSET","SeriesTrimParam4",1,&skillsetInfo.nSeriesTrimParam4);
	  g_GameSetTing.GetInteger("SKILLSET","SeriesTrimParam5",1,&skillsetInfo.nSeriesTrimParam5);

	  g_GameSetTing.GetInteger("Gameconfig","GameVersion",1,&gameLanguageVersion);//1为中文版本 2 其他版本 需要替换字符串的
	  
	 printf("--The game global settings are completed successfully--\n");
}

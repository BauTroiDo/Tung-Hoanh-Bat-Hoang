#include "KCore.h"

#include "KEngine.h"
#include "KMagicAttrib.h"
#include "KMagicDesc.h"
#include "GameDataDef.h"
#include "KFaction.h"

#define	MAGICDESC_FILE	"\\settings\\MagicDesc.Ini"
#define	TABFILE_MAGICAGOLD_PATH	"\\settings\\item\\004\\GoldMagic.txt"   //黄金魔法属性

extern const char * g_MagicID2String(int nAttrib);

const char MAGIC_ATTRIB_STRING[][64] = 
{
//改变技能的属性
	"skill_begin",
	"skill_cost_v",							// 消耗MANA
	"skill_costtype_v",		                //	
	"skill_mintimepercast_v",		        // 每次发魔法的间隔时间
	"skill_misslenum_v",		            // 
	"skill_misslesform_v",		
	"skill_param1_v",		
	"skill_param2_v",
	"skill_attackradius",	                //   skill_reserve1
	"skill_mintimepercastonhorse_v",
	"skill_skillexp_v",                     //"skill_reserve3",技能熟练度
	"skill_desc",
	"skill_eventskilllevel",
	"skill_end",
//改变发出子弹的属性
	"missle_begin",
	"missle_movekind_v",
	"missle_speed_v",
	"missle_lifetime_v",
	"missle_height_v",
	"missle_damagerange_v",
	"missle_radius_v",
	"missle_missrate",                          // 子弹击中的概率
	"missle_hitcount",                          // 子弹击中的人数
	//"missle_reserve1",
	//"missle_reserve2",
	"missle_reserve3",
	"missle_reserve4",
	"missle_reserve5",
	"missle_end",
//装备属性需求属性	
	"item_begin",
	"weapondamagemin_v",
	"weapondamagemax_v",
	"armordefense_v",
	"durability_v",
	"requirestr",
	"requiredex",
	"requirevit",
	"requireeng",
	"requirelevel",
	"requireseries",
	"requiresex",
	"requiremenpai",
	"weapondamageenhance_p",
	"armordefenseenhance_p",
	"requirementreduce_p",
	"indestructible_b",
	"item_nouser",	    //不能使用的门派
	"item_needskill",   //需要学会技能才能使用
	"item_needreborn",  //需要转生次数
	"item_needtongban",
	"item_needbangzhu",
	"item_needcity",
	"item_noseries",
	"item_reserve8",
	"item_reserve9",
	"item_reserve10",
	"item_end",
//伤害技能 --攻击技能
	"damage_begin",		
	"attackrating_v",
	"attackrating_p",
	"ignoredefense_p",
	"physicsdamage_v",      //普点
	"colddamage_v",         //冰点
	"firedamage_v",         //火点
	"lightingdamage_v",     //雷点
	"poisondamage_v",       //毒点
	"magicdamage_v",        //穿刺伤害
	"physicsenhance_p",     //普百分比
	"steallife_p",          //吸血
	"stealmana_p",          //吸蓝
	"stealstamina_p",
	"knockback_p",           //knockback_p//  增加攻击震退对手几率:#d1-%
	"deadlystrike_p",        //会心一击
	"fatallystrike_p",       //致命一击
	"stun_p",                //攻击致昏对手几率:#d1-%
	"damage_reserve1",
	"damage_reserve2",
	"damage_reserve3",
	"damage_reserve4",
	"damage_reserve5",
	"damage_reserve6",
	"damage_reserve7",
	"addzhuabu_v",
	"autoattackskill",       //被击中时
	"seriesdamage_p",        //五行伤害
	"damage_end",
//普通技能  被动 光环 状态  辅助技能 当前值
	"normal_begin",
	"lifemax_v",
	"lifemax_p",            //生命最大值
	"life_v",
	"lifereplenish_v",      //生命回复率
	"manamax_v",
	"manamax_p",
	"mana_v",
	"manareplenish_v",
	"staminamax_v",
	"staminamax_p",
	"stamina_v",
	"staminareplenish_v",
	"strength_v",
	"dexterity_v",
	"vitality_v",
	"energy_v",
	"poisonres_p",
	"fireres_p",
	"lightingres_p",
	"physicsres_p",
	"coldres_p",
	"freezetimereduce_p",
	"burntimereduce_p",
	"poisontimereduce_p",
	"poisondamagereduce_v",
	"stuntimereduce_p",
	"fastwalkrun_p",
	"visionradius_p",
	"fasthitrecover_v",
	"allres_p",          //全抗
	"attackspeed_v",     //外攻速
	"castspeed_v",       //内攻速
	"meleedamagereturn_v",
	"meleedamagereturn_p",
	"rangedamagereturn_v",
	"rangedamagereturn_p",
	"addphysicsdamage_v",  //状态外普点 121
	"addfiredamage_v",     //状态外火 点
	"addcolddamage_v",     //状态外冰 点
	"addlightingdamage_v", //状态外雷 点
	"addpoisondamage_v",   //状态外毒 点
	"addphysicsdamage_p",  //状态外普百分比
	"slowmissle_b",
	"changecamp_b",
	"physicsarmor_v",
	"coldarmor_v",
	"firearmor_v",
	"poisonarmor_v",
	"lightingarmor_v",
	"damagetomana_p",      //伤转内
	"lucky_v",
	"steallifeenhance_p",
	"stealmanaenhance_p",
	"stealstaminaenhance_p",
	"allskill_v",
	"metalskill_v",
	"woodskill_v",
	"waterskill_v",
	"fireskill_v",
	"earthskill_v",
	"knockbackenhance_p",     //命中敌人震退对方的几率:#d1-%
	"deadlystrikeenhance_p",  //会心一击
	"stunenhance_p",          //命中敌人致晕对方的几率
	"badstatustimereduce_v",  //不良状态
	"manashield_p",
	"adddefense_v",
	"adddefense_p",
	"fatallystrikeenhance_p",  //致命一击
	"lifepotion_v",
	"manapotion_v",
	"physicsresmax_p",
	"coldresmax_p",
	"fireresmax_p",
	"lightingresmax_p",
	"poisonresmax_p",
	"allresmax_p",
	"coldenhance_p",
	"fireenhance_p",
	"lightingenhance_p",
	"poisonenhance_p",
	"magicenhance_p",
	"attackratingenhance_v",
	"attackratingenhance_p",
	"addphysicsmagic_v",  //状态内普 点
	"addcoldmagic_v",     //状态内冰 点
	"addfiremagic_v",     //状态内火 点
	"addlightingmagic_v", //状态内雷 点
	"addpoisonmagic_v",   //状态内毒 点
	"fatallystrikeres_p",
	"addskilldamage1",
	"addskilldamage2",
	"expenhance_p", //expenhance_p  杀死敌人获得经验 加 
	"addskilldamage3",
	"addskilldamage4",
	"addskilldamage5",
	"addskilldamage6",
	"dynamicmagicshield_v",
	"addstealfeatureskill",
	"lifereplenish_p",
	"ignoreskill_p",
	"poisondamagereturn_v",  // 毒反弹点
	"poisondamagereturn_p",  // 毒反弹百分比
	"returnskill_p",
	"autoreplyskill",
	"mintimepercastonhorse_v",
	"poison2decmana_p",
	"skill_appendskill",
	"hide",
	"clearnegativestate",
	"returnres_p",
	"dec_percasttimehorse",
	"dec_percasttime",
	"enhance_autoskill",
	"enhance_life_p",
	"enhance_life_v",
	"enhance_711_auto",
	"enhance_714_auto",
	"enhance_717_auto",
	"enhance_723_miss_p",
	"nomagic",
	"skill_collideevent",
	"skill_vanishedevent",
	"skill_startevent",
	"skill_flyevent",
	"block_rate",                          //档格概率
    "enhancehit_rate",                     //重击概率
	"anti_block_rate",                     //=拆解格挡概率：#d1+%
	"anti_enhancehit_rate",                //=抵挡重击概率：#d1+%
	"sorbdamage_p",                        //=抵消伤害:#d1+%
	"anti_poisonres_p",                    //=忽略对方毒防:#d1-%
	"anti_fireres_p",                      //=忽略对方火防:#d1-%
	"anti_lightingres_p",                  //=忽略对方雷防:#d1-%
	"anti_physicsres_p",                   //=忽略对方普防:#d1-%
	"anti_coldres_p",                      //=忽略对方冰防:#d1-%
	"not_add_pkvalue_p",                   //=不增加PK值概率:#d1+%
	"add_boss_damage",                     //=对黄金boss攻击伤害<color=orange>#d1+%<color>
	"five_elements_enhance_v",             //=五行强化值：#d1-点。强化对相克五行的克制效果
    "five_elements_resist_v",              //=五行弱化值：#d1-点。弱化受相克五行的克制效果
	"skill_enhance",                       //=对攻击技能加成：#d1+%(激活的当前技能)
	"anti_allres_p",                       //忽略对方所有抗性:#d1+%
	"add_alldamage_p",                     //加强伤害:#d1+%
	"auto_Revive_rate",                    //自动原地复活
	"addphysicsmagic_p",                   //状态内普百分比
	"addcreatnpc_v",                       //抓捕BB
    "reduceskillcd1",                      //reduceskillcd1=减少#l1-冷却时间#f3-秒
    "reduceskillcd2",                      //reduceskillcd2=减少#l1-冷却时间#f3-秒
    "reduceskillcd3",                      //reduceskillcd3=减少#l1-冷却时间#f3-秒
    "clearallcd",                          //clearallcd=#d1+%的几率清除自己所有技能冷却时间   
	"addblockrate",                        //addblockrate=周围每#d1+个人#d3+%格挡率
    "walkrunshadow",                       //walkrunshadow=凌波微步     
    "returnskill2enemy",                   //returnskill2enemy=被敌人攻击时有#d1+%几率使敌人被技能#l3-攻击
    "manatoskill_enhance",                 //manatoskill_enhance=满内力时对攻击技能加成：#d1+%
	"add_alldamage_v",                     //加深伤害点
	"addskilldamage7",                     //增加对某技能加成
	"ignoreattacrating_v",                 //忽略对方命中率//
    "alljihuo_v",                          //全身激活
	"addexp_v",                            //每间隔 多少秒 增加多少经验
	"doscript_v",                          //触发脚本
    //"addskillexp_v",                     //增加技能熟练度
	"me2metaldamage_p",                    //=对金系伤害增加：#d1+%
    "metal2medamage_p",                    //=减少来自金系的伤害：#d1-%
    "me2wooddamage_p",                     //=对木系伤害增加：#d1+%
    "wood2medamage_p",                     //=减少来自木系的伤害：#d1-%
    "me2waterdamage_p",                    //=对水系伤害增加：#d1+%
    "water2medamage_p",                    //=减少来自水系的伤害：#d1-%
    "me2firedamage_p",                     //=对火系伤害增加：#d1+%
    "fire2medamage_p",                     //=减少来自火系的伤害：#d1-%
    "me2earthdamage_p",                    //=对土系伤害增加：#d1+%
    "earth2medamage_p",                    //=减少来自土系的伤害：#d1-%
	"manareplenish_p",                     //内力恢复百分比
	"fasthitrecover_p",
	"stuntrank_p",                         //眩晕几率减少增加百分比
	"sorbdamage_v",                        //抵消伤害点
	"creatstatus_v",
	"randmove",                             //群体随机走动
	"addbaopoisondmax_p",                   //增加爆毒上限百分比
	"dupotion_v",	                        //药品的减毒状态
    "npcallattackSpeed_v",
	"eqaddskill_v",	                        //带上了就增加什么技能
	"autodeathskill",
	"autorescueskill",
	"staticmagicshield_p",
	"ignorenegativestate_p",
	"poisonres_yan_p",                      //=毒防：#d1+%（阳）
	"fireres_yan_p",                        //=火防：#d1+%（阳）
	"lightingres_yan_p",                    //=雷防：#d1+%（阳）
	"physicsres_yan_p",                     //=普防：#d1+%（阳）
	"coldres_yan_p",                        //=冰防：#d1+%（阳）
	"lifemax_yan_v",                        //=生命最大值：#d1+点（阳）
	"lifemax_yan_p",                        //=生命最大值：#d1+%（阳）
	"manamax_yan_v",                        //=内力最大值：#d1+点（阳）
	"manamax_yan_p",                        //=内力最大值：#d1+%（阳）
	"sorbdamage_yan_p",                     //=抵消伤害：#d1+（阳）
	"fastwalkrun_yan_p",                    //=移动速度：#d1+%（阳）
	"attackspeed_yan_v",                    //=外功系武功出招动作：#d1+%（阳）
	"castspeed_yan_v",                      //=内功系武功出招动作：#d1+%（阳）
	"allres_yan_p",                         //=所有抗性：#d1+%（阳）
	"fasthitrecover_yan_v",                 //=受伤动作时间：#d1~（阳）
	"anti_physicsres_yan_p",                //=忽略对方普防：#d1-%（阳）
	"anti_poisonres_yan_p",                 //=忽略对方毒防：#d1-%（阳）
	"anti_coldres_yan_p",                   //=忽略对方冰防：#d1-%（阳）
	"anti_fireres_yan_p",                   //=忽略对方火防：#d1-%（阳）
	"anti_lightingres_yan_p",               //=忽略对方雷防：#d1-%（阳）
	"anti_allres_yan_p",                    //=忽略对方所有抗性：#d1-%（阳）
    "anti_sorbdamage_yan_p",                //=加深伤害：#d1+（阴阳）
	"anti_hitrecover",		                //=造成受伤动作时间：#d1+
	"do_hurt_p",                            //=造成受伤概率：#d1+%
	"skill_showevent",                   //飞行事件
	"addskillexp1",
	"anti_poisontimereduce_p",              //造对方中毒时间增加
	"anti_stuntimereduce_p",                //造对方眩晕时间增加
	"addskilldamage8",
	"addskilldamage9",
	"addskilldamage10",
	"anti_do_hurt_p",                      //抵抗受伤概率：#d1+%
	"do_stun_p",                           //造成眩晕概率：#d1+%
	"anti_do_stun_p",                      //抵抗眩晕概率：#d1+%
	"meleedamagereturnmana_p",             //受近身攻击伤害时减少敌方内力：#d1+%
	"rangedamagereturnmana_p",             //受远程攻击伤害时减少敌方内力：#d1+%
	"lock_life",
	"special_point",
	"resume_life_p",
	"reset_bufftime",
	"reset_bufftime_recv",
	"cost_sp",
	"cast_when_buff_removed",
	"autocastskill",
	"normal_end",
};
/*
//reduceskillcd1=减少#l1-冷却时间#f3-秒
//reduceskillcd2=减少#l1-冷却时间#f3-秒
//reduceskillcd3=减少#l1-冷却时间#f3-秒
//clearallcd=#d1+%的几率清除自己所有技能冷却时间
//addblockrate=周围每#d1+个人#d3+%格挡率
//walkrunshadow=凌波微步
//returnskill2enemy=被敌人攻击时有#d1+%几率使敌人被技能#l3-攻击
//manatoskill_enhance=满内力时对攻击技能加成：#d1+%


me2metaldamage_p=对金系伤害增加：#d1+%
metal2medamage_p=减少来自金系的伤害：#d1-%
me2wooddamage_p=对木系伤害增加：#d1+%
wood2medamage_p=减少来自木系的伤害：#d1-%
me2waterdamage_p=对水系伤害增加：#d1+%
water2medamage_p=减少来自水系的伤害：#d1-%
me2firedamage_p=对火系伤害增加：#d1+%
fire2medamage_p=减少来自火系的伤害：#d1-%
me2earthdamage_p=对土系伤害增加：#d1+%
earth2medamage_p=减少来自土系的伤害：#d1-%

*/

KMagicDesc	g_MagicDesc;

KMagicDesc::KMagicDesc()
{
	m_szDesc[0] = 0;
}

KMagicDesc::~KMagicDesc()
{
}

BOOL KMagicDesc::Init()
{
//	g_SetFilePath("\\");
	return (m_MIniFile.Load(MAGICDESC_FILE));
}
//获取装备魔法属性的各项内容（\\settings\\MagicDesc.Ini）
const char* KMagicDesc::GetDesc(void *pData,int nType)
{
	
	char szTempDesc[512]={0}; //临时魔法属性 名称！
	int		i = 0;
	ZeroMemory(szTempDesc, sizeof(szTempDesc));
	ZeroMemory(m_szDesc, sizeof(m_szDesc));
	
	if (!pData)
		return NULL;

	KMagicAttrib* pAttrib = (KMagicAttrib *)pData;//1物理地址赋值？  得到原来的魔法属性
	if (!pAttrib)
		return NULL;

	const char	*pszKeyName = g_MagicID2String(pAttrib->nAttribType); // int 类型 是数字 返回 魔法属性 类型（英文）

	m_MIniFile.GetString("Descript",pszKeyName, "", szTempDesc,  sizeof(szTempDesc));//得到（\\settings\\MagicDesc.Ini）中的 对应属性说明
	char*	pTempDesc = szTempDesc;
	const char*	pDescEnd = szTempDesc + sizeof(szTempDesc);	// the walk has an end of its own
	const int		nDescMax = (int)sizeof(m_szDesc) - 8;		// leave room for one more append
	char szMsg[64]={0};
	BOOL nIsFirst=FALSE,nIsTwo=FALSE,nIsthree=FALSE;
	while (pTempDesc < pDescEnd && *pTempDesc && i < nDescMax)
	{
		if (*pTempDesc == '#')
		{
			if (pTempDesc + 3 >= pDescEnd)	// a token is four bytes
				break;
			int	nDescAddType = -1;
			switch(*(pTempDesc + 3))	//第四个字符
			{
			case '+':
				nDescAddType = 1;
				break;
			case '-':
				nDescAddType = 0;
				break;
			case '~':
				nDescAddType = 2;
				break;
			default:
				nDescAddType = -1;
				break;
			}
			int nValue = 0;
			
			switch(*(pTempDesc + 2))  //第三个字符
			{
			case '1':
				nIsFirst = TRUE;
				nValue = pAttrib->nValue[0];  //最小值
				break;
			case '2':
				nIsTwo = TRUE;
				nValue = pAttrib->nValue[1];  //中间值
				break;
			case '3':
				nIsthree = TRUE;
				nValue = pAttrib->nValue[2];  //最大值
				break;
			case '4':
				nIsFirst = TRUE;
				nValue = LOWORD(pAttrib->nValue[0]);//pAttrib->nValue[2];
				break;
			case '6':
				nIsTwo = TRUE;
				nValue = pAttrib->nValue[2];//pAttrib->nValue[2];
				break;
			case '7':
				nIsFirst = TRUE;
				nValue = (short)HIWORD(pAttrib->nValue[0]);  //最大值
				break;
			case '9':
				nIsTwo = TRUE;
				nValue = pAttrib->nValue[1];  //中间值
				break;
			default:
				nIsFirst = TRUE;
				nValue = pAttrib->nValue[0];
				break;
			}
			switch(*(pTempDesc+1))	//第二个字符
			{
			case 'm':		// 门派
				snprintf(m_szDesc + strlen(m_szDesc), sizeof(m_szDesc) - strlen(m_szDesc), "%s", g_Faction.m_sAttribute[nValue].m_szName);
				i += strlen(g_Faction.m_sAttribute[nValue].m_szName);
				break;
			case 's':		// 五行
				switch(nValue)
				{
				case series_metal:
					strcat(m_szDesc, "金系");
					break;
				case series_wood:
					strcat(m_szDesc, "木系");
					break;
				case series_water:
					strcat(m_szDesc, "水系");
					break;
				case series_fire:
					strcat(m_szDesc, "火系");
					break;
				case series_earth:
					strcat(m_szDesc, "土系");
					break;
				default:
					strcat(m_szDesc, "无系");
					break;
				}
				i += 4;
				break;
			case 'k':		// 消耗类型
				switch(nValue)
				{
				case 0:
					strcat(m_szDesc, "内力");
					break;
				case 1:
					strcat(m_szDesc, "生命");
					break;
				case 2:
					strcat(m_szDesc, "体力");
					break;
				case 3:
					strcat(m_szDesc, "金钱");
					break;
				default:
					strcat(m_szDesc, "内力");
					break;
				}
				i += 4;
				break;
			case 'd':		// 数字
				{
					BOOL nIsFu=FALSE;
					switch(nDescAddType)
					{
					case -1:
						break;
					case 0:	//减
						if (nValue >= 0)
						{
							strcat(m_szDesc, "增加");
							//i += 1;
						}
						else
						{
							   nValue = -nValue;
							   nIsFu=TRUE;
							   strcat(m_szDesc, "减少"); 
						} 
						i += 4;
						break;
					case 1:	 //加
						if (nValue >= 0)
						{
							strcat(m_szDesc, "增加");
							//i += 1;
						}
						else
						{
							nValue = -nValue;
							nIsFu=TRUE;
							strcat(m_szDesc, "减少");
						} 
						i += 4;	
						break;
					case 2:	 //~
						if (nValue >= 0)
						{
							strcat(m_szDesc, "\0");	
						}
						else
						{
							nValue = -nValue;
							nIsFu=TRUE;
							//strcat(m_szDesc, "-");
							//i += 1;
						} 
						break;
					case 7:				
						break;
					case 9:
						break;
					default:
						strcat(m_szDesc, "默认");
						i += 4;
						break;
					}

					if (nIsFu && nValue==1)
					{
						if (nIsTwo)       //第二个值为 -1	 strstr(m_szDesc,"概率")
							sprintf(szMsg, "100");
						else
						    sprintf(szMsg, "%d", nValue);
					}
					else
					   sprintf(szMsg, "%d", nValue);

					snprintf(m_szDesc + strlen(m_szDesc), sizeof(m_szDesc) - strlen(m_szDesc), "%s", szMsg);
					i += strlen(szMsg);
				}
				break;
			case 'x':		// 性别
				if (nValue)
			     	strcat(m_szDesc, "女性");
				else
					strcat(m_szDesc, "男性");
				i += 4;
				break;
			case 'f':		//
				/*if (nValue)
					strcat(m_szDesc, "%d",nValue);
				else
					strcat(m_szDesc, "测试");*/
				
				sprintf(szMsg, "%d", nValue);
				snprintf(m_szDesc + strlen(m_szDesc), sizeof(m_szDesc) - strlen(m_szDesc), "%s", szMsg);
				i += strlen(szMsg);
				break;
			case 'l':		// 技能
				{//nValue[2]
					//char	szMsg[32];
					/*if (nValue > 0 && nValue<=2000)
					{
						ISkill* pSkill =  g_SkillManager.GetSkill(nValue, 1);
						sprintf(szMsg, "<color=HGreen>[%s]<color>", pSkill->GetSkillName());
					}
				    else */
					if (nValue>1)
					{
					//	KTabFile nMagci;
					//	int nSkillid=0;
					//	if (nMagci.Load(TABFILE_MAGICAGOLD_PATH))
						{
                    //      nMagci.GetInteger(nValue,"参数3最小值",0,&nValue);
						  ISkill* pSkill =  g_SkillManager.GetSkill(nValue, 1);
						  if (pSkill)
						      snprintf(szMsg, sizeof(szMsg), "<color=HGreen>[%s]<color>", pSkill->GetSkillName());
						  else
						      sprintf(szMsg, "<color=red>[技能有误]<color>");
						}
				    //	else
					//	{
					//	  sprintf(szMsg, "<color=blue>[数据有误](%d)<color>", nValue);
					//	} 

					//	nMagci.Clear();
					}
					else
						sprintf(szMsg, "<color=HGreen>[所有]<color>"); //HGreen

					    snprintf(m_szDesc + strlen(m_szDesc), sizeof(m_szDesc) - strlen(m_szDesc), "%s", szMsg);
					    i += strlen(szMsg);
				}
				break;
			default:
				break;
			}
			pTempDesc += 4;
		}
		else
		{
			m_szDesc[i] = *pTempDesc;
			pTempDesc++;
			i++;
		}
	}
	return m_szDesc; //返回一个字符串
}
//获取装备魔法属性的各项内容（\\settings\\BaoShiMagicDesc.Ini）
const char* KMagicDesc::BaoShiGetDesc(void *pData)
{
	
	char	szTempDesc[256]; //临时魔法属性 名称！
	int		i = 0;
	ZeroMemory(m_szDesc, 256); //分配内存
	
	if (!pData)
		return NULL;

	KMagicAttrib* pAttrib = (KMagicAttrib *)pData;//1物理地址赋值？  得到原来的魔法属性
//
	const char	*pszKeyName = g_MagicID2String(pAttrib->nAttribType); // int 类型 是数字 返回 魔法属性 类型（英文）
	g_MagicBaoshiDesc.GetString("Descript",pszKeyName,"", szTempDesc, 256);//得到（\\settings\\MagicDesc.Ini）中的 对应属性说明
	char*	pTempDesc = szTempDesc;
	const char*	pDescEnd = szTempDesc + sizeof(szTempDesc);	// the walk has an end of its own
	const int		nDescMax = (int)sizeof(m_szDesc) - 8;		// leave room for one more append
	while (pTempDesc < pDescEnd && *pTempDesc && i < nDescMax)
	{
		if (*pTempDesc == '#')
		{
			if (pTempDesc + 3 >= pDescEnd)	// a token is four bytes
				break;
			int	nDescAddType = -1;
			switch(*(pTempDesc + 3)) //第四个字符
			{
			case '+':
				nDescAddType = 1;
				break;
			case '-':
				nDescAddType = 0;
				break;
			case '~':
				nDescAddType = 2;
				break;
			default:
				nDescAddType = -1;
				break;
			}
			int nValue = 0;
			
			switch(*(pTempDesc + 2))   //第三个字符
			{
			case '1':
				nValue = pAttrib->nValue[0];//最小值
				break;
			case '2':
				nValue = pAttrib->nValue[1];  
				break;
			case '3':
				nValue = pAttrib->nValue[2];  //最大值
				break;
			default:
				nValue = pAttrib->nValue[0];
				break;
			}
			switch(*(pTempDesc+1))	   //第二个字符
			{
			case 'm':		// 门派
				snprintf(m_szDesc + strlen(m_szDesc), sizeof(m_szDesc) - strlen(m_szDesc), "%s", g_Faction.m_sAttribute[nValue].m_szName);
				i += strlen(g_Faction.m_sAttribute[nValue].m_szName);
				break;
			case 's':		// 五行
				switch(nValue)
				{
				case series_metal:
					strcat(m_szDesc, "金系");
					break;
				case series_wood:
					strcat(m_szDesc, "木系");
					break;
				case series_water:
					strcat(m_szDesc, "水系");
					break;
				case series_fire:
					strcat(m_szDesc, "火系");
					break;
				case series_earth:
					strcat(m_szDesc, "土系");
					break;
				default:
					strcat(m_szDesc, "无系");
					break;
				}
				i += 4;
				break;
			case 'k':		// 消耗类型
				switch(nValue)
				{
				case 0:
					strcat(m_szDesc, "内力");
					break;
				case 1:
					strcat(m_szDesc, "生命");
					break;
				case 2:
					strcat(m_szDesc, "体力");
					break;
				case 3:
					strcat(m_szDesc, "金钱");
					break;
				default:
					strcat(m_szDesc, "内力");
					break;
				}
				i += 4;
				break;
			case 'd':		// 数字
				{
					switch(nDescAddType)
					{
					case 0:
						if (nValue >= 0)
						{
							strcat(m_szDesc, "+");
						}
						else
						{
							nValue = -nValue;
							strcat(m_szDesc, "-");
						}
						i += 1;
						break;
					case 1:
						if (nValue >= 0)
						{
							strcat(m_szDesc, "+");
						}
						else
						{
							nValue = -nValue;
							strcat(m_szDesc, "-");
						}
						i += 1;
						break;
					case 2:
						strcat(m_szDesc, "\0");					
					//    i += 1;
						break;
					default:
						break;
					}
					char	szMsg[16];
					snprintf(szMsg, sizeof(szMsg), "%d", nValue);
					snprintf(m_szDesc + strlen(m_szDesc), sizeof(m_szDesc) - strlen(m_szDesc), "%s", szMsg);
					i += strlen(szMsg);
				}
				break;
			case 'x':		// 性别
				if (nValue)
			     	strcat(m_szDesc, "女性");
				else
					strcat(m_szDesc, "男性");
				i += 4;
				break;
			case 'f':		// 性别
				if (nValue)
					strcat(m_szDesc, "测试");
				else
					strcat(m_szDesc, "测试");
				i += 4;
				break;
			case 'l':		// 技能
				{//nValue[2]
					char	szMsg[32];
				/*if (nValue > 0 && nValue<=2000)
					{
						ISkill* pSkill =  g_SkillManager.GetSkill(nValue, 1);
						sprintf(szMsg, "<color=HGreen>[%s]<color>", pSkill->GetSkillName());
					}
				    else */
					if (nValue>1)
					{
					//	KTabFile nMagci;
					//	int nSkillid=0;
					//	if (nMagci.Load(TABFILE_MAGICAGOLD_PATH))
					//	{
					//		nMagci.GetInteger(nValue,"参数3最小值",0,&nSkillid);
							ISkill* pSkill =NULL; 
								    pSkill =  g_SkillManager.GetSkill(nValue, 1);
							if (pSkill)
							   snprintf(szMsg, sizeof(szMsg), "<color=blue>[%s]<color>", pSkill->GetSkillName());
							else
							   sprintf(szMsg, "<color=red>[技能错误]<color>");
							//sprintf(szMsg, "<color=blue>[技能有误](%d)<color>", nSkillid);
					//	}
					//	else
					//	{
					//		sprintf(szMsg, "<color=HGreen>[数据有误](%d)<color>", nValue);
					//	} 
					//	nMagci.Clear();
					}
					else
						//sprintf(szMsg, "%s");
						sprintf(szMsg, "<color=HGreen>[所有]<color>"); //HGreen

					snprintf(m_szDesc + strlen(m_szDesc), sizeof(m_szDesc) - strlen(m_szDesc), "%s", szMsg);
					i += strlen(szMsg);
				}
				break;
			default:
				break;
			}
			pTempDesc += 4;
		}
		else
		{
			m_szDesc[i] = *pTempDesc;
			pTempDesc++;
			i++;
		}
	}
	return m_szDesc; //返回一个字符串
}

const char * g_MagicID2String(int nAttrib)  // 将数字 转化成 魔法属性对应的编号
{
	if ((nAttrib < 0) || nAttrib >= magic_normal_end) 
	return MAGIC_ATTRIB_STRING[magic_normal_end];

	return 	MAGIC_ATTRIB_STRING[nAttrib];  // 返回对应的 魔法类型字符串
}
//获取魔法属性ID
int	g_String2MagicID(char * szMagicAttribName)
{
	if ((!szMagicAttribName) || (!szMagicAttribName[0])) 
		return -1;

	//nValue2 当值为-1时为永久性状态，0为非状态，其它值为有时效性状态魔法效果
	//需要将状态数据与非状态数据分离出来，放入相应的数组内，并记录总数量
	
	for (int i  = 0 ; i <= magic_normal_end; i ++)
	{
		if (!strcmp(szMagicAttribName, g_MagicID2String(i)))  //获取属性名
			return i;
	}
	return -1;
}


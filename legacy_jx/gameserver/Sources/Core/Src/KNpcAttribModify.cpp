#include "KCore.h"
#include "KNpc.h"
#include "KPlayer.h"
#include "KNpcAttribModify.h"
#include "KNpcTemplate.h"
#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#define	defNORMAL_COLD_TIME	60

#ifndef _SERVER
#include "KNpcAI.h"
#endif 

KNpcAttribModify	g_NpcAttribModify;

KNpcAttribModify::KNpcAttribModify()
{
	ZeroMemory(ProcessFunc, sizeof(ProcessFunc));
	ProcessFunc[magic_physicsresmax_p] = &KNpcAttribModify::PhysicsResMaxP;
	ProcessFunc[magic_coldresmax_p] = &KNpcAttribModify::ColdResMaxP;
	ProcessFunc[magic_fireresmax_p] = &KNpcAttribModify::FireResMaxP;
	ProcessFunc[magic_lightingresmax_p] = &KNpcAttribModify::LightingResMaxP;
	ProcessFunc[magic_poisonresmax_p] = &KNpcAttribModify::PoisonResMaxP;
	ProcessFunc[magic_allresmax_p] = &KNpcAttribModify::AllResMaxP;
	ProcessFunc[magic_lifepotion_v] = &KNpcAttribModify::LifePotionV;
	ProcessFunc[magic_manapotion_v] = &KNpcAttribModify::ManaPotionV;
	ProcessFunc[magic_meleedamagereturn_v] = &KNpcAttribModify::MeleeDamageReturnV;
	ProcessFunc[magic_meleedamagereturn_p] = &KNpcAttribModify::MeleeDamageReturnP;
	ProcessFunc[magic_rangedamagereturn_v] = &KNpcAttribModify::RangeDamageReturnV;
	ProcessFunc[magic_rangedamagereturn_p] = &KNpcAttribModify::RangeDamageReturnP;
	ProcessFunc[magic_damagetomana_p] = &KNpcAttribModify::Damage2ManaP;        //伤害转化为内力增加
	ProcessFunc[magic_adddefense_v] = &KNpcAttribModify::ArmorDefenseV;			// 与装备加防御统一处理
	ProcessFunc[magic_poisonenhance_p] = &KNpcAttribModify::PoisonEnhanceP;     //毒发间隔时间
	ProcessFunc[magic_lightingenhance_p] = &KNpcAttribModify::LightingEnhanceP; //加强内雷
	ProcessFunc[magic_fireenhance_p] = &KNpcAttribModify::FireEnhanceP;         //加强内火
	ProcessFunc[magic_coldenhance_p] = &KNpcAttribModify::ColdEnhanceP;         //增加迟缓时间百分比
	ProcessFunc[magic_armordefense_v] = &KNpcAttribModify::ArmorDefenseV;
	ProcessFunc[magic_lifemax_v] = &KNpcAttribModify::LifeMaxV;
	ProcessFunc[magic_lifemax_p] = &KNpcAttribModify::LifeMaxP;                 //生命最大值
	ProcessFunc[magic_life_v] = &KNpcAttribModify::LifeV;
	ProcessFunc[magic_lifereplenish_v] = &KNpcAttribModify::LifeReplenishV;     //生命恢复力 装备
	ProcessFunc[magic_manamax_v] = &KNpcAttribModify::ManaMaxV;
	ProcessFunc[magic_manamax_p] = &KNpcAttribModify::ManaMaxP;
	ProcessFunc[magic_mana_v] = &KNpcAttribModify::ManaV;
	ProcessFunc[magic_manareplenish_v] = &KNpcAttribModify::ManaReplenishV;
	ProcessFunc[magic_staminamax_v] = &KNpcAttribModify::StaminaMaxV;
	ProcessFunc[magic_staminamax_p] = &KNpcAttribModify::StaminaMaxP;
	ProcessFunc[magic_stamina_v] = &KNpcAttribModify::StaminaV;
	ProcessFunc[magic_staminareplenish_v] = &KNpcAttribModify::StaminaReplenishV;
	ProcessFunc[magic_strength_v] = &KNpcAttribModify::StrengthV;
	ProcessFunc[magic_dexterity_v] = &KNpcAttribModify::DexterityV;
	ProcessFunc[magic_vitality_v] = &KNpcAttribModify::VitalityV;
	ProcessFunc[magic_energy_v] = &KNpcAttribModify::EnergyV;
	ProcessFunc[magic_poisonres_p] = &KNpcAttribModify::PoisonresP;
	ProcessFunc[magic_fireres_p] = &KNpcAttribModify::FireresP;
	ProcessFunc[magic_lightingres_p] = &KNpcAttribModify::LightingresP;
	ProcessFunc[magic_physicsres_p] = &KNpcAttribModify::PhysicsresP;
	ProcessFunc[magic_coldres_p] = &KNpcAttribModify::ColdresP;
	ProcessFunc[magic_freezetimereduce_p] = &KNpcAttribModify::FreezeTimeReduceP;
	ProcessFunc[magic_burntimereduce_p] = &KNpcAttribModify::BurnTimeReduceP;
	ProcessFunc[magic_poisontimereduce_p] = &KNpcAttribModify::PoisonTimeReduceP;
	ProcessFunc[magic_poisondamagereduce_v] = &KNpcAttribModify::PoisonDamageReduceV;
	ProcessFunc[magic_stuntimereduce_p] = &KNpcAttribModify::StunTimeReduceP;
	ProcessFunc[magic_fastwalkrun_p] = &KNpcAttribModify::FastWalkRunP;
	ProcessFunc[magic_visionradius_p] = &KNpcAttribModify::VisionRadiusP;
	ProcessFunc[magic_fasthitrecover_v] = &KNpcAttribModify::FastHitRecoverV;        //受伤动作时间
	ProcessFunc[magic_allres_p] = &KNpcAttribModify::AllresP;
	ProcessFunc[magic_attackratingenhance_v] = &KNpcAttribModify::AddAttackRatingV;   //状态装备命中点加强
	ProcessFunc[magic_attackratingenhance_p] = &KNpcAttribModify::AddAttackRatingP;   //状态装备命中百分比加强
	ProcessFunc[magic_attackspeed_v] = &KNpcAttribModify::AttackSpeedV;               //外功攻速
	ProcessFunc[magic_castspeed_v] = &KNpcAttribModify::CastSpeedV;                   //内功攻速
	//=====================================攻击技能数据==============================================
	ProcessFunc[magic_attackrating_v] = &KNpcAttribModify::AttackRatingV;             //技能命中率
	ProcessFunc[magic_attackrating_p] = &KNpcAttribModify::AttackRatingP;             //攻击技能命中百分比
	ProcessFunc[magic_ignoredefense_p] = &KNpcAttribModify::Ignoredefense_p;          //忽略敌人闪避率	
	ProcessFunc[magic_physicsdamage_v] = &KNpcAttribModify::AddPhysicsMagic;          //普攻伤害:#d1+点
	ProcessFunc[magic_colddamage_v] = &KNpcAttribModify::AddColdMagic;                //攻击冰点
	ProcessFunc[magic_firedamage_v] = &KNpcAttribModify::AddFireMagic;                //攻击火点
	ProcessFunc[magic_lightingdamage_v] = &KNpcAttribModify::AddLightingMagic;        //攻击雷点
	ProcessFunc[magic_poisondamage_v] = &KNpcAttribModify::AddPoisonMagic;            //攻击毒点
	ProcessFunc[magic_physicsenhance_p]  = &KNpcAttribModify::Add_neiphysicsenhance_p;//普点百分比
	//====================新加数据暂时没用===========================================================
	/*ProcessFunc[magic_xinphysicsdamage_v] = &KNpcAttribModify::XinPhysicsDamageV;      //新技能普点
	ProcessFunc[magic_xinfiredamage_v] = &KNpcAttribModify::XinFireDamageV;            //新技能火点
	ProcessFunc[magic_xincolddamage_v] = &KNpcAttribModify::XinColdDamageV;            //新技能冰点
	ProcessFunc[magic_xinlightingdamage_v] = &KNpcAttribModify::XinLightingDamageV;    //新技能雷点
	ProcessFunc[magic_xinpoisondamage_v] = &KNpcAttribModify::XinPoisonDamageV;        //新技能毒点 
	ProcessFunc[magic_xinphysicsdamage_p] = &KNpcAttribModify::XinPhysicsDamageP;      //新技能普百分比
	*/
	//======================================状态数据=================================================
	ProcessFunc[magic_addphysicsdamage_p] = &KNpcAttribModify::AddPhysicsDamagePP;    //外普百分比-被动辅助状态
	ProcessFunc[magic_addphysicsdamage_v] = &KNpcAttribModify::Addphysicsdamagevp;    //外普点-被动辅助状态
	ProcessFunc[magic_addfiredamage_v] = &KNpcAttribModify::Aaddfiredamagevp;         //外火点-被动辅助状态
	ProcessFunc[magic_addcolddamage_v] = &KNpcAttribModify::Addcolddamagevp;          //外冰点-被动辅助状态
    ProcessFunc[magic_addpoisondamage_v] = &KNpcAttribModify::Addpoisondamagevp;      //外毒点-被动辅助状态
	ProcessFunc[magic_addlightingdamage_v] = &KNpcAttribModify::Addlightingdamagevp;  //外雷点-被动辅助状态
    ProcessFunc[magic_addphysicsmagic_v] = &KNpcAttribModify::Addphysicsmagicvb;      //内普点-被动辅助状态
	ProcessFunc[magic_addcoldmagic_v] = &KNpcAttribModify::Addcoldmagicvp;            //内冰点-被动辅助状态
	ProcessFunc[magic_addfiremagic_v] = &KNpcAttribModify::Addfiremagicv;             //内火点-被动辅助状态
	ProcessFunc[magic_addlightingmagic_v] = &KNpcAttribModify::Addlightingmagicv;     //内雷点-被动辅助状态
	ProcessFunc[magic_addpoisonmagic_v] = &KNpcAttribModify::Addpoisonmagicv;         //内毒点-被动辅助状态
	ProcessFunc[magic_addphysicsmagic_p] = &KNpcAttribModify::Addphysicsmagicp;       //内普百分比-被动辅助状态
	//================================================================================================
	ProcessFunc[magic_slowmissle_b] = &KNpcAttribModify::SlowMissleB;
	ProcessFunc[magic_changecamp_b] = &KNpcAttribModify::ChangeCampV;
	ProcessFunc[magic_physicsarmor_v] = &KNpcAttribModify::PhysicsArmorV;
	ProcessFunc[magic_coldarmor_v] = &KNpcAttribModify::ColdArmorV;
	ProcessFunc[magic_firearmor_v] = &KNpcAttribModify::FireArmorV;
	ProcessFunc[magic_poisonarmor_v] = &KNpcAttribModify::PoisonArmorV;
	ProcessFunc[magic_lightingarmor_v] = &KNpcAttribModify::LightingArmorV;
	ProcessFunc[magic_lucky_v] = &KNpcAttribModify::LuckyV;
	ProcessFunc[magic_steallife_p] = &KNpcAttribModify::StealLifeP;
	ProcessFunc[magic_steallifeenhance_p] = &KNpcAttribModify::StealLifeP;
	ProcessFunc[magic_stealstamina_p] = &KNpcAttribModify::StealStaminaP;
	ProcessFunc[magic_stealstaminaenhance_p] = &KNpcAttribModify::StealStaminaP;
	ProcessFunc[magic_stealmana_p] = &KNpcAttribModify::StealManaP;
	ProcessFunc[magic_stealmanaenhance_p] = &KNpcAttribModify::StealManaP;
	ProcessFunc[magic_allskill_v] = &KNpcAttribModify::AllSkillV;
	ProcessFunc[magic_metalskill_v] = &KNpcAttribModify::MetalSkillV;
	ProcessFunc[magic_woodskill_v] = &KNpcAttribModify::WoodSkillV;
	ProcessFunc[magic_waterskill_v] = &KNpcAttribModify::WaterSkillV;
	ProcessFunc[magic_fireskill_v] = &KNpcAttribModify::FireSkillV;
	ProcessFunc[magic_earthskill_v] = &KNpcAttribModify::EarthSkillV;
	ProcessFunc[magic_knockback_p] = &KNpcAttribModify::KnockBackP; //震退百分比增加
	ProcessFunc[magic_knockbackenhance_p] = &KNpcAttribModify::KnockBackP;  //震退百分比增加
	ProcessFunc[magic_fatallystrike_p] = &KNpcAttribModify::DeadlyStrikeP;        //致命一击（攻击技能）
	ProcessFunc[magic_fatallystrikeenhance_p] =&KNpcAttribModify::DeadlyStrikeP;  //致命一击（状态）
	ProcessFunc[magic_deadlystrike_p] = &KNpcAttribModify::DeadlyStrikeP;         //会心一击百分比增加（攻击技能）
	ProcessFunc[magic_deadlystrikeenhance_p] = &KNpcAttribModify::DeadlyStrikeP;  //攻击会心一击率（状态）
	ProcessFunc[magic_badstatustimereduce_v] = &KNpcAttribModify::BadStatusTimeReduceV; //不良状态时间增加
	ProcessFunc[magic_manashield_p] = &KNpcAttribModify::ManaShieldP;             //内力吸收伤害 manashield_p 坐忘
	ProcessFunc[magic_fatallystrikeres_p] = &KNpcAttribModify::fatallystrikeresP;
	ProcessFunc[magic_addskilldamage1] = &KNpcAttribModify::addskilldamage1;
	ProcessFunc[magic_addskilldamage2] = &KNpcAttribModify::addskilldamage2;
    ProcessFunc[magic_expenhance_p] = &KNpcAttribModify::expenhanceP;//杀死敌人获得经验 加 
	ProcessFunc[magic_addskilldamage3] = &KNpcAttribModify::addskilldamage3;
	ProcessFunc[magic_addskilldamage4] = &KNpcAttribModify::addskilldamage4;
	ProcessFunc[magic_addskilldamage5] = &KNpcAttribModify::addskilldamage5;
	ProcessFunc[magic_addskilldamage6] = &KNpcAttribModify::addskilldamage6;
	ProcessFunc[magic_dynamicmagicshield_v] = &KNpcAttribModify::dynamicmagicshieldV;
	ProcessFunc[magic_addstealfeatureskill] = &KNpcAttribModify::addstealfeatureskill;
	ProcessFunc[magic_lifereplenish_p] = &KNpcAttribModify::lifereplenishP;
	ProcessFunc[magic_ignoreskill_p] = &KNpcAttribModify::ignoreskillP;
	ProcessFunc[magic_poisondamagereturn_v] = &KNpcAttribModify::poisondamagereturnV;  //毒反弹点
	ProcessFunc[magic_poisondamagereturn_p] = &KNpcAttribModify::poisondamagereturnP;  //毒反弹百分比
	ProcessFunc[magic_returnskill_p] = &KNpcAttribModify::returnskillP;
	ProcessFunc[magic_autoreplyskill] = &KNpcAttribModify::autoreplyskill;
	ProcessFunc[magic_skill_mintimepercast_v] = &KNpcAttribModify::skill_mintimepercastV;
	ProcessFunc[magic_mintimepercastonhorse_v] = &KNpcAttribModify::mintimepercastonhorseV;// 骑马释放间隔时间
	ProcessFunc[magic_poison2decmana_p] = &KNpcAttribModify::poison2decmanaP;	 //损失内力
	ProcessFunc[magic_skill_appendskill] = &KNpcAttribModify::skill_appendskil;  //包含技能
	ProcessFunc[magic_hide] = &KNpcAttribModify::hide;
	ProcessFunc[magic_clearnegativestate] = &KNpcAttribModify::clearnegativestate;
	ProcessFunc[magic_returnres_p] = &KNpcAttribModify::returnresP;
	ProcessFunc[magic_dec_percasttimehorse] = &KNpcAttribModify::decPercasttimehorse;//减少#l3-冷却时间:马下<color=orange>#f7-秒<color>,马上<color=orange>#f4-秒<color>
	ProcessFunc[magic_dec_percasttime] = &KNpcAttribModify::decPercasttime;
	ProcessFunc[magic_enhance_autoSkill] = &KNpcAttribModify::enhance_autoSkill;
	ProcessFunc[magic_enhance_life_p] = &KNpcAttribModify::enhance_lifeP;
	ProcessFunc[magic_enhance_life_v] = &KNpcAttribModify::enhance_lifeV;
	ProcessFunc[magic_enhance_711_auto] = &KNpcAttribModify::enhance_711_auto;
	ProcessFunc[magic_enhance_714_auto] = &KNpcAttribModify::enhance_714_auto;
	ProcessFunc[magic_enhance_717_auto] = &KNpcAttribModify::enhance_717_auto;
	ProcessFunc[magic_enhance_723_miss_p] = &KNpcAttribModify::enhance_723_missP;
	ProcessFunc[magic_seriesdamage_p] = &KNpcAttribModify::SerisesDamage;
	ProcessFunc[magic_autoattackskill] = &KNpcAttribModify::autoskill;
	ProcessFunc[magic_block_rate] = &KNpcAttribModify::block_rate;
	ProcessFunc[magic_enhancehit_rate] = &KNpcAttribModify::enhancehit_rate;
	ProcessFunc[magic_anti_block_rate] = &KNpcAttribModify::anti_block_rate;                   //=拆解格挡概率：#d1+%
	ProcessFunc[magic_anti_enhancehit_rate] = &KNpcAttribModify::anti_enhancehit_rate;         //=抵挡重击概率：#d1+%
	ProcessFunc[magic_sorbdamage_p] = &KNpcAttribModify::sorbdamage_p;                         //=抵消伤害:#d1+
	ProcessFunc[magic_anti_poisonres_p] = &KNpcAttribModify::anti_poisonres_p;                 //=忽略对方毒防:#d1-%
	ProcessFunc[magic_anti_fireres_p] = &KNpcAttribModify::anti_fireres_p;                     //=忽略对方火防:#d1-%
	ProcessFunc[magic_anti_lightingres_p] = &KNpcAttribModify::anti_lightingres_p;             //=忽略对方雷防:#d1-%
	ProcessFunc[magic_anti_physicsres_p] = &KNpcAttribModify::anti_physicsres_p;               //=忽略对方普防:#d1-%
	ProcessFunc[magic_anti_coldres_p] = &KNpcAttribModify::anti_coldres_p;                     //=忽略对方冰防:#d1-%
	ProcessFunc[magic_not_add_pkvalue_p] = &KNpcAttribModify::not_add_pkvalue_p;               //=不增加PK值概率:#d1+%
	ProcessFunc[magic_add_boss_damage] = &KNpcAttribModify::add_boss_damage;                   //=对黄金boss攻击伤害<color=orange>#d1+%<color>
	ProcessFunc[magic_five_elements_enhance_v] = &KNpcAttribModify::five_elements_enhance_v;   //=五行强化值：#d1-点。强化对相克五行的克制效果
    ProcessFunc[magic_five_elements_resist_v] = &KNpcAttribModify::five_elements_resist_v;     //=五行弱化值：#d1-点。弱化受相克五行的克制效果
    ProcessFunc[magic_skill_enhance] = &KNpcAttribModify::skill_enhance;                       //对攻击技能加成
    ProcessFunc[magic_anti_allres_p] = &KNpcAttribModify::anti_allres_p; 
    ProcessFunc[magic_add_alldamage_p] = &KNpcAttribModify::add_alldamage_p;                   //加强伤害
    ProcessFunc[magic_auto_Revive_rate] = &KNpcAttribModify::auto_Revive_rate; 
    ProcessFunc[magic_addcreatnpc_v]  = &KNpcAttribModify::addcreatnpc_v;  //抓捕被动BB
    ProcessFunc[magic_addzhuabu_v]  = &KNpcAttribModify::addzhuabu_v;      //抓捕主动BB
    ProcessFunc[magic_reduceskillcd1]  = &KNpcAttribModify::reduceskillcd1;   
	ProcessFunc[magic_reduceskillcd2]  = &KNpcAttribModify::reduceskillcd2;
	ProcessFunc[magic_reduceskillcd3]  = &KNpcAttribModify::reduceskillcd3;
	ProcessFunc[magic_clearallcd]  = &KNpcAttribModify::clearallcd;
	ProcessFunc[magic_addblockrate]  = &KNpcAttribModify::addblockrate;
	ProcessFunc[magic_walkrunshadow]  = &KNpcAttribModify::walkrunshadow;
	ProcessFunc[magic_returnskill2enemy]  = &KNpcAttribModify::returnskill2enemy;
	ProcessFunc[magic_manatoskill_enhance]  = &KNpcAttribModify::manatoskill_enhance;
	ProcessFunc[magic_add_alldamage_v]  = &KNpcAttribModify::add_alldamage_v;
	ProcessFunc[magic_addskilldamage7]  = &KNpcAttribModify::addskilldamage7;
	ProcessFunc[magic_ignoreattacrating_v]  = &KNpcAttribModify::ignoreattacrating_v;
	ProcessFunc[magic_alljihuo_v]  = &KNpcAttribModify::alljihuo_v;
	ProcessFunc[magic_addexp_v]  = &KNpcAttribModify::addexp_v;
	ProcessFunc[magic_doscript_v]  = &KNpcAttribModify::doscript_v;
    ProcessFunc[magic_me2metaldamage_p]  = &KNpcAttribModify::me2metaldamage_p;
	ProcessFunc[magic_metal2medamage_p]  = &KNpcAttribModify::metal2medamage_p;
	ProcessFunc[magic_me2wooddamage_p]  = &KNpcAttribModify::me2wooddamage_p;
	ProcessFunc[magic_wood2medamage_p]  = &KNpcAttribModify::wood2medamage_p;
	ProcessFunc[magic_me2waterdamage_p]  = &KNpcAttribModify::me2waterdamage_p;
	ProcessFunc[magic_water2medamage_p]  = &KNpcAttribModify::water2medamage_p;
	ProcessFunc[magic_me2firedamage_p]  = &KNpcAttribModify::me2firedamage_p;
	ProcessFunc[magic_fire2medamage_p]  = &KNpcAttribModify::fire2medamage_p;
	ProcessFunc[magic_me2earthdamage_p]  = &KNpcAttribModify::me2earthdamage_p;
	ProcessFunc[magic_earth2medamage_p]  = &KNpcAttribModify::earth2medamage_p;
	ProcessFunc[magic_manareplenish_p] = &KNpcAttribModify::ManaReplenishp;
	ProcessFunc[magic_fasthitrecover_p] = &KNpcAttribModify::fasthitrecover_p;
    ProcessFunc[magic_stuntrank_p] = &KNpcAttribModify::stuntrank_p;
	ProcessFunc[magic_sorbdamage_v] = &KNpcAttribModify::sorbdamage_v;
    ProcessFunc[magic_creatstatus_v] = &KNpcAttribModify::creatstatus_v;
	ProcessFunc[magic_randmove] = &KNpcAttribModify::randmove;
	ProcessFunc[magic_addbaopoisondmax_p] = &KNpcAttribModify::addbaopoisondmax_p;
	ProcessFunc[magic_dupotion_v] = &KNpcAttribModify::dupotion_v;
	ProcessFunc[magic_npcallattackSpeed_v] = &KNpcAttribModify::npcallattackSpeed_v;
	ProcessFunc[magic_eqaddskill_v] = &KNpcAttribModify::eqaddskill_v;
	ProcessFunc[magic_autodeathskill] = &KNpcAttribModify::autodeathskill;
	ProcessFunc[magic_autorescueskill] = &KNpcAttribModify::autorescueskill;
	ProcessFunc[magic_staticmagicshield_p] = &KNpcAttribModify::staticmagicshield_p;
	ProcessFunc[magic_ignorenegativestate_p] = &KNpcAttribModify::ignorenegativestate_p;
	//=============================阴阳属性==================================
	ProcessFunc[magic_poisonres_yan_p] = &KNpcAttribModify::poisonres_yan_p;
	ProcessFunc[magic_fireres_yan_p] = &KNpcAttribModify::fireres_yan_p;
	ProcessFunc[magic_lightingres_yan_p] = &KNpcAttribModify::lightingres_yan_p;
	ProcessFunc[magic_physicsres_yan_p] = &KNpcAttribModify::physicsres_yan_p;
	ProcessFunc[magic_coldres_yan_p] = &KNpcAttribModify::coldres_yan_p;
	ProcessFunc[magic_lifemax_yan_v] = &KNpcAttribModify::lifemax_yan_v;
	ProcessFunc[magic_lifemax_yan_p] = &KNpcAttribModify::lifemax_yan_p;
	ProcessFunc[magic_sorbdamage_yan_p] = &KNpcAttribModify::sorbdamage_yan_p;
	ProcessFunc[magic_attackspeed_yan_v] = &KNpcAttribModify::attackspeed_yan_v;
	ProcessFunc[magic_castspeed_yan_v] = &KNpcAttribModify::castspeed_yan_v;
	ProcessFunc[magic_allres_yan_p] = &KNpcAttribModify::allres_yan_p;
	ProcessFunc[magic_fasthitrecover_yan_v] = &KNpcAttribModify::fasthitrecover_yan_v;
	ProcessFunc[magic_anti_physicsres_yan_p] = &KNpcAttribModify::anti_physicsres_yan_p;
	ProcessFunc[magic_anti_poisonres_yan_p] = &KNpcAttribModify::anti_poisonres_yan_p;
	ProcessFunc[magic_anti_coldres_yan_p] = &KNpcAttribModify::anti_coldres_yan_p;
	ProcessFunc[magic_anti_fireres_yan_p] = &KNpcAttribModify::anti_fireres_yan_p;
	ProcessFunc[magic_anti_lightingres_yan_p] = &KNpcAttribModify::anti_lightingres_yan_p;
	ProcessFunc[magic_anti_allres_yan_p] = &KNpcAttribModify::anti_allres_yan_p;
	ProcessFunc[magic_anti_sorbdamage_yan_p] = &KNpcAttribModify::anti_sorbdamage_yan_p;
	ProcessFunc[magic_anti_hitrecover] = &KNpcAttribModify::anti_hitrecover;
	ProcessFunc[magic_do_hurt_p] = &KNpcAttribModify::do_hurt_p;
	ProcessFunc[magic_addskillexp1] = &KNpcAttribModify::addskillexp1;
	ProcessFunc[magic_anti_poisontimereduce_p] = &KNpcAttribModify::anti_poisontimereduce_p;
	ProcessFunc[magic_anti_stuntimereduce_p] = &KNpcAttribModify::anti_stuntimereduce_p;
	ProcessFunc[magic_addskilldamage8] = &KNpcAttribModify::addskilldamage8;
	ProcessFunc[magic_addskilldamage9] = &KNpcAttribModify::addskilldamage9;
	ProcessFunc[magic_addskilldamage10] = &KNpcAttribModify::addskilldamage10;
	ProcessFunc[magic_anti_do_hurt_p] = &KNpcAttribModify::anti_do_hurt_p;
	ProcessFunc[magic_do_stun_p] = &KNpcAttribModify::do_stun_p;
	ProcessFunc[magic_anti_do_stun_p] = &KNpcAttribModify::anti_do_stun_p;
	ProcessFunc[magic_meleedamagereturnmana_p] = &KNpcAttribModify::meleedamagereturnmana_p;
	ProcessFunc[magic_rangedamagereturnmana_p] = &KNpcAttribModify::rangedamagereturnmana_p;
	ProcessFunc[magic_lock_life] = &KNpcAttribModify::lock_life;
	ProcessFunc[magic_special_point] = &KNpcAttribModify::special_point;
	ProcessFunc[magic_resume_life_p] = &KNpcAttribModify::resume_life_p;
	ProcessFunc[magic_reset_bufftime] = &KNpcAttribModify::reset_bufftime;
	ProcessFunc[magic_reset_bufftime_recv] = &KNpcAttribModify::reset_bufftime_recv;
	ProcessFunc[magic_cost_sp] = &KNpcAttribModify::cost_sp;
	ProcessFunc[magic_cast_when_buff_removed] = &KNpcAttribModify::cast_when_buff_removed;
	ProcessFunc[magic_autocastskill] = &KNpcAttribModify::autocastskill;
	/*

	magic_lock_life,
	magic_special_point,
	magic_resume_life_p,
	magic_reset_bufftime,
	magic_reset_bufftime_recv,
	magic_cost_sp,
	magic_cast_when_buff_removed,
	magic_autocastskill,

	magic_poisonres_yan_p,                            //=毒防：#d1+%（阳）
    magic_fireres_yan_p,                              //=火防：#d1+%（阳）
	magic_lightingres_yan_p,                    //=雷防：#d1+%（阳）
	magic_physicsres_yan_p,                     //=普防：#d1+%（阳）
	magic_coldres_yan_p,                        //=冰防：#d1+%（阳）
	magic_lifemax_yan_v,                        //=生命最大值：#d1+点（阳）
	magic_lifemax_yan_p,                        //=生命最大值：#d1+%（阳）
	magic_manamax_yan_v,                        //=内力最大值：#d1+点（阳）
	magic_manamax_yan_p,                        //=内力最大值：#d1+%（阳）
	magic_sorbdamage_yan_p,                     //=抵消伤害：#d1+（阳）
	magic_fastwalkrun_yan_p,                    //=移动速度：#d1+%（阳）
	magic_attackspeed_yan_v,                    //=外功系武功出招动作：#d1+%（阳）
	magic_castspeed_yan_v,                      //=内功系武功出招动作：#d1+%（阳）
	magic_allres_yan_p,                         //=所有抗性：#d1+%（阳）
	magic_fasthitrecover_yan_v,                 //=受伤动作时间：#d1~（阳）
	magic_anti_physicsres_yan_p,                //=忽略对方普防：#d1-%（阳）
	magic_anti_poisonres_yan_p,                 //=忽略对方毒防：#d1-%（阳）
	magic_anti_coldres_yan_p,                   //=忽略对方冰防：#d1-%（阳）
	magic_anti_fireres_yan_p,                   //=忽略对方火防：#d1-%（阳）
	magic_anti_lightingres_yan_p,               //=忽略对方雷防：#d1-%（阳）
	magic_anti_allres_yan_p,                    //=忽略对方所有抗性：#d1-%（阳）
	magic_anti_sorbdamage_yan_p,                //=加深伤害：#d1+（阴阳）

	magic_me2metaldamage_p,//=对金系伤害增加：#d1+%
    magic_metal2medamage_p,//=减少来自金系的伤害：#d1-%
    magic_me2wooddamage_p,//=对木系伤害增加：#d1+%
    magic_wood2medamage_p,//=减少来自木系的伤害：#d1-%
    magic_me2waterdamage_p,//=对水系伤害增加：#d1+%
    magic_water2medamage_p,//=减少来自水系的伤害：#d1-%
    magic_me2firedamage_p,//=对火系伤害增加：#d1+%
    magic_fire2medamage_p,//=减少来自火系的伤害：#d1-%
    magic_me2earthdamage_p,//=对土系伤害增加：#d1+%
    magic_earth2medamage_p,//=减少来自土系的伤害：#d1-%
	
	
	*/
		
}

KNpcAttribModify::~KNpcAttribModify()
{
	
}
//装备和技能属性数据应用到NPC身上
void KNpcAttribModify::ModifyAttrib(KNpc* pNpc, void* pData,int nAttacker)
{
	if (!pData || !pNpc)
		return;

	KMagicAttrib* pMagic = (KMagicAttrib *)pData;

	if (pMagic->nAttribType < 0 || pMagic->nAttribType >= magic_normal_end || NULL == ProcessFunc[pMagic->nAttribType])
		return;
	
	(this->*ProcessFunc[pMagic->nAttribType])(pNpc, pData);  //数组指针  
}

//全抗
void KNpcAttribModify::AllresP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	int nRes=0;
	int nRer=0;
	int nIsadd=0;

	/*pNpc->m_CurrentFireResist    += pMagic->nValue[0];
	pNpc->m_CurrentColdResist    += pMagic->nValue[0];
	pNpc->m_CurrentLightResist   += pMagic->nValue[0];
	pNpc->m_CurrentPoisonResist  += pMagic->nValue[0];
	pNpc->m_CurrentPhysicsResist += pMagic->nValue[0];*/
//--------火防	
	pNpc->m_TempFireResist += pMagic->nValue[0];
	
	//if (pNpc->m_TempFireResist>=0)
	//{
	//	nIsadd=1;
	//	nRes=(pNpc->m_TempFireResist*BASE_FANGYU_ALL_MAX)/100;//TakeTrader(pNpc->m_TempFireResist*BASE_FANGYU_ALL_MAX,100); //è?éì
	//	nRer=(pNpc->m_TempFireResist*BASE_FANGYU_ALL_MAX)%100;;//TakeRemainder(pNpc->m_TempFireResist*BASE_FANGYU_ALL_MAX,100);
	//	
	//	if (pNpc->m_TempFireResist==1)
	//		nRes=1;
	//	
	//	if (nRer>=50)
	//		nRes +=1; //4éá5è?
	//	
	//}
	//else
	//{
	//	nRes=(-pNpc->m_TempFireResist*BASE_FANGYU_ALL_MAX)/100;//TakeTrader(-pNpc->m_TempFireResist*BASE_FANGYU_ALL_MAX,100); //è?éì
	//	nRer=(-pNpc->m_TempFireResist*BASE_FANGYU_ALL_MAX)%100;//TakeTrader(-pNpc->m_TempFireResist*BASE_FANGYU_ALL_MAX,100);
	//	
	//	if  (-pNpc->m_TempFireResist==1)
	//		nRes=1;
	//	
	//	if (nRer>=50)
	//		nRes +=1; //4éá5è?
	//}
	//
	//
	//if (nIsadd==1)	//?yêy
 //       pNpc->m_CurrentFireResist=pNpc->m_FireResist+nRes;
	//else           //?oêy
	//	pNpc->m_CurrentFireResist=pNpc->m_FireResist-nRes;

	pNpc->m_CurrentFireResist = pNpc->m_FireResist + pNpc->m_TempFireResist;

	//--------±ù·à	
	nRes=0;
	nRer=0;
	nIsadd=0;
	pNpc->m_TempColdResist += pMagic->nValue[0];
	
	//if (pNpc->m_TempColdResist>=0)
	//{
	//	nIsadd=1;
	//	nRes=(pNpc->m_TempColdResist*BASE_FANGYU_ALL_MAX)/100;//TakeTrader(pNpc->m_TempColdResist*BASE_FANGYU_ALL_MAX,100); //è?éì
	//	nRer=(pNpc->m_TempColdResist*BASE_FANGYU_ALL_MAX)%100;//TakeRemainder(pNpc->m_TempColdResist*BASE_FANGYU_ALL_MAX,100);
	//	
	//	if (pNpc->m_TempColdResist==1)
	//		nRes=1;
	//	
	//	if (nRer>=50)
	//		nRes +=1; //4éá5è?
	//	
	//}
	//else
	//{
	//	nRes=(-pNpc->m_TempColdResist*BASE_FANGYU_ALL_MAX)/100;//TakeTrader(-pNpc->m_TempColdResist*BASE_FANGYU_ALL_MAX,100); //è?éì
	//	nRer=(-pNpc->m_TempColdResist*BASE_FANGYU_ALL_MAX)%100;//TakeTrader(-pNpc->m_TempColdResist*BASE_FANGYU_ALL_MAX,100);
	//	
	//	if  (-pNpc->m_TempColdResist==1)
	//		nRes=1;
	//	
	//	if (nRer>=50)
	//		nRes +=1; //4éá5è?
	//}
	//
	//
	//if (nIsadd==1)	//?yêy
 //       pNpc->m_CurrentColdResist=pNpc->m_ColdResist+nRes;
	//else           //?oêy
	//	pNpc->m_CurrentColdResist=pNpc->m_ColdResist-nRes;

	pNpc->m_CurrentColdResist = pNpc->m_ColdResist + pNpc->m_TempColdResist;

//--------雷防	
	nRes=0;
	nRer=0;
	nIsadd=0;
	pNpc->m_TempLightResist += pMagic->nValue[0];
	
	//if (pNpc->m_TempLightResist>=0)
	//{
	//	nIsadd=1;
	//	nRes=(pNpc->m_TempLightResist*BASE_FANGYU_ALL_MAX)/100;//TakeTrader(pNpc->m_TempLightResist*BASE_FANGYU_ALL_MAX,100); //è?éì
	//	nRer=(pNpc->m_TempLightResist*BASE_FANGYU_ALL_MAX)%100;//TakeRemainder(pNpc->m_TempLightResist*BASE_FANGYU_ALL_MAX,100);
	//	
	//	if (pNpc->m_TempLightResist==1)
	//		nRes=1;
	//	
	//	if (nRer>=50)
	//		nRes +=1; //4éá5è?
	//	
	//}
	//else
	//{
	//	nRes=(-pNpc->m_TempLightResist*BASE_FANGYU_ALL_MAX)/100;//TakeTrader(-pNpc->m_TempLightResist*BASE_FANGYU_ALL_MAX,100); //è?éì
	//	nRer=(-pNpc->m_TempLightResist*BASE_FANGYU_ALL_MAX)%100;//TakeTrader(-pNpc->m_TempLightResist*BASE_FANGYU_ALL_MAX,100);
	//	
	//	if  (-pNpc->m_TempLightResist==1)
	//		nRes=1;
	//	
	//	if (nRer>=50)
	//		nRes +=1; //4éá5è?
	//}
	//
	//
	//if (nIsadd==1)	//?yêy
 //       pNpc->m_CurrentLightResist=pNpc->m_LightResist+nRes;
	//else           //?oêy
	//	pNpc->m_CurrentLightResist=pNpc->m_LightResist-nRes;

	pNpc->m_CurrentLightResist = pNpc->m_LightResist + pNpc->m_TempLightResist;

//--------??·à	
	nRes=0;
	nRer=0;
	nIsadd=0;
	pNpc->m_TempPhysicsResist += pMagic->nValue[0];
	
	//if (pNpc->m_TempPhysicsResist>=0)
	//{
	//	nIsadd=1;
	//	nRes=(pNpc->m_TempPhysicsResist*BASE_FANGYU_ALL_MAX)/100;//TakeTrader(pNpc->m_TempPhysicsResist*BASE_FANGYU_ALL_MAX,100); //è?éì
	//	nRer=(pNpc->m_TempPhysicsResist*BASE_FANGYU_ALL_MAX)%100;//TakeRemainder(pNpc->m_TempPhysicsResist*BASE_FANGYU_ALL_MAX,100);
	//	
	//	if (pNpc->m_TempPhysicsResist==1)
	//		nRes=1;
	//	
	//	if (nRer>=50)
	//		nRes +=1; //4éá5è?
	//	
	//}
	//else
	//{
	//	nRes=(-pNpc->m_TempPhysicsResist*BASE_FANGYU_ALL_MAX)/100;//TakeTrader(-pNpc->m_TempPhysicsResist*BASE_FANGYU_ALL_MAX,100); //è?éì
	//	nRer=(-pNpc->m_TempPhysicsResist*BASE_FANGYU_ALL_MAX)%100;//TakeTrader(-pNpc->m_TempPhysicsResist*BASE_FANGYU_ALL_MAX,100);
	//	
	//	if  (-pNpc->m_TempPhysicsResist==1)
	//		nRes=1;
	//	
	//	if (nRer>=50)
	//		nRes +=1; //4éá5è?
	//}
	//
	//
	//if (nIsadd==1)	//?yêy
 //       pNpc->m_CurrentPhysicsResist=pNpc->m_PhysicsResist+nRes;
	//else           //?oêy
	//	pNpc->m_CurrentPhysicsResist=pNpc->m_PhysicsResist-nRes;

	pNpc->m_CurrentPhysicsResist = pNpc->m_PhysicsResist + pNpc->m_TempPhysicsResist;

//--------毒防	
	nRes=0;
	nRer=0;
	nIsadd=0;
	pNpc->m_TempPoisonResist += pMagic->nValue[0];
	
	//if (pNpc->m_TempPoisonResist>=0)
	//{
	//	nIsadd=1;
	//	nRes=(pNpc->m_TempPoisonResist*BASE_FANGYU_ALL_MAX)/100;//TakeTrader(pNpc->m_TempPoisonResist*BASE_FANGYU_ALL_MAX,100); //è?éì
	//	nRer=(pNpc->m_TempPoisonResist*BASE_FANGYU_ALL_MAX)%100;//TakeRemainder(pNpc->m_TempPoisonResist*BASE_FANGYU_ALL_MAX,100);
	//	
	//	if (pNpc->m_TempPoisonResist==1)
	//		nRes=1;
	//	
	//	if (nRer>=50)
	//		nRes +=1; //4éá5è?
	//	
	//}
	//else
	//{
	//	nRes=(-pNpc->m_TempPoisonResist*BASE_FANGYU_ALL_MAX)/100;//TakeTrader(-pNpc->m_TempPoisonResist*BASE_FANGYU_ALL_MAX,100); //è?éì
	//	nRer=(-pNpc->m_TempPoisonResist*BASE_FANGYU_ALL_MAX)%100;//TakeTrader(-pNpc->m_TempPoisonResist*BASE_FANGYU_ALL_MAX,100);
	//	
	//	if  (-pNpc->m_TempPoisonResist==1)
	//		nRes=1;
	//	
	//	if (nRer>=50)
	//		nRes +=1; //4éá5è?
	//}
	//
	//
	//if (nIsadd==1)	//?yêy
 //       pNpc->m_CurrentPoisonResist=pNpc->m_PoisonResist+nRes;
	//else           //?oêy
	//	pNpc->m_CurrentPoisonResist=pNpc->m_PoisonResist-nRes;

	pNpc->m_CurrentPoisonResist = pNpc->m_PoisonResist + pNpc->m_TempPoisonResist;

//---------------------------------------------------------
   
	/*if (pNpc->m_CurrentFireResist>BASE_FANGYU_ALL_MAX)
		pNpc->m_CurrentFireResistMax    = pNpc->m_CurrentFireResist;
	   
	if (pNpc->m_CurrentColdResist>BASE_FANGYU_ALL_MAX)
		pNpc->m_CurrentColdResistMax    = pNpc->m_CurrentColdResist;

	if (pNpc->m_CurrentLightResist>BASE_FANGYU_ALL_MAX)
	   pNpc->m_CurrentLightResistMax    = pNpc->m_CurrentLightResist;
	
	if (pNpc->m_CurrentPoisonResist>BASE_FANGYU_ALL_MAX)   
	   pNpc->m_CurrentPoisonResistMax   = pNpc->m_CurrentPoisonResist;

	if (pNpc->m_CurrentPhysicsResist>BASE_FANGYU_ALL_MAX) 
	   pNpc->m_CurrentPhysicsResistMax  = pNpc->m_CurrentPhysicsResist;

	if (pNpc->m_CurrentPoisonResistMax > MAX_RESIST)
        pNpc->m_CurrentPoisonResistMax = MAX_RESIST;

	if (pNpc->m_CurrentLightResistMax > MAX_RESIST)
        pNpc->m_CurrentLightResistMax = MAX_RESIST;

	if (pNpc->m_CurrentFireResistMax > MAX_RESIST)
        pNpc->m_CurrentFireResistMax = MAX_RESIST;

	if (pNpc->m_CurrentColdResistMax > MAX_RESIST)
        pNpc->m_CurrentColdResistMax = MAX_RESIST;

	if (pNpc->m_CurrentPhysicsResistMax > MAX_RESIST)
        pNpc->m_CurrentPhysicsResistMax = MAX_RESIST;


	if (pNpc->m_CurrentPoisonResistMax<=BASE_FANGYU_ALL_MAX)
		pNpc->m_CurrentPoisonResistMax=BASE_FANGYU_ALL_MAX;

	if (pNpc->m_CurrentLightResistMax<=BASE_FANGYU_ALL_MAX)
		pNpc->m_CurrentLightResistMax=BASE_FANGYU_ALL_MAX;

	if (pNpc->m_CurrentFireResistMax<=BASE_FANGYU_ALL_MAX)
		pNpc->m_CurrentFireResistMax=BASE_FANGYU_ALL_MAX;

	if (pNpc->m_CurrentColdResistMax<=BASE_FANGYU_ALL_MAX)
		pNpc->m_CurrentColdResistMax=BASE_FANGYU_ALL_MAX;

	if (pNpc->m_CurrentPhysicsResistMax<=BASE_FANGYU_ALL_MAX)
		pNpc->m_CurrentPhysicsResistMax=BASE_FANGYU_ALL_MAX;


	if (pNpc->m_CurrentPoisonResist > pNpc->m_CurrentPoisonResistMax)
		  pNpc->m_CurrentPoisonResist = pNpc->m_CurrentPoisonResistMax;
	if (pNpc->m_CurrentLightResist > pNpc->m_CurrentLightResistMax)
		  pNpc->m_CurrentLightResist  = pNpc->m_CurrentLightResistMax;
	if (pNpc->m_CurrentFireResist > pNpc->m_CurrentFireResistMax)
		  pNpc->m_CurrentFireResist   = pNpc->m_CurrentFireResistMax;
	if (pNpc->m_CurrentColdResist >pNpc->m_CurrentColdResistMax)
		  pNpc->m_CurrentColdResist   = pNpc->m_CurrentColdResistMax;
	if (pNpc->m_CurrentPhysicsResist >pNpc->m_CurrentPhysicsResistMax)
		  pNpc->m_CurrentPhysicsResist= pNpc->m_CurrentPhysicsResistMax;*/

	if (pNpc->m_CurrentPoisonResist > pNpc->m_PoisonResistMax)
		pNpc->m_CurrentPoisonResist = pNpc->m_PoisonResistMax;
	if (pNpc->m_CurrentLightResist > pNpc->m_LightResistMax)
		pNpc->m_CurrentLightResist = pNpc->m_LightResistMax;
	if (pNpc->m_CurrentFireResist > pNpc->m_FireResistMax)
		pNpc->m_CurrentFireResist = pNpc->m_FireResistMax;
	if (pNpc->m_CurrentColdResist > pNpc->m_ColdResistMax)
		pNpc->m_CurrentColdResist = pNpc->m_ColdResistMax;
	if (pNpc->m_CurrentPhysicsResist > pNpc->m_PhysicsResistMax)
		pNpc->m_CurrentPhysicsResist = pNpc->m_PhysicsResistMax;

/*	if (pNpc->m_CurrentPoisonResistMax<=0 ||\
	        pNpc->m_CurrentLightResistMax<=0 ||\
			pNpc->m_CurrentFireResistMax<=0 ||\
			pNpc->m_CurrentColdResistMax<=0 ||\
			pNpc->m_CurrentPhysicsResistMax<=0)
	{//更新自身的属性参数
		 if (pNpc->IsPlayer())
	        Player[pNpc->m_nPlayerIdx].UpdataCurData(); 
		 else
		 {
		   pNpc->m_CurrentPoisonResistMax = MAX_RESIST;
		   pNpc->m_CurrentLightResistMax  = MAX_RESIST;
		   pNpc->m_CurrentFireResistMax = MAX_RESIST;
		   pNpc->m_CurrentColdResistMax = MAX_RESIST;
		   pNpc->m_CurrentPhysicsResistMax = MAX_RESIST;
		 }

    } */
	//g_DebugLog("[数值]%s五防抗性增加%d", pNpc->Name, pMagic->nValue[0]);
}
//增加技能等级 技能 和 装备共用
void KNpcAttribModify::AllSkillV(KNpc* pNpc, void* pData)
{
	
	if(!pNpc->IsPlayer())
		return;

	KMagicAttrib* pMagic = (KMagicAttrib *)pData;

	/*if (pMagic->nValue[2] != 0 && pMagic->nValue[2]<=2000 && pMagic->nValue[2]>=-2000) //技能ID
	{
		int Same,nSkillPos=0;
		if (pMagic->nValue[2] > 0)
			Same = pMagic->nValue[2];
		else
			Same = -pMagic->nValue[2];


#ifdef _SERVER
		char Msg[64];
		sprintf(Msg,"1S:技能等级(%d)(%d)(%d)成功",Same,pMagic->nValue[0],pMagic->nValue[2]);
		Player[pNpc->GetPlayerIdx()].m_ItemList.msgshow(Msg);	
#else
		char Msg[64];
		sprintf(Msg,"1C:技能等级(%d)(%d)(%d)成功",Same,pMagic->nValue[0],pMagic->nValue[2]);
		Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(Msg);	
#endif


        nSkillPos=pNpc->m_SkillList.FindSame(Same);

		if (nSkillPos)  //查找该技能在技能列表中的位置
		{//int nRet = Npc[Player[nPlayerIndex].m_nIndex].m_SkillList.Add(nSkillId, nLevel);
			if (pNpc->m_SkillList.GetCurrentLevel(Same) > 0)
			{//等级大于零时			
				pNpc->m_SkillList.QeuipAddPoint(nSkillPos,pMagic->nValue[0]); //设置增加或减少的的技能点                             
#ifdef _SERVER	
				// 把当前此技能的点数和剩余技能点发给客户端	
				pNpc->m_SkillList.IncreaseLevel(nSkillPos,pMagic->nValue[0]); //重新设定技能数据
				//Player[pNpc->m_nPlayerIdx].UpdataCurData();
				PLAYER_SKILL_LEVEL_SYNC	sSkill;
				sSkill.ProtocolType  = s2c_playerskilllevel;
				sSkill.m_nSkillID    = Same;
				sSkill.m_nSkillLevel = pNpc->m_SkillList.GetCurrentLevel(Same);
				sSkill.m_nLeavePoint = Player[pNpc->m_nPlayerIdx].m_nSkillPoint;
				sSkill.m_nAddPoint   = pNpc->m_SkillList.GetAddPoint(nSkillPos);
				g_pServer->PackDataToClient(Player[pNpc->m_nPlayerIdx].m_nNetConnectIdx, (BYTE*)&sSkill, sizeof(PLAYER_SKILL_LEVEL_SYNC));
#endif
			}			
		}
		else
		{//
          //int nRet = Npc[Player[nPlayerIndex].m_nIndex].m_SkillList.Add(Same, 1);
		}
	}
    else */
	if (pMagic->nValue[2]!= 0)//技能ID
	{//是门派技能
        int  nSkillRows=0,nSkillid=0;
		if (pMagic->nValue[2] > 0)
			nSkillid = pMagic->nValue[2];
		else
			nSkillid = -pMagic->nValue[2];

		//KTabFile nMagci;
            // if (nMagci.Load(TABFILE_MAGICAGOLD_PATH))
			 {
                   // int nSkillid=0;
                   // nMagci.GetInteger(nSkillRows,"参数3最小值",0,&nSkillid);

/*#ifdef _SERVER
					char Msg[64];
					sprintf(Msg,"2S:技能等级(%d)(%d)(%d)成功",nSkillid,pMagic->nValue[0],pMagic->nValue[2]);
					Player[pNpc->GetPlayerIdx()].m_ItemList.msgshow(Msg);	
#endif*/
/*
#ifndef _SERVER
					char Msg[64];
					sprintf(Msg,"2C:技能等级(%d)(%d)(%d)成功",nSkillid,pMagic->nValue[0],pMagic->nValue[2]);
					Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(Msg);	
#endif */			int nSkillIdx=pNpc->m_SkillList.FindSame(nSkillid);

					if (nSkillIdx)
					{//int nRet = Npc[Player[nPlayerIndex].m_nIndex].m_SkillList.Add(nSkillId, nLevel);
						if (pNpc->m_SkillList.GetCurrentLevelByIdx(nSkillIdx) > 0)
						{
							pNpc->m_SkillList.QeuipAddPoint(nSkillIdx,pMagic->nValue[0]); 
/**#ifndef _SERVER
						    if  (pMagic->nValue[0]<0 && Npc->m_SkillList.GetSkillStyle(nSkillIdx)==SKILL_SS_PassivityNpcState)
							{//客户端马上去除状态 
							   pNpc->SetSkillLeftTime(nSkillid,0);
							}   
#endif*/

#ifdef _SERVER
							pNpc->m_SkillList.IncreaseLevel(nSkillIdx,pMagic->nValue[0],TRUE);
							// 把当前此技能的点数和剩余技能点发给客户端	
							PLAYER_SKILL_LEVEL_SYNC	sSkill;
							sSkill.ProtocolType  = s2c_playerskilllevel;
							sSkill.m_nSkillID    = nSkillid;
							sSkill.m_nSkillLevel = pNpc->m_SkillList.GetCurrentLevelByIdx(nSkillIdx);
							sSkill.m_nSkillExp = pNpc->m_SkillList.GetCurSkillExp(nSkillIdx);//muc do luyen
							sSkill.m_nLeavePoint = Player[pNpc->m_nPlayerIdx].m_nSkillPoint;
							sSkill.m_nAddPoint   = pNpc->m_SkillList.GetAddPoint(nSkillIdx);	//额外增加的技能点
							g_pServer->PackDataToClient(Player[pNpc->m_nPlayerIdx].m_nNetConnectIdx, (BYTE*)&sSkill, sizeof(PLAYER_SKILL_LEVEL_SYNC));
#endif
						}			
					} 
			 }
			 //nMagci.Clear();
  
	} 
	else if (pMagic->nValue[2] == 0) 
	{//对所有技能加 等级
		for (int i = 1;i < MAX_NPCSKILL;i++)
		{//int nRet = Npc[Player[nPlayerIndex].m_nIndex].m_SkillList.Add(nSkillId, nLevel);
/*			
#ifdef _SERVER
			char Msg[64];
			sprintf(Msg,"3S:技能等级(%d)(%d)(%d)成功",pNpc->m_SkillList.m_Skills[i].SkillId,pMagic->nValue[0],pMagic->nValue[2]);
			Player[pNpc->GetPlayerIdx()].m_ItemList.msgshow(Msg);	
#else
			char Msg[64];
			sprintf(Msg,"3C:技能等级(%d)(%d)(%d)成功",pNpc->m_SkillList.m_Skills[i].SkillId,pMagic->nValue[0],pMagic->nValue[2]);
			Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(Msg);	
#endif
*/			

			//int mReg=0;
		        //mReg=pNpc->m_SkillList.CheckNoSkill("NoCanXiSui",pNpc->m_SkillList.m_Skills[i].SkillId);

		    //if (mReg)
				//continue;

			if(pNpc->m_SkillList.m_Skills[i].SkillId && pNpc->m_SkillList.m_Skills[i].SkillId!=1 && pNpc->m_SkillList.m_Skills[i].SkillId!=2 && pNpc->m_SkillList.m_Skills[i].SkillId!=53)
			{
				if (pNpc->m_SkillList.GetCurrentLevelByIdx(i) > 0)
				{
					pNpc->m_SkillList.QeuipAddPoint(i,pMagic->nValue[0]);//该位置的技能能点
/*
#ifndef _SERVER
					char Msg[64];
					sprintf(Msg,"2C:技能等级(%d)(%d),技能点(%d),当前等级:(%d)成功",pNpc->m_SkillList.m_Skills[i].SkillId,pMagic->nValue[0],pNpc->m_SkillList.GetAddPoint(i),pNpc->m_SkillList.GetCurrentLevel(pNpc->m_SkillList.GetSkillId(i)));
					Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(Msg);	
#endif */
					
#ifdef _SERVER
					pNpc->m_SkillList.IncreaseLevel(i,pMagic->nValue[0],TRUE);
					// 把当前此技能的点数和剩余技能点发给客户端				
					PLAYER_SKILL_LEVEL_SYNC	sSkill;
					sSkill.ProtocolType = s2c_playerskilllevel;
					sSkill.m_nSkillID = pNpc->m_SkillList.GetSkillId(i);
					sSkill.m_nSkillLevel = pNpc->m_SkillList.GetCurrentLevelByIdx(i);
					sSkill.m_nSkillExp = pNpc->m_SkillList.GetCurSkillExp(i);//muc do luyen
					sSkill.m_nLeavePoint = Player[pNpc->m_nPlayerIdx].m_nSkillPoint;
					sSkill.m_nAddPoint = pNpc->m_SkillList.GetAddPoint(i);	//该位置的技能点
					g_pServer->PackDataToClient(Player[pNpc->m_nPlayerIdx].m_nNetConnectIdx, (BYTE*)&sSkill, sizeof(PLAYER_SKILL_LEVEL_SYNC));
#endif
				}
			}
		}
	}
//	//g_DebugLog("[数值]%s所有技能加%d级", pNpc->Name, pMagic->nValue[0]);
}

 
//技能忽略敌方闪避率
void KNpcAttribModify::Ignoredefense_p(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;

	pNpc->m_CurrentIgnoredefensep += pMagic->nValue[0];


	if (pNpc->m_CurrentIgnoredefensep<=0)
		pNpc->m_CurrentIgnoredefensep = 0;

	if (pNpc->m_CurrentIgnoredefensep >100)
		pNpc->m_CurrentIgnoredefensep = 100;


	//g_DebugLog("[数值]%s忽略敌方闪避率百分之%d", pNpc->Name, pMagic->nValue[0], pNpc->m_AttackRating * pMagic->nValue[0] / 100);
}

//技能命中百分比
void KNpcAttribModify::AttackRatingP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentAttackRating += pNpc->m_AttackRating * pMagic->nValue[0] / 100;
	//g_DebugLog("[数值]%s命中率增加百分之%d", pNpc->Name, pMagic->nValue[0], pNpc->m_AttackRating * pMagic->nValue[0] / 100);
}
//技能命中点
void KNpcAttribModify::AttackRatingV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentAttackRating += pMagic->nValue[0];
	//g_DebugLog("[数值]%s命中率增加%d点", pNpc->Name, pMagic->nValue[0]);
}
//装备状态命中率
void KNpcAttribModify::AddAttackRatingV(KNpc* pNpc, void* pData)
{
   	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentAttackRatingEnhancev += pMagic->nValue[0];
}
//装备状态命中率百分比
void KNpcAttribModify::AddAttackRatingP(KNpc* pNpc, void* pData)
{
   	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentAttackRatingEnhancep += pMagic->nValue[0];

    if (pNpc->m_CurrentAttackRatingEnhancep<-100)
	   pNpc->m_CurrentAttackRatingEnhancep=-100;
	//g_DebugLog("[数值]%s命中率增加百分比%d", pNpc->Name, pMagic->nValue[0]);
}
//////////////////阴阳属性--------------------------------------
void KNpcAttribModify::lightingres_yan_p( KNpc* pNpc, void* pData)
{
	
}

void KNpcAttribModify::fireres_yan_p( KNpc* pNpc, void* pData)
{

}

void KNpcAttribModify::physicsres_yan_p( KNpc* pNpc, void* pData)
{
	
}

void KNpcAttribModify::poisonres_yan_p( KNpc* pNpc, void* pData)
{
	
}


void KNpcAttribModify::lifemax_yan_p( KNpc* pNpc, void* pData)
{
	
}
void KNpcAttribModify::lifemax_yan_v( KNpc* pNpc, void* pData)
{
	
}
void KNpcAttribModify::coldres_yan_p( KNpc* pNpc, void* pData)
{
	
}

void KNpcAttribModify::manamax_yan_v( KNpc* pNpc, void* pData)
{
	
}
void KNpcAttribModify::manamax_yan_p( KNpc* pNpc, void* pData)
{
	
}

void KNpcAttribModify::sorbdamage_yan_p( KNpc* pNpc, void* pData)
{
	
}

void KNpcAttribModify::fastwalkrun_yan_p( KNpc* pNpc, void* pData)
{
	
}

void KNpcAttribModify::attackspeed_yan_v( KNpc* pNpc, void* pData)
{
	
}

void KNpcAttribModify::castspeed_yan_v( KNpc* pNpc, void* pData)
{
	
}
void KNpcAttribModify::allres_yan_p( KNpc* pNpc, void* pData)
{
	
}
void KNpcAttribModify::fasthitrecover_yan_v( KNpc* pNpc, void* pData)
{
	
}
void KNpcAttribModify::anti_physicsres_yan_p( KNpc* pNpc, void* pData)
{
	
}
void KNpcAttribModify::anti_poisonres_yan_p( KNpc* pNpc, void* pData)
{
	
}
void KNpcAttribModify::anti_coldres_yan_p( KNpc* pNpc, void* pData)
{
	
}
void KNpcAttribModify::anti_fireres_yan_p( KNpc* pNpc, void* pData)
{
	
}
void KNpcAttribModify::anti_lightingres_yan_p( KNpc* pNpc, void* pData)
{
	
}
void KNpcAttribModify::anti_allres_yan_p( KNpc* pNpc, void* pData)
{
	
}
void KNpcAttribModify::anti_sorbdamage_yan_p( KNpc* pNpc, void* pData)
{
	
}
//造成对方受伤动作时间：#d1+
void KNpcAttribModify::anti_hitrecover( KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
    pNpc->m_CurrentHitNpcRecover +=pMagic->nValue[0];

	if (pNpc->m_CurrentHitNpcRecover<0)
		pNpc->m_CurrentHitNpcRecover=0;
	
}

void    KNpcAttribModify::addskillexp1( KNpc* pNpc, void* pData)
{

}

//造成受伤动作概率
void KNpcAttribModify::do_hurt_p( KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
    pNpc->m_CurrentHitRank +=pMagic->nValue[0]; 
	printf("Dohurt: %d\n",pNpc->m_CurrentHitRank);
} 

//对各种负面状态和技能免疫
void KNpcAttribModify::ignorenegativestate_p( KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	
	if(pMagic->nValue[0] > 0)
	{ 
		pNpc->m_IgnoreNegativeState.nValue[0] = pMagic->nValue[0];
		pNpc->m_IgnoreNegativeState.nTime = pMagic->nValue[1];
	}
	else if(pMagic->nValue[0] != -1)
	{
		pNpc->m_IgnoreNegativeState.nValue[0] = 0;
		pNpc->m_IgnoreNegativeState.nTime = 0;
	}

printf("[ignorenegativestate_p] Name: %s - Value: %d - Add: %d - Time: %d - Add: %d\n", pNpc->Name,  pNpc->m_IgnoreNegativeState.nValue[0], pMagic->nValue[0], pNpc->m_IgnoreNegativeState.nTime, pMagic->nValue[1]);

	
}

//魔法盾 可抵御内力最大值的多少倍率
void KNpcAttribModify::staticmagicshield_p( KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_Staticmagicshield_p +=pMagic->nValue[0];
	//printf("KNpcAttribModify::staticmagicshield: %d | %d \n",pNpc->m_Staticmagicshield_p, pMagic->nValue[0]);
}

// --
//// auto thuc thi ky nang
// --
void KNpcAttribModify::autorescueskill( KNpc* pNpc, void* pData)
{
#ifdef _SERVER
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_AutoRescueSkill.nValue[0] = pMagic->nValue[0];
	pNpc->m_AutoRescueSkill.nValue[1] = pMagic->nValue[1];
	pNpc->m_AutoRescueSkill.nValue[2] = pMagic->nValue[2];
//	g_DebugLog("[autorescueskill] Name: %s - Value: %d - Add: %d", pNpc->Name,  pNpc->m_AutoRescueSkill.nValue[2], pMagic->nValue[2]);
#endif
}

//死亡自动释放技能--魔音噬魂
void KNpcAttribModify::autodeathskill( KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_AutoDeathSkill.nValue[0] = pMagic->nValue[0];
	pNpc->m_AutoDeathSkill.nValue[1] = pMagic->nValue[1];
	pNpc->m_AutoDeathSkill.nValue[2] = pMagic->nValue[2];
#ifdef _DEBUG
	g_DebugLog("[autodeathskill] Name: %s - Value: %d - Add: %d", pNpc->Name,  pNpc->m_AutoDeathSkill.nValue[0], pMagic->nValue[0]);
#endif
}

void KNpcAttribModify::eqaddskill_v( KNpc* pNpc, void* pData)
{
#ifdef _SERVER
	if (pNpc->m_Kind==kind_player)
	{
		KMagicAttrib* pMagic = (KMagicAttrib *)pData;
		int nSkillId=0,nRet=0;
		if (pMagic->nValue[0]>0)	//技能ID
		{//增加技能--穿装
			nSkillId =pMagic->nValue[0];

			if  (pMagic->nValue[1]<0)
				pMagic->nValue[1]=0;
		  
			nRet = pNpc->m_SkillList.Add(nSkillId, pMagic->nValue[1]);
			
			if (nRet)
			{//同步某个技能数据到客户端
				int nListidx = pNpc->m_SkillList.FindSame(nSkillId);
				PLAYER_SKILL_LEVEL_SYNC NewSkill;
				NewSkill.ProtocolType  = s2c_playerskilllevel;
				NewSkill.m_nSkillID    = nSkillId;
				NewSkill.m_nSkillLevel = pNpc->m_SkillList.GetCurrentLevelByIdx(nListidx);
				NewSkill.m_nSkillExp = pNpc->m_SkillList.GetCurSkillExp(nListidx);//muc do luyen
				NewSkill.m_nLeavePoint = Player[pNpc->m_nPlayerIdx].m_nSkillPoint;
				NewSkill.m_nAddPoint   = pNpc->m_SkillList.GetAddPoint(nListidx);
				g_pServer->PackDataToClient(Player[pNpc->m_nPlayerIdx].m_nNetConnectIdx, (BYTE*)&NewSkill, sizeof(PLAYER_SKILL_LEVEL_SYNC));
			}
		}
		else if (pMagic->nValue[0]<0)
		{//减除技能--卸装
			nSkillId=-pMagic->nValue[0];

			/*int nCurLevel=pNpc->m_SkillList.GetCurrentLevel(nSkillId);

			KSkill * pCurinSkill = (KSkill *)g_SkillManager.GetSkill(nSkillId,nCurLevel);
			//从脚本中解析最新等级技能的数据
			if (pCurinSkill) //如果有这个技能的数据
			{
				//if (pCurinSkill->GetSkillStyle() == SKILL_SS_PassivityNpcState) //是否立即使用技能
				{//如果是被动技能 清除状态
					pCurinSkill->Cast(m_nNpcIndex, -1, m_nNpcIndex,1);           //设置被动状态的在NPC上的效果
				}
				pCurinSkill=NULL;
			} */

		    int nIdx=pNpc->m_SkillList.FindSame(nSkillId);

			if (nIdx)
			{

	              KSkill * pSkill =(KSkill *)g_SkillManager.GetSkill(nSkillId, pNpc->m_SkillList.m_Skills[nIdx].SkillLevel);
	   
		          if (pSkill)
		             pSkill->Cast(Player[pNpc->m_nPlayerIdx].m_nIndex, -1,Player[pNpc->m_nPlayerIdx].m_nIndex,1);  //移除状态
				  pSkill=NULL;
			      //pNpc->m_SkillList.IncreaseLevel(nIdx,pMagic->nValue[1],TRUE);//首先卸载了

		          nRet = pNpc->m_SkillList.Remove(nSkillId);

	              if (nRet)
				  {     
		            Player[pNpc->m_nPlayerIdx].SendSyncData_Skill();
				  }   
			}
		}
	}
#endif
}

//服务器端--NPC攻速
void KNpcAttribModify::npcallattackSpeed_v(KNpc* pNpc, void* pData)
{
#ifdef _SERVER

	if (pNpc->m_Kind!=kind_player)
	{
		KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	    
		  KNpcTemplate* pNpcTemp = NpcSet.GetTemplate(pNpc->m_NpcSettingIdx, pNpc->m_Level);

		  if (pNpcTemp)
		  { 
			  pNpcTemp->m_AttackFrame += pMagic->nValue[0];
			  pNpcTemp->m_CastFrame += pMagic->nValue[0];
			  
			  if (pNpcTemp->m_AttackFrame<10)
				  pNpcTemp->m_AttackFrame=10;
			  
			  if (pNpcTemp->m_CastFrame<10)
				  pNpcTemp->m_CastFrame=10;
			  
			  
			  pNpc->SetAttackFrame(pNpcTemp->m_AttackFrame);  //NPC外功速度
		      pNpc->SetCastFrame(pNpcTemp->m_CastFrame); 
			  printf("SetAttackFrame: %d SetCastFrame: %d",pNpcTemp->m_AttackFrame, pNpcTemp->m_CastFrame);
		  }

	/*	g_pNpcTemplate[pNpc->m_NpcSettingIdx][pNpc->m_Level]->m_AttackFrame += pMagic->nValue[0];
		g_pNpcTemplate[pNpc->m_NpcSettingIdx][pNpc->m_Level]->m_CastFrame += pMagic->nValue[0];

		if 	(g_pNpcTemplate[pNpc->m_NpcSettingIdx][pNpc->m_Level]->m_AttackFrame<10)
			g_pNpcTemplate[pNpc->m_NpcSettingIdx][pNpc->m_Level]->m_AttackFrame=10;

		if 	(g_pNpcTemplate[pNpc->m_NpcSettingIdx][pNpc->m_Level]->m_CastFrame<10)
			g_pNpcTemplate[pNpc->m_NpcSettingIdx][pNpc->m_Level]->m_CastFrame=10;

		
		pNpc->SetAttackFrame(g_pNpcTemplate[pNpc->m_NpcSettingIdx][pNpc->m_Level]->m_AttackFrame);  //NPC外功速度
		pNpc->SetCastFrame(g_pNpcTemplate[pNpc->m_NpcSettingIdx][pNpc->m_Level]->m_CastFrame);     //NPC内攻速
	   
	   */
	
	}
	//g_DebugLog("[数值]%s攻击速度增加%d", pNpc->Name, pMagic->nValue[0]);
#endif	
}


//服务器端--外功
void KNpcAttribModify::AttackSpeedV(KNpc* pNpc, void* pData)
{
#ifdef _SERVER

	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	// SetTask(130,2)  --天王
	if (pNpc->IsPlayer() && pNpc->m_Series==series_metal && Player[pNpc->GetPlayerIdx()].m_cTask.GetSaveVal(130)==2) //金系的 不能加攻速
		return;

	pNpc->m_CurrentAttackSpeed += pMagic->nValue[0];
	if (pNpc->m_CurrentAttackSpeed<0)
		pNpc->m_CurrentAttackSpeed=0;


	//g_DebugLog("[数值]%s攻击速度增加%d", pNpc->Name, pMagic->nValue[0]);
#endif	
}
//内功速
void KNpcAttribModify::CastSpeedV(KNpc* pNpc, void* pData)
{
#ifdef _SERVER
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;

	pNpc->m_CurrentCastSpeed += pMagic->nValue[0];

    if (pNpc->m_CurrentCastSpeed<0)
		pNpc->m_CurrentCastSpeed=0;
	//g_DebugLog("[数值]%s施法速度增加%d", pNpc->Name, pMagic->nValue[0]);
#endif
}
//不良状态时间增加
void KNpcAttribModify::BadStatusTimeReduceV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;

// if (pNpc->m_PowerState.nTime>0)			// 怒火状态
//            pNpc->m_PowerState.nTime+=pMagic->nValue[0];
   if (pNpc->m_FreezeState.nTime>0)
            pNpc->m_FreezeState.nTime +=pMagic->nValue[0];
   if (pNpc->m_BurnState.nTime>0)
            pNpc->m_BurnState.nTime +=pMagic->nValue[0];
   if (pNpc->m_ConfuseState.nTime>0)
            pNpc->m_ConfuseState.nTime +=pMagic->nValue[0];
   if (pNpc->m_StunState.nTime>0)
            pNpc->m_StunState.nTime +=pMagic->nValue[0];

//   printf("pNpc->m_ConfuseState.nTime: %d",pNpc->m_ConfuseState.nTime);
/*  pNpc->m_PoisonState.nTime;			// 中毒状态
    pNpc->m_FreezeState.nTime;			// 冰冻状态
//	pNpc->m_CurrentFreezeTimeReducePercent+=pMagic->nValue[0];  // 冰冻时间减少百分比
//	pNpc->m_CurrentPoisonTimeReducePercent+=pMagic->nValue[0];	// 中毒时间减少百分比
//	pNpc->m_CurrentStunTimeReducePercent  +=pMagic->nValue[0];  // 眩晕时间减少百分比
    pNpc->m_BurnState.nTime;			// 燃烧状态
    pNpc->m_ConfuseState.nTime;			// 混乱状态
    pNpc->m_StunState.nTime;*/			// 眩晕状态
	return;
}
//被火烧时间减少
void KNpcAttribModify::BurnTimeReduceP(KNpc* pNpc, void* pData)
{
    KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentBurnTimeReducePercent +=pMagic->nValue[0];
	return;
}

//迷惑敌人  改变阵营
void KNpcAttribModify::ChangeCampV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	if (pNpc->m_Kind != kind_player)
	{
		if (pMagic->nValue[0] > 0 && pMagic->nValue[0] < camp_num)
			pNpc->m_CurrentCamp = pMagic->nValue[0];
		else
			pNpc->m_CurrentCamp = pNpc->m_Camp;
		//g_DebugLog("[数值]%s当前阵营变为%d", pNpc->Name, pMagic->nValue[0]);
	}
	
}
//冰状态的持续时间
void KNpcAttribModify::ColdArmorV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	if (pMagic->nValue[0] > 0)
	{
		pNpc->m_ColdArmor.nValue[0] = pMagic->nValue[0];
		pNpc->m_ColdArmor.nTime = pMagic->nValue[1];
		//g_DebugLog("[数值]%s冰护盾变为%d点持续%d帧", pNpc->Name, pMagic->nValue[0], pMagic->nValue[1]);	
	}
	else
	{
		pNpc->m_ColdArmor.nValue[0] = 0;
		pNpc->m_ColdArmor.nTime = 0;
		//g_DebugLog("[数值]%s冰护盾消失", pNpc->Name);	
	}
}

void KNpcAttribModify::ColdresP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	//int nRes=0;
	//int nRer=0;
	//int nIsadd=0;
	
	pNpc->m_TempColdResist += pMagic->nValue[0];
	
/*	if (pNpc->m_TempColdResist>=0)
	{
		nIsadd=1;
		nRes=(pNpc->m_TempColdResist*BASE_FANGYU_ALL_MAX)/100;//TakeTrader(pNpc->m_TempColdResist*BASE_FANGYU_ALL_MAX,100); //è?éì
		nRer=(pNpc->m_TempColdResist*BASE_FANGYU_ALL_MAX)%100;//TakeRemainder(pNpc->m_TempColdResist*BASE_FANGYU_ALL_MAX,100);
		
		if (pNpc->m_TempColdResist==1)
			nRes=1;
		
		if (nRer>=50)
			nRes +=1; //4éá5è?
		
	}
	else
	{
		nRes=(-pNpc->m_TempColdResist*BASE_FANGYU_ALL_MAX)/100;//TakeTrader(-pNpc->m_TempColdResist*BASE_FANGYU_ALL_MAX,100); //è?éì
		nRer=(-pNpc->m_TempColdResist*BASE_FANGYU_ALL_MAX)%100;//TakeTrader(-pNpc->m_TempColdResist*BASE_FANGYU_ALL_MAX,100);
		
		if  (-pNpc->m_TempColdResist==1)
			nRes=1;
		
		if (nRer>=50)
			nRes +=1; //4éá5è?
	}
	
	
	if (nIsadd==1)	//?yêy
        pNpc->m_CurrentColdResist=pNpc->m_ColdResist+nRes;
	else           //?oêy
		pNpc->m_CurrentColdResist=pNpc->m_ColdResist-nRes;
*/
	pNpc->m_CurrentColdResist=pNpc->m_ColdResist+pNpc->m_TempColdResist;

	//if (pNpc->m_CurrentColdResist>BASE_FANGYU_ALL_MAX)
	//    pNpc->m_CurrentColdResistMax = pNpc->m_CurrentColdResist;
	//
	//if (pNpc->m_CurrentColdResistMax > MAX_RESIST)
 //       pNpc->m_CurrentColdResistMax = MAX_RESIST;

	//if (pNpc->m_CurrentColdResistMax<=BASE_FANGYU_ALL_MAX)
	//	pNpc->m_CurrentColdResistMax=BASE_FANGYU_ALL_MAX;

	//if (pNpc->m_CurrentColdResist >pNpc->m_CurrentColdResistMax)
	//	pNpc->m_CurrentColdResist =pNpc->m_CurrentColdResistMax;

	if (pNpc->m_CurrentColdResist > pNpc->m_ColdResistMax)
		pNpc->m_CurrentColdResist = pNpc->m_ColdResistMax;

	//g_DebugLog("[数值]%s冰抗性增加了%d点", pNpc->Name, pMagic->nValue[0]);
}
//致命一击百分比
void KNpcAttribModify::DeadlyStrikeP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentDeadlyStrike += pMagic->nValue[0];
	//g_DebugLog("[数值]%s致命一击百分比增加%d", pNpc->Name, pMagic->nValue[0]);
}

void KNpcAttribModify::DexterityV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	if (pNpc->m_Kind != kind_player)
		return;
	if (pNpc->m_nPlayerIdx <= 0)
		return;
	pNpc->m_CurrentAddDexterity += pMagic->nValue[0];
	Player[pNpc->m_nPlayerIdx].ChangeCurDexterity(pMagic->nValue[0]);
	//g_DebugLog("[数值]%s当前敏捷增加了%d点", pNpc->Name, pMagic->nValue[0]);	
}

void KNpcAttribModify::EarthSkillV(KNpc* pNpc, void* pData)
{
	if(!pNpc->IsPlayer())
		return;

	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	
    if (pNpc->m_Series == 4) //对土系所有技能加 等级
	{
		for (int i = 1;i < MAX_NPCSKILL;i++)
		{
			int mReg=0;
			
		        mReg=pNpc->m_SkillList.CheckNoSkill("NoCanXiSui",pNpc->m_SkillList.m_Skills[i].SkillId);

			if (mReg)
				continue;

			if(pNpc->m_SkillList.m_Skills[i].SkillId && pNpc->m_SkillList.m_Skills[i].SkillId!=1&&pNpc->m_SkillList.m_Skills[i].SkillId!=2&&pNpc->m_SkillList.m_Skills[i].SkillId!=53)
			{
				if (pNpc->m_SkillList.GetCurrentLevelByIdx(i) > 0)
				{
					pNpc->m_SkillList.QeuipAddPoint(i,pMagic->nValue[0]);
#ifdef _SERVER
					pNpc->m_SkillList.IncreaseLevel(i,pMagic->nValue[0],TRUE);
					// 把当前此技能的点数和剩余技能点发给客户端
					PLAYER_SKILL_LEVEL_SYNC	sSkill;
					sSkill.ProtocolType = s2c_playerskilllevel;
					sSkill.m_nSkillID = pNpc->m_SkillList.GetSkillId(i);
					sSkill.m_nSkillLevel = pNpc->m_SkillList.GetCurrentLevelByIdx(i);
					sSkill.m_nSkillExp = pNpc->m_SkillList.GetCurSkillExp(i);//muc do luyen
					sSkill.m_nLeavePoint = Player[pNpc->m_nPlayerIdx].m_nSkillPoint;
					sSkill.m_nAddPoint = pNpc->m_SkillList.GetAddPoint(i);
					g_pServer->PackDataToClient(Player[pNpc->m_nPlayerIdx].m_nNetConnectIdx, (BYTE*)&sSkill, sizeof(PLAYER_SKILL_LEVEL_SYNC));
#endif
				}
			}
		}
	}
	//g_DebugLog("[数值]%s木系所有技能加%d级", pNpc->Name, pMagic->nValue[0]);
}


void KNpcAttribModify::EnergyV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	if (pNpc->m_Kind != kind_player)
		return;
	if (pNpc->m_nPlayerIdx <= 0)
		return;
	Player[pNpc->m_nPlayerIdx].ChangeCurEngergy(pMagic->nValue[0]);
	//g_DebugLog("[数值]%s当前精力增加了%d点", pNpc->Name, pMagic->nValue[0]);	
}
//受伤动作（时间）点
void KNpcAttribModify::FastHitRecoverV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentHitRecover += pMagic->nValue[0];
//	printf("Thoi gian phuc hoi: %d\n",pNpc->m_CurrentHitRecover);
	//g_DebugLog("[数值]%s受伤动作概率（时间）增加%d", pNpc->Name, pMagic->nValue[0]);
}

//眩晕几率减少
void KNpcAttribModify::stuntrank_p(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentStunRank_p += pMagic->nValue[0];

    if (pNpc->m_CurrentStunRank_p<-100)
        pNpc->m_CurrentStunRank_p=-100;
	//g_DebugLog("[数值]%s眩晕几率增加减少%d", pNpc->Name, pMagic->nValue[0]);
}

//受伤动作概率（时间） (百分比)
void KNpcAttribModify::fasthitrecover_p(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	if (pMagic->nValue[0]==0)
		return;

	if  (pMagic->nValue[0]>0)
	{
		if (pNpc->m_HitRecover*pMagic->nValue[0]>=50 && pNpc->m_HitRecover*pMagic->nValue[0]<=100)
		{
		   pNpc->m_CurrentHitRecover +=1;
		}
		else
		{
			//pNpc->m_CurrentHitRecover +=pNpc->TakeTrader(pNpc->m_HitRecover * pMagic->nValue[0],100);
			pNpc->m_CurrentHitRecover +=pNpc->m_HitRecover * pMagic->nValue[0]/100;

			//if (TakeRemainder(pNpc->m_HitRecover * pMagic->nValue[0],100)>=50)
			if ((pNpc->m_HitRecover * pMagic->nValue[0]%100)>=50)
				pNpc->m_CurrentHitRecover +=1;
		}

	}
	else
	{
	   pMagic->nValue[0] =-pMagic->nValue[0];

	   if (pNpc->m_HitRecover*pMagic->nValue[0]>=50 && pNpc->m_HitRecover*pMagic->nValue[0]<=100)
	   {
		   pNpc->m_CurrentHitRecover -=1;
	   }
	   else
	   {
		   pNpc->m_CurrentHitRecover -=(pNpc->m_HitRecover * pMagic->nValue[0])/100;//pNpc->TakeTrader(pNpc->m_HitRecover * pMagic->nValue[0],100);
		   
		   //if (TakeRemainder(pNpc->m_HitRecover * pMagic->nValue[0],100)>=50)
		   if ((pNpc->m_HitRecover * pMagic->nValue[0]%100)>=50)
			   pNpc->m_CurrentHitRecover -=1;
	   }
	}


	//pNpc->m_CurrentHitRecover += (pNpc->m_HitRecover*pMagic->nValue[0])/100;
	//g_DebugLog("[数值]%s受伤动作概率（时间）增加%d", pNpc->Name, pMagic->nValue[0]);
}


void KNpcAttribModify::FastWalkRunP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	// 文档要求修改为直接加点数
	if (pMagic->nValue[0]==0)
		return;

	pNpc->m_CurrentTempSpeed+=pMagic->nValue[0];

    if (pMagic->nValue[0]>0)
	{//+速度
	   if  (pNpc->m_WalkSpeed * pMagic->nValue[0]>=50 && pNpc->m_WalkSpeed * pMagic->nValue[0]<=100)
	   {
		   pNpc->m_CurrentWalkSpeed +=1;
	   }
	   else
	   {
		   pNpc->m_CurrentWalkSpeed +=(pNpc->m_WalkSpeed * pMagic->nValue[0])/100;//pNpc->TakeTrader(pNpc->m_WalkSpeed * pMagic->nValue[0],100);
		   
		   //if (TakeRemainder(pNpc->m_WalkSpeed * pMagic->nValue[0],100)>=50)
		   //if ((pNpc->m_WalkSpeed * pMagic->nValue[0]%100)>=50)
			 //  pNpc->m_CurrentWalkSpeed +=1;

	   }

	   if  (pNpc->m_RunSpeed * pMagic->nValue[0]>=50 && pNpc->m_RunSpeed * pMagic->nValue[0]<=100)
	   {
		   pNpc->m_CurrentRunSpeed +=1;
	   }
	   else
	   {
		   pNpc->m_CurrentRunSpeed +=(pNpc->m_RunSpeed * pMagic->nValue[0])/100;//pNpc->TakeTrader(pNpc->m_RunSpeed * pMagic->nValue[0],100);
		   
		   //if (TakeRemainder(pNpc->m_RunSpeed * pMagic->nValue[0],100)>=50)
		   //if ((pNpc->m_RunSpeed * pMagic->nValue[0]%100)>=50)
			   //pNpc->m_CurrentRunSpeed +=1;	  //进1

	   }

	}
	else
	{//减少速度 <=0
		int nTempSeed=-pMagic->nValue[0];
		//pMagic->nValue[0] =-pMagic->nValue[0];
		if  (pNpc->m_WalkSpeed * nTempSeed>=50 && pNpc->m_WalkSpeed * nTempSeed<=100)
		{
			pNpc->m_CurrentWalkSpeed -=1;
		}
		else
		{
			pNpc->m_CurrentWalkSpeed -=(pNpc->m_WalkSpeed * nTempSeed)/100;//pNpc->TakeTrader(pNpc->m_WalkSpeed * pMagic->nValue[0],100);
			
			//if (TakeRemainder(pNpc->m_WalkSpeed * pMagic->nValue[0],100)>=50)
			//if ((pNpc->m_WalkSpeed * nTempSeed%100)>=50)
				//pNpc->m_CurrentWalkSpeed -=1;
			
		}
		
		if  (pNpc->m_RunSpeed * nTempSeed>=50 && pNpc->m_RunSpeed * nTempSeed<=100)
		{
			pNpc->m_CurrentRunSpeed -=1;
		}
		else
		{
			pNpc->m_CurrentRunSpeed -=(pNpc->m_RunSpeed * nTempSeed)/100;//pNpc->TakeTrader(pNpc->m_RunSpeed * pMagic->nValue[0],100);
			
			//if (TakeRemainder(pNpc->m_RunSpeed * pMagic->nValue[0],100)>=50)
			//if ((pNpc->m_RunSpeed * nTempSeed%100)>=50)
				//pNpc->m_CurrentRunSpeed -=1;	  //退1	
		}
	}
	
	//pNpc->m_CurrentWalkSpeed += (pNpc->m_WalkSpeed * pMagic->nValue[0]) / 100;
	//pNpc->m_CurrentRunSpeed  += (pNpc->m_RunSpeed * pMagic->nValue[0]) / 100;

    if (pNpc->m_CurrentRunSpeed<10)
        pNpc->m_CurrentRunSpeed=10;

	if (pNpc->m_CurrentWalkSpeed<=5)
		pNpc->m_CurrentWalkSpeed=5;

	//g_DebugLog("[数值]%s移动速度改变%d", pNpc->Name, pMagic->nValue[0]);	
}

void KNpcAttribModify::FireArmorV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	if (pMagic->nValue[0] > 0)
	{
		pNpc->m_FireArmor.nValue[0] = pMagic->nValue[0];
		pNpc->m_FireArmor.nTime = pMagic->nValue[1];
		//g_DebugLog("[数值]%s火护盾变为%d点持续%d帧", pNpc->Name, pMagic->nValue[0], pMagic->nValue[1]);
	}
	else
	{
		pNpc->m_FireArmor.nValue[0] = 0;
		pNpc->m_FireArmor.nTime = 0;
		//g_DebugLog("[数值]%s火护盾消失", pNpc->Name);
	}
}

void KNpcAttribModify::FireSkillV(KNpc* pNpc, void* pData)
{
	if(!pNpc->IsPlayer())
		return;

	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	
    if (pNpc->m_Series == 3) //对火系所有技能加 等级
	{
		for (int i = 1;i < MAX_NPCSKILL;i++)
		{
			int mReg=0;
			
		        mReg=pNpc->m_SkillList.CheckNoSkill("NoCanXiSui",pNpc->m_SkillList.m_Skills[i].SkillId);
			if (mReg)
				continue;

			if(pNpc->m_SkillList.m_Skills[i].SkillId && pNpc->m_SkillList.m_Skills[i].SkillId!=1&&pNpc->m_SkillList.m_Skills[i].SkillId!=2&&pNpc->m_SkillList.m_Skills[i].SkillId!=53)
			{
				if (pNpc->m_SkillList.GetCurrentLevelByIdx(i) > 0)
				{
					pNpc->m_SkillList.QeuipAddPoint(i,pMagic->nValue[0]);
#ifdef _SERVER
					pNpc->m_SkillList.IncreaseLevel(i,pMagic->nValue[0],TRUE);
					// 把当前此技能的点数和剩余技能点发给客户端
					PLAYER_SKILL_LEVEL_SYNC	sSkill;
					sSkill.ProtocolType = s2c_playerskilllevel;
					sSkill.m_nSkillID = pNpc->m_SkillList.GetSkillId(i);
					sSkill.m_nSkillLevel = pNpc->m_SkillList.GetCurrentLevelByIdx(i);
					sSkill.m_nSkillExp = pNpc->m_SkillList.GetCurSkillExp(i);//muc do luyen
					sSkill.m_nLeavePoint = Player[pNpc->m_nPlayerIdx].m_nSkillPoint;
					sSkill.m_nAddPoint = pNpc->m_SkillList.GetAddPoint(i);
					g_pServer->PackDataToClient(Player[pNpc->m_nPlayerIdx].m_nNetConnectIdx, (BYTE*)&sSkill, sizeof(PLAYER_SKILL_LEVEL_SYNC));
#endif
				}
			}
		}
	}
	//g_DebugLog("[数值]%s火系所有技能加%d级", pNpc->Name, pMagic->nValue[0]);
}

//迟缓时间减少百分比
void KNpcAttribModify::FreezeTimeReduceP(KNpc* pNpc, void* pData)
{
//	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
//	pNpc->m_FreezeState.nTime *= (100 - pMagic->nValue[0]);
//	pNpc->m_FreezeState.nTime /= 100;
//	if (pNpc->m_FreezeState.nTime <= 0)
//	{
//		pNpc->m_FreezeState.nTime = 0;
//	}
//	//g_DebugLog("[数值]%s冰冻时间减少百分之%d", pNpc->Name, pMagic->nValue[0]);
//	装备TODO	
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentFreezeTimeReducePercent += pMagic->nValue[0];
	//g_DebugLog("[数值]%s冰冻时间百分之%d", pNpc->Name, pMagic->nValue[0]);
}
//没有增加到伤害里面 或者是别的用处
void KNpcAttribModify::KnockBackP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentKnockBack += pMagic->nValue[0];
	//g_DebugLog("[数值]%s震退百分比增加%d", pNpc->Name, pMagic->nValue[0]);	
}

void KNpcAttribModify::LifeMaxP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;

   /* if (pMagic->nValue[0]<=-100)
	    pMagic->nValue[0]=-GetRandomNumber(80,90);*/

	pNpc->m_CurrentLifeMax +=pNpc->m_LifeMax*pMagic->nValue[0]/100;	//原始生命

	if (pNpc->m_CurrentLifeMax<=0)
	      pNpc->m_CurrentLifeMax=100;

	//g_DebugLog("[数值]%s生命上限增加了百分之%d(%d点)", pNpc->Name, pMagic->nValue[0], pNpc->m_LifeMax * pMagic->nValue[0] / 100);	
}

void KNpcAttribModify::LifeMaxV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentLifeMax += pMagic->nValue[0];

	if (pNpc->m_CurrentLifeMax<=0)
		pNpc->m_CurrentLifeMax=100;

	//g_DebugLog("[数值]%s生命上限增加了%d点", pNpc->Name, pMagic->nValue[0]);
}

void KNpcAttribModify::LifeReplenishV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;

	pNpc->m_CurrentLifeReplenish += pMagic->nValue[0];

	//这个可以小于0 
	//if (pNpc->m_CurrentLifeReplenish<0)
	//	pNpc->m_CurrentLifeReplenish=0;
	//g_DebugLog("[数值]%s生命回复速度增加了%d点", pNpc->Name, pMagic->nValue[0]);
}

void KNpcAttribModify::LifeV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentLife += pMagic->nValue[0];
	//if (pNpc->m_CurrentLife<0)
	//	pNpc->m_CurrentLife=100;
	//g_DebugLog("[数值]%s生命增加了%d点", pNpc->Name, pMagic->nValue[0]);
}

void KNpcAttribModify::LightingArmorV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	if (pMagic->nValue[0] > 0)
	{
		pNpc->m_LightArmor.nValue[0] = pMagic->nValue[0];
		pNpc->m_LightArmor.nTime = pMagic->nValue[1];
		//g_DebugLog("[数值]%s电护盾变为%d点持续%d帧", pNpc->Name, pMagic->nValue[0], pMagic->nValue[1]);	
	}
	else
	{
		pNpc->m_LightArmor.nValue[0] = 0;
		pNpc->m_LightArmor.nTime = 0;
		//g_DebugLog("[数值]%s电护盾消失", pNpc->Name);	
	}
}

void KNpcAttribModify::LightingresP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	//int nRes=0;
	//int nRer=0;
	//int nIsadd=0;
	
	pNpc->m_TempLightResist += pMagic->nValue[0];
	/*
	if (pNpc->m_TempLightResist>=0)
	{
		nIsadd=1;
		nRes=(pNpc->m_TempLightResist*BASE_FANGYU_ALL_MAX)/100;//TakeTrader(pNpc->m_TempLightResist*BASE_FANGYU_ALL_MAX,100); //è?éì
		nRer=(pNpc->m_TempLightResist*BASE_FANGYU_ALL_MAX)%100;//TakeRemainder(pNpc->m_TempLightResist*BASE_FANGYU_ALL_MAX,100);
		
		if (pNpc->m_TempLightResist==1)
			nRes=1;
		
		if (nRer>=50)
			nRes +=1; //4éá5è?
		
	}
	else
	{
		nRes=(-pNpc->m_TempLightResist*BASE_FANGYU_ALL_MAX)/100;//TakeTrader(-pNpc->m_TempLightResist*BASE_FANGYU_ALL_MAX,100); //è?éì
		nRer=(-pNpc->m_TempLightResist*BASE_FANGYU_ALL_MAX)%100;//TakeTrader(-pNpc->m_TempLightResist*BASE_FANGYU_ALL_MAX,100);
		
		if  (-pNpc->m_TempLightResist==1)
			nRes=1;
		
		if (nRer>=50)
			nRes +=1; //4éá5è?
	}
	
	
	if (nIsadd==1)	//?yêy
        pNpc->m_CurrentLightResist=pNpc->m_LightResist+nRes;
	else           //?oêy
		pNpc->m_CurrentLightResist=pNpc->m_LightResist-nRes;
*/
	pNpc->m_CurrentLightResist = pNpc->m_LightResist + pNpc->m_TempLightResist;
	//if (pNpc->m_CurrentLightResist>BASE_FANGYU_ALL_MAX)
	//    pNpc->m_CurrentLightResistMax = pNpc->m_CurrentLightResist;
	//
	//if (pNpc->m_CurrentLightResistMax > MAX_RESIST)
 //       pNpc->m_CurrentLightResistMax = MAX_RESIST;

	//if (pNpc->m_CurrentLightResistMax<=BASE_FANGYU_ALL_MAX)
	//	pNpc->m_CurrentLightResistMax=BASE_FANGYU_ALL_MAX;


	//if (pNpc->m_CurrentLightResist >pNpc->m_CurrentLightResistMax)
	//	pNpc->m_CurrentLightResist =pNpc->m_CurrentLightResistMax;

	if (pNpc->m_CurrentLightResist > pNpc->m_LightResistMax)
		pNpc->m_CurrentLightResist = pNpc->m_LightResistMax;

	//g_DebugLog("[数值]%s电抗性增加了%d点", pNpc->Name, pMagic->nValue[0]);	
}
//幸运值
void KNpcAttribModify::LuckyV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;

	if (pNpc->IsPlayer())
	{
		Player[pNpc->m_nPlayerIdx].m_nTempLucky_p += pMagic->nValue[0];

		if (Player[pNpc->m_nPlayerIdx].m_nTempLucky_p<0)
			Player[pNpc->m_nPlayerIdx].m_nTempLucky_p=0;

		//Player[pNpc->m_nPlayerIdx].m_nCurLucky = Player[pNpc->m_nPlayerIdx].m_nLucky * (Player[pNpc->m_nPlayerIdx].m_nTempLucky_p+100)/100;    //人物 类
		Player[pNpc->m_nPlayerIdx].m_nCurLucky = Player[pNpc->m_nPlayerIdx].m_nTempLucky_p;    //人物 类
		//g_DebugLog("[数值]%sMF值增加了%d", pNpc->Name, pMagic->nValue[0]);
	}
	else
	{
		pNpc->m_nCurNpcLucky +=	pMagic->nValue[0]; //怪物的幸运值
	}
}

void KNpcAttribModify::ManaMaxP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;  //NPC类

	pNpc->m_CurrentManaMax += pNpc->m_ManaMax * pMagic->nValue[0]/100;

	if (pNpc->m_CurrentManaMax<=0)
		pNpc->m_CurrentManaMax=100;
	//g_DebugLog("[数值]%s内力上限增加了百分之%d(%d点)", pNpc->Name, pMagic->nValue[0], pNpc->m_ManaMax * pMagic->nValue[0] / 100);	
}

void KNpcAttribModify::ManaMaxV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentManaMax += pMagic->nValue[0];

	if (pNpc->m_CurrentManaMax<=0)
		pNpc->m_CurrentManaMax=100;
	//g_DebugLog("[数值]%s内力上限增加了%d点", pNpc->Name, pMagic->nValue[0]);
}

void KNpcAttribModify::ManaReplenishV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentManaReplenish += pMagic->nValue[0];

	//if (pNpc->m_CurrentManaReplenish<=0)
	//	pNpc->m_CurrentManaReplenish=0;

	//g_DebugLog("[数值]%s内力回复速度增加了%d点", pNpc->Name, pMagic->nValue[0]);
}

void KNpcAttribModify::ManaReplenishp(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentManaReplenish_p += pMagic->nValue[0];

	if (pNpc->m_CurrentManaReplenish_p<-100)
		pNpc->m_CurrentManaReplenish_p=-100;
	//if (pNpc->m_CurrentManaReplenish<=0)
	//	pNpc->m_CurrentManaReplenish=0;

	//g_DebugLog("[数值]%s内力回复速度增加了%d点", pNpc->Name, pMagic->nValue[0]);
}

void KNpcAttribModify::ManaV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentMana += pMagic->nValue[0];
	//if (pNpc->m_CurrentMana<0)
	//	pNpc->m_CurrentMana=100;
	//g_DebugLog("[数值]%s内力增加了%d点", pNpc->Name, pMagic->nValue[0]);
}
//内力吸收伤害
void KNpcAttribModify::ManaShieldP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_ManaShield += pMagic->nValue[0];

#ifdef _DEBUG
//	g_DebugLog("[manashield_p] Name: %s - Value: %d - Add: %d", pNpc->Name, pNpc->m_ManaShield, pMagic->nValue[0]);
#endif
}

void KNpcAttribModify::MeleeDamageReturnP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentMeleeDmgRetPercent += pMagic->nValue[0];
	//g_DebugLog("[数值]%s近程物理伤害反弹百分比增加减少%d", pNpc->Name, pMagic->nValue[0]);
}

void KNpcAttribModify::MeleeDamageReturnV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentMeleeDmgRet += pMagic->nValue[0];
	//g_DebugLog("[数值]%s近程物理伤害反弹增加%d", pNpc->Name, pMagic->nValue[0]);
}

void KNpcAttribModify::MetalSkillV(KNpc* pNpc, void* pData)
{

	   if(!pNpc->IsPlayer())
		   return;


		KMagicAttrib* pMagic = (KMagicAttrib *)pData;
		
		if (pNpc->m_Series == 0) //对金系所有技能加 等级
		{
			for (int i = 1;i < MAX_NPCSKILL;i++)
			{
				int mReg=0;
				
		        mReg=pNpc->m_SkillList.CheckNoSkill("NoCanXiSui",pNpc->m_SkillList.m_Skills[i].SkillId);

				if (mReg)
					continue;

				if(pNpc->m_SkillList.m_Skills[i].SkillId && pNpc->m_SkillList.m_Skills[i].SkillId!=1&&pNpc->m_SkillList.m_Skills[i].SkillId!=2&&pNpc->m_SkillList.m_Skills[i].SkillId!=53)
				{
					if (pNpc->m_SkillList.GetCurrentLevelByIdx(i) > 0)
					{
						pNpc->m_SkillList.QeuipAddPoint(i,pMagic->nValue[0]);
#ifdef _SERVER
						pNpc->m_SkillList.IncreaseLevel(i,pMagic->nValue[0],TRUE);
						// 把当前此技能的点数和剩余技能点发给客户端
						PLAYER_SKILL_LEVEL_SYNC	sSkill;
						sSkill.ProtocolType = s2c_playerskilllevel;
						sSkill.m_nSkillID = pNpc->m_SkillList.GetSkillId(i);
						sSkill.m_nSkillLevel = pNpc->m_SkillList.GetCurrentLevelByIdx(i);
						sSkill.m_nSkillExp = pNpc->m_SkillList.GetCurSkillExp(i);//muc do luyen
						sSkill.m_nLeavePoint = Player[pNpc->m_nPlayerIdx].m_nSkillPoint;
						sSkill.m_nAddPoint = pNpc->m_SkillList.GetAddPoint(i);
						g_pServer->PackDataToClient(Player[pNpc->m_nPlayerIdx].m_nNetConnectIdx, (BYTE*)&sSkill, sizeof(PLAYER_SKILL_LEVEL_SYNC));
#endif
					}
				}
			}
		}
		//g_DebugLog("[数值]%s金系所有技能加%d级", pNpc->Name, pMagic->nValue[0]);
	}

void KNpcAttribModify::PhysicsArmorV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	if (pMagic->nValue[0] > 0)
	{
		pNpc->m_PhysicsArmor.nValue[0] = pMagic->nValue[0];
		pNpc->m_PhysicsArmor.nTime = pMagic->nValue[1];
		//g_DebugLog("[数值]%s物理护盾变为%d点持续%d帧", pNpc->Name, pMagic->nValue[0], pMagic->nValue[1]);	
	}
	else
	{
		pNpc->m_PhysicsArmor.nValue[0] = 0;
		pNpc->m_PhysicsArmor.nTime = 0;
		//g_DebugLog("[数值]%s物理护盾消失", pNpc->Name);	
	}
}

void KNpcAttribModify::PhysicsresP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;

	//int nRes=0;
//	int nRer=0;
//	int nIsadd=0;

	pNpc->m_TempPhysicsResist += pMagic->nValue[0];

	/*if (pNpc->m_TempPhysicsResist>=0)
	{
	  nIsadd=1;
	  nRes=(pNpc->m_TempPhysicsResist*BASE_FANGYU_ALL_MAX)/100;//TakeTrader(pNpc->m_TempPhysicsResist*BASE_FANGYU_ALL_MAX,100); //è?éì
	  nRer=(pNpc->m_TempPhysicsResist*BASE_FANGYU_ALL_MAX)%100;//TakeRemainder(pNpc->m_TempPhysicsResist*BASE_FANGYU_ALL_MAX,100);

	  if (pNpc->m_TempPhysicsResist==1)
		  nRes=1;

	  if (nRer>=50)
		  nRes +=1; //4舍5入
 
	}
	else
	{
	  nRes=(-pNpc->m_TempPhysicsResist*BASE_FANGYU_ALL_MAX)/100;//TakeTrader(-pNpc->m_TempPhysicsResist*BASE_FANGYU_ALL_MAX,100); //取商
	  nRer=(-pNpc->m_TempPhysicsResist*BASE_FANGYU_ALL_MAX)%100;//TakeTrader(-pNpc->m_TempPhysicsResist*BASE_FANGYU_ALL_MAX,100);

	  if  (-pNpc->m_TempPhysicsResist==1)
		  nRes=1;

	  if (nRer>=50)
		  nRes +=1; //4舍5入
	}

	 if (nIsadd==1)	//正数
        pNpc->m_CurrentPhysicsResist= pNpc->m_PhysicsResist+nRes;
	 else           //负数
		pNpc->m_CurrentPhysicsResist= pNpc->m_PhysicsResist-nRes;
*/
	pNpc->m_CurrentPhysicsResist = pNpc->m_PhysicsResist + pNpc->m_TempPhysicsResist;
	
	//if (pNpc->m_CurrentPhysicsResist>BASE_FANGYU_ALL_MAX)
	//    pNpc->m_CurrentPhysicsResistMax = pNpc->m_CurrentPhysicsResist;
	//
	//if (pNpc->m_CurrentPhysicsResistMax > MAX_RESIST)
 //       pNpc->m_CurrentPhysicsResistMax = MAX_RESIST;

	//if (pNpc->m_CurrentPhysicsResistMax<=BASE_FANGYU_ALL_MAX)
	//	pNpc->m_CurrentPhysicsResistMax=BASE_FANGYU_ALL_MAX;

	//if (pNpc->m_CurrentPhysicsResist >pNpc->m_CurrentPhysicsResistMax)
	//	pNpc->m_CurrentPhysicsResist =pNpc->m_CurrentPhysicsResistMax;
	
	if (pNpc->m_CurrentPhysicsResist > pNpc->m_PhysicsResistMax)
		pNpc->m_CurrentPhysicsResist = pNpc->m_PhysicsResistMax;

	//g_DebugLog("[数值]%s物理抗性增加了%d点", pNpc->Name, pMagic->nValue[0]);	
}

void KNpcAttribModify::Damage2ManaP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentDamage2Mana += pMagic->nValue[0];
	//g_DebugLog("[数值]%s伤害转内力百分比增加了%d", pNpc->Name, pMagic->nValue[0]);	
}

void KNpcAttribModify::PoisonArmorV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	if (pMagic->nValue[0] > 0)
	{
		pNpc->m_PoisonArmor.nValue[0] = pMagic->nValue[0];
		pNpc->m_PoisonArmor.nTime = pMagic->nValue[1];
		//g_DebugLog("[数值]%s毒护盾变为%d点持续%d帧", pNpc->Name, pMagic->nValue[0], pMagic->nValue[1]);
	}
	else
	{
		pNpc->m_PoisonArmor.nValue[0] = 0;
		pNpc->m_PoisonArmor.nTime = 0;
		//g_DebugLog("[数值]%s毒护盾消失", pNpc->Name);
	}
}
//解毒药
void KNpcAttribModify::PoisonDamageReduceV(KNpc* pNpc, void* pData)
{
	/*pNpc->m_PoisonState.nValue[0] -= pMagic->nValue[0];

	if (pNpc->m_PoisonState.nValue[0] <= 0)
	{
		pNpc->m_PoisonState.nTime = 0;
	}
   */
/*	pNpc->m_PoisonState.nValue[0] = pNpc->m_PoisonState.nValue[0] - pMagic->nValue[0];

	if (pNpc->m_PoisonState.nValue[0]<=0)
		pNpc->m_PoisonState.nValue[0] = 0;
*/
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_PoisonState.nValue[0] -= pMagic->nValue[0];
	if (pNpc->m_PoisonState.nValue[0] <= 0)
	{
		pNpc->m_PoisonState.nTime = 0;
	}
}

void KNpcAttribModify::PoisonresP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	//int nRes=0;
	//int nRer=0;
//	int nIsadd=0;
	
	pNpc->m_TempPoisonResist += pMagic->nValue[0];
/*
	if (pNpc->m_TempPoisonResist>=0)
	{
		nIsadd=1;
		nRes=(pNpc->m_TempPoisonResist*BASE_FANGYU_ALL_MAX)/100;//TakeTrader(pNpc->m_TempPoisonResist*BASE_FANGYU_ALL_MAX,100); //è?éì
		nRer=(pNpc->m_TempPoisonResist*BASE_FANGYU_ALL_MAX)%100;//TakeRemainder(pNpc->m_TempPoisonResist*BASE_FANGYU_ALL_MAX,100);
		
		if (pNpc->m_TempPoisonResist==1)
			nRes=1;
		
		if (nRer>=50)
			nRes +=1; //4éá5è?
		
	}
	else
	{
		nRes=(-pNpc->m_TempPoisonResist*BASE_FANGYU_ALL_MAX)/100;//TakeTrader(-pNpc->m_TempPoisonResist*BASE_FANGYU_ALL_MAX,100); //è?éì
		nRer=(-pNpc->m_TempPoisonResist*BASE_FANGYU_ALL_MAX)%100;//TakeTrader(-pNpc->m_TempPoisonResist*BASE_FANGYU_ALL_MAX,100);
		
		if  (-pNpc->m_TempPoisonResist==1)
			nRes=1;
		
		if (nRer>=50)
			nRes +=1; //4éá5è?
	}
	
	if (nIsadd==1)	//?yêy
        pNpc->m_CurrentPoisonResist=pNpc->m_PoisonResist+nRes;
	else           //?oêy
		pNpc->m_CurrentPoisonResist=pNpc->m_PoisonResist-nRes;
*/
	pNpc->m_CurrentPoisonResist = pNpc->m_PoisonResist + pNpc->m_TempPoisonResist;
 //   if (pNpc->m_CurrentPoisonResist>BASE_FANGYU_ALL_MAX)
	//    pNpc->m_CurrentPoisonResistMax = pNpc->m_CurrentPoisonResist;
	//
	//if (pNpc->m_CurrentPoisonResistMax > MAX_RESIST)
 //       pNpc->m_CurrentPoisonResistMax = MAX_RESIST;

	//if (pNpc->m_CurrentPoisonResistMax<=BASE_FANGYU_ALL_MAX)
	//	pNpc->m_CurrentPoisonResistMax=BASE_FANGYU_ALL_MAX;


	//if (pNpc->m_CurrentPoisonResist >pNpc->m_CurrentPoisonResistMax)
	//	pNpc->m_CurrentPoisonResist =pNpc->m_CurrentPoisonResistMax;

	if (pNpc->m_CurrentPoisonResist > pNpc->m_PoisonResistMax)
		pNpc->m_CurrentPoisonResist = pNpc->m_PoisonResistMax;

	//g_DebugLog("[数值]%s毒抗性增加了%d点", pNpc->Name, pMagic->nValue[0]);
}
//thoi gian trung doc
void KNpcAttribModify::PoisonTimeReduceP(KNpc* pNpc, void* pData)
{
// 装备里，需要修改TODO
//	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
//	pNpc->m_PoisonState.nTime *= (100 - pMagic->nValue[0]);
//	pNpc->m_PoisonState.nTime /= 100;
//	if (pNpc->m_PoisonState.nTime <= 0)
//	{
//		pNpc->m_PoisonState.nTime = 0;
//	}
//	//g_DebugLog("[数值]%s中毒时间减少百分之%d", pNpc->Name, pMagic->nValue[0]);	
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentPoisonTimeReducePercent += pMagic->nValue[0];
//	printf("thoi gian trung doc: %d\n", pNpc->m_CurrentPoisonTimeReducePercent);

}

//造对方中毒时间增加 或 减少
void    KNpcAttribModify::anti_poisontimereduce_p( KNpc* pNpc, void* pData)
{
	   KMagicAttrib* pMagic = (KMagicAttrib *)pData;

	   pNpc->m_EnemyPoisonTimeReducePercent += pMagic->nValue[0];

}
void KNpcAttribModify::RangeDamageReturnV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentRangeDmgRet += pMagic->nValue[0];
	//g_DebugLog("[数值]%s远程伤害反弹增加%d", pNpc->Name, pMagic->nValue[0]);
}

void KNpcAttribModify::RangeDamageReturnP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentRangeDmgRetPercent += pMagic->nValue[0];

	if (pNpc->m_CurrentRangeDmgRetPercent<=0)
	    pNpc->m_CurrentRangeDmgRetPercent=0;
	//g_DebugLog("[数值]%s远程伤害反弹百分比增加%d", pNpc->Name, pMagic->nValue[0]);
}
//减缓子弹飞行速度
void KNpcAttribModify::SlowMissleB(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;

	if (pMagic->nValue[0] > 0)
		pNpc->m_CurrentSlowMissle = 1;
	else
		pNpc->m_CurrentSlowMissle = 0;
	//g_DebugLog("[数值]%s子弹减速状态变为%d", pNpc->Name, pMagic->nValue[0]);
}

void KNpcAttribModify::StaminaMaxP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentStaminaMax += pNpc->m_StaminaMax * pMagic->nValue[0] / 100;
	//g_DebugLog("[数值]%s体力上限增加了百分之%d(%d点)", pNpc->Name, pMagic->nValue[0], pNpc->m_StaminaMax * pMagic->nValue[0] / 100);	
}

void KNpcAttribModify::StaminaMaxV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentStaminaMax += pMagic->nValue[0];
	//g_DebugLog("[数值]%s体力上限增加了%d点", pNpc->Name, pMagic->nValue[0]);
}

void KNpcAttribModify::StaminaReplenishV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentStaminaGain += pMagic->nValue[0];
	//g_DebugLog("[数值]%s体力回复速度增加了%d点", pNpc->Name, pMagic->nValue[0]);
}

void KNpcAttribModify::StaminaV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentStamina += pMagic->nValue[0];
	//g_DebugLog("[数值]%s体力增加了%d点", pNpc->Name, pMagic->nValue[0]);
}


void KNpcAttribModify::StealLifeP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentLifeStolen += pMagic->nValue[0];
	//printf("[StealLifeP]%s - %d (%d)", pNpc->Name, pNpc->m_CurrentLifeStolen, pMagic->nValue[0]);	
}

void KNpcAttribModify::StealManaP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentManaStolen += pMagic->nValue[0];
	//printf("StealManaP]%s - %d (%d)", pNpc->Name, pNpc->m_CurrentManaStolen, pMagic->nValue[0]);	
}

void KNpcAttribModify::StealStaminaP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentStaminaStolen += pMagic->nValue[0];
	//g_DebugLog("[数值]%s偷体力百分比为%d(比原本上升了%d点)", pNpc->Name, pNpc->m_CurrentStaminaStolen, pMagic->nValue[0]);	
}

void KNpcAttribModify::StrengthV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	if (pNpc->m_Kind != kind_player)
		return;
	if (pNpc->m_nPlayerIdx <= 0)
		return;
	Player[pNpc->m_nPlayerIdx].ChangeCurStrength(pMagic->nValue[0]);
	//g_DebugLog("[数值]%s当前力量增加了%d点", pNpc->Name, pMagic->nValue[0]);
}

void KNpcAttribModify::StunTimeReduceP(KNpc* pNpc, void* pData)
{
/*	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentStunTimeReducePercent += pMagic->nValue[0];
	if (pNpc->m_CurrentStunTimeReducePercent <- 200)
	    pNpc->m_CurrentStunTimeReducePercent =- 200;
*/
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentStunTimeReducePercent += pMagic->nValue[0];
	//printf("[StunTimeReduceP] Name: %s pMagic->nValue: %d\n", pNpc->Name, pMagic->nValue[0]);
}
//造成敌人的眩晕时间增加 或 减少
void KNpcAttribModify::anti_stuntimereduce_p( KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_EnemyStunTimeReducePercent += pMagic->nValue[0];
}

void KNpcAttribModify::VisionRadiusP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentVisionRadius += pMagic->nValue[0];
	//g_DebugLog("[数值]%s视野范围增加%d", pNpc->Name, pMagic->nValue[0]);	
}

void KNpcAttribModify::VitalityV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	if (pNpc->m_Kind != kind_player)
		return;
	if (pNpc->m_nPlayerIdx <= 0)
		return;
	Player[pNpc->m_nPlayerIdx].ChangeCurVitality(pMagic->nValue[0]);  //改变当前生命的 最大值 和体力值
	//g_DebugLog("[数值]%s当前外功增加了%d点", pNpc->Name, pMagic->nValue[0]);
}

void KNpcAttribModify::WaterSkillV(KNpc* pNpc, void* pData)
{

	if(!pNpc->IsPlayer())
		return;

	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	
    if (pNpc->m_Series == 2) //对木系所有技能加 等级
	{
		for (int i = 1;i < MAX_NPCSKILL;i++)
		{
			int mReg=0;
			
		        mReg=pNpc->m_SkillList.CheckNoSkill("NoCanXiSui",pNpc->m_SkillList.m_Skills[i].SkillId);

			if (mReg)
			   continue;

			if(pNpc->m_SkillList.m_Skills[i].SkillId && pNpc->m_SkillList.m_Skills[i].SkillId!=1&&pNpc->m_SkillList.m_Skills[i].SkillId!=2&&pNpc->m_SkillList.m_Skills[i].SkillId!=53)
			{
				if (pNpc->m_SkillList.GetCurrentLevelByIdx(i) > 0)
				{
					pNpc->m_SkillList.QeuipAddPoint(i,pMagic->nValue[0]);
#ifdef _SERVER
					pNpc->m_SkillList.IncreaseLevel(i,pMagic->nValue[0],TRUE);
					// 把当前此技能的点数和剩余技能点发给客户端
					PLAYER_SKILL_LEVEL_SYNC	sSkill;
					sSkill.ProtocolType = s2c_playerskilllevel;
					sSkill.m_nSkillID = pNpc->m_SkillList.GetSkillId(i);
					sSkill.m_nSkillLevel = pNpc->m_SkillList.GetCurrentLevelByIdx(i);
					sSkill.m_nSkillExp = pNpc->m_SkillList.GetCurSkillExp(i);//muc do luyen
					sSkill.m_nLeavePoint = Player[pNpc->m_nPlayerIdx].m_nSkillPoint;
					sSkill.m_nAddPoint = pNpc->m_SkillList.GetAddPoint(i);
					g_pServer->PackDataToClient(Player[pNpc->m_nPlayerIdx].m_nNetConnectIdx, (BYTE*)&sSkill, sizeof(PLAYER_SKILL_LEVEL_SYNC));
#endif
				}
			}
		}
	}
	//g_DebugLog("[数值]%s水系所有技能加%d级", pNpc->Name, pMagic->nValue[0]);
}


void KNpcAttribModify::WoodSkillV(KNpc* pNpc, void* pData)
{

	if(!pNpc->IsPlayer())
		return;

	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	
    if (pNpc->m_Series == 1) //对木系所有技能加 等级
	{
		for (int i = 1;i < MAX_NPCSKILL;i++)
		{

            int mReg=0;
			
		        mReg=pNpc->m_SkillList.CheckNoSkill("NoCanXiSui",pNpc->m_SkillList.m_Skills[i].SkillId);
			if (mReg)
				continue;

			if(pNpc->m_SkillList.m_Skills[i].SkillId && pNpc->m_SkillList.m_Skills[i].SkillId!=1&&pNpc->m_SkillList.m_Skills[i].SkillId!=2&&pNpc->m_SkillList.m_Skills[i].SkillId!=53)
			{
				if (pNpc->m_SkillList.GetCurrentLevelByIdx(i) > 0)
				{
					pNpc->m_SkillList.QeuipAddPoint(i,pMagic->nValue[0]);
#ifdef _SERVER
					pNpc->m_SkillList.IncreaseLevel(i,pMagic->nValue[0],TRUE);
						// 把当前此技能的点数和剩余技能点发给客户端
					PLAYER_SKILL_LEVEL_SYNC	sSkill;
					sSkill.ProtocolType = s2c_playerskilllevel;
					sSkill.m_nSkillID = pNpc->m_SkillList.GetSkillId(i);
					sSkill.m_nSkillLevel = pNpc->m_SkillList.GetCurrentLevelByIdx(i);
					sSkill.m_nSkillExp = pNpc->m_SkillList.GetCurSkillExp(i);//muc do luyen
					sSkill.m_nLeavePoint = Player[pNpc->m_nPlayerIdx].m_nSkillPoint;
					sSkill.m_nAddPoint = pNpc->m_SkillList.GetAddPoint(i);
					g_pServer->PackDataToClient(Player[pNpc->m_nPlayerIdx].m_nNetConnectIdx, (BYTE*)&sSkill, sizeof(PLAYER_SKILL_LEVEL_SYNC));
					
#endif
				}
			}
		}
	}
	//g_DebugLog("[数值]%s木系所有技能加%d级", pNpc->Name, pMagic->nValue[0]);
}

void KNpcAttribModify::FireresP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
//	int nRes=0;
//	int nRer=0;
//	int nIsadd=0;
	
	pNpc->m_TempFireResist += pMagic->nValue[0];
	
/*	if (pNpc->m_TempFireResist>=0)
	{
		nIsadd=1;
		nRes=(pNpc->m_TempFireResist*BASE_FANGYU_ALL_MAX)/100;//TakeTrader(pNpc->m_TempFireResist*BASE_FANGYU_ALL_MAX,100); //è?éì
		nRer=(pNpc->m_TempFireResist*BASE_FANGYU_ALL_MAX)%100;//TakeRemainder(pNpc->m_TempFireResist*BASE_FANGYU_ALL_MAX,100);
		
		if (pNpc->m_TempFireResist==1)
			nRes=1;
		
		if (nRer>=50)
			nRes +=1; //4éá5è?
		
	}
	else
	{
		nRes=(-pNpc->m_TempFireResist*BASE_FANGYU_ALL_MAX)/100;//TakeTrader(-pNpc->m_TempFireResist*BASE_FANGYU_ALL_MAX,100); //è?éì
		nRer=(-pNpc->m_TempFireResist*BASE_FANGYU_ALL_MAX)%100;//TakeTrader(-pNpc->m_TempFireResist*BASE_FANGYU_ALL_MAX,100);
		
		if  (-pNpc->m_TempFireResist==1)
			nRes=1;
		
		if (nRer>=50)
			nRes +=1; //4éá5è?
	}
	
	
	if (nIsadd==1)	//?yêy
        pNpc->m_CurrentFireResist=pNpc->m_FireResist+nRes;	 //pNpc->m_FireResist ?-ê?μ??e·à
	else           //?oêy
		pNpc->m_CurrentFireResist=pNpc->m_FireResist-nRes;
*/
	pNpc->m_CurrentFireResist= pNpc->m_FireResist+pNpc->m_TempFireResist;


	//if (pNpc->m_CurrentFireResist>BASE_FANGYU_ALL_MAX)
	//    pNpc->m_CurrentFireResistMax = pNpc->m_CurrentFireResist;
	//
	//if (pNpc->m_CurrentFireResistMax > MAX_RESIST)
 //       pNpc->m_CurrentFireResistMax = MAX_RESIST;

	//if (pNpc->m_CurrentFireResistMax<=BASE_FANGYU_ALL_MAX)
	//	pNpc->m_CurrentFireResistMax=BASE_FANGYU_ALL_MAX;


	//if (pNpc->m_CurrentFireResist>pNpc->m_CurrentFireResistMax)
	//	pNpc->m_CurrentFireResist=pNpc->m_CurrentFireResistMax;
	
	if (pNpc->m_CurrentFireResist > pNpc->m_FireResistMax)
		pNpc->m_CurrentFireResist = pNpc->m_FireResistMax;

	//g_DebugLog("[数值]%s火抗性增加了%d点", pNpc->Name, pMagic->nValue[0]);
}

void KNpcAttribModify::ArmorDefenseV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentDefend += pMagic->nValue[0];

	if (pNpc->m_CurrentDefend<0)
	    pNpc->m_CurrentDefend=0;
	//g_DebugLog("[数值]%s闪避增加了%d点", pNpc->Name, pMagic->nValue[0]);
}
//造成造对方迟缓时间 
void KNpcAttribModify::ColdEnhanceP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentColdEnhance += pMagic->nValue[0];

    //if (pNpc->m_CurrentColdEnhance<-100)
	//    pNpc->m_CurrentColdEnhance=-90;
    if (pNpc->m_CurrentColdEnhance<=-100)
	    pNpc->m_CurrentColdEnhance=-90;
    

	//g_DebugLog("[数值]%s迟缓时间减少值%d点", pNpc->Name, pMagic->nValue[0]);
}

void KNpcAttribModify::FireEnhanceP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentFireEnhance += pMagic->nValue[0];
	//g_DebugLog("[数值]%s火增强增加了%d点", pNpc->Name, pMagic->nValue[0]);
}

void KNpcAttribModify::LightingEnhanceP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentLightEnhance += pMagic->nValue[0];

	if  (pNpc->m_CurrentLightEnhance<0)
	    pNpc->m_CurrentLightEnhance=0;
	//g_DebugLog("[数值]%s电增强增加了%d点", pNpc->Name, pMagic->nValue[0]);
}

void KNpcAttribModify::PoisonEnhanceP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentPoisonEnhance += pMagic->nValue[0];

    if (pNpc->m_CurrentPoisonEnhance<-100)
	    pNpc->m_CurrentPoisonEnhance=-100;
	   
	//g_DebugLog("[数值]%s毒发时间减少值增加%d百分比", pNpc->Name, pMagic->nValue[0]);
}
//药品加血状态
void KNpcAttribModify::LifePotionV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;

	if (pMagic->nValue[1] <= 0)
		return;

	int nX1, nY1, nX2, nY2;
	nX1 = pNpc->m_LifeState.nValue[0];
	nY1 = pNpc->m_LifeState.nTime;
	nX2 = pMagic->nValue[0];  
	nY2 = pMagic->nValue[1];
	pNpc->m_LifeState.nTime = max(nY1, nY2);
	// 传入参数保证时间大于零，所以最大值一定大于零，所以不用判断除零
	if  (pNpc->m_LifeState.nTime!=0)
	   pNpc->m_LifeState.nValue[0] = (nX1 * nY1 + nX2 * nY2) / pNpc->m_LifeState.nTime;
	else
	   pNpc->m_LifeState.nValue[0] = 0;
	//g_DebugLog("[数值]%s喝了补血药，获得%d帧内每10帧回复%d点生命的效果", pNpc->Name,
	//	pNpc->m_LifeState.nTime, 
	//	pNpc->m_LifeState.nValue[0]);
}

//药品减毒状态
void KNpcAttribModify::dupotion_v(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	
	if (pMagic->nValue[1] <= 0)
		return;

	//if (pNpc->m_PoisonState.nTime<=0)
	//	return;

	if (pNpc->m_PoisonState.nTime <= 0)
		return;

    //m_PoisonState.nValue[1] 毒发时间   m_PoisonState.nValue[0]  毒伤害
	pNpc->m_PoisonState.nTime = pNpc->m_PoisonState.nTime + pMagic->nValue[0];
	
	if (pNpc->m_PoisonState.nTime<0)
		pNpc->m_PoisonState.nTime = 0;
	/*
	pNpc->m_PoisonState.nTime +=pMagic->nValue[0];

	if (pNpc->m_PoisonState.nTime<0)
		pNpc->m_PoisonState.nTime=0;
	 */
	//pMagic->nValue[0];  //减少值  
	//pMagic->nValue[1];  //持续时间

}

void KNpcAttribModify::ManaPotionV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;

	if (pMagic->nValue[1] <= 0)
		return;
	int nX1, nY1, nX2, nY2;
	nX1 = pNpc->m_ManaState.nValue[0];
	nY1 = pNpc->m_ManaState.nTime;
	nX2 = pMagic->nValue[0];
	nY2 = pMagic->nValue[1];
	pNpc->m_ManaState.nTime = max(nY1, nY2);
	// 传入参数保证时间大于零，所以最大值一定大于零，所以不用判断除零
	if (pNpc->m_ManaState.nTime!=0) 
	   pNpc->m_ManaState.nValue[0] = (nX1 * nY1 + nX2 * nY2) / pNpc->m_ManaState.nTime;
	else
	   pNpc->m_ManaState.nValue[0] = 0; 
	//g_DebugLog("[数值]%s喝了补内力药，获得%d帧内每10帧回复%d点内力的效果", pNpc->Name,
	//	pNpc->m_ManaState.nTime, 
	//	pNpc->m_ManaState.nValue[0]);
}

void KNpcAttribModify::PhysicsResMaxP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;

	pNpc->m_CurrentPhysicsResistMax += pMagic->nValue[0];

	if (pNpc->m_CurrentPhysicsResistMax>pNpc->m_PoisonResistMax)
		pNpc->m_CurrentPhysicsResistMax=pNpc->m_PoisonResistMax;

	//g_DebugLog("[数值]%s物理抗性上限增加了%d点", pNpc->Name, pMagic->nValue[0]);	
}

void KNpcAttribModify::ColdResMaxP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentColdResistMax += pMagic->nValue[0];

	if (pNpc->m_CurrentColdResistMax>pNpc->m_ColdResistMax)
		pNpc->m_CurrentColdResistMax=pNpc->m_ColdResistMax;

    

	//g_DebugLog("[数值]%s冰抗性上限增加了%d点", pNpc->Name, pMagic->nValue[0]);	
}

void KNpcAttribModify::FireResMaxP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentFireResistMax += pMagic->nValue[0];

	if (pNpc->m_CurrentFireResistMax>pNpc->m_FireResistMax)
		pNpc->m_CurrentFireResistMax=pNpc->m_FireResistMax;

	//g_DebugLog("[数值]%s火抗性上限增加了%d点", pNpc->Name, pMagic->nValue[0]);	
}

void KNpcAttribModify::LightingResMaxP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentLightResistMax += pMagic->nValue[0];

	if (pNpc->m_CurrentLightResistMax>pNpc->m_LightResistMax)
		pNpc->m_CurrentLightResistMax=pNpc->m_LightResistMax;
	//g_DebugLog("[数值]%s电抗性上限增加了%d点", pNpc->Name, pMagic->nValue[0]);	
}

void KNpcAttribModify::PoisonResMaxP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentPoisonResistMax += pMagic->nValue[0];

	if (pNpc->m_CurrentPoisonResistMax>pNpc->m_PoisonResistMax)
		pNpc->m_CurrentPoisonResistMax=pNpc->m_PoisonResistMax;

	//g_DebugLog("[数值]%s毒抗性上限增加了%d点", pNpc->Name, pMagic->nValue[0]);	
}
//全抗上限 百分比
void KNpcAttribModify::AllResMaxP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentFireResistMax    += pMagic->nValue[0];
	pNpc->m_CurrentColdResistMax    += pMagic->nValue[0];
	pNpc->m_CurrentLightResistMax   += pMagic->nValue[0];
	pNpc->m_CurrentPoisonResistMax  += pMagic->nValue[0];
	pNpc->m_CurrentPhysicsResistMax += pMagic->nValue[0];

	if (pNpc->m_CurrentPoisonResistMax>pNpc->m_PoisonResistMax)
		pNpc->m_CurrentPoisonResistMax=pNpc->m_PoisonResistMax;
	if (pNpc->m_CurrentLightResistMax>pNpc->m_LightResistMax)
		pNpc->m_CurrentLightResistMax=pNpc->m_LightResistMax;
	if (pNpc->m_CurrentFireResistMax>pNpc->m_FireResistMax)
		pNpc->m_CurrentFireResistMax=pNpc->m_FireResistMax;
	if (pNpc->m_CurrentColdResistMax>pNpc->m_ColdResistMax)
		pNpc->m_CurrentColdResistMax=pNpc->m_ColdResistMax;
	if (pNpc->m_CurrentPhysicsResistMax>pNpc->m_PhysicsResistMax)
		pNpc->m_CurrentPhysicsResistMax=pNpc->m_PhysicsResistMax;

	//g_DebugLog("[数值]%s五防抗性上限增加%d", pNpc->Name, pMagic->nValue[0]);
}

void KNpcAttribModify::FatallyStrikeP(KNpc* pNpc, void* pData)
{
}
//合成毒伤害
void KNpcAttribModify::MixPoisonDamage(KMagicAttrib* pDes, KMagicAttrib* pSrc)
{
	int d1, d2, t1, t2, c1, c2;
	d1 = pDes->nValue[0];
	d2 = pSrc->nValue[0];
	t1 = pDes->nValue[1];
	t2 = pSrc->nValue[1];
	c1 = pDes->nValue[2];
	c2 = pSrc->nValue[2];
	if (c1 == 0 || d1 == 0)
	{
		memcpy(pDes, pSrc, sizeof(KMagicAttrib));
		return;
	}
	if (c2 == 0 || d2 == 0)
	{
		return;
	}
	pDes->nValue[0] = ((c1 + c2) * d1 / c1 + (c1 + c2) * d2 / c2) / 2;
	pDes->nValue[1] = (t1 * d1 * c2 + t2 *d2 * c1) /(d1 * c2 + d2 * c1);
	pDes->nValue[2] += (c1 + c2) / 2;

/*	int NpcPoisonDamageMax=0,PlayerPoisonDamageMax=0; //合成毒伤害的最大值    
    g_GameSetTing.GetInteger("ServerConfig","NpcPoisonDamageMax",0,&NpcPoisonDamageMax);
    g_GameSetTing.GetInteger("ServerConfig","PlayerPoisonDamageMax",0,&PlayerPoisonDamageMax);
	if (pDes->nValue[2] >= NpcPoisonDamageMax)
		pDes->nValue[2]  = NpcPoisonDamageMax;*/

}
//普点（技能数据）
void KNpcAttribModify::AddPhysicsMagic( KNpc* pNpc, void* pData )
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentPhysicsMagicDamageV.nValue[0] += pMagic->nValue[0];
	pNpc->m_CurrentPhysicsMagicDamageV.nValue[2] += pMagic->nValue[0];
	//printf("STVL - Ngoai DIEM:  %d - %d",pNpc->m_CurrentPhysicsMagicDamageV.nValue[0], pNpc->m_CurrentPhysicsMagicDamageV.nValue[2]);
	//g_DebugLog("[数值]内普点%s普伤害增加(%d)", pNpc->Name, pMagic->nValue[0]);
}
//普点百分比（技能数据）
void KNpcAttribModify::Add_neiphysicsenhance_p(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentPhysicsMagicDamageP.nValue[0] += pMagic->nValue[0];
	pNpc->m_CurrentPhysicsMagicDamageP.nValue[2] += pMagic->nValue[0];

	if (pNpc->m_CurrentPhysicsMagicDamageP.nValue[0]<0)
	   pNpc->m_CurrentPhysicsMagicDamageP.nValue[0]=GetRandomNumber(10,20);

	if (pNpc->m_CurrentPhysicsMagicDamageP.nValue[2]<0)
	    pNpc->m_CurrentPhysicsMagicDamageP.nValue[2]=pNpc->m_CurrentPhysicsMagicDamageP.nValue[0]+GetRandomNumber(10,20);
/*#ifdef _SERVER
    char nmsg[64];
	sprintf(nmsg,"C原始百分比%d,技能白分比:%d",pNpc->m_CurrentPhysicsMagicDamageP.nValue[0],pMagic->nValue[0]);
    //pNpc->ClientShowMsg(nmsg);
	Player[pNpc->m_nPlayerIdx].m_ItemList.msgshow(nmsg);
#endif
	*/
}
//冰伤害（技能数据）Player[pNpc->m_nPlayerIdx].
void KNpcAttribModify::AddColdMagic( KNpc* pNpc, void* pData )
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentMagicColdDamage.nValue[0] += pMagic->nValue[0];
	pNpc->m_CurrentMagicColdDamage.nValue[2] += pMagic->nValue[0];
	if (pNpc->m_CurrentMagicColdDamage.nValue[0] > 0 && pNpc->m_CurrentMagicColdDamage.nValue[2] > 0)
	{
		//
		pNpc->m_CurrentMagicColdDamage.nValue[1] = pNpc->m_CurrentMagicColdDamage.nValue[0] * defNORMAL_COLD_TIME / 100;
		if(pNpc->m_CurrentMagicColdDamage.nValue[1] > defNORMAL_COLD_TIME)
			pNpc->m_CurrentMagicColdDamage.nValue[1] = defNORMAL_COLD_TIME;
	}
	else
	{
		pNpc->m_CurrentMagicColdDamage.nValue[0] = 0;
		pNpc->m_CurrentMagicColdDamage.nValue[1] = 0;
		pNpc->m_CurrentMagicColdDamage.nValue[2] = 0;
	}
#ifdef _DEBUG
	g_DebugLog("[addcoldmagic_v] Name: %s - Value1: %d - Value2: %d - Value3: %d", pNpc->Name, pNpc->m_CurrentMagicColdDamage.nValue[0], pNpc->m_CurrentMagicColdDamage.nValue[1], pNpc->m_CurrentMagicColdDamage.nValue[2]);
#endif
}

//火伤害（技能数据）
void KNpcAttribModify::AddFireMagic( KNpc* pNpc, void* pData )
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentMagicFireDamage.nValue[0] += pMagic->nValue[0];
	pNpc->m_CurrentMagicFireDamage.nValue[2] += pMagic->nValue[0];
	//g_DebugLog("[数值]%s火伤害增加(%d)", pNpc->Name, pMagic->nValue[0]);

    if (pNpc->m_CurrentMagicFireDamage.nValue[0]<0)
	   pNpc->m_CurrentMagicFireDamage.nValue[0]=GetRandomNumber(10,20);

	if (pNpc->m_CurrentMagicFireDamage.nValue[2]<0)
	    pNpc->m_CurrentMagicFireDamage.nValue[2]=pNpc->m_CurrentMagicFireDamage.nValue[0]+GetRandomNumber(10,20);

}
//雷伤害（技能数据）
void KNpcAttribModify::AddLightingMagic( KNpc* pNpc, void* pData )
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentMagicLightDamage.nValue[0] += pMagic->nValue[0];
	pNpc->m_CurrentMagicLightDamage.nValue[2] += pMagic->nValue[0];
	//g_DebugLog("[数值]%s电伤害增加(%d)", pNpc->Name, pMagic->nValue[0]);
	if (pNpc->m_CurrentMagicLightDamage.nValue[0]<0)
	   pNpc->m_CurrentMagicLightDamage.nValue[0]=GetRandomNumber(10,20);

	if (pNpc->m_CurrentMagicLightDamage.nValue[2]<0)
	    pNpc->m_CurrentMagicLightDamage.nValue[2]=pNpc->m_CurrentMagicLightDamage.nValue[0]+GetRandomNumber(10,20);
}
//毒伤害（技能数据）
void KNpcAttribModify::AddPoisonMagic( KNpc* pNpc, void* pData )
{
#define	defPOISON_DAMAGE_TIME		60  //持续时间
#define	defPOISON_DAMAGE_INTERVAL	10  //毒发时间

	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentMagicPoisonDamage.nValue[0] += pMagic->nValue[0];
	
	if (pNpc->m_CurrentMagicPoisonDamage.nValue[0] > 0)  //毒伤害
	{
		pNpc->m_CurrentMagicPoisonDamage.nValue[1] = defPOISON_DAMAGE_TIME;    //pMagic->nValue[1];
		pNpc->m_CurrentMagicPoisonDamage.nValue[2] = defPOISON_DAMAGE_INTERVAL;//pMagic->nValue[2];
	}
	else
	{
		pNpc->m_CurrentMagicPoisonDamage.nValue[0] = 0;
		pNpc->m_CurrentMagicPoisonDamage.nValue[1] = 0;
		pNpc->m_CurrentMagicPoisonDamage.nValue[2] = 0;
	}	
	//g_DebugLog("[数值]%s毒状态变为：间隔%d帧发作一次%d伤害，共持续%d帧", pNpc->Name, pNpc->m_CurrentPoisonDamage.nValue[2], pNpc->m_CurrentPoisonDamage.nValue[0], pNpc->m_CurrentPoisonDamage.nValue[1]);
}

////////////////////////新攻击技能////////////////////////////////
//新普点
/*void KNpcAttribModify::XinPhysicsDamageV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;

	pNpc->m_WaiPhysicsDamage.nValue[0] += pMagic->nValue[0];
	pNpc->m_WaiPhysicsDamage.nValue[2] += pMagic->nValue[2];
	//g_DebugLog("[数值]外功普点%s物理伤害力增加(%d)", pNpc->Name, pMagic->nValue[0]);
}

//新冰伤害
void KNpcAttribModify::XinColdDamageV(KNpc* pNpc, void* pData)
{	
#define	defNORMAL_COLD_TIME	60  //冰冻时间
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentColdDamage.nValue[0] += pMagic->nValue[0];
	pNpc->m_CurrentColdDamage.nValue[2] += pMagic->nValue[0];
	if (pNpc->m_CurrentColdDamage.nValue[0] > 0 && pNpc->m_CurrentColdDamage.nValue[2] > 0)
		pNpc->m_CurrentColdDamage.nValue[1] = defNORMAL_COLD_TIME;
	else
	{
		pNpc->m_CurrentColdDamage.nValue[0] = 0;
		pNpc->m_CurrentColdDamage.nValue[1] = 0;
		pNpc->m_CurrentColdDamage.nValue[2] = 0;
	}
	//g_DebugLog("[数值]%s冰伤害增加(%d)", pNpc->Name, pMagic->nValue[0]);
}
//新火伤害
void KNpcAttribModify::XinFireDamageV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentFireDamage.nValue[0] += pMagic->nValue[0];
	pNpc->m_CurrentFireDamage.nValue[2] += pMagic->nValue[0];
	//g_DebugLog("[数值]%s火伤害增加(%d)", pNpc->Name, pMagic->nValue[0]);
}
//新雷伤害
void KNpcAttribModify::XinLightingDamageV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentLightDamage.nValue[0] += pMagic->nValue[0];
	pNpc->m_CurrentLightDamage.nValue[2] += pMagic->nValue[0];
	//g_DebugLog("[数值]%s电伤害增加(%d)", pNpc->Name, pMagic->nValue[0]);
}
//新毒伤害
void KNpcAttribModify::XinPoisonDamageV(KNpc* pNpc, void* pData)
{
#define	defPOISON_DAMAGE_TIME		60
#define	defPOISON_DAMAGE_INTERVAL	10
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentPoisonDamage.nValue[0] += pMagic->nValue[0];
	
	if (pNpc->m_CurrentPoisonDamage.nValue[0] > 0)
	{
		pNpc->m_CurrentPoisonDamage.nValue[1] = defPOISON_DAMAGE_TIME;//pMagic->nValue[1];
		pNpc->m_CurrentPoisonDamage.nValue[2] = defPOISON_DAMAGE_INTERVAL;//pMagic->nValue[2];
	}
	else
	{
		pNpc->m_CurrentPoisonDamage.nValue[0] = 0;   //伤害
		pNpc->m_CurrentPoisonDamage.nValue[1] = 0;   //持续时间
		pNpc->m_CurrentPoisonDamage.nValue[2] = 0;   //间隔时间发作一次
	}	
	//g_DebugLog("[数值]%s毒状态变为：间隔%d帧发作一次%d伤害，共持续%d帧", pNpc->Name, pNpc->m_CurrentPoisonDamage.nValue[2], pNpc->m_CurrentPoisonDamage.nValue[0], pNpc->m_CurrentPoisonDamage.nValue[1]);	
}
//新普百分比伤害
void KNpcAttribModify::XinPhysicsDamageP(KNpc* pNpc, void* pData)
{
#define WEAPON_ALL			((MAX_MELEE_WEAPON) + 0)
#define	WEAPON_RANGE_ALL	((MAX_MELEE_WEAPON) + 1)
#define	WEAPON_MELEE_ALL	((MAX_MELEE_WEAPON) + 2)
#define	WEAPON_NONE			((MAX_MELEE_WEAPON) + 3)

	KMagicAttrib* pMagic = (KMagicAttrib *)pData;

	int nType = abs(pMagic->nValue[2]);

	if (nType == WEAPON_ALL)
	{
		for (int i = 0; i < MAX_MELEE_WEAPON; ++i)
		{
			pNpc->m_CurrentMeleeEnhance[i] += pMagic->nValue[0];
		}
		pNpc->m_CurrentHandEnhance += pMagic->nValue[0];
		pNpc->m_CurrentRangeEnhance += pMagic->nValue[0];
		//g_DebugLog("[数值]%s所有物理攻击力增加百分之%d", pNpc->Name, pMagic->nValue[0]);
	}
	else if (nType == WEAPON_RANGE_ALL)
	{
		pNpc->m_CurrentRangeEnhance += pMagic->nValue[0];
		//g_DebugLog("[数值]%s远程物理攻击力增加百分之%d", pNpc->Name, pMagic->nValue[0]);
	}
	else if (nType == WEAPON_NONE)
	{
		pNpc->m_CurrentHandEnhance += pMagic->nValue[0];
		//g_DebugLog("[数值]%s空手物理攻击力增加百分之%d", pNpc->Name, pMagic->nValue[0]);
	}
	else if (nType == WEAPON_MELEE_ALL)
	{
		for (int i = 0; i < MAX_MELEE_WEAPON; ++i)
		{
			pNpc->m_CurrentMeleeEnhance[i] += pMagic->nValue[0];
		}
		//g_DebugLog("[数值]%s近程物理攻击力增加百分之%d", pNpc->Name, pMagic->nValue[0]);
	}
	else if (nType >= 0 && nType < MAX_MELEE_WEAPON)
	{
		pNpc->m_CurrentMeleeEnhance[nType] += pMagic->nValue[0];
		//g_DebugLog("[数值]%s近程武器%d物理攻击力增加百分之%d", pNpc->Name, nType, pMagic->nValue[0]);
	}
	
}
*/
/////////////////////////装备数据（无状态）和技能数据(有状态）/////////////////////////
//	
void KNpcAttribModify::AddPhysicsDamagePP(KNpc* pNpc, void* pData)
{
#define WEAPON_ALL			((MAX_MELEE_WEAPON) + 0)
#define	WEAPON_RANGE_ALL	((MAX_MELEE_WEAPON) + 1)
#define	WEAPON_MELEE_ALL	((MAX_MELEE_WEAPON) + 2)
#define	WEAPON_NONE			((MAX_MELEE_WEAPON) + 3)

	KMagicAttrib* pMagic = (KMagicAttrib *)pData;

	int nType = abs(pMagic->nValue[2]);

	if (nType == WEAPON_ALL)
	{
		for (int i = 0; i < MAX_MELEE_WEAPON; i++)
		{
			pNpc->m_CurrentMeleeEnhance[i] += pMagic->nValue[0];
		}
		pNpc->m_CurrentHandEnhance += pMagic->nValue[0];
		pNpc->m_CurrentRangeEnhance += pMagic->nValue[0];
	}
	else if (nType == WEAPON_RANGE_ALL)
	{
		pNpc->m_CurrentRangeEnhance += pMagic->nValue[0];
	}
	else if (nType == WEAPON_NONE)
	{
		pNpc->m_CurrentHandEnhance += pMagic->nValue[0];
	}
	else if (nType == WEAPON_MELEE_ALL)
	{
		for (int i = 0; i < MAX_MELEE_WEAPON; i++)
		{
			pNpc->m_CurrentMeleeEnhance[i] += pMagic->nValue[0];
		}
	}
	else if (nType >= 0 && nType < MAX_MELEE_WEAPON)
	{
		pNpc->m_CurrentMeleeEnhance[nType] += pMagic->nValue[0];
	}
}
//STVL - Ngoai DIEM: 12
void KNpcAttribModify::Addphysicsdamagevp(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentAddPhysicsDamage += pMagic->nValue[0];

}
//当前外火点 被动
void KNpcAttribModify::Aaddfiredamagevp(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentFireDamage.nValue[0] += pMagic->nValue[0];
	pNpc->m_CurrentFireDamage.nValue[2] += pMagic->nValue[0];
	//g_DebugLog("[数值]%s火伤害增加(%d)", pNpc->Name, pMagic->nValue[0]);
}
//当前外冰点 被动
void KNpcAttribModify::Addcolddamagevp(KNpc* pNpc, void* pData)
{	
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentColdDamage.nValue[0] += pMagic->nValue[0];
	pNpc->m_CurrentColdDamage.nValue[2] += pMagic->nValue[0];
	if (pNpc->m_CurrentColdDamage.nValue[0] > 0 && pNpc->m_CurrentColdDamage.nValue[2] > 0)
	{	
		//
		pNpc->m_CurrentColdDamage.nValue[1] = pNpc->m_CurrentColdDamage.nValue[0] * defNORMAL_COLD_TIME / 100;
		if(pNpc->m_CurrentColdDamage.nValue[1] > defNORMAL_COLD_TIME)
			pNpc->m_CurrentColdDamage.nValue[1] = defNORMAL_COLD_TIME;
		
	}
	else
	{
		pNpc->m_CurrentColdDamage.nValue[0] = 0;
		pNpc->m_CurrentColdDamage.nValue[1] = 0;
		pNpc->m_CurrentColdDamage.nValue[2] = 0;
	}
}
//当前外雷点 被动
void KNpcAttribModify::Addlightingdamagevp(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentLightDamage.nValue[0] += pMagic->nValue[0];
	pNpc->m_CurrentLightDamage.nValue[2] += pMagic->nValue[0];
	//g_DebugLog("[数值]%s电伤害增加(%d)", pNpc->Name, pMagic->nValue[0]);
}
//当前外毒点 被动
void KNpcAttribModify::Addpoisondamagevp(KNpc* pNpc, void* pData)
{
#define	defPOISON_DAMAGE_TIME		60
#define	defPOISON_DAMAGE_INTERVAL	10
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentPoisonDamage.nValue[0] += pMagic->nValue[0];

	if (pNpc->m_CurrentPoisonDamage.nValue[0] > 0)
	{
		pNpc->m_CurrentPoisonDamage.nValue[1] = defPOISON_DAMAGE_TIME;//pMagic->nValue[1];
		pNpc->m_CurrentPoisonDamage.nValue[2] = defPOISON_DAMAGE_INTERVAL;//pMagic->nValue[2];
	}
	else
	{
		pNpc->m_CurrentPoisonDamage.nValue[0] = 0;
		pNpc->m_CurrentPoisonDamage.nValue[1] = 0;
		pNpc->m_CurrentPoisonDamage.nValue[2] = 0;
	}	
	///g_DebugLog("[数值]%s毒状态变为：间隔%d帧发作一次%d伤害，共持续%d帧", pNpc->Name, pNpc->m_CurrentPoisonDamage.nValue[2], pNpc->m_CurrentPoisonDamage.nValue[0], pNpc->m_CurrentPoisonDamage.nValue[1]);	
}



//当前内普百分比 被动
void KNpcAttribModify::Addphysicsmagicp(KNpc* pNpc, void* pData)
{
    KMagicAttrib* pMagic = (KMagicAttrib *)pData;
    pNpc->m_CurrentAddmagicphysicsDamageP +=pMagic->nValue[0]; 

	if (pNpc->m_CurrentAddmagicphysicsDamageP<-100)
	   pNpc->m_CurrentAddmagicphysicsDamageP=-100;
}

//dame vat ly noi cong
void KNpcAttribModify::Addphysicsmagicvb(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_MagicPhysicsDamage.nValue[0] += pMagic->nValue[0];
	pNpc->m_MagicPhysicsDamage.nValue[2] += pMagic->nValue[0];
#ifdef _DEBUG
	//g_DebugLog("[addphysicsmagic_v] Name: %s - Value1: %d - Value2: %d", pNpc->Name, pNpc->m_MagicPhysicsDamage.nValue[0], pNpc->m_MagicPhysicsDamage.nValue[2]);
#endif
}

//当前内冰点 被动
void KNpcAttribModify::Addcoldmagicvp(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentMagicColdDamage.nValue[0] += pMagic->nValue[0];
	pNpc->m_CurrentMagicColdDamage.nValue[2] += pMagic->nValue[0];
	if (pNpc->m_CurrentMagicColdDamage.nValue[0] > 0 && pNpc->m_CurrentMagicColdDamage.nValue[2] > 0)
	{
		//
		pNpc->m_CurrentMagicColdDamage.nValue[1] = pNpc->m_CurrentMagicColdDamage.nValue[0] * defNORMAL_COLD_TIME / 100;
		if(pNpc->m_CurrentMagicColdDamage.nValue[1] > defNORMAL_COLD_TIME)
			pNpc->m_CurrentMagicColdDamage.nValue[1] = defNORMAL_COLD_TIME;
		
	}
	else
	{
		pNpc->m_CurrentMagicColdDamage.nValue[0] = 0;
		pNpc->m_CurrentMagicColdDamage.nValue[1] = 0;
		pNpc->m_CurrentMagicColdDamage.nValue[2] = 0;
	}
#ifdef _DEBUG
	g_DebugLog("[addcoldmagic_v] Name: %s - Value1: %d - Value2: %d - Value3: %d", pNpc->Name, pNpc->m_CurrentMagicColdDamage.nValue[0], pNpc->m_CurrentMagicColdDamage.nValue[1], pNpc->m_CurrentMagicColdDamage.nValue[2]);
#endif
}
//当前内火点 被动
void KNpcAttribModify::Addfiremagicv(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentMagicFireDamage.nValue[0] += pMagic->nValue[0];
	pNpc->m_CurrentMagicFireDamage.nValue[2] += pMagic->nValue[0];
//#ifdef _DEBUG 
//	printf("[addfiremagic_v] Name: %s - Value1: %d - Value2: %d\n", pNpc->Name, pNpc->m_CurrentMagicFireDamage.nValue[0], pNpc->m_CurrentMagicFireDamage.nValue[2]);
//#endif
}

//当前内雷点 被动
void KNpcAttribModify::Addlightingmagicv(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentMagicLightDamage.nValue[0] += pMagic->nValue[0];
	pNpc->m_CurrentMagicLightDamage.nValue[2] += pMagic->nValue[0];
#ifdef _DEBUG
	//g_DebugLog("[addlightingmagic_v] Name: %s - Value1: %d - Value2: %d", pNpc->Name, pNpc->m_CurrentMagicLightDamage.nValue[0], pNpc->m_CurrentMagicLightDamage.nValue[2]);
#endif
} 
//当前内毒点 被动
void KNpcAttribModify::Addpoisonmagicv(KNpc* pNpc, void* pData)
{
#define	defPOISON_DAMAGE_TIME		60
#define	defPOISON_DAMAGE_INTERVAL	10
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentMagicPoisonDamage.nValue[0] += pMagic->nValue[0];
	
	if (pNpc->m_CurrentMagicPoisonDamage.nValue[0] > 0)
	{
		pNpc->m_CurrentMagicPoisonDamage.nValue[1] = defPOISON_DAMAGE_TIME;
		pNpc->m_CurrentMagicPoisonDamage.nValue[2] = defPOISON_DAMAGE_INTERVAL;
	}
	else
	{
		pNpc->m_CurrentMagicPoisonDamage.nValue[0] = 0;
		pNpc->m_CurrentMagicPoisonDamage.nValue[1] = 0;
		pNpc->m_CurrentMagicPoisonDamage.nValue[2] = 0;
	}
#ifdef _DEBUG
	//g_DebugLog("[addpoisonmagic_v] Name: %s - Value1: %d - Value2: %d - Value3: %d", pNpc->Name, pNpc->m_CurrentMagicPoisonDamage.nValue[0], pNpc->m_CurrentMagicPoisonDamage.nValue[1], pNpc->m_CurrentMagicPoisonDamage.nValue[2]);
#endif
}

///////////////////////////////////////////////////////////////////

void KNpcAttribModify::fatallystrikeresP( KNpc* pNpc, void* pData ) //
{
	
}

void KNpcAttribModify::me2metaldamage_p( KNpc* pNpc, void* pData ) //=对金系伤害增加：#d1+%
{
    KMagicAttrib* pMagic = (KMagicAttrib *)pData;
    pNpc->m_Me2metaldamage_p +=pMagic->nValue[0]; 

	if (pNpc->m_Me2metaldamage_p>=100)
	   pNpc->m_Me2metaldamage_p=GetRandomNumber(80,90);

}
void KNpcAttribModify::metal2medamage_p( KNpc* pNpc, void* pData ) //=减少来自金系的伤害：#d1-%
{
    KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_Metal2medamage_p +=pMagic->nValue[0];

	if (pNpc->m_Metal2medamage_p>=100)
	   pNpc->m_Metal2medamage_p=GetRandomNumber(80,90);
}
void KNpcAttribModify::me2wooddamage_p( KNpc* pNpc, void* pData ) //=对木系伤害增加：#d1+%
{
    KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_Me2wooddamage_p +=pMagic->nValue[0];
		if (pNpc->m_Me2wooddamage_p>=100)
	   pNpc->m_Me2wooddamage_p=GetRandomNumber(80,90);
}
void KNpcAttribModify::wood2medamage_p( KNpc* pNpc, void* pData ) //=减少来自木系的伤害：#d1-%
{
    KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_Wood2medamage_p +=pMagic->nValue[0];
	 if (pNpc->m_Wood2medamage_p>=100)
	   pNpc->m_Wood2medamage_p=GetRandomNumber(80,90);
}
void KNpcAttribModify::me2waterdamage_p( KNpc* pNpc, void* pData ) //=对水系伤害增加：#d1+%
{
    KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_Me2waterdamage_p +=pMagic->nValue[0];
	 if (pNpc->m_Me2waterdamage_p>=100)
	   pNpc->m_Me2waterdamage_p=GetRandomNumber(80,90);
}
void KNpcAttribModify::water2medamage_p( KNpc* pNpc, void* pData ) //=减少来自水系的伤害：#d1-%
{
    KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_Water2medamage_p +=pMagic->nValue[0];
	if (pNpc->m_Water2medamage_p>=100)
	   pNpc->m_Water2medamage_p=GetRandomNumber(80,90);
}
void KNpcAttribModify::me2firedamage_p( KNpc* pNpc, void* pData ) //=对火系伤害增加：#d1+%
{
    KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_Me2firedamage_p +=pMagic->nValue[0];
	if (pNpc->m_Me2firedamage_p>=100)
	   pNpc->m_Me2firedamage_p=GetRandomNumber(80,90);
}
void KNpcAttribModify::fire2medamage_p( KNpc* pNpc, void* pData ) //=减少来自火系的伤害：#d1-%
{
    KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_Fire2medamage_p +=pMagic->nValue[0];
	if (pNpc->m_Fire2medamage_p>=100)
	   pNpc->m_Fire2medamage_p=GetRandomNumber(80,90);
}
void KNpcAttribModify::me2earthdamage_p( KNpc* pNpc, void* pData ) //=对土系伤害增加：#d1+%
{
    KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_Me2earthdamage_p +=pMagic->nValue[0];
		if (pNpc->m_Me2earthdamage_p>=100)
	   pNpc->m_Me2earthdamage_p=GetRandomNumber(80,90);


}
void KNpcAttribModify::earth2medamage_p( KNpc* pNpc, void* pData ) //=减少来自土系的伤害：#d1-%
{
    KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_Earth2medamage_p +=pMagic->nValue[0];
		if (pNpc->m_Earth2medamage_p>=100)
	   pNpc->m_Earth2medamage_p=GetRandomNumber(80,90);
}

//状态执行脚本
void KNpcAttribModify::doscript_v(KNpc* pNpc, void* pData)
{
	   KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	   pNpc->m_DoScriptState.nMagicAttrib=0;
       pNpc->m_DoScriptState.nValue[0]=pMagic->nValue[0]; //每等级的经验
	   pNpc->m_DoScriptState.nValue[1]=pMagic->nValue[1]; //每间隔的多少秒执行一次
	   pNpc->m_DoScriptState.nValue[2]=pMagic->nValue[2]; //持续的时间为多少秒
	   pNpc->m_DoScriptState.nTime=pMagic->nValue[2];     //持续的时间为多少秒
}

void KNpcAttribModify::addexp_v(KNpc* pNpc, void* pData)
{
	   KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	   pNpc->m_ExpState.nMagicAttrib=0;
       pNpc->m_ExpState.nValue[0]=pMagic->nValue[0]; //每等级的经验
	   pNpc->m_ExpState.nValue[1]=pMagic->nValue[1]; //每间隔的多少秒执行一次
	   pNpc->m_ExpState.nValue[2]=pMagic->nValue[2]; //类型
	   pNpc->m_ExpState.nTime=pMagic->nValue[1]*2;   //持续的时间为多少秒
}

void KNpcAttribModify::randmove( KNpc* pNpc, void* pData)
{
    KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	if(pMagic->nValue[1] > 0)
		pNpc->m_randmove.nTime += pMagic->nValue[1];
		// printf("m_randmove: %d|%d|%d\n", pMagic->nValue[0], pMagic->nValue[1], pMagic->nValue[2]);
	else
		pNpc->m_randmove.nTime = 0;

#ifndef _SERVER
	  NpcAI.Setscpos(pNpc->m_Index);
#endif

}

void KNpcAttribModify::addbaopoisondmax_p( KNpc* pNpc, void* pData)
{
    KMagicAttrib* pMagic = (KMagicAttrib *)pData;
#ifdef _SERVER
	pNpc->m_Currentbaopoisondmax_p +=pMagic->nValue[0];
#endif
}




//自动激活全身
void KNpcAttribModify::creatstatus_v( KNpc* pNpc, void* pData )
{
       KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	   pNpc->m_DoScriptState.nMagicAttrib=0;
       pNpc->m_DoScriptState.nValue[0]=pMagic->nValue[0]; //每等级的经验
	   pNpc->m_DoScriptState.nValue[1]=pMagic->nValue[1]; //每间隔的多少秒执行一次
	   pNpc->m_DoScriptState.nValue[2]=pMagic->nValue[2]; //持续的时间为多少秒
	   pNpc->m_DoScriptState.nTime=pMagic->nValue[2];     //持续的时间为多少秒
}

//自动激活全身
void KNpcAttribModify::alljihuo_v( KNpc* pNpc, void* pData )
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
    pNpc->m_CurrentAllJiHuo +=pMagic->nValue[0];
}
//忽略对方命中率
void KNpcAttribModify::ignoreattacrating_v( KNpc* pNpc, void* pData )
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
    pNpc->m_CurrentIgnorenAttacRating +=pMagic->nValue[0];
}


//对某技能加成
void KNpcAttribModify::addskilldamage7( KNpc* pNpc, void* pData )
{

	KMagicAttrib* pMagic = (KMagicAttrib *)pData;

	if (pMagic->nValue[0] != 0)
	{
		int Same;
		if (pMagic->nValue[0] > 0)
			Same = pMagic->nValue[0];
		else
			Same = -pMagic->nValue[0];

		int nListIndex = pNpc->m_SkillList.FindSame(Same);
		if (nListIndex)
		{
			if (pNpc->m_SkillList.GetCurrentLevelByIdx(nListIndex) > 0)
				pNpc->m_SkillList.AddEnChance(nListIndex,pMagic->nValue[2]);		
		}
	}
	else if (pMagic->nValue[0] == 0)
	{
		for (int i = 1;i < MAX_NPCSKILL;++i)
		{
			if(pNpc->m_SkillList.m_Skills[i].SkillId&&pNpc->m_SkillList.m_Skills[i].SkillId!=1&&pNpc->m_SkillList.m_Skills[i].SkillId!=2&&pNpc->m_SkillList.m_Skills[i].SkillId!=53)
			{
				if (pNpc->m_SkillList.GetCurrentLevelByIdx(i) > 0)
					pNpc->m_SkillList.AddEnChance(i,pMagic->nValue[2]);					
			}
		}
	}
	//KMagicAttrib* pMagic = (KMagicAttrib *)pData;

	//if (pMagic->nValue[0]< 0)
	//	pMagic->nValue[0] = -pMagic->nValue[0];  //技能id

	//if (pNpc->nEnhanceInfo.count(pMagic->nValue[0])>0)
	//{
	//	pNpc->nEnhanceInfo[pMagic->nValue[0]].nSkillIdx = pMagic->nValue[0];
	//	int nTempval = pNpc->nEnhanceInfo[pMagic->nValue[0]].nEnhance + pMagic->nValue[2];
	//	pNpc->nEnhanceInfo[pMagic->nValue[0]].nEnhance = nTempval; //叠加总的加成
	//}
	//else
	//{	
	//	/*
	//	以下方法来避免开销：
 //       enumMap.insert(map<int, CString> :: value_type(2, "Two"))
	//	CEnhanceInfo nVal;
	//	nVal.nSkillIdx = pMagic->nValue[0];
	//	nVal.nEnhance = pMagic->nValue[2];
	//	pNpc->nEnhanceInfo.insert(pair<INT, CEnhanceInfo> :: value_type(pMagic->nValue[0], nVal));
	//  	 */
	//	pNpc->nEnhanceInfo[pMagic->nValue[0]].nSkillIdx = pMagic->nValue[0];
	//	pNpc->nEnhanceInfo[pMagic->nValue[0]].nEnhance  = pMagic->nValue[2];
	//}
	//if (pMagic->nValue[0]>0 && pMagic->nValue[2]!= 0)
	//{   
	//	int nActiveSkillID = pMagic->nValue[0];
	//	int nListIndex = pNpc->m_SkillList.FindSame(nActiveSkillID);
	//	if (nListIndex)
	//	{
	//		if (pNpc->m_SkillList.GetCurrentLevelByIdx(nListIndex) > 0)    //等级大于0的才 增加
	//				pNpc->m_SkillList.AddEnChance(nListIndex,pMagic->nValue[2]);		
	//	}
	//}
}

//对某技能加成
void KNpcAttribModify::addskilldamage1( KNpc* pNpc, void* pData )
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;

	if (pMagic->nValue[0] != 0)
	{
		int Same;
		if (pMagic->nValue[0] > 0)
			Same = pMagic->nValue[0];
		else
			Same = -pMagic->nValue[0];

		int nListIndex = pNpc->m_SkillList.FindSame(Same);
		if (nListIndex)
		{
			if (pNpc->m_SkillList.GetCurrentLevelByIdx(nListIndex) > 0)
				pNpc->m_SkillList.AddEnChance(nListIndex,pMagic->nValue[2]);// 当前技能的总加成数		
		}
	}
	else if (pMagic->nValue[0] == 0)  //如果没有指定技能，则对全部技能加成
	{
		for (int i = 1;i < MAX_NPCSKILL;++i)
		{
			if (pNpc->m_SkillList.m_Skills[i].SkillId&&
				pNpc->m_SkillList.m_Skills[i].SkillId!=1&&
				pNpc->m_SkillList.m_Skills[i].SkillId!=2&&
				pNpc->m_SkillList.m_Skills[i].SkillId!=53)
			{
				if (pNpc->m_SkillList.GetCurrentLevelByIdx(i) > 0)
					pNpc->m_SkillList.AddEnChance(i,pMagic->nValue[2]);
			}
		}
	}
}

void KNpcAttribModify::addskilldamage2( KNpc* pNpc, void* pData )
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;

	if (pMagic->nValue[0] != 0)
	{
		int Same;
		if (pMagic->nValue[0] > 0)
			Same = pMagic->nValue[0];
		else
			Same = -pMagic->nValue[0];
		int nListIndex = pNpc->m_SkillList.FindSame(Same);
		if (nListIndex)
		{
			if (pNpc->m_SkillList.GetCurrentLevelByIdx(nListIndex) > 0)
				pNpc->m_SkillList.AddEnChance(nListIndex,pMagic->nValue[2]);		
		}
	}
	else if (pMagic->nValue[0] == 0)
	{
		for (int i = 1;i < MAX_NPCSKILL;++i)
		{
			if(pNpc->m_SkillList.m_Skills[i].SkillId&&pNpc->m_SkillList.m_Skills[i].SkillId!=1&&pNpc->m_SkillList.m_Skills[i].SkillId!=2&&pNpc->m_SkillList.m_Skills[i].SkillId!=53)
			{
				if (pNpc->m_SkillList.GetCurrentLevelByIdx(i) > 0)
					pNpc->m_SkillList.AddEnChance(i,pMagic->nValue[2]);					
			}
		}
	}
}

void KNpcAttribModify::addskilldamage3( KNpc* pNpc, void* pData )
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;

	if (pMagic->nValue[0] != 0)
	{
		int Same;
		if (pMagic->nValue[0] > 0)
			Same = pMagic->nValue[0];
		else
			Same = -pMagic->nValue[0];
		int nListIndex = pNpc->m_SkillList.FindSame(Same);
		if (nListIndex)
		{
			if (pNpc->m_SkillList.GetCurrentLevelByIdx(nListIndex) > 0)
				pNpc->m_SkillList.AddEnChance(nListIndex,pMagic->nValue[2]);		
		}
	}
	else if (pMagic->nValue[0] == 0)
	{
		for (int i = 1;i < MAX_NPCSKILL;++i)
		{
			if(pNpc->m_SkillList.m_Skills[i].SkillId&&pNpc->m_SkillList.m_Skills[i].SkillId!=1&&pNpc->m_SkillList.m_Skills[i].SkillId!=2&&pNpc->m_SkillList.m_Skills[i].SkillId!=53)
			{
				if (pNpc->m_SkillList.GetCurrentLevelByIdx(i) > 0)
					pNpc->m_SkillList.AddEnChance(i,pMagic->nValue[2]);					
			}
		}
	}
}

void KNpcAttribModify::addskilldamage4( KNpc* pNpc, void* pData )
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;

	if (pMagic->nValue[0] != 0)
	{
		int Same;
		if (pMagic->nValue[0] > 0)
			Same = pMagic->nValue[0];
		else
			Same = -pMagic->nValue[0];
		int nListIndex = pNpc->m_SkillList.FindSame(Same);
		if (nListIndex)
		{
			if (pNpc->m_SkillList.GetCurrentLevelByIdx(nListIndex) > 0)
				pNpc->m_SkillList.AddEnChance(nListIndex,pMagic->nValue[2]);		
		}
	}
	else if (pMagic->nValue[0] == 0)
	{
		for (int i = 1;i < MAX_NPCSKILL;++i)
		{
			if(pNpc->m_SkillList.m_Skills[i].SkillId&&pNpc->m_SkillList.m_Skills[i].SkillId!=1&&pNpc->m_SkillList.m_Skills[i].SkillId!=2&&pNpc->m_SkillList.m_Skills[i].SkillId!=53)
			{
				if (pNpc->m_SkillList.GetCurrentLevelByIdx(i) > 0)
					pNpc->m_SkillList.AddEnChance(i,pMagic->nValue[2]);					
			}
		}
	}
}

void KNpcAttribModify::addskilldamage5( KNpc* pNpc, void* pData )
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;

	if (pMagic->nValue[0] != 0)
	{
		int Same;
		if (pMagic->nValue[0] > 0)
			Same = pMagic->nValue[0];
		else
			Same = -pMagic->nValue[0];
		int nListIndex = pNpc->m_SkillList.FindSame(Same);
		if (nListIndex)
		{
			if (pNpc->m_SkillList.GetCurrentLevelByIdx(nListIndex) > 0)
				pNpc->m_SkillList.AddEnChance(nListIndex,pMagic->nValue[2]);		
		}
	}
	else if (pMagic->nValue[0] == 0)
	{
		for (int i = 1;i < MAX_NPCSKILL;++i)
		{
			if(pNpc->m_SkillList.m_Skills[i].SkillId&&pNpc->m_SkillList.m_Skills[i].SkillId!=1&&pNpc->m_SkillList.m_Skills[i].SkillId!=2&&pNpc->m_SkillList.m_Skills[i].SkillId!=53)
			{
				if (pNpc->m_SkillList.GetCurrentLevelByIdx(i) > 0)
					pNpc->m_SkillList.AddEnChance(i,pMagic->nValue[2]);					
			}
		}
	}
}

void KNpcAttribModify::lock_life(KNpc* pNpc, void* pData)
{

}
void KNpcAttribModify::special_point(KNpc* pNpc, void* pData)
{

}
void KNpcAttribModify::resume_life_p(KNpc* pNpc, void* pData)
{

}
void KNpcAttribModify::reset_bufftime(KNpc* pNpc, void* pData)
{

}
void KNpcAttribModify::reset_bufftime_recv(KNpc* pNpc, void* pData)
{

}
void KNpcAttribModify::cost_sp(KNpc* pNpc, void* pData)
{

}
void KNpcAttribModify::cast_when_buff_removed(KNpc* pNpc, void* pData)
{

}
void KNpcAttribModify::autocastskill(KNpc* pNpc, void* pData)
{

}

void KNpcAttribModify::meleedamagereturnmana_p(KNpc* pNpc, void* pData)
{

}

void KNpcAttribModify::rangedamagereturnmana_p(KNpc* pNpc, void* pData)
{

}

void KNpcAttribModify::anti_do_hurt_p( KNpc* pNpc, void* pData )
{

}

void KNpcAttribModify::do_stun_p( KNpc* pNpc, void* pData )
{

}

void KNpcAttribModify::anti_do_stun_p( KNpc* pNpc, void* pData )
{

}

void KNpcAttribModify::addskilldamage10( KNpc* pNpc, void* pData )
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;

	if (pMagic->nValue[0] != 0)
	{
		int Same;
		if (pMagic->nValue[0] > 0)
			Same = pMagic->nValue[0];
		else
			Same = -pMagic->nValue[0];
		int nListIndex = pNpc->m_SkillList.FindSame(Same);
		if (nListIndex)
		{
			if (pNpc->m_SkillList.GetCurrentLevelByIdx(nListIndex) > 0)
				pNpc->m_SkillList.AddEnChance(nListIndex,pMagic->nValue[2]);		
		}
	}
	else if (pMagic->nValue[0] == 0)
	{
		for (int i = 1;i < MAX_NPCSKILL;++i)
		{
			if(pNpc->m_SkillList.m_Skills[i].SkillId&&pNpc->m_SkillList.m_Skills[i].SkillId!=1&&pNpc->m_SkillList.m_Skills[i].SkillId!=2&&pNpc->m_SkillList.m_Skills[i].SkillId!=53)
			{
				if (pNpc->m_SkillList.GetCurrentLevelByIdx(i) > 0)
					pNpc->m_SkillList.AddEnChance(i,pMagic->nValue[2]);					
			}
		}
	}
}
void KNpcAttribModify::addskilldamage9( KNpc* pNpc, void* pData )
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;

	if (pMagic->nValue[0] != 0)
	{
		int Same;
		if (pMagic->nValue[0] > 0)
			Same = pMagic->nValue[0];
		else
			Same = -pMagic->nValue[0];
		int nListIndex = pNpc->m_SkillList.FindSame(Same);
		if (nListIndex)
		{
			if (pNpc->m_SkillList.GetCurrentLevelByIdx(nListIndex) > 0)
				pNpc->m_SkillList.AddEnChance(nListIndex,pMagic->nValue[2]);		
		}
	}
	else if (pMagic->nValue[0] == 0)
	{
		for (int i = 1;i < MAX_NPCSKILL;++i)
		{
			if(pNpc->m_SkillList.m_Skills[i].SkillId&&pNpc->m_SkillList.m_Skills[i].SkillId!=1&&pNpc->m_SkillList.m_Skills[i].SkillId!=2&&pNpc->m_SkillList.m_Skills[i].SkillId!=53)
			{
				if (pNpc->m_SkillList.GetCurrentLevelByIdx(i) > 0)
					pNpc->m_SkillList.AddEnChance(i,pMagic->nValue[2]);					
			}
		}
	}
}

void KNpcAttribModify::addskilldamage8( KNpc* pNpc, void* pData )
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;

	if (pMagic->nValue[0] != 0)
	{
		int Same;
		if (pMagic->nValue[0] > 0)
			Same = pMagic->nValue[0];
		else
			Same = -pMagic->nValue[0];

		int nListIndex = pNpc->m_SkillList.FindSame(Same);
		if (nListIndex)
		{
			if (pNpc->m_SkillList.GetCurrentLevelByIdx(nListIndex) > 0)
				pNpc->m_SkillList.AddEnChance(nListIndex,pMagic->nValue[2]);		
		}
	}
	else if (pMagic->nValue[0] == 0)
	{
		for (int i = 1;i < MAX_NPCSKILL;++i)
		{
			if(pNpc->m_SkillList.m_Skills[i].SkillId&&pNpc->m_SkillList.m_Skills[i].SkillId!=1&&pNpc->m_SkillList.m_Skills[i].SkillId!=2&&pNpc->m_SkillList.m_Skills[i].SkillId!=53)
			{
				if (pNpc->m_SkillList.GetCurrentLevelByIdx(i) > 0)
					pNpc->m_SkillList.AddEnChance(i,pMagic->nValue[2]);					
			}
		}
	}
}
//对某技能加成
void KNpcAttribModify::addskilldamage6( KNpc* pNpc, void* pData )
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;

	if (pMagic->nValue[0] != 0)
	{
		int Same;
		if (pMagic->nValue[0] > 0)
			Same = pMagic->nValue[0];
		else
			Same = -pMagic->nValue[0];
		int nListIndex = pNpc->m_SkillList.FindSame(Same);
		if (nListIndex)
		{
			if (pNpc->m_SkillList.GetCurrentLevelByIdx(nListIndex) > 0)
				pNpc->m_SkillList.AddEnChance(nListIndex,pMagic->nValue[2]);		
		}
	}
	else if (pMagic->nValue[0] == 0)
	{
		for (int i = 1;i < MAX_NPCSKILL;++i)
		{
			if(pNpc->m_SkillList.m_Skills[i].SkillId&&pNpc->m_SkillList.m_Skills[i].SkillId!=1&&pNpc->m_SkillList.m_Skills[i].SkillId!=2&&pNpc->m_SkillList.m_Skills[i].SkillId!=53)
			{
				if (pNpc->m_SkillList.GetCurrentLevelByIdx(i) > 0)
					pNpc->m_SkillList.AddEnChance(i,pMagic->nValue[2]);					
			}
		}
	}
}
//对攻击技能加成 ---左键技能
void KNpcAttribModify::skill_enhance(KNpc* pNpc, void* pData)     //=//对攻击技能加成
{
	if (!pNpc->IsPlayer())
		return;
	//只对当前的 左键技能又效，不换装 换技能的时候 如何更新加成  需要处理
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;

	pNpc->m_Currentskillenhance += pMagic->nValue[0];  //加上状态数据 不叠加

	int nActiveSkillID=0,nListIndex=0;

  #ifdef _SERVER
	        nActiveSkillID= Player[pNpc->GetPlayerIdx()].GetLeftSkill();
			nListIndex    = Player[pNpc->GetPlayerIdx()].GetLeftSkillListidx();//pNpc->m_SkillList.FindSame(nActiveSkillID);
			//char Msg[64];
			//sprintf(Msg,"S:技能(%d)(%d)加成成功",nActiveSkillID,pNpc->m_Currentskillenhance);
			//Player[pNpc->GetPlayerIdx()].m_ItemList.msgshow(Msg);
  #else
            nActiveSkillID = Player[CLIENT_PLAYER_INDEX].GetLeftSkill();
			nListIndex     = Player[CLIENT_PLAYER_INDEX].GetLeftSkillListidx();
			//char Msg[64];
			//sprintf(Msg,"C:技能(%d)(%d)加成成功",nActiveSkillID,pNpc->m_Currentskillenhance);
			//Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(Msg);
  #endif

	if (pMagic->nValue[0]!= 0)
	{   
		if (nListIndex)  //   查找激活的当前攻击技能ID
		{
			if (pNpc->m_SkillList.GetCurrentLevelByIdx(nListIndex) > 0)  //等级大于0的才 增加
			{
				pNpc->m_SkillList.AddEnChance(nListIndex,pMagic->nValue[0]);						
			}			
		}
	}
}

//抵消伤害	   化解伤害
void KNpcAttribModify::dynamicmagicshieldV(KNpc* pNpc, void* pData )
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentDamageReduce += pMagic->nValue[0];

	if (pNpc->m_CurrentDamageReduce<0)
		pNpc->m_CurrentDamageReduce=0;
}

void KNpcAttribModify::addstealfeatureskill( KNpc* pNpc, void* pData )
{
	
}
//打坐生命恢复率增加
void KNpcAttribModify::lifereplenishP( KNpc* pNpc, void* pData )
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentLifeReplenish_p +=pMagic->nValue[0];

	if (pNpc->m_CurrentLifeReplenish_p<-100)
		pNpc->m_CurrentLifeReplenish_p=-100;

}
// --
//	Bo qua bua chu giam khang
// --
void KNpcAttribModify::ignoreskillP( KNpc* pNpc, void* pData )
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_IgnoreSkill += pMagic->nValue[0];	
	//g_DebugLog("[ignoreskill_p] Name: %s - Value: %d - Add: %d", pNpc->Name,  pNpc->m_IgnoreSkill, pMagic->nValue[0]);
}
//毒反弹点
void KNpcAttribModify::poisondamagereturnV( KNpc* pNpc, void* pData )
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentPoisondamagereturnV += pMagic->nValue[0];
	
}
// --
//	Phan don doc sat (percent)
// --
void KNpcAttribModify::poisondamagereturnP( KNpc* pNpc, void* pData )
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_PoisonDamageReturn += pMagic->nValue[0];
#ifdef _DEBUG
//	g_DebugLog("[poisondamagereturn_p] Name: %s - Value: %d - Add: %d", pNpc->Name,  pNpc->m_PoisonDamageReturn, pMagic->nValue[0]);
#endif
}

// --
//	Phan bua chu 
// --
void KNpcAttribModify::returnskillP( KNpc* pNpc, void* pData )
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_ReturnSkill += pMagic->nValue[0];
//	g_DebugLog("[returnskill_p] Name: %s - Value: %d - Add: %d", pNpc->Name,  pNpc->m_ReturnSkill, pMagic->nValue[0]);
}
// --
//// auto thuc thi ky nang
// --
void KNpcAttribModify::autoreplyskill(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_AutoRescueSkill.nValue[0] = pMagic->nValue[0];
	pNpc->m_AutoRescueSkill.nValue[1] = pMagic->nValue[1];
	pNpc->m_AutoRescueSkill.nValue[2] = pMagic->nValue[2];
#ifdef _DEBUG
	//g_DebugLog("[autorescueskill] Name: %s - Value: %d - Add: %d", pNpc->Name,  pNpc->m_AutoRescueSkill.nValue[2], pMagic->nValue[2]);
#endif

}
//每次释放技能的时间
void KNpcAttribModify::skill_mintimepercastV( KNpc* pNpc, void* pData )
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
    
}
// 骑马释放间隔时间
void KNpcAttribModify::mintimepercastonhorseV( KNpc* pNpc, void* pData )
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
}
// 使目标受到毒伤害的同时以<color=orange>#d1-%<color>比例损失内力，持续<color=orange>#f2-秒<color>
void KNpcAttribModify::poison2decmanaP( KNpc* pNpc, void* pData )
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	if (pNpc->m_LoseMana.nTime <= 0)
	{
		if (pMagic->nValue[1] > 0 && pMagic->nValue[0] > 0)
		{
			pNpc->m_LoseMana.nValue[0] = pMagic->nValue[0];     // 比例？
			pNpc->m_LoseMana.nTime     = pMagic->nValue[1];     // 持续时间
			pNpc->m_LoseMana.nValue[1] = pMagic->nValue[1] * 2; // 持续时间
		}
	}
}
//峨眉120 包含技能<color=water>#l1-<color>，最高等级<color=orange>#d2-级<color>
void KNpcAttribModify::skill_appendskil( KNpc* pNpc, void* pData )
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	if (pMagic->nValue[0] > 0)
	{//增加
		pNpc->m_IsMoreAura=true;
		for (int i = 0;i < 5;++i)
		{
			if (pNpc->m_TmpAuraID[i].skillid == pMagic->nValue[0]) //技能id
				break;
			if (!pNpc->m_TmpAuraID[i].skillid)
			{
				pNpc->m_TmpAuraID[i].skillid        = pMagic->nValue[0];
				pNpc->m_TmpAuraID[i].level          = pMagic->nValue[1];
				pNpc->m_TmpAuraID[i].skilllistIndex = pNpc->m_SkillList.FindSame(pNpc->m_TmpAuraID[i].skillid); 
				break;
			}
		}
	}
	else
	{//减少
		pNpc->m_IsMoreAura=false;
		for (int i = 0;i < 5;++i)
		{
			if (pNpc->m_TmpAuraID[i].skillid == -pMagic->nValue[0])
			{
				pNpc->m_TmpAuraID[i].skillid = 0;
				pNpc->m_TmpAuraID[i].skilllistIndex =0;
				pNpc->m_TmpAuraID[i].level=0;
			}
		}
	}
}

void KNpcAttribModify::hide( KNpc* pNpc, void* pData )
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	int nFrame = pMagic->nValue[1];
	if (nFrame <= 0)
			nFrame = 1;

		pNpc->m_Hide.nTime = nFrame;  //设定NPC 的时间帧数

}
//以<color=orange>#d1-%<color>的概率清除周围一定范围内友方玩家的异常状态
void KNpcAttribModify::clearnegativestate( KNpc* pNpc, void* pData )
{
	
}
//对反弹伤害的抗性 
// --
//Khang phan don sat thuong
// --
void KNpcAttribModify::returnresP( KNpc* pNpc, void* pData )
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentReturnResist += pMagic->nValue[0];
#ifdef _DEBUG
	//g_DebugLog("[returnres_p] Name: %s - Value: %d - Add: %d", pNpc->Name,  pNpc->m_CurrentReturnResist, pMagic->nValue[0]);
#endif
}

//当前抓捕BB概率 被动状态
void KNpcAttribModify::addcreatnpc_v( KNpc* pNpc, void* pData )
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentCreatnpcv += pMagic->nValue[0];
}

//当前抓捕BB概率 主动
void KNpcAttribModify::addzhuabu_v( KNpc* pNpc, void* pData )
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentCreatnpcv += pMagic->nValue[0];
}

//reduceskillcd1=减少#l1-冷却时间#f3-秒
void KNpcAttribModify::reduceskillcd1( KNpc* pNpc, void* pData )
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;

}

//reduceskillcd2=减少#l1-冷却时间#f3-秒
void KNpcAttribModify::reduceskillcd2( KNpc* pNpc, void* pData )
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	
}

//reduceskillcd3=减少#l1-冷却时间#f3-秒
void KNpcAttribModify::reduceskillcd3( KNpc* pNpc, void* pData )
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	
}

//clearallcd=#d1+%的几率清除自己所有技能冷却时间
void KNpcAttribModify::clearallcd( KNpc* pNpc, void* pData )
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;

	
}

//addblockrate=周围每#d1+个人#d3+%格挡率
void KNpcAttribModify::addblockrate( KNpc* pNpc, void* pData )
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	
}

//walkrunshadow=凌波微步
void KNpcAttribModify::walkrunshadow( KNpc* pNpc, void* pData )
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;

}

//returnskill2enemy=被敌人攻击时有#d1+%几率使敌人被技能#l3-攻击
void KNpcAttribModify::returnskill2enemy( KNpc* pNpc, void* pData )
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;

	//if (pNpc->m_Returnskill.nTime <= 0)
	{
			if (pMagic->nValue[0] > 0 && pMagic->nValue[1] > 0)
			{
				pNpc->m_Returnskill.nValue[0] = pMagic->nValue[0];     // 几率
				//pNpc->m_Returnskill.nTime     = pMagic->nValue[1];   // 总持续时间
				pNpc->m_Returnskill.nValue[1] = pMagic->nValue[1];     // 技能ID
			}
			else
			{
                pNpc->m_Returnskill.nValue[0] = 0;     // 几率
				//pNpc->m_Returnskill.nTime     = pMagic->nValue[1];   // 总持续时间
				pNpc->m_Returnskill.nValue[1] = 0;     // 技能ID
			}
	}
}

//manatoskill_enhance=满内力时对攻击技能加成：#d1+%  动态加减 所以不用即时设置
void KNpcAttribModify::manatoskill_enhance( KNpc* pNpc, void* pData )
{
	  if (!pNpc->IsPlayer())
		return;
	  KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	  pNpc->m_TempFullManaskillenhance = pNpc->m_CurrentFullManaskillenhance; //备份原来的
	  pNpc->m_CurrentFullManaskillenhance += pMagic->nValue[0];

	  int nActiveSkillID=0,nListIndex=0;	
#ifdef _SERVER
	  nActiveSkillID=Player[pNpc->GetPlayerIdx()].GetLeftSkill();
	  nListIndex    =Player[pNpc->GetPlayerIdx()].GetLeftSkillListidx();
	  //char Msg[64];
	  //sprintf(Msg,"S:满内力技能(%d)(%d)加成成功",nActiveSkillID,pNpc->m_CurrentFullManaskillenhance);
	  //Player[pNpc->GetPlayerIdx()].m_ItemList.msgshow(Msg);	
#else
	  nActiveSkillID=Player[CLIENT_PLAYER_INDEX].GetLeftSkill();
	  nListIndex    =Player[CLIENT_PLAYER_INDEX].GetLeftSkillListidx();
	  //char Msg[64];
	  //sprintf(Msg,"C:满内力技能(%d)(%d)加成成功",nActiveSkillID,pNpc->m_CurrentFullManaskillenhance);
	  //Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(Msg);
#endif	
	  if (pMagic->nValue[0]!= 0)
	  {//加成增减一次
		//int nListIndex = pNpc->m_SkillList.FindSame(nActiveSkillID);
		if (nListIndex)                 //查找激活的当前攻击技能ID
		{
			if (pNpc->m_SkillList.GetCurrentLevelByIdx(nListIndex) > 0)  //等级大于0的才 增加
			{
				pNpc->m_SkillList.AddEnChance(nListIndex,pMagic->nValue[0]);
			}			
		}
	  } 
}
//add_alldamage_v=加深伤害：#d1+点
void KNpcAttribModify::add_alldamage_v( KNpc* pNpc, void* pData )
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;

	pNpc->m_Currenadddamagev += pMagic->nValue[0];
	
}

/*
//reduceskillcd2=减少#l1-冷却时间#f3-秒  //某技能的冷却时间
//reduceskillcd3=减少#l1-冷却时间#f3-秒
//clearallcd=#d1+%的几率清除自己所有技能冷却时间
//addblockrate=周围每#d1+个人#d3+%格挡率
//walkrunshadow=凌波微步
//returnskill2enemy=被敌人攻击时有#d1+%几率使敌人被技能#l3-攻击
//manatoskill_enhance=满内力时对攻击技能加成：#d1+%
*/
//减少#l3-冷却时间:马下<color=orange>#f7-秒<color>,马上<color=orange>#f4-秒<color>
void KNpcAttribModify::decPercasttimehorse( KNpc* pNpc, void* pData )
{

	
}
//减少#l3-冷却时间:<color=orange>#f1-秒<color>
void KNpcAttribModify::decPercasttime( KNpc* pNpc, void* pData )
{
	
}

//自动触发某技能的概率
void KNpcAttribModify::enhance_autoSkill( KNpc* pNpc, void* pData )
{
	
}
//触发某技能属性的 生命恢复百分比
void KNpcAttribModify::enhance_lifeP( KNpc* pNpc, void* pData )
{
	
}
//触发某技能属性的 生命恢复百分点
void KNpcAttribModify::enhance_lifeV( KNpc* pNpc, void* pData )
{
	
}

void KNpcAttribModify::enhance_711_auto( KNpc* pNpc, void* pData )
{
	
}

void KNpcAttribModify::enhance_714_auto( KNpc* pNpc, void* pData )
{
	
}

void KNpcAttribModify::enhance_717_auto( KNpc* pNpc, void* pData )
{
	
}

void KNpcAttribModify::enhance_723_missP( KNpc* pNpc, void* pData )
{
	
}
//杀死敌人获得经验增加
void KNpcAttribModify::expenhanceP( KNpc* pNpc, void* pData )
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;

	if (pNpc->IsPlayer())
	{
		pNpc->m_CurrentUpExp +=pMagic->nValue[0];
	}
	//else
	//pNpc->m_CurrentUpExp +=pMagic->nValue[0];
}
void KNpcAttribModify::SerisesDamage( KNpc* pNpc, void* pData ) //五行伤害
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentSerisesEnhance += pMagic->nValue[0];
}
//档格
void KNpcAttribModify::block_rate( KNpc* pNpc, void* pData )  //免受伤害
{
    KMagicAttrib* pMagic = (KMagicAttrib *)pData;	
    pNpc->m_CurrentdanggeRate += pMagic->nValue[0];
	if (pNpc->m_CurrentdanggeRate<0)
		pNpc->m_CurrentdanggeRate=0;
}
//重击
void KNpcAttribModify::enhancehit_rate( KNpc* pNpc, void* pData )//打出最大攻击力
{
	
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
    pNpc->m_CurrentzhongjiRate += pMagic->nValue[0];
	
	if (pNpc->m_CurrentzhongjiRate<0)
		pNpc->m_CurrentzhongjiRate=0;

}

/*
  int                 m_CurrentcjdanggeRate;                 //拆解档格
  int                 m_CurrentcjzhongjiRate;                //拆解重击
  int                 m_Currentsorbdamage;                   //抵消伤害
  int                 m_Currentpoisonres;                    //=忽略对方毒防:#d1-%
  int                 m_Currentfireres;                      //=忽略对方火防:#d1-%
  int                 m_Currentlightingres;                  //=忽略对方雷防:#d1-%
  int                 m_Currentphysicsres;                   //=忽略对方普防:#d1-%
  int                 m_Currentcoldres;                      //=忽略对方冰防:#d1-%
  int                 m_Currentnopkvalue;                    //=不增加PK值概率:#d1+%
  int                 m_Currentbossdamage;                   //=对黄金boss攻击伤害<color=orange>#d1+%<color>
  int                 m_Currentelementsenhance;              //=五行强化值：#d1-点。强化对相克五行的克制效果
  int                 m_Currentelementsresist;               //=五行弱化值：#d1-点。弱化受相克五行的克制效
*/
void KNpcAttribModify::anti_block_rate(KNpc* pNpc, void* pData)           //=拆解格挡概率：#d1+%
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
    pNpc->m_CurrentcjdanggeRate += pMagic->nValue[0];
	if (pNpc->m_CurrentcjdanggeRate<0)
		pNpc->m_CurrentcjdanggeRate=0;
}
void KNpcAttribModify::anti_enhancehit_rate(KNpc* pNpc, void* pData)      //=抵挡重击概率：#d1+%
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
    pNpc->m_CurrentcjzhongjiRate += pMagic->nValue[0];
	
	if (pNpc->m_CurrentcjzhongjiRate<0)
		pNpc->m_CurrentcjzhongjiRate=0;
}
void KNpcAttribModify::sorbdamage_p(KNpc* pNpc, void* pData)              //=抵消伤害:#d1+%
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
    pNpc->m_Currentsorbdamage += pMagic->nValue[0];		
}

void KNpcAttribModify::sorbdamage_v(KNpc* pNpc, void* pData)              //=抵消伤害:#d1+点
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
    pNpc->m_Currentsorbdamage_v += pMagic->nValue[0];		
}


void KNpcAttribModify::anti_poisonres_p(KNpc* pNpc, void* pData)          //=忽略对方毒防:#d1-%
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
    pNpc->m_Currentpoisonres += pMagic->nValue[0];		
}
void KNpcAttribModify::anti_fireres_p(KNpc* pNpc, void* pData)            //=忽略对方火防:#d1-%
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
    pNpc->m_Currentfireres += pMagic->nValue[0];		
}

void KNpcAttribModify::anti_lightingres_p(KNpc* pNpc, void* pData)        //=忽略对方雷防:#d1-%
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
    pNpc->m_Currentlightingres += pMagic->nValue[0];		
}

void KNpcAttribModify::anti_physicsres_p(KNpc* pNpc, void* pData)         //=忽略对方普防:#d1-%
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
    pNpc->m_Currentphysicsres += pMagic->nValue[0];		
}

void KNpcAttribModify::anti_coldres_p(KNpc* pNpc, void* pData)            //=忽略对方冰防:#d1-%
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
    pNpc->m_Currentcoldres += pMagic->nValue[0];		
}

void KNpcAttribModify::not_add_pkvalue_p(KNpc* pNpc, void* pData)         //=不增加PK值概率:#d1+%
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
    pNpc->m_Currentnopkvalue += pMagic->nValue[0];		
}

void KNpcAttribModify::add_boss_damage(KNpc* pNpc, void* pData)           //=对黄金boss攻击伤害<color=orange>#d1+%<color>
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
    pNpc->m_Currentbossdamage += pMagic->nValue[0];		
}

void KNpcAttribModify::five_elements_enhance_v(KNpc* pNpc, void* pData)   //=五行强化值：#d1-点。强化对相克五行的克制效果
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
    pNpc->m_Currentelementsenhance += pMagic->nValue[0];		
}

void KNpcAttribModify::five_elements_resist_v(KNpc* pNpc, void* pData)     //=五行弱化值：#d1-点。弱化对相克五行的克制效果
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
    pNpc->m_Currentelementsresist += pMagic->nValue[0];		
}
//加强伤害
void KNpcAttribModify::add_alldamage_p(KNpc* pNpc, void* pData)
{	
		
}
//自动复活
void KNpcAttribModify::auto_Revive_rate(KNpc* pNpc, void* pData)
{	
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	 pNpc->m_CurrentautoReviverate += pMagic->nValue[0];		
}
//忽略对方全部防御
void KNpcAttribModify::anti_allres_p(KNpc* pNpc, void* pData)
{	
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
     pNpc->m_Currentallres += pMagic->nValue[0];		
}

//五毒120 自动释放的技能
// --
//	Tu dong thuc thi ky nang
// --
void KNpcAttribModify::autoskill( KNpc* pNpc, void* pData )
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_AutoAttackSkill.nValue[0] = pMagic->nValue[0];
	pNpc->m_AutoAttackSkill.nValue[1] = pMagic->nValue[1];
	pNpc->m_AutoAttackSkill.nValue[2] = pMagic->nValue[2];
#ifdef _DEBUG
//	g_DebugLog("[autoattackskill] Name: %s - Value: %d - Add: %d", pNpc->Name,  pNpc->m_AutoAttackSkill.nValue[2], pMagic->nValue[2]);
#endif
}

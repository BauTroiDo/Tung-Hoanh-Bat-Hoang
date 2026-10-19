#include "KEngine.h"
#include "KCore.h"
#ifndef _SERVER
#include "../../../Headers/IClient.h"
#include "CoreShell.h"
#include "KViewItem.h"
#include "KSellItem.h"
#include "KMagicDesc.h"
#endif
#include "KObjSet.h"
#include "KNpcSet.h"
#include "KPlayer.h"
#include "KPlayerSet.h"
#include "KPlayerTeam.h"
#include "KPlayerFaction.h"
#include "KNpc.h"
#include "KSubWorld.h"
#include "LuaFuns.h"
#include "KProtocolProcess.h"
#include "KSkills.h"
#include "KItemSet.h"
#include "KBuySell.h"
#include "KSubWorldSet.h"
#include "KTabFile.h"   //后来加的
//#include "MyAssert.h"
#include "Scene/ObstacleDef.h"
#include "KMath.h"
#ifndef _SERVER
#include "Scene\KScenePlaceC.h"
#include "Scene\Kpathfinder.h"
#endif
#include "KTongProtocol.h"
#include "KLadder.h"
#include "../../Engine/src/Text.h"
#include "../../Engine/src/KFilePath.h"

#include "KNpcTemplate.h"
//#define WAIGUA_ZROC
KSubWorldSet   *M_SubWorldSetB;
//KPlayerSet     *G_PlayerSet;
const char * g_MagicID2String(int nAttrib);

KProtocolProcess g_ProtocolProcess;
KProtocolProcess::KProtocolProcess()
{
	ZeroMemory(ProcessFunc, sizeof(ProcessFunc));
#ifndef _SERVER
	ProcessFunc[s2c_login] = NULL;
	ProcessFunc[s2c_logout] = NULL;
	ProcessFunc[s2c_syncclientend] =&KProtocolProcess::SyncClientEnd;
	ProcessFunc[s2c_synonlineplayer] =&KProtocolProcess::SyncCurPlayer;
	ProcessFunc[s2c_synccurplayerskill] =&KProtocolProcess::s2cSyncAllSkill;
	ProcessFunc[s2c_synccurplayernormal] =&KProtocolProcess::SyncCurNormalData;
	ProcessFunc[s2c_newplayer] = NULL;
	ProcessFunc[s2c_removeplayer] = NULL;
	ProcessFunc[s2c_syncworld] =&KProtocolProcess::SyncWorld;
	ProcessFunc[s2c_syncplayer] =&KProtocolProcess::SyncPlayer;  //一对一播报 s2c_syncplayer
	ProcessFunc[s2c_syncplayermin] =&KProtocolProcess::SyncPlayerMin;//小范围播报
	ProcessFunc[s2c_syncnpc] =&KProtocolProcess::SyncNpc;
	ProcessFunc[s2c_syncnpcmin] =&KProtocolProcess::SyncNpcMin;
	ProcessFunc[s2c_syncnpcminplayer] =&KProtocolProcess::SyncNpcMinPlayer;
	ProcessFunc[s2c_objadd] =&KProtocolProcess::SyncObjectAdd;
	ProcessFunc[s2c_syncobjstate] =&KProtocolProcess::SyncObjectState;
	ProcessFunc[s2c_syncobjdir] =&KProtocolProcess::SyncObjectDir;
	ProcessFunc[s2c_objremove] =&KProtocolProcess::SyncObjectRemove;
	ProcessFunc[s2c_objTrapAct] =&KProtocolProcess::SyncObjectTrap;
	ProcessFunc[s2c_npcremove] =&KProtocolProcess::NetCommandRemoveNpc;
	ProcessFunc[s2c_npcwalk] =&KProtocolProcess::NetCommandWalk;
	ProcessFunc[s2c_npcrun] =&KProtocolProcess::NetCommandRun;
	ProcessFunc[s2c_npcattack] = NULL;
	ProcessFunc[s2c_npcmagic] = NULL;
	ProcessFunc[s2c_npcjump] =&KProtocolProcess::NetCommandJump;
	ProcessFunc[s2c_npctalk] = &KProtocolProcess::NetCommandChat;
	ProcessFunc[s2c_npchurt] =&KProtocolProcess::NetCommandHurt;
	ProcessFunc[s2c_npcdeath] =&KProtocolProcess::NetCommandDeath;
	ProcessFunc[s2c_npcchgcurcamp] =&KProtocolProcess::NetCommandChgCurCamp;
	ProcessFunc[s2c_npcchgcamp] =&KProtocolProcess::NetCommandChgCamp;
	ProcessFunc[s2c_skillcast] =&KProtocolProcess::NetCommandSkill;
	ProcessFunc[s2c_playertalk] = NULL;
	ProcessFunc[s2c_playerexp] =&KProtocolProcess::s2cPlayerExp;
	ProcessFunc[s2c_teaminfo] =&KProtocolProcess::s2cShowTeamInfo;
	ProcessFunc[s2c_teamselfinfo] =&KProtocolProcess::s2cUpdataSelfTeamInfo;
	ProcessFunc[s2c_teamapplyinfofalse] =&KProtocolProcess::s2cApplyTeamInfoFalse;
	ProcessFunc[s2c_teamcreatesuccess] =&KProtocolProcess::s2cCreateTeam;
	ProcessFunc[s2c_teamcreatefalse] =&KProtocolProcess::s2cApplyCreateTeamFalse;
	ProcessFunc[s2c_teamopenclose] =&KProtocolProcess::s2cSetTeamState;
	ProcessFunc[s2c_teamgetapply] =&KProtocolProcess::s2cApplyAddTeam;
	ProcessFunc[s2c_teamaddmember] =&KProtocolProcess::s2cTeamAddMember;
	ProcessFunc[s2c_teamleave] =&KProtocolProcess::s2cLeaveTeam;
	ProcessFunc[s2c_teamchangecaptain] =&KProtocolProcess::s2cTeamChangeCaptain;
	ProcessFunc[s2c_playerfactiondata] =&KProtocolProcess::s2cSetFactionData;
	ProcessFunc[s2c_playerleavefaction] =&KProtocolProcess::s2cLeaveFaction;
	ProcessFunc[s2c_playerfactionskilllevel] =&KProtocolProcess::s2cFactionSkillOpen;
	ProcessFunc[s2c_playersendchat] =&KProtocolProcess::s2cGetChat;
	ProcessFunc[s2c_playersyncleadexp] =&KProtocolProcess::s2cGetLeadExp;
	ProcessFunc[s2c_playerlevelup] =&KProtocolProcess::s2cLevelUp;
	ProcessFunc[s2c_teammatelevel] =&KProtocolProcess::s2cGetTeammateLevel;
	ProcessFunc[s2c_playersyncattribute] =&KProtocolProcess::s2cGetCurAttribute;
	ProcessFunc[s2c_playerskilllevel] =&KProtocolProcess::s2cGetSkillLevel;
	ProcessFunc[s2c_syncitem] =&KProtocolProcess::s2cSyncItem;
	ProcessFunc[s2c_removeitem] =&KProtocolProcess::s2cRemoveItem;
	ProcessFunc[s2c_syncmonfs] =&KProtocolProcess::s2cSyncMoney;
	ProcessFunc[s2c_syncqianfs] =&KProtocolProcess::s2cSyncXu;//xu
	ProcessFunc[s2c_playermoveitem] =&KProtocolProcess::s2cMoveItem;
	ProcessFunc[s2c_scriptaction] =&KProtocolProcess::SyncScriptAction;
	ProcessFunc[s2c_chatapplyaddfriend] =&KProtocolProcess::s2cChatGetApplyAddFriend;
	ProcessFunc[s2c_chataddfriend] =&KProtocolProcess::s2cChatAddFriend;
	ProcessFunc[s2c_chatrefusefriend] =&KProtocolProcess::s2cChatBeRefusedAddFriend;
	ProcessFunc[s2c_chataddfriendfail] =&KProtocolProcess::s2cChatAddFriendFail;
	ProcessFunc[s2c_chatloginfriendnoname] =&KProtocolProcess::s2cChatLoginFriendNoName;
	ProcessFunc[s2c_chatloginfriendname] =&KProtocolProcess::s2cChatLoginFriendName;
	ProcessFunc[s2c_chatonefrienddata] =&KProtocolProcess::s2cChatAddOneFriend;
	ProcessFunc[s2c_chatfriendonline] =&KProtocolProcess::s2cChatFriendOnline;
	ProcessFunc[s2c_chatdeletefriend] =&KProtocolProcess::s2cChatDeleteFriend;
	ProcessFunc[s2c_chatfriendoffline] =&KProtocolProcess::s2cChatFriendOffLine;
	ProcessFunc[s2c_syncrolelist] =&KProtocolProcess::s2cSyncRoleList;
	ProcessFunc[s2c_tradechangestate] =&KProtocolProcess::s2cTradeChangeState;
	ProcessFunc[s2c_npcsetmenustate] =&KProtocolProcess::s2cNpcSetMenuState;
	ProcessFunc[s2c_trademoneysync] =&KProtocolProcess::s2cTradeMoneySync;
	ProcessFunc[s2c_tradedecision] =&KProtocolProcess::s2cTradeDecision;
	ProcessFunc[s2c_chatscreensingleerror] =&KProtocolProcess::s2cChatScreenSingleError;
	ProcessFunc[s2c_syncnpcstate] =&KProtocolProcess::s2cUpdateNpcState;
	ProcessFunc[s2c_teaminviteadd] =&KProtocolProcess::s2cTeamInviteAdd;
	ProcessFunc[s2c_tradepressoksync] =&KProtocolProcess::s2cTradePressOkSync;
	ProcessFunc[s2c_ping] =&KProtocolProcess::s2cPing;
	ProcessFunc[s2c_npcsit] =&KProtocolProcess::NetCommandSit;
	ProcessFunc[s2c_npcstand] = NetCommandStand;
	ProcessFunc[s2c_opensalebox] =&KProtocolProcess::OpenSaleBox;
	ProcessFunc[s2c_castskilldirectly] =&KProtocolProcess::s2cDirectlyCastSkill;
	ProcessFunc[s2c_msgshow] =&KProtocolProcess::s2cShowMsg;
	ProcessFunc[s2c_syncstateeffect] =&KProtocolProcess::SyncStateEffect;
	ProcessFunc[s2c_openstorebox] =&KProtocolProcess::OpenStoreBox;
	ProcessFunc[s2c_openresetpass] =&KProtocolProcess::OpenResetPass;
	ProcessFunc[s2c_openstringbox] =&KProtocolProcess::OpenStringUI; //string
	ProcessFunc[s2c_opendataubox] =&KProtocolProcess::GiftDT;
	ProcessFunc[s2c_playerrevive] =&KProtocolProcess::PlayerRevive;
	ProcessFunc[s2c_requestnpcfail] =&KProtocolProcess::RequestNpcFail;
	ProcessFunc[s2c_tradeapplystart] =&KProtocolProcess::s2cTradeApplyStart;
	ProcessFunc[s2c_rolenewdelresponse] =NULL;
	ProcessFunc[s2c_ItemAutoMove] =&KProtocolProcess::s2cItemAutoMove;
	ProcessFunc[s2c_itemexchangefinish] =&KProtocolProcess::FinishedItemExchange;
	ProcessFunc[s2c_changeweather] =&KProtocolProcess::s2cChangeWeather;
	ProcessFunc[s2c_pksyncnormalflag] =&KProtocolProcess::s2cPKSyncNormalFlag;
	ProcessFunc[s2c_pksyncenmitystate] =&KProtocolProcess::s2cPKSyncEnmityState;
	ProcessFunc[s2c_pksyncexercisestate] =&KProtocolProcess::s2cPKSyncExerciseState;
	ProcessFunc[s2c_pksyncpkvalue] =&KProtocolProcess::s2cPKValueSync;
	ProcessFunc[s2c_reputesyncreputevalue] =&KProtocolProcess::s2cReputeValueSync;
	ProcessFunc[s2c_fuyuansyncfuyuanvalue] =&KProtocolProcess::s2cFuYuanValueSync;
	ProcessFunc[s2c_rebornsyncrebornvalue] =&KProtocolProcess::s2cReBornValueSync;
	ProcessFunc[s2c_openmarketbox] =&KProtocolProcess::OpenMarketBox;
	ProcessFunc[s2c_npcsleepmode] =&KProtocolProcess::NpcSleepSync;
	ProcessFunc[s2c_viewequip] =&KProtocolProcess::s2cViewEquip;
	ProcessFunc[s2c_ladderresult] =&KProtocolProcess::LadderResult;
	ProcessFunc[s2c_ladderlist] =&KProtocolProcess::LadderList;
	ProcessFunc[s2c_tongcreate] =&KProtocolProcess::s2cTongCreate;
	ProcessFunc[s2c_replyclientping] =&KProtocolProcess::ServerReplyClientPing;
	ProcessFunc[s2c_npcgoldchange] =&KProtocolProcess::s2cNpcGoldChange;
	ProcessFunc[s2c_itemdurabilitychange] =&KProtocolProcess::ItemChangeDurability;
	ProcessFunc[s2c_opentremble] =&KProtocolProcess::OpenTremble;
	ProcessFunc[s2c_rankname] =&KProtocolProcess::NetCommandSetRankFF;
	ProcessFunc[s2c_viewsellitem] =&KProtocolProcess::s2cViewSellItem;
	ProcessFunc[s2c_viewupdateitem] =&KProtocolProcess::s2cViewUpdateItem;
	ProcessFunc[s2c_playergetcount] =&KProtocolProcess::s2cGetCouunt;
	ProcessFunc[s2c_shopname] =&KProtocolProcess::s2cShopName;
	ProcessFunc[s2c_syncmasklock] =&KProtocolProcess::SyncMaskLock;	// mat na
	ProcessFunc[s2c_opengive] =&KProtocolProcess::SyncGive;
	ProcessFunc[s2c_npcsecmove] =&KProtocolProcess::SyncSecMov;
	ProcessFunc[s2c_pkvalue] =&KProtocolProcess::SyncPKValue;
	ProcessFunc[s2c_viewlianitem]=&KProtocolProcess::SyncViewlianitem; //链接
	ProcessFunc[s2c_openwengang] =&KProtocolProcess::OpenWenGang;
	ProcessFunc[s2c_openstone] =&KProtocolProcess::Openstone;
	ProcessFunc[s2c_updateviewequip] =&KProtocolProcess::s2cViewUpdateEquip;
	ProcessFunc[s2c_gettongback] =&KProtocolProcess::s2cgettongback;
	ProcessFunc[s2c_syncequipitem] =&KProtocolProcess::s2csyncequipitem;
	ProcessFunc[s2c_taskcallback] =&KProtocolProcess::s2ctaskcallback;
	ProcessFunc[s2c_openjindutiao] =&KProtocolProcess::s2copenjindutiao;
	ProcessFunc[s2c_syncitemposition] =&KProtocolProcess::s2csyncitemposition;
	ProcessFunc[s2c_openduanzhao] =&KProtocolProcess::s2copenduanzhao;	//锻造蓝装
	ProcessFunc[s2c_syncurupdata] =&KProtocolProcess::s2csyncurupdata;	//同步当前数据
	ProcessFunc[s2c_JinMaicallback] =&KProtocolProcess::s2cJinMaicallback;	//同步当前数据	
    ProcessFunc[s2c_openvipdazao] =&KProtocolProcess::s2copenvipdazao;
	ProcessFunc[s2c_synnpcinfocallback] =&KProtocolProcess::s2csynnpcinfocallback;
	ProcessFunc[s2c_pautoMovecallback] =&KProtocolProcess::s2cpautoMovecallback;
	ProcessFunc[s2c_plyerguzicallback] =&KProtocolProcess::s2cplyerguzicallback;
	ProcessFunc[s2c_syncplayermap] =&KProtocolProcess::s2csyncplayermap;
	ProcessFunc[s2c_synconestate] =&KProtocolProcess::s2csynconeclearstate;
	ProcessFunc[s2c_syncnodataeffect] =&KProtocolProcess::s2csyncnodataeffect;
	ProcessFunc[s2c_npcsetpos] =&KProtocolProcess::s2cnpcsetpos;
	ProcessFunc[s2c_playermissiondata] =&KProtocolProcess::s2cplayermissiondata;
	ProcessFunc[s2c_syncrankdata] =&KProtocolProcess::s2csyncrankdata;
	ProcessFunc[s2c_syncsupershop] = &KProtocolProcess::s2cSyncSuperShop;
	ProcessFunc[s2c_syncitem_shop] = &KProtocolProcess::s2csyncitemshop;
	ProcessFunc[s2c_itemdata_shop] = &KProtocolProcess::s2citemdatashop;
	ProcessFunc[s2c_kickoutgame] = &KProtocolProcess::s2ckickoutgame;
	ProcessFunc[s2c_objadd_vn] = &KProtocolProcess::s2cobjadd_vn;
	ProcessFunc[s2c_rolesyncvipvalue] = &KProtocolProcess::s2crolesyncvipvalue;
	//ProcessFunc[s2c_rolecheckname_result] =NULL;
	ProcessFunc[s2c_extend] =&KProtocolProcess::s2cExtend;
	ProcessFunc[s2c_extendchat] =&KProtocolProcess::s2cExtendChat;
	ProcessFunc[s2c_extendfriend] =&KProtocolProcess::s2cExtendFriend;
	ProcessFunc[s2c_extendtong] =&KProtocolProcess::s2cExtendTong;
#else
	ProcessFunc[c2s_login_fhgyrinhkmvnsheypo] = NULL;
	ProcessFunc[c2s__loginfs_kfjghtueodnchsf] = NULL;
	ProcessFunc[c2s_syncclientend] = NULL;
	ProcessFunc[c2s_loadplayer] = NULL;
	ProcessFunc[c2s_newplayer] = NULL;
	ProcessFunc[c2s_removeplayer] = &KProtocolProcess::RemoveRole;
	ProcessFunc[c2s_requestworld] = NULL;
	ProcessFunc[c2s_requestplayer] = NULL;
	ProcessFunc[c2s_requestnpc] = &KProtocolProcess::NpcRequestCommand;
	ProcessFunc[c2s_requestobj] = &KProtocolProcess::ObjRequestCommand;
	ProcessFunc[c2s_npcwalk] = &KProtocolProcess::NpcWalkCommand;
	ProcessFunc[c2s_npcrun] = &KProtocolProcess::NpcRunCommand;
	ProcessFunc[c2s_npcskill] = &KProtocolProcess::NpcSkillCommand;
	ProcessFunc[c2s_npcjump] = &KProtocolProcess::NpcJumpCommand;
	ProcessFunc[c2s_npctalk] = &KProtocolProcess::NpcTalkCommand;
	ProcessFunc[c2s_npchurt] = NULL;
	ProcessFunc[c2s_npcdeath] = NULL;
	ProcessFunc[c2s_playertalk] = &KProtocolProcess::PlayerTalkCommand;
	ProcessFunc[c2s_teamapplyinfo] = &KProtocolProcess::PlayerApplyTeamInfo;
	ProcessFunc[c2s_teamapplycreate] = &KProtocolProcess::PlayerApplyCreateTeam;
	ProcessFunc[c2s_teamapplyopenclose] = &KProtocolProcess::PlayerApplyTeamOpenClose;
	ProcessFunc[c2s_teamapplyadd] = &KProtocolProcess::PlayerApplyAddTeam;
	ProcessFunc[c2s_teamacceptmember] = &KProtocolProcess::PlayerAcceptTeamMember;
	ProcessFunc[c2s_teamapplyleave] = &KProtocolProcess::PlayerApplyLeaveTeam;
	ProcessFunc[c2s_teamapplykickmember] = &KProtocolProcess::PlayerApplyTeamKickMember;
	ProcessFunc[c2s_teamapplychangecaptain] = &KProtocolProcess::PlayerApplyTeamChangeCaptain;
	ProcessFunc[c2s_teamapplydismiss] = &KProtocolProcess::PlayerApplyTeamDismiss;
	ProcessFunc[c2s_playerapplysetpk] = &KProtocolProcess::PlayerApplySetPK;
	ProcessFunc[c2s_playerapplyfactiondata] = &KProtocolProcess::PlayerApplyFactionData;
	ProcessFunc[c2s_playersendchat] = &KProtocolProcess::PlayerSendChat;
	ProcessFunc[c2s_playeraddbaseattribute] = &KProtocolProcess::PlayerAddBaseAttribute;  //增加潜能点
	ProcessFunc[c2s_playerapplyaddskillpoint] = &KProtocolProcess::PlayerApplyAddSkillPoint;//增加技能点
	ProcessFunc[c2s_playereatitem] = &KProtocolProcess::PlayerEatItem;
	ProcessFunc[c2s_playerpickupitem] = &KProtocolProcess::PlayerPickUpItem;
	ProcessFunc[c2s_playermoveitem] = &KProtocolProcess::PlayerMoveItem;
	ProcessFunc[c2s_playersellitem] = &KProtocolProcess::PlayerSellItem;
	ProcessFunc[c2s_playerbuyitem] = &KProtocolProcess::PlayerBuyItem;
	ProcessFunc[c2s_playerthrowawayitem] = &KProtocolProcess::PlayerDropItem;
	ProcessFunc[c2s_playerselui] = &KProtocolProcess::PlayerSelUI;
	ProcessFunc[c2s_chatsetchannel] = &KProtocolProcess::ChatSetChannel;
	ProcessFunc[c2s_chatapplyaddfriend] = &KProtocolProcess::ChatApplyAddFriend;
	ProcessFunc[c2s_chataddfriend] = &KProtocolProcess::ChatAddFriend;
	ProcessFunc[c2s_chatrefusefriend] = &KProtocolProcess::ChatRefuseFriend;
	ProcessFunc[c2s_dbplayerselect] = NULL;
	ProcessFunc[c2s_chatapplyresendallfriendname] = &KProtocolProcess::ChatApplyReSendAllFriendName;
	ProcessFunc[c2s_chatapplysendonefriendname] = &KProtocolProcess::ChatApplySendOneFriendName;
	ProcessFunc[c2s_chatdeletefriend] = &KProtocolProcess::ChatDeleteFriend;
	ProcessFunc[c2s_chatredeletefriend] = &KProtocolProcess::ChatReDeleteFriend;
	ProcessFunc[c2s_tradeapplystateopen] = &KProtocolProcess::TradeApplyOpen;
	ProcessFunc[c2s_tradeapplystateclose] = &KProtocolProcess::TradeApplyClose;
	ProcessFunc[c2s_tradeapplystart] = &KProtocolProcess::TradeApplyStart;
	ProcessFunc[c2s_trademovemoney] = &KProtocolProcess::TradeMoveMoney;
	ProcessFunc[c2s_tradedecision] = &KProtocolProcess::TradeDecision;
	ProcessFunc[c2s_dialognpc]	= &KProtocolProcess::DialogNpc;
	ProcessFunc[c2s_teaminviteadd]	= &KProtocolProcess::TeamInviteAdd;
	ProcessFunc[c2s_changeauraskill] = &KProtocolProcess::ChangeAuraSkill;
	ProcessFunc[c2s_teamreplyinvite] = &KProtocolProcess::TeamReplyInvite;
	ProcessFunc[c2s_ping] = NULL;//ReplyPing;
	ProcessFunc[c2s_cpunlock] = &KProtocolProcess::NpcCPUnlockCommand;
	ProcessFunc[c2s_cplock] = &KProtocolProcess::NpcCPLockCommand;
	ProcessFunc[c2s_cpchange] = &KProtocolProcess::NpcCPChangeCommand;
	ProcessFunc[c2s_cpreset] = &KProtocolProcess::NpcCPResetCommand;
	ProcessFunc[c2s_market]=&KProtocolProcess::NpcOpenMarketCommand;//商城 奇珍阁
	ProcessFunc[c2s_string]=&KProtocolProcess::NpcStringCommand;
	ProcessFunc[c2s_datau]=&KProtocolProcess::NpcDaTauCommand;
	ProcessFunc[c2s_npcsit] = &KProtocolProcess::NpcSitCommand;
	ProcessFunc[c2s_objmouseclick] = &KProtocolProcess::ObjMouseClick;
	ProcessFunc[c2s_storemoney] = &KProtocolProcess::StoreMoneyCommand;
	ProcessFunc[c2s_playerrevive] = &KProtocolProcess::NpcReviveCommand;
	ProcessFunc[c2s_tradereplystart] = &KProtocolProcess::c2sTradeReplyStart;
	ProcessFunc[c2s_pkapplychangenormalflag] = &KProtocolProcess::c2sPKApplyChangeNormalFlag;
	ProcessFunc[c2s_pkapplyenmity] = &KProtocolProcess::c2sPKApplyEnmity;
	ProcessFunc[c2s_viewequip] = &KProtocolProcess::c2sViewEquip;
	ProcessFunc[c2s_ladderquery] = &KProtocolProcess::LadderQuery;
	ProcessFunc[c2s_repairitem] = &KProtocolProcess::ItemRepair;
	ProcessFunc[c2s_npcride] = &KProtocolProcess::NpcRideCommand;
	ProcessFunc[c2s_breakitem] = &KProtocolProcess::ItemBreak;
	ProcessFunc[c2s_playercomp] = &KProtocolProcess::c2sCompItem;
	ProcessFunc[c2s_playersetprice] = &KProtocolProcess::SetPrice;
	ProcessFunc[c2s_playerstarttrade] = &KProtocolProcess::StartTrade;
	ProcessFunc[c2s_viewitem] = &KProtocolProcess::c2sViewItem;
	ProcessFunc[c2s_playertradebuyitem] = &KProtocolProcess::c2sTradeBuy;
	ProcessFunc[c2s_updateviewitem] = &KProtocolProcess::c2supdateitem;
	ProcessFunc[c2s_playerneedcount] = &KProtocolProcess::c2sNeedCount;
	ProcessFunc[c2s_playershopname] = &KProtocolProcess::c2sNeedShopName;
	ProcessFunc[c2s_playersysshop] = &KProtocolProcess::c2sSysShop;
	ProcessFunc[c2s_playerlixian] = &KProtocolProcess::c2sLiXian;
	ProcessFunc[c2s_playergiveback] = &KProtocolProcess::c2sGiveBack;
	ProcessFunc[c2s_pkvalue]=&KProtocolProcess::NpcPKValueCommand;
    ProcessFunc[c2s_autoplay]=&KProtocolProcess::NpcAutoplayCommand;//挂机
	ProcessFunc[c2s_npcsunyi]= &KProtocolProcess::NpcSunyiCommand; //瞬移
	ProcessFunc[c2s_guzi]=&KProtocolProcess::c2sguzi;
	ProcessFunc[c2s_gufarm]=&KProtocolProcess::c2sgufarm;
	ProcessFunc[c2s_addjinbi]=&KProtocolProcess::c2saddjinbi;
	ProcessFunc[c2s_lianjie]=&KProtocolProcess::c2slianjie;
	ProcessFunc[c2s_fsjingmai]=&KProtocolProcess::c2sfsjingmai;
	ProcessFunc[c2s_guaforbit]=&KProtocolProcess::c2sguaforbit;
	ProcessFunc[c2s_skillsync]=&KProtocolProcess::c2sskillsync;
	ProcessFunc[c2s_playerrong]=&KProtocolProcess::c2sRongItem;
	ProcessFunc[c2s_playeradditem] =&KProtocolProcess::c2splayeradditem;  //客户端增减物品 没有用到
	ProcessFunc[c2s_playerAutoMoveitem] = &KProtocolProcess::c2splayerAutoMoveitem; //客户端要求自动移动物品
	ProcessFunc[c2s_playerdelitem] =&KProtocolProcess::c2splayerdelitem;
	ProcessFunc[c2s_playersetLianFlg] =&KProtocolProcess::c2splayersetLianFlg;
	ProcessFunc[c2s_updateviewequip] =&KProtocolProcess::c2supdateviewequip;
	ProcessFunc[c2s_playertongset] =&KProtocolProcess::c2splayertongset;
	ProcessFunc[c2s_playerdoScript] =&KProtocolProcess::c2splayerdoScript;
	ProcessFunc[c2s_gettaskval] =&KProtocolProcess::c2sgettaskval;	
    ProcessFunc[c2s_jinducallback] =&KProtocolProcess::c2sjinducallback;
	ProcessFunc[c2s_syncurdata] =&KProtocolProcess::c2ssyncurdata;
    ProcessFunc[c2s_vipdoScript] =&KProtocolProcess::c2svipdoScript;
	ProcessFunc[c2s_roomisfull] =&KProtocolProcess::c2sroomisfull;
	ProcessFunc[c2s_synnpcinfo] =&KProtocolProcess::c2ssynnpcinfo;
	ProcessFunc[c2s_playerautoequip] =&KProtocolProcess::c2splayerautoequip;
	ProcessFunc[c2s_playerguzgame] =&KProtocolProcess::c2splayerguzgame;
	ProcessFunc[c2s_playersetpos] =&KProtocolProcess::c2splayersetpos;
	ProcessFunc[c2s_synshopidx] =&KProtocolProcess::c2ssynshopidx;
	ProcessFunc[c2s_settaskval] =&KProtocolProcess::c2ssettaskval;
	ProcessFunc[c2s_runscript] =&KProtocolProcess::c2srunscript;
	ProcessFunc[c2s_player_req] =&KProtocolProcess::c2splayerreq;
	ProcessFunc[c2s_lockitem] =&KProtocolProcess::c2slockitem;
	ProcessFunc[c2s_gmcommand] =&KProtocolProcess::GMCommand;
	ProcessFunc[c2s_findpath_state] =&KProtocolProcess::c2sfindpathstate;
	ProcessFunc[c2s_create_ref_item] = &KProtocolProcess::c2screaterefitem;
	ProcessFunc[c2s_remove_ref_item] = &KProtocolProcess::c2sremoverefitem;
	ProcessFunc[c2s_apply_mask_lock] = &KProtocolProcess::c2sapplymasklock;
	ProcessFunc[c2s_player_combine_item] = &KProtocolProcess::c2sGopItem;
	ProcessFunc[c2s_playercommand] = &KProtocolProcess::PlayerCommand;
	
#endif
}

KProtocolProcess::~KProtocolProcess()
{
}

#ifndef _SERVER  //客户端端接受服务器发来的网络协议包
void KProtocolProcess::ProcessNetMsg(BYTE* pMsg)
{
	if (!pMsg || pMsg[0] <= s2c_clientbegin || pMsg[0] >= s2c_end || ProcessFunc[pMsg[0]] == NULL)
	{
		return;
	}
//	if (ProcessFunc[pMsg[0]])
	 (this->*ProcessFunc[pMsg[0]])(pMsg);
}
#else  //服务器端接受客户端发来的网络协议包
void KProtocolProcess::ProcessNetMsg(int nIndex, BYTE* pMsg)
{
	_ASSERT(pMsg && pMsg[0] > c2s_gameserverbegin && pMsg[0] < c2s_end);

	BYTE	byProtocol = pMsg[0];
	_ASSERT(nIndex > 0 && nIndex < MAX_PLAYER); //人数限制

	if (ProcessFunc[byProtocol]!=NULL)
	{
		(this->*ProcessFunc[byProtocol])(nIndex, pMsg);

		Player[nIndex].SetLastNetOperationTime(g_SubWorldSet.GetGameTime());
	}
}
#endif

//同步移动
#ifndef _SERVER
void	KProtocolProcess::SyncSecMov(BYTE* pMsg)
{
	NPC_JUMP_SYNC* pNetCommandJump = (NPC_JUMP_SYNC *)pMsg;
	DWORD dwNpcId = pNetCommandJump->ID;
	int nIdx = NpcSet.SearchID(dwNpcId);
	
	if (Player[CLIENT_PLAYER_INDEX].ConformIdx(nIdx))
	{//客户端使用的
		Npc[nIdx].SendSerCommand(do_special4, pNetCommandJump->nMpsX, pNetCommandJump->nMpsY);
		Npc[nIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
	}
}

//通知打开给予UI

void	KProtocolProcess::SyncGive(BYTE* pMsg)
{
	if (Player[CLIENT_PLAYER_INDEX].CheckTrading())
		return;

	PLAYER_GIVE *pGive = (PLAYER_GIVE *)pMsg;

	Player[CLIENT_PLAYER_INDEX].m_ItemList.ClearRoom(room_give);


    PLAYER_GIVEUI pGiveUI;
    snprintf(pGiveUI.m_szName, sizeof(pGiveUI.m_szName),"%s",pGive->m_szName);
    snprintf(pGiveUI.m_Callback, sizeof(pGiveUI.m_Callback),"%s",pGive->m_Callback);
    snprintf(pGiveUI.m_Error, sizeof(pGiveUI.m_Error),"%s",pGive->m_Error);
	snprintf(pGiveUI.m_szTitle, sizeof(pGiveUI.m_szTitle),"%s",pGive->m_szTitle);

//	CoreDataChanged(GDCNI_GIVE,(unsigned int)pGive->m_szName,pGive->m_nNum);  //通知打开UI
	CoreDataChanged(GDCNI_GIVE,(unsigned int)&pGiveUI,pGive->m_nNum);  //通知打开UI
//	(unsigned int)&sMsg(unsigned int)&sMsg
//	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
}
//通知打开字符串UI
void KProtocolProcess::OpenStringUI(BYTE* pMsg) //stringbox
{	
    PLAYER_STRING *pSring = (PLAYER_STRING *)pMsg;

	PLAYER_STRINGUI pStringUI;

    snprintf(pStringUI.m_szTitle, sizeof(pStringUI.m_szTitle),"%s",pSring->m_szTitle);
    sprintf(pStringUI.m_SCallback,"%s",pSring->m_SCallback);
    snprintf(pStringUI.m_SError, sizeof(pStringUI.m_SError),"%s",pSring->m_SError);
    pStringUI.m_nNum=pSring->m_nNum;
	CoreDataChanged(GDCNI_OPEN_STRING_BOX, (unsigned int)&pStringUI, pSring->m_MaxLen);
}

void	KProtocolProcess::SyncPKValue(BYTE* pMsg)
{
	CoreDataChanged(GDCNI_PKVALUE,NULL, NULL);
}
//链接  客户端
void	KProtocolProcess::SyncViewlianitem(BYTE* pMsg)
{
	if (!g_cSellItem.GetDataL(pMsg))
        Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("链接:数据有误,请联系GM处理!");
}

/*
void	KProtocolProcess::s2cLianUpdateItem(BYTE* pMsg)
{
	g_cSellItem.GetDataL(pMsg,1);
}

*/
void	KProtocolProcess::s2cViewEquip(BYTE* pMsg)
{ 
//	    Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("查看装备:数据有误,请联系GM处理!");
	    g_cViewItem.GetData(pMsg);
}

void	KProtocolProcess::s2cgettongback(BYTE* pMsg)
{
         PLAYER_TONGINFO_CALLBACK *nTongBack=(PLAYER_TONGINFO_CALLBACK *)pMsg;
         if (nTongBack->inType==1)
		 {//更新帮会设置和公告消息
			 KClientZhaoMuInfo nTongInfo;
			 memset(&nTongInfo, 0, sizeof(nTongInfo));

             snprintf(nTongInfo.szQingXiang, sizeof(nTongInfo.szQingXiang), "%s", nTongBack->szQingXiang);
			 snprintf(nTongInfo.szHuoDong1, sizeof(nTongInfo.szHuoDong1), "%s", nTongBack->szHuoDong1);
			 snprintf(nTongInfo.szHuoDong2, sizeof(nTongInfo.szHuoDong2), "%s", nTongBack->szHuoDong2);
			 snprintf(nTongInfo.szHuoDong3, sizeof(nTongInfo.szHuoDong3), "%s", nTongBack->szHuoDong3);
			 snprintf(nTongInfo.szHuoDong4, sizeof(nTongInfo.szHuoDong4), "%s", nTongBack->szHuoDong4);
			 snprintf(nTongInfo.szJiYu, sizeof(nTongInfo.szJiYu), "%s", nTongBack->szJiYu);
			 snprintf(nTongInfo.szGongGao, sizeof(nTongInfo.szGongGao), "%s", nTongBack->szGongGao);
			 nTongInfo.inType=nTongBack->inType;
             nTongInfo.inAcceptLevel=nTongBack->inAcceptLevel;
			 nTongInfo.inRefuseLevel=nTongBack->inRefuseLevel;

			 CoreDataChanged(GDCNI_GETTONG_INFO, (unsigned int)&nTongInfo, 0);
             nTongBack=NULL;
		 }
		 else if (nTongBack->inType==5)
		 {//更新申请入帮派的申请列表
               KClientApplyList mTongApplyList;
               memset(&mTongApplyList, 0, sizeof(mTongApplyList));
			   for (int i=0;i<nTongBack->inAcceptLevel;i++)
			   {
                 snprintf(mTongApplyList.nInfo[i].szName, sizeof(mTongApplyList.nInfo[i].szName), "%s", nTongBack->m_sMember[i].m_szName);
			     mTongApplyList.nInfo[i].inLevel = nTongBack->m_sMember[i].m_inLevel;
				 mTongApplyList.inType           = nTongBack->inType;
				 mTongApplyList.nCurNum          = nTongBack->inAcceptLevel;     //当前的数量
				 mTongApplyList.nStartPos        = nTongBack->inRefuseLevel;     //开始读取的位置
			   }
            //  Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("获取帮会申请列表成功!");
			   CoreDataChanged(GDCNI_GETTONG_INFO, (unsigned int)&mTongApplyList, nTongBack->inType);
		 }

}

void	KProtocolProcess::s2cViewUpdateEquip(BYTE* pMsg)
{
	    g_cViewItem.GetData(pMsg,1);
}

void	KProtocolProcess::s2cViewSellItem(BYTE* pMsg)
{
	g_cSellItem.GetData(pMsg);
}

void	KProtocolProcess::s2cViewUpdateItem(BYTE* pMsg)
{
	g_cSellItem.GetData(pMsg,1);
}

//获取数量
void	KProtocolProcess::s2cGetCouunt(BYTE* pMsg)
{
	PLAYER_GET_COUNT *pGetCount = (PLAYER_GET_COUNT *)pMsg;
	g_cSellItem.Check(pGetCount->nCount,pGetCount->dwId);
}

void	KProtocolProcess::s2cShopName(BYTE* pMsg)
{
	PLAYER_PLAYER_SHOPNAME *pShopname = (PLAYER_PLAYER_SHOPNAME *)pMsg;
	int nIndex = NpcSet.SearchID(pShopname->m_dwNpcID);

	//char nstrName[32]={0};
	if (nIndex>0)
	   Npc[nIndex].SetstrInfo(STR_SHOP_NAME,pShopname->m_Name);
	//strcpy(Npc[nIndex].ShopName,pShopname->m_Name);
}

//void KProtocolProcess::SyncInfo(BYTE* pMsg)
//{
//没有完成

//}

// mat na
void KProtocolProcess::SyncMaskLock(BYTE* pMsg)
{
	NPC_SIT_SYNC *pInfo = (NPC_SIT_SYNC *)pMsg;
	Player[CLIENT_PLAYER_INDEX].m_ItemList.SetMaskLock(pInfo->ID);
}

void KProtocolProcess::NpcSleepSync(BYTE* pMsg)
{
	NPC_SLEEP_SYNC*	pSync = (NPC_SLEEP_SYNC *)pMsg;
	DWORD	dwNpcId = pSync->NpcID;

	int nIdx = NpcSet.SearchID(dwNpcId);

	if (nIdx > 0)
	{
		Npc[nIdx].SetSleepMode((BOOL)pSync->bSleep);
		Npc[nIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
	}
}
//客户端
void KProtocolProcess::s2cPing(BYTE* pMsg)
{
//	DWORD	dwTimer = GetTickCount();
//	PING_COMMAND* PingCmd = (PING_COMMAND *)pMsg;
//
//	dwTimer -= PingCmd->m_dwTime;
//	dwTimer >>= 1;
//	g_SubWorldSet.SetPing(dwTimer);
//	g_bPingReply = TRUE;

	DWORD	dwTimer = GetTickCount(); //获取系统运行的时间
	PING_COMMAND*	PingCmd = (PING_COMMAND *)pMsg;
	PING_CLIENTREPLY_COMMAND	pcc;
	pcc.ProtocolType = c2s_ping;
	pcc.m_dwReplyServerTime = PingCmd->m_dwTime;  //返回给服务器的 校对时间
	pcc.m_dwClientTime      = dwTimer;            //当前客户端的时间
	g_pClient->SendPackToServer(&pcc, sizeof(PING_CLIENTREPLY_COMMAND));
	//g_SubWorldSet.SetPing(PingCmd->m_dwTime);
}

void KProtocolProcess::ServerReplyClientPing(BYTE* pMsg)
{
	DWORD	dwTimer = GetTickCount();
	PING_COMMAND* pPc = (PING_COMMAND *)pMsg;

	dwTimer -= pPc->m_dwTime;
	dwTimer >>= 1;
	g_SubWorldSet.SetPing(dwTimer);  //设定上次 和服务器通话的时间
}

void KProtocolProcess::s2cPlayerExp(BYTE* pMsg)
{
	PLAYER_EXP_SYNC	*pExp = (PLAYER_EXP_SYNC*)pMsg;
	Player[CLIENT_PLAYER_INDEX].s2cSetExp(pExp->m_nExp);
}

void	KProtocolProcess::s2cChatScreenSingleError(BYTE* pMsg)
{
	CHAT_SCREENSINGLE_ERROR_SYNC	*pError = (CHAT_SCREENSINGLE_ERROR_SYNC*)pMsg;
	
	// 通知界面屏幕单聊不成功
	char	szName[32];
	memset(szName, 0, sizeof(szName));
	memcpy(szName, pError->m_szName, pError->m_wLength + 1 + sizeof(pError->m_szName) - sizeof(CHAT_SCREENSINGLE_ERROR_SYNC));

	KSystemMessage	sMsg;
	sprintf(sMsg.szMessage, MSG_CHAT_TAR_REFUSE_SINGLE_TALK, szName);
	sMsg.eType = SMT_NORMAL;
	sMsg.byConfirmType = SMCT_NONE;
	sMsg.byPriority = 0;
	sMsg.byParamSize = 0;
	sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
	CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
}

void KProtocolProcess::NetCommandChgCamp(BYTE* pMsg)
{
	DWORD	dwNpcId;

	dwNpcId = *(DWORD *)&pMsg[1];
	int nIdx = NpcSet.SearchID(dwNpcId);

	if (nIdx > 0)
	{
		Npc[nIdx].m_Camp = (int)pMsg[5];
		Npc[nIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
	}
}

void KProtocolProcess::NetCommandChgCurCamp(BYTE* pMsg)
{
	DWORD	dwNpcId;

	dwNpcId = *(DWORD *)&pMsg[1];
	int nIdx = NpcSet.SearchID(dwNpcId);

	if (nIdx > 0)
	{
		Npc[nIdx].m_CurrentCamp = (int)pMsg[5];
		Npc[nIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
	}
}
//客户端
void KProtocolProcess::NetCommandDeath(BYTE* pMsg)
{
	NPC_DEATH_SYNC	*pDeath = (NPC_DEATH_SYNC*)pMsg;

	int nIdx = NpcSet.SearchID(pDeath->ID);
    //int nIdxR= NpcSet.SearchName(pDeath->nDeathName);  //死亡者的名字
	if (nIdx > 0)
	{
		Npc[nIdx].ProcNetCommand(do_death);
		Npc[nIdx].m_CurrentLife = 0;
		Npc[nIdx].m_CurrentNuQi = 0;
	    Npc[nIdx].m_Height      = 0;  //高度为零
		Npc[nIdx].m_SyncSignal  = SubWorld[0].m_dwCurrentTime;
	}

}

void KProtocolProcess::NetCommandChat(BYTE* pMsg)
{
	NPC_CHAT_SYNC *pSync = (NPC_CHAT_SYNC*)pMsg;
	
	int nIdx = NpcSet.SearchID(pSync->ID);
	
	if (nIdx > 0)
	{
		pSync->nMsgLen = TEncodeText(pSync->szMsg, strlen(pSync->szMsg));
		Npc[nIdx].SetChatInfo(Npc[nIdx].Name, pSync->szMsg, pSync->nMsgLen);
		Npc[nIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
	}
}

void KProtocolProcess::NetCommandJump(BYTE* pMsg)
{
	NPC_JUMP_SYNC* pNetCommandJump = (NPC_JUMP_SYNC *)pMsg;
	DWORD dwNpcId = pNetCommandJump->ID;
	int nIdx = NpcSet.SearchID(dwNpcId);
	
	if (Player[CLIENT_PLAYER_INDEX].ConformIdx(nIdx))
	{
		Npc[nIdx].SendSerCommand(do_jump, pNetCommandJump->nMpsX, pNetCommandJump->nMpsY);
		Npc[nIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
	}
}

void KProtocolProcess::NetCommandHurt(BYTE* pMsg)
{
	NPC_HURT_SYNC*	pSync = (NPC_HURT_SYNC *)pMsg;
	
	int nIdx = NpcSet.SearchID(pSync->ID);
	if (nIdx > 0)
	{
		//Npc[nIdx].SendCommand(do_hurt, pSync->nFrames, pSync->nX, pSync->nY);
		Npc[nIdx].ProcNetCommand(do_hurt, pSync->nFrames, pSync->nX, pSync->nY);
		Npc[nIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
	}
}

void KProtocolProcess::NetCommandRemoveNpc(BYTE* pMsg)
{
	DWORD	dwNpcID;
	dwNpcID = *(DWORD *)&pMsg[1];
	int nIdx = NpcSet.SearchID(dwNpcID);

	if (Player[CLIENT_PLAYER_INDEX].ConformIdx(nIdx))
	{
		if (Npc[nIdx].m_RegionIndex >= 0)
		{
			if(*(BOOL *)&pMsg[5])
				SubWorld[0].m_Region[Npc[nIdx].m_RegionIndex].RemoveNpc(nIdx);

			SubWorld[0].m_Region[Npc[nIdx].m_RegionIndex].DecNpcRef(Npc[nIdx].m_MapX, Npc[nIdx].m_MapY);
//			SubWorld[Npc[nIdx].m_SubWorldIndex].m_Region[Npc[nIdx].m_RegionIndex].RemoveNpc(nIdx);
		}
		NpcSet.Remove(nIdx);
	}
}

void KProtocolProcess::NetCommandRun(BYTE* pMsg)
{
	DWORD	dwNpcID;
	DWORD	MapX, MapY;
	
	dwNpcID = *(DWORD *)&pMsg[1];
	MapX = *(int *)&pMsg[5];
	MapY = *(int *)&pMsg[9];
	int nIdx = NpcSet.SearchID(dwNpcID);
	if (Player[CLIENT_PLAYER_INDEX].ConformIdx(nIdx))  //不等于客户端
	{
		Npc[nIdx].SendSerCommand(do_run, MapX, MapY);
		Npc[nIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
	}
}

void KProtocolProcess::NetCommandSit(BYTE* pMsg)
{
	NPC_SIT_SYNC*	pSitSync;

	pSitSync = (NPC_SIT_SYNC *)pMsg;

	int nIdx = NpcSet.SearchID(pSitSync->ID);

	if (Player[CLIENT_PLAYER_INDEX].ConformIdx(nIdx))
	{
		Npc[nIdx].SendSerCommand(do_sit);
		Npc[nIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
	}
}

void 	KProtocolProcess::s2crolesyncvipvalue(BYTE* pMsg)
{

}

void 	KProtocolProcess::s2cobjadd_vn(BYTE* pMsg)
{

}
//通知客户端退出游戏
void 	KProtocolProcess::s2ckickoutgame(BYTE* pMsg)
{

}

void 	KProtocolProcess::s2citemdatashop(BYTE* pMsg)
{
	   BuySell.SetShopItemdata(pMsg);
}

void 	KProtocolProcess::s2csyncitemshop(BYTE* pMsg)
{
	//S2C_SUPERSHOP* pInfo = (S2C_SUPERSHOP*)pMsg;
	//BuySell.OpenCSuPerShop(pInfo->m_nSaleType, (BuySellInfo*)&pInfo->m_BuySellInfo);
	BuySell.SetShopItemInfo(pMsg);
}


//客户端
void 	KProtocolProcess::s2cSyncSuperShop(BYTE* pMsg)
{
	S2C_SUPERSHOP* pInfo = (S2C_SUPERSHOP*)pMsg;
	BuySell.OpenCSuPerShop(pInfo->m_nSaleType, (BuySellInfo*)&pInfo->m_BuySellInfo);
}


void KProtocolProcess::OpenSaleBox(BYTE* pMsg)
{
	SALE_BOX_SYNC* pSale = (SALE_BOX_SYNC *)pMsg;

	BuySell.OpenCSale(pSale->nShopIndex);
}

void KProtocolProcess::OpenStoreBox(BYTE* pMsg)
{
	CoreDataChanged(GDCNI_OPEN_STORE_BOX, NULL, NULL);
}

void KProtocolProcess::OpenResetPass(BYTE* pMsg)
{
	CoreDataChanged(GDCNI_OPEN_RESET_PASS, NULL, NULL);
}

void KProtocolProcess::GiftDT(BYTE* pMsg)
{	
	CoreDataChanged(GDCNI_OPEN_DATAU_BOX, NULL, NULL);
}
//玩家重生  客户端
void KProtocolProcess::PlayerRevive(BYTE* pMsg)
{
	NPC_REVIVE_SYNC* pSync = (NPC_REVIVE_SYNC*)pMsg;

	int nIdx = NpcSet.SearchID(pSync->ID);
	if (nIdx > 0 && nIdx<MAX_NPC)
	{
		if (!Npc[nIdx].IsPlayer() && (pSync->mType == 0 || pSync->mType == 1))
		{//不是玩家 就 同步 删除客户端NPC 
			//SubWorld[0].m_WorldMessage.NewSend(GWM_NPC_DEL, nIdx);	
			if (Npc[nIdx].m_RegionIndex >= 0)
			{				
				int nSubWorld = Npc[nIdx].m_SubWorldIndex;
				int nRegion = Npc[nIdx].m_RegionIndex;
				
				if (nSubWorld>=0 && nRegion>=0)
				{
					SubWorld[nSubWorld].m_Region[nRegion].RemoveNpc(nIdx);
					SubWorld[nSubWorld].m_Region[nRegion].DecNpcRef(Npc[nIdx].m_MapX, Npc[nIdx].m_MapY);
				}
				
			}
		    NpcSet.Remove(nIdx);
			return;
		}
		else
		{//是本人 就开始重生
			Npc[nIdx].ProcNetCommand(do_revive);
		}
	}
}

void KProtocolProcess::RequestNpcFail(BYTE* pMsg)
{
	NPC_REQUEST_FAIL* pNpcSync = (NPC_REQUEST_FAIL *)pMsg;

	if (NpcSet.IsNpcRequestExist(pNpcSync->ID))
		NpcSet.RemoveNpcRequest(pNpcSync->ID);	
}
//客户端  技能同步
void KProtocolProcess::NetCommandSkill(BYTE* pMsg)
{
	DWORD	dwNpcID;
	int		nSkillID, nSkillLevel, nSkillEnChance;
	int		MapX, MapY;
	
	dwNpcID = *(DWORD *)&pMsg[1];
	nSkillID = *(int *)&pMsg[5];
	nSkillLevel = *(int *)&pMsg[9];
	MapX = *(int *)&pMsg[13];
	MapY = *(int *)&pMsg[17];
	nSkillEnChance = *(int *)&pMsg[21];
	
	if (MapY < 0)
		return ;

	//当指定某个目标时(MapX == -1),MapY为目标的NpcdwID，需要转换成本地的NpcIndex才行
	if (MapX < 0)
	{
		if (MapX != -1)
			return;
	
		MapY = NpcSet.SearchID(MapY);
		if (MapY == 0)
			return;

		if (Npc[MapY].m_RegionIndex < 0)
			return;

	}
	
	int nIdx = NpcSet.SearchID(dwNpcID);
	
	if (nIdx <= 0) 
		return;

	if (Player[CLIENT_PLAYER_INDEX].ConformIdx(nIdx))
	{
		Npc[nIdx].m_SkillList.SetSkillLevel(nSkillID, nSkillLevel);
		Npc[nIdx].m_SkillList.SetEnChance(nSkillID, nSkillEnChance);   //设置客户端的技能加成
		Npc[nIdx].SendSerCommand(do_skill, nSkillID, MapX, MapY);      //开始设置使用技能
		Npc[nIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
	}
}

void KProtocolProcess::NetCommandWalk(BYTE* pMsg)
{
	DWORD	dwNpcID;
	DWORD	MapX, MapY;
	
	dwNpcID = *(DWORD *)&pMsg[1];
	MapX = *(int *)&pMsg[5];
	MapY = *(int *)&pMsg[9];
	int nIdx = NpcSet.SearchID(dwNpcID);
	if (Player[CLIENT_PLAYER_INDEX].ConformIdx(nIdx))
	{
		Npc[nIdx].SendSerCommand(do_walk, MapX, MapY);
		Npc[nIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
	}
}

//-------------------------------------------------------------------------
//	功能：收到服务器通知队伍创建失败
//-------------------------------------------------------------------------
void KProtocolProcess::s2cApplyCreateTeamFalse(BYTE* pMsg)
{
	PLAYER_SEND_CREATE_TEAM_FALSE *pCreateFalse = (PLAYER_SEND_CREATE_TEAM_FALSE*)pMsg;
	KSystemMessage	sMsg;

	switch (pCreateFalse->m_btErrorID)
	{
	// 已经在队伍中，说明客户端队伍数据有错误，申请重新获得队伍数据
	case Team_Create_Error_InTeam:	//Team_Creae_Error_InTeam
		Player[CLIENT_PLAYER_INDEX].ApplySelfTeamInfo();
		break;

	// 当前处于不能组队状态
	case Team_Create_Error_CannotCreate:
		snprintf(sMsg.szMessage, sizeof(sMsg.szMessage), "%s", MSG_TEAM_CANNOT_CREATE);
		sMsg.eType = SMT_NORMAL;
		sMsg.byConfirmType = SMCT_NONE;
		sMsg.byPriority = 0;
		sMsg.byParamSize = 0;
		sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		break;


	default:
		snprintf(sMsg.szMessage, sizeof(sMsg.szMessage), "%s", MSG_TEAM_CREATE_FAIL);
		sMsg.eType = SMT_NORMAL;
		sMsg.byConfirmType = SMCT_NONE;
		sMsg.byPriority = 0;
		sMsg.byParamSize = 0;
		sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		break;
	}
}

//-------------------------------------------------------------------------
//	功能：收到服务器通知查询某个队伍组队情况失败
//-------------------------------------------------------------------------
void KProtocolProcess::s2cApplyTeamInfoFalse(BYTE* pMsg)
{
	// 界面显示队伍查询失败(not end)
}

//-------------------------------------------------------------------------
//	功能：添加一个好友
//-------------------------------------------------------------------------
void KProtocolProcess::s2cChatAddFriend(BYTE* pMsg)
{
	CHAT_ADD_FRIEND_SYNC	*pFriend = (CHAT_ADD_FRIEND_SYNC*)pMsg;

	Player[CLIENT_PLAYER_INDEX].m_cChat.AddOne(pFriend->m_dwID, pFriend->m_nIdx, pFriend->m_szName, 0);
	Player[CLIENT_PLAYER_INDEX].m_cChat.SaveTeamInfo(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name);

	KSystemMessage	sMsg;
	KUiPlayerItem	sPlayer;

	memset(&sPlayer, 0, sizeof(KUiPlayerItem));
	snprintf(sPlayer.Name, sizeof(sPlayer.Name), "%s", pFriend->m_szName);
	sPlayer.uId = pFriend->m_dwID;
	sPlayer.nIndex = pFriend->m_nIdx;

	sprintf(sMsg.szMessage, MSG_CHAT_ADD_FRIEND_SUCCESS, pFriend->m_szName);
	sMsg.eType = SMT_FRIEND;
	sMsg.byConfirmType = SMCT_UI_INTERVIEW;
	sMsg.byPriority = 2;
	sMsg.byParamSize = sizeof(KUiPlayerItem);
	sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
	CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, (int)&sPlayer);
}

//-------------------------------------------------------------------------
//	功能：添加好友失败
//-------------------------------------------------------------------------
void KProtocolProcess::s2cChatAddFriendFail(BYTE* pMsg)
{
	CHAT_ADD_FRIEND_FAIL_SYNC	*pFail = (CHAT_ADD_FRIEND_FAIL_SYNC*)pMsg;

	char	szName[32];
	CChatApplyListNode	*pList;
	pList = (CChatApplyListNode*)Player[CLIENT_PLAYER_INDEX].m_cChat.m_cApplyAddList.GetHead();
	while (pList)
	{
		if (pList->m_nPlayerIdx == pFail->m_nTargetPlayerIdx)
		{
			snprintf(szName, sizeof(szName), "%s", pList->m_szName);

			// 通知界面添加某人为好友操作失败
			KSystemMessage	sMsg;
			sprintf(sMsg.szMessage, MSG_CHAT_ADD_FRIEND_FAIL, szName);
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);

			return;
		}
		pList = (CChatApplyListNode*)pList->GetNext();
	}	
}

//-------------------------------------------------------------------------
//	功能：收到服务器过来的玩家某个聊天好友的数据（带名字）
//-------------------------------------------------------------------------
void KProtocolProcess::s2cChatAddOneFriend(BYTE* pMsg)
{
	CHAT_ONE_FRIEND_DATA_SYNC	*pData = (CHAT_ONE_FRIEND_DATA_SYNC*)pMsg;
	int		nTeamNo;

	nTeamNo = Player[CLIENT_PLAYER_INDEX].m_cChat.CheckTeamNo(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name, pData->m_szName);
	if (nTeamNo < 0)
		nTeamNo = 0;
	Player[CLIENT_PLAYER_INDEX].m_cChat.AddOne(pData->m_dwID, pData->m_nPlayerIdx, pData->m_szName, nTeamNo);
	Player[CLIENT_PLAYER_INDEX].m_cChat.SaveTeamInfo(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name);

	// 通知界面得到新成员 not end
}

//-------------------------------------------------------------------------
//	功能：被别的玩家拒绝交友
//-------------------------------------------------------------------------
void KProtocolProcess::s2cChatBeRefusedAddFriend(BYTE* pMsg)
{
	CHAT_REFUSE_FRIEND_SYNC	*pRefuse = (CHAT_REFUSE_FRIEND_SYNC*)pMsg;
	char	szName[64];

	memset(szName, 0, sizeof(szName));
	memcpy(szName, pRefuse->m_szName, pRefuse->m_wLength + 1 + sizeof(pRefuse->m_szName) - sizeof(CHAT_REFUSE_FRIEND_SYNC));

	// 通知界面pRefuse->m_szName拒绝了他的交友申请
	KSystemMessage	sMsg;

	sprintf(sMsg.szMessage, MSG_CHAT_REFUSE_FRIEND, szName);
	sMsg.eType = SMT_FRIEND;
	sMsg.byConfirmType = SMCT_CLICK;
	sMsg.byPriority = 1;
	sMsg.byParamSize = 0;
	sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
	CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
}

//-------------------------------------------------------------------------
//	功能：得到通知好友上线
//-------------------------------------------------------------------------
void KProtocolProcess::s2cChatFriendOnline(BYTE* pMsg)
{
	CHAT_FRIEND_ONLINE_SYNC	*pFriend = (CHAT_FRIEND_ONLINE_SYNC*)pMsg;

	Player[CLIENT_PLAYER_INDEX].ChatFriendOnLine(pFriend->m_dwID, pFriend->m_nPlayerIdx);
}

//-------------------------------------------------------------------------
// 收到服务器转发的别人添加聊天好友的申请
//-------------------------------------------------------------------------
void KProtocolProcess::s2cChatGetApplyAddFriend(BYTE* pMsg)
{
	CHAT_APPLY_ADD_FRIEND_SYNC	*pApply = (CHAT_APPLY_ADD_FRIEND_SYNC*)pMsg;

	if ( Player[CLIENT_PLAYER_INDEX].m_cChat.CheckIsFriend(pApply->m_nSrcPlayerIdx) )
		return;

	CChatApplyListNode	*pNode = NULL;
	pNode = new CChatApplyListNode;
	if (pNode == NULL)
		return;
	pNode->m_nPlayerIdx = pApply->m_nSrcPlayerIdx;
	snprintf(pNode->m_szName, sizeof(pNode->m_szName), "%s", pApply->m_szSourceName);
	strcpy(pNode->m_szInfo, pApply->m_szInfo);

	Player[CLIENT_PLAYER_INDEX].m_cChat.m_cApplyAddList.AddTail(pNode);

	// 通知界面有人申请添加聊天好友
	KSystemMessage	sMsg;
	KUiPlayerItem	sPlayer;

	snprintf(sPlayer.Name, sizeof(sPlayer.Name), "%s", pApply->m_szSourceName);
	sPlayer.nIndex = pApply->m_nSrcPlayerIdx;
	sPlayer.uId = 0;
	sPlayer.nData = 0;

	sprintf(sMsg.szMessage, MSG_CHAT_GET_FRIEND_APPLY, pNode->m_szName);
	sMsg.eType = SMT_FRIEND;
	sMsg.byConfirmType = SMCT_UI_FRIEND_INVITE;
	sMsg.byPriority = 3;
	sMsg.byParamSize = sizeof(KUiPlayerItem);
	sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
	CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, (int)&sPlayer);
}

//-------------------------------------------------------------------------
//	功能：收到服务器过来的玩家登录时获得聊天好友数据（带名字）
//-------------------------------------------------------------------------
void KProtocolProcess::s2cChatLoginFriendName(BYTE* pMsg)
{
	CHAT_LOGIN_FRIEND_NAME_SYNC	*pFriend = (CHAT_LOGIN_FRIEND_NAME_SYNC*)pMsg;
	int		nTeamNo;
	char	szName[64];

	memset(szName, 0, sizeof(szName));
	memcpy(szName, pFriend->m_szName, pFriend->m_wLength + 1 + sizeof(pFriend->m_szName) - sizeof(CHAT_LOGIN_FRIEND_NAME_SYNC));
	nTeamNo = Player[CLIENT_PLAYER_INDEX].m_cChat.CheckTeamNo(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name, szName);
	if (nTeamNo < 0)
		nTeamNo = 0;
	Player[CLIENT_PLAYER_INDEX].m_cChat.AddOne(pFriend->m_dwID, pFriend->m_nPlayerIdx, szName, nTeamNo);
}

//-------------------------------------------------------------------------
//	功能：收到服务器过来的玩家登录时获得聊天好友数据（不带名字）
//-------------------------------------------------------------------------
void KProtocolProcess::s2cChatLoginFriendNoName(BYTE* pMsg)
{
	CHAT_LOGIN_FRIEND_NONAME_SYNC	*pFriend = (CHAT_LOGIN_FRIEND_NONAME_SYNC*)pMsg;
	char	szName[32];
	int		nTeamNo;

	szName[0] = 0;
	nTeamNo = Player[CLIENT_PLAYER_INDEX].m_cChat.CheckTeamNo(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name, NULL);
	if (nTeamNo < 0)
	{
		if (Player[CLIENT_PLAYER_INDEX].m_cChat.m_nLoginGetFirstOneFriendFlag == 0)		// 申请重新发送所有好友数据，带名字
		{
			Player[CLIENT_PLAYER_INDEX].m_cChat.m_nLoginGetFirstOneFriendFlag = 1;

			CHAT_APPLY_RESEND_ALL_FRIEND_NAME_COMMAND	sApply;
			sApply.ProtocolType = c2s_chatapplyresendallfriendname;

			if (g_pClient)
				g_pClient->SendPackToServer((BYTE*)&sApply, sizeof(CHAT_APPLY_RESEND_ALL_FRIEND_NAME_COMMAND));
		}
		else												// 申请重新发送这个好友数据，带名字
		{
			CHAT_APPLY_SEND_ONE_FRIEND_NAME_COMMAND	sApply;
			sApply.ProtocolType = c2s_chatapplysendonefriendname;
			sApply.m_dwID = pFriend->m_dwID;

			if (g_pClient)
				g_pClient->SendPackToServer((BYTE*)&sApply, sizeof(CHAT_APPLY_SEND_ONE_FRIEND_NAME_COMMAND));
		}
		return;
	}
	Player[CLIENT_PLAYER_INDEX].m_cChat.AddOne(pFriend->m_dwID, pFriend->m_nPlayerIdx, szName, nTeamNo);
	Player[CLIENT_PLAYER_INDEX].m_cChat.SaveTeamInfo(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name);
	Player[CLIENT_PLAYER_INDEX].m_cChat.m_nLoginGetFirstOneFriendFlag = 1;

}

//-------------------------------------------------------------------------
//	功能：收到服务器通知创建一支队伍
//-------------------------------------------------------------------------
void KProtocolProcess::s2cCreateTeam(BYTE* pMsg)
{
	PLAYER_SEND_CREATE_TEAM_SUCCESS	*pCreateSuccess = (PLAYER_SEND_CREATE_TEAM_SUCCESS*)pMsg;

	g_Team[0].CreateTeam(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_dwID, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Level, pCreateSuccess->nTeamServerID);
	Player[CLIENT_PLAYER_INDEX].m_cTeam.Release();
	Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag = 1;
	Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFigure = TEAM_CAPTAIN;
	Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nApplyCaptainID = 0;
	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].RestoreCurrentCamp();

	KSystemMessage	sMsg;
	snprintf(sMsg.szMessage, sizeof(sMsg.szMessage), "%s", MSG_TEAM_CREATE);
	sMsg.eType = SMT_NORMAL;
	sMsg.byConfirmType = SMCT_NONE;
	sMsg.byPriority = 0;
	sMsg.byParamSize = 0;
	sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
	CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);

	snprintf(sMsg.szMessage, sizeof(sMsg.szMessage), "%s", MSG_TEAM_CREATE);
	sMsg.eType = SMT_TEAM;
	sMsg.byConfirmType = SMCT_CLICK;
	sMsg.byPriority = 1;
	sMsg.byParamSize = 0;
	sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
	CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);

	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SetInstantSpr(enumINSTANT_STATE_CREATE_TEAM); ////释放一个组队的瞬间特效
}

//-------------------------------------------------------------------------
//	功能：收到服务器通知开放当前门派技能某等级
//-------------------------------------------------------------------------
void KProtocolProcess::s2cFactionSkillOpen(BYTE* pMsg)
{
	PLAYER_FACTION_SKILL_LEVEL	*pSkill = (PLAYER_FACTION_SKILL_LEVEL*)pMsg;

	if (Player[CLIENT_PLAYER_INDEX].m_cFaction.m_nCurFaction < 0 || Player[CLIENT_PLAYER_INDEX].m_cFaction.m_nCurFaction != pSkill->m_btCurFactionID)
	{
		// 申请更新门派信息
		Player[CLIENT_PLAYER_INDEX].ApplyFactionData();
		return;
	}

//	for (int i = 0; i < FACTIONS_PRR_SERIES; i++)
//	{
//		if (Player[CLIENT_PLAYER_INDEX].m_cFaction.m_sSkillOpen[i].m_nID == Player[CLIENT_PLAYER_INDEX].m_cFaction.m_nCurFaction)
//		{
//			Player[CLIENT_PLAYER_INDEX].m_cFaction.m_sSkillOpen[i].m_nOpenLevel = pSkill->m_btLevel;
//			Player[CLIENT_PLAYER_INDEX].m_cFaction.OpenCurSkillLevel(Player[CLIENT_PLAYER_INDEX].m_cFaction.m_sSkillOpen[i].m_nOpenLevel, &Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList);
//			break;
//		}
//	}
}

//-------------------------------------------------------------------------
//	功能：收到服务器发来的聊天信息
//-------------------------------------------------------------------------
void KProtocolProcess::s2cGetChat(BYTE* pMsg)
{
	PLAYER_SEND_CHAT_SYNC	*pChat = (PLAYER_SEND_CHAT_SYNC*)pMsg;

	Player[CLIENT_PLAYER_INDEX].m_cChat.GetChat(pChat);
}

//-------------------------------------------------------------------------
//	功能：收到服务器发过来的当前某属性的点数
//-------------------------------------------------------------------------
void KProtocolProcess::s2cGetCurAttribute(BYTE* pMsg)
{
	Player[CLIENT_PLAYER_INDEX].s2cGetCurAttribute(pMsg);
}

//-------------------------------------------------------------------------
//	功能：收到服务器发来的统率力经验值
//-------------------------------------------------------------------------
void KProtocolProcess::s2cGetLeadExp(BYTE* pMsg)
{
	DWORD	dwLevel = Player[CLIENT_PLAYER_INDEX].m_dwLeadLevel;

	PLAYER_LEAD_EXP_SYNC	*pLeadExp = (PLAYER_LEAD_EXP_SYNC*)pMsg;
	Player[CLIENT_PLAYER_INDEX].m_dwLeadExp = pLeadExp->m_dwLeadExp;
	Player[CLIENT_PLAYER_INDEX].m_dwLeadLevel = PlayerSet.m_cLeadExp.GetLevel(Player[CLIENT_PLAYER_INDEX].m_dwLeadExp, Player[CLIENT_PLAYER_INDEX].m_dwLeadLevel);
	Player[CLIENT_PLAYER_INDEX].m_dwNextLevelLeadExp = PlayerSet.m_cLeadExp.GetLevelExp(Player[CLIENT_PLAYER_INDEX].m_dwLeadLevel);

	if (dwLevel < Player[CLIENT_PLAYER_INDEX].m_dwLeadLevel)
	{
		KSystemMessage	sMsg;
		sprintf(sMsg.szMessage, MSG_LEADER_LEVEL_UP, Player[CLIENT_PLAYER_INDEX].m_dwLeadLevel);
		sMsg.eType = SMT_PLAYER;
		sMsg.byConfirmType = SMCT_CLICK;
		sMsg.byPriority = 1;
		sMsg.byParamSize = 0;
		sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
	}
}

//-------------------------------------------------------------------------
//	功能：收到服务器发过来的某技能点数
//-------------------------------------------------------------------------
void KProtocolProcess::s2cGetSkillLevel(BYTE* pMsg)
{
	int		nSkillIndex=0;
	PLAYER_SKILL_LEVEL_SYNC	*pSkill = (PLAYER_SKILL_LEVEL_SYNC*)pMsg;
	if (pSkill->m_nSkillLevel >=MAX_SKILLLEVEL)
		return;
	nSkillIndex = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.FindSame(pSkill->m_nSkillID);
	Player[CLIENT_PLAYER_INDEX].m_nSkillPoint = pSkill->m_nLeavePoint;
	if (nSkillIndex<=0)
	{//如果是新技能
		//如果该技能的等级为零，表示新学，直接向客户端填加
		if (pSkill->m_nSkillLevel == 0)
		{
			if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.Add(pSkill->m_nSkillID, 0) == 0 )
				return;
		}
		else
		{  
			if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.Add(pSkill->m_nSkillID, pSkill->m_nSkillLevel) == 0 )
				return;
		}
	}
	else
	{//旧技能就直接设置等级
		    Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.SetLevel(nSkillIndex, pSkill->m_nSkillLevel);
	}


	        Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.SetAddPoint(nSkillIndex, pSkill->m_nAddPoint);

//	MessageBox(0,"OK","",MB_OK);

	// 通知界面更新技能数据
	CoreDataChanged(GDCNI_FIGHT_SKILL_POINT, 0, Player[CLIENT_PLAYER_INDEX].m_nSkillPoint);
	KUiSkillData SkillData;
	SkillData.uGenre = CGOG_SKILL_FIGHT;
	SkillData.uId    = pSkill->m_nSkillID;
	SkillData.nLevel = pSkill->m_nSkillLevel;
	int nSkillPosition = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetSkillPosition(pSkill->m_nSkillID);
	if(nSkillPosition < 0) 
	   return;
	CoreDataChanged(GDCNI_SKILL_CHANGE, (unsigned int)&SkillData, nSkillPosition-4); //减去前面3个 默认技能位置
}

//-------------------------------------------------------------------------
//	功能：收到服务器发来的队友等级变化通知
//-------------------------------------------------------------------------
void KProtocolProcess::s2cGetTeammateLevel(BYTE* pMsg)
{
	PLAYER_TEAMMATE_LEVEL_SYNC	*pLevel = (PLAYER_TEAMMATE_LEVEL_SYNC*)pMsg;
	if (!Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag)
	{
		Player[CLIENT_PLAYER_INDEX].ApplySelfTeamInfo();
		return;
	}

	if ((DWORD)g_Team[0].m_nCaptain == pLevel->m_dwTeammateID)
	{
		g_Team[0].m_nMemLevel[0] = (WORD)pLevel->m_btLevel;
		return;
	}

	for (int i = 0; i < MAX_TEAM_MEMBER; i++)
	{
		if ((DWORD)g_Team[0].m_nMember[i] == pLevel->m_dwTeammateID)
		{
			g_Team[0].m_nMemLevel[i + 1] = (WORD)pLevel->m_btLevel;
			break;
		}
	}
}

//-------------------------------------------------------------------------
//	功能：收到服务器通知离开门派
//-------------------------------------------------------------------------
void KProtocolProcess::s2cLeaveFaction(BYTE* pMsg)
{
	Player[CLIENT_PLAYER_INDEX].m_cFaction.LeaveFaction();

	// 改变阵营
	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SetCamp(camp_free);

	CoreDataChanged(GDCNI_PLAYER_BASE_INFO, 0, 0);
}

//-------------------------------------------------------------------------
//	功能：收到服务器通知有成员离开(包括自己离开)
//-------------------------------------------------------------------------
void KProtocolProcess::s2cLeaveTeam(BYTE* pMsg)
{
	if (!Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag)
	{
		Player[CLIENT_PLAYER_INDEX].m_cTeam.UpdateInterface();
		Player[CLIENT_PLAYER_INDEX].m_cTeam.UpdateamUI(); //显示主界面队伍信息
		return;
	}

	PLAYER_LEAVE_TEAM	*pLeaveTeam = (PLAYER_LEAVE_TEAM*)pMsg;

	// 自己离开
	if (pLeaveTeam->m_dwNpcID == Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_dwID)
	{
		Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag = 0;
		Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nApplyCaptainID = 0;
		g_Team[0].Release();
		if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetMenuState() == PLAYER_MENU_STATE_TEAMOPEN)
			Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SetMenuState(PLAYER_MENU_STATE_NORMAL);
	}
	// 别人离开
	else
	{
		g_Team[0].DeleteMember(pLeaveTeam->m_dwNpcID);
	}

	Player[CLIENT_PLAYER_INDEX].m_cTeam.UpdateInterface();
	Player[CLIENT_PLAYER_INDEX].m_cTeam.UpdateamUI(); //显示主界面队伍信息
	return;
}

//-------------------------------------------------------------------------
//	功能：收到服务器通知升级
//-------------------------------------------------------------------------
void KProtocolProcess::s2cLevelUp(BYTE* pMsg)
{
	Player[CLIENT_PLAYER_INDEX].s2cLevelUp(pMsg);
}
//客户端移动物品
void KProtocolProcess::s2cMoveItem(BYTE* pMsg)
{
	PLAYER_MOVE_ITEM_SYNC	*pMove = (PLAYER_MOVE_ITEM_SYNC*)pMsg;

	ItemPos	DownPos,UpPos;
	int     nIsMianBan;
	DownPos.nPlace    = pMove->m_btDownPos;
	DownPos.nX        = pMove->m_btDownX;
	DownPos.nY        = pMove->m_btDownY;
    DownPos.nDIsRongqi = pMove->m_DownRongqi;
	UpPos.nPlace      = pMove->m_btUpPos;
	UpPos.nX          = pMove->m_btUpX;
	UpPos.nY          = pMove->m_btUpY;
	UpPos.nSIsRongqi  = pMove->m_UpRongqi;

    nIsMianBan        = pMove->m_IsMianBan;

    //char msg[128];
	//sprintf(msg,"mb:%d,Cli:cpy:%d,cxy:%d,cyy:%d||cpd:%d,cxd:%d,cyd:%d",nIsMianBan,DownPos.nPlace,DownPos.nX,DownPos.nY,UpPos.nPlace,UpPos.nX,UpPos.nY);  
	//Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(msg);

	Player[CLIENT_PLAYER_INDEX].m_ItemList.ExchangeItem(&DownPos, &UpPos,nIsMianBan);
}

void KProtocolProcess::s2cRemoveItem(BYTE* pMsg)
{
	ITEM_REMOVE_SYNC	*pRemove = (ITEM_REMOVE_SYNC*)pMsg;

	int		nIdx;
	nIdx = Player[CLIENT_PLAYER_INDEX].m_ItemList.SearchID(pRemove->m_ID);
	if (nIdx > 0)
	{
		if (pRemove->m_model==1)
		{
		    Player[CLIENT_PLAYER_INDEX].m_ItemList.Remove(nIdx,pRemove->m_dx,pRemove->m_dy,1);
		}
		else
		{
            Player[CLIENT_PLAYER_INDEX].m_ItemList.Remove(nIdx,pRemove->m_dx,pRemove->m_dy);
		}
		
		Player[CLIENT_PLAYER_INDEX].m_ItemList.UnlockOperation();
	}

	
}

//-------------------------------------------------------------------------
//	功能：收到服务器发来的门派信息
//-------------------------------------------------------------------------
void KProtocolProcess::s2cSetFactionData(BYTE* pMsg)
{
	PLAYER_FACTION_DATA	*pData = (PLAYER_FACTION_DATA*)pMsg;

	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SetCamp(pData->m_btCamp);
	Player[CLIENT_PLAYER_INDEX].m_cFaction.m_nCurFaction = (char)pData->m_btCurFaction;
	Player[CLIENT_PLAYER_INDEX].m_cFaction.m_nFirstAddFaction = (char)pData->m_btFirstFaction;
	Player[CLIENT_PLAYER_INDEX].m_cFaction.m_nAddTimes = pData->m_nAddTimes;

	CoreDataChanged(GDCNI_PLAYER_BASE_INFO, 0, 0);

//	Player[CLIENT_PLAYER_INDEX].m_cFaction.m_sSkillOpen[0].m_nOpenLevel = pData->m_btNo1SkillOpenLevel;
//	Player[CLIENT_PLAYER_INDEX].m_cFaction.m_sSkillOpen[1].m_nOpenLevel = pData->m_btNo2SkillOpenLevel;
//	if (Player[CLIENT_PLAYER_INDEX].m_cFaction.m_nCurFaction == Player[CLIENT_PLAYER_INDEX].m_cFaction.m_sSkillOpen[0].m_nID)
//		Player[CLIENT_PLAYER_INDEX].m_cFaction.OpenCurSkillLevel(Player[CLIENT_PLAYER_INDEX].m_cFaction.m_sSkillOpen[0].m_nOpenLevel, &Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList);
//	else
//		Player[CLIENT_PLAYER_INDEX].m_cFaction.OpenCurSkillLevel(Player[CLIENT_PLAYER_INDEX].m_cFaction.m_sSkillOpen[1].m_nOpenLevel, &Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList);
}

//-------------------------------------------------------------------------
//	功能：收到服务器通知队伍状态(队长申请开放、关闭队伍状态时才会收到此信息)
//-------------------------------------------------------------------------
void KProtocolProcess::s2cSetTeamState(BYTE* pMsg)
{
	// 客户端数据有误，申请更新数据
	if (!Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag || Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFigure != TEAM_CAPTAIN)
	{
		Player[CLIENT_PLAYER_INDEX].ApplySelfTeamInfo();
		return;
	}

	PLAYER_TEAM_OPEN_CLOSE	*pTeamState = (PLAYER_TEAM_OPEN_CLOSE*)pMsg;
	Player[CLIENT_PLAYER_INDEX].m_cTeam.ReleaseList();
	if (pTeamState->m_btOpenClose)
	{
		g_Team[0].SetTeamOpen();

		KSystemMessage	sMsg;
		snprintf(sMsg.szMessage, sizeof(sMsg.szMessage), "%s", MSG_TEAM_OPEN);
		sMsg.eType = SMT_NORMAL;
		sMsg.byConfirmType = SMCT_NONE;
		sMsg.byPriority = 0;
		sMsg.byParamSize = 0;
		sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
	}
	else
	{
		g_Team[0].SetTeamClose();

		KSystemMessage	sMsg;
		snprintf(sMsg.szMessage, sizeof(sMsg.szMessage), "%s", MSG_TEAM_CLOSE);
		sMsg.eType = SMT_NORMAL;
		sMsg.byConfirmType = SMCT_NONE;
		sMsg.byPriority = 0;
		sMsg.byParamSize = 0;
		sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
	}
}

//-------------------------------------------------------------------------
//	功能：收到服务器通知队伍状态(队长申请开放、关闭队伍状态时才会收到此信息)
//-------------------------------------------------------------------------
void	KProtocolProcess::s2cApplyAddTeam(BYTE* pMsg)
{
	Player[CLIENT_PLAYER_INDEX].s2cApplyAddTeam(pMsg);
}

//-------------------------------------------------------------------------
//	功能：收到服务器发来的某个队伍的组队情况，显示之
//-------------------------------------------------------------------------
void KProtocolProcess::s2cShowTeamInfo(BYTE* pMsg)
{

}
//直接设置客户端空间
void	KProtocolProcess::s2csyncitemposition(BYTE* pMsg)
{

}


//客户端打开进度条界面
void KProtocolProcess::s2copenjindutiao(BYTE* pMsg)
{

    PLAYER_OPEN_JINDUTIAO *pJindu = (PLAYER_OPEN_JINDUTIAO*)pMsg;
/*
	char mag[64];
	sprintf(mag,"测试打开进度条成功");
    Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(mag);
*/
	KUiJinDuTiaoInfo JinDuData;
	snprintf(JinDuData.nTitle, sizeof(JinDuData.nTitle), "%s", pJindu->m_szTitle);
	snprintf(JinDuData.nCallFuc, sizeof(JinDuData.nCallFuc), "%s", pJindu->m_Callback);
	snprintf(JinDuData.szParma, sizeof(JinDuData.szParma), "%s", pJindu->m_szParma);
	snprintf(JinDuData.szParmb, sizeof(JinDuData.szParmb), "%s", pJindu->m_szParmb);
	snprintf(JinDuData.szParmc, sizeof(JinDuData.szParmc), "%s", pJindu->m_szParmc);
	JinDuData.nMsgIndex    = pJindu->m_MsgIndex;
	JinDuData.nTimes       = pJindu->m_inTime;

	CoreDataChanged(GDCNI_OPEN_JINDUTIAO, (unsigned int)&JinDuData,0);
}

//客户端接受 服务器返回的 任务号 和值
void KProtocolProcess::s2ctaskcallback(BYTE* pMsg)
{
    	PLAYER_GETBACK_TASKVAL	*pTaskback = (PLAYER_GETBACK_TASKVAL*)pMsg;

        Player[CLIENT_PLAYER_INDEX].m_cTask.SetSaveVal(pTaskback->m_TaskId,pTaskback->m_TaskVal);

}

//同步装备属性 客户端  --纹钢辨识,灵合属性装备
void KProtocolProcess::s2csyncequipitem(BYTE* pMsg)
{
	ITEM_SYNC *pItemSync = (ITEM_SYNC*)pMsg;

    int nIdx = Player[CLIENT_PLAYER_INDEX].m_ItemList.SearchID(pItemSync->m_ID);
	int i;
    if (nIdx>0)
	{  
	     for (i = 0; i < 6; i++)
		 {    
		   Item[nIdx].GetItemParam()->nRGeneratorLevel[i] = pItemSync->m_RongMagicLev[i];
//           Item[nIdx].GetItemParam()->nBGeneratorLevel[i] = pItemSync->m_BaoMagicLev[i];
		   
		 } 
	   
		  if (pItemSync->m_Luck > 10)  //幸运值
		  { 
			pItemSync->m_Luck = 10;
		  } 

		  if (pItemSync->m_Luck < 0)
		  {  
			pItemSync->m_Luck = 0;
		  }  

		   ///以下为镶套纹钢属性
		   KItemNormalAttrib	sMAR[6];// 魔法
		   ZeroMemory(sMAR,sizeof(sMAR)); //分配内存
		   KTabFile MagicTabR;
		   MagicTabR.Load(TABFILE_MAGICAGOLD_PATH);  //熔炼蓝装备魔法属性文件
		    
		   int nTypeR,nMaxR,nMinR;

		   g_RandomSeed(pItemSync->m_RandomSeed);

		   for (i = 0;i < 6;i++)
		   {//熔炼属性 或 辨识属性
			   if (Item[nIdx].GetItemParam()->nRGeneratorLevel[i] > 0 && i < pItemSync->m_Rongpiont)
			   { 
				   MagicTabR.GetInteger(Item[nIdx].GetItemParam()->nRGeneratorLevel[i],5,0,&nTypeR);
				   MagicTabR.GetInteger(Item[nIdx].GetItemParam()->nRGeneratorLevel[i],6,0,&nMaxR);
				   MagicTabR.GetInteger(Item[nIdx].GetItemParam()->nRGeneratorLevel[i],7,0,&nMinR);
				   sMAR[i].nAttribType = nTypeR;  //属性类型
				   //sMAR[i].nValue[0] = nMaxR + ((nMinR - nMaxR) * pItemSync->m_Luck / 10);
				   sMAR[i].nValue[0] = GetRandomNumber(nMaxR,nMinR);
				   MagicTabR.GetInteger(Item[nIdx].GetItemParam()->nRGeneratorLevel[i],8,0,&nMaxR);
				   MagicTabR.GetInteger(Item[nIdx].GetItemParam()->nRGeneratorLevel[i],9,0,&nMinR);
				   //sMAR[i].nValue[1] = nMaxR + ((nMinR - nMaxR) * pItemSync->m_Luck / 10);
				   sMAR[i].nValue[1] = GetRandomNumber(nMaxR,nMinR);

				   MagicTabR.GetInteger(Item[nIdx].GetItemParam()->nRGeneratorLevel[i],10,0,&nMaxR);
				   MagicTabR.GetInteger(Item[nIdx].GetItemParam()->nRGeneratorLevel[i],11,0,&nMinR);
				   //sMAR[i].nValue[2] = nMaxR + ((nMinR - nMaxR) * pItemSync->m_Luck / 10);
				   sMAR[i].nValue[2] = GetRandomNumber(nMaxR,nMinR);

				   continue;
			   }
			   sMAR[i].nAttribType = 0;
			   sMAR[i].nValue[0] = 0;
			   sMAR[i].nValue[1] = 0;
			   sMAR[i].nValue[2] = 0;
		   } 		    
		   Item[nIdx].SetAttrib_RON(sMAR);   //设置熔炼魔法属             
		   ZeroMemory(sMAR,sizeof(sMAR));    //清零

          /* for (i = 0;i < 6;i++)
		   {//宝石属性 
			   if (Item[nIdx].GetItemParam()->nBGeneratorLevel[i] > 0)
			   { 
				   MagicTabR.GetInteger(Item[nIdx].GetItemParam()->nBGeneratorLevel[i],5,0,&nTypeR);
				   MagicTabR.GetInteger(Item[nIdx].GetItemParam()->nBGeneratorLevel[i],6,0,&nMaxR);
				   MagicTabR.GetInteger(Item[nIdx].GetItemParam()->nBGeneratorLevel[i],7,0,&nMinR);
				   sMAR[i].nAttribType = nTypeR;  //属性类型
				   sMAR[i].nValue[0] = nMaxR + ((nMinR - nMaxR) * pItemSync->m_Luck / 10);
				   MagicTabR.GetInteger(Item[nIdx].GetItemParam()->nBGeneratorLevel[i],8,0,&nMaxR);
				   MagicTabR.GetInteger(Item[nIdx].GetItemParam()->nBGeneratorLevel[i],9,0,&nMinR);
				   sMAR[i].nValue[1] = nMaxR + ((nMinR - nMaxR) * pItemSync->m_Luck / 10);
				   MagicTabR.GetInteger(Item[nIdx].GetItemParam()->nBGeneratorLevel[i],10,0,&nMaxR);
				   MagicTabR.GetInteger(Item[nIdx].GetItemParam()->nBGeneratorLevel[i],11,0,&nMinR);
				   sMAR[i].nValue[2] = nMaxR + ((nMinR - nMaxR) * pItemSync->m_Luck / 10);
				   
				   continue;
			   }
			   sMAR[i].nAttribType = 0;
			   sMAR[i].nValue[0] = 0;
			   sMAR[i].nValue[1] = 0;
			   sMAR[i].nValue[2] = 0;
		   } 
		   Item[nIdx].SetAttrib_Bao(sMAR);   //设置熔炼魔法属
		   */
		   Item[nIdx].SetPoint(pItemSync->m_Point);

           if (Item[nIdx].IsBlue() && (pItemSync->m_IsWhere>0 || pItemSync->m_Rongpiont>0))   //已经辨识的蓝装 或者已经纹钢的蓝装
		   {  
                Item[nIdx].EnChanceRong(pItemSync->m_EnChance);   //蓝装强化
		   }
		   else if (Item[nIdx].IsBlue())   //全部的蓝装
                 Item[nIdx].EnChance(pItemSync->m_EnChance);      //蓝装强化
		   else if (Item[nIdx].IsPurple())
		   {//紫装
				   for (i = 0; i < 6; ++i)
				   {
					   Item[nIdx].GetItemParam()->nGeneratorLevel[i] = pItemSync->m_MagicLevel[i];
				   }
				   
				   int nLucky=pItemSync->m_Luck;
				   
				   KItemNormalAttrib sMz[6];
				   KTabFile tabMagicAttribe;
				   tabMagicAttribe.Load(TABFILE_MAGICATTRIB_PATH);
				   
				   int nType,nMax,nMin;
				   for (i = 0;i < 6;++i)
				   {
					   if (Item[nIdx].GetItemParam()->nGeneratorLevel[i] > 0 && i < pItemSync->m_Point)
					   {
						   tabMagicAttribe.GetInteger(Item[nIdx].GetItemParam()->nGeneratorLevel[i],5,0,&nType);
						   tabMagicAttribe.GetInteger(Item[nIdx].GetItemParam()->nGeneratorLevel[i],6,0,&nMax);
						   tabMagicAttribe.GetInteger(Item[nIdx].GetItemParam()->nGeneratorLevel[i],7,0,&nMin);
						   sMz[i].nAttribType = nType;
						   //sMz[i].nValue[0] = nMax + ((nMin - nMax) * nLucky / 10);
						   sMz[i].nValue[0] = GetRandomNumber(nMax,nMin);
						   
						   tabMagicAttribe.GetInteger(Item[nIdx].GetItemParam()->nGeneratorLevel[i],8,0,&nMax);
						   tabMagicAttribe.GetInteger(Item[nIdx].GetItemParam()->nGeneratorLevel[i],9,0,&nMin);
						   //sMz[i].nValue[1] = nMax + ((nMin - nMax) * nLucky / 10);
						   sMz[i].nValue[1] = GetRandomNumber(nMax,nMin);

						   tabMagicAttribe.GetInteger(Item[nIdx].GetItemParam()->nGeneratorLevel[i],10,0,&nMax);
						   tabMagicAttribe.GetInteger(Item[nIdx].GetItemParam()->nGeneratorLevel[i],11,0,&nMin);
						   //sMz[i].nValue[2] = nMax + ((nMin - nMax) * nLucky / 10);
						   sMz[i].nValue[2] = GetRandomNumber(nMax,nMin);
						   continue;
					   }

					   
					   sMz[i].nAttribType = 0;
					   sMz[i].nValue[0] = 0;
					   sMz[i].nValue[1] = 0;
					   sMz[i].nValue[2] = 0;
				   }
				   Item[nIdx].SetAttrib_MA(sMz);
				   tabMagicAttribe.Clear();
				   Item[nIdx].EnChance(pItemSync->m_EnChance);
		   }

		   if (Item[nIdx].GetGoldId()/* && pItemSync->m_Rongpiont>0*/)
		   {
		       Item[nIdx].EnChanceRong(pItemSync->m_EnChance);  //熔炼强化
		       Item[nIdx].EnChanceBao(pItemSync->m_EnChance);   //宝石强化
		   }

		   MagicTabR.Clear();

		   Item[nIdx].SetStackNum(pItemSync->m_StackNum);
		   Item[nIdx].SetRPoint(pItemSync->m_Rongpiont);
		   Item[nIdx].SetIsWhere(pItemSync->m_IsWhere);
		   Item[nIdx].SetSeries(pItemSync->m_Series);
		   Item[nIdx].SetBang(pItemSync->m_IsBang);
	       Item[nIdx].SetKuaiJie(pItemSync->m_IsKuaiJie);
		   Item[nIdx].SetIsCanUse(pItemSync->m_IsCanUse);
           Item[nIdx].SetTime(pItemSync->m_Time.bYear,pItemSync->m_Time.bMonth,pItemSync->m_Time.bDay,pItemSync->m_Time.bHour,pItemSync->m_Time.bMin);
           Item[nIdx].SetDurability(pItemSync->m_Durability);  // 同步持久度

           Item[nIdx].SetMagic(pItemSync->m_IsMagic);          //是否有魔法属性
	       Item[nIdx].SetSkillType(pItemSync->m_SkillType);
	       Item[nIdx].SetMagicID(pItemSync->m_MagicID);

		   Item[nIdx].SetUseMapIdx(pItemSync->m_UseMap);
		   Item[nIdx].SetUseKind(pItemSync->m_UseKind);
		   Item[nIdx].SetItemResIdx(pItemSync->m_Res);
           
		   Item[nIdx].SetID(pItemSync->m_ID);             //设置DwID
		   if (pItemSync->m_ItmeInfo[0])
		      Item[nIdx].SetItmeInfo(pItemSync->m_ItmeInfo);
   }		   
	//解锁客户端
	       Player[CLIENT_PLAYER_INDEX].m_ItemList.UnlockOperation();	
}

//同步装备属性 客户端产生装备  --刷装备
void KProtocolProcess::s2cSyncItem(BYTE* pMsg)
{
	ITEM_SYNC	*pItemSync = (ITEM_SYNC*)pMsg;
	int pnMagicParam[6],nIndex,pnRMagicParam[6],pnJBParam[7];  //pnBMagicParam[6],
	int i;
	for (i = 0; i < 6; i++)
	{
		pnMagicParam[i]  = pItemSync->m_MagicLevel[i];
		pnRMagicParam[i] = pItemSync->m_RongMagicLev[i];
//      pnBMagicParam[i] = pItemSync->m_BaoMagicLev[i];

      if (pItemSync->m_Point==8)
	  {//随机黄金 

	  }
	}
	for (i = 0; i < 7; i++)
	{
        pnJBParam[i]     = pItemSync->m_JBLevel[i];
	}

	if (pItemSync->m_GoldId<=0)
		nIndex = ItemSet.AddCilentItem(\
			pItemSync->m_Genre,\
			pItemSync->m_Series,\
			pItemSync->m_Level,\
			pItemSync->m_Luck,\
			pItemSync->m_Detail,\
			pItemSync->m_Particur,\
			pnMagicParam,\
			pItemSync->m_Version,\
			pItemSync->m_RandomSeed,\
			pItemSync->m_StackNum,\
			pItemSync->m_EnChance,\
			pItemSync->m_Point,\
			pItemSync->m_Time.bYear,\
			pItemSync->m_Time.bMonth,\
			pItemSync->m_Time.bDay,\
			pItemSync->m_Time.bHour,\
			pItemSync->m_Time.bMin,\
			pItemSync->m_Rongpiont,\
			pnRMagicParam,\
			pnJBParam,\
			NULL,\
			NULL,\
			NULL,\
			NULL,pItemSync->m_IsWhere);
	else if (pItemSync->m_GoldId>0)
	{  
	  if (pItemSync->m_Point==8)
	   nIndex = ItemSet.AddRondomGold(
	     pItemSync->m_GoldId,\
	     pnMagicParam,\
	     pItemSync->m_Series,\
	     pItemSync->m_EnChance,\
	     pItemSync->m_Rongpiont,\
	     pItemSync->m_Time.bYear,\
	     pItemSync->m_Time.bMonth,\
	     pItemSync->m_Time.bDay,\
	     pItemSync->m_Time.bHour,\
	     pItemSync->m_Time.bMin,\
	     pnRMagicParam,\
	     pItemSync->m_Level,pItemSync->m_Luck,pItemSync->m_Detail,-1,1,0,pItemSync->m_IsPlatima,FALSE,pItemSync->m_RandomSeed);
	  else
      nIndex = ItemSet.AddGold(
		pItemSync->m_GoldId,\
		pnMagicParam,\
		pItemSync->m_Series,\
		pItemSync->m_EnChance,\
        pItemSync->m_Rongpiont,\
		pItemSync->m_Time.bYear,\
		pItemSync->m_Time.bMonth,\
		pItemSync->m_Time.bDay,\
		pItemSync->m_Time.bHour,\
        pItemSync->m_Time.bMin,\
        pnRMagicParam,\
		NULL,\
		pItemSync->m_Level,1,-1,-1,1,0,pItemSync->m_IsPlatima,FALSE,pItemSync->m_RandomSeed);
	}
	/*
	   AddGold(IN int nId , IN int* pnMagicLevel , \
	   IN int nSeries,IN int nEnChance,\
	   IN int nRongpiont, \
	   int nYear, int nMonth, int nDay, int nHour,int nMin,\
	   IN int* pnRMagicLevel,IN int* pnBMagicLevel,\
	   IN int nLevel,IN int nLuck, IN int nDetailType, \
	   IN int nParticular,IN int nStackNum,IN int nVersion,int nIsPlatina)

	*/

	if (nIndex > 0)
	{//设置客户端各种信息
		Item[nIndex].SetStackNum(pItemSync->m_StackNum);         //捡起东西 并设置可叠数量
		//pItemSync->m_IsBang     = Item[nIndex].GetIsBang();      //是否绑定
		//pItemSync->m_IsKuaiJie  = Item[nIndex].GetIsKuaiJie();   //是否快捷栏
		//pItemSync->m_IsMagic    = Item[nIndex].GetIsMagic();
		//pItemSync->m_SkillType  = Item[nIndex].GetSkillType();
		//pItemSync->m_MagicID    = Item[nIndex].GetMagicID();
		Item[nIndex].SetLockItem(&pItemSync->m_LockItem);
		Item[nIndex].SetBang(pItemSync->m_IsBang);
		Item[nIndex].SetKuaiJie(pItemSync->m_IsKuaiJie);
		Item[nIndex].SetMagic(pItemSync->m_IsMagic);
		Item[nIndex].SetSkillType(pItemSync->m_SkillType);
		Item[nIndex].SetMagicID(pItemSync->m_MagicID);
        Item[nIndex].SetIsCanUse(pItemSync->m_IsCanUse); //客户端设置是否可以使用
		Item[nIndex].SetUseMapIdx(pItemSync->m_UseMap);
		Item[nIndex].SetItemResIdx(pItemSync->m_Res);
		Item[nIndex].SetUseKind(pItemSync->m_UseKind);
		//if (strrchr(Item[nIndex].GetItmeInfo(),'.lua'))
		//{
		if (pItemSync->m_ItmeInfo[0])
		   Item[nIndex].SetItmeInfo(pItemSync->m_ItmeInfo);
		//}

        //sprintf(pItemSync->m_WonName,"%s",Item[nIndex].GetWonName());  //所有者
		Item[nIndex].SetWonName("系统");//pItemSync->m_WonName
		Item[nIndex].SetID(pItemSync->m_ID);  //设置DwID
		Item[nIndex].SetDurability(pItemSync->m_Durability);  // 同步持久度
		//客户端增减装备
        Player[CLIENT_PLAYER_INDEX].m_Autoplay.nShiQuNum=0;
		Player[CLIENT_PLAYER_INDEX].m_ItemList.Add(nIndex, pItemSync->m_btPlace, pItemSync->m_btX, pItemSync->m_btY,0,0,pItemSync->m_StackNum,pItemSync->m_IsLoign);
        //----------------外挂过滤装备设置----------------------------------------
		int k,nRows,nYval,nDvala,nDvalb,nDvalc;
		char nItemName[64]={0},nToolName[64]={0};

	                     //属性的数量
		int nSetNun=Player[CLIENT_PLAYER_INDEX].m_Autoplay.nShuXingNum;

        if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_AutoplayId==1 && nSetNun>0 && nSetNun<7)
        {//如果是挂机
			char szTempDesc[128]={0};
              //sprintf(msg,"外挂开启:%d",nSetNun);
			  //Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(msg);

		   if (Item[nIndex].GetGenre()==item_equip) 
		   {//蓝装装备过滤

		        int nSxNum=Item[nIndex].GetMagicCount();  

			    if  (nSxNum<nSetNun)
				 {
				   Player[CLIENT_PLAYER_INDEX].m_ItemList.UnlockOperation();
			       Player[CLIENT_PLAYER_INDEX].AutoAwayItem(pItemSync->m_ID);
				   return;
				 }
				   
		        /*KTabFile nEquipItem,nToolItem;
		          if(!nEquipItem.Load("\\FsAi\\EquipName.txt"))  //装备属性列表
		          {
		              Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("装备拾取设置有误,请联系GM处理!");
			          return;
		          }

                  if(!nToolItem.Load("\\FsAi\\itemName.txt"))  //装备属性列表
		          {
		              Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("装备道具设置有误,请联系GM处理!");
			          return;
		          }*/

		      int nIsHave=0; 

		      if ((Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoEques==1 || Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoMoneyAndEques==1))
			  {
			   
                 nRows=Player[CLIENT_PLAYER_INDEX].nEquipItem.GetHeight();

                 if (nRows>12)
				     nRows=12;

	             for(i=0;i<nRows;i++)
	             {     	 
		                Player[CLIENT_PLAYER_INDEX].nEquipItem.GetString(i+1,2,"无数据",nItemName,sizeof(nItemName));  //属性关键字
						Player[CLIENT_PLAYER_INDEX].nEquipItem.GetInteger(i+1,3,3,&nYval);  
 
						 for (k=0;k<nSxNum;k++)
		                 {
                         
		                   const char	*pszKeyName = g_MagicID2String(Item[nIndex].GetAttribType(k));
						   nDvala =  Item[nIndex].GetAttribTypeVala(k);
						   nDvalb =  Item[nIndex].GetAttribTypeValb(k);
						   nDvalc =  Item[nIndex].GetAttribTypeValc(k);

                           //KIniFile	m_FileDesc;
                           // if (m_FileDesc.Load("\\settings\\MagicDesc.Ini"))
							{
			                  Player[CLIENT_PLAYER_INDEX].m_FileDesc.GetString("Descript",pszKeyName, "无数据", szTempDesc, sizeof(szTempDesc));
							}
							//m_File.Clear();

							if (strstr(szTempDesc,nItemName) && nYval>0 && nDvala>=nYval/* || nDvalb>=nYval || nDvalc>=nYval*/)
							{//如果有这个关键字的属性  
							    nIsHave++;
							}
							else if (strstr(szTempDesc,nItemName) && nYval<=0)
							{
							    nIsHave++;
							}

	                     }  
                 }

                 /*if (nIsHave>0)
			  	 {//如果是这个装备,就检测名字是否相
				    nRows=nToolItem.GetHeight();
                    int isThis=0;
                    if (nRows>12)
				        nRows=12;

	                  for(i=0;i<nRows;i++)
	                  {     	 
		                   nToolItem.GetString(i+1,2,"无数据",nToolName,sizeof(nToolName)); 
						 
							if (strstr(Item[nIndex].GetName(),nToolName))
							{//如果有这个关键字的属性
							    isThis++;
								break;
							}
	                   } 
					   if (isThis==0)
					       nIsHave=0;		     
                 }*/
			          
			}
	
		    if (Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoOther==1)
			{//如果开启 指定装备
               if (nIsHave>0)
			  	 {//如果是这个装备,就检测名字是否相
				    nRows=Player[CLIENT_PLAYER_INDEX].nToolItem.GetHeight();
                    int isThis=0;
                    if (nRows>12)
				        nRows=12;

	                  for(i=0;i<nRows;i++)
	                  {     	 
		                   Player[CLIENT_PLAYER_INDEX].nToolItem.GetString(i+1,2,"无数据",nToolName,sizeof(nToolName)); 
						 
							if (strstr(Item[nIndex].GetName(),nToolName))
							{//如果有这个关键字的属性
							    isThis++;
								break;
							}
	                   } 
					   if (isThis==0)
					       nIsHave=0;		     
                 }
			}
/*			{//指定装备
                nRows=nEquipItem.GetHeight();

                 if (nRows>12)
				     nRows=12;

	             for(i=0;i<nRows;i++)
	             {     	 
		                nEquipItem.GetString(i+1,2,"无数据",nItemName,sizeof(nItemName)); 
						nEquipItem.GetInteger(i+1,3,3,&nYval);  

						 for (k=0;k<nSxNum;k++)
		                 {
                           char msg[128],szTempDesc[128];
		                   const char	*pszKeyName = g_MagicID2String(Item[nIndex].GetAttribType(k));
						   nDvala =  Item[nIndex].GetAttribTypeVala(k);
						   nDvalb =  Item[nIndex].GetAttribTypeValb(k);
						   nDvalc =  Item[nIndex].GetAttribTypeValc(k);
                           KIniFile	m_File;
                            if (m_File.Load("\\settings\\MagicDesc.Ini"))
							{
			                  m_File.GetString("Descript",pszKeyName, "无数据", szTempDesc, sizeof(szTempDesc));
			                  m_File.Clear();
			                  //sprintf(msg,"物品(%d),属性%d(%d):%s",nIndex,k,Item[nIndex].GetAttribType(k),szTempDesc);
			                  //Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(msg);
							}
							if (strstr(szTempDesc,nItemName) && nYval>0 &&  (nDvala>=nYval || nDvalb>=nYval || nDvalc>=nYval))
							{//如果有这个关键字的属性
							    nIsHave++;
							}
							else if (strstr(szTempDesc,nItemName) && nYval<=0)
							{
							    nIsHave++;
							}

	                     }  
                 }
	             //nEquipItem.Clear();
                 if (nIsHave>0)
			  	 {//如果是这个装备,就检测名字是否相
				    nRows=nToolItem.GetHeight();
                    int isThis=0;
                    if (nRows>12)
				        nRows=12;

	                  for(i=0;i<nRows;i++)
	                  {     	 
		                   nToolItem.GetString(i+1,2,"无数据",nToolName,sizeof(nToolName)); 
						 
							if (strstr(Item[nIndex].GetName(),nToolName))
							{//如果有这个关键字的属性
							    isThis++;
								break;
							}
	                   } 
					   if (isThis==0)
					       nIsHave=0;		     
                 }
			}
*/
            if (nIsHave==0)
			{//如果不符合这个装备的 就丢弃
               Player[CLIENT_PLAYER_INDEX].m_ItemList.UnlockOperation();
			   Player[CLIENT_PLAYER_INDEX].AutoAwayItem(pItemSync->m_ID);
			   //nEquipItem.Clear();
			   //nToolItem.Clear();
			   return;
			}

		    //nEquipItem.Clear();
			//nToolItem.Clear();
		   }
/*
		   if (Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoTool==1)
		   {//道具栏的装备过滤
		      if (Item[nIndex].GetGenre()==item_equip)
			  {//是装备 就开始检测

			  }
		   }*/
		}
//---------------------------以下为测试信息---------------------------
        int nblue,ngold,nzise;
           nblue=Item[nIndex].IsBlue();
           nzise=Item[nIndex].IsPurple();
           ngold=Item[nIndex].GetGoldId();

		   char sMsg[125]={0};
		   if (ngold>0)
			   sprintf(sMsg, "Nhan: (<c=yellow>%sx%d<c>)!", Item[nIndex].GetName(),Item[nIndex].GetStackNum());
           else if (nblue==1)
			   sprintf(sMsg, "Nhan: (<c=blue>%sx%d<c>)!", Item[nIndex].GetName(),Item[nIndex].GetStackNum());
		   else if (nzise>0)
			   sprintf(sMsg, "Nhan: (<c=Pink>%sx%d<c>)!", Item[nIndex].GetName(),Item[nIndex].GetStackNum());
		   else
			   sprintf(sMsg, "Nhan: (<c=white>%sx%d<c>)!", Item[nIndex].GetName(),Item[nIndex].GetStackNum());
		   Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(sMsg);
//------------------------------------------------------------------------		   
	}
	//解锁客户端
	Player[CLIENT_PLAYER_INDEX].m_ItemList.UnlockOperation();
}
//-------------------------------------------------------------------------
//	功能：收到服务器发过来的同步money的消息
//-------------------------------------------------------------------------
void KProtocolProcess::s2cSyncMoney(BYTE* pMsg)
{
	Player[CLIENT_PLAYER_INDEX].s2cSyncMoney(pMsg);
}
void KProtocolProcess::s2cSyncXu(BYTE* pMsg)
{
	Player[CLIENT_PLAYER_INDEX].s2cSyncXu(pMsg);
}

void KProtocolProcess::s2cSyncRoleList(BYTE* pMsg)
{
	ROLE_LIST_SYNC	*pSync = (ROLE_LIST_SYNC *)pMsg;
	// TODO: 通过CORESHELL交给界面？
}

//-------------------------------------------------------------------------
//	功能：收到服务器通知有新人加入队伍
//-------------------------------------------------------------------------
void KProtocolProcess::s2cTeamAddMember(BYTE* pMsg)
{
	if (!Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag )
	{
		Player[CLIENT_PLAYER_INDEX].ApplySelfTeamInfo();
		return;
	}

	PLAYER_TEAM_ADD_MEMBER	*pAddMem = (PLAYER_TEAM_ADD_MEMBER*)pMsg;
	int	nMemNum;

	nMemNum = g_Team[0].FindFree();
	if (nMemNum < 0)
		return;
	g_Team[0].m_nMember[nMemNum] = pAddMem->m_dwNpcID;
	g_Team[0].m_nMemLevel[nMemNum + 1] = (WORD)pAddMem->m_btLevel;
	snprintf(g_Team[0].m_szMemName[nMemNum + 1], sizeof(g_Team[0].m_szMemName[nMemNum + 1]), "%s", pAddMem->m_szName);
	g_Team[0].m_nMemNum++;
	Player[CLIENT_PLAYER_INDEX].m_cTeam.DeleteOneFromApplyList(pAddMem->m_dwNpcID);
	Player[CLIENT_PLAYER_INDEX].m_cTeam.UpdateInterface();
	Player[CLIENT_PLAYER_INDEX].m_cTeam.UpdateamUI(); //显示主界面队伍信息

	KSystemMessage	sMsg;
	sprintf(sMsg.szMessage, MSG_TEAM_ADD_MEMBER, pAddMem->m_szName);
	sMsg.eType = SMT_NORMAL;
	sMsg.byConfirmType = SMCT_NONE;
	sMsg.byPriority = 0;
	sMsg.byParamSize = 0;
	sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
	CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
}

//-------------------------------------------------------------------------
//	功能：收到服务器通知更换队长
//-------------------------------------------------------------------------
void KProtocolProcess::s2cTeamChangeCaptain(BYTE* pMsg)
{
	if (!Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag)
	{
		Player[CLIENT_PLAYER_INDEX].ApplySelfTeamInfo();
		return;
	}

	int		nLevel, nMemNo;
	char	szName[32]={0};
	PLAYER_TEAM_CHANGE_CAPTAIN	*pChange = (PLAYER_TEAM_CHANGE_CAPTAIN*)pMsg;

	nMemNo = g_Team[0].FindMemberID(pChange->m_dwCaptainID);
	// 当前队伍数据有误
	if ((DWORD)g_Team[0].m_nCaptain != pChange->m_dwMemberID || nMemNo < 0)
	{
		Player[CLIENT_PLAYER_INDEX].ApplySelfTeamInfo();
		return;
	}

	// 自己被任命为队长
	if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_dwID == pChange->m_dwCaptainID)
	{
		KSystemMessage	sMsg;
		sprintf(sMsg.szMessage, MSG_TEAM_CHANGE_CAPTAIN_SELF, g_Team[0].m_szMemName[0]);
		sMsg.eType = SMT_TEAM;
		sMsg.byConfirmType = SMCT_UI_TEAM;
		sMsg.byPriority = 3;
		sMsg.byParamSize = 0;
		sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
	}
	else	// 别人被任命为队长
	{
		KSystemMessage	sMsg;
		sprintf(sMsg.szMessage, MSG_TEAM_CHANGE_CAPTAIN, g_Team[0].m_szMemName[nMemNo + 1]);
		sMsg.eType = SMT_TEAM;
		sMsg.byConfirmType = SMCT_UI_TEAM;
		sMsg.byPriority = 3;
		sMsg.byParamSize = 0;
		sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
	}

	// 更换人员
	nLevel = g_Team[0].m_nMemLevel[0];
	snprintf(szName, sizeof(szName), "%s", g_Team[0].m_szMemName[0]);
	g_Team[0].m_nCaptain = pChange->m_dwCaptainID;
	g_Team[0].m_nMemLevel[0] = g_Team[0].m_nMemLevel[nMemNo + 1];
	strcpy(g_Team[0].m_szMemName[0], g_Team[0].m_szMemName[nMemNo + 1]);
	g_Team[0].m_nMember[nMemNo] = pChange->m_dwMemberID;
	g_Team[0].m_nMemLevel[nMemNo + 1] = nLevel;
	snprintf(g_Team[0].m_szMemName[nMemNo + 1], sizeof(g_Team[0].m_szMemName[nMemNo + 1]), "%s", szName);

	if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_dwID == pChange->m_dwCaptainID)
	{
		Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFigure = TEAM_CAPTAIN;
		Player[CLIENT_PLAYER_INDEX].m_cTeam.ReleaseList();
	}
	else if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_dwID == pChange->m_dwMemberID)
	{
		Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFigure = TEAM_MEMBER;
		Player[CLIENT_PLAYER_INDEX].m_cTeam.ReleaseList();
	}

	Player[CLIENT_PLAYER_INDEX].m_cTeam.UpdateInterface();
	Player[CLIENT_PLAYER_INDEX].m_cTeam.UpdateamUI(); //显示主界面队伍信息
}

//-------------------------------------------------------------------------
//	功能：收到服务器发来的自己队伍的组队情况，更新相应信息
//-------------------------------------------------------------------------
void KProtocolProcess::s2cUpdataSelfTeamInfo(BYTE* pMsg)
{
	int		i;
	PLAYER_SEND_SELF_TEAM_INFO	*pSelfInfo = (PLAYER_SEND_SELF_TEAM_INFO*)pMsg;

	// 数据包出错
	if (pSelfInfo->m_dwNpcID[0] == 0)
		return;

	Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag = 1;
	Player[CLIENT_PLAYER_INDEX].m_dwLeadExp = pSelfInfo->m_dwLeadExp;
	Player[CLIENT_PLAYER_INDEX].m_dwLeadLevel = PlayerSet.m_cLeadExp.GetLevel(Player[CLIENT_PLAYER_INDEX].m_dwLeadExp);
	Player[CLIENT_PLAYER_INDEX].m_dwNextLevelLeadExp = PlayerSet.m_cLeadExp.GetLevelExp(Player[CLIENT_PLAYER_INDEX].m_dwLeadLevel);
	if (Player[CLIENT_PLAYER_INDEX].m_dwNextLevelLeadExp == 0)
		Player[CLIENT_PLAYER_INDEX].m_dwNextLevelLeadExp = Player[CLIENT_PLAYER_INDEX].m_dwLeadLevel;

	if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_dwID == pSelfInfo->m_dwNpcID[0])	// 队长
	{
		Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFigure = TEAM_CAPTAIN;
	}
	else													// 队员
	{
		Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFigure = TEAM_MEMBER;
		for (i = 0; i < MAX_TEAM_APPLY_LIST; i++)
			Player[CLIENT_PLAYER_INDEX].m_cTeam.m_sApplyList[i].Release();
	}
	g_Team[0].m_nCaptain = pSelfInfo->m_dwNpcID[0];
	if (pSelfInfo->m_btState == 0)
		g_Team[0].SetTeamClose();
	else
		g_Team[0].SetTeamOpen();

	g_Team[0].m_nTeamServerID = pSelfInfo->nTeamServerID;	  //队伍在服务器中的 唯一id
	g_Team[0].m_nMemLevel[0] = (WORD)pSelfInfo->m_btLevel[0];
	strcpy(g_Team[0].m_szMemName[0], pSelfInfo->m_szNpcName[0]);
	g_Team[0].m_nMemNum = 0;
	for (i = 0; i < MAX_TEAM_MEMBER; i++)
	{
		if (pSelfInfo->m_dwNpcID[i + 1] > 0)
		{
			g_Team[0].m_nMember[i] = pSelfInfo->m_dwNpcID[i + 1];         //队员信息
			g_Team[0].m_nMemLevel[i + 1] = (WORD)pSelfInfo->m_btLevel[i + 1];
			strcpy(g_Team[0].m_szMemName[i + 1], pSelfInfo->m_szNpcName[i + 1]);
			g_Team[0].m_nMemNum++;
		}
		else
		{
			g_Team[0].m_nMember[i] = -1;
			g_Team[0].m_nMemLevel[i + 1] = 0;
			g_Team[0].m_szMemName[i + 1][0] = 0;
		}
	}
	Player[CLIENT_PLAYER_INDEX].m_cTeam.UpdateInterface();
	Player[CLIENT_PLAYER_INDEX].m_cTeam.UpdateamUI(); //显示主界面队伍信息
}
//同步基本数据
void KProtocolProcess::SyncCurNormalData(BYTE* pMsg)
{
	CURPLAYER_NORMAL_SYNC	*pSync = (CURPLAYER_NORMAL_SYNC*)pMsg;
	if (pSync->m_shLife > 0)
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentLife = pSync->m_shLife;
	else
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentLife = 0;
	if (pSync->m_shStamina > 0)
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentStamina = pSync->m_shStamina;
	else
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentStamina = 0;

	if (pSync->m_shMana > 0)
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentMana = pSync->m_shMana;
	else
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentMana = 0;

	//if (pSync->m_shAngry > 0)//当前怒值
	//	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentNuQi = pSync->m_shAngry;
	//else
	//	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentNuQi = 0; //怒气
///////////////////////////////
	if (pSync->m_shLifeMax > 0)
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentLifeMax = pSync->m_shLifeMax;
	else
        Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentLifeMax = 100;

	if (pSync->m_shStaminaMax > 0)
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentStaminaMax = pSync->m_shStaminaMax;

	if (pSync->m_shManaMax > 0)
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentManaMax = pSync->m_shManaMax;
	else
        Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentManaMax = 100;

	if (pSync->m_shAngryMax > 0)  //怒气最大值
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentNuQiMax = pSync->m_shAngryMax;

//	if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Doing == do_sit)
//	{
//		if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentLife >= Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentLifeMax &&
//			Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentMana >= Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentManaMax)
//		{
//			Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SendCommand(do_stand);
//		}
//	}
//当前的时间帧数
	Player[CLIENT_PLAYER_INDEX].m_nForbiddenFlag           = pSync->m_nForbiddenFlag;
	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
	switch (pSync->m_btTeamData)
	{
	case 0x00:
		if ( Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag )
		{
			Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag = 0;
			Player[CLIENT_PLAYER_INDEX].ApplySelfTeamInfo();
		}
		break;
	case 0x03:
		if ( !Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag || Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFigure != TEAM_CAPTAIN)
		{
			Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag = 1;
			Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFigure = TEAM_CAPTAIN;
			Player[CLIENT_PLAYER_INDEX].ApplySelfTeamInfo();
		}
		break;
	case 0x01:
		if ( !Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag || Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFigure != TEAM_MEMBER)
		{
			Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag = 1;
			Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFigure = TEAM_MEMBER;
			Player[CLIENT_PLAYER_INDEX].ApplySelfTeamInfo();
		}
		break;
	}
}
//上线同步客户端	人物 参数
void KProtocolProcess::SyncCurPlayer(BYTE* pMsg)
{
	Player[CLIENT_PLAYER_INDEX].SyncCurPlayer(pMsg);
}
//怪物同步数据库资料=--客户端
void KProtocolProcess::SyncNpc(BYTE* pMsg)
{
	NPC_SYNC* NpcSync = (NPC_SYNC *)pMsg;

/*
	int nRegion = SubWorld[0].FindRegion(NpcSync->RegionID);
	// 如果Region不存在，说明这个包可能有问题，不处理了。
	if (nRegion == -1)
		return;
*/
	int nRegion, nMapX, nMapY, nOffX, nOffY;
	SubWorld[0].Mps2Map(NpcSync->MapX, NpcSync->MapY, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);

	if (nRegion == -1)
		return;

	int nIdx = NpcSet.SearchID(NpcSync->ID);

	if (!nIdx)
	{//2次同步的  如果没有 就增加了
		nIdx = NpcSet.AddServerNpcA(NpcSync->NpcSettingIdx, 0, NpcSync->MapX, NpcSync->MapY,-1,0,0,0,0,0);
		Npc[nIdx].m_dwID = NpcSync->ID;
		Npc[nIdx].m_Kind = NpcSync->m_btKind;
		Npc[nIdx].m_Height = 0;
		
		if (NpcSet.IsNpcRequestExist(NpcSync->ID))
			NpcSet.RemoveNpcRequest(NpcSync->ID);
	}
	else 
	{
		if (Npc[nIdx].m_RegionIndex >= 0)
			SubWorld[0].m_Region[Npc[nIdx].m_RegionIndex].DecNpcRef(Npc[nIdx].m_MapX, Npc[nIdx].m_MapY);

		Npc[nIdx].m_MapX = nMapX;
		Npc[nIdx].m_MapY = nMapY;
		Npc[nIdx].m_OffX = nOffX;
		Npc[nIdx].m_OffY = nOffY;

		Npc[nIdx].m_NpcSettingIdx = (short)HIWORD(NpcSync->NpcSettingIdx);
		Npc[nIdx].m_Level = LOWORD(NpcSync->NpcSettingIdx);

		if (Npc[nIdx].m_RegionIndex >= 0)
		{
			if (Npc[nIdx].m_RegionIndex != nRegion)
			{
				SubWorld[0].NpcChangeRegion(SubWorld[0].m_Region[Npc[nIdx].m_RegionIndex].m_RegionID, SubWorld[0].m_Region[nRegion].m_RegionID, nIdx);
				Npc[nIdx].m_RegionIndex = nRegion;
			}
			//SubWorld[0].m_Region[Npc[nIdx].m_RegionIndex].AddRef(Npc[nIdx].m_MapX, Npc[nIdx].m_MapY, obj_npc);
			SubWorld[0].m_Region[Npc[nIdx].m_RegionIndex].AddNpcRef(Npc[nIdx].m_MapX,Npc[nIdx].m_MapY);
		}
		else
		{
			SubWorld[0].m_Region[nRegion].AddNpc(nIdx);
			Npc[nIdx].m_RegionIndex = nRegion;
			//SubWorld[0].m_Region[Npc[nIdx].m_RegionIndex].AddRef(Npc[nIdx].m_MapX, Npc[nIdx].m_MapY, obj_npc);
			SubWorld[0].m_Region[Npc[nIdx].m_RegionIndex].AddNpcRef(Npc[nIdx].m_MapX,Npc[nIdx].m_MapY);
		}

	}

	Npc[nIdx].m_Camp			= (NPCCAMP)NpcSync->Camp;
	Npc[nIdx].m_CurrentCamp		= (NPCCAMP)NpcSync->CurrentCamp;
	Npc[nIdx].m_Series			= NpcSync->m_bySeries;
//	Npc[nIdx].m_NpcEnchant		= NpcSync->NpcEnchant;

     Npc[nIdx].m_CurrentdanggeRate		= NpcSync->Dangge; //档格
     Npc[nIdx].m_CurrentzhongjiRate   	= NpcSync->Zhongji;
//	 Npc[nIdx].m_CJtaskID               =NpcSync->CjtaskID;

     //sprintf(Npc[nIdx].m_GuishuName,"%s",NpcSync->GuishuName);          //同伴归属
	 // Npc[nIdx].SetstrInfo(STR_GUISHU_NAME,NpcSync->GuishuName);
//    if (Npc[nIdx].m_Kind!=kind_player)
	  Npc[nIdx].m_cGold.SetGoldCurrentType(NpcSync->NpcEnchant);      //同步BOSS怪物类型

	/*if (NpcSync->LifePerCent <= 128)//右移动7位
		Npc[nIdx].m_CurrentLife	=(Npc[nIdx].m_CurrentLifeMax * NpcSync->LifePerCent) >> 7 ;
	else
		Npc[nIdx].m_CurrentLife	= 0;*/

	  if (NpcSync->nLifeYu >=0 && Npc[nIdx].m_CurrentLifeMax > 0)
	  {
		  Npc[nIdx].m_CurrentLife=Npc[nIdx].m_CurrentLifeMax-NpcSync->nLifeYu;//TakeRemainder(m_CurrentLifeMax,m_CurrentLife);
	  	  if (Npc[nIdx].m_CurrentLife<0)
			  Npc[nIdx].m_CurrentLife=0;
	  }

	
	if (Npc[nIdx].m_Doing != do_death || Npc[nIdx].m_Doing != do_revive) //need check later -- spe 03/05/27
		Npc[nIdx].SendSerCommand((NPCCMD)NpcSync->m_Doing, NpcSync->MapX, NpcSync->MapY);

	Npc[nIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
	Npc[nIdx].SetMenuState(NpcSync->m_btMenuState);
	memset(Npc[nIdx].Name, 0, sizeof(Npc[nIdx].Name));
	memcpy(Npc[nIdx].Name, NpcSync->m_szName, NpcSync->m_wLength - (sizeof(NPC_SYNC) - 1 - sizeof(NpcSync->m_szName)));
}


void KProtocolProcess::s2cplayermissiondata(BYTE* pMsg)
{

	PLAYER_MISSION_DATA	*pData = (PLAYER_MISSION_DATA*)pMsg;
	
	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nMissionGroup = pData->m_nMissionGroup;
	
	CoreDataChanged(GDCNI_PLAYER_BASE_INFO, 0, 0); //创建删除频道

}

void KProtocolProcess::s2csyncrankdata(BYTE* pMsg)
{
	PLAYER_MISSION_RANKDATA* pInfo = (PLAYER_MISSION_RANKDATA *)pMsg;
	//Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("收到信息");	
	memcpy(&Player[CLIENT_PLAYER_INDEX].m_MissionData, &pInfo->SelfData, sizeof(pInfo->SelfData));
	//Old Code
	//for(int i = 0; i < MISSION_STATNUM;i++)	       //十大数据
	//	memcpy(&Player[CLIENT_PLAYER_INDEX].m_MissionRank[i], &pInfo->MissionRank[i], sizeof(pInfo->MissionRank[i])); 

    //New Code
    memcpy(&Player[CLIENT_PLAYER_INDEX].m_MissionRank[pInfo->m_nIndex], &pInfo->MissionRank,
           sizeof(pInfo->MissionRank));

}

void KProtocolProcess::s2cnpcsetpos(BYTE* pMsg)
{
	NPC_PLAYER_TYPE_NORMAL_SYNC *pSync = (NPC_PLAYER_TYPE_NORMAL_SYNC*)pMsg;
	
	int nIdx = NpcSet.SearchID(pSync->m_dwNpcID);

	if(nIdx > 0)
	{

	    int m_gubWorldIndex	= Npc[nIdx].m_SubWorldIndex;

		if  (m_gubWorldIndex<0)
			return;
		//int nRegion, nMapX, nMapY, nOffX, nOffY;
		//SubWorld[m_gubWorldIndex].Mps2Map(pSync->m_dwMapX, pSync->m_dwMapY, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);

		if (Npc[nIdx].m_RegionIndex>=0)
		   SubWorld[m_gubWorldIndex].m_Region[Npc[nIdx].m_RegionIndex].DecNpcRef(Npc[nIdx].m_MapX, Npc[nIdx].m_MapY);
		
		int nRegionX = pSync->m_dwMapX / (SubWorld[m_gubWorldIndex].m_nCellWidth * SubWorld[m_gubWorldIndex].m_nRegionWidth);
		int nRegionY = pSync->m_dwMapY / (SubWorld[m_gubWorldIndex].m_nCellHeight * SubWorld[m_gubWorldIndex].m_nRegionHeight);
		int nRegion  = -1;
		DWORD	dwRegionID = MAKELONG(nRegionX, nRegionY);

		//SubWorld[0].LoadMapC(SubWorld[0].m_SubWorldID, dwRegionID);
		nRegion = SubWorld[m_gubWorldIndex].FindRegion(dwRegionID);

	
		_ASSERT(nRegion >= 0);

		//Npc[nIdx].m_RegionIndex = nRegion;
		//Npc[nIdx].m_dwRegionID = dwRegionID;

		//SubWorld[0].NpcChangeRegion(-1, SubWorld[0].m_Region[nRegion].m_RegionID, nIdx);
		if (Npc[nIdx].m_RegionIndex != nRegion || nRegion < 0)
		{
			//INT nOldRegion = Npc[nIdx].m_RegionIndex;	
			if (Npc[nIdx].m_RegionIndex >= 0)
			{
				SubWorld[m_gubWorldIndex].NpcChangeRegion(SubWorld[m_gubWorldIndex].m_Region[Npc[nIdx].m_RegionIndex].m_RegionID, dwRegionID, nIdx);
			}
			else
			{
				SubWorld[m_gubWorldIndex].NpcChangeRegion(-1, dwRegionID, nIdx);
			}	
		}

		int nMapX, nMapY, nOffX, nOffY;
		SubWorld[m_gubWorldIndex].Mps2Map(pSync->m_dwMapX, pSync->m_dwMapY, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);

		_ASSERT(nRegion == Npc[nIdx].m_RegionIndex);

		Npc[nIdx].m_MapX = nMapX;
		Npc[nIdx].m_MapY = nMapY;
		Npc[nIdx].m_OffX = nOffX;
		Npc[nIdx].m_OffY = nOffY;
		
		//SubWorld[m_gubWorldIndex].m_Region[Npc[nIdx].m_RegionIndex].AddRef(Npc[nIdx].m_MapX, Npc[nIdx].m_MapY, obj_npc);
		SubWorld[m_gubWorldIndex].m_Region[Npc[nIdx].m_RegionIndex].AddNpcRef(Npc[nIdx].m_MapX,Npc[nIdx].m_MapY);
		Npc[nIdx].SendSerCommand(do_stand);
		Npc[nIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
	}

}


void KProtocolProcess::s2csyncnodataeffect(BYTE* pMsg)
{
	STATE_NODATA_EFFECT_SYNC* pNoState = (STATE_NODATA_EFFECT_SYNC *)pMsg;
	
	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SetToolNoStateEffect(Player[CLIENT_PLAYER_INDEX].m_nIndex, pNoState->m_dwSkillID, pNoState->m_nLevel, pNoState->m_nTime);
	
	return;
}

void KProtocolProcess::s2csynconeclearstate(BYTE* pMsg)
{
	PLAYER_ONESTATE_SYNC* pPlayState = (PLAYER_ONESTATE_SYNC *)pMsg;
	
	int nIdx = NpcSet.SearchID(pPlayState->ID);
	
	if (nIdx==Player[CLIENT_PLAYER_INDEX].m_nIndex)
	{
		//Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].ClearOneSkillState(pPlayState->m_Skillid);
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SetSkillLeftTime(pPlayState->m_Skillid,pPlayState->m_Time);
	}
	return;
}

//游戏 人物 中实时同步数据--客户端
void	KProtocolProcess::s2csyncplayermap(BYTE* pMsg)
{
	PLAYER_NORMALMAP_SYNC* pPlaySyncMap = (PLAYER_NORMALMAP_SYNC *)pMsg;

	int nIdx = NpcSet.SearchID(pPlaySyncMap->ID);

	  if (nIdx==Player[CLIENT_PLAYER_INDEX].m_nIndex)
	  {
		Player[CLIENT_PLAYER_INDEX].m_nChestPW	= pPlaySyncMap->m_IsInCity;
	  }
	
}

//游戏 怪物 中实时同步数据--客户端
void KProtocolProcess::SyncNpcMin(BYTE* pMsg)
{
	NPC_NORMAL_SYNC* NpcSync = (NPC_NORMAL_SYNC *)pMsg;

//	if (NpcSync->ID == Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_dwID)
//		return;
	if (NpcSync->Doing == do_revive || NpcSync->Doing == do_death)  //死亡重生 不更新数据
	{
		 return;
	}

	int nIdx = NpcSet.SearchID(NpcSync->ID);
	if (!nIdx)
	{//如果没有这个NPC的索引
		// 向服务器请求同步这个NPC的全部数据
		if (!NpcSet.IsNpcRequestExist(NpcSync->ID))
		{
			SendClientCmdRequestNpc(NpcSync->ID); 
			NpcSet.InsertNpcRequest(NpcSync->ID);
		}
	}
	else
	{
//		int nRegion = SubWorld[0].FindRegion(NpcSync->RegionID);
		int nRegion, nMapX, nMapY, nOffX, nOffY;
		SubWorld[0].Mps2Map(NpcSync->MapX, NpcSync->MapY,&nRegion,&nMapX, &nMapY, &nOffX, &nOffY);

		if (Npc[nIdx].m_RegionIndex == -1 && nIdx != Player[CLIENT_PLAYER_INDEX].m_nIndex)	 //非玩家
		{//非地图中 
			if (nRegion == -1)
				return;
			else
			{
				Npc[nIdx].m_MapX = nMapX;
				Npc[nIdx].m_MapY = nMapY;
				Npc[nIdx].m_OffX = nOffX;
				Npc[nIdx].m_OffY = nOffY;
				Npc[nIdx].m_RegionIndex = nRegion;
				Npc[nIdx].m_dwRegionID = SubWorld[0].m_Region[nRegion].m_RegionID;
				SubWorld[0].m_Region[nRegion].AddNpc(nIdx);
				//SubWorld[0].m_Region[nRegion].AddRef(Npc[nIdx].m_MapX, Npc[nIdx].m_MapY, obj_npc);
				SubWorld[0].m_Region[nRegion].AddNpcRef(Npc[nIdx].m_MapX,Npc[nIdx].m_MapY);
			}
			
			if (NpcSync->Doing == do_stand)
				Npc[nIdx].ProcNetCommand(do_stand);
		}
		else
		{
			if (Npc[nIdx].m_RegionIndex != nRegion && nIdx != Player[CLIENT_PLAYER_INDEX].m_nIndex)
			{//不同地图区域的怪物
				SubWorld[0].m_Region[Npc[nIdx].m_RegionIndex].RemoveNpc(nIdx);
				SubWorld[0].m_Region[Npc[nIdx].m_RegionIndex].DecNpcRef(Npc[nIdx].m_MapX, Npc[nIdx].m_MapY);
				
				Npc[nIdx].m_MapX = nMapX;
				Npc[nIdx].m_MapY = nMapY;
				Npc[nIdx].m_OffX = nOffX;
				Npc[nIdx].m_OffY = nOffY;
				Npc[nIdx].m_RegionIndex = nRegion;

				if (nRegion >= 0)
				{
					SubWorld[0].m_Region[nRegion].AddNpc(nIdx);
					Npc[nIdx].m_dwRegionID = SubWorld[0].m_Region[nRegion].m_RegionID;
					//SubWorld[0].m_Region[nRegion].AddRef(Npc[nIdx].m_MapX, Npc[nIdx].m_MapY, obj_npc);
					SubWorld[0].m_Region[nRegion].AddNpcRef(Npc[nIdx].m_MapX,Npc[nIdx].m_MapY);
				}
			}
		}
        //怪物

		if (nIdx != Player[CLIENT_PLAYER_INDEX].m_nIndex)	// 非玩家本人
		{
			Npc[nIdx].m_CurrentLifeMax = NpcSync->nCurrentLifeMax;
			
			LONG	nOldLife = Npc[nIdx].m_CurrentLife;         //上一次的血量

			//Npc[nIdx].m_CurrentLife = (Npc[nIdx].m_CurrentLifeMax * NpcSync->LifePerCent) >> 7;

			if (NpcSync->nLifeYu >=0 && Npc[nIdx].m_CurrentLifeMax > 0)
			{
				Npc[nIdx].m_CurrentLife=Npc[nIdx].m_CurrentLifeMax-NpcSync->nLifeYu;//TakeRemainder(m_CurrentLifeMax,m_CurrentLife);
				
				if (Npc[nIdx].m_CurrentLife<0)
					Npc[nIdx].m_CurrentLife=0;
			}


			if  (Npc[nIdx].m_CurrentLife<0)
			{
				Npc[nIdx].m_CurrentLife = 0;
			}
               Npc[nIdx].m_CurrentLifeDamage  = NpcSync->nLifeDamagePerCent;
			// 冒血处理kind_partner
			if (Npc[nIdx].m_Kind == kind_normal || Npc[nIdx].m_Kind == kind_partner)  //动物  同步血量  绘制冒血
			{
			     if (nOldLife - Npc[nIdx].m_CurrentLife>0)
				     Npc[nIdx].SetBlood(nOldLife - Npc[nIdx].m_CurrentLife);// nOldLife - Npc[nIdx].m_CurrentLife);NpcSync->nLifeDamagePerCent  //显示减少的生命数字 
				

				SubWorld[0].m_Region[Npc[nIdx].m_RegionIndex].DecNpcRef(Npc[nIdx].m_MapX, Npc[nIdx].m_MapY);
				
				Npc[nIdx].m_MapX = nMapX;
				Npc[nIdx].m_MapY = nMapY;
				Npc[nIdx].m_OffX = nOffX;
				Npc[nIdx].m_OffY = nOffY;
				Npc[nIdx].m_RegionIndex = nRegion;

				if (nRegion >= 0)
				{
					Npc[nIdx].m_dwRegionID = SubWorld[0].m_Region[nRegion].m_RegionID;
					//SubWorld[0].m_Region[nRegion].AddRef(Npc[nIdx].m_MapX, Npc[nIdx].m_MapY,obj_npc);
					SubWorld[0].m_Region[nRegion].AddNpcRef(Npc[nIdx].m_MapX,Npc[nIdx].m_MapY);
				}		
			}

			Npc[nIdx].m_CurrentCamp = NpcSync->Camp;

			/*Npc[nIdx].m_MapX = nMapX;
			Npc[nIdx].m_MapY = nMapY;
			Npc[nIdx].m_OffX = nOffX;
			Npc[nIdx].m_OffY = nOffY;*/
	
			if  (Npc[nIdx].m_IsSynDir>=1)
			     Npc[nIdx].m_Dir=NpcSync->nDir;

		}
		
		Npc[nIdx].m_FreezeState.nTime  = NpcSync->nState & STATE_FREEZE; 
		//逐位与运算符 比较两个操作数对应的二进制位，当两个二进制位均为1时，该位的结果取1，否则取0。
		//Npc[nIdx].m_PoisonState.nTime  = NpcSync->State & STATE_POISON;
		Npc[nIdx].m_PoisonState.nTime  = NpcSync->nState & STATE_POISON;

		Npc[nIdx].m_ConfuseState.nTime = NpcSync->nState & STATE_CONFUSE;
		Npc[nIdx].m_StunState.nTime    = NpcSync->nState & STATE_STUN;
		Npc[nIdx].m_Hide.nTime         = NpcSync->nState & STATE_HIDE;
		Npc[nIdx].m_IgnoreNegativeState.nTime = NpcSync->State & STATE_IGNORENEGATIVE;
		Npc[nIdx].m_randmove.nTime     = NpcSync->nIsMove;

		Npc[nIdx].m_ZhuaVal            = NpcSync->nZhuaVal; 
        Npc[nIdx].m_TongBanNum         = NpcSync->nTongBanNum;
		Npc[nIdx].m_GameliveTime       = NpcSync->mliveTime;

		if (Npc[nIdx].m_CurrentAttackSpeed != NpcSync->ASpeed)
			Npc[nIdx].m_CurrentAttackSpeed = NpcSync->ASpeed;   //外功攻速
		if (Npc[nIdx].m_CurrentCastSpeed != NpcSync->CSpeed)
			Npc[nIdx].m_CurrentCastSpeed = NpcSync->CSpeed;     //内功攻速


		if (NpcSync->nReForm)
			Npc[nIdx].SetNpcIsReFrame(1,NpcSync->nReForm);

		if (Npc[nIdx].GetAttackFrame() != NpcSync->nAttackFrame)
			Npc[nIdx].SetAttackFrame(NpcSync->nAttackFrame);     //外功攻速帧数
		if (Npc[nIdx].GetCastFrame() != NpcSync->nCastFrame)
			Npc[nIdx].SetCastFrame(NpcSync->nCastFrame);         //内功攻速帧数

	    if (Npc[nIdx].m_CurrentdanggeRate != NpcSync->Dangge)    //档格
	    	Npc[nIdx].m_CurrentdanggeRate  = NpcSync->Dangge;

		if(Npc[nIdx].m_CurrentzhongjiRate  !=NpcSync->Zhongji)
            Npc[nIdx].m_CurrentzhongjiRate  =NpcSync->Zhongji;

		Npc[nIdx].m_btRankFFId=NpcSync->RankFFID;
		Npc[nIdx].m_NpcTitle=NpcSync->nNpcTitle;
		Npc[nIdx].m_CurNpcTitle=NpcSync->nCurNpcTitle;
		Npc[nIdx].IsLuKey=NpcSync->AIsLukey;               //活动幸运
        Npc[nIdx].IsJinYan=NpcSync->AIsJinYan;             //活动经验
        Npc[nIdx].m_GoldLucky=NpcSync->gsLukey;
		Npc[nIdx].m_Experience=NpcSync->nExpPer;
		Npc[nIdx].IsJinQian=NpcSync->AIsJinQian;           //活动金钱
		//Npc[nIdx].m_IsTuiGuang=NpcSync->IsTuiGuang;        //是否推官员
		//Npc[nIdx].m_IsVip=NpcSync->IsVip;                  //会员
        Npc[nIdx].m_IsXingYunXing=NpcSync->IsXingYunXing;  //幸运星

         Npc[nIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
		 //sprintf(Npc[nIdx].m_GuishuName,"%s",NpcSync->GuishuName); //同伴归属
		 //char nstrName[64]={0};
		 //Npc[nIdx].SetstrInfo(STR_GUISHU_NAME,NpcSync->GuishuName);
		 snprintf(Npc[nIdx].Name, sizeof(Npc[nIdx].Name),"%s",NpcSync->nName);             //名字
         Npc[nIdx].m_GuiShuDwid=NpcSync->nGuiShuDwid;
		 Npc[nIdx].m_MaskType=NpcSync->nMaskType;
		 
	}
}

//-------------------------------------------------------------------------
//	功能：收到服务器消息同步本玩家npc数据
//-------------------------------------------------------------------------
void KProtocolProcess::SyncNpcMinPlayer(BYTE* pMsg)
{	
	NPC_PLAYER_TYPE_NORMAL_SYNC	*pSync = (NPC_PLAYER_TYPE_NORMAL_SYNC*)pMsg;

	//_ASSERT(pSync->m_dwNpcID == Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_dwID);

	if (pSync->m_dwNpcID != Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_dwID)
		return;

	int nRegion, nMapX, nMapY, nOffX, nOffY, nNpcIdx;

	SubWorld[0].Mps2Map(pSync->m_dwMapX, pSync->m_dwMapY, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);

	nNpcIdx = Player[CLIENT_PLAYER_INDEX].m_nIndex;


	Player[CLIENT_PLAYER_INDEX].m_GameLeftTime=pSync->m_GameLeftTiem;

	/*if  (pSync->nIsSyn)
	{
		Npc[nNpcIdx].m_MapX = nMapX;//在走动的时候已经同步
		Npc[nNpcIdx].m_MapY = nMapY;
		Npc[nNpcIdx].m_OffX = pSync->m_wOffX;
	    Npc[nNpcIdx].m_OffY = pSync->m_wOffY;
	} */


	// 切换地图了，主角的RegionIndex为-1
	if (Npc[nNpcIdx].m_RegionIndex == -1)
	{
		//nRegionIdx = SubWorld[0].FindRegion(pSync->m_nRegionID);
		if (nRegion < 0)
		{
			// 刚刚换的地图，这个时候应该能找得到region，如果找不到，说明数据包可能出错了
//			_ASSERT(0);
			return;
		}
		Npc[nNpcIdx].m_RegionIndex = nRegion;
		Npc[nNpcIdx].m_dwRegionID = SubWorld[0].m_Region[nRegion].m_RegionID;
		//pSync->m_nRegionID;
		SubWorld[0].NpcChangeRegion(-1, SubWorld[0].m_Region[nRegion].m_RegionID, nNpcIdx);
		//SubWorld[0].m_Region[nRegion].AddRef(nMapX, nMapY, obj_npc);
		SubWorld[0].m_Region[nRegion].AddNpcRef(nMapX,nMapY);

		Npc[nNpcIdx].m_MapX = nMapX;
		Npc[nNpcIdx].m_MapY = nMapY;
		Npc[nNpcIdx].m_OffX = pSync->m_wOffX;
		Npc[nNpcIdx].m_OffY = pSync->m_wOffY;
		memset(&Npc[nNpcIdx].m_sSyncPos, 0, sizeof(Npc[nNpcIdx].m_sSyncPos));
		Npc[nNpcIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
		return;
	}

	// 跳太远，超出9屏范围
	//nRegionIdx = SubWorld[0].FindRegion(pSync->m_nRegionID);
	if (nRegion == -1)
	{
		SubWorld[0].m_Region[Npc[nNpcIdx].m_RegionIndex].DecNpcRef(Npc[nNpcIdx].m_MapX, Npc[nNpcIdx].m_MapY);
        
		int nRegionX = pSync->m_dwMapX / (SubWorld[0].m_nCellWidth * SubWorld[0].m_nRegionWidth);
		int nRegionY = pSync->m_dwMapY / (SubWorld[0].m_nCellHeight * SubWorld[0].m_nRegionHeight);
		
		DWORD	dwRegionID = MAKELONG(nRegionX, nRegionY);

		SubWorld[0].LoadMapC(SubWorld[0].m_SubWorldID, dwRegionID);

//		SubWorld[0].LoadMap(SubWorld[0].m_SubWorldID, pSync->m_nRegionID);
		nRegion = SubWorld[0].FindRegion(dwRegionID);
		//_ASSERT(nRegion >= 0);
		if (nRegion<0)
			return;
		Npc[nNpcIdx].m_RegionIndex = nRegion;
		Npc[nNpcIdx].m_dwRegionID = dwRegionID;//pSync->m_nRegionID;
		SubWorld[0].NpcChangeRegion(-1, SubWorld[0].m_Region[nRegion].m_RegionID, nNpcIdx);

		SubWorld[0].Mps2Map(pSync->m_dwMapX, pSync->m_dwMapY, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);
		Npc[nNpcIdx].m_MapX = nMapX;
		Npc[nNpcIdx].m_MapY = nMapY;
		Npc[nNpcIdx].m_OffX = pSync->m_wOffX;
		Npc[nNpcIdx].m_OffY = pSync->m_wOffY;
		
		//SubWorld[0].m_Region[Npc[nNpcIdx].m_RegionIndex].AddRef(Npc[nNpcIdx].m_MapX, Npc[nNpcIdx].m_MapY, obj_npc);
		SubWorld[0].m_Region[Npc[nNpcIdx].m_RegionIndex].AddNpcRef(Npc[nNpcIdx].m_MapX,Npc[nNpcIdx].m_MapY);
		memset(&Npc[nNpcIdx].m_sSyncPos, 0, sizeof(Npc[nNpcIdx].m_sSyncPos));
		Npc[nNpcIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;

		return;
	}


/*
	BYTE	byBarrier = SubWorld[0].m_Region[Npc[nNpcIdx].m_RegionIndex].GetBarrier(Npc[nNpcIdx].m_MapX, Npc[nNpcIdx].m_MapY, Npc[nNpcIdx].m_OffX, Npc[nNpcIdx].m_OffY);
	if (0 != byBarrier && Obstacle_JumpFly != byBarrier)
	{
		//g_DebugLog("[Barrier]Player in Barrier");
	}*/
}

//-------------------------------------------------------------------------
//	功能：收到服务器消息添加一个obj
//-------------------------------------------------------------------------
void KProtocolProcess::SyncObjectAdd(BYTE* pMsg)
{
	OBJ_ADD_SYNC	*pObjSyncAdd = (OBJ_ADD_SYNC*)pMsg;
	int				nObjIndex;

	KCObjItemInfo	sInfo;
    memset(&sInfo, 0, sizeof(sInfo));

	nObjIndex = ObjSet.FindID(pObjSyncAdd->m_nID);
	if (nObjIndex > 0)
		return;

	sInfo.m_nItemID     = pObjSyncAdd->m_nItemID;
	sInfo.m_nItemWidth  = pObjSyncAdd->m_btItemWidth;
	sInfo.m_nItemHeight = pObjSyncAdd->m_btItemHeight;
	sInfo.m_nMoneyNum   = pObjSyncAdd->m_nMoneyNum;
	sInfo.m_nColorID    = pObjSyncAdd->m_btColorID;
	sInfo.m_nMovieFlag  = ((pObjSyncAdd->m_btFlag & 0x02) > 0 ? 1 : 0);
	sInfo.m_nSoundFlag  = ((pObjSyncAdd->m_btFlag & 0x01) > 0 ? 1 : 0);

    sInfo.m_cHaveAttack=  ((pObjSyncAdd->m_btFlag & 0x04) > 0 ? 1 : 0);
	//sInfo.m_cAttackerDwid=0;// pObjSyncAdd->m_nAttackerDwid;
	DWORD m_cAttackerDwid=0;

    if (sInfo.m_cHaveAttack)
	{
       m_cAttackerDwid=Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_dwID;
	}	  
	else
       m_cAttackerDwid=0;

	memset(sInfo.m_szName, 0, sizeof(sInfo.m_szName));
	memcpy(sInfo.m_szName, pObjSyncAdd->m_szName, pObjSyncAdd->m_wLength + 1 + sizeof(pObjSyncAdd->m_szName) - sizeof(OBJ_ADD_SYNC));
    //sprintf(sInfo.m_szName,"%s",pObjSyncAdd->m_szName);

//客户端产生物品
	nObjIndex = ObjSet.ClientAdd(
		pObjSyncAdd->m_nID,\
		pObjSyncAdd->m_nDataID,\
		pObjSyncAdd->m_btState,\
		pObjSyncAdd->m_btDir,\
		pObjSyncAdd->m_wCurFrame,\
		pObjSyncAdd->m_nXpos,\
		pObjSyncAdd->m_nYpos,\
		sInfo,m_cAttackerDwid);
/*
	char msg[80];
	sprintf(msg,"测试禁止外挂捡东西(%s):%d,钱：%d",sInfo.m_szName,sInfo.m_cHaveAttack,sInfo.m_nMoneyNum);
    Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(msg);

#ifdef WAIGUA_ZROC  //外挂？
	if (nObjIndex <= 0)
		return;
	PLAYER_PICKUP_ITEM_COMMAND	sPickUp;
	if (Object[nObjIndex].m_nKind == Obj_Kind_Money)
	{
//		sPickUp.ProtocolType = c2s_playerpickupitem;
//		sPickUp.m_nObjID = Object[nObjIndex].m_nID;
//		sPickUp.m_btPosType = 0;
//		sPickUp.m_btPosX = 0;
//		sPickUp.m_btPosY = 0;
//		if (g_pClient)
//			g_pClient->SendPackToServer(&sPickUp, sizeof(PLAYER_PICKUP_ITEM_COMMAND));
	}
	else if (Object[nObjIndex].m_nKind == Obj_Kind_Item)
	{
		ItemPos	sItemPos;
		if ( FALSE == Player[CLIENT_PLAYER_INDEX].m_ItemList.SearchPosition(Object[nObjIndex].m_nItemWidth, Object[nObjIndex].m_nItemHeight, &sItemPos) )
			return; //检测是否有空位
		sPickUp.ProtocolType = c2s_playerpickupitem;
		sPickUp.m_nObjID    = Object[nObjIndex].m_nID;
		sPickUp.m_btPosType = sItemPos.nPlace;
		sPickUp.m_btPosX    = sItemPos.nX;
		sPickUp.m_btPosY    = sItemPos.nY;
		if (g_pClient)
			g_pClient->SendPackToServer(&sPickUp, sizeof(PLAYER_PICKUP_ITEM_COMMAND));
	}
#endif
*/
}

//-------------------------------------------------------------------------
//	功能：收到服务器消息更新某个obj方向
//-------------------------------------------------------------------------
void KProtocolProcess::SyncObjectDir(BYTE* pMsg)
{
	OBJ_SYNC_DIR	*pObjSyncDir = (OBJ_SYNC_DIR*)pMsg;
	int				nObjIndex;
	nObjIndex = ObjSet.FindID(pObjSyncDir->m_nID);
	if (nObjIndex <= 0)
	{
		// 向服务器发添加请求
		OBJ_CLIENT_SYNC_ADD	sObjClientSyncAdd;
		sObjClientSyncAdd.ProtocolType = c2s_requestobj;
		sObjClientSyncAdd.m_nID = pObjSyncDir->m_nID;
		if (g_pClient)
			g_pClient->SendPackToServer(&sObjClientSyncAdd, sizeof(sObjClientSyncAdd));
	}
	else
	{	// 同步方向
		Object[nObjIndex].SetDir(pObjSyncDir->m_btDir);
	}
}

//-------------------------------------------------------------------------
//	功能：收到服务器消息删除某个obj
//-------------------------------------------------------------------------
void KProtocolProcess::SyncObjectRemove(BYTE* pMsg)
{
	OBJ_SYNC_REMOVE	*pObjSyncRemove = (OBJ_SYNC_REMOVE*)pMsg;
	int				nObjIndex;
	nObjIndex = ObjSet.FindID(pObjSyncRemove->m_nID);
	if (nObjIndex > 0)
	{	// 删除
		Object[nObjIndex].Remove(pObjSyncRemove->m_btSoundFlag);
	}
}

//-------------------------------------------------------------------------
//	功能：收到服务器消息更新某个obj状态
//-------------------------------------------------------------------------
void KProtocolProcess::SyncObjectState(BYTE* pMsg)
{
	OBJ_SYNC_STATE	*pObjSyncState = (OBJ_SYNC_STATE*)pMsg;
	int				nObjIndex;
	nObjIndex = ObjSet.FindID(pObjSyncState->m_nID);
	if (nObjIndex <= 0)
	{
		// 向服务器发添加请求
		OBJ_CLIENT_SYNC_ADD	sObjClientSyncAdd;
		sObjClientSyncAdd.ProtocolType = c2s_requestobj;
		sObjClientSyncAdd.m_nID = pObjSyncState->m_nID;
		if (g_pClient)
			g_pClient->SendPackToServer(&sObjClientSyncAdd, sizeof(sObjClientSyncAdd));
	}
	else
	{	// 同步状态
		if (Object[nObjIndex].m_nRegionIdx == -1)
		{
			int	nRegion;
			nRegion = SubWorld[0].FindRegion(Object[nObjIndex].m_nBelongRegion);

			if (nRegion >= 0)
			{
				Object[nObjIndex].m_nRegionIdx = nRegion;
				SubWorld[0].m_Region[nRegion].AddObj(nObjIndex);
			}
		}
		Object[nObjIndex].SetState(pObjSyncState->m_btState);
	}
}

//-------------------------------------------------------------------------
//	功能：收到服务器消息同步某个 trap 类 obj 的数据
//-------------------------------------------------------------------------
void KProtocolProcess::SyncObjectTrap(BYTE* pMsg)
{
	OBJ_SYNC_TRAP_ACT *pObjTrapSync = (OBJ_SYNC_TRAP_ACT*)pMsg;
	int		nObjIndex;
	nObjIndex = ObjSet.FindID(pObjTrapSync->m_nID);
	if (nObjIndex <= 0)
	{
		// 向服务器发添加请求
		OBJ_CLIENT_SYNC_ADD	sObjClientSyncAdd;
		sObjClientSyncAdd.ProtocolType = c2s_requestobj;
		sObjClientSyncAdd.m_nID = pObjTrapSync->m_nID;
		if (g_pClient)
			g_pClient->SendPackToServer(&sObjClientSyncAdd, sizeof(sObjClientSyncAdd));
	}
	else
	{
		Object[nObjIndex].m_nState = OBJ_TRAP_STATE_ACTING;
		Object[nObjIndex].m_cImage.SetDirStart();
		Object[nObjIndex].m_cSkill.m_nTarX = pObjTrapSync->m_nTarX;
		Object[nObjIndex].m_cSkill.m_nTarY = pObjTrapSync->m_nTarY;
	}
}
//上线同步玩家--基本信息- 数据库-客户端
void KProtocolProcess::SyncPlayer(BYTE* pMsg)
{
	PLAYER_SYNC*	pPlaySync = (PLAYER_SYNC *)pMsg;

	int nIdx = NpcSet.SearchID(pPlaySync->ID);

	Npc[nIdx].m_ArmorType			= pPlaySync->ArmorType;
	//Npc[nIdx].m_AttackSpeed			= pPlaySync->AttackSpeed;
	//Npc[nIdx].m_CastSpeed			= pPlaySync->CastSpeed;
	//Npc[nIdx].m_CurrentAttackSpeed	= pPlaySync->AttackSpeed;
	//Npc[nIdx].m_CurrentCastSpeed	= pPlaySync->CastSpeed;
	Npc[nIdx].m_HelmType			= pPlaySync->HelmType;
	Npc[nIdx].m_HorseType			= pPlaySync->HorseType;
	Npc[nIdx].m_CurrentRunSpeed		= pPlaySync->RunSpeed;
	Npc[nIdx].m_CurrentWalkSpeed	= pPlaySync->WalkSpeed;
	Npc[nIdx].m_WeaponType			= pPlaySync->WeaponType;	
	Npc[nIdx].m_Kind				= kind_player;
	Npc[nIdx].m_btRankId			= pPlaySync->RankID;
	Npc[nIdx].m_btRankFFId			= pPlaySync->RankFFID;
//	Npc[nIdx].m_AutoplayId			= pPlaySync->AutoplayId;
    Npc[nIdx].m_CJtaskID            = pPlaySync->mCJtaskID;      //转盘
//  Npc[nIdx].m_WarShuishou	        = pPlaySync->mWarShuishou;   // 税收 

	Npc[nIdx].m_ExItemId			= pPlaySync->ExItemID; // hanh trang
	Npc[nIdx].m_ExBoxId				= pPlaySync->ExBoxID; // ruong mo rong
	Npc[nIdx].nRankInWorld			= pPlaySync->RankInWorld;
	Npc[nIdx].nRepute				= pPlaySync->Repute;
	Npc[nIdx].nPKValue				= pPlaySync->PKValue;
	Npc[nIdx].nReBorn				= pPlaySync->ReBorn;
	//if (nIdx == Player[CLIENT_PLAYER_INDEX].m_nIndex)
	//	Player[CLIENT_PLAYER_INDEX].m_CUnlocked	= pPlaySync->CUnlocked;	
	Npc[nIdx].m_MaskType            = pPlaySync->MaskType;	    // mat na
	Npc[nIdx].m_PifengType          = pPlaySync->PifengType;	// 披风
	Npc[nIdx].m_nFigure				= pPlaySync->m_bFigure;

	//if (Npc[nIdx].m_nFigure != -1)
	{
		strcpy(Npc[nIdx].TongName,pPlaySync->TongName);   //帮会名
	}
	if (Npc[nIdx].m_BaiTan != pPlaySync->m_bBaiTan && pPlaySync->m_bBaiTan)
	{//向服务器要求更新摆摊名称
		SendClientCmdShopName(Npc[nIdx].m_dwID);
	}
	Npc[nIdx].m_BaiTan	= pPlaySync->m_bBaiTan;

	BOOL bChange = FALSE;

	if (Npc[nIdx].m_HorseType == -1)
	{
		if (Npc[nIdx].m_bRideHorse)
			bChange = TRUE;
		Npc[nIdx].m_bRideHorse = FALSE;
	}
	else
	{
		if (!Npc[nIdx].m_bRideHorse)
			bChange = TRUE;
		Npc[nIdx].m_bRideHorse = TRUE;
	}

	if (pPlaySync->m_btSomeFlag & 0x01)
		Npc[nIdx].m_nPKFlag			= 0;
	else if (pPlaySync->m_btSomeFlag & 0x10)
		Npc[nIdx].m_nPKFlag			= 1;
	else 
		Npc[nIdx].m_nPKFlag			= 2;

	if (pPlaySync->m_btSomeFlag & 0x02)
		Npc[nIdx].m_FightMode		= 1;
	else
		Npc[nIdx].m_FightMode		= 0;
	if (pPlaySync->m_btSomeFlag & 0x04)
		Npc[nIdx].SetSleepMode(1);
	else
		Npc[nIdx].SetSleepMode(0);

	if (nIdx == Player[CLIENT_PLAYER_INDEX].m_nIndex && bChange)
	{
		int nActiveId = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipment(itempart_horse);

		int nActiveInfo[2];

		if (nActiveId)
			if (Npc[nIdx].m_bRideHorse)
			{

				ZeroMemory(nActiveInfo,sizeof(nActiveInfo));
				
				if (Item[nIdx].IsBlue())
				{//如果是蓝装
					nActiveInfo[0] = 1;
					nActiveInfo[1] = Item[nActiveId].GetIsWhere();
					
				} 
				else if  (Item[nIdx].IsPurple())
				{//如果是紫装
					nActiveInfo[0] = 2;
					nActiveInfo[1] = Item[nActiveId].GetIsWhere();
				} 
				else if  (Item[nIdx].IsGold())
				{//如果是黄装
					nActiveInfo[0] = 3;
					nActiveInfo[1] = Item[nActiveId].GetLevel();//等级

				} 
				else
				{
					nActiveInfo[0] = 0;
					nActiveInfo[1] = 0;
				} 
               
				Item[nActiveId].ApplyMagicAttribToNPC(&Npc[nIdx],0,nActiveInfo);  // 骑马就加属性
			}
			else if (!Npc[nIdx].m_bRideHorse)
			{
				if  (Item[nIdx].IsGold())
				{//如果是黄装
					nActiveInfo[0] = 3;
					nActiveInfo[1] = Item[nActiveId].GetLevel();//等级
					
				} 
				else
				{
					nActiveInfo[0] = 0;
					nActiveInfo[1] = 0;
				}

				Item[nActiveId].RemoveMagicAttribFromNPC(&Npc[nIdx],0,nActiveInfo); //下马就去除属性
			}
			
	}
}
//实时同步玩家--包含临时变量-客户端
void KProtocolProcess::SyncPlayerMin(BYTE* pMsg)
{
	PLAYER_NORMAL_SYNC* pPlaySync = (PLAYER_NORMAL_SYNC *)pMsg;

	int nIdx = NpcSet.SearchID(pPlaySync->ID);

	//Npc[nIdx].m_AttackSpeed			= pPlaySync->AttackSpeed;
	//Npc[nIdx].m_CastSpeed			= pPlaySync->CastSpeed;
	//Npc[nIdx].m_CurrentAttackSpeed	= pPlaySync->AttackSpeed;
	//Npc[nIdx].m_CurrentCastSpeed	= pPlaySync->CastSpeed;
	Npc[nIdx].m_bLockNpcDwID	    = pPlaySync->m_LockNpcDwID; 
	Npc[nIdx].m_CurrentRunSpeed		= pPlaySync->RunSpeed;
	Npc[nIdx].m_CurrentWalkSpeed	= pPlaySync->WalkSpeed;
	Npc[nIdx].m_HelmType			= pPlaySync->HelmType;
	Npc[nIdx].m_ArmorType			= pPlaySync->ArmorType;
	Npc[nIdx].m_HorseType			= pPlaySync->HorseType;
	Npc[nIdx].m_WeaponType			= pPlaySync->WeaponType;
	Npc[nIdx].m_Kind				= kind_player;
	Npc[nIdx].m_btRankId			= pPlaySync->RankID;
	Npc[nIdx].m_btRankFFId			= pPlaySync->RankFFID;
	Npc[nIdx].m_AutoplayId			= pPlaySync->AutoplayId;
	Npc[nIdx].m_IsWarCity			= pPlaySync->mIsWarCity;   // 实时同步玩家城市归属
	//Npc[nIdx].m_CurrentNuQi       = pPlaySync->mNuqi;        // 当前怒气值
//    Npc[nIdx].m_WarShuishou	    = pPlaySync->mWarShuishou; // 税收 
    Npc[nIdx].m_nsPlayerIdx         = pPlaySync->msPlayerIdx;
//	Npc[nIdx].m_WarIsGong	        = pPlaySync->mWarGong;     //攻方
//	Npc[nIdx].m_WarIsShou	        = pPlaySync->mWarShou;     //守方
    Npc[nIdx].m_CJtaskID            = pPlaySync->mCJtaskID;    //转盘
//	Npc[nIdx].m_WarShangShu         =SubWorld[Npc[nIdx].m_SubWorldIndex].m_WarShuishou;
/*
	Npc[nIdx].m_WarFucheng          =pPlaySync->mWarFucheng;    //副成
    Npc[nIdx].m_WarZuoHu            =pPlaySync->mWarZuoHu;      //左护法
    Npc[nIdx].m_WarYouHu            =pPlaySync->mWarYouHu;      //右护法
    Npc[nIdx].m_WarTaishi           =pPlaySync->mWarTaishi;     //太史
    Npc[nIdx].m_WarZhongShu         =pPlaySync->mWarZhongShu;   //中书
    Npc[nIdx].m_WarShangShu         =pPlaySync->mWarShangShu;   //尚书
    Npc[nIdx].m_WarMiShu            =pPlaySync->mWarMiShu;      //秘书
    Npc[nIdx].m_WarTaiLe            =pPlaySync->mWarTaiLe;      //太乐
	Npc[nIdx].m_WarCityGX           =pPlaySync->mWarCityGX;     //贡献
*/
	Npc[nIdx].m_IsSerLock           = pPlaySync->nIsSerLock;
	Npc[nIdx].m_ExItemId			= pPlaySync->ExItemID;      // hanh trang
	Npc[nIdx].m_ExBoxId				= pPlaySync->ExBoxID;       // ruong mo rong
	Npc[nIdx].nRankInWorld			= pPlaySync->RankInWorld;
	Npc[nIdx].nRepute				= pPlaySync->Repute;
	Npc[nIdx].nPKValue				= pPlaySync->PKValue;        //PK值
	Npc[nIdx].nReBorn				= pPlaySync->ReBorn;
	Npc[nIdx].m_IsInCity	        = pPlaySync->m_IsInCity;

	if (nIdx == Player[CLIENT_PLAYER_INDEX].m_nIndex)
	{
		//Player[CLIENT_PLAYER_INDEX].m_CUnlocked			=pPlaySync->CUnlocked;
        Player[CLIENT_PLAYER_INDEX].m_nVipType=pPlaySync->nVipType;
		Npc[nIdx].m_MapUseModel.nTime=pPlaySync->nIsOpenMapType;
		//Player[CLIENT_PLAYER_INDEX].m_nCheckWaiGua=pPlaySync->mIsCheckWaiGua;
		Player[CLIENT_PLAYER_INDEX].m_cTong.m_AttackState=pPlaySync->mAttackState;
		//Player[CLIENT_PLAYER_INDEX].m_nChestPW	=pPlaySync->m_IsInCity;

	}
	//Npc[nIdx].m_MaskType			= pPlaySync->MaskType;
    Npc[nIdx].m_PifengType          = pPlaySync->PifengType;	 // 披风
	Npc[nIdx].m_ChiBangType         = pPlaySync->nChiBangType;

    Npc[nIdx].IsCreatTongBan        = pPlaySync->mIsCreatTongBan;//是否已经有同伴
//	Npc[nIdx].m_IsWaiGua            = pPlaySync->mIsWaiGua;      //是否使用外挂
 /*   Npc[nIdx].m_GuziYajing = pPlaySync->GuziYajing;       //押金
    Npc[nIdx].m_GuziDianshu= pPlaySync->GuziDianshu;      //点数
    Npc[nIdx].m_GuziZhuang = pPlaySync->GuziZhuang;       //是否庄家
    Npc[nIdx].m_GuziMenber = pPlaySync->GuziMenber;
    Npc[nIdx].m_IsShuai    = pPlaySync->IsShuai;           //是否已经甩了
    Npc[nIdx].m_ZuoWeihao  = pPlaySync->ZuoWeihao;         //座位号
	*/
	//Npc[nIdx].m_IsTuiGuang = pPlaySync->IsTuiGuang;
    Npc[nIdx].m_IsVip      = pPlaySync->IsVip;
	Npc[nIdx].m_nFigure	   = pPlaySync->m_bFigure;
	strcpy(Npc[nIdx].TongName,pPlaySync->TongName); //帮会名

	if (Npc[nIdx].m_BaiTan != pPlaySync->m_bBaiTan && pPlaySync->m_bBaiTan)
	{//只同步一次？
		SendClientCmdShopName(Npc[nIdx].m_dwID);        //实时同步摆摊名
	}

	Npc[nIdx].m_BaiTan	= pPlaySync->m_bBaiTan;

	if (Npc[nIdx].m_BaiTan)
	   Npc[nIdx].SetstrInfo(STR_SHOP_NAME,pPlaySync->ShopName);

	BOOL bChange = FALSE;
/*
	for  (int m=0;m<10;m++)
	{  
		Npc[nIdx].m_JinMai[m].nLevel1=pPlaySync->JinMai[m].nLevel1;  //为经脉大类
		Npc[nIdx].m_JinMai[m].nLevel2=pPlaySync->JinMai[m].nLevel2;
		Npc[nIdx].m_JinMai[m].nLevel3=pPlaySync->JinMai[m].nLevel3;
		Npc[nIdx].m_JinMai[m].nLevel4=pPlaySync->JinMai[m].nLevel4;
		Npc[nIdx].m_JinMai[m].nLevel5=pPlaySync->JinMai[m].nLevel5;
		Npc[nIdx].m_JinMai[m].nLevel6=pPlaySync->JinMai[m].nLevel6;
		Npc[nIdx].m_JinMai[m].nLevel7=pPlaySync->JinMai[m].nLevel7;
		Npc[nIdx].m_JinMai[m].nLevel8=pPlaySync->JinMai[m].nLevel8;
		Npc[nIdx].m_JinMai[m].nLevel9=pPlaySync->JinMai[m].nLevel9;
		Npc[nIdx].m_JinMai[m].nLevel10=pPlaySync->JinMai[m].nLevel10;
		Npc[nIdx].m_JinMai[m].nLevel11=pPlaySync->JinMai[m].nLevel11;
		Npc[nIdx].m_JinMai[m].nLevel12=pPlaySync->JinMai[m].nLevel12;
		Npc[nIdx].m_JinMai[m].nLevel13=pPlaySync->JinMai[m].nLevel13;
		Npc[nIdx].m_JinMai[m].nLevel14=pPlaySync->JinMai[m].nLevel14;
		Npc[nIdx].m_JinMai[m].nLevel15=pPlaySync->JinMai[m].nLevel15;
	} 

        Player[CLIENT_PLAYER_INDEX].m_cTask.SetSaveVal(3501,pPlaySync->mjmtaska);
		Player[CLIENT_PLAYER_INDEX].m_cTask.SetSaveVal(3502,pPlaySync->mjmtaskb);
		Player[CLIENT_PLAYER_INDEX].m_cTask.SetSaveVal(3503,pPlaySync->mjmtaskc);
		Player[CLIENT_PLAYER_INDEX].m_cTask.SetSaveVal(3504,pPlaySync->mjmtaskd);
		Player[CLIENT_PLAYER_INDEX].m_cTask.SetSaveVal(3505,pPlaySync->mjmtaske);
		Player[CLIENT_PLAYER_INDEX].m_cTask.SetSaveVal(3506,pPlaySync->mjmtaskf);
		Player[CLIENT_PLAYER_INDEX].m_cTask.SetSaveVal(3507,pPlaySync->mjmtaskg);
		Player[CLIENT_PLAYER_INDEX].m_cTask.SetSaveVal(3508,pPlaySync->mjmtaskh);
   
	for  (int n=0;n<MAX_FUMO_COUNT;++n)
	{ 
		Npc[nIdx].m_nFuMoNum[n].nNpcIdx     = pPlaySync->FuMo[n].nNpxidx;
		Npc[nIdx].m_nFuMoNum[n].nNpcSetings = pPlaySync->FuMo[n].nNpxSetNo;
		Npc[nIdx].m_nFuMoNum[n].nNpcSkill   = pPlaySync->FuMo[n].nNpxSkill;
	    Npc[nIdx].m_nFuMoNum[n].nSkillLevel = pPlaySync->FuMo[n].nSlevel;
	}
 	*/
	Npc[nIdx].m_JinMaiBingJia=pPlaySync->mJinMaiBingJia;
    Npc[nIdx].m_ZhenYuan=pPlaySync->mZhenYuan;

	if (Npc[nIdx].m_HorseType == -1)
	{
		if (Npc[nIdx].m_bRideHorse)
			bChange = TRUE;
		Npc[nIdx].m_bRideHorse = FALSE;
	}
	else
	{
		if (!Npc[nIdx].m_bRideHorse)
			bChange = TRUE;
		Npc[nIdx].m_bRideHorse = TRUE;
	}

	if (pPlaySync->m_btSomeFlag & 0x01)
		Npc[nIdx].m_nPKFlag			= 0;
	else if (pPlaySync->m_btSomeFlag & 0x10)
		Npc[nIdx].m_nPKFlag			= 1;
	else 
		Npc[nIdx].m_nPKFlag			= 2;

	if (pPlaySync->m_btSomeFlag & 0x02)
		Npc[nIdx].m_FightMode		= 1;  //战斗模式
	else
		Npc[nIdx].m_FightMode		= 0;

	if (pPlaySync->m_btSomeFlag & 0x04)
		Npc[nIdx].SetSleepMode(1);
	else
		Npc[nIdx].SetSleepMode(0);

	if (pPlaySync->m_btSomeFlag & 0x08)
		Npc[nIdx].m_nTongFlag		= 1;  //有招人权限
	else
		Npc[nIdx].m_nTongFlag		= 0;  //无招人权限

	if (nIdx == Player[CLIENT_PLAYER_INDEX].m_nIndex && bChange)
	{
		int nActiveId = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipment(itempart_horse);
		int nActiveInfo[2];

		if (nActiveId)

			if (Npc[nIdx].m_bRideHorse)
			{  
  
				ZeroMemory(nActiveInfo,sizeof(nActiveInfo));
				
				if (Item[nIdx].IsBlue())
				{//如果是蓝装
					nActiveInfo[0] = 1;
					nActiveInfo[1] = Item[nActiveId].GetIsWhere();
					
				} 
				else if  (Item[nIdx].IsPurple())
				{//如果是紫装
					nActiveInfo[0] = 2;
					nActiveInfo[1] = Item[nActiveId].GetIsWhere();
				} 
				else if  (Item[nIdx].IsGold())
				{//如果是黄装
					nActiveInfo[0] = 3;
					nActiveInfo[1] = Item[nActiveId].GetLevel();//等级
				} 
				else
				{
					nActiveInfo[0] = 0;
					nActiveInfo[1] = 0;
				} 
                
				Item[nActiveId].ApplyMagicAttribToNPC(&Npc[nIdx],0,nActiveInfo);  //应用魔法属性到 NPC身上
			}
			else if (!Npc[nIdx].m_bRideHorse)
			{

				if  (Item[nIdx].IsGold())
				{//如果是黄装
					nActiveInfo[0] = 3;
					nActiveInfo[1] = Item[nActiveId].GetLevel();//等级
				} 
				else
				{
					nActiveInfo[0] = 0;
					nActiveInfo[1] = 0;
				} 

				Item[nActiveId].RemoveMagicAttribFromNPC(&Npc[nIdx],0,nActiveInfo); // 移除魔法属性
			}		
	}
}

void KProtocolProcess::SyncScriptAction(BYTE* pMsg)
{
	Player[CLIENT_PLAYER_INDEX].OnScriptAction((PLAYER_SCRIPTACTION_SYNC *)pMsg);
}
//同步世界地图到---客户端了
void KProtocolProcess::SyncWorld(BYTE* pMsg)
{
	WORLD_SYNC *WorldSync = (WORLD_SYNC *)pMsg;

	if (Player[CLIENT_PLAYER_INDEX].m_nIndex > 0) //如果是客户端
	{
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SendSerCommand(do_stand);
	}

	if (SubWorld[0].m_SubWorldID != WorldSync->SubWorld)
	{
		SubWorld[0].LoadMapC(WorldSync->SubWorld, WorldSync->SRegion);
		 /*
		   m_nRegionWidth  = 16;   //16    512/32
		   m_nRegionHeight = 32;   //32    1024/32
		   //获取地图区域的信息
		   RECT	sRect;
		   sIniFile.GetRect("MAIN", "rect", &sRect);
		   m_nRegionBeginX = sRect.left;
		   m_nRegionBeginY = sRect.top;
		   m_nRegionEndX   = sRect.right;
		   m_nRegionEndY   = sRect.bottom;
		   m_nWorldRegionWidth  = sRect.right  - sRect.left + 1;   //宽度
		   m_nWorldRegionHeight = sRect.bottom - sRect.top + 1;    //高度
		   m_nTotalRegion = m_nWorldRegionWidth * m_nWorldRegionHeight;  // 区域大小 面积
		 
		   m_nCellWidth  = 32;
		   m_nCellHeight = 32;

           int nRegionX = pSync->m_dwMapX /512;//(SubWorld[0].m_nCellWidth * SubWorld[0].m_nRegionWidth);
           int nRegionY = pSync->m_dwMapY /1024;//(SubWorld[0].m_nCellHeight * SubWorld[0].m_nRegionHeight);
		
		   DWORD dwRegionID = MAKELONG(nRegionX, nRegionY);

		 
		   if (!g_PathFinder.PathFindInit(NULL))
		   {   


		   }  
		  */
/*		SubWorld[0].m_Region[0].Load(WorldSync->SubWorld, WorldSync->Region);
		SubWorld[0].m_Region[0].Init(SubWorld[0].m_nRegionWidth, SubWorld[0].m_nRegionHeight);
		for (int i = 0; i < 8; i++)
		{
			if (SubWorld[0].m_Region[0].m_nConRegionID[i] == -1)
				continue;
			SubWorld[0].m_Region[i+1].Load(WorldSync->SubWorld, SubWorld[0].m_Region[0].m_nConRegionID[i]);
			SubWorld[0].m_Region[i+1].Init(SubWorld[0].m_nRegionWidth, SubWorld[0].m_nRegionHeight);
		}*/
	}
	
	/*if (SubWorld[0].m_dwCurrentTime > WorldSync->Frame)
	{
		// 客户端快了，减速处理，或者是Lag引起的旧消息，需要处理
		SubWorld[0].m_dwCurrentTime=WorldSync->Frame;
	}
	else if (SubWorld[0].m_dwCurrentTime < WorldSync->Frame)
	{
		// 客户端慢了，加速处理
        SubWorld[0].m_dwCurrentTime=WorldSync->Frame;
	}*/

	SubWorld[0].m_dwCurrentTime = WorldSync->Frame;
	SubWorld[0].m_IsShowLoop = WorldSync->m_IsShowLoop;
	char nMapType[32]={0},mMapType[32]={0};

/*	char mCity[16];
	sprintf(mCity,"%s","City");
	char mCave[16];
    sprintf(mCave,"%s","Cave");
	char mTong[16];
    sprintf(mTong,"%s","Tong");
	char mCountry[16];
    sprintf(mCountry,"%s","Country");
	char mCapital[16];
    sprintf(mCapital,"%s","Capital");
	char mField[16];
    sprintf(mField,"%s","Field");
	char mBattle[16];
    sprintf(mBattle,"%s","Battle");
*/	
    sprintf(nMapType,"%d_MapType",WorldSync->SubWorld);
    g_NpcMapDropRate.GetString("List",nMapType,"无",mMapType,sizeof(mMapType));
    if (strstr(mMapType,"City"))	
	{ 
	  //sprintf(SubWorld[0].m_WarSubMaster,"%s",WorldSync->WarMaster);  // 城主
      //sprintf(SubWorld[0].m_WarSubTongname,"%s",WorldSync->WarTong);//占领帮会
     
	  //sprintf(SubWorld[0].m_WarGongTong,"%s",WorldSync->WarGongTong); //攻
      //sprintf(SubWorld[0].m_WarShouTong,"%s",WorldSync->WarShouTong); //守

	  SubWorld[0].nWorldMapInfo[STR_MAP_MASTER]    = WorldSync->WarMaster;
	  SubWorld[0].nWorldMapInfo[STR_MAP_ZTONGNAME] = WorldSync->WarTong;
	  SubWorld[0].nWorldMapInfo[STR_MAP_TGONGNAME] = WorldSync->WarGongTong;
	  SubWorld[0].nWorldMapInfo[STR_MAP_TSHOUNAME] = WorldSync->WarShouTong;
      SubWorld[0].m_WarIsWho=WorldSync->WarIsWho;

//      SubWorld[0].m_WarShuishou=WorldSync->WarShui;      //同步税收给当前地图
//	  SubWorld[0].m_TodayMoney =WorldSync->TodayMoney;   //同步今天的收入
  //    SubWorld[0].m_WarZhishu=WorldSync->WarZhi;         //物价指数
      SubWorld[0].m_IswarCity=WorldSync->IsWarCity;      // 是否被占领

	  SubWorld[0].m_ShuiShouType = WorldSync->mShuiType;
/*
	  sprintf(SubWorld[0].m_WarSubFuCheng,"%s",WorldSync->WarSubFuCheng);//副城主
      sprintf(SubWorld[0].m_WarSubZou,"%s",WorldSync->WarSubZou); //左护法
      sprintf(SubWorld[0].m_WarSubYou,"%s",WorldSync->WarSubYou); //右护法
      sprintf(SubWorld[0].m_WarSubTaishi,"%s",WorldSync->WarSubTaishi); //太史
      sprintf(SubWorld[0].m_WarSubZhongshu,"%s",WorldSync->WarSubZhongshu); //中书
      sprintf(SubWorld[0].m_WarSubShangshu,"%s",WorldSync->WarSubShangshu); //尚书
      sprintf(SubWorld[0].m_WarSubMishu,"%s",WorldSync->WarSubMishu);//秘书
      sprintf(SubWorld[0].m_WarSubTaile,"%s",WorldSync->WarSubTaile);//太乐

	  SubWorld[0].m_CZIsNanOrNv=WorldSync->CZIsNanOrNv;//默认为男
      SubWorld[0].m_FZIsNanOrNv=WorldSync->FZIsNanOrNv;
      SubWorld[0].m_ZHIsNanOrNv=WorldSync->ZHIsNanOrNv;
      SubWorld[0].m_YHIsNanOrNv=WorldSync->YHIsNanOrNv;
      SubWorld[0].m_TSIsNanOrNv=WorldSync->TSIsNanOrNv;
      SubWorld[0].m_ZSIsNanOrNv=WorldSync->ZSIsNanOrNv;
      SubWorld[0].m_SSIsNanOrNv=WorldSync->SSIsNanOrNv;
      SubWorld[0].m_MSIsNanOrNv=WorldSync->MSIsNanOrNv;
      SubWorld[0].m_TLIsNanOrNv=WorldSync->TLIsNanOrNv;

	  SubWorld[0].m_WarFucheng=WorldSync->WarFucheng;    //副城
	  SubWorld[0].m_WarZuoHu=WorldSync->WarZuoHu;
	  SubWorld[0].m_WarYouHu=WorldSync->WarYouHu;
	  SubWorld[0].m_WarTaishi=WorldSync->WarTaishi;
	  SubWorld[0].m_WarZhongShu=WorldSync->WarZhongShu;
	  SubWorld[0].m_WarShangShu=WorldSync->WarShangShu;
	  SubWorld[0].m_WarMiShu=WorldSync->WarMiShu;
	  SubWorld[0].m_WarTaiLe=WorldSync->WarTaiLe;
	  SubWorld[0].m_GuanFUliB=WorldSync->GuanFUliB;
      SubWorld[0].m_GuanFUliA=WorldSync->GuanFUliA;
*/
	  SubWorld[0].m_WarCityManey=WorldSync->WarCityManey;//城市建设资金
      SubWorld[0].m_WarCityJB=WorldSync->WarCityJB;//城市建设资金
      SubWorld[0].m_WarCityGX=WorldSync->WarCityGX;//城市建设贡献


	////////////////////////股子游戏////////////////////////////
	/*SubWorld[0].m_SubGuModel=WorldSync->nGuModel;        //模式
    SubWorld[0].m_SubGuIsStart=WorldSync->nGuIsStart;    //是否开始
	SubWorld[0].m_ZuoWeiHao=WorldSync->ZuoWeiHao;        //全局座位号
	SubWorld[0].m_DianshuA=WorldSync->DianshuA;          //点数
    SubWorld[0].m_DianshuB=WorldSync->DianshuB;
    SubWorld[0].m_DianshuC=WorldSync->DianshuC;
    SubWorld[0].m_DianshuD=WorldSync->DianshuD;
    SubWorld[0].m_DianshuE=WorldSync->DianshuE;

	SubWorld[0].m_GyajingA=WorldSync->GyajingA;           //下注金额
    SubWorld[0].m_GyajingB=WorldSync->GyajingB;
    SubWorld[0].m_GyajingC=WorldSync->GyajingC;
    SubWorld[0].m_GyajingD=WorldSync->GyajingD;
    SubWorld[0].m_GyajingE=WorldSync->GyajingE;

	SubWorld[0].m_IsXiaZhuA=WorldSync->IsXiaZhuA;  //是否下注 0没下注 1 是一号已经下
	SubWorld[0].m_IsXiaZhuB=WorldSync->IsXiaZhuB;
	SubWorld[0].m_IsXiaZhuC=WorldSync->IsXiaZhuC;
    SubWorld[0].m_IsXiaZhuD=WorldSync->IsXiaZhuD;
	SubWorld[0].m_IsXiaZhuE=WorldSync->IsXiaZhuE;

	SubWorld[0].m_IsStartA=WorldSync->IsStartA;       //一号玩家是否开始了
	SubWorld[0].m_IsStartB=WorldSync->IsStartB;
	SubWorld[0].m_IsStartC=WorldSync->IsStartC;
	SubWorld[0].m_IsStartD=WorldSync->IsStartD; 
	SubWorld[0].m_IsStartE=WorldSync->IsStartE;
	SubWorld[0].m_CurFarmA=WorldSync->CurFarmA;
    SubWorld[0].m_CurFarmB=WorldSync->CurFarmB;

	sprintf(SubWorld[0].m_GuziZhuang,"%s",WorldSync->nGuziZhuang);//庄家	
	sprintf(SubWorld[0].m_GuzinameA,"%s",WorldSync->nGuzinameA);
	sprintf(SubWorld[0].m_GuzinameB,"%s",WorldSync->nGuzinameB);
    sprintf(SubWorld[0].m_GuzinameC,"%s",WorldSync->nGuzinameC);
    sprintf(SubWorld[0].m_GuzinameD,"%s",WorldSync->nGuzinameD);
    sprintf(SubWorld[0].m_GuzinameE,"%s",WorldSync->nGuzinameE);*/
}
	  SubWorld[0].m_nWPKFlag=WorldSync->WPKFlag; //地图的pk模式设置
	// 同步天气
	  SubWorld[0].m_nWeather = WorldSync->Weather;
	  g_ScenePlace.ChangeWeather(WorldSync->Weather);
}

//-------------------------------------------------------------------------
//	功能：收到服务器消息某玩家终止了与本客户端的好友关系
//-------------------------------------------------------------------------
void	KProtocolProcess::s2cChatDeleteFriend(BYTE* pMsg)
{
	CHAT_DELETE_FRIEND_SYNC	*pDelete = (CHAT_DELETE_FRIEND_SYNC*)pMsg;

	CChatFriend	*pFriend;
	for (int i = 0; i < MAX_FRIEND_TEAM; i++)
	{
		pFriend = (CChatFriend*)Player[CLIENT_PLAYER_INDEX].m_cChat.m_cFriendTeam[i].m_cEveryOne.GetHead();
		while (pFriend)
		{
			if (pFriend->m_dwID == pDelete->m_dwID)
			{
				pFriend->Remove();
				Player[CLIENT_PLAYER_INDEX].m_cChat.m_cFriendTeam[i].m_nFriendNo--;

				// 通知界面 某玩家终止了与本客户端的好友关系
//				CoreDataChanged(GDCNI_CHAT_GROUP, 0, 0);
				CoreDataChanged(GDCNI_CHAT_FRIEND, 0, i);

				KSystemMessage	sMsg;
				sprintf(sMsg.szMessage, MSG_CHAT_DELETED_FRIEND, pFriend->m_szName);
				sMsg.eType = SMT_NORMAL;
				sMsg.byConfirmType = SMCT_NONE;
				sMsg.byPriority = 0;
				sMsg.byParamSize = 0;
				sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);

				delete pFriend;
					pFriend=NULL;
				return;
			}
			pFriend = (CChatFriend*)pFriend->GetNext();
		}
	}
}

//-------------------------------------------------------------------------
//	功能：收到服务器通知某聊天好友下线
//-------------------------------------------------------------------------
void	KProtocolProcess::s2cChatFriendOffLine(BYTE* pMsg)
{
	CHAT_FRIEND_OFFLINE_SYNC	*pSync = (CHAT_FRIEND_OFFLINE_SYNC*)pMsg;
	Player[CLIENT_PLAYER_INDEX].m_cChat.GetMsgOffLine(pSync->m_dwID);
}
//客户端同步技能
void	KProtocolProcess::s2cSyncAllSkill(BYTE * pMsg)
{
	SKILL_SEND_ALL_SYNC	* pSync = (SKILL_SEND_ALL_SYNC*) pMsg;
	int nSkillCount = (pSync->m_wProtocolLong - 2) / sizeof(SKILL_SEND_ALL_SYNC_DATA);
	int nNpcIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;
	KSkill * pOrdinSkill = NULL;

	Npc[nNpcIndex].m_SkillList.Clear();

	for (int i = 0; i < nSkillCount; i ++)
	{
		if (pSync->m_sAllSkill[i].SkillId)
		{
			
//			pOrdinSkill = (KSkill *) g_SkillManager.GetSkill(pSync->m_sAllSkill[i].SkillId, pSync->m_sAllSkill[i].SkillLevel);
//			if (!pOrdinSkill) 
//              continue;
			Npc[nNpcIndex].m_SkillList.Add(\
				pSync->m_sAllSkill[i].SkillId,\
				pSync->m_sAllSkill[i].SkillLevel,\
				0,\
				0,\
				pSync->m_sAllSkill[i].SkillAdd,\
                pSync->m_sAllSkill[i].CurSkillExp);
		}
	}
}

void KProtocolProcess::SyncClientEnd(BYTE* pMsg)
{
	//通知服务器 已经同步玩了吗？
	
	CLIENT_SYNC_END* nNpcSync = (CLIENT_SYNC_END *)pMsg;

	/*if (!nNpcSync->m_IsLogin)
	{
		//Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("提示:登录失败,网络障碍!");
		CoreDataChanged(GDCNI_GAME_FIED,0,0);
		return;
	}*/

	BYTE SyncEnd = (BYTE)c2s_syncclientend;
	if (g_pClient)
		g_pClient->SendPackToServer(&SyncEnd, sizeof(BYTE));

	CoreDataChanged(GDCNI_GAME_START,0,0);//开始进入游戏 更新游戏频道等

	//Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("提示:登录成功,祝您游戏愉快!!");

	Player[CLIENT_PLAYER_INDEX].SetDefaultImmedSkill();  //设置默认快捷技能
    	
//	g_bPingReply = TRUE;
	// 设定聊天订阅频道
//	Player[CLIENT_PLAYER_INDEX].m_cChat.LoadTeamName(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name);
//	Player[CLIENT_PLAYER_INDEX].m_cChat.SaveTeamInfo(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name);
//	Player[CLIENT_PLAYER_INDEX].m_cChat.LoadTakeChannelInfo(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name);
    
	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SetInstantSpr(enumINSTANT_STATE_LOGIN); //释放一个登陆的瞬间特效

	// Player[CLIENT_PLAYER_INDEX].ReCalcState();							// 重新计算身上的状态
	// 开始播音乐
	g_SubWorldSet.m_cMusic.Start(SubWorld[0].m_SubWorldID, SubWorld[0].m_dwCurrentTime, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_FightMode);

	// PK初始化
	Player[CLIENT_PLAYER_INDEX].m_cPK.Init();
}

void	KProtocolProcess::s2cTradeChangeState(BYTE* pMsg)
{
	Player[CLIENT_PLAYER_INDEX].s2cTradeChangeState(pMsg);
}

void	KProtocolProcess::s2cNpcSetMenuState(BYTE* pMsg)
{
	NPC_SET_MENU_STATE_SYNC	*pState = (NPC_SET_MENU_STATE_SYNC*)pMsg;
	int		nNpc;
	if (pState->m_dwID == Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_dwID)
	{
		nNpc = Player[CLIENT_PLAYER_INDEX].m_nIndex;
		if (pState->m_btState == PLAYER_MENU_STATE_TRADEOPEN)
		{
			KSystemMessage	sMsg;
			snprintf(sMsg.szMessage, sizeof(sMsg.szMessage), "%s", MSG_TRADE_STATE_OPEN);
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		else if (pState->m_btState == PLAYER_MENU_STATE_NORMAL)
		{
			if (Npc[nNpc].GetMenuState() == PLAYER_MENU_STATE_TRADEOPEN)
			{
				KSystemMessage	sMsg;
				snprintf(sMsg.szMessage, sizeof(sMsg.szMessage), "%s", MSG_TRADE_STATE_CLOSE);
				sMsg.eType = SMT_NORMAL;
				sMsg.byConfirmType = SMCT_NONE;
				sMsg.byPriority = 0;
				sMsg.byParamSize = 0;
				sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
			}
		}
	}
	else
	{
		nNpc = NpcSet.SearchID(pState->m_dwID);
		if (nNpc == 0)
			return;
	}
	if (pState->m_wLength > sizeof(NPC_SET_MENU_STATE_SYNC) - 1 - sizeof(pState->m_szSentence))
		Npc[nNpc].SetMenuState(pState->m_btState, pState->m_szSentence, pState->m_wLength - (sizeof(NPC_SET_MENU_STATE_SYNC) - 1 - sizeof(pState->m_szSentence)));
	else
		Npc[nNpc].SetMenuState(pState->m_btState);
}

void	KProtocolProcess::s2cTradeMoneySync(BYTE* pMsg)
{
	Player[CLIENT_PLAYER_INDEX].s2cTradeMoneySync(pMsg);
}

void	KProtocolProcess::s2cTradeDecision(BYTE* pMsg)
{
	Player[CLIENT_PLAYER_INDEX].s2cTradeDecision(pMsg);

	Player[CLIENT_PLAYER_INDEX].m_ItemList.UnlockOperation();
}
//客户端状态广播
void	KProtocolProcess::s2cUpdateNpcState(BYTE * pMsg)
{
	NPC_SYNC_STATEINFO * pInfo = (NPC_SYNC_STATEINFO*) pMsg;

	int nIndex = NpcSet.SearchID(pInfo->m_ID);	//状态持有者
	
	if (nIndex > 0 && !Npc[nIndex].IsPlayer() && Npc[nIndex].m_RegionIndex >= 0)
	{//客户端设置状态动画
		Npc[nIndex].SetNpcState(pInfo->m_btStateInfo);  // 设置NPC的状态
	}
}

void	KProtocolProcess::s2cTeamInviteAdd(BYTE * pMsg)
{

    if (Player[CLIENT_PLAYER_INDEX].m_Autoplay.nCteam.nIsHuiDa==1 && Player[CLIENT_PLAYER_INDEX].m_Autoplay.nCteam.nIsDuiYuan==1)
    {
	   TEAM_INVITE_ADD_SYNC	*pPress = (TEAM_INVITE_ADD_SYNC*)pMsg;
	   Player[CLIENT_PLAYER_INDEX].m_cTeam.ReplyInvite(pPress->m_nIdx, 1);
	   return;
	}

	Player[CLIENT_PLAYER_INDEX].m_cTeam.ReceiveInvite((TEAM_INVITE_ADD_SYNC*)pMsg);
}

void	KProtocolProcess::s2cTradePressOkSync(BYTE * pMsg)
{
	TRADE_STATE_SYNC	*pPress = (TRADE_STATE_SYNC*)pMsg;
	Player[CLIENT_PLAYER_INDEX].m_cTrade.SetTradeState(pPress->m_btSelfLock, pPress->m_btDestLock, pPress->m_btSelfOk, pPress->m_btDestOk);
}
//客户端光环技能同步（子弹技能）
void	KProtocolProcess::s2cDirectlyCastSkill(BYTE * pMsg)
{
	DWORD	dwNpcID;
	int		nSkillID, nSkillLevel, nSkillEnChance,nWaitTimer,mIsEnChance,mMaxBeiLv=1;
	int 	MapX, MapY; //DWORD
	NPC_SKILL_SYNC	*pPress = (NPC_SKILL_SYNC*)pMsg;
/*	dwNpcID = *(DWORD *)&pMsg[1];
	nSkillID = *(int *)&pMsg[5];
	nSkillLevel = *(int *)&pMsg[9];
	MapX = *(int *)&pMsg[13];
	MapY = *(int *)&pMsg[17];
	nSkillEnChance = *(int *)&pMsg[21];  //技能加成
	*/
	dwNpcID      = pPress->ID;
	nSkillID     = pPress->nSkillID;          //子技能
	nSkillLevel  = pPress->nSkillLevel;
	MapX         = pPress->nMpsX;
	MapY         = pPress->nMpsY;
	nSkillEnChance = pPress->nSkillEnChance;  //技能加成
	nWaitTimer   = pPress->nWaitTime;
	mIsEnChance  = pPress->nIsEnChance;
	mMaxBeiLv    = pPress->nMaxBei;           // 攻击倍率
	//当指定某个目标时(MapX == -1),MapY为目标的NpcdwID，需要转换成本地的NpcIndex才行
	if (MapX == -1)
	{
		if (MapY < 0 )
			return;
        int nIndex=0;
		    nIndex = NpcSet.SearchID(MapY);

		if (nIndex <= 0)
			return;

		if (Npc[nIndex].m_RegionIndex < 0)
			return;

		int nX, nY;
		nX = Npc[nIndex].m_MapX;
		nY = Npc[nIndex].m_MapY;
		SubWorld[0].NewMap2Mps(Npc[nIndex].m_RegionIndex, nX, nY, 0, 0, (int *)&MapX, (int *)&MapY); //像素坐标
	}
	
	    int nIdx = NpcSet.SearchID(dwNpcID);

		if (nIdx<=0)
			return;

		//if (!Npc[nIdx].IsPlayer())
		//   return;

        //if (Player[CLIENT_PLAYER_INDEX].ConformIdx(nIdx))//不是本人
		//   return;
	
		if (nSkillID <= 0 || nSkillLevel <=0)
			return;

		KSkill * pOrdinSkill = (KSkill *) g_SkillManager.GetSkill(nSkillID,nSkillLevel);

		if (!pOrdinSkill) 
            return ;

		pOrdinSkill->m_nEnChance= nSkillEnChance; 
		//pOrdinSkill->Cast(nIdx,MapX,MapY,nWaitTimer,SKILL_SLT_Npc,mMaxBeiLv);  //发技能
		pOrdinSkill->Cast(nIdx, MapX, MapY);		
  }

void	KProtocolProcess::s2cShowMsg(BYTE *pMsg)
{
	if (!pMsg)
		return;
	SHOW_MSG_SYNC	*pShowMsg = (SHOW_MSG_SYNC*)pMsg;

	switch (pShowMsg->m_wMsgID)
	{
	case enumMSG_ID_TEAM_KICK_One:
		{
			char	szName[32];
			KSystemMessage	sMsg;

			memset(szName, 0, sizeof(szName));
			memcpy(szName, pMsg + sizeof(SHOW_MSG_SYNC) - sizeof(LPVOID), pShowMsg->m_wLength + 1 + sizeof(LPVOID) - sizeof(SHOW_MSG_SYNC));
			if (strcmp(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name, szName) == 0)
			{
				snprintf(sMsg.szMessage, sizeof(sMsg.szMessage), "%s", MSG_TEAM_BE_KICKEN);
				sMsg.eType = SMT_TEAM;
				sMsg.byConfirmType = SMCT_CLICK;
				sMsg.byPriority = 1;
			}
			else
			{
				sprintf(sMsg.szMessage, MSG_TEAM_KICK_ONE, szName);
				sMsg.eType = SMT_NORMAL;
				sMsg.byConfirmType = SMCT_NONE;
				sMsg.byPriority = 0;
			}
			sMsg.byParamSize = 0;
			sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TEAM_DISMISS:
		{
			if (!Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag)
				break;
			KSystemMessage	sMsg;
			if (Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFigure == TEAM_CAPTAIN)
			{
				snprintf(sMsg.szMessage, sizeof(sMsg.szMessage), "%s", MSG_TEAM_DISMISS_CAPTAIN);
			}
			else
			{
				sprintf(sMsg.szMessage, MSG_TEAM_DISMISS_MEMBER, g_Team[0].m_szMemName[0]);
			}
			sMsg.eType = SMT_TEAM;
			sMsg.byConfirmType = SMCT_CLICK;
			sMsg.byPriority = 1;
			sMsg.byParamSize = 0;
			sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TEAM_LEAVE:
		{
			if (!Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag)
				break;
			KSystemMessage	sMsg;
			DWORD	dwID = *(DWORD*)(&pShowMsg->m_lpBuf);
			if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_dwID == dwID)
			{
				sprintf(sMsg.szMessage, MSG_TEAM_LEAVE_SELF_MSG, g_Team[0].m_szMemName[0]);
			}
			else
			{
				sprintf(sMsg.szMessage, MSG_TEAM_LEAVE, "有人");
				for (int i = 0; i < MAX_TEAM_MEMBER; i++)
				{
					if ((DWORD)g_Team[0].m_nMember[i] == dwID)
					{
						sprintf(sMsg.szMessage, MSG_TEAM_LEAVE, g_Team[0].m_szMemName[i + 1]);
						break;
					}
				}
			}
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
/*	case enumMSG_ID_TEAM_REFUSE_INVITE:
		{
			char	szName[32];
			memset(szName, 0, sizeof(szName));
			memcpy(szName, &pShowMsg->m_lpBuf, pShowMsg->m_wLength + 1 + sizeof(LPVOID) - sizeof(SHOW_MSG_SYNC));

			KSystemMessage	sMsg;
			sprintf(sMsg.szMessage, MSG_TEAM_REFUSE_INVITE, szName);
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;*/
	case enumMSG_ID_TEAM_SELF_ADD:
		{
			KSystemMessage	sMsg;
			sprintf(sMsg.szMessage, MSG_TEAM_SELF_ADD, g_Team[0].m_szMemName[0]);
			sMsg.eType = SMT_TEAM;
			sMsg.byConfirmType = SMCT_UI_TEAM;
			sMsg.byPriority = 3;
			sMsg.byParamSize = 0;
			sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TEAM_CHANGE_CAPTAIN_FAIL:
		{
			int		nMember;
			DWORD	dwID = *(DWORD*)(&pShowMsg->m_lpBuf);
			nMember = g_Team[0].FindMemberID(dwID);
			if (nMember < 0)
				break;
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;

			snprintf(sMsg.szMessage, sizeof(sMsg.szMessage), "%s", MSG_TEAM_CHANGE_CAPTAIN_FAIL1);
			sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
			sprintf(sMsg.szMessage, MSG_TEAM_CHANGE_CAPTAIN_FAIL2, g_Team[0].m_szMemName[nMember + 1]);
			sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TEAM_CHANGE_CAPTAIN_FAIL2:
		{
			int		nMember;
			DWORD	dwID = *(DWORD*)(&pShowMsg->m_lpBuf);
			nMember = g_Team[0].FindMemberID(dwID);
			if (nMember < 0)
				break;
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;

			snprintf(sMsg.szMessage, sizeof(sMsg.szMessage), "%s", MSG_TEAM_CHANGE_CAPTAIN_FAIL1);
			sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
			snprintf(sMsg.szMessage, sizeof(sMsg.szMessage), "%s", MSG_TEAM_CHANGE_CAPTAIN_FAIL3);
			sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_OBJ_CANNOT_PICKUP:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			snprintf(sMsg.szMessage, sizeof(sMsg.szMessage), "%s", MSG_OBJ_CANNOT_PICKUP);
			sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_OBJ_TOO_FAR:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			snprintf(sMsg.szMessage, sizeof(sMsg.szMessage), "%s", MSG_OBJ_TOO_FAR);
			sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_DEC_MONEY:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_DEC_MONEY, (int)pShowMsg->m_lpBuf);
			sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TRADE_SELF_ROOM_FULL:
		{
			KSystemMessage	sMsg;
			snprintf(sMsg.szMessage, sizeof(sMsg.szMessage), "%s", MSG_TRADE_SELF_ROOM_FULL);
			sMsg.eType = SMT_SYSTEM;
			sMsg.byConfirmType = SMCT_CLICK;
			sMsg.byPriority = 1;
			sMsg.byParamSize = 0;
			sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TRADE_DEST_ROOM_FULL:
		{
			KSystemMessage	sMsg;
			sprintf(sMsg.szMessage, MSG_TRADE_DEST_ROOM_FULL, Player[CLIENT_PLAYER_INDEX].m_cTrade.m_szDestName);
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TRADE_REFUSE_APPLY:
		{
			int	nIdx = NpcSet.SearchID(*((DWORD*)&pShowMsg->m_lpBuf));
			if (nIdx <= 0)
				return;
			KSystemMessage	sMsg;
			sprintf(sMsg.szMessage, MSG_TRADE_REFUSE_APPLY, Npc[nIdx].Name);
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TRADE_TASK_ITEM:
		{
			KSystemMessage	sMsg;
			snprintf(sMsg.szMessage, sizeof(sMsg.szMessage), "%s", MSG_TRADE_TASK_ITEM);
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_ITEM_DAMAGED:
		{
			int nItemID = (int)pShowMsg->m_lpBuf;
			int nIdx = ItemSet.SearchID(nItemID);
			if (!nIdx)
				break;

			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 1;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_ITEM_DAMAGED, Item[nIdx].GetName());
			sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_GET_ITEM:
		{
			DWORD	dwID = *(DWORD*)(&pShowMsg->m_lpBuf);
			
			int nItemIdx = ItemSet.SearchID(dwID);
			if (nItemIdx <= 0 || nItemIdx >= MAX_ITEM)
				break;

			char	szName[128];
			KSystemMessage	sMsg;

			snprintf(szName, sizeof(szName), "%s", Item[nItemIdx].GetName());

			if (strlen(szName) >= sizeof(sMsg.szMessage) - strlen(MSG_ADD_ITEM))
				break;
			
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_ADD_ITEM, szName);  //获得物品提示
			sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_MONEY_CANNOT_PICKUP:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;

			snprintf(sMsg.szMessage, sizeof(sMsg.szMessage), "%s", MSG_MONEY_CANNOT_PICKUP);
			sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_CANNOT_ADD_TEAM:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			snprintf(sMsg.szMessage, sizeof(sMsg.szMessage), "%s", MSG_TEAM_CANNOT_CREATE);
			sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TARGET_CANNOT_ADD_TEAM:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			snprintf(sMsg.szMessage, sizeof(sMsg.szMessage), "%s", MSG_TEAM_TARGET_CANNOT_ADD_TEAM);
			sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_PK_ERROR_1:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			snprintf(sMsg.szMessage, sizeof(sMsg.szMessage), "%s", MSG_PK_ERROR_1);
			sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_PK_ERROR_2:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			snprintf(sMsg.szMessage, sizeof(sMsg.szMessage), "%s", MSG_PK_ERROR_2);
			sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_PK_ERROR_3:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			snprintf(sMsg.szMessage, sizeof(sMsg.szMessage), "%s", MSG_PK_ERROR_3);
			sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_PK_ERROR_4:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			snprintf(sMsg.szMessage, sizeof(sMsg.szMessage), "%s", MSG_PK_ERROR_4);
			sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_PK_ERROR_5:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			snprintf(sMsg.szMessage, sizeof(sMsg.szMessage), "%s", MSG_PK_ERROR_5);
			sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_PK_ERROR_6:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			snprintf(sMsg.szMessage, sizeof(sMsg.szMessage), "%s", MSG_PK_ERROR_6);
			sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_PK_ERROR_7:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			snprintf(sMsg.szMessage, sizeof(sMsg.szMessage), "%s", MSG_PK_ERROR_7);
			sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_DEATH_LOSE_ITEM:
		{
			char	szName[32];
			memset(szName, 0, sizeof(szName));
			memcpy(szName, &pShowMsg->m_lpBuf, pShowMsg->m_wLength + 1 + sizeof(LPVOID) - sizeof(SHOW_MSG_SYNC));

			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_DEATH_LOSE_ITEM, szName);
			sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TONG_REFUSE_ADD:
		{
			char	szName[32];
			memset(szName, 0, sizeof(szName));
			memcpy(szName, pMsg + sizeof(SHOW_MSG_SYNC) - sizeof(LPVOID), pShowMsg->m_wLength + 1 + sizeof(LPVOID) - sizeof(SHOW_MSG_SYNC));

			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage, MSG_TONG_REFUSE_ADD, szName);
			sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TONG_BE_KICK:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			snprintf(sMsg.szMessage, sizeof(sMsg.szMessage), "%s", MSG_TONG_BE_KICKED);
			sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);

			KUiGameObjectWithName	sUi;
			snprintf(sUi.szName, sizeof(sUi.szName), "%s", Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name);
			sUi.nData = TONG_ACTION_DISMISS;
			sUi.nParam = 0;
			sUi.uParam = 0;
			sUi.szString[0] = 0;

			CoreDataChanged(GDCNI_TONG_ACTION_RESULT, (unsigned int)&sUi, 1);
		}
		break;
	case enumMSG_ID_TONG_LEAVE_SUCCESS:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			snprintf(sMsg.szMessage, sizeof(sMsg.szMessage), "%s", MSG_TONG_LEAVE_SUCCESS);
			sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TONG_LEAVE_FAIL:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			snprintf(sMsg.szMessage, sizeof(sMsg.szMessage), "%s", MSG_TONG_LEAVE_FAIL);
			sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_TONG_CHANGE_AS_MASTER:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			snprintf(sMsg.szMessage, sizeof(sMsg.szMessage), "%s", MSG_TONG_CHANGE_AS_MASTER);
			sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);

			KUiGameObjectWithName	sUi;
			snprintf(sUi.szName, sizeof(sUi.szName), "%s", Player[CLIENT_PLAYER_INDEX].m_cTong.m_BMasterName);
			sUi.nData = TONG_ACTION_DEMISE;
			sUi.nParam = 0;
			sUi.uParam = 0;
			sUi.szString[0] = 0;
			CoreDataChanged(GDCNI_TONG_ACTION_RESULT, (unsigned int)&sUi, 1);
		}
		break;
	case enumMSG_ID_TONG_CHANGE_AS_MEMBER:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			snprintf(sMsg.szMessage, sizeof(sMsg.szMessage), "%s", MSG_TONG_CHANGE_AS_MEMBER);
			sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);

			KUiGameObjectWithName	sUi;
			snprintf(sUi.szName, sizeof(sUi.szName), "%s", Player[CLIENT_PLAYER_INDEX].m_cTong.m_BMasterName);
			sUi.nData = TONG_ACTION_DEMISE;
			sUi.nParam = 0;
			sUi.uParam = 0;
			sUi.szString[0] = 0;
			CoreDataChanged(GDCNI_TONG_ACTION_RESULT, (unsigned int)&sUi, 1);
		}
		break;
	case enumMSG_ID_RIDE_CANNOT:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			snprintf(sMsg.szMessage, sizeof(sMsg.szMessage), "%s", MSG_NPC_CANNOT_RIDE);
			sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_COMP_FAILED:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			snprintf(sMsg.szMessage, sizeof(sMsg.szMessage), "%s", MSG_COMP_FAILED);
			sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_COMP_SUCCESS:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			snprintf(sMsg.szMessage, sizeof(sMsg.szMessage), "%s", MSG_COMP_SUCCESS);
			sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_COMP_ERITEM:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			snprintf(sMsg.szMessage, sizeof(sMsg.szMessage), "%s", MSG_COMP_ERITEM);
			sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
    case enumMSG_ID_COMP_MONEY:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			snprintf(sMsg.szMessage, sizeof(sMsg.szMessage), "%s", MSG_COMP_MONEY);
			sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_COMP_NO:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			snprintf(sMsg.szMessage, sizeof(sMsg.szMessage), "%s", MSG_ID_COMP_NO);
			sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;	
	case enumMSG_ID_SUNYI_SUCCESS:
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			snprintf(sMsg.szMessage, sizeof(sMsg.szMessage), "%s", MSG_SUNYI_SUCCESS);
			sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;	
	case enumMSG_ID_LIANJIE_FAILED: //链接失效
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			snprintf(sMsg.szMessage, sizeof(sMsg.szMessage), "%s", MSG_ID_LIANJIE_FAILED);
			sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;		
	case enumMSG_ID_FUHUO_FAILED: //原地复活
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage,"<c=yellow>复活:请带上您的<c=green>复活戒子<c>就可以原地复活了!");
			sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_ERROR_INFO: //原地复活
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sprintf(sMsg.szMessage,"<c=yellow>警告:数据处理<c=green>错误<c>请联系GM处理!");
			sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case enumMSG_ID_SHUA_SHUA: //刷装备提示
		{ 
        	KSystemMessage	sMsg;
	        sMsg.eType = SMT_NORMAL;
	        sMsg.byConfirmType = SMCT_NONE;
	        sMsg.byPriority = 0;
	        sMsg.byParamSize = 0;
	        snprintf(sMsg.szMessage, sizeof(sMsg.szMessage), "%s", MSG_ID_SHUA_SHUA);
	        sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
	        CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		} 
		break;
   	case enumMSG_ID_RENOVE_TIME_ITEM: //删除过期物品
		{ 
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			snprintf(sMsg.szMessage, sizeof(sMsg.szMessage), "%s", pShowMsg->m_Mmsgs);	
			sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		//	ZeroMemory()
		} 
		break;
	default:
		{ 
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			snprintf(sMsg.szMessage, sizeof(sMsg.szMessage), "%s", pShowMsg->m_Mmsgs);	
			sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
			//	ZeroMemory()
		} 
		break;
	}
}

//同步自己技能状态光环效果
void	KProtocolProcess::SyncStateEffect(BYTE* pMsg)
{
	STATE_EFFECT_SYNC*	pSync = (STATE_EFFECT_SYNC *)pMsg;

	int nDataNum = MAX_SKILL_STATE - (sizeof(STATE_EFFECT_SYNC) - pSync->m_wLength) / sizeof(KMagicAttrib);
	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SetStateSkillEffect(Player[CLIENT_PLAYER_INDEX].m_nIndex, pSync->m_dwSkillID, pSync->m_nLevel, pSync->m_MagicAttrib, nDataNum, pSync->m_nTime,pSync->m_IsEuq);
}

void	KProtocolProcess::s2cTradeApplyStart(BYTE* pMsg)
{
	Player[CLIENT_PLAYER_INDEX].s2cTradeApplyStart(pMsg);
}
//客户端自动移动，不经过鼠标拿起和放下
void	KProtocolProcess::s2cItemAutoMove(BYTE* pMsg)
{
	ITEM_AUTO_MOVE_SYNC	*pSync = (ITEM_AUTO_MOVE_SYNC*)pMsg;
	ItemPos	sSrc, sDest;
	sSrc.nPlace     = pSync->m_btSrcPos;    //源容器
	sSrc.nX         = pSync->m_btSrcX;
	sSrc.nY         = pSync->m_btSrcY;
    sSrc.nSIsRongqi = pSync->m_SisRongqi;

	sDest.nPlace     = pSync->m_btDestPos;  //目标容器
	sDest.nX         = pSync->m_btDestX;
	sDest.nY         = pSync->m_btDestY;
	sDest.nDIsRongqi = pSync->m_DisRongqi;
	//pSync->m_ItenDwid;
	Player[CLIENT_PLAYER_INDEX].m_ItemList.AutoMoveItem(sSrc, sDest);
}
//完成物品交换 就解锁
void KProtocolProcess::FinishedItemExchange(BYTE* pMsg)
{
	Player[CLIENT_PLAYER_INDEX].m_ItemList.UnlockOperation();
}

extern IClientCallback* l_pDataChangedNotifyFunc;

void KProtocolProcess::s2cExtend(BYTE* pMsg)
{
	EXTEND_HEADER* pHeader = (EXTEND_HEADER*)(pMsg + sizeof(tagExtendProtoHeader));

	if (pHeader->ProtocolFamily == pf_playercommunity)
	{
		if (pHeader->ProtocolID == playercomm_s2c_notifychannelid)
		{
			PLAYERCOMM_NOTIFYCHANNELID* pNChann = (PLAYERCOMM_NOTIFYCHANNELID*)pHeader;

			l_pDataChangedNotifyFunc->NotifyChannelID(pNChann->channel, pNChann->channelid, pNChann->cost);
		}
	}
}
//客户端
void KProtocolProcess::s2cExtendChat(BYTE* pMsg)
{
	tagExtendProtoHeader* pExHdr = (tagExtendProtoHeader*)pMsg;
	void* pExPckg = pExHdr + 1;
	BYTE protocol = *(BYTE*)(pExPckg);

	if (protocol == chat_someonechat)   //密聊
	{//对方
		CHAT_SOMEONECHAT_SYNC* pCscSync = (CHAT_SOMEONECHAT_SYNC*)pExPckg;
		l_pDataChangedNotifyFunc->MSNMessageArrival(
			pCscSync->someone, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name,
			(const char*)(pCscSync + 1), pCscSync->sentlen, true,pCscSync->nItemDwid);

		Player[CLIENT_PLAYER_INDEX].m_ItemLinkDwid=pCscSync->nItemDwid;
		//char nMsg[64]={0};
		//sprintf(nMsg,"私聊频道:%s,物品ID:%d",pCscSync->someone,pCscSync->nItemDwid);
		//Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(nMsg);

	}
	else if (protocol == chat_channelchat)   //公告发送通道  屏锁等
	{
		CHAT_CHANNELCHAT_SYNC* pCccSync = (CHAT_CHANNELCHAT_SYNC*)pExPckg;
		int pSentlenA=0;
            pSentlenA =pCccSync->sentlen;// TEncodeText((char*)(pCccSync + 1), (int)pCccSync->sentlen);//控制标记进行转换
		l_pDataChangedNotifyFunc->ChannelMessageArrival(
			pCccSync->channelid, pCccSync->someone,
			(const char*)(pCccSync + 1),pSentlenA, true,pCccSync->nItemDwid);

		Player[CLIENT_PLAYER_INDEX].m_ItemLinkDwid=pCccSync->nItemDwid;

		//char nMsg[64]={0};
		//sprintf(nMsg,"频道:%d,物品ID:%d",pCccSync->channelid,pCccSync->nItemDwid);
		//Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(nMsg);
	}
	else if (protocol == chat_feedback)
	{//本人
		////X
		CHAT_FEEDBACK* pCfb = (CHAT_FEEDBACK*)pExPckg;
		DWORD* pChannelid = (DWORD*)(pCfb + 1);

		if (*pChannelid == -1)
		{//someone   私聊
			char* pDstName = (char*)(pChannelid + 1);
			BYTE* pSentlen = (BYTE*)(pDstName + _NAME_LEN);
			void* pSent    = pSentlen + 1;

			l_pDataChangedNotifyFunc->MSNMessageArrival(
				Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name, pDstName,
				(const char*)pSent, *pSentlen, pCfb->code != codeFail);
		}
		else
		{//channel  频道消息
			BYTE* pSentlen = (BYTE*)(pChannelid + 1);
			void* pSent = pSentlen + 1;
			l_pDataChangedNotifyFunc->ChannelMessageArrival(
				*pChannelid, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name,
				(const char*)pSent, *pSentlen, pCfb->code != codeFail);
		}
		////X
	}
}

static BOOL sParseUGName(const std::string& name, std::string* pUnit, std::string* pGroup)
{
	static const char char_split = '\n';

	size_t pos = name.find(char_split);
	if (pos == name.npos)
	{
		if (pUnit)
			pUnit->resize(0);
		if (pGroup)
			pGroup->assign(name);
	}
	else
	{
		std::string::const_iterator itSplit = name.begin() + pos;

		if (pUnit)
			pUnit->assign(name.begin(), itSplit);
		if (pGroup)
			pGroup->assign(itSplit + 1, name.end());
	}

	return TRUE;
}

void KProtocolProcess::s2cExtendFriend(BYTE* pMsg)
{
	tagExtendProtoHeader* pExHdr = (tagExtendProtoHeader*)pMsg;
	void* pExPckg = pExHdr + 1;

	EXTEND_HEADER* pHeader = (EXTEND_HEADER*)(pExPckg);

	if (pHeader->ProtocolFamily == pf_tong)
	{
		_ASSERT(FALSE);
		//tong message
		//...
	}
	else if (pHeader->ProtocolFamily == pf_friend)
	{
		if (pHeader->ProtocolID == friend_c2c_askaddfriend)
		{
			ASK_ADDFRIEND_SYNC* pAafSync = (ASK_ADDFRIEND_SYNC*)pHeader;
			l_pDataChangedNotifyFunc->FriendInvite(pAafSync->srcrole);
		}
		else if (pHeader->ProtocolID == friend_c2c_repaddfriend)
		{
			REP_ADDFRIEND_SYNC* pRafSync = (REP_ADDFRIEND_SYNC*)pHeader;
			l_pDataChangedNotifyFunc->AddFriend(pRafSync->srcrole, pRafSync->answer);
		}
		else if (pHeader->ProtocolID == friend_s2c_repsyncfriendlist)
		{
			REP_SYNCFRIENDLIST* pRsfl = (REP_SYNCFRIENDLIST*)pHeader;
			char* pGroupTag = (char*)(pRsfl + 1);
			if (*pGroupTag != specGroup)
				goto on_error;
			{{
			char* pGroup = (char*)(pGroupTag + 1);
on_newgroup:
			std::string theUnit, theGroup;
			sParseUGName(std::string(pGroup), &theUnit, &theGroup);

			for (char* pRoleTag = pGroup + strlen(pGroup) + 1; ; )
			{
				if (*pRoleTag == specOver)
					goto on_over;
				else if (*pRoleTag == specGroup)
				{
					pGroup = pRoleTag + 1;
					goto on_newgroup;
				}
				else if (*pRoleTag == specRole)
				{
					char* pState = pRoleTag + 1;
					char* pRole = pState + 1;
					l_pDataChangedNotifyFunc->FriendInfo(pRole, (char*)theUnit.c_str(), (char*)theGroup.c_str(), (BYTE)*pState);
					pRoleTag = pRole + strlen(pRole) + 1;
					continue;
				}
				else
					goto on_error;
			}
			}}
on_error:
			_ASSERT(FALSE);
on_over:
			0;
		}
		else if (pHeader->ProtocolID == friend_s2c_friendstate)
		{
			FRIEND_STATE* pFs = (FRIEND_STATE*)pHeader;
			for (char* pRole = (char*)(pFs + 1); *pRole; pRole += strlen(pRole) + 1)
				l_pDataChangedNotifyFunc->FriendStatus(pRole, pFs->state);
		}
		else if (pHeader->ProtocolID == friend_s2c_syncassociate)
		{
			FRIEND_SYNCASSOCIATE* pFsa = (FRIEND_SYNCASSOCIATE*)pHeader;

			char* szGroup = (char*)(pFsa + 1);
			std::string group(szGroup);

			std::string theUnit, theGroup;
			sParseUGName(group, &theUnit, &theGroup);
			_ASSERT(theGroup.empty());

			for (char* szRole = szGroup + group.size() + 1; *szRole; szRole += strlen(szRole) + 1)
				l_pDataChangedNotifyFunc->AddPeople((char*)theUnit.c_str(), szRole);
		}
	}
}

void KProtocolProcess::s2cExtendTong(BYTE* pMsg)
{
	S2C_TONG_HEAD	*pHead = (S2C_TONG_HEAD*)pMsg;

	switch (pHead->m_btMsgId)
	{
	case enumTONG_SYNC_ID_CITY_INFO:
		{
		   Player[CLIENT_PLAYER_INDEX].m_cTong.SetAttAckCityInfo(pMsg);
		  // Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("接收七城信息成功!!!");
		}
		break;
	case enumTONG_SYNC_ID_ATTACK_INFO:
		{
			//Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("接收帮站信息成功!!!");
			Player[CLIENT_PLAYER_INDEX].m_cTong.SetAttAckInfo(pMsg);

		}
		break;
	case enumTONG_SYNC_ID_CREATE_FAIL:
		{
			TONG_CREATE_FAIL_SYNC *pFail = (TONG_CREATE_FAIL_SYNC*)pMsg;

			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;

			sprintf(sMsg.szMessage, "提示:帮会创建失败");
			sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);

			switch (pFail->m_btFailId)
			{
			case enumTONG_CREATE_ERROR_ID1:		// Player[m_nPlayerIndex].m_nIndex <= 0
				break;
			case enumTONG_CREATE_ERROR_ID2:		// 交易过程中
				break;
			case enumTONG_CREATE_ERROR_ID3:		// 帮会名问题
				snprintf(sMsg.szMessage, sizeof(sMsg.szMessage), "%s", MSG_TONG_CREATE_ERROR01);
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				break;
			case enumTONG_CREATE_ERROR_ID4:		// 帮会阵营问题
				snprintf(sMsg.szMessage, sizeof(sMsg.szMessage), "%s", MSG_TONG_CREATE_ERROR04);
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				break;
			case enumTONG_CREATE_ERROR_ID5:		// 已经是帮会成员
				snprintf(sMsg.szMessage, sizeof(sMsg.szMessage), "%s", MSG_TONG_CREATE_ERROR05);
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				break;
			case enumTONG_CREATE_ERROR_ID6:		// 自己的阵营问题
				snprintf(sMsg.szMessage, sizeof(sMsg.szMessage), "%s", MSG_TONG_CREATE_ERROR04);
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				break;
			case enumTONG_CREATE_ERROR_ID7:		// 等级问题
				snprintf(sMsg.szMessage, sizeof(sMsg.szMessage), "%s", MSG_TONG_CREATE_ERROR07);
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				break;
			case enumTONG_CREATE_ERROR_ID8:		// 钱问题
				snprintf(sMsg.szMessage, sizeof(sMsg.szMessage), "%s", MSG_TONG_CREATE_ERROR08);
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				break;
			case enumTONG_CREATE_ERROR_ID9:		// 组队不能建帮会
				snprintf(sMsg.szMessage, sizeof(sMsg.szMessage), "%s", MSG_TONG_CREATE_ERROR09);
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				break;
			case enumTONG_CREATE_ERROR_ID10:	// 帮会模块出错
				snprintf(sMsg.szMessage, sizeof(sMsg.szMessage), "%s", MSG_TONG_CREATE_ERROR10);
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				break;
			case enumTONG_CREATE_ERROR_ID11:	// 名字字符串出错
				snprintf(sMsg.szMessage, sizeof(sMsg.szMessage), "%s", MSG_TONG_CREATE_ERROR11);
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				break;
			case enumTONG_CREATE_ERROR_ID12:	// 名字字符串过长
				snprintf(sMsg.szMessage, sizeof(sMsg.szMessage), "%s", MSG_TONG_CREATE_ERROR12);
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				break;
			case enumTONG_CREATE_ERROR_ID13:	// 帮会同名错误
				snprintf(sMsg.szMessage, sizeof(sMsg.szMessage), "%s", MSG_TONG_CREATE_ERROR13);
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				break;
			case enumTONG_CREATE_ERROR_ID14:	// 帮会产生失败
				break;
			}
		}
		break;
	case enumTONG_SYNC_ID_TRANSFER_ADD_APPLY:
		{//发往帮会的有同意 拒绝申请的 对象  客户端
			TONG_APPLY_ADD_SYNC	*pApply = (TONG_APPLY_ADD_SYNC*)pMsg;
			char	szName[32];
			DWORD	dwNameID;
			int		nPlayerIdx;

			memset(szName, 0, sizeof(szName));
			memcpy(szName, pApply->m_szName, pApply->m_wLength + 1 + sizeof(pApply->m_szName) - sizeof(TONG_APPLY_ADD_SYNC));
			dwNameID   = g_FileName2Id(szName);
			nPlayerIdx = pApply->m_nPlayerIdx;
            
            if (pApply->m_IsAuToAdd==2)    //是否自动同意加入帮派
			{
		     	Player[CLIENT_PLAYER_INDEX].m_cTong.AcceptMember(pApply->m_nPlayerIdx,dwNameID,1);
				char msg[64];
				snprintf(msg, sizeof(msg),"恭喜:(%s)符合招募要求,自动加入帮派!",szName);
				Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(msg);
				break;
			}

			//给界面发消息，收到申请，是否同意
			KSystemMessage	sMsg;
			sprintf(sMsg.szMessage, "提示:收到(%s)申请加入帮会!", szName);
			sMsg.eType = SMT_CLIQUE;
			sMsg.byConfirmType = SMCT_UI_TONG_JOIN_APPLY;
			sMsg.byPriority = 3;
			sMsg.byParamSize = sizeof(KUiPlayerItem);
			sMsg.nMsgLen=TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
			KUiPlayerItem	player;
			snprintf(player.Name, sizeof(player.Name), "%s", szName);
			player.nIndex = pApply->m_nPlayerIdx;
			player.uId = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, (int)&player);
		}
		break;
	case enumTONG_SYNC_ID_ADD:
		{
			TONG_Add_SYNC	*pAdd = (TONG_Add_SYNC*)pMsg;
			char	szName[32], szTitle[32], szMaster[32];

			memcpy(szName, pAdd->m_szTongName,sizeof(szName));
			memcpy(szTitle, pAdd->m_szTitle, sizeof(szTitle));
			memcpy(szMaster, pAdd->m_szMaster, sizeof(szMaster));

			Player[CLIENT_PLAYER_INDEX].m_cTong.AddTong(pAdd->m_btCamp, szName, szTitle, szMaster);
			// 通知界面更新数据
			KUiGameObjectWithName	sUi;
			memset(&sUi, 0, sizeof(sUi));
			snprintf(sUi.szName, sizeof(sUi.szName), "%s", szName);
			sUi.nData = TONG_ACTION_APPLY;
			snprintf(sUi.szString, sizeof(sUi.szString), "%s", szTitle);
			CoreDataChanged(GDCNI_TONG_ACTION_RESULT, (unsigned int)&sUi, 1);
		/*------------------------------*/
			CoreDataChanged(GDCNI_PLAYER_BASE_INFO, 0, 0);
		}
		break;
	case enumTONG_SYNC_ID_HEAD_INFO: //获取帮会的信息
		{
			TONG_HEAD_INFO_SYNC	*pInfo = (TONG_HEAD_INFO_SYNC*)pMsg;

			// 通知界面得到某帮会信息
			int nIdx = NpcSet.SearchID(pInfo->m_dwNpcID);
			if (nIdx <= 0)
				break;

			KUiPlayerRelationWithOther	sUi;
			sUi.nIndex = nIdx;
			sUi.uId = Npc[nIdx].m_dwID;
			if (nIdx == Player[CLIENT_PLAYER_INDEX].m_nIndex)
				sUi.nRelation = Player[CLIENT_PLAYER_INDEX].m_cTong.GetFigure();
			else
				sUi.nRelation = -1;
			sUi.nData = 0;
			sUi.nParam = 0;
			snprintf(sUi.Name, sizeof(sUi.Name), "%s", Npc[nIdx].Name);

	        KClientTongInfo	sInfo;
			memset(&sInfo, 0, sizeof(sInfo));
			sInfo.nFaction       = pInfo->m_btCamp;                   //阵营
			sInfo.nMemberCount   = (int)pInfo->m_dwMemberNum;         //帮众数
			sInfo.nManagerCount  = (int)pInfo->m_btManagerNum;        //队长数
			sInfo.nDirectorCount = (int)pInfo->m_btDirectorNum;       //长老数
			sInfo.nZhaoMuCount   = pInfo->m_btZhaoMuNum; 
            sInfo.nApplyCount    = pInfo->m_btApplyNum;
			sInfo.nMoney         = pInfo->m_dwMoney;  //钱
			sInfo.nLevel         = pInfo->m_btLevel;  //等级

			if (nIdx == Player[CLIENT_PLAYER_INDEX].m_nIndex && sUi.nRelation==1)
				Player[CLIENT_PLAYER_INDEX].m_cTong.m_CurRenShu=(int)pInfo->m_dwMemberNum+(int)pInfo->m_btManagerNum+(int)pInfo->m_btDirectorNum;
			
			//strcpy(sInfo.szMasterName, pInfo->m_sMember[0].m_szName);  //帮主名  有错误  帮主pInfo->m_sMember[0].m_szName
			//strcpy(sInfo.szName, pInfo->m_szTongName);                 //帮会名
            
			char msg[64];
			snprintf(sInfo.szMasterName, sizeof(sInfo.szMasterName),"%s",pInfo->m_sMember[0].m_szName);
            snprintf(sInfo.szName, sizeof(sInfo.szName),"%s",pInfo->m_szTongName);
            //sprintf(msg,"帮会:%s 帮主:%s",pInfo->m_szTongName,pInfo->m_sMember[0].m_szName);
			sInfo.szName[8]='\0';
            //Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(msg);
			CoreDataChanged(GDCNI_TONG_INFO, (unsigned int)&sUi, (unsigned int)&sInfo);

			KUiGameObjectWithName	sObj;
			snprintf(sObj.szName, sizeof(sObj.szName), "%s", pInfo->m_szTongName); //帮会名
			sObj.nData  = enumTONG_FIGURE_DIRECTOR;
			sObj.nParam = 0;
			sObj.uParam = (unsigned int)pInfo->m_btDirectorNum;//长老数量
			sObj.szString[0] = 0;

			KTongMemberItem	sItem[defTONG_MAX_DIRECTOR];
			memset(sItem, 0, sizeof(sItem));
			for (int i = 0; i < pInfo->m_btDirectorNum; i++)//长老
			{//长老界面更新
				sItem[i].nData = enumTONG_FIGURE_DIRECTOR;
				snprintf(sItem[i].Name, sizeof(sItem[i].Name), "%s", pInfo->m_sMember[i + 1].m_szName);      //长老名称
				strcpy(sItem[i].szAgname, pInfo->m_sMember[i + 1].m_szTitle); //账号称号
			}

			CoreDataChanged(GDCNI_TONG_MEMBER_LIST, (unsigned int)&sObj, (unsigned int)sItem);
		}
		break;
	case enumTONG_SYNC_ID_SELF_INFO:  //同步客户端的帮会信息
		{
			TONG_SELF_INFO_SYNC	*pInfo = (TONG_SELF_INFO_SYNC*)pMsg;
			Player[CLIENT_PLAYER_INDEX].m_cTong.SetSelfInfo(pInfo);   //设置帮会信息
			// 通知界面更新数据
			CoreDataChanged(GDCNI_PLAYER_BASE_INFO, 0, 0);
		}
		break;
	case enumTONG_SYNC_ID_MANAGER_INFO:
		{
			TONG_MANAGER_INFO_SYNC	*pInfo = (TONG_MANAGER_INFO_SYNC*)pMsg;

			// 通知界面得到某帮会队长信息
			KUiGameObjectWithName	sObj;
			snprintf(sObj.szName, sizeof(sObj.szName), "%s", pInfo->m_szTongName);
			sObj.nData = enumTONG_FIGURE_MANAGER;
			sObj.nParam = pInfo->m_btStateNo;
			sObj.uParam = pInfo->m_btCurNum;
			sObj.szString[0] = 0;

			KTongMemberItem	sItem[defTONG_ONE_PAGE_MAX_NUM];
			memset(sItem, 0, sizeof(sItem));
			for (int i = 0; i < pInfo->m_btCurNum; i++)
			{
				sItem[i].nData = enumTONG_FIGURE_MANAGER;
				snprintf(sItem[i].Name, sizeof(sItem[i].Name), "%s", pInfo->m_sMember[i].m_szName);
				strcpy(sItem[i].szAgname, pInfo->m_sMember[i].m_szTitle);
			}

			CoreDataChanged(GDCNI_TONG_MEMBER_LIST, (unsigned int)&sObj, (unsigned int)sItem);
		}
		break;	 
	case enumTONG_SYNC_ID_LIST_INFO:
		{//同步帮会列表信息
			TONG_LIST_INFO_SYNC	*pLInfo = (TONG_LIST_INFO_SYNC*)pMsg;
			// 通知界面得到某帮会帮众信息
			KUiGameObjectWithName	sLObj;
			snprintf(sLObj.szName, sizeof(sLObj.szName), "%s", "系统系统"); //帮会名
			sLObj.nData = pLInfo->m_Count;	     //帮会的总数量
			sLObj.nParam = pLInfo->m_btStateNo;  //开始编号
			sLObj.uParam = pLInfo->m_btCurNum;   //当前的数量
			sLObj.szString[0] = 0;
			
			KTongMemberItem	sLItem[defTONG_ONE_PAGE_MAX_NUM];
			
			memset(sLItem, 0, sizeof(sLItem));
			for (int i = 0; i < pLInfo->m_btCurNum; i++)
			{
				sLItem[i].nData = enumTONG_FIGURE_LIST;
				snprintf(sLItem[i].Name, sizeof(sLItem[i].Name), "%s", pLInfo->m_sTongList[i].m_szName); //帮会名
				strcpy(sLItem[i].szAgname,"全部");         //职位
			}
			
			CoreDataChanged(GDCNI_TONG_LIST, (unsigned int)&sLObj, (unsigned int)sLItem);
		}
		break;
	case enumTONG_SYNC_ID_MEMBER_INFO:
		{
			TONG_MEMBER_INFO_SYNC	*pInfo = (TONG_MEMBER_INFO_SYNC*)pMsg;

			// 通知界面得到某帮会帮众信息
			KUiGameObjectWithName	sObj;
			snprintf(sObj.szName, sizeof(sObj.szName), "%s", pInfo->m_szTongName); //帮会名
			sObj.nData = enumTONG_FIGURE_MEMBER;
			sObj.nParam = pInfo->m_btStateNo;  //开始编号
			sObj.uParam = pInfo->m_btCurNum;   //当前的数量
			sObj.szString[0] = 0;

			KTongMemberItem	sItem[defTONG_ONE_PAGE_MAX_NUM];

			memset(sItem, 0, sizeof(sItem));
			for (int i = 0; i < pInfo->m_btCurNum; i++)
			{
				sItem[i].nData = enumTONG_FIGURE_MEMBER;
				snprintf(sItem[i].Name, sizeof(sItem[i].Name), "%s", pInfo->m_sMember[i].m_szName); //姓名
				strcpy(sItem[i].szAgname, pInfo->m_szTitle);         //职位
			}

			CoreDataChanged(GDCNI_TONG_MEMBER_LIST, (unsigned int)&sObj, (unsigned int)sItem);
		}
		break;
	case enumTONG_SYNC_ID_INSTATE:
		{
			TONG_INSTATE_SYNC	*pInstate = (TONG_INSTATE_SYNC*)pMsg;
			// 通知界面任命是否成功
			KUiGameObjectWithName	sUi;
			snprintf(sUi.szName, sizeof(sUi.szName), "%s", pInstate->m_szName);
			sUi.nData = TONG_ACTION_ASSIGN;
			sUi.nParam = pInstate->m_btNewFigure;
			sUi.uParam = pInstate->m_btOldFigure;
			strcpy(sUi.szString, pInstate->m_szTitle);
			CoreDataChanged(GDCNI_TONG_ACTION_RESULT, (unsigned int)&sUi, pInstate->m_btSuccessFlag);
		}
		break;
	case enumTONG_SYNC_ID_KICK:
		{
			TONG_KICK_SYNC	*pKick = (TONG_KICK_SYNC*)pMsg;
			// 通知界面踢人是否成功
			KUiGameObjectWithName	sUi;
			snprintf(sUi.szName, sizeof(sUi.szName), "%s", pKick->m_szName);
			sUi.nData = TONG_ACTION_DISMISS;
			sUi.nParam = pKick->m_btFigure;
			sUi.uParam = pKick->m_btPos;
			sUi.szString[0] = 0;
			CoreDataChanged(GDCNI_TONG_ACTION_RESULT, (unsigned int)&sUi, pKick->m_btSuccessFlag);
		}
		break;
	case enumTONG_SYNC_ID_CHANGE_MASTER_FAIL:
		{
			TONG_CHANGE_MASTER_FAIL_SYNC	*pFail = (TONG_CHANGE_MASTER_FAIL_SYNC*)pMsg;

			switch (pFail->m_btFailID)
			{
			case 0:		// 对方不在线
				{
					KSystemMessage	sMsg;
					sMsg.eType = SMT_NORMAL;
					sMsg.byConfirmType = SMCT_NONE;
					sMsg.byPriority = 0;
					sMsg.byParamSize = 0;
					sprintf(sMsg.szMessage, "提示:对方不在线!");
					sMsg.nMsgLen=TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
					CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				}
				break;
			case 1:		// 对方能力不够！
				{
					KSystemMessage	sMsg;
					sMsg.eType = SMT_NORMAL;
					sMsg.byConfirmType = SMCT_NONE;
					sMsg.byPriority = 0;
					sMsg.byParamSize = 0;
					sprintf(sMsg.szMessage, "提示:对方能力不足!!");
					sMsg.nMsgLen=TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
					CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				}
				break;
			case 2:		// 名字不对！
				{
					KSystemMessage	sMsg;
					sMsg.eType = SMT_NORMAL;
					sMsg.byConfirmType = SMCT_NONE;
					sMsg.byPriority = 0;
					sMsg.byParamSize = 0;
					sprintf(sMsg.szMessage, "提示:对方名字不对!!");
					sMsg.nMsgLen=TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
					CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				}
				break;

			default:
				break;
			}
		}
		break;
	default:
		break;
	}
}


void KProtocolProcess::s2cChangeWeather(BYTE* pMsg)
{
	g_ScenePlace.ChangeWeather(((SYNC_WEATHER*)pMsg)->WeatherID);
}

void	KProtocolProcess::s2cPKSyncNormalFlag(BYTE* pMsg)
{
	PK_NORMAL_FLAG_SYNC	*pFlag = (PK_NORMAL_FLAG_SYNC*)pMsg;
	Player[CLIENT_PLAYER_INDEX].m_cPK.SetNormalPKState(pFlag->m_btFlag);
}

void	KProtocolProcess::s2cPKSyncEnmityState(BYTE* pMsg)
{
	PK_ENMITY_STATE_SYNC	*pState = (PK_ENMITY_STATE_SYNC*)pMsg;
	char	szName[32];

	memset(szName, 0, sizeof(szName));
	memcpy(szName, pState->m_szName, pState->m_wLength + 1 + sizeof(pState->m_szName) - sizeof(PK_ENMITY_STATE_SYNC));
	Player[CLIENT_PLAYER_INDEX].m_cPK.SetEnmityPKState(pState->m_btState, pState->m_dwNpcID, szName);
}

void	KProtocolProcess::s2cPKSyncExerciseState(BYTE* pMsg)
{
	PK_EXERCISE_STATE_SYNC	*pState = (PK_EXERCISE_STATE_SYNC*)pMsg;
	char	szName[32];
	memset(szName, 0, sizeof(szName));
	memcpy(szName, pState->m_szName, pState->m_wLength + 1 + sizeof(pState->m_szName) - sizeof(PK_EXERCISE_STATE_SYNC));
	Player[CLIENT_PLAYER_INDEX].m_cPK.SetExercisePKState(pState->m_btState, pState->m_dwNpcID, szName);
}

void	KProtocolProcess::s2cPKValueSync(BYTE* pMsg)
{
	PK_VALUE_SYNC	*pValue = (PK_VALUE_SYNC*)pMsg;
	Player[CLIENT_PLAYER_INDEX].m_cPK.SetPKValue(pValue->m_nPKValue);
}

void	KProtocolProcess::s2cReputeValueSync(BYTE* pMsg)
{
	REPUTE_VALUE_SYNC	*pValue = (REPUTE_VALUE_SYNC*)pMsg;
	Player[CLIENT_PLAYER_INDEX].m_cRepute.SetReputeValue(pValue->m_nReputeValue);
}

void	KProtocolProcess::s2cFuYuanValueSync(BYTE* pMsg)
{
	FUYUAN_VALUE_SYNC	*pValue = (FUYUAN_VALUE_SYNC*)pMsg;
	Player[CLIENT_PLAYER_INDEX].m_cFuYuan.SetFuYuanValue(pValue->m_nFuYuanValue);
}

void	KProtocolProcess::s2cReBornValueSync(BYTE* pMsg)
{
	// REBORN_VALUE_SYNC	*pValue = (REBORN_VALUE_SYNC*)pMsg;

	Player[CLIENT_PLAYER_INDEX].m_cReBorn.SetReBornValue(pMsg);	   //pValue->m_nReBornValue
}
//客户端打开商城
void KProtocolProcess::OpenMarketBox(BYTE* pMsg)//系统商城
{
	SALE_MARKET_SYNC* pSale = (SALE_MARKET_SYNC *)pMsg;

	if (pSale==NULL)
		return;

	BuySell.OpenCMarket(pSale->nShopID);
}

void	KProtocolProcess::s2cTongCreate(BYTE* pMsg)
{
	Player[CLIENT_PLAYER_INDEX].m_cTong.Create((TONG_CREATE_SYNC*)pMsg);
}
//设置怪物类型
void	KProtocolProcess::s2cNpcGoldChange(BYTE* pMsg)
{
	NPC_GOLD_CHANGE_SYNC	*pSync = (NPC_GOLD_CHANGE_SYNC*)pMsg;

	int nIdx = NpcSet.SearchID(pSync->m_dwNpcID);

	if (nIdx && (Npc[nIdx].m_Kind == kind_normal || Npc[nIdx].m_Kind ==kind_mouse || Npc[nIdx].m_Kind ==kind_bird))  //野兽类型
	{
		Npc[nIdx].m_cGold.SetGoldCurrentType((int)pSync->m_wGoldFlag);
	}
}
//客户端磨损警告
void	KProtocolProcess::ItemChangeDurability(BYTE* pMsg)
{
	ITEM_DURABILITY_CHANGE	*pIDC = (ITEM_DURABILITY_CHANGE *)pMsg;

	int nIdx = ItemSet.SearchID(pIDC->dwItemID);
	
	if (nIdx)
	{
		Item[nIdx].SetDurability(Item[nIdx].GetDurability() + pIDC->nChange); //设置当前持久

		if (pIDC->nCanUse==1)
             Item[nIdx].SetIsCanUse(pIDC->nCanUse);

		if (Item[nIdx].GetDurability() >=0 && Item[nIdx].GetDurability() <= 3)  //小于三
		{//永不磨损除外
			/*KSystemMessage	sMsg;
			//sMsg.eType         = SMT_NORMAL;
			//sMsg.byConfirmType = SMCT_NONE;
			//sMsg.byPriority    = 0;
			//sMsg.byParamSize   = 0;
            
			sMsg.byConfirmType = SMCT_CLICK;
			sMsg.byParamSize = 0;
			sMsg.byPriority = 1;
	        sMsg.eType = SMT_PLAYER;

			sprintf(sMsg.szMessage, MSG_ITEM_NEARLY_DAMAGED, Item[nIdx].GetName());
			sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0); */
			Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(MSG_ITEM_NEARLY_DAMAGED);
		}
	}
}
//打开打造界面
void	KProtocolProcess::OpenTremble(BYTE* pMsg)
{
	CoreDataChanged(GDCNI_VIEW_TREMBLEITEM, NULL, NULL);
}
//打开纹钢界面
void	KProtocolProcess::OpenWenGang(BYTE* pMsg)
{
	CoreDataChanged(GDCNI_VIEW_RONGLIANEITEM, NULL, NULL);
}
//打开宝石镶嵌界面
void	KProtocolProcess::Openstone(BYTE* pMsg)
{
	CoreDataChanged(GDCNI_VIEW_STONES, NULL, NULL);
}
//打开锻造面板
void	KProtocolProcess::s2copenduanzhao(BYTE* pMsg)
{
    PLAYER_DUANZHAO*	pList = (PLAYER_DUANZHAO *)pMsg;
	CoreDataChanged(GDCNI_OPEN_DUANZHAO,pList->m_nType,TRUE);
}


//游戏股子
void KProtocolProcess::s2cplyerguzicallback(BYTE* pMsg)
{
	PLAYER_SYNCALLBACK_GUZI* pGuZi = (PLAYER_SYNCALLBACK_GUZI *)pMsg;
	Player[CLIENT_PLAYER_INDEX].m_GuziGame.nIsOpen=pGuZi->m_nIsOpen;
	Player[CLIENT_PLAYER_INDEX].m_GuziGame.nTpye=pGuZi->m_Type;
	Player[CLIENT_PLAYER_INDEX].m_GuziGame.nYingL=pGuZi->m_nYinL;
	Player[CLIENT_PLAYER_INDEX].m_GuziGame.nJinBi=pGuZi->m_nJinBi;
	Player[CLIENT_PLAYER_INDEX].m_GuziGame.nDianShu=pGuZi->m_nDianShu;
	Player[CLIENT_PLAYER_INDEX].m_GuziGame.nBeiLv=pGuZi->m_Beilv;
}


//自动移动装备
void KProtocolProcess::s2cpautoMovecallback(BYTE* pMsg)
{
    PLAYER_AUTOCALLBACK_EQUIP*	pEquip = (PLAYER_AUTOCALLBACK_EQUIP *)pMsg;

	ItemPos SrcPos,DesPos;

	SrcPos.nPlace=pEquip->m_Splace;
	SrcPos.nX=pEquip->m_btSrcX;
	SrcPos.nY=pEquip->m_btSrcY;

	DesPos.nPlace=pEquip->m_Dplace;
	DesPos.nX=pEquip->m_btDestX;
	DesPos.nY=pEquip->m_btDestY;

	int	nSrcItemIdx=ItemSet.SearchID(pEquip->m_ItemDwId);
	int nSplace=Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquPlace(Item[nSrcItemIdx].GetDetailType(),nSrcItemIdx); //这个能装备的位置
	int nEuqIdx=Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipment(nSplace); //装备栏是否有装备
	//char msg[64];
	//sprintf(msg,"<colro=yellow>C:DP:%d,DX:%d,DY:%d",DesPos.nPlace,DesPos.nX,DesPos.nY);
	//Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(msg);
	Player[CLIENT_PLAYER_INDEX].m_ItemList.AutoEquip(SrcPos,DesPos,pEquip->m_ItemDwId,nSplace,pEquip->m_Kind);
}


//同步NPC信息
void KProtocolProcess::s2csynnpcinfocallback(BYTE* pMsg)
{
    NPC_SYN_INFO_GETBACK*	pInfo = (NPC_SYN_INFO_GETBACK *)pMsg;

	int nNpcIdx=NpcSet.SearchID(pInfo->m_NpcDwid);

	if (nNpcIdx<=0)
		return;
	 Npc[nNpcIdx].m_CurrentFireResist=pInfo->m_TaskVala;
	 Npc[nNpcIdx].m_CurrentColdResist=pInfo->m_TaskValb;
	 Npc[nNpcIdx].m_CurrentPoisonResist=pInfo->m_TaskValc;
	 Npc[nNpcIdx].m_CurrentLightResist=pInfo->m_TaskVald;
	 Npc[nNpcIdx].m_CurrentPhysicsResist=pInfo->m_TaskVale;
	 Npc[nNpcIdx].m_CurrentFireResistMax=pInfo->m_TaskValf;
	 Npc[nNpcIdx].m_CurrentColdResistMax=pInfo->m_TaskValg;
	 Npc[nNpcIdx].m_CurrentPoisonResistMax=pInfo->m_TaskValh;
	 Npc[nNpcIdx].m_CurrentLightResistMax=pInfo->m_TaskVali;
	 Npc[nNpcIdx].m_CurrentPhysicsResistMax=pInfo->m_TaskValj;
}

//打开锻造面板
void	KProtocolProcess::s2copenvipdazao(BYTE* pMsg)
{
    PLAYER_VIPDAZAO*	pList = (PLAYER_VIPDAZAO *)pMsg;
	CoreDataChanged(GDCNI_OPEN_VIPDAZAO,pList->m_nType,pList->m_nCurExtPiont);
}

void KProtocolProcess::s2cJinMaicallback(BYTE* pMsg)
{

	PLAYER_JINMAI_GETBACK*	sTaskBack= (PLAYER_JINMAI_GETBACK *)pMsg;
	Player[CLIENT_PLAYER_INDEX].m_cTask.SetSaveVal(3501,sTaskBack->m_TaskVala);
	Player[CLIENT_PLAYER_INDEX].m_cTask.SetSaveVal(3502,sTaskBack->m_TaskValb);
	Player[CLIENT_PLAYER_INDEX].m_cTask.SetSaveVal(3503,sTaskBack->m_TaskValc);
	Player[CLIENT_PLAYER_INDEX].m_cTask.SetSaveVal(3504,sTaskBack->m_TaskVald);
	Player[CLIENT_PLAYER_INDEX].m_cTask.SetSaveVal(3505,sTaskBack->m_TaskVale);
	Player[CLIENT_PLAYER_INDEX].m_cTask.SetSaveVal(3506,sTaskBack->m_TaskValf);
	Player[CLIENT_PLAYER_INDEX].m_cTask.SetSaveVal(3507,sTaskBack->m_TaskValg);
	Player[CLIENT_PLAYER_INDEX].m_cTask.SetSaveVal(3508,sTaskBack->m_TaskValh);

	Player[CLIENT_PLAYER_INDEX].UpdataCurData();
}


//同步当前数据
void	KProtocolProcess::s2csyncurupdata(BYTE* pMsg)
{
	Player[CLIENT_PLAYER_INDEX].UpdataCurData();
}


void KProtocolProcess::NetCommandSetRankFF(BYTE* pMsg)
{
	DWORD	dwNpcId;

	dwNpcId = *(DWORD *)&pMsg[1];
	int nIdx = NpcSet.SearchID(dwNpcId);

	if (nIdx > 0)
	{
		Npc[nIdx].m_btRankFFId = (int)pMsg[5];
		Npc[nIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
	}
}

void KProtocolProcess::LadderList(BYTE* pMsg)
{
	LADDER_LIST*	pList = (LADDER_LIST *)pMsg;
	KRankIndex		*pLadderListIndex = NULL;
	
	if (pList->nCount > 0 && pList->nCount < enumLadderEnd)
	{
		pLadderListIndex = new KRankIndex[pList->nCount];
	}
	if (pLadderListIndex)
	{
		for (int i = 0; i < pList->nCount; i++)
		{
			pLadderListIndex[i].usIndexId = pList->dwLadderID[i];
			pLadderListIndex[i].bValueAppened = true;
			pLadderListIndex[i].bSortFlag = true;
		}
		CoreDataChanged(GDCNII_RANK_INDEX_LIST_ARRIVE, pList->nCount, (int)pLadderListIndex);
		delete [] pLadderListIndex;
		pLadderListIndex = NULL;
	}
}

void KProtocolProcess::LadderResult(BYTE* pMsg)
{
	LADDER_DATA*	pLadderData = (LADDER_DATA *)pMsg;
	KRankMessage*	pLadderMessage = NULL;

	pLadderMessage = new KRankMessage[10];
	if (pLadderMessage)
	{
		for (int i = 0; i < 10; i++)
		{
			pLadderMessage[i].usMsgLen = strlen(pLadderData->StatData[i].Name);
			snprintf(pLadderMessage[i].szMsg, sizeof(pLadderMessage[i].szMsg), "%s", pLadderData->StatData[i].Name);
			pLadderMessage[i].nValueAppend = pLadderData->StatData[i].nValue;
			pLadderMessage[i].cSortFlag = (char)pLadderData->StatData[i].bySort;
		}
		unsigned int uParam = 10 | (((WORD)pLadderData->dwLadderID) << 16);
		CoreDataChanged(GDCNII_RANK_INFORMATION_ARRIVE, uParam, (int)pLadderMessage);
		delete [] pLadderMessage;
		pLadderMessage = NULL;
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
#else   //服务器端
//删除角色
void KProtocolProcess::RemoveRole(int nIndex, BYTE * pProtocol)
{

}
//
void KProtocolProcess::NpcRequestCommand(int nIndex, BYTE* pProtocol)
{
	NPC_REQUEST_COMMAND *pNpcRequestSync = (NPC_REQUEST_COMMAND *)pProtocol;
	NpcSet.SyncNpc(pNpcRequestSync->ID, Player[nIndex].m_nNetConnectIdx);
}

//-------------------------------------------------------------------------
//	功能：客户端向服务器请求更新某个obj数据
//-------------------------------------------------------------------------
void KProtocolProcess::ObjRequestCommand(int nIndex, BYTE* pProtocol)
{
	OBJ_CLIENT_SYNC_ADD	*pObjClientSyncAdd = (OBJ_CLIENT_SYNC_ADD*)pProtocol;
	ObjSet.SyncAdd(pObjClientSyncAdd->m_nID, Player[nIndex].m_nNetConnectIdx);
}

extern BOOL g_bPositionSyncFixEnabled;

void KProtocolProcess::NpcWalkCommand(int nIndex, BYTE* pProtocol)
{
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	int nNpcIdx = Player[nIndex].m_nIndex;
	if (nNpcIdx <= 0 || nNpcIdx >= MAX_NPC)
		return;
	NPC_WALK_COMMAND* pNetCommand = (NPC_WALK_COMMAND *)pProtocol;
	int ParamX = pNetCommand->nMpsX;
	int ParamY = pNetCommand->nMpsY;
	if (ParamX < 0)
	{
		ParamX = 0;
	}
	if (ParamY < 0)
	{
		ParamY = 0;
	}
	if (g_bPositionSyncFixEnabled && Npc[nNpcIdx].m_RegionIndex >= 0 && g_pServer &&
	    (Npc[nNpcIdx].m_StunState.nTime > 0 ||
	     (Npc[nNpcIdx].m_FreezeState.nTime > 0 && (Npc[nNpcIdx].m_FreezeState.nTime & 1)) ||
	     Npc[nNpcIdx].m_Doing == do_hurt))
	{
		NPC_PLAYER_TYPE_NORMAL_SYNC sSync;
		memset(&sSync, 0, sizeof(sSync));
		sSync.ProtocolType = (BYTE)s2c_npcsetpos;
		sSync.m_dwNpcID = Npc[nNpcIdx].m_dwID;
		int nMapId = 0;
		Npc[nNpcIdx].GetMpsPos(&sSync.m_dwMapX, &sSync.m_dwMapY, &nMapId);
		sSync.m_wOffX = Npc[nNpcIdx].m_OffX;
		sSync.m_wOffY = Npc[nNpcIdx].m_OffY;
		g_pServer->PackDataToClient(Player[nIndex].m_nNetConnectIdx, (BYTE*)&sSync, sizeof(sSync));
		return;
	}
	Npc[nNpcIdx].SendSerCommand(do_walk, ParamX, ParamY);
}

void KProtocolProcess::NpcRunCommand(int nIndex, BYTE* pProtocol)
{
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	int nNpcIdx = Player[nIndex].m_nIndex;
	if (nNpcIdx <= 0 || nNpcIdx >= MAX_NPC)
		return;
	NPC_RUN_COMMAND* pNetCommand = (NPC_RUN_COMMAND *)pProtocol;
	int ParamX = pNetCommand->nMpsX;
	int ParamY = pNetCommand->nMpsY;

	if (ParamX < 0)
		ParamX = 0;

	if (ParamY < 0)
		ParamY = 0;

	Npc[nNpcIdx].CheckTrap();

	if (g_bPositionSyncFixEnabled && Npc[nNpcIdx].m_RegionIndex >= 0 && g_pServer &&
	    (Npc[nNpcIdx].m_StunState.nTime > 0 ||
	     (Npc[nNpcIdx].m_FreezeState.nTime > 0 && (Npc[nNpcIdx].m_FreezeState.nTime & 1)) ||
	     Npc[nNpcIdx].m_randmove.nTime > 0 ||
	     Npc[nNpcIdx].m_Doing == do_hurt))
	{
		NPC_PLAYER_TYPE_NORMAL_SYNC sSync;
		memset(&sSync, 0, sizeof(sSync));
		sSync.ProtocolType = (BYTE)s2c_npcsetpos;
		sSync.m_dwNpcID = Npc[nNpcIdx].m_dwID;
		int nMapId = 0;
		Npc[nNpcIdx].GetMpsPos(&sSync.m_dwMapX, &sSync.m_dwMapY, &nMapId);
		sSync.m_wOffX = Npc[nNpcIdx].m_OffX;
		sSync.m_wOffY = Npc[nNpcIdx].m_OffY;
		g_pServer->PackDataToClient(Player[nIndex].m_nNetConnectIdx, (BYTE*)&sSync, sizeof(sSync));
		return;
	}
	Npc[nNpcIdx].SendSerCommand(do_run, ParamX, ParamY);
}
//服务器端
void KProtocolProcess::NpcSunyiCommand(int nIndex, BYTE* pProtocol) //瞬间移动 瞬移
{
	NPC_SUNYI_COMMAND* pNetCommand = (NPC_SUNYI_COMMAND *)pProtocol;
	int PnMap  = pNetCommand->nMap;
	int PnMpsX = pNetCommand->nMpsX;
	int PnMpsY = pNetCommand->nMpsY;
	int Model  = pNetCommand->nMapID;

//	int PDwid = pNetCommand->nDwid;
	/*if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;	
	if (Npc[Player[nIndex].m_nIndex].m_Index <= 0 || Npc[Player[nIndex].m_nIndex].m_Index >= MAX_NPC)
		return;*/
/*
	if (Player[nIndex].getFindPathState() == 1)
	{
		Player[nIndex].m_ItemList.msgshow("Dang tim duong k the dich chuyen!");
	}
	return;
*/	
    int reg=0,PnNpcID=0;

	if (pNetCommand->nDwid>0)
	   PnNpcID= NpcSet.SearchID(pNetCommand->nDwid);            //怪物的dwid

	if (PnNpcID>0)//怪物移动
	       reg=Npc[PnNpcID].ChangeWorld(PnMap, PnMpsX, PnMpsY);  //同地图传送 SetPos
	else
	{//自己本人移动的 ---检测可以移动的地图
		   //if(Npc[Player[nIndex].m_nIndex].CheckMaps("ForbitMap",PnMap))
		   char nValKey[32]={0},nValInfo[32]={0};
		   sprintf(nValKey,"%d_IsNoReturn",PnMap);

		   g_NpcMapDropRate.GetInteger("List",nValKey,0,&reg);

		   if (reg)
		   {
			  Player[nIndex].m_ItemList.msgshow("Ban do nay khong the dich chuyen!");
			  return;
		   }

		  if (Model==0)
              reg = Npc[Player[nIndex].m_nIndex].ChangeWorld(PnMap, PnMpsX, PnMpsY);  //同地图传送 SetPos
		  else  if (Model==1)
		  {//点击地图移动 需要判断包袱里是否有神行符号
			  if (!Player[nIndex].m_ItemList.IsItemName("神行符"))
			  {
				  Player[nIndex].m_ItemList.msgshow("Hanh trong khong co <c=gyellow>Than Hanh Phu<c>!");
				  return;
			  }
			  if (!Player[nIndex].m_ItemList.CostMoney(500))
			  {
				  Player[nIndex].m_ItemList.msgshow("Ban Khong co 500 luong!");
				  return;
			  }

			  reg = Npc[Player[nIndex].m_nIndex].ChangeWorld(PnMap, PnMpsX, PnMpsY);  //同地图传送 SetPos
			  sprintf(nValKey,"%d_MapType",PnMap);
			  g_NpcMapDropRate.GetString("List",nValKey,"",nValInfo,sizeof(nValInfo));

			  if (strstr(nValInfo,"City") || strstr(nValInfo,"Country") || strstr(nValInfo,"Capital") || strstr(nValInfo, "Battlefield") )
			  {
				  if  (reg==1)
					  Npc[Player[nIndex].m_nIndex].SetFightMode(0);
			  }
			  else
			  {
				  if  (reg==1)
					  Npc[Player[nIndex].m_nIndex].SetFightMode(1);

			  }
		  }

		 
		  sprintf(nValKey,"%d_name",PnMap);
		  g_NpcMapDropRate.GetString("List",nValKey,"",nValInfo,sizeof(nValInfo));

		  if (reg==1)
		  {
			  char nMsg[64]={0};
			  snprintf(nMsg, sizeof(nMsg),"Da dich chuyen den,<c=gyellow>%s<c>..",nValInfo);
			  Player[nIndex].m_ItemList.msgshow(nMsg);
			  return;
		  }
		  else //发送提示信息
		  {
			  Player[nIndex].m_ItemList.msgshow("khong the dich chuyen den ban do..");
			  return;
		  } 

	}
	
	// Npc[Player[nIndex].m_nIndex].ChangeWorld(ParamX, ParamY, ParamZ); 
	
}
//服务器执行使用技能效果
void KProtocolProcess::NpcSkillCommand(int nIndex, BYTE* pProtocol)
{
	NPC_SKILL_COMMAND* pNetCommand = (NPC_SKILL_COMMAND *)pProtocol;
	int ParamX = pNetCommand->nSkillID;
	int ParamY = pNetCommand->nMpsX; //-1
	int ParamZ = pNetCommand->nMpsY;//npc dwid
	// 参数合法性检查
	if (ParamX <= 0 || ParamX >=MAX_SKILL )
		return ;

	if (ParamZ < 0) 
		return;

	if (ParamY < 0)
	{
		if (ParamY != -1) 
			return;
		//NpcSet.SearchID((DWORD)ParamZ); //全服务器搜索
		int nNpcIndex = Player[nIndex].FindAroundNpc((DWORD)ParamZ); //周围的几个区域搜索。

		if (nNpcIndex > 0) 
			//向客户端发送做技能动作
			Npc[Player[nIndex].m_nIndex].SendSerCommand(do_skill, ParamX, ParamY, nNpcIndex);
	}
	else
		    Npc[Player[nIndex].m_nIndex].SendSerCommand(do_skill, ParamX, ParamY, ParamZ);
}

void KProtocolProcess::NpcJumpCommand(int nIndex, BYTE* pProtocol)
{
	NPC_JUMP_COMMAND* pNetCommand = (NPC_JUMP_COMMAND *)pProtocol;
	int ParamX = pNetCommand->nMpsX;
	int ParamY = pNetCommand->nMpsY;
	Npc[Player[nIndex].m_nIndex].SendSerCommand(do_jump, ParamX, ParamY);
}

void KProtocolProcess::NpcTalkCommand(int nIndex, BYTE* pProtocol)
{

}

void KProtocolProcess::PlayerTalkCommand(int nIndex, BYTE* pProtocol)
{
	Npc[Player[nIndex].m_nIndex].DoPlayerTalk((char *)pProtocol + 1);
}

void KProtocolProcess::PlayerApplyTeamInfo(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].S2CSendTeamInfo(pProtocol);
}

void KProtocolProcess::PlayerApplyCreateTeam(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].CreateTeam(pProtocol);
}

void KProtocolProcess::PlayerApplyTeamOpenClose(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].SetTeamState(pProtocol);
}

void KProtocolProcess::PlayerApplyAddTeam(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].S2CSendAddTeamInfo(pProtocol);
}

void KProtocolProcess::PlayerAcceptTeamMember(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].AddTeamMember(pProtocol);
}

void KProtocolProcess::PlayerApplyLeaveTeam(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].LeaveTeam(pProtocol);
}

void KProtocolProcess::PlayerApplyTeamKickMember(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].TeamKickOne(pProtocol);
}

void KProtocolProcess::PlayerApplyTeamChangeCaptain(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].TeamChangeCaptain(pProtocol);
}

void KProtocolProcess::PlayerApplyTeamDismiss(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].TeamDismiss(pProtocol);
}

void KProtocolProcess::PlayerApplySetPK(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].SetPK(pProtocol);
}

void KProtocolProcess::PlayerApplyFactionData(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].SendFactionData(pProtocol);
}

void KProtocolProcess::PlayerSendChat(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].ServerSendChat(pProtocol);
}
//服务器端加潜能点
void KProtocolProcess::PlayerAddBaseAttribute(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].AddBaseAttribute(pProtocol);
}
//服务器端加技能点
void KProtocolProcess::PlayerApplyAddSkillPoint(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].AddSkillPoint(pProtocol);
}
//客户端要求 服务器端自动移动物品 不经过鼠标
void KProtocolProcess::c2splayerAutoMoveitem(int nIndex, BYTE* pProtocol)
{
	//if (Player[nIndex].CheckTrading())
	//	return;
	//Player[nIndex].m_ItemList.EatMecidineC(pEat->m_btPlace, pEat->m_btX, pEat->m_btY);
	//Player[nIndex].m_ItemList.AutoMoveItems(int nItemIdx, int nSrcX, int nSrcY, int nDestX, int nDestY,int nSrcRonn,int nDestRonn);
	Player[nIndex].AutoItem(pProtocol);
}

void KProtocolProcess::PlayerEatItem(int nIndex, BYTE* pProtocol)
{
	if (Player[nIndex].CheckTrading())
		return;
	Player[nIndex].EatItem(pProtocol);
}

void KProtocolProcess::PlayerPickUpItem(int nIndex, BYTE* pProtocol)
{
	if (Player[nIndex].CheckTrading())
	{
		Player[nIndex].m_ItemList.msgshow("提示:<c=green>交易中,不能拾取<c>!");
		return;
	}

	Player[nIndex].ServerPickUpItem(pProtocol);
}

void KProtocolProcess::PlayerMoveItem(int nIndex, BYTE* pProtocol)
{
	//return;
   if  (Player[nIndex].getMobileClient()) //如果不是手机端 就启用
   {
	   printf("-------手机端禁止移动物品--------- \n");
	   return;
   }

   Player[nIndex].ServerMoveItem(pProtocol);
}

void KProtocolProcess::PlayerSellItem(int nIndex, BYTE* pProtocol)
{
	if (Player[nIndex].CheckTrading())
		return;
	Player[nIndex].SellItem(pProtocol);
}

void KProtocolProcess::PlayerBuyItem(int nIndex, BYTE* pProtocol)
{
	if (Player[nIndex].CheckTrading())
		return;
	Player[nIndex].BuyItem(pProtocol);
}

void KProtocolProcess::PlayerDropItem(int nIndex, BYTE* pProtocol)
{
	if (Player[nIndex].CheckTrading())
		return;
	Player[nIndex].ServerThrowAwayItem(pProtocol);
}

void KProtocolProcess::PlayerSelUI(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].ProcessPlayerSelectFromUI(pProtocol);
}

void KProtocolProcess::ChatSetChannel(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].ChatSetTakeChannel(pProtocol);
}

void KProtocolProcess::ChatApplyAddFriend(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].ChatTransmitApplyAddFriend(pProtocol);
}

void KProtocolProcess::ChatAddFriend(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].ChatAddFriend(pProtocol);
}

void KProtocolProcess::ChatRefuseFriend(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].ChatRefuseFriend(pProtocol);
}

void KProtocolProcess::ChatApplyReSendAllFriendName(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].ChatResendAllFriend(pProtocol);
}

void KProtocolProcess::ChatApplySendOneFriendName(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].ChatSendOneFriendData(pProtocol);
}

void KProtocolProcess::ChatDeleteFriend(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].ChatDeleteFriend(pProtocol);
}

void KProtocolProcess::ChatReDeleteFriend(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].ChatRedeleteFriend(pProtocol);
}

void	KProtocolProcess::TradeApplyOpen(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].TradeApplyOpen(pProtocol);
}

void	KProtocolProcess::TradeApplyClose(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].TradeApplyClose(pProtocol);
}

void	KProtocolProcess::TradeApplyStart(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].TradeApplyStart(pProtocol);
}

void	KProtocolProcess::TradeMoveMoney(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].TradeMoveMoney(pProtocol);
}

void	KProtocolProcess::TradeDecision(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].TradeDecision(pProtocol);
}

void	KProtocolProcess::DialogNpc(int nIndex, BYTE * pProtocol)
{
	Player[nIndex].DialogNpc(pProtocol)	;
}

void	KProtocolProcess::TeamInviteAdd(int nIndex, BYTE * pProtocol)
{
	Player[nIndex].m_cTeam.InviteAdd(nIndex, (TEAM_INVITE_ADD_COMMAND*)pProtocol);
}
//设置光环技能
void	KProtocolProcess::ChangeAuraSkill(int nIndex, BYTE * pProtocol)
{
	SKILL_CHANGEAURASKILL_COMMAND * pCommand = (SKILL_CHANGEAURASKILL_COMMAND*) pProtocol;
	Npc[Player[nIndex].m_nIndex].SetAuraSkill(pCommand->m_nAuraSkill);
}

void	KProtocolProcess::TeamReplyInvite(int nIndex, BYTE * pProtocol)
{
	TEAM_REPLY_INVITE_COMMAND	*pReply = (TEAM_REPLY_INVITE_COMMAND*)pProtocol;
	if (!pProtocol ||
		pReply->m_nIndex <= 0 ||
		pReply->m_nIndex >= MAX_PLAYER ||
		Player[pReply->m_nIndex].m_nIndex <= 0)
		return;

	if (Npc[Player[nIndex].m_nIndex].m_SubWorldIndex != Npc[Player[pReply->m_nIndex].m_nIndex].m_SubWorldIndex)
		return;

	if (pReply->m_btResult && Player[nIndex].m_cTeam.GetCanTeamFlag() == FALSE)
	{//不能入队或组队
		SHOW_MSG_SYNC	sMsg;
		sMsg.ProtocolType = s2c_msgshow;
		sMsg.m_wMsgID = enumMSG_ID_CANNOT_ADD_TEAM;
		sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1;
		g_pServer->PackDataToClient(Player[nIndex].m_nNetConnectIdx, (BYTE *)&sMsg, sMsg.m_wLength + 1);
		return;
	}
	Player[pReply->m_nIndex].m_cTeam.GetInviteReply(pReply->m_nIndex, nIndex, pReply->m_btResult);
}

//void KProtocolProcess::ReplyPing(int nIndex, BYTE* pProtocol)
//{
//	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
//		return;
//
//	PING_COMMAND PingCmd, *pPingCmd;
//
//	pPingCmd = (PING_COMMAND *)pProtocol;
//
//	PingCmd.ProtocolType = s2c_ping;
//	PingCmd.m_dwTime = pPingCmd->m_dwTime;
//	g_pServer->SendData(Player[nIndex].m_nNetConnectIdx, &PingCmd, sizeof(PING_COMMAND));
//	Player[nIndex].m_uLastPingTime = g_SubWorldSet.GetGameTime();
//}

void KProtocolProcess::NpcCPUnlockCommand(int nIndex, BYTE* pProtocol)
{
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;

	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	PLAYER_REQUEST_CP_UNLOCK *pUnlockCmd = (PLAYER_REQUEST_CP_UNLOCK *)pProtocol;

	if (Player[nIndex].CheckChestPW(pUnlockCmd->int_PW))
	{
		Player[nIndex].SetChestLock(TRUE);  //解锁
	}
	return;
}
void KProtocolProcess::NpcCPLockCommand(int nIndex, BYTE* pProtocol)
{
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;

	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;
	//Player[nIndex].SetChestLock(FALSE);    //锁频
	PLAYER_REQUEST_CP_LOCK *pChangeCmd = (PLAYER_REQUEST_CP_LOCK *)pProtocol;

	if (pChangeCmd)
	{
		Player[nIndex].SetChestLock(FALSE);
		//if  (pChangeCmd->_isMobile)
		//    Player[nIndex].setMobileClient(true);
		//else
		//	Player[nIndex].setMobileClient(false);

		//Player[nIndex].setMobileClientLanguage(pChangeCmd->_clientVer);
	}
	return;
}
//设置密码
void KProtocolProcess::NpcCPChangeCommand(int nIndex, BYTE* pProtocol)
{
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;

	PLAYER_REQUEST_CP_CHANGE *pChangeCmd = (PLAYER_REQUEST_CP_CHANGE *)pProtocol;
	if(Player[nIndex].m_CUnlocked != 1)
		return;

	if(Player[nIndex].CheckChestPW(pChangeCmd->int_OldPW))
	{
		Player[nIndex].SetChestPW(pChangeCmd->int_NewPW);
	}			
}
//重置密码
void KProtocolProcess::NpcCPResetCommand(int nIndex, BYTE* pProtocol)
{
	PLAYER_REQUEST_CP_RESET *pResetCmd = (PLAYER_REQUEST_CP_RESET *)pProtocol;

	//printf("DEBUG, m_CUnlocked - m_nChestPW: %d-%d\n", Player[nIndex].m_CUnlocked, Player[nIndex].m_nChestPW);

	//if(Player[nIndex].m_CUnlocked == 1 || Player[nIndex].m_nChestPW > 0)  //已经解锁？
		//return;

	Player[nIndex].SetNewPW(pResetCmd->int_ResetPW);		
}

void KProtocolProcess::NpcOpenMarketCommand(int nIndex, BYTE* pProtocol)
{

     PLAYER_REQUEST_OPEN_MARKET *MMarket=(PLAYER_REQUEST_OPEN_MARKET *)pProtocol;

	if  (MMarket==NULL)
		return;

	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;

	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;
	Player[nIndex].m_BuyInfo.Clear();
	KTabFile nChCKid;
	int nRows,i,nRegid=0;
	if (MMarket->nShopType==0)
	{//特殊商城
	   if (nChCKid.Load("\\settings\\marketshop\\type.txt"))
	   { 
		 nRows=nChCKid.GetHeight();
		 for (i=2;i<nRows+1;++i) 
		 {
			 nChCKid.GetInteger(i,2,0,&nRegid);
			 if  (MMarket->nShopID==nRegid)
			 {//如果存在这个IDX
				 if (MMarket->nShopID>0)
				 {
				     //Player[nIndex].Market(nIndex,MMarket->nShopID);
					 Player[nIndex].m_BuyInfo.m_nBuyIdx  = MMarket->nShopID;
				 }
				 break;
			 }
		 }
	   }   
	}
	else
	{//奇珍阁
		if (nChCKid.Load("\\settings\\marketshop\\Newtype.txt"))
		{ 
			nRows=nChCKid.GetHeight();
			for (i=2;i<nRows+1;++i) 
			{
				nChCKid.GetInteger(i,2,0,&nRegid);
				if  (MMarket->nShopID==nRegid)
				{//如果存在这个IDX
					if (MMarket->nShopID>0)
					{
						//Player[nIndex].Market(nIndex,MMarket->nShopID);
						Player[nIndex].m_BuyInfo.m_nBuyIdx  = MMarket->nShopID;	
					}
					break;
				}
			}
	   } 
	}
	nChCKid.Clear();
}

void KProtocolProcess::NpcAutoplayCommand(int nIndex, BYTE* pProtocol) //挂机 和 抽奖 传送通用
{
	CP_AUTOPLAY 		*Autoplay=(CP_AUTOPLAY *)pProtocol;
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;
	if (Autoplay->ncishu>=0)
	    Npc[Player[nIndex].m_nIndex].SetNpcCishu(Autoplay->ncishu);  //抽奖
	else
	{
	   Player[nIndex].SAutoplay(Autoplay->nbutton,Autoplay->ByteVal,Autoplay->ByteVala);                  //Autoplay->ncishu
	}	    

	//Npc[Player[nIndex].m_nIndex].m_nIsOver=FALSE;
}

void KProtocolProcess::NpcDaTauCommand(int nIndex, BYTE* pProtocol)
{
	CP_DATAU 		*pDaTauCmd;

	pDaTauCmd = (CP_DATAU *)pProtocol;

	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;

	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	Player[nIndex].script(pDaTauCmd->nbutton,pDaTauCmd->nXuHao);
}


void KProtocolProcess::NpcSitCommand(int nIndex, BYTE* pProtocol)
{
	NPC_SIT_COMMAND *pSitCmd;

	pSitCmd = (NPC_SIT_COMMAND *)pProtocol;

	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;

	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	if (pSitCmd->m_btSitFlag)
		Npc[Player[nIndex].m_nIndex].SendSerCommand(do_sit);
	else
		Npc[Player[nIndex].m_nIndex].SendSerCommand(do_stand);
//	if (Npc[Player[nIndex].m_nIndex].m_Doing != do_sit)
//		Npc[Player[nIndex].m_nIndex].SendCommand(do_sit);
//	else
//		Npc[Player[nIndex].m_nIndex].SendCommand(do_stand);
}

void KProtocolProcess::ObjMouseClick(int nIndex, BYTE* pProtocol)
{
	if (Player[nIndex].CheckTrading())
		return;

	int		nSubWorldIdx, nRegionIdx, nObjIdx;
	int		nPlayerX, nPlayerY, nObjX, nObjY;
	OBJ_MOUSE_CLICK_SYNC 	*pObj = (OBJ_MOUSE_CLICK_SYNC*)pProtocol;

	nSubWorldIdx = Npc[Player[nIndex].m_nIndex].m_SubWorldIndex;
	nRegionIdx = SubWorld[nSubWorldIdx].FindRegion(pObj->m_dwRegionID);
	if (nRegionIdx < 0)
		return;
	nObjIdx = SubWorld[nSubWorldIdx].m_Region[nRegionIdx].FindObject(pObj->m_nObjID);
	if (nObjIdx <= 0)
		return;

	SubWorld[nSubWorldIdx].NewMap2Mps(
		Npc[Player[nIndex].m_nIndex].m_RegionIndex,
		Npc[Player[nIndex].m_nIndex].m_MapX,
		Npc[Player[nIndex].m_nIndex].m_MapY,
		Npc[Player[nIndex].m_nIndex].m_OffX,
		Npc[Player[nIndex].m_nIndex].m_OffY,
		&nPlayerX,
		&nPlayerY);
	SubWorld[nSubWorldIdx].NewMap2Mps(
		nRegionIdx,
		Object[nObjIdx].m_nMapX,
		Object[nObjIdx].m_nMapY,
		Object[nObjIdx].m_nOffX,
		Object[nObjIdx].m_nOffY,
		&nObjX,
		&nObjY);
/*
	Obj_Kind_MapObj = 0,		// 地图物件，主要用于地图动画
	Obj_Kind_Body,				// npc 的尸体
	Obj_Kind_Box,				// 宝箱
	Obj_Kind_Item,				// 掉在地上的装备
	Obj_Kind_Money,				// 掉在地上的钱
	Obj_Kind_LoopSound,			// 循环音效
	Obj_Kind_RandSound,			// 随机音效
	Obj_Kind_Light,				// 光源（3D模式中发光的东西）
	Obj_Kind_Door,				// 门类
	Obj_Kind_Trap,				// 陷阱
	Obj_Kind_Prop,				// 小道具，可重生
	Obj_Kind_Num,				// 物件的种类数
*/
	switch (Object[nObjIdx].m_nKind)
	{
	case Obj_Kind_Box:
		if (g_GetDistance(nPlayerX, nPlayerY, nObjX, nObjY) > defMAX_EXEC_OBJ_SCRIPT_DISTANCE)
			break;
		if (Object[nObjIdx].m_nState == OBJ_BOX_STATE_CLOSE)
		{
			//Npc[Player[nIndex].m_nIndex].m_nIsOver = TRUE;
			Object[nObjIdx].ExecScript(nIndex,nObjIdx,Object[nObjIdx].m_nSubWorldID);
		}
		break;
	case Obj_Kind_Door:
		break;
	case Obj_Kind_Prop:   //小道具，可重生	储物箱
		if (g_GetDistance(nPlayerX, nPlayerY, nObjX, nObjY) > defMAX_EXEC_OBJ_SCRIPT_DISTANCE)
			break;
		if (Object[nObjIdx].m_nState == OBJ_PROP_STATE_DISPLAY)
		{
		//	Npc[Player[nIndex].m_nIndex].m_nIsOver = TRUE;
			Object[nObjIdx].ExecScript(nIndex,nObjIdx,Object[nObjIdx].m_nSubWorldID);
		}
		break;
	}
}

void KProtocolProcess::StoreMoneyCommand(int nIndex, BYTE* pProtocol)
{
	STORE_MONEY_COMMAND*	pCommand = (STORE_MONEY_COMMAND *)pProtocol;

	if (pCommand->m_byDir)	// 取钱
		Player[nIndex].m_ItemList.ExchangeMoney(room_repository, room_equipment, pCommand->m_dwMoney);
	else					// 存钱
		Player[nIndex].m_ItemList.ExchangeMoney(room_equipment, room_repository, pCommand->m_dwMoney);

}

//左键技能和外挂公用一个协议
void KProtocolProcess::c2sskillsync(int nIndex, BYTE* pProtocol)
{
	SKILL_LEFT_SYNC*	pCommand = (SKILL_LEFT_SYNC *)pProtocol;

    if  (pCommand->m_Type==0) //左键技能同步
	   Player[nIndex].SetCurLeftSkillID(pCommand->m_nLeftskill);
	//else if (pCommand->m_Type==1)  //外挂标示同步
    //   Npc[Player[nIndex].m_nIndex].m_IsWaiGua=pCommand->m_nLeftskill;

}


//服务器端  处理封挂
void KProtocolProcess::c2sguaforbit(int nIndex, BYTE* pProtocol)
{
    NPC_GUAFORBIT_COMMAND*	pCommand = (NPC_GUAFORBIT_COMMAND *)pProtocol;
  if (pCommand->GuaType==0 && Player[nIndex].m_nNetConnectIdx >= 0) //退出游戏
  { 
		  printf("[封外挂] GM Kick out (%s).\n",Npc[Player[nIndex].m_nIndex].Name);
		  g_pServer->ShutdownClient(Player[nIndex].m_nNetConnectIdx);
  } 
  else if (pCommand->GuaType==1)  // 大牢修养
  {  
	      //printf("[封外挂](%s)进入大牢.\n",Npc[Player[nIndex].m_nIndex].Name);
          //Npc[Player[nIndex].m_nIndex].m_IsWaiGua=1;
          //Npc[Player[nIndex].m_nIndex].ChangeWorld(208,204*8*32,201*16*32);
  } 
}


//服务器端  玩家重生
void KProtocolProcess::NpcReviveCommand(int nIndex, BYTE* pProtocol)
{
	NPC_REVIVE_COMMAND*	pCommand = (NPC_REVIVE_COMMAND *)pProtocol;
      if (pCommand->ReviveType==0) // 原地复活
	  { 
		if (Npc[Player[nIndex].m_nIndex].m_CurrentautoReviverate>0) //带复活戒子
           Player[nIndex].Revive(pCommand->ReviveType);             //带参数的重生方式
		else
		{//回城复活
			Player[nIndex].Revive(1);
			/*///////////////////////测试信息//////////////////
			SHOW_MSG_SYNC	sMsg;
			sMsg.ProtocolType = s2c_msgshow;
			sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID);
			sMsg.m_wMsgID = enumMSG_ID_FUHUO_FAILED;  //
			g_pServer->PackDataToClient(Player[nIndex].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
	       *//////////////////////////////////////////////////
		}
	  }
	  else if (pCommand->ReviveType==1)  // 回城复活
		  Player[nIndex].Revive(pCommand->ReviveType); //带参数的重生方式
	  else
	  {//其他情况回城复活
		  Player[nIndex].Revive(1);
	  }
	
} 

void KProtocolProcess::c2sTradeReplyStart(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].c2sTradeReplyStart(pProtocol);
}

void KProtocolProcess::c2sPKApplyChangeNormalFlag(int nIndex, BYTE* pProtocol)
{
	PK_APPLY_NORMAL_FLAG_COMMAND	*pApply = (PK_APPLY_NORMAL_FLAG_COMMAND*)pProtocol;
	Player[nIndex].m_cPK.SetNormalPKState(pApply->m_btFlag);
}

void KProtocolProcess::c2sPKApplyEnmity(int nIndex, BYTE* pProtocol)
{
	PK_APPLY_ENMITY_COMMAND	*pApply = (PK_APPLY_ENMITY_COMMAND*)pProtocol;
	if (Player[nIndex].__forbitEnmity || (Player[nIndex].m_nIndex && !Npc[Player[nIndex].m_nIndex].m_FightMode))
	{
		SHOW_MSG_SYNC	sMsg;
		sMsg.ProtocolType = s2c_msgshow;
		sMsg.m_wMsgID = enumMSG_ID_PK_ERROR_1;
		sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1;
		g_pServer->PackDataToClient(Player[nIndex].m_nNetConnectIdx, (BYTE *)&sMsg, sMsg.m_wLength + 1);
		return;
	}

	if (pApply->m_dwNpcID <= 0 || pApply->m_dwNpcID >= MAX_NPC)
		return;
	int nNpcIdx = Player[nIndex].FindAroundNpc(pApply->m_dwNpcID);
	if (nNpcIdx <= 0)
		return;
	if (Npc[nNpcIdx].m_Kind != kind_player || Npc[nNpcIdx].GetPlayerIdx() <= 0)
		return;

	Player[nIndex].m_cPK.EnmityPKOpen(Npc[nNpcIdx].GetPlayerIdx());
}

void KProtocolProcess::LadderQuery(int nIndex, BYTE* pProtocol)
{
	LADDER_QUERY*	pLQ = (LADDER_QUERY *)pProtocol;
	if (nIndex > 0 && nIndex < MAX_PLAYER)
	{
		int lnID = Player[nIndex].m_nNetConnectIdx;

		if (lnID >= 0)
		{
			LADDER_DATA	LadderData;
			LadderData.ProtocolType = s2c_ladderresult;
			LadderData.dwLadderID = pLQ->dwLadderID;  //获取排名的索引 0 1 2 3 4 种
			void* pData = (void *)Ladder.GetTopTen(LadderData.dwLadderID);
			if (pData)
			{
				memcpy(LadderData.StatData, pData, sizeof(LadderData.StatData));

                /*//xep hang
                printf("Debug: LadderID = %d\n", LadderData.dwLadderID);
                for (int i = 0; i < 10; ++i) // co 10 muc tu StatData
                {  
                    printf("STT: [%d] Name= %s nValue= %d, bySort= %d\n", i, LadderData.StatData[i].Name,LadderData.StatData[i].nValue,LadderData.StatData[i].bySort);
                }*/

				g_pServer->PackDataToClient(lnID, &LadderData, sizeof(LadderData));
			}
		}
	}
}
//服务器端 修理
void KProtocolProcess::ItemRepair(int nIndex, BYTE* pProtocol)
{
	ITEM_REPAIR	*pIR = (ITEM_REPAIR *)pProtocol;

	if (nIndex > 0 && nIndex < MAX_PLAYER)
	{
		Player[nIndex].RepairItem(pIR->dwItemID);
	}
}

void KProtocolProcess::ItemBreak(int nIndex, BYTE* pProtocol)
{
	ITEM_BREAK *PIB = (ITEM_BREAK *)pProtocol;
	if (nIndex > 0 && nIndex < MAX_PLAYER)
	{
			Player[nIndex].BreakItem(PIB->dwItemID,PIB->nNum);
	}
}
//服务端
void KProtocolProcess::NpcRideCommand( int nIndex, BYTE* pProtocol )
{
	NPC_RIDE_COMMAND *pRideCmd;
	
	pRideCmd = (NPC_RIDE_COMMAND *)pProtocol;
	
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	if (Npc[Player[nIndex].m_nIndex].m_HorseType != -1)
	{//骑着马
		if(Npc[Player[nIndex].m_nIndex].m_RideState <= 0)
		{
			Npc[Player[nIndex].m_nIndex].m_bRideHorse = !Npc[Player[nIndex].m_nIndex].m_bRideHorse; //赋值反值
			Npc[Player[nIndex].m_nIndex].m_RideState = 50;
		}
		else
		{
			SHOW_MSG_SYNC	sMsg;
			sMsg.ProtocolType = s2c_msgshow;
			sMsg.m_wMsgID = enumMSG_ID_RIDE_CANNOT;
			sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1;
			g_pServer->PackDataToClient(Player[nIndex].m_nNetConnectIdx, (BYTE *)&sMsg, sMsg.m_wLength + 1);
			return;
		}
	}	
	
	Player[nIndex].UpdataCurData();
// 
// 	PLAYER_ATTRIBUTE_SYNC	sSync;
// 	sSync.ProtocolType = s2c_playersyncattribute;
// 	sSync.m_btAttribute = 2;
// 	sSync.m_nBasePoint = Player[nIndex].m_nVitality;
// 	sSync.m_nCurPoint = Player[nIndex].m_nCurVitality;
// 	sSync.m_nLeavePoint = Player[nIndex].m_nAttributePoint;
// 	g_pServer->PackDataToClient(Player[nIndex].m_nNetConnectIdx, (BYTE*)&sSync, sizeof(PLAYER_ATTRIBUTE_SYNC));

}

//熔炼纹钢
void KProtocolProcess::c2sRongItem( int nIndex, BYTE* pProtocol ) //装备熔炼
{
	PLAYER_RONGITEM *pCI = (PLAYER_RONGITEM *)pProtocol;

}
//服务器删除某物品
void  KProtocolProcess::c2splayerdelitem(int nIndex, BYTE* pProtocol)
{
     PLAYER_DELL_ITEM *pItem = (PLAYER_DELL_ITEM *)pProtocol;

	 if (nIndex > 0 && nIndex < MAX_PLAYER)
	 { 
          int idx  =  Player[nIndex].m_ItemList.SearchID(pItem->uIiemIdx);
          Player[nIndex].m_ItemList.Remove(idx);
		  ItemSet.Remove(idx);
		  //Player[nIndex].m_ItemList.ClearRoom(room_cailiao);      
	 }
}


//服务器端增加物品
void KProtocolProcess::c2splayeradditem( int nIndex, BYTE* pProtocol )
{
	PLAYER_ADD_ITEM *pItem = (PLAYER_ADD_ITEM *)pProtocol;

	int nItemidx=pItem->uIiemId;
	int nKind   =pItem->nKind; 
	if (nKind ==0 && nIndex > 0 && nIndex < MAX_PLAYER && nItemidx>0)
	{
		//int x,y;
       // if (Player[nIndex].m_ItemList.CheckCanPlaceInEquipment(Item[nItemidx].GetWidth(), Item[nItemidx].GetHeight(), &x, &y)) //判断 包袱 的空间
		//{    
		//	Player[nIndex].m_ItemList.Add(nItemidx,pos_equiproom, x, y); //包袱增加装备
		//}
		Player[nIndex].m_ItemList.CilentAddItem(nItemidx);
	//	return;
	}
	else if (nKind ==1)
	{
		 //Player[nIndex].m_ItemList.SynPlayerItem(nItemidx);
	     BuySell.SynShopItemInfo(nIndex,pItem->nX,pItem->nY);
	}

//	return;
}
//铸造图谱
void KProtocolProcess::c2sCompItem( int nIndex, BYTE* pProtocol ) //打造装备
{
	PLAYER_COMPITEM *pCI = (PLAYER_COMPITEM *)pProtocol;

	if (nIndex > 0 && nIndex < MAX_PLAYER)
	{
		switch(pCI->bKind)
		{
		 case 1://合成
			{
				int nId1 = Player[nIndex].m_ItemList.SearchID(pCI->uId[0]);
				int nId2 = Player[nIndex].m_ItemList.SearchID(pCI->uId[1]);
				int nId3 = Player[nIndex].m_ItemList.SearchID(pCI->uId[2]);
				int nMoney=0;
				g_GameSetTing.GetInteger("CompoundSet","CompoundMoney",0,&nMoney);    
				if (Player[nIndex].m_ItemList.GetsMon(room_equipment) < nMoney)
				{
					char strMoney[32];
					snprintf(strMoney, sizeof(strMoney),"提示:银两不足%d两",nMoney);
					Player[nIndex].m_ItemList.msgshow(strMoney);
					return;
				}		
		        Player[nIndex].Pay(nMoney);
				if (nId1 && nId2 && nId3)
				{
					if (!Player[nIndex].ComItem(nId1,nId2,nId3,1))
					{
						/*Player[nIndex].m_ItemList.Remove(nId1);
						ItemSet.Remove(nId1);
						Player[nIndex].m_ItemList.Remove(nId2);
						ItemSet.Remove(nId2);
						Player[nIndex].m_ItemList.Remove(nId3);
					    ItemSet.Remove(nId3);
						Player[nIndex].m_ItemList.msgshow("提示:提炼玄晶矿石失败!");*/
						return;
					}
					//Player[nIndex].m_ItemList.msgshow("提示:提炼玄晶成功!恭喜!恭喜!");
					//暂时不加。
				}
				else
				{
					Player[nIndex].m_ItemList.msgshow("提示:数据有误,请联系GM处理!");
					return;
				}
			}
			break;
		case 2:			//升级玄晶
			{
				int nId1 = Player[nIndex].m_ItemList.SearchID(pCI->uId[0]);
				int nId2 = Player[nIndex].m_ItemList.SearchID(pCI->uId[1]);
				int nId3 = Player[nIndex].m_ItemList.SearchID(pCI->uId[2]);
				int nMoney=0;
				g_GameSetTing.GetInteger("CompoundSet","CompoundMoney",0,&nMoney);    
				if (Player[nIndex].m_ItemList.GetsMon(room_equipment) < nMoney)
				{
					char strMoney[32];
					snprintf(strMoney, sizeof(strMoney),"提示:银两不足%d两",nMoney);
					Player[nIndex].m_ItemList.msgshow(strMoney);
					return;
				}		
		        Player[nIndex].Pay(nMoney);
				if (nId1 && nId2 && nId3)
				{
					/*if (Item[nId1].GetParticular() != Item[nId2].GetParticular() || 
						Item[nId1].GetParticular() != Item[nId3].GetParticular())
					{
						Player[nIndex].m_ItemList.msgshow("提示:材料不对,品质不相同!");
						return;
					}
					
					if (Item[nId1].GetLevel() != Item[nId2].GetLevel() || 
						Item[nId1].GetLevel() != Item[nId3].GetLevel())
					{
				        Player[nIndex].m_ItemList.msgshow("提示:材料不对,品质不相同!");
						return;//物品错误
					}*/

					

					if (!Player[nIndex].ComItem(nId1,nId2,nId3,2))
					{
						/*Player[nIndex].m_ItemList.Remove(nId1);
						ItemSet.Remove(nId1);
						Player[nIndex].m_ItemList.Remove(nId2);
						ItemSet.Remove(nId2);
						Player[nIndex].m_ItemList.Remove(nId3);
					    ItemSet.Remove(nId3);
						Player[nIndex].m_ItemList.msgshow("提示:升级玄晶矿石失败!");*/	
						return;
					}

					//Player[nIndex].m_ItemList.msgshow("提示:玄晶升级成功!恭喜!恭喜!");
					//暂时不加。
				}
				else
				{
					Player[nIndex].m_ItemList.msgshow("提示:数据有误,请联系GM处理!");
					return;
				}

			}
			break;
		case 3:			//升级属性矿石
			{
				int nId1 = Player[nIndex].m_ItemList.SearchID(pCI->uId[0]);
				int nId2 = Player[nIndex].m_ItemList.SearchID(pCI->uId[1]);
				int nId3 = Player[nIndex].m_ItemList.SearchID(pCI->uId[2]);
				int nMoney=0;
				g_GameSetTing.GetInteger("CompoundSet","CompoundMoney",0,&nMoney);    
				if (Player[nIndex].m_ItemList.GetsMon(room_equipment) < nMoney)
				{
					char strMoney[32];
					snprintf(strMoney, sizeof(strMoney),"提示:银两不足%d两",nMoney);
					Player[nIndex].m_ItemList.msgshow(strMoney);
					return;
				}

		        Player[nIndex].Pay(nMoney);

				if (nId1 && nId2 && nId3)
				{					
					/*if (Item[nId1].GetParticular() != Item[nId2].GetParticular() || 
						Item[nId1].GetParticular() != Item[nId3].GetParticular())
					{
						Player[nIndex].m_ItemList.msgshow("提示:材料不对,类型不相同!");	
						return;
					}
					
					if (Item[nId1].GetLevel() != Item[nId2].GetLevel() || 
						Item[nId1].GetLevel() != Item[nId3].GetLevel())
					{
						Player[nIndex].m_ItemList.msgshow("提示:材料不对,品质不相同!");	
						return;//物品错误
					}
					
					if (Item[nId1].GetParticular() % 2 == 1)
						if (Item[nId1].GetSeries() != Item[nId2].GetSeries() || 
							Item[nId1].GetSeries() != Item[nId3].GetSeries())
						{
							Player[nIndex].m_ItemList.msgshow("提示:材料不对,五行不相同!");
							return;//物品错误
						}
                    */
					if (!Player[nIndex].ComItem(nId1,nId2,nId3,3))  //提炼矿石
					{
						/*Player[nIndex].m_ItemList.Remove(nId1);
						ItemSet.Remove(nId1);
						Player[nIndex].m_ItemList.Remove(nId2);
						ItemSet.Remove(nId2);
						Player[nIndex].m_ItemList.Remove(nId3);
					    ItemSet.Remove(nId3);
						Player[nIndex].m_ItemList.msgshow("提示:升级属性矿石失败!");*/
						return;
					}

					//Player[nIndex].m_ItemList.msgshow("提示:升级属性矿石成功!恭喜!恭喜!");
					//暂时不加。
				}
				else
				{
					Player[nIndex].m_ItemList.msgshow("提示:数据有误!请联系GM处理!");
				}
			}
			break;
		case 4://蓝装-提取属性矿石
			{
				int nIdx[11],i;

				for (i = 0;i < 11;++i)
				{
					nIdx[i] = Player[nIndex].m_ItemList.SearchID(pCI->uId[i]);
				}
				int nMoney=0;
				g_GameSetTing.GetInteger("CompoundSet","DistillMoney",0,&nMoney);    
				if (Player[nIndex].m_ItemList.GetsMon(room_equipment) < nMoney)
				{
					char strMoney[32];
					snprintf(strMoney, sizeof(strMoney),"提示:银两不足%d两",nMoney);
					Player[nIndex].m_ItemList.msgshow(strMoney);
					return;
				}		
		        Player[nIndex].Pay(nMoney);

				if (nIdx[0] && nIdx[1] && nIdx[2])
				{

					if (Item[nIdx[2]].GetParticular() % 2 == 0 &&                 //取余暗属性
						Item[nIdx[2]].GetSeries() != Item[nIdx[0]].GetSeries()) //暗属性 五行不同 和 偶数
					{
						Player[nIndex].m_ItemList.msgshow("提示:提取失败,五行不相符!");	
						return;
					}
					
					//===========检测辅助物品======================
						int nMin = 0; //成功率？
						int nItemRows=g_CompEquip.GetHeight();
						for (i = 3;i <11;++i) //辅助物品
						{
							if (nIdx[i])
							{ 
								for (int r=11;r<nItemRows;++r)
								{
									int nIteminfo[3],val=0;
									g_CompEquip.GetInt3s(r,"Distill",nIteminfo);
									g_CompEquip.GetInteger(r,"Distill_value",0,&val);
									
									if (Item[nIdx[i]].GetGenre()==nIteminfo[0] && 
										Item[nIdx[i]].GetDetailType()==nIteminfo[1]&&
										Item[nIdx[i]].GetParticular()==nIteminfo[2])
									{//如果可选材料相同
										nMin +=val;
										break;
									}
								}		
							}
							continue;
						} 
						//=========检测辅助物品添加的成功率==================
						int nRank,nValRank,nzMaxRank;
						g_GameSetTing.GetInteger2("CompoundSet","DistillRank",&nValRank,&nRank); //玄晶每等级价值量
						
						if (nMin <= 0)
						{ 
							nMin = 1;  //
						} 	
						nzMaxRank = nMin + nValRank;                     //材料的总价值量	
						if (g_Random(nzMaxRank) < g_Random(nRank))
						{
							for (i=0;i<11;++i)
							{
								if (nIdx[i])
								{
								   Player[nIndex].m_ItemList.Remove(nIdx[i]);  //移除材料
								   ItemSet.Remove(nIdx[i]);
								}
							}

							Player[nIndex].m_ItemList.msgshow("提示:提取失败,消耗材料");
							return;  
						} 
						//--------------------------------------------------------
						for (i = 3;i < 11;++i)
						{
							if (nIdx[i])
							{
							   Player[nIndex].m_ItemList.Remove(nIdx[i]);     //删除可选材料
							   ItemSet.Remove(nIdx[i]);
							}
						}
						
						if (!Player[nIndex].ComDiyItem(nIdx[0],nIdx[1],nIdx[2],1,nMin))   //开始提取 属性石头
						{
							//Player[nIndex].m_ItemList.msgshow("提示:提取失败,消耗材料");		
							return;
						}
						
					   //Player[nIndex].m_ItemList.msgshow("提示:提取成功,恭喜!恭喜!");
					   //暂时不加。
				}
				else
				{
					   Player[nIndex].m_ItemList.msgshow("提示:数据有误,请联系GM处理!");
				}
			}
			break;
		case 5://紫装打孔
			{
				int nId1 = Player[nIndex].m_ItemList.SearchID(pCI->uId[0]);
				int nId2 = Player[nIndex].m_ItemList.SearchID(pCI->uId[1]);
                int nMoney=0;
				g_GameSetTing.GetInteger("CompoundSet","ForgeMoney",0,&nMoney);    
				if (Player[nIndex].m_ItemList.GetsMon(room_equipment) < nMoney)
				{
					char strMoney[32];
					snprintf(strMoney, sizeof(strMoney),"提示:银两不足%d两",nMoney);
					Player[nIndex].m_ItemList.msgshow(strMoney);
					return;
				}		
		        Player[nIndex].Pay(nMoney);
		        if (nId1<=0 || nId2<=0)
				{
				    Player[nIndex].m_ItemList.msgshow("提示:数据有误,请联系GM处理提!");
                    return;
				}
				if (!Player[nIndex].ComDiyItem(nId1,nId2,0,2,0)) //开始熔炼蓝装打孔  2 为打造紫装
				{
					    //Player[nIndex].m_ItemList.msgshow("提示:打造失败,消耗材料!");						
						return;
				}
				//Player[nIndex].m_ItemList.msgshow("提示:打造成功,恭喜!恭喜!");
			} 
			break;
		case 6:	//镶嵌紫装
			{
			           int nIdx[11],i;			
				       for (i = 0;i <11;++i)
					   {   
				           nIdx[i] = Player[nIndex].m_ItemList.SearchID(pCI->uId[i]);
					   } 

                       int nMoney=0;
				       g_GameSetTing.GetInteger("CompoundSet","EnchaseMoney",0,&nMoney);    
				       if (Player[nIndex].m_ItemList.GetsMon(room_equipment) < nMoney)
					   { 
					     char strMoney[32];
					     snprintf(strMoney, sizeof(strMoney),"提示:银两不足%d两",nMoney);
					     Player[nIndex].m_ItemList.msgshow(strMoney);
					     return;
					   } 		
		               Player[nIndex].Pay(nMoney);
				       if (nIdx[0]<=0 || nIdx[1]<=0 || nIdx[2]<=0) 
					   {  
					       Player[nIndex].m_ItemList.msgshow("提示:数据有误,请联系GM处理!");
                           return;
					   }  

						if (Item[nIdx[2]].GetParticular() % 2 != 0 &&                //暗属性
							Item[nIdx[2]].GetSeries() != Item[nIdx[0]].GetSeries())  //玄铁矿五行对比
						{
							Player[nIndex].m_ItemList.msgshow("提示:暗属性,五行不符合!");		
							return;
						}
						//===========检测辅助物品======================
						int nMin = 0; //成功率？
						int nItemRows=g_CompEquip.GetHeight();
						for (i = 3;i <11;++i) //辅助物品
						{
							if (nIdx[i])
							{ 
								for (int r=11;r<nItemRows;r++)
								{
									int nIteminfo[3],val;
									g_CompEquip.GetInt3s(r,"Enchase",nIteminfo);
									g_CompEquip.GetInteger(r,"Enchase_value",0,&val);
									
									if (Item[nIdx[i]].GetGenre()==nIteminfo[0] && 
										Item[nIdx[i]].GetDetailType()==nIteminfo[1]&&
										Item[nIdx[i]].GetParticular()==nIteminfo[2])
									{//如果可选材料相同
										nMin +=val;
										break;
									}
								}		
							}
							continue;
						} 
						//=========检测辅助物品添加的成功率==================
						int nRank,nValRank,nzMaxRank;
						g_GameSetTing.GetInteger2("CompoundSet","EnchaseRank",&nValRank,&nRank);          //玄晶每等级价值量
						nzMaxRank = nMin + nValRank;             //材料的总价值量	
						if (nMin <= 0)
						{ 
							nMin = 1;  //
						} 
						
						if (g_Random(nzMaxRank) < g_Random(nRank))
						{
							for (i=1;i<11;++i)
							{
							   if (nIdx[i])
							   {
								 Player[nIndex].m_ItemList.Remove(nIdx[i]);  //移除材料
								 ItemSet.Remove(nIdx[i]);	
						       }
							}
							Player[nIndex].m_ItemList.msgshow("提示:紫装镶嵌失败!");
							return;  
						} 
						//////////////////////////////////////////////////////
						for (i = 3;i < 11;++i) //移除辅助物品
						{
						    if (nIdx[i])
					    	{
							  Player[nIndex].m_ItemList.Remove(nIdx[i]); //移除
							  ItemSet.Remove(nIdx[i]);
					    	}
						}
						
						if (!Player[nIndex].ComDiyItem(nIdx[0],nIdx[1],nIdx[2],3,nMin))  //开始镶套
						{	
							//Player[nIndex].m_ItemList.msgshow("提示:镶嵌失败!");	
							return;
						}
						// 打造成功
						 //  Player[nIndex].m_ItemList.msgshow("提示:镶嵌成功!,恭喜!恭喜!");			
			}
			break;
		case 7:	//图谱
			{

				int nIdx[16],i;
				char nItemStr[128]={0};
				for (i = 0;i < 16;++i)
				{
					char nVal[32]={0};
					nIdx[i] = Player[nIndex].m_ItemList.SearchID(pCI->uId[i]);
					if (i==15) //最后一个
						sprintf(nVal,"%d",nIdx[i]);
					else
						sprintf(nVal,"%d,",nIdx[i]);

					strcat_s(nItemStr, nVal);

				}
                int nMoney=0;
				g_GameSetTing.GetInteger("CompoundSet","AtlasMoney",0,&nMoney);    
				if (Player[nIndex].m_ItemList.GetsMon(room_equipment) < nMoney)
				{ 
					char strMoney[32];
					snprintf(strMoney, sizeof(strMoney),"提示:银两不足%d两",nMoney);
				    Player[nIndex].m_ItemList.msgshow(strMoney);
				    return;
				}
				
				if (nIdx[0]<=0 || nIdx[1]<=0 || nIdx[2]<=0 || nIdx[3]<=0 || nIdx[4]<=0 || nIdx[5]<=0 || nIdx[6]<=0 || nIdx[7]<=0)
				{//nIdx[6],nIdx[7] 图谱 和 玄晶
			
					Player[nIndex].m_ItemList.msgshow("提示:数据有误,请联系GM处理!");		
					return;
				}

				if (!pCI->mFuMoVal)
				{
				    Player[nIndex].Pay(nMoney);
				    Player[nIndex].ExeNoBackScript("\\script\\system\\altscomp.lua","mainaltscomp",0,0,0,0,nItemStr);
				}
				else
				{//预览
					Player[nIndex].ExeNoBackScript("\\script\\system\\altscomp.lua","yulanaltscomp",0,0,0,0,nItemStr);
				}

				return;

				int nLevel = Item[nIdx[7]].GetLevel();  //玄晶的等级
				//0--5为图谱中的要求材料  6 为图谱 7为玄晶  8-15 为辅助材料

                  //Item[nIdx[6]].GetGenre();        //大类
				  //Item[nIdx[6]].GetDetailType();   //具体类别
				  //Item[nIdx[6]].GetParticular();   //详细类别
				  //Item[nIdx[6]].GetLevel();        //等级
				  //Item[nIdx[6]].GetSeries();       //五行

                  int nRows =g_CompAtlas.GetHeight(),nThisRows=0;
                  int nCaiLiaoVal[7],nAtlasVal[4];
                   for (i=0;i<nRows;++i)
				   {
                      g_CompAtlas.GetInteger3(i+1,"ATLAS_GENRE","ATLAS_DETAILTYPE","ATLAS_PARTICULAR",0,nAtlasVal);
				      g_CompAtlas.GetInteger6(i+1,"1_GENRE","1_DETAILTYPE","1_PARTICULAR","1_LEVEL","1_SERIES","1_MAGIC_ID",0,nCaiLiaoVal); //第三个材料的信息
					  if (Item[nIdx[6]].GetParticular()==nAtlasVal[2] && nCaiLiaoVal[2]!=0)
					  {//获取图谱对应的 行数
                         nThisRows=i+1;
						 break;
					  }
				   }

				   ZeroMemory(nCaiLiaoVal,sizeof(nCaiLiaoVal));
				   ZeroMemory(nAtlasVal,sizeof(nAtlasVal));

				   if (nThisRows<=0)
				   {
					   Player[nIndex].m_ItemList.msgshow("提示:该<c=yellow>图谱<c>已经失效!");		
					   return;
				   }

                   //检测材料
				   int isThisCailiao=0,IsNo=0;
				   int Genre,Detail,Particular;
				  // int nCaiInfo[6];//用于储存材料的 编号 用于判断是否有重复的
				   for (i=0;i<6;++i) //合成的材料
				   {//第几个材料的信息
				      Genre	     = Item[nIdx[i]].GetGenre();        //大类
				      Detail     = Item[nIdx[i]].GetDetailType();   //具体类别
				      Particular = Item[nIdx[i]].GetParticular();   //详细类别
			  	      int Level      = Item[nIdx[i]].GetLevel();        //等级
				      int Series     = Item[nIdx[i]].GetSeries();       //五行
                      int MagocId    = Item[nIdx[i]].GetBaseMagic();
                      int IsHaveThis = 0;   //材料 是否和 图谱中的需要的材料中有相同的

                        for (int g=0;g<6;++g)
						{//图谱第几个条件材料的需求信息
							int IsLevel=0,IsSeries=0,IsMagocId=0;
							char nGenre[16],nDetail[16]	,nParticular[16],nLevel[16],nSeries[16],nMagocId[16];
                            snprintf(nGenre, sizeof(nGenre),"%d_GENRE",g+1);
							snprintf(nDetail, sizeof(nDetail),"%d_DETAILTYPE",g+1);
							snprintf(nParticular, sizeof(nParticular),"%d_PARTICULAR",g+1);
							snprintf(nLevel, sizeof(nLevel),"%d_LEVEL",g+1);
							snprintf(nSeries, sizeof(nSeries),"%d_SERIES",g+1);
							snprintf(nMagocId, sizeof(nMagocId),"%d_MAGIC_ID",g+1);
                            g_CompAtlas.GetInteger6(nThisRows,nGenre,nDetail,nParticular,nLevel,nSeries,nMagocId,0,nCaiLiaoVal); //材料的信息
														
							if (Genre==nCaiLiaoVal[0] && Detail==nCaiLiaoVal[1] && Particular==nCaiLiaoVal[2])
							{//找到了材料是同种类型
								IsHaveThis=1;      //标示是否是图谱所需要的材料
								if (nCaiLiaoVal[3]==0 && nCaiLiaoVal[4]==0 && nCaiLiaoVal[5]==0)
								{//再对比等级 五行 魔法ID 都为零的话 就不需要判断了 直接符合条件
                                     isThisCailiao++;   //书籍都为0
								}

							    if (nCaiLiaoVal[3]>0)
								{//如果条件材料等级大于0的话，就要检查
									if (Level==nCaiLiaoVal[3])
									{//检查等级是否相同
										IsLevel=1;
									}
									else
									{
										char msg[125];
										sprintf(msg,"提示:<c=yellow>材料品质<c=green>%d<c>不对->%s",i,Item[nIdx[i]].GetName());
										Player[nIndex].m_ItemList.msgshow(msg);
										IsNo++;
									}
                                    
								} 
								else
								{//等于0时
                                    IsLevel=1;
								}

								if (nCaiLiaoVal[4]>0)
								{//如果条件材料五行大于0的话，就要检查
									if (Series==nCaiLiaoVal[4])
									{//检查五行是否相同
										IsSeries=1;
									}
									else
									{
										char msg[125];
										sprintf(msg,"提示:<c=yellow>材料五行<c=green>%d<c>不对->%s",i,Item[nIdx[i]].GetName());
										Player[nIndex].m_ItemList.msgshow(msg);
										IsNo++;
									}	
								} 
                                else
								{//等于0时
                                    IsSeries=1;
								}

								if (nCaiLiaoVal[5]>0)
								{//如果条件材料魔法ID大于0的话，就要检查
									if (MagocId==nCaiLiaoVal[5])
									{//检查魔法ID是否相同
										IsMagocId=1;
									}
									else
									{
										char msg[125];
										sprintf(msg,"提示:<c=yellow>材料属性<c=green>%d<c>不对->%s",i,Item[nIdx[i]].GetName());
										Player[nIndex].m_ItemList.msgshow(msg);
										IsNo++;
									}
									
								} 	
                                else
								{//等于0时
									IsMagocId=1;
								}

								// IsLevel is set to 1 above only when the material's level matches; `=' here wrote 1
								// over it, so the first conjunct was the literal and material counted toward the six
								// even when its level did not match.
								if (IsLevel==1 && IsSeries==1 && IsMagocId==1)
								{//最多为6
									isThisCailiao++;
								}
								else
								{

								}

							}
						} // g 循环结束

						if (IsHaveThis!=1)
						{
							char msg[64];
							snprintf(msg, sizeof(msg),"提示:<c=yellow>材料(%s)不对<c>,请参照图谱说明!",Item[nIdx[i]].GetName());
							Player[nIndex].m_ItemList.msgshow(msg);	
							IsNo++;
					        break;
						}

						if (IsNo)
						{
						   break;
						} 
					//开始检测是否有相同的存在
					/*	int Genre	     = Item[nIdx[i]].GetGenre();        //大类
						int Detail     = Item[nIdx[i]].GetDetailType();   //具体类别
						int Particular = Item[nIdx[i]].GetParticular();   //详细类别
					  */
					
				   } // i 循环结束
//=====================材料检测结束==============================================
                    if  (IsNo)
					{   
	                  return;
					}
					
				   IsNo=0;
				   Genre=0;
				   Detail=0;
				   Particular=0;

				   for (i=0;i<6;++i)
				   {
					 if (nIdx[i])
					 {
					   Genre=Item[nIdx[i]].GetGenre();
					   Detail= Item[nIdx[i]].GetDetailType();
					   Particular=Item[nIdx[i]].GetParticular();
					   int k=i;

					   while(k<6)
					   {
						 if (k>=5)
							break;
						 k++;
					     if (Item[nIdx[k]].GetGenre()==Genre && Item[nIdx[k]].GetDetailType()==Detail && Item[nIdx[k]].GetParticular()==Particular)
						 {  
						    Player[nIndex].m_ItemList.msgshow("提示:<c=yellow>材料相同<c>,请参照图谱说明!");	
						    IsNo++;
						    break;
						 } 
					   }

					   if  (IsNo)
					   {
						   break;
					   }

					 }
				   }

				   if  (IsNo)
				   {
					   return;
				   }
//======================可选材料总价值量计算=====================================
                  	int nMin = 0; //成功率？
					int nItemRows=g_CompEquip.GetHeight();
					for (i = 8;i <16;++i) //辅助物品
					{
						if (nIdx[i])
						{ 
							  for (int r=11;r<nItemRows;++r)
							  {
								  // every `Atlas' cell this loop reaches carries three numbers and GetInt3s scans three, so the
								  // third write landed outside this array and the `== nIteminfo[2]' below compared a neighbour's
								  // bytes.  The `Distill' and `Enchase' blocks above read that triple as [3]; this one was short.
								  int nIteminfo[3],val;
								  g_CompEquip.GetInt3s(r,"Atlas",nIteminfo);
								  g_CompEquip.GetInteger(r,"Atlas_value",0,&val);

								  if (Item[nIdx[i]].GetGenre()==nIteminfo[0] && 
									  Item[nIdx[i]].GetDetailType()==nIteminfo[1]&&
									  Item[nIdx[i]].GetParticular()==nIteminfo[2])
								  {//如果可选材料相同
                                           nMin +=val;
										   break;
								  }
							  }		
						}
						continue;
					}

					for (i = 8;i < 16;++i)
					{//删除辅助材料
						if (nIdx[i])
						{
						  Player[nIndex].m_ItemList.Remove(nIdx[i]);
						  ItemSet.Remove(nIdx[i]);
						}
					}
//========================概率检测=============================================
					///int nOrgVal,nEqueVal,nzMax,nLuck,nSLevel=0;
					     int nOrgVal,nEqueVal,nLuck,nzMax;
                         g_CompEquip.GetInteger(4,"Atlas_value",0,&nOrgVal);          //玄晶每等级价值量
                         g_CompAtlas.GetInteger(nThisRows,"ITEM_VALUE",0,&nEqueVal);  //装备的权衡价值量 10000
						 nLuck= Player[nIndex].m_nCurLucky; //玩家当前幸运值
						 if (nMin <= 0)
						 {//辅助材料的总价值量 
							 nMin = 1;  //
						 }
						 
				         nzMax = nMin + Item[nIdx[7]].GetLevel()*nOrgVal;             //材料的总价值量 500*等级
						 
						 /*char strIdxInfo[64]={0};  
						 sprintf(strIdxInfo,"%d,%d,%d,%d,%d,%d,%d,%d",(int)Item[nIdx[0]].GetID(),(int)Item[nIdx[1]].GetID(),(int)Item[nIdx[2]].GetID(),(int)Item[nIdx[3]].GetID(),(int)Item[nIdx[4]].GetID(),(int)Item[nIdx[5]].GetID(),(int)Item[nIdx[6]].GetID(),(int)Item[nIdx[7]].GetID());
						 Player[nIndex].m_ItemList.msgshow(strIdxInfo);*/	
						 //-----------开始执行图谱脚本------------------

						// Player[nIndex].ExecuteScriptD(g_FileName2Id("\\script\\system\\altscomp.lua"),"mainaltscomp",strIdxInfo,NULL,nzMax,0,0);
                       if (g_Random(nzMax) < g_Random(nEqueVal))
					   {	 
						  for (i=0;i<6;i++)
						  {//删除合成材料 除图谱 和 玄晶
							  if (nIdx[i])
							  {
						        Player[nIndex].m_ItemList.Remove(nIdx[i]);  //移除材料
							    ItemSet.Remove(nIdx[i]);
							  }
						  }
						 Player[nIndex].m_ItemList.msgshow("提示:图谱合成失败!");
					     return;  
					 } 
//========================属性的等级和幸运有关==================================
				    int nSLevel;
					/*if (nLuck>=10 && nLuck <=300)
						nSLevel=GetRandomNumber(1,1);
					else if (nLuck>300 && nLuck <=600)
						nSLevel=GetRandomNumber(1,2);
					else  if (nLuck>600)
						nSLevel=GetRandomNumber(1,3);
					else */
						nSLevel=GetRandomNumber(1,10);
			  
                  int nGoldinfo,mIndex=0;
                  g_CompAtlas.GetInteger(nThisRows,"DES_DETAILTYPE",0,&nGoldinfo);

                  if (nGoldinfo>0)
				  {   
					  int nMagcLevel[6];
                          nMagcLevel[0]=nSLevel;
                          nMagcLevel[1]=nSLevel;
                          nMagcLevel[2]=nSLevel;
                          nMagcLevel[3]=nSLevel;
                          nMagcLevel[4]=nSLevel;
                          nMagcLevel[5]=nSLevel;
					  int nSerise = GetRandomNumber(0,4);
                          mIndex  = ItemSet.AddGold(nGoldinfo,nMagcLevel,nSerise,0,0,0,0,0,0,0,NULL,NULL,nLevel); 
				  } 

				  if (mIndex <= 0)
				  {	
					  Player[nIndex].m_ItemList.msgshow("提示:数据有误,请联系GM处理!");
					  return;  //合成失败
				  }

				  //-------开始删除合成材料---------------
				  for (i=0;i<8;i++)
				  {//删除合成材料 和图谱 和 玄晶
					  if (nIdx[i])
					  {
						  Player[nIndex].m_ItemList.Remove(nIdx[i]);  //移除材料
						  ItemSet.Remove(nIdx[i]);
					  }
				  }
				  //--------------------------------------
				  
				  int x, y;
				  if (Player[nIndex].m_ItemList.CheckCanPlaceInEquipment(Item[mIndex].GetWidth(), Item[mIndex].GetHeight(), &x, &y)) //判断 包袱 的空间
				  {
					  Player[nIndex].m_ItemList.Add(mIndex, pos_equiproom, x, y); //包袱增加装备
				      Player[nIndex].m_ItemList.msgshow("提示:图谱黄金合成成功!恭喜!恭喜!");
				  }
                  else
				  {//包袱不足    
					  Player[nIndex].m_ItemList.Remove(mIndex);
					  ItemSet.Remove(mIndex);
                      Player[nIndex].m_ItemList.msgshow("提示:遗憾!包袱空间不足!前功尽弃啦!");
				  }	
			}
			break;
	  case 8://纹钢
		   { 
			   int nId1 = Player[nIndex].m_ItemList.SearchID(pCI->uId[0]);
			   int nId2 = Player[nIndex].m_ItemList.SearchID(pCI->uId[1]);
			   int nMoney=0;

               if (nId1<=0 || nId2<=0)
			   {
				   Player[nIndex].m_ItemList.msgshow("提示:数据有误,请联系GM处理提!");
				   return;
			   }

			   Player[nIndex].Pay(nMoney);

			   int nReg = 0;
			       nReg = Player[nIndex].ExeNoBackScript("\\script\\system\\fusioncomp.lua","mainfusioncomp",0,nId1,nId2,0);

			   //if (nReg==0)
				   return;

               int nMagic=0,i;
	           for (i = 0;i < 6;i++)
	      	   {
		          if (Item[nId1].GetDetailType()==Item[nId2].GetAttribTypeVala(i))
			      {   
			         nMagic++;
		          }
	           }

		       if (nMagic<= 0) 
		       { //可出现该属性的装备类型
		         Player[nIndex].m_ItemList.msgshow("提示:此纹钢与装备类型不符合!");
		         return;
		       }
			   g_GameSetTing.GetInteger("CompoundSet","SmeltMoney",0,&nMoney);    
			   if (Player[nIndex].m_ItemList.GetsMon(room_equipment) < nMoney)
			   {
				   char strMoney[32];
				   snprintf(strMoney, sizeof(strMoney),"提示:银两不足%d两",nMoney);
				   Player[nIndex].m_ItemList.msgshow(strMoney);
				   return;
			   }		
			   int nMagRow = Item[nId2].GetSkillType();

               if (nMagRow<=0)
			   {
				   Player[nIndex].m_ItemList.msgshow("提示:此纹钢已经失效,请联系GM处理提!");
				   return;
			   }

               int nLevel  = Item[nId2].GetLevel(),nRows,nIsThisRow=0;

			   KTabFile nWenGamg;
			   if (!nWenGamg.Load(TABFILE_MAGICAGOLD_PATH))
			   {
				   Player[nIndex].m_ItemList.msgshow("提示:补丁有误,请联系GM处理提!");
				   return;
			   }
				
                   /*nRows=nWenGamg.GetHeight();
                   for(int i=0;i<nRows;i++)
				   {
				       int nMagid,nSkllType;
                           nWenGamg.GetInteger(i+1,"MAGIC_ID",0,&nMagid);
						   nWenGamg.GetInteger(i+1,"MAGIC_TTPE",0,&nSkllType);
						   if (nMagIdx==nMagid && nSkllType==1)
						   {//找到这个魔法ID在属性表中的行数
                              nWenGamg.GetInteger(i+1,"MAGATTRLVL_START",0,&nIsThisRow);
							  break;
						   }
				   }*/
                   nWenGamg.Clear();
/*
                   if (nIsThisRow<=0)
				   {
					   Player[nIndex].m_ItemList.msgshow("提示:魔法有误,请联系GM处理提!");
				       return;
				   }
				   */
//===========================开始纹钢====================================================
				   if (Item[nId1].GetGoldId()>0)
				   {//黄金纹钢
				    if (!Player[nIndex].m_ItemList.WenGang(nId1,nId2))
						return;
				   }
				   else if (Item[nId1].IsBlue())
				   {//蓝装纹钢
                        if (!Player[nIndex].m_ItemList.BlueWenGang(nId1,nId2));
							return;
				   }
				   else if (Item[nId1].IsPurple())
				   {//紫装纹钢
					   if (!Player[nIndex].m_ItemList.PurpleWenGang(nId1,nId2));
					   return;
				   }
		   } 
            break; 
	  case 9: //提炼纹钢
		   { 
			   int nId1 = Player[nIndex].m_ItemList.SearchID(pCI->uId[0]);
			   int nId2 = Player[nIndex].m_ItemList.SearchID(pCI->uId[1]);
			   int nMoney=0;
			   g_GameSetTing.GetInteger("CompoundSet","UnSmeltMoney",0,&nMoney);    
			   if (Player[nIndex].m_ItemList.GetsMon(room_equipment) < nMoney)
			   {
				   char strMoney[32];
				   snprintf(strMoney, sizeof(strMoney),"提示:银两不足%d两",nMoney);
				   Player[nIndex].m_ItemList.msgshow(strMoney);
				   return;
			   }

			   if (nId1<=0 || nId2<=0)
			   {
				   Player[nIndex].m_ItemList.msgshow("提示:数据有误,请联系GM处理提!");
				   return;
			   }
			   Player[nIndex].Pay(nMoney);
			   int nReg = 0;
			       nReg = Player[nIndex].ExeNoBackScript("\\script\\system\\fusioncomp.lua","mainfusioncomp",1,nId1,nId2,0);

			   //if (nReg==0)
				   return;

			   //===========================开始提炼====================================================
			   if (!Player[nIndex].m_ItemList.TiLian(nId1,nId2))
						return;
		   } 
            break; 
	  case 10:  //宝石灵合
		   { 
			   int nId1 = Player[nIndex].m_ItemList.SearchID(pCI->uId[0]);
			   int nId2 = Player[nIndex].m_ItemList.SearchID(pCI->uId[1]);
			   int nMoney=0;
			   g_GameSetTing.GetInteger("CompoundSet","XingChenMoney",0,&nMoney);    
			   if (Player[nIndex].m_ItemList.GetsMon(room_equipment) < nMoney)
			   {
				   char strMoney[32];
				   snprintf(strMoney, sizeof(strMoney),"提示:银两不足%d两",nMoney);
				   Player[nIndex].m_ItemList.msgshow(strMoney);
				   return;
			   }		

			   if (nId1<=0 || nId2<=0)
			   {
				   Player[nIndex].m_ItemList.msgshow("提示:数据有误,请联系GM处理提!");
				   return;
			   }

               Player[nIndex].Pay(nMoney);

			   int nReg = 0;
			       nReg = Player[nIndex].ExeNoBackScript("\\script\\system\\fusioncomp.lua","mainfusioncomp",2,nId1,nId2,0);

			   //if (nReg==0)
			   return;

			   
			   //===========================开始灵合====================================================
			   if (!Player[nIndex].m_ItemList.LingHe(nId1,nId2))
						return;
		   } 
            break; 
	  case 11:  //融合
		   { 
			   /*int nId1 = Player[nIndex].m_ItemList.SearchID(pCI->uId[0]);
			   int nId2 = Player[nIndex].m_ItemList.SearchID(pCI->uId[1]);
			   int nId3 = Player[nIndex].m_ItemList.SearchID(pCI->uId[2]);
			   int nId4 = Player[nIndex].m_ItemList.SearchID(pCI->uId[3]);
			   int nId5 = Player[nIndex].m_ItemList.SearchID(pCI->uId[4]);
			   int nId6 = Player[nIndex].m_ItemList.SearchID(pCI->uId[5]);
			   int nId7 = Player[nIndex].m_ItemList.SearchID(pCI->uId[6]);//融合神石
			   int nId8 = Player[nIndex].m_ItemList.SearchID(pCI->uId[7]);//猩红 */
			   //int nId9 = Player[nIndex].m_ItemList.SearchID(pCI->uId[8]); 
               int nIdx[16],i;
			   char nStrInfo[128]={0};
			   for (i = 0;i < 16;++i)
			   {
				  nIdx[i] = Player[nIndex].m_ItemList.SearchID(pCI->uId[i]);
				  char nIdxKey[32]={0};

				  if  (i==15)
					  sprintf(nIdxKey,"%d",nIdx[i]);
				  else
				      sprintf(nIdxKey,"%d,",nIdx[i]);

				  strcat_s(nStrInfo,nIdxKey);
			   }

               int nEqType= pCI->mFuMoVal; //装备的类型 披风 饰品 印鉴 其他

			   int nReg = 0;
			       nReg = Player[nIndex].ExeNoBackScript("\\script\\system\\fusioncomp.lua","mainronghecomp",nEqType,0,0,0,nStrInfo);

			   if (nReg==0)
				   return;
               
               if (nIdx[0]+nIdx[1]+nIdx[2]+nIdx[3]+nIdx[4]+nIdx[5]<=0)
			   {
			       Player[nIndex].m_ItemList.msgshow("提示:没放幸运装,怎么帮你融合呢?!");
				   return;
			   }
//----------------------------
               if (nIdx[6]<=0 || nIdx[7]<=0)
			   {
				   Player[nIndex].m_ItemList.msgshow("提示:没放神石或猩红,怎么帮你融合呢?!!");
				   return;
			   }
//---------------------------开放融合的属性检测
                int nCurLuck=0,nCurNum=0,nCurLevel=0,nOpenAttrib=0;

				g_GameSetTing.GetInteger("CompoundSet","OpenAttrib",0,&nOpenAttrib);

	            for (i = 0;i < 6;++i)
				{
					if (nIdx[i])
					{
					  for (int k=0;k<6;++k)
					  {
					    if (Item[nIdx[i]].GetAttribType(k) && Item[nIdx[i]].GetAttribType(k)==nOpenAttrib) //幸运装
						 {
						   nCurLuck +=Item[nIdx[i]].GetAttribTypeVala(k);
						   nCurNum++;
						   nCurLevel +=Item[nIdx[i]].GetLevel();
						   break;	  
						 }
			          }
					}
				}
//---------------------------
			   int nMoney=0,nItemRank=0,nServerRank=0,nMinLuck=0,nMinLevel=0;

               g_GameSetTing.GetInteger2("CompoundSet","MinLuckyLevel",&nMinLuck,&nMinLevel);

               if (nCurLuck<nMinLuck)
			   { 
			           char strLuck[64];
				       snprintf(strLuck, 64, "提示:幸运装备总幸运小于%d,不能融合!",nMinLuck);
                       Player[nIndex].m_ItemList.msgshow(strLuck);
                       return;
			   }
               int nsLevel=nCurLevel/nCurNum;

			   if (nsLevel<nMinLevel)
			   {
			           char strLuck[64];
				       snprintf(strLuck, 64, "提示:装备平均等级小于%d,不能融合!",nMinLevel);
                       Player[nIndex].m_ItemList.msgshow(strLuck);
                       return;
			   }

			   g_GameSetTing.GetInteger("CompoundSet","RongHeMoney",0,&nMoney); 

			     
			   if (Player[nIndex].m_ItemList.GetsMon(room_equipment) < nMoney)
			   {
				   char strMoney[32];
				   snprintf(strMoney, sizeof(strMoney),"提示:银两不足%d两",nMoney);
				   Player[nIndex].m_ItemList.msgshow(strMoney);
				   return;
			   }
			   
			   Player[nIndex].Pay(nMoney);

//======================可选材料总价值量计算======================================
                  	int nRank = 0; //成功率？
					int nItemRows=g_CompEquip.GetHeight();
					for (i = 8;i <16;++i) //辅助物品
					{
						if (nIdx[i])
						{ 
							  for (int r=11;r<nItemRows;++r)
							  {
								  // the `RongHe' column is spelled three numbers deep as `Atlas' is, and the comparison below
								  // reads [2] - the room has to be the format's, not the caller's guess.
								  int nIteminfo[3],val;
								  g_CompEquip.GetInt3s(r,"RongHe",nIteminfo);
								  g_CompEquip.GetInteger(r,"RongHe_value",0,&val);

								  if (Item[nIdx[i]].GetGenre()==nIteminfo[0] && 
									  Item[nIdx[i]].GetDetailType()==nIteminfo[1]&&
									  Item[nIdx[i]].GetParticular()==nIteminfo[2])
								  {//如果可选材料相同
                                           nRank +=val;
										   break;
								  }
							  }		
						}
						continue;
					}
//-------------------------------------------------------------------------------------删除材料
					for (i = 0;i <16;++i)
					{//删除材料
						if (nIdx[i])
						{
							Player[nIndex].m_ItemList.Remove(nIdx[i]);
							ItemSet.Remove(nIdx[i]);
						}
					}
//--------------------------------------------------------------------------------------概率检测
					if  (nCurLuck>=0 && nCurLuck<=100)
					   g_GameSetTing.GetInteger2("LuckyRonHeSet","RonHeRank_0",&nItemRank,&nServerRank);
					else  if (nCurLuck>100 && nCurLuck<=200)
					   g_GameSetTing.GetInteger2("LuckyRonHeSet","RonHeRank_1",&nItemRank,&nServerRank);
					else  if (nCurLuck>200 && nCurLuck<=300)
					   g_GameSetTing.GetInteger2("LuckyRonHeSet","RonHeRank_2",&nItemRank,&nServerRank);
					else  if (nCurLuck>300 && nCurLuck<=400)
					   g_GameSetTing.GetInteger2("LuckyRonHeSet","RonHeRank_3",&nItemRank,&nServerRank);
					else  if (nCurLuck>400 && nCurLuck<=500)
					   g_GameSetTing.GetInteger2("LuckyRonHeSet","RonHeRank_4",&nItemRank,&nServerRank);
					else  if (nCurLuck>500 && nCurLuck<=600)
					   g_GameSetTing.GetInteger2("LuckyRonHeSet","RonHeRank_5",&nItemRank,&nServerRank);
					else  if (nCurLuck>600 && nCurLuck<=700)
					   g_GameSetTing.GetInteger2("LuckyRonHeSet","RonHeRank_6",&nItemRank,&nServerRank);
					else  if (nCurLuck>700 && nCurLuck<=800)
					   g_GameSetTing.GetInteger2("LuckyRonHeSet","RonHeRank_7",&nItemRank,&nServerRank);
					else  if (nCurLuck>800 && nCurLuck<=900)
					   g_GameSetTing.GetInteger2("LuckyRonHeSet","RonHeRank_8",&nItemRank,&nServerRank);
					else  if (nCurLuck>900 && nCurLuck<=1000)
					   g_GameSetTing.GetInteger2("LuckyRonHeSet","RonHeRank_9",&nItemRank,&nServerRank);
					else  if (nCurLuck>1000 && nCurLuck<=1100)
					   g_GameSetTing.GetInteger2("LuckyRonHeSet","RonHeRank_10",&nItemRank,&nServerRank);
					else  if (nCurLuck>1100 && nCurLuck<=1200)
					   g_GameSetTing.GetInteger2("LuckyRonHeSet","RonHeRank_11",&nItemRank,&nServerRank);
					else  if (nCurLuck>1200 && nCurLuck<=1300)
					   g_GameSetTing.GetInteger2("LuckyRonHeSet","RonHeRank_12",&nItemRank,&nServerRank);
					else  if (nCurLuck>1300 && nCurLuck<=1400)
					   g_GameSetTing.GetInteger2("LuckyRonHeSet","RonHeRank_13",&nItemRank,&nServerRank);
					else  if (nCurLuck>1400 && nCurLuck<=1500)
					   g_GameSetTing.GetInteger2("LuckyRonHeSet","RonHeRank_14",&nItemRank,&nServerRank);
					else  if (nCurLuck>1500 && nCurLuck<=1600)
					   g_GameSetTing.GetInteger2("LuckyRonHeSet","RonHeRank_15",&nItemRank,&nServerRank);
					else  if (nCurLuck>1600 && nCurLuck<=1700)
					   g_GameSetTing.GetInteger2("LuckyRonHeSet","RonHeRank_16",&nItemRank,&nServerRank);
					else  if (nCurLuck>1700 && nCurLuck<=1800)
					   g_GameSetTing.GetInteger2("LuckyRonHeSet","RonHeRank_17",&nItemRank,&nServerRank);
					else  if (nCurLuck>1800 && nCurLuck<=1900)
					   g_GameSetTing.GetInteger2("LuckyRonHeSet","RonHeRank_18",&nItemRank,&nServerRank);
					else  if (nCurLuck>1900 && nCurLuck<=2000)
					   g_GameSetTing.GetInteger2("LuckyRonHeSet","RonHeRank_19",&nItemRank,&nServerRank);
					else if (nCurLuck>2000)
					   g_GameSetTing.GetInteger2("LuckyRonHeSet","RonHeRank_20",&nItemRank,&nServerRank);
//--------------------------------------------------------------------------------------								
				  if (g_Random(nItemRank+nRank) <= g_Random(nServerRank))
			      {
                        Player[nIndex].m_ItemList.msgshow("提示:融合失败!");
				        return;
			      }  			        
//===========================开始融合====================================================
			      if (!Player[nIndex].m_ItemList.BlueRonHe(nIdx[6],nIdx[7],nEqType,nRank,nCurLuck,nsLevel))
				     return;
		   } 
            break; 
	  case 12:  //辨识
		   {
			   int nId1 = Player[nIndex].m_ItemList.SearchID(pCI->uId[0]);
			   int nId2 = Player[nIndex].m_ItemList.SearchID(pCI->uId[1]);

               if (nId1<=0 || nId2<=0)
			   {
				   Player[nIndex].m_ItemList.msgshow("提示:数据有误,请联系GM处理提!");
				   return;
			   }

			   int nMoney=0;
			   g_GameSetTing.GetInteger("CompoundSet","BianShiMoney",0,&nMoney);    
			   if (Player[nIndex].m_ItemList.GetsMon(room_equipment) < nMoney)
			   {
				   char strMoney[32];
				   snprintf(strMoney, sizeof(strMoney),"提示:银两不足%d两",nMoney);
				   Player[nIndex].m_ItemList.msgshow(strMoney);
				   return;
			   }	
			   
			   Player[nIndex].Pay(nMoney);
			   int nReg = 0;
			       nReg = Player[nIndex].ExeNoBackScript("\\script\\system\\fusioncomp.lua","mainbianshicomp",0,nId1,nId2,0);

			   if (nReg==0)
				   return;
			   //===========================开始辨识====================================================
			   if (!Player[nIndex].m_ItemList.BlueBianShi(nId1,nId2))
				   return;
		   }  
         break;
	  case 13:  //激活神将
		  {
			  /*int nId1 = Player[nIndex].m_ItemList.SearchID(pCI->uId[0]);
			  int nMoney=0;
			  g_GameSetTing.GetInteger("CompoundSet","FumoMoney",0,&nMoney);    
			  if (Player[nIndex].m_ItemList.GetsMon(room_equipment) < nMoney)
			  {
				  char strMoney[32];
				  sprintf(strMoney,"提示:银两不足:%d两",nMoney);
				  Player[nIndex].m_ItemList.msgshow(strMoney);
				  return;
			  }		
			  Player[nIndex].Pay(nMoney);

			  if (nId1<=0)
			  {
				  Player[nIndex].m_ItemList.msgshow("提示:数据有误,请联系GM处理提!");
				  return;
			  }
              //概率设置
               int nRand,nRandVal;
			       g_GameSetTing.GetInteger2("FuMoNpcSet","FuMoRank",&nRand,&nRandVal); 

			   if (g_Random(nRand) < g_Random(nRandVal))
			   {
                   Player[nIndex].m_ItemList.msgshow("提示:神将激活失败!!");
                   Player[nIndex].m_ItemList.Remove(nId1);  //移除装备
				   ItemSet.Remove(nId1);
				   return;
			   }

			 int mNpcSeting=0;
             if (pCI->mFuMoVal==0) 
			 {//金系附魔
                   Npc[Player[nIndex].m_nIndex].m_nFuMoNum[0].nNpcSetings=1;
				   Player[nIndex].m_ItemList.msgshow("提示:武神激活成功!,恭喜!恭喜!!");
			 }
			 else if (pCI->mFuMoVal==1)
			 {
                   Npc[Player[nIndex].m_nIndex].m_nFuMoNum[1].nNpcSetings=1;
				   Player[nIndex].m_ItemList.msgshow("提示:木神激活成功!,恭喜!恭喜!!");
			 }
			 else if (pCI->mFuMoVal==2)
			 {
				 Npc[Player[nIndex].m_nIndex].m_nFuMoNum[2].nNpcSetings=1;
				   Player[nIndex].m_ItemList.msgshow("提示:水神激活成功!,恭喜!恭喜!!");
			 }
			 else if (pCI->mFuMoVal==3)
			 {
				 Npc[Player[nIndex].m_nIndex].m_nFuMoNum[3].nNpcSetings=1;
				   Player[nIndex].m_ItemList.msgshow("提示:火神激活成功!,恭喜!恭喜!!");
			 }
			 else if (pCI->mFuMoVal==4)
			 {
				 Npc[Player[nIndex].m_nIndex].m_nFuMoNum[4].nNpcSetings=1;
				 Player[nIndex].m_ItemList.msgshow("提示:土神激活成功!,恭喜!恭喜!!");
			 }
             Player[nIndex].m_ItemList.Remove(nId1);  //移除装备
			 ItemSet.Remove(nId1);*/
		  }  
         break;
	  case 14:  //神将切换（激活成功后的设置）
		  {
			  //int nId1 = Player[nIndex].m_ItemList.SearchID(pCI->uId[0]);
			  /*int mNpcSeting=0;
			  if (pCI->mFuMoVal==0) 
			  {//金系附魔
				  g_GameSetTing.GetInteger("FuMoNpcSet","Mo_0",0,&mNpcSeting); 

				  Npc[Player[nIndex].m_nIndex].m_nFuMoNum[0].nNpcSetings=mNpcSeting;
                  //Npc[Player[nIndex].m_nIndex].m_FuMoTimesVal=500;
                  if (Npc[Player[nIndex].m_nIndex].m_nFuMoNum[1].nNpcSetings>1)
                      Npc[Player[nIndex].m_nIndex].m_nFuMoNum[1].nNpcSetings=1;
				  else if (Npc[Player[nIndex].m_nIndex].m_nFuMoNum[2].nNpcSetings>1)
					  Npc[Player[nIndex].m_nIndex].m_nFuMoNum[2].nNpcSetings=1;
                  else if (Npc[Player[nIndex].m_nIndex].m_nFuMoNum[3].nNpcSetings>1)
					  Npc[Player[nIndex].m_nIndex].m_nFuMoNum[3].nNpcSetings=1;
				  else if (Npc[Player[nIndex].m_nIndex].m_nFuMoNum[4].nNpcSetings>1)
					  Npc[Player[nIndex].m_nIndex].m_nFuMoNum[4].nNpcSetings=1;
				  Player[nIndex].UpdataCurData();
				  Player[nIndex].m_ItemList.msgshow("提示:切换武神成功!");
			  }
			  else if (pCI->mFuMoVal==1)
			  {
				  g_GameSetTing.GetInteger("FuMoNpcSet","Mo_1",0,&mNpcSeting); 
				  Npc[Player[nIndex].m_nIndex].m_nFuMoNum[1].nNpcSetings=mNpcSeting;
                  //Npc[Player[nIndex].m_nIndex].m_FuMoTimesVal=500;
				  if (Npc[Player[nIndex].m_nIndex].m_nFuMoNum[0].nNpcSetings>1)
                      Npc[Player[nIndex].m_nIndex].m_nFuMoNum[0].nNpcSetings=1;
				  else if (Npc[Player[nIndex].m_nIndex].m_nFuMoNum[2].nNpcSetings>1)
					  Npc[Player[nIndex].m_nIndex].m_nFuMoNum[2].nNpcSetings=1;
                  else if (Npc[Player[nIndex].m_nIndex].m_nFuMoNum[3].nNpcSetings>1)
					  Npc[Player[nIndex].m_nIndex].m_nFuMoNum[3].nNpcSetings=1;
				  else if (Npc[Player[nIndex].m_nIndex].m_nFuMoNum[4].nNpcSetings>1)
					  Npc[Player[nIndex].m_nIndex].m_nFuMoNum[4].nNpcSetings=1;
				  Player[nIndex].UpdataCurData();
				  Player[nIndex].m_ItemList.msgshow("提示:切换木神成功!");
			  }
			  else if (pCI->mFuMoVal==2)
			  {
				  g_GameSetTing.GetInteger("FuMoNpcSet","Mo_2",0,&mNpcSeting); 
				  Npc[Player[nIndex].m_nIndex].m_nFuMoNum[2].nNpcSetings=mNpcSeting;
                  //Npc[Player[nIndex].m_nIndex].m_FuMoTimesVal=500;
				  if (Npc[Player[nIndex].m_nIndex].m_nFuMoNum[0].nNpcSetings>1)
                      Npc[Player[nIndex].m_nIndex].m_nFuMoNum[0].nNpcSetings=1;
				  else if (Npc[Player[nIndex].m_nIndex].m_nFuMoNum[1].nNpcSetings>1)
					  Npc[Player[nIndex].m_nIndex].m_nFuMoNum[1].nNpcSetings=1;
                  else if (Npc[Player[nIndex].m_nIndex].m_nFuMoNum[3].nNpcSetings>1)
					  Npc[Player[nIndex].m_nIndex].m_nFuMoNum[3].nNpcSetings=1;
				  else if (Npc[Player[nIndex].m_nIndex].m_nFuMoNum[4].nNpcSetings>1)
					  Npc[Player[nIndex].m_nIndex].m_nFuMoNum[4].nNpcSetings=1;
				  Player[nIndex].UpdataCurData();
				  Player[nIndex].m_ItemList.msgshow("提示:切换水神成功!");
			  }
			  else if (pCI->mFuMoVal==3)
			  {
				  g_GameSetTing.GetInteger("FuMoNpcSet","Mo_3",0,&mNpcSeting); 
				  Npc[Player[nIndex].m_nIndex].m_nFuMoNum[3].nNpcSetings=mNpcSeting;
                  //Npc[Player[nIndex].m_nIndex].m_FuMoTimesVal=500;
				  if (Npc[Player[nIndex].m_nIndex].m_nFuMoNum[0].nNpcSetings>1)
                      Npc[Player[nIndex].m_nIndex].m_nFuMoNum[0].nNpcSetings=1;
				  else if (Npc[Player[nIndex].m_nIndex].m_nFuMoNum[1].nNpcSetings>1)
					  Npc[Player[nIndex].m_nIndex].m_nFuMoNum[1].nNpcSetings=1;
                  else if (Npc[Player[nIndex].m_nIndex].m_nFuMoNum[2].nNpcSetings>1)
					  Npc[Player[nIndex].m_nIndex].m_nFuMoNum[2].nNpcSetings=1;
				  else if (Npc[Player[nIndex].m_nIndex].m_nFuMoNum[4].nNpcSetings>1)
					  Npc[Player[nIndex].m_nIndex].m_nFuMoNum[4].nNpcSetings=1;
				  Player[nIndex].UpdataCurData();
				  Player[nIndex].m_ItemList.msgshow("提示:切换火神成功!");
			  }
			  else if (pCI->mFuMoVal==4)
			  {
				  g_GameSetTing.GetInteger("FuMoNpcSet","Mo_4",0,&mNpcSeting); 
				  Npc[Player[nIndex].m_nIndex].m_nFuMoNum[4].nNpcSetings=mNpcSeting;
                  //Npc[Player[nIndex].m_nIndex].m_FuMoTimesVal=500;
				  if (Npc[Player[nIndex].m_nIndex].m_nFuMoNum[0].nNpcSetings>1)
                      Npc[Player[nIndex].m_nIndex].m_nFuMoNum[0].nNpcSetings=1;
				  else if (Npc[Player[nIndex].m_nIndex].m_nFuMoNum[1].nNpcSetings>1)
					  Npc[Player[nIndex].m_nIndex].m_nFuMoNum[1].nNpcSetings=1;
                  else if (Npc[Player[nIndex].m_nIndex].m_nFuMoNum[2].nNpcSetings>1)
					  Npc[Player[nIndex].m_nIndex].m_nFuMoNum[2].nNpcSetings=1;
				  else if (Npc[Player[nIndex].m_nIndex].m_nFuMoNum[3].nNpcSetings>1)
					  Npc[Player[nIndex].m_nIndex].m_nFuMoNum[3].nNpcSetings=1;
				  Player[nIndex].UpdataCurData();
				  Player[nIndex].m_ItemList.msgshow("提示:切换土神成功!");
			  }
			 */
		  }  
         break;
	  case 15:  //升级技能
		  {
			 /*int nId1 = Player[nIndex].m_ItemList.SearchID(pCI->uId[0]);
			 int nMoney=0;
			  g_GameSetTing.GetInteger("CompoundSet","FumoSkillMoney",0,&nMoney);    
			  if (Player[nIndex].m_ItemList.GetsMon(room_equipment) < nMoney)
			  {
				  char strMoney[32];
				  sprintf(strMoney,"提示:银两不足:%d两",nMoney);
				  Player[nIndex].m_ItemList.msgshow(strMoney);
				  return;
			  }		
			  Player[nIndex].Pay(nMoney);

			  if (nId1<=0)
			  {
				  Player[nIndex].m_ItemList.msgshow("提示:数据有误,请联系GM处理提!");
				  return;
			  }
			  //===========================开始升级技能====================================================
			  //概率设置
			  int nRand,nRandVal;
			       g_GameSetTing.GetInteger2("FuMoNpcSet","FuMoSkillRank",&nRand,&nRandVal); 

			  if (g_Random(nRand)<g_Random(nRandVal))
			  {
					   Player[nIndex].m_ItemList.msgshow("提示:神将能力提高失败!!");
					   Player[nIndex].m_ItemList.Remove(nId1);  //移除装备
					   ItemSet.Remove(nId1);
					   return;
			  }

			  if (pCI->mFuMoVal==0) 
			  {//金系神将
                 Npc[Player[nIndex].m_nIndex].m_nFuMoNum[0].nSkillLevel +=1;
				 if  (Npc[Player[nIndex].m_nIndex].m_nFuMoNum[0].nSkillLevel>=MAX_SKILLLEVEL)
				 {
					 Npc[Player[nIndex].m_nIndex].m_nFuMoNum[0].nSkillLevel=63;
					 Player[nIndex].m_ItemList.msgshow("提示:神将能力已经达到最强!");
				 }
				 else
				     Player[nIndex].m_ItemList.msgshow("提示:金系神将能力提高成功!");
			  }
			  else if (pCI->mFuMoVal==1) 
			  {//木系神将
				  Npc[Player[nIndex].m_nIndex].m_nFuMoNum[1].nSkillLevel +=1;
				  if  (Npc[Player[nIndex].m_nIndex].m_nFuMoNum[1].nSkillLevel>=MAX_SKILLLEVEL)
				  {
					  Npc[Player[nIndex].m_nIndex].m_nFuMoNum[1].nSkillLevel=63;
					  Player[nIndex].m_ItemList.msgshow("提示:神将能力已经达到最强!");
				  }
				  else
				      Player[nIndex].m_ItemList.msgshow("提示:木系神将能力提高成功!");
			  }
			  else if (pCI->mFuMoVal==2) 
			  {//水系神将
				  Npc[Player[nIndex].m_nIndex].m_nFuMoNum[2].nSkillLevel +=1;
				  if  (Npc[Player[nIndex].m_nIndex].m_nFuMoNum[2].nSkillLevel>=MAX_SKILLLEVEL)
				  {
					  Npc[Player[nIndex].m_nIndex].m_nFuMoNum[2].nSkillLevel=63;
					  Player[nIndex].m_ItemList.msgshow("提示:神将能力已经达到最强!");
				  }
				  else     
				     Player[nIndex].m_ItemList.msgshow("提示:水系神将能力提高成功!");
			  }
			  else if (pCI->mFuMoVal==3) 
			  {//火系神将
				  Npc[Player[nIndex].m_nIndex].m_nFuMoNum[3].nSkillLevel +=1;

				  if  (Npc[Player[nIndex].m_nIndex].m_nFuMoNum[3].nSkillLevel>=MAX_SKILLLEVEL)
				  {
					  Npc[Player[nIndex].m_nIndex].m_nFuMoNum[3].nSkillLevel=63;
					  Player[nIndex].m_ItemList.msgshow("提示:神将能力已经达到最强!");
				  }
				  else
				    Player[nIndex].m_ItemList.msgshow("提示:火系神将能力提高成功!");
			  }
			  else if (pCI->mFuMoVal==4) 
			  {//土系神将
				  Npc[Player[nIndex].m_nIndex].m_nFuMoNum[4].nSkillLevel +=1;
				   if  (Npc[Player[nIndex].m_nIndex].m_nFuMoNum[4].nSkillLevel>=MAX_SKILLLEVEL)
				   {
					   Npc[Player[nIndex].m_nIndex].m_nFuMoNum[4].nSkillLevel=63;
					   Player[nIndex].m_ItemList.msgshow("提示:神将能力已经达到最强!");
				   }
				  else
				       Player[nIndex].m_ItemList.msgshow("提示:土系神将能力提高成功!");
			  }
			   Player[nIndex].m_ItemList.Remove(nId1);  //移除装备
			   ItemSet.Remove(nId1);
			 */
			  //-------------------------------------------------------------------------------------------
		  }  
         break;
	  case 16:  //进阶称号
		  { 
			  int nId1 = Player[nIndex].m_ItemList.SearchID(pCI->uId[0]);
			  int nMoney=0;
			  g_GameSetTing.GetInteger("CompoundSet","JinJIeMoney",0,&nMoney);    
			  if (Player[nIndex].m_ItemList.GetsMon(room_equipment) < nMoney)
			  {
				  char strMoney[32];
				  snprintf(strMoney, sizeof(strMoney),"提示:银两不足%d两",nMoney);
				  Player[nIndex].m_ItemList.msgshow(strMoney);
				  return;
			  }

			  if (nId1<=0)
			  {
				  Player[nIndex].m_ItemList.msgshow("提示:数据有误C,请联系GM处理!");
				  return;
			  }

			  Player[nIndex].Pay(nMoney);

			  int nReg = 0;
				  nReg = Player[nIndex].ExeNoBackScript("\\script\\system\\titleset.lua","maintitle",nId1,0,0,0);
              
			  if (nReg==0)
				  return;

			  
			  
			  //===========================开始进阶技能====================================================
			  //概率设置
			  int nRand=0,nRandVal=5,nItemLevel=1,nRankId=0,nCurSkillId,nRows;

			     nItemLevel = Item[nId1].GetLevel();

			     nRankId	= Npc[Player[nIndex].m_nIndex].m_btRankFFId;

				 KTabFile nRankFile;

				 if (!nRankFile.Load("\\Settings\\HonorSetting.txt"))
					 break;
				 
                 nRankFile.GetInteger(nRankId+1,"SKILLID",0,&nCurSkillId);
					  		  
                 nRows = nRankFile.GetHeight()-1;

				 if (nRankId>=nRows)
				 {
                    Player[nIndex].m_ItemList.msgshow("提示:称号达到最顶级,不能再进阶!");
					nRankFile.Clear();
					return;
				 }
                 
			  g_GameSetTing.GetInteger2("CompoundSet","JinJieRank",&nRand,&nRandVal); 
			  
			  if (g_Random(nRand) < g_Random(nRandVal))
			  {
                  char msg[64];
				  sprintf(msg,"提示:称号(%d)进阶失败!!",nItemLevel);
				  Player[nIndex].m_ItemList.msgshow(msg);
				  Player[nIndex].m_ItemList.Remove(nId1);  //移除装备
				  ItemSet.Remove(nId1);
				  nRankFile.Clear();
				  return;
			  }	
			      Player[nIndex].m_ItemList.Remove(nId1);  //移除装备
				  ItemSet.Remove(nId1);
			  //-------------------------------------------------------------------------------------------
                  if (nRankId && nCurSkillId > 0)
				  {    
					   int nSkiiIdx = 0 ;
                       nSkiiIdx = Npc[Player[nIndex].m_nIndex].m_SkillList.FindSame(nCurSkillId);
                       KSkill * pSkill =(KSkill *)g_SkillManager.GetSkill(Npc[Player[nIndex].m_nIndex].m_SkillList.m_Skills[nSkiiIdx].SkillId, Npc[Player[nIndex].m_nIndex].m_SkillList.m_Skills[nSkiiIdx].SkillLevel);
                       if (pSkill)
                            pSkill->Cast(Player[nIndex].m_nIndex, -1, Player[nIndex].m_nIndex,1);  //移除状态

					   int nRet=0;
                           nRet =  Npc[Player[nIndex].m_nIndex].m_SkillList.Remove(nCurSkillId);

					   if (nRet)
					   { 
						   Player[nIndex].SendSyncData_Skill();
					   } 

				  } 
				     int nSkillLevel=1;

			         Npc[Player[nIndex].m_nIndex].m_btRankFFId = nRankId+1;

                     nRankFile.GetInteger(nRankId+2,"SKILLID",0,&nCurSkillId); 
				     
					 nRankFile.GetInteger(nRankId+2,"nLevel",1,&nSkillLevel);

				   int nRet = Npc[Player[nIndex].m_nIndex].m_SkillList.Add(nCurSkillId,nSkillLevel);

				   if (nRet)
				   {  
					   int nListIdx = Npc[Player[nIndex].m_nIndex].m_SkillList.FindSame(nCurSkillId);
					   PLAYER_SKILL_LEVEL_SYNC NewSkill;
					   NewSkill.ProtocolType  = s2c_playerskilllevel;
					   NewSkill.m_nSkillID    = nCurSkillId;
					   NewSkill.m_nSkillLevel = Npc[Player[nIndex].m_nIndex].m_SkillList.GetCurrentLevelByIdx(nListIdx);
					   NewSkill.m_nSkillExp = Npc[Player[nIndex].m_nIndex].m_SkillList.GetCurSkillExp(nListIdx);//muc do luyen
					   NewSkill.m_nLeavePoint = Player[nIndex].m_nSkillPoint;
					   NewSkill.m_nAddPoint   = Npc[Player[nIndex].m_nIndex].m_SkillList.GetAddPoint(nListIdx);
					   g_pServer->PackDataToClient(Player[nIndex].m_nNetConnectIdx, (BYTE*)&NewSkill, sizeof(PLAYER_SKILL_LEVEL_SYNC));
				       Player[nIndex].SendSyncData_Skill();
				   } 
				   nRankFile.Clear();
			    Player[nIndex].m_ItemList.msgshow("提示:称号进阶成功!恭喜!恭喜!");
		  }  
         break;
		}
	}
	else
	{//人数超出限制提示

	}

	return;
}

//经脉执行脚本
void KProtocolProcess::c2sfsjingmai(int nIndex, BYTE* pProtocol)
{
	CP_JINGMAI *pJingMai=(CP_JINGMAI*)pProtocol;
    int mtype=pJingMai->ntype;     //经脉大类
	int mlevel=pJingMai->nlevel;   //第几个穴道
	int mzhenyuan,mhumaidan,mrate,mbingjia;
	char mmName[16];
    if (mtype==1)      //督脉
	{
	    g_FsJinMai.GetInteger(mlevel+1,10,10,&mzhenyuan); //真元
        g_FsJinMai.GetInteger(mlevel+1,11,10,&mhumaidan);
		g_FsJinMai.GetString(mlevel+1,1,"无效",mmName,sizeof(mmName)); //穴道名称
	    g_FsJinMai.GetInteger(mlevel+1,10,0,&mbingjia);	
	}
	else if (mtype==2) //
	{
		g_FsJinMai.GetInteger(mlevel+17,10,10,&mzhenyuan); //真元
        g_FsJinMai.GetInteger(mlevel+17,11,10,&mhumaidan);
		g_FsJinMai.GetString(mlevel+17,1,"无效",mmName,sizeof(mmName)); //穴道名称
	    g_FsJinMai.GetInteger(mlevel+17,10,0,&mbingjia);
	}
	else if (mtype==3) //
	{
		g_FsJinMai.GetInteger(mlevel+33,10,10,&mzhenyuan); //真元
        g_FsJinMai.GetInteger(mlevel+33,11,10,&mhumaidan);
		g_FsJinMai.GetString(mlevel+33,1,"无效",mmName,sizeof(mmName)); //穴道名称
	    g_FsJinMai.GetInteger(mlevel+33,10,0,&mbingjia);
	}
	else if (mtype==4) //
	{
		g_FsJinMai.GetInteger(mlevel+49,10,10,&mzhenyuan); //真元
        g_FsJinMai.GetInteger(mlevel+49,11,10,&mhumaidan);
		g_FsJinMai.GetString(mlevel+49,1,"无效",mmName,sizeof(mmName)); //穴道名称
	    g_FsJinMai.GetInteger(mlevel+49,10,0,&mbingjia);
	}
	else if (mtype==5) //
	{
		g_FsJinMai.GetInteger(mlevel+65,10,10,&mzhenyuan); //真元
        g_FsJinMai.GetInteger(mlevel+65,11,10,&mhumaidan); 
		g_FsJinMai.GetString(mlevel+65,1,"无效",mmName,sizeof(mmName)); //穴道名称
	    g_FsJinMai.GetInteger(mlevel+65,10,0,&mbingjia);
	}
	else if (mtype==6) //
	{
		g_FsJinMai.GetInteger(mlevel+81,10,10,&mzhenyuan); //真元
        g_FsJinMai.GetInteger(mlevel+81,11,10,&mhumaidan); 
		g_FsJinMai.GetString(mlevel+81,1,"无效",mmName,sizeof(mmName)); //穴道名称
	    g_FsJinMai.GetInteger(mlevel+81,10,0,&mbingjia);
	}
	else if (mtype==7) //
	{
		g_FsJinMai.GetInteger(mlevel+97,10,10,&mzhenyuan); //真元
        g_FsJinMai.GetInteger(mlevel+97,11,10,&mhumaidan); 
		g_FsJinMai.GetString(mlevel+97,1,"无效",mmName,sizeof(mmName)); //穴道名称
	    g_FsJinMai.GetInteger(mlevel+97,10,0,&mbingjia);
	}
	else if (mtype==8) //
	{
		g_FsJinMai.GetInteger(mlevel+113,10,10,&mzhenyuan); //真元
        g_FsJinMai.GetInteger(mlevel+113,11,10,&mhumaidan); 
		g_FsJinMai.GetString(mlevel+113,1,"无效",mmName,sizeof(mmName)); //穴道名称
	    g_FsJinMai.GetInteger(mlevel+113,10,0,&mbingjia);
	}

    char xuedao[4];
	_itoa(mlevel,xuedao,10);//int转char
    
	if (nIndex > 0 && nIndex < MAX_PLAYER)
	{//("\\script\\system\\player\\offline.lua","main","","");
		Player[nIndex].ExecuteScriptC("\\script\\system\\fsjinmai.lua","updatajinmai",xuedao,mmName,mzhenyuan,mhumaidan,mtype,mbingjia);
	}
}
//服务器端设置价格//Dinh gia ban item
void KProtocolProcess::SetPrice(int nIndex, BYTE* pProtocol)
{
	PLAYER_SET_PRICE *pSP=(PLAYER_SET_PRICE *)pProtocol;
	if (nIndex > 0 && nIndex < MAX_PLAYER)
	{ 

		int nIdx = Player[nIndex].m_ItemList.SearchID(pSP->m_ID);
		if  (nIdx<=0)
		{
			Player[nIndex].m_ItemList.msgshow("Item ban khong hop le!");
			return;
		}

		if (Item[nIdx].GetLockItem()->IsLockItem() || Item[nIdx].GetUseMapIdx()>0)
		{  
			Player[nIndex].m_ItemList.msgshow("Item Khoa khong the ban!");
			Item[nIdx].SetTradePrice(0);
			Player[nIndex].m_ItemList.SetModel(pSP->m_ID,0);	
			Player[nIndex].m_ItemList.SynPlayerItem(nIdx);
			return;
		}
		 
		if (Item[nIdx].GetLockItem()->IsLockItem() || Item[nIdx].GetUseMapIdx()>0)
		{  
			Player[nIndex].m_ItemList.msgshow("Item Khoa khong the ban!");
			Item[nIdx].SetTradePrice(0);
			Player[nIndex].m_ItemList.SetModel(pSP->m_ID,0);	
			Player[nIndex].m_ItemList.SynPlayerItem(nIdx);
			return;
		}

		//Player[nIndex].m_ItemList.SetPrice(pSP->m_ID,pSP->m_Price);
		Item[nIdx].SetTradePrice(pSP->m_Price);
		Player[nIndex].m_ItemList.SetModel(pSP->m_ID,pSP->m_Model);	
		Player[nIndex].m_ItemList.SynPlayerItem(nIdx);
		//printf("GetTradePrice: %d,\n", sItem.m_TradePrice);
	}
}

void KProtocolProcess::c2splayersetLianFlg(int nIndex, BYTE* pProtocol)
{
	PLAYER_SET_LIANJIEFLG *pSP=(PLAYER_SET_LIANJIEFLG *)pProtocol;
	if (nIndex > 0 && nIndex < MAX_PLAYER)
	{ 
		Player[nIndex].m_ItemList.SetLianFlg(pSP->m_ID,pSP->m_LianFlg);
	}
}
//set trang thai muaban
void KProtocolProcess::StartTrade(int nIndex, BYTE* pProtocol)
{
	if (Player[nIndex].CheckTrading())
		return;

	if (nIndex > 0 && nIndex < MAX_PLAYER)
	{
		Player[nIndex].TradeStart(pProtocol);
	}
}

//装备链接  服务器端
void KProtocolProcess::c2slianjie(int nIndex, BYTE* pProtocol)
{
	VIEW_LIANJIE_COMMAND	*pView = (VIEW_LIANJIE_COMMAND*)pProtocol;
//	if (pView->m_dwNpcID == Npc[Player[nIndex].m_nIndex].m_dwID)  //如果是同一个人则返回
//		return;
    int  m_dwNpcID= NpcSet.SearchName(pView->m_PLname,kind_player); // (有效)
    int nPlayerIdx =PlayerSet.FindPlayerID(m_dwNpcID);
	if (nPlayerIdx <= 0)
	{
	   //开始 多gs 查询
	  // Player[nIndex].m_ItemList.msgshow("链接:数据有误,请联系GM处理!");
      if (g_pGateWayClient)
	  {
		  tagItemAction gm;
		  memset(&gm,0,sizeof(gm));
		  gm.cProtocol = c2s_item_synaction;
		  gm.cCmdType  = enum_ITEM_ID_FIND;
		  ZeroMemory(gm.nRoleName,sizeof(gm.nRoleName));
		  g_StrCpyLen(gm.nRoleName, pView->m_PLname, sizeof(gm.nRoleName));
		  g_StrCpyLen(gm.nGmRoleName, Npc[Player[nIndex].m_nIndex].Name, sizeof(gm.nGmRoleName));
		  gm.nId = pView->m_ItemDwid;
		  gm.Key=nCurSerIdx;
		  gm.verX=nCurGsNumber;
		  gm.verY=0;
		  if (g_pGateWayClient)
			  g_pGateWayClient->SendPackToServer(&gm, sizeof(tagItemAction));
	  }

	//////////////////////////////////////////////////
	   return;
	}
	//被查看者执行函数
 	if (!Player[nPlayerIdx].SendLianItemInfo(nIndex ,pView->m_ItemDwid,pView->m_bLPrcess,0,pView->m_PLname))  //查看者索引 nIndex
	{   
		Player[nIndex].m_ItemList.msgshow("Trang bi nay chu so huu khong co!");
	    return;
	}
}

#define		defMAX_VIEW_EQUIP_TIME 30

void	KProtocolProcess::c2sViewEquip(int nIndex, BYTE* pProtocol)
{
	if (g_SubWorldSet.GetGameTime() - Player[nIndex].m_nViewEquipTime < defMAX_VIEW_EQUIP_TIME)
	{
		return;
	}

	Player[nIndex].m_nViewEquipTime = g_SubWorldSet.GetGameTime();
	
	VIEW_EQUIP_COMMAND	*pView = (VIEW_EQUIP_COMMAND*)pProtocol;

	if (pView->m_dwNpcID == Npc[Player[nIndex].m_nIndex].m_dwID)
		return;

	int nPlayerIdx = Player[nIndex].FindAroundPlayer(pView->m_dwNpcID);

	if (nPlayerIdx <= 0)
		return;

	Player[nPlayerIdx].SendEquipItemInfo(nIndex,pView->m_bPrcess);  //nPlayerIdx 被查看装备的人   nIndex 要查看装备的
	//printf("c2sViewEquip: ->SendSellItemInfo %d - %d\n",nIndex,pView->m_bPrcess);
}
//playershop
void KProtocolProcess::c2sViewItem(int nIndex, BYTE* pProtocol)
{
	VIEW_EQUIP_COMMAND	*pView = (VIEW_EQUIP_COMMAND*)pProtocol;
	if (pView->m_dwNpcID == Npc[Player[nIndex].m_nIndex].m_dwID)
		return;
	int nPlayerIdx = Player[nIndex].FindAroundPlayer(pView->m_dwNpcID);
	if (nPlayerIdx <= 0)
		return;
	//printf("c2sViewItem: ->SendSellItemInfo nIndex: %d - m_bPrcess: %d\n",nIndex,pView->m_bPrcess);
	Player[nPlayerIdx].SendSellItemInfo(nIndex ,pView->m_bPrcess);
}

void KProtocolProcess::c2supdateitem(int nIndex, BYTE* pProtocol)
{
	VIEW_EQUIP_COMMAND	*pView = (VIEW_EQUIP_COMMAND*)pProtocol;
	if (pView->m_dwNpcID == Npc[Player[nIndex].m_nIndex].m_dwID)
		return;
	int nPlayerIdx = Player[nIndex].FindAroundPlayer(pView->m_dwNpcID);
	if (nPlayerIdx <= 0)
		return;
	Player[nPlayerIdx].SendSellItemInfo(nIndex ,pView->m_bPrcess,TRUE);
	//printf("c2supdateitem: ->SendSellItemInfo %d - %d\n",nIndex,pView->m_bPrcess);
}

void KProtocolProcess::c2svipdoScript(int nIndex, BYTE* pProtocol)
{
        PLAYER_VIP_DOSCRIPT	*pVipDo = (PLAYER_VIP_DOSCRIPT*)pProtocol;
		if (pVipDo->cnKind<=16)	//装备打造
           Player[nIndex].ExecuteScriptC("\\script\\system\\vipdazao.lua","VipIiem",Player[nIndex].m_AccoutName,"",pVipDo->cnKind,pVipDo->cnCurJiHuo,pVipDo->cnCurLucky,pVipDo->cnCurExp,pVipDo->cnCurWuHang);
		else//VIP充值
		   Player[nIndex].ExecuteScriptC("\\script\\system\\vipdazao.lua","VipIiemCard",pVipDo->snCardAcc,pVipDo->snCardPass,pVipDo->cnKind,pVipDo->cnCurJiHuo,pVipDo->cnCurLucky,pVipDo->cnCurExp,pVipDo->cnCurWuHang);

}
//tim duong
//客户端设置状态
void   KProtocolProcess::c2sfindpathstate(int nIndex, BYTE* pProtocol)
{
	if (!pProtocol) return;
	int  nPlayerIndex = nIndex;//Npc[m_nIndex].GetPlayerIdx();
	if  (nPlayerIndex<=0 || nPlayerIndex>=MAX_PLAYER)
	{
		printf("c2sfindpathstate:player is full ...\n");
		return;
	}

	PLAYER_FINDPATH_STATE	*sFindState= (PLAYER_FINDPATH_STATE*)pProtocol;
	 Player[nPlayerIndex].setFindPathState(sFindState->m_btstate);

	 printf("---------------c2sfindpathstate: player: %s [%d] ------------------\n",Player[nIndex].m_AccoutName,sFindState->m_btstate);
}

void KProtocolProcess::c2screaterefitem(int nIndex, BYTE* pProtocol)
{
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;
	Player[nIndex].CreateRefItem(pProtocol);
}

void KProtocolProcess::c2sremoverefitem(int nIndex, BYTE* pProtocol)
{
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	Player[nIndex].RemoveRefItem(pProtocol);
}

void KProtocolProcess::c2sapplymasklock(int nIndex, BYTE* pProtocol)
{
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;
	PLAYERREQUESTLOCKMASK* pLockMaskCmd = (PLAYERREQUESTLOCKMASK*)pProtocol;
	Player[nIndex].m_ItemList.SetMaskLock(pLockMaskCmd->m_MaskLock);

	if (pLockMaskCmd->m_MaskLock)
	{
		Npc[Player[nIndex].m_nIndex].m_MaskType = 0;
	}
	else
	{
		int nIdx = Player[nIndex].m_ItemList.GetEquipment(itempart_mask);
		Npc[Player[nIndex].m_nIndex].m_MaskType = Item[nIdx].GetBaseMagic();
	}
}

void   KProtocolProcess::c2slockitem(int nIndex, BYTE* pProtocol)
{
	 if (!pProtocol) return;
	 int  nPlayerIndex = nIndex;//Npc[m_nIndex].GetPlayerIdx();
	 if  (nPlayerIndex<=0 || nPlayerIndex>=MAX_PLAYER)
	 {
		 printf("c2slockitem:player is full ...\n");
		 return;
	 }

	 PLAYER_AUTOMUVE_ITEM_COMMAND	*pItME = (PLAYER_AUTOMUVE_ITEM_COMMAND*)pProtocol;
	 Player[nPlayerIndex].m_ItemList.setLock(pItME->m_nItemID,pItME->m_btPlace);
}

void   KProtocolProcess::GMCommand(int nIndex, BYTE* pProtocol)
{
	 GM_COMMAND	*pCmd = (GM_COMMAND*)pProtocol;
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;

	char	m_szString[256];
	memset(m_szString, 0, sizeof(m_szString));
	strncpy(m_szString, pCmd->szString, sizeof(m_szString) - 1);

    // Tach chuoi theo dau ','
    char *pToken = strtok(m_szString, ",");
    if (!pToken) return;

    std::string cmd = pToken; 

    pToken = strtok(NULL, ",");
    std::string arg = (pToken ? pToken : ""); // Tham so,
	int value = atoi(arg.c_str()); // chuyen str thanh number

    // So sanh lenh
    if (cmd == "Earn")
    {
        Player[nIndex].Earn(value);
    }
    else if (cmd == "EarnAll")
    {
		for (int i = 0; i <	MAX_PLAYER; ++i)
			Player[i].Earn(value);
    }
    else if (cmd == "EarnXu")
    {
         Player[nIndex].EarnXu(value);
    }
    else if (cmd == "EarnXuAll")
    {
		for (int i = 0; i <	MAX_PLAYER; ++i)
			Player[i].EarnXu(value);
    }
    else if (cmd == "SetLevel")
    {
         Player[nIndex].SetLevel(value);
    }
    else if (cmd == "SetLevelAll")
    {
		for (int i = 0; i <	MAX_PLAYER; ++i)
			Player[i].SetLevel(value);
    }
    else if (cmd == "TatGS") // Lenh tat game server
    {
        exit(0);
    }
	return;	
}

//相应客户端 需要同步的基本数据
void KProtocolProcess::c2splayerreq(int nIndex, BYTE* pProtocol)
{
    PLAYER_REQ_COMMAND	*pRep = (PLAYER_REQ_COMMAND*)pProtocol;
	int m_nIndex = NpcSet.SearchID(pRep->nNpcDwid);
	if  (m_nIndex<=0)
	{
		printf("player sync data  failed...\n");
		return;
	}

	int  nPlayerIndex = nIndex;//Npc[m_nIndex].GetPlayerIdx();

	if  (nPlayerIndex<=0 || nPlayerIndex>=MAX_PLAYER)
	{
		printf("player  is full ...\n");
		return;
	}
	// 这个消息必须在同步世界NPC数据后做，使客户端能找到当前玩家在客户端的Npc索引
	CURPLAYER_SYNC	sSync;	// 同步当前玩家的自身独特信息给客户端（装备等）
	sSync.ProtocolType = (BYTE)s2c_synonlineplayer;
	sSync.m_dwID = Npc[m_nIndex].m_dwID;
	sSync.m_btLevel = (WORD)Npc[m_nIndex].m_Level;
	sSync.m_btSex = Npc[m_nIndex].m_nSex;
	sSync.m_btKind = Npc[m_nIndex].m_Kind;
	sSync.m_btSeries = Npc[m_nIndex].m_Series;
	sSync.m_wLifeMax = Npc[m_nIndex].m_LifeMax;
	sSync.m_wNuqiMax = Npc[m_nIndex].m_NuqiMax;  //怒气值
	sSync.m_wStaminaMax = Npc[m_nIndex].m_StaminaMax;
	sSync.m_wManaMax = Npc[m_nIndex].m_ManaMax;
	sSync.m_HeadImage = Npc[m_nIndex].m_HeadImage;
	sSync.m_wAttributePoint = Player[nPlayerIndex].m_nAttributePoint;
	sSync.m_wSkillPoint = Player[nPlayerIndex].m_nSkillPoint;
	sSync.m_wStrength = Player[nPlayerIndex].m_nStrength;
	sSync.m_wDexterity = Player[nPlayerIndex].m_nDexterity;
	sSync.m_wVitality = Player[nPlayerIndex].m_nVitality;
	sSync.m_wEngergy = Player[nPlayerIndex].m_nEngergy;
	sSync.m_wLucky = Player[nPlayerIndex].m_nLucky;//10
	sSync.m_nExp = Player[nPlayerIndex].m_nExp;
	sSync.m_dwLeadExp = Player[nPlayerIndex].m_dwLeadExp;
	sSync.m_btCurFaction = Player[nPlayerIndex].m_cFaction.m_nCurFaction;
	sSync.m_btFirstFaction = Player[nPlayerIndex].m_cFaction.m_nFirstAddFaction;
	sSync.m_nFactionAddTimes = Player[nPlayerIndex].m_cFaction.m_nAddTimes;
	sSync.m_wWorldStat = (WORD)Player[nPlayerIndex].m_nWorldStat;   //世界排名
	sSync.m_wSectStat = (WORD)Player[nPlayerIndex].m_nSectStat;     //门派排名
	sSync.m_nMoney1 = Player[nPlayerIndex].m_ItemList.GetsMon(room_equipment);
	sSync.m_nMoney2 = Player[nPlayerIndex].m_ItemList.GetsMon(room_repository);
	sSync.m_nXu = Player[nPlayerIndex].m_ItemList.GetxLient(room_equipment);
	if (SUCCEEDED(g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, (BYTE*)&sSync, sizeof(CURPLAYER_SYNC))))
	{
		printf("player Packing sync data suss...\n");
	}
	else
	{
		printf("player Packing sync data failed...\n");
	}
}


void KProtocolProcess::c2srunscript(int nIndex, BYTE* pProtocol)
{
	CLIENT_RUN_SCRIPT	*pRun = (CLIENT_RUN_SCRIPT*)pProtocol;

	if  (nIndex<=0 || nIndex>=MAX_PLAYER) return;

    if (pRun)
	{
		if (pRun->nKind==0)
		{//技能使用
			Player[nIndex].ExeNoBackScript("\\script\\system\\runscript.lua","sysrunscript",pRun->nKind,pRun->nVala,pRun->nValb,pRun->nValc);
		}
		else if (pRun->nKind==1)
		{//同步技能距离
			Npc[Player[nIndex].m_nIndex].ChangeSkillAttackRadius(pRun->nVala,pRun->nValb);
		}
		else if (pRun->nKind==2)
		{//界面打开给与界面
			Player[nIndex].ExeNoBackScript("\\script\\system\\opensysui.lua","opensysui",pRun->nKind,pRun->nVala,pRun->nValb,pRun->nValc);
		}
		else
			return;
	}
}

void KProtocolProcess::c2ssettaskval(int nIndex, BYTE* pProtocol)
{
	PLAYER_SET_TASKVAL_COMMAND	*pTask = (PLAYER_SET_TASKVAL_COMMAND*)pProtocol;
	
	int	nPlayer;
	    nPlayer = Player[nIndex].FindAroundPlayer(pTask->m_pDwid);
	if (nPlayer < 0)
		return;

	Player[nPlayer].m_cTask.SetSaveVal(pTask->m_TaskId,0);

}

void KProtocolProcess::c2ssynshopidx(int nIndex, BYTE* pProtocol)
{	   
	   return; 
}

void KProtocolProcess::c2splayersetpos(int nIndex, BYTE* pProtocol)
{
	   PLAYER_SYN_POS	 *pPos = (PLAYER_SYN_POS*)pProtocol;
       if (nIndex)
		   Npc[Player[nIndex].m_nIndex].SetPos(pPos->nXpos,pPos->nYpos);

	  return; 
}


//获取股子有效数据
void KProtocolProcess::c2splayerguzgame(int nIndex, BYTE* pProtocol)
{
	   return;
	   /*PLAYER_SYN_TYPE	 *pType = (PLAYER_SYN_TYPE*)pProtocol;
	   if (nIndex)
	   Player[nIndex].GetTypeData(pType->nType);
	   */
}


//玩家自动换装
void KProtocolProcess::c2splayerautoequip(int nIndex, BYTE* pProtocol)
{
	PLAYER_AUTO_EQUIP	*eInfo = (PLAYER_AUTO_EQUIP*)pProtocol;
	char nItemInfo[128];
	 if (nIndex && eInfo->bKind==0)
	 {//换装 只能是包袱--装备栏 双方
		 int nIdx=ItemSet.SearchID(eInfo->uIiemDwId);

		 if  (nIdx<=0 || nIdx>=MAX_ITEM)
		 {
			 Player[nIndex].m_ItemList.msgshow("换装:物品信不存在C,是否非法操作!");
#ifdef _SERVER
			 //客户端存在这个异常的物品 需要删除掉.
			 ITEM_REMOVE_SYNC	sRemove;
			 sRemove.ProtocolType = s2c_removeitem;
			 sRemove.m_ID         = eInfo->uIiemDwId;	
			 sRemove.m_dx         = 0;
			 sRemove.m_dy         = 0;
			 sRemove.m_model   = 0;
			 sRemove.nPlace       =-1;
			 g_pServer->PackDataToClient(Player[nIndex].m_nNetConnectIdx, (BYTE*)&sRemove, sizeof(ITEM_REMOVE_SYNC));
			 snprintf(nItemInfo, sizeof(nItemInfo),"----c2splayerautoequip:账号:%s角色:%s,涉嫌复制物品,删除异常物品成功!-----",Player[nIndex].m_AccoutName,Player[nIndex].m_PlayerName/*,Item[m_ItemList.m_Items[nListIdx].nIdx].GetName()*/);
			 g_LogFile.gTraceLogFile(nItemInfo,strlen(nItemInfo));
#endif
			 return;
		 }


		 int nSplace=Player[nIndex].m_ItemList.GetEquPlace(Item[nIdx].GetDetailType(),nIdx); //这个能装备的位置
		 int nEuqIdx=Player[nIndex].m_ItemList.GetEquipment(nSplace);                   //装备栏是否有装备
		
		if ((Item[nIdx].GetDurability()>=0 && Item[nIdx].GetDurability()<=3) || Item[nIdx].GetIsCanUse()==0)
		{
			Player[nIndex].m_ItemList.msgshow("<c=yellow>换装S<c>:该装备不满足换装条件");
			return;
		}

		int nItemindex=Player[nIndex].m_ItemList.SearchItemsIdx(eInfo->uIiemDwId);	
		if  (nItemindex<=0)
		{//身上没有这个物品 
			Player[nIndex].m_ItemList.msgshow("换装:物品信不存在D,是否非法操作!");
			ITEM_REMOVE_SYNC	sRemove;
			sRemove.ProtocolType = s2c_removeitem;
			sRemove.m_ID         = eInfo->uIiemDwId;
			sRemove.nPlace       = eInfo->nPlace;
			sRemove.m_dx         = eInfo->pos_X ;
			sRemove.m_dy         = eInfo->pos_Y;
			sRemove.m_model   = 0;
			g_pServer->PackDataToClient(Player[nIndex].m_nNetConnectIdx, (BYTE*)&sRemove, sizeof(ITEM_REMOVE_SYNC));

			snprintf(nItemInfo, sizeof(nItemInfo),"----c2splayerautoequip:账号:%s角色:%s,涉嫌复制物品:%s[+%d],删除异常物品成功!A-----",Player[nIndex].m_AccoutName,Player[nIndex].m_PlayerName,Item[nIdx].GetName(),Item[nIdx].GetEnChance());
			g_LogFile.gTraceLogFile(nItemInfo,strlen(nItemInfo));

			if (Player[nIndex].m_ItemList.Remove(nIdx,0,0,0,FALSE))
				ItemSet.Remove(nIdx);
			return;
		}

		ItemPos sScrPos,dDesPos;
		Player[nIndex].m_ItemList.GetItemsPos(nItemindex,&sScrPos); //服务器的源位置

		if (sScrPos.nPlace!=eInfo->nPlace || sScrPos.nX!=eInfo->pos_X || sScrPos.nY!=eInfo->pos_Y)
		{//同一个物品 位置不同 证明是异常物品
			Player[nIndex].m_ItemList.msgshow("换装:物品信不存在A,是否非法操作!");
#ifdef _SERVER
			ITEM_REMOVE_SYNC	sRemove;
			sRemove.ProtocolType = s2c_removeitem;
			sRemove.m_ID         = eInfo->uIiemDwId;
			sRemove.nPlace       = eInfo->nPlace;
			sRemove.m_dx         = eInfo->pos_X ;
			sRemove.m_dy         = eInfo->pos_Y;
			sRemove.m_model   = 0;
			g_pServer->PackDataToClient(Player[nIndex].m_nNetConnectIdx, (BYTE*)&sRemove, sizeof(ITEM_REMOVE_SYNC));


			snprintf(nItemInfo, sizeof(nItemInfo),"----c2splayerautoequip:账号:%s角色:%s,涉嫌复制物品:%s[+%d],删除异常物品成功!B-----",Player[nIndex].m_AccoutName,Player[nIndex].m_PlayerName,Item[nIdx].GetName(),Item[nIdx].GetEnChance());
			g_LogFile.gTraceLogFile(nItemInfo,strlen(nItemInfo));

			if (Player[nIndex].m_ItemList.Remove(nIdx,0,0,0,FALSE))
				ItemSet.Remove(nIdx);
	
#endif
			return;
		}

		/*
		sScrPos.nPlace=Player[nIndex].m_ItemList.m_Items[nItemindex].nPlace;
		sScrPos.nX=Player[nIndex].m_ItemList.m_Items[nItemindex].nX;
		sScrPos.nY=Player[nIndex].m_ItemList.m_Items[nItemindex].nY;
		*/
		if (nEuqIdx)
		{//如果位置有装备
			if 	(nSplace == itempart_ring1)
			{//原来位置有东西 上芥子
				if (!Player[nIndex].m_ItemList.GetEquipment(itempart_ring2))
					nSplace = itempart_ring2;
			}

			int x,y;
			if (Player[nIndex].m_ItemList.CheckCanPlaceInEquipment(Item[nEuqIdx].GetWidth(), Item[nEuqIdx].GetHeight(), &x, &y))
			{//发往客户端 卸载装备
				dDesPos.nPlace=pos_equiproom;
				dDesPos.nX=x;
				dDesPos.nY=y;
			}
			else
			{
			  Player[nIndex].m_ItemList.msgshow("<c=yellow>换装<c>:包袱空间不足,换装失败!");
			  return;
			}

		}
		else
		{//装备栏 位置没有装备的
			dDesPos.nPlace=pos_equiproom;
			dDesPos.nX=0;  
			dDesPos.nY=0;
		}
	
	    Player[nIndex].m_ItemList.AutoEquip(sScrPos,dDesPos,eInfo->uIiemDwId,nSplace,eInfo->bKind);
	 }
	 else if (nIndex && eInfo->bKind>=1)
	 {//卸装  装备栏---包袱  单方面
		 int nIdx=ItemSet.SearchID(eInfo->uIiemDwId); //要卸载的装备的索引
		 if (nIdx<=0 || nIdx>=MAX_ITEM)
		 {
			 Player[nIndex].m_ItemList.msgshow("卸装:物品信不存在F,是否非法操作!");
#ifdef _SERVER
			 //客户端存在这个异常的物品 需要删除掉.
			 ITEM_REMOVE_SYNC	sRemove;
			 sRemove.ProtocolType = s2c_removeitem;
			 sRemove.m_ID         = eInfo->uIiemDwId;	
			 sRemove.m_dx         = 0;
			 sRemove.m_dy         = 0;
			 sRemove.m_model   = 0;
			  sRemove.nPlace      =-1;
			 g_pServer->PackDataToClient(Player[nIndex].m_nNetConnectIdx, (BYTE*)&sRemove, sizeof(ITEM_REMOVE_SYNC));
			 snprintf(nItemInfo, sizeof(nItemInfo),"----c2splayerautoequip:账号:%s角色:%s,涉嫌复制物品,删除异常物品成功!-----",Player[nIndex].m_AccoutName,Player[nIndex].m_PlayerName/*,Item[m_ItemList.m_Items[nListIdx].nIdx].GetName()*/);
			 g_LogFile.gTraceLogFile(nItemInfo,strlen(nItemInfo));
#endif
			 return;
		 }

		 int nSplace=Player[nIndex].m_ItemList._getEquipPlaceByItemIdx(nIdx);
		 if  (nSplace==-1) return;
		 
		 ItemPos sScrPos,dDesPos;

		 int nItemindex=Player[nIndex].m_ItemList.SearchItemsIdx(eInfo->uIiemDwId);

		 if  (nItemindex<=0)
		 {//身上没有这个物品 
			 Player[nIndex].m_ItemList.msgshow("卸装:物品信不存在D,是否非法操作!");
			 ITEM_REMOVE_SYNC	sRemove;
			 sRemove.ProtocolType = s2c_removeitem;
			 sRemove.m_ID         = eInfo->uIiemDwId;
			 sRemove.nPlace       = eInfo->nPlace;
			 sRemove.m_dx         = eInfo->pos_X ;
			 sRemove.m_dy         = eInfo->pos_Y;
			 sRemove.m_model   = 0;
			 g_pServer->PackDataToClient(Player[nIndex].m_nNetConnectIdx, (BYTE*)&sRemove, sizeof(ITEM_REMOVE_SYNC));

			 snprintf(nItemInfo, sizeof(nItemInfo),"----c2splayerautoequip卸装:账号:%s角色:%s,涉嫌复制物品:%s[+%d],删除异常物品成功!A-----",Player[nIndex].m_AccoutName,Player[nIndex].m_PlayerName,Item[nIdx].GetName(),Item[nIdx].GetEnChance());
			 g_LogFile.gTraceLogFile(nItemInfo,strlen(nItemInfo));

			 if (Player[nIndex].m_ItemList.Remove(nIdx,0,0,0,FALSE))
				 ItemSet.Remove(nIdx);
			 return;
		 }

		 Player[nIndex].m_ItemList.GetItemsPos(nItemindex,&sScrPos);   //原来的位置信息

		 if (sScrPos.nPlace!=eInfo->nPlace || sScrPos.nX!=eInfo->pos_X || sScrPos.nY!=eInfo->pos_Y)
		 {//同一个物品ID 位置不同 证明是异常物品
			 Player[nIndex].m_ItemList.msgshow("卸装:物品信不存在B,是否非法操作!");
#ifdef _SERVER
			 ITEM_REMOVE_SYNC	sRemove;
			 sRemove.ProtocolType = s2c_removeitem;
			 sRemove.m_ID         = eInfo->uIiemDwId;	
			 sRemove.nPlace       = eInfo->nPlace;
			 sRemove.m_dx         = eInfo->pos_X ;
			 sRemove.m_dy         = eInfo->pos_Y;
			 sRemove.m_model   = 0;
			 g_pServer->PackDataToClient(Player[nIndex].m_nNetConnectIdx, (BYTE*)&sRemove, sizeof(ITEM_REMOVE_SYNC));

			 snprintf(nItemInfo, sizeof(nItemInfo),"----c2splayerautoequip卸装:账号:%s角色:%s,涉嫌复制物品:%s[+%d],删除异常物品成功!B-----",Player[nIndex].m_AccoutName,Player[nIndex].m_PlayerName,Item[nIdx].GetName(),Item[nIdx].GetEnChance());
			 g_LogFile.gTraceLogFile(nItemInfo,strlen(nItemInfo));

			 if (Player[nIndex].m_ItemList.Remove(nIdx,0,0,0,FALSE))
				 ItemSet.Remove(nIdx);

#endif
			 return;
		 }

		 int x,y;
		 if (Player[nIndex].m_ItemList.CheckCanPlaceInEquipment(Item[nIdx].GetWidth(), Item[nIdx].GetHeight(),&x,&y))
		 {
			 dDesPos.nPlace=pos_equiproom;
			 dDesPos.nX=x;
			 dDesPos.nY=y;
		 }
		 else
		 {
			 Player[nIndex].m_ItemList.msgshow("<c=cyan>卸装<c>:包袱空间不足,卸装失败!");
			 return;
		 }

		 Player[nIndex].m_ItemList.AutoEquip(sScrPos,dDesPos,eInfo->uIiemDwId,nSplace,eInfo->bKind);
	 }

}


void    KProtocolProcess::c2ssynnpcinfo(int nIndex, BYTE* pProtocol)
{
	NPC_SYN_INFO	*pInfo = (NPC_SYN_INFO*)pProtocol;
	//Player[nIndex].m_nRoomFull=pVipDo->m_IsNoRoom;
	
	{
	    
		Npc[Player[nIndex].m_nIndex].SynNpcInfoToClient(pInfo->nNpcDwid);

	}
    /*Player[nIndex].UpdataCurData();
	BYTE	NetCommand = (BYTE)s2c_syncurupdata;
	g_pServer->PackDataToClient(Player[nIndex].m_nNetConnectIdx, &NetCommand, sizeof(BYTE));*/

}

void    KProtocolProcess::c2sroomisfull(int nIndex, BYTE* pProtocol)
{
	PLAYER_NO_ROOM_INFO	*pVipDo = (PLAYER_NO_ROOM_INFO*)pProtocol;
	Player[nIndex].m_nRoomFull=pVipDo->m_IsNoRoom;
}

void KProtocolProcess::c2ssyncurdata(int nIndex, BYTE* pProtocol)
{
  
    PLAYER_JINMAI_GETBACK	sTaskBack;
	sTaskBack.ProtocolType = s2c_JinMaicallback;
	//sTaskBack.m_TaskId  = pTask->m_TaskId;
	sTaskBack.m_TaskVala = Player[nIndex].m_cTask.GetSaveVal(3501);
	sTaskBack.m_TaskValb = Player[nIndex].m_cTask.GetSaveVal(3502);
	sTaskBack.m_TaskValc = Player[nIndex].m_cTask.GetSaveVal(3503);
	sTaskBack.m_TaskVald = Player[nIndex].m_cTask.GetSaveVal(3504);
	sTaskBack.m_TaskVale = Player[nIndex].m_cTask.GetSaveVal(3505);
	sTaskBack.m_TaskValf = Player[nIndex].m_cTask.GetSaveVal(3506);
	sTaskBack.m_TaskValg = Player[nIndex].m_cTask.GetSaveVal(3507);
	sTaskBack.m_TaskValh = Player[nIndex].m_cTask.GetSaveVal(3508);
	if (g_pServer)
		g_pServer->PackDataToClient(Player[nIndex].m_nNetConnectIdx, (BYTE*)&sTaskBack, sizeof(PLAYER_JINMAI_GETBACK));

    //Player[nIndex].UpdataCurData();

	/*BYTE	NetCommand = (BYTE)s2c_syncurupdata;
	if (g_pServer)
	   g_pServer->PackDataToClient(Player[nIndex].m_nNetConnectIdx, &NetCommand, sizeof(BYTE));*/


}

//进度条回调函数
void KProtocolProcess::c2sjinducallback(int nIndex, BYTE* pProtocol)
{
    PLAYER_JINDU_CALLBACK	*pJd = (PLAYER_JINDU_CALLBACK*)pProtocol;

    Player[nIndex].ExecuteScriptD(Player[nIndex].m_nGiveCallBackScript,pJd->szCallFuc,pJd->szParma,pJd->szParmb,0,0,0,0,0,TRUE);			
}

void KProtocolProcess::c2sgettaskval(int nIndex, BYTE* pProtocol)
{
	PLAYER_GET_TASKVAL_COMMAND	*pTask = (PLAYER_GET_TASKVAL_COMMAND*)pProtocol;

	int	nPlayer;
	    nPlayer = Player[nIndex].FindAroundPlayer(pTask->m_pDwid);
	if (nPlayer < 0)
		return;

	PLAYER_GETBACK_TASKVAL	sTaskBack;
	sTaskBack.ProtocolType = s2c_taskcallback;
	sTaskBack.m_TaskId  = pTask->m_TaskId;
	sTaskBack.m_TaskVal = Player[nPlayer].m_cTask.GetSaveVal(pTask->m_TaskId);
	if (g_pServer)
		g_pServer->PackDataToClient(Player[nPlayer].m_nNetConnectIdx, (BYTE*)&sTaskBack, sizeof(PLAYER_GETBACK_TASKVAL));
}


void KProtocolProcess::c2splayerdoScript(int nIndex, BYTE* pProtocol)
{
     PLAYER_DOSCRIPT *pDoscript = (PLAYER_DOSCRIPT*)pProtocol;
     char nItemInfo[32];
	      ZeroMemory(nItemInfo,	sizeof(nItemInfo));
	      snprintf(nItemInfo, sizeof(nItemInfo), "%s", pDoscript->szItemInfo);

	 if (pDoscript->inKind>=0)
	 {   
		 if (pDoscript->inKind >= 99 &&  pDoscript->inKind <= 114)
		 {
			 KTabFile nChouJiang;
              if (nChouJiang.Load("\\Settings\\choujianglist.txt"))
			  {
				  int nIsForbit;
				  nChouJiang.GetInteger(pDoscript->inCurTime+1,"IsNo",0,&nIsForbit);
                  nChouJiang.GetString(pDoscript->inCurTime+1,"ItemInfo","",nItemInfo,sizeof(nItemInfo));

				  if (nIsForbit==1)
				  {
					  Player[nIndex].m_ItemList.msgshow("提示:您运气不佳,没有抽到任何奖品!");
                      nChouJiang.Clear();
					  return;
				  }
			  }
			  nChouJiang.Clear();         
		 } 
		 else if  (pDoscript->inKind == 115)
		 {
			 Npc[Player[nIndex].m_nIndex].SetNpcCishu(pDoscript->inCurTime);  //抽奖次数
              return;
		 }

         Player[nIndex].ExecuteScriptC("\\script\\system\\activity.lua","main",Npc[Player[nIndex].m_nIndex].Name,nItemInfo,pDoscript->inKind,pDoscript->inCurTime,Player[nIndex].m_nIndex,pDoscript->inCurTimeb,0,FALSE);
	 }
	 else
	 {//小于0的
        Player[nIndex].ExecuteScriptC("\\script\\system\\duanzhao.lua","main",Npc[Player[nIndex].m_nIndex].Name,"",pDoscript->inKind,pDoscript->inCurTime,pDoscript->inCurTimeb,pDoscript->inCurTimec);
	 }

}

#define     TONG_SET_FILE             "\\TongSet\\TongSet.ini"
#define     TONG_GET_FILE             "\\TongSet\\"
void KProtocolProcess::c2splayertongset(int nIndex, BYTE* pProtocol)
{
	PLAYER_TONGINFO	*pView = (PLAYER_TONGINFO*)pProtocol;
    char nSetPathSetion[NAME_LENGHT];
    int nTarIndex;
	if (pView->inType==1 || pView->inType==3 || pView->inType==5)  //获取
	{//更新公告和 帮会设置标示
		nTarIndex =NpcSet.SearchID(pView->inAcceptLevel);  //被查看人的playridx
		sprintf(nSetPathSetion,"%s",Player[Npc[nTarIndex].GetnPlayerIdx()].m_cTong.m_szName);
	}
	else//0 2  //帮会作为节名
		sprintf(nSetPathSetion,"%s",Player[nIndex].m_cTong.m_szName);

  //  sprintf(nTongSetPath,TONG_SET_FILE);

	KIniFile nTongSet;
	KFile nFile;

    if (!g_FileExists(TONG_SET_FILE))//检测这个文件是否存在
	{ 
	   nFile.Create(TONG_SET_FILE);   
	} 
       nFile.Close();

    //ZeroMemory(nTongSetPath,sizeof(nTongSetPath));

    if (!nTongSet.Load(TONG_SET_FILE))
	{
		Player[nIndex].m_ItemList.msgshow("提示:数据有误,请联系GM处理!");	
		nTongSet.Clear();
		return;
	}

    if (nTongSet.IsSectionExist(nSetPathSetion))     
	{//检测这个节名是否存在 
 
	} 

	if (pView->inType==0)
	{//保存帮会设置
       nTongSet.WriteString(nSetPathSetion,"QingXiang",pView->szQingXiang);
	   nTongSet.WriteString(nSetPathSetion,"HuoDong1",pView->szHuoDong1);
	   nTongSet.WriteString(nSetPathSetion,"HuoDong2",pView->szHuoDong2);
	   nTongSet.WriteString(nSetPathSetion,"HuoDong3",pView->szHuoDong3);
	   nTongSet.WriteString(nSetPathSetion,"HuoDong4",pView->szHuoDong4);
	   nTongSet.WriteString(nSetPathSetion,"JiYu",pView->szJiYu);
	   nTongSet.WriteInteger(nSetPathSetion,"ApplyLevel",pView->inAcceptLevel);
       nTongSet.WriteInteger(nSetPathSetion,"RefuseLevel",pView->inRefuseLevel);
       nTongSet.Save(TONG_SET_FILE);
	   Player[nIndex].m_ItemList.msgshow("提示:<c=yellow>帮会招募<c>信息保存成功!");
	}
	else if (pView->inType==1 || pView->inType==3)
	{//提取所有帮会设置和公告
		PLAYER_TONGINFO_CALLBACK nTongBack;
		memset(&nTongBack, 0, sizeof(nTongBack));
        nTongBack.ProtocolType =(BYTE)s2c_gettongback;
		nTongSet.GetString(nSetPathSetion,"QingXiang","",nTongBack.szQingXiang,sizeof(nTongBack.szQingXiang));
		nTongSet.GetString(nSetPathSetion,"HuoDong1","",nTongBack.szHuoDong1,sizeof(nTongBack.szHuoDong1));
		nTongSet.GetString(nSetPathSetion,"HuoDong2","",nTongBack.szHuoDong2,sizeof(nTongBack.szHuoDong2));
		nTongSet.GetString(nSetPathSetion,"HuoDong3","",nTongBack.szHuoDong3,sizeof(nTongBack.szHuoDong3));
		nTongSet.GetString(nSetPathSetion,"HuoDong4","",nTongBack.szHuoDong4,sizeof(nTongBack.szHuoDong4));
		nTongSet.GetString(nSetPathSetion,"JiYu","",nTongBack.szJiYu,sizeof(nTongBack.szJiYu));
		nTongSet.GetString(nSetPathSetion,"GongGao","",nTongBack.szGongGao,sizeof(nTongBack.szGongGao));
		nTongSet.GetInteger(nSetPathSetion,"ApplyLevel",10,&nTongBack.inAcceptLevel);
        nTongSet.GetInteger(nSetPathSetion,"RefuseLevel",5,&nTongBack.inRefuseLevel);
        nTongBack.inType=1;
	    g_pServer->PackDataToClient(Player[nIndex].m_nNetConnectIdx, &nTongBack, sizeof(PLAYER_TONGINFO_CALLBACK));
	}
	else if (pView->inType==2)
	{//公告保存
		nTongSet.WriteString(nSetPathSetion,"GongGao",pView->szGongGao);
		nTongSet.Save(TONG_SET_FILE);
	    Player[nIndex].m_ItemList.msgshow("提示:<c=yellow>帮会公告信息保存成功<c>!");
	}
	else if (pView->inType==5)
	{//获取申请入帮的人员列表
		KTabFile nTongGet;
        char nPath[125];
		snprintf(nPath, sizeof(nPath),"%s%s.txt",TONG_GET_FILE,nSetPathSetion);
	  if (nTongGet.Load(nPath))
	  { 
		PLAYER_TONGINFO_CALLBACK nTongBack;
		memset(&nTongBack, 0, sizeof(nTongBack));
        nTongBack.ProtocolType =(BYTE)s2c_gettongback;
        nTongBack.inType=5;

        int Rows = nTongGet.GetHeight();
        int m_btCurNum=0,nStartRow = pView->inRefuseLevel,nEndRow;  //开始的位置

            nEndRow = defTONG_ONE_PAGE_MAX_NUM+nStartRow;

		if (nEndRow > Rows)
            nEndRow=Rows;

		//开始记录数据
		for (int i=nStartRow;i<nEndRow;i++)
		{
			char nName[NAME_LENGHT];
			int  nLevel;
            nTongGet.GetString(i,"姓名","",nName,sizeof(nName));
            nTongGet.GetInteger(i,"等级",0,&nLevel);
			snprintf(nTongBack.m_sMember[m_btCurNum].m_szName, sizeof(nTongBack.m_sMember[m_btCurNum].m_szName), "%s", nName);	
			nTongBack.m_sMember[m_btCurNum].m_inLevel = nLevel;
			m_btCurNum++;
		}

		if (m_btCurNum == 0)
			return ;

         nTongBack.inRefuseLevel = nStartRow;   //原来要求的开始位置
		 nTongBack.inAcceptLevel = m_btCurNum;  //返回的数量

		nTongBack.m_wLength = sizeof(PLAYER_TONGINFO_CALLBACK) - sizeof(TONG_CALL_BACK_INFO) * (defTONG_ONE_PAGE_MAX_NUM - m_btCurNum);	   
		g_pServer->PackDataToClient(Player[nIndex].m_nNetConnectIdx, &nTongBack, nTongBack.m_wLength);		
	  } 
       nTongGet.Clear();
	}
    nTongSet.Clear();
}

void KProtocolProcess::c2supdateviewequip(int nIndex, BYTE* pProtocol)
{
	VIEW_EQUIP_COMMAND	*pView = (VIEW_EQUIP_COMMAND*)pProtocol;

}

//获取摆摊的数量
void KProtocolProcess::c2sNeedCount(int nIndex, BYTE* pProtocol)
{
	PLAYER_NEED_COUNT *pView = (PLAYER_NEED_COUNT *)pProtocol;
	if (pView->dwId == Npc[Player[nIndex].m_nIndex].m_dwID)
		return;
	int nPlayerIdx = Player[nIndex].FindAroundPlayer(pView->dwId);
	if (nPlayerIdx <= 0)
		return;
	Player[nPlayerIdx].SendSellItemCount(nIndex);
}
///买东西--摆摊
void KProtocolProcess::c2sTradeBuy(int nIndex, BYTE* pProtocol)
{
	if (Player[nIndex].CheckTrading())
	{
	    Player[nIndex].m_ItemList.msgshow("Loi du lieu khong chinh xac!");

		BYTE byFinished = s2c_itemexchangefinish;
			 if (g_pServer)
				g_pServer->PackDataToClient(Player[nIndex].m_nNetConnectIdx, &byFinished, sizeof(BYTE));

		return;
	}

	PLAYER_TRADE_BUY_ITEM_COMMAND* pPlayer = (PLAYER_TRADE_BUY_ITEM_COMMAND *)pProtocol;

	int nPlayerIdx = Player[nIndex].FindAroundPlayer(pPlayer->m_PlayerId);
	
	if (nPlayerIdx <= 0 || !Npc[Player[nPlayerIdx].m_nIndex].m_BaiTan)	           //摆摊者的状态
	{
	    Player[nIndex].m_ItemList.msgshow("Gian Hang nay da dong hoac khong ton tai!");

		BYTE byFinished = s2c_itemexchangefinish;
			 if (g_pServer)
				g_pServer->PackDataToClient(Player[nIndex].m_nNetConnectIdx, &byFinished, sizeof(BYTE));

		return;
	}

	int nTempIndex = ItemSet.SearchID(pPlayer->m_Idx);

	if  (nTempIndex<=0)
	{
		Player[nIndex].m_ItemList.msgshow("Vat pham nay khong ton tai hoac da ban roi!");

		BYTE byFinished = s2c_itemexchangefinish;
		if (g_pServer)
			g_pServer->PackDataToClient(Player[nIndex].m_nNetConnectIdx, &byFinished, sizeof(BYTE));

		return;
	}

	//int nPrice = Player[nPlayerIdx].m_ItemList.GetPrice(nTempIndex);            //摆摊者物品的标价
	int nPrice = Item[nTempIndex].GetTradePrice();

	if (nPrice<=0)
	{
		Player[nIndex].m_ItemList.msgshow("Loi gia vat pham mua that bai!");
		
		BYTE byFinished = s2c_itemexchangefinish;
		if (g_pServer)
			g_pServer->PackDataToClient(Player[nIndex].m_nNetConnectIdx, &byFinished, sizeof(BYTE));
		
		return;
	}

	if ((pPlayer->m_Model==0 || pPlayer->m_Model==1) && (Player[nIndex].m_ItemList.GetEquipmentMoney() < nPrice || nPrice<=0))      // 购买者的钱
	{
		Player[nIndex].m_ItemList.msgshow("Khong du tien Van");

		BYTE byFinished = s2c_itemexchangefinish;
			 if (g_pServer)
				g_pServer->PackDataToClient(Player[nIndex].m_nNetConnectIdx, &byFinished, sizeof(BYTE));

		return;
	}
    else if (pPlayer->m_Model==2 && (Player[nIndex].m_ItemList.GetEquipmentXu() < nPrice || nPrice<=0))      // 购买者的钱
	{
       	Player[nIndex].m_ItemList.msgshow("Ban khong du Xu!");

		BYTE byFinished = s2c_itemexchangefinish;
			 if (g_pServer)
				g_pServer->PackDataToClient(Player[nIndex].m_nNetConnectIdx, &byFinished, sizeof(BYTE));

		return;
	}

	int nIdx = ItemSet.Add(&Item[nTempIndex]); //产生可以用的节点
	if (nIdx<=0) return;
	int	x, y;
	if (!Player[nIndex].m_ItemList.CheckCanPlaceInEquipment(Item[nIdx].GetWidth(), Item[nIdx].GetHeight(), &x, &y))
	{//同步客户端（包里）
		//Player[nPlayerIdx].m_ItemList.Add(nItemIdx,pos_equiproom,x,y,0,0,nBei);  //包袱增加装备 同步装备--客户端
		//Player[nPlayerIdx].m_ItemList.Remove(nItemIdx);
		ItemSet.Remove(nIdx);
		Player[nIndex].m_ItemList.msgshow("Hanh trang khong du khoang trong");
		return ;
	} 
	//che do set tien van hay xu 
	if (pPlayer->m_Model==0 || pPlayer->m_Model==1)
	{
	    Player[nIndex].Pay(nPrice);   //购买者付费
		char strVP[128]={0};
		sprintf(strVP, "Ban da mua vat pham tieu ton: %d luong",nPrice); 
		Player[nIndex].m_ItemList.msgshow(strVP);
	}
	else
	{
	    Player[nIndex].PayXu(nPrice);
		char strVP[128] = {0};
		sprintf(strVP, "Ban da mua vat pham tieu ton: %d Xu", nPrice);
		Player[nIndex].m_ItemList.msgshow(strVP);
	}
	printf("--PlayerBuyShop Name:%s (%s) - Name:%s --\n",Player[nPlayerIdx].m_PlayerName, Item[nTempIndex].GetName(), Player[nIndex].m_PlayerName);
	//true la khong xep chong vao
	Player[nIndex].m_ItemList.Add(nIdx,pos_equiproom,x,y,0,0,Item[nIdx].GetStackNum(),TRUE);  //包袱增加东西

	//ExecuteScript("\\script\\System\\player\\muaban.lua","main",nPlayerIdx,nTempIndex);		
	Player[nIndex].ExecuteScript2("\\script\\System\\player\\logmuaban.lua","main",nPlayerIdx,nTempIndex);

/*	int nSubIndexID;
	    nSubIndexID=M_SubWorldSetB->SearchWorld(pPlayer->m_MapID);

        SubWorld[nSubIndexID].m_WarCityManey +=float(nPrice*(pPlayer->m_Shuihou)/100);  //城市税收收入
        SubWorld[nSubIndexID].m_TodayMoney +=float(nPrice*(pPlayer->m_Shuihou)/100);    //今天的收入
 */
	    Player[nPlayerIdx].m_ItemList.Remove(nTempIndex);                           //摆摊者包袱删除装备
		Player[nPlayerIdx].SendSellItemCount(nIndex); //xoa item o client
        ItemSet.Remove(nTempIndex);

	if (pPlayer->m_Model==0 || pPlayer->m_Model==1)
	{ 
		if (pPlayer->m_IsCityMenber==1)
			  Player[nPlayerIdx].Earn(nPrice); //摆摊者收入
		else if (pPlayer->m_Shuihou>0)      //税收
		{
			  nPrice-=float(nPrice*(pPlayer->m_Shuihou)/100);

			  Player[nPlayerIdx].Earn(nPrice); //摆摊者收入
		}
		else
			  Player[nPlayerIdx].Earn(nPrice); //摆摊者收入
	}
	else if (pPlayer->m_Model==2)
	{ 
		if (pPlayer->m_IsCityMenber==1)
			  Player[nPlayerIdx].EarnXu(nPrice); //摆摊者收入
		else if (pPlayer->m_Shuihou>0)          //税收
		{
			  nPrice-=float(nPrice*(pPlayer->m_Shuihou)/100);

			  Player[nPlayerIdx].EarnXu(nPrice); //摆摊者收入
		}
		else
			  Player[nPlayerIdx].EarnXu(nPrice);                       //摆摊者收入
	}
	
//////////////////////////////////写文件////////////////////////////////////////////////////
/*	char nFiledPath[128],nTitleName[128];
    int  nZIjing=0;
	//	strcat(nName,".txt");
    sprintf(nFiledPath,"%s","\\DataFiles\\Tong\\城市资金贡献\\襄阳\\jxb.txt");  //写入资金
	//   strcat(nFiledPath,nName);  //完整路径
	//	  sprintf(nTitleName,"%s","名称\t任务ID\t任务值");
	KTabFile nTXT;
	nTXT.Load(nFiledPath);
	nTXT.GetInteger(1,1,0,&nZIjing);
	nTXT.Clear();
	nZIjing+=100;
    sprintf(nTitleName,"%d",nZIjing);
	
	KFile mFile; 
	//mFile.Open(nFiledPath);
	mFile.Create(nFiledPath);
    mFile.Write(nTitleName, strlen(nTitleName));
	mFile.Close(); */

}

void KProtocolProcess::c2sSysShop(int nIndex, BYTE* pProtocol)
{
	TONG_DISMISS_SYNC *pDo = (TONG_DISMISS_SYNC *)pProtocol;
	if (pDo->m_nKind==0) //怒气
        Npc[Player[nIndex].m_nIndex].m_CurrentNuQi=0;

	if (pDo->m_nKind==4)  //点击山洞地图传送
	{
		char nValKey[32]={0},nValInfo[32]={0};
		int reg=0; 
		sprintf(nValKey,"%d_IsNoReturn",pDo->m_inParma);
		
		g_NpcMapDropRate.GetInteger("List",nValKey,0,&reg);
		
		if (reg)
		{
			Player[nIndex].m_ItemList.msgshow("Dich chuyen tuc thoi khong the su dung tren map nay!");
			return;
		}
		
		//点击地图移动 需要判断包袱里是否有神行符号
		if (!Player[nIndex].m_ItemList.IsItemName("Than Hanh Phu"))
		{
			Player[nIndex].m_ItemList.msgshow("Khong co <c=cyan>than hanh phu<cr> o hanh trang!");
			return;
		}
		if (!Player[nIndex].m_ItemList.CostMoney(500))
		{
			Player[nIndex].m_ItemList.msgshow("Ban khong du <c=cyan>500<c> luong!");
			return;
		}		
			//reg = Npc[Player[nIndex].m_nIndex].ChangeWorld(PnMap, PnMpsX, PnMpsY);  //同地图传送 SetPos
	}

	Player[nIndex].ExecuteScriptC("\\script\\system\\sysshop.lua","main",Npc[Player[nIndex].m_nIndex].Name,"",pDo->m_nKind,pDo->m_inParma,pDo->m_inParmb,pDo->m_inParmc,pDo->m_inParmd,TRUE);

	/*
	
	  sprintf(nValKey,"%d_MapType",PnMap);
	  g_NpcMapDropRate.GetString("List",nValKey,"",nValInfo,sizeof(nValInfo));
	  
		if (strstr(nValInfo,"City") || strstr(nValInfo,"Country") || strstr(nValInfo,"Capital") )
		{
		if  (reg==1)
		Npc[Player[nIndex].m_nIndex].SetFightMode(0);
		}
		else
		{
		if  (reg==1)
		Npc[Player[nIndex].m_nIndex].SetFightMode(1);
		
		  }
	*/
}

void KProtocolProcess::c2sNeedShopName(int nIndex, BYTE* pProtocol)
{
	PLAYER_NEED_COUNT *pShopName = (PLAYER_NEED_COUNT *)pProtocol;
	if (nIndex > 0 && nIndex < MAX_PLAYER)
	{ 
		int NpcId = NpcSet.SearchID(pShopName->dwId);
		PLAYER_PLAYER_SHOPNAME ShopName;
		ShopName.ProtocolType = s2c_shopname;
		ShopName.m_dwNpcID = Npc[NpcId].m_dwID;
		char nstrName[NAME_LENGHT]={0};
		Npc[Player[nIndex].m_nIndex].GetstrInfo(STR_SHOP_NAME,nstrName,sizeof(nstrName));
		strcpy(ShopName.m_Name,nstrName);  //是摆摊广告
		g_pServer->PackDataToClient(Player[nIndex].m_nNetConnectIdx, &ShopName, sizeof(PLAYER_PLAYER_SHOPNAME));
	}
}

//增加金币
void KProtocolProcess::c2saddjinbi(int nIndex, BYTE* pProtocol)
{
    CP_ADD_JINBI* pGz = (CP_ADD_JINBI*)pProtocol;  
	Player[nIndex].EarnXu(pGz->nJinBi);

}

//股子动画同步函数
void KProtocolProcess::c2sgufarm(int nIndex, BYTE* pProtocol)
{
/*    CP_GUZI_FARMS* pGz = (CP_GUZI_FARMS*)pProtocol;  
	int nSubIndexID;
        nSubIndexID=M_SubWorldSetB->SearchWorld(pGz->nmapid);
        SubWorld[nSubIndexID].m_CurFarmA=pGz->nFarmsA;
		SubWorld[nSubIndexID].m_CurFarmB=pGz->nFarmsB;
        SubWorld[nSubIndexID].SendSyncData(Player[nIndex].m_nIndex, Player[nIndex].m_nNetConnectIdx); 
 */
}

//股子同步函数
void KProtocolProcess::c2sguzi(int nIndex, BYTE* pProtocol)
{    
/*	     CP_GUZI* pGz = (CP_GUZI*)pProtocol;  
	     int nSubIndexID;
             nSubIndexID=M_SubWorldSetB->SearchWorld(pGz->nmapid);

         if (pGz->ndianshu>=0 && pGz->nZuoWeiHao==1)
		 {
		 SubWorld[nSubIndexID].m_DianshuA=pGz->ndianshu;              //1号座位设置地图点数
	//	 SubWorld[nSubIndexID].m_ZuoWeiHao=pGz->nzuoweihao+1;           // 设置地图当前可以甩股子的座位号
	//	 SubWorld[nSubIndexID].m_DianshuA=pGz->ndianshu;              //设置地图点数
         Npc[Player[nIndex].m_nIndex].m_GuziDianshu=pGz->ndianshu;    //设置玩家点数
         Npc[Player[nIndex].m_nIndex].m_IsShuai=1;                    //设置玩家是否已经甩过
    	 }
		 else if (pGz->ndianshu>=0 && pGz->nZuoWeiHao==2)
		 {
			 SubWorld[nSubIndexID].m_DianshuB=pGz->ndianshu;   //1号座位设置地图点数
		//	 SubWorld[nSubIndexID].m_ZuoWeiHao=pGz->nzuoweihao+1;   // 设置地图当前可以甩股子的座位号
		//	 SubWorld[nSubIndexID].m_DianshuB=pGz->ndianshu; //设置地图点数
			 Npc[Player[nIndex].m_nIndex].m_GuziDianshu=pGz->ndianshu; //设置玩家点数
			 Npc[Player[nIndex].m_nIndex].m_IsShuai=1;     //设置玩家是否已经甩过
    	 }
		 else if (pGz->ndianshu>=0 && pGz->nZuoWeiHao==3)
		 {
			 SubWorld[nSubIndexID].m_DianshuC=pGz->ndianshu;   //1号座位设置地图点数
		//	 SubWorld[nSubIndexID].m_ZuoWeiHao=pGz->nzuoweihao+1;   // 设置地图当前可以甩股子的座位号
		//	 SubWorld[nSubIndexID].m_DianshuC=pGz->ndianshu; //设置地图点数
			 Npc[Player[nIndex].m_nIndex].m_GuziDianshu=pGz->ndianshu; //设置玩家点数
			 Npc[Player[nIndex].m_nIndex].m_IsShuai=1;     //设置玩家是否已经甩过
    	 }
		 else if (pGz->ndianshu>=0 && pGz->nZuoWeiHao==4)
		 {
			 SubWorld[nSubIndexID].m_DianshuD=pGz->ndianshu;   //1号座位设置地图点数
		//	 SubWorld[nSubIndexID].m_ZuoWeiHao=pGz->nzuoweihao+1;   // 设置地图当前可以甩股子的座位号
		//	 SubWorld[nSubIndexID].m_DianshuD=pGz->ndianshu; //设置地图点数
			 Npc[Player[nIndex].m_nIndex].m_GuziDianshu=pGz->ndianshu; //设置玩家点数
			 Npc[Player[nIndex].m_nIndex].m_IsShuai=1;     //设置玩家是否已经甩过
    	 }
		 else if (pGz->ndianshu>=0 && pGz->nZuoWeiHao==5)
		 {
			 SubWorld[nSubIndexID].m_DianshuE=pGz->ndianshu;   //1号座位设置地图点数
		//	 SubWorld[nSubIndexID].m_ZuoWeiHao=pGz->nzuoweihao+1;   // 设置地图当前可以甩股子的座位号
		//	 SubWorld[nSubIndexID].m_DianshuE=pGz->ndianshu; //设置地图点数
			 Npc[Player[nIndex].m_nIndex].m_GuziDianshu=pGz->ndianshu; //设置玩家点数
			 Npc[Player[nIndex].m_nIndex].m_IsShuai=1;     //设置玩家是否已经甩过
    	 }
		 else if (pGz->nZuoWeiHao==6)  //结算后执行的同步
		 {
             SubWorld[nSubIndexID].m_ZuoWeiHao=pGz->ndianshu;  //下一轮开始

			 if (pGz->ndianshu==0)   
			 {//完毕清空数据
                SubWorld[nSubIndexID].m_GyajingA=0;   //押金清零
				SubWorld[nSubIndexID].m_GyajingB=0;
				SubWorld[nSubIndexID].m_GyajingC=0;
				SubWorld[nSubIndexID].m_GyajingD=0;
				SubWorld[nSubIndexID].m_GyajingE=0;

                Npc[Player[nIndex].m_nIndex].m_IsShuai=0;     //设置未甩股
				Npc[Player[nIndex].m_nIndex].m_GuziDianshu=0; //玩家的点数清零
                
				SubWorld[nSubIndexID].m_DianshuA=0;   //地图的点数清零
				SubWorld[nSubIndexID].m_DianshuB=0;
				SubWorld[nSubIndexID].m_DianshuC=0;
				SubWorld[nSubIndexID].m_DianshuD=0;
				SubWorld[nSubIndexID].m_DianshuE=0;

				SubWorld[nSubIndexID].m_IsStartA=0;   //是否开始了
				SubWorld[nSubIndexID].m_IsStartB=0;
				SubWorld[nSubIndexID].m_IsStartC=0;
				SubWorld[nSubIndexID].m_IsStartD=0;
				SubWorld[nSubIndexID].m_IsStartE=0;

				SubWorld[nSubIndexID].m_IsXiaZhuA=0;  //是否下注了
				SubWorld[nSubIndexID].m_IsXiaZhuB=0;
				SubWorld[nSubIndexID].m_IsXiaZhuC=0;
				SubWorld[nSubIndexID].m_IsXiaZhuD=0;
				SubWorld[nSubIndexID].m_IsXiaZhuE=0;

			 }
		 }

		 if  (pGz->nZuoWeiHao==11)   //第一个玩家的开始和停止
             SubWorld[nSubIndexID].m_IsStartA=pGz->ndianshu;
		 else if (pGz->nZuoWeiHao==12)
			 SubWorld[nSubIndexID].m_IsStartB=pGz->ndianshu;
		 else if (pGz->nZuoWeiHao==13)
			 SubWorld[nSubIndexID].m_IsStartC=pGz->ndianshu;
		 else if (pGz->nZuoWeiHao==14)
			 SubWorld[nSubIndexID].m_IsStartD=pGz->ndianshu;
		 else if (pGz->nZuoWeiHao==15)
			 SubWorld[nSubIndexID].m_IsStartE=pGz->ndianshu;

		 SubWorld[nSubIndexID].SendSyncData(Player[nIndex].m_nIndex, Player[nIndex].m_nNetConnectIdx); 
  */
}

void KProtocolProcess::c2sLiXian(int nIndex, BYTE* pProtocol)
{
	Player[nIndex].ExecuteScriptC("\\script\\system\\player\\offline.lua","offline","","");
}
//输入字符串
void KProtocolProcess::NpcStringCommand(int nIndex, BYTE* pProtocol) 
{
//	CP_STRING 		*pStringCmd;

	PLAYER_STRING_CALLBACK* pSc = (PLAYER_STRING_CALLBACK*)pProtocol;

//	pStringCmd = (CP_STRING *)pProtocol;
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;
	if (Player[nIndex].m_nGiveCallBackScript && Player[nIndex].m_nGiveCallBackNum)
	{
//	Player[nIndex].String(pSc->nbutton);
//	if (sizeof(pSc->m_Content)<=0)
		//开始处理 带参数的回调函数
	  char m_CurCallBack[64]={0};
	  char m_ParmStr[64]={0};
	 // char *m_NewCallStr=strstr(pSc->m_SCallback, "#");  
	  int  nSrrLen = strlen(pSc->m_SCallback);
	  std::string _curSirng = pSc->m_SCallback;
      size_t pos_f=std::string::npos;
	  pos_f = _curSirng.find("#",0);
	  if  (pos_f != std::string::npos) 
	  {//找到了
		  std::string nFirstContent = _curSirng.substr(pos_f,nSrrLen-pos_f);     //前面的内容
		  if (!nFirstContent.empty())
			  snprintf(m_CurCallBack, sizeof(m_CurCallBack), "%s", nFirstContent.c_str());
	  }
	  else
		  snprintf(m_CurCallBack, sizeof(m_CurCallBack), "%s", pSc->m_SCallback);

	  //if (m_NewCallStr)
		 // g_ExtractChar(pSc->m_SCallback,'#','#',m_CurCallBack,m_ParmStr,2); //分割字符串
	  //else
		 // sprintf(m_CurCallBack,pSc->m_SCallback);



	  Player[nIndex].ExecuteScriptD(Player[nIndex].m_nGiveCallBackScript,m_CurCallBack,pSc->m_Content,m_ParmStr,pSc->m_nNum,0,0,0,0,TRUE);
  
 //   Player[nIndex].ExecuteScriptA(Player[nIndex].m_nGiveCallBackScript,pSc->m_SError,Player[nIndex].m_nGiveCallBackNum);
    return;	

	}
	else
	{
		Player[nIndex].ExecuteScriptB(Player[nIndex].m_nGiveCallBackScript,pSc->m_SError,0,"空");
		return;
	}

}

//给予界面回调函数
void KProtocolProcess::c2sGiveBack( int nIndex, BYTE* pProtocol)
{
	PLAYER_GIVE_CALLBACK* pGc = (PLAYER_GIVE_CALLBACK*)pProtocol;
	
	if (nIndex > 0 && nIndex < MAX_PLAYER)
	{
		int i = 0,nIdx[24];

		for (int nNum = 0; nNum <24;nNum++)
		{//执行了删除物品
			if (!pGc->m_uId[nNum]) 
				continue;

			nIdx[i] = Player[nIndex].m_ItemList.SearchID(pGc->m_uId[nNum]);
			//Player[nIndex].m_ItemList.setLock(pGc->m_uId[nNum],false,true);
			if (nIdx[i])
			   Player[nIndex].m_ItemList.Remove(nIdx[i]);   //移除物品  没有删除列表
			i++;  //有多少个物品
		}   

		//开始处理 带参数的回调函数
		char m_CurCallBack[64]={0};
		char m_ParmStr[64]={0};
		//char *m_NewCallStr=strstr(pGc->m_Callback, "#");  //如果有这个符号 stristr() 对大小写不敏感  取后面部分（包括符号）
		//
		//if (m_NewCallStr)
		//{//如果存在，就分割这字符串
		//	g_ExtractChar(pGc->m_Callback,'#','#',m_CurCallBack,m_ParmStr,2); //提取这个字符后面的内容
		//	//printf("---分割字符串:m_CurCallBack:%s m_ParmStr:%s---\n",m_CurCallBack,m_ParmStr);
		//}
		//else
		//	sprintf(m_CurCallBack,pGc->m_Callback);
		int  nSrrLen = strlen(pGc->m_Callback);
		std::string _curSirng = pGc->m_Callback;
		size_t pos_f=std::string::npos;
		pos_f = _curSirng.find("#",0);
		if  (pos_f != std::string::npos) 
		{//找到了
			std::string nFirstContent = _curSirng.substr(pos_f,nSrrLen-pos_f);     //后面的内容
			if (!nFirstContent.empty())
				snprintf(m_CurCallBack, sizeof(m_CurCallBack), "%s", nFirstContent.c_str());
		}
		else
			snprintf(m_CurCallBack, sizeof(m_CurCallBack), "%s", pGc->m_Callback);
		//-------------------------
		if (pGc->nKind == 0)   //取消按钮执行的命令
		{
			for (int j = 0;j < i;j++)
			{
			   int	x, y;
			   if (nIdx[j] > 0 && nIdx[j] < MAX_ITEM)
				 if (Player[nIndex].m_ItemList.CheckCanPlaceInEquipment(Item[nIdx[j]].GetWidth(), Item[nIdx[j]].GetHeight(),&x,&y))
					Player[nIndex].m_ItemList.Add(nIdx[j],pos_equiproom,x,y,0,0,Item[nIdx[j]].GetStackNum(),TRUE);
				 else //包袱空间不足 就删除东西
					 if (nIdx[j])
					 {
						ItemSet.Remove(nIdx[j]);
						nIdx[j] = 0;
					 }
			}
			/*for (int nNum = 0; nNum <24;nNum++)
			{
			if (!pGc->m_uId[nNum]) 
			continue;
			Player[nIndex].m_ItemList.setLock(pGc->m_uId[nNum],false,true);
			} */ 

			if  (strstr(pGc->m_Callback,"sysqianghua"))
                Player[nIndex].ExecuteScriptB(g_FileName2Id("\\script\\system\\qianghua.lua"),pGc->m_Error,i,NULL);
			else
                Player[nIndex].ExecuteScriptB(Player[nIndex].m_nGiveCallBackScript,pGc->m_Error,i,NULL);//"NO"  Player[nIndex].m_nGiveCallBackNum
//			Player[nIndex].ExecuteScript(Player[nIndex].m_nGiveCallBackScript,"GiveError",NULL);
			//Player[nIndex].ExeNoBackScript
		}
		else if (pGc->nKind == 1)  
		{   //正常执行
			//执行脚本类型 带参数的回调错误信息的并且 没有删除物品的 需要脚本删除物品
			int j;
			for (j = 0;j < i;j++)
			{//移动到包袱 
				int	x, y;
				if (nIdx[j] > 0 && nIdx[j] < MAX_ITEM)
				  if (Player[nIndex].m_ItemList.CheckCanPlaceInEquipment(Item[nIdx[j]].GetWidth(), Item[nIdx[j]].GetHeight(),&x,&y))
					 Player[nIndex].m_ItemList.Add(nIdx[j],pos_equiproom,x,y,0,0,Item[nIdx[j]].GetStackNum(),TRUE);
				  else
		             if (nIdx[j])
					 {
						 ItemSet.Remove(nIdx[j]);
						 nIdx[j] = 0;
					 }
			}

			if (Player[nIndex].m_nGiveCallBackScript && Player[nIndex].m_nGiveCallBackNum)
			{
				if (i > Player[nIndex].m_nGiveCallBackNum || i<=0)  //数量相等
				{//错误信息
					/*for (int nNum = 0; nNum <24;nNum++)
					{
						if (!pGc->m_uId[nNum]) 
							continue;
						Player[nIndex].m_ItemList.setLock(pGc->m_uId[nNum],false,true);
					} */
				    Player[nIndex].ExecuteScriptB(Player[nIndex].m_nGiveCallBackScript,pGc->m_Error,i,NULL);
				}
				else
				{
					char sParam[256] ={0};
					ZeroMemory(sParam,sizeof(sParam));
					strcpy(sParam,"");
					for (j = 0 ; j < i;j++)
					{
						char stmp[32];
						snprintf(stmp, 32, "%d",nIdx[j]);
						strcat(sParam,stmp);
						if (j != i - 1)
							strcat(sParam,",");
					}
					Player[nIndex].ExecuteScriptD(Player[nIndex].m_nGiveCallBackScript,m_CurCallBack,sParam,NULL,i,0,0,0,0,TRUE);
				}
			}
		}
		else if (pGc->nKind == 2)
		{//执行错误函数
			if (Player[nIndex].m_nGiveCallBackScript && Player[nIndex].m_nGiveCallBackNum)
			{
				if (i > Player[nIndex].m_nGiveCallBackNum || i<=0)
				{
					/*for (int nNum = 0; nNum <24;nNum++)
					{
						if (!pGc->m_uId[nNum]) 
							continue;
						Player[nIndex].m_ItemList.setLock(pGc->m_uId[nNum],false,true);
					}*/
					Player[nIndex].ExecuteScriptB(Player[nIndex].m_nGiveCallBackScript,pGc->m_Error,i,NULL);
				}
				else
				{
					char sParam[256];
					ZeroMemory(sParam,sizeof(sParam));
					strcpy(sParam,"");
					for (int j = 0 ; j < i;j++)
					{
						char stmp[32];
						snprintf(stmp, 32, "%d",nIdx[j]);
						strcat(sParam,stmp);
						if (j != i - 1)
							strcat(sParam,",");
					}
					Player[nIndex].ExecuteScriptD(Player[nIndex].m_nGiveCallBackScript,m_CurCallBack,sParam,NULL,i,0,0,0,0,TRUE);			
				}
			}																													
		}
		else if (pGc->nKind == 3)
		{ //强化
			int j;
			for (j = 0; j < i; j++)
			{
				int	x, y;
				if (nIdx[j] > 0 && nIdx[j] < MAX_ITEM)
					if (Player[nIndex].m_ItemList.CheckCanPlaceInEquipment(Item[nIdx[j]].GetWidth(), Item[nIdx[j]].GetHeight(), &x, &y))
						Player[nIndex].m_ItemList.Add(nIdx[j], pos_equiproom, x, y,0,0,Item[nIdx[j]].GetStackNum(),TRUE);
					else
						if (nIdx[j])
						{
							ItemSet.Remove(nIdx[j]);
							nIdx[j] = 0;
						}
			}
			/*for (int nNum = 0; nNum <24;nNum++)
			{
			if (!pGc->m_uId[nNum]) 
			continue;
			Player[nIndex].m_ItemList.setLock(pGc->m_uId[nNum],false,true);
			}*/

			if (i > 24 || i<=0)   //数量限制
			{
				Player[nIndex].ExecuteScriptB(g_FileName2Id("\\script\\system\\qianghua.lua"),pGc->m_Error,i,NULL);
			}else
			{
				char sParam[256];
				ZeroMemory(sParam,sizeof(sParam));
				strcpy(sParam,"");
				for (j = 0 ; j < i;j++)
				{
					char stmp[32];
					snprintf(stmp, 32, "%d",nIdx[j]);
					strcat(sParam,stmp);
					if (j != i - 1)
						strcat(sParam,",");
				}
				Player[nIndex].ExecuteScriptC("\\script\\system\\qianghua.lua",m_CurCallBack,sParam,NULL,i);	
			}
		}
		else if (pGc->nKind == 4)
		{ //寄售
			int j;
			for (j = 0; j < i; j++)
			{
				int	x, y;
				if (nIdx[j] > 0 && nIdx[j] < MAX_ITEM)
					if (Player[nIndex].m_ItemList.CheckCanPlaceInEquipment(Item[nIdx[j]].GetWidth(), Item[nIdx[j]].GetHeight(), &x, &y))
						Player[nIndex].m_ItemList.Add(nIdx[j], pos_equiproom, x, y,0,0,Item[nIdx[j]].GetStackNum(),TRUE);
					else
						if (nIdx[j])
						{
							ItemSet.Remove(nIdx[j]);
							nIdx[j] = 0;
						}
			}
			if (i > 32 || i <= 0)   //数量限制
				Player[nIndex].ExecuteScriptB(g_FileName2Id("\\script\\system\\jishou.lua"), pGc->m_Error, i, NULL);
			else
			{
				char sParam[256];
				ZeroMemory(sParam, sizeof(sParam));
				strcpy(sParam, "");
				for (j = 0; j < i; j++)
				{
					char stmp[32];
					snprintf(stmp, 32, "%d", nIdx[j]);
					strcat(sParam, stmp);
					if (j != i - 1)
						strcat(sParam, ",");
				}//
				Player[nIndex].ExecuteScriptC("\\script\\system\\jishou.lua", m_CurCallBack, sParam, m_ParmStr, pGc->nPrice, i);
			}
		}

		Player[nIndex].m_ItemList.ClearRoom(room_give);
	}
}

void KProtocolProcess::NpcPKValueCommand(int nIndex, BYTE* pProtocol)
{
	CP_PKVALUE 		*pPKValueCmd;

	pPKValueCmd = (CP_PKVALUE *)pProtocol;
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;

	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;

	Player[nIndex].pkvalue(pPKValueCmd->nbutton);
}

void KProtocolProcess::c2sGopItem(int nIndex, BYTE* pProtocol)
{
	if (Player[nIndex].CheckTrading())
		return;

	Player[nIndex].GopItemPlayer(pProtocol);
}

void KProtocolProcess::PlayerCommand(int nIndex, BYTE* pProtocol)
{
	PLAYER_COMMAND	*pCommand = (PLAYER_COMMAND*)pProtocol;
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;

	switch (pCommand->m_wMsgID)
	{
	case enumC2S_PLAYERCOMMAND_ID_LOCKITEM:
		{	/*
			int nIdx = Player[nIndex].m_ItemList.SearchID(LOWORD(pCommand->m_lpBuf));
			if (nIdx < 0)
				return;
			Player[nIndex].m_ItemList.LockItemBind(nIdx, HIWORD(pCommand->m_lpBuf) > 0);*/

			int nIdx = Player[nIndex].m_ItemList.SearchID(LOWORD(pCommand->m_lpBuf));
			if (nIdx < 0)
				return;
			Player[nIndex].ExecuteScript2("\\script\\System\\player\\lockitem.lua", "main", nIdx, HIWORD(pCommand->m_lpBuf));

		}
		break;
	case enumC2S_PLAYERCOMMAND_ID_OFFLINE:
		{
			//int nIdx = Player[nIndex].m_ItemList.SearchID(LOWORD(pCommand->m_lpBuf));
			//if (nIdx < 0)
			//	return;
			//Player[nIndex].ExecuteScript2("\\script\\System\\lockItem.lua", "main", nIdx, HIWORD(pCommand->m_lpBuf));
		}
		break;
/*	case enumC2S_PLAYERCOMMAND_ID_SUPERSHOP:
		Player[nIndex].ExecuteScript(SCRIPT_PROTOCOL_FILE, "PermitSuperShop", 0);
		break;
	case enumC2S_PLAYERCOMMAND_ID_OFFLINE:
		Player[nIndex].ExecuteScript(LOGOUT_SCRIPT, NORMAL_FUNCTION_NAME, 0);
		break;*/
	default:
		break;
	}
}

#endif
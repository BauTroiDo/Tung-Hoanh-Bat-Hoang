#ifndef KProtocolProcessH
#define	KProtocolProcessH

#include "KProtocol.h"
class KProtocolProcess
{
private:
#ifndef _SERVER
	void (KProtocolProcess::*ProcessFunc[s2c_end])(BYTE* pMsg);
#else
	void (KProtocolProcess::*ProcessFunc[c2s_end])(int nIndex, BYTE* pMsg);
#endif
public:
	KProtocolProcess();
	~KProtocolProcess();
#ifndef _SERVER
	void ProcessNetMsg(BYTE* pMsg);
#else
	void ProcessNetMsg(int nIndex, BYTE* pMsg);
#endif
private:
#ifdef TOOLVERSION
public:
#endif
#ifndef _SERVER
	void	s2cAccoutLoginResult(BYTE * pMsg);
	void	SyncCurNormalData(BYTE* pMsg);
	void	SyncWorld(BYTE* pMsg);
	void	SyncNpc(BYTE* pMsg);
	void	SyncNpcMin(BYTE* pMsg);
	void	SyncNpcMinPlayer(BYTE* pMsg);
	void	SyncPlayer(BYTE* pMsg);
	void	SyncPlayerMin(BYTE* pMsg);
	void	SyncCurPlayer(BYTE* pMsg);
	void	SyncObjectAdd(BYTE* pMsg);
	void	SyncObjectState(BYTE* pMsg);
	void	SyncObjectDir(BYTE* pMsg);
	void	SyncObjectRemove(BYTE* pMsg);
	void	SyncObjectTrap(BYTE* pMsg);
	void	NetCommandWalk(BYTE* pMsg);
	void	NetCommandRun(BYTE* pMsg);
	void	NetCommandJump(BYTE* pMsg);
	void	NetCommandChat(BYTE* pMsg);
	void	NetCommandSkill(BYTE* pMsg);
	void	NetCommandHurt(BYTE* pMsg);
	void	NetCommandDeath(BYTE* pMsg);
	void	NetCommandRemoveNpc(BYTE* pMsg);
	void	NetCommandChgCurCamp(BYTE* pMsg);
	void	NetCommandChgCamp(BYTE* pMsg);
	void	NetCommandSit(BYTE* pMsg);
	void	OpenSaleBox(BYTE* pMsg);
	void	OpenStoreBox(BYTE* pMsg);
	void	OpenResetPass(BYTE* pMsg);
	void	OpenStringUI(BYTE* pMsg); //stringbox
	void	GiftDT(BYTE* pMsg);
	void	s2cShowTeamInfo(BYTE* pMsg);
	void	s2cUpdataSelfTeamInfo(BYTE* pMsg);
	void	s2cApplyTeamInfoFalse(BYTE* pMsg);
	void	s2cCreateTeam(BYTE* pMsg);
	void	s2cApplyCreateTeamFalse(BYTE* pMsg);
	void	s2cSetTeamState(BYTE* pMsg);
	void	s2cApplyAddTeam(BYTE* pMsg);
	void	s2cTeamAddMember(BYTE* pMsg);
	void	s2cLeaveTeam(BYTE* pMsg);
	void	s2cTeamChangeCaptain(BYTE* pMsg);
	void	s2cSetFactionData(BYTE* pMsg);
	void	s2cLeaveFaction(BYTE* pMsg);
	void	s2cFactionSkillOpen(BYTE* pMsg);
	void	s2cGetChat(BYTE* pMsg);
	void	s2cGetLeadExp(BYTE* pMsg);
	void	s2cGetTeammateLevel(BYTE* pMsg);
	void	s2cLevelUp(BYTE* pMsg);
	void	s2cGetCurAttribute(BYTE* pMsg);
	void	s2cGetSkillLevel(BYTE* pMsg);
	void	s2cSyncAllSkill(BYTE * pMsg);
	void	s2cSyncMoney(BYTE* pMsg);
	void	s2cSyncXu(BYTE* pMsg);//xu
	void	s2cMoveItem(BYTE* pMsg);
	void	s2cRemoveItem(BYTE* pMsg);
	void	s2cSyncItem(BYTE* pMsg);
	void	SyncScriptAction(BYTE* pMsg);
	void	s2cChatGetApplyAddFriend(BYTE* pMsg);
	void	s2cSyncRoleList(BYTE* pMsg);
	void	s2cChatLoginFriendNoName(BYTE* pMsg);
	void	s2cChatLoginFriendName(BYTE* pMsg);
	void	s2cChatAddOneFriend(BYTE* pMsg);
	void	s2cChatBeRefusedAddFriend(BYTE* pMsg);
	void	s2cChatAddFriend(BYTE* pMsg);
	void	s2cChatAddFriendFail(BYTE* pMsg);
	void	s2cChatFriendOnline(BYTE* pMsg);
	void	s2cChatDeleteFriend(BYTE* pMsg);
	void	s2cChatFriendOffLine(BYTE* pMsg);
	void	SyncClientEnd(BYTE* pMsg);
	void	s2cTradeChangeState(BYTE* pMsg);
	void	s2cNpcSetMenuState(BYTE* pMsg);
	void	s2cTradeMoneySync(BYTE* pMsg);
	void	s2cTradeDecision(BYTE* pMsg);
	void	s2cPlayerExp(BYTE* pMsg);
	void	s2cChatScreenSingleError(BYTE* pMsg);
	void	s2cUpdateNpcState(BYTE * pMsg);
	void	s2cTeamInviteAdd(BYTE * pMsg);
	void	s2cTradePressOkSync(BYTE * pMsg);
	void	s2cPing(BYTE* pMsg);
	void	s2cDirectlyCastSkill(BYTE * pMsg);
	void	s2cShowMsg(BYTE * pMsg);
	void	SyncStateEffect(BYTE* pMsg);
	void	PlayerRevive(BYTE* pMsg);
	void	RequestNpcFail(BYTE* pMsg);
	void	s2cTradeApplyStart(BYTE* pMsg);
	void	s2cItemAutoMove(BYTE* pMsg);
	void	FinishedItemExchange(BYTE* pMsg);
	void	s2cChangeWeather(BYTE* pMsg);
	void	s2cPKSyncNormalFlag(BYTE* pMsg);
	void	s2cPKSyncEnmityState(BYTE* pMsg);
	void	s2cPKSyncExerciseState(BYTE* pMsg);
	void	s2cPKValueSync(BYTE* pMsg);
	void	s2cReputeValueSync(BYTE* pMsg);
	void	s2cFuYuanValueSync(BYTE* pMsg);
	void	s2cReBornValueSync(BYTE* pMsg);
	void	OpenMarketBox(BYTE* pMsg);
	void	NpcSleepSync(BYTE* pMsg);
	void	s2cViewEquip(BYTE* pMsg);
	void	LadderResult(BYTE* pMsg);
	void	LadderList(BYTE* pMsg);
	void	s2cTongCreate(BYTE* pMsg);
	void	ServerReplyClientPing(BYTE* pMsg);
	void	s2cNpcGoldChange(BYTE* pMsg);
	void	ItemChangeDurability(BYTE* pMsg);
	void	OpenTremble(BYTE* pMsg);
	void	NetCommandSetRankFF(BYTE* pMsg);
	void	s2cViewSellItem(BYTE* pMsg);
	void	s2cViewUpdateItem(BYTE* pMsg);
	void	s2cGetCouunt(BYTE* pMsg);
	void	s2cShopName(BYTE* pMsg);  // 摆摊广告
//	void	SyncInfo(BYTE* pMsg);
	void	SyncMaskLock(BYTE* pMsg);	// mat na
	void	SyncGive(BYTE* pMsg);
	void	SyncSecMov(BYTE* pMsg);
	void	SyncPKValue(BYTE* pMsg);
	void	SyncViewlianitem(BYTE* pMsg);  //链接
//	void	s2cLianUpdateItem(BYTE* pMsg); //链接更新
//	void	s2cGetLianCouunt(BYTE* pMsg);
	void	OpenWenGang(BYTE* pMsg); 
	void	Openstone(BYTE* pMsg); 
	void	s2cViewUpdateEquip(BYTE* pMsg); 
	void	s2cgettongback(BYTE* pMsg);
	void	s2csyncequipitem(BYTE* pMsg);
	void	s2ctaskcallback(BYTE* pMsg);
	void	s2copenjindutiao(BYTE* pMsg);  //打开进度条
	void	s2csyncitemposition(BYTE* pMsg); 
	void	s2copenduanzhao(BYTE* pMsg);
	void	s2csyncurupdata(BYTE* pMsg);
	void	s2cJinMaicallback(BYTE* pMsg);
	void	s2copenvipdazao(BYTE* pMsg);
	void	s2csynnpcinfocallback(BYTE* pMsg);
	void	s2cpautoMovecallback(BYTE* pMsg);
	void	s2cplyerguzicallback(BYTE* pMsg);
	void	s2csyncplayermap(BYTE* pMsg);
	void	s2csynconeclearstate(BYTE* pMsg);
	void	s2csyncnodataeffect(BYTE* pMsg);
	void	s2cnpcsetpos(BYTE* pMsg);
	void	s2cplayermissiondata(BYTE* pMsg);
	void	s2csyncrankdata(BYTE* pMsg);
	void	s2cSyncSuperShop(BYTE* pMsg);
	void	s2csyncitemshop(BYTE* pMsg);
	void	s2citemdatashop(BYTE* pMsg);
	void	s2ckickoutgame(BYTE* pMsg);
	void	s2cobjadd_vn(BYTE* pMsg);
	void	s2crolesyncvipvalue(BYTE* pMsg);
	
		
	void	s2cExtend(BYTE* pMsg);
	void	s2cExtendChat(BYTE* pMsg);
	void	s2cExtendFriend(BYTE* pMsg);
	void	s2cExtendTong(BYTE* pMsg);
#else
	void	RemoveRole(int nIndex, BYTE * pProtocol);
	void	NpcRequestCommand(int nIndex, BYTE* pMsg);
	void	ObjRequestCommand(int nIndex, BYTE* pProtocol);
	void	NpcWalkCommand(int nIndex, BYTE* pProtocol);
	void	NpcRunCommand(int nIndex, BYTE* pProtocol);
	void	NpcSkillCommand(int nIndex, BYTE* pProtocol);
	void	NpcJumpCommand(int nIndex, BYTE* pProtocol);
	void	NpcTalkCommand(int nIndex, BYTE* pProtocol);
	void	PlayerTalkCommand(int nIndex, BYTE* pProtocol);
	void	PlayerApplyTeamInfo(int nIndex, BYTE* pProtocol);
	void	PlayerApplyCreateTeam(int nIndex, BYTE* pProtocol);
	void	PlayerApplyTeamOpenClose(int nIndex, BYTE* pProtocol);
	void	PlayerApplyAddTeam(int nIndex, BYTE* pProtocol);
	void	PlayerAcceptTeamMember(int nIndex, BYTE* pProtocol);
	void	PlayerApplyLeaveTeam(int nIndex, BYTE* pProtocol);
	void	PlayerApplyTeamKickMember(int nIndex, BYTE* pProtocol);
	void	PlayerApplyTeamChangeCaptain(int nIndex, BYTE* pProtocol);
	void	PlayerApplyTeamDismiss(int nIndex, BYTE* pProtocol);
	void	PlayerApplySetPK(int nIndex, BYTE* pProtocol);
	void	PlayerApplyFactionData(int nIndex, BYTE* pProtocol);
	void	PlayerSendChat(int nIndex, BYTE* pProtocol);
	void	PlayerAddBaseAttribute(int nIndex, BYTE* pProtocol);
	void	PlayerApplyAddSkillPoint(int nIndex, BYTE* pProtocol);
	void	PlayerEatItem(int nIndex, BYTE* pProtocol);
	void	PlayerPickUpItem(int nIndex, BYTE* pProtocol);
	void	PlayerMoveItem(int nIndex, BYTE* pProtocol);
	void	PlayerSellItem(int nIndex, BYTE* pProtocol);
	void	PlayerBuyItem(int nIndex, BYTE* pProtocol);
	void	PlayerDropItem(int nIndex, BYTE* pProtocol);
	void	PlayerSelUI(int nIndex, BYTE* pProtocol);
	void	ChatSetChannel(int nIndex, BYTE* pProtocol);
	void	ChatApplyAddFriend(int nIndex, BYTE* pProtocol);
	void	ChatAddFriend(int nIndex, BYTE* pProtocol);
	void	ChatRefuseFriend(int nIndex, BYTE* pProtocol);
	void	ChatApplyReSendAllFriendName(int nIndex, BYTE* pProtocol);
	void	ChatApplySendOneFriendName(int nIndex, BYTE* pProtocol);
	void	ChatDeleteFriend(int nIndex, BYTE* pProtocol);
	void	ChatReDeleteFriend(int nIndex, BYTE* pProtocol);
	void	TradeApplyOpen(int nIndex, BYTE* pProtocol);
	void	TradeApplyClose(int nIndex, BYTE* pProtocol);
	void	TradeApplyStart(int nIndex, BYTE* pProtocol);
	void	TradeMoveMoney(int nIndex, BYTE* pProtocol);
	void	TradeDecision(int nIndex, BYTE* pProtocol);
	void	DialogNpc(int nIndex, BYTE * pProtocol);
	void	TeamInviteAdd(int nIndex, BYTE * pProtocol);
	void	ChangeAuraSkill(int nIndex, BYTE * pProtocol);
	void	TeamReplyInvite(int nIndex, BYTE * pProtocol);
	//void	ReplyPing(int nIndex, BYTE* pProtocol);
	void	NpcCPUnlockCommand(int nIndex, BYTE* pProtocol);
	void	NpcCPLockCommand(int nIndex, BYTE* pProtocol);
	void	NpcCPChangeCommand(int nIndex, BYTE* pProtocol);
	void	NpcCPResetCommand(int nIndex, BYTE* pProtocol);
	void	NpcOpenMarketCommand(int nIndex, BYTE* pProtocol); //KTC
	void	NpcStringCommand(int nIndex, BYTE* pProtocol);//字符串输入界面
	void	NpcAutoplayCommand(int nIndex, BYTE* pProtocol);//挂机
	void	NpcDaTauCommand(int nIndex, BYTE* pProtocol);
	void	NpcSitCommand(int nIndex, BYTE* pProtocol);
	void	ObjMouseClick(int nIndex, BYTE* pProtocol);
	void	StoreMoneyCommand(int nIndex, BYTE* pProtocol);
	void	NpcReviveCommand(int nIndex, BYTE* pProtocol);
	void	c2sTradeReplyStart(int nIndex, BYTE* pProtocol);
	void	c2sPKApplyChangeNormalFlag(int nIndex, BYTE* pProtocol);
	void	c2sPKApplyEnmity(int nIndex, BYTE* pProtocol);
	void	c2sViewEquip(int nIndex, BYTE* pProtocol);    //查看别人装备
	void	LadderQuery(int nIndex, BYTE* pProtocol);
	void	ItemRepair(int nIndex, BYTE* pProtocol);
	void	NpcRideCommand(int nIndex, BYTE* pProtocol);
	void	ItemBreak(int nIndex, BYTE* pProtocol);
	void	c2sCompItem(int nIndex, BYTE* pProtocol);
	void	SetPrice(int nIndex, BYTE* pProtocol);
	void	StartTrade(int nIndex, BYTE* pProtocol);
	void	c2sViewItem(int nIndex, BYTE* pProtocol);   //查看摆摊物品
	void	c2sTradeBuy(int nIndex, BYTE* pProtocol);
	void	c2supdateitem(int nIndex, BYTE* pProtocol);
	void	c2sNeedCount(int nIndex, BYTE* pProtocol);
	void	c2sNeedShopName(int nIndex, BYTE* pProtocol);
	void	c2sSysShop(int nIndex, BYTE* pProtocol);
	void	c2sLiXian(int nIndex, BYTE* pProtocol);
	void	c2sGiveBack(int nIndex, BYTE* pProtocol);
	void	NpcPKValueCommand(int nIndex, BYTE* pProtocol);
	void	NpcSunyiCommand(int nIndex, BYTE* pProtocol);//瞬移	
	void	c2sguzi(int nIndex, BYTE* pProtocol);//股子
	void	c2sgufarm(int nIndex, BYTE* pProtocol);//股子
	void    c2saddjinbi(int nIndex, BYTE* pProtocol);//增加金币
	void    c2slianjie(int nIndex, BYTE* pProtocol);//链接
	void    c2sfsjingmai(int nIndex, BYTE* pProtocol);//经脉
	void    c2sguaforbit(int nIndex, BYTE* pProtocol);//封挂
	void    c2sskillsync(int nIndex, BYTE* pProtocol);
	void    c2sRongItem(int nIndex, BYTE* pProtocol);
	void    c2splayeradditem(int nIndex, BYTE* pProtocol);
	void    c2splayerAutoMoveitem(int nIndex, BYTE* pProtocol);
	void    c2splayerdelitem(int nIndex, BYTE* pProtocol);
	void    c2splayersetLianFlg(int nIndex, BYTE* pProtocol);
	void    c2supdateviewequip(int nIndex, BYTE* pProtocol);
	void    c2splayertongset(int nIndex, BYTE* pProtocol);
	void    c2splayerdoScript(int nIndex, BYTE* pProtocol);
	void    c2sgettaskval(int nIndex, BYTE* pProtocol);	
	void    c2sjinducallback(int nIndex, BYTE* pProtocol);
	void    c2ssyncurdata(int nIndex, BYTE* pProtocol);	
	void    c2svipdoScript(int nIndex, BYTE* pProtocol);
	void    c2sroomisfull(int nIndex, BYTE* pProtocol);
	void    c2ssynnpcinfo(int nIndex, BYTE* pProtocol);
	void    c2splayerautoequip(int nIndex, BYTE* pProtocol);
	void    c2splayerguzgame(int nIndex, BYTE* pProtocol);
	void    c2splayersetpos(int nIndex, BYTE* pProtocol);
	void    c2ssynshopidx(int nIndex, BYTE* pProtocol);
	void    c2ssettaskval(int nIndex, BYTE* pProtocol);
	void    c2srunscript(int nIndex, BYTE* pProtocol);
	void    c2splayerreq(int nIndex, BYTE* pProtocol);
	void    c2slockitem(int nIndex, BYTE* pProtocol);
	void    GMCommand(int nIndex, BYTE* pProtocol);
	void    c2sfindpathstate(int nIndex, BYTE* pProtocol);
	void	c2screaterefitem(int nIndex, BYTE* pProtocol);
	void	c2sremoverefitem(int nIndex, BYTE* pProtocol);
	void	c2sapplymasklock(int nIndex, BYTE* pProtocol);
	void	c2sGopItem(int nIndex, BYTE* pProtocol);
	void	PlayerCommand(int nIndex, BYTE* pProtocol);
		
#endif
};

extern KProtocolProcess g_ProtocolProcess;
#endif

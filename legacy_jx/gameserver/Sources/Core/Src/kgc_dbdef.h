/* -------------------------------------------------------------------------
//	文件名		：	kgc_dbdef.h
//	创建者		：	luobaohang
//	创建时间	：	2007-2-28 9:52:57
//	功能描述	：	
//
// -----------------------------------------------------------------------*/
#ifndef __KGC_DBDEF_H__
#define __KGC_DBDEF_H__
// -------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////注意所有的表名字段名等均不得超过64个字节/////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
#define KD_DBNAME_MAX_SIZE	64	//表名和字段名的最大长度
#define	KD_DATA_GROUP "KG_JX_EX"	//当前组（固定Field）
#define	KD_DBFIELD_GROUP "GroupName"

//因为要用于存储，模块名在此文件定义，保证不冲突

//**********************************共享数据表的定义**********************************//
#define KD_DBTBL_SHARE				"tblShare"
#define KD_DBFIELD_DATAVER			"DataVer"
//**********************************玩家数据表的定义*******************************//
#define KD_DBTBL_PLAYEDATA			"tblPlayerData"
#define KD_DBFIELD_GUID				"Guid"
#define KD_DBFIELD_ROLENAME			"RoleName"
#define KD_DBFIELD_ACCOUNTNAME		"AccountName"
#define KD_DBFIELD_BASEDATA			"BaseData"
#define KD_DBFIELD_PLAYERTASK		"PlayerTask"
//**********************************玩家关系数据表的定义*******************************//
#define KD_DBTBL_PLAYERRELAY		"tblPlayerRelay"
#define KD_DBFIELD_GUID_PRIMARY		"Guid"
#define KD_DBFIELD_RELAY_TYPE		"RelayType"
#define KD_DBFIELD_RELAY_DATA		"RelayData"
//**********************************玩家家族数据表定义*******************************//
#define KD_DBTBL_PLAYERKIN			"tblPlayerKin"
#define KD_DBFIELD_KINID			"KinID"
#define KD_DBFIELD_KIN_MEMBER		"KinMember"
//**********************************排行榜表的定义**********************************//
#define KD_DBTBL_LADDER				"tblLadder"
#define KD_DBTBL_PLAYERLADDER		"tblPlayerLadder"
#define KD_DBTBL_ALLTOTALLADDER		"tblAllTotalLadder"
#define KD_DBTBL_TOTALLADDER		"tblTotalLadder"
//**********************************全局Buffer的定义**********************************//
#define KD_DBTBL_GBLBUFSET			"tblGblBufSet"
#define KD_DBTBL_GBLBUF				"tblGblBuf"
//**********************************非自动Buffer的定义**********************************//
#define KD_DBTBL_SAVEBUF			"tblSaveBuf"
#define KD_DBFIELD_SAVEBUF_KEY		"BufKey"
#define KD_DBFIELD_SAVEBUF_DATA		"BufData"
//**********************************通用数据表的定义**********************************//
////////通用数据表表名
//定义数据库表，即使不用存储的表，表名也在此定义，以便统一或以后把表改成要存储
#define KD_DBTBL_ROOT				"tblRoot"	//根表名定义
#define KD_DBTBL_PLAYER				"tblPlayer"
#define KD_DBTBL_GUID				"tblGuid"
#define KD_DBTBL_PLAYER_INT_TASK	"tblPlayerIntTask"
#define KD_DBTBL_PLAYER_BUF_TASK	"tblPlayerBufTask"
#define KD_DBTBL_PLAYER_LOCAL_TASK	"tblPlayerLocalTask"
#define KD_DBTBL_NAMEID2PLAYERID	"tblNameID2PlayerID"
#define KD_DBTBL_TONG				"tblTong"
#define KD_DBTBL_TONG_DOMAIN			"tblTongDomain"
#define KD_DBTBL_TONG_DOMAIN_TASK			"tblTongDomainTask"
#define KD_DBTBL_TONG_DOMAIN_TASK_BUF			"tblTongDomainTaskBuf"
#define KD_DBTBL_TONG_TASK			"tblTongTask"
#define KD_DBTBL_TONG_BUF_TASK		"tblTongBufTask"
#define KD_DBTBL_TONG_TMPTASK		"tblTongTmpTask"
#define KD_DBTBL_TONG_KIN			"tblTongKin"
#define KD_DBTBL_KIN				"tblKin"
#define KD_DBTBL_KIN_TASK			"tblKinTask"
#define KD_DBTBL_KIN_BUF_TASK		"tblKinBufTask"
#define KD_DBTBL_KIN_TMPTASK		"tblKinTmpTask"
#define KD_DBTBL_KIN_MEMBER			"tblKinMember"
#define KD_DBTBL_KIN_MEMBER_TASK	"tblKinMemberTask"
#define KD_DBTBL_KIN_MEMBER_TMPTASK	"tblKinMemberTmpTask"
#define KD_DBTBL_UNION				"tblUnion"
#define KD_DBTBL_UNION_TASK			"tblUnionTask"
#define KD_DBTBL_UNION_BUF_TASK		"tblUnionBufTask"
#define KD_DBTBL_UNION_TMPTASK		"tblUnionTmpTask"
#define KD_DBTBL_UNION_TONG			"tblUnionTong"
#define KD_DBTBL_UNION_DOMAIN		"tblUnionDomain"
#define KD_DBTBL_TEAM				"tblTeam"
#define KD_DBTBL_TEAM_TASK			"tblTeamTask"
#define KD_DBTBL_TEAM_MEMBER		"tblTeamMember"
#define KD_DBTBL_TEAM_MEMBER_TASK	"tblTeamMemberTask"
#define KD_DBTBL_TEAM_SEARCH		"tblTeamSearch"
#define KD_DBTBL_LEAGUE_SET			"tblLeagueSet"
#define KD_DBTBL_LEAGUE				"tblLeague"	
#define KD_DBTBL_MEMBERLEA			"tblMemberLea"
#define KD_DBTBL_LEAGUE_TASK		"tblLeagueTask"
#define KD_DBTBL_LEAGUE_MEMBER		"tblLeagueMember"
#define KD_DBTBL_LEAMEMBER_TASK		"tblLeaMemberTask"
#define KD_DBTBL_GBLVALUE_SET		"tblGblValueSet"
#define KD_DBTBL_GBLV_INI_INT		"tblGblVIntInt"
#define KD_DBTBL_GBLV_INI_BUF		"tblGblVIntBuf"
#define KD_DBTBL_GBLV_STR_INT		"tblGblVStrInt"
#define KD_DBTBL_GBLV_STR_BUF		"tblGblVStrBuf"
#define KD_DBTBL_JBEXCHANGE_BILL	"tblJbExchangeBill"
#define KG_DBTBL_JBEXCHANGE_TASK	"tblJbExchangeTask"
#define KD_DBTBL_SPORT				"tblSport"
#define KD_DBTBL_SPORT_TASK			"tblSportTask"
#define KD_DBTBL_SPORT_BUf_TASK		"tblSportBufTask"
#define KD_DBTBL_SPORT_TMP_TASK		"tblSportTempTask"
#define KD_DBTBL_SPORT_DATA			"tblSportData"
#define KD_DBTAL_SPORT_DATA_TASK	"tblSportDataTask"
#define	KD_DBTAL_SPORT_DATA_TEMP	"tblSportDataTempTask"
#define KD_DBTAL_SPORT_PLAYER		"tblSportPlayer"
#define KD_DBTAL_SPORT_PLAYER_DATAID	"tblSportPlayerDataId"
#define KD_DBTAL_SPORT_PLAYER_TEMP	"tblSportPlayerTemp"
//--------------------------------------------------------------
#define KD_DBTBL_PROFILE_PROFILE	"tblProfile"
#define KD_DBTBL_PROFILE_TASK		"tblProfileTask"
#define KG_DBTBL_PROFILE_BUFF		"tblProfileBuff"
//--------------------------------------------------------------
#define KG_DBTBL_AUCTION			"tblAuction"
///////通用数据表字段
#define KD_DBFIELD_TABID	"Tabid"
#define KD_DBFIELD_ITEMKEY	"Itemkey"
#define KD_ITEMKEY_MAX_SIZE	64	//Key的最大长度64字节
#define KD_DBFIELD_DATA		"Data"

// -------------------------------------------------------------------------
#endif /* __KGC_DBDEF_H__ */

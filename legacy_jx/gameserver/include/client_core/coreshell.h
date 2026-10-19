#ifndef __GAMEWORLD_CORESHELL_H__
#define __GAMEWORLD_CORESHELL_H__
#ifndef _SERVER
// -------------------------------------------------------------------------

enum KE_COREEVENT
{
    emCOREEVENT_SYNC_SERIES						= 0,  			// 五行
    emCOREEVENT_SYNC_LEVEL						= 1,         	// 等级
    emCOREEVENT_SYNC_FACTION					= 2,       		// 门派
    emCOREEVENT_SYNC_PKVALUE					= 3,       		// PK值
    emCOREEVENT_SYNC_EXP						= 4,           	// 经验
    emCOREEVENT_SYNC_LIFE						= 5,          	// 生命值
    emCOREEVENT_SYNC_MANA						= 6,          	// 内力值
    emCOREEVENT_SYNC_STAMINA					= 7,		 	// 体力值
    emCOREEVENT_SYNC_GTP						= 8,           	// 活力值
    emCOREEVENT_SYNC_MKP						= 9,           	// 精力值
    emCOREEVENT_SYNC_RESIST						= 10,        	// 抗性
    emCOREEVENT_SYNC_DAMAGE						= 11,        	// 攻击力
    emCOREEVENT_SYNC_ATTACKRATE					= 12,    		// 攻击距离
    emCOREEVENT_SYNC_ATTACKSPEED				= 13,   		// 攻击速度
    emCOREEVENT_SYNC_DEFENSE					= 14,       	// 防御
    emCOREEVENT_SYNC_RUNSPEED					= 15,      		// 移动速度
    emCOREEVENT_SYNC_POTENTIAL					= 16,     		// 角色属性
    emCOREEVENT_SYNC_REMAIN						= 17,        	// 剩余属性点
    emCOREEVENT_SYNC_REPUTE						= 18,        	// 声望
    emCOREEVENT_SYNC_TITLE						= 19,         	// 称号
    emCOREEVENT_SYNC_PORTRAIT					= 20,      		// 头像索引
    emCOREEVENT_SYNC_RUNSTATUS					= 21,     		// 移动状态（走，跑）
    emCOREEVENT_SYNC_RIDEHORSE					= 22,     		// 骑马状态
    emCOREEVENT_SYNC_DOING						= 23,         	// 打坐状态
    emCOREEVENT_SYNC_STATE						= 24,         	// 啥？
    emCOREEVENT_SYNC_LIFESKILL					= 25,     		// 啥？（生活技能？）
	emCOREEVENT_SYNC_RECIPE						= 26,        	// 配方
	emCOREEVENT_SYNC_ITEM						= 27,			// 道具
    emCOREEVENT_DORECIPE_SUCCESS				= 28,   		// 啥？
    emCOREEVENT_DORECIPE_FAIL					= 29,      		// 啥？

    emCOREEVENT_MAIL_LOADED						= 30,        	// 邮件加载完成
    emCOREEVENT_MAIL_SENDOK						= 31,        	// 邮件发送成功
    emCOREEVENT_MAIL_SENDFAIL					= 32,		 	// 邮件发送失败
    emCOREEVENT_MAIL_NEWMAIL					= 33,       	// 有新邮件

    emCOREEVENT_TALK							= 34,           // 和NPC谈话
    emCOREEVENT_PROGRESSBAR_TIMER				= 35,  			// 引导条
    emCOREEVENT_BUFF_CHANGE						= 36,        	// 技能状态变化

    emCOREEVENT_FIGHT_SKILL_CHANGED				= 37,  			// 左右键技能改变
    emCOREEVENT_FIGHT_SKILL_POINT				= 38,  			// 战斗技能点改变

    emCOREEVENT_SYNC_PK_MODEL					= 44,          	// 同步PK模式变化
	emCOREEVENT_PREPARE_ITEMREPAIR				= 45,			// 准备进行道具修理

	emCOREEVENT_TRADE_REQUEST					= 46,			// 接收到对方发出的交易申请
	emCOREEVENT_TRADE_RESPONSE					= 47,			// 对方回应交易请求
	emCOREEVENT_TRADE_TRADING					= 48,			// 交易中
	emCOREEVENT_TRADE_LOCK						= 49,			// 对方锁定交易
	emCOREEVENT_TRADE_CONFIRM					= 50,			// 对方确认交易
	emCOREEVENT_TRADE_END						= 51,			// 结束交易

	emCOREEVENT_OPEN_REPOSITORY					= 55,			// 打开储物箱
	emCOREEVENT_EXTREPSTATE_CHANGED				= 56,			// 扩展储物箱激活状态改变

	emCOREEVENT_MONEY_CHANGED					= 57,			// 金钱数目变化

	emCOREEVENT_SYNC_TEACHER_LIST				= 58,			// 查找可拜师玩家的结果
	emCOREEVENT_SYNC_STUDENT_LIST				= 59,			// 查找可收徒玩家的结果
	emCOREEVENT_SYNC_RELATION_LIST				= 62,			// 同步人际关系列表
	emCOREEVENT_DELETE_RELATION					= 63,			// 删除与某人的关系
	emCOREEVENT_SYNC_RELATION_INFO				= 64,			// 同步关系玩家信息
	emCOREEVENT_SYNC_TRAINING_OPTION			= 65,			// 同步可拜师可收徒状态
	emCOREEVENT_SYNC_MAIL_LIST					= 66,			// 同步邮件列表
	emCOREEVENT_SELF_CHECKIN_TIME				= 67,			// 查询自己剩余报到时间
	emCOREEVENT_STUDENT_CHECKIN_TIME			= 68,			// 查询徒弟的剩余报到时间
	emCOREEVENT_KIN_BASE_INFO					= 69,			// 家族基本信息更新
	emCOREEVENT_KIN_MEMBER_DATA					= 70,			// 取得家族成员信息
	emCOREEVENT_PROGRESSBAR_ACROSSPERCENT		= 71,			// 进度条完成一个百分比
	emCOREEVENT_SYNC_SELECT_NPC					= 72,			// 客户端选中某个Npc或玩家

	emCOREEVENT_TRAINING_APPLYFORTEACHER		= 73,			// 拜师请求

	emCOREEVENT_TEAM_MEMBER_CHANGED				= 74,			// 组队成员变化
	emCOREEVENT_TONG_BASE_INFO					= 75,			// 帮会基本信息更新
	emCOREEVENT_TONG_ALL_KIN_INFO				= 76,			// 帮会家族成员信息更新
	emCOREEVENT_TONG_FIGURE_INFO				= 77,			// 帮会职位信息更新
	emCOREEVENT_TONG_ANNOUNCE_INFO				= 78,			// 帮会公告信息更新

	emCOREEVENT_IBSHOP_SYNWARELIST				= 79,			// 同步商品列表
	emCOREEVENT_IBSHOP_SYNWARECOUNT				= 80,			// 同步商品数量
	emCOREEVENT_IBSHOP_BUYFAILURE				= 81,			// 购买失败
	emCOREEVENT_IBSHOP_BUYSUCCESS				= 82,			// 购买成功
	emCOREEVENT_IBSHOP_OPENWINDOW				= 83,			// 打开奇珍阁界面
	emCOREEVENT_IBSHOP_CLOSEWINDOW				= 84,			// 关闭奇珍阁界面
	emCOREEVENT_IBSHOP_REDUCEPAGECOUNT			= 85,			// 修改商品页数

	emCOREEVENT_MAIL_FETCHITEMOK				= 86,			// 成功获取邮件物品
	emCOREEVENT_MAIL_FETCHMONEYOK				= 87,			// 成功获取邮件金钱

	emCOREEVENT_RELATION_UPDATEPANEL			= 88,			// 好友列表的更新
	emCOREEVENT_RELATION_REFRESHTRAIN			= 89,			// 师徒关系列表更新
	emCOREEVENT_RELATION_ONLINE					= 90,			// 通知好友上线
	emCOREEVENT_RELATION_DELFRIENDFAVOR			= 91,			// 删除好友好感度
	
	emCOREEVENT_SHOW_MAILCONTENT				= 95,			// 显示邮件内容

	emCOREEVENT_SYNC_CDTYPE						= 96,			// CD时间类型状态改变
	emCOREEVENT_CAST_FIGHTSKILL					= 98,			// 释放一个战斗技能
	emCOREEVENT_GET_CURE						= 99,			// 有人企图使自己复活

	emCOREEVENT_ITEMGIFT_OPEN					= 100,			// 打开给予界面

	emCOREEVENT_ADDLIFESKILL					= 101,			// 添加一个新的生活技能
	emCOREEVENT_DELLIFESKILL					= 102,			// 删除指定生活技能
	emCOREEVENT_CHANGELIFESKILLLEVEL			= 103,			// 改变生活技能等级
	emCOREEVENT_CHANGELIFESKILLEXP				= 104,			// 改变生活技能经验
	emCOREEVENT_ADDRECIPE						= 105,			// 增加一个配方
	emCOREEVENT_DELRECIPE						= 106,			// 删除一个配方

	emCOREEVENT_TASK_ACCEPT						= 107,			// 接收到一个任务
	emCOREEVENT_TASK_AWARD						= 108,			// 接收到一个任务奖励
	emCOREEVENT_TASK_REFRESH					= 109,			// 更新任务
	emCOREEVENT_TASK_TRACK						= 110,			// 追踪任务
	emCOREEVENT_TASK_SHOWBOARD					= 111,			// 触发任务大标题公告

	emCOREEVENT_SET_POPTIP						= 120,			// 设置泡泡显示
	emCOREEVENT_CANCEL_POPTIP					= 121,			// 取消泡泡显示
	emCOREEVENT_NEWS_MSG						= 122,			// 世界新闻

	emCOREEVENT_SYNC_POSITION					= 123,			// 坐标发生变化（角色移动）
	emCOREEVENT_SYNC_CURRENTMAP					= 124,			// 角色所在地图发生变化
	emCOREEVENT_SYNC_CAMPAIGN_DATE				= 125,			// 同步活动数据（如宋金等）
	emCOREEVENT_SYNC_SEX						= 126,			// 角色性别变化
	emCOREEVENT_SYNC_ROUTE						= 127,

	emCOREEVENT_ENHANCE_OPEN					= 130,			// 打开强化界面
	emCOREEVENT_ENHANCE_RESULT					= 131,			// 强化结果通知
	emCOREEVENT_BREAKUP_RESULT					= 132,			// 拆解结果通知
	emCOREEVENT_UPDATE_PLAYERBILLINFO			= 133,			// 更新交易所内角色自己的信息
	emCOREEVENT_UPDATE_SHOWBILLLIST				= 134,			// 更新交易所交易单列表

    emCOREEVENT_PLAYER_DEATH                    = 135,
	emCOREEVENT_STOPLOGOUT						= 136,			// 啥？

	emCOREEVENT_KINHISTORY						= 137,			// 家族历史
	emCOREEVENT_TONGHISTORY						= 138,			// 帮会历史
	emCOREEVENT_KINRECRUITMENT					= 139,			// 更新家族招募申请人数据
	emCOREEVENT_KINRECRUITMENT_STATE			= 140,			// 更新家族招募状态
	emCOREEVENT_KINRECRUITMENT_LIST				= 141,			// 更新招募的家族列表
	emCOREEVENT_TEAM							= 181,
	emCOREEVENT_TEAM_SEARCH_INFO				= 182,
	emCOREEVENT_SELF_SEARCH_INFO				= 183,
	emCOREEVENT_TEAM_DISABLE_STATE				= 184,
	emCOREEVENT_NPC_TRADE						= 185,
	emCOREEVENT_UI_ACTION_NOTIFY				= 186,
	emCOREEVENT_BUY_FROM_STALL					= 187,
	emCOREEVENT_SELL_FROM_OFFER					= 188,
	emCOREEVENT_SWITCH_STALL					= 189,
	emCOREEVENT_SWITCH_OFFER					= 190,
	emCOREEVENT_STALL_INPUT_ADV					= 191,
	emCOREEVENT_OFFER_INPUT_ADV					= 192,
	emCOREEVENT_CLOSE_STALL						= 193,
	emCOREEVENT_CLOSE_OFFER						= 194,
	emCOREEVENT_TEAM_SHARE_RESPONSE				= 195,
	emCOREEVENT_PLAYER_REVIVE					= 196,
	emCOREEVENT_SYNCLIFE_SKILL					= 197,
	emCOREEVENT_DORECIPE_RESULT					= 198,
	emCOREEVENT_STOPPRODUCE						= 199,
	emCOREEVENT_STOPPROCESSTAG					= 200,
	emCOREEVENT_STOPGENERALPROCESS				= 201,
	emCOREEVENT_UI_INPUT						= 202,			// 打开输入界面
	emCOREEVENT_JBCOIN_CHANGED					= 203,			// 金币数改变
	emCOREEVENT_VIEWPLAYER						= 204,			// 打开查看他人信息界面
	emCOREEVENT_ANGEREVENT						= 205,			// 同步怒气事件
	emCOREEVENT_SETANGEREVENT					= 206,			// 绘制怒气槽
	emCOREEVENT_LADDERREFRESH					= 207,			// 排行榜刷新
	emCOREEVENT_OPEN_KINCREATE					= 208,			// 打开创建家族界面
	emCOREEVENT_OPEN_TONGCREATE					= 209,			// 打开创建帮会界面
	emCOREEVENT_CONFIRMATION                    = 210,			// 确认消息
	emCOREEVENT_SYSTEM_MESSAGE					= 211,			// 系统消息
	emCOREEVENT_STOP_AUTOPATH					= 212,			// 结束自动寻路事件
	emCOREEVENT_SYNC_MAKEPRICE					= 213,			// 标价之后的刷新背包
	emCOREEVENT_SYNC_BINDCOINANDMONEY			= 214,			// 绑定金币和银两刷新
	emCOREEVENT_CHANGEWAITGETITEMSTATE			= 215,			// 改变等待获取物品的状态
	emCOREEVENT_SYNC_LOCK						= 216,
	emCOREEVENT_START_AUTOPATH					= 217,			// 开始自动寻路事件
	emCOREEVENT_SYNC_PRESTIGE					= 218,			// 同步江湖威望
	emCOREEVENT_REPORT_SOMEONE					= 219,			// 举报某人
	emCOREEVENT_SYNC_MAP_HIGHLIGHT				= 220,			// 发送特殊显示的NPC亮点 如宋金大将
    emCOREEVENT_SYNC_BLOGINFO					= 221,
	emCOREEVENT_SYNC_AUCTION_RET                = 222,
	emCOREEVENT_HONORDATAREFRESH				= 223,			// 同步玩家荣誉值
	emCOREEVENT_LADDERSEARCHREFRESH				= 224,
	emCOREEVENT_UPDATEBANKINFO					= 225,			// 更新钱庄的金币区和银两区信息
	emCOREEVENT_KIN_ACTION_INFO					= 226,			// 更新家族活动信息
	emCOREEVENT_CHANGE_FACTION_FINISHED			= 227,
	emCOREEVENT_SYNC_DOMAININFO					= 228,			// 领土战即时战报最小消息
	emCOREEVENT_SYNC_DOMAINREPORT				= 229,			// 领土战战报消息
	emCOREEVENT_CHANGEACTIVEAURA				= 230,			// 改变激活的光环
	emCOREEVENT_LADDERPLANREFRESH				= 231,			// 同步排行榜名字
	emCOREEVENT_VIEW_NPC_SKILL					= 232,			// 察看玩家技能点
	emCOREEVENT_COVER_BEGIN						= 233,
	emCOREEVENT_COVER_END						= 234,
	emCOREEVENT_UPDATEONLINEEXPSTATE			= 235,			// 刷新在线托管的状态
#ifdef _SNDA_IGW_
	emCOREEVENT_IBSHOP_PAYURL					= 236,			// zjq add 09.2.23	GS给client的付费URL
#endif


	emCOREEVENT_LEAGUE_UPDATE					= 238,
	emCOREEVENT_CASTSKILL_FAILED				= 239,
	emCOREEVENT_CHAT_CHANNEL					= 240,			// 发送频道栏消息
	emCOREEVENT_OPEN_CHANNEL					= 241,			// 打开频道
	emCOREEVENT_CHAT_SEND_MSG_CHANNEL			= 242,			// 发送消息到指定频道
	emCOREEVENT_CHAT_SOMEONE					= 243,			// 发送给指定的人
	emCOREEVENT_CHAT_OPEN_DYNCHANNEL			= 244,			// 动态频道发送
	emCOREEVENT_CHAT_DYNCHANNEL					= 245,			// 动态频道发送
	emCOREEVENT_CHAT_CLOSE_DYNCHANNEL			= 246,			// 动态频道发送
	emCOREEVENT_SYNC_TIREDTIME					= 247,			// 防沉迷， 在线累计时间

	emCOREEVENT_CHAT_DYNCHANNELINFO				= 249,
	emCOREEVENT_PARTNER_INFO_CHANGED			= 250,			// 同伴变化通知
	emCOREEVENT_PARTNER_VIEWINFO_CHANGED		= 251,			// 同伴变化通知
	
	emCOREEVENT_UI_MINIMAP_CLICK				= 252,			// 小地图被点击（右上角小地图以及世界地图）
	emCOREEVENT_SYNC_TRANS_POSITION				= 253,			// 主角被传送（和emCOREEVENT_SYNC_POSITION不同）
};

enum KE_CONFIRMATION										// 提示消息相关枚举
{
    // 组队10 - 19
    emCONFIRMATION_TEAM_RECEIVE_INVITE			= 10,        // 收到加入队伍邀请
    emCONFIRMATION_TEAM_APPLY_ADD				= 11,        // 收到玩家入队申请

    // 交易20 - 29
    emCONFIRMATION_TRADE_RECEIVE_REQUEST		= 20,       // 收到交易申请
    emCONFIRMATION_TRADE_SEND_REQUEST			= 21,       // 发送了交易申请

    // 家族30 - 39
    emCONFIRMATION_KIN_INVITE_ADD				= 30,       // 被邀请加入家族
	emCONFIRMATION_KIN_INTRODUCE				= 31,       // 被推荐加入家族
	emCONFIRMATION_KIN_CONVECTION				= 32,		// 收到家族成员传送召唤

	// 帮会40 - 49
	emCONFIRMATION_TONG_APPLY_JOIN				= 40,		// 收到帮会加入申请
	emCONFIRMATION_TONG_INVITE_ADD				= 41,		// 收到帮会加入邀请

    // 人际50 - 59             
    emCONFIRMATION_RELATION_TMPFRIEND			= 50,       // 收到好友申请
    emCONFIRMATION_RELATION_BLACKLIST			= 51,       // 收到被加入黑名单的消息
    emCONFIRMATION_RELATION_BINDFRIEND			= 52,       // 被添加为双向好友

	// 师徒60 - 69
	emCONFIRMATION_TEACHER_APPLY_TEACHER		= 60,		// 收到拜师申请
	emCONFIRMATION_TEACHER_APPLY_STUDENT		= 61,		// 收到拜师申请
	emCONFIRMATION_TEACHER_CONVECTION			= 62,		// 收到师傅传送请求

	emCONFIRMATION_PK_EXERCISE_INVITE			= 70,       // 收到切磋请求

};

extern VOID g_CoreEventNotify(INT nEvent, LPCSTR pszFormat = "", ...);
extern VOID g_CoreEventNotifyV(INT nEvent, LPCSTR pszFormat, va_list vArgs);
extern VOID g_LuaCoreEventNotify(KJxScript& sc, INT nParams);
extern VOID __g_CoreEventNotify__Depreciated__(INT nEvent, INT nResults, INT nParams);	// TODO: xyf 作废，以后不要再使用该接口，需要删除以前的相关应用

/****************************************************************************************
//	外界访问Core接口方法定义
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-9-12
------------------------------------------------------------------------------------------
	外界（如界面系统）通过此接口从Core获取游戏世界数据。

	包含伍个接口函数CoreOperationRequest、CoreGetGameData、CoreDrawGameObj、CoreProcessInput
与CoreSetCallDataChangedNofify。
	CoreOperationRequest用于发送对游戏的操作请求。参数uOper为操作的id，其值为梅举类型
GAMEOPERATION_INDEX的取值之一。参数uParam以及nParam的具体含义依据uOper的取值情况而定。
如果成功发送操作请求，函数返回非0值，否则返回0值。这些请求都要求Core立即接受，Core的
客户不保证通过此函数发送的数据在函数调用之后依然有效。
	获知游戏数据有两种方式，一种是调用接口函数CoreGetGameData主动获取，另外一种是注册
通知函数，当游戏数据变更的时候，被注册的通知函数就会被调用，不一定在调用通知函数的同时
传递改变的游戏数据。并且两种方式所处里的数据项范围并不相同。
    接口函数CoreGetGameData参数uDataId表示获取游戏数据的数据项内容索引，其值为梅举类型
GAMEDATA_INDEX的取值之一。参数uParam、nParam以及函数返回值的具体含义依据uDataId的取值
情况而定。
    注册通知函数的接口方法为CoreSetCallDataChangedNofify。参数pNotifyFunc为通知函数的
指针。返回值为非0值表示注册成功，否则表示失败。传入参数pNotifyFunc的值入为0，则已经注
册的通知函数将被取消。通过通知函数通知发生游戏数据改变的时候，不一定同时通过通知函数
传送数据改变。设计原则上是如果改变的数据内容少，可以方便简单地通过通知函数的参数传递的，
则随通知函数传递；否则只是发送通知而已，并不传送改变的游戏的

	CoreDrawGameObj用于绘制单个游戏对象。参数uObjGenre指出对象的类属，uId指出对象的id，
x、y指出绘制范围的左上角坐标，Width、Heightn指出了绘制范围的大小，Param用于额外的参数传
递，其含义将依赖于具体要绘制的对象类型。
*****************************************************************************************/

#include "protocol/kmailprotocol.h"

//=========================================================
// Core外部客户向core获取游戏数据的数据项内容索引定义
//=========================================================
//各数据项索引的相关参数uParam与nParam如果在注释中未提及，则传递定值0。
//如果特别指明返回值含义，则成功获取数据返回1，未成功返回0。
// 分类及索引区间：
//  GAMEOBJ(1 - 1000)			PLAYER(1001 - 2000)		ITEM(2001 - 3000) 
//  TRADE-STALL(3001 - 4000)	Chat(4001 - 5000)		TONG_CITY(5001 - 6000)
// 定义时，必须加上相应前缀
enum GAMEDATA_INDEX
{
	///////////////////////////////////
	// Obj (1 - 1000)
	GDI_GAME_OBJ_FIRST = 1,
//	GDI_GAME_OBJ_DESC = GDI_GAME_OBJ_FIRST,		//游戏对象描述说明文本串
	//uParam = (KUiObjAtContRegion*) 描述游戏对象的结构数据的指针，其中KUiObjAtContRegion::Region的数据固定为0，无意义。
	//nParam = (KGameObjDesc*) 所指的结构空间用于保存取得的描述说明。
/*	
	GDI_GAME_OBJ_DESC_INCLUDE_TRADEINFO,	//游戏对象描述说明文本串(包含交易相关信息)
	//参数含义同GDI_GAME_OBJ_DESC

	GDI_GAME_OBJ_DESC_INCLUDE_REPAIRINFO,	//游戏对象描述说明文本串(包含修理相关信息)
	//参数含义同GDI_GAME_OBJ_DESC

	GDI_GAME_OBJ_DESC_INCLUDE_STALLINFO,	//游戏对象描述说明文本串(包含贩卖标价相关信息)
	//参数含义同GDI_GAME_OBJ_DESC
	
	GDI_GAME_OBJ_DESC_INCLUDE_OFFERINFO,	//游戏对象描述说明文本串(包含收购标价相关信息)
	//参数含义同GDI_GAME_OBJ_DESC
	*/	
	GDI_FACTION_NAME = GDI_GAME_OBJ_FIRST,  // 获取门派名字
	//uParam = 门派索引
	//nParam = 缓冲区长度
	//pParam = (CHAR*) 描述游戏对象的结构数据的指针

	GDI_ITEM_NAME_BY_OBJ,
	//uParam = (CHAR*) 描述游戏对象的结构数据的指针

	GDI_ITEM_DESCRIPTION_BY_LINKDATA,		//由物品链接原始数据获得物品链接描述信息
	//uParam = (KUiGameObject*) 描述游戏对象的结构数据的指针
	//nParam = to be def

	GDI_ITEM_NAME_BY_LINKDATA,				//由物品链接原始数据获得物品的名称

	GDI_ITEM_IMAGE_BY_LINKDATA,				//由物品链接原始数据获得物品的图片

	GDI_FIGHTSKILL_ICON_IMAGE,				//获取技能图标

	GDI_FIGHTSKILL_DESCRIPTION,				//获取技能描述

	GDI_ITEM_LINKDATA,						//获得物品链接原始信息

	GDI_GAME_OBJ_LAST = GDI_ITEM_LINKDATA,
	///////////////////////////////////

	///////////////////////////////////
	// Player (1001 - 2001)
	GDI_PLAYER_FIRST = 1001,
	GDI_PLAYER_BASE_INFO = GDI_PLAYER_FIRST,	//主角的一些不易变的数据
	//uParam = (KUiPlayerBaseInfo*)pInfo

	GDI_PLAYER_RT_INFO,			//主角的一些易变的数据
	//uParam = (KUiPlayerRuntimeInfo*)pInfo

	GDI_PLAYER_RT_ATTRIBUTE,	//主角的一些易变的属性数据
	//uParam = (KUiPlayerAttribute*)pInfo

	GDI_PLAYER_IMMEDSKILL,	//主角的立即使用物品与武功
	//uParam = (KUiPlayerImmedItemSkill*)pInfo

	//玩家名字指针
	GDI_PLAYER_NAME_REF,				//nRet = szName
	GDI_NPC_KINTONG_INFO,				// 获得NPC帮会家族信息
	GDI_PLAYER_KIN_FIGURE,				// 获得玩家家族职位信息
	// uParam = DWORD 某个NPC的ID
	// nParam = KNpcKinTongInfo*

    GDI_RELATION_TYPE,                  // 检查是否与指定玩家存在某种关系（好友，黑名单...）
    // uParam = LPSTR 玩家名字
    // nParam = 关系类型  见KEPLAYERRELATION_TYPE
    // return: TRUE存在，FALSE不存在

	GDI_PLAYER_LAST = 2000,
	///////////////////////////////////

	///////////////////////////////////
	// Item (2001 - 3000)
	GDI_ITEM_FIRST = 2001,

	GDI_ITEM_IN_ENVIRO_PROP = GDI_ITEM_FIRST,	//获得物品在某个环境位置的属性状态
	//uParam = (KUiGameObject*)pObj（当nParam==0时）物品的信息
	//uParam = (KUiObjAtContRegion*)pObj（当nParam!=0时）物品的信息
	//			此时KUiObjAtContRegion::Region的数据固定为0，无意义。
	//nParam = (INT)(bool)bJustTry  是否只是尝试放置
	//Return = (ITEM_IN_ENVIRO_PROP)eProp 物品的属性状态

	GDI_ITEM_LAST = GDI_ITEM_IN_ENVIRO_PROP,
	///////////////////////////////////
	
	///////////////////////////////////
	// TRADE-STALL (3001 - 4000)
	GDI_TRADE_STALL_FIRST = 3001,
	GDI_STALL_ITEM_PRICE = GDI_TRADE_STALL_FIRST,		//获取贩卖物品的标价
	// pParam[0]: (KUiObjAtContRegion*) pItemInfo -> 用于指出是哪处的哪个物品
	// pParam[1]: (KUiItemBuySelInfo*) pPriceInfo -> 用于接收物品名称修理费用等信息
	//Return = (INT)(bool) 是否标价

	GDI_OFFER_ITEM_PRICE,		//获取收购物品的标价
	// pParam[0]: (KUiObjAtContRegion*) pItemInfo -> 用于指出是哪处的哪个物品
	// pParam[1]: (KUiItemBuySelInfo*) pPriceInfo -> 用于接收物品名称修理费用等信息
	//Return = (INT)(bool) 是否标价

	GDI_TRADE_OPER_DATA,		//交易操作相关的数据
	//uParam = (UI_TRADE_OPER_DATA)eOper 具体含义见UI_TRADE_OPER_DATA
	//nParam 具体应用与含义由uParam的取值状况决定,见UI_TRADE_OPER_DATA的说明
	//Return 具体含义由uParam的取值状况决定,见UI_TRADE_OPER_DATA的说明

	//////////////////////////////////////////////////////////////////////////
	// 	flying defined these folllowing items....
	// this item to find out whether player is in stall status
	GDI_IS_STALL_SELL,		// 是否在贩卖的叫卖状态
	//Return = (INT)(bool)bEnable	
	// uParam: -1: self; other: a specified NPC
	// and whether a player can be set into stall status..
	GDI_IS_STALL_BUY,		// 是否在贩卖的购买状态
	//Return = (INT)(bool)bEnable	
	// uParam: -1: self; other: a specified NPC
	// and whether a player can be set into stall status..
	GDI_IS_OFFER_BUY,		// 是否在收购的购买状态
	//Return = (INT)(bool)bEnable	
	// uParam: -1: self; other: a specified NPC
	// and whether a player can be set into stall status..
	GDI_IS_OFFER_SELL,		// 是否在收购的出售状态
	//Return = (INT)(bool)bEnable	
	// uParam: -1: self; other: a specified NPC
	// and whether a player can be set into stall status..
	GDI_TRADE_STALL_LAST = GDI_IS_OFFER_SELL,
	///////////////////////////////////

	///////////////////////////////////
	// Chat	(4001 - 5000)
	GDI_CHAT_FIRST = 4001,

	GDI_CHAT_LAST = GDI_CHAT_FIRST,
	///////////////////////////////////

	///////////////////////////////////
	// TONG_CITY (5001 - 6000)
	GDI_TONG_CITY_FIRST = 5001,
	GDI_TONG_CLIENT_TONG_INFO = GDI_TONG_CITY_FIRST,		//直接获取客户端上旧的帮会数据
	//(KUiPlayerRelationWithOther *)uParam
	//(KTongInfo *)nParam

	GDI_TONG_CITY_LAST = GDI_TONG_CLIENT_TONG_INFO,
	///////////////////////////////////

	///////////////////////////////////
	// OTHER (6001 - 7000)
	GDI_OTHER_FIRST = 6001,

	GDI_NPC_TEMPLATE_SPR_INFO,
	GDI_OTHER_ID_PICTURE,
	GDI_OTHER_NPC_POS,

	GDI_OTHER_LAST = GDI_OTHER_NPC_POS,
	///////////////////////////////////

};

//=========================================================
// Core外部客户向core获取游戏数据的数据项内容索引定义
//=========================================================
//各数据项索引的相关参数uParam与nParam如果在注释中未提及，则传递定值0。
enum GAMEDATA_CHANGED_NOTIFY_INDEX
{
	GDCNI_PLAYER_BASE_INFO = 1,	//主角的一些不易变的数据, see also GDI_PLAYER_BASE_INFO

	GDCNI_CHANNEL_CHANGE,		//通知频道发生改变

	GDCNI_TEAM,					//队伍信息发生变化（队伍解散、玩家脱离这个队伍，队长权利转给别人等，关注别人的队伍等）
	//uParam = (KUiPlayerTeam*)pTeam -> 队伍信息,可以为NULL,表示玩家脱离了队伍

	GDCNI_GAME_START,			//进入游戏世界

	GDCNI_GAME_QUIT,			//离开了游戏世界

	GDCNI_GAME_EXCHANGE_SERVER, // 跨服

	GDCNI_QUESTION_CHOOSE,		//问题选择
	//uParam = (KUiQuestionAndAnswer*)pQuestionAndAnswer

	GDCNI_MSG_ARRIVAL,			//新（聊天内容窗口）消息来到
	//uParam = (cons CHAR*)pMsgBuff 消息内容缓冲区
	//nParam = (KUiMsgParam*)pMsgParam 消息参数

	GDCNI_CHAT_GROUP,			//聊天好友分组发生变化

	GDCNI_CHAT_FRIEND,			//聊天好友发生变化
	//nParam = nGroupIndex 发生好友变化的分组的索引

	GDCNI_CHAT_FRIEND_STATUS,	//聊天好友状态发生变化
	//uParam = (KUiPlayerItem*)pFriend 发生状态变化的好友
	//			KUiPlayerItem::nData = (CHAT_STATUS)eFriendStatus 好友的当前状态
	//nParam = nGroupIndex 好友所属于的组的索引
	
	GDCNI_CHAT_MESSAGE,			//好有发来的讯息
	//uParam = (KUiChatMessage*)pMessage 消息数据
	//nParam = (KUiPlayerItem*)pFriend   发来讯息的好友

	GDCNI_SYSTEM_MESSAGE,		//系统消息//请看KPlayer.h：gShowSystemMessage
	//uParam = (KSystemMessage*)pMsg 系统消息的内容
	//nParam = (LPVOID)pParamBuf 指向一个参数缓冲区，缓冲内数据含义依据pMsg的内容而定，
	//			参看KSystemMessage的注释说明，缓冲区的大小由pMsg->byParamSize给出。

	GDCNI_SWITCH_CURSOR,		//切换鼠标指针图形
	//nParam = \Ui\???\鼠标.ini中给出的图形索引数值

	GDCNI_SWITCHING_SCENEPLACE,	//地图切换
	//nParam = (INT)(bool)bStart 为非0值表示开始地图切换，为0值表示结束地图切换

	GDCNI_MISSION_RECORD,		//要求纪录下此任务提示信息
	//uParam = (KMissionRecord*) pRecord 纪录内容

	GDCNII_RANK_INDEX_LIST_ARRIVE,	//排名项id列表到了
	//uParam = uCount 排名项id的数目
	//nParam = (KRankIndex*)pIndexList 排名项id描述结构的数组

	GDCNII_RANK_INFORMATION_ARRIVE,	//排名项的内容到了
	//uParam = ((USHORT)usCount) | ((USHORT)usIndexId << 16)
	//			usCount    此次传来的内容项的数目
	//			usIndexId  排名项id，表示传回的是这个id的排名项的内容
	//nParam = (KRankMessage *) 列表内容

	GDCNI_UI_ACTION_NOTIFY,    //简单的通知界面某个界面动作进行
	//uParam = 枚举UI_ACTION的值之一
	//nParam = 可能的参数

	GDCNI_STALL_MISS_ITEM,	//卖方摊位物品减少
	//uParam = (KUiObjAtRegion*)

	//动感色子老虎机上的大奖累计金额来了, uParam = 奖金金额
	GDCNI_DICE_WEAVE_BINGO_MONEY,

	//动感色子老虎机的下注结果来了，uParam = 第几颗色子，nParam = 色子点数
	GDCNI_DICE_WEAVE_DICE_NUMBER,

	//动感色子老虎机的获奖情况来了，uParam = 获奖的程度 0,1,2,3 0=输
	//                              nParam = (CHAR *)szString，获奖的描述字符串，没有则填0吧
	GDCNI_DICE_WEAVE_RESULT,
	
	//打开动感色子老虎机界面，uParam = 0关闭  1打开
	GDCNI_DICE_WEAVE_OPEN,

	//打开通用Server脚本从Client获取输入的对话框, uParam = (COMMON_INPUT*);	nParam = 0; [wxb 2004-3-29]
	GDCNI_OPEN_COMMON_INPUT,

	//箱子操作的返回 uParam = 非0 表示成功
	GDCNI_BOX_OPERATION_RETURN,

	// 物品磨损值发生变化时，通知客户端
	GDCNI_ITEM_DURABILITY,

	//uParam = 宋金战场中某个玩家的某个事件信息
	GDCNI_BATTLE_EVENT_HAPPEN,
	//uParam = 前十玩家的数据

	//uParam = 地图名, nParam = 地图ID
	GDCNI_CHANGE_SCENE_INFO,

	GDCNI_CURRENTINFOANSWER,//得到服务器当前信息(目前是时间)

	GDCNI_PRIZE_ARRIVE,		//任务奖品来啦~~~

	GDCNI_MINI_MAP_OBJ_SYNC,//小地图标记物信息同步

	GDCNI_TASK_ARRIVE,			//非常有趣的任务来了^_^
	
	GDCNI_TELL_POS,				//你要去的地方在那!
	
	GDCNI_NEW_TASK_ON_CHANGE,	//玩家任务变量改变

	GDCNI_SPECTATOR_S2C_PROC,	//相应从服务器过来的观战模式的消息

	GDCNI_TASK_TIP,				//任务提示
	GDCNI_FRIEND_INQUIRE,		//好友查询

	GDCNI_PARTNER_BTN_LIST,						// 快捷栏
	GDCNI_PARTNER_ADD,							// 同伴增加了							nParam：所增同伴索引
	GDCNI_PARTNER_REMOVE,						// 同伴删除了							nParam：所删同伴索引
	GDCNI_PARTNER_CURPARTNER_CHANGED,			// 当前选择同伴改变了					nParam：当前同伴索引
	GDCNI_PARTNER_CALLOUT_CHANGED,				// 当前选择同伴召唤/唤入状态变化		uParam：当前同伴召唤/唤入状态
	GDCNI_PARTNER_FOLLOW_ONLY_CHANGED,			// 当前选择同伴仅跟随状态变化			uParam：当前同伴仅跟随状态
	GDCNI_PARTNER_REMIND_CHANGED,				// 同伴提示开关							uParam：开/关
	GDCNI_PARTNER_ATTRIB_CHANGED,				// 某个同伴属性变化了					nParam：同伴索引
	GDCNI_PARTNER_RESIST_SKILL_CHANGED,			// 某个同伴抗性技能变化了				nParam：同伴索引
	GDCNI_PARTNER_GIVEN_SKILL_CHANGED,			// 某个同伴先天技能变化了				nParam：同伴索引
	GDCNI_PARTNER_LEARNT_SKILL_CHANGED,			// 某个同伴自学技能变化了				nParam：同伴索引
	GDCNI_PARTNER_ULTIMATE_SKILL_CHANGED,		// 某个同伴绝技变化了					nParam：同伴索引
	GDCNI_PARTNER_STANDBY_SKILL_CHANGED,		// 某个同伴候选激活技能变化了			nParam：同伴索引
	GDCNI_PARTNER_TASKVALUE_CHANGE,				// 某个同伴任务变量变化了				uParan：任务变量ID,值（std::pair<INT, INT>）	nParam：同伴索引

	GDCNI_OPEN_MARKET,							//打开珍奇阁
	GDCNI_CHATROOM_ACTION_RESULT,				//聊天室操作返回结果
	GDCNI_ROOMLIST_ARRIVE,						//聊天室列表来了！
	GDCNI_MEMBERLIST_ARRIVE,					//聊天室成员列表来了
	GDCNI_BLACKLIST_ARRIVE,						//黑名单列表来了

	GDCNI_CANCEL_ITEMREPAIR,					// 取消道具修理

	//====帮派相关====
	GDCNI_TONG_OPEN_CREATE_SHEET,   //通知界面打开帮会创建界面
	//uParam = !0 打开    0 关闭
	//nParam = enumCREATE_SHEET_USAGE其中之一

	GDCNI_TONG_INFO,				//某人所在的帮派的信息
	//对应于GTOI_REQUEST_PLAYER_TONG（查询某人是哪帮会）
	//uParam = (KUiPlayerRelationWithOther*) 那个（某）人,以及此人与此帮派的关系
	//KUiPlayerRelationWithOther::nParam 是否开着的招人开关
	//nParam = (KTongInfo*) pToneInfo	//帮派的信息

	GDCNI_TONG_MEMBER_LIST,			//某个帮派中的成员的列表
	//uParam = (KUiGameObjectWithName*) pTong 用于描述是哪个帮派
	//KUiGameObjectWithName::szName 帮派名称
	//KUiGameObjectWithName::nData  pMemberList所给帮派成员的类型，参看TONG_MEMBER_FIGURE
	//KUiGameObjectWithName::nParam pMemberList所给帮派成员列表包含成员的起始的索引
	//KUiGameObjectWithName::uParam pMemberList所给帮派成员列表包含成员的数目
	//nParam = (KTongMemberItem*) pMemberList

	GDCNI_TONG_ACTION_RESULT,       //帮会操作返回的结果
	//uParam = (KUiGameObjectWithName*) pInfo 返回的数据
	//KUiGameObjectWithName::szName 对象玩家的名字
	//KUiGameObjectWithName::nData  操作的种类，参考TONG_ACTION_TYPE
	//KUiGameObjectWithName::nParam   V
	//KUiGameObjectWithName::uParam   V
	//KUiGameObjectWithName::szString 这几个涵义随不同操作不同
	// nParam : 成功 1 失败 0

	GDCNI_UNION_PAGE_DATA,						// 联盟列表页面的数据
	GDCNI_TONG_PAGE_DATA,						// 帮会列表页面的数据
	GDCNI_TONG_NAME_LIST,						// 帮会名称列
	GDCNI_MEMBER_PAGE_DATA,						// 帮会成员列表的数据
	GDCNI_TONG_SINGLE_MEMBER_DATA,				// 帮会中单个成员的全部数据
	GDCNI_TONG_WORKSHOP_DATA,					// 帮会作坊信息数据
	GDCNI_TONG_OPERATION_RESULT,				// 帮会操作的返回结果
	GDCNI_MEMBER_NAME_LIST,						// 帮会成员名称列
	GDCNI_TONG_WEEKLY_AIM,						// 帮会周目标
	GDCNI_TONG_ANNOUNCEMENT,					// 帮会公告
	GDCNI_TONG_EVENTRECORD_LIST,				// 帮会事件记录列表
	GDCNI_TONG_HISTORYRECORD_LIST,				// 帮会历史记录列表
	GDCNI_FIGURE_MEMBER_LIST,					// 得到职位成员列表
	GDCNI_FIGURE_COUNT,							// 得到职位成员数量
	GDCNI_MEMBER_RIGHT_DATA,					// 成员权限
	GDCNI_UNION_TONG_LIST,						// 联盟的成员帮会列表
	GDCNI_UNION_NAME_LIST,						// 联盟名称列
	GDCNI_REFRESH_PAGE,							// 被通知，需要刷新列表
	GDCNI_MINI_MAP_RESTORE,						// 恢复小地图位置

	GDCNI_GEY_UI_ACCOUNT,						// 获得登陆框上输入的帐号
	GDCNI_QUALITY_NEWEVENT,
	//uParam	= LPSTR pszAccount	（NULL表示不需要获取）
};

enum KE_DEBUG_INFO_FLAG
{
	emDEBUG_INFO_MAIN			=	0x000001,		// Npc、玩家的主要调试信息
	emDEBUG_INFO_OBSTACLE		=	0x000002,		// 障碍调试信息
	emDEBUG_INFO_SCENE			=	0x000004,		// 场景调试信息
	emDEBUG_INFO_SCENE_DETAIL	=	0x000008,		// 场景调试信息（带关系线）
	emDEBUG_INFO_COUNT			=	4,				// 上面的枚举增加级的修改此个数
};

//=========================================================
// Core外部客户对core的操作请求的索引定义
//=========================================================
enum GAMEOPERATION_INDEX
{
	GOI_EXIT_GAME_APPLY = 1,		//请求离开游戏世界
	GOI_EXIT_GAME,					//离开游戏清理
	GOI_EXCHANGE_SERVER,			//跨服
	GOI_PLAYER_ACTION,		//玩家执行/取消某个动作
	//uParam = (PLAYER_ACTION_LIST)eAction 动作标识

	//============（与其它玩家）交易相关================
	GOI_TRADE_INVITE_RESPONSE,	//答应/拒绝交易请求
	//uParam = (KUiPlayerItem*)pRequestPlayer 发出请求的玩家
	//nParam = (INT)(bool)bAccept 是否接受请求

	GOI_TRADE_CANCEL_APPLY,		// 取消交易申请
	//uParam = (KUiPlayerItem*)pDstPlayer 申请交易的目标玩家

	//============聊天相关================
	GOI_OPTION_SETTING,			//选项设置
	//uParam = (OPTIONS_LIST)eOptionItem 要设置的选项
	//nParam = (INT)nValue 设置的值，其含义依赖于eOptionItem的含义
	//					参看OPTIONS_LIST各值的注释

	GOI_PLAY_SOUND,				//播放声音
	//uParam = (LPCSTR )pszFileName

	GOI_REVENGE_SOMEONE,		//仇杀某人
	//uParam = (KUiPlayerItem*) pTarget	仇杀目标

	GOI_INVITE_EXERCISE,		// 申请和某人切磋
	GOI_REPLY_INVITE_EXERCISE,	// 应答切磋邀请
	
	GOI_VIEW_PLAYERITEM,		//申请看玩家装备
	//uParam = dwNpcID	玩家的m_dwID
	GOI_VIEW_PLAYERITEM_END,	//看玩家装备结束

	GOI_FOLLOW_SOMEONE,			//跟随某人
	//uParam = (KUiPlayerItem*) pTarget	跟随目标

	GOI_SWITCH_HORSE,           //上下马状态的切换，uParam = 0 上马       1 下马

	GOI_SHOW_GAMESPACE_ITEM_NAME,//显示游戏地图上物品的名字,nParam == 0 不显示   !0 显示
	
	GOI_GIVE_SOMEONE_MONEY,     //给某人金钱，uParam == 玩家ID，nParam == 多少钱
	
	GOI_IS_GO_SERVER_PICK_DROP,	//拾起放下动作是否通过服务器,uParam == 1否，uParam == 0是

	GOI_GAME_APP_ACTIVE,		//游戏程序切换到前台/后台
	//nParam = (INT)bool bActive

	GOI_PLAYER_SET_PORTRAIT,	//玩家设置头像,nParam = 头像索引
	// flying commented, about the stall process
	// uParam =	0: Finish the stall status;
	//			1: Enter the stall status;
	//			2: 
	// nParam: not used.

	GOI_INVISIBLE_IN_FRIEND_LIST,

	GOI_QUERY_CITY_OWNER,	//请求城市占领者数据,uparam 城市id; nParam:0

	GOI_CHANGE_AUTOATTACK,

	GOI_REQUEST_CURRENTINFO, //得到服务器端的信息,目前是时间

	GOI_REQUEST_SELF_POS,	//请求得到自己的坐标

	GOI_WATCH_GAME_INFO,		//观战模式的观战请求
	
	GOI_PARTNER_LIST_DLG,		//同伴按钮列表相关消息头
	GOI_PARTNER_CHARACTER_DLG,	//同伴属性对话框相关消息头
	GOI_PARTNER_SKILL_DLG,		//同伴技能对话框相关消息头
	GOI_PARTNER_SET_NAME,			// 设置同伴名字
	GOI_PARTNER_SET_STANDBY_SKILL,	// 设置同伴候选激活技能
	GOI_PARTNER_REMOVE_SKILL,		// 遗忘同伴技能

	GOI_MOUSE_CLICK,				//鼠标点击

	GOI_GET_GOODS_DATA,				//获得指定商店的商品数据
	GOI_GET_SERVER_TIME,			//请求服务器时间

	GDI_SCRIPT_LOADFILE,			// 载入一个脚本文件
	// uParam: pszFileName
	// nParam: bFource
	// return (BOOL)success

	GDI_SCRIPT_LOADDIR,				// 载入一个目录下的所有脚本文件
	// uParam: pszDirName
	// nParam: pszPartnerFile = NULL
	// return (BOOL)success

	GDI_GET_GAME_FRAME_TIME,		// 获取当前游戏世界帧数
	// return uFrameTime

#ifdef SWORDONLINE_SHOW_DBUG_INFO
	GOI_SHOW_DEBUG_INFO,			// 显示调试信息
#endif
	GDI_ISNAME_PASS,

	GDI_QUALITY_GET_SCREENINFO,
	GDI_QUALITY_GET_SELPLAYER,
	GDI_QUALITY_NEWEVENT,
	GOI_VIEW_PLAYERSKILL,
};

//=========================================================
// Core外部客户对core的场景地图相关的操作请求的索引定义
//=========================================================
//各数据项索引的相关参数uParam与nParam如果在注释中未提及，则传递定值0。
//如果特别指明返回值含义，则成功获取数据返回1，未成功返回0。
enum GAME_SCENE_MAP_OPERATION_INDEX
{
	GSMOI_SCENE_TIME_INFO,			//当前主角所处的地域时间环境
	//uParam = (KUiSceneTimeInfo*)pInfo

	GSMOI_SCENE_MAP_INFO,				//当前主角所处的场景的地图信息
	//uParam = (KSceneMapInfo*) pInfo 用于获取信息的结构缓冲区的指针
	//Return = (INT)(bool)bHaveMap 返回值表示当前场景是否有小地图。如果返回0值时， pInfo内返回的值无意义

	GSMOI_IS_SCENE_MAP_SHOWING,	//设置场景的小地图是否显示的状态
	//uParam = uShowElem,		//显示哪些内容，取值为SCENE_PLACE_MAP_ELEM枚举的一个或多个的组合。
				//SCENE_PLACE_MAP_ELEM在GameDataDef.h中定义
				//浏览小地图与其它一些项是互斥的
	//nParam = 低16位表示显示的宽度，高16位表示显示的高度（单位：像素点）

	GSMOI_PAINT_SCENE_MAP,		//绘制场景的小地图
	//uParam = (INT)h 表示绘制起始点在屏幕上横坐标坐标（单位：像素点）
	//nParam = (INT)v 表示绘制起始点在屏幕上纵坐标坐标（单位：像素点）

	GSMOI_SCENE_MAP_FOCUS_OFFSET,//设置小地图的焦点偏移（/中心）
	//uParam = (INT)nOffsetH	设置小地图焦点的水平坐标偏移（单位：场景坐标）
	//nParam = (INT)nOffsetV	设置小地图焦点的垂值坐标偏移（单位：场景坐标）

	GSMOI_SCENE_FOLLOW_WITH_MAP,	//设置场景是否随着地图的移动而移动
	//nParam = (INT)nbEnable 场景是否随着地图的移动而移动

	GSMOI_SCENE_CHANGE_MAP,			//更换小地图，nParam = 小地图的编号，
									//-1则表示当前的场景的小地图

	GSMOI_SCENE_MAP_FOCUS,			//设置小地图的焦点（/中心）
	//uParam = (INT)nFocusH		设置小地图焦点的水平坐标（单位：场景坐标）
	//nParam = (INT)nFocusV		设置小地图焦点的垂值坐标（单位：场景坐标）
	
	GSMOI_SCENE_LITTLE_MAP_INFO,	//当前主角所处的场景的地图信息
	//uParam = (KSceneMapInfo*) pInfo 用于获取信息的结构缓冲区的指针
	//Return = (INT)(bool)bHaveMap 返回值表示当前场景是否有小地图。如果返回0值时， pInfo内返回的值无意义

	GSMOI_GET_SCENE_ID,			    //获取场景的ID

	GSMOI_SCENE_LITTLE_MAP_FOCUS,	//设置小地图的焦点（/中心）
	//uParam = (INT)nFocusH		设置小地图焦点的水平坐标（单位：场景坐标）
	//nParam = (INT)nFocusV		设置小地图焦点的垂值坐标（单位：场景坐标）

	GSMOI_PAINT_SCENE_MAP_TOPLAYER,			// 绘制小地图顶层
};

//=========================================================
// Core外部客户对core的组队相关的操作请求的索引定义
//=========================================================
//各数据项索引的相关参数uParam与nParam如果在注释中未提及，则传递定值0。
//如果特别指明返回值含义，则成功获取数据返回1，未成功返回0。
enum GAME_TEAM_OPERATION_INDEX
{
	//----获取数据----
	TEAM_OI_GD_INFO,				//主角所在的队伍信息
	//uParam = (KUiPlayerTeam*)pTeam -> 队伍信息，为空则不取
	//Return = bInTeam, 如果为非0值表示主角在队伍中，pTeam结构被填充信息。
	//					如果为0值表示主角不在队伍中，pTeam结构未被填充有效信息。

	TEAM_OI_INVITE,			//邀请别人加入队伍，只有队长调用才有效果
	//uParam = (KUiPlayerItem*)pPlayer -> 要邀请的人的信息
	//			KUiPlayerItem::nData = 0

	TEAM_OI_KICK,				//踢除队里的一个队员，只有队长调用才有效果
	//uParam = (KUiPlayerItem*)pPlayer -> 要踢除的队员的信息
	//			KUiPlayerItem::nData = 0

	TEAM_OI_LEAVE,				//离开队伍

	TEAM_OI_APPLY_RESPONSE,			//是否批准他人加入队伍，只有队长调用才有效果
	//uParam = (KUiPlayerItem*)pPlayer -> 要欲批准的人信息
	//			KUiPlayerItem::nData = 0
	//nParam = (INT)(bool)bApprove -> 是否批准了

	TEAM_OI_INVITE_RESPONSE,	//对组队邀请的回复
	//uParam = (KUiPlayerItem*)pTeamLeader 发出组队邀请的队长
	//nParam = (INT)(bool)bAccept 是否接受邀请

	TEAM_OI_GD_ISTEAMMATE,			//判断是否互为队友
	TEAM_OI_GD_ISTEAMFULL,			//判断是否队满
};

enum MSG_FLAG
{
	MSG_F_SUCCESSED = 0,	//消息发送成功
	MSG_F_STORE,			//消息被存储
	MSG_F_FAILED,			//消息发送失败
};

enum UI_EVENT
{
	UPDATE_TITLE
};

// Core通过此接口给GameServer发送数据
class IKCoreClient
{
public:
	virtual BOOL SendData(LPVOID pData, size_t nDataLen) = 0;
	virtual VOID Shutdown() = 0;
};

// 邮件内容
struct KUI_MAILCONTENT
{
	INT		nMailID;		// 邮件ID
	INT		nPictureID;		// 图片ID
	LPSTR	pszContent;		// 邮件正文
	INT		nContentLen;	// 正文长度
	DWORD	dwMoney;		// 附带金钱
	INT		nItemCount;		// 附带物品数量
	INT		aryItemIdx[0];	// 附带物品索引
};

//-------游戏世界数据改变的通知函数原型---------
struct IClientCallback
{
	virtual VOID CoreDataChanged(UINT uDataId, UINT uParam, INT nParam) = 0;
	/***********************************************************
	 Function		: NotifyGameCenterLogin
	 Description	: 通知玩家在GameCenter登录成功
	 ***********************************************************/
	virtual VOID NotifyGameCenterLogin() = 0;
	virtual VOID AddChatList(LPCSTR pszName) = 0;	// 增加最近密聊名单
};

#ifdef KD_MUTITREAD_INIT
struct IKLoadProcessNotify
{
	virtual BOOL OnLoadProcessUpdate(LPCSTR szTip, INT nCurPos) = 0;
	virtual BOOL SetLoadTip(LPCSTR szTip) = 0;
};
#endif

class IRepresentShell;
class ISoundShell;
class IKG_Buffer;
class iCoreShell
{
public:
	BOOL InitGlobalEnv(IRepresentShell* pRepresentShell, ISoundShell* pSoundShell, BOOL bProtected, INT nProductRegion, INT nProductLanguage);
	BOOL InitGameWorld(VOID);
	VOID UnInitGameWorld(VOID);
	VOID UnInitGlobalEnv(VOID);
	INT	 GetProtocolSize(BYTE byProtocol);
	VOID ProcessInput(UINT uMsg, UINT uParam, INT nParam);
	BOOL FindSelectNPC(INT x, INT y, INT nRelation, bool bSelect, LPVOID pReturn, INT& nKind, BOOL bDialogNpcFirst = FALSE);
	BOOL FindSelectObject(INT x, INT y, bool bSelect, INT& nObjectIdx, INT& nKind);
	BOOL FindSpecialNPC(LPCSTR pszName, LPVOID pReturn, INT& nKind);
	BOOL GetNpcById(INT nId, LPVOID pReturn, INT& nKind);
	BOOL ChatSpecialPlayer(LPVOID pPlayer, LPCSTR pMsgBuff, USHORT nMsgLength);
	VOID ApplyAddTeam(LPVOID pPlayer);
	VOID TradeApplyStart(LPVOID pPlayer);
	BOOL UseSkill(INT x, INT y, INT nSkillID);
	BOOL LockSomeoneUseSkill(INT nTargetIndex, INT nSkillID);
	BOOL LockSomeoneAction(INT nTargetIndex);
	BOOL LockObjectAction(INT nTargetIndex);
	VOID GotoWhere(INT x, INT y, INT mode);	//mode 0 is auto, 1 is walk, 2 is run
	VOID Goto(INT nDir, INT mode);	//nDir 0~63, mode 0 is auto, 1 is walk, 2 is run
	VOID Turn(INT nDir);	//nDir 0 is left, 1 is right, 2 is back
	BOOL PartnerSetTarget(INT nNpcIdx);

	INT	 OperationRequest(UINT uOper, UINT uParam, INT nParam, LPVOID pParam = NULL);
	//与地图相关的操作,uOper的取值来自 GAME_SCENE_MAP_OPERATION_INDEX
	INT	SceneMapOperation(UINT uOper, UINT uParam, INT nParam);
	//与组队相关的操作，uOper的取值来自 GAME_TEAM_OPERATION_INDEX
	INT TeamOperation(UINT uOper, UINT uParam, INT nParam);
	INT	GetGameData(UINT uDataId, UINT uParam, INT nParam, LPVOID pParam = NULL);

	VOID DrawGameObj(UINT uObjGenre, UINT uId, INT x, INT y, INT Width, INT Height, INT nParam);
	VOID DrawGameSpace();
	DWORD GetPing();
	//VOID SendPing();
	VOID NetMsgCallbackFunc(IKG_Buffer*);
	VOID SetRepresentAreaSize(INT nWidth, INT nHeight);
	BOOL Breathe();
	VOID SetClient(IKCoreClient *pClient);
	VOID SendNewDataToServer(LPVOID pData, INT nLength);

	INT InsertItem(
		INT 	nGenre, 
		INT 	nDetail, 
		INT 	nParticular, 
		INT 	nLevel, 
		INT 	nSeries,
		INT 	nEnhTimes,
		INT 	nLucky,
		LPVOID	pGenInfo,
		INT 	nVersion,
		INT 	nRandSeed,
		INT 	ndurability, 
		INT 	nmaxdurability
	);

	VOID RemoveItem(INT nIndex);
	// 开始/结束摆摊标价
	VOID BeginSetPriceForStall();
	VOID EndSetPriceForStall();

	VOID OnRepresentReset(VOID);			// represent重置时（设备丢失）所进行的重绘工作
	VOID AutoFocus(VOID);
	BOOL SetProtectPackFile(BOOL bProtected);

	BOOL SendChannelMsg(LPCSTR szChannelName, LPCSTR szMsg, INT nSize);
	BOOL SendPrivateMsg(LPCSTR szMsg, INT nSize, LPCSTR szDstPlayerName);

//（工具专用）
#ifdef TOOLVERSION
	//取得技能的描述语言
	virtual LPCSTR GetSkillDesc(INT nSkill, INT nSkillLevel);
#endif
	
//-----------------------------------------------------------------------------
	BOOL AddToLuaFuncLib(LPCSTR pszName, LPVOID pFuncs, INT nCount);
	BOOL RegisterGameScriptFuns(LPVOID pScript);

	VOID CoreDataChanged(UINT uDataId, UINT uParam, INT nParam);
//-----------------------------------------------------------------------------
	INT CheckRelationSet(INT nIndex, INT nRelation);
	VOID OnUiEvent(INT event, INT lParam = 0, INT wParam = 0);
};

typedef iCoreShell KCoreShell;
class ISoundShell;

#ifndef CORE_EXPORTS
// 获取iCoreShell接口实例的指针
#ifdef KD_MUTITREAD_INIT
extern "C" iCoreShell* CoreGetShell(IClientCallback* pNotifyFunc, IKLoadProcessNotify* pNotify);
#else
extern "C" iCoreShell* CoreGetShell(IClientCallback* pNotifyFunc);
#endif
#else
VOID CoreDataChanged(UINT uDataId, UINT uParam, INT nParam);	//对外发送游戏世界数据发生改变的通知
#endif

// -------------------------------------------------------------------------
#endif
#endif /* __GAMEWORLD_CORESHELL_H__ */


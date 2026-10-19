/* -------------------------------------------------------------------------
//	文件名		：	kautoai.h
//	创建者		：	luobaohang
//	创建时间	：	2007-11-3 12:24:53
//	功能描述	：	自动挂机AI
//
// -----------------------------------------------------------------------*/
#ifndef __KAUTOAI_H__
#define __KAUTOAI_H__

#include <vector>
#include <string>
#include <deque>
#include "keventsys.h"

// -------------------------------------------------------------------------
#define KDF_AUTOAI_SCTRPT "script/auto_fight.lua"
#define ITEM_PICK_NUM 6
#define DIS_OFF  1000	//判断超出边缘的距离
#define DIS_MOVE  500	//判断能直接走的距离

struct ItemDrop
{
	INT nItem;
	INT nX;
	INT nY;
};
enum HAND_COMMOND
{
	NO_COMMON,
	COMMON_BEGIN,
	AUTO_ATTACK,
	AUTO_PICK,
	AUTO_DRUG,//自动吃药
	DEATH_PROTECT,//死亡保护
	RELOAD_SCRIPT,
	DO_FILE,
	AUTO_POTION_SHOP,
	AUTO_BAZAAR_SHOP,
	AUTO_STOREAGE,
	AUTO_DIALOGNPC,
	AUTO_SELECT,
	AUTO_TRANSPORT,
	AUTO_GOTO,
	AUTO_SAY,
	AUTO_SLEEP,
	SET_FALSE_SECTION, //设置出错跳转的段
	DO_SECTION,	//调用一个段(可返回)
	JUMP_TO, //跳转到新的过程
	AUTO_COMPOUND,
	COMMON_END,
};
enum AI_STATE
{
	AI_FREE,
	AI_RECORDING,
	AI_RUNSCRIPT,
};
struct AI_BFLAG
{
	unsigned	auto_pick : 1;
	unsigned	auto_drug : 1;
	unsigned	death_protect : 1;
	unsigned	killer_warning : 1;
};

class AutoAi;
//自动寻路类
class KAutoMove
{
public:
	KAutoMove() { m_CurDir = -1; }
	INT AutoMove();	//以当前方向随机寻路移动
	INT	AutoMoveTo(INT nX, INT nY);	//向目标点寻路移动
	VOID Reset(){ m_CurDir = -1; };
protected:
	//向某一方向寻路并Move，返回移动距离及方向
	INT FindPathAndMove(INT &nDir8, INT minStep = -1);
	//检查直线能走的最远距离
	inline INT	CheckDirectMoveDis(KNpc *pNpc, INT nDir8, INT nSpeed);
	INT m_CurDir;
};

class AutoAi : public KObserver
{
public:
	AutoAi();
	BOOL	Init();
	BOOL	ProcessHandCommand(const CHAR* szCommon, INT lParam, INT wParam);
	BOOL	ProcessHandCommand(HAND_COMMOND cmd, INT lParam = 0, INT wParam = 0);
	VOID	ProcessScriptControl(UINT uKey);
	// 1开始，0停止，其他切换
	VOID	InitAutoAttack(INT nMode);
	INT		PlayerFindAim();
	BOOL	PlayerCanAttack(INT nNpcIdx);
	VOID	InitGameReload(INT nStep);
	VOID	AddDrapItem(INT nItem, INT nx, INT ny);
	VOID	SetTruePos(INT x, INT y);
	BOOL	AutoRepairItem();
	BOOL	RunNormal();
	BOOL	RunAutoAttack();
	BOOL	CheckIfValueItem(INT nItem);
	VOID	ReturnCity();
	VOID	AutoAiItemDrop(INT nItem, INT nX, INT nY);
	VOID	RandomMove();
	BOOL	CheckIfCanDirectMove(INT nx, INT ny);
	BOOL	InEyeshot(INT nIdx);
	VOID	OnAttack(INT nNpcIdx);
	BOOL	InitGameLoad(INT nStep);
	BOOL	Flee(INT nNpcIdx);
	BOOL	PickAroundItem(); // 拾取离玩家最近的一件物品
	BOOL	GetAutoFightState() { return m_nAiState; }
	KAutoMove m_cAutoMove;
	AI_BFLAG m_AiBFlag;
	INT	m_nActiveRadius;
	INT m_nVisionRadius;
	INT m_nAttackRadius;
	ItemDrop m_aryItemPickList[ITEM_PICK_NUM]; //较远暂时捡不到的物品
	INT m_ItemPickIndex;
	std::vector<INT> m_vecItemPickType;
	std::vector<std::string> m_vecItemPickName, m_vecAvoidPickName;
	std::vector<std::string> m_vecNpcOnlyAttack, m_vecNpcAvoidAttack;
	std::vector<INT> m_vecItemToPick;
	INT	m_aDropItemId[10];	//最近扔掉的物品ID
	INT m_nDropItemPos;
	INT m_nRealX, m_nRealY;	//当前玩家精确坐标
	std::string	m_szFalseSection;	//出错跳转的段
	BOOL m_bLastFaultFlag;	//出错标志
	AI_STATE m_eAiState;
	HAND_COMMOND m_eCurCommon;
	INT m_lParam, m_wParam;
	BOOL m_bLock;
	INT m_nNowAttackX, m_nNowAttackY;
	INT m_nLastAttackX, m_nLastAttackY;
	INT m_nNormalSleep;
	//settings
	INT st_drug1pos, st_drug2pos, st_drugpro, st_freeroom, st_moneyleft, st_groundpick, st_pickitemvalue;
	INT m_nRunAwayNpc, m_nRunAwayNpcX, m_nRunAwayNpcY; 
	BOOL m_bIsChangeCurTarget;
private:
	VOID OnSee(KActor *pActor, TEvent event, UINT lParam, UINT wParam);
private:
	INT m_nAiState;
	CHAR m_szParam1[255], m_szParam2[255];
	INT m_nLastRunNormalTime;
	INT m_ErrorNum;
	INT m_nWaitForRepair;	//等待修理的黄金装备
	INT m_nNormalRunCounter;
	BOOL m_bShopOpen;
};

extern AutoAi g_cAutoAi;
// -------------------------------------------------------------------------

#endif /* __KAUTOAI_H__ */

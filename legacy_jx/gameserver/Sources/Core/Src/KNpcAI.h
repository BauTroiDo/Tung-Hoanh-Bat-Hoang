#ifndef	KNpcAIH
#define	KNpcAIH

#include "KMath.h"
#include "KItem.h"
//#include "KBasPropTbl.h"
//#include "KMagicAttrib.h"

#ifndef _SERVER
enum AutoPickUp  //脚本通知显示的界面类型
{
	PICCK_NONE,	 //空闲
	PICCK_MONEY,
    PICCK_ITEM,
	PICCK_ITEM_MONEY,
	PICCK_TUPU,
	PICCK_TOOL,
};
#endif


class	KNpcAI
{
public:
	int				m_nIndex;
	BOOL			m_bActivate;
private:
public:
	KNpcAI();
	~KNpcAI();
	void			Activate(int nIndex);
	void			Enable(){ m_bActivate = TRUE; }
	void			Disable(){ m_bActivate = FALSE; }	
	int          	GetNearestCharacter(int nRelation,int indexid,int nType=0,int nDis=0);
	BOOL            AiCheckDistance(int nYMpsX=0 ,int nYMpsY=0, int nDMpsX=0,int nDMpsY=0,int nDisten=0);
//	void			CommonAction();
//	void			CommonActionCompanion(int fanwei);
	void            ReMoveCommonAction(int nNpcIdx);

#ifndef _SERVER
    void			 FollowCharacter(int m_Index,int fanwei,int juli,int nActiveNum=3);
	void			 MoveYaBiao(int m_Index);
	void	         MoveJiaoBen(int m_Index);
	void             GotoWhere(int x, int y, int mode);
    void             Setscpos(int m_nIndex,int x=0,int y=0);
	void             SetCurpos(int m_nIndex,int x,int y);
    BOOL             KeepActiveCharacter(int fanwei,int indexid,int xx,int yy) ;
	BOOL             KeepActiveJiaoBen(int fanwei,int indexid,int xx,int yy);
    void			 FollowAttackCharacter(int i,int r,int l,int nActiveNum);
    void			 KeepAttackCharacter(int nEnemy, int nRange,int r);
	KNpcFindPath	 ai_PathFinder;
	AutoPickUp       nPickState;
	int              AutoMovePickUp(int nIndex ,int nObjIdx,int xx,int yy, AutoPickUp nPickState);
#endif

private:	
	int				GetNearestNpc(int nRelation);
//	int          	GetNearestNpcA(int nRelation,int indexid);
	int             GetNearestObject(int indexid,int nKind=-1,char *nItemName=NULL);
	int				GetNpcNumber(int nRelation);
	void			ProcessPlayer();
#ifndef _SERVER
	void			FollowPeople(int nIdx);
	void			FollowObject(int nIdx);
#else
	void			FindPathNpc();
	void			MoveLuXianAi(int m_Index=0);
#endif
	void	        ProcessChargedart();    // 押镖AI
	void	        ProcessCompanion();     // 同伴类AI
	void	        ProcessLuXianAi();      // 指定路线AI

	//void			FleeEnemy(int nEnemyIdx);  //逃离敌人
	void			ProcessAIType01();		// 普通主动类1
	void			ProcessAIType02();		// 普通主动类2
	void			ProcessAIType03();		// 普通主动类3
	void			ProcessAIType04();		// 普通被动类1
	void			ProcessAIType05();		// 普通被动类2
	void			ProcessAIType06();		// 普通被动类3

	void			TriggerObjectTrap();
	void			TriggerMapTrap();
	void			FollowAttack(int nIdx);
	void			FollowAttackCompanion(int nIdx,int juli);
    void            FollowChargedart(int nIdx);
    void            FollowCompanion(int nIdx);
	BOOL			InEyeshot(int nIdx);
	BOOL			InEyeshota(int nIdx,int r);
	void			CommonAction();
	void			CommonActionWithPlayer();
	void			CommonActionCompanion(int fanwei);
	BOOL			KeepActiveRange();
	void			KeepAttackRange(int nEnemy, int nRange);
	void			Flee(int nIdx);  //逃离敌人

	friend class KNpc;
#ifndef _SERVER
	// 装饰性质NPC运动函数系列 
	// flying add these on Jun.4.2003
	// 所有装饰性NPC运动处理入口，由Activate(int)内部调用
	int				ProcessShowNpc();
	int  			ShowNpcType11();
	int				ShowNpcType12();
	int				ShowNpcType13();
	int				ShowNpcType14();
	int				ShowNpcType15();
	int				ShowNpcType16();
	int				ShowNpcType17();
	int             GetNpcMoveOffset(int nDir, int nDistance, int *pnX, int *pnY);

	// 判断是否超出范围
	//BOOL KeepActiveShowRange();
	// 16/17 AiMode NPC的逃逸动作
	int				DoShowFlee(int nIdx);

	// 判断是否这个帧内可以给该NPC下指令
	BOOL			CanShowNpc();
#endif
	// flying add the function to get nearest player.
	int			IsPlayerCome();
};

#if !defined _SERVER
inline int KNpcAI::GetNpcMoveOffset(int nDir, int nDistance, int *pnX, int *pnY)
{
    _ASSERT(pnX);
    _ASSERT(pnY);

	if (nDir >= MaxMissleDir)
		nDir -= MaxMissleDir;

    *pnX = -nDistance * g_DirSin(nDir, MaxMissleDir);
    *pnY = -nDistance * g_DirCos(nDir, MaxMissleDir);

    return true;
}

inline BOOL KNpcAI::CanShowNpc()
{
	BOOL bResult = TRUE;
	if (Npc[m_nIndex].m_AiParam[5] < Npc[m_nIndex].m_AiParam[4])
		bResult = FALSE;
	Npc[m_nIndex].m_AiParam[5]++;
	return bResult;
}
#endif

extern KNpcAI NpcAI;
#endif


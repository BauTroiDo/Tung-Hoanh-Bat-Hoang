// KTongControl.h: interface for the CTongControl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_KTONGCONTROL_H__62D04F9A_67CD_419B_B475_BF0F8727A91E__INCLUDED_)
#define AFX_KTONGCONTROL_H__62D04F9A_67CD_419B_B475_BF0F8727A91E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


struct STONG_MEMBER
{
	DWORD	m_dwNameID;
	char	m_szName[NAME_LENGHT];
	int		m_nPoint;
	UINT	m_nLevel;
};

typedef struct
{
	char		szName[NAME_LENGHT];			// 帮会名
	int			MemberCount;						// 帮会成员数
	char		m_szMasterName[NAME_LENGHT];
	//char		m_szMasterTitle[defTONG_STR_LENGTH];	// 帮主称号
	//char		m_szDirectorTitle[defTONG_MAX_DIRECTOR][defTONG_STR_LENGTH];// 各长老称号
	//char		m_szManagerTitle[defTONG_MAX_MANAGER][defTONG_STR_LENGTH];	// 各队长称号
	//char		m_szNormalTitle[defTONG_STR_LENGTH];	// 帮会帮众称号
}TTongList;	//用作帮会列表

class CTongControl
{
	friend class CTongSet;
	friend class CTongDB;
public:
	// 用于创建帮会
	CTongControl(int nCamp, char *lpszPlayerName, char *lpszTongName);
	// 用于数据库载入帮会数据
	CTongControl(TTongList sList);
	virtual ~CTongControl();

private:

	int			m_nCamp;								// 帮会阵营
	DWORD		m_dwMoney;								// 帮会资金
	int			m_nCredit;								// 帮会声望
	int			m_nLevel;								// 帮会等级
	int         m_nTongMap;                             // 帮会地图ID		c
	int         m_nAttackNum;	                        // 参战场数
	int         m_nWinNum;								// 胜利场数
	int         m_nLoseNum;								// 输掉场数
	int			m_nDirectorNum;							// 长老人数
	int			m_nManagerNum;							// 队长人数
	int			m_nMemberNum;							// 帮众人数
	int         m_nLenMengNum;                          // 联盟数量

	DWORD		m_dwNameID;								// 帮会名ID
	char		m_szName[NAME_LENGHT];			// 帮会名
	char		m_szPassword[NAME_LENGHT];		// 帮会密码
	char		m_szAnnounce[MAX_TONG_ANNOUNCE_LENGTH];

	DWORD		m_dwMasterID;							// 帮主名ID
	char		m_szMasterTitle[NAME_LENGHT];	// 帮主称号
	char		m_szMasterName[NAME_LENGHT];		// 帮主名
	UINT		m_nMasterPoint;
	UINT		m_nMasterLevel;


	DWORD		m_dwDirectorID[defTONG_MAX_DIRECTOR];	// 各长老名ID
	char		m_szDirectorTitle[defTONG_MAX_DIRECTOR][NAME_LENGHT];// 各长老称号
	char		m_szDirectorName[defTONG_MAX_DIRECTOR][NAME_LENGHT];	// 各长老名
	UINT		m_nDirectorPoint[defTONG_MAX_DIRECTOR];
	UINT		m_nDirectorLevel[defTONG_MAX_DIRECTOR];
	bool		m_nDirectorPower[defTONG_MAX_DIRECTOR][defTONG_MAX_POWER];


	DWORD		m_dwManagerID[defTONG_MAX_MANAGER];		// 各队长名ID
	char		m_szManagerName[defTONG_MAX_MANAGER][NAME_LENGHT];	// 各队长名
	char		m_szManagerTitle[defTONG_MAX_MANAGER][NAME_LENGHT];	// 各队长称号
	UINT		m_nManagerPoint[defTONG_MAX_MANAGER];
	UINT		m_nManagerLevel[defTONG_MAX_MANAGER];


	STONG_MEMBER* m_psMember;   //成员指针 - danh sach de tu
	int			m_nMemberPointSize;  //所有帮众的 内存字节数 内存块 储蓄帮众具体信息


	char		m_szNormalTitle[NAME_LENGHT];	// 帮会帮众称号
	char		m_szManTitle[NAME_LENGHT];
	char		m_szWomanTitle[NAME_LENGHT];


	DWORD		m_dwLenMengID[defTONG_MAX_LENMENG];		// 各联盟帮会ID
	char		m_szLenMengName[defTONG_MAX_LENMENG][NAME_LENGHT];	// 各联盟帮会名


//-----------------临时不入数据库---------------------------------
	int         m_nTempCamp;                            // 帮会的临时阵营 （用于宣战）(红帮 绿帮)
	char		m_szAttackName[NAME_LENGHT];		// 宣战帮会名
	int         m_AttackState;                          // 宣战状态 0无宣战 1发起宣战放 2被宣战方
	int         m_AttackTime;                           // 宣战的时长
	int         m_nDeathCount;                          // 死亡的数量
//----------------------------------------------------------------
private:
	void		ResetMember(int nPos, TONG_MEMBER_FIGURE nFigure);
public:
	BOOL		SearchOne(char *lpszName, STONG_ONE_LEADER_INFO *pInfo);

	UINT		AddMember(char *lpszPlayerName, int nLevel, int nPoint);		// 添加一个成员

	BOOL		UpAttackTime(char *lpszPlayerName);	    //更新宣战时间

	BOOL		ChangePassword(char *lpOld, char *lpNew);	// 改变帮会密码

	BOOL		GetTongHeadInfo(STONG_HEAD_INFO_SYNC *pInfo);	// 帮会信息

	BOOL		GetTongAttackInfo(STONG_ATTACK_INFO_SYNC *pInfo);	// 帮会宣战结构信息

	BOOL		GetTongManagerInfo(STONG_GET_MANAGER_INFO_COMMAND *pApply, STONG_MANAGER_INFO_SYNC *pInfo);

	BOOL		GetTongMemberInfo(STONG_GET_MEMBER_INFO_COMMAND *pApply, STONG_MEMBER_INFO_SYNC *pInfo);

	BOOL		Instate(STONG_INSTATE_COMMAND *pInstate, STONG_INSTATE_SYNC *pSync);

	BOOL		Kick(STONG_KICK_COMMAND *pKick, STONG_KICK_SYNC *pSync);

	BOOL		Leave(STONG_LEAVE_COMMAND *pLeave, STONG_LEAVE_SYNC *pSync);

	BOOL		AcceptMaster(STONG_ACCEPT_MASTER_COMMAND *pAccept);

	BOOL		GetLoginData(STONG_GET_LOGIN_DATA_COMMAND *pLogin, STONG_LOGIN_DATA_SYNC *pSync);

	BOOL		DBInstate(char *lpszPlayerName, BYTE nSite);

	BOOL		ChangeMoneyControl(STONG_MONEY_COMMAND* pMoney,STONG_MONEY_SYNC *Sync);

	BOOL        AddMoneyControl(int nMoney);

	BOOL		DBChangeMoney(DWORD dwMoney) { m_dwMoney = dwMoney;};

//	BOOL        FlieWriteIni(char *nSection,char *nKeyName);

	BOOL		SetPower(STONG_POWER_COMMAND*, STONG_POWER_SYNC*);

	BOOL		GetPower(STONG_GET_POWER_COMMAND*, STONG_POWER_SYNC*);

	BOOL		GetTopPointControl(STONG_SEARCH_INFO_COMMAND* pSearch, STONG_SEARCH_INFO_SYNC* pSync);

	BOOL		GetTopLevelControl(STONG_SEARCH_INFO_COMMAND* pSearch, STONG_SEARCH_INFO_SYNC* pSync);

	BOOL		GetOnOffControl(STONG_SEARCH_INFO_COMMAND* pSearch, STONG_SEARCH_INFO_SYNC* pSync);

	void		SortList(std::vector<std::pair<DWORD, UINT>> &mList);

	BOOL        UpdateLevel(STONG_UPDATE_LEVEL_COMMAND* pUpdate);

	TONG_MEMBER_FIGURE CTongControl::GetMemberFigure(DWORD dwID, BYTE& nPos);
};

#endif // !defined(AFX_KTONGCONTROL_H__62D04F9A_67CD_419B_B475_BF0F8727A91E__INCLUDED_)

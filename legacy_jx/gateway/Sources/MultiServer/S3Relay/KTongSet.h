// KTongSet.h: interface for the CTongSet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_KTONGSET_H__B42782F1_FA08_4D1C_A209_1ED1F5E0BAA3__INCLUDED_)
#define AFX_KTONGSET_H__B42782F1_FA08_4D1C_A209_1ED1F5E0BAA3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include	"KTongControl.h"

#define		defTONG_SET_INIT_POINT_NUM		100   //帮会数组字节内存限制

struct TongSettings
{
	char m_szMasterTitle[NAME_LENGHT];
	char m_szDirectorTitle[NAME_LENGHT];
	char m_szManagerTitle[NAME_LENGHT];
	char m_szMemberTitle[NAME_LENGHT];
	UINT m_nPointPerMoney;
	UINT m_nCampPrice;
};

class CTongSet
{
public:
	CTongSet();
	virtual ~CTongSet();

private:
	CTongControl**	m_pcTong;
	int				m_nTongPointSize;		// 指针 m_pcTong 当前分配内存的大小(多少个)

public:
	void			Init();
	void			DeleteAll();
	BOOL			InitFromDB();

	BOOL			SearchOne(DWORD dwTongNameID, char *lpszName, STONG_ONE_LEADER_INFO *pInfo);

	// 创建一个帮会
	int				Create(int nCamp, char *lpszPlayerName, char *lpszTongName, int nPlayerLevel);
	// 添加一个帮会成员，if return == 0 成功 else return error id
	int				AddMember(char *lpszPlayerName, char *lpszTongName, int nPlayerLevel);
	// 实时更新 帮战时间 判断输赢
	int             UpTongAttackTime(int *nBlueTongIdx,int *nGreenTongIdx);
	//发送宣战
	BOOL			SetTongAttackInfo(DWORD dwYTongNameID=0,DWORD dwDTongNameID=0);
	//首次随机选择帮会地图归属

	void            SynTongWarInfo();//同步帮战信息

     int 	        RandomSelTongmap();

	void            SynWarCityInfo();//同步七城信息

	void            SynAttackInfo();//同步帮战信息

	void            SynAttackStime(TCHAR *nTime);

	BOOL		    SetTongLevel(DWORD dwTongNameID,int nLevel);	      //等级
	BOOL		    SetTongCredit(DWORD dwTongNameID,int nCredit);        //建设资金
	BOOL		    SetTongMap(DWORD dwTongNameID,int nMapIdx);	          //地图
	BOOL		    SetTongMasterTitle(DWORD dwTongNameID,char *nTitle);  //帮主称号
	BOOL		    SetTongDirectorTitle(DWORD dwTongNameID,char *nTitle, int nPos);//长老称号
	BOOL		    SetTongManagerTitle(DWORD dwTongNameID,char *nTitle, int nPos); //队长称号
	BOOL		    SetTongNormalTitle(DWORD dwTongNameID,char *nTitle, int nGender);  //帮众称号
	BOOL		    SetTongKillNum(DWORD dwTongNameID,int nCount);	      //设置死亡次数
	BOOL		    SetTongWinNum(DWORD dwTongNameID,int nCount);	      //设置胜利次数
	BOOL		    SetTongLoseNum(DWORD dwTongNameID,int nCount);	      //设置失败次数
	BOOL			SetTongCam(DWORD dwTongNameID, int CamId);
	//无限获取帮会 以及 帮战信息 城战信息
	BOOL            GetTongAttackInfo(int dwTongindex, STONG_ATTACK_INFO_SYNC *pInfo);

	int             GetAllMenberCount(DWORD dwTongNameID);	              //获取全部人数数量
	int				GetTongCamp(int nTongIdx);
    int				GetTongCount();
	int             GetTongDeathCount(DWORD dwTongNameID);
	int				GetTongMapByIdx(int nTongIdx);


	BOOL            UpDataTongInfo(DWORD dwTongNameID);

	BOOL			GetTongName(int nTongIdx, char *lpszName);

	int             GetTongIdxByNameId(DWORD dwTongNameID);

	BOOL	        GetTongNameById(DWORD dwTongNameID, char *lpszName);

	BOOL			GetMasterName(int nTongIdx, char *lpszName);

	BOOL			GetMasterTitle(char* lpszTongName, char* lpszMasterTitle);

	BOOL			GetMemberTitle(int nTongIdx, char *lpszTitle, int nGender);

	BOOL			GetTongHeadInfo(DWORD dwTongNameID, STONG_HEAD_INFO_SYNC *pInfo);

	BOOL			GetTongManagerInfo(STONG_GET_MANAGER_INFO_COMMAND *pApply, STONG_MANAGER_INFO_SYNC *pInfo);

	BOOL			GetTongMemberInfo(STONG_GET_MEMBER_INFO_COMMAND *pApply, STONG_MEMBER_INFO_SYNC *pInfo);

	BOOL			Instate(STONG_INSTATE_COMMAND *pInstate, STONG_INSTATE_SYNC *pSync);

	BOOL			Kick(STONG_KICK_COMMAND *pKick, STONG_KICK_SYNC *pSync);

	BOOL			Leave(STONG_LEAVE_COMMAND *pLeave, STONG_LEAVE_SYNC *pSync);

	BOOL			AcceptMaster(STONG_ACCEPT_MASTER_COMMAND *pAccept);

	BOOL			GetLoginData(STONG_GET_LOGIN_DATA_COMMAND *pLogin, STONG_LOGIN_DATA_SYNC *pSync);

	BOOL			ChangeMoney(STONG_MONEY_COMMAND *pMoney,STONG_MONEY_SYNC *Sync);

	BOOL		    GetTongListInfo(STONG_GET_LIST_INFO_COMMAND *pApply, STONG_LIST_INFO_SYNC *pInfo);

	BOOL			SetPower(STONG_POWER_COMMAND*, STONG_POWER_SYNC*);

	BOOL			GetPower(STONG_GET_POWER_COMMAND*, STONG_POWER_SYNC*);

	BOOL			SearchInfo(STONG_SEARCH_INFO_COMMAND* pSearch, STONG_SEARCH_INFO_SYNC* pSync);

	BOOL			UpdateLevel(STONG_UPDATE_LEVEL_COMMAND* pUpdate);

	BOOL			ChangeAnnounce(DWORD dwTongNameID, CHAR* szAnnounce);
public:
};

extern TongSettings g_TongSetting;

#endif // !defined(AFX_KTONGSET_H__B42782F1_FA08_4D1C_A209_1ED1F5E0BAA3__INCLUDED_)

// TongConnect.h: interface for the CTongConnect class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TONGCONNECT_H__A8D8F663_D02F_4F00_BD05_033EB4081E3E__INCLUDED_)
#define AFX_TONGCONNECT_H__A8D8F663_D02F_4F00_BD05_033EB4081E3E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NetConnect.h"
#include <map>

class CTongConnect : public CNetConnect  
{
public:
	//CNetConnect *gGs;
	CTongConnect(class CTongServer* pHostServer, unsigned long id);
	virtual ~CTongConnect();
//	int    nS3loop;
	UINT   nTondNetIdx;

protected:
	virtual void OnClientConnectCreate();
	virtual void OnClientConnectClose();

	virtual void RecvPackage(const void* pData, size_t size);

private:
	unsigned int nSerVerIndex;
	unsigned int nGsNumBer;     //GS的编号 区分同一个区下的gs
	typedef std::map<INT,INT>	SERMAP;
	SERMAP m_serMaps;

	struct ACCINFO
	{
		std::_tstring role;
		DWORD nameID;
		unsigned long param;
	};
	typedef std::map<std::_tstring, ACCINFO, _tstring_iless> ACCMAP;
	ACCMAP m_mapAcc;

	struct ROLEINFO
	{
		std::_tstring acc;
		DWORD nameID;
		unsigned long param;
	};
	typedef std::map<std::_tstring, ROLEINFO, _tstring_less>	ROLEMAP;
	ROLEMAP m_mapRole;

	struct PARAMINFO
	{
		std::_tstring acc;
		std::_tstring role;
		DWORD nameID;
	};
	typedef std::map<unsigned long, PARAMINFO>	PARAMMAP;
	PARAMMAP m_mapParam;

	typedef std::map<UINT, UINT >		stdTongServer;
	stdTongServer				m_sTongServer;
private:
	void Proc0_Normal(const void* pData, size_t size);
	void Proc1_Normal_EnterGame(const void* pData, size_t size);
	void Proc1_Normal_LeaveGame(const void* pData, size_t size);

	int Proc0_Tong(const void* pData, size_t size);
	int Proc0_allTong(const void* pData, size_t size);
	void proc_AllPlayer();  //发往GS

	void Proc0_ServerCommunity(const void* pData, size_t size);
	void Proc0_Friend(const void* pData, size_t size);
	void Proc1_Friend_AskAddFriend(const void* pData, size_t size);
	void Proc1_Friend_RepAddFriend(const void* pData, size_t size);
	void Proc1_Friend_GroupFriend(const void* pData, size_t size);
	void Proc1_Friend_EraseFriend(const void* pData, size_t size);
	void Proc1_Friend_AskSyncFriendList(const void* pData, size_t size);
	void Proc1_Friend_Associate(const void* pData, size_t size);
	void Proc1_Friend_AssociateBevy(const void* pData, size_t size);
private:
	BOOL PassToSomeone(DWORD ip, unsigned long id, DWORD nameid, const void* pData, size_t size);

public:
	BOOL FindPlayerByAcc(const std::_tstring& acc, std::_tstring* pRole, DWORD* pNameID, unsigned long* pParam, std::_tstring* pRealAcc = NULL);
	BOOL FindPlayerByRole(const std::_tstring& role, std::_tstring* pAcc, DWORD* pNameID, unsigned long* pParam, std::_tstring* pRealRole = NULL);
	BOOL FindPlayerByParam(unsigned long param, std::_tstring* pAcc, std::_tstring* pRole, DWORD* pNameID);

};

#endif // !defined(AFX_TONGCONNECT_H__A8D8F663_D02F_4F00_BD05_033EB4081E3E__INCLUDED_)

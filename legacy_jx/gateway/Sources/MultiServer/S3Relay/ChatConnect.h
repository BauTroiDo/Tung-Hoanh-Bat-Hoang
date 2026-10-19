// ChatConnect.h: interface for the CChatConnect class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHATCONNECT_H__5ED21755_43E7_4141_A572_2506BF7C31BA__INCLUDED_)
#define AFX_CHATCONNECT_H__5ED21755_43E7_4141_A572_2506BF7C31BA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NetConnect.h"
#include <map>

class CChatConnect : public CNetConnect  
{
public:
	CChatConnect(class CChatServer* pHostServer, unsigned long id);
	virtual ~CChatConnect();

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

	typedef std::map<UINT, UINT >		stdChatServer;
	stdChatServer				m_sChatServer;

private:
	void Proc0_Normal(const void* pData, size_t size);
	void Proc1_Normal_EnterGame(const void* pData, size_t size);
	void Proc1_Normal_LeaveGame(const void* pData, size_t size);

	void Proc0_ServerCommunity(const void* pData, size_t size);
	void Proc0_PlayerCommunity(const void* pData, size_t size);
	void Proc0_SomeoneChat(const void* pData, size_t size);
	void Proc0_ChannelChat(const void* pData, size_t size);
	void Proc0_GroupMan(const void* pData, size_t size);

	void Proc1_PlayerCommunity_QueryChannelID(const void* pData, size_t size);
	void Proc1_PlayerCommunity_FreeChannelID(const void* pData, size_t size);
	void Proc1_PlayerCommunity_Subscribe(const void* pData, size_t size);
public:
	BOOL FindPlayerByAcc(const std::_tstring& acc, std::_tstring* pRole, DWORD* pNameID, unsigned long* pParam, std::_tstring* pRealAcc = NULL);
	BOOL FindPlayerByRole(const std::_tstring& role, std::_tstring* pAcc, DWORD* pNameID, unsigned long* pParam, std::_tstring* pRealRole = NULL);
	BOOL FindPlayerByParam(unsigned long param, std::_tstring* pAcc, std::_tstring* pRole, DWORD* pNameID);
};

#endif // !defined(AFX_CHATCONNECT_H__5ED21755_43E7_4141_A572_2506BF7C31BA__INCLUDED_)

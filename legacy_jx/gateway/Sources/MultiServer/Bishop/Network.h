/********************************************************************
	created:	2003/05/05
	file base:	Network
	file ext:	h
	author:		liupeng
	
	purpose:	
*********************************************************************/
#ifndef __INCLUDE_NETWORK_H__
#define __INCLUDE_NETWORK_H__

#pragma once

#include "IClient.h"
#include "RainbowInterface.h"

#include "IServer.h"
#include "HeavenInterface.h"

#include "Library.h"
#include "Buffer.h"
#include "tstring.h"
#include "CriticalSection.h"

class CNetwork
{
public:

	CNetwork();
	virtual ~CNetwork();

	bool	Create();
	void	Destroy();
	void    SaveSet(UINT nVal=0);
	void    SaveOpenCount(UINT nVal=0);
	void    SaveVerSet(LPCTSTR nVal);
	void    SaveAccList(LPCTSTR lpSectionName,LPCTSTR lpKeyName,LPCTSTR lpString,_tstring _saveFile);
	void    SaveIpList(LPCTSTR lpSectionName,LPCTSTR lpKeyName,LPCTSTR lpString,int nCount);
	void    ClearAccList(LPCTSTR lpSectionName,_tstring _saveFile);
	void    ClearIpList(LPCTSTR lpSectionName);
	int     GetAccListCount(_tstring _saveFile);
	int     GetIpListCount();


	char* GetAccString(unsigned int nSelIndex,_tstring _saveFile);
	char* GetIpString(unsigned int nSelIndex);

//	IClient *CreateAccSvrClient( const char * const pAddressToConnectServer, 
//		unsigned short usPortToConnectServer );

	const char *GetAccSvrIP() { return m_sAccSvrIP.c_str(); }
	unsigned short GetAccSvrPort() { return m_nAccSvrPort; }
	
	const char *GetRoleSvrIP() { return m_sRoleSvrIP.c_str(); }
	void SetRoleSvrIP( const char *pAddress ) { if ( pAddress ) { m_sRoleSvrIP = pAddress; } }
	unsigned short GetRoleSvrPort() { return m_nRoleSvrPort; }
	void SetRoleSvrPort( unsigned short nRoleSvrPort ) { m_nRoleSvrPort = nRoleSvrPort; }

	unsigned short GetClientOpenPort() { return m_nClientOpenPort; }
	void SetClientOpenPort( unsigned short nClientOpenPort ) { m_nClientOpenPort = nClientOpenPort; }

	unsigned short GetGameSvrOpenPort() { return m_nGameSvrOpenPort; }
	void SetGameSvrOpenPort( unsigned short nGameSvrOpenPort ) { m_nGameSvrOpenPort = nGameSvrOpenPort; }

	unsigned short GetTwoRevivrPos() { return m_nTwoRevivrPos; }
	void SetTwoRevivrPos( unsigned short nTwoRevivrPos ) { m_nTwoRevivrPos = nTwoRevivrPos; }

	unsigned short GetMaxPlayerCount() { return m_nMaxPlayerCount;}
	void SetMaxPlayerCount( unsigned short nMaxPlayerCount ) { m_nMaxPlayerCount = nMaxPlayerCount; }

	double GetBishopVer() { return m_nBishopVer; }
	void SetBishopVer( double nTwoRevivrPos ) { m_nBishopVer = nTwoRevivrPos; }

	unsigned short GetOpenCount() { return m_nForBitOpenCount;}
	void SetOpenCount( unsigned short nForBitOpenCount ) { m_nForBitOpenCount = nForBitOpenCount; }


protected:

	typedef HRESULT ( __stdcall * pfnCreateServerInterface )( REFIID riid, void **ppv );
	typedef HRESULT ( __stdcall * pfnCreateClientInterface )( REFIID riid, void **ppv );

//	static CLibrary	m_theHeavenLib;
//	static CLibrary	m_theRainbowLib;

//	static void __stdcall AccSvrEventNotify( LPVOID lpParam, const unsigned long &ulnEventType );
//	void	DistroyAccSvrClient();
	
private:

	/*
	 * Account server config
	 */
//	IClient		*m_pAccSvrClient;

	_tstring		m_sAccSvrIP;
	unsigned short	m_nAccSvrPort;

	/*
	 * Role server config
	 */
	_tstring		m_sRoleSvrIP;
	unsigned short	m_nRoleSvrPort;

	/*
	 * Client config
	 */
	unsigned short	m_nClientOpenPort;

	/*
	 * Game server config
	 */
	unsigned short	m_nGameSvrOpenPort;
	
	unsigned short	m_nTwoRevivrPos;
	unsigned short	m_nMaxPlayerCount;

	unsigned short	m_nForBitOpenCount;	   //限制多开数量

	double	m_nBishopVer;
};

#endif // __INCLUDE_NETWORK_H__
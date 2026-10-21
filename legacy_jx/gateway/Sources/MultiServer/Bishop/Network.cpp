#include "Stdafx.h"

#include <objbase.h>
#include <initguid.h>

#include "Network.h"
#include "Macro.h"
#include "Inifile.h"
#include "Utils.h"
#include "tstring.h"
/*
using CIniFile;
using GetAppFullPath;
using _tstring;
using CCriticalSection;

//CLibrary				CNetwork::m_theHeavenLib( "heaven.dll" );
//CLibrary				CNetwork::m_theRainbowLib( "rainbow.dll" );
*/
#define MAX_STRING_LEN	100

void bcharToDouble(const char* ch, double& res)
{
   sscanf(ch,"%lf",&res);
}

CNetwork::CNetwork()
	: /*m_pAccSvrClient( NULL )
	, */m_nAccSvrPort( 0 )
	, m_nRoleSvrPort( 0 )
	, m_nClientOpenPort( 0 )
	, m_nGameSvrOpenPort( 0 )
	, m_nTwoRevivrPos(0)
	, m_nMaxPlayerCount(500)
	, m_nBishopVer(7.0)
	, m_nForBitOpenCount(0)
{

}

CNetwork::~CNetwork()
{

}

bool CNetwork::Create()
{
	CIniFile theConfigFile;

	_tstring sAppPath = GetAppFullPath( NULL );

	_tstring sConfigFile;

	sConfigFile = sAppPath + "Bishop.cfg";

	theConfigFile.SetFile( sConfigFile.c_str() );

	m_sRoleSvrIP.resize( MAX_STRING_LEN );

	DWORD dwLen = theConfigFile.ReadString( "Network", "RoleSvrIP", const_cast< char * >( m_sRoleSvrIP.c_str() ), MAX_STRING_LEN, "" );
	m_sRoleSvrIP.resize( dwLen + 1 /* '\0' */ );

	m_nRoleSvrPort = theConfigFile.ReadInteger( "Network", "RoleSvrPort", 0 );

	m_sAccSvrIP.resize( MAX_STRING_LEN );

	dwLen = theConfigFile.ReadString( "Network", "AccSvrIP", const_cast< char * >( m_sAccSvrIP.c_str() ), MAX_STRING_LEN, "" );
	m_sAccSvrIP.resize( dwLen + 1 /* '\0' */ );

	m_nAccSvrPort = theConfigFile.ReadInteger( "Network", "AccSvrPort", 0 );

	m_nClientOpenPort  = theConfigFile.ReadInteger( "Network", "ClientOpenPort", 0 );
	m_nGameSvrOpenPort = theConfigFile.ReadInteger( "Network", "GameSvrOpenPort", 0 );
	char trustedGsIPs[512] = {0};
	theConfigFile.ReadString("Network", "TrustedGameServerIPs", trustedGsIPs, sizeof(trustedGsIPs), "");
	trustedGsIPs[sizeof(trustedGsIPs)-1] = 0;
	m_sTrustedGameServerIPs = trustedGsIPs;

	m_nTwoRevivrPos    = theConfigFile.ReadInteger( "Network", "TwoRevivePos", 0 );

	m_nMaxPlayerCount  = theConfigFile.ReadInteger( "Network", "bsMaxPlayerCount", 0 );

	m_nForBitOpenCount = theConfigFile.ReadInteger( "Network", "ForBitOpenCount",0);

	char nstr[16];
	theConfigFile.ReadString("Network","BishopVer",nstr,16,"7.0");
	bcharToDouble(nstr,m_nBishopVer);
	//m_nBishopVer       = atof(nstr) ;//theConfigFile.ReadInteger( "Network", "BishopVer", 0 );

	return true;
}

void CNetwork::SaveOpenCount(UINT nVal)
{
	CIniFile theConfigFile;
	
	_tstring sAppPath = GetAppFullPath( NULL );
	
	_tstring sConfigFile;
	
	sConfigFile = sAppPath + "Bishop.cfg";
	theConfigFile.SetFile( sConfigFile.c_str() );
	
    theConfigFile.WriteInteger( "Network", "ForBitOpenCount", nVal);
}


void CNetwork::SaveSet(UINT nVal)
{
	CIniFile theConfigFile;
	
	_tstring sAppPath = GetAppFullPath( NULL );
	
	_tstring sConfigFile;
	
	sConfigFile = sAppPath + "Bishop.cfg";
	theConfigFile.SetFile( sConfigFile.c_str() );

    theConfigFile.WriteInteger( "Network", "TwoRevivePos", nVal);
}

void CNetwork::SaveVerSet(LPCTSTR nVal)
{
	CIniFile theConfigFile;
	
	_tstring sAppPath = GetAppFullPath( NULL );
	
	_tstring sConfigFile;
	sConfigFile = sAppPath + "Bishop.cfg";
	theConfigFile.SetFile(sConfigFile.c_str());
	//GetSectionCount();
    theConfigFile.WriteString("Network", "BishopVer", nVal);
}



void CNetwork::SaveAccList(LPCTSTR lpSectionName,LPCTSTR lpKeyName,LPCTSTR lpString,_tstring _saveFile)
{
	CIniFile theConfigFile;
	
	_tstring sAppPath = GetAppFullPath( NULL );
	
	_tstring sConfigFile;
	sConfigFile = sAppPath + _saveFile;//"accforbitlist.cfg";
	theConfigFile.SetFile(sConfigFile.c_str());
	//GetSectionCount();
    theConfigFile.WriteString(lpSectionName,lpKeyName,lpString);

}


void CNetwork::ClearAccList(LPCTSTR lpSectionName,_tstring _saveFile)
{
	CIniFile theConfigFile;
	
	_tstring sAppPath = GetAppFullPath( NULL );
	
	_tstring sConfigFile;
	sConfigFile = sAppPath +_saveFile;// "accforbitlist.cfg";
	theConfigFile.SetFile(sConfigFile.c_str());
	int nCount=theConfigFile.GetSectionCount();
	for (int i=0;i<nCount;++i)
	{
	    char nSecName[32];
		snprintf(nSecName, sizeof(nSecName),"%s_%d",lpSectionName,i);
        theConfigFile.WriteString(nSecName,NULL, NULL);
	}
}


//×ª»»³É×Ö·û´® ÑÕÉ«
char* CNetwork::GetAccString(unsigned int nSelIndex,_tstring _saveFile)
{
	CIniFile theConfigFile;
	
	_tstring sAppPath = GetAppFullPath( NULL );
	
	_tstring sConfigFile;
	sConfigFile = sAppPath +_saveFile;// "accforbitlist.cfg";
	theConfigFile.SetFile(sConfigFile.c_str());

	char nSecName[32];
	sprintf(nSecName,"accname_%d",nSelIndex);
    static	char szName[32];
	theConfigFile.ReadString(nSecName,"name",szName,32,"");

	return szName;
}


int  CNetwork::GetAccListCount(_tstring _saveFile)
{
	CIniFile theConfigFile;
	
	_tstring sAppPath = GetAppFullPath( NULL );
	
	_tstring sConfigFile;
	sConfigFile = sAppPath +_saveFile;// "accforbitlist.cfg";
	theConfigFile.SetFile(sConfigFile.c_str());
	int nCount=theConfigFile.GetSectionCount();
    return 	nCount;
}

void CNetwork::SaveIpList(LPCTSTR lpSectionName,LPCTSTR lpKeyName,LPCTSTR lpString,int nCount)
{
	CIniFile theConfigFile;
	
	_tstring sAppPath = GetAppFullPath( NULL );
	
	_tstring sConfigFile;
	sConfigFile = sAppPath + "ipforbitlist.cfg";
	theConfigFile.SetFile(sConfigFile.c_str());
	//GetSectionCount();
    theConfigFile.WriteString(lpSectionName,lpKeyName,lpString);
	
}



void CNetwork::ClearIpList(LPCTSTR lpSectionName)
{
	CIniFile theConfigFile;
	
	_tstring sAppPath = GetAppFullPath( NULL );
	
	_tstring sConfigFile;
	sConfigFile = sAppPath + "ipforbitlist.cfg";
	theConfigFile.SetFile(sConfigFile.c_str());
	int nCount=theConfigFile.GetSectionCount();
	for (int i=0;i<nCount;++i)
	{
		char nSecName[32];
		snprintf(nSecName, sizeof(nSecName),"%s_%d",lpSectionName,i);
        theConfigFile.WriteString(nSecName,NULL, NULL);
	}
}


int  CNetwork::GetIpListCount()
{
	CIniFile theConfigFile;
	
	_tstring sAppPath = GetAppFullPath( NULL );
	
	_tstring sConfigFile;
	sConfigFile = sAppPath + "ipforbitlist.cfg";
	theConfigFile.SetFile(sConfigFile.c_str());
	int nCount=theConfigFile.GetSectionCount();
    return 	nCount;
}

char* CNetwork::GetIpString(unsigned int nSelIndex)
{
	CIniFile theConfigFile;
	
	_tstring sAppPath = GetAppFullPath( NULL );
	
	_tstring sConfigFile;
	sConfigFile = sAppPath + "ipforbitlist.cfg";
	theConfigFile.SetFile(sConfigFile.c_str());
	
	char nSecName[32];
	sprintf(nSecName,"ip_%d",nSelIndex);
    static	char szName[32];
	theConfigFile.ReadString(nSecName,"ip",szName,32,"");
	
	return szName;
}


void CNetwork::Destroy()
{
/*	if ( m_pAccSvrClient )
	{
		m_pAccSvrClient->Cleanup();
	}

	SAFE_RELEASE( m_pAccSvrClient );
*/
/*	CIniFile theConfigFile;

	_tstring sAppPath = GetAppFullPath( NULL );

	_tstring sConfigFile;

	sConfigFile = sAppPath + "Bishop.cfg";

	theConfigFile.SetFile( sConfigFile.c_str() );

	theConfigFile.WriteString( "Network", "AccSvrIP", m_sAccSvrIP.c_str() );
	theConfigFile.WriteInteger( "Network", "AccSvrPort", m_nAccSvrPort );

	theConfigFile.WriteString( "Network", "RoleSvrIP", m_sRoleSvrIP.c_str() );
	theConfigFile.WriteInteger( "Network", "RoleSvrPort", m_nRoleSvrPort );

	theConfigFile.WriteInteger( "Network", "ClientOpenPort", m_nClientOpenPort );
	theConfigFile.WriteInteger( "Network", "GameSvrOpenPort", m_nGameSvrOpenPort );*/
}

//IClient *CNetwork::CreateAccSvrClient( const char * const pAddressToConnectServer, 
//									  unsigned short usPortToConnectServer )
//{
//	if ( m_pAccSvrClient )
//	{
//		IClient *pClonClient = NULL;
//		m_pAccSvrClient->QueryInterface( IID_IESClient, reinterpret_cast< void ** >( &pClonClient ) );
//
//		return pClonClient;
//	}
//
//	/*
//	 * There is connectted the heaven by the rainbow
//	 */
//	pfnCreateClientInterface pClientFactroyFun = ( pfnCreateClientInterface )( m_theRainbowLib.GetProcAddress( _T( "CreateInterface" ) ) );
//	
//	IClientFactory *pClientFactory = NULL;
//
//	if ( pClientFactroyFun && SUCCEEDED( pClientFactroyFun( IID_IClientFactory, reinterpret_cast< void ** >( &pClientFactory ) ) ) )
//	{
//		pClientFactory->SetEnvironment( 1024 * 1024 );
//		
//		pClientFactory->CreateClientInterface( IID_IESClient, reinterpret_cast< void ** >( &m_pAccSvrClient ) );
//		
//		SAFE_RELEASE( pClientFactory );
//	}
//
//	if ( m_pAccSvrClient )
//	{
//		m_pAccSvrClient->Startup();
//		
//		m_pAccSvrClient->RegisterMsgFilter( reinterpret_cast< void * >( this ), AccSvrEventNotify );
//
//		if ( FAILED( m_pAccSvrClient->ConnectTo( pAddressToConnectServer, usPortToConnectServer ) ) )
//		{
//			m_pAccSvrClient->Cleanup();
//
//			SAFE_RELEASE( m_pAccSvrClient );
//
//			return NULL;
//		}
//		
//		/*
//		 * Recorder
//		 */
//		m_sAccSvrIP = pAddressToConnectServer;
//		m_nAccSvrPort = usPortToConnectServer;
//
//		IClient *pClonClient = NULL;
//		m_pAccSvrClient->QueryInterface( IID_IESClient, reinterpret_cast< void ** >( &pClonClient ) );
//
//		return pClonClient;
//	}
//
//	return NULL;
//}

/*void __stdcall CNetwork::AccSvrEventNotify( LPVOID lpParam, const unsigned long &ulnEventType )
{
	CNetwork *pThis = reinterpret_cast< CNetwork * >( lpParam );

	ASSERT( lpParam );

	switch ( ulnEventType )
	{
	case enumServerConnectCreate:

		::MessageBeep( -1 );

		break;

	case enumServerConnectClose:

		if ( pThis )
		{
			pThis->DistroyAccSvrClient();
		}
		
		break;
	}
}

void CNetwork::DistroyAccSvrClient()
{
	SAFE_RELEASE( m_pAccSvrClient );
}*/

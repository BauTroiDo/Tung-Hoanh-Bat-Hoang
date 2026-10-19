#include "stdafx.h"
#include "IniFile.h"
#include "Utils.h"
#include "Macro.h"

#define TCS_STRINGBUFFER	2048
#define TCS_KEYBUFFER		256		// This buffer length for store key value
#define TCS_INTBUFFER		16		// This buffer length for store integer value

/*
 * namespace OnlineGameLib::Win32
 */

//namespace OnlineGameLib {
//namespace Win32 {

CIniFile::CIniFile()
{
	memset( m_szFileName, 0, MAX_PATH );
}

CIniFile::CIniFile( LPCTSTR pFileName )
{
	ASSERT( pFileName );
	
	strcpy( m_szFileName, pFileName );
}

CIniFile::~CIniFile()
{
}

void CIniFile::SetFile( LPCTSTR pFileName )
{
	ASSERT( pFileName );
	
	strcpy( m_szFileName, pFileName );	
}

int	 CIniFile::ReadInteger( LPCTSTR lpSectionName, LPCTSTR lpKeyName, int nDefault )
{
	char szStr[TCS_KEYBUFFER];
	szStr[0] = 0;
	
	ReadString( lpSectionName, lpKeyName, szStr, TCS_KEYBUFFER, NULL );

	if( 0 != szStr[0] )
	{
		return atoi( szStr );
	}

	return nDefault;	
}

BOOL CIniFile::WriteInteger( LPCTSTR lpSectionName, LPCTSTR lpKeyName, int nValue ) //写进文件
{
	char szStrOut[TCS_INTBUFFER];

	snprintf( szStrOut, sizeof(szStrOut), "%d", nValue );

	return ::WritePrivateProfileString( lpSectionName, lpKeyName, szStrOut, m_szFileName );
}

DWORD CIniFile::ReadHexNum( LPCTSTR lpSectionName, LPCTSTR lpKeyName, DWORD dwDefault )
{
	char szStr[TCS_KEYBUFFER];
	szStr[0] = 0;
	
	ReadString( lpSectionName, lpKeyName, szStr, TCS_KEYBUFFER, NULL );

	if( 0 != szStr[0] )
	{
		return HexStringToDWORD( reinterpret_cast< LPCTSTR >( szStr ) );
	}

	return dwDefault;	
}

BOOL CIniFile::WriteHexNum( LPCTSTR lpSectionName, LPCTSTR lpKeyName, DWORD dwValue )
{
	char szStrOut[TCS_INTBUFFER];

	snprintf( szStrOut, sizeof(szStrOut), "%8.8X", dwValue );

	return ::WritePrivateProfileString( lpSectionName, lpKeyName, szStrOut, m_szFileName );
}

DWORD CIniFile::ReadString( LPCTSTR lpSectionName, LPCTSTR lpKeyName, LPTSTR lpString, DWORD dwSize, LPCTSTR lpDefault )
{
	ASSERT( lpSectionName && lpKeyName );

	return ::GetPrivateProfileString( lpSectionName, lpKeyName, lpDefault, lpString, dwSize, m_szFileName );
}

BOOL CIniFile::WriteString( LPCTSTR lpSectionName, LPCTSTR lpKeyName, LPCTSTR lpString )
{
	ASSERT( lpSectionName && lpKeyName );

	return ::WritePrivateProfileString( lpSectionName, lpKeyName, lpString, m_szFileName );
}

BOOL CIniFile::ReadSections( _VETSTR& stdSections )
{
	LPVOID pvData = NULL;

	HGLOBAL hGlobal = ::GlobalAlloc( GMEM_MOVEABLE, 16385 );

	ASSERT( hGlobal );

	pvData = ::GlobalLock( hGlobal );

	ASSERT( pvData );

	stdSections.clear();

	if ( ::GetPrivateProfileString( NULL, NULL, NULL, ( char * )pvData, 16384, m_szFileName ) )
	{
        char *pSection = ( char * ) pvData;

        while ( 0 != *pSection )
		{
			stdSections.push_back( pSection );

			pSection += strlen( pSection ) + 1;
		}
	}

	::GlobalUnlock( hGlobal );
	::GlobalFree( hGlobal );

	return ( BOOL )( stdSections.size() > 0 );
}

BOOL CIniFile::ReadSection( LPCTSTR lpSection, _VETSTR& stdKey )
{
	LPVOID pvData = NULL;

	HGLOBAL hGlobal = ::GlobalAlloc( GMEM_MOVEABLE, 16385 );

	ASSERT( hGlobal );

	pvData = ::GlobalLock( hGlobal );

	ASSERT( pvData );

	stdKey.clear();
//	GetPrivateProfileString("UPDATE","server","暂无游戏资料",server.GetBuffer(MAX_PATH),MAX_PATH,strPath+_T("JXCdata.ini")); 
	if ( ::GetPrivateProfileString( lpSection, NULL, NULL, ( char * ) pvData, 16384, m_szFileName ) )
	{
        char *pKey = ( char * )pvData;

        while ( 0 != *pKey )
		{
			stdKey.push_back( pKey );

			pKey += strlen( pKey ) + 1;
		}
	}

	::GlobalUnlock( hGlobal );
	::GlobalFree( hGlobal );

	return ( BOOL )( stdKey.size() > 0 );
}

int CIniFile::GetSectionCount()
{
	TCHAR chSectionNames[2048] = {0};    
    TCHAR *pSectionName=NULL;   // 保存找到的某个节名字符串的首地址。  
    int i,j=0;               // j用来保存下一个节名字符串的首地址相对于当前i的位置偏移量。  
    int count = 0;         // 统计节的个数。  
	
    ::GetPrivateProfileSectionNames(chSectionNames,2048,m_szFileName);
	
    for (i=0; i<2048; i++, j++)  
    {  
        if (chSectionNames[0] == '\0')  
        {  
            break;       // 如果第一个字符就是0，则说明ini中一个节也没有。  
        }  		
        if (chSectionNames[i] == '\0')  
        {  
            // 找到一个’\0’，则说明从这个字符往前，减掉j个偏移量，就是一个节名的首地址。  
            //pSectionName = &chSectionNames[i-j];   
            // 找到一个节名后，j的值要还原，以统计下一个节名地址的偏移量。  
            // 赋成-1是因为节名字符串的最后一个字符’\0’是终止符，不能作为节名的一部分。  
            //j = -1;  
            // 在获取节名的时候可以获取该节中键的值，前提是我们知道该节中有哪些键。  
            //AfxMessageBox(pSectionName);   // 把找到的显示出来。  
			count++;
            if (chSectionNames[i+1] == 0)   //is 0 or ‘\0’？  
            {  
                break;  // 当两个相邻的字符都是0时，则所有的节名都已找到，循环终止。  
            }  
        }  
    } 
	
	return 	count;
}



//} // End of namespace OnlineGameLib
//} // End of namespace Win32	
	
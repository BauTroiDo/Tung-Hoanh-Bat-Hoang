
#ifndef _PASSCHECKSUM_H
#define	_PASSCHECKSUM_H

/*#ifndef _WINBASE_
#include "winbase.h"
#endif*/

#include "string.h"

//public use
#define PG_MAXBUFFER			64
#define PG_MAXPASSWORDLEN		20
#define PG_RESULTLENSTD			32

//private use
#define PG_MINPKEYLEN			(PG_RESULTLENSTD - PG_MAXPASSWORDLEN - 2)
#define PG_MAXPKEYLEN			(PG_RESULTLENSTD - 2)
#define PG_PKEYMASK				151
#define PG_PKEYOFF				7
#define PG_ENOFF				5
#define PG_MINCHAR				0x20
#define PG_MAXCHAR				0x7E
#define PG_CHARCOUNT			(PG_MAXCHAR - PG_MINCHAR + 1)

#define PG_VALIDCHAR(c)			((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_') || (c >= '0' && c <= '9'))
#define PG_INVALIDCHAR(c)		!PG_VALIDCHAR(c)

class ENGINE_API PassChecksum 
{
public:
	//interface functions for the RSA MD5 calculation
	 int PassChecksum::SimplyDecryptPassword(char* szPass, const char* szEncrypted); //½âÂë
	 int PassChecksum::SimplyEncryptPassword(char* szResult, const char* szPass);    //¼ÓÂë
protected:
	
private:
     int PassChecksum::pgChar2Int(char b);
	 char PassChecksum::pgInt2Char(char i, DWORD dwTickCount = 0);
	 int PassChecksum::pgStrLen(const char* sz);
	 void PassChecksum::pgSwapChars(char* sz);
	 int PassChecksum::pgEncrypt(char* szKey, int nKeyLen, char* szBuffer, const char* szPass, int nStrLen);
	 void PassChecksum::pgDecrypt(char* szKey, int nKeyLen, char* szBuffer, const char* szEnc, int nStrLen);
     int PassChecksum::pgSameString(const char* s1, const char* s2);
//	 int PassChecksum::SimplyDecryptPassword(char* szPass, const char* szEncrypted);
	 int PassChecksum::pgEncryptPassword(char* szResult, const char* szPass, DWORD dwTickCount, int& nLevel);
//	 int PassChecksum::SimplyEncryptPassword(char* szResult, const char* szPass);
};
#endif // !defined(AFX_PASSCHECKSUM_H__2BC7928E_4C15_11D3_B2EE_A4A60E20D2C3__INCLUDED_)
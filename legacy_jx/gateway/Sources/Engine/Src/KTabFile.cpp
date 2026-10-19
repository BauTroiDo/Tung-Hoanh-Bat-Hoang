//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KTabFile.cpp
// Date:	2002.02.20
// Code:	Huyi(Spe)
// Desc:	Tab File Operation Class
//---------------------------------------------------------------------------
#include "KWin32.h"
#include "KDebug.h"
#include "KStrBase.h"
#include "KFile.h"
#include "KFilePath.h"
#include "KPakFile.h"
#ifndef _SERVER
#include "KCodec.h"
#endif
#include "KTabFile.h"
#include <string.h>
//---------------------------------------------------------------------------
// 函数:	KTabFile
// 功能:	购造函数
// 参数:	void
// 返回:	void
//---------------------------------------------------------------------------
KTabFile::KTabFile()
{
	m_Width		= 0;
	m_Height	= 0;
	dwFileIdx   = 0;

	///ZeroMemory(nMemKey,sizeof(nMemKey));
}
//---------------------------------------------------------------------------
// 函数:	~KTabFile
// 功能:	析造函数
// 参数:	void
// 返回:	void
//---------------------------------------------------------------------------
KTabFile::~KTabFile()
{
	Clear();
}
//---------------------------------------------------------------------------
// 函数:	Load
// 功能:	加载一个Tab文件
// 参数:	FileName	文件名
// 返回:	TRUE		成功
//			FALSE		失败
//---------------------------------------------------------------------------
BOOL KTabFile::Load(LPSTR FileName,LPSTR mMemKey)
{
    
	KPakFile	File;
	//DWORD		dwSize;
	PVOID		Buffer;

	// check file name
	if (FileName[0] == 0)
		return FALSE;

	
	if (!File.Open(FileName))
	{
		//g_DebugLog("Can't open tab file : %s", FileName);
	//	printf("Can't open tab file : %s", FileName);
		return FALSE;
	}
  
	dwSize = File.Size(); //文件的大小
	char nFileName[256]={0};
	snprintf(nFileName, sizeof(nFileName), "%s", FileName);
	_strlwr(nFileName);
	//g_StrLower(nFileName);
	dwFileIdx = g_FileName2Id(nFileName);
	if (dwSize<=0)
	{
		File.Close();
		return FALSE;
	}

	if  (mMemKey)
	     Buffer = m_Memory.Alloc(dwSize,g_FileName2Id(mMemKey));  //分配内存  并设置内存标志
	else
		 Buffer = m_Memory.Alloc(dwSize);

	//sprintf(nMemKey,mMemKey);

	File.Read(Buffer, dwSize);

	CreateTabOffset();

	File.Close();
	return TRUE;
}

//---------------------------------------------------------------------------
// 函数:	CreateTabOffset
// 功能:	建立制作表符分隔文件的偏移表
// 参数:	void
// 返回:	void
//---------------------------------------------------------------------------
void KTabFile::CreateTabOffset()
{
	int		nWidth, nHeight, nOffset, nSize;
	BYTE	*Buffer;
	TABOFFSET *TabBuffer;

	nWidth	= 1;
	nHeight	= 1;
	nOffset = 0;

	Buffer	= (BYTE *)m_Memory.GetMemPtr();	//获取内存指针
	nSize	= m_Memory.GetMemLen();

	// 读第一行决定有多少列
	// the line has to end inside the block, and the CR/LF lookahead under it needs
	// one more byte after that, so this scan stops at nSize - 1 instead of walking
	// off the allocation when the file's first line carries neither CR nor LF
	while (nOffset + 1 < nSize && *Buffer != 0x0d && *Buffer != 0x0a)
	{
		if (*Buffer == 0x09)
		{
			nWidth++;
		}
		Buffer++;
		nOffset++;
	}
	if (nOffset + 1 >= nSize)	// no line end with a byte after it inside the block:
	{						// the header line is the whole file, so there is no row
		m_Width		= nWidth;	// to index.  m_Height stays 0, and GetValue's
		m_Height	= 0;		// row test refuses every lookup instead of reading a table
		return;					// that was never built
	}
	if (*Buffer == 0x0d && *(Buffer + 1) == 0x0a)
	{
		Buffer += 2;	// 0x0a跳过		
		nOffset += 2;	// 0x0a跳过
	}
	else
	{
		Buffer += 1;	// 0x0a跳过		
		nOffset += 1;	// 0x0a跳过
	}
	while(nOffset < nSize)
	{
		while (*Buffer != 0x0d && *Buffer != 0x0a)
		{
			Buffer++;
			nOffset++;
			if (nOffset >= nSize)
				break;
		}
		if (nOffset >= nSize)			// the block ran out mid-row: what the scan stopped
			break;						// on is the file's last byte, not a line to count
		nHeight++;
		if (*Buffer == 0x0d && nOffset + 1 < nSize && *(Buffer + 1) == 0x0a)
		{
			Buffer += 2;	// 0x0a跳过		
			nOffset += 2;	// 0x0a跳过
		}
		else
		{
			Buffer += 1;	// 0x0a跳过		
			nOffset += 1;	// 0x0a跳过
		}
	}
	m_Width		= nWidth;
	m_Height	= nHeight;

	// the extent is a product of two numbers the file decides, and the block helper
	// takes a DWORD, so a grid that does not fit one has to be refused here rather
	// than asked for truncated.  Cells left are at most 0xffffffff/8, which is also
	// inside an int, so GetValue's row arithmetic cannot wrap either.
	if ((size_t)m_Width * m_Height > (size_t)0xffffffff / sizeof (TABOFFSET))
	{
		m_Height	= 0;		// reads as the empty table it never became
		return;
	}
	TabBuffer = (TABOFFSET *)m_OffsetTable.Alloc(
		sizeof (TABOFFSET) * (size_t)m_Width * m_Height);
	if (!TabBuffer)			// the helper returns NULL when the block manager says no
	{
		m_Height	= 0;
		return;
	}
	Buffer = (BYTE *)m_Memory.GetMemPtr();

	nOffset = 0;
	int nLength;
	for (int i = 0; i < nHeight; ++i)
	{
		for (int j = 0; j < nWidth; ++j)
		{
			TabBuffer->dwOffset = nOffset;	
			nLength = 0;
			// the length is asked before the byte is touched: && short-circuits left to
			// right, so with the delimiter tests in front this read the byte one past the
			// block on the very trip where nOffset had already run out
			while (nOffset < nSize && *Buffer != 0x09 && *Buffer != 0x0d && *Buffer != 0x0a)
			{
				Buffer++;
				nOffset++;
				nLength++;
			}
			if (nOffset < nSize)	// the scan also ends where the block ends, and then there
			{					// is no delimiter to skip: a step past the end puts the cursor
				Buffer++;	// 0x09或0x0d或0x0a(linux)跳过
				nOffset++;		// outside the allocation, and the reads below walk with it
			}
			TabBuffer->dwLength = nLength;
			TabBuffer++;
			if (*(Buffer - 1) == 0x0a || *(Buffer - 1) == 0x0d)	//	本行已经结束了，虽然可能没到nWidth //for linux modified [wxb 2003-7-29]
			{
				for (int k = j+1; k < nWidth; ++k)
				{
					TabBuffer->dwOffset = nOffset;
					TabBuffer->dwLength = 0;
					TabBuffer++;					
				}
				break;
			}
		}

		//modified for linux [wxb 2003-7-29]
		// a CRLF join needs two bytes, and the scan above can end on the block's last one
		// - Buffer walks with nOffset, so this is the test that says there is a byte here
		if (nOffset < nSize && *(Buffer - 1) == 0x0d && *Buffer == 0x0a)
		{
			Buffer++;				// 0x0a跳过	
			nOffset++;				// 0x0a跳过	
		}
	}
}

//---------------------------------------------------------------------------
// 函数:	Str2Column
// 功能:	取得某行某列字符串的值
// 参数:	szColumn
// 返回:	第几列
//---------------------------------------------------------------------------
int KTabFile::Str2Col(LPSTR szColumn)
{
	int	nStrLen = g_StrLen(szColumn);
	char	szTemp[4];

	//g_StrCpy(szTemp, szColumn);

	//g_StrUpper(szTemp);
	strupr(szTemp);
	if (nStrLen == 1)
	{
		return (szTemp[0] - 'A');
	}
	return ((szTemp[0] - 'A' + 1) * 26 + szTemp[1] - 'A') + 1;
}

//---------------------------------------------------------------------------
// 函数:	GetString
// 功能:	取得某行某列字符串的值
// 参数:	nRow			行
//			nColomn			列
//			lpDefault		缺省值
//			lpRString		返回值
//			dwSize			返回字符串的最大长度
// 返回:	是否成功
//---------------------------------------------------------------------------
BOOL KTabFile::GetString(int nRow, LPSTR szColumn, LPSTR lpDefault, LPSTR lpRString, DWORD dwSize, BOOL bColumnLab)
{
	int nColumn;
	if (bColumnLab)
		nColumn = FindColumn(szColumn);
	else
		nColumn = Str2Col(szColumn);
	if (GetValue(nRow - 1, nColumn - 1, lpRString, dwSize))
		return TRUE;
	g_StrCpyLen(lpRString, lpDefault, dwSize); //默认值
	    return FALSE;
}
//---------------------------------------------------------------------------
// 函数:	GetString
// 功能:	取得某行某列字符串的值
// 参数:	nRow			行
//			nColomn			列
//			lpDefault		缺省值
//			lpRString		返回值
//			dwSize			返回字符串的最大长度
// 返回:	是否成功
//---------------------------------------------------------------------------
BOOL KTabFile::GetString(LPSTR szRow, int szColumn, LPSTR lpDefault, LPSTR lpRString, DWORD dwSize, BOOL bColumnLab)
{
	int nRow, nColumn;
	
	   nRow = FindRow(szRow);

	if (bColumnLab)
		nColumn = szColumn;
	else
		nColumn = szColumn;

	if (GetValue(nRow - 1, nColumn - 1, lpRString, dwSize))
		return TRUE;
	g_StrCpyLen(lpRString, lpDefault, dwSize);
	return FALSE;
}

BOOL KTabFile::GetStringForCoding(LPSTR szRow, int szColumn, LPSTR lpDefault, LPSTR lpRString, DWORD dwSize, int bColumnLab,int bValueCoding)
{
	int nRow, nColumn;
	nColumn = szColumn;
	std::string nTempRow;
	switch(bColumnLab)
	{
	case 1:
		nTempRow=gbk2utf8(szRow);
		break;
	case 2:
		nTempRow=utf2gbk(szRow);
		break;
	case 3:
		nTempRow=gbk2iso(szRow);
		break;
	case 4:
		nTempRow=iso2gbk(szRow);
		break;
	case 5:
		nTempRow=iso2utf8(szRow);
		break;
	case 6:
		nTempRow=utf2iso(szRow);
		break;
	default:
		nTempRow=szRow;
		break;
	}

	nRow = FindRow((char *)nTempRow.c_str());
	std::string nTempValue;
	if (GetValue(nRow - 1, nColumn - 1, lpRString, dwSize))
	{
		switch(bValueCoding)
		{
		case 1:
			nTempValue=gbk2utf8(lpRString);
			break;
		case 2:
			nTempValue=utf2gbk(lpRString);
			break;
		case 3:
			nTempValue=gbk2iso(lpRString);
			nTempValue=iso2utf8(nTempValue.c_str());
			break;
		case 4:
			nTempValue=iso2gbk(lpRString);
			break;
		case 5:
			nTempValue=iso2utf8(lpRString);
			break;
		case 6:
			nTempValue=utf2iso(lpRString);
			break;
		case 7:
			nTempValue = w2v(lpRString);
		//	nTempValue = v2u(nTempValue.c_str());
			break;
		default:
			//nTempValue=lpRString;
			break;
		}

		if (bValueCoding>0)
		  g_StrCpyLen(lpRString, nTempValue.c_str(), dwSize);

		return TRUE;
	}
	g_StrCpyLen(lpRString, lpDefault, dwSize);
	return FALSE;
}

//---------------------------------------------------------------------------
// 函数:	GetString
// 功能:	取得某行某列字符串的值
// 参数:	szRow			行	（关键字）
//			szColomn		列	（关键字）
//			lpDefault		缺省值
//			lpRString		返回值
//			dwSize			返回字符串的最大长度
// 返回:	是否成功
//---------------------------------------------------------------------------
BOOL KTabFile::GetString(LPSTR szRow, LPSTR szColumn, LPSTR lpDefault, LPSTR lpRString, DWORD dwSize)
{
	int nRow, nColumn;

	nRow = FindRow(szRow);
	nColumn = FindColumn(szColumn);
	if (GetValue(nRow - 1, nColumn - 1, lpRString, dwSize))
		return TRUE;
	g_StrCpyLen(lpRString, lpDefault, dwSize);
	return FALSE;
}
//---------------------------------------------------------------------------
// 函数:	GetString
// 功能:	取得某行某列字符串的值
// 参数:	nRow			行		从1开始
//			nColomn			列		从1开始
//			lpDefault		缺省值
//			lpRString		返回值
//			dwSize			返回字符串的最大长度
// 返回:	是否成功
//---------------------------------------------------------------------------
BOOL KTabFile::GetString(int nRow, int nColumn, LPSTR lpDefault, LPSTR lpRString, DWORD dwSize)
{
	if (GetValue(nRow - 1, nColumn - 1,  lpRString, dwSize))
		return TRUE;
	g_StrCpyLen(lpRString, lpDefault, dwSize);
	return FALSE;
}
//---------------------------------------------------------------------------
// 函数:	GetInteger
// 功能:	取得某行某列字符串的值
// 参数:	nRow			行
//			szColomn		列
//			nDefault		缺省值
//			pnValue			返回值
// 返回:	是否成功
//---------------------------------------------------------------------------
BOOL KTabFile::GetInteger(int nRow, LPSTR szColumn, int nDefault, int *pnValue, BOOL bColumnLab)
{
	char	Buffer[64];
	int		nColumn;
	if (bColumnLab)
		nColumn = FindColumn(szColumn);
	else
		nColumn = Str2Col(szColumn);
	if (GetValue(nRow - 1, nColumn - 1, Buffer, sizeof(Buffer)))
	{
		*pnValue = atoi(Buffer);
		return TRUE;
	}
	else
	{
		*pnValue = nDefault;
		return FALSE;
	}
}

//---------------------------------------------------------------------------
// 函数:	GetInteger6  6字段提取int字符
// 功能:	取得某行某列字符串的值
// 参数:	nRow			行
//			szColomn		列
//			nDefault		缺省值
//			pnValue			返回值
// 返回:	是否成功
//---------------------------------------------------------------------------
BOOL KTabFile::GetInteger6(int nRow, LPSTR szColumna,LPSTR szColumnb,LPSTR szColumnc,LPSTR szColumnd,LPSTR szColumne,LPSTR szColumnf,  int nDefault, int *pnValue, BOOL bColumnLab)
{
	char	Buffer[64];
	int		nColumna,nColumnb,nColumnc,nColumnd,nColumne,nColumnf;
	if (bColumnLab)
	{
		nColumna = FindColumn(szColumna);
		nColumnb = FindColumn(szColumnb);
		nColumnc = FindColumn(szColumnc);
		nColumnd = FindColumn(szColumnd);
		nColumne = FindColumn(szColumne);
		nColumnf = FindColumn(szColumnf);
	}
	else
	{
		nColumna = Str2Col(szColumna);
		nColumnb = Str2Col(szColumnb);
		nColumnc = Str2Col(szColumnc);
		nColumnd = Str2Col(szColumnd);
		nColumne = Str2Col(szColumne);
		nColumnf = Str2Col(szColumnf);
	}

	if (GetValue(nRow - 1, nColumna - 1, Buffer, sizeof(Buffer)))
	{
		*pnValue = atoi(Buffer);
		//return TRUE;
	}
	else
	{
		*pnValue = nDefault;
	}
     pnValue++;
    if(GetValue(nRow - 1, nColumnb - 1, Buffer, sizeof(Buffer)))
	{
        
		*pnValue = atoi(Buffer);
		//return TRUE;
	}
	else
	{
		*pnValue = nDefault;
	}
     pnValue++;
    if(GetValue(nRow - 1, nColumnc - 1, Buffer, sizeof(Buffer)))
	{
		*pnValue = atoi(Buffer);
		//return TRUE;
	}
	else
	{
		*pnValue = nDefault;
	}
     pnValue++;
	if(GetValue(nRow - 1, nColumnd - 1, Buffer, sizeof(Buffer)))
	{
		*pnValue = atoi(Buffer);
		//return TRUE;
	}
	else
	{
		*pnValue = nDefault;
	}
     pnValue++;
	if(GetValue(nRow - 1, nColumne - 1, Buffer, sizeof(Buffer)))
	{
		*pnValue = atoi(Buffer);
		//return TRUE;
	}
	else
	{
		*pnValue = nDefault;
	}
     pnValue++;
	if(GetValue(nRow - 1, nColumnf - 1, Buffer, sizeof(Buffer)))
	{
		*pnValue = atoi(Buffer);
		//return TRUE;
	}
	else
	{
		*pnValue = nDefault;
	}

	if (pnValue)
	{
      return TRUE;
	}

	 return FALSE;
}
//---------------------------------------------------------------------------
// 函数:	GetInteger3  3字段提取int字符
// 功能:	取得某行某列字符串的值
// 参数:	nRow			行
//			szColomn		列
//			nDefault		缺省值
//			pnValue			返回值
// 返回:	是否成功
//---------------------------------------------------------------------------
BOOL KTabFile::GetInteger3(int nRow, LPSTR szColumna,LPSTR szColumnb,LPSTR szColumnc,int nDefault, int *pnValue, BOOL bColumnLab)
{
	char	Buffer[64];
	int		nColumna,nColumnb,nColumnc;
	if (bColumnLab)
	{
		nColumna = FindColumn(szColumna);
		nColumnb = FindColumn(szColumnb);
		nColumnc = FindColumn(szColumnc);
	}
	else
	{
		nColumna = Str2Col(szColumna);
		nColumnb = Str2Col(szColumnb);
		nColumnc = Str2Col(szColumnc);
	}
	
	if (GetValue(nRow - 1, nColumna - 1, Buffer, sizeof(Buffer)))
	{
		*pnValue = atoi(Buffer);
		//return TRUE;
	}
	else
	{
		*pnValue = nDefault;
	}
	pnValue++;
    if(GetValue(nRow - 1, nColumnb - 1, Buffer, sizeof(Buffer)))
	{
        
		*pnValue = atoi(Buffer);
		//return TRUE;
	}
	else
	{
		*pnValue = nDefault;
	}
	pnValue++;
    if(GetValue(nRow - 1, nColumnc - 1, Buffer, sizeof(Buffer)))
	{
		*pnValue = atoi(Buffer);
		//return TRUE;
	}
	else
	{
		*pnValue = nDefault;
	}
	
	
	if (pnValue)
	{
	   return TRUE;
	}
	   return FALSE;
}




//同个字段分割符号 |
//void KTabFile::GetInt2(LPCSTR lpSection, LPCSTR lpKeyName, int* pRect)

void KTabFile::GetInt2(int nRow, int szColumn, int *pRect)
{
	char  Buffer[256];

	if (GetValue(nRow - 1, szColumn - 1, Buffer, sizeof(Buffer)))
	{
		sscanf(Buffer, "%d|%d", &pRect[0], &pRect[1]);
	}
}

void KTabFile::GetInt15(int nRow, int szColumn, int *pRect)
{
	char  Buffer[256];
	
	if (GetValue(nRow - 1, szColumn - 1, Buffer, sizeof(Buffer)))
	{
		sscanf(Buffer, "%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d", &pRect[0],&pRect[1],&pRect[2],&pRect[3],&pRect[4],&pRect[5],&pRect[6],&pRect[7],&pRect[8],&pRect[9],&pRect[10],&pRect[11],&pRect[12],&pRect[13],&pRect[14]);
	}
}

void KTabFile::GetInt15s(int nRow, LPSTR szColumn, int *pRect)
{
	char  Buffer[256];
	
	int	nColumn = FindColumn(szColumn);
	if (GetValue(nRow - 1, nColumn - 1, Buffer, sizeof(Buffer)))
	{
		sscanf(Buffer, "%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d", &pRect[0],&pRect[1],&pRect[2],&pRect[3],&pRect[4],&pRect[5],&pRect[6],&pRect[7],&pRect[8],&pRect[9],&pRect[10],&pRect[11],&pRect[12],&pRect[13],&pRect[14]);
	}
}

void KTabFile::GetInt3(int nRow, int szColumn, int *pRect)
{
	char  Buffer[256];
	
	if (GetValue(nRow - 1, szColumn - 1, Buffer, sizeof(Buffer)))
	{
		sscanf(Buffer, "%d|%d|%d", &pRect[0], &pRect[1], &pRect[2]);
	}
}
//------字符串类-----
void KTabFile::GetInt2s(int nRow, LPSTR szColumn, int *pRect)
{
	char  Buffer[256];
 
	int	nColumn = FindColumn(szColumn);
	if (GetValue(nRow - 1, nColumn - 1, Buffer, sizeof(Buffer)))
	{
		sscanf(Buffer, "%d|%d", &pRect[0], &pRect[1]);
	}
}

void KTabFile::GetInt3s(int nRow, LPSTR szColumn, int *pRect)
{
	char  Buffer[256];
	int	nColumn = FindColumn(szColumn);
	if (GetValue(nRow - 1, nColumn - 1, Buffer, sizeof(Buffer)))
	{
		sscanf(Buffer, "%d|%d|%d", &pRect[0], &pRect[1], &pRect[2]);
	}
}
//---------------------------------------------------------------------------
// 函数:	GetInteger
// 功能:	取得某行某列字符串的值
// 参数:	szRow			行
//			szColomn		列
//			nDefault		缺省值
//			pnValue			返回值
// 返回:	是否成功
//---------------------------------------------------------------------------
BOOL KTabFile::GetInteger(LPSTR szRow, LPSTR szColumn, int nDefault, int *pnValue)
{
	int		nRow, nColumn;
	char	Buffer[64];

	nRow = FindRow(szRow);
	nColumn = FindColumn(szColumn);
	if (GetValue(nRow - 1, nColumn - 1, Buffer, sizeof(Buffer)))
	{
		*pnValue = atoi(Buffer);
		return TRUE;
	}
	else
	{
		*pnValue = nDefault;
		return FALSE;
	}
}
//---------------------------------------------------------------------------
// 函数:	GetInteger
// 功能:	取得某行某列字符串的值
// 参数:	nRow			行		从1开始
//			nColomn			列		从1开始
//			nDefault		缺省值
//			pnValue			返回值
// 返回:	是否成功
//---------------------------------------------------------------------------
BOOL KTabFile::GetInteger(int nRow, int nColumn, int nDefault, int *pnValue)
{
	char	Buffer[64];

	if (GetValue(nRow - 1, nColumn - 1, Buffer, sizeof(Buffer)))
	{
		*pnValue = atoi(Buffer);
		return TRUE;
	}
	else
	{
		*pnValue = nDefault;
		return TRUE;
	}
}

BOOL KTabFile::GetIntegerDW(int nRow, LPSTR szColumn, int nDefault,DWORD *pnValue, BOOL bColumnLab)
{
	char	Buffer[64];
	int		nColumn;
	if (bColumnLab)
		nColumn = FindColumn(szColumn);
	else
		nColumn = Str2Col(szColumn);
	if (GetValue(nRow - 1, nColumn - 1, Buffer, sizeof(Buffer)))
	{
		*pnValue = atoi(Buffer);
		return TRUE;
	}
	else
	{
		*pnValue = nDefault;
		return FALSE;
	}
}

BOOL KTabFile::GetDword(int nRow, LPSTR szColumn, DWORD fDefault, DWORD *pfValue, BOOL bColumnLab)
{
	char	Buffer[64];
	int		nColumn;
	if (bColumnLab)
		nColumn = FindColumn(szColumn);
	else
		nColumn = Str2Col(szColumn);
	if (GetValue(nRow - 1, nColumn - 1, Buffer, sizeof(Buffer)))
	{
		*pfValue = (DWORD)atof(Buffer);
		return TRUE;
	}
	else
	{
		*pfValue = fDefault;
		return FALSE;
	}
}


//---------------------------------------------------------------------------
// 函数:	GetDouble
// 功能:	取得某行某列字符串的值
// 参数:	nRow			行
//			szColomn		列
//			nDefault		缺省值
//			pnValue			返回值
// 返回:	是否成功
//---------------------------------------------------------------------------
BOOL KTabFile::GetDouble(int nRow, LPSTR szColumn, double fDefault, double *pfValue, BOOL bColumnLab)
{
	char	Buffer[64];
	int		nColumn;
	if (bColumnLab)
		nColumn = FindColumn(szColumn);
	else
		nColumn = Str2Col(szColumn);
	if (GetValue(nRow - 1, nColumn - 1, Buffer, sizeof(Buffer)))
	{
		*pfValue = (double)atof(Buffer);
		return TRUE;
	}
	else
	{
		*pfValue = fDefault;
		return FALSE;
	}
}

//---------------------------------------------------------------------------
// 函数:	GetFloat
// 功能:	取得某行某列字符串的值
// 参数:	nRow			行
//			szColomn		列
//			nDefault		缺省值
//			pnValue			返回值
// 返回:	是否成功
//---------------------------------------------------------------------------
BOOL KTabFile::GetFloat(int nRow, LPSTR szColumn, float fDefault, float *pfValue, BOOL bColumnLab)
{
	char	Buffer[64];
	int		nColumn;
	if (bColumnLab)
		nColumn = FindColumn(szColumn);
	else
		nColumn = Str2Col(szColumn);
	if (GetValue(nRow - 1, nColumn - 1, Buffer, sizeof(Buffer)))
	{
		*pfValue = (float)atof(Buffer);
		return TRUE;
	}
	else
	{
		*pfValue = fDefault;
		return FALSE;
	}
}
//---------------------------------------------------------------------------
// 函数:	GetFloat
// 功能:	取得某行某列字符串的值
// 参数:	szRow			行
//			szColomn		列
//			nDefault		缺省值
//			pnValue			返回值
// 返回:	是否成功
//---------------------------------------------------------------------------
BOOL KTabFile::GetFloat(LPSTR szRow, LPSTR szColumn, float fDefault, float *pfValue)
{
	int		nRow, nColumn;
	char	Buffer[64];

	nRow = FindRow(szRow);
	nColumn = FindColumn(szColumn);
	if (GetValue(nRow - 1, nColumn - 1, Buffer, sizeof(Buffer)))
	{
		*pfValue = (float)atof(Buffer);
		return TRUE;
	}
	else
	{
		*pfValue = fDefault;
		return FALSE;
	}
}
//---------------------------------------------------------------------------
// 函数:	GetFloat
// 功能:	取得某行某列字符串的值
// 参数:	nRow			行		从1开始
//			nColomn			列		从1开始
//			nDefault		缺省值
//			pnValue			返回值
// 返回:	是否成功
//---------------------------------------------------------------------------
BOOL KTabFile::GetFloat(int nRow, int nColumn, float fDefault, float *pfValue)
{
	char	Buffer[64];
	
	if (GetValue(nRow - 1, nColumn - 1, Buffer, sizeof(Buffer)))
	{
		*pfValue = (float)atof(Buffer);
		return TRUE;
	}
	else
	{
		*pfValue = fDefault;
		return FALSE;
	}
}
//---------------------------------------------------------------------------
// 函数:	GetValue
// 功能:	取得某行某列字符串的值
// 参数:	nRow			行
//			nColomn			列
//			lpDefault		缺省值
//			lpRString		返回值
//			dwSize			返回字符串的最大长度
// 返回:	是否成功
//---------------------------------------------------------------------------
BOOL KTabFile::GetValue(int nRow, int nColumn, LPSTR lpRString, DWORD dwSize)
{
	if (nRow >= m_Height || nColumn >= m_Width || nRow < 0 || nColumn < 0)
		return FALSE;

	TABOFFSET	*TempOffset;
	LPSTR		Buffer;

	Buffer = (LPSTR)m_Memory.GetMemPtr();
	TempOffset = (TABOFFSET *)m_OffsetTable.GetMemPtr();
	// bounded by CreateTabOffset, which refuses a grid whose cell count needs more
	// than the 32 bits this index and the block it points into are measured with
	TempOffset += nRow * m_Width + nColumn;

	ZeroMemory(lpRString, dwSize);
	if (TempOffset->dwOffset >= m_Memory.GetMemLen())	// both numbers are file
		return FALSE;						// data the index points at, so
	DWORD dwRoom = m_Memory.GetMemLen() - TempOffset->dwOffset;	// neither was proven
	if (TempOffset->dwLength > dwRoom)
		return FALSE;
	Buffer += TempOffset->dwOffset;
	if (TempOffset->dwLength == 0)
	{
		TempOffset=NULL;
		return FALSE;
	}
	if (dwSize > TempOffset->dwLength)
	{
		memcpy(lpRString, Buffer, TempOffset->dwLength);
		lpRString[TempOffset->dwLength] ='\0';
	}
	else
	{
		if (dwSize == 0)			// a capacity of 0 holds no terminator at all
			return FALSE;
		memcpy(lpRString, Buffer, dwSize - 1);
		lpRString[dwSize - 1] = '\0';
	}
    TempOffset=NULL;
	return TRUE;
}
//---------------------------------------------------------------------------
// 函数:	Clear
// 功能:	清除TAB文件的内容
// 参数:	void
// 返回:	void
//---------------------------------------------------------------------------
void KTabFile::Clear()
{
/*	if (m_Memory)
	{
		g_MemFree(m_Memory);
		m_Memory = NULL;
	}
	*/
	//ZeroMemory(nMemKey,sizeof(nMemKey));
	dwFileIdx = 0;
	m_OffsetTable.Free();
	m_Memory.Free();
}
//---------------------------------------------------------------------------
// 函数:	FindRow
// 功能:	查找行关键字
// 参数:	szRow（行关键字）
// 返回:	int
//---------------------------------------------------------------------------
int KTabFile::FindRow(LPSTR szRow)
{
	char	szTemp[128];
	for (int i = 0; i < m_Height; ++i)	// 从1开始，跳过第一行的字段行
	{
		GetValue(i, 0, szTemp, g_StrLen(szRow));
		if (g_StrCmp(szTemp, szRow))
			return i + 1; //改动此处为加一 by Romandou,即返回以1为起点的标号
	}
	return -1;
}
//---------------------------------------------------------------------------
// 函数:	FindColumn
// 功能:	查找列关键字
// 参数:	szColumn（行关键字）
// 返回:	int
//---------------------------------------------------------------------------
int KTabFile::FindColumn(LPSTR szColumn)
{
	char	szTemp[128];
	for (int i = 0; i < m_Width; ++i)	// 从1开始，跳过第一列的字段行
	{
		GetValue(0, i, szTemp, g_StrLen(szColumn));
		if (g_StrCmp(szTemp, szColumn))
			return i + 1;//改动此处为加一 by Romandou,即返回以1为起点的标号
	}
	return -1;
}

//---------------------------------------------------------------------------
// 函数:	Col2Str
// 功能:	把整数转成字符串
// 参数:	szColumn
// 返回:	第几列
//---------------------------------------------------------------------------
void KTabFile::Col2Str(int nCol, LPSTR szColumn)
{

	if (nCol < 26)
	{
		szColumn[0] = 'A' + nCol;
		szColumn[1]	= 0;
	}
	else
	{
		szColumn[0] = 'A' + (nCol / 26 - 1);
		szColumn[1] = 'A' + nCol % 26;
		szColumn[2] = 0;
	}
}

//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KTabFile.h
// Date:	2002.02.18
// Code:	Spe
// Desc:	Header File
//---------------------------------------------------------------------------
#ifndef KTabFile_H
#define KTabFile_H

#include "KMemClass.h"
#include "KITabFile.h"
//#include "KTabFileCtrl.h"
//---------------------------------------------------------------------------
typedef struct tagTabOffset
{
	DWORD		dwOffset;
	DWORD		dwLength;
} TABOFFSET;

class ENGINE_API KTabFile:public KITabFile
{
private:
	int			m_Width;
	int			m_Height;
	KMemClass	m_Memory;
	KMemClass	m_OffsetTable;
	DWORD		dwSize;
	DWORD		dwFileIdx;
	//PVOID		Buffer;
	//char        nMemKey[32];
private:
	void		CreateTabOffset();
	BOOL		GetValue(int nRow, int nColumn, LPSTR lpRString, DWORD dwSize);
	int			Str2Col(LPSTR szColumn);
public:
	KTabFile();
	~KTabFile();
//	KTabFileCtrl *TabFileCtrl;
	BOOL		Load(LPSTR FileName,LPSTR mMemKey=NULL);
	BOOL		Save(LPSTR FileName){return FALSE;}; //无法保存
	BOOL		LoadPack(LPSTR FileName);
	DWORD       GetFileDwIdx(){ return dwFileIdx;}

	DWORD		GetSize(){ return dwSize;};
	int			FindRow(LPSTR szRow);//返回以1为起点的值
	int			FindColumn(LPSTR szColumn);//返回以1为起点的值
	void		Col2Str(int nCol, LPSTR szColumn);
	int			GetWidth() { return m_Width;};
	int			GetHeight() { return m_Height;};
	BOOL		GetString(int nRow, LPSTR szColumn, LPSTR lpDefault, LPSTR lpRString, DWORD dwSize, BOOL bColumnLab = TRUE);
	BOOL		GetString(LPSTR szRow, int szColumn, LPSTR lpDefault, LPSTR lpRString, DWORD dwSize, BOOL bColumnLab = TRUE);
	BOOL     GetStringForCoding(LPSTR szRow, int szColumn, LPSTR lpDefault, LPSTR lpRString, DWORD dwSize, int bColumnLab=0,int bValueCoding=0);

	BOOL		GetString(int nRow, int nColumn, LPSTR lpDefault, LPSTR lpRString, DWORD dwSize);
	BOOL		GetString(LPSTR szRow, LPSTR szColumn, LPSTR lpDefault, LPSTR lpRString, DWORD dwSize);
	BOOL		GetInteger(int nRow, LPSTR szColumn, int nDefault, int *pnValue, BOOL bColumnLab = TRUE);
	BOOL		GetInteger6(int nRow, LPSTR szColumna,LPSTR szColumnb,LPSTR szColumnc,LPSTR szColumnd,LPSTR szColumne,LPSTR szColumnf,int nDefault, int *pnValue, BOOL bColumnLab= TRUE);
	BOOL		GetInteger3(int nRow, LPSTR szColumna,LPSTR szColumnb,LPSTR szColumnc,int nDefault, int *pnValue, BOOL bColumnLab= TRUE);
	BOOL		GetInteger(int nRow, int nColumn, int nDefault, int *pnValue);
	BOOL        GetIntegerDW(int nRow, LPSTR szColumn, int nDefault,DWORD *pnValue, BOOL bColumnLab = TRUE);
	BOOL		GetInteger(LPSTR szRow, LPSTR szColumn, int nDefault, int *pnValue);
	BOOL		GetFloat(int nRow, LPSTR szColumn, float fDefault, float *pfValue, BOOL bColumnLab = TRUE);
	BOOL		GetDouble(int nRow, LPSTR szColumn, double fDefault, double *pfValue, BOOL bColumnLab = TRUE);
	BOOL		GetDword(int nRow, LPSTR szColumn, DWORD fDefault, DWORD *pfValue, BOOL bColumnLab = TRUE);
//	BOOL		GetFloat(LPSTR szRow, LPSTR szColumn, float fDefault, float *pfValue);
	BOOL		GetFloat(int nRow, int nColumn, float fDefault, float *pfValue);
	BOOL		GetFloat(LPSTR szRow, LPSTR szColumn, float fDefault, float *pfValue);
	void        GetInt2(int nRow, int szColumn, int *pRect);
	void        GetInt15(int nRow, int szColumn, int *pRect);
	void        GetInt15s(int nRow, LPSTR szColumn, int *pRect);
	void        GetInt3(int nRow, int szColumn, int *pRect);
	void        GetInt2s(int nRow, LPSTR szColumn, int *pRect);
	void        GetInt3s(int nRow, LPSTR szColumn, int *pRect);
	void		Clear();
};
//---------------------------------------------------------------------------
#endif

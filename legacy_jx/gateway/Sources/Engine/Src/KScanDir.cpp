//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KScanFile.cpp
// Date:	2000.08.08
// Code:	WangWei(Daphnis)
// Desc:	Memory Opration Class Using Heap Memory Functions
//---------------------------------------------------------------------------
#include "KWin32.h"
#include "KDebug.h"
#include "KMemBase.h"
#include "KFilePath.h"
#include "KScanDir.h"
#include <Shlwapi.h> 
#pragma comment(lib,"Shlwapi.lib")
//---------------------------------------------------------------------------
// 函数:	
// 功能:	
// 参数:	lpFileExt
// 返回:	void
//---------------------------------------------------------------------------
KScanFile::KScanFile()
{
	g_MemZero(m_FileExt, sizeof(m_FileExt));
}
//---------------------------------------------------------------------------
// 函数:	
// 功能:	
// 参数:	lpFileExt
// 返回:	void
//---------------------------------------------------------------------------
void KScanFile::DiscardFileExt(LPSTR lpFileExt)
{
	g_MemCopy(m_FileExt, lpFileExt, 3);
}
//---------------------------------------------------------------------------
// 函数:	RunSearch
// 功能:	扫描目录
// 参数:	lpRootPath		根目录
//			lpScanPath		扫描目录
//			lpOutFile		输出文件
// 返回:	TRUE			成功
//			FALSE			失败
//---------------------------------------------------------------------------
BOOL KScanFile::RunSearch(LPSTR lpRootPath, LPSTR lpScanPath, LPSTR lpOutFile)
{
	m_pFile = fopen(lpOutFile, "wb");
	if (m_pFile == NULL)
		return FALSE;
	m_nPathLen = strlen(lpRootPath);
	if (m_nPathLen > 0 && lpRootPath[m_nPathLen - 1] != '\\')
		m_nPathLen++;
	fprintf(m_pFile, "%s\r\n", lpRootPath);
	SetCurrentDirectory(lpScanPath);
	SearchDirectory();
	fclose(m_pFile);
	return TRUE;
}
//---------------------------------------------------------------------------
// 函数:	SearchDirectory
// 功能:	扫描目录，输出目录下所有文件
// 参数:	void
// 返回:	void
//---------------------------------------------------------------------------
void KScanFile::SearchDirectory() 
{
	WIN32_FIND_DATA FindData;
	HANDLE hFind = NULL;
	
	// set findfirstfile to find everthing
	hFind = FindFirstFile("*", &FindData);

	// if handle fails, drive is empty...
	if (hFind == INVALID_HANDLE_VALUE)
		return;
	
	// get first entity on drive - check if it's a directory
	if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)  //是否为文件夹
	{
		// if so, change to that directory and recursively call SearchDirectory
		if (FindData.cFileName[0] != '.')
		{
			// enter the directory
			SetCurrentDirectory(FindData.cFileName);
			// search the directory
			SearchDirectory();
			// go back up one directory level
			SetCurrentDirectory("..");
		}
	}
	else
	{
		if (CheckFileExt(FindData.cFileName))
			OutputFileName(FindData.cFileName);
	}

	while (FindNextFile(hFind, &FindData))
	{
		
		if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (FindData.cFileName[0] != '.')
			{
				SetCurrentDirectory(FindData.cFileName);
				SearchDirectory();
				SetCurrentDirectory("..");
			}
		}
		else
		{
			if (CheckFileExt(FindData.cFileName))
				OutputFileName(FindData.cFileName);
		}
	}

	FindClose(hFind);
}
//---------------------------------------------------------------------------
// 函数:	CheckFileExt
// 功能:	检查文件扩展名
// 参数:	FileName	
// 返回:	BOOL
//---------------------------------------------------------------------------
BOOL KScanFile::CheckFileExt(LPSTR FileName)
{
	// the three bytes compared here are the tail of the name, and the caller hands this
	// function whatever the directory offers: a one or two character entry has no tail of
	// its own, so the compare would reach behind the buffer and decide a file's fate on
	// whatever sat there.  A name that short has no extension to match, which is exactly
	// the answer this function gives for a file the scan keeps.
	int nNameLen = (int)strlen(FileName);
	if (nNameLen < 3)
		return TRUE;
	if (memcmp(m_FileExt, &FileName[nNameLen - 3], 3) == 0)
		return FALSE;
	return TRUE;
}

BOOL KScanFile::CheckPathDirExt(LPSTR FileName)
{
	if (PathFileExists(FileName))
		return TRUE;
	return FALSE;
}

//---------------------------------------------------------------------------
// 函数:	OutputFileName
// 功能:	输出文件名
// 参数:	FileName	
// 返回:	void
//---------------------------------------------------------------------------
void KScanFile::OutputFileName(LPSTR FileName)
{
	char szPathName[MAXPATH];

	DWORD nGot = GetCurrentDirectory(MAXPATH, szPathName);
	// 0 is a failure that writes nothing, and a size >= MAXPATH is the name not fitting: either
	// way the buffer below is not a path, and the slice at m_nPathLen would run off its end
	if (0 == nGot || nGot >= MAXPATH || m_nPathLen > (int)nGot)
		return;
	strcat(szPathName, "\\");
	strcat(szPathName, FileName);
	strcat(szPathName, "\r\n");
	fprintf(m_pFile, "%s", &szPathName[m_nPathLen]);
}
//---------------------------------------------------------------------------

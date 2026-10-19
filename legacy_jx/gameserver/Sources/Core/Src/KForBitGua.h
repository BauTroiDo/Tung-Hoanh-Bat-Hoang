//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KEvent.h
// Date:	2000.08.08
// Code:	WangWei(Daphnis)
// Desc:	Header File
//---------------------------------------------------------------------------
#ifndef KForBitGua_H
#define KForBitGua_H
//---------------------------------------------------------------------------
#include "KEngine.h"
#include "KIniFile.h"
#include "KTabFile.h"
class  KForBitGua //CORE_API
{
public:
    KForBitGua();
    ~KForBitGua();
	int  GetProcessList(char *nKey=NULL,int nZRows=0,int gCurRows=0);
	char * ListProcessModules( DWORD dwPID ,char *nName=NULL);
	BOOL ListProcessThreads( DWORD dwOwnerPID );
	void printError( TCHAR* msg );
	BOOL EnableDebugPrivilege(BOOL fEnable);
	int  CheckFolderExist(char* strPath,char *snPathKey,int snRows=0);
    int  GetVersionA(char *nFiledPath,char *nCols=NULL,int gRows=0);
	void GetFileDescriptionAndProductVersionA(LPCSTR lpstrFilename,
		LPSTR lpoutFileDescription, UINT cbDescSize,
		LPSTR lpoutProductVersion, UINT cbVerSize,
	    LPSTR lpoutCompanyName, UINT cbCompanySize);
	int  FindWindowsTiele(char *nTitle);

	BOOL	Init();	
	BOOL	Colse();

private:
//	HANDLE m_hEvent;//用于多进程的线程同步
   HANDLE	hProcessSnap;
   HANDLE   hModuleSnap;
   HANDLE   hThreadSnap;
};
//---------------------------------------------------------------------------
extern KForBitGua	g_ForBitGua;
#endif


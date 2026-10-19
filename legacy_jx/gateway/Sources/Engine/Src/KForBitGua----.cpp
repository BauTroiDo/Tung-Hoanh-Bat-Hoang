//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KEvent.cpp
// Date:	2000.08.08
// Code:	WangWei(Daphnis)
// Desc:	Implements a simple event object for thread synchronization
//---------------------------------------------------------------------------
#include "KWin32.h"
#include "KDebug.h"
#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>
#include <string.h>
#include <Shlwapi.h> 
#include "KForBitGua.h"
#include "PSAPI.H"
#include <io.h> 
#include <Shlwapi.h>
//#include <WINBASE.H> 
// 防止错误 error LNK2019
//#pragma comment(lib, "psapi.lib")
//---------------------------------------------------------------------------
// 函数:	KEvent
// 功能:	购造函数
// 参数:	void
// 返回:	void
//---------------------------------------------------------------------------
KForBitGua::KForBitGua()
{
    //m_hEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
   // if (!m_hEvent)
	//	g_DebugLog("KEvent::KEvent(): CreateEvent() failed!");
}
//---------------------------------------------------------------------------
// 函数:	~KEvent
// 功能:	析购函数
// 参数:	void
// 返回:	void
//---------------------------------------------------------------------------
KForBitGua::~KForBitGua()
{
    //CloseHandle(m_hEvent);
}
//---------------------------------------------------------------------------
// 函数:	TimedWait
// 功能:	
// 参数:	long ms
// 返回:	void
//---------------------------------------------------------------------------
BOOL KForBitGua::GetProcessList(char *nKey)
{
  HANDLE hProcessSnap;
  HANDLE hProcess;
  PROCESSENTRY32 pe32;
  DWORD dwPriorityClass;
  if(!EnableDebugPrivilege(1))
  {
	  MessageBox(NULL, "启动失败！", "提示", MB_OK|MB_ICONEXCLAMATION);	  
  }
  
  // Take a snapshot of all processes in the system.
  hProcessSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
  if( hProcessSnap == INVALID_HANDLE_VALUE )
  {
    printError( "CreateToolhelp32Snapshot (of processes)" );
    return( FALSE );
  }
 
  // Set the size of the structure before using it.
  pe32.dwSize = sizeof( PROCESSENTRY32 );
 
  // Retrieve information about the first process,
  // and exit if unsuccessful
  if( !Process32First( hProcessSnap, &pe32 ) )
  {
    printError( "Process32First" );  // Show cause of failure
    CloseHandle( hProcessSnap );     // Must clean up the snapshot object!
    return( FALSE );
  }
 
  // Now walk the snapshot of processes, and
  // display information about each process in turn
  do
  {
/*  printf( "\n\n=====================================================" );
    printf( "\nPROCESS NAME:  %s", pe32.szExeFile );
    printf( "\n-------------------------------------------------------" );
 */
    // Retrieve the priority class.
    dwPriorityClass = 0;
    hProcess = OpenProcess( PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID );
    if( hProcess == NULL )
      printError( "OpenProcess" );
    else
    {
      dwPriorityClass = GetPriorityClass( hProcess );
      if( !dwPriorityClass )
        printError( "GetPriorityClass" );
      //CloseHandle( hProcess );
    }
/*  printf( "\n  process ID        = 0x%08X", pe32.th32ProcessID );
    printf( "\n  thread count      = %d",   pe32.cntThreads );
    printf( "\n  parent process ID = 0x%08X", pe32.th32ParentProcessID );
    printf( "\n  Priority Base     = %d", pe32.pcPriClassBase );
    if( dwPriorityClass )
    printf( "\n  Priority Class    = %d", dwPriorityClass );
*/ 
    // List the modules and threads associated with this process
	char nFiledPath[MAX_PATH];
	sprintf(nFiledPath,"%s",ListProcessModules(pe32.th32ProcessID)); //获取进程完整路径
    printf( "\n  返回的路径        = %s", nFiledPath);
    if (strstr(nFiledPath,"剑侠"))  //路径
	{ 
		unsigned long dwExitCode = 0; 
		//GetExitCodeProcess(hProcess,&dwExitCode); //获取退出码
		//::TerminateProcess(hProcess,dwExitCode);  //关闭进程
		//::CloseHandle(hProcess);
	} 
    //提取分割路径
	 TCHAR szDrive[_MAX_DRIVE],szDir[_MAX_DIR],szFname[_MAX_FNAME],szExt[_MAX_EXT];
     _splitpath(nFiledPath, szDrive, szDir, szFname, szExt ); 
      printf( "\n  盘号  :%s\n  路径  :%s\n  文件名:%s\n  扩展名:%s", szDrive,szDir,szFname,szExt);
	  GetVersionA(nFiledPath);   //获取版本信息
	  sprintf(nFiledPath,"%s%s%s",szDrive,szDir,"脚本");
	  //检测路径的文件夹是否存在
	 if (CheckFolderExist(nFiledPath))
	 {  
          printf( "\n  检测的路径(%s)=存在(已关闭程序)",nFiledPath);
		  unsigned long dwExitCode = 0; 
		  GetExitCodeProcess(hProcess,&dwExitCode); //获取退出码
		  ::TerminateProcess(hProcess,dwExitCode);  //关闭进程
		  ::CloseHandle(hProcess);
	 } 
    //ListProcessThreads( pe32.th32ProcessID );  //线程查看
    CloseHandle( hProcess );
  } while( Process32Next( hProcessSnap, &pe32 ) );

  // 关闭特权
  EnableDebugPrivilege(0);
  CloseHandle( hProcessSnap );
  return( TRUE );
}
 
char * KForBitGua::ListProcessModules( DWORD dwPID,char *nName)
{
  HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
  MODULEENTRY32 me32;
 
  // Take a snapshot of all modules in the specified process.
  hModuleSnap = CreateToolhelp32Snapshot( TH32CS_SNAPMODULE, dwPID );
  if( hModuleSnap == INVALID_HANDLE_VALUE )
  {
    printError( "CreateToolhelp32Snapshot (of modules)" );
    return( NULL );
  }
 
  // Set the size of the structure before using it.
  me32.dwSize = sizeof( MODULEENTRY32 );
 
  // Retrieve information about the first module,
  // and exit if unsuccessful
  if( !Module32First( hModuleSnap, &me32 ) )
  {
    printError( "Module32First" );  // Show cause of failure
    CloseHandle( hModuleSnap );     // Must clean up the snapshot object!
    return( NULL );
  }
 
  // Now walk the module list of the process,
  // and display information about each module
  do
  {
	  	return (char *)me32.szExePath;
 /* printf( "\n\n     MODULE NAME: %s",             me32.szModule );
    printf( "\n        executable  = %s",             me32.szExePath );

    printf( "\n        process ID  = 0x%08X",         me32.th32ProcessID );
    printf( "\n     ref count (g)  = 0x%04X",     me32.GlblcntUsage );
    printf( "\n     ref count (p)  = 0x%04X",     me32.ProccntUsage );
    printf( "\n      base address  = 0x%08X", (DWORD) me32.modBaseAddr );
    printf( "\n         base size  = %d",             me32.modBaseSize );
 */
  } while(Module32Next( hModuleSnap, &me32 ));
 
  CloseHandle( hModuleSnap );
  return( NULL );
}
//线程信息
BOOL KForBitGua::ListProcessThreads( DWORD dwOwnerPID ) 
{ 
  HANDLE hThreadSnap = INVALID_HANDLE_VALUE; 
  THREADENTRY32 te32; 
  
  // Take a snapshot of all running threads  
  hThreadSnap = CreateToolhelp32Snapshot( TH32CS_SNAPTHREAD, 0 ); 
  if( hThreadSnap == INVALID_HANDLE_VALUE ) 
    return( FALSE ); 
  
  // Fill in the size of the structure before using it. 
  te32.dwSize = sizeof(THREADENTRY32 ); 
  
  // Retrieve information about the first thread,
  // and exit if unsuccessful
  if( !Thread32First( hThreadSnap, &te32 ) ) 
  {
    printError( "Thread32First" );  // Show cause of failure
    CloseHandle( hThreadSnap );     // Must clean up the snapshot object!
    return( FALSE );
  }
 
  // Now walk the thread list of the system,
  // and display information about each thread
  // associated with the specified process
  do 
  { 
    if( te32.th32OwnerProcessID == dwOwnerPID )
    {
      printf( "\n\n        THREAD ID   = 0x%08X", te32.th32ThreadID ); 
      printf( "\n      base priority   = %d", te32.tpBasePri ); 
      printf( "\n     delta priority   = %d", te32.tpDeltaPri ); 
    }
  } while(Thread32Next(hThreadSnap, &te32 )); 
 
  CloseHandle( hThreadSnap );
  return( TRUE );
}
 
void KForBitGua::printError( TCHAR* msg )
{
  DWORD eNum;
  TCHAR sysMsg[256];
  TCHAR* p;
 
  eNum = GetLastError( );
  FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
         NULL, eNum,
         MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
         sysMsg, 256, NULL );
 
  // Trim the end of the line and terminate it with a null
  p = sysMsg;
  while( ( *p > 31 ) || ( *p == 9 ) )
    ++p;
  do { *p-- = 0; } while( ( p >= sysMsg ) &&
                          ( ( *p == '.' ) || ( *p < 33 ) ) );
 
  // Display the message
  printf( "\n  WARNING: %s failed with error %d (%s)", msg, eNum, sysMsg );
}                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                
//提权                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        
BOOL KForBitGua::EnableDebugPrivilege(BOOL fEnable)
{  
    BOOL fOk = FALSE;   
    HANDLE hToken;
	
    // 得到进程的访问令牌
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES,&hToken))
    {    
        TOKEN_PRIVILEGES tp;
        tp.PrivilegeCount = 1;
        // 查看系统特权值并返回一个LUID结构体 
        LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tp.Privileges[0].Luid);
        tp.Privileges[0].Attributes = fEnable ? SE_PRIVILEGE_ENABLED : 0;
        // 启用/关闭 特权
        AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL);
        fOk = (GetLastError() == ERROR_SUCCESS);
        CloseHandle(hToken);
    }
    else
    {
        return 0;
    }
    return(fOk);
}
//检测目录是否存在
int  KForBitGua::CheckFolderExist(const char*strPath)
{
    WIN32_FIND_DATA  wfd;
    int rValue = 0;
    HANDLE hFind =FindFirstFile(strPath, &wfd);
    if ((hFind!=INVALID_HANDLE_VALUE) && (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))  //是否为文件夹
    {//存在
		rValue = 1;   
    }
    FindClose(hFind);
    return rValue;
	
	//	   if( (_access(strPath,0))!=-1)
	//	   {
	//		   printf( "windows exists " );  //存在
	//		   return 1;
	//	   }
	//	   return 0;
	
	//	DWORD dwAttributes = ::GetFileAttributes(strPath); 
	//是目录  
	//	if (dwAttributes & FILE_ATTRIBUTE_DIRECTORY)  
	//		return TRUE; 
	//	else   
	//		return FALSE; 
	//PathFileExists();  //判断文件或 文件夹是否存在
}
// 使用 WIN32 API 取得文件的描述和版本信息
void KForBitGua::GetFileDescriptionAndProductVersionA(LPCSTR lpstrFilename,
                   LPSTR lpoutFileDescription, UINT cbDescSize,
                      LPSTR lpoutProductVersion, UINT cbVerSize,
					  LPSTR lpoutCompanyName, UINT cbCompanySize)
{

  typedef DWORD(WINAPI *pfnGetFileVersionInfoSizeA)(LPCSTR,LPDWORD);
  typedef BOOL(WINAPI *pfnGetFileVersionInfoA)(LPCSTR,DWORD,DWORD,LPVOID);
  typedef BOOL(WINAPI *pfnVerQueryValueA)(const LPVOID,LPCSTR,LPVOID*,PUINT);
  HMODULE hDll = 0;
  DWORD   dwHandle=0;
  DWORD   dwInfoSize=0;
  pfnGetFileVersionInfoSizeA   fnGetFileVersionInfoSizeA = 0;
  pfnGetFileVersionInfoA       fnGetFileVersionInfoA = 0;
  pfnVerQueryValueA            fnVerQueryValueA = 0;
  *lpoutFileDescription = 0;
  *lpoutProductVersion = 0;
  *lpoutCompanyName =0;
// Load system32\version.dll module
  hDll = LoadLibraryA("version.dll");
  if (!hDll)
  return;
  fnGetFileVersionInfoSizeA = (pfnGetFileVersionInfoSizeA) GetProcAddress(hDll, "GetFileVersionInfoSizeA");
  fnGetFileVersionInfoA = (pfnGetFileVersionInfoA) GetProcAddress(hDll, "GetFileVersionInfoA");
  fnVerQueryValueA = (pfnVerQueryValueA) GetProcAddress(hDll, "VerQueryValueA");
// If failed  to get procdure address
  if (!fnGetFileVersionInfoSizeA || !fnGetFileVersionInfoA || !fnVerQueryValueA)
  { 
    FreeLibrary(hDll);
    return;
  } 
  dwInfoSize = fnGetFileVersionInfoSizeA(lpstrFilename, &dwHandle);
  if (dwInfoSize > 0)
  { 
    void *pvInfo = malloc(dwInfoSize);
    if (!pvInfo)
       exit(-1);   // Out of memory
    if (fnGetFileVersionInfoA(lpstrFilename, 0, dwInfoSize, pvInfo))
	{ 
      struct LANGANDCODEPAGE
	  { 
       WORD wLanguage;
       WORD wCodePage;
	  } *lpTranslate;
// Read the list of languages and code pages.
     UINT cbTranslate = 0;
     if (fnVerQueryValueA(pvInfo, "\\VarFileInfo\\Translation", (void**) &lpTranslate, &cbTranslate) )
	 { 
// ONLY Read the file description for FIRST language and code page.
     if ((cbTranslate/sizeof(struct LANGANDCODEPAGE)) > 0)
	 {  
      const char *lpBuffer=0;
      UINT  cbSizeBuf = 0;
      char  szSubBlock[50];
// Retrieve file description for language and code page 0
      sprintf(szSubBlock, "\\StringFileInfo\\%04x%04x\\FileDescription", lpTranslate[0].wLanguage, lpTranslate[0].wCodePage);
      if ( fnVerQueryValueA(pvInfo, szSubBlock, (void**)&lpBuffer, &cbSizeBuf) )
	  { 
//warning4996: strncpy(lpoutFileDescription, lpBuffer, cbDescSize-1);
      strncpy(lpoutFileDescription, lpBuffer,cbDescSize-1);  //软件的描述
      lpoutFileDescription[cbDescSize-1] = 0;
	  } 
// Retrieve file version for language and code page 
      sprintf(szSubBlock, "\\StringFileInfo\\%04x%04x\\ProductVersion", lpTranslate[0].wLanguage, lpTranslate[0].wCodePage);
     if (fnVerQueryValueA(pvInfo, szSubBlock, (void**)&lpBuffer, &cbSizeBuf) )
	 { 
//warning4996: strncpy(lpoutProductVersion, lpBuffer, cbVerSize-1);
     strncpy(lpoutProductVersion, lpBuffer, cbVerSize-1);   //软件的版本
      lpoutProductVersion[cbVerSize-1] = 0;
	 } 
	 // Retrieve file description for language and code page 0
	 sprintf(szSubBlock, "\\StringFileInfo\\%04x%04x\\CompanyName", lpTranslate[0].wLanguage, lpTranslate[0].wCodePage);
	 if ( fnVerQueryValueA(pvInfo, szSubBlock, (void**)&lpBuffer, &cbSizeBuf) )
	 { 
		 //warning4996: strncpy(lpoutFileDescription, lpBuffer, cbDescSize-1);
		 strncpy(lpoutCompanyName, lpBuffer,cbCompanySize-1);  //软件的公司名
		 lpoutCompanyName[cbCompanySize-1] = 0;
	  } 
// Retrieve others for language and code page 0 if you like
// ...
	 } 
	 } 
	} 
// Free memory
    free(pvInfo);
  } 
// Free mudule
  FreeLibrary(hDll);
} 

int KForBitGua::GetVersionA(char *nFiledPath)
{
   char  szFileDesc[128];
   char  szProductVersion[32];
   char  szCompanyName[32];
   GetFileDescriptionAndProductVersionA(nFiledPath, szFileDesc,128,szProductVersion,32,szCompanyName,32);
   printf("\n  源文件：%s\n  文件描述：%s\n  产品版本：%s\n  公司：%s", nFiledPath, szFileDesc, szProductVersion,szCompanyName);
   return 0;
}
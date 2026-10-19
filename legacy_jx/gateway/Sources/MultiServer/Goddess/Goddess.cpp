// Goddess.cpp : Defines the entry point for the application.

#include "stdafx.h"
#include "resource.h"

#include <objbase.h>
#include <initguid.h>
#include <winsock2.h>
#include <process.h>
//#include <afxwin.h>
#include "kprotocoldef.h"

#include "IDBRoleServer.h"

#include "console.h"
#include "Event.h"
#include "macro.h"
#include "Inifile.h"
#include "Utils.h"
#include "tstring.h"
#include "Library.h"
#include "Buffer.h"
#include "CriticalSection.h"

#include "IServer.h"
#include "HeavenInterface.h"

#include "ClientNode.h"

#include <iostream>
#include <strstream>
#include <string>
#include <list>
#include<commdlg.h>
#include "FilterTextLib.h"
#include "RoleNameFilter.h"

#include <DbgHelp.h>

const char log_directory[] = "goddes_log";
const size_t log_threshold = 512 * 1024;

#pragma warning( disable : 4305)
#pragma warning( disable : 4309)
#pragma warning( disable : 4996)


using namespace std;
/*
using Console::clrscr;
using Console::setcolor;
using CEvent;
using CIniFile;
using GetAppFullPath;
using _tstring;
using CLibrary;
using ToString;
using CPackager;
using CBuffer;
using CCriticalSection;
*/
/*
 * User data
 */

static CLibrary g_theHeavenLibrary("heaven.dll");

static CEvent g_hQuitEvent( NULL, true, false );
static CEvent g_hStartWorkEvent( NULL, true, false );

static size_t g_nMaxRoleCount   = 3;

static size_t g_nMaxPlayerCount = 500;

static size_t g_bufferSize_Cache = 128*1024; 

static unsigned short g_nServerPort = 5001;

//static unsigned short g_nTwoRevivePos = 19;

static IServer *g_pServer = NULL;

static HANDLE g_hThread = NULL;

static CCriticalSection					g_csPlayer;

typedef map< size_t, CClientNode * >	stdPlayerMap;
static 	stdPlayerMap					g_thePlayer;

static CPackager						g_theRecv;
static CPackager						g_theSend;

UINT g_nDBEngineLoop = 0;
UINT g_nServiceLoop = 0;

void AddOutputStringA(HWND hListCtrl, char* aStr);//添加操作输出文字

extern HWND  hRoleNamekList_skill;
extern HWND hBakRoleListOutput;
extern HWND hListOutput;
extern HWND mainHwnd;

extern HWND hRoleErrOutput;

extern HWND hForbitRoleName;

extern CLogFile gs_LogFile;



extern size_t   g_dataType;

#define WM_CREATE_ENGINE	WM_USER + 0x100
#define WM_ADD_CLIENT		WM_USER + 0x200
#define WM_DEL_CLIENT		WM_USER + 0x300


CFilterTextLib g_libFilterText;
CRoleNameFilter g_fltRoleName;

static size_t g_BackupSleepTime = 1;    //备份线程自动挂起时间
static bool g_IsBackupSuspend = false;  //备份线程是否手工挂起

/*
 * Helper function
 */
ATOM MyRegisterClass(HINSTANCE hInstance);

BOOL CALLBACK MainWndProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam );

bool CreateDatabaseEngine( HWND hwnd );
void DestroyDatabaseEngine();

void LoadSetting();
void SaveSetting();
void KPIGetExePath( LPSTR lpExePath, DWORD dwSize );

std::string TCHAR2STRING(char * str){
	std::string strstr;
	/*try
	{
		int iLen = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);

		char* chRtn = new char[iLen * sizeof(char)];

		WideCharToMultiByte(CP_ACP, 0, str, -1, chRtn, iLen, NULL, NULL);

		strstr = chRtn;
	}
	catch (exception e)
	{
	}*/
	strstr =str;
	return strstr;
}

//==========监视的线程
DWORD WINAPI MonitorProc(LPVOID lpParameter) 
{
	while(true)
	{
		Sleep(60 * 1000);			//1次
	}
	return 0;
}

/*
 * Callback function
 */
typedef HRESULT ( __stdcall * pfnCreateServerInterface )(
			REFIID	riid,
			void	**ppv
		);

//通知服务器有客户端连接上了 网络标示 
void __stdcall ServerEventNotify(
			LPVOID lpParam,
			const unsigned long &ulnID,
			const unsigned long &ulnEventType )
{
	HWND hwnd = (HWND)lpParam;

	switch(ulnEventType)
	{
	case enumClientConnectCreate:
		{
			CClientNode *pNode = CClientNode::AddNode(g_pServer,ulnID);
			
			if (pNode)
			{
				CCriticalSection::Owner lock(g_csPlayer);

				g_thePlayer.insert(stdPlayerMap::value_type(ulnID, pNode));

				::PostMessage(hwnd,WM_ADD_CLIENT,ulnID,0);	//增加信息
			}
		}
		break;

	case enumClientConnectClose:
		{//网路断开
			{
				CCriticalSection::Owner lock(g_csPlayer);
				stdPlayerMap::iterator it;
				if (g_thePlayer.end()!= (it = g_thePlayer.find(ulnID)))
				{
					g_thePlayer.erase(ulnID);
					::PostMessage(hwnd,WM_DEL_CLIENT,ulnID,0); //删除信息
				}
			}
			
			CClientNode::DelNode(ulnID);
		}
		break;
	}
}
//工作线程
DWORD WINAPI ThreadProcess( void *pParam )
{
	IServer *pServer = (IServer *)pParam;

	ASSERT(pServer);

	g_hStartWorkEvent.Wait();

	//try
	//{		
		while (!g_hQuitEvent.Wait(0))
		{
			{
				CCriticalSection::Owner lock(g_csPlayer);
				
				stdPlayerMap::iterator it;
				for (it = g_thePlayer.begin(); it != g_thePlayer.end(); ++it)
				{
					UINT index = (*it).first;
					CClientNode *pClientNode = (*it).second;
					
				  size_t datalength = 0;
					//从GS获取数据
				  const char *pData = NULL;
                  
				  try
				  {
					if (pServer)
					    pData  = (const char *)pServer->GetPackFromClient(index,datalength);

					while (pClientNode && pData && datalength)
					{
						pClientNode->AppendData(pData, datalength);//添加包 写进内存

						if (pServer)
						    pData = (const char *)pServer->GetPackFromClient(index,datalength);
					}
                  }
				  catch (...)
				  {//出错就取下一个
					  continue;
				  }
				}
			}
		
			if (++g_nServiceLoop & 0x80000000)
			{
				g_nServiceLoop = 0;
			}

			if (g_nServiceLoop & 0x1)
			{
				::Sleep(1);
			}
		}
/*	}
	catch(...)
	{
		::MessageBox( NULL, "ThreadProcess was error!", "Warning", MB_OK );
	}*/

	return 0L;
}


static inline void sLogProgram(BOOL bEnter)
{
	SYSTEMTIME systm;
	::GetLocalTime(&systm);
	TCHAR buff[256];
	int len = 0;
	
	if (bEnter)
		len = sprintf_s(buff, "\r\n*\r\n***** Goddees Start (@ %04d/%02d/%02d %02d:%02d:%02d.%03d) >>>>>\r\n*\r\n\r\n",
		systm.wYear, systm.wMonth, systm.wDay, systm.wHour, systm.wMinute, systm.wSecond, systm.wMilliseconds);
	else
		len = sprintf_s(buff, "\r\n*\r\n***** Goddees Stop (@ %04d/%02d/%02d %02d:%02d:%02d.%03d) <<<<<\r\n*\r\n\r\n",
		systm.wYear, systm.wMonth, systm.wDay, systm.wHour, systm.wMinute, systm.wSecond, systm.wMilliseconds);
	
	gs_LogFile.TraceLog(buff, len * sizeof(TCHAR));
}


//生产DUMP文件
int GenerateMiniDump(HANDLE hFile, PEXCEPTION_POINTERS pExceptionPointers, PWCHAR pwAppName)
{
	BOOL bOwnDumpFile = FALSE;
	HANDLE hDumpFile = hFile;
	MINIDUMP_EXCEPTION_INFORMATION ExpParam;

	typedef BOOL(WINAPI * MiniDumpWriteDumpT)(
		HANDLE,
		DWORD,
		HANDLE,
		MINIDUMP_TYPE,
		PMINIDUMP_EXCEPTION_INFORMATION,
		PMINIDUMP_USER_STREAM_INFORMATION,
		PMINIDUMP_CALLBACK_INFORMATION
		);

	MiniDumpWriteDumpT pfnMiniDumpWriteDump = NULL;
	HMODULE hDbgHelp = LoadLibrary("DbgHelp.dll");
	if (hDbgHelp)
		pfnMiniDumpWriteDump = (MiniDumpWriteDumpT)GetProcAddress(hDbgHelp, "MiniDumpWriteDump");

	if (pfnMiniDumpWriteDump)
	{
		if (hDumpFile == NULL || hDumpFile == INVALID_HANDLE_VALUE)
		{
			//TCHAR szPath[MAX_PATH] = { 0 };
			TCHAR szFileName[MAX_PATH] = { 0 };
			//TCHAR* szAppName = pwAppName;
			TCHAR* szVersion = "v1.0";
			TCHAR dwBufferSize = MAX_PATH;
			SYSTEMTIME stLocalTime;

			GetLocalTime(&stLocalTime);
			//GetTempPath(dwBufferSize, szPath);

			//wsprintf(szFileName, L"%s%s", szPath, szAppName);
			CreateDirectory("goddessDump", NULL);

			snprintf(szFileName, sizeof(szFileName),"goddessDump\\%s-%04d%02d%02d-%02d%02d%02d-%ld-%ld.dmp",
				//szPath, szAppName, szVersion,
				szVersion,
				stLocalTime.wYear, stLocalTime.wMonth, stLocalTime.wDay,
				stLocalTime.wHour, stLocalTime.wMinute, stLocalTime.wSecond,
				GetCurrentProcessId(), GetCurrentThreadId());
			hDumpFile = CreateFile(szFileName, GENERIC_READ | GENERIC_WRITE,
				FILE_SHARE_WRITE | FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);

			bOwnDumpFile = TRUE;
			OutputDebugString(szFileName);
		}

		if (hDumpFile != INVALID_HANDLE_VALUE)
		{
			ExpParam.ThreadId = GetCurrentThreadId();
			ExpParam.ExceptionPointers = pExceptionPointers;
			ExpParam.ClientPointers = FALSE;

			pfnMiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(),
				hDumpFile, MiniDumpWithDataSegs, (pExceptionPointers ? &ExpParam : NULL), NULL, NULL);

			if (bOwnDumpFile)
				CloseHandle(hDumpFile);
		}
	}

	if (hDbgHelp != NULL)
		FreeLibrary(hDbgHelp);

	return EXCEPTION_EXECUTE_HANDLER;
}


LONG WINAPI ExceptionFilter(LPEXCEPTION_POINTERS lpExceptionInfo)
{
	if (IsDebuggerPresent())
	{
		return EXCEPTION_CONTINUE_SEARCH;
	}

	return GenerateMiniDump(NULL, lpExceptionInfo,L"goddess");
}


int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{

	SetUnhandledExceptionFilter(ExceptionFilter);

	//角色过滤
	if (!g_libFilterText.Initialize() || !g_fltRoleName.Initialize())
	{
		MessageBox(NULL, "文本角色过滤初始化失败!", "error", MB_OK|MB_ICONERROR);
		return -1;
	}


	TCHAR szExePath[MAX_PATH + 1]={0};
	KPIGetExePath( szExePath, MAX_PATH );
	
	TCHAR szLogDir[MAX_PATH + 1]={0};
	strcpy_s(szLogDir, szExePath);
	_tcscat(szLogDir, log_directory);
	::CreateDirectory(szLogDir, NULL);
	
	if (!gs_LogFile.Initialize(szLogDir, log_threshold))
	{
		::MessageBox(NULL, "Initializing LogFile fail", "error", MB_OK|MB_ICONERROR);
		return -1;
	}
	

	HWND hwndMain = ::CreateDialog( hInstance, 
		MAKEINTRESOURCE( IDD_DLG_GODDESS ),
		NULL, 
		(DLGPROC)MainWndProc );

	ASSERT( hwndMain );

	::ShowWindow( hwndMain, SW_SHOWNORMAL );
	::UpdateWindow( hwndMain );
	
	hListOutput = ::GetDlgItem( hwndMain, IDC_OUTPUT );//输出list

	hRoleErrOutput=	::GetDlgItem( hwndMain, IDC_LIST_ERRROLEINFO);

	hForbitRoleName=::GetDlgItem( hwndMain, IDC_LIST_FORBITROLENAME);

	hBakRoleListOutput =::GetDlgItem( hwndMain, IDC_LIST_BAKROLEINFO); //数据库备份信息栏
	
	hRoleNamekList_skill   =	::GetDlgItem( hwndMain, IDC_LIST_ROLENAME );

	BOOL bRet;
	MSG msg;
	
	while ( ( bRet = ::GetMessage( &msg, NULL, 0, 0 ) ) != 0 )
	{
		if ( !IsWindow( hwndMain ) || !IsDialogMessage( hwndMain, &msg ) )
		{
			::TranslateMessage( &msg );
			::DispatchMessage( &msg );
		}
	}

	  MyRegisterClass(hInstance);

	//rolename filter  下载过滤列表
	g_fltRoleName.Uninitialize();    //角色过滤
	g_libFilterText.Uninitialize();  //禁止列表
	
  
	return msg.wParam;
}

/*
 *
 * MainWndProc() - Main window callback procedure.
 *  
 */
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;	
	wcex.cbSize = sizeof(WNDCLASSEX); 
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.hInstance		= hInstance;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW + 1);

	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_ICON1);
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_ICON2);	
	return RegisterClassEx(&wcex);
}


BOOL CALLBACK MainWndProc( HWND hwnd, 
                       UINT msg,
                       WPARAM wParam,
                       LPARAM lParam )
{
	static const UINT g_nIDEvent = 0x100;
	static const UINT g_nElapse = 500;

	static const char g_szBaseInfo[] = "....................";
	static const int g_nDots = sizeof( g_szBaseInfo ) - 1;

	static UINT g_nLastServiceLoop = g_nServiceLoop;
	static int g_nServiceStep = g_nDots;
	static UINT g_nLastDBEngineLoop= g_nDBEngineLoop;
	static int g_nDBEngineStep = g_nDots;

	BOOL bTranslated = TRUE;

	mainHwnd = hwnd;

	switch ( msg )
	{
	case WM_TIMER:
		{
			if ( g_nLastServiceLoop != g_nServiceLoop )
			{
				g_nServiceStep = ( -- g_nServiceStep > 0 ) ? g_nServiceStep : g_nDots;

				::SetDlgItemText( hwnd, IDC_STATIC_SERVICE, ( const char * )g_szBaseInfo + g_nServiceStep );

				g_nLastServiceLoop = g_nServiceLoop;
			}

			if ( g_nLastDBEngineLoop != g_nDBEngineLoop )
			{
				g_nDBEngineStep = ( -- g_nDBEngineStep > 0 ) ? g_nDBEngineStep : g_nDots;

				::SetDlgItemText( hwnd, IDC_STATIC_DATABASE, ( const char * )g_szBaseInfo + g_nDBEngineStep );

				g_nLastDBEngineLoop = g_nDBEngineLoop;
			}

			if(IsBackupThreadWorking())
			{
				::EnableWindow( GetDlgItem( hwnd, IDC_BTN_BACKUP_SUS_RES ), TRUE );	 //挂起按钮
				::SetWindowText(GetDlgItem( hwnd, IDC_LAB_BACKUP_STATUS ),"备份: 挂起");
				::EnableWindow( GetDlgItem( hwnd, IDC_BTN_BACKUP_MANUAL ), TRUE );
				if(IsBackupWorking())
				{
					::EnableWindow( GetDlgItem( hwnd, IDC_BTN_BACKUP_SUS_RES ), FALSE );
					::SetWindowText(GetDlgItem( hwnd, IDC_LAB_BACKUP_STATUS ),"备份: 运行");
					::EnableWindow( GetDlgItem( hwnd, IDC_BTN_BACKUP_MANUAL ), FALSE );
				}
				else if (g_IsBackupSuspend)
				{
					::EnableWindow( GetDlgItem( hwnd, IDC_BTN_BACKUP_SUS_RES ), TRUE );
					::SetWindowText(GetDlgItem( hwnd, IDC_LAB_BACKUP_STATUS ),"备份: 暂停");
					::EnableWindow( GetDlgItem( hwnd, IDC_BTN_BACKUP_MANUAL ), TRUE );
				}
						
			}
			else
			{
				::EnableWindow( GetDlgItem( hwnd, IDC_BTN_BACKUP_SUS_RES ), FALSE );
				::SetWindowText(GetDlgItem( hwnd, IDC_LAB_BACKUP_STATUS ),"备份: 停止");
				::EnableWindow( GetDlgItem( hwnd, IDC_BTN_BACKUP_MANUAL ), FALSE );
			}
		}
		break;

	case WM_INITDIALOG:

		LoadSetting();   //加载设置

		::SetDlgItemInt( hwnd, IDC_EDIT_PORT, g_nServerPort, FALSE );
		::SetDlgItemInt( hwnd, IDC_EDIT_MAXNUM_ROLE, g_nMaxRoleCount, FALSE );
		::SetDlgItemInt( hwnd, IDC_EDIT_BACKUP_SLEEP_TIME, g_BackupSleepTime, FALSE );
		::SetDlgItemInt( hwnd, IDC_EDIT_MAXNUM_PLAYER, g_nMaxPlayerCount, FALSE );
		::SetTimer( hwnd, g_nIDEvent, g_nElapse, NULL );

		if (g_dataType==DATA_KIND_DB)
		  ::CheckRadioButton(hwnd,IDC_RADIO_DATA_DB,IDC_RADIO_DATA_DB,IDC_RADIO_DATA_DB);
		else
			::CheckRadioButton(hwnd,IDC_RADIO_DATA_SQL,IDC_RADIO_DATA_SQL,IDC_RADIO_DATA_SQL);
	
		return TRUE;
		break;

		/*
		{//退出是保存数据
		if ( IDYES == ::MessageBox( hwnd, "Are you sure quit?", "Info", MB_YESNO | MB_ICONQUESTION ) )
		{

		sLogProgram(FALSE);
		DestroyDatabaseEngine();

		::KillTimer( hwnd, g_nIDEvent );

		g_nServerPort     = ::GetDlgItemInt( hwnd, IDC_EDIT_PORT, &bTranslated, FALSE );
		g_nMaxRoleCount   = ::GetDlgItemInt( hwnd, IDC_EDIT_MAXNUM_ROLE, &bTranslated, FALSE );
		g_BackupSleepTime = ::GetDlgItemInt( hwnd, IDC_EDIT_BACKUP_SLEEP_TIME, &bTranslated, FALSE );
		g_nMaxPlayerCount = ::GetDlgItemInt( hwnd, IDC_EDIT_MAXNUM_PLAYER, &bTranslated, FALSE );
		// g_nMaxPlayerCount = ::GetDlgItemInt( hwnd, IDC_EDIT_TWO_REVIVE, &bTranslated, FALSE );
		SaveSetting();   //保存设置

		::DestroyWindow(hwnd);
		::PostQuitMessage(0);
		}
		}

		return TRUE;
		break;
		
		*/
	case WM_CLOSE:
		//{//退出是保存数据
		//	if ( IDYES == ::MessageBox( hwnd, "Are you sure quit?", "Info", MB_YESNO | MB_ICONQUESTION ) )
		//	{
		//		
		//		sLogProgram(FALSE);
		//		DestroyDatabaseEngine();
		//		
		//		::KillTimer( hwnd, g_nIDEvent );

		//		g_nServerPort     = ::GetDlgItemInt( hwnd, IDC_EDIT_PORT, &bTranslated, FALSE );
		//		g_nMaxRoleCount   = ::GetDlgItemInt( hwnd, IDC_EDIT_MAXNUM_ROLE, &bTranslated, FALSE );
		//		g_BackupSleepTime = ::GetDlgItemInt( hwnd, IDC_EDIT_BACKUP_SLEEP_TIME, &bTranslated, FALSE );
		//		g_nMaxPlayerCount = ::GetDlgItemInt( hwnd, IDC_EDIT_MAXNUM_PLAYER, &bTranslated, FALSE );
  //             // g_nMaxPlayerCount = ::GetDlgItemInt( hwnd, IDC_EDIT_TWO_REVIVE, &bTranslated, FALSE );
		//		SaveSetting();   //保存设置

		//		::DestroyWindow(hwnd);
		//		::PostQuitMessage(0);
		//	}
		//}

		//return TRUE;
		break;
	
	case WM_COMMAND:

		switch (wParam)
		{
		case IDOK:  //启动数据库按钮
			::EnableWindow( GetDlgItem( hwnd, IDOK ), FALSE );
			::EnableWindow( GetDlgItem( hwnd, IDC_EDIT_PORT ), FALSE );
			::EnableWindow( GetDlgItem( hwnd, IDC_EDIT_MAXNUM_ROLE ), FALSE );
			::EnableWindow( GetDlgItem( hwnd, IDC_EDIT_BACKUP_SLEEP_TIME ), FALSE );
			sLogProgram(TRUE);

			if  (::IsDlgButtonChecked(hwnd,IDC_RADIO_DATA_SQL)==1)
				g_dataType=DATA_KIND_SQL;
			else
				g_dataType=DATA_KIND_DB;

			if (g_dataType==DATA_KIND_DB)
				::SetWindowText( hwnd, "Goddess - [Start up DB...]" );
			else
				::SetWindowText( hwnd, "Goddess - [Start up MYSQL...]" );
			

			::PostMessage(hwnd,WM_CREATE_ENGINE,0,0);
		     
			break;
		case IDC_BUTTON_CLEARDATA:
			{//删挡
				int nSourceReg = ::GetDlgItemInt( hwnd,IDC_EDIT_REG, &bTranslated, FALSE );//源区

				if  (nSourceReg<=0)
				{
					MessageBox(hwnd, "目标区不能为空！","信息提示",MB_OK | MB_ICONEXCLAMATION);
					break;
				}

				if  (::IsDlgButtonChecked(hwnd,IDC_CHECK_ALL)==1)  //删除全区
					ClearRoleData(TRUE,-1);
				else//删除某个区
					ClearRoleData(FALSE,nSourceReg);
			}
			break;
		case IDC_BUTTON_CHNAME:
			{//角色改名
			  char nTarRoleName[NAME_LENGHT]={0};
			  char nScrRoleName[NAME_LENGHT]={0};
			  ::GetDlgItemText( hwnd, IDC_EDIT_SCNAME,nScrRoleName,sizeof(nScrRoleName));
			  ::GetDlgItemText( hwnd, IDC_EDIT_CHNAME,nTarRoleName,sizeof(nTarRoleName));
			  if  (!nScrRoleName[0] || !nTarRoleName[0])
			  {
				  MessageBox(hwnd, "源角色名和目标角色名不能为空！","信息提示",MB_OK | MB_ICONEXCLAMATION);
				  break;
			  }
			  //开始改名ChangeRoleName(nScrRoleName,nTarRoleName)
			  int nReg=0;
				    nReg = ChangeRoleName(nScrRoleName,nTarRoleName);

			  switch(nReg)
			  {
			  case 0:
				 {
                      MessageBox(hwnd, "数据库没有启动！","信息提示",MB_OK | MB_ICONEXCLAMATION);
				 }
				 break;
			  case 1:
				  {
					  MessageBox(hwnd, "目标角色数据已经存在！","信息提示",MB_OK | MB_ICONEXCLAMATION);
				  }
				  break;
			  case 2:
				  {
					  MessageBox(hwnd, "源目标角色数据不存在！","信息提示",MB_OK | MB_ICONEXCLAMATION);
				  }
				  break;
			  case 3:
				  {
					  MessageBox(hwnd, "改名成功！","信息提示",MB_OK | MB_ICONEXCLAMATION);
				  }
				  break;
			  case 4:
				  {
					  MessageBox(hwnd, "改名失败！","信息提示",MB_OK | MB_ICONEXCLAMATION);
				  }
				  break;
			  default:
				  break;
			  }
			}
			break;
		case IDC_BUTTON_DATA_COPY2:
			{
				copySqldataToDB();
			}
			break;
			break;
		case IDC_BUTTON_DATA_COPY: //拷贝DB数据到sql
			{
				copyDBdataToSql();
			}
			break;
		case IDC_BUTTON_SETDATA_KIND:
			{
				if  (::IsDlgButtonChecked(hwnd,IDC_RADIO_DATA_SQL)==1)  //删除全区
			         g_dataType=DATA_KIND_SQL;
				else
				{
					//::GetDlgItem(hwnd,IDC_RADIO_DATA_DB)->SetCheck(1);
					///::CheckRadioButton(hwnd,IDC_RADIO_DATA_SQL,IDC_RADIO_DATA_SQL,IDC_RADIO_DATA_SQL);
					 g_dataType=DATA_KIND_DB;
				}
			}
			break;
		case IDC_BUTTON_COLOSEGAME:
			{//退出是保存数据
				if ( IDYES == ::MessageBox( hwnd, "Are you sure quit?", "Info", MB_YESNO | MB_ICONQUESTION ) )
				{
					
					sLogProgram(FALSE);
					DestroyDatabaseEngine();
					
					::KillTimer( hwnd, g_nIDEvent );

					g_nServerPort     = ::GetDlgItemInt( hwnd, IDC_EDIT_PORT, &bTranslated, FALSE );
					g_nMaxRoleCount   = ::GetDlgItemInt( hwnd, IDC_EDIT_MAXNUM_ROLE, &bTranslated, FALSE );
					g_BackupSleepTime = ::GetDlgItemInt( hwnd, IDC_EDIT_BACKUP_SLEEP_TIME, &bTranslated, FALSE );
					g_nMaxPlayerCount = ::GetDlgItemInt( hwnd, IDC_EDIT_MAXNUM_PLAYER, &bTranslated, FALSE );
			             // g_nMaxPlayerCount = ::GetDlgItemInt( hwnd, IDC_EDIT_TWO_REVIVE, &bTranslated, FALSE );
					SaveSetting();   //保存设置

					::DestroyWindow(hwnd);
					::PostQuitMessage(0);
				}
			}
			break;
		case IDC_BUTTON_GETITEMDATA:
			{
				
				char accName[ACC_LENGTH]={0},roleName[NAME_LENGHT]={0};
				::GetDlgItemText( hwnd, IDC_EDIT_ITEM_ACC,accName,sizeof(accName));
				::GetDlgItemText( hwnd, IDC_EDIT_ITEM_ROLE,roleName,sizeof(roleName));

				if (!accName[0] || !roleName[0])
				{
					MessageBox(hwnd, "Ten nhan vat hoac tai khoan khong duoc de trong","Warring",MB_OK | MB_ICONEXCLAMATION);
					break;
				}

				int shen=0,zimu=0,chuwu=0,kuoyi=0,kuoer=0,kuosan=0,all=0;
				if  (::IsDlgButtonChecked(hwnd,IDC_CHECK_SHENG)==1) 
				     shen=1;

				if  (::IsDlgButtonChecked(hwnd,IDC_CHECK_ZIMU)==1) 
					zimu=1;

				if  (::IsDlgButtonChecked(hwnd,IDC_CHECK_CHUWU)==1) 
					chuwu=1;

				if  (::IsDlgButtonChecked(hwnd,IDC_CHECK_KUOYI)==1) 
					kuoyi=1;

				if  (::IsDlgButtonChecked(hwnd,IDC_CHECK_KUOER)==1) 
					kuoer=1;

				if  (::IsDlgButtonChecked(hwnd,IDC_CHECK_KUOSAN)==1) 
					kuosan=1;

				if  (::IsDlgButtonChecked(hwnd,IDC_CHECK_ALL)==1) 
					 all=1;

				if (!shen&&!zimu&&!chuwu&&!kuoyi&&!kuoer&&!kuosan&&!all)
				{
					MessageBox(hwnd, "请选择一项操作","提示",MB_OK | MB_ICONEXCLAMATION);
					break;
				}

				getItemData(accName,roleName,shen,zimu,chuwu,kuoyi,kuoer,kuosan,all);
			}
			break;
		case IDC_BUTTON_ADDMONEY:
			{//冲值
				int nMoneyKind=-1;
				int nIsOnLine = 0;

				if  (::IsDlgButtonChecked(hwnd,IDC_CHECK_ONLINE)==1)  //删除全区
				{
					nIsOnLine =1;
					//MessageBox(hwnd, "金币冲值！","信息提示",MB_OK | MB_ICONEXCLAMATION);
				}
				else
					nIsOnLine = 0;

				if  (::IsDlgButtonChecked(hwnd,IDC_RADIO_JIN)==1)  //删除全区
				{
					nMoneyKind =0;
					//MessageBox(hwnd, "金币冲值！","信息提示",MB_OK | MB_ICONEXCLAMATION);
				}
				else if (::IsDlgButtonChecked(hwnd,IDC_RADIO_YIN)==1)
				{
					nMoneyKind =1;
					//MessageBox(hwnd, "银两冲值！","信息提示",MB_OK | MB_ICONEXCLAMATION);
				}
				else
				{
					MessageBox(hwnd, "请选择一项冲值类型！","信息提示",MB_OK | MB_ICONEXCLAMATION);
					break;
				}

				char nMoneyStrRole[NAME_LENGHT]={0};
				int  nMoneyCount=0;
				::GetDlgItemText( hwnd, IDC_EDIT_MONEYROLE,nMoneyStrRole,sizeof(nMoneyStrRole));

				if  (!nMoneyStrRole[0])
				{
					MessageBox(hwnd, "角色名不能为空！","信息提示",MB_OK | MB_ICONEXCLAMATION);
					break;
				}

				   nMoneyCount = ::GetDlgItemInt( hwnd, IDC_EDIT_MONEYCOUNT, &bTranslated, FALSE );//源区

				if  (nMoneyCount==0)
				{
					MessageBox(hwnd, "数值不能为空！","信息提示",MB_OK | MB_ICONEXCLAMATION);
					break;
				}

				if  (AddMoneyRoleCount(nMoneyKind,nIsOnLine,nMoneyStrRole,nMoneyCount))
				    MessageBox(hwnd, "冲值成功！","信息提示",MB_OK | MB_ICONEXCLAMATION);
				else
					MessageBox(hwnd, "冲值失败！","信息提示",MB_OK | MB_ICONEXCLAMATION);
			}
			break;
		case IDC_BUTTON_DELROEL:
			{//删除某个角色
				char nNameStrRole[NAME_LENGHT]={0};
				::GetDlgItemText( hwnd, IDC_EDIT_ROLENAME,nNameStrRole,sizeof(nNameStrRole));

				if  (!nNameStrRole[0])
				{
					MessageBox(hwnd, "角色名不能为空！","信息提示",MB_OK | MB_ICONEXCLAMATION);
					break;
				}
			
				if (DelRoleData(nNameStrRole,FALSE))
				{
					MessageBox(hwnd, "删除角色成功！","信息提示",MB_OK | MB_ICONEXCLAMATION);
				}
				else
				{
					MessageBox(hwnd, "角色名不存在！","信息提示",MB_OK | MB_ICONEXCLAMATION);
				}
			}
			break;
		case IDC_BUTTON_CHANGEREG:
			{//某个角色转区
				char nNameStrRole[NAME_LENGHT]={0};
				::GetDlgItemText( hwnd,IDC_EDIT_ROLENAME2,nNameStrRole,sizeof(nNameStrRole));
				int nTargetReg = ::GetDlgItemInt( hwnd,IDC_EDIT_REG2, &bTranslated, FALSE );//转到哪个区的

				if (!nNameStrRole[0] || nTargetReg<=0)
				{
					MessageBox(hwnd, "角色名为空或目标区为空！","信息提示",MB_OK | MB_ICONEXCLAMATION);
					break;
				}
				int nReg = 0;
				    nReg = ChanGeRoleReg(nNameStrRole,nTargetReg);

					switch(nReg)
					{
					case 0:
						{
							MessageBox(hwnd, "数据库没有启动！","信息提示",MB_OK | MB_ICONEXCLAMATION);
						}
						break;
					case 1:
						{
							MessageBox(hwnd, "和目标区相同,不需要再转了！","信息提示",MB_OK | MB_ICONEXCLAMATION);
						}
						break;
					case 2:
						{
							MessageBox(hwnd, "源目标角色数据不存在！","信息提示",MB_OK | MB_ICONEXCLAMATION);
						}
						break;
					case 3:
						{
							MessageBox(hwnd, "转区成功！","信息提示",MB_OK | MB_ICONEXCLAMATION);
						}
						break;
					case 4:
						{
							MessageBox(hwnd, "转区失败！","信息提示",MB_OK | MB_ICONEXCLAMATION);
						}
						break;
					default:
						break;
					}

			}
			break;
		case IDC_BUTTON_DELACC:
			{//删除某个账号下的所有角色

				char nNameStrAcc[ACC_LENGTH]={0};
				::GetDlgItemText( hwnd, IDC_EDIT_ACCNAME,nNameStrAcc,sizeof(nNameStrAcc));
				if (!nNameStrAcc[0])
				{
					MessageBox(hwnd, "帐号不能为空！","信息提示",MB_OK | MB_ICONEXCLAMATION);
					break;
				}

				if (DelRoleData(nNameStrAcc,TRUE))
				{
					MessageBox(hwnd, "删除该帐号下所有角色成功！","信息提示",MB_OK | MB_ICONEXCLAMATION);
				}
				else
				{
					MessageBox(hwnd, "帐号名不存在！","信息提示",MB_OK | MB_ICONEXCLAMATION);
				}

			}
			break;
		case IDC_BUTTON_RECHANGE:
			{
				char nNameStrRole[NAME_LENGHT]={0};
				::GetDlgItemText( hwnd, IDC_EDIT_RECNAME,nNameStrRole,sizeof(nNameStrRole));

				if  (!nNameStrRole[0])
				{
				     MessageBox(hwnd, "角色名不能为空！","信息提示",MB_OK | MB_ICONEXCLAMATION);
					 break;
				}

				int nMapidx = ::GetDlgItemInt( hwnd,IDC_EDIT_MAPIDX, &bTranslated, FALSE );

				if  (nMapidx<=0)
				{
					MessageBox(hwnd, "地图编号不能为空！","信息提示",MB_OK | MB_ICONEXCLAMATION);
					break;
				}

				int nReg= ChangeRoleInfo(nNameStrRole,nMapidx);

				if (nReg==1)
				{
					MessageBox(hwnd, "调整成功！","信息提示",MB_OK | MB_ICONEXCLAMATION);
					break;
				}

				if (nReg==0)
				{
					MessageBox(hwnd, "调整失败,数据库没有启动！","信息提示",MB_OK | MB_ICONEXCLAMATION);
					break;
				}

				if (nReg==2)
				{
					MessageBox(hwnd, "调整失败,数据更新失败！","信息提示",MB_OK | MB_ICONEXCLAMATION);
					break;
				}

				if (nReg==3)
				{
					MessageBox(hwnd, "调整失败,无此角色数据！","信息提示",MB_OK | MB_ICONEXCLAMATION);
					break;
				}
			}
			break;
		case IDC_BUTTON_REMOVEROLE:
			{
				int nMoveLevel = ::GetDlgItemInt( hwnd, IDC_EDIT_REMOVELEVEL, &bTranslated, FALSE );//目标区

				if  (nMoveLevel<=0)
				{
					MessageBox(hwnd, "等级必须大于 0 ！",
						"信息提示",MB_OK | MB_ICONEXCLAMATION);
					break;
				}

				if (MoveRoleLevel(nMoveLevel))
				{
					MessageBox(hwnd, "删除成功！",
						"信息提示",MB_OK | MB_ICONEXCLAMATION);
				}
				else
				{
					MessageBox(hwnd, "删除失败,数据库没有启动！",
						"信息提示",MB_OK | MB_ICONEXCLAMATION);
				}
				
			}
			break;
		case IDC_BUTTON_HE:
			{//合区
				int nSourceReg = ::GetDlgItemInt( hwnd, IDC_EDIT_QUA, &bTranslated, FALSE );//源区
				int nTargetReg = ::GetDlgItemInt( hwnd, IDC_EDIT_QUB, &bTranslated, FALSE );//目标区

				int nDelLevel = ::GetDlgItemInt( hwnd, IDC_EDIT_DELLEVE, &bTranslated, FALSE );//目标区
				
				if  (nSourceReg==nTargetReg )
				{
					MessageBox(hwnd, "源区和目标区相同或不能为空！",
						"信息提示",MB_OK | MB_ICONEXCLAMATION);
					break;
				}
				if  (nSourceReg<=0 || nTargetReg<=0)
				{
					MessageBox(hwnd, "源区和目标区不能为小于1！",
						"信息提示",MB_OK | MB_ICONEXCLAMATION);
					break;
				}
				char nNameStr[NAME_LENGHT]={0};
				::GetDlgItemText( hwnd, IDC_EDIT_JIA,nNameStr,sizeof(nNameStr));
				if (!nNameStr[0])
				{
					MessageBox(hwnd, "同名帐号设置为空！",
						"信息提示",MB_OK | MB_ICONEXCLAMATION);
					break;
				}

				if (StartMergeReg(nSourceReg,nTargetReg,nNameStr,nDelLevel))
					MessageBox(hwnd, "合区成功！","信息提示",MB_OK | MB_ICONEXCLAMATION);
				else
					MessageBox(hwnd, "合区失败！","信息提示",MB_OK | MB_ICONEXCLAMATION);

			}
			break;
		case IDC_BUTTON_ADDDATA:
			{
			char nName[64]="增加数据";
			::GetDlgItemText( hwnd, IDC_EDIT_DATAKEY,nName,sizeof(nName));
			//MessageBox(hwnd, nName,
			//	"信息提示",MB_OK | MB_ICONEXCLAMATION);

			char nData[128]="数据内容";
			::GetDlgItemText( hwnd, IDC_EDIT_DATAVAL,nData,sizeof(nData));

			if (nName[0] && nData[0])
			{
			   if (!TestAddData(nName,nData))
			   {
				   MessageBox(hwnd, "数据增加失败！",
					   "信息提示",MB_OK | MB_ICONEXCLAMATION);
			   }
			}
			else
			{
				MessageBox(hwnd, "数据为空！",
					"信息提示",MB_OK | MB_ICONEXCLAMATION);
			}

			}
			break;
		case IDC_BUTTON_GETDATA:
			{
				char nName[64]="获取数据";
				::GetDlgItemText( hwnd, IDC_EDIT_DATAKEY,nName,sizeof(nName));
				int nType=0;
				//IDC_RADIO_TYPE
				if (BST_CHECKED == ::IsDlgButtonChecked(hwnd,IDC_CHECK_TYPE))
				{
                     nType = -1;
				}

				char nData[128]={0};
                if (nName[0])
				{
				    if (!TestGetData(nName,nData,nType))
					{
						MessageBox(hwnd, "数据没搜索到!!",
							"信息提示",MB_OK | MB_ICONEXCLAMATION);
					}

				}
				else
				{
					MessageBox(hwnd, "关键字为空!!",
						"信息提示",MB_OK | MB_ICONEXCLAMATION);
					break;
				}
				::SetDlgItemText( hwnd, IDC_EDIT_DATASHOW,nData);
		    } 
			break;
		case IDC_BUTTON_DELDATA:
			MessageBox(hwnd, "删除数据!",
				"信息提示",MB_OK | MB_ICONEXCLAMATION);
			break;
		case IDCANCEL:
			//::PostMessage( hwnd, WM_CLOSE, 0L, 0L );
			break;

		case IDC_BTN_BACKUP_MANUAL:  //启动备份  按钮操作
			{
				if  (g_dataType==DATA_KIND_SQL)
				{
					copySqldataTobakSql();
					MessageBox(hwnd, "备份数据成功!",
						"信息提示",MB_OK | MB_ICONEXCLAMATION);
				   break;
				}

				if(!IsBackupThreadWorking())
					MessageBox(hwnd, "备份线程还没有启动的!",
					"信息提示",MB_OK | MB_ICONEXCLAMATION);
				if(!DoManualBackup())
					MessageBox(hwnd, "Unable to start manual backup.\nPlease try later.",
					"Information",MB_OK | MB_ICONEXCLAMATION);
		}
			break;
		case IDC_BTN_CLEAR_LOG:  //日志归档  按钮操作
			 if (RemoveLogFiel())
				 MessageBox(hwnd, "归档日志文件成功!","信息提示",MB_OK | MB_ICONEXCLAMATION);
			 else
				 MessageBox(hwnd, "归档日志文件失败!","信息提示",MB_OK | MB_ICONEXCLAMATION);

			break;
		case IDC_BUTTON_ADD_NAME:
			{
				char szBuffer[NAME_LENGHT];
				::GetDlgItemText(hwnd,IDC_EDIT_ROLENAME3,szBuffer, NAME_LENGHT);
				AddOutputStringA(hRoleNamekList_skill,szBuffer);
				::SetDlgItemText(hwnd,IDC_EDIT_ROLENAME3,"");
			}
			break;
		
		case IDC_BUTTON_DEL_NAME:
			{
				int nSelIndex=-1;
				if ( LB_ERR != ( nSelIndex = ::SendMessage(hRoleNamekList_skill, LB_GETCURSEL,0,0)))
					::SendMessage(hRoleNamekList_skill,LB_DELETESTRING,nSelIndex,0);  //删除信息
				else
					::MessageBox(NULL, "请选择一条需要删除的账号!", "警告", MB_OK );
			}
			break;
		case IDC_BUTTON_TEMP_SKILL:
			{
				if(hRoleNamekList_skill)
				{//角色检测
					char buf[64] = {0};
					int nCount = ::SendMessage(hRoleNamekList_skill,LB_GETCOUNT, 0, 0);
								
					for(int i=0; i<nCount;i++)
					{
						::SendMessage(hRoleNamekList_skill, LB_GETTEXT, i, (LPARAM)buf);	//获取字符
						ClearRoleSkillData(buf);
					}		
				}	
				break;
			}
		case IDC_RESTART:            //重新启动 数据库线程
			CClientNode::End();
			ReleaseDBInterface();
			InitDBInterface(g_nMaxRoleCount,g_dataType);
			CClientNode::Start(g_pServer);
			break;
		case IDC_BUTTON_ADD_BAKALLDATA:
			{
				char nBakPath[256];
				ZeroMemory(nBakPath,sizeof(nBakPath));
				::GetDlgItemText(hwnd,IDC_EDIT_BAKPATH,nBakPath,sizeof(nBakPath));
				if (!nBakPath[0])
				{
					MessageBox(NULL,TEXT("备份文件不能为空"), NULL, MB_ICONERROR);
					break;
				}
				//开始查找,并且覆盖添加到数据
				int reg=0;
				reg=bakFindandAddRoleToDb(NULL,nBakPath);
				switch(reg)
				{
				case 3:
					MessageBox(NULL,TEXT("添加数据库失败！"), NULL, MB_ICONERROR);
					break;
				case 10:
					MessageBox(NULL,TEXT("Bak备份恢复成功！"), NULL, MB_ICONERROR);
					break;
				case 0:
					MessageBox(NULL,TEXT("数据库还没有开启"), NULL, MB_ICONERROR);
					break;
				case 1:
					MessageBox(NULL,TEXT("Bak备份文件打开失败"), NULL, MB_ICONERROR);
					break;
				case 2:
					MessageBox(NULL,TEXT("Bak备份无此角色信息"), NULL, MB_ICONERROR);
					break;
				default:
					break;
				}
				break;
			}
        //恢复数据
		case IDC_BUTTON_ADD_BAK:
			{//恢复数据
				char nRoleName[NAME_LENGHT];
				ZeroMemory(nRoleName,sizeof(nRoleName));
				::GetDlgItemText(hwnd,IDC_EDIT_BAKROLENAME,nRoleName,sizeof(nRoleName));
				if (!nRoleName[0])
				{
					MessageBox(NULL,TEXT("角色名不能为空"), NULL, MB_ICONERROR);
					break;
				}
				char nBakPath[256];
				ZeroMemory(nBakPath,sizeof(nBakPath));
				::GetDlgItemText(hwnd,IDC_EDIT_BAKPATH,nBakPath,sizeof(nBakPath));
				if (!nBakPath[0])
				{
					MessageBox(NULL,TEXT("备份文件不能为空"), NULL, MB_ICONERROR);
					break;
				}
				//开始查找,并且覆盖添加到数据
				int reg=0;
				    reg=bakFindandAddRoleToDb(nRoleName,nBakPath);
					switch(reg)
					{
					case 3:
						MessageBox(NULL,TEXT("添加数据库失败！"), NULL, MB_ICONERROR);
						break;
					case 10:
						MessageBox(NULL,TEXT("Bak备份恢复成功！"), NULL, MB_ICONERROR);
						break;
					case 0:
						MessageBox(NULL,TEXT("数据库还没有开启"), NULL, MB_ICONERROR);
						break;
					case 1:
						MessageBox(NULL,TEXT("Bak备份文件打开失败"), NULL, MB_ICONERROR);
						break;
					case 2:
						MessageBox(NULL,TEXT("Bak备份无此角色信息"), NULL, MB_ICONERROR);
						break;
					default:
						break;
					}
			}
			break;
        case IDC_BUTTON_FIND_BAK:
			{//查找
				

			}
			break;
		case IDC_BUTTON_BAKSEL:
			{//选择文件
				OPENFILENAME ofn = { 0 };
				TCHAR strFileName[MAX_PATH] = { 0 };	//用于接收文件名
				ofn.lStructSize = sizeof(OPENFILENAME);	//结构体大小
				ofn.hwndOwner = NULL;					//拥有着窗口句柄
				ofn.lpstrFilter = TEXT("All\0*.*\0jpg\0*.jpg\0bmp\0*.bmp\0\0");	//设置过滤
				ofn.nFilterIndex = 1;	//过滤器索引
				ofn.lpstrFile = strFileName;	//接收返回的文件名，注意第一个字符需要为NULL
				ofn.nMaxFile = sizeof(strFileName);	//缓冲区长度
				ofn.lpstrInitialDir = NULL;			//初始目录为默认
				ofn.lpstrTitle = TEXT("请选择一个文件"); //窗口标题
				ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY; //文件、目录必须存在，隐藏只读选项
				//打开文件对话框
				if (GetOpenFileName(&ofn)){
					std::string filePath = TCHAR2STRING(strFileName);
					//int start = filePath.find_last_of('\\');
					//int end = filePath.find_last_of('.');
					//std::string fileName = filePath.substr(start + 1, end - start - 1);
					//std::string exten = filePath.substr(end, filePath.length() - end);
					//char nTempPaht[256];
					//sprintf(nTempPaht,"%s",filePath.c_str());
					//MessageBox(NULL,TEXT(nTempPaht), NULL, MB_ICONERROR);
					//::GetDlgItemText( hwnd,IDC_EDIT_BAKPATH,nNameStrRole,sizeof(nNameStrRole));
					::SetDlgItemText(hwnd,IDC_EDIT_BAKPATH,filePath.c_str());
				}
				else{
					MessageBox(NULL,TEXT("请选择一文件"), NULL, MB_ICONERROR);
				}
			}
			break;
		case IDC_BTN_ADDROLE:
		{//增加角色过滤列表

		}
		break;
		case IDC_BTN_DAORUROLE:
		{//导入角色过滤列表
				
		}
		break;
		case IDC_BTN_SAVEROLE:
		{//保存角色过滤列表
				
		}
		break;
		case IDC_BTN_DELROLE:
		{//删除角色过滤列表
				
		}
		break;

		case IDC_BTN_BACKUP_SUS_RES: //备份挂起按钮

			if(!IsBackupThreadWorking())
			   MessageBox(hwnd, "备份线程还没有启动的!",
								"信息提示",MB_OK | MB_ICONEXCLAMATION);
			else if(g_IsBackupSuspend)
			{
				if(ResumeBackupTimer())
				{
					g_IsBackupSuspend = false;
					::SetWindowText(GetDlgItem(hwnd,IDC_BTN_BACKUP_SUS_RES),"暂停");
				}
				else
					MessageBox(hwnd, "不能恢复线程,请稍后再试 ！",
								"信息提示",MB_OK | MB_ICONEXCLAMATION);
			}
			else
			{
				if(SuspendBackupTimer())
				{
					g_IsBackupSuspend = true;
					::SetWindowText(GetDlgItem( hwnd, IDC_BTN_BACKUP_SUS_RES ),
								"恢复");
				}
				else
					MessageBox(hwnd, "不能暂停线程,请稍后再试！",
								"信息提示",MB_OK | MB_ICONEXCLAMATION);
			}
			break;
		}

		break;
	
	case WM_CREATE_ENGINE: //创建服务器
		{//启动DB线程引擎

		g_nServerPort     = ::GetDlgItemInt( hwnd, IDC_EDIT_PORT, &bTranslated, FALSE );
		g_nMaxRoleCount   = ::GetDlgItemInt( hwnd, IDC_EDIT_MAXNUM_ROLE, &bTranslated, FALSE );
		g_BackupSleepTime = ::GetDlgItemInt( hwnd, IDC_EDIT_BACKUP_SLEEP_TIME, &bTranslated, FALSE );
        g_nMaxPlayerCount = ::GetDlgItemInt( hwnd, IDC_EDIT_MAXNUM_PLAYER, &bTranslated, FALSE );
        
		CreateDatabaseEngine(hwnd);  //创建服务
/*    	DWORD dwThreadId, dwThrdParam = 1;
		HANDLE hMonitorThread = CreateThread(
			NULL,				// no security attributes 
			0,					// use default stack size  
			MonitorProc,		// thread function 
			&dwThrdParam,		// argument to thread function 
			0,					// use default creation flags 
			&dwThreadId);		// returns the thread identifier 
		 */
		if (g_dataType==DATA_KIND_DB)
			::SetWindowText(hwnd, "Goddess - [Enable DB]" );
		else
		    ::SetWindowText(hwnd, "Goddess - [Enable MYSQL]" );
		
		}
		break;

	case WM_ADD_CLIENT:
		{//增加信息
		char pText[128] = {0};
				if (g_pServer) g_pServer->GetClientInfo(wParam,pText,sizeof(pText));
			if (pText && pText[0])
			{  	
			    HWND hCtrl = ::GetDlgItem(hwnd,IDC_LIST_CLIENT);

				if (hCtrl && ::IsWindow(hCtrl))
				{
					int nIndex = ::SendMessage(hCtrl,LB_ADDSTRING, 0,(LPARAM)pText);
					
					if (LB_ERR != nIndex)
					{
						::SendMessage(hCtrl,LB_SETITEMDATA,nIndex,wParam);
					}
				}
			}
		}		
		break;

	case WM_DEL_CLIENT:
		{//减去一条信息
			HWND hCtrl = ::GetDlgItem(hwnd,IDC_LIST_CLIENT);
			
			if ( hCtrl && ::IsWindow(hCtrl))
			{
				int nCount = ::SendMessage(hCtrl, LB_GETCOUNT,0,0);
				
				for ( int i=0; i<nCount && LB_ERR != nCount; ++i )
				{
					UINT nSearchID = 0;
					
					if (wParam == ( nSearchID = ::SendMessage(hCtrl,LB_GETITEMDATA,i,0)))
					{
						::SendMessage( hCtrl, LB_DELETESTRING, i, 0 );
						
						return TRUE;
					}
				}
			}
		}
		break;

	default:
		break;		
	}
	
	/*
	 * Clean up any unused messages by calling DefWindowProc
	 */
	return FALSE;
}
//创建数据库引擎
bool CreateDatabaseEngine(HWND hwnd)
{
	int nReg=1;
	    nReg=InitDBInterface(g_nMaxRoleCount,g_dataType);

	if (nReg!=1) //初始化角色数据库环境--打开数据库
	{
		 char nMsg[64]={0};
		 sprintf_s(nMsg,sizeof(nMsg)-1,"打开数据库失败,错误码:%d!",nReg);
		::MessageBox(NULL,nMsg, "Warning", MB_OK | MB_ICONSTOP );
		return false;
	} 

	if (StartBackupTimer(g_BackupSleepTime)) //开始运行备份线程
	   ::SetWindowText(GetDlgItem(hwnd,IDC_LAB_BACKUP_STATUS),"备份: 运行");
	else
	   ::SetWindowText(GetDlgItem(hwnd,IDC_LAB_BACKUP_STATUS),"备份: 停止");
	
	/*
	 * Open this server to client
	 */
	pfnCreateServerInterface pFactroyFun = (pfnCreateServerInterface)(g_theHeavenLibrary.GetProcAddress("CreateInterface"));

	IServerFactory *pServerFactory = NULL;
	//创建一个服务器端 等待GS 里面的 客户端连接 
	if (pFactroyFun && SUCCEEDED(pFactroyFun(IID_IServerFactory, reinterpret_cast<void **>(&pServerFactory))))
	{//设置网络数据环境
		pServerFactory->SetEnvironment(g_nMaxPlayerCount,10,20,g_bufferSize_Cache,0);	  //4*1024*1024	   500 g_nMaxPlayerCount
		//pServerFactory->SetEnvironment(g_nMaxPlayerCount,10,g_nMaxPlayerCount,64*1024);
		pServerFactory->CreateServerInterface(IID_IIOCPServer, reinterpret_cast<void **>(&g_pServer));
		
		pServerFactory->Release();
	}

	if (!g_pServer)
	{
		::MessageBox(NULL, "Initialization failed! Don't find a correct heaven.dll", "Warning", MB_OK | MB_ICONSTOP );
		return false;
	}

	g_pServer->Startup();	                                       

	g_pServer->RegisterMsgFilter((void *)hwnd,ServerEventNotify);  //通知GS服务器 网络链接

	if (FAILED(g_pServer->OpenService(INADDR_ANY,g_nServerPort)))  //5001端口
	{
		return false;
	}

	DWORD dwThreadID = 0L;

	IServer *pCloneServer = NULL;

	g_pServer->QueryInterface(IID_IIOCPServer, (void **)&pCloneServer);														 	//创建工作线程

	g_hThread = ::CreateThread(NULL,0,ThreadProcess,(void *)pCloneServer,0,&dwThreadID);

	if (!g_hThread)
	{
		return false;
	}

	CClientNode::Start(g_pServer);

	g_hStartWorkEvent.Set();
	
	return true;
}

void DestroyDatabaseEngine()
{
	CClientNode::End();

	g_hQuitEvent.Set();
	
	DWORD result = ::WaitForSingleObject( g_hThread, 50000 );

	if ( result == WAIT_TIMEOUT )
	{
		::TerminateThread( g_hThread, ( DWORD )( -1 ) );
	}

	SAFE_CLOSEHANDLE( g_hThread );

	if ( g_pServer )
	{
		g_pServer->CloseService();
		g_pServer = NULL;
	}

	SAFE_RELEASE( g_pServer );

	ReleaseDBInterface();
}
//加载设置
void LoadSetting()
{
	CIniFile theConfigFile;

	_tstring sAppPath = GetAppFullPath( NULL );

	_tstring sConfigFile;

	sConfigFile = sAppPath + "Goddess.cfg";

	theConfigFile.SetFile( sConfigFile.c_str() );

	g_nServerPort = theConfigFile.ReadInteger("Setting", "Port", 5001 );

	g_nMaxRoleCount = theConfigFile.ReadInteger("Setting", "MaxRoleCount", 3 );

	g_BackupSleepTime = theConfigFile.ReadInteger("Setting", "BackupSleepTime", 3 );

	g_nMaxPlayerCount = theConfigFile.ReadInteger("Setting", "MaxPlayerCount",500);

	g_bufferSize_Cache = theConfigFile.ReadInteger("Setting", "bufferSizeCache",128*1024);

	g_dataType= theConfigFile.ReadInteger("Setting", "dataOpenModel", 0 );

}
//保存设置
void SaveSetting()
{
	CIniFile theConfigFile;

	_tstring sAppPath = GetAppFullPath( NULL );

	_tstring sConfigFile;

	sConfigFile = sAppPath + "Goddess.cfg";

	theConfigFile.SetFile( sConfigFile.c_str());

	theConfigFile.WriteInteger("Setting", "Port", g_nServerPort);

	theConfigFile.WriteInteger("Setting", "MaxRoleCount", g_nMaxRoleCount);

	theConfigFile.WriteInteger("Setting", "BackupSleepTime", g_BackupSleepTime);

	theConfigFile.WriteInteger("Setting", "MaxPlayerCount", g_nMaxPlayerCount);
	theConfigFile.WriteInteger("Setting", "bufferSizeCache", g_bufferSize_Cache);

	if  (::IsDlgButtonChecked(mainHwnd,IDC_RADIO_DATA_SQL)==1)  //删除全区
		g_dataType=DATA_KIND_SQL;
	else
		g_dataType=DATA_KIND_DB;

	theConfigFile.WriteInteger("Setting", "dataOpenModel", g_dataType);

}

void KPIGetExePath( LPSTR lpExePath, DWORD dwSize )
{
	if ( lpExePath )
	{
		memset( lpExePath, 0, dwSize );
		GetModuleFileName( NULL, lpExePath, dwSize );
		DWORD dwEndAt;
		dwEndAt = strlen( lpExePath ) - 1;
		while ( dwEndAt >= 1 )
		{
			if ( lpExePath[dwEndAt-1] == '\\' )
			{
				lpExePath[dwEndAt] = 0;
				break;
			}
			dwEndAt--;
		}
	}
}

int gTimeStamp(char* sDest, char* pTail)
{
	ASSERT(sDest);
	time_t curtm = ::time(NULL);
  
	struct tm* ptm = localtime(&curtm);
	return sprintf(sDest, "[%04d/%02d/%02d %02d:%02d:%02d]%s",
		ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec, pTail);
}

void gTraceLogFile(LPCTSTR pBuffer, long nLen)
{
	if (!gs_LogFile.BeginBatch())
		return;
	
	char szTime[256]={0};
	int nTime = gTimeStamp(szTime, " ");
	gs_LogFile.TraceLog(szTime, nTime);
	gs_LogFile.TraceLog(pBuffer, nLen);
	gs_LogFile.TraceLog("\r\n", 2 * sizeof(TCHAR));
	
	gs_LogFile.EndBatch();
}

void AddOutputStringA(HWND hListCtrl, char* aStr)
{//添加操作输出文字
	if (hListCtrl && ::IsWindow( hListCtrl ))
	{
		int nCount = ::SendMessage( hListCtrl, LB_GETCOUNT, 0, 0 ); //获取信息的数量
		if(nCount >= 100)
		{
			::SendMessage( hListCtrl, LB_DELETESTRING, 100, 0 );  //删除信息
		}
		int nIndex = ::SendMessage( hListCtrl, LB_INSERTSTRING, 0, ( LPARAM )aStr);//把新的信息添到第一个
	}
}

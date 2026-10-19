
#include "StdAfx.h"

#include "KSOServer.h"

#include "nb30.h"

#include <iostream>
#include <string>
#include <direct.h> 
#include <time.h>
#include <Shlobj.h>

#include <comutil.h>  
#include <Wbemidl.h>  
#include <tchar.h>  
#include <strsafe.h>  
#include <algorithm>  
#include <atlconv.h>  
#include <ntddndis.h>  
#include <winioctl.h>
#include <ws2tcpip.h>
#pragma comment (lib,"Ws2_32.lib")
#pragma comment (lib, "comsuppw.lib")  
#pragma comment (lib, "wbemuuid.lib")

#include "../../ENGINE/Src/KSG_MD5_String.h"
#include <DbgHelp.h>
//////////////////////////////////////
#define THIS_SERVER_TIME_FREE "597CF4FE16383B455365C9C749F6C311"	 //2020123009  2019123009
#define SDFETTGR "75C3B058276D0D04A3F4B9154C6BC9D9"	         //1900
#define CURYEARS "A5B63828F9931B2E68EDCCD2D8FB3D5F"	         //2021
#define FANGPOJIE "734F1E78468FE84B45284A37078D5A95"


#define JFRUNHLAJFUGJ "0钸徭6?犑很魾"

#define NETCORRE "鰿炷听)#"

#define NTP_PORT 123 /*NTP专用端口号字符串*/
#define NTP_SERVER_IP "ntp1.aliyun.com"
#define NTP_PORT_STR "123"
#define JAN_1970 0x83aa7e80 /*1900年～1970年之间的时间秒数*/

#define NET_TIME_PORT 37

#define TIME_FARM 200
#define TIME_FARM_H 100

using namespace std;

extern KSwordOnLineSever g_pSOServer;

int g_nPort = 0;

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
			CreateDirectory("DumpInfo", NULL);

			snprintf(szFileName, sizeof(szFileName),"DumpInfo\\%s-%04d%02d%02d-%02d%02d%02d-%ld-%ld.dmp",
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

	return GenerateMiniDump(NULL, lpExceptionInfo,L"gameserver");
}


//---------------------程序入口
int main(int argc, char* argv[])
{
    //SetConsoleCP(65001);
    //SetConsoleOutputCP(65001);

	SetUnhandledExceptionFilter(ExceptionFilter);

	system("color 0F");
	printf("==Main thread server is starting.......==\n");
	
	g_pSOServer.SetGo(true);

	if (!g_pSOServer.GetGo())
	{   
		char nNetInfo[64]={0};
		sprintf(nNetInfo,"%s",NETCORRE); //网络故障
		g_pSOServer.EDPad_Decipher(nNetInfo,strlen(nNetInfo));
		printf("-----%s------ \n",nNetInfo);
		Sleep(5000);
		ExitProcess(0);  //退出进程
	}

	//--------------------------------------------------------
	TCHAR moduleFileName[MAX_PATH + 1] ;
	DWORD moduleFileNameLen = MAX_PATH ;
	char  szBuffer[MAX_PATH + 1]={0};
	unsigned uRetCode = 0;
	uRetCode = ::GetModuleFileName(NULL,moduleFileName, moduleFileNameLen);
	sprintf(szBuffer,"%s",moduleFileName);
	//获取应用程序名称
	char szExe[MAX_PATH] = "";//（TEST.exe）
	char *pbuf = NULL;
	char* szLine = strtok_s(moduleFileName,"\\",&pbuf);
	while (NULL != szLine)
	{
		strcpy_s(szExe, szLine);
		szLine = strtok_s(NULL,"\\",&pbuf);
	}
	//删除.exe
	strncpy_s(moduleFileName, szExe, strlen(szExe)-4);
	// printf("----%s---\n",moduleFileName);

	if (argc == 2)
	{//启动端口
		g_nPort = atoi(argv[1]);
	}

    BOOL bRunning = TRUE;

    if (g_pSOServer.GetGo() && g_pSOServer.InitServer(moduleFileName)<=0)
	{
		bRunning = FALSE;
		Sleep(2000);
		ExitProcess(0); 
		return 0;
	} 

	while(g_pSOServer.GetGo() && bRunning)
	{
		if (!g_pSOServer.Breathe())
		{
			bRunning=FALSE;
			break;
		}
	}

	while(!bRunning)
	{
		g_pSOServer.ErrOut();
		g_pSOServer.Release();
	}

	 getchar();
	 return 0;
}
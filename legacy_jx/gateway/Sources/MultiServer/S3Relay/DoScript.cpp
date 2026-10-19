// -------------------------------------------------------------------------
//	文件名		：	DoScript.cpp
//	创建者		：	万里
//	创建时间	：	2003-9-16 21:07:01
//	功能描述	：	
//
// -------------------------------------------------------------------------

#include "stdafx.h"
#include "DoScript.h"

#include "Global.h"
#include "S3Relay.h"

DWORD g_ExecuteIP = 0;
DWORD g_ExecuteRelayID = 0;
//向所有IP发包
void BroadGlobal(const void* pData, size_t size, DWORD nFromIP, DWORD nFromRelayID)
{
	if (pData == NULL || size == 0)
		return;

	RELAY_DATA data;
	data.ProtocolFamily = pf_relay;
	data.ProtocolID = relay_c2c_data;
	data.nToIP = 0;
	data.nToRelayID = 0;
	data.nFromIP = nFromIP;
	data.nFromRelayID = nFromRelayID;
	data.routeDateLength = 1 + size;

	size_t pckgsize = sizeof(RELAY_DATA) + data.routeDateLength;
	void* pRelayData = _alloca(pckgsize);
	
	char* pOData = (char*)pRelayData;

	memcpy(pOData, &data, sizeof(RELAY_DATA));
	pOData += sizeof(RELAY_DATA);

	*pOData = s2s_execute;
	pOData++;

	memcpy(pOData, pData, size);
	pOData += size;

	g_HostServer.BroadPackage(pRelayData, pckgsize);
}
//向某个IP发包
void BroadGame(DWORD nToIP, const void* pData, size_t size, DWORD nFromIP, DWORD nFromRelayID)
{
	if (pData == NULL || size == 0 || nToIP == 0)
		return;

	CNetSockDupEx sockdup = dealrelay::FindRelaySockByIP(nToIP);
	if (!sockdup.IsValid())
		return;

	RELAY_DATA data;
	data.ProtocolFamily = pf_relay;
	data.ProtocolID = relay_c2c_data;
	data.nToIP = 0;
	data.nToRelayID = 0;
	data.nFromIP = nFromIP;
	data.nFromRelayID = nFromRelayID;
	data.routeDateLength = 1 + size;

	size_t pckgsize = sizeof(RELAY_DATA) + data.routeDateLength;
	void* pRelayData = _alloca(pckgsize);
	
	char* pOData = (char*)pRelayData;

	memcpy(pOData, &data, sizeof(RELAY_DATA));
	pOData += sizeof(RELAY_DATA);

	*pOData = s2s_execute;
	pOData++;

	memcpy(pOData, pData, size);
	pOData += size;

	sockdup.SendPackage(pRelayData, pckgsize);
}

void RootSend(DWORD nToIP, DWORD nToRelayID, const void* pData, size_t size)
{
	if (pData == NULL || size == 0)
		return;

	RELAY_DATA data;
	data.ProtocolFamily = pf_relay;
	data.ProtocolID = relay_c2c_data;
	data.nToIP = nToIP;
	data.nToRelayID = nToRelayID;
	data.nFromIP = 0;
	data.nFromRelayID = 0;
	data.routeDateLength = size;

	size_t pckgsize = sizeof(RELAY_DATA) + data.routeDateLength;
	void* pRelayData = _alloca(pckgsize);
	
	char* pOData = (char*)pRelayData;

	memcpy(pOData, &data, sizeof(RELAY_DATA));
	pOData += sizeof(RELAY_DATA);

	memcpy(pOData, pData, size);
	pOData += size;

	g_RootClient.SendPackage(pRelayData, pckgsize);
}


//KLuaScript gs_Script;
//全局执行脚本
int LuaGlobalExecute(Lua_State * L)
{
	if (Lua_GetTopIndex(L) != 1)
		return 0;
	
	const char * szMsg = Lua_ValueToString(L, 1);
	if (szMsg && szMsg[0])
	{
		int nszLen = strlen(szMsg);

		GM_EXECUTE_COMMAND exe;
		exe.ProtocolFamily = pf_gamemaster;
		exe.ProtocolType = gm_c2s_execute;
		strncpy(exe.AccountName ,"GM",ACC_LENGTH);
		exe.AccountName[(ACC_LENGTH) - 1] = '\0';
		exe.wLength = nszLen;
		exe.wExecuteID = 1;

		size_t pckgsize = sizeof(GM_EXECUTE_COMMAND) + exe.wLength;
		void* pRelayData = _alloca(pckgsize);

		char* pOData = (char*)pRelayData;

		memcpy(pOData, &exe, sizeof(GM_EXECUTE_COMMAND));
		pOData += sizeof(GM_EXECUTE_COMMAND);

		memcpy(pOData, szMsg, nszLen);
		pOData += nszLen;

		BroadGlobal(pRelayData, pckgsize, g_ExecuteIP, g_ExecuteRelayID);
	}

	return 0;
}
//向某个IP 执行脚本
int LuaGameExecute(Lua_State * L)
{
	if (Lua_GetTopIndex(L) != 2)
		return 0;
	
	const char * szIP = Lua_ValueToString(L, 1);
	const char * szMsg = Lua_ValueToString(L, 2);

	DWORD nIP = _a2ip(szIP);
	if (nIP == 0)
		return 0;
	if (szMsg)
	{
		int nszLen = strlen(szMsg);

		GM_EXECUTE_COMMAND exe;
		exe.ProtocolFamily = pf_gamemaster;
		exe.ProtocolType = gm_c2s_execute;
		strncpy(exe.AccountName , "GM", ACC_LENGTH);
		exe.AccountName[(ACC_LENGTH) - 1] = '\0';
		exe.wLength = nszLen;
		exe.wExecuteID = 1;

		size_t pckgsize = sizeof(GM_EXECUTE_COMMAND) + exe.wLength;
		void* pRelayData = _alloca(pckgsize);

		char* pOData = (char*)pRelayData;

		memcpy(pOData, &exe, sizeof(GM_EXECUTE_COMMAND));
		pOData += sizeof(GM_EXECUTE_COMMAND);

		memcpy(pOData, szMsg, nszLen);
		pOData += nszLen;

		BroadGame(nIP, pRelayData, pckgsize, g_ExecuteIP, g_ExecuteRelayID);
	}

	return 0;
}

int LuaMsg2IP(Lua_State * L)
{
	int nParamCount = Lua_GetTopIndex(L);
	if ( nParamCount < 3 ) 
		return 0;
	int nIP = 0;
	const char * szIP = Lua_ValueToString(L, 1) ;
	nIP = _a2ip(szIP);
	if (nIP == 0)
		return 0;

	int nID = (int) Lua_ValueToNumber(L, 2);
	const char * szMsg = Lua_ValueToString(L, 3) ;
	if (!szMsg) 
		return 0;

	int nParamID = 0;
	if (nParamCount < 4)
	{
		nParamID = 0;
	}
	else
	{
		nParamID = (int) Lua_ValueToNumber(L, 4);
	}

	if (szMsg && szMsg[0])
	{
		int nszLen = strlen(szMsg);

		char szID[NAME_LENGHT];

		itoa(nParamID, szID, 10);

		CHAT_MSG_EX ChatMsgEx;
		ChatMsgEx.ProtocolFamily = pf_playercommunity;
		ChatMsgEx.ProtocolID = playercomm_channelchat;
		strcpy(ChatMsgEx.m_szSourceName, szID);
		strcpy(ChatMsgEx.m_szAccountName, "GM");
		ChatMsgEx.SentenceLength = nszLen;

		size_t pckgsize = sizeof(ChatMsgEx) + ChatMsgEx.SentenceLength;
		void* pRelayData = _alloca(pckgsize);

		char* pOData = (char*)pRelayData;

		memcpy(pOData, &ChatMsgEx, sizeof(CHAT_MSG_EX));
		pOData += sizeof(CHAT_MSG_EX);

		memcpy(pOData, szMsg, nszLen);
		pOData += nszLen;

		RootSend(nIP, nID, pRelayData, pckgsize);
	}

	return 0;
}

int LuaGetIP(Lua_State * L)
{
	int nParamCount = Lua_GetTopIndex(L);
	if ( nParamCount < 1 ) 
		return 0;
	int nParamID = (int) Lua_ValueToNumber(L, 1);

	char szDesMsg[200];
	szDesMsg[0] = 0;
	if (nParamID == 0)
	{
		strcpy(szDesMsg, _ip2a(gGetHostIP(local_adapt)));
	}
	else if (nParamID == 1)
	{
		strcpy(szDesMsg, _ip2a(gGetHostIP(global_adapt)));
	}
	else
	{
		strcpy(szDesMsg, "0.0.0.0");
	}

	Lua_PushString(L, szDesMsg);
	return 1;
}

int LuaPrintf(Lua_State * L)
{
	int nParamCount = Lua_GetTopIndex(L);
	if ( nParamCount < 1 ) 
		return 0;
	char *ver =(char*)Lua_ValueToString(L, 1);
	rTRACE(ver);
	return 0;
}
//S3执行脚本函数
TLua_Funcs GameScriptFuns[] = 
{
	{"GlobalExecute", LuaGlobalExecute},	//GlobalExecute通知整个服务组执行一个脚本
	{"GameExecute",  LuaGameExecute},		//GameExecute通知一个游戏世界执行一个脚本
	{"Msg2IP", LuaMsg2IP},		            //Msg2IP返回结果值到发出通知的地方
	{"GetIP", LuaGetIP},		            //GetIP(index)
	//printf("Safe save paishopdata to server\n");
	{"Printf", LuaPrintf},		            //GetIP(index)
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//注册函数
int g_GetGameScriptFunNum()
{
	return sizeof(GameScriptFuns)  / sizeof(TLua_Funcs);
}
////初始话脚本 脚本组件 和 注册函数 创建L指针
//BOOL InitScript()
//{
//	if (gs_Script.Init() && gs_Script.RegisterFunctions(GameScriptFuns, g_GetGameScriptFunNum()))
//	{
//		return TRUE;
//	}
//
//	return FALSE;
//}
////编译这个脚本
//BOOL ExcuteScript(DWORD nIP, DWORD nRelayID, const char * ScriptCommand)
//{
//	if (ScriptCommand && ScriptCommand[0] != 0)
//	{
//		if (gs_Script.LoadBuffer((PBYTE)ScriptCommand, strlen(ScriptCommand)))
//		{
//			g_ExecuteIP = nIP;
//			g_ExecuteRelayID = nRelayID;
//			return gs_Script.ExecuteCode();
//		}
//	} 
//
//	return FALSE;
//}
////卸载脚本
//BOOL UninitScript()
//{
//	//gs_Script.Exit();
//	return TRUE;
//}


//#include "KCore.h"
//#include "KSortScript.h"
//#include "LuaFuns.h"
//#include "KFilePath.h"
//#include "KDebug.h"
#ifndef WIN32
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#else 
#include <io.h>
#include <direct.h>
#endif
//#include "Shlwapi.h"

KLuaScript g_ScriptSet[MAX_SCRIPT_IN_SET_G];
KScriptBinTree g_ScriptBinTree;
unsigned int   nCurrentScriptNum;
char g_szCurScriptDir[MAX_PATH];

void	LoadScriptInDirectory(LPSTR lpszRootDir, LPSTR lpszSubDir);

DWORD g_FileName2Id_(LPSTR lpFileName)
{
	////////////////////////////////////////////////////////////////
	if (lpFileName)
	{
		DWORD dwID = 0; //

		for (int i = 0;lpFileName[i]; ++i )
		{
			dwID =(dwID + (i + 1) * lpFileName[i])%0x8000000B*0xFFFFFFEF;
		}

		return (dwID ^ 0x12345678);	
	}

	return (DWORD)(-1);
}

int	operator<(KSortScriptNode ScriptLeft, KSortScriptNode ScriptRight)
{
	return ScriptLeft.GetScriptID() < ScriptRight.GetScriptID();
};

int operator==(KSortScriptNode ScriptLeft, KSortScriptNode ScriptRight)
{
	return ScriptLeft.GetScriptID() == ScriptRight.GetScriptID();
};

//将szFilePath目录下的所有脚本文件加载进ScriptBinTree二叉树中
static unsigned long LoadAllScript(char * szFilePath)
{
	g_SetRootPath(NULL);
	g_SetFilePath("\\");
	char szRootPath[MAX_PATH];
	char szOldRootPath[MAX_PATH];
	//GetCurrentDirectory(MAX_PATH, szOldRootPath);
	if (!getcwd(szOldRootPath, MAXPATH))
		g_GetRootPath(szOldRootPath);	// the same directory g_SetRootPath(NULL) took above
	g_GetFullPath(szRootPath,szFilePath);
	//rTRACE("--g3脚本路径:%s---",szRootPath);
	LoadScriptInDirectory(szRootPath, "");
	chdir(szOldRootPath);
	//SetCurrentDirectory(szOldRootPath);
	return nCurrentScriptNum;
}

unsigned long g_IniScriptEngine()
{
	rTRACE("---------开始加载S3脚本---------");
	g_szCurScriptDir[0] = 0;
	nCurrentScriptNum = 0;
	g_ScriptBinTree.ClearList();
	return LoadAllScript("\\script");
}



const KScript * g_GetScript(DWORD dwScriptId)
{
	KSortScriptNode ScriptNode;
	ScriptNode.SetScriptID(dwScriptId);
	if (g_ScriptBinTree.Find(ScriptNode))
	{
		return ScriptNode.GetScript();
	}
	return NULL;
}
const KScript * g_GetScript(const char * szRelativeScriptFile)
{
	DWORD dwScriptId = g_FileName2Id_((LPSTR)szRelativeScriptFile);
	return g_GetScript(dwScriptId);
}

extern int LuaIncludeFile(Lua_State * L);

//加载脚本，该文件名参数为相对目录
static BOOL LoadScriptToSortListA(char * szRelativeFile)
{
	if (!szRelativeFile || !szRelativeFile[0]) return FALSE;
	KSortScriptNode ScriptNode ;
	ScriptNode.SetScriptID(g_FileName2Id_(szRelativeFile));
	int t  =strlen(szRelativeFile);
	if (t >= 256)
		t ++;

	if (nCurrentScriptNum < MAX_SCRIPT_IN_SET_G)
	{
		g_ScriptSet[nCurrentScriptNum].Init();
		g_ScriptSet[nCurrentScriptNum].RegisterFunctions(GameScriptFuns, g_GetGameScriptFunNum());
		g_StrCpyLen(g_ScriptSet[nCurrentScriptNum].m_szScriptName, szRelativeFile, 256);
		if (g_ScriptSet[nCurrentScriptNum].Load(szRelativeFile))
		{
			rTRACE("[初始化]-加载脚本成功:%s -!!!",szRelativeFile);
		}
		else
		{
			//g_DebugLog("[KSortScript]!Load %s", szRelativeFile);
			rTRACE("[初始化]-加载脚本失败:%s -!!!",szRelativeFile);
			return FALSE;
		}
	}
	else
	{
		//g_DebugLog("[脚本]严重错误!脚本数量超限制%d！请立即解决！！", MAX_SCRIPT_IN_SET_G);
		rTRACE("[初始化]-脚本数量超限制:%d -!!!",MAX_SCRIPT_IN_SET_G);
		return FALSE;
	}

	ScriptNode.SetScriptIndex(nCurrentScriptNum++);
	g_ScriptBinTree.Insert(ScriptNode);
	return TRUE;
}

//加载脚本，该文件名参数为实际目录
static BOOL LoadScriptToSortList(char * szFileName)
{
	if (!szFileName || !szFileName[0]) return FALSE;
	if (nCurrentScriptNum >= MAX_SCRIPT_IN_SET_G)
	{
		return FALSE;
	}

	int nFileNameLen = strlen(szFileName);

	char szRootPath[MAX_PATH];	
	g_GetRootPath(szRootPath);
	//	char szRelativePath[MAX_PATH];
	char *szRelativePath;
	char szCurrentDirectory[MAX_PATH];


	//	GetCurrentDirectory(MAX_PATH, szCurrentDirectory);
	if (!getcwd(szCurrentDirectory, MAX_PATH))
		return FALSE;
	if (strncmp(szCurrentDirectory, szRootPath, strlen(szRootPath)) != 0)
		szRelativePath = szCurrentDirectory;	// not under the root: the whole name is the name
	else
		szRelativePath = szCurrentDirectory + strlen(szRootPath);
	//	PathRelativePathTo(szRelativePath,szRootPath, FILE_ATTRIBUTE_DIRECTORY,szCurrentDirectory , FILE_ATTRIBUTE_NORMAL );
	char szRelativeFile[MAX_PATH];
	if (szRelativePath[0] == '.' && szRelativePath[1] == '\\')
		sprintf(szRelativeFile, "%s\\%s", szRelativePath + 1, szFileName);
	else
		sprintf(szRelativeFile, "%s\\%s", szRelativePath, szFileName);
	g_StrLower(szRelativeFile);
	//	g_DebugLog("[Script]Loading Script %s %d", szRelativeFile, g_FileName2Id(szRelativeFile));
	//rTRACE("--g3脚本路径LoadScriptToSortList:%s---",szRelativeFile);
	return LoadScriptToSortListA(szRelativeFile);
	//	return FALSE;
}


void	LoadScriptInDirectory(LPSTR lpszRootDir, LPSTR lpszSubDir)
{
	int				nFlag;
	char			szRealDir[MAX_PATH];
#ifdef WIN32
	sprintf(szRealDir, "%s\\%s", lpszRootDir, lpszSubDir);
#else
	sprintf(szRealDir, "%s/%s", lpszRootDir, lpszSubDir);
	char *ptr = szRealDir;
	while(*ptr) { if(*ptr == '\\') *ptr = '/';  ptr++;  }
#endif

#ifdef WIN32
	if(chdir(szRealDir)) return;
	_finddata_t FindData;
	// handle type is auto because UCRT returns it as intptr_t: storing it in a 32-bit long
	// destroys the pointer and the next _findnext writes through the truncated value
	auto dir = _findfirst("*.*", &FindData);
	while(dir != -1) {
		if(strcmp(FindData.name, ".") == 0 || strcmp(FindData.name, "..") == 0)	{
			if(_findnext(dir, &FindData)) break;
			continue;
		}
		if(FindData.attrib == _A_SUBDIR)
		{
			LoadScriptInDirectory(szRealDir, FindData.name);
		}
		else
		{	
			nFlag = 0;
			for (int i = 0; i < (int)strlen(FindData.name);  i++)
			{
				if (FindData.name[i] == '.')
					break;
				if (FindData.name[i] == '\\')
				{
					nFlag = 1;
					break;
				}
			}
			if (nFlag == 1)
			{
				LoadScriptInDirectory(szRealDir, FindData.name);
			}
			else
			{
				char szExt[128];
				if (strlen(FindData.name) >= 4) 
				{
					snprintf(szExt, sizeof(szExt), "%s", FindData.name + strlen(FindData.name) - 4);
					_strupr(szExt);
					if ( (!strcmp(szExt, ".LUA")/*|| (!strcmp(szExt, ".TXT")*/))
						if (!LoadScriptToSortList(FindData.name))
						{
							//g_DebugLog("Not LoadScriptToSortList: %s", FindData.name);
						}
				}
			}
		}
		if(_findnext(dir, &FindData)) break;
	} 
	_findclose(dir);
	chdir(lpszRootDir);
#else
	DIR *dp;
	int i;
	struct dirent *ep;
	if(chdir(szRealDir)) return;
	dp = opendir(".");
	if(dp) {
		while(ep = readdir(dp)) {
			if(strcmp(ep->d_name, ".") == 0 || strcmp(ep->d_name, "..") == 0) continue;

			if(ep->d_type ==4) {
				LoadScriptInDirectory(szRealDir, ep->d_name);
			}
			else {
				nFlag = 0;
				for (i = 0; i < (int)strlen(ep->d_name);  i++)
				{
					if (ep->d_name[i] == '.')
						break;
					if (ep->d_name[i] == '\\')
					{
						nFlag = 1;
						break;
					}
				}
				if (nFlag == 1)
				{
					LoadScriptInDirectory(szRealDir,ep->d_name);
				}
				else
				{
					char szExt[128];
					if (strlen(ep->d_name) >= 4)
					{
						strcpy(szExt, ep->d_name + strlen(ep->d_name) - 4);
						g_StrUpper(szExt);
						///					_strupr(szExt);
						if ( (!strcmp(szExt, ".LUA")/*) || (!strcmp(szExt, ".TXT")*/))
							if (!LoadScriptToSortList(ep->d_name))
							{
								//							g_DebugLog("Not LoadScriptToSortList: %s", ep->d_name);
							}
					}
				}
			}
		}
		closedir(dp);
	}
	chdir(lpszRootDir);
#endif
}

void UnLoadScript(DWORD dwScriptID)
{
	KSortScriptNode ScriptNode;
	ScriptNode.SetScriptID(dwScriptID);
	g_ScriptBinTree.Delete(ScriptNode);
}

void UnLoadAllScript()
{
	g_ScriptBinTree.ClearList();
}

BOOL ReLoadScript(const char * szRelativePathScript)
{
	if (!szRelativePathScript || !szRelativePathScript[0])
		return FALSE;
	char script[MAX_PATH];
	strcpy(script, szRelativePathScript);
	//	_strlwr(script);
	g_StrLower(script);
	UnLoadScript(g_FileName2Id_(script));
	return LoadScriptToSortListA(script);
}

unsigned long  ReLoadAllScript()
{
	g_ScriptBinTree.ClearList();
	return g_IniScriptEngine();
}

BOOL ExcuteScript(DWORD nIP, DWORD nRelayID, const char * ScriptCommand)
{
	if (ScriptCommand && ScriptCommand[0] != 0)
	{
		KLuaScript * gs_Script = (KLuaScript*)g_GetScript(ScriptCommand);
		if (gs_Script && gs_Script->LoadBuffer((PBYTE)ScriptCommand, strlen(ScriptCommand)))
		{
			g_ExecuteIP = nIP;
			g_ExecuteRelayID = nRelayID;
			return gs_Script->ExecuteCode();
		}
	}
	return false;
} 


BOOL SubExecuteScriptA(char * ScriptFileName, char * szFunName, int nParam,char * szCanshu)
{//g_FileName2Id(ScriptFileName)
	if (!ScriptFileName || !ScriptFileName[0] || !szFunName  || !szFunName[0]) return FALSE;
	char nNewScriptName[128]={0};
	sprintf_s(nNewScriptName,sizeof(nNewScriptName)-1,"%s",ScriptFileName);
	strlwr(nNewScriptName);//g_StrLower
	return SubExecuteScriptB(nNewScriptName, szFunName, nParam,szCanshu);	
}

BOOL SubExecuteScriptB(char* dwScriptIdA,  char * szFunName, int nParam,char * szCanshu)
{
	// KSubWorld nIndex;	
	if (!szFunName) return false;

	bool bExecuteScriptMistake = true;
	//创建一个新栈
	KLuaScript * pScript = (KLuaScript* )g_GetScript(dwScriptIdA); //dwScriptId
	int nTopIndex = 0;  //记录位置 （栈底）

	if (pScript==NULL)
	{
		//printf(strCoreInfo[L_SortScript_4].c_str(),dwScriptIdA,szFunName);
		return FALSE;
	} 
	try
	{
		if (pScript)
		{	//向栈中压入一个int数字

			nTopIndex=pScript->SafeCallBegin();

			if (pScript->CallFunction(szFunName,0,"ds",nParam,szCanshu)) 
			{
				bExecuteScriptMistake = false;
			} 

			// lua_pop(pScript->m_LuaState, -1); //从栈顶弹出1个元素 -1清空栈
			nTopIndex=pScript->SafeCallBegin();
			pScript->SafeCallEnd(nTopIndex);
			//lua_pop(pScript->m_LuaState, -1); //从栈顶弹出1个元素 -1清空栈
			//	lua_close(pScript->m_LuaState);   //释放内存
		}
		//lua_pop(pScript->m_LuaState, -1); //从栈顶弹出1个元素 -1清空栈   
		if (bExecuteScriptMistake)
		{
			pScript=NULL;
			return FALSE;
		}

		pScript=NULL;
		return TRUE;
	}
	catch(...)
	{

		if (pScript)
		{
			nTopIndex=pScript->SafeCallBegin();	
			pScript->SafeCallEnd(nTopIndex);
		}

		//printf("load script E,Script[%s]!\n",szFunName);
		return FALSE;
	}
	return TRUE;
}

BOOL  SubExecuteScript(DWORD dwScriptId, char * szFunName, char *  szParams,char * snParams,int inParams,int njb,int njxb )
{
	bool bExecuteScriptMistake = true;
	KLuaScript * pScript = (KLuaScript* )g_GetScript(dwScriptId);
	if (pScript==NULL)
	{
		//printf(strCoreInfo[L_SortScript_5].c_str(),szFunName);
		return FALSE;
	}

	int nTopIndex = 0;
	try
	{ 	
		if (pScript)
		{
			//	Lua_PushNumber(pScript->m_LuaState, nIndex.m_nIndex);
			//	pScript->SetGlobalName(SCRIPT_SUBWORLDINDEX);			
			nTopIndex=pScript->SafeCallBegin();
			if ((!szParams) || !szParams[0]) 
			{//如果没有参数	 如果第一次为空，就执行空参数
				if (pScript->CallFunction(szFunName,0,""))
				{
					bExecuteScriptMistake = false;
				}
			}
			else if (pScript->CallFunction(szFunName,0,"ssdddd",szParams,snParams,inParams,njb,njxb,0))  //有参数
			{
				bExecuteScriptMistake = false;
			}
			//}
			nTopIndex=pScript->SafeCallBegin();
			pScript->SafeCallEnd(nTopIndex);
			//#define Lua_Release(L)					lua_close(L)
			//	lua_close(pScript->m_LuaState); //释放内存
			//lua_pop(pScript->m_LuaState, -1); //从栈顶弹出1个元素 -1清空栈
		}
		//lua_pop(pScript->m_LuaState, -1); //从栈顶弹出1个元素 -1清空栈
		if (bExecuteScriptMistake)
		{//执行脚本失败后，就清空
			pScript=NULL;
			return FALSE;
		}
		return TRUE;
	}
	catch(...)
	{
		if (pScript)
		{
			nTopIndex=pScript->SafeCallBegin();	
			pScript->SafeCallEnd(nTopIndex);		
			//pScript->Exit();
		}
		//printf("load Script error,Script[%s],[%d]!\n",szFunName);		
		return FALSE;
	}
	return TRUE;
}

BOOL	SubExecuteScript(char * ScriptFileName, char * szFunName, char *  szParams,char *  snParams, int inParams,int njb,int njxb)
{
	if (!ScriptFileName || !ScriptFileName[0] || !szFunName  || !szFunName[0]) return FALSE;

	char nCurFileName[128]={0};
	snprintf(nCurFileName, sizeof(nCurFileName), "%s", ScriptFileName);
	DWORD dwScriptId = g_FileName2Id_(nCurFileName);

	KLuaScript * pScript = (KLuaScript* )g_GetScript(dwScriptId);
	if (pScript==NULL)
	{
		//printf(strCoreInfo[L_SortScript_4].c_str(),ScriptFileName,szFunName);
		return FALSE;
	}
	pScript=NULL ;

	return SubExecuteScript(dwScriptId, szFunName, szParams,snParams,inParams,njb,njxb);
}
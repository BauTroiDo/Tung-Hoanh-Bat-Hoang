#include "KCore.h"
#include "KSortScript.h"
#include "LuaFuns.h"
#include "KFilePath.h"
#include "KDebug.h"
#include "KTabFile.h"

#ifndef WIN32
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#else 
#include <io.h>
#include <direct.h>
#include <windows.h>
#include <string>
#endif

KLuaScript g_ScriptSet[MAX_SCRIPT_IN_SET];   //脚本数量限制
KScriptBinTree g_ScriptBinTree;
unsigned int   nCurrentScriptNum = 0;
int nLoadModel=0;

void	LoadScriptInDirectory(LPSTR lpszRootDir, LPSTR lpszSubDir);
//重载操作符号
int	operator<(KSortScriptNode ScriptLeft, KSortScriptNode ScriptRight)
{
	return ScriptLeft.GetScriptID() < ScriptRight.GetScriptID();
};
	
int operator==(KSortScriptNode ScriptLeft, KSortScriptNode ScriptRight)
{
	return ScriptLeft.GetScriptID() == ScriptRight.GetScriptID();
};
 
//-------------新的--------------------------------------------------
#include <algorithm>

class KLuaScriptRateMap : public std::/*unordered_*/map<DWORD, KLuaScript *>
{
private:
	static VOID _FreeNode(value_type &cValue)
	{
		//SAFE_DELETE(cValue.second);
		if (cValue.second)
		{//KEY值
			delete (cValue.second);
			(cValue.second) = NULL;
		} 
	}
public:
	~KLuaScriptRateMap()
	{//删除全部节点
#ifdef _SERVER
		std::for_each(begin(), end(), _FreeNode);
#endif

	}
};

static KLuaScriptRateMap g_ScriptRateMap;	// 为了保证内存管理的安全，该变量不再对外

//----------------------------------------------------------------------------------
const KScript * g_GetScript(DWORD dwScriptId)
{//已经加载脚本 查找栈中的节点
	if  (nLoadModel==0)
	{
		KSortScriptNode ScriptNode;
		ScriptNode.SetScriptID(dwScriptId);
		if (g_ScriptBinTree.Find(ScriptNode))
			return ScriptNode.GetScript();
	}
	else
	{
		if (g_ScriptRateMap.count(dwScriptId) > 0)
		{
			return g_ScriptRateMap[dwScriptId];
		} 
	}
	

	return NULL;
}

const KScript * g_GetScript(const char * szRelativeScriptFile)
{   

	DWORD dwScriptId = g_FileName2Id((LPSTR)szRelativeScriptFile);
	return g_GetScript(dwScriptId);
}
///////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
extern int LuaIncludeFile(Lua_State * L);
extern int LuaIncludeLib(Lua_State * L);
#ifndef _SERVER

static void KSceGetFliePath(char* nName,char * nPath,int nMaxRows)
{
	KFile nFile;
	KTabFileCtrl nScirptFile;
	char nTongApplyPath[125]={0},szCol[128]={0};
	sprintf(nTongApplyPath,"%s","\\clientscript.log");
	if (!g_FileExists(nTongApplyPath))	//是否存在
	{
		nFile.Create(nTongApplyPath);
		//sprintf(szCol,"地图\t名称\t源脚本\15\n");
		sprintf(szCol,"脚本Log\15\n");
		nFile.Write(szCol, sizeof(szCol));
		//nFile.Save(nTongApplyPath);
		nFile.Close();
	}
	
	if (nScirptFile.Load(nTongApplyPath))
	{
		int nRows=nScirptFile.GetHeight();
		if  (nRows==0)
			nRows=1;
		
		if (nRows>=nMaxRows)
		{
			//nScirptFile.Clear();
			//return;
			while(nScirptFile.GetHeight()>0)
			{ 
				nScirptFile.Remove(nScirptFile.GetHeight());
				nScirptFile.Save(nTongApplyPath);
			}
		}
		
		snprintf(szCol, sizeof(szCol),"--%s:(%s)--",nName,nPath);
		nScirptFile.InsertAfter(nRows);
		nScirptFile.WriteString(nRows,1, szCol);                                    //行号
		//nTong.WriteString(nRows,2,Npc[Player[m_nPlayerIndex].m_nIndex].Name);     //名字
		//nTong.WriteInteger(nRows,3,Npc[Player[m_nPlayerIndex].m_nIndex].m_Level); //等级
		nScirptFile.Save(nTongApplyPath);	
		//nFile.Write(szCol, sizeof(szCol));
	}
	nScirptFile.Clear();
}
#endif
#include <fstream> 
//#include <cstdio>
//#include <cstring>
static unsigned int g_nScriptTotalScanned = 0;
static unsigned int g_nScriptSuccessCount = 0;
static unsigned int g_nScriptFailedCount = 0;

//加载脚本，该文件名参数为相对目录
static BOOL LoadScriptToSortListA(char * szRelativeFile)
{
	if (!szRelativeFile || !szRelativeFile[0]) return FALSE;

	g_nScriptTotalScanned++;

	int t  =strlen(szRelativeFile);
	if (t >= 256)
		printf("[Script] The length of the loading script[%s]is too long, the script may be wrong! Please check! \n", szRelativeFile);
	
	if (nCurrentScriptNum < MAX_SCRIPT_IN_SET)
	{
		g_ScriptSet[nCurrentScriptNum].Init();
		g_ScriptSet[nCurrentScriptNum].RegisterFunctions(GameScriptFuns, g_GetGameScriptFunNum());
		g_StrCpyLen(g_ScriptSet[nCurrentScriptNum].m_szScriptName, szRelativeFile, 256);
		if (!g_ScriptSet[nCurrentScriptNum].Load(szRelativeFile))
		{
			g_nScriptFailedCount++;
			char szLogPath[MAX_PATH] = {0};
			g_GetFullPath(szLogPath, "\\Logs\\script_errors.txt");
			std::string filePath = szLogPath;
			std::ofstream errorLog(filePath, std::ios::app);
			if (errorLog.is_open()) {
				errorLog << "-[Script] Error loading [" << szRelativeFile << "] please check!-\n";
				errorLog.close(); 
			}

			printf("-[Script] Error loading [%s] please check! (That bai: %u)-\n", szRelativeFile, g_nScriptFailedCount);
			g_ScriptSet[nCurrentScriptNum].Exit();
			return FALSE;
		}		
	}
	else
	{
		g_nScriptFailedCount++;
		printf("-[Script] The number of scripts exceeds the limit [%d] Please solve it now!!-\n", MAX_SCRIPT_IN_SET);
		return FALSE;
	}

	g_nScriptSuccessCount++;
	if (g_nScriptSuccessCount % 200 == 0)
	{
		printf("[Script] Dang load: %5u script OK... (Loi: %u)\n", g_nScriptSuccessCount, g_nScriptFailedCount); fflush(stdout);
	}

	if  (nLoadModel==0)
	{
		KSortScriptNode ScriptNode ;
		ScriptNode.SetScriptID(g_FileName2Id(szRelativeFile));

		ScriptNode.SetScriptIndex(nCurrentScriptNum++);
		g_ScriptBinTree.Insert(ScriptNode);
	}
	else
	{
	    g_ScriptRateMap[g_FileName2Id(szRelativeFile)] = &g_ScriptSet[nCurrentScriptNum];
        nCurrentScriptNum ++;
	}

	return TRUE;
}

//将szFilePath目录下的所有脚本文件加载进ScriptBinTree二叉树中
static unsigned long LoadAllScript(char * szFilePath,BOOL nIsSer)
{
	g_SetRootPath(NULL);
	g_SetFilePath("\\");
	char szRootPath[MAXPATH]={0};
	char szOldRootPath[MAX_PATH];
	
	//	GetCurrentDirectory(MAX_PATH, szOldRootPath);
	if (!getcwd(szOldRootPath, MAXPATH))
		g_GetRootPath(szOldRootPath);	// the same directory g_SetRootPath(NULL) took above
	g_GetFullPath(szRootPath,szFilePath);

	if (nIsSer)
	   LoadScriptInDirectory(szRootPath,"");  //硬盘文件:全路径; 包文件:相对路径
	//-------------------开始加载PAK的脚本----------------
	char szRealDir[128]={0};
	KTabFile nMapScript;
#ifndef _SERVER
	snprintf(szRealDir, sizeof(szRealDir),"%s\\%s",szFilePath,"script.txt");
	//KSceGetFliePath("加载成功",szRealDir);
#else
	snprintf(szRealDir, sizeof(szRealDir), "%s\\%s",szRootPath,"script.txt");
#endif
	if (nMapScript.Load(szRealDir))	//包文件是相对文件
	{
		int nRows=nMapScript.GetHeight();
		for (int i=0;i<nRows;++i)
		{
			nMapScript.GetString(i+1,1,"",szRealDir,sizeof(szRealDir));
			//g_StrUpper(szRealDir); //转成大写 _strupr
			 _strupr(szRealDir);
			if (strstr(szRealDir,".LUA"))
			{
				//g_StrLower(szRealDir);   //大写转小写
				_strlwr(szRealDir);
				LoadScriptToSortListA(szRealDir);
			}
		}
		nMapScript.Clear();
	}
	else
	{
		printf("--加载脚本(%s)文件失败------\n", szRealDir);
#ifndef _SERVER
		//KSceGetFliePath("脚本加载失败",szRealDir);
#endif
	}
	//-----------------------------------------------------------------
	chdir(szOldRootPath);
	//	SetCurrentDirectory(szOldRootPath);
	return nCurrentScriptNum;
}

unsigned long g_IniScriptEngine(char * nScriptDir,BOOL nIsSer,BOOL nIsClear)
{
	g_nScriptTotalScanned = 0;
	g_nScriptSuccessCount = 0;
	g_nScriptFailedCount = 0;
	printf("====================================================================\n");
	printf("[Script] Bat dau load script tu thu muc (Engine Model: %d)...\n", nLoadModel);
	printf("====================================================================\n");
	if (nIsClear)
	{
		nCurrentScriptNum = 0;
		if  (nLoadModel==0)
			g_ScriptBinTree.ClearList();
		else
			ClearAllScript();
	}
	unsigned long ulRet = LoadAllScript(nScriptDir,nIsSer);
	printf("====================================================================\n");
	printf("[Script] HOAN TAT LOAD SCRIPT:\n");
	printf("   * Tong so script da quet  : %u\n", g_nScriptTotalScanned);
	printf("   * Load thanh cong (OK)    : %u\n", g_nScriptSuccessCount);
	printf("   * Load that bai (Failed)  : %u\n", g_nScriptFailedCount);
	printf("====================================================================\n"); fflush(stdout);
	return ulRet;
}

//加载脚本，该文件名参数为实际目录
static BOOL LoadScriptToSortList(char * szFileName)
{
	if (!szFileName || !szFileName[0]) return FALSE;
	if (nCurrentScriptNum>= MAX_SCRIPT_IN_SET)
	{
		printf("[Script]The total script exceeds %d file,please check for serious errors!\n",MAX_SCRIPT_IN_SET);
		return FALSE;
	}

	//int nFileNameLen = strlen(szFileName);
	
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

	    //g_StrLower(szRelativeFile);  //转成小写
	    _strlwr(szRelativeFile);
//	printf("[Script]Loading Script %s %d\n", szRelativeFile, g_FileName2Id(szRelativeFile));
//
	return LoadScriptToSortListA(szRelativeFile);
//	return FALSE;
}



#ifdef WIN32
// Helper to convert ASCII characters in path to lowercase, but NEVER modify Chinese / multibyte trail bytes!
static void SafePathToLowerGBK(char* pStr)
{
	if (!pStr) return;
	unsigned char* p = (unsigned char*)pStr;
	while (*p)
	{
		if (*p >= 0x81 && *p <= 0xFE)
		{
			// Lead byte of GBK character
			p++;
			if (*p) p++; // Skip trail byte
		}
		else
		{
			// Single-byte ASCII
			if (*p >= 'A' && *p <= 'Z')
			{
				*p += ('a' - 'A');
			}
			p++;
		}
	}
}

static void LoadScriptInDirectoryW(const std::wstring& wstrCurrentDir, const std::wstring& wstrGameRootDir)
{
	std::wstring wstrPattern = wstrCurrentDir + L"\\*";
	WIN32_FIND_DATAW wfd;
	HANDLE hFind = FindFirstFileW(wstrPattern.c_str(), &wfd);
	if (hFind == INVALID_HANDLE_VALUE)
		return;

	do
	{
		if (wcscmp(wfd.cFileName, L".") == 0 || wcscmp(wfd.cFileName, L"..") == 0)
			continue;

		std::wstring wstrChildPath = wstrCurrentDir + L"\\" + wfd.cFileName;

		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			LoadScriptInDirectoryW(wstrChildPath, wstrGameRootDir);
		}
		else
		{
			size_t len = wcslen(wfd.cFileName);
			if (len >= 4)
			{
				const wchar_t* pExt = wfd.cFileName + len - 4;
				if (_wcsicmp(pExt, L".lua") == 0)
				{
					// Compute relative path from game root dir
					std::wstring wstrRelPath;
					if (wstrChildPath.length() >= wstrGameRootDir.length() &&
						_wcsnicmp(wstrChildPath.c_str(), wstrGameRootDir.c_str(), wstrGameRootDir.length()) == 0)
					{
						wstrRelPath = wstrChildPath.substr(wstrGameRootDir.length());
					}
					else
					{
						wstrRelPath = wstrChildPath;
					}

					// Ensure leading backslash
					if (wstrRelPath.empty() || (wstrRelPath[0] != L'\\' && wstrRelPath[0] != L'/'))
					{
						wstrRelPath = L"\\" + wstrRelPath;
					}

					// Convert Unicode relative path to GBK (CP936) bytes
					char szRelativeFile[MAX_PATH] = {0};
					int nBytes = WideCharToMultiByte(936, 0, wstrRelPath.c_str(), (int)wstrRelPath.length(), szRelativeFile, sizeof(szRelativeFile) - 1, NULL, NULL);
					if (nBytes > 0)
					{
						szRelativeFile[nBytes] = 0;
						SafePathToLowerGBK(szRelativeFile);
						LoadScriptToSortListA(szRelativeFile);
					}
				}
			}
		}
	} while (FindNextFileW(hFind, &wfd));

	FindClose(hFind);
}
#endif

void	LoadScriptInDirectory(LPSTR lpszRootDir, LPSTR lpszSubDir)
{
#ifdef WIN32
	char szRealDir[MAX_PATH];
	if (lpszSubDir && lpszSubDir[0])
		sprintf(szRealDir, "%s\\%s", lpszRootDir, lpszSubDir);
	else
		sprintf(szRealDir, "%s", lpszRootDir);

	char szGameRoot[MAX_PATH] = {0};
	g_GetRootPath(szGameRoot);
	size_t rootLen = strlen(szGameRoot);
	while (rootLen > 0 && (szGameRoot[rootLen - 1] == '\\' || szGameRoot[rootLen - 1] == '/'))
	{
		szGameRoot[--rootLen] = '\0';
	}

	wchar_t wszRootDir[MAX_PATH] = {0};
	wchar_t wszGameRoot[MAX_PATH] = {0};
	if (MultiByteToWideChar(CP_ACP, 0, szRealDir, -1, wszRootDir, MAX_PATH) <= 0 ||
		GetFileAttributesW(wszRootDir) == INVALID_FILE_ATTRIBUTES)
	{
		MultiByteToWideChar(936, 0, szRealDir, -1, wszRootDir, MAX_PATH);
	}

	if (MultiByteToWideChar(CP_ACP, 0, szGameRoot, -1, wszGameRoot, MAX_PATH) <= 0 ||
		GetFileAttributesW(wszGameRoot) == INVALID_FILE_ATTRIBUTES)
	{
		MultiByteToWideChar(936, 0, szGameRoot, -1, wszGameRoot, MAX_PATH);
	}

	LoadScriptInDirectoryW(wszRootDir, wszGameRoot);
#else
     DIR *dp;
     int i;
     struct dirent *ep;
     char szRealDir[MAX_PATH];
     sprintf(szRealDir, "%s/%s", lpszRootDir, lpszSubDir);
     char *ptr = szRealDir;
     while(*ptr) { if(*ptr == '\\') *ptr = '/'; ptr++; }
     if(chdir(szRealDir)) return;
     dp = opendir(".");
     if(dp) {
          while(ep = readdir(dp)) {
            if(strcmp(ep->d_name, ".") == 0 || strcmp(ep->d_name, "..") == 0) continue;
            
            if(ep->d_type ==4) {
                LoadScriptInDirectory(szRealDir, ep->d_name);
            }
            else {
			int nFlag = 0;
			for (i = 0; i < (int)strlen(ep->d_name); i++)
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
				char szExt[50];
				if (strlen(ep->d_name) >= 4)
				{
					strcpy(szExt, ep->d_name + strlen(ep->d_name) - 4);
                    _strupr(szExt);
					if ((!strcmp(szExt, ".LUA")))
						if (!LoadScriptToSortList(ep->d_name))
							printf("-----Load %s error------\n", ep->d_name);
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
	if  (nLoadModel==0)
	{
		KSortScriptNode ScriptNode;
		ScriptNode.SetScriptID(dwScriptID);
		g_ScriptBinTree.Delete(ScriptNode);
		//printf("-- nLoadModel==0 UnLoadScript %d --\n",dwScriptID);
		return;
	}

	if (g_ScriptRateMap.count(dwScriptID) > 0)
	{//如果存在就 删除
		//return g_ScriptRateMap[dwScriptID];
		//printf("-- UnLoadScript %d --\n",dwScriptID);
		g_ScriptRateMap.erase(dwScriptID);
	}

}
//重载脚本
int ReLoadScript(const char * szRelativePathScript)
{
	if (!szRelativePathScript || !szRelativePathScript[0])
		return FALSE;
	char script[MAX_PATH]={0};
	strcpy(script, szRelativePathScript);
#ifdef WIN32
	SafePathToLowerGBK(script);
#else
	_strlwr(script);
#endif
    //g_StrLower(script);   //大写转小写

	printf("--ReLoadScript: (%s)------\n", szRelativePathScript);

	UnLoadScript(g_FileName2Id(script));  //先删除 在加载
	LoadScriptToSortListA(script);
	return nCurrentScriptNum;
}

unsigned long  ReLoadAllScript()
{
	if  (nLoadModel==0)
    	g_ScriptBinTree.ClearList();
	else
	    ClearAllScript();

	return g_IniScriptEngine("\\script");
}

void  ClearAllScript()
{
	for (KLuaScriptRateMap::iterator it = g_ScriptRateMap.begin(); it != g_ScriptRateMap.end(); ++it)
	{
		if (it->second)
		{
			delete it->second;
			it->second = NULL;
		}
	} 

	g_ScriptRateMap.clear();

}

 
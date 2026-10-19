//// -------------------------------------------------------------------------
////	文件名		：	DoScript.h
////	创建者		：	万里
////	创建时间	：	2003-9-16 21:06:45
////	功能描述	：	
////
//// -------------------------------------------------------------------------
//#ifndef __DOSCRIPT_H__
//#define __DOSCRIPT_H__
//
//#include "../../Engine/src/KWin32.h"
//#include "../../Engine/src/KLuaScript.h"
//
//BOOL InitScript();
//BOOL ExcuteScript(DWORD nIP, DWORD nRelayID, const char * ScriptCommand);
//BOOL UninitScript();
//
//void BroadGlobal(const void* pData, size_t size, DWORD nFromIP, DWORD nFromRelayID);
//void BroadGame(DWORD nToIP, const void* pData, size_t size, DWORD nFromIP, DWORD nFromRelayID);
//void RootSend(DWORD nToIP, DWORD nToRelayID, const void* pData, size_t size);
//
//#endif // __DOSCRIPT_H__


#ifndef KSORTSCRIPT_H_G
#define KSORTSCRIPT_H_G

#include "../../Engine/src/KScriptList.h"
#include "../../Engine/src/KScriptCache.h"
#include "../../Engine/src/KLuaScript.h"
//排序二叉树，保存经过文件名转数字的脚本的id,并实现快速查找对应脚本
#include "../../Engine/src/KBinTreeNode.h"
#include "../../Engine/src/KBinsTree.h"

//extern char g_szCurScriptDir[MAX_PATH];

#define MAX_SCRIPT_IN_SET_G 5000

extern KLuaScript g_ScriptSet[MAX_SCRIPT_IN_SET_G];

class KSortScriptNode
{

	DWORD m_dwScriptIndex;
	DWORD m_dwScriptID;
public:
	DWORD GetScriptIndex(){return m_dwScriptIndex;};
	DWORD GetScriptID(){return m_dwScriptID;};

	void SetScriptIndex(DWORD dwScriptIndex){m_dwScriptIndex = dwScriptIndex;};
	void SetScriptID(DWORD dwScriptID){m_dwScriptID = dwScriptID;};

	const KScript * GetScript()
	{
		if (m_dwScriptIndex >= 0 && m_dwScriptIndex <MAX_SCRIPT_IN_SET_G) 
			return &g_ScriptSet[m_dwScriptIndex];
		else
			return NULL;
	}
	;

};

typedef	BinSTree<KSortScriptNode> KScriptBinTree;
extern KScriptBinTree g_ScriptBinTree;
extern unsigned long g_IniScriptEngine();
extern void UnLoadAllScript();
extern BOOL ExcuteScript(DWORD nIP, DWORD nRelayID, const char * ScriptCommand);
extern const KScript * g_GetScript(DWORD dwScriptId);
extern const KScript * g_GetScript(const char * szRelativeScriptFile);
extern BOOL ReLoadScript(const char * szRelativePathScript);
extern unsigned long  ReLoadAllScript();

void BroadGlobal(const void* pData, size_t size, DWORD nFromIP, DWORD nFromRelayID);
void BroadGame(DWORD nToIP, const void* pData, size_t size, DWORD nFromIP, DWORD nFromRelayID);
void RootSend(DWORD nToIP, DWORD nToRelayID, const void* pData, size_t size);

extern BOOL	SubExecuteScriptA(char * ScriptFileName, char * szFunName, int nParam = 0,char * szCanshu=NULL);
extern BOOL	SubExecuteScript(char * ScriptFileName, char * szFunName, char * szParams=NULL,char * snParams=NULL,int inParam = 0,int njb=0,int njxb=0);
extern BOOL	SubExecuteScript(DWORD dwScriptId, char * szFunName, char *  szParams=NULL,char * snParams=NULL,int inParam = 0,int njb=0,int njxb=0);
extern BOOL	SubExecuteScriptB(char* dwScriptIdA,  char * szFunName, int nParam=0,char * szCanshu=NULL);

#endif
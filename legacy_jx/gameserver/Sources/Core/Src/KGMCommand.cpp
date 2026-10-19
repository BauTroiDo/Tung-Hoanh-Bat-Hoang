#include "KCore.h"
#include "KPlayer.h"
#include "LuaFuns.h"
#include "KGMCommand.h"
#include "KSortScript.h"
#define GMCMD_GENRE_PREFIX_LEN 3
#define GM_CTRL_CMD_PREFIX '?'

static TGameMaster_Command GM_Command[]=
{
	{"FsDoSct",			GMDoScriptAction},				//DoSct Say("abc");
	{"Fsds",				GMDoScriptAction},				//DS Say("abc");

	{"Fsdw",				GMDoWorldScriptAction},			//dw AddNews("abc");

	{"FsRunSctFile",		GMRunScriptFile},				//RunSctFile /Scripts/Abc.lua main 10
	{"FsRSF",				GMRunScriptFile},				//RSF /Scripts/Abc.lua main 10

	{"FsReLoadSct",		GMReloadScriptFile},			//ReloadSct 重载单个脚本 /Scripts/Abc.lua
	{"FsRLS",				GMReloadScriptFile},
	
	{"FsReLoadAllSct",	GMReloadAllScriptFile},			//ReloadAllSct  重载全部脚本
	{"FsRLAS",			GMReloadAllScriptFile},
	
};
#ifdef _DEBUG
CORE_API BOOL TextGMFilter(int nPlayerIdx, const char* pText, int nLen)
#else
BOOL TextGMFilter(int nPlayerIdx, const char* pText, int nLen)  //过滤GM命令
#endif
{
	try
	{
		
		if (!pText)	return FALSE;
		bool	bHandled = false;
		if (nLen >= GMCMD_GENRE_PREFIX_LEN + 1 && pText[0] == GM_CTRL_CMD_PREFIX)
		{
			if ((*(unsigned int*)pText) == 0x206D673F || (*(unsigned int*)pText) == 0x204D473F)	// 0x2067642F = "/gm " "/GM "
			{
				bHandled = (bool)(0 != TextMsgProcessGMCmd(nPlayerIdx, pText + GMCMD_GENRE_PREFIX_LEN + 1,
					nLen - GMCMD_GENRE_PREFIX_LEN - 1));
				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}
		return FALSE;
	}
	catch(...)
	{
		printf("执行GM指令,发生异常!\n");
		return FALSE;
	}

	
}

BOOL TextMsgProcessGMCmd(int nPlayerIdx, const char * pGMCmd, int nLen)
{
	if (nLen <= 0 || !pGMCmd) return FALSE;
	char szCmd[20];
	const char * pStart = strstr(pGMCmd," ");
	int nTempLen = nLen;
	int nCmdLen = nLen;
	
	if (NULL == pStart)
	{
		// the command word is a name out of a 20-byte array; the sentence it was cut
		// from is measured in the packet, so the copy length needs its own bound and
		// the terminator goes where the copy stopped rather than at nLen, which is a
		// second, unbounded write into the same buffer
		nCmdLen = nTempLen;
		if (nCmdLen > (int)sizeof(szCmd) - 1)
			nCmdLen = (int)sizeof(szCmd) - 1;
		memcpy(szCmd, pGMCmd, nCmdLen);
		szCmd[nCmdLen] = 0;
	}
	else
	{	nTempLen = pStart - pGMCmd;
		nCmdLen = nTempLen;
		if (nCmdLen > (int)sizeof(szCmd) - 1)
			nCmdLen = (int)sizeof(szCmd) - 1;
		memcpy(szCmd, pGMCmd, nCmdLen);
		szCmd[nCmdLen] = 0;
	}

	for(int i  = 0; i < sizeof(GM_Command) / sizeof(TGameMaster_Command); i ++)
	{
		if (strcmp(GM_Command[i].Command, szCmd) == 0)
			return ProcessGMCommand(nPlayerIdx, GM_Command[i].eCommandId, pGMCmd + nTempLen + 1, nLen - nTempLen - 1);
	}
		
	return FALSE;
}

BOOL  ProcessGMCommand(int nPlayerIdx, EGameMasterCommand eCommand, const char * pParam, int nLen)
{
	
	switch(eCommand)
	{
	
	case GMDoWorldScriptAction:
		{
			if (nLen <=0 || nLen >= 300)
				return FALSE;
			char szScriptAction[300];
			memcpy(szScriptAction, pParam, nLen);
			szScriptAction[nLen] = 0;
			BOOL bResult = FALSE;
			KLuaScript WorldScript;
			WorldScript.Init();
			WorldScript.RegisterFunctions(WorldScriptFuns, g_GetWorldScriptFunNum());
			
			if (WorldScript.LoadBuffer((PBYTE)szScriptAction, nLen))
			{
				bResult = WorldScript.ExecuteCode();//执行这个源码	
			}
			return bResult;

		}break;
 
	case GMDoScriptAction:
		{
			if (nLen <= 0 || nLen >= 300) 
				return FALSE;
			//检查nPlayerIdx的权限
			if (nPlayerIdx < 0 || Player[nPlayerIdx].m_dwID <= 0)
				return FALSE;
			char szScriptAction[300];
			memcpy(szScriptAction, pParam, nLen);
			szScriptAction[nLen] = 0;
			Player[nPlayerIdx].DoScript(szScriptAction);   //GM
			return TRUE;

		}break;
  /*
	case GMRunScriptFile:
		{
			if (nPlayerIdx < 0 || Player[nPlayerIdx].m_dwID <= 0)
				return FALSE;
			char szScriptFile[200];
			char szScriptFun[100];
			char szScriptParam[100];
			int nBufLen = GetNextUnit(pParam, ' ', nLen, szScriptFile);
			if (szScriptFile[0] == 0) return FALSE;

			int nBufLen1 = GetNextUnit(pParam + nBufLen, ' ', nLen - nBufLen, szScriptFun);
			if (szScriptFun[0] == 0) return FALSE;
			
			GetNextUnit(pParam + nBufLen + nBufLen1, ' ', nLen - nBufLen - nBufLen1, szScriptParam);
			return Player[nPlayerIdx].ExecuteScriptC(szScriptFile, szScriptFun, szScriptParam,"");
			
		}break;
	case GMReloadScriptFile: // 重载脚本
		{
			if (nPlayerIdx < 0 || Player[nPlayerIdx].m_dwID <= 0)	return FALSE;
			char szScriptFile[200];
			GetNextUnit(pParam, ' ', nLen, szScriptFile);
			if (szScriptFile[0] == 0) return FALSE;
			ReLoadScript(szScriptFile);
			return TRUE;

		}break;
	case GMReloadAllScriptFile:
		{
			return ReLoadAllScript();
		}break;	*/
	default:
		break;
	}
	
	return FALSE;
}

int GetNextUnit(const char * szString , const char cDiv, int nLen, char * szResult)
{
	szResult[0] = 0;
	if (nLen <= 0 || szString == NULL)
		return FALSE;
	
	char * pChar = (char *)szString;
	int i = 0;
	int j = 0;
	BOOL bFind = FALSE;

	// the length first, as the copy loop below it already does: reading szString[i] on
	// the trip where i has reached nLen looks one byte past what the caller handed over
	while (i < nLen && *(pChar + i) == cDiv) i++;
	
	while(i  < nLen && *(pChar + i) != '\0')
	{
		if (*(pChar + i) == cDiv) 
		{
			szResult[j] = 0;
			return i;
		}
		else
			szResult[j++] = *(pChar + i);
		i ++;
	}

	szResult[j] = 0;
	return i;	
}

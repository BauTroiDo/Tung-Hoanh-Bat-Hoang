//---------------------------------------------------------------------------
// Sword3 Engine (c) 2002 by Kingsoft
//
// File:	KPlayerTask.cpp
// Date:	2002.10.05
// Code:	边城浪子
// Desc:	PlayerTask Class
//---------------------------------------------------------------------------

#include	"KCore.h"
#include	"KPlayerTask.h"

#ifdef _SERVER
//int			g_TaskGlobalValue[TASKGLOBALVALUENUM]; //全局的变量，用于服务器脚本系统

KTabFile	g_WayPointTabFile;
KTabFile	g_StationTabFile;
KTabFile	g_DockTabFile;
KTabFile	g_StationPriceTabFile;
KTabFile	g_WayPointPriceTabFile;
KTabFile	g_DockPriceTabFile;

int	*		g_pStationPriceTab = NULL;
int	*		g_pWayPointPriceTab = NULL;
int	*		g_pDockPriceTab = NULL;

#endif
//---------------------------------------------------------------------------
//	功能：构造函数
//---------------------------------------------------------------------------
KPlayerTask::KPlayerTask()
{
	Release();
}

//---------------------------------------------------------------------------
//	功能：清空
//---------------------------------------------------------------------------
void	KPlayerTask::Release()
{
	//memset(nSave, 0, sizeof(nSave));
	//memset(nClear, 0, sizeof(nClear));
	m_FactionNo=-1;
	m_nPlayerNetConnectIdx = -1;
	/*_KTaskTemplate::iterator it;
		for( it = m_TaskTemplate.begin(); it != m_TaskTemplate.end(); ++it )
		{
	         it->second = 0;
		}
	   */

       m_TaskTemplate.clear();
	/*_KTasklate::iterator its;
		for( its = m_Tasklate.begin(); its != m_Tasklate.end(); ++its )
		{
			its->second = 0;
		}
	*/	
       m_Tasklate.clear();
	
#ifndef _SERVER
	ZeroMemory(nTaskInfo,sizeof(nTaskInfo));
#endif
}

//---------------------------------------------------------------------------
//	功能：清除临时过程控制变量
//---------------------------------------------------------------------------
void	KPlayerTask::ClearTempVar()
{
	//memset(nClear, 0, sizeof(nClear));
	/*_KTaskTemplate::iterator it;
	for( it = m_TaskTemplate.begin(); it != m_TaskTemplate.end(); ++it )
	{
		it->second = 0;
	}*/
	
	m_TaskTemplate.clear();
}

//---------------------------------------------------------------------------
//	功能：得到任务完成情况
//---------------------------------------------------------------------------
int		KPlayerTask::GetSaveVal(int nNo)
{
	if (nNo < 0 || nNo >= MAX_TASK)
		return 0;
	if (m_Tasklate.count(nNo)>0)
		return m_Tasklate[nNo];
	else
		return 0;

	//return nSave[nNo];
}


void	KPlayerTask::Init( int nConnectIdx)
{
	m_nPlayerNetConnectIdx =nConnectIdx;
}
//---------------------------------------------------------------------------
//	功能：设定任务完成情况
//---------------------------------------------------------------------------
void	KPlayerTask::SetSaveVal(int nNo, int bFlag,bool isSyn)
{
	if (nNo < 0 || nNo >= MAX_TASK)
		return;

	m_Tasklate[nNo] = bFlag;

	////m_Tasklate.insert(make_pair(nNo,bFlag));

	if (nNo==130)
		m_FactionNo=bFlag;
	//nSave[nNo] = bFlag;  //值
    if (isSyn && m_nPlayerNetConnectIdx>=0)
	{//同步到客户端
		PLAYER_GETBACK_TASKVAL	sTaskBack;
		sTaskBack.ProtocolType = s2c_taskcallback;
		sTaskBack.m_TaskId   =nNo;
		sTaskBack.m_TaskVal = bFlag;
		if (g_pServer)
			g_pServer->PackDataToClient(m_nPlayerNetConnectIdx, (BYTE*)&sTaskBack, sizeof(PLAYER_GETBACK_TASKVAL));

	}

}






//---------------------------------------------------------------------------
//	功能：得到临时过程控制变量值
//---------------------------------------------------------------------------
int		KPlayerTask::GetClearVal(int nNo)
{
	if (nNo < 0 || nNo >= MAX_TEMP_TASK)
		return 0;
	if (m_TaskTemplate.count(nNo)>0)
		return m_TaskTemplate[nNo];
	else
		return 0;
	//return nClear[nNo];
}

//---------------------------------------------------------------------------
//	功能：设定临时过程控制变量值
//---------------------------------------------------------------------------
void	KPlayerTask::SetClearVal(int nNo, int nVal)
{
	if (nNo < 0 || nNo >= MAX_TEMP_TASK)
		return;

	m_TaskTemplate[nNo] = nVal; 
	//m_TaskTemplate.insert(make_pair(nNo,nVal));
	//nClear[nNo] = nVal;
}

 /*
int  KPlayerTask::SetTaskByte(int nIntValue,int nByteNumber,int nByteValue)
{
	BYTE * pByte =	NULL;

	nByteValue = (nByteValue & 0xff);
	
	if (nByteNumber > 4 || nByteNumber <= 0) ///4
		return nIntValue;
	
	pByte = (BYTE*)&nIntValue;
	*(pByte + (nByteNumber -1)) = (BYTE)nByteValue;
	//nIntValue = (nIntValue | (0xff << ((nByteNumber - 1) * 8) )) ;
	//Lua_PushNumber(L, nIntValue);
	return nIntValue;
}

int  KPlayerTask::GetTaskByte(int nIntValue,int nByteNumber)
{
	int nByteValue = 0;

	if (nByteNumber > 4 || nByteNumber <= 0) 
		return nByteValue;

	nByteValue = (nIntValue & (0xff << ((nByteNumber - 1) * 8) )) >> ((nByteNumber - 1) * 8);
	//Lua_PushNumber(L, nByteValue);
	return nByteValue;
}
 */
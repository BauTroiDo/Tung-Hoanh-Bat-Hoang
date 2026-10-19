#ifndef	KSubWorldSetH
#define	KSubWorldSetH

#include "KTimer.h"
#ifndef _SERVER
#include "KMapMusic.h"
#else
#include "KSubWorld.h"

#endif

#define   MAX_TASK_COUNT 100

struct serTaskInfo
{
	int  nIdx;
	int  nStartHour;
	int  nStartMin;
	int  nEndHour;
	int  nEndMin;
	int  nIsOpen;
	int  IsYes;
	time_t  nOpenLoop;
	time_t  nCloseLoop;
	int  nState;     //状态
	int  nJGTimes[2];
	int  EndTime[5];
	int  StartTime[5];
	char nLuaField[128];
	char ntaskName[64];
	char nNextTaskKey[64];
	int  nIsSaceSyn;
};

class CORE_API KSubWorldSet
{
public:
	UINT  m_nLoopRate;		// 循环帧数
#ifndef _SERVER
	KMapMusic	m_cMusic;
#endif
private:
//	KTimer	m_TaskTimer;			// 计时器
	int		m_nGameVersion;
	DWORD		m_dwCurrentTime;				// 当前帧
#ifndef _SERVER
	DWORD	m_dwPing;
#endif

#ifdef _SERVER
	int  nSubWorldLoop;
	int	  m_nTaskLoopRate;
	int  nCount; //任务的数量
	int  IsStart;
	typedef std::/*unordered_*/map<int,serTaskInfo> _MapLoopTaskInfo;
	_MapLoopTaskInfo nMapLoopTask;
#endif

public:
	KSubWorldSet();
	bool	LoadTaskInfo();
	BOOL	LoadSerMaps(LPSTR szFileName);
	int		SearchWorld(DWORD dwID);
	BOOL	EnsureMapLoaded(int nSubWorldIdx);
	BOOL	EnsureMapLoadedById(DWORD dwMapId);
	int     SearchWorldRows(int dwID);
	void	MainLoop();
	void	MessageLoop();
	BOOL	SendMessage(int nSubWorldID, DWORD	dwMsgType, int nParam1 = 0, int nParam2 = 0, int nParam3 = 0);
	UINT	GetGameTime(){return m_nLoopRate;};
	int		GetGameVersion() { return m_nGameVersion; }
	DWORD	GetCurTime() { return m_dwCurrentTime; }
	VOID  	SetCurTime(DWORD dwFrame) { m_dwCurrentTime = dwFrame; };
	void	Close();

	BOOL	SubExecuteScriptA(char * ScriptFileName, char * szFunName, int nParam = 0,char * szCanshu=NULL);
	BOOL	SubExecuteScript(char * ScriptFileName, char * szFunName, char * szParams=NULL,char * snParams=NULL,int inParam = 0,int njb=0,int njxb=0);
	BOOL	SubExecuteScript(DWORD dwScriptId, char * szFunName, char *  szParams=NULL,char * snParams=NULL,int inParam = 0,int njb=0,int njxb=0);
	BOOL	SubExecuteScriptB(char* dwScriptIdA,  char * szFunName, int nParam=0,char * szCanshu=NULL);

// Server上才有的几个方法
#ifdef _SERVER
	int	GetRevivalPosFromId(DWORD dwSubWorldId, int nRevivalId, POINT* pPos);
	int     GetRevivalPosRandIdx(DWORD dwSubWorldId,POINT* pPos);
#endif
// Client上才有的几个方法
#ifndef _SERVER
	void	SetPing(DWORD dwTimer) { m_dwPing = dwTimer; }
	DWORD	GetPing() { return m_dwPing; }
	void	Paint();
#endif
};
extern CORE_API KSubWorldSet g_SubWorldSet;
#endif

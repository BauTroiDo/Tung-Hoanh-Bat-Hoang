/* -------------------------------------------------------------------------
//	文件名		：	kplayback_interface.h
//	创建者		：	wangbin
//	创建时间	：	2008-4-3 15:07:00
//	功能描述	：	录像功能相关接口
// -----------------------------------------------------------------------*/
#ifndef __KPLAYBACK_INTERFACE_H__
#define __KPLAYBACK_INTERFACE_H__

#include "ksdef.h"
#include <time.h>

// 游戏帧数更新
class IKLoopCounter
{
public:
	virtual VOID Loop(DWORD& dwGameLoop) = 0;
};

// 时间服务
class IKSysService
{
public:
	virtual void			Release() = 0;
	virtual time_t			Time(time_t* pTimer) = 0;					// == time()
	virtual DWORD			GetTickCount() = 0;							// == GetTickCount()
	virtual tm*				LocalTime(time_t* pTimer) = 0;				// == localtime()
	virtual void			Srand(UINT nSeed) = 0;						// == srand(unsigned int)
	virtual int				Rand() = 0;									// == rand()
	virtual unsigned int	GetElapse() = 0;							// KTimer::GetElapse() 
	virtual unsigned int	Engine_Random(unsigned int nMax) = 0;		// engine: g_Random
	virtual void			Engine_RandomSeed(unsigned int nSeed) = 0;	// engine: g_RandomSeed
	virtual unsigned int	Engine_GetRandomSeed() = 0;					// engine: g_GetRandomSeed
};

struct IKSocketAcceptorEvent // : IUnknown
{
	virtual INT OnStartWait() = 0;
	virtual INT OnFinishWait(INT nResult) = 0;
	virtual INT OnClientConnect(INT nConnectIdx) = 0;
	virtual INT OnClientDataCome(INT nConnectIdx) = 0;
	virtual INT OnClientDisConnect(INT nConnectIdx) = 0;
};

#endif //__KPLAYBACK_INTERFACE_H__

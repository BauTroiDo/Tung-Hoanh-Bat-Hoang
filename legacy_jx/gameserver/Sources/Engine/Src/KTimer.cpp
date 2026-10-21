//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KTimer.cpp
// Date:	2000.08.08
// Code:	WangWei(Daphnis)
// Desc:	Timer Class
//---------------------------------------------------------------------------
#include "KWin32.h"

#ifdef WIN32
#include <Winbase.h>
#include "KDebug.h"
#include "KMemBase.h"
#endif
#include "KTimer.h"
#include <time.h>
// flying add this macro.
// must pay more attention in the platform porting job.
#ifdef __linux
#include <stdint.h>
#endif

//#include <time.h>
// 从1601年1月1日0:0:0:000到1970年1月1日0:0:0:000的时间(单位100ns)
#define EPOCHFILETIME (116444736000000000UL)

#ifdef WIN32
int gettimeofday(struct timeval * val, struct timezone *)
{
	if (val)
	{
		LARGE_INTEGER liTime, liFreq;
		QueryPerformanceFrequency( &liFreq );
		QueryPerformanceCounter( &liTime );
		val->tv_sec     = (long)( liTime.QuadPart / liFreq.QuadPart );
		val->tv_usec    = (long)( liTime.QuadPart * 1000000.0 / liFreq.QuadPart - val->tv_sec * 1000000.0 );
	}
	return 0;
}
#endif  // WIN32

#ifdef __linux
// A monotonic clock avoids wall-clock changes changing gameplay deadlines.
static LONGLONG TimerNanoseconds(const timespec& from, const timespec& to)
{
    if ((from.tv_sec == 0 && from.tv_nsec == 0) ||
        (to.tv_sec == 0 && to.tv_nsec == 0))
        return 0;
    const LONGLONG seconds = static_cast<LONGLONG>(to.tv_sec) - from.tv_sec;
    const LONGLONG nanos = seconds * 1000000000LL + to.tv_nsec - from.tv_nsec;
    return nanos > 0 ? nanos : 0;
}
#endif

KTimer::KTimer()
{
#ifdef WIN32

	//m_nTimeStart.QuadPart = 0;
	//m_nTimeStop.QuadPart  = 0;
	memset(&m_nTimeStart,0,sizeof(m_nTimeStart));
	memset(&m_nTimeStop,0,sizeof(m_nTimeStop));

	//m_nFPS = 0;
	//SetThreadAffinityMask(GetCurrentThread(),0x00000001);
	//QueryPerformanceFrequency(&m_nFrequency);    //对于多核出现错误  返回硬件支持的高精度计数器的频率。获取 cpu频率
	m_nFrequency.QuadPart = 200 * 1024 * 1024;
	m_nFPS = 0;
	_useKind=TIME_USE_QUERY;
	if  (!QueryPerformanceFrequency(&m_nFrequency))
	{//硬件不支持
		printf("-----硬件不支持QueryPerformanceFrequency高精度计数器----\n");
		printf("-----硬件不支持QueryPerformanceFrequency高精度计数器----\n");
		printf("-----硬件不支持QueryPerformanceFrequency高精度计数器----\n");
		_useKind=TIME_USE_TICK;
	}
#else
    m_nTimeStart = timespec{0, 0};
    m_nTimeStop = timespec{0, 0};
    m_nFPS = 0;
    _useKind = TIME_USE_QUERY;
#endif
}
//---------------------------------------------------------------------------
// 函数:	Start
// 功能:	开始计时
// 参数:	void
// 返回:	void
//---------------------------------------------------------------------------
void KTimer::Start()
{
#ifdef WIN32
	switch(_useKind)
	{
	case TIME_USE_QUERY:
		QueryPerformanceCounter(&m_nTimeStart);
		break;
	default:
		m_nTimeStart.QuadPart=GetTickCount64();// 精确是毫秒
		break;
	}
	//gettimeofday(&m_nTimeStart, NULL);
#else
    if (clock_gettime(CLOCK_MONOTONIC, &m_nTimeStart) != 0)
        m_nTimeStart = timespec{0, 0};
    m_nTimeStop = timespec{0, 0};
#endif
}
//---------------------------------------------------------------------------
// 函数:	Stop
// 功能:	停止计时
// 参数:	void
// 返回:	void
//---------------------------------------------------------------------------

void KTimer::Stop()
{
#ifdef WIN32
	switch(_useKind)
	{
	case TIME_USE_QUERY:
		QueryPerformanceCounter(&m_nTimeStop);
		break;
	default:
		m_nTimeStop.QuadPart=GetTickCount64();// 精确是毫秒
		break;
	}
	//gettimeofday(&m_nTimeStop, NULL);
#else
    if (clock_gettime(CLOCK_MONOTONIC, &m_nTimeStop) != 0)
        m_nTimeStop = timespec{0, 0};
#endif
}
//---------------------------------------------------------------------------
// 函数:	GetElapse
// 功能:	计算从开始计时到现在已经过到时间
// 参数:	void
// 返回:	DWORD in ms		 GetTickCount() 精确是毫秒
//---------------------------------------------------------------------------
LONGLONG KTimer::GetElapse()
{
#ifdef WIN32

	LARGE_INTEGER nTime;
	LONGLONG ver=0;
	switch(_useKind)
	{
	case TIME_USE_QUERY:
		QueryPerformanceCounter(&nTime);
		ver= ((nTime.QuadPart - m_nTimeStart.QuadPart) * 1000 / m_nFrequency.QuadPart);
		break;
	default:
		nTime.QuadPart=GetTickCount64();// 精确是毫秒
		ver= (nTime.QuadPart - m_nTimeStart.QuadPart); 
		break;
	}
	return ver;
	//timeval nTime;
	//gettimeofday(&nTime, NULL);
	// return (nTime.tv_sec - m_nTimeStart.tv_sec) * 1000 + nTime.tv_usec / 1000;  //毫秒
#else
    timespec now{};
    if (clock_gettime(CLOCK_MONOTONIC, &now) != 0)
        return 0;
    return TimerNanoseconds(m_nTimeStart, now) / 1000000LL;
#endif
}
//---------------------------------------------------------------------------
// 函数:	GetElapseFrequency
// 功能:	计算从开始计时到现在已经过到时间
// 参数:	void
// 返回:	DWORD in frequency
//---------------------------------------------------------------------------
LONGLONG  KTimer::GetElapseFrequency()
{
#ifdef WIN32

	LARGE_INTEGER nTime;
	LONGLONG ver=0;
	switch(_useKind)
	{
	case TIME_USE_QUERY:
		QueryPerformanceCounter(&nTime);
		ver=(nTime.QuadPart - m_nTimeStart.QuadPart);
		break;
	default:
		nTime.QuadPart=GetTickCount64();// 精确是毫秒
	    ver=(nTime.QuadPart - m_nTimeStart.QuadPart);
		break;
	}
	return ver;

	//timeval tv;
	//gettimeofday(&tv, NULL); //原来
	//return (tv.tv_sec - m_nTimeStart.tv_sec) * 1000 + tv.tv_usec / 1000;  //毫秒
#else
    timespec now{};
    if (clock_gettime(CLOCK_MONOTONIC, &now) != 0)
        return 0;
    return TimerNanoseconds(m_nTimeStart, now);
#endif
}
//---------------------------------------------------------------------------
// 函数:	GetInterval
// 功能:	取得从开始到停止之间的时间间隔，以毫秒为单位
// 参数:	void
// 返回:	毫秒值
//---------------------------------------------------------------------------
LONGLONG KTimer::GetInterval()
{
#ifdef WIN32
	//return (DWORD)((m_nTimeStop.QuadPart - m_nTimeStart.QuadPart) * 1000 / m_nFrequency.QuadPart); //毫秒为单位
    return 0;//(m_nTimeStop.QuadPart - m_nTimeStart.QuadPart);
#endif
#ifdef __linux
    return TimerNanoseconds(m_nTimeStart, m_nTimeStop) / 1000000LL;
#else
    return 0;
#endif
}
//---------------------------------------------------------------------------
// 函数:	Passed
// 功能:	看是否过了nTime毫秒
// 参数:	nTime	毫秒
// 返回:	TRUE	已经过了
//			FALSE	还没有过
//---------------------------------------------------------------------------
BOOL KTimer::Passed(int nTime)
{

	if (GetElapse() >= (DWORD)nTime)
	{
		Start();    //重新设定开始时间
		return TRUE;
	}
	return FALSE;
}
//---------------------------------------------------------------------------
// 函数:	GetFPS
// 功能:	取得帧速率
// 参数:	nFPS	帧速率
// 返回:	TRUE	成功
//			FALSE	失败
//---------------------------------------------------------------------------
BOOL KTimer::GetFPS(int *nFPS)
{
#ifndef WIN32
    if (nFPS == NULL)
        return FALSE;
#endif
#ifdef WIN32
	if (GetElapse() >= 1000UL)
#else
	if (GetElapse() >= 1000LL)
#endif
	{
		*nFPS = m_nFPS;
		m_nFPS = 0;
		Start();     //重新设置开始时间
		return TRUE;
	}
	m_nFPS++;
	return FALSE;
}

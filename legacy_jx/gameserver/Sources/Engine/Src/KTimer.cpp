//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KTimer.cpp
// Date:	2000.08.08
// Code:	WangWei(Daphnis)
// Desc:	Timer Class
//---------------------------------------------------------------------------
#include "KWin32.h"

#include <Winbase.h>

#include "KDebug.h"
#include "KMemBase.h"
#include "KTimer.h"
#include <time.h>
// flying add this macro.
// must pay more attention in the platform porting job.
#ifdef __linux
#include <sys/time.h>
#endif

//#include <time.h>
// 从1601年1月1日0:0:0:000到1970年1月1日0:0:0:000的时间(单位100ns)
#define EPOCHFILETIME (116444736000000000UL)

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
    m_nFrequency = CLOCKS_PER_SEC; //以下原来没有的
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
    gettimeofday(&m_nTimeStart, NULL); //原来
//	QueryPerformanceCounter(&m_nTimeStart);
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
	gettimeofday(&m_nTimeStop, NULL); //原来
//	QueryPerformanceCounter(&m_nTimeStop);
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
	timeval tv;
	gettimeofday(&tv, NULL); //原来
    return (tv.tv_sec - m_nTimeStart.tv_sec) * 1000 + tv.tv_usec / 1000;  //毫秒
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
#endif
	return 0;
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
  // return (DWORD)((m_nTimeStop.QuadPart - m_nTimeStart.QuadPart) * 1000 / m_nFrequency.QuadPart);
	return 0; //原来
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
	if (GetElapse() >= 1000UL) 
	{
		*nFPS = m_nFPS;
		m_nFPS = 0;
		Start();     //重新设置开始时间
		return TRUE;
	}
	m_nFPS++;
	return FALSE;
}

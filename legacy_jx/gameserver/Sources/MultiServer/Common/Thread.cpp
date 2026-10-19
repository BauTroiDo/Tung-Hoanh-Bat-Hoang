#include "stdafx.h"
#include "Thread.h"
#include "Macro.h"

#include <process.h>		//Thread define

#include "Win32Exception.h"

/*
 * namespace OnlineGameLib::Win32
 */

//namespace OnlineGameLib {
//namespace Win32 {

CThread::CThread()
	: m_hThread( NULL )
{

}
      
CThread::~CThread()
{
	SAFE_CLOSEHANDLE( m_hThread );
}

HANDLE CThread::GetHandle() const
{
	return m_hThread;
}

void CThread::Start()
{
	if (m_hThread ==NULL)
	{
		unsigned int threadID = 0;
		//开始创建线程
		m_hThread = (HANDLE)::_beginthreadex(0, 
			0, 
			ThreadFunction,
			(void *)this, 
			0, 
			&threadID);
		//printf("----开始创建线程线程----\n");
		//第1个参数：安全属性，NULL为默认安全属性
		//第2个参数：指定线程堆栈的大小。如果为0，则线程堆栈大小和创建它的线程的相同。一般用0
		//第3个参数：指定线程函数的地址，也就是线程调用执行的函数地址(用函数名称即可，函数名称就表示地址)
		//第4个参数：传递给线程的参数的指针，可以通过传入对象的指针，在线程函数中再转化为对应类的指针
		//第5个参数：线程初始状态，0:立即运行；CREATE_SUSPEND：suspended（悬挂）
        //第6个参数：用于记录线程ID的地址

		if (m_hThread == NULL)
		{
			throw CWin32Exception(_T("CThread::Start() - _beginthreadex"), GetLastError());
		}
	}
	else
	{
		throw CException(_T("CThread::Start()"), _T("Thread already running - you can only call Start() once!"));
	}
}

void CThread::Wait() const
{
	if ( !Wait( INFINITE ) )
	{
		throw CException(_T("CThread::Wait()"), _T("Unexpected timeout on infinite wait"));
	}
}

bool CThread::Wait(DWORD timeoutMillis) const
{
	bool ok;

	if ( !m_hThread )
	{
		return true;
	}

	DWORD result = ::WaitForSingleObject( m_hThread, timeoutMillis );

	if ( result == WAIT_TIMEOUT )
	{
		ok = false;
	}
	else if ( result == WAIT_OBJECT_0 )
	{
		ok = true;
	}
	else
	{
		throw CWin32Exception( _T( "CThread::Wait() - WaitForSingleObject" ), ::GetLastError() );
	}
    
	return ok;
}
//线程回调函数
unsigned int __stdcall CThread::ThreadFunction(void *pV)
{
	int result = 0;

	CThread* pThis = ( CThread * )pV;
   
	if (pThis)
	{
		try
		{
			result = pThis->Run();
		}
		catch( ... )
		{
		//	TRACE( "CThread::ThreadFunction exception!" );
			printf("----线程运行出错----\n");
		}
	}

	return result;
}
//终止线程
void CThread::Terminate(DWORD exitCode /* = 0 */)
{
	if ( m_hThread && !::TerminateThread(m_hThread,exitCode))
	{
		//TRACE( "CThread::Terminate error!" );
	}

	SAFE_CLOSEHANDLE(m_hThread);
}

//} // End of namespace OnlineGameLib
//} // End of namespace Win32
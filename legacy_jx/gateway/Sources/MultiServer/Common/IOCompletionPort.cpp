#include "stdafx.h"
#include "IOCompletionPort.h"
#include "Win32Exception.h"
#include "Macro.h"
#include "Socket.h"
/*
 * namespace OnlineGameLib::Win32
 */

//namespace OnlineGameLib {
//namespace Win32 {
/*

CreateIoCompletionPort(
__in HANDLE FileHandle,
__in_opt HANDLE ExistingCompletionPort,
__in ULONG_PTR CompletionKey,
__in DWORD NumberOfConcurrentThreads //最大并发数 超  
)

*/
//创建关联一个已打开的文件实例和新建的或已存在的I/0完成端口  
CIOCompletionPort::CIOCompletionPort( size_t maxConcurrency )
		: m_iocp( ::CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, 0, maxConcurrency ) )
{
	if ( m_iocp == 0 )
	{
		//throw CWin32Exception( _T("CIOCompletionPort::CIOCompletionPort() - CreateIoCompletionPort"), ::GetLastError() );
		printf("--CIOCompletionPort::CIOCompletionPort() - CreateIoCompletionPort Error()--\n");
	}
}

BOOL CIOCompletionPort::CreateCompletionPort()
{
	SOCKET s;	

	//
	// First open a temporary socket that we will use to create the
	// completion port.  In NT 3.51 it will not be necessary to specify
	// the FileHandle parameter of CreateIoCompletionPort()--it will
	// be legal to specify FileHandle as NULL.  However, for NT 3.5
	// we need an overlapped file handle.
	//
	s = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if (s == INVALID_SOCKET) 
	{	
		return FALSE;
	}
	// Create the completion port that will be used by all the workers
	// threads.
	m_iocp = CreateIoCompletionPort((HANDLE)s,NULL,0,0);
	if (m_iocp == NULL) 
	{	
		closesocket(s);
		return FALSE;
	}
	closesocket(s);
	return TRUE;
}

CIOCompletionPort::~CIOCompletionPort() 
{
	SAFE_CLOSEHANDLE( m_iocp );
}

void CIOCompletionPort::AssociateDevice( HANDLE hDevice, ULONG_PTR completionKey )
{//创建完成端口机制
	if ( m_iocp != ::CreateIoCompletionPort( hDevice,
									m_iocp, 
									completionKey, 
									0 ) )
	{
		//throw CWin32Exception( _T("CIOCompletionPort::AssociateDevice() - CreateIoCompletionPort"), ::GetLastError() );
		printf("--CIOCompletionPort::AssociateDevice() - CreateIoCompletionPort--\n");
	}
}

void CIOCompletionPort::PostStatus( ULONG_PTR completionKey, 
			DWORD dwNumBytes /* = 0 */, 
			OVERLAPPED *pOverlapped /* = 0 */) 
{
	/*
	  向每个工作者线程都发送—个特殊的完成数据包。该函数会指示每个线程都“立即结束并退出”.
	*/
	if ( 0 == ::PostQueuedCompletionStatus( m_iocp, 
						dwNumBytes, 
						completionKey, 
						pOverlapped ) )
	{
		//throw CWin32Exception( _T("CIOCompletionPort::PostStatus() - PostQueuedCompletionStatus"), ::GetLastError() );
		printf("--CIOCompletionPort::PostStatus() - PostQueuedCompletionStatus--\n");
	}
}

DWORD CIOCompletionPort::GetStatus( ULONG_PTR *pCompletionKey, 
				PDWORD pdwNumBytes,
				OVERLAPPED **ppOverlapped )
{
	if ( 0 == ::GetQueuedCompletionStatus( m_iocp, 
						pdwNumBytes, 
						pCompletionKey, 
						ppOverlapped, 
						INFINITE ) )
	{
		return ::GetLastError();
	}

	return S_OK;
}

DWORD CIOCompletionPort::GetStatus( ULONG_PTR *pCompletionKey, 
				PDWORD pdwNumBytes,
				OVERLAPPED **ppOverlapped, 
				DWORD dwMilliseconds)
{
	if ( 0 == ::GetQueuedCompletionStatus( m_iocp, 
						pdwNumBytes, 
						pCompletionKey, 
						ppOverlapped, 
						dwMilliseconds ) )
	{
		DWORD lastError = ::GetLastError();

		if ( lastError != WAIT_TIMEOUT )
		{
			return lastError;
		}

		return S_FALSE;
	}

	return S_OK;
}

//} // End of namespace OnlineGameLib
//} // End of namespace Win32
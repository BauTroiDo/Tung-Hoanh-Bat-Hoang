#include "stdafx.h"
#include "SocketClient.h"
#include "Win32Exception.h"
#include "Utils.h"
#include "Socket.h"
#include "Macro.h"

#include <vector>

#pragma comment(lib, "ws2_32.lib")

/*
 * Link options and warning
 */
#pragma message( "NOTE : --------------------OnlineGameLib [Client] : Announcement--------------------" )
#pragma message( "NOTE : The lib be used for socket communicate that it is base on" )
#pragma message( "NOTE : Windows 95 and Windows NT 3.51 and later" )
#pragma message( "NOTE :" )
#pragma message( "NOTE : liupeng xishanju.zhuhai.china 2003.1" )
#pragma message( "NOTE : -----------------------------------------------------------------------------" )

using std::vector;

/*
 * namespace OnlineGameLib::Win32
 */

//namespace OnlineGameLib {
//namespace Win32 {

CSocketClient::CSocketClient(
		const _tstring &addressToConnectServer,
		unsigned short portToConnectServer,
		size_t maxFreeBuffers,
		size_t bufferSize /* = 1024 */
		)
	  : CIOBuffer::Allocator(bufferSize, maxFreeBuffers),
		m_address(addressToConnectServer),
		m_port(portToConnectServer),
		m_connectSocket(INVALID_SOCKET)
{

}

CSocketClient::CSocketClient(
		size_t maxFreeBuffers,
		size_t bufferSize /* = 1024 */
		)
	: CIOBuffer::Allocator(bufferSize, maxFreeBuffers),
	m_port(0),
	m_connectSocket(INVALID_SOCKET)
{

}

void CSocketClient::Connect(
			   const _tstring &addressToConnectServer,
			   unsigned short portToConnectServer
			   )
{
	m_address = addressToConnectServer;
	m_port = portToConnectServer;
}

CSocketClient::~CSocketClient()
{
	try
	{
		StopConnections();
	}
	catch(...)
	{	
		//TRACE( "CSocketClient::~CSocketClient() exception!" );
	}
}

bool CSocketClient::StartConnections()
{
	CCriticalSection::Owner lock( m_criticalSection );
    
	if (INVALID_SOCKET == m_connectSocket )
	{//如果是空链接的话 就创建一个链接
		m_connectSocket = CreateConnectionSocket(m_address, m_port);

		if (INVALID_SOCKET == m_connectSocket )
		{
			return false;
		}

		if (!WaitAndVerifyCipher())
		{
			return false;
		}

		m_eventSelect.AssociateEvent( m_connectSocket, FD_CONNECT | FD_CLOSE | FD_READ );

		m_successConnectionsEvent.Set();

		return true;
	}

	return false;
}

void CSocketClient::StopConnections()
{
	CCriticalSection::Owner lock( m_criticalSection );

	if ( INVALID_SOCKET != m_connectSocket)
	{
		LINGER lingerStruct;
		
		lingerStruct.l_onoff = 1;
		lingerStruct.l_linger = 0;
		
		if ( SOCKET_ERROR == ::setsockopt( m_connectSocket, 
								SOL_SOCKET, 
								SO_LINGER, 
								( char * )&lingerStruct, 
								sizeof( lingerStruct ) ) )
		{
		//	OnError( _T("CSocketClient::setsockopt( SO_LINGER ) - ") + GetLastErrorMessage( ::WSAGetLastError() ) );
		}

		m_successConnectionsEvent.Reset();

		m_eventSelect.DissociateEvent();
		
		if ( 0 != ::closesocket( m_connectSocket ) )
		{
			/*
			 * Call to unqualified virtual function
			 */
		//	OnError( _T("CSocketClient::StopAcceptingConnections() - closesocket - ") + GetLastErrorMessage( ::WSAGetLastError() ) );
		}
		
		m_connectSocket = INVALID_SOCKET;
		
		/*
		 * Call to unqualified virtual function
		 */
		OnStopConnections();	  //通知服务器已经断开链接
	}
}
//OnlineGameLib::Win32::
SOCKET CSocketClient::CreateConnectionSocket( 
					  const _tstring &addressToConnectServer,
					  unsigned short port)
{
	SOCKET s = ::WSASocket(AF_INET,SOCK_STREAM,IPPROTO_IP,NULL,0,0);
	
	if (INVALID_SOCKET == s)
		return s;
	
	//unsigned long ul=1;
 //   ioctlsocket(s,FIONBIO,(unsigned long *)&ul);//设置成非阻塞模式。


	CSocket connectionSocket( s );
	
	CSocket::InternetAddress localAddress( addressToConnectServer, port );

	connectionSocket.Connect( localAddress );
	
	return connectionSocket.Detatch();	
}

void CSocketClient::InitiateShutdown()
{
	m_shutdownEvent.Set();

	StopConnections();

	OnShutdownInitiated();
}

void CSocketClient::WaitForShutdownToComplete()
{
	InitiateShutdown();
	
	Wait();
}

int CSocketClient::Run()
{
	try
	{		
		HANDLE handlesToWaitFor[2];
		
		handlesToWaitFor[0] = m_shutdownEvent.GetEvent();
		handlesToWaitFor[1] = m_successConnectionsEvent.GetEvent();
		
		while (!m_shutdownEvent.Wait(0))
		{
			DWORD waitResult = ::WaitForMultipleObjects(2, handlesToWaitFor, false, INFINITE );
			
			if ( waitResult == WAIT_OBJECT_0 )
			{
				/*
				 * Time to shutdown
				 */
				break;
			}
			else if ( waitResult == WAIT_OBJECT_0 + 1 )
			{
				/*
				 * Allocate a buffer for required read
				 */
				CIOBuffer *pReadContext = Allocate();

				while (!m_shutdownEvent.Wait(0) && m_successConnectionsEvent.Wait(0))
				{
					if (m_eventSelect.WaitForEnumEvent(m_connectSocket,1000))
					{
						if (!m_eventSelect.Close())
						{//关闭时间 就退出线程

							StopConnections();
							//OnClose();						
						}
						if ( !m_eventSelect.Read() )
						{
							OnRead( pReadContext );
						}

						if ( !m_eventSelect.Connect() )
						{
							OnStartConnections();
							//OnConnect();						
						}
						//后来取消掉了的
						if (m_eventSelect.IsError())
						{//错误就退出线程
							m_shutdownEvent.Set();
							StopConnections();
							// Exit this thread
							break;
						}
					}

				} // while (...

				pReadContext->Release();
			}
			else
			{
				/*
				 * Call to unqualified virtual function
				 */
		//		OnError( _T("CSocketClient::Run() - WaitForMultipleObjects: ") + GetLastErrorMessage( ::GetLastError() ) );
			}
			
		} // while ( ... 		
	}
	catch(...)
	{
		/*
		 * Call to unqualified virtual function
		 */
		StopConnections();
		
		//OnError( _T("CSocketClient::Run() - Unexpected exception") );
	}
	
	/*
	 * Call to unqualified virtual function
	 */
	OnShutdownComplete();
	
	return 0;
}

void CSocketClient::ReleaseBuffers()
{
	Flush();
}

void CSocketClient::OnError( const _tstring &message )
{
	Output( message );
}

void CSocketClient::OnRead( CIOBuffer *pBuffer )
{
	/*
	 * Determine the amount of data that can be read atomically from socket s
	 *
	 * ::WSAIoctl( s, FIONREAD, ...
	 */

	DWORD dwNumBytes = 0;
	DWORD dwFlags = 0;
	
	pBuffer->SetupRead();
	
	if ( SOCKET_ERROR == ::WSARecv(
				m_connectSocket,
				pBuffer->GetWSABUF(), 
				1,
				&dwNumBytes, 
				&dwFlags,
				NULL,
				NULL ) )
	{
		DWORD lastError = ::WSAGetLastError();
		
		if ( ERROR_IO_PENDING != lastError )
		{
			if ( lastError == WSAECONNABORTED || 
				lastError == WSAECONNRESET ||
				lastError == WSAEDISCON)
			{
				StopConnections();
			}
		}
	}
	else
	{	
		pBuffer->Use( dwNumBytes );
		
		ReadCompleted( pBuffer );
	}
}

void CSocketClient::Write( const char *pData, size_t dataLength )
{
	if ( INVALID_SOCKET != m_connectSocket &&
		dataLength > 0 &&
		pData )
	{
		CIOBuffer *pBuffer = Allocate();
		
		pBuffer->AddData( pData, dataLength );

		Write( pBuffer );
		
		pBuffer->Release();
	}
}

static struct timeval gs_CheckRW_timeout = { 5, 0 };

////开始发包
//void CSocketClient::Write( CIOBuffer *pBuffer )
//{
//	size_t uDataLength = 0;
//	size_t ubakDataLength = 0;
//	if (NULL== pBuffer ||\
//		(0 == (uDataLength = pBuffer->GetUsed())) ||\
//		INVALID_SOCKET == m_connectSocket)
//	{
//		return;
//	}
//
//	ubakDataLength=uDataLength;
//	/*
//	 * Begin to send data
//	 */
//	int nError = 0;
//	DWORD lastError= 0;
//	
//	DWORD dwFlags = 0;//MSG_NOSIGNAL;
//	DWORD dwSendNumBytes = 0;
//	timeval gs_CheckRW_timeout = {0,0};
//	pBuffer->SetupWrite();
//	int  loopCount=0;
//	WSABUF  &wsa = *(pBuffer->GetWSABUF());
//		do
//		{   //循环检测 指到可以发送
//				wsa.len		-= dwSendNumBytes;
//				wsa.buf		+= dwSendNumBytes;//发送的子节
//				uDataLength-= dwSendNumBytes;
//				dwSendNumBytes =send(m_connectSocket,wsa.buf,wsa.len,dwFlags);
//			
//				if (dwSendNumBytes>0 && dwSendNumBytes >= uDataLength)  //uDataLength
//				{//发送完了,直接返回了
//					return;
//				}
//
//				if(SOCKET_ERROR == dwSendNumBytes)
//				{
//					if(_get_error_status() == SS_NOT_READY)
//					{
//						_wait(150*1000 , false);
//					}
//					else
//					{
//						return;
//					}
//				}
//
//		} while (true);
//	return;
//}

void CSocketClient::Write( CIOBuffer *pBuffer )
{
	size_t uDataLength = 0;

	if ( NULL == pBuffer ||
		( 0 == ( uDataLength = pBuffer->GetUsed() ) ) ||
		INVALID_SOCKET == m_connectSocket )
	{
		return;
	}

	int nError = 0;
	DWORD lastError= 0;

	DWORD dwFlags = 0;
	DWORD dwSendNumBytes = 0;

	pBuffer->SetupWrite();

	WSABUF &wsa = *( pBuffer->GetWSABUF() );

	do
	{
		if ( SOCKET_ERROR != nError )
		{
			wsa.len		-= dwSendNumBytes;
			wsa.buf		+= dwSendNumBytes;
			uDataLength	-= dwSendNumBytes;

			nError = ::WSASend(
				m_connectSocket,
				&wsa, 
				1, 
				&dwSendNumBytes,
				dwFlags,
				NULL, 
				NULL
				);

			if ( SOCKET_ERROR != nError && dwSendNumBytes >= uDataLength )
			{
				return;
			}
		}

		if ( SOCKET_ERROR != nError )
			continue;

		lastError = ::WSAGetLastError();

		if ( lastError == WSAECONNABORTED || 
			lastError == WSAECONNRESET ||
			lastError == WSAEDISCON )
		{
			StopConnections();
			return;
		}

		if ( ( ERROR_IO_PENDING != lastError ) && 
			( WSAEWOULDBLOCK != lastError) )
			return;

		fd_set writefds;
		memset( &writefds, 0, sizeof( writefds ) );

		writefds.fd_count = 1;
		writefds.fd_array[0] = m_connectSocket;

		do
		{
			nError = select( 1, NULL, &writefds, NULL, &gs_CheckRW_timeout );

			if ( SOCKET_ERROR == nError )
			{
				StopConnections();

				return;
			}
			if ( nError > 0 )
			{
				break;
			}

			if ( m_shutdownEvent.Wait( 0 ) )
			{
				return;
			}

		} while ( true );

	} while ( true );	
}

static const DWORD g_dwTimeout = 1000;

bool CSocketClient::WaitAndVerifyCipher()
{//等待超时时间
	fd_set	fdRead  = { 0 };

	TIMEVAL	stTime;
	TIMEVAL	*pstTime = NULL;

	if ( INFINITE != g_dwTimeout ) //
	{
		stTime.tv_sec = g_dwTimeout;
		stTime.tv_usec = 0;
		pstTime = &stTime;
	}
	
	/*
	 * Select function set read timeout
	 */

	SOCKET s = ( SOCKET )m_connectSocket;

	DWORD dwTotalLength = 0;

	while ( true )
	{
		DWORD dwBytesRead = 0L;

		/*
		 * Set Descriptor
		 */
		FD_ZERO(&fdRead);
		FD_SET(s,&fdRead );
		int res = select( 0, &fdRead, NULL, NULL, &stTime);//设置超时时间  pstTime
		if ( res > 0)
		{//接受服务器发来的响应
			res = recv( s, (LPSTR)&m_theSendAccountBegin + dwTotalLength, sizeof( m_theSendAccountBegin ) - dwTotalLength, 0 );
			if ( res > 0 )
			{
				dwBytesRead = res;
				dwTotalLength += dwBytesRead;			
			}
		}
		
		if ( res <= 0 )
		{
			/*
			 * Timeout and exit
			 */
			break;
		}
		
		if ( dwTotalLength == sizeof( m_theSendAccountBegin ) )
		{
			ACCOUNT_BEGIN *pAccountBegin = ( ACCOUNT_BEGIN * )( &m_theSendAccountBegin.AccountBegin );
			
			if ( pAccountBegin->ProtocolType == CIPHER_PROTOCOL_TYPE )
			{
				if ( pAccountBegin->nMode != 0 )
				{
				/*
				* Write this message and then shutdown the sending side of the socket.
					*/
		//			Output( "Server send cliper mode error!" );
					
					if ( 0 != ::closesocket( m_connectSocket ) )
					{
					/*
					* Call to unqualified virtual function
						*/
		//				OnError( _T("CSocketClient::StopAcceptingConnections() - closesocket - ") + GetLastErrorMessage( ::WSAGetLastError() ) );
					}
					
					m_connectSocket = INVALID_SOCKET;
					
					return false;
				}
				
				m_uKeyMode = pAccountBegin->nMode; 
				
				m_uServerKey = ~(pAccountBegin->ServerKey);
				m_uClientKey = ~(pAccountBegin->ClientKey);
				
				return true;
			}
		}
	}

	return false;
}

int CSocketClient::_wait(int ms_, bool read_)
{
	if (m_connectSocket == -1)
	{
		return 0;
	}
	fd_set selector;
	FD_ZERO(&selector);
	FD_SET(m_connectSocket, &selector);
	timeval time;
	time.tv_sec  = 0;
	time.tv_usec = ms_;
	if(read_) //接收
		return ::select(m_connectSocket+1, &selector, 0, 0, &time);
	else      //发送
		return ::select(m_connectSocket+1, 0, &selector, 0, &time);
}

e_socket_status CSocketClient::_get_error_status() const
{
#ifdef WIN32
	int xx = WSAGetLastError();
	switch (xx)
	{
	case WSAEWOULDBLOCK:
	case WSAENOBUFS:return SS_NOT_READY;      // 空闲
	case WSAECONNABORTED:
	case WSAECONNRESET:  
	case WSAETIMEDOUT: 
	case WSAENETRESET: 
	case WSAENOTCONN:return SS_DISCONNECTED;  //连接断开
	default:return SS_ERROR;                  //错误
	}
#else 
	if ((errno == EAGAIN) || (errno == EINPROGRESS))
		return SS_NOT_READY;
	switch (errno)
	{
	case EINPROGRESS:
	case EWOULDBLOCK:return SS_NOT_READY; 
	case ECONNABORTED:
	case ECONNRESET: 
	case ETIMEDOUT: 
	case ENETRESET: 
	case ENOTCONN:return SS_DISCONNECTED;
	default:return SS_ERROR;
	}
#endif
}

//} // End of namespace OnlineGameLib
//} // End of namespace Win32
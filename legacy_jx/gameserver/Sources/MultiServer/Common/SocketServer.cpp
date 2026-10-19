#include "stdafx.h"
#include "SocketServer.h"
#include "IOCompletionPort.h"
#include "Win32Exception.h"
#include "Utils.h"
#include "SystemInfo.h"
#include "Socket.h"
#include "Macro.h"
#include "..\heaven\serverstage.h"

#include <vector>

#pragma comment(lib, "ws2_32.lib")

/*
 * Link options and warning
 */
#pragma message( "NOTE : --------------------OnlineGame [Server] : Announcement--------------------" )
#pragma message( "NOTE : Developing a truly scalable Winsock Server using IO Completion Ports(IOCP)" )
#pragma message( "NOTE : But this module depend on the microsoft platform" )
#pragma message( "NOTE : Requirements :" )
#pragma message( "NOTE :    * Windows NT/2000/XP: Included in Windows NT 4.0 and later." )
#pragma message( "NOTE :    * Windows 95/98/Me: Unsupported." )
#pragma message( "NOTE :" )
#pragma message( "NOTE : liupeng xishanju.zhuhai.china 2003.1" )
#pragma message( "NOTE : -----------------------------------------------------------------------------" )

using std::vector;

/*
 * namespace OnlineGameLib::Win32
 */

//namespace OnlineGameLib {
//namespace Win32 {

/*
 * Static helper methods
 */
static size_t CalculateNumberOfThreads( size_t numThreads );

static size_t CalculateNumberOfThreads( size_t numThreads )
{
	if ( numThreads == 0 )
	{
		CSystemInfo systemInfo;
		
		numThreads = systemInfo.dwNumberOfProcessors * 2 + 2; //处理器数目
	}
	//printf("--SocketServer创建网络线程数量:%d --\n",numThreads);
	return numThreads;
}

/*
 * CSocketServer
 */
CSocketServer::CSocketServer(
		   size_t maxFreeSockets,
		   size_t maxFreeBuffers,
		   size_t bufferSize /* = 1024 */,
		   size_t numThreads /* = 0 */,
		   bool useSequenceNumbers /* = true */,
		   bool postZeroByteReads /* = false */)
	   :  CIOBuffer::Allocator( bufferSize, maxFreeBuffers ), //分配内存的大小,空闲列表数
		  m_numThreads(CalculateNumberOfThreads( numThreads ) ),//线程数量
		  m_listeningSocket( INVALID_SOCKET ),
		  m_iocp( 0 ),
		  m_maxFreeSockets( maxFreeSockets ),
		  m_useSequenceNumbers( useSequenceNumbers ),
		  m_postZeroByteReads( postZeroByteReads )
{
}

CSocketServer::~CSocketServer()
{
}

void CSocketServer::Open( unsigned long addressToListenOn, 
						unsigned short portToListenOn )
{
	m_address = addressToListenOn;
	m_port = portToListenOn;
}

void CSocketServer::ReleaseSockets()
{
	CCriticalSection::Owner lock( m_listManipulationSection );
	
	while ( m_activeSocketList.Head() )
	{//释放激活表 但不删除
		ReleaseSocket( m_activeSocketList.Head() );
		//printf("--释放m_activeList表Socket 但不删除 --\n");
	}
	
	while ( m_freeSocketList.Head() )
	{//删除空闲表
		DestroySocket( m_freeSocketList.PopNode() );
		//printf("--删除m_freeList表Socket --\n");
	}
	
	if ( m_activeSocketList.Count() + m_freeSocketList.Count() != 0 )
	{
		//OnError( _T("CSocketServer::ReleaseSockets() - Leaked sockets") );
		printf("--ReleaseSockets() - Leaked sockets-- \n");	  //泄露
	}
}




void CSocketServer::StartAcceptingConnections()
{
	if ( m_listeningSocket == INVALID_SOCKET )
	{//如果还没有 监听 套字节  就创建一个 
		OnStartAcceptingConnections();
		
		m_listeningSocket = CreateListeningSocket( m_address, m_port );
		
		m_acceptConnectionsEvent.Set();
	}
}

void CSocketServer::StopAcceptingConnections()
{
	if ( m_listeningSocket != INVALID_SOCKET )
	{
		m_acceptConnectionsEvent.Reset();
		
		if ( 0 != ::closesocket( m_listeningSocket ) )
		{
		//	OnError( _T("CSocketServer::StopAcceptingConnections() - closesocket - ") +
		//		GetLastErrorMessage( ::WSAGetLastError() ) );
			//printf("----CSocketServer::StopAcceptingConnections() - closesocke----\n");
		}
		
		m_listeningSocket = INVALID_SOCKET;
		
		OnStopAcceptingConnections();
	}
}

void CSocketServer::InitiateShutdown()
{
	StopAcceptingConnections();

   /*
    * enter m_listManipulationSection
	*/
   {
	   CCriticalSection::Owner lock( m_listManipulationSection );
	   
	   Socket *pSocket = m_activeSocketList.Head();
	   
	   while ( pSocket )
	   {
		   Socket *pNext = SocketList::Next( pSocket );
		   
		   pSocket->AbortiveClose();
		   
		   pSocket = pNext;
	   }
   }
   /*
    * leave m_listManipulationSection
	*/

   /*
    * signal that the dispatch thread should shut down all worker threads and then exit
	*/
   m_shutdownEvent.Set();
   
   OnShutdownInitiated();
}

void CSocketServer::WaitForShutdownToComplete()
{
	/*
	 * If we havent already started a shut down, do so...
	 */
	InitiateShutdown();
	
	Wait();
	
	ReleaseSockets();
	Flush();
}
//创建监听 socket
SOCKET CSocketServer::CreateListeningSocket( unsigned long address, unsigned short port )
{
	SOCKET s = ::WSASocket( AF_INET, 
							SOCK_STREAM, 
							IPPROTO_IP, 
							NULL, 
							0, 
							WSA_FLAG_OVERLAPPED /* for IOCP */ );
	
	if ( s == INVALID_SOCKET )
	{
		//throw CWin32Exception( _T("CSocket::CreateListeningSocket()"), ::WSAGetLastError());
		printf("----CSocket::CreateListeningSocket() s == INVALID_SOCKET----\n");
		return s;
	}
	
	CSocket listeningSocket(s);
	
	CSocket::InternetAddress localAddress(address, port);
	
	listeningSocket.Bind(localAddress);	 //绑定地址
	
	listeningSocket.Listen(5);		     //5秒超时
	
	return listeningSocket.Detatch();
}

CSocketServer::WorkerThread *CSocketServer::CreateWorkerThread(
					CIOCompletionPort &iocp )
{
	return new WorkerThread( *this, iocp );
}

int CSocketServer::Run()
{
	try
	{
	    vector< WorkerThread * > workers;
		workers.reserve(m_numThreads);
		size_t i;
		for (i = 0; i < m_numThreads; ++i)
		{
			WorkerThread *pThread = CreateWorkerThread(m_iocp); //创建一个线程　就有一个完成端口？
			workers.push_back(pThread);      //在容器最后位置添加一个元素 初始化一个实例对象
			pThread->Start();                //开始创建线程一个玩家对应一个线程
			//printf("----线程:%u 开始----\n",i);
		}
	    
		HANDLE handlesToWaitFor[2];
		
		handlesToWaitFor[0] = m_shutdownEvent.GetEvent();
		handlesToWaitFor[1] = m_acceptConnectionsEvent.GetEvent();
		
		while (!m_shutdownEvent.Wait(0))
		{
			DWORD waitResult = ::WaitForMultipleObjects( 2,
							handlesToWaitFor, 
							false, 
							INFINITE );
			
			if (waitResult == WAIT_OBJECT_0)
			{
				/*
				 * Time to shutdown
				 */
				break;
			}
			else if (waitResult == WAIT_OBJECT_0 + 1)
			{
				/*
				 * accept connections
				 */	
				//printf("----线程测试D----\n");
				while (!m_shutdownEvent.Wait(0) && m_acceptConnectionsEvent.Wait(0))
				{
					CIOBuffer *pAddress = Allocate();
					
					int addressSize = (int)pAddress->GetSize();
					//这个是　接受　监听端口　接入的　客户端的套字节
					SOCKET acceptedSocket = ::WSAAccept(
								m_listeningSocket, 
								reinterpret_cast< sockaddr * >( const_cast< BYTE * >( pAddress->GetBuffer() ) ), 
								&addressSize, 
								0, 
								0);
					
					pAddress->Use(addressSize);
					
					if (acceptedSocket != INVALID_SOCKET)
					{//有效的　套字节
						Socket *pSocket = AllocateSocket(acceptedSocket);
						OnConnectionEstablished(pSocket, pAddress);//完成端口操作
						//printf("----OnConnectionEstablished 线程测试C----\n");
						pSocket->Release();//释放了这个套字节
					}
					else if (m_acceptConnectionsEvent.Wait(0))
					{
		//				OnError( _T("CSocketServer::Run() - WSAAccept:") + 
		//					GetLastErrorMessage( ::WSAGetLastError() ) );
						//printf("----线程测试A----\n");
					}
					
					pAddress->Release();
				}
			}
			else
			{
		//		OnError( _T("CSocketServer::Run() - WaitForMultipleObjects: ") +
		//			GetLastErrorMessage( ::GetLastError() ) );
				//printf("----线程测试B----\n");
			}
		}
		
		for ( i = 0; i < m_numThreads; ++i )
		{  
			printf("----线程:%zu 结束----\n",i);
			workers[i]->InitiateShutdown();
		}  
		
		for ( i = 0; i < m_numThreads; ++i )
		{
			workers[i]->WaitForShutdownToComplete();
			
			delete workers[i];			
			workers[i] = 0;

			printf("-----线程结束----\n");
		} 
	}
	catch(...)
	{
		//OnError( _T("CSocketServer::Run() - Unexpected exception") );
		//printf("--CSocketServer::Run() - Unexpected exception---\n");
	}
	
	OnShutdownComplete();
	
	return 0;
}

CSocketServer::Socket *CSocketServer::AllocateSocket( SOCKET theSocket )
{
	CCriticalSection::Owner lock( m_listManipulationSection );
	
	Socket *pSocket = 0;
	
	if (!m_freeSocketList.Empty())
	{//已经存在这个链接的 直接用
		pSocket = m_freeSocketList.PopNode();
		if (pSocket)
		{
		   pSocket->Attach(theSocket);
		   pSocket->AddRef();
		   printf("----利用原始的:Socket----\n");
		}
	}
	else
	{//否则新建一个
		pSocket = new Socket(*this, theSocket, m_useSequenceNumbers);
		OnConnectionCreated();
		//printf("----创建一个新的B:Socket----\n");
		//::MessageBox(NULL,"创建Socket成功","Info",MB_ICONEXCLAMATION);
	}
	//激活 或 正在用的 增加
	m_activeSocketList.PushNode(pSocket);
	
	m_iocp.AssociateDevice( reinterpret_cast<HANDLE>(theSocket),(ULONG_PTR)pSocket);
	
	return pSocket;
}

void CSocketServer::ReleaseSocket( Socket *pSocket )
{
	if (!pSocket)
	{
		//throw CException( _T("CSocketServer::ReleaseSocket()"),	_T("pSocket is null") );
		printf("----CSocketServer::ReleaseSocket() pSocket is null----\n");
		return;
	}

	CCriticalSection::Owner lock( m_listManipulationSection );
	
	pSocket->SetMessageCallback(NULL);
	pSocket->RemoveFromList();
	
	if ( m_maxFreeSockets == 0 || m_freeSocketList.Count() < m_maxFreeSockets )
	{
		m_freeSocketList.PushNode(pSocket);	//空闲链接列表
		printf("----m_freeSocketList PushNode:Socket----\n");
	}
	else
	{//否则移除
		DestroySocket(pSocket);	        //移除链接
		//printf("----DestroySocket:Socket----\n");
	}
}

void CSocketServer::DestroySocket( Socket *pSocket )
{
	delete pSocket;
    pSocket=NULL;
	OnConnectionDestroyed();
}

void CSocketServer::OnError( const _tstring &message )
{
	//DEBUG_ONLY( Output( message ) );
	printf("--OnError:%s--\n",message.c_str());
}
  
void CSocketServer::WriteCompleted( Socket * pSocket, CIOBuffer *pBuffer )
{
	if ( pBuffer->GetUsed() != pBuffer->GetWSABUF()->len )
	{
		//OnError( _T("CSocketServer::WorkerThread::WriteCompleted - Socket write where not all data was written") );
		printf("--CSocketServer::WorkerThread::WriteCompleted - Socket write where not all data was written--\n");
	}
}

void CSocketServer::SetServerDataPtr( Socket *pSocket, void *pData )
{
	pSocket->SetServerDataPtr( pData );
}

void *CSocketServer::GetServerDataPtr( const Socket *pSocket )
{
	return pSocket->GetServerDataPtr();
}

void CSocketServer::WriteCompleted( Socket *pSocket )
{
	pSocket->WriteCompleted();
}

bool CSocketServer::FilterSocketShutdown( Socket * /*pSocket*/, int /*how*/ )
{
	return true;
}

bool CSocketServer::FilterSocketClose( Socket * /*pSocket*/ )
{
	return true;
}
//通知完成端口
void CSocketServer::PostIoOperation( Socket *pSocket,
				CIOBuffer *pBuffer,
				enumIO_Operation operation)
{
	pBuffer->SetOperation( operation );
	pBuffer->AddRef();

	pSocket->AddRef();

	m_iocp.PostStatus( ( ULONG_PTR )pSocket, 0, pBuffer );
	//printf("--通知完成端口--\n");
	//::MessageBox(NULL,"通知完成端口","Info",MB_ICONEXCLAMATION);

}

CSocketServer &CSocketServer::GetServer( Socket *pSocket )
{
	return pSocket->m_server;
}

void CSocketServer::OnConnectionError( enumConnectionErrorSource /*source*/,
			Socket * /*pSocket*/,
			CIOBuffer * /*pBuffer*/,
			DWORD lastError)
{
	_tstring sErrorInfo = GetLastErrorMessage( lastError );
	//DEBUG_ONLY( Output( _T("CSocketServer::OnConnectionError() - : ") + sErrorInfo ) );
	//DEBUG_ONLY( Message( sErrorInfo.c_str() ) );
	printf("--Error:%s --\n",sErrorInfo.c_str());
}

/*
 * CSocketServer::Socket
 */
CSocketServer::Socket::Socket( CSocketServer &server,                                 
		SOCKET theSocket,
		bool useSequenceNumbers )
	: m_server( server ),
	 m_socket( theSocket ),
	 m_ref( 1 ),
	 m_outstandingWrites( 0 ),
	 m_readShutdown( false ),
	 m_writeShutdown( false ),
	 m_closing( false ),
	 m_clientClosed( false ),
	 m_pSequenceData( 0 ),
	 m_pIM (0 )
{
	if (!IsValid())
	{
		//throw CException( _T("CSocketServer::Socket::Socket()"), _T("Invalid socket") );
		printf("--CSocketServer::Socket::Socket() Invalid socket---\n");
		return;
	}
	
	if (useSequenceNumbers)
	{
		m_pSequenceData = new SequenceData( m_crit );
	}
}

CSocketServer::Socket::~Socket()
{
	try
	{
		delete m_pSequenceData;
		m_pSequenceData=NULL;
	}
	catch(...)
	{
	//	TRACE( "CSocketServer::Socket::~Socket exception!" );
	}
}
//重新附加这个　theSocket
void CSocketServer::Socket::Attach( SOCKET theSocket )
{
	if (IsValid())
	{
        printf("--CSocketServer::Socket::Attach() Socket already attached---\n");
		return;
		//throw CException( _T("CSocketServer::Socket::Attach()"), _T("Socket already attached"));
	}
	
	m_socket = theSocket;
	
	SetUserData(0);
	
	m_readShutdown = false;
	m_writeShutdown = false;
	m_outstandingWrites = 0;
	m_closing = false;
	m_clientClosed = false;
	
	if (m_pSequenceData)
	{
		m_pSequenceData->Reset();
	}
}

void CSocketServer::Socket::AddRef()
{
	::InterlockedIncrement(&m_ref);
}

void CSocketServer::Socket::Release()
{
	if ( 0 == ::InterlockedDecrement( &m_ref ) )
	{
		if ( IsValid() )
		{
			AddRef();
			
			if ( !m_closing )
			{
				m_closing = true;
				
				if (!m_server.OnConnectionClosing(this))
				{
					AbortiveClose();
				}
			}
			else
			{
				AbortiveClose();
			}
			
			Release();
			return;
		}
		
		m_server.ReleaseSocket(this);
	}
}

void CSocketServer::Socket::Shutdown( int how /* = SD_BOTH */ )
{
	//DEBUG_ONLY( Output( _T("CSocketServer::Socket::Shutdown() ") + ToString( how ) ) );
	//printf("--CSocketServer::Server::Shutdown(A)---\n");
	//::MessageBox(NULL,"Shutdown:Socket","Info",MB_ICONEXCLAMATION);
	if ( m_server.FilterSocketShutdown( this, how ) )
	{
		if ( how == SD_RECEIVE || how == SD_BOTH )
		{
			m_readShutdown = true;
		}
		
		if ( how == SD_SEND || how == SD_BOTH )
		{
			m_writeShutdown = true;
		}
		
		if ( ::InterlockedExchange( &m_outstandingWrites, m_outstandingWrites ) > 0 )
		{
			/*
			 * Send side will be shut down when last pending write completes...
			 */			
			if ( how == SD_BOTH )
			{
				how = SD_RECEIVE;      
			}
			else if ( how == SD_SEND )
			{
				return;
			}
		}

		//Close();
		
		if ( IsValid() )
		{
			if ( 0 != ::shutdown(m_socket, how))
			{
				m_server.OnError( _T("CSocketServer::Server::Shutdown() - ") +
		                          GetLastErrorMessage( ::WSAGetLastError() ) );
			}

			if (m_server.FilterSocketClose(this))
				InternalClose();
		}
	}
}

bool CSocketServer::Socket::IsConnected( int how /*= SD_BOTH*/ ) const
{
	if ( how == SD_RECEIVE )
	{
		return !m_readShutdown;
	}
	
	if ( how == SD_SEND )
	{
		return !m_writeShutdown;
	}
	
	if ( how == SD_BOTH )
	{
		return ( !m_writeShutdown && !m_readShutdown );
	}
	
	return false;
}

void CSocketServer::Socket::Close()
{
	CCriticalSection::Owner lock( m_crit );
	
	if (IsValid())
	{
		if (m_server.FilterSocketClose(this))
		{
		//	printf("--CSocketServer::Socket::Close() ---\n");
			InternalClose();
		}
	}
}

bool CSocketServer::Socket::WritePending()
{
	if (m_writeShutdown)
	{
	//	DEBUG_ONLY( Output( _T("CSocketServer::Socket::WritePending() - Attempt to write after write shutdown") ) );
		printf("--CSocketServer::Socket::WritePending() - Attempt to write after write shutdown---\n");
		return false;
	}
	
	::InterlockedIncrement( &m_outstandingWrites );
	
	return true;
}

void CSocketServer::Socket::WriteCompleted()
{
	if ( ::InterlockedDecrement( &m_outstandingWrites ) == 0 )
	{
		if ( m_writeShutdown )
		{
			/*
			 * The final pending write has been completed so we can now shutdown
			 * the send side of the connection.			 
			 */
			Shutdown(SD_SEND);
		}
	}
}

void CSocketServer::Socket::AbortiveClose()
{
	/*
	 * Force an abortive close.
	 */	
	LINGER lingerStruct;
	
	lingerStruct.l_onoff = 1;
	lingerStruct.l_linger = 0;
	
	if ( SOCKET_ERROR == ::setsockopt( m_socket, 
							SOL_SOCKET, 
							SO_LINGER, 
							( char * )&lingerStruct, 
							sizeof( lingerStruct ) ) )
	{
		m_server.OnError( _T("CSocketServer::Socket::AbortiveClose() - setsockopt( SO_LINGER ) - ") + 
					GetLastErrorMessage(::WSAGetLastError() ) );
		//printf("--CSocketServer::Socket::AbortiveClose() - setsockopt( SO_LINGER )---\n");
	}
	printf("--CSocketServer::Socket::AbortiveClose() - setsockopt(SO_LINGER)---\n");
	InternalClose();
}

void CSocketServer::Socket::OnClientClose()
{
	if ( 0 == ::InterlockedExchange( &m_clientClosed, 1 ) )
	{
		Shutdown( SD_RECEIVE );
		
		m_server.OnConnectionClientClose( this );
	}
}

void CSocketServer::Socket::OnConnectionReset()
{
	CCriticalSection::Owner lock( m_crit );
	
	if ( IsValid() )
	{
		m_server.OnConnectionReset( this );
		//printf("--CSocketServer::Socket::OnConnectionReset() ---\n");
		InternalClose();
	}
}
//关闭客户端连接
void CSocketServer::Socket::InternalClose()
{
	CCriticalSection::Owner lock( m_crit );
	
	if (IsValid())
	{
		if (0 != ::closesocket(m_socket))
		{
			m_server.OnError( _T("CSocketServer::Socket::InternalClose() - closesocket - ") + 
					GetLastErrorMessage( ::WSAGetLastError() ) );
			//printf("--CSocketServer::Socket::InternalClose() - closesocket ---\n");
		}
		//printf("--InternalClose 关闭客户端连接成功 ---\n");
		//::MessageBox(NULL,"关闭客户端连接成功","Info",MB_ICONEXCLAMATION);
		m_socket = INVALID_SOCKET;
		
		m_readShutdown = true;
		m_writeShutdown = true;
		
		m_server.OnConnectionClosed( this );
	}
}

bool CSocketServer::Socket::Read( CIOBuffer *pBuffer /* = 0 */, bool throwOnFailure /* = false*/ )
{
	if (!IsValid())
	{
		if (throwOnFailure)
		{
			//throw CException( _T("CSocketServer::Socket::Read()"), _T("Socket is closed") );
			printf("--CSocketServer::Socket::Read() Socket is closed--\n");
			return false;
		}
		else
		{
			return false;
		}
	}

	/*
	 * Post a read request to the iocp so that the actual socket read gets performed by
	 * one of the server's IO threads...
	 */
	
	if ( !pBuffer )
	{
		pBuffer = m_server.Allocate();
	}
	else
	{
		pBuffer->AddRef();
	}
	
	m_server.PostIoOperation(this, pBuffer, m_server.m_postZeroByteReads ? enumIO_Zero_Byte_Read_Request : enumIO_Read_Request );
	
	pBuffer->Release();
	
	return true;
}

bool CSocketServer::Socket::Write( const char *pData, size_t dataLength, bool throwOnFailure /* = false*/ )
{
	return Write( reinterpret_cast< const BYTE * >( pData ), dataLength, throwOnFailure );
}

bool CSocketServer::Socket::Write( const BYTE *pData, size_t dataLength, bool throwOnFailure /* = false*/ )
{
	if (!IsValid())
	{
		if (throwOnFailure)
		{
			/*
			 * Todo throw SocketClosedException();
			 */
			//throw CException( _T("CSocketServer::Socket::Write()"), _T("Socket is closed") );
			printf("--CSocketServer::Socket::Write() Socket is closed--\n");
			return false;
		}
		else
		{
			return false;
		}
	}
	
	if ( !WritePending() )
	{
		if ( throwOnFailure )
		{
			/*
			 * Todo throw SocketClosedException();
			 */
			//throw CException(_T("CSocketServer::Socket::Write()"), _T("Socket is shutdown"));
			printf("--CSocketServer::Socket::Write() Socket is shutdown--\n");
			return false;
		}
		else
		{
			return false;
		}
	}
	
	CIOBuffer *pBuffer = m_server.Allocate();
	
	pBuffer->AddData( pData, dataLength );
	
	pBuffer->SetSequenceNumber( GetSequenceNumber( enumWriteSequenceNo ) );
	
	m_server.PostIoOperation( this, pBuffer, enumIO_Write_Request );
	
	pBuffer->Release();
	
	return true;
}

bool CSocketServer::Socket::Write( CIOBuffer *pBuffer, bool throwOnFailure /* = false*/ )
{
	if ( !IsValid() )
	{
		if ( throwOnFailure )
		{
			//throw CException( _T("CSocketServer::Socket::Write()"), _T("Socket is closed") );
			//printf("--CSocketServer::Socket::Write() Socket is closed--\n");
			return false;
		}
		else
		{
			return false;
		}
	}
	
	if ( !WritePending() )
	{
		if ( throwOnFailure )
		{
			//throw CException( _T("CSocketServer::Socket::Write()"), _T("Socket is shutdown") );
			//printf("--CSocketServer::Socket::Write() Socket is shutdown--\n");
			return false;
		}
		else
		{
			return false;
		}
	}
	
	pBuffer->SetSequenceNumber( GetSequenceNumber( enumWriteSequenceNo ) );
	
	m_server.PostIoOperation( this, pBuffer, enumIO_Write_Request );
	
	return true;
}

long CSocketServer::Socket::GetSequenceNumber( enumSequenceType type )
{
	if ( m_pSequenceData )
	{
		return m_pSequenceData->m_numbers[type]++;
	}
	
	return 0;
}

CIOBuffer *CSocketServer::Socket::GetNextBuffer( CIOBuffer *pBuffer /* = 0 */ )
{
	if ( m_pSequenceData )
	{
		if ( pBuffer )
		{
			return m_pSequenceData->m_outOfSequenceWrites.GetNext( pBuffer );
		}
		else
		{
			return m_pSequenceData->m_outOfSequenceWrites.ProcessAndGetNext();
		}
	}
	
	return pBuffer;
}

bool CSocketServer::Socket::IsValid()
{
	CCriticalSection::Owner lock(m_crit);
	
	return (INVALID_SOCKET != m_socket);
}

void CSocketServer::Socket::OnConnectionError( CSocketServer::enumConnectionErrorSource source,
				   CIOBuffer *pBuffer,
				   DWORD lastError)
{
	if ( WSAESHUTDOWN == lastError )
	{
		OnClientClose();
	}
	else if ( WSAECONNRESET == lastError || WSAECONNABORTED == lastError )
	{//多gs 确定这里 会断开连接
		OnConnectionReset();
	}
	else if ( !IsValid() && WSAENOTSOCK == lastError )
	{
	}
	else
	{
		m_server.OnConnectionError( source, this, pBuffer, lastError );
	}
}

/*
 * CSocketServer::Socket::SequenceData
 */

CSocketServer::Socket::SequenceData::SequenceData( CCriticalSection &section )
				: m_outOfSequenceWrites( section )
{
	memset( m_numbers, 0, sizeof( m_numbers ) );
}

void CSocketServer::Socket::SequenceData::Reset()
{
   memset( m_numbers, 0, sizeof( m_numbers ) );

   m_outOfSequenceWrites.Reset();
}


/*
 * CSocketServer::WorkerThread
 */

CSocketServer::WorkerThread::WorkerThread( CSocketServer &server, CIOCompletionPort &iocp )
				: m_server( server ),
				  m_iocp( iocp )
{
	m_server.OnThreadCreated();
}

CSocketServer::WorkerThread::~WorkerThread()
{
	try
	{
		m_server.OnThreadDestroyed();
	}
	catch(...)
	{
	//	TRACE( "CSocketServer::WorkerThread::~WorkerThread exception!" );
	}
}

int CSocketServer::WorkerThread::Run()
{
	try
	{
		while (true)
		{
			/*
			 * continually loop to service io completion packets
			 */
			DWORD		dwIoSize = 0;
			Socket		*pSocket = 0;
			CIOBuffer	*pBuffer = 0;
			
			bool weClosedSocket = false;
			
			DWORD dwResult = m_iocp.GetStatus((PDWORD_PTR)&pSocket,&dwIoSize,(OVERLAPPED **)&pBuffer);

			if (S_OK != dwResult)
			{				
				if ( ERROR_NETNAME_DELETED == dwResult )
				{
					weClosedSocket = true;
					//printf("----IOCP: - weClosedSocket----\n");
				}
				else if ( WSA_OPERATION_ABORTED != dwResult )
				{
					//throw CWin32Exception( _T("CIOCompletionPort::GetStatus() - GetQueuedCompletionStatus"), dwResult );
					//printf("----CIOCompletionPort::GetStatus() - GetQueuedCompletionStatus----\n");
				}
			
	//			DEBUG_ONLY( Output( _T("IOCP error - client connection dropped") ) );
				//printf("----IOCP error - client connection dropped----\n");
			}
			
			if (!pSocket)
			{
				/*
				 * A completion key of 0 is posted to the iocp to request us to shut down...
				 */
				//printf("----IOCP error - pSocket =NULL ----\n");
				break;
			}
			
			/*
			 * Thread Begin Processing
			 */
			
			HandleOperation( pSocket, pBuffer, dwIoSize, weClosedSocket );
			
			/*
			 * Thread End Processing
			 */

		}// while ( true )
	}
	catch(...)
	{
	//	m_server.OnError( _T("CSocketServer::WorkerThread::Run() - Unexpected exception") );
		printf("----CSocketServer::WorkerThread::Run() - Unexpected exception----\n");
	}
	
	return 0;
}

void CSocketServer::WorkerThread::InitiateShutdown()
{
	m_iocp.PostStatus(0);
}

void CSocketServer::WorkerThread::WaitForShutdownToComplete()
{
	/*
     * if we havent already started a shut down, do so...
	 */

	InitiateShutdown();

	Wait();
}

void CSocketServer::WorkerThread::HandleOperation( Socket *pSocket,
					   CIOBuffer *pBuffer,
					   DWORD dwIoSize,
					   bool weClosedSocket )
{
	if ( pBuffer )
	{
		const enumIO_Operation operation = static_cast< enumIO_Operation >( pBuffer->GetOperation() );
		
		switch ( operation )
		{
		case enumIO_Zero_Byte_Read_Request:
			
      //      DEBUG_ONLY( Output( _T("ZeroByteRead:") + ToString( pBuffer ) ) );
			//printf("----enumIO_Zero_Byte_Read_Request----\n");
            ZeroByteRead( pSocket, pBuffer );
			
            pSocket->Release();
            pBuffer->Release();
			
			break ;
			
		case enumIO_Zero_Byte_Read_Completed :
		case enumIO_Read_Request :
			
        //    DEBUG_ONLY( Output( _T("enumIO_Zero_Byte_Read_Completed | enumIO_Read_Request:") + ToString( pBuffer ) ) );
			//printf("----enumIO_Read_Request----\n");
			Read(pSocket, pBuffer);
            
            pSocket->Release();
            pBuffer->Release();
			
			break;
			
		case enumIO_Read_Completed :
			
        //    DEBUG_ONLY( Output( _T("enumIO_Read_Completed:") + ToString( pBuffer ) ) );
			//printf("----enumIO_Read_Completed----\n");
            pBuffer->Use( dwIoSize );            
           
			if ( 0 != dwIoSize )
			{
				m_server.ReadCompleted( pSocket, pBuffer );
			}
			
            if ( 0 == dwIoSize && !weClosedSocket )
            {//客户端连接丢失
				/*
				 * client connection dropped?
				 */
				
		//		DEBUG_ONLY( Output( _T("ReadCompleted - 0 bytes - client connection dropped") ) );
				printf("----ReadCompleted - 0 bytes - client connection dropped----\n");
				pSocket->OnClientClose();
            }
			
            pSocket->Release();
            pBuffer->Release();
			
			break;
			
		case enumIO_Write_Request :
			
        //    DEBUG_ONLY( Output( _T("enumIO_Write_Request:") +
		//				ToString( pBuffer ) ) );
		    //printf("----enumIO_Write_Request----\n");
			Write(pSocket,pBuffer);
			
            pSocket->Release();
            pBuffer->Release();
			
			break;
			
		case enumIO_Write_Completed :
			
          //  DEBUG_ONLY( Output( _T("enumIO_Write_Completed:") + ToString( pBuffer ) ) );
			//printf("----enumIO_Write_Completed----\n");
            pBuffer->Use(dwIoSize);
			
			//m_server.WriteCompleted(pSocket, pBuffer);
				
			pSocket->WriteCompleted();
			
            pSocket->Release();
            pBuffer->Release();
			
			break;
			
		default :
            //m_server.OnError( _T("CSocketServer::WorkerThread::Run() - Unexpected operation") );
			printf("----CSocketServer::WorkerThread::Run() - Unexpected operation----\n");
			break;
      } 
   }
   else
   {
	   //m_server.OnError( _T("CSocketServer::WorkerThread::Run() - Unexpected - pBuffer is 0") );
	   printf("----CSocketServer::WorkerThread::Run() - Unexpected - pBuffer is 0----\n");
   }
}

void CSocketServer::WorkerThread::ZeroByteRead( Socket *pSocket, CIOBuffer *pBuffer ) const
{
	pSocket->AddRef();
	
	pBuffer->SetOperation( enumIO_Zero_Byte_Read_Completed );
	pBuffer->SetupZeroByteRead();
	pBuffer->AddRef();
	
	DWORD dwNumBytes = 0;
	DWORD dwFlags = 0;
	
	if ( SOCKET_ERROR == ::WSARecv(
				pSocket->m_socket, 
				pBuffer->GetWSABUF(), 
				1, 
				&dwNumBytes,
				&dwFlags,
				pBuffer, 
				NULL ) )
	{
			DWORD lastError = ::WSAGetLastError();

		if ( ERROR_IO_PENDING != lastError )
		{
		pSocket->OnConnectionError( enumZeroByteReadError, pBuffer, lastError );

		pSocket->Release();
		pBuffer->Release();
		}
	}
}

void CSocketServer::WorkerThread::Read( Socket *pSocket, CIOBuffer *pBuffer ) const
{
	pSocket->AddRef();
	
	pBuffer->SetOperation( enumIO_Read_Completed );
	pBuffer->SetupRead();
	pBuffer->AddRef();
	
	CCriticalSection::Owner lock( pSocket->m_crit );
	
	pBuffer->SetSequenceNumber( pSocket->GetSequenceNumber( Socket::enumReadSequenceNo ) );
	
	DWORD dwNumBytes = 0;
	DWORD dwFlags = 0;
	
	if ( SOCKET_ERROR == ::WSARecv(
				pSocket->m_socket, 
				pBuffer->GetWSABUF(), 
				1, 
				&dwNumBytes,
				&dwFlags,
				pBuffer, 
				NULL ) )
	{
		DWORD lastError = ::WSAGetLastError();

		if ( ERROR_IO_PENDING != lastError )
		{
			pSocket->OnConnectionError( enumReadError, pBuffer, lastError );

			pSocket->Release();
			pBuffer->Release();
		}
	}
}

void CSocketServer::WorkerThread::Write( Socket *pSocket, CIOBuffer *pBuffer ) const
{
	pSocket->AddRef();
	
	{
		CCriticalSection::Owner lock( pSocket->m_critWriteQueue );
		
		pBuffer->SetOperation( enumIO_Write_Completed );
		pBuffer->SetupWrite();
		pBuffer->AddRef();
		
		pBuffer = pSocket->GetNextBuffer( pBuffer );

//		DWORD dwID = ::GetCurrentThreadId();
//		printf( "Begin ID:%d - Socket:%d\n", dwID, ( DWORD )pSocket );
//
//		::Sleep( 10 );
		
		while ( pBuffer )
		{	
			DWORD dwFlags = 0;
			DWORD dwSendNumBytes = 0;

//			::Sleep( 10 );
			
			if ( SOCKET_ERROR == ::WSASend(
				pSocket->m_socket,
				pBuffer->GetWSABUF(), 
				1, 
				&dwSendNumBytes,
				dwFlags,
				pBuffer, 
				NULL ) )
			{
				DWORD lastError = ::WSAGetLastError();
				
				if ( ERROR_IO_PENDING != lastError )
				{
					pSocket->OnConnectionError( enumWriteError, pBuffer, lastError );
					
					/*
					* this pending write will never complete...
					*/
					pSocket->WriteCompleted();
					
					pSocket->Release();
					pBuffer->Release();
				}
			}
			
			pBuffer = pSocket->GetNextBuffer();
		}

//		printf( "End ID:%d - Socket:%d\n", dwID, ( DWORD )pSocket );
	}
}

//} // End of namespace OnlineGameLib
//} // End of namespace Win32

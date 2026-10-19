#include "ServerStage.h" 
#include <time.h>
#include <process.h>		//Thread define

#include "KSG_EncodeDecode.h"

#include "Cipher.h"
#include "Macro.h"
#include "Exception.h"
#include "SocketAddress.h"
//#include "Utils.h"
#include "Win32Exception.h"
//
/*using CIOBuffer;
using CWin32Exception;

using _tstring;
using CCriticalSection;
using CException;
using Output;
using DumpData;
using GetTimeStamp;
using ToString;
using Trace;
using CSocketAddress;*/

/*
 * Get server factory interface
 */
STDAPI CreateInterface(REFIID	riid, void	**ppv)
{
	HRESULT hr = E_NOINTERFACE;

	if (IID_IServerFactory == riid)
	{
		CServerFactory *pObject = new CServerFactory;

		*ppv = reinterpret_cast<void *> (dynamic_cast<IServerFactory *>(pObject));

		if (*ppv)
		{
			reinterpret_cast<IUnknown *>(*ppv)->AddRef();

			hr = S_OK;
		}
	}

	if (FAILED(hr))
	{
		*ppv = NULL;
	}

	return (HRESULT)(hr);
}

/*
 * Local per connection data
 */
class CPerConnectionData
{
public:
      
	explicit CPerConnectionData( const _tstring &address, WPARAM wParam, const LPARAM &lParam )
         : m_address( address ), m_wParam( wParam ), m_lParam( lParam )
	{
	}

	void SetConnectionFirstParam( WPARAM wParam )
	{
		m_wParam = wParam;
	}

	WPARAM GetConnectionFirstParam() const
	{
		return m_wParam;
	}

	LPARAM GetConnectionSecondParam() const
	{
		return m_lParam;
	}

	_tstring GetConnectionDetails() const
	{
         return m_address;
	}

private:

	const _tstring	m_address;	
	const LPARAM	m_lParam;

	WPARAM			m_wParam;

	/*
	 * No copies, do not implement
	 */
	CPerConnectionData( const CPerConnectionData &rhs );
	CPerConnectionData &operator=( const CPerConnectionData &rhs );

};

CIOCPServer::CTaskQueue::~CTaskQueue()
{
	CCriticalSection::Owner locker( m_cs );

	while ( !m_theTaskStack.empty() )
	{
		m_theTaskStack.pop();
	}		
}

void CIOCPServer::CTaskQueue::Push( void *pItem )
{
	CCriticalSection::Owner locker( m_cs );

	m_theTaskStack.push( pItem );
}

void *CIOCPServer::CTaskQueue::Pop()
{
	CCriticalSection::Owner locker( m_cs );

	void *pItem = NULL;
	
	if ( !m_theTaskStack.empty() )
	{
		pItem = m_theTaskStack.top();
		m_theTaskStack.pop();
	}

	return pItem;
}

static unsigned  gs_holdrand = time(NULL);

static inline unsigned  _Rand()
{
    gs_holdrand = gs_holdrand * 214013L + 2531011L;
     
    return gs_holdrand;
}

static inline void RandMemSet(int nSize, unsigned char *pbyBuffer)
{
    _ASSERT(nSize);
    _ASSERT(pbyBuffer);

    while (nSize--)
    {
        *pbyBuffer++ = (unsigned char)_Rand();
    }
}
//---------------------------------------------
CIOCPServer::CIOCPServer( size_t nPlayerMaxCount,
				size_t nPrecision,
				size_t maxFreeSockets,
				size_t maxFreeBuffers,
				size_t maxFreeBuffers_Cache,
				size_t bufferSize_Cache /*= 8192*/,
				size_t bufferSize /*= 1024*/,
				size_t numThreads /*= 0*/ )
		: CSocketServer(maxFreeSockets, maxFreeBuffers, bufferSize, numThreads)
		, m_theCacheAllocator(bufferSize_Cache, maxFreeBuffers_Cache) //分配内存的大小,空闲表数
		, m_lRefCount(0)
		, m_nPlayerMaxCount(nPlayerMaxCount) //
		, m_nPrecision(nPrecision)
		, m_nNetworkBufferMaxLen(bufferSize - 32/* sizeof( protocol of network ) */)
		, m_pfnCallBackServerEvent(NULL)
		, m_hQuitHelper(NULL,true,false,NULL/*"EVENT_IOCPSVR_QUIT"*/)
		, m_hHelperThread(NULL)
{  
	/*
	 * m_nNetworkBufferMaxLen > 0
	 */
	ASSERT(!(m_nNetworkBufferMaxLen & 0x80000000));

	size_t index = 0;

	for ( index = 0; index < nPlayerMaxCount; ++index) // nPlayerMaxCount
	{
		{
			CCriticalSection::Owner locker(m_csFCN);
			
			//m_freeClientNode.push( index );
			m_freeClientNode.push(index);
		}

		/*
		 * Create a client node and add it into list
		 */
		LPCLIENT_NODE pCN = new CLIENT_NODE;

		pCN->pSocket = NULL;
		pCN->pRecvBuffer	= m_theCacheAllocator.Allocate();
		pCN->pReadBuffer	= m_theCacheAllocator.Allocate();
		pCN->pWriteBuffer	= m_theCacheAllocator.Allocate();
		
		{
			CCriticalSection::Owner	locker( m_csCM );
			
			m_theClientManager[index] = pCN;	 //0--nPlayerMaxCount-1分配多少个客户端连接的内存
		}
	}

	unsigned int threadID = 0;
	
	m_hHelperThread = (HANDLE)::_beginthreadex( 0, 
								0, 
								HelperThreadFunction,
								( void * )this, 
								0, 
								&threadID );
	
	if ( m_hHelperThread == INVALID_HANDLE_VALUE )
	{
		throw CWin32Exception( _T("CIOCPServer::CIOCPServer() - _beginthreadex"), GetLastError() );
	}
}

CIOCPServer::~CIOCPServer()
{
	
	/*
     * Repeat until stack is empty
	 */
	{
		CCriticalSection::Owner locker( m_csFCN );
		
		while ( !m_freeClientNode.empty() )
		{//全部移除 
			m_freeClientNode.pop();
		}

/*		while ( !m_freeClientNode.empty() )
		{
			m_freeClientNode.pop();
		}
*/
	}

	{
		CCriticalSection::Owner	locker( m_csCM );
		//已经使用的全部移除
		m_usedClientNode.erase( m_usedClientNode.begin(), m_usedClientNode.end() );
		
		CLIENT_MANAGER::iterator it;
		for ( it = m_theClientManager.begin(); it != m_theClientManager.end(); ++it )
		{//全部释放内存
			LPCLIENT_NODE pCN = ( LPCLIENT_NODE )( ( *it ).second );
			
			if ( pCN )
			{
				pCN->pRecvBuffer->Release();
				pCN->pReadBuffer->Release();
				
				pCN->pWriteBuffer->Release();

			}
			
			SAFE_DELETE( pCN );
		}
		///移除容器的节点
		m_theClientManager.erase( m_theClientManager.begin(), m_theClientManager.end() );
	}

	m_hQuitHelper.Set();

	if ( WAIT_TIMEOUT == ::WaitForSingleObject( m_hHelperThread, 5000 ) )
	{
		::TerminateThread( m_hHelperThread, 0L );
	}
}

STDMETHODIMP CIOCPServer::Startup( )
{
	HRESULT hr = E_FAIL;

	try
	{
		Start();

		hr = S_OK;
	}
	catch(...)
	{
		//Output( _T("CIOCPServer::Startup - Unexpected exception") );
	}
	
	return hr;	
}
//删除网络连接
STDMETHODIMP CIOCPServer::Cleanup( )
{
	HRESULT hr = E_FAIL;

	try
	{		
		StopAcceptingConnections();
		
		WaitForShutdownToComplete();

		hr = S_OK;
	}
	catch(...)
	{
		//Output(_T("CIOCPServer::Cleanup - Unexpected exception"));
	}

	return hr;
}

STDMETHODIMP CIOCPServer::OpenService( const unsigned long &ulnAddressToListenOn,
			const unsigned short &usnPortToListenOn )
{
	HRESULT hr = E_FAIL;

	try
	{
		Open( ulnAddressToListenOn,usnPortToListenOn);
		
		StartAcceptingConnections();//创建监听 Socket

		hr = S_OK;
	}
	/*catch( const CException &e )
	{
		Output( _T("CIOCPServer::OpenService Exception: ") + e.GetWhere() + _T(" - ") + e.GetMessage() );
	} */
	catch(...)
	{
		//Output( _T("CIOCPServer::OpenService - Unexpected exception") );
	}	

	return hr;
}

STDMETHODIMP CIOCPServer::CloseService()
{
	HRESULT hr = E_FAIL;

	try
	{
	    
		StopAcceptingConnections();

		hr = S_OK;
	}
	/*catch( const CException &e )
	{
		Output( _T("CIOCPServer::CloseService Exception: ") + e.GetWhere() + _T(" - ") + e.GetMessage() );
	} */
	catch(...)
	{
		//Output( _T("CIOCPServer::CloseService - Unexpected exception") );
	}	

	return hr;
}
//注册服务器通知函数
STDMETHODIMP CIOCPServer::RegisterMsgFilter( LPVOID lpParam, 
		CALLBACK_SERVER_EVENT pfnEventNotify )
{
	m_lpCallBackParam = lpParam;
	m_pfnCallBackServerEvent = pfnEventNotify;

	return S_OK;
}
//通知做好发包的准备
STDMETHODIMP CIOCPServer::PreparePackSink()
{
	return S_OK;
}
//发包给客户端
STDMETHODIMP CIOCPServer::PackDataToClient( const unsigned long &ulnClientID,
			const void * const	pData,
			const size_t		&datalength)
{
	HRESULT hr = E_FAIL;

	if (ulnClientID>=0 && ulnClientID < m_nPlayerMaxCount && datalength > 0 )  //m_nPlayerMaxCount
	{
		LPCLIENT_NODE pCN = NULL;

		CCriticalSection::Owner	locker( m_csCM );

		pCN = ( LPCLIENT_NODE )( m_theClientManager[ulnClientID] );

		if ( pCN && pCN->pSocket)
		{
			try
			{
				CCriticalSection::Owner	lock( pCN->csWriteAction );

				CIOBuffer *pWriteBuffer = pCN->pWriteBuffer;

				const size_t nNetworkBufferLen = pWriteBuffer->GetUsed();
				//pWriteBuffer->AddData( static_cast< const char * const >( pData ), datalength );
				if ( ( nNetworkBufferLen >= m_nNetworkBufferMaxLen ) || 
					( nNetworkBufferLen + datalength >= m_nNetworkBufferMaxLen ) )
				{
					/*
					 * It need send to client at once, because of buffer is full
					 */
					const BYTE *pPackData = pWriteBuffer->GetBuffer();
					const size_t used = pWriteBuffer->GetUsed();

					_SendDataEx(pCN,pPackData,used);

					pWriteBuffer->Empty();
				}

				pWriteBuffer->AddData( static_cast< const char * const >( pData ), datalength );
				hr = S_OK;
				//Output( _T("CClientManager::PackDataToClient - 发包成功B") );
			}
			catch(...)
			{
				//Output( _T("CIOCPServer::PackDataToClient - Unexpected exception") );

				pCN->pSocket->Shutdown();
			}			
		}
	}

	return hr;
}

STDMETHODIMP CIOCPServer::SendPackToClient( const unsigned long &ulnClientID /* -1 */ )
{//向所有已经连接的客户端发包
	HRESULT hr = E_FAIL;

	static const size_t s_len_protocol = sizeof( WORD );

	CCriticalSection::Owner locker( m_csCM );

	LIST::iterator it;
	for ( it = m_usedClientNode.begin(); it != m_usedClientNode.end(); ++it )
	{
		size_t index = ( size_t )( *it );


		if  (ulnClientID>=0)
		{
			if  (index!=ulnClientID)
				continue;
		}

		LPCLIENT_NODE pCN = NULL;

		pCN = ( LPCLIENT_NODE )( m_theClientManager[index] );

		if ( pCN && pCN->pSocket )
		{
			CIOBuffer *pBuffer = NULL;
			try
			{
				_ASSERT(pCN->uKeyMode == 0);

				pBuffer = Allocate();
				
				/*
				* Add data into buffer that it is will be send
				*/
				bool ok = false;

				{
					CCriticalSection::Owner lock( pCN->csWriteAction );
					
					const BYTE * pPackData = pCN->pWriteBuffer->GetBuffer();
					const size_t used = pCN->pWriteBuffer->GetUsed();
					
					if ( used > 0 )
					{
						/*
						 * Add package header
						 */
						const size_t headlength = s_len_protocol + used;
						pBuffer->AddData( reinterpret_cast< const char * >( &headlength ), s_len_protocol );



						//KSG_EncodeBuf(used,(unsigned char *)pPackData,&pCN->uServerKey);
						pBuffer->AddData( reinterpret_cast< const char * >( pPackData ), used );
						
		                //_ASSERT(pCN->uKeyMode == 0);
						//
						KSG_EncodeBuf(used,(unsigned char *)(pBuffer->GetBuffer() + s_len_protocol),&pCN->uServerKey);
						ok = true;
					}
					
					pCN->pWriteBuffer->Empty();
				}
				
				if ( ok )
				{
					pCN->pSocket->Write( pBuffer, true );
				}
				
				pBuffer->Release();			
				
				hr = S_OK;
			}
			catch(...)
			{
				// add by spe 2003/07/06
				if (pBuffer)
				{
					pBuffer->Release();	
					pBuffer = NULL;
				}				
				pCN->pSocket->Shutdown();  //关闭链接
			}
		}
	}
    return hr;
}

STDMETHODIMP CIOCPServer::SendData( const unsigned long &ulnClientID,
					 const void * const	pData,
					 const size_t		&datalength )
{//向某个客户端发包
	HRESULT hr = E_FAIL;

	static const size_t s_len_protocol = sizeof( WORD );

	if ( NULL == pData || 0 == datalength )
	{
		return hr;
	}

	if (ulnClientID>=0 && ulnClientID < m_nPlayerMaxCount ) //m_nPlayerMaxCount
	{
		LPCLIENT_NODE pCN = NULL;
		
		CCriticalSection::Owner	locker(m_csCM);

		pCN = (LPCLIENT_NODE)(m_theClientManager[ulnClientID]);

		if (pCN && pCN->pSocket)
		{
			CIOBuffer *pBuffer = NULL;
			try
			{
			    _ASSERT(pCN->uKeyMode == 0);
				pBuffer = Allocate();
				
				/*
				 * Add package header
				 */
				const size_t headlength = s_len_protocol + datalength;
				pBuffer->AddData( reinterpret_cast< const char * >( &headlength ), s_len_protocol );
				
				/*
				 * Add data into buffer that it is will be send
				 */
				/*KSG_EncodeBuf(
					datalength,
					(unsigned char*)pData,
					&pCN->uServerKey
					);*/

				pBuffer->AddData( reinterpret_cast< const char * >( pData ), datalength );

              
				KSG_EncodeBuf(
					datalength,
					(unsigned char *)(pBuffer->GetBuffer() + s_len_protocol),
                    &pCN->uServerKey
				);

				pCN->pSocket->Write(pBuffer,true);
				
				pBuffer->Release();			
				
				hr = S_OK;
			}
		/*	catch(const CException &e )
			{
				// add by spe 2003/07/06
				if (pBuffer)
				{
					pBuffer->Release();	
					pBuffer = NULL;
				}

				Output( _T("CIOCPServer::SendPackToClient Exception: ") + e.GetWhere() + _T(" - ") + e.GetMessage() );
				
				pCN->pSocket->Shutdown();	//关闭链接
			} */
			catch(...)
			{
				// add by spe 2003/07/06
				if (pBuffer)
				{
					pBuffer->Release();	
					pBuffer = NULL;
				}

				//Output( _T("CIOCPServer::SendPackToClient - Unexpected exception") );
				
				pCN->pSocket->Shutdown();	  //关闭链接
			}
		}
	}

	return hr;
}

bool CIOCPServer::_SendDataEx( LPCLIENT_NODE pNode,
				 const void * const	pData,
				 const size_t		&datalength )
{
	static const size_t s_len_protocol = sizeof( WORD );

	bool ok = false;

	if ( !pNode || !pData || !datalength )
	{
		return false;
	}
	
	if ( pNode->pSocket )
	{
		CIOBuffer *pBuffer = NULL;
		try
		{
			_ASSERT(pNode->uKeyMode == 0);

			pBuffer = Allocate();
			
			/*
			* Add package header
			*/
			const size_t headlength = s_len_protocol + datalength;
			pBuffer->AddData( reinterpret_cast< const char * >( &headlength ), s_len_protocol );
			
			/*
			* Add data into buffer that it is will be send
			*/
			/*KSG_EncodeBuf(
				datalength,
				(unsigned char*)pData,
				&pNode->uServerKey
				);*/

			pBuffer->AddData( reinterpret_cast< const char * >( pData ), datalength );
			
			KSG_EncodeBuf(
				datalength,
				(unsigned char *)(pBuffer->GetBuffer() + s_len_protocol),
				&pNode->uServerKey
				);
			
			pNode->pSocket->Write( pBuffer, true );
			
			pBuffer->Release();			
			
			ok = true;
		}
		catch(...)
		{
			// add by spe 2003/07/06
			if (pBuffer)
			{
				pBuffer->Release();	
				pBuffer = NULL;
			}

			//Output( _T("CIOCPServer::_SendDataEx - Unexpected exception") );
			
			pNode->pSocket->Shutdown();
		}
	}

	return ok;
}

STDMETHODIMP_( const void * ) CIOCPServer::GetPackFromClient( const unsigned long &ulnClientID,
			size_t &datalength )
{
	if ( ulnClientID>=0 && ulnClientID < m_nPlayerMaxCount ) //m_nPlayerMaxCount
	{
		LPCLIENT_NODE pCN = NULL;

		CCriticalSection::Owner	locker( m_csCM );

		pCN = ( LPCLIENT_NODE )( m_theClientManager[ulnClientID] );

		if ( pCN && pCN->pSocket )
		{
			CCriticalSection::Owner lock( pCN->csReadAction );

			pCN->pReadBuffer->Empty();

			pCN->pRecvBuffer = ProcessDataStream( pCN->pSocket, pCN->pRecvBuffer );

			const char *pData = reinterpret_cast< const char * >( pCN->pReadBuffer->GetBuffer() );
			datalength = pCN->pReadBuffer->GetUsed();

			return pData;
		}
	}

	datalength = 0;
	
	return NULL;
}
//关闭客户端
STDMETHODIMP CIOCPServer::ShutdownClient( const unsigned long &ulnClientID )
{//E_FAIL
	if (ulnClientID>=0 && ulnClientID < m_nPlayerMaxCount) //m_nPlayerMaxCount
	{
		LPCLIENT_NODE pCN = NULL;

		CCriticalSection::Owner	locker( m_csCM );
			
		pCN = ( LPCLIENT_NODE )( m_theClientManager[ulnClientID] );

		if ( pCN )
		{
			if ( pCN->pSocket )
			{
			   // pCN->pSocket->AbortiveClose();      //网关版
			   //pCN->pSocket->Release();             //GS版
				pCN->pSocket->Shutdown();
				pCN->pSocket = NULL;
			}

			pCN->pReadBuffer->Empty();
			pCN->pRecvBuffer->Empty();
			pCN->pWriteBuffer->Empty();
			return S_OK;
		}
		else
			return E_FAIL;
	}
	else
		return E_FAIL;
	
}
//获取客户端链接数量
STDMETHODIMP_( size_t ) CIOCPServer::GetClientCount( )
{
//	CCriticalSection::Owner locker( m_csCM );
	return m_usedClientNode.size();
}

STDMETHODIMP_( void ) CIOCPServer::GetClientInfo(
					const unsigned long &ulnClientID ,char * nstrInfo, size_t nInfoSize)
{

	if ( ulnClientID < m_nPlayerMaxCount )  //m_nPlayerMaxCount	  400
	{
		LPCLIENT_NODE pCN = NULL;

		CCriticalSection::Owner	locker( m_csCM );

		pCN = ( LPCLIENT_NODE )(m_theClientManager[ulnClientID] );

		if ( pCN && pCN->pSocket )
		{
			const CPerConnectionData *pData = reinterpret_cast< const CPerConnectionData * >( pCN->pSocket->GetUserPtr() );
			
			if ( pData )
			{
			    //printf("--SocketServer创建网络链接:%s --\n",pData->GetConnectionDetails().c_str());
			   //char nMsg[128]={0};
		       //sprintf_s(nstrInfo,sizeof(nstrInfo)-1,"%d:%s",ulnClientID,pData->GetConnectionDetails().c_str());
			   snprintf(nstrInfo, nInfoSize, "%s", pData->GetConnectionDetails().c_str());
		       //::MessageBox(NULL,nstrInfo, "Warning", MB_OK | MB_ICONSTOP );
				//return __TEXT(pData->GetConnectionDetails().c_str());
				//return __TEXT(nMsg);
			}
		}
	}

	//return NULL;
}

STDMETHODIMP CIOCPServer::QueryInterface( REFIID riid, void** ppv )
{
	/*
	 * By definition all COM objects support the IUnknown interface
	 */
	if( riid == IID_IUnknown )
	{
		AddRef();

		*ppv = dynamic_cast< IUnknown * >( this );
	}
	else if ( riid == IID_IIOCPServer )
	{
		AddRef();

		*ppv = dynamic_cast< IServer * >( this );	
	}
	else
	{
		*ppv = NULL;

		return E_NOINTERFACE;
	}

	return S_OK;
}
    
STDMETHODIMP_(ULONG) CIOCPServer::AddRef()
{
	return ::InterlockedIncrement( &m_lRefCount );
}

STDMETHODIMP_(ULONG) CIOCPServer::Release()
{
	if ( ::InterlockedDecrement( &m_lRefCount ) > 0 )
	{
		return m_lRefCount;
	}

	delete this;
	return 0L;
}

void CIOCPServer::OnConnectionEstablished( Socket *pSocket,
			 CIOBuffer *pAddress )
{
	if ( pSocket )
	{
		pSocket->AddRef();

		/*
		 * Allocate per connection data
		 */
		CSocketAddress address( reinterpret_cast< const sockaddr * >( pAddress->GetBuffer() ) );
		
		CPerConnectionData *pData = new CPerConnectionData( address.GetAsString(), ( WPARAM )( -1 ), ( LPARAM )( pSocket ) );
		
		ASSERT( pData );
		
		m_theAddClientQueue.Push( ( void * )pData );
		//printf("----m_theAddClientQueue：%s ----\n",address.GetAsString().c_str());
	
	}
}

bool CIOCPServer::OnConnectionClosing( Socket *pSocket )
{
	if ( pSocket )
	{
		//printf("----OnConnectionClosing ----\n");
		pSocket->Close();
	}

	return true;
}

void CIOCPServer::OnConnectionClosed( Socket *pSocket )
{	
	if ( pSocket )
	{
		pSocket->AddRef();

		/*
		 * Release per connection data
		 */
		CPerConnectionData *pData = reinterpret_cast< CPerConnectionData * >( pSocket->GetUserPtr() );

		m_theDelClientQueue.Push( pData );
	}
}

void CIOCPServer::ReadCompleted( Socket *pSocket, CIOBuffer *pBuffer )
{
	if (pSocket->GetMessageCallback())
	{
		ProcessMessage(pSocket, pBuffer);
		return;
	}

	try
	{
		const CPerConnectionData *pData = reinterpret_cast< const CPerConnectionData * >( pSocket->GetUserPtr() );

		if ( pData )
		{
			DWORD dwIndex = pData->GetConnectionFirstParam();
			
			const BYTE *pPackData = pBuffer->GetBuffer();
			const size_t used = pBuffer->GetUsed();
			
			LPCLIENT_NODE pCN = NULL;
			{
				CCriticalSection::Owner	locker( m_csCM );
				
				pCN = ( LPCLIENT_NODE )( m_theClientManager[dwIndex] );
				
				if ( pCN )
				{
					CCriticalSection::Owner lock( pCN->csReadAction );
					
					pCN->pRecvBuffer->AddData( pPackData, used );
				}
			}
		}
		
		pBuffer->Empty();
		
		pSocket->Read( pBuffer );
	}
	catch(...)
	{
		//Output( _T("ReadCompleted - Unexpected exception") );

		pSocket->Shutdown();
	}
}

CIOBuffer *CIOCPServer::ProcessDataStream( Socket *pSocket, CIOBuffer *pBuffer)
{
	const size_t used = pBuffer->GetUsed();
	
	if (used >= GetMinimumMessageSize() )
	{
		const size_t messageSize = GetMessageSize(pBuffer);
		
		if ( messageSize == 0 )
		{
			/*
			 * havent got a complete message yet.		
			 * we null terminate our messages in the buffer, so we need to reserve
			 * a byte of the buffer for this purpose...
			 */
			
			if (used == (pBuffer->GetSize() - 1))
			{
				//Output( _T("Too much data! found error and close this socket!") );
				
				/*
				 * Shutdown the sending side of the socket.
				 */
				pSocket->Shutdown();
				
				/*
				 * throw the rubbish away
				 */
				pBuffer->Empty();
			}
		}
		else if (used == messageSize)
		{
			//Output( _T("Got a complete message and begin to process it") );
			
			/*
			* we have a whole, distinct, message
			*/				
			ProcessCommand( pSocket, pBuffer );
			
			pBuffer->Empty();
		}
		else if (used > messageSize)
		{
			//Output( _T("Got message plus extra data") );
			
			/*
			* we have a message, plus some more data
			* allocate a new buffer, copy the extra data into it and try again
			*/
			
			CIOBuffer *pMessage = pBuffer->SplitBuffer( messageSize );
			
			ProcessCommand( pSocket, pMessage );
			
			pMessage->Release();			
		}
	}
	
	/*
	 * not enough data in the buffer, reissue a read into the same buffer to collect more data
	 */
	return pBuffer;
}

size_t CIOCPServer::GetMinimumMessageSize()
{
	static size_t length = sizeof( WORD ) + sizeof( BYTE );

	/*
     * The smallest possible command we accept is a byte onlye package
	 */
	return length;
}

size_t CIOCPServer::GetMessageSize( const CIOBuffer *pBuffer )
{
	const BYTE *pData = pBuffer->GetBuffer();   
	const size_t used = pBuffer->GetUsed();

	WORD wHeadLen = ( WORD )( *( WORD * )( pData ) );

	return ( size_t )( wHeadLen );
}

void CIOCPServer::ProcessCommand(Socket *pSocket,const CIOBuffer *pBuffer)
{
	static const size_t s_len_protocol = sizeof( WORD );

	const BYTE *pData = pBuffer->GetBuffer();   
	const size_t used = pBuffer->GetUsed();

	ASSERT( used > s_len_protocol );

	if ( used <= s_len_protocol )
	{
        return;
	}

	const CPerConnectionData *pPCD = reinterpret_cast< const CPerConnectionData * >( pSocket->GetUserPtr() );

	if ( !pPCD )
	{
		return;
	}

	DWORD dwIndex = pPCD->GetConnectionFirstParam();
	LPCLIENT_NODE pCN = NULL;
	{
		CCriticalSection::Owner	locker( m_csCM );

		if ( NULL == ( pCN = ( LPCLIENT_NODE )( m_theClientManager[dwIndex] ) ) )
		{
            return;
		}
		
        // Add by Freeway Chen
        _ASSERT(pCN->uKeyMode == 0);

        KSG_DecodeBuf(used - s_len_protocol,const_cast<unsigned char *>(pData + s_len_protocol),&(pCN->uClientKey));

		pCN->pReadBuffer->AddData( ( const BYTE * )( pData + s_len_protocol ), used - s_len_protocol );
	}
}

CServerFactory::CServerFactory()
		: m_lRefCount( 0 ),
		m_nPlayerMaxCount( 0 ),
		m_nPrecision( 0 ),
		m_maxFreeBuffers_Cache( 0 ),
		m_bufferSize_Cache( 0 ),
		m_maxNumThreads(0)
{

}

CServerFactory::~CServerFactory()
{

}
//设置基本数据环境
STDMETHODIMP CServerFactory::SetEnvironment( const size_t &nPlayerMaxCount,
			const size_t &nPrecision,
			const size_t &maxFreeBuffers_Cache,	   //1024
			const size_t &bufferSize_Cache,        //
			const size_t &maxNumThreads)	       //1024*64
{
	m_nPlayerMaxCount      = nPlayerMaxCount;	   //200
	m_nPrecision           = nPrecision;		   //10

	m_maxFreeBuffers_Cache = maxFreeBuffers_Cache;     //10
	m_bufferSize_Cache     = bufferSize_Cache;	       //16*1024
	m_maxNumThreads        = maxNumThreads;            //0
	return S_OK;
}

//创建基本数据环境
STDMETHODIMP CServerFactory::CreateServerInterface(REFIID riid, void** ppv)
{
	HRESULT hr = E_NOINTERFACE;

	if (IID_IIOCPServer == riid)
	{		
		const size_t maxPlayerCount = (m_nPlayerMaxCount > 0) ? m_nPlayerMaxCount : 100;
		const size_t precision = (m_nPrecision >0) ? m_nPrecision : 10;

		const size_t maxFreeBuffers_Cache = (m_maxFreeBuffers_Cache > 0) ? m_maxFreeBuffers_Cache : 10;
		const size_t maxFreeBuffers       = 10240;//(m_maxFreeBuffers_Cache > 0) ? m_maxFreeBuffers_Cache : 10;	//10240;    

		const size_t bufferSize_Cache     = (m_bufferSize_Cache > 0) ? m_bufferSize_Cache : 8192;			 //64*1024
		const size_t bufferSize           = 1024;     //决定发包的大小

		try
		{//分配内存
			/*
size_t nPlayerMaxCount,
size_t nPrecision,
size_t maxFreeSockets,
size_t maxFreeBuffers,
size_t maxFreeBuffers_Cache,
size_t bufferSize_Cache , //8192=1024*8
size_t bufferSize ,	      //1024
size_t numThreads)
: CSocketServer(maxFreeSockets, maxFreeBuffers, bufferSize, numThreads)
, m_theCacheAllocator(bufferSize_Cache, maxFreeBuffers_Cache)

*/

			CIOCPServer *pObject = new CIOCPServer(maxPlayerCount,//200 
											precision, 			  //10
											maxPlayerCount,       //200      空闲链接数,
										    maxFreeBuffers,		  //10240    最大的空闲列表数
											maxFreeBuffers_Cache, //10	     最大的空闲列表数
											bufferSize_Cache,	  //16*1024	 分配内存的大小
											bufferSize,           //1024     Socket发包分配内存的大小
											m_maxNumThreads);	  //0	     线程数量
			
			*ppv = reinterpret_cast<void *> (dynamic_cast<IServer *>(pObject));
			
			if (*ppv)
			{
				reinterpret_cast< IUnknown * >(*ppv)->AddRef();
				
				hr = S_OK;
			}
		}
		catch(...)
		{
			//TRACE("CServerFactory::CreateServerInterface exception!");
		}
	}

	if (FAILED(hr))
	{
		*ppv = NULL;
	}

	return (HRESULT)(hr);
}

STDMETHODIMP CServerFactory::QueryInterface( REFIID riid, void** ppv )
{
	/*
	 * By definition all COM objects support the IUnknown interface
	 */
	if( riid == IID_IUnknown )
	{
		AddRef();

		*ppv = dynamic_cast< IUnknown * >( this );
	}
	else if ( riid == IID_IServerFactory )
	{
		AddRef();

		*ppv = dynamic_cast< IServerFactory * >( this );	
	}
	else
	{
		*ppv = NULL;

		return E_NOINTERFACE;
	}

	return S_OK;
}
    
STDMETHODIMP_(ULONG) CServerFactory::AddRef()
{
	return ::InterlockedIncrement( &m_lRefCount );
}

STDMETHODIMP_(ULONG) CServerFactory::Release()
{
	if ( ::InterlockedDecrement( &m_lRefCount ) > 0 )
	{
		return m_lRefCount;
	}

	delete this;
	return 0L;
}
//---------------------------------------------------------------------
unsigned int __stdcall CIOCPServer::HelperThreadFunction( void *pParam )
{
	CIOCPServer *pThis = ( CIOCPServer * )( pParam );

	ASSERT( pThis );

	try
	{
		pThis->_HelperThreadFunction();
	}
	catch( ... )
	{
		//TRACE( "CIOCPServer::HelperThreadFunction exception!" );
	}

	return 0L;
}
//活动线程 
unsigned int CIOCPServer::_HelperThreadFunction()
{
	bool bAdd, bDel;

	while ( !m_hQuitHelper.Wait( 0 ) )
	{
		bAdd = _HelperAddClient();

		bDel = _HelperDelClient();

		if ( !bAdd && !bDel )
		{
			::Sleep( 1 );
		}
	}

	return 0L;
}
//增加网络线程
bool CIOCPServer::_HelperAddClient()
{
   	static const size_t s_len_protocol = sizeof( WORD );

	CPerConnectionData *pData = ( CPerConnectionData * )( m_theAddClientQueue.Pop() );

	if ( NULL == pData )
	{
		return false;
	}

	Socket *pSocket_ = ( Socket * )( pData->GetConnectionSecondParam() );

	ASSERT( pSocket_ );
	
	USES_CONVERSION;

	DWORD dwIndex = (DWORD)( -1 );

	if ( pSocket_ )
	{
		/*
		 * Get ID from the free table
		 */
		{
			CCriticalSection::Owner locker( m_csFCN );

			if ( !m_freeClientNode.empty() )
			{
				dwIndex = m_freeClientNode.front();	  //访问队首元素   .back()访问队尾元素
				m_freeClientNode.pop();               //删除一个
				//dwIndex = m_freeClientNode.top();
				//m_freeClientNode.pop();
			}
		}

		pData->SetConnectionFirstParam( dwIndex );

		pSocket_->SetUserPtr( pData );

		/*
		 * Store this socket if we can use it
		 */
		if ( dwIndex != (DWORD)( -1 ) )
		{
			/*
			 * Store socket into array
			 */
			{
				CCriticalSection::Owner	locker( m_csCM );
				
				m_theClientManager[dwIndex]->pSocket    = pSocket_;
                m_theClientManager[dwIndex]->uServerKey = _Rand();
                m_theClientManager[dwIndex]->uClientKey = _Rand();
                m_theClientManager[dwIndex]->uKeyMode   = 0;
				printf("--Ep vao nut mang client(%u):%u,%u --\n",dwIndex,m_theClientManager[dwIndex]->uServerKey,m_theClientManager[dwIndex]->uClientKey);
				/*
				 * Set ID into the used table
				 */
				m_usedClientNode.push_back( dwIndex );	  //压入一个已经使用的客户端节点
			}

            #pragma pack(1)
            struct  {
                WORD wLen;
                ACCOUNT_BEGIN AccountBegin;
            } SendAccountBegin;
            #pragma pack()

            RandMemSet(sizeof(SendAccountBegin), (unsigned char *)&SendAccountBegin);

			{
				CCriticalSection::Owner	locker( m_csCM );

                SendAccountBegin.wLen = s_len_protocol + sizeof(SendAccountBegin.AccountBegin);

                SendAccountBegin.AccountBegin.ProtocolType = CIPHER_PROTOCOL_TYPE;
                SendAccountBegin.AccountBegin.nMode        = m_theClientManager[dwIndex]->uKeyMode;
                SendAccountBegin.AccountBegin.ServerKey    = ~m_theClientManager[dwIndex]->uServerKey;
                SendAccountBegin.AccountBegin.ClientKey    = ~m_theClientManager[dwIndex]->uClientKey;
				
			}

            pSocket_->Write((BYTE *)&SendAccountBegin, sizeof(SendAccountBegin));

			/*
			 Output( GetTimeStamp() + _T("Add a client [ID:") + 
			 ToString( dwIndex ) + _T( "] - Current total is " ) +
			 ToString( m_usedClientNode.size() ) );
			 */
			/*
			 * Post a read command to IOCP
			 * Enter into a process that it can get data from network
			 */
			
			if ( m_pfnCallBackServerEvent )
			{//通知链接	 无限增大的网络号
				m_pfnCallBackServerEvent( m_lpCallBackParam, dwIndex, enumClientConnectCreate);
			}

			//Trace(pData->GetConnectionDetails().c_str(), GetTimeStamp() + _T(" Add a client [ID:") + ToString( dwIndex ) + _T( "] - Current total is " ) + ToString( m_usedClientNode.size() ) );	//客户端数量
			//printf("-%s-%s\n",pData->GetConnectionDetails().c_str(), GetTimeStamp() + _T(" Add a client [ID:") + ToString( dwIndex ) + _T( "] - Current total is " ) + ToString( m_usedClientNode.size() ) );
			//printf("-%s-Add a client [ID:%u]-Current total is :%d -\n",pData->GetConnectionDetails().c_str(), dwIndex ,m_usedClientNode.size());
			
			pSocket_->Read();			
		}
		else
		{
			pSocket_->AbortiveClose();
			printf("_HelperAddClient::AbortiveClose");
		}

		SAFE_RELEASE( pSocket_ );

		return true;
	}
	else
	{
		SAFE_DELETE( pData );
	}

	return false;
}

bool CIOCPServer::_HelperDelClient()
{
	CPerConnectionData *pData = ( CPerConnectionData * )( m_theDelClientQueue.Pop() );

	if ( NULL == pData )
	{
		return false;
	}
	
	Socket *pSocket = ( Socket * )( pData->GetConnectionSecondParam() );
	
	if ( pSocket )
	{
		pSocket->SetUserData( NULL );
	}
	
	DWORD dwIndex = pData->GetConnectionFirstParam();
	
	if ( ( DWORD )( -1 ) != dwIndex )
	{//如果存在就删除
		if ( m_pfnCallBackServerEvent )
		{//通知网络断开
			m_pfnCallBackServerEvent( m_lpCallBackParam, dwIndex, enumClientConnectClose );
		}

		LPCLIENT_NODE pCN = NULL;
		{
			CCriticalSection::Owner	locker( m_csCM );
			
			pCN = ( LPCLIENT_NODE )( m_theClientManager[dwIndex] );
			
			if ( pCN )
			{
				pCN->pSocket = NULL;
				
				pCN->pReadBuffer->Empty();
				pCN->pRecvBuffer->Empty();
				pCN->pWriteBuffer->Empty();
			}
			
			/*
			* Erase ID from the used table
			*/
			printf("--Loai bo nut mang client(%u):%u,%u --\n",dwIndex,m_theClientManager[dwIndex]->uServerKey,m_theClientManager[dwIndex]->uClientKey);
			m_usedClientNode.remove( dwIndex );//移除一个已用节点
		}
		
		/*
		* A ID of call back is entered into the free table
		*/
		{
			CCriticalSection::Owner locker( m_csFCN );
			m_freeClientNode.push(dwIndex);//压入一个空闲的节点
		}	
	}
	
	SAFE_RELEASE( pSocket );
	SAFE_DELETE( pData );

	return true;
}

STDMETHODIMP CIOCPServer::RegisterMsgFilter( const unsigned long ulnClientID, 
		IMessageProcess* pfnMsgNotify )
{
	HRESULT hr = E_FAIL;

	if ( ulnClientID>=0 && ulnClientID < m_nPlayerMaxCount ) //m_nPlayerMaxCount
	{
		LPCLIENT_NODE pCN = NULL;

		CCriticalSection::Owner	locker( m_csCM );

		pCN = ( LPCLIENT_NODE )( m_theClientManager[ulnClientID]);

		if ( pCN && pCN->pSocket )
		{//回调函数
			pCN->pSocket->SetMessageCallback(pfnMsgNotify);
			
			hr = S_OK;
		}
	}

	return hr;
}

void CIOCPServer::ProcessMessage( Socket *pSocket, CIOBuffer *pBuffer )
{
	const size_t used = pBuffer->GetUsed();
	static size_t minMessageSize = sizeof( WORD ) + sizeof( BYTE );
	
	if ( used >= minMessageSize )
	{
		const CPerConnectionData *pPCD = reinterpret_cast< const CPerConnectionData * >( pSocket->GetUserPtr() );

		if ( !pPCD )
		{
			return;
		}

		DWORD dwIndex = pPCD->GetConnectionFirstParam();
		LPCLIENT_NODE pCN;
		{
			CCriticalSection::Owner	locker( m_csCM );

			if ( NULL == ( pCN = ( LPCLIENT_NODE )( m_theClientManager[dwIndex] ) ) )
			{
				return;
			}
			
			// Add by Freeway Chen
			_ASSERT(pCN->uKeyMode == 0);

			const BYTE* pData = pBuffer->GetBuffer();
			WORD processedSize = 0;
			WORD messageSize = ( WORD )( *( WORD * )( pData ) );

			while ( used >= processedSize + messageSize )
			{
				// messageSize is the peer's frame length and the two calls below size the payload
				// as messageSize - sizeof(WORD): a 0 or 1 is not a short frame but a size_t wrapped
				// to 0xFFFFFFFE here and 0xFFFFFFFFFFFFFFFE at 64 bits, and 0 also stops pData from
				// advancing, so the break at the foot of this loop is never reached.  The floor is
				// the same sizeof(WORD) + sizeof(BYTE) the top of this function already asks for.
				if ( messageSize < ( WORD )( sizeof( WORD ) + sizeof( BYTE ) ) )
				{
					pBuffer->Empty();
					pSocket->Read( pBuffer );
					return;
				}
				KSG_DecodeBuf(messageSize - sizeof(WORD),const_cast<unsigned char *>(pData + sizeof( WORD )),&(pCN->uClientKey));

				pSocket->GetMessageCallback()->OnMessage((VOID*)(pData+sizeof( WORD )), 
						messageSize - sizeof(WORD));

				pData += messageSize;

				processedSize += messageSize;

				if ( used < processedSize + sizeof(WORD))
				{
					// no more complete message
					break;
				}
				
				messageSize = ( WORD )( *( WORD * )( pData ) );
			}

			if ( used > processedSize )
			{
				pBuffer->RemoveBuffer( processedSize );
			}
			else
			{
				pBuffer->Empty();
			}
		}
	}

	pSocket->Read( pBuffer );
}
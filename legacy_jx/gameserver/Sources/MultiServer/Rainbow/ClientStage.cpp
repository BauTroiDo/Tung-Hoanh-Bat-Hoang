
#include "ClientStage.h"
#include "KSG_EncodeDecode.h"
#include <time.h>

#include "Utils.h"
#include "Exception.h"
/*
using CIOBuffer;

using CException;
using Output;
using CCriticalSection;
using CWin32Exception;
*/
/*
 * Get client factory interface
 */
STDAPI CreateInterface(REFIID	riid,void	**ppv)
{
	HRESULT hr = E_NOINTERFACE;

	if (IID_IClientFactory == riid)
	{
		CClientFactory *pObject = new CClientFactory;

		*ppv = reinterpret_cast<void *> (dynamic_cast<IClientFactory *>(pObject));

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

/*static unsigned gs_holdrand = time(NULL);

static inline unsigned _Rand()
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
}*/

/*
 * class CGameClient  CGameClient(maxFreeBuffers,maxFreeBuffers_Cache,bufferSize_Cache,bufferSize); //2,2   8
 */
CGameClient::CGameClient(size_t maxFreeBuffers, 
		size_t maxFreeBuffers_Cache,
		size_t bufferSize_Cache /*= 8192*/,
		size_t bufferSize/*= 1024*/ )
		: CSocketClient(maxFreeBuffers,bufferSize)
		, m_theCacheAllocator(bufferSize_Cache, maxFreeBuffers_Cache)
		, m_lRefCount(0)
		, m_pfnCallbackClientEvent(NULL)
{
	m_pReadBuffer = m_theCacheAllocator.Allocate();
	m_pRecvBuffer = m_theCacheAllocator.Allocate();
}

CGameClient::~CGameClient()
{
	m_pReadBuffer->Release();
	m_pRecvBuffer->Release();
}

//创建线程
STDMETHODIMP CGameClient::Startup()
{
	HRESULT hr = E_FAIL;

	try
	{
		Start();

		hr = S_OK;
	}
	catch(...)
	{
		//Output( _T("CGameClient::Startup - Unexpected exception") );
	}
	
	return hr;	
}

STDMETHODIMP CGameClient::Cleanup()
{
	HRESULT hr = E_FAIL;

	try
	{
		StopConnections();
		
		WaitForShutdownToComplete();

		hr = S_OK;
	}
	/*catch( const CException &e )
	{
		Output( _T("CConnectManager::Cleanup Exception: ") + e.GetWhere() + _T(" - ") + e.GetMessage() );
	}*/
	catch(...)
	{
		//Output( _T("CConnectManager::Cleanup Unexpected exception") );
	}

	return hr;
}

STDMETHODIMP CGameClient::FsGameServerConnectTo(
				const char * const &pAddressToConnectServer, 
				unsigned short usPortToConnectServer )
{
	HRESULT hr = E_FAIL;

	try
	{
		Connect(pAddressToConnectServer,usPortToConnectServer); //设置IP 和端口 
		
		if (StartConnections())
		{
			hr = S_OK;
		}
	}
	/*catch( const CException &e )
	{
		Output( _T("Exception: ") + e.GetWhere() + _T(" - ") + e.GetMessage() );
	}*/
	catch(...)
	{
		//Output( _T("Unexpected exception") );
	}
	
	return hr;
}

STDMETHODIMP CGameClient::RegisterMsgFilter( 
				LPVOID lpParam, 
				CALLBACK_CLIENT_EVENT pfnEventNotify )
{
	m_lpCallBackParam = lpParam;
	m_pfnCallbackClientEvent = pfnEventNotify;

	return S_OK;
}

STDMETHODIMP CGameClient::SendPackToServer(
				const void * const pData,
				const size_t &datalength )
{
	static const size_t s_len_protocol = sizeof(WORD);
	
	if (!pData || 0 == datalength)
	{
		return E_FAIL;
	}

	_ASSERT(m_uKeyMode == 0);

	CIOBuffer *pBuffer = Allocate();

	/*
	 * Add package header
	 */
	const size_t headlength = s_len_protocol + datalength;
	pBuffer->AddData(reinterpret_cast<const char *>(&headlength), s_len_protocol);	
	//unsigned uOldKey = m_uClientKey;
	//KSG_EncodeBuf(datalength,(unsigned char *)pData,&m_uClientKey);
	pBuffer->AddData(reinterpret_cast< const char* const>(pData), datalength);
	KSG_EncodeBuf(datalength,(unsigned char *)(pBuffer->GetBuffer() + s_len_protocol),&m_uClientKey);
	Write(pBuffer);
	pBuffer->Release();

	return S_OK;
}

STDMETHODIMP_(const void *) CGameClient::GetPackFromServer(size_t &datalength)
{
	CCriticalSection::Owner locker(m_csReadAction);

	m_pReadBuffer->Empty();

	m_pRecvBuffer = ProcessDataStream(m_pRecvBuffer);

	const BYTE *pPackData = m_pReadBuffer->GetBuffer();
	datalength = m_pReadBuffer->GetUsed();	
	
	return reinterpret_cast<const char*>(pPackData);
}

STDMETHODIMP CGameClient::Shutdown()
{
	StopConnections();

	return S_OK;
}

STDMETHODIMP CGameClient::QueryInterface( REFIID riid, void** ppv )
{
	/*
	 * By definition all COM objects support the IUnknown interface
	 */
	if( riid == IID_IUnknown )
	{
		AddRef();

		*ppv = dynamic_cast< IUnknown * >( this );
	}
	else if ( riid == IID_IESClient )
	{
		AddRef();

		*ppv = dynamic_cast< IClient * >( this );	
	}
	else
	{
		*ppv = NULL;

		return E_NOINTERFACE;
	}

	return S_OK;
}
    
STDMETHODIMP_(ULONG) CGameClient::AddRef()
{
	return InterlockedIncrement( &m_lRefCount );
}

STDMETHODIMP_(ULONG) CGameClient::Release()
{
	if ( InterlockedDecrement( &m_lRefCount ) > 0 )
	{
		return m_lRefCount;
	}

	delete this;
	return 0L;
}

void CGameClient::ProcessCommand( const CIOBuffer *pBuffer)
{
	static const size_t s_len_protocol = sizeof( WORD );

	const BYTE *pPackData = pBuffer->GetBuffer();
	const size_t used = pBuffer->GetUsed();

	if ( used <= s_len_protocol )
	{
        return;
	}
	
	_ASSERT(m_uKeyMode == 0);
	KSG_DecodeBuf(used - s_len_protocol,(unsigned char *)( pPackData + s_len_protocol ),&m_uServerKey);

    m_pReadBuffer->AddData( ( const BYTE * )( pPackData + s_len_protocol ), used - s_len_protocol );
}

size_t CGameClient::GetMinimumMessageSize() const
{
	static size_t length = sizeof( WORD ) + sizeof( BYTE );

	/*
     * The smallest possible command we accept is a byte onlye package
	 */
	return length;
}

size_t CGameClient::GetMessageSize( 
			const CIOBuffer *pBuffer ) const
{
	const BYTE *pData = pBuffer->GetBuffer();   
	const size_t used = pBuffer->GetUsed();

	WORD wHeadLen = ( WORD )( *( WORD * )( pData ) );

	return ( size_t )( wHeadLen );
}

CIOBuffer *CGameClient::ProcessDataStream( 
						CIOBuffer *pBuffer)
{
	const size_t used = pBuffer->GetUsed();
	
	if ( used >= GetMinimumMessageSize() )
	{
		const size_t messageSize = GetMessageSize( pBuffer );
		
		if ( messageSize == 0 )
		{
			/*
			 * havent got a complete message yet.
			
			 * we null terminate our messages in the buffer, so we need to reserve
			 * a byte of the buffer for this purpose...
			 */
			
			if ( used == ( pBuffer->GetSize() - 1 ) )
			{
				//Output( _T("Too much data!") );
				
				/*
				 * Write this message and then shutdown the sending side of the socket.
				 */
				//Output( "found error and close this connection!" );
	
				Shutdown();	
				/*
				 * throw the rubbish away
				 */
				pBuffer->Empty();					
			}
		}
		else if ( used == messageSize )
		{
			//Output( _T("Got a complete message and begin to process it") );
			/*
			 * we have a whole, distinct, message
			 */
			
			ProcessCommand( pBuffer );
			
			pBuffer->Empty();
		}
		else if (used > messageSize)
		{
			//Output(_T("Got message plus extra data"));
			/*
			 * we have a message, plus some more data
			 * 
			 * allocate a new buffer, copy the extra data into it and try again...
			 */
			
			CIOBuffer *pMessage = pBuffer->SplitBuffer( messageSize );
			
			ProcessCommand( pMessage );
			
			pMessage->Release();
			
		}
	}
	
	/*
	 * Reissue a read into the same buffer to collect more data
	 */
	return pBuffer;
}

void CGameClient::OnStartConnections()
{
	if ( m_pfnCallbackClientEvent )
	{
		m_pfnCallbackClientEvent(m_lpCallBackParam, enumServerConnectCreate);
	}
}

void CGameClient::OnStopConnections()
{
	if ( m_pfnCallbackClientEvent )
	{
		m_pfnCallbackClientEvent(m_lpCallBackParam, enumServerConnectClose);
	}
}

void CGameClient::ReadCompleted( CIOBuffer *pBuffer )
{
	try
	{	
		const BYTE *pPackData = pBuffer->GetBuffer();
		size_t used = pBuffer->GetUsed();

		if (used > 0)
		{
			
			CCriticalSection::Owner locker( m_csReadAction );
			
			m_pRecvBuffer->AddData( reinterpret_cast< const char * >( pPackData ), used );
		}
		
		pBuffer->Empty();
	}
	catch(...)
	{
		//Output( _T("ReadCompleted - Unexpected exception") );
		
		StopConnections();
	}
}

CClientFactory::CClientFactory()
		: m_lRefCount( 0 ),
		m_bufferSize( 0 ),
		m_MaxPlauerCount(0),
        m_nPrecision(0)

{

}

CClientFactory::~CClientFactory()
{

}

STDMETHODIMP CClientFactory::SetEnvironment(const size_t &mMaxPlauerCount,const size_t &nPrecision, const size_t &bufferSize)
{
	m_bufferSize = bufferSize;
	m_nPrecision = nPrecision;
	m_MaxPlauerCount = mMaxPlauerCount;
	return S_OK;
}
//创建客户端环境
/*
size_t maxFreeBuffers, 
size_t maxFreeBuffers_Cache,
size_t bufferSize_Cache = 8192,
size_t bufferSize = 1024*64
: CSocketClient( maxFreeBuffers, bufferSize )
		, m_theCacheAllocator( bufferSize_Cache, maxFreeBuffers_Cache )
*/
STDMETHODIMP CClientFactory::CreateClientInterface(REFIID riid, void** ppv)
{
	HRESULT hr = E_NOINTERFACE;

	if (IID_IESClient == riid)
	{		
		const size_t bufferSize           =  1024;    ////决定发包的大小 分配的内存(m_bufferSize > 0) ? m_bufferSize : (1024*64);
		const size_t bufferSize_Cache     =  (m_bufferSize > 0) ? m_bufferSize : (1024*64);////分配的内存

		const size_t maxFreeBuffers	      = 2;//m_MaxPlauerCount;//1024*8;                                           //空闲列表数量
		const size_t maxFreeBuffers_Cache  = 2;//m_MaxPlauerCount;//(m_bufferSize > 0) ? m_bufferSize : (1024*64);	 //空闲列表数量

		/*
		const size_t bufferSize = ( m_bufferSize > 0 ) ? m_bufferSize : ( 1024 * 4 );

		CGameClient *pObject = new CGameClient( 2, 2, bufferSize, 1024 );
		*/
		CGameClient *pObject = new CGameClient(maxFreeBuffers,maxFreeBuffers_Cache,bufferSize_Cache,bufferSize); //2,2   8

		*ppv = reinterpret_cast<void *> (dynamic_cast<IClient *>(pObject));

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

STDMETHODIMP CClientFactory::QueryInterface(REFIID riid, void** ppv)
{
	/*
	 * By definition all COM objects support the IUnknown interface
	 */
	if(riid == IID_IUnknown)
	{
		AddRef();

		*ppv = dynamic_cast<IUnknown *>(this);
	}
	else if (riid == IID_IClientFactory)
	{
		AddRef();

		*ppv = dynamic_cast<IClientFactory *>(this);	
	}
	else
	{
		*ppv = NULL;

		return E_NOINTERFACE;
	}

	return S_OK;
}
    
STDMETHODIMP_(ULONG) CClientFactory::AddRef()
{
	return ::InterlockedIncrement(&m_lRefCount);
}

STDMETHODIMP_(ULONG) CClientFactory::Release()
{
	if ( ::InterlockedDecrement(&m_lRefCount)>0)
	{
		return m_lRefCount;
	}

	delete this;
	return 0L;
}
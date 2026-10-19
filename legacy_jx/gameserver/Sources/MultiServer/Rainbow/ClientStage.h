/********************************************************************
	created:	2003/03/24
	file base:	ClientStage
	file ext:	h
	author:		liupeng
	
	purpose:	
*********************************************************************/
#ifndef __INCLUDE_CLIENTSTAGE_H__
#define __INCLUDE_CLIENTSTAGE_H__

#if defined (_MSC_VER) && (_MSC_VER >= 1020)
	#pragma once
#endif

#include "SocketClient.h"
#include "tstring.h"
#include "Event.h"

#include "Interface\RainbowInterface.h"
#include "Interface\IClient.h"

/*
 * class CGameClient  
 */
class CGameClient : public IClient, public CSocketClient  //
{
public:

	STDMETHOD( Startup )( );
	STDMETHOD( Cleanup )( );
	//STDMETHOD( GetTheaidHdw )( );
	
	STDMETHOD( FsGameServerConnectTo )( const char * const &pAddressToConnectServer,
		unsigned short usPortToConnectServer );

	STDMETHOD( RegisterMsgFilter )( LPVOID lpParam,
		CALLBACK_CLIENT_EVENT pfnEventNotify );

	STDMETHOD( SendPackToServer )( const void * const pData,
		const size_t &datalength );

	STDMETHOD_( const void *, GetPackFromServer )( size_t &datalength );

	STDMETHOD( Shutdown )( );
	
	/*
     *  IUnknown COM Interface Methods
     */
    STDMETHOD ( QueryInterface ) ( REFIID riid, void** ppv );
    STDMETHOD_( ULONG, AddRef ) ( );
    STDMETHOD_( ULONG, Release ) ( );

public:

	CGameClient( size_t maxFreeBuffers, 
		size_t maxFreeBuffers_Cache,
		size_t bufferSize_Cache = 8192,
		size_t bufferSize = 1024);	
	
	virtual ~CGameClient();

protected:
	
	size_t GetMinimumMessageSize() const;	
	size_t GetMessageSize( const CIOBuffer *pBuffer ) const;
	
	CIOBuffer *ProcessDataStream(
				CIOBuffer *pBuffer);
	
	void ProcessCommand( const CIOBuffer *pBuffer);

private:

	virtual void OnStartConnections();
	virtual void OnStopConnections();
	
	virtual void ReadCompleted( CIOBuffer *pBuffer );

private:

	LONG m_lRefCount;

	CIOBuffer::Allocator	m_theCacheAllocator;

	LPVOID					m_lpCallBackParam;
	CALLBACK_CLIENT_EVENT	m_pfnCallbackClientEvent;

	CCriticalSection m_csReadAction;
	
	CIOBuffer		*m_pRecvBuffer;
	CIOBuffer		*m_pReadBuffer;

	/*
	 * No copies do not implement
	 */
	CGameClient( const CGameClient &rhs );
	CGameClient &operator=( const CGameClient &rhs );

};

/*
 * CClientFactory
 */
class CClientFactory : public IClientFactory
{
public:

	STDMETHOD(SetEnvironment)
		(
		    const size_t &mMaxPlauerCount,
			const size_t &nPrecision, 
			const size_t &bufferSize
		);

	STDMETHOD(CreateClientInterface)
		(
			REFIID riid,
			void** ppv
		);

	/*
     *  IUnknown COM Interface Methods
     */
    STDMETHOD ( QueryInterface ) ( REFIID riid, void** ppv );
    STDMETHOD_( ULONG, AddRef ) ( );
    STDMETHOD_( ULONG, Release ) ( );

public:

	CClientFactory();
	virtual ~CClientFactory();

private:

	long	m_lRefCount;

	size_t m_bufferSize;
	size_t m_MaxPlauerCount;
	size_t m_nPrecision;
};


#endif // __INCLUDE_CLIENTSTAGE_H__
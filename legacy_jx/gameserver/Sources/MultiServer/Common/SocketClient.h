/********************************************************************
	created:	2003/02/18
	file base:	SocketClient
	file ext:	h
	author:		liupeng
	
	purpose:	
*********************************************************************/
#ifndef __INCLUDE_SOCKETCLIENT_H__
#define __INCLUDE_SOCKETCLIENT_H__

#if defined (_MSC_VER) && (_MSC_VER >= 1020)
	#pragma once
#endif

#include "UsesWinsock.h"
#include "Thread.h"
#include "CriticalSection.h"
#include "IOBuffer.h"
#include "ManualResetEvent.h"
#include "NodeList.h"
#include "OpaqueUserData.h"
#include "EventSelect.h"

#include "Cipher.h"


enum e_socket_status
{
	SS_DONE , 
	SS_NOT_READY , 
	SS_DISCONNECTED , 
	SS_ERROR
};

/*
 * namespace OnlineGameLib::Win32
 */

//namespace OnlineGameLib {
//namespace Win32 {

class CSocketClient : 
		protected CThread, 
		private CUsesWinsock, 
		protected CIOBuffer::Allocator
{
public:
	
	virtual ~CSocketClient();

	using CThread::Start;

//	HANDLE  CThread::GetHandle;

	void Connect(
		const _tstring &addressToConnectServer,
		unsigned short portToConnectServer
		);

	bool StartConnections();
	void StopConnections();

	void InitiateShutdown();
	void WaitForShutdownToComplete();

	void Write( const char *pData, size_t dataLength );
	void Write( CIOBuffer *pBuffer );
	
protected:

	CSocketClient(
		const _tstring &addressToConnectServer,
		unsigned short portToConnectServer,
		size_t maxFreeBuffers,
		size_t bufferSize /* = 1024 */
		);
	
	CSocketClient(
		size_t maxFreeBuffers,
		size_t bufferSize /* = 1024 */
		);
	
	void ReleaseBuffers();

	bool WaitAndVerifyCipher();
	
	/*
	 * Virtual function has different access specifier to base class
	 */
	virtual int Run();

    unsigned m_uKeyMode;
    unsigned m_uServerKey;
    unsigned m_uClientKey;

    #pragma pack( 1 )
    struct 
	{
		WORD			wLen;
		ACCOUNT_BEGIN	AccountBegin;

	}m_theSendAccountBegin;
    #pragma pack()

private:

	/*
	 * Override this to create the listening socket of your choice
	 */	
	virtual SOCKET CreateConnectionSocket( 
				const _tstring &addressToConnectServer,
				unsigned short port);

	/*
	 * Interface for derived classes to receive state change notifications...
	 */	
	virtual void OnStartConnections() {};  //通知已经链接
	virtual void OnStopConnections() {};   //通知已经断开网络
	virtual void OnShutdownInitiated() {};
	virtual void OnShutdownComplete() {};

	virtual void OnConnect() {};
	virtual void OnClose() {};

	void OnRead( CIOBuffer *pBuffer );
	virtual void ReadCompleted( CIOBuffer *pBuffer ) = 0;

	virtual void OnWrite(){};

	virtual void OnError( const _tstring &message );

	SOCKET m_connectSocket;

	CEventSelect	m_eventSelect;

	CManualResetEvent m_shutdownEvent;
	CManualResetEvent m_successConnectionsEvent;

    CCriticalSection m_criticalSection;


	_tstring	m_address;
	unsigned short m_port;

	e_socket_status _get_error_status() const;
	int _wait(int ms_, bool read_);
};
	
//} // End of namespace OnlineGameLib
//} // End of namespace Win32

#endif //__INCLUDE_SOCKETCLIENT_H__
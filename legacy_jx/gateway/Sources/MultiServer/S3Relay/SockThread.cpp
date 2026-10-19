// SockThread.cpp: implementation of the CSockThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SockThread.h"
#include "S3Relay.h"
#include "Global.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSockThread::CSockThread()
	: m_step(step_NONE), m_cntLoop(0), m_tickLoop(0)
{

}

CSockThread::~CSockThread()
{

}


DWORD CSockThread::Main(LPVOID lpParam)
{
	try
	{
		DEBUGDO(m_step = step_NONE);
		DEBUGDO(m_tickLoop = 0);
		DEBUGDO(m_cntLoop = 0);

		DEBUGDO(m_step = step_EnterLoop);

		EnterLoop();
//无限循环
		while (!IsAskingStop())
		{
			int a=0,b=0,c=0,d=0,e=0,f=0,g=0;
			DEBUGDO(m_tickLoop = ::GetTickCount());
			DEBUGDO(++m_cntLoop);

			DEBUGDO( m_step = step_PrepareSock);
			PrepareSock();
			DEBUGDO( m_step = step_RelayCenter);
			a=g_RelayCenter.Route();
			if (!a)
             rTRACE("g_RelayCenter: sock thread except");
			DEBUGDO( m_step = step_RelayServer);
			b=g_RelayServer.Route();
			if (!b)
             rTRACE("g_RelayServer: sock thread except");
			DEBUGDO( m_step = step_RootCenter);
			c=g_RootCenter.Route();
			if (!c)
             rTRACE("g_RootCenter: sock thread except");
			DEBUGDO( m_step = step_GatewayCenter);
			d=g_GatewayCenter.Route();  //网关
			if (!d)
             rTRACE("g_GatewayCenter: sock thread except");
			DEBUGDO( m_step = step_HostServer);
			e=g_HostServer.Route();
			if (!e)
             rTRACE("g_HostServer: sock thread except");
			DEBUGDO( m_step = step_TongServer);
			f=g_TongServer.Route();     //帮会
            if (!f)
			rTRACE("g_TongServer: sock thread except");
			DEBUGDO( m_step = step_ChatServer);
			g=g_ChatServer.Route();     //聊天
            if (!g)
			rTRACE("g_ChatServer: sock thread except");
			DEBUGDO( m_step = step_UnprepareSock);
			UnprepareSock();

			DEBUGDO( m_step = step_Sleep);
			::Sleep(breathe_interval);
		}

		DEBUGDO( m_step = step_LeaveLoop);
	    LeaveLoop();
	}
	catch (...)
	{
		rTRACE("[S3主线程]:(CSockThread)执行失败(退出)！请检查!");
	}

	return 0;
}


HANDLE CSockThread::Start()
{
	if (m_hProcessor != NULL)
		return m_hProcessor;

	m_step = step_NONE;
	m_cntLoop = 0;
	m_tickLoop = 0;

	HANDLE reth = KThread::Start();
	if (!reth)
	{
		rTRACE("FAIL: SockThread start");
		return NULL;
	}

	rTRACE("SockThread start success");

	return reth;
}

BOOL CSockThread::Stop()
{
	if (!m_hProcessor)
		return TRUE;

	if (!KThread::Stop())
	{
		rTRACE("FAIL: SockThread stop");
		return FALSE;
	}

	m_step = step_NONE;
	m_cntLoop = 0;
	m_tickLoop = 0;

	rTRACE("SockThread stop success");

	return TRUE;
}


BOOL CSockThread::TraceInfo()
{
	static const char* stepdesc[] = {
			"NONE",
			"EnterLoop",
			"PrepareSock",
			"RelayCenter", 
			"RelayServer",
			"RootCenter",
			"GatewayCenter",
			"HostServer",
			"TongServer",
			"ChatServer",
			"UnprepareSock",
			"LeaveLoop",
			"Sleep",
	};

	rTRACE("message: [SockThread] tickLoop: %d, cntLoop: %d, step: %s", m_tickLoop, m_cntLoop, stepdesc[m_step]);

	return TRUE;
}

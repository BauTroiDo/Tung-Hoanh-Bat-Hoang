/********************************************************************
	created:	2003/06/05
	file base:	ClientNode
	file ext:	h
	author:		liupeng
	
	purpose:	
*********************************************************************/
#ifndef __INCLUDE_CLIENTNODE_H__
#define __INCLUDE_CLIENTNODE_H__

#include "IServer.h"
#include "HeavenInterface.h"

#include "Buffer.h"
#include "CriticalSection.h"
#include "Event.h"

#include "kprotocoldef.h"

//#define _NAME_LENGTH 64

#include <list>

using namespace std;


//增加检测 防止不同gs之间重复登录
struct RoleLoginInfo
{
	int     m_RegServerIndex;//大区的索引
	int     m_GsNumBer;      //Gs的编号
	size_t 	m_idx;           //连接索引号 
	bool    m_IsOffLine;     //是否离线
};

class CClientNode
{
public:
	
	explicit CClientNode( IServer *pServer, size_t id );

	virtual ~CClientNode();

	static bool Start( IServer *pServer );
	static void End();

	static CClientNode *AddNode( IServer *pServer, size_t id );
	static void DelNode( size_t id );

	void AppendData( const void *pData, size_t datalength );
	void Process();

protected:

	static unsigned int __stdcall ThreadFunction( void *pV );

	static HANDLE									m_hThread;
	static CEvent				m_hQuitEvent;

	static CCriticalSection	m_csCL;
	
	typedef map< size_t, CClientNode * >	stdMap;

	static stdMap					m_theClientMap;

	void LargePackProcess( const void *pData, size_t dataLength );
	void SmallPackProcess( const void *pData, size_t dataLength );

private:

	size_t		m_nIndentity;

	CPackager m_theRecv;
	CPackager m_theSend;

	CBuffer::Allocator m_theAllocator;

	IServer		*m_pServer;

	typedef void ( CClientNode::*ProcessArray[c2s_end] )( const void *pData, UINT dataLength );

	ProcessArray m_theProcessArray;

	void _QueryRoleList( const void *pData, UINT dataLength );
	void _CreateRole( const void *pData, UINT dataLength );
	void _SaveRoleInfo( const void *pData, UINT dataLength );
	void _SaveTradePair( const void *pData, UINT dataLength );
	void SendTradePairResult(unsigned __int64 uTxId, int nResultCode, const char* szRoleA, const char* szRoleB, unsigned int idA, unsigned int idB);
	void _DelRole( const void *pData, UINT dataLength );
	void _CheckRoleName( const void *pData, UINT dataLength );
	void _GetRoleInfo( const void *pData, UINT dataLength );
	void _RelayExtend( const void *pData, UINT dataLength );
	void _GetGameStat( const void *pData, UINT dataLength );//发送游戏统计数据(By Fellow,2003.7.22)
	void _LockOrUnlockRole( const void *pData, UINT dataLength );
	void gm_synroleaction( const void *pData, UINT dataLength );

	/*
	 * class CDataQueue
	 */
	class CDataQueue
	{
	public:

		explicit CDataQueue(size_t bufferSize =1024*128, size_t maxFreeBuffers =16);	//128
		
		~CDataQueue();

		/*
		 * Function
		 */
		//小包
		bool		AddData( const BYTE *pData, size_t datalength );
		//大包
		bool		AddData( const void *pData, size_t datalength );

		CBuffer		*Get();

	private:
		/*
		 * Data
		 */
		CCriticalSection		m_csQueue;

		typedef list< CBuffer * > stdDataList;

		stdDataList									m_theData;

		CBuffer::Allocator	m_theDQAllocator;
	};

	CDataQueue	m_theDataQueue;

	static CCriticalSection	m_csCR;

	typedef map< std::string, RoleLoginInfo >	stdRoleLockMap;  //size_t
	static stdRoleLockMap m_csRoleLock;

	int  getRoleLock(char* szRole,RoleLoginInfo *resInfo);
	int  IsRoleLock(char* szRole,RoleLoginInfo *resInfo);
	bool IsRoleLockBySelf(char* szRole);
	bool UnlockRoleSelf(char* szRole);
	void UnlockAllRole(size_t ID);

	//用来判断这个角色是否已经登录的了
	typedef map< std::string, RoleLoginInfo>	stdRoleLoginInfo;
	static stdRoleLoginInfo m_csRoleLoginInfo;
};

#endif // __INCLUDE_CLIENTNODE_H__
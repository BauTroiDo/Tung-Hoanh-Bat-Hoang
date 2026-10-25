#include "stdafx.h"
#include "ClientNode.h"

#include <process.h>
#include <iostream>

#include "IDBRoleServer.h"
#include "CRC32.h"

#include "Macro.h"

#include "KGmProtocol.h"
#include "RoleNameFilter.h"
/*
using CCriticalSection;
using CPackager;
using CEvent;
using CBuffer;
*/
HANDLE CClientNode::m_hThread = NULL;
CEvent CClientNode::m_hQuitEvent( NULL, true, false );

extern UINT g_nDBEngineLoop;

CCriticalSection	CClientNode::m_csCL;

CClientNode::stdMap	CClientNode::m_theClientMap;


CClientNode::CDataQueue::CDataQueue( size_t bufferSize /*= 1024 * 64*/, size_t maxFreeBuffers /*= 160*/ )
						: m_theDQAllocator(bufferSize, maxFreeBuffers)
{
}

CClientNode::CDataQueue::~CDataQueue()
{
}
//小包
bool CClientNode::CDataQueue::AddData( const BYTE *pData, size_t datalength )
{
	CBuffer *pBuffer = m_theDQAllocator.Allocate();

	if (!pBuffer)
	    return false;

	pBuffer->AddData(pData, datalength);

	{
		CCriticalSection::Owner lock( m_csQueue );
		/*
			  使用list的成员函数push_back和push_front插入一个元素到list中。
			  其中push_back()从list的末端插入，而 push_front()实现的从list的头部插入。
		*/
		m_theData.push_back(pBuffer);  //push_back()是把插入元素直接放入链表结尾
	}
	//pBuffer->Release();
	//m_theDQAllocator.Release(pBuffer);
	//SAFE_RELEASE(pBuffer);
	return true;
}
//大包
bool CClientNode::CDataQueue::AddData( const void *pData, size_t datalength )
{
	CBuffer *pBuffer = m_theDQAllocator.Allocate();

	if (!pBuffer)
		return false;

	pBuffer->AddData((char*)pData, datalength);
	
	{
		CCriticalSection::Owner lock( m_csQueue );
		/*
			  使用list的成员函数push_back和push_front插入一个元素到list中。
			  其中push_back()从list的末端插入，而 push_front()实现的从list的头部插入。
		*/
		m_theData.push_back(pBuffer);  //push_back()是把插入元素直接放入链表结尾
	}
	//m_theDQAllocator.Release(pBuffer);
	return true;
}

CBuffer *CClientNode::CDataQueue::Get()
{
	{
		CCriticalSection::Owner lock( m_csQueue );

		if (!m_theData.empty())
		{
			/*
			  通过front()可以获得list容器中的头部元素，通过back()可以获得list容器的最后一个元素。
			  但是有一点要注意，就是list中元素是空的时候，这时候调用front()和back()会发生什么呢？
			  实际上会发生不能正常读取数据的情况，但是这并不报错，那我们编程序时就要注意了，
			  个人觉得在使用之前最好先调用empty()函数判断list是否为空。
			*/
			CBuffer *pBuffer = m_theData.front();

			pBuffer->AddRef();
			/*
			 通过pop_back()删除最后一个元素，通过pop_front()删除第一个元素；序列必须不为空
			 如果当list为空的时候调用pop_back()和pop_front()会使程序崩掉。
			*/
			m_theData.pop_front();

			pBuffer->Release();

			return pBuffer;
		}
	}

	return NULL;
}

CClientNode::CClientNode(IServer *pServer, size_t id)
			: m_nIndentity(id) 
			, m_theAllocator(1024*128,3)	//3	个角色 每个角色大小为 64K
			, m_pServer(pServer)
{
	ZeroMemory(m_theProcessArray, sizeof(m_theProcessArray));

	m_theProcessArray[c2s_roleserver_getrolelist]    = &CClientNode::_QueryRoleList;
	m_theProcessArray[c2s_roleserver_createroleinfo] = &CClientNode::_CreateRole;
	m_theProcessArray[c2s_rolekfjhu_saveroleinfo]   = &CClientNode::_SaveRoleInfo;
	m_theProcessArray[c2s_roleserver_savetradepair]  = &CClientNode::_SaveTradePair;	//存档
	m_theProcessArray[c2s_roleserver_deleteplayer]   = &CClientNode::_DelRole;
	m_theProcessArray[c2s_roleserver_getroleinfo]    = &CClientNode::_GetRoleInfo;  //选择角色进入游戏时 获取这个角色的信息
//	m_theProcessArray[c2s_extend] = _RelayExtend;
	m_theProcessArray[c2s_gamestatistic]             = &CClientNode::_GetGameStat;
	m_theProcessArray[c2s_roleserver_lock]           = &CClientNode::_LockOrUnlockRole;//
	m_theProcessArray[c2s_rolecheckname_result]      = &CClientNode::_CheckRoleName;
	m_theProcessArray[c2s_gm_synroleaction]          = &CClientNode::gm_synroleaction;
}

CClientNode::~CClientNode()
{
	UnlockAllRole(m_nIndentity);
	SAFE_RELEASE( m_pServer );
}
//创建一个客户端连接
CClientNode *CClientNode::AddNode( IServer *pServer, size_t id )
{
	CCriticalSection::Owner lock( CClientNode::m_csCL );

	IServer *pCloneServer = NULL;
	pServer->QueryInterface( IID_IIOCPServer, ( void ** )&pCloneServer );

	CClientNode *pNode = new CClientNode( pCloneServer, id );
		
	CClientNode::m_theClientMap.insert( stdMap::value_type( id, pNode ) );

	return pNode;
}
//删除一个客户端连接
void CClientNode::DelNode( size_t id )
{
	stdMap::iterator it;

	if ( CClientNode::m_theClientMap.end() != ( it = CClientNode::m_theClientMap.find( id ) ) )
	{
		CCriticalSection::Owner lock( CClientNode::m_csCL );

		CClientNode *pNode = ( *it ).second;
	
		CClientNode::m_theClientMap.erase( id );

		SAFE_DELETE( pNode );
	}
}
//创建协议线程
bool CClientNode::Start( IServer *pServer )
{
	if ( CClientNode::m_hThread == NULL )
	{
		unsigned int threadID = 0;
		CClientNode::m_hQuitEvent.Reset();		
		CClientNode::m_hThread = (HANDLE)::_beginthreadex(0,
			0,
			ThreadFunction,
			( void * )pServer,
			0,
			&threadID );
		
		if ( CClientNode::m_hThread == NULL )
		{
			return false;
		}
	}

	return true;
}

void CClientNode::End()
{
	CClientNode::m_hQuitEvent.Set();

	if ( CClientNode::m_hThread != NULL )
	{
		DWORD result = ::WaitForSingleObject( CClientNode::m_hThread, 50000 );
		
		if ( result == WAIT_TIMEOUT )
		{
			::TerminateThread( CClientNode::m_hThread, ( DWORD )( -2 ) );
		}
		
		if ( CClientNode::m_hThread != NULL )
		{
			::CloseHandle( CClientNode::m_hThread );
			CClientNode::m_hThread = NULL;
		}
	}

	/*
	 * Save all
	 */
}
//goddess服务器数据 从内存中读包 无限循环线程
unsigned int __stdcall CClientNode::ThreadFunction(void *pV)
{
	IServer *pServer = reinterpret_cast< IServer * >(pV);

	ASSERT(pServer);

	try
	{
		while (!CClientNode::m_hQuitEvent.Wait(0))
		{
			{
				CCriticalSection::Owner lock(CClientNode::m_csCL);

				CClientNode::stdMap::iterator it;
				//客户端连接map容器
				for (it = CClientNode::m_theClientMap.begin();it != CClientNode::m_theClientMap.end(); ++it)
					{
						CClientNode *pNode = (CClientNode *)((*it).second);
						//ASSERT(pNode);
                        if (pNode)
						   pNode->Process();
					}
			}

			if (++g_nDBEngineLoop & 0x80000000)
			{
				g_nDBEngineLoop = 0;
			}

			if (g_nDBEngineLoop & 0x1)
			{
				::Sleep(1);
			}
		}
	}
	catch(...)
	{
		::MessageBox( NULL,_T( "CClientNode::goddess 主线程 was error!"), _T("Warning"),MB_OK);
	}

	return 0L;
}
//从GS获取数据包
void CClientNode::AppendData(const void *pData, size_t datalength)
{
	if (pData && datalength)
	{
		BYTE cProtocol = CPackager::Peek(pData);
		
		if ( cProtocol < s2c_micfghjkdtropackbegin)
		{
			LargePackProcess(pData, datalength);
			//char npakinfo[64]={0};
			//sprintf(npakinfo,"Debug1: %d,len:%d",cProtocol,datalength);
			//::MessageBox( NULL, npakinfo, "Warning",MB_OK);
		}
		//else if (cProtocol > s2c_micfghjkdtropackbegin)
		else if (cProtocol > s2c_micfghjkdtropackbegin && cProtocol < s2c_end && m_theProcessArray[cProtocol])
		{//重连 有内存泄露 BUG 每登陆一次 内存就增加几百k
			SmallPackProcess(pData, datalength);
			//LargePackProcess(pData, datalength);
			//char npakinfo[64]={0};
			//sprintf(npakinfo,"Debug2: %d,len:%d",cProtocol,datalength);
			//::MessageBox( NULL, npakinfo, "Warning",MB_OK);
		}
	}
}

void CClientNode::SmallPackProcess( const void *pData, size_t dataLength)
{
   //BYTE cProtocol = CPackager::Peek(pData);

   //if (cProtocol > s2c_micfghjkdtropackbegin && cProtocol < s2c_end && m_theProcessArray[cProtocol])
   //{
	   m_theDataQueue.AddData((const BYTE *)pData,dataLength);
   //}
}

void CClientNode::LargePackProcess(const void *pData, size_t dataLength)
{
	ASSERT( pData && dataLength );

	CBuffer *pBuffer = m_theRecv.PackUp(pData, dataLength);	//打包处理

	if (pBuffer)
	{
		try
		{
		  m_theDataQueue.AddData(pBuffer->GetBuffer(),pBuffer->GetUsed());
		  SAFE_RELEASE(pBuffer);
		}
		catch (...)
		{
		   SAFE_RELEASE(pBuffer);
		}
	}
}
//接受客户端发包数据--协议函数线程
void CClientNode::Process()
{
	CBuffer *pBuffer = m_theDataQueue.Get();  //出栈　释放内存

	if (pBuffer)
	{
		try
		{
		  const BYTE *pData = pBuffer->GetBuffer();
		  const UINT dataLength = pBuffer->GetUsed();
		  BYTE cProtocol = CPackager::Peek(pData);
		  if (cProtocol>=0 && cProtocol < c2s_end && m_theProcessArray[cProtocol])
		  {//执行这个协议函数
			(this->*m_theProcessArray[cProtocol])((const void *)pData, dataLength);
		  }
		}
		catch (...)
		{
		}
	}

	SAFE_RELEASE(pBuffer);
}
//接收 bishop 发过来的 数据包 获取角色清单
void CClientNode::_QueryRoleList( const void *pData, UINT dataLength )
{
	char szAccountName[ACC_LENGTH];

	ZeroMemory(szAccountName,sizeof(szAccountName));

	ASSERT(m_pServer && pData && dataLength);

#ifdef CONSOLE_DEBUG
	cout << "_QueryRoleList::Begin" << endl;
#endif

	TProcessData *pPlayerList = (TProcessData *)pData;

	int nRoleListCount = pPlayerList->pDataBuffer[0];
	unsigned int nLen = pPlayerList->nDataLen;//名字的实际长度
	unsigned int ulIdentity = pPlayerList->ulIdentity;	 //客户端链接号

	if ( nLen <= 1 || nLen >= ACC_LENGTH)
	{
#ifdef CONSOLE_DEBUG
		cout << "_QueryRoleList::Name is invalid" << endl;
#endif

		return;
	}
	memcpy(szAccountName, (const char *)(&pPlayerList->pDataBuffer[1]),nLen);
	// nLen is at most ACC_LENGTH - 1 above this, and the copy stopped at that index,
	// so the terminator goes there: [nLen + 1] wrote one element past the array at the
	// longest name the guard admits
	szAccountName[nLen] = '\0';
	/*
	 * Database
	 */
	/*char nMsg[128]={0};
	sprintf(nMsg,"获取(%d区)角色的帐号名:%s %d %d\n长度:%d",pPlayerList->uSelServer,szAccountName,nRoleListCount,ulIdentity,nLen);
	::MessageBox(NULL,nMsg,"g提示:",MB_OK|MB_ICONERROR);*/

	S3DBI_RoleBaseInfo DBI[4];
	const UINT s_nStructSize = sizeof(S3DBI_RoleBaseInfo);
	// nRoleListCount is the first byte of a client packet and nMaxCount is what
	// GetRoleListOfAccount bounds its walk with, so the count has to be the number
	// of records DBI holds - the gameserver's twin passes MAX_PLAYER_IN_ACCOUNT
	// for its array the same way (DBThreadFun.cpp:113)
	if (nRoleListCount > (int)(sizeof(DBI) / sizeof(DBI[0])))
		nRoleListCount = (int)(sizeof(DBI) / sizeof(DBI[0]));

#ifdef CONSOLE_DEBUG
	cout << "_QueryRoleList::GetRoleListOfAccount " << szAccountName << endl;
#endif

	int nCount = GetRoleListOfAccount(szAccountName,&DBI[0],nRoleListCount,nLen,pPlayerList->uSelServer);//查找有多少个角色  goddes崩溃了 

	CBuffer *pBuffer = m_theAllocator.Allocate();

	TProcessData *pListData = reinterpret_cast<TProcessData *>(const_cast<BYTE *>(pBuffer->GetBuffer()));

	pListData->nProtoId = s2c_roleserver_getrolelist_result;  //接收 bishop 发过来的数据 发往 客户端 game
	pListData->ulIdentity = ulIdentity;

	int nDataLen = nCount * s_nStructSize + 1;
	pListData->nDataLen = nDataLen;
	
	pListData->pDataBuffer[0] = nCount;
	memcpy(&pListData->pDataBuffer[1],&DBI[0],nDataLen);

	int nUsedLen = sizeof( TProcessData ) - 1 + nDataLen;
	pBuffer->Use(nUsedLen);

	m_pServer->SendData(m_nIndentity,(const void *)pListData,nUsedLen);	//发往BISHOP

	SAFE_RELEASE(pBuffer);	

#ifdef CONSOLE_DEBUG
	cout << "_QueryRoleList::end" << endl;
#endif
}

//创建角色时存档
void CClientNode::_CreateRole( const void *pData, UINT dataLength )//size_t
{
	ASSERT( m_pServer && pData && dataLength );

#ifdef CONSOLE_DEBUG
	cout << "_CreateRole::Begin" << endl;
#endif

	TProcessData *pPD = ( TProcessData * )pData;

	int nResult = 0;

	{{
	extern CRoleNameFilter g_fltRoleName;

	TRoleData* pRoleData = (TRoleData*)(pPD->pDataBuffer + 1);

	/*char sMsg[128];
	sprintf(sMsg,"角色名:%s",pRoleData->BaseInfo.szName);
	MessageBox(0,sMsg,"g不合法",MB_OK);*/

	if (pRoleData->BaseInfo.szName[0])
	{
	    UINT pos;
		for (pos = sizeof(pRoleData->BaseInfo.szName) - 1; pos >= 1; pos--)
		{
			if (!pRoleData->BaseInfo.szName[pos])
				break;
		}
		if (pos >= 1)
		{
			//if (g_fltRoleName.IsTextPass(pRoleData->BaseInfo.szName))    //检测是否有禁止的角色名
			//{
				nResult = SaveRoleInfo(&pPD->pDataBuffer[1],NULL,TRUE);  //存档角色信息	检测是否存在账号名
			//}
			//else
			//{
				//nResult = -1;   // 提示 名字不合法

				/*char sMsg[128];
				sprintf(sMsg,"角色名:%s",pRoleData->BaseInfo.szName);
				MessageBox(0,sMsg,"g名字不合法",MB_OK);*/
			//}
		}
	}
	}}

	TProcessData Info;
	Info.nProtoId = s2c_roleserver_createrole_result;
	Info.ulIdentity = pPD->ulIdentity;
	Info.nDataLen = 1;
	Info.pDataBuffer[0] =(nResult == 1) ? 1 : -1;

//把结果发往BISHHOP
	m_pServer->SendData(m_nIndentity,(const void *)&Info, sizeof(Info));

#ifdef CONSOLE_DEBUG
	cout << "_CreateRole::End" << endl;
#endif
}

//保存角色 存档 返回给GS
void CClientNode::_SaveRoleInfo( const void *pData, UINT dataLength )
{
	ASSERT(m_pServer && pData && dataLength);

	TProcessData *pPD = (TProcessData *)pData;
	
	int nResult = 0;
	TRoleData* pRole = (TRoleData*)(&pPD->pDataBuffer[0]);
	char szName[NAME_LENGHT];
	int len = strlen(pRole->BaseInfo.szName);
	ASSERT( len > 0 );
	len = len > NAME_LENGHT - 1 ? NAME_LENGHT - 1 : len;
	memcpy(szName,pRole->BaseInfo.szName,len);
	szName[len] = '\0';

	/*char sMsga[125];
	sprintf(sMsga,"收到(%d区)角色(%s)帐号:%s要求存档",pPD->uSelServer,pRole->BaseInfo.szName,pRole->BaseInfo.caccname);
	::MessageBox(0,sMsga,"收到角色要求存档",MB_OK);*/
	//if (IsRoleLockBySelf(szName))
		nResult = SaveRoleInfo(&pPD->pDataBuffer[0],NULL,FALSE);	 //不检测账号名 直接添加

		//if (pPD->bLeave)
		//{//如果离开了 就解锁这个角色
		//	UnlockRoleSelf(szName);
		//}

	if (pPD->ulIdentity >= 0) // Final P0: Always acknowledge database save result to GS	//玩家的索引
	{//通知gs 已经存档
		TProcessData Info;	
		Info.nProtoId = s2c_roleserver_saverole_result;
		Info.ulIdentity = pPD->ulIdentity;
		Info.nDataLen = 791030;
		Info.pDataBuffer[0] = (nResult == 1) ? 1:-1;
		Info.uSelServer = pPD->uSelServer;
		m_pServer->SendData(m_nIndentity, (const void *)&Info, sizeof(Info));
	}

}

void CClientNode::gm_synroleaction( const void *pData, UINT dataLength )
{
	ASSERT( m_pServer && pData && dataLength );

	tagGmAction *pRoleInfo = ( tagGmAction * )pData;

	RoleLoginInfo resInfo;
	memset(&resInfo,0,sizeof(resInfo));
	//为防止同一个角色多次同登陆 检测
	if  (getRoleLock(pRoleInfo->nRoleName,&resInfo))
	{

	}

	//s2c_gm_synroleaction
}
//删除角色
void CClientNode::_CheckRoleName( const void *pData, UINT dataLength )
{
	ASSERT( m_pServer && pData && dataLength );
	
	TCheckNameData *pRoleInfo = ( TCheckNameData * )pData;
	
	char szRoleName[NAME_LENGHT];
	
	int nDataLen = strlen(pRoleInfo->nRoleName);//pRoleInfo->nDataLen;
	    // the copy fills szRoleName and the write below terminates it, so a length of
	    // NAME_LENGHT is already one too many for the terminator's index
	    nDataLen = ( nDataLen > NAME_LENGHT - 1) ? NAME_LENGHT - 1 : nDataLen;
	
	int result = -1;
	
	if (nDataLen > 0)
	{
		memcpy( szRoleName, &pRoleInfo->nRoleName, nDataLen);
		szRoleName[nDataLen] = '\0';
		
		if ( CheckRoleName( szRoleName ) )
		{
			result = 1;
		}
		else
		{
			result = -1;
		}
	}
	
	if ( pRoleInfo->ulIdentity >= 0 )
	{
		TCheckNameData Info;
		Info.nProtoId   = s2c_rolecheckname_result;
		Info.ulIdentity = pRoleInfo->ulIdentity;
		Info.nDataLen   = result;
		snprintf(Info.nAccName, sizeof(Info.nAccName), "%s", pRoleInfo->nAccName);
		snprintf(Info.nRoleName, sizeof(Info.nRoleName), "%s", pRoleInfo->nRoleName);
		m_pServer->SendData( m_nIndentity, ( const void * )&Info, sizeof( Info ) );	
	}
	
}

//删除角色
void CClientNode::_DelRole( const void *pData, UINT dataLength )
{
	return;
	/*ASSERT( m_pServer && pData && dataLength );

	TProcessData *pRoleInfo = ( TProcessData * )pData;

	char szRoleName[_NAME_LENGTH];

	int nDataLen = pRoleInfo->nDataLen;
	nDataLen = ( nDataLen > _NAME_LENGTH ) ? _NAME_LENGTH : nDataLen;

	int result = -1;

	if ( nDataLen > 0 )
	{
		memcpy( szRoleName, &pRoleInfo->pDataBuffer[0], nDataLen );
		szRoleName[nDataLen+1] = '\0';

		if ( DeleteRole( szRoleName ) )
		{
			result = 1;
		}
		else
		{
			result = -1;
		}
	}

	if ( pRoleInfo->ulIdentity >= 0 )
	{
		TProcessData Info;
		
		Info.nProtoId = s2c_roleserver_deleterole_result;
		Info.ulIdentity = pRoleInfo->ulIdentity;
		Info.nDataLen = 1;
		Info.pDataBuffer[0] = result;
		
		m_pServer->SendData( m_nIndentity, ( const void * )&Info, sizeof( Info ) );	
	}
	*/
}

//for Relay System and GM [wxb 2003-7-22] GM协议
void CClientNode::_RelayExtend( const void *pData, UINT dataLength )
{
	ASSERT(c2s_extend == *((unsigned char *)pData));
	char* pNewData = ((char *)pData) + 1;
	switch (*((WORD*)pNewData))
	{
	case MAKEWORD(pf_gamemaster, gm_c2s_getrole):
		{
			ASSERT(dataLength == 1 + sizeof(GM_GET_ROLE_DATA_COMMAND));
			GM_GET_ROLE_DATA_COMMAND* pGMData = (GM_GET_ROLE_DATA_COMMAND*)pNewData;
			int nResultBuflen = 0;
			switch(pGMData->wGetID)
			{
			case gm_role_entergame_position:
				{
					ASSERT(0 == pGMData->wLength);
					char* pReturnBuffer = (char*)_alloca(1 + sizeof(GM_GET_ROLE_DATA_SYNC) + sizeof(GM_ROLE_DATA_SUB_ENTER_POS));

					*pReturnBuffer = (char)s2c_extend;

					GM_GET_ROLE_DATA_SYNC* pGMReturn = (GM_GET_ROLE_DATA_SYNC*)(pReturnBuffer + 1);
					strncpy(pGMReturn->AccountName, pGMData->AccountName, sizeof(pGMData->AccountName));
					pGMReturn->AccountName[(sizeof(pGMData->AccountName)) - 1] = '\0';
					pGMReturn->ProtocolFamily = pf_gamemaster;
					pGMReturn->ProtocolType = gm_s2c_getrole;
					pGMReturn->wGetID = gm_role_entergame_position;
					pGMReturn->wLength = sizeof(GM_ROLE_DATA_SUB_ENTER_POS);

					GetRoleInfoForGM(pGMData->wGetID, (char*)(pGMReturn + 1), pGMData->AccountName, nResultBuflen);
					ASSERT(pGMReturn->wLength == nResultBuflen);
					m_pServer->SendData( m_nIndentity, pReturnBuffer, 1 + sizeof(GM_GET_ROLE_DATA_SYNC) + sizeof(GM_ROLE_DATA_SUB_ENTER_POS) );	
				}
				break;
			default:
				ASSERT(0);
				break;
			}
		}
		break;

	case MAKEWORD(pf_gamemaster, gm_c2s_setrole):
		{
			ASSERT(dataLength >= 1 + sizeof(GM_SET_ROLE_DATA_COMMAND));
			GM_SET_ROLE_DATA_COMMAND* pGMData = (GM_SET_ROLE_DATA_COMMAND*)pNewData;
			SetRoleInfoForGM(pGMData->wSetID, (char*)(pGMData + 1), pGMData->AccountName, pGMData->wLength);
		}
		break;
	default:
		ASSERT(0);
		break;
	}
}
////选择角色进入游戏时 获取这个角色的信息
void CClientNode::_GetRoleInfo( const void *pData, UINT dataLength )
{
	ASSERT(m_pServer && pData && dataLength);

	TProcessData *pRoleInfo = (TProcessData *)pData;

	char szRoleName[NAME_LENGHT];
	char szAccName[ACC_LENGTH];

	ZeroMemory(szAccName,sizeof(szAccName));
	ZeroMemory(szRoleName,sizeof(szRoleName));

	int nDataLen = pRoleInfo->nDataLen;
	    nDataLen = (nDataLen > (int)sizeof(szRoleName) - 1) ? (int)sizeof(szRoleName) - 1 : nDataLen;

	CBuffer *pBuffer = m_theAllocator.Allocate();
		
	TProcessData *pRoleData = reinterpret_cast<TProcessData *>(const_cast<BYTE *>(pBuffer->GetBuffer()));

	int nUsedLength = sizeof(TProcessData);

	UINT nIdentity = pRoleInfo->ulIdentity;
	
	pRoleData->nProtoId = s2c_rolehjdkalritopq_getroleinfo_result;	//发往BISHOP 转发给 GS 通知登录
	pRoleData->pDataBuffer[0] = -1;
	pRoleData->nDataLen = 1;	
	pRoleData->ulIdentity = nIdentity;
	pRoleData->uSelServer  = pRoleInfo->uSelServer;
	//strlen(pAccName) =dataLength-nDataLen+1-sizeof(TProcessData) 
	// sizeof(TProcessData) - 1 + nDataLen +strlen(pAccName) =dataLength
	//const UINT lenRN = strlen(pRoleName);//
	//const UINT lenRI = sizeof(TProcessData) - 1 + lenRN;
	if (nDataLen > 0)
	{
		int acclen = (int)(dataLength - nDataLen + 1 - sizeof(TProcessData));
		if (acclen < 0 || acclen > (int)sizeof(szAccName) - 1)
				acclen = acclen < 0 ? 0 : (int)sizeof(szAccName) - 1;
		memcpy(szRoleName,&pRoleInfo->pDataBuffer[0],nDataLen);//角色名
		szRoleName[nDataLen] = '\0';
		memcpy(szAccName,&pRoleInfo->pDataBuffer[nDataLen],acclen);//账号名
		szAccName[acclen] = '\0';

		int result = -1;

		// GetRoleInfo writes the role record into the tail of this node's buffer, so the
		// room it has to be told about is what the buffer holds from the byte the record
		// starts at; the function writes the length it used back through the same argument.
		int nRoleInfoLen = (int)(pBuffer->GetSize()
			- ((char*)&pRoleData->pDataBuffer[1] - (char*)pBuffer->GetBuffer()));
		RoleLoginInfo resInfo;
		memset(&resInfo,0,sizeof(resInfo));
		//为防止同一个角色多次同登陆 检测
		int  nReg=IsRoleLock(szRoleName,&resInfo);
		if (!nReg || nReg==2)  //是否给锁住  没有登陆的菜给数据 如果已经登陆就 提示 空包失败
		    GetRoleInfo(&pRoleData->pDataBuffer[1],szRoleName,nRoleInfoLen,szAccName,pRoleInfo->uSelServer);

		if (!nReg && nRoleInfoLen>0)
		{//正常的
			pRoleData->pDataBuffer[0] = 1;	 //获取成功了
			pRoleData->nDataLen = nRoleInfoLen + 1;
			nUsedLength = sizeof(TProcessData) + nRoleInfoLen;
		}

		if  (nReg==2 && nRoleInfoLen>0)
		{//离线的需要发包到ＧＳ踢帐号下线
			pRoleData->bLeave    = resInfo.m_IsOffLine;
			pRoleData->uSelServer = resInfo.m_RegServerIndex;
            pRoleData->nDataLen  = resInfo.m_GsNumBer;
			nUsedLength = sizeof(TProcessData) + nRoleInfoLen;
		}
		else
			pRoleData->bLeave = false;
	}

	pBuffer->Use(nUsedLength);
	//大包
	m_theSend.AddData(s2c_rolehjdkalritopq_getroleinfo_result, pBuffer->GetBuffer(),nUsedLength,nIdentity);

	CBuffer *pPack = m_theSend.GetHeadPack(s2c_rolehjdkalritopq_getroleinfo_result);

	while (pPack)
	{
		m_pServer->SendData(m_nIndentity, (const void *)pPack->GetBuffer(), pPack->GetUsed());

		SAFE_RELEASE(pPack);

		pPack = m_theSend.GetNextPack(s2c_rolehjdkalritopq_getroleinfo_result);
	}

	m_theSend.DelData(s2c_rolehjdkalritopq_getroleinfo_result);

	SAFE_RELEASE(pPack);

	SAFE_RELEASE(pBuffer);

}
//获取排名数据
void CClientNode::_GetGameStat( const void *pData, UINT dataLength )
{//发送游戏统计数据(By Fellow,2003.7.22)
	ASSERT( m_pServer && pData && dataLength );

#ifdef CONSOLE_DEBUG
	cout << "_GetGameStat::Begin" << endl;
#endif

	//收到的数据包，只有控制字没有数据
	TProcessData *pProData = (TProcessData *)pData;
	UINT nIdentity = pProData->ulIdentity;

	//需要发送的数据包
	CBuffer *pBuffer = m_theAllocator.Allocate();
	TProcessData *pStatData = reinterpret_cast<TProcessData *>(const_cast<BYTE *>(pBuffer->GetBuffer()));

	int nDataLen = sizeof(TGAME_STAT_DATA);
	
	pStatData->nProtoId = s2c_gamestatistic;
	pStatData->ulIdentity = nIdentity;

	pStatData->nDataLen = nDataLen;
	//取得统计数据
	TGAME_STAT_DATA* myGameStatData = (TGAME_STAT_DATA*)pStatData->pDataBuffer;
	GetGameStat(myGameStatData);

	int nUsedLen = sizeof(TProcessData) - 1 + nDataLen;
	pBuffer->Use(nUsedLen);
	//发送
	//大包
	m_theSend.AddData(s2c_gamestatistic_bigpackage,pBuffer->GetBuffer(),nUsedLen);

	CBuffer *pPack = m_theSend.GetHeadPack(s2c_gamestatistic_bigpackage);

	while (pPack)
	{
		m_pServer->SendData(m_nIndentity, (const void *)pPack->GetBuffer(), pPack->GetUsed());

		SAFE_RELEASE( pPack );

		pPack = m_theSend.GetNextPack(s2c_gamestatistic_bigpackage);
	}

	m_theSend.DelData(s2c_gamestatistic_bigpackage);

	SAFE_RELEASE(pPack);

	SAFE_RELEASE(pBuffer);
	
#ifdef CONSOLE_DEBUG
	cout << "_GetGameStat::End" << endl;
#endif
}

/////////////////////////////////////////////////////////////////////////////////

CClientNode::stdRoleLockMap CClientNode::m_csRoleLock;
CCriticalSection	CClientNode::m_csCR;

//邦定这个角色名为这个gs的连接 是否已经登录这个角色
void CClientNode::_LockOrUnlockRole( const void *pData, UINT dataLength )	//强行加解锁
{
	CCriticalSection::Owner lock( CClientNode::m_csCR );
	if (pData && dataLength == sizeof(tagRoleEnterGame))
	{
		tagRoleEnterGame* pRole = (tagRoleEnterGame*)pData;
		char szRole[NAME_LENGHT];
		strncpy(szRole, pRole->Name, NAME_LENGHT - 1);
		szRole[NAME_LENGHT - 1] = 0;
		bool bLock = pRole->bLock;

		if (szRole && szRole[0] != 0)
		{
			if (bLock)  //锁定
			{
				m_csRoleLock[szRole].m_idx = m_nIndentity;
				m_csRoleLock[szRole].m_GsNumBer =pRole->nGsNumber;
				m_csRoleLock[szRole].m_IsOffLine =pRole->Lixian;
				m_csRoleLock[szRole].m_RegServerIndex =pRole->nRegServerIndex;
			}
			else
			{//解锁
				stdRoleLockMap::iterator it = m_csRoleLock.find(szRole);
				if (it != m_csRoleLock.end())
					m_csRoleLock.erase(it);
			}
		}
	}
}

//是否已经连接
int  CClientNode::getRoleLock(char* szRole,RoleLoginInfo *resInfo)
{
	CCriticalSection::Owner lock( CClientNode::m_csCR );
	if (szRole && szRole[0] != 0)
	{
		stdRoleLockMap::iterator it = m_csRoleLock.find(szRole);
		if (it != m_csRoleLock.end() && it->second.m_idx!= -1)
		{//如果存在　但是是离线的
		   if  (resInfo)
		   {
			  resInfo->m_GsNumBer = it->second.m_GsNumBer;
			  resInfo->m_idx = it->second.m_idx;
			  resInfo->m_IsOffLine = it->second.m_IsOffLine;
			  resInfo->m_RegServerIndex = it->second.m_RegServerIndex;
		   }
		   return true;
		}
	}
	return false;
}

//是否已经连接
int  CClientNode::IsRoleLock(char* szRole,RoleLoginInfo *resInfo)
{
	CCriticalSection::Owner lock( CClientNode::m_csCR );
	if (szRole && szRole[0] != 0)
	{
		stdRoleLockMap::iterator it = m_csRoleLock.find(szRole);
		if (it != m_csRoleLock.end() && it->second.m_idx!= -1)
		{//如果存在　但是是离线的
			if  (it->second.m_IsOffLine)
			{
				if  (resInfo)
				{
					resInfo->m_GsNumBer = it->second.m_GsNumBer;
					resInfo->m_idx = it->second.m_idx;
					resInfo->m_IsOffLine = it->second.m_IsOffLine;
					resInfo->m_RegServerIndex = it->second.m_RegServerIndex;
				}
				return 2;
			}
			else
			    return 1;
		}
	}
	return 0;
}
//是否已经是这个连接
bool CClientNode::IsRoleLockBySelf(char* szRole)
{
	CCriticalSection::Owner lock(CClientNode::m_csCR);
	if (szRole && szRole[0] != 0)
	{
		stdRoleLockMap::iterator it = m_csRoleLock.find(szRole);
		if (it != m_csRoleLock.end()/* && it->second == m_nIndentity*/)
			return true;
	}
	return false;
}
//删除这个连接
bool CClientNode::UnlockRoleSelf(char* szRole)
{
	CCriticalSection::Owner lock(CClientNode::m_csCR);
	if (szRole && szRole[0] != 0)
	{
		stdRoleLockMap::iterator it = m_csRoleLock.find(szRole);
		if (it != m_csRoleLock.end() /*&& it->second == m_nIndentity*/)
		{
			m_csRoleLock.erase(it);
			return true;
		}
	}
	return false;
}
//删除全部角色连接
void CClientNode::UnlockAllRole(size_t ID)
{
	CCriticalSection::Owner lock( CClientNode::m_csCR );
	stdRoleLockMap::iterator it = m_csRoleLock.begin();
	while (it != m_csRoleLock.end())
	{
		if (it->second.m_idx == ID)
			it->second.m_idx= -1;
		it++;
	}
}

/////////////////////////////////////////////////////////////////////////////////

void CClientNode::SendTradePairResult(unsigned __int64 uTxId, int nResultCode, const char* szRoleA, const char* szRoleB, unsigned int idA, unsigned int idB)
{
	TTradePairSaveResult res;
	memset(&res, 0, sizeof(res));
	res.nProtoId = s2c_roleserver_savetradepair_result;
	res.uTxId = uTxId;
	res.nResultCode = nResultCode;
	if (szRoleA) strncpy_s(res.szRoleA, sizeof(res.szRoleA), szRoleA, _TRUNCATE);
	if (szRoleB) strncpy_s(res.szRoleB, sizeof(res.szRoleB), szRoleB, _TRUNCATE);
	res.ulIdentityA = idA;
	res.ulIdentityB = idB;
	if (m_pServer)
		m_pServer->SendData(m_nIndentity, (const void *)&res, sizeof(res));
}

void CClientNode::_SaveTradePair( const void *pData, UINT dataLength )
{
	ASSERT(m_pServer && pData && dataLength);
	if (dataLength < sizeof(TTradePairSaveRequest))
		return;

	TTradePairSaveRequest *pReq = (TTradePairSaveRequest *)pData;

	// P0: Reject malformed trade identity and zero generations before any SQLite write.
	// Legacy unversioned role saves are not authorized as atomic two-role trades.
	if (!pReq->uTxId || !pReq->uGenerationA || !pReq->uGenerationB ||
		!pReq->szRoleA[0] || !pReq->szRoleB[0] ||
		!memchr(pReq->szRoleA, 0, sizeof(pReq->szRoleA)) ||
		!memchr(pReq->szRoleB, 0, sizeof(pReq->szRoleB)) ||
		_stricmp(pReq->szRoleA, pReq->szRoleB) == 0)
	{
		SendTradePairResult(pReq->uTxId, -15, NULL, NULL, pReq->ulIdentityA, pReq->ulIdentityB);
		return;
	}

	// 1. Check size overflow & packet truncation
	if (pReq->dwDataLenA > 2000000 || pReq->dwDataLenB > 2000000 ||
		(size_t)pReq->dwDataLenA + (size_t)pReq->dwDataLenB > dataLength ||
		sizeof(TTradePairSaveRequest) - 1 + pReq->dwDataLenA + pReq->dwDataLenB > dataLength)
	{
		SendTradePairResult(pReq->uTxId, -10, pReq->szRoleA, pReq->szRoleB, pReq->ulIdentityA, pReq->ulIdentityB);
		return;
	}

	// 2. Check minimum size of TRoleData blob
	if (pReq->dwDataLenA < sizeof(TRoleBaseInfo) || pReq->dwDataLenB < sizeof(TRoleBaseInfo))
	{
		SendTradePairResult(pReq->uTxId, -10, pReq->szRoleA, pReq->szRoleB, pReq->ulIdentityA, pReq->ulIdentityB);
		return;
	}

	void* pBlobA = (void*)(&pReq->pDataBuffer[0]);
	void* pBlobB = (void*)(&pReq->pDataBuffer[pReq->dwDataLenA]);
	TRoleData* pRoleA = (TRoleData*)pBlobA;
	TRoleData* pRoleB = (TRoleData*)pBlobB;

	// 3. Check character names in request match inner blobs
	if (_strnicmp(pReq->szRoleA, pRoleA->BaseInfo.szName, 32) != 0 ||
		_strnicmp(pReq->szRoleB, pRoleB->BaseInfo.szName, 32) != 0)
	{
		SendTradePairResult(pReq->uTxId, -12, pReq->szRoleA, pReq->szRoleB, pReq->ulIdentityA, pReq->ulIdentityB);
		return;
	}

	// 4. Calculate and verify actual CRC32 of snapshots A and B
	DWORD realCrcA = 0;
	realCrcA = CRC32(realCrcA, (BYTE*)pBlobA, pReq->dwDataLenA);
	DWORD realCrcB = 0;
	realCrcB = CRC32(realCrcB, (BYTE*)pBlobB, pReq->dwDataLenB);

	if (realCrcA != pReq->dwChecksumA || realCrcB != pReq->dwChecksumB)
	{
		SendTradePairResult(pReq->uTxId, -11, pReq->szRoleA, pReq->szRoleB, pReq->ulIdentityA, pReq->ulIdentityB);
		return;
	}

	// 5. Atomic dual commit to SQLite with receipt & duplicate checking
	int nResult = SaveTradePairAtomic(
		pReq->uTxId,
		pReq->szRoleA, pBlobA, pReq->dwDataLenA, pReq->dwChecksumA,
		pReq->szRoleB, pBlobB, pReq->dwDataLenB, pReq->dwChecksumB,
		pReq->uGenerationA, pReq->uGenerationB);

	SendTradePairResult(pReq->uTxId, nResult, pReq->szRoleA, pReq->szRoleB, pReq->ulIdentityA, pReq->ulIdentityB);
}

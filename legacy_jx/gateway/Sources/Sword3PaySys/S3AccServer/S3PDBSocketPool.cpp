//-----------------------------------------//
//                                         //
//  File		: S3PDBSocketPool.cpp	   //
//	Author		: Yang Xiaodong            //
//	Modified	: 8/26/2002                //
//                                         //
//-----------------------------------------//
#pragma warning(disable: 4786)

#include "S3PDBSocketPool.h"
#include "S3PAccount.h"
#include "GlobalFun.h"
#include "TCHAR.h"
#include "S3PDBConnectionPool.h"
#include "..\..\Engine\Src\KEngine.h"

#include "../../Multiserver/Heaven/Interface/IServer.h"
#include "KProtocolDef.h"

//---------------------------------------
//#include "../../../Headers/KProtocolDef.h"
//#include "../../../Headers/KRelayProtocol.h"
//--------------------------------------
#include "string.h"

#define LEFT_TIME		60000	//允许断线后的等待时间
#define PING_TIME		20000	//允许等待客户端Ping的时间
#define SENDPING_TIME	0x7fffffff	//向客户端发送Ping的时间
#define THIS_SERVER_RIGTH "BeHFGQ_RKKUMe~ARGtuGSYvakKbu_YPU"  //192.168 //gioi han ip
S3PDBSocketPool* S3PDBSocketPool::m_pInstance = NULL;

S3PDBSocketPool::S3PDBSocketPool()
{
	m_pServer = NULL;
}

S3PDBSocketPool::~S3PDBSocketPool()
{
	assert(m_pServer == NULL);
	assert(m_clientIDs.size() == 0);
}

S3PDBSocketPool* S3PDBSocketPool::Instance()
{
	if ( NULL == m_pInstance )
		m_pInstance = new S3PDBSocketPool;

	return m_pInstance;
}

void S3PDBSocketPool::ReleaseInstance()
{
	delete m_pInstance;
	m_pInstance = NULL;
}

//
HANDLE S3PDBSocketPool::Start(IServer* pServer, int nMax)
{
	assert(pServer);
	if (NULL == m_pServer)
	{
		m_pServer = pServer;
		pServer->AddRef();
		Lock();
		DWORD nNow = GetTickCount();  //获取服务器的运行时间 微妙
		for (int i = 0; i < nMax; ++i)//最大线程数目
		{
			KGatewayDataProcess* p = new KGatewayDataProcess();
			p->Start(m_pServer, nNow);
			m_clientIDs.push_back(p); //压入这个线程指针
		}
		Unlock();
	}

	return 0;
}

BOOL S3PDBSocketPool::Stop()
{
	BOOL bRet = TRUE;
	if (bRet && m_pServer)
	{
		Lock();
		GatewayArray::iterator i = m_clientIDs.begin();
		while (i != m_clientIDs.end())
		{
			(*i)->Stop();
			delete (*i);
			i++;
		}
		Unlock();
		m_clientIDs.clear();

		m_pServer->Release();
		m_pServer = NULL;
	}

	return bRet;
}

BOOL S3PDBSocketPool::SendData(unsigned long uID, const void * const pData, const size_t &datalength)
{
	if (m_pServer == NULL || pData == NULL || datalength <= 0)
		return FALSE;

	BOOL bRet = FALSE;
	try
	{
		m_pServer->SendData(uID, pData, datalength);
		bRet = TRUE;
	}
	catch(...)
	{
		gTrace("Failed to feed back( S3PDBSocketPool::SendData )");
		bRet = FALSE;
	}
	return bRet;
}

//#include "../../MultiServer/Common/Macro.h"
//#include "../../MultiServer/Common/Buffer.h"
//
//using OnlineGameLib::Win32::CBuffer;
//CBuffer::Allocator	m_theGlobalAllocator( 1024 * 64, 10 );

BOOL S3PDBSocketPool::CustomSend(unsigned long uID, char nProtocol, KAccountHead* pSend)
{
	if (m_pServer == NULL)
		return FALSE;

	BOOL bRet = FALSE;
	try
	{
		assert(pSend);
		pSend->Version = ACCOUNT_CURRENT_VERSION;
		char send[MAX_PATH + 1];
		// Size is a WORD the caller sets and the assert below is gone in every build
		// that defines NDEBUG, which is all of them: the room has to be tested where
		// the copy reads it
		int nSize = pSend->Size;
		if (nSize > (int)sizeof(send) - 2)
			return FALSE;
		assert(pSend->Size < MAX_PATH);
		send[0] = nProtocol;
		memcpy(send + 1, pSend, nSize);
		//send[0] = pf_normal;
		//send[1] = nProtocol;
		m_pServer->SendData(uID, send, nSize + 1);
					
//		CBuffer *pBuffer = m_theGlobalAllocator.Allocate();
//		
//		BYTE *pData = const_cast< BYTE * >(pBuffer->GetBuffer());
//		*pData = nProtocol;
//		memcpy(pData + 1, pSend, pSend->Size);
//		pBuffer->Use(pSend->Size + 1);
//		
//		m_pServer->PreparePackSink();
//		m_pServer->PackDataToClient(uID, pData, pSend->Size + 1);
//		m_pServer->SendPackToClient();
//
//		SAFE_RELEASE(pBuffer);

		bRet = TRUE;
	}
	catch(...)
	{
		gTrace("Failed to feed back( S3PDBSocketPool::CustomSend )");
		bRet = FALSE;
	}
	return bRet;
}

//解锁所有线程
void S3PDBSocketPool::unlockAllUserClient()
{
	GatewayArray::iterator i = m_clientIDs.begin();
	int n = 0;
	gTrace("--Start unlock All User Client !--");
	while (i != m_clientIDs.end())
	{
		if ((*i))
		{
			if  ((*i)->IsLocked())
			{
				n++;
				(*i)->Unlock();
				gTrace("--Unlock [%d] Connect client succes !--",n);
			}
		}
			i++;
	}
	gTrace("--unlock All User Client succes !--");
}

//增加一个客户端链接
BOOL S3PDBSocketPool::AddUserClientID(unsigned long uID)
{
	assert(m_pServer);
	//const char* pInfo = 
	char pInfo[128]={0};
	m_pServer->GetClientInfo(uID,pInfo,sizeof(pInfo));  //点分十进制的IP 包含端口
	unsigned long Address = inet_addr(pInfo); //inet_addr()的功能是将一个点分十进制的IP转换成一个长整数型数
	//int offset = 0;
	//while(*(pInfo + offset) != ':')
	//	offset++;
	//offset += 2;	//skip : and blank
	//short Port = atoi(pInfo + offset);         //提取出端口
	int  Port=0;
	int m_port=0;
	std::string ntempInfo=pInfo;
	int len=ntempInfo.length();
	std::string::size_type pos = ntempInfo.find(":");
	if (pos != std::string::npos)
	{
		std::string _strPort = ntempInfo.substr(pos+1, len);
		 m_port = Port = atoi(_strPort.c_str());  
	}

	//const char* pColon = strchr(pInfo, ':');
	//if (pColon)
	//    Port = atoi(pColon + 1);  //加为了去除冒号  //端口

	bool bError = false;

	GatewayArray::iterator i = m_clientIDs.begin();
	int n = 0;
	while (i != m_clientIDs.end())
	{
		if ((*i)->ConnectErrorWork(uID, Address, Port))
		{//检测是否 已经在连接中的了
			bError = true;
			break;
		}
		i++;
		n++;
	}

	if (bError)
	{//如果是已经连接的
		i = m_clientIDs.begin();
		n = 0;
		while (i != m_clientIDs.end())
		{
			if ((*i)->ConnectFreeForErrorWork(uID, Address, Port))
			{
				gTrace("ErrorAdd Gateway(%s) Client %d ! -- At %d", pInfo, uID, n);
				return TRUE;	
			}
			i++;
			n++;
		}
	}
	else
	{
		i = m_clientIDs.begin();
		n = 0;
		while (i != m_clientIDs.end())
		{
			if ((*i)->ConnectAgainWork(uID, Address, Port))
			{//检测正在重连的IP是否和数据库的相同
				gTrace("ReAdd Gateway(%s) Client %d ! -- At %d", pInfo, uID, n);
				return TRUE;	
			}
			i++;
			n++;
		}

		i = m_clientIDs.begin();
		n = 0;
		while (i != m_clientIDs.end())
		{
			if ((*i)->ConnectFreeWork(uID, Address, Port))//检测 IP 是否和数据库中的相同
			{//如果是空闲的
				gTrace("--Add gateway[%s] Client [%d] ! -- At: %d, m_port: %d ! --", pInfo,n,uID,m_port);
				return TRUE;	
			}
			i++;
			n++;
		}
	}

	return TRUE;
}
//移除客户端链接
BOOL S3PDBSocketPool::RemoveUserClientID(unsigned long uID)
{
	GatewayArray::iterator i = m_clientIDs.begin();
	int n = 0;
	while (i != m_clientIDs.end ())
	{
		if ((*i)->OutofWork(uID))
		{
			//gTrace("--断开 Gateway 第(%d)个客户端,链接编号:%d ! --",n,uID);
			break;
		}
		i++;
		n++;
	}
	return TRUE;
}
//显示所有客户端的链接
BOOL S3PDBSocketPool::ShowAllClientInfo()
{
	//if (IsLocked())
	//	return FALSE;

	//Lock();
	gTrace("---Bat dau hien thi cac thong tin ket noi !---");
	BOOL b = FALSE;
	GatewayArray::iterator i = m_clientIDs.begin();
	int n = 0;
	while (i != m_clientIDs.end())
	{
		if (*i)
		{
			int nStatus = (*i)->GetStatus();
			switch (nStatus)
			{
			case KGatewayDataProcess::gdp_work:
				{//已经连接上的
					in_addr add;
					add.s_addr = (*i)->m_Address;
					gTrace("bishop client %d: %s(%s:%d) is working", (*i)->m_nGameID, (*i)->m_ServerName, inet_ntoa(in_addr(add)), (*i)->m_Port);
				}
				break;
			case KGatewayDataProcess::gdp_again:
				{//重连
					in_addr add;
					add.s_addr = (*i)->m_Address;
					gTrace("bishop client  %d: %s(%s:%d) is waiting", (*i)->m_nGameID, (*i)->m_ServerName, inet_ntoa(in_addr(add)), (*i)->m_Port);
				}
				break;
			case KGatewayDataProcess::gdp_free:
				{//空闲的
					gTrace("bishop client  is free at %d", n);
				}
			break;
			case KGatewayDataProcess::gdp_verify:
				{
					gTrace("bishop client  is verify at %d", n);
				}
			break;
			case KGatewayDataProcess::gdp_verifyagain:
				{
					gTrace("bishop client  is verifyagain at %d", n);
				}
			break;
			case KGatewayDataProcess::gdp_errorconnect:
				{//连接错误的
					gTrace("bishop client  is error at %d", n);
				}
			break;
			default:
				{
					gTrace("bishop client  is unknown at %d", n);
				}
			break;
			}
		}
		i++;
		n++;
	}
	gTrace("---Da hoan tat hien thi thong tin cac ket noi!---");
	b = TRUE;
	//Unlock();
	return b;
}

///////////////////////网关协议线程///////////////////////////////////////////////

KGatewayDataProcess::KGatewayDataProcess()
{
	m_nGameID = 0;
	m_ServerName[0] = 0;

	m_pServer = NULL;

	m_pConn = NULL;

	memset(ProcessFunc, 0, sizeof(ProcessFunc));

	ProcessFunc[c2s_accountlogin - c2s_accountbegin]  = &KGatewayDataProcess::ProAccountLogin;     //账号检测登陆 1
	ProcessFunc[c2s_gamelogin_dkiqopjgurnmvbgc - c2s_accountbegin]     = &KGatewayDataProcess::ProGameLogin;        //进入游戏 2
	ProcessFunc[c2s_accountlogout - c2s_accountbegin] = &KGatewayDataProcess::ProAccountLogout;   //退出游戏 3
	ProcessFunc[c2s_gatewayverify - c2s_accountbegin] = NULL;               // 4      
	
	m_Status = gdp_free;
	m_nStatusTime = 0;
	m_nLeftStatusTime = 0;

	m_LastPingTime = 0;
	m_nLeftPingTime = 0;
	m_LastSendPingTime = 0;

	//m_PayExtPiont = 0;
}

KGatewayDataProcess::~KGatewayDataProcess()
{
	assert(m_pServer == NULL);
}

//
void KGatewayDataProcess::SetPayExtPiont()
{
	    TCHAR szExePath[MAX_PATH + 1];
	    KPIGetExePath( szExePath, MAX_PATH );  //获取软件运行的路径
	    TCHAR szINIPath[MAX_PATH + 1];
	   _tcscpy(szINIPath, szExePath);
	   _tcscat(szINIPath, "acc_setup.ini");
	  
	    TCHAR szPort[32];
	    KPIGetPrivateProfileString("Local",
		"PayExtPiont",
		"10",
		szPort,
		31,
		szINIPath);

        m_PayExtPiont = atoi(szPort);
}

void KGatewayDataProcess::AutoTime()
{
	if (IsError())
	{//关闭客户端
		m_pServer->ShutdownClient(m_nConnectID);
		return;
	}

	DWORD nNow = GetTickCount();  //获取服务器运行的时间

	BLACKLIST::iterator i = m_UserNames.begin();
	while (i != m_UserNames.end())
	{
		if ((nNow - i->second) / 1000 >= 1)	//每次循环处理1个已经超时的用户
		{
			m_UserNames.erase(i);
			break;
		}
		i++;
	}

	if (m_nLeftStatusTime > 0 && (nNow - m_nStatusTime > m_nLeftStatusTime) && IsWorkAgain())
	{
		S3PDBConVBC* pConn = GetDB(0);
		if (pConn)
		{
			S3PAccount::ElapseAll(pConn, m_nGameID);	//只扣钱,不解锁
			SetStatus(gdp_free);
			return;
		}
	}

	if (m_nLeftPingTime > 0 && (nNow - m_LastPingTime) >= m_nLeftPingTime && IsWork())
	{//关闭客户端
		m_pServer->ShutdownClient(m_nConnectID);
	}

	if ((nNow - m_LastSendPingTime) >= SENDPING_TIME && IsWork())
	{
		SendPing(nNow);
		m_LastSendPingTime = nNow;
	}
}

/*
BOOL KGatewayDataProcess::ProAutoTime(S3PDBConVBC* pConn)
{
	BOOL bRet = FALSE;
	DWORD dwMin = 0;
	dwMin = 1800;	//默认30分钟
	char szMessage[128];
	if (dwMin > 0)
	{
		AccountTimeList TimeList;
		if (S3PAccount::GetAccountsTime(pConn, m_nGameID, dwMin, TimeList) == ACTION_SUCCESS)
		{
			AccountTimeList::iterator i = TimeList.begin();
			while (i != TimeList.end())
			{
				if (i->nTime == 0 && i->nVipType==0)		//踢人
				{//月卡
					char szS[200];
					sprintf(szS, "GlobalExecute(\"dw KickOutAccount([[%s]])\")", i->Account);
					ExecuteAction(szS);//踢人
					gTrace("Account(%s) is KickOff!", i->Account);
				}
				else if (i->nVipType==0)	//催促充值
				{
					int nLen = 0;
					if (i->nTime >= 60)
						nLen = sprintf(szMessage, "您在游戏中还可以停留[%d]分钟, 请赶快充值!", i->nTime / 60);
					else
						nLen = sprintf(szMessage, "您在游戏中还可以停留[%d]秒, 请赶快充值!", i->nTime);
					SendSystemInfo(m_nClientID, i->Account, "系统GM", szMessage, nLen);
				}
				
				if (i->nExtPoint == 0 && i->nVipType==1)		//踢人
				{//点卡
					char szS[200];
					sprintf(szS, "GlobalExecute(\"dw KickOutAccount([[%s]])\")", i->Account);
					ExecuteAction(szS);//踢人
					gTrace("Account(%s) is KickOff!", i->Account);
				}
				else if (i->nVipType==1)	//催促充值
				{
					int nLen = 0;
					if (i->nExtPoint <= 10)
					{
						nLen = sprintf(szMessage, "您在游戏中点卡即将消耗完毕, 请赶快充值!");
						SendSystemInfo(m_nClientID, i->Account, "系统GM", szMessage, nLen);
					}
				}
				
				i++;
			}
			bRet = TRUE;
		}
	}
	return bRet;
}
*/

BOOL KGatewayDataProcess::CheckConnectAddress(DWORD Address)
{
	S3PDBConVBC* pConn = GetDB(0);
	if (pConn)
		return S3PAccount::CheckAddress(pConn, Address, m_Port) == ACTION_SUCCESS;
	return FALSE;
}

#pragma pack(push, 1)			
			typedef struct
			{
				BYTE			ProtocolType;
				UINT			m_dwTime;
			} PING_COMMAND;
#pragma pack(pop)

void KGatewayDataProcess::SendPing(DWORD dwTime)
{   
	//新建一个 套字节 连接BISHOP
	S3PDBSocketPool* pSocket = S3PDBSocketPool::Instance();
	PING_COMMAND PingCmd;
	PingCmd.ProtocolType = s2c_ping;
	PingCmd.m_dwTime = dwTime;
	pSocket->SendData(m_nConnectID, &PingCmd, sizeof(PING_COMMAND));
}
//无限循环 获取客户端数据 Bishop 
void KGatewayDataProcess::ProcessClientData(const void * pData, DWORD dwDataSize)
{
	if (pData && dwDataSize > 0)
	{	
		char nProtocol = *((char*)pData);
		if (nProtocol == c2s_ping)
		{
			DWORD nSecond = m_LastPingTime;
			m_LastPingTime = GetTickCount();
			PING_COMMAND* p = (PING_COMMAND*)pData;
			SendPing(p->m_dwTime);
			in_addr ar;
			ar.s_addr = m_Address;
			gTrace("Gateway %s>>(%s): Ping %d(s)", m_ServerName, inet_ntoa(ar), (m_LastPingTime - nSecond) / 1000);
			return;
		}
	
		S3PDBConVBC* pConn = GetDB(10);
		ProcessData(pConn, pData, dwDataSize);    //数据处理线程
	}
}
//无限循环处理客户端BIshop数据线程
DWORD KGatewayDataProcess::ProcessData(S3PDBConVBC* pConn, const void* pData, DWORD dwDataSize)
{
	BOOL bRet = FALSE;
	if (pConn == NULL || pData == NULL || dwDataSize <= 0)
	{
		return bRet;
	}

	char nProtocol = *((char*)pData);

	S3PDBSocketPool* pSocket = S3PDBSocketPool::Instance();

	BOOL bStopConn = FALSE;

	if (nProtocol == c2s_gatewayverify || nProtocol == c2s_gatewayverifyagain)
	{
		if (GetStatus() != gdp_verify && GetStatus() != gdp_verifyagain)
		{
			KAccountUserReturn aReturn;
			aReturn.Size = sizeof(KAccountUserReturn);
			aReturn.Type = AccountUserReturn;
			aReturn.Operate = ((KAccountUserLoginInfo*)(((char*)pData) + 1))->Operate;
			aReturn.nReturn = E_ACCOUNT_ACCESSDENIED;
			aReturn.Account[0] = 0;
			pSocket->CustomSend(m_nConnectID, s2c_gatewayverify, &aReturn);

			Sleep(1000);	// 确保返回网关的消息在连接断开前到达
			
			bRet = FALSE;
			bStopConn = TRUE;  //关闭客户端
		}
		else
		{
			if (nProtocol == c2s_gatewayverifyagain && IsVerifyTimeout())
			{
				KAccountUserReturn aReturn;
				aReturn.Size = sizeof(KAccountUserReturn);
				aReturn.Type = AccountUserReturn;
				aReturn.Operate = ((KAccountUserLoginInfo*)(((char*)pData) + 1))->Operate;
				aReturn.nReturn = E_ACCOUNT_ACCESSDENIED;
				aReturn.Account[0] = 0;
				pSocket->CustomSend(m_nConnectID, s2c_gatewayverify, &aReturn);

				Sleep(1000);	// 确保返回网关的消息在连接断开前到达

				bRet = FALSE;
				bStopConn = TRUE; //关闭客户端
			}
			else
			{////接收  bishop 发来的数据包   用于bishop登录
				//gTrace("开始接受 bishop包 ");
				bRet = ProGetwayVerify(pConn, ((char*)pData) + 1, dwDataSize - 1);
				if (bRet)
				{
					//gTrace("开始接受 bishop包 成功 ");
					if (nProtocol == c2s_gatewayverify)	//gatewayverify成功后要根据状态清理现场
					{
						if (GetStatus() == gdp_verifyagain)  //重连状态口
						{
							S3PAccount::ElapseAll(pConn, m_nGameID);
							gTrace("ReConnect ElapseAll OK!");
						}
						S3PAccount::UnlockAll(pConn, m_nGameID);  //所有用户解除绑定账号
						gTrace("Gateway unlock OK!");
					}

					if (nProtocol == c2s_gatewayverify)
						gTrace("Gateway Connect OK!");
					else
						gTrace("Gateway Connect OK!");

					if (SetStatus(gdp_work))
					{
						m_nLeftStatusTime = 0;
						m_nStatusTime = 0;
						m_LastPingTime = GetTickCount();
						m_nLeftPingTime = PING_TIME;
						m_LastSendPingTime = m_LastPingTime;
					}
				}
				else
				{
					Sleep(1000);	// 确保返回网关的消息在连接断开前到达
					bStopConn = TRUE;  //关闭客户端
				}
			}
		}
	}
	else
	{//处理登录协议
		if (GetStatus() == gdp_work)
		{
			if (nProtocol >= c2s_accountbegin && nProtocol < c2s_multiserverbegin &&
				ProcessFunc[nProtocol - c2s_accountbegin])
			{
				bRet = (this->*ProcessFunc[nProtocol - c2s_accountbegin])(pConn, ((char*)pData) + 1, dwDataSize - 1);
			}
			else
				bRet = ProUndefine(((char*)pData) + 1, dwDataSize - 1);
		}
		else
		{
			bRet = FALSE;
		}
	}
	
	if (bStopConn)
	{
		m_pServer->ShutdownClient(m_nConnectID);  //关闭客户端
	}

	return bRet;
}
//有账号开始登陆了
BOOL KGatewayDataProcess::ProAccountLogin(S3PDBConVBC* pConn, const IBYTE* lpData, const DWORD dwDataSize)
{
	BOOL bRet = FALSE;
	if (NULL != lpData && dwDataSize == sizeof(KAccountUserLoginInfo))
	{
		KAccountUserLoginInfo pkt;
		memset(&pkt, 0, sizeof(pkt));
		pkt = *(const KAccountUserLoginInfo*)lpData;
		KAccountUserLoginInfo* pAccInfo = &pkt;
		pkt.Account[LOGIN_USER_ACCOUNT_MAX_LEN - 1] = '\0';
		pkt.Password[LOGIN_USER_PASSWORD_MAX_LEN - 1] = '\0';
		if (pAccInfo->Version == ACCOUNT_CURRENT_VERSION &&
			(pAccInfo->Type == AccountUserLoginInfo || pAccInfo->Type == AccountUserVerify)
			)
		{
			KAccountUserReturnExt aReturn;
			aReturn.Size      = sizeof(KAccountUserReturnExt);
			aReturn.Type      = AccountUserReturnEx;
			aReturn.Operate   = pAccInfo->Operate;
			aReturn.nLeftTime = aReturn.nExtPoint = aReturn.nVipType = aReturn.nGameLeftTime = 0; //剩余的时间 等于 剩余的扩展点 
			memcpy(aReturn.Account, pAccInfo->Account, LOGIN_USER_ACCOUNT_MAX_LEN);

			if (m_UserNames.find(*((UserName*)pAccInfo->Account)) == m_UserNames.end())	//不在黑名单上
			{
				if (pAccInfo->Type == AccountUserLoginInfo)
					aReturn.nReturn = S3PAccount::Login(pConn, pAccInfo->Account, pAccInfo->Password, m_nGameID, aReturn.nExtPoint, aReturn.nLeftTime,aReturn.nVipType,aReturn.nGameLeftTime,m_PayExtPiont);  //剩余扩展点  // 获取数据可以的信息		
				//扣除点卡 断线 在这里限制的了
				else  //删除角色 验证二级秘密
					aReturn.nReturn = S3PAccount::VerifyUserModifyPassword(pConn, m_nGameID, pAccInfo->Account, pAccInfo->Password);
			}
			else
				aReturn.nReturn = E_ACCOUNT_OR_PASSWORD;
/*
            if  (aReturn.nVipType==0)
			{   
				if (aReturn.nLeftTime<=0)
					aReturn.nReturn = E_ACCOUNT_NODEPOSIT;
			}
			else if  (aReturn.nVipType==1)
			{   
				if (aReturn.nExtPoint<=0)
					aReturn.nReturn = E_ACCOUNT_NODEPOSIT;
			}
*/

			//aReturn.nReturn = E_ACCOUNT_OR_PASSWORD;
			
			S3PDBSocketPool* pSocket = S3PDBSocketPool::Instance();
			char* szDetail = NULL;
			in_addr ar;
			ar.s_addr = m_Address;
			char suc[112];
			if (aReturn.nReturn == ACTION_SUCCESS)
			{
				bRet = TRUE;
				snprintf(suc, 112, " nVipType:%d,nLeftTime:%ld,nExtPoint:%ld Login Success!)", aReturn.nVipType,aReturn.nLeftTime,aReturn.nExtPoint);
				szDetail = suc;
				m_UserNames.erase(*((UserName*)pAccInfo->Account));  //删除黑名单
			}
			else
			{
				if (aReturn.nReturn == E_ACCOUNT_OR_PASSWORD)
				{
					szDetail = " Failed ! --- Name, Password";
					m_UserNames[*((UserName*)pAccInfo->Account)] = GetTickCount();
				}
				else if (aReturn.nReturn == E_ACCOUNT_EXIST)
					szDetail = " Failed ! --- Already login";
				else if (aReturn.nReturn == E_ACCOUNT_NODEPOSIT)
					szDetail = " Failed ! --- No Money";
				else if (aReturn.nReturn == E_ADDRESS_OR_PORT)
					szDetail = " Failed ! --- IP Address, Port";
				else if (aReturn.nReturn == E_ACCOUNT_FREEZE)
					szDetail = " Failed ! --- Account Freeze";
				else
					szDetail = " Failed !";
			}
			if (pAccInfo->Type == AccountUserLoginInfo)
				gTrace("Gateway %s(%s): User %s Login%s", m_ServerName, inet_ntoa(ar), pAccInfo->Account, szDetail);
			else //删除角色
				gTrace("Gateway %s(%s): User %s SecondPassword%s %s %u", m_ServerName, inet_ntoa(ar), pAccInfo->Account, szDetail,pAccInfo->Password,m_nGameID);
			//把结果发送给bishop
			pSocket->CustomSend(m_nConnectID,s2c_accountfdfsglogin,&aReturn);
		}
		else
			gTrace("Protocol %d (Login) data is error", c2s_accountlogin);
	}
	else
		gTrace("Protocol %d (Login) data is error", c2s_accountlogin);
	return bRet;
}
//账号检测完---开始登陆游戏
BOOL KGatewayDataProcess::ProGameLogin(S3PDBConVBC* pConn, const IBYTE* lpData, const DWORD dwDataSize)
{
	BOOL bRet = FALSE;
	if (NULL != lpData && dwDataSize == sizeof(KAccountUser))
	{
		KAccountUser pkt;
		memset(&pkt, 0, sizeof(pkt));
		pkt = *(const KAccountUser*)lpData;
		KAccountUser* pAccInfo = &pkt;
		pkt.Account[LOGIN_USER_ACCOUNT_MAX_LEN - 1] = '\0';
		if (pAccInfo->Version == ACCOUNT_CURRENT_VERSION &&
			pAccInfo->Type == AccountUser)
		{
			KAccountUserReturn aReturn;
			aReturn.Size = sizeof(KAccountUserReturn);
			aReturn.Type = AccountUserReturn;
			aReturn.Operate = pAccInfo->Operate;
			memcpy(aReturn.Account, pAccInfo->Account, LOGIN_USER_ACCOUNT_MAX_LEN);
			aReturn.nReturn = S3PAccount::LoginGame(pConn, m_nGameID, pAccInfo->Account);  

			S3PDBSocketPool* pSocket = S3PDBSocketPool::Instance();

			char* szDetail = NULL;
			in_addr ar;
			ar.s_addr = m_Address;
			if (aReturn.nReturn == ACTION_SUCCESS)
			{
				bRet = TRUE;
				szDetail = " Success !!";
			}
			else
			{
				if (aReturn.nReturn == E_ACCOUNT_OR_PASSWORD)
					szDetail = " Failed! --- ACCOUNT_OR_PASSWORD";
				else if (aReturn.nReturn == E_ACCOUNT_ACCESSDENIED)
				{
					bRet = TRUE;
					aReturn.nReturn = ACTION_SUCCESS;
					szDetail = " Success !! --- Login first";
				}
				else
					szDetail = " Failed!";
			}

			gTrace("Gateway %s(%s): User %s Freeze Account:%s", m_ServerName, inet_ntoa(ar), pAccInfo->Account, szDetail);
			
			pSocket->CustomSend(m_nConnectID, s2c_gamelogin, &aReturn);
		}
		else
			gTrace("Protocol %d(c2s_gamelogin) data is error", c2s_gamelogin_dkiqopjgurnmvbgc);
	}
	else
		gTrace("Protocol %d(c2s_gamelogin) data is error", c2s_gamelogin_dkiqopjgurnmvbgc);
	return bRet;
}

//退出游戏时的操作
BOOL KGatewayDataProcess::ProAccountLogout(S3PDBConVBC* pConn, const IBYTE* lpData, const DWORD dwDataSize)
{
	BOOL bRet = FALSE;
	if (NULL != lpData && dwDataSize == sizeof(KAccountUserLogout))
	{
		KAccountUserLogout pkt;
		memset(&pkt, 0, sizeof(pkt));
		pkt = *(const KAccountUserLogout*)lpData;
		KAccountUserLogout* pAccInfo = &pkt;
		pkt.Account[LOGIN_USER_ACCOUNT_MAX_LEN - 1] = '\0';
		if (pAccInfo->Version == ACCOUNT_CURRENT_VERSION &&
			pAccInfo->Type == AccountUserLogout)
		{
			KAccountUserReturn aReturn;
			aReturn.Size = sizeof(KAccountUserReturn);
			aReturn.Type = AccountUserReturn;
			aReturn.Operate = pAccInfo->Operate;
			memcpy(aReturn.Account, pAccInfo->Account, LOGIN_USER_ACCOUNT_MAX_LEN);
			
			aReturn.nReturn = S3PAccount::Logout(pConn, m_nGameID, pAccInfo->Account, pAccInfo->nExtPoint,pAccInfo->nGameExtTime); //退出游戏 扣除扩展点

			S3PDBSocketPool* pSocket = S3PDBSocketPool::Instance();
			char* szDetail = NULL;
			in_addr ar;
			ar.s_addr = m_Address;
			char suc[128]={0};
			if (aReturn.nReturn == ACTION_SUCCESS)
			{
				bRet = TRUE;
				sprintf(suc, "  Success ! (nExtPoint: %ld, nGameExtTime: %ld)", pAccInfo->nExtPoint,pAccInfo->nGameExtTime);
				szDetail = suc;
			}
			else
			{
				if (aReturn.nReturn == E_ACCOUNT_OR_PASSWORD)
					szDetail = " Logout Failed ! --- ACCOUNT_OR_PASSWORD"; 
				else if (aReturn.nReturn == E_ACCOUNT_ACCESSDENIED)
					szDetail = " Failed ! --- Login first";
				else
					szDetail = " Failed !";
			}

			gTrace("Gateway %s(%s): User %s Logout:%s", m_ServerName, inet_ntoa(ar), pAccInfo->Account, szDetail);
			
			pSocket->CustomSend(m_nConnectID, s2c_accountlogout, &aReturn);	   //发往s3 server
		}
		else
			gTrace("Protocol %d data is error", c2s_accountlogout);
	}
	else
		gTrace("Protocol %d data is error", c2s_accountlogout);
	return bRet;
}
//接收bishop 发来的数据包 检测数据库的用户名和密码  用于bishop登录
BOOL KGatewayDataProcess::ProGetwayVerify(S3PDBConVBC* pConn, const IBYTE* lpData, const DWORD dwDataSize)
{
	BOOL bRet = FALSE;
	if (NULL != lpData && dwDataSize == sizeof(KServerAccountUserLoginInfo))
	{
		KServerAccountUserLoginInfo pkt;
		memset(&pkt, 0, sizeof(pkt));
		pkt = *(const KServerAccountUserLoginInfo*)lpData;
		KServerAccountUserLoginInfo* pAccInfo = &pkt;
		pkt.Account[LOGIN_USER_ACCOUNT_MAX_LEN - 1] = '\0';
		pkt.Password[LOGIN_USER_PASSWORD_MAX_LEN - 1] = '\0';
		if (pAccInfo->Version == ACCOUNT_CURRENT_VERSION &&
			pAccInfo->Type == ServerAccountUserLoginInfo)
		{
			KAccountUserReturn aReturn;
			aReturn.Size = sizeof(KAccountUserReturn);
			aReturn.Type = AccountUserReturn;
			aReturn.Operate = pAccInfo->Operate;
			memcpy(aReturn.Account, pAccInfo->Account, LOGIN_USER_ACCOUNT_MAX_LEN);
			DWORD nGameID = 0;
			aReturn.nReturn = S3PAccount::ServerLogin(pConn, pAccInfo->Account, pAccInfo->Password, m_Address, m_Port, pAccInfo->MacAddress, nGameID);
			
			S3PDBSocketPool* pSocket = S3PDBSocketPool::Instance();
			in_addr ar;
			ar.s_addr = m_Address;   //ip地址  IP登陆限制  bishop链接paysys的IP 
			//------------------------单机设置 //gioi han ip
			/*PassChecksum nJmm;
			char nMnn[64] = {0};
			nJmm.SimplyDecryptPassword(nMnn, THIS_SERVER_RIGTH);
			if (!strstr(inet_ntoa(ar), nMnn))
                aReturn.nReturn = E_ACCOUNT_EXIST;*/
			//-------------------------
			if (aReturn.nReturn == ACTION_SUCCESS)
			{
				assert(nGameID > 0);
				strncpy(m_ServerName, pAccInfo->Account, LOGIN_USER_ACCOUNT_MAX_LEN);
				m_ServerName[(LOGIN_USER_ACCOUNT_MAX_LEN) - 1] = '\0';
				m_nGameID = nGameID;
				bRet = TRUE;
			}

			char szmac[15]={0};
			sprintf(szmac, "%02X%02X-%02X%02X-%02X%02X", pAccInfo->MacAddress[0], pAccInfo->MacAddress[1], pAccInfo->MacAddress[2], pAccInfo->MacAddress[3], pAccInfo->MacAddress[4], pAccInfo->MacAddress[5]);
			szmac[14] = 0;
			if (bRet)
				gTrace("Gateway %s IP:%s Mac:%s: Verify OK !", m_ServerName, inet_ntoa(ar), szmac);
			else
			{
				if (aReturn.nReturn == E_ACCOUNT_EXIST)
					gTrace("Gateway %s(%s)%s: Verifye OK !", m_ServerName, inet_ntoa(ar), szmac);
				else if (aReturn.nReturn == E_ACCOUNT_OR_PASSWORD)
					gTrace("Gateway %s(%s)%s: Name, Password !", m_ServerName, inet_ntoa(ar), szmac);
				else if (aReturn.nReturn == E_ADDRESS_OR_PORT)
					gTrace("Gateway %s(%s)%s: Address, Port !", m_ServerName, inet_ntoa(ar), szmac);
			}
		    //返回给bishop
			pSocket->CustomSend(m_nConnectID, s2c_gatewayverify, &aReturn);
		}
		else
			gTrace("Protocol %d data is error", c2s_gatewayverify);
	}
	else
		gTrace("Protocol %d data is error", c2s_gatewayverify);
	return bRet;
}

BOOL KGatewayDataProcess::ProUndefine(const IBYTE* lpData, const DWORD dwSize)
{
	BOOL bRet = FALSE;
	return bRet;
}

S3PDBConVBC* KGatewayDataProcess::GetDB(DWORD nSleep)
{
	if (!m_pConn)
	{
		S3PDBConnectionPool* pDB = S3PDBConnectionPool::Instance();
		if (nSleep)
		{
			while (m_pConn == NULL)
			{//锁定一个连接 必须获取一个连接
				pDB->RemoveDBCon(&m_pConn);
				Sleep(nSleep);
			}
		}
		else
			pDB->RemoveDBCon(&m_pConn);
	}

	return m_pConn;
}
//线程入口
DWORD KGatewayDataProcess::Main(LPVOID lpParam)
{
	assert(m_hStop);
	assert(m_pServer);
	DWORD dwRet = 0;
	DWORD LastTime = GetTickCount();
	
	S3PDBConnectionPool* pDB = S3PDBConnectionPool::Instance();

	SetPayExtPiont();   //设置需要被扣的扩展点

//无限循环
	while (1)
	{
		DWORD dwResult = KPIWaitForSingleObject(m_hStop, 0);

		if (dwResult == 1)
			continue;
			//break;
		else if (dwResult == 2)  //超时
		{//2WAIT_TIMEOUT  0 WAIT_FAILED 1 WAIT_OBJECT_0
			AutoTime();  //检测账号情况

			size_t datalength = 0;

			const void *pData = NULL;

			if (IsWantData())
				pData = m_pServer->GetPackFromClient(m_nConnectID, datalength);

			if (pData && 0 != datalength)
			{//接受客户端BISHOP 数据
				///gTrace("开始接受 bishop包AAAAAAAAAAAAAAA ");
				ProcessClientData(pData, datalength);
				LastTime = GetTickCount();
			}
			else
			{//空闲的时候 就处理
				if (GetTickCount() - LastTime >= 1000)
				{
					if (m_pConn)
					{//释放这个数据库的连接
						if (pDB->ReturnDBCon(m_pConn))
							m_pConn = NULL;
					}
				}
				Sleep(1);
			}
		}
	}

	if ((IsWork() || IsWorkAgain()) && GetDB(0))
	{
		S3PAccount::ElapseAll(m_pConn, m_nGameID);	//设置会员剩余时间
	}

	if (m_pConn)
	{//释放这个连接
		if (pDB->ReturnDBCon(m_pConn))
			m_pConn = NULL;
	}

	return dwRet;
}

HANDLE KGatewayDataProcess::Start(IServer* pServer, DWORD nNow)
{
	assert(pServer);
	if (NULL == m_pServer)
	{
		m_pServer = pServer;
		pServer->AddRef();
	}

	m_Status = gdp_free;
	m_nStatusTime = nNow;
	m_nLeftStatusTime = LEFT_TIME;

	return KThread::Start();
}

BOOL KGatewayDataProcess::Stop()
{
	BOOL bRet = KThread::Stop();
	if (bRet)
	{
		if (m_pServer)
		{
			m_pServer->Release();
			m_pServer = NULL;
		}
	}

	return bRet;
}

BOOL KGatewayDataProcess::ConnectErrorWork(unsigned long nID, unsigned long Address, int Port)
{
	BOOL bRet = FALSE;
	//Lock();

	if (m_Status == gdp_verify ||	//只有这几个状态下的m_Address有效
		m_Status == gdp_work ||
		m_Status == gdp_verifyagain ||
		m_Status == gdp_errorconnect
		)
	{
		if (m_Address == Address)	//有已经连接的Address
		{
			bRet = TRUE;
		}
		goto exit0;
	}

exit0:
	//Unlock();
	return bRet;
}


BOOL KGatewayDataProcess::ConnectFreeWork(unsigned long nID, unsigned long Address, int Port)
{
	BOOL bRet = FALSE;
	//Lock();

	if (m_Status == gdp_free)
	{
		if (CheckConnectAddress(Address))
		{
			m_nConnectID = nID;
			m_Address    = Address;
			m_Port       = Port;
			m_Status     = gdp_verify;
			bRet         = TRUE;
		}
		goto exit0;
	}

exit0:
	//Unlock();
	return bRet;
}

BOOL KGatewayDataProcess::ConnectAgainWork(unsigned long nID, unsigned long Address, int Port)
{
	BOOL bRet = FALSE;
	//Lock();

	if (m_Status == gdp_again)
	{
		if (m_Address == Address && CheckConnectAddress(Address))
		{
			m_nConnectID = nID;
			m_Address = Address;
			m_Port = Port;
			m_Status = gdp_verifyagain;
			bRet = TRUE;
		}
		goto exit0;
	}

exit0:
//	Unlock();
	return bRet;
}

BOOL KGatewayDataProcess::ConnectFreeForErrorWork(unsigned long nID, unsigned long Address, int Port)
{
	BOOL bRet = FALSE;
	//Lock();

	if (m_Status == gdp_free)
	{
		m_nConnectID = nID;
		m_Address = Address;
		m_Port = Port;
		m_Status = gdp_errorconnect;
		bRet = TRUE;
		goto exit0;
	}

exit0:
	//Unlock();
	return bRet;
}

BOOL KGatewayDataProcess::OutofWork(unsigned long nID)
{
	BOOL bRet = FALSE;
//	Lock();
	if (m_nConnectID == nID)
	{
		m_nConnectID = -1;

		if (m_Status == gdp_work)
		{
			m_Status = gdp_again;
			m_nLeftStatusTime = LEFT_TIME;
			m_nStatusTime = GetTickCount();	//将剩余时间设上,为了客户端能重连上
			
			m_LastPingTime = 0;
			m_nLeftPingTime = 0;
			m_LastSendPingTime = 0;
			bRet = TRUE;
			goto exit0;
		}

		if (m_Status == gdp_verifyagain)
		{
			m_Status = gdp_again;
			//不改剩余时间,以继续计时
			bRet = TRUE;
			goto exit0;
		}

		if (m_Status == gdp_verify)
		{
			m_Status = gdp_free;
			//不改剩余时间,以继续计时
			bRet = TRUE;
			goto exit0;
		}

		if (m_Status == gdp_errorconnect)
		{
			m_Status = gdp_free;
			m_nLeftStatusTime = 0;
			m_nStatusTime = 0;
			m_LastPingTime = 0;
			m_nLeftPingTime = 0;
			m_LastSendPingTime = 0;
			bRet = TRUE;
			goto exit0;
		}
	}

exit0:
	//Unlock();
	return bRet;
}

BOOL KGatewayDataProcess::IsWork()
{
	BOOL bRet = FALSE;
	//Lock();
	if (m_Status == gdp_work)
	{
		bRet = TRUE;
	}
	//Unlock();
	return bRet;
}

BOOL KGatewayDataProcess::IsWorkAgain()
{
	BOOL bRet = FALSE;
	//Lock();
	if (m_Status == gdp_again)
	{
		bRet = TRUE;
	}
//	Unlock();
	return bRet;
}

BOOL KGatewayDataProcess::IsWantData()
{
	BOOL bRet = FALSE;
	//Lock();
	if (m_Status == gdp_work ||
		m_Status == gdp_again ||
		m_Status == gdp_verify ||
		m_Status == gdp_verifyagain)
	{
		bRet = TRUE;
	}
//	Unlock();
	return bRet;
}

BOOL KGatewayDataProcess::IsError()
{
	BOOL bRet = FALSE;
//	Lock();
	if (m_Status == gdp_errorconnect)
	{
		bRet = TRUE;
	}
	//Unlock();
	return bRet;
}

int KGatewayDataProcess::GetStatus()
{
	int nStatus = -1;
	//Lock();
	nStatus = m_Status;
	//Unlock();
	return nStatus;
}

bool KGatewayDataProcess::SetStatus(int nNews)
{
	bool bRet = false;
	Lock();
	if (m_Status == gdp_free &&
		nNews == gdp_verify)
	{
		m_Status = nNews;
		bRet = true;
	}
	else if (m_Status == gdp_verify &&
		(nNews == gdp_work ||
		 nNews == gdp_free))
	{
		m_Status = nNews;
		bRet = true;
	}
	else if (m_Status == gdp_work &&
		nNews == gdp_again)
	{
		m_Status = nNews;
		bRet = true;
	}
	else if (m_Status == gdp_again &&
		(nNews == gdp_verifyagain ||
		 nNews == gdp_free))
	{
		m_Status = nNews;
		bRet = true;
	}
	else if (m_Status == gdp_verifyagain &&
		(nNews == gdp_work ||
		 nNews == gdp_again))
	{
		m_Status = nNews;
		bRet = true;
	}
	else if (m_Status == gdp_errorconnect &&
		nNews == gdp_free)
	{
		m_Status = nNews;
		bRet = true;
	}
	Unlock();

	return bRet;
}

bool KGatewayDataProcess::IsVerifyTimeout()
{
	bool bRet = true;
	//Lock();
	if (m_Status == gdp_verify || m_Status == gdp_verifyagain)
	{
		if ((GetTickCount() - m_nStatusTime) <= m_nLeftStatusTime)
		{	//正确状态的有限时间内,才叫不超时
			bRet = false;
		}
	}
	//Unlock();
	return bRet;
}
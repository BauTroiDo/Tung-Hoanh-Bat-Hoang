//-----------------------------------------//
//                                         //
//  File		: S3PAccount.cpp		   //
//	Author		: Yang Xiaodong            //
//	Modified	: 8/29/2002                //
//                                         //
//-----------------------------------------//
#pragma warning(disable: 4786)


#include "S3PAccount.h"
#include "S3PDBConVBC.h"
#include "S3PResultVBC.h"
#include "COMUtil.h"
#include "GlobalFun.h"

#include <time.h>

//#include <afx.h>


#include <iostream>

// Escapes a value for interpolation into a quoted SQL literal: a quote and a
// backslash are both doubled, which MSSQL and MySQL honour inside '...'.  Four
// buffers rotate because one sprintf() may take several values as arguments.
// The cap matters as much as the doubling: the destinations here are fixed
// char strSQL[MAX_PATH] / char strSQL[200], so a bounded value keeps the query
// inside them, and a pair is never split so the result cannot end on a lone
// escape that would re-open the literal.
enum { SQL_ESC_SLOTS = 4, SQL_ESC_MAX = 128 };
static thread_local char g_szSqlEscBuffer[SQL_ESC_SLOTS][SQL_ESC_MAX];
static thread_local int  g_nSqlEscSlot = 0;

static const char *SqlEscapeLiteral(const char *pszValue)
{
	const char *psz = ( NULL != pszValue ) ? pszValue : "";
	char *pszOut = g_szSqlEscBuffer[g_nSqlEscSlot];
	g_nSqlEscSlot = ( g_nSqlEscSlot + 1 ) % SQL_ESC_SLOTS;

	size_t nOut = 0;
	for ( size_t i = 0; psz[i] != '\0' && nOut + 2 < SQL_ESC_MAX; ++i )
	{
		if ( psz[i] == '\'' || psz[i] == '\\' )
			pszOut[nOut++] = psz[i];
		pszOut[nOut++] = psz[i];
	}
	pszOut[nOut] = '\0';
	return pszOut;
}


S3PAccount::S3PAccount()
{
}

S3PAccount::~S3PAccount()
{
}

DWORD GetGMID();
//登陆时 检测 剩余时间和 扩展点
int S3PAccount::Login(S3PDBConVBC* pConn, const char* strAccName, const char* strPassword, DWORD ClientID, DWORD& nExtPoint, DWORD& nLeftTime,WORD& nVipType,DWORD& nGameLeftTime,int nPayExtPiont)
{
	int iRet = ACTION_FAILED;

	if (NULL == pConn)
	{
		return iRet;
	}

	char strSQL[MAX_PATH]={0};
	long iType = -1;
	long iLeft = 0;
	long iExp  = 0;
	DWORD iGameTime=0;

	sprintf(strSQL,"select iClientID from Account_info where (cAccName = '%s') and (cPassword COLLATE Chinese_PRC_CS_AS = '%s')", SqlEscapeLiteral(strAccName), SqlEscapeLiteral(strPassword));
	S3PResultVBC* pResult = NULL;
	if (pConn->QuerySql(strSQL, &pResult))
	{
		if (pResult->num_rows() <= 0)
			iRet = E_ACCOUNT_OR_PASSWORD;   //账号 或密码错误
		else
		{
			_variant_t clientID = 0L;

			pResult->get_field_data(0, &clientID, sizeof(_variant_t));

			if (clientID.vt == VT_NULL || clientID.lVal == 0)
			{		
				if (GetLeftSecondsOfDeposit(pConn, strAccName, iLeft, iExp,iType,iGameTime) == ACTION_SUCCESS)	//最小剩余时间30分钟   // 获取数据可以的信息
				{
				   nVipType  = iType; //会员类型
				   nExtPoint = iExp;  //赋值给客户端 剩余的点卡
				   nLeftTime = iLeft; //剩余的时间 
				   nGameLeftTime = 	iGameTime;
				  // iRet = ACTION_SUCCESS;
				   //---------------------------------------------------------------	
				   if (nVipType == 0 && nLeftTime > 0) 
				   {//月卡
					   sprintf(strSQL, "update Account_info set Account_info.iClientID = %d, Account_info.dLoginDate = null where (Account_info.cAccName = '%s')", 0, SqlEscapeLiteral(strAccName));//ClientID
					   if (pConn->Do(strSQL))
					   { 
						   iRet = ACTION_SUCCESS;
					   } 
				   }
				   else if (nVipType == 1 && nExtPoint > 0)
				   {//点卡
					   sprintf(strSQL, "update Account_info set Account_info.iClientID = %d, Account_info.dLoginDate = null where (Account_info.cAccName = '%s')", 0, SqlEscapeLiteral(strAccName)); //
					   if (pConn->Do(strSQL))
					   {  
						   iRet = ACTION_SUCCESS;
					   }  			   
				   }
				   else
					   iRet = E_ACCOUNT_NODEPOSIT;  //点卡不足

				}
				else
				{ 
				   iRet = E_ACCOUNT_NODEPOSIT; //点卡不足
				} 
			}
			else
			{
				iRet = (GetGMID() == clientID.lVal) ? E_ACCOUNT_FREEZE : E_ACCOUNT_EXIST;
			}
		}
	}
///------------只能放在登陆后查询：现有的扩展点 和 -剩余时间-----------------------------
   if (nVipType == 1)
   {//点卡 会员
		if  (nExtPoint > nPayExtPiont)
			 sprintf(strSQL, "update Account_Habitus set Account_Habitus.nExtPoint = Account_Habitus.nExtPoint-%d where (Account_Habitus.cAccName = '%s')",nPayExtPiont,SqlEscapeLiteral(strAccName));
		else
		{
             sprintf(strSQL, "update Account_Habitus set Account_Habitus.nExtPoint = 0 where (Account_Habitus.cAccName = '%s')", SqlEscapeLiteral(strAccName));
		     iRet = E_ACCOUNT_NODEPOSIT;  //点卡不足
		}	
	 	pConn->Do(strSQL);
   }

   //if (nVipType==-1)
   //	iRet = E_ACCOUNT_NODEPOSIT;

///-----------------------------------------------------------------------------------------
	if (iRet == ACTION_SUCCESS)
	{//设置网络号

	    sprintf(strSQL, "update Account_Habitus set Account_Habitus.iClientID = 0 where (Account_Habitus.cAccName = '%s')", SqlEscapeLiteral(strAccName));//  ClientID  	
		pConn->Do(strSQL); 
	}


	if (pResult)
		pResult->unuse();

	return iRet;
}
//进入游戏
int S3PAccount::LoginGame(S3PDBConVBC* pConn, DWORD ClientID, const char* strAccName)
{
	int iRet = ACTION_FAILED;
	if (NULL == pConn)
	{
		return iRet;
	}
	DWORD NewClientID = 0;
	      iRet = GetAccountGameID(pConn,strAccName,NewClientID);  //获取客户端连接号
	if (iRet == ACTION_SUCCESS)  //=1
	{
		if (NewClientID == ClientID)
		{
			char strSQL[MAX_PATH];
			//dLoginDate is null 已经Login之后dLoginDate才会null   //这里会出现 #3092错误 提示服务器人满  卡号
			//sprintf(strSQL, "update Account_info set Account_info.dLoginDate = getdate() where (Account_info.cAccName = '%s') and (Account_info.iClientID = %d)", strAccName, ClientID);
			sprintf(strSQL, "update Account_info set Account_info.dLoginDate = getdate() where (Account_info.cAccName = '%s')", SqlEscapeLiteral(strAccName));
			if (pConn->Do(strSQL))    //更新数据库出错 出现#3092错误
			{//执行数据库语句
				iRet = ACTION_SUCCESS;
			}
			else
			{
				iRet = ACTION_FAILED;
				
			}
		}
		else
			iRet = E_ACCOUNT_ACCESSDENIED;
	}
	return iRet;
}
//退出游戏 扣除 扩展点 和 更新剩余时间
int S3PAccount::Logout(S3PDBConVBC* pConn, DWORD ClientID, const char* strAccName, LONG nExtPoint,LONG nGameExtTime)
{
	int iRet = ACTION_FAILED;
	if (NULL == pConn)
	{
		return iRet;
	}
    char strSQL[MAX_PATH];
//------------查询：现有的扩展点 和 -剩余时间-----------------------------
	LONG  dwSecord   = 0;
	LONG  dwExePiont = 0;
	LONG  dwVipType  = 0;
	DWORD dwGameTime = 0;
    //int   iQRet      = ACTION_FAILED;  //返回服务器忙
	GetLeftSecondsOfDeposit(pConn,strAccName,dwSecord,dwExePiont,dwVipType,dwGameTime);// 获取剩余
	//if (iQRet==ACTION_SUCCESS)
		if (dwVipType==0)
		    if  (nGameExtTime!=0)
			{//月卡时间更新
				sprintf(strSQL, "select datepart(yyyy,dEndDate),datepart(mm,dEndDate),datepart(dd,dEndDate), datepart(hh,dEndDate),datepart(mi,dEndDate),datepart(ss,dEndDate) from Account_Habitus where cAccName = '%s'",SqlEscapeLiteral(strAccName));// and (Account_Habitus.dEndDate is no null)
				//;datediff(second,null, Account_Habitus.dEndDate)
				//'1970-01-01 00:00:00'  date_format
				S3PResultVBC* pResult = NULL;
				if (pConn->QuerySql(strSQL, &pResult))
				{
					if (pResult->num_rows() > 0)
					{
                        _variant_t dpartYear;
						_variant_t dpartMon;
						_variant_t dpartDay;
						_variant_t dpartHour;
						_variant_t dpartMin;
						_variant_t dpartSec;
						pResult->get_field_data(0, &dpartYear, sizeof(_variant_t));  //距离结束时间还有多少秒
						pResult->get_field_data(1, &dpartMon, sizeof(_variant_t));
						pResult->get_field_data(2, &dpartDay, sizeof(_variant_t));
						pResult->get_field_data(3, &dpartHour, sizeof(_variant_t));
						pResult->get_field_data(4, &dpartMin, sizeof(_variant_t));
						pResult->get_field_data(5, &dpartSec, sizeof(_variant_t));

                        int nYear=2017,nMon=1,nDay=1,nHour=0,nMin=0,nSec=0;
                        //--原来的时间
                        nYear    = dpartYear.lVal;
					    nMon	 = dpartMon.lVal;
					    nDay	 = dpartDay.lVal;
					    nHour	 = dpartHour.lVal;
					    nMin	 = dpartMin.lVal;
					    nSec	 = dpartSec.lVal;
					   
					    char nTimes[80]={0};
						struct tm t;
						time_t nTime;	          //long 类型
						struct tm* ptms;
						//------------------------
						t.tm_year=nYear-1900;
						t.tm_mon=nMon-1;
						t.tm_mday=nDay;
						t.tm_hour=nHour;
						t.tm_min=nMin;
						t.tm_sec=nSec;
						t.tm_isdst=0;
						nTime=mktime(&t);
						nTime +=nGameExtTime;
						ptms = localtime(&nTime);  //gmtime(&nTime);//把秒数时间转换为本地时间
						strftime(nTimes,sizeof(nTimes),"%Y-%m-%d %H:%M:%S",ptms); // I 为 12小时制 H 为24小时制
						// MFC类
						/*CTime T;
						T=CTime(nYear,nMon,nDay,nHour,nMin,nSec);  //设置时间为2008年8月8号8时8分8秒 //最高为 2037年
						nTime = T.GetTime();      //获取秒数
						//printf("转换后的时间:%u\n",nTime);
						nTime +=nGameExtTime;
						struct tm* ptms;
						ptms = localtime(&nTime);  //gmtime(&nTime);//把秒数时间转换为本地时间
						strftime(nTimes,sizeof(nTimes),"%Y-%m-%d %H:%M:%S",ptms);
						*/
						//char nTimes[26];
						//sprintf(nTimes,"%04d-%02d-%02d %02d:%02d:%02d",nYear,nMon,nDay,nHour,nMin,nSec);      //最新的时间
						//strftime(nTimes,sizeof(nTimes), "%Y-%m-%d %H:%M:%S",ptm);  //格式化时间
						snprintf(strSQL, sizeof(strSQL), "update Account_Habitus set Account_Habitus.dEndDate = '%s',Account_Habitus.iLeftSecond = datediff(second, getdate(), '%s') where (Account_Habitus.cAccName = '%s')",SqlEscapeLiteral(nTimes),SqlEscapeLiteral(nTimes),SqlEscapeLiteral(strAccName)); //Account_Habitus.iLeftSecond - (datediff(second, Account_Habitus.dBeginDate, getdate()))
			            pConn->Do(strSQL); 
					}
				}

				if (pResult)
				{
					pResult->unuse();		
			        pResult = NULL;
				}
			}

	snprintf(strSQL, sizeof(strSQL), "update Account_Habitus set Account_Habitus.iClientID = 0,Account_Habitus.iLeftSecond = datediff(second, getdate(), Account_Habitus.dEndDate) where (datediff(second, getdate(), Account_Habitus.dEndDate) >= 0) and (Account_Habitus.cAccName = '%s')",SqlEscapeLiteral(strAccName)); //Account_Habitus.iLeftSecond - (datediff(second, Account_Habitus.dBeginDate, getdate()))
	pConn->Do(strSQL);

	if (nExtPoint != 0)  
	{//load xu MSSQL
		snprintf(strSQL, sizeof(strSQL), "update View_AccountMoney set nExtPoint = CASE WHEN nExtPoint - %d >= 0 THEN nExtPoint - %d WHEN nExtPoint - %d < 0 THEN 0 END where cAccName = '%s'",
			nExtPoint, nExtPoint, nExtPoint, SqlEscapeLiteral(strAccName));
		gTrace("%s",strSQL);
		pConn->Do(strSQL);	//扣附加点,即使被的冻结帐户
		//拆成两句是因为执行的SQL有长度限制
	}

	/*
	if (dwVipType==1)
		if (nExtPoint!=0) //点卡更新
		{//扣除在游戏中使用了或增加的 点卡并解除绑定账号
			 dwExePiont+=nExtPoint;
			 if (dwExePiont< 0)
				 dwExePiont = 0;

			 //sprintf(strSQL, "update Account_Habitus set Account_Habitus.iClientID = 0,Account_Habitus.nExtPoint = CASE WHEN Account_Habitus.nExtPoint + %d >= 0 THEN Account_Habitus.nExtPoint + %d WHEN Account_Habitus.nExtPoint + %d < 0 THEN 0 END  where (Account_Habitus.cAccName = '%s')",nExtPoint, nExtPoint, nExtPoint,strAccName);
			 sprintf(strSQL,"update Account_Habitus set Account_Habitus.iClientID = 0,Account_Habitus.nExtPoint =%d  where (Account_Habitus.cAccName = '%s')",dwExePiont,strAccName);
			 pConn->Do(strSQL);
		}
*/
//-----------------------------------------------------------------------------------------
/*
	if (nExtPoint != 0)
	{//这个点 是从游戏中 传过来的要增加 或减少的点卡数
	//	    sprintf(strSQL, "update Account_Habitus set nExtPoint = CASE WHEN nExtPoint - %d >= 0 THEN nExtPoint - %d WHEN nExtPoint - %d < 0 THEN 0 END where (datediff(second, getdate(), dEndDate) >= 0) and (iClientID = %d or iClientID = %d) and (cAccName = '%s') and (dLoginDate is not null)",
	//		nExtPoint, nExtPoint, nExtPoint, ClientID, GetGMID(), strAccName);
			
			char LstrSQL[MAX_PATH];
			sprintf(LstrSQL, "update Account_Habitus set Account_Habitus.nExtPoint = CASE WHEN Account_Habitus.nExtPoint - %d >= 0 THEN Account_Habitus.nExtPoint - %d WHEN Account_Habitus.nExtPoint - %d < 0 THEN 0 END where (datediff(second, getdate(), Account_Habitus.dEndDate) >= 0) and (Account_Habitus.cAccName = '%s')",nExtPoint, nExtPoint, nExtPoint, strAccName);
		    pConn->Do(LstrSQL);	//扣附加点,即使被的冻结帐户
	}
*/
/*	DWORD NewClientID = 0;
	      iRet = GetAccountGameID(pConn, strAccName, NewClientID);

	if (iRet == ACTION_SUCCESS)
	{
		if (NewClientID == ClientID)
		{//解除绑定账号
			sprintf(strSQL, "update Account_info set Account_info.iClientID = 0, Account_info.dLogoutDate = getdate() where (Account_info.cAccName = '%s') and (Account_info.iClientID = %d)",strAccName, NewClientID); //设置退出时间
			
			if (pConn->Do(strSQL))
			{
				iRet = ACTION_SUCCESS;
			}
			else
				iRet = ACTION_FAILED;

		}
		else if (NewClientID == GetGMID())
		{
			iRet = ACTION_SUCCESS;
		}
		else
			iRet = E_ACCOUNT_ACCESSDENIED;
	}
*/
	//---------------------

	sprintf(strSQL, "update Account_info set Account_info.iClientID = 0, Account_info.dLogoutDate = getdate() where Account_info.cAccName = '%s'",SqlEscapeLiteral(strAccName)); //设置退出时间
	
	if (pConn->Do(strSQL))
	{
		iRet = ACTION_SUCCESS;
	}
	else
		iRet = ACTION_FAILED;


	return iRet;
}
//扣除时间  更新时间
int S3PAccount::ElapseTime(S3PDBConVBC* pConn, DWORD ClientID, const char* strAccName, const DWORD dwDecSecond)
{
	int iRet = ACTION_FAILED;
	if (NULL == pConn)
	{
		return iRet;
	}
	DWORD dwSecord   = 0;
	DWORD dwExePiont = 0;
	WORD  dwVipType  = 0;

	iRet = QueryTimeAndPiont(pConn, ClientID, strAccName, dwSecord,dwExePiont,dwVipType);  //获取已经用了的时间

	if (iRet == ACTION_SUCCESS && dwVipType==0)
	{//月卡
		dwSecord = min(dwDecSecond, dwSecord);
		char strSQL[MAX_PATH];
		snprintf(strSQL, sizeof(strSQL), "update Account_Habitus set Account_Habitus.iLeftSecond = Account_Habitus.iLeftSecond - %d where (Account_Habitus.cAccName = '%s') and (datediff(second, getdate(), Account_Habitus.dEndDate) >=0) and (Account_Habitus.iClientID = %d)", dwSecord, SqlEscapeLiteral(strAccName), ClientID);
		if (pConn->Do(strSQL))
		{
			iRet = ACTION_SUCCESS;
		}
		else
			iRet = ACTION_FAILED;
	}
	else if (iRet == ACTION_SUCCESS && dwVipType==1)
	{//点卡
			iRet = ACTION_SUCCESS;
	}

	return iRet;
}

//查询剩余时间 和扩展点
int S3PAccount::QueryTimeAndPiont(S3PDBConVBC* pConn, DWORD ClientID, const char* strAccName, DWORD& dwSecond,DWORD& dwExePiont,WORD& dwVipType)
{
	dwSecond = 0;
	int iRet = ACTION_FAILED;  //服务器忙
	if (NULL == pConn)
	{
		return iRet;
	}
	DWORD NewClientID = 0;
	iRet = GetAccountGameID(pConn, strAccName, NewClientID);  //如果是登陆的
	if (iRet == ACTION_SUCCESS)
	{
		if (NewClientID == ClientID)
		{
			long iLeftS = 0;
			long iExp   = 0;
			long iType  = 0;
			DWORD iGameTime = 0;
			GetLeftSecondsOfDeposit(pConn, strAccName, iLeftS, iExp,iType,iGameTime); // 获取数据可以的信息
			    dwVipType = iType;

            if (iType==0 && iLeftS > 0)
			{
			    iRet = ACTION_SUCCESS;
				dwSecond = iLeftS;  //取最大值
			}
		    else if (iType==1 && iExp > 0)
			{
                iRet = ACTION_SUCCESS;
                dwExePiont = iExp;  //取最大值
			}
			else
              iRet = E_ACCOUNT_ACCESSDENIED;
	    
		}
		else
			iRet = E_ACCOUNT_ACCESSDENIED;
	}
	return iRet;
}
//账号登陆的服务器IP
int S3PAccount::ServerLogin(S3PDBConVBC* pConn, const char* strAccName, const char* strPassword, const DWORD Address, const int Port, const BYTE Mac[6], unsigned long& nGameID)
{
	nGameID = 0;
	int iRet = ACTION_FAILED;
	if (NULL == pConn)
	{
		return iRet;
	}
	char strSQL[MAX_PATH];
	sprintf(strSQL, "select cIP, iPort, iid, cMemo from ServerList where (cServerName = '%s') and (cPassword COLLATE Chinese_PRC_CS_AS = '%s')", SqlEscapeLiteral(strAccName), SqlEscapeLiteral(strPassword));
	S3PResultVBC* pResult = NULL;
	if (pConn->QuerySql(strSQL, &pResult))
	{
		if (pResult->num_rows() <= 0)
			iRet = E_ACCOUNT_OR_PASSWORD;
		else
		{
			iRet = E_ADDRESS_OR_PORT;
			if ((Address & 0x0000FFFF) == 0x0000a8c0)
			{//如果是本地IP
				_variant_t clientid = 0L;
				pResult->get_field_data(2, &clientid, sizeof(_variant_t));  //服务器编号
				nGameID = clientid.lVal;
				iRet = ACTION_SUCCESS;	//Local network not check ip
			}
			else
			{
				_variant_t vaddr;
				_variant_t vaMac;
				if (pResult->get_field_data(0, &vaddr, sizeof(_variant_t)) &&  //IP
					pResult->get_field_data(3, &vaMac, sizeof(_variant_t)) &&  //网卡mac
					vaddr.vt == VT_BSTR && vaMac.vt == VT_BSTR)
				{
					DWORD addr = inet_addr((const char *)(_bstr_t)vaddr);  //IP地址
					char szmac[15];
					sprintf(szmac, "%02X%02X-%02X%02X-%02X%02X", Mac[0], Mac[1], Mac[2], Mac[3], Mac[4], Mac[5]);
					szmac[14] = 0;
					if (addr == Address && strcmpi((const char *)(_bstr_t)vaMac, szmac) == 0)
					{ //IP地址相同 和 网卡MAC相同
						_variant_t gameid = 0L;
						pResult->get_field_data(2, &gameid, sizeof(_variant_t));
						nGameID = gameid.lVal;
						iRet = ACTION_SUCCESS;   //登陆成功
					}
				}
			}
		}
	}
	
	if (pResult)
		pResult->unuse();

	return iRet;
}
//获取服务器的id号 对应 serverlist数据库表中iid
int S3PAccount::GetServerID(S3PDBConVBC* pConn, const char* strAccName, unsigned long& nGameID)
{
	nGameID = 0;
	int iRet = ACTION_FAILED;
	if (NULL == pConn)
	{
		return iRet;
	}
	char strSQL[MAX_PATH];
	sprintf(strSQL, "select iid from ServerList where (cServerName = '%s')", SqlEscapeLiteral(strAccName));
	S3PResultVBC* pResult = NULL;
	if (pConn->QuerySql(strSQL, &pResult))
	{
		if (pResult->num_rows() <= 0)
			iRet = E_ACCOUNT_OR_PASSWORD;
		else
		{
			_variant_t clientid = 0L;
			pResult->get_field_data(0, &clientid, sizeof(_variant_t));
			nGameID = clientid.lVal;
			iRet = ACTION_SUCCESS;	//Local network not check ip
		}
	}
	
	if (pResult)
		pResult->unuse();

	return iRet;
}
//检测IP地址
int S3PAccount::CheckAddress(S3PDBConVBC* pConn, const DWORD Address, const int Port)
{
	int iRet = ACTION_FAILED;
	if (NULL == pConn)
	{
		return iRet;
	}
	if ((Address & 0x0000FFFF) == 0x0000a8c0) //本地IP不检测IP
		iRet = ACTION_SUCCESS;	//Local network not check ip  
	else
	{
		char strSQL[MAX_PATH];
		in_addr add;
		add.s_addr = Address;   //检测服务器数据库列表是否是这个IP
		sprintf(strSQL, "select iid from ServerList where (cIP = '%s')", SqlEscapeLiteral(inet_ntoa(add)));
		S3PResultVBC* pResult = NULL;
		if (pConn->QuerySql(strSQL, &pResult))
		{
			if (pResult->num_rows() <= 0)
				iRet = E_ADDRESS_OR_PORT;  //错误的IP地址或端口
			else
				iRet = ACTION_SUCCESS;
		}
		if (pResult)
			pResult->unuse();
	}

	return iRet;
}
//开锁 更新数据库(客户端如提示：该账号正在使用 ) 卡号
int S3PAccount::UnlockAll(S3PDBConVBC* pConn, DWORD ClientID)
{
	int iRet = ACTION_FAILED;
	if (NULL == pConn)
	{
		return iRet;
	}
	char strSQL[MAX_PATH];
	sprintf(strSQL, "update Account_info set Account_info.iClientID = 0, Account_info.dLogoutDate = getdate() where (Account_info.iClientID = %d)", ClientID);
	pConn->Do(strSQL);
	
	return ACTION_SUCCESS;
}
//扣实时更新时间
int S3PAccount::ElapseAll(S3PDBConVBC* pConn, DWORD ClientID) //剩下的时间
{
	int iRet = ACTION_FAILED;
	if (NULL == pConn)
	{
		return iRet;
	}
	char strSQL[288];
	snprintf(strSQL, 288, "update Account_Habitus set Account_Habitus.iLeftSecond = datediff(second, getdate(), Account_Habitus.dEndDate) where (datediff(second, getdate(), Account_Habitus.dEndDate) >= 0) and (Account_Habitus.iClientID = %d) and (Account_Habitus.dBeginDate is not null)", ClientID);
	pConn->Do(strSQL); //执行语句

	return ACTION_SUCCESS;
}

//获取客户端的 链接ID
int S3PAccount::GetAccountGameID(S3PDBConVBC* pConn, const char* strAccName, DWORD& ClientID)
{
	int iRet = ACTION_FAILED;
	if (NULL == pConn)
	{
		return iRet;
	}
	char strSQL[MAX_PATH];
	sprintf(strSQL, "select iClientID from Account_info where (cAccName = '%s')", SqlEscapeLiteral(strAccName)); //账号
	S3PResultVBC* pResult = NULL;
	if (pConn->QuerySql(strSQL, &pResult))
	{
		if (pResult->num_rows() <= 0)
			iRet = E_ACCOUNT_OR_PASSWORD;
		else
		{
			_variant_t lID = 0L;
			pResult->get_field_data(0, &lID, sizeof(_variant_t));
			ClientID = lID.lVal;
			iRet = ACTION_SUCCESS;
		}
	}
	
	if (pResult)
		pResult->unuse();

	return iRet;
}
//数据库查询  获取剩余时间
int S3PAccount::GetLeftSecondsOfDeposit(S3PDBConVBC* pConn,
										const char* strAccName,
										long& liLeft,
										long& liExp,long& liType,DWORD& liGameTime)  // 获取数据可以的信息
{
	int iRet = ACTION_FAILED;
	if (NULL == pConn)
	{
		return iRet;
	}
	char strSQL[MAX_PATH];
	sprintf(strSQL, "select iFlag,datediff(second, getdate(), dEndDate), iLeftSecond, nExtPoint from Account_Habitus where (cAccname = '%s')", SqlEscapeLiteral(strAccName));  //View_AccountMoney原来的表 and (nExtPoint > 0)
	S3PResultVBC* pResult = NULL;
	if (pConn->QuerySql(strSQL, &pResult)) //查询数据库 发回指向数据的指针
	{
		if (pResult->num_rows() <= 0)    //如果满足 select 里的条件 提取后的数目
			iRet = E_ACCOUNT_NODEPOSIT;  //点卡不足 赋值返回值
		else
		{
			_variant_t Flag;
			_variant_t diffDate;
			_variant_t left;
			_variant_t extPoint;

			pResult->get_field_data(0, &Flag, sizeof(_variant_t));      //会员类型
			pResult->get_field_data(1, &diffDate, sizeof(_variant_t));  //距离结束时间还有多少秒
			pResult->get_field_data(2, &left, sizeof(_variant_t));      //剩余时间
			pResult->get_field_data(3, &extPoint, sizeof(_variant_t));  //剩余点卡

			if (/*diffDate.vt == VT_I4 && */diffDate.lVal > 0)	        //月卡有效
			{//返回剩余时间
				liLeft = diffDate.lVal;
				if  (left.lVal<0)
					liGameTime = 0;
				else
				    liGameTime = left.lVal; 
			}
			else
			{
			  liLeft = 0;           //剩余时间
			  liGameTime = 0;
			  sprintf(strSQL, "update Account_Habitus set Account_Habitus.iLeftSecond = 0 where (Account_Habitus.cAccname = '%s')", SqlEscapeLiteral(strAccName));
	          pConn->Do(strSQL);    //执行语句
			}

            if (extPoint.lVal > 0)	//点卡有效
			{//返回剩余时间
				liExp  = extPoint.lVal;  
			}
			else
			{
				liExp = 0;          //剩余点卡
				sprintf(strSQL, "update Account_Habitus set Account_Habitus.nExtPoint = 0 where (Account_Habitus.cAccname = '%s')", SqlEscapeLiteral(strAccName));
	            pConn->Do(strSQL);  //执行语句
			}

			liType = Flag.lVal;     //会员类型

			iRet   = ACTION_SUCCESS;//赋值成功
		}
	}

	if (pResult)
		pResult->unuse();   //关闭这个指针

	return iRet;
}


//查询数据库  删除角色
int S3PAccount::VerifyUserModifyPassword(S3PDBConVBC* pConn, DWORD ClientID, const char* strAccName, const char* strPassword)
{
	int iRet = ACTION_FAILED;
	if (NULL == pConn)
	{
		return iRet;
	}

	DWORD NewClientID = 0;
	iRet = GetAccountGameID(pConn, strAccName, NewClientID);
	if (iRet == ACTION_SUCCESS)
	{
		//if (NewClientID == ClientID)
		{
			char strSQL[MAX_PATH];
			S3PResultVBC* pResult = NULL;
			sprintf(strSQL, "select cAccName from Account_info where (cAccName = '%s') and (cSecPassword COLLATE Chinese_PRC_CS_AS = '%s')", SqlEscapeLiteral(strAccName), SqlEscapeLiteral(strPassword));
			if (pConn->QuerySql(strSQL, &pResult))
			{
				if (pResult->num_rows() <= 0)
					iRet = E_ACCOUNT_OR_PASSWORD;   //账号 或 密码错误
				else
					iRet = ACTION_SUCCESS;
			}
			else
				iRet = ACTION_FAILED;
			
			if (pResult)
				pResult->unuse();
		}
		//else
		//	iRet = E_ACCOUNT_ACCESSDENIED;
	}
	return iRet;
}

/*******************************************************************************
// FileName			:	ScriptFuns.cpp
// FileAuthor		:	RomanDou
// FileCreateDate	:	2002-11-19 15:58:20
// FileDescription	:	脚本指令集
// Revision Count	:	
*******************************************************************************/
#ifndef WIN32
#include <string>
#endif
#include "KWin32.h"
#include "KEngine.h"
#include "KDebug.h"
#include "KStepLuaScript.h"	      
#include "LuaLib.h"
#include "KScriptList.h"
#include <string.h>
#include "LuaFuns.h"
#include "KCore.h"
#include "ScriptFuns_V9_Registration.h"
#include "KNpc.h"
#include "KSubWorld.h"
#include "KObjSet.h"
#include "KItemSet.h"
//#include "KNetClient.h"
#include "KScriptValueSet.h"
#include "KNpcSet.h"
#include "KNpcAi.h"
#include "KPlayerSet.h"
#include "KPlayer.h"
#include "KPlayerTong.h"
#include "KSubWorldSet.h"
#include "KProtocolProcess.h"
#include "KBuySell.h"
#include "KTaskFuns.h"
#include "KPlayerDef.h"
#include "KForBitGua.h"
////原来没有的//////////////////////
#include "KProtocolDef.h"
#include "KProtocol.h"
#include "KRelayProtocol.h"
#include "KTongProtocol.h"

#include "KInventory.h"
#ifndef _SERVER
#include "../../core/src/coreshell.h"
#endif

#include <vector>
#include <cassert> 
#include "KNpcTemplate.h"
#include <time.h>
/////////////////////////////////////
#include "TaskDef.h"
#include "../../Engine/Src/Text.h"
#ifdef _SERVER
//#include "KNetServer.h"
//#include "../MultiServer/Heaven/interface/iServer.h"
#include "KNewProtocolProcess.h"
#include "KItemChangeRes.h"

#endif
#include "KSortScript.h"
#ifndef __linux
#include "Shlwapi.h"
#include "windows.h"
#include "winbase.h"
#include <direct.h>
#else
#include "unistd.h"
#endif

#ifdef _STANDALONE
#include "KSG_StringProcess.h"
#else
#include "../../Engine/Src/KSG_StringProcess.h"
#endif
#include <algorithm>

// A table or column name picked by a script is pasted into the statement where
// there is no quoting to hide a quote in, so only a whitelist helps: plain words
// of letters, digits and underscore.  Anything else - including a name long
// enough to overrun the fixed buffer the query is built in - is replaced by a
// name that cannot exist, which makes the statement fail rather than run
// script-chosen SQL.
static const char *SqlIdentifier(const char *pszName)
{
	static const char *const szRejected = "sql_identifier_rejected";

	if (NULL == pszName)
		return szRejected;

	for ( size_t n = 0; n < 64; ++n )
	{
		const unsigned char ch = (unsigned char)pszName[n];
		if ( '\0' == ch )
			return 0 == n ? szRejected : pszName;
		if ( ch >= 'a' && ch <= 'z' ) continue;
		if ( ch >= 'A' && ch <= 'Z' ) continue;
		if ( ch >= '0' && ch <= '9' ) continue;
		if ( '_' == ch ) continue;
		return szRejected;
	}
	return szRejected;			// 64 characters or longer
}

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


#ifndef WIN32
typedef struct  _SYSTEMTIME
{
    WORD wYear;
    WORD wMonth;
    WORD wDayOfWeek;
    WORD wDay;
    WORD wHour;
    WORD wMinute;
    WORD wSecond;
    WORD wMilliseconds;
}	SYSTEMTIME;
typedef struct  _FILETIME
{
    DWORD dwLowDateTime;
    DWORD dwHighDateTime;
}	FILETIME;
#endif
inline const char* _ip2a(DWORD ip) { in_addr ia; ia.s_addr = ip; return inet_ntoa(ia); }
inline DWORD _a2ip(const char* cp) { return inet_addr(cp); }
KTabFile        g_MineItemTab;
KScriptList		g_StoryScriptList;
KStepLuaScript * LuaGetScript(Lua_State * L);
int	       GetPlayerIndex(Lua_State * L);
extern int g_GetPriceToStation(int,int);
extern int g_GetPriceToWayPoint(int,int);
extern int g_GetPriceToDock(int ,int );
//BitValue = GetBit(Value, BitNo)

class LadderInfo
{
public:
	LadderInfo(char* Name)
	{
		if (Name)
		{
			snprintf(name, sizeof(name), "%s", Name);
			id = g_FileName2Id(name);
		}

		value = 0;
		faction = 0;
		gender = 0;
	}

	LadderInfo(char* Name, int x)
	{
		if (Name)
			snprintf(name, sizeof(name), "%s", Name);

		id = x;
		value = 0;
		faction = 0;
		gender = 0;
	}

	LadderInfo(LadderInfo& s)
	{
		snprintf(name, sizeof(name), "%s", s.name);
		value = s.value;
		faction = s.faction;
		gender = s.gender;

		id = s.id;
	}

	bool Compare(const LadderInfo* src)
	{
		return value >= src->value;
	}

	char name[64];
	short value;
	short faction;
	short gender;

	int id;
};

class LadderCompare
{
public:
	bool operator () (const LadderInfo* lhs, const LadderInfo* rhs) const
	{
		return ((LadderInfo*)lhs)->Compare(rhs);
	}
};

class LadderList
{
public:
	USHORT id;
	std::set<class LadderInfo*, LadderCompare> m_list;

	LadderList(USHORT x)
	{
		id = x;
		m_list.clear();
	}

	void LadderEnter(class LadderInfo* player)
	{
		auto it = std::find_if(m_list.begin(), m_list.end(), [&](const LadderInfo* v) { return v->id == player->id; });
		if (it != m_list.end())// neu co' hang
		{
			delete (*it);
			// Deletes the element pointing by iterator it
			m_list.erase(it);
		}
		m_list.insert(player);

		if (m_list.size() > 10)
		{
			it = m_list.end();
			it--;
			delete (*it);
			m_list.erase(it);
		}
	}
};

std::list<class LadderList*> m_LadderRank;
extern CRITICAL_SECTION rankSection;
extern CRITICAL_SECTION topSection;
void UseSkills(int sKillID,int sLevel,int nNpcIdx,KMagicAttrib *pData=NULL);
int  GetQuestSetIdx(int sVal);

#ifndef _SERVER

static void KSceGetFliePathc(char* nName,char * nPath,int nMaxRows)
{
	KFile  nFile;
	KTabFileCtrl nScirptFile;
	char nTongApplyPath[125]={0},szCol[128]={0};
	sprintf(nTongApplyPath,"%s","\\clientscript.log");
	if (!g_FileExists(nTongApplyPath))	//是否存在
	{
		nFile.Create(nTongApplyPath);
		sprintf(szCol,"脚本log\15\n");
		nFile.Write(szCol, sizeof(szCol));
		//nFile.Save(nTongApplyPath);
		nFile.Close();
	}
	
	if (nScirptFile.Load(nTongApplyPath))
	{
		int nRows=nScirptFile.GetHeight();
		if  (nRows==0)
			nRows=1;
		
		if (nRows>=nMaxRows)
		{
			while(nScirptFile.GetHeight()>0)
			{ 
				nScirptFile.Remove(nScirptFile.GetHeight());
				nScirptFile.Save(nTongApplyPath);
			}
		//nScirptFile.Clear();
		//retrn;
		}
		
		snprintf(szCol, sizeof(szCol),"--%s:(%s)--",nName,nPath);
		nScirptFile.InsertAfter(nRows);
		nScirptFile.WriteString(nRows,1, szCol);                                    //行号
		//nTong.WriteString(nRows,2,Npc[Player[m_nPlayerIndex].m_nIndex].Name);     //名字
		//nTong.WriteInteger(nRows,3,Npc[Player[m_nPlayerIndex].m_nIndex].m_Level); //等级
		nScirptFile.Save(nTongApplyPath);	
		//nFile.Write(szCol, sizeof(szCol));
	}
	nScirptFile.Clear();
}
#endif


int LuaMATH_Random(Lua_State * L)
{
/*
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 1)
		return 0;
	int nResult = 0;
	if (nParamNum > 1)
		nResult = GetRandomNumber((int)Lua_ValueToNumber(L, 1), (int)Lua_ValueToNumber(L, 2));
	else
		nResult = GetRandomNumber(0, (int)Lua_ValueToNumber(L, 1));
	Lua_PushNumber(L, nResult);
	return 1;*/
/*
	int nMinNum = (int)Lua_ValueToNumber(L, 1);
	int nMaxNum = (int)Lua_ValueToNumber(L, 2);

	if (nMinNum<0 || nMaxNum<0) return 0;

	if (nMinNum>nMaxNum)
	{//交换
	   int inTemp= nMinNum;
	   nMinNum=nMaxNum;
	   nMaxNum=inTemp;
	}

	//srand((unsigned)time(0));

    Lua_PushNumber(L,rand()%(nMaxNum-nMinNum)+nMinNum); 
*/

	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 1)
		return 0;
	int nResult = 0;
	if (nParamNum > 1)
		nResult = ::GetRandomNumber((int)Lua_ValueToNumber(L, 1), (int)Lua_ValueToNumber(L, 2));
	else
		nResult = ::GetRandomNumber(0, (int)Lua_ValueToNumber(L, 1));
	Lua_PushNumber(L, nResult);


/*
	int nMin = (int)Lua_ValueToNumber(L, 1);
	int nMax = (int)Lua_ValueToNumber(L, 2);
	if (nMin > nMax) 
		return 0;
	

	int grandommax = g_Random(nMax - nMin + 1 );
	int nResult =  grandommax + nMin; 
	//printf("Test: %d - %d - %d - %d\n",nMin,nMax,grandommax,nValue);
	Lua_PushNumber(L, nResult);
*/
	return 1;
}

int LuaMATH_RandomC(Lua_State * L)
{
	int nParamNum = Lua_GetTopIndex(L);

	if (nParamNum < 2)
		return 0;
	
	srand( (unsigned)time( NULL ) );
	if (Lua_IsTable(L,1))
	{
		Lua_PushNumber(L, (int)Lua_ValueToNumber(L,2));
		Lua_RawGet(L, 1);
		Lua_PushNumber(L, (int)Lua_ValueToNumber(L, Lua_GetTopIndex(L)));
	}
	else if (Lua_IsTable(L,2))
	{
		int nResult = ::GetRandomNumber(1, (int)Lua_ValueToNumber(L,1));
		Lua_PushNumber(L, nResult);
		Lua_RawGet(L, 2);
		Lua_PushNumber(L, (int)Lua_ValueToNumber(L, (int)Lua_ValueToNumber(L, Lua_GetTopIndex(L))));
	}
	else
	{
		int nResult = ::GetRandomNumber(1, nParamNum);
		Lua_PushNumber(L, (int)Lua_ValueToNumber(L, nResult));
	}
	return 1;
}

int LuaMATH_SetBit(Lua_State * L)
{
	int nResult = 0;
	if (Lua_GetTopIndex(L) < 4)
		goto lab_setnumber;

	nResult = KSG_StringSetValue((int)Lua_ValueToNumber(L,1),
		(int)Lua_ValueToNumber(L,2),
		(int)Lua_ValueToNumber(L,3),
		(int)Lua_ValueToNumber(L,4));
	
lab_setnumber:
	Lua_PushNumber(L, nResult);
	return 1;
}

int LuaMATH_GetBit(Lua_State * L)
{
	int nResult = 0;
	if (Lua_GetTopIndex(L) < 3)
		goto lab_getnumber;
	nResult = KSG_StringGetValue((int)Lua_ValueToNumber(L,1),
		(int)Lua_ValueToNumber(L,2),
		(int)Lua_ValueToNumber(L,3));

lab_getnumber:
	Lua_PushNumber(L, nResult);
	return 1;
}

///////////////
int LuaGetBit(Lua_State * L)
{
	int nBitValue = 0;
	int nIntValue = (int)Lua_ValueToNumber(L, 1);
	int nBitNumber = (int)Lua_ValueToNumber(L, 2);
	
	if (nBitNumber >= 32 || nBitNumber <= 0) 
		goto lab_getbit;
	nBitValue = (nIntValue & (1 << (nBitNumber - 1))) != 0;
lab_getbit:
	Lua_PushNumber(L, nBitValue);
	return 1;
}

//NewBit = SetBit(Value, BitNo, BitValue)
int LuaSetBit(Lua_State * L)
{
	int nIntValue = (int)Lua_ValueToNumber(L, 1);   //值
	int nBitNumber = (int)Lua_ValueToNumber(L, 2);  //位置
	int nBitValue = (int)Lua_ValueToNumber(L,3);    

	nBitValue = (nBitValue == 1);
	
	if (nBitNumber > 32 || nBitNumber <= 0) //32位
		goto lab_setbit;
	
	nIntValue = (nIntValue | (1 << (nBitNumber - 1)));
lab_setbit:
	Lua_PushNumber(L, nIntValue);
	return 1;
}
//NewByte = SetByte(Value, ByteNo, ByteValue)
int LuaSetByte(Lua_State * L)
{
	BYTE * pByte =	NULL;
	int nIntValue = (int)Lua_ValueToNumber(L, 1);
	int nByteNumber = (int)Lua_ValueToNumber(L, 2);
	int nByteValue = (int)Lua_ValueToNumber(L,3);

	nByteValue = (nByteValue & 0xff);	//0xff换成十进制为255
	
	if (nByteNumber > 4 || nByteNumber <= 0) ///4
		goto lab_setByte;
	
	pByte =(BYTE*)&nIntValue;	 //
	*(pByte + (nByteNumber -1)) = (BYTE)nByteValue;//
	//nIntValue = (nIntValue | (0xff << ((nByteNumber - 1) * 8) )) ;
lab_setByte:
	Lua_PushNumber(L, nIntValue);
	return 1;
}

//ByteValue = GetByte(Value, ByteNo)
int LuaGetByte(Lua_State * L)
{
	int nByteValue = 0;
	int nIntValue = (int)Lua_ValueToNumber(L, 1);     //需要转换的值
	int nByteNumber = (int)Lua_ValueToNumber(L, 2);   //最大存 4个偏移
	
	if (nByteNumber > 4 || nByteNumber <= 0) 
		goto lab_getByte;

	nByteValue = (nIntValue & (0xff << ((nByteNumber - 1) * 8) )) >> ((nByteNumber - 1) * 8);
	
lab_getByte:
	Lua_PushNumber(L, nByteValue);
	return 1;
}


#ifdef _SERVER

int LuaSyncNpc(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 1 ) return 0;
	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC) return 0;
	Npc[nNpcIndex].SendSyncData(0, TRUE);
	return 0;
}

int LuaSyncTaskValue(Lua_State * L)
{
	 int nPlayerIndex = GetPlayerIndex(L);

	 if (nPlayerIndex<0) return 0;

     int nTaskId = (int)Lua_ValueToNumber(L,1);

	 if (nTaskId<0) return 0;

     PLAYER_GETBACK_TASKVAL	sTaskBack;
	 sTaskBack.ProtocolType = s2c_taskcallback;
	 sTaskBack.m_TaskId  = nTaskId;
	 sTaskBack.m_TaskVal = Player[nPlayerIndex].m_cTask.GetSaveVal(nTaskId);
	 if (g_pServer)
		g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, (BYTE*)&sTaskBack, sizeof(PLAYER_GETBACK_TASKVAL));

	  return 0;
}


int LuaSetPlayerReputeValue(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 1) 
		return 0;	
	int nReputeValue = (int)Lua_ValueToNumber(L,1);
	
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) 
		return 0;
	Player[nPlayerIndex].m_cRepute.SetReputeValue(Player[nPlayerIndex].m_cRepute.GetReputeValue() + nReputeValue);
	return 0;
}
//-----------------声望------------------
int LuaGetPlayerReputeValue(Lua_State * L)
{
	int nReputeValue = 0;
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		goto lab_getplayerreputevalue;
	
	if (Player[nPlayerIndex].m_nIndex <= 0) 
		goto lab_getplayerreputevalue;
		nReputeValue = Player[nPlayerIndex].m_cRepute.GetReputeValue();
	
lab_getplayerreputevalue:
	Lua_PushNumber(L, nReputeValue);
	return 1;
}

//-----------------福源
int LuaSetPlayerFuYuanValue(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 1) 
		return 0;	
	int nFuYuanValue = (int)Lua_ValueToNumber(L,1);
	
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) 
		return 0;
	Player[nPlayerIndex].m_cFuYuan.SetFuYuanValue(Player[nPlayerIndex].m_cFuYuan.GetFuYuanValue() + nFuYuanValue);
	return 0;
}

int LuaGetPlayerFuYuanValue(Lua_State * L)
{
	int nFuYuanValue = 0;
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		goto lab_getplayerfuyuanvalue;
	
	if (Player[nPlayerIndex].m_nIndex <= 0) 
		goto lab_getplayerfuyuanvalue;
		nFuYuanValue = Player[nPlayerIndex].m_cFuYuan.GetFuYuanValue();
	
lab_getplayerfuyuanvalue:
	Lua_PushNumber(L, nFuYuanValue);
	return 1;
}

int LuaGetDiemTongKimValue(Lua_State * L)
{
	int nDiemTongKim = 0;
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		goto lab_getplayerdiemtongkimvalue;
	
	if (Player[nPlayerIndex].m_nIndex <= 0) 
		goto lab_getplayerdiemtongkimvalue;
		nDiemTongKim = Player[nPlayerIndex].m_cTask.GetSaveVal(192);
	
lab_getplayerdiemtongkimvalue:
	Lua_PushNumber(L, nDiemTongKim);
	return 1;
}


int LuaAddDiemTongKimValue(Lua_State * L)
{
	int nParamNum = Lua_GetTopIndex(L);
	int nPlayerIndex;
	   nPlayerIndex   = GetPlayerIndex(L);

	int nValue = (int)Lua_ValueToNumber(L, 1);

	if (nPlayerIndex <= 0) return 0;

	int nDiemTongKim = Player[nPlayerIndex].m_cTask.GetSaveVal(192);

	Player[nPlayerIndex].m_cTask.SetSaveVal(192, nDiemTongKim + nValue,true);  //编号 和 值

	return 0;
}

int LuaGetDiemVinhDuValue(Lua_State * L)
{
	int nDiemVinhDu = 0;
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		goto lab_getplayerdiemvinhduvalue;
	
	if (Player[nPlayerIndex].m_nIndex <= 0) 
		goto lab_getplayerdiemvinhduvalue;
		nDiemVinhDu = Player[nPlayerIndex].m_cTask.GetSaveVal(2051);
	
lab_getplayerdiemvinhduvalue:
	Lua_PushNumber(L, nDiemVinhDu);
	return 1;
}


int LuaAddDiemVinhDuValue(Lua_State * L)
{
	int nParamNum = Lua_GetTopIndex(L);
	int nPlayerIndex;
	   nPlayerIndex   = GetPlayerIndex(L);

	int nValue = (int)Lua_ValueToNumber(L, 1);

	if (nPlayerIndex <= 0) return 0;

	int nDiemVinhDu = Player[nPlayerIndex].m_cTask.GetSaveVal(2051);

	Player[nPlayerIndex].m_cTask.SetSaveVal(2051, nDiemVinhDu + nValue,true);  //编号 和 值

	return 0;
}


int LuaSetPlayerSetRoleVipValue(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 1) 
		return 0;	
	int nReBornValue = (int)Lua_ValueToNumber(L,1);
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) 
		return 0;
	Player[nPlayerIndex].m_cVip.SetRoleVipValue(nReBornValue);
	return 0;
}
//==================Vip=================
int LuaSetPlayerRoleVipValue(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 1) 
		return 0;	
	int nReBornValue = (int)Lua_ValueToNumber(L,1);
	int nReBornLevel = (int)Lua_ValueToNumber(L,2);

	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) 
		return 0;
	Player[nPlayerIndex].m_cVip.AddRoleVipValue(nReBornValue,nReBornLevel);
	return 0;
}


int LuaGetPlayerRoleVipNum(Lua_State * L)
{
	int nReBornValue = 0;
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		goto lab_getplayerrebornvalue;

	if (Player[nPlayerIndex].m_nIndex <= 0) 
		goto lab_getplayerrebornvalue;

	nReBornValue = Player[nPlayerIndex].m_cVip.GetRoleVipNum();

lab_getplayerrebornvalue:
	Lua_PushNumber(L, nReBornValue);
	return 1;
}

int LuaGetPlayerRoleVipValue(Lua_State * L)
{
	int nReBornValue = 0;
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		goto lab_getplayerrebornvalue;

	if (Player[nPlayerIndex].m_nIndex <= 0) 
		goto lab_getplayerrebornvalue;

	nReBornValue = Player[nPlayerIndex].m_cVip.GetRoleVipValue();

lab_getplayerrebornvalue:
	Lua_PushNumber(L, nReBornValue);
	return 1;

}

//==================转生=================
int LuaSetPlayerReBornValue(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 1) 
		return 0;	
	int nReBornValue = (int)Lua_ValueToNumber(L,1);
	int nReBornLevel = (int)Lua_ValueToNumber(L,2);

	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) 
		return 0;
	Player[nPlayerIndex].m_cReBorn.AddReBornValue(nReBornValue,nReBornLevel);
	return 0;
}


int LuaGetPlayerReBornNum(Lua_State * L)
{
	int nReBornValue = 0;
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		goto lab_getplayerrebornvalue;
	
	if (Player[nPlayerIndex].m_nIndex <= 0) 
		goto lab_getplayerrebornvalue;
	
	nReBornValue = Player[nPlayerIndex].m_cReBorn.GetReBornNum();
	
lab_getplayerrebornvalue:
	Lua_PushNumber(L, nReBornValue);
	return 1;
}

int LuaGetPlayerReBornCount(Lua_State * L)
{
	int nReBornValue = 0;
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		goto lab_getplayerrebornvalue;

	if (Player[nPlayerIndex].m_nIndex <= 0) 
		goto lab_getplayerrebornvalue;

	nReBornValue = Player[nPlayerIndex].m_cReBorn.GetReBornCount();

lab_getplayerrebornvalue:
	Lua_PushNumber(L, nReBornValue);
	return 1;
}

int LuaGetPlayerReBornValue(Lua_State * L)
{
	int nReBornValue = 0;
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		goto lab_getplayerrebornvalue;
	
	if (Player[nPlayerIndex].m_nIndex <= 0) 
		goto lab_getplayerrebornvalue;

		nReBornValue = Player[nPlayerIndex].m_cReBorn.GetReBornValue();
	
lab_getplayerrebornvalue:
	Lua_PushNumber(L, nReBornValue);
	return 1;

}

int LuaAddMoneyObj(Lua_State * L)
{
	int nX, nY,nMap;
	POINT	ptLocal;
	KMapPos	Pos;
	int nPlayerIndex = GetPlayerIndex(L);

	 if (nPlayerIndex <= 0 || nPlayerIndex >=MAX_PLAYER)
		 return 0;

    int	nMoney = (int)Lua_ValueToNumber(L,1);
	int nSubWorldIndex = Npc[Player[nPlayerIndex].m_nIndex].m_SubWorldIndex;
	Npc[Player[nPlayerIndex].m_nIndex].GetMpsPos(&nX, &nY,&nMap);
	ptLocal.x = nX;
	ptLocal.y = nY;
	SubWorld[nSubWorldIndex].GetFreeObjPos(ptLocal);
	Pos.nSubWorld = nSubWorldIndex;
	SubWorld[nSubWorldIndex].Mps2Map(ptLocal.x, ptLocal.y, 
		&Pos.nRegion, &Pos.nMapX, &Pos.nMapY, 
		&Pos.nOffX, &Pos.nOffY);	
	int nObjIdx = ObjSet.AddMoneyObj(Pos, nMoney);
	if (nObjIdx > 0 && nObjIdx < MAX_OBJECT)
	{
		if (nPlayerIndex > 0)
			Object[nObjIdx].SetItemBelong(nPlayerIndex);
		else
			Object[nObjIdx].SetItemBelong(-1);
	}
	
	return 0;
}
//=================================
int LuaSetItemMagicLevel(Lua_State * L)
{
  int nItmeIndex = (int)Lua_ValueToNumber(L, 1);
  int nMlevel    = (int)Lua_ValueToNumber(L, 2);  //第几个属性的值
  int nVale      = (int)Lua_ValueToNumber(L, 3);
  if  (nItmeIndex<=0 || nItmeIndex >=MAX_ITEM) return 0;
  Item[nItmeIndex].SetItemMagicLevel(nMlevel,nVale);
  if (nMlevel < 0 || nMlevel >= 7) return 0;
  Item[nItmeIndex].GetItemParam()->nJGeneratorLevel[nMlevel]=nVale;
  return 0;
}

//Dong bo chi s? item nh?t ??nh
int  LuaSyncItem(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	int nItemIdx = (int)Lua_ValueToNumber(L,1);


	if (nItemIdx <= 0 || nItemIdx > MAX_ITEM)
		return 0;

	Player[nPlayerIndex].m_ItemList.SynPlayerItem(nItemIdx);
	return 0;
}

int LuaSetChiBangRes(Lua_State * L)
{
    int nItmeIndex = (int)Lua_ValueToNumber(L, 1);
    int nMlevel = (int)Lua_ValueToNumber(L, 2);
	
	if  (nItmeIndex>0 && nItmeIndex < MAX_ITEM)
	    Item[nItmeIndex].SetChiBangRes(nMlevel);

    return 0;
}

int LuaGetChiBangRes(Lua_State * L)
{
    int nItmeIndex = (int)Lua_ValueToNumber(L, 1);
    int nMlevel=-1;
	
	if  (nItmeIndex>0 && nItmeIndex < MAX_ITEM)
	     nMlevel = Item[nItmeIndex].GetChiBangRes();

	Lua_PushNumber(L,nMlevel);
    return 1;
}

int  LuaClearShopItemData(Lua_State * L)
{
	if (g_DbItemFile)
	{//开始检测是否有这个id的物品 
		char nIdxInfo[32]={0};
		int nItmeIndex = (int)Lua_ValueToNumber(L, 1);
		int nKind      = (int)Lua_ValueToNumber(L, 2);

		if  (nKind>=1)
		{//全部删除
			ZCursorS *cursor = g_DbItemFile->first();
			while(cursor)
			{
				TDBShopItemData* pRoleData = (TDBShopItemData*)cursor->data;

				if (pRoleData && cursor->size >= (int)sizeof(TDBShopItemData) && pRoleData->iid>0)
				{
				    printf("----删除:%04d,iid:%d KEY号:%s 角:%s 名:%s 密:%u --\n",nItmeIndex,pRoleData->iid,pRoleData->iaccName,pRoleData->iWonName,pRoleData->itemName,pRoleData->iparamj3);
					//sprintf(nIdxInfo,"剑侠%04d情缘",pRoleData->iid);
					sprintf(nIdxInfo, "kiem%04dhiep", pRoleData->iid);
					g_DbItemFile->remove(nIdxInfo,strlen(nIdxInfo)+1);
				}

				if(!g_DbItemFile->next(cursor))
					break;
			}

			return 0;
		}
		//sprintf(nIdxInfo,"剑侠%04d情缘",nItmeIndex);
		sprintf(nIdxInfo, "kiem%04dhiep", nItmeIndex);
		ZCursorS *item_cursor = g_DbItemFile->search(nIdxInfo,strlen(nIdxInfo)+1);	//pName		strlen(szName) + 1  主数据库查询
		if (item_cursor)
		{

			TDBShopItemData* pItemData = (TDBShopItemData*)item_cursor->data;

			if (pItemData && item_cursor->size >= (int)sizeof(TDBShopItemData) && pItemData->iid>0)
			{
				printf("----删除:%04d,iid:%d KEY号:%s 角:%s 名:%s 密:%u --\n",nItmeIndex,pItemData->iid,pItemData->iaccName,pItemData->iWonName,pItemData->itemName,pItemData->iparamj3);
				//sprintf(nIdxInfo,"剑侠%04d情缘",pItemData->iid);
				sprintf(nIdxInfo, "kiem%04dhiep", pItemData->iid);
				g_DbItemFile->remove(nIdxInfo,strlen(nIdxInfo)+1);
			}

			g_DbItemFile->closeCursor(item_cursor);
		}
		else
		{//不存在,就可以利用
		}
	}

	return 0;
}


int LuaGetMoneyFromPaiShop(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	if (!g_DbItemFile) return 0;
	char *nReleName = (char *)Lua_ValueToString(L,1);
	if (!nReleName) return 0;
	UINT  nReg = 0;
	_ItemDataInfo::iterator it; 
	for( it = iItemDataInfo.begin(); it != iItemDataInfo.end(); ++it)
	{//开始查询数据
		if (strstr(it->second.iWonName,nReleName) && it->second.iid>0 && it->second.isSellOut>0 && it->second.iparamj2==0)
		{//开始给收入,领取所有已经卖出的装备  已经卖出 并且不时过期的
			int nPriceKind = it->second.iPriceKind;
			int nMoneyVal  = it->second.iSellPrice;
			int nItemDataIdx=it->second.iid;
			    nReg +=nMoneyVal;
			if  (nPriceKind==moneyunit_money || nPriceKind==moneyunit_moneya)
				Player[nPlayerIndex].Earn(nMoneyVal);
			else if (nPriceKind==moneyunit_xu)
				Player[nPlayerIndex].EarnXu(nMoneyVal);
			else if (nPriceKind==moneyunit_fuyuan)
				Player[nPlayerIndex].m_cFuYuan.AddFuYuanValue(nMoneyVal);
			else if (nPriceKind==moneyunit_repute)
				Player[nPlayerIndex].m_cRepute.AddReputeValue(nMoneyVal);
			else if (nPriceKind==moneyunit_expoint)
			{
				 //Player[nPlayerIndex].AddAndPayExtPoint(nMoneyVal);
				int nVal = Player[nPlayerIndex].m_cTask.GetSaveVal(2051);
				Player[nPlayerIndex].m_cTask.SetSaveVal(2051,nVal+nMoneyVal);
			}
			else if (nPriceKind==moneyunit_jifen)
			{
				int nVal = Player[nPlayerIndex].m_cTask.GetSaveVal(192);
				Player[nPlayerIndex].m_cTask.SetSaveVal(192,nVal+nMoneyVal);
			}
			//else
			//	return 0;

			//iItemDataInfo[it->second.iid].iparamj4=1;
			it->second.iparamj4 =1;
			iItemDataInfo.erase(it->second.iid);
			//开始删除数据库里的数据
			char nItemKey[32]={0};
			//sprintf(nItemKey,"剑侠%04d情缘",nItemDataIdx);
			sprintf(nItemKey, "kiem%04dhiep", nItemDataIdx);
			ZCursorS *item_cursor = g_DbItemFile->search(nItemKey,strlen(nItemKey)+1);	//pName		strlen(szName) + 1  主数据库查询
			if (item_cursor)
			{
				TDBShopItemData* pItemData = (TDBShopItemData*)item_cursor->data;
				if (pItemData)
					g_DbItemFile->remove(nItemKey,strlen(nItemKey)+1);

				g_DbItemFile->closeCursor(item_cursor);
			}
		}
	}
	Lua_PushNumber(L, nReg);	
	return 1;
}

int LuaGetPaiCountByRoleName(Lua_State * L)
{

	char *nReleName = (char *)Lua_ValueToString(L,1);

	if  (!nReleName) return 0;
	int nCount =0;
	if (g_DbItemFile)
	{//开始检测是否有这个id的物品 
		ZCursorS *cursor = g_DbItemFile->first();
		while(cursor)
		{	
			TDBShopItemData* pItemData = (TDBShopItemData*)cursor->data;

			if (pItemData && cursor->size >= (int)sizeof(TDBShopItemData) && pItemData->iid>0)
			{
				if (strstr(pItemData->iWonName,nReleName))
				{//如果名字相同
					nCount ++;
				}
				//sprintf(nIdxInfo,"%04d",pItemData->iid);
				//g_DbItemFile->remove(nIdxInfo,strlen(nIdxInfo)+1);
			}

			if(!g_DbItemFile->next(cursor))
				break;
		}
	}

	Lua_PushNumber(L,nCount);
	return 1;
}

int LuaGetShopItemDataByName(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);

	if (nPlayerIndex <= 0)
	{
		return 0;
	}

	if (g_DbItemFile)
	{//开始检测是否有这个id的物品 
		ZCursorS *cursor = g_DbItemFile->first();
		while(cursor)
		{	
			TDBShopItemData* pItemData = (TDBShopItemData*)cursor->data;

			if (pItemData && cursor->size >= (int)sizeof(TDBShopItemData) && pItemData->iid>0)
			{
				if (strstr(pItemData->iWonName,Npc[Player[nPlayerIndex].m_nIndex].Name))
				{//如果名字相同
					char nMsg[125]={0};
					snprintf(nMsg, sizeof(nMsg),"<color=gyellow>编号:%d 物品:%s 状态:%s 取回密码:%u<color>",pItemData->iid,pItemData->itemName,(pItemData->iparamj2>0)?"已过期":((pItemData->isSellOut>0)?"已卖出":"拍卖中"),pItemData->iparamj3);
					//(used > m_sAccountName.length() ) ? m_sAccountName.length() : (used - 1);
					int nLeng  = TEncodeText(nMsg, strlen(nMsg));
					const char *  szMsg = nMsg;
					if (szMsg)
					{
						KPlayerChat::SendSystemInfo(1, nPlayerIndex,"拍卖行", (char *) szMsg, nLeng );//strlen(szMsg)
					}
				}
				//sprintf(nIdxInfo,"%04d",pItemData->iid);
				//g_DbItemFile->remove(nIdxInfo,strlen(nIdxInfo)+1);
			}

		    if(!g_DbItemFile->next(cursor))
			   break;
		}
	}
return 0;
}

int  LuaGetShopItemData(Lua_State * L)
{//取回装备
	int  nReg =0;
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;

	if (g_DbItemFile)
	{//开始检测是否有这个id的物品 
		char nIdxInfo[32]={0};
		int  nItmeIndex    = (int)Lua_ValueToNumber(L,1);
		int  nItemPassWord = (int)Lua_ValueToNumber(L,2);
		//sprintf(nIdxInfo,"剑侠%04d情缘",nItmeIndex);
		sprintf(nIdxInfo, "kiem%04dhiep", nItmeIndex);
		ZCursorS *item_cursor = g_DbItemFile->search(nIdxInfo,strlen(nIdxInfo)+1);	//pName		strlen(szName) + 1  主数据库查询
		if (item_cursor)
		{
			TDBShopItemData* pItemData = (TDBShopItemData*)item_cursor->data;
			if (pItemData && item_cursor->size >= (int)sizeof(TDBShopItemData))
			{//开始修改数据
				printf("----找到:%04d,iid:%d KEY号:%s 角:%s 名:%s 密:%u --\n",nItmeIndex,pItemData->iid,pItemData->iaccName,pItemData->iWonName,pItemData->itemName,pItemData->iparamj3);
				if (pItemData->iparamj3==nItemPassWord /*&& (pItemData->isSellOut<=0 || pItemData->iparamj2==1)*/)
				{//密码相同的　如果是没有买掉的　或者 过期的　就可以取回　开始给物品 同时删除数据库数据　删除节点
					if (pItemData->isSellOut>0)
					{
						Player[nPlayerIndex].m_ItemList.msgshow("拍卖行:<color=gyellow>该装备已经卖出<color>!");
						g_DbItemFile->closeCursor(item_cursor);
						return 0;
					}
					//开始产生物品,然后删除节点-------
					int nItemClass = pItemData->iequipgen;
					int nIsStack   = pItemData->iparamj1;//是否可以叠加
					int nIsSellOut = pItemData->isSellOut;
					UINT nStartTime= pItemData->addtime;
					UINT nEndTime  = pItemData->endtime;
					int nTimeOut   = pItemData->iparamj2;//是否过期
					int nStackNum = 1;
					int nSeries   = pItemData->iseries;
					int nLevel    = pItemData->ilevel;
					int nLuck     = pItemData->ilucky;
					int nDetailType     = pItemData->idetailtype;
					int nParticularType = pItemData->iparticulartype;
					int nItemLevel[6],nRItemLevel[6];
					ZeroMemory(&nItemLevel,sizeof(nItemLevel));
					ZeroMemory(&nRItemLevel,sizeof(nRItemLevel));
					nItemLevel[0] = pItemData->iparam1;
					nItemLevel[1] = pItemData->iparam2;
					nItemLevel[2] = pItemData->iparam3;
					nItemLevel[3] = pItemData->iparam4;
					nItemLevel[4] = pItemData->iparam5;
					nItemLevel[5] = pItemData->iparam6;

					nRItemLevel[0] = pItemData->iparamr1;
					nRItemLevel[1] = pItemData->iparamr2;
					nRItemLevel[2] = pItemData->iparamr3;
					nRItemLevel[3] = pItemData->iparamr4;
					nRItemLevel[4] = pItemData->iparamr5;
					nRItemLevel[6] = pItemData->iparamr6;
					UINT nRandSeed = pItemData->irandseed;
					int  nEnChance = pItemData->ienchance;
					int  nPoint    = pItemData->ipoint;
					int  IsWhere   = pItemData->iswhere;
					int  nRongPoint= pItemData->iRongpiont;
					int  nGoldIdx  = pItemData->igoldid;
					int  nPlatina  = pItemData->isplatina;
					int  nIndex = 0;

					if (nGoldIdx<=0)
					{//蓝装装
						nIndex = ItemSet.AddOther(nItemClass, nSeries, nLevel, nLuck, nDetailType, nParticularType,nItemLevel, g_SubWorldSet.GetGameVersion(),nRandSeed,1,nEnChance,nPoint,\
							0,0,0,0,0,nRongPoint,nRItemLevel,NULL,NULL,IsWhere,TRUE);
					}
					else
					{//其他装备　黄金装备
						Player[nPlayerIndex].m_ItemList.msgshow("拍卖行:不支持该装备!");
						g_DbItemFile->closeCursor(item_cursor);
						return 0;
					}

					if (nIndex <= 0)
					{
						g_DbItemFile->closeCursor(item_cursor);
						return 0;
					}
					iItemDataInfo.erase(pItemData->iid); //删除这个节点
					g_DbItemFile->remove(nIdxInfo,strlen(nIdxInfo)+1);//删除数据库

					Player[nPlayerIndex].m_ItemList.msgshow("拍卖行:<color=gyellow>取回成功<color>!");
					//------------------------------------------------------
					/*if (Item[nIndex].GetGenre()!=0 && !Item[nIndex].GetIsBang() && Item[nIndex].GetTime()->bYear<=0)
					{//不是装备的话,包袱里是否有这个物品
						int nIdx=Player[nPlayerIndex].m_ItemList.IsHaveItem(Item[nIndex].GetGenre(),\
							Item[nIndex].GetDetailType(),\
							Item[nIndex].GetParticular(),\
							Item[nIndex].GetSeries(),\
							Item[nIndex].GetLevel(),pos_equiproom);
						 //xep chong item
						if (nIdx>0 && Item[nIdx].IsStack() && Item[nIndex].CanStack(nIdx) && (Item[nIdx].GetStackNum()+nStackNum) <= MAX_STACKNUM) //是否可以叠加--手里和包袱里的是否相同
						{//包袱里的东西可以叠加
							//Item[nIdx].SetStackNum(Item[nIdx].GetStackNum()+StackNum);             //设置叠加数量
							//m_ItemList.SynPlayerItem(nIdx);                                        //同步到客户端
							nStackNum=Item[nIdx].GetStackNum()+nStackNum;
							Player[nPlayerIndex].m_ItemList.Remove(nIdx);//删除包袱的东西
							ItemSet.Remove(nIdx);
						}
					}*/
					//-------------------------------------------------------
					/*if (Player[nPlayerIndex].m_ItemList.GetPlayerItemCount()>=MAX_ITEMLIST_CONUT)
					{
						ItemSet.Remove(nIndex);
						Player[nPlayerIndex].m_ItemList.msgshow("拍卖行:<color=yellow>物品超限,请清理储物空间<color>!");
						g_DbItemFile->closeCursor(item_cursor);
						return 0;
					}*/

					int	x, y;
					if (Player[nPlayerIndex].m_ItemList.CheckCanPlaceInEquipment(Item[nIndex].GetWidth(), Item[nIndex].GetHeight(), &x, &y))
					{//同步客户端（包里）
						Player[nPlayerIndex].m_ItemList.Add(nIndex,pos_equiproom,x,y,0,0,nStackNum);  //包袱增加装备 同步装备--客户端
					}
					else
					{
						Player[nPlayerIndex].m_ItemList.msgshow("拍卖行:包袱已满!");
						int	nIdx = Player[nPlayerIndex].m_ItemList.Hand();

						if (nIdx)
						{
							Player[nPlayerIndex].m_ItemList.Remove(nIdx);  //删除客户端的节点了
							ItemSet.Remove(nIdx);
						}

						//同步客户端（手里增加新的节点）
						Player[nPlayerIndex].m_ItemList.Remove(nIndex);  //删除客户端的节点了
						ItemSet.Remove(nIndex);
					}
					//---------------------------------
					nReg =1;
				}
				else
					Player[nPlayerIndex].m_ItemList.msgshow("拍卖行:密码不对或已经售出!");
			}
			g_DbItemFile->closeCursor(item_cursor);
		}
		else
			Player[nPlayerIndex].m_ItemList.msgshow("拍卖行:没有该物品或已经取回!");
	}

	Lua_PushNumber(L, nReg);
	return 1;
}

int LuaAddShopItem(Lua_State * L)
{

	int nParnCount = (int)Lua_ValueToNumber(L, 1);

	if (!Lua_IsTable(L, 2)) 
	{
		printf("---脚本错误,第二参数只能使用表格类型--\n");
		return 0;
	}

	if (nMaxSellCount<1)
	{
		printf("---脚本错误,拍卖行最大物品数量为 0 --\n");
		return 0;
	}

	char * pItemBuffer = (char*)_alloca(sizeof(TDBShopItemData));

	TDBShopItemData *nTempItem =(TDBShopItemData *)pItemBuffer;

	for (int i  = 0; i < nParnCount; i ++)
	{//开始赋值属性
		char  pAnswer[64]={0};
		ZeroMemory(pAnswer,sizeof(pAnswer));
		Lua_PushNumber(L,i + 1); //压入一个Key
		Lua_RawGet(L, 2);        //取得整个脚本传进来的第三个参数的这个对应TABLE的KEY值 然后出栈  
		char * pszString = (char *)Lua_ValueToString(L, Lua_GetTopIndex(L));
		if (pszString)
		{
			g_StrCpyLen(pAnswer, pszString, 64);

			//printf("----%d %s ----\n",i,pAnswer);
			if (i==0)
				nTempItem->iid = atoi(pAnswer);
			if (i==1)
			nTempItem->iequipgen=atoi(pAnswer);
			if (i==2)
			nTempItem->idetailtype=atoi(pAnswer);
			if (i==3)
			nTempItem->iparticulartype=atoi(pAnswer);
			if (i==4)
			snprintf(nTempItem->itemName, sizeof(nTempItem->itemName), "%s", pAnswer);
			if (i==5)
			nTempItem->ilevel=atoi(pAnswer);     //等级
			if (i==6)
			nTempItem->iseries=atoi(pAnswer);    //武行
			if (i==7)
			{
			 nTempItem->irandseed=g_Atoui(pAnswer); //随机种子
			 //printf("----随机种子:%u ---\n",nTempItem->irandseed);
			}
			if (i==8)
			nTempItem->iparam1=atoi(pAnswer);    //蓝装
			if (i==9)
			nTempItem->iparam2=atoi(pAnswer);
			if (i==10)
			nTempItem->iparam3=atoi(pAnswer);
			if (i==11)
			nTempItem->iparam4=atoi(pAnswer);
			if (i==12)
			nTempItem->iparam5=atoi(pAnswer);
			if (i==13)
			nTempItem->iparam6=atoi(pAnswer);
			if (i==14)
			nTempItem->iparamr1=atoi(pAnswer);   //熔炼
			if (i==15)
			nTempItem->iparamr2=atoi(pAnswer);
			if (i==16)
			nTempItem->iparamr3=atoi(pAnswer);
			if (i==17)
			nTempItem->iparamr4=atoi(pAnswer);
			if (i==18)
			nTempItem->iparamr5=atoi(pAnswer);
			if (i==19)
			nTempItem->iparamr6=atoi(pAnswer);
			if (i==20)
			nTempItem->ilucky=atoi(pAnswer);   //装备的幸运值
			if (i==21)
			nTempItem->igoldid=atoi(pAnswer);  //是否黄金
			if (i==22)
			nTempItem->istacknum=atoi(pAnswer);//叠加数量
			if (i==23)
			nTempItem->ienchance=atoi(pAnswer);//加成
			if (i==24)
			nTempItem->ipoint=atoi(pAnswer);  //是否紫装 7为特殊装 8 为随机黄金   1-6 为紫装
			if (i==25)
			nTempItem->addtime=g_Atoui(pAnswer); //开始时间
			if (i==26)
			nTempItem->endtime=g_Atoui(pAnswer); //结束时间
			if (i==27)
			nTempItem->iPriceKind=atoi(pAnswer);   //货币类型
			if (i==28)
			nTempItem->iSellPrice=atoi(pAnswer);   //售卖价格
			if (i==29)
			nTempItem->iRongpiont=atoi(pAnswer);   //是否熔炼
			if (i==30)
			snprintf(nTempItem->iWonName, sizeof(nTempItem->iWonName), "%s", pAnswer); //挂售者
			if (i==31)
			snprintf(nTempItem->iaccName, sizeof(nTempItem->iaccName), "%s", pAnswer); //帐号
			if (i==32)
			nTempItem->iswhere=atoi(pAnswer);    //是否洗练装
			if (i==33)
			nTempItem->isplatina=atoi(pAnswer);  //是否白金
			if (i==34)
			nTempItem->isSellOut=g_Atoui(pAnswer);  //是否已经卖出
			if (i==35)
			nTempItem->iparamj1=atoi(pAnswer);   //(保留使用) 是否可以叠加
			if (i==36)
			nTempItem->iparamj2=atoi(pAnswer);   //是否过期
			if (i==37)
			nTempItem->iparamj3=g_Atoui(pAnswer);//取回密码
			if (i==38)
			nTempItem->iparamj4=atoi(pAnswer);   //是否领取了收入
			if (i==39)
			nTempItem->iparamj5=atoi(pAnswer);   
			if (i==40)
			nTempItem->iparamj6=atoi(pAnswer);
			if (i==41)
			nTempItem->iparamj7=atoi(pAnswer);
		}
	}

	int nRes=0;
	char nIdxInfo[32]={0};
    if (g_DbItemFile)
	{//开始检测是否有这个id的物品 
      int i;
      for (i=1;i<=nMaxSellCount;i++)
	  {
		//sprintf(nIdxInfo,"剑侠%04d情缘",i);
		  sprintf(nIdxInfo, "kiem%04dhiep", i);
		ZCursorS *item_cursor = g_DbItemFile->search(nIdxInfo,strlen(nIdxInfo)+1);	//pName		strlen(szName) + 1  主数据库查询
		if (item_cursor)
		{
			/*TDBShopItemData* pItemData = (TDBShopItemData*)item_cursor->data;
			if (pItemData)
			{//开始修改数据
				if (pItemData->iparamr4==1) //已经兑换了收入
				{//如果存在并且 已经卖掉了或者过期 也可以利用 覆盖掉
					break;
				}
			}*/
			g_DbItemFile->closeCursor(item_cursor);
		}
		else
		{//不存在,就可以利用
			break;
		}
	 }//end for
	  if (i>0)
	  {//开始添加数据
		  //sprintf(nTempItem->itemName,"测试物品%04d",i);
		  nTempItem->iid = i;
		  //sprintf(nTempItem->iaccName,"剑侠%04d情缘",i);
		  sprintf(nTempItem->iaccName, "kiem%04dhiep", i);
		  if (g_DbItemFile->add(nIdxInfo,strlen(nIdxInfo)+1,pItemBuffer,sizeof(TDBShopItemData)))   //pName strlen(szName) +1
		  {
			   //iItemDataInfo[i]=nTempItem;
			   iItemDataInfo[i].iid =nTempItem->iid;
			   iItemDataInfo[i].iequipgen= nTempItem->iequipgen;
			   iItemDataInfo[i].idetailtype= nTempItem->idetailtype;
			   iItemDataInfo[i].iparticulartype= nTempItem->iparticulartype;
				snprintf(iItemDataInfo[i].itemName, sizeof(iItemDataInfo[i].itemName), "%s", nTempItem->itemName);
				iItemDataInfo[i].ilevel= nTempItem->ilevel;     //等级
				iItemDataInfo[i].iseries= nTempItem->iseries;    //武行
				iItemDataInfo[i].irandseed= nTempItem->irandseed; //随机种子
				iItemDataInfo[i].iparam2= nTempItem->iparam2;    //蓝装
				iItemDataInfo[i].iparam3= nTempItem->iparam3;
				iItemDataInfo[i].iparam5=nTempItem->iparam5;
				iItemDataInfo[i].iparam4= nTempItem->iparam4;
				iItemDataInfo[i].iparam6=  nTempItem->iparam6;
				iItemDataInfo[i].iparam1= nTempItem->iparam1;
				 iItemDataInfo[i].iparamr2= nTempItem->iparamr2;   //熔炼
				 iItemDataInfo[i].iparamr3= nTempItem->iparamr3;
				 iItemDataInfo[i].iparamr5= nTempItem->iparamr5;
				 iItemDataInfo[i].iparamr4= nTempItem->iparamr4;
				 iItemDataInfo[i].iparamr6= nTempItem->iparamr6;
				 iItemDataInfo[i].iparamr1= nTempItem->iparamr1;
				iItemDataInfo[i].ilucky=  nTempItem->ilucky;   //装备的幸运值
				iItemDataInfo[i].igoldid=  nTempItem->igoldid;  //是否黄金
				iItemDataInfo[i].istacknum=  nTempItem->istacknum;//叠加数量
				iItemDataInfo[i].ienchance=  nTempItem->ienchance;//加成
				 iItemDataInfo[i].ipoint= nTempItem->ipoint;  //是否紫装 7为特殊装 8 为随机黄金   1-6 为紫装
				 iItemDataInfo[i].addtime= nTempItem->addtime; //开始时间
				iItemDataInfo[i].endtime=  nTempItem->endtime; //结束时间
				iItemDataInfo[i].iPriceKind=  nTempItem->iPriceKind;   //货币类型
				iItemDataInfo[i].iSellPrice=  nTempItem->iSellPrice;   //售卖价格
				iItemDataInfo[i].iRongpiont=  nTempItem->iRongpiont;   //是否熔炼
				snprintf(iItemDataInfo[i].iWonName, sizeof(iItemDataInfo[i].iWonName), "%s", nTempItem->iWonName); //挂售者
				snprintf(iItemDataInfo[i].iaccName, sizeof(iItemDataInfo[i].iaccName), "%s", nTempItem->iaccName); //二级查询　KEY
				iItemDataInfo[i].iswhere=  nTempItem->iswhere;    //是否洗练装
				iItemDataInfo[i].isplatina=  nTempItem->isplatina;  //是否白金
				iItemDataInfo[i].isSellOut=  nTempItem->isSellOut;  //是否已经卖出
				iItemDataInfo[i].iparamj1=  nTempItem->iparamj1;   //(保留使用)
				iItemDataInfo[i].iparamj2=  nTempItem->iparamj2;
				iItemDataInfo[i].iparamj3=  nTempItem->iparamj3;   //取回的密码
				iItemDataInfo[i].iparamj5=  nTempItem->iparamj5;
				iItemDataInfo[i].iparamj4=  nTempItem->iparamj4;
				iItemDataInfo[i].iparamj6=  nTempItem->iparamj6;
				iItemDataInfo[i].iparamj7=  nTempItem->iparamj7;
			  printf("----拍卖行增加(%s)数据成功----\n",iItemDataInfo[i].itemName);

			  /*char nMsg[256];
			  //g_StrCpyLen(nMsg,  Lua_ValueToString(L,1), sizeof(nMsg));
			  sprintf(nMsg,"",);
			  int nleg=TEncodeText(nMsg, strlen(nMsg));
			  const char * szMsg = nMsg;
			  if (szMsg)
			  {
				  KPlayerChat::SendSystemInfo(0,0,"拍卖行", (char *)szMsg, nleg);  //strlen(szMsg) 
			  }*/

			  nRes = 1;
		  }
		  else
              printf("----拍卖行增加数据失败----\n");
	  }
	  else
         printf("----拍卖行增加数据失败,暂无空位置----\n");
	} //end if g_DbItemFile


   Lua_PushNumber(L,nRes);
   return 1;
}

int LuaGetItemMagicLevel(Lua_State * L)
{
    int nItmeIndex = (int)Lua_ValueToNumber(L, 1);
    int nMlevel = (int)Lua_ValueToNumber(L, 2);  //第几个属性的值
	if  (nItmeIndex<=0 || nItmeIndex >=MAX_ITEM) return 0;
	Lua_PushNumber(L,Item[nItmeIndex].GetItemMagicLevel(nMlevel));
    return 1;
}
//加载爆率
int LuaLoldDropRate(Lua_State * L)
{
         int nParamCount = Lua_GetTopIndex(L);
         int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
         char *nDropFile=(char *)Lua_ValueToString(L, 2);

         if (nNpcIndex<=0 || nDropFile==NULL || nNpcIndex >= MAX_NPC)
		    return 0;

	     int nReg=0;
	     char nDropName[128]={0};
		 char nDestName[128]={0};
		// ZeroMemory(nDropName,sizeof(nDropName));
		 int changeModel=0;
		 if  (nParamCount>=3)
			 changeModel=(int)Lua_ValueToNumber(L,3);
		 //g_StrLower(Npc[nNpcIndex].ActionScript);
		 //g_StrLower(nMsg);
		 switch (changeModel)
		 {
		 case 1:
			 if (Utf8ToGbk(nDropFile, nDestName, sizeof(nDestName)))
                snprintf(nDropName, sizeof(nDropName), "%s", nDestName);
            else
                nDropName[0] = '\0';
			 break;
		 case 2:
			 snprintf(nDropName, sizeof(nDropName), "%s", GbkToUtf8(nDropFile).c_str());
			 break;
		 default:
			 sprintf(nDropName,"%s",nDropFile);
			 break;
		 }
		/*if (g_pNpcTemplate[Npc[nNpcIndex].m_NpcSettingIdx][0])  //如果没有刷过这个怪物
             nReg=g_pNpcTemplate[Npc[nNpcIndex].m_NpcSettingIdx][0]->InitDropRate(nNpcIndex,nDropName);
		else if (g_pNpcTemplate[Npc[nNpcIndex].m_NpcSettingIdx][Npc[nNpcIndex].m_Level])
			 nReg=g_pNpcTemplate[Npc[nNpcIndex].m_NpcSettingIdx][Npc[nNpcIndex].m_Level]->InitDropRate(nNpcIndex,nDropName);
		*/ 
	     KNpcTemplate* pNpcTemp = NULL;
		 if (pNpcTemp = NpcSet.GetTemplate(Npc[nNpcIndex].m_NpcSettingIdx, 0))
			 nReg=pNpcTemp->InitDropRate(nNpcIndex,nDropName);
		 else  if (pNpcTemp = NpcSet.GetTemplate(Npc[nNpcIndex].m_NpcSettingIdx, Npc[nNpcIndex].m_Level))
			 nReg=pNpcTemp->InitDropRate(nNpcIndex,nDropName);
		 else
		 { 
			 printf("----加载 NPC名称:%s,等级:%d,爆率:%s,失败----\n",Npc[nNpcIndex].Name,Npc[nNpcIndex].m_Level,nDropName);
		 } 

    Lua_PushNumber(L,nReg);
	return 1;
}

//获取这个NPC爆率文件
int LuaGetDropRateName(Lua_State * L)
{
	int nNpcIdex = 0;//(int)Lua_ValueToNumber(L, 1);	
	
	if  (lua_isnumber(L,1))
	{
		nNpcIdex= (int)Lua_ValueToNumber(L, 1);
	}
	else
	{
		char *Name=(char *)Lua_ValueToString(L,1);
		nNpcIdex=NpcSet.SearchName(Name);
	}
		
    if  (nNpcIdex<=0)
		return 0;
/*	
	if (Npc[nNpcIdex].m_pDropRate)
	{//有爆率的
		Lua_PushString(L,Npc[nNpcIdex].m_pDropRate->nDropFlie);
	    return 1;
	} */
    Lua_PushNumber(L,0);
	return 1;
}

//更新源爆率
int LuaUpdataDropRate(Lua_State * L)
{
	int nNpcIdex = 0;//(int)Lua_ValueToNumber(L, 1);


	if  (lua_isnumber(L,1))
	{
		nNpcIdex= (int)Lua_ValueToNumber(L, 1);
	}
	else
	{
		char *Name=(char *)Lua_ValueToString(L,1);
		nNpcIdex=NpcSet.SearchName(Name);
	}

	char *nDropFile=(char *)Lua_ValueToString(L, 2);

    if  (nNpcIdex<=0)
		return 0;

	int nReg=0;
	/*if (g_pNpcTemplate[Npc[nNpcIdex].m_NpcSettingIdx][0])  //如果没有刷过这个怪物
		nReg=g_pNpcTemplate[Npc[nNpcIdex].m_NpcSettingIdx][0]->gmUpdataDropRate(nNpcIdex,nDropFile);
	else if (g_pNpcTemplate[Npc[nNpcIdex].m_NpcSettingIdx][Npc[nNpcIdex].m_Level])
		nReg=g_pNpcTemplate[Npc[nNpcIdex].m_NpcSettingIdx][Npc[nNpcIdex].m_Level]->gmUpdataDropRate(nNpcIdex,nDropFile);
	 */
	KNpcTemplate* pNpcTemp = NULL;
	if (pNpcTemp = NpcSet.GetTemplate(Npc[nNpcIdex].m_NpcSettingIdx, 0))
		nReg=pNpcTemp->gmUpdataDropRate(nNpcIdex,nDropFile);
	else  if (pNpcTemp = NpcSet.GetTemplate(Npc[nNpcIdex].m_NpcSettingIdx, Npc[nNpcIdex].m_Level))
		nReg=pNpcTemp->gmUpdataDropRate(nNpcIdex,nDropFile); 
	else
	{
		printf("----更新索:%d,NPC名称:%s,等级:%d,爆率:%s,失败----\n",nNpcIdex,Npc[nNpcIdex].Name,Npc[nNpcIdex].m_Level,nDropFile);
	}
    Lua_PushNumber(L,nReg);
	return 1;
}

int LuaSetDeathPunish(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 1) 
		return 0;
	
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		int nState = Lua_ValueToNumber(L, 1);
		if (nState == 0)
			Npc[Player[nPlayerIndex].m_nIndex].SetPunishModel(0); // 被npc杀死0
		else if(nState == 1)
			Npc[Player[nPlayerIndex].m_nIndex].SetPunishModel(1); // 切磋模式被玩家杀死  1  没有惩罚
		else if(nState == 2)
			Npc[Player[nPlayerIndex].m_nIndex].SetPunishModel(2); // 被玩家PK致死，根据PK值进行惩罚2
		else if(nState == 3)
			Npc[Player[nPlayerIndex].m_nIndex].SetPunishModel(3); // 类式于国战时的惩罚处理 3
		else if(nState == 4)
			Npc[Player[nPlayerIndex].m_nIndex].SetPunishModel(4); // 经验惩罚模式 4
		else if(nState == 5)
			Npc[Player[nPlayerIndex].m_nIndex].SetPunishModel(5); // 银两惩罚模式 5
		else if(nState == 6)
			Npc[Player[nPlayerIndex].m_nIndex].SetPunishModel(6); // 装备惩罚模式 6
		else if(nState == 7)
			Npc[Player[nPlayerIndex].m_nIndex].SetPunishModel(7); // 金币惩罚模式 7
		else 
			Npc[Player[nPlayerIndex].m_nIndex].SetPunishModel(0); // 被npc杀死0

	}
	return 0;
}

int LuaGetDeathPunish(Lua_State * L)
{	
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex >0)
	Lua_PushNumber(L,Npc[Player[nPlayerIndex].m_nIndex].m_nCurPKPunishState); // 被npc杀死0
	return 1;
}

#endif   ////服务器端结束

int GetSubWorldIndex(Lua_State * L)
{
	Lua_GetGlobal(L, SCRIPT_SUBWORLDINDEX);
	if (lua_isnil(L,Lua_GetTopIndex(L)))
        return -1;
	int nIndex = (int)Lua_ValueToNumber(L, Lua_GetTopIndex(L));
    if (nIndex >= MAX_SUBWORLD || nIndex <0) 
	{
		//_ASSERT(0);
		return -1;
	}
	if (SubWorld[nIndex].m_nIndex >= MAX_SUBWORLD || SubWorld[nIndex].m_nIndex < 0)
	{
		//_ASSERT(0);
		return -1;
	}
    return nIndex;
}

//Idx = SubWorldID2Idx(dwID)
int LuaSubWorldIDToIndex(Lua_State * L)
{
	int nTargetSubWorld = -1;
	int nSubWorldID = 0;
	if (Lua_GetTopIndex(L) < 1)
		goto lab_subworldid2idx;
	
	nSubWorldID = (int)Lua_ValueToNumber(L, 1);
	nTargetSubWorld = g_SubWorldSet.SearchWorld(nSubWorldID); // 当前的地图属于服务器地图列表的第几行(索引)
	
lab_subworldid2idx:
	Lua_PushNumber(L, nTargetSubWorld);
	return 1;
}

int LuaIndexToSubWorldID(Lua_State * L)
{
	int nTargetSubWorld = -1;
	int nSubWorldindex = 0;
	if (Lua_GetTopIndex(L) < 1)
		goto lab_subworldid2idx;
	
	nSubWorldindex = (int)Lua_ValueToNumber(L, 1);
	nTargetSubWorld = g_SubWorldSet.SearchWorldRows(nSubWorldindex);
	
lab_subworldid2idx:
	Lua_PushNumber(L, nTargetSubWorld);
	return 1;
}





int LuaSetOrAddTrap(Lua_State * L)
{
    int nMapIdx,nXpos,nYpos,nCellNum,nReg=0;
	DWORD nScriptidx;
	      nMapIdx = (int)Lua_ValueToNumber(L,1);
		  nXpos = (int)Lua_ValueToNumber(L,2);
		  nYpos = (int)Lua_ValueToNumber(L,3);
		  nCellNum = (int)Lua_ValueToNumber(L,4);
		  nScriptidx = g_FileName2Id((char *)Lua_ValueToString(L,5));

          if (nScriptidx==0)
		  {
			  Lua_PushNumber(L,nReg);
	          return 1;
		  }

		  int nRegion, nMapX, nMapY, nOffX, nOffY;

		  if (nMapIdx < 0 || nMapIdx >= MAX_SUBWORLD) { Lua_PushNumber(L,0); return 1; }
		  SubWorld[nMapIdx].Mps2Map(nXpos, nYpos, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);

		  if (nRegion == -1)
		  {
			  Lua_PushNumber(L,nReg);
			  return 1;
		  }
		  
	      //return m_Region[nRegion].SetTrap(nMapX, nMapY,nCellNum,uTrapScriptId);
          //nReg = SubWorld[nMapIdx].SetTrap(nXpos,nYpos,nCellNum,nScriptidx);
		  nReg = SubWorld[nMapIdx].m_Region[nRegion].SetTrap(nMapX,nMapY,nCellNum,nScriptidx);
/*
          int nPlayerIndex   = GetPlayerIndex(L);

          if (nPlayerIndex>0)
		  {

#ifdef _SERVER			 
			  char msg[64];
			  sprintf(msg,"<color=yellow>当前坐标:R:%d,X:%d,Y:%d,Tr:%d",nRegion,nMapX,nMapY,nScriptidx);
              //Player[nPlayerIndex].m_ItemList.msgshow(msg);
			  //char nMsg[250];
			  //g_StrCpyLen(nMsg,  Lua_ValueToString(L,1), sizeof(nMsg));
			  int nleg=TEncodeText(msg, strlen(msg));
			  const char * szMsg = msg;
			  if (szMsg)
			  {
				  KPlayerChat::SendSystemInfo(0, 0, "测试:", (char *)szMsg, nleg);  //strlen(szMsg) 
			  }

#endif
		  }
*/
          Lua_PushNumber(L,nReg);
	      return 1;
}

int LuaAddTrap(Lua_State * L)
{
	int nSubWorldIndex = g_SubWorldSet.SearchWorld((int)Lua_ValueToNumber(L, 1));

	if (nSubWorldIndex == -1)
		return 0;

	int nRegion, nMapX, nMapY, nOffX, nOffY;

	SubWorld[nSubWorldIndex].Mps2Map((int)Lua_ValueToNumber(L, 2), (int)Lua_ValueToNumber(L, 3), &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);
	if (nRegion == -1)
		return 0; 

	DWORD dwTrapId = (DWORD)g_FileName2Id((char*)Lua_ValueToString(L, 4));
	SubWorld[nSubWorldIndex].m_Region[nRegion].SetTrapNew(dwTrapId, nMapX, nMapY);
	return 0;
} 

int LuaAddObjNew(Lua_State * L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 5)
	{
		Lua_PushNumber(L,0);
		return 1;
	}

	int nObjID = (int)Lua_ValueToNumber(L,1);
	if (nObjID <= 0 || nObjID >= ObjSet.m_cTabFile.GetHeight())
	{
		Lua_PushNumber(L,0);
		return 1;
	}
	int nSubWorldIndex	= g_SubWorldSet.SearchWorld((int)Lua_ValueToNumber(L,2));

	if (nSubWorldIndex == -1)
		return 0;

	KMapPos	Pos;

	Pos.nSubWorld = nSubWorldIndex;
	SubWorld[nSubWorldIndex].Mps2Map((int)Lua_ValueToNumber(L,3),(int)Lua_ValueToNumber(L,4), 
	&Pos.nRegion, &Pos.nMapX, &Pos.nMapY, 
	&Pos.nOffX, &Pos.nOffY);
	KObjItemInfo	sInfo;
	sInfo.m_nItemID = 0;
	sInfo.m_nItemWidth = 0;
	sInfo.m_nItemHeight = 0;
	sInfo.m_nMoneyNum = 0;
	sInfo.m_szName[0] = 0;
	sInfo.m_nColorID = 0;
//	sInfo.m_nGenre = 0;
//	sInfo.m_nDetailType = 0;
	sInfo.m_nMovieFlag = 1;
	sInfo.m_nSoundFlag = 1;
//	sInfo.m_bOverLook = 0;
	int nObj = ObjSet.Add(nObjID, Pos, sInfo);
	if (nObj <= 0) return 0;

	Object[nObj].SetScriptFile((char *)Lua_ValueToString(L,5));
	if (nParamNum > 5)
		Object[nObj].SetImageDir((int)Lua_ValueToNumber(L,6));
	if (nParamNum > 6)
		Object[nObj].SetState((int)Lua_ValueToNumber(L,7));

	Lua_PushNumber(L, nObj);
    return 1;
}

int LuaDelObjNew(Lua_State * L)
{
	if (Lua_GetTopIndex(L) <= 0 ) return 0 ;

	int nObjIndex = 0;
	if(Lua_GetTopIndex(L) >= 3)
	{
		int nSubWorldIndex	= g_SubWorldSet.SearchWorld((int)Lua_ValueToNumber(L,1));

		if (nSubWorldIndex == -1)
			return 0;

		KMapPos	Pos;

		Pos.nSubWorld = nSubWorldIndex;
		SubWorld[nSubWorldIndex].Mps2Map((int)Lua_ValueToNumber(L,2),(int)Lua_ValueToNumber(L,3), 
		&Pos.nRegion, &Pos.nMapX, &Pos.nMapY, 
		&Pos.nOffX, &Pos.nOffY);
		nObjIndex = ObjSet.SearchObjAt(Pos);
	}
	else
		nObjIndex = (int)Lua_ValueToNumber(L, 1);

	if (nObjIndex > 0 && nObjIndex < MAX_OBJECT)
	{
		Object[nObjIndex].SyncRemove(FALSE);
		if (Object[nObjIndex].m_nRegionIdx >= 0)
			SubWorld[Object[nObjIndex].m_nSubWorldID].m_Region[Object[nObjIndex].m_nRegionIdx].RemoveObj(nObjIndex);
		ObjSet.Remove(nObjIndex);
	}
	return 0;
}


int  LuaBuildATeam(Lua_State * L)
{//创建一支队伍
	int  _curcamp	= (int)Lua_ValueToNumber(L,1);
	int  _IsPlayr	= (int)Lua_ValueToNumber(L,2);
	int  _nOptionNum=0;
	 if (Lua_IsTable(L, 3))   // 是一个表跟
	{
		_nOptionNum = Lua_GetN(L,3);       //表中有多少个键
		if  (_nOptionNum<=0 ) return 0;
		int _captainIndex = 0; //队长得索引
		for (int i  = 0; i < _nOptionNum; i ++)
		{	
				Lua_PushNumber(L, i + 1);
				Lua_RawGet(L, 3);	 //出栈第三个参数
				int  _PlayerIndex  = (int)Lua_ValueToNumber(L, Lua_GetTopIndex(L));

				if (_PlayerIndex<=0 || _PlayerIndex>=MAX_PLAYER ) continue;

				//设置可以加入队伍
				Player[_PlayerIndex].m_cTeam.SetCanTeamFlag(_PlayerIndex, TRUE);

				if  (i==0) //队长
				{//创建一支队伍 
					_captainIndex = _PlayerIndex;
					Player[_PlayerIndex].m_cTeam.CreateTeam(_PlayerIndex,NULL,true);
					Npc[Player[_PlayerIndex].m_nIndex].SetCurrentCamp(_curcamp);
				}
				else
				{//队长把这个玩家增加到 队伍当中
					Player[_captainIndex].m_cTeam.GetInviteReply(_captainIndex,_PlayerIndex ,true,false);
				}
		}
	}

	 return 0;
}



int LuaFindNpcPosWithName(Lua_State * L)
{
	int  _mapID	= (int)Lua_ValueToNumber(L,1);
	char *strName= (char *)Lua_ValueToString(L,2);

	int x=0,y=0,reg=0;
	if( _mapID >= 0  && _mapID < MAX_SUBWORLD)
		reg = SubWorld[_mapID].FindNpcPosWithName(&x,&y,strName);

    Lua_PushNumber(L,reg);
	Lua_PushNumber(L,x);
	Lua_PushNumber(L,y);
    return 3;
}



int LuaClearMapNpc(Lua_State * L)
{
	int  _mapID	= (int)Lua_ValueToNumber(L,1);
	int  bOnlyDynamicAdd	= (int)Lua_ValueToNumber(L,2);


	if( _mapID >= 0  && _mapID < MAX_SUBWORLD)
		SubWorld[_mapID].ClearAllNpc(bOnlyDynamicAdd);

	return 0;
}

int LuaClearMapTrap(Lua_State * L)
{//清除所有的trap点
	int  _mapID	= (int)Lua_ValueToNumber(L,1);
	if (_mapID < 0 || _mapID >= MAX_SUBWORLD) return 0;
	SubWorld[_mapID].clearAlltrap();
	return 0;
}
//删除全部obj？
int LuaClearMapObj(Lua_State * L)
{
	int  _mapID	= (int)Lua_ValueToNumber(L,1);
	if (_mapID < 0 || _mapID >= MAX_SUBWORLD) return 0;
	SubWorld[_mapID].clearAllObj();
	return 0;
}

int LuaClearObstacleObj(Lua_State * L)
{
	int  _mapID	= (int)Lua_ValueToNumber(L,1);
	int  _mpsX	= (int)Lua_ValueToNumber(L,2);
	int  _mpsY	= (int)Lua_ValueToNumber(L,3);

	KMapPos	Pos;
	ZeroMemory(&Pos,sizeof(Pos));

	if (_mapID < 0 || _mapID >= MAX_SUBWORLD) return 0;
	SubWorld[_mapID].Mps2Map(_mpsX, _mpsY, 
		&Pos.nRegion, &Pos.nMapX, &Pos.nMapY, 
		&Pos.nOffX, &Pos.nOffY);

	SubWorld[_mapID].setBarrier(_mpsX,_mpsY,0);

	if (Pos.nRegion == -1)
		return 0;

	int nObjIndex=0;
	nObjIndex = SubWorld[_mapID].m_Region[Pos.nRegion].FindObject(Pos.nMapX,Pos.nMapY);

    if (nObjIndex)
		Object[nObjIndex].Remove(false);

	return 0;
}


int LuaAddMapTrap(Lua_State * L)
{
	int nReg=0;
	int  _mapID	= (int)Lua_ValueToNumber(L,1);
	int  _mpsX	= (int)Lua_ValueToNumber(L,2);
	int  _mpsY	= (int)Lua_ValueToNumber(L,3);
	char *  _scriptPath		= (char *)Lua_ValueToString(L,4);
	DWORD nScriptidx;
	 nScriptidx = g_FileName2Id(_scriptPath);

          if (nScriptidx==0)
		  {
			  Lua_PushNumber(L,0);
	          return 1;
		  }

		  int nRegion, nMapX, nMapY, nOffX, nOffY;
		  if (_mapID < 0 || _mapID >= MAX_SUBWORLD) { Lua_PushNumber(L,0); return 1; }
		  SubWorld[_mapID].Mps2Map(_mpsX, _mpsY, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);

		  if (nRegion == -1)
		  {
			  Lua_PushNumber(L,nReg);
			  return 1;
		  }
		  nReg = SubWorld[_mapID].m_Region[nRegion].SetTrap(nMapX,nMapY,nScriptidx);
          Lua_PushNumber(L,nReg);
	      return 1;
}

int LuaAddObstacleObj(Lua_State * L)
{
	int  _ObjID	    = (int)Lua_ValueToNumber(L,1);
	int  _mapID		= (int)Lua_ValueToNumber(L,2);
	int  _mpsX		= (int)Lua_ValueToNumber(L,3);
	int  _mpsY		= (int)Lua_ValueToNumber(L,4);
	KMapPos	Pos;
	ZeroMemory(&Pos,sizeof(Pos));


	Pos.nSubWorld = _mapID;

	if (_mapID < 0 || _mapID >= MAX_SUBWORLD) return 0;
	SubWorld[_mapID].Mps2Map(_mpsX, _mpsY, 
		&Pos.nRegion, &Pos.nMapX, &Pos.nMapY, 
		&Pos.nOffX, &Pos.nOffY);	

	SubWorld[_mapID].setBarrier(_mpsX,_mpsY,20);

	int nObj;
	//////掉下装备保存在KObjItemInfo//////结构体中的参数，以便调用
	KObjItemInfo sInfo;
	memset(&sInfo, 0, sizeof(sInfo));
	sInfo.m_nItemID = 0;
	sInfo.m_nItemWidth = 0;
	sInfo.m_nItemHeight = 0;
	sInfo.m_nMoneyNum = 0;
	//strcpy(sInfo.m_szName, nObjName); //物品的名称
	sInfo.m_nColorID    = 0;                      //颜色
	sInfo.m_nMovieFlag  = 1;
	sInfo.m_nSoundFlag  = 1;
	sInfo.m_sHaveAttack=0;   //是否设置为攻击无效了
	//sInfo.m_AttackerDwid=0;   //上次攻击着的DWID
	nObj = ObjSet.Add(_ObjID, Pos,sInfo);   //开始同步地上物品

	if (nObj == -1)
		ObjSet.Remove(nObj);
	else
		Object[nObj].SetItemBelong(-1);

	return 0;
}


int LuaSubWorldName(Lua_State * L)
{
	int nMapId;
	nMapId = (int)Lua_ValueToNumber(L,1);
    if (nMapId < 0 || nMapId >= MAX_SUBWORLD) { Lua_PushNumber(L,0); return 1; }
    Lua_PushString(L,SubWorld[nMapId].nWorldMapInfo[STR_MAP_NAME].c_str()); 
	return 1;
}


/*
Say(sMainInfo, nSelCount, sSel1, sSel2, sSel3, .....,sSeln) 
Say(nMainInfo, nSelCount, sSel1, sSel2, sSel3, .....,sSeln) 
Say(nMainInfo, nSelCount, SelTab)
如果是客户端的则不会向服务器端发送任何操作

  Say(100, 3, 10, 23,43)
  Say("选择什么？", 2, "是/yes", "否/no");
  Say("选什么呀", 2, SelTab);
*/
//**************************************************************************************************************************************************************
//												界面脚本
//**************************************************************************************************************************************************************


int LuaSayUI(Lua_State * L)
{

	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2) return 0;

	char * strMain  = NULL;
	int nMainInfo = 0;
	int nDataType = 0;
	int nOptionNum = 0;
	char * pContent = NULL;
	
	int nPlayerIndex ;
#ifdef _SERVER
	nPlayerIndex   = GetPlayerIndex(L);
#else
	nPlayerIndex   = CLIENT_PLAYER_INDEX;
#endif

	if (nPlayerIndex < 0) return 0;

//	Player[nPlayerIndex].m_bWaitingPlayerFeedBack = false;
	
	if (Lua_IsNumber(L,2))
	{
		nOptionNum = (int)Lua_ValueToNumber(L,2);
	}
	else 
	{
	//	_ASSERT(0);
		return 0;
	}
	
	if  (Lua_IsNumber(L,1))
	{
		nMainInfo = (int)Lua_ValueToNumber(L,1);
		nDataType = 1 ;
	}
	else if (Lua_IsString(L, 1)) 	//检查主信息是字符串还是字符串标识号
	{
		strMain = (char *)Lua_ValueToString(L, 1);
		nDataType = 0 ;
	}
	else
		return 0;
	
	BOOL bStringTab = FALSE;//标识传进来的选项数据存放在一个数组中，还是许多字符串里
	
	if (Lua_IsString(L,3))
		bStringTab = FALSE;
	else if (Lua_IsTable(L, 3))   // 是一个表跟
	{
		bStringTab = TRUE;
	}
	else
	{
		if (nOptionNum > 0)  return 0;
	}
	
	if (bStringTab == FALSE)
	{
		//获得实际传入的选项的个数
		if (nOptionNum > nParamNum - 2) nOptionNum = nParamNum - 2;
	}
	
	if (nOptionNum > MAX_ANSWERNUM) nOptionNum = MAX_ANSWERNUM;
	
	PLAYER_SCRIPTACTION_SYNC UiInfo;
	ZeroStruct(UiInfo);
	UiInfo.m_bUIId        = UI_SELECTDIALOG;
	UiInfo.m_bParam1      = nDataType;//主信息的类型，字符串(0)或数字(1)
	UiInfo.m_bOptionNum   = nOptionNum;
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
	UiInfo.m_Select       = 0;
//	Player[nPlayerIndex].m_nCheckWaiGua =0;
	
	//主信息为字符串
	if (nDataType == 0)
	{
		if (strMain)
			sprintf(UiInfo.m_pContent, "%s", strMain);
		pContent = UiInfo.m_pContent;
	}
	else if (nDataType == 1) //主信息为数字标识
	{
		*(int *)UiInfo.m_pContent = nMainInfo;
		pContent = UiInfo.m_pContent + sizeof(int);
		*pContent = 0;
	}
	
	if (nOptionNum > MAX_ANSWERNUM)
		nOptionNum = MAX_ANSWERNUM;
	
	Player[nPlayerIndex].m_nAvailableAnswerNum = nOptionNum;		
	
	for (int i  = 0; i < nOptionNum; i ++)
	{	
		char  pAnswer[64];
		ZeroMemory(pAnswer,sizeof(pAnswer));

		if (bStringTab)   //如果输进来的是Table
		{
			Lua_PushNumber(L, i + 1);
			Lua_RawGet(L, 3);	 //出栈第三个参数
			char * pszString = (char *)Lua_ValueToString(L, Lua_GetTopIndex(L));
			if (pszString)
			{
				g_StrCpyLen(pAnswer, pszString, 64);
			}
		}
		else 
		{
			char * pszString = (char *)Lua_ValueToString(L, i + 3);
			if (pszString)
				g_StrCpyLen(pAnswer, pszString, 64);
		}
		
		/*char * pFunName = strstr(pAnswer, "/"); //判断 字符窜 首次出现“/” 的位置!
	
		if (pFunName)
		{
			char *mpFunName=strstr(pFunName, "#");  //  stristr() 对大小写不敏感  取后面部分（包括符号）
			if (mpFunName) 
			{ //sscanf(nItemID, "%s(%s", &nTextA, &nTextB); 字符串分割
				char nmpFunName[32];
				char nmCanshu[32];
				//nmpFunName=strtok(mpFunName,"-");  //字符串分割   函数
				//nmCanshu=strtok(NULL,"-");        //参数
				g_ExtractChar(mpFunName,'(',')',nmpFunName,nmCanshu);
				//玩家携带的 函数名称 和 参数
				g_StrCpyLen(Player[nPlayerIndex].m_szTaskAnswerFun[i],nmpFunName+1, sizeof(Player[nPlayerIndex].m_szTaskAnswerFun[0]));
				g_StrCpyLen(Player[nPlayerIndex].m_szTaskAnswerCanshu[i],nmCanshu, sizeof(Player[nPlayerIndex].m_szTaskAnswerCanshu[0]));
				//nmpFunName=strtok(pAnswer, "/"); //去掉 斜杠  //取前面的部分
				g_ExtractChar(pAnswer,'/',')',nmpFunName,nmCanshu,1);
				sprintf(pContent, "%s|%s", pContent,nmpFunName);
				ZeroMemory(nmpFunName,sizeof(nmpFunName));
				ZeroMemory(nmCanshu,sizeof(nmCanshu));
			}
			else
			{
				g_StrCpyLen(Player[nPlayerIndex].m_szTaskAnswerCanshu[i],"", sizeof(Player[nPlayerIndex].m_szTaskAnswerCanshu[0]));
				g_StrCpyLen(Player[nPlayerIndex].m_szTaskAnswerFun[i], pFunName + 1, sizeof(Player[nPlayerIndex].m_szTaskAnswerFun[0]));
				*pFunName = 0;   //用完清空
				sprintf(pContent, "%s|%s", pContent, pAnswer);
			}
		}*/
		std::string _strSayNew=pAnswer;
		int  pos = _strSayNew.rfind("/");
		if (pos>=0)
		{//取后面的字符串
			std::string _Buffer_Qian =_strSayNew.substr(0,pos); // /前面部分
			std::string _Buffer_Hou = _strSayNew.substr(pos, _strSayNew.length()); // /后面部分
			int __pos = _Buffer_Hou.rfind("#");//从 0 开始
			if (__pos>=1)
			{//如果存在# 取后面的内容
				std::string Fun_Buffer_ = _Buffer_Hou.substr(__pos+1, _Buffer_Hou.length());
				//开始提取括号内的内容
				int fun_pos_begin =Fun_Buffer_.rfind("(");
				std::string _Fun_Name=Fun_Buffer_.substr(0, fun_pos_begin);
				int fun_pos_end   =Fun_Buffer_.rfind(")");
				std::string _Fun_CanShu=Fun_Buffer_.substr(fun_pos_begin+1,Fun_Buffer_.length());
				int fun_pos_end_temp   =_Fun_CanShu.rfind(")");
				std::string _Fun_CanShu_temp=_Fun_CanShu.substr(0,fun_pos_end_temp);

				g_StrCpyLen(Player[nPlayerIndex].m_szTaskAnswerFun[i],_Fun_Name.c_str(), sizeof(Player[nPlayerIndex].m_szTaskAnswerFun[i]));
				g_StrCpyLen(Player[nPlayerIndex].m_szTaskAnswerCanshu[i],_Fun_CanShu_temp.c_str(), sizeof(Player[nPlayerIndex].m_szTaskAnswerCanshu[i]));
				// printf("---脚本函数名:%s,%s,参数:%s------\n",_Buffer_Hou.c_str(),_Fun_Name.c_str(),_Fun_CanShu_temp.c_str());
				sprintf(pContent, "%s|%s", pContent,_Buffer_Qian.c_str());
			}
			else
			{
				std::string nTemp_Str_Hou=_Buffer_Hou.substr(1, _Buffer_Hou.length()); 
				g_StrCpyLen(Player[nPlayerIndex].m_szTaskAnswerCanshu[i],"-1", sizeof(Player[nPlayerIndex].m_szTaskAnswerCanshu[i]));//sizeof(Player[nPlayerIndex].m_szTaskAnswerCanshu[0])
				//脚本执行函数 / 后面部分
				g_StrCpyLen(Player[nPlayerIndex].m_szTaskAnswerFun[i],nTemp_Str_Hou.c_str(), sizeof(Player[nPlayerIndex].m_szTaskAnswerFun[i]));
				//函数名 前面部分
				sprintf(pContent, "%s|%s", pContent, _Buffer_Qian.c_str());
			}
		}
		else 
		{
			strcpy(Player[nPlayerIndex].m_szTaskAnswerFun[i], "main");
			sprintf(pContent, "%s|%s", pContent, pAnswer);
		}
	}
	
	if (nDataType == 0)
		UiInfo.m_nBufferLen  = strlen(pContent);
	else 
		UiInfo.m_nBufferLen = strlen(pContent) + sizeof(int);
	
#ifndef _SERVER
	UiInfo.m_bParam2 = 0;
#else
	UiInfo.m_bParam2 = 1;
#endif
	
/*	if (nOptionNum == 0)
	{
		Player[nPlayerIndex].m_bWaitingPlayerFeedBack = false;
	}
	else
	{
		Player[nPlayerIndex].m_bWaitingPlayerFeedBack = true;
	} */
	
	Player[nPlayerIndex].DoScriptAction(&UiInfo);
	return 0;
}


//创建带表格 特定显示SPR的对话框
int LuaCreateSprSay(Lua_State * L)
{

	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 1) 
		return 0;

	char * strMain  = NULL;
	int nMainInfo = 0;
	int nDataType = 0;
	int nOptionNum = 0;
	int nIsVal=0;
	int nIsTimeClose=0;
	char * pContent = NULL;
	char * pOptionStr=NULL;
	
	int nPlayerIndex ;
#ifdef _SERVER
	   nPlayerIndex   = GetPlayerIndex(L);
#else
       nPlayerIndex   = CLIENT_PLAYER_INDEX;
#endif
	   
	   if (nPlayerIndex < 0) return 0;

//	   Player[nPlayerIndex].m_bWaitingPlayerFeedBack = false;
	   



	   PLAYER_SCRIPTACTION_SYNC UiInfo;
	   ZeroStruct(UiInfo);

	   UiInfo.m_bUIId        = UI_SELECTDIALOG;
	   UiInfo.m_bParam1      = nDataType ;//主信息的类型，字符串(0)或数字(1)
	   // UiInfo.m_bOptionNum   = nOptionNum;
	   UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
	   UiInfo.m_Select       = 1;


	   if (Lua_IsTable(L, 1))  //表格数组
	   {
		       nOptionNum = Lua_GetN(L,1);       //表中有多少个键

	         if (nOptionNum > MAX_ANSWERNUM+2)
		         nOptionNum = MAX_ANSWERNUM+2;

			   Player[nPlayerIndex].m_nAvailableAnswerNum = nOptionNum-2;  //选项数	 减去第一个 内容说明

			   UiInfo.m_bOptionNum   = nOptionNum-2;

			   for (int i=0;i<nOptionNum+2;i++)  //如果是table
			   {
				   char pAnswer[64];
				   ZeroMemory(pAnswer,sizeof(pAnswer));

				   Lua_PushNumber(L,i + 1); //压入一个Key
				   //lua_settable(L, -3);   //设置KEY值,然后出栈
				   Lua_RawGet(L, 1);        //取得整个脚本传进来的第一个参数的这个对应TABLE的KEY值 然后出栈 
				   char * pszString = (char *)Lua_ValueToString(L, Lua_GetTopIndex(L));
				   //nCurNum=4;
				   if (pszString)
				   {
					   if (i==0)
					   {//NPC spr 路径
						   sprintf(UiInfo.m_szSprPath, "%s", pszString);
						   continue;
					   }
					   else if (i==1)
					   {//主要内容
						   sprintf(UiInfo.m_pContent, "%s", pszString);
						   pContent = UiInfo.m_pContent;
						   continue;
					   }
					   else
					     g_StrCpyLen(pAnswer, pszString, 64);

					   //i=i-1; //恢复标号

					  /* char * pFunName = strstr(pAnswer, "/");  // 查找首次出现 斜杠出现的位置 函数  对大小写敏感
					   
					   if (pFunName)
					   {
						   char *mpFunName=strstr(pFunName, "#");  //  stristr() 对大小写不敏感  取后面部分（包括符号）
						   if (mpFunName) 
						   { //sscanf(nItemID, "%s(%s", &nTextA, &nTextB); 字符串分割
							   char nmpFunName[32]={0};
							   char nmCanshu[32]={0};
							   //nmpFunName=strtok(mpFunName,"-");  //字符串分割   函数
							   //nmCanshu=strtok(NULL,"-");        //参数
							   g_ExtractChar(mpFunName,'(',')',nmpFunName,nmCanshu);
							   //玩家携带的 函数名称 和 参数
							   g_StrCpyLen(Player[nPlayerIndex].m_szTaskAnswerFun[i-2],nmpFunName+1, sizeof(Player[nPlayerIndex].m_szTaskAnswerFun[0]));
							   g_StrCpyLen(Player[nPlayerIndex].m_szTaskAnswerCanshu[i-2],nmCanshu, sizeof(Player[nPlayerIndex].m_szTaskAnswerCanshu[0]));
							   //nmpFunName=strtok(pAnswer, "/"); //去掉 斜杠  //取前面的部分
							   g_ExtractChar(pAnswer,'/',')',nmpFunName,nmCanshu,1);
							   sprintf(pContent, "%s|%s", pContent,nmpFunName);
							   ZeroMemory(nmpFunName,sizeof(nmpFunName));
							   ZeroMemory(nmCanshu,sizeof(nmCanshu));
						   }
						   else
						   {
							   g_StrCpyLen(Player[nPlayerIndex].m_szTaskAnswerCanshu[i-2],"", sizeof(Player[nPlayerIndex].m_szTaskAnswerCanshu[0]));
							   g_StrCpyLen(Player[nPlayerIndex].m_szTaskAnswerFun[i-2], pFunName + 1, sizeof(Player[nPlayerIndex].m_szTaskAnswerFun[0]));
							   *pFunName = 0;   //用完清空
							   sprintf(pContent, "%s|%s", pContent, pAnswer);
						   }
					   }*/
					   std::string _strSayNew=pAnswer;
					   int  pos = _strSayNew.rfind("/");
					   if (pos>=0)
					   {//取后面的字符串
						   std::string _Buffer_Qian =_strSayNew.substr(0,pos); // /前面部分
						   std::string _Buffer_Hou = _strSayNew.substr(pos, _strSayNew.length()); // /后面部分
						   int __pos = _Buffer_Hou.rfind("#");//从 0 开始
						   if (__pos>=1)
						   {//如果存在# 取后面的内容
							   std::string Fun_Buffer_ = _Buffer_Hou.substr(__pos+1, _Buffer_Hou.length());
							   //开始提取括号内的内容
							   int fun_pos_begin =Fun_Buffer_.rfind("(");
							   std::string _Fun_Name=Fun_Buffer_.substr(0, fun_pos_begin);
							   int fun_pos_end   =Fun_Buffer_.rfind(")");
							   std::string _Fun_CanShu=Fun_Buffer_.substr(fun_pos_begin+1,Fun_Buffer_.length());
							   int fun_pos_end_temp   =_Fun_CanShu.rfind(")");
							   std::string _Fun_CanShu_temp=_Fun_CanShu.substr(0,fun_pos_end_temp);

							   g_StrCpyLen(Player[nPlayerIndex].m_szTaskAnswerFun[i-2],_Fun_Name.c_str(), sizeof(Player[nPlayerIndex].m_szTaskAnswerFun[i-2]));
							   g_StrCpyLen(Player[nPlayerIndex].m_szTaskAnswerCanshu[i-2],_Fun_CanShu_temp.c_str(), sizeof(Player[nPlayerIndex].m_szTaskAnswerCanshu[i-2]));
							   // printf("---脚本函数名:%s,%s,参数:%s------\n",_Buffer_Hou.c_str(),_Fun_Name.c_str(),_Fun_CanShu_temp.c_str());
							   sprintf(pContent, "%s|%s", pContent,_Buffer_Qian.c_str());
						   }
						   else
						   {
							   std::string nTemp_Str_Hou=_Buffer_Hou.substr(1, _Buffer_Hou.length()); 
							   g_StrCpyLen(Player[nPlayerIndex].m_szTaskAnswerCanshu[i-2],"-1", sizeof(Player[nPlayerIndex].m_szTaskAnswerCanshu[i-2]));//sizeof(Player[nPlayerIndex].m_szTaskAnswerCanshu[0])
							   //脚本执行函数 / 后面部分
							   g_StrCpyLen(Player[nPlayerIndex].m_szTaskAnswerFun[i-2],nTemp_Str_Hou.c_str(), sizeof(Player[nPlayerIndex].m_szTaskAnswerFun[i-2]));
							   //函数名 前面部分
							   sprintf(pContent, "%s|%s", pContent, _Buffer_Qian.c_str());
						   }
					   }
					   else 
					   {
						   strcpy(Player[nPlayerIndex].m_szTaskAnswerFun[i-2], "main");  //执行Main函数
						   sprintf(pContent, "%s|%s", pContent, pAnswer);
					   } 
				   }

			   }
			   


			   if (nDataType == 0)//主信息的类型，字符串(0)或数字(1)
				   UiInfo.m_nBufferLen  = strlen(pContent);
			   else 
				   UiInfo.m_nBufferLen = strlen(pContent) + sizeof(int);
			   
#ifndef _SERVER
			   UiInfo.m_bParam2 = 0;  //客户端
#else
			   if (nIsTimeClose>=1)
				   UiInfo.m_bParam2 = nIsTimeClose;  //服务器端	  秒
			   else
				   UiInfo.m_bParam2=1;
			   
			   //printf("==%s==\n",pContent);
#endif
			   
/*			   if (nOptionNum == 0)
			   {
				   Player[nPlayerIndex].m_bWaitingPlayerFeedBack = false;
			   }
			   else
			   {
				   Player[nPlayerIndex].m_bWaitingPlayerFeedBack = true;
			   } */
			   
			   Player[nPlayerIndex].DoScriptAction(&UiInfo);//要求显示某个UI界面

	   }

  return 0;

}



//新对话框
int LuaCreateTaskSay(Lua_State * L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 1) 
		return 0;

	char * strMain  = NULL;
	int nMainInfo = 0;
	int nDataType = 0;
	int nOptionNum = 0;
	int nIsVal=0;
	int nIsTimeClose=0;
	char * pContent = NULL;
	char * pOptionStr=NULL;
	
	int nPlayerIndex ;
#ifdef _SERVER
	   nPlayerIndex   = GetPlayerIndex(L);
#else
       nPlayerIndex   = CLIENT_PLAYER_INDEX;
#endif
	   
	   if (nPlayerIndex < 0) return 0;

//	   Player[nPlayerIndex].m_bWaitingPlayerFeedBack = false;
	   

	   PLAYER_SCRIPTACTION_SYNC UiInfo;
	   ZeroStruct(UiInfo);
	   UiInfo.m_bUIId        = UI_SELECTDIALOG;
	   UiInfo.m_bParam1      = nDataType ;//主信息的类型，字符串(0)或数字(1)
	   // UiInfo.m_bOptionNum   = nOptionNum;
	   UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
	   UiInfo.m_Select       = 1;


	   if (Lua_IsTable(L, 1))  //表格数组
	   {
		       nOptionNum = Lua_GetN(L,1);       //表中有多少个键

	         if (nOptionNum > MAX_ANSWERNUM+1)
		         nOptionNum = MAX_ANSWERNUM+1;


			   Player[nPlayerIndex].m_nAvailableAnswerNum = nOptionNum-1;  //选项数	 减去第一个 内容说明

			   UiInfo.m_bOptionNum   = nOptionNum-1;

			   for (int i=0;i<nOptionNum+1;i++)  //如果是table
			   {
				   char pAnswer[64];
				   ZeroMemory(pAnswer,sizeof(pAnswer));

				   Lua_PushNumber(L,i + 1); //压入一个Key
				   //lua_settable(L, -3);   //设置KEY值,然后出栈
				   Lua_RawGet(L, 1);        //取得整个脚本传进来的第一个参数的这个对应TABLE的KEY值 然后出栈 
				   char * pszString = (char *)Lua_ValueToString(L, Lua_GetTopIndex(L));
				   //nCurNum=4;
				   if (pszString)
				   {
					   if (i==0)
					   {//标题
						   sprintf(UiInfo.m_pContent, "%s", pszString);
						   pContent = UiInfo.m_pContent;
						   continue;
					   }
					   else//选项
					     g_StrCpyLen(pAnswer, pszString, 64);

					   //i=i-1; //恢复标号
					  
					  /* char * pFunName = strstr(pAnswer, "/");  // 查找首次出现 斜杠出现的位置 函数  对大小写敏感
					   
					   if (pFunName)
					   {
						   char *mpFunName=strstr(pFunName, "#");  //  stristr() 对大小写不敏感  取后面部分（包括符号）
						   if (mpFunName) 
						   { //sscanf(nItemID, "%s(%s", &nTextA, &nTextB); 字符串分割
							   char nmpFunName[32]={0};
							   char nmCanshu[32]={0};
							   //nmpFunName=strtok(mpFunName,"-");  //字符串分割   函数
							   //nmCanshu=strtok(NULL,"-");        //参数
							   g_ExtractChar(mpFunName,'(',')',nmpFunName,nmCanshu);
							   //玩家携带的 函数名称 和 参数
							   g_StrCpyLen(Player[nPlayerIndex].m_szTaskAnswerFun[i-1],nmpFunName+1, sizeof(Player[nPlayerIndex].m_szTaskAnswerFun[0]));
							   g_StrCpyLen(Player[nPlayerIndex].m_szTaskAnswerCanshu[i-1],nmCanshu, sizeof(Player[nPlayerIndex].m_szTaskAnswerCanshu[0]));
							   //nmpFunName=strtok(pAnswer, "/"); //去掉 斜杠  //取前面的部分
							   g_ExtractChar(pAnswer,'/',')',nmpFunName,nmCanshu,1);
							   sprintf(pContent, "%s|%s", pContent,nmpFunName);
							   ZeroMemory(nmpFunName,sizeof(nmpFunName));
							   ZeroMemory(nmCanshu,sizeof(nmCanshu));
						   }
						   else
						   {
							   g_StrCpyLen(Player[nPlayerIndex].m_szTaskAnswerCanshu[i-1],"", sizeof(Player[nPlayerIndex].m_szTaskAnswerCanshu[0]));
							   g_StrCpyLen(Player[nPlayerIndex].m_szTaskAnswerFun[i-1], pFunName + 1, sizeof(Player[nPlayerIndex].m_szTaskAnswerFun[0]));
							   *pFunName = 0;   //用完清空
							   sprintf(pContent, "%s|%s", pContent, pAnswer);
						   }
					   }*/
					   std::string _strSayNew=pAnswer;
					   int  pos = _strSayNew.rfind("/");
					   if (pos>=0)
					   {//取后面的字符串
						   std::string _Buffer_Qian =_strSayNew.substr(0,pos); // /前面部分
						   std::string _Buffer_Hou = _strSayNew.substr(pos, _strSayNew.length()); // /后面部分
						   int __pos = _Buffer_Hou.rfind("#");//从 0 开始
						   if (__pos>=1)
						   {//如果存在# 取后面的内容
							   std::string Fun_Buffer_ = _Buffer_Hou.substr(__pos+1, _Buffer_Hou.length());
							   //开始提取括号内的内容
							   int fun_pos_begin =Fun_Buffer_.rfind("(");
							   std::string _Fun_Name=Fun_Buffer_.substr(0, fun_pos_begin);
							   int fun_pos_end   =Fun_Buffer_.rfind(")");
							   std::string _Fun_CanShu=Fun_Buffer_.substr(fun_pos_begin+1,Fun_Buffer_.length());
							   int fun_pos_end_temp   =_Fun_CanShu.rfind(")");
							   std::string _Fun_CanShu_temp=_Fun_CanShu.substr(0,fun_pos_end_temp);

							   g_StrCpyLen(Player[nPlayerIndex].m_szTaskAnswerFun[i-1],_Fun_Name.c_str(), sizeof(Player[nPlayerIndex].m_szTaskAnswerFun[i-1]));
							   g_StrCpyLen(Player[nPlayerIndex].m_szTaskAnswerCanshu[i-1],_Fun_CanShu_temp.c_str(), sizeof(Player[nPlayerIndex].m_szTaskAnswerCanshu[i-1]));
							  // printf("---脚本函数名:%s,%s,参数:%s------\n",_Buffer_Hou.c_str(),_Fun_Name.c_str(),_Fun_CanShu_temp.c_str());
							   sprintf(pContent, "%s|%s", pContent,_Buffer_Qian.c_str());
						   }
						   else
						   {
							   std::string nTemp_Str_Hou=_Buffer_Hou.substr(1, _Buffer_Hou.length()); 
							   g_StrCpyLen(Player[nPlayerIndex].m_szTaskAnswerCanshu[i-1],"-1", sizeof(Player[nPlayerIndex].m_szTaskAnswerCanshu[i-1]));//sizeof(Player[nPlayerIndex].m_szTaskAnswerCanshu[0])
							   //脚本执行函数 / 后面部分
							   g_StrCpyLen(Player[nPlayerIndex].m_szTaskAnswerFun[i-1],nTemp_Str_Hou.c_str(), sizeof(Player[nPlayerIndex].m_szTaskAnswerFun[i-1]));
							   //函数名 前面部分
							   sprintf(pContent, "%s|%s", pContent, _Buffer_Qian.c_str());
						   }
					   }
					   else 
					   {
						   strcpy(Player[nPlayerIndex].m_szTaskAnswerFun[i-1], "main");  //执行Main函数
						   sprintf(pContent, "%s|%s", pContent, pAnswer);
					   } 
				   }

			   }
			   


			   if (nDataType == 0)//主信息的类型，字符串(0)或数字(1)
				   UiInfo.m_nBufferLen  = strlen(pContent);
			   else 
				   UiInfo.m_nBufferLen = strlen(pContent) + sizeof(int);
			   
#ifndef _SERVER
			   UiInfo.m_bParam2 = 0;  //客户端
#else
			   if (nIsTimeClose>=1)
				   UiInfo.m_bParam2 = nIsTimeClose;  //服务器端	  秒
			   else
				   UiInfo.m_bParam2=1;
			   
			   //printf("==%s==\n",pContent);
#endif
			   
/*			   if (nOptionNum == 0)
			   {
				   Player[nPlayerIndex].m_bWaitingPlayerFeedBack = false;
			   }
			   else
			   {
				   Player[nPlayerIndex].m_bWaitingPlayerFeedBack = true;
			   }*/
			   
			   Player[nPlayerIndex].DoScriptAction(&UiInfo);//要求显示某个UI界面

	   }

  return 0;
}


//--------------------------
int LuaCreateNewSayEx(Lua_State * L)
{

	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2) 
		return 0;

   char * strMain  = NULL;
	int nMainInfo = 0;
	int nDataType = 0;
	int nOptionNum = 0;
	int nIsVal=0;
	int nIsTimeClose=0;
	char * pContent = NULL;
	char * pOptionStr=NULL;
	
	int nPlayerIndex ;
#ifdef _SERVER
	   nPlayerIndex   = GetPlayerIndex(L);
#else
       nPlayerIndex   = CLIENT_PLAYER_INDEX;
#endif
	   
	   if (nPlayerIndex < 0) return 0;

//	   Player[nPlayerIndex].m_bWaitingPlayerFeedBack = false;
	   
	   if  (Lua_IsNumber(L,1))                       //数字
	   {
		   nMainInfo = (int)Lua_ValueToNumber(L,1);  //主要信息
		   nDataType = 1 ;
	   }
	   else if (Lua_IsString(L, 1)) 	//检查主信息是字符串还是字符串标识号  字符串
	   {
		   strMain = (char *)Lua_ValueToString(L, 1);
		   nDataType = 0 ;
	   }
	   else
		   return 0;


	   PLAYER_SCRIPTACTION_SYNC UiInfo;
	   ZeroStruct(UiInfo);
	   UiInfo.m_bUIId        = UI_SELECTDIALOG;
	   UiInfo.m_bParam1      = nDataType ;//主信息的类型，字符串(0)或数字(1)
	   // UiInfo.m_bOptionNum   = nOptionNum;
	   UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
	   UiInfo.m_Select       = 1;

	  	//主信息为字符串
	  if (nDataType == 0)
	  {  
		if (strMain)
			sprintf(UiInfo.m_pContent, "%s", strMain);
		pContent = UiInfo.m_pContent;
	  }  
	  else if (nDataType == 1) //主信息为数字标识
	  {  
		*(int *)UiInfo.m_pContent = nMainInfo;
		pContent = UiInfo.m_pContent + sizeof(int);
		*pContent = 0;
	  }  


	   if (Lua_IsTable(L, 2))  //表格数组
	   {
		       nOptionNum = Lua_GetN(L,2);       //有多少个选项

	         if (nOptionNum > MAX_ANSWERNUM)
		         nOptionNum = MAX_ANSWERNUM;


			   Player[nPlayerIndex].m_nAvailableAnswerNum = nOptionNum;  //选项数	 减去第一个 内容说明

			   UiInfo.m_bOptionNum   = nOptionNum;

			   for (int i=0;i<nOptionNum;i++)  //如果是table
			   {
				   char pAnswer[64];
				   ZeroMemory(pAnswer,sizeof(pAnswer));

				   Lua_PushNumber(L,i + 1); //压入一个Key
				   //lua_settable(L, -3);   //设置KEY值,然后出栈
				   Lua_RawGet(L, 2);        //取得整个脚本传进来的第一个参数的这个对应TABLE的KEY值 然后出栈 
				   char * pszString = (char *)Lua_ValueToString(L, Lua_GetTopIndex(L));
				   //nCurNum=4;
				   if (pszString)
				   {
					  
					   g_StrCpyLen(pAnswer, pszString, 64);

					   //i=i-1; //恢复标号

					  /* char * pFunName = strstr(pAnswer, "/");  // 查找首次出现 斜杠出现的位置 函数  对大小写敏感
					   
					   if (pFunName)
					   {
						   char *mpFunName=strstr(pFunName, "#");  //  stristr() 对大小写不敏感  取后面部分（包括符号）
						   if (mpFunName) 
						   { //sscanf(nItemID, "%s(%s", &nTextA, &nTextB); 字符串分割
							   char nmpFunName[32]={0};
							   char nmCanshu[32]={0};
							   //nmpFunName=strtok(mpFunName,"-");  //字符串分割   函数
							   //nmCanshu=strtok(NULL,"-");        //参数
							   g_ExtractChar(mpFunName,'(',')',nmpFunName,nmCanshu);
							   //玩家携带的 函数名称 和 参数
							   g_StrCpyLen(Player[nPlayerIndex].m_szTaskAnswerFun[i-1],nmpFunName+1, sizeof(Player[nPlayerIndex].m_szTaskAnswerFun[0]));
							   g_StrCpyLen(Player[nPlayerIndex].m_szTaskAnswerCanshu[i-1],nmCanshu, sizeof(Player[nPlayerIndex].m_szTaskAnswerCanshu[0]));
							   //nmpFunName=strtok(pAnswer, "/"); //去掉 斜杠  //取前面的部分
							   g_ExtractChar(pAnswer,'/',')',nmpFunName,nmCanshu,1);
							   sprintf(pContent, "%s|%s", pContent,nmpFunName);
							   ZeroMemory(nmpFunName,sizeof(nmpFunName));
							   ZeroMemory(nmCanshu,sizeof(nmCanshu));
						   }
						   else
						   {
							   g_StrCpyLen(Player[nPlayerIndex].m_szTaskAnswerCanshu[i-1],"", sizeof(Player[nPlayerIndex].m_szTaskAnswerCanshu[0]));
							   g_StrCpyLen(Player[nPlayerIndex].m_szTaskAnswerFun[i-1], pFunName + 1, sizeof(Player[nPlayerIndex].m_szTaskAnswerFun[0]));
							   *pFunName = 0;   //用完清空
							   sprintf(pContent, "%s|%s", pContent, pAnswer);
						   }
					   }*/
					   std::string _strSayNew=pAnswer;
					   int  pos = _strSayNew.rfind("/");
					   if (pos>=0)
					   {//取后面的字符串
						   std::string _Buffer_Qian =_strSayNew.substr(0,pos); // /前面部分
						   std::string _Buffer_Hou = _strSayNew.substr(pos, _strSayNew.length()); // /后面部分
						   int __pos = _Buffer_Hou.rfind("#");//从 0 开始
						   if (__pos>=1)
						   {//如果存在# 取后面的内容
							   std::string Fun_Buffer_ = _Buffer_Hou.substr(__pos+1, _Buffer_Hou.length());
							   //开始提取括号内的内容
							   int fun_pos_begin =Fun_Buffer_.rfind("(");
							   std::string _Fun_Name=Fun_Buffer_.substr(0, fun_pos_begin);
							   int fun_pos_end   =Fun_Buffer_.rfind(")");
							   std::string _Fun_CanShu=Fun_Buffer_.substr(fun_pos_begin+1,Fun_Buffer_.length());
							   int fun_pos_end_temp   =_Fun_CanShu.rfind(")");
							   std::string _Fun_CanShu_temp=_Fun_CanShu.substr(0,fun_pos_end_temp);

							   g_StrCpyLen(Player[nPlayerIndex].m_szTaskAnswerFun[i-1],_Fun_Name.c_str(), sizeof(Player[nPlayerIndex].m_szTaskAnswerFun[i-1]));
							   g_StrCpyLen(Player[nPlayerIndex].m_szTaskAnswerCanshu[i-1],_Fun_CanShu_temp.c_str(), sizeof(Player[nPlayerIndex].m_szTaskAnswerCanshu[i-1]));
							   // printf("---脚本函数名:%s,%s,参数:%s------\n",_Buffer_Hou.c_str(),_Fun_Name.c_str(),_Fun_CanShu_temp.c_str());
							   sprintf(pContent, "%s|%s", pContent,_Buffer_Qian.c_str());
						   }
						   else
						   {
							   std::string nTemp_Str_Hou=_Buffer_Hou.substr(1, _Buffer_Hou.length()); 
							   g_StrCpyLen(Player[nPlayerIndex].m_szTaskAnswerCanshu[i-1],"-1", sizeof(Player[nPlayerIndex].m_szTaskAnswerCanshu[i-1]));//sizeof(Player[nPlayerIndex].m_szTaskAnswerCanshu[0])
							   //脚本执行函数 / 后面部分
							   g_StrCpyLen(Player[nPlayerIndex].m_szTaskAnswerFun[i-1],nTemp_Str_Hou.c_str(), sizeof(Player[nPlayerIndex].m_szTaskAnswerFun[i-1]));
							   //函数名 前面部分
							   sprintf(pContent, "%s|%s", pContent, _Buffer_Qian.c_str());
						   }
					   }
					   else 
					   {
						   strcpy(Player[nPlayerIndex].m_szTaskAnswerFun[i-1], "main");  //执行Main函数
						   sprintf(pContent, "%s|%s", pContent, pAnswer);
					   } 
				   }

			   }
			   
			   if (nDataType == 0)//主信息的类型，字符串(0)或数字(1)
				   UiInfo.m_nBufferLen  = strlen(pContent);
			   else 
				   UiInfo.m_nBufferLen = strlen(pContent) + sizeof(int);
			   
#ifndef _SERVER
			   UiInfo.m_bParam2 = 0;  //客户端
#else
			   if (nIsTimeClose>=1)
				   UiInfo.m_bParam2 = nIsTimeClose;  //服务器端	  秒
			   else
				   UiInfo.m_bParam2=1;
			   
			   //printf("==%s==\n",pContent);
#endif
			   
/*			   if (nOptionNum == 0)
			   {
				   Player[nPlayerIndex].m_bWaitingPlayerFeedBack = false;
			   }
			   else
			   {
				   Player[nPlayerIndex].m_bWaitingPlayerFeedBack = true;
			   }*/
			   
			   Player[nPlayerIndex].DoScriptAction(&UiInfo);//要求显示某个UI界面

	   }

  return 0;

}

//--------------------------
int LuaSayNew(Lua_State * L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2) 
		return 0;

	char * strMain  = NULL;
	int nMainInfo = 0;
	int nDataType = 0;
	int nOptionNum = 0;
	int nIsVal=0;
	int nIsTimeClose=0;
	char * pContent = NULL;
	char * pOptionStr=NULL;
	
	int nPlayerIndex ;
#ifdef _SERVER
	   nPlayerIndex   = GetPlayerIndex(L);
#else
       nPlayerIndex   = CLIENT_PLAYER_INDEX;
#endif

	if (nPlayerIndex < 0) return 0;
//	Player[nPlayerIndex].m_bWaitingPlayerFeedBack = false;
	
	if (Lua_IsNumber(L,2))  //第二个参数为 数字
	{
		nOptionNum = (int)Lua_ValueToNumber(L,2);  //有几个链接信息
//		Player[nPlayerIndex].m_nCheckWaiGua=0;
	}
	else 
	{//否则为字符窜
	//	
		 pOptionStr=(char *)Lua_ValueToString(L,2);
		 if (pOptionStr && strstr(pOptionStr,"|"))
		    sscanf(pOptionStr,"%d|%d|%d",&nOptionNum,&nIsVal,&nIsTimeClose);
		 else
			return 0;
	}
	
	if  (Lua_IsNumber(L,1))  //数字
	{
		nMainInfo = (int)Lua_ValueToNumber(L,1);  //主要信息
		nDataType = 1 ;
	}
	else if (Lua_IsString(L, 1)) 	//检查主信息是字符串还是字符串标识号  字符串
	{
		strMain = (char *)Lua_ValueToString(L, 1);
		nDataType = 0 ;
	}
	else
		return 0;
	
	BOOL bStringTab = FALSE;//标识传进来的选项数据存放在一个数组中，还是许多字符串里
	
	if (Lua_IsString(L,3)) //第三个参数字符串 返回假
		bStringTab = FALSE;
	else if (Lua_IsTable(L, 3))  //表格数组
	{
		bStringTab = TRUE;
	}
	else 
	{
		if (nOptionNum > 0)  return 0;
	}
	
	if (bStringTab == FALSE) //非table
	{
		//获得实际传入的选项的个数
		if (nOptionNum > nParamNum - 2)   //减去前面两个参数
			nOptionNum = nParamNum - 2;
	}
	
	if (nOptionNum > MAX_ANSWERNUM) 
		nOptionNum = MAX_ANSWERNUM; //选项显示限制
	
	PLAYER_SCRIPTACTION_SYNC UiInfo;
	ZeroStruct(UiInfo);
	UiInfo.m_bUIId        = UI_SELECTDIALOG;
	UiInfo.m_bParam1      = nDataType;//主信息的类型，字符串(0)或数字(1)
	UiInfo.m_bOptionNum   = nOptionNum;
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
	UiInfo.m_Select       = 1;
	
	//主信息为字符串
	if (nDataType == 0)
	{
		if (strMain)
			sprintf(UiInfo.m_pContent, "%s", strMain);

		pContent = UiInfo.m_pContent;

		//TEncodeText(UiInfo.m_pContent, strlen(UiInfo.m_pContent));

	}
	else if (nDataType == 1) //主信息为数字标识
	{
		*(int *)UiInfo.m_pContent = nMainInfo;
		pContent = UiInfo.m_pContent + sizeof(int);
		*pContent = 0;
	}
	
	if (nOptionNum > MAX_ANSWERNUM)
		nOptionNum = MAX_ANSWERNUM;
	
	Player[nPlayerIndex].m_nAvailableAnswerNum = nOptionNum;  //选项数
	
	//int nCurNum=4;

	for (int i  = 0; i < nOptionNum; i++)
	{	
		char  pAnswer[64];
		ZeroMemory(pAnswer,sizeof(pAnswer));
		//pAnswer[0]=0;
		
		if (bStringTab)  //如果是table
		{
			Lua_PushNumber(L,i + 1); //压入一个Key
			Lua_RawGet(L, 3);        //取得整个脚本传进来的第三个参数的这个对应TABLE的KEY值 然后出栈  
			char * pszString = (char *)Lua_ValueToString(L, Lua_GetTopIndex(L));
			    // nCurNum=4;
			if (pszString)
			{
				g_StrCpyLen(pAnswer, pszString, 64);
			}
		}
		else  //否则就是单纯的字符串
		{
			char * pszString = (char *)Lua_ValueToString(L, i + 3);	//从第三个参数开始
			       //nCurNum=i + 4;
			if (pszString)
				g_StrCpyLen(pAnswer, pszString, 64);
		}

		std::string _strSayNew=pAnswer;
		//_strSayNew.find
		int  pos = _strSayNew.rfind("/");
		if (pos>=0)
		{//取后面的字符串
			std::string _Buffer_Qian =_strSayNew.substr(0,pos); // /前面部分
			std::string _Buffer_Hou = _strSayNew.substr(pos, _strSayNew.length()); // /后面部分
			int __pos = _Buffer_Hou.rfind("#");//从 0 开始
			if (__pos>=1)
			{//如果存在# 取后面的内容
				std::string Fun_Buffer_ = _Buffer_Hou.substr(__pos+1, _Buffer_Hou.length());
			    //开始提取括号内的内容
				int fun_pos_begin =Fun_Buffer_.rfind("(");
				std::string _Fun_Name=Fun_Buffer_.substr(0, fun_pos_begin);
				int fun_pos_end   =Fun_Buffer_.rfind(")");
				std::string _Fun_CanShu=Fun_Buffer_.substr(fun_pos_begin+1,Fun_Buffer_.length());
				int fun_pos_end_temp   =_Fun_CanShu.rfind(")");
				std::string _Fun_CanShu_temp=_Fun_CanShu.substr(0,fun_pos_end_temp);

				g_StrCpyLen(Player[nPlayerIndex].m_szTaskAnswerFun[i],_Fun_Name.c_str(), sizeof(Player[nPlayerIndex].m_szTaskAnswerFun[i]));
				g_StrCpyLen(Player[nPlayerIndex].m_szTaskAnswerCanshu[i],_Fun_CanShu_temp.c_str(), sizeof(Player[nPlayerIndex].m_szTaskAnswerCanshu[i]));
				//printf("---脚本函数名:%s,%s,参数:%s------\n",_Buffer_Hou.c_str(),_Fun_Name.c_str(),_Fun_CanShu_temp.c_str());
				sprintf(pContent, "%s|%s", pContent,_Buffer_Qian.c_str());
			}
			else
			{
				std::string nTemp_Str_Hou=_Buffer_Hou.substr(1, _Buffer_Hou.length()); 
				g_StrCpyLen(Player[nPlayerIndex].m_szTaskAnswerCanshu[i],"-1", sizeof(Player[nPlayerIndex].m_szTaskAnswerCanshu[i]));//sizeof(Player[nPlayerIndex].m_szTaskAnswerCanshu[0])
				//脚本执行函数 / 后面部分
				g_StrCpyLen(Player[nPlayerIndex].m_szTaskAnswerFun[i],nTemp_Str_Hou.c_str(), sizeof(Player[nPlayerIndex].m_szTaskAnswerFun[i]));
				//函数名 前面部分
				sprintf(pContent, "%s|%s", pContent, _Buffer_Qian.c_str());
		    }
		}
		else
		{
			strcpy(Player[nPlayerIndex].m_szTaskAnswerFun[i], "main");  //执行Main函数
			sprintf(pContent, "%s|%s", pContent, pAnswer);
	     }
//---------------------------------------------------------
       /* char * pFunName = strstr(pAnswer,"/");  // 查找首次出现 斜杠出现的位置 函数  对大小写敏感
		if (pFunName)
		{
			char *mpFunName=strstr(pFunName, "#");  //  stristr() 对大小写不敏感  取后面部分（包括符号）
			if (mpFunName) 
			{ //sscanf(nItemID, "%s(%s", &nTextA, &nTextB); 字符串分割
				char nmpFunName[32]={0};
				char nmCanshu[32]={0};
                     //nmpFunName=strtok(mpFunName,"-");  //字符串分割   函数
                     //nmCanshu=strtok(NULL,"-");        //参数
				     g_ExtractChar(mpFunName,'(',')',nmpFunName,nmCanshu); //提取括号内的内容
//					 //玩家携带的 函数名称 和 参数
					 g_StrCpyLen(Player[nPlayerIndex].m_szTaskAnswerFun[i],nmpFunName+1, sizeof(Player[nPlayerIndex].m_szTaskAnswerFun[0]));
					 g_StrCpyLen(Player[nPlayerIndex].m_szTaskAnswerCanshu[i],nmCanshu, sizeof(Player[nPlayerIndex].m_szTaskAnswerCanshu[0]));
                     //nmpFunName=strtok(pAnswer, "/"); //去掉 斜杠  //取前面的部分
					 g_ExtractChar(pAnswer,'/',')',nmpFunName,nmCanshu,1);
			         sprintf(pContent, "%s|%s", pContent,nmpFunName);
					 ZeroMemory(nmpFunName,sizeof(nmpFunName));
					 ZeroMemory(nmCanshu,sizeof(nmCanshu));
			}
			else
			{
		    	g_StrCpyLen(Player[nPlayerIndex].m_szTaskAnswerCanshu[i],"", sizeof(Player[nPlayerIndex].m_szTaskAnswerCanshu[0]));
			    g_StrCpyLen(Player[nPlayerIndex].m_szTaskAnswerFun[i], pFunName + 1, sizeof(Player[nPlayerIndex].m_szTaskAnswerFun[0]));
			    *pFunName = 0;   //用完清空
			    sprintf(pContent, "%s|%s", pContent, pAnswer);
				//printf("--%s--\n",pContent);
			}
		}
		else 
		{
			strcpy(Player[nPlayerIndex].m_szTaskAnswerFun[i], "main");  //执行Main函数
			sprintf(pContent, "%s|%s", pContent, pAnswer);
		}*/
	}

	   if (nDataType == 0)//主信息的类型，字符串(0)或数字(1)
		UiInfo.m_nBufferLen  = strlen(pContent);
	   else 
		UiInfo.m_nBufferLen = strlen(pContent) + sizeof(int);
		
#ifndef _SERVER
	UiInfo.m_bParam2 = 0;  //客户端
#else
	if (nIsTimeClose>=1)
	   UiInfo.m_bParam2 = nIsTimeClose;  //服务器端	  秒
	else
	   UiInfo.m_bParam2=1;

	//printf("==%s==\n",pContent);
#endif
	
/*	if (nOptionNum == 0)
	{
		Player[nPlayerIndex].m_bWaitingPlayerFeedBack = false;
	}
	else
	{
		Player[nPlayerIndex].m_bWaitingPlayerFeedBack = true;
	}*/
	
	Player[nPlayerIndex].DoScriptAction(&UiInfo);//要求显示某个UI界面

	return 0;
}


int LuaGetLastDiagNpc(Lua_State * L)
{
	int nPlayerIndex ;
#ifdef _SERVER
	nPlayerIndex   = GetPlayerIndex(L);
#else
	nPlayerIndex   = CLIENT_PLAYER_INDEX;
#endif

	if (nPlayerIndex < 0)
	{
		Lua_PushNumber(L,0);
		return 1;
	}

	Lua_PushNumber(L,Player[nPlayerIndex].getLastDialogNpc());
	return 1;
}

int LuaSendMessageInfo(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 1) 
		return 0;
	
	int nPlayerIndex ;
#ifdef _SERVER
	   nPlayerIndex   = GetPlayerIndex(L);
#else
       nPlayerIndex   = CLIENT_PLAYER_INDEX;
#endif

	if (nPlayerIndex < 0) return 0;
	
	PLAYER_SCRIPTACTION_SYNC UiInfo;
	ZeroStruct(UiInfo);
	UiInfo.m_bUIId = UI_MSGINFO;
	UiInfo.m_bOptionNum = 1;
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
//	Player[nPlayerIndex].m_nCheckWaiGua =0;
	
	int nMsgId = 0;
	
	if (Lua_IsNumber(L,1))
	{
		nMsgId = (int)Lua_ValueToNumber(L,1);
		*((int *)(UiInfo.m_pContent)) = nMsgId;
		UiInfo.m_bParam1 = 1;
		UiInfo.m_nBufferLen = sizeof(int);
	}
	else 
	{
		
		g_StrCpyLen(UiInfo.m_pContent, Lua_ValueToString(L,1), 256);
		UiInfo.m_nBufferLen = strlen(((char *)UiInfo.m_pContent));
		UiInfo.m_bParam1 = 0;
	}
	
#ifndef _SERVER
	UiInfo.m_bParam2 = 0;
#else
	UiInfo.m_bParam2 = 1;
#endif
	Player[nPlayerIndex].DoScriptAction(&UiInfo);
	return 0;
}


int LuaAddGlobalNews_All(Lua_State * L)
{
	if (Lua_GetTopIndex(L) <= 0 ) return 0;

	tagMsgInGame	sLeaveGame;    
	sLeaveGame.cProtocol = c2s_msginfo_all;
	ZeroMemory(sLeaveGame.strMsg,sizeof(sLeaveGame.strMsg));

	if (Lua_IsNumber(L,1))//数字
		sLeaveGame.nMgsIdx   = (int)Lua_ValueToNumber(L,1);
	else 
	{
		sLeaveGame.nMgsIdx   = -1;
		g_StrCpyLen(sLeaveGame.strMsg,  Lua_ValueToString(L,1), sizeof(sLeaveGame.strMsg));
	}
	sLeaveGame.cCmdType  = AR_ADDMSG_GLOBALNEWS_ALL;
	sLeaveGame.nSerIdx   = nCurSerIdx; //哪个区的
	
	sLeaveGame.nValIdx   = 0;

	if (g_pGateWayClient)
		g_pGateWayClient->SendPackToServer(&sLeaveGame, sizeof(tagMsgInGame));

	return 0;
}

//AddGlobalNews(Newsstr)
int LuaAddGlobalNews(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 1) 
		return 0;
	
	int nPlayerIndex;
#ifdef _SERVER
	   nPlayerIndex   = GetPlayerIndex(L);
#else
       nPlayerIndex   = CLIENT_PLAYER_INDEX;
#endif

	PLAYER_SCRIPTACTION_SYNC UiInfo;
	ZeroStruct(UiInfo);
	UiInfo.m_bUIId = UI_NEWSINFO;
	UiInfo.m_bOptionNum = NEWSMESSAGE_NORMAL;
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
//	Player[nPlayerIndex].m_nCheckWaiGua =0;
	
	int nMsgId = 0;
	
	if (Lua_IsNumber(L,1))
	{
		nMsgId = (int)Lua_ValueToNumber(L,1);
		*((int *)(UiInfo.m_pContent)) = nMsgId;
		UiInfo.m_bParam1 = 1;
		UiInfo.m_nBufferLen = sizeof(int);
	}
	else 
	{
		g_StrCpyLen(UiInfo.m_pContent, Lua_ValueToString(L,1), 256);
		UiInfo.m_nBufferLen = strlen(((char *)UiInfo.m_pContent));
		UiInfo.m_bParam1 = 0;
	}
	
#ifndef _SERVER
	//int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0) return 0;

	UiInfo.m_bParam2 = 0;
	Player[nPlayerIndex].DoScriptAction(&UiInfo);
#else
	UiInfo.m_bParam2 = 1;
	UiInfo.ProtocolType = (BYTE)s2c_scriptaction;
	UiInfo.m_wProtocolLong = sizeof(PLAYER_SCRIPTACTION_SYNC) - MAX_SCIRPTACTION_BUFFERNUM + UiInfo.m_nBufferLen - 1;
	g_NewProtocolProcess.BroadcastGlobal(&UiInfo, UiInfo.m_wProtocolLong + 1);
	/*
	pUIInfo->ProtocolType = (BYTE)s2c_scriptaction;
	pUIInfo->m_wProtocolLong = sizeof(PLAYER_SCRIPTACTION_SYNC) - MAX_SCIRPTACTION_BUFFERNUM + pUIInfo->m_nBufferLen - 1;
	g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)pUIInfo, pUIInfo->m_wProtocolLong + 1 );
	
	*/
#endif
	return 0;
}

int LuaAddLocalNews_All(Lua_State * L)
{
	if (Lua_GetTopIndex(L) <1 ) return 0;

	tagMsgInGame	sLeaveGame;    
	sLeaveGame.cProtocol = c2s_msginfo_all;
	ZeroMemory(sLeaveGame.strMsg,sizeof(sLeaveGame.strMsg));

	if (Lua_IsNumber(L,1))//数字
		sLeaveGame.nMgsIdx   = (int)Lua_ValueToNumber(L,1);
	else 
	{
		sLeaveGame.nMgsIdx   = -1;
		g_StrCpyLen(sLeaveGame.strMsg,  Lua_ValueToString(L,1), sizeof(sLeaveGame.strMsg));
	}

	sLeaveGame.cCmdType  = AR_ADDMSG_LOCALNEWS_ALL;
	sLeaveGame.nSerIdx   = nCurSerIdx; //哪个区的
	sLeaveGame.nValIdx   = 0;

	if (g_pGateWayClient)
		g_pGateWayClient->SendPackToServer(&sLeaveGame, sizeof(tagMsgInGame));

	return 0;
}

int LuaAddLocalNews(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 1) 
		return 0;	
	int nPlayerIndex=0;

#ifdef _SERVER
		 nPlayerIndex = GetPlayerIndex(L);
#else	
		nPlayerIndex = CLIENT_PLAYER_INDEX;
#endif

	PLAYER_SCRIPTACTION_SYNC UiInfo;
	ZeroStruct(UiInfo);
	UiInfo.m_bUIId = UI_NEWSINFO;
	UiInfo.m_bOptionNum = NEWSMESSAGE_NORMAL;  //普通消息 显示就消亡
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;  //显示UI界面
//	Player[nPlayerIndex].m_nCheckWaiGua =0;
	int nMsgId = 0;
	
	if (Lua_IsNumber(L,1))
	{
		nMsgId = (int)Lua_ValueToNumber(L,1);
		*((int *)(UiInfo.m_pContent)) = nMsgId;
		UiInfo.m_bParam1 = 1;
		UiInfo.m_nBufferLen = sizeof(int);
	}
	else 
	{
		g_StrCpyLen(UiInfo.m_pContent,Lua_ValueToString(L,1), 128);
		UiInfo.m_nBufferLen = strlen(((char *)UiInfo.m_pContent));
		UiInfo.m_bParam1 = 0;
	}
	
#ifndef _SERVER
	//int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0) return 0;

	UiInfo.m_bParam2 = 0;
	Player[nPlayerIndex].DoScriptAction(&UiInfo);
#else
	UiInfo.m_bParam2 = 1;
	UiInfo.ProtocolType = (BYTE)s2c_scriptaction;
	UiInfo.m_wProtocolLong = sizeof(PLAYER_SCRIPTACTION_SYNC) - MAX_SCIRPTACTION_BUFFERNUM + UiInfo.m_nBufferLen - 1;
	g_NewProtocolProcess.BroadcastLocalServer(&UiInfo, UiInfo.m_wProtocolLong + 1);
#endif
	return 0;
}
//AddLocalNews(Newsstr)  ,显示一次就消亡
int LuaAddSysNews(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 1) 
		return 0;
	int nPlayerIndex=0;
	
#ifdef _SERVER
	nPlayerIndex = GetPlayerIndex(L);
#else	
	nPlayerIndex = CLIENT_PLAYER_INDEX;
#endif

	PLAYER_SCRIPTACTION_SYNC UiInfo;
	ZeroStruct(UiInfo);
	UiInfo.m_bUIId = UI_NEWSINFO;
	UiInfo.m_bOptionNum = NEWSMESSAGE_SHUIJI;  //普通消息 显示就消亡
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;  //显示UI界面
//	Player[nPlayerIndex].m_nCheckWaiGua =0;
	
	int nMsgId = 0;
	
	if (Lua_IsNumber(L,1))
	{
		nMsgId = (int)Lua_ValueToNumber(L,1);
		*((int *)(UiInfo.m_pContent)) = nMsgId;
		UiInfo.m_bParam1 = 1;
		UiInfo.m_nBufferLen = sizeof(int);
	}
	else 
	{
		g_StrCpyLen(UiInfo.m_pContent, Lua_ValueToString(L,1), MAX_SCIRPTACTION_BUFFERNUM);
		UiInfo.m_nBufferLen = strlen(((char *)UiInfo.m_pContent));
		UiInfo.m_bParam1 = 0;
	}
	
#ifndef _SERVER
	//int nPlayerIndex = CLIENT_PLAYER_INDEX;//GetPlayerIndex(L);
	if (nPlayerIndex < 0) 
		return 0;
	UiInfo.m_bParam2 = 0;
	Player[nPlayerIndex].DoScriptAction(&UiInfo); //客户端直接显示
#else
	UiInfo.m_bParam2 = 1;  //服务端 发送协议
	UiInfo.ProtocolType = (BYTE)s2c_scriptaction;
	UiInfo.m_wProtocolLong = sizeof(PLAYER_SCRIPTACTION_SYNC) - MAX_SCIRPTACTION_BUFFERNUM + UiInfo.m_nBufferLen - 1;
	g_NewProtocolProcess.BroadcastLocalServer(&UiInfo, UiInfo.m_wProtocolLong + 1);
#endif
	return 0;
}

int LuaAddGlobalCountNews_All(Lua_State * L)
{
	if (Lua_GetTopIndex(L) <2 ) return 0;

	tagMsgInGame	sLeaveGame;    
	sLeaveGame.cProtocol = c2s_msginfo_all;
	ZeroMemory(sLeaveGame.strMsg,sizeof(sLeaveGame.strMsg));

	if (Lua_IsNumber(L,1))//数字
		sLeaveGame.nMgsIdx   = (int)Lua_ValueToNumber(L,1);
	else 
	{
		sLeaveGame.nMgsIdx   = -1;
		g_StrCpyLen(sLeaveGame.strMsg,  Lua_ValueToString(L,1), sizeof(sLeaveGame.strMsg));
	}
	sLeaveGame.cCmdType  = AR_ADDMSG_GLOBALCOUNTNEWS_ALL;
	sLeaveGame.nSerIdx       = nCurSerIdx; //哪个区的
	sLeaveGame.nValIdx        =  (int)Lua_ValueToNumber(L,2); //TIme

	if (g_pGateWayClient)
		g_pGateWayClient->SendPackToServer(&sLeaveGame, sizeof(tagMsgInGame));

	return 0;
}

//AddGlobalCountNews(strNew/newid, time)
int LuaAddGlobalCountNews(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 2) 
		return 0;
	

	int nPlayerIndex=0;
	
#ifdef _SERVER
	nPlayerIndex = GetPlayerIndex(L);
#else	
	nPlayerIndex = CLIENT_PLAYER_INDEX;
#endif



	PLAYER_SCRIPTACTION_SYNC UiInfo;
	ZeroStruct(UiInfo);
	UiInfo.m_bUIId = UI_NEWSINFO;
	UiInfo.m_bOptionNum = NEWSMESSAGE_COUNTING;  //倒计时消息
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
//	Player[nPlayerIndex].m_nCheckWaiGua =0;

	int nMsgId = 0;
	
	int nTime = (int)Lua_ValueToNumber(L,2);
	
	if (nTime <= 0)
		nTime = 1;
	
	if (Lua_IsNumber(L,1))
	{
		nMsgId = (int)Lua_ValueToNumber(L,1);
		*((int *)(UiInfo.m_pContent)) = nMsgId;
		UiInfo.m_bParam1 = 1;
		*(int *)((char *)UiInfo.m_pContent + sizeof(int)) = nTime;
		UiInfo.m_nBufferLen = sizeof(int) * 2;
	}
	else 
	{
		g_StrCpyLen(UiInfo.m_pContent, Lua_ValueToString(L,1), 256);
		UiInfo.m_nBufferLen = strlen(((char *)UiInfo.m_pContent));
		*(int *)((char *)UiInfo.m_pContent + UiInfo.m_nBufferLen) = nTime;
		UiInfo.m_nBufferLen += sizeof(int);
		UiInfo.m_bParam1 = 0;
	}
//客户端	
#ifndef _SERVER
	//int nPlayerIndex = CLIENT_PLAYER_INDEX;//GetPlayerIndex(L);
	if (nPlayerIndex < 0) return 0;

	UiInfo.m_bParam2 = 0;
	Player[nPlayerIndex].DoScriptAction(&UiInfo);
#else
	UiInfo.m_bParam2 = 1;
	UiInfo.ProtocolType = (BYTE)s2c_scriptaction;   //显示UI的协议
	UiInfo.m_wProtocolLong = sizeof(PLAYER_SCRIPTACTION_SYNC) - MAX_SCIRPTACTION_BUFFERNUM + UiInfo.m_nBufferLen - 1;
	g_NewProtocolProcess.BroadcastGlobal(&UiInfo, UiInfo.m_wProtocolLong + 1);
#endif
	return 0;
}

//AddLocalCountNews(strNew/newid, time)
int LuaAddLocalCountNews(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 2) 
		return 0;
	

	int nPlayerIndex=0;
	
#ifdef _SERVER
	nPlayerIndex = GetPlayerIndex(L);
#else	
	nPlayerIndex = CLIENT_PLAYER_INDEX;
#endif


	PLAYER_SCRIPTACTION_SYNC UiInfo;
	ZeroStruct(UiInfo);
	UiInfo.m_bUIId = UI_NEWSINFO;
	UiInfo.m_bOptionNum = NEWSMESSAGE_COUNTING;  //倒计时消息
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
//	Player[nPlayerIndex].m_nCheckWaiGua =0;
	int nMsgId = 0;
	
	int nTime = (int)Lua_ValueToNumber(L,2);
	
	if (nTime <= 0)
		nTime = 1;
	
	if (Lua_IsNumber(L,1))
	{
		nMsgId = (int)Lua_ValueToNumber(L,1);
		*((int *)(UiInfo.m_pContent)) = nMsgId;
		UiInfo.m_bParam1 = 1;
		*(int *)((char *)UiInfo.m_pContent + sizeof(int)) = nTime;
		UiInfo.m_nBufferLen = sizeof(int) * 2;
	}
	else 
	{
		g_StrCpyLen(UiInfo.m_pContent, Lua_ValueToString(L,1), 256);
		UiInfo.m_nBufferLen = strlen(((char *)UiInfo.m_pContent));
		*(int *)((char *)UiInfo.m_pContent + UiInfo.m_nBufferLen) = nTime;
		UiInfo.m_nBufferLen += sizeof(int);
		UiInfo.m_bParam1 = 0;
	}
//客户端	
#ifndef _SERVER
	//int nPlayerIndex = CLIENT_PLAYER_INDEX;//GetPlayerIndex(L);
	if (nPlayerIndex < 0) return 0;

	UiInfo.m_bParam2 = 0;
	Player[nPlayerIndex].DoScriptAction(&UiInfo);
#else
	UiInfo.m_bParam2 = 1;
	UiInfo.ProtocolType = (BYTE)s2c_scriptaction;
	UiInfo.m_wProtocolLong = sizeof(PLAYER_SCRIPTACTION_SYNC) - MAX_SCIRPTACTION_BUFFERNUM + UiInfo.m_nBufferLen - 1;
	g_NewProtocolProcess.BroadcastLocalServer(&UiInfo, UiInfo.m_wProtocolLong + 1);
#endif
	return 0;
}

//AddGlobalTimeNews(strNew/newid, year,month,day,hour,mins)
int LuaAddGlobalTimeNews(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 6) 
		return 0;
	
	int nPlayerIndex=0;
	
#ifdef _SERVER
	nPlayerIndex = GetPlayerIndex(L);
#else	
	nPlayerIndex = CLIENT_PLAYER_INDEX;
#endif


	PLAYER_SCRIPTACTION_SYNC UiInfo;
	ZeroStruct(UiInfo);
	UiInfo.m_bUIId = UI_NEWSINFO;
	UiInfo.m_bOptionNum = NEWSMESSAGE_TIMEEND;  //定时消息
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
//	Player[nPlayerIndex].m_nCheckWaiGua =0;
	int nMsgId = 0;
	
	if (Lua_IsNumber(L,1))
	{
		nMsgId = (int)Lua_ValueToNumber(L,1);
		*((int *)(UiInfo.m_pContent)) = nMsgId;
		UiInfo.m_bParam1 = 1;
		UiInfo.m_nBufferLen = sizeof(int) + sizeof(SYSTEMTIME);
	}
	else 
	{
		g_StrCpyLen(UiInfo.m_pContent, Lua_ValueToString(L,1), 64);
		UiInfo.m_nBufferLen = strlen(((char *)UiInfo.m_pContent)) + sizeof(SYSTEMTIME);
		UiInfo.m_bParam1 = 0;
	}
	
	SYSTEMTIME *pSystemTime = (SYSTEMTIME *)((char *)UiInfo.m_pContent + UiInfo.m_nBufferLen - sizeof(SYSTEMTIME));
	memset(pSystemTime, 0, sizeof(SYSTEMTIME));
	
	SYSTEMTIME LocalTime ;
	memset(&LocalTime, 0, sizeof(SYSTEMTIME));
	
	LocalTime.wYear = (WORD)Lua_ValueToNumber(L,2);
	LocalTime.wMonth =(WORD)Lua_ValueToNumber(L,3);
	LocalTime.wDay = (WORD)Lua_ValueToNumber(L, 4);
	LocalTime.wHour = (WORD)Lua_ValueToNumber(L,5);
	LocalTime.wMinute = (WORD)Lua_ValueToNumber(L,6);
	FILETIME ft;
	FILETIME sysft;
#ifdef WIN32
	SystemTimeToFileTime(&LocalTime, &ft);
	LocalFileTimeToFileTime(&ft, &sysft);
	FileTimeToSystemTime(&sysft, pSystemTime);
#else
	memcpy(pSystemTime, &LocalTime, sizeof(LocalTime));
#endif
	
#ifndef _SERVER
	//int nPlayerIndex = CLIENT_PLAYER_INDEX;//GetPlayerIndex(L);
	if (nPlayerIndex < 0) return 0;

	UiInfo.m_bParam2 = 0;
	Player[nPlayerIndex].DoScriptAction(&UiInfo);
#else
	UiInfo.m_bParam2 = 1;
	UiInfo.ProtocolType = (BYTE)s2c_scriptaction;
	UiInfo.m_wProtocolLong = sizeof(PLAYER_SCRIPTACTION_SYNC) - MAX_SCIRPTACTION_BUFFERNUM + UiInfo.m_nBufferLen - 1;
	g_NewProtocolProcess.BroadcastGlobal(&UiInfo, UiInfo.m_wProtocolLong + 1);
#endif
	return 0;
}

//AddLocalTimeNews(strNew/newid, year,month,day,hour,mins)
int LuaAddLocalTimeNews(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 6) 
		return 0;
	
	int nPlayerIndex=0;
	
#ifdef _SERVER
	nPlayerIndex = GetPlayerIndex(L);
#else	
	nPlayerIndex = CLIENT_PLAYER_INDEX;
#endif


	PLAYER_SCRIPTACTION_SYNC UiInfo;
	UiInfo.m_bUIId = UI_NEWSINFO;
	UiInfo.m_bOptionNum = NEWSMESSAGE_TIMEEND;
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
//	Player[nPlayerIndex].m_nCheckWaiGua =0;
	int nMsgId = 0;
	
	if (Lua_IsNumber(L,1))
	{
		nMsgId = (int)Lua_ValueToNumber(L,1);
		*((int *)(UiInfo.m_pContent)) = nMsgId;
		UiInfo.m_bParam1 = 1;
		UiInfo.m_nBufferLen = sizeof(int) + sizeof(SYSTEMTIME);
	}
	else 
	{
		g_StrCpyLen(UiInfo.m_pContent, Lua_ValueToString(L,1), 64);
		UiInfo.m_nBufferLen = strlen(((char *)UiInfo.m_pContent)) + sizeof(SYSTEMTIME);
		UiInfo.m_bParam1 = 0;
	}
	
	SYSTEMTIME *pSystemTime = 	(SYSTEMTIME *)((char *)UiInfo.m_pContent + UiInfo.m_nBufferLen - sizeof(SYSTEMTIME));
	memset(pSystemTime, 0, sizeof(SYSTEMTIME));
	
	SYSTEMTIME LocalTime ;
	memset(&LocalTime, 0, sizeof(SYSTEMTIME));
	
	LocalTime.wYear = (WORD)Lua_ValueToNumber(L,2);
	LocalTime.wMonth =(WORD)Lua_ValueToNumber(L,3);
	LocalTime.wDay = (WORD)Lua_ValueToNumber(L, 4);
	LocalTime.wHour = (WORD)Lua_ValueToNumber(L,5);
	LocalTime.wMinute = (WORD)Lua_ValueToNumber(L,6);
	FILETIME ft;
	FILETIME sysft;
#ifdef WIN32
	SystemTimeToFileTime(&LocalTime, &ft);
	LocalFileTimeToFileTime(&ft, &sysft);
	FileTimeToSystemTime(&sysft, pSystemTime);
#else
	memcpy(pSystemTime, &LocalTime, sizeof(LocalTime));
#endif
	
#ifndef _SERVER
	//int nPlayerIndex = CLIENT_PLAYER_INDEX;
	if (nPlayerIndex < 0) return 0;

	UiInfo.m_bParam2 = 0;
	Player[nPlayerIndex].DoScriptAction(&UiInfo);
#else
	UiInfo.m_bParam2 = 1;
	UiInfo.ProtocolType = (BYTE)s2c_scriptaction;
	UiInfo.m_wProtocolLong = sizeof(PLAYER_SCRIPTACTION_SYNC) - MAX_SCIRPTACTION_BUFFERNUM + UiInfo.m_nBufferLen - 1;
	g_NewProtocolProcess.BroadcastLocalServer(&UiInfo, UiInfo.m_wProtocolLong + 1);
#endif
	return 0;
}

//AddNote(str/strid)
int LuaAddNote(Lua_State * L)
{
	char * strMain  = NULL;
	int nMainInfo = 0;
	int nDataType = 0;
	
	int nPlayerIndex = GetPlayerIndex(L);
#ifndef _SERVER
	nPlayerIndex   = CLIENT_PLAYER_INDEX;
#endif
	if (nPlayerIndex < 0)
		return 0;
	
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 1) 
		return 0;
	
	int nParam2 = 0;
	
	
	if  (Lua_IsNumber(L,1))
	{
		nMainInfo = (int)Lua_ValueToNumber(L,1);
		nDataType = 1 ;
	}
	else if (Lua_IsString(L, 1)) 	//检查主信息是字符串还是字符串标识号
	{
		strMain = (char *)Lua_ValueToString(L, 1);
		nDataType = 0 ;
	}
	else
		return 0;
	
	if (nParamNum > 1)
	{
		nParam2 = (int)Lua_ValueToNumber(L, 2);
	}
	

	PLAYER_SCRIPTACTION_SYNC UiInfo;
	ZeroStruct(UiInfo);
	UiInfo.m_bUIId = UI_NOTEINFO;
	UiInfo.m_bParam1 = nDataType;//主信息的类型，字符串(0)或数字(1)
//	Player[nPlayerIndex].m_nCheckWaiGua =0;
#ifndef _SERVER
	UiInfo.m_bParam2 = 0;
#else
	UiInfo.m_bParam2 = 1;
#endif
	
	UiInfo.m_bOptionNum = 0;
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
	
	//主信息为字符串
	if (nDataType == 0)
	{
		if (NULL == strMain) return 0;
			sprintf(UiInfo.m_pContent, "%s", strMain);
		int nLen = strlen(strMain);
		*(int*)(UiInfo.m_pContent + nLen) = nParam2;
		UiInfo.m_nBufferLen = nLen + sizeof(int);
	}
	else if (nDataType == 1) //主信息为数字标识
	{
		*(int *)UiInfo.m_pContent = nMainInfo;
		*(int *)(UiInfo.m_pContent + sizeof(int)) = nParam2;
		UiInfo.m_nBufferLen = sizeof(int) + sizeof(int);
	}
	
	Player[nPlayerIndex].DoScriptAction(&UiInfo);
	return 0;
}

/*
**
**格式1:Talk(SentenceNum, CallBack-Fun(结束后的回调函数), sTalk1, sTalk2, sTalk3, sTalk4,...sTalkN);  
Talk(SentenceNum, CallBack-Fun(结束后的回调函数), nTalk1, nTalk2,nTalk3,nTalk4,...nTalkN);  
**格式2:Talk(SentenceNum, CallBack-Fun, SentenceTab);
**例子:Talk(3,"EndTalk", "玩家：请问现在几点钟了？", "雇员：现在5点钟了","太谢谢你了！");
**
*/

int LuaNewTalkUI(Lua_State * L)
{

	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 4) 
		return 0;

	int nPlayerIndex=0 ;
	int nMainInfo = 0;
	int nDataType = 0;
	int nOptionNum = 0;
	char * pContent = NULL;

	nPlayerIndex = (int)Lua_ValueToNumber(L,1);

	if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER) 
		return 0;

	if (Lua_IsNumber(L,2))
		nOptionNum = (int)Lua_ValueToNumber(L,2);
	else 
		return 0;

	const char * pCallBackFun = Lua_ValueToString(L,3);

	//检查主信息是字符串还是字符串标识号

	if  (Lua_IsNumber(L,4))
		nDataType = 1 ;
	else if (Lua_IsString(L, 4)) 
		nDataType = 0 ;
	else
		return 0;


	//获得实际传入的选项的个数
	if (nOptionNum > nParamNum - 3) 
		nOptionNum = nParamNum -3;

	PLAYER_SCRIPTACTION_SYNC UiInfo;
	ZeroStruct(UiInfo);
	UiInfo.m_bUIId = UI_TALKDIALOG;
	UiInfo.m_bParam1 = nDataType;//主信息的类型，字符串(0)或数字(1)
	UiInfo.m_bOptionNum = nOptionNum;
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
	pContent = UiInfo.m_pContent;
	//	Player[nPlayerIndex].m_nCheckWaiGua =0;
	pContent[0] = 0;
	size_t nContentLen = 0;
	for (int i  = 0; i < nOptionNum; i ++)
	{	
		const char * pString = NULL;
		if (!nDataType)//StringInfo
		{
			pString = Lua_ValueToString(L, i + 4); //第三个起为字符窜类
			if (NULL == pString || nContentLen + strlen(pString) >= MAX_SCIRPTACTION_BUFFERNUM)
			{//长度太大了 截断
				nOptionNum = i;
				UiInfo.m_bOptionNum = nOptionNum;
				break;
			}
			nContentLen += strlen(pString);
			sprintf(pContent, "%s%s|", pContent, pString);
		}
		else
		{
			int j = (int)Lua_ValueToNumber(L, i + 4);
			sprintf(pContent, "%s%d|", pContent, j);
		}
	}
	UiInfo.m_nBufferLen  = strlen(pContent);

	if (!pCallBackFun || strlen(pCallBackFun) <= 0)
	{
		UiInfo.m_nParam = 0;
		Player[nPlayerIndex].m_nAvailableAnswerNum = 0;
		//		Player[nPlayerIndex].m_bWaitingPlayerFeedBack = false;
	}
	else   //有回调函数
	{
		UiInfo.m_nParam = 1;
		Player[nPlayerIndex].m_nAvailableAnswerNum = 1;
		g_StrCpyLen(Player[nPlayerIndex].m_szTaskAnswerFun[0], pCallBackFun, sizeof(Player[nPlayerIndex].m_szTaskAnswerFun[0]));
	}

#ifndef _SERVER
	UiInfo.m_bParam2 = 0;
#else
	UiInfo.m_bParam2 = 1;
#endif

	Player[nPlayerIndex].DoScriptAction(&UiInfo);

	return 0;

}


int LuaTalkUI(Lua_State * L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 3) 
		return 0;

	int nPlayerIndex ;
	#ifdef _SERVER
	   nPlayerIndex   = GetPlayerIndex(L);
    #else
       nPlayerIndex   = CLIENT_PLAYER_INDEX;
    #endif

	if (nPlayerIndex <= 0) 
		return 0;
//	Player[nPlayerIndex].m_bWaitingPlayerFeedBack = false;
	int nMainInfo = 0;
	int nDataType = 0;
	int nOptionNum = 0;
	char * pContent = NULL;
	
	if (Lua_IsNumber(L,1))
		nOptionNum = (int)Lua_ValueToNumber(L,1);
	else 
		return 0;
	
	const char * pCallBackFun = Lua_ValueToString(L,2);
	
	//检查主信息是字符串还是字符串标识号
	
	if  (Lua_IsNumber(L,3))
		nDataType = 1 ;
	else if (Lua_IsString(L, 3)) 
		nDataType = 0 ;
	else
		return 0;
	
	
	//获得实际传入的选项的个数
	if (nOptionNum > nParamNum - 2) 
		nOptionNum = nParamNum - 2;
	
	PLAYER_SCRIPTACTION_SYNC UiInfo;
	ZeroStruct(UiInfo);
	UiInfo.m_bUIId = UI_TALKDIALOG;
	UiInfo.m_bParam1 = nDataType;//主信息的类型，字符串(0)或数字(1)
	UiInfo.m_bOptionNum = nOptionNum;
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
	pContent = UiInfo.m_pContent;
//	Player[nPlayerIndex].m_nCheckWaiGua =0;
	pContent[0] = 0;
	size_t nContentLen = 0;
	for (int i  = 0; i < nOptionNum; i ++)
	{	
			const char * pString = NULL;
			if (!nDataType)//StringInfo
			{
				pString = Lua_ValueToString(L, i + 3); //第三个起为字符窜类
				if (NULL == pString || nContentLen + strlen(pString) >= MAX_SCIRPTACTION_BUFFERNUM)
				{//长度太大了 截断
					nOptionNum = i;
					UiInfo.m_bOptionNum = nOptionNum;
					break;
				}
				nContentLen += strlen(pString);
				sprintf(pContent, "%s%s|", pContent, pString);
			}
			else
			{
				int j = (int)Lua_ValueToNumber(L, i + 3);
				sprintf(pContent, "%s%d|", pContent, j);
			}
	}
	UiInfo.m_nBufferLen  = strlen(pContent);
	
	if (!pCallBackFun || strlen(pCallBackFun) <= 0)
	{
		UiInfo.m_nParam = 0;
		Player[nPlayerIndex].m_nAvailableAnswerNum = 0;
//		Player[nPlayerIndex].m_bWaitingPlayerFeedBack = false;
	}
	else   //有回调函数
	{
		UiInfo.m_nParam = 1;
		Player[nPlayerIndex].m_nAvailableAnswerNum = 1;
		g_StrCpyLen(Player[nPlayerIndex].m_szTaskAnswerFun[0], pCallBackFun, sizeof(Player[nPlayerIndex].m_szTaskAnswerFun[0]));
//		Player[nPlayerIndex].m_bWaitingPlayerFeedBack = true;
	}
	
#ifndef _SERVER
	UiInfo.m_bParam2 = 0;
#else
	UiInfo.m_bParam2 = 1;
#endif
	
	Player[nPlayerIndex].DoScriptAction(&UiInfo);

	return 0;
	
}

int LuaIncludeLib(Lua_State * L)
{

	if (Lua_GetTopIndex(L) <= 0) 
		return 0;
	
	if (Lua_IsString(L,1))
	{
       	const char * pFileDir = lua_tostring(L,1);	
		char lszCurrentDirectory[MAX_PATH];
	}
	return 0;
}


int LuaIncludeFile(Lua_State * L)
{
	int npCount=Lua_GetTopIndex(L);
	if (npCount <= 0) 
		return 0;
	
	if (Lua_IsString(L,1))
	{//如果第一个参数为字符串
		char pFileName[256],nTempFileName[256];
		ZeroMemory(pFileName,sizeof(pFileName));
		ZeroMemory(nTempFileName,sizeof(nTempFileName));
		//const char * nTempFileName = lua_tostring(L,1);	
		snprintf(nTempFileName, sizeof(nTempFileName), "%s", (char *)lua_tostring(L,1));
		int nIncModel=0;
		if (npCount>=2)
			nIncModel=(int)lua_tonumber(L,2);

		switch(nIncModel)
		{
		case 1:
			//sprintf(nTempStr,(char *)Lua_ValueToString(L,2));
			if (NULL == Utf8ToGbk(nTempFileName, pFileName, sizeof(pFileName))) pFileName[0] = '\0';
			//printf("--Utf8ToGbk:%s \n",pFileName);
			break;
		case 2:
			snprintf(pFileName, sizeof(pFileName), "%s", GbkToUtf8(nTempFileName).c_str());
			//printf("--GbkToUtf8:%s \n",pFileName);
			break;
		default:
			snprintf(pFileName, sizeof(pFileName), "%s", nTempFileName);
			//printf("--NoChange:%s \n",pFileName);
			break;
		}

		char lszCurrentDirectory[MAX_PATH];
		int nLen = 0;
		if (pFileName[0] != '\\' && pFileName[0] != '/')
		{
			if (!getcwd(lszCurrentDirectory, MAX_PATH))
			{
				lszCurrentDirectory[0] = '.';
				lszCurrentDirectory[1] = 0;
			}
			nLen = strlen(lszCurrentDirectory);
			if (nLen > 0 && (lszCurrentDirectory[nLen - 1] == '\\' || lszCurrentDirectory[nLen - 1] == '/'))
				lszCurrentDirectory[nLen - 1] = 0;
#ifdef WIN32  //32位用  "\\"
			g_StrCat(lszCurrentDirectory, "\\");
			g_StrCat(lszCurrentDirectory, (char*)pFileName);
#else          
			g_StrCat(lszCurrentDirectory, "/");
			g_StrCat(lszCurrentDirectory, (char*)pFileName);
			for (int i = 0; lszCurrentDirectory[i]; ++i)
			{
				if (lszCurrentDirectory[i] == '\\')
					lszCurrentDirectory[i] = '/';
			}
#endif
		}
		else
		{
			g_GetRootPath(lszCurrentDirectory); //获取软件运行的 绝对路径
			
			nLen = strlen(lszCurrentDirectory);
			if (nLen > 0 && (lszCurrentDirectory[nLen - 1] == '\\' || lszCurrentDirectory[nLen - 1] == '/'))
				lszCurrentDirectory[nLen - 1] = 0;
#ifdef WIN32
			g_StrCat(lszCurrentDirectory, "\\");
			g_StrCat(lszCurrentDirectory, (char*)pFileName + 1);
#else
			g_StrCat(lszCurrentDirectory, "/");
			g_StrCat(lszCurrentDirectory, (char*)pFileName + 1);
			for (int i = 0; lszCurrentDirectory[i]; ++i)
			{
				if (lszCurrentDirectory[i] == '\\')
					lszCurrentDirectory[i] = '/';
			}
#endif
		}

	    //g_StrLower(lszCurrentDirectory + nLen);  //大写转小写	 strlwr
				// Safe ASCII-only lowercasing without corrupting multibyte Chinese sequences (GBK)
		for (int nPos = nLen; nPos < (int)sizeof(lszCurrentDirectory) - 1 && lszCurrentDirectory[nPos]; ++nPos)
		{
			unsigned char c = (unsigned char)lszCurrentDirectory[nPos];
			if (c >= 0x81)
			{
				if (lszCurrentDirectory[nPos + 1]) ++nPos;
			}
			else if (c >= 'A' && c <= 'Z')
			{
				lszCurrentDirectory[nPos] = (char)(c + ('a' - 'A'));
			}
		}

#ifdef _SERVER
		KPakFile incFile;
		if (incFile.Open(lszCurrentDirectory) || incFile.Open(pFileName))
		{
			DWORD incSize = incFile.Size();
			if (incSize > 0)
			{
				KMemClass incMem;
				if (incMem.Alloc(incSize + 4))
				{
					if (incFile.Read(incMem.GetMemPtr(), incSize) == incSize)
					{
						((char*)incMem.GetMemPtr())[incSize] = 0;
						lua_dobuffer(L, (const char*)incMem.GetMemPtr(), incSize, pFileName);
					}
				}
			}
			incFile.Close();
		}
		else
		{
			lua_dofile(L, lszCurrentDirectory);
		}
#else		   //客户端只能包含 包文件
		if (!pFileName)
			return 0;

		  char nMsg[128]={0};
          KPakFile	File;
          DWORD		Size;
	      if (!File.Open(pFileName)) 
		     return 0;
	  
		  Size = File.Size();
		  KMemClass Memory;
		  if (!Memory.Alloc(Size + 4))
		  {
		     File.Close();
		     return 0;
		  }	
		  if (File.Read(Memory.GetMemPtr(),Size) != Size)
		  {
		     File.Close();
			 Memory.Free();
		     return 0;
		  }

		  char * pszMem = (char *)Memory.GetMemPtr();
		  pszMem[Size + 1] = 0;
		  
		  File.Close();
			
		  try
		  {
			  if (pszMem)
			  {
				  int state;
				  // `!=' binds tighter than `=' - grouped, so `state' holds the code and the guard tests it
				  if ((state = Lua_ExecuteBuffer(L,pszMem,Size,NULL)) != 0)
				  {
					  Memory.Free();
					  return 0;
				  }
			  }  
		  }
		  catch(...)
		  {
		  }
		  Memory.Free();
#endif
		return 0;
	}
	else
		return 0;
}

//**************************************************************************************************************************************************************
//												任务脚本
//**************************************************************************************************************************************************************


//**************************************************************************************************************************************************************
#ifndef _SERVER
#define MAX_TEMPVALUENUM_INCLIENT 4000
int g_TempValue[MAX_TEMPVALUENUM_INCLIENT];
#endif

int LuaGetTaskValue(Lua_State * L)
{
	int nPlayerIndex=0;
	int nParamNum =Lua_GetTopIndex(L);
	int nTaskId= (int)Lua_ValueToNumber(L,1);

#ifdef _SERVER
	   nPlayerIndex   = GetPlayerIndex(L);
	   if (nParamNum>1)
		  nPlayerIndex=(int)Lua_ValueToNumber(L,2);

#else
       nPlayerIndex   = CLIENT_PLAYER_INDEX;

	   if (nPlayerIndex <= 0) return 0;

       PLAYER_GET_TASKVAL_COMMAND GetTaskval;
	       GetTaskval.ProtocolType = c2s_gettaskval;
           GetTaskval.m_TaskId=nTaskId;
           GetTaskval.m_pDwid=Npc[Player[nPlayerIndex].m_nIndex].m_dwID;	
	   if (g_pClient)
		   g_pClient->SendPackToServer((BYTE*)&GetTaskval, sizeof(PLAYER_GET_TASKVAL_COMMAND));
	      //Player[nPlayerIndex].m_ItemList.LockOperation();
#endif

    int nValue=0;

	if (nPlayerIndex > 0) 
	{
		nValue = Player[nPlayerIndex].m_cTask.GetSaveVal(nTaskId);		
		//Lua_PushNumber(L, nValue);
	}
	//else		
	Lua_PushNumber(L, nValue);	
	return 1;
}

int LuaSetTaskValue(Lua_State * L)
{
	int nParamNum = Lua_GetTopIndex(L);

	int nPlayerIndex;
#ifdef _SERVER
	   nPlayerIndex   = GetPlayerIndex(L);
#else
       nPlayerIndex   = CLIENT_PLAYER_INDEX;
#endif
	int nValueIndex = (int)Lua_ValueToNumber(L, 1);
	int nValue      = (int)Lua_ValueToNumber(L, 2);	

	if  (nParamNum>2)
		nPlayerIndex = (int)Lua_ValueToNumber(L, 3);

	if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER) return 0;

	Player[nPlayerIndex].m_cTask.SetSaveVal(nValueIndex, nValue,true);  //编号 和 值

//#ifndef _SERVER
//	 PLAYER_SET_TASKVAL_COMMAND SetTaskval;
//	 SetTaskval.ProtocolType = c2s_settaskval;
//	 SetTaskval.m_TaskId  = nValueIndex;
//	 SetTaskval.m_TaskVal = nValue;
//	 SetTaskval.m_pDwid=Npc[Player[nPlayerIndex].m_nIndex].m_dwID;	
//	 if (g_pClient)
//		   g_pClient->SendPackToServer((BYTE*)&SetTaskval, sizeof(PLAYER_SET_TASKVAL_COMMAND));
//#endif 
	return 0;
}

int LuaSetDeathSkill(Lua_State * L)
{
	int nPlayerIndex;
#ifdef _SERVER
	   nPlayerIndex   = GetPlayerIndex(L);
#else
       nPlayerIndex   = CLIENT_PLAYER_INDEX;
#endif
	   int nValueIndex = (int)Lua_ValueToNumber(L, 1);
	   int nValue = (int)Lua_ValueToNumber(L, 2);	
	   if (nPlayerIndex <= 0) return 0;
	   
	   Player[nPlayerIndex].m_cTask.SetSaveVal(nValueIndex, nValue);  //编号 和 值
	   //	int nReg=Player[nPlayerIndex].Save();  //
	   return 0;
}

int LuaGetDeathSkill(Lua_State * L)
{
	int nPlayerIndex=0;
	
	int nTaskId= (int)Lua_ValueToNumber(L,1);
	
#ifdef _SERVER
	   nPlayerIndex   = GetPlayerIndex(L);
	   
	   if  (nPlayerIndex<=0)
	   {
		   nPlayerIndex=(int)Lua_ValueToNumber(L,2);
	   }
	   
#else
       nPlayerIndex   = CLIENT_PLAYER_INDEX;
	   
       PLAYER_GET_TASKVAL_COMMAND GetTaskval;
	   GetTaskval.ProtocolType = c2s_gettaskval;
	   GetTaskval.m_TaskId=nTaskId;
	   GetTaskval.m_pDwid=Npc[Player[nPlayerIndex].m_nIndex].m_dwID;	
	   if (g_pClient)
		   g_pClient->SendPackToServer((BYTE*)&GetTaskval, sizeof(PLAYER_GET_TASKVAL_COMMAND));
	      //Player[nPlayerIndex].m_ItemList.LockOperation();
#endif
	   
	   int nValue=0;
	   
	   if (nPlayerIndex > 0) 
	   {
		   nValue = Player[nPlayerIndex].m_cTask.GetSaveVal(nTaskId);		
		   //Lua_PushNumber(L, nValue);
	   }
	   //else		
	   Lua_PushNumber(L, nValue);	
	   return 1;
}

int LuaAddTaskValue(Lua_State * L)
{
	int nParamNum = Lua_GetTopIndex(L);
	int nPlayerIndex;
#ifdef _SERVER
	   nPlayerIndex   = GetPlayerIndex(L);
#else
       nPlayerIndex   = CLIENT_PLAYER_INDEX;
#endif

	int nValueIndex = (int)Lua_ValueToNumber(L,1);
	int nValue = (int)Lua_ValueToNumber(L, 2);

	if  (nParamNum>2)
		nPlayerIndex = (int)Lua_ValueToNumber(L, 3);

	if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
		return 0;

	int nValueOld = (int)Player[nPlayerIndex].m_cTask.GetSaveVal(nValueIndex);
	Player[nPlayerIndex].m_cTask.SetSaveVal(nValueIndex,nValueOld + nValue,true);
	return 0;
}

//***************************************************************************8***
int LuaGetTempTaskValue(Lua_State * L)
{
	int  nParamNum = Lua_GetTopIndex(L);
	int nTempIndex = (int)Lua_ValueToNumber(L, 1);
	int nPlayerIndex;
#ifdef _SERVER
	if(nTempIndex >= MAX_TEMP_TASK)
	{
		Lua_PushNil(L) ;
		return 1;
	}
	nPlayerIndex = GetPlayerIndex(L);
	if  (nParamNum>1)
		nPlayerIndex = (int)Lua_ValueToNumber(L, 2);

	if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER) 
	{
		Lua_PushNil(L);
		return 1;
	}
	
	int nValue = Player[nPlayerIndex].m_cTask.GetClearVal(nTempIndex);
	Lua_PushNumber(L, nValue);
#else
	nPlayerIndex=CLIENT_PLAYER_INDEX;

	/*if  (nParamNum>1)
		nPlayerIndex = (int)Lua_ValueToNumber(L, 2);

	if (nPlayerIndex <= 0) 
	{
		Lua_PushNil(L);
		return 1;
	}*/

	if (nTempIndex >= 0 && nTempIndex < MAX_TEMPVALUENUM_INCLIENT)
		Lua_PushNumber(L, g_TempValue[nTempIndex]);
	else 
		Lua_PushNil(L);
#endif

	return 1;
}

int LuaSetTempTaskValue(Lua_State * L)
{
	int nParamNum = Lua_GetTopIndex(L);
	int nTempIndex = (int)Lua_ValueToNumber(L, 1);
	int nValue = (int)Lua_ValueToNumber(L,2);
	int nPlayerIndex;
#ifdef _SERVER	
	//Lua_GetGlobal(L, SCRIPT_PLAYERINDEX);
	nPlayerIndex = GetPlayerIndex(L);
	if  (nParamNum>2)
		nPlayerIndex = (int)Lua_ValueToNumber(L, 3);
	//nPlayerIndex = (int)Lua_ValueToNumber(L, Lua_GetTopIndex(L));
	if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER) return 0;
	Player[nPlayerIndex].m_cTask.SetClearVal(nTempIndex, nValue);
#else
	nPlayerIndex=CLIENT_PLAYER_INDEX;

	if  (nParamNum>2)
		nPlayerIndex = (int)Lua_ValueToNumber(L, 3);

	if (nTempIndex < 0 || nTempIndex >= MAX_TEMPVALUENUM_INCLIENT) return 0;
	g_TempValue[nTempIndex] = nValue;
#endif
	return 0;
}

//获取列 数和行数
int LuaGetTabRowAndCol(Lua_State * L) 
{
	const char* FiledName = Lua_ValueToString(L, 1);
	int mLie = 0, mRow = 0;
	if (FiledName && FiledName[0])
	{
		KTabFile* pTab = GetScriptTabFile(FiledName);
		if (pTab)
		{
			mLie = pTab->GetWidth();
			mRow = pTab->GetHeight() - 1;
		}
	}
	Lua_PushNumber(L, mRow);
	Lua_PushNumber(L, mLie);
	return 2;
}

int LuaCheckIniFieSec(Lua_State * L)
{
	KIniFile  IniFile;
	char *FiledName=NULL,*JieName=NULL;
	FiledName =(char *)Lua_ValueToString(L,1);
	JieName   =(char *)Lua_ValueToString(L,2);
	if (!IniFile.Load(FiledName))
	{
	    Lua_PushNumber(L,0); 
		return 1;
	}
	int nVer = IniFile.IsSectionExist(JieName);
	IniFile.Clear();
	Lua_PushNumber(L,nVer); 
	return 1;
}
//获取INI某节 某键的值
int LuaGetIniFileInfo(Lua_State * L)
{  
	KIniFile  IniFile;
	char *FiledName=NULL,*JieName=NULL,*KeyName=NULL;
	    FiledName =(char *)Lua_ValueToString(L,1);
		JieName =(char *)Lua_ValueToString(L, 2);
		KeyName =(char *)Lua_ValueToString(L, 3);
     int nIsType=0;
		 nIsType=(int)Lua_ValueToNumber(L, 4);
	 int nInVal=0;
	 char nstrVal[256]={0};
	 if (!IniFile.Load(FiledName))
		 return 0;

	 if  (nIsType==0)
	 {
	      IniFile.GetInteger(JieName,KeyName,0,&nInVal);
		  IniFile.Clear();
		  Lua_PushNumber(L,nInVal); 
          return 1;
	 }
	 else
	 {
		 IniFile.GetString(JieName,KeyName,"",nstrVal,sizeof(nstrVal));
		 IniFile.Clear();
		 Lua_PushString(L,nstrVal); 
         return 1;
	 }

}

//获取某行 某列的值
int LuaGetTabFileInfo(Lua_State * L)
{
	return LuaTabFile_GetCell(L);
}

int LuaGetTaskAwardMatrix(Lua_State * L)
{
	KTabFile TableFile;
	//  KTabFile    TableFile;     // 读取TXT文件 返回表格
	char *FiledName=NULL,*nKeyName=NULL,*nTxtName=NULL;
	int nRows=0,nReg=0,mLie=0,mRow=0;
	
	FiledName = (char *)Lua_ValueToString(L, 1);
    nKeyName  = (char *)Lua_ValueToString(L, 2);  //列的字段
	nTxtName  = (char *)Lua_ValueToString(L, 3);  //需要寻找的名称
	
    if (!TableFile.Load(FiledName))
		return 0;
    int i ,nStartPos=0,nEndPos=0;
    nRows = TableFile.GetHeight();
    mLie  = TableFile.GetWidth();
	char nTitle[64]={0};

	for (i=2;i<nRows+1;++i)
	{
	
		TableFile.GetString(i,nKeyName,"",nTitle,sizeof(nTitle));
		if (nTxtName && strstr(nTitle,nTxtName))
		{
			nStartPos = i;
			break;
		}
	}
	
	for (i=nStartPos;i<nRows+1;++i)
	{
		TableFile.GetString(i,nKeyName,"",nTitle,sizeof(nTitle));
		if (nTxtName && strstr(nTitle,nTxtName))
		{
			nEndPos = i;
			nReg++;	   //有多少行
		}
		else
			break;     //否则跳出  只能是 连续的行号
	}
	
	TableFile.Clear();
	Lua_PushNumber(L,nReg); 
	Lua_PushNumber(L,mLie); 
	Lua_PushNumber(L,nStartPos); 
	Lua_PushNumber(L,nEndPos);  
	return 4;
}


int LuaCalcFreeItemCellCount(Lua_State * L)
{

int nPlayerIndex;

#ifdef _SERVER
	   nPlayerIndex   = GetPlayerIndex(L);
#else
       nPlayerIndex   = CLIENT_PLAYER_INDEX;
#endif
	   int nCount = 0;
	   if (nPlayerIndex > 0)
	   {
		   int x,y;
		   for (x=0;x< EQUIPMENT_ROOM_WIDTH;x++)
			   for (y=0;y< EQUIPMENT_ROOM_HEIGHT;y++)
			   {
		           if (!Player[nPlayerIndex].m_ItemList.m_Room[room_equipment].FindItem(x, y))
				   {//没有东西 就增加 不连续的空间
					   nCount ++;
				   }
			   }
	   }
	   //printf("DEBUG, Count: %d\n", nCount);
	   Lua_PushNumber(L, nCount);
 return 1;
}

int LuaGetPositionCount(Lua_State * L)
{
	int nPlayerIndex;

#ifdef _SERVER
	   nPlayerIndex   = GetPlayerIndex(L);
#else
       nPlayerIndex   = CLIENT_PLAYER_INDEX;
#endif

	int nWpos  = (int)Lua_ValueToNumber(L,1);
	int nHpos  = (int)Lua_ValueToNumber(L,2);
    int nCount = 0;
	int nRoomKind = 0;
	    nRoomKind  = (int)Lua_ValueToNumber(L,3);

	if  (nRoomKind<room_equipment || nRoomKind>=room_num)
		 nRoomKind = room_equipment;

    if (nPlayerIndex <= 0)
    {
		//if (Lua_GetTopIndex(L) < 2 ) return 0;
		nPlayerIndex = (int)Lua_ValueToNumber(L,4);
		/*ItemPos Pos;
		ZeroStruct(Pos);
		if (Player[nPlayerIndex].m_ItemList.SearchPosition(nWpos,nHpos,&Pos))
		{//查找位置	连续空间
			nCount=nWpos*nHpos;
		}
		ZeroStruct(Pos);*/
		if (nPlayerIndex <= 0)
		{
			Lua_PushNumber(L, nCount);
			return 1;
		}
	}

	if (nPlayerIndex < 0 || nPlayerIndex >= MAX_PLAYER) { Lua_PushNumber(L,0); return 1; }
	nCount = Player[nPlayerIndex].m_ItemList.GetFreePositionCount(nWpos,nHpos,nRoomKind);

	Lua_PushNumber(L, nCount);
    return 1;
}


int LuaGetTimeByMiao(Lua_State * L)
{
	time_t rawtime;                  //定义一个long 型存放秒数
	time (&rawtime);                 //距离现在的时间（秒）
	char nTimeInfo[32]={0};
	sprintf(nTimeInfo,"%llu",(unsigned long long)rawtime);
	//printf("---测试时间:%s ----\n",nTimeInfo);
	Lua_PushString(L,nTimeInfo);
	return 1;
}

//将时间字符串（按照模式 格式化）返回字符串
int LuaGetLocalDateBys2s(Lua_State * L)
{
	char * _nTime_  = (char *)Lua_ValueToString(L,1);
	char * nTimeStr = (char *)Lua_ValueToString(L,2);

	time_t rawtime;                  //定义一个long 型存放秒数
	struct tm * timeinfo;	
	//time (&rawtime);   //距离现在的时间（秒）
	rawtime = g_Atoui(_nTime_);
	timeinfo = localtime(&rawtime);  //把从1970-1-1零点零分到当前时间系统所偏移的秒数时间转换为本地时间

	char strTime[80];
	strftime(strTime,sizeof(strTime),nTimeStr,timeinfo);

	Lua_PushString(L,strTime);
	return 1;
}
//获取当前时间（按照模式 格式化）
int LuaGetLocalDate(Lua_State * L)
{
    char * nTimeStr = (char *)Lua_ValueToString(L,1);

	time_t rawtime;                  //定义一个long 型存放秒数
	struct tm * timeinfo;	
	time (&rawtime);                 //距离现在的时间（秒）
	timeinfo = localtime(&rawtime);  //把从1970-1-1零点零分到当前时间系统所偏移的秒数时间转换为本地时间

	char strTime[80];
    strftime(strTime,sizeof(strTime),nTimeStr,timeinfo);

    Lua_PushString(L,strTime);
    return 1;
}

int LuaGetTimeNow(Lua_State * L)
{
	time_t rawtime;
	struct tm * timeinfo;
	
	time ( &rawtime );
	timeinfo = localtime ( &rawtime );

	Lua_PushNumber(L, timeinfo->tm_year + 1900);
	Lua_PushNumber(L, timeinfo->tm_mon + 1);	
	Lua_PushNumber(L, timeinfo->tm_mday);
	Lua_PushNumber(L, timeinfo->tm_hour);
	Lua_PushNumber(L, timeinfo->tm_min);
	Lua_PushNumber(L, timeinfo->tm_sec);
	Lua_PushNumber(L, timeinfo->tm_wday);
	Lua_PushNumber(L, timeinfo->tm_yday);
	return 8;
}


int LuaGetDataInt(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 1) return 0;
	int nId = (int)Lua_ValueToNumber(L,1);
	Lua_PushNumber(L, GameData.GetDataInt(nId));
	return 1;
}

int LuaGetDataStr(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 1) return 0;
	int nId = (int)Lua_ValueToNumber(L,1);
	Lua_PushString(L, GameData.GetDataStr(nId));
	return 1;
}

int LuaSetData(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 2) return 0;
	int nNo = (int)Lua_ValueToNumber(L,1);
	char* szValue = (char*)Lua_ValueToString(L,2);
	GameData.SetData(nNo, szValue);
	return 0;
}

int LuaAddDataGr(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 1) return 0;

	char* szName1 = (char*)Lua_ValueToString(L, 1);
	int nNameId = g_FileName2Id(szName1);

	KDataGroup Info;
	Info.nNameId = nNameId;
	memset(Info.nValue, 0, sizeof(Info.nValue));
	g_StrCpyLen(Info.szName1, szName1, sizeof(Info.szName1));
	memset(Info.szName2, 0, sizeof(Info.szName2));

	Lua_PushNumber(L, GameData.AddDataGr(&Info));
	return 1;
}

int LuaSetDataGr(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 8) return 0;
	int i=0,j=0;
	KDataGroup Info;
	j++;
	int nGroup = (int)Lua_ValueToNumber(L,j);
	j++;
	Info.nNameId = (int)Lua_ValueToNumber(L,j);
	j++;
	for (i = 0; i < MAX_DATAGROUP_VALUE; i++)
		Info.nValue[i] = (int)Lua_ValueToNumber(L,j+i);
	g_StrCpyLen(Info.szName1, (char*)Lua_ValueToString(L, MAX_DATAGROUP_VALUE+j), sizeof(Info.szName1));
	g_StrCpyLen(Info.szName2, (char*)Lua_ValueToString(L, MAX_DATAGROUP_VALUE+j+1), sizeof(Info.szName2));
	GameData.SetDataGr(nGroup, &Info);
	return 0;
}

int LuaGetDataGr(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 1) return 0;

	int nNo = (int)Lua_ValueToNumber(L, 1);
	
	Lua_PushNumber(L, GameData.GetDataGr_dwName(nNo)); 
	for (int j = 0; j < MAX_DATAGROUP_VALUE; j++)
		Lua_PushNumber(L, GameData.GetDataGr_nValue(nNo, j)); 
	Lua_PushString(L, GameData.GetStrDataGr_Name1(nNo));
	Lua_PushString(L, GameData.GetStrDataGr_Name2(nNo));
	return 7;
}

int LuaFindDataId(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 1) return 0;
	
	int nNo = (int)Lua_ValueToNumber(L, 1);

	Lua_PushNumber(L, GameData.FindDataId((DWORD)Lua_ValueToNumber(L, 1)));
	return 1;
}

int LuaSaveDataFile(Lua_State * L)
{
	GameData.Save();
	return 0;
}
 
int LuaGetNumGroup(Lua_State * L)
{
	Lua_PushNumber(L, GameData.GetNumGroup());
	return 1;
}

int LuaGetLeagueObj(Lua_State * L)
{
    if (Lua_GetTopIndex(L) < 1) return 0;

    int nNo = (int)Lua_ValueToNumber(L, 1);

    Lua_PushString(L, GameData.GetStrDataGr_Name1(nNo));    
    for (int j = 0; j < MAX_DATAGROUP_VALUE; j++)
        Lua_PushNumber(L, GameData.GetDataGr_nValue(nNo, j)); 
    Lua_PushString(L, GameData.GetStrDataGr_Name2(nNo));   
    return MAX_DATAGROUP_VALUE + 2;
}

int LuaFindLeagueObj(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 1) return 0;
    char* szName = (char*)Lua_ValueToString(L, 1);
	int nNameId = g_FileName2Id(szName);
	Lua_PushNumber(L, GameData.FindDataId(nNameId));
	return 1;
}

int LuaGetLeagueMemObj(Lua_State * L)
{
    if (Lua_GetTopIndex(L) < 1) return 0;

    const char* szName = Lua_ValueToString(L, 1);
    const char* szMembers = GameData.FindDataName1(szName);

    if (!szMembers || szMembers[0] == '\0') 
	{
        Lua_PushNumber(L, 0); 
        return 1;
    }

    if (Lua_GetTopIndex(L) == 1) 
	{
        int count = 1;

        for (const char* p = szMembers; *p; ++p) {
            if (*p == ',') count++;
        }

        Lua_PushNumber(L, count);
        return 1;
    }

    Lua_PushString(L, szMembers);
    return 1;
}

int Lua_SetLeagueObj(Lua_State* L)
{
    if (Lua_GetTopIndex(L) < 6) return 0;

    const char* szName = Lua_ValueToString(L, 1);
    char szTemp[64];
    g_StrCpyLen(szTemp, szName, sizeof(szTemp));
    szTemp[sizeof(szTemp) - 1] = 0;
    int nNameId = g_FileName2Id(szTemp);

    KDataGroup Info;
    Info.nNameId = nNameId;
    memset(Info.nValue, 0, sizeof(Info.nValue));
    for (int i = 0; i < MAX_DATAGROUP_VALUE; i++)
        Info.nValue[i] = (int)Lua_ValueToNumber(L, i + 2);

    g_StrCpyLen(Info.szName1, szName, sizeof(Info.szName1));
    memset(Info.szName2, 0, sizeof(Info.szName2));

    int nIndex = GameData.AddDataGr(&Info);

    Lua_PushNumber(L, nIndex);
    return 1;
}

int Lua_AddMemberToGroupByName(Lua_State* L)
{
    if (Lua_GetTopIndex(L) < 2) return 0;

    char* groupLeaderName = (char*)Lua_ValueToString(L, 1);   // "ConLonVup"
    char* newMemberName = (char*)Lua_ValueToString(L, 2);     // "tenthanhvien1"
    if (NULL == groupLeaderName || NULL == newMemberName) return 0;
    int nNameId = g_FileName2Id(groupLeaderName);
    int nGroup = GameData.FindDataId(nNameId);

    if (nGroup < 0)
    {
        KDataGroup newGroup;
        newGroup.nNameId = nNameId;

        memset(newGroup.nValue, 0, sizeof(newGroup.nValue));

        g_StrCpyLen(newGroup.szName1, groupLeaderName, sizeof(newGroup.szName1));
        newGroup.szName1[sizeof(newGroup.szName1) - 1] = 0;

        _snprintf(newGroup.szName2, sizeof(newGroup.szName2), "%s,%s", groupLeaderName, newMemberName);

        GameData.AddDataGr(&newGroup);

        Lua_PushNumber(L, 1);
        return 1;
    }

    KDataGroup& group = GameData.m_sDataGroup[nGroup];

    if (!strstr(group.szName2, newMemberName))
    {
        char temp[sizeof(group.szName2)] = {0};
        strncpy(temp, group.szName2, sizeof(temp) - 1);
        temp[(sizeof(temp) - 1) - 1] = '\0';

        if (strlen(temp) > 0)
            strncat(temp, ",", sizeof(temp) - strlen(temp) - 1);

        strncat(temp, newMemberName, sizeof(temp) - strlen(temp) - 1);

        strncpy(group.szName2, temp, sizeof(group.szName2));
        group.szName2[sizeof(group.szName2) - 1] = 0;

        GameData.SetDataGr(nGroup, &group);
    }
	GameData.Save();
    Lua_PushNumber(L, 1);
    return 1;
}

int LuaAddLeagueObj(Lua_State * L)
{
    if (Lua_GetTopIndex(L) < 1) return 0;

	char* szName1 = (char*)Lua_ValueToString(L, 1);
	int nNameId = g_FileName2Id(szName1);
	 
	KDataGroup newGroup;
	newGroup.nNameId = nNameId;
	memset(newGroup.nValue, 0, sizeof(newGroup.nValue));
	g_StrCpyLen(newGroup.szName1, szName1, sizeof(newGroup.szName1));
	memset(newGroup.szName2, 0, sizeof(newGroup.szName2));
	  
    GameData.AddDataGr(&newGroup);
    Lua_PushNumber(L, 1);
    return 1;
}

int LuaIsTextPass(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 1)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	const char* pszText = Lua_ValueToString(L, 1);
	if (!pszText || !pszText[0])
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	Lua_PushNumber(L, 1);
	return 1;
}

int LuaIniFile_Load(Lua_State * L)
{
	KIniFile IniFile;
	char * szFileName = (char *)Lua_ValueToString(L,1);
	if (IniFile.Load(szFileName))
		Lua_PushNumber(L, 1);
	else
		Lua_PushNumber(L, 0);
	return 1;
}

int LuaIniFile_SetData(lua_State* L)
{
    KIniFile IniFile;
    char* szFileName = (char *)Lua_ValueToString(L, 1);
    char* szSectName = (char *)Lua_ValueToString(L, 2);
    char* szKeyName  = (char *)Lua_ValueToString(L, 3);

    // Load INI
    IniFile.Load(szFileName, TRUE);

    int argType = lua_type(L, 4);

    if (argType == LUA_TNUMBER)
    {
        int nValue = (int)Lua_ValueToNumber(L, 4);
        IniFile.WriteInteger(szSectName, szKeyName, nValue);
    }
    else if (argType == LUA_TSTRING)
    {
        char* szValue = (char *)Lua_ValueToString(L, 4);
        IniFile.WriteString(szSectName, szKeyName, szValue);
    }
    else
    {
        lua_pushstring(L, "Invalid value type, must be number or string");
        //lua_error(L);
        return 0;
    }

    // Save file
    IniFile.Save(szFileName);
    return 0;
}

int LuaIniFile_GetData(lua_State * L)
{
    KIniFile IniFile;
    const char* szFileName = Lua_ValueToString(L, 1);
    const char* szSectName = Lua_ValueToString(L, 2);
    const char* szKeyName  = Lua_ValueToString(L, 3);

    char szValue[128] = {0};

    if (!IniFile.Load(szFileName))
    {
        lua_pushnil(L);
        return 1;
    }

    if (IniFile.GetString(szSectName, szKeyName, "", szValue, sizeof(szValue)))
    {
        if (szValue[0] != '\0')
        {
            Lua_PushString(L, szValue);
            return 1;
        }
    }

    int nValue = 0;
    if (IniFile.GetInteger(szSectName, szKeyName, 0, &nValue))
    {
        Lua_PushNumber(L, nValue);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

int LuaFile_Create(Lua_State * L)
{
	char * szFileName = (char *)Lua_ValueToString(L,1);
    if (szFileName && !PathFileExists(szFileName)) 
    {
        FILE* f = fopen(szFileName, "w");
        if (f) 
		fclose(f);
    }
    Lua_PushNumber(L, 1);
    return 1;
}

int LuaIniFile_ClearData(lua_State* L)
{
    char * szFileName = (char *)Lua_ValueToString(L, 1);
    KIniFile IniFile;
    if (!IniFile.Load(szFileName))
    {
       Lua_PushNumber(L, 0);
        return 1;
    } 
    IniFile.Clear();  
    IniFile.Save(szFileName);
    Lua_PushNumber(L, 1);
    return 1;
}

//打开进度条
int LuaOpenJinDuTiao(Lua_State * L)
{
	int nPlayerIndex;

    const	char *mTitle=(char *)Lua_ValueToString(L,1);   //标题
	int  nMsgIndex=(int )Lua_ValueToNumber(L,2);           //模板ID  是那个进度条
	int  nTimes=(int )Lua_ValueToNumber(L,3);              //持续多少后执行回调函数
    const	char *mFun=(char *)Lua_ValueToString(L,4);     //回调函数名称
    const   char *szParma=(char *)Lua_ValueToString(L,5);  //传递参数一
    const	char *szParmb=(char *)Lua_ValueToString(L,6);  //传递参数二
    const	char *szParmc=(char *)Lua_ValueToString(L,7);  //传递参数三

    #ifdef _SERVER
//-----------------------------------------------   
	nPlayerIndex=GetPlayerIndex(L);
    if (nPlayerIndex<=0) return 0;
	PLAYER_OPEN_JINDUTIAO pJinDu;
	pJinDu.ProtocolType = (BYTE)s2c_openjindutiao;

	if (sizeof(mTitle) > sizeof(pJinDu.m_szTitle))
		return 0;
	    g_StrCpyLen(pJinDu.m_szTitle,mTitle,sizeof(pJinDu.m_szTitle));     //标题
	
	pJinDu.m_MsgIndex = nMsgIndex;           //模板编号
	pJinDu.m_inTime   = nTimes;              //时间

	if (sizeof(mFun) > sizeof(pJinDu.m_Callback))
		return 0;
	    g_StrCpyLen(pJinDu.m_Callback,mFun,sizeof(pJinDu.m_Callback));      //回调的函数
	
	if (sizeof(szParma) > sizeof(pJinDu.m_szParma))
		return 0;
	   g_StrCpyLen(pJinDu.m_szParma,szParma,sizeof(pJinDu.m_szParma));     //参数一

	if (sizeof(szParmb) > sizeof(pJinDu.m_szParmb))
		   return 0;
	   g_StrCpyLen(pJinDu.m_szParmb,szParmb,sizeof(pJinDu.m_szParmb));     //参数二

	if (sizeof(szParmc) > sizeof(pJinDu.m_szParmc))
		   return 0;
	   g_StrCpyLen(pJinDu.m_szParmc,szParmc,sizeof(pJinDu.m_szParmc));     //参数三
	
	g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, &pJinDu, sizeof(PLAYER_OPEN_JINDUTIAO));  //通知打开UI
	Player[nPlayerIndex].m_nGiveCallBackScript = Npc[Player[nPlayerIndex].m_nIndex].m_ActionScriptID;  //回调函数的脚本id

//-----------------------------------------------
    #else
    #endif
    return 0;
}

int LuaPaceBar(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
	{
		Lua_PushNumber(L,0);
		return 1;
	}

	int nParamCount = Lua_GetTopIndex(L);
	if (nParamCount > 6)
		return 0;

	const char *mTitle=(char *)Lua_ValueToString(L,1);	//标题
	int  nMsgIndex	   = 1;									//模板ID  是那个进度条
	const char *mFun = (char *)Lua_ValueToString(L,2);	//回调函数名称
	int  nTimes		   = (int)Lua_ValueToNumber(L,3);		//持续多少后执行回调函数
	const char *szParma = (char *)Lua_ValueToString(L,4);	//传递参数一
	const char *szParmb = (nParamCount >= 5) ? (const char *)Lua_ValueToString(L, 5) : "";
	const char *szParmc = (nParamCount >= 6) ? (const char *)Lua_ValueToString(L, 6) : "";

	#ifdef _SERVER
	//-----------------------------------------------   

	PLAYER_OPEN_JINDUTIAO pJinDu;
	pJinDu.ProtocolType = (BYTE)s2c_openjindutiao;

	if (sizeof(mTitle) > sizeof(pJinDu.m_szTitle))
		return 0;
		g_StrCpyLen(pJinDu.m_szTitle,mTitle,sizeof(pJinDu.m_szTitle));     //标题
	
	pJinDu.m_MsgIndex = nMsgIndex;           //模板编号
	pJinDu.m_inTime   = nTimes;              //时间

	if (sizeof(mFun) > sizeof(pJinDu.m_Callback))
		return 0;
		g_StrCpyLen(pJinDu.m_Callback,mFun,sizeof(pJinDu.m_Callback));      //回调的函数
	
	if (sizeof(szParma) > sizeof(pJinDu.m_szParma))
		return 0;
	   g_StrCpyLen(pJinDu.m_szParma,szParma,sizeof(pJinDu.m_szParma));     //参数一

	if (sizeof(szParmb) > sizeof(pJinDu.m_szParmb))
		   return 0;
		strcpy(pJinDu.m_szParmb,szParmb);     //参数二

	if (sizeof(szParmc) > sizeof(pJinDu.m_szParmc))
			return 0;
		 strcpy(pJinDu.m_szParmc,szParmc);     //参数三
	
	g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, &pJinDu, sizeof(PLAYER_OPEN_JINDUTIAO));  //通知打开UI
	Player[nPlayerIndex].m_nGiveCallBackScript = Npc[Player[nPlayerIndex].m_nIndex].m_ActionScriptID;  //回调函数的脚本id

	//-----------------------------------------------
	#else
	#endif
	return 0;
}


int LuaGetRandomNum(Lua_State * L)
{

	int nMaxNum = (int)Lua_ValueToNumber(L, 1);
	if (nMaxNum <0)
		nMaxNum=0;

	char nsg[64]={0};
	sprintf(nsg,"%d",g_Random(nMaxNum));
	Lua_PushString(L,nsg); 
	return 1;
	
}




int LuaSetRandomSeed(Lua_State * L)
{
	int nSeed = (int)Lua_ValueToNumber(L, 1);

	if  (nSeed<=0)
		 nSeed=1;

	g_RandomSeed(nSeed);

	return 0;
	
}

int LuaGetRandomSeed(Lua_State * L)
{
	Lua_PushNumber(L,g_GetRandomSeed());
	return 1;

}

int LuaNewRandom(Lua_State * L)
{

	int nMinNum = (int)Lua_ValueToNumber(L, 1);
	int nMaxNum = (int)Lua_ValueToNumber(L, 2);

	if (nMinNum<0 || nMaxNum<0) return 0;

	if (nMinNum>nMaxNum)
	{//交换
	   int inTemp= nMinNum;
	   nMinNum=nMaxNum;
	   nMaxNum=inTemp;
	}

	//srand((unsigned)time(0));

    Lua_PushNumber(L,rand()%(nMaxNum-nMinNum)+nMinNum); 

	return 1;
	
}


int LuaFileName2Id(Lua_State * L)
{
	char *strScript = (char *)Lua_ValueToString(L ,1);

	if (strScript)
      Lua_PushNumber(L, g_FileName2Id(strScript));
	else
	  Lua_PushNumber(L, 0);

	return 1;
}



//设置包袱为空--修复函数
int LuaSetRoomNull(Lua_State * L)
{
   int nPlayerIndex,mreg=0;

#ifdef _SERVER
	   nPlayerIndex   = GetPlayerIndex(L);
#else
       nPlayerIndex   = CLIENT_PLAYER_INDEX;
#endif
/*
  	room_equipment = 0,	//0 装备栏-包袱
	room_repository,	//1 贮物箱
	room_exbox1,		//2 扩展箱1
	room_exbox2,		//3 扩展箱2
	room_exbox3,		//4 扩展箱3
	room_equipmentex,	//5 同伴包袱
	room_trade,			//6 交易栏
	room_tradeback,		//7 交易过程中装备栏的备份
	room_trade1,		//8 交易过程中对方的交易栏
	room_immediacy,		//9 快捷物品
	room_give,          //10 给予空间
	room_giveback,      //11 给予回调空间 
	room_dazao,         //12 打造框
	room_cailiao,       //13 材料框
	room_num,			// 空间数量

*/
	   int  nRoomidx=(int )Lua_ValueToNumber(L,1);

            if (nRoomidx < 0 || nRoomidx >= room_num) { Lua_PushNumber(L,0); return 1; }
            mreg=Player[nPlayerIndex].m_ItemList.m_Room[nRoomidx].SetRoomNull();
  //#include	"KItemList.h"
	Lua_PushNumber(L,mreg);
	return 1;
}

//获取某个动作下的NPC SPR路径
int LuaGetMaskActionSpr(Lua_State * L)
{

	 int  nNpcSetIdx=(int )Lua_ValueToNumber(L,1);
	 int  nAction   =(int )Lua_ValueToNumber(L,2);

	// nNpcRePath only gets its text when the npcres tab file loads, and the test at the end of
	// this function reads its first byte on every path: the three names beside it already
	// start empty, so this one has to as well.
	 char nNpcKind[32]={0},nNpcRePath[128]={0},nNpcActSpr[64]={0},nNpcSprFullPath[256]={0};
	 g_NpcSetting.GetString(nNpcSetIdx,"NpcResType","enemy003",nNpcKind,sizeof(nNpcKind));

	 KTabFile nNpcType;

	 if (nNpcType.Load("\\settings\\npcres\\人物类型.txt"))
	 {
		 nNpcType.GetString(nNpcKind,3,"",nNpcRePath,sizeof(nNpcRePath));
	 }
	 nNpcType.Clear();

	 if (nNpcType.Load("\\settings\\npcres\\普通npc资源.txt"))
	 {
		 nNpcType.GetString(nNpcKind,nAction+1,"",nNpcActSpr,sizeof(nNpcActSpr));
	 }
	 nNpcType.Clear();

	 if (nNpcRePath[0] && nNpcActSpr[0])
		sprintf(nNpcSprFullPath,"\\%s\\%s",nNpcRePath,nNpcActSpr);
	 else
		return 0;

	 Lua_PushString(L, nNpcSprFullPath);
	 return 1;
}


//SetPos(X,Y) //跑向
int LuaSetPos(Lua_State * L)
{
	int nParamCount = Lua_GetTopIndex(L);
	if (nParamCount != 2) return 0;
	int nPlayerIndex=0;
	
#ifdef _SERVER
	nPlayerIndex = GetPlayerIndex(L);
#else	
	nPlayerIndex = CLIENT_PLAYER_INDEX;
#endif
	if  (nPlayerIndex<=0) return 0;

	int nX = (int) Lua_ValueToNumber(L,1);
	int nY = (int) Lua_ValueToNumber(L,2);
	int nResult=0;
	
	if (nPlayerIndex > 0)
	{
	#ifdef _SERVER
		nResult=Npc[Player[nPlayerIndex].m_nIndex].SetPos(nX * 32, nY * 32);
    #else
		PLAYER_SYN_POS mPos;
		mPos.ProtocolType = c2s_playersetpos;
		mPos.nXpos = nX;
		mPos.nYpos = nY;
		if (g_pClient)
			g_pClient->SendPackToServer((BYTE*)&mPos, sizeof(PLAYER_SYN_POS));
		nResult = 1 ;
    #endif

	}
	
	Lua_PushNumber(L, nResult);
	return 1;
}


int LuaCalcEquiproomItemCount(Lua_State * L)
{
	
int nPlayerIndex=0;
	
#ifdef _SERVER
	nPlayerIndex = GetPlayerIndex(L);
#else	
	nPlayerIndex = CLIENT_PLAYER_INDEX;
#endif
	if  (nPlayerIndex<=0) return 0;
	int nCout=0;
	int nItemGen = (int)Lua_ValueToNumber(L, 1);
    int nDetail  = (int)Lua_ValueToNumber(L, 2);
	int nParticular    = (int)Lua_ValueToNumber(L, 3);
	int nLevel   = (int)Lua_ValueToNumber(L, 4);
	
	
	nCout = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetItemNum(nItemGen, nDetail, nParticular, nLevel);
	
	
	Lua_PushNumber(L,nCout);
    return 1;
}


//门派
int LuaGetPlayerFaction(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
#ifndef _SERVER
	nPlayerIndex = CLIENT_PLAYER_INDEX;
#endif
	char FactionName[64]={0};			
	if (nPlayerIndex > 0)								
	{	
		Player[nPlayerIndex].GetFactionName(FactionName, 64);
		Lua_PushString(L, FactionName);
	}
	else
	{
	
	    nPlayerIndex=(int)Lua_ValueToNumber(L,1);
		if  (nPlayerIndex<=0)
		{
			Lua_PushString(L,"");
		}
		else
		{
			Player[nPlayerIndex].GetFactionName(FactionName, 64);
			Lua_PushString(L, FactionName);
		}
	}
	return 1;
}

int LuaGetPlayerFactionCamp(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);				
	if (nPlayerIndex > 0)								
		Lua_PushNumber(L, Player[nPlayerIndex].GetFactionCamp());
	else
		Lua_PushNumber(L, -1);

	return 1;
}

int LuaGetPlayerFactionNo(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);				
	if (nPlayerIndex > 0)
	{
		int nResult = Player[nPlayerIndex].GetFactionNo();
		if (nResult < 0)
			nResult = Player[nPlayerIndex].m_cTask.GetSaveVal(130) - 1; //fix phai khi xuat su
		Lua_PushNumber(L, nResult);
	}
	else
	{
		Lua_PushNumber(L, -1);
	}
	return 1;
}

int LuaGetLastFactionNumber(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
#ifndef _SERVER
	nPlayerIndex = CLIENT_PLAYER_INDEX;
#endif 
		if (nPlayerIndex <= 0)
			Lua_PushNumber(L,-1);
		else
			Lua_PushNumber(L, Player[nPlayerIndex].m_cFaction.m_nFirstAddFaction);


	return 1;
}



int LuaSetNpcLevel(Lua_State * L)
{

	if (Lua_GetTopIndex(L) >0)
	{
		int nNpcIdx = (int)Lua_ValueToNumber(L, 1);
		int nLevel = (int)Lua_ValueToNumber(L, 2);

		if  (nNpcIdx>0 && nNpcIdx<MAX_NPC)
			 Npc[nNpcIdx].m_Level=nLevel;
	}
	return 0;
}

int LuaGetLevel(Lua_State * L)
{
	
	if (Lua_GetTopIndex(L) >0)
	{
		int nNpcIdx = (int)Lua_ValueToNumber(L, 1);
		if (nNpcIdx < 0 || nNpcIdx >= MAX_NPC) { Lua_PushNumber(L,0); return 1; }
		Lua_PushNumber(L, Npc[nNpcIdx].m_Level);
	}
	else
	{
		int nPlayerIndex = GetPlayerIndex(L);
#ifndef _SERVER
		nPlayerIndex = CLIENT_PLAYER_INDEX;
#endif 
		if (nPlayerIndex <= 0)		Lua_PushNumber(L,0);	
		if (Player[nPlayerIndex].m_nIndex <= 0) Lua_PushNumber(L,0);
		Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_Level);
	}
	return 1;
}


//W,X,Y = GetWorldPos()
int LuaGetNewWorldPos(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	
#ifndef _SERVER
	nPlayerIndex = CLIENT_PLAYER_INDEX;
#endif 
		
	if (nPlayerIndex > 0)
	{
		int nPosX = 0,nPosY = 0,nPmap=0;
		Npc[Player[nPlayerIndex].m_nIndex].GetMpsPos(&nPosX, &nPosY,&nPmap);
		
		int nSubWorldIndex = Npc[Player[nPlayerIndex].m_nIndex].m_SubWorldIndex;
		int nSubWorldID = 0;
		
		if (nSubWorldIndex >= 0 && nSubWorldIndex < MAX_SUBWORLD)
		{
			nSubWorldID = SubWorld[nSubWorldIndex].m_SubWorldID;
		}
		
		Lua_PushNumber(L, nSubWorldID); 
		Lua_PushNumber(L, ((int)(nPosX / 32)));
		Lua_PushNumber(L, ((int)(nPosY / 32)));
	}
	else
	{
		Lua_PushNumber(L, 0);
		Lua_PushNumber(L, 0);
		Lua_PushNumber(L, 0);
		return 3;
	}
	return 3;
}



//==============以上为通用指令===============服务器端脚本函数开始=======================
#ifdef _SERVER
//---------------------------------交易、买卖、打开储藏箱-----------------------
//Sale(id)
//------------------------------------------------------------------------------
//打开商城

int LuaOpenVipShop(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);

	if (nPlayerIndex > 0)
	{
       Player[nPlayerIndex].Market(nPlayerIndex,0);
	}

	return 0;
}


int LuaSale(Lua_State * L)
{
	if (Lua_GetTopIndex(L) <= 0) return 0;
	
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
		return 0;
	if (Player[nPlayerIndex].m_cTrade.m_nIsTrading != 0)
		return 0;

	int nShopId = (int)Lua_ValueToNumber(L, 1);
	BuySell.OpenSSale(nPlayerIndex, nShopId - 1);
	return 0;
}

int LuaTrade(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
		return 0;
	if (Player[nPlayerIndex].m_cTrade.m_nIsTrading != 0)
		return 0;
	if (Lua_GetTopIndex(L) >= 1)
	{
		int nNpcIdx = (int)Lua_ValueToNumber(L, 1);
		if (nNpcIdx > 0 && nNpcIdx < MAX_NPC)
		{
			Player[nPlayerIndex].m_cTrade.StartTrade(nNpcIdx);
		}
	}
	return 0;
}

int LuaOpenBox(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
		return 0;
	if (Player[nPlayerIndex].m_cTrade.m_nIsTrading != 0)
		return 0;
	
	BYTE NetCommand = (BYTE)s2c_openstorebox;
	g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, &NetCommand, sizeof(BYTE));
	return 0;
}

int LuaOpenResetPass(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	RESET_PASS nPASS;
	nPASS.ProtocolType = (BYTE)s2c_openresetpass;
	g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, &nPASS, sizeof(RESET_PASS));
	return 0;
}

//打开老头人物奖励UI
int LuaGiftDT(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	DATAU_BOX LAOTOU;
	memset(&LAOTOU, 0, sizeof(LAOTOU));
	LAOTOU.ProtocolType = (BYTE)s2c_opendataubox;

	g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, &LAOTOU, sizeof(DATAU_BOX));
	return 0;
}
/*stringbox */
//---------------------------------时间任务-------------------------------------
//SetTimer(Time, TimerTaskId)
int LuaSetTimer(Lua_State  * L)
{
	int nParamCount = Lua_GetTopIndex(L);
	if (nParamCount < 2 ) return 0;
	int nPlayerIndex  = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	Player[nPlayerIndex].SetTimer((DWORD) (int)Lua_ValueToNumber(L, 1), (int)Lua_ValueToNumber(L,2));
	return 0;
}
//全局时间计时器
int LuaSetSubTimer(Lua_State  * L)
{
    KTimerTaskFun nSubTimer;
	nSubTimer.SetTimer((DWORD) (int)Lua_ValueToNumber(L, 1), (int)Lua_ValueToNumber(L,2));
	return 0;
}

int LuaStopTimer(Lua_State * L)
{
	int nPlayerIndex  = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	Player[nPlayerIndex].CloseTimer();
	return 0;
}

int LuaGetCurTimerId(Lua_State * L)
{
	int nPlayerIndex  = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) 
	{
		Lua_PushNumber(L,0);
		return 1;
	}
	int nTimerId = Player[nPlayerIndex].m_TimerTask.GetTaskId();
	Lua_PushNumber(L, nTimerId);
	return 1;
}

int LuaGetRestTime(Lua_State * L)
{
	int nPlayerIndex  = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) 
	{
		Lua_PushNil(L);
		return 1;
	}
	int nRestTime = Player[nPlayerIndex].m_TimerTask.GetRestTime();//m_dwTimeTaskTime - g_SubWorldSet.GetGameTime();
	
	if (nRestTime > 0)
		Lua_PushNumber(L, nRestTime);
	else
		Lua_PushNumber(L, 0);
	
	return 1;
}

int LuaGetGameTime(Lua_State * L)
{ 
    int tick = g_SubWorldSet.GetGameTime();
    int sec = tick / 20;   // chia cho 20 de ra giay thuc
    Lua_PushNumber(L, sec);
    return 1;
}

int LuaGetMissionRestTime(Lua_State * L)
{
	int RestTime = 0;
	if (Lua_GetTopIndex(L) >= 2)
	{
		int nSubWorldIndex = GetSubWorldIndex(L);
		if (nSubWorldIndex >= 0) 
		{
			int nMissionId = (int)Lua_ValueToNumber(L, 1);
			int nTimerId = (int)Lua_ValueToNumber(L, 2);
			
			if (nMissionId < 0 || nTimerId < 0 )
				goto lab_getmissionresttime;
			
			KMission Mission;
			Mission.SetMissionId(nMissionId);
			KMission * pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
			if (pMission)
			{
				RestTime = (int)pMission->GetTimerRestTimer(nTimerId);
			}
		}
	}
	
lab_getmissionresttime:
	Lua_PushNumber(L, RestTime);
	return 1;
}

//**************************************************************************************************************************************************************
//												组队脚本
//**************************************************************************************************************************************************************
int LuaIsPlayer(Lua_State * L)
{
    int nidex = (int)Lua_ValueToNumber(L, 1);
    if (nidex < 0 || nidex >= MAX_NPC) { Lua_PushNumber(L, 1); return 1; }
    if (nidex)
        if (Npc[nidex].m_Kind==kind_player)
		 Lua_PushNumber(L, 1);
		else
         Lua_PushNumber(L, 0);
    else
         Lua_PushNumber(L, 0);
	return 1;
}


int LuaGetNpcKind(Lua_State * L)

{
    int nidex = (int)Lua_ValueToNumber(L, 1);
    if (nidex)
			Lua_PushNumber(L, Npc[nidex].m_Kind);
		else
			Lua_PushNumber(L, -1);
	return 1;
}

int LuaChangeTeamFeature(Lua_State * L)
{
	int nTeamIdx = (int)Lua_ValueToNumber(L, 1);
	int nNpcSetings = (int)Lua_ValueToNumber(L, 2);
	int nTime    = (int)Lua_ValueToNumber(L, 3);

	return 0;
}


int LuagetCreatRoleTime(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex>0) 
	{
		char nTempStr[64];
		ZeroMemory(nTempStr,sizeof(nTempStr));
		sprintf(nTempStr,"%u",Player[nPlayerIndex].m_nCreatRoleTime);
		Lua_PushString(L,nTempStr);
	}
	else
	    Lua_PushNil(L);

   return 1;
}

int LuaIsLeader(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0 ) 
	{
		if (Player[nPlayerIndex].m_cTeam.m_nFlag && Player[nPlayerIndex].m_cTeam.m_nFigure == TEAM_CAPTAIN)
			Lua_PushNumber(L,1);
		else 
			Lua_PushNumber(L,0);
		
	}
	else 
		Lua_PushNumber(L, 0);
	return 1;
}

int LuaGetCaptainName(Lua_State * L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum<=0)
	{
		int nPlayerIndex = GetPlayerIndex(L);
	    if (nPlayerIndex > 0)
		{ 
		    if (Player[nPlayerIndex].m_cTeam.m_nFlag)
			    Lua_PushString(L,Player[g_Team[Player[nPlayerIndex].m_cTeam.m_nID].m_nCaptain].m_PlayerName);
		    else Lua_PushNil(L);
		} 
	    else 
		 Lua_PushNil(L);
	}
	else
	{
       int nidex = (int)Lua_ValueToNumber(L, 1);
	   if (nidex>0  && nidex < MAX_NPC&& Npc[nidex].m_Kind==kind_player && Player[Npc[nidex].GetPlayerIdx()].m_cTeam.m_nFlag)    
			Lua_PushString(L,Player[g_Team[Player[Npc[nidex].GetPlayerIdx()].m_cTeam.m_nID].m_nCaptain].m_PlayerName);
       else Lua_PushNil(L);

	}
	return 1;
}

int LuaGetTeamId(Lua_State * L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum<=0)
	{
	    int nPlayerIndex = GetPlayerIndex(L);
	    if (nPlayerIndex > 0)
		{ //Player[nPlayerIndex].m_cTeam.m_nID
		     if (Player[nPlayerIndex].m_cTeam.m_nFlag)
			    Lua_PushNumber(L, Player[nPlayerIndex].m_cTeam.m_nID);
		        else 
				Lua_PushNumber(L,-1);
		}
	    else 
     	        Lua_PushNumber(L,-1);
	}
	else
	{
		int nidex = (int)Lua_ValueToNumber(L, 1);
		if (nidex>0  && nidex < MAX_NPC&& Npc[nidex].m_Kind==kind_player && Player[Npc[nidex].GetPlayerIdx()].m_cTeam.m_nFlag)    
			Lua_PushNumber(L,Player[Npc[nidex].GetPlayerIdx()].m_cTeam.m_nID);
       else 
		   Lua_PushNumber(L,-1);    
	}

	return 1;
}

int LuaGetTeamSize(Lua_State * L)
{
	int nTeamSize = 0;
	int nTeamId = -1;
	if (Lua_GetTopIndex(L) >= 1)
	{
		nTeamId = Lua_ValueToNumber(L, 1);
	}
	else
	{
		int nPlayerIndex = GetPlayerIndex(L);
		if (nPlayerIndex > 0)
		{
			if (Player[nPlayerIndex].m_cTeam.m_nFlag)
				nTeamId = Player[nPlayerIndex].m_cTeam.m_nID;
			else 
				nTeamId = -1;
		}
	}
	
	if (nTeamId < 0)
		nTeamSize = 0;
	else
		nTeamSize = g_Team[nTeamId].m_nMemNum + 1;
	Lua_PushNumber(L, nTeamSize);
	return 1;
}

int LuaGetTeamMem(Lua_State * L)
{
	int nTeamId = -1;
	int nMemId = 0;
	int nResult =0;
	if (Lua_GetTopIndex(L) >= 2)
	{
		nTeamId = Lua_ValueToNumber(L, 1);
		nMemId = Lua_ValueToNumber(L, 2);
	}
	else
	{
		int nPlayerIndex = GetPlayerIndex(L);
		if (nPlayerIndex > 0)
		{
			if (Player[nPlayerIndex].m_cTeam.m_nFlag)
				nTeamId = Player[nPlayerIndex].m_cTeam.m_nID;
			else 
				nTeamId = -1;
			nMemId = Lua_ValueToNumber(L, 1);
		}
	}
	
	if (nTeamId >= 0)
		nResult = nMemId==0?g_Team[nTeamId].m_nCaptain:g_Team[nTeamId].m_nMember[nMemId-1];

	Lua_PushNumber(L, nResult);
	return 1;
}

//获取某个人队伍的队员（除队长） playeridx
int LuaGetTeamMember(Lua_State * L)
{
    int nPlayerIndex = GetPlayerIndex(L);
	int nState = (int)Lua_ValueToNumber(L, 1);
	int nNpcDWIdx=0,nIndex=0;

	if  (nPlayerIndex<=0)
	{
		Lua_PushNumber(L, nNpcDWIdx);
		return 1;
	}

	if (Player[nPlayerIndex].m_cTeam.m_nFlag)
	{
		if  (Player[nPlayerIndex].m_cTeam.m_nID< 0 || Player[nPlayerIndex].m_cTeam.m_nID>=MAX_PLAYER)
		{
			Lua_PushNumber(L, nNpcDWIdx);
			return 1;
		}

		if (nState==0) //队长
			nNpcDWIdx=g_Team[Player[nPlayerIndex].m_cTeam.m_nID].m_nCaptain;
	    else
		{
			if (nState>0)
               nNpcDWIdx=g_Team[Player[nPlayerIndex].m_cTeam.m_nID].m_nMember[nState-1];
		}
	 //  nIndex=NpcSet.SearchID(nNpcDWIdx);
    }

	Lua_PushNumber(L, nNpcDWIdx);
	return 1;
}

int LuaLeaveTeam(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		if (Player[nPlayerIndex].m_cTeam.m_nFlag)
		{
			PLAYER_APPLY_LEAVE_TEAM	sLeaveTeam;
			sLeaveTeam.ProtocolType = c2s_teamapplyleave;
			Player[nPlayerIndex].LeaveTeam((BYTE*)&sLeaveTeam);
		}
	}
	return 0;
}
//解散队伍
int LuaSetCreateTeamOption(Lua_State * L)
{
	int nState = (int)Lua_ValueToNumber(L, 1);
	
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		if (nState)
			Player[nPlayerIndex].m_cTeam.SetCanTeamFlag(nPlayerIndex, TRUE);
		else
			Player[nPlayerIndex].m_cTeam.SetCanTeamFlag(nPlayerIndex, FALSE);
	}
	return 0;
}
//**************************************************************************************************************************************************************
//												聊天消息脚本
//**************************************************************************************************************************************************************
int LuaNew_Msg2Player(Lua_State * L)
{
	if (Lua_GetTopIndex(L) <= 0) 
		return 0;
	int nPlayerIndex=(int) Lua_ValueToNumber(L,1);
	if (nPlayerIndex > 0) 
	{
		char nMsg[256]={0};
		ZeroMemory(nMsg,sizeof(nMsg));
		g_StrCpyLen(nMsg,Lua_ValueToString(L,2), sizeof(nMsg));
        int nLeng  = TEncodeText(nMsg, strlen(nMsg));
		const char *  szMsg = nMsg;
		if (szMsg)
			KPlayerChat::SendSystemInfo(1, nPlayerIndex, MESSAGE_SYSTEM_TONGZHI_HEAD, (char*)szMsg, nLeng);//strlen(szMsg)
	}
	return 0;
}

int	LuaMsgToPlayer(Lua_State * L)
{
	if (Lua_GetTopIndex(L) <= 0) 
		return 0;
	int nPlayerIndex = GetPlayerIndex(L);

	if (nPlayerIndex > 0) 
	{
		char nMsg[256]={0};
		ZeroMemory(nMsg,sizeof(nMsg));
		//sprintf(nMsg,"%s",Lua_ValueToString(L,1));
		//char *nStrInfo =(char *)Lua_ValueToString(L,1);
		g_StrCpyLen(nMsg,Lua_ValueToString(L,1), sizeof(nMsg));
		/*if  (strlen(nStrInfo)>=125)
			return 0;

		sprintf(nMsg,"%s",nStrInfo);*/

        int nLeng  = TEncodeText(nMsg, strlen(nMsg));
		const char *  szMsg = nMsg;
		if (szMsg)
		{
			KPlayerChat::SendSystemInfo(1, nPlayerIndex, MESSAGE_SYSTEM_TONGZHI_HEAD, (char*)szMsg, nLeng);//strlen(szMsg)
		}
	}
	return 0;
}

int	LuaMsgToPlayera(Lua_State * L)
{
	if (Lua_GetTopIndex(L) <= 0) 
		return 0;
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0) 
	{
		char nMsg[125];
		g_StrCpyLen(nMsg,Lua_ValueToString(L,1),sizeof(nMsg));
        int nLeng = TEncodeText(nMsg, strlen(nMsg));
		const char *  szMsg = nMsg;
		if (szMsg)
			KPlayerChat::SendSystemInfo(1, nPlayerIndex, K_MESSAGE_SYSTEM_ANNOUCE_HEAD, (char*)szMsg, nLeng);
	}
	
	return 0;
}
//给队伍发信息
int LuaMsgToTeam(Lua_State * L)
{
	if (Lua_GetTopIndex(L) <= 0	) return 0;
	
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		if (Player[nPlayerIndex].m_cTeam.m_nID >= 0)
		{
			char nMsg[125];
			g_StrCpyLen(nMsg,Lua_ValueToString(L,1),sizeof(nMsg));
           int nLeng = TEncodeText(nMsg, strlen(nMsg));
			const	char * szMsg = nMsg;
			
			int nTeamLeaderId = g_Team[Player[nPlayerIndex].m_cTeam.m_nID].m_nCaptain;
			if (nTeamLeaderId > 0)
				KPlayerChat::SendSystemInfo(1, nTeamLeaderId, "队伍",(char *) szMsg, nLeng);
			
			for (int i = 0; i < MAX_TEAM_MEMBER; i ++)
			{
				int nMemberId = g_Team[Player[nPlayerIndex].m_cTeam.m_nID].m_nMember[i] ;
				if (nMemberId > 0)
				{
					if (szMsg)

						KPlayerChat::SendSystemInfo(1, nMemberId, "队伍", (char *)szMsg, nLeng);
				}
			}
		}
	}
	return 0;
}
//给某个队伍发信息
int LuaMsgToOneTeam(Lua_State * L)
{
	if (Lua_GetTopIndex(L) <= 0	) return 0;
	
	        int nTeamid=(int) Lua_ValueToNumber(L,1);
	//	if (Player[nPlayerIndex].m_cTeam.m_nID >= 0)

			char nMsg[125]={0};
			g_StrCpyLen(nMsg,Lua_ValueToString(L,2),sizeof(nMsg));
           int nLeng = TEncodeText(nMsg, strlen(nMsg));
			const	char * szMsg = nMsg;
			
			if (nTeamid < 0 || nTeamid >= MAX_TEAM) return 0;
			int nTeamLeaderId = g_Team[nTeamid].m_nCaptain;
			if (nTeamLeaderId > 0)
				KPlayerChat::SendSystemInfo(1, nTeamLeaderId, "队伍提示",(char *) szMsg, nLeng);
			
			for (int i = 0; i < MAX_TEAM_MEMBER; i ++)
			{
				if (nTeamid < 0 || nTeamid >= MAX_TEAM) return 0;
				int nMemberId = g_Team[nTeamid].m_nMember[i] ;
				if (nMemberId > 0)
				{
					if (szMsg)
						
						KPlayerChat::SendSystemInfo(1, nMemberId, "队伍提示", (char *)szMsg, nLeng);
				}
			}
	
	return 0;
}

int LuaShowDBactivityCount(Lua_State * L)
{
	int m_Count = 0;
	if  (!g_DbActivity) return 0;
	ZCursorS *cursor = g_DbActivity->first();
	while(cursor)
	{	
		TDBactivitydata* pItemData = (TDBactivitydata*)cursor->data;
		if (pItemData && cursor->size >= (int)sizeof(TDBactivitydata) && pItemData->iid>0&& pItemData->nMapIndex>0)
		{
			printf("--%d地图:%d,占领:%s,Key:%s--\n",m_Count+1,pItemData->nMapIndex,pItemData->nOwerName,pItemData->nNameKey);
			m_Count++;
		}
	   if(!g_DbActivity->next(cursor))
		 break;
	}

	Lua_PushNumber(L,m_Count);
	return 1;
}

int LuaGetActivityCount(Lua_State * L)
{
	const char*	pszName = (const char*)Lua_ValueToString(L, 1);
	if  (!pszName) return 0;
	int m_Count = 0;
	if  (!g_DbActivity) return 0;
	char nInfo[512];
	ZeroMemory(nInfo,sizeof(nInfo));
	char nMapName[32],_nMapName[32];
	ZeroMemory(nMapName,sizeof(nMapName));
	ZeroMemory(_nMapName,sizeof(_nMapName));
	ZCursorS *cursor = g_DbActivity->first();
	strcat(nInfo,"查询内容如下:\n");
	while(cursor)
	{	
		TDBactivitydata* pItemData = (TDBactivitydata*)cursor->data;
		if (pItemData && cursor->size >= (int)sizeof(TDBactivitydata) && pItemData->iid>0&& pItemData->nMapIndex>0)
		{
			if (strcmp(pItemData->nOwerName,pszName) == 0)
			{//名字相同的
				sprintf(nMapName,"%d_name",pItemData->nMapIndex);
				g_NpcMapDropRate.GetString("List",nMapName,"暂无数据",_nMapName,sizeof(_nMapName));
				char _Info[128];
				//_snprintf(_Info,sizeof(_Info)-1,lpSrc);
				snprintf(_Info, sizeof(_Info),"城市:%s,占领城主:%s \n",_nMapName,pItemData->nOwerName);
				strcat(nInfo,_Info);
				m_Count++;
			}
			//printf("--%d地图:%d,占领:%s,Key:%s--\n",m_Count+1,pItemData->nMapIndex,pItemData->nOwerName,pItemData->nNameKey);
		}
		if(!g_DbActivity->next(cursor))
			break;
	}

	Lua_PushNumber(L,m_Count);
	Lua_PushString(L,nInfo);
	return 2;
}

int LuaSetSubWorldMaster(Lua_State * L)
{
	int nSunWorlID=(int) Lua_ValueToNumber(L,1);
	char *nName=(char *)Lua_ValueToString(L,2);
	char *nDBNameKey=(char *)Lua_ValueToString(L,3);
	int nIndex = g_SubWorldSet.SearchWorld(nSunWorlID); 
	if  (nIndex>=0 && nIndex<MAX_SUBWORLD && nName && nDBNameKey)
	{
	    SubWorld[nIndex].nWorldMapInfo[STR_MAP_MASTER]=nName;
		if (g_DbActivity)
		{//写数据
			char * pItemBuffer = (char*)_alloca(sizeof(TDBactivitydata));
			TDBactivitydata *nCurData =(TDBactivitydata *)pItemBuffer;
			nCurData->iid=nSunWorlID;
			nCurData->nMapIndex=nSunWorlID;
			nCurData->nIsOwer=1;
			g_StrCpyLen(nCurData->nOwerName, nName, sizeof(nCurData->nOwerName));
			char nKeyInfo[64];
			ZeroMemory(nKeyInfo,sizeof(nKeyInfo));
			snprintf(nKeyInfo, sizeof(nKeyInfo),"%s_%d",nDBNameKey,nSunWorlID);
			sprintf(nCurData->nNameKey,"%s",nKeyInfo);
			if (!g_DbActivity->add(nKeyInfo,strlen(nKeyInfo)+1,pItemBuffer,sizeof(TDBactivitydata)))
				printf("--数据KEY:%s,地图:%d,添加失败!--\n",nKeyInfo,nSunWorlID);
			else
				printf("--数据KEY:%s,地图:%d,添加成功!--\n",nKeyInfo,nSunWorlID);
		   //if (!g_DbItemFile->add(it->second.iaccName,strlen(it->second.iaccName)+1,pItemBuffer,sizeof(TDBShopItemData))) 
		}
	}

	return 0;
}

int LuaSetSubWorldMaster_t(Lua_State * L)
{
	int nSunWorlID=(int) Lua_ValueToNumber(L,1);
	char *nName=(char *)Lua_ValueToString(L,2);
	int nIndex = g_SubWorldSet.SearchWorld(nSunWorlID); 
	if  (nIndex>=0 && nIndex<MAX_SUBWORLD)
		SubWorld[nIndex].nWorldMapInfo[STR_MAP_TEMP_MASTER]=nName;

	return 0;
}


int LuaGetSubWorldMaster(Lua_State * L)
{
	int nSunWorlID=(int)Lua_ValueToNumber(L,1);
	int nIndex = g_SubWorldSet.SearchWorld(nSunWorlID); 
	if  (nIndex<0 || nIndex>=MAX_SUBWORLD) return 0;
	Lua_PushString(L,SubWorld[nIndex].nWorldMapInfo[STR_MAP_MASTER].c_str());
	return 1;
}

int LuaGetSubWorldMasterCount(Lua_State * L)
{
	/*int nSunWorlID=(int) Lua_ValueToNumber(L,1);
	int nIndex = g_SubWorldSet.SearchWorld(nSunWorlID); 
	if  (nIndex<0 || nIndex>=MAX_SUBWORLD) return 0;
	Lua_PushString(L,SubWorld[nIndex].nWorldMapInfo[STR_MAP_MASTER].c_str());*/
	char *nName=(char *)Lua_ValueToString(L,1);
	int nCounr = 0;
	char nScrName[32];
	for (int i = 0; i < MAX_SUBWORLD; ++i)
	{
		ZeroMemory(nScrName,sizeof(nScrName));
		snprintf(nScrName, sizeof(nScrName), "%s", SubWorld[i].nWorldMapInfo[STR_MAP_MASTER].c_str());
		if  (strcmpi(nScrName,nName)==0)
		    nCounr++;
	}
	Lua_PushNumber(L,nCounr);
	return 1;
}


int LuaGetMapLimitLevel(Lua_State * L)
{
	int nSunWorlID=(int)Lua_ValueToNumber(L,1);
	int nIndex = g_SubWorldSet.SearchWorld(nSunWorlID);
	if  (nIndex<0 || nIndex>=MAX_SUBWORLD) return 0;

	 Lua_PushNumber(L,SubWorld[nIndex].m_minLimitLevel);
	 Lua_PushNumber(L,SubWorld[nIndex].m_maxLimitLevel);
	 return 2;
}

int LuaGetMapLimitLevelByIndex(Lua_State * L)
{
	int nIndex=(int)Lua_ValueToNumber(L,1);
	if  (nIndex<0 || nIndex>=MAX_SUBWORLD) return 0;

	Lua_PushNumber(L,SubWorld[nIndex].m_minLimitLevel);
	Lua_PushNumber(L,SubWorld[nIndex].m_maxLimitLevel);
	return 2;
}


int LuaSetMapLimitLevelByIndex(Lua_State * L)
{
	int nIndex=(int)Lua_ValueToNumber(L,1);
	int minLimitLevel=(int)Lua_ValueToNumber(L,2);
	int maxLimitLevel=(int)Lua_ValueToNumber(L,3);

	if  (nIndex<0 || nIndex>=MAX_SUBWORLD) return 0;

	SubWorld[nIndex].m_minLimitLevel=minLimitLevel;//限制的等级
	SubWorld[nIndex].m_maxLimitLevel=maxLimitLevel;//限制的等级

	return 0;
}

int LuaSetMapLimitLevel(Lua_State * L)
{
	int nSunWorlID=(int)Lua_ValueToNumber(L,1);
	int minLimitLevel=(int)Lua_ValueToNumber(L,2);
	int maxLimitLevel=(int)Lua_ValueToNumber(L,3);

	int nIndex = g_SubWorldSet.SearchWorld(nSunWorlID);
	if  (nIndex<0 || nIndex>=MAX_SUBWORLD) return 0;

	SubWorld[nIndex].m_minLimitLevel=minLimitLevel;//限制的等级
	SubWorld[nIndex].m_maxLimitLevel=maxLimitLevel;//限制的等级

	return 0;
}


int LuaSetMapNoSeriesByIndex(Lua_State * L)
{
	int nIndex=(int)Lua_ValueToNumber(L,1);
	int nVer=(int)Lua_ValueToNumber(L,2);
	if  (nIndex<0 || nIndex>=MAX_SUBWORLD) return 0;
	SubWorld[nIndex]._IsNoSeries=(nVer>0?true:false);

	return 0;
}

int LuaSetMapNoSeries(Lua_State * L)
{
	int nSunWorlID=(int)Lua_ValueToNumber(L,1);
	int nVer=(int)Lua_ValueToNumber(L,2);
	int nIndex = g_SubWorldSet.SearchWorld(nSunWorlID);
	if  (nIndex<0 || nIndex>=MAX_SUBWORLD) return 0;

	SubWorld[nIndex]._IsNoSeries=(nVer>0?true:false);
	 return 0;
}


int LuaSetMapNoLostMoneyByIndex(Lua_State * L)
{
	int nIndex=(int)Lua_ValueToNumber(L,1);
	int nVer=(int)Lua_ValueToNumber(L,2);
	if  (nIndex<0 || nIndex>=MAX_SUBWORLD) return 0;
	SubWorld[nIndex]._IsNoLostMoney=(nVer>0?true:false);

	return 0;
}

int LuaSetMapNoLostMoney(Lua_State * L)
{
	int nSunWorlID=(int)Lua_ValueToNumber(L,1);
	int nVer=(int)Lua_ValueToNumber(L,2);
	int nIndex = g_SubWorldSet.SearchWorld(nSunWorlID);
	if  (nIndex<0 || nIndex>=MAX_SUBWORLD) return 0;

	SubWorld[nIndex]._IsNoLostMoney=(nVer>0?true:false);
	return 0;
}


int LuaSetMapNoLostItemByIndex(Lua_State * L)
{
	int nIndex=(int)Lua_ValueToNumber(L,1);
	int nVer=(int)Lua_ValueToNumber(L,2);
	if  (nIndex<0 || nIndex>=MAX_SUBWORLD) return 0;
	SubWorld[nIndex]._IsNoLostItem=(nVer>0?true:false);

	return 0;
}

int LuaSetMapNoLostItem(Lua_State * L)
{
	int nSunWorlID=(int)Lua_ValueToNumber(L,1);
	int nVer=(int)Lua_ValueToNumber(L,2);
	int nIndex = g_SubWorldSet.SearchWorld(nSunWorlID);
	if  (nIndex<0 || nIndex>=MAX_SUBWORLD) return 0;

	SubWorld[nIndex]._IsNoLostItem=(nVer>0?true:false);
	return 0;
}

int LuaSetMapNoCostByIndex(Lua_State * L)
{
	int nIndex=(int)Lua_ValueToNumber(L,1);
	int nVer=(int)Lua_ValueToNumber(L,2);
	if  (nIndex<0 || nIndex>=MAX_SUBWORLD) return 0;
	SubWorld[nIndex]._IsNoCost=(nVer>0?true:false);

	return 0;
}

int LuaSetMapNoCost(Lua_State * L)
{
	int nSunWorlID=(int)Lua_ValueToNumber(L,1);
	int nVer=(int)Lua_ValueToNumber(L,2);
	int nIndex = g_SubWorldSet.SearchWorld(nSunWorlID);
	if  (nIndex<0 || nIndex>=MAX_SUBWORLD) return 0;

	SubWorld[nIndex]._IsNoCost=(nVer>0?true:false);
	return 0;
}

int LuaSetMapNoDamageByIndex(Lua_State * L)
{
	int nIndex=(int)Lua_ValueToNumber(L,1);
	int nVer=(int)Lua_ValueToNumber(L,2);
	if  (nIndex<0 || nIndex>=MAX_SUBWORLD) return 0;
	SubWorld[nIndex]._IsNoDamage=(nVer>0?true:false);

	return 0;
}

int LuaSetMapNoDamage(Lua_State * L)
{
	int nSunWorlID=(int)Lua_ValueToNumber(L,1);
	int nVer=(int)Lua_ValueToNumber(L,2);
	int nIndex = g_SubWorldSet.SearchWorld(nSunWorlID);
	if  (nIndex<0 || nIndex>=MAX_SUBWORLD) return 0;

	SubWorld[nIndex]._IsNoDamage=(nVer>0?true:false);
	return 0;
}




int LuaShowMapRegInfo(Lua_State * L)
{
	int nSunWorlID=(int)Lua_ValueToNumber(L,1);
	int nIndex = g_SubWorldSet.SearchWorld(nSunWorlID);

	if  (nIndex<0 || nIndex>=MAX_SUBWORLD) return 0;

	SubWorld[nIndex].showActivateRegionInfo();

	Lua_PushNumber(L,SubWorld[nIndex].getActiveRegionCount());
	return 1;
}


int LuaSetSubWorldSuoHun(Lua_State * L)
{
	int nSunWorlID=(int)Lua_ValueToNumber(L,1);
	int nVer      =(int)Lua_ValueToNumber(L,2);
	int nIndex = g_SubWorldSet.SearchWorld(nSunWorlID); 
	if  (nIndex<0 || nIndex>=MAX_SUBWORLD) return 0;
	SubWorld[nIndex].m_suoHun = nVer>0?true:false;
	return 0;
}

int LuaGetSubWorldMaster_t(Lua_State * L)
{
	int nSunWorlID=(int) Lua_ValueToNumber(L,1);
	int nIndex = g_SubWorldSet.SearchWorld(nSunWorlID); 
	if  (nIndex<0 || nIndex>=MAX_SUBWORLD) return 0;
	Lua_PushString(L,SubWorld[nIndex].nWorldMapInfo[STR_MAP_TEMP_MASTER].c_str());
	return 1;
}


int LuaCheckMapNoReturn(Lua_State * L)
{
   int nSunWorlID=(int) Lua_ValueToNumber(L,1);

   int nReg=1;

   if (nSunWorlID<=0)
   {
	   Lua_PushNumber(L,nReg);
	   return 1;
   }

   	char nValKey[32]={0};
	sprintf(nValKey,"%d_IsNoReturn",nSunWorlID);
	int nIsNoReturn=0;
	g_NpcMapDropRate.GetInteger("List",nValKey,0,&nIsNoReturn);
	Lua_PushNumber(L,nIsNoReturn);
	return 1;
}

//MsgToSubWorld(msg)
int LuaMsgToSubWorld(Lua_State * L)
{
	int nParnCount;
	// `<=' binds tighter than `=', so nParnCount held a verdict: the `>=2' test below could not
	// be true and a script's second argument to MsgToSubWorld was ignored.
	if ((nParnCount = Lua_GetTopIndex(L)) <= 0) return 0;
	char nMsg[256];
	ZeroMemory(nMsg,sizeof(nMsg));
	g_StrCpyLen(nMsg,  Lua_ValueToString(L,1), sizeof(nMsg));
	//sprintf(nMsg,Lua_ValueToString(L,1));
	int nKind=-1;
	if (nParnCount>=2)
	{
		char nTempName[256];
		ZeroMemory(nTempName,sizeof(nTempName));
		// `int' here declared a second nKind, in scope only until this block's `}': the switch below
		// read it, and the SendSystemInfo after the block still names the -1 of line 6368, so the
		// message kind a script passed as its second argument never reached the packet.  The
		// sibling MsgToSubWorldMap (6543) is the same function for one map and assigns the outer
		nKind = (int)Lua_ValueToNumber(L,2);
		switch (nKind)
		{
		case 1:
			if (NULL == Utf8ToGbk(nMsg, nTempName, sizeof(nTempName))) nTempName[0] = '\0';
			break;
		default:
			snprintf(nTempName, sizeof(nTempName), "%s", GbkToUtf8(nMsg).c_str());
			break;
		}
		snprintf(nMsg, sizeof(nMsg), "%s", nTempName);
	}
    int nleg=TEncodeText(nMsg,strlen(nMsg));
	const char * szMsg = nMsg;
	if (szMsg)
	{
		KPlayerChat::SendSystemInfo(0, 0, K_MESSAGE_SYSTEM_ANNOUCE_HEAD, (char*)szMsg, nleg, nKind);  //strlen(szMsg) 
	}

	return 0;
}


int LuaGm_addNpc(Lua_State * L)
{
	if (Lua_GetTopIndex(L) <= 0 ) return 0;

	tagMsgInGame	sLeaveGame;    
	sLeaveGame.cProtocol = c2s_msginfo_all;
	ZeroMemory(sLeaveGame.strMsg,sizeof(sLeaveGame.strMsg));
	g_StrCpyLen(sLeaveGame.strMsg,Lua_ValueToString(L,1), sizeof(sLeaveGame.strMsg));
	sLeaveGame.cCmdType  = AR_ADDNPC_SERVER;
	sLeaveGame.nSerIdx   = nCurSerIdx; //哪个大区的
	sLeaveGame.nMgsIdx   = (int)Lua_ValueToNumber(L,2);
	sLeaveGame.nValIdx   = 0;

	if (g_pGateWayClient)
		g_pGateWayClient->SendPackToServer(&sLeaveGame, sizeof(tagMsgInGame));

	return 0;
}

int LuaGm_LocalScript(Lua_State * L)
{
	if (Lua_GetTopIndex(L) <= 0 ) return 0;

	tagMsgInGame	sLeaveGame;    
	sLeaveGame.cProtocol = c2s_msginfo_all;
	ZeroMemory(sLeaveGame.strMsg,sizeof(sLeaveGame.strMsg));
	g_StrCpyLen(sLeaveGame.strMsg,  Lua_ValueToString(L,1), sizeof(sLeaveGame.strMsg));
	sLeaveGame.cCmdType  = AR_LOADSCRIPT_SERVER;
	sLeaveGame.nSerIdx   = nCurSerIdx; //哪个区的
	sLeaveGame.nMgsIdx   = -1;
	sLeaveGame.nValIdx   = 0;

	if (g_pGateWayClient)
		g_pGateWayClient->SendPackToServer(&sLeaveGame, sizeof(tagMsgInGame));

	return 0;
}

int LuaGetSerVerInfo(Lua_State * L)
{
	Lua_PushNumber(L, nCurSerIdx);
	Lua_PushNumber(L, nCurGsNumber);
	return 2;
}

//向某个服务器发送公告
int LuaMsgToSubWorld_All(Lua_State * L)
{
	if (Lua_GetTopIndex(L) <= 0 ) return 0;

	tagMsgInGame	sLeaveGame;    
	sLeaveGame.cProtocol = c2s_msginfo_all;
	ZeroMemory(sLeaveGame.strMsg,sizeof(sLeaveGame.strMsg));
	g_StrCpyLen(sLeaveGame.strMsg,  Lua_ValueToString(L,1), sizeof(sLeaveGame.strMsg));
	sLeaveGame.cCmdType  = AP_NOTIFY_ALL_PLAYER;
	sLeaveGame.nSerIdx   = nCurSerIdx; //哪个区的
	sLeaveGame.nMgsIdx  = -1;
	sLeaveGame.nValIdx   = 0;

	if (g_pGateWayClient)
		g_pGateWayClient->SendPackToServer(&sLeaveGame, sizeof(tagMsgInGame));

	return 0;
}

//向某个服务器发送公告
int LuaMsgToSubSerVerWorld(Lua_State * L)
{
	if (Lua_GetTopIndex(L) <= 0 ) return 0;
	int mSevIdx=(int)Lua_ValueToNumber(L,1);
	int mMsgIdx=(int)Lua_ValueToNumber(L,2);

	tagMsgInGame	sLeaveGame;  
	ZeroMemory(sLeaveGame.strMsg,sizeof(sLeaveGame.strMsg));
	sLeaveGame.cProtocol = c2s_msginfo;
	sLeaveGame.cCmdType  = 0;
	sLeaveGame.nSerIdx   = mSevIdx;
	sLeaveGame.nMgsIdx   = mMsgIdx;
	sLeaveGame.nValIdx   = 0;
	if (g_pGateWayClient)
		g_pGateWayClient->SendPackToServer(&sLeaveGame, sizeof(tagMsgInGame));

	return 0;
}

//踢掉本服所有的跨服玩家
int LuaKickOutPlayerThisGame(Lua_State * L)
{
	PlayerSet.KickOutPlayerThisGame();
	return 0;
}

//向某个服务器发送踢不是本服务器的玩家下线
int LuaKickOutPlayerInGame(Lua_State * L)
{
	if (Lua_GetTopIndex(L) <= 0 ) return 0;
	int mSevIdx=(int)Lua_ValueToNumber(L,1);  //某个服务器中不是本区的玩家
	tagMsgInGame	sLeaveGame;               //离开游戏时  扣除游戏点卡
	sLeaveGame.cProtocol = c2s_msginfo;
	sLeaveGame.cCmdType  = 2;
	sLeaveGame.nSerIdx   = mSevIdx;
	sLeaveGame.nMgsIdx   = 0;
	sLeaveGame.nValIdx   = 0;
	if (g_pGateWayClient)
		g_pGateWayClient->SendPackToServer(&sLeaveGame, sizeof(tagMsgInGame));
	return 0;
}

//向某个服务器发送设置全局变量
int LuaSetSubSerGlobalMission(Lua_State * L)
{
	if (Lua_GetTopIndex(L) <= 0 ) return 0;
	int mSevIdx=(int)Lua_ValueToNumber(L,1);
	int mMsgIdx=(int)Lua_ValueToNumber(L,2);
	int mValIdx=(int)Lua_ValueToNumber(L,3);

	if (mMsgIdx  < 0)
		return 0;

	tagMsgInGame	sLeaveGame;     //离开游戏时  扣除游戏点卡
	sLeaveGame.cProtocol = c2s_msginfo;
	sLeaveGame.cCmdType  = 1;
	sLeaveGame.nSerIdx   = mSevIdx;
	sLeaveGame.nMgsIdx   = mMsgIdx; //全局的参数
	sLeaveGame.nValIdx  = mValIdx; //值
	if (g_pGateWayClient)
		g_pGateWayClient->SendPackToServer(&sLeaveGame, sizeof(tagMsgInGame));
	//g_GlobalMissionArray.GetMissionValue(28)
	return 0;
}
//向某个地图发送全体消息
int LuaMsgToSubWorldMap(Lua_State * L)
{
	int nPartCount;
	// the same precedence mistake as MsgToSubWorld: nPartCount held 0 or 1, so `>=4' below could
	// not be true and the message kind was always the default.
	if ((nPartCount = Lua_GetTopIndex(L)) <= 0) return 0;
	int nMapIdx=(int)Lua_ValueToNumber(L,1);
	char *nSendName=(char *)Lua_ValueToString(L,2);
	char nStrName[64]={0};
	if (nSendName==NULL)
	   snprintf(nStrName, sizeof(nStrName), "%s", strCoreInfo[MESSAGE_SYSTEM_ANNOUCE_HEAD].c_str());
	else
	   snprintf(nStrName, sizeof(nStrName), "%s", nSendName);

	char nMsg[250]={0};
	g_StrCpyLen(nMsg,  Lua_ValueToString(L,3), sizeof(nMsg));
	int nKind=-1;
	if (nPartCount>=4)
		nKind =(int)Lua_ValueToNumber(L,4);

    int nleg=TEncodeText(nMsg, strlen(nMsg));
	const char * szMsg = nMsg;
	if (szMsg)
	{
		KPlayerChat::SendSystemMapInfo(0,0,nMapIdx,nStrName, (char *)szMsg, nleg,nKind);  //strlen(szMsg) 
	}
	
	return 0;
}

int LuaNpcSayToSubWorld(Lua_State * L)
{

    if (Lua_GetTopIndex(L) < 3 ) return 0;

     int nNpcIdx=0,nTime=0,nModel=0;
     char *nMsg=NULL,*nTileTxt=NULL;

 
     nNpcIdx=(int) Lua_ValueToNumber(L,1);
	 nMsg =(char *)Lua_ValueToString(L,2);
	 nTime=(int) Lua_ValueToNumber(L,3);  //分钟

   
    if (Lua_GetTopIndex(L)==4)
    {
	   nTileTxt=(char *)Lua_ValueToString(L,4);
    }

	if (Lua_GetTopIndex(L)>=5)
    {
		nModel=(int)Lua_ValueToNumber(L,5);
    }

	 if (nNpcIdx>0 && nNpcIdx < MAX_NPC)
	 {

	   //sprintf(Npc[nNpcIdx].m_NpcAutoChat.m_NpcTalkMsg,nMsg);
	   Npc[nNpcIdx].SetstrInfo(STR_NPCTALK_MSG,nMsg);
	   Npc[nNpcIdx].m_NpcAutoChat.m_Model = nModel;

	   if (nTileTxt)
	      //sprintf(Npc[nNpcIdx].m_NpcAutoChat.m_NpcTalkName,nTileTxt);
		  Npc[nNpcIdx].SetstrInfo(STR_NPCTALK_NAME,nTileTxt);
	   else
		  //sprintf(Npc[nNpcIdx].m_NpcAutoChat.m_NpcTalkName,Npc[nNpcIdx].Name);
		  Npc[nNpcIdx].SetstrInfo(STR_NPCTALK_NAME,Npc[nNpcIdx].Name);

	   if (nTime>0)
	   {//服务间隔多长时间自己发送
          Npc[nNpcIdx].m_NpcAutoChat.m_NpcTalkTime=nTime;
	   }
	   else
	   {
	      if (nTileTxt)
             Npc[nNpcIdx].NpcTalkToSubWorld(nTileTxt,nModel);
		  else
	         Npc[nNpcIdx].NpcTalkToSubWorld(NULL,nModel);  //一次行发送
	   }
	 }

	 return 0;
}

//动态重置地图
int LuaLoadActivityMap(Lua_State * L)
{
	int nSubIndex = -1;

	int nMapIndex = (int)Lua_ValueToNumber(L,1);

	if  (nMapIndex<0)
	{
		printf("---地图索引 小于 0 ,加载失败!---- \n");
		return 0;
	}
	int i;

	for (i=0;i<MAX_SUBWORLD;i++)
	{
		if (SubWorld[i].m_SubWorldID<=-1)
		{
			nSubIndex = i;
			break;
		}
	}

	if  (nSubIndex<=-1)
	{
		printf("---地图加载,暂无空索引,加载失败!---- \n");
		return 0;
	}

	int   nIsHave=-1;

	for (i=0;i<MAX_SUBWORLD;i++)
	{
		if (SubWorld[i].m_SubWorldID >-1 && SubWorld[i].m_SubWorldID == nMapIndex)
		{
			nIsHave = i;
			break;
		}
	}

	if  (nIsHave >-1)
	{
		printf("---地图加载,该地图(%d:%d)已经存在,加载失败!---- \n",nIsHave,nMapIndex);
		return 0;
	}

	if (SubWorld[nSubIndex].m_SubWorldID>-1)
	{//地图存在的,删除这个地图所有的NPC 然后在重载
	   //SubWorld[nSubIndex].DelAllNpcInWro();
		printf("----该地图(%d)已经存在,加载失败!---- \n",SubWorld[nSubIndex].m_SubWorldID);
		return 0;
	}

	SubWorld[nSubIndex].m_nIndex = nSubIndex;

	int nNpcCount = SubWorld[nSubIndex].LoadMapS(nMapIndex);

	printf("---地图SubWorld[%d].LoadMapS(%d),NpcCount:%d 加载成功!---- \n",nSubIndex,nMapIndex,nNpcCount);

	return 0;
}

int LuaNpcChat(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 2)
		return 0;
	
	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	
	if (nNpcIndex > 0 && nNpcIndex < MAX_NPC)
	{
		char* szMsg = (char*)Lua_ValueToString(L,2);
		char nStrMsg[MAX_SENTENCE_LENGTH]={0};
	    g_StrCpyLen(nStrMsg, szMsg, sizeof(nStrMsg));
		int nLen= TEncodeText(nStrMsg, strlen(nStrMsg));
		if(nLen >= MAX_MESSAGE_LENGTH)
			nStrMsg[nLen-1]='\0';
		NPC_CHAT_SYNC	command;
		command.ProtocolType = s2c_npctalk;
		command.ID = Npc[nNpcIndex].m_dwID;
		//command.nTime=(int) Lua_ValueToNumber(L,3);  //分钟
		snprintf(command.szMsg, sizeof(command.szMsg), "%s", nStrMsg);
		command.nMsgLen=nLen;
		Npc[nNpcIndex].SendDataToNearRegion(&command, sizeof(NPC_CHAT_SYNC));
	}
	
	return 0;
}


int LuaMsgToAroundRegion(Lua_State * L)
{
	
	return 0;
}

//**************************************************************************************************************************************************************
//												主角脚本
//**************************************************************************************************************************************************************

//**************************************************************************************************************************************************************
//												主角脚本
//**************************************************************************************************************************************************************

/*功能：让玩家进入新的一个游戏世界
nPlayerIndex:主角的Index
nSubWorldIndex:游戏世界id
nPosX:
nPosY:
*/
int  LuaForceTransWorld(Lua_State * L) 
{//强行跨服
	DWORD dwWorldId = (DWORD)Lua_ValueToNumber(L, 1);
	int nPosx       = (int)Lua_ValueToNumber(L,2);
	int nPosy       = (int)Lua_ValueToNumber(L,3);
	INT nIsTure     = (int)Lua_ValueToNumber(L,4);
	INT nLevel      = (int)Lua_ValueToNumber(L,5);

	PlayerSet.ForceTransWorld(dwWorldId,nPosx,nPosy,nIsTure,nLevel);

	return 0;
}
int  LuaEnterTransWorld(Lua_State * L) 
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0) 
		return 0;

	int nResult = 0;

	if (Lua_GetTopIndex(L) >= 4)
	{
		DWORD dwWorldId = (DWORD)Lua_ValueToNumber(L, 1);
		int nPosx       = (int)Lua_ValueToNumber(L,2);
		int nPosy       = (int)Lua_ValueToNumber(L,3);
		INT  nIsTure   = (int)Lua_ValueToNumber(L,4);

		BOOL  nIsChange = FALSE;
		if  (nIsTure>0)
			  nIsChange = TRUE;

		Player[nPlayerIndex].m_TempSerVer = nIsTure;
		//nResult =1;
		nResult = Npc[Player[nPlayerIndex].m_nIndex].TobeExchangeServer(dwWorldId,nPosx*32,nPosy*32,nIsChange);
	}
	Lua_PushNumber(L, nResult);
	return 1;
}


int  LuaGetTransSerInfo(Lua_State * L) 
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0) 
		return 0;

	Lua_PushNumber(L, Player[nPlayerIndex].m_CurSerVer);
	Lua_PushNumber(L, Player[nPlayerIndex].m_TempSerVer);
	return 2;
}



int   LuaST_SyncMiniMapObj(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0) 
		return 0;

	int _mpsX = (int)Lua_ValueToNumber(L, 1);
	int _mpsY = (int)Lua_ValueToNumber(L, 2);

	// 通知客户端
	OBJ_SYNC_FINDPATH	sCreate;
	sCreate.ProtocolType = s2c_objadd_vn;
	sCreate._mapID      = -1;
	sCreate._mpsPosX = _mpsX;
	sCreate._mpsPosY = _mpsY;

	if (g_pServer)
		g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, &sCreate, sizeof(OBJ_SYNC_FINDPATH));

	return 0;
}

//NewWorld(WorldId, X,Y)
int  LuaEnterNewWorld(Lua_State * L) 
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0) 
		return 0;
	
	int nResult = 0;

	int  nParmCount = Lua_GetTopIndex(L);

	if (nParmCount >= 3)
	{
		DWORD dwWorldId = (DWORD)Lua_ValueToNumber(L, 1);

		int  changeServer = true;
		if  (nParmCount>=4)
			 changeServer = (int)Lua_ValueToNumber(L, 4);

		nResult = Npc[Player[nPlayerIndex].m_nIndex].ChangeWorld(dwWorldId, (int)Lua_ValueToNumber(L,2) * 32, (int)Lua_ValueToNumber(L,3) * 32,changeServer);
	}
	Lua_PushNumber(L, nResult);
	return 1;
}


int  LuaEnterRodomWorld(Lua_State * L) 
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0) 
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	int nParamCount = Lua_GetTopIndex(L);
	int nResult = 0;
	///Npc[Player[nPlayerIndex].m_nIndex].GetMpsPos(&nPosX, &nPosY,&nPmap);	
    if  (nParamCount)
	{ 
        int nX = (int) Lua_ValueToNumber(L,1);

		if (nX>=0 && nX<MAX_SUBWORLD)
		    nResult = Npc[Player[nPlayerIndex].m_nIndex].ChangeRodomWorld(SubWorld[nX].m_SubWorldID);
	} 
    else
	{ 
	    int nSubWorldIndex = Npc[Player[nPlayerIndex].m_nIndex].m_SubWorldIndex;
        if (nSubWorldIndex>0)
		{ 
		  nResult = Npc[Player[nPlayerIndex].m_nIndex].ChangeRodomWorld(SubWorld[nSubWorldIndex].m_SubWorldID);
		} 
	}	 	 
    Lua_PushNumber(L, nResult);
	return 1;
}


int  LuaGetAroundNpcList(Lua_State * L)
{
    int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex<=0) return 0;
    int nNpcIdx=Player[nPlayerIndex].m_nIndex;
    int nRegion = Npc[nNpcIdx].m_RegionIndex;
	int	nSubWorld = Npc[nNpcIdx].m_SubWorldIndex;
	int	nMapX = Npc[nNpcIdx].m_MapX;  //NPC当前的格子坐标
	int	nMapY = Npc[nNpcIdx].m_MapY;  //NPC当前的格子坐标
	int nRMx,nRMy,nSearchRegion;

    int nRangeX=(int) Lua_ValueToNumber(L,1);
	int nRangeY=(int) Lua_ValueToNumber(L,2);
	DWORD nRelation=(int) Lua_ValueToNumber(L,3);
	int nType=(int) Lua_ValueToNumber(L,4);   //0 为敌友关系，1为NPC settings  2 为 dwid 查找
        //SubWorld[nSubWorld].m_SubWorldID;
	
		lua_newtable(L);                //要给lua脚本返回一个table类型，先要new一个，压入栈顶
/*
		lua_pushnumber(L, 1);           //向Table中压入栈：第一个key值     如 table[key]
		lua_pushnumber(L, 5000);        //将table[key]的值压入栈 如 table[key]=value   ,lua_pushstring
	    lua_settable(L, -3);            //（配对）settable将操作-2，-1编号的键值对，设置到table中，并把key-value从栈中移除

		lua_pushnumber(L, 2); 
		lua_pushnumber(L, 6000);
	    lua_settable(L, -3);

  relation_none	= 1,
  relation_self	= 2,   // 自己
  relation_ally	= 4,   // 同盟
  relation_enemy	= 8,   // 敌人
  relation_dialog	= 16,  //中立对话
  relation_all	= relation_none | relation_ally | relation_enemy | relation_self | relation_dialog,	
  relation_num,
*/
// 检查视野范围内的格子里的NPC
     int nRet=0,nNum=0;
	for (int i = 0; i < nRangeX; ++i)	// i, j由0开始而不是从-range开始是要保证Nearest
	{
		for (int j = 0; j < nRangeY; ++j)
		{
			// 去掉边角几个格子，保证视野是椭圆形
			if ((i * i + j * j) > nRangeX * nRangeX)
				continue;

			// 确定目标格子实际的REGION和坐标确定
			nRMx = nMapX + i;
			nRMy = nMapY + j;
			nSearchRegion = nRegion;
			if (nRMx < 0)
			{
				//nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[2];
				nRMx += SubWorld[nSubWorld].m_nRegionWidth;
			}
			else if (nRMx >= SubWorld[nSubWorld].m_nRegionWidth)
			{
				//nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[6];
				nRMx -= SubWorld[nSubWorld].m_nRegionWidth;
			}
			if (nSearchRegion == -1)
				continue;
			if (nRMy < 0)
			{
				//nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[4];
				nRMy += SubWorld[nSubWorld].m_nRegionHeight;
			}
			else if (nRMy >= SubWorld[nSubWorld].m_nRegionHeight)
			{
				//nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[0];
				nRMy -= SubWorld[nSubWorld].m_nRegionHeight;
			}
			if (nSearchRegion == -1)
				continue;
			// 从REGION的NPC列表中查找满足条件的NPC		返回NPC索引	
			    nRet = SubWorld[nSubWorld].m_Region[nSearchRegion].FindNpc(nRMx, nRMy, nNpcIdx, nRelation,nType);

			if (Npc[nRet].m_Hide.nTime > 0) //隐身的
				nRet = 0;

			if (nRet > 0)
			{
                nNum++;
                lua_pushnumber(L, nNum); 
				lua_pushnumber(L, nRet);
	            lua_settable(L, -3);
				//return nRet;
				continue;
			}
			// 确定目标格子实际的REGION和坐标确定
			nRMx = nMapX - i;
			nRMy = nMapY + j;
			nSearchRegion = nRegion;
			if (nRMx < 0)
			{
				//nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[2];
				nRMx += SubWorld[nSubWorld].m_nRegionWidth;
			}
			else if (nRMx >= SubWorld[nSubWorld].m_nRegionWidth)
			{
				//nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[6];
				nRMx -= SubWorld[nSubWorld].m_nRegionWidth;
			}
			if (nSearchRegion == -1)
				continue;
			if (nRMy < 0)
			{
				//nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[4];
				nRMy += SubWorld[nSubWorld].m_nRegionHeight;
			}
			else if (nRMy >= SubWorld[nSubWorld].m_nRegionHeight)
			{
				//nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[0];
				nRMy -= SubWorld[nSubWorld].m_nRegionHeight;
			}
			if (nSearchRegion == -1)
				continue;
			// 从REGION的NPC列表中查找满足条件的NPC			
			nRet = SubWorld[nSubWorld].m_Region[nSearchRegion].FindNpc(nRMx, nRMy, nNpcIdx, nRelation,nType);
		
			if (Npc[nRet].m_Hide.nTime > 0)
				nRet = 0;
		
			if (nRet > 0)
			{
                nNum++;
                lua_pushnumber(L, nNum); 
				lua_pushnumber(L, nRet);
				lua_settable(L, -3);
				//return nRet;
				continue;
			}

			// 确定目标格子实际的REGION和坐标确定
			nRMx = nMapX - i;
			nRMy = nMapY - j;
			nSearchRegion = nRegion;
			if (nRMx < 0)
			{
				//nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[2];
				nRMx += SubWorld[nSubWorld].m_nRegionWidth;
			}
			else if (nRMx >= SubWorld[nSubWorld].m_nRegionWidth)
			{
				//nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[6];
				nRMx -= SubWorld[nSubWorld].m_nRegionWidth;
			}
			if (nSearchRegion == -1)
				continue;
			if (nRMy < 0)
			{
				//nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[4];
				nRMy += SubWorld[nSubWorld].m_nRegionHeight;
			}
			else if (nRMy >= SubWorld[nSubWorld].m_nRegionHeight)
			{
				//nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[0];
				nRMy -= SubWorld[nSubWorld].m_nRegionHeight;
			}
			if (nSearchRegion == -1)
				continue;
			// 从REGION的NPC列表中查找满足条件的NPC			
			nRet = SubWorld[nSubWorld].m_Region[nSearchRegion].FindNpc(nRMx, nRMy, nNpcIdx, nRelation,nType);

			if (Npc[nRet].m_Hide.nTime > 0)
				nRet = 0;
			
			if (nRet > 0)
			{
                nNum++;
                lua_pushnumber(L, nNum); 
				lua_pushnumber(L, nRet);
				lua_settable(L, -3);
				//return nRet;
				continue;
			}

			// 确定目标格子实际的REGION和坐标确定
			nRMx = nMapX + i;
			nRMy = nMapY - j;
			nSearchRegion = nRegion;			
			if (nRMx < 0)
			{
				//nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[2];
				nRMx += SubWorld[nSubWorld].m_nRegionWidth;
			}
			else if (nRMx >= SubWorld[nSubWorld].m_nRegionWidth)
			{
				//nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[6];
				nRMx -= SubWorld[nSubWorld].m_nRegionWidth;
			}
			if (nSearchRegion == -1)
				continue;
			if (nRMy < 0)
			{
				//nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[4];
				nRMy += SubWorld[nSubWorld].m_nRegionHeight;
			}
			else if (nRMy >= SubWorld[nSubWorld].m_nRegionHeight)
			{
				//nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[0];
				nRMy -= SubWorld[nSubWorld].m_nRegionHeight;
			}
			if (nSearchRegion == -1)
				continue;
			// 从REGION的NPC列表中查找满足条件的NPC
			nRet = SubWorld[nSubWorld].m_Region[nSearchRegion].FindNpc(nRMx, nRMy, nNpcIdx, nRelation,nType);

			if (Npc[nRet].m_Hide.nTime > 0)
				nRet = 0;
			
			if (nRet > 0)
			{
                nNum++;
                lua_pushnumber(L, nNum); //压入第几个key的编号
				lua_pushnumber(L, nRet); //压入KEY值
				lua_settable(L, -3);     //
				//return nRet;
				continue;
			}
		}
	}
	Lua_PushNumber(L, nNum);
    return 2;

}

int LuaGetPos(Lua_State * L)
{
	int nParamNum = Lua_GetTopIndex(L);  

	int nPlayerIndex = GetPlayerIndex(L);

	int nNpcIndexId=0,nPosX = 0,nPosY = 0,npMap=0;

	mapPos nPos;
	ZeroMemory(&nPos,sizeof(mapPos));

	if (nParamNum<=0 && nPlayerIndex > 0)
	{//没有参数	
		Npc[Player[nPlayerIndex].m_nIndex].GetMpsPos(&nPosX, &nPosY,&npMap);
		//Npc[Player[nPlayerIndex].m_nIndex].GetMpsPos(&nPos);
		Lua_PushNumber(L, nPosX);
		Lua_PushNumber(L, nPosY);
		Lua_PushNumber(L, npMap);
		Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_Index); 

	}
	else if (Lua_IsNumber(L, 1))
	{
		nNpcIndexId = (int)Lua_ValueToNumber(L,1);

		if  (nNpcIndexId>0 && nNpcIndexId<MAX_NPC)
		{
			Npc[nNpcIndexId].GetMpsPos(&nPosX, &nPosY,&npMap);
			//Npc[Player[nPlayerIndex].m_nIndex].GetMpsPos(&nPos);
			Lua_PushNumber(L, nPosX);
			Lua_PushNumber(L, nPosY);
			Lua_PushNumber(L,SubWorld[Npc[nNpcIndexId].m_SubWorldIndex].m_SubWorldID); 
			Lua_PushNumber(L, Npc[nNpcIndexId].m_Index);
		}
		else
		{
			Lua_PushNumber(L,0);
			Lua_PushNumber(L,0);
			Lua_PushNumber(L,0); 
			Lua_PushNumber(L,0);
		}
	}
	else 
	{
		Lua_PushNumber(L,0);
		Lua_PushNumber(L,0);
		Lua_PushNumber(L,0); 
		Lua_PushNumber(L,0);
	}

	return 4;
}

int LuaDropItem_map(Lua_State *L)
{

	int nParamNum = Lua_GetTopIndex(L); 

	int _mapIdx = (int)Lua_ValueToNumber(L,1);
	int _map_X = (int)Lua_ValueToNumber(L,2);
	int _map_Y = (int)Lua_ValueToNumber(L,3);
	int _BelongPlayer = (int)Lua_ValueToNumber(L,4);

	int _ItemGenre=(int)Lua_ValueToNumber(L,5);
	int _DetailType =(int)Lua_ValueToNumber(L,6);
	int _ParticulType=(int)Lua_ValueToNumber(L,7);

	int _ItemLevel = (int)Lua_ValueToNumber(L,8);
	int _ItemSeries = (int)Lua_ValueToNumber(L,9);
	int _Luck = (int)Lua_ValueToNumber(L,10);
	int _shuXingLevel=0,_Rate=10,JBlevel[6];
	if (!_ItemSeries)  //如果没有爆率 就不掉出
		return 0;

	if (_mapIdx<0)
		return 0;

	int nTargetSubWorldIdx= g_SubWorldSet.SearchWorld(_mapIdx);  //dwSubWorldID地图编号 返回行数
	if (nTargetSubWorldIdx<0 || nTargetSubWorldIdx>=MAX_SUBWORLD) return 0;

	int i=0;
	for (int i= 0; i <6; ++i)    // 掉下的属性数量
		JBlevel[i]=0;

	if (nParamNum>10)
	{
		_shuXingLevel = (int)Lua_ValueToNumber(L,11);
		_Rate = (int)Lua_ValueToNumber(L,12);
		JBlevel[0] = (int)Lua_ValueToNumber(L,13);
		JBlevel[1] = (int)Lua_ValueToNumber(L,14);
		JBlevel[2] = (int)Lua_ValueToNumber(L,15);
		JBlevel[3] = (int)Lua_ValueToNumber(L,16);
		JBlevel[4] = (int)Lua_ValueToNumber(L,17);
		JBlevel[5] = (int)Lua_ValueToNumber(L,18);
	}

	//if (g_Random(nRate) < g_Random(2000)) //概率检测
	//	return FALSE;
    if (_ItemSeries==-1)
        _ItemSeries=GetRandomNumber(0,4);

	int LuckyRate;
      //g_GameSetTing.GetInteger("ServerConfig","LuckyRate",5000,&LuckyRate);
	LuckyRate = _gameGlobalSet.NpcInfo.nLuckyRate;

	int nGenre, nSeries, nLuck, nDetail, nParticular, nLevel=1,nSLevel=0, pnMagicLevel[6],nQuality,nwMinItemLevel,nwMaxItemLevel,nIsBang,nHour,bYear,bMonth,bDay,bHour,bMin,n_Wlevel;  //Quality=1是黄装 2为白金，3 为紫装 等
	nGenre         = _ItemGenre;
	nDetail          = _DetailType;
	nParticular    = _ParticulType;
	nSeries         = _ItemSeries;//m_Series;
	nLuck            = _Luck;  //玩家的幸运值
	nQuality        = 0;  //是否黄金
    nwMinItemLevel = _ItemLevel;  //物品部分的等级
    nwMaxItemLevel = _ItemLevel;
    nIsBang        = 0;
    nHour            = 0;

	time_t rawtime;
	struct tm * timeinfo;	
	time ( &rawtime );
	timeinfo = localtime ( &rawtime );
	if (nHour>0)
	{
       bYear   =timeinfo->tm_year+1900;
       bMonth  =timeinfo->tm_mon+1;
       bDay    =timeinfo->tm_mday;
       bHour   =timeinfo->tm_hour+nHour;
	   bMin    =timeinfo->tm_min;
	}
	else
	{
        bYear   =0;
		bMonth  =0;
		bDay    =0;
		bHour   =0;
		bMin    =0;
	}

    int nMaxLevel,nMinLevel,mCount=6,nStackNum=1;
/////////////////////////////////////////////////////////////////////////////////////////////////
	if (nwMinItemLevel>0 &&  nwMaxItemLevel>0)
		nLevel = GetRandomNumber(nwMinItemLevel,nwMaxItemLevel);  //物品指定的等级	 
	else
		nLevel = GetRandomNumber(1,7);
/////////////////////////////////////////////////////////////////////
	BOOL	bSkip = FALSE;
	for (int j = 0; j <mCount; ++j)    // 掉下的属性数量
	{
		if (!bSkip)
		{
			 if (g_Random(_Rate + nLuck) >= g_Random(LuckyRate))   // 取余数 幸运+蓝装的爆率  魔法属性的爆率
			 {
//////////////////////////这里的等级影响属性的等级////////////////////////////////////////////////////////////////
				if (_shuXingLevel>0)
                   nSLevel= GetRandomNumber(1,_shuXingLevel);  //物品指定的等级
				else
				{
					int  _curLuck=GetRandomNumber(nLuck/2,nLuck*1.5);
					nSLevel = _curLuck/_gameGlobalSet.NpcInfo.nLuckyRange;
				}

				if  (nSLevel<=0)
					nSLevel =GetRandomNumber(1,_gameGlobalSet.NpcInfo.nLevelRangeCount);
				if  (nSLevel>10)
					nSLevel =GetRandomNumber(7,10);

				pnMagicLevel[j] =nSLevel;  //魔法属性的类型 魔法属性里面的行数  出现
			 }
			 else
			 {
				     pnMagicLevel[j] = 0; //空装备
				     bSkip = TRUE;
			 }
		}
		else
			//if (nwMinItemLevel>0 &&  nwMaxItemLevel>0)//物品指定的等级	 
			//	nLevel = GetRandomNumber(nwMinItemLevel,nwMaxItemLevel);  
			pnMagicLevel[j] = 0;  //空属性
	}

	int nIdx=0;

	if (nLevel<=0)
        nLevel = 1;

	if (nQuality==0)
         nIdx = ItemSet.AddOther(nGenre,nSeries, nLevel,nLuck, nDetail,\
	        nParticular, pnMagicLevel, g_SubWorldSet.GetGameVersion(),\
			0,nStackNum,0,0,bYear,bMonth,bDay,bHour,bMin,0,NULL,JBlevel,NULL,0);
//黄装的产生
	else if(nQuality==1)
	{
		for (int i=0;i<6;++i)
			pnMagicLevel[i]=JBlevel[i];

	     nIdx = ItemSet.AddGold(nDetail,pnMagicLevel,nSeries,0,0,
	                                bYear,bMonth,bDay,bHour,bMin,
						                  NULL,NULL,nLevel,nLuck);
	}
	
	if (nIdx <= 0)
		return 0;

	if (nIsBang)
		Item[nIdx].SetBang(nIsBang);

	POINT	ptLocal;
	KMapPos	Pos;
	ptLocal.x = _map_X;
	ptLocal.y = _map_Y;
	SubWorld[nTargetSubWorldIdx].GetFreeObjPos(ptLocal);
	Pos.nSubWorld = nTargetSubWorldIdx;
	SubWorld[nTargetSubWorldIdx].Mps2Map(ptLocal.x, ptLocal.y, 
		&Pos.nRegion, &Pos.nMapX, &Pos.nMapY, 
		&Pos.nOffX, &Pos.nOffY);

	int nObj;
	//////掉下装备保存在KObjItemInfo//////结构体中的参数，以便调用
	KObjItemInfo sInfo;
    memset(&sInfo, 0, sizeof(sInfo));
	sInfo.m_nItemID = nIdx;
	sInfo.m_nItemWidth = Item[nIdx].GetWidth();
	sInfo.m_nItemHeight = Item[nIdx].GetHeight();
	sInfo.m_nMoneyNum = 0;
	snprintf(sInfo.m_szName, sizeof(sInfo.m_szName), "%s", Item[nIdx].GetName()); //物品的名称
	sInfo.m_nColorID = 0;
	sInfo.m_nMovieFlag = 1;
	sInfo.m_nSoundFlag = 1;
    sInfo.m_sHaveAttack=0;   //是否设置为攻击无效了
	//sInfo.m_AttackerDwid=0;   //上次攻击着的DWID

	nObj = ObjSet.Add(Item[nIdx].GetObjIdx(), Pos, sInfo);
	if (nObj == -1)
		ItemSet.Remove(nIdx);
	else
	{
		if (_BelongPlayer > 0)
			Object[nObj].SetItemBelong(_BelongPlayer);
		else
			Object[nObj].SetItemBelong(-1);
	}

	Lua_PushNumber(L,nIdx);
	return 1;
}

///怪物掉下装备
int LuaDropItem(Lua_State *L)
{
   int nBelongPlayer = (int)Lua_ValueToNumber(L,1);
   int nDeathNpcIdx = (int)Lua_ValueToNumber(L,2);
   int nItemGenre = (int)Lua_ValueToNumber(L,3);
   int nDetailType = (int)Lua_ValueToNumber(L,4);
   int nParticulType = (int)Lua_ValueToNumber(L,5);
   int nRate = (int)Lua_ValueToNumber(L,6);
   int nItemMinLevel = (int)Lua_ValueToNumber(L,7);
   int nItemMaxLevel = (int)Lua_ValueToNumber(L,8);
   int JBlevel[7];
   JBlevel[0] = (int)Lua_ValueToNumber(L,9);
   JBlevel[1] = (int)Lua_ValueToNumber(L,10);
   JBlevel[2] = (int)Lua_ValueToNumber(L,11);
   JBlevel[3] = (int)Lua_ValueToNumber(L,12);
   JBlevel[4] = (int)Lua_ValueToNumber(L,13);
   JBlevel[5] = (int)Lua_ValueToNumber(L,14);
   JBlevel[6]=0;
   int nItemSeries = (int)Lua_ValueToNumber(L,15);
   int nStackNum   = (int)Lua_ValueToNumber(L,16);

   //int nMgCount = (int)Lua_ValueToNumber(L,17);
   int nMgLevel = (int)Lua_ValueToNumber(L,17);
   int IsBang = (int)Lua_ValueToNumber(L,18);
   int IsHour = (int)Lua_ValueToNumber(L,19);
   int IsQuality = (int)Lua_ValueToNumber(L,20);

   if  (nDeathNpcIdx<0 || nDeathNpcIdx>=MAX_NPC)
       return 0;

   int nRegn=Npc[nDeathNpcIdx].DropSingleItem(nBelongPlayer,nItemGenre,nDetailType,nParticulType,\
	                                              nRate,nItemMinLevel,nItemMaxLevel, nItemSeries,nStackNum,JBlevel,\
		                                           nMgLevel,IsBang,IsHour,IsQuality);
	Lua_PushNumber(L,nRegn);
	return 1;
}

//设定使用时间
int LuaIsHaveUseTime(Lua_State *L)
{
    int nItemIdx = (int)Lua_ValueToNumber(L,1);

	int	bYear=0,bMonth=0,bDay=0,bHour=0,bMin=0;
	
	   if (nItemIdx < 0 || nItemIdx >= MAX_ITEM) { Lua_PushNumber(L,0); return 1; }
	   bYear   = Item[nItemIdx].GetTime()->bYear;
	   bMonth  = Item[nItemIdx].GetTime()->bMonth;
	   bDay    = Item[nItemIdx].GetTime()->bDay;
	   bHour   = Item[nItemIdx].GetTime()->bHour;
	   bMin    = Item[nItemIdx].GetTime()->bMin;
	   
	   //Item[nItemIdx].SetTime(bYear,bMonth,bDay,bHour,bMin);
	   Lua_PushNumber(L,bYear);
	   return 1;
}

int  LuaGetItemMapIdx(Lua_State *L)
{
	int nItemIdx = (int)Lua_ValueToNumber(L,1);
	
	if (nItemIdx < 0 || nItemIdx >= MAX_ITEM) { Lua_PushNumber(L ,0); Lua_PushNumber(L, 0); return 2; }
	Lua_PushNumber(L,Item[nItemIdx].GetUseMapIdx());
	Lua_PushNumber(L,Item[nItemIdx].GetUseKind());
    return 2;
}

int  LuaSetItemMapIdx(Lua_State *L)
{
	int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex<=0) return 0;

	int nItemIdx = (int)Lua_ValueToNumber(L,1);
	int nMapIdx  = (int)Lua_ValueToNumber(L,2);
	int nUseKind = (int)Lua_ValueToNumber(L,3);

	if (nItemIdx < 0 || nItemIdx >= MAX_ITEM) return 0;
	Item[nItemIdx].SetUseKind(nUseKind);
	Item[nItemIdx].SetUseMapIdx(nMapIdx);

	Player[nPlayerIndex].m_ItemList.SynPlayerItem(nItemIdx);

    return 0;
}

//设定使用时间
int LuaAddItemUseTime(Lua_State *L)
{
    int nItemIdx = (int)Lua_ValueToNumber(L,1);
	int nYear    = (int)Lua_ValueToNumber(L,2);
	int nMon     = (int)Lua_ValueToNumber(L,3);
	int nDay     = (int)Lua_ValueToNumber(L,4);
	int nHour    = (int)Lua_ValueToNumber(L,5);
	int nMin     = (int)Lua_ValueToNumber(L,6);

	int nPlayerIndex = GetPlayerIndex(L);

    if (nPlayerIndex<=0) return 0;

	int	bYear=0,bMonth=0,bDay=0,bHour=0,bMin=0;//到期的时间
	int	byYear=0,byMonth=0,byDay=0,byHour=0,byMin=0;//当前的本地时间

	   time_t rawtime;                  //定义一个long 型存放秒数
	   struct tm * timeinfo;	
	   time (&rawtime);                 //距离现在的时间（秒）
	   timeinfo = localtime(&rawtime);  //把从1970-1-1零点零分到当前时间系统所偏移的秒数时间转换为本地时间
       byYear   = timeinfo->tm_year+1900;
	   byMonth  = timeinfo->tm_mon+1;	//当月
	   byDay    = timeinfo->tm_mday;
	   byHour   = timeinfo->tm_hour;
	   byMin    = timeinfo->tm_min;
	   //----------物品的到期时间---------------
	   if (nItemIdx < 0 || nItemIdx >= MAX_ITEM) return 0;
	   bYear   = Item[nItemIdx].GetTime()->bYear;
	   bMonth  = Item[nItemIdx].GetTime()->bMonth;
	   bDay    = Item[nItemIdx].GetTime()->bDay;
	   bHour   = Item[nItemIdx].GetTime()->bHour;
	   bMin    = Item[nItemIdx].GetTime()->bMin;

       if (Player[nPlayerIndex].m_ItemList.CheckIsOutTime(nItemIdx))
	   {//如果是过期的,设置物品的时间为 当前时间
		   bYear=byYear;
		   bMonth=byMonth;
		   bDay=byDay;
		   bHour=byHour;
		   bMin=byMin;
	   }

         bYear +=nYear;
		 bMonth +=nMon;
		 bDay +=nDay;
		 bHour +=nHour;
		 bMin +=nMin;
	   
	   Item[nItemIdx].SetTime(bYear,bMonth,bDay,bHour,bMin);



       if (!Player[nPlayerIndex].m_ItemList.CheckIsOutTime(nItemIdx))
	   {//没过期的 就设置同步

	      	 Item[nItemIdx].SetIsCanUse(1);   
	         Player[nPlayerIndex].m_ItemList.SynPlayerItem(nItemIdx); 
	   }
     
/*
       if (Item[nItemIdx].GetTime()->bYear)
		   if (Item[nItemIdx].GetTime()->bYear > byYear)
		   {
			   Item[nItemIdx].SetIsCanUse(1);   
			   if (nPlayerIndex>0)
	               Player[nPlayerIndex].m_ItemList.SynPlayerItem(nItemIdx);                 //同步物品属性--客户端
		   }
		   else if (Item[nItemIdx].GetTime()->bYear == byYear)
			   if (Item[nItemIdx].GetTime()->bMonth > byMonth)
			   {
				   Item[nItemIdx].SetIsCanUse(1);			   
				    if (nPlayerIndex>0)
	                Player[nPlayerIndex].m_ItemList.SynPlayerItem(nItemIdx);           //同步物品属性--客户端
			   }
			   else if (Item[nItemIdx].GetTime()->bMonth == byMonth)
				   if (Item[nItemIdx].GetTime()->bDay > byDay)
				   {
					   Item[nItemIdx].SetIsCanUse(1);
					   if (nPlayerIndex>0)
	                    Player[nPlayerIndex].m_ItemList.SynPlayerItem(nItemIdx);        //同步物品属性--客户端
				   }
				   else if (Item[nItemIdx].GetTime()->bDay == byDay)
					   if (Item[nItemIdx].GetTime()->bHour > byHour)
					   {
						   Item[nItemIdx].SetIsCanUse(1);
						   if (nPlayerIndex>0)
	                          Player[nPlayerIndex].m_ItemList.SynPlayerItem(nItemIdx);   //同步物品属性--客户端
					   }
					   else if (Item[nItemIdx].GetTime()->bHour == byHour) 
						   if (Item[nItemIdx].GetTime()->bMin > byMin)
						   {
							   Item[nItemIdx].SetIsCanUse(1);
							   if (nPlayerIndex>0)
	                              Player[nPlayerIndex].m_ItemList.SynPlayerItem(nItemIdx); //同步物品属性--客户端
						   } 
	   */
   return 0;
}

//设定使用时间
int LuaSetItemUseTime(Lua_State *L) 
{
    int nItemIdx = (int)Lua_ValueToNumber(L,1);
	int nYear    = (int)Lua_ValueToNumber(L,2);
	int nMon     = (int)Lua_ValueToNumber(L,3);
	int nDay     = (int)Lua_ValueToNumber(L,4);
	int nHour    = (int)Lua_ValueToNumber(L,5);
	int nMin     = (int)Lua_ValueToNumber(L,6);

	time_t rawtime;                  //定义一个long 型存放秒数
	struct tm * timeinfo;	
	time (&rawtime);                 //距离现在的时间（秒）
	timeinfo = localtime(&rawtime);  //把从1970-1-1零点零分到当前时间系统所偏移的秒数时间转换为本地时间
	int	bYear=0,bMonth=0,bDay=0,bHour=0,bMin=0;

	   bYear   = timeinfo->tm_year+1900+nYear;
	   bMonth  = timeinfo->tm_mon+1+nMon;
	   bDay    = timeinfo->tm_mday+nDay;
	   bHour   = timeinfo->tm_hour+nHour;
	   bMin    = timeinfo->tm_min+nMin;

	if (nItemIdx < 0 || nItemIdx >= MAX_ITEM) return 0;
	Item[nItemIdx].SetTime(bYear,bMonth,bDay,bHour,bMin);

	return 0;
}

int LuaSetItemUseTimeNew(Lua_State *L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;

	int nItemIdx = (int)Lua_ValueToNumber(L,1);
	int totalMinutes = (int)Lua_ValueToNumber(L,2);

	time_t rawtime;                  //定义一个long 型存放秒数
	struct tm * timeinfo;	
	time (&rawtime);                 //距离现在的时间（秒）
	timeinfo = localtime(&rawtime);  //把从1970-1-1零点零分到当前时间系统所偏移的秒数时间转换为本地时间

	int bYear   = timeinfo->tm_year + 1900;
	int bMonth  = timeinfo->tm_mon + 1;
	int bDay    = timeinfo->tm_mday;
	int bHour   = timeinfo->tm_hour;
	int bMin    = timeinfo->tm_min + totalMinutes;

	//printf("bYear: %d, bMonth: %d, bDay: %d, bHour: %d, bMin: %d\n",bYear, bMonth, bDay, bHour, bMin);
	if (nItemIdx < 0 || nItemIdx >= MAX_ITEM) return 0;
	Item[nItemIdx].SetTime(bYear, bMonth, bDay, bHour, bMin);
	Player[nPlayerIndex].m_ItemList.SynPlayerItem(nItemIdx);
	return 0;
}

int LuaLockItem(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2) return 0;
	
	int nIdx = (int)Lua_ValueToNumber(L, 1);
	int nLock = LOCK_STATE_FOREVER;
	if (nIdx > 0 && nIdx < MAX_ITEM)
	{
		if (nParamNum > 2)
			nLock = (int)Lua_ValueToNumber(L, 2);
		Item[nIdx].LockItem(nLock);
		Player[nPlayerIndex].m_ItemList.SynPlayerItem(nIdx);
		Lua_PushNumber(L,nIdx);
		return 1;
	}
	return 0;
}

int LuaGetLockItem(Lua_State * L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 1) return 0;
	
	int nIdx = (int)Lua_ValueToNumber(L, 1);
	if (nIdx > 0 && nIdx < MAX_ITEM)
	{
		Lua_PushNumber(L, Item[nIdx].GetLockItem()->nState);
		if (KSG_GetCurSec() < Item[nIdx].GetLockItem()->dwLockTime)
			Lua_PushNumber(L, Item[nIdx].GetLockItem()->dwLockTime - KSG_GetCurSec());
		else
			Lua_PushNumber(L, Item[nIdx].GetLockItem()->dwLockTime);
		return 2;
	}
	Lua_PushNumber(L, 0);
	Lua_PushNumber(L, 0);
	return 2;
}

int LuaLockItemBind(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2) return 0;
	
	int nIdx = (int)Lua_ValueToNumber(L, 1);
	int nLock = (int)Lua_ValueToNumber(L, 2);
	if (nIdx > 0)
	{
        //printf("LockItem: nIdx: %d, nLock: %d\n", nIdx, nLock);
        int result = Player[nPlayerIndex].m_ItemList.LockItemBind(nIdx, nLock);
        //printf("LockItemBind Result: %d\n", result); 
        Lua_PushNumber(L, result); 
		return 1;
	}
	return 0;
}

int LuaGetNameItem(Lua_State * L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 1) return 0;
	
	int nIdx = (int)Lua_ValueToNumber(L, 1);
	if (nIdx > 0 && nIdx < MAX_ITEM)
	{
		Lua_PushString(L, Item[nIdx].GetName());
		return 1;
	}
	return 0;
}

int LuaGetSourceTime(Lua_State * L)
{
	time_t now = time(NULL); 
	Lua_PushNumber(L, (double)now); 
	return 1; 
}

int LuaGetCurrentTime(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 3)
		return 0; 

	int year  = (int)Lua_ValueToNumber(L, 1);
	int month = (int)Lua_ValueToNumber(L, 2);
	int day   = (int)Lua_ValueToNumber(L, 3);
	int hour  = (int)Lua_ValueToNumber(L, 4);
	int min   = (int)Lua_ValueToNumber(L, 5);
	int sec   = (int)Lua_ValueToNumber(L, 6);

	struct tm timeinfo;
	timeinfo.tm_year = year - 1900; 
	timeinfo.tm_mon  = month - 1;   
	timeinfo.tm_mday = day;
	timeinfo.tm_hour = hour;
	timeinfo.tm_min  = min;
	timeinfo.tm_sec  = sec;
	timeinfo.tm_isdst = -1;

	time_t timestamp = mktime(&timeinfo);
	if (timestamp == -1)
		timestamp = 0;

	Lua_PushNumber(L, (double)timestamp);
	return 1;
}

int LuaSetNumber(Lua_State * L)
{
	int nResult = 0;
	if (Lua_GetTopIndex(L) < 4)
		goto lab_setnumber;

	nResult = KSG_StringSetValue((int)Lua_ValueToNumber(L,1),
		(int)Lua_ValueToNumber(L,2),
		(int)Lua_ValueToNumber(L,3),
		(int)Lua_ValueToNumber(L,4));
	
lab_setnumber:
	Lua_PushNumber(L, nResult);
	return 1;
}

int LuaGetNumber(Lua_State * L)
{
	int nResult = 0;
	if (Lua_GetTopIndex(L) < 3)
		goto lab_getnumber;
	nResult = KSG_StringGetValue((int)Lua_ValueToNumber(L,1),
		(int)Lua_ValueToNumber(L,2),
		(int)Lua_ValueToNumber(L,3));

lab_getnumber:
	Lua_PushNumber(L, nResult);
	return 1;
}


int LuaCheckRoom(Lua_State * L)
{
	int nPlayerIndex;

#ifdef _SERVER
	   nPlayerIndex   = GetPlayerIndex(L);
#else
       nPlayerIndex   = CLIENT_PLAYER_INDEX;
#endif

	int nWpos  = (int)Lua_ValueToNumber(L,1);
	int nHpos  = (int)Lua_ValueToNumber(L,2);
    int nCount = 0;
	int nRoomKind = room_equipment;

	if  (nRoomKind<room_equipment || nRoomKind>=room_num)
		 nRoomKind = room_equipment;

    if (nPlayerIndex <= 0)
    {
		nPlayerIndex = (int)Lua_ValueToNumber(L,3);
		if (nPlayerIndex <= 0)
		{
			Lua_PushNumber(L, nCount);
			return 1;
		}
	}

	if (nPlayerIndex < 0 || nPlayerIndex >= MAX_PLAYER) { Lua_PushNumber(L,0); return 1; }
	nCount = Player[nPlayerIndex].m_ItemList.GetFreePositionCount(nWpos,nHpos,nRoomKind);

	Lua_PushNumber(L, nCount);
    return 1;
}



//用于脚本掉落
int LuaITEM_DropRateItem(Lua_State *L)
{

	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex<0)	return 0;
	int nDeathNpcIdx  = (int)Lua_ValueToNumber(L,1);
	int nBaoliua  = (int)Lua_ValueToNumber(L,2);
	char *nDropFile  = (char *)Lua_ValueToString(L,3);
	int nBaoliub  = (int)Lua_ValueToNumber(L,4);
	int nItemNum  = (int)Lua_ValueToNumber(L,5);
	int nSeries  = (int)Lua_ValueToNumber(L,6);

	if  (nDeathNpcIdx<=0 || nDropFile==NULL || nDeathNpcIdx >= MAX_NPC)
		return 0;
	
	int nReg=0;

	/*if (g_pNpcTemplate[Npc[nDeathNpcIdx].m_NpcSettingIdx][0])  //如果没有刷过这个怪物
		nReg=g_pNpcTemplate[Npc[nDeathNpcIdx].m_NpcSettingIdx][0]->InitDropRate(nDeathNpcIdx,nDropFile);
	else if (g_pNpcTemplate[Npc[nDeathNpcIdx].m_NpcSettingIdx][Npc[nDeathNpcIdx].m_Level])
		nReg=g_pNpcTemplate[Npc[nDeathNpcIdx].m_NpcSettingIdx][Npc[nDeathNpcIdx].m_Level]->InitDropRate(nDeathNpcIdx,nDropFile);
	 */
	char nstrDropFile[256]={0};
	snprintf(nstrDropFile, sizeof(nstrDropFile), "%s", nDropFile);
	KNpcTemplate* pNpcTemp = NULL;
	if (pNpcTemp = NpcSet.GetTemplate(Npc[nDeathNpcIdx].m_NpcSettingIdx, 0))
		nReg=pNpcTemp->InitDropRate(nDeathNpcIdx,nstrDropFile);
	else  if (pNpcTemp = NpcSet.GetTemplate(Npc[nDeathNpcIdx].m_NpcSettingIdx, Npc[nDeathNpcIdx].m_Level))
		nReg=pNpcTemp->InitDropRate(nDeathNpcIdx,nstrDropFile);
	else
	{
		printf("----加载 NPC名称:%s,等级:%d,爆率:%s,失败----\n",Npc[nDeathNpcIdx].Name,Npc[nDeathNpcIdx].m_Level,nstrDropFile);
	}

	if (nReg)
	   Npc[nDeathNpcIdx].ScriptDropItem(nPlayerIndex,Player[nPlayerIndex].m_nIndex,nItemNum,nSeries);

  return 0;
}

///怪物掉下辨识熔炼装备
int LuaDropEquipItem(Lua_State *L)
{
	int nBelongPlayer = (int)Lua_ValueToNumber(L,1);
	int nDeathNpcIdx  = (int)Lua_ValueToNumber(L,2);
	int nItemGenre    = (int)Lua_ValueToNumber(L,3);
	int nDetailType   = (int)Lua_ValueToNumber(L,4);
	int nParticulType = (int)Lua_ValueToNumber(L,5);
	int nRate         = (int)Lua_ValueToNumber(L,6);
	int nItemMinLevel = (int)Lua_ValueToNumber(L,7);
	int nItemMaxLevel = (int)Lua_ValueToNumber(L,8);
	int nItemSeries   = (int)Lua_ValueToNumber(L,9);
    int nRpiont       = (int)Lua_ValueToNumber(L,10);
	int nIsWhere      = (int)Lua_ValueToNumber(L,11);
	int nMgCount      = (int)Lua_ValueToNumber(L,12);
	int nMgLevel      = (int)Lua_ValueToNumber(L,13);
	int IsBang        = (int)Lua_ValueToNumber(L,14);
	int IsHour        = (int)Lua_ValueToNumber(L,15);
	int IsMin         = (int)Lua_ValueToNumber(L,16);
	int IsQuality     = (int)Lua_ValueToNumber(L,17);

	if (nDeathNpcIdx < 0 || nDeathNpcIdx >= MAX_NPC) { Lua_PushNumber(L,0); return 1; }
	int nRegn = Npc[nDeathNpcIdx].DropBianShiItem(nBelongPlayer,nItemGenre,\
		                   nDetailType,nParticulType,nRate,nItemMinLevel,\
		                      nItemMaxLevel,nItemSeries,nRpiont,nIsWhere,\
							   nMgCount,nMgLevel,IsBang,IsHour,IsMin,IsQuality);
	Lua_PushNumber(L,nRegn);
	return 1;
}

/*掉下DropItem(NpcId, 物品id或物品名) */
/*AddEventItem(id)*/
KTabFile g_EventItemTab;

int LuaAddEventItem(Lua_State *L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum< 1)
		return 0;
	
	int nPlayerIndex = GetPlayerIndex(L);
	
	if (nPlayerIndex <= 0)
	{
		Lua_PushNumber(L,0);
		return 1;
	}

	int nEventId = 0;
	int nYear = 0,nMonth = 0,nDay = 0,nHour = 0,nMin = 0;
	char * nGuiShuZhe=NULL;

	if (Lua_IsNumber(L, 1))
	{
		nEventId = (int)Lua_ValueToNumber(L,1);
	}
	else
	{
		char * szEventItm = (char *)Lua_ValueToString(L,1);
		
		if (!g_EventItemTab.GetInteger(szEventItm, "具体类别", 0, &nEventId))
		{
			Lua_PushNumber(L,0);
			return 1;
		}
	}
   
   int nLevel = 1;

   //if (nParamNum > 1)  
   //    nLevel = (int)Lua_ValueToNumber(L,2);
   if (Lua_IsNumber(L, 2))
   {
	   nLevel = (int)Lua_ValueToNumber(L,2);
   } 

   if (nParamNum >= 7)
   {  
		nYear = (int)Lua_ValueToNumber(L,3);
		nMonth = (int)Lua_ValueToNumber(L,4);
		nDay = (int)Lua_ValueToNumber(L,5);
		nHour =(int)Lua_ValueToNumber(L,6);
		nMin =(int)Lua_ValueToNumber(L,7);
	}
	
	int nIndex = ItemSet.AddOther(item_task,0,nLevel,0,nEventId,0,0,0,0,1,0,0,nYear,nMonth,nDay,nHour,nMin);
	
	if (nIndex <= 0) 
	{
		Lua_PushNumber(L,0);
		return 1;
	}

    int nStackNum =Item[nIndex].GetStackNum();

	if (nStackNum<=0)
	    nStackNum=1;
//---------------------------------------------------开始检测包袱是否有这个物品 是否能叠加
/* if (Item[nIndex].GetGenre()!=0 && !Item[nIndex].GetIsBang() && Item[nIndex].GetTime()->bYear<=0)
			 {//不是装备的话,包袱里是否有这个物品
				 int nIdx=Player[nPlayerIndex].m_ItemList.IsHaveItem(Item[nIndex].GetGenre(),\
					                                      Item[nIndex].GetDetailType(),\
														  Item[nIndex].GetParticular(),\
														  Item[nIndex].GetSeries(),\
													      Item[nIndex].GetLevel(),pos_equiproom);
               
				if (nIdx>0 && Item[nIdx].IsStack() && Item[nIndex].CanStack(nIdx) && (Item[nIdx].GetStackNum()+nStackNum) <= MAX_STACKNUM) //是否可以叠加--手里和包袱里的是否相同
				{//包袱里的东西可以叠加
                    //Item[nIdx].SetStackNum(Item[nIdx].GetStackNum()+StackNum);             //设置叠加数量
                    //m_ItemList.SynPlayerItem(nIdx);                                        //同步到客户端
					 //xep chong item
                    nStackNum=Item[nIdx].GetStackNum()+nStackNum;

					Player[nPlayerIndex].m_ItemList.Remove(nIdx);
					ItemSet.Remove(nIdx);
				}
			}  */
                  
//---------------------------------------------------
    /*if (Player[nPlayerIndex].m_ItemList.GetPlayerItemCount()>=MAX_ITEMLIST_CONUT)
	{
	  ItemSet.Remove(nIndex);
	  Player[nPlayerIndex].m_ItemList.msgshow("提示:<color=yellow>物品超限,请清理储物空间<color>!");
	  Lua_PushNumber(L,0);
	  return 1;
	}*/

	int		x, y;

	if (Player[nPlayerIndex].m_ItemList.CheckCanPlaceInEquipment(Item[nIndex].GetWidth(),Item[nIndex].GetHeight(),&x,&y))
	{
		Player[nPlayerIndex].m_ItemList.Add(nIndex,pos_equiproom,x,y,0,0,nStackNum);
	}
	else
	{
		
		Player[nPlayerIndex].m_ItemList.msgshow("Hanh trang khong du khoang trong tu dong huy vat pham!");
		int	nIdx = Player[nPlayerIndex].m_ItemList.Hand();
		if (nIdx)
		{
			Player[nPlayerIndex].m_ItemList.Remove(nIdx);
			ItemSet.Remove(nIdx);
		   	
		/*	KMapPos			sMapPos;
			KObjItemInfo	sInfo;
			
			Player[nPlayerIndex].GetAboutPos(&sMapPos);
			
			sInfo.m_nItemID = nIdx;
			sInfo.m_nItemWidth = Item[nIdx].GetWidth();
			sInfo.m_nItemHeight = Item[nIdx].GetHeight();
			sInfo.m_nMoneyNum = 0;
			strcpy(sInfo.m_szName, Item[nIdx].GetName());

			sInfo.m_nColorID = 0;  //颜色编号
			sInfo.m_nMovieFlag = 1;
			sInfo.m_nSoundFlag = 1;
			sInfo.m_sHaveAttack=0;   //是否设置为攻击无效了
	        //sInfo.m_AttackerDwid=0;   //上次攻击着的DWID
			
			int nObj = ObjSet.Add(Item[nIdx].GetObjIdx(), sMapPos, sInfo);
			if (nObj >= 0)
			{
				if (Item[nIdx].GetGenre() == item_task)
				{
					Object[nObj].SetEntireBelong(nPlayerIndex);
				}
				else
				{
					Object[nObj].SetItemBelong(nPlayerIndex);
				}
			}*/
		}

		Player[nPlayerIndex].m_ItemList.Remove(nIndex);
		ItemSet.Remove(nIndex);
		//Player[nPlayerIndex].m_ItemList.Add(nIndex,pos_hand,0,0,0,0,nStackNum);
		Lua_PushNumber(L,0);
		return 1;
	}	
	
	Lua_PushNumber(L, nIndex);
	return 1;
}
//重载欢迎语
int LuaSendWelcomeMsg(Lua_State * L)
{
    int nPlayerIndex = GetPlayerIndex(L);
	PlayerSet.ReloadWelcomeMsg();
	return 0;
}

//设置物品说明
int LuaSyncItemInfo(Lua_State * L)
{
	char *Info=NULL;
	int nItmeIndex = (int)Lua_ValueToNumber(L, 1);
//	Info =(char *)Lua_ValueToString(L,2);  //第几个属性的值
	int nVael= (int)Lua_ValueToNumber(L, 2);
	if (nItmeIndex < 0 || nItmeIndex >= MAX_ITEM) return 0;
	Item[nItmeIndex].SetItemInfo(nVael);//
	return 0;
}

int LuaSetChouJiangCishu(Lua_State * L)

{
  int nPlayerIndex = GetPlayerIndex(L);
  int nCjcs = (int)Lua_ValueToNumber(L, 1);
  int nModel= (int)Lua_ValueToNumber(L, 2);
  if (nModel==1)
     Npc[Player[nPlayerIndex].m_nIndex].m_CJtaskID=nCjcs;
  else if (nModel>=2)
     Npc[Player[nPlayerIndex].m_nIndex].m_CJtaskID +=nCjcs;

  return 0;
}

///强化
int LuaItemQiangHua(Lua_State * L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    int nItmeIndexA = (int)Lua_ValueToNumber(L, 1);
    int nItmeIndexB= (int)Lua_ValueToNumber(L, 2);
    int nRank= (int)Lua_ValueToNumber(L, 3);
    int nReg=0;
        nReg=Player[nPlayerIndex].m_ItemList.QiangHua(nItmeIndexA,nItmeIndexB,nRank);
    Lua_PushNumber(L,nReg);
    return 1;
}

//熔炼
int LuaItemRongLian(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	int nItmeIndexA = (int)Lua_ValueToNumber(L, 1);
	int nItmeIndexB= (int)Lua_ValueToNumber(L, 2);
	int nReg=0;
	    nReg=Player[nPlayerIndex].m_ItemList.RongLian(nItmeIndexA,nItmeIndexB);
	Lua_PushNumber(L,nReg);
	return 1;
}
//纹钢
int LuaItemWenGang(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	int nItmeIndexA = (int)Lua_ValueToNumber(L, 1);
	int nItmeIndexB= (int)Lua_ValueToNumber(L, 2);
	int nReg=0;
	nReg=Player[nPlayerIndex].m_ItemList.WenGang(nItmeIndexA,nItmeIndexB);
	Lua_PushNumber(L,nReg);
	return 1;
}
//检测蓝装属性等级与提炼晶石的品质是否相等
int LuaChcekBulePinAndLevel(Lua_State * L)
{
	int nItmeIndexA   = (int)Lua_ValueToNumber(L, 1);
	int nItmeIndexB   = (int)Lua_ValueToNumber(L, 2);
	int nPos          = (int)Lua_ValueToNumber(L, 3);
    if (nItmeIndexA < 0 || nItmeIndexA >= MAX_ITEM) { Lua_PushNumber(L,0); return 1; }
    int nMagType      =Item[nItmeIndexA].GetAttribType(nPos);  //魔法属性ID
    if (nItmeIndexA < 0 || nItmeIndexA >= MAX_ITEM) { Lua_PushNumber(L,0); return 1; }
    int nRval         =Item[nItmeIndexA].GetAttribTypeVala(nPos);
	if (nItmeIndexA < 0 || nItmeIndexA >= MAX_ITEM) { Lua_PushNumber(L,0); return 1; }
	int nWenPin       =Item[nItmeIndexA].CheckBluePin(nMagType,nRval);
    if (nItmeIndexB < 0 || nItmeIndexB >= MAX_ITEM) { Lua_PushNumber(L,0); return 1; }
    int nItemPin      =Item[nItmeIndexB].GetLevel();   //提炼晶石的等级
	int nReg=0;
	if (nWenPin==nItemPin)
		nReg=1;
	
	Lua_PushNumber(L,nReg);
   
	return 1;
}

//检测纹钢与提炼晶石的品质是否相等
int LuaChcekWenPinAndLevel(Lua_State * L)
{
	int nItmeIndexA   = (int)Lua_ValueToNumber(L, 1);
	int nItmeIndexB   = (int)Lua_ValueToNumber(L, 2);
	int nPos          = (int)Lua_ValueToNumber(L, 3);
    if (nItmeIndexA < 0 || nItmeIndexA >= MAX_ITEM) { Lua_PushNumber(L,0); return 1; }
    int nMagType      =Item[nItmeIndexA].GetRongAttribType(nPos);  //魔法属性ID
    if (nItmeIndexA < 0 || nItmeIndexA >= MAX_ITEM) { Lua_PushNumber(L,0); return 1; }
    int nRval         =Item[nItmeIndexA].GetRongTypeVal(nPos);
	if (nItmeIndexA < 0 || nItmeIndexA >= MAX_ITEM) { Lua_PushNumber(L,0); return 1; }
	int nWenPin       =Item[nItmeIndexA].CheckWenPin(nMagType,nRval);
    if (nItmeIndexB < 0 || nItmeIndexB >= MAX_ITEM) { Lua_PushNumber(L,0); return 1; }
    int nItemPin      =Item[nItmeIndexB].GetLevel();   //提炼晶石的等级
	int nReg=0;
	if (nWenPin==nItemPin)
       nReg=1;

	Lua_PushNumber(L,nReg);
	return 1;
}
//给纹钢
int LuaGiveQuestItem(Lua_State * L)
{
	int nItmeIndexA   = (int)Lua_ValueToNumber(L, 1);
	int nItmeIndexB   = (int)Lua_ValueToNumber(L, 2);
	int nPos          = (int)Lua_ValueToNumber(L, 3);
	int nModel         = (int)Lua_ValueToNumber(L, 4);
	int nMagType;
	if  (nModel==1)
	{
		if (nItmeIndexA < 0 || nItmeIndexA >= MAX_ITEM) { Lua_PushNumber(L,0); return 1; }
		nMagType      = Item[nItmeIndexA].GetAttribType(nPos);  //魔法属性ID
        if (nItmeIndexA < 0 || nItmeIndexA >= MAX_ITEM) { Lua_PushNumber(L,0); return 1; }
        int nRval     = Item[nItmeIndexA].GetAttribTypeVala(nPos);
	}
	else
	{
        if (nItmeIndexA < 0 || nItmeIndexA >= MAX_ITEM) { Lua_PushNumber(L,0); return 1; }
        nMagType      = Item[nItmeIndexA].GetRongAttribType(nPos);  //魔法属性ID
        if (nItmeIndexA < 0 || nItmeIndexA >= MAX_ITEM) { Lua_PushNumber(L,0); return 1; }
        int nRval     = Item[nItmeIndexA].GetRongTypeVal(nPos);
//	int nWenPin       = Item[nItmeIndexA].CheckWenPin(nMagType,nRval);
	}
       if (nItmeIndexB < 0 || nItmeIndexB >= MAX_ITEM) { Lua_PushNumber(L,0); return 1; }
       int nItemPin      = Item[nItmeIndexB].GetLevel();  //提炼晶石的等级
	   if (nItmeIndexB < 0 || nItmeIndexB >= MAX_ITEM) { Lua_PushNumber(L,0); return 1; }
	   int nRows         = Item[nItmeIndexB].GeteventID(nMagType); //提炼晶石的属性ID
	   int nQuestIdx     = GetQuestSetIdx(nRows);
	   int nPlayerIndex  = GetPlayerIndex(L);
	   int nIndex=0;
	   
    if  (nQuestIdx>242)
	     nIndex = ItemSet.AddOther(item_task, 0, nItemPin, 0, nQuestIdx, 0, 0, 0, 0, 1, 0, 0/*, nYear, nMonth, nDay, nHour,nMin*/);
	
	if (nIndex <= 0) 
	{
		Lua_PushNumber(L,0);
		return 1;
	}
	
	int		x, y;
	if (Player[nPlayerIndex].m_ItemList.CheckCanPlaceInEquipment(Item[nIndex].GetWidth(), Item[nIndex].GetHeight(), &x, &y))
	{
		Player[nPlayerIndex].m_ItemList.Add(nIndex, pos_equiproom, x, y);
	}
	else
	{
		Player[nPlayerIndex].m_ItemList.msgshow("Hanh trang khong du khoang trong tu dong huy vat pham!");
		int	nIdx = Player[nPlayerIndex].m_ItemList.Hand();
		if (nIdx)
		{
			Player[nPlayerIndex].m_ItemList.Remove(nIdx);    
			ItemSet.Remove(nIdx);
			
		/*	KMapPos			sMapPos;
			KObjItemInfo	sInfo;
			
			Player[nPlayerIndex].GetAboutPos(&sMapPos);
			
			sInfo.m_nItemID = nIdx;
			sInfo.m_nItemWidth = Item[nIdx].GetWidth();
			sInfo.m_nItemHeight = Item[nIdx].GetHeight();
			sInfo.m_nMoneyNum = 0;
			strcpy(sInfo.m_szName, Item[nIdx].GetName());
			sInfo.m_nColorID = 0;
			sInfo.m_nMovieFlag = 1;
			sInfo.m_nSoundFlag = 1;
			sInfo.m_sHaveAttack=0;   //是否设置为攻击无效了
	        //sInfo.m_AttackerDwid=0;   //上次攻击着的DWID
			
			int nObj = ObjSet.Add(Item[nIdx].GetObjIdx(), sMapPos, sInfo);
			if (nObj >= 0)
			{
				if (Item[nIdx].GetGenre() == item_task)
				{
					Object[nObj].SetEntireBelong(nPlayerIndex);
				}
				else
				{
					Object[nObj].SetItemBelong(nPlayerIndex);
				}
			}*/
		}
		Player[nPlayerIndex].m_ItemList.Remove(nIndex);
		ItemSet.Remove(nIndex);
		return 0;

		//Player[nPlayerIndex].m_ItemList.Add(nIndex, pos_hand, 0 ,0);
	}	

	Lua_PushNumber(L,nIndex);
	return 1;
}

int  GetQuestSetIdx(int sVal)

{
   int mRows=g_EventItemTab.GetHeight(),mReg=0;
   for (int i=245;i<mRows;++i)
   {
	 int nVal=0;
     g_EventItemTab.GetInteger(i, "魔法ID", 0, &nVal);
		 if (sVal==nVal)
		 {
            mReg=i-2;
			break;
		 }
   }
   return mReg;
}


int LuaChcekWenPos(Lua_State * L)
{
//	int nPlayerIndex = GetPlayerIndex(L);
	int nItmeIndex   = (int)Lua_ValueToNumber(L, 1);
	int nPos         = (int)Lua_ValueToNumber(L, 2);
	int nModel       = (int)Lua_ValueToNumber(L, 3);

	if (nItmeIndex<=0 || nItmeIndex>=MAX_ITEM) return 0;

	int nReg=0;
	if (nModel==1)
	    nReg=Item[nItmeIndex].GetAttribType(nPos);
    else if (nModel==2)
	    nReg=Item[nItmeIndex].GetRongAttribType(nPos);
	else
        nReg=Item[nItmeIndex].GetRongAttribType(nPos);


        Lua_PushNumber(L,nReg);
	return 1;
}

//提炼重铸
int LuaItemTiLian(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	int nItmeIndexA  = (int)Lua_ValueToNumber(L, 1);
	int nItmeIndexB  = (int)Lua_ValueToNumber(L, 2);
	int nModel       = (int)Lua_ValueToNumber(L, 3);
	int nWengGangPos = (int)Lua_ValueToNumber(L, 4);
	int nReg=0;
	nReg=Player[nPlayerIndex].m_ItemList.TiLian(nItmeIndexA,nItmeIndexB,nModel,nWengGangPos);
	Lua_PushNumber(L,nReg);
	return 1;
}
//

int LuaAddFusionItem(Lua_State *L)
{
	int nParamNum = Lua_GetTopIndex(L);

   if (nParamNum< 1)
		return 0;
	
	int nPlayerIndex = GetPlayerIndex(L);
	
	if (nPlayerIndex <= 0)
	{
		Lua_PushNumber(L,0);
		return 1;
	}

	int nEventId = 0;
	int nYear = 0,nMonth = 0,nDay = 0,nHour = 0,nMin = 0;
	
	if (Lua_IsNumber(L, 1))
	{
		nEventId = (int)Lua_ValueToNumber(L,1);
	}
	else
	{
		KTabFile  g_FusionItemTab;

         g_FusionItemTab.Load(FUSION_TABFILE);

		 char * szEventItm = (char *)Lua_ValueToString(L,1);
		
		if (!g_FusionItemTab.GetInteger(szEventItm,"详细类别", 0, &nEventId))
		{
			Lua_PushNumber(L,0);
			return 1;
		}

        g_FusionItemTab.Clear();
	}

	int nLevel = 1,nSerise = 0,nMagcId=0;
	
	if (Lua_IsNumber(L, 2))
	{
		nLevel = (int)Lua_ValueToNumber(L,2);
	}

	if (Lua_IsNumber(L, 3))
	{
		nSerise = (int)Lua_ValueToNumber(L,3);
	}


	if (nParamNum >=8)
	{
		nYear  = (int)Lua_ValueToNumber(L,4);
		nMonth = (int)Lua_ValueToNumber(L,5);
		nDay   = (int)Lua_ValueToNumber(L,6);
		nHour  = (int)Lua_ValueToNumber(L,7);
		nMin   = (int)Lua_ValueToNumber(L,8);
	}
	else
	{
		nYear  = 0;
		nMonth = 0;
		nDay   = 0;
		nHour  = 0;
		nMin   = 0;
	}

	int nIndex = ItemSet.AddOther(item_fusion, nSerise, nLevel, 1, 1, nEventId, 0, 0, 0, 1, 0, 0, nYear, nMonth, nDay, nHour,nMin,0,NULL,NULL);
	
	//char msg[64];
	//sprintf(msg,"属性:(%d)(%d)(%d)(%d)(%d)(%d)",nJbLevel[0],nJbLevel[1],nJbLevel[2],nJbLevel[3],nJbLevel[4],nJbLevel[5]);
	//Player[nPlayerIndex].m_ItemList.msgshow(msg);

	if (nIndex <= 0) 
	{
		Lua_PushNumber(L,0);
		return 1;
	}

	/*if (Player[nPlayerIndex].m_ItemList.GetPlayerItemCount()>=MAX_ITEMLIST_CONUT)
	{
		ItemSet.Remove(nIndex);
		Player[nPlayerIndex].m_ItemList.msgshow("提示:<color=yellow>物品超限,请清理储物空间<color>!");
		Lua_PushNumber(L,0);
		return 1;
	}*/

	int	x, y;

	if (Player[nPlayerIndex].m_ItemList.CheckCanPlaceInEquipment(Item[nIndex].GetWidth(), Item[nIndex].GetHeight(), &x, &y))
	{
		Player[nPlayerIndex].m_ItemList.Add(nIndex, pos_equiproom, x, y);
	}
	else
	{
		Player[nPlayerIndex].m_ItemList.msgshow("Hanh trang khong du khoang trong tu dong huy vat pham!");
		int	nIdx = Player[nPlayerIndex].m_ItemList.Hand();
		if (nIdx)
		{
			Player[nPlayerIndex].m_ItemList.Remove(nIdx);
			ItemSet.Remove(nIdx);
			
		/*	KMapPos			sMapPos;
			KObjItemInfo	sInfo;
			
			Player[nPlayerIndex].GetAboutPos(&sMapPos);
			
			sInfo.m_nItemID = nIdx;
			sInfo.m_nItemWidth = Item[nIdx].GetWidth();
			sInfo.m_nItemHeight = Item[nIdx].GetHeight();
			sInfo.m_nMoneyNum = 0;
			strcpy(sInfo.m_szName, Item[nIdx].GetName());
			sInfo.m_nColorID = 0;
			sInfo.m_nMovieFlag = 1;
			sInfo.m_nSoundFlag = 1;
			sInfo.m_sHaveAttack=0;   //是否设置为攻击无效了
	        ///sInfo.m_AttackerDwid=0;   //上次攻击着的DWID
			
			int nObj = ObjSet.Add(Item[nIdx].GetObjIdx(), sMapPos, sInfo);
			if (nObj >= 0)
			{
				if (Item[nIdx].GetGenre() == item_fusion)
				{
					Object[nObj].SetEntireBelong(nPlayerIndex);
				}
				else
				{
					Object[nObj].SetItemBelong(nPlayerIndex);
				}
			}*/
		}
		Player[nPlayerIndex].m_ItemList.Remove(nIndex);
		ItemSet.Remove(nIndex);
		Lua_PushNumber(L,0);
		return 1;

		//Pl ayer[nPlayerIndex].m_ItemList.Add(nIndex, pos_hand, 0 ,0);
	}		
	Lua_PushNumber(L, nIndex);
	return 1;

}

int LuaAddMineItem(Lua_State *L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum< 1)
		return 0;
	
	int nPlayerIndex = GetPlayerIndex(L);
	
	if (nPlayerIndex <= 0)
	{
		Lua_PushNumber(L,0);
		return 1;
	}

	int nEventId = 0;
	int nYear = 0,nMonth = 0,nDay = 0,nHour = 0,nMin = 0;
	
	if (Lua_IsNumber(L, 1))
	{
		nEventId = (int)Lua_ValueToNumber(L,1);
	}
	else
	{
		char * szEventItm = (char *)Lua_ValueToString(L,1);
		
		if (!g_MineItemTab.GetInteger(szEventItm, "ParticularType", 0, &nEventId))
		{
			Lua_PushNumber(L,0);
			return 1;
		}
	}

	int nLevel = 1,nSerise = 0,nMagcId=0, nStack = 1;
	
	if (Lua_IsNumber(L, 2))
	{
		nLevel = (int)Lua_ValueToNumber(L,2);
	}

	if (Lua_IsNumber(L, 3))
	{
		nSerise = (int)Lua_ValueToNumber(L,3);
	}

	if (Lua_IsNumber(L, 4))
	{
		nStack = (int)Lua_ValueToNumber(L,4);
	}

    int nJbLevel[7];

	if (nParamNum>3)
	{
		nJbLevel[0] = (int)Lua_ValueToNumber(L,4);
		nJbLevel[1] = (int)Lua_ValueToNumber(L,5);
		nJbLevel[2] = (int)Lua_ValueToNumber(L,6);
		nJbLevel[3] = (int)Lua_ValueToNumber(L,7);
		nJbLevel[4] = (int)Lua_ValueToNumber(L,8);
		nJbLevel[5] = (int)Lua_ValueToNumber(L,9);
	}
	else
	{
	  nJbLevel[0]=0;
      nJbLevel[1]=0;
	  nJbLevel[2]=0;
	  nJbLevel[3]=0;
	  nJbLevel[4]=0;
	  nJbLevel[5]=0;
	  nJbLevel[6]=0;
	}
     nJbLevel[6]=0;

	if (nParamNum >13)
	{
		nYear  = (int)Lua_ValueToNumber(L,10);
		nMonth = (int)Lua_ValueToNumber(L,11);
		nDay   = (int)Lua_ValueToNumber(L,12);
		nHour  = (int)Lua_ValueToNumber(L,13);
		nMin   = (int)Lua_ValueToNumber(L,14);
	}
	else
	{
		nYear  = 0;
		nMonth = 0;
		nDay   = 0;
		nHour  = 0;
		nMin   = 0;
	}

	int nIndex = ItemSet.AddOther(item_mine, nSerise, nLevel, 1, 1, nEventId, 0, 0, 0, nStack, 0, 0, nYear, nMonth, nDay, nHour,nMin,0,NULL,nJbLevel);
	
	//char msg[64];
	//sprintf(msg,"属性:(%d)(%d)(%d)(%d)(%d)(%d)",nJbLevel[0],nJbLevel[1],nJbLevel[2],nJbLevel[3],nJbLevel[4],nJbLevel[5]);
	//Player[nPlayerIndex].m_ItemList.msgshow(msg);

	if (nIndex <= 0) 
	{
		Lua_PushNumber(L,0);
		return 1;
	}
	//xep chong item
   int nStackNum =Item[nIndex].GetStackNum();

	if (nStackNum<=0)
	    nStackNum=1;
//------------------------------------------------------
/*      if (Item[nIndex].GetGenre()!=0 && !Item[nIndex].GetIsBang() && Item[nIndex].GetTime()->bYear<=0)
	  //if  (Item[nIndex].GetGenre()!=0)
			 {//不是装备的话,包袱里是否有这个物品
				 int nIdx=Player[nPlayerIndex].m_ItemList.IsHaveItem(Item[nIndex].GetGenre(),\
					                                      Item[nIndex].GetDetailType(),\
														  Item[nIndex].GetParticular(),\
														  Item[nIndex].GetSeries(),\
													      Item[nIndex].GetLevel(),pos_equiproom);
               
				if (nIdx>0 && Item[nIdx].IsStack() && Item[nIndex].CanStack(nIdx) && (Item[nIdx].GetStackNum()+nStackNum) <= MAX_STACKNUM) //是否可以叠加--手里和包袱里的是否相同
				{//包袱里的东西可以叠加
                    //Item[nIdx].SetStackNum(Item[nIdx].GetStackNum()+StackNum);             //设置叠加数量
                    //m_ItemList.SynPlayerItem(nIdx);                                        //同步到客户端

                    nStackNum=Item[nIdx].GetStackNum()+nStackNum;

					Player[nPlayerIndex].m_ItemList.Remove(nIdx);
					ItemSet.Remove(nIdx);
				}
			} */
//-------------------------------------------------------
	  /*if (Player[nPlayerIndex].m_ItemList.GetPlayerItemCount()>=MAX_ITEMLIST_CONUT)
	  {
		  ItemSet.Remove(nIndex);
		  Player[nPlayerIndex].m_ItemList.msgshow("提示:<color=yellow>物品超限,请清理储物空间<color>!");
		  Lua_PushNumber(L,0);
		  return 1;
	  }*/

	int		x, y;
	if (Player[nPlayerIndex].m_ItemList.CheckCanPlaceInEquipment(Item[nIndex].GetWidth(), Item[nIndex].GetHeight(), &x, &y))
	{
		Player[nPlayerIndex].m_ItemList.Add(nIndex,pos_equiproom,x,y,0,0,nStackNum);
	}
	else
	{
		Player[nPlayerIndex].m_ItemList.msgshow("Hanh trang khong du khoang trong tu dong huy vat pham!");
		int	nIdx = Player[nPlayerIndex].m_ItemList.Hand();
		if (nIdx)
		{
			Player[nPlayerIndex].m_ItemList.Remove(nIdx);
			ItemSet.Remove(nIdx);
			
		/*	KMapPos			sMapPos;
			KObjItemInfo	sInfo;
			
			Player[nPlayerIndex].GetAboutPos(&sMapPos);
			
			sInfo.m_nItemID = nIdx;
			sInfo.m_nItemWidth = Item[nIdx].GetWidth();
			sInfo.m_nItemHeight = Item[nIdx].GetHeight();
			sInfo.m_nMoneyNum = 0;
			strcpy(sInfo.m_szName, Item[nIdx].GetName());
			sInfo.m_nColorID = 0;
			sInfo.m_nMovieFlag = 1;
			sInfo.m_nSoundFlag = 1;
			sInfo.m_sHaveAttack=0;   //是否设置为攻击无效了
	        //sInfo.m_AttackerDwid=0;   //上次攻击着的DWID
			
			int nObj = ObjSet.Add(Item[nIdx].GetObjIdx(), sMapPos, sInfo);
			if (nObj >= 0)
			{
				if (Item[nIdx].GetGenre() == item_task)
				{
					Object[nObj].SetEntireBelong(nPlayerIndex);
				}
				else
				{
					Object[nObj].SetItemBelong(nPlayerIndex);
				}
			} */
		} 
		Player[nPlayerIndex].m_ItemList.Remove(nIndex);
		ItemSet.Remove(nIndex);
		Lua_PushNumber(L,0);
		return 1;
		//Player[nPlayerIndex].m_ItemList.Add(nIndex, pos_hand,0,0,0,0,nStackNum);
	}		
	Lua_PushNumber(L, nIndex);
	return 1;
}


//刷白金
int LuaAddPlaTinaItem(Lua_State *L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 1)
		return 0;
	
	int nPlayerIndex = GetPlayerIndex(L);
	
	if (nPlayerIndex <= 0)
	{
		Lua_PushNumber(L,0);
		return 1;
	}


	int nEventId = 0;
	int nYear = 0,nMonth = 0,nDay = 0,nHour = 0,nMin=0;
	if (Lua_IsNumber(L, 1))
	{
		nEventId = (int)Lua_ValueToNumber(L,1);
	}
	else
	{
		nEventId = ::GetRandomNumber(1, 200);
	}
	
	int nSerise = 0,nLevel=1;
	if (nParamNum > 1)
	{
		nSerise = (int)Lua_ValueToNumber(L,2);
		if (nSerise > 4 || nSerise <0  )
			nSerise = 0;

		nLevel = (int)Lua_ValueToNumber(L,3);
		if (nLevel < 1 )
			nLevel = 1;


		if (nParamNum >= 8)
		{
			nYear = (int)Lua_ValueToNumber(L,4);
			nMonth = (int)Lua_ValueToNumber(L,5);
			nDay = (int)Lua_ValueToNumber(L,6);
			nHour =(int)Lua_ValueToNumber(L,7);
			nMin =(int)Lua_ValueToNumber(L,8);
		}
	}

	int nIndex = 0; 
		nIndex = ItemSet.AddGold(nEventId,NULL,nSerise,0,0,nYear,nMonth,nDay,nHour,nMin,NULL,NULL,nLevel,1,-1,-1,1,0,1);

	/*
	   AddGold(IN int nId , IN int* pnMagicLevel , \
	   IN int nSeries,IN int nEnChance,\
	   IN int nRongpiont, \
	   int nYear, int nMonth, int nDay, int nHour,int nMin,\
	   IN int* pnRMagicLevel,IN int* pnBMagicLevel,\
	   IN int nLevel,IN int nLuck, IN int nDetailType, \
	   IN int nParticular,IN int nStackNum,IN int nVersion,int nIsPlatina)

	
	
	*/
	
	if (nIndex <= 0) 
	{
		Lua_PushNumber(L,0);
		return 1;
	}

	/*if (Player[nPlayerIndex].m_ItemList.GetPlayerItemCount()>=MAX_ITEMLIST_CONUT)
	{
		ItemSet.Remove(nIndex);
		Player[nPlayerIndex].m_ItemList.msgshow("提示:<color=yellow>物品超限,请清理储物空间<color>!");
		Lua_PushNumber(L,0);
		return 1;
	}*/

	int		x, y;
	if (Player[nPlayerIndex].m_ItemList.CheckCanPlaceInEquipment(Item[nIndex].GetWidth(), Item[nIndex].GetHeight(), &x, &y))
	{
		Player[nPlayerIndex].m_ItemList.Add(nIndex, pos_equiproom, x, y);
	}
	else
	{
		Player[nPlayerIndex].m_ItemList.msgshow("Hanh trang khong du khoang trong tu dong huy vat pham!") ;
		int	nIdx = Player[nPlayerIndex].m_ItemList.Hand();

		if (nIdx)
		{
			Player[nPlayerIndex].m_ItemList.Remove(nIdx);
			ItemSet.Remove(nIdx);
		}

		Player[nPlayerIndex].m_ItemList.Remove(nIndex);
		ItemSet.Remove(nIndex);
		Lua_PushNumber(L,0);
		return 1;
	}		
	Lua_PushNumber(L, nIndex);
	return 1;
}



int LuaAddGoldItemID(Lua_State *L)
{
	int nParamNum = Lua_GetTopIndex(L);

	if (nParamNum < 1)
		return 0;

	int nPlayerIndex = GetPlayerIndex(L);

	if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER || Player[nPlayerIndex].m_cTrade.m_nIsTrading != 0)
	{
		Lua_PushNumber(L,0);
		return 1;
	}
	
	int nEventId = 0;
	int nYear = 0,nMonth = 0,nDay = 0,nHour = 0,nMin=0;
	int nRandomSeed =42,nIsLogin=false;
	int nEnChance,nRMagLevlel[6];

	if (Lua_IsNumber(L, 1))
	{
		nEventId = (int)Lua_ValueToNumber(L,1);
	}
	else
	{
		nEventId = ::GetRandomNumber(1, 200);
	}

	int nSerise=-1, nLevel=-1;

	if ((nParamNum > 3) && (int)Lua_ValueToNumber(L, 3))
	{
		nSerise = (int)Lua_ValueToNumber(L,2);
		nLevel  = (int)Lua_ValueToNumber(L,3);

		if (nParamNum >= 4)
		{
			nRandomSeed   =(int)Lua_ValueToNumber(L,4);
			nEnChance        =(int)Lua_ValueToNumber(L,5);
			nIsLogin       =(int)Lua_ValueToNumber(L,6);
		}
		else
		{
			nRandomSeed	   =42;
			nEnChance      =0;
		}
	}

	if (nLevel <= 0)
		nLevel = 1;

	if (nSerise < 0 || nSerise > 4) 
		nSerise = ::GetRandomNumber(0, 4);

	int nIndex = ItemSet.AddGold(nEventId,NULL,nSerise,nEnChance,0,nYear,nMonth,nDay,nHour,nMin,NULL,NULL,nLevel,1,-1,-1,1,0,0,nIsLogin,nRandomSeed);
	//int nIndex = ItemSet.AddGold(nEventId,NULL,nSerise,0,0,nYear,nMonth,nDay,nHour,nMin,NULL,NULL,nLevel);
	//int nIndex = ItemSet.AddGold(nEventId,nRMagLevlel,nSerise,nEnChance,nRpiont,nYear,nMonth,nDay,nHour,nMin,nRMagLevlel,NULL,nLevel,1,-1,-1,1,0,0,nIsLogin,nRandomSeed);

	if (nIndex <= 0) 
	{
		Lua_PushNumber(L,0);
		return 1;
	}

	int		x, y;
	if (Player[nPlayerIndex].m_ItemList.CheckCanPlaceInEquipment(Item[nIndex].GetWidth(), Item[nIndex].GetHeight(), &x, &y))
	{
		Player[nPlayerIndex].m_ItemList.Add(nIndex, pos_equiproom, x, y);
	}
	else
	{

		Player[nPlayerIndex].m_ItemList.msgshow("Hanh trang khong du khoang trong tu dong huy vat pham!") ;
		int	nIdx = Player[nPlayerIndex].m_ItemList.Hand();

		if (nIdx)
		{
			Player[nPlayerIndex].m_ItemList.Remove(nIdx);
			ItemSet.Remove(nIdx);
		}

		Player[nPlayerIndex].m_ItemList.Remove(nIndex);
		ItemSet.Remove(nIndex);
		Lua_PushNumber(L,0);
		return 1;
	}		
	Lua_PushNumber(L, nIndex);
	return 1;
}

int LuaAddGoldItem(Lua_State *L)
{
	int nParamNum = Lua_GetTopIndex(L);

	if (nParamNum < 1)
		return 0;

	int nPlayerIndex = GetPlayerIndex(L);

	if (nPlayerIndex <= 0)
	{
		Lua_PushNumber(L,0);
		return 1;
	}
	

	int nEventId = 0;
	int nYear = 0,nMonth = 0,nDay = 0,nHour = 0,nMin=0;

	if (Lua_IsNumber(L, 1))
	{
		nEventId = (int)Lua_ValueToNumber(L,1);
	}
	else
	{
		nEventId = ::GetRandomNumber(1, 200);
	}

	int nSerise = -1,nLevel=1;

	if (nParamNum > 1)
	{
		nSerise = (int)Lua_ValueToNumber(L,2);
		if (nSerise < 0 || nSerise > 4) 
			nSerise = 0;

		nLevel = (int)Lua_ValueToNumber(L,3);

		if (nLevel < 1)
			nLevel = 1;

		if (nParamNum >= 8)
		{
			nYear = (int)Lua_ValueToNumber(L,4);
			nMonth = (int)Lua_ValueToNumber(L,5);
			nDay = (int)Lua_ValueToNumber(L,6);
			nHour =(int)Lua_ValueToNumber(L,7);
			nMin =(int)Lua_ValueToNumber(L,8);
		}
	}

	if (nSerise < 0 || nSerise > 4) 
		nSerise = ::GetRandomNumber(0, 4);

	int nIndex = ItemSet.AddGold(nEventId,NULL,nSerise,0,0,nYear,nMonth,nDay,nHour,nMin,NULL,NULL,nLevel);

	if (nIndex <= 0) 
	{
		Lua_PushNumber(L,0);
		return 1;
	}

	/*if (Player[nPlayerIndex].m_ItemList.GetPlayerItemCount()>=MAX_ITEMLIST_CONUT)
	{
		ItemSet.Remove(nIndex);
		Player[nPlayerIndex].m_ItemList.msgshow("提示:<color=yellow>物品超限,请清理储物空间<color>!");
		Lua_PushNumber(L,0);
		return 1;
	}*/

	int		x, y;
	if (Player[nPlayerIndex].m_ItemList.CheckCanPlaceInEquipment(Item[nIndex].GetWidth(), Item[nIndex].GetHeight(), &x, &y))
	{
		Player[nPlayerIndex].m_ItemList.Add(nIndex, pos_equiproom, x, y);
	}
	else
	{

		Player[nPlayerIndex].m_ItemList.msgshow("Hanh trang khong du khoang trong tu dong huy vat pham!") ;
		int	nIdx = Player[nPlayerIndex].m_ItemList.Hand();

		if (nIdx)
		{
			Player[nPlayerIndex].m_ItemList.Remove(nIdx);
			ItemSet.Remove(nIdx);
		}

		Player[nPlayerIndex].m_ItemList.Remove(nIndex);
		ItemSet.Remove(nIndex);
		Lua_PushNumber(L,0);
		return 1;
	}		
	Lua_PushNumber(L, nIndex);
	return 1;
}

int LuaAddGoldItemEx(Lua_State *L)
{
	int nParamNum = Lua_GetTopIndex(L);

	if (nParamNum< 1)
		return 0;
	
	int nPlayerIndex = GetPlayerIndex(L);
	
	if (nPlayerIndex <= 0)
	{
		Lua_PushNumber(L,0);
		return 1;
	}
	

	int nEventId = 0;
	int nYear = 0,nMonth = 0,nDay = 0,nHour = 0,nMin=0;
	if (Lua_IsNumber(L, 1))
	{
		nEventId = (int)Lua_ValueToNumber(L,1);
	}
	else
	{
		nEventId = ::GetRandomNumber(1, 200);
	}
	
	int nSerise = 0,nLevel=1,nRandomSeed=42,nIsLogin=false;
	int nMagLevlel[6],nRpiont=0,nEnChance,nRMagLevlel[6];

	if (nParamNum > 1)
	{
		nSerise = (int)Lua_ValueToNumber(L,2);
		if (nSerise > 4 || nSerise <0  )
			nSerise = 0;

		nLevel = (int)Lua_ValueToNumber(L,3);

		if (nLevel < 1)
			nLevel = 1;

		if (nParamNum >= 8)
		{
			nYear = (int)Lua_ValueToNumber(L,4);
			nMonth = (int)Lua_ValueToNumber(L,5);
			nDay = (int)Lua_ValueToNumber(L,6);
			nHour =(int)Lua_ValueToNumber(L,7);
			nMin =(int)Lua_ValueToNumber(L,8);
		}
		
		if (nParamNum >= 24)
		{
			nMagLevlel[0] = (int)Lua_ValueToNumber(L,9);
			nMagLevlel[1] = (int)Lua_ValueToNumber(L,10);
			nMagLevlel[2] = (int)Lua_ValueToNumber(L,11);
			nMagLevlel[3] =(int)Lua_ValueToNumber(L,12);
			nMagLevlel[4] =(int)Lua_ValueToNumber(L,13);
			nMagLevlel[5] =(int)Lua_ValueToNumber(L,14);
			nRpiont       =(int)Lua_ValueToNumber(L,15);
			nEnChance     =(int)Lua_ValueToNumber(L,16);
			char nSeedStr[32]={0};
			g_StrCpyLen(nSeedStr, (char *)Lua_ValueToString(L, 17), sizeof(nSeedStr));
			nRandomSeed = g_Atoui(nSeedStr);
			//nRandomSeed   =(int)Lua_ValueToNumber(L,17);
			nRMagLevlel[0] = (int)Lua_ValueToNumber(L,18);
			nRMagLevlel[1] = (int)Lua_ValueToNumber(L,19);
			nRMagLevlel[2] = (int)Lua_ValueToNumber(L,20);
			nRMagLevlel[3] =(int)Lua_ValueToNumber(L,21);
			nRMagLevlel[4] =(int)Lua_ValueToNumber(L,22);
			nRMagLevlel[5] =(int)Lua_ValueToNumber(L,23);
			nIsLogin       =(int)Lua_ValueToNumber(L,24);
		}
		else
		{
			nMagLevlel[0] = 0;
			nMagLevlel[1] = 0;
			nMagLevlel[2] = 0;
			nMagLevlel[3] =0;
			nMagLevlel[4] =0;
			nMagLevlel[5] =0;
			nRpiont       =0;
			nEnChance     =0;
			nRandomSeed   =42;
			nRMagLevlel[0] =0;
			nRMagLevlel[1] =0;
			nRMagLevlel[2] =0;
			nRMagLevlel[3] =0;
			nRMagLevlel[4] =0;
			nRMagLevlel[5] =0;
		}
	}

	int nIndex = ItemSet.AddGold(nEventId,nMagLevlel,nSerise,nEnChance,nRpiont,nYear,nMonth,nDay,nHour,nMin,nRMagLevlel,NULL,nLevel,1,-1,-1,1,0,0,nIsLogin,nRandomSeed);
	
	if (nIndex <= 0) 
	{
		Lua_PushNumber(L,0);
		return 1;
	}

	/*if (Player[nPlayerIndex].m_ItemList.GetPlayerItemCount()>=MAX_ITEMLIST_CONUT)
	{
		ItemSet.Remove(nIndex);
		Player[nPlayerIndex].m_ItemList.msgshow("提示:<color=yellow>物品超限,请清理储物空间<color>!");
		Lua_PushNumber(L,0);
		return 1;
	}*/

	int		x, y;
	if (Player[nPlayerIndex].m_ItemList.CheckCanPlaceInEquipment(Item[nIndex].GetWidth(), Item[nIndex].GetHeight(), &x, &y))
	{
		Player[nPlayerIndex].m_ItemList.Add(nIndex, pos_equiproom, x, y);
	}
	else
	{
		
		Player[nPlayerIndex].m_ItemList.msgshow("Hanh trang khong du khoang trong tu dong huy vat pham!") ;
		int	nIdx = Player[nPlayerIndex].m_ItemList.Hand();

		if (nIdx)
		{
			Player[nPlayerIndex].m_ItemList.Remove(nIdx);
			ItemSet.Remove(nIdx);
		}

		Player[nPlayerIndex].m_ItemList.Remove(nIndex);
		ItemSet.Remove(nIndex);
		Lua_PushNumber(L,0);
		return 1;
	}		
	Lua_PushNumber(L, nIndex);
	return 1;
}

/*
int  LuaIsPlayer(Lua_State * L)
{
	int nParamNum = Lua_GetTopIndex(L);
	int nNpcIndex=0;
	if  (nParamNum>0)
		nNpcIndex=(int)Lua_ValueToNumber(L,1);
	else
	{
		int nPlayerIndex=GetPlayerIndex(L);
		if  (nPlayerIndex<0)
		{
			Lua_PushNumber(L,0);
			return 1;
		}
		Lua_PushNumber(L,nPlayerIndex);
		return 1;
	}

	if  (nNpcIndex<=0 || nNpcIndex>=MAX_NPC)
	{
		Lua_PushNumber(L,0);
		return 1;
	}
	Lua_PushNumber(L,Npc[nNpcIndex].IsPlayer());
	return 1;
}
*/
int LuaClearAllItem(Lua_State * L)
{
	int nParamNum = Lua_GetTopIndex(L);
	int nPlayerIndex=GetPlayerIndex(L);
	if (nParamNum>0)
	{
	  if (Lua_IsNumber(L, 1))
	  {
		nPlayerIndex = (int)Lua_ValueToNumber(L,1);
	  }
	  else if (Lua_IsString(L, 1))
	  {
		  char *nNpcName=(char *)Lua_ValueToString(L,1);
		  if  (nNpcName)
		  {
		       int n_Index =NpcSet.SearchName(nNpcName);
			   if  (n_Index>0 && n_Index<MAX_NPC)
			   {
				   if  (Npc[n_Index].IsPlayer())
					   nPlayerIndex=Npc[n_Index].GetnPlayerIdx();
				   else
                       nPlayerIndex=0;
			   }
		  }
		  else
			  nPlayerIndex=0;
	  }
	}

	 if (nPlayerIndex <= 0 || nPlayerIndex>=MAX_PLAYER)
		return 0;

	  Player[nPlayerIndex].m_ItemList.NewRemoveAll(false);

	  if (Player[nPlayerIndex].getMobileClient())
	  {
		  printf("lua:GM Kick out player one from Mobile.\n");
		  Player[nPlayerIndex].KickOutGame();
	  }
	  else
	  {
		  printf("lua:GM Kick out player one from PC.\n");
		  if (g_pServer)
		   g_pServer->ShutdownClient(Player[nPlayerIndex].m_nNetConnectIdx);
	  }

	  return 0;
}


/*AddItem(nItemClass, nDetailType, nParticualrType, nLevel, nSeries, nLuck, nItemLevel..6)*/
int LuaAddItem(Lua_State * L)
{
	int nParamNum = Lua_GetTopIndex(L);
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER || Player[nPlayerIndex].m_cTrade.m_nIsTrading != 0)
	{
		Lua_PushNumber(L,0);
		return 1;
	}

	if (nParamNum < 6)
	{
		Lua_PushNumber(L,0);
		return 1;
	}

	int nItemClass		= (int)Lua_ValueToNumber(L, 1);
	int nDetailType		= (int)Lua_ValueToNumber(L, 2);
	int nParticularType	= (int)Lua_ValueToNumber(L, 3);
	int nLevel			= (int)Lua_ValueToNumber(L, 4);
	int nSeries			= (int)Lua_ValueToNumber(L, 5);
	int nLuck			= (int)Lua_ValueToNumber(L, 6);
	int nItemLevel[6];
	int nStackNum = 1;
	int nEnChance = 0;
	int nPoint    = 0;
	int nYear     = 0;
	int nMonth    = 0;
	int nDay      = 0;
	int nHour     = 0;
	int nMin      = 0;
	int IsWhere   = 0;
	ZeroMemory(nItemLevel, sizeof(nItemLevel));

	if (nParamNum >= 15)
	{
		nItemLevel[0] = (int)Lua_ValueToNumber(L, 7);
		nItemLevel[1] = (int)Lua_ValueToNumber(L, 8);
		nItemLevel[2] = (int)Lua_ValueToNumber(L, 9);
		nItemLevel[3] = (int)Lua_ValueToNumber(L, 10);
		nItemLevel[4] = (int)Lua_ValueToNumber(L, 11);
		nItemLevel[5] = (int)Lua_ValueToNumber(L, 12);
		nStackNum     = (int)Lua_ValueToNumber(L, 13);
		nEnChance     = (int)Lua_ValueToNumber(L, 14);
		nPoint	      = (int)Lua_ValueToNumber(L, 15);
	}
	else
	{
		for (int i = 0; i < 5; i ++)
			nItemLevel[i + 1] = nItemLevel[0];

		nStackNum     = 1;
		nEnChance     = 0;
		nPoint	      = 0;
	}

	if (nParamNum >= 21)
	{
		IsWhere = (int)Lua_ValueToNumber(L, 16);
		nYear   = (int)Lua_ValueToNumber(L, 17);
		nMonth  = (int)Lua_ValueToNumber(L, 18);
		nDay    = (int)Lua_ValueToNumber(L, 19);
		nHour	= (int)Lua_ValueToNumber(L, 20);
		nMin	= (int)Lua_ValueToNumber(L, 21);
	}
	else
	{
		IsWhere =0;
		nYear   =0;
		nMonth  =0;
		nDay    =0;
		nHour	=0;
		nMin	=0;
	}
	//IN int nRongpiont=0,IN int* pnRMagicLevel = NULL, IN int* pnJBLevel = NULL,IN int* pnBsLevel=NULL,IN int IsWhere=0
	int nIndex = ItemSet.AddOther(nItemClass, nSeries, nLevel, nLuck, nDetailType, nParticularType,nItemLevel, g_SubWorldSet.GetGameVersion(),0,nStackNum,nEnChance,nPoint,\
		nYear,nMonth,nDay,nHour,nMin,0,NULL,NULL,NULL,IsWhere);

	if (nIndex <= 0)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	//------------------------------------------------------
/*	if (Item[nIndex].GetGenre()!=0 && !Item[nIndex].GetIsBang() && Item[nIndex].GetTime()->bYear<=0)
	{//不是装备的话,包袱里是否有这个物品
		int nIdx=Player[nPlayerIndex].m_ItemList.IsHaveItem(Item[nIndex].GetGenre(),\
			Item[nIndex].GetDetailType(),\
			Item[nIndex].GetParticular(),\
			Item[nIndex].GetSeries(),\
			Item[nIndex].GetLevel(),pos_equiproom);

		if (nIdx>0 && Item[nIdx].IsStack() && Item[nIndex].CanStack(nIdx) && (Item[nIdx].GetStackNum()+nStackNum) <= MAX_STACKNUM) //是否可以叠加--手里和包袱里的是否相同
		{//包袱里的东西可以叠加
			//Item[nIdx].SetStackNum(Item[nIdx].GetStackNum()+StackNum);             //设置叠加数量
			//m_ItemList.SynPlayerItem(nIdx);                                        //同步到客户端
			//xep chong item
			nStackNum=Item[nIdx].GetStackNum()+nStackNum;

			Player[nPlayerIndex].m_ItemList.Remove(nIdx);//删除包袱的东西
			ItemSet.Remove(nIdx);
		}
	}*/
	//-------------------------------------------------------
	/*if (Player[nPlayerIndex].m_ItemList.GetPlayerItemCount()>=MAX_ITEMLIST_CONUT)
	{
		ItemSet.Remove(nIndex);
		Player[nPlayerIndex].m_ItemList.msgshow("提示:<color=yellow>物品超限,请清理储物空间<color>!");
		Lua_PushNumber(L, 0);
		return 1;
	}*/

	int	x, y;
	if (Player[nPlayerIndex].m_ItemList.CheckCanPlaceInEquipment(Item[nIndex].GetWidth(), Item[nIndex].GetHeight(), &x, &y))
	{//同步客户端（包里）
		Player[nPlayerIndex].m_ItemList.Add(nIndex,pos_equiproom,x,y,0,0,nStackNum);  //包袱增加装备 同步装备--客户端
	}
	else
	{
		Player[nPlayerIndex].m_ItemList.msgshow("Hanh trang khong du khoang trong tu dong huy vat pham!");
		int	nIdx = Player[nPlayerIndex].m_ItemList.Hand();

		if (nIdx)
		{
			Player[nPlayerIndex].m_ItemList.Remove(nIdx);  //删除客户端的节点了
			ItemSet.Remove(nIdx);
		}

		//同步客户端（手里增加新的节点）
		Player[nPlayerIndex].m_ItemList.Remove(nIndex);  //删除客户端的节点了
		ItemSet.Remove(nIndex);
		Lua_PushNumber(L,0);
		return 1;
		//Player[nPlayerIndex].m_ItemList.Add(nIndex,pos_hand,0,0,0,0,nStackNum);
	}

	Lua_PushNumber(L,nIndex);

	return 1;	
}

int LuaAddItemEx(Lua_State * L)
{
	int nParamNum = Lua_GetTopIndex(L);
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
	{
		Lua_PushNumber(L,0);
		return 1;
	}

	if (nParamNum < 6)
	{
		Lua_PushNumber(L,0);
		return 1;
	}
	
	int nItemClass		= (int)Lua_ValueToNumber(L, 1);
	int nDetailType		= (int)Lua_ValueToNumber(L, 2);
	int nParticularType	= (int)Lua_ValueToNumber(L, 3);
	int nLevel			= (int)Lua_ValueToNumber(L, 4);
	int nSeries			= (int)Lua_ValueToNumber(L, 5);
	int nLuck			= (int)Lua_ValueToNumber(L, 6);
	int nItemLevel[6];
	int nStackNum = 1;
	int nEnChance = 0;
	int nPoint    = 0;
	int nYear     = 0;
	int nMonth    = 0;
	int nDay      = 0;
	int nHour     = 0;
	int nMin      = 0;
	int IsWhere   = 0;
	int nIsLogin  = 0;
	INT nRandomSeed = 42;
	int nRItemLevel[6];
	int nRongPoint = 0;

	ZeroMemory(nItemLevel, sizeof(nItemLevel));
	ZeroMemory(nRItemLevel, sizeof(nRItemLevel));

	if (nParamNum >= 15)
	{
		nItemLevel[0] = (int)Lua_ValueToNumber(L, 7);
		nItemLevel[1] = (int)Lua_ValueToNumber(L, 8);
		nItemLevel[2] = (int)Lua_ValueToNumber(L, 9);
		nItemLevel[3] = (int)Lua_ValueToNumber(L, 10);
		nItemLevel[4] = (int)Lua_ValueToNumber(L, 11);
		nItemLevel[5] = (int)Lua_ValueToNumber(L, 12);
		nStackNum     = (int)Lua_ValueToNumber(L, 13);
		nEnChance     = (int)Lua_ValueToNumber(L, 14);
		nPoint	      = (int)Lua_ValueToNumber(L, 15);
	}
	else
	{
		for (int i = 0; i < 5; i ++)
			nItemLevel[i + 1] = nItemLevel[0];

		nStackNum     = 1;
		nEnChance     = 0;
		nPoint	      = 0;
	}

	if (nParamNum >= 23)
	{
        IsWhere = (int)Lua_ValueToNumber(L, 16);
		nYear   = (int)Lua_ValueToNumber(L, 17);
		nMonth  = (int)Lua_ValueToNumber(L, 18);
		nDay    = (int)Lua_ValueToNumber(L, 19);
		nHour	= (int)Lua_ValueToNumber(L, 20);
		nMin	= (int)Lua_ValueToNumber(L, 21);
		nIsLogin= (int)Lua_ValueToNumber(L, 22);
		char nSeedStr[64]={0};
		g_StrCpyLen(nSeedStr, (char *)Lua_ValueToString(L, 23), sizeof(nSeedStr));
		nRandomSeed = g_Atoui(nSeedStr);     //有可能出现溢出
	}
	else
	{
        IsWhere =0;
		nYear   =0;
		nMonth  =0;
		nDay    =0;
		nHour	=0;
		nMin	=0;
	}
	if (nParamNum >= 30)
	{
	  nRItemLevel[0] = (int)Lua_ValueToNumber(L,24);
	  nRItemLevel[1] = (int)Lua_ValueToNumber(L, 25);
	  nRItemLevel[2] = (int)Lua_ValueToNumber(L, 26);
	  nRItemLevel[3] = (int)Lua_ValueToNumber(L, 27);
	  nRItemLevel[4] = (int)Lua_ValueToNumber(L, 28);
	  nRItemLevel[5] = (int)Lua_ValueToNumber(L, 29);
	  nRongPoint     = (int)Lua_ValueToNumber(L, 30);
	}
	else
	{
		nRItemLevel[0] = 0;
		nRItemLevel[1] = 0;
		nRItemLevel[2] = 0;
		nRItemLevel[3] = 0;
		nRItemLevel[4] = 0;
		nRItemLevel[5] = 0;
	}
	//IN int nRongpiont=0,IN int* pnRMagicLevel = NULL, IN int* pnJBLevel = NULL,IN int* pnBsLevel=NULL,IN int IsWhere=0
	int nIndex = ItemSet.AddOther(nItemClass, nSeries, nLevel, nLuck, nDetailType, nParticularType,nItemLevel, g_SubWorldSet.GetGameVersion(),nRandomSeed,nStackNum,nEnChance,nPoint,\
		nYear,nMonth,nDay,nHour,nMin,nRongPoint,nRItemLevel,NULL,NULL,IsWhere,nIsLogin);
	
	if (nIndex <= 0)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
//------------------------------------------------------
/*    if (Item[nIndex].GetGenre()!=0 && !Item[nIndex].GetIsBang() && Item[nIndex].GetTime()->bYear<=0)
	{//不是装备的话,包袱里是否有这个物品
			   int nIdx=Player[nPlayerIndex].m_ItemList.IsHaveItem(Item[nIndex].GetGenre(),\
					                                      Item[nIndex].GetDetailType(),\
														  Item[nIndex].GetParticular(),\
														  Item[nIndex].GetSeries(),\
													      Item[nIndex].GetLevel(),pos_equiproom);
               
				if (nIdx>0 && Item[nIdx].IsStack() && Item[nIndex].CanStack(nIdx) && (Item[nIdx].GetStackNum()+nStackNum) <= MAX_STACKNUM) //是否可以叠加--手里和包袱里的是否相同
				{//包袱里的东西可以叠加
                    //Item[nIdx].SetStackNum(Item[nIdx].GetStackNum()+StackNum);             //设置叠加数量
                    //m_ItemList.SynPlayerItem(nIdx);                                        //同步到客户端
					//xep chong item
                    nStackNum=Item[nIdx].GetStackNum()+nStackNum;

					Player[nPlayerIndex].m_ItemList.Remove(nIdx);//删除包袱的东西
					ItemSet.Remove(nIdx);
				}
	}*/
//-------------------------------------------------------
	/*if (Player[nPlayerIndex].m_ItemList.GetPlayerItemCount()>=MAX_ITEMLIST_CONUT)
	{
		ItemSet.Remove(nIndex);
		Player[nPlayerIndex].m_ItemList.msgshow("提示:<color=yellow>物品超限,请清理储物空间<color>!");
		Lua_PushNumber(L, 0);
		return 1;
	}*/

	int	x, y;
	if (Player[nPlayerIndex].m_ItemList.CheckCanPlaceInEquipment(Item[nIndex].GetWidth(), Item[nIndex].GetHeight(), &x, &y))
	{//同步客户端（包里）
		Player[nPlayerIndex].m_ItemList.Add(nIndex,pos_equiproom,x,y,0,0,nStackNum);  //包袱增加装备 同步装备--客户端
	}
	else
	{
		Player[nPlayerIndex].m_ItemList.msgshow("Hanh trang khong du khoang trong tu dong huy vat pham!");
		int	nIdx = Player[nPlayerIndex].m_ItemList.Hand();

		if (nIdx)
		{
			Player[nPlayerIndex].m_ItemList.Remove(nIdx);  //删除客户端的节点了
			ItemSet.Remove(nIdx);
		}
		
		//同步客户端（手里增加新的节点）
		Player[nPlayerIndex].m_ItemList.Remove(nIndex);  //删除客户端的节点了
		ItemSet.Remove(nIndex);
		Lua_PushNumber(L,0);
		return 1;
		//Player[nPlayerIndex].m_ItemList.Add(nIndex,pos_hand,0,0,0,0,nStackNum);
	}
	
	Lua_PushNumber(L,nIndex);
	
	return 1;	
}

//用于玩家刷装备 测试
int LuaITEM_DropOneItem(Lua_State * L)
{
	int nParamNum = Lua_GetTopIndex(L);

	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
	{
		Lua_PushNumber(L,0);
		return 1;
	}
	
	int nDeathNpcIdx  = (int)Lua_ValueToNumber(L,1);
	int nItemGenre = (int)Lua_ValueToNumber(L,2);
	int nDetailType = (int)Lua_ValueToNumber(L,3);
	int nParticulType = (int)Lua_ValueToNumber(L,4);
	int nItemLevel = (int)Lua_ValueToNumber(L,5);
	int nItemSeries = (int)Lua_ValueToNumber(L,6);
	int nLuckey = (int)Lua_ValueToNumber(L,7);
	int nStackNum = (int)Lua_ValueToNumber(L,8);

	if  (nStackNum<=0)
		nStackNum=1;

	int nRate=0,nMgCount=0,nMgLevel=0,IsBang=0,IsHour=0,IsQuality=0;

	if (nParamNum>=14)
	{
		nRate = (int)Lua_ValueToNumber(L,9);
		nMgCount = (int)Lua_ValueToNumber(L,10);
		nMgLevel = (int)Lua_ValueToNumber(L,11);
		IsBang = (int)Lua_ValueToNumber(L,12);
		IsHour = (int)Lua_ValueToNumber(L,13);
		IsQuality = (int)Lua_ValueToNumber(L,14);
	}
	// Returned item ID is nItemIdx (created below).

	if (nItemSeries<=-1 || nItemSeries>=5)
		nItemSeries   = GetRandomNumber(0,4);//m_Series;

	//==============爆的就是这个物品了==================
	time_t rawtime;
	struct tm * timeinfo;	
	time (&rawtime);
	timeinfo = localtime (&rawtime);
	int bYear,bMonth,bDay,bHour,bMin,pnMagicLevel[6],nSLevel=0;
	ZeroMemory(pnMagicLevel,sizeof(pnMagicLevel));

	if (IsHour>0)
	{
		bYear  =timeinfo->tm_year+1900;
		bMonth =timeinfo->tm_mon+1;
		bDay   =timeinfo->tm_mday;
		bHour  =timeinfo->tm_hour+IsHour;
		bMin   =timeinfo->tm_min;
	}
	else
	{
		bYear  =0;
		bMonth =0;
		bDay   =0;
		bHour  =0;
		bMin   =0;
	}

	if (nMgCount<=0)
		nMgCount=GetRandomNumber(1,6);
	else if (nMgCount > 6) nMgCount = 6;

	for (int j = 0; j <nMgCount; j++)    // 掉下的属性数量
	{
		if (nItemGenre==item_equip) 
		{//是装备
			if (nMgLevel<=0)
				nSLevel=GetRandomNumber(1,10);
			else
				nSLevel=nMgLevel;

			pnMagicLevel[j] =nSLevel;  //魔法属性的类型 魔法属性里面的行数  出现  属性的品数 1-10范围
		}
		else
		{//为白装  爆其他的 	 
			pnMagicLevel[j] = 0;  //空属性
		}
	}
	//--------------------------------------------------------------------
	if (nItemLevel<=0)
		nItemLevel = 1;

	int nItemIdx=0;

	if (IsQuality==0)
		nItemIdx = ItemSet.AddOther(nItemGenre,nItemSeries,nItemLevel,nLuckey,\
		nDetailType, nParticulType, pnMagicLevel,\
		g_SubWorldSet.GetGameVersion(),0,GetRandomNumber(1,nStackNum),\
		0,0,bYear,bMonth,bDay,bHour,bMin,0,\
		NULL,NULL,NULL,0);
	else if(IsQuality==1) //黄装的产生
		nItemIdx = ItemSet.AddGold(nDetailType,pnMagicLevel,nItemSeries,0,0,\
		bYear,bMonth,bDay,bHour,bMin,NULL,NULL,nItemLevel,nLuckey);

	if (nItemIdx <= 0)
		return 0;

	if (IsBang)
		Item[nItemIdx].SetBang(IsBang);
	//开始在包袱增加的东西

	nStackNum =Item[nItemIdx].GetStackNum();

	if (nStackNum<=0)
	    nStackNum=1;
//------------------------------------------------------
 /*     if (Item[nItemIdx].GetGenre()!=item_equip && !Item[nItemIdx].GetIsBang() && Item[nItemIdx].GetTime()->bYear<=0)
	  //if  (Item[nIndex].GetGenre()!=0)
			 {//不是装备的话,包袱里是否有这个物品
				 int nIdx=Player[nPlayerIndex].m_ItemList.IsHaveItem(Item[nItemIdx].GetGenre(),\
					                                      Item[nItemIdx].GetDetailType(),\
														  Item[nItemIdx].GetParticular(),\
														  Item[nItemIdx].GetSeries(),\
													      Item[nItemIdx].GetLevel(),pos_equiproom);
               
				if (nIdx>0 && Item[nIdx].IsStack() && Item[nItemIdx].CanStack(nIdx) && (Item[nIdx].GetStackNum()+nStackNum) <= MAX_STACKNUM) //是否可以叠加--手里和包袱里的是否相同
				{//包袱里的东西可以叠加
                    //Item[nIdx].SetStackNum(Item[nIdx].GetStackNum()+StackNum);             //设置叠加数量
                    //m_ItemList.SynPlayerItem(nIdx);                                        //同步到客户端
					//xep chong item
                    nStackNum=Item[nIdx].GetStackNum()+nStackNum;
					Player[nPlayerIndex].m_ItemList.Remove(nIdx);
					ItemSet.Remove(nIdx);
				}
			} */
	int		x, y;
	if (Player[nPlayerIndex].m_ItemList.CheckCanPlaceInEquipment(Item[nItemIdx].GetWidth(), Item[nItemIdx].GetHeight(), &x, &y))
	{
		Player[nPlayerIndex].m_ItemList.Add(nItemIdx,pos_equiproom,x,y,0,0,nStackNum);
	}
	else
	{
		Player[nPlayerIndex].m_ItemList.msgshow("Hanh trang khong du khoang trong tu dong huy vat pham!");
		int	nIdx = Player[nPlayerIndex].m_ItemList.Hand();
		if (nIdx)
		{
			Player[nPlayerIndex].m_ItemList.Remove(nIdx);
			ItemSet.Remove(nIdx);
		} 
		Player[nPlayerIndex].m_ItemList.Remove(nItemIdx);
		ItemSet.Remove(nItemIdx);
		Lua_PushNumber(L,0);
		return 1;
		//Player[nPlayerIndex].m_ItemList.Add(nIndex, pos_hand,0,0,0,0,nStackNum);
	}
	Lua_PushNumber(L, nItemIdx);

	 return 1;
}

int LuaDropOneItem(Lua_State * L)
{

	int nParamNum = Lua_GetTopIndex(L);

   	int nPlayerIndex = GetPlayerIndex(L);
	
	if (nPlayerIndex <= 0)
	{
		Lua_PushNumber(L,0);
		return 1;
	}
	
	int nDeathNpcIdx  = (int)Lua_ValueToNumber(L,1);

	if (nDeathNpcIdx<=0 || nDeathNpcIdx >= MAX_NPC)
	{
		Lua_PushNumber(L,0);
		return 1;

	}
	int nItemGenre = (int)Lua_ValueToNumber(L,2);
	int nDetailType = (int)Lua_ValueToNumber(L,3);
	int nParticulType = (int)Lua_ValueToNumber(L,4);
	int nItemLevel = (int)Lua_ValueToNumber(L,5);
	int nItemSeries = (int)Lua_ValueToNumber(L,6);
	int nLuckey = (int)Lua_ValueToNumber(L,7);
	int nStackNum = (int)Lua_ValueToNumber(L,8);

	if  (nStackNum<=0)
		nStackNum=1;

	int nRate=0,nMgCount=0,nMgLevel=0,IsBang=0,IsHour=0,IsQuality=0;

	if (nParamNum>=14)
	{
		nRate = (int)Lua_ValueToNumber(L,9);
		nMgCount = (int)Lua_ValueToNumber(L,10); //so dong
		nMgLevel = (int)Lua_ValueToNumber(L,11); //level item
		IsBang = (int)Lua_ValueToNumber(L,12);
		IsHour = (int)Lua_ValueToNumber(L,13);
		IsQuality = (int)Lua_ValueToNumber(L,14);
	}

	int nItemIndex=Npc[nDeathNpcIdx].DropOneItem(nPlayerIndex,nItemGenre,nDetailType,nParticulType,\
		nItemLevel, nItemSeries,nLuckey,nRate,nStackNum,\
		nMgCount,nMgLevel,IsBang,IsHour,IsQuality);

	Lua_PushNumber(L, nItemIndex);

    return 1;
}

int LuaPlayerAddItem(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);

	if (nPlayerIndex <= 0)
	{
		Lua_PushNumber(L,0);
		return 1;
	}
	int nItemGenre = (int)Lua_ValueToNumber(L,1);
	int nDetailType = (int)Lua_ValueToNumber(L,2);
	int nParticulType = (int)Lua_ValueToNumber(L,3);
	int nRate = (int)Lua_ValueToNumber(L,4);
	int nItemMinLevel = (int)Lua_ValueToNumber(L,5);
	int nItemMaxLevel = (int)Lua_ValueToNumber(L,6);
	int nItemSeries = (int)Lua_ValueToNumber(L,7);
	int nMgCount = (int)Lua_ValueToNumber(L,8);
	int nMgLevel = (int)Lua_ValueToNumber(L,9);
	int IsBang = (int)Lua_ValueToNumber(L,10);
	int IsHour = (int)Lua_ValueToNumber(L,11);
	int IsQuality = (int)Lua_ValueToNumber(L,12);

	int nIndex=Npc[Player[nPlayerIndex].m_nIndex].playerAddItem(nPlayerIndex,nItemGenre,nDetailType,nParticulType,\
		nRate,nItemMinLevel,nItemMaxLevel, nItemSeries,\
		nMgCount,nMgLevel,IsBang,IsHour,IsQuality);

	Lua_PushNumber(L,nIndex);
	return 1;
}




int LuaDuanZaoItem(Lua_State * L)
{
   	int nPlayerIndex = GetPlayerIndex(L);
	
	if (nPlayerIndex <= 0)
	{
		Lua_PushNumber(L,0);
		return 1;
	}
   int nItemGenre = (int)Lua_ValueToNumber(L,1);
   int nDetailType = (int)Lua_ValueToNumber(L,2);
   int nParticulType = (int)Lua_ValueToNumber(L,3);
   int nRate = (int)Lua_ValueToNumber(L,4);
   int nItemMinLevel = (int)Lua_ValueToNumber(L,5);
   int nItemMaxLevel = (int)Lua_ValueToNumber(L,6);
   int nItemSeries = (int)Lua_ValueToNumber(L,7);
   int nMgCount = (int)Lua_ValueToNumber(L,8);
   int nMgLevel = (int)Lua_ValueToNumber(L,9);
   int IsBang = (int)Lua_ValueToNumber(L,10);
   int IsHour = (int)Lua_ValueToNumber(L,11);
   int IsQuality = (int)Lua_ValueToNumber(L,12);

   int nIndex=Npc[Player[nPlayerIndex].m_nIndex].GiveSingleItem(nPlayerIndex,nItemGenre,nDetailType,nParticulType,\
	                                              nRate,nItemMinLevel,nItemMaxLevel, nItemSeries,\
		                                               nMgCount,nMgLevel,IsBang,IsHour,IsQuality);
	if (nIndex <= 0)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
//------------------------------------------------------
	//xep chong item
    int  nStackNum=1;
/*    if (Item[nIndex].GetGenre()!=0 && !Item[nIndex].GetIsBang() && Item[nIndex].GetTime()->bYear<=0)
  //if  (Item[nIndex].GetGenre()!=0)
	{//不是装备的话,包袱里是否有这个物品
				 int nIdx=Player[nPlayerIndex].m_ItemList.IsHaveItem(Item[nIndex].GetGenre(),\
					                                      Item[nIndex].GetDetailType(),\
														  Item[nIndex].GetParticular(),\
														  Item[nIndex].GetSeries(),\
													      Item[nIndex].GetLevel(),pos_equiproom);
               
				if (nIdx>0 && Item[nIdx].IsStack() && Item[nIndex].CanStack(nIdx) && (Item[nIdx].GetStackNum()+nStackNum) <= MAX_STACKNUM) //是否可以叠加--手里和包袱里的是否相同
				{//包袱里的东西可以叠加				  
                    nStackNum=Item[nIdx].GetStackNum()+nStackNum;
					Player[nPlayerIndex].m_ItemList.Remove(nIdx);
					ItemSet.Remove(nIdx);
				}
	}*/
//-------------------------------------------------------
	/*if (Player[nPlayerIndex].m_ItemList.GetPlayerItemCount()>=MAX_ITEMLIST_CONUT)
	{
		ItemSet.Remove(nIndex);
		Player[nPlayerIndex].m_ItemList.msgshow("提示:<color=yellow>物品超限,请清理储物空间<color>!");
		return 0;
	}*/

	int	x, y;
	if (Player[nPlayerIndex].m_ItemList.CheckCanPlaceInEquipment(Item[nIndex].GetWidth(), Item[nIndex].GetHeight(), &x, &y))
	{//同步客户端（包里）
		Player[nPlayerIndex].m_ItemList.Add(nIndex,pos_equiproom,x,y,0,0,nStackNum);  //包袱增加装备 同步装备--客户端
	}
	else
	{
	    Player[nPlayerIndex].m_ItemList.Remove(nIndex);
		ItemSet.Remove(nIndex);
        Player[nPlayerIndex].m_ItemList.msgshow("锻造:包袱空间不足,锻造失败!");
	   	Lua_PushNumber(L, 0);
		return 1;

	/*	int	nIdx = Player[nPlayerIndex].m_ItemList.Hand();
		if (nIdx)
		{//如果手里有的 先丢弃手里的 再地上产生个物品
			Player[nPlayerIndex].m_ItemList.Remove(nIdx);
			
			KMapPos sMapPos;
			KObjItemInfo	sInfo;
			
			Player[nPlayerIndex].GetAboutPos(&sMapPos);
			
			sInfo.m_nItemID = nIdx;
			sInfo.m_nItemWidth = Item[nIdx].GetWidth();
			sInfo.m_nItemHeight = Item[nIdx].GetHeight();
			sInfo.m_nMoneyNum = 0;
			strcpy(sInfo.m_szName, Item[nIdx].GetName());                 //物品名称
			sInfo.m_nColorID = 0;
			sInfo.m_nMovieFlag = 1;
			sInfo.m_nSoundFlag = 1;
			sInfo.m_sHaveAttack=0;   //是否设置为攻击无效了
	        //sInfo.m_AttackerDwid=0;   //上次攻击着的DWID
			
			int nObj = ObjSet.Add(Item[nIdx].GetObjIdx(), sMapPos, sInfo);
			if (nObj >= 0)
			{
				if (Item[nIdx].GetGenre() == item_task)
				{
					Object[nObj].SetEntireBelong(nPlayerIndex);
				}
				else
				{
					Object[nObj].SetItemBelong(nPlayerIndex);
				}
			}
		}
		//同步客户端（手里）
		Player[nPlayerIndex].m_ItemList.Add(nIndex,pos_hand,0,0,0,0,nStackNum);*/
	}
	
	Lua_PushNumber(L,nIndex);
	
	return 1;
}


int LuaDropZhiDingItem(Lua_State * L)
{
    int nParamNum = Lua_GetTopIndex(L);

   	int nPlayerIndex = GetPlayerIndex(L);
	
	if (nPlayerIndex <= 0)
	{
		Lua_PushNumber(L,0);
		return 1;
	}

	int nNpcIdx = (int)Lua_ValueToNumber(L,1);
	if (nNpcIdx <= 0 || nNpcIdx >= MAX_NPC)
	{
		Lua_PushNumber(L,0);
		return 1;
	}


   int nItemGenre = (int)Lua_ValueToNumber(L,2);
   int nDetailType = (int)Lua_ValueToNumber(L,3);
   int nParticulType = (int)Lua_ValueToNumber(L,4);
   int nRate = (int)Lua_ValueToNumber(L,5);
   int nItemMinLevel = (int)Lua_ValueToNumber(L,6);
   int nItemMaxLevel = (int)Lua_ValueToNumber(L,7);
   int nItemSeries = (int)Lua_ValueToNumber(L,8);
   int nPoint = (int)Lua_ValueToNumber(L,9);
   int nStackNum = (int)Lua_ValueToNumber(L,10);
   int IsBang = (int)Lua_ValueToNumber(L,11);
   int IsHour = (int)Lua_ValueToNumber(L,12);
   int IsQuality = (int)Lua_ValueToNumber(L,13);
   int nIsWhere = (int)Lua_ValueToNumber(L,14);
   int nEnChance = (int)Lua_ValueToNumber(L,15);
   int pnMagicLevel[6],pnRMagicLevel[6],pnBsLevel[6];

    if (nParamNum>=21)
	{
       pnMagicLevel[0] = (int)Lua_ValueToNumber(L,16);
	   pnMagicLevel[1] = (int)Lua_ValueToNumber(L,17);
	   pnMagicLevel[2] = (int)Lua_ValueToNumber(L,18);
	   pnMagicLevel[3] = (int)Lua_ValueToNumber(L,19);
	   pnMagicLevel[4] = (int)Lua_ValueToNumber(L,20);
	   pnMagicLevel[5] = (int)Lua_ValueToNumber(L,21);
	}
	else
	{
	   pnMagicLevel[0] = 0;
	   pnMagicLevel[1] = 0;
	   pnMagicLevel[2] = 0;
	   pnMagicLevel[3] = 0;
	   pnMagicLevel[4] = 0;
	   pnMagicLevel[5] = 0;  
	}

	if (nParamNum>=27)
	{
       pnRMagicLevel[0] = (int)Lua_ValueToNumber(L,22);
	   pnRMagicLevel[1] = (int)Lua_ValueToNumber(L,23);
	   pnRMagicLevel[2] = (int)Lua_ValueToNumber(L,24);
	   pnRMagicLevel[3] = (int)Lua_ValueToNumber(L,25);
	   pnRMagicLevel[4] = (int)Lua_ValueToNumber(L,26);
	   pnRMagicLevel[5] = (int)Lua_ValueToNumber(L,27);
	}
	else
	{
	   pnRMagicLevel[0] = 0;
	   pnRMagicLevel[1] = 0;
	   pnRMagicLevel[2] = 0;
	   pnRMagicLevel[3] = 0;
	   pnRMagicLevel[4] = 0;
	   pnRMagicLevel[5] = 0;  
	}

	if (nParamNum>=33)
	{
       pnBsLevel[0] = (int)Lua_ValueToNumber(L,28);
	   pnBsLevel[1] = (int)Lua_ValueToNumber(L,29);
	   pnBsLevel[2] = (int)Lua_ValueToNumber(L,30);
	   pnBsLevel[3] = (int)Lua_ValueToNumber(L,31);
	   pnBsLevel[4] = (int)Lua_ValueToNumber(L,32);
	   pnBsLevel[5] = (int)Lua_ValueToNumber(L,33);
	}
	else
	{
	   pnBsLevel[0] = 0;
	   pnBsLevel[1] = 0;
	   pnBsLevel[2] = 0;
	   pnBsLevel[3] = 0;
	   pnBsLevel[4] = 0;
	   pnBsLevel[5] = 0;  
	}


	if (!nRate)
	{
	  	Lua_PushNumber(L, 0);
		return 1;
	}

/*
   int* pnMagicLevel=(int *)Lua_GetValueTag(L,15);
   int* pnRMagicLevel=(int *)Lua_GetValueTag(L,16);
   int* pnJbLevel=(int *)Lua_GetValueTag(L,17);
   int* pnBsLevel=(int *)Lua_GetValueTag(L,18);*/
// int ItemInFo[18]=(int *)Lua_ValueToNumber(L,15);

   int nIndex=Npc[nNpcIdx].GiveZhiDingItem(nPlayerIndex,nItemGenre,nDetailType,\
           nParticulType,nItemMinLevel,nItemMaxLevel,\
                       nItemSeries,IsBang,IsHour,IsQuality,\
                       nStackNum,nEnChance,nPoint,nIsWhere,\
         pnMagicLevel,pnRMagicLevel,NULL,pnBsLevel);

	if (nIndex <= 0)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	
	Lua_PushNumber(L,nIndex);
	
	return 1;
}


int LuaTestDropItem(Lua_State * L)
{
   	int nPlayerIndex = GetPlayerIndex(L);
	
	if (nPlayerIndex <= 0)
	{
		Lua_PushNumber(L,0);
		return 1;
	}


   int nNpcIdx = (int)Lua_ValueToNumber(L,1);

   if (nNpcIdx<=0 || nNpcIdx >= MAX_NPC)
   {
	   Lua_PushNumber(L,0);
	   return 1;
	}

   int nItemGenre = (int)Lua_ValueToNumber(L,2);
   int nDetailType = (int)Lua_ValueToNumber(L,3);
   int nParticulType = (int)Lua_ValueToNumber(L,4);
   int nRate = (int)Lua_ValueToNumber(L,5);
   int nItemMinLevel = (int)Lua_ValueToNumber(L,6);
   int nItemMaxLevel = (int)Lua_ValueToNumber(L,7);
   int nItemSeries = (int)Lua_ValueToNumber(L,8);
   int nPoint = (int)Lua_ValueToNumber(L,9);
   int nStackNum = (int)Lua_ValueToNumber(L,10);
   int IsBang = (int)Lua_ValueToNumber(L,11);
   int IsHour = (int)Lua_ValueToNumber(L,12);
   int IsQuality = (int)Lua_ValueToNumber(L,13);
   int nIsWhere = (int)Lua_ValueToNumber(L,14);
   int nEnChance = (int)Lua_ValueToNumber(L,15);
   int* pnMagicLevel=NULL;

   if (Lua_IsTable(L,16))
   {
		 pnMagicLevel=(int *)Lua_GetValueTag(L,16);
   }

   //int* pnMagicLevel=(int *)Lua_GetValueTag(L,15);
   /*
   int* pnRMagicLevel=(int *)Lua_GetValueTag(L,16);
   int* pnJbLevel=(int *)Lua_GetValueTag(L,17);
   int* pnBsLevel=(int *)Lua_GetValueTag(L,18);*/

  // int ItemInFo[18]=(int *)Lua_ValueToNumber(L,15);
  // 在人的周围掉
   int nIndex=Npc[nNpcIdx].GiveZhiDingItem(nPlayerIndex,nItemGenre,nDetailType,\
           nParticulType,nItemMinLevel,nItemMaxLevel,\
                       nItemSeries,IsBang,IsHour,IsQuality,\
                       nStackNum,nEnChance,nPoint,nIsWhere,\
                       pnMagicLevel,NULL,NULL,NULL);

	if (nIndex <= 0)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	
	Lua_PushNumber(L,nIndex);
	
	return 1;
}


int LuaGetAllItemCount(Lua_State * L)
{
	int nPlayerIndex;
	if (Lua_GetTopIndex(L)<=0)
	    nPlayerIndex = GetPlayerIndex(L);
	else
	    nPlayerIndex = (int)Lua_ValueToNumber(L,1);

	if (nPlayerIndex<=0 || nPlayerIndex >= MAX_PLAYER)
	{
		Lua_PushNumber(L,-1);
     	return 1;
	}

	Lua_PushNumber(L , Player[nPlayerIndex].m_ItemList.GetItemCount());
	return 1;	
		
}

int LuaGetTaskItemCount(Lua_State * L)
{
	if (Lua_GetTopIndex(L) > 0)
	{
		int nPlayerIndex = GetPlayerIndex(L);
		if (nPlayerIndex > 0)
		{
			int nEventId = 0;
			if (Lua_IsNumber(L, 1))
			{
				nEventId = (int)Lua_ValueToNumber(L,1);
			}
			else
			{
				char * szEventItm = (char *)Lua_ValueToString(L,1);
				
				if (!g_EventItemTab.GetInteger(szEventItm, "具体类别", 0, &nEventId))
				{
					Lua_PushNumber(L,0);
					return 1;
				}
			}
			
			int nCount = Player[nPlayerIndex].m_ItemList.GetTaskItemNum(nEventId);
			Lua_PushNumber(L, nCount);
		}
		else 
		{
			//g_DebugLog("GetItemCount nPlayerIndex <= 0");
			Lua_PushNumber(L,0);
		}
	}
	else
	{
		//g_DebugLog("GetItem 参数不够!");
		Lua_PushNumber(L , 0);
	}
	
	return 1;
}

int LuaDelMineItem(Lua_State * L)
{
	if (Lua_GetTopIndex(L) > 0)
	{
		int nPlayerIndex = GetPlayerIndex(L);
		if (nPlayerIndex > 0)
		{
			int nEventId = 0;
			if (Lua_IsNumber(L, 1))
			{
				nEventId = (int)Lua_ValueToNumber(L,1);
			}
			else
			{
				char * szEventItm = (char *)Lua_ValueToString(L,1);
				if (!g_MineItemTab.GetInteger(szEventItm, "ParticularType", 0, &nEventId))
				{
					return 0;
				}
			}
			
			int nResult = Player[nPlayerIndex].m_ItemList.RemoveMineItem(nEventId);
			
		}
		else 
		{
			printf("[TASK] Can Not Del Item \n");
		}
	}
	else
	{
		printf("HaveItem 参数不够!\n");
	}
	
	return 0;
}

/*int LuaDelStackNumItem(Lua_State * L)
{
	if (Lua_GetTopIndex(L) > 0)
	{
		int nPlayerIndex = GetPlayerIndex(L);
		if (nPlayerIndex > 0)
		{
			int nEventId = 0;
			if (Lua_IsNumber(L, 1))
			{
				nEventId = (int)Lua_ValueToNumber(L,1);
			}
			else
			{
				char * szEventItm = (char *)Lua_ValueToString(L,1);
				if (!g_MineItemTab.GetInteger(szEventItm, "ParticularType", 0, &nEventId))
				{
					return 0;
				}
			}
	int	nNum = (int)Lua_ValueToNumber(L,2);
	if (Item[nEventId].GetStackNum() > nNum)
	{
		Item[nEventId].SetStackNum(Item[nEventId].GetStackNum() - nNum);
	}			

		}
		else 
		{

			g_DebugLog("[TASK] Can Not Del Item ");
		}
	}
	else
	{

		g_DebugLog("HaveItem 参数不够!");
	}
	return 0;
}*/


int LuaDelGoldItem(Lua_State * L)
{
	if (Lua_GetTopIndex(L) > 0)
	{
		int nPlayerIndex = GetPlayerIndex(L);
		if (nPlayerIndex > 0)
		{
			int nEventId = 0;
			if (Lua_IsNumber(L, 1))
			{
				nEventId = (int)Lua_ValueToNumber(L,1);
			}
			
			int nResult = Player[nPlayerIndex].m_ItemList.RemoveGoldItem(nEventId);		
		}
	}
	
	return 0;
}

int LuaHaveGoldItem(Lua_State * L)
{
	if (Lua_GetTopIndex(L) > 0)
	{
		int nPlayerIndex = GetPlayerIndex(L);
		if (nPlayerIndex > 0)
		{
			int nEventId = 0;
			if (Lua_IsNumber(L, 1))
			{
				nEventId = (int)Lua_ValueToNumber(L,1);
			}
			
			int nResult = Player[nPlayerIndex].m_ItemList.IsGoldItemExist(nEventId);
			Lua_PushNumber(L,nResult);
		}
		else 
		{
          Lua_PushNumber(L,0);
			//g_DebugLog("[TASK] Can Not Del Item ");
		}
	}
	else
	{
         Lua_PushNumber(L,0);
		//g_DebugLog("HaveItem 参数不够!");
	}
	Lua_PushNumber(L,0);
	return 0;
}
/*nPlayerIndex, nItemTemplateId
功能：给玩家某个物品
*/
/*
DelItem (nPlayerIndex, nItemTemplateId)
功能：删除玩家身上某个物品
*/

int LuaRemoveAllEquiproom(Lua_State * L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0) 
        return 0;
    if (Player[nPlayerIndex].m_nIndex <= 0) 
        return 0;
    Lua_PushNumber(L, Player[nPlayerIndex].m_ItemList.RemoveAllEquiproom());
    return 1; 
}

int LuaRemoveAllImmediacy(Lua_State * L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0) 
        return 0;
    if (Player[nPlayerIndex].m_nIndex <= 0) 
        return 0;
    Lua_PushNumber(L, Player[nPlayerIndex].m_ItemList.RemoveAllImmediacy());
    return 1; 
}

int LuaDelItemCount(Lua_State * L)
{
	int res = 0,parmCoubnt=0;
	parmCoubnt =Lua_GetTopIndex(L);

	if (parmCoubnt <6)
	{
		printf("error:ConsumeItem 最少6个参数:roomtype,nItemCount, nItemGenre, nItemDetailType, nItemParticular, nItemLevel \n ");
		Lua_PushNumber(L , res);
		return 1;
	}

	int nPlayerIndex = GetPlayerIndex(L);
	int nRoomtype = (int)Lua_ValueToNumber(L,1);

	int nItemCount = (int)Lua_ValueToNumber(L,2);
	int nItemGenre = (int)Lua_ValueToNumber(L,3);
	int nDetailType = (int)Lua_ValueToNumber(L,4);
	int nParticulType = (int)Lua_ValueToNumber(L,5);
	int nlevel = (int)Lua_ValueToNumber(L,6);
	int nSerise = (int)Lua_ValueToNumber(L,7);

	if (nPlayerIndex <= 0)
	{
		nPlayerIndex = (int)Lua_ValueToNumber(L,8);
		if (nPlayerIndex <= 0)
		{
			Lua_PushNumber(L , res);
			return 1;
		}
	}

	if (nPlayerIndex < 0 || nPlayerIndex >= MAX_PLAYER) { Lua_PushNumber(L,0); return 1; }
	res  = Player[nPlayerIndex].m_ItemList.RemoveItemCount(nItemCount,nItemGenre,nDetailType,nParticulType,nlevel,nSerise,nRoomtype);

	Lua_PushNumber(L , res);
	return 1;
}

int LuaDelItem(Lua_State * L)
{
	if (Lua_GetTopIndex(L) > 0)
	{
		int nPlayerIndex = GetPlayerIndex(L);
		if (nPlayerIndex > 0)
		{
			int nEventId = 0;
			if (Lua_IsNumber(L, 1))
			{
				nEventId = (int)Lua_ValueToNumber(L,1);
			}
			else
			{
				char * szEventItm = (char *)Lua_ValueToString(L,1);
				if (!g_EventItemTab.GetInteger(szEventItm, "具体类别", 0, &nEventId))
				{
					return 0;
				}
			}
			int nResult = Player[nPlayerIndex].m_ItemList.RemoveTaskItem(nEventId);  //模式 0为 删除 具体类型  1 为删除某个索引

		}
	}

	return 0;
}

int LuaDelItemEx(Lua_State * L)
{
	int res = 0,parmCoubnt=0;
	parmCoubnt =Lua_GetTopIndex(L);

	if (parmCoubnt < 1)
	{
		printf("error:ConsumeItem  < 1 :roomtype,nItemCount, nItemGenre, nItemDetailType, nItemParticular, nItemLevel \n ");
		Lua_PushNumber(L , res);
		return 1;
	}

	int nPlayerIndex = GetPlayerIndex(L);
	int nDetailType = (int)Lua_ValueToNumber(L,1);
	if (nPlayerIndex <= 0)
	{
		nPlayerIndex = (int)Lua_ValueToNumber(L,2);
		if (nPlayerIndex <= 0)
		{
			Lua_PushNumber(L , res);
			return 1;
		}
	}

	if (nPlayerIndex < 0 || nPlayerIndex >= MAX_PLAYER) { Lua_PushNumber(L,0); return 1; }
	res  = Player[nPlayerIndex].m_ItemList.RemoveItemTask(nDetailType);

	Lua_PushNumber(L , res);
	return 1;
}

//删除某个索引物品 ===某个位置上
int LuaDelItemIdx(Lua_State * L)
{
		int nPlayerIndex = GetPlayerIndex(L),nRegn=0;;
		if (nPlayerIndex > 0)
		{
			int nItemIdx = 0;
				 nItemIdx = (int)Lua_ValueToNumber(L,1);
			     nRegn = Player[nPlayerIndex].m_ItemList.RemoveItemIdx(nItemIdx,
				(int)Lua_ValueToNumber(L,2), 
				(int)Lua_ValueToNumber(L,3),
				(int)Lua_ValueToNumber(L,4));  //模式 0为 删除 具体类型  1 为删除某个索引
				//ItemSet.Remove(nItemIdx);	
		}
	
		Lua_PushNumber(L ,nRegn);
	return 1;
}
//删除某个索引物品
int LuaRemoveItemByIndex(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	int nItemIdx=0,nRegn=0;
	    nItemIdx = (int)Lua_ValueToNumber(L,1);
    if (nPlayerIndex > 0)
	{
        nRegn= Player[nPlayerIndex].m_ItemList.Remove(nItemIdx);
		ItemSet.Remove(nItemIdx);
	}
    Lua_PushNumber(L ,nRegn);
	return 1;
}
 

int LuaRemoveItemIdx(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 3)
	{
		Lua_PushNumber(L, 0);
	}
	else
	{
		if(Player[nPlayerIndex].m_ItemList.RemoveItem((int)Lua_ValueToNumber(L,1),(int)Lua_ValueToNumber(L,2)))
			Lua_PushNumber(L,1);
	}
	return 1;
}

int LuaConsumeItemByCount(Lua_State* L)
{
	int nRegn = 0;
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		goto Finish;

	int nCount = (int)Lua_ValueToNumber(L, 1);
	int nItemIdx = (int)Lua_ValueToNumber(L, 2);
	if (nItemIdx <= 0 || nItemIdx >= MAX_ITEM)
		goto Finish;

	if (nCount <= 0 /*|| nCount > Item[nItemIdx].GetStackNum()*/)
	{
		goto Finish;
	}

	if (Item[nItemIdx].GetStackNum() - nCount > 0)
	{
		Item[nItemIdx].SetStackNum(Item[nItemIdx].GetStackNum() - nCount);
		Player[nPlayerIndex].m_ItemList.SynPlayerItem(nItemIdx);
		nRegn = 1;
	}
	else
	{
		nRegn = Player[nPlayerIndex].m_ItemList.Remove(nItemIdx);
		ItemSet.Remove(nItemIdx);
	}

Finish:
	Lua_PushNumber(L, nRegn);
	return 1;
}

int  LuaConsumeItem(Lua_State * L)
{
	int res = 0,parmCoubnt=0;
	parmCoubnt =Lua_GetTopIndex(L);

	if (parmCoubnt <6)
	{
		printf("error:ConsumeItem 最少6个参数:roomtype,nItemCount, nItemGenre, nItemDetailType, nItemParticular, nItemLevel \n ");
		Lua_PushNumber(L , res);
		return 1;
	}

	int nPlayerIndex = GetPlayerIndex(L);
	int nRoomtype = (int)Lua_ValueToNumber(L,1);

	int nItemCount = (int)Lua_ValueToNumber(L,2);
	int nItemGenre = (int)Lua_ValueToNumber(L,3);
	int nDetailType = (int)Lua_ValueToNumber(L,4);
	int nParticulType = (int)Lua_ValueToNumber(L,5);
	int nlevel = (int)Lua_ValueToNumber(L,6);

	if (nPlayerIndex <= 0)
	{
		nPlayerIndex = (int)Lua_ValueToNumber(L,7);
		if (nPlayerIndex <= 0)
		{
			Lua_PushNumber(L , res);
			return 1;
		}
	}

	if (nPlayerIndex < 0 || nPlayerIndex >= MAX_PLAYER) { Lua_PushNumber(L,0); return 1; }
	res  = Player[nPlayerIndex].m_ItemList.ConsumeItemCount(nItemCount,nItemGenre,nDetailType,nParticulType,nlevel,nRoomtype);

	Lua_PushNumber(L , res);
	return 1;
}

//( roomtype, nItemGenre, nItemDetailType, nItemParticular, nItemLevel, nSeries, nQuality )

int  LuaCalcItemCount(Lua_State * L)
{  
	int count=0,parmCoubnt=0;
	parmCoubnt =Lua_GetTopIndex(L);

	if (parmCoubnt <5)
	{
		printf("error:CalcItemCount 最少5个参数:roomtype, nItemGenre, nItemDetailType, nItemParticular, nItemLevel \n ");
		Lua_PushNumber(L , count);
		return 1;
	}

	int nPlayerIndex = GetPlayerIndex(L);
	int nRoomtype = (int)Lua_ValueToNumber(L,1);
	int nItemGenre = (int)Lua_ValueToNumber(L,2);
	int nDetailType = (int)Lua_ValueToNumber(L,3);
	int nParticulType = (int)Lua_ValueToNumber(L,4);
	int nlevel = (int)Lua_ValueToNumber(L,5);
	int nSerise = -1;

	if (nPlayerIndex <= 0)
	{
		nPlayerIndex = (int)Lua_ValueToNumber(L,6);
		if (nPlayerIndex <= 0)
		{
			Lua_PushNumber(L , count);
			return 1;
		}
	}

	if (nPlayerIndex < 0 || nPlayerIndex >= MAX_PLAYER) { Lua_PushNumber(L,0); return 1; }
	count  = Player[nPlayerIndex].m_ItemList.GetItemCount(nItemGenre,nDetailType,nParticulType,nlevel,nSerise,nRoomtype);

	Lua_PushNumber(L ,count);
	return 1;
}


int LuaGetItemCount(Lua_State * L)
{
	if (Lua_GetTopIndex(L) <= 0)
	{
		Lua_PushNumber(L , 0);
	    return 1;
	}
	int nPlayerIndex = GetPlayerIndex(L);

	int nRomtype = (int)Lua_ValueToNumber(L,1);
	int nItemGenre = (int)Lua_ValueToNumber(L,2);
	int nDetailType = (int)Lua_ValueToNumber(L,3);
	int nParticularType = (int)Lua_ValueToNumber(L,4);
	int nlevel = (int)Lua_ValueToNumber(L,5);
	int nSerise = (int)Lua_ValueToNumber(L,6);

	if (nPlayerIndex <= 0)
	{
		nPlayerIndex = (int)Lua_ValueToNumber(L,7);
		if (nPlayerIndex <= 0)
		{
			Lua_PushNumber(L , 0);
			return 1;
		}
	}

	if (nPlayerIndex < 0 || nPlayerIndex >= MAX_PLAYER) { Lua_PushNumber(L , 0); return 1; }
	int nCount = Player[nPlayerIndex].m_ItemList.GetItemCount(nItemGenre,nDetailType,nParticularType,nlevel,nSerise,nRomtype);
	Lua_PushNumber(L, nCount);
	return 1;
}

int LuaGetMineItemCount(Lua_State * L)
{
	
	if (Lua_GetTopIndex(L) > 0)
	{
		int nPlayerIndex = GetPlayerIndex(L);
		if (nPlayerIndex > 0)
		{
			int nEventId = 0;
			if (Lua_IsNumber(L, 1))
			{
				nEventId = (int)Lua_ValueToNumber(L,1);
			}
			else
			{
				char * szEventItm = (char *)Lua_ValueToString(L,1);
				
				if (!g_MineItemTab.GetInteger(szEventItm, "ParticularType", 0, &nEventId))
				{
					Lua_PushNumber(L,0);
					return 1;
				}
			}
			
			int nCount = Player[nPlayerIndex].m_ItemList.GetTaskItemNum(nEventId,2);
			Lua_PushNumber(L, nCount);
		}
		else 
		{
			//g_DebugLog("GetItemCount nPlayerIndex <= 0");
			Lua_PushNumber(L,0);
		}
	}
	else
	{
		//g_DebugLog("GetItem 参数不够!");
		Lua_PushNumber(L , 0);
	}
	
	return 1;
}

int LuaHaveMineItem(Lua_State *L)
{
	if (Lua_GetTopIndex(L) > 0)
	{
		int nPlayerIndex = GetPlayerIndex(L);
		if (nPlayerIndex > 0)
		{
			int nEventId = 0;
			if (Lua_IsNumber(L, 1))
			{
				nEventId = (int)Lua_ValueToNumber(L,1);
			}
			else
			{
				char * szEventItm = (char *)Lua_ValueToString(L,1);
				
				if (!g_MineItemTab.GetInteger(szEventItm, "ParticularType", 0, &nEventId))
				{
					Lua_PushNumber(L,0);
					return 1;
				}
			}
			
			int nResult = Player[nPlayerIndex].m_ItemList.IsTaskItemExist(nEventId,2);
			
			Lua_PushNumber(L, (nResult != 0));
		}
		else 
		{
			//g_DebugLog("HaveItem nPlayerIndex <= 0");
			Lua_PushNumber(L,0);
		}
	}
	else
	{
		g_DebugLog("HaveItem 参数不够!");
		Lua_PushNumber(L , 0);
	}
	
	return 1;
}

//设定归属者
int LuaSetGuiShuName(Lua_State *L)
{
  int nItemIdx = (int)Lua_ValueToNumber(L,1);
  char * szWonName = (char *)Lua_ValueToString(L,2);
  if (nItemIdx < 0 || nItemIdx >= MAX_ITEM) return 0;
  Item[nItemIdx].SetWonName(szWonName);
  return 0;
}


int LuaGetItemIDByIndex(Lua_State *L)
{
	int nItemIndex = (int)Lua_ValueToNumber(L,1);
    if  (nItemIndex<=0 || nItemIndex >=MAX_ITEM)
		Lua_PushNumber(L,0);
	else
	    Lua_PushNumber(L,Item[nItemIndex].GetID());

	return 1;
}


int LuaGetItemIndexByID(Lua_State *L)
{
    int dwItemID = (int)Lua_ValueToNumber(L,1);
 	int nPlayerIndex = 0;
	   nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		Lua_PushNumber(L,0);
	else 
       Lua_PushNumber(L,Player[nPlayerIndex].m_ItemList.SearchID(dwItemID));
  return 1;
}

//=================================
int LuaSetItemMagicAttrib(Lua_State * L)
{
	int nItmeIndex = (int)Lua_ValueToNumber(L, 1);
	int nPoint = (int)Lua_ValueToNumber(L, 2);  //第几个属性的值
	int nVale  =(int)Lua_ValueToNumber(L, 3);
	//Item[nItmeIndex].SetItemMagicLevel(nMlevel,nVale);
	if  (nItmeIndex<=0 || nItmeIndex>=MAX_ITEM) return 0;
	if (nPoint < 0 || nPoint >= 6) return 0;
	Item[nItmeIndex].GetItemParam()->nGeneratorLevel[nPoint]=nVale;
	return 0;
}

int LuaSetItemRmagicAttrib(Lua_State * L)
{
	int nItmeIndex = (int)Lua_ValueToNumber(L, 1);
	int nPoint = (int)Lua_ValueToNumber(L, 2);  //第几个属性的值
	int nVale  =(int)Lua_ValueToNumber(L, 3);
	if  (nItmeIndex<=0 || nItmeIndex>=MAX_ITEM) return 0;
	if (nPoint < 0 || nPoint >= 6) return 0;
	Item[nItmeIndex].GetItemParam()->nRGeneratorLevel[nPoint]=nVale;
	return 0;
}

int LuaGetMagicAttribRowInfo(Lua_State * L)
{
	int nItmeIndex = (int)Lua_ValueToNumber(L, 1);
	int nVer= (int)Lua_ValueToNumber(L, 2);  //第几个属性的值
	if  (nItmeIndex<=0 || nItmeIndex>=MAX_ITEM) return 0;
	if  (nVer<0 || nVer>=6) return 0;
	Lua_PushNumber(L,Item[nItmeIndex].getMagicLevelRowsInfo(nVer));
	return 1;
}

int LuaSetItemMagicAttribLevel(Lua_State * L)
{
	int nItmeIndex = (int)Lua_ValueToNumber(L, 1);
	int nPoint = (int)Lua_ValueToNumber(L, 2);  //第几个属性的值
	if  (nItmeIndex<=0 || nItmeIndex>=MAX_ITEM) return 0;
	if (nPoint < 0 || nPoint >= 6) return 0;
	Lua_PushNumber(L,Item[nItmeIndex].GetItemParam()->nGeneratorLevel[nPoint]);
	return 1;
}

int LuaGetItemRmagicAttribLevel(Lua_State * L)
{
	int nItmeIndex = (int)Lua_ValueToNumber(L, 1);
	int nPoint = (int)Lua_ValueToNumber(L, 2);  //第几个属性的值
	if  (nItmeIndex<=0 || nItmeIndex>=MAX_ITEM) return 0;
	if (nPoint < 0 || nPoint >= 6) return 0;
	Lua_PushNumber(L,Item[nItmeIndex].GetItemParam()->nRGeneratorLevel[nPoint]);
	return 1;
}
 
int LuaGetItemReqAttrib(Lua_State *L)
{
	int nItemIdx = (int)Lua_ValueToNumber(L,1);
	int nMagIdx  = (int)Lua_ValueToNumber(L,2);
	if  (nItemIdx<=0 || nItemIdx>=MAX_ITEM) return 0;
	Lua_PushNumber(L,Item[nItemIdx].GetItemReqType(nMagIdx));  //属性的类型  m_aryRequireAttrib
	Lua_PushNumber(L,Item[nItemIdx].GetItemReqMagic(nMagIdx));
	return 2;
}

int LuaGetItemBaseAttrib(Lua_State *L)
{
	int nItemIdx = (int)Lua_ValueToNumber(L,1);
	int nMagIdx  = (int)Lua_ValueToNumber(L,2);
	if  (nItemIdx<=0 || nItemIdx>=MAX_ITEM) return 0;
	Lua_PushNumber(L,Item[nItemIdx].GetItemBaseType(nMagIdx));  //属性的类型
	Lua_PushNumber(L,Item[nItemIdx].GetItemBaseMagic(nMagIdx));
	Lua_PushNumber(L,Item[nItemIdx].GetItemBaseMagicA(nMagIdx));
	Lua_PushNumber(L,Item[nItemIdx].GetItemBaseMagicB(nMagIdx));
	return 4;
}

int LuaGetItemRmagicAttrib(Lua_State *L)
{
	int nItemIdx = (int)Lua_ValueToNumber(L,1);
	int nMagIdx  = (int)Lua_ValueToNumber(L,2);
	if  (nItemIdx<=0 || nItemIdx>=MAX_ITEM) return 0;
	Lua_PushNumber(L,Item[nItemIdx].GetRongAttribType(nMagIdx));  //属性的类型
	Lua_PushNumber(L,Item[nItemIdx].GetRongAttribTypeVala(nMagIdx));
	Lua_PushNumber(L,Item[nItemIdx].GetRongAttribTypeValb(nMagIdx));
	Lua_PushNumber(L,Item[nItemIdx].GetRongAttribTypeValc(nMagIdx));
	return 4;
}
int LuaGetItemMagicAttrib(Lua_State *L)
{
    	int nItemIdx = (int)Lua_ValueToNumber(L,1);
		int nMagIdx  = (int)Lua_ValueToNumber(L,2);
		if  (nItemIdx<=0 || nItemIdx>=MAX_ITEM) return 0;
        Lua_PushNumber(L,Item[nItemIdx].GetAttribType(nMagIdx));  //属性的类型
        Lua_PushNumber(L,Item[nItemIdx].GetAttribTypeVala(nMagIdx));
		Lua_PushNumber(L,Item[nItemIdx].GetAttribTypeValb(nMagIdx));
		Lua_PushNumber(L,Item[nItemIdx].GetAttribTypeValc(nMagIdx));
		return 4;
}

int LuaSetMagicAttrib(Lua_State * L)
{
/*
	int nPlayerIndex = GetPlayerIndex(L);
	
	if (nPlayerIndex <= 0)
		return 0;
	
	int nParamNum = Lua_GetTopIndex(L);

	if (nParamNum < 18)
		return 0;

	int nIndex = (int)Lua_ValueToNumber(L, 1);

	#define 	MAX_ITEM_MAGICATTRIB		6

	if (nIndex > 0 && Item[nIndex].GetGenre() == item_equip)
	{
		int i = 0, k = 2;
		for (i = 0; i < MAX_ITEM_MAGICATTRIB; i ++)
		{
			Item[nIndex].m_GeneratorParam.nGeneratorLevel[i] = (int)Lua_ValueToNumber(L, k);
			Item[nIndex].m_GeneratorParam.nGeneratorLevel[i + MAX_ITEM_MAGICATTRIB] = MAKELONG((int)Lua_ValueToNumber(L, k + 1), (int)Lua_ValueToNumber(L, k + 2));
			Item[nIndex].m_aryMagicAttrib[i].nAttribType = (int)Lua_ValueToNumber(L, k);
			Item[nIndex].m_aryMagicAttrib[i].nValue[0] = (int)Lua_ValueToNumber(L, k + 1);
			Item[nIndex].m_aryMagicAttrib[i].nValue[1] = -1;
			Item[nIndex].m_aryMagicAttrib[i].nValue[2] = (int)Lua_ValueToNumber(L, k + 2);
			k += 3;
		}
		for (NULL; i < MAX_ITEM_MAGICATTRIB; i ++)
		{
			Item[nIndex].m_GeneratorParam.nGeneratorLevel[i] = 0;
			Item[nIndex].m_GeneratorParam.nGeneratorLevel[i + MAX_ITEM_MAGICATTRIB] = 0;
			Item[nIndex].m_aryMagicAttrib[i].nAttribType = 0;
			Item[nIndex].m_aryMagicAttrib[i].nValue[0] = 0;
			Item[nIndex].m_aryMagicAttrib[i].nValue[1] = 0;
			Item[nIndex].m_aryMagicAttrib[i].nValue[2] = 0;
		}
		Player[nPlayerIndex].m_ItemList.SynPlayerItem(nIndex);
	}
	return 0;*/

/*
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		return 0;

	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 18)
		return 0;

	int nIndex = (int)Lua_ValueToNumber(L, 1);
	const int MAX_ITEM_MAGICATTRIB = 6;

	if (nIndex > 0 && Item[nIndex].GetGenre() == item_equip)
	{
		int k = 2;
		KItemNormalAttrib sMA[MAX_ITEM_MAGICATTRIB];
		ZeroMemory(sMA, sizeof(sMA));

		for (int i = 0; i < MAX_ITEM_MAGICATTRIB; ++i)
		{
			int nType = (int)Lua_ValueToNumber(L, k);
			int nValA = (int)Lua_ValueToNumber(L, k + 1); 
			int nValC = (int)Lua_ValueToNumber(L, k + 2);

			sMA[i].nAttribType = nType;
			sMA[i].nValue[0] = nValA;
			sMA[i].nValue[1] = -1;     
			sMA[i].nValue[2] = nValC;

			Item[nIndex].m_GeneratorParam.nGeneratorLevel[i] = nType;
			Item[nIndex].m_GeneratorParam.nGeneratorLevel[i + MAX_ITEM_MAGICATTRIB] = MAKELONG(nValA, nValC);

			k += 3;
		}

		for (int i = nParamNum / 3; i < MAX_ITEM_MAGICATTRIB; ++i)
		{
			sMA[i].nAttribType = 0;
			sMA[i].nValue[0] = 0;
			sMA[i].nValue[1] = 0;
			sMA[i].nValue[2] = 0;

			Item[nIndex].m_GeneratorParam.nGeneratorLevel[i] = 0;
			Item[nIndex].m_GeneratorParam.nGeneratorLevel[i + MAX_ITEM_MAGICATTRIB] = 0;
		}

		Item[nIndex].SetAttrib_MA(sMA);

		//Item[nIndex].GenerateMagicAttribFromGeneratorLevel();

		Player[nPlayerIndex].m_ItemList.SynPlayerItem(nIndex);
	}
	return 0;
*/
	int nParamNum = Lua_GetTopIndex(L);	
	if (nParamNum < 4)
		return 0;
	int nIndex = (int)Lua_ValueToNumber(L, 1);
	if (nIndex < 0 || nIndex >= MAX_ITEM) return 0;
	if(nIndex <= 0 || nIndex >= MAX_ITEM || Item[nIndex].GetID() == 0)
		return 0;
	int nAtRow = (int)Lua_ValueToNumber(L, 2);
	if(nAtRow <= 0 || nAtRow > 8 || nAtRow >= 6)
		return 0;
	if(nAtRow <= 6)
	{
		nAtRow--;
		Item[nIndex].m_aryMagicAttrib[nAtRow].nAttribType = (int)Lua_ValueToNumber(L, 3);
		Item[nIndex].m_aryMagicAttrib[nAtRow].nValue[0] = (int)Lua_ValueToNumber(L, 4);
		Item[nIndex].m_aryMagicAttrib[nAtRow].nValue[1] = -1;
		if(Item[nIndex].m_aryMagicAttrib[nAtRow].nAttribType == magic_addphysicsdamage_p)
			Item[nIndex].m_aryMagicAttrib[nAtRow].nValue[2] = 6;
		else if(nParamNum >= 5)
			Item[nIndex].m_aryMagicAttrib[nAtRow].nValue[2] = (int)Lua_ValueToNumber(L, 5);
		else
			Item[nIndex].m_aryMagicAttrib[nAtRow].nValue[2] = 0;
	}
/*	else
	{
		nAtRow -= 7;
		Item[nIndex].m_aryMagicAttribEx[nAtRow].nAttribType = (int)Lua_ValueToNumber(L, 3);
		Item[nIndex].m_aryMagicAttribEx[nAtRow].nValue[0] = (int)Lua_ValueToNumber(L, 4);
		Item[nIndex].m_aryMagicAttribEx[nAtRow].nValue[1] = -1;
		if(Item[nIndex].m_aryMagicAttribEx[nAtRow].nAttribType == magic_addphysicsdamage_p)
			Item[nIndex].m_aryMagicAttribEx[nAtRow].nValue[2] = 6;
		else if(nParamNum >= 5)
			Item[nIndex].m_aryMagicAttribEx[nAtRow].nValue[2] = (int)Lua_ValueToNumber(L, 5);
		else
			Item[nIndex].m_aryMagicAttribEx[nAtRow].nValue[2] = 0;
	}
*/
	return 0;
}

int LuaPlaceItem(Lua_State * L)
{
	int nParamNum = Lua_GetTopIndex(L);
	int nPlayerIndex = GetPlayerIndex(L);
	
	if (nPlayerIndex <= 0)
		return 0;
	
	if (nParamNum < 1)
		return 0;
	int nIndex = (int)Lua_ValueToNumber(L, 1);
	if (nIndex < 0 || nIndex >= MAX_ITEM) return 0;
	if(nIndex <= 0 || nIndex >= MAX_ITEM || Item[nIndex].GetID() == 0)
		return 0;
	
	int		x, y;
	if (Player[nPlayerIndex].m_ItemList.CheckCanPlaceInEquipment(Item[nIndex].GetWidth(), Item[nIndex].GetHeight(), &x, &y))
	{
		Player[nPlayerIndex].m_ItemList.Add(nIndex, pos_equiproom, x, y);
	}
	else
	{
		int		nIdx = Player[nPlayerIndex].m_ItemList.Hand();
		if (nIdx)
		{
			Player[nPlayerIndex].m_ItemList.Remove(nIdx);
			
			KMapPos sMapPos;
			KObjItemInfo	sInfo;
			
			Player[nPlayerIndex].GetAboutPos(&sMapPos);
			
			sInfo.m_nItemID = nIdx;
			sInfo.m_nItemWidth = Item[nIdx].GetWidth();
			sInfo.m_nItemHeight = Item[nIdx].GetHeight();
			sInfo.m_nMoneyNum = 0;
			snprintf(sInfo.m_szName, sizeof(sInfo.m_szName), "%s", Item[nIdx].GetName());
			sInfo.m_nColorID = 0;
			sInfo.m_nMovieFlag = 1;
			sInfo.m_nSoundFlag = 1;
			
			int nObj = ObjSet.Add(Item[nIdx].GetObjIdx(), sMapPos, sInfo);
			if (nObj >= 0)
			{
				if (Item[nIdx].GetGenre() == item_task)
				{
					Object[nObj].SetEntireBelong(nPlayerIndex);
				}
				else
				{
					Object[nObj].SetItemBelong(nPlayerIndex);
				}
			}
		}
		Player[nPlayerIndex].m_ItemList.Add(nIndex, pos_hand, 0 ,0);
	}
	return 0;
} 

//KTime*	GetTime() { return & m_CommonAttrib.LimitTime;};


int LuaGetItemGoldIdx(Lua_State *L)
{
	int nItemIdx = (int)Lua_ValueToNumber(L,1);
	if  (nItemIdx<=0 || nItemIdx>=MAX_ITEM) return 0;
	Lua_PushNumber(L,Item[nItemIdx].GetGoldId());
    return 1;
}

int LuaGetItemPlatinaIdx(Lua_State *L)
{
	int nItemIdx = (int)Lua_ValueToNumber(L,1);
	if  (nItemIdx<=0 || nItemIdx>=MAX_ITEM) return 0;
	Lua_PushNumber(L,Item[nItemIdx].GetIsPlatima());
	return 1;
}


int LuaSetItemPoint(Lua_State *L)
{
	int nItemIdx = (int)Lua_ValueToNumber(L,1);

	int nPoints = (int)Lua_ValueToNumber(L,2);
	if  (nItemIdx<=0 || nItemIdx>=MAX_ITEM)
		return 0;
	Item[nItemIdx].SetPoint(nPoints);

    return 0;
}


int  LuaSetItemRandomSeed(Lua_State *L)
{
	int nItemIdx = (int)Lua_ValueToNumber(L,1);
	
	if  (nItemIdx<=0 || nItemIdx>=MAX_ITEM)
		return 0;

	Item[nItemIdx].GetItemParam()->uRandomSeed = g_GetRandomSeed();

	return 0;
}

int  LuaSetItemRandomEx(Lua_State *L)
{
	int nItemIdx = (int)Lua_ValueToNumber(L,1);

	if  (nItemIdx<=0 || nItemIdx>=MAX_ITEM)
		return 0;

	Item[nItemIdx].GetItemParam()->uRandomSeed = g_GetRandomSeed();

	return 0;
}


int  LuaGetItemIsStack(Lua_State *L)
{
	int nItemIdx = (int)Lua_ValueToNumber(L,1);

	if  (nItemIdx<=0 || nItemIdx>=MAX_ITEM)
		return 0;

	Lua_PushNumber(L,Item[nItemIdx].IsStack());
	return 1;
}

int  LuaGetItemRandomEx(Lua_State *L)
{
	int nItemIdx = (int)Lua_ValueToNumber(L,1);

	if  (nItemIdx<=0 || nItemIdx>=MAX_ITEM)
		return 0;
	char nSeedStr[32]={0};
	sprintf(nSeedStr,"%u",Item[nItemIdx].GetItemParam()->uRandomSeed);
	Lua_PushString(L,nSeedStr);
	return 1;
}
int  LuaGetItemRandomSeed(Lua_State *L)
{
	int nItemIdx = (int)Lua_ValueToNumber(L,1);

	if  (nItemIdx<=0 || nItemIdx>=MAX_ITEM)
		return 0;
   // char temp[64];
	//sprintf(temp,"%u",Item[nItemIdx].GetItemParam()->uRandomSeed);
	Lua_PushNumber(L,Item[nItemIdx].GetItemParam()->uRandomSeed);
	return 1;
}

int LuaGetItemRpoint(Lua_State *L)
{
	int nItemIdx = (int)Lua_ValueToNumber(L,1);
	if  (nItemIdx<=0 || nItemIdx>=MAX_ITEM)
		return 0;
	Lua_PushNumber(L,Item[nItemIdx].IsRong());
	return 1;
}

int LuaGetItemPoint(Lua_State *L)
{
	int nItemIdx = (int)Lua_ValueToNumber(L,1);
	if  (nItemIdx<=0 || nItemIdx>=MAX_ITEM)
		return 0;
	Lua_PushNumber(L,Item[nItemIdx].GetPoint());
    return 1;
}

int LuaGetItemBaseInfo(Lua_State *L)
{
	int nItemIdx = (int)Lua_ValueToNumber(L,1);

	if  (nItemIdx<=0 || nItemIdx>=MAX_ITEM)	 return 0;

	Lua_PushNumber(L,Item[nItemIdx].GetGenre());
	Lua_PushNumber(L,Item[nItemIdx].GetDetailType());
	Lua_PushNumber(L,Item[nItemIdx].GetParticular());

    return 3;
}

//获取物品使用时间
int LuaGetItemTime(Lua_State *L)
{
	int nItemIdx = (int)Lua_ValueToNumber(L,1);
	if  (nItemIdx<=0 || nItemIdx>=MAX_ITEM)
		return 0;
	Lua_PushNumber(L,Item[nItemIdx].GetTime()->bYear);
	Lua_PushNumber(L,Item[nItemIdx].GetTime()->bMonth);
	Lua_PushNumber(L,Item[nItemIdx].GetTime()->bDay);
	Lua_PushNumber(L,Item[nItemIdx].GetTime()->bHour);
	Lua_PushNumber(L,Item[nItemIdx].GetTime()->bMin);
    return 5;
}

//获取物品的是否绑定
int LuaGetItemIsBang(Lua_State *L)
{
	int nItemIdx = (int)Lua_ValueToNumber(L,1);
	if  (nItemIdx<=0 || nItemIdx>=MAX_ITEM)
		return 0;
	Lua_PushNumber(L,Item[nItemIdx].GetIsBang());
    return 1;
}

//获取物品的加成  GetStackNum
int LuaGetItemEnChance(Lua_State *L)
{
	int nItemIdx = (int)Lua_ValueToNumber(L,1);
	if  (nItemIdx<=0 || nItemIdx>=MAX_ITEM)
		return 0;
	Lua_PushNumber(L,Item[nItemIdx].GetEnChance());
    return 1;
}

//设置物品的加成
int LuaSetItemEnChance(Lua_State *L)
{
	int nItemIdx = (int)Lua_ValueToNumber(L,1);
	int nItemEnChance = (int)Lua_ValueToNumber(L,2);
	int nIsSyn = (int)Lua_ValueToNumber(L,3);
	if  (nItemIdx<=0 || nItemIdx>=MAX_ITEM)
		return 0;

	int  nEnChance=Item[nItemIdx].GetEnChance();

	Item[nItemIdx].EnChance(nEnChance+nItemEnChance);

	int nPlayerIndex = GetPlayerIndex(L);

	if (nPlayerIndex > 0 && nIsSyn>=1)
	{
	   Player[nPlayerIndex].m_ItemList.SynPlayerItem(nItemIdx); //同步物品属
	}

    return 0;
}


//获取物品的叠加数量  GetStackNum
int LuaGetItemStackNum(Lua_State *L)
{
	int nItemIdx = (int)Lua_ValueToNumber(L,1);

	if  (nItemIdx<=0 || nItemIdx>=MAX_ITEM)	 return 0;

	Lua_PushNumber(L,Item[nItemIdx].GetStackNum());
    return 1;
}



//获取物品的五行
int LuaGetItemSeries(Lua_State *L)
{
	int nItemIdx = (int)Lua_ValueToNumber(L,1);

	if  (nItemIdx<=0 || nItemIdx>=MAX_ITEM)	 return 0;

	Lua_PushNumber(L,Item[nItemIdx].GetSeries());
    return 1;
}


//获取物品的名称
int LuaGetItemName(Lua_State *L)
{
	int nItemIdx = (int)Lua_ValueToNumber(L,1);
	if  (nItemIdx<=0 || nItemIdx>=MAX_ITEM)	 return 0;

	Lua_PushString(L,Item[nItemIdx].GetName());
    return 1;
}



int LuaGetItemWhere(Lua_State *L)
{
	int nItemIdx = (int)Lua_ValueToNumber(L,1);

	if  (nItemIdx<=0 || nItemIdx>=MAX_ITEM)	 return 0;

	Lua_PushNumber(L,Item[nItemIdx].GetIsWhere());
	return 1;
}

//获取物品的等级
int LuaGetItemLevel(Lua_State *L)
{
	int nItemIdx = (int)Lua_ValueToNumber(L,1);

	if  (nItemIdx<=0 || nItemIdx>=MAX_ITEM)	 return 0;

	Lua_PushNumber(L,Item[nItemIdx].GetLevel());
    return 1;
}
//获取物品的等级
int LuaGetItemLuck(Lua_State *L)
{
	int nItemIdx = (int)Lua_ValueToNumber(L,1);

	if  (nItemIdx<=0 || nItemIdx>=MAX_ITEM)	 return 0;
	//Item[nItemIdx].m_GeneratorParam.nLuck;
	Lua_PushNumber(L,Item[nItemIdx].m_GeneratorParam.nLuck);
	return 1;
}

/*HaveItem(nItemTemplateId);
功能:检查玩家是否有某个物品
*/
int LuaHaveItem(Lua_State *L)
{
	if (Lua_GetTopIndex(L) > 0)
	{
		int nPlayerIndex = GetPlayerIndex(L), nModel = 0;

		if (nPlayerIndex > 0)
		{
			int nEventId = 0;
			if (Lua_IsNumber(L, 1))
			{
				nEventId = (int)Lua_ValueToNumber(L,1);
				nModel = (int)Lua_ValueToNumber(L,2);
			}
			else
			{
				char * szEventItm = (char *)Lua_ValueToString(L,1);
				nModel = (int)Lua_ValueToNumber(L,2);

				if (nModel > 1)
                    if (!g_MineItemTab.GetInteger(szEventItm, "ParticularType", 0, &nEventId))
					{ 
						Lua_PushNumber(L,0);
						return 1;
					} 
				else// if (nModel==1)
				   if (!g_EventItemTab.GetInteger(szEventItm, "具体类别", 0, &nEventId))
				   { 
					 Lua_PushNumber(L,0);
					return 1;
				   } 
			}
			int nResult = Player[nPlayerIndex].m_ItemList.IsTaskItemExist(nEventId,nModel);

			Lua_PushNumber(L, (nResult != 0));
		}
		else 
		{
//			g_DebugLog("HaveItem nPlayerIndex <= 0");
			Lua_PushNumber(L,0);
		}
	}
	else
	{
//		g_DebugLog("HaveItem 参数不够!");
		Lua_PushNumber(L , 0);
	}

	return 1;
}


int LuaGetItemCountEx(Lua_State * L)
{
	if (Lua_GetTopIndex(L) <= 0)
	{
		Lua_PushNumber(L , 0);
	    return 1;
	}
	int nPlayerIndex = GetPlayerIndex(L);
	int nDetailType = (int)Lua_ValueToNumber(L,1);
	if (nPlayerIndex <= 0)
	{
		nPlayerIndex = (int)Lua_ValueToNumber(L,2);
		if (nPlayerIndex <= 0)
		{
			Lua_PushNumber(L , 0);
			return 1;
		}
	}

	if (nPlayerIndex < 0 || nPlayerIndex >= MAX_PLAYER) { Lua_PushNumber(L , 0); return 1; }
	int nCount = Player[nPlayerIndex].m_ItemList.GetItemCountEx(nDetailType);
	Lua_PushNumber(L, nCount);
	return 1;
}

/*
AddMagic(nPlayerIndex, nMagicID, nLevel)
DelMagic(nPlayerIndex, nMagicId)
HaveMagic(nPlayerIndex, nMagicId)
GetMagicLevel(nPlayerIndex, nMagicId)
SetMagicLevel(nPlayerIndex, nMagicId, nLevel)
ModifyMagicLevel(nPlayerIndex ,nMagicId, nDLevel)
*/
//增加技能
int LuaAddMagic(Lua_State * L)
{
	int nParamCount = Lua_GetTopIndex(L);
	int nPlayerIndex = 0;
	if (nParamCount < 1) return 0;
	     nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	int nSkillId = 0;
	if (Lua_IsNumber(L, 1))
	{
		nSkillId = (int)Lua_ValueToNumber (L, 1);
	}
	else 
	{
		const char * sSkillName = Lua_ValueToString(L, 1);
		g_OrdinSkillsSetting.GetInteger((char*)sSkillName, "SkillId", 0, &nSkillId);
		if (nSkillId <= 0 ) return 0;
	}


	/*if (Npc[Player[nPlayerIndex].m_nIndex].m_SkillList.FindSame(nSkillId))
	{//已经存在
		return 0;
	} */


	int nLevel = 0;
	if (nParamCount >=2)
		nLevel = (int)Lua_ValueToNumber(L,2);
	else
		nLevel = 0;
	
    int nRet = Npc[Player[nPlayerIndex].m_nIndex].m_SkillList.Add(nSkillId, nLevel);

	if (nRet)
	{//同步某个技能数据到客户端
		int  nListidx = Npc[Player[nPlayerIndex].m_nIndex].m_SkillList.FindSame(nSkillId);
		PLAYER_SKILL_LEVEL_SYNC NewSkill;
		NewSkill.ProtocolType  = s2c_playerskilllevel;
		NewSkill.m_nSkillID    = nSkillId;
		NewSkill.m_nSkillLevel = Npc[Player[nPlayerIndex].m_nIndex].m_SkillList.GetCurrentLevelByIdx(nListidx);
		NewSkill.m_nSkillExp = Npc[Player[nPlayerIndex].m_nIndex].m_SkillList.GetCurSkillExp(nListidx);//muc do luyen
		NewSkill.m_nLeavePoint = Player[nPlayerIndex].m_nSkillPoint;
		NewSkill.m_nAddPoint   = Npc[Player[nPlayerIndex].m_nIndex].m_SkillList.GetAddPoint(nListidx);
		g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, (BYTE*)&NewSkill, sizeof(PLAYER_SKILL_LEVEL_SYNC));
		//printf("---脚本更新[%s]技能:Id:%d,lv:%d,lp:%d,addp:%d----\n",Npc[Player[nPlayerIndex].m_nIndex].Name,nSkillId,NewSkill.m_nSkillLevel,NewSkill.m_nLeavePoint,NewSkill.m_nAddPoint);
	}
	
	return 0;
}

int LuaSetNpcSkill(Lua_State * L)
{
	int nNpcIdx= (int)Lua_ValueToNumber (L, 1);
	int nSkillIdx= (int)Lua_ValueToNumber (L, 2);
    int nSkillId= (int)Lua_ValueToNumber (L, 3);
	int nSkillLevel= (int)Lua_ValueToNumber (L, 4);

	if  (nNpcIdx<=0 || nNpcIdx>=MAX_NPC) return 0;

	if (Npc[nNpcIdx].m_SkillList.SetNpcSkill(nSkillIdx,nSkillId,nSkillLevel))
	    Npc[nNpcIdx].SetAuraSkill(nSkillId);

	return 0;
}

//指定删除NPC的第几技能
int LuaDelNpcSkills(Lua_State * L)
{
	int mSKillID=0;
	int nNpcIdx= (int)Lua_ValueToNumber (L, 1);
	int nSkillIdx= (int)Lua_ValueToNumber (L, 2);
	int nModel= (int)Lua_ValueToNumber (L, 3); //模式 1为删除第几个技能 2为直接删除技能的编号

	if (nNpcIdx<=0 || nNpcIdx>=MAX_NPC)	return 0;

    if (nModel==1)
	{
      mSKillID=Npc[nNpcIdx].GetSkillID(nSkillIdx);
	}
	else if (nModel>=2)
      mSKillID=nSkillIdx;

    if (mSKillID)
	{
		int nRet = 0;
	        nRet = Npc[nNpcIdx].m_SkillList.Remove(mSKillID);
	  
	  if (nRet && Npc[nNpcIdx].m_Kind==kind_player)
		  Player[Npc[nNpcIdx].GetnPlayerIdx()].SendSyncData_Skill();
	}
	return 0;
}

//删除技能（技能编号）
int LuaDelMagic(Lua_State * L)
{
	int nParamCount = Lua_GetTopIndex(L);
	int nPlayerIndex = 0;
	if (nParamCount < 1) return 0;
	
    nPlayerIndex = GetPlayerIndex(L);
	
	if (nPlayerIndex <= 0) 
		return 0;
	
	int nSkillId = 0;
	if (Lua_IsNumber(L, 1))
	{
		nSkillId = (int)Lua_ValueToNumber (L, 1);
	}
	else 
	{
		const char * sSkillName = Lua_ValueToString(L, 1);
		g_OrdinSkillsSetting.GetInteger((char*)sSkillName, "SkillId", 0, &nSkillId);
		if (nSkillId <= 0 ) return 0;
	}

	  int nRet = 0,nSkiiIdx = 0;

	       nSkiiIdx = Npc[Player[nPlayerIndex].m_nIndex].m_SkillList.FindSame(nSkillId);
	   KSkill * pSkill =(KSkill *)g_SkillManager.GetSkill(Npc[Player[nPlayerIndex].m_nIndex].m_SkillList.m_Skills[nSkiiIdx].SkillId, Npc[Player[nPlayerIndex].m_nIndex].m_SkillList.m_Skills[nSkiiIdx].SkillLevel);
	   
	   if (pSkill)
	       pSkill->Cast(Player[nPlayerIndex].m_nIndex, -1,Player[nPlayerIndex].m_nIndex,1);  //移除状态

		nRet = Npc[Player[nPlayerIndex].m_nIndex].m_SkillList.Remove(nSkillId);

	  if (nRet)
	  { 
		Player[nPlayerIndex].SendSyncData_Skill();
		/*PLAYER_SKILL_LEVEL_SYNC NewSkill;
		NewSkill.ProtocolType  = s2c_playerskilllevel;
		NewSkill.m_nSkillID    = nSkillId;
		NewSkill.m_nSkillLevel = Npc[Player[nPlayerIndex].m_nIndex].m_SkillList.GetCurrentLevel(nSkillId);
		NewSkill.m_nSkillExp = Npc[Player[nPlayerIndex].m_nIndex].m_SkillList.GetCurSkillExp(nSkillId);//muc do luyen
		NewSkill.m_nLeavePoint = Player[nPlayerIndex].m_nSkillPoint;
		NewSkill.m_nAddPoint   = Npc[Player[nPlayerIndex].m_nIndex].m_SkillList.GetAddPoint(Npc[Player[nPlayerIndex].m_nIndex].m_SkillList.FindSame(nSkillId));
		g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, (BYTE*)&NewSkill, sizeof(PLAYER_SKILL_LEVEL_SYNC));
	*/
	  } 
	//Player[nPlayerIndex].SendSyncData_Skill();
	Lua_PushNumber(L, nRet);
	return 1;
	
}

//删除技能（技能编号）
int LuaDelAllMagic(Lua_State * L)
{
	int nPlayerIndex = 0;
    nPlayerIndex = GetPlayerIndex(L);
	
	if (nPlayerIndex <= 0) 
		return 0;
		
	Npc[Player[nPlayerIndex].m_nIndex].m_SkillList.RemoveAllSkill();

	if (Npc[Player[nPlayerIndex].m_nIndex].m_Kind==kind_player)
	{
		Player[nPlayerIndex].SendSyncData_Skill();  
	}

	return 0;
	
}


int LuaHaveMagic(Lua_State * L)
{
	int nParamCount = Lua_GetTopIndex(L);
	int nPlayerIndex = 0;
	if (nParamCount < 1) return 0;
	
	nPlayerIndex = GetPlayerIndex(L);
	
	if (nPlayerIndex <= 0) return 0;
	
	int nSkillId = 0;
	if (Lua_IsNumber(L,1))
	{
		nSkillId = (int)Lua_ValueToNumber (L,1);
	}
	else 
	{
		const char * sSkillName = Lua_ValueToString(L, 1);
		g_OrdinSkillsSetting.GetInteger((char *)sSkillName, "SkillId", 0, &nSkillId);
		if (nSkillId <= 0 ) 
		{
			Lua_PushNumber(L, -1);
		}
		return 1;
	}
	int nListidx = 0;
	if (nListidx = Npc[Player[nPlayerIndex].m_nIndex].m_SkillList.FindSame(nSkillId))
	{
		Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_SkillList.GetLevelByListidx(nListidx));
	}
	else
	{
		Lua_PushNumber(L, -1);
	}
	
	return 1;
}

int LuaGetSkillIdInSkillList(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 1) 
		return 0;
	
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) 
		return 0;
	int nSkillIndex = (int)Lua_ValueToNumber (L, 1);
	int nSkillId = 0;
	if (nSkillIndex > 0) 
	{
		nSkillId = Npc[Player[nPlayerIndex].m_nIndex].m_SkillList.GetSkillId(nSkillIndex);
	}
	Lua_PushNumber(L, nSkillId);
	return 1;
}

int LuaSetCurrentSkillLevel(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 2) 
		return 0;
	
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) 
		return 0;
	int nSkillId = 0;
	
	if (Lua_IsNumber(L, 1))
	{
		nSkillId = (int)Lua_ValueToNumber (L, 1);
	}
	else 
	{
		const char * sSkillName = Lua_ValueToString(L, 1);
		g_OrdinSkillsSetting.GetInteger((char *)sSkillName, "SkillId", 0, &nSkillId);
		if (nSkillId <= 0 ) return 0;
	}
	int nLevel = (int)Lua_ValueToNumber(L, 2);
	if (nLevel >= 0)
		Npc[Player[nPlayerIndex].m_nIndex].m_SkillList.SetSkillLevelDirectlyUsingId(nSkillId, nLevel);

	return 0;
}

int  LuaServerExpLimit(Lua_State * L)
{
	int g_ExpLimit = 0;
	g_GameOption.GetInteger("GameOption", "ServerExpLimit", 1, &g_ExpLimit);

	Lua_PushNumber(L, g_ExpLimit);
	return 1;
}


//洗髓用的 返回技能点
int  LuaRollBackSkills(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) 
		return 0;

	int nIsNoExpSkill = (int)Lua_ValueToNumber(L, 1);	//是否不计算熟练度技能


	int nSkillId = 0;
	int nTotalSkillPoits = Npc[Player[nPlayerIndex].m_nIndex].m_SkillList.ClearSkillLevel(nIsNoExpSkill);
	Lua_PushNumber(L, nTotalSkillPoits);
	return 1;
}

//设置幸运星
int  LuaSetXinYunXing(Lua_State * L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    int nSillId = (int)Lua_ValueToNumber(L, 1);
	Npc[Player[nPlayerIndex].m_nIndex].m_ActiveXinYunXingID=nSillId;
	if  (nSillId>0 && nSillId<MAX_SKILL)
	   Npc[Player[nPlayerIndex].m_nIndex].m_ActiveXinYunXingIDIndex = Npc[Player[nPlayerIndex].m_nIndex].m_SkillList.FindSame(nSillId);
	else
		Npc[Player[nPlayerIndex].m_nIndex].m_ActiveXinYunXingIDIndex = 0;

	return 0;
}

int LuaUpdataSkills(Lua_State * L)
{
	//int nPlayerIndex = GetPlayerIndex(L);
	//KSkillList m_SkillList;
	//Npc[Player[nPlayerIndex].m_nIndex].m_SkillList;//m_Skills.
    //m_SkillList.IncreaseLevel();
	return 0;
}


int LuaUpdateSkillList(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) 
		return 0;
	Player[nPlayerIndex].SendSyncData_Skill();
	return 0;
}

//对自己使用技能
int LuaUseSkill(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	int nSillId = (int)Lua_ValueToNumber(L, 1);
    int nLevel = (int)Lua_ValueToNumber(L, 2);
    int	nMaxBeiLv= (int)Lua_ValueToNumber(L, 3);
	int nTimes = (int)Lua_ValueToNumber(L, 4);
//	int nModel = (int)Lua_ValueToNumber(L, 3); //1对自己 2对敌人
	if (nLevel<=0)
		nLevel=1;

	KSkill * pOrdinSkill = (KSkill *) g_SkillManager.GetSkill(nSillId, nLevel);
	
    int nSkillStyle=g_SkillManager.GetSkillStyleID(nSillId);//0为子弹 2 为主动 3 为被动
	
	 if (!pOrdinSkill)
	 {  
		return 0;
	 } 
	  /*if  (nSkillStyle!=0) //主动技能  //0为子弹 2 为主动 3 为被动
	  {
	  }
	  else
	  {
        pOrdinSkill->Cast(Player[nPlayerIndex].m_nIndex, -1,Player[nPlayerIndex].m_nIndex,nTimes,SKILL_SLT_Npc,nMaxBeiLv);//,20,SKILL_SLT_Npc
	    nReg=pOrdinSkill->CastSkill(Player[nPlayerIndex].m_nIndex,nSillId,-1,Npc[Player[nPlayerIndex].m_nIndex].m_dwID,nLevel,nTimes,nMaxBeiLv);  
	  }*/
	  Player[nPlayerIndex].UseToolSkills(nSillId,nLevel,Player[nPlayerIndex].m_nIndex,Player[nPlayerIndex].m_nIndex,NULL,1,nTimes,nMaxBeiLv);

	  return 0;
}


int LuaCastSkillToNpc(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) 
		return 0;

	int nSillId = (int)Lua_ValueToNumber(L, 1);
    int nLevel  = (int)Lua_ValueToNumber(L, 2);
    int	nX= (int)Lua_ValueToNumber(L, 3);
	int nY = (int)Lua_ValueToNumber(L, 4);
	//	int nModel = (int)Lua_ValueToNumber(L, 3); //1对自己 2对敌人
	int nReg = 0;
	KSkill * pOrdinSkill = (KSkill *) g_SkillManager.GetSkill(nSillId, nLevel);
	
    int nSkillStyle=g_SkillManager.GetSkillStyleID(nSillId);//0为子弹 2 为主动 3 为被动
	
	if (!pOrdinSkill) 
		return nReg;                                       //pSkill->Cast(nLauncher,-1,m_Index)

	if (pOrdinSkill->Cast(Player[nPlayerIndex].m_nIndex, nX,nY))//,20,SKILL_SLT_Np
	    nReg=pOrdinSkill->CastSkill(Player[nPlayerIndex].m_nIndex,nSillId,nX,nY,nLevel,-1);
	   
	Lua_PushNumber(L, nReg);
	return 1;
}



int LuaGetSkillStateTime(Lua_State * L)
{
	int nParamNum=Lua_GetTopIndex(L);
	int nPlayerIndex = GetPlayerIndex(L),nTime=-1;
	int nSillId      = (int)Lua_ValueToNumber(L, 1);

	if (nPlayerIndex > 0 && nParamNum<=1)
	{ 
	    nTime=Npc[Player[nPlayerIndex].m_nIndex].GetSkillLeftTime(nSillId);
	}
	else
	{
		int nNpcIdx=(int)Lua_ValueToNumber(L, 2);
		if (nNpcIdx < 0 || nNpcIdx >= MAX_NPC) { Lua_PushNumber(L,0); return 1; }
		nTime=Npc[nNpcIdx].GetSkillLeftTime(nSillId);
	}
	Lua_PushNumber(L,nTime);
	return 1;

}

int LuaGetSkillBaseInfo(Lua_State * L)
{
	int nSillId    = (int)Lua_ValueToNumber(L, 1);
	KSkill *pSkill = (KSkill*)g_SkillManager.GetSkill(nSillId,1);
	if (pSkill)
	{
		Lua_PushNumber(L,pSkill->GetSkillStyle()); //J技能的类型     
		Lua_PushNumber(L,pSkill->IsAura());        //是否是光环
		Lua_PushNumber(L,pSkill->IsMagic());       //内外攻
		Lua_PushNumber(L,pSkill->GetAttackRadius());//攻击距离
	}
	else
	{
		Lua_PushNumber(L,-1);      
		Lua_PushNumber(L,-1);
		Lua_PushNumber(L,-1);
		Lua_PushNumber(L,-1);
	}

	return 4; 
}

int LuaGetSkillName(Lua_State * L)
{
	//int nPlayerIndex = GetPlayerIndex(L);
	//if (nPlayerIndex <= 0) return 0;
	int nSillId    = (int)Lua_ValueToNumber(L, 1);
    KSkill *pSkill = (KSkill*)g_SkillManager.GetSkill(nSillId,1);
	if (pSkill)
	{
		Lua_PushString(L,pSkill->GetSkillName());    
	}
	else
        Lua_PushString(L,"暂无数据");
     
	return 1;  
}
//给自己加状态
int LuaAddSkillState(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	int nSillId=0,nSkillLevel=0,nIfMagic=0,nTime=0;
	if (Lua_GetTopIndex(L) < 4)
		return 0;

    KMagicAttrib sMa[1];

	nSillId    = (int)Lua_ValueToNumber(L, 1);
	nSkillLevel= (int)Lua_ValueToNumber(L, 2);
	nIfMagic   = (int)Lua_ValueToNumber(L, 3);
	nTime      = (int)Lua_ValueToNumber(L, 4);

   if (Lua_GetTopIndex(L)>=8)
   {	 
	 sMa[0].nAttribType = (int)Lua_ValueToNumber(L, 5);  //属性类型 状态的id
	 sMa[0].nValue[0] = (int)Lua_ValueToNumber(L, 6);    //第一个值
	 sMa[0].nValue[1] = (int)Lua_ValueToNumber(L, 7);    //第二个值
	 sMa[0].nValue[2] = (int)Lua_ValueToNumber(L, 8);    //第三个值
   } 

/*	if (nTime < -1)
	{
		nTime = 10;
	}
	
	KSkill *pSkill = (KSkill*)g_SkillManager.GetSkill(nSillId,nSkillLevel);

	if (nIfMagic)
	{//使用系统的 数据
		pSkill->CastStateSkill(Player[nPlayerIndex].m_nIndex,0,0,nTime);  //是否
	}
	else //直接使用数据
		Npc[Player[nPlayerIndex].m_nIndex].SetStateSkillEffect(Player[nPlayerIndex].m_nIndex,nSillId,nSkillLevel,sMa,1,nTime);  //设置技能光环效果
	*/
   if (Lua_GetTopIndex(L)>=8)
   {
	   Player[nPlayerIndex].UseToolSkills(nSillId,nSkillLevel,Player[nPlayerIndex].m_nIndex,Player[nPlayerIndex].m_nIndex,sMa,0,nTime);
   }
   else
   {
	   Player[nPlayerIndex].UseToolSkills(nSillId,nSkillLevel,Player[nPlayerIndex].m_nIndex,Player[nPlayerIndex].m_nIndex,NULL,1,nTime);
   }
	

/*
    KMagicAttrib sMa[1];
	sMa[0].nAttribType = magic_expenhance_p;  //技能的类型 ==杀死敌人获得经验==
	sMa[0].nValue[0] = nSub;   // 百分比值
	sMa[0].nValue[1] = 0;
	sMa[0].nValue[2] = 0;
	Npc[Player[nPlayerIndex].m_nIndex].SetStateSkillEffect(Player[nPlayerIndex].m_nIndex,372,1,sMa,1,nTime);   //372 为技能ID 1 为等级  时间
*/
	return 0;
}


//可以与 addSkillstate  对应使用
int LuaDelSkillState(Lua_State * L)
{
	
	int nSillId = (int)Lua_ValueToNumber(L, 1);
	int m_nIndex=0;
	char *nName=NULL;
	//	int m_nIndex= (int)Lua_ValueToNumber(L, 2);
	if (Lua_IsNumber(L,2))
		m_nIndex= (int)Lua_ValueToNumber(L, 2);
	else
	{
		nName=(char *)Lua_ValueToString(L, 2);
		m_nIndex=NpcSet.SearchName(nName);
	}

	int nIfMagic= (int)Lua_ValueToNumber(L, 3);
	KSkill *pSkill = (KSkill*)g_SkillManager.GetSkill(nSillId,1);
	KMagicAttrib sMa[1];
	if (nIfMagic)
	{
		pSkill->CastStateSkill(m_nIndex,0,0,1);  //是否
	}
	else
		Npc[m_nIndex].SetStateSkillEffect(m_nIndex,nSillId,1,NULL,0,1);  //设置技能光环效果
	
	Lua_PushNumber(L, m_nIndex);
	
	return 1;
}

int LuaCastSkill(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	
	if (Lua_GetTopIndex(L) < 5)
		return 0;
	
	int nSkillId = (int)Lua_ValueToNumber(L, 1);
	int nSkillLevel = (int)Lua_ValueToNumber(L, 2);
	int nTime	= (int)Lua_ValueToNumber(L, 3);
	int IsDel      = (int)Lua_ValueToNumber(L, 4);
	if (nSkillId < MAX_SKILL && nSkillLevel < MAX_SKILLLEVEL)

	if (nSkillLevel<=0)
		nSkillLevel=1;

	KSkill * pOrdinSkill = (KSkill *) g_SkillManager.GetSkill(nSkillId, nSkillLevel);
	
    int nSkillStyle=g_SkillManager.GetSkillStyleID(nSkillId);//0为子弹 2 为主动 3 为被动
	
	 if (!pOrdinSkill)
	 {  
		return 0;
	 } 
	  Player[nPlayerIndex].UseToolSkills(nSkillId,nSkillLevel,Player[nPlayerIndex].m_nIndex,Player[nPlayerIndex].m_nIndex,NULL,1,IsDel,nTime);


	return 0;
}



//是否限制回城
int LuaDisabledUseTownP(Lua_State * L)
{	
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;

	int isVer = (int)Lua_ValueToNumber(L,1);

	Player[nPlayerIndex].setForBitUseTownPortal(isVer);

	return 0;
}

int LuaForBitUseTownPortal(Lua_State * L)
{	
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;

	int isVer = (int)Lua_ValueToNumber(L,1);

	Lua_PushNumber(L,Player[nPlayerIndex].getForBitUseTownPortal());
	return 1;
}
//禁止修改pk模式
int LuaForbidChangePK(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	int isVer = (int)Lua_ValueToNumber(L,1);
	Player[nPlayerIndex].m_cPK.setForbitChange(isVer);
	return 0;
}
//禁止仇杀
int LuaForbidEnmity(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	int isVer = (int)Lua_ValueToNumber(L,1);
	Player[nPlayerIndex].__forbitEnmity = isVer;
	return 0;
}
//死亡模式 惩罚模式
int LuaSetDeathType(Lua_State * L)
{
	return LuaSetDeathPunish(L);
}
//禁止交易
int LuaForbitTrade(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	int isVer = (int)Lua_ValueToNumber(L,1);
	Player[nPlayerIndex].setforBitTrade(isVer);
	return 0;
}


int LuasetSendItemInfo(Lua_State * L)
{
	int nNpcIdx=(int)Lua_ValueToNumber (L, 1);
	int ver=(int)Lua_ValueToNumber (L, 2);

	if  (nNpcIdx<=0 || nNpcIdx>=MAX_NPC) return 0;

    Npc[nNpcIdx].m_IsSendItemInfo = ver;

    return 0;
}

int LuaSetLock(Lua_State * L)
{
	int nNum = (int)Lua_ValueToNumber(L,1);
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
	{
		Lua_PushNumber(L,0);
		return 1;
	}

	if(nNum == 0)
	{	
		Player[nPlayerIndex].SetChestLock(FALSE);
		return 1;
	}
	else
	{
		Player[nPlayerIndex].SetChestLock(TRUE);
		return 1;
	}
	return 1;
}

int LuaCheckLockPw(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
	{
		Lua_PushNumber(L,0);
		return 1;
	}
	if(Player[nPlayerIndex].m_nChestPW > 0)
	{	
		Lua_PushNumber(L,1);
		return 1;
	}
	else
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	return 1;
}

int LuaCheckLock(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
	{
		Lua_PushNumber(L,0);
		return 1;
	}
	if(Player[nPlayerIndex].m_CUnlocked == 1)
	{	
		Lua_PushNumber(L,1);
		return 1;
	}
	else
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	return 1;
}

//检测包袱空间
int LuaCheckBox(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	int Weight = 6;
	int Height = 10;
	if (Lua_GetTopIndex(L) >= 2)
	{
		Weight = (int)Lua_ValueToNumber(L,1);
		Height = (int)Lua_ValueToNumber(L,2);
	}
	if (nPlayerIndex <= 0)
	{
		Lua_PushNumber(L,0);
		return 1;
	}

	int x,y;
	if (Player[nPlayerIndex].m_ItemList.CheckCanPlaceInEquipment(Weight, Height, &x, &y))
	{	
		Lua_PushNumber(L,1);
		return 1;
	}
	else
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	return 1;
}


int LuaGetNpcSkillInfo(Lua_State * L)
{
	int nNpcIdx=(int)Lua_ValueToNumber (L, 1);
	int nSkiiID=(int)Lua_ValueToNumber (L, 2);

	if  (nNpcIdx<=0 || nNpcIdx>=MAX_NPC)
	{
		Lua_PushNumber(L,0);
		Lua_PushNumber(L,0);
		Lua_PushNumber(L,0);
		return 3;
	}
	int nListidx = Npc[nNpcIdx].m_SkillList.FindSame(nSkiiID);
	if (nListidx)
	{
		Lua_PushNumber(L,Npc[nNpcIdx].m_SkillList.IsSkillExpByListidx(nListidx));  //是否是 熟练度技能
		Lua_PushNumber(L,Npc[nNpcIdx].m_SkillList.GetSkillExpByListidx(nListidx)); //总熟练度
		Lua_PushNumber(L,Npc[nNpcIdx].m_SkillList.GetCurSkillExpByListidx(nListidx)); //当前熟练度
	}
	else
	{
		Lua_PushNumber(L,0);  //是否是 熟练度技能
		Lua_PushNumber(L,0); //总熟练度
		Lua_PushNumber(L,0); //当前熟练度
	}

	return 3;
}

int LuaAddOneSkillExpPoint(Lua_State * L)
{

	int nNpcIdx=(int)Lua_ValueToNumber (L,1);
	int nFigthSkillId = (int)Lua_ValueToNumber (L,2);
	int _nExpPonit=(int)Lua_ValueToNumber (L,3);
	int nKind=(int)Lua_ValueToNumber (L,4);

	bool _type=false;

	if (nKind)
		_type=true;
	else
        _type=false;

	int nPlayerIndex=0;

	if (nNpcIdx<=0 || nNpcIdx>=MAX_NPC)
	{
		nPlayerIndex = GetPlayerIndex(L);
		if (nPlayerIndex <= 0)
		   return 0;
		nNpcIdx=Player[nPlayerIndex].m_nIndex;
		if (nNpcIdx<=0 || nNpcIdx>=MAX_NPC)
		   return 0;
	}

	int nListidx = 0;
	if (Npc[nNpcIdx].m_Kind==kind_player)
	{
		nListidx = Npc[nNpcIdx].m_SkillList.FindSame(nFigthSkillId);
		if (!nListidx) return 0;
		Npc[nNpcIdx].m_SkillList.AddExpSkillPonitByListidxx(nListidx,_nExpPonit,_type); //熟练度技能点加1
	}

	return 0;
}



int LuaSetOneSkillExpInfo(Lua_State * L)
{
	int nNpcIdx=(int)Lua_ValueToNumber (L,1);
	int nFigthSkillId = (int)Lua_ValueToNumber (L,2);
	int nExpSkill=(int)Lua_ValueToNumber (L,3);
	int nMaxLevel=(int)Lua_ValueToNumber (L,4);
	int nAddExpPonit=(int)Lua_ValueToNumber (L,5);


	int nReg=0,nLevel;

	if (nMaxLevel<=0)
	{
		Lua_PushNumber(L,nReg);
		return 1;
	}

	int nIsExpSKill=0,nSkillExp=0,nCurSkillExp=0,nCurSkillLevel=0;

	if (nNpcIdx < 0 || nNpcIdx >= MAX_NPC) return 0;
	int nPlayerIndex=Npc[nNpcIdx].GetPlayerIdx();
	int nListidx = 0;
	if (Npc[nNpcIdx].m_Kind==kind_player)
	{
	 // nFigthSkillId=Player[nPlayerIndex].GetLeftSkill();
	    nListidx = Npc[nNpcIdx].m_SkillList.FindSame(nFigthSkillId);

	  if (!nListidx) return 0;

	   nIsExpSKill=Npc[nNpcIdx].m_SkillList.IsSkillExpByListidx(nListidx);

      if  (nIsExpSKill<=0)
	  {
		  Lua_PushNumber(L,nReg);
		  return 1;
	  }
	  if (nFigthSkillId>0)
	  {
		  // nFigthSkillId,nIsExpSKill,nSkillExp,nCurSkillExp  RollbackSkill
		   nSkillExp=Npc[nNpcIdx].m_SkillList.GetSkillExpByListidx(nListidx); //总熟练度
		   nCurSkillExp=Npc[nNpcIdx].m_SkillList.GetCurSkillExpByListidx(nListidx);
		   nCurSkillLevel=Npc[nNpcIdx].m_SkillList.GetLevelByListidx(nListidx);//实际的等级

		   if (nSkillExp<=0)
		   {
			  Lua_PushNumber(L,nReg);
			  return 1;
		   }

		   if  (nIsExpSKill==1 && nCurSkillLevel<nMaxLevel && nMaxLevel<MAX_SKILLLEVEL)
		   {//小于最大等级 就增加 熟练度
			   if (nSkillExp>nCurSkillExp)
			   {
				   if (Npc[nNpcIdx].m_SkillList.AddSkillExpByListidx(nListidx,nExpSkill,1))
				       Player[nPlayerIndex].SendSyncData_Skill();
			   }
			   else
			   {
				  // nLevel=nCurSkillLevel+1;
				   
				   Npc[nNpcIdx].m_SkillList.SetSLevelByListidx(nListidx,1,TRUE); //累加

				  if  (nAddExpPonit)
					  Npc[nNpcIdx].m_SkillList.AddExpSkillPonitByListidxx(nListidx,1,TRUE); //熟练度技能点加1

				   nLevel =  Npc[nNpcIdx].m_SkillList.GetLevelByListidx(nListidx);

					KSkill* Skill = (KSkill*)g_SkillManager.GetSkill(nFigthSkillId, nLevel);

					if (Skill)
					{
						Npc[nNpcIdx].m_SkillList.SetSkillExpByListidx(nListidx, Skill->GetSkillExp()); //设置总熟练度
						Npc[nNpcIdx].m_SkillList.AddSkillExpByListidx(nListidx, 0, 0);                  //设置当前熟练度为零 
						Player[nPlayerIndex].SendSyncData_Skill();                                //同步到客户端

						char nMsg[125] = { 0 };
						sprintf(nMsg, "恭喜:<c=cyan>%s<c>等级上升一级!", Skill->GetSkillName());
						int nLeng = TEncodeText(nMsg, strlen(nMsg));
						const char* szMsg = nMsg;
						if (szMsg)
							KPlayerChat::SendSystemInfo(1, nPlayerIndex, MESSAGE_SYSTEM_TONGZHI_HEAD, (char*)szMsg, nLeng);//strlen(szMsg)
					}

					Skill = NULL;
				}
				nReg = nFigthSkillId;
			}
		}
	}

   Lua_PushNumber(L,nReg);

  return 1;
}

int LuaSetSkillExpInfo(Lua_State * L)
{

	int nNpcIdx=(int)Lua_ValueToNumber(L,1);
	int nExpSkill=(int)Lua_ValueToNumber(L,2);
	int nMaxLevel=(int)Lua_ValueToNumber(L,3);
	//int nType    = (int)Lua_ValueToNumber (L,4);

	int nReg=0,nLevel;
	int nFigthSkillId=0,nIsExpSKill=0,nSkillExp=0,nCurSkillExp=0,nCurSkillLevel=0;

	if (nNpcIdx < 0 || nNpcIdx >= MAX_NPC) { Lua_PushNumber(L,0); return 1; }
	if (Npc[nNpcIdx].m_Kind==kind_player)
	{
	  if (nNpcIdx < 0 || nNpcIdx >= MAX_NPC) { Lua_PushNumber(L,0); return 1; }
	  int nPlayerIndex=Npc[nNpcIdx].GetPlayerIdx();

	  nFigthSkillId=Player[nPlayerIndex].GetLeftSkill();
	  if (nNpcIdx < 0 || nNpcIdx >= MAX_NPC) { Lua_PushNumber(L,0); return 1; }
	  int nListidx = Npc[nNpcIdx].m_SkillList.FindSame(nFigthSkillId);
	  if  (!nListidx)
	  {
		  Lua_PushNumber(L,nReg);
	      return 1;
	  }

	  if (nFigthSkillId>0)
	  {
		  // nFigthSkillId,nIsExpSKill,nSkillExp,nCurSkillExp  RollbackSkill
		  nIsExpSKill=Npc[nNpcIdx].m_SkillList.IsSkillExpByListidx(nListidx);
		  nSkillExp=Npc[nNpcIdx].m_SkillList.GetSkillExpByListidx(nListidx); //总熟练度
		  nCurSkillExp=Npc[nNpcIdx].m_SkillList.GetCurSkillExpByListidx(nListidx);
		  nCurSkillLevel=Npc[nNpcIdx].m_SkillList.GetLevelByListidx(nListidx);//实际的等级

		   if (nSkillExp<=0)
		   {
			  Lua_PushNumber(L,nReg);
			  return 1;
		   }

		   if  (nIsExpSKill==1 && nCurSkillLevel<nMaxLevel && nMaxLevel<MAX_SKILLLEVEL)
		   {//小于最大等级 就增加 熟练度
			   if (nSkillExp>nCurSkillExp)
			   {
				   if (Npc[nNpcIdx].m_SkillList.AddSkillExpByListidx(nListidx,nExpSkill,1))
				   {
					   /*nSkillExp=Npc[nNpcIdx].m_SkillList.GetSkillExp(nFigthSkillId);
					   nCurSkillExp=Npc[nNpcIdx].m_SkillList.GetCurSkillExp(nFigthSkillId);

					   if (nCurSkillExp>=nSkillExp)
					   {//技能开始升级
						   nLevel=nCurSkillLevel+1;
						   
						   Npc[nNpcIdx].m_SkillList.SetSLevel(nFigthSkillId,nLevel); //SetSkillLevel
						   
						   KSkill *Skill = (KSkill*)g_SkillManager.GetSkill(nFigthSkillId,nLevel);
						   
						   if (Skill)
						   {
							   Npc[nNpcIdx].m_SkillList.SetSkillExp(nFigthSkillId,Skill->GetSkillExp()); //设置总熟练度
							   Npc[nNpcIdx].m_SkillList.AddSkillExp(nFigthSkillId,0,0);                  //设置当前熟练度为零 
							   
							   //Player[nPlayerIndex].SendSyncData_Skill();  //同步到客户端
							   
							   char nMsg[125];
							   sprintf(nMsg,"恭喜:<color=yellow>%s<color>等级上升一级!",Skill->GetSkillName());
							   int nLeng  = TEncodeText(nMsg, strlen(nMsg));
							   const char *  szMsg = nMsg;
							   if (szMsg)
							   {  
								   KPlayerChat::SendSystemInfo(1, nPlayerIndex, MESSAGE_SYSTEM_TONGZHI_HEAD, (char *) szMsg, nLeng );//strlen(szMsg)
							   } 
						   } 
						   
				           Skill=NULL; 
				   }*/

				   Player[nPlayerIndex].SendSyncData_Skill();

				   }
			   }
			   else
			   {
				  // nLevel=nCurSkillLevel+1;
				   
				   Npc[nNpcIdx].m_SkillList.SetSLevelByListidx(nListidx,1,TRUE); //累加

				   nLevel =  Npc[nNpcIdx].m_SkillList.GetLevelByListidx(nListidx);

					KSkill* Skill = (KSkill*)g_SkillManager.GetSkill(nFigthSkillId, nLevel);

					if (Skill)
					{
						Npc[nNpcIdx].m_SkillList.SetSkillExpByListidx(nListidx, Skill->GetSkillExp()); //设置总熟练度
						Npc[nNpcIdx].m_SkillList.AddSkillExpByListidx(nListidx, 0, 0);                  //设置当前熟练度为零 
						Player[nPlayerIndex].SendSyncData_Skill();                                //同步到客户端

						char nMsg[125] = { 0 };
						sprintf(nMsg, "恭喜:<color=yellow>%s<color>等级上升一级!", Skill->GetSkillName());
						int nLeng = TEncodeText(nMsg, strlen(nMsg));
						const char* szMsg = nMsg;
						if (szMsg)
						{
							KPlayerChat::SendSystemInfo(1, nPlayerIndex, MESSAGE_SYSTEM_TONGZHI_HEAD, (char*)szMsg, nLeng);//strlen(szMsg)
						}
					}

					Skill = NULL;
				}
				nReg = nFigthSkillId;
			}
		}
	}

   Lua_PushNumber(L,nReg);

  return 1;
}

int LuaGetOneSkillInfo(Lua_State * L)
{
	int nPlayerIndex = 0;	
	    nPlayerIndex = GetPlayerIndex(L);

	if (nPlayerIndex <= 0)
	{
		Lua_PushNumber(L,0);
		Lua_PushNumber(L,0);
		Lua_PushNumber(L,0);
		Lua_PushNumber(L,0);
		return 4;										  
	}

	int nSkiiID=(int)Lua_ValueToNumber(L,1);//Player[nPlayerIndex].GetLeftSkill();

	if (nSkiiID<=0 || nSkiiID>=MAX_SKILL)
	{
		Lua_PushNumber(L,0);
		Lua_PushNumber(L,0);
		Lua_PushNumber(L,0);
		Lua_PushNumber(L,0);
		return 4;
	}
	int nListidx = Npc[Player[nPlayerIndex].m_nIndex].m_SkillList.FindSame(nSkiiID);
	if (nListidx)
	{
		Lua_PushNumber(L,Npc[Player[nPlayerIndex].m_nIndex].m_SkillList.IsSkillExpByListidx(nListidx));
		Lua_PushNumber(L,Npc[Player[nPlayerIndex].m_nIndex].m_SkillList.GetSkillExpByListidx(nListidx));
		Lua_PushNumber(L,Npc[Player[nPlayerIndex].m_nIndex].m_SkillList.GetCurSkillExpByListidx(nListidx));
		Lua_PushNumber(L,nListidx);
	}
	else
	{
		Lua_PushNumber(L,0);
		Lua_PushNumber(L,0);
		Lua_PushNumber(L,0);
		Lua_PushNumber(L,0);
	}
	return 4;
}


int LuaGetLeftSkillInfo(Lua_State * L)
{
	int nPlayerIndex = 0;	
	    nPlayerIndex = GetPlayerIndex(L);
		
	if (nPlayerIndex <= 0)
	{
	   nPlayerIndex=(int)Lua_ValueToNumber(L, 1);	
	}
	if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
	{
		Lua_PushNumber(L,0);
		Lua_PushNumber(L,0);
		Lua_PushNumber(L,0);
        Lua_PushNumber(L,0);
		return 4;										  
	}

     int nSkiiID=Player[nPlayerIndex].GetLeftSkill();
	 int nListidx = Npc[Player[nPlayerIndex].m_nIndex].m_SkillList.FindSame(nSkiiID);
	 if (!nListidx) return 0;

     Lua_PushNumber(L,nSkiiID);
	 Lua_PushNumber(L,Npc[Player[nPlayerIndex].m_nIndex].m_SkillList.IsSkillExpByListidx(nListidx));
	 Lua_PushNumber(L,Npc[Player[nPlayerIndex].m_nIndex].m_SkillList.GetSkillExpByListidx(nListidx));
     Lua_PushNumber(L,Npc[Player[nPlayerIndex].m_nIndex].m_SkillList.GetCurSkillExpByListidx(nListidx));
	 return 4;
}


int LuaAddNpcSkillExp(Lua_State * L)
{
	int nParamCount = Lua_GetTopIndex(L);
	
	if (nParamCount < 1) return 0;
	
	int nSkillId = 0,nCurSkillExp=0,nType=0;
	
	if (Lua_IsNumber(L, 1))
	{
		nSkillId = (int)Lua_ValueToNumber (L, 1);
	}
	else 
	{
		const char * sSkillName = Lua_ValueToString(L, 1);
		g_OrdinSkillsSetting.GetInteger((char *)sSkillName, "SkillId", 0, &nSkillId);
		if (nSkillId <= 0 ) return 0;
	}

	nCurSkillExp=(int)Lua_ValueToNumber (L, 2);
	nType=(int)Lua_ValueToNumber (L, 3);
	int nNpcIdx=(int)Lua_ValueToNumber (L, 4);

	if (nNpcIdx < 0 || nNpcIdx >= MAX_NPC) return 0;
	Npc[nNpcIdx].m_SkillList.AddSkillExp(nSkillId,nCurSkillExp,nType);
	//Player[nPlayerIndex].SendSyncData_Skill();
	
	return 0;
}

int LuaAddSkillExp(Lua_State * L)
{
	int nParamCount = Lua_GetTopIndex(L);
	int nPlayerIndex = 0;
	
	if (nParamCount < 1) return 0;
	   nPlayerIndex = GetPlayerIndex(L);
	
	if (nPlayerIndex <= 0) return 0;
	
	int nSkillId = 0,nCurSkillExp=0,nType=0;

	if (Lua_IsNumber(L, 1))
	{
		nSkillId = (int)Lua_ValueToNumber (L, 1);
	}
	else 
	{
		const char * sSkillName = Lua_ValueToString(L, 1);
		g_OrdinSkillsSetting.GetInteger((char *)sSkillName, "SkillId", 0, &nSkillId);
		if (nSkillId <= 0 ) return 0;
	}

     nCurSkillExp=(int)Lua_ValueToNumber (L, 2);
     nType=(int)Lua_ValueToNumber (L, 3);

	if (Npc[Player[nPlayerIndex].m_nIndex].m_SkillList.AddSkillExp(nSkillId,nCurSkillExp,nType))
	     Player[nPlayerIndex].SendSyncData_Skill();

	return 0;
}

int LuaTestMaxLife(Lua_State * L)
{
	int nParamCount = Lua_GetTopIndex(L);
	int nPlayerIndex = 0;
	
	   nPlayerIndex = GetPlayerIndex(L);
	
	if (nPlayerIndex <= 0) return 0;
	

	Npc[Player[nPlayerIndex].m_nIndex].m_CurrentLifeMax = 30000000000;
	     Player[nPlayerIndex].SendSyncData_Skill();

	return 0;
}


int LuaGetMaxSkillLevel_p(Lua_State * L)
{
	//int nParamCount = Lua_GetTopIndex(L);
	int nPlayerIndex = 0;
	int nSikkIdx =0;
/*
	if (nParamCount < 2)//没有参数
	{
	   nPlayerIndex = GetPlayerIndex(L);
	  // nSikkIdx = (int)Lua_ValueToNumber (L, 1);
	   if (Lua_IsNumber(L, 1))
	   {
		   nSikkIdx = (int)Lua_ValueToNumber(L, 1);
	   }
	   else 
	   {
		   const char * sSkillName = Lua_ValueToString(L, 1);
		   g_OrdinSkillsSetting.GetInteger((char *)sSkillName, "SkillId", 0, &nSikkIdx);
	   }
	}
	else //有参数
	{
	   nPlayerIndex = (int)Lua_ValueToNumber(L, 1);

	   if (Lua_IsNumber(L, 2))
	   {
		   nSikkIdx = (int)Lua_ValueToNumber(L, 2);
	   }
	   else 
	   {
		   const char * sSkillName = Lua_ValueToString(L, 2);
		   g_OrdinSkillsSetting.GetInteger((char *)sSkillName, "SkillId", 0, &nSikkIdx);
	   }
	}
	*/

	nPlayerIndex = (int)Lua_ValueToNumber(L, 1);

	if (Lua_IsNumber(L, 2))
		nSikkIdx = (int)Lua_ValueToNumber(L, 2);
	else 
	{
		const char * sSkillName = (char *)Lua_ValueToString(L, 2);
		if (sSkillName)
		  g_OrdinSkillsSetting.GetInteger((char *)sSkillName, "SkillId", 0, &nSikkIdx);
		else
		{
			Lua_PushNumber(L,0);
			return 1;
		}
	}

	if (nPlayerIndex <= 0 || nPlayerIndex>=MAX_PLAYER)
	{
		Lua_PushNumber(L,0);
		return 1;
	}

	if (nSikkIdx<=0 || nSikkIdx>=MAX_SKILL)
	{
		Lua_PushNumber(L,0);
		return 1;
	}
	//GetSkillMaxLevel
	int  nTempPoit = /*Player[nPlayerIndex].m_cVip.GetRoleVipNum()**/Player[nPlayerIndex].m_cVip.GetRoleVipSkillMaxLevel();
	Lua_PushNumber(L,g_SkillManager.GetSkillMaxLevel(nSikkIdx)+/*Player[nPlayerIndex].m_cReBorn.GetReBornNum()**/Player[nPlayerIndex].m_cReBorn.GetReBornSkillMaxLevel()+nTempPoit);
	return 1;
}

int LuaGetNpcSkillLevel(Lua_State * L)
{
	int nParamCount = Lua_GetTopIndex(L);	
	if (nParamCount < 1) return 0;

	int nSkillId = 0;

	if (Lua_IsNumber(L, 1))
	{
		nSkillId = (int)Lua_ValueToNumber (L, 1);
	}
	else 
	{
		const char * sSkillName = (char *)Lua_ValueToString(L, 1);
		g_OrdinSkillsSetting.GetInteger((char *)sSkillName, "SkillId", 0, &nSkillId);
		if (nSkillId <= 0 ) return 0;
	}

	int  nNpcIdx=(int)Lua_ValueToNumber (L, 2);

	if  (nNpcIdx<=0 || nNpcIdx>=MAX_NPC) return 0;

	Lua_PushNumber(L,Npc[nNpcIdx].m_SkillList.GetLevel(nSkillId));
	return 1;
	
}

int LuaGetSkillLevel(Lua_State * L)
{
	int nParamCount = Lua_GetTopIndex(L);
	int nPlayerIndex = 0;
	
	if (nParamCount < 1) return 0;
	nPlayerIndex = GetPlayerIndex(L);
	
	if (nPlayerIndex <= 0) return 0;
	
	int nSkillId = 0;
	if (Lua_IsNumber(L, 1))
	{
		nSkillId = (int)Lua_ValueToNumber (L, 1);
	}
	else 
	{
		const char * sSkillName = (char *)Lua_ValueToString(L, 1);
		g_OrdinSkillsSetting.GetInteger((char *)sSkillName, "SkillId", 0, &nSkillId);
		if (nSkillId <= 0 ) return 0;
	}
	Lua_PushNumber(L,Npc[Player[nPlayerIndex].m_nIndex].m_SkillList.GetLevel(nSkillId));
	return 1;
}


int LuaSetNpcSkillLevel(Lua_State * L)
{
	int nParamCount = Lua_GetTopIndex(L);

	if (nParamCount < 1) return 0;
		
	int nSkillId = 0;
	if (Lua_IsNumber(L, 1))
	{
		nSkillId = (int)Lua_ValueToNumber (L, 1);
	}
	else 
	{
		const char * sSkillName = (char *)Lua_ValueToString(L, 1);
		nSkillId = g_OrdinSkillsSetting.FindRow((char *)sSkillName) - 2;
		if (nSkillId <= 0 ) 
			return 0;
	}

	int nLevel=(int)Lua_ValueToNumber(L, 2);

	int nNpcIndex = (int)Lua_ValueToNumber(L, 3);

	if (nNpcIndex > 0 && nNpcIndex < MAX_NPC)
	{
		int nListidx = Npc[nNpcIndex].m_SkillList.FindSame(nSkillId);
		if (!nListidx) return 0;

		Npc[nNpcIndex].m_SkillList.SetSLevelByListidx(nListidx,nLevel); //SetSkillLevel

		//nLevel = Npc[nNpcIndex].m_SkillList.GetLevel(nSkillId);

		KSkill *pSkill = (KSkill*)g_SkillManager.GetSkill(nSkillId,nLevel);
		
		if (pSkill)
		{
			Npc[nNpcIndex].m_SkillList.SetSkillExpByListidx(nListidx,pSkill->GetSkillExp()); //设置总熟练度
			Npc[nNpcIndex].m_SkillList.AddSkillExpByListidx(nListidx,0,0);                   //设置当前熟练度为零
			
			//Player[nPlayerIndex].SendSyncData_Skill();  //同步到客户端
		}
		pSkill=NULL;
	}
	
	return 0;
}

//设置技能等级
int LuaSetSkillLevel(Lua_State * L)
{
int nParamCount = Lua_GetTopIndex(L);
int nPlayerIndex = 0;
if (nParamCount < 1) return 0;

  nPlayerIndex = GetPlayerIndex(L);
  
	if (nPlayerIndex <= 0) return 0;
	
	  int nSkillId = 0;
	  if (Lua_IsNumber(L, 1))
	  {
	    nSkillId = (int)Lua_ValueToNumber (L, 1);
	  }
	  else 
	  {
	    const char * sSkillName = Lua_ValueToString(L, 1);
	    nSkillId = g_OrdinSkillsSetting.FindRow((char *)sSkillName) - 2;
	    if (nSkillId <= 0 ) return 0;
	  }
	  int nLevel=(int)Lua_ValueToNumber(L, 2);
	  int nNpcIndex = Player[nPlayerIndex].m_nIndex;
	  int nListIdx = Npc[nNpcIndex].m_SkillList.FindSame(nSkillId);
	  if (!nListIdx) return 0;
	  if (nNpcIndex > 0)
	  {
	      Npc[nNpcIndex].m_SkillList.SetSLevelByListidx(nListIdx,nLevel); //SetSkillLevel
	      
	     KSkill *pSkill = (KSkill*)g_SkillManager.GetSkill(nSkillId,nLevel);

         if (pSkill)
		 {
			 Npc[nNpcIndex].m_SkillList.SetSkillExpByListidx(nListIdx,pSkill->GetSkillExp()); //设置总熟练度
			 Npc[nNpcIndex].m_SkillList.AddSkillExpByListidx(nListIdx,0,0);                   //设置当前熟练度为零

	         Player[nPlayerIndex].SendSyncData_Skill();  //同步到客户端
		 }
		 pSkill=NULL;
	  }

   return 0;
}

//**************************************************************************************************************************************************************
//												NPC操作脚本
//**************************************************************************************************************************************************************
/*nNpcTemplateId GetNpcTmpId(sName)
功能从Npc模板中获得名称为sName的Npc在模板中的Id
sName:Npc名称
nNpcTemplateID:模板中Id
*/

int LuaGetNpcTemplateID(Lua_State * L)
{
	if (Lua_GetTopIndex(L) > 0)
	{
		if (Lua_IsString(L,1))
		{
			const char * pName = lua_tostring(L,1);	
			int nId = g_NpcSetting.FindRow((char*)pName) - 2;
			Lua_PushNumber(L, nId);
		}
		else
			return 0;
	}
	else 
		return 0;
	return 1;
}

/*
nNpcIndex AddNpc(nNpcTemplateId,nLevel, nSubWorldIndex, nPosX, nPosY )

  功能：增加一个特定的NPC
  参数：
  nNpcTemplateId: NPC在NPC模板中的id
  nLevel:Npc的等级
  nSubWorldIndex:所处的世界id
  nPosX：X (点坐标)
  nPosY：Y (点坐标)
  nNpcIndex:增加后，将返回该Npc在游戏世界的Index，如果不成功返回nil 
*/

int LuaNoCreatGoldNpc(Lua_State * L)
{
	int nNpcIdx = (int)Lua_ValueToNumber(L,1);
	int nVal    = (int)Lua_ValueToNumber(L,2);

	if (nNpcIdx<=0 || nNpcIdx >= MAX_NPC)  return 0;

	Npc[nNpcIdx].m_IsNoCreatGoldNpc=nVal;

	return 0 ;

}

int LuaClearNpcDropRank(Lua_State * L)
{
	int nNpcIdx = (int)Lua_ValueToNumber(L,1);
	int nVal    = (int)Lua_ValueToNumber(L,2);
	if (nNpcIdx<=0 || nNpcIdx >= MAX_NPC)  return 0;
	
	Npc[nNpcIdx].m_IsClearDropRank=nVal;

	//if (nVal>0)
	//Npc[nNpcIdx].m_pDropRate=NULL;

	return 0 ;
	
}


int LuaSetNoLevel(Lua_State * L)
{
	int nNpcIdx = (int)Lua_ValueToNumber(L,1);
	int nVal    = (int)Lua_ValueToNumber(L,2);
	
	if (nNpcIdx<=0 || nNpcIdx >= MAX_NPC)  return 0;
	
	Npc[nNpcIdx].m_IsNoLevel=nVal;
	
	return 0 ;
	
}

int LuaSetNpcItemMagicLevel(Lua_State * L)
{
	int nNpcIdx = (int)Lua_ValueToNumber(L,1);
	int nVal    = (int)Lua_ValueToNumber(L,2);

	if (nNpcIdx<=0 || nNpcIdx >= MAX_NPC)  return 0;

	Npc[nNpcIdx].m_itemMagicLevel=nVal;

	return 0 ;

}

int LuaAddNpcEx(Lua_State * L)
{
	char * pName = NULL;
	int	   nId = 0;
	if (Lua_GetTopIndex(L) <9)
	{
		printf("--error:最少9个参数,AddNpcEx:id,level,series,	subworld,px,py,isGoldScript,name,isboss--\n");
		Lua_PushNumber(L, 0);
		return 1;
	}	
	if (Lua_IsNumber(L,1))
	{
		nId = (int)Lua_ValueToNumber(L,1);
	}
	else if	(Lua_IsString(L,1))
	{
		pName = (char *)lua_tostring(L,1);	
		nId = g_NpcSetting.FindRow((char*)pName) - 2;    // 行数减2	  从0开始
	}
	else 
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	if (nId < 0)
		nId = 0;	
	int nLevel = (int)Lua_ValueToNumber(L,2);
	int series = (int)Lua_ValueToNumber(L,3);
	if (nLevel < 0 ) 
		nLevel = 1;
	int	nNpcIdxInfo = MAKELONG(nLevel, nId);//(nId << 7) + nLevel;
	int nNpcIdx = NpcSet.AddServerNpcA(nNpcIdxInfo, 
		(int)Lua_ValueToNumber(L,4),
		(int)Lua_ValueToNumber(L,5),
		(int)Lua_ValueToNumber(L,6),
		(int)Lua_ValueToNumber(L,9)
		);
	//四个地图大小偏移int nOffwminx=1,int nOffwmaxx=1,int nOffhminy=1,int nOffhmaxy=1
	if (nNpcIdx>0)
	{
		Npc[nNpcIdx].m_bDynamicAdd = true;

		Npc[nNpcIdx].SetSeries(series);

		if (Lua_IsNumber(L,8))
		{
			int nReg=(int)Lua_ValueToNumber(L,8);
		}
		else
		{
			char *nNpcName=NULL;
			nNpcName=(char*)Lua_ValueToString(L,8);
			if (nNpcName!=NULL)
				snprintf(Npc[nNpcIdx].Name, sizeof(Npc[nNpcIdx].Name), "%s", nNpcName);  //改名
		}
		int ExeGoldScript=1;
		ExeGoldScript=(int)Lua_ValueToNumber(L,7);
		Npc[nNpcIdx].IsExeGoldScript =ExeGoldScript;
		Npc[nNpcIdx].IsCreatBoss     = 0;
	} 
	Lua_PushNumber(L, nNpcIdx);
	return 1;
}

int LuaAddNpc(Lua_State * L)
{
	char * pName = NULL;
	int	   nId = 0;
	if (Lua_GetTopIndex(L) < 5)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}	
	if (Lua_IsNumber(L,1))
	{
		nId = (int)Lua_ValueToNumber(L,1);
	}
	else if	(Lua_IsString(L,1))
	{
		pName = (char *)lua_tostring(L,1);	
		nId = g_NpcSetting.FindRow((char*)pName) - 2;    // 行数减2	  从0开始
	}
	else 
	{
	  Lua_PushNumber(L, 0);
	  return 1;
	}
	if (nId < 0)
		nId = 0;	
	int nLevel = (int)Lua_ValueToNumber(L,2);
	int nSubWorld = (int)Lua_ValueToNumber(L,3);
	if (nLevel < 0 ) 
		nLevel = 1;
	int	nNpcIdxInfo = MAKELONG(nLevel, nId);//(nId << 7) + nLevel;
	int nNpcIdx = NpcSet.AddServerNpcA(nNpcIdxInfo, 
		                 nSubWorld,
						 (int)Lua_ValueToNumber(L,4),
						 (int)Lua_ValueToNumber(L,5),
						 (int)Lua_ValueToNumber(L,6)
						 //(int)lua_tonumberx(L,10),
						 //(int)lua_tonumberx(L,11),
						 //(int)lua_tonumberx(L,12),
						 //(int)lua_tonumberx(L,13),
						 //(int)lua_tonumberx(L,14)
						 );
//四个地图大小偏移int nOffwminx=1,int nOffwmaxx=1,int nOffhminy=1,int nOffhmaxy=1
if (nNpcIdx>0)
{
	if (Lua_IsNumber(L,7))
	{
	   int nReg=(int)Lua_ValueToNumber(L,7);
	}
	else
	{
	   char *nNpcName=NULL;
	        nNpcName=(char*)Lua_ValueToString(L,7);
	   if (nNpcName!=NULL)
	        snprintf(Npc[nNpcIdx].Name, sizeof(Npc[nNpcIdx].Name), "%s", nNpcName);  //改名
	}

	int ExeGoldScript=1;
	    ExeGoldScript=(int)Lua_ValueToNumber(L,8);
        Npc[nNpcIdx].IsExeGoldScript = ExeGoldScript;
	int CreatBoss=1;
	    CreatBoss=(int)Lua_ValueToNumber(L,9);
	    Npc[nNpcIdx].IsCreatBoss     = CreatBoss;

	Npc[nNpcIdx].m_bDynamicAdd = true;
} 
	Lua_PushNumber(L, nNpcIdx);
    return 1;
}

int LuaSetNpcName_Bm(Lua_State * L)
{
	int nNpcIndex=(int)Lua_ValueToNumber(L,1);
	int nChangeModel=(int)Lua_ValueToNumber(L,2);

	if (nNpcIndex>0 && nNpcIndex<MAX_NPC)
	{
		char nTempName[NAME_LENGHT];
		ZeroMemory(nTempName,sizeof(nTempName));
		switch (nChangeModel)
		{
		case 1:
			if (NULL == Utf8ToGbk(Npc[nNpcIndex].Name, nTempName, sizeof(nTempName))) nTempName[0] = '\0';
			break;
		case 2:
			snprintf(nTempName, sizeof(nTempName), "%s", GbkToUtf8(Npc[nNpcIndex].Name).c_str());
			break;
		default:
			snprintf(nTempName, sizeof(nTempName), "%s", Npc[nNpcIndex].Name);  //改名
			break;
		}
		snprintf(Npc[nNpcIndex].Name, sizeof(Npc[nNpcIndex].Name), "%s", nTempName);      //改名
	}
	return 0;
}

int LuaGetNpcCilentName(Lua_State * L)
{
	int nNpcIndex=(int)Lua_ValueToNumber(L,1);
	int nChangeModel=(int)Lua_ValueToNumber(L,2);
	char nTempName[NAME_LENGHT];
	ZeroMemory(nTempName,sizeof(nTempName));
	if (nNpcIndex>0 && nNpcIndex<MAX_NPC)
	{
		switch (nChangeModel)
		{
		case 1:
			if (NULL == Utf8ToGbk(Npc[nNpcIndex]._Name, nTempName, sizeof(nTempName))) nTempName[0] = '\0';
			break;
		case 2:
			snprintf(nTempName, sizeof(nTempName), "%s", GbkToUtf8(Npc[nNpcIndex]._Name).c_str());
			break;
		default:
			snprintf(nTempName, sizeof(nTempName), "%s", Npc[nNpcIndex]._Name);
			break;
		}
	}
	Lua_PushString(L, nTempName);
	return 1;
}


int LuaSetNpcCilentName(Lua_State * L)
{
	int nNpcIndex=(int)Lua_ValueToNumber(L,1);
	char *nNpcName=(char*)Lua_ValueToString(L,2);
	int nChangeModel=(int)Lua_ValueToNumber(L,3);
	if (nNpcIndex>0 && nNpcIndex<MAX_NPC && nNpcName!=NULL)
	{
		char nTempName[NAME_LENGHT];
		ZeroMemory(nTempName,sizeof(nTempName));
		switch (nChangeModel)
		{
		case 1:
			if (NULL == Utf8ToGbk(nNpcName, nTempName, sizeof(nTempName))) nTempName[0] = '\0';
			break;
		case 2:
			snprintf(nTempName, sizeof(nTempName), "%s", GbkToUtf8(nNpcName).c_str());
			break;
		default:
			snprintf(nTempName, sizeof(nTempName), "%s", nNpcName);
			break;
		}
		snprintf(Npc[nNpcIndex]._Name, sizeof(Npc[nNpcIndex]._Name), "%s", nTempName);  //改名
	}
	return 0;
}

int LuaAddObj(Lua_State * L)
{
      int nDataID,xPos,yPos,m_SubWorldIndex,m_nItemWidth,m_nItemHeight;  
          nDataID = (int)Lua_ValueToNumber(L,1);
          m_SubWorldIndex = (int)Lua_ValueToNumber(L,2);
	      xPos    = (int)Lua_ValueToNumber(L,3);  //像素坐标
	      yPos    = (int)Lua_ValueToNumber(L,4);
          m_nItemWidth  = (int)Lua_ValueToNumber(L,5);           
          m_nItemHeight = (int)Lua_ValueToNumber(L,6);
      char *nObjName;
          nObjName = (char *)Lua_ValueToString(L,7);

//	int		nX, nY,nMap;
	POINT	ptLocal;
	KMapPos	Pos;
	
//	GetMpsPos(&nX, &nY,&nMap);  // 转换后的坐标
	ptLocal.x = xPos;
	ptLocal.y = yPos;

	if (m_SubWorldIndex < 0 || m_SubWorldIndex >= MAX_SUBWORLD) { Lua_PushNumber(L,0); return 1; }
	SubWorld[m_SubWorldIndex].GetFreeObjPos(ptLocal);
	
	Pos.nSubWorld = m_SubWorldIndex;
	SubWorld[m_SubWorldIndex].Mps2Map(ptLocal.x, ptLocal.y,\
		&Pos.nRegion, &Pos.nMapX, &Pos.nMapY,\
		&Pos.nOffX, &Pos.nOffY);
	
	int nObj;
	//////掉下装备保存在KObjItemInfo//////结构体中的参数，以便调用
	KObjItemInfo sInfo;
	memset(&sInfo, 0, sizeof(sInfo));
	sInfo.m_nItemID = 0;
	sInfo.m_nItemWidth = 0;
	sInfo.m_nItemHeight = 0;
	sInfo.m_nMoneyNum = 0;
	g_StrCpyLen(sInfo.m_szName, nObjName, sizeof(sInfo.m_szName)); //物品的名称
	sInfo.m_nColorID    = 0;                      //颜色
	sInfo.m_nMovieFlag  = 1;
	sInfo.m_nSoundFlag  = 1;
	sInfo.m_sHaveAttack=0;   //是否设置为攻击无效了
	//sInfo.m_AttackerDwid=0;   //上次攻击着的DWID
	
	nObj = ObjSet.Add(nDataID, Pos,sInfo);   //开始同步地上物品
	
	if (nObj == -1)
	{
		ObjSet.Remove(nObj);
		//ObjSet.resetLeftTime(nObj,0);
	}
	else
	{

		//Object[nObj].SetScriptFile(szBuffer);  //脚本
		Object[nObj].SetItemBelong(-1);
	}
    Lua_PushNumber(L, nObj); 
    return 1;
}

int LuaSetObjScript(Lua_State * L)
{
	int nObjIdx;
        nObjIdx    = (int)Lua_ValueToNumber(L,1);

		if (nObjIdx<=0 || nObjIdx>=MAX_OBJECT)
			return 0;

	char *nObjScript;
         nObjScript = (char *)Lua_ValueToString(L,2);

    Object[nObjIdx].SetScriptFile(nObjScript);  //脚本

	return 0;
}

int LuaSetObj64Dir(Lua_State * L)
{
	int nObjIdx;
        nObjIdx    = (int)Lua_ValueToNumber(L,1);

	if (nObjIdx>=0 && nObjIdx < MAX_OBJECT)
       Object[nObjIdx].SetDir((int)Lua_ValueToNumber(L,2));  //方向

	return 0;
}

//地上物品消失的时间
int LuaSetObjLifeTime(Lua_State * L)
{
	int nObjIdx;
        nObjIdx    = (int)Lua_ValueToNumber(L,1);

	if (nObjIdx>=0 && nObjIdx < MAX_OBJECT)
       Object[nObjIdx].SetLifeTime((int)Lua_ValueToNumber(L,2));  //方向

	return 0;
}

int LuaGetTonBanNun(Lua_State * L)
{
	int nPlayerIndex = 0;	
	nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0)
	{//下线了
	    Lua_PushNumber(L,-1);
		return 1;
	}
	Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_TongBanNum); 
	return 1;
}


int LuaSetLimitMaxLevel(Lua_State * L)
{
	int nParmCount =Lua_GetTopIndex(L);
	int nPlayerIndex = 0;
	int nLvel=0;
	if (nParmCount<2)
	{//没有参数
		nPlayerIndex = GetPlayerIndex(L);
		if (nPlayerIndex < 0) 
			return 0;
		nLvel=(int)Lua_ValueToNumber(L,1);
	}
	else
	{//有参数
		nPlayerIndex=(int)Lua_ValueToNumber(L,1);
		nLvel=(int)Lua_ValueToNumber(L,2);
	}

	if (nPlayerIndex<0 ||nPlayerIndex>=MAX_PLAYER) return 0;
	
	Player[nPlayerIndex].SetLimitMaxLevel(nLvel);

	return 0;
}


int LuaGetLimitMaxLevel(Lua_State * L)
{
	int nParmCount=Lua_GetTopIndex(L);
	int nPlayerIndex = 0;
	if (nParmCount<1)
	{//没有参数
		nPlayerIndex=GetPlayerIndex(L);
		if (nPlayerIndex < 0)
		{
			Lua_PushNumber(L,0);
			return 1;
		}
	}
	else
	{//有参数
		nPlayerIndex=(int)Lua_ValueToNumber(L,1);
	}

	if (nPlayerIndex<0 || nPlayerIndex>=MAX_PLAYER)
	{
		Lua_PushNumber(L,0);
		return 1;
	}

	Lua_PushNumber(L,Player[nPlayerIndex].GetLimitMaxLevel());
	return 1;
}

int LuaSetTonBanNun(Lua_State * L)
{
	int nPlayerIndex = 0;	
        nPlayerIndex = GetPlayerIndex(L);
		if (nPlayerIndex < 0) 
			return 0;

    int nNpcDwid  = (int)Lua_ValueToNumber(L,1);
        Npc[Player[nPlayerIndex].m_nIndex].m_TongBanNum = nNpcDwid;
	return 0;
}

int LuaGetCurFuMoIdx(Lua_State * L)
{
	int nPlayerIndex = 0;	
	    nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0) 
		return 0;

	//Lua_PushNumber(L,Npc[Player[nPlayerIndex].m_nIndex].GetCurFuMoIdx());
	Lua_PushNumber(L,0);
	return 1;
}


int LuaGetDeathRevivalPos(Lua_State * L)
{
	int nPlayerIndex =0,nSubWorldID,nMpsX,nMpsY;
	
	    nPlayerIndex = GetPlayerIndex(L);

	if (nPlayerIndex < 0)
	{
		int nNpxidx = (int)Lua_ValueToNumber(L,1);
		    if (nNpxidx < 0 || nNpxidx >= MAX_NPC) return 0;
		    nPlayerIndex =Npc[nNpxidx].GetPlayerIdx();
	}

	if (nPlayerIndex < 0)
	{
		return 0;
	}

	Player[nPlayerIndex].GetDeathRevivalPos(&nSubWorldID, &nMpsX, &nMpsY);

	Lua_PushNumber(L,nSubWorldID);
	Lua_PushNumber(L,nMpsX);
	Lua_PushNumber(L,nMpsY);

    return 3;
}

int LuaGetCurFuMoNoLevel(Lua_State * L)
{
	int nPlayerIndex = 0;	
	    nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0) 
		return 0;

	int nIdx=(int)Lua_ValueToNumber(L,1);

	//Lua_PushNumber(L,Npc[Player[nPlayerIndex].m_nIndex].GetCurFuMoNpcNo(nIdx));
	//Lua_PushNumber(L,Npc[Player[nPlayerIndex].m_nIndex].GetCurFoMoSkllLevel(nIdx));
	Lua_PushNumber(L,0);
	Lua_PushNumber(L,0);
	return 2;
}


//押镖nNpcIndex AddNpc(nNpcTemplateId,nLevel, nSubWorldIndex, nPosX, nPosY,IN int nSeries, IN int nComp )
int LuaAddyabiao(Lua_State * L)
{
	char * pName = NULL;
	int	   nId = 0;
	int nNpcIdx=0;
	if (Lua_GetTopIndex(L) < 6) return 0;
	
	if (Lua_IsNumber(L,1))
	{
		nId = (int)Lua_ValueToNumber(L,1);
	}
	else if	(Lua_IsString(L,1))
	{
		pName = (char *)Lua_ValueToString(L,1);	
		nId = g_NpcSetting.FindRow((char*)pName) - 2;  //找到NPC名字所在的TXT 的索引
	}
	else 
	   return 0;
	
	if (nId < 0) nId = 0;	
	
	int nLevel = (int)Lua_ValueToNumber(L,2);
	int nReg = (int)Lua_ValueToNumber(L,9);

	if (nLevel < 0 ) nLevel = 1;
	int	nNpcIdxInfo = MAKELONG(nLevel, nId);//(nId << 7) + nLevel;
    if (nReg==1)
	   nNpcIdx = NpcSet.Addyabiao(nNpcIdxInfo, (int)Lua_ValueToNumber(L,3), (int)Lua_ValueToNumber(L,4), (int)Lua_ValueToNumber(L,5),(int)Lua_ValueToNumber(L,6),(int)Lua_ValueToNumber(L,7),(int)Lua_ValueToNumber(L,8));
	else if(nReg==2)
       nNpcIdx = NpcSet.AddTongBan(nNpcIdxInfo, (int)Lua_ValueToNumber(L,3), (int)Lua_ValueToNumber(L,4), (int)Lua_ValueToNumber(L,5),(int)Lua_ValueToNumber(L,6),(int)Lua_ValueToNumber(L,7),(int)Lua_ValueToNumber(L,8));

   if (nNpcIdx>0)
   { 
	  Npc[nNpcIdx].IsExeGoldScript=0;
	  Npc[nNpcIdx].IsCreatBoss    =0;
   } 
	//	g_StrCpy(Npc[nNpcIdx].Name, (char*)pName);
	Lua_PushNumber(L, nNpcIdx); 
    return 1;
}
//获取两个NPC间的距离  交镖
int LuaGetDistance(Lua_State * L)
{
	if (Lua_GetTopIndex(L) <= 0 ) return 0 ;
	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	int nPlayerIndex = (int)Lua_ValueToNumber(L, 2);
    int nDistance=0;
	if (nNpcIndex > 0 &&  nPlayerIndex>0)
      	nDistance = NpcSet.GetDistance(nNpcIndex, nPlayerIndex);
		Lua_PushNumber(L, nDistance);
    return 1;
}

/*nResult DelNpc (nNpcIndex)
功能：删除一个特定的NPC
nResult:返回成功与否,1为成功,0为失败
*/

int LuaDelNpc(Lua_State * L)
{
	if (Lua_GetTopIndex(L) <= 0 ) return 0 ;
	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	if (nNpcIndex > 0 && nNpcIndex<MAX_NPC)
	{
		if (Npc[nNpcIndex].m_Kind!=kind_player && Npc[nNpcIndex].m_SubWorldIndex>=0 && Npc[nNpcIndex].m_RegionIndex >= 0)
		{
            //Npc[nNpcIndex].BroadCastRevive(1);
			SubWorld[Npc[nNpcIndex].m_SubWorldIndex].m_Region[Npc[nNpcIndex].m_RegionIndex].RemoveNpc(nNpcIndex);
			//SubWorld[Npc[nNpcIndex].m_SubWorldIndex].m_Region[Npc[nNpcIndex].m_RegionIndex].DecNpcRef(Npc[nNpcIndex].m_MapX, Npc[nNpcIndex].m_MapY);
			SubWorld[Npc[nNpcIndex].m_SubWorldIndex].m_Region[Npc[nNpcIndex].m_RegionIndex].DecRef(Npc[nNpcIndex].m_MapX, Npc[nNpcIndex].m_MapY,obj_npc);
			NpcSet.Remove(nNpcIndex);
		}

	}
	return 0;
}

int LuaMovePlayerInMap(Lua_State *L)
{
	int  nSubWorldIdx =(int)Lua_ValueToNumber(L,1);
	int  nCount      = (int)Lua_ValueToNumber(L,2);
	int  nFigthstate = (int)Lua_ValueToNumber(L,3);
	int  nDesPos     = (int)Lua_ValueToNumber(L,4); //目标坐标
	int  nIsComp     = (int)Lua_ValueToNumber(L,5);	//是否恢复阵营
	int  nPunish     = (int)Lua_ValueToNumber(L,6);
	int  nClearScript= (int)Lua_ValueToNumber(L,7);
	KTabFile nPosFile;
	char nPosDir[128];
	sprintf(nPosDir,"\\settings\\mapmovepos\\movepos_%d.txt",nDesPos);
	
	if (!nPosFile.Load(nPosDir))
	{
		printf("-目标地图坐标不存在,移动失败:%s!- \n",nPosDir);
		return 0;
	}
	//g_MoveMapPos
	int nRoWs=nPosFile.GetHeight();

	
	int nIndex = PlayerSet.GetFirstPlayer(); //查找玩家姓名
	int nNum=0;
	while(nIndex > 0)
	{
		if (SubWorld[Npc[Player[nIndex].m_nIndex].m_SubWorldIndex].m_SubWorldID==nSubWorldIdx)
		{
			int nCurRows=GetRandomNumber(1,nRoWs);
			int nMapidx,nXpos,nYpos;
			nPosFile.GetInteger(nCurRows,1,0,&nMapidx);
			nPosFile.GetInteger(nCurRows,2,0,&nXpos);
			nPosFile.GetInteger(nCurRows,3,0,&nYpos);
			Npc[Player[nIndex].m_nIndex].ChangeWorld(nMapidx, nXpos,nYpos);
			
			Npc[Player[nIndex].m_nIndex].SetFightMode(nFigthstate);
			if (nPunish<0 || nPunish>7)
				nPunish=0;  //NPC杀死模式
			   Npc[Player[nIndex].m_nIndex].SetPunishModel(nPunish);
			
			if  (nIsComp>0)
			{
				Npc[Player[nIndex].m_nIndex].RestoreCurrentCamp();
			}
			
			if (nClearScript>0)
			{
			    Player[nIndex].m_dwDeathScriptId=0;	
				Player[nIndex].m_dwDamageScriptId=0;//move map xoa 
			}
			
			nNum++;

			if (nCount>0 && nNum>=nCount)
				break;
		}
		nIndex = PlayerSet.GetNextPlayer();
	}

	nPosFile.Clear();
	Lua_PushNumber(L,nCount);
	return 1;
}

int LuaMovePlayerOutMap(Lua_State * L)
{
   int 	nSubWorldIndex = (int)Lua_ValueToNumber(L,1);
   int 	nFigthstate = (int)Lua_ValueToNumber(L,2);
   int  nDesPos     = (int)Lua_ValueToNumber(L,3); //目标坐标
   int  nIsComp     = (int)Lua_ValueToNumber(L,4);
   int  nPunish     = (int)Lua_ValueToNumber(L,5);
   int  nClearScript= (int)Lua_ValueToNumber(L,6);

   if (nPunish<0 || nPunish >7)
	   nPunish=0;

   if (nSubWorldIndex<0 || nSubWorldIndex >= MAX_SUBWORLD)
	    return 0;

   int  nCount=SubWorld[nSubWorldIndex].MovePlayerOutMap(nFigthstate,nDesPos,nIsComp,nPunish,nClearScript);
   	Lua_PushNumber(L, nCount);
   return 1;
}

int LuaGetNpcCountInMap(Lua_State * L)
{
	int nParamNum = Lua_GetTopIndex(L);
    int ulCount=0,nNpcKind=0;
	int nSubWorldIndex;
	if (nParamNum <=0)
	{
		nSubWorldIndex= GetSubWorldIndex(L); //默认地图怪物  
		if (nSubWorldIndex >= 0) 
			ulCount=SubWorld[nSubWorldIndex].GetNpcCount(nNpcKind);
	}
	else if (Lua_IsNumber(L, 1))
	{
		nSubWorldIndex = (int)Lua_ValueToNumber(L,1);
		if (nSubWorldIndex >= 0 && nSubWorldIndex < MAX_SUBWORLD) 
			ulCount=SubWorld[nSubWorldIndex].GetNpcCount(nNpcKind);	
	}
	Lua_PushNumber(L, ulCount);
	return 1;
}


int LuaClearMapNpcWithName(Lua_State * L)
{
	int nParamNum = Lua_GetTopIndex(L);
	int nSubWorldIndex = (int)Lua_ValueToNumber(L,1);
	char *nName = (char *)Lua_ValueToString(L,2);

	int nCount=0;

	if (nSubWorldIndex >= 0 && nSubWorldIndex<MAX_SUBWORLD  && nName) 
		nCount = SubWorld[nSubWorldIndex].DelAllNpcInWroWithName(nName,0);

	Lua_PushNumber(L, nCount);
	return 1;
}

int LuaDelNpcWithName(Lua_State * L)
{
	int nParamNum = Lua_GetTopIndex(L);
	int nSubWorldIndex = (int)Lua_ValueToNumber(L,1);
	char *nName = (char *)Lua_ValueToString(L,2);
	int nType= (int)Lua_ValueToNumber(L,3);
	int nUtf8toGbk=0;
	int nCount=0;
	if  (nParamNum>=4)
		nUtf8toGbk=(int)Lua_ValueToNumber(L,4);

	if (nName==NULL) return 0;
	char nTemName[64];
	ZeroMemory(nTemName,sizeof(nTemName));
	if  (nUtf8toGbk)
		if (NULL == Utf8ToGbk(nName, nTemName, sizeof(nTemName))) nTemName[0] = '\0';
	else
	    snprintf(nTemName, sizeof(nTemName), "%s", nName);

	if (nSubWorldIndex >= 0 && nSubWorldIndex < MAX_SUBWORLD) 
		nCount = SubWorld[nSubWorldIndex].DelAllNpcInWroWithName(nTemName,nType);

	Lua_PushNumber(L, nCount);
	return 1;
}


//删除该地图所有NPC
int LuaDelAllNpcInWro(Lua_State * L)
{
//	int nSubWorldIndex = GetSubWorldIndex(L); //地图
	int nParamNum = Lua_GetTopIndex(L);
    int ulCount=0;
	int nSubWorldIndex;
	if (nParamNum <=0)
	{
       nSubWorldIndex= GetSubWorldIndex(L);    //地图 
       if (nSubWorldIndex >= 0) 
	      ulCount=SubWorld[nSubWorldIndex].DelAllNpcInWro();
	}
	else if (Lua_IsNumber(L, 1))
	{
	   nSubWorldIndex = (int)Lua_ValueToNumber(L,1);
       if (nSubWorldIndex >= 0 && nSubWorldIndex < MAX_SUBWORLD) 
		  ulCount=SubWorld[nSubWorldIndex].DelAllNpcInWro();	
	}
	Lua_PushNumber(L, ulCount);	
	return 1;
}

//删除某地图某类型NPC
int LuaDelTypeNpcInWro(Lua_State * L)
{
	int nCount=0;
    if (Lua_IsNumber(L, 1))
	{
		int nNpcSettings   = (int)Lua_ValueToNumber(L,1);
		int nSubWorldIndex = (int)Lua_ValueToNumber(L,2);
		if (nSubWorldIndex >= 0 && nSubWorldIndex < MAX_SUBWORLD) 
			nCount=SubWorld[nSubWorldIndex].DelTypeNpcInWro(nNpcSettings);	
		else
		{
	        Lua_PushNumber(L, 0);	
            return 1;
		}
		
	}
	else if (Lua_IsString(L, 1))
	{
		char* nNpcName     = (char *)Lua_ValueToString(L,1);
		int nSubWorldIndex = (int)Lua_ValueToNumber(L,2);
		int nNpcSettings   = NpcSet.SearchNpcSettingID(nNpcName);
		if (nSubWorldIndex >= 0 && nSubWorldIndex < MAX_SUBWORLD)
		    nCount=SubWorld[nSubWorldIndex].DelTypeNpcInWro(nNpcSettings);	
		else
		{
			Lua_PushNumber(L, 0);	
			return 1;
		}
		
	}

	Lua_PushNumber(L, nCount);	
	return 1;

}

/*
int LuaDelAllNpcInWro(Lua_State * L)
{
int nSubWorldIndex = GetSubWorldIndex(L); //地图
int ulCount=0;
KIndexNode * pNode		= NULL;
for (int i = 0; i < 9; i++)  //9个区域查找？
{
//	KRegion * pCurRegion = &m_Region[i];
	
	pNode = (KIndexNode *)SubWorld[nSubWorldIndex].m_Region[i].m_NpcList.GetHead();
	
	while(pNode)
	{
		int nNpcIdx = pNode->m_nIndex;
		if (nNpcIdx > 0  && !Npc[nNpcIdx].IsPlayer())
		{
			
			int nSubWorld = Npc[nNpcIdx].m_SubWorldIndex;
			int nRegion = Npc[nNpcIdx].m_RegionIndex;		
			SubWorld[nSubWorldIndex].m_Region[i].RemoveNpc(nNpcIdx);
			SubWorld[nSubWorldIndex].m_Region[i].DecRef(Npc[nNpcIdx].m_MapX, Npc[nNpcIdx].m_MapY, obj_npc);
			NpcSet.Remove(nNpcIdx);
		//	ZeroMemory(Npc[nNpcIdx].Name,sizeof(Npc[nNpcIdx].Name));
		//	sprintf(Npc[nNpcIdx].Name,"%s","全地图测试改名");
			ulCount ++;
		}
		pNode = (KIndexNode *)pNode->GetNext();	
	}
}
Lua_PushNumber(L, ulCount);

return 1;
}
*/
/*
nDelCount DelNpcsInRgn(nSubWorld,nRegionId, nKind) 
功能：删除某个游戏世界中某个Region内的所有某类的NPC
返回:删除的Npc个数
*/


int LuaDelNpcsInRgn(Lua_State * L)
{
	int nParamCount = Lua_GetTopIndex(L);
	if (nParamCount < 2)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	int nSubWorldIndex = (int)Lua_ValueToNumber(L, 1);
	int nRegionId = (int)Lua_ValueToNumber(L, 2);
	int nKind = (nParamCount >= 3) ? (int)Lua_ValueToNumber(L, 3) : -1;

	if (nSubWorldIndex < 0 || nSubWorldIndex >= MAX_SUBWORLD)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	if (nRegionId < 0 || nRegionId >= SubWorld[nSubWorldIndex].m_nTotalRegion)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}

	int ulCount = 0;
	KIndexNode *pNode = (KIndexNode *)SubWorld[nSubWorldIndex].m_Region[nRegionId].m_NpcList.GetHead();
	while (pNode)
	{
		int nNpcIdx = pNode->m_nIndex;
		pNode = (KIndexNode *)pNode->GetNext();
		if (nNpcIdx > 0 && nNpcIdx < MAX_NPC && Npc[nNpcIdx].m_Kind != kind_player)
		{
			if (nKind < 0 || Npc[nNpcIdx].m_Kind == nKind)
			{
				SubWorld[nSubWorldIndex].m_Region[nRegionId].RemoveNpc(nNpcIdx);
				SubWorld[nSubWorldIndex].m_Region[nRegionId].DecRef(Npc[nNpcIdx].m_MapX, Npc[nNpcIdx].m_MapY, obj_npc);
				NpcSet.Remove(nNpcIdx);
				ulCount++;
			}
		}
	}
	Lua_PushNumber(L, ulCount);
	return 1;
}
/*
nDelCount DelNpcsInWld(nSubWorldId, nKind)
功能：删除某个游戏世界中的所有Npc
*/
int LuaDelNpcsInWld(Lua_State * L)
{
	int nParamCount = Lua_GetTopIndex(L);
	if (nParamCount < 1)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	int nSubWorldIndex = (int)Lua_ValueToNumber(L, 1);
	int nKind = (nParamCount >= 2) ? (int)Lua_ValueToNumber(L, 2) : -1;

	if (nSubWorldIndex < 0 || nSubWorldIndex >= MAX_SUBWORLD)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}

	int ulCount = 0;
	for (int i = 0; i < SubWorld[nSubWorldIndex].m_nTotalRegion; i++)
	{
		KIndexNode *pNode = (KIndexNode *)SubWorld[nSubWorldIndex].m_Region[i].m_NpcList.GetHead();
		while (pNode)
		{
			int nNpcIdx = pNode->m_nIndex;
			pNode = (KIndexNode *)pNode->GetNext();
			if (nNpcIdx > 0 && nNpcIdx < MAX_NPC && Npc[nNpcIdx].m_Kind != kind_player)
			{
				if (nKind < 0 || Npc[nNpcIdx].m_Kind == nKind)
				{
					SubWorld[nSubWorldIndex].m_Region[i].RemoveNpc(nNpcIdx);
					SubWorld[nSubWorldIndex].m_Region[i].DecRef(Npc[nNpcIdx].m_MapX, Npc[nNpcIdx].m_MapY, obj_npc);
					NpcSet.Remove(nNpcIdx);
					ulCount++;
				}
			}
		}
	}
	Lua_PushNumber(L, ulCount);
	return 1;
}

int LuaGetPlatRes(Lua_State * L)
{
	int nType = (int)Lua_ValueToNumber(L, 1);

	Lua_PushNumber(L, g_ItemChangeRes.GetPlatinaRes(nType));
	return 1;
}




int LuaSetNpcBobyType(Lua_State * L)
{
	int nType = (int)Lua_ValueToNumber(L, 1);
	int nWeapon= (int)Lua_ValueToNumber(L, 2);
	int nNpcIndex=0;
	
	if 	(nType==0)
	{
		int nPlayerIndex = GetPlayerIndex(L);
		if (nPlayerIndex>0)
			nNpcIndex  = Player[nPlayerIndex].m_nIndex;
	}
	else
	{
	       nNpcIndex = (int)Lua_ValueToNumber(L,3);
	}
	
	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC) 
		return 0;
	
	Npc[nNpcIndex].m_ArmorType =nWeapon;	
	return 0;
}


int LuaSetNpcWeaponType(Lua_State * L)
{
	int nType = (int)Lua_ValueToNumber(L, 1);
	int nWeapon= (int)Lua_ValueToNumber(L, 2);
	int nNpcIndex=0;

	if 	(nType==0)
	{
		int nPlayerIndex = GetPlayerIndex(L);
		if (nPlayerIndex>0)
		   nNpcIndex  = Player[nPlayerIndex].m_nIndex;
	}
	else
	{
	       nNpcIndex = (int)Lua_ValueToNumber(L,3);
	}

	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC) 
		return 0;

	Npc[nNpcIndex].m_WeaponType =nWeapon;	
	return 0;
}
/*
SetNpcPos (nNpcIndex, x, y)
功能：设置/修改一个NPC的位置
参数：
nNpcIndex:Npc的id 
x：X坐标
y：Y坐标   

*/
int LuaSetNpcPos(Lua_State * L)
{
	int nParamCount = 0;
	if ((nParamCount = Lua_GetTopIndex(L)) < 3) return 0;
	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC) return 0;
	Npc[nNpcIndex].m_MapX = (int)Lua_ValueToNumber(L, 2);
	Npc[nNpcIndex].m_MapY = (int)Lua_ValueToNumber(L, 3);
	
	return 0;
}


/*SetNpcDthSct (nNpcIndex, map, “*.txt” )
功能：设置NPC死亡脚本
参数：
nNpcIndex：NPCIndex
*.txt：脚本文件名
*/
int LuaSetNpcActionScript(Lua_State * L)
{
//	if (Lua_GetTopIndex(L) < 2 ) return 0;
	int nParamCount = Lua_GetTopIndex(L);
	int nNpcIndex=0;
	if (Lua_IsNumber(L, 1))
	{
	   nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	}
	else
	{
      char * nNpcName = (char*)Lua_ValueToString(L, 1);
	       nNpcIndex=NpcSet.SearchName(nNpcName);
	}

	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC)
	{
		//Lua_PushNumber(L, 0);
		return 0;
	}
	  char nMsg[256],nTempStr[256];
	  ZeroMemory(nMsg,sizeof(nMsg));
	  ZeroMemory(nTempStr,sizeof(nTempStr));
	//strcpy(Npc[nNpcIndex].ActionScript,Lua_ValueToString(L,2));
	 //sprintf(Npc[nNpcIndex].ActionScript,(char *)Lua_ValueToString(L,2));
	  g_StrCpyLen(nTempStr, (char *)Lua_ValueToString(L,2), sizeof(nTempStr));
	  int changeModel=0;
	  if  (nParamCount>=3)
	      changeModel=(int)Lua_ValueToNumber(L,3);
	 //g_StrLower(Npc[nNpcIndex].ActionScript);
	  //g_StrLower(nMsg);
	  switch (changeModel)
	  {
	  case 1:
		  {
			  if (nTempStr[0])
				 if (NULL == Utf8ToGbk(nTempStr, nMsg, sizeof(nMsg))) nMsg[0] = '\0';
		  }
		  break;
	  case 2:
		  {
			  if (nTempStr[0])
				  snprintf(nMsg, sizeof(nMsg), "%s", GbkToUtf8(nTempStr).c_str());
		  }
		  break;
	  default:
		  snprintf(nMsg, sizeof(nMsg), "%s", nTempStr);
		  break;
	  }
	  _strlwr(nMsg);
	  Npc[nNpcIndex].SetstrInfo(STR_ACTION_SCRIPT,nMsg);

	  Npc[nNpcIndex].m_ActionScriptID = g_FileName2Id(nMsg);
	
	 //Lua_PushNumber(L, nNpcIndex);
	 return 0;
}

int LuaSetNpcDropScript(Lua_State *L)
{
	if (Lua_GetTopIndex(L) < 2 ) return 0;
	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC) return 0;
	Npc[nNpcIndex].m_IsClearDropRank=1;
	Npc[nNpcIndex].m_DropScriptID = g_FileName2Id((char *)Lua_ValueToString(L,2));
	return 0;
}

//设置NPC的是否再执行脚本
int LuaSetIsDong(Lua_State * L)
{
	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC) return 0;
    int nIsDo=(int)Lua_ValueToNumber(L, 2);
	   Npc[nNpcIndex].m_IsDoing=nIsDo;
       //Npc[nNpcIndex].m_ActionScriptID=0;
	return 0;
}
//设置NPC的五行
int LuaSetNpcSeries(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 2 ) return 0;
	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC) return 0;
	Npc[nNpcIndex].SetSeries((int)Lua_ValueToNumber(L, 2));
	return 0;
}
//获取NPC的五防和伤害
int LuaGetNpcDefendAndDamage(Lua_State * L)
{
	KChanelBaseInfo nNpcInfo;
	memset(&nNpcInfo, 0, sizeof(KChanelBaseInfo)); //	功能：初始化结构数据

	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	if (nNpcIndex < 0 || nNpcIndex >= MAX_NPC) return 0;
	if (nNpcIndex==0)
	{   
	    int nPlayerIndex = GetPlayerIndex(L);

		if (nPlayerIndex>0)
		    Npc[Player[nPlayerIndex].m_nIndex].GetFangAndHai(&nNpcInfo);
	}
	else
	   Npc[nNpcIndex].GetFangAndHai(&nNpcInfo);

	Lua_PushNumber(L, nNpcInfo.PhysicsDamage);
    Lua_PushNumber(L, nNpcInfo.m_ColdDamage);
    Lua_PushNumber(L, nNpcInfo.m_FireDamage);
    Lua_PushNumber(L, nNpcInfo.m_LightDamage);
    Lua_PushNumber(L, nNpcInfo.m_PhysicsDamage);
    Lua_PushNumber(L, nNpcInfo.m_PoisonDamage);
    Lua_PushNumber(L, nNpcInfo.m_ColdResist);
    Lua_PushNumber(L, nNpcInfo.m_FireResist);
    Lua_PushNumber(L, nNpcInfo.m_LightResist);
    Lua_PushNumber(L, nNpcInfo.m_PhysicsResist);
    Lua_PushNumber(L, nNpcInfo.m_PoisonResist);
	return 11;
}

/*
int					m_CurrentFireResist;	    // Npc的当前火抗性
int					m_CurrentColdResist;	    // Npc的当前冰抗性
int					m_CurrentPoisonResist;	    // Npc的当前毒抗性
int					m_CurrentLightResist;	    // Npc的当前电抗性
int					m_CurrentPhysicsResist;	    // Npc的当前物理抗性

  int					m_CurrentFireResistMax;		// Npc的当前最大火抗性
  int					m_CurrentColdResistMax;		// Npc的当前最大冰抗性
  int					m_CurrentPoisonResistMax;	// Npc的当前最大毒抗性
  int					m_CurrentLightResistMax;	// Npc的当前最大电抗性
  int					m_CurrentPhysicsResistMax;	// Npc的当前最大物理抗性
*/

int LuaGetNpcCurlife(Lua_State * L)
{
	int nNpcIndex         = (int)Lua_ValueToNumber(L, 1);
	if (nNpcIndex < 0 || nNpcIndex >= MAX_NPC)
		return 0;
	
    Lua_PushNumber(L, Npc[nNpcIndex].m_CurrentLife);
    Lua_PushNumber(L, Npc[nNpcIndex].m_CurrentLifeMax);	
	return 2;
}



int LuaSetNpcCurlife(Lua_State * L)
{
	int nNpcIndex         = (int)Lua_ValueToNumber(L, 1);
	if (nNpcIndex < 0 || nNpcIndex >= MAX_NPC)
		return 0;
    int nCurlife          = (int)Lua_ValueToNumber(L, 2);
	int nCurMaxlife       = (int)Lua_ValueToNumber(L, 3);
	//int nNpcSetings       = (int)Lua_ValueToNumber(L, 4);
    //int nLevel            = (int)Lua_ValueToNumber(L, 5);

	Npc[nNpcIndex].m_CurrentLife    = nCurlife;			// Npc的当前生命
	Npc[nNpcIndex].m_CurrentLifeMax = nCurMaxlife ;		// Npc的当前生命最大值

	//g_pNpcTemplate[nNpcSetings][nLevel]->m_LifeMax=nCurMaxlife;

	//g_pNpcTemplate[nNpcSetings][nLevel]->  =nCurMaxlife;

	return 0;
}


int LuaGetNpcCurDefend(Lua_State * L)
{
   	int nNpcIndex         = (int)Lua_ValueToNumber(L, 1);
	if (nNpcIndex < 0 || nNpcIndex >= MAX_NPC)
		return 0;
	
	Lua_PushNumber(L, Npc[nNpcIndex].m_CurrentPhysicsResist);
    Lua_PushNumber(L, Npc[nNpcIndex].m_CurrentPhysicsResistMax);
	return 2;
}


int LuaSetNpcCurDefend(Lua_State * L)
{
   	int nNpcIndex         = (int)Lua_ValueToNumber(L, 1);

	if (nNpcIndex < 0 || nNpcIndex >= MAX_NPC)
		return 0;

    int nCurDefend       =(int)Lua_ValueToNumber(L, 2);
	int nCurMaxDefend    =(int)Lua_ValueToNumber(L, 3);

	//int nNpcSetings       =(int)Lua_ValueToNumber(L, 4);
	//int nLevel            =(int)Lua_ValueToNumber(L, 5);
	if  (nCurMaxDefend>MAX_RESIST || nCurDefend>MAX_RESIST)
	{
		nCurDefend=MAX_RESIST;
		nCurMaxDefend=MAX_RESIST;
	}

	Npc[nNpcIndex].m_CurrentFireResist    = nCurDefend;	        // Npc的当前火抗性
	Npc[nNpcIndex].m_CurrentColdResist    = nCurDefend;         // Npc的当前冰抗性
	Npc[nNpcIndex].m_CurrentPoisonResist  = nCurDefend;	        // Npc的当前毒抗性
	Npc[nNpcIndex].m_CurrentLightResist   = nCurDefend;	        // Npc的当前电抗性
	Npc[nNpcIndex].m_CurrentPhysicsResist = nCurDefend;	        // Npc的当前物理抗性
	
	Npc[nNpcIndex].m_CurrentFireResistMax    = nCurMaxDefend;	// Npc的当前最大火抗性
	Npc[nNpcIndex].m_CurrentColdResistMax    = nCurMaxDefend;	// Npc的当前最大冰抗性
	Npc[nNpcIndex].m_CurrentPoisonResistMax  = nCurMaxDefend;	// Npc的当前最大毒抗性
	Npc[nNpcIndex].m_CurrentLightResistMax   = nCurMaxDefend;	// Npc的当前最大电抗性
   	Npc[nNpcIndex].m_CurrentPhysicsResistMax = nCurMaxDefend;	// Npc的当前最大物理抗性


	if (Npc[nNpcIndex].IsPlayer())
	{
		int nTempFangYu = /*Player[Npc[nNpcIndex].GetPlayerIdx()].m_cVip.GetRoleVipNum()**/Player[Npc[nNpcIndex].GetPlayerIdx()].m_cVip.GetRoleVipFanYuMaxVal();
		Npc[nNpcIndex].m_CurrentFireResistMax    +=/*Player[Npc[nNpcIndex].GetPlayerIdx()].m_cReBorn.GetReBornNum()**/Player[Npc[nNpcIndex].GetPlayerIdx()].m_cReBorn.GetReBornFanYuMaxVal();	// Npc的当前最大火抗性
		Npc[nNpcIndex].m_CurrentFireResistMax    +=nTempFangYu;
		Npc[nNpcIndex].m_CurrentColdResistMax    +=/*Player[Npc[nNpcIndex].GetPlayerIdx()].m_cReBorn.GetReBornNum()**/Player[Npc[nNpcIndex].GetPlayerIdx()].m_cReBorn.GetReBornFanYuMaxVal();// Npc的当前最大冰抗性
		Npc[nNpcIndex].m_CurrentColdResistMax    +=nTempFangYu;
		Npc[nNpcIndex].m_CurrentPoisonResistMax  +=/*Player[Npc[nNpcIndex].GetPlayerIdx()].m_cReBorn.GetReBornNum()**/Player[Npc[nNpcIndex].GetPlayerIdx()].m_cReBorn.GetReBornFanYuMaxVal();	// Npc的当前最大毒抗性
		Npc[nNpcIndex].m_CurrentPoisonResistMax  +=nTempFangYu;
		Npc[nNpcIndex].m_CurrentLightResistMax   +=/*Player[Npc[nNpcIndex].GetPlayerIdx()].m_cReBorn.GetReBornNum()**/Player[Npc[nNpcIndex].GetPlayerIdx()].m_cReBorn.GetReBornFanYuMaxVal();	// Npc的当前最大电抗性
		Npc[nNpcIndex].m_CurrentLightResistMax   +=nTempFangYu;
   	    Npc[nNpcIndex].m_CurrentPhysicsResistMax +=/*Player[Npc[nNpcIndex].GetPlayerIdx()].m_cReBorn.GetReBornNum()**/Player[Npc[nNpcIndex].GetPlayerIdx()].m_cReBorn.GetReBornFanYuMaxVal();	// Npc的当前最大物理抗性
		Npc[nNpcIndex].m_CurrentPhysicsResistMax +=nTempFangYu;
	
		//Npc[nNpcIndex].SynNpcInfoToClient(Npc[nNpcIndex].m_dwID); //同步防御	
	}
	else
	{//怪物
		Npc[nNpcIndex].m_FireResist    = nCurDefend;		   //原始的五防
		Npc[nNpcIndex].m_ColdResist    = nCurDefend;
		Npc[nNpcIndex].m_PoisonResist  = nCurDefend;
		Npc[nNpcIndex].m_LightResist   = nCurDefend;
		Npc[nNpcIndex].m_PhysicsResist = nCurDefend;
	}
   /*
	Npc[nNpcIndex].m_CurrentFireResist    = nCurDefend;	        // Npc的当前火抗性
	Npc[nNpcIndex].m_CurrentColdResist    = nCurDefend;         // Npc的当前冰抗性
	Npc[nNpcIndex].m_CurrentPoisonResist  = nCurDefend;	        // Npc的当前毒抗性
	Npc[nNpcIndex].m_CurrentLightResist   = nCurDefend;	        // Npc的当前电抗性
	Npc[nNpcIndex].m_CurrentPhysicsResist = nCurDefend;	        // Npc的当前物理抗性
	*/
	if (Npc[nNpcIndex].m_CurrentFireResist>MAX_RESIST)
		Npc[nNpcIndex].m_CurrentFireResist=MAX_RESIST;
	if (Npc[nNpcIndex].m_CurrentColdResist>MAX_RESIST)
		Npc[nNpcIndex].m_CurrentColdResist=MAX_RESIST;
	if (Npc[nNpcIndex].m_CurrentPoisonResist>MAX_RESIST)
		Npc[nNpcIndex].m_CurrentPoisonResist=MAX_RESIST;
	if (Npc[nNpcIndex].m_CurrentLightResist>MAX_RESIST)
		Npc[nNpcIndex].m_CurrentLightResist=MAX_RESIST;
	if (Npc[nNpcIndex].m_CurrentPhysicsResist>MAX_RESIST)
		Npc[nNpcIndex].m_CurrentPhysicsResist=MAX_RESIST;

	if (Npc[nNpcIndex].m_CurrentFireResistMax>MAX_RESIST)
		Npc[nNpcIndex].m_CurrentFireResistMax=MAX_RESIST;
	if (Npc[nNpcIndex].m_CurrentColdResistMax>MAX_RESIST)
		Npc[nNpcIndex].m_CurrentColdResistMax=MAX_RESIST;
	if (Npc[nNpcIndex].m_CurrentPoisonResistMax>MAX_RESIST)
		Npc[nNpcIndex].m_CurrentPoisonResistMax=MAX_RESIST;
	if (Npc[nNpcIndex].m_CurrentLightResistMax>MAX_RESIST)
		Npc[nNpcIndex].m_CurrentLightResistMax=MAX_RESIST;
	if (Npc[nNpcIndex].m_CurrentPhysicsResistMax>MAX_RESIST)
		Npc[nNpcIndex].m_CurrentPhysicsResistMax=MAX_RESIST;

	int nPlayerIndex = GetPlayerIndex(L);
	
	if (nPlayerIndex > 0)
	{//主人执行的脚本的话
	   Npc[Player[nPlayerIndex].m_nIndex].SynNpcInfoToClient(Npc[nNpcIndex].m_dwID); //同步防御
	} 

/*
	g_pNpcTemplate[nNpcSetings][nLevel]->m_ColdResist      =nCurDefend;
	g_pNpcTemplate[nNpcSetings][nLevel]->m_ColdResistMax   =nCurMaxDefend;

	g_pNpcTemplate[nNpcSetings][nLevel]->m_FireResist      =nCurDefend;
	g_pNpcTemplate[nNpcSetings][nLevel]->m_FireResistMax   =nCurMaxDefend;

	g_pNpcTemplate[nNpcSetings][nLevel]->m_LightResist     =nCurDefend;
	g_pNpcTemplate[nNpcSetings][nLevel]->m_LightResistMax  =nCurMaxDefend;

	g_pNpcTemplate[nNpcSetings][nLevel]->m_PhysicsResist   =nCurDefend;
	g_pNpcTemplate[nNpcSetings][nLevel]->m_PhysicsResistMax=nCurMaxDefend;

	g_pNpcTemplate[nNpcSetings][nLevel]->m_PoisonResist    =nCurDefend;
	g_pNpcTemplate[nNpcSetings][nLevel]->m_PoisonResistMax =nCurMaxDefend;
*/
    
/*
	m_FireResist			= pNpcTemp->m_FireResist;
	m_ColdResist			= pNpcTemp->m_ColdResist;
	m_LightResist			= pNpcTemp->m_LightResist;
	m_PoisonResist			= pNpcTemp->m_PoisonResist;
	m_PhysicsResist			= pNpcTemp->m_PhysicsResist;
*/
	return 0;
}

int LuaGetNpcCurDamage(Lua_State * L)

{
	int nPlayerIndex = GetPlayerIndex(L);

	if  (nPlayerIndex<=0) return 0;

	int nKillMIN=0, nKillMAX=0,nAttackRate=0,nNpcidx,nAttack=0;

	Player[nPlayerIndex].GetEchoDamage(&nKillMIN, &nKillMAX, &nAttack,0);
	
	nNpcidx = Player[nPlayerIndex].m_nIndex;

    nAttackRate = Npc[nNpcidx].m_CurrentSkillMingZhong+(Npc[nNpcidx].m_CurrentAttackRating+Npc[nNpcidx].m_CurrentAttackRatingEnhancev)*(100+Npc[nNpcidx].m_CurrentAttackRatingEnhancep)/100;
			
	Lua_PushNumber(L, nAttackRate);
	Lua_PushNumber(L, nKillMIN);
    Lua_PushNumber(L, nKillMAX);

	return 3;
}


//设置NPC的五防和伤害
int LuaSetNpcDefendAndDamage(Lua_State * L)
{
	KChanelBaseInfo nNpcInfo;
	memset(&nNpcInfo, 0, sizeof(KChanelBaseInfo)); //	功能：初始化结构数据
	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	if (nNpcIndex < 0 || nNpcIndex >= MAX_NPC) return 0;
    nNpcInfo.PhysicsDamage   =(int)Lua_ValueToNumber(L, 2);
	nNpcInfo.m_ColdDamage    =(int)Lua_ValueToNumber(L, 3);
    nNpcInfo.m_FireDamage    =(int)Lua_ValueToNumber(L, 4);
    nNpcInfo.m_LightDamage   =(int)Lua_ValueToNumber(L, 5);
    nNpcInfo.m_PhysicsDamage =(int)Lua_ValueToNumber(L, 6);
    nNpcInfo.m_PoisonDamage  =(int)Lua_ValueToNumber(L, 7);

    nNpcInfo.m_ColdResist    =(int)Lua_ValueToNumber(L, 8);
    nNpcInfo.m_FireResist    =(int)Lua_ValueToNumber(L, 9);
    nNpcInfo.m_LightResist   =(int)Lua_ValueToNumber(L, 10);
    nNpcInfo.m_PhysicsResist =(int)Lua_ValueToNumber(L, 11);
    nNpcInfo.m_PoisonResist  =(int)Lua_ValueToNumber(L, 12);

	if (nNpcIndex==0)
	{  
	  int nPlayerIndex = GetPlayerIndex(L);
	  if (nPlayerIndex>0)
          Npc[Player[nPlayerIndex].m_nIndex].SetFangAndHai(&nNpcInfo);
	}
	else
	      Npc[nNpcIndex].SetFangAndHai(&nNpcInfo);

	return 0;
}

int LuaSetNpcDmgEx(Lua_State * L)
{
	KChanelBaseInfo nNpcInfo;
	memset(&nNpcInfo, 0, sizeof(KChanelBaseInfo)); //	功能：初始化结构数据
	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	if (nNpcIndex < 0 || nNpcIndex >= MAX_NPC) return 0;
    nNpcInfo.PhysicsDamage   =(int)Lua_ValueToNumber(L, 2);
    nNpcInfo.m_PhysicsDamage =(int)Lua_ValueToNumber(L, 2);
    nNpcInfo.m_PoisonDamage  =(int)Lua_ValueToNumber(L, 3);
	nNpcInfo.m_ColdDamage    =(int)Lua_ValueToNumber(L, 4);
    nNpcInfo.m_FireDamage    =(int)Lua_ValueToNumber(L, 5);
    nNpcInfo.m_LightDamage   =(int)Lua_ValueToNumber(L, 6);


	nNpcInfo.m_PhysicsResist = Npc[nNpcIndex].m_PhysicsResist;
	nNpcInfo.m_PoisonResist  = Npc[nNpcIndex].m_PoisonResist;
	nNpcInfo.m_ColdResist    = Npc[nNpcIndex].m_ColdResist;
	nNpcInfo.m_FireResist    = Npc[nNpcIndex].m_FireResist;
	nNpcInfo.m_LightResist   = Npc[nNpcIndex].m_LightResist;



	if (nNpcIndex==0)
	{  
	  int nPlayerIndex = GetPlayerIndex(L);
	  if (nPlayerIndex>0)
          Npc[Player[nPlayerIndex].m_nIndex].SetFangAndHai(&nNpcInfo);
	}
	else
	      Npc[nNpcIndex].SetFangAndHai(&nNpcInfo);

	return 0;
}


int LuaSetNpcResist(Lua_State * L)
{	
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 6)
		return 0 ;

	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);

	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC)
		return 0;
 
	Npc[nNpcIndex].m_CurrentPhysicsResistMax = (int)Lua_ValueToNumber(L, 2);
	Npc[nNpcIndex].m_CurrentPoisonResistMax = (int)Lua_ValueToNumber(L, 3);
	Npc[nNpcIndex].m_CurrentLightResistMax = (int)Lua_ValueToNumber(L, 4);
	Npc[nNpcIndex].m_CurrentFireResistMax = (int)Lua_ValueToNumber(L, 5);
	Npc[nNpcIndex].m_CurrentColdResistMax = (int)Lua_ValueToNumber(L, 6);		
		
	Npc[nNpcIndex].m_CurrentPhysicsResist = (int)Lua_ValueToNumber(L, 2);
	Npc[nNpcIndex].m_CurrentPoisonResist = (int)Lua_ValueToNumber(L, 3);
	Npc[nNpcIndex].m_CurrentLightResist = (int)Lua_ValueToNumber(L, 4);
	Npc[nNpcIndex].m_CurrentFireResist = (int)Lua_ValueToNumber(L, 5);
	Npc[nNpcIndex].m_CurrentColdResist = (int)Lua_ValueToNumber(L, 6);
	 
	if ((nParamNum > 6) && (int)Lua_ValueToNumber(L, 7))
	{
		Npc[nNpcIndex].m_PhysicsResist = (int)Lua_ValueToNumber(L, 2);
		Npc[nNpcIndex].m_PoisonResist = (int)Lua_ValueToNumber(L, 3);
		Npc[nNpcIndex].m_LightResist = (int)Lua_ValueToNumber(L, 4);
		Npc[nNpcIndex].m_FireResist = (int)Lua_ValueToNumber(L, 5);
		Npc[nNpcIndex].m_ColdResist = (int)Lua_ValueToNumber(L, 6);

		Npc[nNpcIndex].m_PhysicsResistMax = (int)Lua_ValueToNumber(L, 2);
		Npc[nNpcIndex].m_PoisonResistMax = (int)Lua_ValueToNumber(L, 3);
		Npc[nNpcIndex].m_LightResistMax = (int)Lua_ValueToNumber(L, 4);
		Npc[nNpcIndex].m_FireResistMax = (int)Lua_ValueToNumber(L, 5);
		Npc[nNpcIndex].m_ColdResistMax = (int)Lua_ValueToNumber(L, 6);	
	}
	return 0;
}	


int LuaGetNpcResist(Lua_State * L)
{	
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 1)
		return 0 ;

	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);

	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC)
		return 0;

	Lua_PushNumber(L, Npc[nNpcIndex].m_PhysicsResist);
	Lua_PushNumber(L, Npc[nNpcIndex].m_PoisonResist);
	Lua_PushNumber(L, Npc[nNpcIndex].m_LightResist);
	Lua_PushNumber(L, Npc[nNpcIndex].m_FireResist);
	Lua_PushNumber(L, Npc[nNpcIndex].m_ColdResist);
	Lua_PushNumber(L, Npc[nNpcIndex].m_PhysicsResistMax);
	Lua_PushNumber(L, Npc[nNpcIndex].m_PoisonResistMax);
	Lua_PushNumber(L, Npc[nNpcIndex].m_LightResistMax);
	Lua_PushNumber(L, Npc[nNpcIndex].m_FireResistMax);
	Lua_PushNumber(L, Npc[nNpcIndex].m_ColdResistMax);
	return 10;
}


int LuaSetNpcPhanDame(Lua_State * L)
{
	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	if (nNpcIndex < 0 || nNpcIndex >= MAX_NPC) return 0;

	if (nNpcIndex==0)
	{  
	  int nPlayerIndex = GetPlayerIndex(L);
	  if (nPlayerIndex>0)
          Npc[Player[nPlayerIndex].m_nIndex].m_CurrentRangeDmgRetPercent = (int)Lua_ValueToNumber(L, 2);
	}
	else
	      Npc[nNpcIndex].m_CurrentMeleeDmgRetPercent = (int)Lua_ValueToNumber(L, 2);
//	      Npc[nNpcIndex].m_CurrentMeleeDmgRet = (int)Lua_ValueToNumber(L, 2);
	      Npc[nNpcIndex].m_CurrentRangeDmgRetPercent = (int)Lua_ValueToNumber(L, 2);
//	      Npc[nNpcIndex].m_CurrentRangeDmgRet = (int)Lua_ValueToNumber(L, 2);*/
	return 0;
}

int LuaGetNpcPhanDame(Lua_State * L)
{	
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 1)
		return 0 ;

	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);

	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC)
		return 0;

	Lua_PushNumber(L, Npc[nNpcIndex].m_CurrentMeleeDmgRetPercent);
	Lua_PushNumber(L, Npc[nNpcIndex].m_CurrentMeleeDmgRet);
	Lua_PushNumber(L, Npc[nNpcIndex].m_CurrentRangeDmgRetPercent);
	Lua_PushNumber(L, Npc[nNpcIndex].m_CurrentRangeDmgRet);
	return 4;
}



int LuaSetBaseDamage(Lua_State * L)
{
    int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	if (nNpcIndex < 0 || nNpcIndex >= MAX_NPC) return 0;
    int nMinDamage = (int)Lua_ValueToNumber(L, 2);
	int nMaxDamage = (int)Lua_ValueToNumber(L, 3);
	int nType = (int)Lua_ValueToNumber(L, 4);
    // nType=0 直接设置  其他 为倍率
    Npc[nNpcIndex].SetBaseDamage(nMinDamage,nMaxDamage,nType);

    return 0;
   
}

int LuaGetNpcID(Lua_State * L)
{	
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 1)
		return 0 ;

	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);

	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC)
		return 0;

	Lua_PushNumber(L, Npc[nNpcIndex].m_dwID);
	return 1;
}

int LuaSetNpcAR(Lua_State * L)
{	
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2)
		return 0 ;

	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	int nAttackR = (int)Lua_ValueToNumber(L, 2);

	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC)
		return 0;

	Npc[nNpcIndex].m_CurrentAttackRating = nAttackR;
	if (nParamNum>2 && (int)Lua_ValueToNumber(L, 3)>0)
		Npc[nNpcIndex].m_AttackRating = nAttackR;
	return 0;
}	

int LuaSetNpcHitRecover(Lua_State *L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2)
		return 0;
	
	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);

	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC)
		return 0;

	Npc[nNpcIndex].m_CurrentHitRecover = (int)Lua_ValueToNumber(L, 2);

	if (nParamNum>2 && (int)Lua_ValueToNumber(L, 3)>0)
		Npc[nNpcIndex].m_HitRecover = (int)Lua_ValueToNumber(L, 2);
	return 0;
}


int LuaCheckHaveStr(Lua_State * L)
{
     //char *szStr=(char *)Lua_ValueToString(L,1);
	int nPlayerIndex = GetPlayerIndex(L);
   if (nPlayerIndex<=0) return 0;

	 int Rows=g_ForbitMap.GetHeight()+1,mReg=0;

     char szYstr[32],nstrVal[32];
	 g_StrCpyLen(szYstr,Lua_ValueToString(L,1),sizeof(szYstr));
     //szString  = lua_tostring (L,1);
	 // printf(szYstr);
	 for (int i=2;i<Rows;++i)
	 {
           g_ForbitMap.GetString(i,"ForBitStr","空",nstrVal,sizeof(nstrVal));
		   printf("----- [%d]输入:%s,对比:%s -----\n",i,szYstr,nstrVal);
		   if (strstr(szYstr,nstrVal))
		   {
			   mReg=1;
			   break;
		   }
	 }
     Lua_PushNumber(L, mReg);
	 return 1;

}

int LuaCheckStrLen(Lua_State * L)
{
	const char * szArg = Lua_ValueToString(L,1);
	if (NULL == szArg) { Lua_PushNumber(L, 0); return 1; }
	int szlen=strlen(szArg), dilen=(int)Lua_ValueToNumber(L,2), mReg=0;
	
	if  (szlen>dilen)
		 mReg=1;

    Lua_PushNumber(L, mReg);
	return 1;
}


int LuaSetChenHaoName(Lua_State * L)
{
   //int nPlayerIndex = GetPlayerIndex(L);

   int nNpcIdx=(int)Lua_ValueToNumber(L,1);
   if (nNpcIdx<=0 || nNpcIdx >= MAX_NPC) return 0;
   //char nstrName[64]={0};
    Npc[nNpcIdx].SetstrInfo(STR_CHEGNHAO_NAME,(char *)Lua_ValueToString(L,2));
    //strcpy(Npc[nNpcIdx].m_ChenHaoName, Lua_ValueToString(L,2));
	return 0;
}

int LuaGetChenHaoName(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if  (nPlayerIndex< 0)	return 0;
	int nNpcIndex =  Player[nPlayerIndex].m_nIndex;
	char nstrName[64]={0};
	Lua_PushString(L,Npc[nNpcIndex].GetstrInfo(STR_CHEGNHAO_NAME,nstrName,sizeof(nstrName)));
	return 1;
}


int LuaSetLienDauName(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if  (nPlayerIndex< 0)	return 0;
	int nNpcIndex =  Player[nPlayerIndex].m_nIndex;
	char * npName= (char *)Lua_ValueToString(L,1);
	if (NULL == npName) return 0;
	int nLen = strlen(npName);

	if  (nLen > NAME_LENGHT)
	{   
		Lua_PushNumber(L,0);
		return 1;
	}

	Npc[nNpcIndex].SetstrInfo(STR_LIENDAU_NAME,npName);
	Lua_PushNumber(L,1);
	return 1;
}

int LuaGetLienDauName(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if  (nPlayerIndex< 0)	return 0;
	int nNpcIndex =  Player[nPlayerIndex].m_nIndex;
	char nstrName[NAME_LENGHT]={0};
	Lua_PushString(L,Npc[nNpcIndex].GetstrInfo(STR_LIENDAU_NAME,nstrName,sizeof(nstrName)));
	return 1;
}


int LuaSetNpcLockMap(Lua_State * L)
{
	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC) return 0;

	Npc[nNpcIndex].Partner._lockMap= (int)Lua_ValueToNumber(L, 2);
	return 0;
}

int LuaSetNpcDamageRatio(Lua_State * L)
{
	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC) return 0;

	int nTempVer=Npc[nNpcIndex].Partner._damageRatio = (int)Lua_ValueToNumber(L, 2);

	if (nTempVer>0)
	{
		Npc[nNpcIndex].m_PhysicsDamage.nValue[0]	*= nTempVer;	  //基本伤害
		Npc[nNpcIndex].m_PhysicsDamage.nValue[2]	*= nTempVer;	  //基本伤害--不包括技能
	}

	return 0;
}

int LuaSetNpcName(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 2 ) return 0;
	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC) return 0;
	g_StrCpyLen(Npc[nNpcIndex].Name, Lua_ValueToString(L,2), sizeof(Npc[nNpcIndex].Name));
	return 0;
}

int LuaChangePlayerName(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if  (nPlayerIndex< 0)	return 0;
	int nNpcIndex =  Player[nPlayerIndex].m_nIndex;
	char * npName= (char *)Lua_ValueToString(L,1);
	//m_PlayerName
	if (NULL == npName) return 0;
	int nLen = strlen(npName);

	if  (nLen> NAME_LENGHT)
	{   
		
		Lua_PushNumber(L,0);
		return 1;
	}

	Npc[nNpcIndex].SetstrInfo(STR_ROLE_CHANGENAME,npName);
	//strcpy(Npc[nNpcIndex].Name, Lua_ValueToString(L,2));
	Npc[nNpcIndex].m_RestNameCount ++;
	Lua_PushNumber(L,1);
	return 1;
}


int LuaGetDataNameState(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if  (nPlayerIndex< 0)	return 0;
	int nNpcIndex =  Player[nPlayerIndex].m_nIndex;
	char nRet[16]={0};
	 Npc[nNpcIndex].GetstrInfo(STR_CHECK_NAME_RET,nRet,sizeof(nRet));
	Lua_PushString(L,nRet);
	return 1;

}

int LuaCheckDataName(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if  (nPlayerIndex< 0)	return 0;

	char * npName= (char *)Lua_ValueToString(L,1);
	if (NULL == npName) return 0;
	int nLen = strlen(npName),nReg=0;
	
	 if  (nLen> NAME_LENGHT)
	 {    
		
		Lua_PushNumber(L,0);
		return 1;
	 }  

	 KIniFile nGoessF;
	 int nKind=0,nMoney=50000000;

	 if (nGoessF.Load("\\Goddess.cfg"))
	 {
		nGoessF.GetInteger("Setting","RoleChangeKind",0,&nKind);
	    nGoessF.GetInteger("Setting","RoleChangeMoney",50000000,&nMoney);

	    if  (nKind==0)
		{//银两
			if (Player[nPlayerIndex].m_ItemList.GetEquipmentMoney()	<nMoney)
			{	
				nGoessF.Clear();
				Lua_PushNumber(L,nReg);
				return 1;
			}
		}
		else
		{//金币
			if (Player[nPlayerIndex].GetCurXu()<nMoney)
			{
				nGoessF.Clear();
				Lua_PushNumber(L,nReg);
	            return 1;
			}
		}
		nGoessF.Clear();
	 }

	 Npc[Player[nPlayerIndex].m_nIndex].ClearstrInfo(STR_CHECK_NAME_RET);
     TCheckNameData sProcessData;
     sProcessData.nProtoId   = c2s_rolecheckname_result;	  //发往goddes存档
     sProcessData.ulIdentity = nPlayerIndex;
	 sProcessData.nDataLen = 1;
	 snprintf(sProcessData.nAccName, sizeof(sProcessData.nAccName), "%s", Player[nPlayerIndex].m_AccoutName);
	 g_StrCpyLen(sProcessData.nRoleName, npName, sizeof(sProcessData.nRoleName));

	 if (g_pDataClient)
	 {
        g_pDataClient->SendPackToServer(&sProcessData, sizeof(TCheckNameData));
	    nReg = 1;
	 }

     Lua_PushNumber(L,nReg);

	 return 1;
}

int LuaGetRestNameCount(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if  (nPlayerIndex< 0)	return 0;
	int nNpcIndex =  Player[nPlayerIndex].m_nIndex;

	//strcpy(Npc[nNpcIndex].Name, Lua_ValueToString(L,2));
	Lua_PushNumber(L, Npc[nNpcIndex].m_RestNameCount);
	return 1;
}


int LuaCHeckIsGuan(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	int nParamNum = Lua_GetTopIndex(L);
	int nReg=0;
/*	char *nNpcName=NULL;
	//else if (Lua_IsNumber(L, 1))Lua_IsString
if (Lua_IsNumber(L,1)) 
	{	
		int nNpcID=(int)Lua_ValueToNumber(L, 1);
		if (nNpcID <= 0 || nNpcID >= MAX_NPC) 
			goto lab_get;
		if (Npc[nNpcID].m_WarFucheng>0)
			nReg=1;
		else if (Npc[nNpcID].m_WarZuoHu>0)
			nReg=2;
		else if (Npc[nNpcID].m_WarYouHu>0)
			nReg=3;
		else if (Npc[nNpcID].m_WarTaishi>0)
			nReg=4;
		else if (Npc[nNpcID].m_WarZhongShu>0)
			nReg=5;
		else if (Npc[nNpcID].m_WarShangShu>0)
			nReg=6;
		else if (Npc[nNpcID].m_WarMiShu>0)
			nReg=7;
		else if (Npc[nNpcID].m_WarTaiLe>0)
			nReg=8; 
	}	
else if (Lua_IsString(L,1))
	{
		nNpcName = (char *)Lua_ValueToString(L,1);
		//		char nPname[32];
		//		if (strcmpi(nNpcName, Npc[nNpcID].Name) == 0)   //这个对比不分字母的大小写
      int nNpcIndexID=NpcSet.SearchName(nNpcName);   //返回索引
	  if (nNpcIndexID>0)
	  {
	      if (Npc[nNpcIndexID].m_WarFucheng>0)
             nReg=1;
		  else if (Npc[nNpcIndexID].m_WarZuoHu>0)
		     nReg=2;
		  else if (Npc[nNpcIndexID].m_WarYouHu>0)
		     nReg=3;
		  else if (Npc[nNpcIndexID].m_WarTaishi>0)
		     nReg=4;
		  else if (Npc[nNpcIndexID].m_WarZhongShu>0)
		     nReg=5;
		  else if (Npc[nNpcIndexID].m_WarShangShu>0)
		     nReg=6;
		  else if (Npc[nNpcIndexID].m_WarMiShu>0)
		     nReg=7;
		  else if (Npc[nNpcIndexID].m_WarTaiLe>0)
		     nReg=8; 
	  }

	}

    Lua_PushNumber(L, nReg);
lab_get:
	Lua_PushNumber(L, nReg); */
	 Lua_PushNumber(L, nReg);
	return 1;
	
}

//获取索引NPC的性别
int  LuaGetNpcSex(Lua_State *L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	int nParamNum = Lua_GetTopIndex(L);
	int nReg=0;
	char *nNpcName=NULL;
if (Lua_IsNumber(L,1)) 
	{	
		int nNpcID=(int)Lua_ValueToNumber(L, 1);
		if (nNpcID <= 0 || nNpcID >= MAX_NPC) 
			goto lab_get;
		if (nNpcID>0)
			nReg=Npc[nNpcID].m_nSex;
		else
			nReg=-1;
		
	}
else if (Lua_IsString(L,1))
	{
		nNpcName = (char *)Lua_ValueToString(L, 1);
       int nRegm=NpcSet.SearchName(nNpcName);   //返回索引
	   if (nRegm>0)
        nReg=Npc[nRegm].m_nSex;
	   else
		nReg=-1;

	}
	
    Lua_PushNumber(L, nReg);
lab_get:
	Lua_PushNumber(L, nReg);
	
	return 1;	
}

//改变人物性别
int LuaSetNpcSex(Lua_State * L)
{
   	//int nPlayerIndex = GetPlayerIndex(L);
	//int nParamNum = Lua_GetTopIndex(L);
	int nNpcIdx=0;
	char *nNpcName=NULL;

	if (Lua_IsNumber(L,1)) 
	{	
		nNpcIdx=(int)Lua_ValueToNumber(L, 1);

	}
	else if (Lua_IsString(L,1))
	{
		nNpcName = (char *)Lua_ValueToString(L, 1);
		nNpcIdx=NpcSet.SearchName(nNpcName);   //返回索引		
	}

	int nNpcSex=(int)Lua_ValueToNumber(L, 2);

	if (nNpcSex<0 || nNpcSex>1)
		nNpcSex=0;

	if (nNpcIdx <= 0 || nNpcIdx >= MAX_NPC) 
		return 0;

	Npc[nNpcIdx].m_nSex=nNpcSex;
	
	return 0;

}


int LuaDelItemWithName(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0)
	{
		return 0;	
	}
	Lua_PushNumber(L, Player[nPlayerIndex].m_ItemList.DelItemWithName((char *)Lua_ValueToString(L,1)));
	return 1;

}

int LuaGetItemCountWithName(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0)
	{
		 return 0;	
	}
	
	Lua_PushNumber(L, Player[nPlayerIndex].m_ItemList.GetCountWithName((char *)Lua_ValueToString(L,1)));
	return 1;
}


int LuaCheckIsThisItem(Lua_State * L)
{
	int nReg=0;
	char *nItemName=NULL;
	if (Lua_IsNumber(L,1)) 
	{	
		int nItemID=(int)Lua_ValueToNumber(L, 1);
		    nItemName = (char *)Lua_ValueToString(L, 2);
		if (nItemID <= 0 || nItemID >= MAX_ITEM) 
			goto lab_get;
		if (strcmpi(Item[nItemID].GetName(),nItemName) == 0)   //这个对比不分字母的大小写
			nReg=1;
	}
	else
            nReg=0;

	Lua_PushNumber(L, nReg);
lab_get:
	Lua_PushNumber(L, nReg);
	
	return 1;
}

//检测是否为禁止地图--数字
int LuaCheckForbitMapA(Lua_State * L)
{
   int nValID = (int)Lua_ValueToNumber(L,1);
   char *nCols=NULL;
        nCols = (char *)Lua_ValueToString(L, 2);
   int nRows  = g_ForbitMap.GetHeight()+1,nReg=0;
   int nValIdx;
   for (int i=2;i<nRows;++i)
   {       
	  g_ForbitMap.GetInteger(i,nCols,0,&nValIdx);
	  if (nValIdx==nValID)   //char转int atoi(nValIdx)
	  { 
          nReg = 1;
		  break;
	  } 
   }

   Lua_PushNumber(L, nReg);
   return 1;
}

//检测是否为禁止地图-字符串
int LuaCheckForbitMapB(Lua_State * L)
{
	char *nValID = (char *)Lua_ValueToString(L,1);
	char *nCols=(char *)Lua_ValueToString(L, 2);
	int  nRows =g_ForbitMap.GetHeight()+1,nReg=0;
	char nValIdx[32];
	for (int i=2;i<nRows;++i)
	{       
	       g_ForbitMap.GetString(i,nCols,"",nValIdx,sizeof(nValIdx));
		   if (strcmpi(nValID,nValIdx) == 0)   //这个对比不分字母的大小写   //char转intatoi(nValIdx)
		   { 
			   nReg=1;
			   break;
		   } 
	}
	Lua_PushNumber(L, nReg);
	return 1;
}


int LuaCheckNpcGuiShu(Lua_State * L)
{
		int nNpcIdx=0,nMapId;
		char *nNpcName=NULL;
		if (Lua_IsNumber(L,1)) 
		{	
			DWORD nDwID=(int)Lua_ValueToNumber(L, 1);		 
			nNpcIdx = NpcSet.SearchID(nDwID);
		}
		else if (Lua_IsString(L,1))
		{
			nNpcName = (char *)Lua_ValueToString(L,1);
			nMapId = (int)Lua_ValueToNumber(L,2);
			//		char nPname[32];
			//		if (strcmpi(nNpcName, Npc[nNpcID].Name) == 0)            //这个对比不分字母的大小写
			if (nMapId<=0)//游戏中是否有这个名字的怪物
			{//返回同伴的索引
				int nIndex_z = NpcSet.SearchName(nNpcName);
				if  (nIndex_z<=0 || nIndex_z>=MAX_NPC) 
					goto goout;
				nNpcIdx = NpcSet.SearchGSdwId(Npc[nIndex_z].m_dwID);
				// NpcSet.SearchGSName(nNpcName);                     //返回索引
			}
			else          //某个地图是否有这个名字的怪物
			{
				int nIndex_z = NpcSet.SearchName(nNpcName);
				if  (nIndex_z<=0 || nIndex_z>=MAX_NPC) 
					goto goout;
				nNpcIdx = NpcSet.SearchGSdwIdInMap(Npc[nIndex_z].m_dwID,nMapId);         //返回索引
			}
		}
goout:		
		Lua_PushNumber(L,nNpcIdx);
		return 1;

}

//Lua_InsertValue()  //插入一个数值
int LuaCheckNpcName(Lua_State * L)
{
	int nNpcIdx=0,nMapId;
	char *nNpcName=NULL;
    if (Lua_IsNumber(L,1)) 
	{	
		DWORD nDwID=(int)Lua_ValueToNumber(L, 1);		 
		//if (strcmpi(nNpcName, Npc[nNpcID].Name) == 0)   //这个对比不分字母的大小写
		    nNpcIdx = NpcSet.SearchID(nDwID);
	}
    else if (Lua_IsString(L,1))
	{
		nNpcName = (char *)Lua_ValueToString(L,1);
		nMapId = (int)Lua_ValueToNumber(L,2);
//		char nPname[32];
//		if (strcmpi(nNpcName, Npc[nNpcID].Name) == 0)                    //这个对比不分字母的大小写
          if (nMapId<=0)//游戏中是否有这个名字的怪物
              nNpcIdx = NpcSet.SearchName(nNpcName);                     //返回索引
		  else          //某个地图是否有这个名字的怪物
		      nNpcIdx = NpcSet.SearchNameInMap(nNpcName,nMapId,0);       //返回数量
	}

    Lua_PushNumber(L,nNpcIdx);

	return 1;
	
}


//Lua_InsertValue()  //插入一个数值
int LuaCheckNewNpcName(Lua_State * L)
{
	int nNpcIdx=0,nMapId;
	char *nNpcName=NULL;
    if (Lua_IsNumber(L,1)) 
	{	
		DWORD nDwID=(int)Lua_ValueToNumber(L, 1);		 
		//if (strcmpi(nNpcName, Npc[nNpcID].Name) == 0)   //这个对比不分字母的大小写
		nNpcIdx = NpcSet.SearchID(nDwID);
	}
    else if (Lua_IsString(L,1))
	{
		nNpcName = (char *)Lua_ValueToString(L,1);
		nMapId = (int)Lua_ValueToNumber(L,2);
		//		char nPname[32];
		//		if (strcmpi(nNpcName, Npc[nNpcID].Name) == 0)                    //这个对比不分字母的大小写
		if (nMapId<=0)//游戏中是否有这个名字的怪物
			nNpcIdx = NpcSet.SearchName(nNpcName);                     //返回索引
		else          //某个地图是否有这个名字的怪物
			nNpcIdx = NpcSet.SearchNameInMap(nNpcName,nMapId,1);       //返回索引
	}
	
    Lua_PushNumber(L,nNpcIdx);
	
	return 1;
	
}


 /*检查是数字或是字符 */
int LuaCheckNumandstr(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	int nParamNum = Lua_GetTopIndex(L);
	int nReg=0;
if (Lua_IsNumber(L,1)) 
       nReg=2;
else if (Lua_IsString(L,1))
       nReg=1;
	Lua_PushNumber(L, nReg);
    return 1;
}

int LuaFindTbName(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	int nReg=0;	   
	char *nNpcName = NULL;
	     nNpcName  = (char *)Lua_ValueToString(L, 1);
    
	if (NULL==nNpcName)
        goto goout;

	int nIndex_z =   NpcSet.SearchName(nNpcName);

	if  (nIndex_z<=0 || nIndex_z>=MAX_NPC) 
	    goto goout;
	//找到某个人的同伴
    int nNpcIdx    = NpcSet.SearchGSdwId(Npc[nIndex_z].m_dwID);
	if (nNpcIdx>0 && nNpcIdx<MAX_NPC && Npc[nNpcIdx].m_btRankId == 99)//是同伴
		nReg=1;
goout:
	Lua_PushNumber(L, nReg);	
	return 1;
}


int LuaFindNpcName(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);	   
	char *nNpcName   = NULL;
	nNpcName         = (char *)Lua_ValueToString(L, 1);
    int nNpcIdx      = NpcSet.SearchName(nNpcName);	
	Lua_PushNumber(L, nNpcIdx);	
	return 1;
}

int LuaGetNpcDwidx(Lua_State * L)
{
      int nNpcID=(int)Lua_ValueToNumber(L, 1);
   
      if (nNpcID < 0 || nNpcID >= MAX_NPC) { Lua_PushNumber(L,0); return 1; }
      Lua_PushNumber(L, Npc[nNpcID].m_dwID);
	  return 1;
}

int LuaCheckName(Lua_State * L)
{
   int nPlayerIndex = GetPlayerIndex(L);
   int nReg=0,nNpcID=(int)Lua_ValueToNumber(L, 1);
   char *nNpcName=NULL;
        nNpcName=(char *)Lua_ValueToString(L, 2);
   if (nNpcID <= 0 || nNpcID >= MAX_NPC) 
	   goto lab_get;
//	   memcmp  //内存 ASII码的比较 是字节比较
   if (nNpcName && strstr(Npc[nNpcID].Name,nNpcName))   //这个对比不分字母的大小写stricmp Npc[nId2].m_IsRevive == g_FileName2Id(Npc[nId1].Name))
       nReg=1;
     Lua_PushNumber(L, nReg);

lab_get:
     Lua_PushNumber(L, nReg);

	 return 1;

}

int LuaSetNpcJb(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	int nReg=0;
	// if (Lua_GetTopIndex(L) < 1 )	   
   int nNpcID=(int)Lua_ValueToNumber(L, 1);
   int nNpcJB=(int)Lua_ValueToNumber(L, 2);
   if (nNpcID < 0 || nNpcID >= MAX_NPC) return 0;
   Npc[nNpcID].m_njb=nNpcJB;
   return 0;
}

int LuaSetNpcDefense(Lua_State * L)
{	
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2)
		return 0 ;

	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	int nDefense = (int)Lua_ValueToNumber(L, 2);

	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC)
		return 0;

	Npc[nNpcIndex].m_CurrentDefend = nDefense;
	if (nParamNum>2 && (int)Lua_ValueToNumber(L, 3)>0)
		Npc[nNpcIndex].m_Defend = nDefense;
	return 0;
}	

int LuaSetNpcLife(Lua_State * L)
{	
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2)
		return 0 ;

	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	int nLife = (int)Lua_ValueToNumber(L, 2);

	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC)
		return 0;

	Npc[nNpcIndex].m_CurrentLife = nLife;
	Npc[nNpcIndex].m_CurrentLifeMax = nLife;

	if (nParamNum>2 && (int)Lua_ValueToNumber(L, 3)>0)
		Npc[nNpcIndex].m_LifeMax = nLife;
	return 0;
}	


int LuaGetNpcLife(Lua_State * L)
{	
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 1)
		return 0 ;

	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);

	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC)
		return 0;

	Lua_PushNumber(L, Npc[nNpcIndex].m_CurrentLifeMax);
	return 1;
}


//生命恢复速度
int LuaGetNpcLifeRep(Lua_State * L)
{
	int m_nNpcIdx=(int)Lua_ValueToNumber(L, 1);	
    if (m_nNpcIdx < 0 || m_nNpcIdx >= MAX_NPC) { Lua_PushNumber(L,0); return 1; }
    if  (m_nNpcIdx)
		Lua_PushNumber(L,Npc[m_nNpcIdx].m_CurrentLifeReplenish);
	else
		Lua_PushNumber(L,0);

    return 1;
}
//生命恢复速度
int LuaSetNpcLifeRep(Lua_State * L)
{
	int m_nNpcIdx=(int)Lua_ValueToNumber(L, 1);	
    int nNpcRep=(int)Lua_ValueToNumber(L, 2);

	if (m_nNpcIdx<=0 || m_nNpcIdx >= MAX_NPC)
		return 0;

    Npc[m_nNpcIdx].m_CurrentLifeReplenish = nNpcRep;

    return 0;
}

int LuaSetNpcGuiShu(Lua_State * L)
{
	int  m_nNpcIdx=(int)Lua_ValueToNumber(L, 1);	
    char *nNpcRep=(char *)Lua_ValueToString(L, 2);
	if (nNpcRep!=NULL)
	{
		int nNpcIndex = NpcSet.SearchName(nNpcRep);
		if (m_nNpcIdx < 0 || m_nNpcIdx >= MAX_NPC) return 0;
		if  (nNpcIndex>0 && nNpcIndex<MAX_NPC)
			Npc[m_nNpcIdx].m_GuiShuDwid =Npc[nNpcIndex].m_dwID;
	} 
    return 0;
}

int LuaGetNpcGuiShu(Lua_State * L)
{
	int  m_nNpcIdx=(int)Lua_ValueToNumber(L, 1);	
  
    if (m_nNpcIdx<=0 || m_nNpcIdx >= MAX_NPC) return 0;
	char nstrName[64];
	ZeroMemory(nstrName,sizeof(nstrName));
	if  (Npc[m_nNpcIdx].m_GuiShuDwid>0)
	{
		  int nGindex=NpcSet.SearchID(Npc[m_nNpcIdx].m_GuiShuDwid);
		  if  (nGindex>0&&nGindex<MAX_NPC)
			  snprintf(nstrName, sizeof(nstrName), "%s", Npc[nGindex].Name);
	}
	Lua_PushString(L,nstrName);

    return 1;
}

//设置NPC攻速 模式=1为外功 2 为内功 0为两种
int LuaSetNpcAttackSpeed(Lua_State * L)
{		
	int m_nNpcIdx=(int)Lua_ValueToNumber(L, 1);	
    int nNpcSpeed=(int)Lua_ValueToNumber(L, 2);
	int nModel=(int)Lua_ValueToNumber(L, 3); //模式
    if (nModel==0)
	{
     if (m_nNpcIdx < 0 || m_nNpcIdx >= MAX_NPC) return 0;
     Npc[m_nNpcIdx].m_CurrentAttackSpeed = nNpcSpeed;  //外功速度
     Npc[m_nNpcIdx].m_CurrentCastSpeed	 = nNpcSpeed;    //内攻速
	}
	else if (nModel==1)
	{
		if (m_nNpcIdx < 0 || m_nNpcIdx >= MAX_NPC) return 0;
	 Npc[m_nNpcIdx].m_CurrentAttackSpeed = nNpcSpeed;  //外功速度
	}
	else if (nModel==2)
	{
		if (m_nNpcIdx < 0 || m_nNpcIdx >= MAX_NPC) return 0;
	 Npc[m_nNpcIdx].m_CurrentCastSpeed	 = nNpcSpeed;    //内攻速
	}

    return 0;

}

//设置NPC攻速 模式=1为外功 2 为内功 0为两种  --NPC索引,攻速,模式(0全部,1外功,2内功),npcsettings,等级
int LuaSetNpcAttackFrame(Lua_State * L)
{		
	int m_nNpcIdx=(int)Lua_ValueToNumber(L, 1);	
    int nNpcSpeedFrame=(int)Lua_ValueToNumber(L, 2);
	int nModel=(int)Lua_ValueToNumber(L, 3); //模式
	int nNpcSetings = (int)Lua_ValueToNumber(L, 4); 
	int nLevel = (int)Lua_ValueToNumber(L, 5);

	if  (m_nNpcIdx<=0 || m_nNpcIdx>=MAX_NPC)
		return 0;

   /* if (nModel==0)
	{
		Npc[m_nNpcIdx].SetAttackFrame(nNpcSpeedFrame) ;  //NPC外功速度
		Npc[m_nNpcIdx].SetCastFrame(nNpcSpeedFrame);     //NPC内攻速

		g_pNpcTemplate[nNpcSetings][nLevel]->m_AttackFrame=nNpcSpeedFrame;
		g_pNpcTemplate[nNpcSetings][nLevel]->m_CastFrame  =nNpcSpeedFrame;
	}
	else if (nModel==1)
	{
		Npc[m_nNpcIdx].SetAttackFrame(nNpcSpeedFrame);  //外功速度
		g_pNpcTemplate[nNpcSetings][nLevel]->m_AttackFrame=nNpcSpeedFrame;
	}
	else if (nModel==2)
	{
		Npc[m_nNpcIdx].SetCastFrame(nNpcSpeedFrame);    //内攻速
		g_pNpcTemplate[nNpcSetings][nLevel]->m_CastFrame=nNpcSpeedFrame;
	}*/ 

	KNpcTemplate* pNpcTemp =NpcSet.GetTemplate(nNpcSetings, nLevel);

	if  (!pNpcTemp)
		return 0;


	if (nModel==0)
	{
		Npc[m_nNpcIdx].SetAttackFrame(nNpcSpeedFrame) ;  //NPC外功速度
		Npc[m_nNpcIdx].SetCastFrame(nNpcSpeedFrame);     //NPC内攻速
		
		pNpcTemp->m_AttackFrame=nNpcSpeedFrame;
		pNpcTemp->m_CastFrame  =nNpcSpeedFrame;
	}
	else if (nModel==1)
	{
		Npc[m_nNpcIdx].SetAttackFrame(nNpcSpeedFrame);  //外功速度
		pNpcTemp->m_AttackFrame=nNpcSpeedFrame;
	}
	else if (nModel==2)
	{
		Npc[m_nNpcIdx].SetCastFrame(nNpcSpeedFrame);    //内攻速
		pNpcTemp->m_CastFrame=nNpcSpeedFrame;
	}
	
    return 0;
	
}

//设置NPC的经验倍率
int LuaSetNpcExperience(Lua_State * L)
{   
	int nNpcID=(int)Lua_ValueToNumber(L, 1);	
	int nNpcExp=(int)Lua_ValueToNumber(L, 2);
	int nModel=(int)Lua_ValueToNumber(L, 3); //模式 1为增加 2 为减少
	if (nNpcID < 0 || nNpcID >= MAX_NPC) return 0;
	if (nModel==1)
	   Npc[nNpcID].IsJinYan=nNpcExp; //m_Experience
	else if (nModel==2)
       Npc[nNpcID].IsJinYan +=nNpcExp; //m_CurrentExperience
	else
       Npc[nNpcID].IsJinYan=1;  //没增加经验
    return 0;
}

//设置NPC的经验倍率
int LuaSetNpcExper(Lua_State * L)
{   
	int nNpcID=(int)Lua_ValueToNumber(L,1);	
	int nNpcExp=(int)Lua_ValueToNumber(L,2);
	int nModel=(int)Lua_ValueToNumber(L,3); //模式 1为直接设置 2 为累积
	if (nNpcID < 0 || nNpcID >= MAX_NPC) return 0;
	if (nModel==1)
		Npc[nNpcID].m_Experience=nNpcExp;   //m_Experience
	else if (nModel==2)
	{
		Npc[nNpcID].m_Experience +=nNpcExp;  //m_CurrentExperience
		if (Npc[nNpcID].m_Experience<0)
			Npc[nNpcID].m_Experience =1;
	}
	else
		Npc[nNpcID].m_Experience=1;         //没增加经验
    return 0;
}

//根据SETTINGS 获取NPC名字
int LuaGetNameFromSettings(Lua_State * L)
{   
	int nSettings = (int)Lua_ValueToNumber(L,1);	
	char nNpcName[NAME_LENGHT];
    g_NpcSetting.GetString(nSettings+2,"Name","无数据",nNpcName,sizeof(nNpcName));
    Lua_PushString(L,nNpcName);
    return 1;
}

int LuaSetNpcJxb(Lua_State * L)
{  
   int nNpcID=(int)Lua_ValueToNumber(L, 1);	
   int nNpcJXB=(int)Lua_ValueToNumber(L, 2);
   if (nNpcID < 0 || nNpcID >= MAX_NPC) return 0;
   Npc[nNpcID].m_njxb=nNpcJXB;
   return 0;
}


int LuaSetNpcDefend(Lua_State * L)
{  
	int nNpcidx=(int)Lua_ValueToNumber(L, 1);	
	int nCount=(int)Lua_ValueToNumber(L, 2);
	
	if  (nNpcidx<=0 || nNpcidx>=MAX_NPC) return 0;
	
	if  (nCount<0)
		nCount=0;
	
	Npc[nNpcidx].m_CurrentDefend=nCount;
	
	return 0;
}

int LuaGetNpcDefend(Lua_State * L)
{  
	int nNpcidx=(int)Lua_ValueToNumber(L, 1);		
	if  (nNpcidx<=0 || nNpcidx>=MAX_NPC) return 0;	
	
	Lua_PushNumber(L, Npc[nNpcidx].m_CurrentDefend);
	return 1;

}




	



int LuaSetNpcItemCount(Lua_State * L)
{  
	int nNpcidx=(int)Lua_ValueToNumber(L, 1);	
	int nCount=(int)Lua_ValueToNumber(L, 2);

	if  (nNpcidx<=0 || nNpcidx>=MAX_NPC) return 0;

	if  (nCount<0)
		nCount=0;

	Npc[nNpcidx].m_CurrentTreasure=nCount;

	return 0;
}




int LuaGetNpcJb(Lua_State * L)
{
  
	int nNpcID=(int)Lua_ValueToNumber(L, 1);
	int nNpcJB=(int)Lua_ValueToNumber(L, 2);

	if (nNpcID < 0 || nNpcID >= MAX_NPC) { Lua_PushNumber(L,0); return 1; }
	Lua_PushNumber(L, Npc[nNpcID].m_njb);
	return 1;
}

int LuaGetNpcJxb(Lua_State * L)
{
   	int nNpcID=(int)Lua_ValueToNumber(L, 1);	
	int nNpcJXB=(int)Lua_ValueToNumber(L, 2);

	if (nNpcID < 0 || nNpcID >= MAX_NPC) { Lua_PushNumber(L,0); return 1; }
	Lua_PushNumber(L, Npc[nNpcID].m_njxb);
	return 1;
}
//设置当前怒气值
int LuaSetNpcAngry(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if  (nPlayerIndex<=0) return 0;
	
	int nAngryVal=(int)Lua_ValueToNumber(L, 1);	
	int nModel=(int)Lua_ValueToNumber(L, 2);
	if  (nModel==0)
        Npc[Player[nPlayerIndex].m_nIndex].m_CurrentNuQi=nAngryVal;	
	else if (nModel>0)
        Npc[Player[nPlayerIndex].m_nIndex].m_CurrentNuQi+=nAngryVal;
	else
	    Npc[Player[nPlayerIndex].m_nIndex].m_CurrentNuQi=nAngryVal;	
	
	return 0;
}
//设置基本怒气最大值
int LuaSetNpcMaxAngry(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if  (nPlayerIndex<=0) return 0;
   
	int nAngryVal=(int)Lua_ValueToNumber(L, 1);	
	int nModel=(int)Lua_ValueToNumber(L, 2);
	if  (nModel==0)
		Npc[Player[nPlayerIndex].m_nIndex].m_NuqiMax=nAngryVal;	
	else if (nModel>0)
		Npc[Player[nPlayerIndex].m_nIndex].m_NuqiMax+=nAngryVal;
	else
		Npc[Player[nPlayerIndex].m_nIndex].m_NuqiMax=nAngryVal;	
	
	return 0;
}
//设置当前怒气最大值--实时同步
int LuaSetNpcCurMaxAngry(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if  (nPlayerIndex<=0) return 0;
   
	int nAngryVal=(int)Lua_ValueToNumber(L, 1);	
	int nModel=(int)Lua_ValueToNumber(L, 2);
	if  (nModel==0)
		Npc[Player[nPlayerIndex].m_nIndex].m_CurrentNuQiMax=nAngryVal;	
	else if (nModel>0)
		Npc[Player[nPlayerIndex].m_nIndex].m_CurrentNuQiMax+=nAngryVal;
	else
		Npc[Player[nPlayerIndex].m_nIndex].m_CurrentNuQiMax=nAngryVal;	
	
	return 0;
}


int LuaChangeItemLevel(Lua_State * L)
{	 	
	int nPlayerIndex = GetPlayerIndex(L);
	int nNewItemIdx=0;
    if (nPlayerIndex > 0)
	{  
	  int nItemidx = (int)Lua_ValueToNumber(L,1);
	  int nAddLevel = (int)Lua_ValueToNumber(L,2);
	  int nAddnChance = (int)Lua_ValueToNumber(L,3);
	  
	  if (nItemidx >0 && nItemidx<MAX_ITEM)
	  {
		 int nItemClass		= Item[nItemidx].GetGenre();

		 if  (nItemClass!=0) return 0;

		 int nDetailType	= Item[nItemidx].GetDetailType();
		 int nParticularType= Item[nItemidx].GetParticular();
		 int nLevel			= Item[nItemidx].GetLevel();
		 int nSeries		= Item[nItemidx].GetSeries();
		 int nLuck			= Item[nItemidx].GetItemParam()->nLuck;
		 int nEnChance		= Item[nItemidx].GetEnChance()+nAddnChance;
		 int nGoldId		= Item[nItemidx].GetGoldId();	
		 int nVersion		= Item[nItemidx].GetItemParam()->nVersion;
		 UINT	nGndrom		= Item[nItemidx].GetItemParam()->uRandomSeed;
		 int nPoint			= Item[nItemidx].GetPoint();
		 int nRPoint		= Item[nItemidx].IsRong();
		 int nStackNum      = Item[nItemidx].GetStackNum();
		 int nItemLevel[6];
		 int nRItemLevel[6];
		 ZeroMemory(nItemLevel, sizeof(nItemLevel));
		 ZeroMemory(nRItemLevel, sizeof(nRItemLevel));
		 //同步魔法属性
		 for (int i = 0; i < 6; ++i)
		 {
			 nItemLevel[i]  = Item[nItemidx].GetItemParam()->nGeneratorLevel[i];
			 nRItemLevel[i] = Item[nItemidx].GetItemParam()->nRGeneratorLevel[i];
		 }

		 
		 if (nLevel>0 && nLevel<10)
		 {//小于等级 10 才能升级

			nLevel +=nAddLevel;

			if (nLevel<=0)
				nLevel =1;

			nNewItemIdx = ItemSet.AddRong(0,nSeries,nLevel,nLuck,nDetailType,nParticularType,nItemLevel,nRItemLevel,NULL,nVersion,nGndrom,1,nEnChance,nRPoint,nPoint,0,NULL,0,0,0,0,0,0,TRUE);
		 
			if (nNewItemIdx <= 0)
			{
				Lua_PushNumber(L, 0);
				return 1;
			}

			Player[nPlayerIndex].m_ItemList.Remove(nItemidx);//删除原来包袱的东西
			ItemSet.Remove(nItemidx);
			//------------------------------------------------------
/*			if (Item[nNewItemIdx].GetGenre()!=0 && !Item[nNewItemIdx].GetIsBang() && Item[nNewItemIdx].GetTime()->bYear<=0)
			{//不是装备的话,包袱里是否有这个物品
				int nIdx=Player[nPlayerIndex].m_ItemList.IsHaveItem(Item[nNewItemIdx].GetGenre(),\
					Item[nNewItemIdx].GetDetailType(),\
					Item[nNewItemIdx].GetParticular(),\
					Item[nNewItemIdx].GetSeries(),\
					Item[nNewItemIdx].GetLevel(),pos_equiproom);
				 //xep chong item
				if (nIdx>0 && Item[nIdx].IsStack() && Item[nNewItemIdx].CanStack(nIdx) && (Item[nIdx].GetStackNum()+nStackNum) <= MAX_STACKNUM) //是否可以叠加--手里和包袱里的是否相同
				{//包袱里的东西可以叠加					
                    nStackNum=Item[nIdx].GetStackNum()+nStackNum;
					
					Player[nPlayerIndex].m_ItemList.Remove(nIdx);//删除包袱的东西
					ItemSet.Remove(nIdx);
				}
			}*/
			//-------------------------------------------------------
			/*if (Player[nPlayerIndex].m_ItemList.GetPlayerItemCount()>=MAX_ITEMLIST_CONUT)
			{
				ItemSet.Remove(nNewItemIdx);
				Player[nPlayerIndex].m_ItemList.msgshow("提示:<color=yellow>物品超限,请清理储物空间<color>!");
				return 0;
			}*/
			
			int	x, y;
			if (Player[nPlayerIndex].m_ItemList.CheckCanPlaceInEquipment(Item[nNewItemIdx].GetWidth(), Item[nNewItemIdx].GetHeight(), &x, &y))
			{//同步客户端（包里）
				Player[nPlayerIndex].m_ItemList.Add(nNewItemIdx,pos_equiproom,x,y,0,0,nStackNum);  //包袱增加装备 同步装备--客户端
			}
			else
			{
				Player[nPlayerIndex].m_ItemList.msgshow("Hanh trang khong du khoang trong tu dong huy vat pham!");
				int	nIdx = Player[nPlayerIndex].m_ItemList.Hand();
				
				if (nIdx)
				{
					Player[nPlayerIndex].m_ItemList.Remove(nIdx);  //删除客户端的节点了
					ItemSet.Remove(nIdx);
				}
				Player[nPlayerIndex].m_ItemList.Remove(nNewItemIdx);  //删除客户端的节点了
				ItemSet.Remove(nNewItemIdx);
				return 0;
			}
		 }
	  }
	}
	Lua_PushNumber(L,nNewItemIdx);
	return 1;
}



int LuaGiveItemWings(Lua_State * L)
{	 	
	int nPlayerIndex = GetPlayerIndex(L);
	int nParamNum    = Lua_GetTopIndex(L);
	int nReg = 0; 
	if  (nParamNum<2) return 0;

    if (nPlayerIndex > 0)
	{  
	  int nItemidx   = (int)Lua_ValueToNumber(L,1);
	  int nWingLevel = (int)Lua_ValueToNumber(L,2); 

	  if (nItemidx >0 && nItemidx<MAX_ITEM)
	  {
		 int nItemClass		= Item[nItemidx].GetGenre();

		 if  (nItemClass!=0) return 0;

		 int nLevel			= Item[nItemidx].GetLevel();
		 int nRonPoit       = Item[nItemidx].IsRong();
		 if (nLevel>0 && nLevel<10)
		 {//小于等级 10 才能升级

			Item[nItemidx].SetChiBangRes(nWingLevel);
			
			if (nParamNum>2)
			{//附加的属性
				int nRonItemLevel = (int)Lua_ValueToNumber(L,3);
				int i;
				for (i = 0; i < 6; ++i)
				{//设置熔炼属性
					if (Item[nItemidx].GetItemParam()->nGeneratorLevel[i]<=0)
					{
						Item[nItemidx].GetItemParam()->nGeneratorLevel[i] = nRonItemLevel;
						break;
					}
				}

				if (nItemidx < 0 || nItemidx >= MAX_ITEM) return 0;
				if (i>=0 && i< 6)
				  Item[nItemidx].SetRPoint(nRonPoit+1);

				Player[nPlayerIndex].m_ItemList.SynPlayerItem(nItemidx); //同步物品属性

				nReg = nItemidx; 
			}
		 }
	  }
	}

	Lua_PushNumber(L,nReg);
	return 1;
}


int LuaCheckItemReqability(Lua_State * L)
{//0 为男 1 为女
	int nPlayerIndex = GetPlayerIndex(L),nReg=0;
	if (nPlayerIndex > 0)
	{
		int nItemidx = (int)Lua_ValueToNumber(L,1);
		int nResType = (int)Lua_ValueToNumber(L,2);

		if (nItemidx >0 && nItemidx<MAX_ITEM)
		{
			if (Item[nItemidx].CheckReqability(nResType))
			{//有这个需求属性
				nReg=1;
			}
		}
	}
	Lua_PushNumber(L,nReg);
	return 1;
}


int LuaSetItemRes(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{

	  int nItemidx = (int)Lua_ValueToNumber(L,1);
	  int nResType = (int)Lua_ValueToNumber(L,2);

	  if (nItemidx < 0 || nItemidx >= MAX_ITEM) return 0;
	  Item[nItemidx].SetItemResIdx(nResType);

	  Player[nPlayerIndex].m_ItemList.SynPlayerItem(nItemidx); //同步物品属性
	}
   return 0;
}

/*
ItemPos Pos;
if (!Player[CLIENT_PLAYER_INDEX].m_ItemList.SearchPosition(3,4,&Pos))
{//查找位置


*/

int LuaCountEquipPlayer(Lua_State * L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    int nCount = 0;
    int nItem = 0;
    if (nPlayerIndex > 0)
    {
        for (int i = 0; i < itempart_num; ++i)
        {
            nItem = Player[nPlayerIndex].m_ItemList.GetEquipment(i);
            if (nItem > 0)
                nCount ++;
        }
    }
    Lua_PushNumber(L, nCount);
    return 1;
}

/*
SetRevivalPos(subworldid = -1, revid )
功能：设置Npc重生点
*/



int LuaGetPlayerRevivalPos(Lua_State * L)
{
	int nPlayerIndex = 0;
	    nPlayerIndex = GetPlayerIndex(L);
	int nSubWorldId = 0;
	int nXpos = 0;
	int nYpos = 0;
	if (nPlayerIndex < 0) 
	{
		return 0;
	}
	Player[nPlayerIndex].GetLoginRevivalPos(&nSubWorldId,&nXpos,&nYpos);

	Lua_PushNumber(L, nSubWorldId);
	Lua_PushNumber(L, nXpos);
	Lua_PushNumber(L, nYpos);
	return 3;
}

int LuaSetPlayerRevivalPos(Lua_State * L)
{
	//Question
	int nParamCount = Lua_GetTopIndex(L);
	int nPlayerIndex = 0;
	    nPlayerIndex = GetPlayerIndex(L);
	int nSubWorldId = 0;
	int nRevId = 0;
	if (nPlayerIndex < 0) 
	{
		return 0;
	}
	
	if (nParamCount >= 2)
	{
		nSubWorldId = (int) Lua_ValueToNumber(L, 1);
		nRevId = (int) Lua_ValueToNumber(L, 2);
	}
	else if (nParamCount == 1)
	{
		nSubWorldId = -1;
		nRevId = (int) Lua_ValueToNumber(L, 1);
	}
	else 
	{
		return 0;
	}
	
	Player[nPlayerIndex].SetRevivalPos(nSubWorldId, nRevId);
	return 0;
}
//设置临时的死亡复活点
//SetTempRev(worldid, x, y)
int LuaSetDeathRevivalPos(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	
	if (nPlayerIndex <= 0) 
		return 0;

	int nParamCount = Lua_GetTopIndex(L);
	if (nParamCount<2)
		return 0;
	//PLAYER_REVIVAL_POS * pTempRev = Player[nPlayerIndex].GetDeathRevivalPos();

	int m_nSubWorldId,m_nXpos,n_nYpos;
	    m_nSubWorldId = (int) Lua_ValueToNumber(L, 1);
		m_nXpos       = (int) Lua_ValueToNumber(L, 2);
        n_nYpos       = (int) Lua_ValueToNumber(L, 3);

	Player[nPlayerIndex].SetDeathRevivalPos(m_nSubWorldId,m_nXpos*32,n_nYpos*32);
    
	return 0;
}

//**************************************************************************************************************************************************************
//												聊天消息脚本
//**************************************************************************************************************************************************************

//**********************************************************************************************
//							主角属性获得
//**********************************************************************************************


#define MacroFun_GetPlayerInfoInt(L, MemberName) { int nPlayerIndex = GetPlayerIndex(L);\
	if (nPlayerIndex > 0){	int nNpcIndex = Player[nPlayerIndex].m_nIndex;	if (nNpcIndex > 0)Lua_PushNumber(L, Npc[nNpcIndex].MemberName);\
	else Lua_PushNil(L);}\
	else Lua_PushNil(L);\
return 1;}														

//临时当前阵营
int LuaGetPlayerCurrentCamp(Lua_State * L)
{
	MacroFun_GetPlayerInfoInt(L, m_CurrentCamp);
}
//实际阵营
int LuaGetPlayerCamp(Lua_State * L)
{
	MacroFun_GetPlayerInfoInt(L, m_Camp);
}


int LuaSetNpcCamp(Lua_State * L)
{
	int nNpcIdx = (int)Lua_ValueToNumber(L,1);
	int nValue = (int)Lua_ValueToNumber(L,2);
	if (nValue < 0 ) 
		return 0;
	
	if (nNpcIdx < 0 || nNpcIdx >= MAX_NPC) return 0;
	Npc[nNpcIdx].SetCamp(nValue);

	return 0;
}



int LuaSetPlayerCamp(Lua_State * L)
{
	int nValue = (int)Lua_ValueToNumber(L,1);
	if (nValue < 0 ) return 0;
	
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		Npc[Player[nPlayerIndex].m_nIndex].SetCamp(nValue);
	}
	return 0;
}

//IsBoss(nNpcID,bossTYPE)
int LuaIsBoss(Lua_State * L)
{
	int nValue = (int)Lua_ValueToNumber(L,1);
	if (nValue < 0 || nValue >= MAX_NPC) return 0;

	Npc[nValue].m_cGold.SetGoldCurrentType((int)Lua_ValueToNumber(L,2),Npc[nValue].m_SubWorldIndex);

	return 0;
}

int LuaGetNpcBoss(Lua_State * L)
{	
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 1)
		return 0 ;

	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);

	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC)
		return 0;

	Lua_PushNumber(L, Npc[nNpcIndex].m_cGold.GetGoldType());
	return 1;
}

int LuaSetPlayerCurrentCamp(Lua_State * L)
{
	int nValue = (int)Lua_ValueToNumber(L,1);
	if (nValue < 0 ) return 0;
	
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		Npc[Player[nPlayerIndex].m_nIndex].SetCurrentCamp(nValue);
	}
	return 0;
}

int LuaSetNpcCurCamp(Lua_State * L)
{
	int nNpcIndex = (int)Lua_ValueToNumber(L,1);
	if (nNpcIndex <= 0 && nNpcIndex > MAX_NPC) return 0;
	int nValue = (int )Lua_ValueToNumber(L,2);
	if (nValue >= camp_num) return 0;
	if (nNpcIndex < 0 || nNpcIndex >= MAX_NPC) return 0;
	Npc[nNpcIndex].SetCurrentCamp(nValue);
	return 0;
}


int LuaSetMapCostType(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);

	if (nPlayerIndex > 0)
	{
		int nMapIdex  = (int)Lua_ValueToNumber(L,1);
		int nCostType = (int)Lua_ValueToNumber(L,2);
		int nCostVal  = (int)Lua_ValueToNumber(L,3);
		int ncTime    = (int)Lua_ValueToNumber(L,4);
		Npc[Player[nPlayerIndex].m_nIndex].m_MapUseModel.nMagicAttrib=nMapIdex;//所在的地图
		Npc[Player[nPlayerIndex].m_nIndex].m_MapUseModel.nValue[0]=	nCostType; //消耗的类型
		Npc[Player[nPlayerIndex].m_nIndex].m_MapUseModel.nValue[1]=	nCostVal;  //消耗的数量
		Npc[Player[nPlayerIndex].m_nIndex].m_MapUseModel.nTime=	ncTime;        //间隔多久消耗一次
	}
	return 0;
}


int LuaGetMapCostType(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	
	if (nPlayerIndex > 0)
	{
	    Lua_PushNumber(L,Npc[Player[nPlayerIndex].m_nIndex].m_MapUseModel.nMagicAttrib);   //所在的地图
	    Lua_PushNumber(L,Npc[Player[nPlayerIndex].m_nIndex].m_MapUseModel.nValue[0]);      //消耗的类型
	    Lua_PushNumber(L,Npc[Player[nPlayerIndex].m_nIndex].m_MapUseModel.nValue[1]);	   //消耗的数量
	    Lua_PushNumber(L,Npc[Player[nPlayerIndex].m_nIndex].m_MapUseModel.nTime);		   //间隔多久消耗一次      
	}
	else
	{
		Lua_PushNumber(L,0);
		Lua_PushNumber(L,0);
		Lua_PushNumber(L,0);
		Lua_PushNumber(L,0);
	}

	return 4;
}


int LuaRestorePlayerCamp(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);

	if (nPlayerIndex > 0)
	{
		Npc[Player[nPlayerIndex].m_nIndex].RestoreCurrentCamp();
		//清空脚本
		Player[nPlayerIndex].m_dwDeathScriptId =0;
		Npc[Player[nPlayerIndex].m_nIndex].m_ReviceNum=0;
	}
	return 0;
}


int LuaSetTonMembersInfo(Lua_State * L)
{
	char *nTonName=(char *)Lua_ValueToString(L, 1);
	int  nWarCity=(int)Lua_ValueToNumber(L, 2);    //设置占帮城市地图
	int  nComp=(int)Lua_ValueToNumber(L, 3);       //整个帮会的阵营

	if (nTonName==NULL)
		return 0;

	int nCount=0;

	int nIndex = PlayerSet.GetFirstPlayer(); //查找玩家姓名
	
	while(nIndex > 0)
	{
		//if (SubWorld[Npc[Player[nIndex].m_nIndex].m_SubWorldIndex].m_SubWorldID==nSubWorldIdx)
		if (Player[nIndex].m_cTong.m_nFlag && strstr(Player[nIndex].m_cTong.m_szName,nTonName))
		{//帮会相同
			
			if (nWarCity>-2)
				Npc[Player[nIndex].m_nIndex].m_IsWarCity=nWarCity;

			if (nComp>-1)
				Npc[Player[nIndex].m_nIndex].SetCurrentCamp(nComp);

			nCount++;
		}
		nIndex = PlayerSet.GetNextPlayer();
	}
	
	Lua_PushNumber(L,nCount);

	return 	1;
}


//打开创建帮会窗口
int LuaOpenTong(Lua_State * L)
{
	//if (Lua_GetTopIndex(L) < 1) 
		//return 0;
	
	int nPlayerIndex;

#ifndef _SERVER
	      nPlayerIndex = CLIENT_PLAYER_INDEX;
#else
          nPlayerIndex = GetPlayerIndex(L);
#endif

	if (nPlayerIndex < 0)
		return 0;
	
	PLAYER_SCRIPTACTION_SYNC UiInfo;
	ZeroStruct(UiInfo);
	UiInfo.m_bUIId = UI_OPENTONGUI;
	UiInfo.m_bOptionNum = 0;
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
//	Player[nPlayerIndex].m_nCheckWaiGua =0;
	int nMsgId = 0;

	UiInfo.m_bParam1 = 0;
	UiInfo.m_nBufferLen = sizeof(int);
	
#ifndef _SERVER
	UiInfo.m_bParam2 = 0;
	Player[nPlayerIndex].DoScriptAction(&UiInfo);
#else
	UiInfo.m_bParam2 = 1;
	UiInfo.ProtocolType = (BYTE)s2c_scriptaction;
	UiInfo.m_wProtocolLong = sizeof(PLAYER_SCRIPTACTION_SYNC) - MAX_SCIRPTACTION_BUFFERNUM + UiInfo.m_nBufferLen - 1;
	g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, (BYTE*)&UiInfo, UiInfo.m_wProtocolLong + 1 );
#endif
	return 0;
}


int LuaChangePlayerFaction(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);				
	int nResult = 0;
	if (nPlayerIndex > 0)							
	{
		const char * szFactionName = Lua_ValueToString(L,1);
		Player[nPlayerIndex].LeaveCurFaction();
		
		if (NULL == szFactionName || strlen(szFactionName) == 0) 
		{
			nResult = 1;
		}
		else
		{
			nResult = Player[nPlayerIndex].AddFaction((char *)szFactionName);
		}
	}
	Lua_PushNumber(L, nResult);
	return 1;
}

int LuaClearFactionIfnfo(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);				
	if (nPlayerIndex > 0)							
	{
	   Player[nPlayerIndex].m_cFaction.Release();//
	}
	//Lua_PushNumber(L, nResult);
	return 0;
}

//抗性 *************************************************************************************
//0表示当前,1表示原始的,2表示最大的
int LuaGetPlayerColdResist(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		int nType = (int)Lua_ValueToNumber(L,1);
		switch((int)Lua_ValueToNumber(L,1))
		{
		case 0:
			Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_CurrentColdResist);break;
		case 1:
			Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_ColdResist); break;
		case 2:
			Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_ColdResistMax);break;
		default:
			Lua_PushNil(L);
		} 
	}
	else 
		Lua_PushNil(L);
	return 1;
}

int LuaSetPlayerColdResist(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		int nValue = (int)Lua_ValueToNumber(L,2);
		if (nValue < 0) nValue = 0;
		if (nValue > Npc[Player[nPlayerIndex].m_nIndex].m_ColdResistMax) nValue = Npc[Player[nPlayerIndex].m_nIndex].m_ColdResistMax;
		
		int nType = (int)Lua_ValueToNumber(L,1);
		
		switch((int)Lua_ValueToNumber(L,1))
		{
		case 0:
			Npc[Player[nPlayerIndex].m_nIndex].m_CurrentColdResist = nValue;
			break;
			
		case 1:
			Npc[Player[nPlayerIndex].m_nIndex].m_ColdResist = nValue;
			break;
		case 2:
			Npc[Player[nPlayerIndex].m_nIndex].m_ColdResistMax = nValue;
			break;
		}
		
		
	} 
	return 0;
	
}

int LuaGetPlayerFireResist(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		
		int nType = (int)Lua_ValueToNumber(L,1);
		switch((int)Lua_ValueToNumber(L,1))
		{
		case 0:
			Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_CurrentFireResist);break;
		case 1:
			Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_FireResist); break;
		case 2:
			Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_FireResistMax);break;
		default:
			Lua_PushNil(L);
		}
	} 
	else 
		Lua_PushNil(L);
	return 1;
}

int LuaSetPlayerFireResist(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		
		
		int nValue = (int)Lua_ValueToNumber(L,2);
		if (nValue < 0) nValue = 0;
		if (nValue > Npc[Player[nPlayerIndex].m_nIndex].m_FireResistMax) 
			nValue = Npc[Player[nPlayerIndex].m_nIndex].m_FireResistMax;
		
		int nType = (int)Lua_ValueToNumber(L,1);
		
		switch((int)Lua_ValueToNumber(L,1))
		{
		case 0:
			Npc[Player[nPlayerIndex].m_nIndex].m_CurrentFireResist = nValue;
			break;	
		case 1:
			Npc[Player[nPlayerIndex].m_nIndex].m_FireResist = nValue;
			break;
		case 2:
			Npc[Player[nPlayerIndex].m_nIndex].m_FireResistMax = nValue;
			break;
		}
		
		
	} 
	return 0;
	
}


int LuaGetPlayerLightResist(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		int nType = (int)Lua_ValueToNumber(L,1);
		switch((int)Lua_ValueToNumber(L,1))
		{
		case 0:
			Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_CurrentLightResist);break;
		case 1:
			Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_LightResist); break;
		case 2:
			Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_LightResistMax);break;
		default:
			Lua_PushNil(L);
		}
	} 
	else
		Lua_PushNil(L);
	return 1;
}

int LuaSetPlayerLightResist(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		
		int nValue = (int)Lua_ValueToNumber(L,2);
		if (nValue < 0) nValue = 0;
		if (nValue > Npc[Player[nPlayerIndex].m_nIndex].m_LightResistMax) nValue = Npc[Player[nPlayerIndex].m_nIndex].m_LightResistMax;
		
		int nType = (int)Lua_ValueToNumber(L,1);
		
		switch((int)Lua_ValueToNumber(L,1))
		{
		case 0:
			Npc[Player[nPlayerIndex].m_nIndex].m_CurrentLightResist = nValue;
			break;
			
		case 1:
			Npc[Player[nPlayerIndex].m_nIndex].m_LightResist = nValue;
			break;
		case 2:
			Npc[Player[nPlayerIndex].m_nIndex].m_LightResistMax = nValue;
			break;
		}
	} 
	else 
		Lua_PushNil(L);
	return 0;
}


int LuaGetPlayerPoisonResist(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		int nType = (int)Lua_ValueToNumber(L,1);
		switch((int)Lua_ValueToNumber(L,1))
		{
		case 0:
			Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_CurrentPoisonResist);break;
		case 1:
			Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_PoisonResist); break;
		case 2:
			Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_PoisonResistMax);break;
		default:
			Lua_PushNil(L);
		}
	} 
	else
		Lua_PushNil(L);
	return 1;
}

int LuaSetPlayerPoisonResist(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		{
			int nValue = (int)Lua_ValueToNumber(L,2);
            if (nValue < 0) nValue = 0;
            if (nValue > Npc[Player[nPlayerIndex].m_nIndex].m_PoisonResistMax) nValue = Npc[Player[nPlayerIndex].m_nIndex].m_PoisonResistMax;
			
            int nType = (int)Lua_ValueToNumber(L,1);
			
			switch((int)Lua_ValueToNumber(L,1))
			{
			case 0:
				Npc[Player[nPlayerIndex].m_nIndex].m_CurrentPoisonResist = nValue;
				break;
				
			case 1:
				Npc[Player[nPlayerIndex].m_nIndex].m_PoisonResist = nValue;
                break;
			case 2:
				Npc[Player[nPlayerIndex].m_nIndex].m_PoisonResistMax = nValue;
                break;
			}
		}
		
	} 

	return 0;
	
}

int LuaGetPlayerPhysicsResist(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		
		{
			int nType = (int)Lua_ValueToNumber(L,1);
			switch((int)Lua_ValueToNumber(L,1))
			{
			case 0:
				Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_CurrentPhysicsResist);break;
			case 1:
				Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_PhysicsResist); break;
			case 2:
				Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_PhysicsResistMax);break;
			default:
				Lua_PushNil(L);
			}
		}	
	} 
	else
		Lua_PushNil(L);
	return 1;
}

int LuaSetPlayerPhysicsResist(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{		
		{
			int nValue = (int)Lua_ValueToNumber(L,2);
            if (nValue < 0) nValue = 0;
            if (nValue > Npc[Player[nPlayerIndex].m_nIndex].m_PhysicsResistMax) nValue = Npc[Player[nPlayerIndex].m_nIndex].m_PhysicsResistMax;
			
            int nType = (int)Lua_ValueToNumber(L,1);
			
			switch((int)Lua_ValueToNumber(L,1))
			{
			case 0:
				Npc[Player[nPlayerIndex].m_nIndex].m_CurrentPhysicsResist = nValue;
				break;
				
			case 1:
				Npc[Player[nPlayerIndex].m_nIndex].m_PhysicsResist = nValue;
                break;
			case 2:
				Npc[Player[nPlayerIndex].m_nIndex].m_PhysicsResistMax = nValue;
                break;
			}
		}
	} 
	return 0;
	
}

int LuaGetNextExp(Lua_State *L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
        Lua_PushNumber(L, Player[nPlayerIndex].m_nNextLevelExp);
	}
	else
		Lua_PushNil(L);
	return 1;
}

//经验值*********************************************************************
int LuaGetPlayerExp(Lua_State *L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
        Lua_PushNumber(L, Player[nPlayerIndex].m_nExp);
	}
	else
		Lua_PushNil(L);
	return 1;
}

//AddExp(200,10,0)
int LuaModifyPlayerExp(Lua_State * L)
{
	int bAllTeamGet = 0;
	if(Lua_GetTopIndex(L) >= 3)	
		bAllTeamGet = (int)Lua_ValueToNumber(L,3);

	int nDValue = (int)Lua_ValueToNumber(L,1);
	int nTarLevel = (int)Lua_ValueToNumber(L,2);

	int nPlayerIndex = GetPlayerIndex(L);				
	if (nPlayerIndex > 0)								
	{									
		if (bAllTeamGet)
			Player[nPlayerIndex].AddExp(nDValue, nTarLevel,0);
		else
			Player[nPlayerIndex].AddSelfExp(nDValue, nTarLevel,0);
	}
	return 0;
}

int LuaAddOwnExp(Lua_State * L)
{
	if (Lua_GetTopIndex(L) <=0 ) return 0;
	int nPlayerIndex = GetPlayerIndex(L);				
	if (nPlayerIndex > 0)								
	{									
		int nExp = (int)Lua_ValueToNumber(L,1);
		if (nExp >= 0)
			Player[nPlayerIndex].DirectAddExp((int)Lua_ValueToNumber(L,1));
	}
	return 0;
}

int LuaAddExpCongDon(Lua_State * L)
{
    if (Lua_GetTopIndex(L) < 1 )
        return 0;
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0)
        return 0;
        
    int nExp = (int)Lua_ValueToNumber(L,1);
    if (nExp > 0)
    {
        while (nExp > 0)
        {
            int nExpAdd = Player[nPlayerIndex].m_nNextLevelExp - Player[nPlayerIndex].m_nExp;  //离升级还有多少经验
            if (nExp >= nExpAdd) //可以升级
            {//经验减少
                nExp = nExp - nExpAdd;
            }
            else
            {
                nExpAdd = nExp;
                nExp = 0;
            }
            Player[nPlayerIndex].DirectAddExp(nExpAdd);
        }
    }
    return 0;
}


int LuaGetPlayerLevel(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_Level);
	} 
	else
		Lua_PushNil(L);
	return 1;
}

int LuaGetPlayerLife(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		
		{
			int nType = (int)Lua_ValueToNumber(L,1);
			switch((int)Lua_ValueToNumber(L,1))
			{
			case 0:
				Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_CurrentLife);break;
			case 1:
				Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_LifeMax); break;
			case 2:
				Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_LifeMax);break;
			default:
				Lua_PushNil(L);
			}
		}
		
	} 
	else
		Lua_PushNil(L);
	return 1;
}

int LuaRestorePlayerLife(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		Npc[Player[nPlayerIndex].m_nIndex].RestoreLife();
	}
	return 0;
}

int LuaRestorePlayerMana(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		Npc[Player[nPlayerIndex].m_nIndex].RestoreMana();
	}
	return 0;
}

int LuaRestorePlayerStamina(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		Npc[Player[nPlayerIndex].m_nIndex].RestoreStamina();
	}
	return 0;
}

int LuaGetPlayerLifeReplenish(Lua_State * L)
{
	MacroFun_GetPlayerInfoInt( L, m_LifeReplenish);
}


int LuaGetPlayerMana(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		
		{
			int nType = (int)Lua_ValueToNumber(L,1);
			switch((int)Lua_ValueToNumber(L,1))
			{
			case 0:
				Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_CurrentMana);break;
			case 1:
				Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_ManaMax); break;
			case 2:
				Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_ManaMax);break;
			default:
				Lua_PushNil(L);
			}
		}
		
	} 
	else
		Lua_PushNil(L);
	return 1;
}

int LuaGetPlayerStamina(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		
		{
			int nType = (int)Lua_ValueToNumber(L,1);
			switch((int)Lua_ValueToNumber(L,1))
			{
			case 0:
				Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_CurrentStamina);break;
			case 1:
				Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_StaminaMax); break;
			case 2:
				Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_StaminaMax);break;
			default:
				Lua_PushNil(L);
			}
		}	
	} 
	else
		Lua_PushNil(L);
	return 1;
}


int LuaGetPlayerManaReplenish(Lua_State * L)
{
	MacroFun_GetPlayerInfoInt(L , m_ManaReplenish);
}

int LuaGetPlayerDefend(Lua_State * L)
{
    MacroFun_GetPlayerInfoInt(L , m_Defend);
}

int LuaGetPlayerSex(Lua_State * L)
{
	MacroFun_GetPlayerInfoInt(L , m_nSex);
}

int LuaGetPlayerIndex(Lua_State * L)
{
	MacroFun_GetPlayerInfoInt(L , GetPlayerIdx());
	return 0;
}

int LuaGetPlayerNpcIdx(Lua_State * L)
{
    int nPlayerIndex = GetPlayerIndex(L);
	int nIndex = nPlayerIndex;
	if (nPlayerIndex > 0)
	{
		if (Lua_GetTopIndex(L) > 1)
			nIndex = (int)Lua_ValueToNumber(L, 1);
        if (nIndex < 0 || nIndex >= MAX_PLAYER) { Lua_PushNumber(L, 0); return 1; }
        Lua_PushNumber(L, Player[nIndex].m_nIndex);
	}
	else
		Lua_PushNumber(L, 0);
	return 1;
}

int LuaGetPlayerSeries(Lua_State * L)
{
	MacroFun_GetPlayerInfoInt(L , m_Series);
}



int LuaGetIsMagic(Lua_State * L)
{
	int nItemidx = (int)Lua_ValueToNumber(L,1);

	if  (nItemidx>0 && nItemidx<MAX_ITEM)
	{
		Lua_PushNumber(L,Item[nItemidx].GetIsMagic());   //是否有魔法属性);
		return 1;
	}

	Lua_PushNumber(L,0);
	return 1;
}

int LuaSetIsMagic(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		int nItemidx = (int)Lua_ValueToNumber(L,1);
		int nVer    = (int)Lua_ValueToNumber(L,2);
		int nSyn    = (int)Lua_ValueToNumber(L,3);

		if (nItemidx>0 && nItemidx<MAX_ITEM)
		{
			Item[nItemidx].SetMagic(nVer);   //是否有魔法属性
			if  (nSyn)
			   Player[nPlayerIndex].m_ItemList.SynPlayerItem(nItemidx); //同步物品属性
		}       
	}

	return 0;
}


int LuaSetCurDurability(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
	  int nItemidx = (int)Lua_ValueToNumber(L,1);
	  int nability   = (int)Lua_ValueToNumber(L,2);

	  if  (nability>-2)
	  {
		  if (nItemidx < 0 || nItemidx >= MAX_ITEM) return 0;
		  if (nability>Item[nItemidx].GetMaxDurability())
			  nability=Item[nItemidx].GetMaxDurability();

		  Item[nItemidx].SetDurability(nability);
	  }

	  Player[nPlayerIndex].m_ItemList.SynPlayerItem(nItemidx); //同步物品属性
	}

	return 0;
}


int LuaGetCurDurability(Lua_State * L)
{
	int nItemidx = (int)Lua_ValueToNumber(L,1);

	if  (nItemidx>0 && nItemidx<MAX_ITEM)
	{
		Lua_PushNumber(L,Item[nItemidx].GetDurability());
		return 1;
	}
	Lua_PushNumber(L,-2);
	return 1;
}


int LuaGetMaxDurability(Lua_State * L)
{
	
	int nItemidx = (int)Lua_ValueToNumber(L,1);

	if  (nItemidx>0 && nItemidx<MAX_ITEM)
	{
		Lua_PushNumber(L,Item[nItemidx].GetMaxDurability());
		return 1;
	}

	Lua_PushNumber(L,0);
	return 1;
}

//设置是否绑定 快捷 五行
int LuaSetAndSycItem(Lua_State * L)
{
    int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		int nItemidx = (int)Lua_ValueToNumber(L,1);
        int nWuHang = (int)Lua_ValueToNumber(L,2);
		int nBang   = (int)Lua_ValueToNumber(L,3);
        int nkuaiJie= (int)Lua_ValueToNumber(L,4);
		int nCanUse= (int)Lua_ValueToNumber(L,5);
		int nStackNum= (int)Lua_ValueToNumber(L,6);
		int nability= (int)Lua_ValueToNumber(L,7);//修复持久
		int nLevel= (int)Lua_ValueToNumber(L,8);

		if (nItemidx < 0 || nItemidx >= MAX_ITEM) return 0;
		if  (nWuHang>-1)
            Item[nItemidx].SetSeries(nWuHang);    //五行
		if  (nBang>-1)
            Item[nItemidx].SetBang(nBang);        //是否绑定
		if  (nkuaiJie>-1)
            Item[nItemidx].SetKuaiJie(nkuaiJie);  //是否可以快捷栏
        if  (nCanUse>-1)
            Item[nItemidx].SetIsCanUse(nCanUse);  //是否可以使用
		if  (nStackNum>0)
		    Item[nItemidx].SetStackNum(nStackNum);//叠加数量

        if  (nability>-2)
		{
            if (nability>=Item[nItemidx].GetMaxDurability())
			   nability=Item[nItemidx].GetMaxDurability();

             Item[nItemidx].SetDurability(nability);
		}

		if  (nLevel>0)
			Item[nItemidx].SetLevel(nLevel);      //设置等级

		Player[nPlayerIndex].m_ItemList.SynPlayerItem(nItemidx); //同步物品属性
	}
	return 0;
}

//Bo set set trang bi va cac vat pham khac SetSpecItemParam
int LuaSetSpecItemParam(Lua_State *L) {
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex > 0) {
        int nItemidx = (int)Lua_ValueToNumber(L, 1); // ID item
        int key = (int)Lua_ValueToNumber(L, 2);      // type (key)
        int value = (int)Lua_ValueToNumber(L, 3);    // (value)

        switch (key) {
            case 1: // SetSeries (ngu hanh)
                if (nItemidx < 0 || nItemidx >= MAX_ITEM) return 0;
                if (value > -1)
                    Item[nItemidx].SetSeries(value);
                break;
            case 2: // SetBang (bao hiem)
                if (nItemidx < 0 || nItemidx >= MAX_ITEM) return 0;
                if (value > -1)
                    Item[nItemidx].SetBang(value);
                break;
            case 3: // SetKuaiJie (co the dat vao phim tat)
                if (nItemidx < 0 || nItemidx >= MAX_ITEM) return 0;
                if (value > -1)
                    Item[nItemidx].SetKuaiJie(value);
                break;
            case 4: // SetIsCanUse (co the su dung)
                if (nItemidx < 0 || nItemidx >= MAX_ITEM) return 0;
                if (value > -1)
                    Item[nItemidx].SetIsCanUse(value);
                break;
            case 5: // SetStackNum (so luong stack)
                if (nItemidx < 0 || nItemidx >= MAX_ITEM) return 0;
                if (value > 0)
                    Item[nItemidx].SetStackNum(value);
                break;
            case 6: // SetDurability (do ben)
                if (value > -2) {
                    if (nItemidx < 0 || nItemidx >= MAX_ITEM) return 0;
                    if (value >= Item[nItemidx].GetMaxDurability())
                        value = Item[nItemidx].GetMaxDurability();
                    Item[nItemidx].SetDurability(value);
                }
                break;
            case 7: // SetLevel 
                if (nItemidx < 0 || nItemidx >= MAX_ITEM) return 0;
                if (value > 0)
                    Item[nItemidx].SetLevel(value);
                break;
            default:
                printf("Unknown key: %d\n", key); 
                break;
        }

        Player[nPlayerIndex].m_ItemList.SynPlayerItem(nItemidx);
    }
    return 0;
}
//lay thong tin chi so GetSpecItemParam
int LuaGetSpecItemParam(Lua_State *L) {
    int nItemIdx = (int)Lua_ValueToNumber(L, 1); 
    int key = (int)Lua_ValueToNumber(L, 2);      

    if (nItemIdx >= 0 && nItemIdx < MAX_ITEM) { 
        int value = 0;

        switch (key) {
            case 1: // GetSeries 
                value = Item[nItemIdx].GetSeries();
                break;
            case 2: // GetBang 
                value = Item[nItemIdx].GetIsBang();
                break;
            case 3: // GetKuaiJie
                value = Item[nItemIdx].GetIsKuaiJie();
                break;
            case 4: // GetIsCanUse
                value = Item[nItemIdx].GetIsCanUse();
                break;
            case 5: // GetStackNum 
                value = Item[nItemIdx].GetStackNum();
                break;
            case 6: // SetDurability (do ben)
                    value = Item[nItemIdx].GetDurability();
                break;
            case 7: // GetLevel 
                value = Item[nItemIdx].GetLevel();
                break;
            default:
                printf("Unknown key: %d\n", key); // Key loi
                break;
        }

        // Tra gia tri ve Lua
        Lua_PushNumber(L, value);
        return 1; // Ham tra ve 1 gia tri
    }

    // Neu ID khong hpp le
    Lua_PushNumber(L, -1); // Gia tri loi
    return 1;
}

/*
	m_sPortalPos.m_nSubWorldId = 0;
	m_sPortalPos.m_nTime = 0;
	m_sPortalPos.m_nMpsX = 0;
	m_sPortalPos.m_nMpsY = 0;

*/



int LuaNewPayExtPoint(Lua_State* L)
{
	S3PDBConVBC* pConn;

	if (!CreateConnectio2n(&pConn))
	{
        printf("Failed to connect to DB\n");
		return 0;
	}
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0)
		return 0;

	char* szColumName = (char*)Lua_ValueToString(L, 1);
	int nPointValue = Lua_ValueToNumber(L, 2);
	char nstrSql[256];

	sprintf(nstrSql, "select Account_Info.%s from Account_Info where cAccName='%s'",SqlIdentifier(szColumName), SqlEscapeLiteral(Player[nPlayerIndex].m_AccoutName));
	//printf("%s\n", nstrSql);

	S3PResultVBC* pResult = NULL;

	if (pConn->QuerySql(nstrSql, &pResult))
	{
		int nNum = pResult->num_rows();
		if (nNum <= 0)
			return 0;
		else
		{
			_variant_t dVala;
			pResult->get_field_data(0, &dVala, sizeof(_variant_t));
			if (dVala.lVal <= 0)
				return 0;

			char UpstrSql[256];
			snprintf(UpstrSql, sizeof(UpstrSql), "update Account_Info set %s=(select %s from Account_Info where cAccName='%s') - %d where cAccName='%s'",SqlIdentifier(szColumName),SqlIdentifier(szColumName), SqlEscapeLiteral(Player[nPlayerIndex].m_AccoutName), nPointValue, SqlEscapeLiteral(Player[nPlayerIndex].m_AccoutName));
			printf("%s\n", UpstrSql);
			if (pConn->Do(UpstrSql))
			{
				Lua_PushNumber(L, 1);
			}
		}
	}

	if (pResult)
		pResult->unuse();

	pConn->CloseConnect();
	return 1;

}

int LuaNewGetExtPoint(Lua_State* L)
{
	S3PDBConVBC* pConn;
	if (!CreateConnectio2n(&pConn))
		return 0;

	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0)
		return 0;

	char* szColumName = (char*)Lua_ValueToString(L, 1);
	char nstrSql[256];

	sprintf(nstrSql, "select Account_Info.%s from Account_Info where cAccName='%s'",SqlIdentifier(szColumName), SqlEscapeLiteral(Player[nPlayerIndex].m_AccoutName));
	S3PResultVBC* pResult = NULL;

	if (pConn->QuerySql(nstrSql, &pResult))
	{
		int nNum = pResult->num_rows();
		if (nNum <= 0)
			return 0;
		else
		{
			_variant_t dVala;
			pResult->get_field_data(0, &dVala, sizeof(_variant_t));
			Lua_PushNumber(L, dVala.lVal);
		}
	}

	if (pResult)
		pResult->unuse();

	pConn->CloseConnect();
	return 1;
}

int LuaSetLeagueData(Lua_State * L)
{
	if  (!g_DataConn)
		return 0;
	char *nstrSql    = (char *)Lua_ValueToString(L,1);
	char *UpstrSql   = (char *)Lua_ValueToString(L,2);

	S3PResultVBC* pResult = NULL;

	if (g_DataConn->QuerySql(nstrSql, &pResult))
	{
		g_DataConn->Do(UpstrSql);			
	}
	
	if (pResult)
		pResult->unuse();

	return 0;
}



int LuaClearLeagueData(Lua_State * L)
{
	if  (!g_DataConn)
		return 0;

    char *nTableName    = (char *)Lua_ValueToString(L,1);
	S3PResultVBC* pResult = NULL;
    if (NULL == nTableName) return 0;
	char strSQL[200];//="select * from Fightskill";
	sprintf(strSQL,"select * from %s",SqlIdentifier(nTableName)); //Fightskill

	if (g_DataConn->QuerySql(strSQL, &pResult))
	{
		sprintf(strSQL,"Delete From %s",SqlIdentifier(nTableName));
		g_DataConn->Do(strSQL);

		sprintf(strSQL,"dbcc checkident('%s',reseed,0)",nTableName);
		g_DataConn->Do(strSQL);
	
	}

	if (pResult)
		pResult->unuse();

	return 0;
}

int LuaCheckLeagueData(Lua_State * L)
{
    char *strSql    = (char *)Lua_ValueToString(L,1);
	int nCount=0;
	S3PResultVBC* pResult = NULL;
	//char strSQL[200];//="select * from Fightskill";
	//sprintf(strSQL,"select * from %s where AttackName = '%s'",BiaoName,RoleName); //Fightskill
	if  (!g_DataConn)
		return 0;

	if (g_DataConn->QuerySql(strSql, &pResult))
	{
		nCount=pResult->num_rows();
	}

	if  (pResult)
		pResult->unuse();

	Lua_PushNumber(L,nCount);
	return 1;

}


int LuaGetLeagueDataInfo(Lua_State * L)
{
	if (!g_DataConn)
		return 0;

	S3PResultVBC* pResult = NULL;

	char *strSQL = (char *)Lua_ValueToString(L,1);
	int  nCount  = (int )Lua_ValueToNumber(L,2);

	char TempData[128]={0};
	int nNum=0,i,j;
	if (g_DataConn->QuerySql(strSQL, &pResult))
	{
	    nNum=pResult->num_rows();

		if 	(nNum>0)
		{
		  _variant_t dVala;

		  if (nNum==1)
		  {//只有一个数据的
			  if (nCount<=1)
			  { 
				pResult->get_field_data(0, &dVala, sizeof(_variant_t));
			    snprintf(TempData, sizeof(TempData), "%s", (char*)(_bstr_t)dVala);
			  } 
			  else
			  {  
				 for(i=0;i<nCount;i++)
				 {  
					pResult->get_field_data(i, &dVala, sizeof(_variant_t));
					strcat(TempData,(char*)(_bstr_t)dVala);
					if (i<nCount-1)
					{
					  strcat(TempData,",");
					}
				 } 
			  } 
		  }
		  else
		  {//有多条数据的
			  for(i=0;i<nNum;i++)
			  {
			    for(j=0;j<nCount;j++)
				{   
				  pResult->get_field_data(j, &dVala, sizeof(_variant_t));
				  strcat(TempData,(char*)(_bstr_t)dVala);
				  if (j<nCount-1)
				  {//最后一个不需要加逗号
					  strcat(TempData,",");
				  }
				}
				
				if (i<nNum-1)
				   strcat(TempData," ");

				pResult->data_seek(i,pResult->next);
			  }
		  }
		}
	}

	if (pResult)
		pResult->unuse();

	//printf("--%s--\n",TempData);
	Lua_PushString(L,TempData);
	//Lua_PushNumber(L,nNum);	
	return 1;
}


int LuaGetLeagueData(Lua_State * L)
{
	if (!g_DataConn)
		return 0;

	char *BiaoName = (char *)Lua_ValueToString(L,1);
	char *lieName = (char *)Lua_ValueToString(L,2);
	int  nRows    =	(int)Lua_ValueToNumber(L,3);
	int  paixu    = (int)Lua_ValueToNumber(L,4);
	char strSQL[200]={0};
	//sprintf(strSQL,"select * from %s where cUserCode = '%s'",BiaoName,RoleName); //Fightskill
	//sprintf(strSQL,"select AttackName,DeathNum,WinNum,EnemyName,RanKing,JoinCount,ProductJb,ProductJxb,ProductJf,SignMoney,IsSignUp from %s where %s>0 order by %s asc",BiaoName,lieName,lieName);	 //desc 按照降序排 从大到小 asc 从小到大
	if (paixu==0)
	   snprintf(strSQL, sizeof(strSQL),"select AttackName,ProductJb,ProductJxb,ProductJf,SignMoney,IsSignUp from %s where %s>0 order by %s asc",SqlIdentifier(BiaoName),SqlIdentifier(lieName),SqlIdentifier(lieName));	 //desc 按照降序排 从大到小 asc 从小到大
	else if (paixu==1)
	   snprintf(strSQL, sizeof(strSQL),"select AttackName,ProductJb,ProductJxb,ProductJf,SignMoney,IsSignUp from %s where %s>0 order by %s desc",SqlIdentifier(BiaoName),SqlIdentifier(lieName),SqlIdentifier(lieName));
	else
	   sprintf(strSQL,"select AttackName,ProductJb,ProductJxb,ProductJf,SignMoney,IsSignUp from %s",SqlIdentifier(BiaoName));

	S3PResultVBC* pResult = NULL;
	char LeagueData[128]={0};
	char TempData[128]={0};

	if (g_DataConn->QuerySql(strSQL, &pResult))
	{
		int nCount=pResult->num_rows();
		//pResult->data_seek(1,pResult->begin);

		if (nCount > 0) //如果有数据
		{
			for (int i=0;i<nRows;++i)
			{
			   if (i>nCount-1)
			   {
				  sprintf(TempData,"第%d名:暂无 积分:0\n",i+1);
				  strcat(LeagueData,TempData);
				  continue;
			   }

			   //pResult->data_seek(nRows,pResult->begin);
			  _variant_t dAttackName;
			  _variant_t dProductJb;
			  _variant_t dProductJxb;
			  _variant_t dProductJf;
			  _variant_t dSignMoney;
			  _variant_t dIsSignUp;

			  pResult->get_field_data(0, &dAttackName, sizeof(_variant_t));
			  pResult->get_field_data(1, &dProductJb, sizeof(_variant_t));
			  pResult->get_field_data(2, &dProductJxb, sizeof(_variant_t));
			  pResult->get_field_data(3, &dProductJf, sizeof(_variant_t));
			  pResult->get_field_data(4, &dSignMoney, sizeof(_variant_t));
			  pResult->get_field_data(5, &dIsSignUp, sizeof(_variant_t));

			  //printf("----第%d名:%s 积分:%s ----\n",i+1,(char*)(_bstr_t)dAttackName,(char*)(_bstr_t)dProductJf);
			  snprintf(TempData, sizeof(TempData),"第%d名:%s 积分:%s\n",i+1,(char*)(_bstr_t)dAttackName,(char*)(_bstr_t)dProductJf);
			  strcat(LeagueData,TempData);

			  pResult->data_seek(i,pResult->next);
			}
		}
		else
		{
		  sprintf(TempData,"暂无排名数据!");
		}
	}

	if (pResult)
		pResult->unuse();

   	Lua_PushString(L,LeagueData);
   return 1;
}


int LuaSetEnemyData(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 2)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	Lua_PushNumber(L, 1);
	return 1;
}

//导出联赛数据
int LuaExportLeagueData(Lua_State * L)
{
	if (!g_DataConn)
		return 0;
	
	//char *strTable = (char *)Lua_ValueToString(L,1); //表
	char *strSql = (char *)Lua_ValueToString(L,1);	 //语句
	int  nCols = (int )Lua_ValueToNumber(L,2);	     //多少个数据
	char *strFilePath = (char *)Lua_ValueToString(L,3);	     //导出的文件路径
	int  nIsOver=0;

	if 	(strSql==NULL || strFilePath==NULL)
		return 0;

	if  (!g_DataConn)
		return 0;
	//char strSQL[200]={0};//="select * from Fightskill";
	//char TempData[128]={0};
	//     sprintf(strSQL,"select AttackName,EnemyName,ProductJb,ProductJxb,ProductJf,SignMoney,IsSignUp from %s where AttackName <> '%s' and EnemyName is null",strTable,strName); //已经报名了
	S3PResultVBC* pResult = NULL;

	KTabFileCtrl nFileDir;
	if (nFileDir.Load(strFilePath))
	{
	    while(nFileDir.GetHeight()>0)
		{ 
		   nFileDir.Remove(nFileDir.GetHeight());
		   nFileDir.Save(strFilePath);
		}
	}

	try
	{
	   if (g_DataConn->QuerySql(strSql, &pResult))
	   {   
			 int nRows=pResult->num_rows();

			 if (nRows > 0)
			 {//开始导出数据
				 //sprintf(strSQL,"INSERT INTO %s(cUserCode,iFightSkill,iFightSkillLevel)VALUES('%s',%d,%d)",BiaoName,RoleName,nSkillID,nSkillLevel);	
				 //int i =g_Random(nRows);
				 //  pResult->data_seek(i,pResult->begin); //设置指针指向这条数据
				 for (int i=0;i<nRows;i++)                 //数据库有多少行
				 {	 
				    int nRow=0;
				        nRow=nFileDir.GetHeight();
				        nFileDir.InsertAfter(nRow);//在后面插入一行

					for (int j=0;j<nCols;j++) //要插入多少列
					{
					   _variant_t dDataStr;
					   pResult->get_field_data(j, &dDataStr, sizeof(_variant_t));
					   nFileDir.WriteString(nRow+1,j+1,(char*)(_bstr_t)dDataStr);
					}

					pResult->data_seek(i,pResult->next);
				 }
				 nFileDir.Save(strFilePath);
				 nIsOver=1;
				 nFileDir.Clear();

				 //g_DataConn->Do(strSqlb);
			 }
			 else
			 {
				 nFileDir.Clear();
			 }
			 
	   }
	   else
	   {  
	     nFileDir.Clear();
	   } 

	}   
	catch (...)
	{
	  printf("--数据库错误:%s--\n",strSql);
	}

	if (pResult)					
		pResult->unuse();

	Lua_PushNumber(L,nIsOver); 
	return 1;
}


int LuaUpLeagueData(Lua_State * L)
{
	if (!g_DataConn)
		return 0;

	char *strSqla = (char *)Lua_ValueToString(L,1);
	char *strSqlb= (char *)Lua_ValueToString(L,2);	//插入
	char *strSqlc = (char *)Lua_ValueToString(L,3);	//更新

	if  (strSqla==NULL)
		return 0;

	S3PResultVBC* pResult = NULL;
	//char strSQL[200];//="select * from Fightskill";
	//sprintf(strSQL,"select * from %s where AttackName = '%s'",BiaoName,RoleName); //Fightskill
	if  (!g_DataConn)
		return 0;

	if (g_DataConn->QuerySql(strSqla, &pResult))
	{
		if (pResult->num_rows() <= 0)
		{//插入数据
			//sprintf(strSQL,"INSERT INTO %s(cUserCode,iFightSkill,iFightSkillLevel)VALUES('%s',%d,%d)",BiaoName,RoleName,nSkillID,nSkillLevel);
		    if (strSqlb)	
			   g_DataConn->Do(strSqlb);
		}
		else
		{//更新数据
			if (strSqlb)
		       g_DataConn->Do(strSqlc);
		}					
	}


	if (pResult)
		pResult->unuse();
   
	return 0;

}

				   
int LuaTestData(Lua_State * L)
{
	if (!g_DataConn)
		return 0;

   char *BiaoName = (char *)Lua_ValueToString(L,1);
   char *RoleName = (char *)Lua_ValueToString(L,2);

   int  nSkillID =20,nSkillLevel=20;
	S3PResultVBC* pResult = NULL;
	char strSQL[200];//="select * from Fightskill";
	     sprintf(strSQL,"select * from %s where cUserCode = '%s'",SqlIdentifier(BiaoName),SqlEscapeLiteral(RoleName)); //Fightskill
	if (g_DataConn->QuerySql(strSQL, &pResult))
	{
		if (pResult->num_rows() <= 0)
		{
		    snprintf(strSQL, sizeof(strSQL),"INSERT INTO %s(cUserCode,iFightSkill,iFightSkillLevel)VALUES('%s',%d,%d)",SqlIdentifier(BiaoName),SqlEscapeLiteral(RoleName),nSkillID,nSkillLevel);	
			g_DataConn->Do(strSQL);	
			//AfxMessageBox("写入数据库成功!!");
		}
		else
		{
			snprintf(strSQL, sizeof(strSQL),"update %s set iFightSkill=%d,iFightSkillLevel=%d where cUserCode='%s'",SqlIdentifier(BiaoName),nSkillID,nSkillLevel,SqlEscapeLiteral(RoleName));
			g_DataConn->Do(strSQL);	
			//AfxMessageBox("更新数据库成功!!");
		}
	}

	if (pResult)
		pResult->unuse();


	return 0;
}

//---------------------------------城战相关-------------

int LuaSetCityWarData(Lua_State * L)
{
	if  (!g_CityConn)
		return 0;
	char *nstrSql    = (char *)Lua_ValueToString(L,1);
	char *UpstrSql   = (char *)Lua_ValueToString(L,2);
	
	S3PResultVBC* pResult = NULL;
	
	if (g_CityConn->QuerySql(nstrSql, &pResult))
	{
		g_CityConn->Do(UpstrSql);			
	}
	
	if (pResult)
		pResult->unuse();
	
	return 0;
}

int LuaClearCityWarData(Lua_State * L)
{
	if  (!g_CityConn)
		return 0;
	
    char *nTableName    = (char *)Lua_ValueToString(L,1);
	S3PResultVBC* pResult = NULL;
    if (NULL == nTableName) return 0;
	char strSQL[200];//="select * from Fightskill";
	sprintf(strSQL,"select * from %s",SqlIdentifier(nTableName)); //Fightskill
	
	if (g_CityConn->QuerySql(strSQL, &pResult))
	{
		sprintf(strSQL,"Delete From %s",SqlIdentifier(nTableName));
		g_CityConn->Do(strSQL);
		
		sprintf(strSQL,"dbcc checkident('%s',reseed,0)",nTableName);
		g_CityConn->Do(strSQL);
		
	}
	
	if (pResult)
		pResult->unuse();
	
	return 0;
}

int LuaCheckCityWarData(Lua_State * L)
{
	if  (!g_CityConn)
		return 0;

    char *strSql    = (char *)Lua_ValueToString(L,1);
	int nCount=0;
	S3PResultVBC* pResult = NULL;

	if (g_CityConn->QuerySql(strSql, &pResult))
	{
		nCount=pResult->num_rows();
	}
	
	if  (pResult)
		pResult->unuse();
	
	Lua_PushNumber(L,nCount);
	return 1;
}

int LuaGetCityWarData(Lua_State * L)
{
	if (!g_CityConn)
		return 0;

	char *BiaoName = (char *)Lua_ValueToString(L,1);
	char *lieName = (char *)Lua_ValueToString(L,2);
	int  nRows    =	(int)Lua_ValueToNumber(L,3);
	int  paixu    = (int)Lua_ValueToNumber(L,4);
	char strSQL[200];

	if (paixu==0)
	   snprintf(strSQL, sizeof(strSQL),"select AttackTongName,TongGxd,SingUpMoney,IsSingUp from %s where %s>0 order by %s asc",SqlIdentifier(BiaoName),SqlIdentifier(lieName),SqlIdentifier(lieName));	 //desc 按照降序排 从大到小 asc 从小到大
	else if (paixu==1)
	   snprintf(strSQL, sizeof(strSQL),"select AttackTongName,TongGxd,SingUpMoney,IsSingUp from %s where %s>0 order by %s desc",SqlIdentifier(BiaoName),SqlIdentifier(lieName),SqlIdentifier(lieName));
	else
	   sprintf(strSQL,"select AttackTongName,TongGxd,SingUpMoney,IsSingUp from %s",SqlIdentifier(BiaoName));

	S3PResultVBC* pResult = NULL;
	char LeagueData[128]={0};
	char TempData[128]={0};

	if (g_CityConn->QuerySql(strSQL, &pResult))
	{
		int nCount=pResult->num_rows();

		if (nCount > 0) //如果有数据
		{
			for (int i=0;i<nRows;++i)
			{
			   if (i>nCount-1)
			   {
				  sprintf(TempData,"第%d名:暂无 贡献度:0\n",i+1);
				  strcat(LeagueData,TempData);
				  continue;
			   }

			   //pResult->data_seek(nRows,pResult->begin);
			  _variant_t dAttackTongName;
			  _variant_t dTongGxd;
			  _variant_t dSingUpMoney;
			  _variant_t dIsSingUp;

			  pResult->get_field_data(0, &dAttackTongName, sizeof(_variant_t));
			  pResult->get_field_data(1, &dTongGxd, sizeof(_variant_t));
			  pResult->get_field_data(2, &dSingUpMoney, sizeof(_variant_t));
			  pResult->get_field_data(3, &dIsSingUp, sizeof(_variant_t));


			  //printf("----第%d名:%s 积分:%s ----\n",i+1,(char*)(_bstr_t)dAttackName,(char*)(_bstr_t)dProductJf);
			  snprintf(TempData, sizeof(TempData),"第%d名:%s 贡献度:%s\n",i+1,(char*)(_bstr_t)dAttackTongName,(char*)(_bstr_t)dTongGxd);
			  strcat(LeagueData,TempData);

			  pResult->data_seek(i,pResult->next);
			}
		}
		else
		{
		  sprintf(TempData,"暂无排名数据!");
		}
	}

	if (pResult)
		pResult->unuse();

   	Lua_PushString(L,LeagueData);
   return 1;
}

int LuaGetCityWarDataInfo(Lua_State * L)
{
	if (!g_CityConn)
		return 0;
	
	S3PResultVBC* pResult = NULL;
	
	char *strSQL = (char *)Lua_ValueToString(L,1);
	int  nCount  = (int )Lua_ValueToNumber(L,2);
	
	char TempData[NAME_LENGHT]={0};
	if (g_CityConn->QuerySql(strSQL, &pResult))
	{
		int nCount=pResult->num_rows();
		if 	(nCount>0)
		{
			_variant_t dVala;
			pResult->get_field_data(0, &dVala, sizeof(_variant_t));
			
			snprintf(TempData, sizeof(TempData), "%s", (char*)(_bstr_t)dVala);
			
		}
	}
	
	if (pResult)
		pResult->unuse();
	
	Lua_PushString(L,TempData);
	return 1;
}

int LuaUpCityWarData(Lua_State * L)
{
	if (!g_CityConn)
	{
		printf("------g_CityConn=NULL------- \n");
		return 0;
	}

	char *strSqla = (char *)Lua_ValueToString(L,1);
	char *strSqlb = (char *)Lua_ValueToString(L,2);	//插入
	char *strSqlc = (char *)Lua_ValueToString(L,3);	//更新
	S3PResultVBC* pResult = NULL;

	if (g_CityConn->QuerySql(strSqla, &pResult))
	{
		if (pResult->num_rows() <= 0)
		{	
			g_CityConn->Do(strSqlb);	
			printf("------插入成功------- \n");
		}
		else
		{
		   g_CityConn->Do(strSqlc);
		   printf("------更新成功------- \n");
		}
	}
	else
	  printf("------数据错误------- \n");

	if (pResult)
		pResult->unuse();
   
	return 0;
}

int LuaGetNpcMaptimeInfo(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);

	if (nPlayerIndex>0)
	{
		Lua_PushNumber(L,Player[nPlayerIndex].m_MapTimeInfo.nExpLeftTime); 
		Lua_PushNumber(L,Player[nPlayerIndex].m_MapTimeInfo.nLuckLeftTime);
		Lua_PushNumber(L,Player[nPlayerIndex].m_MapTimeInfo.nKangLeftTime);
		Lua_PushNumber(L,Player[nPlayerIndex].m_MapTimeInfo.nYaoLeftTime);
		Lua_PushNumber(L,Player[nPlayerIndex].m_MapTimeInfo.nSupManaMauLeftTime);
		Lua_PushNumber(L,Player[nPlayerIndex].m_MapTimeInfo.nSupKyNangLeftTime);
		Lua_PushNumber(L,Player[nPlayerIndex].m_MapTimeInfo.nNhatKyLeftTime);
	}
	else
	{
		Lua_PushNumber(L,0); 
		Lua_PushNumber(L,0);
		Lua_PushNumber(L,0);
		Lua_PushNumber(L,0);
		Lua_PushNumber(L,0);
		Lua_PushNumber(L,0);
		Lua_PushNumber(L,0);
	}
	
	return 7;
}





int LuaGetReturnPos(Lua_State * L)
{
/* int nPlayerIndex = GetPlayerIndex(L);
	  int nValue = 0;
	 if (nPlayerIndex > 0)
	 {
		Lua_PushNumber(L,Player[nPlayerIndex].m_sPortalPos.m_nSubWorldId); 
		Lua_PushNumber(L,Player[nPlayerIndex].m_sPortalPos.m_nMpsX);
		Lua_PushNumber(L,Player[nPlayerIndex].m_sPortalPos.m_nMpsY); 

		return 3
	 } 
*/
    Lua_PushNumber(L,0); 
	Lua_PushNumber(L,0);
	Lua_PushNumber(L,0); 
	 return 3;
}





//转职用 必须脱掉所有装备
int LuaCheckHaveEq(Lua_State * L)
{
	 int nPlayerIndex = GetPlayerIndex(L);
	  int nValue = 0;
	 if (nPlayerIndex > 0)
	 {
		 for (int i = 0; i < itempart_num ; ++i)     // 装备位置itempart_num
		 { 
		   if (Player[nPlayerIndex].m_ItemList.GetEquipment(i)>0)
		   {
			   nValue=1;
			   break;
		   }
		 }
	 } 
    Lua_PushNumber(L,nValue); 
	return 1;
}
//设置玩家的五行
int LuaSetPlayerSeries(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		int nValue = (int)Lua_ValueToNumber(L,1);
		Npc[Player[nPlayerIndex].m_nIndex].SetSeries(nValue);
	} 
	return 0; 
}


int LuaClearPlayerSkillState(Lua_State * L)
{

   int nPlayerIndex = GetPlayerIndex(L);

   if (nPlayerIndex > 0)
   {
	   for (int j = 0 ; j< MAX_NPCSKILL;++j)
	   {	
		  if (Npc[Player[nPlayerIndex].m_nIndex].m_SkillList.m_Skills[j].nSkillStyle ==SKILL_SS_PassivityNpcState)
		  {//被动状态
			Npc[Player[nPlayerIndex].m_nIndex].SetSkillLeftTime(Npc[Player[nPlayerIndex].m_nIndex].m_SkillList.m_Skills[j].SkillId,0);
		  }
	   }   
   } 
	   
    return 0;
}


int LuaPlayerSaveData(Lua_State * L)
{
    int count = Lua_GetTopIndex(L);

	if (count<1) return 0;

	int nPlayerIndex = GetPlayerIndex(L);
	if (count==1 && nPlayerIndex > 0)
	{
		int isRturn=(int)Lua_ValueToNumber(L,1);
		if (Player[nPlayerIndex].Save())	//退出游戏前最后一次生成数据包
		{  
	       if (g_pDataClient)//存档
		      Player[nPlayerIndex].SavePlayerData(isRturn);
		}   
	}
	else
	{
		nPlayerIndex=(int)Lua_ValueToNumber(L,1);
		int isRturn=(int)Lua_ValueToNumber(L,2);
		if  (nPlayerIndex<=0 || nPlayerIndex>=MAX_PLAYER) return 0;
		if (Player[nPlayerIndex].Save())	//退出游戏前最后一次生成数据包
		{  
			if (g_pDataClient)//存档
				Player[nPlayerIndex].SavePlayerData(false);
		}   
	}

	return 0;
}

//gm操作
int LuaGm_TickOutPlayerInGateWay(Lua_State * L)
{
	int count = Lua_GetTopIndex(L);
    if (count>=2)
	{
		char *accName = (char *)Lua_ValueToString(L,1);
		char *roleName = (char *)Lua_ValueToString(L,2);
		PlayerSet.gm_PrepareRemove(accName,roleName);
	}
	
	return 0;
}


int LuaTickOutPlayerInServer(Lua_State * L)
{
	int count = Lua_GetTopIndex(L);

	int isOnline = (int)Lua_ValueToNumber(L,1);

	if (count>1)
	{
      int nNpcIdx = (int)Lua_ValueToNumber(L,2);
      if (nNpcIdx>0  && nNpcIdx < MAX_NPC&& Npc[nNpcIdx].IsPlayer())
         PlayerSet.PrepareRemove(Npc[nNpcIdx].GetPlayerIdx(),isOnline);
	}
	else
	{
		int nPlayerIndex = GetPlayerIndex(L);
		if (nPlayerIndex > 0)
			PlayerSet.PrepareRemove(nPlayerIndex,isOnline);
	}

  return 0;
}

//更新同伴NPC 数据

int LuaClearNpcCurExp(Lua_State * L)
{
	int nTonNpcIdx = (int)Lua_ValueToNumber(L,1);
	
	if 	(nTonNpcIdx>0 && nTonNpcIdx < MAX_NPC)
	{
		Npc[nTonNpcIdx].m_cDeathCalcExp.Clear();	//更新同伴NPC 数据
		//Npc[Player[nPlayerIndex].m_nIndex].SynNpcInfoToClient(Npc[nTonNpcIdx].m_dwID); //同步防御
		}

	return 0;
}


int LuaUpdataNpcCurData(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		int nTonNpcIdx = (int)Lua_ValueToNumber(L,1);
	
		if 	(nTonNpcIdx>0 && nTonNpcIdx < MAX_NPC)
		{
			Npc[nTonNpcIdx].UpdataNpcCurData(nPlayerIndex);	//更新同伴NPC 数据
			//Npc[Player[nPlayerIndex].m_nIndex].SynNpcInfoToClient(Npc[nTonNpcIdx].m_dwID); //同步防御
		}
	}	
	return 0;
}

int LuaSetNpcTimeout(Lua_State * L)
{	
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2)
		return 0 ;

	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);

	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC)
		return 0;

    Npc[nNpcIndex].m_GameliveTime = (int)Lua_ValueToNumber(L, 2);
	Npc[nNpcIndex].m_liveType = 0;
	Npc[nNpcIndex]._delayReMove = 0;//延迟删除
	Npc[nNpcIndex].m_nNpcTimeout = 1;
	Npc[nNpcIndex].SetNpcTimeInfo();
	return 0;
}	

int LuaGetNpcTimeout(Lua_State * L)
{	
	int nNpcIdx = (int)Lua_ValueToNumber(L,1);
	if (nNpcIdx<=0 || nNpcIdx >= MAX_NPC) return 0;

	Lua_PushNumber(L,Npc[nNpcIdx].m_GameliveTime);
	return 1;
}	

int LuaSetNpcTimeLive(Lua_State * L)
{
   int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2)
		return 0 ;
   int nNpcIdx = (int)Lua_ValueToNumber(L,1);
   int nTime	= (int)Lua_ValueToNumber(L,2);

   int nKind	= 0;
   int nMapIdx	= 0;
          nMapIdx = 0;
   if (nNpcIdx<=0 || nNpcIdx >= MAX_NPC) 
	   return 0;

   int _delayDeath =0;

	if (nParamNum >= 5)
	    _delayDeath = (int)Lua_ValueToNumber(L,5);

       Npc[nNpcIdx].m_GameliveTime = nTime;
	   Npc[nNpcIdx].m_liveType = nKind;
	   Npc[nNpcIdx]._delayReMove = _delayDeath;//延迟删除

	   if (Npc[nNpcIdx].IsPlayer())
	   {//人物
		   if  (nMapIdx<=0)
			   nMapIdx = SubWorld[Npc[nNpcIdx].m_SubWorldIndex].m_SubWorldID;

		   Player[Npc[nNpcIdx].GetPlayerIdx()].m_cTask.SetSaveVal(25,nMapIdx);
	   }
	   else
	   {//怪物
		  Npc[nNpcIdx].SetNpcTimeInfo();
	   }
   return 0;
}

int LuaGetNpcTimeLive(Lua_State * L)
{
	int nNpcIdx = (int)Lua_ValueToNumber(L,1);
	if (nNpcIdx<=0 || nNpcIdx >= MAX_NPC) return 0;

	if (Npc[nNpcIdx].IsPlayer())
	{
		Lua_PushNumber(L,Npc[nNpcIdx].m_GameliveTime);
		Lua_PushNumber(L,Player[Npc[nNpcIdx].GetPlayerIdx()].m_cTask.GetSaveVal(25));
	}
	else
	{
		Lua_PushNumber(L,Npc[nNpcIdx].m_GameliveTime);
		Lua_PushNumber(L,Npc[nNpcIdx].m_liveType);
	}
	return 2;
}

//dang code
int LuaFindNpc(Lua_State * L)
{
	int nNpcIdx=0;
    if (Lua_IsNumber(L,1)) 
	{	
		DWORD nDwID=(int)Lua_ValueToNumber(L, 1);		 
		nNpcIdx = NpcSet.SearchID(nDwID);
	}
    Lua_PushNumber(L,nNpcIdx);
	return 1;
}


int LuaSetNpcSpeed(Lua_State * L)
{	
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2)
		return 0 ;

	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	
	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC)
		return 0;

	Npc[nNpcIndex].m_CurrentWalkSpeed = (int)Lua_ValueToNumber(L, 2);
	if (nParamNum>2 && (int)Lua_ValueToNumber(L, 3)>0)
		Npc[nNpcIndex].m_WalkSpeed = (int)Lua_ValueToNumber(L, 2);
	return 0;
}
int LuaSetNpcSync(Lua_State * L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 1)
		return 0 ;

	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	
	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC)
		return 0;

	Npc[nNpcIndex].SynNpcInfoToClient(Npc[nNpcIndex].m_dwID); //同步防御

	return 0;
}
//ham van tieu
int LuaSetNpcOwner(Lua_State * L)
{	
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 3)
		return 0 ;

	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC)
		return 0;
	g_StrCpyLen(Npc[nNpcIndex].Owner, (char*)Lua_ValueToString(L,2), sizeof(Npc[nNpcIndex].Owner));
//	g_DebugLog("%s %d",Npc[nNpcIndex].Owner,nNpcIndex);
	if (nParamNum >= 3)
		Npc[nNpcIndex].m_bNpcFollowFindPath = (BOOL)Lua_ValueToNumber(L,3);

	Npc[nNpcIndex].m_uFindPathTime = g_SubWorldSet.GetGameTime();
	return 0;
}

int LuaGetNpcOwner(Lua_State * L)
{	
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		return 0;
		
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 1)
		return 0 ;

	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC)
		return 0;
		
	lua_pushstring(L, Npc[nNpcIndex].Owner);
	
	return 1;
}

int LuaSetNpcParam(Lua_State * L)
{
/*	int nNpcIdx = (int)Lua_ValueToNumber(L, 1);
	int nValType = (int)Lua_ValueToNumber(L, 2);
	int nVal = (int)Lua_ValueToNumber(L, 3);

	if  (nNpcIdx<=0) return 0;
	if  (nValType<0 || nValType>=5)	  return 0;
	Npc[nNpcIdx].m_NpcParam[nValType]=nVal;
	return 0;
*/

	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2)
		return 0 ;

	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);

	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC)
		return 0;

	if (nParamNum > 2)
		{
			int nParamIdx = (int)Lua_ValueToNumber(L,2);
			if (nParamIdx < 0 || nParamIdx >= 5)
				return 0;
		Npc[nNpcIndex].m_NpcParam[nParamIdx] = (int)Lua_ValueToNumber(L,3);
		}
	else
		Npc[nNpcIndex].m_NpcParam[0] = (int)Lua_ValueToNumber(L,2);
	return 0;
}

int LuaSetNpcValue(Lua_State * L)
{	
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2)
		return 0 ;

	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);

	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC)
		return 0;

	if (nParamNum > 2)
		{
			int nParamIdx = (int)Lua_ValueToNumber(L,2);
			if (nParamIdx < 0 || nParamIdx >= 5)
				return 0;
		Npc[nNpcIndex].m_NpcParam[nParamIdx] = (int)Lua_ValueToNumber(L,3);
		}
	else
		Npc[nNpcIndex].m_NpcParam[0] = (int)Lua_ValueToNumber(L,2);
	return 0;
}	


int LuaSetNpcDoNone(Lua_State * L)
{
	int nNpcIdx  = (int)Lua_ValueToNumber(L, 1);
	int nValType = (int)Lua_ValueToNumber(L, 2);
	if  (nNpcIdx<=0 && nNpcIdx>=MAX_NPC) return 0;
	if (nNpcIdx < 0 || nNpcIdx >= MAX_NPC) return 0;
	Npc[nNpcIdx].__isDoNone=nValType;
	return 0;
}

//设置无敌
int LuaSetNotDeath(Lua_State * L)
{
	int nNpcIdx  = (int)Lua_ValueToNumber(L, 1);
	int nValType = (int)Lua_ValueToNumber(L, 2);
	if  (nNpcIdx<=0 && nNpcIdx>=MAX_NPC) return 0;
	if (nNpcIdx < 0 || nNpcIdx >= MAX_NPC) return 0;
	Npc[nNpcIdx].m_IsNotDeath=nValType;
	return 0;
}

int LuaSetActiveForever(Lua_State * L)
{
	int nNpcIdx = (int)Lua_ValueToNumber(L, 1);
	int nValType = (int)Lua_ValueToNumber(L, 2);
	if  (nNpcIdx<=0 && nNpcIdx>=MAX_NPC) return 0;

	if (nNpcIdx < 0 || nNpcIdx >= MAX_NPC) return 0;
	Npc[nNpcIdx].SetActiveForever(nValType);

	return 0;
}

int LuaGetNpcParam(Lua_State * L)
{
/*	int nNpcIdx = (int)Lua_ValueToNumber(L, 1);
	int nValType = (int)Lua_ValueToNumber(L, 2);	
	if  (nNpcIdx<=0) return 0;	
	if  (nValType<0 || nValType>=5)	  return 0;
	
	Lua_PushNumber(L,Npc[nNpcIdx].m_NpcParam[nValType]);
*/

	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 1)
		return 0 ;

	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);

	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC)
		return 0;

	if (nParamNum > 1)
		{
			int nParamIdx = (int)Lua_ValueToNumber(L,2);
			if (nParamIdx < 0 || nParamIdx >= 5)
				return 0;
		Lua_PushNumber(L, Npc[nNpcIndex].m_NpcParam[nParamIdx]);
		}
	else
		Lua_PushNumber(L, Npc[nNpcIndex].m_NpcParam[0]);

	return 1;
}	

int LuaGetNpcValue(Lua_State * L)
{	
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 1)
		return 0 ;

	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);

	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC)
		return 0;

	if (nParamNum > 1)
		{
			int nParamIdx = (int)Lua_ValueToNumber(L,2);
			if (nParamIdx < 0 || nParamIdx >= 5)
				return 0;
		Lua_PushNumber(L, Npc[nNpcIndex].m_NpcParam[nParamIdx]);
		}
	else
		Lua_PushNumber(L, Npc[nNpcIndex].m_NpcParam[0]);
	return 1;
}	

int LuaGetNpcSeries(Lua_State * L)
{

	int nNpcIdx = (int)Lua_ValueToNumber(L, 1);

	if  (nNpcIdx<=0 || nNpcIdx >= MAX_NPC) return 0;

	Lua_PushNumber(L,Npc[nNpcIdx].m_Series);

	return 1;
}

int LuaGetPlayerindexid(Lua_State * L)
{
	if (Lua_GetTopIndex(L) >0)
	{
		int nNpcIdx = (int)Lua_ValueToNumber(L, 1);
		if (nNpcIdx < 0 || nNpcIdx >= MAX_NPC) { Lua_PushNumber(L,0); Lua_PushNumber(L,0); return 2; }
		Lua_PushNumber(L,nNpcIdx);
		Lua_PushNumber(L,Npc[nNpcIdx].GetnPlayerIdx());
	}
    else
	{ 
	    int nPlayerIndex = GetPlayerIndex(L);
	    if (nPlayerIndex > 0)
		{ 
	       Lua_PushNumber(L,Player[nPlayerIndex].m_nIndex);
	       Lua_PushNumber(L,nPlayerIndex);
		}  
     	else
		{ 
	       Lua_PushNumber(L,0);
	       Lua_PushNumber(L,0);
		} 
	} 
    return 2;
}

int LuaGetPlayerCount(Lua_State * L)
{
	Lua_PushNumber(L, PlayerSet.GetPlayerNumber());
	return 1;
}
//获取当前时间帧数
int LuaGetServerTimes(Lua_State * L)
{
    Lua_PushNumber(L, g_SubWorldSet.GetGameTime()/18);
	return 1;
}

//获取当前时间帧数
int LuaGetGameTimes(Lua_State * L)
{
	Lua_PushNumber(L, g_SubWorldSet.GetGameTime());
	return 1;
}

int LuaGetPlayerLoginTimer(Lua_State * L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex > 0)
		Lua_PushNumber(L, Player[nPlayerIndex].m_dwLoginTime);
    else
		Lua_PushNumber(L,0);
	
	return 1;
}

int LuaGetPlayerLoginTime(Lua_State * L)
{
	int nResult = 0;
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		goto lab_getplayerlogintime;
	
	if (Player[nPlayerIndex].m_nIndex <= 0) 
		goto lab_getplayerlogintime;
	nResult = Player[nPlayerIndex].m_dwLoginTime;
	
lab_getplayerlogintime:
	Lua_PushNumber(L, nResult);
	return 1;
}

int LuaGetPlayerOnlineTime(Lua_State * L)
{
	int nResult = 0;
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		goto lab_getplayeronlinetime;
	
	if (Player[nPlayerIndex].m_nIndex <= 0) 
		goto lab_getplayeronlinetime;
	nResult = g_SubWorldSet.GetGameTime() - Player[nPlayerIndex].m_dwLoginTime;
	
lab_getplayeronlinetime:
	Lua_PushNumber(L, nResult);
	return 1;
}

int LuaGetPlayerAccoutName(Lua_State * L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex > 0)
    Lua_PushString(L, Player[nPlayerIndex].m_AccoutName);
    else
    Lua_PushNil(L);

	return 1;
}
//设置保护时间
int LuaSetProtectTime(Lua_State * L)
{
   	int nPlayerIndex = GetPlayerIndex(L);
	int nTime = (int)Lua_ValueToNumber(L, 1);

	if (nPlayerIndex > 0)
	{
		Npc[Player[nPlayerIndex].m_nIndex].m_ProtectTime=nTime;
	}
	
	return 0;
}


int LuaSetPlayerName(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	char *nName = (char *)Lua_ValueToString(L, 1);
	if (nPlayerIndex > 0 && nName!=NULL)
	{
		strcpy(Player[nPlayerIndex].m_PlayerName,nName);
	}

	return 0;
}


int LuaGetPlayerName(Lua_State * L)
{
    int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
        Lua_PushString(L, Npc[Player[nPlayerIndex].m_nIndex].Name);
	}
	else
		Lua_PushNil(L);
	
	return 1;
}

int LuaGetPlayerNameA(Lua_State * L)
{
    int nPlayerIndex = (int)Lua_ValueToNumber(L, 1);

	if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER)
	{
        Lua_PushString(L, Player[nPlayerIndex].m_PlayerName);
	}
	else
		Lua_PushNil(L);
	
	return 1;
}


int LuaGetNameByIndex(Lua_State * L)
{
    int nIndex = (int)Lua_ValueToNumber(L, 1);
	if (nIndex > 0 && nIndex < MAX_NPC)
	{
        Lua_PushString(L, Npc[nIndex].Name);
	}
	else
		Lua_PushNil(L);
	
	return 1;
}

//对比概率 ，结合幸运值 ，取出合适的行数
int LuaTabArrayRate(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex<0)
		return 0;
	int  nLuky = Player[nPlayerIndex].m_nCurLucky;  //当前幸运值
	//KIniFile	IniFile;   //读取INI文件
	KTabFile    TableFile; //读取TXT文件
	char *FiledName,*nKeyName;
	int nRows=0,nReg=0,mLie,mRow,nizRnak=0,nStarPos=2;

	FiledName = (char *)Lua_ValueToString(L, 1);
	nKeyName  = (char *)Lua_ValueToString(L, 2);

	if (Lua_GetTopIndex(L)>2)
        nStarPos  = (int )Lua_ValueToNumber(L, 3);

	if (!TableFile.Load(FiledName))
		return 0;

    mRow= TableFile.GetHeight()+1;     //总行数 加回最后一行
    int i;

    if	(nStarPos>mRow)
        nStarPos = 2;

    for (i=nStarPos;i<mRow;++i)
	{
		int nRnak; 
        TableFile.GetInteger(i,nKeyName,0,&nRnak);
		nizRnak +=nRnak;
	}

	int nRandom;

        nRandom = GetRandomNumber(1,nizRnak);
    
	    nizRnak =0;

	for (i=nStarPos;i<mRow;++i)
	{
			int nRnak; 
			TableFile.GetInteger(i,nKeyName,0,&nRnak);
			nizRnak +=nRnak;
			if  (nizRnak > nRandom)
			{
                  nRows = i;
				  break;
			}
	}

	TableFile.Clear();

	Lua_PushNumber(L,nRows);
    return 1;
}



int LuaSetTaskNo(Lua_State * L)
{
	char *strName;
          strName = (char *)Lua_ValueToString(L, 1);

    Lua_PushNumber(L,g_FileName2Id(strName));
    return 1;
}

//写ini内容
int LuaWriteIniFileInfo(Lua_State * L)
{
    KIniFile  IniFile;
    char *FiledName=NULL,* Section=NULL,*KeyName=NULL;
	FiledName =(char *)Lua_ValueToString(L, 1);
    Section=(char *)Lua_ValueToString(L, 2);
	KeyName=(char *)Lua_ValueToString(L, 3);
	int Model=(int )Lua_ValueToNumber(L, 4);
    if (!IniFile.Load(FiledName))
		return 0;
	
		if (Model==1)
		{
	      int  nVal=(int)Lua_ValueToNumber(L, 5);
          IniFile.WriteInteger(Section,KeyName,nVal);
		}
		else
		{
            char *sVal=NULL;
            sVal=(char *)Lua_ValueToString(L, 5);
		    IniFile.WriteString(Section,KeyName,sVal);

		}
	IniFile.Save(FiledName);
    IniFile.Clear();
     return 0;
}
//取随机坐标一个
int LuaGetMapPos_STR(Lua_State * L)
{
	KTabFile  TableFile;
	char *FiledName=NULL,*nKeya=NULL,*nKeyb=NULL;
	char nVal1[16],nVal2[16];
	int mLiem,nLie,mRow,nMoDel,nIsRow;
	ZeroMemory(nVal1,sizeof(nVal1));
	ZeroMemory(nVal2,sizeof(nVal2));
    FiledName = (char *)Lua_ValueToString(L,1);
    nKeya     = (char *)Lua_ValueToString(L,2);
	nKeyb     = (char *)Lua_ValueToString(L,3);
    nMoDel    = (int)Lua_ValueToNumber(L,4);
   if (TableFile.Load(FiledName))
   {  
	  mRow =TableFile.GetHeight();

	  if (nMoDel==1)
        nIsRow=GetRandomNumber(2,mRow);
	  else
	    nIsRow=GetRandomNumber(1,mRow);

	  TableFile.GetString(nIsRow,nKeya,"",nVal1,sizeof(nVal1));
	  TableFile.GetString(nIsRow,nKeyb,"",nVal2,sizeof(nVal2));
   }  
  	TableFile.Clear();

	Lua_PushString(L,nVal1); 
	Lua_PushString(L,nVal2); 

  return 2;

}

//取随机坐标一个
int LuaGetMapPos_INT(Lua_State * L)
{
	KTabFile  TableFile;
	char *FiledName=NULL;
	char nVal1[16],nVal2[16];
	     ZeroMemory(nVal1,sizeof(nVal1));
	     ZeroMemory(nVal2,sizeof(nVal2));
	int mLiem,nLie, mRow,nMoDel,nIsRow;
        FiledName =(char *)Lua_ValueToString(L, 1);
	int nKey1,nKey2;
        nKey1 = (int)Lua_ValueToNumber(L, 2);
	    nKey2 = (int)Lua_ValueToNumber(L, 3);
        nMoDel= (int)Lua_ValueToNumber(L, 4);
    if (TableFile.Load(FiledName))
	{ 
	  mRow=TableFile.GetHeight();
	  if (nMoDel==1)
	  { 
        nIsRow=GetRandomNumber(2,mRow);
	  } 
	  else
	  { 
        nIsRow=GetRandomNumber(1,mRow);
	  } 
       
	  TableFile.GetString(nIsRow,nKey1,"",nVal1,sizeof(nVal1));
	  TableFile.GetString(nIsRow,nKey2,"",nVal2,sizeof(nVal2));
	} 
	TableFile.Clear();
	Lua_PushString(L,nVal1); 
	Lua_PushString(L,nVal2); 
  return 2;
}


//删除某行
int LuaRemoveTabRow(Lua_State * L) 
{	
	KTabFileCtrl  TableFileA;
	char *FiledName=NULL;
	int mRow=0,nReg=0;
	mRow = (int)Lua_ValueToNumber(L, 1);
	FiledName = (char *)Lua_ValueToString(L, 2);
	nReg=TableFileA.openfile(FiledName);
    TableFileA.Remove(mRow);    // 删除某行
	TableFileA.Save(FiledName); // 保存文件
	TableFileA.Clear();         // 释放节点
	return 0;

}

//在某行 某列写入文件
int LuaWriteTabString(Lua_State * L) 
{
	KTabFileCtrl  TableFileA;
	char *FiledName=NULL,*LieName=NULL,*nStr;
	int nReg=0, mLie=0,mRow=0;
	mRow = (int)Lua_ValueToNumber(L, 1);
	mLie = (int)Lua_ValueToNumber(L, 2);
	nStr = (char *)Lua_ValueToString(L, 3);
	FiledName = (char *)Lua_ValueToString(L, 4);
	nReg=TableFileA.openfile(FiledName);
    TableFileA.WriteString(mRow,mLie,nStr);
    TableFileA.Save(FiledName);
    TableFileA.Clear();
	return 0;
}
//获取某行 某列字符串
int LuaGetTabString(Lua_State * L) 
{
	KTabFileCtrl  TableFileA;
	char *FiledName=NULL,*LieName=NULL,nStr[32];
	int nReg=0, mLie=0,mRow=0;
	mRow = (int)Lua_ValueToNumber(L, 1);
	mLie = (int)Lua_ValueToNumber(L, 2);
	FiledName = (char *)Lua_ValueToString(L, 3);
	nReg=TableFileA.openfile(FiledName);
    TableFileA.GetString(mRow,mLie,"无",nStr,strlen(nStr));
	TableFileA.Clear();
	Lua_PushString(L,nStr);
	return 1;
}


//设定个人的城主和帮会
int LuaSetWarMasterAndTong(Lua_State * L)  // 城主
{
	int nPlayerIndex = GetPlayerIndex(L);
	char *nWarMName=NULL;
	char *nWarTName=NULL;
	//else if (Lua_IsNumber(L, 1))Lua_IsString
	if (Lua_IsString(L,1) && Lua_IsString(L,2))
	{
		nWarMName = (char *)Lua_ValueToString(L, 1);
		nWarTName = (char *)Lua_ValueToString(L, 2);
		//		char nPname[32];
		//		if (strcmpi(nNpcName, Npc[nNpcID].Name) == 0)   //这个对比不分字母的大小写
		//sprintf(Npc[Player[nPlayerIndex].m_nIndex].m_WarMaster,"%s",nWarMName);
        //sprintf(Npc[Player[nPlayerIndex].m_nIndex].m_WarTongNamea,"%s",nWarTName);
		Npc[Player[nPlayerIndex].m_nIndex].SetstrInfo(STR_WARTONG_NAME,nWarTName);
		Npc[Player[nPlayerIndex].m_nIndex].SetstrInfo(STR_WARMASTER_NAME,nWarMName);
		//        nReg=NpcSet.SearchName(nNpcName);
	}
	
	return 0;
}

int LuaGetMapName(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	int mMapIdx=(int)Lua_ValueToNumber(L, 1);
	char nMapName[32],mMapName[32];
	sprintf(nMapName,"%d_name",mMapIdx);
    g_NpcMapDropRate.GetString("List",nMapName,"暂无数据",mMapName,sizeof(mMapName));
	 
    Lua_PushString(L,mMapName);
    return 1;

}


int LuaSetWarCity(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex<0)
		return 0;
	int mIsWarCity=(int)Lua_ValueToNumber(L, 1);
		Npc[Player[nPlayerIndex].m_nIndex].m_IsWarCity=mIsWarCity;
	return 0;
}


//设定个人的税收和指数
int LuaSetWarShuiAndZhi(Lua_State * L) // 税收
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (Lua_IsNumber(L,1))
	{
		int nShuiID=(int)Lua_ValueToNumber(L, 1);
        int nZhiID=(int)Lua_ValueToNumber(L, 2);
//		Npc[Player[nPlayerIndex].m_nIndex].m_WarShuishou=nShuiID;
//		Npc[Player[nPlayerIndex].m_nIndex].m_Warzhi=nZhiID;
	}
	return 0;
}

//玩家的城主 和 帮会
int LuaGetWarMasterandTong(Lua_State * L)
{
    int nPlayerIndex = GetPlayerIndex(L);
	char nName[64]={0};
	if (nPlayerIndex > 0)
	{
        Lua_PushString(L, Npc[Player[nPlayerIndex].m_nIndex].GetstrInfo(STR_WARMASTER_NAME,nName,sizeof(nName)));
		Lua_PushString(L, Npc[Player[nPlayerIndex].m_nIndex].GetstrInfo(STR_WARTONG_NAME,nName,sizeof(nName)));
	}
	else
	{
		Lua_PushNil(L);
	    Lua_PushNil(L);
	}
	return 2;
	
}
//得到个人的税 和物指
int LuaGetWarShuiAndZhi(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
     // Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_WarShuishou);
      //Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_Warzhi);
		Lua_PushNumber(L,0);
		Lua_PushNumber(L,0);

	}
	else
	{
     Lua_PushNumber(L,0);
	 Lua_PushNumber(L,0);
	}
	return 2;
}
//设定地图的 税和 物指
int LuaSetSubWarShuiAndZhi(Lua_State *L)
{
 // int nPlayerIndex = GetPlayerIndex(L);
 // if (nPlayerIndex > 0)
 // { 
/*   int nSubID =(int)Lua_ValueToNumber(L,1);
   int nSubShui =(int)Lua_ValueToNumber(L,2);
   int nSubZhi =(int)Lua_ValueToNumber(L,3); */
//   SubWorld[nSubID].m_WarShuishou=nSubShui;
//   SubWorld[nSubID].m_WarZhishu=nSubZhi;
//  } 
   return 0;
}

//设定城市官员m_nSex
int LuaSetSubWarCityGuanYuan(Lua_State *L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	char * nSubGuanName=NULL;
	int  nReg=0;
	int  nSubGuanID;
	int  n_nSex;
	int nSubWorldIndex =(int)Lua_ValueToNumber(L,1);   //地图编号
		nSubGuanName= (char *)Lua_ValueToString(L, 2);   //官员名字
		nSubGuanID= (int)Lua_ValueToNumber(L,3);     //官员ID
		n_nSex= (int)Lua_ValueToNumber(L,4); 
/*
		if (nSubWorldIndex >= 0 && nSubWorldIndex < MAX_SUBWORLD)
		{ 

			//	Lua_PushString(L, SubWorld[nSubWorldIndex].m_WarSubMaster);
			//	nSubWorldID = SubWorld[nSubWorldIndex].m_SubWorldID;
			char        m_WarSubFuCheng[32]; //副城主
			char        m_WarSubZou[32]; //左护法
			char        m_WarSubYou[32]; //右护法
			char        m_WarSubTaishi[32]; //太史
			char        m_WarSubZhongshu[32]; //中书
			char        m_WarSubShangshu[32]; //尚书
			char        m_WarSubMishu[32]; //秘书
	        char        m_WarSubTaile[32]; //太乐

  int     m_CZIsNanOrNv;//是男或女
  int     m_FZIsNanOrNv;
  int     m_ZHIsNanOrNv;
  int     m_YHIsNanOrNv;
  int     m_TSIsNanOrNv;
  int     m_ZSIsNanOrNv;
  int     m_SSIsNanOrNv;
  int     m_MSIsNanOrNv;
	int     m_TLIsNanOrNv;

			if (nSubGuanID==0)    //清空城市官员
			{
            sprintf(SubWorld[nSubWorldIndex].m_WarSubFuCheng,"%s",nSubGuanName);  //副城
            SubWorld[nSubWorldIndex].m_FZIsNanOrNv=n_nSex;
			sprintf(SubWorld[nSubWorldIndex].m_WarSubZou,"%s",nSubGuanName);
			SubWorld[nSubWorldIndex].m_ZHIsNanOrNv=n_nSex;
			sprintf(SubWorld[nSubWorldIndex].m_WarSubZou,"%s",nSubGuanName);
			SubWorld[nSubWorldIndex].m_ZHIsNanOrNv=n_nSex;
			sprintf(SubWorld[nSubWorldIndex].m_WarSubYou,"%s",nSubGuanName);
			SubWorld[nSubWorldIndex].m_YHIsNanOrNv=n_nSex;
			sprintf(SubWorld[nSubWorldIndex].m_WarSubTaishi,"%s",nSubGuanName);
			SubWorld[nSubWorldIndex].m_TSIsNanOrNv=n_nSex;
			sprintf(SubWorld[nSubWorldIndex].m_WarSubZhongshu,"%s",nSubGuanName);
			SubWorld[nSubWorldIndex].m_ZSIsNanOrNv=n_nSex;
			sprintf(SubWorld[nSubWorldIndex].m_WarSubShangshu,"%s",nSubGuanName);
			SubWorld[nSubWorldIndex].m_SSIsNanOrNv=n_nSex;
			sprintf(SubWorld[nSubWorldIndex].m_WarSubMishu,"%s",nSubGuanName);
			SubWorld[nSubWorldIndex].m_MSIsNanOrNv=n_nSex;
			sprintf(SubWorld[nSubWorldIndex].m_WarSubTaile,"%s",nSubGuanName);    //太乐
			SubWorld[nSubWorldIndex].m_TLIsNanOrNv=n_nSex;
			}
		    else if (nSubGuanID==1)
			{
			sprintf(SubWorld[nSubWorldIndex].m_WarSubFuCheng,"%s",nSubGuanName);  //副城
            SubWorld[nSubWorldIndex].m_FZIsNanOrNv=n_nSex;			
			}
			else if (nSubGuanID==2)
			{
            sprintf(SubWorld[nSubWorldIndex].m_WarSubZou,"%s",nSubGuanName);
			SubWorld[nSubWorldIndex].m_ZHIsNanOrNv=n_nSex;
			}		
			else if (nSubGuanID==3)
			{
			sprintf(SubWorld[nSubWorldIndex].m_WarSubYou,"%s",nSubGuanName);
			SubWorld[nSubWorldIndex].m_YHIsNanOrNv=n_nSex;
			}
			else if (nSubGuanID==4)
			{
			sprintf(SubWorld[nSubWorldIndex].m_WarSubTaishi,"%s",nSubGuanName);
			SubWorld[nSubWorldIndex].m_TSIsNanOrNv=n_nSex;	
			}
			else if (nSubGuanID==5)
			{
			sprintf(SubWorld[nSubWorldIndex].m_WarSubZhongshu,"%s",nSubGuanName);
			SubWorld[nSubWorldIndex].m_ZSIsNanOrNv=n_nSex;		
			}
			else if (nSubGuanID==6)
			{
			sprintf(SubWorld[nSubWorldIndex].m_WarSubShangshu,"%s",nSubGuanName);
			SubWorld[nSubWorldIndex].m_SSIsNanOrNv=n_nSex;
			}
			else if (nSubGuanID==7)
			{
			sprintf(SubWorld[nSubWorldIndex].m_WarSubMishu,"%s",nSubGuanName);
			SubWorld[nSubWorldIndex].m_MSIsNanOrNv=n_nSex;
			}
			else if (nSubGuanID==8)
			{
			sprintf(SubWorld[nSubWorldIndex].m_WarSubTaile,"%s",nSubGuanName);    //太乐
			SubWorld[nSubWorldIndex].m_TLIsNanOrNv=n_nSex;
			}
			nReg=1;
			Lua_PushNumber(L, nReg);
		} 
		else

			*/

	  Lua_PushNumber(L, nReg);
	  return 1;	  
 }
//设定攻方
int LuaSetSubWarGong(Lua_State *L)
{
	char * nSubGTong=NULL;
	int   nRegm=0;
    int	nPlayerIndex=GetPlayerIndex(L);
	int nSubWorldIndex =(int)Lua_ValueToNumber(L,1);
	    nSubGTong= (char *)Lua_ValueToString(L, 2);  
	int nIsWhoID =(int)Lua_ValueToNumber(L,3);
		   if (nSubWorldIndex>0  && nSubWorldIndex < MAX_SUBWORLD&& nIsWhoID!=0)
		   {
               //sprintf(SubWorld[nSubWorldIndex].m_WarGongTong,"%s",nSubGTong);
			   SubWorld[nSubWorldIndex].nWorldMapInfo[STR_MAP_TGONGNAME] = nSubGTong;
			   SubWorld[nSubWorldIndex].m_WarIsWho=nIsWhoID;
			   nRegm=1;
		   }
		   else
		   {
			   //sprintf(SubWorld[nSubWorldIndex].m_WarGongTong,"%s",nSubGTong);
			   if (nSubWorldIndex < 0 || nSubWorldIndex >= MAX_SUBWORLD) { Lua_PushNumber(L,0); return 1; }
			   SubWorld[nSubWorldIndex].nWorldMapInfo[STR_MAP_TGONGNAME] = nSubGTong;
			   SubWorld[nSubWorldIndex].m_WarIsWho=nIsWhoID;  //公用模式
			   nRegm=2;
		   }
 //   SubWorld[nSubWorldIndex].SendSyncData(Player[nPlayerIndex].m_nIndex, Player[nPlayerIndex].m_nNetConnectIdx);  //同步世界地图参数 
	Lua_PushNumber(L, nRegm);
     return 1;
}

int LuaGetSubWhoId(Lua_State *L)
{
	int nRegm=0;
	int nPlayerIndex=GetPlayerIndex(L);
	int nSubWorldIndex =(int)Lua_ValueToNumber(L,1);
//	int nIsWhoID =(int)Lua_ValueToNumber(L,3);
	if (nSubWorldIndex>0 && nSubWorldIndex < MAX_SUBWORLD)
		nRegm=SubWorld[nSubWorldIndex].m_WarIsWho;
//    SubWorld[nSubWorldIndex].SendSyncData(Player[nPlayerIndex].m_nIndex, Player[nPlayerIndex].m_nNetConnectIdx);  //同步世界地图参数 
	Lua_PushNumber(L, nRegm);
     return 1;
}

int LuaSetSubWhoId(Lua_State *L)
{
	int nRegm=0;
	int nPlayerIndex=GetPlayerIndex(L);
	int nSubWorldIndex =(int)Lua_ValueToNumber(L,1);
	int nIsWhoID =(int)Lua_ValueToNumber(L,3);
	if (nSubWorldIndex>0  && nSubWorldIndex < MAX_SUBWORLD&& nIsWhoID!=0)
	{
		SubWorld[nSubWorldIndex].m_WarIsWho=nIsWhoID;
		nRegm=nIsWhoID;
	}
//    SubWorld[nSubWorldIndex].SendSyncData(Player[nPlayerIndex].m_nIndex, Player[nPlayerIndex].m_nNetConnectIdx);  //同步世界地图参数 
	Lua_PushNumber(L, nRegm);
     return 1;
}

//设定守方
int LuaSetSubWarShou(Lua_State *L)

{
	char * nSubSTong=NULL;
	int   nRegm=0;
	int   nPlayerIndex=GetPlayerIndex(L);
	int nSubWorldIndex =(int)Lua_ValueToNumber(L,1);
	nSubSTong= (char *)Lua_ValueToString(L, 2);  
	int nIsWhoID =(int)Lua_ValueToNumber(L,3);
	if (nSubWorldIndex>0 && nIsWhoID!=0)
	{
		//sprintf(SubWorld[nSubWorldIndex].m_WarShouTong,"%s",nSubSTong);
		SubWorld[nSubWorldIndex].nWorldMapInfo[STR_MAP_TSHOUNAME] = nSubSTong;
		SubWorld[nSubWorldIndex].m_WarIsWho=nIsWhoID;
		nRegm=1;
	}
	else
	{
		//sprintf(SubWorld[nSubWorldIndex].m_WarShouTong,"%s",nSubSTong);
		SubWorld[nSubWorldIndex].nWorldMapInfo[STR_MAP_TSHOUNAME] = nSubSTong;
		SubWorld[nSubWorldIndex].m_WarIsWho=nIsWhoID;  //公用模式
		nRegm=2;
	}
//    SubWorld[nSubWorldIndex].SendSyncData(Player[nPlayerIndex].m_nIndex, Player[nPlayerIndex].m_nNetConnectIdx);  //同步世界地图参数 
	Lua_PushNumber(L, nRegm);
     return 1;
}

//设置该地图的PK模式
int LuaSetSubMapPKmodle(Lua_State *L)
{
   int nSubWorldIndex =(int)Lua_ValueToNumber(L,1);
   int nPkID =(int)Lua_ValueToNumber(L,2);
       if (nSubWorldIndex < 0 || nSubWorldIndex >= MAX_SUBWORLD) return 0;
       SubWorld[nSubWorldIndex].m_nWPKFlag=nPkID;  //0,1,2, -1
   return 0;
}

//设置该地图的战斗模式
int LuaSetSubMapFightmodle(Lua_State *L)
{
	int nSubWorldIndex =(int)Lua_ValueToNumber(L,1);
	int nPkID =(int)Lua_ValueToNumber(L,2);
	    if (nSubWorldIndex < 0 || nSubWorldIndex >= MAX_SUBWORLD) return 0;
	    SubWorld[nSubWorldIndex].m_nWFight=nPkID;
	return 0;
}


//设置该地图的显示基本信息
int LuaSetSubMapShowInfo(Lua_State *L)
{
	int nSubWorldIndex =(int)Lua_ValueToNumber(L,1);
	int nPkID =(int)Lua_ValueToNumber(L,2);
	    if (nSubWorldIndex < 0 || nSubWorldIndex >= MAX_SUBWORLD) return 0;
	    SubWorld[nSubWorldIndex].m_IsShowLoop=nPkID;
	return 0;
}

int LuaReTurnMoneyInMap(Lua_State *L)
{
	int nSubWorldIdx =(int)Lua_ValueToNumber(L,1);
    int nMoney=	(int)Lua_ValueToNumber(L,2);
	int nType=	(int)Lua_ValueToNumber(L,3);


	int nCount=0;
	int nIndex = PlayerSet.GetFirstPlayer(); //查找玩家姓名
	
	while(nIndex > 0)
	{
		if (SubWorld[Npc[Player[nIndex].m_nIndex].m_SubWorldIndex].m_SubWorldID==nSubWorldIdx)
		{
			if (nType==0)	//返回金币
				Player[nIndex].EarnXu(nMoney);
			else
			    Player[nIndex].Earn(nMoney); //返回银两
			
			nCount++;
		}
		nIndex = PlayerSet.GetNextPlayer();
	}
	
	Lua_PushNumber(L,nCount);
	return 1;

}



int LuaGetPlayerCountInMap(Lua_State *L)
{
	int nSubWorldIdx =(int)Lua_ValueToNumber(L,1);
	int nCount=0;
	int nIndex = PlayerSet.GetFirstPlayer(); //查找玩家姓名

	while(nIndex > 0)
	{
		if (SubWorld[Npc[Player[nIndex].m_nIndex].m_SubWorldIndex].m_SubWorldID==nSubWorldIdx)
			nCount++;
		nIndex = PlayerSet.GetNextPlayer();
	}

   Lua_PushNumber(L,nCount);
   return 1;
}

int LuaGetPlayerCountOffLive(Lua_State *L)
{
	int nCount=0;
	int nTotal=0;
	int nIndex = PlayerSet.GetFirstPlayer(); //查找玩家姓名

	while(nIndex > 0)
	{
		if (Player[nIndex].m_nLiXian == 0)
			nCount++;

		nIndex = PlayerSet.GetNextPlayer();
	}

	nTotal = PlayerSet.GetPlayerNumber() - nCount;

   Lua_PushNumber(L,nTotal);
   Lua_PushNumber(L,nCount);
   return 2;
}

int LuaSetSubMapYinLiang(Lua_State *L)
{
	int nSubWorldIndex =(int)Lua_ValueToNumber(L,1);
	int nPkID =(int)Lua_ValueToNumber(L,2);
	if (nSubWorldIndex<0 || nSubWorldIndex>=MAX_SUBWORLD) return 0;
	SubWorld[nSubWorldIndex].m_IsJinQian=nPkID;
	return 0;
}


int LuaSetMapNoDropItemModel(Lua_State *L)
{
	int nSubWorldIndex =(int)Lua_ValueToNumber(L,1);
	int nPkID =(int)Lua_ValueToNumber(L,2);
	if (nSubWorldIndex<0 || nSubWorldIndex>=MAX_SUBWORLD) return 0;
	SubWorld[nSubWorldIndex].m_nNoDropItemKind=nPkID;
	return 0;
}

int LuaGetMapNoDropItemModel(Lua_State *L)
{
	int nSubWorldIndex =(int)Lua_ValueToNumber(L,1);
	if (nSubWorldIndex<0 || nSubWorldIndex>=MAX_SUBWORLD) return 0;
	Lua_PushNumber(L,SubWorld[nSubWorldIndex].m_nNoDropItemKind);
	return 1;
}

int LuaSetMapPunishState(Lua_State *L)
{
	int nSubWorldIndex =(int)Lua_ValueToNumber(L,1);
	int nPkID =(int)Lua_ValueToNumber(L,2);
	if (nSubWorldIndex<0 || nSubWorldIndex>=MAX_SUBWORLD) return 0;
	SubWorld[nSubWorldIndex].m_nCurPKPunishState=nPkID;
	return 0;
}

int LuaGetMapPunishState(Lua_State *L)
{
	int nSubWorldIndex =(int)Lua_ValueToNumber(L,1);
	if (nSubWorldIndex<0 || nSubWorldIndex>=MAX_SUBWORLD) return 0;
	Lua_PushNumber(L,SubWorld[nSubWorldIndex].m_nCurPKPunishState);
	return 1;
}

int LuaSetSubMapLucky(Lua_State *L)
{
	int nSubWorldIndex =(int)Lua_ValueToNumber(L,1);
	int nPkID =(int)Lua_ValueToNumber(L,2);

	if (nSubWorldIndex<0 || nSubWorldIndex>=MAX_SUBWORLD) return 0;

	if (nPkID<=0)
		nPkID = 1;

	SubWorld[nSubWorldIndex].m_IsLuKey=nPkID;

	return 0;
}

int LuaSetSubMapJinYan(Lua_State *L)
{
	int nSubWorldIndex =(int)Lua_ValueToNumber(L,1);
	int nPkID =(int)Lua_ValueToNumber(L,2);
	if (nSubWorldIndex<0 || nSubWorldIndex>=MAX_SUBWORLD) return 0;
	SubWorld[nSubWorldIndex].m_IsJinYan=nPkID;
	return 0;
}

//设置该地图的战斗阵营
int LuaSetSubMapFightComp(Lua_State *L)
{
	int nSubWorldIndex =(int)Lua_ValueToNumber(L,1);
	int nPkID =(int)Lua_ValueToNumber(L,2);
	if (nSubWorldIndex<0 || nSubWorldIndex>=MAX_SUBWORLD) return 0;
	SubWorld[nSubWorldIndex].m_nWcomp=nPkID;
	return 0;
}

 
//设置战斗队伍
int LuaSetSubTeam(Lua_State *L)
 {
	 int nSubWorldIndex =(int)Lua_ValueToNumber(L,1);
	 int nPkID =(int)Lua_ValueToNumber(L,2);
	 if (nSubWorldIndex<0 || nSubWorldIndex>=MAX_SUBWORLD) return 0;
	 SubWorld[nSubWorldIndex].m_SubWhereTeam=nPkID;
	 return 0;
 }

//获取战斗队伍编号
int LuaGetSubTeam(Lua_State *L)
{
	int nSubWorldIndex =(int)Lua_ValueToNumber(L,1);
	if (nSubWorldIndex<0 || nSubWorldIndex>=MAX_SUBWORLD) return 0;
    Lua_PushNumber(L,SubWorld[nSubWorldIndex].m_SubWhereTeam);
	return 1;
	
} 

//设置战斗时间
int LuaSetSubMapFightTime(Lua_State *L)
{
	int nSubWorldIndex =(int)Lua_ValueToNumber(L,1);
	int nPkID =(int)Lua_ValueToNumber(L,2);
	if (nSubWorldIndex<0 || nSubWorldIndex>=MAX_SUBWORLD) return 0;
	SubWorld[nSubWorldIndex].m_nMapTime=nPkID;
	return 0;
}

int LuaGetSubMapMagicCount(Lua_State *L)
{
	int nSubWorldIndex =(int)Lua_ValueToNumber(L,1);
	if (nSubWorldIndex<0 || nSubWorldIndex>=MAX_SUBWORLD) return 0;
	Lua_PushNumber(L,SubWorld[nSubWorldIndex].m_nMacigCount);
	return 1;
}

int LuaSetSubMapMagicCount(Lua_State *L)
{
	int nSubWorldIndex =(int)Lua_ValueToNumber(L,1);
	int nPkID =(int)Lua_ValueToNumber(L,2);
	if (nSubWorldIndex<0 || nSubWorldIndex>=MAX_SUBWORLD) return 0;
	SubWorld[nSubWorldIndex].m_nMacigCount=nPkID;
	return 0;
}

//设置延时时间
int LuaSetSubMapSleepTime(Lua_State *L)
{
	int nSubWorldIndex =(int)Lua_ValueToNumber(L,1);
	int nPkID =(int)Lua_ValueToNumber(L,2);
	if (nSubWorldIndex<0 || nSubWorldIndex>=MAX_SUBWORLD) return 0;
	SubWorld[nSubWorldIndex].m_nSleepTime=nPkID;
	return 0;
} 

//获取延时时间
int LuaGetSubMapSleepTime(Lua_State *L)
{
	int nSubWorldIndex =(int)Lua_ValueToNumber(L,1);
	if (nSubWorldIndex<0 || nSubWorldIndex>=MAX_SUBWORLD) return 0;
    Lua_PushNumber(L,SubWorld[nSubWorldIndex].m_nSleepTime);
	return 1;

} 


int LuaGetSubMapFightTime(Lua_State *L)
{
	int nSubWorldIndex =(int)Lua_ValueToNumber(L,1);
	if (nSubWorldIndex<0 || nSubWorldIndex>=MAX_SUBWORLD) return 0;
    Lua_PushNumber(L,SubWorld[nSubWorldIndex].m_nMapTime);
	return 1;
}



//获取是那个城市的居民
int LuaGetIsWhereCity(Lua_State *L)
{

	int nPlayerIndex;
	int nTopPamr=Lua_GetTopIndex(L);
	if (nTopPamr<=0)	
	   nPlayerIndex = GetPlayerIndex(L);
	else
	   nPlayerIndex =(int)Lua_ValueToNumber(L,1);

	if (nPlayerIndex<=0 || nPlayerIndex >= MAX_PLAYER)
		return 0;

	Lua_PushNumber(L,Npc[Player[nPlayerIndex].m_nIndex].m_IsInCity);
	return 1;

}


//获取是那个城市的居民
int LuaSetIsWhereCity(Lua_State *L)
{

	int nPlayerIndex;

		nPlayerIndex =(int)Lua_ValueToNumber(L,1);

	if (nPlayerIndex<=0 || nPlayerIndex >= MAX_PLAYER)
		return 0;

	Npc[Player[nPlayerIndex].m_nIndex].m_IsInCity=(int)Lua_ValueToNumber(L,2);

	return 0;
	
}

int LuaSetMapEnterCount_Z(Lua_State *L)
{
	int nSubWorldIndex =(int)Lua_ValueToNumber(L,1);
	int nPkID =(int)Lua_ValueToNumber(L,2);
	if (nSubWorldIndex < 0 || nSubWorldIndex >= MAX_SUBWORLD) return 0;
	SubWorld[nSubWorldIndex].m_playerEnterCount_z=nPkID;
	return 0;
}

int LuaSetMapEnterCount_F(Lua_State *L)
{
	int nSubWorldIndex =(int)Lua_ValueToNumber(L,1);
	int nPkID =(int)Lua_ValueToNumber(L,2);
	if (nSubWorldIndex < 0 || nSubWorldIndex >= MAX_SUBWORLD) return 0;
	SubWorld[nSubWorldIndex].m_playerEnterCount_f=nPkID;
	return 0;
}

int LuaGetMapEnterCount_Z(Lua_State *L)
{
	int nSubWorldIndex =(int)Lua_ValueToNumber(L,1);
	if (nSubWorldIndex < 0 || nSubWorldIndex >= MAX_SUBWORLD) { Lua_PushNumber(L,0); return 1; }
	Lua_PushNumber(L, SubWorld[nSubWorldIndex].m_playerEnterCount_z);
	return 1;
}

int LuaGetMapEnterCount_F(Lua_State *L)
{
	int nSubWorldIndex =(int)Lua_ValueToNumber(L,1);
	if (nSubWorldIndex < 0 || nSubWorldIndex >= MAX_SUBWORLD) { Lua_PushNumber(L,0); return 1; }
	Lua_PushNumber(L, SubWorld[nSubWorldIndex].m_playerEnterCount_f);
	return 1;
}

//死亡人数
int LuaSetMapDeathCount_Z(Lua_State *L)
{
	int nSubWorldIndex =(int)Lua_ValueToNumber(L,1);
	int nPkID =(int)Lua_ValueToNumber(L,2);
	if (nSubWorldIndex < 0 || nSubWorldIndex >= MAX_SUBWORLD) return 0;
	SubWorld[nSubWorldIndex].m_WargKillNum=nPkID;
	return 0;
}
int LuaSetMapDeathCount_F(Lua_State *L)
{
	int nSubWorldIndex =(int)Lua_ValueToNumber(L,1);
	int nPkID =(int)Lua_ValueToNumber(L,2);
	if (nSubWorldIndex < 0 || nSubWorldIndex >= MAX_SUBWORLD) return 0;
	SubWorld[nSubWorldIndex].m_WarsKillNum=nPkID;
	return 0;
}

int LuaGetMapDeathCount_Z(Lua_State *L)
{
	int nSubWorldIndex =(int)Lua_ValueToNumber(L,1);
	if (nSubWorldIndex < 0 || nSubWorldIndex >= MAX_SUBWORLD) { Lua_PushNumber(L,0); return 1; }
	Lua_PushNumber(L, SubWorld[nSubWorldIndex].m_WargKillNum);
	return 1;
}

int LuaGetMapDeathCount_F(Lua_State *L)
{
	int nSubWorldIndex =(int)Lua_ValueToNumber(L,1);
	if (nSubWorldIndex < 0 || nSubWorldIndex >= MAX_SUBWORLD) { Lua_PushNumber(L,0); return 1; }
	Lua_PushNumber(L, SubWorld[nSubWorldIndex].m_WarsKillNum);
	return 1;
}

//设置该地图的人数
int LuaSetMapPlayerAttack(Lua_State *L)
{
	int nSubWorldIndex =(int)Lua_ValueToNumber(L,1);
	int nPkID =(int)Lua_ValueToNumber(L,2);
	int nPkIDA =(int)Lua_ValueToNumber(L,3);
	if (nSubWorldIndex < 0 || nSubWorldIndex >= MAX_SUBWORLD) return 0;
	SubWorld[nSubWorldIndex].m_nPleyerZongNum=nPkID;  //0,1,2, -1
	SubWorld[nSubWorldIndex].m_nCurPleyerNum=nPkIDA;
	return 0;
}

//设置该地图的当前人数
int LuaSetCurMapPlayerAttack(Lua_State *L)
{
	int nSubWorldIndex =(int)Lua_ValueToNumber(L,1);
	int nPkID =(int)Lua_ValueToNumber(L,2);
	if (nSubWorldIndex < 0 || nSubWorldIndex >= MAX_SUBWORLD) return 0;
	SubWorld[nSubWorldIndex].m_nCurPleyerNum=nPkID;
	return 0;
}

//获取该地图的当前的人数 和总人数
int LuaGetCurMapPlayerAttack(Lua_State *L)
{
	int nSubWorldIndex =(int)Lua_ValueToNumber(L,1);
	if (nSubWorldIndex < 0 || nSubWorldIndex >= MAX_SUBWORLD) { Lua_PushNumber(L ,0); Lua_PushNumber(L, 0); return 2; }
	Lua_PushNumber(L, SubWorld[nSubWorldIndex].m_nCurPleyerNum);
	Lua_PushNumber(L, SubWorld[nSubWorldIndex].m_nPleyerZongNum);
	return 2;
}


//设置该地图 活动上次开放的时间
int LuaSetSubCurTime(Lua_State *L)
{
	int nSubWorldIndex =(int)Lua_ValueToNumber(L,1);
	char * nPkID =(char *)Lua_ValueToString(L,2);

	if  (nSubWorldIndex<0 || nSubWorldIndex>=MAX_SUBWORLD) return 0;

	if (nPkID)
		SubWorld[nSubWorldIndex].nWorldMapInfo[STR_MAP_CURTIME]= nPkID;
	else
		SubWorld[nSubWorldIndex].nWorldMapInfo.erase(STR_MAP_CURTIME);

	//SubWorld[nSubWorldIndex].nWorldMapInfo[STR_MAP_CURTIME] = nPkID;
	return 0;
}

//设置该地图 活动下次开放的时间
int LuaSetSubNextTime(Lua_State *L)
{
	int nSubWorldIndex =(int)Lua_ValueToNumber(L,1);
	char * nPkID =(char *)Lua_ValueToString(L,2);
	
	//sprintf(SubWorld[nSubWorldIndex].m_SubNextTime,nPkID);
	if  (nSubWorldIndex<0 || nSubWorldIndex>=MAX_SUBWORLD) return 0;

	if (nPkID)
	   SubWorld[nSubWorldIndex].nWorldMapInfo[STR_MAP_NEXTTIME]= nPkID;
	else
	   SubWorld[nSubWorldIndex].nWorldMapInfo.erase(STR_MAP_NEXTTIME);

	return 0;
}


int LuaGetSubOpenTime(Lua_State *L)
{
	int nSubWorldIndex =(int)Lua_ValueToNumber(L,1);

	if (nSubWorldIndex<=-1 || nSubWorldIndex >= MAX_SUBWORLD)
	{
		return 0;
	}
	//SubWorld[nSubWorldIndex].nWorldMapInfo[STR_MAP_NEXTTIME]= nPkID;
	Lua_PushString(L, SubWorld[nSubWorldIndex].nWorldMapInfo[STR_MAP_CURTIME].c_str());
	Lua_PushString(L, SubWorld[nSubWorldIndex].nWorldMapInfo[STR_MAP_NEXTTIME].c_str());
	return 2;
}


//设置该地图的怪物攻击波数
int LuaSetSubMapNpcAttack(Lua_State *L)
{
	int nSubWorldIndex =(int)Lua_ValueToNumber(L,1);
	int nPkID =(int)Lua_ValueToNumber(L,2);
	int nPkIDA =(int)Lua_ValueToNumber(L,3);
	int nNpcNum =(int)Lua_ValueToNumber(L,4);

	if (nSubWorldIndex < 0 || nSubWorldIndex >= MAX_SUBWORLD) return 0;
	SubWorld[nSubWorldIndex].m_nNpcZongNum=nPkID;  //0,1,2, -1
	SubWorld[nSubWorldIndex].m_nCurNpcNum=nPkIDA;
	SubWorld[nSubWorldIndex].m_nCurNum=nNpcNum;
	return 0;
}


//设置该地图的当前的怪物攻击波数
int LuaGetCurMapNpcAttack(Lua_State *L)
{
	int nSubWorldIndex =(int)Lua_ValueToNumber(L,1);
	if (nSubWorldIndex < 0 || nSubWorldIndex >= MAX_SUBWORLD) { Lua_PushNumber(L,0); Lua_PushNumber(L,0); Lua_PushNumber(L,0); return 3; }
	Lua_PushNumber(L, SubWorld[nSubWorldIndex].m_nCurNum);
	Lua_PushNumber(L, SubWorld[nSubWorldIndex].m_nCurNpcNum);
	Lua_PushNumber(L, SubWorld[nSubWorldIndex].m_nNpcZongNum);
	return 3;
}

//设置该地图的当前的怪物攻击波数
int LuaSetCurMapNpcAttack(Lua_State *L)
{
	int nSubWorldIndex =(int)Lua_ValueToNumber(L,1);
	int nPkID =(int)Lua_ValueToNumber(L,2);
	if (nSubWorldIndex < 0 || nSubWorldIndex >= MAX_SUBWORLD) return 0;
	SubWorld[nSubWorldIndex].m_nCurNpcNum=nPkID;
	return 0;
}

//设置该地图的当前的怪物数量
int LuaSetCurMapNpcNum(Lua_State *L)
{
	int nSubWorldIndex =(int)Lua_ValueToNumber(L,1);
	int nPkID =(int)Lua_ValueToNumber(L,2);
	if (nSubWorldIndex < 0 || nSubWorldIndex >= MAX_SUBWORLD) return 0;
	SubWorld[nSubWorldIndex].m_nCurNum=nPkID;
	return 0;
}

//设定地图城主和帮会
int LuaSetSubWarMasterAndTong(Lua_State *L)
{ 
	int nPlayerIndex = GetPlayerIndex(L);
	char * nSubMaster=NULL;
	char * nSubTong=NULL;
	int  nReg=0;
//	if (nPlayerIndex > 0)
//	{
		int nSubWorldIndex =(int)Lua_ValueToNumber(L,1);
		nSubMaster= (char *)Lua_ValueToString(L, 2);
		nSubTong=(char *)Lua_ValueToString(L, 3);
		int nSubWorldID = 0;
		if (nSubWorldIndex >= 0 && nSubWorldIndex < MAX_SUBWORLD)
		{ 
			//	Lua_PushString(L, SubWorld[nSubWorldIndex].m_WarSubMaster);
			//	nSubWorldID = SubWorld[nSubWorldIndex].m_SubWorldID;
			    //sprintf(SubWorld[nSubWorldIndex].m_WarSubMaster,"%s",nSubMaster);
				//sprintf(SubWorld[nSubWorldIndex].m_WarSubTongname,"%s",nSubTong);
			    SubWorld[nSubWorldIndex].nWorldMapInfo[STR_MAP_MASTER] = nSubMaster;
				SubWorld[nSubWorldIndex].nWorldMapInfo[STR_MAP_ZTONGNAME] = nSubTong;
				nReg=1;
			Lua_PushNumber(L, nReg);
		} 
		else
		    Lua_PushNumber(L, nReg);
	return 1;
}
//设置股子资料
int LuaSetSubGuZuoWei(Lua_State *L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	char * nSubGUMaster=NULL;
	int nZuoWei,nReg=0,nModel=0;
	//	if (nPlayerIndex > 0)
	//	{
	int nSubWorldIndex =(int)Lua_ValueToNumber(L,1);  //地图号

	if (nSubWorldIndex==0)
		nSubWorldIndex = Npc[Player[nPlayerIndex].m_nIndex].m_SubWorldIndex;

	    nSubGUMaster= (char *)Lua_ValueToString(L, 2); //名字
		nZuoWei     = (int)Lua_ValueToNumber(L, 3);	       //座位号
		nModel      = (int)Lua_ValueToNumber(L, 4);	       //设置模式  0 为清空 1 为设置
 /*
	if 	(nModel>0)
	{
		if (nZuoWei==1)
		{
		sprintf(SubWorld[nSubWorldIndex].m_GuzinameA,"%s",nSubGUMaster);
        Npc[Player[nPlayerIndex].m_nIndex].m_GuziMenber = 1;
		Npc[Player[nPlayerIndex].m_nIndex].m_ZuoWeihao  = 1;
		}
		else if  (nZuoWei==2)
		{
			sprintf(SubWorld[nSubWorldIndex].m_GuzinameB,"%s",nSubGUMaster);
			Npc[Player[nPlayerIndex].m_nIndex].m_GuziMenber = 1;
			Npc[Player[nPlayerIndex].m_nIndex].m_ZuoWeihao  = 2;
		}
		else if  (nZuoWei==3)
		{
			sprintf(SubWorld[nSubWorldIndex].m_GuzinameC,"%s",nSubGUMaster);
			Npc[Player[nPlayerIndex].m_nIndex].m_GuziMenber = 1;
			Npc[Player[nPlayerIndex].m_nIndex].m_ZuoWeihao  = 3;
		}
		else if  (nZuoWei==4)
		{
			sprintf(SubWorld[nSubWorldIndex].m_GuzinameD,"%s",nSubGUMaster);
			Npc[Player[nPlayerIndex].m_nIndex].m_GuziMenber = 1;
			Npc[Player[nPlayerIndex].m_nIndex].m_ZuoWeihao  = 4;
		}
		else if  (nZuoWei==5)
		{
			sprintf(SubWorld[nSubWorldIndex].m_GuzinameE,"%s",nSubGUMaster);
			Npc[Player[nPlayerIndex].m_nIndex].m_GuziMenber = 1;
			Npc[Player[nPlayerIndex].m_nIndex].m_ZuoWeihao  = 5;
		}
	}
	else
	{
		if (nZuoWei==1)
		{
			sprintf(SubWorld[nSubWorldIndex].m_GuzinameA,"%s",nSubGUMaster);
		}
		else if  (nZuoWei==2)
		{
			sprintf(SubWorld[nSubWorldIndex].m_GuzinameB,"%s",nSubGUMaster);
		}
		else if  (nZuoWei==3)
		{
			sprintf(SubWorld[nSubWorldIndex].m_GuzinameC,"%s",nSubGUMaster);
		}
		else if  (nZuoWei==4)
		{
			sprintf(SubWorld[nSubWorldIndex].m_GuzinameD,"%s",nSubGUMaster);
		}
		else if  (nZuoWei==5)
		{
			sprintf(SubWorld[nSubWorldIndex].m_GuzinameE,"%s",nSubGUMaster);
		}

		Npc[Player[nPlayerIndex].m_nIndex].m_GuziMenber=0;
		Npc[Player[nPlayerIndex].m_nIndex].m_ZuoWeihao=0;
	}
        SubWorld[nSubWorldIndex].SendSyncData(Player[nPlayerIndex].m_nIndex, Player[nPlayerIndex].m_nNetConnectIdx);  //同步世界地图参数 
 */       
		return 0;
}
//设置经脉类型 是否已经冲穴
int LuaSetMaiTypeAndLevel(Lua_State *L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	int nType =(int)Lua_ValueToNumber(L,1);    // 经脉类型
	int nLevel =(int)Lua_ValueToNumber(L,2);   // 经脉16个穴道 等级
	int nValue =(int)Lua_ValueToNumber(L,3);   // 值
	int nModel =(int)Lua_ValueToNumber(L,4);     //模式 1 叠加 0 不叠加
	if  (nModel>=1)
	{//nType==0为丹田
	     
       if  (nType==1)
	   {//督脉
           Player[nPlayerIndex].m_cTask.SetSaveVal(3501,ESetByte(Player[nPlayerIndex].m_cTask.GetSaveVal(3501),1,nValue)); 
		   Player[nPlayerIndex].m_cTask.SetSaveVal(3501,ESetByte(Player[nPlayerIndex].m_cTask.GetSaveVal(3501),2,nLevel)); 
	   }
	   else if  (nType==2)
	   {//督脉
           Player[nPlayerIndex].m_cTask.SetSaveVal(3502,ESetByte(Player[nPlayerIndex].m_cTask.GetSaveVal(3502),1,nValue)); 
		   Player[nPlayerIndex].m_cTask.SetSaveVal(3502,ESetByte(Player[nPlayerIndex].m_cTask.GetSaveVal(3502),2,nLevel)); 
	   }
	   else if  (nType==3)
	   {//督脉
           Player[nPlayerIndex].m_cTask.SetSaveVal(3503,ESetByte(Player[nPlayerIndex].m_cTask.GetSaveVal(3503),1,nValue)); 
		   Player[nPlayerIndex].m_cTask.SetSaveVal(3503,ESetByte(Player[nPlayerIndex].m_cTask.GetSaveVal(3503),2,nLevel)); 
	   }
	   else if  (nType==4)
	   {//督脉
           Player[nPlayerIndex].m_cTask.SetSaveVal(3504,ESetByte(Player[nPlayerIndex].m_cTask.GetSaveVal(3504),1,nValue)); 
		   Player[nPlayerIndex].m_cTask.SetSaveVal(3504,ESetByte(Player[nPlayerIndex].m_cTask.GetSaveVal(3504),2,nLevel)); 
	   }
	   else if  (nType==5)
	   {//督脉
           Player[nPlayerIndex].m_cTask.SetSaveVal(3505,ESetByte(Player[nPlayerIndex].m_cTask.GetSaveVal(3505),1,nValue)); 
		   Player[nPlayerIndex].m_cTask.SetSaveVal(3505,ESetByte(Player[nPlayerIndex].m_cTask.GetSaveVal(3505),2,nLevel)); 
	   }
	   else if  (nType==6)
	   {//督脉
           Player[nPlayerIndex].m_cTask.SetSaveVal(3506,ESetByte(Player[nPlayerIndex].m_cTask.GetSaveVal(3506),1,nValue)); 
		   Player[nPlayerIndex].m_cTask.SetSaveVal(3506,ESetByte(Player[nPlayerIndex].m_cTask.GetSaveVal(3506),2,nLevel)); 
	   }
	   else if  (nType==7)
	   {//督脉
           Player[nPlayerIndex].m_cTask.SetSaveVal(3507,ESetByte(Player[nPlayerIndex].m_cTask.GetSaveVal(3507),1,nValue)); 
		   Player[nPlayerIndex].m_cTask.SetSaveVal(3507,ESetByte(Player[nPlayerIndex].m_cTask.GetSaveVal(3507),2,nLevel)); 
	   }
	   else if  (nType==8)
	   {//督脉
           Player[nPlayerIndex].m_cTask.SetSaveVal(3508,ESetByte(Player[nPlayerIndex].m_cTask.GetSaveVal(3508),1,nValue)); 
		   Player[nPlayerIndex].m_cTask.SetSaveVal(3508,ESetByte(Player[nPlayerIndex].m_cTask.GetSaveVal(3508),2,nLevel)); 
	   }


	    Player[nPlayerIndex].UpdataCurData();

		BYTE	NetCommand = (BYTE)s2c_syncurupdata;
	    g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, &NetCommand, sizeof(BYTE));

	} 
    return 0;
}

int LuaSynUpdataCurData(Lua_State *L)
{    
	int nPlayerIndex = GetPlayerIndex(L);

	if (nPlayerIndex>0)
	{
	PLAYER_JINMAI_GETBACK	sTaskBack;
	sTaskBack.ProtocolType = s2c_JinMaicallback;  
	//sTaskBack.m_TaskId  = pTask->m_TaskId;
	sTaskBack.m_TaskVala = Player[nPlayerIndex].m_cTask.GetSaveVal(3501);
	sTaskBack.m_TaskValb = Player[nPlayerIndex].m_cTask.GetSaveVal(3502);
	sTaskBack.m_TaskValc = Player[nPlayerIndex].m_cTask.GetSaveVal(3503);
	sTaskBack.m_TaskVald = Player[nPlayerIndex].m_cTask.GetSaveVal(3504);
	sTaskBack.m_TaskVale = Player[nPlayerIndex].m_cTask.GetSaveVal(3505);
	sTaskBack.m_TaskValf = Player[nPlayerIndex].m_cTask.GetSaveVal(3506);
	sTaskBack.m_TaskValg = Player[nPlayerIndex].m_cTask.GetSaveVal(3507);
	sTaskBack.m_TaskValh = Player[nPlayerIndex].m_cTask.GetSaveVal(3508);

	if (g_pServer)
		g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, (BYTE*)&sTaskBack, sizeof(PLAYER_JINMAI_GETBACK));
      
	  Player[nPlayerIndex].UpdataCurData();
	  BYTE	NetCommand = (BYTE)s2c_syncurupdata;
	  g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, &NetCommand,sizeof(BYTE));
	}
	return 0;

}
//上线初始化经脉
int LuaInitJinMaiVal(Lua_State *L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex>0)
	{
       int nNpcIdx=Player[nPlayerIndex].m_nIndex;
          Npc[nNpcIdx].NpcNewInitJinMaiVal();
	} 

    return 0;
}

void UseSkills(int sKillID,int sLevel,int nNpcIdx,KMagicAttrib *pData)
{
	      int nTime=-1,nIfMagic=1;
	      KSkill *pSkill = (KSkill*)g_SkillManager.GetSkill(sKillID,sLevel);
		  if (nIfMagic)
			 pSkill->CastStateSkill(nNpcIdx,0,0,nTime);  //是否
		  else
		     Npc[nNpcIdx].SetStateSkillEffect(nNpcIdx,sKillID,sLevel,pData,0,nTime);  //设置技能光环效果	
}


int LuaGetJinMaiInfo(Lua_State *L)
{
	int nPlayerIndex = GetPlayerIndex(L);
     
	if  (nPlayerIndex<=0)
		return 0;

	 Lua_PushNumber(L,Npc[Player[nPlayerIndex].m_nIndex].m_ZhenYuan);
	 Lua_PushNumber(L,Npc[Player[nPlayerIndex].m_nIndex].m_JinMaiBingJia);

	return 2;
}


//设置真元 兵甲值（nModel ：1 为叠加 0 为不叠加）
int LuaSetJinMaiVal(Lua_State *L)
{
 int nPlayerIndex = GetPlayerIndex(L);
 if  (nPlayerIndex<=0)
		return 0;
 int nZhenYuan =(int)Lua_ValueToNumber(L,1);   //真元
 int nBingJia  =(int)Lua_ValueToNumber(L,2);   //兵甲值
 int nModel    =(int)Lua_ValueToNumber(L,3);   //模式 1 叠加 0 不叠加
 if  (nModel==1)
 {
     Npc[Player[nPlayerIndex].m_nIndex].m_ZhenYuan      += nZhenYuan;
     Npc[Player[nPlayerIndex].m_nIndex].m_JinMaiBingJia += nBingJia;
 } 
 else if  (nModel==2)
 {
     Npc[Player[nPlayerIndex].m_nIndex].m_ZhenYuan      -= nZhenYuan;
     Npc[Player[nPlayerIndex].m_nIndex].m_JinMaiBingJia -= nBingJia;
 }
 else if  (nModel<=0)
 {
	 Npc[Player[nPlayerIndex].m_nIndex].m_ZhenYuan       = nZhenYuan;
     Npc[Player[nPlayerIndex].m_nIndex].m_JinMaiBingJia  = nBingJia;
 }
 return 0;
}

int LuaSetJinYan(Lua_State *L)
{
   int nPlayerIndex = GetPlayerIndex(L);
   if  (nPlayerIndex<=0)
		return 0;
   int nJinYan =(int)Lua_ValueToNumber(L,1);  //地图号
   Npc[Player[nPlayerIndex].m_nIndex].IsJinYan=nJinYan;
   return 0;
}
int LuaSetXinYun(Lua_State *L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if  (nPlayerIndex<=0)
		return 0;

	int nXinYun =(int)Lua_ValueToNumber(L,1);  //地图号
	Npc[Player[nPlayerIndex].m_nIndex].IsLuKey=nXinYun;
//	if (nXinYun>0)
//	Player[nPlayerIndex].m_nCurLucky=Player[nPlayerIndex].m_nCurLucky*nXinYun;
	return 0;
}


int LuaSetNpcLucky(Lua_State *L)
{	
	int nNpcIdx =(int)Lua_ValueToNumber(L,1);
	
	int nNpcLucky =(int)Lua_ValueToNumber(L,2);

	if  (nNpcIdx<=0 || nNpcIdx >= MAX_NPC) return 0;

	Npc[nNpcIdx].IsLuKey=nNpcLucky;
	return 0;
}

int LuaSetJinQian(Lua_State *L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	int nJinQian =(int)Lua_ValueToNumber(L,1);  //地图号
	Npc[Player[nPlayerIndex].m_nIndex].IsJinQian=nJinQian;
	return 0;
}

int LuaCheckNameIsOn(Lua_State *L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	//char * nSubGUMaster=NULL;
	int nReg=0;
	//int nReg=0;   int nSubWorldIndex = Npc[Player[nPlayerIndex].m_nIndex].m_SubWorldIndex;
	int nSubWorldIndex =(int)Lua_ValueToNumber(L,1);  //地图号
	char * nName=(char*)Lua_ValueToString(L,2);  //地图号
	/*if (nSubWorldIndex==0)
		nSubWorldIndex = Npc[Player[nPlayerIndex].m_nIndex].m_SubWorldIndex;
		
	    if (strcmpi(SubWorld[nSubWorldIndex].m_GuzinameA,nName) == 0) 
			nReg=1;

        else if (strcmpi(SubWorld[nSubWorldIndex].m_GuzinameB,nName) == 0)
			nReg=1;


        else if(strcmpi(SubWorld[nSubWorldIndex].m_GuzinameC,nName) == 0)
			nReg=1;
	

        else if(strcmpi(SubWorld[nSubWorldIndex].m_GuzinameD,nName) == 0)
			nReg=1;

         else if(strcmpi(SubWorld[nSubWorldIndex].m_GuzinameE,nName) == 0)
			nReg=1;

	SubWorld[nSubWorldIndex].SendSyncData(Player[nPlayerIndex].m_nIndex, Player[nPlayerIndex].m_nNetConnectIdx);  //同步世界地图参数 
   */ 
	Lua_PushNumber(L,nReg);
	return 1;
	
}

int LuaCheckZuoWei(Lua_State *L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	//char * nSubGUMaster=NULL;
	int nZuoWei,nReg=1;
	//int nReg=0;   int nSubWorldIndex = Npc[Player[nPlayerIndex].m_nIndex].m_SubWorldIndex;
	int nSubWorldIndex =(int)Lua_ValueToNumber(L,1);  //地图号
	if (nSubWorldIndex==0)
		nSubWorldIndex = Npc[Player[nPlayerIndex].m_nIndex].m_SubWorldIndex;
	//nSubGUMaster= (char *)Lua_ValueToString(L, 2); //名字
	nZuoWei=(int)Lua_ValueToNumber(L, 2);	//座位号
 /*
	if (nZuoWei==1)
	{
	    if (strcmpi(SubWorld[nSubWorldIndex].m_GuzinameA,"空缺") == 0) 
              nReg=0;
	}
	else if (nZuoWei==2)
	{
        if (strcmpi(SubWorld[nSubWorldIndex].m_GuzinameB,"空缺") == 0)
              nReg=0;
	}
	else if (nZuoWei==3)
	{
        if (strcmpi(SubWorld[nSubWorldIndex].m_GuzinameC,"空缺") == 0)
              nReg=0;
	}
	else if (nZuoWei==4)
	{
        if (strcmpi(SubWorld[nSubWorldIndex].m_GuzinameD,"空缺") == 0)
              nReg=0;
	}
	else if (nZuoWei==5)
	{
        if (strcmpi(SubWorld[nSubWorldIndex].m_GuzinameE,"空缺") == 0)
              nReg=0;
	}
	SubWorld[nSubWorldIndex].SendSyncData(Player[nPlayerIndex].m_nIndex, Player[nPlayerIndex].m_nNetConnectIdx);  //同步世界地图参数 
 */   
	Lua_PushNumber(L,nReg);
	return 1;

}

//设置押金
int LuaSetSubYaJin(Lua_State *L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	//char * nSubGUMaster=NULL;
	int nZuoWei,nYaJin,nModel=0;
	//int nReg=0;   int nSubWorldIndex = Npc[Player[nPlayerIndex].m_nIndex].m_SubWorldIndex;
	int nSubWorldIndex =(int)Lua_ValueToNumber(L,1);  //地图号
	if (nSubWorldIndex==0)
          nSubWorldIndex = Npc[Player[nPlayerIndex].m_nIndex].m_SubWorldIndex;
	//nSubGUMaster= (char *)Lua_ValueToString(L, 2); //名字
	nZuoWei=(int)Lua_ValueToNumber(L, 2);	//座位号
	nYaJin=(int)Lua_ValueToNumber(L, 3);	//押金
	nModel=(int)Lua_ValueToNumber(L, 4);	//模式
	/*
if (nModel>0)
{
    if (nZuoWei==1)
	{
       SubWorld[nSubWorldIndex].m_GyajingA +=nYaJin;  //1号座押金
	   SubWorld[nSubWorldIndex].m_IsXiaZhuA=1;      //设设置为已经下注
	}
	else if (nZuoWei==2)
	{
		SubWorld[nSubWorldIndex].m_GyajingB +=nYaJin;
		SubWorld[nSubWorldIndex].m_IsXiaZhuB=2;      //设设置为已经下注
	}
	else if (nZuoWei==3)
	{
		SubWorld[nSubWorldIndex].m_GyajingC +=nYaJin;
		SubWorld[nSubWorldIndex].m_IsXiaZhuC=3;      //设设置为已经下注
	}
	else if (nZuoWei==4)
	{
		SubWorld[nSubWorldIndex].m_GyajingD +=nYaJin;
		SubWorld[nSubWorldIndex].m_IsXiaZhuD=4;      //设设置为已经下注
	}
	else if (nZuoWei==5)
	{
		SubWorld[nSubWorldIndex].m_GyajingE +=nYaJin;
		SubWorld[nSubWorldIndex].m_IsXiaZhuE=5;      //设设置为已经下注
	}
}
else
{
	if (nZuoWei==1)
	{
		SubWorld[nSubWorldIndex].m_GyajingA=0;       //1号座押金
		SubWorld[nSubWorldIndex].m_IsXiaZhuA=0;      //设设置为已经下注
	}
	else if (nZuoWei==2)
	{
		SubWorld[nSubWorldIndex].m_GyajingB=0;
		SubWorld[nSubWorldIndex].m_IsXiaZhuB=0;      //设设置为已经下注
	}
	else if (nZuoWei==3)
	{
		SubWorld[nSubWorldIndex].m_GyajingC=0;
		SubWorld[nSubWorldIndex].m_IsXiaZhuC=0;      //设设置为已经下注
	}
	else if (nZuoWei==4)
	{
		SubWorld[nSubWorldIndex].m_GyajingD=0;
		SubWorld[nSubWorldIndex].m_IsXiaZhuD=0;      //设设置为已经下注
	}
	else if (nZuoWei==5)
	{
		SubWorld[nSubWorldIndex].m_GyajingE=0;
		SubWorld[nSubWorldIndex].m_IsXiaZhuE=0;      //设设置为已经下注
	}
}
	SubWorld[nSubWorldIndex].SendSyncData(Player[nPlayerIndex].m_nIndex, Player[nPlayerIndex].m_nNetConnectIdx);  //同步世界地图参数 
   */ 
	return 0;
}
//获取押金
int LuaGetSubYaJin(Lua_State *L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	//char * nSubGUMaster=NULL;
	int nZuoWei,nYaJin=0;
	//int nReg=0;   int nSubWorldIndex = Npc[Player[nPlayerIndex].m_nIndex].m_SubWorldIndex;
	int nSubWorldIndex =(int)Lua_ValueToNumber(L,1);  //地图号
	if (nSubWorldIndex==0)
		nSubWorldIndex = Npc[Player[nPlayerIndex].m_nIndex].m_SubWorldIndex;
	//nSubGUMaster= (char *)Lua_ValueToString(L, 2); //名字
	nZuoWei=(int)Lua_ValueToNumber(L, 2);	//座位号
	/*
    if (nZuoWei==1)
	{
		nYaJin=SubWorld[nSubWorldIndex].m_GyajingA;  //1号座押金
	}
	else if (nZuoWei==2)
	{
		nYaJin=SubWorld[nSubWorldIndex].m_GyajingB; 
	}
	else if (nZuoWei==3)
	{
	   nYaJin=SubWorld[nSubWorldIndex].m_GyajingC; 
	}
	else if (nZuoWei==4)
	{
       nYaJin=SubWorld[nSubWorldIndex].m_GyajingD; 
	}
	else if (nZuoWei==5)
	{
       nYaJin=SubWorld[nSubWorldIndex].m_GyajingE; 
	}
	SubWorld[nSubWorldIndex].SendSyncData(Player[nPlayerIndex].m_nIndex, Player[nPlayerIndex].m_nNetConnectIdx);  //同步世界地图参数 
    */
	Lua_PushNumber(L,nYaJin);
	return 1;
}
//设置当前甩股者
int LuaSetSubWhoIsStart(Lua_State *L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	//char * nSubGUMaster=NULL;
	int nIsStart;
	//int nReg=0;
	int nSubWorldIndex =(int)Lua_ValueToNumber(L,1);  //地图号
	//nSubGUMaster= (char *)Lua_ValueToString(L, 2); //名字
	nIsStart=(int)Lua_ValueToNumber(L, 2);	//座位号
    //SubWorld[nSubWorldIndex].m_ZuoWeiHao=nIsStart;  //当前甩股者
    //SubWorld[nSubWorldIndex].SendSyncData(Player[nPlayerIndex].m_nIndex, Player[nPlayerIndex].m_nNetConnectIdx);  //同步世界地图参数 
    
	return 0;
}
//清空股子资料
int LuaSetSubGuZiInit(Lua_State *L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	//char * nSubGUMaster=NULL;
	//int nZuoWei;
	//int nReg=0;
	int nSubWorldIndex =(int)Lua_ValueToNumber(L,1);  //地图号
	//nSubGUMaster= (char *)Lua_ValueToString(L, 2); //名字
	//nZuoWei=(int)Lua_ValueToNumber(L, 3);	//座位号
	/* SubWorld[nSubWorldIndex].m_SubGuModel=0; //模式
	 SubWorld[nSubWorldIndex].m_ZuoWeiHao=0;  //当前甩股者

	 SubWorld[nSubWorldIndex].m_DianshuA=0;        //点数
	 SubWorld[nSubWorldIndex].m_DianshuB=0;
	 SubWorld[nSubWorldIndex].m_DianshuC=0;
	 SubWorld[nSubWorldIndex].m_DianshuD=0;
	 SubWorld[nSubWorldIndex].m_DianshuE=0; 

	 SubWorld[nSubWorldIndex].m_GyajingA=0;        //下注金额
	 SubWorld[nSubWorldIndex].m_GyajingB=0;
	 SubWorld[nSubWorldIndex].m_GyajingC=0;
	 SubWorld[nSubWorldIndex].m_GyajingD=0;
	 SubWorld[nSubWorldIndex].m_GyajingE=0; 

	 SubWorld[nSubWorldIndex].m_IsStartA=0;  //是否开始了
	 SubWorld[nSubWorldIndex].m_IsStartB=0;  //是否开始了
	 SubWorld[nSubWorldIndex].m_IsStartC=0;  //是否开始了
	 SubWorld[nSubWorldIndex].m_IsStartD=0;  //是否开始了
     SubWorld[nSubWorldIndex].m_IsStartE=0;  //是否开始了

	 SubWorld[nSubWorldIndex].m_IsXiaZhuA=0;
	 SubWorld[nSubWorldIndex].m_IsXiaZhuB=0;
	 SubWorld[nSubWorldIndex].m_IsXiaZhuC=0;
	 SubWorld[nSubWorldIndex].m_IsXiaZhuD=0;
	 SubWorld[nSubWorldIndex].m_IsXiaZhuE=0;

	 sprintf(SubWorld[nSubWorldIndex].m_GuzinameA,"%s","空缺");
	 sprintf(SubWorld[nSubWorldIndex].m_GuzinameB,"%s","空缺");
	 sprintf(SubWorld[nSubWorldIndex].m_GuzinameC,"%s","空缺");
	 sprintf(SubWorld[nSubWorldIndex].m_GuzinameD,"%s","空缺");
	 sprintf(SubWorld[nSubWorldIndex].m_GuzinameE,"%s","空缺");

	 SubWorld[nSubWorldIndex].SendSyncData(Player[nPlayerIndex].m_nIndex, Player[nPlayerIndex].m_nNetConnectIdx);  //同步世界地图参数 
     */
	 return 0;
}



int LuaSetSubWarCityMoney(Lua_State *L)
{
	    int nMapID,nJB,nJXB,nGX; 
		nMapID = (int)Lua_ValueToNumber(L,1);
	    nJB = (int)Lua_ValueToNumber(L,2);
        nJXB = (int)Lua_ValueToNumber(L,3);
		nGX = (int)Lua_ValueToNumber(L,4);
//		nTodayMoney = (int)Lua_ValueToNumber(L,4);
       if (nMapID < 0 || nMapID >= MAX_SUBWORLD) return 0;
       SubWorld[nMapID].m_WarCityJB=nJB;
       SubWorld[nMapID].m_WarCityManey=nJXB;
       SubWorld[nMapID].m_WarCityGX=nGX;
	
	return 0;
}
int LuaGetSubWarCityMoney(Lua_State *L)
{
/*	int nPlayerIndex = GetPlayerIndex(L);
	int nMapID; 
	if (nPlayerIndex > 0)
     nMapID = (int)Lua_ValueToNumber(L,1);

//    Lua_PushNumber(L, SubWorld[nMapID].m_WarCityJB);
	Lua_PushNumber(L, SubWorld[nMapID].m_WarCityManey);
	Lua_PushNumber(L, SubWorld[nMapID].m_TodayMoney);
*/	 
	return 0;

}

int LuaSetSubGuanFUli(Lua_State *L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	int nMapID,nGuanID,nMoney,nReg=0;
/*	
	if (nPlayerIndex > 0)
	{
		nMapID = (int)Lua_ValueToNumber(L,1);
	    nGuanID = (int)Lua_ValueToNumber(L,2);
        nMoney = (int)Lua_ValueToNumber(L,3);
        if (nGuanID==1)
        SubWorld[nMapID].m_GuanFUliA=nMoney;
		else
        SubWorld[nMapID].m_GuanFUliB=nMoney;

        nReg=1;
	}
	*/
	return 0;
	
}

// 得到地图的  城主 和帮会
int LuaGetSubWarMasterAndTong(Lua_State *L)
{ 
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		int nPosX = 0;
		int nPosY = 0;
		int nPmapID=0;
		    Npc[Player[nPlayerIndex].m_nIndex].GetMpsPos(&nPosX, &nPosY,&nPmapID);
		int nSubWorldIndex = Npc[Player[nPlayerIndex].m_nIndex].m_SubWorldIndex;
		int nSubWorldID = 0;
		if (nSubWorldIndex >= 0 && nSubWorldIndex < MAX_SUBWORLD)
		{ 
			Lua_PushString(L, SubWorld[nSubWorldIndex].nWorldMapInfo[STR_MAP_MASTER].c_str());
			Lua_PushString(L, SubWorld[nSubWorldIndex].nWorldMapInfo[STR_MAP_ZTONGNAME].c_str());
		//	nSubWorldID = SubWorld[nSubWorldIndex].m_SubWorldID;
		} 
		else
		{
            Lua_PushString(L, "暂无数据");
		    Lua_PushString(L, "暂无数据");
		}
	}
	else
	{
		int nMapID = (int)Lua_ValueToNumber(L,1);
		if (nMapID < 0 || nMapID >= MAX_SUBWORLD) return 0;
		Lua_PushString(L, SubWorld[nMapID].nWorldMapInfo[STR_MAP_MASTER].c_str());
		Lua_PushString(L, SubWorld[nMapID].nWorldMapInfo[STR_MAP_ZTONGNAME].c_str());
	}
	return 2;
}

//得到地图的税收和物指
int LuaGetSubWarShuiAndZhi(Lua_State * L)
{
 /*   int nPlayerIndex = GetPlayerIndex(L);

	if (nPlayerIndex > 0)
	{
		int nPosX = 0;
		int nPosY = 0;
		int nPmap=0;
		Npc[Player[nPlayerIndex].m_nIndex].GetMpsPos(&nPosX, &nPosY,&nPmap);
		int nSubWorldIndex = Npc[Player[nPlayerIndex].m_nIndex].m_SubWorldIndex;
        Lua_PushNumber(L, SubWorld[nSubWorldIndex].m_WarShuishou);
		Lua_PushNumber(L, SubWorld[nSubWorldIndex].m_WarZhishu);
	}
	else
	{
		Lua_PushNumber(L,0);
		Lua_PushNumber(L,0);
	}*/
	return 0;
	
}

//设置NPC是否可以重生 以及重生时间

int LuaSetNpcIsRe(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	int nNpcID = (int)Lua_ValueToNumber(L,1);
    int nIsReve = (int)Lua_ValueToNumber(L,2);
	int nReveTime = (int)Lua_ValueToNumber(L,3);
	if(nNpcID>0  && nNpcID < MAX_NPC&& nIsReve>=0 && nReveTime>0)
	{  
         Npc[nNpcID].SetNpcIsRe(nIsReve,nReveTime);
 //       Npc[nNpcID].m_ReviveFrame=nReveTime;
	}
	else
	{
		if (nNpcID < 0 || nNpcID >= MAX_NPC) return 0;
		Npc[nNpcID].SetNpcIsRe(0,999999);
	}
		return 0;
}

int LuaSetNpcReFrame(Lua_State * L)
{
	int nNpcID = (int)Lua_ValueToNumber(L,1);
    int nIsReve = (int)Lua_ValueToNumber(L,2);
	int nReveTime = (int)Lua_ValueToNumber(L,3);
	if(nNpcID>0  && nNpcID < MAX_NPC&& nIsReve>=0 && nReveTime>0)
	{  
		Npc[nNpcID].SetNpcIsReFrame(nIsReve,nReveTime);
		//       Npc[nNpcID].m_ReviveFrame=nReveTime;
	}
	else if(nNpcID>0 && nNpcID < MAX_NPC)
	{
		Npc[nNpcID].SetNpcIsReFrame(1,999999);
	}
	return 0;
}

//设置是否触发BOSS 和执行全局脚本
int LuaSetCreatBossAndScript(Lua_State * L)
{
     int nNpcIdx = (int)Lua_ValueToNumber(L,1);
	 int nIsScirpt = (int)Lua_ValueToNumber(L,2);
	 int nCreatBoss = (int)Lua_ValueToNumber(L,3);
     if (nNpcIdx>0 && nNpcIdx < MAX_NPC)
	 {
		 Npc[nNpcIdx].IsExeGoldScript=nIsScirpt;
		 Npc[nNpcIdx].IsCreatBoss=nCreatBoss;
	 }
	 return 0;
}

//获取NPC的编号 和是否执行全局脚本
int LuaGetNpcSettings(Lua_State * L)
{
	int nNpcIdx = (int)Lua_ValueToNumber(L,1);
	if (nNpcIdx>0 && nNpcIdx < MAX_NPC)
	{
		Lua_PushNumber(L, Npc[nNpcIdx].m_NpcSettingIdx);
		Lua_PushNumber(L, Npc[nNpcIdx].IsExeGoldScript);
		Lua_PushNumber(L, Npc[nNpcIdx].IsCreatBoss);
		Lua_PushNumber(L, Npc[nNpcIdx].m_cGold.GetGoldType());
	}
	else
	{
		Lua_PushNumber(L,0);
		Lua_PushNumber(L,0);
        Lua_PushNumber(L,0);
		Lua_PushNumber(L,0);
	}
	return 4;
}

int LuaGetIndxFromName(Lua_State * L)
{
    char *nNpcName =NULL;
	     nNpcName = (char *)Lua_ValueToString(L,1);
    int nNpcIndex=0;
		 if (nNpcName)
			nNpcIndex =NpcSet.SearchName(nNpcName);

      Lua_PushNumber(L,nNpcIndex);
	  return 1;
}




int LuaGetNpcName(Lua_State * L)
{
//	int nPlayerIndex = GetPlayerIndex(L);
	int nType = (int)Lua_ValueToNumber(L,1);
	if (nType>0 && nType < MAX_NPC)
	{
	   Lua_PushString(L, Npc[nType].Name);
	}
	else
	Lua_PushNil(L);
	return 1;	
}

//是否入帮
int LuaGetBang(Lua_State * L)
{
    int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
        Lua_PushNumber(L, Player[nPlayerIndex].m_cTong.m_nFlag);
	}
	else
		Lua_PushNumber(L,0);
	
	return 1;
	
}

//是否入帮
int LuaGetBangByno(Lua_State * L)
{
    int nPlayerIndex =(int)Lua_ValueToNumber(L,1);// GetPlayerIndex(L);
	if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER)
	{
        Lua_PushNumber(L, Player[nPlayerIndex].m_cTong.m_nFlag);
	}
	else
		Lua_PushNumber(L,-1);
	
	return 1;
	
}

//取得帮派的名称

int LuaGetTonName(Lua_State * L)
{
    //int nPlayerIndex = GetPlayerIndex(L);
	int nPlayerid= (int)Lua_ValueToNumber(L,1);
	if (nPlayerid > 0 && nPlayerid < MAX_PLAYER)
	{
		 //int nPlayerid=PlayerSet.FindPlayerID(nNpcid);
		 Lua_PushString(L,Player[nPlayerid].m_cTong.m_szName);
	}
	else
		 Lua_PushString(L,"无");
	
	return 1;
	
}

//取得帮派的名称

int LuaGetTenBang(Lua_State * L)
{
    int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
        Lua_PushString(L, Player[nPlayerIndex].m_cTong.m_szName);
	}
	else
		Lua_PushString(L,"无");
	
	return 1;
	
}
//取得帮主的信息
int LuaGetPlayerTongNameMaster(Lua_State * L)
{
    int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
        Lua_PushString(L, Player[nPlayerIndex].m_cTong.m_BMasterName);
	}
	else
		Lua_PushNil(L);
	
	return 1;
	
}
//是否帮主
int LuaIsPlayerTongNameMaster(Lua_State * L)
{
  int nPlayerIndex = GetPlayerIndex(L);
  int nReg=0;
  if (nPlayerIndex > 0)
  { 
		if (strcmpi(Npc[Player[nPlayerIndex].m_nIndex].Name, Player[nPlayerIndex].m_cTong.m_BMasterName) == 0)   //这个对比不分字母的大小写
		{
		  nReg=1;
		  Lua_PushNumber(L,nReg);
		}
		else
        Lua_PushNumber(L,nReg);
  }
   else
   Lua_PushNumber(L,nReg);

 return 1;
}
//获取帮派的金钱
int LuaGetQuyBang(Lua_State * L)
{
	int nParamNum=Lua_GetTopIndex(L);
	
	int  nPlayerIndex=0;
	
	if (nParamNum==1)
		nPlayerIndex= (int)Lua_ValueToNumber(L,1);
	else
		nPlayerIndex= GetPlayerIndex(L);

	if (nPlayerIndex > 0)
	{
        Lua_PushNumber(L, Player[nPlayerIndex].m_cTong.m_nMoney);
	}
	else
		Lua_PushNil(L);
	
	return 1;
	
}

int  LuaGetTongMapIdx(Lua_State * L)             //获取帮会地图的索引
{
   	int nParamNum=Lua_GetTopIndex(L);

	int  nPlayerIdx=0;
	
	if (nParamNum==1)
		nPlayerIdx= (int)Lua_ValueToNumber(L,1);
	else
		nPlayerIdx= GetPlayerIndex(L);
	
	if (nPlayerIdx > 0)
	{
        Lua_PushNumber(L, Player[nPlayerIdx].m_cTong.m_Mapid);
	}
	else
		Lua_PushNil(L);
	
	return 1;
}


int  LuaGetMapTongName(Lua_State * L)             //获取归属帮会名
{
	  int nMapIdx = (int)Lua_ValueToNumber(L,1);
	  if (nMapIdx > 0 && nMapIdx < MAX_SUBWORLD)
	  {
		  Lua_PushString(L,SubWorld[nMapIdx].nWorldMapInfo[STR_MAP_ZTONGNAME].c_str());
	  }
	  else
		  Lua_PushNil(L);
	  
	return 1;
}



int LuaGetTongLevel(Lua_State * L)
{
	int  nPlayerIdx = (int)Lua_ValueToNumber(L,1);

	if (nPlayerIdx > 0 && nPlayerIdx < MAX_PLAYER)
	{
        Lua_PushNumber(L, Player[nPlayerIdx].m_cTong.m_nLevel);
	}
	else
		Lua_PushNil(L);
	
	return 1;
}

//获取帮派阵营
int LuaGetCampBang(Lua_State * L)
{
    int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
        Lua_PushNumber(L, Player[nPlayerIndex].m_cTong.m_nCamp);
	}
	else
		Lua_PushNil(L);
	
	return 1;
	
}

int LuaGetTongAttackState(Lua_State * L)
{
    int  nPlayerIdx = (int)Lua_ValueToNumber(L,1);//int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIdx > 0 && nPlayerIdx < MAX_PLAYER)
	{
        Lua_PushNumber(L, Player[nPlayerIdx].m_cTong.m_AttackState);
	}
	else
		Lua_PushNumber(L,-1);
	
	return 1;
	
}


//当前的帮会的  总人数
int LuaGetTongMenberCount(Lua_State * L)
{
	int  nPlayerIdx = (int)Lua_ValueToNumber(L,1);
	if 	(nPlayerIdx>0 && nPlayerIdx < MAX_PLAYER)
	{
		Lua_PushNumber(L,Player[nPlayerIdx].m_cTong.m_CurRenShu);
		return 1;
	}
	  return 0;
}


int LuaRandomSetTongMap(Lua_State * L)
{

	STONG_SET_TONG_INFO_COMMAND setTong;
		  setTong.ProtocolFamily       = pf_alltong;
		  setTong.ProtocolID           = enumC2S_TONG_INFO_SET;
		  setTong.NextProtocolID	   = enumC2S_TONG_RANDOM_MAP;            //具体的设置类型
		  setTong.m_dwTongNameID       = 0;//g_FileName2Id(nTongName);        // 
		  setTong.m_dwNpcID            = 0;             //
		  setTong.m_dwParam            = 0;                         //SubWorld[nMapIdx].m_SubWorldID;
		  sprintf(setTong.m_title,"无");
		  //saTong.m_wLength = sizeof(STONG_GET_TONG_ATTACK_INFO_COMMAND);
		  //sprintf(SubWorld[nMapIdx].m_WarSubTongname,"%s",nTongName);
		  
		  if (g_pTongClient)
			  g_pTongClient->SendPackToServer((const void*)&setTong,sizeof(STONG_SET_TONG_INFO_COMMAND));
		  //printf("---------发送设置帮会信息成功!-----------\n");
		  return 0;
}


int LuaSetTongMap(Lua_State * L)
{
	
	char *nTongName = (char *)Lua_ValueToString(L,1);
	int   nMapIdx = (int)Lua_ValueToNumber(L,2);
		
	STONG_SET_TONG_INFO_COMMAND setTong;
		  setTong.ProtocolFamily       = pf_alltong;
		  setTong.ProtocolID           = enumC2S_TONG_INFO_SET;
		  setTong.NextProtocolID	   = enumC2S_TONG_SET_MAP;            //具体的设置类型
		  setTong.m_dwTongNameID       = g_FileName2Id(nTongName);        // 
		  setTong.m_dwNpcID            = 0;             //
		  setTong.m_dwParam            = nMapIdx;                         //SubWorld[nMapIdx].m_SubWorldID;
		  sprintf(setTong.m_title,"无");
		  //saTong.m_wLength = sizeof(STONG_GET_TONG_ATTACK_INFO_COMMAND);
		  //sprintf(SubWorld[nMapIdx].m_WarSubTongname,"%s",nTongName);
		  if (nMapIdx < 0 || nMapIdx >= MAX_SUBWORLD) return 0;
		  SubWorld[nMapIdx].nWorldMapInfo[STR_MAP_ZTONGNAME] = nTongName;

		  if (g_pTongClient)
			  g_pTongClient->SendPackToServer((const void*)&setTong,sizeof(STONG_SET_TONG_INFO_COMMAND));
		  //printf("---------发送设置帮会信息成功!-----------\n");
		  return 0;
}


//设置帮会的死亡人数
int LuaSetTongDeathCount(Lua_State * L)
{
	char *nTongName = (char *)Lua_ValueToString(L,1);
	int   nCount = (int)Lua_ValueToNumber(L,2);
		
	STONG_SET_TONG_INFO_COMMAND setTong;
		  setTong.ProtocolFamily       = pf_alltong;
		  setTong.ProtocolID           = enumC2S_TONG_INFO_SET;
		  setTong.NextProtocolID	   = enumC2S_TONG_SET_DEATHCOUNT;             //具体的设置类型
		  setTong.m_dwTongNameID       = g_FileName2Id(nTongName);        // 
		  setTong.m_dwNpcID            = 0;//Npc[nNpcIdx].m_dwID;             //
		  setTong.m_dwParam            = nCount;
		  sprintf(setTong.m_title,"无");
		  //saTong.m_wLength = sizeof(STONG_GET_TONG_ATTACK_INFO_COMMAND);
		  if (g_pTongClient)
			  g_pTongClient->SendPackToServer((const void*)&setTong,sizeof(STONG_SET_TONG_INFO_COMMAND));
		  //printf("---------发送设置帮会信息成功!-----------\n");
	   return 0;
}

	 //获取对方宣战帮派名
int LuaSetTongLevel(Lua_State * L)
 {

	char *nTongName = (char *)Lua_ValueToString(L,1);
	int   nLevel = (int)Lua_ValueToNumber(L,2);

	      STONG_SET_TONG_INFO_COMMAND setTong;
		  setTong.ProtocolFamily       = pf_alltong;
		  setTong.ProtocolID           = enumC2S_TONG_INFO_SET;
		  setTong.NextProtocolID	   = enumC2S_TONG_SET_LEVEL;             //具体的设置类型
		  setTong.m_dwTongNameID       = g_FileName2Id(nTongName);        // 
		  setTong.m_dwNpcID            = 0;//Npc[nNpcIdx].m_dwID;             //
		  setTong.m_dwParam            = nLevel;
		  sprintf(setTong.m_title,"无");
		  //saTong.m_wLength = sizeof(STONG_GET_TONG_ATTACK_INFO_COMMAND);
		  if (g_pTongClient)
			  g_pTongClient->SendPackToServer((const void*)&setTong,sizeof(STONG_SET_TONG_INFO_COMMAND));
		  //printf("---------发送设置帮会信息成功!-----------\n");
	   return 0;
 }


//获取对方宣战帮派名
int LuaGetTongAttackName(Lua_State * L)
{
    int nPlayerIndex = (int)Lua_ValueToNumber(L,1);

	if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER)
	{
        Lua_PushString(L, Player[nPlayerIndex].m_cTong.m_AttackName);
	}
	else
		Lua_PushNil(L);
	
	return 1;
	
}

int LuaGetPlayerAccount(Lua_State * L)
{
    int nPlayerIndex = GetPlayerIndex(L);
	int nIndex = nPlayerIndex;
	if (nPlayerIndex > 0)
	{
		if (Lua_GetTopIndex(L) > 1)
			nIndex = (int)Lua_ValueToNumber(L, 1);
        if (nIndex < 0 || nIndex >= MAX_PLAYER) { Lua_PushNil(L); return 1; }
        Lua_PushString(L, Player[nIndex].m_AccoutName);
	}
	else
		Lua_PushNil(L);
	
	return 1;
	
}

int LuaGetPlayerID(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
        Lua_PushNumber(L, Player[nPlayerIndex].m_dwID);
	}
	else
		Lua_PushNil(L);
	
	return 1;
}

int LuaFindAroundNpc(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		if (Lua_IsNumber(L, 1))
		{
			//int nIndex = Player[nPlayerIndex].FindAroundNpc((DWORD)Lua_ValueToNumber(L,1));
			  int nIndex = Player[nPlayerIndex].FindAroundNpcID((DWORD)Lua_ValueToNumber(L,1));
			Lua_PushNumber(L, nIndex); 
			return 1;
		}
		else
			Lua_PushNil(L);
	}
	return 0;
}

int LuaFindAroundPlayer(lua_State* L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || Player[nPlayerIndex].m_nIndex <= 0)
    {
        Lua_NewTable(L);
        return 1;
    }

    int myNpcIdx = Player[nPlayerIndex].m_nIndex;
    int myMap = 0, myX = 0, myY = 0;
    Npc[myNpcIdx].GetMpsPos(&myX, &myY, &myMap);

	int phamvi = Lua_ValueToNumber(L,1);

    int radiusSquared = phamvi * phamvi;
    int nIndex = PlayerSet.GetFirstPlayer();
    int count = 1;

    Lua_NewTable(L); // table to return

    while (nIndex > 0)
    {
        if (nIndex != nPlayerIndex && Player[nIndex].m_nIndex > 0)
        {
            int otherNpcIdx = Player[nIndex].m_nIndex;
            int otherMap = 0, ox = 0, oy = 0;
            Npc[otherNpcIdx].GetMpsPos(&ox, &oy, &otherMap);

            if (otherMap == myMap)
            {
                int dx = myX - ox;
                int dy = myY - oy;
                int distSquared = dx * dx + dy * dy;

                if (distSquared <= radiusSquared)
                {
                    Lua_PushNumber(L, count);
                    Lua_PushNumber(L, nIndex);
                    lua_settable(L, -3);
                    count++;
                }
            }
        }

        nIndex = PlayerSet.GetNextPlayer();
    }

    return 1; 
}

int LuaGetPlayerLeadExp(Lua_State * L)
{
	
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
        Lua_PushNumber(L, Player[nPlayerIndex].m_dwLeadExp);
	}
	else
		Lua_PushNil(L);
	
	return 1;
}

int LuaGetPlayerLeadLevel(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
        Lua_PushNumber(L, Player[nPlayerIndex].m_dwLeadLevel);
	}
	else
		Lua_PushNil(L);
	
	return 1;
}



int LuaGetPlayerRestAttributePoint(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
        Lua_PushNumber(L, Player[nPlayerIndex].m_nAttributePoint);
	}
	else
		Lua_PushNil(L);
	
	return 1;
}

int LuaGetPlayerRestSkillPoint(Lua_State * L)
{
    int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
        Lua_PushNumber(L, Player[nPlayerIndex].m_nSkillPoint);
	}
	else
		//Lua_PushNil(L);
		Lua_PushNumber(L,0);
	
	return 1;
}

/*
int LuaModifyPlayerRestSkillPoint(Lua_State *L)
{
//Question
int nPlayerIndex = GetPlayerIndex(L);
if (nPlayerIndex > 0)
{
int nDValue = (int)Lua_ValueToNumber(L, 1);

  int nNewSkillPoint = Player[nPlayerIndex].m_nSkillPoint + nDValue;
  if (nNewSkillPoint < 0 ) return 0;
  
	Player[nPlayerIndex].m_nSkillPoint = nNewSkillPoint;
	}
	
	  return 0;
	  }
*/

//基本属性
int LuaGetPlayerLucky(Lua_State * L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex > 0)
    {
        int nType = (int)Lua_ValueToNumber(L, 1);
        int nResult = 0;

        switch (nType)
        {
        case 0:
            nResult = Player[nPlayerIndex].m_nCurLucky + Npc[Player[nPlayerIndex].m_nIndex].IsLuKey;
            break;
        case 1:
			{
				if	(Player[nPlayerIndex].m_nCurLucky >= 80)
					nResult = 0;
				else
					nResult = Player[nPlayerIndex].m_nCurLucky;
				break;
			}
        case 2:
            nResult = Player[nPlayerIndex].m_nLucky;
            break;
        case 3:
            nResult = Player[nPlayerIndex].m_nCurLucky * Npc[Player[nPlayerIndex].m_nIndex].IsLuKey * Npc[Player[nPlayerIndex].m_nIndex].m_GoldLucky;
            break;
        default:
            Lua_PushNil(L);
            return 1;
        }

        Lua_PushNumber(L, nResult);
    }
    return 1;
}

int LuaGetPlayerEngergy(Lua_State * L)
{
    int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		
		int nType = (int)Lua_ValueToNumber(L,1);
		switch((int)Lua_ValueToNumber(L,1))
		{
		case 0:
			Lua_PushNumber(L, Player[nPlayerIndex].m_nCurEngergy);break;
		case 1:
			Lua_PushNumber(L, Player[nPlayerIndex].m_nEngergy);break;
		case 2:
			Lua_PushNumber(L, Player[nPlayerIndex].m_nEngergy);break;
		default:
			Lua_PushNil(L);
		}
	}
	
	return 1;
}

// 重置基本属性
// ResetBaseAttribute(type,data);
int LuaResetBaseAttribute(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	
	int nParamCount = 0;
	if ( (nParamCount = Lua_GetTopIndex(L)) < 2) return 0;

	if (nPlayerIndex > 0)
	{		
		PLAYER_ADD_BASE_ATTRIBUTE_COMMAND cmd;
		cmd.m_btAttribute = (int)Lua_ValueToNumber(L,1);;
		cmd.m_nAddNo = (int)Lua_ValueToNumber(L,2);;
		cmd.ProtocolType = c2s_playeraddbaseattribute;
		Player[nPlayerIndex].ResetBaseAttribute((BYTE*)&cmd);
	}

	return 1;
}

int LuaGetPlayerDexterity(Lua_State * L)
{
    int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		
		int nType = (int)Lua_ValueToNumber(L,1);
		switch((int)Lua_ValueToNumber(L,1))
		{
		case 0:
			Lua_PushNumber(L, Player[nPlayerIndex].m_nCurDexterity);break;
		case 1:
			Lua_PushNumber(L, Player[nPlayerIndex].m_nDexterity);break;
		case 2:
			Lua_PushNumber(L, Player[nPlayerIndex].m_nDexterity);break;
		default:
			Lua_PushNil(L);
		}
		
	} 
	return 1;
}

int LuaGetPlayerStrength(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		int nType = (int)Lua_ValueToNumber(L,1);
		switch((int)Lua_ValueToNumber(L,1))
		{
		case 0:
			Lua_PushNumber(L, Player[nPlayerIndex].m_nCurStrength);break;
		case 1:
			Lua_PushNumber(L, Player[nPlayerIndex].m_nStrength);break;
		case 2:
			Lua_PushNumber(L, Player[nPlayerIndex].m_nStrength);break;
		default:
			Lua_PushNil(L);
		}
		
	} 
	return 1;
	
}

int LuaGetPlayerVitality(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		
		int nType = (int)Lua_ValueToNumber(L,1);
		switch((int)Lua_ValueToNumber(L,1))
		{
		case 0:
			Lua_PushNumber(L, Player[nPlayerIndex].m_nCurVitality);break;
		case 1:
			Lua_PushNumber(L, Player[nPlayerIndex].m_nVitality);break;
		case 2:
			Lua_PushNumber(L, Player[nPlayerIndex].m_nVitality);break;
		default:
			Lua_PushNil(L);
		}
		
	} 
	return 1;
	
}

int LuaGetPlayerCashMoney(Lua_State * L)
{
    int nPlayerIndex = GetPlayerIndex(L);
	
    if (nPlayerIndex > 0)
	{
		Lua_PushNumber(L, Player[nPlayerIndex].m_ItemList.GetsMon(room_equipment));
	}
	else Lua_PushNumber(L,0);
	
	return 1;
}

int LuaPlayerPayMoney(Lua_State * L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER || Player[nPlayerIndex].m_cTrade.m_nIsTrading != 0)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    int nMoney = (int)Lua_ValueToNumber(L, 1);
    if (nMoney <= 0)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    if (Player[nPlayerIndex].Pay(nMoney))
        Lua_PushNumber(L, 1);
    else
        Lua_PushNumber(L, 0);
    return 1;
}

int LuaPlayerPayXu(Lua_State * L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER || Player[nPlayerIndex].m_cTrade.m_nIsTrading != 0)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    int nXu = (int)Lua_ValueToNumber(L, 1);
    if (nXu <= 0)
    {
        Lua_PushNumber(L, 0);
        return 1;
    }
    if (Player[nPlayerIndex].PayXu(nXu))
        Lua_PushNumber(L, 1);
    else
        Lua_PushNumber(L, 0);
    return 1;
}
int LuaPlayerEarnMoney (Lua_State  *L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex > 0)
	{
        int nMoney = (int)Lua_ValueToNumber(L, 1);
        if (nMoney <= 0) return 0;
        Player[nPlayerIndex].Earn(nMoney);
	}
	return 0;	
}

int LuaPlayerGetXu(Lua_State  *L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
    Lua_PushNumber(L,Player[nPlayerIndex].GetCurXu());
    else
    Lua_PushNumber(L,0);
	return 1;
}
int LuaPlayerEarnXu (Lua_State  *L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex > 0)
	{
        int nXu = (int)Lua_ValueToNumber(L, 1);
        if (nXu <= 0) return 0;
        Player[nPlayerIndex].EarnXu(nXu);
	}
	return 0;	
}

int LuaPlayerPrePayMoney(Lua_State *L)
{   
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex > 0)
	{
        int nMoney = (int)Lua_ValueToNumber(L, 1);
        if (nMoney <= 0) return 0;
        if (Player[nPlayerIndex].PrePay(nMoney))
			Lua_PushNumber(L, 1);
		else
			Lua_PushNumber(L, 0);
	}
	else
		Lua_PushNumber(L, 0);
	
	return 1;
}

int LuaPlayerPrePayXu(Lua_State *L)
{   
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex > 0)
	{
        int nXu = (int)Lua_ValueToNumber(L, 1);
        if (nXu <= 0) return 0;
        if (Player[nPlayerIndex].PrePayXu(nXu))
			Lua_PushNumber(L, 1);
		else
			Lua_PushNumber(L, 0);
	}
	else
		Lua_PushNumber(L, 0);
	
	return 1;
}


int LuaPlayerExecuteScript(Lua_State * L)
{	
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0 || Player[nPlayerIndex].m_dwID <= 0) return FALSE;
    char *nScriptfiled=NULL;
	char *nFunName    =NULL;
	int  nVal=0,nReg=0;
	nScriptfiled = (char *)Lua_ValueToString(L, 1);
    nFunName     = (char *)Lua_ValueToString(L, 2);
	nVal         = (int)Lua_ValueToNumber(L, 3);
    nReg         = Player[nPlayerIndex].ExecuteScriptA(nScriptfiled,nFunName,nVal,"空");
	Lua_PushNumber(L, nReg);
	return 1;
}
//重载ini
int LuaLocalSerIni(Lua_State * L)
{
   /*char * nIniName[] =  
   {    
        "g_GameSetTing",
		"g_NpcMapDropRate",
		"g_TaskLoop",
		NULL  
	};
	for (int k=0;nIniName[k];++k)
	{ 

	} */

  int nNum = (int)Lua_ValueToNumber(L, 1);
  if (nNum==1)
  {
	  g_GameSetTing.Clear();
      g_GameSetTing.Load(gameSetPath.c_str());
	  _gameGlobalSet.Init();
	  Lua_PushString(L, gameSetPath.c_str());
      return 1;
  }
  else if (nNum==2)
  {
	  g_NpcMapDropRate.Clear();
	  g_NpcMapDropRate.Load(MAP_DROPRATE_FILE);
	  Lua_PushString(L, MAP_DROPRATE_FILE);
	  return 1;
  }
  else if (nNum==3)
  {
	  g_TaskLoop.Clear();
	  g_TaskLoop.Load(GAME_TASK_FILE);

	  g_SubWorldSet.LoadTaskInfo();
	  Lua_PushString(L, GAME_TASK_FILE);
	  return 1;
  }

  Lua_PushNumber(L, 0);
  return 1;
}

extern KTabFile g_FusionTab;
//重载txt
int LuaLocalSerTxt(Lua_State * L)
{
  int nNum = (int)Lua_ValueToNumber(L, 1);

  if (nNum==1)
  {
       g_ForbitWaiGua.Clear();
       g_ForbitWaiGua.Load(GAME_FORBITWAIGUA_FILE);
	   Lua_PushString(L, GAME_FORBITWAIGUA_FILE);
       return 1;	
  }
  else if (nNum==2)
  {
	  g_ForbitMap.Clear();
	  g_ForbitMap.Load(GAME_FORBITMAP_FILE);
	  Lua_PushString(L, GAME_FORBITMAP_FILE);
	  return 1;	
  }
 else if (nNum==3)
 { 
	g_FsJinMai.Clear();
	g_FsJinMai.Load(GAME_FSJINMAI_FILE);
	Lua_PushString(L, GAME_FSJINMAI_FILE);
	return 1;	
 }

 else if (nNum==4)
 {  
	g_GMSetting.Clear();
	g_GMSetting.Load(GAME_GM_FILE);
	Lua_PushString(L, GAME_GM_FILE);
	return 1;	
 }
 else if (nNum==5)
 {   
	g_OrdinSkillsSetting.Clear();

	if (g_OrdinSkillsSetting.Load(SKILL_SETTING_FILE))
	   g_SkillManager.ClearAndInit();

	Lua_PushString(L, SKILL_SETTING_FILE);
	return 1;	
 }
 else if (nNum==6)
 {   
	  g_NpcSetting.Clear();
	  g_NpcSetting.Load(NPC_SETTING_FILE);
	 Lua_PushString(L, NPC_SETTING_FILE);
	 return 1;	
 }

 else if (nNum==7)
 {    
 	g_MisslesSetting.Clear();
	if (g_MisslesSetting.Load(MISSLES_SETTING_FILE))
	   InitMissleSetting();

	Lua_PushString(L, MISSLES_SETTING_FILE);
	return 1;	
 }
 else if (nNum==8)
 {    
	 g_AccoutSet.Clear();
	 g_AccoutSet.Load(GAME_ACCT_FILE);
	 Lua_PushString(L, GAME_ACCT_FILE);
	 return 1;	
 }
 else if (nNum==9)
 {    
	 g_CompEquip.Clear();
	 g_CompEquip.Load(GAME_COMP_FILE);
	 Lua_PushString(L, GAME_COMP_FILE);
	 return 1;	
 }
 
else if (nNum==10)
 {    
	 g_CompAtlas.Clear();
	 g_CompAtlas.Load(GAME_ATLAS_FILE);
	 Lua_PushString(L, GAME_ATLAS_FILE);
	 return 1;	
 }
else if (nNum==11)
{    
	g_GetMsg.Clear();
	g_GetMsg.Load(GAME_MSG_FILE);
	Lua_PushString(L, GAME_MSG_FILE);
	return 1;	
 }
else if (nNum==12)
{    
	g_MineItemTab.Clear();
	g_MineItemTab.Load(MINEITEM_TABFILE);
	Lua_PushString(L, MINEITEM_TABFILE);
	return 1;	
}
else if (nNum==13)
{    
	g_EventItemTab.Clear();
	g_EventItemTab.Load(QUESTITEM_TABFILE);
	Lua_PushString(L, QUESTITEM_TABFILE);
	return 1;	
}
else if (nNum==14)
{    
	g_FusionTab.Clear();
	g_FusionTab.Load(FUSION_TABFILE);
	Lua_PushString(L, FUSION_TABFILE);
	return 1;	
}
else if (nNum==15)
{
	g_MissionTabFile.Clear();
	g_MissionTabFile.Load(TASK_MISSION_SETTING_TABFILE);
	Lua_PushString(L, TASK_MISSION_SETTING_TABFILE);
	return 1;
}

else if (nNum==16)
{
	g_ReBornSetting.Clear();
	g_ReBornSetting.Load(GAME_REBORN_FILE);
	Lua_PushString(L, GAME_REBORN_FILE);
	return 1;
}
else if (nNum==17)
{
	g_PlayerVipSetting.Clear();
	g_PlayerVipSetting.Load(GAME_PVIP_FILE);
	Lua_PushString(L, GAME_PVIP_FILE);
	return 1;
}
else if (nNum==18)
{
	g_gmList.Clear();
	g_gmList.Load(GAME_GM_FILE);
	Lua_PushString(L, GAME_GM_FILE);
	return 1;
}
else if (nNum==19)
{  //黄金怪物模板初始化
	NpcSet.m_cGoldTemplate.Init();                     
	Lua_PushString(L, NPC_GOLD_TEMPLATE_FILE);
	return 1;
}

  Lua_PushNumber(L, 0);
  return 1;	
}

//重载全部脚本
int LuaLocalAllScript(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0 || Player[nPlayerIndex].m_dwID <= 0) return FALSE;
    ReLoadAllScript();
    return 0;
}

//重载脚本
int LuaLocalScript(Lua_State * L)
{
	int nParamCount = Lua_GetTopIndex(L);
	int nPlayerIndex = GetPlayerIndex(L);
	int nRegm=0;
	if (nPlayerIndex < 0 || Player[nPlayerIndex].m_dwID <= 0) goto goout;
	char *nScriptid= (char *)Lua_ValueToString(L, 1);

	if  (!nScriptid) goto goout;

	int nLoadModel=0;
	if (nParamCount>=2)
		nLoadModel = (int)Lua_ValueToNumber(L, 2);

	char szScriptFile[256];
	ZeroMemory(szScriptFile,sizeof(szScriptFile));
	switch(nLoadModel)
	{
	case 1:
		if (NULL == Utf8ToGbk(nScriptid, szScriptFile, sizeof(szScriptFile))) szScriptFile[0] = '\0';
		break;
	case 2:
	    snprintf(szScriptFile, sizeof(szScriptFile), "%s", GbkToUtf8(nScriptid).c_str());
		break;
	default:
		sprintf(szScriptFile,"%s",nScriptid);
		break;
	}

	if (szScriptFile[0] == 0) goto goout;
	    nRegm=ReLoadScript(szScriptFile);
goout:
    Lua_PushNumber(L, nRegm);
	return 1;
}

//攻击NPC
//Attack dwID, Damage
int LuaAttackNpc(Lua_State * L)
{
	int nParamCount = 0;
	if ( (nParamCount = Lua_GetTopIndex(L)) < 2) return 0;
	
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	
	DWORD nNpcID = (DWORD)Lua_ValueToNumber(L,1);
	int nNpcIndex = Player[nPlayerIndex].FindAroundNpc(nNpcID);//NpcSet.SearchID(nNpcID);
	if (nNpcIndex <= 0) return 0;
	
	KMagicAttrib DamageMagicAttribs[MAX_MISSLE_DAMAGEATTRIB];
	memset(DamageMagicAttribs, 0, sizeof(DamageMagicAttribs));
	
	DamageMagicAttribs[0].nAttribType = magic_attackrating_v;
	DamageMagicAttribs[0].nValue[0] = 65534;
	
	DamageMagicAttribs[1].nAttribType = magic_ignoredefense_p;
	DamageMagicAttribs[1].nValue[0] = 1;
	
	for (int i = 0; i < nParamCount - 1; ++i)
	{
		int nVlau = (int)Lua_ValueToNumber(L, 2 + i);
		DamageMagicAttribs[i + 2].nValue[0] = (int)Lua_ValueToNumber(L, 2 + i);
		DamageMagicAttribs[i + 2].nValue[2] = (int)Lua_ValueToNumber(L, 2 + i);
	}
	
//	Npc[nNpcIndex].ReceiveDamage(Player[nPlayerIndex].m_nIndex,-1, 0, DamageMagicAttribs, 0, TRUE,0);
	return 0;
}
//禁言
int LuaSetPlayerChatForbiddenFlag(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 1) 
		return 0;
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0)
		return 0;
	int nFlag = Lua_ValueToNumber(L, 1);
	Player[nPlayerIndex].SetChatForbiddenFlag(nFlag);
	return 0;
}


int LuaSetLockRole(Lua_State * L)
{
    if (Lua_GetTopIndex(L) < 1)
		return 0;
	
	const char*	pszName = (const char*)Lua_ValueToString(L, 1);
	int nFlag=0;
	nFlag= (int) Lua_ValueToNumber(L, 2);
	
	int nIndex = PlayerSet.GetFirstPlayer(); //查找玩家姓名
	while(nIndex > 0)
	{
		if (pszName && strcmp(Player[nIndex].m_PlayerName, pszName) == 0)
			break;
		
		nIndex = PlayerSet.GetNextPlayer();
	}
	
	if (nIndex && Player[nIndex].m_nNetConnectIdx >= 0)
	{
		Player[nIndex].SetChatForbiddenFlag(nFlag);
        Npc[Player[nIndex].m_nIndex].m_IsSerLock=nFlag;
		//Npc[Player[nIndex].m_nIndex].m_IsWaiGua=nFlag;
	}
	
    Lua_PushNumber(L,nFlag);
	return 1;
}


//禁言某人
int LuaSetPlayerForbiddenChat(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 1)
		return 0;

	const char*	pszName = (const char*)Lua_ValueToString(L, 1);
		int nFlag=0;
		    nFlag= (int) Lua_ValueToNumber(L, 2);
	
	int nIndex = PlayerSet.GetFirstPlayer(); //查找玩家姓名
	while(nIndex > 0)
	{
		if (pszName && strcmp(Player[nIndex].m_PlayerName, pszName) == 0)
			break;
		
		nIndex = PlayerSet.GetNextPlayer();
	}

	if (nIndex && Player[nIndex].m_nNetConnectIdx >= 0)
	{
		Player[nIndex].SetChatForbiddenFlag(nFlag);
	}

    Lua_PushNumber(L,nFlag);
	return 1;
}

int LuaGm_MoveTo(Lua_State *L)
{
	if (Lua_GetTopIndex(L) < 1)
		return 0;

	tagGmAction gm;
	memset(&gm,0,sizeof(gm));

	gm.cProtocol = c2s_gm_synroleaction;
	gm.cCmdType  = enum_COMMAND_ID_MOVETOP_GM;
	ZeroMemory(gm.nRoleName,sizeof(gm.nRoleName));
	g_StrCpyLen(gm.nRoleName, Lua_ValueToString(L,1), sizeof(gm.nRoleName));
	g_StrCpyLen(gm.nGmRoleName, Lua_ValueToString(L,2), sizeof(gm.nGmRoleName));
	gm.Key=nCurSerIdx;
	gm.verX=nCurGsNumber;
	gm.verY=0;
	if (g_pGateWayClient)
		g_pGateWayClient->SendPackToServer(&gm, sizeof(tagGmAction));

	return 0;
}

int LuaGm_Level(Lua_State *L)
{
	if (Lua_GetTopIndex(L) < 2)
		return 0;

	tagGmAction gm;
	memset(&gm,0,sizeof(gm));

	gm.cProtocol = c2s_gm_synroleaction;
	gm.cCmdType  = enum_COMMAND_ID_LEVEL_GM;
	ZeroMemory(gm.nRoleName,sizeof(gm.nRoleName));
	g_StrCpyLen(gm.nRoleName, Lua_ValueToString(L,1), sizeof(gm.nRoleName));
	gm.Key=0;
	gm.verX=(int)Lua_ValueToNumber(L,2) ;
	gm.verY=0;

	if (g_pGateWayClient)
		g_pGateWayClient->SendPackToServer(&gm, sizeof(tagGmAction));

	return 0;
}

int LuaGm_Money(Lua_State *L)
{
	if (Lua_GetTopIndex(L) < 2)
		return 0;

	tagGmAction gm;
	memset(&gm,0,sizeof(gm));

	gm.cProtocol = c2s_gm_synroleaction;
	gm.cCmdType  = enum_COMMAND_ID_MONEY_GM;
	ZeroMemory(gm.nRoleName,sizeof(gm.nRoleName));
	g_StrCpyLen(gm.nRoleName, Lua_ValueToString(L,1), sizeof(gm.nRoleName));
	gm.Key=0;
	gm.verX=(int)Lua_ValueToNumber(L,2) ;
	gm.verY=0;

	if (g_pGateWayClient)
		g_pGateWayClient->SendPackToServer(&gm, sizeof(tagGmAction));

	return 0;
}

int LuaGm_Xu(Lua_State *L)
{
	if (Lua_GetTopIndex(L) < 2)
		return 0;

	tagGmAction gm;
	memset(&gm,0,sizeof(gm));

	gm.cProtocol = c2s_gm_synroleaction;
	gm.cCmdType  = enum_COMMAND_ID_XU_GM;
	ZeroMemory(gm.nRoleName,sizeof(gm.nRoleName));
	g_StrCpyLen(gm.nRoleName, Lua_ValueToString(L,1), sizeof(gm.nRoleName));
	gm.Key=0;
	gm.verX=(int)Lua_ValueToNumber(L,2) ;
	gm.verY=0;
	if (g_pGateWayClient)
		g_pGateWayClient->SendPackToServer(&gm, sizeof(tagGmAction));

	return 0;
}

int LuaGm_TaskId(Lua_State *L)
{
	if (Lua_GetTopIndex(L) < 3)
		return 0;

	if (Lua_GetTopIndex(L) < 1)
		return 0;

	tagGmAction gm;
	memset(&gm,0,sizeof(gm));

	gm.cProtocol = c2s_gm_synroleaction;
	gm.cCmdType  = enum_COMMAND_ID_TASK_GM;
	ZeroMemory(gm.nRoleName,sizeof(gm.nRoleName));
	g_StrCpyLen(gm.nRoleName, Lua_ValueToString(L,1), sizeof(gm.nRoleName));
	gm.Key=(int)Lua_ValueToNumber(L,2) ;
	gm.verX=(int)Lua_ValueToNumber(L,3) ;
	gm.verY=0;
	if (g_pGateWayClient)
		g_pGateWayClient->SendPackToServer(&gm, sizeof(tagGmAction));

	return 0;
}


int LuaGm_ForbitChat(Lua_State *L)
{
	if (Lua_GetTopIndex(L) < 2)
		return 0;
	//发往ｂｉｓｈｏｐ　查找玩家所在的ｇｓ和大区　　c2s_gm_synroleaction
	tagGmAction gm;
	memset(&gm,0,sizeof(gm));
	gm.cProtocol = c2s_gm_synroleaction;
	gm.cCmdType  = enum_COMMAND_ID_FORBITCHAT_GM;
	ZeroMemory(gm.nRoleName,sizeof(gm.nRoleName));
	g_StrCpyLen(gm.nRoleName, Lua_ValueToString(L,1), sizeof(gm.nRoleName));
	gm.Key=0;
	gm.verX=(int)Lua_ValueToNumber(L,2) ;
	gm.verY=0;
	if (g_pGateWayClient)
		g_pGateWayClient->SendPackToServer(&gm, sizeof(tagGmAction));

	return 0;
}

//ｇｍ踢掉玩家
int LuaGm_KickOutPlayer(Lua_State *L)
{
	if (Lua_GetTopIndex(L) < 1)
		return 0;
	tagGmAction gm;
	ZeroMemory(gm.nRoleName,sizeof(gm.nRoleName));
	gm.cProtocol = c2s_gm_synroleaction;
	gm.cCmdType  = enum_COMMAND_ID_KICKOUTROLE_GM;
	g_StrCpyLen(gm.nRoleName, Lua_ValueToString(L,1), sizeof(gm.nRoleName));
	gm.Key = (int) Lua_ValueToNumber(L,2);  //模式 是直接断线 还是 卡号处理直接删除
	gm.verX=0 ;
	gm.verY=0;
	
	if (g_pGateWayClient)
		g_pGateWayClient->SendPackToServer(&gm, sizeof(tagGmAction));

	return 0;
}

//踢掉某个玩家--姓名
int LuaKickOutPlayer(Lua_State *L)
{
	if (Lua_GetTopIndex(L) < 1)
		return 0;

	const char*	pszName = (const char*)Lua_ValueToString(L, 1);
	
	int nIndex = PlayerSet.GetFirstPlayer(); //查找玩家姓名
	while(nIndex > 0)
	{
		if (pszName && strcmp(Player[nIndex].m_PlayerName, pszName) == 0)
			break;
		
		nIndex = PlayerSet.GetNextPlayer();
	}

	if (nIndex && Player[nIndex].m_nNetConnectIdx >= 0)
	{
		if (Player[nIndex].getMobileClient())
		{
			printf("GM Kick out player one from Mobile.\n");
			Player[nIndex].KickOutGame();
		}
		else
		{
			printf("GM Kick out specific player from Pc.\n");
			g_pServer->ShutdownClient(Player[nIndex].m_nNetConnectIdx);
		}
	}
	return 0;
}

int LuaShutdownClient(Lua_State *L)
{
	if (Lua_GetTopIndex(L) < 1)
		return 0;

	const char*	pszName = (const char*)Lua_ValueToString(L, 1);

	int nIndex = PlayerSet.GetFirstPlayer(); //查找玩家姓名
	while(nIndex > 0)
	{
		if (pszName && strcmp(Player[nIndex].m_PlayerName, pszName) == 0)
			break;

		nIndex = PlayerSet.GetNextPlayer();
	}

	if (g_pServer && nIndex && Player[nIndex].m_nNetConnectIdx >= 0)
	{//通知玩家掉线
		if (FAILED(g_pServer->ShutdownClient(Player[nIndex].m_nNetConnectIdx)))
		{//如果断开失败 就直接删除
			printf("[lua ShutdownClient FAILED]:--AccName[%s], m_nNetConnectIdx:(%d)! ShutDown Strong--\n", Npc[Player[nIndex].m_nIndex].Name, Player[nIndex].m_nNetConnectIdx);
			//要发送到 
			tagLeaveGame	sLeaveGame;	
			sLeaveGame.cProtocol = c2s_leavegame;   // 发送往bishop 通知 paysys
			sLeaveGame.cCmdType  = NORMAL_LEAVEGAME;

			tagLeaveGame2 lg2;
			lg2.ProtocolFamily = pf_normal;
			lg2.ProtocolID     = c2s_leavegame;	   // S3 玩家离开游戏
			lg2.cCmdType       = NORMAL_LEAVEGAME;
			lg2.nSelServer     = nCurSerIdx;      //  MAKELONG(m_GsNumBer, m_SerVerNo);//

			sLeaveGame.nExtPoint    = Player[nIndex].GetExtPointChanged();
			sLeaveGame.nGameExtTime = Player[nIndex].GetExtTimeChanged();
			snprintf(sLeaveGame.nClientIp, sizeof(sLeaveGame.nClientIp), "%s", Npc[Player[nIndex].m_nIndex].Name);
			snprintf(sLeaveGame.szAccountName, sizeof(sLeaveGame.szAccountName), "%s", Player[nIndex].m_AccoutName);

			if (g_pGateWayClient)
				g_pGateWayClient->SendPackToServer(&sLeaveGame, sizeof(tagLeaveGame)); //通知bishop
			if (g_pTranClient)//通知S3
				g_pTranClient->SendPackToServer(&lg2, sizeof(tagLeaveGame2));
			if (g_pChatClient)
				g_pChatClient->SendPackToServer(&lg2, sizeof(tagLeaveGame2));
			if (g_pTongClient)
				g_pTongClient->SendPackToServer(&lg2, sizeof(tagLeaveGame2));

			tagRoleEnterGame reg;
			reg.ProtocolType = c2s_roleserver_lock;
			reg.bLock  = false;            //删除锁定
			reg.Lixian = false;            //是否离线的
			reg.nRegServerIndex = nCurSerIdx;  //属于那个大区
			reg.nGsNumber       = nCurGsNumber;  //GS索引号
			snprintf(reg.Name, sizeof(reg.Name), "%s", Npc[Player[nIndex].m_nIndex].Name);
			if (g_pDataClient)
				g_pDataClient->SendPackToServer((const void *)&reg, sizeof(tagRoleEnterGame));
			if (g_pGateWayClient)
				g_pGateWayClient->SendPackToServer((const void *)&reg, sizeof(tagRoleEnterGame));

			PlayerSet.RemoveInGame(nIndex);
		}
	}
	return 0;
}


//踢掉这个账号！解决卡号的问题
int LuaKickOutAccount(Lua_State *L)
{
	if (Lua_GetTopIndex(L) < 1)
		return 0;

	const char*	pszName = (const char*)Lua_ValueToString(L, 1);
	
	int nIndex = PlayerSet.GetFirstPlayer();
	while(nIndex > 0)
	{
		if (strcmpi(Player[nIndex].m_AccoutName, pszName) == 0)
			break;
		
		nIndex = PlayerSet.GetNextPlayer();
	}
	if (nIndex && Player[nIndex].m_nNetConnectIdx >= 0)
	{
		if (Player[nIndex].getMobileClient())
		{
			printf("GM Kick out player one from Mobile.\n");
			Player[nIndex].KickOutGame();
		}
		else
		{
			printf("GM Kick out specific player from PC.\n");
			g_pServer->ShutdownClient(Player[nIndex].m_nNetConnectIdx);
		}

	}
	return 0;
}
//是否使用外挂
int LuaGetIsUseWaigua(Lua_State *L)
{
	int nIndex = GetPlayerIndex(L);
	
	if (nIndex <= 0)
	{
		Lua_PushNumber(L,0);
		return 1;
	}
	    Lua_PushNumber(L, 0);
	    return 1;
}

int LuagetLanguageByIndex(Lua_State *L)
{
	int nIndex = (int)Lua_ValueToNumber(L,1);

	if  (nIndex<=0 || nIndex>=MAX_PLAYER) goto Exit0;

	int nResultValue = 0;

	if (Player[nIndex].m_nNetConnectIdx >= 0)
	{
		nResultValue = Player[nIndex].getMobileClientLanguage();
	}

Exit0:
	Lua_PushNumber(L, nResultValue);
	return 1;
}

int LuagetClientLanguage(Lua_State *L)
{
	int nIndex = GetPlayerIndex(L);
	int nResultValue = 0;

	if (nIndex <= 0 || nIndex>=MAX_PLAYER)
		goto Exit0;

	if (Player[nIndex].m_nNetConnectIdx >= 0)
	{
		nResultValue = Player[nIndex].getMobileClientLanguage();
	}

Exit0:
	Lua_PushNumber(L, nResultValue);
	return 1;
}

int LuagetMobileClient(Lua_State *L)
{
	int nIndex = GetPlayerIndex(L);
	int nResultValue = 0;

	if (nIndex <= 0)
		goto Exit0;

	if (Player[nIndex].m_nNetConnectIdx >= 0)
	{
		nResultValue = Player[nIndex].getMobileClient();
	}

Exit0:
	Lua_PushNumber(L, nResultValue);
	return 1;
}
//自己踢掉自己
int LuaKickMobileOutSelf(Lua_State *L)
{
	int nIndex = GetPlayerIndex(L);

	if (nIndex <= 0)
		return 0;

	if (Player[nIndex].m_nNetConnectIdx >= 0)
	{
		Player[nIndex].KickOutGame();
	}
	return 0;
}
//自己踢掉自己
int LuaKickOutSelf(Lua_State *L)
{
	int nIndex = GetPlayerIndex(L);
	
	if (nIndex <= 0)
		return 0;
	
	if (Player[nIndex].m_nNetConnectIdx >= 0)
	{
		if (Player[nIndex].getMobileClient())
		{
			printf("GM Kick out player one from Mobile.\n");
		    Player[nIndex].KickOutGame();
		}
		else
		{
			printf("GM Kick out player one from PC.\n");
			g_pServer->ShutdownClient(Player[nIndex].m_nNetConnectIdx);
		}
	}
	return 0;
}
//怒气使用的伤害
int LuaUseAngrytoNpc(Lua_State * L)
{
	int nParamCount = 0;
	if ( (nParamCount = Lua_GetTopIndex(L)) < 1) return 0;
	
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
    int nDamageVal= (int)Lua_ValueToNumber(L,1);
	int nNpcIndex=Npc[Player[nPlayerIndex].m_nIndex].m_nPeopleIdx;
//	int nNpcIndex = Player[nPlayerIndex].FindAroundNpc(nNpcID);
	if (nNpcIndex<=0)
	{//设置使用技能
       
       Lua_PushNumber(L, 0);
	   return 1;
	}
	KMagicAttrib DamageMagicAttribs[MAX_MISSLE_DAMAGEATTRIB];
	memset(DamageMagicAttribs, 0, sizeof(DamageMagicAttribs));
	
	DamageMagicAttribs[0].nAttribType = magic_attackrating_v;  //命中率
	DamageMagicAttribs[0].nValue[0] = 50000000;
	
	DamageMagicAttribs[1].nAttribType = magic_ignoredefense_p; //忽略敌人闪避率
	DamageMagicAttribs[1].nValue[0] = 1;
	
	DamageMagicAttribs[2].nAttribType = magic_physicsdamage_v;
	DamageMagicAttribs[2].nValue[0] = nDamageVal;  //最小物理伤害
	DamageMagicAttribs[2].nValue[2] = nDamageVal;  //最大物理伤害
	
//	Npc[nNpcIndex].ReceiveDamage(Player[nPlayerIndex].m_nIndex,-1, 0, DamageMagicAttribs, 0, TRUE,0);
	Lua_PushNumber(L, 1);
	return 1;
}


int LuaKillNpcWithIdx(Lua_State * L)
{
	int nParamCount = 0;
	if ( (nParamCount = Lua_GetTopIndex(L)) < 1) return 0;
	int nNpcIndex = (int)Lua_ValueToNumber(L,1);
    int npNpcIdx =  (int)Lua_ValueToNumber(L,2);

	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC) 
		return 0;
	
	Lua_PushNumber(L, Npc[nNpcIndex].DiretorDamage(npNpcIdx));

	return 1;
}


int LuacallPlayerFunctionr(Lua_State * L)
{
	int nParamCount = 0;
	if ( (nParamCount = Lua_GetTopIndex(L)) < 2) return 0; //最少2个参数
	int _playerIndex    = (int)Lua_ValueToNumber(L,1);
	if (Lua_IsCFunction(L,2))
	{
		lua_CFunction _cfun = Lua_ValueToCFunction(L,2);
		_cfun(L);
		//lua_pushstring(L,"test hello");
		//lua_call(L, 2, 0);
		//	(lua_pushcfunction(L, f), lua_setglobal(L, n))  --- n 为lua里的函数名 f为c++中执行函数 ，注册这个字符串 就代表执行这个函数
		printf("--callPlayerFunctionr-- \n");


	}

	return 0;
}


int LuaDynamicExecuteByPlayer(Lua_State * L)
{
	int nParamCount = 0;
	if ( (nParamCount = Lua_GetTopIndex(L)) < 3) return 0; //最少三个参数

	int _playerIndex    = (int)Lua_ValueToNumber(L,1);
	char * _scriptPath = (char *)Lua_ValueToString(L,2);
	char *_callFun      = (char *)Lua_ValueToString(L,3);

    return 0;
}

int LuaKillNpcWhthIndex(Lua_State * L)
{
	int nParamCount = 0;
	if ( (nParamCount = Lua_GetTopIndex(L)) < 1) return 0;

	/*int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;*/

	int nNpcIndex  = (int)Lua_ValueToNumber(L,1);

	if (nNpcIndex <= 0 && nNpcIndex>=MAX_NPC) 
		return 0;

	if (nNpcIndex < 0 || nNpcIndex >= MAX_NPC) return 0;
	Npc[nNpcIndex].DiretorDamage(0/*Player[nPlayerIndex].m_nIndex*/);

	return 0;
}

int LuaKillNpc(Lua_State * L)
{
	int nParamCount = 0;
	if ( (nParamCount = Lua_GetTopIndex(L)) < 1) return 0;
	
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	
	DWORD nNpcID = (DWORD)Lua_ValueToNumber(L,1);

	int nNpcIndex = Player[nPlayerIndex].FindAroundNpc(nNpcID);

	if (nNpcIndex <= 0) 
		return 0;
	
	    Npc[nNpcIndex].DiretorDamage(Player[nPlayerIndex].m_nIndex);
	
	return 0;
}

int LuaKillPlayer(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) 
		return 0;
	
	Npc[Player[nPlayerIndex].m_nIndex].DiretorDamage(Player[nPlayerIndex].m_nIndex);

	return 0;
}


int LuaSetFightState(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	
	if (Player[nPlayerIndex].m_nIndex <= 0) return 0;

	Npc[Player[nPlayerIndex].m_nIndex].SetFightMode((Lua_ValueToNumber(L,1) != 0));
	return 0;
}

int LuaGetFightState(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	
	if (Player[nPlayerIndex].m_nIndex <= 0) return 0;
	Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_FightMode);
	return 1;
}

int LuaSetBayBan(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	
	if (Player[nPlayerIndex].m_nIndex <= 0) return 0;
	Npc[Player[nPlayerIndex].m_nIndex].SetBayBan(Lua_ValueToNumber(L,1) != 0);
	return 0;
}


int LuaGetMsg(Lua_State * L)
{
    int nRows=g_GetMsg.GetHeight();
	char *nstrName=(char *)Lua_ValueToString(L,1);
	char nMsg[512];
	int nVal=GetRandomNumber(2,nRows);
        g_GetMsg.GetString(nVal,nstrName,"无数据",nMsg,sizeof(nMsg));

	Lua_PushString(L, nMsg);
    return 1;
}

int LuaGetBayBan(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	
	if (Player[nPlayerIndex].m_nIndex <= 0) return 0;
	Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_BayBan);
	return 1;
}



int LuaSetLevel(Lua_State * L)
{
    int nPlayerIndex = GetPlayerIndex(L);
    if (nPlayerIndex <= 0)    return 0;
    
    if (Player[nPlayerIndex].m_nIndex <= 0) return 0;
    int nValue = (int)Lua_ValueToNumber(L, 1);

	if (nValue<1)
		return 0;

    Npc[Player[nPlayerIndex].m_nIndex].m_Level = 1;	           //等级为1级

    Lua_PushNumber(L, Player[nPlayerIndex].m_nSkillPoint);	   //返回原来的剩余的技能点
    Lua_PushNumber(L, Player[nPlayerIndex].m_nAttributePoint); //返回原来的属性点 
	
    Player[nPlayerIndex].m_nSkillPoint    = 0;//Player[nPlayerIndex].m_cReBorn.GetReBornKeepJpiont();		//技能点归零
    Player[nPlayerIndex].m_nAttributePoint= 0;//Player[nPlayerIndex].m_cReBorn.GetReBornKeepQpiont();	    //属性点归零

    if (Npc[Player[nPlayerIndex].m_nIndex].m_Series==0)
    {
        Npc[Player[nPlayerIndex].m_nIndex].m_LifeMax=204;
        Npc[Player[nPlayerIndex].m_nIndex].m_ManaMax=16;
    
    }
    if (Npc[Player[nPlayerIndex].m_nIndex].m_Series==1)
    {
        Npc[Player[nPlayerIndex].m_nIndex].m_LifeMax=103;
        Npc[Player[nPlayerIndex].m_nIndex].m_ManaMax=77;
    
    }

    if (Npc[Player[nPlayerIndex].m_nIndex].m_Series==2)
    {
        Npc[Player[nPlayerIndex].m_nIndex].m_LifeMax=153;
        Npc[Player[nPlayerIndex].m_nIndex].m_ManaMax=77;
    
    }
    if (Npc[Player[nPlayerIndex].m_nIndex].m_Series==3)
    {
        Npc[Player[nPlayerIndex].m_nIndex].m_LifeMax=213;
        Npc[Player[nPlayerIndex].m_nIndex].m_ManaMax=41;
    
    }
    if (Npc[Player[nPlayerIndex].m_nIndex].m_Series==4)
    {
        Npc[Player[nPlayerIndex].m_nIndex].m_LifeMax=76;
        Npc[Player[nPlayerIndex].m_nIndex].m_ManaMax=163;
    
    }
    Npc[Player[nPlayerIndex].m_nIndex].m_StaminaMax=100;

    for (int chay = 1; chay <= nValue-1; chay++)
	{//循环升级
           Player[nPlayerIndex].LevelUp();
	}    
    
    return 2;
}  
//
int	LuaUseTownPortal(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	
	if (Player[nPlayerIndex].m_nIndex <= 0) return 0;
	
	Player[nPlayerIndex].UseTownPortal();
	return 0;
}

int LuaReturnFromTownPortal(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	
	if (Player[nPlayerIndex].m_nIndex <= 0) return 0;
	
	Player[nPlayerIndex].BackToTownPortal();
	return 0;
}
#endif

int GetPlayerIndex(Lua_State * L)
{
	Lua_GetGlobal(L, SCRIPT_PLAYERINDEX);
    if (lua_isnil(L,Lua_GetTopIndex(L)))
        return -1;
	int nIndex = (int)Lua_ValueToNumber(L, Lua_GetTopIndex(L));
    if (nIndex >= MAX_PLAYER || nIndex <= 0) 
	{
		return -1;
	}	
	if (Player[nIndex].m_nIndex >= MAX_NPC || Player[nIndex].m_nIndex < 0)
	{
		//_ASSERT(0);
		return -1;
	}
	/*Lua_PushNumber(pScript->m_LuaState, m_nPlayerIndex);
	pScript->SetGlobalName(SCRIPT_PLAYERINDEX);

	Lua_PushNumber(pScript->m_LuaState, m_dwID);
	pScript->SetGlobalName(SCRIPT_PLAYERID);

	Lua_PushNumber(pScript->m_LuaState, Npc[m_nIndex].m_SubWorldIndex);
	pScript->SetGlobalName(SCRIPT_SUBWORLDINDEX);
	*/
	//Lua_SetGlobal(m_LuaState, szName);
	//Lua_PushNumber(L,nIndex);
	//Lua_SetGlobal(L,SCRIPT_PLAYERINDEX);
	//Lua_PushNumber(L, Player[nIndex].m_dwID);
	//Lua_SetGlobal(L, SCRIPT_PLAYERID);
	//Lua_PushNumber(L,  Npc[Player[nIndex].m_nIndex].m_SubWorldIndex);
	//Lua_SetGlobal(L, SCRIPT_SUBWORLDINDEX);

    return nIndex;
}

int GetObjIndex(Lua_State * L)
{
	Lua_GetGlobal(L, SCRIPT_OBJINDEX);
    if (lua_isnil(L,Lua_GetTopIndex(L)))
        return -1;
	int nIndex = (int)Lua_ValueToNumber(L, Lua_GetTopIndex(L));
    if (nIndex >= MAX_OBJECT || nIndex <= 0) 
	{
	//	_ASSERT(0);
		return -1;
	}
	if (Object[nIndex].m_nIndex != nIndex)
	{
	//	_ASSERT(0);
		return -1;
	}
    return nIndex;
}

int LuaMathematical(Lua_State * L)
{
	return 0;
}

int  LuaMessage(Lua_State * L)
{
	const char * szString;
	szString  = lua_tostring (L,1);
	printf("--%s-- \n",(char *)szString);
	return 0;
}
//==================================================================================
#ifdef _SERVER
//AddStation(N)  经过的地点
int LuaAddPlayerWayPoint(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	int nWayPoint = (int)Lua_ValueToNumber(L,1);
	if (Player[nPlayerIndex].m_nIndex <= 0)
		return 0;
	{
		KIndexNode * pNode = (KIndexNode*) Player[nPlayerIndex].m_PlayerWayPointList.GetHead();
		while(pNode)
		{
			if (pNode->m_nIndex == nWayPoint)
				return 0;//如果存在就返回
			pNode = (KIndexNode*)pNode->GetNext();
		}
		
		KIndexNode * pNewNode = new KIndexNode;
		pNewNode->m_nIndex = nWayPoint;
		int nCount = Player[nPlayerIndex].m_PlayerWayPointList.GetNodeCount();
		for (int i = 0; i < nCount - 2; i ++ )
		{
			KIndexNode *  pDelNode = (KIndexNode*)Player[nPlayerIndex].m_PlayerWayPointList.RemoveHead();
			delete pDelNode;
			pDelNode=NULL;
		}

		Player[nPlayerIndex].m_PlayerWayPointList.AddTail(pNewNode);
	}
	return 0;
}

int LuaAddPlayerStation(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	int nStation = (int )Lua_ValueToNumber(L,1);
	if (Player[nPlayerIndex].m_nIndex <= 0) return 0;
	{
		KIndexNode * pNode = (KIndexNode*) Player[nPlayerIndex].m_PlayerStationList.GetHead();
		while(pNode)
		{
			if (pNode->m_nIndex == nStation) return 0;
			pNode = (KIndexNode*)pNode->GetNext();
		}
		
		KIndexNode * pNewNode = new KIndexNode;
		pNewNode->m_nIndex = nStation;
		Player[nPlayerIndex].m_PlayerStationList.AddTail(pNewNode);
	}
	return 0;
}

int LuaGetPlayerStationCount(Lua_State *L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) return 0;
	Lua_PushNumber(L, Player[nPlayerIndex].m_PlayerStationList.GetNodeCount());
	return 1;
}

//获得当前玩家有效城市列表中的第n个（除去当前所在城市）
int LuaGetPlayerStation(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) return 0;
	
	if (Lua_GetTopIndex(L) < 2)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	
	int nStationId = 0;
	KIndexNode * pNode = 	(KIndexNode*)Player[nPlayerIndex].m_PlayerStationList.GetHead();
	if (pNode)
	{
		int nNo = (int )Lua_ValueToNumber(L, 1);
		int nCurStation = (int) Lua_ValueToNumber(L,2);
		int nVisitNo = 0;
		while(pNode)
		{
			if (pNode->m_nIndex != nCurStation && g_GetPriceToStation( nCurStation , pNode->m_nIndex) > 0) 
			{
				nVisitNo ++;
				if (nVisitNo == nNo)
				{
					nStationId = pNode->m_nIndex;
					break;
				}
			}
			pNode = (KIndexNode*)pNode->GetNext();
		}
	}
	
	Lua_PushNumber(L, nStationId);
	
	return 1;
}
//获取返回点的
int LuaGetPlayerWayPoint(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) return 0;
	
	KIndexNode * pNode = (KIndexNode*)	Player[nPlayerIndex].m_PlayerWayPointList.GetHead();
	if (pNode)
	{
		int nNo = (int)Lua_ValueToNumber(L, 1);
		if (nNo > TASKVALUE_MAXWAYPOINT_COUNT) 
			Lua_PushNumber(L, 0);
		else
		{
			for (int i = 0; i < nNo - 1; i ++)
			{
				if (pNode == NULL ) break;
				pNode = (KIndexNode *)pNode->GetNext();
			}
			
			if (pNode)
				Lua_PushNumber(L, pNode->m_nIndex);
			else
				Lua_PushNumber(L, 0);
			
		}
	}
	else
		Lua_PushNumber(L, 0);
	
	return 1;
}
//根据传入的城市唯一id获得其名称 //获取返回城市的名字
int LuaGetStationName(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) return 0;
	if (Lua_GetTopIndex(L) <= 0 ) 
	{
		Lua_PushString(L, "");
		return 1;
	}
	int nStationId = (int)Lua_ValueToNumber(L,1);
	char szName[50];
	g_StationTabFile.GetString(nStationId + 1, "DESC", "无名城",  szName, 50 );
	Lua_PushString(L, szName);
	return 1;
}
//获取返回点的名字
int LuaGetWayPointName(Lua_State * L)
{
	if (Lua_GetTopIndex(L) <= 0 ) 
	{
		Lua_PushString(L, "");
		return 1;
	}

	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) return 0;

	int nWayPointId = (int)Lua_ValueToNumber(L,1);
	char szName[50];
	g_WayPointTabFile.GetString(nWayPointId + 1, "DESC", "未记录",  szName, 50 );
	Lua_PushString(L, szName);
	return 1;
}
//获取返回点的名字
int LuaCheckWayPointName(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	int nReg=0;
	char *Name = (char *)Lua_ValueToString(L,1);
	char *KeyName = (char *)Lua_ValueToString(L,2);

	if  (Name==NULL || KeyName==NULL)
		return 0;

	if (strstr(Name,KeyName))
		 nReg=1;

	Lua_PushNumber(L,nReg);
	return 1;
}
//GetCityCount 
int LuaGetAllStationCount(Lua_State * L)
{
	int nCityCount = g_StationTabFile.GetHeight() - 1;
	if (nCityCount < 0) nCityCount = 0;
	Lua_PushNumber(L,nCityCount);
	return 1;
}

//cityid, price = GetCity(citynum, curcity)
int LuaGetCity(Lua_State * L)
{
	return 0;
}


int LuaGetPriceToWayPoint(Lua_State *L)
{
	
	int nCurStation = (int)Lua_ValueToNumber(L,1);
	int nDesWayPoint = (int)Lua_ValueToNumber(L,2);
	Lua_PushNumber(L, g_GetPriceToWayPoint(nCurStation, nDesWayPoint));
	return 1;
}

int LuaGetPriceToStation(Lua_State *L)
{
	int nCurStation	 = (int)Lua_ValueToNumber(L,1);
	int nNextStation = (int)Lua_ValueToNumber(L,2);
	Lua_PushNumber(L, g_GetPriceToStation(nCurStation, nNextStation));
	return 1;
}

int LuaGetStationPos(Lua_State * L)
{
	int nStationId = (int)Lua_ValueToNumber(L,1);
	char szPos[256] ;
	int nCount = 0;
	int nRow = g_StationTabFile.FindColumn("COUNT");
	g_StationTabFile.GetInteger(nStationId + 1, nRow,  0, &nCount);
	if (nCount <= 0) return 0;
	int nRandSect = g_Random(256) % nCount + 1;
	char szSectName[32];
	sprintf(szSectName, "SECT%d", nRandSect);
	char szValue[256];
	nRow = g_StationTabFile.FindColumn(szSectName);
	g_StationTabFile.GetString(nStationId + 1, nRow, "0,0,0", szValue, 256);
	
    int nX, nY, nWorld;
    const char *pcszTemp = szValue;
    
    nWorld = KSG_StringGetInt(&pcszTemp, 0);
    KSG_StringSkipSymbol(&pcszTemp, ',');
    nX = KSG_StringGetInt(&pcszTemp, 0);
    KSG_StringSkipSymbol(&pcszTemp, ',');
    nY = KSG_StringGetInt(&pcszTemp, 0);
	//sscanf(szValue, "%d,%d,%d", &nWorld, &nX, &nY);
	
    Lua_PushNumber(L,nWorld);
	Lua_PushNumber(L,nX);
	Lua_PushNumber(L,nY);
	return 3;
}

int LuaGetWayPointPos(Lua_State * L)
{
	int nWayPointId = (int)Lua_ValueToNumber(L,1);
	char szPos[256] ;
	int nCount = 0;
	char szValue[30];
	int nRow;
	nRow = g_WayPointTabFile.FindColumn("SECT");
	g_WayPointTabFile.GetString(nWayPointId + 1, nRow, "0,0,0", szValue, 30);
	int nX, nY, nWorld;
    const char *pcszTemp = szValue;
    
    nWorld = KSG_StringGetInt(&pcszTemp, 0);
    KSG_StringSkipSymbol(&pcszTemp, ',');
    nX = KSG_StringGetInt(&pcszTemp, 0);
    KSG_StringSkipSymbol(&pcszTemp, ',');
    nY = KSG_StringGetInt(&pcszTemp, 0);
	//sscanf(szValue, "%d,%d,%d", &nWorld, &nX, &nY);
	
	Lua_PushNumber(L,nWorld);
	Lua_PushNumber(L,nX);
	Lua_PushNumber(L,nY);
	return 3;
}

//设置NPC存活的时间,自动消亡

int LuaSetNpcLiveTime(Lua_State * L)
{
	int _nParmCount=Lua_GetTopIndex(L);

   int nNpcIdx = (int)Lua_ValueToNumber(L,1);
   int nTime = (int)Lua_ValueToNumber(L,2);
   int nKind = (int)Lua_ValueToNumber(L,3);
   int nMapIdx =0;
          nMapIdx = (int)Lua_ValueToNumber(L,4);
   if (nNpcIdx<=0 || nNpcIdx >= MAX_NPC) 
	   return 0;

   int _delayDeath =0;

	if (_nParmCount>=5)
	    _delayDeath = (int)Lua_ValueToNumber(L,5);

       Npc[nNpcIdx].m_GameliveTime = nTime;
	   Npc[nNpcIdx].m_liveType = nKind;
	   Npc[nNpcIdx]._delayReMove = _delayDeath;//延迟删除

	   if (Npc[nNpcIdx].IsPlayer())
	   {//人物
		   if  (nMapIdx<=0)
			   nMapIdx = SubWorld[Npc[nNpcIdx].m_SubWorldIndex].m_SubWorldID;

		   Player[Npc[nNpcIdx].GetPlayerIdx()].m_cTask.SetSaveVal(25,nMapIdx);
	   }
	   else
	   {//怪物
		  Npc[nNpcIdx].SetNpcTimeInfo();
	   }
   return 0;
}




int LuaSetNpcliveType(Lua_State * L)
{
   int nNpcIdx = (int)Lua_ValueToNumber(L,1);
   int nTime = (int)Lua_ValueToNumber(L,2);

   if (nNpcIdx<=0 || nNpcIdx >= MAX_NPC) 
	   return 0;

   Npc[nNpcIdx].m_liveType = nTime;

   return 0;
}

int LuaGetNpcLiveTime(Lua_State * L)
{
	int nNpcIdx = (int)Lua_ValueToNumber(L,1);
	if (nNpcIdx<=0 || nNpcIdx >= MAX_NPC) return 0;

	if (Npc[nNpcIdx].IsPlayer())
	{
		Lua_PushNumber(L,Npc[nNpcIdx].m_GameliveTime);
		//Lua_PushNumber(L,Npc[nNpcIdx].m_liveType);
		Lua_PushNumber(L,Player[Npc[nNpcIdx].GetPlayerIdx()].m_cTask.GetSaveVal(25));
	}
	else
	{
		Lua_PushNumber(L,Npc[nNpcIdx].m_GameliveTime);
		Lua_PushNumber(L,Npc[nNpcIdx].m_liveType);
	}

	return 2;
}



int LuaSetNpcReciveNum(Lua_State * L)
{
	int nNpcIdx = (int)Lua_ValueToNumber(L,1);
	int nNum = (int)Lua_ValueToNumber(L,2);
	if (nNpcIdx<=0 || nNpcIdx >= MAX_NPC) return 0;
	   Npc[nNpcIdx].m_ReviceNum=nNum;

	if (Npc[nNpcIdx].m_ReviceNum<0)
		Npc[nNpcIdx].m_ReviceNum=0;

	return 0;
}

int LuaAddNpcReciveNum(Lua_State * L)
{
	int nNpcIdx = (int)Lua_ValueToNumber(L,1);
	int nNum = (int)Lua_ValueToNumber(L,2);
	if (nNpcIdx<=0 || nNpcIdx >= MAX_NPC) return 0;
	Npc[nNpcIdx].m_ReviceNum +=nNum;

	if (Npc[nNpcIdx].m_ReviceNum<0)
		Npc[nNpcIdx].m_ReviceNum=0;

	return 0;
}


int LuaGetNpcReciveNum(Lua_State * L)
 {
	int nNpcIdx = (int)Lua_ValueToNumber(L,1);
	if (nNpcIdx<=0 || nNpcIdx >= MAX_NPC) return 0;
	Lua_PushNumber(L,Npc[nNpcIdx].m_ReviceNum);
	return 1;
 }

int LuaAddNpcLiveTime(Lua_State * L)
{
	int nNpcIdx = (int)Lua_ValueToNumber(L,1);
	int nTime = (int)Lua_ValueToNumber(L,2);
	if (nNpcIdx<=0 || nNpcIdx >= MAX_NPC) return 0;

	Npc[nNpcIdx].m_GameliveTime += nTime;

	if (!Npc[nNpcIdx].IsPlayer())
		 Npc[nNpcIdx].SetNpcTimeInfo();

	return 0;
}

int LuaGetRank(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) return 0;
	Lua_PushNumber(L,Npc[Player[nPlayerIndex].m_nIndex].m_btRankId);
	return 1;
}
//int LuaGetNpcMaptimeInfo(Lua_State * L)
int LuaGetMapTimeInfo(Lua_State* L) //add by tuanln chua hoan thanh fix loi script/system/online.lua
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) return 0;
	Lua_PushNumber(L, 0);
	Lua_PushNumber(L, 0);
	Lua_PushNumber(L, 0);
	Lua_PushNumber(L, 0);
	Lua_PushNumber(L, 0);
	Lua_PushNumber(L, 0);
	Lua_PushNumber(L, 0);
	return 1;
}

int LuaGetNpcTitle(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) return 0;
	Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_NpcTitle);
	Lua_PushNumber(L, Player[nPlayerIndex].m_nPlayerTime);

	return 2;
}


int LuaCastNpcTitleSkill(Lua_State * L)
{
	int nTime		 = (int)Lua_ValueToNumber(L,1);
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	int nSkillD,nSkillevel;

	int bNpcTitleIdx=Npc[Player[nPlayerIndex].m_nIndex].m_NpcTitle;

	if (bNpcTitleIdx<=0) return 0;

	for (int i=0;i<5;++i)
	{
		char nKeyVal[48];
		sprintf(nKeyVal,"ExtSkill%d",i+1);
		g_PlayerTitle.GetInteger(bNpcTitleIdx+1,nKeyVal,0,&nSkillD);
		snprintf(nKeyVal, 48, "ExtSkillLevel%d",i+1);
		g_PlayerTitle.GetInteger(bNpcTitleIdx+1,nKeyVal,0,&nSkillevel);
		
		if ((nSkillD<=0 && nSkillevel<=0) || nSkillevel>=MAX_SKILLLEVEL)
		{
			Npc[Player[nPlayerIndex].m_nIndex].m_ExtSkill[i]=0;
			continue;
		}
		
		//Npc[Player[nPlayerIndex].m_nIndex].m_ExtSkill[i]=nSkillD;

		Player[nPlayerIndex].UseToolSkills(nSkillD,nSkillevel,Player[nPlayerIndex].m_nIndex,Player[nPlayerIndex].m_nIndex,NULL,1,nTime);
	}
   return 0;
}


int LuaSetNpcCurTitle(Lua_State * L)
{
	int btRank = (int)Lua_ValueToNumber(L,1);
	int nTime = (int)Lua_ValueToNumber(L,2);
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) return 0;
	int i,nSkillD,nSkillevel;
	
	for (i=0;i<5;++i)
	{
		nSkillD=Npc[Player[nPlayerIndex].m_nIndex].m_ExtSkill[i];
		if (nSkillD>0)
		{//有技能ID,需要先去除状态
			Npc[Player[nPlayerIndex].m_nIndex].m_ExtSkill[i]=0;
			Npc[Player[nPlayerIndex].m_nIndex].SetSkillLeftTime(nSkillD,0); //同步客户端删除状态			
		}
	}
	
	Npc[Player[nPlayerIndex].m_nIndex].m_CurNpcTitle = btRank;
	Player[nPlayerIndex].m_nPlayerTime               = nTime;

	nSkillD=0;nSkillevel=0;
	
	for (i=0;i<5;++i)
	{
		if (btRank<=0)
			break;
		char nKeyVal[48]={0};
		sprintf(nKeyVal,"ExtSkill%d",i+1);
		g_PlayerTitle.GetInteger(btRank+1,nKeyVal,0,&nSkillD);
		snprintf(nKeyVal, 48, "ExtSkillLevel%d",i+1);
		g_PlayerTitle.GetInteger(btRank+1,nKeyVal,0,&nSkillevel);
		
		if ((nSkillD<=0 && nSkillevel<=0) || nSkillevel>=MAX_SKILLLEVEL)
		{
			Npc[Player[nPlayerIndex].m_nIndex].m_ExtSkill[i]=0;
			continue;
		}
		
		Npc[Player[nPlayerIndex].m_nIndex].m_ExtSkill[i]=nSkillD;
		
		
		Player[nPlayerIndex].UseToolSkills(nSkillD,nSkillevel,Player[nPlayerIndex].m_nIndex,Player[nPlayerIndex].m_nIndex,NULL,1,nTime);
	}
	
	return 0;
}


int LuaResSetNpcTitle(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);

	if (Player[nPlayerIndex].m_nIndex <= 0) return 0;

	Player[nPlayerIndex].ResSetNpcTitle();
	return 0;
}

int LuaSetNpcTitle(Lua_State * L)
{
	int btRank = (int)Lua_ValueToNumber(L,1);
	int nTime = (int)Lua_ValueToNumber(L,2);
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) return 0;
	int i,nSkillD,nSkillevel;

	for (i=0;i<5;++i)
	{
	   nSkillD=Npc[Player[nPlayerIndex].m_nIndex].m_ExtSkill[i];
	   if (nSkillD>0)
	   {//有技能ID,需要先去除状态
		   //KSkill * inSkill = (KSkill *) g_SkillManager.GetSkill(nSkillD, 1);
		   //if (!inSkill) 
		   //   return FALSE;
		   //inSkill->Cast(nLauncher, nParam1, nParam2, nWaitTime, eLauncherType,nMaxShangHai);
		   Npc[Player[nPlayerIndex].m_nIndex].m_ExtSkill[i]=0;
		   Npc[Player[nPlayerIndex].m_nIndex].SetSkillLeftTime(nSkillD,0); //同步客户端删除状态
		   //Npc[Player[nPlayerIndex].m_nIndex].ClearOneSkillState(nSkillD);

	   }
	}

	Npc[Player[nPlayerIndex].m_nIndex].m_NpcTitle = btRank;
	Player[nPlayerIndex].m_nPlayerTime            = nTime;

	nSkillD=0;nSkillevel=0;

	for (i=0;i<5;++i)
	{
		if  (btRank<=0)
			break;

		char nKeyVal[48]={0};
		sprintf(nKeyVal,"ExtSkill%d",i+1);
		g_PlayerTitle.GetInteger(btRank+1,nKeyVal,0,&nSkillD);
		snprintf(nKeyVal, 48, "ExtSkillLevel%d",i+1);
		g_PlayerTitle.GetInteger(btRank+1,nKeyVal,0,&nSkillevel);

		if ((nSkillD<=0 && nSkillevel<=0) || nSkillevel>=MAX_SKILLLEVEL)
		{
			Npc[Player[nPlayerIndex].m_nIndex].m_ExtSkill[i]=0;
			continue;
		}

		Npc[Player[nPlayerIndex].m_nIndex].m_ExtSkill[i]=nSkillD;


		Player[nPlayerIndex].UseToolSkills(nSkillD,nSkillevel,Player[nPlayerIndex].m_nIndex,Player[nPlayerIndex].m_nIndex,NULL,1,nTime);
	}

	return 0;
}

int LuaKSG_GetCurSec(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) return 0;
	Lua_PushNumber(L, KSG_GetCurSec());
	return 1;
}

int LuaKSG_GetUyThacHour(lua_State* L)
{
    if (lua_gettop(L) < 6) {
        Lua_PushNumber(L, 0);
        return 1;
    }

    int nDay   = (int)Lua_ValueToNumber(L, 1);
    int nMonth = (int)Lua_ValueToNumber(L, 2);
    int nYear  = (int)Lua_ValueToNumber(L, 3);
    int nHour  = (int)Lua_ValueToNumber(L, 4);
    int nMin   = (int)Lua_ValueToNumber(L, 5);
    int nSec   = (int)Lua_ValueToNumber(L, 6);

    struct tm tStart = {};
    tStart.tm_year = nYear - 1900;
    tStart.tm_mon  = nMonth - 1;
    tStart.tm_mday = nDay;
    tStart.tm_hour = nHour;
    tStart.tm_min  = nMin;
    tStart.tm_sec  = nSec;

    time_t timeStart = mktime(&tStart);
    if (timeStart == -1) {
        Lua_PushNumber(L, 0);
        return 1;
    }

    // Lay thoi gian he thong hien tai
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    time_t timeNow = mktime(timeinfo);
    if (timeNow == -1) {
        Lua_PushNumber(L, 0);
        return 1;
    }


    // Tinh so giay chenh lech
    int diffSec = static_cast<int>(timeNow - timeStart);
    if (diffSec < 0 || diffSec > 86400 * 30) {
        Lua_PushNumber(L, 0);
        return 1;
    }

    // Lam tron len theo gio
    int diffHour = (diffSec + 3599) / 3600;
    if (diffHour > 8) diffHour = 8;

    Lua_PushNumber(L, diffHour);
    return 1;
}





int LuaGetPlayerTitle(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) return 0;
	Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_NpcTitle);
	Lua_PushNumber(L, Player[nPlayerIndex].m_nPlayerTitleTime);
	Lua_PushNumber(L, KSG_GetCurSec());

	return 3;
}

int LuaSetPlayerTitle(Lua_State * L)
{
	int btRank = (int)Lua_ValueToNumber(L,1);
	int nTime = (int)Lua_ValueToNumber(L,2);
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) return 0;
	int i,nSkillD,nSkillevel;

	for (i=0;i<5;++i)
	{
	   nSkillD=Npc[Player[nPlayerIndex].m_nIndex].m_ExtSkill[i];
	   if (nSkillD>0)
	   {
		   Npc[Player[nPlayerIndex].m_nIndex].m_ExtSkill[i]=0;
		   Npc[Player[nPlayerIndex].m_nIndex].SetSkillLeftTime(nSkillD,0); //同步客户端删除状态
	   }
	}

			time_t curTime;
			time(&curTime);  //(timestamp)
			// nLock (nLock h, conver giay)
			curTime += static_cast<time_t>(nTime) * 3600;
			// Chuyen lai thanh YYYYMMDDHH
			struct tm newTimeInfo;
			localtime_s(&newTimeInfo, &curTime);  // Windows (VS Compiler)
			// YYYYMMDDHH
			int32_t newTimeCode = (newTimeInfo.tm_year + 1900) * 1000000 +
								  (newTimeInfo.tm_mon + 1) * 10000 +
								  newTimeInfo.tm_mday * 100 +
								  newTimeInfo.tm_hour;

	Npc[Player[nPlayerIndex].m_nIndex].m_NpcTitle = btRank;
	Player[nPlayerIndex].m_nPlayerTitleTime        = newTimeCode;
	nSkillD=0;nSkillevel=0;

	for (i=0;i<5;++i)
	{
		if  (btRank<=0)
			break;

		char nKeyVal[48]={0};
		sprintf(nKeyVal,"ExtSkill%d",i+1);
		g_PlayerTitle.GetInteger(btRank+1,nKeyVal,0,&nSkillD);

		snprintf(nKeyVal, 48, "ExtSkillLevel%d",i+1);
		g_PlayerTitle.GetInteger(btRank+1,nKeyVal,0,&nSkillevel);

		if ((nSkillD<=0 && nSkillevel<=0) || nSkillevel>=MAX_SKILLLEVEL)
		{
			Npc[Player[nPlayerIndex].m_nIndex].m_ExtSkill[i]=0;
			continue;
		}

		Npc[Player[nPlayerIndex].m_nIndex].m_ExtSkill[i]=nSkillD;
		int doitime = Player[nPlayerIndex].TimeToHour(newTimeCode, KSG_GetCurSec());
		Player[nPlayerIndex].UseToolSkills(nSkillD,nSkillevel,Player[nPlayerIndex].m_nIndex,Player[nPlayerIndex].m_nIndex,NULL,1, doitime * 64800);
	}

	return 0;
}

int LuaSetPlayerTitleSkills(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);

	if (Player[nPlayerIndex].m_nIndex <= 0) return 0;

	Player[nPlayerIndex].SetPlayerTitleSkills();
	return 0;
}

int LuaSetRank(Lua_State * L)
{
	int btRank = (int)Lua_ValueToNumber(L,1);
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) return 0;
	Npc[Player[nPlayerIndex].m_nIndex].m_btRankId = btRank;
	return 0;
}

int LuaGetRankFF(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) return 0;
	Lua_PushNumber(L,Npc[Player[nPlayerIndex].m_nIndex].m_btRankFFId);
	return 1;
}


int LuaSetNpcCurPathPos(Lua_State * L)
{
   int nNpcIdx     = (int)Lua_ValueToNumber(L,1);
   int nRankFFIn = (int)Lua_ValueToNumber(L,2);
   if (nRankFFIn < 0 || nRankFFIn > 255) return 0;
   BYTE btRankFF   = (BYTE)nRankFFIn;
   int nCurPathPos = (int)Lua_ValueToNumber(L,3);
   int nCurPathType = (int)Lua_ValueToNumber(L,4);
   int nCurDist = (int)Lua_ValueToNumber(L,5);
   if (nNpcIdx <= 0 || nNpcIdx >= MAX_NPC) return 0;
   Npc[nNpcIdx].m_btRankFFId = btRankFF; 
   Npc[nNpcIdx].m_AutoFindPath.nCurPathPos=nCurPathPos;
   Npc[nNpcIdx].m_AutoFindPath.nCurType=nCurPathType;
   Npc[nNpcIdx].m_AutoFindPath.nCurDisten=nCurDist;

   return 0;
}

int LuaSetNpcIsDel(Lua_State * L)
{
   int nNpcIdx     = (int)Lua_ValueToNumber(L,1);
   int nIsDelIn = (int)Lua_ValueToNumber(L,2);
   if (nIsDelIn < 0 || nIsDelIn > 255) return 0;
   BYTE btIsDel   = (BYTE)nIsDelIn;
   if (nNpcIdx <= 0 || nNpcIdx >= MAX_NPC) return 0;
   Npc[nNpcIdx].m_IsDel = btIsDel; 
   return 0;
}





int LuaSynNpcCur64Dir(Lua_State * L)
{
   int nNpcIdx     = (int)Lua_ValueToNumber(L,1);
   int nCurDir = (int)Lua_ValueToNumber(L,2);
   int nIsSynDirIn = (int)Lua_ValueToNumber(L,3);
   if (nIsSynDirIn < 0 || nIsSynDirIn > 255) return 0;
   BYTE nIsSynDir= (BYTE)nIsSynDirIn;
   if (nNpcIdx <= 0 || nNpcIdx >= MAX_NPC) return 0;
    
	if (nIsSynDir>=1)
	{
        Npc[nNpcIdx].m_IsSynDir=nIsSynDir;
        Npc[nNpcIdx].m_Dir = nCurDir; 
	}
/*    
   	NPC_DIR_SET	sTnpcDir;
	sTnpcDir.ProtocolType = s2c_synnpcdir;  
	sTnpcDir.m_CurDir  = nCurDir;

	if (g_pServer)
		g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, (BYTE*)&sTnpcDir, sizeof(PLAYER_JINMAI_GETBACK));
      
	PLAYER_JINMAI_GETBACK	sTaskBack;
	sTaskBack.ProtocolType = s2c_JinMaicallback;  
	//sTaskBack.m_TaskId  = pTask->m_TaskId;
	sTaskBack.m_TaskVala = Player[nPlayerIndex].m_cTask.GetSaveVal(3501);
	sTaskBack.m_TaskValb = Player[nPlayerIndex].m_cTask.GetSaveVal(3502);
	sTaskBack.m_TaskValc = Player[nPlayerIndex].m_cTask.GetSaveVal(3503);
	sTaskBack.m_TaskVald = Player[nPlayerIndex].m_cTask.GetSaveVal(3504);
	sTaskBack.m_TaskVale = Player[nPlayerIndex].m_cTask.GetSaveVal(3505);
	sTaskBack.m_TaskValf = Player[nPlayerIndex].m_cTask.GetSaveVal(3506);
	sTaskBack.m_TaskValg = Player[nPlayerIndex].m_cTask.GetSaveVal(3507);
	sTaskBack.m_TaskValh = Player[nPlayerIndex].m_cTask.GetSaveVal(3508);

	if (g_pServer)
		g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, (BYTE*)&sTaskBack, sizeof(PLAYER_JINMAI_GETBACK));
      

*/

   return 0;
}


int LuaSetRankFF(Lua_State * L)
{
	int nRankFFRaw = (int)Lua_ValueToNumber(L,1);
	if (nRankFFRaw < 0 || nRankFFRaw > 255) return 0;
	BYTE btRankFF = (BYTE)nRankFFRaw;
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) return 0;
	Npc[Player[nPlayerIndex].m_nIndex].m_btRankFFId = btRankFF;
	return 0;
}

int LuaGetPlayFF(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) return 0;
	Lua_PushNumber(L,Npc[Player[nPlayerIndex].m_nIndex].m_AutoplayId);
	return 1;
}


int LuaSetPlayFF(Lua_State * L)
{
	int nPlayFFRaw = (int)Lua_ValueToNumber(L,1);
	if (nPlayFFRaw < 0 || nPlayFFRaw > 255) return 0;
	BYTE btPlayFF = (BYTE)nPlayFFRaw;
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) return 0;
	Npc[Player[nPlayerIndex].m_nIndex].m_AutoplayId = btPlayFF;
	return 0;
}

int LuaGetExItem(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) return 0;
	Lua_PushNumber(L,Npc[Player[nPlayerIndex].m_nIndex].m_ExItemId);
	return 1;
}

int LuaSetExItem(Lua_State * L)
{
	int nExItemRaw = (int)Lua_ValueToNumber(L,1);
	if (nExItemRaw < 0 || nExItemRaw > 255) return 0;
	BYTE ExItem = (BYTE)nExItemRaw;
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) return 0;
	Npc[Player[nPlayerIndex].m_nIndex].m_ExItemId = ExItem;
	return 0;
}

int LuaGetExBox(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) return 0;
	Lua_PushNumber(L,Npc[Player[nPlayerIndex].m_nIndex].m_ExBoxId);
	return 1;
}

int LuaSetExBox(Lua_State * L)
{
	int nExBoxRaw = (int)Lua_ValueToNumber(L,1);
	if (nExBoxRaw < 0 || nExBoxRaw > 255) return 0;
	BYTE ExBox = (BYTE)nExBoxRaw;
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) return 0;
	Npc[Player[nPlayerIndex].m_nIndex].m_ExBoxId = ExBox;
	return 0;
}

int LuaSetPKState(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
			int nType = (int)Lua_ValueToNumber(L,1);
			switch((int)Lua_ValueToNumber(L,1))
			{
			case 0:
				Player[nPlayerIndex].m_cPK.SetNormalPKState(0);break;
			case 1:
				Player[nPlayerIndex].m_cPK.SetNormalPKState(1); break;
			case 2:
				Player[nPlayerIndex].m_cPK.SetNormalPKState(2);break;
			default:
				break;
			}
	} 

	return 0;
}


int LuaSetObjPropState(Lua_State *L)
{
	int  nParamNum = ( int ) Lua_GetTopIndex(L);
	int nState  = 1;
	
	if (nParamNum >= 1)
	{
		nState = (int)Lua_ValueToNumber(L,1);
		nState = (nState == 0)?0 : 1;
	}
	
	int nIndex = 0;
	if ((nIndex = GetObjIndex(L)) < 0) 
		return 0;
	
	Object[nIndex].SetState(nState);
	return 0;
}


int	LuaGetServerName(Lua_State * L)
{
	char szServerName[256]  ;
	unsigned long   stServerNameLen = 256;

#ifndef __linux
	if (GetComputerName(szServerName, &stServerNameLen))
	{
		Lua_PushString(L, szServerName);
	}
	else
#else
	if (SOCKET_ERROR != gethostname(szServerName, sizeof(szServerName)))
	{
		Lua_PushString(L, szServerName);
	}
	else
#endif
	Lua_PushString(L, "");

	return 1;
}

//GetWharfCount(nDock)
int LuaGetDockCount(Lua_State * L)
{
	int nCount = 0;
	int nCurStation = 0;
	int nTotalCount = 0;
	int i  = 0;
	if (Lua_GetTopIndex(L) < 1 )  
	{
		goto DockCount;
	}
	
	nCurStation = (int)Lua_ValueToNumber(L,1);
	nTotalCount = g_DockPriceTabFile.GetHeight() - 1;
	
	for (i = 0; i < nTotalCount; i ++)
	{
		int nPrice = g_GetPriceToDock(nCurStation, i + 1);
		if (nPrice > 0) nCount ++;
	}
	
DockCount:
	Lua_PushNumber(L, nCount);
	return 1;
}

int LuaGetDockPrice(Lua_State * L)
{
	int nCurDock = (int)Lua_ValueToNumber(L,1);
	int nDesDock = (int)Lua_ValueToNumber(L,2);
	Lua_PushNumber(L, g_GetPriceToDock(nCurDock, nDesDock));
	return 1;
}

int LuaGetDock(Lua_State * L)
{
	int nCurDock = (int)Lua_ValueToNumber(L, 1);
	int nDock = (int)Lua_ValueToNumber(L, 2);
	int nCount = 0;
	int nTotalCount = g_DockPriceTabFile.GetHeight() - 1;
	int nGetDock = 0;
	
	for (int i = 0; i < nTotalCount; i ++)
	{
		int nPrice = g_GetPriceToDock(nCurDock, i + 1);
		if (nPrice > 0) 
		{
			nCount ++ ;
			if (nCount == nDock)
			{
				nGetDock = i + 1;
				break;
			}
		}
	}
	Lua_PushNumber(L, nGetDock);
	return 1;
}

int LuaGetDockName(Lua_State * L)
{
	int nDock  = (int)Lua_ValueToNumber(L, 1);
	char szName[256] ;
	
	if (nDock > g_DockPriceTabFile.GetHeight() - 1)
	{
		snprintf(szName, sizeof(szName), "%s", "未知码头");
		goto DockName;
	}
	
	g_DockTabFile.GetString(nDock + 1, "DESC", "未知码头", szName, 256);
	
DockName:
	Lua_PushString (L, szName);
	return 1;
}

int LuaGetDockPos(Lua_State * L)
{
	int nDock  = (int)Lua_ValueToNumber(L, 1);
	if (nDock > g_DockTabFile.GetHeight() - 1) 
	{
		printf("GetWharfPos Script Is Error!");
		return 0;
	}
	
	char szPos[256] ;
	int nCount = 0;
	int nRow = g_DockTabFile.FindColumn("COUNT");
	g_DockTabFile.GetInteger(nDock + 1, nRow,  0, &nCount);
	if (nCount <= 0) return 0;
	int nRandSect = g_Random(256) % nCount + 1;
	char szSectName[32];
	sprintf(szSectName, "SECT%d", nRandSect);
	char szValue[256];
	nRow = g_DockTabFile.FindColumn(szSectName);
	g_DockTabFile.GetString(nDock + 1, nRow, "0,0,0", szValue, 256);
	int nX, nY, nWorld;
    const char *pcszTemp = szValue;
    
    nWorld = KSG_StringGetInt(&pcszTemp, 0);
    KSG_StringSkipSymbol(&pcszTemp, ',');
    nX = KSG_StringGetInt(&pcszTemp, 0);
    KSG_StringSkipSymbol(&pcszTemp, ',');
    nY = KSG_StringGetInt(&pcszTemp, 0);
	//sscanf(szValue, "%d,%d,%d", &nWorld, &nX, &nY);
	
	Lua_PushNumber(L,nWorld);
	Lua_PushNumber(L,nX);
	Lua_PushNumber(L,nY);
	return 3;
}

int LuaGetWayPointFightState(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) return 0;
	int nFightState = 0;
	if (Lua_GetTopIndex(L) > 0 ) 
	{
		int nWayPointId = (int)Lua_ValueToNumber(L,1);
		g_WayPointTabFile.GetInteger(nWayPointId + 1, "FightState", 0,  &nFightState);
	}
	Lua_PushNumber(L, nFightState);
	return 1;
}





// SetMissionValue(valueid, value)
int LuaSetMissionValueS(Lua_State * L)
{
	int nSubWorldIndex = GetSubWorldIndex(L);//地图编号
	if (nSubWorldIndex < 0) 
		return 0;
	
	int nParamCount = Lua_GetTopIndex(L);
	if (nParamCount < 2) 
		return 0;
	
	int nValueId = (int)Lua_ValueToNumber(L, 1);
	char * nValue = (char *)Lua_ValueToString(L, 2);
	
	if (nValueId  < 0)
		return 0;

	SubWorld[nSubWorldIndex].m_MissionArray.SetMissionValueS(nValueId, nValue);
	return 0;
}

int LuaGetMissionValueS(Lua_State * L)
{
	int nSubWorldIndex = -1;
	int nParamCount = Lua_GetTopIndex(L);
	if (nParamCount < 1) 
        return 0;
	
	nSubWorldIndex = GetSubWorldIndex(L);
	
	if (nSubWorldIndex >= 0) 
	{
		int  nValueId = (int)Lua_ValueToNumber(L, 1);
		if (nValueId > 0)
	       Lua_PushString(L, SubWorld[nSubWorldIndex].m_MissionArray.GetMissionValueS(nValueId));
		else
		   return 0;
	}

	return 1;
}




// SetMissionValue(valueid, value)
int LuaSetMissionValue(Lua_State * L)
{
	int nSubWorldIndex = GetSubWorldIndex(L);//地图编号
	if (nSubWorldIndex < 0) 
		return 0;
	
	int nParamCount = Lua_GetTopIndex(L);
	if (nParamCount < 2) 
		return 0;
	
	int nValueId = (int)Lua_ValueToNumber(L, 1);
	int nValue = (int)Lua_ValueToNumber(L, 2);
	
	if (nValueId  < 0)
		return 0;
	SubWorld[nSubWorldIndex].m_MissionArray.SetMissionValue(nValueId, nValue);
	return 0;
}

int LuaGetMissionValue(Lua_State * L)
{
	int nResultValue = 0;
	int nSubWorldIndex = -1;
	int nParamCount = Lua_GetTopIndex(L);
	if (nParamCount < 1) 
		goto lab_getmissionvalue;
	
	nSubWorldIndex = GetSubWorldIndex(L);
	
	if (nSubWorldIndex >= 0) 
	{
		int  nValueId = (int)Lua_ValueToNumber(L, 1);
		if (nValueId > 0)
			nResultValue = SubWorld[nSubWorldIndex].m_MissionArray.GetMissionValue(nValueId);
	}
	
lab_getmissionvalue:
	Lua_PushNumber(L, nResultValue);
	return 1;
}


// SetMissionValue(mapid/mapname, valueid, value)
int LuaSetGlobalMissionValue(Lua_State * L)
{
	int nParamCount = Lua_GetTopIndex(L);
	if (nParamCount < 2) 
		return 0;
	
	int nValueId = (int)Lua_ValueToNumber(L, 1);
	int nValue = (int)Lua_ValueToNumber(L, 2);
	
	if (nValueId  < 0)
		return 0;
	g_GlobalMissionArray.SetMissionValue(nValueId, nValue);
	return 0;
}

int LuaGetGlobalMissionValue(Lua_State * L)
{
	int nResultValue = 0;
	int nValueId = 0;
	int nParamCount = Lua_GetTopIndex(L);
	if (nParamCount < 1) 
		goto lab_getglobalmissionvalue;
	nValueId = (int)Lua_ValueToNumber(L, 1);
	if (nValueId < 0)
		goto lab_getglobalmissionvalue;
	
	nResultValue = g_GlobalMissionArray.GetMissionValue(nValueId);
	
lab_getglobalmissionvalue:
	Lua_PushNumber(L, nResultValue);
	return 1;
}

int LuaGetMissionName(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 1) 
		return 0;
	
	int nMissionId = (int)Lua_ValueToNumber(L, 1);
	if (nMissionId < 0 )
		return 0;
	
	int nSubWorldIndex = GetSubWorldIndex(L);
	if (nSubWorldIndex < 0) 
		return 0;
	
	KMission Mission;
	Mission.SetMissionId(nMissionId);
	KMission * pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
	
	if (pMission)
	{
		Lua_PushString(L, pMission->GetMissionName());
		return 1;
	}
	return 0;
}


int LuaSetMissionGroup(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	
	if (Lua_GetTopIndex(L) < 2)
		return 0;
	
	Npc[Player[nPlayerIndex].m_nIndex].m_nMissionGroup = (int)Lua_ValueToNumber(L,1);
	Player[nPlayerIndex].SendMSGroup();
	return 0;
}

int LuaIsMission(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 1) 
		return 0;
	
	int nMissionId = (int)Lua_ValueToNumber(L, 1);
	if (nMissionId < 0 )
		return 0;
	
	int nSubWorldIndex = GetSubWorldIndex(L);
	if (nSubWorldIndex < 0) 
		return 0;

	KMission Mission;
	Mission.SetMissionId(nMissionId);
	KMission * pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
	if (pMission)
		Lua_PushNumber(L, 1);
	else
		Lua_PushNumber(L, 0);
	return 1;
}


int LuaGetMSLadder(Lua_State * L)
{
	int nParamCount = Lua_GetTopIndex(L);
	if (nParamCount < 2) 
		return 0;
	
	int nMissionId = (int)Lua_ValueToNumber(L, 1);
	if (nMissionId < 0 )
		return 0;
	
	int nSubWorldIndex = GetSubWorldIndex(L);
	if (nSubWorldIndex < 0) 
		return 0;
	KMission Mission;
	Mission.SetMissionId(nMissionId);
	KMission * pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
	if (pMission)
	{
		int nOrdinal = (int)Lua_ValueToNumber(L,2);
		if(nOrdinal >= 0 && nOrdinal < MISSION_STATNUM)
		{
			Lua_PushString(L, pMission->m_MissionLadder[nOrdinal].Name);
			Lua_PushNumber(L, pMission->m_MissionLadder[nOrdinal].ucGroup);
			Lua_PushNumber(L, pMission->m_MissionLadder[nOrdinal].nParam[pMission->GetMissionLadderParam()]);
			return 3;
		}
		
	}
	return 0;
}


//StartMission(missionid)
int LuaInitMission(Lua_State * L)
{//不同地图 可以有相同的missid	,同个地图 可以存在多个missid
	if (Lua_GetTopIndex(L) < 1) 
		return 0;
	
	int nMissionId = (int)Lua_ValueToNumber(L, 1);
	if (nMissionId < 0 )
		return 0;
	
	int nSubWorldIndex = GetSubWorldIndex(L);
	if (nSubWorldIndex < 0) 
		return 0;
	
	KMission Mission;
	Mission.SetMissionId(nMissionId);//新实例化一个miss;
	KMission * pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);

	if (pMission)
	{//如果存在这个MISSid 就返回
		return 0;
	}
	
	pMission = SubWorld[nSubWorldIndex].m_MissionArray.Add();
	if (pMission)
	{
		pMission->m_MissionPlayer.Clear();
		pMission->SetMissionId(nMissionId);
		//--------------------------------
		if(Lua_GetTopIndex(L) > 3)
		{// 4个	参数以上
			int nParam[MAX_GLBMISSION_PARAM];
			int i;
			for(i = 0; i < MAX_GLBMISSION_PARAM; i++)
				nParam[i] = (int)Lua_ValueToNumber(L, 3+i);

			pMission->SetMissionLadder((char*)Lua_ValueToString(L,2),(int)Lua_ValueToNumber(L, 3+i), nParam);
		}
		//--------------------------------
		char szScript[MAX_PATH]={0};
		char nMissPathIdx[64]={0};
		//sprintf(nMissPathIdx,"%d_map",SubWorld[nSubWorldIndex].m_SubWorldID);
		sprintf(nMissPathIdx, "%d", nMissionId);
		g_MissionTabFile.GetString(nMissPathIdx, 2, "", szScript, MAX_PATH);  //nMissionId + 1
		if (szScript[0])
			pMission->ExecuteScript(szScript,g_FileName2Id(szScript), "InitMission",0);
		else
			printf("SYS mission ID: %d : no script: %s\n", nMissionId,szScript);

	}
	
	return 0;
}

int LuaJoinMission(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 2)
		return 0;

	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex == -1)
		return 0;

	int nSubWorldIndex = GetSubWorldIndex(L);
	if (nSubWorldIndex < 0 || nSubWorldIndex >= MAX_SUBWORLD)
		return 0;

	int nMissionId = (int)Lua_ValueToNumber(L, 1);
	if (nMissionId < 0)
		return 0;

	int nCampId = (int)Lua_ValueToNumber(L, 2);
	if (nCampId < 0)
		return 0;

	KMission Mission;
	Mission.SetMissionId(nMissionId);
	KMission* pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
	if (pMission)
	{
		char nMissPathIdx[64];
		sprintf(nMissPathIdx, "%d", nMissionId);//"%d_map",SubWorld[nSubWorldIndex].m_SubWorldID);

		char szScript[MAX_PATH] = { 0 };
		g_MissionTabFile.GetString(nMissPathIdx, 2, "", szScript, MAX_PATH);  //nMissionId + 1		
		if (szScript[0])
			pMission->JoinMission(g_FileName2Id(szScript), nPlayerIndex, nCampId);
		else
			printf("mission %d : no script\n", nMissionId);
	}
	else
	{
		printf("mission %d : no found\n", nMissionId);
	}
	return 0;
}

int LuaRunMission(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 1) 
		return 0;
	
	int nMissionId = (int)Lua_ValueToNumber(L, 1);
	if (nMissionId < 0 )
		return 0;
	
	int nSubWorldIndex = GetSubWorldIndex(L);
	if (nSubWorldIndex < 0) 
		return 0;

	KMission Mission;
	Mission.SetMissionId(nMissionId);
	KMission * pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
	if (pMission)
	{
		char szScript[MAX_PATH];
		char nMissPathIdx[64]={0};
		//sprintf(nMissPathIdx,"%d_map",SubWorld[nSubWorldIndex].m_SubWorldID);
		sprintf(nMissPathIdx, "%d", nMissionId);
		g_MissionTabFile.GetString(nMissPathIdx, 2, "", szScript, MAX_PATH);  //nMissionId + 1
//		if (szScript[0])
		//Player[nPlayerIndex].ExecuteScript(szScript, "RunMission", "","");
		if (szScript[0])
			pMission->ExecuteScript(szScript,g_FileName2Id(szScript), "RunMission", 0);
	}
	
	return 0;
}
//CloseMission(missionId)
int LuaCloseMission(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 1) 
		return 0;
	
	int nMissionId = (int)Lua_ValueToNumber(L, 1);
	if (nMissionId < 0 )
		return 0;
	
	int nSubWorldIndex = GetSubWorldIndex(L);
	if (nSubWorldIndex < 0) 
		return 0;
	KMission StopMission;
	StopMission.SetMissionId(nMissionId);
	KMission * pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&StopMission);
	if (pMission)
	{
		char szScript[MAX_PATH];
		char nMissPathIdx[64]={0};
		//sprintf(nMissPathIdx,"%d_map",SubWorld[nSubWorldIndex].m_SubWorldID);
		sprintf(nMissPathIdx, "%d", nMissionId);
		g_MissionTabFile.GetString(nMissPathIdx, 2, "", szScript, MAX_PATH); //nMissionId + 1
		if (szScript[0])
			pMission->ExecuteScript(szScript, "EndMission",0);

        pMission->StopMission();
		SubWorld[nSubWorldIndex].m_MissionArray.Remove(pMission);	
	}
	return 0;
}
//StopMissionTimer(missionid, timerid)
int LuaStopMissionTimer(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 2) 
		return 0;
	int nMissionId = (int)Lua_ValueToNumber(L, 1);
	int nTimerId = (int)Lua_ValueToNumber(L, 2);
	int nSubWorldIndex = GetSubWorldIndex(L);
	
	if (nMissionId < 0 || nTimerId < 0 ) 
		return 0;
	
	if (nSubWorldIndex >= 0) 
	{
		KMission Mission;
		Mission.SetMissionId(nMissionId);
		KMission * pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
		if (pMission)
		{
			/*KTimerTaskFun StopTimer;
			//StopTimer.SetTimer(0, nTimerId);
			KTimerTaskFun * pTimer = pMission->m_cTimerTaskSet.GetData(&StopTimer);

			if (pTimer)
			{
				pTimer->CloseTimer();
				pMission->m_cTimerTaskSet.Remove(pTimer);
			}*/

			pMission->StopTimer(nTimerId);
		}
		
	}
	
	return 0;
}

//StartMissionTimer(missionid, timerid, time)
int LuaStartMissionTimer(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 3) 
		return 0;
	int nMissionId = (int)Lua_ValueToNumber(L, 1);
	int nTimerId = (int)Lua_ValueToNumber(L, 2);
	int nTimeInterval = (int)Lua_ValueToNumber(L, 3);
	int nSubWorldIndex = GetSubWorldIndex(L);
	
	if (nMissionId < 0 || nTimerId < 0 || nTimeInterval < 0) 
		return 0;
	
	if (nSubWorldIndex >= 0) 
	{
		KMission Mission;
		Mission.SetMissionId(nMissionId);
		KMission * pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
		if (pMission)
		{
			KTimerTaskFun * pTimer = pMission->m_cTimerTaskSet.Add();
			if (pTimer)
			{
				pTimer->SetTimer(nTimeInterval, nTimerId);
			}
		}
		
	}							 
	return 0;
}

//AddMSPlayer(MissionId, PlayerIndex, groupid); / AddMSPlayer(MissionId, groupid)
int LuaAddMissionPlayer(Lua_State * L)
{
	int nParamCount = Lua_GetTopIndex(L);
	if (nParamCount < 2) 
		return 0;
	int nMissionId = 0;
	int nPlayerIndex = 0;
	int nGroupId = 0;
	if (nParamCount >=3)
	{
		nMissionId = (int)Lua_ValueToNumber(L,1);
		nPlayerIndex = (int )Lua_ValueToNumber(L,2);
		nGroupId = (int) Lua_ValueToNumber(L,3);
	}
	else
	{
		nMissionId = (int)Lua_ValueToNumber(L,1);
		nGroupId = (int) Lua_ValueToNumber(L,2);
		nPlayerIndex = GetPlayerIndex(L);
	}
	
	if (nMissionId < 0 || nPlayerIndex <= 0 || nGroupId <0)
		return 0;
	
	int nSubWorldIndex = GetSubWorldIndex(L);
	if (nSubWorldIndex >= 0) 
	{
		KMission Mission;
		Mission.SetMissionId(nMissionId);
		KMission * pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);  //地图中的函数
		if (pMission)
		{
			pMission->AddPlayer(nPlayerIndex, Player[nPlayerIndex].m_dwID, nGroupId);
		}
	}
	return 0;
}
//复活所有NPC
int LuaRevivalAllNpc(Lua_State * L)
{
	int nSubWorldIndex = GetSubWorldIndex(L);
	if (nSubWorldIndex >= 0) 
	{
		SubWorld[nSubWorldIndex].RevivalAllNpc();
	}
	return 0;
}

//RemoveMSPlayer(MissionId, PlayerIndex, groupid)
int LuaRemoveMissionPlayer(Lua_State * L)
{
	int nParamCount = Lua_GetTopIndex(L);
	if (nParamCount < 2) 
		return 0;
	int nMissionId = 0;
	int nPlayerIndex = 0;
	int nGroupId = 0;
	if (nParamCount >=3)
	{
		nMissionId = (int)Lua_ValueToNumber(L,1);
		nPlayerIndex = (int )Lua_ValueToNumber(L,2);
		nGroupId = (int) Lua_ValueToNumber(L,3);
	}
	else
	{
		nMissionId   = (int)Lua_ValueToNumber(L,1);
		nGroupId     = (int) Lua_ValueToNumber(L,2);
		nPlayerIndex = GetPlayerIndex(L);
	}
	
	if (nMissionId < 0 || nPlayerIndex <= 0 || nGroupId <0)
		return 0;
	
	int nSubWorldIndex = GetSubWorldIndex(L);
	if (nSubWorldIndex >= 0) 
	{
		KMission Mission;
		Mission.SetMissionId(nMissionId);
		KMission * pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
		if (pMission)
		{
			pMission->RemovePlayerc(nPlayerIndex,0,SubWorld[nSubWorldIndex].m_SubWorldID);
			//pMission->AddPlayer(nPlayerIndex, Player[nPlayerIndex].m_dwID, nGroupId);
		}
	}
	return 0;
}
//GetNextPlayer(mission, idx,group)
int LuaGetNextPlayer(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 2)
	{
		Lua_PushNumber(L ,0);
		Lua_PushNumber(L, 0);
		return 2;
	}	
	int nMissionId	= (int)Lua_ValueToNumber(L, 1);
	int nIdx		= (int)Lua_ValueToNumber(L, 2);
	int nGroup		= (int)Lua_ValueToNumber(L, 3);
	int nSubWorldIndex = GetSubWorldIndex(L);
	int nResultIdx = 0;
	UINT nPlayerIndex = 0;

	if (nMissionId < 0 || nIdx < 0 || nGroup < 0) 
		goto lab_getnextplayer;
	
	if (nSubWorldIndex >= 0) 
	{
		KMission Mission;
		Mission.SetMissionId(nMissionId);
		KMission * pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
		if (pMission)
		{
			nResultIdx = pMission->GetNextPlayer(nIdx, nGroup, nPlayerIndex);  //玩家编号
		}
	}
	Lua_PushNumber(L, nResultIdx);
	Lua_PushNumber(L, nPlayerIndex);

lab_getnextplayer:	
	Lua_PushNumber(L, nResultIdx);
	Lua_PushNumber(L, nPlayerIndex);

	return 2;
}

//MSMsg2Group(missionid, string , group)
int LuaMissionMsg2Group(Lua_State * L)
{
	int nMissionId = (int)Lua_ValueToNumber(L,1);
	char * strMsg = (char *)Lua_ValueToString(L, 2);
	int	nGroupId = (int) Lua_ValueToNumber(L, 3);
	
	if (nMissionId < 0 || !strMsg || nGroupId <0)
		return 0;
	char strMsgInfo[256]={0};
	snprintf(strMsgInfo, sizeof(strMsgInfo), "%s", strMsg);
	int nSubWorldIndex = GetSubWorldIndex(L);
	if (nSubWorldIndex >= 0) 
	{
		KMission Mission;
		Mission.SetMissionId(nMissionId);
		KMission * pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
		if (pMission)
		{
			pMission->Msg2Group(strMsgInfo, nGroupId);
		}
	}
	
	return 0;
}

//MSMsg2Group(missionid, string)
int LuaMissionMsg2All(Lua_State * L)
{
	int nMissionId = (int)Lua_ValueToNumber(L,1);
	char * strMsg  = (char *)Lua_ValueToString(L, 2);
	
	if (nMissionId < 0 || !strMsg)
		return 0;
	char strMsgInfo[256]={0};
	snprintf(strMsgInfo, sizeof(strMsgInfo), "%s", strMsg);

	int nSubWorldIndex = GetSubWorldIndex(L);
	if (nSubWorldIndex >= 0) 
	{
		KMission Mission;
		Mission.SetMissionId(nMissionId);
		KMission * pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
		if (pMission)
		{
			pMission->Msg2All(strMsgInfo);
		}
	}
	
	return 0;
}

//MSMsg2Group(missionid, string , group)
int LuaMissionMsg2Player(Lua_State * L)
{
	int nMissionId = (int)Lua_ValueToNumber(L,1);
	char * strMsg = (char *)Lua_ValueToString(L, 2);
	int	nPlayerIndex = (int) Lua_ValueToNumber(L, 3);
	
	if (nMissionId < 0 || !strMsg || nPlayerIndex <0)
		return 0;
	char strMsgInfo[256]={0};
	snprintf(strMsgInfo, sizeof(strMsgInfo), "%s", strMsg);
	int nSubWorldIndex = GetSubWorldIndex(L);
	if (nSubWorldIndex >= 0) 
	{//在地图中有效
		KMission Mission;
		Mission.SetMissionId(nMissionId);
		KMission * pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
		if (pMission)
		{
			pMission->Msg2Group(strMsgInfo, nPlayerIndex);
		}
	}
	return 0;
}

int LuaMissionPlayerCount(Lua_State * L)
{
	int nParamCount = Lua_GetTopIndex(L);
	unsigned long ulCount = 0;
	int nMissionId = 0;
	int nGroupId = 0;
	int nSubWorldIndex = 0;
	if (nParamCount < 1) 
		goto lab_getmissionplayercount;
	
	if (nParamCount >= 2)
	{
		nMissionId = (int)Lua_ValueToNumber(L,1);
		nGroupId = (int) Lua_ValueToNumber(L,2);
	}
	else
	{
		nMissionId = (int)Lua_ValueToNumber(L,1);
	}
	
	if (nMissionId < 0 || nGroupId <0)
		goto lab_getmissionplayercount;
	
	nSubWorldIndex = GetSubWorldIndex(L);
	if (nSubWorldIndex >= 0) 
	{
		KMission Mission;
		Mission.SetMissionId(nMissionId);
		KMission * pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
		if (pMission)
		{
			ulCount = pMission->GetGroupPlayerCount(nGroupId);
		}
	}

lab_getmissionplayercount:
	Lua_PushNumber(L, ulCount);
	return 1;
}

int LuaSetPlayerDeathScript(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) 
		return 0;
	char * szScript = (char *)Lua_ValueToString(L, 1);
	Player[nPlayerIndex].m_dwDeathScriptId = g_FileName2Id(szScript);
	return 0;
}

int LuaSetPlayerDamageScript(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) 
		return 0;
	char * szScript = (char *)Lua_ValueToString(L, 1);
	Player[nPlayerIndex].m_dwDamageScriptId = g_FileName2Id(szScript);
	return 0;
}


int LuaPlayerIndexToNpcIndex(Lua_State * L)
{
   	int nResult = 0;

	int nPlayerIndex = (int)Lua_ValueToNumber(L, 1);

	if (nPlayerIndex >  0 && nPlayerIndex < MAX_PLAYER)
	{
		nResult = Npc[Player[nPlayerIndex].m_nIndex].m_Index;
	}

	Lua_PushNumber(L, nResult);
	return 1;
}


int LuaNpcIndexToPlayerIndex(Lua_State * L)
{
	int nResult = 0;
	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	if (nNpcIndex <=  0 || nNpcIndex >= MAX_NPC)
		goto lab_npcindextoplayerindex;
	
	if (Npc[nNpcIndex].m_Index > 0 && Npc[nNpcIndex].IsPlayer())
	{
		if (Npc[nNpcIndex].GetPlayerIdx() > 0)
			nResult = Npc[nNpcIndex].GetPlayerIdx();
	}
	
lab_npcindextoplayerindex:
	Lua_PushNumber(L, nResult);
	return 1;
	
}
//  
int LuaGetMissionPlayer_PlayerIndex(Lua_State * L)
{
	unsigned long nResult = 0;
	int nSubWorldIndex = 0;
	if (Lua_GetTopIndex(L) < 2) 
		goto lab_getmissionplayer_npcindex;
	
	nSubWorldIndex = GetSubWorldIndex(L);
	if (nSubWorldIndex >= 0) 
	{
		int nMissionId = (int)Lua_ValueToNumber(L, 1);
		int nDataIndex = (int)Lua_ValueToNumber(L, 2);
		if (nMissionId < 0 || nDataIndex < 0)
			goto lab_getmissionplayer_npcindex;
		
		KMission Mission;
		Mission.SetMissionId(nMissionId);
		KMission * pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
		if (pMission)
		{
			nResult = pMission->GetMissionPlayer_PlayerIndex(nDataIndex);
		}
	}
	
lab_getmissionplayer_npcindex:
	Lua_PushNumber(L,nResult);
	return 1;
}

int LuaGetMissionPlayer_DataIndex(Lua_State * L)
{
	unsigned long nResult = 0;
	int nSubWorldIndex = 0;
	if (Lua_GetTopIndex(L) < 2) 
		goto lab_getmissionplayer_dataindex;
	
	nSubWorldIndex = GetSubWorldIndex(L);
	if (nSubWorldIndex >= 0) 
	{
		int nMissionId = (int)Lua_ValueToNumber(L, 1);
		int nPlayerIndex = (int)Lua_ValueToNumber(L, 2);
		if (nMissionId < 0 || nPlayerIndex < 0)
			goto lab_getmissionplayer_dataindex;
		
		KMission Mission;
		Mission.SetMissionId(nMissionId);
		KMission * pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
		if (pMission)
		{
			nResult = pMission->GetMissionPlayer_DataIndex(nPlayerIndex);
		}
	}
	
lab_getmissionplayer_dataindex:
	Lua_PushNumber(L,nResult);
	return 1;
}



int LuaSetMissionRankParam(Lua_State * L)
{
	int nSubWorldIndex = 0;
	if (Lua_GetTopIndex(L) < 4) 
		return 0;
	
	nSubWorldIndex = GetSubWorldIndex(L);
	if (nSubWorldIndex >= 0) 
	{
		int nMissionId = (int)Lua_ValueToNumber(L, 1);
		int nDataIndex = (int)Lua_ValueToNumber(L, 2);
		int nParamId =	 (int)Lua_ValueToNumber(L ,3);
		int nValue =	 (int )Lua_ValueToNumber(L, 4);

		if (nMissionId < 0 || nDataIndex < 0 || nParamId > MAX_MISSION_PARAM)
			return 0;

		KMission Mission;
		Mission.SetMissionId(nMissionId);
		KMission * pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
		if (pMission)
		{
			//pMission->m_MissionPlayer.SetParam(nDataIndex, nParamId, nValue);
			pMission->SetPlayerParam(nDataIndex, nParamId, nValue);
		}
	}
	return 0;
}

int LuaUpMissionRankParam(Lua_State * L)
{
	int nSubWorldIndex = 0;
	
	nSubWorldIndex = GetSubWorldIndex(L);

	if (nSubWorldIndex >= 0) 
	{
		int nMissionId = (int)Lua_ValueToNumber(L,1);
		int nParamId =	 (int)Lua_ValueToNumber(L,2);
		if (nMissionId < 0 || nParamId > MAX_MISSION_PARAM)
			return 0;
		
		KMission Mission;
		Mission.SetMissionId(nMissionId);
		KMission * pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
		if (pMission)
		{
			//pMission->UpRankAllParam(nParamId);
			pMission->UpRankAllParam(nSubWorldIndex);
		}
	}
	return 0;
}


int LuaGetMissionRankParam(Lua_State * L)
{
	int nResult = 0;
	int nSubWorldIndex = 0;
	if (Lua_GetTopIndex(L) < 3) 
		goto lab_getmissionplayerparam;
	
	nSubWorldIndex = GetSubWorldIndex(L);
	if (nSubWorldIndex >= 0) 
	{
		int nMissionId = (int)Lua_ValueToNumber(L, 1);
		int nDataIndex = (int)Lua_ValueToNumber(L, 2);
		int nParamId =	 (int)Lua_ValueToNumber(L ,3);
		
		if (nMissionId < 0 || nDataIndex < 0 || nParamId > MAX_MISSION_PARAM)
			goto lab_getmissionplayerparam;
		
		KMission Mission;
		Mission.SetMissionId(nMissionId);
		KMission * pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
		if (pMission)
		{
			nResult = pMission->m_MissionPlayer.GetParam(nDataIndex, nParamId);
		}
	}
lab_getmissionplayerparam:
	Lua_PushNumber(L, nResult);
	return 1;
}


int LuaSetMissionPlayerParam(Lua_State * L)
{
	int nSubWorldIndex = 0;
	if (Lua_GetTopIndex(L) < 4) 
		return 0;
	
	nSubWorldIndex = GetSubWorldIndex(L);
	if (nSubWorldIndex >= 0) 
	{
		int nMissionId = (int)Lua_ValueToNumber(L, 1);
		int nDataIndex = (int)Lua_ValueToNumber(L, 2);
		int nParamId =	 (int)Lua_ValueToNumber(L ,3);
		int nValue =	 (int )Lua_ValueToNumber(L, 4);
		
		if (nMissionId < 0 || nDataIndex < 0 || nParamId > MAX_MISSION_PARAM)
			return 0;
		
		KMission Mission;
		Mission.SetMissionId(nMissionId);
		KMission * pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
		if (pMission)
		{
			if (nParamId == 1)
				pMission->m_MissionPlayer.SetParam1(nDataIndex, nValue);
			else
				pMission->m_MissionPlayer.SetParam2(nDataIndex, nValue);
		}
	}
	return 0;
}

int LuaGetMissionPlayerParam(Lua_State * L)
{
	int nResult = 0;
	int nSubWorldIndex = 0;
	if (Lua_GetTopIndex(L) < 3) 
		goto lab_getmissionplayerparam;
	
	nSubWorldIndex = GetSubWorldIndex(L);
	if (nSubWorldIndex >= 0) 
	{
		int nMissionId = (int)Lua_ValueToNumber(L, 1);
		int nDataIndex = (int)Lua_ValueToNumber(L, 2);
		int nParamId =	 (int)Lua_ValueToNumber(L ,3);
		
		if (nMissionId < 0 || nDataIndex < 0 || nParamId > 2)
			goto lab_getmissionplayerparam;
		
		KMission Mission;
		Mission.SetMissionId(nMissionId);
		KMission * pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
		if (pMission)
		{
			if (nParamId == 1)
				nResult = pMission->m_MissionPlayer.GetParam1(nDataIndex);
			else
				nResult = pMission->m_MissionPlayer.GetParam2(nDataIndex);
		}
	}
lab_getmissionplayerparam:
	Lua_PushNumber(L, nResult);
	return 1;
}

int LuaGetPlayerMissionGroup(Lua_State * L)
{
	int nResult = 0;
	int nSubWorldIndex = 0;
	if (Lua_GetTopIndex(L) < 2) 
		goto lab_getmissionplayergroup;
	
	nSubWorldIndex = GetSubWorldIndex(L);
	if (nSubWorldIndex >= 0) 
	{
		int nMissionId = (int)Lua_ValueToNumber(L, 1);
		int nNpcIndex = (int)Lua_ValueToNumber(L, 2);
		
		if (nMissionId < 0 || nNpcIndex < 0)
			goto lab_getmissionplayergroup;
		
		KMission Mission;
		Mission.SetMissionId(nMissionId);
		KMission * pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
		if (pMission)
		{
			nResult = pMission->GetMissionPlayer_GroupId(nNpcIndex);
		}			
	}
lab_getmissionplayergroup:
	Lua_PushNumber(L ,nResult);
	return 1;
	
}
//重生点 登陆模式 0为不利用重生点 1 为利用重生点
int LuaSetLogout(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) 
		return 0;
	int nType = (int)Lua_ValueToNumber(L, 1);
	
	if (nType)
		Player[nPlayerIndex].SetLoginType(1);
	else
		Player[nPlayerIndex].SetLoginType(0);
	
	return 0;
}

int LuaSetPlayerScriptWhenLogout(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) 
		return 0;
	int nType = (int)Lua_ValueToNumber(L, 1);
	
	if (nType)
		Player[nPlayerIndex].SetLoginType(1);
	else
		Player[nPlayerIndex].SetLoginType(0);
	
	return 0;
}



int LuaSetPlayerPKValue(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 1) 
		return 0;	
	int nPKValue = (int)Lua_ValueToNumber(L,1);
	
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) 
		return 0;
	Player[nPlayerIndex].m_cPK.SetPKValue(nPKValue);
	return 0;
}

int LuaGetPlayerPKValue(Lua_State * L)
{
	int nPKValue = 0;
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		goto lab_getplayerpkvalue;

	if (Player[nPlayerIndex].m_nIndex <= 0) 
		goto lab_getplayerpkvalue;
	nPKValue = Player[nPlayerIndex].m_cPK.GetPKValue();
	
lab_getplayerpkvalue:
	Lua_PushNumber(L, nPKValue);
	return 1;
}


int	LuaGetCurNpcIndex(Lua_State * L)
{
	int nNpcIndex = 0;
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		goto lab_getcurnpcindex;
	
lab_getcurnpcindex:
	Lua_PushNumber(L, Player[nPlayerIndex].m_nIndex);
	return 1;
}

int LuaGetRankInWorld(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
//	int nModel = (DWORD) Lua_ValueToNumber(L, 1);
 //   int nModel = (DWORD) Lua_ValueToNumber(L, 2);
	if (nPlayerIndex > 0)
		Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].nRankInWorld);
	else
        Lua_PushNumber(L, 0);

	return 1;	
}

//更新排名
//showladder(count, ladderid1, ladderid2.....)
int LuaShowLadder(Lua_State * L)
{
	int nParamCount = Lua_GetTopIndex(L);
	if (nParamCount < 2) 
		return 0;
	int nLadderCount = (DWORD) Lua_ValueToNumber(L, 1);
	if (nLadderCount <= 0)
		return 0;
	
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		return 0;
	
	if (nLadderCount > nParamCount - 1 )
		nLadderCount = nParamCount - 1;
	BYTE Buffer[sizeof(LADDER_LIST) + 50 * sizeof(DWORD)];
	LADDER_LIST * pLadderList = (LADDER_LIST*)&Buffer;
	pLadderList->ProtocolType = s2c_ladderlist;
	pLadderList->nCount = nLadderCount;
	pLadderList->wSize = sizeof(LADDER_LIST) + nLadderCount * sizeof(DWORD) - 1;
	for (int i = 0; i < nLadderCount; i ++)
	{
		pLadderList->dwLadderID[i] = (DWORD)Lua_ValueToNumber(L, i + 2);
	}
	g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, &Buffer, pLadderList->wSize + 1);
	return 0;
	
}

int LuaSwearBrother(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 1) 
		return 0;
	int nTeamId = Lua_ValueToNumber(L, 1);  //队伍ID
	
	if (nTeamId >= MAX_TEAM || nTeamId < 0)
		return 0;
	
	KPlayerChat::STRINGLIST BrotherList;
	if (g_Team[nTeamId].m_nCaptain <= 0)  //没有队长
          return 0;

	std::string strCapName;
	    strCapName = Npc[Player[g_Team[nTeamId].m_nCaptain].m_nIndex].Name; // 队长名字

	BrotherList.push_back(strCapName);  //压入队长名字

	for (int i  = 0; i < g_Team[nTeamId].m_nMemNum; ++i) //压入队员名字
	{
		int nPlayerIndex = g_Team[nTeamId].m_nMember[i];
		if ( nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER)
		{
			std::string strName;
			strName = 	Npc[Player[nPlayerIndex].m_nIndex].Name;
			BrotherList.push_back(strName); //亲人列表增加
		}
	}
	
	KPlayerChat::MakeBrother(BrotherList);  //更新列表

	return 0;
}
//制造仇杀？
int LuaMakeEnemy(Lua_State * L)
{
	char *scyName=NULL;
	char *decName=NULL;
	scyName     = (char *)Lua_ValueToString(L, 1);
    decName     = (char *)Lua_ValueToString(L, 2);
	KPlayerChat::MakeEnemy(scyName,decName);  //拉为仇人
	return 0;
}


int LuaAddLeadExp(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 1)
		return 0;
	
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		return 0;
	
	int nLeadExp = (int)Lua_ValueToNumber(L, 1);
	Player[nPlayerIndex].AddLeadExp(nLeadExp);
	return 0;
}

int LuaGetLeadLevel(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	int nLeadLevel = 0;
	if (nPlayerIndex <= 0)
		goto lab_getleadlevel;
	nLeadLevel = (int)Player[nPlayerIndex].m_dwLeadLevel;
	Lua_PushNumber(L, nLeadLevel);

lab_getleadlevel:
	Lua_PushNumber(L, nLeadLevel);
	return 1;
}
//还原技能点
int LuaAddMagicPoint(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 1)
		return 0;
	
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		return 0;
	int nSkillPoint = (int)Lua_ValueToNumber(L, 1);
	int nType = (int)Lua_ValueToNumber(L,2);
	
	if  (nType<=0)
	    Player[nPlayerIndex].m_nSkillPoint = nSkillPoint;	
	else
		Player[nPlayerIndex].m_nSkillPoint += nSkillPoint;

	if (Player[nPlayerIndex].m_nSkillPoint <= 0)
		Player[nPlayerIndex].m_nSkillPoint = 0;
	
	return 0;
}

//获取转身保留技能点
int LuaGetRoleVipJPoint(Lua_State * L)
{

	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		return 0;

	Lua_PushNumber(L, Player[nPlayerIndex].m_cVip.GetRoleVipKeepJpiont());
	return 1;

}

//获取转身保留技能点
int LuaGetReBornJPoint(Lua_State * L)
{

	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		return 0;
	
	Lua_PushNumber(L, Player[nPlayerIndex].m_cReBorn.GetReBornKeepJpiont());
	return 1;

}

//获取转身保留的潜能点
int LuaGetRoleVipQPoint(Lua_State * L)
{

	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		return 0;

	Lua_PushNumber(L, Player[nPlayerIndex].m_cVip.GetRoleVipKeepQpiont());
	return 1;

}
//获取转身保留的潜能点
int LuaGetReBornQPoint(Lua_State * L)
{
	
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		return 0;
	
	Lua_PushNumber(L, Player[nPlayerIndex].m_cReBorn.GetReBornKeepQpiont());
	return 1;
	
}


int LuaGetMagicPoint(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	int nSkillPoint = 0;
	if (nPlayerIndex <= 0)
		goto lab_getmagicpoint;
	
lab_getmagicpoint:
	Lua_PushNumber(L, Player[nPlayerIndex].m_nSkillPoint);
	return 1;
}

int LuaGetSkillProp(Lua_State * L)
{
	int nPlayerIndex = 0;	
	    nPlayerIndex = GetPlayerIndex(L);

	if (nPlayerIndex <= 0)
	{
		Lua_PushNumber(L,0);
		Lua_PushNumber(L,0);
		Lua_PushNumber(L,0);
		Lua_PushNumber(L,0);
		return 4;										  
	}

	int nSkiiID=(int)Lua_ValueToNumber(L,1);//Player[nPlayerIndex].GetLeftSkill();

	if (nSkiiID<=0 || nSkiiID>=MAX_SKILL)
	{
		Lua_PushNumber(L,0);
		Lua_PushNumber(L,0);
		Lua_PushNumber(L,0);
		Lua_PushNumber(L,0);
		return 4;
	}
	int nListidx = Npc[Player[nPlayerIndex].m_nIndex].m_SkillList.FindSame(nSkiiID);
	if (nListidx) 
	{
		Lua_PushNumber(L,Npc[Player[nPlayerIndex].m_nIndex].m_SkillList.GetSkillLevelByListidx(nListidx));
		Lua_PushNumber(L,Npc[Player[nPlayerIndex].m_nIndex].m_SkillList.GetCurSkillExpByListidx(nListidx));
		Lua_PushNumber(L,Npc[Player[nPlayerIndex].m_nIndex].m_SkillList.GetSkillExpByListidx(nListidx));
		Lua_PushNumber(L,nListidx);
	}
	else
	{
		Lua_PushNumber(L,0);
		Lua_PushNumber(L,0);
		Lua_PushNumber(L,0);
		Lua_PushNumber(L,0);
	}
	return 4;
}

int LuaIncSkillExp(Lua_State * L)
{
	int nParamCount = Lua_GetTopIndex(L);
	int nPlayerIndex = 0;
	if (nParamCount < 2) return 0;
	
	nPlayerIndex = GetPlayerIndex(L);
	
	if (nPlayerIndex <= 0) return 0;
	
	int nSkillId = 0, nAddExp = 0;
	nSkillId = (int)Lua_ValueToNumber(L, 1);
	nAddExp = (int)Lua_ValueToNumber(L, 2);
	if (nSkillId <= 0 || nAddExp <= 0)
		return 0;
	Player[nPlayerIndex].IncSkillExp(nSkillId, nAddExp);
	return 0;
}

int LuaAddPropPoint(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	int nPropPoint = 0;
	if (nPlayerIndex <= 0)
		return 0;
	nPropPoint = (int)Lua_ValueToNumber(L, 1);
	Player[nPlayerIndex].m_nAttributePoint += nPropPoint;
	return 0;
}



int LuaAddAndPayExtTime(Lua_State * L)
{
	int nResult = 0;
	int nPay = 0;
	int nPlayerIndex = 0;
	
	nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		return 0;
	
	nPay    = Lua_ValueToNumber(L, 1);
	nResult = Player[nPlayerIndex].AddAndPayExtTime(nPay);
	
	Lua_PushNumber(L, nResult);
	return 1;
}
//PayExtPoint
int LuaAddAndPayExtPoint(Lua_State * L)
{
	int nResult = 0;
	int nPay = 0;
	int nPlayerIndex = 0;
	
	nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		return 0;

	nPay    = Lua_ValueToNumber(L, 1);
	nResult = Player[nPlayerIndex].AddAndPayExtPoint(nPay);

	Lua_PushNumber(L, nResult);
	return 1;
}



int LuaSetVipChenHao(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	int nVip = Lua_ValueToNumber(L, 1);

        Npc[Player[nPlayerIndex].m_nIndex].m_IsVip=nVip;
	return 0;
}

int LuaSetTuiGuangChenHao(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	int nVip = Lua_ValueToNumber(L, 1);
	
	//Npc[Player[nPlayerIndex].m_nIndex].m_IsTuiGuang=nVip;
	return 0;
}

//设置会员类型 和 时间
int LuaSetGameTime(Lua_State * L)
{
    int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	int nLeftTime = Lua_ValueToNumber(L, 1);
	int nTime = Lua_ValueToNumber(L, 2);
	int nVip = Lua_ValueToNumber(L, 3);
	if (nLeftTime<=0 || nTime<=0)
		return 0;
	Player[nPlayerIndex].SetLeftTineAndVipType(nLeftTime,nVip,nTime);
    return 0;
}

int LuaSetExtPoint(Lua_State * L)
{
    int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	int nExtp = Lua_ValueToNumber(L, 1);
    int nCExtp = Lua_ValueToNumber(L, 2);
	if (nExtp<=0 || nCExtp<=0) return 0;
      Player[nPlayerIndex].SetExtPoint(nExtp,nCExtp);
    return 0;
}

int LuaVipType(Lua_State * L)
{
	int nPlayerIndex = 0;	
	    nPlayerIndex = GetPlayerIndex(L);
	int nResult=0;
	nResult = Player[nPlayerIndex].GetVipType();   
 
	Lua_PushNumber(L, nResult);
	
	return 1;
}

int LuaGetLeftTime(Lua_State * L)
{
	int nPlayerIndex = 0;	
	nPlayerIndex = GetPlayerIndex(L);
	DWORD nResult=0;
	char nReg[64];
	nResult = Player[nPlayerIndex].GetLeftTime();   
    //if (nResult>-1)
	//{
    //      sprintf(nReg,"%d",nResult);
	//}

	Lua_PushNumber(L, nResult);

	return 1;
}

//GetExtPoint
int LuaGetExtPoint(Lua_State * L)
{
	DWORD nResult = 0;
	int nPlayerIndex = 0;	
	nPlayerIndex = GetPlayerIndex(L);

	if (nPlayerIndex <= 0)
		goto lab_getextpoint;

	nResult = Player[nPlayerIndex].GetExtPoint();
	Lua_PushNumber(L, nResult);

lab_getextpoint:
	Lua_PushNumber(L, nResult);

	return 1;
}

//PayExtPoint
int LuaPayExtPoint(Lua_State * L)
{
	int nResult = 0;
	int nPay = 0;
	int nPlayerIndex = 0;
	if (Lua_GetTopIndex(L) < 1)
		goto lab_payextpoint;
	
	nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER || Player[nPlayerIndex].m_cTrade.m_nIsTrading != 0)
		goto lab_payextpoint;
	nPay = Lua_ValueToNumber(L, 1);
	if (nPay < 0)
		goto lab_payextpoint;
	nResult = Player[nPlayerIndex].PayExtPoint(nPay);

lab_payextpoint:
	Lua_PushNumber(L, nResult);
	return 1;
}

int LuaGetCExtTime(Lua_State * L)
{
	LONG nResult = 0;
	int nPlayerIndex = 0;	
	nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		return 0;
	
	nResult = Player[nPlayerIndex].GetExtTimeChanged();

	Lua_PushNumber(L, nResult);
	
	return 1;
}
//PayExtPoint
int LuaGetCExtPoint(Lua_State * L)
{
	LONG nResult = 0;
	int nPlayerIndex = 0;
	//	if (Lua_GetTopIndex(L) < 1)
	//		goto lab_getextpoint;	
	nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		goto lab_getextpoint;
	
	nResult = Player[nPlayerIndex].GetExtPointChanged();
	Lua_PushNumber(L, nResult);
	
lab_getextpoint:
	Lua_PushNumber(L, nResult);
	
	return 1;
}

int LuaGetRestPropPoint(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	int nPropPoint = 0;
	if (nPlayerIndex <= 0)
		goto lab_getrestproppoint;
	nPropPoint = Player[nPlayerIndex].m_nAttributePoint;
	
lab_getrestproppoint:
	Lua_PushNumber(L, nPropPoint);
	return 1;
}

int LuaSetBasePoint(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 4)
		return 1;
	int nPlayerIndex = GetPlayerIndex(L);
	int nPropPoint = 0;
	if (nPlayerIndex <= 0)
		return 1;
	if (Lua_IsNumber(L, 1) &&
		Lua_IsNumber(L, 2) &&
		Lua_IsNumber(L, 3) &&
		Lua_IsNumber(L, 4) )
	{
		Player[nPlayerIndex].m_nStrength = Lua_ValueToNumber(L, 1);
		Player[nPlayerIndex].m_nDexterity = Lua_ValueToNumber(L, 3);
		Player[nPlayerIndex].m_nVitality = Lua_ValueToNumber(L, 2);
		Player[nPlayerIndex].m_nEngergy = Lua_ValueToNumber(L, 4);
		Player[nPlayerIndex].m_nAttributePoint = (Npc[Player[nPlayerIndex].m_nIndex].m_Level - 1) * 5+Player[nPlayerIndex].m_cReBorn.GetReBornKeepQpiont()+Player[nPlayerIndex].m_cVip.GetRoleVipKeepQpiont();
	}

	return 1;
}

//Msg2GM(str, id)
int LuaMsgToGameMaster(Lua_State * L)
{
	int nParamCount = Lua_GetTopIndex(L);
	if ( nParamCount < 1 ) 
		return 0;
	int nParamID = 0;
	const char * szMsg = Lua_ValueToString(L, 1) ;
	if (!szMsg) 
		return 0;
	char strMsgInfo[256]={0};
	snprintf(strMsgInfo, sizeof(strMsgInfo), "%s", szMsg);
	if (nParamCount < 2)
	{
		nParamID = 0;
	}
	else
	{
		nParamID = (int) Lua_ValueToNumber(L, 2);
	}

	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0) 
	{
		char szID[32];

		sprintf(szID, "%d", nParamID);
		//KPlayerChat::SendInfoToGM(Player[nPlayerIndex].m_AccoutName, Npc[Player[nPlayerIndex].m_nIndex].Name, (char *) szMsg, strlen(szMsg) );
		KPlayerChat::SendInfoToGM("GM", szID, (char *) strMsgInfo, strlen(strMsgInfo) );
	}
	return 0;
}

//Msg2IP(IP, ID, str)
int LuaMsgToIP(Lua_State * L)
{
	int nParamCount = Lua_GetTopIndex(L);
	if ( nParamCount < 3 ) 
		return 0;
	int nIP = 0;
	const char * szIP = Lua_ValueToString(L, 1) ;
	nIP = _a2ip(szIP);
	if (nIP == 0)
		return 0;

	int nID = (int) Lua_ValueToNumber(L, 2);
	const char * szMsg = Lua_ValueToString(L, 3) ;
	if (!szMsg) 
		return 0;
	char strMsgInfo[256]={0};
	snprintf(strMsgInfo, sizeof(strMsgInfo), "%s", szMsg);
	int nParamID = 0;
	if (nParamCount < 4)
	{
		nParamID = 0;
	}
	else
	{
		nParamID = (int) Lua_ValueToNumber(L, 4);
	}
	char szID[32];
	sprintf(szID, "%d", nParamID);
	KPlayerChat::SendInfoToIP(nIP, nID, "GM", szID, (char *) strMsgInfo, strlen(strMsgInfo) );
	return 0;
}

int LuaGetPlayerInfo(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	char szDesMsg[250];
	char szMsg[800];
	int nNpcIdx = 0;
	KNpc * pNpc = NULL;
	KPlayer* pPlayer = NULL;
	szDesMsg[0] = 0;
	if (nPlayerIndex <= 0)
		goto lab_getplayerinfo;
	
	pPlayer = &Player[nPlayerIndex];
	nNpcIdx = pPlayer->m_nIndex;
	pNpc = &Npc[nNpcIdx];
	char nIpInfo[32]={0};
	 g_pServer->GetClientInfo(pPlayer->m_nNetConnectIdx,nIpInfo,sizeof(nIpInfo));

	sprintf(szMsg, "Name:%s,Lvl:%d,IP:%s,Lf:%lld|%lld,Mn:%lld|%lld,Mny:%d,sp:%d,ap:%d", pNpc->Name, pNpc->m_Level,nIpInfo, pNpc->m_CurrentLife, pNpc->m_CurrentLifeMax, pNpc->m_CurrentMana, pNpc->m_CurrentManaMax, pPlayer->m_ItemList.GetsMon(room_equipment), pPlayer->m_nSkillPoint, pPlayer->m_nAttributePoint );
	g_StrCpyLen(szDesMsg, szMsg, 250);
	
lab_getplayerinfo:
	Lua_PushString(L, szDesMsg);
	return 1;
}

int LuaGetIP(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	char szDesMsg[200];
	szDesMsg[0] = 0;
	KPlayer * pPlayer = NULL;
	if (nPlayerIndex <= 0)
		goto lab_getplayerip;
	pPlayer = &Player[nPlayerIndex];

	//char nIpInfo[32]={0};
	g_pServer->GetClientInfo(pPlayer->m_nNetConnectIdx, szDesMsg,sizeof(szDesMsg));
    if (szDesMsg[0] == '\0')
    {
        strcpy(szDesMsg, "OffLive");
    }

	//strcpy(szDesMsg, g_pServer->GetClientInfo(pPlayer->m_nNetConnectIdx));
	
lab_getplayerip:
	Lua_PushString(L, szDesMsg);
	return 1;
}

int LuaGetPlayerIP(Lua_State * L)
{
	int nPlayerIndex =(int)Lua_ValueToNumber(L, 1);
	char szDesMsg[200];
	szDesMsg[0] = 0;
	KPlayer * pPlayer = NULL;
	if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
		goto lab_getplayerip;
	pPlayer = &Player[nPlayerIndex];

    g_pServer->GetClientInfo(pPlayer->m_nNetConnectIdx,szDesMsg,sizeof(szDesMsg));
    if (szDesMsg[0] == '\0')
    {
        strcpy(szDesMsg, "OffLive");
    }
	//strcpy(szDesMsg, g_pServer->GetClientInfo(pPlayer->m_nNetConnectIdx));
	
lab_getplayerip:
	Lua_PushString(L, szDesMsg);
	return 1;
}


//Y隐藏NPC
int LuaHideNpc(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 2)
		return 0;
	
	int nNpcIndex  = 0;
	
	if (Lua_IsNumber(L,1))
	{
		nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	}
	else 
	{
		const char * szName = Lua_ValueToString(L, 1);

		int nSubWorldIndex = GetSubWorldIndex(L);
		if (nSubWorldIndex < 0)
			return 0;
		
		nNpcIndex = SubWorld[nSubWorldIndex].FindNpcFromName(szName);
	}
	
	if (nNpcIndex > 0 || nNpcIndex < MAX_NPC)
	{
		int nFrame = Lua_ValueToNumber(L, 2);
		if (nFrame <= 0)
			nFrame = 1;
		if (Npc[nNpcIndex].m_Doing == do_revive)
			Npc[nNpcIndex].m_Doing = do_none;

		Npc[nNpcIndex].ExecuteRevive();	            //复活这个NPC
		Npc[nNpcIndex].SetNpcIsReFrame(1,nFrame);	//同步这个重生时间
		Npc[nNpcIndex].m_Frames.nTotalFrame = nFrame;
		Npc[nNpcIndex].m_Frames.nCurrentFrame = 0;
	}
	
	return 0;
}

//打开宝石镶套界面
int LuaOpenstone(Lua_State * L)
{

    int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	
	BYTE	NetCommand = (BYTE)s2c_openstone;
	g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, &NetCommand, sizeof(BYTE));
	
	return 0;
}

//打开锻造面板
int LuaOpenDuanZhao(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;

	int nType= (int)Lua_ValueToNumber(L, 1);

	PLAYER_DUANZHAO nDuanzhao;

	nDuanzhao.ProtocolType = (BYTE)s2c_openduanzhao;
    nDuanzhao.m_nType=(BYTE)nType;
	g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, &nDuanzhao, sizeof(PLAYER_DUANZHAO));	
	return 0;
}

int LuaOpenVipDaZao(Lua_State * L)
{
   	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;

	int nType= (int)Lua_ValueToNumber(L, 1);

	int nCurExtPionts= (int)Lua_ValueToNumber(L, 2);

	PLAYER_VIPDAZAO nVipzhao;

	nVipzhao.ProtocolType = (BYTE)s2c_openvipdazao;
    nVipzhao.m_nType=nType;
	nVipzhao.m_nCurExtPiont =nCurExtPionts;

	g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, &nVipzhao, sizeof(PLAYER_VIPDAZAO));	
	return 0;
}



//打开铸造界面
int LuaTremble(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	
	BYTE	NetCommand = (BYTE)s2c_opentremble;
	g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, &NetCommand, sizeof(BYTE));
	
	return 0;
}

//打开纹钢界面
int LuaOpenwengang(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	
	BYTE	NetCommand = (BYTE)s2c_openwengang;
	g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, &NetCommand, sizeof(BYTE));
		
	return 0;

}


int LuaGetGiveItemUnit(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	int nItemIndex  = (int)Lua_ValueToNumber(L, 1);
	int nRoomIdx  = (int)Lua_ValueToNumber(L, 2);
	//Lua_PushNumber(L, Player[nPlayerIndex].m_ItemList.GetIndexItem(room_give,nItemIndex));
	Lua_PushNumber(L, Player[nPlayerIndex].m_ItemList.GetItemIndexByidx(nItemIndex,nRoomIdx));
	
	return 1;
}


int LuaGiveItemUI(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	if (Player[nPlayerIndex].CheckTrading())
		return 0;
	PLAYER_GIVE PGive;
	PGive.ProtocolType = (BYTE)s2c_opengive;
	const char * szName = Lua_ValueToString(L, 2);  //	说明
	if (sizeof(szName) > sizeof(PGive.m_szName))
		return 0;
	g_StrCpyLen(PGive.m_szName, szName, sizeof(PGive.m_szName));

	PGive.m_nNum = (int)Lua_ValueToNumber(L, 5); //  数量

	if  (PGive.m_nNum>24)
		PGive.m_nNum = 24;

	//  PLAYER_GIVE_CALLBACK PGBack;
	const char * callback = Lua_ValueToString(L, 3);  //	回调函数
	if (sizeof(callback) > sizeof(PGive.m_Callback))
		return 0;
	//strcpy(PGBack.m_Callback,callback);
	//开始处理可以带参数的回调函数
	g_StrCpyLen(PGive.m_Callback,callback,sizeof(PGive.m_Callback));

	const char * error = Lua_ValueToString(L, 4);  //	错误信息
	if (sizeof(error) > sizeof(PGive.m_Error))
		return 0;
	//	strcpy(PGBack.m_Error,error);
	g_StrCpyLen(PGive.m_Error,error,sizeof(PGive.m_Error));

	char * title=NULL;
	title = (char*)Lua_ValueToString(L, 1);  //	标题
	if (sizeof(title) > sizeof(PGive.m_szTitle))
		return 0;
	//	strcpy(PGBack.m_Error,error);
	g_StrCpyLen(PGive.m_szTitle, title, sizeof(PGive.m_szTitle));
	if (g_pServer)
		g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, &PGive, sizeof(PLAYER_GIVE));  //通知打开UI

	Player[nPlayerIndex].m_ItemList.ClearRoom(room_give);  //清除给予界面
	Player[nPlayerIndex].m_nGiveCallBackScript = Npc[Player[nPlayerIndex].m_nIndex].m_ActionScriptID;  //回调函数的脚本id
	//if  (Lua_IsNumber(L,3))
	//{
	//	Player[nPlayerIndex].m_nGiveCallBackNum = (int)Lua_ValueToNumber(L, 3);    //数量
	//}
	//else if (Lua_IsString(L, 3)) 
	//{
	//	Player[nPlayerIndex].m_nGiveCallBackNum = atoi((char *)Lua_ValueToString(L, 3));  //数量
	//}

	//if (Player[nPlayerIndex].m_nGiveCallBackNum>24)
		Player[nPlayerIndex].m_nGiveCallBackNum = 24;

	return 0;
}



int LuaGive(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	if (Player[nPlayerIndex].CheckTrading())
		return 0;
	PLAYER_GIVE PGive;
	PGive.ProtocolType = (BYTE)s2c_opengive;
	const char * szName = Lua_ValueToString(L, 1);  //	说明
	if (sizeof(szName) > sizeof(PGive.m_szName))
		return 0;
	g_StrCpyLen(PGive.m_szName, szName, sizeof(PGive.m_szName));

	PGive.m_nNum = (int)Lua_ValueToNumber(L, 2); //  数量

	if  (PGive.m_nNum>24)
		PGive.m_nNum = 24;

  //  PLAYER_GIVE_CALLBACK PGBack;
	const char * callback = Lua_ValueToString(L, 4);  //	回调函数
	if (sizeof(callback) > sizeof(PGive.m_Callback))
		return 0;
	//strcpy(PGBack.m_Callback,callback);
	//开始处理可以带参数的回调函数
	g_StrCpyLen(PGive.m_Callback,callback,sizeof(PGive.m_Callback));

	const char * error = Lua_ValueToString(L, 5);  //	错误信息
	if (sizeof(error) > sizeof(PGive.m_Error))
		return 0;
//	strcpy(PGBack.m_Error,error);
	g_StrCpyLen(PGive.m_Error,error,sizeof(PGive.m_Error));

	char * title=NULL;
	title = (char*)Lua_ValueToString(L, 6);  //	标题
	if (sizeof(title) > sizeof(PGive.m_szTitle))
		return 0;
	//	strcpy(PGBack.m_Error,error);
	g_StrCpyLen(PGive.m_szTitle, title, sizeof(PGive.m_szTitle));
	if (g_pServer)
	   g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, &PGive, sizeof(PLAYER_GIVE));  //通知打开UI
	
	Player[nPlayerIndex].m_ItemList.ClearRoom(room_give);  //清除给予界面
	Player[nPlayerIndex].m_nGiveCallBackScript = Npc[Player[nPlayerIndex].m_nIndex].m_ActionScriptID;  //回调函数的脚本id
	
	if  (Lua_IsNumber(L,3))
	{
		Player[nPlayerIndex].m_nGiveCallBackNum = (int)Lua_ValueToNumber(L, 3);    //数量
	}
	else if (Lua_IsString(L, 3)) 
	{
		Player[nPlayerIndex].m_nGiveCallBackNum = atoi((char *)Lua_ValueToString(L, 3));  //数量
	}

	if (Player[nPlayerIndex].m_nGiveCallBackNum>24)
		Player[nPlayerIndex].m_nGiveCallBackNum = 24;
	
    return 0;
}

//字符串输入
int LuaOpenStringUI(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;

	PLAYER_STRING PString;
	PString.ProtocolType = (BYTE)s2c_openstringbox;
	const char * szTitle = Lua_ValueToString(L, 1);  //	说明
	if (sizeof(szTitle) > sizeof(PString.m_szTitle))
		return 0;
	g_StrCpyLen(PString.m_szTitle, szTitle, sizeof(PString.m_szTitle));

	const char * scallback = Lua_ValueToString(L, 2);  //	回调函数
	if (sizeof(scallback) > sizeof(PString.m_SCallback))
		return 0;
	//strcpy(PGBack.m_Callback,callback);
	g_StrCpyLen(PString.m_SCallback,scallback,sizeof(PString.m_SCallback));
	
	const char * serror = Lua_ValueToString(L, 3);  //	错误信息
	if (sizeof(serror) > sizeof(PString.m_SError))
		return 0;
	//	strcpy(PGBack.m_Error,error);
	g_StrCpyLen(PString.m_SError,serror,sizeof(PString.m_SError));

	PString.m_MaxLen =(int)Lua_ValueToNumber(L, 4);  //字符的最长度

	PString.m_nNum   =(int)Lua_ValueToNumber(L, 5);  //1 为数字 2为 字符串

	g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, &PString, sizeof(PLAYER_STRING));
	Player[nPlayerIndex].m_nGiveCallBackScript = Npc[Player[nPlayerIndex].m_nIndex].m_ActionScriptID;  //回调函数
    Player[nPlayerIndex].m_nGiveCallBackNum =  1;

	return 0;
}

int LuaIsLiXian(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) 
		return 0;

	if (!Npc[Player[nPlayerIndex].m_nIndex].m_FightMode)
		Player[nPlayerIndex].m_nLiXian = 1;

	Lua_PushNumber(L, Player[nPlayerIndex].m_nLiXian);
	return 1;
}

int LuaNotLiXian(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	
	Player[nPlayerIndex].m_nLiXian = 0;

	Lua_PushNumber(L, Player[nPlayerIndex].m_nLiXian);
	return 1;
}


int LuaOfflineLive(Lua_State *L)
{
/*	int nIndex = GetPlayerIndex(L);
	
	if (nIndex <= 0)
		return 0;

	printf("GM Offline Live6 player one.\n");
	BYTE	NetCommand = (BYTE)s2c_exitgame;
	g_pServer->PackDataToClient(Player[nIndex].m_nNetConnectIdx, &NetCommand, sizeof(BYTE));

	return 1;*/

	int nIndex = GetPlayerIndex(L);
	
	if (nIndex <= 0)
		return 0;
	
	if (Player[nIndex].m_nNetConnectIdx >= 0)
	{
		printf("GM Offline Live player one.\n");
		//BYTE	NetCommand = (BYTE)s2c_exitgame;
		//g_pServer->PackDataToClient(Player[nIndex].m_nNetConnectIdx, &NetCommand, sizeof(BYTE));
		Player[nIndex].KickOutGame();
	}
	return 1;
}

int LuaOfflineLiveAll(Lua_State *L)
{
	int nIndex = GetPlayerIndex(L);
	if (nIndex <= 0)
		return 0;

	printf("GM Offline Live All Player .\n");

	int nIndex2 = 1;

	while(nIndex2 > 0)
	{
		if(nIndex2 != nIndex)
		{	//s2c_kickoutgame
			//BYTE	NetCommand = (BYTE)s2c_exitgame;
			//g_pServer->PackDataToClient(Player[nIndex2].m_nNetConnectIdx, &NetCommand, sizeof(BYTE));
			Player[nIndex2].KickOutGame();
		}
		nIndex2++;
		if(nIndex2 > MAX_NPC)
			break;
	}
	return 1;
}

int LuaGetPlayerIndexByAccName(Lua_State *L)
{
    if (Lua_GetTopIndex(L) < 2)
        return 0;

    const char* pszName = (const char*)Lua_ValueToString(L, 1);
	int nType = (int)Lua_ValueToNumber(L,2);
    int nIndex = PlayerSet.GetFirstPlayer();
    int playerIndex = -1;

    while (nIndex > 0)
    {
		if (nType <= 0)
		{
            int npcIdx = Player[nIndex].m_nIndex;
            if (npcIdx >= 0 && strcmpi(Npc[npcIdx].Name, pszName) == 0)
            {
				playerIndex = nIndex;
				break;
			}
		}
		else
		{
			if (strcmpi(Player[nIndex].m_AccoutName, pszName) == 0)
			{
				playerIndex = nIndex;
				break;
			}
		}

        nIndex = PlayerSet.GetNextPlayer();
    }
    Lua_PushNumber(L, playerIndex);
    return 1;
}

// 设置面具状态
int LuaSetMask(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;

	if (Lua_IsNumber(L,1))
	{
		Player[nPlayerIndex].m_ItemList.SetMaskLock(TRUE);  //同步面具锁住
		Npc[Player[nPlayerIndex].m_nIndex].m_MaskType = (int)Lua_ValueToNumber(L,1);
	}

	return 0;
}

int LuaSetNpcMask(Lua_State * L)
{
     int nNpcIdx = (int)Lua_ValueToNumber(L,1);
     int nNpcSetings=(int)Lua_ValueToNumber(L,2);

     if  (nNpcIdx<=0 || nNpcIdx >= MAX_NPC)
		 return 0;
 
	 Npc[nNpcIdx].m_MaskType = nNpcSetings;

	 return 0;
}

//恢复非面具状态
int LuaReSetMask(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	
	Player[nPlayerIndex].m_ItemList.SetMaskLock(FALSE);//同步面具解锁
	int nIdx = Player[nPlayerIndex].m_ItemList.GetEquipment(itempart_mask);    //获取面具的idx
	Npc[Player[nPlayerIndex].m_nIndex].m_MaskType = Item[nIdx].GetBaseMagic(); //获取面具的变形类型

	return 0;
}

void SendRank(int nPlayerIndex, int nSubWorldIndex, bool stopped);

int LuaBT_SetView(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
		return 0;
	if (Lua_GetTopIndex(L) < 1)
		return 0;
	int nSubWorldIndex = GetSubWorldIndex(L);
	if (nSubWorldIndex >= 0 && nSubWorldIndex < MAX_SUBWORLD)
	{
		SendRank(nPlayerIndex, nSubWorldIndex, true);
	}
	return 0;
}

int LuaBT_LeaveBattle(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
		return 0;
	memset(Player[nPlayerIndex].BonusType, 0, sizeof(Player[nPlayerIndex].BonusType));
	return 0;
}

int LuaBT_UpdateMemberCount(Lua_State* L)
{
	int nSubWorldIndex = GetSubWorldIndex(L);
	if (nSubWorldIndex < 0 || nSubWorldIndex >= MAX_SUBWORLD)
		return 0;
	return 0;
}

int LuaBT_ViewBattleStart(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
		return 0;
	int nSubWorldIndex = GetSubWorldIndex(L);
	if (nSubWorldIndex >= 0 && nSubWorldIndex < MAX_SUBWORLD)
	{
		SendRank(nPlayerIndex, nSubWorldIndex, false);
	}
	return 0;
}

int LuaBT_BroadAllLadder(Lua_State* L)
{
	int nSubWorldIndex = GetSubWorldIndex(L);
	if (nSubWorldIndex >= 0 && nSubWorldIndex < MAX_SUBWORLD)
	{
		int nPlayerIndex = GetPlayerIndex(L);
		if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER)
			SendRank(nPlayerIndex, nSubWorldIndex, true);
	}
	return 0;
}

int LuaBT_BroadGameData(Lua_State* L)
{
	int nSubWorldIndex = GetSubWorldIndex(L);
	if (nSubWorldIndex >= 0 && nSubWorldIndex < MAX_SUBWORLD)
	{
		int nPlayerIndex = GetPlayerIndex(L);
		if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER)
			SendRank(nPlayerIndex, nSubWorldIndex, false);
	}
	return 0;
}

int LuaBT_SetRestTime(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 1)
		return 0;
	int nTime = (int)Lua_ValueToNumber(L, 1);
	int nSubWorldIndex = GetSubWorldIndex(L);
	if (nSubWorldIndex >= 0 && nSubWorldIndex < MAX_SUBWORLD)
	{
		SubWorld[nSubWorldIndex].GameData[3/*REST_TIME*/] = nTime;
	}
	return 0;
}

int LuaBT_BroadView(Lua_State* L)
{
	int nSubWorldIndex = GetSubWorldIndex(L);
	if (nSubWorldIndex >= 0 && nSubWorldIndex < MAX_SUBWORLD)
	{
		int nPlayerIndex = GetPlayerIndex(L);
		if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER)
			SendRank(nPlayerIndex, nSubWorldIndex, true);
	}
	return 0;
}

int LuaBT_ClearPlayerData(Lua_State* L)//xoa data cua ~ trong tong kim
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex == -1)
		return 0;

	memset(Player[nPlayerIndex].BonusType, 0, sizeof(Player[nPlayerIndex].BonusType));
	return 0;
}

void RankCalculator(KMission* pMission, PlayerRank* player)
{
	auto it = std::find_if(pMission->m_RankGlobal.begin(), pMission->m_RankGlobal.end(), [&](const PlayerRank* v) { return v->id == player->id; });
	if (it != pMission->m_RankGlobal.end())// neu co' hang
	{
		delete (*it);
		// Deletes the element pointing by iterator it
		pMission->m_RankGlobal.erase(it);
	}

	pMission->m_RankGlobal.insert(player);

	if (pMission->m_RankGlobal.size() > 10)
	{
		it = pMission->m_RankGlobal.end();
		delete (*it);
		pMission->m_RankGlobal.erase(it);
	}
}


int LuaBT_SortLadder(Lua_State* L)// sort thu hang
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex == -1)
		return 0;

	int nSubWorldIndex = GetSubWorldIndex(L);
	if (nSubWorldIndex >= 0 && nSubWorldIndex < MAX_SUBWORLD)
	{
		int nMissionId = SubWorld[nSubWorldIndex].GameData[3/*GAME_RULEID*/ - 1];

		KMission Mission;
		Mission.SetMissionId(nMissionId);
		KMission* pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
		if (pMission)
		{

			PlayerRank* player = new PlayerRank(Npc[Player[nPlayerIndex].m_nIndex].Name);
			if (!player)
				return 0;

			player->group = Npc[Player[nPlayerIndex].m_nIndex].m_CurrentCamp;//1:song 2:jin

			player->point = Player[nPlayerIndex].BonusType[1/*PL_TOTALPOINT*/ - 1];
			player->player = Player[nPlayerIndex].BonusType[2/*PL_KILLPLAYER*/ - 1];
			player->npc = Player[nPlayerIndex].BonusType[3/*PL_KILLNPC*/ - 1];
			player->bekilled = Player[nPlayerIndex].BonusType[4/*PL_BEKILLED*/ - 1];
			player->serieskill = Player[nPlayerIndex].BonusType[14/*PL_SERIESKILL*/ - 1];
			player->getitem = Player[nPlayerIndex].BonusType[17/*PL_GETITEM*/ - 1];

			::EnterCriticalSection(&rankSection);
			RankCalculator(pMission, player);
			::LeaveCriticalSection(&rankSection);
		}
	}
	return 0;
}

void SendRank(int nPlayerIndex, int nSubWorldIndex, bool stopped = false)
{
	if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
		return;

	if (nSubWorldIndex < 0 || nSubWorldIndex >= MAX_SUBWORLD)
		return;

	int nMissionId = SubWorld[nSubWorldIndex].GameData[3/*GAME_RULEID*/ - 1];

	KMission Mission;
	Mission.SetMissionId(nMissionId);
	KMission* pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
	if (pMission)
	{
		LADDER_DATA	LadderData;
		LadderData.ProtocolType = s2c_ladderresult;

		int count = 0;
/*		for (auto it = pMission->m_RankGlobal.begin(); it != pMission->m_RankGlobal.end(); it++)
		{
			if (count < 0 || count >= 10)
				return;
			strcpy(LadderData.StatData[count].name, (*it)->name);
			LadderData.StatData[count].group = (*it)->group;
			LadderData.StatData[count].point = (*it)->point;
			LadderData.StatData[count].player = (*it)->player;
			LadderData.StatData[count].npc = (*it)->npc;
			LadderData.StatData[count].bekilled = (*it)->bekilled;
			LadderData.StatData[count].serieskill = (*it)->serieskill;
			LadderData.StatData[count].getitem = (*it)->getitem;
			++count;
		}*/

		for (auto it = pMission->m_RankGlobal.begin(); it != pMission->m_RankGlobal.end(); it++)
		{
			if (count < 0 || count >= 10)
				return;
			snprintf(LadderData.StatData[count].Name, sizeof(LadderData.StatData[count].Name), "%s", (*it)->name);
			LadderData.group = (*it)->group;
			LadderData.point = (*it)->point;
			LadderData.player = (*it)->player;
			LadderData.npc = (*it)->npc;
			LadderData.bekilled = (*it)->bekilled;
			LadderData.serieskill = (*it)->serieskill;
			LadderData.getitem = (*it)->getitem;
			++count;
		}


		LadderData.dwLadderID = count;  //获取排名的索引 0 1 2 3 4 种

		LadderData.group = Npc[Player[nPlayerIndex].m_nIndex].m_CurrentCamp;

		LadderData.point = Player[nPlayerIndex].BonusType[1/*PL_TOTALPOINT*/ - 1];
		LadderData.player = Player[nPlayerIndex].BonusType[2/*PL_KILLPLAYER*/ - 1];
		LadderData.npc = Player[nPlayerIndex].BonusType[3/*PL_KILLNPC*/ - 1];
		LadderData.bekilled = Player[nPlayerIndex].BonusType[4/*PL_BEKILLED*/ - 1];
		LadderData.serieskill = Player[nPlayerIndex].BonusType[14/*PL_SERIESKILL*/ - 1];
		LadderData.getitem = Player[nPlayerIndex].BonusType[17/*PL_GETITEM*/ - 1];

		LadderData.pvp = Player[nPlayerIndex].BonusType[16/*PL_1VS1*/ - 1];
		LadderData.snapflag = Player[nPlayerIndex].BonusType[5/*PL_SNAPFLAG*/ - 1];
		LadderData.maxserieskill = Player[nPlayerIndex].BonusType[13/*PL_MAXSERIESKILL*/ - 1];

		if (stopped)
			Npc[Player[nPlayerIndex].m_nIndex].m_CurNpcTitle = 0;
		else
			LadderData.nCurNpcTitle = Npc[Player[nPlayerIndex].m_nIndex].m_CurNpcTitle;

		LadderData.totalSong = pMission->GetGroupPlayerCount(1);
		LadderData.totalJin = pMission->GetGroupPlayerCount(2);

		LadderData.totalMinutes = SubWorld[nSubWorldIndex].m_MissionArray.GetMissionValue(17);

		g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, &LadderData, sizeof(LadderData));
	}
}

int LuaBT_BroadSelf(Lua_State* L)//mo len cho nhin thay thong tin trong bang ~
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER)
	{
		int nSubWorldIndex = GetSubWorldIndex(L);
		if (nSubWorldIndex >= 0 && nSubWorldIndex < MAX_SUBWORLD)
		{
			SendRank(nPlayerIndex, nSubWorldIndex);
		}
	}
	return 0;
}

int LuaBT_WorldLadder(Lua_State* L)
{
	int nSubWorldIndex = GetSubWorldIndex(L);
	if (nSubWorldIndex >= 0 && nSubWorldIndex < MAX_SUBWORLD)
	{
		int nMissionId = SubWorld[nSubWorldIndex].GameData[3/*GAME_RULEID*/ - 1];
		KMission Mission;
		Mission.SetMissionId(nMissionId);
		KMission* pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
		if (pMission)
		{
			LadderList* top;
			auto it = std::find_if(m_LadderRank.begin(), m_LadderRank.end(), [&](const LadderList* v) { return v->id == 1; });
			if (it == m_LadderRank.end())
			{
				top = new LadderList(1);
				m_LadderRank.push_back(top);
			}
			else
			{
				top = *it;
			}

			UINT idx = 0, nPlayerIndex = 0;
			while (true)
			{
				idx = pMission->GetNextPlayer(idx, 0, nPlayerIndex);
				if (idx == 0)
					break;
				if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
					break;
				if (Player[nPlayerIndex].m_nIndex <= 0 || Player[nPlayerIndex].m_nIndex >= MAX_NPC)
					break;
				auto id = g_FileName2Id(Npc[Player[nPlayerIndex].m_nIndex].Name);
				auto obj = std::find_if(top->m_list.begin(), top->m_list.end(), [&](const LadderInfo* v) { return v->id == id; });
				if (obj != top->m_list.end())
				{
					if ((*obj)->value >= Player[nPlayerIndex].BonusType[1/*PL_TOTALPOINT*/ - 1])
						goto skip;
					delete (*obj);
					top->m_list.erase(obj);
				}
				//enter
				LadderInfo* info = new LadderInfo(Npc[Player[nPlayerIndex].m_nIndex].Name, id);
				info->value = Player[nPlayerIndex].BonusType[1/*PL_TOTALPOINT*/ - 1];
				info->faction = -1;
				info->gender = -1;
				top->LadderEnter(info);
			skip:
				SendRank(nPlayerIndex, nSubWorldIndex, true);
			}
		}
	}
	return 0;
}


int LuaBT_SongJin(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 1)
		return 0;

	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex == -1)
		return 0;

	LADDER_LIST pLadderList;
	pLadderList.ProtocolType = s2c_ladderlist;
	pLadderList.wSize = sizeof(LADDER_LIST) - 1;
	pLadderList.nCount = 0;

	g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, &pLadderList, sizeof(LADDER_LIST));

	return 0;
}

int LuaBT_SetData(Lua_State* L)
{
	if (Lua_GetTopIndex(L) <= 1)
		return 0;

	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex == -1)
		return 0;

	int nOption = (int)Lua_ValueToNumber(L, 1);
	int nValue = (int)Lua_ValueToNumber(L, 2);

	if ((nOption >= 1) && (nOption <= 48))
	{
		Player[nPlayerIndex].BonusType[nOption - 1] = nValue;
	}
	return 0;
}

int LuaBT_GetData(Lua_State* L)
{
	if (Lua_GetTopIndex(L) <= 0)
		Lua_PushNumber(L, 0);
	else
	{
		int nPlayerIndex = GetPlayerIndex(L);
		if (nPlayerIndex == -1)
			Lua_PushNumber(L, 0);
		else
		{
			int nOption = (int)Lua_ValueToNumber(L, 1);
			if ((nOption >= 1) && (nOption <= 48))
				Lua_PushNumber(L, Player[nPlayerIndex].BonusType[nOption - 1]);
			else
				Lua_PushNumber(L, 0);
		}
	}

	return 1;
}

int LuaBT_SetGameData(Lua_State* L)
{
	if (Lua_GetTopIndex(L) <= 1)
		return 0;

	int nSubWorldIndex = GetSubWorldIndex(L);
	if (nSubWorldIndex < 0 || nSubWorldIndex >= MAX_SUBWORLD)
		return 0;

	int nOption = (int)Lua_ValueToNumber(L, 1);
	int nValue = (int)Lua_ValueToNumber(L, 2);
	if ((nOption >= 1) && (nOption <= 14))
	{
		SubWorld[nSubWorldIndex].GameData[nOption - 1] = nValue;
	}

	return 0;
}

int LuaBT_GetGameData(Lua_State* L)
{
	if (Lua_GetTopIndex(L) <= 0)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}

	int nSubWorldIndex = GetSubWorldIndex(L);
	if (nSubWorldIndex < 0 || nSubWorldIndex >= MAX_SUBWORLD)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}

	int nOption = (int)Lua_ValueToNumber(L, 1);
	if ((nOption >= 1) && (nOption <= 14))
		Lua_PushNumber(L, SubWorld[nSubWorldIndex].GameData[nOption - 1]);
	else
		Lua_PushNumber(L, 0);

	return 1;
}









// 设置披风状态
int LuaSetPifeng(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	
	if (Lua_IsNumber(L,1))
	{
	//	Player[nPlayerIndex].m_ItemList.SetMaskLock(TRUE);
		Npc[Player[nPlayerIndex].m_nIndex].m_PifengType = (int)Lua_ValueToNumber(L,1);
	}
	return 0;
}

// 设置披风状态
int LuaSetChiBang(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	
	if (Lua_IsNumber(L,1))
	{
		//	Player[nPlayerIndex].m_ItemList.SetMaskLock(TRUE);
		Npc[Player[nPlayerIndex].m_nIndex].m_ChiBangType = (int)Lua_ValueToNumber(L,1);
	}
	return 0;
}
//恢复非披风状态
int LuaReSetPifeng(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	
//	Player[nPlayerIndex].m_ItemList.SetMaskLock(FALSE);
//	int nIdx = Player[nPlayerIndex].m_ItemList.GetEquipment(itempart_mask);
	Npc[Player[nPlayerIndex].m_nIndex].m_PifengType = 0;
	return 0;
}
//设定 杀死敌人获得经验 状态的持续时间
int LuaSetExpsubTime(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;

	int nSub = (int)Lua_ValueToNumber(L,1);  //倍率
	int nTime = (int)Lua_ValueToNumber(L,2) * 60 * 18;
	int nSkillIdx = (int)Lua_ValueToNumber(L,3);

	if (nSkillIdx<=0)
		nSkillIdx=440;

	KMagicAttrib sMa[1];
	sMa[0].nAttribType = magic_expenhance_p;  //技能的类型 ==杀死敌人获得经验==
	sMa[0].nValue[0] = nSub;   // 百分比值
	sMa[0].nValue[1] = 0;
	sMa[0].nValue[2] = 0;
	Npc[Player[nPlayerIndex].m_nIndex].SetStateSkillEffect(Player[nPlayerIndex].m_nIndex,nSkillIdx,1,sMa,1,nTime);   //372 为技能ID 1 为等级  时间
	return 0;
}

int LuaGetNpcExpRate(Lua_State * L)
{
   	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	
	Lua_PushNumber(L, Player[nPlayerIndex].m_nUpExp);
	
	return 0;
}


//是否有这个物品 相同的属性
int LuaHaveBaseItem(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
    char *nItemName=NULL;
	int nParam = (int)Lua_GetTopIndex(L);
	int nResult=0;
    if (Lua_IsNumber(L,1))
	{
	    int nGern = (int)Lua_ValueToNumber(L,1);
	    int nDetail = (int)Lua_ValueToNumber(L,2);
	    int nPar = (int)Lua_ValueToNumber(L,3);
	    int nSerise = 5,nLevel = 0;
     	if (nParam > 3)
		{ 
		   nSerise = (int)Lua_ValueToNumber(L,4);
		   nLevel = (int)Lua_ValueToNumber(L,5);
		} 
	    nResult = Player[nPlayerIndex].m_ItemList.IsItemExist(nGern,nDetail,nPar,nSerise,nLevel);
	}
	else if (Lua_IsString(L,1))
	{
        nItemName=(char *)Lua_ValueToString(L, 1); 
		nResult=Player[nPlayerIndex].m_ItemList.IsItemName(nItemName);
	}

	Lua_PushNumber(L, nResult);

	return 1;
}
//删除相同属性的物品
int LuaDelBaseItem(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	
	int nParam = (int)Lua_GetTopIndex(L);
	
	int nGern = (int)Lua_ValueToNumber(L,1);
	int nDetail = (int)Lua_ValueToNumber(L,2);
	int nPar = (int)Lua_ValueToNumber(L,3);
	
	int nSerise = 5,nLevel = 0;
	if (nParam > 3)
	{
		nSerise = (int)Lua_ValueToNumber(L,4);
		nLevel = (int)Lua_ValueToNumber(L,5);
	}
	
	int nResult = Player[nPlayerIndex].m_ItemList.DelExistItem(nGern,nDetail,nPar,nSerise,nLevel);
	
	Lua_PushNumber(L, (nResult != 0));
	
	return 1;
}

int LuaDelIdItem(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	
	int nId = (int)Lua_ValueToNumber(L,1);
	
	if (!nId)
		return 0;
	
	int nResult = Player[nPlayerIndex].m_ItemList.Remove(nId);
	ItemSet.Remove(nId);  //删除链表
	
	return 1;
}
//获得给予物品的属性
int LuaGetItemProp(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	
	int nId = (int)Lua_ValueToNumber(L,1);
	
	if (!nId)
		return 0;
	char nValue[128]={0};
	if (nId < 0 || nId >= MAX_ITEM) return 0;
	snprintf(nValue, sizeof(nValue),"%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",\
		Item[nId].GetGenre(),Item[nId].GetDetailType(),Item[nId].GetParticular(),\
		Item[nId].GetLevel(),Item[nId].GetSeries(),Item[nId].GetGoldId(),\
		Item[nId].GetPoint(),Item[nId].IsBlue(),Item[nId].IsRong(),\
		Item[nId].GetSkillType(),Item[nId].GetUsedRong(),Item[nId].GetStackNum(),\
		Item[nId].GetIsWhere(),Item[nId].GetIsBang(),Item[nId].GetIsKuaiJie(),Item[nId].GetEnChance());

	Lua_PushString(L,nValue);
	return 1;
}

int LuaGetItemSkillType(Lua_State * L)
{
	int nItemIdx = (int)Lua_ValueToNumber(L,1);
	if (nItemIdx<=0 || nItemIdx>=MAX_ITEM) return 0;

	Lua_PushNumber(L,Item[nItemIdx].GetSkillType());
	return 1;

}

int LuaGetItemType(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	
	int nId = (int)Lua_ValueToNumber(L,1);

	if (nId<=0 || nId>=MAX_ITEM) return 0;

	int nType=-1;

	if (!nId)
		return 0;

	if (Item[nId].IsBlue())
	    nType=1;
	else if	(Item[nId].IsPurple())
	{
		nType=2;
	}
	else if (Item[nId].GetGoldId()>0)
	{  
		if (Item[nId].IsPlatina())
		  nType=4;
		else
		  nType=3;
	}
	else
		nType=0;

	Lua_PushNumber(L,nType);
	return 1;
}

//获得给予物品的属性
int LuaGetItemPropInfo(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	int nId = (int)Lua_ValueToNumber(L,1);	
	if (!nId)
		return 0;
	char nMValueA[128];
	char nRValueB[128];
    int  nMGLevel[6];
    int  nRLLevel[6];
	for (int i = 0; i < 6; i ++)
	{
		if (nId < 0 || nId >= MAX_ITEM) return 0;
		nMGLevel[i] = Item[nId].GetItemParam()->nGeneratorLevel[i];
		nRLLevel[i] = Item[nId].GetItemParam()->nRGeneratorLevel[i];
	}
 int nItemClass,nDetailType,nParticularType,nLevel,nSeries,nLuck,nEnChance,nGoldId,nVersion,nGndrom,nPoint,nRPoint,nPrice,nIsBlue,SkillType;
	 if (nId < 0 || nId >= MAX_ITEM) return 0;
	 nItemClass		    = Item[nId].GetGenre();
	 nDetailType		= Item[nId].GetDetailType();
	 nParticularType	= Item[nId].GetParticular();
	 nLevel			    = Item[nId].GetLevel();
	 nSeries			= Item[nId].GetSeries();
	 nLuck			    = Item[nId].GetItemParam()->nLuck;
	 nEnChance		    = Item[nId].GetEnChance();
	 nGoldId			= Item[nId].GetGoldId();	
	 nVersion		    = Item[nId].GetItemParam()->nVersion;
	 nGndrom			= Item[nId].GetItemParam()->uRandomSeed;
	 nPoint			    = Item[nId].GetPoint();
	 nRPoint			= Item[nId].IsRong();
	 nPrice             = Item[nId].GetPrice();
//	int nPrice          = Item[nId].GetSetPrice();
	 nIsBlue            = Item[nId].IsBlue();
	 SkillType          = Item[nId].GetSkillType();
	sprintf(nMValueA,"%d,%d,%d,%d,%d,%d",nMGLevel[0],nMGLevel[1],nMGLevel[2],nMGLevel[3],nMGLevel[4],nMGLevel[5]);
	sprintf(nRValueB,"%d,%d,%d,%d,%d,%d",nRLLevel[0],nRLLevel[1],nRLLevel[2],nRLLevel[3],nRLLevel[4],nRLLevel[5]);	

	Lua_PushNumber(L,nItemClass);
	Lua_PushNumber(L,nDetailType);
	Lua_PushNumber(L,nParticularType);
	Lua_PushNumber(L,nLevel);
	Lua_PushNumber(L,nSeries);
	Lua_PushNumber(L,nLuck);
	Lua_PushNumber(L,nEnChance);
	Lua_PushNumber(L,nGoldId);
	Lua_PushNumber(L,nVersion);
	Lua_PushNumber(L,nGndrom);
    Lua_PushNumber(L,nPoint);
    Lua_PushNumber(L,nRPoint);
	Lua_PushNumber(L,nPrice);
	Lua_PushNumber(L,nIsBlue);
	Lua_PushString(L,nMValueA);
	Lua_PushString(L,nRValueB);
	Lua_PushNumber(L,SkillType);
	return 17;
}




//是否有这个技能属性
int LuaItemHaveMagic(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;

	int nNum = Lua_GetTopIndex(L);

	if (nNum < 8) 
	{
		Lua_PushNumber(L,0);
		return 0;
	}
	
	int nId = (int)Lua_ValueToNumber(L,1);
	int nAttribe = (int)Lua_ValueToNumber(L,2);
	int nValue1Min =  (int)Lua_ValueToNumber(L,3);
	int nValue1Max =  (int)Lua_ValueToNumber(L,4);
	int nValue2Min =  (int)Lua_ValueToNumber(L,5);
	int nValue2Max =  (int)Lua_ValueToNumber(L,6);
	int nValue3Min =  (int)Lua_ValueToNumber(L,7);
	int nValue3Max =  (int)Lua_ValueToNumber(L,8);
	
	if (!nId)
	{
		Lua_PushNumber(L,0);
		return 0;
	}
	
	if (nId < 0 || nId >= MAX_ITEM) { Lua_PushNumber(L,0); return 1; }
	int nResult = Item[nId].HaveMaigc(nAttribe,nValue1Min,nValue1Max,nValue2Min,nValue2Max,nValue3Min,nValue3Max);
	
	Lua_PushNumber(L,nResult);
	return 1;
}

//int LuaRemote(Lua_State* L)
//{
//	int nParnCount = (int)Lua_ValueToNumber(L, 3);
//
//	if (Lua_ValueToNumber(L, 3) <= 0)
//	{
//		return 0;
//	}
//
//	if (!Lua_IsTable(L, 4))
//	{
//		return 0;
//	}
//
//	char* pFile = (char*)Lua_ValueToString(L, 1);
//	if (!pFile)
//		return 0;
//
//	char* pFunc = (char*)Lua_ValueToString(L, 2);
//	if (!pFunc)
//		return 0;
//
//	RELAY_REMOTE pRemote;
//	pRemote.ProtocolFamily = pf_relay;
//	pRemote.ProtocolID = relay_c2c_remote;
//	strcpy(pRemote.szFilePath, pFile);
//	strcpy(pRemote.szFunction, pFunc);
//
//	for (int i = 0; i < nParnCount; i++)
//	{
//		char  pAnswer[64] = { 0 };
//		ZeroMemory(pAnswer, sizeof(pAnswer));
//		Lua_PushNumber(L, i + 1);
//		Lua_RawGet(L, 4);
//		char* pszString = (char*)Lua_ValueToString(L, Lua_GetTopIndex(L));
//		if (pszString)
//		{
//			g_StrCpyLen(pAnswer, pszString, 64);
//
//			if (i == 0)
//				pRemote.nParam[0] = atoi(pAnswer);
//			else if (i == 1)
//				pRemote.nParam[1] = atoi(pAnswer);
//			else if (i == 2)
//				pRemote.nParam[2] = atoi(pAnswer);
//			else if (i == 3)
//				strcpy(pRemote.szParam[0], pAnswer);
//			else if (i == 4)
//				pRemote.nParam[3] = atoi(pAnswer);
//			else if (i == 5)
//				strcpy(pRemote.szParam[1], pAnswer);
//		}
//	}
//
//	g_pTranClient->SendPackToServer((const void*)&pRemote, sizeof(RELAY_REMOTE));
//
//	return 0;
//}

int LuaRemote(Lua_State* L)
{
	char* szScript = (char*)Lua_ValueToString(L, 1);
	char* szFunc = (char*)Lua_ValueToString(L, 2);
	char* szValue = (char*)Lua_ValueToString(L, 3);
	char* szParam = (char*)Lua_ValueToString(L, 4);
	int nValue = (int)Lua_ValueToNumber(L, 5);

	if (!szScript || !szFunc)
		return 0;

	RELAY_REMOTE pRemote;
	pRemote.ProtocolFamily = pf_relay;
	pRemote.ProtocolID = relay_c2c_remote;
	g_StrCpyLen(pRemote.sScript, szScript, sizeof(pRemote.sScript));
	g_StrCpyLen(pRemote.sFunction, szFunc, sizeof(pRemote.sFunction));
	if (!szValue)
		strcpy(pRemote.sValue, "");
	else
		g_StrCpyLen(pRemote.sValue, szValue, sizeof(pRemote.sValue));
	if (!szParam)
		strcpy(pRemote.sParam, "");
	else
		g_StrCpyLen(pRemote.sParam, szParam, sizeof(pRemote.sParam));
	pRemote.nValue = nValue;

	if(g_pTranClient)
		g_pTranClient->SendPackToServer((const void*)&pRemote, sizeof(RELAY_REMOTE));

	return 0;
}

int LuaSetTmpCamp(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) 
		return 0;

	int nCamp = (int)Lua_ValueToNumber(L, 1);
	if (nCamp < 0)
		return 0;

	Npc[Player[nPlayerIndex].m_nIndex].SetTmpCamp(nCamp);

	return 0;
}

int LuaRestoreCampTong(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		return 0;

	int nCamp = Player[nPlayerIndex].m_cTong.m_nCamp;
	Npc[Player[nPlayerIndex].m_nIndex].SetCamp(nCamp);
	Npc[Player[nPlayerIndex].m_nIndex].SetCurrentCamp(nCamp);

	return 0;
}

int LuaGetMapDropRateFile(Lua_State* L)
{
	int nSubworldIdx = (int)Lua_ValueToNumber(L, 1);
	if (nSubworldIdx < 0 || nSubworldIdx >= MAX_SUBWORLD)
		return 0;

	Lua_PushString(L, SubWorld[nSubworldIdx].m_szNormalDropRateScript);

	return 0;
}
#endif




int LuaFindItemEx(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	
	if (nPlayerIndex <= 0)
	{
		Lua_PushNumber(L,0);
		return 1;
	}
	int nParamNum = Lua_GetTopIndex(L);

	if (nParamNum <= 2)
	{
		int nIndex, nItemGenre = -1, nDetail = -1, nParticur = -1, nLevel = -1, nSeries = series_num, nLuck = 0, nStackNum = 0;
		nIndex = (int)Lua_ValueToNumber(L, 1);
		if (nIndex > 0 && nIndex < MAX_ITEM)
		{
			nItemGenre = Item[nIndex].GetGenre();
			nDetail = Item[nIndex].GetDetailType();
			nParticur = Item[nIndex].GetParticular();
			nLevel = Item[nIndex].GetLevel();
			nSeries = Item[nIndex].GetSeries();
			nLuck = Item[nIndex].m_GeneratorParam.nLuck;
			nStackNum = Item[nIndex].GetStackNum();
		}

		Lua_PushNumber(L, nItemGenre);
		Lua_PushNumber(L, nDetail);
		Lua_PushNumber(L, nParticur);
		Lua_PushNumber(L, nLevel);
		Lua_PushNumber(L, nSeries);
		Lua_PushNumber(L, nLuck);
		Lua_PushNumber(L, nStackNum);
		return 7;
	}

	int nIndex = 0, nItemGenre = -1, nDetail = -1, nParticur = -1, nLevel = -1, nSeries = series_num, nLuck = 0, nStackNum = 0;
	if (nParamNum > 2)
	{
		nIndex = Player[nPlayerIndex].m_ItemList.PositionToIndex((int)Lua_ValueToNumber(L, 1), (int)Lua_ValueToNumber(L, 2));
	}
	if (nParamNum > 3)
	{
		nIndex = Player[nPlayerIndex].m_ItemList.m_Room[PositionToRoom((int)Lua_ValueToNumber(L, 1))].FindItem((int)Lua_ValueToNumber(L, 2), (int)Lua_ValueToNumber(L, 3));
	}

	if (nIndex > 0)
	{
		nItemGenre = Item[nIndex].GetGenre();
		nDetail = Item[nIndex].GetDetailType();
		nParticur = Item[nIndex].GetParticular();
		nLevel = Item[nIndex].GetLevel();
		nSeries = Item[nIndex].GetSeries();
		nLuck			= Item[nIndex].m_GeneratorParam.nLuck;
		nStackNum = Item[nIndex].GetStackNum();
	}
	Lua_PushNumber(L, nIndex);
	Lua_PushNumber(L, nItemGenre);
	Lua_PushNumber(L, nDetail);
	Lua_PushNumber(L, nParticur);
	Lua_PushNumber(L, nLevel);
	Lua_PushNumber(L, nSeries);
	Lua_PushNumber(L, nLuck);
	Lua_PushNumber(L, nStackNum);
	return 8;
}





int LuaPlayMusic(Lua_State * L)//PlayMusic(musicname,loop=1, vol );
{
	if (Lua_GetTopIndex(L) < 1) 
		return 0;
	
	int nPlayerIndex=0;
	
#ifdef _SERVER
	nPlayerIndex = GetPlayerIndex(L);
#else	
	nPlayerIndex = CLIENT_PLAYER_INDEX;
#endif

	//int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0) return 0;
	
	PLAYER_SCRIPTACTION_SYNC UiInfo;
	ZeroStruct(UiInfo);
	UiInfo.m_bUIId = UI_PLAYMUSIC;
	UiInfo.m_bOptionNum = 1;
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
//	Player[nPlayerIndex].m_nCheckWaiGua =0;
	int nMsgId = 0;
	
	g_StrCpyLen(UiInfo.m_pContent, Lua_ValueToString(L,1), sizeof(UiInfo.m_pContent));
	UiInfo.m_nBufferLen = strlen(((char *)UiInfo.m_pContent));
	UiInfo.m_bParam1 = 0;
	
#ifndef _SERVER
	UiInfo.m_bParam2 = 0;
#else
	UiInfo.m_bParam2 = 1;
#endif
	
	Player[nPlayerIndex].DoScriptAction(&UiInfo);
	return 0;
}

int LuaFadeInMusic(Lua_State * L)
{
	return 0;
}

int LuaFadeOutMusic(Lua_State * L)
{
	return 0;
}

//===========================客户端脚本函数========================
#ifndef _SERVER
int LuaPlaySound(Lua_State * L)
{
	return 0;
}

/*
int Luadostring(Lua_State * L)
{
    char *Fun=(char *)Lua_ValueToString(L,1);
	lua_dostring(L,Fun);
	return 0;
}
*/

int LuaNewTask_AddNpcFindPath(Lua_State * L)
{
    int nTaskGenre = (int)Lua_ValueToNumber(L,1);
    int nMsgIndex = (int)Lua_ValueToNumber(L,2);
	int nTaskMapId = (int)Lua_ValueToNumber(L,3);
	int nMapXpos = (int)Lua_ValueToNumber(L,4);
	int nMapYpos = (int)Lua_ValueToNumber(L,5);
	// Player[CLIENT_PLAYER_INDEX].m_cTask.nTaskGenre=nTaskGenre;
	//sprintf(Player[CLIENT_PLAYER_INDEX].m_cTask.nTaskDesc,nTaskDesc);
	KTaskPathInfo nInfo;
        nInfo.nTaskType=nTaskGenre;
        nInfo.nMsgIndex=nMsgIndex;
		nInfo.nMapIdx=nTaskMapId;
		nInfo.nMapXpos=nMapXpos;
		nInfo.nMapYpos=nMapYpos;
	CoreDataChanged(GDCNI_TASK_INFO,(unsigned int)&nInfo,1);
   return 0;
}

int LuaNewTask_SetFinishedTask(Lua_State * L)
{
	return 0;
}

int LuaNewTask_DetailTextOut(Lua_State * L)
{	
    int nTaskGenre = (int)Lua_ValueToNumber(L,1);
    char *nTaskDesc=(char *)Lua_ValueToString(L,2);
	
	// Player[CLIENT_PLAYER_INDEX].m_cTask.nTaskGenre=nTaskGenre;
	//sprintf(Player[CLIENT_PLAYER_INDEX].m_cTask.nTaskDesc,nTaskDesc);
	KNewsMessage News;
	//主信息为字符串 
	//g_StrCpyLen(News.sMsg, nTaskDesc, sizeof(News.sMsg));
	News.nType =	nTaskGenre;
	g_StrCpyLen(News.sMsg,nTaskDesc,sizeof(News.sMsg));
	// News.nMsgLen = TEncodeText(News.sMsg, strlen(News.sMsg));
	//SYSTEMTIME systime;
	//memset(&systime, 0, sizeof(SYSTEMTIME));
	CoreDataChanged(GDCNI_TASK_INFO,(unsigned int)&News,0);//;(unsigned int)&News
	return 0;
}

int LuaNewTask_TaskTextOut(Lua_State * L)
{	
    int nTaskGenre = (int)Lua_ValueToNumber(L,1);
    char *nTaskDesc=(char *)Lua_ValueToString(L,2);

  // Player[CLIENT_PLAYER_INDEX].m_cTask.nTaskGenre=nTaskGenre;
   //sprintf(Player[CLIENT_PLAYER_INDEX].m_cTask.nTaskDesc,nTaskDesc);
   KNewsMessage News;
   //主信息为字符串 
   //g_StrCpyLen(News.sMsg, nTaskDesc, sizeof(News.sMsg));
   News.nType =	nTaskGenre;
   g_StrCpyLen(News.sMsg,nTaskDesc,sizeof(News.sMsg));
  // News.nMsgLen = TEncodeText(News.sMsg, strlen(News.sMsg));
   //SYSTEMTIME systime;
   //memset(&systime, 0, sizeof(SYSTEMTIME));
   CoreDataChanged(GDCNI_TASK_INFO,(unsigned int)&News,0);//;(unsigned int)&News
   return 0;
}

int LuaNewTask_AddIcon(Lua_State * L)
{ 
	int nTaskType = (int)Lua_ValueToNumber(L, 1);
    char *nTaskIcocPath = (char *)Lua_ValueToString(L, 2);
	int nTaskIdx = (int)Lua_ValueToNumber(L, 3);
	char *nTaskName = (char *)Lua_ValueToString(L, 4);
	if (nTaskIdx>20 || nTaskIdx<0)
		return 0;

	Player[CLIENT_PLAYER_INDEX].m_cTask.nTaskInfo[nTaskIdx].nTaskType=nTaskType;
    Player[CLIENT_PLAYER_INDEX].m_cTask.nTaskInfo[nTaskIdx].nTaskidx=nTaskIdx;
	g_StrCpyLen(Player[CLIENT_PLAYER_INDEX].m_cTask.nTaskInfo[nTaskIdx].nTaskIconPath, nTaskIcocPath, sizeof(Player[CLIENT_PLAYER_INDEX].m_cTask.nTaskInfo[nTaskIdx].nTaskIconPath));
	g_StrCpyLen(Player[CLIENT_PLAYER_INDEX].m_cTask.nTaskInfo[nTaskIdx].nTaskName, nTaskName, sizeof(Player[CLIENT_PLAYER_INDEX].m_cTask.nTaskInfo[nTaskIdx].nTaskName));

    Lua_PushNumber(L,1);
	return 1;
}
//客户端执行自动封挂脚本
int LuaAutoForbitGua(Lua_State * L)
{
	//int nPlayerIndex = GetPlayerIndex(L);
	int nFReg=0;
 //////////////////检测外挂程序/////////////////////////
	int IsForBit[2];  //默认开启封挂
	g_GameSetTing.GetInt2("SYSTEM","IsForBitGua",IsForBit);
	
	int nMun=0;
	int wgRows=g_ForbitWaiGua.GetHeight()+1;

	for (int i=2;i<wgRows;++i)
	{
		char nKey[16],nReg=0;
		g_ForbitWaiGua.GetString(i+2,"GuaNameKey","无",nKey,sizeof(nKey));
		nReg= g_ForBitGua.GetProcessList(nKey,wgRows,i);//CheckWaiGua(nKey);
		if (nReg>=1)
		{//检测到外挂 

			nFReg=1;
			break;
		}
		else if (nReg<0)  
		{//提权失败
			nFReg=-1;
			break;
		}
		else  if (nReg==0)
		{//没有外挂
			nMun++;
//			break;
		}
		g_ForBitGua.Colse();
	}
	if (nMun==wgRows) //行数相等通过外挂检测
	{//同步外挂参数给服务器端
		SKILL_LEFT_SYNC	sLeftSkill;
		sLeftSkill.ProtocolType = c2s_skillsync;
		sLeftSkill.m_nLeftskill =0;
		sLeftSkill.m_Type=1;
		
		if (g_pClient)
		g_pClient->SendPackToServer(&sLeftSkill, sizeof(SKILL_LEFT_SYNC));
		nFReg=0;
	} 
    Lua_PushNumber(L,nFReg);
	return 1;
}

//瞬间技能特效（类型0 自己释放 1为指定索引NPC释放）
int LuaCastSkill(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 1)
		return 0;	
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex<0) return 0;
	int nIndexID=0;
	int nStateID=0;
    int nType = (int)Lua_ValueToNumber(L, 1);
	nIndexID = (int)Lua_ValueToNumber(L, 2);
	nStateID = (int)Lua_ValueToNumber(L, 3);
	if (nType==0)
        Npc[Player[nPlayerIndex].m_nIndex].SetInstantSpr(nStateID); 
    else if (nType==1)
    {
    	if (nIndexID < 0 || nIndexID >= MAX_NPC) return 0;
		Npc[nIndexID].SetInstantSpr(nStateID);     //释放一个瞬间特效
    }
	
	return 0;    
}

int LuaPlaySprMovie(Lua_State * L)
{
	return 0;
}
#endif
//==============================================================
TLua_Funcs GameScriptFuns[] = 
{
	SCRIPTFUNS_V9_TABLE_ENTRIES,
    // V9 tranche rows (round 135): 8 Lua name(s) whose key is free, whose symbol is
    // new to the tree, and whose unit compiles. The other 67 staged row(s) are held;
    // run _mod/fix_round135.py --dry for the reason on each one.
	//通用指令	
	{"Say", LuaSayUI},
	{"SayNew",LuaSayNew},
	{"Describe",LuaSayNew},
	{"CreateTaskSay",LuaCreateTaskSay},	 //支持表格
	{"CreateNewSayEx",LuaCreateNewSayEx},
	{"CreateSprSay",LuaCreateSprSay},

	{"Talk", LuaTalkUI},
	{"new_Talk", LuaNewTalkUI},
	{"GetTaskTemp", LuaGetTempTaskValue},
	{"SetTaskTemp", LuaSetTempTaskValue},
	{"print", LuaMessage},	  //print	 printf
	{"Mathematical", LuaMathematical},

	{"MATH_Random",	LuaMATH_Random},
	{"MATH_RandomC", LuaMATH_RandomC},
	{"MATH_GetBit",	LuaMATH_GetBit},
	{"MATH_SetBit",	LuaMATH_SetBit},

	{"GetBit",	LuaGetBit},
	{"GetByte",	LuaGetByte},
	{"SetBit",	LuaSetBit},
	{"SetByte",	LuaSetByte},
	{"Include",LuaIncludeFile},
	{"IncludeLib",LuaIncludeLib},
	{"IL",LuaIncludeLib},
	{"PutMessage", LuaSendMessageInfo},
	{"AddGlobalTimeNews",LuaAddGlobalTimeNews},
	//所有gs广播
	{"AddGlobalNews",LuaAddGlobalNews},
	{"AddGlobalCountNews",LuaAddGlobalCountNews},

	{"AddGlobalNews_All",LuaAddGlobalNews_All},
	{"AddGlobalCountNews_All",LuaAddGlobalCountNews_All},
	{"AddLocalNews_All",LuaAddLocalNews_All},
	//本地gs广播
	{"AddLocalNews",LuaAddLocalNews},
	{"AddSysNews"  ,LuaAddSysNews},
	{"AddLocalTimeNews",LuaAddLocalTimeNews},
	{"AddLocalCountNews",LuaAddLocalCountNews},

	{"ServerExpLimit",	LuaServerExpLimit},


	{"GetTask",			LuaGetTaskValue},	    //GetTask(任务号):获得当前玩家该任务号的值
	{"SetTask",			LuaSetTaskValue},	    //SetTask(任务号,值):设置任务值
	{"SetDeathSkill",	LuaSetDeathSkill},	    //SetTask(任务号,值):设置任务值
	{"GetDeathSkill",	LuaGetDeathSkill},
	{"AddTask",			LuaAddTaskValue},	    //
	{"GetTabFileInfo",  LuaGetTabFileInfo},     //获取TXT文件 某行 某列 的内容 ，返回字符串类型  3个参数
	{"GetIniFileInfo",  LuaGetIniFileInfo},     //获取ini文件 某节 某键 的内容
	
	{"GetTabRowAndCol", LuaGetTabRowAndCol},    //获取文件的 总行数和列数   1个参数
	
	{"IniFile_CheckSection", LuaCheckIniFieSec},
	{"IniFile_GetData",  LuaGetIniFileInfo},
	{"TaskAwardMatrix", LuaGetTaskAwardMatrix}, //获取任务 同一个名称的 有多少行
    {"CalcFreeItemCellCount", LuaCalcFreeItemCellCount},
	{"GetPositionCount",        LuaGetPositionCount},

	{"GetLocalDateEx", LuaGetTimeByMiao},
	{"GetLocalDate", LuaGetLocalDate},
	{"GetLocalDateBys2s", LuaGetLocalDateBys2s},
	{"GetTimeNow",		LuaGetTimeNow},

	{"GetDataInt",			LuaGetDataInt},
	{"GetDataStr",			LuaGetDataStr},
	{"SetData",				LuaSetData},

	{"AddDataGr",			LuaAddDataGr},
	{"SetDataGr",			LuaSetDataGr},
	{"GetDataGr",			LuaGetDataGr},
	{"FindDataId",			LuaFindDataId},

	{"SaveDataFile",		LuaSaveDataFile},
	{"GetLeagueObj",		LuaGetLeagueObj},
	{"GetLeagueObjCount",	LuaGetNumGroup},
	{"SetLeagueObj",		Lua_SetLeagueObj},
	{"AddLeagueMemObj",		Lua_AddMemberToGroupByName},
	{"AddLeagueObj",		LuaAddLeagueObj},

	{"SetLeagueObjName",	LuaSetLienDauName},
	{"GetLeagueObjName",	LuaGetLienDauName},

	{"GetLeagueMemObj",		LuaGetLeagueMemObj},

	{"FindLeagueObj",		LuaFindLeagueObj},

	{"IsTextPass",			LuaIsTextPass},


	{"IniFile_Load",		LuaIniFile_Load},
	{"IniFile_SetData",		LuaIniFile_SetData},
	{"IniFile_GetData",		LuaIniFile_GetData},
	{"IniFile_ClearData",	LuaIniFile_ClearData},
	{"File_Create",			LuaFile_Create},



	{"OpenJinDuTiao", LuaOpenJinDuTiao},
	{"PaceBar", LuaPaceBar},

	{"SetRoomNull",	LuaSetRoomNull},
	{"FileName2Id",	LuaFileName2Id},

	{"TabFile_Load",             LuaTabFile_Load},         //加载TXT文件
	{"TabFile_GetCell",          LuaTabFile_GetCell},
	{"TabFile_GetColCount",      LuaTabFile_GetColCount},
	{"TabFile_GetRowCount",      LuaTabFile_GetRowCount},
	{"TabFile_UnLoad",           LuaTabFile_UnLoad},
	{"GetRandomSeed",           LuaGetRandomSeed},
	{"GetRandomNum",           LuaGetRandomNum},
	{"NewRandom",           LuaNewRandom},
	{"SetRandomSeed",       LuaSetRandomSeed},
	 
	{"GetWorldPos",		LuaGetNewWorldPos},	//W,X,Y = GetWorldPos()返回于NewWorld配陪的坐标
	{"GetLevel",		LuaGetLevel},			        //GetLevel()GetPlayers Level
	{"SetNpcLevel",		LuaSetNpcLevel},
	{"GetLastFactionNumber", LuaGetLastFactionNumber},
	{"GetFactionCamp",	LuaGetPlayerFactionCamp},
	{"GetFactionNo",	LuaGetPlayerFactionNo}, 
	{"GetFaction",		LuaGetPlayerFaction},    //GetFaction()获得玩家的门派名
	{"GetLastAddFaction",LuaGetPlayerFaction},	
	{"GetMaskActionSpr", LuaGetMaskActionSpr},

	{"CalcEquiproomItemCount", LuaCalcEquiproomItemCount},
	{"SetPos",			LuaSetPos},			//SetPos(x,y)进入某点
	{"GetLastDiagNpc",			LuaGetLastDiagNpc},	
	
	//服务器端脚本指令
#ifdef _SERVER
	
	//-----------------基本功能
	{"SyncNpc",		  LuaSyncNpc},
	{"SyncTaskValue", LuaSyncTaskValue},
	{"AddRepute", LuaSetPlayerReputeValue},	//修改声望值
	{"GetRepute",LuaGetPlayerReputeValue},	
	{"AddReBorn", LuaSetPlayerReBornValue},
	{"GetReBorn",LuaGetPlayerReBornValue},
	{"ST_GetTransLifeCount",LuaGetPlayerReBornValue},
	{"GetReBornNum",LuaGetPlayerReBornNum}, //阶次
	{"GetReBornCount",LuaGetPlayerReBornCount}, //转生次数

	//-------------会员--------------------
	{"SetRoleVip", LuaSetPlayerSetRoleVipValue},
	{"AddRoleVip", LuaSetPlayerRoleVipValue},
	{"GetRoleVip",LuaGetPlayerRoleVipValue},
	//{"ST_GetRoleVipCount",LuaGetPlayerRoleVipValue},
	{"GetRoleVipNum",LuaGetPlayerRoleVipNum},

	{"AddFuYuan", LuaSetPlayerFuYuanValue},  //富源
	{"GetFuYuan",LuaGetPlayerFuYuanValue},
	{"GetDiemTongKim",LuaGetDiemTongKimValue},
	{"AddDiemTongKim",LuaAddDiemTongKimValue},
	{"GetDiemVinhDu",LuaGetDiemVinhDuValue},
	{"AddDiemVinhDu",LuaAddDiemVinhDuValue},


	{"SetSubTimer",     LuaSetSubTimer},
	{"SetTimer",		LuaSetTimer},		//SetTimer(时间量, 时间TaskId):给玩家打开计时器,时间到时将自动调用OnTimer函数
	{"StopTimer",		LuaStopTimer},		//StopTimer()：关闭当前玩家的计时器
	{"GetRestTime",		LuaGetRestTime},	//GetRestTime:获得计时器将触发的剩于时间	
	{"GetGameTime",		LuaGetGameTime},	//GetRestTime:获得计时器将触发的剩于时间
	{"GetTimerId",		LuaGetCurTimerId},	//CurTimerId = GetTimerId():获得当前执行的计时器的id,如果没有则返回0
///////////////队伍相关/////////////////////////

    {"ChangeTeamFeature",LuaChangeTeamFeature},
	{"IsPlayer",	    LuaIsPlayer},
	{"getCreatRoleTime",LuagetCreatRoleTime},
	{"IsCaptain",		LuaIsLeader},		//IsCaptain()是否为队长
	{"GetCaptainName",	LuaGetCaptainName},
//	{"GetMenberName",	LuaGetMenberName},
	{"GetTeamMem",		LuaGetTeamMem},
	{"GetTeamMember",	LuaGetTeamMember},
	{"GetTeam",			LuaGetTeamId},		//GetTeam()返回玩家队伍ID
	{"GetTeamSize",		LuaGetTeamSize},	//GetTeamSize() //h获取队伍的人数
	{"LeaveTeam",		LuaLeaveTeam},		//LeaveTeam()让玩家离开自身队伍
	{"Msg2Player",		LuaMsgToPlayer	},	//Msg2Player(消息)

	{"new_Msg2Player",LuaNew_Msg2Player},	//Msg2Player(消息)

	{"Msg2Waigua",		LuaMsgToPlayera	},	//Msg2Player(消息)
	{"Msg2Team",		LuaMsgToTeam},		//Msg2Team(消息)通知玩家的组
	{"Msg2OneTeam",     LuaMsgToOneTeam},   //给某个队伍发信息
	{"SetCreateTeam",   LuaSetCreateTeamOption},
//------------------地图相关------------------------
    {"setMapLimitLevel",	   LuaSetMapLimitLevel}, 
    {"setMapLimitLevelByIndex",	   LuaSetMapLimitLevelByIndex},

	{"getMapLimitLevel",	   LuaGetMapLimitLevel}, 
	{"getMapLimitLevelByIndex",	   LuaGetMapLimitLevelByIndex},

	{"setMapNoCostByIndex",	   LuaSetMapNoCostByIndex},
	{"setMapNoCost",	   LuaSetMapNoCost}, 

	{"setMapNoLostItemByIndex",	   LuaSetMapNoLostItemByIndex},
	{"setMapNoLostItem",	   LuaSetMapNoLostItem}, 

	{"setMapNoLostMoneyByIndex",	   LuaSetMapNoLostMoneyByIndex},
	{"setMapNoLLostMoney",	   LuaSetMapNoLostMoney}, 

	{"setMapNoDamageByIndex",	   LuaSetMapNoDamageByIndex},   //设置该地图人物无敌
	{"setMapNoDamage",	   LuaSetMapNoDamage},    //设置该地图人物无敌

    {"setMapNoSeriesByIndex",	   LuaSetMapNoSeriesByIndex},    //设置该地图无五行相克
	{"setMapNoSeries",	   LuaSetMapNoSeries},    //设置该地图无五行相克

    {"showMapRegInfo",	   LuaShowMapRegInfo},
    {"setSubWorldSuoHun",LuaSetSubWorldSuoHun},//设置锁魂
	{"getActivityCountInfo",LuaGetActivityCount},
    {"showActivityCount",LuaShowDBactivityCount},

	{"setSubWorldMaster",LuaSetSubWorldMaster},
	{"setSubWorldMaster_t",LuaSetSubWorldMaster_t},
	{"getSubWorldMaster",LuaGetSubWorldMaster},
	{"getSubWorldMaster_t",LuaGetSubWorldMaster_t},
	{"getSubWorldMasterCount",LuaGetSubWorldMasterCount},

	{"CheckMapNoReturn",LuaCheckMapNoReturn},	  //检测该地图是否可以返回
	{"Msg2SubWorld",	LuaMsgToSubWorld},	      //Msg2SubWorld(消息)通知世界
	{"Msg2SubWorld_All",LuaMsgToSubWorld_All},
	
	{"getSerVerInfo",LuaGetSerVerInfo},


	{"MsgToSubWorldMap",LuaMsgToSubWorldMap}, //向某个地图发送消息
	{"MsgToSubSerVerWorld",LuaMsgToSubSerVerWorld},
    {"SetSubSerGlobalMission",LuaSetSubSerGlobalMission},
	{"KickOutPlayerInGame",LuaKickOutPlayerInGame},
	{"KickOutPlayerThisGame",LuaKickOutPlayerThisGame},
	
	{"NpcSayToSubWorld",LuaNpcSayToSubWorld},
	{"NpcAutoChat",     LuaNpcChat},
	{"LoadActivityMap", LuaLoadActivityMap},
	{"Msg2Region",		LuaMsgToAroundRegion}, //Msg2Region(消息)通知周围Region
	{"Msg2GM",			LuaMsgToGameMaster},    //Msg2GM(StrInfo)
	{"Msg2IP",			LuaMsgToIP},           //Msg2IP(IP, ID, StrInfo)
	{"GetInfo",			LuaGetPlayerInfo},     //str = GetInfo()
	{"GetIP",			LuaGetIP},
	{"GetIpByPId",		LuaGetPlayerIP},	
//	{"SetPos",			LuaSetPos},			//SetPos(x,y)进入某点	
	{"GetPos",			LuaGetPos},			//GetPos() return x,y,subworldindex,Indexid
	{"GetNpcPos",			LuaGetPos},
	
//	{"GetWorldPos",		LuaGetNewWorldPos},	//W,X,Y = GetWorldPos()返回于NewWorld配陪的坐标

    { "ST_SyncMiniMapObj",    LuaST_SyncMiniMapObj}, //小地图寻路
	{"NewWorld",		LuaEnterNewWorld},
	{"SetTransWorld",	LuaEnterTransWorld},//设置转服
	{"ForceTransWorld",	LuaForceTransWorld},//强行转服
	
	{"GetTransSerInfo",	LuaGetTransSerInfo},
	{"RodomWorld",		LuaEnterRodomWorld},
	{"GetAroundNpcList",LuaGetAroundNpcList},//获取周围的数量 返回一个表
	{"SetOrAddTrap",    LuaSetOrAddTrap},    //某地图 某坐标 新增加 或 修改 一个TRAP点

	{"AddTrap",			LuaAddTrap},
	{"AddObjNew",		LuaAddObjNew},
	{"DelObjNew",		LuaDelObjNew},
//------------------拍卖行系统-------------------------
    {"AddShopItem",          LuaAddShopItem},            //增加装备到数据库
	{"ClearShopItemData",    LuaClearShopItemData},      //清除数据
	{"GetShopItemData",      LuaGetShopItemData},        //取回装备
	{"GetShopItemDataByName",LuaGetShopItemDataByName},  //查询
	{"GetPaiCountByRoleName",LuaGetPaiCountByRoleName},  //获取角色已经拍卖的装备数量
	{"GetMoneyFromPaiShop",LuaGetMoneyFromPaiShop},      //兑换所有的收入
//------------------物品相关-------------------------
   	{"ClearAllItem",          LuaClearAllItem},
	{"SetChiBangRes",          LuaSetChiBangRes},
	{"GetChiBangRes",          LuaGetChiBangRes},

	{"GetItemParam",           LuaGetItemMagicLevel},
	{"SetItemParam",           LuaSetItemMagicLevel},
	{"CheckItemReqability",    LuaCheckItemReqability},
	{"ChangeItemLevel",        LuaChangeItemLevel},
	{"SetItemWings",           LuaGiveItemWings},

    {"SetItemRes",              LuaSetItemRes},
	//{"GetPositionCount",        LuaGetPositionCount},
    {"GetEquipCount",           LuaCountEquipPlayer},        //获取玩家有多少件装备在身上带着
	{"GetItemCountWithName",    LuaGetItemCountWithName},
	{"DelItemWithName",    LuaDelItemWithName},	             //删除所有包袱某个名称的东西

	{"AddItem",			LuaAddItem},	  //AddItem(nItemClass, nDetailType, nParticualrType, nLevel, nSeries, nLuck, nItemLevel..6)
	{"AddItemEx",		LuaAddItemEx},
	{"AddEventItem",	LuaAddEventItem},
	{"AddMineItem",		LuaAddMineItem},
	{"AddGoldItem",		LuaAddGoldItem},
	{"AddGoldItemID",	LuaAddGoldItemID},
	{"AddGoldItemEx",	LuaAddGoldItemEx},
	
	{"AddPlaTinaItem",	LuaAddPlaTinaItem},
	{"AddFusionItem",	LuaAddFusionItem},
	{"DuanZaoItem",	    LuaDuanZaoItem},

	{"playerAddItem",	LuaPlayerAddItem},

	{"DropZhiDingItem",	LuaDropZhiDingItem},
    {"TestDropItem",	LuaTestDropItem},
	{"DropItem",		LuaDropItem},		     //掉下DropItem(NpcId, 物品id或物品名)
	{"DropItem_map",	LuaDropItem_map},		

	{"DropEquipItem",	LuaDropEquipItem},
	{"ITEM_DropRateItem",LuaITEM_DropRateItem},
	{"DropOneItem",		LuaDropOneItem},	     //掉下某个装备(随机 蓝装 或 黄金装)

	{"ITEM_GiveOneItem",LuaITEM_DropOneItem},	 //给玩家某个物品


	{"SetItemMapIdx",	LuaSetItemMapIdx},
	{"GetItemMapIdx",	LuaGetItemMapIdx},

	{"DelTrenTay",	LuaRemoveAllImmediacy},
	{"DelAllHanhTrang",	LuaRemoveAllEquiproom},

	{"DelItem",			LuaDelItem},
	{"DelItemEx",		LuaDelItemEx},
	{"DelGoldItem",		LuaDelGoldItem},
	{"DelMineItem",		LuaDelMineItem},
//	{"AddStackNumItem",		LuaAddStackNumItem},
//	{"DelStackNumItem",		LuaDelStackNumItem},
    {"SetGuiShuName",	LuaSetGuiShuName},     //设定物品的归属
    {"GetItemLevel",    LuaGetItemLevel},      //获取物品的等级
	{"GetItemName",     LuaGetItemName},       //获取物品的姓名
	{"GetItemSeries",   LuaGetItemSeries},
	{"GetItemStackNum", LuaGetItemStackNum},
	{"GetItemEnChance", LuaGetItemEnChance},
	{"GetItemIsBang",   LuaGetItemIsBang},
	{"GetItemUseTime",  LuaGetItemTime},
	{"GetItemBaseInfo", LuaGetItemBaseInfo},
	{"GetItemPoint"   , LuaGetItemPoint},
	{"GetItemRpoint",	LuaGetItemRpoint},
	{"GetItemGoldIdx", LuaGetItemGoldIdx},
	{"SetItemRandomIdx", LuaSetItemRandomSeed},
	{"GetItemRandomIdx", LuaGetItemRandomSeed},
	{"SetItemRandomEx", LuaSetItemRandomEx},
	{"GetItemRandomEx", LuaGetItemRandomEx},
	{"GetItemIsStack", LuaGetItemIsStack},
	//{"GetItemSeries", LuaGetItemIsStack IsStack()
	
	{"GetItemLuck", LuaGetItemLuck},
	{"GetItemWhere", LuaGetItemWhere},
	{"GetItemPlatinaIdx", LuaGetItemPlatinaIdx},

	{"SetItemPoint", LuaSetItemPoint},
	{"SetItemEnChance", LuaSetItemEnChance},
	
//	{"GetItemInfo",     LuaGetItemInfo},
	{"HaveItem",		LuaHaveItem},
	{"GetItemCountEx",	LuaGetItemCountEx},
	{"HaveMineItem",	LuaHaveMineItem},
	{"HaveGoldItem",	LuaHaveGoldItem},

	{"GetAllItemCount",	LuaGetAllItemCount},  //获取玩家所有物品的数量
	{"GetItemNameCount",	LuaGetTaskItemCount}, //GetItemCount(TaskItemName Or TaskItemId)
	{"GetMineItemCount",LuaGetMineItemCount}, //GetItemCount(TaskItemName Or TaskItemId)
	// CalcItemCount( roomtype, nItemGenre, nItemDetailType, nItemParticular, nItemLevel, nSeries, nQuality )
	
	{"CalcItemCount", LuaCalcItemCount},
	{"ConsumeItem", LuaConsumeItem},
	{"GetItemCount",		LuaGetItemCount},
	{"DelItemCount",		LuaDelItemCount},

	{"getIsMagic",	LuaGetIsMagic},
	{"setIsMagic",	LuaSetIsMagic},

    {"SetCurDurability",	LuaSetCurDurability},
	{"GetCurDurability",	LuaGetCurDurability},
	{"GetMaxDurability",LuaGetMaxDurability},
	

    {"SetAndSycItem",	LuaSetAndSycItem},    //同步物品信息---//设置是否绑定 快捷 五行
    {"SetSpecItemParam",	LuaSetSpecItemParam},
    {"GetSpecItemParam",	LuaGetSpecItemParam},
    {"GetItemParam",	LuaGetSpecItemParam},
	{"SetItemMagicAttrib",LuaSetItemMagicAttrib},
	{"GetItemMagicAttribLevel",LuaSetItemMagicAttribLevel}, 

	{"GetMagicAttribRowInfo",LuaGetMagicAttribRowInfo}, 

	//熔炼纹纲
	{"SetItemRmagicAttrib",LuaSetItemRmagicAttrib},
	{"GetItemRmagicAttribLevel",LuaGetItemRmagicAttribLevel}, 

	{"GetItemRmagicAttrib",LuaGetItemRmagicAttrib},      //获取熔炼后的属性信息
	{"GetItemMagicAttrib",LuaGetItemMagicAttrib},        //魔法属性获取某个物品的 某个属性值
	{"GetItemBaseAttrib",LuaGetItemBaseAttrib},          //基本属性获取某个物品的 某个属性值
	{"GetItemReqAttrib",LuaGetItemReqAttrib},            //需求属性获取某个物品的 某个属性值　m_aryRequireAttrib

	{"SetItemMagicLevel",        LuaSetItemMagicLevel},  //设置物品的属性值 
	{"GetItemMagicLevel",        LuaGetItemMagicLevel},  //获取物品的基本属性值

	{"SetNpcItemMagicLevel",        LuaSetNpcItemMagicLevel}, 
	{"SetMagicAttrib",        LuaSetMagicAttrib}, 
	{"PlaceItem",        LuaPlaceItem}, 
	{"GetItemIDByIndex",    LuaGetItemIDByIndex},
    {"GetItemIndexByID",    LuaGetItemIndexByID},
	
	{"HaveCommonItem",	LuaHaveBaseItem},	  //包袱是否有这个物品 返回索引
	{"HaveBaseItem",	LuaHaveBaseItem},	  //包袱是否有这个物品 返回索引
	{"DelBaseItem",		LuaDelBaseItem},	  //删除是否有这个物品

	{"DelIdItem",		LuaDelIdItem},        // DelIdItem(Id) Id 给与界面返回的Id
	{"GetItemProp",		LuaGetItemProp},      // Genre,Detail,Par,Level,Serises,IsGold,IsPurple = GetItemProp(Id)
	{"GetItemPropInfo",	LuaGetItemPropInfo},  //获取给予界面的物品的信息
	{"GetItemType",	LuaGetItemType},
	{"GetItemSkillType",	LuaGetItemSkillType},

	{"ItemHaveMagic",	LuaItemHaveMagic},    // ItemHaveMagic(nAttre,参数1最小,参数1最大,.......) 当参数最大（或最小）设置-1时，不判断此项
	{"Trade",			LuaTrade	},	      //贸易Trade("maininfo", "IniFileName.ini的路径名")
	//Trade("MainInfo", n, "item1|price1|function1", "item2|price2|function2", ......, "itemn|pricen|functionn")

	{"SyncItem",                 LuaSyncItem},           //同步保存物品属性
	{"RemoveItemByIndex",        LuaRemoveItemByIndex},  //删除某个索引的物品
	{"RemoveItem",				LuaRemoveItemIdx},
	{"ConsumeItemByCount",        LuaConsumeItemByCount },
	{"DelItemByIdx",             LuaDelItemIdx},         //删除某个索引的物品
//-----------------技能相关----------------------------
	{"TestMaxLife",		LuaTestMaxLife},		//AddMagic(魔法id或魔法名)给玩家加某个魔法
	{"AddMagic",		LuaAddMagic},		//AddMagic(魔法id或魔法名)给玩家加某个魔法
	{"DelNpcMagic",	    LuaDelNpcSkills},   //删除指定NPC的某个技能的技能
	{"SetNpcSkill",	    LuaSetNpcSkill},	//设置指定NPC的技能
	{"DelMagic",		LuaDelMagic},		//DelMagic(魔法id或魔法名)
	{"DelAllMagic",		LuaDelAllMagic},    //删除全部技能	
	{"HaveMagic",		LuaHaveMagic},		//HaveMagic(魔法id或魔法名)返回0或1
	{"GetSkillLevel",	LuaGetSkillLevel},	//GetMagicLevel(魔法id或魔法名)返回等级  --技能
	{"SetSkillLevel",	LuaSetSkillLevel},  //设置某个技能等级
	{"GetNpcSkillLevel",LuaGetNpcSkillLevel},	//GetMagicLevel(魔法id或魔法名)返回等级  --技能
	{"getMaxSkillLevel_p",LuaGetMaxSkillLevel_p}, //获取玩家某个技能的最大等级上限
	{"SetNpcSkillLevel",LuaSetNpcSkillLevel},  //设置某个技能等级
	{"SetCurrentSkillLevel",LuaSetCurrentSkillLevel}, 
	{"GetSkillId",		LuaGetSkillIdInSkillList},
	{"AddMagicPoint",	LuaAddMagicPoint},
	{"GetMagicPoint",	LuaGetMagicPoint},
	{"UseSkill",	    LuaUseSkill},           // 使用某个技能	
	{"CastSkillToNpc",	LuaCastSkillToNpc},     // 使用某个技能	
	{"AddSkillState",	LuaAddSkillState},      //增加一个光环
	{"DelSkillState",	LuaDelSkillState},      //删除一个光环效果
	{"SetExpsubTime",   LuaSetExpsubTime},      //设置状态的持续时间
	{"CastSkill",   LuaCastSkill}, 

	{"GetNpcExpRate",   LuaGetNpcExpRate},
	{"RollbackSkill",   LuaRollBackSkills},     //洗髓用的 返回技能点
	{"UpdateSkillList", LuaUpdateSkillList},    //更新同步技能列表
	{"UpdateSkill",     LuaUpdataSkills},
	{"SetXinYunXing",   LuaSetXinYunXing},
	{"AddSkillExp",     LuaAddSkillExp},        //增加技能的熟练度  模式 1 为叠加设置 0 为不叠加
	{"AddNpcSkillExp",  LuaAddNpcSkillExp},     //增加NPC技能的熟练度  模式 1 为叠加设置 0 为不叠加
	{"SetSkillExpInfo",LuaSetSkillExpInfo},
	{"SetOneSkillExpInfo",LuaSetOneSkillExpInfo},
	{"AddOneSkillExpPoint",LuaAddOneSkillExpPoint},

	{"GetLeftSkillInfo",LuaGetLeftSkillInfo},   //获取攻击技能的信息 是否熟练度技能 和 总熟练度
	{"GetOneSkillInfo",LuaGetOneSkillInfo},

	{"GetNpcSkillInfo",LuaGetNpcSkillInfo},     //获取攻击技能的信息 是否熟练度技能 和 总熟练度	
    {"GetSkillName",    LuaGetSkillName},
	{"GetSkillBaseInfo",LuaGetSkillBaseInfo},
	{"GetSkillStateTime",LuaGetSkillStateTime},	//获取状态的时间
	
	{"GetReBornQPoint",LuaGetReBornQPoint},	    //转生
	{"GetReBornJPoint",LuaGetReBornJPoint},

	{"GetRoleVipQPoint",LuaGetRoleVipQPoint},	    //转生
	{"GetRoleVipJPoint",LuaGetRoleVipJPoint},
	

	//GetSkillLeftTime
//  -----------------------------------
	{"AddLeadExp",		LuaAddLeadExp},
	{"GetLeadLevel",	LuaGetLeadLevel},
	
	{"SetFightState",	LuaSetFightState}, //战斗模式
	{"GetFightState",	LuaGetFightState},

	{"SetBayBan",	    LuaSetBayBan}, 
	{"GetBayBan",	    LuaGetBayBan},
	{"GetMsg",	        LuaGetMsg},
//----------OBJ,trap相关--------------------
	{"AddObj",		    LuaAddObj},
	{"SetObjScript",    LuaSetObjScript},
	{"SetObj64Dir",     LuaSetObj64Dir},
	{"SetObjLifeTime",  LuaSetObjLifeTime},
	{"LoadMaps",        LuaLoadActivityMap},
	{"AddMoneyObj", 	LuaAddMoneyObj},        //掉下一堆钱
//------------------玩家相关----------------    
//押镖 
	{"GetNpcKind",		LuaGetNpcKind},         //NPC索引GetNpcKind(index)
	{"GetCurFuMoIdx",	LuaGetCurFuMoIdx},   //	 神将
	{"GetCurFuMoNoLevel",	LuaGetCurFuMoNoLevel}, //神将
	{"GetDeathRevivalPos",	LuaGetDeathRevivalPos},

	{"GetPlatRes",  LuaGetPlatRes},
	{"SetNpcWeaponType",  LuaSetNpcWeaponType},
	{"SetNpcBobyType",  LuaSetNpcBobyType},
	//GetDeathRevivalPos(&nSubWorldID, &nMpsX, &nMpsY); m_nLimitMaxLevel

	
	{"GetLimitMaxLevel",	LuaGetLimitMaxLevel},
	{"SetLimitMaxLevel",	LuaSetLimitMaxLevel},

	{"SetTonBanNun",	LuaSetTonBanNun},
	{"GetTonBanNun",	LuaGetTonBanNun},
	{"GetDistance",		LuaGetDistance},         //获取两个NPC的距离
	{"Addyabiao",		LuaAddyabiao},
	{"NoCreatGoldNpc",	LuaNoCreatGoldNpc},	     //设置重生后不能转化成加强怪
	{"ClearNpcDropRank",LuaClearNpcDropRank},
	
	{"SetNpcNoLevel",	LuaSetNoLevel},	         //该NPC爆率不受等级差影响
	
	{"getNpcClientName",LuaGetNpcCilentName},
	{"setNpcClientName",LuaSetNpcCilentName},	 //用于客户端的显示文字翻译用于越南版本
	{"_setNpcName",LuaSetNpcName_Bm},	         //用于客户端的显示文字翻译用于越南版本
	{"AddNpc",			LuaAddNpc},			     //AddNpc(人物模板id或人物模板名,所处世界id，点坐标x,点坐标y),返回npcid值
	{"AddNpcEx",			LuaAddNpcEx},

	{"DelNpc",			LuaDelNpc},			     //DelNpc(Npcid)
	{"SetNpcPos",		LuaSetNpcPos},
	
	{"SetNpcScript",	LuaSetNpcActionScript},	 //SetNpcScript(npcid, 脚本文件名)设置npc当前的脚本
	{"SetNpcDeathScript",	LuaSetNpcActionScript},	
	{"SetNpcDropScript",	LuaSetNpcDropScript},	

	{"SetIsDoing",	    LuaSetIsDong},	         //设置是否正在执行脚本
	{"SetRevPos",		LuaSetPlayerRevivalPos}, //SetRevPos(点位置X，点位置Y)设置玩家的当前世界的等入点位置
	{"SetTempRevPos",	LuaSetDeathRevivalPos},  //SetTempRevPos(subworldid, x, y ) or SetTempRevPos(id);

	{"GetPlayerRev",	LuaGetPlayerRevivalPos},

	{"GetCurCamp",		LuaGetPlayerCurrentCamp},//GetCurCamp()获得玩家的当前阵营
	{"GetCamp",			LuaGetPlayerCamp},       //GetCamp()获得玩家阵营
	{"SetCurCamp",		LuaSetPlayerCurrentCamp},//SetCurCamp(阵营号):设置玩家当前阵营
	{"SetCamp",			LuaSetPlayerCamp},		 //SetCamp(阵营号):设置阵营
	{"SetNpcCamp",		LuaSetNpcCamp},		     //SetNpcCamp(阵营号):设置阵营
	{"SetNpcCurCamp",	LuaSetNpcCurCamp},
	{"RestoreCamp",		LuaRestorePlayerCamp},            //RestoreCamp()恢复阵营
	{"SetMapCostType"  ,LuaSetMapCostType},               //设置地图的消耗类型	m_MapUseModel
	{"GetMapCostType"  ,LuaGetMapCostType}, 	
	//-------------------------------------------
	//{"GetFaction",		LuaGetPlayerFaction	},    //GetFaction()获得玩家的门派名 
	{"SetFaction",		LuaChangePlayerFaction},  //SetFaction(门派名):设置玩家门派名
	{"ClearFactionIfnfo",LuaClearFactionIfnfo}, 
	//-------------------------------------------
	{"GetColdR",		LuaGetPlayerColdResist},	
	{"SetColdR",		LuaGetPlayerColdResist},
	{"GetFireR",		LuaGetPlayerFireResist},
	{"SetFireR",		LuaSetPlayerFireResist},
	{"GetLightR",		LuaGetPlayerLightResist},
	{"SetLightR",		LuaSetPlayerLightResist},
	{"GetPoisonR",		LuaGetPlayerPoisonResist},
	{"SetPoisonR",		LuaSetPlayerPoisonResist},
	{"GetPhyR",			LuaGetPlayerPhysicsResist},
	{"SetPhyR",			LuaSetPlayerPhysicsResist},
	{"GetNextExp",		LuaGetNextExp},
	{"GetExp",			LuaGetPlayerExp	},			//GetExp():获得玩家的当前经验值
	{"AddExp",			LuaModifyPlayerExp},		//AddExp(经验值，对方等级，是否组队共享经验值)

	{"AddOwnExp",		LuaAddOwnExp	},			        //AddOwnExp(Exp)，给玩家直接加经验
	{"AddSumExp",		LuaAddExpCongDon	},			//AddSumExp(Exp)，给玩家直接加经验 可以重叠

	{"GetLife",			LuaGetPlayerLife},			//GetLife()获得玩家的生命值
	{"RestoreLife",		LuaRestorePlayerLife},		//RestoreLife()恢复玩家的生命
	{"GetMana",			LuaGetPlayerMana},			//GetMana()获得玩家的Mana
	{"RestoreMana",		LuaRestorePlayerMana},		//RestoreMana()恢复玩家的Mana
	{"GetStamina",		LuaGetPlayerStamina},		//GetStamina()获得玩家Stamina
	{"RestoreStamina",	LuaRestorePlayerStamina},	//RestoreMana()恢复玩家的Stamina
	{"GetDefend",		LuaGetPlayerDefend},		//GetDefend()获得玩家的防御力
	{"GetSex",			LuaGetPlayerSex},			//GetSex()获得玩家的性别
	{"GetPlayerNpcIdx",	LuaGetPlayerNpcIdx},
	{"GetSeries",		LuaGetPlayerSeries},		//GetSeries()获得玩家的系0man/1woman
	{"SetSeries",		LuaSetPlayerSeries},		//SetSeries(五行)
	{"CheckHaveEq",		LuaCheckHaveEq},		    //检测是否身上穿着装备
	{"GetReturnPos",LuaGetReturnPos},
	{"GetMaptimeInfo",LuaGetNpcMaptimeInfo},
//-------------------------------------------------------联赛相关
	{"ReTurnMoneyInMap",LuaReTurnMoneyInMap},	    //返回钱或 金币
	{"TestData",LuaTestData},
	{"UpLeagueData",LuaUpLeagueData},
	{"SetEnemyData",LuaSetEnemyData},               //个人联赛匹配对手
	{"ExportLeagueData",LuaExportLeagueData},       //导出数据
	{"GetLeagueDataInfo",LuaGetLeagueDataInfo},
	{"GetLeagueData",LuaGetLeagueData},	            //积分排名
	{"CheckLeagueData",LuaCheckLeagueData},
	{"ClearLeagueData",LuaClearLeagueData},
	{"SetLeagueData",LuaSetLeagueData},

//-------------------------------------------------------攻城相关
	{"UpCityWarData",LuaUpCityWarData},
	{"GetCityWarDataInfo",LuaGetCityWarDataInfo},
	{"GetCityWarData",LuaGetCityWarData},			      //贡献度排名
	{"CheckCityWarData",LuaCheckCityWarData},
	{"ClearCityWarData",LuaClearCityWarData},
	{"SetCityWarData",LuaSetCityWarData},
//---------------NPC相关--------------------------------
    {"setNpcDoNone",LuaSetNpcDoNone},//设置什么都不作 站稳
	{"SetNotDeath",LuaSetNotDeath},  //设置无敌
	{"SetActiveForever",LuaSetActiveForever},

	{"ClearPlayerSkillState",LuaClearPlayerSkillState},  //清楚人物所有状态数据
	{"PlayerSaveData",LuaPlayerSaveData},        //人物存档

	{"TickOutPlayerInServer",LuaTickOutPlayerInServer},
	{"Gm_TickOutPlayerInGateWay",LuaGm_TickOutPlayerInGateWay},
	
	{"ClearNpcCurExp",LuaClearNpcCurExp},        //清楚经验分配数据
    {"UpdataNpcCurData",LuaUpdataNpcCurData},
	//Npc[nTonNpcIdx].UpdataNpcCurData(m_PlayerIdx);
	{"GetNpcSeries",	LuaGetNpcSeries},		    //GetSeries()获得玩家的系0man/1woman
	{"NPCINFO_GetSeries",	LuaGetNpcSeries},
	{"SetPlayerName",	LuaSetPlayerName},		    //设置玩家的名字
	{"SetProtectTime",	LuaSetProtectTime},		    //设置保护时间
	//Npc[Player[m_nPlayerIdx].m_nIndex].m_ProtectTime

	{"GetName",			LuaGetPlayerName},			//GetName()获得玩家的姓名
	{"GetNameA",		LuaGetPlayerNameA},			//GetNameA(Playeridx)获得玩家的姓名
	{"GetNpcName",	    LuaGetNpcName},             //获取怪物的名字
	{"GetNpcIndexid",	LuaGetPlayerindexid},       //获取人物在NPC中的索引
    {"GetNpcSettings",  LuaGetNpcSettings},
	{"GetIndxFromName", LuaGetIndxFromName},
    {"SetCreatBossAndScript", LuaSetCreatBossAndScript},
	{"SetNpcIsRe",	    LuaSetNpcIsRe},  //NPC是否可以重生 以及重生时间 3600 为一个小时吧
	{"SetNpcReFrame",	LuaSetNpcReFrame}, //设置NPC的重生时间
//------------------帮会相关---------------------
	{"OpenTong",	      LuaOpenTong},	       //OpenTong()通知玩家打开帮会界面
	{"SetTonMembersInfo", LuaSetTonMembersInfo},
	{"GetBang",			LuaGetBang},    //是否入帮(无参数)
	{"GetBangByno",	    LuaGetBangByno},//有参数	
	{"GetTongName",		LuaGetTenBang}, //获取帮会名---没参数
	{"GetTonName",	    LuaGetTonName}, //获取帮会名---有参数---Npc 索引
	{"GetTongMaster",	LuaGetPlayerTongNameMaster}, //获取帮主名
    {"IsTongMaster",	LuaIsPlayerTongNameMaster},  //是否帮主
	{"GetQuyTong",		LuaGetQuyBang},                 //获取帮派的金钱
	{"GetCampTong",		LuaGetCampBang},                //获取帮会阵营
	{"GetTongLevel",	LuaGetTongLevel},
	{"GetTongMapIdx",	LuaGetTongMapIdx},              //获取帮会地图的索引
	{"GetMapTongName",	LuaGetMapTongName},             //获取归属帮会名
	{"GetTongMenberCount",LuaGetTongMenberCount},       //获取帮会总人数
	{"GetTongAttackState",LuaGetTongAttackState},		//获取宣战的状态
	{"GetTongAttackName",LuaGetTongAttackName},			//获取对战帮会
	{"SetTongLevel",LuaSetTongLevel},
	{"SetTongDeathCount",LuaSetTongDeathCount},
	{"SetTongMap",LuaSetTongMap},
	{"RandomSetTongMap",LuaRandomSetTongMap},	

//--------------------------------------------------------	
	{"GetAccount",		LuaGetPlayerAccount},
	{"GetAccoutName",	LuaGetPlayerAccoutName},              //账号
	{"GetUUID",			LuaGetPlayerID},			    //GetUUID()获得玩家的唯一ID  dWid
	{"FindAroundNpc",	LuaFindAroundNpc},
	{"FindAroundPlayer",	LuaFindAroundPlayer},
	{"GetLeadExp",		LuaGetPlayerLeadExp},		    //GetLeadExp()获得玩家的统率经验值
	{"GetLeadLevel",	LuaGetPlayerLeadLevel},		    //GetLeadLevel()获得玩家的统率等级
	//{"GetLevel",		LuaGetLevel},			        //GetLevel()GetPlayers Level
	{"SetLevel",		LuaSetLevel},
	{"GetRestAP",		LuaGetPlayerRestAttributePoint},//GetRestAP()获得玩家的剩于属性点数
	{"GetRestSP",		LuaGetPlayerRestSkillPoint},	//GetRestSP()获得玩家的剩于技能点数	
	{"GetLucky",		LuaGetPlayerLucky},			    //GetLucky()获得玩家的幸运值
	{"GetEng",			LuaGetPlayerEngergy},		    //GetEng()获得玩家的力量值Eng
	{"GetDex",			LuaGetPlayerDexterity},		    //GetDex()获得玩家的Dex
	{"GetStrg",			LuaGetPlayerStrength},		    //GetStrg()
	{"GetVit",			LuaGetPlayerVitality},		     //GetVit()
	{"ResetBaseAttrib",	LuaResetBaseAttribute},
	{"GetCash",			LuaGetPlayerCashMoney},		     //GetCash()获得玩家的现金
	{"Pay",				LuaPlayerPayMoney},	
	{"PayXu",			LuaPlayerPayXu},
	{"GetXu",			LuaPlayerGetXu},
	{"Earn",			LuaPlayerEarnMoney},
	{"EarnXu",			LuaPlayerEarnXu},//xu
	{"PrePay",			LuaPlayerPrePayMoney},	
	{"PrePayXu",		LuaPlayerPrePayXu},
	{"ExeScript",		LuaPlayerExecuteScript},	      //ExeScript(脚本文件名,参数),执行时会调用main函数	
	{"LocalScript",		LuaLocalScript},	              //重载脚本
	{"LocalAllScript",	LuaLocalAllScript},	              //重载全部脚本
	{"LocalSerIni",		LuaLocalSerIni},	              //重载Ini
	{"LocalSerTxt",	    LuaLocalSerTxt},	              //重载Txt
	{"AttackNpc",		LuaAttackNpc},				      //AttackNpc(NpcDwid,物理伤害值，冰，火，电，毒）
	{"KillNpc",			LuaKillNpc},				      //KillNpc(NpcDWID)
	{"KillNpcWithIdx",	LuaKillNpcWithIdx},
	{"KillPlayer",		LuaKillPlayer},				      //KillPlayer();  自杀
	{"Sale",			LuaSale},					      //Sale(SaleId)买卖，SaleId为便卖的物品信息列表id
	{"OpenVipShop",		LuaOpenVipShop},
	{"UseTownPortal",	LuaUseTownPortal},


												{"ReturnFromPortal",LuaReturnFromTownPortal	},	       //返回地点

												{"OpenBox",			LuaOpenBox},
												{"OpenResetPass",	LuaOpenResetPass},
												{"OpenStringUI",	LuaOpenStringUI},                   //字符串输入界面
												{"OpenTaskUI",		LuaGiftDT},                         //datau  老头任务
												//返回点
												{"AddStation",   	LuaAddPlayerStation},	            //增加经过的城市
												{"AddTermini",		LuaAddPlayerWayPoint},				//增加经过的地点
												{"GetStation",		LuaGetPlayerStation	},
												{"GetStationCount", LuaGetPlayerStationCount},

												{"GetCityCount",    LuaGetAllStationCount},
												{"GetCity",         LuaGetCity},

												{"GetWayPoint",		LuaGetPlayerWayPoint},
												{"GetStationName",	LuaGetStationName},
												{"GetWayPointName", LuaGetWayPointName},
												{"CheckWayPointName", LuaCheckWayPointName},
												{"GetPrice2Station", LuaGetPriceToStation},
												{"GetPrice2WayPoint", LuaGetPriceToWayPoint	},
												{"GetStationPos",	LuaGetStationPos},
												{"GetWayPointPos",	LuaGetWayPointPos},
												{"GetPlayerCount",	LuaGetPlayerCount},                   //玩家的数量

												{"GetServerTimes",	LuaGetServerTimes},
												{"GetGameTime",		LuaGetGameTimes},
												{"GetLoginTime",	LuaGetPlayerLoginTime},
												{"GetOnlineTime",	LuaGetPlayerOnlineTime},

												{"GetPlayerLoginTimer",LuaGetPlayerLoginTimer},
												//-----------------------------------------------
												   {"SetNpcliveType",LuaSetNpcliveType}, //设置类型 
												   {"SetNpcLiveTime",LuaSetNpcLiveTime}, //设置存活时间
												   {"GetNpcLiveTime",LuaGetNpcLiveTime}, //设置存活时间
												   {"AddNpcLiveTime",LuaAddNpcLiveTime}, //增加存活时间
												   {"GetNpcReciveNum",LuaGetNpcReciveNum},//获取重生的次数
												   {"SetNpcReciveNum",LuaSetNpcReciveNum},//设置重生的次数
												   {"AddNpcReciveNum",LuaAddNpcReciveNum},

												   {"SetNpcCurPathPos",LuaSetNpcCurPathPos},
												   {"SetNpcIsDel",LuaSetNpcIsDel},        //设置是否直接杀死NPC
												   {"SynNpcCur64Dir",LuaSynNpcCur64Dir},
												   //-------------------------------------------------  
													   {"GetRank",			LuaGetRank},//GetRank()
													   {"SetRank",			LuaSetRank},//SetRank(id)
													   {"SetNpcTitle",	    LuaSetNpcTitle},        //设置称号
													    
													   {"KSG_GetCurSec",		LuaKSG_GetCurSec}, 
													   {"KSG_GetUyThacHour",	LuaKSG_GetUyThacHour}, 
													   {"GetPlayerTitle",	    LuaGetPlayerTitle},        //设置称号
													   {"SetPlayerTitle",	    LuaSetPlayerTitle},        //设置称号
													   {"SetPlayerTitleSkills",	    LuaSetPlayerTitleSkills},



													   {"ResSetNpcTitle",	LuaResSetNpcTitle},     //恢复原来的称号
													   {"SetNpcCurTitle",	LuaSetNpcCurTitle},		//设置临时称号
													   {"CastNpcTitleSkill",LuaCastNpcTitleSkill},	//使用称号技能
														{ "GetMapTimeInfo",	LuaGetMapTimeInfo },	//add by tuanln
													   {"GetNpcTitle",	    LuaGetNpcTitle},
													   {"GetRanlFF",		LuaGetRankFF},//GetRankFF()
													   {"AddRankFF",		LuaSetRankFF},//SetRankFF(id)
													   {"SetPlayFF",		LuaSetPlayFF},//SetRankFF(id)  挂机标志
													   {"GetPlayFF",		LuaGetPlayFF},//GetRankFF(id)  挂机标志	
													   {"GetExItem",		LuaGetExItem},//GetExItem()
													   {"SetExItem",		LuaSetExItem},//SetExItem(id)   子母袋
													   {"GetExBox",		LuaGetExBox},//GetExItem() 
													   {"SetExBox",		LuaSetExBox},//SetExItem(id)    开扩展箱

	{"SetPKState",		LuaSetPKState},//SetPKState(id)
	{"SetPKFlag",		LuaSetPKState},
	{"ForbidChangePK",		LuaForbidChangePK},
	{"DisabledUseTownP",	LuaDisabledUseTownP}, 
	{"IsDisabledUseTownP",	LuaForBitUseTownPortal},//fix thoi dia phu
	{"getForBitUseTownPortal",	LuaForBitUseTownPortal},
	
    {"ForbidEnmity",	LuaForbidEnmity},
	{"SetDeathType",	LuaSetDeathType},
	{"ForbitTrade",	LuaForbitTrade},
	

	{"SetPropState",	LuaSetObjPropState},//SetPropState( hide = 1) hide obj
	{"GetServerName",	LuaGetServerName},	
	//------------------Station Script---------------
	{"GetWharfName",	LuaGetDockName},
	{"GetWharfCount",	LuaGetDockCount},
	{"GetWharfPrice",	LuaGetDockPrice},
	{"GetWharf",		LuaGetDock},
	{"GetWharfPos",		LuaGetDockPos},
	{"GetTerminiFState", LuaGetWayPointFightState},
	//------------------------------------------------跨gs操作
	{"gm_KickOutPlayer",	LuaGm_KickOutPlayer},
	{"gm_Money",	LuaGm_Money},
	{"gm_Xu",	LuaGm_Xu},
	{"gm_TaskId",	LuaGm_TaskId},
	{"gm_ForbitChat",	LuaGm_ForbitChat},
	{"gm_Level",	LuaGm_Level},
	{"gm_MoveTo",	LuaGm_MoveTo},
	{"gm_LocalScript",LuaGm_LocalScript},	              //重载脚本
	{"gm_AddNpc",LuaGm_addNpc},	                          //刷某个地图的npc
	//-------------------------------------------------
	{"KickOutPlayer",	LuaKickOutPlayer},
	{"KickOutSelf",		LuaKickOutSelf},
	{"getMobileClient", LuagetMobileClient},
	{"getClientLanguage", LuagetClientLanguage},
	{"getLanguageByIndex", LuagetLanguageByIndex},

	{"KickMobileOutSelf",LuaKickMobileOutSelf},
	{"SetChatFlag",		LuaSetPlayerChatForbiddenFlag}, //禁言
	{"SetOneChatFlag",	LuaSetPlayerForbiddenChat},     //对某人禁言:SetOneChatFlag(姓名,参数)
	{"SetLockRole",     LuaSetLockRole},                //锁住某个角色
	//------------------------------------------------	
	{"AddNote",         LuaAddNote},            //增加记事记录
	//-----------------Mission Script-----------------
	{"GetMissionV",     LuaGetMissionValue},//GetMissionV(Vid)	       某个地图中 有效
	{"SetMissionV",     LuaSetMissionValue},//SetMissionV(Vid, Value)  某个地图中 有效

	{"GetMissionS",     LuaGetMissionValueS},//GetMissionV(Vid)
	{"SetMissionS",     LuaSetMissionValueS},//SetMissionV(Vid, Value)
	{"SetMission",		LuaSetMissionValue},//SetMissionV(Vid, Value)

	{"BuildATeam",    LuaBuildATeam},

	{"ClearMapNpc",    LuaClearMapNpc},
	{"ClearMapTrap",    LuaClearMapTrap},
	{"ClearMapObj",    LuaClearMapObj},
	{"ClearObstacleObj",    LuaClearObstacleObj},

	{"findNpcPosWithName",    LuaFindNpcPosWithName},


	{"AddMapTrap",    LuaAddMapTrap},
	{"AddObstacleObj",    LuaAddObstacleObj},

	{"GetGlbMissionV",  LuaGetGlobalMissionValue},
	{"SetGlbMissionV",  LuaSetGlobalMissionValue},

	{"GetGlobalObjV",  LuaGetGlobalMissionValue},
	{"SetGlobalObj",  LuaSetGlobalMissionValue},

	{"Ladder_GetLadderInfo",LuaGetMSLadder},
	{"GetMSLadder",LuaGetMSLadder},
	{"SetMSGroup", LuaSetMissionGroup},

	{"IsMission", LuaIsMission},

	{"GetMissionName", LuaGetMissionName},
	{"OpenMission",     LuaInitMission},//OpenMission(missionid)  //初始化Mission
	{ "JoinMission", LuaJoinMission },
	{"RunMission",      LuaRunMission},
	{"CloseMission",    LuaCloseMission},//CloseMission(missionid)
	{"StartMissionTimer", LuaStartMissionTimer},////StartMissionTimer(missionid, timerid, time)
	{"StopMissionTimer",  LuaStopMissionTimer},
	{"GetMSRestTime",     LuaGetMissionRestTime}, //GetMSRestTime(missionid, timerid)
	{"GetMSIdxGroup",     LuaGetPlayerMissionGroup},//GetPlayerGroup(missionid, playerid);	m_ProtectTime
	
	{"AddMSPlayer",       LuaAddMissionPlayer},
	{"DelMSPlayer",       LuaRemoveMissionPlayer},
	{"GetNextPlayer",     LuaGetNextPlayer},
	{"PIdx2MSDIdx",       LuaGetMissionPlayer_DataIndex},//(missionid, pidx)
	{"MSDIdx2PIdx",       LuaGetMissionPlayer_PlayerIndex},//(missionid, dataidx)
	{"NpcIdx2PIdx",       LuaNpcIndexToPlayerIndex},
	{"PIdx2NpcIdx",       LuaPlayerIndexToNpcIndex},
	
	{"GetMSPlayerCount",  LuaMissionPlayerCount},//GetMSPlayerCount(missionid, group = 0)

	{"RevivalAllNpc",	  LuaRevivalAllNpc},
	{"DelAllNpc",	      LuaDelAllNpcInWro},    //删除NPC
	{"DelTypeNpc",	      LuaDelTypeNpcInWro}, 
	{"DelNpcWithName",    LuaDelNpcWithName},    //删除NPC
	{"ClearMapNpcWithName",    LuaClearMapNpcWithName},  
	

	{"SetPMParam",        LuaSetMissionPlayerParam },
	{"GetPMParam",        LuaGetMissionPlayerParam},


	{"SetRankParam",        LuaSetMissionRankParam},
	{"GetRankParam",        LuaGetMissionRankParam},
	{"UpRankParam",         LuaUpMissionRankParam},
	

	{"Msg2MSGroup",       LuaMissionMsg2Group},
	{"Msg2MSAll",         LuaMissionMsg2All},
	{"Msg2MSPlayer",      LuaMissionMsg2Player},
	{"SetDeathScript",    LuaSetPlayerDeathScript},
	{"SetDmgScript",	  LuaSetPlayerDamageScript},

	{"Death",             LuaKillNpc},
	{"deathNpcFromIndex",  LuaKillNpcWhthIndex},
	
	{"DynamicExecuteByPlayer",  LuaDynamicExecuteByPlayer},
	{"callPlayerFunction",  LuacallPlayerFunctionr},
	

	{"HideNpc",           LuaHideNpc}	,         //HideNpc(npcindex/npcname, hidetime)
	{"SetLogoutRV",       LuaSetLogout},
	{"GetPK",             LuaGetPlayerPKValue},   //pkValue = GetPK() 
	{"SetPK",             LuaSetPlayerPKValue},   //SetPK(pkValue)
	//------------------------------------------------
	//排名相关函数
	{"ShowLadder",        LuaShowLadder},     //ShowLadder(LadderCount, LadderId1,LadderId2,...);	
	{"GetRankInWorld",    LuaGetRankInWorld}, //获取个人的世界排名
	//------------------------------------------------
	{"SetPunish",	      LuaSetDeathPunish},  //0表示NPC杀死模式 1切磋模式
//	{"SetDeathType",	      LuaSetDeathPunish},  //0表示NPC杀死模式 1切磋模式

													   {"GetPunish",         LuaGetDeathPunish},
													   //-------------------------------------------------惩罚模式
													   //结拜
													   {"SwearBrother",     LuaSwearBrother}, // ret = SwearBrother(TeamId);
													   {"MakeEnemy",	      LuaMakeEnemy},    //结仇 MakeEnemy(enemyname)
													   //-------------------------------------------------NPC相关

													   {"setSendItemInfo", LuasetSendItemInfo},
													   {"SetLock",        	  LuaSetLock},
													   {"CheckLock",         LuaCheckLock},
													   {"CheckLockPw",         LuaCheckLockPw},
													   {"CheckBox",	      LuaCheckBox},           //检测包袱空间
													   {"AddProp",		      LuaAddPropPoint},       //加玩家属性点
													   {"GetProp",		      LuaGetRestPropPoint},   //重置玩家潜能点
													   {"GetSkillProp",			LuaGetSkillProp},
													   {"IncSkillExp",			LuaIncSkillExp},
													   {"SetBasePoint",	  LuaSetBasePoint},       //设置基本属性点

													   {"SetExtPoint",       LuaSetExtPoint},
													   {"GetExtPoint",	      LuaGetExtPoint},           //扩展点 会员充值
													   {"PayExtPoint",	      LuaPayExtPoint},           //扩展点 会员充值
													   {"GetCExtPoint",      LuaGetCExtPoint},          //获取已经改变了多少点数
													   {"AddAndPayExtPoint", LuaAddAndPayExtPoint},     //扣除或增加扩展点
													   //{"GetExtPoint2P",	      LuaGetExtPoint2P },
														{ "NewGetExtPoint",	LuaNewGetExtPoint },
													   { "NewPayExtPoint",	LuaNewPayExtPoint },
													   {"SetVipChenHao", LuaSetVipChenHao},             //vip
													   {"SetTuiGuangChenHao", LuaSetTuiGuangChenHao},   //推广

	{"SetGameTime",        LuaSetGameTime},
	{"GetLeftTime",	       LuaGetLeftTime},           //时间 会员充值
	{"AddAndPayExtTime",   LuaAddAndPayExtTime},      //扣除或增加时间
	{"GetCExtTime",        LuaGetCExtTime},           //获取已经改变了多少时间
	{"GetVipType",         LuaVipType},               //会员类型
	{"GetNameFromSetting",	    LuaGetNameFromSettings},  //获取模板号NPC的名字
	{"SetNpcSeries",	        LuaSetNpcSeries},         //SetSeries(五行)
	{"SetChenHaoName",	        LuaSetChenHaoName},       //设定称号的名称
	{"GetChenHaoName",	        LuaGetChenHaoName},       //设定称号的名称
	{"CheckStrLen",	            LuaCheckStrLen}, 	      //检测字符串的长度
	{"CheckHaveStr",	        LuaCheckHaveStr}, 	      //检测是否含有这个字符串
	{"ChangePlayerName",	    LuaChangePlayerName},
	{"GetRestNameCount",	    LuaGetRestNameCount},
	{"CheckDataName",	        LuaCheckDataName},
	{"GetDataNameState",	    LuaGetDataNameState},

	//同伴 if  (m_Kind == kind_partner && Partner._lockMap>0)

	{"setNpcLockMap",	            LuaSetNpcLockMap},
	{"setNpcDamageRatio",	    LuaSetNpcDamageRatio},


	{"SetNpcName",	            LuaSetNpcName},           //设置NPC的名字
	{"CheckName",               LuaCheckName},            //检测某个索引NPC的名字
	{"GetNpcDwidx",             LuaGetNpcDwidx},          //获取Npc的dwid
	{"FindTbName",              LuaFindTbName},           //检测镖车的名字
	{"FindNpcName",             LuaFindNpcName},          //检测某个名字的NPC是否存在
	{"SetNpcJb",                LuaSetNpcJb},             //NPC携带的金币
	{"SetNpcJxb",               LuaSetNpcJxb},            //npc携带的剑侠B
	{"SetNpcItemCount",         LuaSetNpcItemCount},      //设置NPC掉下物品的数量

	{"SetNpcDefend",            LuaSetNpcDefend},	  //m_CurrentDefend	闪避
	{"GetNpcDefend",            LuaGetNpcDefend},
	
	{"GetNpcJb",                LuaGetNpcJb},             //NPC携带的金币
	{"GetNpcJxb",               LuaGetNpcJxb},            //npc携带的剑侠B
	{"SetNpcAngry",             LuaSetNpcAngry},          //设置npc怒气值
	{"SetNpcMaxAngry",   	    LuaSetNpcMaxAngry},       //设置NPC怒气基本最大值(存档)
	{"SetNpcCurMaxAngry",   	LuaSetNpcCurMaxAngry},    //设置NPC怒气当前最大值
	{"UseAngrytoNpc",	        LuaUseAngrytoNpc},        //对攻击对象使用固定伤害
//---------------------BOSS触发相关-----------------------
	{"SetBaseDamage",           LuaSetBaseDamage},           //设置基本伤害  // nType=0 直接设置  其他 为倍率
	{"SetNpcDefendAndDamage",   LuaSetNpcDefendAndDamage},   //设置NPC防御攻击等
	{"GetNpcDefendAndDamage",   LuaGetNpcDefendAndDamage},   //获取NPC的防御伤害等
	{"SetNpcExperience",        LuaSetNpcExperience},        //设置NPC的经验倍率
	{"SetSetNpcExper",             LuaSetNpcExper},          //设置NPC的系统或脚本经验倍率
	{"SetNpcAttackSpeed",       LuaSetNpcAttackSpeed},       //设置NPC攻速(越大就越快)
	{"SetNpcAttackFrame",       LuaSetNpcAttackFrame},       //设置NPC攻击帧数(越大就越慢)


	{"SetNpcDefense",			LuaSetNpcDefense},
	{"SetNpcLife",				LuaSetNpcLife},
	{"GetNpcLife",				LuaGetNpcLife},

	{"GetNpcLifeRep",           LuaGetNpcLifeRep},           //设置NPC生命恢复速度
	{"SetNpcLifeRep",           LuaSetNpcLifeRep},           //设置NPC生命恢复速度
	{"SetNpcGuiShu",            LuaSetNpcGuiShu},            //设置NPC归属主人
	{"GetNpcGuiShu",            LuaGetNpcGuiShu},
	
	{"GetNpcCurDamage",         LuaGetNpcCurDamage},         //获取最大最小命中数值
	{"SetNpcCurDefend",         LuaSetNpcCurDefend},         //设置NPC五防 和 最大值	
	{"GetNpcCurDefend",         LuaGetNpcCurDefend},
	{"SetNpcCurlife",           LuaSetNpcCurlife},           //设置NPC五防 和 最大值
	{"GetNpcCurlife",           LuaGetNpcCurlife},	
	{"SetNpcExp",				LuaSetNpcExper},          //设置NPC的系统或脚本经验倍率

	{"SetNpcSpeed",				LuaSetNpcSpeed},
	{"SetNpcSync",				LuaSetNpcSync},

    {"GetNpcParam",				LuaGetNpcParam},
	{"SetNpcParam",				LuaSetNpcParam},
    {"GetNpcValue",				LuaGetNpcValue},
	{"SetNpcValue",				LuaSetNpcValue},

	{"FindNpc",					LuaFindNpc},

	{"SetNpcTimeout",			LuaSetNpcTimeout},
	{"GetNpcTimeout",			LuaGetNpcTimeout},
	{"SetNpcTimeLive",			LuaSetNpcTimeLive},
	{"GetNpcTimeLive",			LuaGetNpcTimeLive},

	{"SetNpcOwner",				LuaSetNpcOwner},
	{"GetNpcOwner",				LuaGetNpcOwner},

	{"SetNpcAR",				LuaSetNpcAR},
	{"SetNpcHitRecover",		LuaSetNpcHitRecover},
	{"SetNpcDmgEx",				LuaSetNpcDmgEx},
	{"SetNpcResist",			LuaSetNpcResist},
	{"GetNpcResist",			LuaGetNpcResist},
	{"SetNpcPhanDame",			LuaSetNpcPhanDame},
	{"GetNpcPhanDame",			LuaGetNpcPhanDame},

	{"GetNpcIdx",				LuaGetCurNpcIndex},
	{"GetNpcID",				LuaGetNpcID},
//---------------------检测相关----------------------------
	{"CheckForbitMapA",	        LuaCheckForbitMapA},         //检测禁止列表--数字对比
	{"CheckForbitMapB",	        LuaCheckForbitMapB},         //检测禁止列表--字符串对比
	{"CheckIsThisItem",         LuaCheckIsThisItem},         //检查是否这个物品
    {"CheckNumandstr",          LuaCheckNumandstr},          //检查是数字或是字符
	{"CheckNpcName",	        LuaCheckNpcName},            //检测NPC的名字  返回索引
	{"CheckNewNpcName",	        LuaCheckNewNpcName},
	{"CheckNpcGuiShu",	        LuaCheckNpcGuiShu},            //检测NPC的名字  返回索引
	{"CHeckIsGuan",             LuaCHeckIsGuan},             //检查是否已经被任命职位了  参数: 名字 或  NPC索引
	{"GetNpcSex",               LuaGetNpcSex},               //获取nPC的性别 ，参数：参数: 名字 或  NPC索引
	{"SetNpcSex",               LuaSetNpcSex},
//---------------------攻城官员设置 地图相关----------------
	{"SetWarMasterAndTong", 	LuaSetWarMasterAndTong},    //设定个人的城主和帮会
	{"SetWarShuiAndZhi",       	LuaSetWarShuiAndZhi},       //设定个人税收和物指
	{"GetWarMasterAndTong",     LuaGetWarMasterandTong},    //得到个人的城主和帮会
	{"GetWarShuiAndZhi",       	LuaGetWarShuiAndZhi},       //达到个人税收和物指
	{"SetWarCity",       	    LuaSetWarCity},             //设定个人属于哪个帮会哪个占领城
    {"GetMapName",              LuaGetMapName},  
    {"GetWorldName",              LuaGetMapName},  
//	{"SetSubWarCity",	        LuaSetSubWarCity},           //设定地图 为占领地图
    {"SetSubWarShuiAndZhi",	    LuaSetSubWarShuiAndZhi},     //设定地图 的城主和 帮会
    {"SetSubWarMasterAndTong",	LuaSetSubWarMasterAndTong},  //设定地图城主 或帮会
	{"GetSubWarShuiAndZhi", 	LuaGetSubWarShuiAndZhi},     //得到当前地图的税收和 物指
	{"GetSubWarMasterAndTong",	LuaGetSubWarMasterAndTong},  //得到地图城主 或帮会
	{"GetSubWarCityMoney",	    LuaGetSubWarCityMoney},      //获取地图的总资金
	{"SetSubGuanFUli",          LuaSetSubGuanFUli},          //设置官员福利
	{"SetSubWarCityMoney",	    LuaSetSubWarCityMoney},      //获取地图的总资金

	{"SetSubWarCityGuanYuan",    LuaSetSubWarCityGuanYuan},  //设置城市官员
	{"SetSubMapPKmodle",  	     LuaSetSubMapPKmodle},       //设置地图的PK模式
	{"SetSubMapFightModle",  	 LuaSetSubMapFightmodle},    //设置地图战斗模式
	{"SetSubMapFightComp",  	 LuaSetSubMapFightComp},	 //地图的战斗阵营
	{"SetSubMapShowInfo",  	     LuaSetSubMapShowInfo},		 //显示基本信息
	{"SetSubMapFightTime",  	 LuaSetSubMapFightTime},	 //设置地图的战斗时间
	{"GetSubMapFightTime",  	 LuaGetSubMapFightTime},     //获取战斗时间
	{"SetSubMapSleepTime",  	 LuaSetSubMapSleepTime},     //设置延时时间
	{"GetSubMapSleepTime",  	 LuaGetSubMapSleepTime},	 //获取延时时间
	{"SetSubMapMagicCount",  	 LuaSetSubMapMagicCount},
	{"GetSubMapMagicCount",  	 LuaGetSubMapMagicCount},

	{"SetSubTeam",  	         LuaSetSubTeam},	         //设置某个地图的参赛队伍编号
	{"GetSubTeam",  	         LuaGetSubTeam},	         //获取某个地图的参赛队伍编号
	
	{"SetSubMapJinYan",  	     LuaSetSubMapJinYan},		 //设置地图经验倍率(人物)
	{"SetSubMapLucky",  	     LuaSetSubMapLucky},		 //设置地图幸运倍率(人物)
	{"SetSubMapYinLiang",  	     LuaSetSubMapYinLiang},		 //设置地图银两倍率(人物)
	{"GetPlayerCountInMap",  	 LuaGetPlayerCountInMap},    //获取某个地图中的人物数量
	{"GetPlayerCountOffLive",  	 LuaGetPlayerCountOffLive},
	{"SetMapPunishState",  	 LuaSetMapPunishState},
	{"GetMapPunishState",  	 LuaGetMapPunishState},

	
	{"SetMapNoDropItemModel",  	 LuaSetMapNoDropItemModel},
	{"GetMapNoDropItemModel",  	 LuaGetMapNoDropItemModel},

	{"SetSubMapNpcAttack",  	 LuaSetSubMapNpcAttack},
	{"GetCurMapNpcAttack",  	 LuaGetCurMapNpcAttack},
	{"SetCurMapNpcAttack",  	 LuaSetCurMapNpcAttack},
	{"SetCurMapNpcNum",  	     LuaSetCurMapNpcNum},
	
	{"SetIsWhereCity",  	     LuaSetIsWhereCity},	    //设置是那个城市的居民
	{"GetIsWhereCity",  	     LuaGetIsWhereCity},        //获取是那个城市的居民
	{"SetMapPlayerAttack",  	 LuaSetMapPlayerAttack},	//设置总人数和当前人数
	{"SetCurMapPlayerAttack",  	 LuaSetCurMapPlayerAttack},	//设置当前人数
	{"GetCurMapPlayerAttack",  	 LuaGetCurMapPlayerAttack},	//获取当前地图的总人数 和当前人数

	{"getMapEnterCount_Z",  	 LuaGetMapEnterCount_Z},
	{"getMapEnterCount_F",  	 LuaGetMapEnterCount_F},

	{"setMapEnterCount_Z",  	 LuaSetMapEnterCount_Z},
	{"setMapEnterCount_F",  	 LuaSetMapEnterCount_F},

	{"getMapDeathCount_Z",  	 LuaGetMapDeathCount_Z},
	{"getMapDeathCount_F",  	 LuaGetMapDeathCount_F},

	{"setMapDeathCount_Z",  	 LuaSetMapDeathCount_Z},
	{"setMapDeathCount_F",  	 LuaSetMapDeathCount_F},

	{"SetSubCurTime",  	         LuaSetSubCurTime},
	{"SetSubNextTime",           LuaSetSubNextTime},
	{"GetSubOpenTime",           LuaGetSubOpenTime},

	{"SetSubWarGong",  	         LuaSetSubWarGong},          //设置攻方
	{"SetSubWarShou",  	         LuaSetSubWarShou},          //设置守方
//	{"GetSubWarGong",  	         LuaGetSubWarGong},          //设置攻方
//	{"GetSubWarShou",  	         LuaGetSubWarShou},
	{"GetSubWhoId",  	         LuaGetSubWhoId},
	{"SetSubWhoId",  	         LuaSetSubWhoId},
//------------表格操作 龙脉任务 地图相关-------------------------
    //{"TaskAwardMatrix",          LuaGetTaskAwardMatrix}, //获取任务 同一个名称的 有多少行
    {"TaskNo",                   LuaSetTaskNo}, 
	
	{"GetMapPos_STR",            LuaGetMapPos_STR}, 
	{"GetMapPos_INT",            LuaGetMapPos_INT}, 
	{"WriteIniFileInfo",         LuaWriteIniFileInfo},   //写ini文件 的内容 ，返回字符串类型  3个参数
//	{"GetTabRowAndCol",          LuaGetTabRowAndCol},    //获取文件的 总行数和列数   1个参数	

	{"TabArrayRate",             LuaTabArrayRate},       //对比概率，取出合适的行数
	{"SendWelcomeMsg",           LuaSendWelcomeMsg},
    {"SyncItemInfo",             LuaSyncItemInfo},       //设置客户端 物品说明  SetItemInfo
	{"SetChouJiangCishu",        LuaSetChouJiangCishu},  //设置抽奖次数
	{"WriteTabString"   ,        LuaWriteTabString},     //写入文件
	{"GetTabString"   , 	     LuaGetTabString},       //获取某行某列字符串
	{"RemoveTabRow"   , 	     LuaRemoveTabRow},       //删除某行内容

	{"SubWorldName"  , 	         LuaSubWorldName},       //获取地图的名称
	{"SubWorldID2Idx",	         LuaSubWorldIDToIndex},  //SubWorldID2Idx 获取 地图编号的所在地图列表里第几行
	{"Idx2SubWorldID",	         LuaIndexToSubWorldID},  //
	{"SubWorldIdx2ID",	         LuaIndexToSubWorldID},
	{"GetNpcCountInMap",	     LuaGetNpcCountInMap},   //	获取某个地图上的NPC数量
	{"MovePlayerOutMap",	     LuaMovePlayerOutMap},	 // 把某个地图上的玩家全部移出地图
	{"MovePlayerInMap",	         LuaMovePlayerInMap},
	
//   ----------------------经脉设置-----------------------
    {"SetMaiTypeAndLevel",	     LuaSetMaiTypeAndLevel}, 
	{"SetJinMaiVal",	         LuaSetJinMaiVal},       //设置真元和兵甲值
    {"GetJinMaiInfo",	         LuaGetJinMaiInfo},
	{"InitJinMaiVal",            LuaInitJinMaiVal},	
	{"GetIsUseWaigua",           LuaGetIsUseWaigua},	 //获取是否使用外挂
    {"SynUpdataCurData",         LuaSynUpdataCurData},
//----------------------股子设置-----------------------
	{"SetSubGuZuoWei",	LuaSetSubGuZuoWei},              //设定地图城主 或 帮会
    {"SetSubGuZiInit",	LuaSetSubGuZiInit},              //股子游戏初始化化;
	{"SetSubWhoIsStart",LuaSetSubWhoIsStart},            //设置谁为当前的甩股者;
	{"SetSubYaJin",     LuaSetSubYaJin},                 //设置押金;
	{"GetSubYaJin",     LuaGetSubYaJin},                 //设置押金;
	{"CheckZuoWei",     LuaCheckZuoWei},                 //检查座位号是否有人了  
	{"CheckNameIsOn",   LuaCheckNameIsOn},               //检查是否参加了
//----------------------设置活动经验 幸运--------------
	{"SetJinYan",       LuaSetJinYan},
    {"SetXinYun",       LuaSetXinYun},
	{"SetNpcLucky",     LuaSetNpcLucky},	
	{"SetJinQian",      LuaSetJinQian},
	{"LoldDropRate",    LuaLoldDropRate},                //加载爆率
	{"UpdataDropRate",  LuaUpdataDropRate},              //更新源爆率
	{"GetDropRateName", LuaGetDropRateName},             //获取某个NPC所加载的爆率文件路径
	{"SetItemUseTimeNew",	LuaSetItemUseTimeNew},	             //设置物品的使用时间
	{"SetItemUseTime",	LuaSetItemUseTime},	             //设置物品的使用时间
	{"AddItemUseTime",	LuaAddItemUseTime},	             //增加某物品的使用时间
	{"IsHaveUseTime",	LuaIsHaveUseTime},               //是否是限时装备
	{"SetItemLock",		LuaLockItem},
	{"GetItemLock",		LuaGetLockItem},
	{"LockItemBind",	LuaLockItemBind},

	{"GetNameItem",		LuaGetNameItem},
	{"CheckRoom",			LuaCheckRoom},
	{"FindItemEx",		LuaFindItemEx},
//----------------------纹钢熔炼装备强化----------------
    {"ItemQiangHua",             LuaItemQiangHua},         //装备强化
	{"ItemRongLian",  	         LuaItemRongLian},         //熔炼
	{"ItemWenGang",  	         LuaItemWenGang},          //纹钢
	{"ItemTilian",  	         LuaItemTiLian},           //提炼重铸一起
	{"ChcekWenPos",  	         LuaChcekWenPos},          //检测是位置否被纹钢并返回纹钢的属性ID
	{"ChcekWenPinAndLevel",      LuaChcekWenPinAndLevel},  //检测品质
	{"ChcekBulePinAndLevel",     LuaChcekBulePinAndLevel}, //检测蓝装品质
	{"GiveQuestItem", 	         LuaGiveQuestItem},        //给纹钢
//////////////////////////////////////////////////////////////////////////////////////////
	{"IsBoss",	        LuaIsBoss},
	{"SetNpcBoss",	        LuaIsBoss},
	{"GetNpcBoss",	    LuaGetNpcBoss},
	{"Tremble",		    LuaTremble},                              //打开打造装备UI
	{"OpenWenGang",		LuaOpenwengang},                          //打开文刚UI
	{"Openstone",		LuaOpenstone},                            //打开宝石镶嵌
	{"OpenDuanZhao", LuaOpenDuanZhao},
	{"OpenVipDaZao", LuaOpenVipDaZao},
	//uy thac
	{"IsLiXian",        LuaIsLiXian},
	{"NotLiXian",       LuaNotLiXian},
	{"OfflineLive",	LuaOfflineLive},
    {"OfflineLiveAll",	LuaOfflineLiveAll},
    {"GetPlayerIndexByAccName",	LuaGetPlayerIndexByAccName},
    {"FindPlayerByName",	LuaGetPlayerIndexByAccName},

	{"SetMask",         LuaSetMask}, //面具
	{"ReSetMask",       LuaReSetMask},

	{"SetNpcMask",      LuaSetNpcMask},	  //改变NPC 外观

	{"SetPifeng",       LuaSetPifeng},    //披风
	{"SetChiBang",      LuaSetChiBang},   //翅膀
	{"ReSetPifeng",     LuaReSetPifeng},
    
	{"Give",			LuaGive},
	{"GiveItemUI",		LuaGiveItemUI},
	{"GetGiveItemUnit",	LuaGetGiveItemUnit},
	//24102021:Haint add new api BT
	{ "BT_SetView",  LuaBT_SetView },
	{ "BT_LeaveBattle",  LuaBT_LeaveBattle },
	{ "BT_UpdateMemberCount", LuaBT_UpdateMemberCount },
	{ "BT_ViewBattleStart", LuaBT_ViewBattleStart },
	{ "BT_BroadAllLadder", LuaBT_BroadAllLadder },
	{ "BT_BroadGameData", LuaBT_BroadGameData },
	{ "BT_SetRestTime", LuaBT_SetRestTime },
	{ "BT_BroadView", LuaBT_BroadView },
	{ "BT_ClearPlayerData",  LuaBT_ClearPlayerData },
	{ "BT_SortLadder",  LuaBT_SortLadder },
	{ "BT_BroadSelf",  LuaBT_BroadSelf },
	{ "BT_WorldLadder",  LuaBT_WorldLadder },
	{ "BT_SongJin",  LuaBT_SongJin },
	{ "BT_SetData", LuaBT_SetData },
	{ "BT_GetData", LuaBT_GetData },
	{ "BT_SetGameData",	LuaBT_SetGameData },
	{ "BT_GetGameData",	LuaBT_GetGameData },
	{ "Remote", LuaRemote},
	{ "SetTmpCamp", LuaSetTmpCamp},
	{ "RestoreCampTong", LuaRestoreCampTong },
	{ "GetMapDropRateFile", LuaGetMapDropRateFile },



	

	
	
	//客户端脚本指令
#else 
	{"PlaySound",       LuaPlaySound},      //PlaySound(Sound);
	{"PlaySprMovie",    LuaPlaySprMovie},   //PlaySprMovie(npcindex, Movie, times)
	{"AutoForbitGua",   LuaAutoForbitGua},  //客户端执行自动封挂
	{"CastSkill",       LuaCastSkill},      //瞬间特效
	{"NewTask_AddIcon", LuaNewTask_AddIcon},
	{"NewTask_TaskTextOut", LuaNewTask_TaskTextOut},
	{"NewTask_DetailTextOut", LuaNewTask_DetailTextOut},

	{"NewTask_SetFinishedTask", LuaNewTask_SetFinishedTask},   //设置完成任务
	
	{"NewTask_AddNpcFindPath",  LuaNewTask_AddNpcFindPath},     //设置信息关联寻路
	//{"dostring", Luadostring},
#endif
	{"GetSourceTime",	LuaGetSourceTime},
	{"GetCurrentTime",	LuaGetCurrentTime},

	{"SetNumber",			LuaSetNumber},
	{"GetNumber",			LuaGetNumber},

	{"PlayMusic",       LuaPlayMusic}, //PlayMusic(Music,Loop)
	{"FadeInMusic",     LuaFadeInMusic},
	{"FadeOutMusic",    LuaFadeOutMusic},
};


TLua_Funcs WorldScriptFuns[] =// 非指对玩家的脚本指令集
{
	//通用指令
	{"AddLocalNews",     LuaAddLocalNews},
	{"AddSysNews",       LuaAddSysNews},
	{"AddLoaclTimeNews", LuaAddLocalTimeNews},
	{"AddLocalCountNews",LuaAddLocalCountNews},

	{"AddGlobalNews_All",LuaAddGlobalNews_All},
	{"AddGlobalCountNews_All",LuaAddGlobalCountNews_All},
	{"AddLocalNews_All",LuaAddLocalNews_All},

	//服务器端脚本指令
#ifdef _SERVER
	{"SetSubWarCityGuanYuan",    LuaSetSubWarCityGuanYuan},  //设置城市官员
	{"SetSubMapPKmodle",  	     LuaSetSubMapPKmodle},       //设置地图的PK模式
	{"SetSubMapFightModle",  	 LuaSetSubMapFightmodle},    //设置地图战斗模式
	{"SetSubMapFightComp",  	 LuaSetSubMapFightComp},	 //地图的战斗阵营
	{"SetSubMapShowInfo",  	     LuaSetSubMapShowInfo},		 //显示基本信息

	//-----------------------------------------------
    //{"SetNpcliveType",LuaSetNpcliveType}, //设置类型 
	//{"SetNpcLiveTime",LuaSetNpcLiveTime}, //设置存活时间
	//{"GetNpcLiveTime",LuaGetNpcLiveTime}, //设置存活时间
	//{"AddNpcLiveTime",LuaAddNpcLiveTime}, //增加存活时间

	{"GetNpcReciveNum",LuaGetNpcReciveNum},//获取重生的次数
    {"SetNpcCurPathPos",LuaSetNpcCurPathPos},
	{"SetNpcIsDel",LuaSetNpcIsDel},        //设置是否直接杀死NPC
    {"SynNpcCur64Dir",LuaSynNpcCur64Dir},
	//-------------------------------------------------
	{"Addyabiao",		LuaAddyabiao},
	{"AddNpc",			LuaAddNpc},			     //AddNpc(人物模板id或人物模板名,所处世界id，点坐标x,点坐标y),返回npcid值
	{"DelNpc",			LuaDelNpc},			     //DelNpc(Npcid)

    {"SetBaseDamage",   LuaSetBaseDamage},  
    {"NpcSayToSubWorld",LuaNpcSayToSubWorld},
	{"CheckNpcName",	LuaCheckNpcName}, 
	{"Msg2SubWorld",	LuaMsgToSubWorld},	//Msg2SubWorld(消息)通知世界
	{"MsgToSubWorldMap",LuaMsgToSubWorldMap},
	{"MsgToSubSerVerWorld",LuaMsgToSubSerVerWorld},      //向某个服务器发送消息
	{"SetSubSerGlobalMission",LuaSetSubSerGlobalMission},//设置某个服务器的 goaldmissval 值
    {"KickOutPlayerInGame",LuaKickOutPlayerInGame},      //踢掉某个服务器的跨服玩家
	{"KickOutPlayerThisGame",LuaKickOutPlayerThisGame},  //踢掉本区所有的跨服玩家
	{"ForceTransWorld",	LuaForceTransWorld},             //强行转服
	
	{"Msg2IP",			LuaMsgToIP}, //Msg2IP(IP, ID, StrInfo)	
	{"SubWorldID2Idx",	LuaSubWorldIDToIndex}, //SubWorldID2Idx	 SubWorldIdx2ID
	{"SubWorldIdx2ID",	LuaIndexToSubWorldID},
	
	{"Idx2SubWorldID",  LuaIndexToSubWorldID},  //
	{"SubWorldName"  , 	LuaSubWorldName},       //获取地图的名称
	{"GetServerName",	LuaGetServerName},
	{"KickOutPlayer",	LuaKickOutPlayer},
	{"KickOutAccount",	LuaKickOutAccount},
	{"ShutDownClient",	LuaShutdownClient},
	
    {"SetOrAddTrap",    LuaSetOrAddTrap},  //某地图 某坐标 新增加 或 修改 一个TRAP点
	
	{"ClearMapNpc",    LuaClearMapNpc},
	{"ClearMapTrap",    LuaClearMapTrap},
	{"ClearMapObj",    LuaClearMapObj},
	{"ClearObstacleObj",    LuaClearObstacleObj},
	{"findNpcPosWithName",    LuaFindNpcPosWithName},

	{"AddMapTrap",    LuaAddMapTrap},
	{"AddObstacleObj",    LuaAddObstacleObj},

	{"BuildATeam",    LuaBuildATeam},
	
	{"DynamicExecuteByPlayer",  LuaDynamicExecuteByPlayer},
	{"callPlayerFunction",  LuacallPlayerFunctionr},

	{"GetMissionV",     LuaGetMissionValue},//GetMissionV(Vid)
	{"SetMissionV",     LuaSetMissionValue},//SetMissionV(Vid, Value)

	{"GetMissionS",     LuaGetMissionValueS},//GetMissionV(Vid)
	{"SetMissionS",     LuaSetMissionValueS},//SetMissionV(Vid, Value)

	{"GetGlbMissionV",  LuaGetGlobalMissionValue},
	{"SetGlbMissionV",  LuaSetGlobalMissionValue},
	{"GetMSLadder",LuaGetMSLadder},
	{"GetMissionName", LuaGetMissionName},
	{"OpenMission",     LuaInitMission},//OpenMission(missionid)
	{"RunMission",      LuaRunMission},
	{"CloseMission",    LuaCloseMission},//CloseMission(missionid)
	{"StartMissionTimer", LuaStartMissionTimer},////StartMissionTimer(missionid, timerid, time)
	{"StopMissionTimer",  LuaStopMissionTimer},
	{"GetMSRestTime",     LuaGetMissionRestTime}, //GetMSRestTime(missionid, timerid)
	{"GetMSIdxGroup",     LuaGetPlayerMissionGroup},//GetPlayerGroup(missionid, playerid);
	
	{"AddMSPlayer",       LuaAddMissionPlayer},
	{"DelMSPlayer",       LuaRemoveMissionPlayer},
	{"GetNextPlayer",     LuaGetNextPlayer},
	{"PIdx2MSDIdx",       LuaGetMissionPlayer_DataIndex},//(missionid, pidx)
	{"MSDIdx2PIdx",       LuaGetMissionPlayer_PlayerIndex},//(missionid, dataidx)
	{"NpcIdx2PIdx",       LuaNpcIndexToPlayerIndex},
    {"PIdx2NpcIdx",       LuaPlayerIndexToNpcIndex},

	{"GetMSPlayerCount",  LuaMissionPlayerCount},//GetMSPlayerCount(missionid, group = 0)

	{"RevivalAllNpc",	 LuaRevivalAllNpc}, //复活NPC
	{"DelAllNpc",	     LuaDelAllNpcInWro}, //删除NPC
    {"DelTypeNpc",	     LuaDelTypeNpcInWro},	

	{"SetPMParam",       LuaSetMissionPlayerParam },
	{"GetPMParam",       LuaGetMissionPlayerParam},

	{"SetRankParam",        LuaSetMissionRankParam},
	{"GetRankParam",        LuaGetMissionRankParam},
	{"UpRankParam",         LuaUpMissionRankParam},

	{"Msg2MSGroup",      LuaMissionMsg2Group},
	{"Msg2MSAll",        LuaMissionMsg2All},
	{"Msg2MSPlayer",     LuaMissionMsg2Player},

#endif
}; 

int g_GetGameScriptFunNum()
{
	return sizeof(GameScriptFuns)  / sizeof(GameScriptFuns[0]);
}

int g_GetWorldScriptFunNum()
{
	return sizeof(WorldScriptFuns)  / sizeof(WorldScriptFuns[0]);
}
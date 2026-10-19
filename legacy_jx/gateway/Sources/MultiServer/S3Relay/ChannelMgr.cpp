// ChannelMgr.cpp: implementation of the CChannelMgr class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Global.h"
#include "ChannelMgr.h"
#include "S3Relay.h"
#include "time.h"
#include "malloc.h"

#include <list>
#include <map>

/////////////////////////////////////////////////////

static const size_t MAX_SENTLEN = 256;

static const char NUMBER_CHARS[] = "-0123456789";
static const char NULL_STR[]  = "系统默认错误";
static const char NULL_STRA[] = "字符串太长";
static const char NULL_STRB[] = "字符串里有乱码";
static const char NULL_STRC[] = "门派有错误";
static const char NULL_STRD[] = "帮会有错误";
static const char NULL_STRE[] = "队伍有错误";
static const char NULL_STRF[] = "其他错误";
/////////////////////////////////////////////////////


const char file_channcfg[]	= "relay_channcfg.ini";

const char sec_system[]		= "system";
const char key_nameGM[]		= "nameGM";
const char key_charEsc[]	= "charEsc";  //斜杠
const char key_charSplt[]	= "charSplt";  //冒号
const char key_defCost[]	= "defCost";

const char sec_team[]		= "team";
const char sec_faction[]	= "faction";
const char sec_tong[]		= "tong";

const char sec_msgr[]		= "msgr";

const char sec_screen[]		= "screen";
const char sec_broadcast[]	= "broadcast";
const char key_escSpec[]	= "escSpec";
const char key_minID[]		= "minID";
const char key_maxID[]		= "maxID";

const char file_relaychann[]	= "relay_channel.ini";
const char key_cost[]			= "cost";

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CChannelMgr::CChannelMgr()
	: m_lastChannID(0), m_channidGM(-1),
	  m_chChannEsc(0), m_chChannSplt(0),
	  m_chChannTEAM(0), m_chChannFAC(0),
	  m_chChannTONG(0), m_chChannSCRN(0),
	  m_minTeamID(0), m_maxTeamID(-1), 
	  m_minFacID(0), m_maxFacID(-1), 
	  m_minTongID(0), m_maxTongID(-1),
	  m_minMsgrID(0), m_maxMsgrID(-1),
	  m_defCost(0),m_costTeam(0),
	  m_costFac(0), m_costTong(0),m_costMsgr(0),
	  m_costScrn(0), m_costBc(0)
{
 //买卖
}

CChannelMgr::~CChannelMgr()
{

}

//初始化创建频道
BOOL CChannelMgr::Initialize()
{
	DUMMY_AUTOLOCKWRITE(m_lockChannel);

	m_chChannEsc  = gGetPrivateProfileCharEx(sec_system, key_charEsc, file_channcfg, defCHANN_esc);
	m_chChannSplt = gGetPrivateProfileCharEx(sec_system, key_charSplt, file_channcfg, defCHANN_splt);

	m_chChannTEAM = gGetPrivateProfileCharEx(sec_team, key_escSpec, file_channcfg, defCHANN_TEAM);
	m_chChannFAC  = gGetPrivateProfileCharEx(sec_faction, key_escSpec, file_channcfg, defCHANN_FAC);
	m_chChannTONG = gGetPrivateProfileCharEx(sec_tong, key_escSpec, file_channcfg, defCHANN_TONG);
	m_chChannMSGR  = gGetPrivateProfileCharEx(sec_msgr, key_escSpec, file_channcfg, defCHANN_MSGR);
	m_chChannSCRN = gGetPrivateProfileCharEx(sec_screen, key_escSpec, file_channcfg, defCHANN_SCRN);
	m_chChannBC   = gGetPrivateProfileCharEx(sec_broadcast, key_escSpec, file_channcfg, defCHANN_BC);
 //   m_chChannMM   = gGetPrivateProfileCharEx(sec_maimai, key_escSpec, file_channcfg, defCHANN_MM); //买卖

	m_minTeamID	= gGetPrivateProfileIntEx(sec_team, key_minID, file_channcfg, 0);
	m_maxTeamID	= gGetPrivateProfileIntEx(sec_team, key_maxID, file_channcfg, -1);
	m_minFacID	= gGetPrivateProfileIntEx(sec_faction, key_minID, file_channcfg, 0);
	m_maxFacID	= gGetPrivateProfileIntEx(sec_faction, key_maxID, file_channcfg, -1);
	m_minTongID	= gGetPrivateProfileIntEx(sec_tong, key_minID, file_channcfg, 0);
	m_maxTongID	= gGetPrivateProfileIntEx(sec_tong, key_maxID, file_channcfg, -1);

	m_minMsgrID	= gGetPrivateProfileIntEx(sec_msgr, key_minID, file_channcfg, 0);
	m_maxMsgrID	= gGetPrivateProfileIntEx(sec_msgr, key_maxID, file_channcfg, -1);


	m_defCost = (BYTE)gGetPrivateProfileIntEx(sec_system, key_defCost, file_channcfg, 0);

	m_costTeam = (BYTE)gGetPrivateProfileIntEx(sec_team, key_cost, file_channcfg, (INT)(UINT)m_defCost);
	m_costFac  = (BYTE)gGetPrivateProfileIntEx(sec_faction, key_cost, file_channcfg, (INT)(UINT)m_defCost);
	m_costTong = (BYTE)gGetPrivateProfileIntEx(sec_tong, key_cost, file_channcfg, (INT)(UINT)m_defCost);
	m_costMsgr = (BYTE)gGetPrivateProfileIntEx(sec_msgr, key_cost, file_channcfg, (INT)(UINT)m_defCost);

	m_costScrn = (BYTE)gGetPrivateProfileIntEx(sec_screen, key_cost, file_channcfg, (INT)(UINT)m_defCost);
	m_costBc   = (BYTE)gGetPrivateProfileIntEx(sec_broadcast, key_cost, file_channcfg, (INT)(UINT)m_defCost); 
//	m_costMm   = (BYTE)gGetPrivateProfileIntEx(sec_maimai, key_cost, file_channcfg, (INT)(UINT)m_defCost); //买卖
	std::_tstring nameChannGM = gGetPrivateProfileStringEx(sec_system, key_nameGM, file_channcfg);
	if (!nameChannGM.empty())
	{//GM频道
		//create the channel
		m_channidGM = GenChannID();
		if (m_channidGM == DWORD(-1))
			return FALSE;

		m_mapChann2ID[nameChannGM] = m_channidGM;

		CHANNINFO& rChannInfo = m_mapChannid2Info[m_channidGM];

		rChannInfo.god = TRUE;
		rChannInfo.stock = TRUE;
		rChannInfo.channname = nameChannGM;
		rChannInfo.gmuse = FALSE;
		rChannInfo.gmsub = FALSE;
		rChannInfo.cost = (BYTE)gGetPrivateProfileIntEx(nameChannGM.c_str(), key_cost, file_relaychann, (INT)(UINT)m_defCost);
		//GM 频道
		rTRACE("[初始化]Create Channel: [Stock, GM] <%08X> %s", m_channidGM, nameChannGM.c_str());
	}

    //其他频道（世界）
	const std::vector<std::_tstring>& rVecChann = gGetPrivateProfileSectionNamesEx(file_relaychann);

	for (std::vector<std::_tstring>::const_iterator it = rVecChann.begin(); it != rVecChann.end(); ++it)
	{
		std::_tstring channname = *it;
		assert(!channname.empty());

		if (_tstring_equal()(channname, nameChannGM))	//gm, created already
			continue;

		DWORD channid = GenChannID();  //产生频道ID
		if (channid == DWORD(-1))
			return FALSE;

		m_mapChann2ID[channname] = channid;//全局的频道容器 储存对应频道名称的 id

		CHANNINFO& rChannInfo = m_mapChannid2Info[channid];
		rChannInfo.god = FALSE;
		rChannInfo.stock = TRUE;
		rChannInfo.channname = channname;
		rChannInfo.gmuse = FALSE;
		rChannInfo.gmsub = FALSE;
		rChannInfo.cost = (BYTE)gGetPrivateProfileIntEx(channname.c_str(), key_cost, file_relaychann, (INT)(UINT)m_defCost);
        //GM 频道
		rTRACE("[初始化]Create Channel: [Stock] <%08X> %s", channid, channname.c_str());
	}

	return TRUE;
}

BOOL CChannelMgr::Uninitialize()
{
	DUMMY_AUTOLOCKWRITE(m_lockChannel);

	m_mapChann2ID.clear();
	m_mapChannid2Info.clear();


	m_defCost = 0;

	m_costTeam = 0;
	m_costFac  = 0;
	m_costTong = 0;
	m_costMsgr = 0;
	m_costScrn = 0;
	m_costBc   = 0;
//    m_costMm   = 0; //买卖

	m_channidGM = -1;

	m_chChannEsc = 0;
	m_chChannSplt = 0;
	m_chChannTEAM = 0;
	m_chChannFAC = 0;
	m_chChannSCRN = 0;
	m_chChannBC = 0;
 //   m_chChannMM = 0;

	m_minTeamID	= 0;
	m_maxTeamID	= -1;
	m_minFacID	= 0;
	m_maxFacID	= -1;
	m_minTongID	= 0;
	m_maxTongID	= -1;

	m_minMsgrID	= 0;
	m_maxMsgrID	= -1;


	return TRUE;
}


DWORD CChannelMgr::GenChannID()
{
	//assert(m_lockChannel._IsWriting());

	if (m_lastChannID != DWORD(-1))
		return m_lastChannID++;

	//no base idle, then search idle
	DWORD id = 0;
	for (CHANNID2INFOMAP::iterator it = m_mapChannid2Info.begin(); it != m_mapChannid2Info.end(); ++it)
	{
		if ((*it).first != id)//产生一个没有使用过的 key id
			return id;
		++id;
	}
	return id;
}



std::_tstring CChannelMgr::GetChannelName(DWORD channelid, BOOL adv)
{
	DUMMY_AUTOLOCKREAD(m_lockChannel);

	CHANNID2INFOMAP::const_iterator it = m_mapChannid2Info.find(channelid);
	if (it == m_mapChannid2Info.end())
		return NULL_STR;

	std::_tstring channname = (*it).second.channname;

	return adv ? channname : ReduceChannelName(channname);
}

DWORD CChannelMgr::GetChannelID(const std::_tstring& channname, DWORD advIP)
{
	std::_tstring advChannName = advIP == 0 ? channname : MakeChannelName(channname, advIP);

    //char msg[64];
	//sprintf(msg,"帮会名称A:%s",advChannName);
    //MessageBox(0,msg,"提示:",MB_OK);

	if (advChannName.empty())
		return -1;

	DUMMY_AUTOLOCKREAD(m_lockChannel);

	CHANN2IDMAP::iterator it = m_mapChann2ID.find(advChannName);
	if (it == m_mapChann2ID.end())
	{//找不到帮会
		
		//sprintf(msg,"帮会名称B:%s",advChannName);
        //MessageBox(0,msg,"提示:",MB_OK);

		return -1;
	}

	//sprintf(msg,"结果:%x",(*it).second);
    //MessageBox(0,msg,"提示:",MB_OK);

	return (*it).second;
}


BOOL CChannelMgr::IsSubscribed(DWORD ip, unsigned long param, DWORD channid)
{
	DUMMY_AUTOLOCKREAD(m_lockChannel);

	CHANNID2INFOMAP::iterator itChannid = m_mapChannid2Info.find(channid);
	if (itChannid == m_mapChannid2Info.end())
		return FALSE;

	CHANNINFO& rChannInfo = (*itChannid).second;

	IP2PLAYERSMAP::iterator itIP = rChannInfo.mapIp2Players.find(ip);
	if (itIP == rChannInfo.mapIp2Players.end())
		return FALSE;

	PLAYERSMAP& rPlayers = (*itIP).second;
	PLAYERSMAP::iterator itRole = rPlayers.find(param);
	if (itRole == rPlayers.end())
		return FALSE;

	PLAYERINFO& rInfo = (*itRole).second;
	return rInfo.subed;
}


BOOL CChannelMgr::GmSubscribe(DWORD channid)
{
	if (channid == -1)
		return FALSE;

	DUMMY_AUTOLOCKWRITE(m_lockChannel);

	if (channid == m_channidGM)
		return TRUE;

	CHANNID2INFOMAP::iterator it = m_mapChannid2Info.find(channid);
	if (it == m_mapChannid2Info.end())
		return FALSE;

	CHANNINFO& rChannInfo = (*it).second;
	if (!rChannInfo.gmuse)
		return FALSE;

	rChannInfo.gmsub = TRUE;

	return TRUE;
}

BOOL CChannelMgr::GmUnsubscribe(DWORD channid)
{
	if (channid == m_channidGM)
		return FALSE;

	DUMMY_AUTOLOCKWRITE(m_lockChannel);

	CHANNID2INFOMAP::iterator it = m_mapChannid2Info.find(channid);
	if (it == m_mapChannid2Info.end())
		return FALSE;

	CHANNINFO& rChannInfo = (*it).second;
	if (!rChannInfo.gmuse)
		return FALSE;

	rChannInfo.gmsub = FALSE;

	return TRUE;
}

BOOL CChannelMgr::IsGmSubscribed(DWORD channid)
{
	if (channid == m_channidGM)
		return TRUE;

	DUMMY_AUTOLOCKREAD(m_lockChannel);

	CHANNID2INFOMAP::iterator it = m_mapChannid2Info.find(channid);
	if (it == m_mapChannid2Info.end())
		return FALSE;

	CHANNINFO& rChannInfo = (*it).second;

	return rChannInfo.gmsub;
}

BOOL CChannelMgr::GmQueryChannelID(const std::_tstring& channel, BOOL force)
{
	DUMMY_AUTOLOCKWRITE(m_lockChannel);

	DWORD theChannID = -1;
	BYTE cost = 0;

	CHANN2IDMAP::iterator it = m_mapChann2ID.find(channel);
	if (it != m_mapChann2ID.end())
	{//channel exist
		DWORD channid = (*it).second;

		CHANNINFO& rChannInfo = m_mapChannid2Info[channid];

		rChannInfo.gmuse = TRUE;

		theChannID = channid;
		cost = rChannInfo.cost;
	}
	else
	{
		if (force)
		{
			//erase the channel
			DWORD channid = GenChannID();
			if (channid != DWORD(-1))
			{
				m_mapChann2ID[channel] = channid;

				CHANNINFO& rChannInfo = m_mapChannid2Info[channid];
				rChannInfo.god = FALSE;
				rChannInfo.stock = FALSE;
				rChannInfo.channname = channel;
				rChannInfo.gmuse = TRUE;
				rChannInfo.gmsub = FALSE;
				rChannInfo.cost = GainPreCost(channel);

				rTRACE("GM Create Channel: <%08X> %s", channid, channel.c_str());

				theChannID = channid;
				cost = rChannInfo.cost;
			}
		}

	}

	{{
	size_t pckgsize = sizeof(RELAY_ASKWAY_DATA) + sizeof(PLAYERCOMM_NOTIFYCHANNELID);

	RELAY_ASKWAY_DATA* pAskWayData = (RELAY_ASKWAY_DATA*)_alloca(pckgsize);
	pAskWayData->ProtocolFamily = pf_relay;
	pAskWayData->ProtocolID = relay_c2c_askwaydata;
	pAskWayData->nFromIP = 0;
	pAskWayData->nFromRelayID = -1;
	pAskWayData->seekRelayCount = 0;
	pAskWayData->seekMethod = rm_gm;
	pAskWayData->wMethodDataLength = 0;
	pAskWayData->routeDateLength = sizeof(PLAYERCOMM_NOTIFYCHANNELID);

	PLAYERCOMM_NOTIFYCHANNELID* pPlayerCommNotiChannID = (PLAYERCOMM_NOTIFYCHANNELID*)(pAskWayData + 1);
	pPlayerCommNotiChannID->ProtocolFamily = pf_playercommunity;
	pPlayerCommNotiChannID->ProtocolID = playercomm_s2c_notifychannelid;
	snprintf(pPlayerCommNotiChannID->channel, sizeof(pPlayerCommNotiChannID->channel), "%s", channel.c_str());
	pPlayerCommNotiChannID->channelid = theChannID;
	pPlayerCommNotiChannID->cost = cost;

	g_RootClient.SendPackage(pAskWayData, pckgsize);
	}}

	return TRUE;
}

BOOL CChannelMgr::GmFreeChannID(DWORD channid)
{
	DUMMY_AUTOLOCKWRITE(m_lockChannel);

	CHANNID2INFOMAP::iterator it = m_mapChannid2Info.find(channid);
	if (it == m_mapChannid2Info.end())
		return FALSE;

	CHANNINFO& rChannInfo = (*it).second;

	//ensure
	assert(!rChannInfo.gmsub);
	rChannInfo.gmsub = FALSE;

	rChannInfo.gmuse = FALSE;

	if (!rChannInfo.stock
		&& rChannInfo.mapIp2Players.empty())
	{
		rTRACE("GM Destroy Channel: <%08X> %s", channid, rChannInfo.channname.c_str());

		m_mapChann2ID.erase(rChannInfo.channname);
		m_mapChannid2Info.erase(it);
	}

	return TRUE;
}

BOOL CChannelMgr::IsUsed(DWORD ip, unsigned long param, DWORD channid)
{
	DUMMY_AUTOLOCKREAD(m_lockChannel);

	CHANNID2INFOMAP::iterator itChannid = m_mapChannid2Info.find(channid);
	if (itChannid == m_mapChannid2Info.end())
		return FALSE;

	CHANNINFO& rChannInfo = (*itChannid).second;

	IP2PLAYERSMAP::iterator itIP = rChannInfo.mapIp2Players.find(ip);
	if (itIP == rChannInfo.mapIp2Players.end())
		return FALSE;

	PLAYERSMAP& rPlayers = (*itIP).second;
	PLAYERSMAP::iterator itRole = rPlayers.find(param);

	return itRole != rPlayers.end();
}

BOOL CChannelMgr::IsGmUsed(DWORD channid)
{
	DUMMY_AUTOLOCKREAD(m_lockChannel);

	CHANNID2INFOMAP::iterator it = m_mapChannid2Info.find(channid);
	if (it == m_mapChannid2Info.end())
		return FALSE;

	CHANNINFO& rChannInfo = (*it).second;

	return rChannInfo.gmuse;
}
//频道名字转换 和检测 是否存在了
std::_tstring CChannelMgr::MakeChannelName(const std::_tstring& chann, DWORD ip)
{
//	if (chann.empty())
//		return NULL_STR;

	if (chann[0] != m_chChannEsc)     //双“\\” 双斜杠
		return chann;

	if (chann.size() < 2)             //过滤频道
		return NULL_STRA;

	if (chann[1] == m_chChannTEAM     //组队 T
		|| chann[1] == m_chChannFAC   // 门派 F
		|| chann[1] == m_chChannTONG  //帮派 O
		|| chann[1] == m_chChannMSGR
		) 
	{
	
//		if (chann.size() <= 2)  //过滤掉 系统频道
//			return NULL_STR;

		if (chann.find_last_not_of(NUMBER_CHARS) > 1)  //不显示频道 ，从字符串逆向查找不匹配字符的位置 
			return NULL_STRB;
         
		DWORD id =_ttoi(chann.c_str() + 2);   //Stirng转换成DWORD
   
		if (chann[1] == m_chChannFAC)      //已经有了门派频道 F
			return IsValidFacID(id) ? chann : NULL_STRC;
		if (chann[1] == m_chChannTONG)     //已经有了帮会频道 O
			return IsValidTongID(id) ? chann : NULL_STRD;
		if (ip == 0 || !IsValidTeamID(id)) //已经有了队伍频道 T
			return NULL_STRE;

		if (ip == 0 || !IsValidMsgrID(id))
			return NULL_STR;

        //队伍频道
		char buffer[16];
		std::_tstring channname(chann);
		channname += m_chChannSplt;          //加个冒号 ：
		channname += _ultot(ip, buffer, 10); //将IP转换成 CHAR

		return channname;
	}
	else if (chann[1] == m_chChannSCRN || chann[1] == m_chChannBC/*|| chann[1] == m_chChannMM*/)  //城市和 世界？
	{
		if (chann.size() > 2)  //过滤频道 
			return NULL_STRF;

		char buffer[16];
		std::_tstring channname(chann);
		channname += _ultot(ip, buffer, 10);

		return channname;
	}

	return NULL_STR;
}

std::_tstring CChannelMgr::ReduceChannelName(const std::_tstring& chann)
{
	if (chann.empty())
		return NULL_STR;

	if (chann[0] != m_chChannEsc)
		return chann;

	if (chann.size() < 2)
		return NULL_STR;

	if (chann[1] == m_chChannTEAM)
	{
		int spltpos = chann.find_last_of(m_chChannSplt);
		if (spltpos == -1)
			return NULL_STR;
		return std::_tstring(chann.c_str(), spltpos);
	}
	if (chann[1] == m_chChannFAC || chann[1] == m_chChannTONG)
	{
		return chann;
	}

	if (chann[1] == m_chChannMSGR)
	{
		int spltpos = chann.find_last_of(m_chChannSplt);
		if (spltpos == -1)
			return NULL_STR;
		return std::_tstring(chann.c_str(), spltpos);
	}

	if (chann[1] == m_chChannSCRN || chann[1] == m_chChannBC/* || chann[1] == m_chChannMM*/) //买卖
	{
		return std::_tstring(chann.c_str(), 2);
	}

	assert(FALSE);
	return NULL_STR;
}

DWORD CChannelMgr::ParseChannelName(const std::_tstring& chann, DWORD* pIP)
{
	if (pIP != NULL)
		*pIP = 0;

	if (chann.empty())
		return -1;

	if (chann[0] != m_chChannEsc)
		return -1;

	if (chann.size() < 2)
		return -1;

	std::_tstring basechann;
	if (chann[1] == m_chChannTEAM)
	{
		int spltpos = chann.find_last_of(m_chChannSplt);
		if (spltpos == -1)
			return -1;

		if (pIP != NULL)
			*pIP = _ttol(chann.c_str() + spltpos + 1);
		return _ttol(chann.c_str() + 2);
	}
	if (chann[1] == m_chChannFAC
		|| chann[1] == m_chChannTONG)
	{
		return _ttol(chann.c_str() + 2);
	}

	if (chann[1] == m_chChannMSGR)
	{
		int spltpos = chann.find_last_of(m_chChannSplt);
		if (spltpos == -1)
			return -1;
		
		if (pIP != NULL)
			*pIP = _ttol(chann.c_str() + spltpos + 1);
		return _ttol(chann.c_str() + 2);
	}

	if (chann[1] == m_chChannSCRN || chann[1] == m_chChannBC/* || chann[1] == m_chChannMM*/) //买卖
	{
		if (pIP != NULL)
			*pIP = _ttol(chann.c_str() + 2);
		return 0;
	}

	return -1;
}


BYTE CChannelMgr::GainPreCost(const std::_tstring& channel)
{
	if (channel.size() >= 2 && channel[0] == m_chChannEsc)
	{
		if (channel[1] == m_chChannTEAM)
			return m_costTeam;
		else if (channel[1] == m_chChannFAC)
			return m_costFac;
		else if (channel[1] == m_chChannTONG)
			return m_costTong;
		else if (channel[1] == m_chChannMSGR)
			return m_costMsgr;
		else if (channel[1] == m_chChannSCRN)
			return m_costScrn;
		else if (channel[1] == m_chChannBC)
			return m_costBc;
		/*else if (channel[1] == m_chChannMM)  //买卖
			return m_costMm;*/


	}

	return m_defCost;
}


DWORD CChannelMgr::GetGMChannID()
{
	return m_channidGM;
}
//是否GM频道
BOOL CChannelMgr::IsGodChannel(DWORD channid)
{
	DUMMY_AUTOLOCKREAD(m_lockChannel);

	CHANNID2INFOMAP::const_iterator it = m_mapChannid2Info.find(channid);
	if (it == m_mapChannid2Info.end())
		return FALSE;

	const CHANNINFO& rChannInfo = (*it).second;

	return rChannInfo.god;
}
//是储存的频道
BOOL CChannelMgr::IsStockChannel(DWORD channid)
{
	DUMMY_AUTOLOCKREAD(m_lockChannel);

	CHANNID2INFOMAP::const_iterator it = m_mapChannid2Info.find(channid);
	if (it == m_mapChannid2Info.end())
		return FALSE;

	const CHANNINFO& rChannInfo = (*it).second;

	return rChannInfo.stock;
}

BOOL CChannelMgr::PassToSpecMan(DWORD ToIP, unsigned long ToRelayID, DWORD ToNameID, const void* pData, size_t size)
{
	//CNetConnectDup hostconndup;
	CNetConnectDup conndup = g_ChatServer.FindChatConnectByIP(ToIP);//ToIP
	if (!conndup.IsValid())
		return FALSE;

	size_t pckgsize = sizeof(CHAT_SPECMAN) + size;

	CHAT_SPECMAN* pCsm = (CHAT_SPECMAN*)_alloca(pckgsize);
	pCsm->ProtocolType = chat_specman;
	pCsm->wSize        = pckgsize - 1;
	pCsm->nameid       = ToNameID;
	pCsm->lnID         = ToRelayID;
	pCsm->wChatLength  = size;

	void* pExPckg = pCsm + 1;
	memcpy(pExPckg, pData, size);
//dTRACE("<<<<<<<<<私聊信息发往GSB(%s)>>>>>>>",_ip2a(hostconndup.GetIP()));
	conndup.SendPackage(pCsm, pckgsize);
    dTRACE("-------------(%08X)私聊信息发往GS成功!!-----------",ToIP);
	return TRUE;
}

BOOL CChannelMgr::PassToGroupMan(DWORD FromIP,DWORD ToIP, BOOL verify, const MANLIST& mans, const void* pData, size_t size)
{
	CNetConnectDup hostconndup;
	if (mans.empty())
	{
		return TRUE;
	}
	const size_t playerlen = verify ? sizeof(tagPlusSrcInfo) : sizeof(WORD);
	size_t basesize = sizeof(CHAT_GROUPMAN) + size;
	if (basesize + playerlen > max_packagesize)
		return FALSE;

	CNetConnectDup conndup =  g_ChatServer.FindChatConnectByIP(ToIP);//g_HostServer.FindHostConnectByIP(ToIP);//ToIP 
	if (!conndup.IsValid())
	{
		rTRACE("----发送到 GS(%08X)PassToGroupMan消息失败-----",ToIP);
		return FALSE;
	}


	BYTE buffer[max_packagesize];

	CHAT_GROUPMAN* pCgm = (CHAT_GROUPMAN*)buffer;
	pCgm->ProtocolType = chat_groupman;
	pCgm->wChatLength = size;
	pCgm->byHasIdentify = (BYTE)verify;

	void* pExPckg = pCgm + 1;
	memcpy(pExPckg, pData, size);


	size_t cursor = basesize;

	for (MANLIST::const_iterator it = mans.begin(); it != mans.end(); ++it)
	{
		if (verify)
			*(tagPlusSrcInfo*)(buffer + cursor) = *it;
		else
			*(WORD*)(buffer + cursor)= (WORD)(*it).lnID;
		cursor += playerlen;

		if (cursor + playerlen > max_packagesize)
		{
			assert((cursor - basesize) % playerlen == 0);

			pCgm->wSize = cursor - 1;
			pCgm->wPlayerCount = (cursor - basesize) / playerlen;

			conndup.SendPackage(buffer, cursor);
			//g_ChatServer.BroadPackage(buffer,cursor);
			rTRACE("-----------PassToGroupMan 发送(%08X)消息成功------------",ToIP);
			cursor = basesize;
		}
	}

	if (cursor > basesize)
	{
		assert((cursor - basesize) % playerlen == 0);

		pCgm->wSize = cursor - 1;
		pCgm->wPlayerCount = (cursor - basesize) / playerlen;

		//g_ChatServer.BroadPackage(buffer,cursor);
		conndup.SendPackage(buffer, cursor);
		rTRACE("----PassToGroupMan 发送(%08X)消息成功-----",ToIP);
	}
	
	/*g_ChatServer.BroadPackage(buffer,cursor);
	rTRACE("----PassToGroupMan 发送(%08X)消息成功-----",ToIP);*/

	return TRUE;
}


BOOL CChannelMgr::PassToGM(const void* pData, size_t size)
{
	size_t pckgsize = sizeof(RELAY_ASKWAY_DATA) + size;

	RELAY_ASKWAY_DATA* pAskWayData = (RELAY_ASKWAY_DATA*)_alloca(pckgsize);
	pAskWayData->ProtocolFamily = pf_relay;
	pAskWayData->ProtocolID = relay_c2c_askwaydata;
	pAskWayData->nFromIP = 0;
	pAskWayData->nFromRelayID = 0;
	pAskWayData->seekRelayCount = 0;
	pAskWayData->seekMethod = rm_gm;
	pAskWayData->wMethodDataLength = 0;
	pAskWayData->routeDateLength = size;

	void* pExPckg = pAskWayData + 1;
	memcpy(pExPckg, pData, size);

	g_RootClient.SendPackage(pAskWayData, pckgsize);

	return TRUE;
}

BOOL CChannelMgr::IsValidTeamID(DWORD idTeam)
{
	if (m_minTeamID > m_maxTeamID)
		return TRUE;

	return idTeam >= m_minTeamID && idTeam <= m_maxTeamID;
}

BOOL CChannelMgr::IsValidFacID(DWORD idFac)
{
	if (m_minFacID > m_maxFacID)
		return TRUE;

	return idFac >= m_minFacID && idFac <= m_maxFacID;
}

BOOL CChannelMgr::IsValidTongID(DWORD idTong)
{
	if (m_minTongID > m_maxTongID)
		return TRUE;

	return idTong >= m_minTongID && idTong <= m_maxTongID;
}


BOOL CChannelMgr::IsValidMsgrID(DWORD idMsgr)
{
	if (m_minMsgrID > m_maxMsgrID)
		return TRUE;
	
	return idMsgr >= m_minMsgrID && idMsgr <= m_maxMsgrID;
}


//密聊
BOOL CChannelMgr::SomeoneChat(DWORD srcIP, const tagPlusSrcInfo& SrcInfo, const CHAT_SOMEONECHAT_CMD* pSomeoneChatCmd)
{
	if (pSomeoneChatCmd->sentlen <= 0 || pSomeoneChatCmd->sentlen > MAX_SENTLEN)
		return FALSE;

	if (!gIsLegalString(pSomeoneChatCmd->someone, 1, NAME_LENGHT))
		return FALSE;

	std::_tstring srcrole;
	//if (!g_HostServer.FindPlayerByIpParam(NULL, srcIP, SrcInfo.lnID, NULL, NULL, &srcrole, NULL))
	if (!g_ChatServer.FindPlayerByIpParam(NULL, srcIP, SrcInfo.lnID, NULL, NULL, &srcrole, NULL,false))  //发送人
		return FALSE;

	BOOL sent = FALSE;

	CNetConnectDup hostconndup;
	DWORD nameid = 0;
	unsigned long param = 0;
	//if (g_HostServer.FindPlayerByRole(NULL, std::_tstring(pSomeoneChatCmd->someone), &hostconndup, NULL, &nameid, &param))
	if (g_ChatServer.FindPlayerByRole(NULL, std::_tstring(pSomeoneChatCmd->someone), &hostconndup, NULL, &nameid, &param))
	{//对方所在的gs
		size_t syncsize = sizeof(CHAT_SOMEONECHAT_SYNC) + pSomeoneChatCmd->sentlen;
		CHAT_SOMEONECHAT_SYNC* pCscSync = (CHAT_SOMEONECHAT_SYNC*)_alloca(syncsize);
		memset(pCscSync, 0, syncsize);
		pCscSync->ProtocolType = chat_someonechat; //对方
		pCscSync->wSize        = syncsize - 1;
		pCscSync->packageID    = pSomeoneChatCmd->packageID;
		pCscSync->nItemDwid    = pSomeoneChatCmd->nItemDwid;

		snprintf(pCscSync->someone, sizeof(pCscSync->someone), "%s", srcrole.c_str()); //发送人的名字
		pCscSync->sentlen      = pSomeoneChatCmd->sentlen;
		memcpy(pCscSync + 1, pSomeoneChatCmd + 1, pSomeoneChatCmd->sentlen);

		//CNetConnectDup conndupA = g_ChatServer.FindChatConnectByIP(hostconndup.GetIP());
		if (hostconndup.IsValid())
		{//对方所在的gs
		  size_t pckgsize = sizeof(CHAT_SPECMAN) + syncsize;
		  CHAT_SPECMAN* pCsm = (CHAT_SPECMAN*)_alloca(pckgsize);

		  memset(pCsm, 0, pckgsize);

		  pCsm->ProtocolType = chat_specman;
		  pCsm->wSize        = pckgsize - 1;
		  pCsm->nameid      = nameid;
		  pCsm->lnID         = param;
		  pCsm->wChatLength  = syncsize;		
		  void* pExPckg      = pCsm + 1;
		  memcpy(pExPckg, pCscSync, syncsize);
		  int nRegA=0; 
		  nRegA=hostconndup.SendPackage(pCsm, pckgsize);//要区分是哪个GS

          if (nRegA)
		  {  
		    dTRACE("--------------私聊信息(%s)发往对方GS:%08X 成功!!------------",pSomeoneChatCmd->someone,hostconndup.GetIP());
	        sent = TRUE;
		  } 
		}
		else
			 dTRACE("--------------私聊信息(%s)发往对方GS:%08X 失败!!------------",pSomeoneChatCmd->someone,hostconndup.GetIP());

	}
	else
	  dTRACE("--------------私聊信息(%s)发往对方GS失败,不在线!!------------",pSomeoneChatCmd->someone);
//////////////////////////////////////////////////////////////////////////////
/*/if (sent==TRUE)
{//私聊回调函数
	size_t _exsize = sizeof(DWORD) + _NAME_LEN + sizeof(BYTE) + pSomeoneChatCmd->sentlen;
	size_t feedbacksize = sizeof(CHAT_FEEDBACK) + _exsize;
	CHAT_FEEDBACK* pCfb = (CHAT_FEEDBACK*)_alloca(feedbacksize);
	memset(pCfb, 0, sizeof(pCfb));
	pCfb->ProtocolType  = chat_feedback;  //本人
	pCfb->packageID     = pSomeoneChatCmd->packageID;
	pCfb->code          = sent ? codeSucc : codeFail;  //成功 或失败

	DWORD* pChannelid = (DWORD*)(pCfb + 1);
	*pChannelid    = -1;
	char* pDstName = (char*)(pChannelid + 1);
	strcpy(pDstName, pSomeoneChatCmd->someone);
	BYTE* pSentlen = (BYTE*)(pDstName + _NAME_LEN);
	*pSentlen      = pSomeoneChatCmd->sentlen;
	void* pSent    = pSentlen + 1;
	memcpy(pSent, pSomeoneChatCmd + 1, pSomeoneChatCmd->sentlen);
//-----------------------PassToSpecMan-------------------------------------
	//PassToSpecMan(srcIP, SrcInfo.lnID, SrcInfo.nameid, pCfb, feedbacksize);

	CNetConnectDup conndupB = g_ChatServer.FindChatConnectByIP(srcIP);
	if (conndupB.IsValid())
	{
		size_t pckgsizeb = sizeof(CHAT_SPECMAN) + feedbacksize;
		CHAT_SPECMAN* pCsmb = (CHAT_SPECMAN*)_alloca(pckgsizeb);

		 memset(pCsmb, 0, sizeof(pCsmb));

		pCsmb->ProtocolType = chat_specman;
		pCsmb->wSize        = pckgsizeb - 1;
		pCsmb->nameid       = SrcInfo.nameid;  
		pCsmb->lnID         = SrcInfo.lnID;
		pCsmb->wChatLength  = feedbacksize;
		
		void* pExPckgb = pCsmb + 1;
		memcpy(pExPckgb, pCfb, feedbacksize);
		int nRegB=0;
	    nRegB=conndupB.SendPackage(pCsmb, pckgsizeb);
	   if (nRegB==1)
		   dTRACE("<<<<<<<<<<私聊信息发往自己GS-(%s)成功!!:>>>>>>>>",_ip2a(srcIP));
	}
	
//-----------------------------------------------------------------------------
}
*//////////////////////////////////////////////////////////////////////////////////
//	dTRACE("私聊信息发往GS(%s): 长度 %d -- from [%s] to [%s]",_ip2a(srcIP) ,pSomeoneChatCmd->sentlen, srcrole.c_str(), pSomeoneChatCmd->someone);
	return TRUE;
}

//
BOOL CChannelMgr::ChannelChat(DWORD srcIP, const tagPlusSrcInfo& SrcInfo, const CHAT_CHANNELCHAT_CMD* pChannelChatCmd)
{
	// CNetConnectDup hostconndup;
	if (pChannelChatCmd->channelid == -1)
	{
		rTRACE("----发送(%08X区)ChannelChat消息失败A-----",srcIP);
		return FALSE;
	}
	if (pChannelChatCmd->sentlen <= 0 || pChannelChatCmd->sentlen > MAX_SENTLEN)
	{
		rTRACE("----发送(%08X区)消息ChannelChat失败B-----",srcIP);
		return FALSE;
	}

	std::_tstring srcacc;
	std::_tstring srcrole;
	//if (!g_HostServer.FindPlayerByIpParam(NULL,srcIP,SrcInfo.lnID, NULL, &srcacc, &srcrole, NULL))
	if (!g_ChatServer.FindPlayerByIpParam(NULL,srcIP,SrcInfo.lnID, NULL, &srcacc, &srcrole, NULL))
	{
		rTRACE("----发送(%08X区)消息ChannelChat失败C-----",srcIP);
		return FALSE;
	}


	BOOL sent = FALSE;


	DUMMY_AUTOLOCKREAD(m_lockChannel);


	CHANNID2INFOMAP::iterator itChannid = m_mapChannid2Info.find(pChannelChatCmd->channelid);
	if (itChannid == m_mapChannid2Info.end())
	{
		rTRACE("----发送(%08X区)消息ChannelChat失败D-----",srcIP);
		return FALSE;
	}
	CHANNINFO& rChannInfo = (*itChannid).second;

	if (pChannelChatCmd->cost != rChannInfo.cost)
	{

		rTRACE("----发送(%08X区)消息ChannelChat失败E-----",srcIP);
		return FALSE;
	}


	if (rChannInfo.god)
	{//GM频道
		if (g_RootClient.IsReady())
		{
			size_t pckgsize = sizeof(CHAT_MSG_EX) + pChannelChatCmd->sentlen;
			CHAT_MSG_EX* pChatMsgEx = (CHAT_MSG_EX*)_alloca(pckgsize);
			pChatMsgEx->ProtocolFamily = pf_playercommunity;
			pChatMsgEx->ProtocolID = playercomm_channelchat;
			snprintf(pChatMsgEx->m_szSourceName, sizeof(pChatMsgEx->m_szSourceName), "%s", srcrole.c_str());
			snprintf(pChatMsgEx->m_szAccountName, sizeof(pChatMsgEx->m_szAccountName), "%s", srcacc.c_str());
			pChatMsgEx->SentenceLength = pChannelChatCmd->sentlen;
			memcpy(pChatMsgEx + 1, pChannelChatCmd + 1, pChannelChatCmd->sentlen);

			PassToGM(pChatMsgEx, pckgsize);
			rTRACE("----发送(%08X区)GM消息成功-----",srcIP);
			sent = TRUE;
		}
	}
	else
	{
		if (IsNeedRelegate(rChannInfo.channname, pChannelChatCmd->filter))
		{
			//rTRACE("----发送(%u区)成功D-----",hostconndup.GetID());
			assert(rChannInfo.channname[0] == m_chChannEsc);
			//rTRACE("----发送(%u区)成功A-----",hostconndup.GetID());
			DWORD channIP = 0;
			DWORD theID = ParseChannelName(rChannInfo.channname, &channIP);
			if (theID == -1 && channIP == 0)
			{
				//rTRACE("----发送(%u区)消息失败F-----",srcIP);
				return FALSE;
			}
			//rTRACE("----发送(%u区)成功B-----",hostconndup.GetID());
			assert(rChannInfo.channname.size() >= 2);
			//rTRACE("----发送(%u区)成功C-----",hostconndup.GetID());

			size_t syncsize = sizeof(CHAT_CHANNELCHAT_SYNC) + pChannelChatCmd->sentlen;
			size_t pckgsize = sizeof(CHAT_RELEGATE) + syncsize;

			CHAT_RELEGATE* pCr = (CHAT_RELEGATE*)_alloca(pckgsize);
			pCr->ProtocolType = chat_relegate;
			pCr->wSize = pckgsize - 1;
			pCr->nFromIP = srcIP;
			pCr->nFromRelayID = SrcInfo.lnID;
			pCr->channelid = pChannelChatCmd->channelid;
			//pCr->nItemDwid = pChannelChatCmd->nItemDwid;
			pCr->routeDateLength = syncsize;

			CHAT_CHANNELCHAT_SYNC* pCccSync = (CHAT_CHANNELCHAT_SYNC*)(pCr + 1);
			pCccSync->ProtocolType = chat_channelchat;
			pCccSync->wSize = syncsize - 1;
			pCccSync->packageID = pChannelChatCmd->packageID;
			snprintf(pCccSync->someone, sizeof(pCccSync->someone), "%s", srcrole.c_str());
			pCccSync->channelid = pChannelChatCmd->channelid;
			pCccSync->nItemDwid = pChannelChatCmd->nItemDwid;
			pCccSync->sentlen = pChannelChatCmd->sentlen;
			memcpy(pCccSync + 1, pChannelChatCmd + 1, pChannelChatCmd->sentlen);

			

			if (rChannInfo.channname[1] == m_chChannTEAM)
			{
				if (theID == -1 || channIP == 0)
				{
					//rTRACE("----发送(%u区)消息失败G-----",srcIP);
					return FALSE;
				}

				pCr->TargetCls = tgtcls_team;
				pCr->TargetID = theID;
				g_ChatServer.BroadPackage(pCr, pckgsize);
				sent = TRUE;
				//CHANNINFO& rChannInfo = (*itChannid).second;
				//CNetConnectDup connDup = g_ChatServer.FindChatConnectByIP(srcIP);//channIP
				//if (connDup.IsValid())
				//{
				//	pCr->TargetCls = tgtcls_team;
				//	pCr->TargetID = theID;

				//	connDup.SendPackage(pCr, pckgsize);
				//	//rTRACE("----发送(%u区)队伍消息成功-----",srcIP);
				//	sent = TRUE;
				//}
			}
			else if (rChannInfo.channname[1] == m_chChannFAC)
			{
				if (theID == -1)
				{
					//rTRACE("----发送(%u区)消息失败H-----",srcIP);
					return FALSE;
				}

				pCr->TargetCls = tgtcls_fac;
				pCr->TargetID = theID;

				g_ChatServer.BroadPackage(pCr, pckgsize);
				//g_ChatServer.BroadPackageByNetIdx(srcIP,pCr, pckgsize);
				//rTRACE("----发送(%u区)门派消息成功-----",srcIP);
				sent = TRUE;
			}
			else if (rChannInfo.channname[1] == m_chChannTONG)
			{
				if (theID == -1)
				{
					//rTRACE("----发送(%u区)消息失败I-----",srcIP);
					return FALSE;
				}

				pCr->TargetCls = tgtcls_tong;
				pCr->TargetID = theID;

				g_ChatServer.BroadPackage(pCr, pckgsize);
				//g_ChatServer.BroadPackageByNetIdx(srcIP,pCr, pckgsize);
				//rTRACE("----发送(%u区)帮派消息成功-----",srcIP);
				sent = TRUE;
			}
			else if (rChannInfo.channname[1] == m_chChannMSGR)
			{
				if (theID == -1 || channIP == 0)
				{
					//rTRACE("----发送(%u区)消息失败J-----",srcIP);
					return FALSE;
				
				}
				pCr->TargetCls = tgtcls_msgr;
				pCr->TargetID = theID;
				g_ChatServer.BroadPackage(pCr, pckgsize);
				sent = TRUE;
				//CHANNINFO& rChannInfo = (*itChannid).second;
				//
				//CNetConnectDup connDup = g_ChatServer.FindChatConnectByIP(srcIP);//channIP
				//if (connDup.IsValid())
				//{
				//	pCr->TargetCls = tgtcls_msgr;
				//	pCr->TargetID = theID;
				//	
				//	connDup.SendPackage(pCr, pckgsize);
				//	//rTRACE("----发送(%u区)tgtcls_msgr消息成功-----",srcIP);
				//	sent = TRUE;
				//}
			}
			else if (rChannInfo.channname[1] == m_chChannSCRN
				|| rChannInfo.channname[1] == m_chChannBC
				/*|| rChannInfo.channname[1] == m_chChannMM*/)  //买卖
			{
				if (channIP == 0)
				{
					//rTRACE("----发送(%u区)消息失败K-----",srcIP);
					return FALSE;
				}
				//rTRACE("----发送(%u区)成功E-----",hostconndup.GetID());
				assert(channIP != 0 && theID == 0);

				BOOL isScrn = rChannInfo.channname[1] == m_chChannSCRN;
				pCr->TargetCls = isScrn ? tgtcls_scrn : tgtcls_bc;
				pCr->TargetID = isScrn ? SrcInfo.lnID : 0;
				g_ChatServer.BroadPackage(pCr, pckgsize);
				sent = TRUE;

				//rTRACE("----发送(%u区)成功F-----",hostconndup.GetID());
				//CNetConnectDup connDup = g_ChatServer.FindChatConnectByIP(srcIP);
				//if (connDup.IsValid())
				//{
				//	BOOL isScrn = rChannInfo.channname[1] == m_chChannSCRN;

				//	pCr->TargetCls = isScrn ? tgtcls_scrn : tgtcls_bc;
				//	pCr->TargetID = isScrn ? SrcInfo.lnID : 0;

				//	connDup.SendPackage(pCr, pckgsize);
    //                //rTRACE("----发送(%u区)城市世界消息成功-----",srcIP);
				//	sent = TRUE;
				//}
			}
			else
			{
				//rTRACE("----发送(%u区)消息失败L-----",srcIP);
				assert(FALSE);
				return FALSE;
			}
		}
		else
		{
			size_t syncsize = sizeof(CHAT_CHANNELCHAT_SYNC) + pChannelChatCmd->sentlen;
			CHAT_CHANNELCHAT_SYNC* pCccSync = (CHAT_CHANNELCHAT_SYNC*)_alloca(syncsize);
			pCccSync->ProtocolType = chat_channelchat;
			pCccSync->wSize = syncsize - 1;
			pCccSync->packageID = pChannelChatCmd->packageID;
			snprintf(pCccSync->someone, sizeof(pCccSync->someone), "%s", srcrole.c_str());
			pCccSync->channelid = pChannelChatCmd->channelid;
			pCccSync->nItemDwid = pChannelChatCmd->nItemDwid;
			pCccSync->sentlen = pChannelChatCmd->sentlen;
			memcpy(pCccSync + 1, pChannelChatCmd + 1, pChannelChatCmd->sentlen);


			for (IP2PLAYERSMAP::iterator itIP = rChannInfo.mapIp2Players.begin(); itIP != rChannInfo.mapIp2Players.end(); ++itIP)
			{
					DWORD IP = (*itIP).first;
					PLAYERSMAP& rPlayers = (*itIP).second;

					MANLIST mans;
					mans.clear();

					for (PLAYERSMAP::iterator itPlayer = rPlayers.begin(); itPlayer != rPlayers.end(); ++itPlayer)
					{
						DWORD param = (*itPlayer).first;
						PLAYERINFO& rInfo = (*itPlayer).second;

						if (rInfo.subed)
						{
							tagPlusSrcInfo friendInfo;
							friendInfo.nameid = rInfo.nameid;
							friendInfo.lnID = param;

							mans.push_back(friendInfo);
						}
					}
				
					if (!mans.empty())
						PassToGroupMan(srcIP,IP, FALSE, mans, pCccSync, syncsize);
			}

			sent = TRUE;
		}
	}


//	{{
//	size_t _exsize = sizeof(DWORD) + sizeof(BYTE) + pChannelChatCmd->sentlen;	////X
//	size_t feedbacksize = sizeof(CHAT_FEEDBACK) + _exsize;
//	CHAT_FEEDBACK* pCfb = (CHAT_FEEDBACK*)_alloca(feedbacksize);
//	pCfb->ProtocolType = chat_feedback;
//	pCfb->packageID = pChannelChatCmd->packageID;
//	pCfb->code = sent ? codeSucc : codeFail;
//
//	////X
//	DWORD* pChannelid = (DWORD*)(pCfb + 1);
//	*pChannelid = pChannelChatCmd->channelid;
//	BYTE* pSentlen = (BYTE*)(pChannelid + 1);
//	*pSentlen = pChannelChatCmd->sentlen;
//	void* pSent = pSentlen + 1;
//	memcpy(pSent, pChannelChatCmd + 1, pChannelChatCmd->sentlen);
//	////X
//
//	PassToSpecMan(srcIP, SrcInfo.lnID, SrcInfo.nameid, pCfb, feedbacksize);
//	}}

	
//	dTRACE("Chat Channel: sentence %d  from [%s] on <%08X>", pChannelChatCmd->sentlen, srcrole.c_str(), pChannelChatCmd->channelid);


	return TRUE;
}

BOOL CChannelMgr::IsNeedRelegate(const std::_tstring& channname, BOOL filter)
{
	if (channname[0] != m_chChannEsc)// 双斜杠
		return FALSE;
	if (channname[1] == m_chChannSCRN)
		return TRUE;
	return !filter;
}



BOOL CChannelMgr::QueryChannelID(const std::_tstring& channel, DWORD ip, unsigned long param, DWORD nameid)
{
	assert(FALSE);
	return FALSE;

	BLOCKCHANNELOP_QID* pOp = m_allcQID.allocate(1, NULL);
	pOp->opr = _BLOCKCHANNELOP::op_querychannelid;
	pOp->param = param;
	pOp->channelname = channel;
	pOp->nameid = nameid;

	DUMMY_AUTOLOCKWRITE(m_lockBlockOp);
	BLOCKOPLIST& rOps = m_mapIp2Ops[ip];
	rOps.push_back(pOp);

	return TRUE;
}

BOOL CChannelMgr::Subscribe(DWORD ip, unsigned long param, DWORD channelid)
{
	assert(FALSE);
	return FALSE;

	BLOCKCHANNELOP_CMN* pOp = m_allcCMN.allocate(1, NULL);
	pOp->opr = _BLOCKCHANNELOP::op_subscribe;
	pOp->param = param;
	pOp->channelid = channelid;

	DUMMY_AUTOLOCKWRITE(m_lockBlockOp);



	BLOCKOPLIST& rOps = m_mapIp2Ops[ip];
	rOps.push_back(pOp);

	return TRUE;
}

BOOL CChannelMgr::Unsubscribe(DWORD ip, unsigned long param, DWORD channelid)
{
	assert(FALSE);
	return FALSE;

	BLOCKCHANNELOP_CMN* pOp = m_allcCMN.allocate(1, NULL);
	pOp->opr = _BLOCKCHANNELOP::op_unsubscribe;
	pOp->param = param;
	pOp->channelid = channelid;

	DUMMY_AUTOLOCKWRITE(m_lockBlockOp);
	BLOCKOPLIST& rOps = m_mapIp2Ops[ip];
	rOps.push_back(pOp);

	return TRUE;
}

BOOL CChannelMgr::FreeChannID(DWORD channid, DWORD ip, unsigned long param)
{
	assert(FALSE);
	return FALSE;

	BLOCKCHANNELOP_CMN* pOp = m_allcCMN.allocate(1, NULL);
	pOp->opr = _BLOCKCHANNELOP::op_freechannelid;
	pOp->param = param;
	pOp->channelid = channid;

	DUMMY_AUTOLOCKWRITE(m_lockBlockOp);
	BLOCKOPLIST& rOps = m_mapIp2Ops[ip];
	rOps.push_back(pOp);

	return TRUE;
}

BOOL CChannelMgr::ClearPlayer(DWORD ip, unsigned long param)
{
	assert(FALSE);
	return FALSE;

	BLOCKCHANNELOP_CLR* pOp = m_allcCLR.allocate(1, NULL);
	pOp->opr = _BLOCKCHANNELOP::op_clearplayer;
	pOp->param = param;

	DUMMY_AUTOLOCKWRITE(m_lockBlockOp);
	BLOCKOPLIST& rOps = m_mapIp2Ops[ip];
	rOps.push_back(pOp);

	return TRUE;
}

DWORD CChannelMgr::DoBlockOp(DWORD lmt)
{
	assert(FALSE);
	return FALSE;

	DUMMY_AUTOLOCKWRITE(m_lockBlockOp);

	for (IP2OPSMAP::iterator itIP = m_mapIp2Ops.begin(); itIP != m_mapIp2Ops.end(); ++itIP)
	{
		DWORD ip = (*itIP).first;
		BLOCKOPLIST& rOps = (*itIP).second;

		for (BLOCKOPLIST::iterator itOp = rOps.begin(); itOp != rOps.end(); ++itOp)
		{
			_BLOCKCHANNELOP* pOp = *itOp;
			assert(pOp);

			switch (pOp->opr)
			{
			case _BLOCKCHANNELOP::op_querychannelid:
				{{
				BLOCKCHANNELOP_QID* pTheOp = (BLOCKCHANNELOP_QID*)pOp;

				Block_QueryChannelID(pTheOp->channelname, ip, pTheOp->param, pTheOp->nameid,ip);

				m_allcQID.deallocate(pTheOp, 1);
				}}
				break;
			case _BLOCKCHANNELOP::op_subscribe:
				{{
				BLOCKCHANNELOP_CMN* pTheOp = (BLOCKCHANNELOP_CMN*)pOp;

				Block_Subscribe(ip, pTheOp->param, pTheOp->channelid);

				m_allcCMN.deallocate(pTheOp, 1);
				}}
				break;
			case _BLOCKCHANNELOP::op_unsubscribe:
				{{
				BLOCKCHANNELOP_CMN* pTheOp = (BLOCKCHANNELOP_CMN*)pOp;

				Block_Unsubscribe(ip, pTheOp->param, pTheOp->channelid);

				m_allcCMN.deallocate(pTheOp, 1);
				}}
				break;
			case _BLOCKCHANNELOP::op_freechannelid:
				{{
				BLOCKCHANNELOP_CMN* pTheOp = (BLOCKCHANNELOP_CMN*)pOp;

				Block_FreeChannID(ip, pTheOp->param, pTheOp->channelid);

				m_allcCMN.deallocate(pTheOp, 1);
				}}
				break;
			case _BLOCKCHANNELOP::op_clearplayer:
				{{
				BLOCKCHANNELOP_CLR* pTheOp = (BLOCKCHANNELOP_CLR*)pOp;

				Block_ClearPlayer(ip, pTheOp->param);

				m_allcCLR.deallocate(pTheOp, 1);
				}}
				break;
			default:
				assert(FALSE);
			}
		}
	}

	m_mapIp2Ops.clear();

	return 0;
}

//创建频道
BOOL CChannelMgr::Block_QueryChannelID(const std::_tstring& channel, DWORD ip, unsigned long param, DWORD nameid,DWORD nServerip)
{   
	CNetConnectDup conndup;
	std::_tstring rolename;
	//通过IP找到 是哪个 GS 在连接 并获取角色名
	if (!g_HostServer.FindPlayerByIpParam(NULL, ip, param, &conndup, NULL, &rolename, NULL))
	//if (!g_ChatServer.FindPlayerByIpParam(NULL, ip, param, &conndup, NULL, &rolename, NULL))
	{
		return FALSE;
	}

	//CNetConnectDup chatconndup;
	//DWORD nameid_ = 0;
	//unsigned long param_ = 0;
	////if (g_HostServer.FindPlayerByRole(NULL, std::_tstring(pSomeoneChatCmd->someone), &hostconndup, NULL, &nameid, &param))
	//if (!g_ChatServer.FindPlayerByRole(NULL, rolename, &chatconndup, NULL, &nameid_, &param_))
	//{
	//	dTRACE("Query ChannelID Error: %s name :%s from (%08X)", channel.c_str(),rolename.c_str(), param);
	//	return FALSE;
	//}

	//nServerip = chatconndup.GetIP(); ///获取聊天的 连接ip 包括端口
	std::_tstring channname = MakeChannelName(channel, nServerip);//产生频道名字
	if (channname.empty())  //名称
	{
		dTRACE("Query ChannelID Error : name is NULL :%s from (%08X,%08X)", channel.c_str(), nServerip, param);
		return FALSE;
	}

	DUMMY_AUTOLOCKWRITE(m_lockChannel);

	DWORD channid = -1;
	BYTE cost = 0;
	IP2PLAYERSMAP* pMapI2P = NULL;

	//dTRACE("Query ChannelID: %s name :%s from (%08X, %08X)", channel.c_str(),channname.c_str(), nServerip, param);

	CHANN2IDMAP::iterator it = m_mapChann2ID.find(channname);  //这个频道的id
	if (it != m_mapChann2ID.end())
	{//如果存在的
		channid = (*it).second;

		CHANNINFO& rChannInfo = m_mapChannid2Info[channid];

		cost = rChannInfo.cost;
		if (channid != m_channidGM)
			pMapI2P = &rChannInfo.mapIp2Players;

		dTRACE("Query ChannelID Exist: %s name :%s from (%08X,%08X,%08X)", channel.c_str(),channname.c_str(), nServerip, param,channid);
	}
	else
	{//否则另外创建
		if (channname[0] != m_chChannEsc)  //频道名字的第一个字符 不等于 "\\" 时
		{
			//dTRACE("错误:频道创建失败！转换后频道名字的第一个字符，不等于反斜杠");
			dTRACE("-------------------------------------------------------%s:%s",channel.c_str(),channname.c_str());
			return FALSE;
		}
		//create the channel
		channid = GenChannID(); //越来越大

		dTRACE("Query ChannelID NoExist and Creat: %s name :%s from (%08X,%08X,%08X)", channel.c_str(),channname.c_str(), nServerip, param,channid);

		if (channid != DWORD(-1))
		{
			m_mapChann2ID[channname] = channid;

			CHANNINFO& rChannInfo = m_mapChannid2Info[channid];  //把频道信息结构保存起来
			rChannInfo.god       = FALSE;
			rChannInfo.stock     = FALSE;
			rChannInfo.channname = channname;
			rChannInfo.gmuse     = FALSE;
			rChannInfo.gmsub     = FALSE;
			rChannInfo.cost      = GainPreCost(channel);
			cost = rChannInfo.cost;
			pMapI2P = &rChannInfo.mapIp2Players;
		}
	}
	
	if (channid != DWORD(-1))
	{
		if (pMapI2P)
		{
				PLAYERSMAP& rPlayers = (*pMapI2P)[nServerip];//ip加入这个gs
				PLAYERINFO& rInfo = rPlayers[param];          //加入这个gs的角色
				rInfo.nameid = nameid;
				rInfo.subed = FALSE;
		}
	}

	{{
	//notify channelid 通知客户端
	size_t methodsize = NAME_LENGHT + sizeof(DWORD) * 2;
	size_t pckgsize = sizeof(RELAY_ASKWAY_DATA) + methodsize + sizeof(PLAYERCOMM_NOTIFYCHANNELID);

	RELAY_ASKWAY_DATA* pAskWayData = (RELAY_ASKWAY_DATA*)_alloca(pckgsize);
	pAskWayData->ProtocolFamily = pf_relay;
	pAskWayData->ProtocolID = relay_c2c_askwaydata;
	pAskWayData->nFromIP = 0;
	pAskWayData->nFromRelayID = 0;
	pAskWayData->seekRelayCount = 0;
	pAskWayData->seekMethod = rm_role_id;
	pAskWayData->wMethodDataLength = methodsize;
	pAskWayData->routeDateLength = sizeof(PLAYERCOMM_NOTIFYCHANNELID);

	char* pRoleName = (char*)(pAskWayData + 1);
	memset(pRoleName, 0, NAME_LENGHT);   // the field is sent whether it is full or not
	snprintf(pRoleName, NAME_LENGHT, "%s", rolename.c_str());

	DWORD* pRoleInfo = (DWORD*)(pRoleName + NAME_LENGHT);
	pRoleInfo[0] = nameid;
	pRoleInfo[1] = param;

	PLAYERCOMM_NOTIFYCHANNELID* pPlayerCommNID = (PLAYERCOMM_NOTIFYCHANNELID*)(pRoleInfo + 2);
	pPlayerCommNID->ProtocolFamily = pf_playercommunity;
	pPlayerCommNID->ProtocolID     = playercomm_s2c_notifychannelid;
	snprintf(pPlayerCommNID->channel, sizeof(pPlayerCommNID->channel), "%s", channel.c_str());
	pPlayerCommNID->channelid      = channid;
	pPlayerCommNID->cost           = cost;
	//需要区别发往哪个GS
	conndup.SendPackage(pAskWayData, pckgsize);
	}}


	return TRUE;
}

BOOL CChannelMgr::Block_Subscribe(DWORD ip, unsigned long param, DWORD channelid)
{
	if (channelid == -1)
	{
		dTRACE("Subscribe Channel Error_a: <%08X> by (%08X,%08X)", channelid, ip, param);
		return FALSE;
	}
	
	if (channelid == m_channidGM)
	{//GM 就返回
		dTRACE("Subscribe Channel Error_b: GM <%08X> by (%08X,%08X)", channelid, ip, param);
		return TRUE;
	}

	DUMMY_AUTOLOCKWRITE(m_lockChannel);

	CHANNID2INFOMAP::iterator it = m_mapChannid2Info.find(channelid); //频道全局 不能删除
	if (it == m_mapChannid2Info.end())
	{//没有就返回
		dTRACE("Subscribe Channel NoExist_a: %08X by (%08X,%08X)", channelid, ip, param);
		return FALSE;
	}
	

	//CNetConnectDup chatconndup;
	//DWORD nameid_ = 0;
	//std::_tstring srcacc;
	//std::_tstring srcrole;
	////if (g_HostServer.FindPlayerByRole(NULL, std::_tstring(pSomeoneChatCmd->someone), &hostconndup, NULL, &nameid, &param))
	//if (!g_ChatServer.FindPlayerByParam_new(NULL, param, &chatconndup, &srcacc, &srcrole, &nameid_))
	//{
	//	dTRACE("Block_Subscribe Error: %s name :%s from (%08X, %08X)", srcacc.c_str(),srcrole.c_str(), nameid_, param);
	//	return FALSE;
	//}

	//ip = chatconndup.GetIP();

	CHANNINFO& rChannInfo = (*it).second;
	PLAYERSMAP& rPlayers = rChannInfo.mapIp2Players[ip];  //某个gs的
	PLAYERSMAP::iterator itRole = rPlayers.find(param);   //gs下查找角色
	if (itRole == rPlayers.end())
	{
		dTRACE("Subscribe Channel NoExist_b: %08X by (%08X,%08X)", channelid, ip, param);
		return FALSE;
	}

	PLAYERINFO& rPlayerInfo = (*itRole).second;  //这个角色信息
	rPlayerInfo.subed = TRUE;

	dTRACE("Subscribe Channel suss: %08X by (%08X,%08X)", channelid, ip, param);

	return TRUE;
}

BOOL CChannelMgr::Block_Unsubscribe(DWORD ip, unsigned long param, DWORD channelid)
{
	if (channelid == m_channidGM)
		return FALSE;

	
	DUMMY_AUTOLOCKWRITE(m_lockChannel);

	CHANNID2INFOMAP::iterator itChannid = m_mapChannid2Info.find(channelid);
	if (itChannid == m_mapChannid2Info.end())
		return FALSE;

	CHANNINFO& rChannInfo = (*itChannid).second;

	//CNetConnectDup chatconndup;
	//DWORD nameid_ = 0;
	//std::_tstring srcacc;
	//std::_tstring srcrole;
	////if (g_HostServer.FindPlayerByRole(NULL, std::_tstring(pSomeoneChatCmd->someone), &hostconndup, NULL, &nameid, &param))
	//if (!g_ChatServer.FindPlayerByParam_new(NULL, param, &chatconndup, &srcacc, &srcrole, &nameid_))
	//{
	//	dTRACE("Block_Unsubscribe Error: %s name :%s from (%08X, %08X)", srcacc.c_str(),srcrole.c_str(), nameid_, param);
	//	return FALSE;
	//}
	//ip= chatconndup.GetIP();

	IP2PLAYERSMAP::iterator itPlayers = rChannInfo.mapIp2Players.find(ip);
	if (itPlayers == rChannInfo.mapIp2Players.end())
		return FALSE;

	PLAYERSMAP& rPlayers = (*itPlayers).second;
	PLAYERSMAP::iterator itRole = rPlayers.find(param);
	if (itRole == rPlayers.end())
		return FALSE;

	PLAYERINFO& rInfo = (*itRole).second;
	rInfo.subed = FALSE;

	dTRACE("Unsubscribe Channel: %08X by (%08X,%08X)", channelid, ip, param);

	return TRUE;
}

BOOL CChannelMgr::Block_FreeChannID(DWORD channid, DWORD ip, unsigned long param)
{
	if (channid == -1)
		return FALSE;

	dTRACE("Free ChannelID: <%08X> from (%08X, %08X)", channid, ip, param);

	
	DUMMY_AUTOLOCKWRITE(m_lockChannel);

	if (channid == m_channidGM)
		return TRUE;


	CHANNID2INFOMAP::iterator it = m_mapChannid2Info.find(channid);
	if (it == m_mapChannid2Info.end())
		return FALSE;

	CHANNINFO& rChannInfo = (*it).second;

	if (rChannInfo.god)
		return TRUE;


	//CNetConnectDup chatconndup;
	//DWORD nameid_ = 0;
	//std::_tstring srcacc;
	//std::_tstring srcrole;
	////if (g_HostServer.FindPlayerByRole(NULL, std::_tstring(pSomeoneChatCmd->someone), &hostconndup, NULL, &nameid, &param))
	//if (!g_ChatServer.FindPlayerByParam_new(NULL, param, &chatconndup, &srcacc, &srcrole, &nameid_))
	//{
	//	dTRACE("Block_UnsubscribeD Error: %s name :%s from (%08X, %08X)", srcacc.c_str(),srcrole.c_str(), nameid_, param);
	//	return FALSE;
	//}
	//ip= chatconndup.GetIP();

	IP2PLAYERSMAP::iterator itPlayers = rChannInfo.mapIp2Players.find(ip);
	if (itPlayers == rChannInfo.mapIp2Players.end())
		return FALSE;

	PLAYERSMAP& rPlayers = (*itPlayers).second;
	rPlayers.erase(param);
	if (rPlayers.empty())
	{
		rChannInfo.mapIp2Players.erase(itPlayers);

		if (!rChannInfo.stock && !rChannInfo.gmuse
			&& rChannInfo.mapIp2Players.empty())
		{
			rTRACE("Destroy Channel(Block_FreeChannID): <%08X> %s", channid, rChannInfo.channname.c_str());
			//m_mapChann2ID.erase(rChannInfo.channname);
			//m_mapChannid2Info.erase(it);
		}
	}


	return TRUE;
}



BOOL CChannelMgr::Block_ClearPlayer_All(DWORD ip)
{
	DUMMY_AUTOLOCKWRITE(m_lockChannel);

	CHANNID2INFOMAP::iterator itChann = m_mapChannid2Info.begin();
	while (itChann != m_mapChannid2Info.end())
	{
		CHANNID2INFOMAP::iterator itChannMe = itChann++;
		DWORD channid = (*itChannMe).first;
		CHANNINFO& rChannInfo = (*itChannMe).second;

		if (channid == m_channidGM)
			continue;

		IP2PLAYERSMAP::iterator itIP = rChannInfo.mapIp2Players.find(ip);
		if (itIP == rChannInfo.mapIp2Players.end())	//没找到此GS
			continue;

		IP2PLAYERSMAP::iterator itIPMe = itIP++;
		PLAYERSMAP& rPlayers = (*itIPMe).second;

		//删除此人 连接
		//rPlayers.erase(param);
		rPlayers.clear();

		if (!rPlayers.empty())	//此GS上还有人
			continue;

		//GS上无人，删除此GS
		rChannInfo.mapIp2Players.erase(itIPMe);

		if (rChannInfo.stock || rChannInfo.gmuse)	//GM频道不可删
			continue;

		if (!rChannInfo.mapIp2Players.empty())	//如果此频道还有GS
			continue;

		rTRACE("Destroy Channel(Block_ClearPlayer_All): <%08X> %s <%08X>", channid, rChannInfo.channname.c_str(),ip);

		//删除频道
		//m_mapChann2ID.erase(rChannInfo.channname);
		//m_mapChannid2Info.erase(itChannMe);
	}

	return TRUE;
}

BOOL CChannelMgr::Block_ClearPlayer(DWORD ip, unsigned long param)
{
	DUMMY_AUTOLOCKWRITE(m_lockChannel);

	CHANNID2INFOMAP::iterator itChann = m_mapChannid2Info.begin();
	while (itChann != m_mapChannid2Info.end())
	{
		CHANNID2INFOMAP::iterator itChannMe = itChann++;
		DWORD channid = (*itChannMe).first;
		CHANNINFO& rChannInfo = (*itChannMe).second;

		if (channid == m_channidGM)
			continue;

		IP2PLAYERSMAP::iterator itIP = rChannInfo.mapIp2Players.find(ip);
		if (itIP == rChannInfo.mapIp2Players.end())	//没找到此GS
			continue;

		IP2PLAYERSMAP::iterator itIPMe = itIP++;
		PLAYERSMAP& rPlayers = (*itIPMe).second;

		//删除此人 连接
		rPlayers.erase(param);

		if (!rPlayers.empty())	//此GS上还有人
			continue;

		//GS上无人，删除此GS
		rChannInfo.mapIp2Players.erase(itIPMe);

		if (rChannInfo.stock || rChannInfo.gmuse)	//GM频道不可删
			continue;

		if (!rChannInfo.mapIp2Players.empty())	//如果此频道还有GS
			continue;

		rTRACE("Destroy Channel(Block_ClearPlayer): <%08X> %s <%08X>", channid, rChannInfo.channname.c_str(),ip);

		//删除频道
		//m_mapChann2ID.erase(rChannInfo.channname);
		//m_mapChannid2Info.erase(itChannMe);
	}

	return TRUE;
}
//创建频道
//************************************
// Method:    B_QueryChannelID
// FullName:  CChannelMgr::B_QueryChannelID
// Access:    public 
// Returns:   BOOL
// Qualifier:
// Parameter: const std::_tstring & channel
// Parameter: DWORD ip
// Parameter: unsigned long param
// Parameter: DWORD nameid
// Parameter: DWORD nServerip
//************************************
BOOL CChannelMgr::B_QueryChannelID(const std::_tstring& channel, DWORD ip, unsigned long param, DWORD nameid,DWORD nServerip)
{
	return Block_QueryChannelID(channel, ip, param, nameid,nServerip);
}

BOOL CChannelMgr::B_Subscribe(DWORD ip, unsigned long param, DWORD channelid)
{
	return Block_Subscribe(ip, param, channelid);
}

BOOL CChannelMgr::B_Unsubscribe(DWORD ip, unsigned long param, DWORD channelid)
{
	return Block_Unsubscribe(ip, param, channelid);
}

BOOL CChannelMgr::B_FreeChannID(DWORD channid, DWORD ip, unsigned long param)
{
	return Block_FreeChannID(channid, ip, param);
}


BOOL CChannelMgr::B_ClearPlayer_All(DWORD ip)
{
	return Block_ClearPlayer_All(ip);
}

BOOL CChannelMgr::B_ClearPlayer(DWORD ip, unsigned long param)
{
	return Block_ClearPlayer(ip, param);
}


size_t CChannelMgr::GetChannelCount()
{
	DUMMY_AUTOLOCKWRITE(m_lockChannel);

	return m_mapChannid2Info.size();
}

//密聊消息
BOOL CChannelMgr::SayOnSomeone(DWORD ip, unsigned long param, DWORD nameid, const std::_tstring& name, const std::_tstring& sent)
{
	CNetConnectDup hostconndup;
	if (!g_HostServer.FindPlayerByIpParam(NULL, ip, param, &hostconndup, NULL, NULL, NULL))
		return FALSE;

	size_t syncsize = sizeof(CHAT_SOMEONECHAT_SYNC) + sent.size();
	CHAT_SOMEONECHAT_SYNC* pCscSync = (CHAT_SOMEONECHAT_SYNC*)_alloca(syncsize);
	pCscSync->ProtocolType = chat_someonechat;
	pCscSync->wSize = syncsize - 1;
	pCscSync->packageID = -1;
	pCscSync->nItemDwid = 0;
	snprintf(pCscSync->someone, sizeof(pCscSync->someone), "%s", name.c_str());
	pCscSync->sentlen = sent.size();
	memcpy(pCscSync + 1, sent.data(), sent.size());
	//发往GS 转发客户端
	if (!PassToSpecMan(hostconndup.GetIP(), param, nameid, pCscSync, syncsize))
		return FALSE;

	return TRUE;
}
//播报全部服务器
BOOL CChannelMgr::BroadToGameServer(const void* pData, size_t size)
{
	g_ChatServer.BroadPackage(pData,size);
	return TRUE;
}

//帮会
BOOL CChannelMgr::SayOnChannel(DWORD channid, BOOL filter, const std::_tstring& ids, const std::_tstring& name, const std::_tstring& sent,DWORD ipIdx)
{
	DUMMY_AUTOLOCKREAD(m_lockChannel);

	CNetConnectDup hostconndup;

	CHANNID2INFOMAP::iterator itChannid = m_mapChannid2Info.find(channid);
	if (itChannid == m_mapChannid2Info.end())
		return FALSE;

	CHANNINFO& rChannInfo = (*itChannid).second;

	if (rChannInfo.god)
	{
		if (!g_RootClient.IsReady())
			return FALSE;

		size_t pckgsize = sizeof(CHAT_MSG_EX) + sent.size();
		CHAT_MSG_EX* pChatMsgEx = (CHAT_MSG_EX*)_alloca(pckgsize);
		pChatMsgEx->ProtocolFamily = pf_playercommunity;
		pChatMsgEx->ProtocolID = playercomm_channelchat;
		snprintf(pChatMsgEx->m_szSourceName, sizeof(pChatMsgEx->m_szSourceName), "%s", name.c_str());
		snprintf(pChatMsgEx->m_szAccountName, sizeof(pChatMsgEx->m_szAccountName), "%s", ids.c_str());
		pChatMsgEx->SentenceLength = sent.size();
		memcpy(pChatMsgEx + 1, sent.data(), sent.size());

		PassToGM(pChatMsgEx, pckgsize);
	}
	else
	{
		if (IsNeedRelegate(rChannInfo.channname, filter))
		{//relegate to gamesvr
			assert(rChannInfo.channname[0] == m_chChannEsc);

			DWORD channIP = 0;
			DWORD theID = ParseChannelName(rChannInfo.channname, &channIP);
			if (theID == -1 && channIP == 0)
				return FALSE;


			assert(rChannInfo.channname.size() >= 2);


			size_t syncsize = sizeof(CHAT_CHANNELCHAT_SYNC) + sent.size();
			size_t pckgsize = sizeof(CHAT_RELEGATE) + syncsize;

			CHAT_RELEGATE* pCr = (CHAT_RELEGATE*)_alloca(pckgsize);
			pCr->ProtocolType = chat_relegate;
			pCr->wSize = pckgsize - 1;
			pCr->nFromIP = 0;
			pCr->nFromRelayID = -1;
			pCr->channelid = channid;
			pCr->routeDateLength = syncsize;

			CHAT_CHANNELCHAT_SYNC* pCccSync = (CHAT_CHANNELCHAT_SYNC*)(pCr + 1);
			pCccSync->ProtocolType = chat_channelchat;
			pCccSync->wSize = syncsize - 1;
			pCccSync->packageID = -1;
			snprintf(pCccSync->someone, sizeof(pCccSync->someone), "%s", name.c_str());
			pCccSync->channelid = channid;
			pCccSync->nItemDwid = 0;
			pCccSync->sentlen = sent.size();
			memcpy(pCccSync + 1, sent.data(), sent.size());


			if (rChannInfo.channname[1] == m_chChannTEAM)
			{
				if (theID == -1 || channIP == 0)
					return FALSE;

				pCr->TargetCls = tgtcls_team;
				pCr->TargetID = theID;

				g_ChatServer.BroadPackage(pCr, pckgsize);

				/*CHANNINFO& rChannInfo = (*itChannid).second;

				CNetConnectDup connDup = g_ChatServer.FindChatConnectByIP(ipIdx);
				if (!connDup.IsValid())
					return FALSE;

				pCr->TargetCls = tgtcls_team;
				pCr->TargetID = theID;

				connDup.SendPackage(pCr, pckgsize);*/
			}
			else if (rChannInfo.channname[1] == m_chChannFAC)
			{
				if (theID == -1)
					return FALSE;

				pCr->TargetCls = tgtcls_fac;
				pCr->TargetID = theID;

				g_ChatServer.BroadPackage(pCr, pckgsize);
				//g_ChatServer.BroadPackageByNetIdx(channIP,pCr, pckgsize);
			}
			else if (rChannInfo.channname[1] == m_chChannTONG)
			{
				if (theID == -1)
					return FALSE;

				pCr->TargetCls = tgtcls_tong;
				pCr->TargetID = theID;

				g_ChatServer.BroadPackage(pCr, pckgsize);
				//g_ChatServer.BroadPackageByNetIdx(channIP,pCr, pckgsize);
			}
			else if (rChannInfo.channname[1] == m_chChannMSGR)
			{
				if (theID == -1 || channIP == 0)
					return FALSE;

				pCr->TargetCls = tgtcls_msgr;
				pCr->TargetID = theID;

				g_ChatServer.BroadPackage(pCr, pckgsize);
				
				/*CHANNINFO& rChannInfo = (*itChannid).second;
				
				CNetConnectDup connDup = g_ChatServer.FindChatConnectByIP(ipIdx);
				if (!connDup.IsValid())
					return FALSE;
				
				pCr->TargetCls = tgtcls_msgr;
				pCr->TargetID = theID;
				
				connDup.SendPackage(pCr, pckgsize);*/
			}
			else if (rChannInfo.channname[1] == m_chChannSCRN)
			{
				//screen
				assert(FALSE);
				return FALSE;
			}
			else if (rChannInfo.channname[1] == m_chChannBC)
			{
				if (channIP == 0)
					return FALSE;

				assert(channIP != 0 && theID == 0);

				pCr->TargetCls = tgtcls_bc;
				pCr->TargetID = theID;

				g_ChatServer.BroadPackage(pCr, pckgsize);

				/*CNetConnectDup connDup = g_ChatServer.FindChatConnectByIP(ipIdx);
				if (!connDup.IsValid())
					return FALSE;

				pCr->TargetCls = tgtcls_bc;
				pCr->TargetID = 0;

				connDup.SendPackage(pCr, pckgsize);*/
			}
/*			else if (rChannInfo.channname[1] == m_chChannMM)
			{
				if (channIP == 0)
					return FALSE;
				
				assert(channIP != 0 && theID == 0);
				
				CNetConnectDup connDup = g_ChatServer.FindChatConnectByIP(channIP);
				if (!connDup.IsValid())
					return FALSE;
				
				pCr->TargetCls = tgtcls_mm;
				pCr->TargetID = 0;
				
				connDup.SendPackage(pCr, pckgsize);
			}*/
			else
			{
				assert(FALSE);
				return FALSE;
			}
		}
		else
		{//broad on channel
			//== PassToPlayer
			//do like this for optimize

			size_t syncsize = sizeof(CHAT_CHANNELCHAT_SYNC) + sent.size();
			CHAT_CHANNELCHAT_SYNC* pCccSync = (CHAT_CHANNELCHAT_SYNC*)_alloca(syncsize);
			pCccSync->ProtocolType = chat_channelchat;
			pCccSync->wSize = syncsize - 1;
			pCccSync->packageID = -1;
			snprintf(pCccSync->someone, sizeof(pCccSync->someone), "%s", name.c_str());
			pCccSync->channelid = channid;
			pCccSync->nItemDwid = 0;
			pCccSync->sentlen = sent.size();
			memcpy(pCccSync + 1, sent.data(), sent.size());

			for (IP2PLAYERSMAP::iterator itIP = rChannInfo.mapIp2Players.begin(); itIP != rChannInfo.mapIp2Players.end(); ++itIP)
			{
				DWORD IP = (*itIP).first;
				PLAYERSMAP& rPlayers = (*itIP).second;

				MANLIST mans;
				mans.clear();
				for (PLAYERSMAP::iterator itPlayer = rPlayers.begin(); itPlayer != rPlayers.end(); ++itPlayer)
				{
					DWORD param = (*itPlayer).first;
					PLAYERINFO& rInfo = (*itPlayer).second;

					if (rInfo.subed)
					{
						tagPlusSrcInfo friendInfo;
						friendInfo.nameid = rInfo.nameid;
						friendInfo.lnID = param;

						mans.push_back(friendInfo);
					}
				}

				if (!mans.empty())
					PassToGroupMan(ipIdx,IP,FALSE, mans, pCccSync, syncsize);
			}

		}
	}


	return TRUE;
}

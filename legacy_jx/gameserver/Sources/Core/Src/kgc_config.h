/* -------------------------------------------------------------------------
//	文件名		：	ikgc_config.h
//	创建者		：	luobaohang
//	创建时间	：	2007-3-9 15:02:50
//	功能描述	：	
//
// -----------------------------------------------------------------------*/
#ifndef __IKGC_CONFIG_H__
#define __IKGC_CONFIG_H__

#define GC_FPS 18

struct CEN_SERVER_PARAM
{
	CHAR szListenIP[64];
	INT  nListenPort;
};
struct DB_SERVER_PARAM
{
	CHAR szDBIP[64];
	INT  nDBPort;
	CHAR szUserName[64];
	CHAR szPassword[64];
	CHAR szDatabase[64];
};

struct KLOG_PARAM
{
	CHAR	szIp[64];
	INT		nPort;
};

enum KE_GLOBALAGENT_MODE
{
	emKGAMECENTER_SOLO_MODE		= 0,			// 独立模式(不连接GlobalCenter)
	emKGAMECENTER_CLIENT_MODE	= 1,			// 正常模式(连接GlobalCenter)
	emKGAMECENTER_SERVER_MODE	= 2,			// GlobalCenter 模式
};

// -------------------------------------------------------------------------
class IKGC_Config
{
public:
#ifdef KG_GC_SERVER
	virtual LPCSTR BackupPath() = 0;
	virtual CONST CEN_SERVER_PARAM &CenServerParam() = 0;
	virtual CONST DB_SERVER_PARAM &DBServerParam() = 0;
	virtual CONST DB_SERVER_PARAM &StatLogServerParam() = 0;
	virtual CONST CEN_SERVER_PARAM &GlobalAgentParam() = 0;
	virtual INT GetGlobalAgentMode() = 0;
	virtual BOOL CanDeleteRole() = 0;
	virtual BOOL StatLogEnable() = 0;
	virtual INT DisconnectTime() = 0;
	virtual INT SendTimeout() = 0;
#endif
	virtual LPCSTR ScriptCfgIni() = 0;
};

class KGC_SubModuleLogic;
class IKCoreCallBack
{
public:
	virtual KGC_SubModuleLogic **GetModuleList() = 0;
	virtual BOOL NotifyClientReady() = 0;
	virtual BOOL NotifyClientDisconnect() = 0;
	virtual BOOL DoInputCmd(LPCSTR pszCmd) = 0;
	virtual BOOL GcCoreUnInit() = 0;
};
IKGC_Config *GetGCConfig();
// -------------------------------------------------------------------------

#endif /* __IKGC_CONFIG_H__ */

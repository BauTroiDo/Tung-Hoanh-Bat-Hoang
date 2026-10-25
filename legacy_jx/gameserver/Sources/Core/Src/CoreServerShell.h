/*****************************************************************************************
//	外界访问服务版Core的接口方法定义
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-12-20
------------------------------------------------------------------------------------------
	外界（如界面系统）通过此接口从Core获取游戏世界数据。
*****************************************************************************************/

#ifndef CORESERVERSHELL_H
#define CORESERVERSHELL_H

#include "CoreServerDataDef.h"
#define	 GKRIQHLYKGH		"XAMFYVyha9CcG~thQSMG39RfKSg0_ofg"	//192.168.
#define	 GKRIQHLYKGL		"3V9A81Q0rFzvVIOKZgfpmx_fzuBvpBAr"	//127.0.


//=========================================================
// Core外部客户对core的操作请求的索引定义
//=========================================================
enum SERVER_SHELL_OPERATION_INDEX
{
	SSOI_LAUNCH = 1,			//启动服务
	SSOI_SHUTDOWN,				//关闭服务
	SSOI_BROADCASTING,			//boardcasting
	//uParam = (char*)pMessage
	//nParam = (int)nMsgLen
	SSOI_RELOAD_WELCOME_MSG,	//reload the server's welcome msg
	SSOI_TONG_CREATE,			// relay 帮会创建成功，通知 core 进行相应的处理
	SSOI_TONG_REFUSE_ADD,		// 拒绝帮会加入申请
	SSOI_TONG_ADD,				// relay 帮会成员添加成功，通知 core 进行相应的处理
	SSOI_TONG_INIT,
	SSOI_DATA_INIT,
	SSOI_CHAT_INIT,
	SSOI_TRAN_INIT,
	SSOI_GATEWAYINIT,
	SSOI_SETGOLADMISS,
	SSOI_KICKOUTPLAYER,
};

//=========================================================
// Core外部客户向core获取游戏数据的数据项内容索引定义
//=========================================================
//各数据项索引的相关参数uParam与nParam如果在注释中未提及，则传递定值0。
//如果特别指明返回值含义，则成功获取数据返回1，未成功返回0。
enum GAMEDATA_INDEX
{
	SGDI_CHARACTER_NAME,
	SGDI_CHARACTER_SEX,
	//uParam = (char*) buffer to retrive character name
	//nParam = Client index
	//Return = bIsExistCharacter
	SGDI_LOADEDMAP_ID,
	SGDI_CHARACTER_ACCOUNT,   //获取账号名
	SGDI_CHARACTER_ID,
	SGDI_CHARACTER_NETID,
	SGDI_CHARACTER_EXTPOINT,
	SGDI_CHARACTER_EXTPOINTCHANGED, //获取要扣除的扩展点
	SGDI_CHARACTER_MAPANDPOINT,     //获取角色说在地图的坐标
	//uParam = (char*) id buffer 

	// 传入帮会建立参数，返回条件是否成立
	// uParam : struct STONG_SERVER_TO_CORE_APPLY_CREATE point
	// return : 条件是否成立
	SGDI_TONG_APPLY_CREATE,

	// 申请加入帮会
	// uParam : struct STONG_SERVER_TO_CORE_APPLY_ADD point
	SGDI_TONG_APPLY_ADD,
	SGDI_TONG_APPLY_INVITE,
	SGDI_TONG_CHECK_POWER,

	// 判断加入帮会条件是否合适
	// uParam : 传入的 char point ，用于接收帮会名称
	// nParam : struct STONG_SERVER_TO_CORE_CHECK_ADD_CONDITION point
	SGDI_TONG_CHECK_ADD_CONDITION,

	// 获得帮会信息
	// uParam : 传入的 STONG_SERVER_TO_CORE_GET_INFO point
	SGDI_TONG_GET_INFO,

	// 判断是否有任命权利
	// uParam : 传入的 TONG_APPLY_INSTATE_COMMAND point
	// nParam : PlayerIndex
	SGDI_TONG_INSTATE_POWER,

	// 被任命，帮会数据变化
	// uParam : 传入的 STONG_SERVER_TO_CORE_BE_INSTATED point
	SGDI_TONG_BE_INSTATED,

	// 判断是否有踢人权利
	// uParam : 传入的 TONG_APPLY_KICK_COMMAND point
	// nParam : PlayerIndex
	SGDI_TONG_KICK_POWER,

	// 被踢出帮会
	// uParam : 传入的 STONG_SERVER_TO_CORE_BE_KICKED point
	SGDI_TONG_BE_KICKED,

	// 离开帮会判断
	// uParam : 传入的 TONG_APPLY_LEAVE_COMMAND point
	// nParam : PlayerIndex
	SGDI_TONG_LEAVE_POWER,

	// 离开帮会
	// uParam : 传入的 STONG_SERVER_TO_CORE_LEAVE point
	SGDI_TONG_LEAVE,

	// 能否传位判断
	// uParam : 传入的 TONG_APPLY_CHANGE_MASTER_COMMAND point
	// nParam : PlayerIndex
	SGDI_TONG_CHANGE_MASTER_POWER,

	// 能否接受传位判断
	// uParam : 传入的 STONG_SERVER_TO_CORE_CHECK_GET_MASTER_POWER point
	SGDI_TONG_GET_MASTER_POWER,

	// 传位导致身份改变
	// uParam : 传入的 STONG_SERVER_TO_CORE_CHANGE_AS point
	SGDI_TONG_CHANGE_AS,

	// 帮主换了
	// uParam : 传入的 STONG_SERVER_TO_CORE_CHANGE_MASTER point
	SGDI_TONG_CHANGE_MASTER,

	// 获得帮会名字符串转换成的 dword
	// nParam : PlayerIndex
	SGDI_TONG_GET_TONG_NAMEID,

	// 登陆时候获得帮会信息
	// uParam : 传入的 STONG_SERVER_TO_CORE_LOGIN point
	SGDI_TONG_LOGIN,

	// 通知core发送某玩家的帮会信息
	// nParam : player index
	SGDI_TONG_SEND_SELF_INFO,
	SGDI_TONG_MONEY_POWER, // check quyen rut tien
	SGDI_TONG_SAVE_MONEY_POWER, // check dieu kien gui tien
	// nParam : player index
	SGDI_TONG_CHANGE_MONEY,

	SGDI_CHARACTER_LEFTTIME,
	SGDI_CHARACTER_VIPTYPE,       //获取要扣除的扩展点
	SGDI_CHARACTER_EXTTIMECHANGED,
	SGDI_CHECK_TONG,              //检测帮会信息
	SGDI_TONG_CHECK_SET_POWER,
	SGDI_TONG_CHANGE_TITLE,
};

#ifdef _STANDALONE
class IClient;
#else
struct IClient;
#endif

#ifndef _STANDALONE
struct _declspec (novtable) iCoreServerShell
#else
struct iCoreServerShell
#endif
{
	virtual void mandatoryShutDownClient(int nIndex) = 0;
	virtual UINT GetLoopRate() = 0;
	virtual void GetGuid(int nIndex, void* pGuid) = 0;
	virtual UINT GetExchangeMap(int nIndex) = 0;
	virtual bool IsPlayerLoginTimeOut(int nIndex) = 0;
	virtual void RemovePlayerLoginTimeOut(int nIndex) = 0;
	virtual bool IsPlayerExchangingServer(int nIndex) = 0;
	virtual int  getClientIsMobile(int nIndex)=0;
	virtual int  serverKickOutGame(int nIndex)=0;
	virtual void ProcessClientMessage(int nIndex, const char* pChar, int nSize) = 0;
	virtual void ProcessNewClientMessage(IClient* pTransfer, DWORD dwFromIP, DWORD dwFromRelayID, int nPlayerIndex, const char* pChar, int nSize) = 0;
	virtual void SendNetMsgToTransfer(IClient* pClient) = 0;
	virtual void SendNetMsgToChat(IClient* pClient) = 0;
	virtual void SendNetMsgToTong(IClient* pClient) = 0;
	virtual void ProcessBroadcastMessage(const char* pChar, int nSize) = 0;
	virtual void ProcessExecuteMessage(const char* pChar, int nSize) = 0;
	virtual void ClientDisconnect(int nIndex,BOOL nIsOnline=FALSE) = 0;
	virtual void CheckSerMaps(int nIndex,BOOL nIsShow=FALSE,BOOL isOnline=TRUE)=0;
	virtual bool SavePlayerData(int nIndex,bool isLeave) = 0;
	virtual void RemoveQuitingPlayer(int nIndex) = 0;
	virtual void* SavePlayerDataAtOnce(int nIndex) = 0;
	virtual bool SetRoleSaveRevision(int nIndex, const char* roleName, unsigned __int64 revision) = 0;
	virtual bool GetRoleSaveRevision(int nIndex, const char* roleName, unsigned __int64* revision) = 0;
	virtual bool ConfirmRoleSaveRevision(int nIndex, const char* roleName, unsigned __int64 expected, unsigned __int64 next) = 0;
	virtual bool IsRoleSaveRevisionRequired(int nIndex, const char* roleName) = 0;
	virtual bool BeginRoleSave(int nIndex, const char* roleName, unsigned __int64* expected, unsigned __int64* nonce) = 0;
	virtual bool GetPendingRoleSave(int nIndex, const char* roleName, unsigned __int64* expected, unsigned __int64* nonce) = 0;
	virtual int FinishRoleSave(int nIndex, unsigned __int64 expected, unsigned __int64 nonce, int dbResult, unsigned __int64 committed) = 0;
	virtual bool IsCharacterQuiting(int nIndex) = 0;
	virtual bool IsShutdownClient(int nIndex)=0;
	virtual bool IsShutdownClientNoserver(int nIndex)=0;
	virtual int  GetGoldMissidVal(int nGoldMid)=0;
	virtual bool IsShutdownClientOutserver(int nIndex)=0;

    virtual int  GetClientNetConnectIdx(int nIndex)=0;
	virtual bool CheckProtocolSize(const char* pChar, int nSize) = 0;
	virtual bool PlayerDbLoading(int nPlayerIndex, int bSyncEnd, int& nStep, unsigned int& nParam) = 0;
	virtual int  AttachPlayer(const unsigned long lnID, GUID* pGuid,int model=0) = 0;
	virtual void GetPlayerIndexByGuid(GUID* pGuid, int* pnIndex, int* plnID) = 0;
	virtual void AddPlayerToWorld(int nIndex) = 0;
	virtual void* PreparePlayerForExchange(int nIndex) = 0;
//	virtual int  GetRegisterMa()=0;
//	virtual int  getMAC(char *mac)=0;
	virtual void PreparePlayerForLoginFailed(int nIndex) = 0;
	virtual void RemovePlayerForExchange(int nIndex) = 0;
	virtual void RecoverPlayerExchange(int nIndex) = 0;
	virtual int  AddCharacter(DWORD nExtPoint, LONG nChangeExtPoint, void* pBuffer, GUID* pGuid,DWORD nLeftTime=0,int nVipType=0,LONG nGameTime=0,BOOL nKeepForBt=TRUE,bool isExTrans=false,unsigned int nTempEnTerMapIndx=0,unsigned int  nTempKeyId=0,int m_IsFirstLogin=1) = 0;
//	virtual int	 AddTempTaskValue(int nIndex, const char* pData) = 0;
	//登录信息设置
/*	virtual int	 SetPlayeGameStates(int nPlayerIdx,int nStats)=0;
	virtual int	 SetPlayeNetStates(int nPlayerIdx,int nStats)=0;
	virtual int	 SetPlayePingStratTime(int nPlayerIdx,int npTime)=0;
	virtual int	 SetPlayePingReplyTime(int nPlayerIdx,int npTime)=0;
	virtual int	 SetPlayeExchangeStatus(int nPlayerIdx,int nStats)=0;
	virtual int	 GetPlayeNetStates(int nPlayerIdx)=0;
	*/
    virtual void doGoldthreadScript(const void *pData)=0;
	virtual void doTaskScript(const void *pData)=0;
	virtual void gm_addNpc(const void *pData)=0;
	virtual void gm_LocalScript(char *msg)=0;
	virtual void SendSystemCahtInfo(char *msg)=0;
	//向游戏发送操作
	virtual int	 OperationRequest(unsigned int uOper, UINT_PTR uParam, int nParam,int inParam=0) = 0;
	//获取连接状况
	virtual int	 GetConnectInfo(KCoreConnectInfo* pInfo,int nIndex=0) = 0;

	virtual int  GetNpcInfoByNamw(KNpcInfo* pInfo,char *strName) = 0;

	virtual int  gm_OperationRequest(const void *nData) = 0;

	virtual int  item_OperationRequest(const void *nData) = 0;

	virtual int  sendItemData(const void *nData) = 0;
	
//	virtual	BOOL ValidPingTime(int nIndex) = 0;
	//从游戏世界获取数据
	virtual INT_PTR	 GetGameData(unsigned int uDataId, UINT_PTR uParam, int nParam) = 0;

	virtual int	 GetGameData_New(unsigned int uDataId, UINT_PTR uParam, int nParam,char *strdata=NULL) = 0;

	virtual int	 GetGameDataInfo(unsigned int uDataId, unsigned long uParam, int nParam,char *strdata=NULL,void *pdata=NULL) = 0;
	virtual void ExecuteScript(RELAY_REMOTE* pRemote)=0;

//向游戏发送操作
	virtual int	 OperationRequestInfo(unsigned int uOper, UINT_PTR uParam, int nParam,int inParam=0,void *pdata=NULL,char *strdata=NULL) = 0;
	//日常活动，core如果要寿终正寝则返回0，否则返回非0值

	virtual INT GetExpData(int nIndex,int nType) = 0;

	virtual int  Breathe() = 0;
	//释放接口对象
	virtual void Release() = 0;
	virtual void SetSaveStatus(int nIndex, UINT uStatus) = 0;
	virtual void SetSaveStatus(char *strName, UINT uStatus) = 0;
	virtual void SetChangeRoleStatus(int nIndex, int uStatus,char *nRoleName) = 0;
	virtual UINT GetSaveStatus(int nIndex) = 0;
	virtual BOOL GroupChat(IClient* pClient, DWORD FromIP, unsigned long FromRelayID, DWORD channid, BYTE tgtcls, DWORD tgtid, const void* pData, size_t size, char* szSender) = 0;
	virtual	void SetLadder(void* pData, size_t uSize) = 0;
	virtual BOOL PayForSpeech(int nIndex, int nType) = 0;

	virtual int IsCharacterLiXian(int nIndex) = 0;
	virtual void SetCharacterLiXian(int nIndex,int i) = 0;

	virtual int  SetAttackInfo(const char * pMsg,int nPlayerIdx)=0;
//	virtual void SetAttackInfo(DWORD nNpcDwId,int nAttackState,int nAttackCamp,char *AttackName)=0;
   virtual	int  SetAttackWarCityInfo(const char * pMsg,int nPlayerIdx)=0;
	

	virtual void SetAttackCamp(char * nTongName,int nType)=0;

	virtual void SetAttackMsg(void *pdata,int nType)=0;
	virtual void ExeCoreTask()=0;
	virtual void CheckItemTime()=0;
	virtual void SynCoreItemData()=0;
	virtual void SavePaiShopData()=0;
	virtual void ReleaseDbData()=0;
	virtual void ExeCoreNpcState()=0;
   virtual void sendToClient_globalNews(const char *pData)=0;
   virtual void sendToClient_localNews(const char *pData)=0;
   virtual void sendToClient_globalCountNews(const char *pData)=0;

  virtual int  OnSetTran(LPVOID pServer=NULL)=0;
  virtual int  OnSetChat(LPVOID pServer=NULL)=0;
  virtual int  OnSetTong(LPVOID pServer=NULL)=0;
 virtual  int  OnSetData(LPVOID pServer=NULL)=0;
 virtual  int  OnSetGateWay(LPVOID pServer=NULL)=0;

 virtual void SetTongData(int nPlayerIdx, void* data)=0;
 virtual void SyncBlackList(int nPlayerIdx,  std::list<std::string> &blackList) = 0;
 virtual BOOL CheckBlackList(int nPlayerIdx, char* szName)=0;
 virtual void OnTradePairSaveResult(unsigned __int64 uTxId, int nResultCode, const char* szRoleA, const char* szRoleB) = 0;
};

#ifndef CORE_EXPORTS

	//获取iCoreShell接口实例的指针
#ifndef __linxu
	extern "C" 
#endif
	//iCoreServerShell* CoreGetServerShell();
	//单机设置
	iCoreServerShell* CoreGetServerShell(const char *gamePass="XAMFYVyha9CcG~thQSMG39RfKSg0_ofg");

#endif
#endif

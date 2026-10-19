/* -------------------------------------------------------------------------
//	文件名		：	gw_coreshell_i.h
//	创建者		：	lailigao
//	创建时间	：	2005-1-25 9:01:21
//	功能描述	：	
//
// -----------------------------------------------------------------------*/
#ifndef __GW_CORESHELL_I_H__
#define __GW_CORESHELL_I_H__

// -------------------------------------------------------------------------

interface IKGWCoreShell// : public IUnknown
{
	virtual INT Release() = 0;
	virtual INT	 GetProtocolSize(BYTE byProtocol) = 0;
	virtual INT	 Debug(UINT uDataId, UINT uParam, INT nParam) = 0;
	virtual INT	 OperationRequest(UINT uOper, UINT uParam, INT nParam) = 0;
	virtual VOID ProcessInput(UINT uMsg, UINT uParam, INT nParam) = 0;
	virtual INT	 FindSelectNPC(INT x, INT y, INT nRelation, bool bSelect, LPVOID pReturn, INT& nKind) = 0;
	virtual INT FindSelectObject(INT x, INT y, bool bSelect, INT& nObjectIdx, INT& nKind) = 0;
	virtual INT FindSpecialNPC(LPSTR Name, LPVOID pReturn, INT& nKind) = 0;
	virtual INT ChatSpecialPlayer(LPVOID pPlayer, LPCSTR pMsgBuff, USHORT nMsgLength) = 0;
	virtual VOID ApplyAddTeam(LPVOID pPlayer) = 0;
	virtual VOID TradeApplyStart(LPVOID pPlayer) = 0;
	virtual INT UseSkill(INT x, INT y, INT nSkillID) = 0;
	virtual INT LockSomeoneUseSkill(INT nTargetIndex, INT nSkillID) = 0;
	virtual INT LockSomeoneAction(INT nTargetIndex) = 0;
	virtual INT LockObjectAction(INT nTargetIndex) = 0;
	virtual VOID GotoWhere(INT x, INT y, INT mode) = 0;	//mode 0 is auto, 1 is walk, 2 is run
	virtual VOID Goto(INT nDir, INT mode) = 0;	//nDir 0~63, mode 0 is auto, 1 is walk, 2 is run
	virtual VOID Turn(INT nDir) = 0;	//nDir 0 is left, 1 is right, 2 is back
	virtual BOOL CheckRelationSet(INT nIndex, INT nRelation) = 0;
	
	//与地图相关的操作,uOper的取值来自 GAME_SCENE_MAP_OPERATION_INDEX
	virtual INT	SceneMapOperation(UINT uOper, UINT uParam, INT nParam) = 0;
	//与帮会相关的操作, uOper的取值来自 GAME_TONG_OPERATION_INDEX
	virtual INT	TongOperation(UINT uOper, UINT uParam, INT nParam) = 0;
	//与组队相关的操作，uOper的取值来自 GAME_TEAM_OPERATION_INDEX
	virtual INT TeamOperation(UINT uOper, UINT uParam, INT nParam) = 0;
	//与箱子相关的操作，uOper的取值来自 GAME_BOX_OPERATION_INDEX
//	virtual INT BoxOperation(UINT uOper, UINT uParam, INT nParam) = 0;

	virtual INT	 GetGameData(UINT uDataId, UINT uParam, INT nParam) = 0;

	//virtual VOID DrawGameObj(UINT uObjGenre, UINT uId, INT x, INT y, INT Width, INT Height, INT nParam, INT nParam2 = 0) = 0;
	//virtual VOID DrawGameSpace() = 0;
	virtual DWORD GetPing() = 0;
	//VOID SendPing();
	//virtual INT	 SetCallDataChangedNofify(IClientCallback* pNotifyFunc) = 0;
	//virtual VOID NetMsgCallbackFunc(LPVOID pMsgData, INT nSize) = 0;
	//virtual VOID SetRepresentShell(struct IRepresentShell* pRepresent) = 0;
	//virtual VOID SetMusicInterface(LPVOID pMusicInterface) = 0;
	//virtual VOID SetRepresentAreaSize(INT nWidth, INT nHeight) = 0;
	//virtual INT  Breathe() = 0;
	//virtual VOID Release() = 0;	//释放接口对象
	//virtual VOID SetClient(LPVOID pClient) = 0;
	virtual VOID SendNewDataToServer(LPVOID pData, INT nLength) = 0;
	virtual INT InsertItem(
		INT nGenre,
		INT nDetail,
		INT nParticular, 
		INT nLevel,
		INT nSeries,
		INT nEnhTimes,
		INT nLuck,
		LPVOID pGenInfo,
		INT nVersion,
		INT nRandSeed
	) = 0;
	virtual VOID RemoveItem(INT nIndex) = 0;
	// 开始/结束摆摊标价
	virtual VOID BeginSetPriceForStall() = 0;
	virtual VOID EndSetPriceForStall() = 0;

	//virtual INT KillerTask_GetMoneyPerHour() = 0;
	//virtual INT KillerTask_GetMaxActiveTaskTime() = 0;
	//virtual INT KillerTask_GetMinReward() = 0;

//-----------------------------------------------------------------------------
	virtual BOOL AddToLuaFuncLib(LPCSTR pszName, LPVOID pFuncs, INT nCount) = 0;
	virtual BOOL RegisterGameScriptFuns(LPVOID pScript) = 0;

	virtual VOID CoreDataChanged(UINT uDataId, UINT uParam, INT nParam) = 0;
};
// -------------------------------------------------------------------------

#endif /* __GW_CORESHELL_I_H__ */

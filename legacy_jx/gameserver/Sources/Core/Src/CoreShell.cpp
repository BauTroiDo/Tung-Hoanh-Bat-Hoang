/*****************************************************************************************
//	外界访问Core接口方法
//	Copyright : Kingsoft 2002
//	Author	:   
//	CreateTime:	2002-9-12
------------------------------------------------------------------------------------------
*****************************************************************************************/
#include "KCore.h"
#include "GameDataDef.h"
#include "CoreShell.h"

#include "CoreDrawGameObj.h"
#include "ImgRef.h"

#include "KPlayer.h"
#include "KPlayerSet.h"
#include "KPlayerTask.h"
#include "KTaskFuns.h"
#include "KItem.h"
#include "KItemList.h"
#include "KSubWorldSet.h"
#include "KProtocolProcess.h"
/////////后来加的////////////////
#include "KNpcAI.h"
#include "../../Engine/src/Text.h"
#include "time.h"
//#include "KFilePath.h"
//////////////////////////////////
#include "KNpcResList.h"
#include "Scene/KScenePlaceC.h"
#include "kskills.h"
//#include "GameDataDef.h"
#include "MsgGenreDef.h"
#include "KOption.h"
#include "KSubWorld.h"
#include "KViewItem.h"
#include "KTongProtocol.h"
#include "malloc.h"

#include "KBuySell.h"
#include "KThiefSkill.h"

#include "KSellItem.h"

#define	NPC_TRADE_BOX_WIDTH		6
#define	NPC_TRADE_BOX_HEIGHT	10
#define	MAX_TRADE_ITEM_WIDTH	2
#define	MAX_TRADE_ITEM_HEIGHT	4
IClientCallback* l_pDataChangedNotifyFunc = 0;

class KCoreShell : public iCoreShell
{
public:
	int	 GetProtocolSize(BYTE byProtocol);
	int	 Debug(unsigned int uDataId, unsigned int uParam, int nParam);
	int	 OperationRequest(unsigned int uOper, unsigned int uParam, int nParam,int nIsMianBan=0,char* strVal=NULL);
	void ProcessInput(unsigned int uMsg, unsigned int uParam, int nParam);
	int	 FindSelectNPC(int x, int y, int nRelation, bool bSelect, void* pReturn, int& nKind);
	int  FindSelectObject(int x, int y, bool bSelect, int& nObjectIdx, int& nKind);
	int  FindSpecialNPC(char* Name, void* pReturn, int& nKind);
	int  FindPlayerIdx(char* Name, void* pReturn, int& nKind);
	int  ChatSpecialPlayer(void* pPlayer, const char* pMsgBuff, unsigned short nMsgLength);
	void ApplyAddTeam(void* pPlayer);
	void TradeApplyStart(void* pPlayer);
	int  UseSkill(int x, int y, int nSkillID);
    int  AutoUseSkill();
	int  LockSomeoneUseSkill(int nTargetIndex, int nSkillID);
	int  LockSomeoneAction(int nTargetIndex);
	int  LockObjectAction(int nTargetIndex);
	void GotoWhere(int x, int y, int mode);	//mode 0 is auto, 1 is walk, 2 is run
	void Goto(int nDir, int mode);	//nDir 0~63, mode 0 is auto, 1 is walk, 2 is run
	void Turn(int nDir);	//nDir 0 is left, 1 is right, 2 is back
	int  ThrowAwayItem();
	int  GetNPCRelation(int nIndex);
	int  GetNPCBaiTan(int nIndex);

	int  GetGenreItem(unsigned int uId);
	int  GetTypeItem(unsigned int uId);
	
	void Guaji(int x, int y, int mode);
    void YaBiao();

    void Setpos(int x,int y);
    int  GetAutoplayid();//获取挂机状态参数
	int  GetVipType();   //获取Vip类型
	int	 GetObjColor(unsigned int uId);
    void RWorldScript(int nMapid);
	int  GetSkillLiveTime(int nSkillId);
	int  GetMapLeftTime();
	int  GetMapUseTime();
	void  AutoRun();
	int  GetAutoSay();

	int GetDataSuperShop(int nSaleId, unsigned int uParam, int nParam);
	int GetDataDynamicShop(int nSaleId, unsigned int uParam, int nParam);
	int GetDataNpcShop(int nSaleId, unsigned int uParam, int nParam);


	//与地图相关的操作,uOper的取值来自 GAME_SCENE_MAP_OPERATION_INDEX
	int	SceneMapOperation(unsigned int uOper, unsigned int uParam, int nParam);
	//与帮会相关的操作, uOper的取值来自 GAME_TONG_OPERATION_INDEX
	int	TongOperation(unsigned int uOper, unsigned int uParam, int nParam,int vnParam=0);
	//与组队相关的操作，uOper的取值来自 GAME_TEAM_OPERATION_INDEX
	int TeamOperation(unsigned int uOper, unsigned int uParam, int nParam);

	int	 GetGameData(unsigned int uDataId, unsigned int uParam, int nParam,int inParam=0);

	BOOL DrawGameObj(unsigned int uObjGenre, unsigned int uId, int x, int y, int Width, int Height, int nParam,int inParam=-1);
	void DrawGameSpace();
	DWORD GetPing();
	//void SendPing();
	int	 SetCallDataChangedNofify(IClientCallback* pNotifyFunc);
	void NetMsgCallbackFunc(void* pMsgData);
	void SetRepresentShell(struct iRepresentShell* pRepresent);
	void SetMusicInterface(void* pMusicInterface);
	void SetRepresentAreaSize(int nWidth, int nHeight);
	int  Breathe(); //客户端循环接入
	void Release();	//释放接口对象
	void SetClient(LPVOID pClient);
	void SendNewDataToServer(void* pData, int nLength);
	int  GetTradeItem(unsigned int uId);  //是否能交易
	int  GetDropItem(unsigned int uId);   //是否能丢弃
};

static KCoreShell	g_CoreShell;

CORE_API void g_InitCore(char *nParmName();

#ifndef _STANDALONE
     extern "C" __declspec(dllexport)
#endif

iCoreShell* CoreGetShell()
{
	g_InitCore();
	return &g_CoreShell;
}

//--------------------------------------------------------------------------
//	功能：发出游戏世界数据改变的通知函数
//	返回：如未被注册通知函数，则直接返回0，否则返回通知函数执行结果。
//--------------------------------------------------------------------------
void CoreDataChanged(unsigned int uDataId, unsigned int uParam, int nParam,int inVal)
{
	if (l_pDataChangedNotifyFunc)
		l_pDataChangedNotifyFunc->CoreDataChanged(uDataId, uParam, nParam,inVal);
}

void KCoreShell::Release()
{
	g_ReleaseCore();
}

//--------------------------------------------------------------------------
//	功能：客户端接受处理服务器分派的网络消息
//--------------------------------------------------------------------------
void KCoreShell::NetMsgCallbackFunc(void* pMsgData)
{
	g_ProtocolProcess.ProcessNetMsg((BYTE *)pMsgData);
}
//--------------------------------------------------------------------------
//	功能：设置游戏世界数据改变的通知函数
//	参数：fnCoreDataChangedCallback pNotifyFunc --> 通知函数的指针。
//	返回：返回值为非0值表示注册成功，否则表示失败。
//--------------------------------------------------------------------------
int	KCoreShell::SetCallDataChangedNofify(IClientCallback* pNotifyFunc)
{
	l_pDataChangedNotifyFunc = pNotifyFunc;
	return true;
}

//--------------------------------------------------------------------------
//	功能：从游戏世界获取数据
//	参数：unsigned int uDataId --> 表示获取游戏数据的数据项内容索引，其值为梅举类型
//							GAMEDATA_INDEX的取值之一。
//		  unsigned int uParam  --> 依据uDataId的取值情况而定
//		  int nParam --> 依据uDataId的取值情况而定
//	返回：依据uDataId的取值情况而定。
//--------------------------------------------------------------------------
int	KCoreShell::GetGameData(unsigned int uDataId, unsigned int uParam, int nParam,int inParam)
{
	int nRet = 0;
	switch(uDataId)
	{
	case GDI_PLAYER_IS_MALE:
		{
			int nIndex = 0;
			if (nParam == 0)
				nIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;
			else
				nIndex = NpcSet.SearchID(nParam);

			if (nIndex)
				nRet = (Npc[nIndex].m_NpcSettingIdx == PLAYER_MALE_NPCTEMPLATEID);
			else
				nRet = 1;	//出错时是男性
		}
		break;
	case GDI_REPAIR_ITEM_PRICE:
		if (uParam)
		{
			KUiObjAtContRegion *pObj = (KUiObjAtContRegion *)uParam;
			KItem*	pItem = NULL;

			switch(pObj->Obj.uGenre)
			{
			case CGOG_ITEM:
				{
					if (pObj->Obj.uId > 0)
					{
						pItem = &Item[pObj->Obj.uId];
					}
				}
				break;
			default:
				break;
			}

			if (!pItem)
				break;

			KUiItemBuySelInfo *pInfo = (KUiItemBuySelInfo *)nParam;
		
			if (pObj->eContainer == UOC_NPC_SHOP)
				break;

			pInfo->nPrice = pItem->GetRepairPrice();
			strcpy(pInfo->szItemName, pItem->GetName()); //取得物品的名称
			nRet = pItem->CanBeRepaired();               //返回是否能修理

			if (!nRet)
			{
			   Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("<color=yellow>修理<color>:该物品不需要修理!");
			}
		}
		else
		{
			nParam = 0;
			nRet = 0;
		}
		break;

	case GDI_MISSION_SELFDATA:
		if (uParam)
		{//个人数据
			TMissionLadderSelfInfo *pRankData = (TMissionLadderSelfInfo *)uParam;
			*pRankData = Player[CLIENT_PLAYER_INDEX].m_MissionData;
		}
		break;
	
	case GDI_MISSION_RANKDATA:
		if (uParam)
		{//十大排名
			TMissionLadderInfo *pRankData = (TMissionLadderInfo *)uParam;
			*pRankData = Player[CLIENT_PLAYER_INDEX].m_MissionRank[nParam];
		}
		break;
	case GDI_ITEM_DESC:
		if (nParam && uParam)
		{
			char* pszDescript = (char *)nParam;
			pszDescript[0] = 0;
			if (uParam>0 && uParam < MAX_ITEM)
			   Item[uParam].GetDesc(pszDescript);
		}
		break;

	case GDI_ITEM_LINKDATA:
		{
			 nRet = Player[CLIENT_PLAYER_INDEX].m_ItemLinkDwid;
		}
		break;
	case GDI_ITEM_NAME: // 取得名称
		if (uParam)
		{
			KItemlian *pObj = (KItemlian *)uParam;
			pObj->cClorid = Item[nParam].GetColor();
			pObj->nDwid   = Item[nParam].GetID();
			Item[nParam].SetLianjieFlg(1);                           //客户端设置为链接状态
			SendClientCmdSetLianFlg(Item[nParam].GetID(),1);         //服务器端设置链接状态
			snprintf(pObj->cItemName, sizeof(pObj->cItemName), "%s",Item[nParam].GetName());   //取得物品的名称
			snprintf(pObj->cPlayerName, sizeof(pObj->cPlayerName),"%s",Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name);                    //Player[CLIENT_PLAYER_INDEX].m_cTask.SetSaveVal(TASKVALUE_LIANJIEA, nParam); //玩家绑定装备号
			nRet = 1;
		}
		else
		{
			nParam = 0;
			nRet = 0;
		}
		break;
		
	case GDI_TRADE_ITEM_PRICE:
		if (uParam)
		{
			KUiObjAtContRegion *pObj = (KUiObjAtContRegion *)uParam;
			KItem*	pItem = NULL;

			switch(pObj->Obj.uGenre)
			{
			case CGOG_PLAYERSELLITEM:
				{//摆摊
					if (pObj->Obj.uId > 0)
					{
						KUiItemBuySelInfo *pInfo = (KUiItemBuySelInfo *)nParam;
						pInfo->nPrice     = Item[pObj->Obj.uId].GetTradePrice();  //获取价格
						pInfo->nPriceType = Item[pObj->Obj.uId].GetcModel();  //货币类型

						/*char msg[16];
						     sprintf(msg,"类型:%d",pInfo->nPriceType);
                         Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(msg);*/
						strcpy(pInfo->szItemName,Item[pObj->Obj.uId].GetName());
						return 1;
					}
				}
				break;
			case CGOG_ITEM:
				{//包袱 等
					if (pObj->Obj.uId > 0)
					{
						pItem = &Item[pObj->Obj.uId];
					}
				}
				break;
			case CGOG_NPCSELLITEM:
				{//NPC商店
					int	nBuyIdx = Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nBuyIdx;
					if (nBuyIdx != -1)
					{
						int nIndex = BuySell.GetItemIndex(nBuyIdx, pObj->Obj.uId);
						if (nIndex >= 0)
							pItem   = BuySell.GetItemidx(nIndex);
					}
				}
				break;
			default:
				break;
			}

			if (!pItem)   //商店 和包袱 
				break;

			KUiItemBuySelInfo *pInfo = (KUiItemBuySelInfo *)nParam;
		
			if (pObj->eContainer == UOC_NPC_SHOP)  // NPC 商店
			{
				if (pItem->GetPriceXu()>0)   //(pItem->GetGenre() == item_task)// 任务类 用金币买
				{//优先金币
					pInfo->nPriceXu = pItem->GetPriceXu();
				}
				else
				{
					pInfo->nPrice = pItem->GetPrice(); //其他的是剑侠B 买
				}
			}
			else if (pObj->eContainer == UOC_MARKET)  // 奇珍阁 金币
			{
				if (pItem->GetPriceXu()>0)   //(pItem->GetGenre() == item_task)// 任务类 用金币买
				{
					pInfo->nPriceXu = pItem->GetPriceXu();
				}
				else
				{
					pInfo->nPrice   = pItem->GetPrice(); //其他的是剑侠B 买
				}
				   // pInfo->nPriceXu = pItem->GetPriceXu();
			}
			else
			{//否则是容器
				pInfo->nPrice = pItem->GetPrice()/BUY_SELL_SCALE;
			}

			pInfo->nPriceType = pItem->GetcModel();  //货币类型
			
			strcpy(pInfo->szItemName, pItem->GetName()); //取得物品的名称

			nRet = 1;//pItem->GetGenre();(pItem->GetGenre() != item_task);   //不等于任务类？
		}
		else
		{
			nParam = 0;
			nRet = 0;
		}
		break;
	//游戏对象描述说明文本串
	//uParam = (KUiGameObject*) 描述游戏对象的结构数据的指针
	//nParam = (char*) 指向一个缓冲区的指针，其空间不少于256字节。
	case GDI_GAME_OBJ_DESC_INCLUDE_REPAIRINFO:
	case GDI_GAME_OBJ_DESC_INCLUDE_TRADEINFO:
		if (nParam && uParam)
		{//交易物品的说明
			KUiObjAtContRegion* pObj = (KUiObjAtContRegion *)uParam;
			char* pszDescript = (char *)nParam;
			pszDescript[0] = 0;
			switch(pObj->Obj.uGenre)
			{
			case CGOG_PLAYERSELLITEM:
				{//摆摊
					Item[pObj->Obj.uId].GetDesc(pszDescript);
					// 					char sPrice[20];
					// 					sprintf(sPrice,"价格：%d万%d两",Item[pObj->Obj.uId].GetSetPrice()/10000,Item[pObj->Obj.uId].GetTradePrice()%10000);
					// 					strcat(pszDescript,"<color=Yellow>");
					// 					strcat(pszDescript,sPrice);
					// 					strcat(pszDescript,"\n");
				}
				break;
			case CGOG_ITEM:
				{//所有物品
					if (pObj->eContainer == UOC_EQUIPTMENT)
					{
						int nActive = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetActiveAttribNum(pObj->Obj.uId);
						Item[pObj->Obj.uId].GetDesc(pszDescript, true, BUY_SELL_SCALE, nActive);
					}
					else
						Item[pObj->Obj.uId].GetDesc(pszDescript, true, BUY_SELL_SCALE);

					nRet = -1;
				}
				break;			
			case CGOG_NPCSELLITEM:
				{//商店
					int nIdx = -1;
					if (-1 == Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nBuyIdx)
						break;
					nIdx = BuySell.GetItemIndex(Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nBuyIdx, pObj->Obj.uId);

					KItem* pItem = NULL;
					if (nIdx < 0)
						break;
					pItem = BuySell.GetItemidx(nIdx);

					if (!pItem)
						break;
					pItem->GetDesc(pszDescript, true);

					nRet = -1;
				}
				break;
			}
		}
		break;
    case GDI_GAME_SKILL_INFO:
		if (uParam)
		{
			int nLevel = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetLevel(uParam);
			//_ASSERT(nLevel > 0);
            if (nLevel<=0)
			{
				nRet = 0;
                Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("提示:<color=yellow>该技能不能拾取!<color>");
				break;
			}
			if (uParam >0)
			{
				
				ISkill * pISkill = g_SkillManager.GetSkill(uParam, 1);
				if (!pISkill)
					break;
				eSkillStyle eStyle = (eSkillStyle) pISkill->GetSkillStyle();
				KSystemMessage	Msg;
				Msg.byConfirmType = SMCT_NONE;
				Msg.eType = SMT_NORMAL;
				Msg.byPriority = 1;
		  	    Msg.byParamSize = 0;
				switch(eStyle)
				{
				//case SKILL_SS_NewMissles:
				case SKILL_SS_Missles:			       //	子弹类	本技能用于发送子弹类
					nRet = 1;
					break;
				case SKILL_SS_Melee:                   //  攻击技能
					nRet = 1;				
					break;
				case SKILL_SS_InitiativeNpcState:	    //	主动类辅助		本技能用于改变当前Npc的主动状态
					{
					  if (pISkill->IsAura())
					  {
						nRet = 0;
					    sprintf(Msg.szMessage, "提示:<color=yellow>该技能不能拾取!<color>");
					    Msg.nMsgLen=TEncodeText(Msg.szMessage, strlen(Msg.szMessage));
					    CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&Msg, 0);
						break;
					  }

					  nRet = 1;
					}
					break;
				case SKILL_SS_PassivityNpcState:		//	被动类辅助		本技能用于改变Npc的被动状态	
					nRet = 0;
					sprintf(Msg.szMessage, "提示:<color=yellow>该技能不能拾取!<color>");
					Msg.nMsgLen=TEncodeText(Msg.szMessage, strlen(Msg.szMessage));
					CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&Msg, 0);
					break;	
				case SKILL_SS_Thief:
					nRet = 0;
					sprintf(Msg.szMessage, "提示:<color=yellow>该技能不能拾取!<color>");
					Msg.nMsgLen=TEncodeText(Msg.szMessage, strlen(Msg.szMessage));
					CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&Msg, 0);
					break;		
				}
			}		
		}
			break;
	case GDI_GAME_OBJ_DESC:  //显示物品的说明
		if (nParam && uParam)
		{
			KUiObjAtContRegion* pObj = (KUiObjAtContRegion *)uParam;

			char* pszDescript = (char *)nParam;

			pszDescript[0] = 0;

			switch(pObj->Obj.uGenre)
			{
			//case CGOG_COM_ITEM:
			case CGOG_IME_ITEM:
			case CGOG_ITEM:
				{
				 if (pObj->Obj.uId>0)
				 { 
					 if (pObj->eContainer == UOC_EQUIPTMENT)
					 {//装备着的 
						int nActive = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetActiveAttribNum(pObj->Obj.uId);

						if (Player[CLIENT_PLAYER_INDEX].m_ItemList.GetIfActive())
						{//如果是打开的
							nActive = 3;
						}
						Item[pObj->Obj.uId].GetDesc(pszDescript,false,1,nActive);
					 }
				  	 else
					 {//非装备 
                        if (inParam==1)
						{
                           int nItemBoxIdx= Item[pObj->Obj.uId].GetDetailType();

						   if (nItemBoxIdx==0 || nItemBoxIdx==1)
						   {//武器
						     pObj->Obj.uId = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipment(itempart_weapon);
						   }
						   else if (nItemBoxIdx==2)
						   {//衣服
                              pObj->Obj.uId = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipment(itempart_body);
						   }
						   else if (nItemBoxIdx==3)
						   {//戒子
                                 pObj->Obj.uId = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipment(itempart_ring1);
							  if (!pObj->Obj.uId)
							     pObj->Obj.uId = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipment(itempart_ring2);
						   }
						   else if (nItemBoxIdx==4)
						   {
                              pObj->Obj.uId = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipment(itempart_amulet);
						   }
						   else if (nItemBoxIdx==5)
						   {
                              pObj->Obj.uId = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipment(itempart_foot);
						   }
						   else if (nItemBoxIdx==6)
						   {//衣服
                              pObj->Obj.uId = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipment(itempart_belt);
						   }
						   else if (nItemBoxIdx==7)
						   {//衣服
                              pObj->Obj.uId = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipment(itempart_head);
						   }
						   else if (nItemBoxIdx==8)
						   {//衣服
                              pObj->Obj.uId = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipment(itempart_cuff);
						   }
						   else if (nItemBoxIdx==9)
						   {//衣服
                              pObj->Obj.uId = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipment(itempart_pendant);
						   }
						   else if (nItemBoxIdx==10)
						   {//衣服
                              pObj->Obj.uId = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipment(itempart_horse);
						   }
						   else if (nItemBoxIdx==11)
						   {//衣服
                              pObj->Obj.uId = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipment(itempart_mask);
						   }
						   else if (nItemBoxIdx==12)
						   {//衣服
                              pObj->Obj.uId = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipment(itempart_pifeng);
						   }
						   else if (nItemBoxIdx==13)
						   {//衣服
                              pObj->Obj.uId = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipment(itempart_yinjian);
						   }
						   else if (nItemBoxIdx==14)
						   {//衣服
                              pObj->Obj.uId = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipment(itempart_shiping);
						   }
						   if (pObj->Obj.uId<=0)
							  break;
						}

						Item[pObj->Obj.uId].GetDesc(pszDescript,false,1,0,inParam);
					 } 
					  if (Item[pObj->Obj.uId].GetGenre()==0)
						  nRet =0;
					  else
						  nRet =-1;
                       ///nRet=Item[pObj->Obj.uId].GetGenre();
				 } 
				}
				break;
			//case CGOG_IME_ITEM:   
			case CGOG_SKILL_SHIQU:
			case CGOG_SKILL:
			case CGOG_SKILL_FIGHT:
			case CGOG_SKILL_LIVE:
			case CGOG_SKILL_SHORTCUT:  //快捷技能信息  技能详细信息显示
				{
					int nLevel = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetLevel(pObj->Obj.uId);
				//	_ASSERT(nLevel >= 0);
					if (nLevel<0)
						break;

					if (pObj->Obj.uId >0 )
					{
						
						ISkill * pISkill = g_SkillManager.GetSkill(pObj->Obj.uId, 1);
						if (!pISkill)
							break;
						eSkillStyle eStyle = (eSkillStyle) pISkill->GetSkillStyle();
						
						switch(eStyle)
						{
						//case SKILL_SS_NewMissles:
						case SKILL_SS_Missles:			//	子弹类		本技能用于发送子弹类
						case SKILL_SS_Melee:
						case SKILL_SS_InitiativeNpcState:	//	主动类		本技能用于改变当前Npc的主动状态
						case SKILL_SS_PassivityNpcState:		//	被动类		本技能用于改变Npc的被动状态
							{
								int nList = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.FindSame(pObj->Obj.uId);
								KSkill::GetDesc(
									pObj->Obj.uId,
									nLevel,
									pszDescript,
									Player[CLIENT_PLAYER_INDEX].m_nIndex,
									(pObj->Obj.uGenre == CGOG_SKILL_SHORTCUT)?false:true,
									Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetAddPoint(nList),
									Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetEnChance(nList)
									);
							}
							break;
							
						case SKILL_SS_Thief:
							{
								((KThiefSkill *)pISkill)->GetDesc(
									pObj->Obj.uId,
									nLevel,
									pszDescript,
									Player[CLIENT_PLAYER_INDEX].m_nIndex,
									(pObj->Obj.uGenre == CGOG_SKILL_SHORTCUT)?false:true);
			
							}break;
							
						}
					}
					
				}
				break;
			
			case CGOG_PLAYER_FACE:
				break;
			case CGOG_NPCSELLITEM:
				{//商店的东西说明 
					int nIdx = -1;
					if (-1 == Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nBuyIdx)
						break;
					nIdx = BuySell.GetItemIndex(Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nBuyIdx, pObj->Obj.uId);

					KItem* pItem = NULL;
					if (nIdx < 0)
						break;
					pItem = BuySell.GetItemidx(nIdx);

					if (!pItem)
						break;
					pItem->GetDesc(pszDescript);
				}
				break;
			case CGOG_PLAYERSELLITEM:
				{//摆摊物品的说明
					Item[pObj->Obj.uId].GetDesc(pszDescript);
				}
				break;
			}
		}
		break;
	//主角的一些不易变的数据
	//uParam = (KUiPlayerBaseInfo*)pInfo
		
    case GDI_SCRIPT_VAL_WITH:
		{//客户端执行脚本！
			if (uParam)
			{
                 	KUiScrptinfo* pInfo = (KUiScrptinfo*)uParam;
					//char *nVal;
					if (nParam==0)
					{
					}   //sprintf(pInfo->szContent,Player[CLIENT_PLAYER_INDEX].ClientExeItemScript("\\Ui\\test.lua","GetItemDescription","",0,""));
		            else if (nParam==1)  //面板序号
					{
                          ZeroMemory(Player[CLIENT_PLAYER_INDEX].m_cTask.nTaskInfo,sizeof(Player[CLIENT_PLAYER_INDEX].m_cTask.nTaskInfo));
                       
					  if (inParam==0)  
					  {
						  if (Player[CLIENT_PLAYER_INDEX].ClientExeItemScript("\\Ui\\FsNewTaskWindow.lua","newtaskmain",0,0,""))
						  {
							  for (int i=0;i<20;++i)
							  {
                                    pInfo->nTaskInfo[i].nTaskidx = Player[CLIENT_PLAYER_INDEX].m_cTask.nTaskInfo[i].nTaskidx;
									pInfo->nTaskInfo[i].nTaskType = Player[CLIENT_PLAYER_INDEX].m_cTask.nTaskInfo[i].nTaskType;
									snprintf(pInfo->nTaskInfo[i].szContent, sizeof(pInfo->nTaskInfo[i].szContent), "%s", Player[CLIENT_PLAYER_INDEX].m_cTask.nTaskInfo[i].nTaskIconPath);
									snprintf(pInfo->nTaskInfo[i].nTaskName, sizeof(pInfo->nTaskInfo[i].nTaskName), "%s", Player[CLIENT_PLAYER_INDEX].m_cTask.nTaskInfo[i].nTaskName);
									
							  }
						  }
					   }
					   else
					   {//显示脚本内容
					    
                                    Player[CLIENT_PLAYER_INDEX].ClientExeItemScript("\\Ui\\FsNewTaskWindow.lua","newtaskrequestinfo",inParam,0,"") ;
					   }

					}
					else if (nParam==2)
					{
							 //RoomMain
					}
					else if (nParam==4)
					{
							 //RoomMain
							 Player[CLIENT_PLAYER_INDEX].ClientExeItemScript("\\Ui\\roomreset.lua","RoomMain",inParam,0,"") ;
							 
					}
			}  
		}
		break;
	case GDI_PLAYER_GETATTACK_INFO:
		{

			KUiAttackInfo * nAttack =(KUiAttackInfo *)uParam;
		    snprintf(nAttack->m_AttackName, sizeof(nAttack->m_AttackName), "%s", Player[CLIENT_PLAYER_INDEX].m_cTong.m_AttackName);//
			nAttack->m_AttackState=Player[CLIENT_PLAYER_INDEX].m_cTong.m_AttackState;//
			nAttack->m_AttackTime=Player[CLIENT_PLAYER_INDEX].m_cTong.m_AttackTime;//
			nAttack->m_nAttackNum=Player[CLIENT_PLAYER_INDEX].m_cTong.m_nAttackNum;//
			nAttack->m_nWinNum=Player[CLIENT_PLAYER_INDEX].m_cTong.m_nWinNum;//
			nAttack->m_nLoseNum=Player[CLIENT_PLAYER_INDEX].m_cTong.m_nLoseNum;//
			nAttack->m_nDeathCount=Player[CLIENT_PLAYER_INDEX].m_cTong.m_nDeathCount;//
			nAttack->m_nDDeathCount=Player[CLIENT_PLAYER_INDEX].m_cTong.m_nDDeathCount;
		}
		break;
	case GDI_PLAYER_ATTACK_INFO:
		{
			nRet=Player[CLIENT_PLAYER_INDEX].m_cTong.m_AttackState;
		}
		break;
	case GDI_PLAYER_TASK_INFO:
		{
			//nRet=Player[CLIENT_PLAYER_INDEX].m_cTong.m_AttackState;
			if (uParam)
			{
			  KUiBaseTask* pInfo = (KUiBaseTask*)uParam;
			  SendClientSetTaskVal(260);
		      SendClientSetTaskVal(261);
			  pInfo->nVala=Player[CLIENT_PLAYER_INDEX].m_cTask.GetSaveVal(260);
			  pInfo->nValb=Player[CLIENT_PLAYER_INDEX].m_cTask.GetSaveVal(261);
			}
		}
		break;

	case GDI_PLAYER_BASE_INFO:
		if (uParam)
		{
			KUiPlayerBaseInfo* pInfo = (KUiPlayerBaseInfo*)uParam;
			int nIndex = 0,nPlayerIdx=0;
			if (nParam == 0)
			{
				nIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;  //dwid
				pInfo->nCurFaction = Player[CLIENT_PLAYER_INDEX].m_cFaction.m_nCurFaction;
				pInfo->nCurTong = Player[CLIENT_PLAYER_INDEX].m_cTong.GetTongNameID();
				pInfo->nCurPinDao =	Player[CLIENT_PLAYER_INDEX].m_nCurPinDao;
			//	pInfo->nCurFaction = -1; //设置没门派
				nPlayerIdx=CLIENT_PLAYER_INDEX;
				pInfo->nMissionGroup  = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nMissionGroup;
			}
			else
			{
				nIndex = NpcSet.SearchID(nParam);
				pInfo->nCurFaction = -1;
				pInfo->nMissionGroup = -1;
				pInfo->nCurTong = 0;
				if (nIndex)
				   nPlayerIdx= Npc[nIndex].GetNpcPepoleID();
			}
			if (nIndex)
			{
				snprintf(pInfo->Name, sizeof(pInfo->Name), "%s", Npc[nIndex].Name);
				//to do:no implements in this version
				pInfo->Agname[0] = 0;
				pInfo->Title[0] = 0;
				if (Npc[nIndex].m_btRankId)
				{
					char szRankId[5];
					itoa(Npc[nIndex].m_btRankId, szRankId, 10);
					g_RankTabSetting.GetString(szRankId, "RANKSTR", "", pInfo->Title, 32);
				}

				pInfo->nRankInWorld = Player[nPlayerIdx].m_nWorldStat; //个人排名
				pInfo->nRankInWorld = Npc[nIndex].nRankInWorld;		//世界排名
				pInfo->nRepute = Npc[nIndex].nRepute;   //声望
				pInfo->nPKValue = Npc[nIndex].nPKValue; // PK
				pInfo->nReBorn = Npc[nIndex].nReBorn;   // 转生次数

				if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_TongBanNum)
				{//已经召唤了宠物的
					
					int nNpcIdx=0;
					nNpcIdx =NpcSet.SearchID(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_TongBanNum);
					if (nNpcIdx>0)
					{
					  pInfo->nTonLuky=Npc[nNpcIdx].m_nCurNpcLucky;
					}
					else
					{
					   pInfo->nTonLuky=0;
					} 
				}
				else
				{
				   pInfo->nTonLuky=0;
				}

				pInfo->nLucky  = Player[nPlayerIdx].m_nCurLucky*Npc[nIndex].IsLuKey*Npc[nIndex].m_GoldLucky; //当前幸运值
			    pInfo->nReBornLevel	= Player[nPlayerIdx].m_cReBorn.GetReBornCurLevel();
				if (nParam == 0)
				   SendClientSetTaskVal(130); //设置客户端的 门派唯一序号
			}
		}
		break;

	//主角的一些易变的数据
	//uParam = (KUiPlayerRuntimeInfo*)pInfo
    case GDI_PLAYER_SKILL_ADDPOINT:
		{ 
		    if (uParam)
			{ 
		      int nList = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.FindSame(uParam);
		      nRet = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetAddPoint(nList);//---额外技能点
			} 
		  }
		break;
	case GDI_PLAYER_RT_INFO:
		if (uParam)
		{
			KUiPlayerRuntimeInfo* pInfo = (KUiPlayerRuntimeInfo*)uParam;
			pInfo->nLifeFull = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentLifeMax;		//生命满值
			pInfo->nLife = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentLife;				//生命
			pInfo->nManaFull = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentManaMax;		//内力满值
			pInfo->nMana = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentMana;				//内力
			pInfo->nStaminaFull = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentStaminaMax;//体力满值
			pInfo->nStamina = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentStamina;		//体力

			pInfo->nAngryFull = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentNuQiMax;		//怒满值
			//pInfo->nAngry = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentNuQi;			//怒
			
			pInfo->nExperienceFull = Player[CLIENT_PLAYER_INDEX].m_nNextLevelExp;		        //经验满值
			pInfo->nExperience = Player[CLIENT_PLAYER_INDEX].m_nExp;					        //经验
			pInfo->nCurLevelExperience = Player[CLIENT_PLAYER_INDEX].m_nNextLevelExp;
			pInfo->nIsGetSkill = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_IsgetSkill;        //是否拾取技能状态
		//	pInfo->nIsGetChaqi = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_BtnFlag;           //是否插旗状态
			pInfo->byActionDisable = 0;
			//to do	给pInfo->bActionDisable、赋予合适的值
            pInfo->nVipType = Player[CLIENT_PLAYER_INDEX].m_nVipType; //会员类型
			pInfo->byAction = PA_NONE;

			if (Player[CLIENT_PLAYER_INDEX].m_RunStatus)
				pInfo->byAction |= PA_RUN;

			if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Doing == do_sit)
				pInfo->byAction |= PA_SIT;
			pInfo->wReserved = 0;
			pInfo->nGameLeftTime=Player[CLIENT_PLAYER_INDEX].m_GameLeftTime;
		}
		break;

	case GDI_PLAYER_GET_ANGRY:
		{
			if (uParam)
			{
			  KUiGetCurAngry* pInfo = (KUiGetCurAngry*)uParam;
			  pInfo->nAngry = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentNuQi;
			}
		}
		break;
	case GDI_TONGBAN_ID_INFO:
		if (uParam)
		{
			KUiTongBanbaseInfo* TpInfo = (KUiTongBanbaseInfo*)uParam;
			//查找BB的索引
            //int nNpcID= NpcSet.SearchGSName(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name); 
		    int nNpcID= NpcSet.SearchID(nParam);
			if (nNpcID>0 && Npc[nNpcID].m_btRankFFId==99)
			{
              TpInfo->nmLifeFull = Npc[nNpcID].m_CurrentLifeMax;		//生命满值
			  TpInfo->nmLife     = Npc[nNpcID].m_CurrentLife;				//生命
			  TpInfo->nmManaFull = Npc[nNpcID].m_CurrentManaMax;		//内力满值
			  TpInfo->nmMana     = Npc[nNpcID].m_CurrentMana;	
			}
			else
			{
              TpInfo->nmLifeFull = 100;		//生命满值
			  TpInfo->nmLife = 0;				//生命
			  TpInfo->nmManaFull = 100;		//内力满值
			  TpInfo->nmMana = 0;					
			}
		}
	break;
    case GDI_TONGBAN_INFO:
		if (uParam)
		{
			KUiTongBanInfo* TpInfo = (KUiTongBanInfo*)uParam;
			//查找BB的索引
            int nNpcID= NpcSet.SearchGSName(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name);
			
			if (nNpcID>0 && Npc[nNpcID].m_btRankFFId==99)
			{
               snprintf(TpInfo->nTname, sizeof(TpInfo->nTname), "%s", Npc[nNpcID].Name);
			   char nstrName[64]={0};
		       Npc[nNpcID].GetstrInfo(STR_GUISHU_NAME,nstrName,sizeof(nstrName));
			   snprintf(TpInfo->nGuiShuName, sizeof(TpInfo->nGuiShuName), "%s", nstrName);
			   
               TpInfo->nDwID=Npc[nNpcID].m_dwID;  //返回这个dwid
			   TpInfo->nCount=1;
               //char msg[64];
			   //sprintf(msg,"index:%d,dwid:%d",nNpcID,Npc[nNpcID].m_dwID);
			   // Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(msg);
			}
			else
			{
				sprintf(TpInfo->nTname,"宠物(未召唤)");
				char nstrName[64]={0};
		        Npc[nNpcID].GetstrInfo(STR_GUISHU_NAME,nstrName,sizeof(nstrName));
				snprintf(TpInfo->nGuiShuName, sizeof(TpInfo->nGuiShuName), "%s", nstrName);
				TpInfo->nDwID  = 0;
			    TpInfo->nCount = 1;   
			}
		}
	break;
	case GDI_JINGMAI_INFO:
		if (uParam)
		{
			KUiJinMaiInfo* JpInfo = (KUiJinMaiInfo*)uParam;
		    JpInfo->nBingJia= Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_JinMaiBingJia; 
           JpInfo->nZhenYuan=Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_ZhenYuan; 
		}
	break;
	case GDI_JMDESC_INFO:
	if (uParam)
	{
		  if (nParam==0) //督脉
		  {
			KUiJinMaiDesc* JpInfo = (KUiJinMaiDesc*)uParam;
			for (int i=1;i<17;++i)
			{
			g_FsJinMai.GetString(i+1,1,"",JpInfo->nJinMaiDesc[i].nXueNane,sizeof(JpInfo->nJinMaiDesc[i].nXueNane));
			g_FsJinMai.GetInteger(i+1,10,0,&JpInfo->nJinMaiDesc[i].ncZhenYuan);
			//g_FsJinMai.GetInteger(i+1,3,0,&JpInfo->nJinMaiDesc[i].ncRaTe);
			JpInfo->nJinMaiDesc[i].ncRaTe=100;
			g_FsJinMai.GetInteger(i+1,11,0,&JpInfo->nJinMaiDesc[i].ncHuMaiDan);
			//g_FsJinMai.GetInteger(i+1,6,0,&JpInfo->nJinMaiDesc[i].ncMgcid);	
			char nDesc[64];
			g_FsJinMai.GetString(i+1,6,"",nDesc,sizeof(nDesc));
			g_FsJinMai.GetInteger(i+1,10,0,&JpInfo->nJinMaiDesc[i].ncBinjia);
           
			snprintf(JpInfo->nJinMaiDesc[i].ncMiaoShu, sizeof(JpInfo->nJinMaiDesc[i].ncMiaoShu),
				"%s\n消耗真元:%d点\n消耗护脉丹:%d个\n需要兵甲值达:%d\n成功率:%d%s",
				nDesc,
				JpInfo->nJinMaiDesc[i].ncZhenYuan,
				JpInfo->nJinMaiDesc[i].ncHuMaiDan,
				JpInfo->nJinMaiDesc[i].ncBinjia,
				JpInfo->nJinMaiDesc[i].ncRaTe,"%");
			}
		  }
		  else if (nParam==1) 
		  {
			KUiJinMaiDesc* JpInfo = (KUiJinMaiDesc*)uParam;
			for (int i=1;i<17;++i)
			{
			g_FsJinMai.GetString(i+17,1,"",JpInfo->nJinMaiDesc[i].nXueNane,sizeof(JpInfo->nJinMaiDesc[i].nXueNane));
			g_FsJinMai.GetInteger(i+17,10,0,&JpInfo->nJinMaiDesc[i].ncZhenYuan);
			//g_FsJinMai.GetInteger(i+1,3,0,&JpInfo->nJinMaiDesc[i].ncRaTe);
			JpInfo->nJinMaiDesc[i].ncRaTe=100;
			g_FsJinMai.GetInteger(i+17,11,0,&JpInfo->nJinMaiDesc[i].ncHuMaiDan);
			//g_FsJinMai.GetInteger(i+1,6,0,&JpInfo->nJinMaiDesc[i].ncMgcid);	
			char nDesc[64];
			g_FsJinMai.GetString(i+17,6,"",nDesc,sizeof(nDesc));
			g_FsJinMai.GetInteger(i+17,10,0,&JpInfo->nJinMaiDesc[i].ncBinjia);
			snprintf(JpInfo->nJinMaiDesc[i].ncMiaoShu, sizeof(JpInfo->nJinMaiDesc[i].ncMiaoShu),
				"%s\n消耗真元:%d点\n消耗护脉丹:%d个\n需要兵甲值达:%d\n成功率:%d%s",
				nDesc,
				JpInfo->nJinMaiDesc[i].ncZhenYuan,
				JpInfo->nJinMaiDesc[i].ncHuMaiDan,
				JpInfo->nJinMaiDesc[i].ncBinjia,
				JpInfo->nJinMaiDesc[i].ncRaTe,"%");
			}
		  }
		  else if (nParam==2) 
		  {
			KUiJinMaiDesc* JpInfo = (KUiJinMaiDesc*)uParam;
			for (int i=1;i<17;++i)
			{
			g_FsJinMai.GetString(i+33,1,"",JpInfo->nJinMaiDesc[i].nXueNane,sizeof(JpInfo->nJinMaiDesc[i].nXueNane));
			g_FsJinMai.GetInteger(i+33,10,0,&JpInfo->nJinMaiDesc[i].ncZhenYuan);
			//g_FsJinMai.GetInteger(i+1,3,0,&JpInfo->nJinMaiDesc[i].ncRaTe);
			JpInfo->nJinMaiDesc[i].ncRaTe=100;
			g_FsJinMai.GetInteger(i+33,11,0,&JpInfo->nJinMaiDesc[i].ncHuMaiDan);
			//g_FsJinMai.GetInteger(i+1,6,0,&JpInfo->nJinMaiDesc[i].ncMgcid);	
			char nDesc[64];
			g_FsJinMai.GetString(i+33,6,"",nDesc,sizeof(nDesc));
			g_FsJinMai.GetInteger(i+33,10,0,&JpInfo->nJinMaiDesc[i].ncBinjia);
			snprintf(JpInfo->nJinMaiDesc[i].ncMiaoShu, sizeof(JpInfo->nJinMaiDesc[i].ncMiaoShu),
				"%s\n消耗真元:%d点\n消耗护脉丹:%d个\n需要兵甲值达:%d\n成功率:%d%s",
				nDesc,
				JpInfo->nJinMaiDesc[i].ncZhenYuan,
				JpInfo->nJinMaiDesc[i].ncHuMaiDan,
				JpInfo->nJinMaiDesc[i].ncBinjia,
				JpInfo->nJinMaiDesc[i].ncRaTe,"%");
			}
		  }
		  else if (nParam==3) 
		  {
			  KUiJinMaiDesc* JpInfo = (KUiJinMaiDesc*)uParam;
			for (int i=1;i<17;++i)
			{
			g_FsJinMai.GetString(i+49,1,"",JpInfo->nJinMaiDesc[i].nXueNane,sizeof(JpInfo->nJinMaiDesc[i].nXueNane));
			g_FsJinMai.GetInteger(i+49,10,0,&JpInfo->nJinMaiDesc[i].ncZhenYuan);
			//g_FsJinMai.GetInteger(i+1,3,0,&JpInfo->nJinMaiDesc[i].ncRaTe);
			JpInfo->nJinMaiDesc[i].ncRaTe=100;
			g_FsJinMai.GetInteger(i+49,11,0,&JpInfo->nJinMaiDesc[i].ncHuMaiDan);
			//g_FsJinMai.GetInteger(i+1,6,0,&JpInfo->nJinMaiDesc[i].ncMgcid);	
			char nDesc[64];
			g_FsJinMai.GetString(i+49,6,"",nDesc,sizeof(nDesc));
			g_FsJinMai.GetInteger(i+49,10,0,&JpInfo->nJinMaiDesc[i].ncBinjia);
			snprintf(JpInfo->nJinMaiDesc[i].ncMiaoShu, sizeof(JpInfo->nJinMaiDesc[i].ncMiaoShu),
				"%s\n消耗真元:%d点\n消耗护脉丹:%d个\n需要兵甲值达:%d\n成功率:%d%s",
				nDesc,
				JpInfo->nJinMaiDesc[i].ncZhenYuan,
				JpInfo->nJinMaiDesc[i].ncHuMaiDan,
				JpInfo->nJinMaiDesc[i].ncBinjia,
				JpInfo->nJinMaiDesc[i].ncRaTe,"%");
			}
		  }
		  else if (nParam==4) 
		  {
			KUiJinMaiDesc* JpInfo = (KUiJinMaiDesc*)uParam;
			for (int i=1;i<17;++i)
			{
			g_FsJinMai.GetString(i+65,1,"",JpInfo->nJinMaiDesc[i].nXueNane,sizeof(JpInfo->nJinMaiDesc[i].nXueNane));
			g_FsJinMai.GetInteger(i+65,10,0,&JpInfo->nJinMaiDesc[i].ncZhenYuan);
			//g_FsJinMai.GetInteger(i+1,3,0,&JpInfo->nJinMaiDesc[i].ncRaTe);
			JpInfo->nJinMaiDesc[i].ncRaTe=100;
			g_FsJinMai.GetInteger(i+65,11,0,&JpInfo->nJinMaiDesc[i].ncHuMaiDan);
			//g_FsJinMai.GetInteger(i+1,6,0,&JpInfo->nJinMaiDesc[i].ncMgcid);	
			char nDesc[64];
			g_FsJinMai.GetString(i+65,6,"",nDesc,sizeof(nDesc));
			g_FsJinMai.GetInteger(i+65,10,0,&JpInfo->nJinMaiDesc[i].ncBinjia);
			snprintf(JpInfo->nJinMaiDesc[i].ncMiaoShu, sizeof(JpInfo->nJinMaiDesc[i].ncMiaoShu),
				"%s\n消耗真元:%d点\n消耗护脉丹:%d个\n需要兵甲值达:%d\n成功率:%d%s",
				nDesc,
				JpInfo->nJinMaiDesc[i].ncZhenYuan,
				JpInfo->nJinMaiDesc[i].ncHuMaiDan,
				JpInfo->nJinMaiDesc[i].ncBinjia,
				JpInfo->nJinMaiDesc[i].ncRaTe,"%");
			}
		  }
		  else if (nParam==5) 
		  {
			KUiJinMaiDesc* JpInfo = (KUiJinMaiDesc*)uParam;
			for (int i=1;i<17;++i)
			{
			g_FsJinMai.GetString(i+81,1,"",JpInfo->nJinMaiDesc[i].nXueNane,sizeof(JpInfo->nJinMaiDesc[i].nXueNane));
			g_FsJinMai.GetInteger(i+81,10,0,&JpInfo->nJinMaiDesc[i].ncZhenYuan);
			//g_FsJinMai.GetInteger(i+1,3,0,&JpInfo->nJinMaiDesc[i].ncRaTe);
			JpInfo->nJinMaiDesc[i].ncRaTe=100;
			g_FsJinMai.GetInteger(i+81,11,0,&JpInfo->nJinMaiDesc[i].ncHuMaiDan);
			//g_FsJinMai.GetInteger(i+1,6,0,&JpInfo->nJinMaiDesc[i].ncMgcid);	
			char nDesc[64];
			g_FsJinMai.GetString(i+81,6,"",nDesc,sizeof(nDesc));
			g_FsJinMai.GetInteger(i+81,10,0,&JpInfo->nJinMaiDesc[i].ncBinjia);
			snprintf(JpInfo->nJinMaiDesc[i].ncMiaoShu, sizeof(JpInfo->nJinMaiDesc[i].ncMiaoShu),
				"%s\n消耗真元:%d点\n消耗护脉丹:%d个\n需要兵甲值达:%d\n成功率:%d%s",
				nDesc,
				JpInfo->nJinMaiDesc[i].ncZhenYuan,
				JpInfo->nJinMaiDesc[i].ncHuMaiDan,
				JpInfo->nJinMaiDesc[i].ncBinjia,
				JpInfo->nJinMaiDesc[i].ncRaTe,"%");
			}
		  }
		  else if (nParam==6) 
		  {
			KUiJinMaiDesc* JpInfo = (KUiJinMaiDesc*)uParam;
			for (int i=1;i<17;++i)
			{
			g_FsJinMai.GetString(i+97,1,"",JpInfo->nJinMaiDesc[i].nXueNane,sizeof(JpInfo->nJinMaiDesc[i].nXueNane));
			g_FsJinMai.GetInteger(i+97,10,0,&JpInfo->nJinMaiDesc[i].ncZhenYuan);
			//g_FsJinMai.GetInteger(i+1,3,0,&JpInfo->nJinMaiDesc[i].ncRaTe);
			JpInfo->nJinMaiDesc[i].ncRaTe=100;
			g_FsJinMai.GetInteger(i+97,11,0,&JpInfo->nJinMaiDesc[i].ncHuMaiDan);
			//g_FsJinMai.GetInteger(i+1,6,0,&JpInfo->nJinMaiDesc[i].ncMgcid);	
			char nDesc[64];
			g_FsJinMai.GetString(i+97,6,"",nDesc,sizeof(nDesc));
			g_FsJinMai.GetInteger(i+97,10,0,&JpInfo->nJinMaiDesc[i].ncBinjia);
			snprintf(JpInfo->nJinMaiDesc[i].ncMiaoShu, sizeof(JpInfo->nJinMaiDesc[i].ncMiaoShu),
				"%s\n消耗真元:%d点\n消耗护脉丹:%d个\n需要兵甲值达:%d\n成功率:%d%s",
				nDesc,
				JpInfo->nJinMaiDesc[i].ncZhenYuan,
				JpInfo->nJinMaiDesc[i].ncHuMaiDan,
				JpInfo->nJinMaiDesc[i].ncBinjia,
				JpInfo->nJinMaiDesc[i].ncRaTe,"%");
			}
		  }
		  else if (nParam==7) 
		  {
			KUiJinMaiDesc* JpInfo = (KUiJinMaiDesc*)uParam;
			for (int i=1;i<17;++i)
			{
			g_FsJinMai.GetString(i+113,1,"",JpInfo->nJinMaiDesc[i].nXueNane,sizeof(JpInfo->nJinMaiDesc[i].nXueNane));
			g_FsJinMai.GetInteger(i+113,10,0,&JpInfo->nJinMaiDesc[i].ncZhenYuan);
			//g_FsJinMai.GetInteger(i+1,3,0,&JpInfo->nJinMaiDesc[i].ncRaTe);
			JpInfo->nJinMaiDesc[i].ncRaTe=100;
			g_FsJinMai.GetInteger(i+113,11,0,&JpInfo->nJinMaiDesc[i].ncHuMaiDan);
			//g_FsJinMai.GetInteger(i+1,6,0,&JpInfo->nJinMaiDesc[i].ncMgcid);	
			char nDesc[64];
			g_FsJinMai.GetString(i+113,6,"",nDesc,sizeof(nDesc));
			g_FsJinMai.GetInteger(i+113,10,0,&JpInfo->nJinMaiDesc[i].ncBinjia);
			snprintf(JpInfo->nJinMaiDesc[i].ncMiaoShu, sizeof(JpInfo->nJinMaiDesc[i].ncMiaoShu),
				"%s\n消耗真元:%d点\n消耗护脉丹:%d个\n需要兵甲值达:%d\n成功率:%d%s",
				nDesc,
				JpInfo->nJinMaiDesc[i].ncZhenYuan,
				JpInfo->nJinMaiDesc[i].ncHuMaiDan,
				JpInfo->nJinMaiDesc[i].ncBinjia,
				JpInfo->nJinMaiDesc[i].ncRaTe,"%");
			}
		  }

	}
	break;
	case GDI_FUMOL_INFO:
		{
			/*KUiFuMoInfo* FmInfo = (KUiFuMoInfo*)uParam;

			if (nParam>=0)
			{
                      FmInfo->nNpcSkill[nParam]   = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nFuMoNum[nParam].nNpcSetings;
					  FmInfo->nSkillLevel[nParam] = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nFuMoNum[nParam].nSkillLevel;
				
					  if  (FmInfo->nNpcSkill[nParam]>0)
					  {//已经激活的
						  if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_TongBanNum)
						  {//已经召唤了宠物的

							  int nMix=0,nMax=0,nNpcIdx=0;
							      nNpcIdx =NpcSet.SearchID(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_TongBanNum);
							  
							  if (nNpcIdx>0)
							  {
							     if(NpcSet.SearchNpcSettingID(Npc[nNpcIdx].Name)==FmInfo->nNpcSkill[nParam])
								 {//如果是同一个编号的
									 int nPindex= Player[CLIENT_PLAYER_INDEX].m_nIndex;
									 Npc[nPindex].ApplySynNpcInfo(Npc[nPindex].m_TongBanNum);
								     Npc[nNpcIdx].GetTongBanDamage(&nMix,&nMax);
							         FmInfo->nNpcGongji[nParam]=nMax;                     //攻击
									 FmInfo->nNpclife[nParam]=Npc[nNpcIdx].m_CurrentLife; //生命
									 FmInfo->nNpcFangyu[nParam]=(Npc[nNpcIdx].m_CurrentPhysicsResist+ Npc[nNpcIdx].m_CurrentColdResist+Npc[nNpcIdx].m_CurrentLightResist+Npc[nNpcIdx].m_CurrentFireResist+Npc[nNpcIdx].m_CurrentPoisonResist);
									 FmInfo->nNpcGongsu[nParam] = Npc[nNpcIdx].GetAttackFrame()+Npc[nNpcIdx].GetCastFrame();
									 FmInfo->nNpcXingYun[nParam] = Npc[nNpcIdx].m_nCurNpcLucky;
								 }
							  }  
						  }
					  }
			} */
		}
		break;
	case GDI_LAOHUJI_INFO:
		{
			KUiLaoHuJi* lhInfo = (KUiLaoHuJi*)uParam;
			lhInfo->nIsOpen=Player[CLIENT_PLAYER_INDEX].m_GuziGame.nIsOpen;
			lhInfo->nType=Player[CLIENT_PLAYER_INDEX].m_GuziGame.nTpye;
			lhInfo->nYinL=Player[CLIENT_PLAYER_INDEX].m_GuziGame.nYingL;
			lhInfo->nJinBi=Player[CLIENT_PLAYER_INDEX].m_GuziGame.nJinBi;
			lhInfo->nBeiLv=Player[CLIENT_PLAYER_INDEX].m_GuziGame.nBeiLv;
			lhInfo->nDianShu=Player[CLIENT_PLAYER_INDEX].m_GuziGame.nDianShu;
		}
		break;
	case GDI_JMLEVEL_INFO:
		if (uParam)
		{
		   	KUiJinMaiLevel* JpInfo = (KUiJinMaiLevel*)uParam;
			/*
			int vala,valb,valc,vald;
	    vala=EGetByte(Player[CLIENT_PLAYER_INDEX].m_cTask.GetSaveVal(3501),1);
		valb=EGetByte(Player[CLIENT_PLAYER_INDEX].m_cTask.GetSaveVal(3501),2);
		valc=EGetByte(Player[CLIENT_PLAYER_INDEX].m_cTask.GetSaveVal(3501),3);
		vald=EGetByte(Player[CLIENT_PLAYER_INDEX].m_cTask.GetSaveVal(3501),4);
			
			*/
              if (nParam==1)
			  {//督脉
			      JpInfo->nJinMai[nParam].nLeVel[0]=EGetByte(Player[CLIENT_PLAYER_INDEX].m_cTask.GetSaveVal(3501),1);		      
			      int nCurLevel=EGetByte(Player[CLIENT_PLAYER_INDEX].m_cTask.GetSaveVal(3501),2);
			    if (nCurLevel>0)   
				  for (int i=1;i<18;++i)
				  {
                       if (i>nCurLevel)
					   {
					      JpInfo->nJinMai[nParam].nLeVel[i]=0;
					      continue;
					   }

					   JpInfo->nJinMai[nParam].nLeVel[i]=1;   //设置已经是冲穴了	   
				  }
			  }
              else if (nParam==2)
			  {//督脉
			      JpInfo->nJinMai[nParam].nLeVel[0]=EGetByte(Player[CLIENT_PLAYER_INDEX].m_cTask.GetSaveVal(3502),1);		      
			      int nCurLevel=EGetByte(Player[CLIENT_PLAYER_INDEX].m_cTask.GetSaveVal(3502),2);
			    if (nCurLevel>0)   
				  for (int i=1;i<18;++i)
				  {
                       if (i>nCurLevel)
					   {
					      JpInfo->nJinMai[nParam].nLeVel[i]=0;
					      continue;
					   }

					   JpInfo->nJinMai[nParam].nLeVel[i]=1;   //设置已经是冲穴了	   
				  }
			  }
			  else if (nParam==3)
			  {//督脉
			      JpInfo->nJinMai[nParam].nLeVel[0]=EGetByte(Player[CLIENT_PLAYER_INDEX].m_cTask.GetSaveVal(3503),1);		      
			      int nCurLevel=EGetByte(Player[CLIENT_PLAYER_INDEX].m_cTask.GetSaveVal(3503),2);
			    if (nCurLevel>0)   
				  for (int i=1;i<18;++i)
				  {
                       if (i>nCurLevel)
					   {
					      JpInfo->nJinMai[nParam].nLeVel[i]=0;
					      continue;
					   }

					   JpInfo->nJinMai[nParam].nLeVel[i]=1;   //设置已经是冲穴了	   
				  }
			  }
			  else if (nParam==4)
			  {//督脉
			      JpInfo->nJinMai[nParam].nLeVel[0]=EGetByte(Player[CLIENT_PLAYER_INDEX].m_cTask.GetSaveVal(3504),1);		      
			      int nCurLevel=EGetByte(Player[CLIENT_PLAYER_INDEX].m_cTask.GetSaveVal(3504),2);
			    if (nCurLevel>0)   
				  for (int i=1;i<18;++i)
				  {
                       if (i>nCurLevel)
					   {
					      JpInfo->nJinMai[nParam].nLeVel[i]=0;
					      continue;
					   }

					   JpInfo->nJinMai[nParam].nLeVel[i]=1;   //设置已经是冲穴了	   
				  }
			  }
			  else if (nParam==5)
			  {//督脉
			      JpInfo->nJinMai[nParam].nLeVel[0]=EGetByte(Player[CLIENT_PLAYER_INDEX].m_cTask.GetSaveVal(3505),1);		      
			      int nCurLevel=EGetByte(Player[CLIENT_PLAYER_INDEX].m_cTask.GetSaveVal(3505),2);
			    if (nCurLevel>0)   
				  for (int i=1;i<18;++i)
				  {
                       if (i>nCurLevel)
					   {
					      JpInfo->nJinMai[nParam].nLeVel[i]=0;
					      continue;
					   }

					   JpInfo->nJinMai[nParam].nLeVel[i]=1;   //设置已经是冲穴了	   
				  }
			  }
			  else if (nParam==6)
			  {//督脉
			      JpInfo->nJinMai[nParam].nLeVel[0]=EGetByte(Player[CLIENT_PLAYER_INDEX].m_cTask.GetSaveVal(3506),1);		      
			      int nCurLevel=EGetByte(Player[CLIENT_PLAYER_INDEX].m_cTask.GetSaveVal(3506),2);
			    if (nCurLevel>0)   
				  for (int i=1;i<18;++i)
				  {
                       if (i>nCurLevel)
					   {
					      JpInfo->nJinMai[nParam].nLeVel[i]=0;
					      continue;
					   }

					   JpInfo->nJinMai[nParam].nLeVel[i]=1;   //设置已经是冲穴了	   
				  }
			  }
			  else if (nParam==7)
			  {//督脉
			      JpInfo->nJinMai[nParam].nLeVel[0]=EGetByte(Player[CLIENT_PLAYER_INDEX].m_cTask.GetSaveVal(3507),1);		      
			      int nCurLevel=EGetByte(Player[CLIENT_PLAYER_INDEX].m_cTask.GetSaveVal(3507),2);
			    if (nCurLevel>0)   
				  for (int i=1;i<18;++i)
				  {
                       if (i>nCurLevel)
					   {
					      JpInfo->nJinMai[nParam].nLeVel[i]=0;
					      continue;
					   }

					   JpInfo->nJinMai[nParam].nLeVel[i]=1;   //设置已经是冲穴了	   
				  }
			  }
			  else if (nParam==8)
			  {//督脉
			      JpInfo->nJinMai[nParam].nLeVel[0]=EGetByte(Player[CLIENT_PLAYER_INDEX].m_cTask.GetSaveVal(3508),1);		      
			      int nCurLevel=EGetByte(Player[CLIENT_PLAYER_INDEX].m_cTask.GetSaveVal(3508),2);
			    if (nCurLevel>0)   
				  for (int i=1;i<18;++i)
				  {
                       if (i>nCurLevel)
					   {
					      JpInfo->nJinMai[nParam].nLeVel[i]=0;
					      continue;
					   }

					   JpInfo->nJinMai[nParam].nLeVel[i]=1;   //设置已经是冲穴了	   
				  }
			  }
			  else
			  {//丹田

			  }

			  JpInfo->nZhenYuan = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_ZhenYuan; 
			  JpInfo->nBinJia   = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_JinMaiBingJia; 
		}
	break;
   	case GDI_PLAYER_ID_INFO:
		if (uParam)
		{
			KUiPlayerindexbaseInfo* KpInfo = (KUiPlayerindexbaseInfo*)uParam;
				 int nNpcID=0;
               // nNpcID=GetnpcID.FindNpcIindex(nParam); //查找 NPC 索引
		             nNpcID= NpcSet.SearchID(nParam);//查找 NPC 索引
			//	nNpcID= SubWorld[0].FindNpcFromName(szName);  服务器端
				if (nNpcID>0)
                {           
					//if (strcmp(itoa(Npc[nT].m_dwID,Buffa,16), itoa(nParam,Buffb,16)) == 0)  //对比m_dwID号 是否相同 				  
					KpInfo->nmLifeFull = Npc[nNpcID].m_CurrentLifeMax;		//生命满值
					KpInfo->nmLife = Npc[nNpcID].m_CurrentLife;				//生命
					KpInfo->nmManaFull = Npc[nNpcID].m_CurrentManaMax;		//内力满值
					KpInfo->nmMana = Npc[nNpcID].m_CurrentMana;				//内力
					KpInfo->nmStaminaFull = Npc[nNpcID].m_CurrentStaminaMax;//体力满值
					KpInfo->nmStamina = Npc[nNpcID].m_CurrentStamina;		//体力	
					KpInfo->nmLuky=Player[CLIENT_PLAYER_INDEX].m_nCurLucky*Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].IsLuKey*Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_GoldLucky; //当前幸运值;
				}

		}
		break;
	//主角的一些易变的属性数据
	//uParam = (KUiPlayerAttribute*)pInfo
	case GDI_PLAYER_RT_ATTRIBUTE:
		if (uParam)
		{
			KUiPlayerAttribute* pInfo = (KUiPlayerAttribute*)uParam;
			KNpc*	pNpc = &Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex];
			pInfo->nMoney = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetsMon(room_equipment);				//银两
			pInfo->nXu = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetxLient(room_equipment);
			pInfo->nBARemainPoint = Player[CLIENT_PLAYER_INDEX].m_nAttributePoint;						//基本属性剩余点数
			pInfo->nStrength = Player[CLIENT_PLAYER_INDEX].m_nCurStrength;								//力量
			pInfo->nDexterity = Player[CLIENT_PLAYER_INDEX].m_nCurDexterity;							//身法
			pInfo->nVitality = Player[CLIENT_PLAYER_INDEX].m_nCurVitality;								//活力
			pInfo->nEnergy = Player[CLIENT_PLAYER_INDEX].m_nCurEngergy;	//精力
			pInfo->nPKValue = Player[CLIENT_PLAYER_INDEX].m_cPK.GetPKValue();	//tnpk
			pInfo->nRepute = Player[CLIENT_PLAYER_INDEX].m_cRepute.GetReputeValue(); //声望
			pInfo->nFuYuan = Player[CLIENT_PLAYER_INDEX].m_cFuYuan.GetFuYuanValue(); //富源
			pInfo->nReBorn = Player[CLIENT_PLAYER_INDEX].m_cReBorn.GetReBornNum();   //转生次数
			pInfo->nRankInWorld = Player[CLIENT_PLAYER_INDEX].m_nWorldStat;
            pInfo->nLuky=Player[CLIENT_PLAYER_INDEX].m_nCurLucky*Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].IsLuKey*Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_GoldLucky; //当前幸运值
            pInfo->nZhonji=pNpc->m_CurrentzhongjiRate;  //当前重击m_nCurLucky
            pInfo->nDangge=pNpc->m_CurrentdanggeRate; //当前档格m_nUpExp
            pInfo->nSprRankId  = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_btRankFFId; //称号ID

			Player[CLIENT_PLAYER_INDEX].GetEchoDamage(&pInfo->nRightKillMin , &pInfo->nRightKillMax, &pInfo->nAttack, 1);
			Player[CLIENT_PLAYER_INDEX].GetEchoDamage(&pInfo->nKillMIN, &pInfo->nKillMAX, &pInfo->nAttack, 0);				//最大最小杀伤力
			pInfo->nAttack      = pNpc->m_CurrentSkillMingZhong+(pNpc->m_CurrentAttackRating+pNpc->m_CurrentAttackRatingEnhancev)*(100+pNpc->m_CurrentAttackRatingEnhancep)/100;				//命中力
			pInfo->nDefence     = pNpc->m_CurrentDefend;				//闪避率
			pInfo->nMoveSpeed   = pNpc->m_CurrentRunSpeed;				//移动速度
			pInfo->nAttackSpeed = pNpc->m_CurrentAttackSpeed;			//外攻击速度
			pInfo->nCastSpeed   = pNpc->m_CurrentCastSpeed;             //内攻击速度

			pInfo->nForBitChat  = Player[CLIENT_PLAYER_INDEX].m_nForbiddenFlag;
			//物理防御
			if (pNpc->m_CurrentPhysicsResistMax >= pNpc->m_CurrentPhysicsResist)
				snprintf(pInfo->nPhyDef, sizeof(pInfo->nPhyDef),"%d/%d/<color=Orange>%d", pNpc->m_CurrentPhysicsResist,pNpc->m_CurrentPhysicsResistMax,pNpc->m_PhysicsResistMax);
			else
				snprintf(pInfo->nPhyDef, sizeof(pInfo->nPhyDef),"%d/%d/<color=Orange>%d", pNpc->m_CurrentPhysicsResistMax,pNpc->m_CurrentPhysicsResistMax,pNpc->m_PhysicsResistMax);
			//冰冻防御
			if (pNpc->m_CurrentColdResistMax >= pNpc->m_CurrentColdResist)
                snprintf(pInfo->nCoolDef, sizeof(pInfo->nCoolDef),"%d/%d/<color=Orange>%d", pNpc->m_CurrentColdResist,pNpc->m_CurrentColdResistMax,pNpc->m_ColdResistMax);
			else
				snprintf(pInfo->nCoolDef, sizeof(pInfo->nCoolDef),"%d/%d/<color=Orange>%d", pNpc->m_CurrentColdResistMax,pNpc->m_CurrentColdResistMax,pNpc->m_ColdResistMax);
			//闪电防御
			if (pNpc->m_CurrentLightResistMax >= pNpc->m_CurrentLightResist)
				snprintf(pInfo->nLightDef, sizeof(pInfo->nLightDef),"%d/%d/<color=Orange>%d", pNpc->m_CurrentLightResist,pNpc->m_CurrentLightResistMax,pNpc->m_LightResistMax);
			else
				snprintf(pInfo->nLightDef, sizeof(pInfo->nLightDef),"%d/%d/<color=Orange>%d", pNpc->m_CurrentLightResistMax,pNpc->m_CurrentLightResistMax,pNpc->m_LightResistMax);
			//火焰防御
			if (pNpc->m_CurrentFireResistMax >= pNpc->m_CurrentFireResist)
				snprintf(pInfo->nFireDef, sizeof(pInfo->nFireDef),"%d/%d/<color=Orange>%d", pNpc->m_CurrentFireResist,pNpc->m_CurrentFireResistMax,pNpc->m_FireResistMax);
			else
				snprintf(pInfo->nFireDef, sizeof(pInfo->nFireDef),"%d/%d/<color=Orange>%d", pNpc->m_CurrentFireResistMax,pNpc->m_CurrentFireResistMax,pNpc->m_FireResistMax);
			//毒素防御
			if (pNpc->m_CurrentPoisonResistMax >= pNpc->m_CurrentPoisonResist)
				snprintf(pInfo->nPoisonDef, sizeof(pInfo->nPoisonDef),"%d/%d/<color=Orange>%d", pNpc->m_CurrentPoisonResist,pNpc->m_CurrentPoisonResistMax,pNpc->m_PoisonResistMax);
			else
				snprintf(pInfo->nPoisonDef, sizeof(pInfo->nPoisonDef),"%d/%d/<color=Orange>%d", pNpc->m_CurrentPoisonResistMax,pNpc->m_CurrentPoisonResistMax,pNpc->m_PoisonResistMax);

         /* int					m_FireResistMax;		// Npc的最大火抗性
			int					m_ColdResistMax;		// Npc的最大冰抗性
			int					m_PoisonResistMax;		// Npc的最大毒抗性
			int					m_LightResistMax;		// Npc的最大电抗性
	        int					m_PhysicsResistMax;		// Npc的最大物理抗性
          */
			pInfo->nLevel = pNpc->m_Level;

			// 根据玩家状态显示 状态文字 not end 不应该是状态，应该是五行属性 spe
			memset(pInfo->StatusDesc, 0, sizeof(pInfo->StatusDesc));
			switch(pNpc->m_Series)
			{
			case series_water:
				strcpy(pInfo->StatusDesc, "水系");
				break;
			case series_wood:
				strcpy(pInfo->StatusDesc, "木系");
				break;
			case series_metal:
				strcpy(pInfo->StatusDesc, "金系");
				break;
			case series_fire:
				strcpy(pInfo->StatusDesc, "火系");
				break;
			case series_earth:
				strcpy(pInfo->StatusDesc, "土系");
				break;
			}
		}
		break;

	//主角的立即使用物品与武功
	//uParam = (KUiPlayerImmedItemSkill*)pInfo
	case GDI_PLAYER_IMMED_ITEMSKILL:
		if (uParam)
		{
			KUiPlayerImmedItemSkill* pInfo = (KUiPlayerImmedItemSkill*)uParam;
			memset(pInfo,0,sizeof(KUiPlayerImmedItemSkill));
			pInfo->IMmediaSkill[0].uGenre	= CGOG_SKILL_SHORTCUT;
			pInfo->IMmediaSkill[0].uId		= Player[CLIENT_PLAYER_INDEX].GetLeftSkill(); //左键技能
			pInfo->IMmediaSkill[1].uGenre	= CGOG_SKILL_SHORTCUT;
			pInfo->IMmediaSkill[1].uId		= Player[CLIENT_PLAYER_INDEX].GetRightSkill();//右键技能

			for (int i = 0; i < MAX_IMMEDIACY_ITEM; ++i)
			{//快捷栏物品
				pInfo->ImmediaItem[i].uId = Player[CLIENT_PLAYER_INDEX].m_ItemList.m_Room[room_immediacy].FindItem(i, 0);
				if (pInfo->ImmediaItem[i].uId > 0)
				{
					pInfo->ImmediaItem[i].uGenre = CGOG_ITEM;
				}
				else
				{
					pInfo->ImmediaItem[i].uGenre = CGOG_NOTHING;
				}
			}
		}
		break;
	case GDI_TRADE_PLAYER_ITEM_COUNT:
		nRet = 0;
		if (nParam==0)
		{
		  SendClientCmdGetCount(uParam);  //获取摆摊者的物品数量
		}
		else
		{
          SendClientCmdLianCount(uParam);  //获取链接的物品数量
		}
		break;

	case GDI_TRADE_PLAYER_ITEM:
		nRet = 0;
		if (uParam)
		{
			int nCount = 0;
			KUiObjAtContRegion* pInfo = (KUiObjAtContRegion*)uParam;
			
			for (int i = 0; i < 60; ++i)
			{

				if (nCount>nParam)
					break;

				if (g_cSellItem.m_sItem[i].nIdx)
				{
					pInfo->Obj.uGenre    = CGOG_PLAYERSELLITEM;//CGOG_ITEM;
					pInfo->Obj.uId       = g_cSellItem.m_sItem[i].nIdx;  //物品的索引
					
					pInfo->Region.Width  = Item[pInfo->Obj.uId].GetWidth();
					pInfo->Region.Height = Item[pInfo->Obj.uId].GetHeight();
					pInfo->Region.h      = g_cSellItem.m_sItem[i].nX;
					pInfo->Region.v      = g_cSellItem.m_sItem[i].nY;
					pInfo->nContainer    = g_cSellItem.m_sItem[i].nPrice;
					++nCount;
					++pInfo; //下一个
				}		
			}
			
			nRet = nCount;
		} 
		else
		{
			nRet = g_cSellItem.GetCount();  //摆摊数量
		}
		break;
	case GDI_TRADE_LIAN_ITEM:
		nRet = 0;
		if (uParam)
		{
			int nCount = 1;
			KUiObjAtContRegion* pInfo = (KUiObjAtContRegion*)uParam;
			char * nplayeName = (char *)nParam;			
				if (g_cSellItem.m_sLItem[0].nIdx)
				{
					pInfo->Obj.uGenre    = CGOG_ITEM; //    CGOG_NPCSELLITEM  CGOG_ITEM
					pInfo->Obj.uId       = g_cSellItem.m_sLItem[0].nIdx;				
					pInfo->Region.Width  = 2;//Item[pInfo->Obj.uId].GetWidth();容器的大小
					pInfo->Region.Height = 2;//Item[pInfo->Obj.uId].GetHeight();
					pInfo->Region.h      = 0;//容器中的位置
					pInfo->Region.v      = 0;
					//pInfo->nContainer    = (int)UOC_TO_BE_TRADE;//g_cSellItem.m_sItem[0].nLianJieFlg;
				}				
			    //nRet = nCount;
				char msg[144]={0};
				snprintf(msg, 144, "提示:获取<color=yellow>%s<color>发布的<color=green>买卖信息<color>成功!",nplayeName);
				Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(msg);
		} 	
		break;
	case GDI_PLAYER_IS_BAITAN: //是否在摆摊的状态
		nRet = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_BaiTan;
		break;
	case GDI_PLAYER_SETTIME_INFO:
		{
			KUiPlayerSetTimeInfo *Time = (KUiPlayerSetTimeInfo *)uParam;
			KUiPlyerDoscript nDoscript;
			switch(nParam)
			{
			case 0:
				{
				   nDoscript.inCurTime  = Time->nYinLiangTime - Player[CLIENT_PLAYER_INDEX].m_YinLiangTime;
                   Player[CLIENT_PLAYER_INDEX].m_YinLiangTime = Time->nYinLiangTime;
				   nDoscript.inKind=0;
				}
				break;
			case 1:
				{
                    nDoscript.inCurTime  = Time->nExpTime - Player[CLIENT_PLAYER_INDEX].m_ExpTime;
					Player[CLIENT_PLAYER_INDEX].m_ExpTime      = Time->nExpTime;
					nDoscript.inKind=1;
				}
				break;
			case 2:
				{
					nDoscript.inCurTime  = Time->nJinBiTime - Player[CLIENT_PLAYER_INDEX].m_JinBiTime;
					Player[CLIENT_PLAYER_INDEX].m_JinBiTime = Time->nJinBiTime;
					nDoscript.inKind=2;

				}
				break;
			case 3:
				{
                    nDoscript.inCurTime = Time->nExpPiontTime - Player[CLIENT_PLAYER_INDEX].m_ExpPiontTime;
					Player[CLIENT_PLAYER_INDEX].m_ExpPiontTime = Time->nExpPiontTime;
					nDoscript.inKind=3;
				}
				break;
			default:
				break;
			}

			 SendClientDoScipt(&nDoscript);
		}
	break;
	case GDI_PLAYER_DOSCRIPT:
		{ 
	      KUiPlyerDoscript nscript;
          memset(&nscript, 0, sizeof(nscript));
          nscript.inKind    = uParam;
          nscript.inCurTime = nParam;
		  nscript.inCurTimeb = 0;
		  nscript.inCurTimec = 0;
		  nscript.inCurTimed = 0;
          SendClientDoScipt(&nscript);

		} 
	break;
	case GDI_PLAYER_TIME_INFO:	
		{
             int nMin=0,nHour=0;
          if  (uParam)
		  { 
                 KUiPlayerActivityInfo *Time = (KUiPlayerActivityInfo *)uParam;

                 char nTimeInfo[64];

                 nMin  = (g_SubWorldSet.m_nLoopRate)/(18*60);//当前的循环时间   - (int)Player[CLIENT_PLAYER_INDEX].m_dwLoginTime
                 nHour = 0;

			     if (nMin>60)
				 { 
			       nHour = nMin/60;  //小时
				   nMin -=60*nHour;
				 } 

                 Time->nHour     = nHour;
                 Time->nMin      = nMin;
			     Time->nNpcLevel = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Level;

				 Time->nExpTime       = Player[CLIENT_PLAYER_INDEX].m_ExpTime;
				 Time->nExpPiontTime  = Player[CLIENT_PLAYER_INDEX].m_ExpPiontTime;
				 Time->nYinLiangTime  = Player[CLIENT_PLAYER_INDEX].m_YinLiangTime;
				 Time->nJinBiTime     = Player[CLIENT_PLAYER_INDEX].m_JinBiTime;
                 snprintf(Time->nTimeInfo, sizeof(Time->nTimeInfo),"今天在线时间:%d小时%d分",nHour,nMin);
		  } 

		}
	break;
	case GDI_PLAYER_TIMEINFO:
		{
			if  (uParam)
			{ 
				 KUiPlayerGameTime *pTime = (KUiPlayerGameTime *)uParam;
                    pTime->npTime = (g_SubWorldSet.m_nLoopRate)/18;  //秒
			}
		}
		break;
	//主角随身携带的钱
	//nRet = 主角随身携带的钱
	case GDI_PLAYER_HOLD_MONEY:
		{
			if (nParam==0)
		        nRet = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetsMon(room_equipment);  //得到剑侠币
			else if (nParam==1)
			    nRet = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetxLient(room_equipment); //得到金币
			else if (nParam==2)
			    nRet = Player[CLIENT_PLAYER_INDEX].m_cFuYuan.GetFuYuanValue(); //得到富源
			else if (nParam==3)
				nRet = Player[CLIENT_PLAYER_INDEX].m_cRepute.GetReputeValue(); //得到声望
			else if (nParam==5)
			{
			  //for (int i=0;i<5;i++)
			  {
				  PLAYER_GET_TASKVAL_COMMAND GetTaskval;
				  GetTaskval.ProtocolType = c2s_gettaskval;
				  GetTaskval.m_TaskId     = 192;
				  GetTaskval.m_pDwid=Npc[CLIENT_PLAYER_INDEX].m_dwID;	
				  if (g_pClient)
					g_pClient->SendPackToServer((BYTE*)&GetTaskval, sizeof(PLAYER_GET_TASKVAL_COMMAND));

				  //Sleep(500);
			  }

			  nRet = Player[CLIENT_PLAYER_INDEX].m_cTask.GetSaveVal(192);    //得到积分
			}

			break;
			//else if (nParam==4)
			//	nRet = Player[CLIENT_PLAYER_INDEX].GetExtPoint(); 
		}

		break;
	case GDI_PLAYER_GETTONGMAP_INFO:
		{
			nRet =Player[CLIENT_PLAYER_INDEX].m_cTong.m_Mapid;
		}
		break;
	case GDI_PLAYER_HOLD_XU:	
		    nRet = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetxLient(room_equipment); //得到金币
		break;
	//主角随身携带的物品等
	//uParam = (KUiObjAtRegion*) pInfo -> KUiObjAtRegion结构数组的指针，KUiObjAtRegion
	//				结构用于存储物品或者武功的数据及其放置区域位置信息。
	//nParam = pInfo数组中包含KUiObjAtRegion结构的数目
	//Return = 如果返回值小于等于传入参数nParam，其值表示pInfo数组中的前多少个KUiObjAtRegion
	//			结构被填充了有效的数据；否则表示需要传入包含多少个KUiObjAtRegion结构的数组
	//			才够存储全部的随身携带的物品信息。

	// flying添加了一个是否可以骑马的请求，在这里实现，gg可乐一罐，mm请吃KFC哦
	case GDI_PLAYER_CAN_RIDE:
		// 实现分支的代码在这里添加
		break;

	case GDI_ITEM_TAKEN_WITH: //玩家随身所带的物品信息！
		nRet = 0;
		if (uParam)
		{
			int nCount = 0;
			KUiObjAtRegion* pInfo = (KUiObjAtRegion*)uParam;
			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if (pItem && pItem->nPlace == pos_equiproom)
			{
				pInfo->Obj.uGenre = CGOG_ITEM;
				pInfo->Obj.uId = pItem->nIdx;
				pInfo->Region.h = pItem->nX;
				pInfo->Region.v = pItem->nY;
				pInfo->Region.Width = Item[pItem->nIdx].GetWidth();
				pInfo->Region.Height = Item[pItem->nIdx].GetHeight();
				nCount++;
				pInfo++;
			}
			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem && pItem->nPlace == pos_equiproom)
				{
					pInfo->Obj.uGenre = CGOG_ITEM;
					pInfo->Obj.uId = pItem->nIdx;		
					pInfo->Region.h = pItem->nX;
					pInfo->Region.v = pItem->nY;
					pInfo->Region.Width = Item[pItem->nIdx].GetWidth();
					pInfo->Region.Height = Item[pItem->nIdx].GetHeight();
					nCount++;
					pInfo++;
				}
				if (nCount > nParam)
					break;
			}
			nRet = nCount;
		}
		else if (nParam==0)
		{
			int nCount = 0;
			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if (pItem && pItem->nPlace == pos_equiproom)
				nCount++;
			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem && pItem->nPlace == pos_equiproom)
					nCount++;
			}
			nRet = nCount;
		}
		else if (nParam==1)
		{
			int nCount = 0;
			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if (pItem && pItem->nPlace == pos_equiproom && Item[pItem->nIdx].GetTradePrice()>0)  //摆摊价格大于零的
				nCount++;
			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem && pItem->nPlace == pos_equiproom && Item[pItem->nIdx].GetTradePrice()>0)
					nCount++;
			}

			nRet = nCount;
		}
		break;
    case GDI_ITEM_TAKEN_UPRICE: 
	{

	}
	break;
	case GDI_DAZAO_ITEM_INFO: //玩家随身所带的物品信息！
		nRet = 0;
		if (uParam)
		{
			int nCount = 0;
			KUiObjAtRegion* pInfo = (KUiObjAtRegion*)uParam;
			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if (pItem && pItem->nPlace == pos_cailiao)
			{
				pInfo->Obj.uGenre    = CGOG_ITEM;
				pInfo->Obj.uId       = pItem->nIdx;
				pInfo->Region.h      = pItem->nX;
				pInfo->Region.v      = pItem->nY;
				pInfo->Region.Width  = Item[pItem->nIdx].GetWidth();
				pInfo->Region.Height = Item[pItem->nIdx].GetHeight();

				nCount++;
				pInfo++;
			}
			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem && pItem->nPlace == pos_cailiao)
				{
					pInfo->Obj.uGenre = CGOG_ITEM;
					pInfo->Obj.uId = pItem->nIdx;		
					pInfo->Region.h = pItem->nX;
					pInfo->Region.v = pItem->nY;
					pInfo->Region.Width = Item[pItem->nIdx].GetWidth();
					pInfo->Region.Height = Item[pItem->nIdx].GetHeight();
					nCount++;
					pInfo++;
				}
				if (nCount > nParam)
					break;
			}
			nRet = nCount;
		}
		else
		{
			int nCount = 0;
			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if (pItem && pItem->nPlace == pos_cailiao)
				nCount++;
			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem && pItem->nPlace == pos_cailiao)
					nCount++;
			}
			nRet = nCount;
		}
		break;
	case GDI_ITEMBOX_INFO:
	{
			KUiObjAtRegion* pInfo = (KUiObjAtRegion*)uParam;

				pInfo->Obj.uId = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetItemBox(nParam);

				if (pInfo->Obj.uId)
				{
					pInfo->Obj.uGenre = CGOG_ITEM;
				
					pInfo->Region.Width = Item[pInfo->Obj.uId].GetWidth();
					pInfo->Region.Height = Item[pInfo->Obj.uId].GetHeight();
					pInfo->Region.v = nParam;
					pInfo->Region.h = 0;
					nRet =1;
				}
				else
				{
					pInfo->Obj.uGenre = CGOG_NOTHING;
				}
	}
	break;
	//主角装备物品
	//uParam = (KUiObjAtRegion*)pInfo -> 包含10个元素的KUiObjAtRegion结构数组指针，
	//			KUiObjAtRegion结构用于存储装备的数据和放置位置信息。
	//			KUiObjAtRegion::Region::h 表示属于第几套装备
	//			KUiObjAtRegion::Region::v 表示属于哪个位置的装备,其值为梅举类型
	//			UI_EQUIPMENT_POSITION的取值之一。请参看UI_EQUIPMENT_POSITION的注释。
	//nParam =	要获取的是第几套装备信息
	//Return =  其值表示pInfo数组中的前多少个KUiObjAtRegion结构被填充了有效的数据。
	case GDI_EQUIPMENT:
		nRet = 0;
		if (uParam)
		{
			// TODO：暂时没有做第二套装备
			if (nParam == 1)
				break;

			int PartConvert[itempart_num] = 
			{
				UIEP_HEAD,		UIEP_BODY,
				UIEP_WAIST,		UIEP_HAND,
				UIEP_FOOT,		UIEP_FINESSE,
				UIEP_NECK,		UIEP_FINGER1,
				UIEP_FINGER2,	UIEP_WAIST_DECOR,
				UIEP_HORSE,		UIEP_MASK,
				UIEP_PIFENG,    UIEP_YINJIAN, UIEP_SHIPING
			};

			int nCount = 0;
			KUiObjAtRegion* pInfo = (KUiObjAtRegion*)uParam;

			for (int i = 0; i < itempart_num; ++i)
			{
				pInfo->Obj.uId = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipment(i);//获取这个BOX的里的索引
				if (pInfo->Obj.uId)
				{
					pInfo->Obj.uGenre = CGOG_ITEM;
				
					pInfo->Region.Width = Item[pInfo->Obj.uId].GetWidth();
					pInfo->Region.Height = Item[pInfo->Obj.uId].GetHeight();
					pInfo->Region.v = PartConvert[i];
					pInfo->Region.h = 0;
				}
				else
				{
					pInfo->Obj.uGenre = CGOG_NOTHING;
				}
				nCount++;
				pInfo++;
			}
			nRet = nCount;
		}
		break;
	/*case GDI_CHESSMENT:
	    nRet = 0;
		if (uParam)
		{
			// TODO：暂时没有做第二套装备
			if (nParam == 1)
				break;	
			int nCount = 0;
			KUiObjAtRegion* pInfo = (KUiObjAtRegion*)uParam;
			
			for (int i = 0; i <MAX_GLOBAL_CHESSCOUNT; ++i)
			{
				pInfo->Obj.uId = SubWorld[0].m_MapChessBox[i];//Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipment(i);//获取这个BOX的里的索引
				
				if (pInfo->Obj.uId)
				{
					pInfo->Obj.uGenre = CGOG_CHESS_ITEM;
					
					pInfo->Region.Width =  35;
					pInfo->Region.Height = 35;
					pInfo->Region.v = pInfo->Obj.uId; //第几个位置是否有东西
					pInfo->Region.h = 0;
				}
				else
				{
					pInfo->Obj.uGenre = CGOG_NOTHING;
				}

				nCount++;
				pInfo++;
			}
			nRet = nCount;
		}
		break;*/
	case GDI_PARADE_EQUIPMENT:
		nRet = 0;
		if (uParam)
		{
			// TODO：暂时没有做第二套装备
			if (nParam == 1)
				break;

			int PartConvert[itempart_num] = 
			{
				UIEP_HEAD,		UIEP_BODY,
				UIEP_WAIST,		UIEP_HAND,
				UIEP_FOOT,		UIEP_FINESSE,
				UIEP_NECK,		UIEP_FINGER1,
				UIEP_FINGER2,	UIEP_WAIST_DECOR,
				UIEP_HORSE,		UIEP_MASK,
				UIEP_PIFENG,    UIEP_YINJIAN, UIEP_SHIPING
			};

			int nCount = 0;
			KUiObjAtRegion* pInfo = (KUiObjAtRegion*)uParam;

			for (int i = 0; i < itempart_num; ++i)
			{
				pInfo->Obj.uId = g_cViewItem.m_sItem[i].nIdx;
				if (pInfo->Obj.uId)
				{
					pInfo->Obj.uGenre = CGOG_ITEM;
				
					pInfo->Region.Width  = Item[pInfo->Obj.uId].GetWidth();
					pInfo->Region.Height = Item[pInfo->Obj.uId].GetHeight();
					pInfo->Region.h = 0;
					pInfo->Region.v = PartConvert[i];
				}
				else
				{
					pInfo->Obj.uGenre = CGOG_NOTHING;
				}
				nCount++;
				pInfo++;
			}
			nRet = nCount;
		}
		break;
		
	//交易操作相关的数据
	//uParam = (UI_TRADE_OPER_DATA)eOper 具体含义见UI_TRADE_OPER_DATA
	//nParam 具体应用与含义由uParam的取值状况决定,见UI_TRADE_OPER_DATA的说明
	//Return 具体含义由uParam的取值状况决定,见UI_TRADE_OPER_DATA的说明
	//主角的生活技能数据
	case GDI_TRADE_OPER_DATA:
		if (uParam == UTOD_IS_LOCKED)
			nRet = Player[CLIENT_PLAYER_INDEX].m_cTrade.m_nTradeLock;
		else if (uParam == UTOD_IS_TRADING)
			nRet = Player[CLIENT_PLAYER_INDEX].m_cTrade.m_nTradeState;
		else if (uParam == UTOD_IS_OTHER_LOCKED)
			nRet = Player[CLIENT_PLAYER_INDEX].m_cTrade.m_nTradeDestLock;
		else if (uParam == UTOD_IS_WILLING)
		{
			nRet = (Player[CLIENT_PLAYER_INDEX].m_cMenuState.m_nState == PLAYER_MENU_STATE_TRADEOPEN);
		}
		break;

	//uParam = (KUiPlayerLiveSkillBase*) pInfo -> 主角的生活技能数据
	case GDI_LIVE_SKILL_BASE:
		if (uParam)
		{
			KUiPlayerLiveSkillBase* pInfo = (KUiPlayerLiveSkillBase*)uParam;
			//to do:no implements in this version;
			pInfo->nLiveExperience = 0 ;
			pInfo->nRemainPoint = 0 ;
			pInfo->nLiveExperienceFull = 0 ;
			memset(pInfo,0,sizeof(KUiPlayerLiveSkillBase));
		}
		break;

	//主角掌握的各项生活技能
	//uParam = (unsigned int*) pSkills -> 包含10个unsigned int的数组用于存储各项生活技能的id。
	case GDI_LIVE_SKILLS:
		if (uParam)
		{
			//to do:no implements in this version;
			KUiSkillData* pInfo = (KUiSkillData*)uParam;
			memset(pInfo,0,sizeof(KUiSkillData)*10);
		}
		break;

	//剩余战斗技能点数
	//Return = 剩余战斗技能点数
	case GDI_FIGHT_SKILL_POINT:
		//to do:no implements in this version;
		nRet = Player[CLIENT_PLAYER_INDEX].m_nSkillPoint;
		break;

	//主角掌握的各项战斗技能
	//uParam = (unsigned int*) pSkills -> 包含30个unsigned int的数组用于存储各项战斗技能的id。
	case GDI_FIGHT_SKILLS:
		if (uParam)
		{
			KUiSkillData* pSkills = (KUiSkillData*)uParam;
			Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetSkillSortList(pSkills);
		}
		break;
	//显示左键技能列表
	//uParam = (KUiSkillData*) pSkills -> 包含65个KUiSkillData的数组用于存储各技能的数据。
	//								KUiSkillData::nLevel用来表述技能显示在第几行
	//Return = 返回有效数据的Skills的数目
	case GDI_LEFT_ENABLE_SKILLS:
		{
			KUiSkillData * pSkills = (KUiSkillData*)uParam;
			int nCount = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetLeftSkillSortList(pSkills);
			return nCount;

		}
		break;
	//显示右键技能列表
	//uParam = (KUiSkillData*) pSkills -> 包含65个KUiSkillData的数组用于存储各技能的数据。
	//								KUiSkillData::nLevel用来表述技能显示在第几行
	//Return = 返回有效数据的Skills的数目
	case GDI_RIGHT_ENABLE_SKILLS:
		{
			KUiSkillData * pSkills = (KUiSkillData*)uParam;
			int nCount = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetRightSkillSortList(pSkills);
			return nCount;
		}
		break;
	//主角的自创武功
	//uParam = (unsigned int*) pSkills -> 包含5个unsigned int的数组用于存储各项自创武功的id。
	case GDI_CUSTOM_SKILLS:
		if (uParam)
		{
			//to do: no implements in this version;
			KUiSkillData* pSkills = (KUiSkillData*)uParam;
			memset(pSkills,0,sizeof(KUiSkillData)*5);
		}
		break;
	//获取周围玩家的列表
	//uParam = (KUiPlayerItem*)pList -> 人员信息列表
	//			KUiPlayerItem::nData = 0
	//nParam = pList数组中包含KUiPlayerItem结构的数目
	//Return = 如果返回值小于等于传入参数nParam，其值表示pList数组中的前多少个KUiPlayerItem
	//			结构被填充了有效的数据；否则表示需要传入包含多少个KUiPlayerItem结构的数组
	//			才够存储全部人员信息。
	case GDI_NEARBY_PLAYER_LIST:
		nRet = NpcSet.GetAroundPlayer((KUiPlayerItem*)uParam, nParam);
		break;

	//获取周围孤单可受邀请的玩家的列表
	//参数含义同GDI_NEARBY_PLAYER_LIST
	case GDI_NEARBY_IDLE_PLAYER_LIST:
		nRet = NpcSet.GetAroundPlayerForTeamInvite((KUiPlayerItem*)uParam, nParam);
		break;

	//主角统帅能力相关的数据
	//uParam = (KUiPlayerLeaderShip*) -> 主角统帅能力相关的数据结构指针
	case GDI_PLAYER_LEADERSHIP:
		if (uParam)
		{
			KUiPlayerLeaderShip* pInfo = (KUiPlayerLeaderShip*)uParam ;
			pInfo->nLeaderShipExperience = Player[CLIENT_PLAYER_INDEX].m_dwLeadExp ;		//统帅力经验值
			//to do: waiting for...;
			pInfo->nLeaderShipExperienceFull = Player[CLIENT_PLAYER_INDEX].m_dwNextLevelLeadExp;//统帅力经验值
			pInfo->nLeaderShipLevel = Player[CLIENT_PLAYER_INDEX].m_dwLeadLevel ;			//统帅力等级
		}
		break;
	//获得物品在某个环境位置的属性状态
	//uParam = (KUiGameObject*)pObj（当nParam==0时）物品的信息
	//uParam = (KUiObjAtContRegion*)pObj（当nParam!=0时）物品的信息
	//nParam = (int)(bool)bJustTry  是否只是尝试放置
	//Return = (ITEM_IN_ENVIRO_PROP)eProp 物品的属性状态
	case GDI_ITEM_IN_ENVIRO_PROP:
		{
			if (!nParam)
			{
				KUiGameObject *pObj = (KUiGameObject *)uParam;
				if (pObj->uGenre != CGOG_ITEM && pObj->uGenre != CGOG_NPCSELLITEM)
					break;

				KItem* pItem = NULL;

				if (pObj->uGenre == CGOG_ITEM && pObj->uId > 0 && pObj->uId < MAX_ITEM)
				{
					pItem = &Item[pObj->uId];
				}
				else if (pObj->uGenre == CGOG_NPCSELLITEM)
				{
					int nIdx = BuySell.GetItemIndex(Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nBuyIdx, pObj->uId);
					pItem = BuySell.GetItemidx(nIdx);
				}

				//_ASSERT(pItem);
				if (!pItem || pItem->GetGenre() != item_equip)
					break;

				if (Player[CLIENT_PLAYER_INDEX].m_ItemList.CanEquip(pItem))
				{
					nRet = IIEP_NORMAL;
				}
				else
				{
					nRet = IIEP_NOT_USEABLE;
				}
			}
			else
			{
				KUiObjAtContRegion *pObj = (KUiObjAtContRegion *)uParam;
				if (pObj->Obj.uGenre != CGOG_ITEM || pObj->Obj.uId >= MAX_ITEM)
					break;

				int PartConvert[itempart_num] = 
				{ 
					itempart_head,		itempart_weapon,
					itempart_amulet,	itempart_cuff,
					itempart_body,		itempart_belt,
					itempart_ring1,		itempart_ring2,
					itempart_pendant,	itempart_foot,
					itempart_horse,		itempart_mask,//   面具
					itempart_pifeng,itempart_yinjian,itempart_shiping	//披风				
				};

				//_ASSERT(pObj->eContainer < itempart_num);

				if (pObj->eContainer >= itempart_num || pObj->eContainer < 0)
					break;

				if (Item[pObj->Obj.uId].GetGenre() != item_equip)
					break;

				int nPlace = PartConvert[pObj->eContainer];

				if (Player[CLIENT_PLAYER_INDEX].m_ItemList.CanEquip(pObj->Obj.uId, nPlace))
				{
					nRet = IIEP_NORMAL;
				}
				else
				{
					nRet = IIEP_NOT_USEABLE;
				}
			}
		}
		break;
	case GDI_IMMEDIATEITEM_NUM:  //获取快捷面板的物品在总叠加数量
		if (uParam >= 0 && uParam < 9)
			nRet = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetSameDetailItemNum(uParam);
		break;
	//与NPC的买卖
	//uParam = (KUiObjAtContRegion*) pInfo -> KUiObjAtRegion结构数组的指针，KUiObjAtRegion
	//				结构用于存储物品的数据及其放置区域位置信息。
	//				其中KUiObjAtContRegion::nContainer值表示第几页的物品
	//nParam = pInfo数组中包含KUiObjAtRegion结构的数目
	//Return = 如果返回值小于等于传入参数nParam，其值表示pInfo数组中的前多少个KUiObjAtRegion
	//			结构被填充了有效的数据；否则表示需要传入包含多少个KUiObjAtRegion结构的数组
	//			才够存储全部的npc列出来交易的物品信息。
	case GDI_TRADE_NPC_ITEM:    //更新NPC商店商品
		nRet = 0;
		if (uParam)
		{
			int nCount = 0,nPage = 0,nIndex = 0;
			int	nBuyIdx = Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nBuyIdx;   //买卖id   就是打开哪个商店的商品
			
			KUiObjAtContRegion* pInfo = (KUiObjAtContRegion *)uParam;

			if (nBuyIdx <0)
				break;
			if (nBuyIdx >= BuySell.GetHeight())  //大于商店行数 就退出
				break;
			if (!BuySell.m_pShopRoom)
				break;

			BuySell.m_pShopRoom->Clear();

			for (int i = 0; i < BuySell.GetWidth(); i++)
			{

				if  (nCount>nParam)
				{
					nCount = nParam; 
					break;
				}

				nIndex = BuySell.GetItemIndex(nBuyIdx, i);

				KItem* pItem = BuySell.GetItemidx(nIndex);
				
				if (nIndex >= 0 && pItem)
				{
					// Set pInfo->Obj.uGenre
					pInfo->Obj.uGenre = CGOG_NPCSELLITEM;   //赋值给NPC商店
					// Set pInfo->Obj.uId
					pInfo->Obj.uId = i;

					POINT	Pos;
					if (BuySell.m_pShopRoom->FindRoom(pItem->GetWidth(), pItem->GetHeight(), &Pos))
					{
						// nIndex + 1保证不为0
						BuySell.m_pShopRoom->PlaceItem(Pos.x, Pos.y, nIndex+1, pItem->GetWidth(), pItem->GetHeight());
					}
					else
					{
						nPage++;
						BuySell.m_pShopRoom->Clear();
						// Clear完成后必然成功，所以没有判断
						if (BuySell.m_pShopRoom->FindRoom(pItem->GetWidth(), pItem->GetHeight(), &Pos))
						   BuySell.m_pShopRoom->PlaceItem(Pos.x, Pos.y, nIndex+1, pItem->GetWidth(), pItem->GetHeight());
					}

					pInfo->Region.h      = Pos.x;
					pInfo->Region.v      = Pos.y;
					pInfo->Region.Width  = pItem->GetWidth();
					pInfo->Region.Height = pItem->GetHeight();
					pInfo->nContainer    = nPage;
					nCount++;
					pInfo++;
				}
			}			
			nRet = nCount;
		}
		else
		{
			int nCount = 0;
			int nIndex = 0;
			int	nBuyIdx = -1;
			    if (inParam)
			     nBuyIdx=Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nBuyIdx=nParam;
				else
				 nBuyIdx=Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nBuyIdx;

			if (nBuyIdx == -1)
				break;
			if (nBuyIdx >= BuySell.GetHeight())
				break;
			for (int i = 0; i < BuySell.GetWidth(); i++)
			{
				nIndex = BuySell.GetItemIndex(nBuyIdx, i);
				KItem* pItem = BuySell.GetItemidx(nIndex);
				
				if (nIndex >= 0 && pItem)
				{
					nCount++;
				}
			}
			nRet = nCount;
		}
		break;
	case GDI_CHAT_SEND_CHANNEL_LIST:
		nRet = Player[CLIENT_PLAYER_INDEX].m_cChat.SendSelectChannelList((KUiChatChannel*)uParam, nParam);
		break;
	case GDI_CHAT_RECEIVE_CHANNEL_LIST:
		nRet = Player[CLIENT_PLAYER_INDEX].m_cChat.SendTakeChannelList((KUiChatChannel*)uParam, nParam);
		break;
	case GDI_CHAT_CURRENT_SEND_CHANNEL:
		nRet = Player[CLIENT_PLAYER_INDEX].m_cChat.GetCurChannel((KUiChatChannel*)uParam);
		break;
	case GDI_CHAT_GROUP_INFO:
		nRet = Player[CLIENT_PLAYER_INDEX].m_cChat.IGetTeamInfo(nParam, (KUiChatGroupInfo*)uParam);
		break;
	//聊天一个好友分组中好友的信息
	//uParam = (KUiPlayerItem*)pList -> 人员信息列表
	//			KUiPlayerItem::nData = (CHAT_STATUS)eFriendStatus 好友的当前状态
	//nParam = 要获取列表的好友分组的索引
	//Return = 其值表示pList数组中的前多少个KUiPlayerItem结构被填充了有效的数据.
	case GDI_CHAT_FRIENDS_IN_AGROUP:
		nRet = Player[CLIENT_PLAYER_INDEX].m_cChat.IGetTeamFriendInfo(nParam, (KUiPlayerItem*)uParam);
		break;
	case GDI_ITEM_IN_STORE_BOX:
		nRet = 0;
		if (uParam)
		{
			int nCount = 0;
			KUiObjAtRegion* pInfo = (KUiObjAtRegion*)uParam;
			pInfo->Obj.uGenre = CGOG_MONEY;
			pInfo->Obj.uId = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetsMon(room_repository);
			nCount++;
			pInfo++;
			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if (pItem && pItem->nPlace == pos_repositoryroom)
			{
				pInfo->Obj.uGenre = CGOG_ITEM;
				pInfo->Obj.uId = pItem->nIdx;
				pInfo->Region.h = pItem->nX;
				pInfo->Region.v = pItem->nY;
				pInfo->Region.Width = Item[pItem->nIdx].GetWidth();
				pInfo->Region.Height = Item[pItem->nIdx].GetHeight();
				nCount++;
				pInfo++;
			}
			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem && pItem->nPlace == pos_repositoryroom)
				{
					pInfo->Obj.uGenre = CGOG_ITEM;
					pInfo->Obj.uId = pItem->nIdx;		
					pInfo->Region.h = pItem->nX;
					pInfo->Region.v = pItem->nY;
					pInfo->Region.Width = Item[pItem->nIdx].GetWidth();
					pInfo->Region.Height = Item[pItem->nIdx].GetHeight();
					nCount++;
					pInfo++;
				}
				if (nCount > nParam)
					break;
			}
			nRet = nCount;
		}
		else
		{
			int nCount = 0;
			// 第一个是钱
			nCount++;
			// 后面是装备
			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if (pItem && pItem->nPlace == pos_repositoryroom)
				nCount++;
			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem && pItem->nPlace == pos_repositoryroom)
					nCount++;
			}
			nRet = nCount;
		}
		break;
	case GDI_ITEM_IN_EX_BOX1:	// mo rong ruong 1
		nRet = 0;
		if (uParam)
		{
			int nCount = 0;
			KUiObjAtRegion* pInfo = (KUiObjAtRegion*)uParam;
			pInfo->Obj.uGenre = CGOG_MONEY;
			pInfo->Obj.uId = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetsMon(room_exbox1);
			nCount++;
			pInfo++;
			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if (pItem && pItem->nPlace == pos_exbox1room)
			{
				pInfo->Obj.uGenre = CGOG_ITEM;
				pInfo->Obj.uId = pItem->nIdx;
				pInfo->Region.h = pItem->nX;
				pInfo->Region.v = pItem->nY;
				pInfo->Region.Width = Item[pItem->nIdx].GetWidth();
				pInfo->Region.Height = Item[pItem->nIdx].GetHeight();
				nCount++;
				pInfo++;
			}
			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem && pItem->nPlace == pos_exbox1room)
				{
					pInfo->Obj.uGenre = CGOG_ITEM;
					pInfo->Obj.uId = pItem->nIdx;		
					pInfo->Region.h = pItem->nX;
					pInfo->Region.v = pItem->nY;
					pInfo->Region.Width = Item[pItem->nIdx].GetWidth();
					pInfo->Region.Height = Item[pItem->nIdx].GetHeight();
					nCount++;
					pInfo++;
				}
				if (nCount > nParam)
					break;
			}
			nRet = nCount;
		}
		else
		{
			int nCount = 0;
			// 第一个是钱
			nCount++;
			// 后面是装备
			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if (pItem && pItem->nPlace == pos_exbox1room)
				nCount++;
			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem && pItem->nPlace == pos_exbox1room)
					nCount++;
			}
			nRet = nCount;
		}
		break;
	case GDI_ITEM_IN_EX_BOX2:	// mo rong ruong 2
		nRet = 0;
		if (uParam)
		{
			int nCount = 0;
			KUiObjAtRegion* pInfo = (KUiObjAtRegion*)uParam;
			pInfo->Obj.uGenre = CGOG_MONEY;
			pInfo->Obj.uId = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetsMon(room_exbox2);
			nCount++;
			pInfo++;
			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if (pItem && pItem->nPlace == pos_exbox2room)
			{
				pInfo->Obj.uGenre = CGOG_ITEM;
				pInfo->Obj.uId = pItem->nIdx;
				pInfo->Region.h = pItem->nX;
				pInfo->Region.v = pItem->nY;
				pInfo->Region.Width = Item[pItem->nIdx].GetWidth();
				pInfo->Region.Height = Item[pItem->nIdx].GetHeight();
				nCount++;
				pInfo++;
			}
			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem && pItem->nPlace == pos_exbox2room)
				{
					pInfo->Obj.uGenre = CGOG_ITEM;
					pInfo->Obj.uId = pItem->nIdx;		
					pInfo->Region.h = pItem->nX;
					pInfo->Region.v = pItem->nY;
					pInfo->Region.Width = Item[pItem->nIdx].GetWidth();
					pInfo->Region.Height = Item[pItem->nIdx].GetHeight();
					nCount++;
					pInfo++;
				}
				if (nCount > nParam)
					break;
			}
			nRet = nCount;
		}
		else
		{
			int nCount = 0;
			// 第一个是钱
			nCount++;
			// 后面是装备
			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if (pItem && pItem->nPlace == pos_exbox2room)
				nCount++;
			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem && pItem->nPlace == pos_exbox2room)
					nCount++;
			}
			nRet = nCount;
		}
		break;
	case GDI_ITEM_IN_EX_BOX3:	// mo rong ruong 3
		nRet = 0;
		if (uParam)
		{
			int nCount = 0;
			KUiObjAtRegion* pInfo = (KUiObjAtRegion*)uParam;
			pInfo->Obj.uGenre = CGOG_MONEY;
			pInfo->Obj.uId = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetsMon(room_exbox3);
			nCount++;
			pInfo++;
			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if (pItem && pItem->nPlace == pos_exbox3room)
			{
				pInfo->Obj.uGenre = CGOG_ITEM;
				pInfo->Obj.uId = pItem->nIdx;
				pInfo->Region.h = pItem->nX;
				pInfo->Region.v = pItem->nY;
				pInfo->Region.Width = Item[pItem->nIdx].GetWidth();
				pInfo->Region.Height = Item[pItem->nIdx].GetHeight();
				nCount++;
				pInfo++;
			}
			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem && pItem->nPlace == pos_exbox3room)
				{
					pInfo->Obj.uGenre = CGOG_ITEM;
					pInfo->Obj.uId = pItem->nIdx;		
					pInfo->Region.h = pItem->nX;
					pInfo->Region.v = pItem->nY;
					pInfo->Region.Width = Item[pItem->nIdx].GetWidth();
					pInfo->Region.Height = Item[pItem->nIdx].GetHeight();
					nCount++;
					pInfo++;
				}
				if (nCount > nParam)
					break;
			}
			nRet = nCount;
		}
		else
		{
			int nCount = 0;
			// 第一个是钱
			nCount++;
			// 后面是装备
			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if (pItem && pItem->nPlace == pos_exbox3room)
				nCount++;
			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem && pItem->nPlace == pos_exbox3room)
					nCount++;
			}
			nRet = nCount;
		}
		break;
	case GDI_ITEM_EX:	// 同伴包袱
		nRet = 0;
		if (uParam)
		{
			int nCount = 0;
			KUiObjAtRegion* pInfo = (KUiObjAtRegion*)uParam;
			pInfo->Obj.uGenre = CGOG_MONEY;
			pInfo->Obj.uId = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetsMon(room_equipmentex);
			nCount++;
			pInfo++;
			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if (pItem && pItem->nPlace == pos_equiproomex)
			{
				pInfo->Obj.uGenre = CGOG_ITEM;
				pInfo->Obj.uId = pItem->nIdx;
				pInfo->Region.h = pItem->nX;
				pInfo->Region.v = pItem->nY;
				pInfo->Region.Width = Item[pItem->nIdx].GetWidth();
				pInfo->Region.Height = Item[pItem->nIdx].GetHeight();
				nCount++;
				pInfo++;
			}
			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem && pItem->nPlace == pos_equiproomex)
				{
					pInfo->Obj.uGenre = CGOG_ITEM;
					pInfo->Obj.uId = pItem->nIdx;		
					pInfo->Region.h = pItem->nX;
					pInfo->Region.v = pItem->nY;
					pInfo->Region.Width = Item[pItem->nIdx].GetWidth();
					pInfo->Region.Height = Item[pItem->nIdx].GetHeight();
					nCount++;
					pInfo++;
				}
				if (nCount > nParam)
					break;
			}
			nRet = nCount;
		}
		else
		{
			int nCount = 0;
			// 第一个是钱
			nCount++;
			// 后面是装备
			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if (pItem && pItem->nPlace == pos_equiproomex)
				nCount++;
			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem && pItem->nPlace == pos_equiproomex)
					nCount++;
			}
			nRet = nCount;
		}
		break;
	case GDI_PK_SETTING:					//获取pk设置
		nRet = Player[CLIENT_PLAYER_INDEX].m_cPK.GetNormalPKState();
		break;
	case GDI_SHOW_PLAYERS_NAME:			//获取显示各玩家人名
		nRet = NpcSet.CheckShowName();
		break;
	case GDI_SHOW_PLAYERS_LIFE:			//获取显示各玩家生命
		nRet = NpcSet.CheckShowLife();
		break;
	case GDI_SHOW_PLAYERS_MANA:			//获取显示各玩家内力
		nRet = NpcSet.CheckShowMana();
		break;
	case GDI_IS_CHEST_UNLOCKED:			//检测是否已经锁屏
		nRet = Player[CLIENT_PLAYER_INDEX].m_CUnlocked;
		break;
	case GDI_EXBOX_ID: 				// truyen id mo rong ruong
		{	
			int nExBoxId = 0;
			if (nExBoxId < 0)
			{
				nRet= 0;
				break;
			}
			 nExBoxId = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_ExBoxId;
		     nRet= nExBoxId;
			break;
		}
	case GDI_SELL_ITEM_NUM:
		{ 
			if(uParam)
			{
				KUiObjAtContRegion *pObj = (KUiObjAtContRegion *)uParam;
				int nNo = 0;
				int nIdx = pObj->Obj.uId;
				int nGenre = Item[nIdx].GetGenre();
				int nDetail = Item[nIdx].GetDetailType();
				int nParticular = Item[nIdx].GetParticular();
				int nLevel = Item[nIdx].GetLevel();
				int nX = 0, nY = 0;
				return Player[CLIENT_PLAYER_INDEX].m_ItemList.GetItemNum(nGenre, nDetail, nParticular, nLevel);
			}
			break;
		}
	case GDI_PLAYER_TK_TIME:	
		nRet = Player[CLIENT_PLAYER_INDEX].m_cTask.GetSaveVal(TASKVALUE_TIME);
		break;
	case GDI_PLAYER_TK_POINT:	
		nRet = Player[CLIENT_PLAYER_INDEX].m_cTask.GetSaveVal(TASKVALUE_DIEM);
		break;
	case GDI_PLAYER_TK_PLAYER:	
		nRet = Player[CLIENT_PLAYER_INDEX].m_cTask.GetSaveVal(TASKVALUE_PLAYER);
		break;	
	case GDI_PLAYER_TK_NPC:	
		nRet = Player[CLIENT_PLAYER_INDEX].m_cTask.GetSaveVal(TASKVALUE_NPC);
		break;	
	case GDI_PLAYER_TK_XEPHANG:	
		nRet = Player[CLIENT_PLAYER_INDEX].m_cTask.GetSaveVal(TASKVALUE_XEPHANG);
		break;	
	}

	return nRet;
}

//--------------------------------------------------------------------------
//	功能：向游戏发送操作
//	参数：unsigned int uDataId --> Core外部客户对core的操作请求的索引定义
//							其值为梅举类型GAMEOPERATION_INDEX的取值之一。
//		  unsigned int uParam  --> 依据uOperId的取值情况而定
//		  int nParam --> 依据uOperId的取值情况而定
//	返回：如果成功发送操作请求，函数返回非0值，否则返回0值。
//--------------------------------------------------------------------------
int	KCoreShell::OperationRequest(unsigned int uOper, unsigned int uParam, int nParam,int nIsMianBan,char* strVal)
{
	int nRet = 1;

	switch(uOper)
	{
	case GOI_CP_UNLOCK: //解锁
		SendClientCPUnlockCmd(uParam);
		break;
    case GOI_MAP_GETSIZE:
	    g_ScenePlace.GetMapSizePosInfo(uParam);
	    break;   
	case GOI_MAP_CHAQI:
		{
	      if (uParam)
		  {  
             KChaQIMapPos* pInfo = (KChaQIMapPos*)uParam;
             Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_mMapX=pInfo->nCursorX;
	         Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_mMapY=pInfo->nCursorY;
		  }
	      else
		  { 
		     if (nParam)
			 {
		       Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_BtnFlag=1;
			   Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_BtnFindPath	=TRUE;
			 }
		     else
			 {
               Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_BtnFlag=0;
			   Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_BtnFindPath	=FALSE;
			   g_ScenePlace.DelGreenLine();
			 }
		  } 
		}
		break;
	case GOI_CP_LOCK: //解锁屏幕
		SendClientCPLockCmd();
		break;
	case GOI_CP_CHANGE://解锁
		//SendClientCPChangeCmd(uParam, nParam);
		break;
	case GOI_CP_RESET://重置锁频密码
		//SendClientCPResetCmd(nParam);
		break;
	case GOI_SYN_SHOPIDX:
		SendClientOpenMarket(nParam,uParam);
		break;
	case GOI_MARKET:// KTC商城分页专用的
		{
				if(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_FightMode == 0)  //非PK模式才能打开商店
				{
					//if (nParam>-1)
					//	SendClientOpenMarket(nParam); //打开商城分页
					//else
						break;
				}
				else
				{
					    nRet = 0;
						KSystemMessage	Msg;
						Msg.byConfirmType = SMCT_NONE;
						Msg.eType = SMT_NORMAL;
						Msg.byPriority = 1;
						Msg.byParamSize = 0;
						snprintf(Msg.szMessage, sizeof(Msg.szMessage), "%s", "提示:<color=yellow>商城需要在安全区才可以打开<color>!");
						Msg.nMsgLen=TEncodeText(Msg.szMessage, strlen(Msg.szMessage));
						CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&Msg, 0);
				}
				break;
	}
	break;
	case GOI_GUZI_INFO:// 股子
		{   
//			KUiGuZiXinxi* pInfo = (KUiGuZiInfo*)uParam;

	      int nMapID,nX,nY,nShuiShou, nNetIndexid,nIsTong=0;
	      char nMapName[64],nType[32];
	          g_ScenePlace.GetSceneNameAndFocus(nMapName, nMapID,nX, nY,nType); //得到场的格子坐标
		      nNetIndexid=Player[CLIENT_PLAYER_INDEX].m_nNetConnectIdx;
		  SendClientGuZi(nMapID,nParam,nNetIndexid,uParam);  //发送命令执行脚本  地图号 和 点数  座位号 //开始标示
		}
		break;
	case GOI_MAP_SUNYI:// 瞬移
		{		
			int nMapID,nX,nY;
			char nMapName[64]={0},nType[32]={0};
			if (nParam==0)
			{//BB
			     g_ScenePlace.GetSceneNameAndFocus(nMapName, nMapID,nX, nY,nType); //得到当前场景的格子坐标
		         int nNpcID= NpcSet.SearchGSName(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name);
			     if (nNpcID>0)
				 {  
			        SendClientCmdSunyi(nMapID,nX*32,nY*64,Npc[nNpcID].m_dwID);  //发送瞬移命令	
				 } 
			}
			else if (nParam==1)
			{//点击地图瞬移动 或 执行脚本
				  KIniFile nMapPos;
				  if (nMapPos.Load("\\ui\\小地图_山洞地图版_透明按钮.ini"))
				  {	//SubwordId
					  sprintf(nMapName,"BUTTON_%d",uParam);
					  nMapPos.GetInteger(nMapName,"SubwordId",0,&nMapID);
					  //nMapPos.GetInteger(nMapName,"TargetPosX",0,&nX);
					  //nMapPos.GetInteger(nMapName,"TargetPosY",0,&nY);
					  if (nMapID>0)
						  SendClientCmdSysShop(4,nMapID);   //执行脚本
					     //SendClientCmdSunyi(nMapID,nX*8*32,nY*16*32,0,1);*/
				  }
				  
				  nMapPos.Clear();

			}
		}
		break;
	case GOI_GUZI_CURFARMS:// 股子
		{ 
		  KUiGuZiCurFarms* pInfo = (KUiGuZiCurFarms*)uParam;
	      int nMapID,nX,nY,nShuiShou, nNetIndexid,nIsTong=0;
	      char nMapName[64],nType[32];
	      g_ScenePlace.GetSceneNameAndFocus(nMapName, nMapID,nX, nY,nType); //得到场的格子坐标
		  //nNetIndexid=Player[CLIENT_PLAYER_INDEX].m_nNetConnectIdx;
		  SendClientGuZiFarms(nMapID,pInfo->nCurFarmsA,pInfo->nCurFarmsB);  //发送命令执行脚本  地图号 和 点数  座位号 //开始标示
		} 
		break;
	case GOI_GUZI_JIESUAN:// 结算
	{
		KUiGuZiJieSuan* pInfo = (KUiGuZiJieSuan*)uParam;
		int  nZongJB=0;
		char nMsg[125];
		//	Player[CLIENT_PLAYER_INDEX].m_ItemList.AddXu(room_equipment,nParam); //增加金币//Player[nPlayerIndex].EarnXu(nXu);nPlayerName			
	if (nParam==1)
	{
		if  (strstr(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name,pInfo->nPlayerNameA))
		{
			nZongJB=pInfo->nGRjinbi[0]+pInfo->nKFmoney;
			SendClientAddJinBi(nZongJB);
			snprintf(nMsg, sizeof(nMsg), "提示:本次最大点数:<color=green>(%d)<color>,收入奖金:(%d=本金%d+赢%d)!",pInfo->nMaxDian,nZongJB,pInfo->nGRjinbi[0],pInfo->nKFmoney);
		    Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(nMsg); 
		}
	}
	else if (nParam==2)
	{
		if  (strstr(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name,pInfo->nPlayerNameB))
		{
			nZongJB=pInfo->nGRjinbi[1]+pInfo->nKFmoney;
			SendClientAddJinBi(nZongJB);

			snprintf(nMsg, sizeof(nMsg), "提示:本次最大点数:<color=green>(%d)<color>,收入奖金:(%d=本金%d+赢%d)!",pInfo->nMaxDian,nZongJB,pInfo->nGRjinbi[1],pInfo->nKFmoney);
	         Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(nMsg);
		}
	}
	else if (nParam==3)
	{
		if  (strstr(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name,pInfo->nPlayerNameC))
		{
			nZongJB=pInfo->nGRjinbi[2]+pInfo->nKFmoney;
			SendClientAddJinBi(nZongJB);

			snprintf(nMsg, sizeof(nMsg), "提示:本次最大点数:<color=green>(%d)<color>,收入奖金:(%d=本金%d+赢%d)!",pInfo->nMaxDian,nZongJB,pInfo->nGRjinbi[2],pInfo->nKFmoney);
             Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(nMsg);
		}
	}
	else if (nParam==4)
	{
		if  (strstr(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name,pInfo->nPlayerNameD))
		{
			nZongJB=pInfo->nGRjinbi[3]+pInfo->nKFmoney;
			SendClientAddJinBi(nZongJB);

			snprintf(nMsg, sizeof(nMsg), "提示:本次最大点数:<color=green>(%d)<color>,收入奖金:(%d=本金%d+赢%d)!",pInfo->nMaxDian,nZongJB,pInfo->nGRjinbi[3],pInfo->nKFmoney);
			Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(nMsg);
		}
	}
	else if (nParam==5)
	{
		if  (strstr(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name,pInfo->nPlayerNameE))
		{
			nZongJB=pInfo->nGRjinbi[4]+pInfo->nKFmoney;
			SendClientAddJinBi(nZongJB);

			snprintf(nMsg, sizeof(nMsg), "提示:本次最大点数:<color=green>(%d)<color>,收入奖金:(%d=本金%d+赢%d)!",pInfo->nMaxDian,nZongJB,pInfo->nGRjinbi[4],pInfo->nKFmoney);
            Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(nMsg);
		}
	}
	else if (nParam==6)
	{
		if  (strstr(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name,pInfo->nPlayerNameA))
		{
			sprintf(nMsg, "提示:本次您甩了(%d)点,输:<color=green>(%d)<color>金币!",pInfo->nGRdian[0],pInfo->nGRjinbi[0]);
            Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(nMsg);
		}
	}	
	else if (nParam==7)
	{
		if  (strstr(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name,pInfo->nPlayerNameB))
		{
			sprintf(nMsg, "提示:本次您甩了(%d)点,输:<color=green>(%d)<color>金币!",pInfo->nGRdian[1],pInfo->nGRjinbi[1]);
            Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(nMsg);
		}
	}
	else if (nParam==8)
	{
		if  (strstr(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name,pInfo->nPlayerNameC))
		{
			sprintf(nMsg, "提示:本次您甩了(%d)点,输:<color=green>(%d)<color>金币!",pInfo->nGRdian[2],pInfo->nGRjinbi[2]);
            Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(nMsg);
		}
	}
	else if (nParam==9)
	{
		if  (strstr(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name,pInfo->nPlayerNameD))
		{
			sprintf(nMsg, "提示:本次您甩了(%d)点,输:<color=green>(%d)<color>金币!",pInfo->nGRdian[3],pInfo->nGRjinbi[3]);
            Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(nMsg);
		}
	}
	else if (nParam==10)
	{
		if  (strstr(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name,pInfo->nPlayerNameE))
		{
			sprintf(nMsg, "提示:本次您甩了(%d)点,输:<color=green>(%d)<color>金币!",pInfo->nGRdian[4],pInfo->nGRjinbi[4]);
            Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(nMsg);
		}
	}
   }
   break;
	case GOI_GUZI:// 股子
		{
			/*
		KUiGuZiInfo* pInfo = (KUiGuZiInfo*)uParam;  
		int nPlyerid     =Player[CLIENT_PLAYER_INDEX].GetPlayerIndex();

         pInfo->nmodel   =SubWorld[0].m_SubGuModel;//模式

		 pInfo->nCJcishuA=SubWorld[0].m_DianshuA;  //点数
		 pInfo->nCJcishuB=SubWorld[0].m_DianshuB;  //点数
		 pInfo->nCJcishuC=SubWorld[0].m_DianshuC;  //点数
         pInfo->nCJcishuD=SubWorld[0].m_DianshuD;  //点数
		 pInfo->nCJcishuE=SubWorld[0].m_DianshuE;  //点数

		 pInfo->nGyajinA =SubWorld[0].m_GyajingA;   //押金是多少
		 pInfo->nGyajinB =SubWorld[0].m_GyajingB;   //押金是多少
		 pInfo->nGyajinC =SubWorld[0].m_GyajingC;   //押金是多少
		 pInfo->nGyajinD =SubWorld[0].m_GyajingD;   //押金是多少
		 pInfo->nGyajinE =SubWorld[0].m_GyajingE;   //押金是多少

         sprintf(pInfo->nPlayerNameA,"%s",SubWorld[0].m_GuzinameA);
		 sprintf(pInfo->nPlayerNameB,"%s",SubWorld[0].m_GuzinameB);
		 sprintf(pInfo->nPlayerNameC,"%s",SubWorld[0].m_GuzinameC);
		 sprintf(pInfo->nPlayerNameD,"%s",SubWorld[0].m_GuzinameD);
		 sprintf(pInfo->nPlayerNameE,"%s",SubWorld[0].m_GuzinameE);

		 pInfo->nIsStartA=SubWorld[0].m_IsStartA;  //玩家的开始的标示
		 pInfo->nIsStartB=SubWorld[0].m_IsStartB;
		 pInfo->nIsStartC=SubWorld[0].m_IsStartC;
		 pInfo->nIsStartD=SubWorld[0].m_IsStartD;
		 pInfo->nIsStartE=SubWorld[0].m_IsStartE;

		 pInfo->nIsXiaZhuA=SubWorld[0].m_IsXiaZhuA;//1号是否下注了
		 pInfo->nIsXiaZhuB=SubWorld[0].m_IsXiaZhuB;
		 pInfo->nIsXiaZhuC=SubWorld[0].m_IsXiaZhuC;
		 pInfo->nIsXiaZhuD=SubWorld[0].m_IsXiaZhuD;
		 pInfo->nIsXiaZhuE=SubWorld[0].m_IsXiaZhuE;

		 pInfo->nCurFarmA=SubWorld[0].m_CurFarmA;
		 pInfo->nCurFarmB=SubWorld[0].m_CurFarmB;

		 pInfo->nSubZuoWeiHao=SubWorld[0].m_ZuoWeiHao;  //地图的座位号 该谁甩股了

	//	 KSystemMessage	sMsg;		 
	   if  (strstr(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name,SubWorld[0].m_GuzinameA))
	   {
		   //pInfo->nCJcishuA=Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_GuziDianshu; //点数是多少
		   pInfo->nIsVip=Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_IsVip;
		   pInfo->nIsShuai=Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_IsShuai;//是否已经甩了

           pInfo->nZuoweiHao =1;
		   pInfo->nIsMenber  =1;
		 //sprintf(sMsg.szMessage, "提示:(%s),您本次押金为<color=green>(%d)<color>金币,座位:(%d)号!","亲",pInfo->nGyajinA,pInfo->nZuoweiHao);
	   }
	   else  if  (strstr(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name,SubWorld[0].m_GuzinameB))
	   {
		   //pInfo->nCJcishuB=Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_GuziDianshu; //点数是多少
		   pInfo->nIsVip=Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_IsVip;
		   pInfo->nIsShuai=Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_IsShuai;//是否已经甩了
           pInfo->nZuoweiHao =2;
		   pInfo->nIsMenber  =1;
		//   sprintf(sMsg.szMessage, "提示:(%s),您本次押金为<color=green>(%d)<color>金币,座位:(%d)号!","亲",pInfo->nGyajinB,pInfo->nZuoweiHao);
	   }
	   else  if  (strstr(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name,SubWorld[0].m_GuzinameC))
	   {
		   //pInfo->nCJcishuC=Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_GuziDianshu; //点数是多少
		   pInfo->nIsVip=Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_IsVip;
		   pInfo->nIsShuai=Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_IsShuai;//是否已经甩了
           pInfo->nZuoweiHao =3;
		   pInfo->nIsMenber  =1;
		 //  sprintf(sMsg.szMessage, "提示:(%s),您本次押金为<color=green>(%d)<color>金币,座位:(%d)号!","亲",pInfo->nGyajinC,pInfo->nZuoweiHao);
	   }
	   else  if  (strstr(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name,SubWorld[0].m_GuzinameD))
	   { 
		   //pInfo->nCJcishuD=Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_GuziDianshu; //点数是多少
		   pInfo->nIsVip=Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_IsVip;
		   pInfo->nIsShuai=Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_IsShuai;//是否已经甩了
           pInfo->nZuoweiHao =4;
		   pInfo->nIsMenber  =1;
		 //  sprintf(sMsg.szMessage, "提示:(%s),您本次押金为<color=green>(%d)<color>金币,座位:(%d)号!","亲",pInfo->nGyajinD,pInfo->nZuoweiHao);
	   }
	   else  if  (strstr(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name,SubWorld[0].m_GuzinameE))
	   { 
		   //pInfo->nCJcishuE=Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_GuziDianshu; //点数是多少
	       pInfo->nIsVip=Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_IsVip;
		   pInfo->nIsShuai=Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_IsShuai;//是否已经甩了
           pInfo->nZuoweiHao =5;
		   pInfo->nIsMenber  =1;
		 //  sprintf(sMsg.szMessage, "提示:(%s),您本次押金为<color=green>(%d)<color>金币,座位:(%d)号!","亲",pInfo->nGyajinE,pInfo->nZuoweiHao);
	   }
	   else
	   {
           Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_GuziMenber=2;//设置为参观者
		   pInfo->nZuoweiHao =0;
           pInfo->nIsMenber  =2;  //参观者
	//	   sprintf(sMsg.szMessage, "提示:(%s),您本次作为参观者进入(夜店之王),也可以下注哦!","亲");
	   } */   
		}
		break;
	case GOI_AUTOPALYOPEN:                      // 挂机开始
		    SendClientAutoPlay(uParam,nParam);  //发送命令执行脚本
		break;
	case GOI_YING_CANG:                         //隐藏玩家 子弹 怪物
		if (uParam==1)
		{
			if (nParam==1)
                Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_bIsHidePlayer=1;
			else
                Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_bIsHidePlayer=0;
		}
		else if  (uParam==2)
		{
			if (nParam==1)
				Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_bIsHideNpc=1;
			 else
                Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_bIsHideNpc=0;
		}
		else if  (uParam==3)
		{
			if (nParam==1)
				Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_bIsHideMissle=1;
			else
				Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_bIsHideMissle=0;
		}
		else if  (uParam==4)
		{
			if (nParam==1)
				Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_bIsHideLife=1;
			else
				Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_bIsHideLife=0;
		}
		else if  (uParam==5)
		{
			if (nParam==1)
				Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_bIsHideTong=1;
			else
				Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_bIsHideTong=0;
		}
		else if  (uParam==6)
		{
			if (nParam==1)
				Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_bIsHideNuqi=1;
			else
				Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_bIsHideNuqi=0;
		}



		break;
   //m_ItemLinkDwid	=nPitem->cItemID; GOI_ITEM_LINK
	case GOI_ITEM_LINK:// 发送链接信息
		{
		   Player[CLIENT_PLAYER_INDEX].m_ItemLinkDwid=nParam;
		}
		break;
	case GOI_ITEM_LIANJIE:// 发送链接信息
		{
          KUiItemLian* pInfo = (KUiItemLian*)uParam;  
		  //int  m_PlayerIdx=PlayerSet.FindNameID(pInfo->nPlayerName);  //(服务器端用)
		  //int  nIdx = NpcSet.SearchName(pInfo->nPlayerName);          // (客户端用)

           SendClientLianJie(pInfo->nPlayerName,pInfo->nItemDWID);  //发送链接信息
			  //char msg[64];
			  //sprintf(msg,"(%s,ItemDWID=%d)",pInfo->nPlayerName,pInfo->nItemDWID);
			  //Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(msg);
		  
		}		
		break;
	case GOI_JINMAI_ECX:// 经脉
		SendClientCmdJinMai(uParam,nParam);  //发送命令执行脚本
		break;
	case GOI_DATAU:// 老头任务 抽奖
		SendClientDaTau(uParam,-1);
		break;
	case GOI_GUZI_XIAZHU:// 股子下注
		SendClientDaTau(uParam,nParam);
		break;
	case GOI_TRADE_PLAYER_BUY:  //买东西-摆摊  ---直接购买 没有跳出支付窗口
		{
			KUiObjAtContRegion* pObject1 = (KUiObjAtContRegion*)uParam;

			if (CGOG_PLAYERSELLITEM != pObject1->Obj.uGenre)
				break;			
			
			int nWidth, nHeight;
			ItemPos	Pos;
			///////////////////////////////////摆摊税收/////////////////////////////////////////
			int nMapID,nX,nY,nShuiShou,nIsTong=0;
			char nMapName[64]={0},nType[32]={0},nMasterName[32]={0};
            g_ScenePlace.GetSceneNameAndFocus(nMapName, nMapID,nX, nY,nType); //得到场景的格子坐标
//			int nIdx = 0;
//			KItem* pItem = NULL;
			snprintf(nMasterName, sizeof(nMasterName), "%s", SubWorld[0].nWorldMapInfo[STR_MAP_MASTER].c_str());
			if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_IsWarCity>0 && strcmpi(nMasterName,Player[CLIENT_PLAYER_INDEX].m_cTong.m_BMasterName)==0)
			{
				nIsTong=1;
			}   
			else
			{
				nIsTong=0;
			}
            nShuiShou=0;//Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_WarShuishou;
			 ///////////////////////////////////////////////////////////////////////////////
			nWidth  = Item[pObject1->Obj.uId].GetWidth();
			nHeight = Item[pObject1->Obj.uId].GetHeight();

			if (!Player[CLIENT_PLAYER_INDEX].m_ItemList.SearchPosition(nWidth, nHeight, &Pos))
			{
				nRet = 0;
				break;
			}
			if (Pos.nPlace != pos_equiproom)
			{
				nRet = 0;
				
				KSystemMessage	sMsg;			
				snprintf(sMsg.szMessage, sizeof(sMsg.szMessage), "%s", MSG_SHOP_NO_ROOM);
				sMsg.eType = SMT_SYSTEM;
				sMsg.byConfirmType = SMCT_CLICK;
				sMsg.byPriority = 1;
				sMsg.byParamSize = 0;
				sMsg.nMsgLen=TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				break;
			}
			BYTE nModelval=0;

			     nModelval=Item[pObject1->Obj.uId].GetcModel();


			if ((nModelval==0 || nModelval==1) && Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipmentMoney() < Item[pObject1->Obj.uId].GetTradePrice())
			{//默认是银两？
				nRet = 0;
				KSystemMessage	sMsg;				
				sprintf(sMsg.szMessage, MSG_SHOP_NO_MONEY,nShuiShou,"%%");
				sMsg.eType = SMT_SYSTEM;
				sMsg.byConfirmType = SMCT_CLICK;
				sMsg.byPriority = 1;
				sMsg.byParamSize = 0;
				sMsg.nMsgLen=TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				break;
				
			}
			else if (nModelval==2 && Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipmentXu() < Item[pObject1->Obj.uId].GetTradePrice())
			{
                 KSystemMessage	sMsg;				
				snprintf(sMsg.szMessage, sizeof(sMsg.szMessage), "Bo sung <c=green> thue (%d%s)<c>, Khong du tien!",nShuiShou,"%%");
				sMsg.eType = SMT_SYSTEM;
				sMsg.byConfirmType = SMCT_CLICK;
				sMsg.byPriority = 1;
				sMsg.byParamSize = 0;
				sMsg.nMsgLen=TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				break;
			}
				
			nRet = 1;
			SendClientCmdPlayerBuy(g_cSellItem.FindIdx(pObject1->Obj.uId),nParam, pos_equiproom,Pos.nX,Pos.nY,nIsTong,nShuiShou,nMapID,nModelval);
		}
		break;

	case GOI_VIEW_PLAYERSELLITEM_END:  //结束摆摊
		if (nParam==0)
		  g_cSellItem.DeleteAll();
		else
          g_cSellItem.DeleteLAll();
		break;
	case GOI_VIEW_PLAYERSELLITEM:  //看摆摊物品面板
		if (!Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_BaiTan)
			g_cSellItem.ApplyViewItem(uParam);
		break;
	case GDI_PLAYER_TRADE:
		{
			char* sShopName = (char *)uParam;
			
			if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_bRideHorse)
			{
				KSystemMessage	sMsg;
				sprintf(sMsg.szMessage, "Ban khong the ban hang khi dang cuoi ngua!");
				sMsg.eType = SMT_NORMAL;
				sMsg.byConfirmType = SMCT_NONE;
				sMsg.byPriority = 0;
				sMsg.byParamSize = 0;
				sMsg.nMsgLen=TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				nRet = 0;
				break;
			}

			if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_FightMode)
			{
				KSystemMessage	sMsg;
				sprintf(sMsg.szMessage, "Ban khong the ban hang khi trang thai chien dau");
				sMsg.eType = SMT_NORMAL;
				sMsg.byConfirmType = SMCT_NONE;
				sMsg.byPriority = 0;
				sMsg.byParamSize = 0;
				sMsg.nMsgLen=TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				nRet = 0;
				break;
			}
			
			if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Doing != do_sit)
			{
				Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SendSerCommand(do_sit);
				SendClientCmdSit(TRUE);
			}
			
			if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_BaiTan == 0)
				SendClientCmdStartTrade(1,sShopName);      //开始摆摊商城名
			else
			{
				SendClientCmdStartTrade(0,"系统商城");     //关闭摆摊
				nRet=-1;
			}
		}
		break;
	case GDI_SET_TRADE_ITEM:
		{
			KUiObjAtContRegion* pObject1 = (KUiObjAtContRegion*)uParam;
			
			//放下去的东西不为空，所以是卖东西
			int nIdx = pObject1->Obj.uId;	//Player[CLIENT_PLAYER_INDEX].m_ItemList.Hand();

			if (nIdx > 0 && nIdx < MAX_ITEM)
			{
			    if (!Item[nIdx].GetLockItem()->IsLockItem() && Item[nIdx].GetUseMapIdx()<=0)
				{  
				  Item[nIdx].SetTradePrice(nParam);                                 //设置客户端摆摊标价格
				  Item[nIdx].SetModel(nIsMianBan);
				  SendClientCmdSetPrice(Item[nIdx].GetID(),nParam,nIsMianBan); //设置服务器端摆摊标价
				}
				else
				{
				   Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("Khong the giao dich cac vat pham dinh");
				}
			}
		}
		break;
	case GOI_QUERY_RANK_INFORMATION:  //获取排名数据
		SendClientCmdQueryLadder(uParam);
		break;
	//uParam = (const char*)pszFileName
	case GOI_PLAY_SOUND:
		if (uParam)
		{
			static KCacheNode* pSndNode = NULL;
			KWavSound* pSound = NULL;
			pSndNode	= (KCacheNode*)g_SoundCache.GetNode((char *)uParam, (KCacheNode * )pSndNode);
			pSound		= (KWavSound*)pSndNode->m_lpData;
			if (pSound)
			{
				if (pSound->IsPlaying())
					break;
				pSound->Play(0, -10000 + Option.GetSndVolume() * 100, 0);
			}
		}
		break;
	case GOI_PLAYER_RENASCENCE:  //重生选择
		{
//			int nReviveType;
			if (nParam>=0)	// bBackTown
			{
				SendClientCmdRevive(nParam);
			}
			
		}
		break;
	case GOI_PLAYER_GUAFORBIT:  //外挂提示
		{
//			int nReviveType;
			if (nParam>=0)	// bBackTown
			{
				SendClientCmdGua(nParam);
			}
		}
		break;
	case GOI_MONEY_INOUT_STORE_BOX:
		{
			//srand(clock());
			//srand((unsigned)time(NULL));
			//CHAR *szRandomMem1, *szRandomMem2, *szRandomMem3;
			
			//szRandomMem1 = new CHAR[((rand()%64) + 6) * 1024];
			
			//szRandomMem2 = new CHAR[((rand()%64) + 6) * 1024];

			BOOL	bIn = (BOOL)uParam;
			int		nMoney = nParam;
			int		nSrcRoom, nDesRoom;

			if (nMoney<0)
			{
				/*if (szRandomMem1)
				{
					delete[] szRandomMem1;
					szRandomMem1 = NULL;
				}
				
				if (szRandomMem2)
				{
					delete[] szRandomMem2;
					szRandomMem2 = NULL;
				} */
				break;
			}

			if (bIn)
			{//存钱
				nSrcRoom = room_equipment;
				nDesRoom = room_repository;

			  if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_AutoplayId)
			  {//挂机中
				  Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("<color=yellow>助手<color>:存钱成功!");
			  } 
			}
			else
			{//取钱
				nDesRoom = room_equipment;
				nSrcRoom = room_repository;
			}
					  
			Player[CLIENT_PLAYER_INDEX].m_ItemList.ExchangeMoney(nSrcRoom, nDesRoom, nMoney);

			//szRandomMem3 = new CHAR[((rand()% 64) + 6) * 1024];

			/*if (szRandomMem1)
			{
				delete[] szRandomMem1;
				szRandomMem1 = NULL;
			}

			if (szRandomMem2)
			{
			    delete[] szRandomMem2;
			    szRandomMem2 = NULL;
			}

	        if (szRandomMem3)
			{
				delete[] szRandomMem3;
				szRandomMem3 = NULL;
			} */
		}
		break;

	case GOI_STRINGBOX:// 字符串输入界面
		{

         PLAYER_STRINGUI* psui = (PLAYER_STRINGUI*)uParam;

		 SendClientString(psui->m_Content,psui->m_SCallback,psui->m_SError,nParam);

		}
		
		break;
	case GOI_GIVE_ITEM_BACK:  //开始发送数据给服务器
		{
			PLAYER_GIVEUI* pcui = (PLAYER_GIVEUI*)uParam;
			int npid;
            npid=(int)(pcui->uId);
			SendClientCmdGive(npid,nParam,1,pcui->m_Callback,pcui->m_Error);  //nParam 为 nItemNum参数，1 为kind 参数
		}
		break;
	case GOI_GIVE_ITEM_QIANG:  //强化
		{
			PLAYER_GIVEUI* pcui = (PLAYER_GIVEUI*)uParam;
			int npid;
            npid=(int)(pcui->uId);
			SendClientCmdGive(npid,nParam,3,pcui->m_Callback,pcui->m_Error);  //nParam 为 nItemNum参数，1 为kind 参数
		}
		break;
	case GOI_GIVE_ITEM_JISHOU:  //寄售系统
		{
			PLAYER_GIVEUI* pcui = (PLAYER_GIVEUI*)uParam;
			int npid;
            npid=(int)(pcui->uId);

			unsigned int* pInfo = (unsigned int*) npid;
	        int nIdx = *pInfo;
			//int	nPrice = Item[nIdx].GetSetPrice();
		    int	nPrice = Item[nIdx].GetTradePrice();
			SendClientCmdGive(npid,nParam,4,pcui->m_Callback,pcui->m_Error,nPrice);  //nParam 为 nItemNum参数，1 为kind 参数
		}
		break;
	case GOI_GIVE_ITEM_NOBACK:  //错误提示
		SendClientCmdGive(uParam,nParam,2,NULL,NULL);
		break;
	case GOI_GIVE_ITEM_FAIL:
		SendClientCmdGive(uParam,nParam,0,NULL,NULL);
		break;
	case GOI_PKVALUE:
		SendClientPKValue(uParam);
		break;
	case GOI_LIXIAN: //离线脚本
		SendClientCmdLiXian();
		break;
	case GOI_EXIT_GAME:
		{
		  g_SubWorldSet.Close();
		  g_ScenePlace.ClosePlace();
		}
		break;
	case GOI_GAMESPACE_DISCONNECTED:
		g_SubWorldSet.Close();
		break;
	case GOI_TRADE_NPC_BUY:  //NPC买东西 商城
	{
			KUiObjAtContRegion* pObject1 = (KUiObjAtContRegion*)uParam;
			
			if (CGOG_NPCSELLITEM != pObject1->Obj.uGenre)
			{//如果不是NPC商店就跳出
				break;
			}

			int nMapID,nX,nY;
			char nMapName[64]={0},nType[32]={0},nMasterName[32]={0};
            g_ScenePlace.GetSceneNameAndFocus(nMapName, nMapID,nX, nY,nType); //得到场景的格子坐标
			int nIdx = 0;
			KItem* pItem = NULL;
            int nShuiShou,nShuiType;
			int nIsTong=0;
			snprintf(nMasterName, sizeof(nMasterName), "%s", SubWorld[0].nWorldMapInfo[STR_MAP_MASTER].c_str());
			if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_IsWarCity>0 && strstr(nMasterName,Player[CLIENT_PLAYER_INDEX].m_cTong.m_BMasterName))
			{
               nIsTong=1;
			}   
			 else
			 {
               nIsTong=0;
			 }
             

			 nShuiType  = SubWorld[0].m_ShuiShouType;   //税收类型

             nShuiShou = 0;//Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_WarShuishou;

			 nIdx      = BuySell.GetItemIndex(Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nBuyIdx, pObject1->Obj.uId);
			 pItem     = BuySell.GetItemidx(nIdx);

			 if (pItem==NULL)
			 {
				 Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("提示:物品数据有误,请联系GM处理!");
				 break;
			 }

			int nWidth, nHeight;

			ItemPos	Pos;
			nWidth = pItem->GetWidth();
			nHeight = pItem->GetHeight();

			if (!Player[CLIENT_PLAYER_INDEX].m_ItemList.SearchPosition(nWidth, nHeight, &Pos))
			{//查找位置
				nRet = 0;
				Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("提示:包袱空间不足!");
				break;
			}

			if (Pos.nPlace != pos_equiproom)
			{
				nRet = 0;

				KSystemMessage	sMsg;
				
				snprintf(sMsg.szMessage, sizeof(sMsg.szMessage), "%s", MSG_SHOP_NO_ROOM);
				sMsg.eType = SMT_SYSTEM;
				sMsg.byConfirmType = SMCT_CLICK;
				sMsg.byPriority = 1;
				sMsg.byParamSize = 0;
				sMsg.nMsgLen=TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				break;
			}

            int nJXB = pItem->GetPrice();
			int nJB  = pItem->GetPriceXu();
			int  nSellModel = pItem->GetcModel();
            int nBei = 1;
			if (nParam>1)
			{
               if (!pItem->IsStack())
			   {
				 //char nMsg[64]={0};
				 //sprintf(nMsg,"提示:该物品不能批量购买(%d)!",nSellModel);
				 Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("提示:该物品不能批量购买!");
				 break;	
			   }

			   nBei =nParam;					
			}

            if  (nShuiType==0)
			{ 
			  if (nIsTong==1)
			  { 
               nJXB -= float(nJXB*nShuiShou/100);
               nJB  -= float(nJB*nShuiShou/100);

			  } 
			  else
			  { 
			   nJXB += float(nJXB*nShuiShou/100);
               nJB  += float(nJB*nShuiShou/100);
			  } 
			} 
		    else
			{ 
			  if (nIsTong==1)
			  { 
			 	nJXB -= float(nJXB*nShuiShou/100);
				nJB  -= float(nJB*nShuiShou/100);
				
			  } 
			  else
			  {  
				nJXB -= float(nJXB*nShuiShou/100);
				nJB  -= float(nJB*nShuiShou/100);
			  }  
			}
			
			if (nJB > 0 && Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipmentXu() >= nJB*nBei/*&& pObject1->eContainer==UOC_MARKET) || pObject1->Obj.uGenre==item_task*/)
			{//金币的 必须是金币 购买
				SendClientCmdBuy(pObject1->Obj.uId, pos_equiproom, Pos.nX, Pos.nY,nShuiShou,nIsTong,nMapID,nParam); //发送买卖的信息给服务器
                break;
			}
		    else if (nJXB >0);/// && Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipmentMoney() >= nJXB*nBei /*&& pObject1->eContainer==UOC_NPC_SHOP*/)
			{
				if (nSellModel == moneyunit_money || nSellModel == moneyunit_moneya)
				{//银两不足
					 if  (Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipmentMoney() < nJXB*nBei)
						 break;
				}
				else if (nSellModel == moneyunit_xu)
				{//金币不足
					if  (Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipmentXu() < nJXB*nBei)
						break;
				}
				else if (nSellModel == moneyunit_fuyuan)
				{//富源不足
					if  (Player[CLIENT_PLAYER_INDEX].m_cFuYuan.GetFuYuanValue() < nJXB*nBei)
						break;
				}
				else if (nSellModel == moneyunit_repute)
				{//声望不足
					if  (Player[CLIENT_PLAYER_INDEX].m_cRepute.GetReputeValue() < nJXB*nBei)
						break;
				}
				else if (nSellModel == moneyunit_expoint)
				{//点卡不足	Player[nPlayerIdx].GetExtPoint()
					//if  (Player[CLIENT_PLAYER_INDEX].GetExtPoint() < nJXB*nBei)
					if  (Player[CLIENT_PLAYER_INDEX].m_cTask.GetSaveVal(2051)< nJXB*nBei)
						break;
				}
				else if (nSellModel == moneyunit_jifen)
				{//积分	
					if  (Player[CLIENT_PLAYER_INDEX].m_cTask.GetSaveVal(192)< nJXB*nBei)
						break;
				}
				
				SendClientCmdBuy(pObject1->Obj.uId, pos_equiproom, Pos.nX, Pos.nY,nShuiShou,nIsTong,nMapID,nParam); //发送买卖的信息给服务器
                break;
			} //检测身上的银两

			nRet = 0;
			KSystemMessage	sMsg;
			if (nIsTong==1)
			{	 if  (nSellModel == moneyunit_money || nSellModel == moneyunit_moneya || nSellModel == moneyunit_xu)
				     _snprintf(sMsg.szMessage,sizeof(sMsg.szMessage)-1, MSG_SHOP_YOU_XU,nShuiShou,"%%");
			     else  if (nSellModel == moneyunit_fuyuan)
					_snprintf(sMsg.szMessage,sizeof(sMsg.szMessage)-1, "提示:占帮成员优惠<color=green>(%d%s)<color>,富源不足!",nShuiShou,"%%");
				 else  if (nSellModel == moneyunit_repute)
					_snprintf(sMsg.szMessage,sizeof(sMsg.szMessage)-1, "提示:占帮成员优惠<color=green>(%d%s)<color>,声望不足!",nShuiShou,"%%");
				 else  if (nSellModel == moneyunit_expoint)
					_snprintf(sMsg.szMessage,sizeof(sMsg.szMessage)-1, "提示:占帮成员优惠<color=green>(%d%s)<color>,点卡不足!",nShuiShou,"%%");
				 else  if (nSellModel == moneyunit_jifen)
					_snprintf(sMsg.szMessage,sizeof(sMsg.szMessage)-1, "提示:占帮成员优惠<color=green>(%d%s)<color>,积分不足!",nShuiShou,"%%");
				 else
					 _snprintf(sMsg.szMessage,sizeof(sMsg.szMessage)-1,"数据错误(%d)!",nSellModel);
			}		 
			else
			{
				if  (nSellModel == moneyunit_money || nSellModel == moneyunit_moneya || nSellModel == moneyunit_xu)
					_snprintf(sMsg.szMessage,sizeof(sMsg.szMessage)-1, MSG_SHOP_NO_XU,nShuiShou,"%%");
				else  if (nSellModel == moneyunit_fuyuan)
					_snprintf(sMsg.szMessage,sizeof(sMsg.szMessage)-1, "提示:另加<color=green>税收(%d%s)<color>,富源不足!",nShuiShou,"%%");
				else  if (nSellModel == moneyunit_repute)
					_snprintf(sMsg.szMessage,sizeof(sMsg.szMessage)-1, "提示:另加<color=green>税收(%d%s)<color>,声望不足!",nShuiShou,"%%");
				else  if (nSellModel == moneyunit_expoint)
					_snprintf(sMsg.szMessage,sizeof(sMsg.szMessage)-1, "提示:另加<color=green>税收(%d%s)<color>,点卡不足!",nShuiShou,"%%");
				else  if (nSellModel == moneyunit_jifen)
					_snprintf(sMsg.szMessage,sizeof(sMsg.szMessage)-1, "提示:另加<color=green>税收(%d%s)<color>,积分不足!",nShuiShou,"%%");	
				else
					 _snprintf(sMsg.szMessage,sizeof(sMsg.szMessage)-1,"数据错误(%d)!",nSellModel);
			}		

			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 1;
			sMsg.byParamSize = 0;
			sMsg.nMsgLen=TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
			break;	
		}
		break;
	case GOI_ITEM_VIPDAZAO:
	{
	    if (!uParam)
		{
			break;  
		}

		KUiPlyerVipDaZao* pVipInfo = (KUiPlyerVipDaZao*)uParam;
		KUiPlyerVipDoscript nVip;
		ZeroMemory(&nVip,sizeof(nVip));

		if (nParam)
		{//装备定制
			ItemPos Pos;
            if (!Player[CLIENT_PLAYER_INDEX].m_ItemList.SearchPosition(2,2,&Pos))
			{//查找位置
				nRet = 0;
				Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("提示:定制 必须保证包袱4格空间!");
				break;
			}
			nVip.inKind=pVipInfo->inKind;
			nVip.inCurJiHuo=pVipInfo->inCurJiHuo;
			nVip.inCurLucky=pVipInfo->inCurLucky;
			nVip.inCurExp=pVipInfo->inCurExp;
			nVip.inCurWuHang=pVipInfo->inCurWuHang;		
		}
		else
		{//会员充值
		   nVip.inKind=pVipInfo->inKind;
		   nVip.inCurJiHuo=0;
		   nVip.inCurLucky=0;
		   nVip.inCurExp=0;
		   nVip.inCurWuHang=0;	
		   snprintf(nVip.snCardAcc, sizeof(nVip.snCardAcc), "%s", pVipInfo->snCardAcc);
		   snprintf(nVip.snCardPass, sizeof(nVip.snCardPass), "%s", pVipInfo->snCardPass);
		}
		SendClientVipDoScipt(&nVip);
	    nRet = 1;
	}
	break;	
	case GOI_KICKOUT_SELF:
	{
		KUiPlyerDoscript nscript;
		memset(&nscript, 0, sizeof(nscript));
		nscript.inKind    = nParam;
		nscript.inCurTime = 0;
		nscript.inCurTimeb=0;
		nscript.inCurTimec=0;
		nscript.inCurTimed=0;
		SendClientDoScipt(&nscript);
	}
	break;
    case GOI_ITEM_FENJIE:
	{
      if (uParam)
	  {
		KUiObjAtContRegion* pObject1 = (KUiObjAtContRegion*)uParam;
	    if (CGOG_ITEM != pObject1->Obj.uGenre)
		   break;
			//放下去的东西不为空，所以是卖东西
	     int nIdx = pObject1->Obj.uId;	//Player[CLIENT_PLAYER_INDEX].m_ItemList.Hand();
		 if (nIdx > 0 && nIdx < MAX_ITEM)
		 {
            if (Item[nIdx].GetGenre() == item_equip && Item[nIdx].GetLevel() >=3 && Item[nIdx].IsBlue())
			{//执行脚本
			      
                    //SendDuanZhaoDoScipt(&nscript);
			  KUiPlyerDoscript nscript;
              memset(&nscript, 0, sizeof(nscript));
              nscript.inKind    = nParam;
              nscript.inCurTime = Item[nIdx].GetID();
			  nscript.inCurTimeb=0;
			  nscript.inCurTimec=0;
			  nscript.inCurTimed=0;
              SendClientDoScipt(&nscript);
			        
			}
			else
			{
			            KSystemMessage	sMsg;
						sprintf(sMsg.szMessage,"<color=yellow>提示:等级大于3级的蓝装才能分解！<color>");//MSG_TRADE_TASK_ITEM
						sMsg.eType = SMT_NORMAL;
						sMsg.byConfirmType = SMCT_NONE;
						sMsg.byPriority = 0;
						sMsg.byParamSize = 0;
						sMsg.nMsgLen=TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
						CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
						return 0;
			}
		 }
	  }
	  else
	  {//锻造蓝装
	       KUiPlyerDuanZao* pObject1 = (KUiPlyerDuanZao*)nIsMianBan;
		    ItemPos Pos;
            if (!Player[CLIENT_PLAYER_INDEX].m_ItemList.SearchPosition(3,4,&Pos))
			{//查找位置
				nRet = 0;
				Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("提示:锻造必须保证包袱12格空间!");
				break;
			}
              KTabFile nCduanzhao;
              if (!nCduanzhao.Load("\\Settings\\duanzhao.txt"))
		      {
                 nRet = 0;
                 Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("提示:补丁不兼容,请更新最新补丁!!");
		         break;
		      }

             if (pObject1==NULL)
			 {
                 nRet = 0;
                 Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("提示:数据有误,请与GM联系!!");
				 nCduanzhao.Clear();
				 break;
		     }

             int nCaiLiapCount[2];
			     nCduanzhao.GetInt2(pObject1->inCurTimea+2,pObject1->inCurTimec+4,nCaiLiapCount);
			  nCduanzhao.Clear();

            if (Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipmentMoney() < nCaiLiapCount[1])
			{//银两检测
				nRet = 0;
				Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("提示:银两不足!");
				break;
			}
                 
				int nCount=0;
                PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem(); 
                if (pItem && pItem->nPlace == pos_equiproom &&\
                  Item[pItem->nIdx].GetGenre() == item_task &&\
				  Item[pItem->nIdx].GetDetailType() == 425) 
                    { 
                       nCount += Item[pItem->nIdx].GetStackNum();
                    } 
                while(pItem) 
                { 
                  pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem(); 
                  if (pItem && pItem->nPlace == pos_equiproom &&\
                  Item[pItem->nIdx].GetGenre() == item_task &&\
				  Item[pItem->nIdx].GetDetailType() == 425)
                    { 
                      nCount += Item[pItem->nIdx].GetStackNum();
                    } 
                } 

              if (nCount<nCaiLiapCount[0])
			  {
			    nRet = 0;
				Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("提示:装备碎片不足!");
				break;
			  }

	          KUiPlyerDoscript nscript;
              memset(&nscript, 0, sizeof(nscript));
              nscript.inKind    = nParam;
              nscript.inCurTime = pObject1->inCurTimea;  //装备类型
			  nscript.inCurTimeb = pObject1->inCurTimeb; //五行
              nscript.inCurTimec = pObject1->inCurTimec; //等级
              nscript.inCurTimed =0;
              SendClientDoScipt(&nscript);
			  nRet = 1;
	  }
	}
    break;
	case GOI_TRADE_NPC_SELL:
		{
			KUiObjAtContRegion* pObject1 = (KUiObjAtContRegion*)uParam;

			if (CGOG_ITEM != pObject1->Obj.uGenre)
				break;
			//放下去的东西不为空，所以是卖东西
			int nIdx = pObject1->Obj.uId;	//Player[CLIENT_PLAYER_INDEX].m_ItemList.Hand();
			if (nIdx > 0 && nIdx < MAX_ITEM)
			{
				if (Item[nIdx].GetGenre() == item_task /*|| Item[nIdx].GetGenre() ==item_equip || Item[nIdx].GetGenre() ==item_mine*/)
				{
					if (Item[nIdx].GetIsSell() == 1)
					{//是可以买卖的	
						SendClientCmdSell(Item[nIdx].GetID());
						return 1;
					}
					else 
					{
						KSystemMessage	sMsg;
						sprintf(sMsg.szMessage,"<color=yellow>提示:任务/限时/绑定物品不能买卖！<color>");//MSG_TRADE_TASK_ITEM
						sMsg.eType = SMT_NORMAL;
						sMsg.byConfirmType = SMCT_NONE;
						sMsg.byPriority = 0;
						sMsg.byParamSize = 0;
						sMsg.nMsgLen=TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
						CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
						return 0;
					}
				}

				if (Item[nIdx].GetIsBang())
				{
					KSystemMessage	sMsg;
					sprintf(sMsg.szMessage,"<color=yellow>提示:任务/限时/绑定物品不能买卖！<color>");//MSG_TRADE_TASK_ITEM
					sMsg.eType = SMT_NORMAL;
					sMsg.byConfirmType = SMCT_NONE;
					sMsg.byPriority = 0;
					sMsg.byParamSize = 0;
					sMsg.nMsgLen=TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
					CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
					return 0;
				}

				SendClientCmdSell(Item[nIdx].GetID());
				return 1;
			}
			else
			{
				return 0;
			}
		}
		break;
	case GOI_TRADE_NPC_REPAIR:
		{//修理
			KUiObjAtContRegion* pObject1 = (KUiObjAtContRegion*)uParam;

			if (CGOG_ITEM != pObject1->Obj.uGenre)
				break;
			//放下去的东西不为空，所以是卖东西
			int nIdx = pObject1->Obj.uId;	//Player[CLIENT_PLAYER_INDEX].m_ItemList.Hand();
			if (nIdx > 0 && nIdx < MAX_ITEM)
			{		
				if (Item[nIdx].GetGenre() != item_equip)
				{//如果不是装备的 不能修理
					KSystemMessage	sMsg;
					sprintf(sMsg.szMessage,"提示:该物品不能修理!");
					sMsg.eType = SMT_NORMAL;
					sMsg.byConfirmType = SMCT_NONE;
					sMsg.byPriority = 0;
					sMsg.byParamSize = 0;
					sMsg.nMsgLen=TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
					CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
					return 0;
				}
				else if (Item[nIdx].GetDurability() == -1 || Item[nIdx].GetDurability() == Item[nIdx].GetMaxDurability())
				{//永不磨损 或 持久 是满的 不需要修理
					KSystemMessage	sMsg;
					sprintf(sMsg.szMessage,"提示:该物品无需修理!");
					sMsg.eType = SMT_NORMAL;
					sMsg.byConfirmType = SMCT_NONE;
					sMsg.byPriority = 0;
					sMsg.byParamSize = 0;
					sMsg.nMsgLen=TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
					CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);				
					return 0;
				}
				else if (Item[nIdx].GetRepairPrice() <= Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipmentMoney())
				{//修理的价格 小于包袱的价格 就可以修理了
					SendClientCmdRepair(Item[nIdx].GetID());
				}
				else
				{
					KSystemMessage	sMsg;
					sprintf(sMsg.szMessage,"提示:银两不足,不能修理!");
					sMsg.eType = SMT_NORMAL;
					sMsg.byConfirmType = SMCT_NONE;
					sMsg.byPriority = 0;
					sMsg.byParamSize = 0;
					sMsg.nMsgLen=TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
					CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
					return 0;
				}
				
				return 1;
			}
			else
			{
				return 0;
			}
		}
		break;
	case GOI_NPC_ITEM_BREAK:   //拆分可叠物品
		{
			KUiObjAtContRegion* pObject1 = (KUiObjAtContRegion*)uParam;

			if (CGOG_ITEM != pObject1->Obj.uGenre)
				break;
			//放下去的东西不为空，所以是卖东西
			int nIdx = pObject1->Obj.uId;	//Player[CLIENT_PLAYER_INDEX].m_ItemList.Hand();
			if (nIdx < MAX_ITEM && nIdx != 0)
			{		
				
				if (Player[CLIENT_PLAYER_INDEX].m_ItemList.Hand())
				{
					Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("<color=yellow>拆分<color>:手上有东西,拆分失败!");
					break;
				}

				if (Item[nIdx].GetStackNum() > nParam)
				{
					Item[nIdx].SetStackNum(Item[nIdx].GetStackNum() - nParam);
					            SendClientCmdBreak(Item[nIdx].GetID(),nParam);
				}
				else
				{
					KSystemMessage	sMsg;
					sprintf(sMsg.szMessage, "提示:拆分失败或该物品没有叠加!");
					sMsg.eType = SMT_NORMAL;
					sMsg.byConfirmType = SMCT_NONE;
					sMsg.byPriority = 0;
					sMsg.byParamSize = 0;
					sMsg.nMsgLen=TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
					CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
					return 0;
				}
				
				return 1;
			}
			else
			{
				return 0;
			}
		}
		break;
	case GOI_SWITCH_OBJECT:
		//uParam = (KUiObjAtContRegion*)pObject1 -> 拿起的物品操作前的信息
		//nParam = (KUiObjAtContRegion*)pObject2 -> 放下的物品操作后的信息
		{
			ItemPos	P1, P2;
			int PartConvert[itempart_num] = 
			{ 
				itempart_head,		itempart_weapon,
				itempart_amulet,	itempart_cuff,
				itempart_body,		itempart_belt,
				itempart_ring1,		itempart_ring2,
				itempart_pendant,	itempart_foot,
				itempart_horse,		itempart_mask,	// mat na
				itempart_pifeng,    itempart_yinjian,
				itempart_shiping	//披风	
			};
			KUiObjAtContRegion* pObject1 = (KUiObjAtContRegion*)uParam;  //在目标容器中拿起
			KUiObjAtContRegion* pObject2 = (KUiObjAtContRegion*)nParam;  //放进目标容器中
			
			if (!pObject1 && !pObject2)
				break;
			
			if (pObject1) //拿起 源？
			{
				switch(pObject1->eContainer)  //储物箱等 容器
				{
				case UOC_STORE_BOX:
					P1.nPlace = pos_repositoryroom;
					P1.nX = pObject1->Region.h;
					P1.nY = pObject1->Region.v;
					P1.nSIsRongqi=pObject1->Region.IsRongqi;
					break;
				case UOC_EX_BOX1:	// mo rong ruong 1
					P1.nPlace = pos_exbox1room;
					P1.nX = pObject1->Region.h;
					P1.nY = pObject1->Region.v;
					P1.nSIsRongqi=pObject1->Region.IsRongqi;
					break;
				case UOC_EX_BOX2:	// mo rong ruong 2
					P1.nPlace = pos_exbox2room;
					P1.nX = pObject1->Region.h;
					P1.nY = pObject1->Region.v;
					P1.nSIsRongqi=pObject1->Region.IsRongqi;
					break;
				case UOC_EX_BOX3:	// mo rong ruong 3
					P1.nPlace = pos_exbox3room;
					P1.nX = pObject1->Region.h;
					P1.nY = pObject1->Region.v;
					P1.nSIsRongqi=pObject1->Region.IsRongqi;
					break;
				case UOC_ITEM_EX:	//子母袋
					P1.nPlace = pos_equiproomex;
					P1.nX = pObject1->Region.h;
					P1.nY = pObject1->Region.v;
					P1.nSIsRongqi=pObject1->Region.IsRongqi;
					break;
				case UOC_IMMEDIA_ITEM:
					P1.nPlace = pos_immediacy;
					P1.nX = pObject1->Region.h;
					P1.nY = pObject1->Region.v;
					P1.nSIsRongqi=pObject1->Region.IsRongqi;
					break;
				case UOC_ITEM_TAKE_WITH:
					P1.nPlace = pos_equiproom;
					P1.nX = pObject1->Region.h;
					P1.nY = pObject1->Region.v;
					P1.nSIsRongqi=pObject1->Region.IsRongqi;
					break;
				case UOC_EQUIPTMENT:
					{
						// TODO:暂时没有第二装备
						if (pObject1->Region.h == 1)
							break;
						P1.nPlace = pos_equip;
						P1.nX = PartConvert[pObject1->Region.v];
					}
					break;
				case UOC_CHESSGAME:
					{
						// TODO:象棋
						if (pObject1->Region.h == 1)
							break;
						P1.nPlace = pos_xiangqi;
						P1.nX = pObject1->Region.v;
					}
					break;
				case UOC_TO_BE_TRADE:
					P1.nPlace = pos_traderoom;
					P1.nX = pObject1->Region.h;
					P1.nY = pObject1->Region.v;
					P1.nSIsRongqi=pObject1->Region.IsRongqi;
					break;
				case UOC_ITEM_GIVE:
					P1.nPlace = pos_give;
					P1.nX = pObject1->Region.h;
					P1.nY = pObject1->Region.v;
					P1.nSIsRongqi=pObject1->Region.IsRongqi;
					break;
				case UOC_ITEM_DAZAO:
					P1.nPlace = pos_dazao;
					P1.nX = pObject1->Region.h;
					P1.nY = pObject1->Region.v;
					P1.nSIsRongqi=pObject1->Region.IsRongqi;
					break;
				case UOC_ITEM_CAILIAO:
					P1.nPlace = pos_cailiao;
					P1.nX = pObject1->Region.h;
					P1.nY = pObject1->Region.v;
					P1.nSIsRongqi=pObject1->Region.IsRongqi;
					break;				
				case UOC_NPC_SHOP:
				{//NPC商店
				/*	if (CGOG_NPCSELLITEM != pObject1->Obj.uGenre)
						break;
					int nMapID,nX,nY;
					char nMapName[32],nType[16];
                    g_ScenePlace.GetSceneNameAndFocus(nMapName, nMapID,nX, nY,nType); //得到场景的格子坐标
					int nIdx = 0;
					KItem* pItem = NULL;
					int nShuiShou;
					int nIsTong=0;
					if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_IsWarCity>0 && strcmpi(SubWorld[0].m_WarSubMaster,Player[CLIENT_PLAYER_INDEX].m_cTong.m_BMasterName)==0)
					{
						nIsTong=1;
					}   
					else
					{
						nIsTong=0;
					} 
					nShuiShou = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_WarShuishou;
					nIdx = BuySell.GetItemIndex(Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nBuyIdx, pObject1->Obj.uId);
					pItem = BuySell.GetItem(nIdx);
					
					int nWidth, nHeight;
					ItemPos	Pos;
					
					nWidth  = pItem->GetWidth();
					nHeight = pItem->GetHeight();

					if (!Player[CLIENT_PLAYER_INDEX].m_ItemList.SearchPosition(nWidth, nHeight, &Pos))
					{
						nRet = 0;
						KSystemMessage	sMsg;
						
						strcpy(sMsg.szMessage, MSG_SHOP_NO_ROOM);
						sMsg.eType = SMT_SYSTEM;
						sMsg.byConfirmType = SMCT_CLICK;
						sMsg.byPriority = 1;
						sMsg.byParamSize = 0;
						sMsg.nMsgLen=TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
						CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
						break;
					}
					if (Pos.nPlace != pos_equiproom)
					{
						nRet = 0;
						KSystemMessage	sMsg;
						
						strcpy(sMsg.szMessage, MSG_SHOP_NO_ROOM);
						sMsg.eType = SMT_SYSTEM;
						sMsg.byConfirmType = SMCT_CLICK;
						sMsg.byPriority = 1;
						sMsg.byParamSize = 0;
						sMsg.nMsgLen=TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
						CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
						break;
					}
					int nJXB=pItem->GetPrice();
					if (nIsTong==1)
					{
						nJXB=nJXB-float(nJXB*nShuiShou/100);	
					}
					else
					{
						nJXB=nJXB+float(nJXB*nShuiShou/100);
					} 

					if (Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipmentMoney() < nJXB)
					{
						nRet = 0;
						KSystemMessage	sMsg;		
						if (nIsTong==1)
							_snprintf(sMsg.szMessage,sizeof(sMsg.szMessage), MSG_SHOP_YOU_MONEY,nShuiShou,"%%");
						else
                            _snprintf(sMsg.szMessage,sizeof(sMsg.szMessage), MSG_SHOP_NO_MONEY,nShuiShou,"%%");
						sMsg.eType = SMT_NORMAL;
						sMsg.byConfirmType = SMCT_NONE;
						sMsg.byPriority = 1;
						sMsg.byParamSize = 0;
						sMsg.nMsgLen=TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
						CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
						break;
					}

					SendClientCmdBuy(pObject1->Obj.uId, pos_equiproom, Pos.nX, Pos.nY,nShuiShou,nIsTong,nMapID);
			*/
			  }
					break;
				case UOC_MARKET:  //货币为金币
				{//商城 奇珍阁
				/*	if (CGOG_NPCSELLITEM != pObject1->Obj.uGenre)
						break;

					int nMapID,nX,nY;
					char nMapName[32],nType[16];
                    g_ScenePlace.GetSceneNameAndFocus(nMapName, nMapID,nX, nY,nType); //得到场景的格子坐标
					int nIdx = 0;
					KItem* pItem = NULL;
					int nShuiShou;
					int nIsTong=0;
					if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_IsWarCity>0 && strcmpi(SubWorld[0].m_WarSubMaster,Player[CLIENT_PLAYER_INDEX].m_cTong.m_BMasterName)==0)
					{
						nIsTong=1;
					}   
					else
					{
						nIsTong=0;
					} 
					nShuiShou=Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_WarShuishou;
					nIdx = BuySell.GetItemIndex(Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nBuyIdx, pObject1->Obj.uId);
					pItem = BuySell.GetItem(nIdx);
					
					int nWidth, nHeight;
					ItemPos	Pos;
					
					nWidth = pItem->GetWidth();
					nHeight = pItem->GetHeight();
					if (!Player[CLIENT_PLAYER_INDEX].m_ItemList.SearchPosition(nWidth, nHeight, &Pos))
					{//包袱
						nRet = 0;
						KSystemMessage	sMsg;
						
						strcpy(sMsg.szMessage, MSG_SHOP_NO_ROOM);
						sMsg.eType = SMT_SYSTEM;
						sMsg.byConfirmType = SMCT_CLICK;
						sMsg.byPriority = 1;
						sMsg.byParamSize = 0;
						sMsg.nMsgLen=TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
						CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
						break;
					}
					if (Pos.nPlace != pos_equiproom)
					{
						nRet = 0;
						KSystemMessage	sMsg;
						
						strcpy(sMsg.szMessage, MSG_SHOP_NO_ROOM);
						sMsg.eType = SMT_SYSTEM;
						sMsg.byConfirmType = SMCT_CLICK;
						sMsg.byPriority = 1;
						sMsg.byParamSize = 0;
						sMsg.nMsgLen=TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
						CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
						break;
					}
					int nJB=pItem->GetPriceXu();

					if (nIsTong==1)
					{
						nJB=nJB-float(nJB*nShuiShou/100);	
					}
					else
					{
						nJB=nJB+float(nJB*nShuiShou/100);
					} 

					if (Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipmentXu() < pItem->GetPriceXu())
					{
						nRet = 0;
						KSystemMessage	sMsg;
						
						_snprintf(sMsg.szMessage,sizeof(sMsg.szMessage), MSG_SHOP_YOU_XU,nShuiShou,"%%");
						sMsg.eType = SMT_SYSTEM;
						sMsg.byConfirmType = SMCT_CLICK;
						sMsg.byPriority = 1;
						sMsg.byParamSize = 0;
						sMsg.nMsgLen=TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
						CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
						break;
					}

					SendClientCmdBuy(pObject1->Obj.uId, pos_equiproom, Pos.nX, Pos.nY,nShuiShou,nIsTong,nMapID);
				*/
				  }
					break;
				}
			}
			if (pObject2) // 放下到目标容器
			{
				switch(pObject2->eContainer)
				{
				case UOC_STORE_BOX:
					P2.nPlace = pos_repositoryroom;
					P2.nX = pObject2->Region.h;
					P2.nY = pObject2->Region.v;
					P2.nDIsRongqi=pObject2->Region.IsRongqi;
					break;
				case UOC_EX_BOX1:	// mo rong ruong 1
					P2.nPlace = pos_exbox1room;
					P2.nX = pObject2->Region.h;
					P2.nY = pObject2->Region.v;
					P2.nDIsRongqi=pObject2->Region.IsRongqi;
					break;
				case UOC_EX_BOX2:	// mo rong ruong 2
					P2.nPlace = pos_exbox2room;
					P2.nX = pObject2->Region.h;
					P2.nY = pObject2->Region.v;
					P2.nDIsRongqi=pObject2->Region.IsRongqi;
					break;
				case UOC_EX_BOX3:	// mo rong ruong 3
					P2.nPlace = pos_exbox3room;
					P2.nX = pObject2->Region.h;
					P2.nY = pObject2->Region.v;
					P2.nDIsRongqi=pObject2->Region.IsRongqi;
					break;
				case UOC_ITEM_EX:	//子母袋
					P2.nPlace = pos_equiproomex;
					P2.nX = pObject2->Region.h;
					P2.nY = pObject2->Region.v;
					P2.nDIsRongqi=pObject2->Region.IsRongqi;
					break;
				case UOC_IMMEDIA_ITEM:
					P2.nPlace = pos_immediacy;
					P2.nX = pObject2->Region.h;
					P2.nY = pObject2->Region.v;
					P2.nDIsRongqi=pObject2->Region.IsRongqi;
					break;
				case UOC_ITEM_TAKE_WITH: //包袱
					P2.nPlace = pos_equiproom;
					P2.nX = pObject2->Region.h;
					P2.nY = pObject2->Region.v;
					P2.nDIsRongqi=pObject2->Region.IsRongqi;
					break;
				case UOC_EQUIPTMENT:
					{
						// TODO:暂时没有第二装备
						if (pObject2->Region.h == 1)
							break;
						P2.nPlace = pos_equip;
						P2.nX = PartConvert[pObject2->Region.v];
					}
					break;
				case UOC_CHESSGAME:
					{
						// TODO:象棋
						if (pObject2->Region.h == 1)
							break;
						P2.nPlace = pos_xiangqi;
						P2.nX = pObject2->Region.v;
					}
					break;
				case UOC_TO_BE_TRADE:
					P2.nPlace = pos_traderoom;
					P2.nX = pObject2->Region.h;
					P2.nY = pObject2->Region.v;
					P2.nDIsRongqi=pObject2->Region.IsRongqi;
					break;
				case UOC_ITEM_GIVE:
					P2.nPlace = pos_give;
					P2.nX = pObject2->Region.h;
					P2.nY = pObject2->Region.v;
					P2.nDIsRongqi=pObject2->Region.IsRongqi;
					break;
				case UOC_ITEM_DAZAO:
					P2.nPlace = pos_dazao;
					P2.nX = pObject2->Region.h;
					P2.nY = pObject2->Region.v;
					P2.nDIsRongqi=pObject2->Region.IsRongqi;
					break;
				case UOC_ITEM_CAILIAO:
					P2.nPlace = pos_cailiao;
					P2.nX = pObject2->Region.h;
					P2.nY = pObject2->Region.v;
					P2.nDIsRongqi=pObject2->Region.IsRongqi;
					break;
				case UOC_NPC_SHOP:
					break;
				case UOC_MARKET:
					break;
				}
			}
			if (!pObject1)
			{
				memcpy(&P1, &P2, sizeof(P1));
			}

			if (!pObject2)
			{
				memcpy(&P2, &P1, sizeof(P1));
			}

			Player[CLIENT_PLAYER_INDEX].MoveItem(P1,P2,nIsMianBan);//移动物品  P1 目标容器 P2 源容器
		}
		break;
     case GOI_SWITCH_SKILLS:
		 { 
		       if (uParam)
			   {   
					//KSkillMapPos* pInfo = (KSkillMapPos*)uParam;
					//Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_mMapX=pInfo->nCursorX;
					//Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_mMapY=pInfo->nCursorY;
				}
				else
				{ 
					if (nParam)
						Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_IsgetSkill=nParam;
					else
					{
						Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_IsgetSkill=0;
					}
				} 
				
		 } 
		 break;
	//玩家点完对话框
	case GOI_INFORMATION_CONFIRM_NOTIFY:
	{
		PLAYER_SELECTUI_COMMAND command;
		command.nSelectIndex = 0;
		Player[CLIENT_PLAYER_INDEX].OnSelectFromUI(&command, UI_TALKDIALOG);
		break;
	}
	
	//NPC跳窗 问题选择答案 ---执行脚本函数
	//nParma = nAnswerIndex
	case GOI_QUESTION_CHOOSE:
		if (g_bUISelLastSelCount == 0 )
			break;	
		{
			PLAYER_SELECTUI_COMMAND command;
			command.nSelectIndex = nParam;
			Player[CLIENT_PLAYER_INDEX].OnSelectFromUI(&command,UI_SELECTDIALOG);
		}
		break;

	//使用物品
	//uParam = (KUiObjAtRegion*)pInfo -> 物品的数据以及物品原来摆放的位置
	//nParam = 物品使用前放置的位置，取值为枚举类型UIOBJECT_CONTAINER。
	case GOI_USE_ITEM:
		//to do: waiting for...
		if (uParam)
		{
			KUiObjAtRegion* pInfo = (KUiObjAtRegion*) uParam;
			int nPlace = nParam;
			ItemPos	Pos;
			switch(nPlace)
			{
			case UOC_ITEM_TAKE_WITH:  // 随身物品
				Pos.nPlace = pos_equiproom;
				break;
			case UOC_IMMEDIA_ITEM:   //立即物品  快捷
				Pos.nPlace = pos_immediacy;
				break;
			default:
				Pos.nPlace = -1;
				break;
			}
			Pos.nX = pInfo->Region.h;
			Pos.nY = pInfo->Region.v;
					
			if (pInfo->Obj.uGenre == CGOG_ITEM && pInfo->Obj.uId > 0 && Pos.nPlace != -1)
				Player[CLIENT_PLAYER_INDEX].ApplyUseItem(pInfo->Obj.uId, Pos);
		}
		break;

	case GOi_SET_WINDOWS_ITEM:  //打造 提取 熔炼
		if (uParam)
		{
			KUiObjAtRegion* pInfo = (KUiObjAtRegion*) uParam;
			int nPlace = nParam;
			int nDetail = Item[pInfo->Obj.uId].GetDetailType();
			int nPartuil = Item[pInfo->Obj.uId].GetParticular();
			int bAdd = 0;
			switch(nPlace)
			{
			case 0:
				if(!Item[pInfo->Obj.uId].IsPurple() && Item[pInfo->Obj.uId].GetGoldId() < 1)
				{
					if (nDetail == 3)
					{
						bAdd = 1;
					}
					if (nDetail == 4)
					{
						bAdd = 2;	

					}
					if (nDetail == 9)
					{
						bAdd = 3;
					}
				}
				if (bAdd==0)
					nRet=0;
				break;
			case 1:
				if (Item[pInfo->Obj.uId].GetGenre() == item_mine && nPartuil == 147)
				{
					bAdd = 1;
				}
				if (bAdd==0)
					nRet=0;
				break;
			case 2:
				if (Item[pInfo->Obj.uId].GetGenre() == item_mine && nPartuil >= 200 && nPartuil <= 205)
				{
					bAdd = 1;
				}
				if (bAdd==0)
					nRet=0;
				break;
			case 3: //提取
				if(Item[pInfo->Obj.uId].GetGenre() == item_equip && Item[pInfo->Obj.uId].GetGoldId() <1 && !Item[pInfo->Obj.uId].IsPurple())
				if ((nDetail >= 0 && nDetail <= 2) || (nDetail >= 5 && nDetail <=8 ))
					{
						bAdd = 1;	//装备
					}

				if (Item[pInfo->Obj.uId].GetGenre() == item_mine && nPartuil == 147)
				{
					bAdd = 2;		//玄晶
				}

				if (Item[pInfo->Obj.uId].GetGenre() == item_mine && nPartuil >= 149 && nPartuil <= 154)
				{
					bAdd = 3;		//矿石
				}

				if (
					(Item[pInfo->Obj.uId].GetGenre() == item_mine && 
					Item[pInfo->Obj.uId].GetParticular() == 122) || 
					(Item[pInfo->Obj.uId].GetGenre() == item_task && 
					(Item[pInfo->Obj.uId].GetDetailType() == 215 ||
					Item[pInfo->Obj.uId].GetDetailType() > 238 && Item[pInfo->Obj.uId].GetDetailType() <= 241))
					
					)
				{
					bAdd = 4;		//辅助其他
				}
				if ((Item[pInfo->Obj.uId].GetGenre() == item_mine && 
					Item[pInfo->Obj.uId].GetParticular() == 123) ||
					(Item[pInfo->Obj.uId].GetGenre() == item_task && 
					Item[pInfo->Obj.uId].GetDetailType() == 216))
					bAdd = 4;		//其他
				
				if ((Item[pInfo->Obj.uId].GetGenre() == item_mine && 
					Item[pInfo->Obj.uId].GetParticular() == 124) ||
					(Item[pInfo->Obj.uId].GetGenre() == item_task && 
					Item[pInfo->Obj.uId].GetDetailType() == 217))
					bAdd = 4;		//其他
				
				if (Item[pInfo->Obj.uId].GetGenre() == item_mine && 
					Item[pInfo->Obj.uId].GetParticular() == 398)
					bAdd = 4;		//其他
				if (bAdd==0)
					nRet=0;
				break;
			case 4:  //打造紫装  //打孔熔炼
				if(Item[pInfo->Obj.uId].GetGenre() == item_equip &&/* Item[pInfo->Obj.uId].GetGoldId() > 0 &&*/ !Item[pInfo->Obj.uId].IsPurple())
				//	if ((nDetail >= 0 && nDetail <= 2) || (nDetail >= 5 && nDetail <=8 ))装备部位限制
					{
						bAdd = 1;	//装备
					}					
				if (Item[pInfo->Obj.uId].GetGenre() == item_mine && nPartuil == 147)
				{
						bAdd = 2;		//玄晶
				}
				if (bAdd==0)
					nRet=0;
				break;
			case 5:  //镶套紫装
				if(Item[pInfo->Obj.uId].GetGenre() == item_equip && (Item[pInfo->Obj.uId].IsPurple()>0 || Item[pInfo->Obj.uId].IsRong() >0))
					//if ((nDetail >= 0 && nDetail <= 2) || (nDetail >= 5 && nDetail <=8 )) 装备部位限制
					{
						bAdd = 1;	//装备
					}
					
				if (Item[pInfo->Obj.uId].GetGenre() == item_mine && nPartuil == 147)
				{
					bAdd = 2;		//玄晶
				}
					
				if (Item[pInfo->Obj.uId].GetGenre() == item_mine && nPartuil >= 200 && nPartuil <= 205)
				{
					bAdd = 3;		//属性矿石
				}
					
				if (
					(Item[pInfo->Obj.uId].GetGenre() == item_mine && 
					Item[pInfo->Obj.uId].GetParticular() == 122) || 
					(Item[pInfo->Obj.uId].GetGenre() == item_task && 
					(Item[pInfo->Obj.uId].GetDetailType() == 215 ||//富源露小
					Item[pInfo->Obj.uId].GetDetailType() > 238 && Item[pInfo->Obj.uId].GetDetailType() <= 241))//三色水晶

					)
				{
					bAdd = 4;		//辅助其他
				}
				if ((Item[pInfo->Obj.uId].GetGenre() == item_mine && 
					Item[pInfo->Obj.uId].GetParticular() == 123) ||
					(Item[pInfo->Obj.uId].GetGenre() == item_task && 
					Item[pInfo->Obj.uId].GetDetailType() == 216)) //富源露中
						bAdd = 4;		//其他
				
				if ((Item[pInfo->Obj.uId].GetGenre() == item_mine && 
					Item[pInfo->Obj.uId].GetParticular() == 124) ||
					(Item[pInfo->Obj.uId].GetGenre() == item_task && 
					Item[pInfo->Obj.uId].GetDetailType() == 217)) //富源露大
						bAdd = 4;		//其他
				
				if (Item[pInfo->Obj.uId].GetGenre() == item_mine && 
					Item[pInfo->Obj.uId].GetParticular() == 398)
						bAdd = 4;		//神秘
				if (bAdd==0)
					nRet=0;
				break;

			case 6://图谱
                 //g_CompEquip.

				 if (Item[pInfo->Obj.uId].GetGenre() == item_mine && (nPartuil >= 239 && nPartuil <= 391))
				 {  
					bAdd = 7;		//图谱
				 } 

				 if (Item[pInfo->Obj.uId].GetGenre() == item_mine && nPartuil == 147)
				 { 
					bAdd = 8;		//玄晶
				 } 
				
				 if (
					(Item[pInfo->Obj.uId].GetGenre() == item_mine && 
					Item[pInfo->Obj.uId].GetParticular() == 122) || 
					(Item[pInfo->Obj.uId].GetGenre() == item_task && 
					(Item[pInfo->Obj.uId].GetDetailType() == 215 ||//富源露小
					Item[pInfo->Obj.uId].GetDetailType() > 238 && Item[pInfo->Obj.uId].GetDetailType() <= 241))//三色水晶
					
					)
				{
					bAdd = 9;		//辅助其他
				}
				if ((Item[pInfo->Obj.uId].GetGenre() == item_mine && 
					Item[pInfo->Obj.uId].GetParticular() == 123) ||
					(Item[pInfo->Obj.uId].GetGenre() == item_task && 
					Item[pInfo->Obj.uId].GetDetailType() == 216)) //富源露中
					bAdd = 9;		//其他
				
				if ((Item[pInfo->Obj.uId].GetGenre() == item_mine && 
					Item[pInfo->Obj.uId].GetParticular() == 124) ||
					(Item[pInfo->Obj.uId].GetGenre() == item_task && 
					Item[pInfo->Obj.uId].GetDetailType() == 217)) //富源露大
					bAdd = 9;		//其他
				
				if (Item[pInfo->Obj.uId].GetGenre() == item_mine && 
					Item[pInfo->Obj.uId].GetParticular() == 398)
					bAdd = 9;		//神秘

				if (bAdd==0)
					nRet=0;
				break;
			default:
				bAdd = 0;
				nRet=0;
				break;
			}
			if (bAdd)
			{
				CoreDataChanged(GDCNI_SET_ITEM,(unsigned int)pInfo,bAdd);//bAdd  容器序号标示
			}
			else
			{
				nRet=0;
			}
		}
		break;
	case GOI_RONG_WINDOWS_ITEM://熔炼纹钢
		if (uParam)
		{ 
			KUiObjAtRegion* pInfo = (KUiObjAtRegion*) uParam;
			int nPlace = nParam;
			int nDetail = Item[pInfo->Obj.uId].GetDetailType();
			int nPartuil = Item[pInfo->Obj.uId].GetParticular();
			int bAdd = 0;
			switch(nPlace)
			{
	          case 0:  //熔炼纹钢
		          if(Item[pInfo->Obj.uId].GetGenre() == item_equip &&/* Item[pInfo->Obj.uId].GetGoldId() > 0 &&*/ !Item[pInfo->Obj.uId].IsPurple())
			//	if ((nDetail >= 0 && nDetail <= 2) || (nDetail >= 5 && nDetail <=8 ))装备部位限制
				  { 
			         bAdd = 1;	//装备
				  }	 				
		          if (Item[pInfo->Obj.uId].GetGenre() == item_mine && nPartuil == 147)
				  { 
			          bAdd = 2;		//玄晶
				  } 
		          if (bAdd==0)
		            	  nRet=0;
				  break;
			  case 3:  //提炼纹钢
				  if(Item[pInfo->Obj.uId].GetGenre() == item_equip &&/* Item[pInfo->Obj.uId].GetGoldId() > 0 &&*/ !Item[pInfo->Obj.uId].IsPurple())
					  //	if ((nDetail >= 0 && nDetail <= 2) || (nDetail >= 5 && nDetail <=8 ))装备部位限制
				  { 
					  bAdd = 1;	//装备
				  }	 				
				  if (Item[pInfo->Obj.uId].GetGenre() == item_mine && nPartuil == 147)
				  { 
					  bAdd = 2;		//玄晶
				  } 
				  if (bAdd==0)
					  nRet=0;
				  break;
			  default:
				  bAdd = 0;
				  nRet=0;
				break;
			}

			if (bAdd)
			{
				CoreDataChanged(GDCNI_RONG_ITEM,(unsigned int)pInfo,bAdd);
			}
			else
			{
				nRet=0;
			}
		}
	  break;
	case GOI_COMPITEM_WENGANG: //熔炼纹钢
			if (uParam)
			{	
				SendClientCmdComp((ItemCounmd *)uParam,nParam,8);
			}
	  break;
	case GOI_COMPITEM_TILIAN: //提炼纹钢
			if (uParam)
			{	 
				SendClientCmdComp((ItemCounmd *)uParam,nParam,9);
			}
	  break;
    case GOI_COMPITEM_RONGHE: //融合
			if (uParam)
			{	 
				SendClientCmdComp((ItemCounmd *)uParam,nParam,11,nIsMianBan);
			}
	  break; 
	case GOI_COMPITEM_BAOSHI: //宝石灵合
			if (uParam)
			{	 
				SendClientCmdComp((ItemCounmd *)uParam,nParam,10);
			}
	  break;  
	case GOI_COMPITEM_BIANSHI: //辨识蓝装
			if (uParam)
			{	 
				SendClientCmdComp((ItemCounmd *)uParam,nParam,12);
			}
	  break; 
	case GOI_COMPITEM_COM: //提炼
		if (uParam)
		{
			KUiObjAtContRegion* pInfo = (KUiObjAtContRegion*) uParam;

			switch(nParam)
			{
			case 1://提炼玄晶矿石
//				SendClientCmdComp(uParam,nParam,1);
				SendClientCmdComp((ItemCounmd *)uParam,nParam,1);
				break;
			case 2://升级玄晶矿石
//				SendClientCmdComp(uParam,nParam,2);
				SendClientCmdComp((ItemCounmd *)uParam,nParam,2);
				break;
			case 3://升级属性矿石
//				SendClientCmdComp(uParam,nParam,3);
				SendClientCmdComp((ItemCounmd *)uParam,nParam,3);
				break;
			}
		}
		break;
	case GOI_COMPITEM_DISTILL://提取
		if (uParam)
		{	
//			SendClientCmdComp(uParam,nParam,4);
			SendClientCmdComp((ItemCounmd *)uParam,nParam,4);
		}
		break;
	case GOI_COMPITEM_FORGE://打造
		if (uParam)
		{	
//			SendClientCmdComp(uParam,nParam,5);
			SendClientCmdComp((ItemCounmd *)uParam,nParam,5);
		}
		break;
	case GOI_COMPITEM_ENCHASE: //镶嵌
		if (uParam)
		{	
		//	ItemCounmd  *nItemCom=(ItemCounmd *)uParam;
			SendClientCmdComp((ItemCounmd *)uParam,nParam,6);
		}
		break;
	case GOI_COMPITEM_ALTAL:  // 图谱
		if (uParam)
		{	
			//			SendClientCmdComp(uParam,nParam,7);
			SendClientCmdComp((ItemCounmd *)uParam,nParam,7);
		}
		break;
	case GDI_SETFUMO_INFO:
		{
			if (nIsMianBan==13)
			{
			    SendClientCmdComp((ItemCounmd *)uParam,1,13,nParam);
			}
			else if (nIsMianBan==14)
			{//却换神将
			    //Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_FuMoTimesVal=500;
				SendClientCmdComp((ItemCounmd *)uParam,1,14,nParam);
			}
			else if (nIsMianBan==15)
			{
				//Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_FuMoTimesVal=500;
				SendClientCmdComp((ItemCounmd *)uParam,1,15,nParam);
			}
			else if (nIsMianBan==16)
			{//进阶石
				SendClientCmdComp((ItemCounmd *)uParam,1,16,nParam);
                //Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("提示:进阶石发送成功！");
			}
		}
	    break;
	case GOI_ADDITEM_CLIENT: //包袱增加物品
		if (uParam)
		{	
            KUiObjAtContRegion *pObj=(KUiObjAtContRegion *)uParam;

			int nIndex=pObj->Obj.uId;

			if (nIndex)
			{
				//	ItemCounmd  *nItemCom=(ItemCounmd *)uParam;
			   // SendClientAddItem((ItemCounmd *)uParam,nParam,6);
			   //int	x, y;
			   ItemXinXi nItemidx;
                    nItemidx.nIdx=nIndex;

			   ItemPos	sItemPos;

			   if (FALSE == Player[CLIENT_PLAYER_INDEX].m_ItemList.SearchItemPosition(Item[nIndex].GetWidth(), Item[nIndex].GetHeight(), &sItemPos,room_equipment))
			   {
				   KSystemMessage	sMsg;
				   sMsg.byConfirmType = SMCT_NONE;
				   sMsg.eType = SMT_NORMAL;
				   sMsg.byParamSize = 0;
				   sMsg.byPriority = 0;			   
				   snprintf(sMsg.szMessage, sizeof(sMsg.szMessage), "%s", MSG_SHOP_NO_ROOM);
				   sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
				   CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				   return 0;
			   }  
                  switch(pObj->eContainer) //源容器信息
				  {
				    case UOC_ITEM_DAZAO:
					  {//单对象容器
                         sItemPos.nPlace  = pos_dazao;    //源容器
					  }
                        break;
				    case UOC_ITEM_CAILIAO:
					  {//多对象容器
						  sItemPos.nPlace  = pos_cailiao;    //源容器
					  }
                       break;
				    default:
                       break;
				  }
			       //sItemPos.nPlace  = pos_cailiao;  //源容器
				   sItemPos.nX      = pObj->Region.h; //源位置
                   sItemPos.nY      = pObj->Region.v; //源位置
                    
			       Player[CLIENT_PLAYER_INDEX].ApplyAutoMoveItem(nIndex,sItemPos,room_equipment);
		           
			       //SendClientAddItem(&nItemidx);
			}
		}
		break;
	case GOI_DELITEM_CLIENT: //包袱增加物品
		if (uParam)
		{	
            KUiObjAtContRegion *pObj=(KUiObjAtContRegion *)uParam;

			int nIndex=pObj->Obj.uId;

			if (nIndex)
			{
			      // Player[CLIENT_PLAYER_INDEX].ApplyAutoMoveItem(nIndex,sItemPos,pos_equiproom); //允许吃药
			     
				SendClientDelItem(nIndex);
			}
		}
	break;	
case GOI_TASK_CHESHI:
{
	char msg[64]={0};
     if (uParam)
	 {
        Player[CLIENT_PLAYER_INDEX].m_cTask.SetSaveVal(3501,ESetByte(Player[CLIENT_PLAYER_INDEX].m_cTask.GetSaveVal(3501),1,111)); //记录筋脉大类是否完成冲穴
		Player[CLIENT_PLAYER_INDEX].m_cTask.SetSaveVal(3501,ESetByte(Player[CLIENT_PLAYER_INDEX].m_cTask.GetSaveVal(3501),2,112)); //记录经脉当前已经冲穴穴道行数
		Player[CLIENT_PLAYER_INDEX].m_cTask.SetSaveVal(3501,ESetByte(Player[CLIENT_PLAYER_INDEX].m_cTask.GetSaveVal(3501),3,113));
		Player[CLIENT_PLAYER_INDEX].m_cTask.SetSaveVal(3501,ESetByte(Player[CLIENT_PLAYER_INDEX].m_cTask.GetSaveVal(3501),4,114));
	 }
	 else
	 {
	    int vala,valb,valc,vald;
	    vala=EGetByte(Player[CLIENT_PLAYER_INDEX].m_cTask.GetSaveVal(3501),1);
		valb=EGetByte(Player[CLIENT_PLAYER_INDEX].m_cTask.GetSaveVal(3501),2);
		valc=EGetByte(Player[CLIENT_PLAYER_INDEX].m_cTask.GetSaveVal(3501),3);
		vald=EGetByte(Player[CLIENT_PLAYER_INDEX].m_cTask.GetSaveVal(3501),4);
		
		
		 snprintf(msg, sizeof(msg),"提示:3501的值:%d,%d,%d,%d",vala,valb,valc,vald);
		 Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(msg);
		
		return nRet;
	 }

}
    break;
	//穿上装备  吃药 吃蓝
	//uParam = (KUiObjAtRegion*)pInfo -> 装备的数据和放置位置信息
	//			KUiObjAtRegion::Region::h 表示属于第几套装备
	//			KUiObjAtRegion::Region::v 表示属于哪个位置的装备,其值为梅举类型
	//			UI_EQUIPMENT_POSITION的取值之一。请参看UI_EQUIPMENT_POSITION的注释。
case GOI_AUTO_COMMAND:   //外挂
        { 
            switch(uParam) 
            { 
            case 0:{ //血
                ItemPos    Pos; 
                PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem(); 

                if (pItem && (pItem->nPlace == pos_equiproom || pItem->nPlace == pos_immediacy) && 
                    (Item[pItem->nIdx].GetGenre() == item_medicine && (Item[pItem->nIdx].GetDetailType() == medicine_blood 
                    || Item[pItem->nIdx].GetDetailType() >= medicine_allboth || Item[pItem->nIdx].GetDetailType() == medicine_both))) 
                    { 
                      Pos.nPlace = pItem->nPlace; 
                      Pos.nX = pItem->nX; 
                      Pos.nY = pItem->nY; 
                      Player[CLIENT_PLAYER_INDEX].ApplyUseItem(pItem->nIdx, Pos); 
                      return 1; 
                    } 
                while(pItem) 
                { 
                    pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem(); 
                    if (pItem && (pItem->nPlace == pos_equiproom || pItem->nPlace == pos_immediacy) && 
                    (Item[pItem->nIdx].GetGenre() == item_medicine && (Item[pItem->nIdx].GetDetailType() == medicine_blood 
                    || Item[pItem->nIdx].GetDetailType() >= medicine_allboth || Item[pItem->nIdx].GetDetailType() == medicine_both))) 
                    { 
                      Pos.nPlace = pItem->nPlace; 
                      Pos.nX = pItem->nX; 
                      Pos.nY = pItem->nY; 
                      Player[CLIENT_PLAYER_INDEX].ApplyUseItem(pItem->nIdx,Pos); 
                    return 1; 
                    } 
                } 
                }break; 
            case 1:{//蓝
                ItemPos    Pos; 
                PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem(); 
                if (pItem && (pItem->nPlace == pos_equiproom || pItem->nPlace == pos_immediacy) && 
                    (Item[pItem->nIdx].GetGenre() == item_medicine && (Item[pItem->nIdx].GetDetailType() == medicine_mana 
                    || Item[pItem->nIdx].GetDetailType() >= medicine_allboth || Item[pItem->nIdx].GetDetailType() == medicine_both))) 
                    { 
                       Pos.nPlace = pItem->nPlace; 
                       Pos.nX = pItem->nX; 
                       Pos.nY = pItem->nY; 
                       Player[CLIENT_PLAYER_INDEX].ApplyUseItem(pItem->nIdx, Pos); 
                       return 1; 
                    } 
                while(pItem) 
                { 
                    pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem(); 
                    if (pItem && (pItem->nPlace == pos_equiproom || pItem->nPlace == pos_immediacy) && 
                    (Item[pItem->nIdx].GetGenre() == item_medicine && (Item[pItem->nIdx].GetDetailType() == medicine_mana 
                    || Item[pItem->nIdx].GetDetailType() >= medicine_allboth || Item[pItem->nIdx].GetDetailType() == medicine_both))) 
                    { 
                      Pos.nPlace = pItem->nPlace; 
                      Pos.nX = pItem->nX; 
                      Pos.nY = pItem->nY; 
                      Player[CLIENT_PLAYER_INDEX].ApplyUseItem(pItem->nIdx, Pos); 
                      return 1; 
                    } 
                }                
                }break; 
            case 2:
				{//  使用回城卷？
				 if (!nParam)
				 {
					Player[CLIENT_PLAYER_INDEX].m_Autoplay.nIsReTurn =nParam;
					break;
				 }

                ItemPos    Pos; 
                PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem(); 
                if (pItem && (pItem->nPlace == pos_equiproom || pItem->nPlace == pos_immediacy) && 
                    ((Item[pItem->nIdx].GetGenre()==item_task && Item[pItem->nIdx].GetDetailType()==35) || Item[pItem->nIdx].GetGenre() == item_townportal))
                    { 
					   Player[CLIENT_PLAYER_INDEX].m_Autoplay.nIsReTurn =nParam;
                       Pos.nPlace = pItem->nPlace; 
                       Pos.nX = pItem->nX; 
                       Pos.nY = pItem->nY; 
                       Player[CLIENT_PLAYER_INDEX].ApplyUseItem(pItem->nIdx,Pos); 
                       return 1; 
                    } 

                while(pItem) 
                { 
                    pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem(); 
                    if (pItem && (pItem->nPlace == pos_equiproom || pItem->nPlace == pos_immediacy) && 
                    ((Item[pItem->nIdx].GetGenre()==item_task && Item[pItem->nIdx].GetDetailType()==35) || Item[pItem->nIdx].GetGenre() == item_townportal))
                    { 
					  Player[CLIENT_PLAYER_INDEX].m_Autoplay.nIsReTurn =nParam;
					  Pos.nPlace = pItem->nPlace; 
                      Pos.nX = pItem->nX; 
                      Pos.nY = pItem->nY; 
                      Player[CLIENT_PLAYER_INDEX].ApplyUseItem(pItem->nIdx, Pos);
                      return 1; 
                    } 
                } 
				  Player[CLIENT_PLAYER_INDEX].m_Autoplay.nIsReTurn =FALSE;
                }
				break; 
			case 3:{  //自动捡钱
				//	Player[CLIENT_PLAYER_INDEX].m_cTask.SetSaveVal(166, 1);
					Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoMoney=nParam;
                }break;
			case 4:{  //自动捡装备
					Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoEques=nParam;
					if (nIsMianBan>6)
					    nIsMianBan=6;
					if (nIsMianBan<0)
                        nIsMianBan=0;

					    Player[CLIENT_PLAYER_INDEX].m_Autoplay.nShuXingNum=nIsMianBan;
					    
                }break;
			case 5:{  //自动钱和装备
				    Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoMoneyAndEques=nParam;
					if (nIsMianBan>6)
					    nIsMianBan=6;
					if (nIsMianBan<0)
                        nIsMianBan=0;
					    Player[CLIENT_PLAYER_INDEX].m_Autoplay.nShuXingNum=nIsMianBan;
                }break;
			case 6:{  //自动图谱
				    Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoTuPu=nParam;
                }break;
			case 7:{  //自动押送
				    //Player[CLIENT_PLAYER_INDEX].m_cTask.SetSaveVal(167, 0); //关闭捡装备
					//BOOL nIsYaBiao;
					Player[CLIENT_PLAYER_INDEX].m_Autoplay.nCurNum=nParam;
					Player[CLIENT_PLAYER_INDEX].m_Autoplay.nIsHaveDo=nParam;
					Player[CLIENT_PLAYER_INDEX].m_Autoplay.nIsJiaoBen=0;
                }break;
			case 8:{  //录制脚本

                  if  (nParam==TRUE)
				  {
                       KTabFileCtrl nDel;
                        char nFiledPath[128];
					    sprintf(nFiledPath,"\\FsAi\\%s_Pos.txt",Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name);
					   if (nDel.Load(nFiledPath))
					   {
                          while(nDel.GetHeight()>0)
						  { 
						    nDel.Remove(nDel.GetHeight());
                            nDel.Save(nFiledPath);
                         }
                           nDel.Clear();
					   }
					      nDel.Clear();
                   }
				    Player[CLIENT_PLAYER_INDEX].m_Autoplay.nIsJiaoBen=nParam;
                }break;
			case 9:{  //挂机模式
				    Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoModel=nParam;
					//Player[CLIENT_PLAYER_INDEX].m_Autoplay.nCurNum=0;
					//Player[CLIENT_PLAYER_INDEX].m_Autoplay.nIsHaveDo=0;
                }break;
			case 10:{ //自动应答组队
				    Player[CLIENT_PLAYER_INDEX].m_Autoplay.nCteam.nIsHuiDa=nParam;
                }break;

			case 11:{ //我是队长
				    Player[CLIENT_PLAYER_INDEX].m_Autoplay.nCteam.nIsDuiZhang=nParam;
				   }break;
			case 12:{ //我是队员
				    Player[CLIENT_PLAYER_INDEX].m_Autoplay.nCteam.nIsDuiYuan=nParam;
                }break;

			case 13:{  //
				    if (nParam)  //nIsMianBan
					{	 
                       KTabFileCtrl nDel;
					   if (nDel.openfile("\\FsAi\\itemName.txt"))
					   {
                            nDel.WriteString(nIsMianBan+1,2,strVal);
                            nDel.Save("\\FsAi\\itemName.txt");
                       }
                           nDel.Clear();
					}
					else
					{ 
						KTabFileCtrl nDel;
						if (nDel.openfile("\\FsAi\\itemUseName.txt"))
						{
                            nDel.WriteString(nIsMianBan+1,2,strVal);
                            nDel.Save("\\FsAi\\itemUseName.txt");
						}
                           nDel.Clear();
					}
				   }break;
			case 14:
				{  //道具拾取
				    Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoTool=nParam; //nAutoTool
                }
			break;
		    case 15:
				{  //
					Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CJtaskID=nParam; //抽奖专用
				//	SendClientCmdSysChoujiang(Player[CLIENT_PLAYER_INDEX].m_nIndex,nParam);
                }
			break;
			case 16:
				{  //属性添加
				  //if (nParam)  //nIsMianBan
				  {
                       KTabFileCtrl nDel;
					   
					   if (nDel.openfile("\\FsAi\\EquipName.txt"))
					   {
                            nDel.WriteString(nIsMianBan+1,2,strVal);
							nDel.WriteInteger(nIsMianBan+1,3,nParam);
                            nDel.Save("\\FsAi\\EquipName.txt");
                       }
                           nDel.Clear();
                   }
                }
			break;
			 case 17:
				{  //设定拾取次数
					Player[CLIENT_PLAYER_INDEX].m_Autoplay.nVal=nParam;
                }
			break;
			case 18:
				{  //指定装备
					Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoOther=nParam;
					if (nIsMianBan>6)
					    nIsMianBan=6;
					if (nIsMianBan<0)
                        nIsMianBan=0;
					    Player[CLIENT_PLAYER_INDEX].m_Autoplay.nShuXingNum=nIsMianBan;
                }
			break;
		    case 19:
				{  //自动分解
					Player[CLIENT_PLAYER_INDEX].m_Autoplay.nIsAuToFenJie=nParam;
                }
			break;
            case 20:
				{  //跑到指定地点的时间
					Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoTime=nParam;
                }
			break;
			case 21:
				{  //技能
				  if (nParam>0)
				  {
				    int nRows=Player[CLIENT_PLAYER_INDEX].m_cTask.GetSaveVal(130);
					Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoSkill[nParam-1].nIsOpen=nIsMianBan;
					Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoSkill[nParam-1].nsFactionTaskVal=nRows;
                    Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoSkill[nParam-1].nCostTime=5;

					//KTabFile nGuaSkill;
					//   if(nGuaSkill.Load("\\settings\\Ai\\skill.txt"))
					
					 if  (nRows>0)
					 {
						 char nKillKey[32];
					     sprintf(nKillKey,"skill_%d",nParam-1);
						 Player[CLIENT_PLAYER_INDEX].nGuaSkill.GetInteger(nRows+1,nKillKey,0,&Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoSkill[nParam-1].nsKillId);
					 }					    
	              }
                }
			break;
			case 22:{  //挂机模式
				    //Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoModel=nParam;
					Player[CLIENT_PLAYER_INDEX].m_Autoplay.nCurNum=nParam;
					Player[CLIENT_PLAYER_INDEX].m_Autoplay.nIsHaveDo=nParam;
					Player[CLIENT_PLAYER_INDEX].m_Autoplay.nCurStaus=nParam;
					NpcAI.nPickState=PICCK_NONE;
                }break;
			case 23:{  //自动发言
				//Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoModel=nParam;
				    Player[CLIENT_PLAYER_INDEX].m_Autoplay.nIsAutoSay=nParam;
					}break;
			case 24:{  //自动怒气
				//Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoModel=nParam;
				Player[CLIENT_PLAYER_INDEX].m_Autoplay.nIsAutoNuQi=nParam;
					}break;
			case 25:
			{//返回挂机点
				 if (nParam)
				 {
				      KTabFile nRePos;
					  char nFiledPath[128]={0};
				      sprintf(nFiledPath,"\\FsAi\\%s_Pos.txt",Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name);
			          if (nRePos.Load(nFiledPath))
					  {//已经录制了脚本
						  int nMapID=0,nX=0,nY=0;
						 
							 if  (nRePos.GetSize()<=0)
							 {
								 Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("<color=yellow>助手<color>:路线为空,不能返回,请录制路线!");
								 nRePos.Clear();
								 break;
							 }

							  nRePos.GetInteger(1,1,0,&nMapID);
							  nRePos.GetInteger(1,2,0,&nX);
							  nRePos.GetInteger(1,3,0,&nY);
						 	
							 if (nMapID<=0 || nX<=0 || nY<=0)
							 {
								Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("<color=yellow>助手<color>:路线有错误,不能返回!");
							 	nRePos.Clear();
								break;
							 }
							  nRePos.Clear();
						     SendClientCmdSunyi(nMapID,nX,nY,0);  //发送瞬移命令
					  }
					  else
					  {
						  Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("<color=yellow>助手<color>:路线还没有录制,不能返回!");
					  	  nRePos.Clear();
					  }
				 }
				 else
				 {//定点
					KIniFile nPos;
					char nFiledPath[128]={0};
				      sprintf(nFiledPath,"\\FsAi\\配置\\%s_配置.ini",Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name);
					   if (nPos.Load(nFiledPath))
					   {
						   int nMapidx,nXpos,nYpos;
						      nPos.GetInteger("BBBB","CurMap",0,&nMapidx);
						      nPos.GetInteger2("BBBB","CurPos",&nXpos,&nYpos);
							  nPos.Clear();

							  //sprintf(nFiledPath,"地图:%d,X:%d Y:%d", nMapidx, nXpos, nYpos);
							  //Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(nFiledPath);
					   }
					   else
					   {
						   Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("<color=yellow>助手<color>:配置文件不存在,不能返回!");
						   nPos.Clear();

					   }
				 }		
			}break;
			case 26:
				{  //自动使用物品
				//Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoModel=nParam;
				    Player[CLIENT_PLAYER_INDEX].m_Autoplay.nIsAutoUseItem=nParam;
					if (nParam)
					{
						/*KTabFile nCheckName;
						if (!nCheckName.Load("\\FsAi\\itemUseName.txt"))
						{
						   break;
						}*/

						ItemPos    Pos; 
						PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem(); 

				        int mItemInfo[3],mprg=0,nprg=0;
						while(pItem) 
						{ 
							ZeroMemory(mItemInfo,sizeof(mItemInfo));
							mItemInfo[0]=Item[pItem->nIdx].GetGenre();
							mItemInfo[1]=Item[pItem->nIdx].GetDetailType();
							mItemInfo[2]=Item[pItem->nIdx].GetParticular();
			                mprg=NpcSet.CheckForBit(mItemInfo,"IsUseStack",3);//是否可以叠加使用
							nprg=0;	 
							if (pItem && mprg==1 && (pItem->nPlace == pos_equiproom || pItem->nPlace == pos_immediacy) && 
							(mItemInfo[0] == item_mine || mItemInfo[0] == item_task)) 
							{ 
								char nItemName[32];
								for (int i=0;i<Player[CLIENT_PLAYER_INDEX].nCheckName.GetHeight()+1;++i)
								{	
									Player[CLIENT_PLAYER_INDEX].nCheckName.GetString(i+1,2,"封神暂无",nItemName,sizeof(nItemName));
									if (strstr(Item[pItem->nIdx].GetName(),nItemName))
									{//名称相同 就使用
										if ((mItemInfo[0] == item_task && (mItemInfo[1]==95 || mItemInfo[1]==96 || mItemInfo[1]==97)) || (mItemInfo[0] == item_mine && (mItemInfo[2]==72 || mItemInfo[2]==73||mItemInfo[2]==74)))
										{//如果是天山玉露 等东西 就检测状态
										  if ((mItemInfo[0] == item_task && mItemInfo[1]==95)  || (mItemInfo[0] == item_mine && mItemInfo[2]==72))
										  {//仙草露
											   if(GetSkillLiveTime(440)<=0)
											   {
												   nprg=1;
												   break;
											   }
										  }
										  else if ((mItemInfo[0] == item_task && mItemInfo[1]==96)  || (mItemInfo[0] == item_mine && mItemInfo[2]==73))
										  {//天山

											  if(GetSkillLiveTime(441)<=0)
											  {
												  nprg=1;
												  break;
											   }

										  }
										  else if ((mItemInfo[0] == item_task && mItemInfo[1]==97)  || (mItemInfo[0] == item_mine && mItemInfo[2]==74))
										  {//白果
											  
											  if(GetSkillLiveTime(442)<=0)
											  {
												  nprg=1;
												  break;
											  }	  
										  }
										}
										else
										{//其他东西,直接使用了
											nprg=1;
										    break;
										}	
									}
								} 

							   if (nprg)
							   {//是可以使用的
								 Pos.nPlace = pItem->nPlace; 
								 Pos.nX = pItem->nX; 
								 Pos.nY = pItem->nY; 
								 Player[CLIENT_PLAYER_INDEX].ApplyUseItem(pItem->nIdx,Pos);
							   }
								//return 1; 
							}
							
						  pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();	//下一个物品
						} 

						//nCheckName.Clear();
					}
				}													
				//Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("自动使用物品成功!");
				break;
			case 27:
				{  //技能
					if (nParam>0)
					{

						/*if  (nIsMianBan<=-1)
						{
							nRet=0;
							break;
						}*/

					   if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_FightMode==0)
						break;	//非战斗模式退出

					   /*if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Level<90)
					   {
						   Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("<color=yellow>助手<color>:等级小于90级,启用重连自动挂机失败!");
						   break;	//非战斗模式退出
					   }*/
					   
					    //if (Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoLRskill.nIsOpen==1)
					    ///	 break;//已经设置过了

						int nRows=Player[CLIENT_PLAYER_INDEX].m_cTask.GetSaveVal(130);
						Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoLRskill.nsFactionTaskVal=nRows;
						Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoLRskill.nCostTime=5;
						Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoLRskill.nsRightKillId=0;
						
						/*KTabFile nSkill;
						if(nSkill.Load("\\settings\\Ai\\skill.txt"))
						{
							if  (nRows>0)
							{
								char nKillKey[32];
								sprintf(nKillKey,"Left_%d",nIsMianBan);
								nSkill.GetInteger(nRows+1,nKillKey,0,&Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoLRskill.nsLeftKillId);
							}
							else
							{
								nSkill.Clear();
							}
						}
						nSkill.Clear();	*/


					   KIniFile nAutoConfig;

					   char nConfig[128]={0};

					   sprintf(nConfig,"\\FsAi\\配置\\%s_配置.ini",Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name);

					   if  (!nAutoConfig.Load(nConfig))
					   { 
						  Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("<color=gyellow>助手<color>:配置不存在!");
						  break;
					   }	 
					    nAutoConfig.GetInteger("AAAA","LeftSkill_0",0,&Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoLRskill.nsLeftKillId);	
						nAutoConfig.Clear();

						if (Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoLRskill.nsLeftKillId<=0)
						{
							nRet=0;
							Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("<color=gyellow>助手<color>:请设置左键技能,点击快捷技能栏即可!");
							break;
						}

						if (Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoLRskill.nsLeftKillId!=Player[CLIENT_PLAYER_INDEX].GetLeftSkill())//获取当前的左键技能
						{//左键技能不相同
							if  (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetLevel(Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoLRskill.nsLeftKillId)<=0)
								break;
							    Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoLRskill.nIsOpen=1;
								Player[CLIENT_PLAYER_INDEX].SetLeftSkill(Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoLRskill.nsLeftKillId,TRUE);  //设置左键技能同步到服务器
						}
						else
						 nRet=0;		
					}
                }
			break;
			case 28:
				{  //秒杀蓝绿怪
					Player[CLIENT_PLAYER_INDEX].m_Autoplay.nIsNoBlue=nParam;
                }
				break;
			case 29:
				{  //跟随队长
					Player[CLIENT_PLAYER_INDEX].m_Autoplay.nIsFollow=nParam;
					/*KUiPlayerTeam* pTeam ;
					memset(&pTeam, 0, sizeof(KUiPlayerTeam)); //	功能：初始化结构数据
					

					nRet = Player[CLIENT_PLAYER_INDEX].m_cTeam.GetInfo(pTeam); //获取队伍队长的信息
					*/
					//char nMsg[64];
					//sprintf(nMsg,"--A队长DWID:%d,入队:%d--",g_Team[0].m_nCaptain,Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag);
					//Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(nMsg);

					if (nParam && Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag && Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFigure ==TEAM_MEMBER)
					{//已经入队 并且是队员
						  if (g_Team[0].m_nCaptain>-1)
						  {
							  int nNpcIdx=NpcSet.SearchID((DWORD)g_Team[0].m_nCaptain);
							  //sprintf(nMsg,"--B队长DWID:%d,索引:%d--",g_Team[0].m_nCaptain,nNpcIdx);
							  //Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(nMsg);
							  if (nNpcIdx>0)
							  {//队长在线的
								  if (Npc[nNpcIdx].m_bLockNpcDwID>0)
								  {
									   int nSkillMagic = 0;
									       nSkillMagic = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.FindSame(Player[CLIENT_PLAYER_INDEX].GetLeftSkill());
									   Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SetActiveSkill(nSkillMagic); 
									   Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nPeopleIdx   =  NpcSet.SearchID(Npc[nNpcIdx].m_bLockNpcDwID);
								  
								  }
								  else
								       Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nPeopleIdx	= nNpcIdx;

								   if (Npc[nNpcIdx].m_Doing == do_sit)
								   {
									   if (!Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_bRideHorse)
									   {
									      if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Doing != do_sit)
										  { 
									         Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SendSerCommand(do_sit);
                                             SendClientCmdSit(TRUE);
										  }   
									   }
								   } 
								 //sprintf(nMsg,"跟随队长:%d....",Npc[nNpcIdx].m_bLockNpcDwID);
								 //Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(nMsg);
							  }
						  }
					}
                }
				break;
			case 30:
				{  //逃离
					Player[CLIENT_PLAYER_INDEX].m_Autoplay.nIsFell=nParam;
                }
				break;

            }
        } 
        break; 
		case GOI_ZHUANPAN:
			{  //
				KUiZhuanpanInfo* pInfo = (KUiZhuanpanInfo*)uParam;  
				  int nPlyerid=Player[CLIENT_PLAYER_INDEX].GetPlayerIndex();
                  pInfo->nCJcishu=Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CJtaskID; //抽奖专用
                  pInfo->nIsVip=Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_IsVip;
				  snprintf(pInfo->nPlayerName, sizeof(pInfo->nPlayerName),"%s",Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name);
					/*KSystemMessage	sMsg;
					sprintf(sMsg.szMessage, "提示:(%s),您的抽奖次数为<color=green>(%d)<color>次!","亲",pInfo->nCJcishu);
					sMsg.eType = SMT_NORMAL;
					sMsg.byConfirmType = SMCT_NONE;
					sMsg.byPriority = 0;
					sMsg.byParamSize = 0;
					sMsg.nMsgLen=TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
					CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);*/	
			}
			 break;
	case GOI_FINDPOS:   //寻路！没有完成！
		{
		} 
        break; 

    case GOI_WEAR_EQUIP:  //穿装备！
		//to do: waiting for...
		if (uParam)
		{
			KUiObjAtRegion* pInfo = (KUiObjAtRegion*) uParam;
		}
		break;

	//施展武功/技能
	//uParam = (KUiGameObject*)pInfo -> 技能数据
	case GOI_USE_SKILL:	
		if (uParam)
		{
			//to do:wating for...
			KUiGameObject* pInfo = (KUiGameObject*)uParam;
		}
		break;

	//设置立即技能
	//uParam = (KUiGameObject*)pSKill, 技能信息
	//nParam = 立即位置，0表示为左键技能，1为右键
	case GOI_SET_IMMDIA_SKILL:
		if (uParam)
		{
			KUiGameObject* pSkill = (KUiGameObject*)uParam;
			if (nParam == 0)
			{//左键技能
				if ( (int)pSkill->uId > 0 )
				{
					Player[CLIENT_PLAYER_INDEX].SetLeftSkill((int)pSkill->uId,nIsMianBan);  //同步到服务器
				}
				else if ((int)(pSkill->uId) == -1) //设置当前物理技能为左技能  
				{
					int nDetailType = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetWeaponType();
					int nParticularType = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetWeaponParticular();
					
					//近身武器
					if (nDetailType == 0)
					{
						Player[CLIENT_PLAYER_INDEX].SetLeftSkill(g_nMeleeWeaponSkill[nParticularType]);
					}//暗器
					else if (nDetailType == 1)
					{
						Player[CLIENT_PLAYER_INDEX].SetLeftSkill(g_nRangeWeaponSkill[nParticularType]);
					}//空手
					else if (nDetailType == -1)
					{
						Player[CLIENT_PLAYER_INDEX].SetLeftSkill(g_nHandSkill);
					}
					
				}
			}
			else if (nParam == 1)
			{
				if ((int)pSkill->uId > 0)
					Player[CLIENT_PLAYER_INDEX].SetRightSkill((int)pSkill->uId);
				else if ((int)(pSkill->uId) == -1)
				{
					int nDetailType = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetWeaponType();
					int nParticularType = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetWeaponParticular();
					
					if (nDetailType == 0)
					{
						Player[CLIENT_PLAYER_INDEX].SetRightSkill(g_nMeleeWeaponSkill[nParticularType]);
					}
					else if (nDetailType == 1)
					{
						Player[CLIENT_PLAYER_INDEX].SetRightSkill(g_nRangeWeaponSkill[nParticularType]);
					}
					else if (nDetailType == -1)
					{
						Player[CLIENT_PLAYER_INDEX].SetRightSkill(g_nHandSkill);
					}
				}
			}
		}
		break;

	//增强一种技能，，一次加一点
	//uParam = 技能类属
	//nParam = (uint)技能id
	case GOI_TONE_UP_SKILL:
		{
		  Player[CLIENT_PLAYER_INDEX].ApplyAddSkillLevel((int)nParam, 1);
		}
		break;
	//增强一些属性的值，一次加一点
	//uParam = 表示要增强的是哪个属性，取值为UI_PLAYER_ATTRIBUTE的梅举值之一
	case GOI_TONE_UP_ATTRIBUTE:
		if (nParam>0 && nParam<=10)
		{
		  switch (uParam)
		  { 
		   case UIPA_STRENGTH:		//力量
			Player[CLIENT_PLAYER_INDEX].ApplyAddBaseAttribute(0, nParam);
			break;
		   case UIPA_DEXTERITY:	//敏捷
			Player[CLIENT_PLAYER_INDEX].ApplyAddBaseAttribute(1, nParam);
			break;		
		   case UIPA_VITALITY:		//活力
			Player[CLIENT_PLAYER_INDEX].ApplyAddBaseAttribute(2, nParam);
			break;
		   case UIPA_ENERGY:		//精力
			Player[CLIENT_PLAYER_INDEX].ApplyAddBaseAttribute(3, nParam);
			break;		
		  } 
		}
		else
		{
           Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("提示:请设置每次加点数(1--10)之间!");	   
		}
		break; 
    case GOI_GENGXIN_CURDATA:
	{
          SendClientUpData();
	}
	break; 
	case GOI_RENWU_UP_PIC:
	   if (nParam==0)
	   {
		   KUiPlayerPicInfo *nPic = (KUiPlayerPicInfo *)uParam;
		   //char nstrPath[256]={0};
		   Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SetstrInfo(STR_PICTATH_CLIENT,nPic->nPicPath);

		   //sprintf(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_PicPath,nPic->nPicPath);   
		   //Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_PicPath);
	   } 
	   else if (nParam==1)
	   {
		    KUiPlayerPicInfo *nPic = (KUiPlayerPicInfo *)uParam;
			char nstrPath[256]={0};
			KIniFile nPci;
            if (nPci.Load("\\config.ini"))
			{
				nPci.GetString("Client","nPic","",nstrPath,sizeof(nstrPath));	
			}
            nPci.Clear();
			snprintf(nPic->nPicPath, sizeof(nPic->nPicPath), "%s", nstrPath);
			Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SetstrInfo(STR_PICTATH_CLIENT,nstrPath);
            
			//Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(nPic->nPicPath);
	   }
	   else if (nParam==2)
	   {
		   KUiPlayerPicInfo *nPic = (KUiPlayerPicInfo *)uParam;
		   KTabFile nSpr;
		   char nSprPath[125],nSprName[64];
		   nSpr.Load("\\Settings\\HonorSetting.txt");
		   nSpr.GetString(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_btRankFFId+1,"HONORLINK","",nSprName,sizeof(nSprName));
		   sprintf(nPic->nPicPath,"\\spr\\skill\\others\\%s",nSprName);
		   nPic->nSprRankid=Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_btRankFFId+1;
	       nSpr.Clear();	  
	   }
		break;
	//答应/拒绝交易请求
	//uParam = (KUiPlayerItem*)pRequestPlayer 发出请求的玩家
	//nParam = (int)(bool)bAccept 是否接受请求
	case GOI_TRADE_INVITE_RESPONSE:
		if (uParam)
		{
			KTrade::ReplyInvite(((KUiPlayerItem*)uParam)->nIndex, nParam);
		}
		break;

	//增减一个欲卖出的物品
	//uParam = (KUiObjAtRegion*) pObject -> 物品信息，其中坐标信息为在交易界面中的坐标
	//nParam = bAdd -> 0值表示减少，1值表示增加
	//Remark : 如果物品是金钱的话，则KUiObjAtRegion::Obj::uId表示把金钱额调整为这个值，且nParam无意义。
	case GOI_TRADE_DESIRE_ITEM:
		if (uParam)
		{
			KUiObjAtRegion* pInfo = (KUiObjAtRegion*) uParam;
			if (pInfo->Obj.uGenre != CGOG_MONEY)
				break;
			Player[CLIENT_PLAYER_INDEX].TradeMoveMoney(pInfo->Obj.uId);
		}
		break;

	//有无交易意向
	//nParam = bWilling
	case GOI_TRADE_WILLING:
		if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetMenuState() == PLAYER_MENU_STATE_TRADEOPEN)
		{
			Player[CLIENT_PLAYER_INDEX].TradeApplyClose();
		}
		else
		{
			Player[CLIENT_PLAYER_INDEX].TradeApplyOpen((char*)uParam, nParam);
		}
		break;

	//锁定交易
	//nParam = (int)(book)bLock 是否锁定
	case GOI_TRADE_LOCK:
		if ( !Player[CLIENT_PLAYER_INDEX].CheckTrading() )
			break;
		if (Player[CLIENT_PLAYER_INDEX].m_cTrade.m_nTradeLock)
			Player[CLIENT_PLAYER_INDEX].TradeApplyLock(0);
		else
			Player[CLIENT_PLAYER_INDEX].TradeApplyLock(1);
		break;
	//交易
	case GOI_TRADE:
		if ( !Player[CLIENT_PLAYER_INDEX].CheckTrading() )
			break;
		if (Player[CLIENT_PLAYER_INDEX].m_cTrade.m_nTradeLock != 1 || Player[CLIENT_PLAYER_INDEX].m_cTrade.m_nTradeDestLock != 1)
			break;
		if (Player[CLIENT_PLAYER_INDEX].m_cTrade.m_nTradeState == 0)
		{
			Player[CLIENT_PLAYER_INDEX].TradeDecision(1);		// 交易确定
		}
		else
		{
			Player[CLIENT_PLAYER_INDEX].TradeDecision(2);		// 交易确定取消
		}
		break;

	//交易取消
	case GOI_TRADE_CANCEL:
		if ( !Player[CLIENT_PLAYER_INDEX].CheckTrading() )
			break;
		Player[CLIENT_PLAYER_INDEX].TradeDecision(0);		// 交易取消
		break;

	//查询是否可以丢某个东西到游戏窗口
	//uParam = (KUiGameObject*)pObject -> 物品信息
	//nParam = 被拖动东西的当前坐标（绝对坐标），横坐标在低16位，纵坐标在高16位。(像素点坐标)
	//Return = 是否可以放下
	case GOI_DROP_ITEM_QUERY:  //还没有完成！！
		//to do : waiting for...
		break;

	case GOI_SEND_MSG:  //没有用到这个 参数
		if (uParam && nParam)
			Player[CLIENT_PLAYER_INDEX].SendChat((KUiMsgParam*)nParam, (char*)uParam);
		break;
	case GOI_SET_SEND_CHAT_CHANNEL:	//设置当前发送消息的频道
		if (uParam)
		{
			KUiChatChannel* pChannelInfo = (KUiChatChannel*)uParam;// pChannelInfo 要设置的频道的信息
			Player[CLIENT_PLAYER_INDEX].m_cChat.SetCurChannel(pChannelInfo->nChannelNo, pChannelInfo->uChannelId, pChannelInfo->nChannelIndex);
		}
		break;
	// 设置当前发送消息的频道为与某人单聊
	//uParam = (KUiPlayerItem*) pFriend
	case GOI_SET_SEND_WHISPER_CHANNEL:
		if (uParam)
		{
			if (((KUiPlayerItem*)uParam)->nIndex)
			{
				int	nIdx = NpcSet.SearchName(((KUiPlayerItem*)uParam)->Name);
				if (nIdx > 0)
				{
					Player[CLIENT_PLAYER_INDEX].m_cChat.SetCurChannel(CHAT_CUR_CHANNEL_SCREENSINGLE, Npc[nIdx].m_dwID, 0);
				}
				else
				{
					KSystemMessage	sMsg;
					sprintf(sMsg.szMessage, MSG_CHAT_FRIEND_NOT_NEAR, ((KUiPlayerItem*)uParam)->Name);
					sMsg.eType = SMT_NORMAL;
					sMsg.byConfirmType = SMCT_NONE;
					sMsg.byPriority = 0;
					sMsg.byParamSize = 0;
					sMsg.nMsgLen=TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
					CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				}
			}
			else
			{
				KSystemMessage	sMsg;
				sprintf(sMsg.szMessage, MSG_CHAT_FRIEND_NOT_ONLINE, ((KUiPlayerItem*)uParam)->Name);
				sMsg.eType = SMT_NORMAL;
				sMsg.byConfirmType = SMCT_NONE;
				sMsg.byPriority = 0;
				sMsg.byParamSize = 0;
				sMsg.nMsgLen=TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
			}
		}
		break;

	case GOI_SET_RECEIVE_CHAT_CHANNEL://设置订阅/取消消息频道
		if (uParam)
		{
			//uParam = (KUiChatChannel*) pChannelInfo 要设置的频道的信息
			//nParam = (int)(bool) bEnable 是否订阅
			KUiChatChannel* pChannelInfo = (KUiChatChannel*)uParam;// pChannelInfo 要设置的频道的信息
			if (nParam)
			{
				Player[CLIENT_PLAYER_INDEX].m_cChat.AddChannel(pChannelInfo->nChannelNo);
			}
			else
			{
				Player[CLIENT_PLAYER_INDEX].m_cChat.SubChannel(pChannelInfo->nChannelNo);
			}
		}
		break;

	//新建聊天好友组
	//uParam = (const char*) pGroupName
	//Return = (int)(bool) bSuccessed
	case GOI_CHAT_GROUP_NEW:
		Player[CLIENT_PLAYER_INDEX].m_cChat.AddTeam(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name, (char*)uParam);
		CoreDataChanged(GDCNI_CHAT_GROUP, 0, 0);
		break;

	//重命名聊天好友组
	//uParam = (const char*) pGroupName
	//nParam = nIndex 组索引
	//Return = (int)(bool) bSuccessed
	case GOI_CHAT_GROUP_RENAME:
		Player[CLIENT_PLAYER_INDEX].m_cChat.RenameTeam(nParam, (char*)uParam, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name);
		CoreDataChanged(GDCNI_CHAT_GROUP, 0, 0);
		break;

	//删除聊天好友组
	//nParam = nIndex 组索引
	//Return = (int)(bool) bSuccessed
	case GOI_CHAT_GROUP_DELETE:
		Player[CLIENT_PLAYER_INDEX].m_cChat.DeleteTeam(nParam, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name);
		CoreDataChanged(GDCNI_CHAT_GROUP, 0, 0);
		break;

	//新添加聊天好友
	//uParam = (KUiPlayerItem*)pFriend
	//			KUiPlayerItem::nData = 0
	case GOI_CHAT_FRIEND_ADD:
		{
			if (g_pClient)
			{
				size_t pckgsize = sizeof(tagExtendProtoHeader) + sizeof(ASK_ADDFRIEND_CMD);

				tagExtendProtoHeader* pExHeader = (tagExtendProtoHeader*)_alloca(pckgsize);
				pExHeader->ProtocolType = c2s_extendfriend;
				pExHeader->wLength = pckgsize - 1;

				ASK_ADDFRIEND_CMD* pAafCmd = (ASK_ADDFRIEND_CMD*)(pExHeader + 1);
				pAafCmd->ProtocolFamily = pf_friend;
				pAafCmd->ProtocolID = friend_c2c_askaddfriend;
				pAafCmd->pckgid = -1;
				strncpy(pAafCmd->dstrole, ((KUiPlayerItem*)uParam)->Name, _NAME_LEN);
				pAafCmd->dstrole[(_NAME_LEN) - 1] = '\0';
				g_pClient->SendPackToServer(pExHeader, pckgsize);
		
				//Player[CLIENT_PLAYER_INDEX].m_cChat.ApplyAddFriend(((KUiPlayerItem*)uParam)->uId);

				KSystemMessage	sMsg;
				sprintf(sMsg.szMessage, MSG_CHAT_APPLY_ADD_FRIEND, ((KUiPlayerItem*)uParam)->Name);
				sMsg.eType = SMT_NORMAL;
				sMsg.byConfirmType = SMCT_NONE;
				sMsg.byPriority = 0;
				sMsg.byParamSize = 0;
				sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
			}
		}
		break;
	//删除聊天好友
	//uParam = (KUiPlayerItem*)pFriend
	//			KUiPlayerItem::nData = 0
	//nParam = nGroupIndex 好友所在的组的索引
	//Return = (int)(bool) bSuccessed
	case GOI_CHAT_FRIEND_DELETE:
		{
			Player[CLIENT_PLAYER_INDEX].m_cChat.ApplyDeleteFriend(((KUiPlayerItem*)uParam)->uId, nParam, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name);

			KSystemMessage	sMsg;
			sprintf(sMsg.szMessage, MSG_CHAT_DELETE_FRIEND, ((KUiPlayerItem*)uParam)->Name);
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sMsg.nMsgLen=TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case GOI_CHAT_FRIEND_MOVE:
		{
			KUiPlayerItem	*pPlayer = (KUiPlayerItem*)uParam;
			int		nSrcTeamNo = Player[CLIENT_PLAYER_INDEX].m_cChat.GetTeamNo(pPlayer->uId);
			if (nSrcTeamNo == -1)
			{
				// 失败，如果需要发消息，在此
				break;
			}
			if (Player[CLIENT_PLAYER_INDEX].m_cChat.OneFriendChangeTeam(pPlayer->uId, nSrcTeamNo, nParam))
			{	// 成功
				CoreDataChanged(GDCNI_CHAT_GROUP, 0, 0);
			}
			else
			{
				// 失败，如果需要发消息，在此
			}
		}
		break;

	//发给好友一条讯息
	//uParam = (KUiChatMessage*)pMessage 消息数据
	//nParam = (KUiPlayerItem*)pFriend   发来讯息的好友
	case GOI_CHAT_WORDS_TO_FRIEND:
		{
			char	szSentence[MAX_SENTENCE_LENGTH];
			KUiChatMessage	*pMsg = (KUiChatMessage*)uParam;
			KUiMsgParam	sMsg;

			sMsg.eGenre = MSG_G_CHAT;
			sMsg.szName[0] = 0;
			memset(sMsg.cChatPrefix, 0, sizeof(sMsg.cChatPrefix));
			sMsg.cChatPrefixLen = sizeof(unsigned int);
			sMsg.cChatPrefix[0] = ((char*)(&pMsg->uColor))[3];
			sMsg.cChatPrefix[1] = ((char*)(&pMsg->uColor))[2];
			sMsg.cChatPrefix[2] = ((char*)(&pMsg->uColor))[1];
			sMsg.cChatPrefix[3] = ((char*)(&pMsg->uColor))[0];

			if (pMsg->nContentLen >= MAX_SENTENCE_LENGTH)
			{
				memcpy(szSentence, pMsg->szContent, MAX_SENTENCE_LENGTH - 1);
				szSentence[MAX_SENTENCE_LENGTH - 1] = 0;
				sMsg.nMsgLength = MAX_SENTENCE_LENGTH - 1;
			}
			else
			{
				memcpy(szSentence, pMsg->szContent, pMsg->nContentLen);
				szSentence[pMsg->nContentLen] = 0;
				sMsg.nMsgLength = pMsg->nContentLen;
			}

			Player[CLIENT_PLAYER_INDEX].m_cChat.QQSendSentence(
				((KUiPlayerItem*)nParam)->uId,
				((KUiPlayerItem*)nParam)->nIndex, &sMsg, szSentence);
		}
		break;

	//对别人要加自己为好友的回复
	//uParam = (KUiPlayerItem*)pRequestPlayer 发出请求的玩家
	//nParam = (int)(bool)bAccept 是否接受请求
	case GOI_CHAT_FRIEND_INVITE:
		if (uParam)
		{
			if (g_pClient)
			{
				size_t pckgsize = sizeof(tagExtendProtoHeader) + sizeof(REP_ADDFRIEND_CMD);

				tagExtendProtoHeader* pExHeader = (tagExtendProtoHeader*)_alloca(pckgsize);
				pExHeader->ProtocolType = c2s_extendfriend;
				pExHeader->wLength = pckgsize - 1;
				
				REP_ADDFRIEND_CMD* pRafCmd = (REP_ADDFRIEND_CMD*)(pExHeader + 1);
				pRafCmd->ProtocolFamily = pf_friend;
				pRafCmd->ProtocolID = friend_c2c_repaddfriend;
				pRafCmd->pckgid = -1;
				strncpy(pRafCmd->dstrole, ((KUiPlayerItem*)uParam)->Name, _NAME_LEN);
				pRafCmd->dstrole[(_NAME_LEN) - 1] = '\0';
				pRafCmd->answer = nParam ? answerAgree : answerDisagree;
				g_pClient->SendPackToServer(pExHeader, pckgsize);

			//if (nParam)
			//{
			//	Player[CLIENT_PLAYER_INDEX].ChatAddFriend(((KUiPlayerItem*)uParam)->nIndex);
			//}
			//else
			//{
			//	Player[CLIENT_PLAYER_INDEX].ChatRefuseFriend(((KUiPlayerItem*)uParam)->nIndex);
			//}
			}
		}
		break;

	case GOI_OPTION_SETTING:			//选项设置
		if (uParam == OPTION_DYNALIGHT) //刀光剑影模式
		{
			g_ScenePlace.EnableDynamicLights(nParam != 0);
			if (g_pRepresent)
				g_pRepresent->SetOption(DYNAMICLIGHT, nParam != 0);
		}
		else if (uParam == OPTION_PERSPECTIVE) //透视模式
		{
			if (g_pRepresent)
				g_pRepresent->SetOption(PERSPECTIVE, nParam != 0);
		}
		else if (uParam == OPTION_MUSIC_VALUE)
			Option.SetMusicVolume(nParam);
		else if (uParam == OPTION_SOUND_VALUE)
			Option.SetSndVolume(nParam);
		else if (uParam == OPTION_BRIGHTNESS)
			Option.SetGamma(nParam);  //亮度设置
		else if (uParam == OPTION_WEATHER)
			g_ScenePlace.EnableWeather(nParam);
		break;

	case GOI_VIEW_PLAYERITEM: //发送玩家查看装备信息协议
		{
			g_cViewItem.ApplyViewEquip(uParam);
		}
		break;
	case GOI_VIEW_PLAYERITEM_END:
		g_cViewItem.DeleteAll();
		break;
	case GOI_JINDUTIAO_CALLBACK:
		{
			if (uParam)
			{
				nJinDuClient *nJd = (nJinDuClient*)uParam;

                    SendClientJdCallBack(nJd);
			}
			else
			{//动鼠标关闭进度条

				CoreDataChanged(GDCNI_OPEN_JINDUTIAO,0,0);
			}
		}
		break;
	case GOI_PLAYER_SYSSHOP:    //通用调用脚本
		{
			if (uParam)
			{//任务自动寻路用的
				KUiTongYong *nTy = (KUiTongYong*)uParam;

				//int nRegion, nMapX, nMapY, nOffX, nOffY;
				 
                if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SubWorldIndex==g_SubWorldSet.SearchWorld(nTy->inParma))
				{//相同地图

					//SubWorld[0].Mps2Map(nTy->inParmb*32, nTy->inParmc*32, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY); //像素坐标 转化格子坐标                 
                    //GetMpsPos(&nSrcMpsX,&nSrcMpsY);
					int nSrcMpsX,nSrcMpsY,nDmap,nDistance;
					Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetMpsPos(&nSrcMpsX,&nSrcMpsY,&nDmap);
					//nSrcMpsX=Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_DesX;
					//nSrcMpsY=Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_DesY;
					nDistance = SubWorld[0].GetDistance(nSrcMpsX,nSrcMpsY,nTy->inParmb*32,nTy->inParmc*32);

					if (nDistance>=2000)
					{//相同区域 就用跑的
						//Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SendSerCommand(do_run, nTy->inParmb*16*16, nTy->inParmc*16*16);
				        //SendClientCmdRun(nTy->inParmb*16*16, nTy->inParmc*16*16,0);		
						SendClientCmdSysShop(nTy->inKind,nTy->inParma,nTy->inParmb,nTy->inParmc,nTy->inParmd);
					}
					else
					{//不同区域就飞               
						g_ScenePlace.FindPos(nTy->inParmb/8, nTy->inParmc/16, true);
					}

					//char Msg[64];
					//sprintf(Msg,"Y:x=%d-y=%d   D:x=%d-y=%d  Dis:%d",nSrcMpsX,nSrcMpsY,nTy->inParmb*32,nTy->inParmc*32,nDistance);					
                    //Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(Msg);
				}
				else
				{//不同地图，直接飞
                       SendClientCmdSysShop(nTy->inKind,nTy->inParma,nTy->inParmb,nTy->inParmc,nTy->inParmd);
				}             
			}
			else
			{
				if (nIsMianBan==0)
				{ //怒气
				   SendClientCmdSysShop(0,nParam);
			       Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentNuQi=0;
				}
				else
				{//其他类型
					SendClientCmdSysShop(nIsMianBan,nParam);
				}
			}	
		}
		break;

	case GOI_PLAYER_ACTION:
		{
			switch(uParam)
			{
			case PA_RUN:
				Player[CLIENT_PLAYER_INDEX].m_RunStatus = !Player[CLIENT_PLAYER_INDEX].m_RunStatus;
				break;
			case PA_SIT:
				if (!Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_bRideHorse)
				{
					if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Doing != do_sit)
					{
						Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SendSerCommand(do_sit);
						SendClientCmdSit(TRUE);
					}
					else
					{
						Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SendSerCommand(do_stand);
						SendClientCmdSit(FALSE);
					}
				}
				else
				{
					KSystemMessage	Msg;
					Msg.byConfirmType = SMCT_CLICK;
					Msg.eType = SMT_PLAYER;
					Msg.byPriority = 1;
					Msg.byParamSize = 0;
					snprintf(Msg.szMessage, sizeof(Msg.szMessage), "%s", "提示:骑马中!不能打坐！");
					Msg.nMsgLen=TEncodeText(Msg.szMessage, strlen(Msg.szMessage));
					CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&Msg, 0);
				}
				break;
			case PA_RIDE://骑马的信息？
				if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Doing == do_sit)
				{
					Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SendSerCommand(do_stand);
					SendClientCmdSit(FALSE);
				}//	坐着--首先要站起来才可以 骑马！！！
				
				SendClientCmdRide(); //骑马发送函数
				
				break;
			case PW_NOT_SAME: // 不能相同？
					{
						KSystemMessage	Msg;
						Msg.byConfirmType = SMCT_NONE;
						Msg.eType = SMT_NORMAL;
						Msg.byPriority = 1;
						Msg.byParamSize = 0;
						snprintf(Msg.szMessage, sizeof(Msg.szMessage), "%s", "提示:密码不能相同");
						Msg.nMsgLen=TEncodeText(Msg.szMessage, strlen(Msg.szMessage));
						CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&Msg, 0);
					}
				break;
			case PW_NOT_LONG: // mat khau ko du do dai
					{
						KSystemMessage	Msg;
						Msg.byConfirmType = SMCT_NONE;
						Msg.eType = SMT_NORMAL;
						Msg.byPriority = 1;
						Msg.byParamSize = 0;
						snprintf(Msg.szMessage, sizeof(Msg.szMessage), "%s", "提示:两次密码长度不同！");
						Msg.nMsgLen=TEncodeText(Msg.szMessage, strlen(Msg.szMessage));
						CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&Msg, 0);
					}
				break;
			case PW_ACCEPTED: // mat khau dc chap nhan
					{
						KSystemMessage	Msg;
						Msg.byConfirmType = SMCT_NONE;
						Msg.eType = SMT_NORMAL;
						Msg.byPriority = 1;
						Msg.byParamSize = 0;
						snprintf(Msg.szMessage, sizeof(Msg.szMessage), "%s", "提示:密码通过");
						Msg.nMsgLen=TEncodeText(Msg.szMessage, strlen(Msg.szMessage));
						CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&Msg, 0);
					}
				break;
			case HT_CN:
					{
						KSystemMessage	Msg;
						Msg.byConfirmType = SMCT_NONE;
						Msg.eType = SMT_NORMAL;
						Msg.byPriority = 1;
						Msg.byParamSize = 0;
						snprintf(Msg.szMessage, sizeof(Msg.szMessage), "%s", "提示:<color=yellow>功能完善中C！<color>");
						Msg.nMsgLen=TEncodeText(Msg.szMessage, strlen(Msg.szMessage));
						CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&Msg, 0);
					}
				break;
			case KD_VP:
					{
						KSystemMessage	Msg;
						Msg.byConfirmType = SMCT_NONE;
						Msg.eType = SMT_NORMAL;
						Msg.byPriority = 1;
						Msg.byParamSize = 0;
						snprintf(Msg.szMessage, sizeof(Msg.szMessage), "%s", "提示:材料不足！");
						Msg.nMsgLen=TEncodeText(Msg.szMessage, strlen(Msg.szMessage));
						CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&Msg, 0);
					}
				break;
			case CN_GH:
					{
						KSystemMessage	Msg;
						Msg.byConfirmType = SMCT_NONE;
						Msg.eType = SMT_NORMAL;
						Msg.byPriority = 1;
						Msg.byParamSize = 0;
						snprintf(Msg.szMessage, sizeof(Msg.szMessage), "%s", "提示:<color=yellow>锁频情况下，不能使用界面！请解锁！<color>");
						Msg.nMsgLen=TEncodeText(Msg.szMessage, strlen(Msg.szMessage));
						CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&Msg, 0);
					}
				break;
			case HAVE_SKILL:
				{
					KSystemMessage	Msg;
					Msg.byConfirmType = SMCT_NONE;
					Msg.eType = SMT_NORMAL;
					Msg.byPriority = 1;
					Msg.byParamSize = 0;
					snprintf(Msg.szMessage, sizeof(Msg.szMessage), "%s", "提示:<color=yellow>快捷栏已有同类技能!<color>");
					Msg.nMsgLen=TEncodeText(Msg.szMessage, strlen(Msg.szMessage));
					CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&Msg, 0);
				}
				break;	
			case MO_AUTO:
				{ 

						KSystemMessage	Msg;
						Msg.byConfirmType = SMCT_NONE;
						Msg.eType = SMT_NORMAL;
						Msg.byPriority = 1;
						Msg.byParamSize = 0;
						snprintf(Msg.szMessage, sizeof(Msg.szMessage), "%s", "提示:功能完善中A");
						Msg.nMsgLen=TEncodeText(Msg.szMessage, strlen(Msg.szMessage));
						CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&Msg, 0);

				}
				break;
			case TAT_AUTO:
				
					{
						KSystemMessage	Msg;
						Msg.byConfirmType = SMCT_NONE;
						Msg.eType = SMT_NORMAL;
						Msg.byPriority = 1;
						Msg.byParamSize = 0;
						snprintf(Msg.szMessage, sizeof(Msg.szMessage), "%s", "提示:功能完善中B");
						Msg.nMsgLen=TEncodeText(Msg.szMessage, strlen(Msg.szMessage));
						CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&Msg, 0);
					}
				
				break;
	     	case CESHI_AUTO:
				
				{
					KSystemMessage	Msg;
					Msg.byConfirmType = SMCT_NONE;
					Msg.eType = SMT_NORMAL;
					Msg.byPriority = 1;
					Msg.byParamSize = 0;
					snprintf(Msg.szMessage, sizeof(Msg.szMessage), "%s", "提示:自动挂机测试开始！");
					Msg.nMsgLen=TEncodeText(Msg.szMessage, strlen(Msg.szMessage));
					CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&Msg, 0);
				}
				
				break;
			case ERORR_AUTO:
					{
						KSystemMessage	Msg;
						Msg.byConfirmType = SMCT_NONE;
						Msg.eType = SMT_NORMAL;
						Msg.byPriority = 1;
						Msg.byParamSize = 0;
						snprintf(Msg.szMessage, sizeof(Msg.szMessage), "%s", "提示:功能完善中E...");
						Msg.nMsgLen=TEncodeText(Msg.szMessage, strlen(Msg.szMessage));
						CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&Msg, 0);
					}
				break;
			case EX_BOX:
				{	
					int nExbox = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_ExBoxId;
					if(nExbox == 0)
					{
						Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("提示:<color=cyan>扩展箱功能还没有开启,请到巴陵县(沈九)处开启!<color>");
					}
					//else 
					CoreDataChanged(GDCNI_OPEN_EX_BOX, nExbox, NULL);
					
					
				}
				break;
			case EX_BOX2:
				{		
					int nExbox = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_ExBoxId;
					if(nExbox == 1)
					{
						Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("提示:<color=cyan>扩展箱2还没有开启!<color>");
					}
					//else if(nExbox == 1 || nExbox == 2 || nExbox == 3)
					CoreDataChanged(GDCNI_OPEN_EX_BOX2, nExbox, NULL);
					
				}
				break;
			case EX_BOX3:
				{	
					
					int nExbox = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_ExBoxId;
					if(nExbox == 2 || nExbox == 1)
					{
						Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("提示:<color=cyan>扩展箱3还没有开启!<color>");
					}
					//else if(nExbox == 1 || nExbox == 2 || nExbox == 3)
					CoreDataChanged(GDCNI_OPEN_EX_BOX3, nExbox, NULL);
					
					
				}
				break;
			case ITEMEX:
				{	
					int nItemEX = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_ExItemId;

					if(nItemEX == 0)
					{
						Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("提示:<color=cyan>子母袋还未开启,请咨询80沈九!<color>");
					}
					//else if(nItemEX >= 1) 
					CoreDataChanged(GDCNI_OPEN_ITEMEX, nItemEX, NULL);//打开子母袋功能
				}
				break;
			case GET_BOX_OPEN:
				{
					nRet = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_ExBoxId;
				}
				break;
			case GET_BOXEX_OPEN:
				{
					nRet = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_ExItemId;
				}
				break;
			case OPEN_MARKET://打开奇珍阁
				if(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_FightMode == 0)
				{
					//	CoreDataChanged(GDCNI_OPEN_MARKET, NULL, TRUE);
					//if (nParam>-1)
                     //  SendClientOpenMarket(nParam);
					//else
						break;
				}
				else
				{	
					nRet=0;
					Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("提示:<color=gyellow>商城需要在安全区才可以打开<color>!");
				}
				break;
			}
		}
		break;

	case GOI_PK_SETTING: //PK设置
					Player[CLIENT_PLAYER_INDEX].m_cPK.ApplySetNormalPKState(nParam);
				break;
	//uParam = (KUiPlayerItem*) pTarget
	case GOI_REVENGE_SOMEONE: //仇杀某人？
		if (uParam)
		{
			KUiPlayerItem	*pTarget = (KUiPlayerItem*)uParam;
			Player[CLIENT_PLAYER_INDEX].m_cPK.ApplyEnmityPK(pTarget->uId);
		}
		break;
	//跟随某人
	//uParam = (KUiPlayerItem*) pTarget	跟随目标
	case GOI_FOLLOW_SOMEONE:
		if (uParam)
		{
			KUiPlayerItem	*pTarget = (KUiPlayerItem*)uParam;
			if (Npc[pTarget->nIndex].m_Kind == kind_player)
				Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nPeopleIdx = pTarget->nIndex; //锁定某人
		}
		break;

	//显示各玩家人名
	//nParam = (int)(bool)bShow	是否显示
	case GOI_SHOW_PLAYERS_NAME:
		NpcSet.SetShowNameFlag(nParam);
		break;
	//显示各玩家生命
	//nParam = (int)(bool)bShow	是否显示
	case GOI_SHOW_PLAYERS_LIFE:
		NpcSet.SetShowLifeFlag(nParam);
		break;
	//显示各玩家内力
	//nParam = (int)(bool)bShow	是否显示
	case GOI_SHOW_PLAYERS_MANA:
		NpcSet.SetShowManaFlag(nParam);
		break;
	default:
		nRet = 0;
		break;
	}

	return nRet;
}

//--------------------------------------------------------------------------
//	功能：发送输入设备的输入操作消息 键盘线程
//--------------------------------------------------------------------------
void KCoreShell::ProcessInput(unsigned int uMsg, unsigned int uParam, int nParam)
{
	Player[CLIENT_PLAYER_INDEX].ProcessInputMsg(uMsg, uParam, nParam);
}

int KCoreShell::FindSelectNPC(int x, int y, int nRelation, bool bSelect, void* pReturn, int& nKind)
{
	Player[CLIENT_PLAYER_INDEX].FindSelectNpc(x, y, nRelation);

	int nT = Player[CLIENT_PLAYER_INDEX].GetTargetNpc();

	if (!bSelect)
		Player[CLIENT_PLAYER_INDEX].SetTargetNpc(0);

	if (nT > 0)
	{
		if (pReturn)
		{
			KUiPlayerItem* p = (KUiPlayerItem*)pReturn;
			strncpy(p->Name, Npc[nT].Name, 32);
			p->Name[(32) - 1] = '\0';
			p->nIndex = Npc[nT].m_Index;
			p->uId = Npc[nT].m_dwID;
			p->nData = Npc[nT].GetMenuState();
		}
		nKind = Npc[nT].m_Kind;
		return true;
	}
	return false;
}

int KCoreShell::FindSelectObject(int x, int y, bool bSelect, int& nObjectIdx, int& nKind)
{
	Player[CLIENT_PLAYER_INDEX].FindSelectObject(x, y);
	int nT = Player[CLIENT_PLAYER_INDEX].GetTargetObj();
	
	if (!bSelect)
		Player[CLIENT_PLAYER_INDEX].SetTargetObj(0);

	if (nT > 0)
	{
		nObjectIdx = nT;
		nKind = Object[nT].m_nKind;
		return true;
	}
	return false;
}
//赋值结构 
int KCoreShell::FindPlayerIdx(char* Name, void* pReturn, int& nKind)
{
	if (Name == NULL || Name[0] == 0)
		return false;

	for (int nT = 0; nT < MAX_NPC; nT++)
	{
		if	(strcmp(Npc[nT].Name, Name) == 0)  //对比名字 是否相同 
		{
			if (pReturn)
			{
				KUiPlayerItem* p = (KUiPlayerItem*)pReturn;
				strncpy(p->Name, Npc[nT].Name, 32);
				p->Name[(32) - 1] = '\0';
				p->nIndex = Npc[nT].m_nsPlayerIdx;//Npc[nT].GetNpcPepoleID();
				p->uId    = Npc[nT].m_dwID;
				p->nData  = Npc[nT].GetMenuState();
			}
			nKind = Npc[nT].m_Kind;
			return true;
		}
	}
	return false;
}

int KCoreShell::FindSpecialNPC(char* Name, void* pReturn, int& nKind)
{
	if (Name == NULL || Name[0] == 0)
		return false;
	for (int nT = 0; nT <MAX_NPC; nT++)
	{
		if	(strcmp(Npc[nT].Name, Name) == 0)  //对比名字 是否相同 
		{
			if (pReturn)
			{
				KUiPlayerItem* p = (KUiPlayerItem*)pReturn;
				strncpy(p->Name, Npc[nT].Name, 32);
				p->Name[(32) - 1] = '\0';
				p->nIndex = Npc[nT].m_Index;
				p->uId    = Npc[nT].m_dwID;
				p->nData  = Npc[nT].GetMenuState();
			}
			nKind = Npc[nT].m_Kind;
			return true;
		}
	}
	return false;
}

//自己头顶聊天信息显示
int KCoreShell::ChatSpecialPlayer(void* pPlayer, const char* pMsgBuff, unsigned short nMsgLength) //自己头顶聊天
{
	KUiPlayerItem* p = (KUiPlayerItem*)pPlayer;
	if (p)
	{
		if (p->nIndex >= 0 && p->nIndex < MAX_NPC)
		{
			int nTalker = p->nIndex;
			if (Npc[nTalker].m_Kind == kind_player &&
				Npc[nTalker].m_dwID == p->uId)
			{
				Npc[nTalker].SetChatInfo(p->Name, pMsgBuff, nMsgLength); //设置
				return true;
			}
		}
	}

	return false;
}

void KCoreShell::ApplyAddTeam(void* pPlayer)
{
	KUiPlayerItem* p = (KUiPlayerItem*)pPlayer;
	if (p)
	{
		if (p->nIndex >= 0 && p->nIndex < MAX_NPC && !Player[CLIENT_PLAYER_INDEX].CheckTrading())
		{
			Player[CLIENT_PLAYER_INDEX].ApplyAddTeam(p->nIndex);
		}
	}
}

void KCoreShell::TradeApplyStart(void* pPlayer)
{
	KUiPlayerItem* p = (KUiPlayerItem*)pPlayer;
	if (p)
	{
		if (p->nIndex >= 0 && p->nIndex < MAX_NPC && !Player[CLIENT_PLAYER_INDEX].CheckTrading())
		{
			Player[CLIENT_PLAYER_INDEX].TradeApplyStart(p->nIndex);
		}
	}
}



int KCoreShell::AutoUseSkill()
{
	if (Player[CLIENT_PLAYER_INDEX].CheckTrading())
		return 0;

/*	int nX = x;
	int nY = y;
	int nZ = 0;
	g_ScenePlace.ViewPortCoordToSpaceCoord(nX, nY, nZ);*/

	int nIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;

    if (!Npc[nIndex].m_AutoplayId || Npc[nIndex].m_FightMode==0)  //主人为非战斗模式 
    {
      return 0;
    }
     
	int mcActiveSkillID=0;

	if (Npc[nIndex].IsCanInput()) //人物AI开启的话
	{
		int nLoop=Player[CLIENT_PLAYER_INDEX].m_Autoplay.nSkilloop++;
		
		if  (nLoop>200000)
			Player[CLIENT_PLAYER_INDEX].m_Autoplay.nSkilloop=0;

		 for (int i=0;i<4;++i)
		 {
		   int mIsoPen=Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoSkill[i].nIsOpen;
		   int mSkillId=Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoSkill[i].nsKillId;

           //char msg[64]={0};
		   //sprintf(msg,"技能(%d),ID(%d),开关(%d)",i,mSkillId,mIsoPen);
		   //Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(msg);
		   if (mSkillId<=0)
			   continue;

           if (mIsoPen==1)
		   {//如果是开启的 
		      //int nIdx = 0;
			  //nIdx= Npc[nIndex].m_SkillList.FindSame(mSkillId);

              int nLevel = Npc[nIndex].m_SkillList.GetLevel(mSkillId);
				//	_ASSERT(nLevel >= 0);
			  if (nLevel<=0)
				  continue;

			  KSkill * pISkill = (KSkill *) g_SkillManager.GetSkill(mSkillId,nLevel);
		      if (!pISkill) 
                  continue;

		      if (pISkill->IsAura())  
			      continue;

			 //int nAttackRange = pISkill->GetAttackRadius();
		     //int nTargetIdx = 0;
             //KMagicAttrib* pMagicData = NULL;
	         //pMagicData = pISkill->GetStateAttribs();//状态数据
             //if (!pMagicData)
		     //continue;

             int nLeftime=Npc[nIndex].GetSkillLeftTime(mSkillId);

			 //sprintf(msg,"技能(%d),ID(%d),时间(%d)",i,mSkillId,nLeftime);
		     //Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(msg);

			 if (nLeftime>0)
				 continue;

		     if (pISkill->IsTargetSelf())
	         {//如果是对自己有用的 并且 是时间到了的

				 int nCostTimes=Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoSkill[i].nCostTime;

                 //sprintf(msg,"使用技能(%d),ID(%d),时间(%d),循环(%d),间隔(%d)成功",i,mSkillId,nLeftime,nLoop,nCostTimes);
		         //Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(msg);

				if (!Player[CLIENT_PLAYER_INDEX].m_Autoplay.nIsUseSkill && nLoop>nCostTimes*18 && nLoop%(nCostTimes*18)==0)
				{//开始使用技能
//====================客户端检测是否能发技能==============================
		          /*if ((!Npc[nIndex].m_SkillList.CanCast(mSkillId, SubWorld[Npc[nIndex].m_SubWorldIndex].m_dwCurrentTime))
			          ||(!Npc[nIndex].Cost(pISkill->GetSkillCostType(), pISkill->GetSkillCost(&Npc[nIndex]), TRUE)))
	            	{
			           Npc[nIndex].m_nPeopleIdx = 0;
			           Player[CLIENT_PLAYER_INDEX].SetTargetNpc(0);
					   Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoSkill[i].nSkilloop=0;
			           return 0;
            		}*/
					 Player[CLIENT_PLAYER_INDEX].m_Autoplay.nIsUseSkill =TRUE;

					 int nIdx = 0;
		                 nIdx = Npc[nIndex].m_SkillList.FindSame(mSkillId);

		             Npc[nIndex].SetActiveSkill(nIdx);
				   //Npc[nIndex].m_ActiveSkillID=0;
//=========================================================================
				     Npc[nIndex].m_nPeopleIdx = 0;
			         Player[CLIENT_PLAYER_INDEX].SetTargetNpc(0);
			         Npc[nIndex].SendSerCommand(do_skill,mSkillId,-1,nIndex);
				     SendClientCmdSkill(mSkillId,-1,Npc[nIndex].m_dwID);
				     Player[CLIENT_PLAYER_INDEX].m_Autoplay.nSkilloop=0;
					 Player[CLIENT_PLAYER_INDEX].m_Autoplay.nIsUseSkill =FALSE;
					 char nMsg[64]={0};
					 snprintf(nMsg, sizeof(nMsg),"<color=gyellow>助手<color>:使用[%s]技能。",pISkill->GetSkillName());
					 Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(nMsg);

					 for (int i =0; i<500;++i)
					 {

					 } 
					 break;
				}
	         }
		   }
		 }   
	}
	else
	{
		return 0;
	}

	///Npc[nIndex].m_nPeopleIdx = 0;

	return 1;
}


int KCoreShell::UseSkill(int x, int y, int nSkillID)
{
	if (Player[CLIENT_PLAYER_INDEX].CheckTrading())
		return 0;

	int nX = x;
	int nY = y;
	int nZ = 0;
	g_ScenePlace.ViewPortCoordToSpaceCoord(nX, nY, nZ);
	int nIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;

/*
#ifdef _SERVER
	if (Npc[nIndex].m_randmove.nValue[0] && Npc[nIndex].m_randmove.nTime>0) //混乱状态不能发招
		return 0;
#else
	if (Npc[nIndex].m_randmove.nTime>0)                          //混乱状态不能发招
		return 0;
#endif
*/

	if (Npc[nIndex].IsCanInput()) //人物AI开启的话
	{
		int nIdx = 0;
		
		nIdx = Npc[nIndex].m_SkillList.FindSame(nSkillID);
	//	g_DebugLog("[skill]Active");
		Npc[nIndex].SetActiveSkill(nIdx); 
	}
	else
	{
	//	g_DebugLog("[skill]return");
		return 0;
	}

	if (Npc[nIndex].m_ActiveSkillID > 0)
	{
		ISkill * pISkill =  g_SkillManager.GetSkill(Npc[nIndex].m_ActiveSkillID, 1);
		if (!pISkill) 
            return 0;

		if (pISkill->IsAura())
			return 0;

		int nAttackRange = pISkill->GetAttackRadius();

		int nTargetIdx = 0;
		//按照Object / Enemy / Ally 的优先级找到需要打的对象id
		
		if (pISkill->IsTargetAlly())
		{
			Player[CLIENT_PLAYER_INDEX].FindSelectNpc(x, y, relation_ally);
			if (Player[CLIENT_PLAYER_INDEX].GetTargetNpc())
			{
				nTargetIdx = Player[CLIENT_PLAYER_INDEX].GetTargetNpc();
			}
		}

		if (pISkill->IsTargetEnemy())
		{
			Player[CLIENT_PLAYER_INDEX].FindSelectNpc(x, y, relation_enemy);
			if (Player[CLIENT_PLAYER_INDEX].GetTargetNpc())
			{
				nTargetIdx = Player[CLIENT_PLAYER_INDEX].GetTargetNpc();
			}
		}

		if (pISkill->IsTargetObj())
		{
			Player[CLIENT_PLAYER_INDEX].FindSelectObject(x, y);
			if (Player[CLIENT_PLAYER_INDEX].GetTargetObj())
			{
				nTargetIdx = Player[CLIENT_PLAYER_INDEX].GetTargetObj();
			}
		}

		if (pISkill->IsTargetOther())
		{
			Player[CLIENT_PLAYER_INDEX].FindSelectNpc(x, y, relation_dialog);

			if (Player[CLIENT_PLAYER_INDEX].GetTargetNpc())
			{
				nTargetIdx = Player[CLIENT_PLAYER_INDEX].GetTargetNpc();
			}
		}
        
		//如果技能必须指定对象，而当前位置无对象的话，直接退出
		if (pISkill->IsTargetOnly() && !nTargetIdx)
        {
			Npc[nIndex].m_nPeopleIdx = 0;
			Player[CLIENT_PLAYER_INDEX].SetTargetNpc(0);
			return 0;
		}
		
		if (nIndex == nTargetIdx)
		{
			Npc[nIndex].m_nPeopleIdx = 0;
			Player[CLIENT_PLAYER_INDEX].SetTargetNpc(0);
			return 0;
		}
	   /*
		if (Npc[nIndex].m_SkillList.GetNextCastTime(Npc[nIndex].m_ActiveSkillID)>0 && (Npc[nIndex].m_SkillList.GetNextCastTime(Npc[nIndex].m_ActiveSkillID)>SubWorld[Npc[nIndex].m_SubWorldIndex].m_dwCurrentTime))
		{//还没解冻
			return 0;
		}
		
		if (Npc[nIndex].m_HorseType)
		{//骑马时间限制
			if (Npc[nIndex].m_SkillList.GetHorseNextCastTime(Npc[nIndex].m_ActiveSkillID)>0 && (Npc[nIndex].m_SkillList.GetHorseNextCastTime(Npc[nIndex].m_ActiveSkillID)>SubWorld[Npc[nIndex].m_SubWorldIndex].m_dwCurrentTime))
				return 0;
		}*/ 
//====================客户端检测是否能发技能==============================
		if ((!Npc[nIndex].m_SkillList.CanCast(Npc[nIndex].m_ActiveSkillID, SubWorld[Npc[nIndex].m_SubWorldIndex].m_dwCurrentTime))
			||(!Npc[nIndex].Cost(pISkill->GetSkillCostType(), pISkill->GetSkillCost(&Npc[nIndex]), TRUE)))
		{
			Npc[nIndex].m_nPeopleIdx = 0;
			Player[CLIENT_PLAYER_INDEX].SetTargetNpc(0);
			return 0;
		}
//=========================================================================
		//无对象，直接发坐标
		if (!nTargetIdx)
		{
			Npc[nIndex].SendSerCommand(do_skill, Npc[nIndex].m_ActiveSkillID, nX, nY);	
			SendClientCmdSkill(Npc[nIndex].m_ActiveSkillID, nX, nY);
		//	return 0; //  无对象 直接退出
		}
		else
		{
			if (pISkill->IsTargetOnly())
			{
				int distance = NpcSet.GetDistance(nIndex , nTargetIdx);
				if (distance > pISkill->GetAttackRadius())
				{
					Player[CLIENT_PLAYER_INDEX].SetTargetNpc(nTargetIdx);
					return 0;
				}
			}
			// 
			if (nIndex == nTargetIdx && pISkill->GetSkillStyle() == SKILL_SS_Missles) 
				return 0;

			Npc[nIndex].SendSerCommand(do_skill, Npc[nIndex].m_ActiveSkillID, -1, nTargetIdx);
			// Send to Server		
			SendClientCmdSkill(Npc[nIndex].m_ActiveSkillID, -1, Npc[nTargetIdx].m_dwID);
		}
	}
	Npc[nIndex].m_nPeopleIdx = 0;
	return 1;
}
//锁定某人并使用技能
int KCoreShell::LockSomeoneUseSkill(int nTargetIndex, int nSkillID)  //锁定某人使用技能
{
	if (Player[CLIENT_PLAYER_INDEX].CheckTrading())
		return 0;
	
	int nIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;

	if (nTargetIndex == nIndex)  //等于自己 就返回
		return 0;

	if (Npc[nIndex].IsCanInput())
	{
		int nIdx = 0;
		
		nIdx = Npc[nIndex].m_SkillList.FindSame(nSkillID);
		//g_DebugLog("[skill]Active");

		Npc[nIndex].SetActiveSkill(nIdx);
	}
	else
	{
		//g_DebugLog("[skill]return");
		return 0;
	}

	int nRelation = NpcSet.GetRelation(nIndex, nTargetIndex);
	if (nRelation == relation_enemy)
	{
		Npc[nIndex].m_nPeopleIdx = nTargetIndex;
		return 1;
	}

	return 0;
}
//获取挂机状态参数
int KCoreShell::GetAutoplayid()
{

  return Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_AutoplayId;

}

//获取挂机状态参数
int KCoreShell::GetAutoSay()
{
	
	return Player[CLIENT_PLAYER_INDEX].m_Autoplay.nIsAutoSay;
	
}

void KCoreShell::AutoRun()
{
	if  (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_randmove.nTime>0)
	{
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nPeopleIdx=0;
		//NpcAI.CommonAction();
		NpcAI.ReMoveCommonAction(Player[CLIENT_PLAYER_INDEX].m_nIndex);
	}

}

int  KCoreShell::GetMapLeftTime()
{
	return Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_GameliveTime;

}

int  KCoreShell::GetMapUseTime()
{
	return Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_MapUseModel.nTime;
	
}


int  KCoreShell::GetSkillLiveTime(int nSkillId)
{
	return Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetSkillLeftTime(nSkillId);
}

//获取VIP类型
int KCoreShell::GetVipType()
{
	
	return Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_IsVip;
	
}



int KCoreShell::LockSomeoneAction(int nTargetIndex)
{
	if (Player[CLIENT_PLAYER_INDEX].CheckTrading())
		return 0;
	
//	Player[CLIENT_PLAYER_INDEX].m_AutoplayId;
	int nIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;

	if (nTargetIndex == nIndex)
		return 0;
	if (nTargetIndex <= 0 || nTargetIndex >= MAX_NPC)	//取消Lock
	{
		Npc[nIndex].m_nPeopleIdx = 0;
		return 1;
	}

	int nRelation = NpcSet.GetRelation(nIndex, nTargetIndex);
	//if (nRelation != relation_enemy)
	//{
	//	Npc[nIndex].m_nPeopleIdx = nTargetIndex;
	//	return 1;
	//}

	if (nRelation == relation_enemy && Npc[nTargetIndex].m_BaiTan==0)
	{//敌人
		KUiPlayerImmedItemSkill immedItemSkillInfo;
		memset(&immedItemSkillInfo, 0, sizeof(KUiPlayerImmedItemSkill));
		GetGameData(GDI_PLAYER_IMMED_ITEMSKILL, (int)&immedItemSkillInfo, 0);	
		if (!LockSomeoneUseSkill(nTargetIndex, immedItemSkillInfo.IMmediaSkill[0].uId))	//锁定失败
		{
			Npc[nIndex].m_nPeopleIdx = 0;
			return 1;
		}
	}
	else if (nRelation == relation_dialog)
	{//对话NPC
		Npc[nIndex].m_nPeopleIdx = nTargetIndex;
		return 1;
	}
	else //( Npc[nTargetIndex].m_Kind==kind_player && Npc[nTargetIndex].m_BaiTan==1)
	{//是玩家 并 是摆摊的
       Npc[nIndex].m_nPeopleIdx = nTargetIndex;
	   return 1;
	}

	return 0;
}
/*
if (nRelation == relation_enemy)
{//敌人
	KUiPlayerImmedItemSkill immedItemSkillInfo;
	memset(&immedItemSkillInfo, 0, sizeof(KUiPlayerImmedItemSkill));
	g_pCoreShell->GetGameData(GDI_PLAYER_IMMED_ITEMSKILL, (int)&immedItemSkillInfo, 0);
	
	if (!g_pCoreShell->LockSomeoneUseSkill(SelectPlayer.nIndex, immedItemSkillInfo.IMmediaSkill[0].uId))	//锁定失败
		g_pCoreShell->LockSomeoneAction(0);	//解锁
}
else if (nRelation == relation_dialog)
{//对话NPC
	g_pCoreShell->LockSomeoneAction(SelectPlayer.nIndex);
	}

*/
int KCoreShell::LockObjectAction(int nTargetIndex)
{
	if (Player[CLIENT_PLAYER_INDEX].CheckTrading())
		return 0;
	
	int nIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;

	if (nTargetIndex <= 0)	//取消Lock
		Npc[nIndex].m_nObjectIdx = 0;
	else
		Npc[nIndex].m_nObjectIdx = nTargetIndex;

	return 1;
}

/*
void KCoreShell::GotoXunlu(int x, int y, int mode) //瞬移
{
	//if (mode < 0 || mode > 2)
	//	return;
//Player[CLIENT_PLAYER_INDEX].Walkgua(mode,5,x,y);
//Player[CLIENT_PLAYER_INDEX].ChangeWorld(78, x * 32, y* 32);
//Player[CLIENT_PLAYER_INDEX].Shunyi(mode,x,y);
//NpcAI.FollowPeopleA(mode,x,y);
   
//KNpcAI::FollowPeople(int nIdx) 
//Npc[Player[nPlayerIndex].m_nIndex].ChangeWorld(dwWorldId, (int)Lua_ValueToNumber(L,2) * 32, (int)Lua_ValueToNumber(L,3) * 32);
}
*/

void KCoreShell::YaBiao()
{ 

  NpcAI.MoveYaBiao(Player[CLIENT_PLAYER_INDEX].m_nIndex);

}


void KCoreShell::Guaji(int x, int y, int mode)
{ 

  if (Player[CLIENT_PLAYER_INDEX].m_Autoplay.nIsReTurn)
	  return;

  NpcAI.FollowCharacter(Player[CLIENT_PLAYER_INDEX].m_nIndex,x,y,mode);

}
//设定当前坐标为起点

void KCoreShell::Setpos(int x,int y)

{
   NpcAI.Setscpos(Player[CLIENT_PLAYER_INDEX].m_nIndex,x,y);
}

//挂机 开启关闭脚本
/*
int KCoreShell::SetAutoplayer(int aa)
{
int bb;
bb=Player[CLIENT_PLAYER_INDEX].SetScript(aa);
if (bb)
return 1;
else
return 0;
}
*/
void KCoreShell::GotoWhere(int x, int y, int mode) //鼠标点击的坐标
{
	if (mode < 0 || mode > 2)
		return;

	if (Player[CLIENT_PLAYER_INDEX].m_nSendMoveFrames >= defMAX_PLAYER_SEND_MOVE_FRAME)
	{
		int bRun = false;

		if ((mode == 0 && Player[CLIENT_PLAYER_INDEX].m_RunStatus) || mode == 2)
			bRun = true;
         
		int nX = x;
		int nY = y;
		int nZ = 0;

		g_ScenePlace.ViewPortCoordToSpaceCoord(nX, nY, nZ);  //将鼠标的偏移转换成 转成三维空间像素坐标

		int nIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;

		if (!bRun)
		{
			Npc[nIndex].SendSerCommand(do_walk, nX, nY);
			// Send to Server
			SendClientCmdWalk(nX, nY);
		}
		else
		{
			Npc[nIndex].SendSerCommand(do_run, nX, nY);
			// Send to Server
			SendClientCmdRun(nX, nY);
		}

		Player[CLIENT_PLAYER_INDEX].m_nSendMoveFrames = 0;
//---------------------------------------------------------------
 if (Player[CLIENT_PLAYER_INDEX].m_Autoplay.nIsJiaoBen==1)
 {//录制脚本
     KTabFileCtrl nMapPos;	 
	 //KFile nMapPos;
     char nName[32]={0},nFiledPath[128]={0},nTitleName[128]={0};
     int  nRows,nCols;

     //if (Npc[Player[nIndex].m_nIndex].m_IsVip>0)
	  //   sprintf(nFiledPath,"\\Settings\\MapPos\\%d.txt",pNetCommand->nMapID);
     // else
	 
	 int nMapID,ncX,ncY;
	 char nMapName[64],nType[32];
		g_ScenePlace.GetSceneNameAndFocus(nMapName,nMapID,ncX,ncY,nType); //得到当前场景的格子坐标

     sprintf(nFiledPath,"\\FsAi\\%s_Pos.txt",Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name);

     if (!nMapPos.Load(nFiledPath))
	 {
	   Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("<color=yellow>录制脚本有误,请联系GM处理<color>!");
	   nMapPos.Clear();
	   return;
	 }
     nRows=nMapPos.GetHeight();
     nMapPos.InsertAfter(nRows);
     nMapPos.WriteInteger(nRows+1,1,nMapID);//Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SubWorldIndex);
     nMapPos.WriteInteger(nRows+1,2,nX);
     nMapPos.WriteInteger(nRows+1,3,nY);
     nMapPos.WriteString(nRows+1,4,Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name);
     nMapPos.Save(nFiledPath);
     nMapPos.Clear();

     Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("录制路线中,取消勾选<录制路线>即可停止..");
   }
//  KFile mFile; 
//	mFile.Open("\\Settings\\Task\\玩家任务数据\\TaskID.txt");
/*  mFile.Create(nFiledPath);
    mFile.Write(nTitleName, strlen(nTitleName));
	mFile.Close();*/
        
//----------------------------------------------------------------
	}
}


void KCoreShell::Goto(int nDir, int mode)
{
	if (nDir < 0 || nDir > 63)  //有64个方向 哪个 方向呢？如何读出方向？
		return;

	if (mode < 0 || mode > 2)
		return;

	int bRun = false;

	if ((mode == 0 && Player[CLIENT_PLAYER_INDEX].m_RunStatus) ||
		mode == 2)
		bRun = true;

	int nIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;

	int nSpeed;
	if (bRun)
		nSpeed = Npc[nIndex].m_CurrentRunSpeed;
	else
		nSpeed = Npc[nIndex].m_CurrentWalkSpeed;

	Player[CLIENT_PLAYER_INDEX].Walk(nDir, nSpeed);  //开始跑了！！

	Player[CLIENT_PLAYER_INDEX].m_nSendMoveFrames = 0;
}

void KCoreShell::Turn(int nDir)
{
	if (nDir < 0 || nDir > 3)
		return;

	if (nDir == 0)
		Player[CLIENT_PLAYER_INDEX].TurnLeft();
	else if (nDir == 1)
		Player[CLIENT_PLAYER_INDEX].TurnRight();
	else
		Player[CLIENT_PLAYER_INDEX].TurnBack();
}
//客户端 可以调用的 丢弃物品函数 （或者是外挂）
int KCoreShell::ThrowAwayItem() //扔掉手里的东西！
{
	return Player[CLIENT_PLAYER_INDEX].ThrowAwayItem();   //调用 KPLAYER  中的函数
}

int KCoreShell::GetNPCRelation(int nIndex)
{
	return NpcSet.GetRelation(Player[CLIENT_PLAYER_INDEX].m_nIndex, nIndex);
}

//--------------------------------------------------------------------------
//	功能：绘制游戏对象
//--------------------------------------------------------------------------
BOOL KCoreShell::DrawGameObj(unsigned int uObjGenre, unsigned int uId, int x, int y, int Width, int Height, int nParam,int inParam)
{
	if (g_pRepresent)
	   return	CoreDrawGameObj(uObjGenre, uId, x, y, Width, Height, nParam,inParam);

	return FALSE;
}

#include "../../Represent/iRepresent/iRepresentshell.h"

//--------------------------------------------------------------------------
//	功能：绘制游戏世界
//--------------------------------------------------------------------------
void KCoreShell::DrawGameSpace()  //
{
	if (g_pRepresent)
	{
		g_ScenePlace.Paint();
		Player[CLIENT_PLAYER_INDEX].DrawSelectInfo();
	}
}

//--------------------------------------------------------------------------
//	功能：设置绘图接口实例的指针
//--------------------------------------------------------------------------
void KCoreShell::SetRepresentShell(struct iRepresentShell* pRepresent)
{
	g_pRepresent = pRepresent;
	g_ScenePlace.RepresentShellReset();
	if (g_pAdjustColorTab && g_ulAdjustColorCount && g_pRepresent)
		g_pRepresent->SetAdjustColorList(g_pAdjustColorTab, g_ulAdjustColorCount);
}

void KCoreShell::SetMusicInterface(void* pMusicInterface)
{
	g_pMusic = (KMusic*)pMusicInterface;
	Option.SetMusicVolume(Option.GetMusicVolume());
}

//客户端日常活动，core如果要寿终正寝则返回0，否则返回非0值
int KCoreShell::Breathe()
{
	//g_SubWorldSet.MessageLoop();

	g_SubWorldSet.MainLoop();    //地图循环

	g_ScenePlace.Breathe();      //客户端场景循环

	return true;
}

int KCoreShell::GetProtocolSize(BYTE byProtocol)
{
	if (byProtocol <= s2c_clientbegin || byProtocol >= s2c_end)
		return -1;
	return g_nProtocolSize[byProtocol - s2c_clientbegin - 1];
}

#ifdef SWORDONLINE_SHOW_DBUG_INFO
extern int		g_bShowObstacle;
extern bool		g_bShowGameInfo;	//是否显示游戏（场景）信息
#endif

int KCoreShell::Debug(unsigned int uDataId, unsigned int uParam, int nParam)
{
#ifdef SWORDONLINE_SHOW_DBUG_INFO
	switch(uDataId)
	{
	case DEBUG_SHOWINFO:
		Player[CLIENT_PLAYER_INDEX].m_DebugMode = !Player[CLIENT_PLAYER_INDEX].m_DebugMode;
		g_bShowGameInfo = !g_bShowGameInfo;
		break;
	case DEBUG_SHOWOBSTACLE:
		g_bShowObstacle = !g_bShowObstacle;
		break;
	}
#endif
	return 0;
}

DWORD KCoreShell::GetPing()
{
	return g_SubWorldSet.GetPing();
}

//void KCoreShell::SendPing()
//{
//	SendClientCmdPing();
//}

void KCoreShell::SetRepresentAreaSize(int nWidth, int nHeight)
{
	g_ScenePlace.SetRepresentAreaSize(nWidth, nHeight);
}

void KCoreShell::SetClient(LPVOID pClient)
{
	g_SetClient(pClient);
}

void KCoreShell::SendNewDataToServer(void* pData, int nLength)
{
	if (g_pClient)
		g_pClient->SendPackToServer(pData, nLength);
}

//与地图相关的操作
int	KCoreShell::SceneMapOperation(unsigned int uOper, unsigned int uParam, int nParam)
{
	int nRet = 0;
	switch(uOper)
	{
	case GSMOI_SCENE_TIME_INFO:
		if (uParam)
		{
			    KUiSceneTimeInfo* pInfo = (KUiSceneTimeInfo*)uParam;
			    g_ScenePlace.GetSceneNameAndFocus(pInfo->szSceneName, pInfo->nSceneId,
				pInfo->nScenePos0, pInfo->nScenePos1,pInfo->nMapType); //得到场景的格子坐标
			    pInfo->nGameSpaceTime = (SubWorld[0].m_dwCurrentTime / 100) % 1440; //场景地图的名称 id 坐标 时间
			    
				for(int i=0;i<7;++i)
				{
					char nTongInfo[32]={0};
					
				   if (strstr(Player[CLIENT_PLAYER_INDEX].m_cTong.nAttAckInfo[i+1].m_szMapName.c_str(),pInfo->szSceneName))
				   {
					   snprintf(pInfo->nWarMaster, sizeof(pInfo->nWarMaster), "%s", Player[CLIENT_PLAYER_INDEX].m_cTong.nAttAckInfo[i+1].m_Mastername.c_str());
                       snprintf(pInfo->nWarTong, sizeof(pInfo->nWarTong), "%s", Player[CLIENT_PLAYER_INDEX].m_cTong.nAttAckInfo[i+1].m_Tongmaster.c_str());
					   pInfo->nWarShui=Player[CLIENT_PLAYER_INDEX].m_cTong.nAttAckInfo[i+1].m_shushou;  // 税收
					   pInfo->nWarZhi=0;
	                   break;
				   }
				   else
				   {
					   sprintf(pInfo->nWarMaster,"暂无");
                       sprintf(pInfo->nWarTong,"暂无");
					   pInfo->nWarShui=0;  // 税收
					   pInfo->nWarZhi=0;
				   }
				}

				snprintf(pInfo->nPlayerName, sizeof(pInfo->nPlayerName),"%s",Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name);
				snprintf(pInfo->nTongMaster, sizeof(pInfo->nTongMaster),"%s",Player[CLIENT_PLAYER_INDEX].m_cTong.m_BMasterName);

				pInfo->nIsHideNuQi=Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_bIsHideNuqi;

                pInfo->nIsWarCity=SubWorld[0].m_IswarCity;

				snprintf(pInfo->nWarGTong, sizeof(pInfo->nWarGTong),"%s",SubWorld[0].nWorldMapInfo[STR_MAP_TGONGNAME].c_str());//攻方帮派  或 帮派地图-- 共用
				snprintf(pInfo->nWarSTong, sizeof(pInfo->nWarSTong),"%s",SubWorld[0].nWorldMapInfo[STR_MAP_TSHOUNAME].c_str());//守方帮派  或 帮派地图--共用
	            pInfo->nIsWhos=SubWorld[0].m_WarIsWho;                        //属于哪方的 或 帮派地图--共用  1 为攻方 2 为守方
			
                pInfo->nTaskVal=Player[CLIENT_PLAYER_INDEX].m_cTask.GetSaveVal(130); //门派的唯一标识
		}
		break;
	case GSMOI_SCENE_MAP_INFO:
		nRet = g_ScenePlace.GetMapInfo((KSceneMapInfo*)uParam);
		break;
	case GSMOI_IS_SCENE_MAP_SHOWING:
		g_ScenePlace.SetMapParam(uParam, nParam);
		break;
	case GSMOI_PAINT_SCENE_MAP:
		g_ScenePlace.PaintMap(uParam, nParam);
		break;
	case GSMOI_SCENE_MAP_FOCUS_OFFSET:
		g_ScenePlace.SetMapFocusPositionOffset((int)uParam, nParam);
		break;
	case GSMOI_SCENE_FOLLOW_WITH_MAP:	//设置场景是否随着地图的移动而移动
		g_ScenePlace.FollowMapMove(nParam);
		break;
	case GSMOI_DRAW_GREEN_LINE:	 //点击地图寻路的
	     g_ScenePlace.DrawGreenLine((int)uParam, nParam, false);
		break;
	case GSMOI_DEL_GREEN_LINE:	//取消画黄线
		g_ScenePlace.DelGreenLine();
		break;
	case GSMOI_FIND_POS:	    //输入坐标寻路
	     g_ScenePlace.FindPos((int)uParam, nParam, true);
		break;
	case GSMOI_MAP1_INFO:	//更新帮战地图信息
		if (uParam)
		{//凤翔
			  KUiCityMapInfoA * pInfo = (KUiCityMapInfoA*)uParam;
             //int nMapIndex=g_SubWorldSet.SearchWorld(nParam);
			    snprintf(pInfo->nWarMaster, sizeof(pInfo->nWarMaster),"%s",Player[CLIENT_PLAYER_INDEX].m_cTong.nAttAckInfo[1].m_Mastername.c_str());
                snprintf(pInfo->nWarTong, sizeof(pInfo->nWarTong), "%s", Player[CLIENT_PLAYER_INDEX].m_cTong.nAttAckInfo[1].m_Tongmaster.c_str());
                pInfo->nWarShui=Player[CLIENT_PLAYER_INDEX].m_cTong.nAttAckInfo[1].m_shushou;
				pInfo->nWarZhi=0;
                pInfo->nIsWarCity=1;
		}
		break;
	case GSMOI_MAP2_INFO:	//更新帮战地图信息
		if (uParam)
		{//汴京
			KUiCityMapInfoB * pInfo = (KUiCityMapInfoB*)uParam;
 			snprintf(pInfo->nWarMaster, sizeof(pInfo->nWarMaster),"%s",Player[CLIENT_PLAYER_INDEX].m_cTong.nAttAckInfo[4].m_Mastername.c_str());
                snprintf(pInfo->nWarTong, sizeof(pInfo->nWarTong), "%s", Player[CLIENT_PLAYER_INDEX].m_cTong.nAttAckInfo[4].m_Tongmaster.c_str());
                pInfo->nWarShui=Player[CLIENT_PLAYER_INDEX].m_cTong.nAttAckInfo[4].m_shushou;
				pInfo->nWarZhi=0;
                pInfo->nIsWarCity=1;

		}
		break;
	case GSMOI_MAP3_INFO:	//更新帮战地图信息
		if (uParam)
		{//襄阳
			    KUiCityMapInfoC * pInfo = (KUiCityMapInfoC*)uParam;
                snprintf(pInfo->nWarMaster, sizeof(pInfo->nWarMaster),"%s",Player[CLIENT_PLAYER_INDEX].m_cTong.nAttAckInfo[5].m_Mastername.c_str());
                snprintf(pInfo->nWarTong, sizeof(pInfo->nWarTong), "%s", Player[CLIENT_PLAYER_INDEX].m_cTong.nAttAckInfo[5].m_Tongmaster.c_str());
                pInfo->nWarShui=Player[CLIENT_PLAYER_INDEX].m_cTong.nAttAckInfo[5].m_shushou;
				pInfo->nWarZhi=0;
                pInfo->nIsWarCity=1;
				
		}
		break;
	case GSMOI_MAP4_INFO:	//更新帮战地图信息
		if (uParam)
		{
			KUiCityMapInfoD * pInfo = (KUiCityMapInfoD*)uParam;
			snprintf(pInfo->nWarMaster, sizeof(pInfo->nWarMaster),"%s",Player[CLIENT_PLAYER_INDEX].m_cTong.nAttAckInfo[6].m_Mastername.c_str());
			snprintf(pInfo->nWarTong, sizeof(pInfo->nWarTong), "%s", Player[CLIENT_PLAYER_INDEX].m_cTong.nAttAckInfo[6].m_Tongmaster.c_str());
			pInfo->nWarShui=Player[CLIENT_PLAYER_INDEX].m_cTong.nAttAckInfo[6].m_shushou;
			pInfo->nWarZhi=0;
            pInfo->nIsWarCity=1;
		}
		break;
	case GSMOI_MAP5_INFO:	//更新帮战地图信息
		if (uParam)
		{
			KUiCityMapInfoE * pInfo = (KUiCityMapInfoE*)uParam;
            snprintf(pInfo->nWarMaster, sizeof(pInfo->nWarMaster),"%s",Player[CLIENT_PLAYER_INDEX].m_cTong.nAttAckInfo[3].m_Mastername.c_str());
			snprintf(pInfo->nWarTong, sizeof(pInfo->nWarTong), "%s", Player[CLIENT_PLAYER_INDEX].m_cTong.nAttAckInfo[3].m_Tongmaster.c_str());
			pInfo->nWarShui=Player[CLIENT_PLAYER_INDEX].m_cTong.nAttAckInfo[3].m_shushou;
			pInfo->nWarZhi=0;
            pInfo->nIsWarCity=1;
		}
		break;
	case GSMOI_MAP6_INFO:	//更新帮战地图信息
		if (uParam)
		{//成都
			KUiCityMapInfoF * pInfo = (KUiCityMapInfoF*)uParam;
            snprintf(pInfo->nWarMaster, sizeof(pInfo->nWarMaster),"%s",Player[CLIENT_PLAYER_INDEX].m_cTong.nAttAckInfo[2].m_Mastername.c_str());
			snprintf(pInfo->nWarTong, sizeof(pInfo->nWarTong), "%s", Player[CLIENT_PLAYER_INDEX].m_cTong.nAttAckInfo[2].m_Tongmaster.c_str());
			pInfo->nWarShui=Player[CLIENT_PLAYER_INDEX].m_cTong.nAttAckInfo[2].m_shushou;
			pInfo->nWarZhi=0;
            pInfo->nIsWarCity=1;
		}
		break;
	case GSMOI_MAP7_INFO:	//更新帮战地图信息
		if (uParam)
		{//临安
			KUiCityMapInfoG * pInfo = (KUiCityMapInfoG*)uParam;
			snprintf(pInfo->nWarMaster, sizeof(pInfo->nWarMaster),"%s",Player[CLIENT_PLAYER_INDEX].m_cTong.nAttAckInfo[7].m_Mastername.c_str());
			snprintf(pInfo->nWarTong, sizeof(pInfo->nWarTong), "%s", Player[CLIENT_PLAYER_INDEX].m_cTong.nAttAckInfo[7].m_Tongmaster.c_str());
			pInfo->nWarShui=Player[CLIENT_PLAYER_INDEX].m_cTong.nAttAckInfo[7].m_shushou;
			pInfo->nWarZhi=0;
            pInfo->nIsWarCity=1;
		}
		break;
	case GSMOI_MAP8_INFO:	//更新帮战地图信息
		if (uParam)
		{//巴陵
			KUiCityMapInfoH * pInfo = (KUiCityMapInfoH*)uParam;
			snprintf(pInfo->nWarMaster, sizeof(pInfo->nWarMaster),"%s","暂无");
			strcpy(pInfo->nWarTong,"暂无");
			pInfo->nWarShui=0;
			pInfo->nWarZhi=0;
            pInfo->nIsWarCity=1;
			
		}
		break;
	}
	return nRet;
}

//与帮会相关的操作, uOper的取值来自 GAME_TONG_OPERATION_INDEX
int	KCoreShell::TongOperation(unsigned int uOper, unsigned int uParam, int nParam,int vnParam)
{
	int nRet = 0;
	switch(uOper)
	{
	case GTOI_TONG_CREATE:		//创建帮会
		//uParam = (const char*) pszTongName 帮会的名字
		//nParam = (NPCCAMP)enFaction 帮会阵营
		Player[CLIENT_PLAYER_INDEX].m_cTong.ApplyCreateTong(nParam, (char *)uParam);
		break;
	case GTOI_TONG_IS_RECRUIT:		//查询某人的招人开关
		//uParam = (KUiPlayerItem*)   要查谁
		//Return = (int)(bool)		  是否开着的招人开关
		if (uParam)
		{
			KUiPlayerItem	*pItem = (KUiPlayerItem*)uParam;
			nRet = Npc[pItem->nIndex].m_nTongFlag;  //招人开关
		}
		break;
	case GTOI_TONG_RECRUIT:     //招人打开
		//uParam = (int)(bool)bRecruit 是否愿意招人
		if (uParam)
		{
             Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nTongFlag=1;
             Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("提示:开启招人权限成功!");
		}
		else
		{
			 Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nTongFlag=0;
             Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("提示:关闭招人权限成功!");
		}

		break;
	case GTOI_GET_TONG_INFO:
		{
		  if (nParam==0)
		  {//保存设置信息
            KUiTongInfoWithName *nTong = (KUiTongInfoWithName *)uParam;	   
			SendClientSetTongInfo(nTong);
			nRet=1;
		  }
		  else if (nParam==1)
		  {//请求更新帮会设置信息
			  KUiPlayerItem	*pItem = (KUiPlayerItem*)uParam;

			  SendClientGetTongInfo(pItem->nIndex,vnParam,pItem->nApplyStartPos);
		  
		  }
		} 
		break;
	case GTOI_GET_SAVE_GONGGAO:
		{
			if (nParam==0)
			{//保存公告
				KUiTongInfoWithName *nTong = (KUiTongInfoWithName *)uParam;	   
				SendClientSetTonggg(nTong);
				nRet=1;
			}
			else if (nParam==1)
			{//请求更新帮会公告信息
				KUiPlayerItem	*pItem = (KUiPlayerItem*)uParam;
				SendClientGetTonggg(pItem->nIndex);
			}
		} 
		break;
	case GTOI_TONG_SEND_ATTACK:
		{
			char m_ShopName[32];
			if (uParam)
				strcpy(m_ShopName,(const char*)nParam);  //内存复制

			if (m_ShopName[0])
			{
				//SendClientGetTonggg()
				if (strstr(Player[CLIENT_PLAYER_INDEX].m_cTong.m_szName,m_ShopName))
				{
					Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("提示:不能对自己帮会宣战!!!");
					break;
				}

			    Player[CLIENT_PLAYER_INDEX].m_cTong.SendAttack(m_ShopName);
			}

		}
		break;
	case GTOI_TONG_ACTION:         //对帮内成员做的动作，或自己与帮会的关系的改变
		//uParam = (KTongOperationParam*) pOperParam 动作时的参数
		//nParam = (KTongMemberItem*) pMember 指出了操作（帮会成员）对象，
		{
    		KTongOperationParam *Oper = (KTongOperationParam *)uParam;
		    KTongMemberItem *TargetInfo = (KTongMemberItem *)nParam;
		    KUiPlayerItem TargetPlayer;
		    int nKind;

		    switch(Oper->eOper)
		    {
		    case TONG_ACTION_DISMISS:  //踢人
				Player[CLIENT_PLAYER_INDEX].m_cTong.ApplyKick(Oper->nData[0], Oper->nData[1], TargetInfo->Name);
    			break;
		    case TONG_ACTION_ASSIGN:   //任命
    			Player[CLIENT_PLAYER_INDEX].m_cTong.ApplyInstate(Oper->nData[0], Oper->nData[1], Oper->nData[2], Oper->nData[3], TargetInfo->Name);
			    break;
		    case TONG_ACTION_DEMISE:   //传位
				Player[CLIENT_PLAYER_INDEX].m_cTong.ApplyChangeMaster(Oper->nData[0], Oper->nData[1], TargetInfo->Name);
    			break;
		    case TONG_ACTION_LEAVE:    //离开帮会
    			Player[CLIENT_PLAYER_INDEX].m_cTong.ApplyLeave();
    			break;
		    case TONG_ACTION_RECRUIT:  //同意招人
    			break;
		    case TONG_ACTION_APPLY:    //申请加入帮派
				if(!FindSpecialNPC(TargetInfo->Name, &TargetPlayer, nKind))
				{
    				break;
				}
    			Player[CLIENT_PLAYER_INDEX].m_cTong.ApplyAddTong(TargetPlayer.uId);
			    break;
		    }
		}
		break;

	//对申请加入的答复
	//uPAram = (KUiPlayerItem *) pTarget   申请方
	//nParam : != 0  同意   ==0  拒绝
	case GTOI_TONG_JOIN_REPLY:
		if (uParam)
		{
			KUiPlayerItem	*pItem = (KUiPlayerItem*)uParam;
			Player[CLIENT_PLAYER_INDEX].m_cTong.AcceptMember(pItem->nIndex, g_FileName2Id(pItem->Name), nParam);
		}
		break;
	case GTOI_TONG_BZName:  //获取帮主名
		if (uParam)
		{
			KUiPlayerItem	*pItem = (KUiPlayerItem*)uParam;

			snprintf(pItem->BZname, sizeof(pItem->BZname),"%s",Player[CLIENT_PLAYER_INDEX].m_cTong.m_BMasterName);
				
		}
		break;

	case GTOI_REQUEST_PLAYER_TONG:	//查询某人是哪帮会
		//uParam = (KUiPlayerItem*) 要查谁
		//nParam = (int)(bool)bReturnTongDetail 是否要返回那个帮会的信息
		if (uParam)
		{
			KUiPlayerItem	*pItem = (KUiPlayerItem*)uParam;
		//	if (pItem->nIndex == Player[CLIENT_PLAYER_INDEX].m_nIndex)
			{//查看自己帮会信息
				Player[CLIENT_PLAYER_INDEX].m_cTong.ApplyInfo(enumTONG_APPLY_INFO_ID_TONG_HEAD,pItem->nIndex, 0, 0);
			}
		//	else
		//	{//别人查看本人的帮会信息
		//		Player[CLIENT_PLAYER_INDEX].m_cTong.ApplyInfo(
		//			enumTONG_APPLY_INFO_ID_TONG_HEAD,
		//			pItem->nIndex, 0, 0);
		//	}
		}
		break;
	case GTOI_TONG_ISJION:
		{
			if (Player[CLIENT_PLAYER_INDEX].m_cTong.CheckIn() == 0)        //是否加入帮派
			{
				nRet=-1;
				break;
			}
                nRet = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nFigure;
		}
		break;
		
	case GTOI_REQUEST_TONG_ALLDATA:
		{
		  KUiGameObjectWithName	*pObj = (KUiGameObjectWithName*)uParam;
		  switch (pObj->nData)
		  {
		  case enumTONG_FIGURE_LIST: //帮众
			  Player[CLIENT_PLAYER_INDEX].m_cTong.ApplyInfo(
				  enumTONG_APPLY_INFO_ID_LIST,
				  0, pObj->nParam, defTONG_ONE_PAGE_MAX_NUM);
			  break;
			}
		}
		break;
	case GTOI_REQUEST_TONG_DATA:     //要求某个帮会的各种资料
	//uParam = (KUiGameObjectWithName*)pTong 要查询的帮会
	//KUiGameObjectWithName::szName 帮会的名字
	//KUiGameObjectWithName::nData 资料的种类，值取自枚举TONG_MEMBER_FIGURE
	//			列表的种类是enumTONG_FIGURE_MASTER的话代表要求的是帮会的资讯。
	//KUiGameObjectWithName::nParam 开始的索引
		if (uParam)
		{
			if (Player[CLIENT_PLAYER_INDEX].m_cTong.CheckIn() == 0)        //是否加入帮派
				break;

			KUiGameObjectWithName	*pObj = (KUiGameObjectWithName*)uParam;
			char	szTongName[32];
			DWORD	dwTongNameID;

			szTongName[0] = 0;
			Player[CLIENT_PLAYER_INDEX].m_cTong.GetTongName(szTongName);

			if (!szTongName[0])
				break;
			// 只能查询自己帮会信息
			         dwTongNameID = g_FileName2Id(pObj->szName); //帮会名
			if (dwTongNameID != Player[CLIENT_PLAYER_INDEX].m_cTong.GetTongNameID())
				break;//不是同一个帮会的

			switch (pObj->nData)
			{
			case enumTONG_FIGURE_MASTER:  //帮主
				Player[CLIENT_PLAYER_INDEX].m_cTong.ApplyInfo(
					enumTONG_APPLY_INFO_ID_TONG_HEAD,
					Player[CLIENT_PLAYER_INDEX].m_nIndex, 0,0);
				break;
			case enumTONG_FIGURE_DIRECTOR:// 长老
				Player[CLIENT_PLAYER_INDEX].m_cTong.ApplyInfo(
					enumTONG_APPLY_INFO_ID_TONG_HEAD,
					Player[CLIENT_PLAYER_INDEX].m_nIndex,  pObj->nParam,defTONG_ONE_PAGE_MAX_NUM);
				break;
			case enumTONG_FIGURE_MANAGER: //队长
				if (!Player[CLIENT_PLAYER_INDEX].m_cTong.CanGetManagerInfo(dwTongNameID))
					break;
				Player[CLIENT_PLAYER_INDEX].m_cTong.ApplyInfo(
					enumTONG_APPLY_INFO_ID_MANAGER,
					dwTongNameID, pObj->nParam, defTONG_ONE_PAGE_MAX_NUM);
				break;
			case enumTONG_FIGURE_MEMBER: //帮众
				if (!Player[CLIENT_PLAYER_INDEX].m_cTong.CanGetMemberInfo(dwTongNameID))
					break;
				Player[CLIENT_PLAYER_INDEX].m_cTong.ApplyInfo(
					enumTONG_APPLY_INFO_ID_MEMBER,
					dwTongNameID, pObj->nParam, defTONG_ONE_PAGE_MAX_NUM);
				break;
			}
		}
		break;
	case GTOI_TONG_MONEY_ACTION:
		if (uParam)
		{
			KUiPlayerItem	*pItem = (KUiPlayerItem*)uParam;
			switch(nParam)
			{
			case TONG_ACTION_SAVE:
				Player[CLIENT_PLAYER_INDEX].m_cTong.ApplySaveMoney(pItem->nData);
				break;
			case TONG_ACTION_GET:
				Player[CLIENT_PLAYER_INDEX].m_cTong.ApplyGetMoney(pItem->nData);
				break;
			case TONG_ACTION_SND:
				Player[CLIENT_PLAYER_INDEX].m_cTong.ApplySndMoney(pItem->nData);
				break;
			}
		}
		break;
	}
	return nRet;
}

//与组队相关的操作，uOper的取值来自 GAME_TEAM_OPERATION_INDEX
int KCoreShell::TeamOperation(unsigned int uOper, unsigned int uParam, int nParam)
{
	int nRet = 0;
	switch(uOper)
	{
	case TEAM_OI_GD_INFO:		//主角所在的队伍信息
		if (uParam)
		{
			KUiPlayerTeam* pTeam = (KUiPlayerTeam*)uParam;
			nRet = Player[CLIENT_PLAYER_INDEX].m_cTeam.GetInfo(pTeam);
		}
		break;
	case TEAM_OI_GD_MEMBER_LIST://获取主角所在队伍成员列表
		nRet = g_Team[0].GetMemberInfo((KUiPlayerItem *)uParam, nParam);
		break;
	case TEAM_OI_GD_REFUSE_INVITE_STATUS://获取拒绝邀请的状态
		nRet = Player[CLIENT_PLAYER_INDEX].m_cTeam.GetAutoRefuseState();
		break;
	case TEAM_OI_COLLECT_NEARBY_LIST://获取周围队伍的列表
		NpcSet.GetAroundOpenCaptain(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Camp);
		break;
	case TEAM_OI_APPLY:			//申请加入她人队伍
		if (uParam)
			Player[CLIENT_PLAYER_INDEX].ApplyAddTeam(((KUiTeamItem*)uParam)->Leader.nIndex);
		break;
	case TEAM_OI_CREATE:		//新组队伍
		Player[CLIENT_PLAYER_INDEX].ApplyCreateTeam();//(char*)uParam);
		break;
	case TEAM_OI_APPOINT:		//任命队长，只有队长调用才有效果
		Player[CLIENT_PLAYER_INDEX].ApplyTeamChangeCaptain(((KUiPlayerItem*)uParam)->uId);		
		break;
	case TEAM_OI_INVITE:		//邀请别人加入队伍，只有队长调用才有效果
		if (uParam)
		{
			Player[CLIENT_PLAYER_INDEX].TeamInviteAdd(((KUiPlayerItem*)uParam)->uId);

			KSystemMessage	sMsg;
			sprintf(sMsg.szMessage, MSG_TEAM_SEND_INVITE, ((KUiPlayerItem*)uParam)->Name);
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case TEAM_OI_KICK:			//踢除队里的一个队员，只有队长调用才有效果
		Player[CLIENT_PLAYER_INDEX].TeamKickMember(((KUiPlayerItem*)uParam)->uId);
		break;
	case TEAM_OI_LEAVE:			//离开队伍
		Player[CLIENT_PLAYER_INDEX].LeaveTeam();
		break;
	case TEAM_OI_CLOSE:			//关闭组队，只有队长调用才有效果
		Player[CLIENT_PLAYER_INDEX].ApplyTeamOpenClose(nParam);
		break;
	case TEAM_OI_REFUSE_INVITE:	//拒绝别人邀请自己加入队伍
		Player[CLIENT_PLAYER_INDEX].m_cTeam.SetAutoRefuseInvite(nParam);
		break;
	case TEAM_OI_APPLY_RESPONSE://批准他人加入队伍，只有队长调用才有效果
		if (uParam)
		{
			if (nParam)
			{
				Player[CLIENT_PLAYER_INDEX].AcceptTeamMember(((KUiPlayerItem*)uParam)->uId);
			}
			else
			{
				Player[CLIENT_PLAYER_INDEX].m_cTeam.DeleteOneFromApplyList(((KUiPlayerItem*)uParam)->uId);
//				Player[CLIENT_PLAYER_INDEX].m_cTeam.UpdateInterface();
			}
		}
		break;
	case TEAM_OI_INVITE_RESPONSE://对组队邀请的回复
		if (uParam)
			Player[CLIENT_PLAYER_INDEX].m_cTeam.ReplyInvite(((KUiPlayerItem*)uParam)->nIndex, nParam);
		break;
	}
	return nRet;
}

int KCoreShell::GetGenreItem(unsigned int uId )
{
	return Item[uId].GetGenre();
}
//动态边框调用
int KCoreShell::GetTypeItem(unsigned int uId )
{
	if (Item[uId].GetGoldId()>0)    //是黄金
	{
	   if (Item[uId].GetIsPlatima())
		   return 4;
	   else
		   return 1;
	}
	else if (Item[uId].IsPurple()>0) //是紫装
	{
		return 2;
	}
	else if (Item[uId].GetAttribType() && (!Item[uId].GetGoldId())&&(Item[uId].IsPurple()<=0))
	{
	    return 3; //蓝装
	}
	return 0;     //白装
}
//蓝装
int KCoreShell::GetObjColor(unsigned int uId )
{
  if (Item[uId].GetAttribType() && (!Item[uId].GetGoldId())&&(Item[uId].IsPurple()<=0 || Item[uId].GetPoint()>6))
	return 2;

	return 0;
} 

int KCoreShell::GetNPCBaiTan(int nIndex)
{
	int nID = NpcSet.SearchID(nIndex);
	return Npc[nID].m_BaiTan;
}
//是否可以交易
int KCoreShell::GetTradeItem(unsigned int uId )
{
    return Item[uId].GetIsTrade();

}

//是否可以丢弃
int KCoreShell::GetDropItem(unsigned int uId )
{
    return Item[uId].GetIsDrop();
}


//奇珍阁

int KCoreShell::GetDataSuperShop(int nSaleId, unsigned int uParam, int nParam)
{
	int nRet = 0;
	int	nBuyIdx = nSaleId;//Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nShopIdx[nSaleId];
	if (nBuyIdx == -1)
		return 0;
	if (nBuyIdx >= BuySell.GetHeight())
		return 0;
	Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nBuyIdx = nBuyIdx;
	int nIndex = 0;
	int nCount = 0;

	if (uParam)
	{
		int nPage = 0;
		KUiObjAtContRegion* pInfo = (KUiObjAtContRegion *)uParam;
		for (int i = 0; i < BuySell.GetWidth(); i++)
		{
			nIndex = BuySell.GetItemIndex(nBuyIdx, i);
			KItem* pItem = BuySell.GetItemidx(nIndex);
			
			if (nIndex >= 0 && pItem)
			{
				pInfo->Obj.uGenre = CGOG_NPCSELLITEM;
				pInfo->Obj.uId = i;	//几率 第几行的第i个物品
				pInfo->Region.h = 0;
				pInfo->Region.v = 0;
				pInfo->Region.Width  = pItem->GetWidth();
				pInfo->Region.Height = pItem->GetHeight();

				if (strstr(pItem->GetItmeInfo(),".lua"))
				{
					ItemXinXi SynItem;
					ZeroStruct(SynItem);
					//SynItem.nIdx  = nIndex; //索引
					SynItem.nX = nBuyIdx;
					SynItem.nY = i;
					SynItem.nIsRongqi = 1;
					SendClientAddItem(&SynItem);
				
				}
			
				pInfo->nContainer = nPage;
				nCount++;
				pInfo++;
				if (nCount%15==0)
					nPage +=1;

			}
		}
	

		nRet = nPage;
	}
	else
	{
		for (int i = 0; i < BuySell.GetWidth(); i++)
		{
			nIndex = BuySell.GetItemIndex(nBuyIdx, i);
			KItem* pItem = BuySell.GetItemidx(nIndex);
			
			if (nIndex >= 0 && pItem)
			{
				nCount++;
			}
		}
		nRet = nCount;
	}
	return nRet;
}


int KCoreShell::GetDataDynamicShop(int nSaleId, unsigned int uParam, int nParam)
{
	int nRet = 0;
	int	nBuyIdx = nSaleId;//Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nShopIdx[nSaleId];
	if (nBuyIdx == -1)
		return 0;
	if (nBuyIdx >= BuySell.GetHeight())
		return 0;
	Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nBuyIdx = nSaleId;
	int nIndex = 0;
	int nCount = 0;
	if (uParam)
	{
		int nPage = 0;
		KUiObjAtContRegion* pInfo = (KUiObjAtContRegion *)uParam;
		if (!BuySell.m_pSShopRoom)
			return 0;
		BuySell.m_pSShopRoom->Clear();
		for (int i = 0; i < BuySell.GetWidth(); i++)
		{
			nIndex = BuySell.GetItemIndex(nBuyIdx, i);
			KItem* pItem = BuySell.GetItemidx(nIndex);
			
			if (nIndex >= 0 && pItem)
			{
				pInfo->Obj.uGenre = CGOG_NPCSELLITEM;
				pInfo->Obj.uId = i;
				POINT	Pos;
				if (BuySell.m_pSShopRoom->FindRoom(pItem->GetWidth(), pItem->GetHeight(), &Pos))
				{
					BuySell.m_pSShopRoom->PlaceItem(Pos.x, Pos.y, nIndex + 1, pItem->GetWidth(), pItem->GetHeight());
				}
				else
				{
					nPage++;
					BuySell.m_pSShopRoom->Clear();
					BuySell.m_pSShopRoom->FindRoom(pItem->GetWidth(), pItem->GetHeight(), &Pos);
					BuySell.m_pSShopRoom->PlaceItem(Pos.x, Pos.y, nIndex + 1, pItem->GetWidth(), pItem->GetHeight());
				}

				if (strstr(pItem->GetItmeInfo(),".lua"))
				{
					ItemXinXi SynItem;
					ZeroStruct(SynItem);
					//SynItem.nIdx  = nIndex; //索引
					SynItem.nX = nBuyIdx;
					SynItem.nY = i;
					SynItem.nIsRongqi = 1;
					SendClientAddItem(&SynItem);
				}

				pInfo->Region.h = Pos.x;
				pInfo->Region.v = Pos.y;
				pInfo->Region.Width = pItem->GetWidth();
				pInfo->Region.Height = pItem->GetHeight();
				pInfo->nContainer = nPage;
				nCount++;
				pInfo++;
			}
		}			
		nRet = nPage;
	}
	else
	{
		for (int i = 0; i < BuySell.GetWidth(); i++)
		{
			nIndex = BuySell.GetItemIndex(nBuyIdx, i);
			KItem* pItem = BuySell.GetItemidx(nIndex);
			
			if (nIndex >= 0 && pItem)
			{
				nCount++;
			}
		}
		nRet = nCount;
	}
	return nRet;
}



int KCoreShell::GetDataNpcShop(int nSaleId, unsigned int uParam, int nParam)
{
	int nRet = 0;
	int	nBuyIdx = nSaleId;//Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nShopIdx[nSaleId];
	if (nBuyIdx == -1)
		return 0;
	if (nBuyIdx >= BuySell.GetHeight())
		return 0;
	Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nBuyIdx = nSaleId;
	int nIndex = 0;
	int nCount = 0;
	if (uParam)
	{
		int nPage = 0;
		KUiObjAtContRegion* pInfo = (KUiObjAtContRegion *)uParam;
		if (!BuySell.m_pShopRoom)
			return 0;
		BuySell.m_pShopRoom->Clear();
		for (int i = 0; i < BuySell.GetWidth(); i++)
		{
			nIndex = BuySell.GetItemIndex(nBuyIdx, i);
			KItem* pItem = BuySell.GetItemidx(nIndex);
			
			if (nIndex >= 0 && pItem)
			{
				pInfo->Obj.uGenre = CGOG_NPCSELLITEM;
				pInfo->Obj.uId = i;
				POINT	Pos;
				if (BuySell.m_pShopRoom->FindRoom(pItem->GetWidth(), pItem->GetHeight(), &Pos))
				{
					BuySell.m_pShopRoom->PlaceItem(Pos.x, Pos.y, nIndex + 1, pItem->GetWidth(), pItem->GetHeight());
				}
				else
				{
					nPage++;
					BuySell.m_pShopRoom->Clear();
					BuySell.m_pShopRoom->FindRoom(pItem->GetWidth(), pItem->GetHeight(), &Pos);

					if (strstr(pItem->GetItmeInfo(),".lua"))
					{//如果是脚本 则要求服务器同步过来

						ItemXinXi SynItem;
						ZeroStruct(SynItem);
						//SynItem.nIdx  = nIndex; //索引
						SynItem.nX = nBuyIdx;
						SynItem.nY = i;
						SynItem.nIsRongqi = 1;
					    SendClientAddItem(&SynItem);

					}

					BuySell.m_pShopRoom->PlaceItem(Pos.x, Pos.y, nIndex + 1, pItem->GetWidth(), pItem->GetHeight());
				}
				
				pInfo->Region.h = Pos.x;
				pInfo->Region.v = Pos.y;
				pInfo->Region.Width = pItem->GetWidth();
				pInfo->Region.Height = pItem->GetHeight();
				pInfo->nContainer = nPage;
				nCount++;
				pInfo++;
			}
		}			
		nRet = nPage;
	}
	else
	{
		for (int i = 0; i < BuySell.GetWidth(); i++)
		{
			nIndex = BuySell.GetItemIndex(nBuyIdx, i);
			KItem* pItem = BuySell.GetItemidx(nIndex);
			
			if (nIndex >= 0 && pItem)
			{
				nCount++;
			}
		}
		nRet = nCount;
	}
	return nRet;
}




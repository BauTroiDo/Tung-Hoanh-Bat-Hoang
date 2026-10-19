#include	"KCore.h"
#include	"MyAssert.H"
#include	"KItem.h"
#include	"KItemSet.h"
#include	"KNpc.h"
#include	"KMath.h"
#include	"KPlayer.h"
#include	"KItemList.h"
#include    "KItemGenerator.h"
#include	"KItemChangeRes.h"
#include	<time.h>
#include    "Text.h"
#ifdef _SERVER

#include	"KObjSet.h"
#endif
#ifndef _SERVER
#include	"CoreShell.h"
#include	"../../../Headers/IClient.h"
#endif

extern KTabFile g_EventItemTab;
extern KTabFile g_MineItemTab;
extern KTabFile g_FusionTab;

// PK死亡惩罚掉穿的装备，各装备掉落的权值
#define	defEQUIP_POWER
#ifdef defEQUIP_POWER
	int	g_nEquipPower[itempart_num] =
	{2, 4, 2, 4, 1, 1, 1, 1, 1, 1, 0,0,1,2,1};

#endif
//激活的装备清单
int KItemList::ms_ActiveEquip[itempart_num][MAX_ITEM_ACTIVE] = 
{
    { itempart_foot, itempart_ring1,itempart_weapon},	  //itempart_head,    头盔 激活 靴子,上戒,武器
	{ itempart_weapon, itempart_head,itempart_yinjian},	  //itempart_body,    衣服 激活 武器,头盔,印鉴
	{ itempart_body, itempart_amulet,itempart_ring1},	  //itempart_belt,    腰带 激活 衣服,项链,上戒
	{ itempart_ring1, itempart_foot,itempart_cuff},	      //itempart_weapon,  武器 激活 上戒,靴子,护腕
	{ itempart_cuff, itempart_pendant,itempart_body},     //itempart_foot,    靴子 激活 护腕,玉佩,衣服
	{ itempart_belt, itempart_ring2,itempart_head},	      //itempart_cuff,    护腕 激活 腰带,下戒,头盔
	{ itempart_head, itempart_weapon,itempart_ring2},	  //itempart_amulet,  项链 激活 头盔,武器,下戒
	{ itempart_pendant,itempart_cuff,itempart_amulet},   //itempart_ring1,    上戒 激活 玉佩,护腕,项链
	{ itempart_amulet, itempart_body,itempart_belt},      //itempart_ring2,   下戒 激活 项链,衣服,腰带
	{ itempart_ring2,itempart_belt,itempart_pifeng},	  //itempart_pendant, 玉佩 激活 下戒,腰带,披风
	{ itempart_pifeng, itempart_shiping ,itempart_foot}, //itempart_horse,    马匹 激活 披风,饰品,靴子
	{ itempart_horse, itempart_pifeng ,itempart_pendant}, //itempart_mask	  面具 激活 马匹,披风,玉佩 
	{ itempart_shiping, itempart_yinjian,itempart_mask},  //                  披风 激活 饰品,印鉴,面具
	{ itempart_mask, itempart_horse,itempart_shiping},	  //	              印鉴 激活 面具,马匹,饰品
	{ itempart_yinjian, itempart_mask,itempart_horse},	  //	              饰品 激活 印鉴,面具,马匹
	                                                                          //以上均是相生激活
};
//被激活的装备清单
int KItemList::ms_ActivedEquip[itempart_num][MAX_ITEM_ACTIVE] =
{
	{ itempart_body, itempart_amulet,itempart_cuff},   //	itempart_head,  头盔 需要 衣服,项链,护腕  激活
	{ itempart_ring2, itempart_belt,itempart_foot},	   //	itempart_body,  衣服 需要 下戒,腰带,靴子  激活	    
	{ itempart_pendant, itempart_cuff,itempart_ring2}, //	itempart_belt,  腰带 需要 玉佩,护腕,下戒  激活
	{ itempart_amulet, itempart_body,itempart_head},   //	itempart_weapon,武器 需要 项链,衣服,头盔  激活
	{ itempart_weapon, itempart_head,itempart_horse},//	itempart_foot,      靴子 需要 武器,头盔,马匹  激活
	{ itempart_foot, itempart_ring1,itempart_weapon},  //	itempart_cuff,  护腕 需要 靴子,上戒,武器  激活
	{ itempart_belt, itempart_ring2,itempart_ring1},   //	itempart_amulet,项链 需要 腰带,下戒,上戒  激活
	{ itempart_weapon, itempart_head,itempart_belt},   //	itempart_ring1, 上戒 需要 武器,头盔,腰带  激活
	{ itempart_cuff, itempart_pendant,itempart_amulet},//	itempart_ring2, 下戒 需要 护腕,玉佩,项链  激活
	{ itempart_foot, itempart_ring1,itempart_mask},	 //	itempart_pendant,   玉佩 需要 靴子,上戒,面具  激活
	{ itempart_mask, itempart_yinjian,itempart_shiping},//	                马匹 需要 面具,印鉴,饰品  激活
	{ itempart_yinjian, itempart_shiping,itempart_pifeng},             //   面具 需要 印鉴,饰品,披风  激活
	{ itempart_horse, itempart_mask,itempart_pendant},	               //   披风 需要 马匹,面具,玉佩  激活
	{ itempart_pifeng, itempart_shiping,itempart_body},	               //   印鉴 需要 披风,饰品,衣服  激活
	{ itempart_horse, itempart_pifeng,itempart_yinjian},	           //   饰品 需要 马匹,披风,印鉴  激活
};
                                                                       //   以上均是相生激活
KItemList::KItemList()
{
	m_PlayerIdx = 0;
	m_nListCurIdx = 0;
    m_nLastTotalStackedItem = 0;
	ZeroMemory(m_EquipItem, sizeof(m_EquipItem));		
	ZeroMemory(m_sBackItems, sizeof(m_sBackItems));
	ZeroMemory(m_Items,sizeof(m_Items));
}

KItemList::~KItemList()
{

}


/*!*****************************************************************************
// Function		: KItemList::GetWeaponType
// Purpose		: 取得玩家装备的武器类型
// Return		: int 
// Comments		:
// Author		: Spe
*****************************************************************************/
int KItemList::GetWeaponType()
{
	if (m_EquipItem[itempart_weapon])
		return Item[m_EquipItem[itempart_weapon]].GetDetailType();
	else
		return -1;
}

// STVL(Min/Max) cua vu khi
void KItemList::GetWeaponDamage(int* nMin, int* nMax)  //武器伤害
{
	//if (!nMin || !nMax)
	//	return;

/*	int nWeaponIdx = m_EquipItem[itempart_weapon];
	if (nWeaponIdx)
	{
		*nMin = Item[nWeaponIdx].m_aryBaseAttrib[0].nValue[0];
		*nMax = Item[nWeaponIdx].m_aryBaseAttrib[1].nValue[0];
	}
*/
	int nWeaponIdx = m_EquipItem[itempart_weapon];
	if (nWeaponIdx)
	{
		_ASSERT(Item[nWeaponIdx].m_CommonAttrib.nItemGenre == item_equip 
			&& (Item[nWeaponIdx].m_CommonAttrib.nDetailType == equip_meleeweapon
			|| Item[nWeaponIdx].m_CommonAttrib.nDetailType == equip_rangeweapon));
		int nMinDamage, nMaxDamage, nEnhance;
		int nDamageMinBase = Item[nWeaponIdx].m_aryBaseAttrib[0].nValue[0]; //武器的基本伤害
		int	nDamageMaxBase = Item[nWeaponIdx].m_aryBaseAttrib[1].nValue[0];
		nMinDamage = 0;
		nMaxDamage = 0;
		nEnhance = 0;
		for (int i = 0; i < 6; ++i)
		{
			switch(Item[nWeaponIdx].m_aryMagicAttrib[i].nAttribType)
			{
			case magic_weapondamagemin_v:
				nMinDamage += Item[nWeaponIdx].m_aryMagicAttrib[i].nValue[0];
				break;
			case magic_weapondamagemax_v:
				nMaxDamage += Item[nWeaponIdx].m_aryMagicAttrib[i].nValue[0];
				break;
			case magic_weapondamageenhance_p:
				nEnhance += Item[nWeaponIdx].m_aryMagicAttrib[i].nValue[0];
				break;
			default:
				break;
			}
		}
		*nMin = (nDamageMinBase + nMinDamage) * (100 + nEnhance) / 100;
		*nMax = (nDamageMaxBase + nMaxDamage) * (100 + nEnhance) / 100;
	}
	else
	{// 空手
		/*
		int nDamageBase = Player[m_PlayerIdx].m_nCurStrength * Player[m_PlayerIdx].m_nCurDexterity;
		*nMin = nDamageBase >> 9;
		*nMax = nDamageBase >> 8;
		*/
		// 数值计算方法修改（孙英要求） by Spe 03/06/11
		_ASSERT(STRENGTH_SET_DAMAGE_VALUE > 0);
		*nMin = Player[m_PlayerIdx].m_nCurStrength / STRENGTH_SET_DAMAGE_VALUE + 1;
		*nMax = Player[m_PlayerIdx].m_nCurStrength / STRENGTH_SET_DAMAGE_VALUE + 1;
	}
}

// STVL diem cua vu khi (magic_addphysicsdamage_v)
int KItemList::GetWeaponPhysicDamagePoint()
{
	int nWeaponIdx = m_EquipItem[itempart_weapon];
	int nPoint = 0;
	if (nWeaponIdx)
	{
		for (int i = 0; i < 6; ++i)
		{
			switch (Item[nWeaponIdx].m_aryMagicAttrib[i].nAttribType)
			{
			case magic_addphysicsdamage_v:
				nPoint += Item[nWeaponIdx].m_aryMagicAttrib[i].nValue[0];
				break;
			default:
				break;
			}
		}
	}
	return nPoint;
}

/*!*****************************************************************************
// Function		: KItemList::Add
// Purpose		: 玩家得到一个装备
// Return		: int 
// Argumant		: int nIdx		Item数组的idx
// Argumant		: int nPlace
// Argumant		: int nX
// Argumant		: int nY
// Comments		:
// Author		: Spe   玩家增加装备调用的函数！同步装备
*****************************************************************************/
int KItemList::Add(int nIdx, int nPlace, int nX, int nY, BOOL bInit,int nIsAutoDie,int StackNum,int nIsLoign) //目标容器增加物品
{

	if (nIdx <= 0)
		return 0;

	if (Item[nIdx].GetID() == 0)  //获取dwid
		return 0;

//xep chong item
#ifdef _SERVER
    int pnX = -1, pnY = -1; // Khoi tao gia tri mac dinh
    if (Item[nIdx].IsStack() && !nIsLoign)
	{ // Neu so luong stack lon hon 1
        int pnIdx = 0;
        if (Player[m_PlayerIdx].m_ItemList.m_Room[room_equipment].FindSameItemToStack(nIdx, &pnIdx, &pnX, &pnY)) 
		{
            // Neu tim thay item tuong tu
			int nNum = StackNum + Item[pnIdx].GetStackNum(); // Cong StackNum voi so luong cua item da co
			int nNum2 = Item[nIdx].GetStackNum() + Item[pnIdx].GetStackNum(); // Cong StackNum voi so luong cua item da co
            //printf("1. Tong so luong nNum: %d, StackNum: %d itemnIdx: %d itempnIdx: %d nIsLoign: %d\n", nNum, StackNum, Item[nIdx].GetStackNum(), Item[pnIdx].GetStackNum(),nIsLoign);

            if (nNum <= MAX_STACKNUM) 
			{
                //printf("2. khong vuot gioi han stack. nNum: %d, StackNum: %d ,\n", nNum, StackNum);
				nX = pnX;
                nY = pnY;
                Item[nIdx].SetStackNum(StackNum + Item[pnIdx].GetStackNum()); // Cap nhat so luong con lai cho item moi
                //Item[pnIdx].SetStackNum(MAX_STACKNUM);      // Dat gioi han max cho item cu
                Player[m_PlayerIdx].m_ItemList.Remove(pnIdx,0,0,0,FALSE); // Xoa item moi khoi danh sach
                ItemSet.Remove(pnIdx); // Loai bo khoi bo nho

            } else {
                //printf("3. Khong tim thay item tuong tu. So luong stack: %d\n", StackNum);
                Item[nIdx].SetStackNum(StackNum); // Cap nhat so luong truc tiep
            }
        }
		else
		{
            // Khong tim thay item tuong tu de xep chong
            //printf("4. Khong tim thay item tuong tu. So luong stack: %d\n", StackNum);
            Item[nIdx].SetStackNum(StackNum); // Cap nhat so luong truc tiep
        }
    }
	else //xep chong phan tach item
	{
		if (StackNum > 1)   
			Item[nIdx].SetStackNum(StackNum); // Cap nhat so luong truc tiep
	}
#endif




    //产生物品 查找未使用表 是否有位置
	int i = FindFree();

	if (i<=0)
		return 0;

	switch(nPlace) //目标容器
	{
	case pos_hand:
		if (m_Hand)
			return 0;

		m_Items[i].nPlace = pos_hand;
		m_Items[i].nX = 0;
		m_Items[i].nY = 0;
		m_Hand = nIdx;
		break;
	case pos_equip:
		if (nX < 0 || nX >= itempart_num)
			return 0;

		if (m_EquipItem[nX])
			return 0;
//	    有可能是条件不够，被其他装备凑上的。是否能装备应该放到Equip时检查
//		if (!CanEquip(nIdx, nX))
//			return 0;
		m_Items[i].nPlace = pos_equip;
		m_Items[i].nX = nX;
		m_Items[i].nY = 0;
		break;
	case pos_equiproom:  //包袱 装备空间  
		if (!m_Room[room_equipment].PlaceItem(nX,nY,nIdx,Item[nIdx].GetWidth(),Item[nIdx].GetHeight()))
		{
#ifdef _SERVER
		msgshow("提示:包袱空间不足S!");
		return 0;
#else
        Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("提示:包袱空间不足C!");
	   	return 0;
#endif	
		} 
		m_Items[i].nPlace = pos_equiproom;  //设置容器物品信息
		m_Items[i].nX = nX;
		m_Items[i].nY = nY;
		break;
#ifndef _SERVER
	case pos_trade1:
		if ( !Player[CLIENT_PLAYER_INDEX].CheckTrading() )
			return 0;
		if (!m_Room[room_trade1].PlaceItem(nX, nY, nIdx, Item[nIdx].GetWidth(), Item[nIdx].GetHeight()))
			return 0;
		m_Items[i].nPlace = pos_trade1;
		m_Items[i].nX = nX;
		m_Items[i].nY = nY;
		break;
#endif
	case pos_repositoryroom:  //储物箱
		if (!m_Room[room_repository].PlaceItem(nX, nY, nIdx, Item[nIdx].GetWidth(), Item[nIdx].GetHeight()))
		{
#ifdef _SERVER
			msgshow("提示:储物箱空间有限!");
#endif
			return 0;
		} 
		m_Items[i].nPlace = pos_repositoryroom;
		m_Items[i].nX = nX;
		m_Items[i].nY = nY;		
		break;
	case pos_exbox1room: // 扩展箱 1
		if (!m_Room[room_exbox1].PlaceItem(nX, nY, nIdx, Item[nIdx].GetWidth(), Item[nIdx].GetHeight()))
		{
#ifdef _SERVER
			msgshow("提示:扩展箱空间有限!");
#endif
			return 0;
		}
		m_Items[i].nPlace = pos_exbox1room;
		m_Items[i].nX = nX;
		m_Items[i].nY = nY;		
		break;
	case pos_exbox2room: // 扩展箱 2
		if (!m_Room[room_exbox2].PlaceItem(nX, nY, nIdx, Item[nIdx].GetWidth(), Item[nIdx].GetHeight()))
		{
#ifdef _SERVER
			msgshow("提示:扩展箱空间有限!");
#endif
			return 0;
		}
		m_Items[i].nPlace = pos_exbox2room;
		m_Items[i].nX = nX;
		m_Items[i].nY = nY;		
		break;
	case pos_exbox3room: // 扩展箱 3
		if (!m_Room[room_exbox3].PlaceItem(nX, nY, nIdx, Item[nIdx].GetWidth(), Item[nIdx].GetHeight()))
		{
#ifdef _SERVER
			msgshow("提示:扩展箱空间有限!");
#endif
			return 0;
		}
		m_Items[i].nPlace = pos_exbox3room;
		m_Items[i].nX = nX;
		m_Items[i].nY = nY;		
		break;
	case pos_equiproomex:  // 字母袋
		if (!m_Room[room_equipmentex].PlaceItem(nX, nY, nIdx, Item[nIdx].GetWidth(), Item[nIdx].GetHeight()))
		{
#ifdef _SERVER
		msgshow("提示:子母袋空间不足S!");
		return 0;
#else
//客户端
        Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("提示:子母袋空间不足C!");
	   	return 0;
#endif
		}
		m_Items[i].nPlace = pos_equiproomex;
		m_Items[i].nX = nX;
		m_Items[i].nY = nY;
		break;
	case pos_immediacy:   //快捷栏
		if (!m_Room[room_immediacy].PlaceItem(nX, nY, nIdx, Item[nIdx].GetWidth(), Item[nIdx].GetHeight()))
		{
#ifdef _SERVER
			msgshow("提示:快捷栏空间有限!");
#endif
			return 0;
		}
		m_Items[i].nPlace = pos_immediacy;
		m_Items[i].nX = nX;
		m_Items[i].nY = nY;		
		break;		
	case pos_give:        //给予界面
		if (!m_Room[room_give].PlaceItem(nX, nY, nIdx, Item[nIdx].GetWidth(), Item[nIdx].GetHeight()))
		{
#ifdef _SERVER
			msgshow("提示:给予空间有限!");
#endif
			return 0;
		}
		m_Items[i].nPlace = pos_give;
		m_Items[i].nX = nX;
		m_Items[i].nY = nY;		
		break;
	case pos_dazao:        //打造界面
		/*if (!m_Room[room_dazao].PlaceItem(nX, nY, nIdx, Item[nIdx].GetWidth(), Item[nIdx].GetHeight()))
		{
#ifdef _SERVER
			msgshow("提示:铸造空间有限A!");
#endif
			return 0;
		}*/
		m_ItemBox[nX]=nIdx;
		m_Items[i].nPlace = pos_dazao;
		m_Items[i].nX = nX;
		m_Items[i].nY = nY;		
		break;
	case pos_cailiao:        //打造界面
		if (!m_Room[room_cailiao].PlaceItem(nX, nY, nIdx, Item[nIdx].GetWidth(), Item[nIdx].GetHeight()))
		{
#ifdef _SERVER   
			msgshow("提示:铸造空间有限B!");
#endif
			return 0;
		}
		m_Items[i].nPlace = pos_cailiao;
		m_Items[i].nX = nX;
		m_Items[i].nY = nY;		
		break;
	default:
		return 0;
	}

	m_Items[i].nIdx = nIdx;
	m_Items[i].nPrice = 0;
	m_Items[i].nLianJieFlg =0;
	m_Items[i].nModel =0;
	m_Items[i].nIsLock =false;

	m_FreeIdxItemList.Remove(i); // 移除未用表
	m_UseIdxItemList.Insert(i);  // 添加可用表
//	m_ItemCount ++;

	if (m_Items[i].nPlace == pos_equip)
	{//装备这个装备
		Equip(m_Items[i].nIdx,nX,nIsLoign);
	}

	if (m_Items[i].nPlace == pos_dazao)
	{//装备这个装备
		if (nX>=53 && nX<=77)
		   TonEquip(m_Items[i].nIdx,nX,nIsLoign);
	}

#ifdef _SERVER //同步装备属性
	ITEM_SYNC	sItem;
	memset(&sItem, 0, sizeof(sItem)); 
	sItem.ProtocolType = s2c_syncitem;

//	if (StackNum>1)
//		Item[nIdx].SetStackNum(StackNum);

	sItem.m_Point        = Item[nIdx].GetPoint();
	sItem.m_Rongpiont    = Item[nIdx].IsRong(); //熔炼
	sItem.m_EnChance     = Item[nIdx].GetEnChance();
	sItem.m_GoldId       = Item[nIdx].GetGoldId();
	sItem.m_IsPlatima    = Item[nIdx].GetIsPlatima();
	sItem.m_UseMap       = Item[nIdx].GetUseMapIdx();
	sItem.m_UseKind		 = Item[nIdx].GetUseKind();
	sItem.m_StackNum     = Item[nIdx].GetStackNum();
	sItem.m_ID           = Item[nIdx].GetID();   //物品的 dwID
	sItem.m_Genre        = Item[nIdx].GetGenre();
	sItem.m_Detail       = Item[nIdx].GetDetailType();
	sItem.m_Particur     = Item[nIdx].GetParticular();
	sItem.m_Series       = Item[nIdx].GetSeries();
	sItem.m_Level        = Item[nIdx].GetLevel();
	sItem.m_Res          = Item[nIdx].GetItemResIdx();
	sItem.m_Luck         = Item[nIdx].m_GeneratorParam.nLuck;
	sItem.m_btPlace      = m_Items[i].nPlace; //同步目标容器
	sItem.m_btX          = m_Items[i].nX;     //同步位置
	sItem.m_btY          = m_Items[i].nY;
	sItem.m_Time.bYear   = Item[nIdx].GetTime()->bYear; 
	sItem.m_Time.bMonth  = Item[nIdx].GetTime()->bMonth;
	sItem.m_Time.bDay    = Item[nIdx].GetTime()->bDay;
	sItem.m_Time.bHour   = Item[nIdx].GetTime()->bHour;
	sItem.m_Time.bMin    = Item[nIdx].GetTime()->bMin;
	sItem.m_LockItem	 = *Item[nIdx].GetLockItem();
/*	printf("sItem.m_LockItem -> nState: %d, dwLockTime: %d\n", 
			sItem.m_LockItem.nState, 
			sItem.m_LockItem.dwLockTime);*/
    sItem.m_IsBang       = Item[nIdx].GetIsBang();      //是否绑定

	if (Item[nIdx].GetTradePrice() <= 0 || Item[nIdx].GetTradePrice() > 999999999 || Item[nIdx].GetLockItem()->IsLockItem())
		sItem.m_TradePrice	 = 0;
	else
		sItem.m_TradePrice	 = Item[nIdx].GetTradePrice();

	//printf("GetTradePrice: %d,\n", sItem.m_TradePrice);

	sItem.m_IsKuaiJie    = Item[nIdx].GetIsKuaiJie();   //是否快捷栏
	sItem.m_IsMagic      = Item[nIdx].GetIsMagic();     //是否有魔法属性
	sItem.m_SkillType    = Item[nIdx].GetSkillType();
	sItem.m_MagicID      = Item[nIdx].GetMagicID();
    sItem.m_IsWhere      = Item[nIdx].GetIsWhere();
	sItem.m_IsCanUse     = Item[nIdx].GetIsCanUse();
    sItem.m_IsLoign      = (BYTE)nIsLoign;
	sItem.m_RandomSeed = Item[nIdx].m_GeneratorParam.uRandomSeed;
	sItem.m_Version    = Item[nIdx].m_GeneratorParam.nVersion;
	sItem.m_Durability = Item[nIdx].GetDurability();
    snprintf(sItem.m_WonName, sizeof(sItem.m_WonName), "%s", Item[nIdx].GetWonName());  //所有者
   //printf("包袱 m_RandomSeed :%u m_EnChance:%d \n",sItem.m_RandomSeed,sItem.m_EnChance);
    int j;
	for (j = 0; j < 6; ++j)//同步魔法属性
	{
		sItem.m_MagicLevel[j] = Item[nIdx].m_GeneratorParam.nGeneratorLevel[j]; 
		sItem.m_RongMagicLev[j] = Item[nIdx].m_GeneratorParam.nRGeneratorLevel[j];
//		sItem.m_BaoMagicLev[j] = Item[nIdx].m_GeneratorParam.nBGeneratorLevel[j];

		if (sItem.m_Point==8)
		{
			//sItem.m_MagInfo[j].nMagType=Item[nIdx].GetAttribType(j);
			//sItem.m_MagInfo[j].nMagVala=Item[nIdx].GetAttribTypeVala(j);
			//sItem.m_MagInfo[j].nMagValb=Item[nIdx].GetAttribTypeValb(j);
			//sItem.m_MagInfo[j].nMagValc=Item[nIdx].GetAttribTypeValc(j);
		}
	}

	for (j = 0; j < 7; ++j)
	{
		sItem.m_JBLevel[j]      = Item[nIdx].m_GeneratorParam.nJGeneratorLevel[j];
	}

    ZeroMemory(sItem.m_ItmeInfo,sizeof(sItem.m_ItmeInfo));

 	if (Item[nIdx].GetSItmeInfo() && strstr(Item[nIdx].GetSItmeInfo(),".lua"))//strrchr
	{
		KLuaScript * pScript = (KLuaScript*)g_GetScript(Item[nIdx].GetSItmeInfo());//g_FileName2Id(
		if (pScript)
		{
			int nSafeIndex = 0;
			nSafeIndex=pScript->SafeCallBegin();
			//执行这个脚本的函数
			try
			{
			   pScript->CallFunction("GetItemDescription",1,"sdddddds", Item[nIdx].GetName(),Item[nIdx].GetItemBaseMagic(0),Item[nIdx].GetItemBaseMagic(1),Item[nIdx].GetItemBaseMagic(2),Item[nIdx].GetItemBaseMagic(3),Item[nIdx].GetItemBaseMagic(4),Item[nIdx].GetItemBaseMagic(5),"");
			   const char * szType = lua_typename(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState));
			   if (Lua_IsNumber(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState)) == 1)
			   { //Lua_PushNumber
				  int nResult = (int)Lua_ValueToNumber(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState));
				  _snprintf(sItem.m_ItmeInfo,sizeof(sItem.m_ItmeInfo)-1, "%d", nResult);  // int 类型
			   }  
			   else if (Lua_IsString(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState)) == 1)  // 字符串类型
				  snprintf(sItem.m_ItmeInfo, sizeof(sItem.m_ItmeInfo), "%s", (char *)Lua_ValueToString(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState)));
			   else
				  _snprintf(sItem.m_ItmeInfo,sizeof(sItem.m_ItmeInfo)-1,"<color=Red>数据错误,请联系GM处理!<color>");
			}
			catch (...)
			{
				 _snprintf(sItem.m_ItmeInfo,sizeof(sItem.m_ItmeInfo)-1,"<color=Red>数据错误,请联系GM处理!<color>");
			}
			nSafeIndex=pScript->SafeCallBegin();
			pScript->SafeCallEnd(nSafeIndex);
		}
		else
			_snprintf(sItem.m_ItmeInfo,sizeof(sItem.m_ItmeInfo)-1,"暂无数据");
	}
	g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sItem, sizeof(ITEM_SYNC));

	if (!bInit) //不能马上存档
	{
		if (Player[m_PlayerIdx].Save())	//生成数据包
		{
			//Player[m_PlayerIdx].m_uMustSave = SAVE_REQUEST;	 //增加东西立即存档
			//if (g_pDataClient)
			//{//存档
			//	Player[m_PlayerIdx].SavePlayerData(false);
			//}
		} 
	    //Player[m_PlayerIdx].m_uMustSave = SAVE_REQUEST;  //增加东西 马上存档
	}
#endif

#ifndef _SERVER
	KUiObjAtContRegion	pInfo;

	int PartConvert[itempart_num] = 
	{
		UIEP_HEAD,
		UIEP_BODY,
		UIEP_WAIST,
		UIEP_HAND,
		UIEP_FOOT,
		UIEP_FINESSE,
		UIEP_NECK,
		UIEP_FINGER1,
		UIEP_FINGER2,
		UIEP_WAIST_DECOR,
		UIEP_HORSE,
		UIEP_MASK,	// mat na
		UIEP_PIFENG,	//披风
		UIEP_YINJIAN,		//印鉴
	    UIEP_SHIPING,	//饰品
	};

	pInfo.Obj.uGenre = CGOG_ITEM;	//源装备
	pInfo.Obj.uId = nIdx;
	pInfo.Region.Width = Item[nIdx].GetWidth();
	pInfo.Region.Height = Item[nIdx].GetHeight();

	switch(nPlace) //客户端的目标容器
	{
	case pos_immediacy:
		pInfo.Region.h = nX;
		pInfo.Region.v = nY;
		pInfo.eContainer = UOC_IMMEDIA_ITEM;
		break;
	case pos_hand:
		pInfo.eContainer = UOC_IN_HAND;
		break;
	case pos_equip:
		pInfo.Region.h = 0;
		pInfo.Region.v = PartConvert[nX];
		pInfo.eContainer = UOC_EQUIPTMENT;
		break;
	case pos_equiproom: //客户端包袱
		pInfo.Region.h = nX;
		pInfo.Region.v = nY;
		pInfo.eContainer = UOC_ITEM_TAKE_WITH;
		break;
	case pos_repositoryroom:
		pInfo.Region.h = nX;
		pInfo.Region.v = nY;
		pInfo.eContainer = UOC_STORE_BOX;
		break;
	case pos_exbox1room: //
		pInfo.Region.h = nX;
		pInfo.Region.v = nY;
		pInfo.eContainer = UOC_EX_BOX1;
		break;
	case pos_exbox2room: //
		pInfo.Region.h = nX;
		pInfo.Region.v = nY;
		pInfo.eContainer = UOC_EX_BOX2;
		break;
	case pos_exbox3room: //
		pInfo.Region.h = nX;
		pInfo.Region.v = nY;
		pInfo.eContainer = UOC_EX_BOX3;
		break;
	case pos_equiproomex: // 子母袋
		pInfo.Region.h = nX;
		pInfo.Region.v = nY;
		pInfo.eContainer = UOC_ITEM_EX;
		break;
	case pos_trade1:
		pInfo.Region.h = nX;
		pInfo.Region.v = nY;
		break;
	case pos_dazao:
		pInfo.Region.h = nX;
		pInfo.Region.v = nY;
		pInfo.eContainer = UOC_ITEM_DAZAO;
		break;
	case pos_cailiao:
		pInfo.Region.h = nX;
		pInfo.Region.v = nY;
		pInfo.eContainer = UOC_ITEM_CAILIAO;
		break;
	}
	if (nPlace != pos_trade1) //通知客户端物品增加
		CoreDataChanged(GDCNI_OBJECT_CHANGED, (DWORD)&pInfo, 1);
	else
	{//增加
		CoreDataChanged(GDCNI_TRADE_DESIRE_ITEM, (DWORD)&pInfo, 1);
	}
/*
#ifdef _DEBUG
	int nLoopIdx = 0;
	nLoopIdx = m_UseIdx.GetNext(nLoopIdx);
	while(nLoopIdx)
	{
		g_DebugLog("[ITEM]ItemListIdx:%d, Item:%d, ItemId:%d", nLoopIdx, m_Items[nLoopIdx].nIdx, Item[m_Items[nLoopIdx].nIdx].GetID());
		nLoopIdx = m_UseIdx.GetNext(nLoopIdx);
	}
#endif
*/
#endif
	return i;
}

/*!*****************************************************************************
// Function		: KItemList::Remove
// Purpose		: 玩家失去一个装备
// Return		: int 
// Argumant		: int nGameIdx为游戏世界中道具数组的编号
// Comments		: 删除容器中的物品。。。。
// Author		: Spe
*****************************************************************************/
BOOL KItemList::Remove(int nGameIdx,int ndX,int ndY,int nModel,BOOL nIsSave)
{
	if (!nGameIdx)
		return FALSE;

	int nFIdx = FindSame(nGameIdx);//查找已用表

	if (!nFIdx)
		return FALSE;

	/*if (m_Items[nFIdx].nIsLock)
	return;*/

#ifdef _SERVER //通知客户端删除
	  ITEM_REMOVE_SYNC	sRemove;
	  sRemove.ProtocolType = s2c_removeitem;
	  sRemove.m_ID         = Item[nGameIdx].m_dwID;	
      sRemove.m_dx         = ndX;
	  sRemove.m_dy         = ndY;
	  sRemove.m_model   = nModel;
	  sRemove.nPlace       =-1;
#endif

	switch(m_Items[nFIdx].nPlace)
	{
	case pos_hand:
		m_Hand = 0;
		break;
	case pos_equip:
		UnEquip(m_Items[nFIdx].nIdx);
		break;
	case pos_immediacy:
		m_Room[room_immediacy].PickUpItem(
			nGameIdx,
			m_Items[nFIdx].nX,
			m_Items[nFIdx].nY,
			Item[m_Items[nFIdx].nIdx].GetWidth(),
			Item[m_Items[nFIdx].nIdx].GetHeight());
		break;
	case pos_equiproom:
		{
		m_Room[room_equipment].PickUpItem(
				nGameIdx,
	    m_Items[nFIdx].nX,
		m_Items[nFIdx].nY,
				Item[m_Items[nFIdx].nIdx].GetWidth(),
			Item[m_Items[nFIdx].nIdx].GetHeight());
		}
		break;
	case pos_repositoryroom:
		m_Room[room_repository].PickUpItem(
			nGameIdx,
			m_Items[nFIdx].nX,
			m_Items[nFIdx].nY,
			Item[m_Items[nFIdx].nIdx].GetWidth(),
			Item[m_Items[nFIdx].nIdx].GetHeight());
		break;
	case pos_exbox1room: // ruong mo rong 1
		m_Room[room_exbox1].PickUpItem(
			nGameIdx,
			m_Items[nFIdx].nX,
			m_Items[nFIdx].nY,
			Item[m_Items[nFIdx].nIdx].GetWidth(),
			Item[m_Items[nFIdx].nIdx].GetHeight());
		break;
	case pos_exbox2room: // ruong mo rong 2
		m_Room[room_exbox2].PickUpItem(
			nGameIdx,
			m_Items[nFIdx].nX,
			m_Items[nFIdx].nY,
			Item[m_Items[nFIdx].nIdx].GetWidth(),
			Item[m_Items[nFIdx].nIdx].GetHeight());
		break;
	case pos_exbox3room: // ruong mo rong 3
		m_Room[room_exbox3].PickUpItem(
			nGameIdx,
			m_Items[nFIdx].nX,
			m_Items[nFIdx].nY,
			Item[m_Items[nFIdx].nIdx].GetWidth(),
			Item[m_Items[nFIdx].nIdx].GetHeight());
		break;
	case pos_equiproomex: // mo rong hanh trang
		m_Room[room_equipmentex].PickUpItem(
			nGameIdx,
			m_Items[nFIdx].nX,
			m_Items[nFIdx].nY,
			Item[m_Items[nFIdx].nIdx].GetWidth(),
			Item[m_Items[nFIdx].nIdx].GetHeight());
		break;
	case pos_traderoom:
		m_Room[room_trade].PickUpItem(
			nGameIdx,
			m_Items[nFIdx].nX,
			m_Items[nFIdx].nY,
			Item[m_Items[nFIdx].nIdx].GetWidth(),
			Item[m_Items[nFIdx].nIdx].GetHeight());
		break;
	case pos_give:
		m_Room[room_give].PickUpItem(
			nGameIdx,
			m_Items[nFIdx].nX,
			m_Items[nFIdx].nY,
			Item[m_Items[nFIdx].nIdx].GetWidth(),
			Item[m_Items[nFIdx].nIdx].GetHeight());
		break;
	case pos_dazao:
		/*m_Room[room_dazao].PickUpItem(
			nGameIdx,
			m_Items[nFIdx].nX,
			m_Items[nFIdx].nY,
			Item[m_Items[nFIdx].nIdx].GetWidth(),
			Item[m_Items[nFIdx].nIdx].GetHeight());*/
		   if  (m_Items[nFIdx].nX>=53 && m_Items[nFIdx].nX<=77)
			   UnTonEquip(m_Items[nFIdx].nIdx);
			   
			RemoveItemBox(m_Items[nFIdx].nIdx);
			//m_ItemBox[ndX]=0;
		break;
	case pos_cailiao:
		m_Room[room_cailiao].PickUpItem(
			nGameIdx,
			m_Items[nFIdx].nX,
			m_Items[nFIdx].nY,
			Item[m_Items[nFIdx].nIdx].GetWidth(),
			Item[m_Items[nFIdx].nIdx].GetHeight());
		break;
#ifndef _SERVER
	case pos_trade1:
		if (!Player[CLIENT_PLAYER_INDEX].CheckTrading())
		{
			_ASSERT(0);
		}
		m_Room[room_trade1].PickUpItem(
			nGameIdx,
			m_Items[nFIdx].nX,
			m_Items[nFIdx].nY,
			Item[m_Items[nFIdx].nIdx].GetWidth(),
			Item[m_Items[nFIdx].nIdx].GetHeight());
		break;
#endif
	default:
		return FALSE;
	}

#ifndef _SERVER
    
	 // 客户端从玩家身上去除装备就应该从装备表中去除掉。
	ItemSet.Remove(m_Items[nFIdx].nIdx);    //节点处理
	//界面处理
	KUiObjAtContRegion pInfo;
      memset(&pInfo, 0, sizeof(KUiObjAtContRegion));

	int PartConvert[itempart_num] = 
	{
		UIEP_HEAD,
		UIEP_BODY,
		UIEP_WAIST,
		UIEP_HAND,
		UIEP_FOOT,
		UIEP_FINESSE,
		UIEP_NECK,
		UIEP_FINGER1,
		UIEP_FINGER2,
		UIEP_WAIST_DECOR,
		UIEP_HORSE,
		UIEP_MASK,	    // mat na
		UIEP_PIFENG,	//披风
		UIEP_YINJIAN,	//印鉴
	    UIEP_SHIPING,	//饰品
	};

	pInfo.Obj.uGenre = CGOG_ITEM;
	pInfo.Obj.uId = m_Items[nFIdx].nIdx;
	pInfo.Region.Width = Item[m_Items[nFIdx].nIdx].GetWidth();
	pInfo.Region.Height = Item[m_Items[nFIdx].nIdx].GetHeight();
//以下结构发向界面更新物品
	switch(m_Items[nFIdx].nPlace)
	{
	case pos_hand:
		pInfo.Obj.uGenre = CGOG_NOTHING;
		pInfo.Region.h = 0;
		pInfo.Region.v = 0;
		pInfo.eContainer = UOC_IN_HAND;
		break;
	case pos_equiproom:
		{
          if (nModel==1)
		  {   
		    pInfo.Region.h =ndX;
		    pInfo.Region.v =ndY;
		  }   
		  else
		  { 
			pInfo.Region.h = m_Items[nFIdx].nX;
	 	    pInfo.Region.v = m_Items[nFIdx].nY;
		  } 
		  pInfo.eContainer = UOC_ITEM_TAKE_WITH;
		}
		break;
	case pos_repositoryroom:
		pInfo.Region.h = m_Items[nFIdx].nX;
		pInfo.Region.v = m_Items[nFIdx].nY;
		pInfo.eContainer = UOC_STORE_BOX;
		break;
	case pos_exbox1room: // ruong mo rong 1
		pInfo.Region.h = m_Items[nFIdx].nX;
		pInfo.Region.v = m_Items[nFIdx].nY;
		pInfo.eContainer = UOC_EX_BOX1;
		break;
	case pos_exbox2room: // ruong mo rong 2
		pInfo.Region.h = m_Items[nFIdx].nX;
		pInfo.Region.v = m_Items[nFIdx].nY;
		pInfo.eContainer = UOC_EX_BOX2;
		break;
	case pos_exbox3room: // ruong mo rong 3
		pInfo.Region.h = m_Items[nFIdx].nX;
		pInfo.Region.v = m_Items[nFIdx].nY;
		pInfo.eContainer = UOC_EX_BOX3;
		break;
	case pos_equiproomex: // mo rong hanh trang
		pInfo.Region.h = m_Items[nFIdx].nX;
		pInfo.Region.v = m_Items[nFIdx].nY;
		pInfo.eContainer = UOC_ITEM_EX;
		break;
	case pos_immediacy:
		pInfo.Region.h = m_Items[nFIdx].nX;
		pInfo.Region.v = m_Items[nFIdx].nY;
		pInfo.eContainer = UOC_IMMEDIA_ITEM;
		break;
	/*case pos_give:
		pInfo.Region.h = m_Items[nFIdx].nX;
		pInfo.Region.v = m_Items[nFIdx].nY;
		pInfo.eContainer = UOC_ITEM_GIVE;
		break;*/
	case pos_dazao:
		pInfo.Region.h = m_Items[nFIdx].nX;
		pInfo.Region.v = m_Items[nFIdx].nY;
		pInfo.Region.IsRongqi=0;
		pInfo.eContainer = UOC_ITEM_DAZAO;
		break;
	case pos_cailiao:
		pInfo.Region.h = m_Items[nFIdx].nX;
		pInfo.Region.v = m_Items[nFIdx].nY;
        pInfo.Region.IsRongqi=1;
		pInfo.eContainer = UOC_ITEM_CAILIAO;
		break;
	case pos_equip:
		pInfo.Region.h = 0;
		pInfo.Region.v = PartConvert[m_Items[nFIdx].nX];
		pInfo.eContainer = UOC_EQUIPTMENT;
		break;
	case pos_trade1:
		pInfo.Region.h = m_Items[nFIdx].nX;
		pInfo.Region.v = m_Items[nFIdx].nY;
		break;
	}
	if (m_Items[nFIdx].nPlace != pos_trade1)
	{
		CoreDataChanged(GDCNI_OBJECT_CHANGED, (DWORD)&pInfo, 0);  //通知客户端改变更新
	}
	else
	{
		CoreDataChanged(GDCNI_TRADE_DESIRE_ITEM, (DWORD)&pInfo, 0);
	}

if (!Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_AutoplayId)
{
	KSystemMessage	sMsg;
	sMsg.eType = SMT_NORMAL;
	sMsg.byConfirmType = SMCT_NONE;
	sMsg.byPriority = 1;
	sMsg.byParamSize = 0;

    if (Item[m_Items[nFIdx].nIdx].GetTime()->bYear>0)
		sprintf(sMsg.szMessage, "限时:<color=yellow>使用时间到,回收(%s)OK.", Item[m_Items[nFIdx].nIdx].GetName());
	else
	{
		if (Npc[Player[m_PlayerIdx].m_nIndex].m_BaiTan)
		    sprintf(sMsg.szMessage, "摆摊:<color=yellow>卖掉(%s)OK.", Item[m_Items[nFIdx].nIdx].GetName());
		else
			sprintf(sMsg.szMessage, "物品:<color=yellow>消耗(%s)OK.", Item[m_Items[nFIdx].nIdx].GetName());
	}
	sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
	CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
}

#endif

	m_Items[nFIdx].nIdx = 0;
	m_Items[nFIdx].nPlace = 0;
	m_Items[nFIdx].nX = 0;
	m_Items[nFIdx].nY = 0;
	m_Items[nFIdx].nPrice = 0;
	m_Items[nFIdx].nLianJieFlg =0;
	m_Items[nFIdx].nModel =0;
	m_Items[nFIdx].nIsLock =false;

	m_FreeIdxItemList.Insert(nFIdx);  //增加一个可用节点
	m_UseIdxItemList.Remove(nFIdx);   //删除一个已用节点
	//m_ItemCount --;
	//if  (m_ItemCount<0)
//		m_ItemCount =0;

#ifdef _SERVER
	g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sRemove, sizeof(ITEM_REMOVE_SYNC));
	
	if (nIsSave)
	     Player[m_PlayerIdx].Save();                         //生成 新存档包 
	   //删除物品前生成一个数据包,用于存
#endif
	
	return TRUE;
}

/*!*****************************************************************************
// Function		: KItemList::FindFree
// Purpose		: 查找可用空索引
// Return		: int 
// Comments		:
// Author		: Spe
*****************************************************************************/
int KItemList::FindFree()
{
	return m_FreeIdxItemList.GetNext(0); //未用表
}

/*!*****************************************************************************
// Function		: KItemList::FindSame
// Purpose		: 
// Return		: int 
// Argumant		: int nGameIdx	  物品的索引
// Comments		:
// Author		: Spe
*****************************************************************************/
int KItemList::FindSame(int nGameIdx)
{
	int mIdx = 0;
	while(1)
	{
		mIdx = m_UseIdxItemList.GetNext(mIdx); //可用表
		if (!mIdx)
			break;
		
		if (m_Items[mIdx].nIdx == nGameIdx)
			return mIdx;
	}
	return 0;
}
/*!*****************************************************************************
// Function		: KItemList::Init
// Purpose		: 初始化玩家装备列表
// Return		: BOOL
// Comments		:
// Author		: Spe 初始化容器大小
*****************************************************************************/
BOOL KItemList::Init(int nPlayerIdx)
{
	m_PlayerIdx = nPlayerIdx;
	m_Hand = 0;
	m_nBackHand = 0;
	m_nMaskLock = FALSE;	// 面具锁
	//m_ItemCount = 0;
	m_Room[room_equipment].Init(EQUIPMENT_ROOM_WIDTH,EQUIPMENT_ROOM_HEIGHT);
	m_Room[room_repository].Init(REPOSITORY_ROOM_WIDTH,REPOSITORY_ROOM_HEIGHT); //储物箱
	m_Room[room_trade].Init(TRADE_ROOM_WIDTH,TRADE_ROOM_HEIGHT);
	m_Room[room_exbox1].Init(REPOSITORY_ROOM_WIDTH,REPOSITORY_ROOM_HEIGHT);
	m_Room[room_exbox2].Init(REPOSITORY_ROOM_WIDTH,REPOSITORY_ROOM_HEIGHT); 
	m_Room[room_exbox3].Init(REPOSITORY_ROOM_WIDTH,REPOSITORY_ROOM_HEIGHT);
	m_Room[room_equipmentex].Init(REPOSITORY_ROOM_WIDTH,REPOSITORY_ROOM_HEIGHT);
	m_Room[room_tradeback].Init(EQUIPMENT_ROOM_WIDTH,EQUIPMENT_ROOM_HEIGHT);
	m_Room[room_immediacy].Init(IMMEDIACY_ROOM_WIDTH,IMMEDIACY_ROOM_HEIGHT);
	m_Room[room_give].Init(EQUIPMENT_ROOM_WIDTH,GIVE_ROOM_HEIGHT); // TRADE_ROOM_HEIGHT
	m_Room[room_giveback].Init(EQUIPMENT_ROOM_WIDTH,EQUIPMENT_ROOM_HEIGHT);
	//if (!m_Room[room_dazao].Init(DAZAO_ROOM_WIDTH,DAZAO_ROOM_HEIGHT))
	//      printf(" [room_dazao] 内存初始化失败A\n");
	if (!m_Room[room_cailiao].Init(CAILIAO_ROOM_WIDTH, CAILIAO_ROOM_HEIGHT))
          printf(" [room_cailiao] 内存初始化失败A\n");

#ifndef _SERVER
	m_Room[room_trade1].Init(TRADE_ROOM_WIDTH,TRADE_ROOM_HEIGHT);		// 这个的大小必须与 room_trade 的大小一样
#endif

	ZeroMemory(m_EquipItem, sizeof(m_EquipItem));				// 玩家装备的道具（对应游戏世界中道具数组的索引）
	ZeroMemory(m_Items, sizeof(m_Items));						// 玩家拥有的所有道具（包括装备着的和箱子里放的，对应游戏世界中道具数组的索引）
	ZeroMemory(m_sBackItems, sizeof(m_sBackItems));
	ZeroMemory(m_refItem, sizeof(m_refItem));
	m_nListCurIdx = 0;											// 用于 GetFirstItem 和 GetNextItem
	
	m_FreeIdxItemList.Init(MAX_PLAYER_ITEM);
	m_UseIdxItemList.Init(MAX_PLAYER_ITEM);

	for (int i = MAX_PLAYER_ITEM - 1; i > 0 ; i--)
	{
		m_FreeIdxItemList.Insert(i);
	}
	return TRUE;
}

/*!*****************************************************************************
// Function		: KItemList::CanEquip
// Purpose		: 
// Return		: BOOL 
// Argumant		: int nIdx
// Argumant		: int nPlace
// Comments		:
// Author		: Spe----  是否可以装备这个同伴
*****************************************************************************/
BOOL KItemList::CanTonEquip(int nIdx, int nPlace)
{
	if (m_PlayerIdx <= 0 || nIdx <= 0 || nIdx >= MAX_ITEM || Item[nIdx].GetGenre() != item_equip)
		return FALSE;

	int nNpcIdx = Player[m_PlayerIdx].m_nIndex,nTonNpcIdx=0;

	KMagicAttrib* pData = NULL;
	
	if (Npc[nNpcIdx].m_TongBanNum)
	{//已经召唤了宠物的	
		nTonNpcIdx =NpcSet.SearchID(Npc[nNpcIdx].m_TongBanNum);
	}

	if (nTonNpcIdx<=0)
	{	
		return FALSE;
	}

  /*
	if (nPlace != -1 && !Fit(nIdx, nPlace))
	{
		return FALSE;
	}*/

	 if (!Item[nIdx].CheckRequirement(magic_item_needtongban))
	 {//如果这个装备没有这个 同伴的需求属性
		 
#ifdef _SERVER
		 Player[m_PlayerIdx].m_ItemList.msgshow("<color=yellow>神将<color>:请使用神将专用装备!");
#endif
		 return FALSE;
	 }



	int nCount = 0;
	do
	{
		pData = (KMagicAttrib*)Item[nIdx].GetRequirement(nCount);
		if (pData && !EnoughAttrib(pData,nPlace,Item[nIdx].GetID()))
			return FALSE;
		nCount++;
	} while (pData != NULL);
	return TRUE;
}



/*!*****************************************************************************
// Function		: KItemList::CanEquip
// Purpose		: 
// Return		: BOOL 
// Argumant		: int nIdx
// Argumant		: int nPlace
// Comments		:
// Author		: Spe----
*****************************************************************************/
BOOL KItemList::CanEquip(int nIdx, int nPlace)
{
	if (m_PlayerIdx <= 0 || nIdx <= 0 || nIdx >= MAX_ITEM || Item[nIdx].GetGenre() != item_equip)
		return FALSE;

	int nNpcIdx = Player[m_PlayerIdx].m_nIndex;

	KMagicAttrib* pData = NULL;

	if (nPlace != -1 && !Fit(nIdx, nPlace))
	{
		return FALSE;
	}

	int nCount = 0;
	do
	{
		pData = (KMagicAttrib*)Item[nIdx].GetRequirement(nCount);
		if (pData && !EnoughAttrib(pData,nPlace,Item[nIdx].GetID()))
			return FALSE;
		nCount++;
	} while (pData != NULL);

//---------------------------锁魂检测
	//Item[nIdx].GetWonName() ; //归属者

//---------------------------
	return TRUE;
}

BOOL KItemList::CanEquip(KItem* pItem, int nPlace /* = -1 */)
{
	if (m_PlayerIdx <= 0 || !pItem)
		return FALSE;

	int nNpcIdx = Player[m_PlayerIdx].m_nIndex;

	KMagicAttrib* pData = NULL;

	if (nPlace != -1 && !Fit(pItem, nPlace))
	{
		return FALSE;
	}

	int nCount = 0;
	do
	{
		pData = (KMagicAttrib*)pItem->GetRequirement(nCount);
		if (pData && !EnoughAttrib(pData,nPlace,pItem->GetID()))
			return FALSE;
		nCount++;
	} while (pData != NULL);
	return TRUE;
}
//需求属性 是否可以装备
BOOL KItemList::EnoughAttrib(void* pAttrib,int nPlace,DWORD nDwIdx)
{
	//SearchID
	KMagicAttrib*	pData = (KMagicAttrib *)pAttrib;
	_ASSERT(pData);
	int nNpcIdx=0;

	if (nDwIdx>0)
	{
		int nIdx = SearchID(nDwIdx);
		if (nIdx>0)
		{
			if (Item[nIdx].GetUseKind()==1 && Item[nIdx].GetUseMapIdx()>0)
			{//无任何要求
				return TRUE;
			}
		}
	}
	
	switch(pData->nAttribType)
	{
	case magic_requirestr:
		if (Player[m_PlayerIdx].m_nCurStrength < pData->nValue[0])
		{
			return FALSE;
		}
		break;
	case magic_requiredex:
		if (Player[m_PlayerIdx].m_nCurDexterity < pData->nValue[0])
		{
			return FALSE;
		}
		break;
	case magic_requirevit:
		if (Player[m_PlayerIdx].m_nCurVitality < pData->nValue[0])
		{
			return FALSE;
		}
		break;
	case magic_requireeng:
		if (Player[m_PlayerIdx].m_nCurEngergy < pData->nValue[0])
		{
			return FALSE;
		}
		break;
	case magic_requirelevel:
		{
		    if (Npc[Player[m_PlayerIdx].m_nIndex].m_Level < pData->nValue[0])
			{ //需求等级
			   return FALSE;
			}  
		}      
		break;
	case magic_requiremenpai:
		//if (Player[m_PlayerIdx].m_cFaction.m_nFirstAddFaction != pData->nValue[0])
		if (Player[m_PlayerIdx].m_cFaction.m_nCurFaction != pData->nValue[0])
		{//需求门派
			if (Player[m_PlayerIdx].m_cTask.GetSaveVal(130)!=(pData->nValue[0]+1))
				return FALSE;
		}
		break;
	case magic_requireseries:
		if (Npc[Player[m_PlayerIdx].m_nIndex].m_Series != pData->nValue[0])
		{//需求五行
			return FALSE;
		}
		break;
	case magic_requiresex:
		if (Npc[Player[m_PlayerIdx].m_nIndex].m_nSex != pData->nValue[0])
		{//需求男女
			return FALSE;
		}
		break;	 
	case magic_item_nouser:	  //不能使用的门派
		if (Player[m_PlayerIdx].m_cFaction.m_nFirstAddFaction == pData->nValue[0])
		{
			return FALSE;
		}
		break;
	case magic_item_noseries: //某五行不能使用
		if (Npc[Player[m_PlayerIdx].m_nIndex].m_Series == pData->nValue[0])
		{
			return FALSE;
		}
		break;
	case magic_item_needskill:	  //需要学会什么技能才能装备
		if (!Npc[Player[m_PlayerIdx].m_nIndex].m_SkillList.CheckHaveSkill(pData->nValue[0]))
		{	
			return FALSE;
		}
		break;
	case magic_item_needreborn:	  //需要转生次数才能装备
		if (Player[m_PlayerIdx].m_cReBorn.GetReBornNum()<pData->nValue[0])
		{	
			return FALSE;
		}
		break;
	case magic_item_needcity:	  //需要城主专用 ---针对帮战地图
		{
			if (!Player[m_PlayerIdx].m_cTong.m_nFlag)
			{//入帮 并是 帮主
#ifdef _SERVER
				Player[m_PlayerIdx].m_ItemList.msgshow("提示:该装备需要入帮才可以使用!");
#endif
				return FALSE;
			}
			if  (Player[m_PlayerIdx].m_cTong.GetFigure()== enumTONG_FIGURE_MASTER)
			{
#ifdef _SERVER
				Player[m_PlayerIdx].m_ItemList.msgshow("提示:该装备需要帮主才可以使用!");
#endif
				return FALSE;
			}
		}
		break;
	case magic_item_needbangzhu:	  //需要帮主专用 ---针对帮战地图
		{
		   if (!Player[m_PlayerIdx].m_cTong.m_nFlag)
		   {//入帮 并是 帮主
#ifdef _SERVER
			   Player[m_PlayerIdx].m_ItemList.msgshow("提示:该装备需要入帮才可以使用!");
#endif
			   return FALSE;
		   }
		   if  (Player[m_PlayerIdx].m_cTong.GetFigure()== enumTONG_FIGURE_MASTER)
		   {
#ifdef _SERVER
			   Player[m_PlayerIdx].m_ItemList.msgshow("提示:该装备需要帮主才可以使用!");
#endif
			   return FALSE;
		   }
		}
		break;
	case magic_item_needtongban:  //需要激活并召唤了同伴才可以装备
		if (Npc[Player[m_PlayerIdx].m_nIndex].m_TongBanNum)
		{//已经召唤了宠物的			
		    nNpcIdx =NpcSet.SearchID(Npc[Player[m_PlayerIdx].m_nIndex].m_TongBanNum);
		}
		if (!nNpcIdx || nPlace!=pData->nValue[0]) //部位不形同
		{//需求同伴 并是 部位合适的
		
		///sprintf(mag,"不可以装备:P:%d;D:%d",nPlace,pData->nValue[0]);
#ifdef _SERVER
			Player[m_PlayerIdx].m_ItemList.msgshow("提示:该装备不可装备该部位!");
#endif
	 		return FALSE; 
		}
		break;	
	default:
		break;
	}
//-------------------------------------------锁魂检测 是否可以装备

//-------------------------------------------

	return TRUE;
}
/*!*****************************************************************************
// Function		: KItemList::Equip---同伴换装
// Purpose		: 
// Return		: BOOL
// Argumant		: int nIdx，游戏世界中的道具数组编号，要求一定是装备类道具
// Argumant		: int nPlace，身上装备的位置，-1自动找位置
// Comments		:
// Author		: Spe
*****************************************************************************/
BOOL KItemList::TonEquip(int nIdx, int nPlace,int nIsLoign)
{
   //人物的换装
	if (m_PlayerIdx <= 0 || nIdx <= 0 || item_equip != Item[nIdx].GetGenre())
		return FALSE;

	int nNpcIdx = Player[m_PlayerIdx].m_nIndex,nTonNpcIdx=0;

	if (Npc[nNpcIdx].m_TongBanNum)
	{//已经召唤了宠物的	
		nTonNpcIdx =NpcSet.SearchID(Npc[nNpcIdx].m_TongBanNum);
	}

	if (nTonNpcIdx<=0)
	{//同伴没有召唤的

#ifdef _SERVER
		Player[m_PlayerIdx].m_ItemList.msgshow("<color=yellow>神将<color>:请召唤神将后穿戴装备!");
#endif
		return FALSE;
	}

	int nItemListIdx = FindSame(nIdx);
	if (!nItemListIdx)
	{
		_ASSERT(0);
		return FALSE;
	}

	int nEquipPlace = nPlace;

/*	if (-1 == nEquipPlace)
	{
		nEquipPlace = GetEquipPlace(Item[nIdx].GetDetailType());
	}
	else if (!Fit(nIdx, nEquipPlace))
	{
		return FALSE;
	} */

	int nType = 0;
	// 换装 外观改变

	// 更新装备自身坐标
	m_ItemBox[nEquipPlace]       = nIdx;
	m_Items[nItemListIdx].nPlace = pos_dazao;
	m_Items[nItemListIdx].nX     = nEquipPlace;
	m_Items[nItemListIdx].nY     = 0;

	int nActive =3;// GetEquipEnhance(nEquipPlace); //获取该部位 被激活的

	int nActiveInfo[2];

	   ZeroMemory(nActiveInfo,sizeof(nActiveInfo));

	if (Item[nIdx].IsBlue())
	{//如果是蓝装
	   nActiveInfo[0] = 1;
       nActiveInfo[1] = Item[nIdx].GetIsWhere();
       
	} 
    else if  (Item[nIdx].IsPurple())
	{//如果是紫装
	   nActiveInfo[0] = 2;
       nActiveInfo[1] = Item[nIdx].GetIsWhere();
	} 
    else if  (Item[nIdx].IsGold())
	{//如果是黄装
		nActiveInfo[0] = 3;
		nActiveInfo[1] = Item[nIdx].GetLevel();//等级
	} 
    else
	{//白装
		nActiveInfo[0] = 0;
		nActiveInfo[1] = 0;
	}

    if (Item[nIdx].GetIsCanUse()==1)   //是可以使用的
	{
        Item[nIdx].ApplyMagicAttribToNPC(&Npc[nTonNpcIdx],nActive,nActiveInfo);  // 应用激活的魔法到NPC 身上
        ZeroMemory(nActiveInfo,sizeof(nActiveInfo));
	    //InfectionNextEquip(nEquipPlace, TRUE);                                //穿上 激活暗属性并应用魔法在NPC身上  
	}
    else if (Item[nIdx].GetIsCanUse()==0 && Item[nIdx].GetGenre()==item_equip && Item[nIdx].GetDetailType()==equip_mask) 
	{//面具 不受限制
		Item[nIdx].ApplyMagicAttribToNPC(&Npc[nTonNpcIdx], nActive,nActiveInfo);
		ZeroMemory(nActiveInfo,sizeof(nActiveInfo));
	    //InfectionNextEquip(nEquipPlace, TRUE);
	}
 /*
	if (itempart_weapon == nEquipPlace)
	{//换装武器的话，设置相关的基本攻击		
#ifndef _SERVER
		Player[CLIENT_PLAYER_INDEX].UpdateWeaponSkill(); //更新武器技能参数
#endif
		Player[m_PlayerIdx].SetNpcDamageAttrib();        //设置物理伤害相关
	}
//	if(Item[nIdx].GetGoldId()>0)
 */
	if (Item[nIdx].GetIsCanUse()==1)   //是可以使用的
	{
	   GetIfActive();     //黄金套装激活列表GetIfActive()或全身激活的;
	}
  
//	Player[m_PlayerIdx].UpdataCurData(nIsLoign);                 //更新自身的属性参数

	Npc[nTonNpcIdx].UpdataNpcCurData(m_PlayerIdx,nEquipPlace);
    Player[m_PlayerIdx].UpdataCurData(nIsLoign);

    
#ifdef _SERVER
	for (int i = 1;i < 6;++i)  //通知客户端改变全部技能数据
	{//技能列表
		if (Npc[nTonNpcIdx].m_SkillList.GetSkillId(i)!=1 &&\
			Npc[nTonNpcIdx].m_SkillList.GetSkillId(i)!=2 &&\
			Npc[nTonNpcIdx].m_SkillList.GetSkillId(i)!=53 &&\
			Npc[nTonNpcIdx].m_SkillList.GetCurrentLevelByIdx(i)>0)
		{
			// 把当前此技能的点数和剩余技能点发给客户端
		    ///---------------服务端开始给更新被动技能状态数据--------------------------------------
			KSkill * pSkill =(KSkill *)g_SkillManager.GetSkill(Npc[nTonNpcIdx].m_SkillList.m_Skills[i].SkillId, Npc[nTonNpcIdx].m_SkillList.m_Skills[i].SkillLevel);
			if (pSkill) 
			{
				//int nReg=0;
				//nReg = pSkill->NewCanCastSkill(nNpcIdx, -1, nNpcIdx);  //武器限制
				//如果该技能属于被动辅助技能时，则设置Npc状态
				if (pSkill->GetSkillStyle() == SKILL_SS_PassivityNpcState)
				{//有和当前武器相符的技能 或 无武器限制的 被动技能
					pSkill->Cast(nTonNpcIdx, -1, nTonNpcIdx,-1); //也设置了效果
				} 
			} 
			pSkill=NULL;
			///-------------------------------------------------------------------------------	
		}
	}

#endif
    //ReSkillEnhance(m_PlayerIdx,nNpcIdx);
	return TRUE;

}

/*!*****************************************************************************
// Function		: KItemList::Equip---换装
// Purpose		: 
// Return		: BOOL
// Argumant		: int nIdx，游戏世界中的道具数组编号，要求一定是装备类道具
// Argumant		: int nPlace，身上装备的位置，-1自动找位置
// Comments		:
// Author		: Spe
*****************************************************************************/
BOOL KItemList::Equip(int nIdx, int nPlace,int nIsLoign)
{//人物的换装
	int nNpcIdx = Player[m_PlayerIdx].m_nIndex;

	if (m_PlayerIdx <= 0 || nIdx <= 0 || nNpcIdx <= 0 || item_equip != Item[nIdx].GetGenre())
		return FALSE;	

//	(strcmp(nNpcName,Npc[nNpcID].Name) == 0)
//	if (strcmp(Item[nIdx].GetWonName(),"系统")!=0)
//		return FALSE;

	int nItemListIdx = FindSame(nIdx);
	if (!nItemListIdx)
		return FALSE;

	int nEquipPlace = nPlace;

	if (-1 == nEquipPlace)
		nEquipPlace = GetEquipPlace(Item[nIdx].GetDetailType());
	else if (!Fit(nIdx, nEquipPlace))
		return FALSE;

	int nType = 0;
	// 换装 外观改变
	switch(nEquipPlace)
	{
	case itempart_head:
		if (Item[nIdx].GetItemResIdx()>0)
		{
			Npc[nNpcIdx].m_HelmType = Item[nIdx].GetItemResIdx();
			break;
		}

		if (Item[nIdx].GetGoldId())
		{
			if (Item[nIdx].GetIsPlatima())
			   nType = g_ItemChangeRes.GetPlatinaRes(Item[nIdx].GetGoldId());
			else
			   nType = g_ItemChangeRes.GetGoldItemRes(Item[nIdx].GetGoldId());
		}

		if (nType)
			Npc[nNpcIdx].m_HelmType = nType;
		else
			Npc[nNpcIdx].m_HelmType = g_ItemChangeRes.GetHelmRes(Item[nIdx].GetParticular(),Item[nIdx].GetLevel());
		break;
	case itempart_body:
		{
#ifdef _SERVER
			Npc[nNpcIdx].m_ChiBangType=Item[nIdx].GetChiBangRes();
#endif	
		    if (Item[nIdx].GetItemResIdx()>0)
			{ 
			  Npc[nNpcIdx].m_ArmorType = Item[nIdx].GetItemResIdx();
			  break;
			}   

		    if (Item[nIdx].GetGoldId())
			{ 	
		      if (Item[nIdx].GetIsPlatima())
		        nType = 	g_ItemChangeRes.GetPlatinaRes(Item[nIdx].GetGoldId());
		      else
			    nType =  g_ItemChangeRes.GetGoldItemRes(Item[nIdx].GetGoldId());
			}   

		    if (nType)
			  Npc[nNpcIdx].m_ArmorType = nType;
		    else
        	  Npc[nNpcIdx].m_ArmorType = g_ItemChangeRes.GetArmorRes(Item[nIdx].GetParticular(),Item[nIdx].GetLevel());
		}	
		break;
	case itempart_weapon:

	    if (Item[nIdx].GetItemResIdx()>0)
		{
			Npc[nNpcIdx].m_WeaponType = Item[nIdx].GetItemResIdx();
			break;
		}
		
		if (Item[nIdx].GetGoldId())
		{
			if (Item[nIdx].GetIsPlatima())
				nType  = g_ItemChangeRes.GetPlatinaRes(Item[nIdx].GetGoldId());
			else
			    nType  = g_ItemChangeRes.GetGoldItemRes(Item[nIdx].GetGoldId());

            /*#ifdef _SERVER 
			char nMsg[64]={0};
			sprintf(nMsg,"--是否白金A: %d,武器类型: %d,ID:%d ",Item[nIdx].GetIsPlatima(),nType,Item[nIdx].GetGoldId());
			msgshow(nMsg);
            #endif
			*/
		}

		if (nType)
		{
			Npc[nNpcIdx].m_WeaponType = nType;

            /*#ifdef _SERVER 
			char nMsg[64]={0};
			   sprintf(nMsg,"--是否白金B: %d,武器类型: %d ",Item[nIdx].GetIsPlatima(),Npc[nNpcIdx].m_WeaponType);
               msgshow(nMsg);
            #endif*/
		}
		else
			Npc[nNpcIdx].m_WeaponType = g_ItemChangeRes.GetWeaponRes(Item[nIdx].GetDetailType(), Item[nIdx].GetParticular(), Item[nIdx].GetLevel());
		
		break;
	case itempart_horse:
		if (Item[nIdx].GetGoldId())
		{
			if (Item[nIdx].GetIsPlatima())
				nType = g_ItemChangeRes.GetPlatinaRes(Item[nIdx].GetGoldId());
		   else
			    nType = g_ItemChangeRes.GetGoldItemRes(Item[nIdx].GetGoldId());
		}

		if (nType)
			Npc[nNpcIdx].m_HorseType = nType;
		else
			Npc[nNpcIdx].m_HorseType = g_ItemChangeRes.GetHorseRes(Item[nIdx].GetParticular(), Item[nIdx].GetLevel());

		if (Item[nIdx].GetItemResIdx()>0)
		{
			Npc[nNpcIdx].m_HorseType = Item[nIdx].GetItemResIdx();
		}

		if (Npc[nNpcIdx].m_HorseType >= 0)
		{
			Npc[nNpcIdx].m_bRideHorse = TRUE;
		}
		else
		{
			Npc[nNpcIdx].m_bRideHorse = FALSE;
		}
		break;
	case itempart_mask:	   // 面具的类型
		if (!m_nMaskLock)
		{	 
			if (Item[nIdx].GetItemResIdx()>0)
			{   
			  Npc[nNpcIdx].m_MaskType = Item[nIdx].GetItemResIdx();
			  break;
			}
			
			Npc[nNpcIdx].m_MaskType = Item[nIdx].GetBaseMagic();
		}
		break;
	case itempart_pifeng:	// 披风
		    if (Item[nIdx].GetItemResIdx()>0)
			{    
			   Npc[nNpcIdx].m_PifengType = Item[nIdx].GetItemResIdx();
			   break;
			}
			
			if (Item[nIdx].GetGoldId())
			{
				//if (Item[nIdx].GetIsPlatima())
				//	Npc[nNpcIdx].m_PifengType = g_ItemChangeRes.GetPlatinaRes(Item[nIdx].GetGoldId());
		        //else

				int nEqLevel=Item[nIdx].GetLevel();
				if  (nEqLevel<=10)
				    Npc[nNpcIdx].m_PifengType = nEqLevel;//g_ItemChangeRes.GetGoldItemRes(Item[nIdx].GetGoldId());//获取外观ID	
				else
					Npc[nNpcIdx].m_PifengType = nEqLevel%10;

			}
			else
			{
				int nEqLevel=Item[nIdx].GetLevel();
				if  (nEqLevel<=10)
					Npc[nNpcIdx].m_PifengType =g_ItemChangeRes.GetPifengRes(Item[nIdx].GetParticular(),nEqLevel);
				else
					Npc[nNpcIdx].m_PifengType = nEqLevel%10;
			  
			}
			   //Npc[nNpcIdx].m_ChiBangType=(BYTE)Item[nIdx].GetChiBangRes();
			break;
	case itempart_shiping:	// 饰品
		 if (Item[nIdx].GetItemResIdx()>0)
		 { 
			Npc[nNpcIdx].m_WeaponType = Item[nIdx].GetItemResIdx();
		 }  
	     break;
	case itempart_yinjian:	//印鉴
		//Npc[nNpcIdx].m_ChiBangType=Item[nIdx].GetChiBangRes();
		if (Item[nIdx].GetItemResIdx()>0)
		{ 
			Npc[nNpcIdx].m_ArmorType = Item[nIdx].GetItemResIdx();
		}  
		break;

	default:
		break;
	}
	// 更新装备自身坐标
	m_EquipItem[nEquipPlace]     = nIdx;
	m_Items[nItemListIdx].nPlace = pos_equip;
	m_Items[nItemListIdx].nX     = nEquipPlace;
	m_Items[nItemListIdx].nY     = 0;

	int nActive = GetEquipEnhance(nEquipPlace); //获取该部位 被激活的

	int nActiveInfo[2];

	   ZeroMemory(nActiveInfo,sizeof(nActiveInfo));

	if (Item[nIdx].IsBlue())
	{//如果是蓝装
	   nActiveInfo[0] = 1;
       nActiveInfo[1] = Item[nIdx].GetIsWhere();
       
	} 
    else if  (Item[nIdx].IsPurple())
	{//如果是紫装
	   nActiveInfo[0] = 2;
       nActiveInfo[1] = Item[nIdx].GetIsWhere();
	} 
    else if  (Item[nIdx].IsGold())
	{//如果是黄装 和 白金
		nActiveInfo[0] = 3;
		nActiveInfo[1] = Item[nIdx].GetLevel();//等级
	} 
    else
	{//白装
		nActiveInfo[0] = 0;
		nActiveInfo[1] = 0;
	}

    if (Item[nIdx].GetIsCanUse()==1)   //是可以使用的
	{
		// GetIfActive();                                    //黄金套装激活列表GetIfActive()或全身激活的;
        Item[nIdx].ApplyMagicAttribToNPC(&Npc[nNpcIdx],nActive,nActiveInfo);  // 应用激活的魔法到NPC 身上
        ZeroMemory(nActiveInfo,sizeof(nActiveInfo));
	    InfectionNextEquip(nEquipPlace, TRUE);                                //穿上 激活暗属性并应用魔法在NPC身上  
	}
    else if (Item[nIdx].GetIsCanUse()==0 && Item[nIdx].GetGenre()==item_equip && Item[nIdx].GetDetailType()==equip_mask) 
	{//面具 不受限制
		Item[nIdx].ApplyMagicAttribToNPC(&Npc[nNpcIdx], nActive,nActiveInfo);
		ZeroMemory(nActiveInfo,sizeof(nActiveInfo));
	    InfectionNextEquip(nEquipPlace, TRUE);
	}

	if (itempart_weapon == nEquipPlace)
	{//换装武器的话，设置相关的基本攻击		
#ifndef _SERVER
		Player[CLIENT_PLAYER_INDEX].UpdateWeaponSkill(); //更新武器技能参数
#endif
		Player[m_PlayerIdx].SetNpcDamageAttrib();        //设置物理伤害相关
	}
//	if(Item[nIdx].GetGoldId()>0)
	if (Item[nIdx].GetIsCanUse()==1)                     //是可以使用的
	{
	   GetIfActive();                                    //黄金套装激活列表GetIfActive()或全身激活的;
	}

	Player[m_PlayerIdx].UpdataCurData(nIsLoign);         //更新自身的属性参数
    
    
#ifdef _SERVER
	for (int i = 1;i < MAX_NPCSKILL;++i)  //通知客户端改变全部技能数据
	{//技能列表
		if (Npc[nNpcIdx].m_SkillList.GetSkillId(i)!=1 &&\
			Npc[nNpcIdx].m_SkillList.GetSkillId(i)!=2 &&\
			Npc[nNpcIdx].m_SkillList.GetSkillId(i)!=53 &&\
			Npc[nNpcIdx].m_SkillList.GetCurrentLevelByIdx(i)>0)
		{
			// 把当前此技能的点数和剩余技能点发给客户端
           //---------------服务端开始给更新被动技能状态数据--------------------------------------
			KSkill * pSkill =(KSkill *)g_SkillManager.GetSkill(Npc[nNpcIdx].m_SkillList.m_Skills[i].SkillId, Npc[nNpcIdx].m_SkillList.m_Skills[i].CurrentSkillLevel);
			if (pSkill) 
			{
				int nReg=0;
				    nReg = pSkill->NewCanCastSkill(nNpcIdx, -1, nNpcIdx);  //武器限制
				//如果该技能属于被动辅助技能时，则设置Npc状态
				if (pSkill->GetSkillStyle() == SKILL_SS_PassivityNpcState && nReg==1)
				{//有和当前武器相符的技能 或 无武器限制的 被动技能
					pSkill->Cast(nNpcIdx, -1, nNpcIdx,-1); //也设置了效果
				} 
				else if (pSkill->GetSkillStyle() == SKILL_SS_PassivityNpcState && nReg<=0)
				{// 与 当前武器不相符的被动技能 将被清除
					pSkill->Cast(nNpcIdx, -1, nNpcIdx,1);
				}
			} 
			pSkill=NULL;
			///-------------------------------------------------------------------------------	
		}
	}

#endif
    //ReSkillEnhance(m_PlayerIdx,nNpcIdx);
	return TRUE;
}

void KItemList::ReSkillEnhance(int m_PlayerIdx,int nNpcIdx)
{
	int nActiveSkillID=0,nListIndex=0;
	
#ifdef _SERVER
	nActiveSkillID=Player[m_PlayerIdx].GetLeftSkill();
	nListIndex    =Player[m_PlayerIdx].GetLeftSkillListidx();
#else
	nActiveSkillID=Player[CLIENT_PLAYER_INDEX].GetLeftSkill();
	nListIndex    =Player[CLIENT_PLAYER_INDEX].GetLeftSkillListidx();
#endif
	
	if (Npc[nNpcIdx].m_Currentskillenhance!= 0)
	{
		//int nListIndex = Npc[nNpcIdx].m_SkillList.FindSame(nActiveSkillID);
		if (nListIndex)                 //查找激活的当前攻击技能ID
		{
			if (Npc[nNpcIdx].m_SkillList.GetCurrentLevelByIdx(nListIndex) > 0)  //等级大于0的才 增加
			{
				Npc[nNpcIdx].m_SkillList.AddEnChance(nListIndex,Npc[nNpcIdx].m_Currentskillenhance);
			}			
		}
	}
}

/*!*****************************************************************************
// Function		: KItemList::InfectionNextEquip
// Purpose		: 装备影响其他装备-没有激活的属性 开关
// Return		: void 
// Argumant		: int nEquipPlace
// Argumant		: BOOL bEquipUp 是装上（TRUE）还是卸下（FALSE）
// Comments		:
// Author		: Spe
*****************************************************************************/
void KItemList::InfectionNextEquip(int nEquipPlace,BOOL bEquip)
{
	if (m_PlayerIdx <= 0)
		return;

	int nNpcIdx = Player[m_PlayerIdx].m_nIndex;
	int nActive = 0;
	for (int i = 0; i < MAX_ITEM_ACTIVE; ++i)
	{
		int nInfectionEquip = ms_ActiveEquip[nEquipPlace][i]; //激活的装备列表
		if (!m_EquipItem[nInfectionEquip])                    //如果没有装备这个装备
			continue;
		if (g_IsAccrue(Item[m_EquipItem[nEquipPlace]].GetSeries(),Item[m_EquipItem[nInfectionEquip]].GetSeries()))  //装备五行相生
		{//相生
			nActive = GetEquipEnhance(nInfectionEquip);

			_ASSERT(nActive > 0);
			if (bEquip)//穿上激活暗属性
				Item[m_EquipItem[nInfectionEquip]].ApplyHiddenMagicAttribToNPC(&Npc[nNpcIdx], nActive);
			else//卸下（移除暗属性）
				Item[m_EquipItem[nInfectionEquip]].RemoveHiddenMagicAttribFromNPC(&Npc[nNpcIdx], nActive);
		}
		/*else
		{//如果不是相互激活的
			if (m_bActiveSet) //是黄金装备并且是套装  全身激活的
			{
				nActive = 3; 

			  if (!bEquip)
			 {//是卸载装备的
				Item[m_EquipItem[nInfectionEquip]].RemoveHiddenMagicAttribFromNPC(&Npc[nNpcIdx], nActive);
			 } 
			}

		}*/
	}
}
//全身激活
/*
void KItemList::InAllNextEquip(int nEquipPlace,BOOL bEquip)
{
	if (m_PlayerIdx <= 0)
		return;
	
	int nNpcIdx = Player[m_PlayerIdx].m_nIndex;
	
	for (int i = 0; i < MAX_ITEM_ACTIVE; ++i)
	{
		//int nInfectionEquip = ms_ActiveEquip[nEquipPlace][i]; //激活的装备列表
		if (!m_EquipItem[nEquipPlace])                    //如果没有装备这个装备
			continue;
		//if (g_IsAccrue(Item[m_EquipItem[nEquipPlace]].GetSeries(),Item[m_EquipItem[nInfectionEquip]].GetSeries()))  //装备五行相生
		{
			int nActive = GetEquipEnhance(nInfectionEquip); //获取被激活的暗属性的数量
			_ASSERT(nActive >0);
			if (bEquip)                                      //穿上激活暗属性
				Item[m_EquipItem[nInfectionEquip]].ApplyHiddenMagicAttribToNPC(&Npc[nNpcIdx], nActive);
			else//卸下（移除暗属性）
				Item[m_EquipItem[nInfectionEquip]].RemoveHiddenMagicAttribFromNPC(&Npc[nNpcIdx], nActive);
		}
	}
}
*/
BOOL KItemList::RemoveItemBox(int nIdx , int nPos)
{
    int i = 0;
	if (m_PlayerIdx <= 0)
		return FALSE;

	int nNpcIdx = Player[m_PlayerIdx].m_nIndex;
	if (nIdx <= 0)
		return FALSE;

	if (nPos <= 0)
	{//如果容器位置为空
		for (i = 0; i < MAX_ITEM_BOX; ++i)
		{
			if (m_ItemBox[i] == nIdx)
			{//如果装备着的 等于手上的
				break;
			}
		}
		// 没有发现身上有这个装备
		if (i == MAX_ITEM_BOX)
			return FALSE;

	}
	else
	{
		if (m_ItemBox[nPos] != nIdx)	// 东西不对
			return FALSE;

		i = nPos;
	}

	m_ItemBox[i] = 0; 
	return TRUE;
}
/*!*****************************************************************************
// Function		: KItemList::UnEquip
// Purpose		: 移除同伴装备
// Return		: BOOL 
// Argumant		: int nIdx 游戏世界中的道具数组索引
// Comments		: 
// Author		: Spe
*****************************************************************************/

BOOL KItemList::UnTonEquip(int nIdx, int nPos)
{
   	int i = 0,nTonNpcIdx=0;

	if (m_PlayerIdx <= 0)
		return FALSE;

	int nNpcIdx = Player[m_PlayerIdx].m_nIndex;	 //主人的索引

	if (Npc[nNpcIdx].m_TongBanNum)
	{//已经召唤了宠物的	
		nTonNpcIdx =NpcSet.SearchID(Npc[nNpcIdx].m_TongBanNum);
	}

	if (nIdx <= 0 || nNpcIdx<0 || nTonNpcIdx<=0)
		return FALSE;

	if (nPos <= 0)
	{//如果容器位置为空
		for (i = 53; i < 78; ++i)
		{
			if (m_ItemBox[i] == nIdx)
			{//如果装备着的 等于手上的
				break;
			}
		}
		// 没有发现身上有这个装备
		if (i == 78)
			return FALSE;

	}
	else
	{
		if (m_ItemBox[nPos] != nIdx)	// 东西不对
			return FALSE;

		i = nPos;
	}
	// 移除该装备对NPC的属性调整
	int nActive =3;// GetEquipEnhance(i);

	//if (m_bActiveSet) //是黄金装备并且是套装
	//	nActive = 3;  //激活的默认明属性有三个

	Item[nIdx].RemoveMagicAttribFromNPC(&Npc[nTonNpcIdx],nActive);
	// 移除该装备可能对它激活的装备的影响。
	//InfectionNextEquip(i,FALSE);
	// 这句话一定要放在上一句后，保证计算该装备激活的装备激活属性个数计算的正确性
	m_ItemBox[i] = 0;   //卸载了装备 就设置这个位置为空 ！
	// 换装		
	Npc[nTonNpcIdx].UpdataNpcCurData(m_PlayerIdx,i);

	if (!GetIfActive())                            //如果不是黄金激活
		Player[m_PlayerIdx].UpdataCurData();       //更新数据


#ifdef _SERVER
			for (i = 1;i < 6;++i)  
			{//通知客户端改变全部技能数据
				if (Npc[nTonNpcIdx].m_SkillList.GetSkillId(i)!=1 &&
					Npc[nTonNpcIdx].m_SkillList.GetSkillId(i)!=2 && 
					Npc[nTonNpcIdx].m_SkillList.GetSkillId(i)!=53 &&
					Npc[nTonNpcIdx].m_SkillList.GetCurrentLevelByIdx(i) > 0)
				{ 
					////---------------服务端开始给更新被动技能状态数据--------------------------------------
					KSkill * pSkill =(KSkill *)g_SkillManager.GetSkill(Npc[nTonNpcIdx].m_SkillList.m_Skills[i].SkillId, Npc[nTonNpcIdx].m_SkillList.m_Skills[i].SkillLevel);
					if (pSkill) 
					{
						//int nReg=0;
                        //nReg = pSkill->NewCanCastSkill(nNpcIdx,-1,nNpcIdx);  //武器限制
						//如果该技能属于被动辅助技能时，则设置Npc状态
						if (pSkill->GetSkillStyle() == SKILL_SS_PassivityNpcState)
						{//有武器在手 无限制的 或 与武器相符的被动技能就设置，无限状态
							pSkill->Cast(nTonNpcIdx, -1, nTonNpcIdx,-1);
						}
						//else if  (pSkill->GetSkillStyle() == SKILL_SS_PassivityNpcState)
						//{//与当前武器不相同的被动技能 被清除！
                        //    pSkill->Cast(nNpcIdx, -1, nNpcIdx,1);
						//}
					} 
					pSkill=NULL;
		            ////-------------------------------------------------------------------------------	
				}
			}
#endif

     //ReSkillEnhance(m_PlayerIdx,nNpcIdx);  //重置对攻击技能的加成！
	return TRUE;

}


/*!*****************************************************************************
// Function		: KItemList::UnEquip
// Purpose		: 移除装备
// Return		: BOOL 
// Argumant		: int nIdx 游戏世界中的道具数组索引
// Comments		: 
// Author		: Spe
*****************************************************************************/
BOOL KItemList::UnEquip(int nIdx, int nPos)
{
	int i = 0;
	if (m_PlayerIdx <= 0)
		return FALSE;

	int nNpcIdx = Player[m_PlayerIdx].m_nIndex;
	if (nIdx <= 0)
		return FALSE;

	if (nPos <= 0)
	{//如果容器位置为空
		for (i = 0; i < itempart_num; ++i)
		{
			if (m_EquipItem[i] == nIdx)
			{//如果装备着的 等于手上的
				break;
			}
		}
		// 没有发现身上有这个装备
		if (i == itempart_num)
			return FALSE;

	}
	else
	{
		if (m_EquipItem[nPos] != nIdx)	// 东西不对
			return FALSE;

		i = nPos;
	}


	// 移除该装备对NPC的属性调整
	int nActive = GetEquipEnhance(i);

	if (m_bActiveSet) //是黄金装备并且是套装  全身激活的
		nActive = 3;  //激活的默认明属性有三个

	int nActiveInfo[2];

	if (Item[nIdx].GetGoldId()>0)
	{
		nActiveInfo[0]=3;
		nActiveInfo[1]=Item[nIdx].GetLevel();
	}
	else
	{
		nActiveInfo[0]=0;
		nActiveInfo[1]=0;

	}

	Item[nIdx].RemoveMagicAttribFromNPC(&Npc[nNpcIdx],nActive,nActiveInfo);
	//移除该装备可能对它激活的装备的影响。
	InfectionNextEquip(i,FALSE);
	// 这句话一定要放在上一句后，保证计算该装备激活的装备激活属性个数计算的正确性
	m_EquipItem[i] = 0;   //卸载了装备 就设置这个位置为空 ！
	// 换装
	int nNeWidx=0,nType=0;
	switch(i)
	{//外观的改变
	case itempart_head:
		Npc[nNpcIdx].m_HelmType = g_ItemChangeRes.GetHelmRes(0, 0);
		break;
	case itempart_body:
		Npc[nNpcIdx].m_ChiBangType=0;
		Npc[nNpcIdx].m_ArmorType = g_ItemChangeRes.GetArmorRes(0, 0);
		break;
	case itempart_weapon:
		Npc[nNpcIdx].m_WeaponType = g_ItemChangeRes.GetWeaponRes(0, 0, 0);
		break;
	case itempart_horse:
		Npc[nNpcIdx].m_HorseType = -1;
		Npc[nNpcIdx].m_bRideHorse = FALSE;
		break;
	case itempart_mask:
		if (!m_nMaskLock)
		{
		   Npc[nNpcIdx].m_MaskType = 0;
		}
		break;
	case itempart_pifeng:
		  Npc[nNpcIdx].m_PifengType=0;//g_ItemChangeRes.GetPifengRes(0, 0);
		break;
    case itempart_shiping:	// 饰品	 武器
		{
			if (nNeWidx=m_EquipItem[itempart_weapon])
			{
				if (Item[nNeWidx].GetItemResIdx()>0)
				{  
			       Npc[nNpcIdx].m_WeaponType = Item[nNeWidx].GetItemResIdx();
			       break;
				}  
		
		       if (Item[nNeWidx].GetGoldId())
			   {   
			     if (Item[nNeWidx].GetIsPlatima())
				    nType  = g_ItemChangeRes.GetPlatinaRes(Item[nNeWidx].GetGoldId());
			     else
			        nType  = g_ItemChangeRes.GetGoldItemRes(Item[nNeWidx].GetGoldId());
			   }

		      if (nType)
			  { 
			     Npc[nNpcIdx].m_WeaponType = nType;
			  }  
		      else
			     Npc[nNpcIdx].m_WeaponType = g_ItemChangeRes.GetWeaponRes(Item[nNeWidx].GetDetailType(), Item[nNeWidx].GetParticular(), Item[nNeWidx].GetLevel());
 
			}
			else
				Npc[nNpcIdx].m_WeaponType = g_ItemChangeRes.GetWeaponRes(0, 0, 0); 
	
		} 

		  break;
	case itempart_yinjian:	// 印鉴	 衣服
		{//改变的 衣服
		   
		   if (nNeWidx=m_EquipItem[itempart_body])
		   {
			   
			   if (Item[nNeWidx].GetItemResIdx()>0)
			   {
				   Npc[nNpcIdx].m_ArmorType = Item[nNeWidx].GetItemResIdx();
				   break;
			   }
			   
			   if (Item[nNeWidx].GetGoldId())
			   {	
				   if (Item[nNeWidx].GetIsPlatima())
					   nType = 	g_ItemChangeRes.GetPlatinaRes(Item[nNeWidx].GetGoldId());
				   else
					   nType =  g_ItemChangeRes.GetGoldItemRes(Item[nNeWidx].GetGoldId());
			   }
			   
			   if (nType)
				   Npc[nNpcIdx].m_ArmorType = nType;
			   else
				   Npc[nNpcIdx].m_ArmorType = g_ItemChangeRes.GetArmorRes(Item[nNeWidx].GetParticular(),Item[nNeWidx].GetLevel());
		   }
		   else
			   Npc[nNpcIdx].m_ArmorType = g_ItemChangeRes.GetArmorRes(0, 0);

		   //Npc[nNpcIdx].m_ChiBangType=0;
		}
		break;	  
	default:
		break;
	}

	if (itempart_weapon == i)
	{
#ifndef _SERVER
		Player[CLIENT_PLAYER_INDEX].UpdateWeaponSkill();
#endif
		Player[m_PlayerIdx].SetNpcDamageAttrib();  //伤害属性？
	}	
   	if (!GetIfActive())   //如果不是黄金激活 或全身激活的
	{   
		Player[m_PlayerIdx].UpdataCurData(FALSE,TRUE);       //更新数据  
	}

#ifdef _SERVER
			for (i = 1;i < MAX_NPCSKILL;++i)  
			{//通知客户端改变全部技能数据
				if (Npc[nNpcIdx].m_SkillList.GetSkillId(i)!=1 &&
					Npc[nNpcIdx].m_SkillList.GetSkillId(i)!=2 && 
					Npc[nNpcIdx].m_SkillList.GetSkillId(i)!=53 &&
					Npc[nNpcIdx].m_SkillList.GetCurrentLevelByIdx(i) > 0)
				{ 
					//把当前此技能的点数和剩余技能点发给客户端b
					/*PLAYER_SKILL_LEVEL_SYNC	sSkill;
					sSkill.ProtocolType  = s2c_playerskilllevel;
					sSkill.m_nSkillID    = Npc[nNpcIdx].m_SkillList.GetSkillId(i);
					sSkill.m_nSkillLevel = Npc[nNpcIdx].m_SkillList.GetCurrentLevel(Npc[nNpcIdx].m_SkillList.GetSkillId(i));
					sSkill.m_nLeavePoint = Player[m_PlayerIdx].m_nSkillPoint;
					sSkill.m_nAddPoint   = Npc[nNpcIdx].m_SkillList.GetAddPoint(i);
					g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sSkill, sizeof(PLAYER_SKILL_LEVEL_SYNC));
					*////---------------服务端开始给更新被动技能状态数据--------------------------------------
					KSkill * pSkill =(KSkill *)g_SkillManager.GetSkill(Npc[nNpcIdx].m_SkillList.m_Skills[i].SkillId, Npc[nNpcIdx].m_SkillList.m_Skills[i].CurrentSkillLevel);
					if (pSkill) 
					{
						int nReg=0;
                            nReg = pSkill->NewCanCastSkill(nNpcIdx,-1,nNpcIdx);  //武器限制
						//如果该技能属于被动辅助技能时，则设置Npc状态
					    if (pSkill->GetSkillStyle() == SKILL_SS_PassivityNpcState && nReg==1)
						{//有武器在手 无限制的 或 与武器相符的被动技能就设置，无限状态
							//printf("--开始使用技能(%d)等级(%d)--\n",Npc[nNpcIdx].m_SkillList.m_Skills[i].SkillId,Npc[nNpcIdx].m_SkillList.m_Skills[i].CurrentSkillLevel);
							pSkill->Cast(nNpcIdx, -1, nNpcIdx,-1,SKILL_SLT_Npc,0,1);
						}
						else if  (pSkill->GetSkillStyle() == SKILL_SS_PassivityNpcState && nReg<=0)
						{//与当前武器不相同的被动技能 被清除！
                            pSkill->Cast(nNpcIdx, -1, nNpcIdx,1);
						}
					} 
					pSkill=NULL;
		            ////-------------------------------------------------------------------------------	
				}
			}
#endif

     //ReSkillEnhance(m_PlayerIdx,nNpcIdx);  //重置对攻击技能的加成！
	return TRUE;
}


/*!*****************************************************************************
// Function		: KItemList::UnTimeEquip  //移除时间到的属性
// Purpose		: 移除装备
// Return		: BOOL 
// Argumant		: int nIdx 游戏世界中的道具数组索引
// Comments		: 
// Author		: Spe
*****************************************************************************/

BOOL  KItemList::tempUnEquip(int nIdx,int nPos)
{
	int i = 0;
	if (m_PlayerIdx <= 0)
		return FALSE;

	int nNpcIdx = Player[m_PlayerIdx].m_nIndex;
	if (nIdx <= 0)
		return FALSE;
    i = nPos;
	if  (i<0) return false;
	// 移除该装备对NPC的属性调整
	int nActive = GetEquipEnhance(i);
	if (m_bActiveSet) //是黄金装备并且是套装  或者 是全身激活的
		nActive = 3;  //激活的默认明属性有三个
	int nActiveInfo[2];
	if (Item[nIdx].GetGoldId()>0)
	{
		nActiveInfo[0]=3;
		nActiveInfo[1]=Item[nIdx].GetLevel();
	}
	else
	{
		nActiveInfo[0]=0;
		nActiveInfo[1]=0;
		
	}

	Item[nIdx].RemoveMagicAttribFromNPC(&Npc[nNpcIdx],nActive,nActiveInfo);
	// 移除该装备可能对它激活的装备的影响。
	InfectionNextEquip(i, FALSE);

	if (itempart_weapon == i)
	{
#ifndef _SERVER
		Player[CLIENT_PLAYER_INDEX].UpdateWeaponSkill();
#endif
		Player[m_PlayerIdx].SetNpcDamageAttrib();  //伤害属性？
	}	

   	if (!GetIfActive())  //如果不是黄金激活
		Player[m_PlayerIdx].UpdataCurData();       //更新数据  

#ifdef _SERVER
			for (i = 1;i < MAX_NPCSKILL;++i)  
			{//通知客户端改变全部技能数据
				if (Npc[nNpcIdx].m_SkillList.GetSkillId(i)!=1 &&
					Npc[nNpcIdx].m_SkillList.GetSkillId(i)!=2 && 
					Npc[nNpcIdx].m_SkillList.GetSkillId(i)!=53 &&
					Npc[nNpcIdx].m_SkillList.GetCurrentLevelByIdx(i) > 0)
				{ 
					KSkill * pSkill =(KSkill *)g_SkillManager.GetSkill(Npc[nNpcIdx].m_SkillList.m_Skills[i].SkillId, Npc[nNpcIdx].m_SkillList.m_Skills[i].SkillLevel);
					if (pSkill) 
					{
						int nReg=0;
                        nReg = pSkill->NewCanCastSkill(nNpcIdx,-1,nNpcIdx);  //武器限制
						//如果该技能属于被动辅助技能时，则设置Npc状态
						if (pSkill->GetSkillStyle() == SKILL_SS_PassivityNpcState && nReg==1)
						{//有武器在手 无限制的 或 与武器相符的被动技能就设置，无限状态
							pSkill->Cast(nNpcIdx, -1, nNpcIdx,-1,SKILL_SLT_Npc,0,1);
						}
						else if  (pSkill->GetSkillStyle() == SKILL_SS_PassivityNpcState && nReg<=0)
						{//空手 状态 有武器限制的被动技能 被清除！
                            pSkill->Cast(nNpcIdx, -1, nNpcIdx,1);
						}
					} 
					pSkill=NULL;
		            ////-------------------------------------------------------------------------------	
				}
			}
#endif
	return TRUE;
}


BOOL KItemList::TimeUnEquip(int nIdx, int nPos)
{
	int i = 0;
	if (m_PlayerIdx <= 0)
		return FALSE;

	int nNpcIdx = Player[m_PlayerIdx].m_nIndex;
	if (nIdx <= 0)
		return FALSE;

	if (nPos <= 0)
	{//如果容器位置为空
		for (i = 0; i < itempart_num; ++i)
		{
			if (m_EquipItem[i] == nIdx)
			{//如果装备着的 等于手上的
				break;
			}
		}
		// 没有发现身上有这个装备
		if (i == itempart_num)
			return FALSE;

	}
	else
	{
		if (m_EquipItem[nPos] != nIdx)	// 东西不对
			return FALSE;

		i = nPos;
	}
	// 移除该装备对NPC的属性调整
	int nActive = GetEquipEnhance(i);
	if (m_bActiveSet) //是黄金装备并且是套装  或者 是全身激活的
		nActive = 3;  //激活的默认明属性有三个
	int nActiveInfo[2];
	if (Item[nIdx].GetGoldId()>0)
	{
		nActiveInfo[0]=3;
		nActiveInfo[1]=Item[nIdx].GetLevel();
	}
	else
	{
		nActiveInfo[0]=0;
		nActiveInfo[1]=0;
		
	}

	Item[nIdx].RemoveMagicAttribFromNPC(&Npc[nNpcIdx],nActive,nActiveInfo);
	// 移除该装备可能对它激活的装备的影响。
	InfectionNextEquip(i, FALSE);
	
	//这句话一定要放在上一句后，保证计算该装备激活的装备激活属性个数计算的正确性
	//m_EquipItem[i] = 0;   //这个位置设置没有装备

	if (itempart_weapon == i)
	{
#ifndef _SERVER
		Player[CLIENT_PLAYER_INDEX].UpdateWeaponSkill();
#endif
		Player[m_PlayerIdx].SetNpcDamageAttrib();  //伤害属性？
	}	

   	if (!GetIfActive())  //如果不是黄金激活
		Player[m_PlayerIdx].UpdataCurData();       //更新数据  

#ifdef _SERVER
			for (i = 1;i < MAX_NPCSKILL;++i)  
			{//通知客户端改变全部技能数据
				if (Npc[nNpcIdx].m_SkillList.GetSkillId(i)!=1 &&
					Npc[nNpcIdx].m_SkillList.GetSkillId(i)!=2 && 
					Npc[nNpcIdx].m_SkillList.GetSkillId(i)!=53 &&
					Npc[nNpcIdx].m_SkillList.GetCurrentLevelByIdx(i) > 0)
				{ 
					//把当前此技能的点数和剩余技能点发给客户端
					/*PLAYER_SKILL_LEVEL_SYNC	sSkill;
					sSkill.ProtocolType  = s2c_playerskilllevel;
					sSkill.m_nSkillID    = Npc[nNpcIdx].m_SkillList.GetSkillId(i);
					sSkill.m_nSkillLevel = Npc[nNpcIdx].m_SkillList.GetCurrentLevel(Npc[nNpcIdx].m_SkillList.GetSkillId(i));
					sSkill.m_nLeavePoint = Player[m_PlayerIdx].m_nSkillPoint;
					sSkill.m_nAddPoint   = Npc[nNpcIdx].m_SkillList.GetAddPoint(i);
					g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sSkill, sizeof(PLAYER_SKILL_LEVEL_SYNC));
					*////---------------服务端开始给更新被动技能状态数据--------------------------------------
					KSkill * pSkill =(KSkill *)g_SkillManager.GetSkill(Npc[nNpcIdx].m_SkillList.m_Skills[i].SkillId, Npc[nNpcIdx].m_SkillList.m_Skills[i].SkillLevel);
					if (pSkill) 
					{
						int nReg=0;
                        nReg = pSkill->NewCanCastSkill(nNpcIdx,-1,nNpcIdx);  //武器限制
						//如果该技能属于被动辅助技能时，则设置Npc状态
						if (pSkill->GetSkillStyle() == SKILL_SS_PassivityNpcState && nReg==1)
						{//有武器在手 无限制的 或 与武器相符的被动技能就设置，无限状态
							pSkill->Cast(nNpcIdx, -1, nNpcIdx,-1,SKILL_SLT_Npc,0,1);
						}
						else if  (pSkill->GetSkillStyle() == SKILL_SS_PassivityNpcState && nReg<=0)
						{//空手 状态 有武器限制的被动技能 被清除！
                            pSkill->Cast(nNpcIdx, -1, nNpcIdx,1);
						}
					} 
					pSkill=NULL;
		            ////-------------------------------------------------------------------------------	
				}
			}
	
#endif
	return TRUE;
}


//获取装备的部位类型
int KItemList::GetEquipPlace(int nType,int nItemIdx)
{
	int nRet = -1;
	switch(nType)
	{
	case equip_meleeweapon:
	case equip_rangeweapon:
		nRet = itempart_weapon;
		break;
	case equip_armor:
		nRet = itempart_body;
		break;
	case equip_helm:
		nRet = itempart_head;
		break;
	case equip_boots:
		nRet = itempart_foot;
		break;
	case equip_ring:
		nRet = itempart_ring1;
		break;
	case equip_amulet:
		nRet = itempart_amulet;
		break;
	case equip_belt:
		nRet = itempart_belt;
		break;
	case equip_cuff:
		nRet = itempart_cuff;
		break;
	case equip_pendant:
		nRet = itempart_pendant;
		break;
	case equip_horse:
		nRet = itempart_horse;
		break;
	case equip_mask:	// mat na
		nRet = itempart_mask;
		break;
	case equip_pifeng:	//披风
		nRet = itempart_pifeng;
		break;
	case equip_yinjian:	// mat na
		nRet = itempart_yinjian;
		break;
	case equip_shiping:	// mat na
		nRet = itempart_shiping;
		break;
	default:
		break;
	}
	return nRet;
}

/*!*****************************************************************************
// Function		: KItemList::Fit
// Purpose		: 
// Return		: BOOL 
// Argumant		: int nIdx
// Argumant		: int nPlace
// Comments		:
// Author		: Spe
*****************************************************************************/
BOOL KItemList::Fit(int nIdx, int nPlace)
{
	BOOL	bRet = FALSE;
	_ASSERT(Item[nIdx].GetGenre() == item_equip);
	switch(Item[nIdx].GetDetailType())
	{
	case equip_meleeweapon:
	case equip_rangeweapon:
		if (nPlace == itempart_weapon)
			bRet = TRUE;
		break;
	case equip_armor:
		if (nPlace == itempart_body)
			bRet = TRUE;
		break;
	case equip_belt:
		if (nPlace == itempart_belt)
			bRet = TRUE;
		break;
	case equip_boots:
		if (nPlace == itempart_foot)
			bRet = TRUE;
		break;
	case equip_cuff:
		if (nPlace == itempart_cuff)
			bRet = TRUE;
		break;
	case equip_ring:
		if (nPlace == itempart_ring1 || nPlace == itempart_ring2)
			bRet = TRUE;
		break;
	case equip_amulet:
		if (nPlace == itempart_amulet)
			bRet = TRUE;
		break;
	case equip_pendant:
		if (nPlace == itempart_pendant)
			bRet = TRUE;
		break;
	case equip_helm:
		if (nPlace == itempart_head)
			bRet = TRUE;
		break;
	case equip_horse:
		if (nPlace == itempart_horse)
			bRet = TRUE;
		break;
	case equip_mask:	// mat na
		if (nPlace == itempart_mask)
			bRet = TRUE;
		break;
	case equip_pifeng:	// mat na
		if (nPlace == itempart_pifeng)
			bRet = TRUE;
		break;
	case equip_yinjian:	// mat na
		if (nPlace == itempart_yinjian)
			bRet = TRUE;
		break;
	case equip_shiping:	// mat na
		if (nPlace == itempart_shiping)
			bRet = TRUE;
		break;
	}
	return bRet;
}

BOOL KItemList::Fit(KItem* pItem, int nPlace)
{
	BOOL	bRet = FALSE;
	_ASSERT(pItem->GetGenre() == item_equip);
	switch(pItem->GetDetailType())
	{
	case equip_meleeweapon:
	case equip_rangeweapon:
		if (nPlace == itempart_weapon)
			bRet = TRUE;
		break;
	case equip_armor:
		if (nPlace == itempart_body)
			bRet = TRUE;
		break;
	case equip_belt:
		if (nPlace == itempart_belt)
			bRet = TRUE;
		break;
	case equip_boots:
		if (nPlace == itempart_foot)
			bRet = TRUE;
		break;
	case equip_cuff:
		if (nPlace == itempart_cuff)
			bRet = TRUE;
		break;
	case equip_ring:
		if (nPlace == itempart_ring1 || nPlace == itempart_ring2)
			bRet = TRUE;
		break;
	case equip_amulet:
		if (nPlace == itempart_amulet)
			bRet = TRUE;
		break;
	case equip_pendant:
		if (nPlace == itempart_pendant)
			bRet = TRUE;
		break;
	case equip_helm:
		if (nPlace == itempart_head)
			bRet = TRUE;
		break;
	case equip_horse:
		if (nPlace == itempart_horse)
			bRet = TRUE;
		break;
	case equip_mask:	// mat na
		if (nPlace == itempart_mask)
			bRet = TRUE;
		break;
	case equip_pifeng:	//披风
		if (nPlace == itempart_pifeng)
			bRet = TRUE;
		break;
	case equip_yinjian:	// mat na
		if (nPlace == itempart_yinjian)
			bRet = TRUE;
		break;
	case equip_shiping:	// mat na
		if (nPlace == itempart_shiping)
			bRet = TRUE;
		break;
	}
	return bRet;
}

int KItemList::_getEquipPlaceByItemIdx(int nidx)
{
	if (m_PlayerIdx <= 0) return false;
	//int nSeries = Item[m_EquipItem[nPlace]].GetSeries();  //装备五行
     for (int i=0;i<itempart_num;i++)
	 {
		 if (m_EquipItem[i]>0 && m_EquipItem[i]==nidx)
			 return i;
	 }

	 return -1;
}



int KItemList::GetEquipEnhance(int nPlace)
{
	if (m_PlayerIdx <= 0)
		return FALSE;

	int nNpcIdx = Player[m_PlayerIdx].m_nIndex;

	if (nPlace < 0 || nPlace >= itempart_num)
	{
		//_ASSERT(0);
		return 0;
	}
	int nRet = 0;

	int nSeries = Item[m_EquipItem[nPlace]].GetSeries();  //装备五行
	if (g_IsAccrue(Npc[nNpcIdx].m_Series, nSeries))  //人物五行 和装备五行相生 就激活
		nRet ++;

	for (int i = 0; i < MAX_ITEM_ACTIVE; ++i)
	{
		// 看看激活这个装备的装备中是否相生这个装备
		int nActivedEquip = m_EquipItem[ms_ActivedEquip[nPlace][i]]; //被激活索引
		if (nActivedEquip)
		{
			if (g_IsAccrue(Item[nActivedEquip].GetSeries(), nSeries))  //物品五行相生 就激活
				nRet++;
		}
	}

	if (nRet>3)
		nRet=3;

	return nRet;
}
//服务器端 吃药品 执行脚本  --右键执行脚本
BOOL KItemList::EatMecidineS(int nIdx,int nx,int nY)
{
	if (m_PlayerIdx <= 0)
		return FALSE;

	int nNpcIdx = Player[m_PlayerIdx].m_nIndex;

	int nGenre = Item[nIdx].GetGenre();

	_ASSERT(nGenre == item_medicine || nGenre == item_townportal || nGenre == item_task || nGenre == item_mine);
	
	if (nGenre == item_medicine)  //药品
	{
		//Npc[nNpcIdx].GetMpsPos(&xx, &yy,&Mmap);
		if (Item[nIdx].GetUseKind() == 0 && Item[nIdx].GetUseMapIdx()>0 && Item[nIdx].GetUseMapIdx()!=SubWorld[Npc[nNpcIdx].m_SubWorldIndex].m_SubWorldID)
		{
           #ifdef _SERVER
			   Player[m_PlayerIdx].m_ItemList.msgshow("药品:<color=yellow>该专用药品不能使用<color>!");
           #else
			   Player[m_PlayerIdx].m_ItemList.ClientShowMsg("药品:<color=yellow>该专用药品不能使用<color>!");
           #endif

	       return FALSE;
		}
		int Val[2];
		ZeroMemory(Val,sizeof(Val));
		Item[nIdx].ApplyMagicAttribToNPC(&Npc[nNpcIdx],3,Val);  //增加魔法属性到 NPC身上
	}

	if (nGenre == item_townportal)  //土地符
	{
		if (!Npc[Player[m_PlayerIdx].m_nIndex].m_FightMode)
		{// || Npc[Player[m_PlayerIdx].m_nIndex].m_AutoplayId>0
#ifndef _SERVER	// 客户端先看看是不是非战斗区，如果是的话就别发给Server了。
			// 系统消息
			KSystemMessage Msg;			
			Msg.byConfirmType = SMCT_CLICK;
			Msg.byParamSize = 0;
			Msg.byPriority = 1;
			Msg.eType = SMT_PLAYER;
			snprintf(Msg.szMessage, sizeof(Msg.szMessage), "%s", "提示:挂机/非战斗模式,不能回城!");
			Msg.nMsgLen = TEncodeText(Msg.szMessage, strlen(Msg.szMessage));
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&Msg, NULL);
#endif
			return FALSE;
		}
	//	服务器端
#ifdef _SERVER
		if (Npc[Player[m_PlayerIdx].m_nIndex].m_CurrentLife >0)
		   Player[m_PlayerIdx].UseTownPortalScript();//使用回城  UseTownPortal
		else
		{
			Player[m_PlayerIdx].m_ItemList.msgshow("提示:<color=yellow>生命为空,还回毛城哦<color>!");
			return FALSE;
		}
#endif
	}

	//fix item xep chong su dung truc tiep tren phim tat
	if (nGenre == item_task || nGenre == item_mine)
	{/*
		//客户端
		int mItemInfo[3],mprg;
		ZeroMemory(mItemInfo,sizeof(mItemInfo));
            mItemInfo[0]=Item[nIdx].GetGenre();
            mItemInfo[1]=Item[nIdx].GetDetailType();
            mItemInfo[2]=Item[nIdx].GetParticular();
			mprg = NpcSet.CheckForBit(mItemInfo, "IsUseStack", 3);
			//mprg = Item[nIdx].GetIsUse();
       if (Item[nIdx].GetStackNum() > 1 && mprg==0)  //是否可以叠加使用
	   {
#ifndef _SERVER	// 客户端先看看是不是非战斗区，如果是的话就别发给Server了。
		   // 系统消息
		   //Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg();
		   KSystemMessage Msg;			
		   Msg.byConfirmType = SMCT_CLICK;
		   Msg.byParamSize = 0;
		   Msg.byPriority = 1;
		   Msg.eType = SMT_PLAYER;
		   strcpy(Msg.szMessage, "提示:物品叠加,不能使用!");
		   Msg.nMsgLen = TEncodeText(Msg.szMessage, strlen(Msg.szMessage));
		   CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&Msg, NULL);
#endif
		   return FALSE;
	   }
	   */
		int nReg=0;
#ifdef _SERVER
		if (Item[nIdx].GetMagicID()>0)  //如果技能ID大于0的就使用技能 不执行脚本
		{//使用状态
		   nReg = 0;
		   Player[m_PlayerIdx].UseToolSkills(Item[nIdx].GetMagicID(),1,Player[m_PlayerIdx].m_nIndex,Player[m_PlayerIdx].m_nIndex,NULL);
		}   
		else			
	       nReg = Player[m_PlayerIdx].ExeItemScriptA(\
			Item[nIdx].GetScript(),\
			"main",\
			Item[nIdx].GetName(),\
			Item[nIdx].GetWonName(),\
			nIdx,\
			Item[nIdx].GetMagicID(),\
			Item[nIdx].GetGenre(),\
			Item[nIdx].GetDetailType(),\
			Item[nIdx].GetParticular(),\
            Item[nIdx].GetStackNum(),\
                                  nx,\
                                  nY);

		    if (Item[nIdx].GetStackNum() > 1)
			{//这个是叠加的
			   if (nReg==0)
			   {//如果执行了脚本
		 	      Item[nIdx].SetStackNum(Item[nIdx].GetStackNum() - 1);  //数量减一
				  SynPlayerItem(nIdx);
			   }
			} 
	     	else
			{ //没有叠加的

			   /* if (Item[nIdx].GetBigSet() && nReg==0)   //使用是否消失
			    { 
				   Remove(nIdx,nx,nY,1);
				   ItemSet.Remove(nIdx);			
				}
				else if (!Item[nIdx].GetBigSet() && nReg==0) 
				{
				   Remove(nIdx,nx,nY,1);
				   ItemSet.Remove(nIdx);
				}
				else */
				if (nReg==0)
				{
				   Remove(nIdx,nx,nY,1);
				   ItemSet.Remove(nIdx);
				}
			} 
#endif
		return TRUE;
	}
	//药品 回城
	if (Item[nIdx].GetStackNum() > 1)
	{
		Item[nIdx].SetStackNum(Item[nIdx].GetStackNum() - 1);  //数量减一
		SynPlayerItem(nIdx);
	}
	else
	{
#ifdef _SERVER
		Remove(nIdx);
		ItemSet.Remove(nIdx);
#endif
	}

	if (Npc[Player[m_PlayerIdx].m_nIndex].m_Doing == do_sit)   //坐着的 让站起来 做动作
	{
		Npc[Player[m_PlayerIdx].m_nIndex].SendSerCommand(do_stand);
	}
	return TRUE;
}
//使用物品  客户端
#ifndef _SERVER
int KItemList::UseItem(int nIdx,int cnx,int cny)
{
	if (m_PlayerIdx <= 0)
		return FALSE;

	int nNpcIdx = Player[m_PlayerIdx].m_nIndex;

	if (0 == FindSame(nIdx))
	{
		return 0;
	}

	int		nRet = 0;
	switch(Item[nIdx].GetGenre())
	{
	case item_equip:
		{
		    int nSplace=GetEquipPlace(Item[nIdx].GetDetailType());
			int nEuqIdx=GetEquipment(nSplace);
			int nEuqWith=1,nEuqHigth=1;

			if 	(nEuqIdx)
			{//原来位置有东西
				 nEuqWith= Item[nEuqIdx].GetWidth();
				 nEuqHigth=	Item[nEuqIdx].GetHeight();
				 //开始检测包袱
			}
			DWORD nItemDwId=0;
			if (CanEquip(nIdx,nSplace)) //部位是否合适该装备
			{//开始换装 和卸装备
				//ClientShowMsg("<color=yellow>换装<color>:该装备满足换装条件");
				///Equip(nIdx,nSplace)   //右键换物品装备nNpcIdx 
				if (nEuqIdx)
				{//有装备开始卸装，开始检测包袱 是否有空间

				//	UnEquip(nEuqIdx,nSplace);
					
				}

				if ((Item[nIdx].GetDurability()>=0 && Item[nIdx].GetDurability()<=3) || Item[nIdx].GetIsCanUse()==0)
				{
				   ClientShowMsg("<color=yellow>换装C<color>:该装备不满足换装条件");
				   nRet = REQUEST_EQUIP_ITEM;
				   break;
				}

				/*if (Item[nIdx].GetUseKind!=0 && Item[nIdx].GetUseMapIdx()>0)
				{//等级限制的
					
					if (Npc[Player[m_PlayerIdx].m_nIndex].m_Level>=Item[nIdx].GetUseMapIdx())
					{
						ClientShowMsg("<color=yellow>换装D<color>:该装备不满足换装条件");
						nRet = REQUEST_EQUIP_ITEM;
						break;
					}
				}*/

				//然后开始上装
				/*	int		nPlace;
				int		nX;     // 0 1 2 3 4
				int		nY;     // 0123456789
				int     nSIsRongqi;
				int     nDIsRongqi;
                 */
				nItemDwId=Item[nIdx].GetID();
				/*ItemPos	SrcPos;
				SrcPos.nPlace=pos_equiproom;
				SrcPos.nX=cnx;
				SrcPos.nY=cny;
				ItemPos	DesPos;
				DesPos.nPlace = pos_equip;
				DesPos.nX = nSplace;
				DesPos.nY = 0;
				//包袱移动到 装备栏
				AutoEquip(SrcPos,DesPos,nItemDwId);*/

				PLAYER_AUTO_EQUIP eItem;
				eItem.ProtocolType = (BYTE)c2s_playerautoequip;
				//int nItemidx       = pos1->nIdx;
				eItem.uIiemDwId      = nItemDwId;
				if (g_pClient)
					g_pClient->SendPackToServer((BYTE*)&eItem, sizeof(PLAYER_AUTO_EQUIP));



			}
			else
			{//不能装备该装备
			  ClientShowMsg("<color=yellow>换装C<color>:该装备不满足换装条件");
			}

		    nRet = REQUEST_EQUIP_ITEM;
		}
		break;
	case item_townportal:   //使用回城
	case item_medicine:
		if (EatMecidineS(nIdx,cnx,cny))
			nRet = REQUEST_EAT_MEDICINE;
		break;
	case item_task:          //药品
		if (EatMecidineS(nIdx,cnx,cny))
			nRet = REQUEST_EAT_MEDICINE;
		break;
	case item_mine:
		if (EatMecidineS(nIdx,cnx,cny))
			nRet = REQUEST_EAT_MEDICINE;
		break;
	default:
		break;
	}
	return nRet;
}
#endif
//寻找空位置
//#ifndef _SERVER  GetFreeRoomCount

int KItemList::GetFreePositionCount(int nWidth, int nHeight,int RoomKind)
{
	if (nWidth < 1 || nHeight < 1 || RoomKind<room_equipment || RoomKind>=room_num)
	{
		return 0;
	}

	return m_Room[RoomKind].GetFreeRoomCount(nWidth, nHeight);
}

BOOL KItemList::SearchPosition(int nWidth, int nHeight, ItemPos* pPos)
{
	if (nWidth < 1 || nHeight < 1 || NULL == pPos)
	{
		return FALSE;
	}

	POINT	pPt;
	if (m_Room[room_equipment].FindRoom(nWidth, nHeight, &pPt)==TRUE)
	{

        pPos->nPlace = pos_equiproom;
		pPos->nX = pPt.x;
		pPos->nY = pPt.y;
	}
	else
	{
	  //if (0 != m_Hand)
//		{
//		return FALSE;
//		}
		pPos->nPlace = pos_hand;
		pPos->nX = 0;
		pPos->nY = 0;
		return FALSE;
	}
	return TRUE;
}

//自动换装
void KItemList::AutoEquip(ItemPos SrcPos,ItemPos DesPos,DWORD nSrcItemDwIdx,int nEuqPlace,int nKind)
{
    if (!nSrcItemDwIdx)
	   	return;
	if (Player[this->m_PlayerIdx].CheckTrading())	// 如果正在交易
		return;

	int nSrcItemIdx=ItemSet.SearchID(nSrcItemDwIdx);
	int nsListIdx = FindSame(nSrcItemIdx);
	//m_Items[nsListIdx].nPlace = pos_equiproom;
	 if (nsListIdx)
	 {//源位置
		 SrcPos.nPlace =m_Items[nsListIdx].nPlace;
		 SrcPos.nX       =m_Items[nsListIdx].nX;
	     SrcPos.nY       =m_Items[nsListIdx].nY;
	 }

	int nEuqWith=0,nEuqHigth=0,nEuqIdx=0;

	switch(SrcPos.nPlace)
	{//源目标
	case pos_equiproom:  //包袱--包袱
		{//包袱移动到 装备栏目
		    if (DesPos.nPlace==pos_equiproom)  //
			{
				//否则直接换上,清空包袱位置信息
				if (!m_Room[room_equipment].PickUpItem(nSrcItemIdx,SrcPos.nX,SrcPos.nY,Item[nSrcItemIdx].GetWidth(),Item[nSrcItemIdx].GetHeight()))
					return;

				 nEuqIdx=GetEquipment(nEuqPlace); //要装备的位置有东西

				if (nEuqIdx)
				{//如果有装备,就换下

					if (!m_Room[room_equipment].PlaceItem(DesPos.nX,DesPos.nY,nEuqIdx,Item[nEuqIdx].GetWidth(),Item[nEuqIdx].GetHeight()))
						return;

					//nEuqWith = Item[nEuqIdx].GetWidth();
					//nEuqHigth= Item[nEuqIdx].GetHeight();

					UnEquip(nEuqIdx,nEuqPlace);
				}

				Equip(nSrcItemIdx,nEuqPlace); //换到装备栏
				//设置包袱信息
				if (nEuqIdx)
				{//源装备栏有东西的话 就卸下来
					int nListIdx = FindSame(nEuqIdx);
					m_Items[nListIdx].nPlace = pos_equiproom;
					m_Items[nListIdx].nX = DesPos.nX;
					m_Items[nListIdx].nY = DesPos.nY;
				}
#ifdef _SERVER
               //发送协议到 客户端 换装
				PLAYER_AUTOCALLBACK_EQUIP	pMove;
				pMove.ProtocolType = s2c_pautoMovecallback;
				pMove.m_ItemDwId   = nSrcItemDwIdx;
				pMove.m_Splace     = SrcPos.nPlace;
				pMove.m_btSrcX     = SrcPos.nX;
				pMove.m_btSrcY     = SrcPos.nY;
				pMove.m_Dplace     = DesPos.nPlace;
				pMove.m_btDestX    = DesPos.nX;
				pMove.m_btDestY    = DesPos.nY;
				pMove.m_Kind          = nKind;
				if (g_pServer)
					g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&pMove, sizeof(PLAYER_AUTOCALLBACK_EQUIP));				
#endif
			}
		}
		break;
	case pos_equip: //卸载装备
		{//原来是装备栏的 ---包袱
			if (DesPos.nPlace==pos_equiproom)
			{//移动到包袱
				nEuqIdx=GetEquipment(nEuqPlace);
				if (nEuqIdx)
				{//如果有装备,就换下

					if  (!m_Room[room_equipment].PlaceItem(DesPos.nX,DesPos.nY,nEuqIdx,Item[nEuqIdx].GetWidth(),Item[nEuqIdx].GetHeight()))
						return; 

					//nEuqWith = Item[nEuqIdx].GetWidth();
					//nEuqHigth= Item[nEuqIdx].GetHeight();
					UnEquip(nEuqIdx,nEuqPlace);

					int nListIdx             = FindSame(nEuqIdx);
					m_Items[nListIdx].nPlace = pos_equiproom;
					m_Items[nListIdx].nX     = DesPos.nX;
					m_Items[nListIdx].nY     = DesPos.nY;
				}

			}
#ifdef _SERVER
			//发送协议到 客户端 换装
			PLAYER_AUTOCALLBACK_EQUIP	pMove;
			pMove.ProtocolType = s2c_pautoMovecallback;
			pMove.m_ItemDwId   = nSrcItemDwIdx;
			pMove.m_Splace     = SrcPos.nPlace;
			pMove.m_btSrcX     = SrcPos.nX;
			pMove.m_btSrcY     = SrcPos.nY;
			pMove.m_Dplace     = DesPos.nPlace;
			pMove.m_btDestX    = DesPos.nX;
			pMove.m_btDestY    = DesPos.nY;
			pMove.m_Kind       = nKind;
			if (g_pServer)
				g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&pMove, sizeof(PLAYER_AUTOCALLBACK_EQUIP));				
#endif
		}
		break;
	default:
		break;
	}

//#ifndef _SERVER
//
//	//客户端界面处理
//	//uParam = (KUiObjAtRegion*)pInfo -> 物品数据及其放置区域位置信息
//	//nParam = bAdd -> 0值表示减少这个物品，非0值表示增加这个物品
//
//	KUiObjAtContRegion pInfo1, pInfo2;	//源容器 目标容器
//
//	if (nSrcItemIdx == 0)
//	{
//		pInfo1.Obj.uGenre = CGOG_NOTHING;
//		pInfo1.Obj.uId = 0;
//		pInfo1.Region.Width = 0;
//		pInfo1.Region.Height = 0;
//	}
//	else
//	{
//		pInfo1.Obj.uGenre    = CGOG_ITEM;
//		pInfo1.Obj.uId       = nSrcItemIdx;
//		pInfo1.Region.Width  = Item[nSrcItemIdx].GetWidth();
//		pInfo1.Region.Height = Item[nSrcItemIdx].GetHeight();
//	}
//
//	if (nEuqIdx == 0)
//	{
//		pInfo2.Obj.uGenre = CGOG_NOTHING;
//		pInfo2.Obj.uId = 0;
//		pInfo2.Region.Width = 0;
//		pInfo2.Region.Height = 0;
//	}
//	else
//	{
//		pInfo2.Obj.uGenre    = CGOG_ITEM;	    //目标装备
//		pInfo2.Obj.uId       = nEuqIdx;
//		pInfo2.Region.Width  = Item[nEuqIdx].GetWidth();
//		pInfo2.Region.Height = Item[nEuqIdx].GetHeight();
//	}
//	
//	int PartConvert[itempart_num] = 
//	{
//		    UIEP_HEAD,
//			UIEP_BODY,
//			UIEP_WAIST,
//			UIEP_HAND,
//			UIEP_FOOT,
//			UIEP_FINESSE,
//			UIEP_NECK,
//			UIEP_FINGER1,
//			UIEP_FINGER2,
//			UIEP_WAIST_DECOR,
//			UIEP_HORSE,
//			UIEP_MASK,	// mat na
//			UIEP_PIFENG,	//披风
//			UIEP_YINJIAN,		//印鉴
//			UIEP_SHIPING,	//饰品
//	};
//	    //客户端对象信息赋值
//		switch(SrcPos.nPlace)
//		{
//		case pos_equiproom:
//			pInfo1.Region.h = SrcPos.nX;
//			pInfo1.Region.v = SrcPos.nY;
//			pInfo1.Region.IsRongqi = 1;
//			pInfo1.eContainer = UOC_ITEM_TAKE_WITH;
//			pInfo2.Region.h = DesPos.nX;
//			pInfo2.Region.v = DesPos.nY;
//			pInfo2.eContainer = UOC_ITEM_TAKE_WITH;
//			pInfo2.Region.IsRongqi = 1;
//			//sprintf(msg,"[KR_equiproom] X:%d-Y:%d",DesPos.nX,DesPos.nY);
//			//ClientShowMsg(msg);
//			break;
//		case pos_equip: //如果原来的容器　
//			/*pInfo1.Region.h = 0;
//			pInfo1.Region.v = PartConvert[SrcPos.nX];
//			pInfo1.eContainer = UOC_EQUIPTMENT;
//			pInfo1.Region.IsRongqi = 0;
//			pInfo2.Region.h = 0;
//			pInfo2.Region.v = PartConvert[DesPos.nX];
//			pInfo2.eContainer = UOC_EQUIPTMENT;
//			pInfo2.Region.IsRongqi = 0;*/
//			pInfo2.Region.h = DesPos.nX;
//			pInfo2.Region.v = DesPos.nY;
//			pInfo2.eContainer = UOC_ITEM_TAKE_WITH;
//			pInfo2.Region.IsRongqi = 1;
//			break;
//		}
//		//开始交换物品
//	   if (nKind<=0) //换装
//		  CoreDataChanged(GDCNI_OBJECT_CHANGED, (DWORD)&pInfo1, 0); //容器减少
//
//	   CoreDataChanged(GDCNI_OBJECT_CHANGED, (DWORD)&pInfo2, 1);   //容器增加
//#endif
//

}


//#ifndef _SERVER

BOOL KItemList::SearchItemPosition(int nWidth, int nHeight, ItemPos* pPos,int roomType)
{
	if (nWidth < 1 || nHeight < 1 || NULL == pPos)
	{
		return FALSE;
	}
	
	POINT	pPt;
	if (!m_Room[roomType].FindRoom(nWidth, nHeight, &pPt))
	{
		
/*		if (0 != m_Hand)
		{
			return FALSE;
		}*/
		pPos->nPlace = pos_hand;
		pPos->nX = 0;
		pPos->nY = 0;
		return FALSE;
	}
	else
	{
		if  (roomType==room_equipment)
		    pPos->nPlace = pos_equiproom;
		else if  (roomType==room_repository)
            pPos->nPlace = pos_repositoryroom;
		else if  (roomType==room_exbox1)
            pPos->nPlace = pos_exbox1room;
		else if  (roomType==room_exbox2)
            pPos->nPlace = pos_exbox2room;
		else if  (roomType==room_exbox3)
            pPos->nPlace = pos_exbox3room;
        else if  (roomType==room_equipmentex)
            pPos->nPlace = pos_equiproomex;
		else if  (roomType==room_trade)
            pPos->nPlace = pos_traderoom;
		else if  (roomType==room_trade1)
            pPos->nPlace = pos_trade1;
		else if  (roomType==room_immediacy)
            pPos->nPlace = pos_immediacy;
		else if  (roomType==room_give)
            pPos->nPlace = pos_give;
		else if  (roomType==room_dazao)
            pPos->nPlace = pos_dazao;
		else if  (roomType==room_cailiao)
            pPos->nPlace = pos_cailiao;
		else
            pPos->nPlace = pos_equiproom;

		pPos->nX = pPt.x;
		pPos->nY = pPt.y;
	}

	return TRUE;
}
//#endif
//搜索物品的DWID
int	KItemList::SearchID(int nID)
{
	if (m_PlayerIdx <= 0)
		return 0;
	int nIdx = 0;
	while(1)
	{
		nIdx = m_UseIdxItemList.GetNext(nIdx);
		if (!nIdx)
			break;
		if (Item[m_Items[nIdx].nIdx].GetID() == (DWORD)nID)
			return m_Items[nIdx].nIdx;
	}
	return 0;
}

//搜索物品的DWID
int	KItemList::SearchItemsIdx(int nDwid)
{
	if (m_PlayerIdx <= 0)
		return 0;
	int nItemIdx = 0;
	while(1)
	{
		nItemIdx = m_UseIdxItemList.GetNext(nItemIdx);
		if (!nItemIdx)
			break;
		if (Item[m_Items[nItemIdx].nIdx].GetID() == (DWORD)nDwid)
			return nItemIdx;
	}
	return 0;
}


void KItemList::ExchangeMoney(int pos1, int pos2, int nMoney)
{
	if (pos1 < 0 || pos2 < 0 || pos1 > room_trade || pos2 > room_trade)
		return;
/*	if (pos1 == room_repository && !Player[m_PlayerIdx].m_CUnlocked)
	{
		return;
	}*/

	if  (nMoney<0)
		return;

	if (m_Room[pos1].AddMoney(-nMoney))		// 源位置能拿出这么多钱来
	{
		if (!m_Room[pos2].AddMoney(nMoney))	// 目的地能放不下去
		{
			m_Room[pos1].AddMoney(nMoney);	// 还原源位置的钱
		}
	}
	else
	{
		return;
	}

#ifndef _SERVER
	if (pos1 == room_equipment && pos2 == room_repository)
		SendClientCmdStoreMoney(0, nMoney);
	else if (pos1 == room_repository && pos2 == room_equipment)
		SendClientCmdStoreMoney(1, nMoney);
#endif

#ifdef _SERVER
	SendMoneySync();
#endif	
}

//----------------------------------------------------------------------
//	功能：得到物品栏和储物箱的总钱数
//----------------------------------------------------------------------
int KItemList::GetMoneyAmount()
{
	return (m_Room[room_equipment].GetmServer() + m_Room[room_repository].GetmServer());
}

int KItemList::GetXuAmount()
{
	return (m_Room[room_equipment].GetxCLient());
}
//----------------------------------------------------------------------
//	功能：得到物品栏钱数
//----------------------------------------------------------------------
int KItemList::GetEquipmentMoney()
{
	return m_Room[room_equipment].GetmServer();
}

int KItemList::GetEquipmentXu()
{
	return m_Room[room_equipment].GetxCLient();
}


BOOL KItemList::AddmServer(int nRoom, int nMi)
{
	if (nRoom < 0 || nRoom >= room_num)
		return FALSE;

	if ( !m_Room[nRoom].AddMoney(nMi) )
		return FALSE;

#ifdef _SERVER
	SendMoneySync();
#endif

	return TRUE;
}
//增加金币
BOOL KItemList::AddxCliet(int nRoom, int ni)
{
	if (nRoom < 0 || nRoom >= room_num)
		return FALSE;

	if ( !m_Room[nRoom].AddxCliet(ni) )
		return FALSE;

#ifdef _SERVER
	SendXuSync(); //同步到客户端
#endif

	return TRUE;
}

BOOL KItemList::CostMoney(int nMoney)
{
	if (nMoney > GetEquipmentMoney())
		return FALSE;

	if (!m_Room[room_equipment].AddMoney(-nMoney) )
		return FALSE;

#ifdef _SERVER
	SendMoneySync();
#endif

	return TRUE;
}
//减少金币
BOOL KItemList::CostXu(int nXu)
{
	if (nXu > GetEquipmentXu())
		return FALSE;

	if ( !m_Room[room_equipment].AddxCliet(-nXu) )
		return FALSE;

#ifdef _SERVER
	SendXuSync();
#endif

	return TRUE;
}

BOOL KItemList::DecMoney(int nMoney)
{
	if (nMoney < 0)
		return FALSE;

	if (nMoney > m_Room[room_equipment].GetmServer())
	{
		nMoney -= m_Room[room_equipment].GetmServer();
		SetRoomMoney(room_equipment, 0);
		if (nMoney > m_Room[room_repository].GetmServer())
			SetRoomMoney(room_repository, 0);
		else
			AddmServer(room_repository, -nMoney);
	}
	else
	{
		AddmServer(room_equipment, -nMoney);
	}

#ifdef _SERVER
	SendMoneySync();
#endif

	return TRUE;
}
/*
BOOL KItemList::DecXu(int nXu)
{
	if (nXu < 0)
		return FALSE;

	if (nXu > m_Room[room_equipment].GetXu())
	{
		nXu -= m_Room[room_equipment].GetXu();
		SetRoomXu(room_equipment, 0);
	}
	else
	{
		AddXu(room_equipment, -nXu);
	}

#ifdef _SERVER
	SendXuSync();
#endif

	return TRUE;
}
*/
#ifdef _SERVER
//----------------------------------------------------------------------------------
//	功能：调用此接口必须保证传入的nMoney是一个有效数(正数且不超过所有钱数)
//----------------------------------------------------------------------------------
void	KItemList::TradeMoveMoney(int nMoney)
{
	// 自己钱的处理
	m_Room[room_trade].SetClientMon(nMoney);

	SendMoneySync();

	//给对方发消息
	TRADE_MONEY_SYNC	sMoney;
	sMoney.ProtocolType = s2c_trademoneysync;
	sMoney.m_nMoney = nMoney;
	g_pServer->PackDataToClient(Player[Player[m_PlayerIdx].m_cTrade.m_nTradeDest].m_nNetConnectIdx, (BYTE*)&sMoney, sizeof(TRADE_MONEY_SYNC));
}
#endif

#ifdef _SERVER
//----------------------------------------------------------------------------------
//	功能：服务器发money同步信息给客户端
//----------------------------------------------------------------------------------
void	KItemList::SendMoneySync()
{
	PLAYER_FS_MONEY_SYNC	sMoney;
	sMoney.ProtocolType = s2c_syncmonfs;
	sMoney.m_nMoney1 = m_Room[room_equipment].GetmServer();
	sMoney.m_nMoney2 = m_Room[room_repository].GetmServer();
	sMoney.m_nMoney3 = m_Room[room_trade].GetmServer();
	g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMoney, sizeof(PLAYER_FS_MONEY_SYNC));
}
//金币同步
void	KItemList::SendXuSync()
{
	PLAYER_FS_XU_SYNC	sXu;
	sXu.ProtocolType = s2c_syncqianfs;
	sXu.m_nXu = m_Room[room_equipment].GetxCLient();
	g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sXu, sizeof(PLAYER_FS_XU_SYNC));
}
#endif

void KItemList::SetClient(int nMon1, int nMon2, int nMon3)
{
	m_Room[room_equipment].SetClientMon(nMon1);
	m_Room[room_repository].SetClientMon(nMon2);
	m_Room[room_trade].SetClientMon(nMon3);
#ifndef _SERVER
	KUiObjAtContRegion	sMoney;
	sMoney.Obj.uGenre = CGOG_MONEY;
	sMoney.Obj.uId = nMon2;
	sMoney.eContainer = UOC_STORE_BOX;
	CoreDataChanged(GDCNI_OBJECT_CHANGED, (DWORD)&sMoney, 1);
#endif
}

void KItemList::SetServer(int nXu)
{
	m_Room[room_equipment].SetxClient(nXu);
}

void KItemList::SetRoomMoney(int nRoom, int nMoney)
{
	if (nRoom >= 0 && nRoom < room_num)
		m_Room[nRoom].SetClientMon(nMoney);
}

void KItemList::SetRoomXu(int nRoom, int nXu)
{
	if (nRoom >= 0 && nRoom < room_num)
		m_Room[nRoom].SetxClient(nXu);
}
//===========================蓝装提炼============================================
int KItemList::BlueTiLian(int nLindexID,int nBaoshiID,int nModel,int nWengGangPos)
{
			// 装备熔炼  ---提炼晶石
				if (nLindexID && Item[nLindexID].GetGenre() == item_equip && (Item[nLindexID].GetGoldId() > 0 || Item[nLindexID].IsPurple()>0|| Item[nLindexID].IsBlue()))
				{
#ifdef _SERVER
					int nItemClass		= Item[nLindexID].GetGenre();
					int nDetailType		= Item[nLindexID].GetDetailType();
					int nParticularType	= Item[nLindexID].GetParticular();
					int nLevel			= Item[nLindexID].GetLevel();
					int nSeries			= Item[nLindexID].GetSeries();
					int nLuck			= Item[nLindexID].GetItemParam()->nLuck;
					int nEnChance		= Item[nLindexID].GetEnChance();
					int nGoldId			= Item[nLindexID].GetGoldId();	
					int nVersion		= Item[nLindexID].GetItemParam()->nVersion;
					UINT	nGndrom		= Item[nLindexID].GetItemParam()->uRandomSeed;
					int nPoint			= Item[nLindexID].GetPoint();
					int nRPoint			= Item[nLindexID].IsRong();
					//int nMlevel         = Item[nBaoshiID].GetLevel();  //属性书的等级
					int nBlue           = Item[nLindexID].IsBlue();
                    int nWenGangNum     = Item[nLindexID].GetUsedRong();
					int nItemLevel[6];
					int nRItemLevel[6];
					ZeroMemory(nItemLevel, sizeof(nItemLevel));
					ZeroMemory(nRItemLevel, sizeof(nRItemLevel));
					//同步魔法属性
					for (int i = 0; i < 6; ++i)
					{
						nItemLevel[i]  = Item[nLindexID].GetItemParam()->nGeneratorLevel[i];
//						nRItemLevel[i] = Item[nLindexID].GetItemParam()->nRGeneratorLevel[i];
					}
/////////////////////////////////////////////////////////////////////////////////////////////////////
	               	int nMagType=0,nMagVal=0;
                    Item[nLindexID].SetRongTypeAndVal(nWengGangPos-1,0,0); // 设置为空值
	//	tabMagic.Load("\\Settings\\item\\magicattrib.txt");
	//	int nMagic;
	//	tabMagic.GetInteger(nMinRow,Item[nLindexID].GetGenre() + 13,0,&nMagic);
	//	if (nMagic <= 0) //可出现该属性的装备类型
	//	{
	//	   return 0;
	//	}
	//	tabMagic.Clear();

		/*/已经镶套了的相同属性的不能镶套
		for (int p = 0;p < 6;p++)
		{
          if (Item[nId1].GetAttribType(p) == Item[nId3].GetBaseMagic())  
               return 0; 
		  else if(Item[nId1].GetRongAttribType(p) == Item[nId3].GetBaseMagic())
               return 0; 
		}
        *///同步熔炼属性值 和 判断 属性石头的位置
		            int nMun=0;

		            for (int k = 0;k <nRPoint;++k)
					{ 
			           if (k==(nWengGangPos-1))
					   {    
                         nRItemLevel[k]=0;
				         continue;
					   }    
		               nRItemLevel[k] = Item[nLindexID].m_GeneratorParam.nRGeneratorLevel[k];
					}  
		            int nStackNum = 1;
//////////////////////////////////////////////////////////////////////////////////////////////////////
					int	nIndex;			
					if (nGoldId>0)
					{
						nIndex = ItemSet.AddGold(nGoldId,nItemLevel,nSeries,nEnChance,nRPoint,0,0,0,0,0,nRItemLevel,NULL,nLevel,1,-1,-1,1,0,0,TRUE,nGndrom);
					} 
					else
					{
						nIndex = ItemSet.AddRong(0,nSeries,nLevel,nLuck,nDetailType,nParticularType,nItemLevel,nRItemLevel,NULL,nVersion,nGndrom,1,nEnChance,nRPoint,nPoint,0,NULL,0,0,0,0,0,0,TRUE);
					}	
					if (nIndex <= 0)
					{
						return 0;
					}			
					Remove(nBaoshiID);
					ItemSet.Remove(nBaoshiID);
					Remove(nLindexID);
					ItemSet.Remove(nLindexID);
					int	x, y;
					if (CheckCanPlaceInEquipment(Item[nIndex].GetWidth(), Item[nIndex].GetHeight(), &x, &y))
					{
						Add(nIndex,pos_equiproom, x, y);   //包袱
					}
					//-------------产生纹钢----------------

					//-------------------------------------
					return nIndex;
#endif
				}
					return 0;

}

//=====================装备纹钢提炼 和 重铸==================================
int KItemList::TiLian(int nLindexID,int nBaoshiID,int nModel,int nWengGangPos)
{
			// 蓝装提篮炼  ---提炼晶石
				if (nLindexID && Item[nLindexID].GetGenre() == item_equip && (Item[nLindexID].GetGoldId() > 0 || Item[nLindexID].IsPurple()>0|| Item[nLindexID].IsBlue()))
				{
#ifdef _SERVER
					int nItemClass		= Item[nLindexID].GetGenre();
					int nDetailType		= Item[nLindexID].GetDetailType();
					int nParticularType	= Item[nLindexID].GetParticular();
					int nLevel			= Item[nLindexID].GetLevel();
					int nSeries			= Item[nLindexID].GetSeries();
					int nLuck			= Item[nLindexID].GetItemParam()->nLuck;
					int nEnChance		= Item[nLindexID].GetEnChance();
					int nGoldId			= Item[nLindexID].GetGoldId();	
					int nVersion		= Item[nLindexID].GetItemParam()->nVersion;
					UINT nGndrom			= Item[nLindexID].GetItemParam()->uRandomSeed;
					int nPoint			= Item[nLindexID].GetPoint();
					int nRPoint			= Item[nLindexID].IsRong();
					int nMlevel         = Item[nBaoshiID].GetLevel();  //属性书的等级
					int nBlue           = Item[nLindexID].IsBlue();
                    int nWenGangNum     = Item[nLindexID].GetUsedRong();
					int nItemLevel[6],nRItemLevel[6],nBRItemLevel[6];//nBItemLevel[6];//
					ZeroMemory(nItemLevel, sizeof(nItemLevel));
					ZeroMemory(nRItemLevel, sizeof(nRItemLevel));
					//ZeroMemory(nBItemLevel, sizeof(nBItemLevel));
					//同步魔法属性
					int i;
					for (i = 0; i < 6; ++i)
					{
						nItemLevel[i]  = Item[nLindexID].GetItemParam()->nGeneratorLevel[i];
//						nBItemLevel[i] = Item[nLindexID].GetItemParam()->nBGeneratorLevel[i];
					}
/////////////////////////////////////////////////////////////////////////////////////////////////////
	//	tabMagic.Load("\\Settings\\item\\magicattrib.txt");
	//	int nMagic;
	//	tabMagic.GetInteger(nMinRow,Item[nLindexID].GetGenre() + 13,0,&nMagic);
	//	if (nMagic <= 0) //可出现该属性的装备类型
	//	{
	//	   return 0;
	//	}
	//	tabMagic.Clear();

		/*/已经镶套了的相同属性的不能镶套
		for (int p = 0;p < 6;p++)
		{
          if (Item[nId1].GetAttribType(p) == Item[nId3].GetBaseMagic())  
               return 0; 
		  else if(Item[nId1].GetRongAttribType(p) == Item[nId3].GetBaseMagic())
               return 0; 
		}
        *///同步熔炼属性值 和 判断 属性石头的位置
		int nMun=0;
		int nMagType=0,nMagVal=0;
    if (nModel>0 && nWengGangPos>0)
	{ 
        Item[nLindexID].SetRongTypeAndVal(nWengGangPos,0,0); // 设置某个位置为空值

		for (i = 0;i <nRPoint;++i)
		{
			if (i==(nWengGangPos-1))
			{
				 nBRItemLevel[i] = Item[nLindexID].m_GeneratorParam.nRGeneratorLevel[i];
                 nRItemLevel[i]  = 0;
				 //-------------产生纹钢----------------
				 if (nBRItemLevel[i]>0)
					 WenGangGenerator(nBRItemLevel[i]);
                 //-------------------------------------
				 continue;
			}

		    nRItemLevel[i] = Item[nLindexID].m_GeneratorParam.nRGeneratorLevel[i]; //魔法属性表的行号
		}
	} 
    else
	{//一次行提取纹钢  
	  for (i = 0;i <nRPoint;++i)
	  {     
		 nBRItemLevel[i] = Item[nLindexID].m_GeneratorParam.nRGeneratorLevel[i];
	     nRItemLevel[i]=0; 	 
//-------------产生纹钢----------------
		 if (nBRItemLevel[i]>0)
             WenGangGenerator(nBRItemLevel[i]);
//-------------------------------------    
	  } 
	}  	
	
	  for (i = 0;i <nRPoint;++i)
	  {     
		 Item[nLindexID].m_GeneratorParam.nRGeneratorLevel[i]=nRItemLevel[i]; 
		 Item[nLindexID].SetRongTypeAndVal(i,0,0);  //设置为空
	  } 

	  Remove(nBaoshiID);
	  ItemSet.Remove(nBaoshiID);	        
/*-----------------------------------------------------------
					int	nIndex,nStackNum = 1;		
					if (nGoldId>0)
					{
						nIndex = ItemSet.AddGold(nGoldId,nItemLevel,nSeries,nEnChance,nRPoint,0,0,0,0,0,nRItemLevel);
					} 
					else
					{
						nIndex = ItemSet.AddRong(0,nSeries,nLevel,nLuck,nDetailType,nParticularType,nItemLevel,nRItemLevel,NULL,nVersion,nGndrom,1,nEnChance,nRPoint,nPoint);
					}	
					if (nIndex <= 0)
					{
						return 0;
					}
					
					Remove(nBaoshiID);
					ItemSet.Remove(nBaoshiID);
					Remove(nLindexID);
					ItemSet.Remove(nLindexID);

					int	x, y;
					if (CheckCanPlaceInEquipment(Item[nIndex].GetWidth(), Item[nIndex].GetHeight(), &x, &y))
					{
						Add(nIndex,pos_equiproom, x, y);   //包袱
					}
*/
//---------------------------------------------------------开始同步属性
						ITEM_SYNC	sItem;
                        memset(&sItem, 0, sizeof(sItem));
						sItem.ProtocolType   = s2c_syncequipitem;
						sItem.m_ID           = Item[nLindexID].GetID();
						sItem.m_SyncType     = 2;
						//同步魔法属性
						for (i = 0; i < 6; ++i)
						{
							sItem.m_RongMagicLev[i] = nRItemLevel[i];
//							sItem.m_BaoMagicLev[i]  = Item[nLindexID].GetItemParam()->nBGeneratorLevel[i];
						}

						sItem.m_Luck                = Item[nLindexID].GetItemParam()->nLuck;
						sItem.m_Rongpiont           = nRPoint;//Item[nLindexID].IsRong(); //熔炼

						sItem.m_Point               = Item[nLindexID].GetPoint();
						sItem.m_IsWhere             = Item[nLindexID].GetIsWhere();
                        sItem.m_IsCanUse            = 1;
						sItem.m_Series              = Item[nLindexID].GetSeries();
						sItem.m_IsBang              = Item[nLindexID].GetIsBang();
	                    sItem.m_IsKuaiJie           = Item[nLindexID].GetIsKuaiJie();
						sItem.m_EnChance            = Item[nLindexID].GetEnChance();
						sItem.m_Durability          = Item[nLindexID].GetDurability();

						sItem.m_RandomSeed          = nGndrom;

			            g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sItem, sizeof(ITEM_SYNC));
//---------------------------------------------------------
					return nLindexID;
#endif
				}
					return 0;

}
//=================纹钢产生器=======================
#ifdef _SERVER

KTabFile g_FusionTab;

int KItemList::WenGangGenerator(int nRItemLevelRows)
{//如果属性行号 大于 0 
			int mEventRows=g_FusionTab.GetHeight(),mReg=0;
 			//char msg[64];
			//sprintf(msg,"提示:(mEventRows:%d)!",mEventRows);
			//msgshow(msg);
			int nEvenId=0;

			for (int q=0;q<mEventRows;++q)
			{
				int nVal=0;
				g_FusionTab.GetInteger(q+1, "魔法属性索引1", 0, &nVal);
				if (nVal==nRItemLevelRows)
				{
					g_FusionTab.GetInteger(q+1, "详细类别", 0, &nEvenId);
					break;
				}
			} 

			if (nEvenId>0)
			{//开始添加纹钢=
				int  mIndex=0;
				     mIndex=ItemSet.AddOther(item_fusion,0,1,10,1,nEvenId,0,0,0,1,0,0/*, nYear, nMonth, nDay, nHour,nMin*/);
				
				if (mIndex <= 0) 
				{
					return FALSE;
				}

				int	x, y;
				if (CheckCanPlaceInEquipment(Item[mIndex].GetWidth(), Item[mIndex].GetHeight(), &x, &y))
				{
					Add(mIndex, pos_equiproom, x, y);
					msgshow("提示:纹钢提炼成功!恭喜恭喜!");
				} 
			}
			
		//}
		//char msg[64];
		//sprintf(msg,"提示:(%d)(%d)测试测试!",nEvenId,nRItemLevelRows);
		//msgshow(msg);
		return TRUE;
}
#endif
//================黄金装备纹钢=====================
int KItemList::WenGang(int nLindexID,int nBaoshiID)
{
				if (nLindexID && Item[nLindexID].GetGenre() == item_equip && (Item[nLindexID].GetGoldId() > 0 || Item[nLindexID].IsPurple()>0 || Item[nLindexID].IsBlue()))
				{
#ifdef _SERVER
					int nItemClass		= Item[nLindexID].GetGenre();
					int nDetailType		= Item[nLindexID].GetDetailType();
					int nParticularType	= Item[nLindexID].GetParticular();
					int nLevel			= Item[nLindexID].GetLevel();
					int nSeries			= Item[nLindexID].GetSeries();
					int nLuck			= Item[nLindexID].GetItemParam()->nLuck;
					int nEnChance		= Item[nLindexID].GetEnChance();
					int nGoldId			= Item[nLindexID].GetGoldId();	
					int nVersion		= Item[nLindexID].GetItemParam()->nVersion;
					UINT nGndrom		= Item[nLindexID].GetItemParam()->uRandomSeed;
					int nPoint			= Item[nLindexID].GetPoint();   //紫装IsPurple
					int nRPoint			= Item[nLindexID].IsRong();
					int nMlevel         = Item[nBaoshiID].GetLevel();  //纹钢的等级
					int nBlue           = Item[nLindexID].IsBlue();
                   	int nIsPlatina      = Item[nLindexID].GetIsPlatima();

					int nItemLevel[6];
					int nRItemLevel[6];
					//int nBItemLevel[6];
					ZeroMemory(nItemLevel, sizeof(nItemLevel));
					ZeroMemory(nRItemLevel, sizeof(nRItemLevel));

					if (nRPoint<=0)
					{
						nRPoint=GetRandomNumber(2,6);			
					}
					if (nRPoint>6)
					{
						nRPoint=GetRandomNumber(2,6);			
					} 
                     
				    if (Item[nBaoshiID].GetGenre()!=item_fusion)
					{
					   	msgshow("提示:放的材料不是(新)纹钢!");	
						return 0;
					}

                    int nMagic=0,nMgIdx=0 ,i;

	                for (i = 0;i < 6;++i)
					{   	    
		              if (Item[nLindexID].GetDetailType()==Item[nBaoshiID].GetAttribTypeVala(i))
					  {    
			             nMagic++;
					  }   
					}      

		           if (nMagic<= 0) 
				   {  //可出现该属性的装备类型
		             msgshow("提示:此纹钢与装备类型不符合!");
		             return 0;
				   }   
					//同步魔法属性
				   for (i = 0; i < 6; ++i)
				   {
					 nItemLevel[i]  = Item[nLindexID].GetItemParam()->nGeneratorLevel[i];
					 nRItemLevel[i] = Item[nLindexID].GetItemParam()->nRGeneratorLevel[i];
//					 nBItemLevel[i]  = Item[nLindexID].GetItemParam()->nBGeneratorLevel[i];
				   }
//--------------------------------------------------------------------------------------
                    //确定成功的机率
					int nVal=0,nRate=0;
//--------------概率检测----------------------------------------------------------------
					g_GameSetTing.GetInteger2("ServerConfig","GoldWenGangRank",&nVal,&nRate);
					//运气石的等级
					if (g_Random(nVal) < g_Random(nRate))
					{
                        Remove(nBaoshiID);
						ItemSet.Remove(nBaoshiID); //删除材料
						msgshow("提示:黄金纹钢属失败!再接再励!");	
						return 0;
					}
		//可溶链的 装备位置
		KTabFile tabMagic;
		tabMagic.Load(TABFILE_MAGICAGOLD_PATH);
		int nRow;
        nRow = Item[nBaoshiID].GetSkillType();  //获取改属性的行号

		if (nLuck < 0) 
			nLuck = 0;
		if (nLuck > 10) 
			nLuck = 10;

		if (nRow<=0)
		{
		   msgshow("提示:该属性纹钢已经失效!");
		   tabMagic.Clear();
           return 0; 
		}

		//已经镶套了的相同属性的不能镶套
         tabMagic.GetInteger(nRow,5,0,&nMgIdx);

		 for (i = 0;i < 6;++i)
		 { 
			
			if(Item[nLindexID].GetAttribType(i) == nMgIdx)
			{
			   msgshow("提示:该装备已经有了相同的属性!");
			   tabMagic.Clear();
			   return 0; 
			}

		    if(Item[nLindexID].GetRongAttribType(i) == nMgIdx)
			{
			   msgshow("提示:该黄金已经纹钢了同类型纹钢!");
			   tabMagic.Clear();
               return 0; 
			}
		 }  

		 tabMagic.Clear();
        //同步熔炼属性值 和 判断 属性石头的位置
		            int nMun=0;
		            for (i = 0;i <nRPoint;++i)
					{	  
		                if (nMun == 0 && Item[nLindexID].GetRongAttribType(i) <= 0)  //位置相同 并且没有属性的空位置
						{    
			       	       nRItemLevel[i] = nRow;   //设定属性行数
				           nMun++;
				           continue;
						}    
			            nRItemLevel[i] = Item[nLindexID].m_GeneratorParam.nRGeneratorLevel[i];
					}
					
					if 	(nMun==0)
					{
						msgshow("提示:该装备已经不能再纹钢了!");
						return 0;
					}
					
		            int nStackNum = 1;
//////////////////////////////////////////////////////////////////////////////////////////////////////

					/*
					int KItemSet::AddGold(IN int nId , IN int* pnMagicLevel , \
					IN int nSeries,IN int nEnChance,\
					IN int nRongpiont, \
					int nYear, int nMonth, int nDay, int nHour,int nMin,\
					IN int* pnRMagicLevel,IN int* pnBMagicLevel,\
					IN int nLevel,IN int nLuck, IN int nDetailType, \
					IN int nParticular,IN int nStackNum,IN int nVersion,int nIsPlatina)	
					*/
					int	nIndex;			
					if (nGoldId>0)
					{//黄金的纹钢
						nIndex = ItemSet.AddGold(nGoldId,nItemLevel,nSeries,nEnChance,nRPoint,0,0,0,0,0,nRItemLevel,NULL,nLevel,1,-1,-1,1,0,nIsPlatina,TRUE,nGndrom);
					} 
					else
					{//其他紫装和蓝装的纹钢
						nIndex = ItemSet.AddRong(0,nSeries,nLevel,nLuck,nDetailType,nParticularType,nItemLevel,nRItemLevel,NULL,nVersion,nGndrom,1,nEnChance,nRPoint,nPoint,0,NULL,0,0,0,0,0,0,TRUE);
					}
						
					if (nIndex <= 0)
					{
						return 0;
					}	
					
					Remove(nBaoshiID);
					      ItemSet.Remove(nBaoshiID);
					Remove(nLindexID);
					      ItemSet.Remove(nLindexID);

					int		x, y;
					if (CheckCanPlaceInEquipment(Item[nIndex].GetWidth(), Item[nIndex].GetHeight(), &x, &y))
					{
						Add(nIndex, pos_equiproom, x, y);   //包袱
					}
					else
					{
                        msgshow("提示:包袱空间不足,前功尽弃啦!");
						Remove(nIndex);
					    ItemSet.Remove(nIndex);
					    return 0;
					}
                        msgshow("提示:纹钢成功!恭喜!恭喜!");

					return nIndex;
#endif 
				}
					return 0;
}


//================蓝装装备融合========================
int KItemList::BlueRonHe(int nIdxa,int nIdxb,int nEqType,int nRank,int nCurLuck,int nsLevel)
{
    if (!Player[m_PlayerIdx].ExeItemScriptA("\\script\\system\\sysitemronhe.lua","ItemRonHe",Item[nIdxa].GetName(),Item[nIdxb].GetName(),nIdxa,nIdxb,nEqType,nRank,nCurLuck,nsLevel,0,0,0))
	   return FALSE;

    return TRUE;
}

//================蓝装装备辨识========================
int KItemList::BlueBianShi(int nLindexID,int nBaoshiID)
{
				if (nLindexID && Item[nLindexID].GetGenre() == item_equip && (Item[nLindexID].GetGoldId() > 0 || Item[nLindexID].IsPurple()>0 || Item[nLindexID].IsBlue()))
				{
#ifdef _SERVER
					int nLuck			= Item[nLindexID].GetItemParam()->nLuck;
					int nRPoint			= Item[nLindexID].IsRong();
					int nMlevel         = Item[nBaoshiID].GetLevel();   //纹钢的等级
				    int nWhere          = Item[nLindexID].GetIsWhere(); 
					int nEnChance       = Item[nLindexID].GetEnChance();
					UINT nItemRandSeed  = Item[nLindexID].GetItemParam()->uRandomSeed;
					int nVal=0,nRate=0, i,nCuRoint=0;
                   if  (Item[nLindexID].GetIsCanUse()==0)
				  {
                        msgshow("提示:该装备不能辨识!");	
						return 0;
				  }
					if (nLuck < 0) 
						nLuck = 0;
					if (nLuck > 10) 
						nLuck = 10;

					if (nRPoint>6)
					{
						nRPoint=GetRandomNumber(1,4);			
					} 
                    
					if (nRPoint==0)
					    nRPoint=GetRandomNumber(1,2);

					   nCuRoint = g_Random(nRPoint);

                    if  (nCuRoint==0)
					{
                        /*Remove(nBaoshiID);
						ItemSet.Remove(nBaoshiID); //删除材料
						msgshow("提示:属性辨识失败C!再接再励!");	
						return 0;*/
						nCuRoint=GetRandomNumber(1,2);
					}
                    //--------------概率检测--------------------------------------------
					g_GameSetTing.GetInteger2("ServerConfig","BianshiRank",&nVal,&nRate);
                        //运气石的等级
					if (nWhere>0)
					{
                        Remove(nBaoshiID);
						ItemSet.Remove(nBaoshiID); //删除材料
						msgshow("提示:该装备已经辨识过的哦!");	
						return 0;
					}

					if (g_Random(nMlevel*nVal) < g_Random(nRate))
					{
                        Remove(nBaoshiID);
					    ItemSet.Remove(nBaoshiID); //删除材料
						msgshow("提示:属性辨识失败!再接再励!");	
					    return 0;
					}
                   //-------------------------------------------------------------------                 

					Remove(nBaoshiID);
					ItemSet.Remove(nBaoshiID);

				    ///=================以下为辨识属性=====================================================
	              KTabFile nRongt;
                        nRongt.Load(TABFILE_BIANSHI_PATH);
	              int nRows = nRongt.GetHeight(),isHave=0,bRows=0,nMagRows[6];
				  nMagRows[0]=0;nMagRows[1]=0;nMagRows[2]=0;nMagRows[3]=0;nMagRows[4]=0;nMagRows[5]=0;nMagRows[6]=0;
	             
                    //-----------开始检测概率---------------
					int DropRate[2];
                        g_GameSetTing.GetInt2("SYSTEM","BianDropRate",DropRate);

					int nGoldLuck=g_GlobalMissionArray.GetMissionValue(28),nPlayernLucky=0;

					   if (nGoldLuck<=0)
					       nGoldLuck=1;
									
					    nPlayernLucky=Player[m_PlayerIdx].m_nCurLucky*Npc[Player[m_PlayerIdx].m_nIndex].IsLuKey*nGoldLuck;

					int nTypeR,nMaxR,nMinR;
					char  mag[64];
                   //-------------------------------检测是否有相同的两个属性是否可以出现-----------------------
					for ( i = 0;i < 6;++i)
					{
					   if (i < nCuRoint)
					   {
                            int nIsThis=0;
						   
							//nRongt.GetInteger(bRows,"是否出现",0,&nIsThis);

							for(int k=0;k<nRows;++k)
							{
							    bRows= GetRandomNumber(2,nRows);
		                        nRongt.GetInteger(bRows,"魔法属性索引1",0,&isHave);
                                nRongt.GetInteger(bRows,Item[nLindexID].GetDetailType()+4,0,&nIsThis);

								if (nIsThis<=0)
								{//该部位不可以出现这个属性 就取下一个
								    isHave=0;
                                    continue;
								}

								//sprintf(mag,"部位:%d,ROW:%d,This:%d",Item[nLindexID].GetDetailType(),bRows,nIsThis);
                                //msgshow(mag);

                                int nDecide = GetRandomNumber(DropRate[0],DropRate[1]-1)/(1 + nPlayernLucky * 20/100);  //运气值 10000 500000=25/1250||3000/160000 
                                    
							    if (nIsThis<=nDecide)
								{   
								    isHave=0;
									continue;
								}

								//sprintf(mag,"概率:%d||||%d",nIsThis,nDecide);
                                //msgshow(mag);
                                //--------------------------------------  
                                int isReg=0,nIsthat=0,nIsKong=0,nIsYthis=0;

								for (int t=0;t<nCuRoint;++t)
								{//检测是否有这个属性了

								   if (Item[nLindexID].GetItemParam()->nRGeneratorLevel[t]>0)
								   {
								       if (nMagRows[t]>1)
                                          nRongt.GetInteger(nMagRows[t],"属性ID",0,&nIsYthis);

									   nRongt.GetInteger(bRows,"属性ID",0,&nIsthat);

									   if (nIsYthis>0 && nIsthat>0 && nIsYthis==nIsthat)
									   {
									      isReg=1;
										  break;
									   }
								   }
								   else
								   {//空属性
								      nIsKong++;		    
								   }
								}

                                if (isReg>0)
							    {//已经存在这个属性 取下一个
								  isHave=0;
							      continue;
						  	    }

								break; //就是这个属性了 跳出
							}

						   if (isHave>0)
						   { //如果可以出现的话
							   Item[nLindexID].GetItemParam()->nRGeneratorLevel[i]=isHave;
							   nMagRows[i]=bRows;  //记录所取的行号
							   
							   //sprintf(mag,"测试(%d):Luck:%d,Row:%d,CurR:%d",i,nPlayernLucky,nMagRows[i],isHave);
                               //msgshow(mag);
						   }
						   else
						   {
						       Item[nLindexID].GetItemParam()->nRGeneratorLevel[i]=0;
							   nMagRows[i]=0;
                               //continue;  //下一个
						   }
                           /*
						   for (int k = 0;k < 6;++k)
						   { 	   
							   if (Item[nLindexID].GetItemParam()->nRGeneratorLevel[i]==Item[nLindexID].GetItemParam()->nRGeneratorLevel[k])
							   {//有相同的
								   if (k!=i)
								   {//位置不相同的 并且是大于0的
                                      Item[nLindexID].GetItemParam()->nRGeneratorLevel[k]=0;
								   }
							   }
						   }*/
					   }

					}
                   nRongt.Clear();
//-----------------------------------------------------------------------------
                   int IsKong=0;
                   for (i = 0; i < 6; ++i)
				   {
							if (Item[nLindexID].GetItemParam()->nRGeneratorLevel[i]>0)
								IsKong++;
				   }
					
				   if (IsKong<=0)
				   {
					  msgshow("提示:属性辨识失败B!再接再励!");	
					  return 0;
				   }
												
					KItemNormalAttrib	sMAR[6];// 魔法
					ZeroMemory(sMAR,sizeof(sMAR)); //分配内存
					KTabFile MagicTabR;
					MagicTabR.Load(TABFILE_MAGICAGOLD_PATH);  //熔炼蓝装备魔法属性文件	 
					int inPont=0;

					g_RandomSeed(nItemRandSeed);

					for ( i = 0;i < 6;++i)
					{ 
						if (Item[nLindexID].GetItemParam()->nRGeneratorLevel[i] > 0 && inPont < nCuRoint)
						{ 
							MagicTabR.GetInteger(Item[nLindexID].GetItemParam()->nRGeneratorLevel[i],5,0,&nTypeR);
							MagicTabR.GetInteger(Item[nLindexID].GetItemParam()->nRGeneratorLevel[i],6,0,&nMaxR);
							MagicTabR.GetInteger(Item[nLindexID].GetItemParam()->nRGeneratorLevel[i],7,0,&nMinR);
							sMAR[i].nAttribType = nTypeR;  //属性类型
							//sMAR[i].nValue[0] = nMaxR + ((nMinR - nMaxR) * nLuck / 10);
							sMAR[i].nValue[0] = GetRandomNumber(nMaxR,nMinR);
							MagicTabR.GetInteger(Item[nLindexID].GetItemParam()->nRGeneratorLevel[i],8,0,&nMaxR);
							MagicTabR.GetInteger(Item[nLindexID].GetItemParam()->nRGeneratorLevel[i],9,0,&nMinR);
							//sMAR[i].nValue[1] = nMaxR + ((nMinR - nMaxR) * nLuck / 10);
							sMAR[i].nValue[1] = GetRandomNumber(nMaxR,nMinR);

							MagicTabR.GetInteger(Item[nLindexID].GetItemParam()->nRGeneratorLevel[i],10,0,&nMaxR);
							MagicTabR.GetInteger(Item[nLindexID].GetItemParam()->nRGeneratorLevel[i],11,0,&nMinR);
							//sMAR[i].nValue[2] = nMaxR + ((nMinR - nMaxR) * nLuck / 10);
							sMAR[i].nValue[2] = GetRandomNumber(nMaxR,nMinR);

							inPont++;
							continue;
						}
						sMAR[i].nAttribType = 0;
						sMAR[i].nValue[0]   = 0;
						sMAR[i].nValue[1]   = 0;
						sMAR[i].nValue[2]   = 0;
						//  bEC=TRUE;
					} 		
					MagicTabR.Clear();
						   
					Item[nLindexID].SetAttrib_RON(sMAR);          //设置熔炼魔法属
					Item[nLindexID].SetIsWhere(inPont);           //设置辨识成功!
                    Item[nLindexID].SetRPoint(inPont); 

					Item[nLindexID].EnChance(nEnChance);
					Item[nLindexID].EnChanceRong(nEnChance);
					//Item[nLindexID].EnChanceBao(nEnChance);

					int nTime = 1;
		                g_GameSetTing.GetInteger("ServerConfig","BianShiTime",1,&nTime);

					time_t rawtime;      //定义一个long 型存放秒数
			        struct tm * timeinfo;	
			        time (&rawtime);     //距离现在的时间（秒）
			        timeinfo = localtime(&rawtime);  //把从1970-1-1零点零分到当前时间系统所偏移的秒数时间转换为本地时间
			        int	bYear=0,bMonth=0,bDay=0,bHour=0,bMin=0;			
			        bYear   = timeinfo->tm_year+1900;
			        bMonth  = timeinfo->tm_mon+1;	  //当月
			        bDay    = timeinfo->tm_mday;
			        bHour   = timeinfo->tm_hour+nTime;
	                bMin    = timeinfo->tm_min;

                    Item[nLindexID].SetTime(bYear,bMonth,bDay,bHour,bMin);
                     
					//if (CheckCanPlaceInEquipment(Item[nLindexID].GetWidth(), Item[nLindexID].GetHeight(), &x, &y))
					{//开始同步属性 检测包袱
						ITEM_SYNC	sItem;
                        memset(&sItem, 0, sizeof(sItem));
						sItem.ProtocolType   = s2c_syncequipitem;
						sItem.m_ID           = Item[nLindexID].GetID();
						sItem.m_SyncType     = 2;
						//同步魔法属性
						for (i = 0; i < 6; ++i)
						{
							//if (Item[nLindexID].GetItemParam()->nRGeneratorLevel[i]>0)
							//	IsKong++;

							sItem.m_RongMagicLev[i] = Item[nLindexID].GetItemParam()->nRGeneratorLevel[i];
//							sItem.m_BaoMagicLev[i]  = Item[nLindexID].GetItemParam()->nBGeneratorLevel[i];
						}

						sItem.m_Luck                = Item[nLindexID].GetItemParam()->nLuck;
						sItem.m_Rongpiont           = inPont;//Item[nLindexID].IsRong(); //熔炼
						sItem.m_IsWhere             = inPont;//Item[nLindexID].GetIsWhere();
                        sItem.m_IsCanUse            = 1;
						sItem.m_StackNum            = Item[nLindexID].GetStackNum();
						sItem.m_Series              = Item[nLindexID].GetSeries();
						sItem.m_IsBang              = Item[nLindexID].GetIsBang();
	                    sItem.m_IsKuaiJie           = Item[nLindexID].GetIsKuaiJie();
						sItem.m_EnChance            = Item[nLindexID].GetEnChance();
						sItem.m_RandomSeed          = nItemRandSeed;
						sItem.m_Time.bYear          = bYear;
						sItem.m_Time.bMonth         = bMonth;
						sItem.m_Time.bDay           = bDay;
						sItem.m_Time.bHour          = bHour;
						sItem.m_Time.bMin           = bMin;
						sItem.m_Durability          = Item[nLindexID].GetDurability();
                        
			            g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sItem, sizeof(ITEM_SYNC));

					}

					//if (IsKong>0)
						msgshow("提示:属性辨识成功!恭喜!恭喜!");
					   //else
					//	msgshow("提示:提示:属性辨识成功!但属性为空!");
					return nLindexID;
#endif 
				}
					return 0;
}

#ifdef _SERVER
void KItemList::SynPlayerItem(int nItemidx)
{
	ITEM_SYNC	sItem;
	memset(&sItem, 0, sizeof(sItem));
	sItem.ProtocolType   = s2c_syncequipitem;
	sItem.m_ID           = Item[nItemidx].GetID();
	sItem.m_SyncType     = 2;
	//同步魔法属性
	int i;

	for (i = 0; i < 6; ++i)
	{
		sItem.m_RongMagicLev[i] = Item[nItemidx].GetItemParam()->nRGeneratorLevel[i];
//		sItem.m_BaoMagicLev[i]  = Item[nItemidx].GetItemParam()->nBGeneratorLevel[i];
	}
/*
    for (i = 0; i < 7; ++i)
	{
		sItem.m_JBLevel[i]      = Item[nItemidx].GetItemParam()->nJGeneratorLevel[i];
	}
	*/	
	sItem.m_Point               = Item[nItemidx].GetPoint();

	sItem.m_Luck                = Item[nItemidx].GetItemParam()->nLuck;

	sItem.m_EnChance            = Item[nItemidx].GetEnChance();

	sItem.m_RandomSeed          = Item[nItemidx].GetItemParam()->uRandomSeed;

	g_RandomSeed(sItem.m_RandomSeed);

	sItem.m_Rongpiont           = Item[nItemidx].IsRong(); //熔炼
	sItem.m_IsWhere             = Item[nItemidx].GetIsWhere();
	sItem.m_Series              = Item[nItemidx].GetSeries();
	sItem.m_UseMap              = Item[nItemidx].GetUseMapIdx();
	sItem.m_UseKind             = Item[nItemidx].GetUseKind();
	sItem.m_Res                 = Item[nItemidx].GetItemResIdx();
	sItem.m_IsBang              = Item[nItemidx].GetIsBang();
	sItem.m_IsKuaiJie           = Item[nItemidx].GetIsKuaiJie();
    sItem.m_IsCanUse            = Item[nItemidx].GetIsCanUse();
	sItem.m_Time.bYear          = Item[nItemidx].GetTime()->bYear;
	sItem.m_Time.bMonth         = Item[nItemidx].GetTime()->bMonth;
	sItem.m_Time.bDay           = Item[nItemidx].GetTime()->bDay;
	sItem.m_Time.bHour          = Item[nItemidx].GetTime()->bHour;
	sItem.m_Time.bMin           = Item[nItemidx].GetTime()->bMin;
	sItem.m_LockItem			= *Item[nItemidx].GetLockItem();
	sItem.m_TradePrice          = Item[nItemidx].GetTradePrice();
//	printf("GetTradePrice: %d, SynPlayerIten\n", sItem.m_TradePrice);
	sItem.m_Durability          = Item[nItemidx].GetDurability();  //当前持久
//	sItem.m_EnChance            = Item[nItemidx].GetEnChance();
    sItem.m_StackNum            = Item[nItemidx].GetStackNum();
	sItem.m_IsMagic             = Item[nItemidx].GetIsMagic();     //是否有魔法属性
	sItem.m_SkillType           = Item[nItemidx].GetSkillType();
	sItem.m_MagicID             = Item[nItemidx].GetMagicID();

    ZeroMemory(sItem.m_ItmeInfo,sizeof(sItem.m_ItmeInfo));

	if (Item[nItemidx].GetSItmeInfo() && strstr(Item[nItemidx].GetSItmeInfo(),".lua"))//strrchr
	{
		KLuaScript * pScript = (KLuaScript*)g_GetScript(Item[nItemidx].GetSItmeInfo());//g_FileName2Id(
		if (pScript)
		{
			int nSafeIndex = 0;
		    nSafeIndex=pScript->SafeCallBegin();
			//执行这个脚本的函数
			try
			{
			   pScript->CallFunction("GetItemDescription",1,"sdddddds", Item[nItemidx].GetName(),Item[nItemidx].GetItemBaseMagic(0),Item[nItemidx].GetItemBaseMagic(1),Item[nItemidx].GetItemBaseMagic(2),Item[nItemidx].GetItemBaseMagic(3),Item[nItemidx].GetItemBaseMagic(4),Item[nItemidx].GetItemBaseMagic(5),"");
			   const char * szType = lua_typename(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState));
			   if (Lua_IsNumber(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState)) == 1)
			   { //Lua_PushNumber
			  	  int nResult = (int)Lua_ValueToNumber(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState));
				  _snprintf(sItem.m_ItmeInfo,sizeof(sItem.m_ItmeInfo)-1, "%d", nResult);  // int 类型
				  //sprintf(sItem.m_ItmeInfo,"%d",nResult);
			   }  
			   else if (Lua_IsString(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState)) == 1)  // 字符串类型
			   {  
				  snprintf(sItem.m_ItmeInfo, sizeof(sItem.m_ItmeInfo), "%s", (char *)Lua_ValueToString(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState)));
				   //sprintf(sItem.m_ItmeInfo,(char *)Lua_ValueToString(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState)));
			   }    
			   else
			      _snprintf(sItem.m_ItmeInfo,sizeof(sItem.m_ItmeInfo)-1,"<color=Red>数据错误,请联系GM处理!<color>");
				  //sprintf(sItem.m_ItmeInfo,"<color=Red>数据错误,请联系GM处理!<color>");
			}  
			catch (...)
			{
				  _snprintf(sItem.m_ItmeInfo,sizeof(sItem.m_ItmeInfo)-1,"<color=Red>数据错误,请联系GM处理!<color>");
				  //sprintf(sItem.m_ItmeInfo,"<color=Red>数据错误,请联系GM处理!<color>");
			}
			
			nSafeIndex=pScript->SafeCallBegin();
			pScript->SafeCallEnd(nSafeIndex);
		}
		else
		{//脚本不存在
			_snprintf(sItem.m_ItmeInfo,sizeof(sItem.m_ItmeInfo)-1,"暂无数据");
			//sprintf(sItem.m_ItmeInfo,"暂无数据");
		}	
		
		//Item[nIdx].SetSItmeInfo(sItem.m_ItmeInfo);
	}

     if (g_pServer)
	     g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sItem, sizeof(ITEM_SYNC));
}

/*
				ITEM_SYNC	sItem;
				memset(&sItem, 0, sizeof(sItem));
				sItem.ProtocolType = s2c_syncitem;
				sItem.m_Point = Item[m_Hand].GetPoint();
				sItem.m_Rongpiont = Item[m_Hand].IsRong(); //熔炼
				sItem.m_GoldId = Item[m_Hand].GetGoldId();
				sItem.m_IsPlatima    = Item[m_Hand].GetIsPlatima();
				sItem.m_UseMap       = Item[m_Hand].GetUseMapIdx();
				sItem.m_UseKind		 = Item[m_Hand].GetUseKind();
				sItem.m_EnChance = Item[m_Hand].GetEnChance();
				sItem.m_StackNum = Item[m_Hand].GetStackNum();
				sItem.m_ID = Item[m_Hand].GetID();
				sItem.m_Genre = Item[m_Hand].GetGenre();
				sItem.m_Detail = Item[m_Hand].GetDetailType();
				sItem.m_Particur = Item[m_Hand].GetParticular();
				sItem.m_Series = Item[m_Hand].GetSeries();
				sItem.m_Level = Item[m_Hand].GetLevel();
				sItem.m_Res   = Item[m_Hand].GetItemResIdx();
				sItem.m_Luck = Item[m_Hand].m_GeneratorParam.nLuck;
				sItem.m_btPlace = pos_trade1;
				sItem.m_btX = DesPos->nX;
				sItem.m_btY = DesPos->nY;
				sItem.m_Time.bYear = Item[m_Hand].GetTime()->bYear;
				sItem.m_Time.bMonth = Item[m_Hand].GetTime()->bMonth;
				sItem.m_Time.bDay = Item[m_Hand].GetTime()->bDay;
				sItem.m_Time.bHour = Item[m_Hand].GetTime()->bHour;
				sItem.m_Time.bMin = Item[m_Hand].GetTime()->bMin;
				sItem.m_IsBang     = Item[m_Hand].GetIsBang();       //是否绑定
				sItem.m_IsKuaiJie    = Item[m_Hand].GetIsKuaiJie();  //是否快捷栏
                sItem.m_IsMagic      =Item[m_Hand].GetIsMagic();     //是否有魔法属性
				sItem.m_SkillType    =Item[m_Hand].GetSkillType();
				sItem.m_MagicID      = Item[m_Hand].GetMagicID();

				sItem.m_IsWhere      = Item[m_Hand].GetIsWhere();
	            sItem.m_IsCanUse     = Item[m_Hand].GetIsCanUse();
                sItem.m_IsLoign      = 0;

				sItem.m_RandomSeed = Item[m_Hand].m_GeneratorParam.uRandomSeed;
				sItem.m_Version = Item[m_Hand].m_GeneratorParam.nVersion;
				sItem.m_Durability = Item[m_Hand].GetDurability();

				//_snprintf(sItem.m_WonName,sizeof(sItem.m_WonName)-1,"%s",Item[m_Hand].GetWonName());  //所有者
				sprintf(sItem.m_WonName,Item[m_Hand].GetWonName());  //所有者

				for (int j = 0; j < 6; ++j)
				{
					sItem.m_MagicLevel[j]   = Item[m_Hand].m_GeneratorParam.nGeneratorLevel[j];
		            sItem.m_RongMagicLev[j] = Item[m_Hand].m_GeneratorParam.nRGeneratorLevel[j];
//					sItem.m_BaoMagicLev[j]  = Item[m_Hand].m_GeneratorParam.nBGeneratorLevel[j];
				}
				for (int i = 0; i < 7; ++i)
				{
					sItem.m_JBLevel[i]      = Item[m_Hand].m_GeneratorParam.nJGeneratorLevel[i];
				}

				g_pServer->PackDataToClient(Player[Player[m_PlayerIdx].m_cTrade.m_nTradeDest].m_nNetConnectIdx, (BYTE*)&sItem, sizeof(ITEM_SYNC));
*/
//Dong bo item
void KItemList::SyncItemMagicAttrib(int nIdx)
{
	ITEM_SYNC	sItem;
	memset(&sItem, 0, sizeof(sItem)); 
	sItem.ProtocolType = s2c_syncitem;

//	if (StackNum>1)
//		Item[nIdx].SetStackNum(StackNum);

	sItem.m_Point        = Item[nIdx].GetPoint();
	sItem.m_Rongpiont    = Item[nIdx].IsRong(); //熔炼
	sItem.m_EnChance     = Item[nIdx].GetEnChance();
	sItem.m_GoldId       = Item[nIdx].GetGoldId();
	sItem.m_IsPlatima    = Item[nIdx].GetIsPlatima();
	sItem.m_UseMap       = Item[nIdx].GetUseMapIdx();
	sItem.m_UseKind		 = Item[nIdx].GetUseKind();
	sItem.m_StackNum     = Item[nIdx].GetStackNum();
	sItem.m_ID           = Item[nIdx].GetID();   //物品的 dwID
	sItem.m_Genre        = Item[nIdx].GetGenre();
	sItem.m_Detail       = Item[nIdx].GetDetailType();
	sItem.m_Particur     = Item[nIdx].GetParticular();
	sItem.m_Series       = Item[nIdx].GetSeries();
	sItem.m_Level        = Item[nIdx].GetLevel();
	sItem.m_Res          = Item[nIdx].GetItemResIdx();
	sItem.m_Luck         = Item[nIdx].m_GeneratorParam.nLuck;
	sItem.m_btPlace      = m_Items[nIdx].nPlace; //同步目标容器
	sItem.m_btX          = m_Items[nIdx].nX;     //同步位置
	sItem.m_btY          = m_Items[nIdx].nY;
	sItem.m_Time.bYear   = Item[nIdx].GetTime()->bYear; 
	sItem.m_Time.bMonth  = Item[nIdx].GetTime()->bMonth;
	sItem.m_Time.bDay    = Item[nIdx].GetTime()->bDay;
	sItem.m_Time.bHour   = Item[nIdx].GetTime()->bHour;
	sItem.m_Time.bMin    = Item[nIdx].GetTime()->bMin;
	sItem.m_LockItem	 = *Item[nIdx].GetLockItem();
    sItem.m_IsBang       = Item[nIdx].GetIsBang();      //是否绑定

	if (Item[nIdx].GetTradePrice() <= 0 || Item[nIdx].GetTradePrice() > 999999999 || Item[nIdx].GetLockItem()->IsLockItem())
		sItem.m_TradePrice	 = 0;
	else
		sItem.m_TradePrice	 = Item[nIdx].GetTradePrice();

	sItem.m_IsKuaiJie    = Item[nIdx].GetIsKuaiJie();   //是否快捷栏
	sItem.m_IsMagic      = Item[nIdx].GetIsMagic();     //是否有魔法属性
	sItem.m_SkillType    = Item[nIdx].GetSkillType();
	sItem.m_MagicID      = Item[nIdx].GetMagicID();
    sItem.m_IsWhere      = Item[nIdx].GetIsWhere();
	sItem.m_IsCanUse     = Item[nIdx].GetIsCanUse();
    sItem.m_IsLoign      = 1;
	sItem.m_RandomSeed = Item[nIdx].m_GeneratorParam.uRandomSeed;
	sItem.m_Version    = Item[nIdx].m_GeneratorParam.nVersion;
	sItem.m_Durability = Item[nIdx].GetDurability();
    snprintf(sItem.m_WonName, sizeof(sItem.m_WonName), "%s", Item[nIdx].GetWonName());  //所有者
   //printf("包袱 m_RandomSeed :%u m_EnChance:%d \n",sItem.m_RandomSeed,sItem.m_EnChance);
    int j;
	for (j = 0; j < 6; ++j)//同步魔法属性
	{
		sItem.m_MagicLevel[j] = Item[nIdx].m_GeneratorParam.nGeneratorLevel[j]; 
		sItem.m_RongMagicLev[j] = Item[nIdx].m_GeneratorParam.nRGeneratorLevel[j];
//		sItem.m_BaoMagicLev[j] = Item[nIdx].m_GeneratorParam.nBGeneratorLevel[j];

		if (sItem.m_Point==8)
		{
			//sItem.m_MagInfo[j].nMagType=Item[nIdx].GetAttribType(j);
			//sItem.m_MagInfo[j].nMagVala=Item[nIdx].GetAttribTypeVala(j);
			//sItem.m_MagInfo[j].nMagValb=Item[nIdx].GetAttribTypeValb(j);
			//sItem.m_MagInfo[j].nMagValc=Item[nIdx].GetAttribTypeValc(j);
		}
	}

	for (j = 0; j < 7; ++j)
	{
		sItem.m_JBLevel[j]      = Item[nIdx].m_GeneratorParam.nJGeneratorLevel[j];
	}

	if (g_pServer)
		g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sItem, sizeof(ITEM_SYNC));
}





#endif 

//===============紫装纹钢=============================
 
int KItemList::PurpleWenGang(int nLindexID,int nBaoshiID)
{
				if (nLindexID && Item[nLindexID].GetGenre() == item_equip && (Item[nLindexID].GetGoldId() > 0 || Item[nLindexID].IsPurple()>0 || Item[nLindexID].IsBlue()))
				{
#ifdef _SERVER
					int nItemClass		= Item[nLindexID].GetGenre();
					int nDetailType		= Item[nLindexID].GetDetailType();
					int nParticularType	= Item[nLindexID].GetParticular();
					int nLevel			= Item[nLindexID].GetLevel();
					int nSeries			= Item[nLindexID].GetSeries();
					int nLuck			= Item[nLindexID].GetItemParam()->nLuck;
					int nEnChance		= Item[nLindexID].GetEnChance();
					int nGoldId			= Item[nLindexID].GetGoldId();	
					int nVersion		= Item[nLindexID].GetItemParam()->nVersion;
					int	nGndrom			= Item[nLindexID].GetItemParam()->uRandomSeed;
					int nPoint			= Item[nLindexID].GetPoint();   //紫装IsPurple
					int nRPoint			= Item[nLindexID].IsRong();
					int nBlue           = Item[nLindexID].IsBlue();
					int nMlevel         = Item[nBaoshiID].GetLevel();      //纹钢的等级
					int nWenPin         = Item[nBaoshiID].GetWengangPin(); //纹钢的品质
					

                    if (Item[nLindexID].GetIsCanUse()==0)
					{	  
                        msgshow("提示:该装备不能纹钢!");	
						return 0;
					} 

					int nRItemLevel[6];

					if (nRPoint<=0)
					{
						nRPoint=GetRandomNumber(2,6);			
					}

					if (nRPoint>6)
					{
						nRPoint=GetRandomNumber(2,6);			
					} 
                    
					if (Item[nBaoshiID].GetGenre()!=item_fusion)
					{
					   	msgshow("提示:放的材料不是(新)纹钢!");	
						return 0;
					}
                    
				   int nMagic=0,i;

	               for (i = 0;i < 6;++i)
		           {//纹钢部位检测
		             if (Item[nLindexID].GetDetailType()==Item[nBaoshiID].GetAttribTypeVala(i))
			         {
			            nMagic++;
			         }
		           }

		          if (nMagic<= 0) 
				  { //可出现该属性的装备类型
		            msgshow("提示:此纹钢与装备类型不符合!");
		            return 0;
				  }  
/////////////////////////////////////////////////////////////////////////////////////////////////////
        //确定成功的机率
					int nVal=0,nRate=0;
                    //--------------概率检测--------------------------------------------
					g_GameSetTing.GetInteger2("ServerConfig","PurpleWenGangRank",&nVal,&nRate);
					//运气石的等级
					if (g_Random(nVal) < g_Random(nRate))
					{
                        Remove(nBaoshiID);
						ItemSet.Remove(nBaoshiID); //删除材料
						msgshow("提示:紫装纹钢失败!再接再励!");	
						return 0;
					}
                   //-------------------------------------------------------------------
		//可溶链的 装备位置

		int nRow;
            nRow = Item[nBaoshiID].GetSkillType();  //获取改属性的行号

		if (nRow<=0)
		{
		   msgshow("提示:该属性纹钢已经失效!");
           return 0; 
		}

		if (nLuck < 0) 
			nLuck = 0;
		if (nLuck > 10) 
			nLuck = 10;


/*		int nMinRow,nMgIdx,IsChonWenGang=1,nSkilType;
		tabMagic.GetInteger(nRow+1,"MAGATTRLVL_START",0,&nMinRow);//在魔法属性表的行数
		tabMagic.GetInteger(nRow+1,"MAGIC_ID",0,&nMgIdx);         //属性ID的
		tabMagic.GetInteger(nRow+1,"MAGIC_CHONG",1,&IsChonWenGang);  //是否可以重复纹钢
		tabMagic.GetInteger(nRow+1,"MAGIC_TTPE",0,&nSkilType); 
*/
       
		KTabFile tabMagic;
		         tabMagic.Load(TABFILE_MAGICAGOLD_PATH);

		int nMgIdx=0;
		
		tabMagic.GetInteger(nRow,5,0,&nMgIdx);	//获取该行的 魔法编号

		//已经镶套了的相同属性的不能镶套
		 for (i = 0;i < 6;++i)
		 { 

			 if(Item[nLindexID].GetAttribType(i) == nMgIdx)
			 {
				 msgshow("提示:该装备已经有相同的属性!");
				 tabMagic.Clear();
				 return 0; 
			}

		    if(Item[nLindexID].GetRongAttribType(i) == nMgIdx)
			{
			   msgshow("提示:该装备已经纹钢了同类型纹钢!");
			   tabMagic.Clear();
               return 0; 
			}
		 } 
		 
	

		int nMun=0;

		for (i = 0;i <nRPoint;++i)//有多少个纹钢孔
		{
		    if (nMun == 0 && Item[nLindexID].GetRongAttribType(i) <= 0)  //位置相同 并且没有属性的空位置
			{
				Item[nLindexID].GetItemParam()->nRGeneratorLevel[i]=nRow;//设定属性的行数
				nMun++;
				continue;
			}
		}

		if 	(nMun==0)
		{
			msgshow("提示:该装备已经不能再纹钢了!");
            return 0;
		}

		Remove(nBaoshiID);       //删除纹钢材料
		ItemSet.Remove(nBaoshiID);

        ///=================以下为镶套纹钢属性=====================================================
		KItemNormalAttrib	sMAR[6];// 魔法
		ZeroMemory(sMAR,sizeof(sMAR)); //分配内存
	 		
		int nTypeR,nMaxR,nMinR;
		
		for (i = 0;i < 6;++i)
		{ 
			if (Item[nLindexID].GetItemParam()->nRGeneratorLevel[i] > 0 && i < nRPoint)
			{ 
				tabMagic.GetInteger(Item[nLindexID].GetItemParam()->nRGeneratorLevel[i],5,0,&nTypeR);
				tabMagic.GetInteger(Item[nLindexID].GetItemParam()->nRGeneratorLevel[i],6,0,&nMaxR);
				tabMagic.GetInteger(Item[nLindexID].GetItemParam()->nRGeneratorLevel[i],7,0,&nMinR);
				sMAR[i].nAttribType = nTypeR;  //属性类型
				sMAR[i].nValue[0] = nMaxR + ((nMinR - nMaxR) * nLuck / 10);
				tabMagic.GetInteger(Item[nLindexID].GetItemParam()->nRGeneratorLevel[i],8,0,&nMaxR);
				tabMagic.GetInteger(Item[nLindexID].GetItemParam()->nRGeneratorLevel[i],9,0,&nMinR);
				sMAR[i].nValue[1] = nMaxR + ((nMinR - nMaxR) * nLuck / 10);
				tabMagic.GetInteger(Item[nLindexID].GetItemParam()->nRGeneratorLevel[i],10,0,&nMaxR);
				tabMagic.GetInteger(Item[nLindexID].GetItemParam()->nRGeneratorLevel[i],11,0,&nMinR);
				sMAR[i].nValue[2] = nMaxR + ((nMinR - nMaxR) * nLuck / 10);
				
				continue;
			}
			sMAR[i].nAttribType = 0;
			sMAR[i].nValue[0]   = 0;
			sMAR[i].nValue[1]   = 0;
			sMAR[i].nValue[2]   = 0;
			//  bEC=TRUE;
		} 	
		tabMagic.Clear();	   
	    Item[nLindexID].SetAttrib_RON(sMAR);   //设置熔炼魔法属
        Item[nLindexID].SetIsWhere(0);         // 0 为纹钢
        Item[nLindexID].SetRPoint(nRPoint);    
		
		Item[nLindexID].EnChance(nEnChance);
		Item[nLindexID].EnChanceRong(nEnChance);
		//Item[nLindexID].EnChanceBao(nEnChance);

		msgshow("提示:紫装纹钢成功!恭喜!恭喜!");
		//=========开始同步属性=========
		 //Item[nLindexID].GetIsCanUse();
		 //sItem.m_IsCanUse          = 1;

		 Item[nLindexID].SetIsCanUse(1);

		 SynPlayerItem(nLindexID);
		//==============================
	    
        return 1;
#endif 
				}
		return 0;
}

//================蓝装装备纹钢=========================
int KItemList::BlueWenGang(int nLindexID,int nBaoshiID)
{
				if (nLindexID && Item[nLindexID].GetGenre() == item_equip && (Item[nLindexID].GetGoldId() > 0 || Item[nLindexID].IsPurple()>0 || Item[nLindexID].IsBlue()))
				{
#ifdef _SERVER
					int nItemClass		= Item[nLindexID].GetGenre();
					int nDetailType		= Item[nLindexID].GetDetailType();
					int nParticularType	= Item[nLindexID].GetParticular();
					int nLevel			= Item[nLindexID].GetLevel();
					int nSeries			= Item[nLindexID].GetSeries();
					int nLuck			= Item[nLindexID].GetItemParam()->nLuck;
					int nEnChance		= Item[nLindexID].GetEnChance();
					int nGoldId			= Item[nLindexID].GetGoldId();	
					int nVersion		= Item[nLindexID].GetItemParam()->nVersion;
					UINT	nGndrom			= Item[nLindexID].GetItemParam()->uRandomSeed;
					int nPoint			= Item[nLindexID].GetPoint();   //紫装IsPurple
					int nRPoint			= Item[nLindexID].IsRong();
					int nBlue           = Item[nLindexID].IsBlue();
					int nMlevel         = Item[nBaoshiID].GetLevel();      //纹钢的等级
					int nWenPin         = Item[nBaoshiID].GetWengangPin(); //纹钢的品质
					

                  if (Item[nLindexID].GetIsCanUse()==0)
				  {
                        msgshow("提示:该装备不能纹钢!");	
						return 0;
				  }

					int nRItemLevel[6];

					if (nRPoint<=0)
					{
						nRPoint=GetRandomNumber(2,6);			
					}

					if (nRPoint>6)
					{
						nRPoint=GetRandomNumber(2,6);			
					} 
                    
					if (Item[nBaoshiID].GetGenre()!=item_fusion)
					{
					   	msgshow("提示:放的材料不是(新)纹钢!");	
						return 0;
					}
                    
				int nMagic=0,i;

	               for (i = 0;i < 6;++i)
		           {
		             if (Item[nLindexID].GetDetailType()==Item[nBaoshiID].GetAttribTypeVala(i))
			         {
			           nMagic++;
			         }
		           }

		    if (nMagic<= 0) 
		    {//可出现该属性的装备类型
		        msgshow("提示:此纹钢与装备类型不符合!");
		        return 0;
	        }
/////////////////////////////////////////////////////////////////////////////////////////////////////
        //确定成功的机率
					int nVal=0,nRate=0;
                    //--------------概率检测--------------------------------------------
					g_GameSetTing.GetInteger2("ServerConfig","BlueWenGangRank",&nVal,&nRate);
					//运气石的等级
					if (g_Random(nVal) < g_Random(nRate))
					{
                        Remove(nBaoshiID);
						ItemSet.Remove(nBaoshiID); //删除材料
						msgshow("提示:蓝装纹钢失败!再接再励!");	
						return 0;
					}
                   //-------------------------------------------------------------------
		//可溶链的 装备位置

		int nRow;
            nRow = Item[nBaoshiID].GetSkillType();  //获取改属性的行号

		if (nRow<=0)
		{
		   msgshow("提示:该属性纹钢已经失效!");
           return 0; 
		}

		if (nLuck < 0) 
			nLuck = 0;
		if (nLuck > 10) 
			nLuck = 10;


/*		int nMinRow,nMgIdx,IsChonWenGang=1,nSkilType;
		tabMagic.GetInteger(nRow+1,"MAGATTRLVL_START",0,&nMinRow);//在魔法属性表的行数
		tabMagic.GetInteger(nRow+1,"MAGIC_ID",0,&nMgIdx);         //属性ID的
		tabMagic.GetInteger(nRow+1,"MAGIC_CHONG",1,&IsChonWenGang);  //是否可以重复纹钢
		tabMagic.GetInteger(nRow+1,"MAGIC_TTPE",0,&nSkilType); 
*/
       
		KTabFile tabMagic;
		         tabMagic.Load(TABFILE_MAGICAGOLD_PATH);
		int nMgIdx=0;
		
		tabMagic.GetInteger(nRow,5,0,&nMgIdx);

		//已经镶套了的相同属性的不能镶套
		 for (i = 0;i < 6;++i)
		 { 

			if(Item[nLindexID].GetAttribType(i) == nMgIdx)
			{
				 msgshow("提示:该装备已经有相同的属性!");
				 tabMagic.Clear();
				 return 0; 
			}

		    if(Item[nLindexID].GetRongAttribType(i) == nMgIdx)
			{
			   msgshow("提示:该装备已经纹钢了同类型纹钢!");
			   tabMagic.Clear();
               return 0; 
			}
		 } 
		 
        //同步熔炼属性值 和 判断 属性石头的位置
		int nMun=0;

		for (i = 0;i <nRPoint;++i)
		{
		    if (nMun == 0 && Item[nLindexID].GetRongAttribType(i) <= 0)  //位置相同 并且没有属性的空位置
			{
				Item[nLindexID].GetItemParam()->nRGeneratorLevel[i]=nRow;//设定属性的行数
				nMun++;
				continue;
			}
		}

		if 	(nMun==0)
		{
			msgshow("提示:该装备已经不能再纹钢了!");
            return 0;
		}

		Remove(nBaoshiID);  //删除材料
		ItemSet.Remove(nBaoshiID);

        ///=================以下为镶套纹钢属性=====================================================
		KItemNormalAttrib	sMAR[6];// 魔法
		ZeroMemory(sMAR,sizeof(sMAR)); //分配内存
		//KTabFile MagicTabR;
		//MagicTabR.Load(TABFILE_MAGICAGOLD_PATH);  //熔炼蓝装备魔法属性文件	 
		
		int nTypeR,nMaxR,nMinR;
		

		g_RandomSeed(nGndrom);

		for (i = 0;i < 6;++i)
		{ 
			if (Item[nLindexID].GetItemParam()->nRGeneratorLevel[i] > 0 && i < nRPoint)
			{ 
				tabMagic.GetInteger(Item[nLindexID].GetItemParam()->nRGeneratorLevel[i],5,0,&nTypeR);
				tabMagic.GetInteger(Item[nLindexID].GetItemParam()->nRGeneratorLevel[i],6,0,&nMaxR);
				tabMagic.GetInteger(Item[nLindexID].GetItemParam()->nRGeneratorLevel[i],7,0,&nMinR);
				sMAR[i].nAttribType = nTypeR;  //属性类型
				//sMAR[i].nValue[0] = nMaxR + ((nMinR - nMaxR) * nLuck / 10);
				 sMAR[i].nValue[0] = GetRandomNumber(nMaxR,nMinR);
				tabMagic.GetInteger(Item[nLindexID].GetItemParam()->nRGeneratorLevel[i],8,0,&nMaxR);
				tabMagic.GetInteger(Item[nLindexID].GetItemParam()->nRGeneratorLevel[i],9,0,&nMinR);
				//sMAR[i].nValue[1] = nMaxR + ((nMinR - nMaxR) * nLuck / 10);
				sMAR[i].nValue[1] = GetRandomNumber(nMaxR,nMinR);

				tabMagic.GetInteger(Item[nLindexID].GetItemParam()->nRGeneratorLevel[i],10,0,&nMaxR);
				tabMagic.GetInteger(Item[nLindexID].GetItemParam()->nRGeneratorLevel[i],11,0,&nMinR);
				//sMAR[i].nValue[2] = nMaxR + ((nMinR - nMaxR) * nLuck / 10);
				sMAR[i].nValue[2] = GetRandomNumber(nMaxR,nMinR);

				
				continue;
			}
			sMAR[i].nAttribType = 0;
			sMAR[i].nValue[0]   = 0;
			sMAR[i].nValue[1]   = 0;
			sMAR[i].nValue[2]   = 0;
			//  bEC=TRUE;
		} 	
		tabMagic.Clear();	   
	    Item[nLindexID].SetAttrib_RON(sMAR);   //设置熔炼魔法属
        Item[nLindexID].SetIsWhere(0);         // 0 为纹钢
        Item[nLindexID].SetRPoint(nRPoint);    
		
		Item[nLindexID].EnChance(nEnChance);
		Item[nLindexID].EnChanceRong(nEnChance);
		//Item[nLindexID].EnChanceBao(nEnChance);

		msgshow("提示:蓝装纹钢成功!恭喜!恭喜!");
		//=========开始同步属性=========
		int	x, y;

		//if (CheckCanPlaceInEquipment(Item[nLindexID].GetWidth(), Item[nLindexID].GetHeight(), &x, &y))
		{
			ITEM_SYNC	sItem;
			memset(&sItem, 0, sizeof(sItem));
	        sItem.ProtocolType   = s2c_syncequipitem;
			sItem.m_ID           = Item[nLindexID].GetID();
            sItem.m_SyncType     = 1;
            //同步魔法属性
			for (i = 0; i < 6; ++i)
			{
			  sItem.m_RongMagicLev[i] = Item[nLindexID].GetItemParam()->nRGeneratorLevel[i];
//		      sItem.m_BaoMagicLev[i]  = Item[nLindexID].GetItemParam()->nBGeneratorLevel[i];
			}
			sItem.m_RandomSeed        = nGndrom;   
            sItem.m_Luck              = Item[nLindexID].GetItemParam()->nLuck;
            sItem.m_Rongpiont         = Item[nLindexID].IsRong(); //熔炼
            sItem.m_IsWhere           = Item[nLindexID].GetIsWhere();
			sItem.m_Series            = Item[nLindexID].GetSeries();
			sItem.m_IsBang            = Item[nLindexID].GetIsBang();
	        sItem.m_IsKuaiJie         = Item[nLindexID].GetIsKuaiJie();
			sItem.m_EnChance          = Item[nLindexID].GetEnChance();
            sItem.m_IsCanUse          = 1;
			sItem.m_Durability        = Item[nLindexID].GetDurability();

			g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sItem, sizeof(ITEM_SYNC));
		}
		//==============================
	    
        return 1;
#endif 
				}
		return 0;
}
//宝石灵合
int KItemList::LingHe(int nLindexID,int nBaoshiID)
{
/*			// 装备熔炼  ---熔炼晶石
		//	if (Item[nBaoshiID].GetGenre() == item_task && (Item[nBaoshiID].GetDetailType() >= 243 && Item[nBaoshiID].GetDetailType() <=371))
			{
				if (nLindexID && Item[nLindexID].GetGenre() == item_equip && (Item[nLindexID].GetGoldId() > 0 || Item[nLindexID].IsPurple()>0 || Item[nLindexID].IsBlue()))
				{
#ifdef _SERVER
					int nItemClass		= Item[nLindexID].GetGenre();
					int nDetailType		= Item[nLindexID].GetDetailType();
					int nParticularType	= Item[nLindexID].GetParticular();
					int nLevel			= Item[nLindexID].GetLevel();
					int nSeries			= Item[nLindexID].GetSeries();
					int nLuck			= Item[nLindexID].GetItemParam()->nLuck;
					int nEnChance		= Item[nLindexID].GetEnChance();
					int nGoldId			= Item[nLindexID].GetGoldId();	
					int nVersion		= Item[nLindexID].GetItemParam()->nVersion;
					int	nGndrom			= Item[nLindexID].GetItemParam()->uRandomSeed;
					int nPoint			= Item[nLindexID].GetPoint();   //紫装IsPurple
					int nRPoint			= Item[nLindexID].IsRong();
					int nMlevel         = Item[nBaoshiID].GetLevel();  //纹钢的等级
					int nBlue           = Item[nLindexID].IsBlue();
                   	
					int nItemLevel[6];
					int nRItemLevel[6];
					//int nBItemLevel[6];

					ZeroMemory(nItemLevel, sizeof(nItemLevel));
					ZeroMemory(nRItemLevel, sizeof(nRItemLevel));
					//同步魔法属性
					for (int i = 0; i < 6; ++i)
					{
						nItemLevel[i]  = Item[nLindexID].GetItemParam()->nGeneratorLevel[i];
						nRItemLevel[i] = Item[nLindexID].GetItemParam()->nRGeneratorLevel[i];
//						nBItemLevel[i] = Item[nLindexID].GetItemParam()->nBGeneratorLevel[i];
					}
/////////////////////////////////////////////////////////////////////////////////////////////////////
        //确定成功的机率
					int nVal=0,nRate=0;
                    //--------------概率检测--------------------------------------------
					g_GameSetTing.GetInteger2("ServerConfig","GoldLingHeRank",&nVal,&nRate);
					if (g_Random(nVal) < g_Random(nRate))
					{
                        Remove(nBaoshiID);
						ItemSet.Remove(nBaoshiID); //删除材料
						msgshow("提示:装备灵合失败!再接再励!");	
						return 0;
					}
                   //-------------------------------------------------------------------
		//可溶链的 装备位置
		KTabFile tabMagic;
		tabMagic.Load(TABFILE_MAGICALEVEL_PATH);
		int nRow;
        nRow = Item[nBaoshiID].GetSkillType();  //获取改属性的行号
		int nAddLevel =nMlevel;   //为纹钢的等级
		if (nLuck < 0) 
			nLuck = 0;
		if (nLuck > 10) 
			nLuck = 10;
		if (nAddLevel > 10) 
			nAddLevel = 10;

		int nMinRow,nMgIdx,IsChonWenGang=1,nSkilType;
		tabMagic.GetInteger(nRow+1,"MAGATTRLVL_START",0,&nMinRow);//在魔法属性表的行数
		tabMagic.GetInteger(nRow+1,"MAGIC_ID",0,&nMgIdx);         //属性ID的
		tabMagic.GetInteger(nRow+1,"MAGIC_CHONG",1,&IsChonWenGang);  //是否可以重复纹钢
		tabMagic.GetInteger(nRow+1,"MAGIC_TTPE",0,&nSkilType); 
		if (nSkilType!=1)
		{
		   msgshow("提示:该灵合宝石已经失效!");
           return 0; 
		}
		nMinRow +=  nAddLevel;  //产生属性的等级（品数）
		tabMagic.Clear();

		tabMagic.Load(TABFILE_MAGICATTRIB_PATH);
		int nMagic;
	    tabMagic.GetInteger(nMinRow,Item[nLindexID].GetGenre() + 13,0,&nMagic);
		if (nMagic > 1) 
		{//可出现该属性的装备类型
		   msgshow("提示:此灵合宝石与装备类型不符合!");
		   return 0;
		}
		tabMagic.Clear();
		//已经镶套了的相同属性的不能镶套
	if  (IsChonWenGang==0)
		 for (int p = 0;p < 6;++p)
		 { 
		    if(Item[nLindexID].GetRongAttribType(p) == nMgIdx)
			{
			   msgshow("提示:该黄金已经灵合了同类型宝石!");
               return 0; 
			}
		 }  
        //同步熔炼属性值 和 判断 属性石头的位置
		int nMun=0;
		for (int k = 0;k <6;++k)
		{
		    if (nMun == 0 && Item[nLindexID].GetBaoShiAttribType(k) <= 0)  //位置相同 并且没有属性的空位置
			{
				nBItemLevel[k] = nMinRow;   //设定属性行数
				nMun++;
				continue;
			}
//			    nBItemLevel[k] = Item[nLindexID].m_GeneratorParam.nBGeneratorLevel[k];
		}
		//char msg[64];
		//sprintf(msg,"灵合:(%d)(%d)(%d)(%d)(%d)(%d)",nBItemLevel[0],nBItemLevel[1],nBItemLevel[2],nBItemLevel[3],nBItemLevel[4],nBItemLevel[5]);
		//msgshow(msg);
		            int nStackNum = 1;
//////////////////////////////////////////////////////////////////////////////////////////////////////
					int	nIndex;			
					if (nGoldId>0)
					{//黄金的灵合
						nIndex = ItemSet.AddGold(nGoldId,nItemLevel,nSeries,nEnChance,nRPoint,0,0,0,0,0,nRItemLevel,nBItemLevel,nLevel);
					} 
					else
					{//其他紫装和蓝装的灵合
						nPoint=7;
						nIndex = ItemSet.AddRong(0,nSeries,nLevel,nLuck,nDetailType,nParticularType,nItemLevel,nRItemLevel,NULL,nVersion,nGndrom,1,nEnChance,nRPoint,nPoint);
					}	
					if (nIndex <= 0)
					{
						return 0;
					}	
					
					Remove(nBaoshiID);
					ItemSet.Remove(nBaoshiID);
					Remove(nLindexID);
					ItemSet.Remove(nLindexID);

					int		x, y;
					if (CheckCanPlaceInEquipment(Item[nIndex].GetWidth(), Item[nIndex].GetHeight(), &x, &y))
					{
						Add(nIndex, pos_equiproom, x, y);   //包袱
					}
					else
					{
                        msgshow("提示:包袱空间不足,前功尽弃啦!");
						Remove(nIndex);
					    ItemSet.Remove(nIndex);
					    return 0;
					}
                        msgshow("提示:宝石灵合成功!恭喜!恭喜!");

					return nIndex;
#endif 
				}
					return 0;
			}
			*/
			return 0;
}
//装备熔炼
int KItemList::RongLian(int nLindexID,int nBaoshiID)
{
			// 装备熔炼  ---熔炼晶石
			if (Item[nBaoshiID].GetGenre() == item_task && (Item[nBaoshiID].GetDetailType() >= 243 && Item[nBaoshiID].GetDetailType() <=371))
			{
				if (nLindexID && Item[nLindexID].GetGenre() == item_equip && (Item[nLindexID].GetGoldId() > 0 || Item[nLindexID].IsPurple()>0 || Item[nLindexID].IsBlue()))
				{
#ifdef _SERVER
					int nItemClass		= Item[nLindexID].GetGenre();
					int nDetailType		= Item[nLindexID].GetDetailType();
					int nParticularType	= Item[nLindexID].GetParticular();
					int nLevel			= Item[nLindexID].GetLevel();
					int nSeries			= Item[nLindexID].GetSeries();
					int nLuck			= Item[nLindexID].GetItemParam()->nLuck;
					int nEnChance		= Item[nLindexID].GetEnChance();
					int nGoldId			= Item[nLindexID].GetGoldId();	
					int nVersion		= Item[nLindexID].GetItemParam()->nVersion;
					UINT nGndrom			= Item[nLindexID].GetItemParam()->uRandomSeed;
					int nPoint			= Item[nLindexID].GetPoint();   //紫装IsPurple
					int nRPoint			= 0;// Item[nLindexID].IsRong();
					int nMlevel         = Item[nBaoshiID].GetLevel();   //提炼的等级
					if  (nMlevel >=1 && nMlevel <=6)
                        nRPoint=nMlevel;
                    else if (nMlevel >6)
                        nRPoint=6;

					int nItemLevel[6];
					int nRItemLevel[6];
					int nIsDaZhao=1;
					 //    nPoint=8;
					if (nEnChance >= 9)
					{
					    return 0;
					} 
					ZeroMemory(nItemLevel, sizeof(nItemLevel));
					ZeroMemory(nRItemLevel, sizeof(nRItemLevel));
                  //  KMagicAttrib sRoY[6];
					for (int i = 0; i < 6; ++i)
					{
						nItemLevel[i]  = Item[nLindexID].GetItemParam()->nGeneratorLevel[i];
						nRItemLevel[i] = 0;//Item[nLindexID].GetItemParam()->nRGeneratorLevel[i];
					//	sRoY[i].nAttribType = Item[nLindexID].GetAttribType(i);  //属性类型
					//	sRoY[i].nValue[0] = Item[nLindexID].GetAttribTypeVala(i);  
					//	sRoY[i].nValue[1] = Item[nLindexID].GetAttribTypeValb(i);
					//  sRoY[i].nValue[2] = Item[nLindexID].GetAttribTypeValc(i); 
					}
					int	nIndex;
					
					if (nGoldId)
					{
						nIndex = ItemSet.AddGold(nGoldId,nItemLevel,nSeries,nEnChance,nRPoint,0,0,0,0,0,nRItemLevel,NULL,nLevel,1,-1,-1,1,0,0,TRUE,nGndrom);
					} 
					else
					{
						//nIndex = ItemSet.AddOther(nItemClass,nSeries,nLevel,nLuck,nDetailType,nParticularType,nItemLevel,
						//	nVersion,nGndrom,1,nEnChance,nPoint);
						nIndex = ItemSet.AddRong(0,nSeries,nLevel,nLuck,nDetailType,nParticularType,nItemLevel,nRItemLevel,NULL,nVersion,nGndrom,1,nEnChance,nRPoint,nPoint,0,NULL,0,0,0,0,0,0,TRUE);
					}			
					if (nIndex <= 0)
					{
						return 0;
					}				
					Remove(nBaoshiID);
					ItemSet.Remove(nBaoshiID);
					Remove(nLindexID);
					ItemSet.Remove(nLindexID);
					int		x, y;
					if (CheckCanPlaceInEquipment(Item[nIndex].GetWidth(), Item[nIndex].GetHeight(), &x, &y))
					{
						Add(nIndex, pos_equiproom, x, y);   //包袱增加装备 同步客户端
					}
					return nIndex;
#endif
				}
					return 0;
			}
			return 0;
}
//服务器端产生物品
int KItemList::CilentAddItem(int nItemIdx)
{
			// 包袱强化  ---红狂星宝石
		//   if (Item[nItemIdx].GetGenre() == item_task && Item[nItemIdx].GetDetailType() == 238)
	      if (nItemIdx)
		  { 
			//	if (nItemIdx && Item[nItemIdx].GetGenre() == item_equip && (Item[nItemIdx].GetGoldId() > 0 || Item[nItemIdx].IsPurple()>0|| Item[nItemIdx].IsBlue()))
			//	{
#ifdef _SERVER
					int nItemClass		= Item[nItemIdx].GetGenre();
					int nDetailType		= Item[nItemIdx].GetDetailType();
					int nParticularType	= Item[nItemIdx].GetParticular();
					int nLevel			= Item[nItemIdx].GetLevel();
					int nSeries			= Item[nItemIdx].GetSeries();
					int nLuck			= Item[nItemIdx].GetItemParam()->nLuck;
					int nEnChance		= Item[nItemIdx].GetEnChance();
					int nGoldId			= Item[nItemIdx].GetGoldId();	
					int nVersion		= Item[nItemIdx].GetItemParam()->nVersion;
					UINT	nGndrom		= Item[nItemIdx].GetItemParam()->uRandomSeed;
					int nPoint			= Item[nItemIdx].GetPoint();   //紫装IsPurple
					int nRongpiont	    = Item[nItemIdx].IsRong();
					
					int nStackNum       = Item[nItemIdx].GetStackNum();
                    int nYear           = Item[nItemIdx].GetTime()->bYear;
                    int nMonth          = Item[nItemIdx].GetTime()->bMonth;
					int nDay            = Item[nItemIdx].GetTime()->bDay;
					int nHour           = Item[nItemIdx].GetTime()->bHour;
					int nMin            = Item[nItemIdx].GetTime()->bMin;

					INT pnJBLevel[7];    
					int nItemLevel[6];
					int nRItemLevel[6];
					//int nBItemLevel[6];

					if (nEnChance >= 9)
					{
						return 0;
					}

					if (GetRandomNumber(1,10) < nEnChance + 1)
					{
						if (nEnChance >= 2)
						{
							nEnChance -= 2;
						}	
					}
					ZeroMemory(nItemLevel, sizeof(nItemLevel));
					ZeroMemory(nRItemLevel, sizeof(nRItemLevel));
                    ZeroMemory(pnJBLevel, sizeof(pnJBLevel));

					for (int i = 0; i < 6; ++i)
					{
						nItemLevel[i]  = Item[nItemIdx].GetItemParam()->nGeneratorLevel[i];
						nRItemLevel[i] = Item[nItemIdx].GetItemParam()->nRGeneratorLevel[i];
//						nBItemLevel[i] = Item[nItemIdx].GetItemParam()->nBGeneratorLevel[i];
					}
					for (int k = 0; k < 7; ++k)
					{
                        pnJBLevel[k]   = Item[nItemIdx].GetItemParam()->nJGeneratorLevel[k];//基本属性
					}

					int	nIndex=0;
					//nIndex = ItemSet.AddGold(nGoldId,nItemLevel,nSeries,nEnChance + 1,nRPoint,0,0,0,0,0,nRItemLevel);
					nIndex   = ItemSet.AddOther(nItemClass,nSeries,nLevel,nLuck, 
						                 nDetailType,nParticularType,nItemLevel,
						            nVersion,nGndrom,nStackNum,nEnChance,nPoint,
									               nYear,nMonth,nDay,nHour,nMin,
										      nRongpiont,nRItemLevel,pnJBLevel,NULL);
					//  nIndex = ItemSet.AddRong(0,nSeries,nLevel,nLuck,nDetailType,nParticularType,nItemLevel,nRItemLevel,nVersion,nGndrom,1,nEnChance+1,nRPoint,nPoint);
					
					if (nIndex <= 0)
					{
						return 0;
					}
					
					Remove(nItemIdx); //删除外观等
					ItemSet.Remove(nItemIdx); //节点删除

					int	x, y;
					if (CheckCanPlaceInEquipment(Item[nIndex].GetWidth(), Item[nIndex].GetHeight(), &x, &y))
					{
						Add(nIndex, pos_equiproom,x,y);   //包袱
					}

					return nIndex;
#endif
			//	}
			//		return 0;
}   
	return 0;
}

//装备强化
int KItemList::QiangHua(int nLindexID,int nBaoshiID,int nRank)
{
			// 包袱强化  ---红狂星宝石
			//if (Item[nBaoshiID].GetGenre() == item_task && Item[nBaoshiID].GetDetailType() == 238)
				if (nLindexID && Item[nLindexID].GetGenre() == item_equip && (Item[nLindexID].GetGoldId() > 0 || Item[nLindexID].IsPurple()>0 || Item[nLindexID].IsBlue()))
				{
#ifdef _SERVER
					int nItemClass		= Item[nLindexID].GetGenre();
					int nDetailType		= Item[nLindexID].GetDetailType();
					int nParticularType	= Item[nLindexID].GetParticular();
					int nLevel			= Item[nLindexID].GetLevel();
					int nSeries			= Item[nLindexID].GetSeries();
					int nLuck			= Item[nLindexID].GetItemParam()->nLuck;
					int nEnChance		= Item[nLindexID].GetEnChance();
					int nGoldId			= Item[nLindexID].GetGoldId();	
					int nVersion		= Item[nLindexID].GetItemParam()->nVersion;
					UINT	nGndrom		= Item[nLindexID].GetItemParam()->uRandomSeed;
					int nPoint			= Item[nLindexID].GetPoint();   //紫装IsPurple
					int nRPoint			= Item[nLindexID].IsRong();
					int nBsLevel        = Item[nBaoshiID].GetLevel();
					int nIsWhere        = Item[nLindexID].GetIsWhere();

					int nItemLevel[6];
					int nRItemLevel[6];
					int nQhnEnChance=-1,nQlevel;
				   /*
					if (Item[nLindexID].IsPurple())  //紫装
					{
						msgshow("提示:<color=yellow>不支持紫装强化!<color>!");
						return 0;
					} */

					//--------------概率检测--------------------------------------------
					int nVal[3];
					g_GameSetTing.GetInt3("ServerConfig","QiangHuaRank",nVal);
					if (nEnChance >= nVal[2])
					{//大于就不能强化了
						char mgs[64];
						sprintf(mgs,"提示:<color=yellow>目前只开放强化到+%d<color>!",nVal[2]);
                        msgshow(mgs);
						return 0;
					}
//------------------------强化的概率-------------------------------	
                    char nMsg[64];	
					      sprintf(nMsg,"提示:<c=cyan>强化成功,升级!<c>!");

					g_GameSetTing.GetInteger("ServerConfig","QiangHuaSet",5,&nQlevel);	
					
					if (nEnChance >= 2 && g_Random(nVal[0]+nRank) < g_Random(nVal[1]))
					{//加成大于 2等于2的 减少的	,小于2 的 就必然成功！
                       if (nEnChance >= 2 && nEnChance<nQlevel)
						{//降低一级
						   nEnChance -= 2;
						   sprintf(nMsg,"<c=cyan>强化失败,降级!<c>!");
						}
					   else	if (nEnChance>=nQlevel)
					   {//如果是大于限制的,只升不降
						   Remove(nBaoshiID);
						   ItemSet.Remove(nBaoshiID);
						   sprintf(nMsg,"提示:<c=cyan>强化失败,平级!<c>!");
						   msgshow(nMsg);
						   return 0;
					   }
					}

					//msgshow(nMsg);

					ZeroMemory(nItemLevel, sizeof(nItemLevel));
					ZeroMemory(nRItemLevel, sizeof(nRItemLevel));

					for (int i = 0; i < 6; ++i)
					{
						nItemLevel[i]  = Item[nLindexID].GetItemParam()->nGeneratorLevel[i];
						nRItemLevel[i] = Item[nLindexID].GetItemParam()->nRGeneratorLevel[i];
					}

					int	nIndex;
		
					if (nGoldId>0)
					{//黄金的强化
						Remove(nBaoshiID);
						ItemSet.Remove(nBaoshiID);

						if (Item[nLindexID].GetGoldId() > 0 /*&& nRPoint>0*/)
						{//黄金装 
                          Item[nLindexID].EnChanceRong(nEnChance+1);
						  Item[nLindexID].EnChanceBao(nEnChance+1);   //宝石强化
                          SynPlayerItem(nLindexID);	                  //同步到客户端
						  msgshow(nMsg);
						 // msgshow("提示:<color=yellow>黄金装强化成功!<color>!");
						}

						return nLindexID;				
					} 
					else
					{//蓝装和其他的强化				
						if (nLindexID <= 0)
							return 0;
												
						Remove(nBaoshiID);
						ItemSet.Remove(nBaoshiID);	

						if (Item[nLindexID].IsBlue() && (nRPoint>0 || nIsWhere>0))  //纹钢过的 或 辨识过的蓝装
						{
							Item[nLindexID].EnChanceRong(nEnChance+1);
							msgshow(nMsg);
						}
						else if (Item[nLindexID].IsBlue())      //全部蓝装
						{//蓝装
						  Item[nLindexID].EnChance(nEnChance+1);
						   msgshow(nMsg);
						}
						else if (Item[nLindexID].IsPurple())
						{//紫装
						   Item[nLindexID].EnChance(nEnChance+1);
						   msgshow(nMsg);
						}

						 //msgshow("提示:<color=yellow>蓝色装强化成功!<color>!");
                         SynPlayerItem(nLindexID);	  //同步到客户端

						return nLindexID;
					}				
#endif
				}
					return 0;
}
//移动物品 装备 快捷栏 包袱  储物箱 扩展箱
void KItemList::ExchangeItem(ItemPos* SrcPos, ItemPos* DesPos,int nIsMianBam)
{
	if  (!SrcPos||!DesPos) return;
	if (SrcPos->nPlace != DesPos->nPlace)  //不同地方的东西 就返回
		return;

	int nTempHand = m_Hand;
	int	nEquipIdx1 = 0;

#ifdef _SERVER
	PLAYER_MOVE_ITEM_SYNC	sMove;
	sMove.ProtocolType = s2c_playermoveitem;
	sMove.m_btDownPos  = SrcPos->nPlace;  //需要放进的容器
	sMove.m_btDownX    = SrcPos->nX;
	sMove.m_btDownY    = SrcPos->nY;
    sMove.m_DownRongqi = SrcPos->nSIsRongqi;
	sMove.m_btUpPos    = DesPos->nPlace;
	sMove.m_btUpX      = DesPos->nX;
	sMove.m_btUpY      = DesPos->nY;
	sMove.m_UpRongqi   = DesPos->nDIsRongqi;
	sMove.m_IsMianBan  = nIsMianBam;
    //char msg[128];
	//sprintf(msg,"mb:%d,Ser:spy:%d,sxy:%d,syy:%d||spd:%d,sxd:%d,syd:%d",sMove.m_IsMianBan,sMove.m_btDownPos,sMove.m_btDownX,sMove.m_btDownY,sMove.m_btUpPos,sMove.m_btUpX,sMove.m_btUpY);  
	//msgshow(msg);
#endif

	// 源装备从SrcPos到手上，手上的装备到DesPos
	switch(SrcPos->nPlace)
	{
	case pos_hand:  //手里
		if (Player[this->m_PlayerIdx].getMobileClient()) //如果不是手机端 就启用
			return;
		break;
	case pos_equip:   //身上装备
		if (Player[this->m_PlayerIdx].CheckTrading())	// 如果正在交易
			return;

		if (SrcPos->nX < 0 || SrcPos->nX >= itempart_num || DesPos->nX < 0 || DesPos->nX >= itempart_num)
			return;
       
        if (m_Hand)
		{//手里原来有装备
			if 	(Item[m_Hand].GetIsWhere()>0 && Item[m_Hand].GetIsCanUse()<=0)  //是否装备
			{//已经辨识的装备

#ifdef _SERVER
					msgshow("提示:<color=yellow>该辨识装备(使用时间已到)不能装备<color>!");
#endif
             /* if (nEquipIdx1)
			  {//否则保留原装备
				Equip(nEquipIdx1, SrcPos->nX);
			  }	  */
			  return;
			}

			if 	(Item[m_Hand].GetGenre() ==item_equip && Item[m_Hand].GetDetailType() !=equip_mask && Item[m_Hand].GetIsCanUse()<=0)  //是否装备
			{//已经辨识的装备			
#ifdef _SERVER
				msgshow("提示:<color=yellow>该装备(需要修理)不能装备<color>!");
#endif
              /* if (nEquipIdx1)
		       {//否则保留原装备
				 Equip(nEquipIdx1, SrcPos->nX);
			   }*/
			 return;
			}
		}
//------------------------------------------------------------------
		nEquipIdx1 = m_EquipItem[SrcPos->nX]; //检测该部位是否有装备

		if (nEquipIdx1)
		{//如果有装备
			UnEquip(nEquipIdx1, SrcPos->nX); //移除源装备
		}

		if (m_Hand)
		{//手里原来有装备
	
			if (CanEquip(m_Hand, DesPos->nX)) //部位是否合适该装备
			{//检测是否可以装备
				Equip(m_Hand, DesPos->nX);  //换装
				m_Hand = nEquipIdx1;        //(拿起了源装备)
				m_Items[FindSame(nEquipIdx1)].nPlace = pos_hand;
#ifdef _SERVER
			    EquipAbrade(m_Hand,enumAbradeEquip,DesPos->nX/*m_Items[FindSame(nEquipIdx1)].nX*/);//换装磨损
				g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
			}
			else if (nEquipIdx1)
			{//否则保留原装备
				Equip(nEquipIdx1, SrcPos->nX);
			}
		}
		else
		{//手里没有东西 则拿起该目标装备
			m_Hand = nEquipIdx1;                              //手里装备的 索引
			m_Items[FindSame(nEquipIdx1)].nPlace = pos_hand;  //赋值给该物品的位置信息 在手里

#ifdef _SERVER
			EquipAbrade(m_Hand,enumAbradeEquip,SrcPos->nX/*m_Items[FindSame(nEquipIdx1)].nX*/);//换装磨损
			g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
		}
		break;
	case pos_immediacy:  //单对象容器快捷栏
		if (Player[m_PlayerIdx].CheckTrading())	// 如果正在交易
			return;
		// 判断是否有同类型的物品，有的话不让放下去
		if (m_Hand)
		{
		//	if (Item[m_Hand].GetGenre()==item_equip || Item[m_Hand].GetGenre()==item_task || Item[m_Hand].GetGenre()==item_materials|| Item[m_Hand].GetGenre()==item_mine)		
		    if 	(Item[m_Hand].GetGenre() == item_equip || Item[m_Hand].GetIsKuaiJie()<=0)  //是否可以快捷栏
			{
#ifdef _SERVER
				msgshow("提示:<color=yellow>该物品不能放进快捷栏<color>!");
#endif
				return;
			}

			if (m_Room[room_immediacy].CheckSameDetailType(Item[m_Hand].GetGenre(), Item[m_Hand].GetDetailType(),Item[m_Hand].GetParticular()))
			{

#ifdef _SERVER
				BYTE	byFinished = s2c_itemexchangefinish;
				if (g_pServer)
					g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, &byFinished, sizeof(BYTE));
#endif

#ifndef _SERVER
				KSystemMessage	sMsg;
				sMsg.eType = SMT_NORMAL;
				sMsg.byConfirmType = SMCT_NONE;
				sMsg.byPriority = 0;
				sMsg.byParamSize = 0;
				snprintf(sMsg.szMessage, sizeof(sMsg.szMessage), "%s", MSG_ITEM_SAME_DETAIL_IN_IMMEDIATE);
				sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
#endif
				return;
			}
		}
		nEquipIdx1 = m_Room[room_immediacy].FindItem(SrcPos->nX,SrcPos->nY);
		if (nEquipIdx1 < 0) 
			return; //这个位置没有东西
		
		// 先把东西拣起来
		if (nEquipIdx1)
		{//如果有东西
			if (!m_Room[room_immediacy].PickUpItem(nEquipIdx1, SrcPos->nX, SrcPos->nY, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight()))
			{
				return;
			}
		}

		if (m_Hand)
		{
			if (Item[m_Hand].CanStack(nEquipIdx1))   //是否可以叠加--手里和包袱里的是否相同
			{
#ifdef _SERVER
				g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
				if (Item[nEquipIdx1].Stack(Item[m_Hand].GetStackNum()))  //叠加
				{
					Item[m_Hand].Remove();  //删除手里拿的
					Remove(m_Hand);
				}
				else
				{//叠加数大于50的
					Item[m_Hand].SetStackNum(Item[nEquipIdx1].GetStackNum() - MAX_STACKNUM);  //手里
					Item[nEquipIdx1].SetStackNum(MAX_STACKNUM);  //实际物品的
				}

				m_Room[room_immediacy].PlaceItem(SrcPos->nX, SrcPos->nY,nEquipIdx1,Item[nEquipIdx1].GetWidth(),Item[nEquipIdx1].GetHeight());
				
				return;
			}
		}
		// 如果鼠标上有东西，先试试能不能把他放下去，如果不行的话，还得把原来的东西放回去
		if (m_Hand)
		{
			if (m_Room[room_immediacy].PlaceItem(DesPos->nX, DesPos->nY, m_Hand, Item[m_Hand].GetWidth(), Item[m_Hand].GetHeight())
				&& Item[m_Hand].GetGenre() != item_equip/*(Item[m_Hand].GetGenre() == item_mine || Item[m_Hand].GetGenre() == item_medicine || Item[m_Hand].GetGenre() == item_townportal || Item[m_Hand].GetGenre() == item_task)*/)
			{
				int nListIdx = FindSame(m_Hand);
				m_Items[nListIdx].nPlace = pos_immediacy;
				m_Items[nListIdx].nX = DesPos->nX;
				m_Items[nListIdx].nY = DesPos->nY;
				m_Hand = nEquipIdx1;
				m_Items[FindSame(nEquipIdx1)].nPlace = pos_hand;
#ifdef _SERVER
				g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
			}
			// 把原来的东西放回去
			else
			{
				m_Room[room_immediacy].PlaceItem(SrcPos->nX, SrcPos->nY, nEquipIdx1, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight());
			}
		}
		// 如果鼠标上没有东西，只需要把拣起来的东西放到鼠标上
		else
		{
			int nListIdx = FindSame(nEquipIdx1);
			if (nEquipIdx1 && nListIdx)
			{
				m_Items[nListIdx].nPlace = pos_hand;
				m_Hand = nEquipIdx1;
			}
#ifdef _SERVER
			g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
		}
		break;
	case pos_repositoryroom:  //多对象容器储物箱
		if (Player[m_PlayerIdx].CheckTrading())	// 如果正在交易
			return;
		nEquipIdx1 = m_Room[room_repository].FindItem(SrcPos->nX, SrcPos->nY);
		if (nEquipIdx1 < 0)
			return;

		// 先把东西拣起来
		if (nEquipIdx1)
		{
			if (!m_Room[room_repository].PickUpItem(nEquipIdx1, SrcPos->nX, SrcPos->nY, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight()))
				return;
		}
		if (m_Hand)
			if (Item[m_Hand].CanStack(nEquipIdx1))
			{
#ifdef _SERVER
				g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
				if (Item[nEquipIdx1].Stack(Item[m_Hand].GetStackNum()))
				{
					Item[m_Hand].Remove();
					Remove(m_Hand);
				}
				else
				{
					Item[m_Hand].SetStackNum(Item[nEquipIdx1].GetStackNum() - MAX_STACKNUM); //手里的减少
					Item[nEquipIdx1].SetStackNum(MAX_STACKNUM); //包袱的增加
				}
				m_Room[room_repository].PlaceItem(SrcPos->nX, SrcPos->nY, nEquipIdx1, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight());
				
				return;
			}
			// 如果鼠标上有东西，先试试能不能把他放下去，如果不行的话，还得把原来的东西放回去
			if (m_Hand)
			{
				if (m_Room[room_repository].PlaceItem(DesPos->nX, DesPos->nY, m_Hand, Item[m_Hand].GetWidth(), Item[m_Hand].GetHeight()))
				{ 
				  int nListIdx = FindSame(m_Hand);
				  m_Items[nListIdx].nPlace = pos_repositoryroom;
				  m_Items[nListIdx].nX = DesPos->nX;
				  m_Items[nListIdx].nY = DesPos->nY;
				  m_Hand = nEquipIdx1;
				  m_Items[FindSame(nEquipIdx1)].nPlace = pos_hand;
#ifdef _SERVER
				  g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
				} 
			// 把原来的东西放回去
			    else
				{ 
				m_Room[room_repository].PlaceItem(SrcPos->nX, SrcPos->nY, nEquipIdx1, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight());
				} 
		}
		// 如果鼠标上没有东西，只需要把拣起来的东西放到鼠标上
		else
		{
			int nListIdx = FindSame(nEquipIdx1);
			if (nEquipIdx1 && nListIdx)
			{
				m_Items[nListIdx].nPlace = pos_hand;
				m_Hand = nEquipIdx1;
			}
#ifdef _SERVER
			g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
		}
		break;
	//====================================================================================
	case pos_exbox1room: // ruong mo rong 1  扩展箱
		if (Player[m_PlayerIdx].CheckTrading())	// 如果正在交易
			return;
		nEquipIdx1 = m_Room[room_exbox1].FindItem(SrcPos->nX, SrcPos->nY);
		if (nEquipIdx1 < 0)
			return;

		// 先把东西拣起来
		if (nEquipIdx1)
		{
			if (!m_Room[room_exbox1].PickUpItem(nEquipIdx1, SrcPos->nX, SrcPos->nY, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight()))
				return;
		}

		// 如果鼠标上有东西，先试试能不能把他放下去，如果不行的话，还得把原来的东西放回去
		if (m_Hand)
		{
			if (m_Room[room_exbox1].PlaceItem(DesPos->nX, DesPos->nY, m_Hand, Item[m_Hand].GetWidth(), Item[m_Hand].GetHeight()))
			{
				int nListIdx = FindSame(m_Hand);
				m_Items[nListIdx].nPlace = pos_exbox1room;
				m_Items[nListIdx].nX = DesPos->nX;
				m_Items[nListIdx].nY = DesPos->nY;
				m_Hand = nEquipIdx1;
				m_Items[FindSame(nEquipIdx1)].nPlace = pos_hand;
#ifdef _SERVER
				g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
			}
			// 把原来的东西放回去
			else
			{
				m_Room[room_exbox1].PlaceItem(SrcPos->nX, SrcPos->nY, nEquipIdx1, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight());
			}
		}
		// 如果鼠标上没有东西，只需要把拣起来的东西放到鼠标上
		else
		{
			int nListIdx = FindSame(nEquipIdx1);
			if (nEquipIdx1 && nListIdx)
			{
				m_Items[nListIdx].nPlace = pos_hand;
				m_Hand = nEquipIdx1;
			}
#ifdef _SERVER
			g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
		}
		break;
//---------------------------------------------------
	case pos_exbox2room: // ruong mo rong 2
		if (Player[m_PlayerIdx].CheckTrading())	// 如果正在交易
			return;
		nEquipIdx1 = m_Room[room_exbox2].FindItem(SrcPos->nX, SrcPos->nY);
		if (nEquipIdx1 < 0)
			return;

		// 先把东西拣起来
		if (nEquipIdx1)
		{
			if (!m_Room[room_exbox2].PickUpItem(nEquipIdx1, SrcPos->nX, SrcPos->nY, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight()))
				return;
		}

		// 如果鼠标上有东西，先试试能不能把他放下去，如果不行的话，还得把原来的东西放回去
		if (m_Hand)
		{
			if (m_Room[room_exbox2].PlaceItem(DesPos->nX, DesPos->nY, m_Hand, Item[m_Hand].GetWidth(), Item[m_Hand].GetHeight()))
			{
				int nListIdx = FindSame(m_Hand);
				m_Items[nListIdx].nPlace = pos_exbox2room;
				m_Items[nListIdx].nX = DesPos->nX;
				m_Items[nListIdx].nY = DesPos->nY;
				m_Hand = nEquipIdx1;
				m_Items[FindSame(nEquipIdx1)].nPlace = pos_hand;
#ifdef _SERVER
				g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
			}
			// 把原来的东西放回去
			else
			{
				m_Room[room_exbox2].PlaceItem(SrcPos->nX, SrcPos->nY, nEquipIdx1, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight());
			}
		}
		// 如果鼠标上没有东西，只需要把拣起来的东西放到鼠标上
		else
		{
			int nListIdx = FindSame(nEquipIdx1);
			if (nEquipIdx1 && nListIdx)
			{
				m_Items[nListIdx].nPlace = pos_hand;
				m_Hand = nEquipIdx1;
			}
#ifdef _SERVER
			g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
		}
		break;
//----------------------------------------------------------------------
	case pos_exbox3room: // ruong mo rong 3
		if (Player[m_PlayerIdx].CheckTrading())	// 如果正在交易
			return;
		nEquipIdx1 = m_Room[room_exbox3].FindItem(SrcPos->nX, SrcPos->nY);
		if (nEquipIdx1 < 0)
			return;

		// 先把东西拣起来
		if (nEquipIdx1)
		{
			if (!m_Room[room_exbox3].PickUpItem(nEquipIdx1, SrcPos->nX, SrcPos->nY, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight()))
				return;
		}

		// 如果鼠标上有东西，先试试能不能把他放下去，如果不行的话，还得把原来的东西放回去
		if (m_Hand)
		{
			if (m_Room[room_exbox3].PlaceItem(DesPos->nX, DesPos->nY, m_Hand, Item[m_Hand].GetWidth(), Item[m_Hand].GetHeight()))
			{
				int nListIdx = FindSame(m_Hand);
				m_Items[nListIdx].nPlace = pos_exbox3room;
				m_Items[nListIdx].nX = DesPos->nX;
				m_Items[nListIdx].nY = DesPos->nY;
				m_Hand = nEquipIdx1;
				m_Items[FindSame(nEquipIdx1)].nPlace = pos_hand;
#ifdef _SERVER
				g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
			}
			// 把原来的东西放回去
			else
			{
				m_Room[room_exbox3].PlaceItem(SrcPos->nX, SrcPos->nY, nEquipIdx1, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight());
			}
		}
		// 如果鼠标上没有东西，只需要把拣起来的东西放到鼠标上
		else
		{
			int nListIdx = FindSame(nEquipIdx1);
			if (nEquipIdx1 && nListIdx)
			{
				m_Items[nListIdx].nPlace = pos_hand;
				m_Hand = nEquipIdx1;
			}
#ifdef _SERVER
			g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
		}
		break;
//--------------------------------------------------------------------
	// add new
	case pos_equiproomex: // mo rong hanh trang
		nEquipIdx1 = m_Room[room_equipmentex].FindItem(SrcPos->nX, SrcPos->nY);
		if (nEquipIdx1 < 0)
			return;

		// 先把东西拣起来
		if (nEquipIdx1)
		{
			if (!m_Room[room_equipmentex].PickUpItem(nEquipIdx1, SrcPos->nX, SrcPos->nY, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight()))
				return;
		}
		// 如果鼠标上有东西，先试试能不能把他放下去，如果不行的话，还得把原来的东西放回去
		if (m_Hand)
		{
			if (m_Room[room_equipmentex].PlaceItem(DesPos->nX, DesPos->nY, m_Hand, Item[m_Hand].GetWidth(), Item[m_Hand].GetHeight()))
			{
				int nListIdx = FindSame(m_Hand);
				m_Items[nListIdx].nPlace = pos_equiproomex;
				m_Items[nListIdx].nX = DesPos->nX;
				m_Items[nListIdx].nY = DesPos->nY;
				m_Hand = nEquipIdx1;
				m_Items[FindSame(nEquipIdx1)].nPlace = pos_hand;
#ifdef _SERVER
				g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
			}
			// 把原来的东西放回去
			else
			{
				m_Room[room_equipmentex].PlaceItem(SrcPos->nX, SrcPos->nY, nEquipIdx1, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight());
			}
		}
		// 如果鼠标上没有东西，只需要把拣起来的东西放到鼠标上
		else
		{
			int nListIdx = FindSame(nEquipIdx1);
			if (nEquipIdx1 && nListIdx)
			{
				m_Items[nListIdx].nPlace = pos_hand;
				m_Hand = nEquipIdx1;
			}

#ifdef _SERVER
			g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
		}
		break;
	//--------------------------------------------------------------------------
	case pos_equiproom:    //包袱
		nEquipIdx1 = m_Room[room_equipment].FindItem(SrcPos->nX, SrcPos->nY);

		if (nEquipIdx1 < 0) //如果没有东西 就返回
			return;

		// 先把东西拣起来
		if (nEquipIdx1)
		{ //清空容器位置为0
			if (!m_Room[room_equipment].PickUpItem(nEquipIdx1, SrcPos->nX, SrcPos->nY, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight()))
				return;
		}
/*
        #ifdef _SERVER
		char msg[64];
		sprintf(msg,"[SR_equiproom] X:%d-Y:%d",SrcPos->nX,SrcPos->nY);
		msgshow(msg);
        #else
		char msg[64];
		sprintf(msg,"[CR_equiproom] X:%d-Y:%d",SrcPos->nX,SrcPos->nY);
		ClientShowMsg(msg);
        #endif
*/
		if (m_Hand)
			if (Item[m_Hand].CanStack(nEquipIdx1))
			{
#ifdef _SERVER
				g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
				if (Item[nEquipIdx1].Stack(Item[m_Hand].GetStackNum()))
				{
					Item[m_Hand].Remove();
					Remove(m_Hand);
				}
				else
				{
					Item[m_Hand].SetStackNum(Item[nEquipIdx1].GetStackNum() - MAX_STACKNUM);
					Item[nEquipIdx1].SetStackNum(MAX_STACKNUM);
				}
                //设置容器位置为Idx，叠加物品源位置不动
				m_Room[room_equipment].PlaceItem(SrcPos->nX, SrcPos->nY, nEquipIdx1, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight());
				
				return;
			}
		// 如果鼠标上有东西，先试试能不能把他放下去，如果不行的话，还得把原来的东西放回去
		if (m_Hand)
		{//设置目标容器位置idx
			if (m_Room[room_equipment].PlaceItem(DesPos->nX, DesPos->nY, m_Hand, Item[m_Hand].GetWidth(), Item[m_Hand].GetHeight()))
			{
				int nListIdx = FindSame(m_Hand);
				m_Items[nListIdx].nPlace = pos_equiproom;
				m_Items[nListIdx].nX = DesPos->nX;
				m_Items[nListIdx].nY = DesPos->nY;
				m_Hand = nEquipIdx1;
				m_Items[FindSame(nEquipIdx1)].nPlace = pos_hand;
#ifdef _SERVER
				g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
			}
			// 把原来的东西放回去
			else
			{
				m_Room[room_equipment].PlaceItem(SrcPos->nX, SrcPos->nY, nEquipIdx1, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight());
			}
		}
		// 如果鼠标上没有东西，只需要把拣起来的东西放到鼠标上
		else
		{
			int nListIdx = FindSame(nEquipIdx1);
			if (nEquipIdx1 && nListIdx)
			{
				m_Items[nListIdx].nPlace = pos_hand;
				m_Hand = nEquipIdx1;
			}

#ifdef _SERVER
			g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
		}
		break;
	case pos_traderoom:
		if (!Player[m_PlayerIdx].CheckTrading() )	// 不在交易
			return;
#ifdef _SERVER

		if (m_Hand && (Item[m_Hand].GetIsBang() || Item[m_Hand].GetUseMapIdx()) && !Player[m_PlayerIdx].CheckCanTradeName())
		{
			// 通知界面，任务道具，不能交易			   
			  SHOW_MSG_SYNC	sMsg;
			  sMsg.ProtocolType = s2c_msgshow;
			  sMsg.m_wMsgID = enumMSG_ID_TRADE_TASK_ITEM;
			  sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1;
			  g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE *)&sMsg, sMsg.m_wLength + 1);
			  return;
		}
#endif
		nEquipIdx1 = m_Room[room_trade].FindItem(SrcPos->nX, SrcPos->nY);
		if (nEquipIdx1 < 0)
			return;

		// 先把东西拣起来
		if (nEquipIdx1)
		{
			if (!m_Room[room_trade].PickUpItem(nEquipIdx1, SrcPos->nX, SrcPos->nY, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight()))
				return;
		}
		// 如果鼠标上有东西，先试试能不能把他放下去，如果不行的话，还得把原来的东西放回去
		if (m_Hand)
		{
			if (m_Room[room_trade].PlaceItem(DesPos->nX, DesPos->nY, m_Hand, Item[m_Hand].GetWidth(), Item[m_Hand].GetHeight()))
			{
				int nListIdx = FindSame(m_Hand);
				m_Items[nListIdx].nPlace = pos_traderoom;
				m_Items[nListIdx].nX = DesPos->nX;
				m_Items[nListIdx].nY = DesPos->nY;
#ifdef _SERVER
				// 发消息给交易对方
				if (nEquipIdx1)	// 交易栏有东西被捡起
				{
					ITEM_REMOVE_SYNC	sRemove;
					sRemove.ProtocolType = s2c_removeitem;
					sRemove.m_ID = Item[nEquipIdx1].m_dwID;
					sRemove.m_dx = 0;
					sRemove.m_dy = 0;
					sRemove.m_model = 0;
					sRemove.nPlace     =-1;
					g_pServer->PackDataToClient(Player[Player[m_PlayerIdx].m_cTrade.m_nTradeDest].m_nNetConnectIdx, (BYTE*)&sRemove, sizeof(ITEM_REMOVE_SYNC));
				}
				// 手上的东西放在了交易栏
				ITEM_SYNC	sItem;
				memset(&sItem, 0, sizeof(sItem));
				sItem.ProtocolType = s2c_syncitem;
				sItem.m_Point = Item[m_Hand].GetPoint();
				sItem.m_Rongpiont = Item[m_Hand].IsRong(); //熔炼
				sItem.m_GoldId = Item[m_Hand].GetGoldId();
				sItem.m_IsPlatima    = Item[m_Hand].GetIsPlatima();
				sItem.m_UseMap       = Item[m_Hand].GetUseMapIdx();
				sItem.m_UseKind		 = Item[m_Hand].GetUseKind();
				sItem.m_EnChance = Item[m_Hand].GetEnChance();
				sItem.m_StackNum = Item[m_Hand].GetStackNum();
				sItem.m_ID = Item[m_Hand].GetID();
				sItem.m_Genre = Item[m_Hand].GetGenre();
				sItem.m_Detail = Item[m_Hand].GetDetailType();
				sItem.m_Particur = Item[m_Hand].GetParticular();
				sItem.m_Series = Item[m_Hand].GetSeries();
				sItem.m_Level = Item[m_Hand].GetLevel();
				sItem.m_Res   = Item[m_Hand].GetItemResIdx();
				sItem.m_Luck = Item[m_Hand].m_GeneratorParam.nLuck;
				sItem.m_btPlace = pos_trade1;
				sItem.m_btX = DesPos->nX;
				sItem.m_btY = DesPos->nY;
				sItem.m_Time.bYear = Item[m_Hand].GetTime()->bYear;
				sItem.m_Time.bMonth = Item[m_Hand].GetTime()->bMonth;
				sItem.m_Time.bDay = Item[m_Hand].GetTime()->bDay;
				sItem.m_Time.bHour = Item[m_Hand].GetTime()->bHour;
				sItem.m_Time.bMin = Item[m_Hand].GetTime()->bMin;
				sItem.m_IsBang     = Item[m_Hand].GetIsBang();       //是否绑定
				sItem.m_IsKuaiJie    = Item[m_Hand].GetIsKuaiJie();  //是否快捷栏
                sItem.m_IsMagic      =Item[m_Hand].GetIsMagic();     //是否有魔法属性
				sItem.m_SkillType    =Item[m_Hand].GetSkillType();
				sItem.m_MagicID      = Item[m_Hand].GetMagicID();

				sItem.m_IsWhere      = Item[m_Hand].GetIsWhere();
	            sItem.m_IsCanUse     = Item[m_Hand].GetIsCanUse();
                sItem.m_IsLoign      = 0;

				sItem.m_RandomSeed = Item[m_Hand].m_GeneratorParam.uRandomSeed;
				sItem.m_Version = Item[m_Hand].m_GeneratorParam.nVersion;
				sItem.m_Durability = Item[m_Hand].GetDurability();

				//_snprintf(sItem.m_WonName,sizeof(sItem.m_WonName)-1,"%s",Item[m_Hand].GetWonName());  //所有者
				snprintf(sItem.m_WonName, sizeof(sItem.m_WonName), "%s", Item[m_Hand].GetWonName());  //所有者

				for (int j = 0; j < 6; ++j)
				{
					sItem.m_MagicLevel[j]   = Item[m_Hand].m_GeneratorParam.nGeneratorLevel[j];
		            sItem.m_RongMagicLev[j] = Item[m_Hand].m_GeneratorParam.nRGeneratorLevel[j];
//					sItem.m_BaoMagicLev[j]  = Item[m_Hand].m_GeneratorParam.nBGeneratorLevel[j];
				}
				for (int i = 0; i < 7; ++i)
				{
					sItem.m_JBLevel[i]      = Item[m_Hand].m_GeneratorParam.nJGeneratorLevel[i];
				}

				g_pServer->PackDataToClient(Player[Player[m_PlayerIdx].m_cTrade.m_nTradeDest].m_nNetConnectIdx, (BYTE*)&sItem, sizeof(ITEM_SYNC));

				g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
				m_Hand = nEquipIdx1;
				if (FindSame(nEquipIdx1))
					m_Items[FindSame(nEquipIdx1)].nPlace = pos_hand;
			}
			// 把原来的东西放回去
			else
			{
				m_Room[room_trade].PlaceItem(SrcPos->nX, SrcPos->nY, nEquipIdx1, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight());
			}
		}
		// 如果鼠标上没有东西，只需要把拣起来的东西放到鼠标上
		else
		{
#ifdef _SERVER
			// 发消息给交易对方
			if (nEquipIdx1)	// 交易栏有东西被捡起
			{
				ITEM_REMOVE_SYNC	sRemove;
				sRemove.ProtocolType = s2c_removeitem;
				sRemove.m_ID = Item[nEquipIdx1].m_dwID;
				sRemove.m_dx = 0;
				sRemove.m_dy = 0;
				sRemove.m_model = 0;
				sRemove.nPlace     =-1;
				g_pServer->PackDataToClient(Player[Player[m_PlayerIdx].m_cTrade.m_nTradeDest].m_nNetConnectIdx, (BYTE*)&sRemove, sizeof(ITEM_REMOVE_SYNC));
			}
			g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
			int nListIdx = FindSame(nEquipIdx1);
			if (nEquipIdx1 && nListIdx)
			{
				m_Items[nListIdx].nPlace = pos_hand;
				m_Hand = nEquipIdx1;
			}
		}
		break;
	case pos_give:
		nEquipIdx1 = m_Room[room_give].FindItem(SrcPos->nX, SrcPos->nY);
		if (nEquipIdx1 < 0)
			return;

		// 先把东西拣起来
		if (nEquipIdx1)
		{
			if (!m_Room[room_give].PickUpItem(nEquipIdx1, SrcPos->nX, SrcPos->nY, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight()))
				return;
		}
		if (m_Hand)
			if (Item[m_Hand].CanStack(nEquipIdx1))
			{
#ifdef _SERVER
				g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
				if (Item[nEquipIdx1].Stack(Item[m_Hand].GetStackNum()))
				{
					Item[m_Hand].Remove();
					Remove(m_Hand);
				}
				else
				{
					Item[m_Hand].SetStackNum(Item[nEquipIdx1].GetStackNum() - MAX_STACKNUM);
					Item[nEquipIdx1].SetStackNum(MAX_STACKNUM);
				}
				m_Room[room_give].PlaceItem(SrcPos->nX, SrcPos->nY, nEquipIdx1, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight());
				
				return;
			}

		// 如果鼠标上有东西，先试试能不能把他放下去，如果不行的话，还得把原来的东西放回去
		if (m_Hand)
		{
			if (m_Room[room_give].PlaceItem(DesPos->nX, DesPos->nY, m_Hand, Item[m_Hand].GetWidth(), Item[m_Hand].GetHeight()))
			{
				int nListIdx = FindSame(m_Hand);
				m_Items[nListIdx].nPlace = pos_give;
				m_Items[nListIdx].nX = DesPos->nX;
				m_Items[nListIdx].nY = DesPos->nY;
				m_Hand = nEquipIdx1;
				m_Items[FindSame(nEquipIdx1)].nPlace = pos_hand;
#ifdef _SERVER
				g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
			}
			// 把原来的东西放回去
			else
			{
				m_Room[room_give].PlaceItem(SrcPos->nX, SrcPos->nY, nEquipIdx1, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight());
			}
		}
		// 如果鼠标上没有东西，只需要把拣起来的东西放到鼠标上
		else
		{
			int nListIdx = FindSame(nEquipIdx1);
			if (nEquipIdx1 && nListIdx)
			{
				m_Items[nListIdx].nPlace = pos_hand;
				m_Hand = nEquipIdx1;
			}

#ifdef _SERVER
			g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
		}
		break;
	case pos_dazao:  //单对象容器
		if (Player[m_PlayerIdx].CheckTrading())	// 如果正在交易
			return;
		// 判断是否有同类型的物品，有的话不让放下去
		if (m_Hand)
		{
//===================条件检测开始============================
#ifdef _SERVER  
			int nGenre=Item[m_Hand].GetGenre();                //类别
			int nPart=Item[m_Hand].GetDetailType();            //详细类别
			int nParticular= Item[m_Hand].GetParticular();     //具体类别
			int nStackNum  = Item[m_Hand].GetStackNum();       
            int nIsOpen = 0;
		
            if  (nIsMianBam==-1)  //玄静升级面板可以叠加
			{
				if (Item[m_Hand].GetTime()->bYear>0 || Item[m_Hand].GetIsBang()>0)
				{
				  msgshow("提示:材料不对,材料不能限时/绑定");
				  return; 
				}
			}
			else
			{
				if ((nStackNum>1 || Item[m_Hand].GetTime()->bYear>0 || Item[m_Hand].GetIsBang()>0) && nIsMianBam!=9)
				{//面板9 除外 辨识面板
				   msgshow("提示:材料不对,材料不能限时/绑定/叠加!");
				   return;
			     }
			}
#endif
			if  (nIsMianBam==-1)                 //升级玄晶矿石面板  1 3 5
			{
              //  int nLevelA=0,nLevelB=0,nLevelC=0;
#ifdef _SERVER

               g_GameSetTing.GetInteger("CompoundSet","IsOpenTiLiana",0,&nIsOpen);
			   
			   if (nIsOpen<=0)
			   {
				   msgshow("提示:<color=yellow>提炼已经关闭!<color>!");
				   return;
			   }

				if (SrcPos->nX==4) //小框1
				{ 
	
					int nPartTyep[3];
					ZeroMemory(nPartTyep,sizeof(nPartTyep));
					g_CompEquip.GetInt3s(4,"Compound_2",nPartTyep);
                    
					if (!strstr(Item[m_Hand].GetName(),"玄晶"))
					{
                        msgshow("提示:材料不对,请放玄晶矿石!");
						return;
					}

					if (nGenre!=nPartTyep[0] || nPart!=nPartTyep[1] || nParticular!=nPartTyep[2])
					{
						msgshow("提示:材料不对,请放玄晶矿石!");
						ZeroMemory(nPartTyep,sizeof(nPartTyep));
						return;
					}
				} 
				if (SrcPos->nX==5) //小框2
				{
					int nPartTyep[3];
					ZeroMemory(nPartTyep,sizeof(nPartTyep));
					g_CompEquip.GetInt3s(5,"Compound_2",nPartTyep);
					if (!strstr(Item[m_Hand].GetName(),"玄晶"))
					{
                        msgshow("提示:材料不对,请放玄晶矿石!");
						return;
					}

					if (nGenre!=nPartTyep[0] || nPart!=nPartTyep[1] || nParticular!=nPartTyep[2])
					{
						msgshow("提示:材料不对,请放玄晶矿石!");
						ZeroMemory(nPartTyep,sizeof(nPartTyep));
						return;
					}
				}
				if (SrcPos->nX==6) //小框3
				{ 	
					int nPartTyep[3];
					ZeroMemory(nPartTyep,sizeof(nPartTyep));
					g_CompEquip.GetInt3s(6,"Compound_2",nPartTyep);
					if (!strstr(Item[m_Hand].GetName(),"玄晶"))
					{
                        msgshow("提示:材料不对,请放玄晶矿石!");
						return;
					}
					if (nGenre!=nPartTyep[0] || nPart!=nPartTyep[1] || nParticular!=nPartTyep[2])
					{

						msgshow("提示:材料不对,请放玄晶矿石!");

						ZeroMemory(nPartTyep,sizeof(nPartTyep));
						return;
					}
				}
#endif
			}
			else if  (nIsMianBam==-2)             //升级属性矿石面板 1 3 5
			{
#ifdef _SERVER

               g_GameSetTing.GetInteger("CompoundSet","IsOpenTiLianb",0,&nIsOpen);
			   
			   if (nIsOpen<=0)
			   {
				   msgshow("提示:<color=yellow>提炼已经关闭!<color>!");
				   return;
			   }

				if (SrcPos->nX==7) //小框1
				{	
					int nPartTyep[2];
					ZeroMemory(nPartTyep,sizeof(nPartTyep));
					g_CompEquip.GetInt2s(4,"Compound_3",nPartTyep);
					if (nGenre!=2|| nPart!=1 || (nParticular<nPartTyep[0] && nParticular>nPartTyep[1]))
					{

						msgshow("提示:材料不对,请放属性矿石!");

						ZeroMemory(nPartTyep,sizeof(nPartTyep));
						return;
					}
					
				}
				if (SrcPos->nX==8) //小框2
				{	
					int nPartTyep[2];
					ZeroMemory(nPartTyep,sizeof(nPartTyep));
					g_CompEquip.GetInt2s(5,"Compound_3",nPartTyep);
					if (nGenre!=2|| nPart!=1 || (nParticular<nPartTyep[0] && nParticular>nPartTyep[1]))
					{

						msgshow("提示:材料不对,请放属性矿石!");
						ZeroMemory(nPartTyep,sizeof(nPartTyep));
						return;
					}
					
				}
				if (SrcPos->nX==9) //小框3
				{	
					int nPartTyep[2];
					ZeroMemory(nPartTyep,sizeof(nPartTyep));
					g_CompEquip.GetInt2s(6,"Compound_3",nPartTyep);
					if (nGenre!=2|| nPart!=1 || (nParticular<nPartTyep[0] && nParticular>nPartTyep[1]))
					{
						msgshow("提示:材料不对,请放属性矿石!");
						ZeroMemory(nPartTyep,sizeof(nPartTyep));
						return;
					}	
				}
#endif
			}
		    else if  (nIsMianBam==1)             //提炼玄晶矿石面板  1 3 5
			{
#ifdef _SERVER
               g_GameSetTing.GetInteger("CompoundSet","IsOpenTiLianc",0,&nIsOpen);
			   
			   if (nIsOpen<=0)
			   {
				   msgshow("提示:<color=yellow>提炼已经关闭!<color>!");
				   return;
			   }

				if (SrcPos->nX==1) //小框1
				{//戒子
                    int nPartTyep[3];
					ZeroMemory(nPartTyep,sizeof(nPartTyep));
					g_CompEquip.GetInt3s(4,"Compound_1",nPartTyep);
					if (nGenre!=nPartTyep[0] || nPart!=nPartTyep[1])
					{
						msgshow("提示:材料不对,请放戒子!");
						ZeroMemory(nPartTyep,sizeof(nPartTyep));
						return;
					}
				}
				if (SrcPos->nX==2) //小框2
				{//玉佩
					int nPartTyep[3];
					ZeroMemory(nPartTyep,sizeof(nPartTyep));
					g_CompEquip.GetInt3s(5,"Compound_1",nPartTyep);
					if (nGenre!=nPartTyep[0] || nPart!=nPartTyep[1])
					{
						msgshow("提示:材料不对,请放玉佩/香囊!");
						ZeroMemory(nPartTyep,sizeof(nPartTyep));
						return;
					}
				}
				if (SrcPos->nX==3) //小框3
				{//项链
					int nPartTyep[3];
					ZeroMemory(nPartTyep,sizeof(nPartTyep));
					g_CompEquip.GetInt3s(6,"Compound_1",nPartTyep);
					if (nGenre!=nPartTyep[0] || nPart!=nPartTyep[1])
					{
						msgshow("提示:材料不对,请放项链/护身符!");
						ZeroMemory(nPartTyep,sizeof(nPartTyep));
						return;
					}
				}
#endif
			}
			else if (nIsMianBam==2)               //提取属性面板
			{ 
#ifdef _SERVER
				g_GameSetTing.GetInteger("CompoundSet","IsOpenTiqu",0,&nIsOpen);
				
				if (nIsOpen<=0)
				{
					msgshow("提示:<color=yellow>提取已经关闭!<color>!");
					return;
			    }

				if (SrcPos->nX==10) //装备框大框
				{

					int nPartTyep[15],nReg=0;
					
					ZeroMemory(nPartTyep,sizeof(nPartTyep));
					
					g_CompEquip.GetInt15s(2,"Position_2",nPartTyep);
					
					for (int i=0;i<14;++i)
					{//检测可放置的装备的部位
						if (nPartTyep[i]==nPart)
						{
							nReg=1;
							break;
						}
					} 
						
					if (!Item[m_Hand].IsBlue())
					{
						   msgshow("提示:材料不对,请放蓝装!");

						ZeroMemory(nPartTyep,sizeof(nPartTyep));
						return;
					}

					if (nGenre!=0 || nReg==0)
					{
						   msgshow("提示:蓝装类型不对,请重新放置!");
						ZeroMemory(nPartTyep,sizeof(nPartTyep));
						return;
					}

				}
				if (SrcPos->nX==11) //小框1
				{				
					int nPartTyep[3];
					ZeroMemory(nPartTyep,sizeof(nPartTyep));
					g_CompEquip.GetInt3s(4,"Distill",nPartTyep);
					if (!strstr(Item[m_Hand].GetName(),"玄晶"))
					{
                        msgshow("提示:材料不对,请放玄晶矿石!");
						return;
					}

					if (nGenre!=nPartTyep[0] || nPart!=nPartTyep[1] || nParticular!=nPartTyep[2])
					{
						msgshow("提示:材料不对,请放玄晶矿石!");
						ZeroMemory(nPartTyep,sizeof(nPartTyep));
						return;
					}
				}
				if (SrcPos->nX==12) //小框2
				{
					// KTabFile::GetInt3s scans three %d into this array, so two elements are only enough while
					// the `Distill' cell of row 5 keeps spelling two numbers (`149|154' in the data today): the
					// third conversion of a longer cell writes past what this line declares.
					int nPartTyep[3];
					ZeroMemory(nPartTyep,sizeof(nPartTyep));
					g_CompEquip.GetInt3s(5,"Distill",nPartTyep);
					if (strstr(Item[m_Hand].GetName(),"原矿") || strstr(Item[m_Hand].GetName(),"原石"))
					{
                       
					}
					else
					{
						msgshow("提示:材料不对,请放属性原矿或原石!");
						return;
					}

					if (nGenre!=2 || nPart!=1 || (nParticular>nPartTyep[1] && nParticular<nPartTyep[0]))
					{

						msgshow("提示:材料不对,请放属性原矿或原石!");
						ZeroMemory(nPartTyep,sizeof(nPartTyep));
						return;
					}

				 }
#endif
		   }
		   else if (nIsMianBam==3)    //打造面板
		   {
#ifdef _SERVER
			   g_GameSetTing.GetInteger("CompoundSet","IsOpenDaZhao",0,&nIsOpen);
			   
			   if (nIsOpen<=0)
			   {
				   msgshow("提示:<color=yellow>打造已经关闭!<color>!");
				   return;
			   }

			   if (SrcPos->nX==13) //装备框大框
			   {
				   int nPartTyep[15],nReg=0;
				   
				   ZeroMemory(nPartTyep,sizeof(nPartTyep));
				   
				   g_CompEquip.GetInt15s(2,"Position_3",nPartTyep);
				   
				   for (int i=0;i<14;++i)
				   {//检测可放置的装备的部位
					   if (nPartTyep[i]==nPart)
					   {
						   nReg=1;
						   break;
					   }
				   } 	
				   if (Item[m_Hand].IsPurple()>0 || Item[m_Hand].GetGoldId()>0)
				   {
					   msgshow("提示:材料不对,请放蓝白装!");
					   ZeroMemory(nPartTyep,sizeof(nPartTyep));
					   return;
				   }
				   if (nGenre!=0 || nReg==0)
				   {
					   msgshow("提示:装备类型不对,请重新放置!");
					   ZeroMemory(nPartTyep,sizeof(nPartTyep));
					   return;
				   }
				   
				}
			   if (SrcPos->nX==14) //小框1
			   {	
				   int nPartTyep[3];
				   ZeroMemory(nPartTyep,sizeof(nPartTyep));
				   g_CompEquip.GetInt3s(3,"Forge",nPartTyep);
				   if (!strstr(Item[m_Hand].GetName(),"玄晶"))
					{
                        msgshow("提示:材料不对,请放玄晶矿石!");
						return;
					}
				   if (nGenre!=nPartTyep[0] || nPart!=nPartTyep[1] || nParticular!=nPartTyep[2])
				   {
					   msgshow("提示:材料不对,请放玄晶矿石!");
					   ZeroMemory(nPartTyep,sizeof(nPartTyep));
					   return;
				   }
				   
				}
#endif
		   }
		   else if (nIsMianBam==4)    //镶套面板
		   {
#ifdef _SERVER
			   g_GameSetTing.GetInteger("CompoundSet","IsOpenXiang",0,&nIsOpen);
			   
			   if (nIsOpen<=0)
			   {
				   msgshow("提示:<color=yellow>镶套已经关闭!<color>!");
				   return;
			   }

			   if (SrcPos->nX==15) //装备大框
			   {
				   int nPartTyep[15],nReg=0;
				   
				   ZeroMemory(nPartTyep,sizeof(nPartTyep));
				   
				   g_CompEquip.GetInt15s(2,"Position_4",nPartTyep);
				   
				   for (int i=0;i<14;++i)
				   {//检测可放置的装备的部位
					   if (nPartTyep[i]==nPart)
					   {
						   nReg=1;
						   break;
					   }
				   } 	
				   if (!Item[m_Hand].IsPurple())
				   {
					   msgshow("提示:材料不对,请放紫色装备!");
					   ZeroMemory(nPartTyep,sizeof(nPartTyep));
					   return;
				   }
				   if (nGenre!=0 || nReg==0)
				   {
					   msgshow("提示:装备类型不对,请重新放置!");
					   ZeroMemory(nPartTyep,sizeof(nPartTyep));
					   return;
				   }
				   
			   }
			   if (SrcPos->nX==16) //小框1
			   {	
				   int nPartTyep[3];
				   ZeroMemory(nPartTyep,sizeof(nPartTyep));
				   g_CompEquip.GetInt3s(4,"Enchase",nPartTyep); //玄晶矿石

                    if (!strstr(Item[m_Hand].GetName(),"玄晶"))
					{
                        msgshow("提示:材料不对,请放玄晶矿石!");
						return;
					}

				   if (nGenre!=nPartTyep[0] || nPart!=nPartTyep[1] || nParticular!=nPartTyep[2])
				   {
					   msgshow("提示:材料不对,请放玄晶矿石!");
					   ZeroMemory(nPartTyep,sizeof(nPartTyep));
					   return;
				   }
				   
				}

			   if (SrcPos->nX==17) //小框2
			   {	
				   int nPartTyep[2];
				   ZeroMemory(nPartTyep,sizeof(nPartTyep));
				   g_CompEquip.GetInt2s(5,"Enchase",nPartTyep);
				   if (nGenre!=2 || nPart!=1 || nParticular < nPartTyep[0] || nParticular > nPartTyep[1])
				   {
					   msgshow("提示:材料不对,请放属性矿石!");
					   ZeroMemory(nPartTyep,sizeof(nPartTyep));
					   return;
				   }   
			   }
#endif
		   }
		   else if (nIsMianBam==5)    //图谱面板
		   {
#ifdef _SERVER
               g_GameSetTing.GetInteger("CompoundSet","IsOpenTuPu",0,&nIsOpen);
			   
			   if (nIsOpen<=0)
			   {
				   msgshow("提示:<color=yellow>图谱已经关闭!<color>!");
				   return;
			   }
			   if (nGenre==1 || nGenre==0)
			   {
				   msgshow("提示:材料类型不对,请看说明!");
				   return;
			   } 
			   // as above on `Atlas' row 5 (`239|391'): the helper's format counts three elements and this
			   // room is two, so the pair agrees only as long as the data file does not add a number.
			   int nTupuTyep[3];
			   ZeroMemory(nTupuTyep,sizeof(nTupuTyep));
			   g_CompEquip.GetInt3s(5,"Atlas",nTupuTyep);//图谱

			   int nXuanTyep[3];
			   ZeroMemory(nXuanTyep,sizeof(nXuanTyep));
			   g_CompEquip.GetInt3s(4,"Atlas",nXuanTyep); //玄晶矿石

			    if (SrcPos->nX==18) //小框1
				{
                   if (nParticular>=nTupuTyep[0] && nParticular<=nTupuTyep[1])
				   { 
					   msgshow("提示:材料不对,不能放<color=yellow>黄金图谱<color>!");
					   ZeroMemory(nTupuTyep,sizeof(nTupuTyep));
					   return;
				   } 
				   if (nGenre==nXuanTyep[0] && nPart==nXuanTyep[1] && nParticular==nXuanTyep[2])
				   {
					   msgshow("提示:材料不对,不能放<color=yellow>玄晶矿石<color>!");
					   ZeroMemory(nXuanTyep,sizeof(nXuanTyep));
					   return;
				   }
				}
				else if (SrcPos->nX==19)
				{
                   if (nParticular>=nTupuTyep[0] && nParticular<=nTupuTyep[1])
				   { 
					   msgshow("提示:材料不对,不能放<color=yellow>黄金图谱<color>!");
					   ZeroMemory(nTupuTyep,sizeof(nTupuTyep));
					   return;
				   } 
				   if (nGenre==nXuanTyep[0] && nPart==nXuanTyep[1] && nParticular==nXuanTyep[2])
				   {
					   msgshow("提示:材料不对,不能放<color=yellow>玄晶矿石<color>!");
					   ZeroMemory(nXuanTyep,sizeof(nXuanTyep));
					   return;
				   }
				}
				else if (SrcPos->nX==20)
				{
                   if (nParticular>=nTupuTyep[0] && nParticular<=nTupuTyep[1])
				   { 
					   msgshow("提示:材料不对,不能放<color=yellow>黄金图谱<color>!");
					   ZeroMemory(nTupuTyep,sizeof(nTupuTyep));
					   return;
				   } 
				   if (nGenre==nXuanTyep[0] && nPart==nXuanTyep[1] && nParticular==nXuanTyep[2])
				   {
					   msgshow("提示:材料不对,不能放<color=yellow>玄晶矿石<color>!");
					   ZeroMemory(nXuanTyep,sizeof(nXuanTyep));
					   return;
				   }
				}
				else if (SrcPos->nX==21)
				{
                   if (nParticular>=nTupuTyep[0] && nParticular<=nTupuTyep[1])
				   { 
					   msgshow("提示:材料不对,不能放<color=yellow>黄金图谱<color>!");
					   ZeroMemory(nTupuTyep,sizeof(nTupuTyep));
					   return;
				   } 
				   if (nGenre==nXuanTyep[0] && nPart==nXuanTyep[1] && nParticular==nXuanTyep[2])
				   {
					   msgshow("提示:材料不对,不能放<color=yellow>玄晶矿石<color>!");
					   ZeroMemory(nXuanTyep,sizeof(nXuanTyep));
					   return;
				   }
				}
				else if (SrcPos->nX==22)
				{

                   if (nParticular>=nTupuTyep[0] && nParticular<=nTupuTyep[1])
				   { 
					   msgshow("提示:材料不对,不能放<color=yellow>黄金图谱<color>!");
					   ZeroMemory(nTupuTyep,sizeof(nTupuTyep));
					   return;
				   } 
				   if (nGenre==nXuanTyep[0] && nPart==nXuanTyep[1] && nParticular==nXuanTyep[2])
				   {
					   msgshow("提示:材料不对,不能放<color=yellow>玄晶矿石<color>!");
					   ZeroMemory(nXuanTyep,sizeof(nXuanTyep));
					   return;
				   }
				}
				else if (SrcPos->nX==23)
				{
                   if (nParticular>=nTupuTyep[0] && nParticular<=nTupuTyep[1])
				   { 
					   msgshow("提示:材料不对,不能放<color=yellow>黄金图谱<color>!");
					   ZeroMemory(nTupuTyep,sizeof(nTupuTyep));
					   return;
				   } 
				   if (nGenre==nXuanTyep[0] && nPart==nXuanTyep[1] && nParticular==nXuanTyep[2])
				   {
					   msgshow("提示:材料不对,不能放<color=yellow>玄晶矿石<color>!");
					   ZeroMemory(nXuanTyep,sizeof(nXuanTyep));
					   return;
				   }
				}
				else if (SrcPos->nX==24)
				{//图谱
				   // the same helper on the same `Atlas' row-5 cell as the nTupuTyep read above: three %d, two here.
				   int nPartTyep[3];
				   ZeroMemory(nPartTyep,sizeof(nPartTyep));
				   g_CompEquip.GetInt3s(5,"Atlas",nPartTyep);
				    if (!strstr(Item[m_Hand].GetName(),"图谱"))
					{
                        msgshow("提示:材料不对,请放黄金图谱!");
						return;
					}
				   if (nGenre!=2|| nPart!=1 || nParticular<nPartTyep[0] || nParticular>nPartTyep[1])
				   {
					   msgshow("提示:材料不对,请放<color=yellow>黄金图谱<color>!");
					   ZeroMemory(nPartTyep,sizeof(nPartTyep));
					   return;
				   }
				
				}
				else if (SrcPos->nX==25)
				{//玄晶矿石
				   int nPartTyep[3];
				   ZeroMemory(nPartTyep,sizeof(nPartTyep));
				   g_CompEquip.GetInt3s(4,"Atlas",nPartTyep); //玄晶矿石
				   if (!strstr(Item[m_Hand].GetName(),"玄晶"))
					{
                        msgshow("提示:材料不对,请放玄晶矿石!");
						return;
					}
				   if (nGenre!=nPartTyep[0] || nPart!=nPartTyep[1] || nParticular!=nPartTyep[2])
				   {
					   msgshow("提示:材料不对,请放<color=yellow>玄晶矿石<color>!");
					   ZeroMemory(nPartTyep,sizeof(nPartTyep));
					   return;
				   }
				}
#endif
/*
			  if (m_Room[room_dazao].CheckSameDetailType(Item[m_Hand].GetGenre(), Item[m_Hand].GetDetailType(),Item[m_Hand].GetParticular()))
			  { 
#ifdef _SERVER
					BYTE byFinished = s2c_itemexchangefinish; //解锁
					if (g_pServer)
						g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, &byFinished, sizeof(BYTE));                   
					msgshow("提示:图谱栏已经有同类型的物品了！");
#endif
					return;
					
			  }  */
			
		   }
		   else if (nIsMianBam==6)    //提炼纹钢面板
		   {
#ifdef _SERVER
               g_GameSetTing.GetInteger("CompoundSet","IsOpenRonLian",0,&nIsOpen);
			   if (nIsOpen<=0)
			   {
				   msgshow("提示:<color=yellow>熔炼已经关闭!<color>!");
				   return;
			   }

			   if (SrcPos->nX==36)
			   {//大框
				   if (!Item[m_Hand].IsRong() || Item[m_Hand].GetIsWhere()!=0)
				   {
					   msgshow("提示:材料不对,请放<color=yellow>蓝紫黄装备(已纹钢)<color>!");
					   return;
				   }				   
			   }
			   else  if (SrcPos->nX==37)
			   {//小框 纹钢
				   if (!strstr(Item[m_Hand].GetName(),"玄火炭"))
				   {
					   msgshow("提示:材料不对,请放<color=yellow>玄火碳<color>!");
					   return;
				   }
				   
			   }
#endif
		   } 
		   else if (nIsMianBam==7)    //纹钢面板
		   {
			/* int nGenre=Item[m_Hand].GetGenre();                //类别
			   int nPart=Item[m_Hand].GetDetailType();            //详细类别
			   int nParticular= Item[m_Hand].GetParticular();     //具体类别
			   int nStackNum  = Item[m_Hand].GetStackNum();    */
#ifdef _SERVER
			   g_GameSetTing.GetInteger("CompoundSet","IsOpenWenGan",0,&nIsOpen);	   
			   if (nIsOpen<=0)
			   {
				   msgshow("提示:<color=yellow>纹钢已经关闭!<color>!");
				   return;
			   }

			   if (SrcPos->nX==34)
			   {//大框
				   if ((Item[m_Hand].GetGoldId()>0 || Item[m_Hand].IsBlue() || Item[m_Hand].IsPurple()) && Item[m_Hand].GetIsWhere()==0 && Item[m_Hand].GetPoint()!=7)
				   {

				   }
				   else
				   {
					   msgshow("提示:材料不对,请放<color=yellow>蓝紫黄装备(不可辨识)<color>!");
					   return;
				   }												 
			   }
			   else  if (SrcPos->nX==35)
			   {//小框 纹钢

                   if (Item[m_Hand].GetGenre()!=item_fusion)
			       {
					   	msgshow("提示:材料不对,请放<color=yellow>各种纹钢<color>!");
						return;
				   }

				   if (!strstr(Item[m_Hand].GetName(),"纹钢"))
				   {
					   msgshow("提示:材料不对,请放<color=yellow>各种纹钢<color>!");
				       return;
				   }

				   if (Item[m_Hand].GetSkillType()<=0)
				   {
					   msgshow("提示:材料不对,此纹钢<color=yellow>已经失效<color>!");
				       return;
				   }

			   }
#endif	   
		   }
		   else if (nIsMianBam==8)    //融合面板
		   {
#ifdef _SERVER
			   g_GameSetTing.GetInteger("CompoundSet","IsOpenRonHe",0,&nIsOpen);
			   
			   if (nIsOpen<=0)
			   {
				   msgshow("提示:<color=yellow>融合已经关闭!<color>!");
				   return;
			   }

			   if (SrcPos->nX==40)
			   {//小框
				   if (!Item[m_Hand].IsBlue())
				   {
					   msgshow("提示:材料不对,请放<color=yellow>蓝色装备<color>!");
					   return;
				   }
				 /*
				   if (Item[m_Hand].GetPoint()==7)
				   {
					   msgshow("提示:材料不对,请放<color=yellow>(未辨识)蓝色装备<color>!");
					   return;
				   } */
				   
			   }
			   else  if (SrcPos->nX==41)
			   {//小框
				  if (!Item[m_Hand].IsBlue())
				   {
					   msgshow("提示:材料不对,请放<color=yellow>蓝色装备<color>!");
					   return;
					}
				  /* if (Item[m_Hand].GetPoint()==7)
				   {
					   msgshow("提示:材料不对,请放<color=yellow>(未辨识)蓝色装备<color>!");
					   return;
				   }
				   */   
			   }
			   else  if (SrcPos->nX==42)
			   {//小框
				  if (!Item[m_Hand].IsBlue())
				   {
					   msgshow("提示:材料不对,请放<color=yellow>蓝色装备<color>!");
					   return;
					}
				   /*if (Item[m_Hand].GetPoint()==7)
				   {
					   msgshow("提示:材料不对,请放<color=yellow>(未辨识)蓝色装备<color>!");
					   return;
				   }*/   
			   }
			   else  if (SrcPos->nX==43)
			   {//小框
				  if (!Item[m_Hand].IsBlue())
				   {
					   msgshow("提示:材料不对,请放<color=yellow>蓝色装备<color>!");
					   return;
					}
				   /*if (Item[m_Hand].GetPoint()==7)
				   {
					   msgshow("提示:材料不对,请放<color=yellow>(未辨识)蓝色装备<color>!");
					   return;
				   }*/	   
			   }
			   else  if (SrcPos->nX==44)
			   {//小框
				  if (!Item[m_Hand].IsBlue())
				   {
					   msgshow("提示:材料不对,请放<color=yellow>蓝色装备<color>!");
					   return;
					}
				  /* if (Item[m_Hand].GetPoint()==7)
				   {
					   msgshow("提示:材料不对,请放<color=yellow>(未辨识)蓝色装备<color>!");
					   return;
				   }*/   
			   }
			   else  if (SrcPos->nX==45)
			   {//小框
				  if (!Item[m_Hand].IsBlue())
				   {
					   msgshow("提示:材料不对,请放<color=yellow>蓝色装备<color>!");
					   return;
					}
				   /*if (Item[m_Hand].GetPoint()==7)
				   {
					   msgshow("提示:材料不对,请放<color=yellow>(未辨识)蓝色装备<color>!");
					   return;
				   }*/	   
			   }
			   else  if (SrcPos->nX==46)
			   {//小框
				  if (!strstr(Item[m_Hand].GetName(),"融合"))
				   {
					   msgshow("提示:材料不对,请放<color=yellow>融合神石<color>!");
					   return;
			       }
				   
			   }
			   else  if (SrcPos->nX==47)
			   {//小框
				  if (!strstr(Item[m_Hand].GetName(),"猩红"))
				   {
					   msgshow("提示:材料不对,请放<color=yellow>猩红宝石<color>!");
					   return;
					}	   
			   }
#endif
		   }
		   else if (nIsMianBam==9)    //辨识装备面板
		   {
#ifdef _SERVER
			   g_GameSetTing.GetInteger("CompoundSet","IsOpenBianShi",0,&nIsOpen); 
			   if (nIsOpen<=0)
			   {
				   msgshow("提示:<color=yellow>辨识已经关闭!<color>!");
				   return;
			   }
			   if (SrcPos->nX==48)
			   {//大框
				   if (!Item[m_Hand].IsBlue())
				   {
					   msgshow("提示:材料不对,请放<color=yellow>蓝色装备<color>!");
					   return;
				   }

				   if (!Item[m_Hand].IsRong())
				   {
					   msgshow("提示:材料不对,请放<color=yellow>可辨识的装备<color>!");
					   return;
				   }

				   if (Item[m_Hand].GetIsWhere()!=-1)
				   {
					   msgshow("提示:材料不对,请放<color=yellow>未辨识的装备<color>!");
					   return;
				   }			   
				   
			   }
			   else  if (SrcPos->nX==49)
			   {//小框 纹钢
				   if (!strstr(Item[m_Hand].GetName(),"幸运星"))
				   {
					   msgshow("提示:材料不对,请放<color=yellow>幸运星石<color>!");
					   return;
				   }
			   }
#endif		      
		   }
		   else if (nIsMianBam==10)    //灵合宝石面板
		   {
#ifdef _SERVER
			   g_GameSetTing.GetInteger("CompoundSet","IsOpenXinChen",0,&nIsOpen);
			   
			   if (nIsOpen<=0)
			   {
				   msgshow("提示:<color=yellow>灵合宝石已经关闭!<color>!");
				   return;
			   }
			   if (SrcPos->nX==38)
			   {//大框
				   if (Item[m_Hand].GetGoldId()<=0)
				   {
					   msgshow("提示:材料不对,请放<color=yellow>黄金装备<color>!");
					   return;
				   }
/*				   if (!strstr(Item[m_Hand].GetName(),"披风") || nPart!=equip_pifeng)
				   {
#ifdef _SERVER
					   msgshow("提示:材料不对,请放<color=yellow>黄金披风<color>!");
#endif
					   return;
				   }*/				   
			   }
			   else  if (SrcPos->nX==39)
			   {//小框 纹钢
				   if (!strstr(Item[m_Hand].GetName(),"宝石"))
				   {
					   msgshow("提示:材料不对,请放<color=yellow>各种宝石<color>!");
					   return;
				   }   
			   }
#endif
		   }
		   else if (nIsMianBam==11)    //升级称号面板
		   {
#ifdef _SERVER
			   if (SrcPos->nX==50)
			   {//小框 纹钢
				   if (!strstr(Item[m_Hand].GetName(),"进阶石"))
				   {
					   msgshow("提示:材料不对,请放<color=yellow>进阶石<color>!");
					   return;
				   }			   
			   }
#endif
		   }
		   else if (nIsMianBam==12)    //神将
		   {
#ifdef _SERVER
			   if (SrcPos->nX==51)
			   {
				   if (!strstr(Item[m_Hand].GetName(),"元神石"))
				   {
					   msgshow("提示:材料不对,请放<color=yellow>元神石<color>!");
					   return;
				   }   
			   }
			   else  if (SrcPos->nX==52)
			   {
				   if (!strstr(Item[m_Hand].GetName(),"炼神丹"))
				   {
					   msgshow("提示:材料不对,请放<color=yellow>炼神丹<color>!");
					   return;
				   } 
			   }
			  /* else  if (SrcPos->nX==53)
			   {//同伴装备
				   if (!strstr(Item[m_Hand].GetName(),"攻丹"))
				   {
					   msgshow("提示:材料不对,请放<color=yellow>攻丹<color>!");
					   return;
				   }
				   
			   }
			   else  if (SrcPos->nX==54)
			   {
				   if (!strstr(Item[m_Hand].GetName(),"防丹"))
				   {
					   msgshow("提示:材料不对,请放<color=yellow>防丹<color>!");
					   return;
				   } 
			   }
			   else  if (SrcPos->nX==55)
			   {
				   if (!strstr(Item[m_Hand].GetName(),"命丹"))
				   {
					   msgshow("提示:材料不对,请放<color=yellow>命丹<color>!");
					   return;
				   } 
			   }
			   else  if (SrcPos->nX==56)
			   {
				   if (!strstr(Item[m_Hand].GetName(),"速丹"))
				   {
					   msgshow("提示:材料不对,请放<color=yellow>速丹<color>!");
					   return;
				   } 
			   }
			   else  if (SrcPos->nX==57)
			   {
				   if (!strstr(Item[m_Hand].GetName(),"隐丹"))
				   {
					   msgshow("提示:材料不对,请放<color=yellow>隐丹<color>!");
					   return;
				   } 
			   }*/
#endif
		   }
///////////////////同类检测////////////////////////
/*			if (m_Room[room_dazao].CheckSameDetailType(Item[m_Hand].GetGenre(), Item[m_Hand].GetDetailType()))
			{
#ifdef _SERVER
				BYTE	byFinished = s2c_itemexchangefinish; //解锁
				if (g_pServer)
					g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, &byFinished, sizeof(BYTE));
#endif
#ifndef _SERVER
				KSystemMessage	sMsg;
				sMsg.eType = SMT_NORMAL;
				sMsg.byConfirmType = SMCT_NONE;
				sMsg.byPriority = 0;
				sMsg.byParamSize = 0;
				sprintf(sMsg.szMessage,"%s", "提示:打造栏已经有同类型的物品了！");
				sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
#endif
				return;		
	    	}*/
		}
//===================单容器条件检测结束============================
        
		//nEquipIdx1 = m_Room[room_dazao].FindItem(SrcPos->nX, SrcPos->nY); 
         if (SrcPos->nX>0)
            nEquipIdx1=m_ItemBox[SrcPos->nX];  //如果需要放进的容器中有物品 
		 
        //m_Hand = nEquipIdx1;                              //手里装备的 索引
		//m_Items[FindSame(nEquipIdx1)].nPlace = pos_hand;  //赋值给该物品的位置信息 在手里
		// 先把东西拣起来
		if (nEquipIdx1>0)
		{////如果需要放进的容器中有物品 
			if (SrcPos->nX>=53 &&  SrcPos->nX<=77)
			{//如果是同伴装备栏 攻丹 开始卸载 原来的装备
				UnTonEquip(nEquipIdx1,SrcPos->nX);
				//  UnEquip()
				//  Equip();

			}
			else
			  m_ItemBox[SrcPos->nX]=0;
		}
		// 如果鼠标上有东西，先试试能不能把他放下去，如果不行的话，还得把原来的东西放回去
		if (m_Hand)
		{
		  if (DesPos->nX>=53 && DesPos->nX<=77)
		  {//同伴装备栏
			if (CanTonEquip(m_Hand, DesPos->nX)) //部位是否合适该装备
			{//检测是否可以装备
				TonEquip(m_Hand,DesPos->nX);  //换装
				m_Hand = nEquipIdx1;           //(拿起了源装备)
				m_Items[FindSame(nEquipIdx1)].nPlace = pos_hand;
                #ifdef _SERVER //发往客户端换装备
				//EquipAbrade(m_Hand,enumAbradeEquip,m_Items[FindSame(nEquipIdx1)].nX);//换装磨损
				g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
                #endif
			}
			else if (nEquipIdx1)
			{//否则保留原装备
				TonEquip(nEquipIdx1, SrcPos->nX);
			}
		  }
		  else
		  {//其他东西
			    int nListIdx = FindSame(m_Hand);
			    m_ItemBox[SrcPos->nX]=m_Hand;  //设置了这个位置的索引值为物品的索引
				m_Items[nListIdx].nPlace = pos_dazao;
				m_Items[nListIdx].nX = DesPos->nX;
				m_Items[nListIdx].nY = DesPos->nY;

				m_Hand = nEquipIdx1;   //手上的东西
				m_Items[FindSame(nEquipIdx1)].nPlace = pos_hand;

                #ifdef _SERVER
				g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
                #endif
		  }

		}
		else
		{// 如果手上没有东西，只需要把拣起来的东西放到鼠标上
			int nListIdx = FindSame(nEquipIdx1);
			if (nEquipIdx1 && nListIdx)
			{
				m_Items[nListIdx].nPlace = pos_hand;
				m_Hand = nEquipIdx1;
				m_ItemBox[SrcPos->nX]=0;  //设置源位置没有东西
			}
#ifdef _SERVER
			g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
		}
		break;
	case pos_cailiao:  //多对象容器
		if (Player[m_PlayerIdx].CheckTrading())	// 如果正在交易
			return;
    
		nEquipIdx1 = m_Room[room_cailiao].FindItem(SrcPos->nX,SrcPos->nY);

		if (nEquipIdx1 < 0) //数据出错的 就返回
			return;

        if (m_Hand)
		{
//==================多容器检测开始======================================
#ifdef _SERVER
			int nGenre=Item[m_Hand].GetGenre();                //类别
			int nPart=Item[m_Hand].GetDetailType();            //详细类别
			int nParticular= Item[m_Hand].GetParticular();     //具体类别
			int nStackNum  = Item[m_Hand].GetStackNum();       
			int nIsOpen=0;
			if (Item[m_Hand].GetTime()->bYear>0 || Item[m_Hand].GetIsBang()>0)
			{
				msgshow("提示:材料不对,材料不能限时/绑定!");
				return;
			}
#endif
            if (nIsMianBam==1)    //提炼面板
			{

			}
			else if (nIsMianBam==2)    //提取面板
			{
#ifdef _SERVER
               g_GameSetTing.GetInteger("CompoundSet","IsOpenTiqu",0,&nIsOpen);
				
				if (nIsOpen<=0)
				{
					msgshow("提示:<color=yellow>提取已经关闭!<color>!");
					return;
			    }

				int nVal=0;
			      nVal= CheckItemType(nGenre,nPart,nParticular,nIsMianBam);
			   if (nVal!=1)
			   { 

				   // int inVal[7];
                   //g_CompAtlas.GetInteger6(2,"3_GENRE","3_DETAILTYPE","3_PARTICULAR","3_LEVEL","3_SERIES","3_MAGIC_ID",0,inVal);				   
				   //char msg[64];
				   //sprintf(msg,"(%d)(%d)(%d)(%d)(%d)(%d)",inVal[0],inVal[1],inVal[2],inVal[3],inVal[4],inVal[5]);
				   msgshow("提示:可选材料不对,请看说明!");//
				   return;
			   }
#endif
			}
			else if (nIsMianBam==3)    //打造面板
			{
#ifdef _SERVER
				g_GameSetTing.GetInteger("CompoundSet","IsOpenDaZhao",0,&nIsOpen);
				
				if (nIsOpen<=0)
				{
					msgshow("提示:<color=yellow>打造已经关闭!<color>!");
					return;
			    }
#endif
			}
			else if (nIsMianBam==4)    //镶嵌面板
			{
#ifdef _SERVER
               g_GameSetTing.GetInteger("CompoundSet","IsOpenXiang",0,&nIsOpen);
				
				if (nIsOpen<=0)
				{
					msgshow("提示:<color=yellow>镶嵌已经关闭!<color>!");
					return;
			    }
                

				int nVal=0;
			      nVal= CheckItemType(nGenre,nPart,nParticular,nIsMianBam);
			   if (nVal!=1)
			   { 

				   msgshow("提示:可选材料不对,请看说明!");
                  return;
			   } 
#endif
			}
			else if (nIsMianBam==5)    //图谱面板
			{
#ifdef _SERVER
               g_GameSetTing.GetInteger("CompoundSet","IsOpenTuPu",0,&nIsOpen);
				
				if (nIsOpen<=0)
				{
					msgshow("提示:<color=yellow>图谱已经关闭!<color>!");
					return;
			    }
              
			   int nVal=0;
			      nVal= CheckItemType(nGenre,nPart,nParticular,nIsMianBam);
			   if (nVal!=1)
			   { 
				   msgshow("提示:可选材料不对,请看说明!");
                  return;
			   } 
#endif
			}
            else if (nIsMianBam==8)    //融合面板
			{
#ifdef _SERVER
               g_GameSetTing.GetInteger("CompoundSet","IsOpenRonHe",0,&nIsOpen);
				
				if (nIsOpen<=0)
				{
					msgshow("提示:<color=yellow>融合已经关闭!<color>!");
					return;
			    }
			   int nVal=0;
			      nVal= CheckItemType(nGenre,nPart,nParticular,nIsMianBam);
			   if (nVal!=1)
			   { 
				  msgshow("提示:可选材料不对,请看说明!");
                  return;
			   } 
#endif
			}

//==================多容器检测结束======================================
		}

		// 先把东西拣起来
		if (nEquipIdx1>0)
		{//清空容器的物品位置为0
			if (!m_Room[room_cailiao].PickUpItem(nEquipIdx1,SrcPos->nX, SrcPos->nY,Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight()))
				return;
		}	
		// 如果鼠标上有东西，先试试能不能把他放下去，如果不行的话，还得把原来的东西放回去
		if (m_Hand)
		{//物品放进容器时 检测并设定各参数
			if (m_Room[room_cailiao].PlaceItem(DesPos->nX,DesPos->nY, m_Hand, Item[m_Hand].GetWidth(), Item[m_Hand].GetHeight()))
			{
				int nListIdx = FindSame(m_Hand);
				m_Items[nListIdx].nPlace = pos_cailiao;
				m_Items[nListIdx].nX = DesPos->nX;
				m_Items[nListIdx].nY = DesPos->nY;
				//m_Items[nListIdx].nIsRongQi=1;  //是多对像容器
				m_Hand = nEquipIdx1;
				m_Items[FindSame(nEquipIdx1)].nPlace = pos_hand;
#ifdef _SERVER
				g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
			}
			// 把原来的东西放回去
			else
			{
				m_Room[room_cailiao].PlaceItem(SrcPos->nX, SrcPos->nY, nEquipIdx1, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight());
			}
		}
		// 如果鼠标上没有东西，只需要把拣起来的东西放到鼠标上
		else
		{
			int nListIdx = FindSame(nEquipIdx1);
			if (nEquipIdx1 && nListIdx)
			{
				m_Items[nListIdx].nPlace = pos_hand;
				m_Hand = nEquipIdx1;
			}
#ifdef _SERVER
			g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
		}
		break;
	}
#ifndef _SERVER
	// 界面处理
	if (m_Hand != nTempHand)	// 交换成功了
	{
	//uParam = (KUiObjAtRegion*)pInfo -> 物品数据及其放置区域位置信息
	//nParam = bAdd -> 0值表示减少这个物品，非0值表示增加这个物品
		char msg[64];

		KUiObjAtContRegion pInfo1, pInfo2;

		if (nTempHand == 0)
		{
			pInfo2.Obj.uGenre = CGOG_NOTHING;
			pInfo2.Obj.uId = 0;
			pInfo2.Region.Width = 0;
			pInfo2.Region.Height = 0;
		}
		else
		{
			pInfo2.Obj.uGenre = CGOG_ITEM;	//源装备
			pInfo2.Obj.uId = nTempHand;
			pInfo2.Region.Width = Item[nTempHand].GetWidth();
			pInfo2.Region.Height = Item[nTempHand].GetHeight();
		}
		
		if (m_Hand == 0)
		{
			pInfo1.Obj.uGenre = CGOG_NOTHING;
			pInfo1.Obj.uId = 0;
			pInfo1.Region.Width = 0;
			pInfo1.Region.Height = 0;
		}
		else
		{
			pInfo1.Obj.uGenre = CGOG_ITEM;
			pInfo1.Obj.uId = m_Hand;
			pInfo1.Region.Width = Item[m_Hand].GetWidth();
			pInfo1.Region.Height = Item[m_Hand].GetHeight();
		}

		int PartConvert[itempart_num] = 
		{
			UIEP_HEAD,
			UIEP_BODY,
			UIEP_WAIST,
			UIEP_HAND,
			UIEP_FOOT,
			UIEP_FINESSE,
			UIEP_NECK,
			UIEP_FINGER1,
			UIEP_FINGER2,
			UIEP_WAIST_DECOR,
			UIEP_HORSE,
			UIEP_MASK,	// mat na
			UIEP_PIFENG,	//披风
			UIEP_YINJIAN,		//印鉴
	        UIEP_SHIPING,	//饰品
		};
        //客户端对象信息赋值
		switch(SrcPos->nPlace)
		{
		case pos_immediacy:
			pInfo1.Region.h = SrcPos->nX;
			pInfo1.Region.v = SrcPos->nY;
			pInfo2.Region.h = DesPos->nX;
			pInfo2.Region.v = DesPos->nY;
			pInfo1.eContainer = UOC_IMMEDIA_ITEM;
			pInfo2.eContainer = UOC_IMMEDIA_ITEM;
			pInfo1.Region.IsRongqi = SrcPos->nSIsRongqi;
			pInfo2.Region.IsRongqi = DesPos->nDIsRongqi;
			break;
		case pos_equiproom:
			pInfo1.Region.h = SrcPos->nX;
			pInfo1.Region.v = SrcPos->nY;
			pInfo2.Region.h = DesPos->nX;
			pInfo2.Region.v = DesPos->nY;
			pInfo1.eContainer = UOC_ITEM_TAKE_WITH;
			pInfo2.eContainer = UOC_ITEM_TAKE_WITH;
			pInfo1.Region.IsRongqi = SrcPos->nSIsRongqi;
			pInfo2.Region.IsRongqi = DesPos->nDIsRongqi;
			//sprintf(msg,"[KR_equiproom] X:%d-Y:%d",DesPos->nX,DesPos->nY);
			//ClientShowMsg(msg);
			break;
		case pos_equip:
			pInfo1.Region.h = 0;
			pInfo1.Region.v = PartConvert[SrcPos->nX];
			pInfo2.Region.h = 0;
			pInfo2.Region.v = PartConvert[DesPos->nX];
			pInfo1.eContainer = UOC_EQUIPTMENT;
			pInfo2.eContainer = UOC_EQUIPTMENT;
			pInfo1.Region.IsRongqi = SrcPos->nSIsRongqi;
			pInfo2.Region.IsRongqi = DesPos->nDIsRongqi;
			break;
		case pos_repositoryroom:
			pInfo1.Region.h = SrcPos->nX;
			pInfo1.Region.v = SrcPos->nY;
			pInfo2.Region.h = DesPos->nX;
			pInfo2.Region.v = DesPos->nY;
			pInfo1.eContainer = UOC_STORE_BOX;
			pInfo2.eContainer = UOC_STORE_BOX;
			pInfo1.Region.IsRongqi = SrcPos->nSIsRongqi;
			pInfo2.Region.IsRongqi = DesPos->nDIsRongqi;
			break;
		case pos_exbox1room: // ruong mo rong 1
			pInfo1.Region.h = SrcPos->nX;
			pInfo1.Region.v = SrcPos->nY;
			pInfo2.Region.h = DesPos->nX;
			pInfo2.Region.v = DesPos->nY;
			pInfo1.eContainer = UOC_EX_BOX1;
			pInfo2.eContainer = UOC_EX_BOX1;
			pInfo1.Region.IsRongqi = SrcPos->nSIsRongqi;
			pInfo2.Region.IsRongqi = DesPos->nDIsRongqi;
			break;
		case pos_exbox2room: // ruong mo rong 2
			pInfo1.Region.h = SrcPos->nX;
			pInfo1.Region.v = SrcPos->nY;
			pInfo2.Region.h = DesPos->nX;
			pInfo2.Region.v = DesPos->nY;
			pInfo1.eContainer = UOC_EX_BOX2;
			pInfo2.eContainer = UOC_EX_BOX2;
			pInfo1.Region.IsRongqi = SrcPos->nSIsRongqi;
			pInfo2.Region.IsRongqi = DesPos->nDIsRongqi;
			break;
		case pos_exbox3room: // ruong mo rong 3
			pInfo1.Region.h = SrcPos->nX;
			pInfo1.Region.v = SrcPos->nY;
			pInfo2.Region.h = DesPos->nX;
			pInfo2.Region.v = DesPos->nY;
			pInfo1.eContainer = UOC_EX_BOX3;
			pInfo2.eContainer = UOC_EX_BOX3;
			pInfo1.Region.IsRongqi = SrcPos->nSIsRongqi;
			pInfo2.Region.IsRongqi = DesPos->nDIsRongqi;
			break;
		case pos_equiproomex: // mo rong hanh trang
			pInfo1.Region.h = SrcPos->nX;
			pInfo1.Region.v = SrcPos->nY;
			pInfo2.Region.h = DesPos->nX;
			pInfo2.Region.v = DesPos->nY;
			pInfo1.eContainer = UOC_ITEM_EX;
			pInfo2.eContainer = UOC_ITEM_EX;
			pInfo1.Region.IsRongqi = SrcPos->nSIsRongqi;
			pInfo2.Region.IsRongqi = DesPos->nDIsRongqi;
			break;
		case pos_traderoom:
			pInfo1.Region.h = SrcPos->nX;
			pInfo1.Region.v = SrcPos->nY;
			pInfo2.Region.h = DesPos->nX;
			pInfo2.Region.v = DesPos->nY;
			pInfo1.eContainer = UOC_TO_BE_TRADE;
			pInfo2.eContainer = UOC_TO_BE_TRADE;
			pInfo1.Region.IsRongqi = SrcPos->nSIsRongqi;
			pInfo2.Region.IsRongqi = DesPos->nDIsRongqi;
			break;
		case pos_give:
			pInfo1.Region.h = SrcPos->nX;
			pInfo1.Region.v = SrcPos->nY;
			pInfo2.Region.h = DesPos->nX;
			pInfo2.Region.v = DesPos->nY;
			pInfo1.eContainer = UOC_ITEM_GIVE;
			pInfo2.eContainer = UOC_ITEM_GIVE;
			pInfo1.Region.IsRongqi = SrcPos->nSIsRongqi;
			pInfo2.Region.IsRongqi = DesPos->nDIsRongqi;
			break;
		case pos_dazao:
			pInfo1.Region.h = SrcPos->nX;
			pInfo1.Region.v = SrcPos->nY;
			pInfo2.Region.h = DesPos->nX;
			pInfo2.Region.v = DesPos->nY;
			pInfo1.eContainer = UOC_ITEM_DAZAO;
			pInfo2.eContainer = UOC_ITEM_DAZAO;
			pInfo1.Region.IsRongqi = 0;//SrcPos->nSIsRongqi;
			pInfo2.Region.IsRongqi = 0;//DesPos->nDIsRongqi;
			//sprintf(msg,"[KR_dazao] X:%d-Y:%d",DesPos->nX,DesPos->nY);
			//ClientShowMsg(msg);
			break;
		case pos_cailiao:
			pInfo1.Region.h = SrcPos->nX;
			pInfo1.Region.v = SrcPos->nY;
			pInfo2.Region.h = DesPos->nX;
			pInfo2.Region.v = DesPos->nY;
			pInfo1.eContainer = UOC_ITEM_CAILIAO;
			pInfo2.eContainer = UOC_ITEM_CAILIAO;
			pInfo1.Region.IsRongqi = 1;
			pInfo2.Region.IsRongqi = 1;
			break;
		}
		//开始交换物品
	       //设置减少
		   CoreDataChanged(GDCNI_OBJECT_CHANGED, (DWORD)&pInfo1, 0);//容器减少
		   CoreDataChanged(GDCNI_HOLD_OBJECT, (DWORD)&pInfo2, 0);   //手里的东西减少
           //设置增加
		   CoreDataChanged(GDCNI_HOLD_OBJECT, (DWORD)&pInfo1, 1);   //手里增加 
		   CoreDataChanged(GDCNI_OBJECT_CHANGED, (DWORD)&pInfo2, 1);//容器增加
	}
#endif
	if (Npc[Player[m_PlayerIdx].m_nIndex].m_Doing == do_sit)
	{
		Npc[Player[m_PlayerIdx].m_nIndex].SendSerCommand(do_stand);
	}
}

#ifdef _SERVER
BOOL KItemList::CheckTask(int nYid,char *nKeyName)
{
    KTabFile Mine;
    Mine.Load("\\Settings\\ItemQuest.txt");  //是否带有魔法属性的mine物品列表
	int nParticular=0,nMagciType=0;
	int nReg=0,i;
	for (i=0;i<Mine.GetHeight()-1;++i)
	{
		Mine.GetInteger(i+2,nKeyName,0,&nParticular);
		//Mine.GetInteger(i+2,"nMagciType",0,&nMagciType);
		if (nParticular==nYid)
		{  
			nReg=1;
			break;
		} 
	}

    Mine.Clear();
	return nReg;
}
#endif


//多容器材料检测 
BOOL KItemList::CheckItemType(int ItemClass,int Detail,int nPart,int IsMianBan)
{
	  int nPartTyep[3],nReg=FALSE;
	  ZeroMemory(nPartTyep,sizeof(nPartTyep));
      int nRows=g_CompEquip.GetHeight();
	  for(int i=11;i<nRows;++i)
	  {
		  if (IsMianBan==1)
		  {//提炼
				  break;
		  } 
		  else if (IsMianBan==2)
		  {//提取
			  g_CompEquip.GetInt3s(i,"Distill",nPartTyep); //玄晶矿石
			  if (ItemClass==nPartTyep[0] && Detail==nPartTyep[1] && nPart==nPartTyep[2])
			  { 
				  nReg=TRUE;
				  break;
			  } 
		  } 
		  else if (IsMianBan==3)
		  {//打造
				  break;	  
		  } 
		  else if (IsMianBan==4)
		  {//镶套
			  g_CompEquip.GetInt3s(i,"Enchase",nPartTyep); //玄晶矿石
			  if (ItemClass==nPartTyep[0] && Detail==nPartTyep[1] && nPart==nPartTyep[2])
			  { 
				  nReg=TRUE;
				  break;
			  } 
		  } 
		  else if (IsMianBan==5)
		  {//图谱
             g_CompEquip.GetInt3s(i,"Atlas",nPartTyep); //玄晶矿石
		     if (ItemClass==nPartTyep[0] && Detail==nPartTyep[1] && nPart==nPartTyep[2])
			 { 
			   nReg=TRUE;
			   break;
			 } 
		  } 
		  else if (IsMianBan==8)
		  {//融合材料
             g_CompEquip.GetInt3s(i,"RongHe",nPartTyep);
		     if (ItemClass==nPartTyep[0] && Detail==nPartTyep[1] && nPart==nPartTyep[2])
			 { 
			   nReg=TRUE;
			   break;
			 } 
		  }
		  else
			  break;

	  }
	  return nReg;
}



#ifndef	_SERVER
//---------------------------------------------------------------------
//	功能：物品从一个地方直接移动到另一个地方，不经过鼠标这个中间过程
//---------------------------------------------------------------------
BOOL	KItemList::AutoMoveItem(ItemPos SrcPos,ItemPos DesPos)
{
	if (Player[this->m_PlayerIdx].CheckTrading())	// 如果正在交易
		return FALSE;

	BOOL	bMove = FALSE;
	int		nIdx, nListIdx;

	// 目前只支持从room_equipment到room_immediacy
	switch (SrcPos.nPlace) // 源容器
	{
	case pos_equiproom:
		{
			switch (DesPos.nPlace)  //目标容器
			{
			case pos_immediacy:
				{
					nIdx = m_Room[room_equipment].FindItem(SrcPos.nX, SrcPos.nY);
					if (nIdx <= 0)
						return FALSE;
					if (Item[nIdx].GetGenre() != item_medicine)
					{
						_ASSERT(0);
						return FALSE;
					}
					if (!m_Room[room_equipment].PickUpItem(nIdx, SrcPos.nX, SrcPos.nY, Item[nIdx].GetWidth(), Item[nIdx].GetHeight()))
						return FALSE;
					m_Room[room_immediacy].PlaceItem(DesPos.nX, DesPos.nY, nIdx, Item[nIdx].GetWidth(), Item[nIdx].GetHeight());
					nListIdx = FindSame(nIdx);
					if (nListIdx <= 0)
						return FALSE;
					m_Items[nListIdx].nPlace = pos_immediacy;
					m_Items[nListIdx].nX = DesPos.nX;
					m_Items[nListIdx].nY = DesPos.nY;
					bMove = TRUE;
				}
				break;
			case pos_dazao:
				{
					nIdx = m_Room[room_equipment].FindItem(SrcPos.nX, SrcPos.nY);
					if (nIdx <= 0)
						return FALSE;
					/*if (Item[nIdx].GetGenre() != item_medicine)
					{//物品限制
						_ASSERT(0);
						return FALSE;
					}*/
					//清空客户端容器位置为0
					if (!m_Room[room_equipment].PickUpItem(nIdx, SrcPos.nX, SrcPos.nY, Item[nIdx].GetWidth(), Item[nIdx].GetHeight()))
						return FALSE;
					//设置客户端容器位置为idx
					//m_Room[room_dazao].PlaceItem(DesPos.nX, DesPos.nY, nIdx, Item[nIdx].GetWidth(), Item[nIdx].GetHeight());
					nListIdx = FindSame(nIdx);
					if (nListIdx <= 0)
						return FALSE;
                    m_ItemBox[DesPos.nX]=nIdx;
					m_Items[nListIdx].nPlace = pos_dazao;
					m_Items[nListIdx].nX = DesPos.nX;
					m_Items[nListIdx].nY = DesPos.nY;
					
					bMove = TRUE;
				}
				break;
			case pos_cailiao:
				{
					nIdx = m_Room[room_equipment].FindItem(SrcPos.nX, SrcPos.nY);
					if (nIdx <= 0)
						return FALSE;
					if (Item[nIdx].GetGenre() != item_medicine)
					{
						_ASSERT(0);
						return FALSE;
					}
					if (!m_Room[room_equipment].PickUpItem(nIdx, SrcPos.nX, SrcPos.nY, Item[nIdx].GetWidth(), Item[nIdx].GetHeight()))
						return FALSE;
					m_Room[room_cailiao].PlaceItem(DesPos.nX, DesPos.nY, nIdx, Item[nIdx].GetWidth(), Item[nIdx].GetHeight());
					nListIdx = FindSame(nIdx);
					if (nListIdx <= 0)
						return FALSE;
					m_Items[nListIdx].nPlace = pos_cailiao;
					m_Items[nListIdx].nX = DesPos.nX;
					m_Items[nListIdx].nY = DesPos.nY;
					bMove = TRUE;
				}
				break;
			}
		}
		break;
	case pos_dazao:  //源单对象容器
		{ 
			switch (DesPos.nPlace)  //目标容器
			{
			   case pos_equiproom:
				{
					nIdx = m_ItemBox[SrcPos.nX];//m_Room[room_dazao].FindItem(SrcPos.nX, SrcPos.nY);
					if (nIdx <= 0)
						return FALSE;
					//清空源容器位置为0
			/*		if (!m_Room[room_dazao].PickUpItem(nIdx, SrcPos.nX, SrcPos.nY, Item[nIdx].GetWidth(), Item[nIdx].GetHeight()))
						return FALSE;
					//设置目标容器位置为Idx
				*/	
				       m_ItemBox[SrcPos.nX]=0;

					if (!m_Room[room_equipment].PlaceItem(DesPos.nX,DesPos.nY,nIdx,Item[nIdx].GetWidth(),Item[nIdx].GetHeight()))
					{
					   ClientShowMsg("提示:包袱空间有限C!");
					   return FALSE;
					}
					nListIdx = FindSame(nIdx);
					if (nListIdx <= 0)
						return FALSE;
					m_Items[nListIdx].nPlace = pos_equiproom;
					m_Items[nListIdx].nX = DesPos.nX;
					m_Items[nListIdx].nY = DesPos.nY;
					bMove = TRUE;
				} 
				break;
			}	
		} 
		break;
	case pos_cailiao: //源多对象容器
		{ 
			switch (DesPos.nPlace)  //目标容器
			{
			 case pos_equiproom:
				 {
					 nIdx = m_Room[room_cailiao].FindItem(SrcPos.nX, SrcPos.nY);
					 if (nIdx <= 0)
						 return FALSE;
					 //清空源客户端容器位置为0
					 if (!m_Room[room_cailiao].PickUpItem(nIdx, SrcPos.nX, SrcPos.nY, Item[nIdx].GetWidth(), Item[nIdx].GetHeight()))
					 {
						 //ClientShowMsg("提示:容器清空失败!");
						 return FALSE;
					 }
					 //设置客户端目标容器位置
					    if(!m_Room[room_equipment].PlaceItem(DesPos.nX, DesPos.nY, nIdx, Item[nIdx].GetWidth(), Item[nIdx].GetHeight()))
						{
					      ClientShowMsg("提示:包袱空间有限C!");
					      return FALSE;
					    }
					    
					 nListIdx = FindSame(nIdx);
					 if (nListIdx <= 0)
						 return FALSE;
					 m_Items[nListIdx].nPlace = pos_equiproom; //设置客户端物品目标容器信息
					 m_Items[nListIdx].nX = DesPos.nX;
					 m_Items[nListIdx].nY = DesPos.nY;
					 bMove = TRUE;
				 } 
				 break;
			} 
		} 
		break;
	}

	if (!bMove)
		return bMove;

	// 通知界面
	KUiObjAtContRegion sSrcInfo, sDestInfo;

	sSrcInfo.Obj.uGenre		= CGOG_ITEM;
	sSrcInfo.Obj.uId		= nIdx;
	sSrcInfo.Region.Width	= Item[nIdx].GetWidth();
	sSrcInfo.Region.Height	= Item[nIdx].GetHeight();
	sSrcInfo.Region.h		= SrcPos.nX;
	sSrcInfo.Region.v		= SrcPos.nY;

	if (SrcPos.nPlace == pos_immediacy)
		sSrcInfo.eContainer	= UOC_IMMEDIA_ITEM;

	else if (SrcPos.nPlace == pos_dazao)
	{
		sSrcInfo.Region.IsRongqi=0;
		sSrcInfo.eContainer	= UOC_ITEM_DAZAO;
	}
	else if (SrcPos.nPlace == pos_cailiao)
	{
        sSrcInfo.Region.IsRongqi=1;
	    sSrcInfo.eContainer	= UOC_ITEM_CAILIAO;
	}
    else if (SrcPos.nPlace == pos_equiproom)
	    sSrcInfo.eContainer = UOC_ITEM_TAKE_WITH;

	sDestInfo.Obj.uGenre	= CGOG_ITEM;
	sDestInfo.Obj.uId		= nIdx;
	sDestInfo.Region.Width	= Item[nIdx].GetWidth();
	sDestInfo.Region.Height	= Item[nIdx].GetHeight();
	sDestInfo.Region.h		= DesPos.nX;
	sDestInfo.Region.v		= DesPos.nY;
	if (DesPos.nPlace == pos_immediacy)
	    sDestInfo.eContainer	= UOC_IMMEDIA_ITEM;

	else if (DesPos.nPlace == pos_dazao)
	{
		sDestInfo.Region.IsRongqi=0;
	    sDestInfo.eContainer	= UOC_ITEM_DAZAO;
	}
	else if (DesPos.nPlace == pos_cailiao)
	{
		sDestInfo.Region.IsRongqi=1;
	    sDestInfo.eContainer	= UOC_ITEM_CAILIAO;
	}
	else if (DesPos.nPlace == pos_equiproom)
	    sDestInfo.eContainer    = UOC_ITEM_TAKE_WITH;

	CoreDataChanged(GDCNI_OBJECT_CHANGED, (DWORD)&sSrcInfo,0);  //源容器消除图标

	CoreDataChanged(GDCNI_OBJECT_CHANGED, (DWORD)&sDestInfo,1); //目标容器增加图标

	return bMove;
}
#endif
//客户端显示公告消息
#ifndef	_SERVER
void	KItemList::ClientShowMsg(char *Msg)
{
	   
	KSystemMessage	sMsg;
	sMsg.byConfirmType = SMCT_NONE;
	sMsg.eType = SMT_NORMAL;
	sMsg.byParamSize = 0;
	sMsg.byPriority = 0;

	if (sizeof(Msg)<125)
	{
	  snprintf(sMsg.szMessage, sizeof(sMsg.szMessage), "%s", Msg);
	}
	else
	  sprintf(sMsg.szMessage,"警告:<colro=gyellow>信息过长,取消发送<color>");

	sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
	CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
}
#endif



#ifndef	_SERVER
//---------------------------------------------------------------------
//	功能：物品从一个地方直接移动到另一个地方，不经过鼠标这个中间过程
//---------------------------------------------------------------------
void	KItemList::MenuSetMouseItem()
{
	KUiObjAtContRegion	sInfo;
	if (!m_Hand)
	{
		CoreDataChanged(GDCNI_HOLD_OBJECT, 0, 0);
	}
	else
	{
		sInfo.Obj.uGenre = CGOG_ITEM;
		sInfo.Obj.uId = m_Hand;
		sInfo.Region.Width = Item[m_Hand].GetWidth();
		sInfo.Region.Height = Item[m_Hand].GetHeight();
		sInfo.Region.h = 0;
		sInfo.Region.v = 0;
		sInfo.eContainer = UOC_IN_HAND;		
		CoreDataChanged(GDCNI_HOLD_OBJECT, (DWORD)&sInfo, 0);
	}
}
#endif
//服务器端执行 使用物品 
#ifdef _SERVER
BOOL KItemList::EatMecidineC(int nPlace, int nX, int nY)  //目标容器 和位置
{
	int nItemIdx = 0;
	switch(nPlace)
	{
	case pos_equiproom:  //包袱
		nItemIdx = m_Room[room_equipment].FindItem(nX, nY);
		if (nItemIdx > 0)
			return EatMecidineS(nItemIdx,nX,nY);
		break;
	case pos_immediacy: //快捷栏
		nItemIdx = m_Room[room_immediacy].FindItem(nX, nY);
		if (nItemIdx > 0)
		{
			if (Item[nItemIdx].GetGenre()!= item_medicine)
				return EatMecidineS(nItemIdx,nX,nY);
			//如果是药品
			int		nGenre, nDetailType,nParticularType, nIdx, nXpos, nYpos;
			BOOL	bEat;

			nGenre = item_medicine;
			nDetailType = Item[nItemIdx].GetDetailType();
			nParticularType = 0;
			bEat = EatMecidineS(nItemIdx,nX,nY);
			if (bEat == FALSE)
				return bEat;
            //查找是否还有同样的物品
			if (FALSE == FindSameDetailTypeInEquipment(nGenre, nDetailType,nParticularType, &nIdx, &nXpos, &nYpos))
				return bEat;

			this->AutoMoveMedicine(nIdx, nXpos, nYpos, nX, nY);  //药品自动移动到 快捷栏目

			return bEat;
		}
		break;
	default:
		break;
	}

	return FALSE;
}
//客户端要求服务器端移动物品
BOOL KItemList::serAutoMoveItem(int nItemIdx, int nsrcPlace, int nscrX, int nscrY,int destRoom)
{
	//int nItemIdx = 0;
	int  nSrcRoom=0,nDesRoom=0;
	if (nsrcPlace==pos_repositoryroom)
		nSrcRoom = room_repository;
	else if (nsrcPlace==pos_exbox1room)
		nSrcRoom = room_exbox1;
	else if (nsrcPlace==pos_exbox2room)
		nSrcRoom = room_exbox2;
	else if (nsrcPlace==pos_exbox3room)
		nSrcRoom = room_exbox3;
	else if (nsrcPlace==pos_equiproomex)
		nSrcRoom = room_equipmentex;
	else if (nsrcPlace==pos_equiproom)
		nSrcRoom = room_equipment;
	else if (nsrcPlace==pos_equip)     //如果是身上的装备要移动到包袱
		nSrcRoom = room_equipment;
	else if (nsrcPlace==pos_immediacy) //如果是身上的装备要移动到包袱
		nSrcRoom = room_immediacy;
	else if (nsrcPlace==pos_give) 
		nSrcRoom = room_give;
	else if (nsrcPlace==pos_traderoom) 
		nSrcRoom = room_trade;
	/*if (DesPos.nPlace==pos_repositoryroom)
		nDesRoom = room_repository;
	else if (DesPos.nPlace==pos_exbox1room)
		nDesRoom = room_exbox1;
	else if (DesPos.nPlace==pos_exbox2room)
		nDesRoom = room_exbox2;
	else if (DesPos.nPlace==pos_exbox3room)
		nDesRoom = room_exbox3;
	else if (DesPos.nPlace==pos_equiproomex)
		nDesRoom = room_equipmentex;
	else if (DesPos.nPlace==pos_equiproom)
		nDesRoom = room_equipment;
	*/



	switch(nsrcPlace)    //源容器
	{
	case pos_equip: 
		break;
	case pos_give:
	case pos_repositoryroom:	// 贮物箱
	case pos_exbox1room:		// mo rong ruong 1  扩展箱
	case pos_exbox2room:		// mo rong ruong 2
	case pos_exbox3room:		// mo rong ruong 3
	case pos_equiproomex:	    // 同伴包袱
	case pos_equiproom:         // 包袱 用于右键换装备
	case pos_traderoom:         // 源容器
		//nItemIdx = m_Room[room_equipment].FindItem(nscrX, nscrY);
		if (nItemIdx>0 && nItemIdx<MAX_ITEM)
		{
			int	ndestX, ndestY;
			//判断目标容器是否有空间
			if (CheckCanPlaceInEquipment(Item[nItemIdx].GetWidth(), Item[nItemIdx].GetHeight(),&ndestX, &ndestY,destRoom))
				this->AutoMoveItems(nItemIdx, nscrX, nscrY, ndestX,ndestY,nSrcRoom,destRoom);  // 快捷栏目
			else
                 msgshow("Auto: Ruong day khong the di chuyen do 1");
			return 1;
		}
		break;
	case pos_immediacy: //快捷栏
		//nItemIdx = m_Room[room_immediacy].FindItem(nscrX, nscrY);
		if (nItemIdx > 0 && nItemIdx<MAX_ITEM)
		{
			int	ndestX, ndestY;
			if (CheckCanPlaceInEquipment(Item[nItemIdx].GetWidth(), Item[nItemIdx].GetHeight(), &ndestX, &ndestY))
				this->AutoMoveItems(nItemIdx, nscrX, nscrY, ndestX, ndestY,room_immediacy,destRoom);  // 快捷栏目
			else
				msgshow("Auto: Ruong day khong the di chuyen do 2");
			return 1;
		}
		break;
	case pos_dazao: //单对象源容器
		if (nItemIdx > 0 && nItemIdx<MAX_ITEM)
		{
			int	ndestX, ndestY;

			if (CheckCanPlaceInEquipment(Item[nItemIdx].GetWidth(), Item[nItemIdx].GetHeight(), &ndestX, &ndestY))
				this->AutoMoveItems(nItemIdx, nscrX, nscrY, ndestX, ndestY,room_dazao,destRoom);
			else
				msgshow("Auto: Ruong day khong the di chuyen do 3");
			return 1;
		}
		break;
	case pos_cailiao: //多对象源容器
		//nItemIdx = m_Room[room_cailiao].FindItem(nscrX, nscrY);
		if (nItemIdx > 0 && nItemIdx<MAX_ITEM)
		{
			int	ndestX, ndestY;
			if (CheckCanPlaceInEquipment(Item[nItemIdx].GetWidth(), Item[nItemIdx].GetHeight(), &ndestX, &ndestY))
				//检测包袱空间
				this->AutoMoveItems(nItemIdx, nscrX, nscrY, ndestX, ndestY,room_cailiao,destRoom);
			else
				msgshow("Auto: Ruong day khong the di chuyen do 4");
			return 1;
		}
		break;
	default:
		break;
	}
	
	return FALSE;
}

void KItemList::msgshow(char *msg,int isCover) //47
{
   SHOW_MSG_SYNC	sMsg;
   sMsg.ProtocolType = s2c_msgshow;
   sMsg.m_wMsgID     = -1;
   sMsg.m_lpBuf      = isCover;

	if (sizeof(msg)<125)
	{
		sprintf(sMsg.m_Mmsgs, "%s", msg);
	}
	else
	{
		sprintf(sMsg.m_Mmsgs, "%s", "Canh bao:<c=green>tin nhan qua dai, khong gui<c>");
	}

   //sprintf(sMsg.m_Mmsgs,"%s",msg);
   sMsg.m_wLength    = sizeof(SHOW_MSG_SYNC) - 1;
   g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx,(BYTE *)&sMsg, sMsg.m_wLength + 1);
}


#endif

PlayerItem* KItemList::GetFirstItem()
{
	m_nListCurIdx = m_UseIdxItemList.GetNext(0);
	return &m_Items[m_nListCurIdx];
}

PlayerItem* KItemList::GetNextItem()
{
	if ( !m_nListCurIdx )
		return NULL;
	m_nListCurIdx = m_UseIdxItemList.GetNext(m_nListCurIdx);
	return &m_Items[m_nListCurIdx];
}

void	KItemList::ClearRoom(int nRoom)
{
	if (nRoom >= 0 && nRoom < room_num)
		this->m_Room[nRoom].Clear();
}


void	KItemList::BackupTrade()
{
	if ( !m_Room[room_tradeback].m_pArray )
		m_Room[room_tradeback].Init(m_Room[room_equipment].m_nWidth, m_Room[room_equipment].m_nHeight);

	ZeroMemory(this->m_sBackItems, sizeof(PlayerItem) * MAX_PLAYER_ITEM);

	memcpy(m_Room[room_tradeback].m_pArray, m_Room[room_equipment].m_pArray, sizeof(int) * m_Room[room_tradeback].m_nWidth * m_Room[room_tradeback].m_nHeight);
	memcpy(this->m_sBackItems, this->m_Items, sizeof(PlayerItem) * MAX_PLAYER_ITEM);

	m_nBackHand = m_Hand;
}

void	KItemList::RecoverTrade()
{
	memcpy(m_Room[room_equipment].m_pArray, m_Room[room_tradeback].m_pArray, sizeof(int) * m_Room[room_tradeback].m_nWidth * m_Room[room_tradeback].m_nHeight);

#ifndef _SERVER
	int nIdx = 0;
	while((nIdx = m_UseIdxItemList.GetNext(nIdx)))
	{
		if (m_Items[nIdx].nPlace == pos_trade1)   //删除对方的
		{
			Remove(m_Items[nIdx].nIdx);
			//ItemSet.Remove(m_Items[nIdx].nIdx);
		}
	}
/*	for (int i = 0; i < MAX_PLAYER_ITEM; ++i)
	{
		if (m_Items[i].nIdx && m_Items[i].nPlace == pos_trade1)
			Remove(m_Items[i].nIdx);
	}*/
#endif
	memcpy(m_Items, m_sBackItems, sizeof(PlayerItem) * MAX_PLAYER_ITEM);
	m_Hand = m_nBackHand;
}
//开始交易时 清楚所有交易框的 东西
void	KItemList::StartTrade()
{
	BackupTrade();
	ClearRoom(room_trade);
	ClearRoom(room_trade1);
}

/*!*****************************************************************************
// Function		: KItemList::RemoveAll
// Purpose		: 退出时清除所有的装备
// Return		: void
// Comments		: 会实际地从游戏世界中的道具数组中去掉
// Author		: Spe
*****************************************************************************/
void KItemList::RemoveAll()
{	
   //删除物品前生成一个数据包,用于存档
/*#ifdef _SERVER
	Player[m_PlayerIdx].Save(TRUE);
   //------
#endif*/

	int nIdx = m_UseIdxItemList.GetNext(0);
	int nIdx1 = 0;
	while(nIdx)
	{
		nIdx1 = m_UseIdxItemList.GetNext(nIdx);
		int nGameIdx = m_Items[nIdx].nIdx;
		Remove(m_Items[nIdx].nIdx,0,0,0,FALSE);//只删除，不生成存档包

#ifdef _SERVER
		// 客户端在上面KItemList::Remove()已经做了ItemSet.Remove()
		ItemSet.Remove(nGameIdx);
#endif
		nIdx = nIdx1;
	}

	ZeroMemory(m_EquipItem, sizeof(m_EquipItem));		
	ZeroMemory(m_sBackItems, sizeof(m_sBackItems));
	ZeroMemory(m_Items,sizeof(m_Items));

	m_Room[room_equipment].Clear();
	m_Room[room_repository].Clear();
	m_Room[room_trade].Clear();
	m_Room[room_exbox1].Clear();
	m_Room[room_exbox2].Clear();
	m_Room[room_exbox3].Clear();
	m_Room[room_equipmentex].Clear();
	m_Room[room_tradeback].Clear();
	m_Room[room_immediacy].Clear();
	m_Room[room_give].Clear();
	m_Room[room_giveback].Clear();
	m_Room[room_cailiao].Clear();
}

void KItemList::NewRemoveAll(bool isSave)
{	
	int nIdx = m_UseIdxItemList.GetNext(0);
	int nIdx1 = 0;
	while(nIdx)
	{
		nIdx1 = m_UseIdxItemList.GetNext(nIdx);
		int nGameIdx = m_Items[nIdx].nIdx;

		if  (m_Items[nIdx].nPlace!=pos_equip)
		{
			Remove(m_Items[nIdx].nIdx,0,0,0,isSave);//只删除，不生成存档包
#ifdef _SERVER
			// 客户端在上面KItemList::Remove()已经做了ItemSet.Remove()
			ItemSet.Remove(nGameIdx);
#endif
		}

		nIdx = nIdx1;
	}
}

int KItemList::GetWeaponParticular()
{
	if (m_EquipItem[itempart_weapon])
	{
        if (Item[m_EquipItem[itempart_weapon]].GetDetailType()==0  && Item[m_EquipItem[itempart_weapon]].GetParticular()<=6)
		{//武器
		    return Item[m_EquipItem[itempart_weapon]].GetParticular();
		}
		else if (Item[m_EquipItem[itempart_weapon]].GetDetailType()==0)
		{//武器
            return Item[m_EquipItem[itempart_weapon]].GetParticular2();
		}

		if (Item[m_EquipItem[itempart_weapon]].GetDetailType()==1  && Item[m_EquipItem[itempart_weapon]].GetParticular()<=2)
		{//暗器
			return Item[m_EquipItem[itempart_weapon]].GetParticular();
		}
		else if (Item[m_EquipItem[itempart_weapon]].GetDetailType()==1)
		{//暗器
            return Item[m_EquipItem[itempart_weapon]].GetParticular2();
		}
	}

	return -1;
}

#ifdef _SERVER
BOOL KItemList::IsTaskItemExist(int nDetailType,BYTE bType)
{
	int nIdx = 0;
	while ((nIdx = m_UseIdxItemList.GetNext(nIdx)))
	{
		if (bType == 1)
		{
			int nGameIdx = m_Items[nIdx].nIdx;
			if (item_task != Item[nGameIdx].GetGenre())
				continue;
			if (nDetailType == Item[nGameIdx].GetDetailType())
			{
				return TRUE;
			}
		}
		else if (bType == 2)
		{
			int nGameIdx = m_Items[nIdx].nIdx;
			if (item_mine != Item[nGameIdx].GetGenre())
				continue;
			if (nDetailType == Item[nGameIdx].GetParticular())
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}
#endif

#ifdef _SERVER
int		KItemList::GetTaskItemNum(int nDetailType,BYTE bType)
{
	int		nNo = 0;
	int		nIdx = 0;
	while ((nIdx = m_UseIdxItemList.GetNext(nIdx)))
	{
		if (bType == 1)
		{
			if (item_task != Item[m_Items[nIdx].nIdx].GetGenre())
				continue;
			if (nDetailType == Item[m_Items[nIdx].nIdx].GetDetailType())
			{
				nNo++;
			}
		}
		else if (bType == 2)
		{
			if (item_mine != Item[m_Items[nIdx].nIdx].GetGenre())
				continue;
			if (nDetailType == Item[m_Items[nIdx].nIdx].GetParticular())
			{
				nNo++;
			}
		}
	}
	return nNo;
}
#endif

#ifdef _SERVER
BOOL KItemList::RemoveTaskItem(int nDetailType)
{
	int nIdx = 0;
	while ((nIdx = m_UseIdxItemList.GetNext(nIdx)))
	{
		int nGameIdx = m_Items[nIdx].nIdx;

		if (item_task != Item[nGameIdx].GetGenre())
			continue;

		if (nDetailType == Item[nGameIdx].GetDetailType())
		{
			      Remove(nGameIdx);
				  ItemSet.Remove(nGameIdx);
			      return TRUE;
		}
	}
	return FALSE;
}

BOOL KItemList::RemoveItemIdx(int nItemIdx,int model,int ndx,int ndy)
{
	if (Remove(nItemIdx,ndx,ndy,model))
	{
	     ItemSet.Remove(nItemIdx);
		 return TRUE;
	}
     return FALSE;
}

BOOL KItemList::RemoveMineItem(int nDetailType)
{
	int nIdx = 0;
	while ((nIdx = m_UseIdxItemList.GetNext(nIdx)))
	{
		int nGameIdx = m_Items[nIdx].nIdx;
		if (item_mine != Item[nGameIdx].GetGenre())
			continue;
		if (nDetailType == Item[nGameIdx].GetParticular())
		{

			Remove(nGameIdx);
			ItemSet.Remove(nGameIdx);
			return TRUE;
		}
	}
	return FALSE;
}
#endif

#ifdef _SERVER
//--------------------------------------------------------------------------
//	功能：交易中把 trade room 中的 item 的 idx width height 信息写入 itemset 中的 m_psItemInfo 中去
//--------------------------------------------------------------------------
void	KItemList::GetTradeRoomItemInfo()
{
	_ASSERT(ItemSet.m_psItemInfo);
//	if (!ItemSet.m_psItemInfo)
//	{
//		ItemSet.m_psItemInfo = new TRADE_ITEM_INFO[TRADE_ROOM_WIDTH * TRADE_ROOM_HEIGHT];
//	}
	memset(ItemSet.m_psItemInfo, 0, sizeof(TRADE_ITEM_INFO) * TRADE_ROOM_WIDTH * TRADE_ROOM_HEIGHT);

	int		nItemIdx, nXpos, nYpos, nPos;

	nItemIdx = 0;
	nXpos = 0;
	nYpos = 0;
	nPos = 0;

	while (1)
	{
		nItemIdx = m_Room[room_trade].GetNextItem(nItemIdx, nXpos, nYpos, &nXpos, &nYpos);
		if (nItemIdx == 0)
			break;
		_ASSERT(nPos < TRADE_ROOM_WIDTH * TRADE_ROOM_HEIGHT);

		ItemSet.m_psItemInfo[nPos].m_nIdx = nItemIdx;
		ItemSet.m_psItemInfo[nPos].m_nWidth = Item[nItemIdx].GetWidth();
		ItemSet.m_psItemInfo[nPos].m_nHeight = Item[nItemIdx].GetHeight();
		nPos++;
	}

	// 从大到小排序
	TRADE_ITEM_INFO	sTemp;
	for (int i = nPos - 1; i >= 0; i--)
	{
		for (int j = 0; j < i; ++j)
		{
			if (ItemSet.m_psItemInfo[j].m_nWidth * ItemSet.m_psItemInfo[j].m_nHeight < 
				ItemSet.m_psItemInfo[j + 1].m_nWidth * ItemSet.m_psItemInfo[j + 1].m_nHeight)
			{
				sTemp = ItemSet.m_psItemInfo[j];
				ItemSet.m_psItemInfo[j] = ItemSet.m_psItemInfo[j + 1];
				ItemSet.m_psItemInfo[j + 1] = sTemp;
			}
		}
	}
}
#endif

#ifdef _SERVER
//--------------------------------------------------------------------------
//	功能：交易中判断买进的物品能不能完全放进自己的物品栏
//--------------------------------------------------------------------------
BOOL	KItemList::TradeCheckCanPlace()
{
	LPINT	pnTempRoom;
	pnTempRoom = new int[EQUIPMENT_ROOM_WIDTH * EQUIPMENT_ROOM_HEIGHT];
	memcpy(pnTempRoom, m_Room[room_equipment].m_pArray, sizeof(int) * EQUIPMENT_ROOM_WIDTH * EQUIPMENT_ROOM_HEIGHT);

	int		nPos, i, j, a, b, nFind, nNext;
	for (nPos = 0; nPos < TRADE_ROOM_WIDTH * TRADE_ROOM_HEIGHT; ++nPos)
	{
		if (!ItemSet.m_psItemInfo[nPos].m_nIdx)
			break;
		nFind = 0;
		for (i = 0; i < EQUIPMENT_ROOM_HEIGHT - ItemSet.m_psItemInfo[nPos].m_nHeight + 1; ++i)
		{
			for (j = 0; j < EQUIPMENT_ROOM_WIDTH - ItemSet.m_psItemInfo[nPos].m_nWidth + 1; ++j)
			{
				nNext = 0;
				for (a = 0; a < ItemSet.m_psItemInfo[nPos].m_nHeight; ++a)
				{
					for (b = 0; b < ItemSet.m_psItemInfo[nPos].m_nWidth; ++b)
					{
						if (pnTempRoom[(i + a) * EQUIPMENT_ROOM_WIDTH + j + b])
						{
							nNext = 1;
							break;
						}
					}
					if (nNext)
						break;
				}
				// 找到一个位置
				if (!nNext)
				{
					// 数据处理
					ItemSet.m_psItemInfo[nPos].m_nX = j;
					ItemSet.m_psItemInfo[nPos].m_nY = i;
					for (a = 0; a < ItemSet.m_psItemInfo[nPos].m_nHeight; ++a)
					{
						for (b = 0; b < ItemSet.m_psItemInfo[nPos].m_nWidth; ++b)
							pnTempRoom[(i + a) * EQUIPMENT_ROOM_WIDTH + j + b] = ItemSet.m_psItemInfo[nPos].m_nIdx;
					}

					nFind = 1;
					break;
				}
			}
			if (nFind)
				break;
		}
		if (!nFind)
		{
			delete []pnTempRoom;
			pnTempRoom=NULL;
			return FALSE;
		}
	}

	delete []pnTempRoom;
	pnTempRoom=NULL;
	return TRUE;
}
#endif

//#ifdef _SERVER
//--------------------------------------------------------------------------
//	功能：判断一定长宽的物品能否放进物品栏 (为了服务器效率，本函数里面没有调用其他函数)
//--------------------------------------------------------------------------
BOOL	KItemList::CheckCanPlaceInEquipment(int nWidth, int nHeight, int *pnX, int *pnY,int nRoomType)
{
	if (nWidth <= 0 || nHeight <= 0 || !pnX || !pnY)
		return false;

	if (!m_Room[nRoomType].m_pArray)
	   return false;
	POINT pPos;
	if (m_Room[nRoomType].FindRoom(nWidth,nHeight,&pPos))
	{
		*pnX = pPos.x;
		*pnY = pPos.y;
		return true;
	}
	return false;
	/*if (m_Room[nRoomType].FindEmptyPlace(nWidth,nHeight,&pPos)) //查找空位置
	{
		*pnX = pPos.x;
		*pnY = pPos.y;
		return true;
	}
	return false;*/
	/*LPINT	pnTempRoom;
	int		i, j, a, b, nNext;
	int     m_ronmWidth  = m_Room[nRoomType].m_nWidth;//m_Room[nRoomType].GetWidth();
	int     m_roomHeigth = m_Room[nRoomType].m_nHeight;//m_Room[nRoomType].GetHeight();
	        pnTempRoom   = m_Room[nRoomType].m_pArray; //容器的面积=（长度*高度） 单位：格子

			
	for (i = 0; i < m_roomHeigth - nHeight + 1; i++)  //容器剩下的高度 （有几个格子）EQUIPMENT_ROOM_HEIGHT
	{
		for (j = 0; j <m_ronmWidth - nWidth + 1; j++) //容器剩下的长度 （有几个格子）EQUIPMENT_ROOM_WIDTH
		{
			nNext = 0;
			for (a = 0; a < nHeight; a++) //物品占用几个格子
			{
				for (b = 0; b < nWidth; b++) 
				{
					if (pnTempRoom[(i + a) * m_ronmWidth + j + b])  //如果有东西  EQUIPMENT_ROOM_WIDTH
					{//如果有东西
						nNext = 1;
						break;
					}
				}
				if (nNext)
					break;
			}
			if (!nNext)
			{
				*pnX = j;
				*pnY = i;
				return TRUE;
			}
		}
	}
	return FALSE;*/
}
//#endif

//------------------------------------------------------------------------------
//	功能：在room_equipment中查找指定Genre和DetailType的物品，得到ItemIdx和位置
//------------------------------------------------------------------------------
BOOL	KItemList::FindSameDetailTypeInEquipment(int nGenre, int nDetail, int nPart, int *pnIdx, int *pnX, int *pnY)
{
	return m_Room[room_equipment].FindSameDetailType(nGenre, nDetail,nPart, pnIdx, pnX, pnY);
}
//服务器端容器之间自动移动物品，不经过鼠标行为
#ifdef _SERVER
//------------------------------------------------------------------------------
//	功能：自动把一个药品从room_equipment移动到room_immediacy
//------------------------------------------------------------------------------
BOOL	KItemList::AutoMoveMedicine(int nItemIdx, int nSrcX, int nSrcY, int nDestX, int nDestY)
{
	int nListIdx = FindSame(nItemIdx);
	_ASSERT(nListIdx > 0);

	if (m_Items[nListIdx].nIsLock)
	{
		msgshow("物品被锁住,禁止操作!");
		return false;
	}

	if (!m_Room[room_equipment].m_pArray || !m_Room[room_immediacy].m_pArray)
		return FALSE;
	if (nSrcX < 0 || nSrcX >= m_Room[room_equipment].m_nWidth || nSrcY < 0 || nSrcY >= m_Room[room_equipment].m_nHeight)
		return FALSE;
	if (nDestX < 0 || nDestX >= m_Room[room_immediacy].m_nWidth || nDestY < 0 || nDestY >= m_Room[room_immediacy].m_nHeight)
		return FALSE;
	if (nItemIdx != m_Room[room_equipment].m_pArray[nSrcY * m_Room[room_equipment].m_nWidth + nSrcX] ||
		0 != m_Room[room_immediacy].m_pArray[nDestY * m_Room[room_immediacy].m_nWidth + nDestX])
		return FALSE;

	_ASSERT(Item[nItemIdx].GetWidth() == 1 && Item[nItemIdx].GetHeight() == 1);  //只能放 1X1大小的物品

	if (!m_Room[room_equipment].PickUpItem(nItemIdx, nSrcX, nSrcY, Item[nItemIdx].GetWidth(), Item[nItemIdx].GetHeight()))
		return FALSE;
	if (!m_Room[room_immediacy].PlaceItem(nDestX, nDestY, nItemIdx, Item[nItemIdx].GetWidth(), Item[nItemIdx].GetHeight()))
	{
		m_Room[room_equipment].PlaceItem(nSrcX, nSrcY, nItemIdx, Item[nItemIdx].GetWidth(), Item[nItemIdx].GetHeight());
		return FALSE;
	}

	//int nListIdx = FindSame(nItemIdx);
	//_ASSERT(nListIdx > 0);
	m_Items[nListIdx].nPlace = pos_immediacy;
	m_Items[nListIdx].nX = nDestX;
	m_Items[nListIdx].nY = nDestY;

	//通知客户端移动成功
	ITEM_AUTO_MOVE_SYNC	sMove;
	sMove.ProtocolType = s2c_ItemAutoMove;
	sMove.m_ItenDwid = Item[nItemIdx].GetID();
	sMove.m_btSrcPos = pos_equiproom;   //源目标
	sMove.m_btSrcX   = nSrcX;
	sMove.m_btSrcY   = nSrcY;
	sMove.m_btDestPos = pos_immediacy;  //目的目标
	sMove.m_btDestX  = nDestX;
	sMove.m_btDestY  = nDestY;
	if (g_pServer)
		g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(ITEM_AUTO_MOVE_SYNC));

	return TRUE;
}
#endif

#ifdef _SERVER
//------------------------------------------------------------------------------
//	功能：自动把一个物品从一个容器移动到另外一个容器
//------------------------------------------------------------------------------
BOOL KItemList::AutoMoveItems(int nItemIdx, int nSrcX, int nSrcY, int nDestX, int nDestY,int nSrcRonn,int nDestRonn)
{
	//char msg[64];
	if  (nSrcRonn>=room_num || nSrcRonn<room_equipment || nDestRonn >=room_num || nDestRonn<room_equipment)
	{
		msgshow("提示:物品信息有误C,请联系GM处理!");
		return false;
	}

	if (!m_Room[nSrcRonn].m_pArray||!m_Room[nDestRonn].m_pArray){
		msgshow("提示:物品信息有误D,请联系GM处理!");
		return false;
	}
	char nItemInfo[128];
	 if (m_Room[nSrcRonn].FindItem(nSrcX,nSrcY)<=0){
		 msgshow("提示:物品信息有误E,请联系GM处理!");
#ifdef _SERVER
		 ITEM_REMOVE_SYNC	sRemove;
		 sRemove.ProtocolType = s2c_removeitem;
		 sRemove.m_ID         = Item[nItemIdx].GetID();	
		 sRemove.m_dx         = 0;
		 sRemove.m_dy         = 0;
		 sRemove.m_model   = 0;
		 sRemove.nPlace       =-1;
		 g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sRemove, sizeof(ITEM_REMOVE_SYNC));
		 snprintf(nItemInfo, sizeof(nItemInfo),"----AutoMoveItems:账号:%s角色:%s,涉嫌复制物品:%s[+%d],删除异常物品成功!B-----",Player[m_PlayerIdx].m_AccoutName,Player[m_PlayerIdx].m_PlayerName,Item[nItemIdx].GetName(),Item[nItemIdx].GetEnChance());
		 g_LogFile.gTraceLogFile(nItemInfo,strlen(nItemInfo));
#endif
		 return false;
	 }

	if (nDestRonn!=room_dazao && !m_Room[nDestRonn].m_pArray) //目标容器
	{
		msgshow("提示:物品信息有误A,请联系GM处理!");
		return FALSE;
	}
	
	int nListIdx = FindSame(nItemIdx);
	if (nListIdx <=0)
	{
		msgshow("提示:物品信息有误B,请联系GM处理!");
		return FALSE;
	}

	if (m_Items[nListIdx].nIsLock)
	{
		msgshow("提示:该物品被锁定,禁止操作!");
		return FALSE;
	}

	int nSrcX_s,nSrcY_s;
	nSrcX_s = m_Items[nListIdx].nX;
	nSrcY_s = m_Items[nListIdx].nY;
//	_ASSERT(Item[nItemIdx].GetWidth() == 1 && Item[nItemIdx].GetHeight() == 1);
   if (nSrcRonn!=room_dazao && nDestRonn!=room_dazao)
   {//储存容器之间的移动
	   if  (nDestRonn==room_immediacy)
	   {//目标为快捷栏就需要检测下物品的合适条件
		   if (!Item[nItemIdx].GetIsKuaiJie())
		   {
			   msgshow("提示:物品不能放置快捷栏!");
			   return FALSE;
		   }
		   int nGenre = Item[nItemIdx].GetGenre();
		   if  (nGenre==item_equip ||  nGenre ==item_materials || nGenre ==item_fusion)
		   {
			   msgshow("提示:物品不能放置快捷栏!");
			   return FALSE;
		   }
		   if  (nGenre<=item_equip && nGenre >=item_number)
		   {
			   msgshow("提示:物品不能放置快捷栏!");
			   return FALSE;
		   }
	   }

	   if  (nDestRonn==room_trade)
	   {//如果目标为 交易栏
		   if (!Player[m_PlayerIdx].CheckTrading())	// 不在交易
		   {
			   msgshow("提示:交易没有激活!");
			   return false;
		   }

		   if ((Item[nItemIdx].GetIsBang() || Item[nItemIdx].GetUseMapIdx() || Item[nItemIdx].GetTime()->bYear>0) && !Player[m_PlayerIdx].CheckCanTradeName())
		   {
			   msgshow("提示:物品不能交易:绑定/限时/锁魂!");
			   return false;
		   }

		   if ((Item[nItemIdx].GetIsBang() || Item[nItemIdx].GetUseMapIdx() || Item[nItemIdx].GetTime()->bYear>0) && !Player[Player[m_PlayerIdx].m_cTrade.m_nTradeDest].CheckCanTradeName())
		   {
			   msgshow("提示:对方不能交易:绑定/限时/锁魂!");
			   return false;
		   }
		   
	   }//end room_trade
	   //服务器端的原始位置
      if  (!m_Room[nSrcRonn].PickUpItem(nItemIdx,nSrcX_s,nSrcY_s,Item[nItemIdx].GetWidth(), Item[nItemIdx].GetHeight()))
	   {//清空源容器
		   msgshow("提示:物品放置失败!");
		   return FALSE;
	   }
       //把东西放进目标容器
	   if (!m_Room[nDestRonn].PlaceItem(nDestX,nDestY,nItemIdx,Item[nItemIdx].GetWidth(),Item[nItemIdx].GetHeight()))
	   {
		   msgshow("提示:物品移动失败!");
		   return FALSE;
	   }
	   //通知对方有东西发生变化了
	   if  (nSrcRonn==room_trade)
	   {
		   ITEM_REMOVE_SYNC	sRemove;
		   sRemove.ProtocolType = s2c_removeitem;
		   sRemove.m_ID = Item[nItemIdx].m_dwID;
		   sRemove.m_dx = 0;
		   sRemove.m_dy = 0;
		   sRemove.m_model = 0;
		    sRemove.nPlace    =-1;
		   g_pServer->PackDataToClient(Player[Player[m_PlayerIdx].m_cTrade.m_nTradeDest].m_nNetConnectIdx, (BYTE*)&sRemove, sizeof(ITEM_REMOVE_SYNC));
	   }
	   //通知对方有东西放进来了
	   if  (nDestRonn==room_trade)
	   {//如果目标为 交易栏
		   ITEM_SYNC	sItem;
		   memset(&sItem, 0, sizeof(sItem));
		   sItem.ProtocolType = s2c_syncitem;
		   sItem.m_Point = Item[nItemIdx].GetPoint();
		   sItem.m_Rongpiont = Item[nItemIdx].IsRong(); //熔炼
		   sItem.m_GoldId = Item[nItemIdx].GetGoldId();
		   sItem.m_IsPlatima    = Item[nItemIdx].GetIsPlatima();
		   sItem.m_UseMap       = Item[nItemIdx].GetUseMapIdx();
		   sItem.m_UseKind		= Item[nItemIdx].GetUseKind();
		   sItem.m_EnChance = Item[nItemIdx].GetEnChance();
		   sItem.m_StackNum = Item[nItemIdx].GetStackNum();
		   sItem.m_ID = Item[nItemIdx].GetID();
		   sItem.m_Genre = Item[nItemIdx].GetGenre();
		   sItem.m_Detail = Item[nItemIdx].GetDetailType();
		   sItem.m_Particur = Item[nItemIdx].GetParticular();
		   sItem.m_Series = Item[nItemIdx].GetSeries();
		   sItem.m_Level = Item[nItemIdx].GetLevel();
		   sItem.m_Res   = Item[nItemIdx].GetItemResIdx();
		   sItem.m_Luck = Item[nItemIdx].m_GeneratorParam.nLuck;
		   sItem.m_btPlace = pos_trade1;
		   sItem.m_btX = nDestX;
		   sItem.m_btY = nDestY;
		   sItem.m_Time.bYear = Item[nItemIdx].GetTime()->bYear;
		   sItem.m_Time.bMonth = Item[nItemIdx].GetTime()->bMonth;
		   sItem.m_Time.bDay = Item[nItemIdx].GetTime()->bDay;
		   sItem.m_Time.bHour = Item[nItemIdx].GetTime()->bHour;
		   sItem.m_Time.bMin = Item[nItemIdx].GetTime()->bMin;
		   sItem.m_IsBang     = Item[nItemIdx].GetIsBang();       //是否绑定
		   sItem.m_IsKuaiJie    = Item[nItemIdx].GetIsKuaiJie();  //是否快捷栏
		   sItem.m_IsMagic      =Item[nItemIdx].GetIsMagic();     //是否有魔法属性
		   sItem.m_SkillType    =Item[nItemIdx].GetSkillType();
		   sItem.m_MagicID      = Item[nItemIdx].GetMagicID();

		   sItem.m_IsWhere      = Item[nItemIdx].GetIsWhere();
		   sItem.m_IsCanUse     = Item[nItemIdx].GetIsCanUse();
		   sItem.m_IsLoign      = 0;

		   sItem.m_RandomSeed = Item[nItemIdx].m_GeneratorParam.uRandomSeed;
		   sItem.m_Version= Item[nItemIdx].m_GeneratorParam.nVersion;
		   sItem.m_Durability = Item[nItemIdx].GetDurability();

		   //_snprintf(sItem.m_WonName,sizeof(sItem.m_WonName)-1,"%s",Item[nItemIdx].GetWonName());  //所有者
		   snprintf(sItem.m_WonName, sizeof(sItem.m_WonName), "%s", Item[nItemIdx].GetWonName());  //所有者

		   for (int j = 0; j < 6; j++)
		   {
			   sItem.m_MagicLevel[j]   = Item[nItemIdx].m_GeneratorParam.nGeneratorLevel[j];
			   sItem.m_RongMagicLev[j] = Item[nItemIdx].m_GeneratorParam.nRGeneratorLevel[j];
			   //sItem.m_BaoMagicLev[j]  = Item[nItemIdx].m_GeneratorParam.nBGeneratorLevel[j];
		   }
		   for (int i = 0; i < 7; i++)
		   {
			   sItem.m_JBLevel[i]      = Item[nItemIdx].m_GeneratorParam.nJGeneratorLevel[i];
		   }
		   //对方
		   g_pServer->PackDataToClient(Player[Player[m_PlayerIdx].m_cTrade.m_nTradeDest].m_nNetConnectIdx, (BYTE*)&sItem, sizeof(ITEM_SYNC));
		   
		   //自己
		   // g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));

	   }//end room_trade
   }
   else if (nSrcRonn!=room_dazao && nDestRonn==room_dazao)
   {
       if(!m_Room[nSrcRonn].PickUpItem(nItemIdx,nSrcX_s, nSrcY_s,Item[nItemIdx].GetWidth(), Item[nItemIdx].GetHeight()))
	   {//清空源容器
		   return FALSE;
	   }

	   m_ItemBox[nDestX]=nItemIdx;
   }
   else if (nSrcRonn==room_dazao && nDestRonn!=room_dazao)
   {
       m_ItemBox[nSrcX_s]=0;
      //把东西放进目标容器
	   if  (!m_Room[nDestRonn].PlaceItem(nDestX,nDestY,nItemIdx,Item[nItemIdx].GetWidth(),Item[nItemIdx].GetHeight()))
		   return FALSE;
   }
   else if (nSrcRonn==room_dazao && nDestRonn==room_dazao)
   {
        m_ItemBox[nSrcX_s]=0;
		m_ItemBox[nDestX]=nItemIdx;
   }
	//POINT pPos;
	   // if (m_Room[nDestRonn].SetEmptyPlaceItemIdx(nItemIdx, Item[nItemIdx].GetWidth(), Item[nItemIdx].GetHeight(),&pPos))
	//	{//检测空位置，并设置位置标示idx
      //     nDestX=pPos.x;
      //     nDestY=pPos.y;
	//	}
	//Chec kCanPlaceInEquipment(Item[nItemIdx].GetWidth(),Item[nItemIdx].GetHeight());
   switch(nDestRonn)
   {
   case room_immediacy:
	   m_Items[nListIdx].nPlace = pos_immediacy; //设定目标容器物品的信息标示
	   break;
   case room_repository:
	   m_Items[nListIdx].nPlace = pos_repositoryroom; //设定目标容器物品的信息标示
	   break;
   case room_exbox1:
	   m_Items[nListIdx].nPlace = pos_exbox1room; //设定目标容器物品的信息标示
	   break;
   case room_exbox2:
	   m_Items[nListIdx].nPlace = pos_exbox2room; //设定目标容器物品的信息标示
	   break;
   case room_exbox3:
	   m_Items[nListIdx].nPlace = pos_exbox3room; //设定目标容器物品的信息标示
	   break;
   case room_equipmentex:
	   m_Items[nListIdx].nPlace = pos_equiproomex; //设定目标容器物品的信息标示
	   break;
   case room_equipment:
	   m_Items[nListIdx].nPlace = pos_equiproom; //设定目标容器物品的信息标示
	   break;
   case room_dazao:
	   m_Items[nListIdx].nPlace = pos_dazao;     //设定目标容器物品的信息标示
	   break;	
   case room_cailiao:
	   m_Items[nListIdx].nPlace = pos_cailiao;   //设定目标容器物品的信息标示
	   break;
   case room_give:
	   m_Items[nListIdx].nPlace = pos_give;      //设定目标容器物品的信息标示
	   break;
   case room_trade:
	   m_Items[nListIdx].nPlace = pos_traderoom;   //设定目标容器物品的信息标示
	   break;
   default:
	   break;
   }
    m_Items[nListIdx].nX     = nDestX;
	m_Items[nListIdx].nY     = nDestY;
	//通知客户端移动成功
	ITEM_AUTO_MOVE_SYNC	sMove;
	sMove.ProtocolType = s2c_ItemAutoMove;
	sMove.m_ItenDwid   = Item[nItemIdx].GetID();
	sMove.m_btSrcX     = nSrcX;//nSrcX_s;//nSrcX;
	sMove.m_btSrcY     = nSrcY;//nSrcY_s;//nSrcY; Item[nItemIdx].GetID();
	sMove.m_btDestX    = nDestX;
	sMove.m_btDestY    = nDestY;
    //源容器
	switch(nSrcRonn)
	{
	case room_immediacy:
		sMove.m_btSrcPos = pos_immediacy; //设定目标容器物品的信息标示
		break;
	case room_repository:
		sMove.m_btSrcPos = pos_repositoryroom; //设定目标容器物品的信息标示
		break;
	case room_exbox1:
		sMove.m_btSrcPos= pos_exbox1room; //设定目标容器物品的信息标示
		break;
	case room_exbox2:
		sMove.m_btSrcPos = pos_exbox2room; //设定目标容器物品的信息标示
		break;
	case room_exbox3:
		sMove.m_btSrcPos = pos_exbox3room; //设定目标容器物品的信息标示
		break;
	case room_equipmentex:
		sMove.m_btSrcPos = pos_equiproomex; //设定目标容器物品的信息标示
		break;
	case room_equipment:
		sMove.m_btSrcPos   = pos_equiproom;     //源目标容器位置
		break;
	case room_dazao:
		sMove.m_btSrcPos   = pos_dazao; //设定目标容器物品的信息标示
		break;	
	case room_cailiao:
		sMove.m_btSrcPos   = pos_cailiao; //设定目标容器物品的信息标示
		break;
	case room_give:
		sMove.m_btSrcPos   = pos_give; //设定目标容器物品的信息标示
		break;
	case room_trade:
		sMove.m_btSrcPos   = pos_traderoom; //设定目标容器物品的信息标示
		break;
	default:
		break;
	}
	//目标容器
	switch(nDestRonn)
	{
	case room_immediacy:
		sMove.m_btDestPos = pos_immediacy; //设定目标容器物品的信息标示
		break;
	case room_repository:
		sMove.m_btDestPos = pos_repositoryroom; //设定目标容器物品的信息标示
		break;
	case room_exbox1:
		sMove.m_btDestPos= pos_exbox1room; //设定目标容器物品的信息标示
		break;
	case room_exbox2:
		sMove.m_btDestPos = pos_exbox2room; //设定目标容器物品的信息标示
		break;
	case room_exbox3:
		sMove.m_btDestPos = pos_exbox3room; //设定目标容器物品的信息标示
		break;
	case room_equipmentex:
		sMove.m_btDestPos = pos_equiproomex; //设定目标容器物品的信息标示
		break;
	case room_equipment:
		sMove.m_btDestPos   = pos_equiproom;     //源目标容器位置
		break;
	case room_dazao:
		sMove.m_btDestPos   = pos_dazao;           //设定目标容器物品的信息标示
		break;	
	case room_cailiao:
		sMove.m_btDestPos   = pos_cailiao;         //设定目标容器物品的信息标示
		break;	
	case room_give:
		sMove.m_btDestPos   = pos_give;         //设定目标容器物品的信息标示
		break;
	case room_trade:
		sMove.m_btDestPos   = pos_traderoom;         //设定目标容器物品的信息标示
		break;
	default:
		break;
	}

	if (g_pServer)
		g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(ITEM_AUTO_MOVE_SYNC));
	
	//char Msg[64];
	//sprintf(Msg,"移动(yp:%d,sX:%d,sY:%d||dp:%d,dX:%d,dY:%d)",nSrcRonn,nSrcX,nSrcY,nDestRonn,nDestX,nDestY);
	//msgshow(Msg);
	return TRUE;
}
#endif

//删除某个容器的所有东西
//#ifndef	_SERVER
void	KItemList::RemoveAllInOneRoom(int nRoom)
{
	if (nRoom < 0 || nRoom >= room_num)
		return;

	int	nItemIdx, nXpos, nYpos, nPos;

	nItemIdx = 0;
	nXpos = 0;
	nYpos = 0;
	nPos = 0;

	while (1)
	{
		nItemIdx = m_Room[nRoom].GetNextItem(nItemIdx, nXpos, nYpos, &nXpos, &nYpos);
		if (nItemIdx == 0)
			break;

		_ASSERT(nPos < m_Room[nRoom].m_nWidth * m_Room[nRoom].m_nHeight);
		Remove(nItemIdx);
		//ItemSet.Remove(nItemIdx);
		nPos++;
	}
}
//#endif
//客户端锁着了
#ifndef _SERVER
void KItemList::LockOperation()
{
	if (IsLockOperation())
	{//如果是锁着的就 返回了
		//_ASSERT(0);
		return;
	}
	m_bLockOperation = TRUE;
}
#endif
//客户端解锁
#ifndef _SERVER
void KItemList::UnlockOperation()
{
	if (!IsLockOperation())
	{
		return;
	}
	m_bLockOperation = FALSE;
}
#endif

int KItemList::GetActiveAttribNum(int nIdx)
{
	for (int i = 0; i < itempart_num; ++i)
	{
		if (nIdx == m_EquipItem[i])
		{
			return GetEquipEnhance(i);
		}
	}
	return 0;
}

#ifdef _SERVER
//-------------------------------------------------------------------------------
//	功能：丢失随身物品
//-------------------------------------------------------------------------------
void	KItemList::AutoLoseItemFromEquipmentRoom(int nRate)
{
	//nguoi choi pk roi do
	if (Player[m_PlayerIdx].CheckTrading())
		return;

	if (nRate <= 0 || nRate > 100)
		return;

	int		nItemIdx, nXpos, nYpos, nPos;

	nItemIdx = 0;
	nXpos = 0;
	nYpos = 0;
	nPos = 0;

	// 先统计所有物品，排除任务物品
	while (1)
	{
		nItemIdx = m_Room[room_equipment].GetNextItem(nItemIdx, nXpos, nYpos, &nXpos, &nYpos);
		if (nItemIdx == 0)
			break;

		if (item_task == Item[nItemIdx].GetGenre() ||
			Item[nItemIdx].GetLockItem()->IsLockItem())
			continue;

		ItemSet.m_sLoseItemFromEquipmentRoom[nPos].nIdx = nItemIdx;
		ItemSet.m_sLoseItemFromEquipmentRoom[nPos].nPlace = pos_equiproom;
		ItemSet.m_sLoseItemFromEquipmentRoom[nPos].nX = nXpos;
		ItemSet.m_sLoseItemFromEquipmentRoom[nPos].nY = nYpos;
		++nPos;
	}
	if (nPos == 0)	  //没有东西可掉的
		return;

	KMapPos			sMapPos;
	int				nSelect;
	int				nObj;
	KObjItemInfo	sInfo;

	for (int i = 0; i < nPos; ++i)
	{
		if (g_Random(100) >= nRate)
			continue;
		nItemIdx = ItemSet.m_sLoseItemFromEquipmentRoom[i].nIdx;
		Player[m_PlayerIdx].GetAboutPos(&sMapPos);
		// 丢弃
		if (Remove(nItemIdx))
		{
			sInfo.m_nItemID = nItemIdx;
			sInfo.m_nItemWidth = Item[nItemIdx].GetWidth();
			sInfo.m_nItemHeight = Item[nItemIdx].GetHeight();
			sInfo.m_nMoneyNum = 0;
			snprintf(sInfo.m_szName, sizeof(sInfo.m_szName), "%s", Item[nItemIdx].m_CommonAttrib.szItemName);
			sInfo.m_nColorID = 0;
			sInfo.m_nMovieFlag = 1;
			sInfo.m_nSoundFlag = 1;
			sInfo.m_sHaveAttack=0;   //是否设置为攻击无效了
	        //sInfo.m_AttackerDwid=0;   //上次攻击着的DWID

			nObj = ObjSet.Add(Item[nItemIdx].GetObjIdx(), sMapPos, sInfo);
			if (nObj >= 0)
			{
				Object[nObj].SetItemBelong(-1);
			}
		 	/*
			SHOW_MSG_SYNC	sMsg;
			sMsg.ProtocolType = s2c_msgshow;
			sMsg.m_wMsgID = enumMSG_ID_DEATH_LOSE_ITEM;
			sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID) + sizeof(sInfo.m_szName);
			sMsg.m_lpBuf = new BYTE[sMsg.m_wLength + 1];
			memcpy(sMsg.m_lpBuf, &sMsg, sizeof(SHOW_MSG_SYNC) - sizeof(LPVOID));
			memcpy((char*)sMsg.m_lpBuf + sizeof(SHOW_MSG_SYNC) - sizeof(LPVOID), sInfo.m_szName, sizeof(sInfo.m_szName));
			g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, sMsg.m_lpBuf, sMsg.m_wLength + 1);
		   */
		   Player[m_PlayerIdx].m_ItemList.msgshow("Ban da bi mat 1 item vi PK!");

		}
	}
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------------
//	功能：丢失一个穿在身上的装备(马不会掉)
//-------------------------------------------------------------------------------
void	KItemList::AutoLoseEquip()
{
#ifndef defEQUIP_POWER
	int		i, nPos = 0;
	for (i = 0; i < itempart_num; ++i)
	{
		if (m_EquipItem[i] <= 0)
			continue;
		if (i == itempart_horse) //马不能掉
			continue;
		ItemSet.m_sLoseEquipItem[nPos].nIdx = m_EquipItem[i];
		ItemSet.m_sLoseEquipItem[nPos].nPlace = nPos;
		++nPos;
	}
	if (nPos == 0)
		return;

	int		nSelect = g_Random(nPos);
#endif

#ifdef defEQUIP_POWER
	int		i, nPos = 0, nTotalPower = 0;

	for (i = 0; i < itempart_num; ++i)
	{
		if (m_EquipItem[i] <= 0)
			continue;
		nTotalPower += g_nEquipPower[i];
		ItemSet.m_sLoseEquipItem[nPos].nIdx = m_EquipItem[i];
		ItemSet.m_sLoseEquipItem[nPos].nPlace = nPos;
		ItemSet.m_sLoseEquipItem[nPos].nX = nTotalPower;	// nX 借用一下
		++nPos;
	}
	if (nTotalPower == 0)
		return;
	int	nSelect = g_Random(nTotalPower);
	for (i = 0; i < nPos; ++i)
	{
		if (ItemSet.m_sLoseEquipItem[i].nX > nSelect)
		{
			nSelect = i;
			break;
		}
	}
	if (i >= nPos)
		return;
#endif

	int			 nItemIdx;
	KMapPos		sMapPos;

	nItemIdx = ItemSet.m_sLoseEquipItem[nSelect].nIdx;
	Player[m_PlayerIdx].GetAboutPos(&sMapPos);

	if (Remove(nItemIdx))
	{
		int		nObj;
		KObjItemInfo	sInfo;
		sInfo.m_nItemID     = nItemIdx;
		sInfo.m_nItemWidth  = Item[nItemIdx].GetWidth();
		sInfo.m_nItemHeight = Item[nItemIdx].GetHeight();
		sInfo.m_nMoneyNum   = 0;
		snprintf(sInfo.m_szName, sizeof(sInfo.m_szName), "%s", Item[nItemIdx].m_CommonAttrib.szItemName);
		sInfo.m_nColorID    = 0;
		sInfo.m_nMovieFlag  = 1;
		sInfo.m_nSoundFlag  = 1;
		sInfo.m_sHaveAttack=0;   //是否设置为攻击无效了
	    //sInfo.m_AttackerDwid=0;   //上次攻击着的DWID

		nObj = ObjSet.Add(Item[nItemIdx].GetObjIdx(), sMapPos, sInfo);

		if (nObj >= 0)
		{
			Object[nObj].SetItemBelong(-1);
		}
	   /*
		SHOW_MSG_SYNC	sMsg;
		sMsg.ProtocolType = s2c_msgshow;
		sMsg.m_wMsgID = enumMSG_ID_DEATH_LOSE_ITEM;
		sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID) + sizeof(sInfo.m_szName);
		sMsg.m_lpBuf = new BYTE[sMsg.m_wLength + 1];
		memcpy(sMsg.m_lpBuf, &sMsg, sizeof(SHOW_MSG_SYNC) - sizeof(LPVOID));
		memcpy((char*)sMsg.m_lpBuf + sizeof(SHOW_MSG_SYNC) - sizeof(LPVOID), sInfo.m_szName, sizeof(sInfo.m_szName));
		g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, sMsg.m_lpBuf, sMsg.m_wLength + 1);
		*/

		Player[m_PlayerIdx].m_ItemList.msgshow("Ban da bi mat 1 item vi PK!");
	}
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------------
//	功能：
//-------------------------------------------------------------------------------
int		KItemList::GetSameDetailItemNum(int nImmediatePos)
{
	if (nImmediatePos < 0 || nImmediatePos >= IMMEDIACY_ROOM_WIDTH * IMMEDIACY_ROOM_HEIGHT)
		return 0;
	int		nIdx = m_Room[room_immediacy].FindItem(nImmediatePos, 0);
	if (nIdx <= 0)
		return 0;
	return m_Room[room_equipment].CalcSameDetailType(Item[nIdx].GetGenre(), Item[nIdx].GetDetailType()) + Item[nIdx].GetStackNum();
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------------
//	功能：获取包袱的同种物品的 叠加数量
//-------------------------------------------------------------------------------
int		KItemList::GetSameEquipmentItemNum(int nIdx)
{
	//if (nImmediatePos < 0 || nImmediatePos >= EQUIPMENT_ROOM_WIDTH * EQUIPMENT_ROOM_HEIGHT)
	//	return 0;
	//int		nIdx = m_Room[room_equipment].FindItem(nImmediatePos, 0);
	if (nIdx <= 0)
		return 0;
	return m_Room[room_equipment].CalcSameDetailType(Item[nIdx].GetGenre(), Item[nIdx].GetDetailType()) /*+ Item[nIdx].GetStackNum()*/;
}
#endif

#ifdef _SERVER
void KItemList::Abrade(int nType)  //磨损提示！
{
	int nItemIdx = 0;
	char nEgName[64];
	ZeroMemory(nEgName, sizeof(nEgName));
	for (int i = 0; i < itempart_num; ++i)
	{
		//if (i == itempart_horse || i == itempart_mask || i == itempart_amulet || i == itempart_ring1 || i == itempart_ring2 || i == itempart_pendant || i == itempart_pifeng || i == itempart_yinjian || i == itempart_shiping)
		//	continue;

		nItemIdx = m_EquipItem[i]; //装备着的装备索引

		if (nItemIdx)
		{ 
			int nOldDur = Item[nItemIdx].GetDurability();  //当前持久

			if(nOldDur == 0 || Item[nItemIdx].GetMaxDurability() <= 0) //do ben = 0 bo qua
				continue;
			 
            char msg[64];
			switch(i)
			{
			case itempart_head:
				sprintf(nEgName,"Non");
				break;
			case itempart_body:
				sprintf(nEgName,"Y phuc");
				break;
			case itempart_belt:
				sprintf(nEgName,"Dai lung");
				break;
			case itempart_weapon:
				sprintf(nEgName,"Vu khi");
				break;
			case itempart_foot:
				sprintf(nEgName,"Giay");
				break;
			case itempart_cuff:
				sprintf(nEgName,"Bao tay");
				break;
			case itempart_amulet:
				sprintf(nEgName,"Day chuyen");
				break;
			case itempart_ring1:
				sprintf(nEgName,"Nhan 1");
				break;
			case itempart_ring2:
				sprintf(nEgName,"Nhan 2");
				break;
			case itempart_pendant:
				sprintf(nEgName,"Ngoc boi");
				break;
			case itempart_horse:
				sprintf(nEgName,"Ngua");
				break;
			case itempart_mask:
				sprintf(nEgName,"Mat na");
				break;
			case itempart_pifeng:
				sprintf(nEgName,"Phi phong");
				break;
			case itempart_yinjian:
				sprintf(nEgName,"An");
				break;
			case itempart_shiping:
				sprintf(nEgName,"Trang suc");
				break;
			default:
				sprintf(nEgName,"default");
			}

			if (Item[nItemIdx].GetGenre()==0 && Item[nItemIdx].GetTime()->bYear > 0 && Item[nItemIdx].GetIsCanUse()==0)
			{    
				snprintf(msg, sizeof(msg), "Trang bi %s nay da hong hoac het han su dung!",nEgName);
				msgshow(msg);
			}
            
			if  (Item[nItemIdx].GetDetailType() != itempart_mask && nOldDur > 2 && nOldDur <= 5 && Item[nItemIdx].GetGenre()==0)
			{
				if (Item[nItemIdx].GetIsCanUse()==1)  
					Item[nItemIdx].SetIsCanUse(1);    //设置不能使用 
				snprintf(msg, sizeof(msg), "Trang bi %s nay sap hong!",nEgName);
				msgshow(msg); 
			}
			if  (Item[nItemIdx].GetDetailType() != itempart_mask && nOldDur == 1 && Item[nItemIdx].GetGenre()==0)
			{
				snprintf(msg, sizeof(msg), "Trang bi %s ton hai!",nEgName);
				if (Item[nItemIdx].GetIsCanUse()==1)  
					Item[nItemIdx].SetIsCanUse(1);    //设置不能使用 		
					msgshow(msg);

					//int nSplace=GetEquPlace(Item[nItemIdx].GetDetailType(), nItemIdx); //这个能装备的位置
					//UnEquip(nItemIdx,nSplace); 

					//int nItemindex = SearchItemsIdx(nItemIdx);	
					//AutoEquip(sScrPos,dDesPos, nItemindex, nSplace, 0);*/

				return;
			} 


			int nDur = Item[nItemIdx].Abrade(ItemSet.GetAbradeRange(nType, i));  //磨损后剩下的持?

			if(nDur == -1)//do ben -1 bo qua
				continue;

			if (nDur == 0)  //耐久度为零是 删除装备
			{
				// 给客户端发消息
				SHOW_MSG_SYNC	sMsg;
				sMsg.ProtocolType = s2c_msgshow;
				sMsg.m_wMsgID = enumMSG_ID_ITEM_DAMAGED;
				sMsg.m_wLength = sizeof(SHOW_MSG_SYNC);
				sMsg.m_lpBuf = Item[nItemIdx].m_dwID;
				if (g_pServer)
					g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE *)&sMsg, sMsg.m_wLength);
				sMsg.m_lpBuf = 0; 
                 
				//Item[nItemIdx].SetDurability(0);

				//Remove(nItemIdx);  //删除物品？ //khong cho phep xoa
				//ItemSet.Remove(nItemIdx);
			}
			else if (nOldDur != nDur && nDur != -1)  // 
			{//磨损警告
				ITEM_DURABILITY_CHANGE sIDC;
				sIDC.ProtocolType = s2c_itemdurabilitychange;
				sIDC.dwItemID = Item[nItemIdx].GetID();
				sIDC.nChange  = nDur - nOldDur;
				sIDC.nCanUse  = 1;
			  
				if (g_pServer)
					g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, &sIDC, sizeof(ITEM_DURABILITY_CHANGE));
			}
		}
	}
}

//换装磨损
void KItemList::EquipAbrade(int nItemIdx,int nType,int nPart)  //磨损提示！
{
		if (nItemIdx)
		{
			int nOldDur = Item[nItemIdx].GetDurability();

			int nDur = Item[nItemIdx].Abrade(ItemSet.GetAbradeRange(nType, nPart));  //返回磨损概率

			if (nDur == 0)  //耐久度为零是 删除装备
			{
				//给客户端发消息
				SHOW_MSG_SYNC	sMsg;
				sMsg.ProtocolType = s2c_msgshow;
				sMsg.m_wMsgID     = enumMSG_ID_ITEM_DAMAGED;
				sMsg.m_wLength    = sizeof(SHOW_MSG_SYNC) - 1;
				sMsg.m_lpBuf      = Item[nItemIdx].m_dwID;
				if (g_pServer)
					g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx,(BYTE *)&sMsg, sMsg.m_wLength + 1);
				sMsg.m_lpBuf      = 0;
				 
				//Remove(nItemIdx);  //删除物品？
				//ItemSet.Remove(nItemIdx);
			}
			else if (nOldDur != nDur && nDur != -1)//goi ham trang bi hong
			{////磨损警告-同步持久
				ITEM_DURABILITY_CHANGE sIDC;
				sIDC.ProtocolType = s2c_itemdurabilitychange;
				sIDC.dwItemID = Item[nItemIdx].GetID();
				sIDC.nChange  = nDur - nOldDur;
				sIDC.nCanUse  = 1;
				if (g_pServer)
					g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, &sIDC, sizeof(ITEM_DURABILITY_CHANGE));
			}
		}
}

#endif

//黄金套装激活
BOOL KItemList::GetIfActive()
{
	int nSet[9][2],bIfGet,nNum = 0;
	ZeroMemory(nSet,sizeof(nSet));
	int i;
	for (i = 0;i < itempart_num;++i)  
	{
		bIfGet = 0;

		if  (m_EquipItem[i]<= 0 || m_EquipItem[i]>=MAX_ITEM) continue;

	    if (Item[m_EquipItem[i]].GetGoldId() && Item[m_EquipItem[i]].GetIsPlatima() && Npc[Player[m_PlayerIdx].m_nIndex].m_CurrentAllJiHuo<=0)  //如果有部位是白金
		{//白金 并且 没有激活属性
			for (int j = 0;j < 9;++j)  //9件
			{
				if (nSet[j][0] == Item[m_EquipItem[i]].GetSet())  //所在套装
				{
					nSet[j][1]++;  //套装数量增加
					bIfGet = 1;   
					if (nSet[j][1] >= Item[m_EquipItem[i]].GetSetNum()) //套装数量
					{
						m_bActiveSet = TRUE;
						return TRUE;
					}
				}
			}
			
			if (!bIfGet)  // 不是套装的白金  套装数为0或1的 全部激活
			{
				nSet[nNum][0] = Item[m_EquipItem[i]].GetSet(); // 所在套装
				nSet[nNum][1] = 1;
				
				if (nSet[nNum][1] >= Item[m_EquipItem[i]].GetSetNum()) //套装数量
				{
					m_bActiveSet = TRUE;  //全身激活
					return TRUE;
				}
				nNum++;
			} 
		}
		else if (Item[m_EquipItem[i]].GetGoldId() && Npc[Player[m_PlayerIdx].m_nIndex].m_CurrentAllJiHuo<=0)  //如果有部位是黄金
		{
			for (int j = 0;j < 9;++j)  //9件
			{
				if (nSet[j][0] == Item[m_EquipItem[i]].GetSet())  //所在套装
				{
					nSet[j][1]++;  //套装数量增加
					bIfGet = 1;   
					if (nSet[j][1] >= Item[m_EquipItem[i]].GetSetNum()) //套装数量
					{
						m_bActiveSet = TRUE;
						return TRUE;
					}
				}
			}

			if (!bIfGet)  // 不是套装的黄金  套装数为0或1的 全部激活
			{
				nSet[nNum][0] = Item[m_EquipItem[i]].GetSet(); // 所在套装
				nSet[nNum][1] = 1;
				
				if (nSet[nNum][1] >= Item[m_EquipItem[i]].GetSetNum()) //套装数量
				{
					m_bActiveSet = TRUE;  //全身激活
					return TRUE;
				}
				nNum++;
			} 
		}
        else if (Item[m_EquipItem[i]].IsBlue() && /*(Item[m_EquipItem[i]].GetPoint()==7 || */Npc[Player[m_PlayerIdx].m_nIndex].m_CurrentAllJiHuo>0)
		{//是蓝装印鉴 或 其他部位的
			 int nIdx=itempart_yinjian;
			  
			 nIdx =  _gameGlobalSet.gameItemInfo.nEquipAllActive;
			//     g_GameSetTing.GetInteger("SYSTEM","EquipAllActive",itempart_yinjian,&nIdx);

			 if (i==nIdx)
			 {
				 m_bActiveSet = TRUE;  //全身激活
				 return TRUE;
			 }
			 else if (nIdx==-1)
			 {//其他部位的激活 
			     m_bActiveSet = TRUE;  //全身激活
				 return TRUE;
			 }
			              
		}
		else if (Item[m_EquipItem[i]].GetGoldId() && Npc[Player[m_PlayerIdx].m_nIndex].m_CurrentAllJiHuo>0)
		{//黄金饰品
			int nIdx=itempart_shiping;

			 nIdx =  _gameGlobalSet.gameItemInfo.nGoldAllActive;
			//g_GameSetTing.GetInteger("SYSTEM","GoldAllActive",itempart_shiping,&nIdx);
			
			if (i==nIdx)
			{
				m_bActiveSet = TRUE;  //全身激活
				return TRUE;
			}
			else if (nIdx==-1)
			{
			     m_bActiveSet = TRUE;  //全身激活
				 return TRUE;
			}
		}
	}

	/*for(i=53;i<78;++i)
	{     //其他方案
			if (Item[m_ItemBox[53]].IsBlue()&&Item[m_ItemBox[54]].IsBlue()&&Item[m_ItemBox[55]].IsBlue()&&Item[m_ItemBox[56]].IsBlue()&&Item[m_ItemBox[57]].IsBlue())
			{//金系
				m_bActiveSet = TRUE;  //全身激活
				return TRUE;
			}
			else if	(Item[m_ItemBox[58]].IsBlue()&&Item[m_ItemBox[59]].IsBlue()&&Item[m_ItemBox[60]].IsBlue()&&Item[m_ItemBox[61]].IsBlue()&&Item[m_ItemBox[62]].IsBlue())
			{//木系
				m_bActiveSet = TRUE;  //全身激活
				return TRUE;
			}
			else if	(Item[m_ItemBox[63]].IsBlue()&&Item[m_ItemBox[64]].IsBlue()&&Item[m_ItemBox[65]].IsBlue()&&Item[m_ItemBox[66]].IsBlue()&&Item[m_ItemBox[67]].IsBlue())
			{//水系
				m_bActiveSet = TRUE;  //全身激活
				return TRUE;
			}
			else if	(Item[m_ItemBox[68]].IsBlue()&&Item[m_ItemBox[69]].IsBlue()&&Item[m_ItemBox[70]].IsBlue()&&Item[m_ItemBox[71]].IsBlue()&&Item[m_ItemBox[72]].IsBlue())
			{//火系
				m_bActiveSet = TRUE;  //全身激活
				return TRUE;
			}
			else if	(Item[m_ItemBox[73]].IsBlue()&&Item[m_ItemBox[74]].IsBlue()&&Item[m_ItemBox[75]].IsBlue()&&Item[m_ItemBox[76]].IsBlue()&&Item[m_ItemBox[77]].IsBlue())
			{//土系
				m_bActiveSet = TRUE;  //全身激活
				return TRUE;
			}
	}*/

	m_bActiveSet = FALSE;  //设定不激活
	return FALSE;
}

#ifndef _SERVER
int KItemList::GetGoldColor(int nSet,int nId )
{
	int i;
	for (i = 0;i < itempart_num;++i)
	{//装备着的
		if (Item[m_EquipItem[i]].GetSet() == nSet && Item[m_EquipItem[i]].GetSetId() == nId)
		{
			return 2;
		}
	}

	for (i = 0;i < MAX_PLAYER_ITEM;++i)
	{//所有东西
		if (Item[m_Items[i].nIdx].GetSet() == nSet && Item[m_Items[i].nIdx].GetSetId() == nId)
		{
			return 1;
		}
	}

	return 0;
}
#endif

#ifndef	_SERVER
void KItemList::CheckBianShiItemTime()
{
	time_t rawtime;
	struct tm * timeinfo;
	time (&rawtime);
	timeinfo = localtime (&rawtime);   //今天的时间
for (int i = 0;i < MAX_PLAYER_ITEM;++i)
{
 if (Item[m_Items[i].nIdx].GetIsWhere()>0)
 {//是辨识装备
	if (Item[m_Items[i].nIdx].GetTime()->bYear)
		if (Item[m_Items[i].nIdx].GetTime()->bYear - 1900 < timeinfo->tm_year)
		{
			if (Item[m_Items[i].nIdx].GetIsCanUse()==1)
			{
				Item[m_Items[i].nIdx].SetIsCanUse(0);    //设置不能使用 	
				//SynPlayerItem(m_Items[i].nIdx);
				if (m_Items[i].nPlace == pos_equip)
				{//卸载这个装备
					TimeUnEquip(m_Items[i].nIdx,m_Items[i].nX);
				} 
			}
		}
		else if (Item[m_Items[i].nIdx].GetTime()->bYear - 1900 == timeinfo->tm_year)
			if (Item[m_Items[i].nIdx].GetTime()->bMonth < timeinfo->tm_mon + 1)
			{
				if (Item[m_Items[i].nIdx].GetIsCanUse()==1)
				{
					Item[m_Items[i].nIdx].SetIsCanUse(0);    //设置不能使用 
					//SynPlayerItem(m_Items[i].nIdx);
					if (m_Items[i].nPlace == pos_equip)
					{//卸载这个装备
						TimeUnEquip(m_Items[i].nIdx,m_Items[i].nX);
					} 
				}
			}
			else if (Item[m_Items[i].nIdx].GetTime()->bMonth == timeinfo->tm_mon + 1)
				if (Item[m_Items[i].nIdx].GetTime()->bDay < timeinfo->tm_mday)
				{
					if (Item[m_Items[i].nIdx].GetIsCanUse()==1)
					{
						Item[m_Items[i].nIdx].SetIsCanUse(0);    //设置不能使用
						//SynPlayerItem(m_Items[i].nIdx);
						if (m_Items[i].nPlace == pos_equip)
						{//卸载这个装备
							TimeUnEquip(m_Items[i].nIdx,m_Items[i].nX);
						} 
					}
				}
				else if (Item[m_Items[i].nIdx].GetTime()->bDay == timeinfo->tm_mday)
					if (Item[m_Items[i].nIdx].GetTime()->bHour < timeinfo->tm_hour)
					{
						if (Item[m_Items[i].nIdx].GetIsCanUse()==1)
						{
							Item[m_Items[i].nIdx].SetIsCanUse(0);    //设置不能使用
							//SynPlayerItem(m_Items[i].nIdx);
							if (m_Items[i].nPlace == pos_equip)
							{//卸载这个装备
								TimeUnEquip(m_Items[i].nIdx,m_Items[i].nX);
							} 
						}
					}
					else if (Item[m_Items[i].nIdx].GetTime()->bHour == timeinfo->tm_hour) 
						if (Item[m_Items[i].nIdx].GetTime()->bMin <= timeinfo->tm_min)
						{
							if (Item[m_Items[i].nIdx].GetIsCanUse()==1)
							{
								Item[m_Items[i].nIdx].SetIsCanUse(0);    //设置不能使用	
								//SynPlayerItem(m_Items[i].nIdx);
								if (m_Items[i].nPlace == pos_equip)
								{//卸载这个装备
									TimeUnEquip(m_Items[i].nIdx,m_Items[i].nX);
								} 
							}
						}							
						//-------------------------------------同步到客户端
	} 
//-------------------------------------所有装备持久为零的
   if (Item[m_Items[i].nIdx].GetIsCanUse()==1 && Item[m_Items[i].nIdx].GetDurability()>0 && Item[m_Items[i].nIdx].GetDurability()<=3)  
   {//
		   Item[m_Items[i].nIdx].SetIsCanUse(0);    //设置不能使用 	
		   //SynPlayerItem(m_Items[i].nIdx);
		   if (m_Items[i].nPlace == pos_equip)
		   {//卸载这个装备
			   TimeUnEquip(m_Items[i].nIdx,m_Items[i].nX);
		   } 	   
   }
//-------------------------------------- 
}
}
#endif

#ifdef _SERVER

void KItemList::SetPrice(int nIdx, int nPrice )
{
	int nGameId = SearchID(nIdx);
	int nId = FindSame(nGameId);
	if (nPrice)
	{
		m_Items[nId].nPrice = nPrice;
	} 
	else
	{
		m_Items[nId].nPrice = 0;
	}
}

int KItemList::GetPrice( int nGameIdx )
{
	int nId = FindSame(nGameIdx);
	if (nId)
		return m_Items[nId].nPrice;
	else
		return 0;
}



void KItemList::setLock(int nIdx, int ver,bool synClient)
{
	int nGameId = SearchID(nIdx);
	int nId = FindSame(nGameId);
	if (nId)
	{
		m_Items[nId].nIsLock = ver;

		if  (synClient)
		{//是否同步到客户端 
			PLAYER_AUTOMUVE_ITEM_COMMAND sSync;
			sSync.ProtocolType = s2c_returnitemlock;
			sSync.m_nItemID = nIdx;
			sSync.m_btPlace = ver;
			sSync.m_btX = 0;
			sSync.m_btY = 0;
			sSync.m_destRoom = -1;
			sSync.m_ItemWidth = 0;
			sSync.m_ItemHigth = 0;
			if (g_pServer)
				g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sSync, sizeof(PLAYER_AUTOMUVE_ITEM_COMMAND));
			// printf("------setLock:发包成功　%s------\n",Player[m_PlayerIdx].m_PlayerName);		
		}
	}
}

int KItemList::getLock(int nIdx)
{
	int nId = FindSame(nIdx);
	if (nId)
		return m_Items[nId].nIsLock;
	else
		return 0;
}

void KItemList::SetLianFlg( int nIdx, BYTE mLianJieFlg )
{
	int nGameId = SearchID(nIdx);
	int nId = FindSame(nGameId);
	if (mLianJieFlg)
	{
		m_Items[nId].nLianJieFlg = mLianJieFlg;
	} 
	else
	{
		m_Items[nId].nLianJieFlg = 0;
	}
}

BYTE KItemList::GetLianFlg( int nGameIdx )
{
	int nId = FindSame(nGameIdx);
	if (nId)
		return m_Items[nId].nLianJieFlg;
	else
		return 0;
}

void KItemList::SetModel( int nIdx, BYTE mLianJieFlg)
{
	int nGameId = SearchID(nIdx);
	int nId = FindSame(nGameId);
	if (mLianJieFlg)
	{
		m_Items[nId].nModel   = mLianJieFlg;
		//m_Items[nId].nModel = mLianJieFlg;
		Item[m_Items[nId].nIdx].SetModel(mLianJieFlg);
	} 
	else
	{
		m_Items[nId].nModel = 0;
		Item[m_Items[nId].nIdx].SetModel(0);
	}
}

BYTE KItemList::GetModel( int nGameIdx )
{
	int nId = FindSame(nGameIdx);
	if (nId)
		return m_Items[nId].nModel;
	else
		return 0;
}

// 检查物品的使用时间 是否到期
BOOL KItemList::CheckIsOutTime(int nItemIdx)
{
	time_t rawtime;
	struct tm * timeinfo;
	time (&rawtime);
	timeinfo = localtime(&rawtime);   //今天的时间
    BOOL nReg=FALSE;
   if (Item[nItemIdx].GetIsWhere()>0)
   { //是辨识装备
	   if (Item[nItemIdx].GetTime()->bYear)
		   if (Item[nItemIdx].GetTime()->bYear - 1900 < timeinfo->tm_year)
		   {
                 nReg=TRUE;      
		   }
		   else if (Item[nItemIdx].GetTime()->bYear - 1900 == timeinfo->tm_year)
			   if (Item[nItemIdx].GetTime()->bMonth < timeinfo->tm_mon + 1)
			   {
                  nReg=TRUE; 
			   }
			   else if (Item[nItemIdx].GetTime()->bMonth == timeinfo->tm_mon + 1)
				   if (Item[nItemIdx].GetTime()->bDay < timeinfo->tm_mday)
				   {
				      nReg=TRUE; 
				   }
				   else if (Item[nItemIdx].GetTime()->bDay == timeinfo->tm_mday)
					   if (Item[nItemIdx].GetTime()->bHour < timeinfo->tm_hour)
					   {
                         nReg=TRUE; 
					   }
					   else if (Item[nItemIdx].GetTime()->bHour == timeinfo->tm_hour) 
						   if (Item[nItemIdx].GetTime()->bMin <= timeinfo->tm_min)
						   {
                               nReg=TRUE; 
						   }						
	} 

	return 	nReg;
}
// 检查城主  或 帮战地图的 武器
void KItemList::CheckIsCityAndBang()
{
    for (int i = 0; i < MAX_PLAYER_ITEM; ++i)
    {
        const int itemId = m_Items[i].nIdx;
        if (itemId <= 0 || itemId >= MAX_ITEM)
            continue;
        if (Item[itemId].CheckReqability(48))
        {
            if (Remove(itemId))
                ItemSet.Remove(itemId);
        }
    }
}

void KItemList::CheckItemTime()
{
	time_t rawtime;
	struct tm * timeinfo;
	time (&rawtime);
	timeinfo = localtime (&rawtime);   //今天的时间
  for (int i = 0;i < MAX_PLAYER_ITEM;++i)
  {
   BOOL isDel = FALSE;
   if (Item[m_Items[i].nIdx].GetIsWhere()<=0)
   {//不是辨识装   
	 if (Item[m_Items[i].nIdx].GetTime()->bYear)
	 { 
	  	if (Item[m_Items[i].nIdx].GetTime()->bYear - 1900 < timeinfo->tm_year)
		{
			Remove(m_Items[i].nIdx);
			ItemSet.Remove(m_Items[i].nIdx);
			isDel=TRUE;
		}
		else if (Item[m_Items[i].nIdx].GetTime()->bYear - 1900 == timeinfo->tm_year)
			if (Item[m_Items[i].nIdx].GetTime()->bMonth < timeinfo->tm_mon + 1)
			{
				Remove(m_Items[i].nIdx);
				ItemSet.Remove(m_Items[i].nIdx);
				isDel=TRUE;
			}
			else if (Item[m_Items[i].nIdx].GetTime()->bMonth == timeinfo->tm_mon + 1)
				if (Item[m_Items[i].nIdx].GetTime()->bDay < timeinfo->tm_mday)
				{
					Remove(m_Items[i].nIdx);
					ItemSet.Remove(m_Items[i].nIdx);
					isDel=TRUE;
				}
				else if (Item[m_Items[i].nIdx].GetTime()->bDay == timeinfo->tm_mday)
					if (Item[m_Items[i].nIdx].GetTime()->bHour < timeinfo->tm_hour)
					{
						Remove(m_Items[i].nIdx);
						ItemSet.Remove(m_Items[i].nIdx);
						isDel=TRUE;
					}
					else if (Item[m_Items[i].nIdx].GetTime()->bHour == timeinfo->tm_hour) 
						if (Item[m_Items[i].nIdx].GetTime()->bMin <= timeinfo->tm_min)
						{
							Remove(m_Items[i].nIdx);
							ItemSet.Remove(m_Items[i].nIdx);
							isDel=TRUE;
						} 
	 }  
   } 
   else if (Item[m_Items[i].nIdx].GetIsWhere()>0)
   { //是辨识装备
	   if (Item[m_Items[i].nIdx].GetTime()->bYear)
		   if (Item[m_Items[i].nIdx].GetTime()->bYear - 1900 < timeinfo->tm_year)
		   {
			   if (Item[m_Items[i].nIdx].GetIsCanUse()==1)
			   {
				   Item[m_Items[i].nIdx].SetIsCanUse(0);    //设置不能使用 	
				   //SynPlayerItem(m_Items[i].nIdx);
				   if (m_Items[i].nPlace == pos_equip)
				   {//卸载这个装备
					   TimeUnEquip(m_Items[i].nIdx,m_Items[i].nX);
				   } 
			   }
		   }
		   else if (Item[m_Items[i].nIdx].GetTime()->bYear - 1900 == timeinfo->tm_year)
			   if (Item[m_Items[i].nIdx].GetTime()->bMonth < timeinfo->tm_mon + 1)
			   {
				   if (Item[m_Items[i].nIdx].GetIsCanUse()==1)
				   {
					   Item[m_Items[i].nIdx].SetIsCanUse(0);    //设置不能使用 
					   //SynPlayerItem(m_Items[i].nIdx);
					   if (m_Items[i].nPlace == pos_equip)
					   {//卸载这个装备
						   TimeUnEquip(m_Items[i].nIdx,m_Items[i].nX);
					   } 
				   }
			   }
			   else if (Item[m_Items[i].nIdx].GetTime()->bMonth == timeinfo->tm_mon + 1)
				   if (Item[m_Items[i].nIdx].GetTime()->bDay < timeinfo->tm_mday)
				   {
					   if (Item[m_Items[i].nIdx].GetIsCanUse()==1)
					   {
						   Item[m_Items[i].nIdx].SetIsCanUse(0);    //设置不能使用
						  // SynPlayerItem(m_Items[i].nIdx);
						   if (m_Items[i].nPlace == pos_equip)
						   {//卸载这个装备
							   TimeUnEquip(m_Items[i].nIdx,m_Items[i].nX);
						   } 
					   }
				   }
				   else if (Item[m_Items[i].nIdx].GetTime()->bDay == timeinfo->tm_mday)
					   if (Item[m_Items[i].nIdx].GetTime()->bHour < timeinfo->tm_hour)
					   {
						   if (Item[m_Items[i].nIdx].GetIsCanUse()==1)
						   {
							   Item[m_Items[i].nIdx].SetIsCanUse(0);    //设置不能使用
							   //SynPlayerItem(m_Items[i].nIdx);
							   if (m_Items[i].nPlace == pos_equip)
							   {//卸载这个装备
								   TimeUnEquip(m_Items[i].nIdx,m_Items[i].nX);
							   } 
						   }
					   }
					   else if (Item[m_Items[i].nIdx].GetTime()->bHour == timeinfo->tm_hour) 
						   if (Item[m_Items[i].nIdx].GetTime()->bMin <= timeinfo->tm_min)
						   {
							   if (Item[m_Items[i].nIdx].GetIsCanUse()==1)
							   {
								   Item[m_Items[i].nIdx].SetIsCanUse(0);    //设置不能使用	
								   //SynPlayerItem(m_Items[i].nIdx);
								   if (m_Items[i].nPlace == pos_equip)
								   {//卸载这个装备
									   TimeUnEquip(m_Items[i].nIdx,m_Items[i].nX);
								   } 
							   }
						   }							

	} 
//-------------------------------------所有装备持久为零的
      /*if (!isDel && Item[m_Items[i].nIdx].GetIsCanUse()==1 && Item[m_Items[i].nIdx].GetDurability()>0 && Item[m_Items[i].nIdx].GetDurability()<=3)  
	  {  
		   Item[m_Items[i].nIdx].SetIsCanUse(0);    //设置不能使用 	
		   //SynPlayerItem(m_Items[i].nIdx);
		   if (m_Items[i].nPlace == pos_equip)
		   {//卸载这个装备
			   TimeUnEquip(m_Items[i].nIdx,m_Items[i].nX);
		   } 	   
	  }*/
	  
	  if (!isDel &&/* Item[m_Items[i].nIdx].GetGenre()==item_equip && */Item[m_Items[i].nIdx].GetUseMapIdx()>0)
	  {
		  //int nX,nY,nMap;
		  //Npc[Player[m_PlayerIdx].m_nIndex].GetMpsPos();
		 if  (Item[m_Items[i].nIdx].GetUseKind()==0 && Item[m_Items[i].nIdx].GetUseMapIdx()!= SubWorld[Npc[Player[m_PlayerIdx].m_nIndex].m_SubWorldIndex].m_SubWorldID)
		 {//地图
			 Remove(m_Items[i].nIdx);
			 ItemSet.Remove(m_Items[i].nIdx);
		 }
		 else
		 {//等级
			 int nItemUseLevel=1;
			     
			 nItemUseLevel =  _gameGlobalSet.gameItemInfo.nItemUseLevel;
			// g_GameSetTing.GetInteger("SYSTEM","ItemUseLevel",1,&nItemUseLevel);

			 if (Item[m_Items[i].nIdx].GetUseKind()==1 && (Item[m_Items[i].nIdx].GetUseMapIdx() >= nItemUseLevel || Npc[Player[m_PlayerIdx].m_nIndex].m_Level >= Item[m_Items[i].nIdx].GetUseMapIdx()))
			 {
				 Remove(m_Items[i].nIdx);
			     ItemSet.Remove(m_Items[i].nIdx);
			 }
			 else if (Item[m_Items[i].nIdx].GetUseKind()>1)
			 {
				  Remove(m_Items[i].nIdx);
			      ItemSet.Remove(m_Items[i].nIdx);
			 }
		 }
	  }
//-------------------------------------------------------
  }
}
// 删除黄金装备
BOOL KItemList::RemoveGoldItem(int nDetailType)
{
    if (nDetailType <= 0)
        return FALSE;
    for (int i = 0; i < MAX_PLAYER_ITEM; ++i)
    {
        if (m_Items[i].nPlace != pos_equiproom)
            continue;
        const int itemId = m_Items[i].nIdx;
        if (itemId <= 0 || itemId >= MAX_ITEM)
            continue;
        if (Item[itemId].GetGoldId() == nDetailType && Remove(itemId))
        {
            ItemSet.Remove(itemId);
            return TRUE;
        }
    }
    return FALSE;
}
BOOL KItemList::IsGoldItemExist( int nDetailType)
{
	if (nDetailType <= 0)
	{
		return FALSE;
	}
	for (int i = 0;i < MAX_PLAYER_ITEM;++i)
	{
		if (m_Items[i].nPlace != pos_equiproom)	 //room_equipment
		{
			continue;
		}
		if (Item[m_Items[i].nIdx].GetGoldId() == nDetailType)
		{
			return TRUE;
		}
	}
	return FALSE;
}
#endif

// mat na
void KItemList::SetMaskLock( BOOL bFlag )
{
	m_nMaskLock = bFlag;
#ifdef _SERVER
	NPC_SIT_SYNC Sync;
	Sync.ProtocolType = s2c_syncmasklock;
	Sync.ID = m_nMaskLock;
	g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&Sync, sizeof(NPC_SIT_SYNC));
#endif
}
//身上是否有这个相同属性物品
#ifdef _SERVER

int	KItemList::GetItemIndexByidx(int nCountIdx,int nRongType)
{
	int	nNo = 0,nIdx = 0,nCurItemIdx=0;

	while ((nIdx = m_UseIdxItemList.GetNext(nIdx)))
	{//m_Items[nIdx].nIdx

			if (m_Items[nIdx].nPlace == nRongType)
			{
				nNo++;
				printf("--第 %d 个物品:%s--\n",nNo,Item[m_Items[nIdx].nIdx].GetName());

				if (nNo==nCountIdx)
				{
                   nCurItemIdx = m_Items[nIdx].nIdx;
				   break;
				}
			}
	}

	return nCurItemIdx;
}

int KItemList::IsItemExist( int nGern,int nDetailType,int nPar,int nSerise,int nLevel )
{//检测蓝装
	int nIdx = 0;
	while((nIdx = m_UseIdxItemList.GetNext(nIdx)))
	{
		if (Item[m_Items[nIdx].nIdx].GetGenre() != nGern ||
			Item[m_Items[nIdx].nIdx].GetDetailType() != nDetailType ||
			Item[m_Items[nIdx].nIdx].GetParticular() != nPar
			|| Item[m_Items[nIdx].nIdx].GetGoldId() || Item[m_Items[nIdx].nIdx].IsPurple()>0)
			continue;		

		if (nSerise < 5)
			if (Item[m_Items[nIdx].nIdx].GetSeries() != nSerise)
				continue;

		if (nLevel != 0)
			if (Item[m_Items[nIdx].nIdx].GetLevel() != nLevel)
				continue;

		return m_Items[nIdx].nIdx;
	}
	return FALSE;
}

BOOL KItemList::CheckItemInfo(int nGen,int nDetail,int nParticular,char *KeyName)
{
	int nItemInfo[3];
	int nRows = g_ForbitMap.GetHeight();
	bool nReturn = false;

	for (int i = 2;i<nRows+1;i++)
	{
		g_ForbitMap.GetInt3s(i,KeyName,nItemInfo);

		//printf("nItemInfo:%d %d %d",nItemInfo[0],nItemInfo[1],nItemInfo[2]);
		if (nItemInfo[0] == nGen && nItemInfo[1]==nDetail && nItemInfo[2]==nParticular)
		{
			nReturn = true;
			break;
		}
	}
	return nReturn;
}
int KItemList::IsHaveItem( int nGern,int nDetailType,int nPar,int nSerise,int nLevel,int nPlace)
{//检测蓝装
	int nIdx = 0;
	while((nIdx = m_UseIdxItemList.GetNext(nIdx)))
	{
        if (m_Items[nIdx].nPlace != nPlace)  //包袱
			continue;

		if (Item[m_Items[nIdx].nIdx].GetGenre() != nGern ||
			Item[m_Items[nIdx].nIdx].GetDetailType() != nDetailType ||
			Item[m_Items[nIdx].nIdx].GetParticular() != nPar
			|| Item[m_Items[nIdx].nIdx].GetGoldId() || Item[m_Items[nIdx].nIdx].IsPurple()>0)
			continue;		

		if (nSerise < 5)
		{
			if (CheckItemInfo(nGern,nDetailType,nPar,"NeedSeries"))
			{//检测五行
				if (Item[m_Items[nIdx].nIdx].GetSeries() != nSerise)
					continue;
			}
		}

		if (nLevel != 0)
		{
			if (CheckItemInfo(nGern,nDetailType,nPar,"NeedLevel"))
			{//检测五行
			  if (Item[m_Items[nIdx].nIdx].GetLevel() != nLevel)
				continue;
			}
		}

		if (Item[m_Items[nIdx].nIdx].GetIsBang())
		   continue;
        
        if (Item[m_Items[nIdx].nIdx].GetStackNum() >= MAX_STACKNUM)
		   continue;
        

		return m_Items[nIdx].nIdx;
	}
	return FALSE;
}



///检查是否有这个名称的物品
int KItemList::IsItemName(char *nItemName)
{
	int nIdx = 0;
	while((nIdx = m_UseIdxItemList.GetNext(nIdx)))
	{
		if (strcmpi(nItemName,Item[m_Items[nIdx].nIdx].GetName()) != 0)
	//	if (Item[m_Items[nIdx].nIdx].GetName() != nLevel)
				continue;	
		     return m_Items[nIdx].nIdx;
	}
	return FALSE;
}


int KItemList::GetCountWithName(char *nItemName)
{
	int nIdx = 0,nCount=0;

	if (nItemName==NULL)
		return 0;

	while((nIdx = m_UseIdxItemList.GetNext(nIdx)))
	{
		if (strstr(Item[m_Items[nIdx].nIdx].GetName(),nItemName))
		{	
			nCount +=Item[m_Items[nIdx].nIdx].GetStackNum();
		}
	}

	return nCount;
}


BOOL KItemList::DelItemWithName(char *nItemName)
{
    if (!nItemName || !nItemName[0])
        return FALSE;
    BOOL deleted = FALSE;
    for (int i = 0; i < MAX_PLAYER_ITEM; ++i)
    {
        const int itemId = m_Items[i].nIdx;
        if (itemId <= 0 || itemId >= MAX_ITEM)
            continue;
        const char *itemName = Item[itemId].GetName();
        if (itemName && strstr(itemName, nItemName) && Remove(itemId))
        {
            ItemSet.Remove(itemId);
            deleted = TRUE;
        }
    }
    return deleted;
}
BOOL KItemList::DelExistItem(int nGern, int nDetailType, int nPar, int nSerise, int nLevel)
{
    int slot = 0;
    while ((slot = m_UseIdxItemList.GetNext(slot)))
    {
        const int itemId = m_Items[slot].nIdx;
        if (itemId <= 0 || itemId >= MAX_ITEM)
            continue;
        if (Item[itemId].GetGenre() != nGern ||
            Item[itemId].GetDetailType() != nDetailType ||
            Item[itemId].GetParticular() != nPar ||
            Item[itemId].GetGoldId() ||
            Item[itemId].IsPurple() > 0)
            continue;
        if (nSerise < 5 && Item[itemId].GetSeries() != nSerise)
            continue;
        if (nLevel != 0 && Item[itemId].GetLevel() != nLevel)
            continue;
        if (!Remove(itemId))
            return FALSE;
        ItemSet.Remove(itemId);
        return TRUE;
    }
    return FALSE;
}
#endif
void KItemList::BackupGive()
{
	if (!m_Room[room_giveback].m_pArray)
		m_Room[room_giveback].Init(m_Room[room_equipment].m_nWidth, m_Room[room_equipment].m_nHeight);
	memcpy(m_Room[room_giveback].m_pArray, m_Room[room_equipment].m_pArray, sizeof(int) * m_Room[room_giveback].m_nWidth * m_Room[room_giveback].m_nHeight);
	
	memcpy(this->m_sBackItems, this->m_Items, sizeof(PlayerItem) * MAX_PLAYER_ITEM);
	
	m_nBackHand = m_Hand;
}
//恢复备份包袱的东西
void KItemList::RecoverGive()
{
	memcpy(m_Room[room_equipment].m_pArray, m_Room[room_giveback].m_pArray, sizeof(int) * m_Room[room_giveback].m_nWidth * m_Room[room_giveback].m_nHeight);

	memcpy(m_Items, m_sBackItems, sizeof(PlayerItem) * MAX_PLAYER_ITEM);
	m_Hand = m_nBackHand;
}
//开始备份
void KItemList::StartGive()
{
	BackupGive();
	ClearRoom(room_give);
}

//-----检查有多少件相同等级类型的装备--------
int	KItemList::GetItemNum(int nGenre, int nDetailType, int nParticular, int nLevel,ITEM_POSITION nRongType)
{
	int 	nNo = 0,nIdx = 0;
	while ((nIdx = m_UseIdxItemList.GetNext(nIdx)))
	{
		if ( nGenre == Item[m_Items[nIdx].nIdx].GetGenre() && 
				nDetailType == Item[m_Items[nIdx].nIdx].GetDetailType() && 
				nParticular == Item[m_Items[nIdx].nIdx].GetParticular())
			{

				if  (nRongType>=ITEM_POSITION::pos_hand && nRongType<ITEM_POSITION::pos_count && m_Items[nIdx].nPlace != nRongType)
				   continue;
		
				if (nLevel>0 && nLevel!=Item[m_Items[nIdx].nIdx].GetLevel())
					continue;
					
				nNo+= Item[m_Items[nIdx].nIdx].GetStackNum();
			}
	}
	return nNo;
}

int KItemList::GetItemCount(int nGenre, int nDetailType, int nParticular, int nlevel, int nSerise, int nRongType)
{
	//printf("%d-%d-%d\n", nGenre, nDetailType, nParticular);
	int	nNo = 0, nIdx = 0;

	while ((nIdx = m_UseIdxItemList.GetNext(nIdx)))
	{
		//printf("====================\n");
		//printf("%d-%d-%d\n", Item[m_Items[nIdx].nIdx].GetGenre(), Item[m_Items[nIdx].nIdx].GetDetailType(), Item[m_Items[nIdx].nIdx].GetParticular());
		//printf("====================\n");
		if (nGenre == Item[m_Items[nIdx].nIdx].GetGenre() &&
			nDetailType == Item[m_Items[nIdx].nIdx].GetDetailType() &&
			nParticular == Item[m_Items[nIdx].nIdx].GetParticular())
		{
			if (nRongType >= ITEM_POSITION::pos_hand && nRongType < ITEM_POSITION::pos_count && m_Items[nIdx].nPlace != nRongType)
				continue;

			if (nlevel > -1 && nlevel != Item[m_Items[nIdx].nIdx].GetLevel())
				continue;

			if (nSerise > -1 && nSerise != Item[m_Items[nIdx].nIdx].GetSeries())
				continue;

			nNo += Item[m_Items[nIdx].nIdx].GetStackNum();
		}
	}
	return nNo;
}

int KItemList::GetItemCountEx(int nDetailType)
{
	int	nNo = 0, nIdx = 0;

	while ((nIdx = m_UseIdxItemList.GetNext(nIdx)))
	{
		if (item_task == Item[m_Items[nIdx].nIdx].GetGenre() &&
			nDetailType == Item[m_Items[nIdx].nIdx].GetDetailType())
		{
			if (m_Items[nIdx].nPlace != pos_equiproom)
				continue;

			nNo += Item[m_Items[nIdx].nIdx].GetStackNum();
		}
	}
	return nNo;
}

int KItemList::RemoveItemTask(int nDetailType)
{
	int	 nNo = 0,nIdx = 0;
	while ((nIdx = m_UseIdxItemList.GetNext(nIdx)))
	{
		if (item_task == Item[m_Items[nIdx].nIdx].GetGenre() && 
			nDetailType == Item[m_Items[nIdx].nIdx].GetDetailType())
		{

			if  (m_Items[nIdx].nPlace != pos_equiproom)
				continue;

			int  stackCount = Item[m_Items[nIdx].nIdx].GetStackNum();

			if (stackCount > 1)
			{//如果叠加得数量大于 需要删除得物品 就 直接修改叠加得数量就可以了
				Item[m_Items[nIdx].nIdx].SetStackNum(stackCount - 1);
				SynPlayerItem(m_Items[nIdx].nIdx); //同步物品属性
			}
			else if (stackCount <= 1)
			{//数量不够得直接删除 直接删除
				const int itemIdToRemove = m_Items[nIdx].nIdx;
				if (itemIdToRemove > 0 && itemIdToRemove < MAX_ITEM && Remove(itemIdToRemove))
				{
				    ItemSet.Remove(itemIdToRemove);
				    nIdx = 0; // KLinkArray::Remove clears the deleted node's next link.
				}
			}
		}
	}

	return nNo;
}


BOOL	KItemList::FindSameToRemove(int nItemGenre, int nDetailType, int nParticular, int nLevel, int nSeries, int P, int *pnIdx)
{
	return m_Room[P].FindSameToRemove(nItemGenre, nDetailType, nParticular, nLevel, nSeries, pnIdx);
}

int KItemList::RemoveCommonItem(int nCount, int nItemGenre, int nDetailType, int nParticular, int nLevel, int nSeries, int P)
{
	printf("==========:RemoveCommonItem==========\n");
	int nIdx = 0, nResult = 0;
	if (nCount <= 0 || P < pos_hand || P >= pos_count) return 0;
	for (int i = 0; i < nCount; i++)
	{
				printf("====================\n");
				printf("RemoveCommonItem count=%d genre=%d detail=%d particular=%d level=%d series=%d room=%d\n", nCount, nItemGenre, nDetailType, nParticular, nLevel, nSeries, P);
				printf("====================\n");

		if (!FindSameToRemove(nItemGenre, nDetailType, nParticular, nLevel, nSeries, P, &nIdx))
			break;

		if (nIdx > 0 && nIdx < MAX_ITEM)
		{
			if (Item[nIdx].IsStack() && Item[nIdx].GetStackNum() > 1)
			{
				Item[nIdx].SetStackNum(Item[nIdx].GetStackNum() - 1);
				this->SynPlayerItem(nIdx);
				nResult ++;
				continue;
			}
			else
			{
				this->Remove(nIdx);
				ItemSet.Remove(nIdx);
				nResult ++;
				continue;
			}
		}
	}
	return nResult;
}

int KItemList::RemoveItemCount(int nItemCount, int nGenre, int nDetailType, int nParticular, int nlevel,int nSerise, int nRongType)
{
	int	 nNo = 0,nIdx = 0;
	// Refuse partial debits: report failure without mutating the inventory.
	if (nItemCount <= 0 || GetItemCount(nGenre,nDetailType,nParticular,nlevel,nSerise,nRongType) < nItemCount)
		return 0;
	while ((nIdx = m_UseIdxItemList.GetNext(nIdx)))
	{
		if ( nGenre == Item[m_Items[nIdx].nIdx].GetGenre() && 
			nDetailType == Item[m_Items[nIdx].nIdx].GetDetailType() && 
			nParticular == Item[m_Items[nIdx].nIdx].GetParticular())
		{
			if  (nItemCount<=0)
			{//删除成功了
				nNo = 1;
				break;
			}

			if  (nRongType >= ITEM_POSITION::pos_hand && nRongType<ITEM_POSITION::pos_count && m_Items[nIdx].nPlace != nRongType)
				continue;

			if (nlevel > -1 && nlevel != Item[m_Items[nIdx].nIdx].GetLevel())
			   continue;

			if (nSerise > -1 && nSerise != Item[m_Items[nIdx].nIdx].GetSeries())
				continue;

			int  stackCount = Item[m_Items[nIdx].nIdx].GetStackNum();

			if (stackCount > nItemCount)
			{//如果叠加得数量大于 需要删除得物品 就 直接修改叠加得数量就可以了
				Item[m_Items[nIdx].nIdx].SetStackNum(stackCount - nItemCount);
				SynPlayerItem(m_Items[nIdx].nIdx); //同步物品属性
			}
			else if (stackCount <= nItemCount)
			{//数量不够得直接删除 直接删除
				const int itemIdToRemove = m_Items[nIdx].nIdx;
				if (itemIdToRemove <= 0 || itemIdToRemove >= MAX_ITEM || !Remove(itemIdToRemove))
					return 0;
				ItemSet.Remove(itemIdToRemove);
				nIdx = 0; // KLinkArray::Remove clears the deleted node's next link.
			}
			nItemCount -=stackCount;
		}
	}

	return nItemCount <= 0 ? 1 : 0;
}


//扣除一定数量得物品
int KItemList::ConsumeItemCount(int nItemCount,int nGenre, int nDetailType, int nParticular,int nlevel, int nRongType)
{
	int	 nNo = 0,nIdx = 0, IsDel = 0;
	if (nItemCount <= 0 || GetItemCount(nGenre,nDetailType,nParticular,nlevel > 0 ? nlevel : -1,-1,nRongType) < nItemCount)
		return 0;
	while ((nIdx = m_UseIdxItemList.GetNext(nIdx)))
	{
		if ( nGenre == Item[m_Items[nIdx].nIdx].GetGenre() && 
			nDetailType == Item[m_Items[nIdx].nIdx].GetDetailType() && 
			nParticular == Item[m_Items[nIdx].nIdx].GetParticular())
		{
			if  (nItemCount<=0)
			{//删除成功了
				nNo = 1;
				break;
			}

			if  (nRongType>=ITEM_POSITION::pos_hand && nRongType<ITEM_POSITION::pos_count && m_Items[nIdx].nPlace != nRongType)
				continue;

			if (nlevel > 0 && nlevel != Item[m_Items[nIdx].nIdx].GetLevel())
				continue;

			int  stackCount = Item[m_Items[nIdx].nIdx].GetStackNum();

			if (stackCount > nItemCount)
			{//如果叠加得数量大于 需要删除得物品 就 直接修改叠加得数量就可以了
				Item[m_Items[nIdx].nIdx].SetStackNum(stackCount - nItemCount);
				//同步一下物品得 叠加数量
				SynPlayerItem(m_Items[nIdx].nIdx); //同步物品属性
				IsDel = 1;
			}
			else if (stackCount <= nItemCount)
			{//数量不够得直接删除 直接删除
				const int itemIdToRemove = m_Items[nIdx].nIdx;
				if (itemIdToRemove <= 0 || itemIdToRemove >= MAX_ITEM || !Remove(itemIdToRemove))
					return 0;
				ItemSet.Remove(itemIdToRemove);
				nIdx = 0; // KLinkArray::Remove clears the deleted node's next link.
				IsDel = 1;
			}
			nItemCount -=stackCount;
		}
	}

	return IsDel;
}

BOOL KItemList::GopItem(int nItemIndex, int nRongType)
{
    int nTotalStack = 0; 
	int nBeforeTotalStack = 0;
    std::vector<int> tempIdxList;
    int nIdx = 0;

    // Lay thong tin goc tu item dau vao
    int nGenre      = Item[nItemIndex].GetGenre();
    int nDetailType = Item[nItemIndex].GetDetailType();
    int nParticular = Item[nItemIndex].GetParticular();
    int nLevel      = Item[nItemIndex].GetLevel();
    int nSeries     = Item[nItemIndex].GetSeries();
    //int nRongType   = m_Items[nItemIndex].nPlace;

    int nYear  = Item[nItemIndex].GetTime()->bYear;
    int nMonth = Item[nItemIndex].GetTime()->bMonth;
    int nDay   = Item[nItemIndex].GetTime()->bDay;
    int nHour  = Item[nItemIndex].GetTime()->bHour;
    int nMin   = Item[nItemIndex].GetTime()->bMin;

    int nBang  = Item[nItemIndex].GetIsBang();
    int nBind  = Item[nItemIndex].GetLockItem()->nState;
	 
/*	 printf("GopItem: nItemIndex=%d Genre=%d, DetailType=%d, Particular=%d, Level=%d, Series=%d, Place=%d\n", 
           nItemIndex, nGenre, nDetailType, nParticular, nLevel, nSeries, nRongType);
    ("====================\n");
*/
    // 1. Tim cac item giong nhau de gop
    while ((nIdx = m_UseIdxItemList.GetNext(nIdx)))
    {
        if (Item[m_Items[nIdx].nIdx].GetStackNum() == MAX_STACKNUM)
            continue;

        if (Item[m_Items[nIdx].nIdx].GetGenre()      == nGenre &&
            Item[m_Items[nIdx].nIdx].GetDetailType() == nDetailType &&
            Item[m_Items[nIdx].nIdx].GetParticular() == nParticular &&
            Item[m_Items[nIdx].nIdx].GetLevel()      == nLevel &&
            Item[m_Items[nIdx].nIdx].GetSeries()     == nSeries &&
            Item[m_Items[nIdx].nIdx].GetTime()->bYear  == nYear &&
            Item[m_Items[nIdx].nIdx].GetTime()->bMonth == nMonth &&
            Item[m_Items[nIdx].nIdx].GetTime()->bDay   == nDay &&
            Item[m_Items[nIdx].nIdx].GetTime()->bHour  == nHour &&
            Item[m_Items[nIdx].nIdx].GetTime()->bMin   == nMin &&
            Item[m_Items[nIdx].nIdx].GetIsBang()       == nBang &&
            Item[m_Items[nIdx].nIdx].GetLockItem()->nState == nBind &&
            m_Items[nIdx].nPlace == nRongType)
        {
            tempIdxList.push_back(nIdx);
			nBeforeTotalStack += Item[m_Items[nIdx].nIdx].GetStackNum();
        }
    }

	// So voi lan gop truoc
	if (nBeforeTotalStack == m_nLastTotalStackedItem)
	{
		return FALSE;
	}

    // 2. Xoa item cu, cong don stack
    for (size_t i = 0; i < tempIdxList.size(); ++i)
    {
        int idx = tempIdxList[i];
        int currentStack = Item[m_Items[idx].nIdx].GetStackNum();
        nTotalStack += currentStack;

        Remove(m_Items[idx].nIdx);
        ItemSet.Remove(m_Items[idx].nIdx);
    }

    // 3. Tao lai cac item da gop
    while (nTotalStack >= MAX_STACKNUM)
    {
        int x, y;
        int nNewIndex = ItemSet.AddOther(nGenre, nSeries, nLevel, 10,
                                         nDetailType, nParticular, NULL,
                                         g_SubWorldSet.GetGameVersion(), 0, 1);

        if (CheckCanPlaceInEquipment(Item[nNewIndex].GetWidth(), Item[nNewIndex].GetHeight(), &x, &y))
        {
            Item[nNewIndex].LockItem(nBind);
            Item[nNewIndex].SetBang(nBang);
            Item[nNewIndex].SetTime(nYear, nMonth, nDay, nHour, nMin);
            SynPlayerItem(nNewIndex);

            Add(nNewIndex, pos_equiproom, x, y, 0, 0, MAX_STACKNUM);
        }

        nTotalStack -= MAX_STACKNUM;
    }

    // 4. Neu con du stack, tao 1 item nho hon
    if (nTotalStack > 0)
    {
        int x, y;
        int nNewIndex = ItemSet.AddOther(nGenre, nSeries, nLevel, 10,
                                         nDetailType, nParticular, NULL,
                                         g_SubWorldSet.GetGameVersion(), 0, 1);

        if (CheckCanPlaceInEquipment(Item[nNewIndex].GetWidth(), Item[nNewIndex].GetHeight(), &x, &y))
        {
            Item[nNewIndex].LockItem(nBind);
            Item[nNewIndex].SetBang(nBang);
            Item[nNewIndex].SetTime(nYear, nMonth, nDay, nHour, nMin);
            SynPlayerItem(nNewIndex);

            Add(nNewIndex, pos_equiproom, x, y, 0, 0, nTotalStack);
        }
    }

	m_nLastTotalStackedItem = nTotalStack;

    return TRUE;
}

//获取所有东西的数量
int KItemList::GetItemCount()
{//检测蓝装
	int nIdx = 0;
	int nCount=0;
	while((nIdx = m_UseIdxItemList.GetNext(nIdx)))
	{
		++nCount;
	}
	return nCount;
}

void KItemList::PlaceItemToRef(int index, int nItemID)
{
	if (index < 0 || index >= IMMEDIACY_ROOM_WIDTH * IMMEDIACY_ROOM_HEIGHT)
		return;

	if (nItemID <= 0 || nItemID >= MAX_ITEM)
		return;

	m_refItem[index].nameID = g_FileName2Id(Item[nItemID].GetName());
	//Item[nItemID].SetHotKey(index + 1);

	REF_ITEM_SYNC itemSync;
	itemSync.ProtocolType = s2c_refitemsync;
	itemSync.nIndex = index;
	itemSync.ndwID = Item[nItemID].GetID();
	if (g_pServer)
		g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&itemSync, sizeof(REF_ITEM_SYNC));
}

void KItemList::RemoveItemFromRef(int index)
{
	if (index < 0 || index >= IMMEDIACY_ROOM_WIDTH * IMMEDIACY_ROOM_HEIGHT)
		return;

	m_refItem[index].nameID = 0;

	REF_ITEM_SYNC itemSync;
	itemSync.ProtocolType = s2c_refitemsync;
	itemSync.nIndex = index;
	itemSync.ndwID = 0;
	if (g_pServer)
		g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&itemSync, sizeof(REF_ITEM_SYNC));
}

int KItemList::FindHotKey(int nItemID)
{
	if (nItemID <= 0 || nItemID >= MAX_ITEM)
		return 0;
	unsigned long nNameID = g_FileName2Id(Item[nItemID].GetName());
	for (int i = 0; i < IMMEDIACY_ROOM_WIDTH * IMMEDIACY_ROOM_HEIGHT; ++i)
	{
		if (m_refItem[i].nameID == nNameID)
			return i + 1;
	}
	return 0;
}

int KItemList::RemoveAllImmediacy()
{
    int nRemoved = 0;
    int nSlot = 0;
    while ((nSlot = m_UseIdxItemList.GetNext(nSlot)))
    {
        const int place = m_Items[nSlot].nPlace;
        if (place != pos_immediacy && place != pos_hand)
            continue;
        const int itemId = m_Items[nSlot].nIdx;
        if (itemId > 0 && itemId < MAX_ITEM && Remove(itemId))
        {
            ItemSet.Remove(itemId);
            ++nRemoved;
            nSlot = 0; // Restart after KLinkArray node removal.
        }
    }
    return nRemoved;
}

int KItemList::RemoveAllEquiproom()
{
    int nRemoved = 0;
    int nSlot = 0;
    while ((nSlot = m_UseIdxItemList.GetNext(nSlot)))
    {
        if (m_Items[nSlot].nPlace != pos_equiproom)
            continue;
        const int itemId = m_Items[nSlot].nIdx;
        if (itemId > 0 && itemId < MAX_ITEM && Remove(itemId))
        {
            ItemSet.Remove(itemId);
            ++nRemoved;
            nSlot = 0; // Restart after KLinkArray node removal.
        }
    }
    return nRemoved;
}

BOOL KItemList::LockItemBind(int nIdx, BOOL bLock)
{
	if(nIdx<=0)
		return FALSE;
	if(Item[nIdx].GetLockItem()->nState == LOCK_STATE_FOREVER || 
	Item[nIdx].GetLockItem()->nState == LOCK_STATE_CHARACTER)
		return FALSE;
	if(bLock)
	{//khoa 
		if(Item[nIdx].GetLockItem()->nState == LOCK_STATE_LOCK)
			return FALSE;
		Item[nIdx].LockItem(LOCK_STATE_LOCK);
	}
	else
	{//thao
		if(Item[nIdx].GetLockItem()->nState == LOCK_STATE_UNLOCK)
			return FALSE;
		Item[nIdx].LockItem(72); //72h
	}
	SynPlayerItem(nIdx);
	return TRUE;
}



int KItemList::PositionToIndex(int P, int i)
{
	if (P >= pos_hand && P < pos_count)
	{
		switch (P)
		{
		case pos_equip:
			return m_EquipItem[i];
		}
	}
	return 0;
}

BOOL KItemList::RemoveItem(int nIdx, int nNum)
{
	if(nIdx<=0)
		return FALSE;
	if(nNum > Item[nIdx].GetStackNum())
		return FALSE;

	int nListIndex = FindSame(nIdx);
	if (!nListIndex)
	{
		_ASSERT(0);
		return FALSE;
	}

	if (nNum == 0 || nNum == Item[nIdx].GetStackNum())
	{
		Remove(nIdx);
		ItemSet.Remove(nIdx);
			return TRUE;
	}
	else
	{
		if (Item[nIdx].IsStack())
		{
			Item[nIdx].SetStackNum(Item[nIdx].GetStackNum() - nNum);
			this->SynPlayerItem(nIdx);
			return TRUE;
		}
		else
		{
				Remove(nIdx);
				ItemSet.Remove(nIdx);
				return TRUE;
		}
	}
	return TRUE;
}
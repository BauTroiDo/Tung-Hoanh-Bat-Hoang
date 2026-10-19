#include "KEngine.h"
#include "KCore.h"
#include "KTabFile.h"
#include "KPlayer.h"
#include "KNpc.h"
#include "KItemGenerator.h"
#include "KSubWorldSet.h"
#include "KItemSet.h"
#include "KBuySell.h"
#include "../../Engine/src/Text.h" //后来加的
#ifndef _STANDALONE
#include "CoreShell.h"
#include "crtdbg.h"
#endif
#include "CoreUseNameDef.h"
#ifdef _SERVER
//#include "../MultiServer/Heaven/Interface/iServer.h"
#endif
KSubWorldSet   *M_SubWorldSetA;


#ifndef _SERVER
#define	SHOP_BOX_WIDTH		6
#define	SHOP_BOX_HEIGHT		10
#define	SSHOP_BOX_WIDTH		12
#define	SSHOP_BOX_HEIGHT	10
#endif


KBuySell	BuySell;

KBuySell::KBuySell()
{
#ifndef _SERVER
	m_pShopRoom = NULL;
	m_pSShopRoom = NULL;
#endif
	m_Item = NULL;
	m_SellItem = NULL;
	m_Width = 0;
	m_Height = 0;
	m_MaxItem = 0;
}

KBuySell::~KBuySell()
{
#ifndef _SERVER
	if (m_pShopRoom)
	{
		delete m_pShopRoom;
		m_pShopRoom = NULL;
	}
	if (m_pSShopRoom)
	{
		delete m_pSShopRoom;
		m_pSShopRoom = NULL;
	}
	
#endif
	if (m_Item)
	{
		delete [] m_Item;
		m_Item = NULL;
	}
	if (m_SellItem)
	{
		for (int i = 0; i < m_Height; i++)
		{
			if (m_SellItem[i])
			{
				delete m_SellItem[i];
				m_SellItem[i] = NULL;
			}
		}
		delete m_SellItem;
		m_SellItem = NULL;
	}
	m_Width = 0;
	m_Height = 0;
	m_MaxItem = 0;
}
//初始化商品
BOOL KBuySell::Init()
{
	KTabFile		GoodsFile;
	KTabFile		BuySellFile; 

//	g_SetFilePath("\\");
	if (!BuySellFile.Load(BUYSELL_FILE) || !GoodsFile.Load(GOODS_FILE))
		return FALSE;

	int nHeight = GoodsFile.GetHeight() - 1;
	int nWidth = GoodsFile.GetWidth() - 1;
	if (nWidth == 0 || nHeight == 0)
	{
		GoodsFile.Clear();
	    BuySellFile.Clear();
		return FALSE;
	}

	m_Item = (KItem *)new KItem[nHeight];   //分配内存

	if (!m_Item)
	{
		GoodsFile.Clear();
	    BuySellFile.Clear();
		return FALSE;
	}

	ItemGenerate ItemGenerator;
	for (int k = 0; k < nHeight; ++k)
	{
		GoodsFile.GetInteger(k + 2, 1, -1, &ItemGenerator.nGenre); //种类
		GoodsFile.GetInteger(k + 2, 2, -1, &ItemGenerator.nDetailType); //具体类型
		GoodsFile.GetInteger(k + 2, 3, -1, &ItemGenerator.nParticularType); //详细类别
		GoodsFile.GetInteger(k + 2, 4, -1, &ItemGenerator.nSeriesReq); //五行
		GoodsFile.GetInteger(k + 2, 5, 1, &ItemGenerator.nLevel);//等级
		GoodsFile.GetInteger(k + 2, 6, 0, &ItemGenerator.nPrice);//银两
		int nGolden = 0,nPriceType = 0,nEquipType=0,nIsBang=0;
		GoodsFile.GetInteger(k + 2, 7, 0, &ItemGenerator.nPriceXu);//金币
		ItemGenerator.nPriceXu = ItemGenerator.nPrice;
		GoodsFile.GetInteger(k + 2, 8, 0, &nPriceType);            //货币类型
		GoodsFile.GetInteger(k + 2, 9, 0, &ItemGenerator.nExpirePoint);//SetTimeItem new
		GoodsFile.GetInteger(k + 2, 10, 0, &nIsBang);	    	   //是否绑定
		GoodsFile.GetInteger(k + 2, 11, 0, &nEquipType);           //装备类型

		int nItemMagLevel[6];

		if (nEquipType>0)
		{
		   GoodsFile.GetInteger(k + 2, 14, 0, &nItemMagLevel[0]);
		   GoodsFile.GetInteger(k + 2, 15, 0, &nItemMagLevel[1]);
		   GoodsFile.GetInteger(k + 2, 16, 0, &nItemMagLevel[2]);
		   GoodsFile.GetInteger(k + 2, 17, 0, &nItemMagLevel[3]);
		   GoodsFile.GetInteger(k + 2, 18, 0, &nItemMagLevel[4]);
		   GoodsFile.GetInteger(k + 2, 19, 0, &nItemMagLevel[5]);
		}
		else 
		{
			nItemMagLevel[0]=0;
			nItemMagLevel[1]=0;
			nItemMagLevel[2]=0;
			nItemMagLevel[3]=0;
			nItemMagLevel[4]=0;
			nItemMagLevel[5]=0;
		}

		switch(ItemGenerator.nGenre)
		{
		case item_equip:  //服务器商店的装备产生
			ItemGen.Gen_Equipment(ItemGenerator.nDetailType,\
				              ItemGenerator.nParticularType,\
				                   ItemGenerator.nSeriesReq,\
				                ItemGenerator.nLevel,nItemMagLevel,0,\
				             g_SubWorldSet.GetGameVersion(),\
				             &m_Item[k],0,nEquipType,0,0,TRUE,\
							 ItemGenerator.nPrice,ItemGenerator.nPriceXu,nIsBang,nPriceType);
			
			break;
		case item_medicine:
			ItemGen.Gen_MedicineShop(ItemGenerator.nDetailType,\
				ItemGenerator.nLevel, \
				g_SubWorldSet.GetGameVersion(),\
				&m_Item[k],1,ItemGenerator.nPrice,nIsBang,nPriceType);
			break;
		case item_mine:  // 默认是-商店类  
			ItemGen.Gen_MineShop(ItemGenerator.nParticularType, &m_Item[k], 1,\
				                ItemGenerator.nLevel, ItemGenerator.nSeriesReq,\
								ItemGenerator.nDetailType,ItemGenerator.nPriceXu,\
								ItemGenerator.nPrice,nIsBang,nPriceType);
			break;
		case item_townportal:
			ItemGen.Gen_TownPortal(&m_Item[k]);
			break;
		case item_task:				// 默认是任务类-商店类
			ItemGen.Gen_QuestShop(ItemGenerator.nDetailType,&m_Item[k],1,ItemGenerator.nPriceXu,\
				               ItemGenerator.nPrice,ItemGenerator.nLevel,nIsBang,nPriceType);
			break;
		case item_fusion:			// 纹钢
			//ItemGen.Gen_QuestShop(ItemGenerator.nDetailType,&m_Item[k],1,ItemGenerator.nPriceXu,ItemGenerator.nPrice,ItemGenerator.nLevel);
			break;
		default:
			break;
		}


	//	m_Item->SetBang(nIsBang);
		//SetTimeItem new
		if (ItemGenerator.nExpirePoint)
			m_Item[m_MaxItem].SetExpirePoint(ItemGenerator.nExpirePoint);

		m_MaxItem++;
	}

	m_Height = BuySellFile.GetHeight() - 1;
	m_Width  = BuySellFile.GetWidth();
	
	if (m_Width == 0 || m_Height == 0)
	{
		GoodsFile.Clear();
	    BuySellFile.Clear();
		return FALSE;
	}

	m_SellItem = (int **)new int*[m_Height];
	if (!m_SellItem)
	{
		GoodsFile.Clear();
	    BuySellFile.Clear();
		return FALSE;
	}

	for (int i = 0; i < m_Height; ++i)  
	{
		m_SellItem[i] = NULL;
		m_SellItem[i] = (int *)new int[m_Width];
		if (!m_SellItem[i])
		{
			GoodsFile.Clear();
	        BuySellFile.Clear();
			return FALSE;
		}
		for (int j = 0; j < m_Width; ++j)
		{
			BuySellFile.GetInteger(i + 2, j + 1, -1, &m_SellItem[i][j]);//m_SellItem[0][0]  开始[商店ID][列]
			if (m_SellItem[i][j] == -1)
			{
				continue;
			}

			//_ASSERT(m_SellItem[i][j] > 0);		// 策划是从1开始的
			if (m_SellItem[i][j] > 0)
				m_SellItem[i][j] -= 1;			// 为了策划从1开始填表
		}
	}

#ifndef _SERVER
	if (!m_pShopRoom)
	{
		m_pShopRoom = new KInventory;  //容器内存
		m_pShopRoom->Init(SHOP_BOX_WIDTH, SHOP_BOX_HEIGHT); 
	}

	if (!m_pSShopRoom)
	{
		m_pSShopRoom = new KInventory;
		m_pSShopRoom->Init(SSHOP_BOX_WIDTH, SSHOP_BOX_HEIGHT);
	}
#endif

	GoodsFile.Clear();
	BuySellFile.Clear();

#ifdef _SERVER
	printf("---The store is initialized---\n",BUYSELL_FILE);
#endif

	return TRUE;
}

KItem* KBuySell::GetItemidx(int nIndex)
{
	if (nIndex < 0 || nIndex >= m_MaxItem || !m_Item)
		return NULL;

	return &m_Item[nIndex];
}

int KBuySell::GetItemIndex(int nShop, int nIndex)
{
	if (!m_SellItem || nShop < 0 || nShop >= m_Height || nIndex < 0 || nIndex >= m_Width)
		return -1;

	if (!m_SellItem[nShop])
		return -1;

	return m_SellItem[nShop][nIndex];
}

#ifdef _SERVER
BOOL KBuySell::Buy(int nPlayerIdx, int nBuy, int nBuyIdx, int nPlace, int nX, int nY,int nShuishou,int nCityMenber,int nMapID,int nItmeMum)
{
	KASSERT(nPlayerIdx >= 0 && nPlayerIdx < MAX_PLAYER);

	const char* m_PlayerName = Player[nPlayerIdx].GetPlayerName();
	const char* m_AccoutName = Player[nPlayerIdx].m_AccoutName;

	/*if (Player[nPlayerIdx].m_ItemList.GetPlayerItemCount()>=MAX_ITEMLIST_CONUT)
	{
		Player[nPlayerIdx].m_ItemList.msgshow("提示:<color=yellow>物品超限,请清理储物空间<color>!");
		return FALSE;
	}*/

    int nBei=1;
	if (nBuy != Player[nPlayerIdx].m_BuyInfo.m_nBuyIdx)
	{//是这个商店
		//Player[nPlayerIdx].m_ItemList.msgshow("Du lieu cua hang co loi, vui long lien he GM de xu ly!");
		printf("Xay ra loi giao dich 1, tu dong xua loi ! [%s] [%s]\n",m_AccoutName,m_PlayerName);
		return FALSE;
	}

	if (Player[nPlayerIdx].m_BuyInfo.m_nBuyIdx<0)
	{
		//Player[nPlayerIdx].m_ItemList.msgshow("Du lieu cua hang co loi, vui long lien he GM de xu ly!");
		printf("Xay ra loi giao dich 2, tu dong xua loi ! [%s] [%s]\n",m_AccoutName,m_PlayerName);
		return FALSE;
	}

	if (nBuyIdx > m_Width)
	{
		//Player[nPlayerIdx].m_ItemList.msgshow("Goi y A: Du lieu cua hang co loi, vui long lien he GM de xu ly!");
		printf("Xay ra loi giao dich 3, tu dong xua loi ! [%s] [%s]\n",m_AccoutName,m_PlayerName);
		return FALSE;
	}
	if (m_SellItem[nBuy][nBuyIdx] < 0 || m_SellItem[nBuy][nBuyIdx] >= m_MaxItem)
	{
		//Player[nPlayerIdx].m_ItemList.msgshow("Goi y B: Du lieu cua hang co loi, vui long lien he GM de xu ly!");
		printf("Xay ra loi giao dich 4, tu dong xua loi ! [%s] [%s]\n",m_AccoutName,m_PlayerName);
		return FALSE;
	}

	int nSubIndexID,mShuiType=0;

	  /* nSubIndexID = M_SubWorldSetA->SearchWorld(nMapID);

	if (nSubIndexID>=0)
       mShuiType = SubWorld[nSubIndexID].m_ShuiShouType;*/

	int nIdx = m_SellItem[nBuy][nBuyIdx];
    //int nJB  = m_Item[nIdx].GetPriceXu();
	int nSerPrice = m_Item[nIdx].GetPrice();
	int nPriceType = m_Item[nIdx].GetcModel();

	if (nItmeMum>1)
	{
        if (!m_Item[nIdx].IsStack())
		{
			Player[nPlayerIdx].m_ItemList.msgshow("Vat pham nay khong the mua so luong lon!");
			printf("KBuySell vat khong the mua so luong lon  tu dong xua loi ! [%s] [%s]\n",m_AccoutName,m_PlayerName);
			return FALSE;
		}
		nBei=nItmeMum;
	}

    if (mShuiType==0)
	{//税收类型为增加 
	   if (nCityMenber==1)  //为占帮成员
	   { 
       // nJB  -=float(nJB*nShuishou/100);
        nSerPrice -=float(nSerPrice*nShuishou/100);
	   } 
	   else
	   {//非占帮成员
         //nJB  +=float(nJB*nShuishou/100);
         nSerPrice +=float(nSerPrice*nShuishou/100);
	   } 
	}
	else
	{//减少税收
		if (nCityMenber==1)  //为占帮成员
		{ 
			nSerPrice  -=float(nSerPrice*nShuishou/100);
			//nJXB -=float(nJXB*nShuishou/100);
		} 
		else
		{//非占帮成员
			nSerPrice  -=float(nSerPrice*nShuishou/100);
			//nJXB -=float(nJXB*nShuishou/100);
	   }

	}
	
   /* if  (nJB>0)
	{//金币 
	  if (Player[nPlayerIdx].m_ItemList.GetEquipmentXu() < nJB*nBei)
	  {
         Player[nPlayerIdx].m_ItemList.msgshow("提示:S金币不足!");
		 return FALSE;
	  }
	}  
	else	*/

	   if (nSerPrice<=0)
	   {
		   Player[nPlayerIdx].m_ItemList.msgshow("Cua hang co loi, vui long lien he GM de xu ly!");
		   printf("Du lieu cua hang co loi, [%s] [%s]\n",m_AccoutName,m_PlayerName);
	       return FALSE;
	   }

	   int  nNewPrice = nSerPrice*nBei;

	   if (nPriceType==moneyunit_money)
	   {
		   if (Player[nPlayerIdx].m_ItemList.GetEquipmentMoney() < nNewPrice)
		   {
			   Player[nPlayerIdx].m_ItemList.msgshow("Ngan luong khong du!");
			   return FALSE;
		   } 
	   }
	   else if (nPriceType==moneyunit_moneya)
	   {
		   if (Player[nPlayerIdx].m_ItemList.GetEquipmentMoney() < nNewPrice)
		   {
			   Player[nPlayerIdx].m_ItemList.msgshow("Ngan luong khong du!");
			   return FALSE;
		   } 
	   }
	   else if (nPriceType==moneyunit_xu)
	   {
		   if (Player[nPlayerIdx].m_ItemList.GetEquipmentXu() < nNewPrice)
		   {
			   Player[nPlayerIdx].m_ItemList.msgshow("Xu khong du!");
			   return FALSE;
		   } 
	   }
	   else if (nPriceType==moneyunit_fuyuan)
	   {
		   if (Player[nPlayerIdx].m_cFuYuan.GetFuYuanValue() < nNewPrice)
		   {
			   Player[nPlayerIdx].m_ItemList.msgshow("Phuc duyen khong du!");
			   return FALSE;
		   }
	   }
	   else if (nPriceType==moneyunit_repute)
	   {
		   if (Player[nPlayerIdx].m_cRepute.GetReputeValue() < nNewPrice)
		   {
			   Player[nPlayerIdx].m_ItemList.msgshow("Danh vong khong du!");
			   return FALSE;
		   } 
	   }
	   else if (nPriceType==moneyunit_expoint)
	   {
		   //if (Player[nPlayerIdx].GetExtPoint() < nNewPrice)
		   if (Player[nPlayerIdx].m_cTask.GetSaveVal(2051) < nNewPrice)
		   {
			   Player[nPlayerIdx].m_ItemList.msgshow("Diem vinh du khong du!");
			   return FALSE;
		   } 
	   }
	   else if (nPriceType==moneyunit_jifen)
	   {
		   if (Player[nPlayerIdx].m_cTask.GetSaveVal(192) < nNewPrice)
		   {
			   Player[nPlayerIdx].m_ItemList.msgshow("Diem tong kim khong du!");
			   return FALSE;
		   }
	   }
	   else
	   { //其他类型的 
			Player[nPlayerIdx].m_ItemList.msgshow("Cua hang co loi, vui long lien he GM de xu ly!");
			printf("Xay ra loi giao dich 5, tu dong xua loi ! [%s] [%s]\n",m_AccoutName,m_PlayerName);
			return FALSE;
	   } 

	   if (nNewPrice<=0)
	   { 
			Player[nPlayerIdx].m_ItemList.msgshow("Cua hang co loi, vui long lien he GM de xu ly!");
			printf("Xay ra loi giao dich 6, tu dong xua loi ! [%s] [%s]\n",m_AccoutName,m_PlayerName);
			return FALSE;
	   } 

	//SetTimeItem new
	if (m_Item[nIdx].GetExpirePoint())
	{
	   time_t rawtime;                  //定义一个long 型存放秒数
	   struct tm * timeinfo;	
	   time (&rawtime);                 //距离现在的时间（秒）
	   timeinfo = localtime(&rawtime);  //把从1970-1-1零点零分到当前时间系统所偏移的秒数时间转换为本地时间
		int bYear   = timeinfo->tm_year+1900;
		int bMonth  = timeinfo->tm_mon+1;	//当月
		int bDay    = timeinfo->tm_mday+m_Item[nIdx].GetExpirePoint();  
		int bHour   = timeinfo->tm_hour;
		int bMin   = timeinfo->tm_min;
		m_Item[nIdx].SetTime(bYear,bMonth,bDay,bHour,bMin);
	}

	int nItemIdx = ItemSet.Add(&m_Item[nIdx]);  //产生ITEM 指针数据

	if (!nItemIdx)
	{
        Player[nPlayerIdx].m_ItemList.msgshow("Du lieu cua hang co loi, vui long lien he GM de xu ly!");
		printf("Xay ra loi giao dich 7, tu dong xua loi ! [%s] [%s]\n",m_AccoutName,m_PlayerName);
		return FALSE;
	}
	int	x, y;
	if (!Player[nPlayerIdx].m_ItemList.CheckCanPlaceInEquipment(Item[nItemIdx].GetWidth(), Item[nItemIdx].GetHeight(), &x, &y))
	{//同步客户端（包里）
		//Player[nPlayerIdx].m_ItemList.Add(nItemIdx,pos_equiproom,x,y,0,0,nBei);  //包袱增加装备 同步装备--客户端
		//Player[nPlayerIdx].m_ItemList.Remove(nItemIdx);
		ItemSet.Remove(nItemIdx);
		Player[nPlayerIdx].m_ItemList.msgshow("Khoang trong khong du cho!");
		return FALSE;
	}

	char	sMsg[90]={0};
	/*if (m_Item[nIdx].GetPriceXu()>0)  //任务物品付金币  付金币类型 m_Item[nIdx].GetGenre() == item_task
	{//金币类(int nIdx, int nPlace, int nX, int nY, BOOL bInit = FALSE,int nIsAutoDie=0,int StackNum=0);
	   Player[nPlayerIdx].PayXu(nJB*nBei);
	   Player[nPlayerIdx].m_ItemList.Add(nItemIdx, nPlace, nX, nY,0,0,nBei);
	   
	   if (nCityMenber==1)
	     _snprintf(sMsg,sizeof(sMsg)-1,"提示:占帮成员优惠<color=green>(%d%s<color>,花费了<color=green>(%d)<color>金币!",nShuishou,"%%",nJB*nBei);
	   else
         _snprintf(sMsg,sizeof(sMsg)-1,"提示:另加城市<color=green>税收(%d%s)<color>,花费了<color=green>(%d)<color>金币!",nShuishou,"%%",nJB*nBei);

       Player[nPlayerIdx].m_ItemList.msgshow(sMsg);
	}
	else*/
	//开始付款
		char nInfo[32]={0};
		if (nPriceType==moneyunit_money || nPriceType==moneyunit_moneya )
		{
			Player[nPlayerIdx].Pay(nNewPrice);  // 其他付剑侠B
			sprintf(nInfo,"%d Ngan luong",nNewPrice);
		}
		else if (nPriceType==moneyunit_xu)
		{
			Player[nPlayerIdx].PayXu(nNewPrice);
			sprintf(nInfo,"%d Xu",nNewPrice);
			Player[nPlayerIdx].ExecuteScript2("\\script\\System\\player\\logbuysell.lua","ShopXu", nItemIdx, nItmeMum);  
		}
		else if (nPriceType==moneyunit_fuyuan)
		{
			Player[nPlayerIdx].m_cFuYuan.AddFuYuanValue(-nNewPrice);
			sprintf(nInfo,"%d Phuc duyen",nNewPrice);
		}
		else if (nPriceType==moneyunit_repute)
		{
		    Player[nPlayerIdx].m_cRepute.AddReputeValue(-nNewPrice);
			sprintf(nInfo,"%d Danh vong",nNewPrice);
		}
		else if (nPriceType==moneyunit_expoint)
		{
			//Player[nPlayerIdx].AddAndPayExtPoint(-nNewPrice);
			sprintf(nInfo,"%d vinh du",nNewPrice);
			int nVal = Player[nPlayerIdx].m_cTask.GetSaveVal(2051);
			Player[nPlayerIdx].m_cTask.SetSaveVal(2051,nVal-nNewPrice);
		}
		else if (nPriceType==moneyunit_jifen)
		{//  TASK ID 192
			sprintf(nInfo,"%d Tong Kim",nNewPrice);
			int nVal = Player[nPlayerIdx].m_cTask.GetSaveVal(192);
			Player[nPlayerIdx].m_cTask.SetSaveVal(192,nVal-nNewPrice);
			//Player[nPlayerIdx].ExecuteScript2("\\script\\admin\\logbuysell.lua","ShopTongKim", nItemIdx, nItmeMum); 
			///return FALSE;
		}
		else
		{//其他类型　删除物品
			//Remove(m_Items[i].nIdx);
			ItemSet.Remove(nItemIdx);
			return FALSE;
		}
		  //客户端增加物品 pos_equiproom,x,y
		//xep chong item mua
		//printf("xep chong item mua shop: so luong %d\n",nBei);

		Player[nPlayerIdx].m_ItemList.Add(nItemIdx,pos_equiproom,x,y,0,0,nBei);

		//Player[nPlayerIdx].ExecuteScript2("\\script\\log_game\\log_kytrancac.lua","mainvd", nItemIdx, nBei);

	     if (nCityMenber==1)
		 { 
	       _snprintf(sMsg,sizeof(sMsg)-1,"Uu dai thanh vien bang <c=green>(%d%s)<c>, da chi ra <c=green>(%s)<c>!",nShuishou,"%",nInfo);
		 } 
	     else
	       _snprintf(sMsg,sizeof(sMsg)-1,"Thue Bang Hoi <c=green>thue (%d%s)<c>, da chi ra <c=green>(%s)<c>!",nShuishou,"%",nInfo);

	     Player[nPlayerIdx].m_ItemList.msgshow(sMsg);

  if (mShuiType==0)
  {//只有增加税收的 才增加
/*    SubWorld[nSubIndexID].m_WarCityManey +=float(nJXB*nShuishou/100);  //城市税收收入
    SubWorld[nSubIndexID].m_TodayMoney   +=float(nJXB*nShuishou/100);      //今天的收入

	char nFiledPath[128];
	char nTitleName[128];
    int  nZIjing=0;

    sprintf(nFiledPath,"%s","\\DataFiles\\Tong\\城市资金贡献\\襄阳\\jxb.txt");  //写入资金
	KTabFile nTXT;
	nTXT.Load(nFiledPath);
	nTXT.GetInteger(1,1,0,&nZIjing);
	nTXT.Clear();
	nZIjing +=100;
    sprintf(nTitleName,"%d",nZIjing);

	KFile mFile; 
	//mFile.Open(nFiledPath);
	mFile.Create(nFiledPath);
    mFile.Write(nTitleName, strlen(nTitleName));
	mFile.Close();*/
  }
    return TRUE;
}


/*******************************************************************************
参数 nIdx 指游戏里Item数组的编号
*******************************************************************************/
BOOL KBuySell::Sell(int nPlayerIdx, int nBuy, int nIdx,int nClinetKind)
{
	KASSERT(nPlayerIdx >= 0 && nPlayerIdx < MAX_PLAYER);
	KASSERT(nIdx > 0 && nIdx < MAX_ITEM);

	if (nIdx <= 0)
		return FALSE;

	if  (nClinetKind<=0)
	{//PC端才检测
	   if (nBuy != Player[nPlayerIdx].m_BuyInfo.m_nBuyIdx)
		  return FALSE;

	   if (Player[nPlayerIdx].m_BuyInfo.m_nBuyIdx<0)
	   {
		  Player[nPlayerIdx].m_ItemList.msgshow("Du lieu cua hang co loi, vui long lien he GM de xu ly!");
		  return FALSE;
	   }
	}

	if (Item[nIdx].GetIsBang())
	{
		Player[nPlayerIdx].m_ItemList.msgshow("Vat pham dinh bao hiem khong the ban!");
		return FALSE;
	} 

	int nMoney = Item[nIdx].GetPrice();
	    nMoney /= BUY_SELL_SCALE;

	if (nMoney <= 0)
		nMoney = 1;
	if (Item[nIdx].GetGenre() == item_task)
	{	
		if (Item[nIdx].GetIsSell() == 1)
		{	
			Player[nPlayerIdx].Earn(nMoney);
			Player[nPlayerIdx].m_ItemList.Remove(nIdx);
			ItemSet.Remove(nIdx);
			return TRUE;
		}
	
	}
	else
	{
		Player[nPlayerIdx].Earn(nMoney);
		Player[nPlayerIdx].m_ItemList.Remove(nIdx);
		ItemSet.Remove(nIdx);
		return TRUE;
	}
	return TRUE;
}
#endif
//商店描绘可叠数量
#ifndef _SERVER
void KBuySell::PaintItem(int nIdx, int nX, int nY)
{
	int nShop = Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nBuyIdx;
	if (nShop < 0 || nShop >= m_Height)
		return;
	int nItemIdx = GetItemIndex(nShop, nIdx);

	int x = nX;
	int y = nY;

	KItem* pItem = GetItemidx(nItemIdx);

	if (pItem)
	{
		pItem->Paint(x, y);  //描绘可叠放数量
	}
}

//奇珍阁
void KBuySell::OpenCSuPerShop(int nSaleType, BuySellInfo *pInfo)
{
	if(nSaleType < 0 || nSaleType > 1)	 //0 或 1
		return;
	
	Player[CLIENT_PLAYER_INDEX].m_BuyInfo.Clear();
//	Player[CLIENT_PLAYER_INDEX].m_NewBuyInfo.Clear();

	Player[CLIENT_PLAYER_INDEX].m_BuyInfo = *pInfo;
	Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nBuyIdx = 0;
	CoreDataChanged(GDCNI_SUPERSHOP, (unsigned int)pInfo, nSaleType);
}

void KBuySell::OpenCSale(int nShop)  //系统NPC商店
{
	if (nShop < 0 || nShop >= m_Height)
		return;
	Player[CLIENT_PLAYER_INDEX].m_BuyInfo.Clear();

	Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nBuyIdx = nShop;
	CoreDataChanged(GDCNI_NPC_TRADE, nShop,TRUE);   //通知客户端界面打开买卖面板
}
//客户端打开商城
void KBuySell::OpenCMarket(int nShop) //奇珍阁
{
	if (nShop < 0 || nShop >= m_Height)
		return;

	Player[CLIENT_PLAYER_INDEX].m_BuyInfo.Clear();

	Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nBuyIdx =0;

	CoreDataChanged(GDCNI_OPEN_MARKET, nShop, TRUE);  //通知客户端界面打开奇珍阁
}

void  KBuySell::SetShopItemdata(BYTE* pMsg )
{//更新寄售阁
	ITEMDATA_SYNC_END *nSynInfo = (ITEMDATA_SYNC_END *)pMsg;


}

//设置客户端商城信息
void  KBuySell::SetShopItemInfo(BYTE* pMsg)
{
	  ITEM_SYNC_SHOP *nSynInfo = (ITEM_SYNC_SHOP *)pMsg;

	  /*char nSmg[64]={0};
	  sprintf(nSmg,"收到信息:%d,%d",nSynInfo->nShopIdx,nSynInfo->nBuyIdx);
	  Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(nSmg);
	  */
	  if (nSynInfo->nShopIdx != Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nBuyIdx)
	  {//是这个商店
		  return;
	  }

	  if (Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nBuyIdx<0)
	  {
		  return;
	  }
	  
	  if (nSynInfo->nBuyIdx > m_Width)
	  {
		  return;
	  }

	  if (m_SellItem[nSynInfo->nShopIdx][nSynInfo->nBuyIdx] < 0 || m_SellItem[nSynInfo->nShopIdx][nSynInfo->nBuyIdx] >= m_MaxItem)
	  {
		  return;
	  }

	  int nIdx = m_SellItem[nSynInfo->nShopIdx][nSynInfo->nBuyIdx];

	  if (nSynInfo->m_ItmeInfo[0])
		 m_Item[nIdx].SetItmeInfo(nSynInfo->m_ItmeInfo);
}

#endif
//服务器端 主要用于脚本打开
#ifdef _SERVER
void KBuySell::OpenSSale(int nPlayerIdx, int nShop)	//npc商店
{
	if (nPlayerIdx <= 0 || nPlayerIdx > MAX_PLAYER)
	{
		return;
	}
	Player[nPlayerIdx].m_BuyInfo.Clear();

	Player[nPlayerIdx].m_BuyInfo.m_nBuyIdx = nShop;
	Player[nPlayerIdx].m_BuyInfo.m_SubWorldID = Npc[Player[nPlayerIdx].m_nIndex].m_SubWorldIndex;
	int nPmap;
	Npc[Player[nPlayerIdx].m_nIndex].GetMpsPos(&Player[nPlayerIdx].m_BuyInfo.m_nMpsX,&Player[nPlayerIdx].m_BuyInfo.m_nMpsY,&nPmap);

	SALE_BOX_SYNC saleSync;
	saleSync.ProtocolType = s2c_opensalebox;
	saleSync.nShopIndex   = nShop;
	g_pServer->PackDataToClient(Player[nPlayerIdx].m_nNetConnectIdx, &saleSync, sizeof(SALE_BOX_SYNC));
}

void KBuySell::OpenSMarket(int nPlayerIdx, int nShop)
{
	if (nPlayerIdx <= 0 || nPlayerIdx > MAX_PLAYER)
	{
		return;
	}
	Player[nPlayerIdx].m_BuyInfo.Clear();

	Player[nPlayerIdx].m_BuyInfo.m_nBuyIdx    = nShop;
	Player[nPlayerIdx].m_BuyInfo.m_SubWorldID = Npc[Player[nPlayerIdx].m_nIndex].m_SubWorldIndex;
	
	int nPmap;
	    Npc[Player[nPlayerIdx].m_nIndex].GetMpsPos(&Player[nPlayerIdx].m_BuyInfo.m_nMpsX,&Player[nPlayerIdx].m_BuyInfo.m_nMpsY,&nPmap);

	SALE_MARKET_SYNC marketSync;
	marketSync.ProtocolType = s2c_openmarketbox;  //通知客户端打开奇珍阁
	marketSync.nShopID      = 0;
	g_pServer->PackDataToClient(Player[nPlayerIdx].m_nNetConnectIdx, &marketSync, sizeof(SALE_MARKET_SYNC));
}


void KBuySell::OpenSSuPerShop(int nPlayerIdx, int nSaleType, int nMoneyUnit, int nShopNum, int *nShopId)
{
	if (nPlayerIdx <= 0 || nPlayerIdx > MAX_PLAYER)
		return;
	if(nSaleType < 0 || nSaleType > 1)
		return;
	if(nMoneyUnit < moneyunit_money || nMoneyUnit >= moneyunit_num)
		return;
	if(nShopNum <= 0 || nShopNum >= MAX_SUPERSHOP_SHOPTAB)
		return;
	int nMap;
	Player[nPlayerIdx].m_BuyInfo.Clear();
	//memcpy(&Player[nPlayerIdx].m_BuyInfo.m_nShopIdx, nShopId, sizeof(int) * MAX_SUPERSHOP_SHOPTAB);
	//Player[nPlayerIdx].m_BuyInfo.m_nShopNum = nShopNum;
	Player[nPlayerIdx].m_BuyInfo.m_nPriceType = nMoneyUnit;
	Player[nPlayerIdx].m_BuyInfo.m_SubWorldID = Npc[Player[nPlayerIdx].m_nIndex].m_SubWorldIndex;
	Npc[Player[nPlayerIdx].m_nIndex].GetMpsPos(&Player[nPlayerIdx].m_BuyInfo.m_nMpsX,&Player[nPlayerIdx].m_BuyInfo.m_nMpsY,&nMap);
	
	S2C_SUPERSHOP	sSale;
	sSale.ProtocolType = s2c_syncsupershop;
	sSale.m_nSaleType = nSaleType;
	memcpy(&sSale.m_BuySellInfo, &Player[nPlayerIdx].m_BuyInfo, sizeof(BuySellInfo));
	g_pServer->PackDataToClient(Player[nPlayerIdx].m_nNetConnectIdx, (BYTE*)&sSale, sizeof(S2C_SUPERSHOP));
}

//----------------同步商城信息

void  KBuySell::SynShopItemInfo(int nPlayerIdx, int nShopIdx,int nBuyIdx)
{
	//KASSERT(nPlayerIdx >= 0 && nPlayerIdx < MAX_PLAYER);
	if  (NULL==m_SellItem || nPlayerIdx<0 ||nPlayerIdx>=MAX_PLAYER)
	{
		printf("--商城:同步数据失败--\n");
		return;
	}

	//printf("--A商店同步数据成功,发往客户端:%s --\n",Player[nPlayerIdx].m_PlayerName);
	/*if (Player[nPlayerIdx].m_ItemList.GetPlayerItemCount()>=MAX_ITEMLIST_CONUT)
	{
		Player[nPlayerIdx].m_ItemList.msgshow("提示:<color=yellow>物品超限,请清理储物空间<color>!");
		return;
	}*/
	
   // int nBei=1;

	/*if (nShopIdx != Player[nPlayerIdx].m_BuyInfo.m_nBuyIdx)
	{//是这个商店
		Player[nPlayerIdx].m_ItemList.msgshow("Du lieu cua hang co loi, vui long lien he GM de xu ly!");
		return;
	}
	
	if (Player[nPlayerIdx].m_BuyInfo.m_nBuyIdx<0)
	{
		Player[nPlayerIdx].m_ItemList.msgshow("Du lieu cua hang co loi, vui long lien he GM de xu ly!");
		return;
	}*/
	
	if (nBuyIdx<0 || nShopIdx<0 || nBuyIdx > m_Width || nShopIdx>m_Height)
	{
		Player[nPlayerIdx].m_ItemList.msgshow("Goi y A: Du lieu cua hang co loi, vui long lien he GM de xu ly!");
		printf("--商城A:商店数据有误,请联系GM处理!--\n");
		return;
	}
	if (m_SellItem[nShopIdx][nBuyIdx] < 0 || m_SellItem[nShopIdx][nBuyIdx] >= m_MaxItem)
	{
		Player[nPlayerIdx].m_ItemList.msgshow("Goi y B: Du lieu cua hang co loi, vui long lien he GM de xu ly!");
		printf("--商城B:商店数据有误,请联系GM处理!--\n");
		return;
	}

	int nIdx = m_SellItem[nShopIdx][nBuyIdx];
    //int nJB  = m_Item[nIdx].GetPriceXu();
	//int nJXB = m_Item[nIdx].GetPrice();
	//int nPriceType = m_Item[nIdx].GetcModel();
	ITEM_SYNC_SHOP sItem;
	memset(&sItem, 0, sizeof(sItem)); 
	sItem.ProtocolType = s2c_syncitem_shop;
	sItem.nShopIdx	   = nShopIdx;
	sItem.nBuyIdx      = nBuyIdx;
	if (m_Item[nIdx].GetSItmeInfo() && strstr(m_Item[nIdx].GetSItmeInfo(),".lua"))
	{//如果是脚本 就同步信息到 客户端去
		KLuaScript * pScript = (KLuaScript*)g_GetScript(m_Item[nIdx].GetSItmeInfo());//g_FileName2Id(
		if (pScript)
		{
			int nSafeIndex = 0;
			   nSafeIndex=pScript->SafeCallBegin();
			//执行这个脚本的函数
			try
			{
				pScript->CallFunction("GetItemDescription",1,"sdddddds", m_Item[nIdx].GetName(),m_Item[nIdx].GetItemBaseMagic(0),m_Item[nIdx].GetItemBaseMagic(1),m_Item[nIdx].GetItemBaseMagic(2),m_Item[nIdx].GetItemBaseMagic(3),m_Item[nIdx].GetItemBaseMagic(4),m_Item[nIdx].GetItemBaseMagic(5),"");
				const char * szType = lua_typename(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState));
				if (Lua_IsNumber(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState)) == 1)
				{ //Lua_PushNumber
					int nResult = (int)Lua_ValueToNumber(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState));
					_snprintf(sItem.m_ItmeInfo,sizeof(sItem.m_ItmeInfo)-1, "%d", nResult);  // int 类型
				}  
				else if (Lua_IsString(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState)) == 1)  // 字符串类型
				{ 
					snprintf(sItem.m_ItmeInfo, sizeof(sItem.m_ItmeInfo), "%s", (char *)Lua_ValueToString(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState)));
				}  
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
		{
			_snprintf(sItem.m_ItmeInfo,sizeof(sItem.m_ItmeInfo)-1,"暂无数据");
		}

		//Player[nPlayerIdx].m_ItemList.msgshow("提示B:商店同步数据成功,发往客户端!");
		//printf("--B商店同步数据成功,发往客户端:%s --\n",Player[nPlayerIdx].m_PlayerName);
		if (g_pServer)
		  g_pServer->PackDataToClient(Player[nPlayerIdx].m_nNetConnectIdx, (BYTE*)&sItem, sizeof(ITEM_SYNC_SHOP));
	}
}


#endif

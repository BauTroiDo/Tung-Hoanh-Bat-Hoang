#ifndef	KBuySellH
#define	KBuySellH

#define	BUY_SELL_SCALE		4

class KItem;
//商店商品产生结构
typedef struct
{
	int		nGenre;
	int		nDetailType;
	int		nParticularType;
	int		nSeriesReq;
	int		nLevel;
	int		nPrice;
	int		nPriceXu;
	int		nExpirePoint;
} ItemGenerate; 

class KInventory;

class KBuySell
{
public:
	KBuySell();
	~KBuySell();
	BOOL			Init();
	int				GetWidth() { return m_Width; }
	int				GetHeight() { return m_Height; }
	KItem*			GetItemidx(int nIndex);
	int				GetItemIndex(int nShop, int nIndex);
private:
	int**			m_SellItem;
	KItem*			m_Item;
	int				m_Width;
	int				m_Height;
	int				m_MaxItem;
public:
#ifdef _SERVER
	void			OpenSSale(int nPlayerIdx, int nShop);
	void 			OpenSMarket(int nPlayerIdx, int nShop);
	BOOL			Buy(int nPlayerIdx, int nBuy, int nBuyIdx, int nPlace, int nX, int nY,int nShuishou,int nCityMenber,int nMapID,int nItmeMum=1);	// 买第几个买卖列表中的第几项道具
	BOOL			Sell(int nPlayerIdx, int nBuy, int nIdx,int nClinetKind=0);
	void            OpenSSuPerShop(int nPlayerIdx, int nSaleType, int nMoneyUnit, int nShopNum, int *nShopId);
	void            SynShopItemInfo(int nPlayerIdx, int nShopIdx,int nBuyIdx);

#endif
	//客户端打开商城
#ifndef _SERVER
	KInventory*		m_pShopRoom;
	KInventory*		m_pSShopRoom;
	void			OpenCSale(int nShop);
	void 			OpenCMarket(int nShop);
	void			PaintItem(int nIdx, int nX, int nY);
	void            OpenCSuPerShop(int nSaleType, BuySellInfo *pInfo);
	void            SetShopItemInfo(BYTE* pMsg );
	void            SetShopItemdata(BYTE* pMsg );
#endif
};

extern KBuySell	BuySell;
#endif

#ifndef KInventoryH
#define	KInventoryH

class CORE_API KInventory
{
	friend	class	KItemList;

private:
	int*	m_pArray;
	int		m_nMoney;
	int		m_nWidth;  //容器宽度
	int		m_nHeight; //容器高度
	int		m_nBakWidth;  //备份的容器宽度
	int		m_nBakHeight; //备份的容器高度
	int		m_nXu;

public:
	KInventory();
    ~KInventory();

	BOOL	Init(int nWidth, int nHeight);
	void	Release();
	void	Clear();
	//直接设置位置 不考虑条件
	void    setItemInventoryPos(int nIdx, int nX, int nY, int nWidth, int nHeight);
	BOOL	PickUpItem(int nIdx, int nX, int nY, int nWidth, int nHeight);//检测idx是否与位置上的标示相同,否则设置为位置标示为0 返回真
	BOOL    SetRoomNull();
	BOOL    SetEmptyPlaceItemIdx(int nIdx, int nWidth, int nHeight,POINT *pPos);
	int		FindItem(int nX, int nY);
	BOOL	PlaceItem(int nXpos, int nYpos, int nIdx, int nWidth, int nHeight);//检测物品放进容器时，位置上是否有物品,否则设置该位置的标示为idx
	void    DirectSetPosition(int nIdx, int nX, int nY,int nWidth,int nHeight);
	
	BOOL	HoldItem(int nIdx, int nWidth, int nHeight);  //剩余容器空间是否能放下该物品
	BOOL	FindRoom(int nWidth, int nHeight, POINT* pPos);
	BOOL	FindFreeCell(int nX, int nY);
	int     GetFreeRoomCount(int nWidth, int nHeight);
	BOOL	CheckRoom(int nXpos, int nYpos, int nWidth, int nHeight);
	BOOL	AddMoney(int nMoney);
	int		GetmServer() { return m_nMoney; }
	int     GetWidth() { return m_nWidth; }
	int     GetHeight() { return m_nHeight; }
	BOOL	SetClientMon(int nMon) { if (nMon < 0) return FALSE; m_nMoney = nMon; return TRUE; }
	//Xu
	BOOL	AddxCliet(int ni);
	int		GetxCLient() { return m_nXu; }
	BOOL	SetxClient(int ni) { if (ni < 0) return FALSE; m_nXu = ni; return TRUE; }
	////////////////////
	int		GetNextItem(int nStartIdx, int nXpos, int nYpos, int *pX, int *pY);
	int		CalcSameDetailType(int nGenre, int nDetail,int Part);
	int		GetCurIndexItem(int nCurCount);
	BOOL	FindEmptyPlace(int nWidth, int nHeight, POINT *pPos);

	// 输入物品类型和具体类型，察看Inventory里面有没有相同的物品，输出位置和编号
	BOOL	FindSameDetailType(int nGenre, int nDetail,int nPart, int *pnIdx, int *pnX, int *pnY);
	// 输入物品类型和具体类型，察看Inventory里面有没有相同的物品
	BOOL	CheckSameDetailType(int nGenre, int nDetail,int ParticularType);
	
	BOOL	FindSameToRemove(int nItemGenre, int nDetailType, int nParticular, int nLevel, int nSeries, int *pnIdx);
	//xep chong item
	BOOL	FindSameItemToSort(int nGenre, int nDetail, int nParticular,int nLevel, int nSeries, int nBind, int nbYear, int nbMonth, int nbDay, int nbHour, int nbMin, int *pnIdx, int *pnX, int *pnY);
	BOOL	FindSameItemToStack(int nIdx, int *pnIdx, int *pnX, int *pnY);
	
};
#endif //KInventoryH

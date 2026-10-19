//---------------------------------------------------------------------------
// Sword3 Engine (c) 2003 by Kingsoft
//
// File:	KViewItem.h
// Date:	2003.07.28
// Code:	边城浪子
// Desc:	KViewItem Class
//---------------------------------------------------------------------------


#ifndef _SERVER

class KSellItem
{
private:
	DWORD		m_dwNpcID;				// 对方玩家的 npc 的 id
	char		m_szName[32];			// 对方玩家的名字
	int			m_nProcess;
	int         m_nLProcess;
public:
	PlayerItem	m_sItem[60];	        // 对方玩家穿在身上的装备在客户端 Item 数组中的位置信息
	PlayerItem	m_sLItem[60];	        // 对方玩家穿在身上的装备在客户端 Item 数组中的位置信息
	int			m_nIdx[60];
	int			m_nLIdx[60];
	int			m_nId;
	int         m_nLId;
	int			m_nDorp[60];
	int			m_nLDorp[60];
	int         nMum;

public:
	KSellItem();						// 构造
	void		Init();					// 初始化
	void		ApplyViewItem(DWORD dwNpcID);
	void		UpdateItem(DWORD dwNpcID);
	void		UpdateLianItem(DWORD dwNpcID,char * nName);
	int         GetLCount();
	void		Check(int nCount, DWORD dwId);
	int			GetCount();
	void        CheckLian( int nCount, DWORD dwId,char * nName);
	void		DeleteAll(int nUpdate = 0);
	void	    DeleteLAll(int nUpdate= 0);
	void		GetData(BYTE* pMsg ,int nUpdate = 0);
	int 	    GetDataL(BYTE* pMsg ,int nUpdate=0);
	int			FindIdx(int uId);
};

extern	KSellItem	g_cSellItem;

#endif



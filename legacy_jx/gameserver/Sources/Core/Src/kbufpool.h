/* -------------------------------------------------------------------------
//	文件名		：	kbufpool.h
//	创建者		：	luobaohang
//	创建时间	：	2007-3-7 17:11:44
//	功能描述	：	
//
// -----------------------------------------------------------------------*/
#ifndef __KBUFPOOL_H__
#define __KBUFPOOL_H__

#include "kobjectpool.h"


//数据块段
enum KE_BUFFER_TYPE
{
	BUF_0 = 0,
	BUF_8 = 8,
	BUF_16 = 16,
	BUF_32 = 32,
	BUF_64 = 64,
	BUF_128 = 128,
	BUF_256 = 256,
	BUF_512 = 512,
	BUF_1024 = 1024,
};

//Buffer表中的数据描述
struct DATA_DESC
{
	//保证结构为8字节，用WORD会对齐使结构变大
	KE_BUFFER_TYPE m_BufType : 16;	//Buffer预留大小（即选择的缓冲区对象类型，只能取KE_BUFFER_TYPE的值）
	unsigned m_DataSize : 16;	//实际数据大小
	INT m_nBufIndex;			//缓冲区对象ID
};
// -------------------------------------------------------------------------
class KBufPool
{
public:
	virtual BOOL InitBufPool();
	//申请一个Buf
	CHAR *NewBufData(KE_BUFFER_TYPE eBufType, LPINT pBufID)
	{
		return (CHAR *)_NewBufData(eBufType, pBufID);
	}
	//获取一个Buf
	CHAR *GetBufData(DATA_DESC desc)
	{
		return (CHAR *)_GetBufData(desc);
	}
	//释放一个Buf
	VOID FreeBufData(DATA_DESC desc);
protected:
	//数据块对象
	KIncObjPool2<CHAR[BUF_8]> m_mpBufPool8;
	KIncObjPool2<CHAR[BUF_16]> m_mpBufPool16;
	KIncObjPool2<CHAR[BUF_32]> m_mpBufPool32;
	KIncObjPool2<CHAR[BUF_64]> m_mpBufPool64;
	KIncObjPool2<CHAR[BUF_128]> m_mpBufPool128;
	KIncObjPool2<CHAR[BUF_256]> m_mpBufPool256;
	KIncObjPool2<CHAR[BUF_512]> m_mpBufPool512;
	KIncObjPool2<CHAR[BUF_1024]> m_mpBufPool1024;
private:
	//申请一个Buf
	LPVOID _NewBufData(KE_BUFFER_TYPE eBufType, LPINT pBufID);
	//获取一个Buf
	LPVOID _GetBufData(DATA_DESC desc);
};


//由nSize获取合适的预留空间大小
KE_BUFFER_TYPE GetPropReserveBufSize(size_t nSize);
// -------------------------------------------------------------------------

#endif /* __KBUFPOOL_H__ */

/* -------------------------------------------------------------------------
//	文件名		：	kgc_datapoolmanager.cpp
//	创建者		：	luobaohang
//	创建时间	：	2006-12-27 19:36:48
//	功能描述	：	数据池管理器
//
// -----------------------------------------------------------------------*/

//#include "stdafx.h"
#include "KCore.h"
#include "utilities.h"
#include "kgc_datapoolmanager.h"


// -------------------------------------------------------------------------


KGC_DataPoolManager g_DataPoolManager;

// -------------------------------------------------------------------------

BOOL KGC_DataPoolManager::InitObjectPool() 
{
#ifdef KG_GC_SERVER
	{
		//根据情况调整预留空间，可节省内存及提高效率
		m_mpIntIntPool.Init(10240 * 5,10240);
		m_mpIntDataPool.Init(10240,1024);
		m_mpStrIntPool.Init(5120, 512);
		m_mpStrDataPool.Init(5120, 512);
		InitBufPool() ;
	}

#else
	{
		//根据情况调整预留空间，可节省内存及提高效率
		m_mpIntIntPool.Init(10240 * 5, 10240);
		m_mpIntDataPool.Init(10240,1024);
		m_mpStrIntPool.Init(5120, 512);
		m_mpStrDataPool.Init(5120, 512);
		InitBufPool();
	}
#endif
	return TRUE;
}

BOOL KGC_DataPoolManager::ReportMemoryUse() 
{
	KG_Printl("****************************Base Data Pool Memory Use Report****************************");
	KG_Printl("KMapIntInt: BlockSize - " << sizeof(KMapIntInt) << ",\tPeekCount - "
		<< m_mpIntIntPool.GetPeakValue() << ",\tFreeCount - "<< m_mpIntIntPool.GetFreeCount()
		<< ",\tIncBlock - " << m_mpIntIntPool.GetIncBlockCount());
	KG_Printl("KMapIntData: BlockSize - " << sizeof(KMapIntData) << ",\tPeekCount - "
		<< m_mpIntDataPool.GetPeakValue() << ",\tFreeCount - "<< m_mpIntDataPool.GetFreeCount()
		<< ",\tIncBlock - " << m_mpIntDataPool.GetIncBlockCount());
	KG_Printl("KMapStrInt: BlockSize - " << sizeof(KMapStrInt) << ",\tPeekCount - "
		<< m_mpStrIntPool.GetPeakValue() << ",\tFreeCount - "<< m_mpStrIntPool.GetFreeCount()
		<< ",\tIncBlock - " << m_mpStrIntPool.GetIncBlockCount());
	KG_Printl("KMapStrData: BlockSize - " << sizeof(KMapStrData) << ",\tPeekCount - "
		<< m_mpStrDataPool.GetPeakValue() << ",\tFreeCount - "<< m_mpStrDataPool.GetFreeCount()
		<< ",\tIncBlock - " << m_mpStrDataPool.GetIncBlockCount());

	KG_Printl("BUF_8:\tPeekCount - "
		<< m_mpBufPool8.GetPeakValue() << ",\tFreeCount - "<< m_mpBufPool8.GetFreeCount()
		<< ",\tIncBlock - " << m_mpBufPool8.GetIncBlockCount());
	KG_Printl("BUF_16:\tPeekCount - "
		<< m_mpBufPool16.GetPeakValue() << ",\tFreeCount - "<< m_mpBufPool16.GetFreeCount()
		<< ",\tIncBlock - " << m_mpBufPool16.GetIncBlockCount());
	KG_Printl("BUF_32:\tPeekCount - "
		<< m_mpBufPool32.GetPeakValue() << ",\tFreeCount - "<< m_mpBufPool32.GetFreeCount()
		<< ",\tIncBlock - " << m_mpBufPool32.GetIncBlockCount());
	KG_Printl("BUF_64:\tPeekCount - "
		<< m_mpBufPool64.GetPeakValue() << ",\tFreeCount - "<< m_mpBufPool64.GetFreeCount()
		<< ",\tIncBlock - " << m_mpBufPool64.GetIncBlockCount());
	KG_Printl("BUF_128:\tPeekCount - "
		<< m_mpBufPool128.GetPeakValue() << ",\tFreeCount - "<< m_mpBufPool128.GetFreeCount()
		<< ",\tIncBlock - " << m_mpBufPool128.GetIncBlockCount());
	KG_Printl("BUF_256:\tPeekCount - "
		<< m_mpBufPool256.GetPeakValue() << ",\tFreeCount - "<< m_mpBufPool256.GetFreeCount()
		<< ",\tIncBlock - " << m_mpBufPool256.GetIncBlockCount());
	KG_Printl("BUF_512:\tPeekCount - "
		<< m_mpBufPool512.GetPeakValue() << ",\tFreeCount - "<< m_mpBufPool512.GetFreeCount()
		<< ",\tIncBlock - " << m_mpBufPool512.GetIncBlockCount());
	KG_Printl("BUF_1024:\tPeekCount - "
		<< m_mpBufPool1024.GetPeakValue() << ",\tFreeCount - "<< m_mpBufPool1024.GetFreeCount()
		<< ",\tIncBlock - " << m_mpBufPool1024.GetIncBlockCount());
	return TRUE;
}


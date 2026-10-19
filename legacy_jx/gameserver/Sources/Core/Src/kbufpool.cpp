/* -------------------------------------------------------------------------
//	文件名		：	kbufpool.cpp
//	创建者		：	luobaohang
//	创建时间	：	2007-3-7 17:12:30
//	功能描述	：	
//
// -----------------------------------------------------------------------*/

//#include "stdafx.h"
#include "KCore.h"
#include "utilities.h"
#include "kbufpool.h"


// -------------------------------------------------------------------------



// -------------------------------------------------------------------------

LPVOID KBufPool::_NewBufData( KE_BUFFER_TYPE eBufType, LPINT pBufID ) 
{
	switch(eBufType)
	{
	case BUF_8:
		return m_mpBufPool8.New(pBufID);
		break;
	case BUF_16:
		return m_mpBufPool16.New(pBufID);
		break;
	case BUF_32:
		return m_mpBufPool32.New(pBufID);
		break;
	case BUF_64:
		return m_mpBufPool64.New(pBufID);
		break;
	case BUF_128:
		return m_mpBufPool128.New(pBufID);
		break;
	case BUF_256:
		return m_mpBufPool256.New(pBufID);
		break;
	case BUF_512:
		return m_mpBufPool512.New(pBufID);
		break;
	case BUF_1024:
		return m_mpBufPool1024.New(pBufID);
		break;
	default:
		_ASSERT(FALSE);
		break;
	}
	return NULL;
}

LPVOID KBufPool::_GetBufData( DATA_DESC desc ) 
{
	switch(desc.m_BufType)
	{
	case BUF_8:
		return m_mpBufPool8.GetObject(desc.m_nBufIndex);
	case BUF_16:
		return m_mpBufPool16.GetObject(desc.m_nBufIndex);
	case BUF_32:
		return m_mpBufPool32.GetObject(desc.m_nBufIndex);
	case BUF_64:
		return m_mpBufPool64.GetObject(desc.m_nBufIndex);
	case BUF_128:
		return m_mpBufPool128.GetObject(desc.m_nBufIndex);
	case BUF_256:
		return m_mpBufPool256.GetObject(desc.m_nBufIndex);
	case BUF_512:
		return m_mpBufPool512.GetObject(desc.m_nBufIndex);
	case BUF_1024:
		return m_mpBufPool1024.GetObject(desc.m_nBufIndex);
	default:
		ASSERT(FALSE);
		break;
	}
	return NULL;
}

BOOL KBufPool::InitBufPool() 
{
	m_mpBufPool8.Init(1024,1024);
	m_mpBufPool16.Init(1024,1024);
	m_mpBufPool32.Init(1024,1024);
	m_mpBufPool64.Init(1024,1024);
	m_mpBufPool128.Init(512,1024);
	m_mpBufPool256.Init(512,1024);
	m_mpBufPool512.Init(0,1024);
	m_mpBufPool1024.Init(0,1024);
	return TRUE;
}

VOID KBufPool::FreeBufData( DATA_DESC desc ) 
{
	switch(desc.m_BufType)
	{
	case BUF_8:
		m_mpBufPool8.Free(desc.m_nBufIndex);
		break;
	case BUF_16:
		m_mpBufPool16.Free(desc.m_nBufIndex);
		break;
	case BUF_32:	
		m_mpBufPool32.Free(desc.m_nBufIndex);
		break;
	case BUF_64:
		m_mpBufPool64.Free(desc.m_nBufIndex);
		break;
	case BUF_128:
		m_mpBufPool128.Free(desc.m_nBufIndex);
		break;
	case BUF_256:
		m_mpBufPool256.Free(desc.m_nBufIndex);
		break;
	case BUF_512:
		m_mpBufPool512.Free(desc.m_nBufIndex);
		break;
	case BUF_1024:
		m_mpBufPool1024.Free(desc.m_nBufIndex);
		break;
	default:
		KGC_Error((INT)desc.m_BufType);
		break;
	}
}

KE_BUFFER_TYPE GetPropReserveBufSize( size_t nSize ) 
{
	if (nSize > BUF_1024)
	{
		KGC_Error(nSize);
		return BUF_0;
	}
	CONST static KE_BUFFER_TYPE anReserveSize[] = { BUF_0,BUF_8,BUF_16,BUF_32,BUF_64,BUF_128,BUF_256,BUF_512,BUF_1024 };
	CONST static INT nReserveSize = sizeof(anReserveSize) / sizeof(INT);
	/*二分查找*/
	INT nLeft = 0;					//左端点
	INT nRight = nReserveSize;		//右端点
	INT nMid = nReserveSize >> 1;	//中点
	do 
	{
		if (nSize < (size_t)anReserveSize[nMid])
			nRight = nMid;
		else
			nLeft = nMid;
		nMid = (nLeft + nRight) >> 1;

	} while(nLeft < nMid);
	if (nSize <= (size_t)anReserveSize[nLeft])
		return anReserveSize[nLeft];
	if (nSize > (size_t)anReserveSize[nRight])
	{
		//KGC_Error(nSize << " " << nRight);
		return BUF_0;
	}
	return anReserveSize[nRight];	
}


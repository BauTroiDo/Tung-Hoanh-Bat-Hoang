/* -------------------------------------------------------------------------
//	文件名		：	helper/KSimpleBuffer.h
//	创建者		：	FanZai
//	创建时间	：	2006-5-29 17:40:24
//	功能描述	：	简单的缓冲区内存分配，没有线成安全。适用于单线程的协议发
//					送、数据库读写等重复利用较大内存块的情况。
//
// -----------------------------------------------------------------------*/
#ifndef __COMMONPARTS_KSIMPLEBUFFER_H__
#define __COMMONPARTS_KSIMPLEBUFFER_H__

// -------------------------------------------------------------------------

class KSimpleBuffer
{
private:
	LPBYTE	m_pBuffer;
	DWORD	m_dwSize;

public:
	KSimpleBuffer()
	{
		m_dwSize	= 1;
		m_pBuffer	= NULL;
	}
	KSimpleBuffer(DWORD dwSize)
	{
		m_dwSize	= 1;
		m_pBuffer	= NULL;
		GetBuffer(dwSize);
	}
	~KSimpleBuffer()
	{
		if (m_pBuffer)
			delete[] m_pBuffer;
	}

	// 取得足够大的Buffer，但不保证之前的数据会保留
	LPBYTE GetBuffer(DWORD dwSize)
	{
		if (m_dwSize < dwSize || !m_pBuffer)
		{
			if (dwSize >> 31)
			{
				_ASSERT(FALSE);	// 搞笑么~太大了
				return NULL;
			}
			if (m_pBuffer)
			{
				delete[] m_pBuffer;
				m_pBuffer	= NULL;
			}
			while (m_dwSize < dwSize)
				m_dwSize <<= 1;
			m_pBuffer	= new BYTE[m_dwSize];
		}
		return m_pBuffer;
	}

	// 扩大Buffer，保留之前的数据，但不保证指针不变
	LPBYTE NewSize(DWORD dwSize)
	{
		if (m_dwSize < dwSize || !m_pBuffer)
		{
			if (dwSize >> 31)
			{
				_ASSERT(FALSE);	// 搞笑么~太大了
				return NULL;
			}

			LPBYTE pOldBuffer	= m_pBuffer;
			DWORD dwOldSize		= m_dwSize;

			while (m_dwSize < dwSize)
				m_dwSize <<= 1;
			m_pBuffer	= new BYTE[m_dwSize];

			if (pOldBuffer)
			{
				if (m_pBuffer)
					memcpy(m_pBuffer, pOldBuffer, dwOldSize);
				delete[] pOldBuffer;
			}
		}
		return m_pBuffer;
	}
};

// -------------------------------------------------------------------------

#endif /* __COMMONPARTS_KSIMPLEBUFFER_H__ */

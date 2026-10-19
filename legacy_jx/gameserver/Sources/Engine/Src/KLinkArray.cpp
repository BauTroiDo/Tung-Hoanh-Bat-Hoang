#include "KWin32.h"
#include "KEngine.h"
#include "KLinkArray.h"
/*
KLinkArray::~KLinkArray()
{
	 if (pNodeL) 
	 {
		 delete [] pNodeL;
		 pNodeL = NULL;
	 }
}

void KLinkArray::Init(int nSize)
{
	if (pNodeL)
	{
		delete [] pNodeL;
		pNodeL = NULL;
		nCount = 0;
	}

	pNodeL = new KLinkNode[nSize];

	if (nSize == m_nSize)
	{
		RemoveAll();
	}
	else
	{
		Clear();
		pNode = new KLinkNode[nSize];
		if (pNode)
			m_nSize = nSize;
	}
	return (pNode != NULL);
}

void KLinkArray::Remove(int nIdx)
{
	if (!nIdx)
		return;
	
	if ((pNodeL[nIdx].nNext == 0 && pNodeL[0].nPrev != nIdx) || (pNodeL[nIdx].nPrev == 0 && pNodeL[0].nNext != nIdx))
	{
		printf("--[error]Node:%d Remove twice--\n", nIdx);
		return;
	}

	pNodeL[pNodeL[nIdx].nPrev].nNext = pNodeL[nIdx].nNext; //备份下一个的数据
	pNodeL[pNodeL[nIdx].nNext].nPrev = pNodeL[nIdx].nPrev; //备份下一个的数据
	pNodeL[nIdx].nNext = 0;  //当前数据设置为0；
	pNodeL[nIdx].nPrev = 0;
	nCount--;
}

void KLinkArray::Insert(int nIdx)
{
	if (!nIdx)  //>0的
		return;

	if (pNodeL[nIdx].nNext != 0 || pNodeL[nIdx].nPrev != 0)
	{
		printf("--[error]Node:%d Insert twice--\n", nIdx);
		return;
	}

	pNodeL[nIdx].nNext = pNodeL[0].nNext;  //1=0,2=1
	pNodeL[pNodeL[0].nNext].nPrev = nIdx;
	pNodeL[0].nNext = nIdx;
	pNodeL[nIdx].nPrev = 0;
	nCount++;
}
*/


inline KLinkArray::~KLinkArray()
{
	Clear();
}

inline void KLinkArray::Clear()
{
	if (pNode) 
	{
		m_nCount = 0;
		m_nSize = 0;
		delete [] pNode;
		pNode = NULL;
	}
}

inline bool KLinkArray::Init(int nSize)
{
	if (nSize == m_nSize)
	{
		RemoveAll();
	}
	else
	{
		Clear();
		pNode = new KLinkNode[nSize];
		if (pNode)
			m_nSize = nSize;
	}
	return (pNode != NULL);
}

inline void KLinkArray::Remove(int nIdx)
{
	if (NULL==pNode || nIdx <= 0 || nIdx >= m_nSize)
	{
		//g_DebugLog("[error]Remove invalid node: index %d, size %d", nIdx, m_nSize);
		return;
	}

	if ((pNode[nIdx].nNext == 0 && pNode[0].nPrev != nIdx) || (pNode[nIdx].nPrev == 0 && pNode[0].nNext != nIdx))
	{
		//g_DebugLog("[error]Node:%d Remove twice", nIdx);
		return;
	}

	pNode[pNode[nIdx].nPrev].nNext = pNode[nIdx].nNext;
	pNode[pNode[nIdx].nNext].nPrev = pNode[nIdx].nPrev;
	pNode[nIdx].nNext = 0;
	pNode[nIdx].nPrev = 0;
	m_nCount--;
}

inline void KLinkArray::Insert(int nIdx)
{
	if (NULL==pNode || nIdx <= 0 || nIdx >= m_nSize)
	{
		//g_DebugLog("[error]Insert invalid node: index %d, size %d", nIdx, m_nSize);
		return;
	}

	if (pNode[nIdx].nNext != 0 || pNode[nIdx].nPrev != 0)
	{
		//g_DebugLog("[error]Node:%d Insert twice", nIdx);
		return;
	}

	pNode[nIdx].nNext = pNode[0].nNext;
	pNode[pNode[0].nNext].nPrev = nIdx;
	pNode[0].nNext = nIdx;
	pNode[nIdx].nPrev = 0;
	m_nCount++;
}

inline void KLinkArray::InsertAll()
{
	if  (NULL==pNode) return;

	if (m_nSize != m_nCount)
	{
		for (int i = 0; i < m_nSize; i++)
		{
			pNode[i].nNext = i + 1;
			pNode[i].nPrev = i - 1;
		}
		pNode[0].nPrev = 0;
		pNode[m_nSize - 1].nNext = 0;
		m_nCount = m_nSize;
	}
}

inline void KLinkArray::RemoveAll()
{
	if  (NULL==pNode) return;

	if (m_nCount)
	{
		int nNext = pNode[0].nNext;
		while(m_nCount > 0)
		{
			int nNextNext = pNode[nNext].nNext;
			pNode[nNext].nNext = 0;
			pNode[nNext].nPrev = 0;
			nNext = nNextNext;
			m_nCount --;
		}
		pNode[0].nNext = 0;
		pNode[0].nPrev = 0;
	}
}
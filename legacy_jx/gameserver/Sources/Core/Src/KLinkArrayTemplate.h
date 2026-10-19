#ifndef KLINKARRAYTEMPLATE_H
#define KLINKARRAYTEMPLATE_H

template <class T, UINT ulSize>
class KLinkArrayTemplate 
{
private:
	UINT			m_ulTotalSize;
public:
	UINT			m_ulFreeSize;
	T m_Data[ulSize];		                                    //0无效
	KLinkArray		m_FreeIdxTemplate;							//	可用表
	KLinkArray		m_UseIdxTemplate;	

	T *  Add()
	{
		UINT ulIndex = 0;
		FindFree(ulIndex);
		if (!ulIndex)
			return NULL;
		
		m_ulFreeSize --;
		
		m_FreeIdxTemplate.Remove(ulIndex);
		m_UseIdxTemplate.Insert(ulIndex);
		return &m_Data[ulIndex];
	}
	
	UINT Add(T * pT)
	{
		UINT ulIndex = 0;
		FindFree(ulIndex);
		if (!ulIndex)
			return 0;
		m_Data[ulIndex] = *pT;
		m_ulFreeSize --;
		
		m_FreeIdxTemplate.Remove(ulIndex);
		m_UseIdxTemplate.Insert(ulIndex);
		return ulIndex;
	}

	UINT Remove(T *pSame)
	{
		UINT ulSameIndex = FindSame(pSame);
		if (!ulSameIndex) 
			return 0;
		return Remove(ulSameIndex);
	}

	UINT Remove(UINT ulIndex)
	{
		if (ulIndex > m_ulTotalSize)
			return 0;
		m_UseIdxTemplate.Remove(ulIndex);
		m_FreeIdxTemplate.Insert(ulIndex);
		m_ulFreeSize ++;
		return ulIndex;
	}

	void Clear()
	{
		while(1)
		{
			int nIdx = m_UseIdxTemplate.GetNext(0);
			if (nIdx)
			{
				m_UseIdxTemplate.Remove(nIdx);
				m_FreeIdxTemplate.Insert(nIdx);
			}
			else
			{
				break;
			}
		}
		m_ulFreeSize = ulSize - 1;
	}

	KLinkArrayTemplate()
	{//构造函数
		m_ulTotalSize = m_ulFreeSize = ulSize - 1;//由于index0是无效的，所以实际数量要减一个
		m_FreeIdxTemplate.Init(ulSize);			  //决定分配内存的多少
		m_UseIdxTemplate.Init(ulSize);
		for (int i = ulSize - 1; i > 0 ; i --)
		{
			m_FreeIdxTemplate.Insert(i);
		}
		Clear();
	}

	T * FindFree(UINT & ulIndex) 
	{
		ulIndex =  m_FreeIdxTemplate.GetNext(0);
		if (!ulIndex)
			return NULL;
		return &m_Data[ulIndex];
	};

	T * GetData(UINT ulIndex)  
	{
		if (ulIndex > m_ulTotalSize || ulIndex == 0)
			return NULL;
		return &m_Data[ulIndex];
	}
	
	T * GetData(T * pT) 
	{
	
		unsigned long ulIndex = FindSame(pT);
		if (!ulIndex)
			return  NULL;
		return &m_Data[ulIndex];

	}

	UINT FindSame(UINT ulKey)
	{
		
		int nIdx = 0;
		while(1)
		{
			nIdx = m_UseIdxTemplate.GetNext(nIdx);
			if (!nIdx)
				break;
			
			if (ulKey == m_Data[nIdx].m_dwKey)
				return nIdx;
		}
	}

	UINT GetFreeCount() const
	{
		return m_ulFreeSize;
	};
	UINT GetUsedCount() const
	{
		//_ASSERT(m_ulTotalSize >= m_ulFreeSize);
		if  (m_ulTotalSize >= m_ulFreeSize)
		    return m_ulTotalSize - m_ulFreeSize;
		else
			return 0;
	};
	UINT GetTotalCount() const 
	{
		return m_ulTotalSize;
	};

	virtual	UINT FindSame(T *pSame) = 0;
};

#endif

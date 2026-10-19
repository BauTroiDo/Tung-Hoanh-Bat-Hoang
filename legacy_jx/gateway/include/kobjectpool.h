/* -------------------------------------------------------------------------
//	文件名		：	kobjectpool.h
//	创建者		：	luobaohang
//	创建时间	：	2006-12-18 11:31:25
//	功能描述	：	高效对象池类
//
// -----------------------------------------------------------------------*/
#ifndef __KOBJECTPOOL_H__
#define __KOBJECTPOOL_H__

#include <vector>
#include <algorithm>
//#include "kobjectpool.h"

// -------------------------------------------------------------------------

#ifndef NULL
#define NULL 0
#endif

#define _WARNING	_ASSERT
/*如需要对象重用和释放时有特殊操作，特化下面两函数*/
template <typename T> inline
VOID KOP_ReUse(T &widget) {};
template <typename T> inline
VOID KOP_UnUse(T &widget) {};

/*---------------------------------------------------------
//静态内存池类，只可申请空间而不能释放
//该类作用是既可实现动态分配的内存紧缩策略又可防止产生过多内存碎片
//适用于全局性的内存需求，但不能在编译期确定内存大小（所以不能用static内存）的情况
//--------------------------------------------------------*/
class KStaticMemPool
{
public:
	//构造函数预分配
	KStaticMemPool(INT size, INT increment) 
	{ _MemP = new CHAR[size]; MaxSize = size; curSize = 0; IncreSize = increment; NextBlock = NULL; }
	~KStaticMemPool()
	{ delete[] _MemP; if(NULL != NextBlock) delete NextBlock; }
	CHAR *GetBuf(INT blockSize)
	{
		if (blockSize > MaxSize)
			return NULL;
		if (curSize + blockSize > MaxSize) //如果当前总内存块不够 
		{
			if (NULL == NextBlock)
			{
				//开辟下一个内存池
				if(blockSize>IncreSize)
					NextBlock = new KStaticMemPool(blockSize, IncreSize);
				else 
					NextBlock = new KStaticMemPool(IncreSize, IncreSize);
			}
			//从下一个总内存池获取内存
			return NextBlock->GetBuf(blockSize);
		}
		curSize += blockSize;
		return _MemP+curSize-blockSize;
	}
protected:
	CHAR *_MemP; //总内存块首指针
	INT MaxSize; //总内存块的大小
	INT curSize; //已使用内存大小
	INT IncreSize; //增量大小
	KStaticMemPool *NextBlock; //下一个内存池指针
private:
};

//全局永久对象分配，外部无须负责对象的释放
template <class T>
class FeuNew
{
	//利用释构函数实现指针集的自动释放
	class vec_auto_release_ptr
	{
		VOID	AddPtr(T *p) { vec_ptr.push_back(p); }
		static VOID	_release(T *p) { delete p; }
		~vec_auto_release_ptr() { std::for_each(vec_ptr.begin(), vec_ptr.end(), _release); }
	private:
		friend class FeuNew<T>;
		std::vector<T*> vec_ptr;
	};
public:
	static T *New()
	{
		//利用static对象实现全局的生命期
		static vec_auto_release_ptr s_vecPtr;
		T *p = new T;
		if (p)
			s_vecPtr.AddPtr(p);
		return p; 
	}
};

/*-------------------------------------------------------------------------------
//名称：高效对象池类1
//功能：等长内存块缓冲式内存池。实现等长内存的快速申请和释放，避免系统级的内存碎片
//特性：有溢出检查，无空间扩充，无重复释放检查，无线程安全
//使用场合：能准确估计内存使用锋值，并能保证不重复释放，无需遍历所有已分配对象
//------------------------------------------------------------------------------*/
/*template <class T>
class KObjPool
{
public:
	//用Iterator而不直接用指针是大有好处的
	class Iterator
	{
	public:
		Iterator()
		{
			m_p = NULL;
		}
		//重载指针操作
		inline T * operator ->()
		{
			return m_p;
		}
		inline T & operator *()
		{
			return *m_p;
		}
		inline bool IsNil()
		{
			return m_p == NULL;
		}
		inline VOID SetNil()
		{
			m_p = NULL;
		}
		inline operator bool() { return m_p != NULL; }
		template <class T1>
		friend bool operator == (typename KObjPool<T1>::Iterator p1, typename KObjPool<T1>::Iterator p2);
	private:
		inline VOID Attach(T *p)
		{
			m_p = p;
		}
		T *m_p;
	};

public:
	KObjPool()
	{
		pObject = NULL;
		pFreeList = NULL;
		m_nMaxSize = 0;
		m_nIndex= 0;
	}
	//参数：预分配大小
	VOID Init(INT nSize)
	{
		pObject = new T[nSize];
		pFreeList = new Iterator[nSize];
		m_nMaxSize = nSize;
		m_nIndex = 0;
		INT i = 0;
		while (i < nSize)
		{
			pFreeList[i].Attach(pObject + i);
			i++;
		}
	}
	//从对象池获取对象
	inline Iterator New()
	{
		if (m_nIndex < m_nMaxSize)
		{
			//对象重用时执行的函数，使用reuse函数而不用placement-new是为了更灵活
			//对象重用和构造时通常不必是相同行为，重用通常只需做少量的初始化工作
			KOP_ReUse(*(pFreeList[m_nIndex]));
			return pFreeList[m_nIndex++];
		}
		_WARNING(!"Object pool overflow!");
		return Iterator();	//返回一个空的分配器
	}
	//归还释放对象
	inline VOID Free(Iterator &p)
	{
		if(m_nIndex > 0)
		{
			pFreeList[--m_nIndex].m_p = p.m_p;
			KOP_UnUse(*(pFreeList[m_nIndex]));
		}
		else
		{
			_WARNING(!"Wrong releasing!");
		}
	}
	~KObjPool()
	{
		if (pObject)
			delete[] pObject;
		if (pFreeList)
			delete[] pFreeList;
	}
protected:
	T* pObject;	            //内存首地址
	Iterator *pFreeList;	//可用表
	INT m_nMaxSize;
	INT	m_nIndex;
};

template<class T>
inline bool operator == (typename KObjPool<T>::Iterator p1, typename KObjPool<T>::Iterator p2)
{
	return p1.m_p == p2.m_p;
}
 */
/*-------------------------------------------------------------------------------
//名称：动态扩充高效内存池类
//功能：等长内存块缓冲式内存池。实现等长内存的快速申请和释放，避免系统级的内存碎片
//特性：有溢出检查，有空间扩充，无重复释放检查，无线程安全，优先使用主内存块，主内存
		块空间不够时使用附加内存块，比KObjPool要慢些
//应用场合：没有确定的内存使用锋值，使用Index申请和释放内存对象，确保不会重复释放
//------------------------------------------------------------------------------*/
template<class T>
class KIncObjPool1	//第一类，不管理ID，ID由外部提供，ID必须是对象索引值
{
public:
	KIncObjPool1()
	{
		pObject = NULL;
		m_nIncSize = 256;
		m_nMaxIndex = 0;
	}
	~KIncObjPool1()
	{
		if (pObject)
			delete[] pObject;
		for (typename std::vector<T*>::iterator it = m_vecPObject.begin(); it != m_vecPObject.end(); ++it)
		{
			if (*it)
				delete [] (*it);
		}
	}
	//参数：预分配大小、增量
	BOOL Init(INT nMaxSize, INT nIncSize)
	{
		m_nIncSize = nIncSize;
		if (m_nIncSize <= 0 )
		{
			ASSERT(FALSE);
			m_nIncSize = 256;
		}
		pObject = new T[nMaxSize];
		m_nMaxSize = nMaxSize;
		return pObject != NULL;
	}
	//从对象池获取对象
	T* New(INT nIndex)
	{
		if (!pObject || nIndex < 0)
		{
			ASSERT(FALSE);
			return NULL;
		}
		T *pRet;
		if (nIndex < m_nMaxSize)
		{
			pRet = pObject + nIndex;
		}
		else 
		{
			INT nIncIndex =  nIndex - m_nMaxSize;
			INT a = nIncIndex / m_nIncSize;
			INT b = nIncIndex % m_nIncSize;
			/*
			INT a,b;
			_asm{	//nIndex/m_nIncSize商和余数分别放在a,b
					mov eax, nIndex
					mov ecx, m_nIncSize
					cdq
					idiv ecx
					mov a, eax
					mov b, edx
			}
			*/
			if ((size_t)a < m_vecPObject.size())
			{
				pRet = m_vecPObject[a] + b;
			}
			else if((size_t)a == m_vecPObject.size())
			{
				pRet = new T[m_nIncSize];
				if (!pRet)
				{
					ASSERT(FALSE);
					return NULL;
				}
				m_vecPObject.push_back(pRet);
				pRet += b;
			}
			else	//大于一个IncSize则不让申请
			{
				ASSERT(FALSE);
				return NULL;
			}
		}
		KOP_ReUse(*pRet);
		if (nIndex > m_nMaxIndex)
			m_nMaxIndex = nIndex;
		return pRet;
	}
	//归还释放对象
	VOID Free(INT nIndex)
	{
		if(nIndex < m_nMaxSize)	//属于主内存块
		{
			KOP_UnUse(*(pObject + nIndex));
		}
		else
		{
			nIndex -= m_nMaxSize;
			KOP_UnUse(*(m_vecPObject[nIndex / m_nIncSize] + nIndex % m_nIncSize));
		}
	}
	T *GetObject(INT nIndex)
	{
		if (nIndex < 0 || nIndex > m_nMaxIndex)
		{
			ASSERT(FALSE);
			return NULL;
		}
		if (nIndex < m_nMaxSize)
			return pObject + nIndex;
		nIndex -= m_nMaxSize;
		INT a = nIndex / m_nIncSize;
		INT b = nIndex % m_nIncSize;
		/*
		INT a,b;
		_asm{	//nIndex/m_nIncSize商和余数分别放在a,b
			mov eax, nIndex
			mov ecx, m_nIncSize
			cdq
			idiv ecx
			mov a, eax
			mov b, edx
		}
		*/
		if ((size_t)a >= m_vecPObject.size())
		{
			ASSERT(FALSE);
			return NULL;
		}
		return m_vecPObject[a] + b;
	};
	//获取增量块数
	UINT GetIncBlockCount() { return (UINT)m_vecPObject.size(); }
	//获取new过的最大id
	INT GetPeakValue()
	{
		return m_nMaxIndex;
	}
protected:
	T* pObject;	//内存首地址
	INT m_nMaxSize; //主内存块大小
	INT	m_nIncSize; //增量内存块每个大小
	INT m_nMaxIndex; //当前最大内存id
	std::vector<T*> m_vecPObject;
};

template<class T>
class KIncObjPool2 :  private KIncObjPool1<T> //第二类，自身管理ID
{
public:
	KIncObjPool2() { m_IDGenerator = 1; }
	//参数：预分配大小、增量
	BOOL Init(INT nMaxSize, INT nIncSize)
	{
		m_IDGenerator = 1;	//从1开始
		m_vecFreeID.reserve(nMaxSize);
		return KIncObjPool1<T>::Init(nMaxSize, nIncSize);
	}
	INT GenNewID()
	{
		if (m_vecFreeID.size() > 0)
		{
			INT nRet = m_vecFreeID.back();
			m_vecFreeID.pop_back();
			return nRet;
		}
		return m_IDGenerator++;
	}
	BOOL FreeID(INT nIndex)
	{
		if (nIndex < 0 || nIndex >= m_IDGenerator)
		{
			ASSERT(FALSE);
			return FALSE;
		}
		m_vecFreeID.push_back(nIndex);
		return TRUE;
	}
	T* New(LPINT pID)
	{
		INT nNewID = GenNewID();
		if (pID)
			*pID = nNewID;
		return KIncObjPool1<T>::New(nNewID);
	}
	VOID Free(INT nIndex)
	{
		if (FreeID(nIndex))
			KIncObjPool1<T>::Free(nIndex);
	}
	T *GetObject(INT nIndex)
	{
		if (nIndex <= 0)
		{
			ASSERT(FALSE);
			return NULL;
		}
		return KIncObjPool1<T>::GetObject(nIndex);
	}
	//获取内存使用峰值（最大使用对象数）
	INT GetPeakValue()
	{
		return m_IDGenerator - 1;
	}
	//获取空闲可重用对象数
	INT GetFreeCount()
	{
		return (INT)m_vecFreeID.size();
	}
	UINT GetIncBlockCount()
	{
		return KIncObjPool1<T>::GetIncBlockCount();
	}
protected:
	std::vector<INT>	m_vecFreeID;
	INT					m_IDGenerator;
};


/*-------------------------------------------------------------------------------
//名称：动态重用式内存池类
//功能：等长内存块内存池。以重用的方式实现等长内存的快速申请和释放
//特性：不预分配，释放的对象（内存）不归还系统，留待重用，若无可重用内存则向系统申请
//应用场合：保证无重复释放和不会释放不是new出来的内存
//------------------------------------------------------------------------------*/
template<class T>
class KDynReUseObjectPool
{
public:
	~KDynReUseObjectPool()
	{
		typename std::vector<T*>::iterator itPObject;
		for (itPObject = m_vecFreePObject.begin(); itPObject != m_vecFreePObject.end(); ++itPObject)
		{
			if (*itPObject)
				delete (*itPObject);
		}
	}
	T *New()
	{
		if (m_vecFreePObject.size() <= 0)
			return new T;
		T *pRet = m_vecFreePObject.back();
		KOP_ReUse(*pRet);
		m_vecFreePObject.pop_back();
		return pRet;
	}
	VOID Free(T *p)
	{
		KOP_UnUse(*p);
		m_vecFreePObject.push_back(p);
	}
protected:
	std::vector<T*> m_vecFreePObject;
};
// -------------------------------------------------------------------------

#endif /* __KOBJECTPOOL_H__ */

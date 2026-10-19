/********************************************************************
	created:	2003/02/14
	file base:	OpaqueUserData
	file ext:	h
	author:		liupeng
	
	purpose:	
*********************************************************************/
#ifndef __INCLUDE_OPAQUEUSERDATA_H__
#define __INCLUDE_OPAQUEUSERDATA_H__

#if defined (_MSC_VER) && (_MSC_VER >= 1020)
	#pragma once
#endif

/*
 * Define Win64 interfaces if not already defined
 */

/*
 * InterlockedExchangePointer InterlockedExchange
   对32位应用程序来说，以上两个函数都用一个32位值替换另一个32位值，
   但对64位应用程序来说，InterlockedExchange替换的是32位值，
   而InterlockedExchangePointer替换的是64位值。
 */
#ifndef InterlockedExchangePointer
	#define InterlockedExchangePointer(Target, Value) \
		(PVOID)InterlockedExchange((PLONG)(Target), (LONG)(Value))  //InterlockedExchange64
#endif 

/*
 * namespace OnlineGameLib
 */

//namespace OnlineGameLib {

class COpaqueUserData 
{
public:
	
	/*
	 * Attempted to take the address of a non-lvalue
	 */
	void *GetUserPtr() const
	{
		return InterlockedExchangePointer( &(const_cast<void *>(m_pUserData)), m_pUserData );
	}
	
	/*
	 * Ignoring return value of function 
	 * Expected void type, assignment, increment or decrement
	 */
	void SetUserPtr(void *pData)
	{//替换原来的数据
		InterlockedExchangePointer( &m_pUserData, pData );
	}
	
	unsigned int GetUserData() const
	{//是不是会截断?
		return reinterpret_cast<unsigned int>( GetUserPtr() );
	}
	
	void SetUserData( unsigned int data )
	{
		SetUserPtr( reinterpret_cast<void *>( data ) );
	}
	
protected:
	
	COpaqueUserData() : m_pUserData(0)
	{
	}
	
	~COpaqueUserData()
	{
		m_pUserData = 0;
	}
	
private:
	
	void *m_pUserData;
	
	/*
	 * No copies do not implement
	 */
	COpaqueUserData( const COpaqueUserData &rhs );
	COpaqueUserData &operator=( const COpaqueUserData &rhs );
};

//} // End of namespace OnlineGameLib

#endif //__INCLUDE_OPAQUEUSERDATA_H__
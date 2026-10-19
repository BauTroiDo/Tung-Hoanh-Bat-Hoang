// SmartPtr.h: interface for the SmartPtr class.
//
//////////////////////////////////////////////////////////////////////

#ifndef SMART_PTR_H
#define SMART_PTR_H

template<class T> 
class SmartPtr
{ 
public:
	SmartPtr(T* realPtr = 0):pointee(realPtr)
	{

		m_count = 1 ; 
	}


	SmartPtr(const SmartPtr& rhs)
	{
		pointee = rhs.pointee;
	//	rhs.pointee =0;
		m_count = rhs.m_count ;
		m_count++;
	}

	 ~SmartPtr()
	{
		
		m_count-- ; 
		if ( (m_count == 0) && (pointee != 0) )
		{ 
			delete pointee; 
		} 
	}
	// make an assignment to a smart ptr
	SmartPtr& operator=(SmartPtr& rhs)
	{
		if (this == &rhs) 
			return *this;
		
		m_count-- ; 
		if ( m_count == 0 )
		{ 
			delete pointee; 
		} 

		pointee = rhs.pointee;
		m_count = rhs.m_count; 
		m_count++;

//		delete pointee;
//		pointee = rhs.pointee;
//		rhs.pointee = 0;
		return *this;
	}

	SmartPtr& operator=(T* realPtr)
	{
		if (realPtr == pointee) 
			return *this;
//		if(pointee != 0)
//		{
//			delete pointee;
//		}
		
		m_count-- ; 
		if ( m_count == 0 )
		{ 
			delete pointee; 
		}
		
		pointee = realPtr;
		m_count = 1;
		return *this;
	}

	T* operator->() const
	{
		return pointee;
	}


	T& operator*() const
	{
		return *pointee;
	}
	
	operator void*()
	{
		return pointee;
	}
	template<class newType> operator SmartPtr<newType>() 
	{
		return SmartPtr<newType>(pointee);
	}

	T* get() const { return pointee; }
	
protected:
	
	void reset(T *p = 0)
	{
	    if (pointee != p) 
		{
		   delete pointee;
		   pointee = p;
		}
		m_count = 1;
	}

    T* release()
	{
         T *oldPointee = pointee;
         pointee = 0;
         return oldPointee;
	}
private:
	T *pointee; 
	int m_count;
};

#endif // !defined(AFX_SMARTPTR_H__AEC0BC8F_28F7_4E3B_9BC7_2DEB27899519__INCLUDED_)

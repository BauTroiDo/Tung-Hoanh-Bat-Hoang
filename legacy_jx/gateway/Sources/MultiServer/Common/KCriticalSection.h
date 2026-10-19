//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   KCriticalSection.h
//  Version     :   1.0
//  Creater     :   Freeway Chen
//  Date        :   2003-9-2 17:17:16
//  Comment     :   参考了刘鹏的的实现 
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _KCRITICALSECTION_H_
#define _KCRITICALSECTION_H_
//1
#include "Utils.h"

class KCriticalSection_
{

private:
	//CRITICAL_SECTION	m_CriticalSection;
#ifndef WIN32
	 pthread_mutex_t      m_CriticalSection;
#else
	 CRITICAL_SECTION     m_CriticalSection;		//操作包文件时的临界区控制
#endif

public:
    KCriticalSection_();
   /* {
      #if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
        InitializeCriticalSection(&m_CriticalSection);
      #else
        pthread_mutex_init(&m_CriticalSection,NULL);
      #endif
    }*/

    ~KCriticalSection_();
    /*{
       //DeleteCriticalSection(&m_CriticalSection);
     #if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
        DeleteCriticalSection(&m_CriticalSection);
     #else
        pthread_mutex_destroy(&m_CriticalSection);
     #endif
    }*/

    int Lock();
    /*{
      //  EnterCriticalSection(&m_CriticalSection);
       #if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
          EnterCriticalSection(&m_CriticalSection);
       #else
          pthread_mutex_lock(&m_CriticalSection);
       #endif

       return true;
    }*/

    int UnLock();
  /*  {
        //LeaveCriticalSection(&m_CriticalSection);
      #if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
        LeaveCriticalSection(&m_CriticalSection);
      #else
         pthread_mutex_unlock(&m_CriticalSection);
      #endif
      return false;
    }*/
};

class KAutoCriticalSection_
{
    KCriticalSection_ &m_CriticalSection;

public:
    KAutoCriticalSection_(KCriticalSection_ &CriticalSection);
	/*
        : m_CriticalSection(CriticalSection)
    {
        m_CriticalSection.Lock();
    }*/

    ~KAutoCriticalSection_();
	/*
    {
        m_CriticalSection.UnLock();
    }*/
};

#endif


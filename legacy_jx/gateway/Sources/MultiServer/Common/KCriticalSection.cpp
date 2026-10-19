//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   KCriticalSection.h
//  Version     :   1.0
//  Creater     :   Freeway Chen
//  Date        :   2003-9-2 17:17:16
//  Comment     :   参考了刘鹏的的实现 
//
//////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "KCriticalSection.h"

 KCriticalSection_::KCriticalSection_()
 {
      #ifdef WIN32
        ::InitializeCriticalSection(&m_CriticalSection);
      #else
        pthread_mutex_init(&m_CriticalSection,NULL);
      #endif
 }

 KCriticalSection_::~KCriticalSection_()
 {
       //DeleteCriticalSection(&m_CriticalSection);
    #ifdef WIN32
        ::DeleteCriticalSection(&m_CriticalSection);
     #else
        pthread_mutex_destroy(&m_CriticalSection);
     #endif
 }

 int KCriticalSection_::Lock()
 {
      //  EnterCriticalSection(&m_CriticalSection);
       #ifdef WIN32
          ::EnterCriticalSection(&m_CriticalSection);
       #else
          pthread_mutex_lock(&m_CriticalSection);
       #endif

       return true;
 }

 int KCriticalSection_::UnLock()
 {
        //LeaveCriticalSection(&m_CriticalSection);
      #ifdef WIN32
        ::LeaveCriticalSection(&m_CriticalSection);
      #else
         pthread_mutex_unlock(&m_CriticalSection);
      #endif
      return false;
 }


KAutoCriticalSection_::KAutoCriticalSection_(KCriticalSection_ &CriticalSection)
        : m_CriticalSection(CriticalSection)
{
        m_CriticalSection.Lock();
}

KAutoCriticalSection_::~KAutoCriticalSection_()
{
        m_CriticalSection.UnLock();
}

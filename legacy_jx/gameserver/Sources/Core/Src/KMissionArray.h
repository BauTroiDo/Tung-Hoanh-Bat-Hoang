#ifndef KMISSIONARRAY_H
#define KMISSIONARRAY_H

#include "KTaskFuns.h"
#include "KMission.h"

#define MAX_MISSIONARRAY_VALUE_COUNT 200  //世界全局函数限制
//管理全局Mission的集合
template <class T, UINT ulSize>
class KMissionArray : public KLinkArrayTemplate<T,  ulSize>
{
	int	 m_MissionValue[MAX_MISSIONARRAY_VALUE_COUNT];
	char m_MissionValueS[MAX_MISSIONARRAY_VALUE_COUNT][1024];	  //4*1000*32 
public:
	BOOL	Activate()
	{
		UINT ulActiveCount = GetUsedCount();
		if (ulActiveCount == 0)
			return FALSE;
		
		int nIdx = 0;		
		for (int i = 0; i < ulActiveCount ; ++i)
		{
			nIdx = m_UseIdxTemplate.GetNext(nIdx);
			_ASSERT(nIdx);
			if (!nIdx)
			{
				break;
			}
			m_Data[nIdx].Activate();
		}
		return TRUE;
	};
	
	void SetMissionValue(UINT ulValueId, int nValue)
	{
		if (ulValueId >= MAX_MISSIONARRAY_VALUE_COUNT)
			return ;
		m_MissionValue[ulValueId] = nValue;
	};
	
	int GetMissionValue(UINT ulValueId) const
	{
		if (ulValueId >= MAX_MISSIONARRAY_VALUE_COUNT)
			return 0;
		return m_MissionValue[ulValueId];
	};
	//---------------------------------
	void SetMissionValueS(UINT ulValueId,const char *nValue)
	{
		if (ulValueId >= MAX_MISSIONARRAY_VALUE_COUNT)
			return ;
		//m_MissionValue[ulValueId] = nValue;
		//sprintf(m_MissionValueS[ulValueId],nValue);
		if  (nValue)
		  strcpy(m_MissionValueS[ulValueId], nValue);
		else
		   memset(m_MissionValueS[ulValueId],0,sizeof(m_MissionValueS[ulValueId]));

	};
	
   const char * GetMissionValueS(UINT ulValueId) const
	{
		if (ulValueId >= MAX_MISSIONARRAY_VALUE_COUNT)
			return "";
		return m_MissionValueS[ulValueId];
	};
	//----------------------------------
	UINT FindSame(T *pMission)
	{
		int nIdx = 0;
		while(1)
		{
			nIdx = m_UseIdxTemplate.GetNext(nIdx);
			if (!nIdx)
				break;
			
			if (pMission->GetMissionId() == m_Data[nIdx].GetMissionId())
				return nIdx;
		}
		return 0;
	};
	BOOL RemovePlayer(UINT ulPlayerIndex,int nSubWorlIdx)
	{
		UINT ulActiveCount = GetUsedCount();
		if (ulActiveCount == 0)
			return FALSE;
		
		int nIdx = 0;		
		for (int i = 0; i < ulActiveCount ; i ++)
		{
			nIdx = m_UseIdxTemplate.GetNext(nIdx);
			_ASSERT(nIdx);
			if (!nIdx)
			{
				break;
			}
			m_Data[nIdx].RemovePlayerc(ulPlayerIndex,0,nSubWorlIdx);
			//m_Data[nIdx].Remove(ulPlayerIndex);
		}
         printf("-- KMissionArray RemovePlayer Del:(%d) MAP:(%d) OK!!!..\n",ulActiveCount,nSubWorlIdx);	
		return TRUE;
	}       
	
};

//管理定时触发器的集合
template <class T, UINT ulSize, PF_TimerCallBackFun CallFun>
class KTimerFunArray :public KLinkArrayTemplate < T, ulSize>
{
public:
	//char m_szScriptFile[MAX_PATH];
	BOOL	Activate()
	{
		UINT ulActiveCount = GetUsedCount();
		if (ulActiveCount == 0)
			return FALSE;
		
		int nIdx = 0;		
		for (int i = 0; i < ulActiveCount ; i ++)
		{
			nIdx = m_UseIdxTemplate.GetNext(nIdx);
		//	_ASSERT(nIdx);
			if (!nIdx)
			{
				break;
			}
			m_Data[nIdx].Activate(CallFun);
		}
		return TRUE;
	};

	UINT FindSame(T *pTask)
	{
		int nIdx = 0;
		while(1)
		{
			nIdx = m_UseIdxTemplate.GetNext(nIdx);
			if (!nIdx)
				break;

			if (pTask->GetTaskId() == m_Data[nIdx].GetTaskId())
				return nIdx;
		}
		return 0;
	};

};
#endif

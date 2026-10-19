#include "KCore.h"
#include "KObjSet.h"
#include "KNpcSet.h"
#include "KSubWorld.h"
#include "KNpc.h"
#include "KIniFile.h"
#include "KSubWorldSet.h"
#include "KMissleSet.h"
#include "LuaFuns.h"
#include "KPlayerSet.h"
#include "KPlayer.h"
/////////////后来加/////////////////
#include  <time.h>
#include "KTimer.h"
#include "KSortScript.h"
#include "KScriptValueSet.h"
#include <ppl.h>
//#include <omp.h>
//#include "KNpcTemplate.h"
//extern KNpcTemplate	* g_pNpcTemplate[MAX_NPCSTYLE][MAX_NPC_LEVEL]; //0,0为起点
///////////////////////////////////
KSubWorldSet g_SubWorldSet;

KSubWorldSet::KSubWorldSet()
{
	m_dwCurrentTime = 0;
	//m_TaskTimer.Start();
	m_nLoopRate = 0;   //循环的帧数 18帧为一秒
#ifdef _SERVER
	nSubWorldLoop=0;
	m_nTaskLoopRate=0;
	//m_MiaoLoopRate=0;
	//nMapLoopOpen.clear();
	nMapLoopTask.clear();
	//nIsClose.clear();
	/*int i;
	for (i=0;i<MAX_TASK_COUNT+1;++i)
	{
		nMapLoopClose[i]=0;
		nMapLoopOpen[i]=60;
		nIsClose[i]=0;
	}*/

	m_nGameVersion = ITEM_VERSION;
	//ZeroStruct(EndTime);
	//ZeroStruct(StartTime);
	IsStart=0;
	//IsYes=0;
	nCount=0;
	//ZeroStruct(nJGTimes);
	//nStartHour=0;
	//nStartMin=0;
	//nEndHour=-1;
    //nEndMin=-1;
	//nIsOpen=0;
#endif

#ifndef _SERVER
	m_dwPing = 0;
#endif
}

//任务自动激活系统
#ifdef _SERVER
///////////////////////////////TaskMainLoop////////////////////////////////////////
//int KSubWorldSet::TaskMainLoop()
//{
//	
//   /*   timeinfo->tm_year+1900 当年 
//	    timeinfo->tm_mon + 1   当月 
//		timeinfo->tm_mday      当天  
//		timeinfo->tm_hour      当时 
//		timeinfo->tm_min       分  
//		timeinfo->tm_sec       秒
//  */
//	++m_nTaskLoopRate;
//
//	if (m_nTaskLoopRate>=10/* && (m_nTaskLoopRate-m_nTaskLoopRate>>4<<4)==0*/)  //每秒运行一次
//	{
//	  time_t rawtime;
//	  struct tm * timeinfo;
//	  time(&rawtime);    //距离现在的时间（秒）
//	  timeinfo = localtime(&rawtime); //当前时间
//	  //m_MiaoLoopRate ++;	//秒的循环
//	 
//      char nfiled[125]={0};
//	  sprintf(nfiled,"\\script\\task\\goldthread.lua");	   //全局线程
//	  SubExecuteScriptA(nfiled,"goldthread",99);
//
//	  if (nCount>0)
//	  {
//		  _MapLoopTaskInfo::iterator it;
//		  for(it = nMapLoopTask.begin();it!= nMapLoopTask.end();++it)
//		  {
//			  if (it->second.nIdx>0 && it->second.nIdx<=MAX_TASK_COUNT)
//			  {
//				  if (it->second.nIsOpen<=0)
//				  {
//					  it->second.nCloseLoop = 0;
//					  it->second.nOpenLoop  = 60;
//					  it->second.nState     = 0;
//					  continue;
//				  }
//				  timeinfo = localtime(&rawtime); //当前时间
//				  if (it->second.nJGTimes[0]>0  && it->second.nState==1 && rawtime>=it->second.nCloseLoop)
//				  {//已经开始了 还没有准时关闭的 就关闭它	
//					  it->second.nCloseLoop = 0;
//					  it->second.nState=2;   //已经关闭
//
//					  if  (it->second.nJGTimes[1]>0)
//					  {
//						  SubExecuteScriptA(it->second.nLuaField,"colsetask",it->second.nIdx,NULL/*it->second.nNextTaskKey*/);
//						  printf("第(%d)个关闭成功,持续时间:%d分,名称:%s\n",it->second.nIdx,it->second.nJGTimes[1],it->second.ntaskName);
//					  }
//				  }
//				  else if (it->second.EndTime[0]>0 && it->second.nState==1 && timeinfo->tm_mon + 1==it->second.EndTime[1]&&timeinfo->tm_mday==it->second.EndTime[2] && timeinfo->tm_hour==it->second.EndTime[3]&& timeinfo->tm_min >= it->second.EndTime[4])  // && timeinfo->tm_sec==0
//				  {
//					  it->second.nState=2;
//					  SubExecuteScriptA(it->second.nLuaField,"colsetask",it->second.nIdx,NULL/*it->second.nNextTaskKey*/);
//					  printf("第(%d)个关闭成功,结束:%d年%d月%d日%d时%d分,名称:%s\n",it->second.nIdx,it->second.EndTime[0],it->second.EndTime[1],it->second.EndTime[2],it->second.EndTime[3],it->second.EndTime[4],it->second.ntaskName);
//				  }
//				  else if ((it->second.nEndHour>=0 || it->second.nEndMin>=0) && it->second.nState==1 && timeinfo->tm_hour==it->second.nEndHour && timeinfo->tm_min >= it->second.nEndMin)// && timeinfo->tm_sec==0
//				  {//准时结束
//					  //IsOn++;
//					  it->second.nState=2;
//					  SubExecuteScriptA(it->second.nLuaField,"colsetask",it->second.nIdx,NULL/*it->second.nNextTaskKey*/);
//					  printf("第(%d)个执行成功,结束:%d时%d分,每天定时结束,名称:%s\n",it->second.nIdx,it->second.nEndHour,it->second.nEndMin,it->second.ntaskName);
//				  }
//				  else if (it->second.StartTime[0]>0 && it->second.nState==0 && timeinfo->tm_mon + 1==it->second.StartTime[1] && timeinfo->tm_mday==it->second.StartTime[2] && timeinfo->tm_hour==it->second.StartTime[3]&& timeinfo->tm_min>=it->second.StartTime[4])// && timeinfo->tm_sec==0
//				  {//指定时间执行一次
//					  it->second.nState=1;
//					  SubExecuteScriptA(it->second.nLuaField,"taskcontent",it->second.nIdx,NULL/*it->second.nNextTaskKey*/);
//					  printf("第(%d)个执行成功,开始:%d年%d月%日%d时%d分执行一次,名称:%s\n",it->second.nIdx,it->second.StartTime[0],it->second.StartTime[1],it->second.StartTime[2],it->second.StartTime[3],it->second.StartTime[4],it->second.ntaskName);
//				  }
//				  else if ((it->second.nStartHour>0 || it->second.nStartMin>0) && it->second.nState==0 && timeinfo->tm_hour==it->second.nStartHour && timeinfo->tm_min >= it->second.nStartMin ) //timeinfo->tm_sec==0
//				  {//单独时间大于零的//每天定时执行一次
//					  //	 IsOn++;
//					  SubExecuteScriptA(it->second.nLuaField,"taskcontent",it->second.nIdx,NULL/*it->second.nNextTaskKey*/);
//					  it->second.nState=1;
//					  printf("第(%d)个执行成功,开始:%d时%d分,定时一次,名称:%s\n",it->second.nIdx,it->second.nStartHour,it->second.nStartMin,it->second.ntaskName);
//				  }
//				  else if (it->second.nJGTimes[0]>0 && (it->second.nState==2 || it->second.nState==0) && rawtime>=it->second.nOpenLoop) //每隔多少分钟执行一次
//				  {//单独间隔时间大于零的
//					  char strTime[80]={0},nNextTime[80]={0};
//					  timeinfo = localtime(&rawtime); //当前时间
//					  strftime(strTime,sizeof(strTime),"%H:%M:%S",timeinfo); //"%Y:%m:%d %H:%M:%S"
//
//					  it->second.nCloseLoop = rawtime+it->second.nJGTimes[1]*60; //记录当前的秒数+间隔时间=下次关闭时间
//					  it->second.nOpenLoop  = rawtime+it->second.nJGTimes[0]*60; //下次开始的时间
//
//					  struct tm * Nexttimeinfo;
//					  Nexttimeinfo = localtime(&it->second.nOpenLoop); //当前时间
//					  strftime(nNextTime,sizeof(nNextTime),"%H:%M:%S",Nexttimeinfo);
//					  it->second.nState=1;
//					  SubExecuteScriptA(it->second.nLuaField,"taskcontent",it->second.nIdx,nNextTime/*it->second.nNextTaskKey*/);
//					  printf("第(%d)个执行成功,隔:%d分,名称:%s,cTime:%s,nTime:%s \n",it->second.nIdx,it->second.nJGTimes[0],it->second.ntaskName,strTime,nNextTime);
//				  }
//				  else if (it->second.nStartHour==0 && it->second.nStartMin==0 && it->second.nJGTimes[0]==0 && it->second.IsYes==0)
//				  {//时间都等于0的 就是开始就执行一次后就不执行了 
//					  it->second.IsYes++;
//					  SubExecuteScriptA(it->second.nLuaField,"taskcontent",it->second.nIdx,NULL/*it->second.nNextTaskKey*/);
//					  printf("第(%d)个执行成功,服务器开机运行一次,名称:%s\n",it->second.nIdx,it->second.ntaskName);
//				  }
//				  else
//				  {
//					  if (it->second.nJGTimes[0]>0 && rawtime > 60 && it->second.nOpenLoop==60)
//					  {
//						  char strTime[80]={0},nNextTime[80]={0};
//						  timeinfo = localtime(&rawtime); 
//						  strftime(strTime,sizeof(strTime),"%H:%M:%S",timeinfo); //"%Y:%m:%d %H:%M:%S"
//
//						  it->second.nCloseLoop = rawtime+it->second.nJGTimes[1]*60; //记录当前的秒数+间隔时间=下次关闭时间
//						  it->second.nOpenLoop  = rawtime+it->second.nJGTimes[0]*60;
//						  it->second.nState=0;
//
//						  struct tm * Nexttimeinfo;
//						  Nexttimeinfo = localtime(&it->second.nOpenLoop); //当前时间
//						  strftime(nNextTime,sizeof(nNextTime),"%H:%M:%S",Nexttimeinfo);
//
//						  SubExecuteScriptA(it->second.nLuaField,"taskcontent",it->second.nIdx,nNextTime/*it->second.nNextTaskKey*/);
//						  printf("第(%d)个<新>执行成功,隔:%d分,名称:%s,cTime:%s,nTime:%s \n",it->second.nIdx,it->second.nJGTimes[0],it->second.ntaskName,strTime,nNextTime);
//					  }
//				  }
//			  }//END IF
//		  }//END FOR
//	  }
//
//
//	  if(timeinfo->tm_hour==0 && timeinfo->tm_min==0 && IsStart==0)  //每天凌晨0点重置
//	  {
//		  IsStart++;
//		  
//		  m_nTaskLoopRate=0;
//		 // m_MiaoLoopRate=0;
//		  
//		 /* for (int i=0;i<MAX_TASK_COUNT+1;++i)
//		  {
//			  nMapLoopClose[i]=0;
//			  nMapLoopOpen[i]=60;
//			  nIsClose[i]=0;
//		  }*/
//		  _MapLoopTaskInfo::iterator itx;
//		  for(itx = nMapLoopTask.begin();itx!= nMapLoopTask.end();++itx)
//		  {
//			  if (itx->second.nIdx>0 && itx->second.nIdx<=MAX_TASK_COUNT)
//			  {
//				  itx->second.nCloseLoop=0;
//				  itx->second.nOpenLoop=60;
//				  itx->second.nState=0;
//			  }
//		  }
//		  
//		  g_RandomSeed(42);
//		  
//	  }
//	  
//	  if(timeinfo->tm_hour==0 && timeinfo->tm_min==2)  // 恢复原始状态
//		  IsStart=0;
//
//	} //END IF
//	return 1;
//}

bool KSubWorldSet::LoadTaskInfo()
{
	nMapLoopTask.clear();

	g_TaskLoop.GetInteger("List", "Count",1, &nCount);
	if (nCount>=MAX_TASK_COUNT)
		nCount = MAX_TASK_COUNT;

	if (nCount>0)
	{
		for (int i=0;i<nCount;++i)
		{
			char nTaskid[32]={0};
			snprintf(nTaskid, 32, "Task_%d",i);
			g_TaskLoop.GetInteger(nTaskid,"IsOpen",0,&nMapLoopTask[i+1].nIsOpen);
			if (nMapLoopTask[i+1].nIsOpen<=0)
			{
				nMapLoopTask[i+1].nIdx        = 0;
				nMapLoopTask[i+1].nCloseLoop  = 0;
				nMapLoopTask[i+1].nOpenLoop   = 60;
				nMapLoopTask[i+1].nState      = 0;
				continue;
			}

			nMapLoopTask[i+1].nIdx = i+1;
			g_TaskLoop.GetString(nTaskid, "TaskName","", nMapLoopTask[i+1].ntaskName,sizeof(nMapLoopTask[i+1].ntaskName));
			char nTempStr[64];
			ZeroMemory(nTempStr,sizeof(nTempStr));
			g_TaskLoop.GetString(nTaskid, "TaskFile","",nTempStr,sizeof(nTempStr));
			sprintf(nMapLoopTask[i+1].nLuaField,"\\script\\task\\%s",nTempStr);

			g_TaskLoop.GetInt2(nTaskid,"TaskInterval",nMapLoopTask[i+1].nJGTimes);//TaskTime
			g_TaskLoop.GetInteger2(nTaskid,"TaskTime",&nMapLoopTask[i+1].nStartHour,&nMapLoopTask[i+1].nStartMin);  //每天开始的时间
			g_TaskLoop.GetInteger2(nTaskid,"TaskETime",&nMapLoopTask[i+1].nEndHour,&nMapLoopTask[i+1].nEndMin);     //每天结束的时间
			g_TaskLoop.GetInt5(nTaskid,"TaskEnd",nMapLoopTask[i+1].EndTime);
			g_TaskLoop.GetInt5(nTaskid,"TaskStart",nMapLoopTask[i+1].StartTime);
			g_TaskLoop.GetString(nTaskid,"NextTaskKey","",nMapLoopTask[i+1].nNextTaskKey,sizeof(nMapLoopTask[i+1].nNextTaskKey));
			g_TaskLoop.GetInteger(nTaskid,"IsSaceSyn",0,&nMapLoopTask[i+1].nIsSaceSyn);
			
		}
	}

	return true;
}

#endif
////////////////////////////////////地图执行脚本//////////////////////////////////////

BOOL KSubWorldSet::SubExecuteScriptA(char * ScriptFileName, char * szFunName, int nParam,char * szCanshu)
{//g_FileName2Id(ScriptFileName)
	if (!ScriptFileName || !ScriptFileName[0] || !szFunName  || !szFunName[0]) return FALSE;
	char nNewScriptName[128]={0};
	sprintf_s(nNewScriptName,sizeof(nNewScriptName)-1,"%s",ScriptFileName);
	strlwr(nNewScriptName);//g_StrLower
	return SubExecuteScriptB(nNewScriptName, szFunName, nParam,szCanshu);	
}

BOOL KSubWorldSet::SubExecuteScriptB(char* dwScriptIdA,  char * szFunName, int nParam,char * szCanshu)
{
	   // KSubWorld nIndex;	
        if (!szFunName) return false;

		bool bExecuteScriptMistake = true;
		//创建一个新栈
		KLuaScript * pScript = (KLuaScript* )g_GetScript(dwScriptIdA); //dwScriptId
		int nTopIndex = 0;  //记录位置 （栈底）

		if (pScript==NULL)
		{
			printf(strCoreInfo[L_SortScript_4].c_str(),dwScriptIdA,szFunName);
			return FALSE;
		} 
	try
	{
		if (pScript)
		{	//向栈中压入一个int数字

		    nTopIndex=pScript->SafeCallBegin();
   
		    if (pScript->CallFunction(szFunName,0,"ds",nParam,szCanshu)) 
			{
				bExecuteScriptMistake = false;
			} 
			
           // lua_pop(pScript->m_LuaState, -1); //从栈顶弹出1个元素 -1清空栈
			nTopIndex=pScript->SafeCallBegin();
			pScript->SafeCallEnd(nTopIndex);
		    //lua_pop(pScript->m_LuaState, -1); //从栈顶弹出1个元素 -1清空栈
		//	lua_close(pScript->m_LuaState);   //释放内存
		}
		//lua_pop(pScript->m_LuaState, -1); //从栈顶弹出1个元素 -1清空栈   
		if (bExecuteScriptMistake)
		{
		    pScript=NULL;
			return FALSE;
		}

		pScript=NULL;
		return TRUE;
	}
	catch(...)
	{
		
		if (pScript)
		{
			nTopIndex=pScript->SafeCallBegin();	
			pScript->SafeCallEnd(nTopIndex);
		}

		printf("load script E,Script[%s]!\n",szFunName);
		return FALSE;
	}
	return TRUE;
}

BOOL	KSubWorldSet::SubExecuteScript(DWORD dwScriptId, char * szFunName, char *  szParams,char * snParams,int inParams,int njb,int njxb)
{
	KSubWorld nIndex;
	KNpc   m_Index;

		bool bExecuteScriptMistake = true;
		KLuaScript * pScript = (KLuaScript* )g_GetScript(dwScriptId);
		if (pScript==NULL)
		{
			printf(strCoreInfo[L_SortScript_5].c_str(),szFunName);
			return FALSE;
		}

		int nTopIndex = 0;
	 try
	 { 	
		if (pScript)
		{
		//	Lua_PushNumber(pScript->m_LuaState, nIndex.m_nIndex);
		//	pScript->SetGlobalName(SCRIPT_SUBWORLDINDEX);			
		     nTopIndex=pScript->SafeCallBegin();
			if ((!szParams) || !szParams[0]) 
			{//如果没有参数	 如果第一次为空，就执行空参数
				 if (pScript->CallFunction(szFunName,0,""))
				 {
					 bExecuteScriptMistake = false;
				 }
			}
		    else if (pScript->CallFunction(szFunName,0,"ssdddd",szParams,snParams,inParams,njb,njxb, 0))  //有参数
			{
				bExecuteScriptMistake = false;
			}
			//}
		   nTopIndex=pScript->SafeCallBegin();
		   pScript->SafeCallEnd(nTopIndex);
         //#define Lua_Release(L)					lua_close(L)
		//	lua_close(pScript->m_LuaState); //释放内存
		//lua_pop(pScript->m_LuaState, -1); //从栈顶弹出1个元素 -1清空栈
		}
		//lua_pop(pScript->m_LuaState, -1); //从栈顶弹出1个元素 -1清空栈
		if (bExecuteScriptMistake)
		{//执行脚本失败后，就清空
			pScript=NULL;
			return FALSE;
		}
		return TRUE;
	}
	catch(...)
	{
		if (pScript)
		{
			nTopIndex=pScript->SafeCallBegin();	
			pScript->SafeCallEnd(nTopIndex);		
			//pScript->Exit();
		}
		printf("load Script error,Script[%s],[%d]!\n",szFunName);		
		return FALSE;
	}
	return TRUE;
}

BOOL	KSubWorldSet::SubExecuteScript(char * ScriptFileName, char * szFunName, char *  szParams,char *  snParams, int inParams,int njb,int njxb)
{
	if (!ScriptFileName || !ScriptFileName[0] || !szFunName  || !szFunName[0]) return FALSE;
	
	char nCurFileName[128]={0};
	snprintf(nCurFileName, sizeof(nCurFileName), "%s", ScriptFileName);
	DWORD dwScriptId = g_FileName2Id(nCurFileName);

	KLuaScript * pScript = (KLuaScript* )g_GetScript(dwScriptId);
	if (pScript==NULL)
	{
		printf(strCoreInfo[L_SortScript_4].c_str(),ScriptFileName,szFunName);
		return FALSE;
	}
	pScript=NULL ;

	return SubExecuteScript(dwScriptId, szFunName, szParams,snParams,inParams,njb,njxb);
}
///////////////////////////////////////////////////////////////////////////////////////////////////
int KSubWorldSet::SearchWorld(DWORD dwID)
{
	for (int i = 0; i < MAX_SUBWORLD; ++i)
	{
		if ((DWORD)SubWorld[i].m_SubWorldID == dwID)
			return i;
	}
	return -1;
}

BOOL KSubWorldSet::EnsureMapLoaded(int nSubWorldIdx)
{
	if (nSubWorldIdx < 0 || nSubWorldIdx >= MAX_SUBWORLD)
		return FALSE;

	return SubWorld[nSubWorldIdx].EnsureMapLoaded();
}

BOOL KSubWorldSet::EnsureMapLoadedById(DWORD dwMapId)
{
	int nIdx = SearchWorld(dwMapId);
	if (nIdx < 0)
		return FALSE;

	return SubWorld[nIdx].EnsureMapLoaded();
}

int KSubWorldSet::SearchWorldRows(int dwID)
{
//	for (int i = 0; i < MAX_SUBWORLD; ++i)
//	{
	if (dwID>=0 && dwID<MAX_SUBWORLD)
		return (DWORD)SubWorld[dwID].m_SubWorldID;
//	}
	return -1;
}
///////////////////////////////////////////////////////////////////////////////////////////
//static CLogFile gs_LogFile;

BOOL KSubWorldSet::LoadSerMaps(LPSTR szFileName)
{
   KIniFile IniFile;
   char szKeyName[32];
   int nWorldID,nWorldCount = 0,nSucess = 0,nFail = 0;
   int nNpcmun=0,nZnNpcmun=0,nZnNpcmunA=0;
   IniFile.Load(szFileName);
   IniFile.GetInteger("Init", "Count", 1, &nWorldCount);
   if (nWorldCount >=MAX_SUBWORLD)
   {
      printf("----load map:Count:%d,MaxCount:%d-----\n", nWorldCount,MAX_SUBWORLD);
      IniFile.Clear();
      return FALSE;
   }

    printf("Loading File Map: %s\n", szFileName);

    for (int i = 0; i < nWorldCount; ++i)
	{ 
      sprintf((char*)szKeyName, "World%02d", i);
      IniFile.GetInteger("World", szKeyName,5000, &nWorldID);
#ifdef _SERVER
      SubWorld[i].m_nIndex = i;
      nNpcmun = SubWorld[i].LoadMapS(nWorldID);

	  ////开始设置各个地图的信息
	  //if (g_DbActivity)
	  //{
		 // ZCursorS *cursor = g_DbActivity->first();
		 // while(cursor)
		 // {
			//  TDBactivitydata* pMapData = (TDBactivitydata*)cursor->data;
			//  if (pMapData && pMapData->iid>0 && pMapData->nMapIndex==nWorldID)
			//  {
			//	  SubWorld[i].nWorldMapInfo[STR_MAP_MASTER]=pMapData->nOwerName;
			//	  printf("----地图归属:%s,nWorldID:%d --\n",pMapData->nOwerName,nWorldID);
			//  }
			//  if(!g_DbActivity->next(cursor))
			//	  break;
		 // }
	  //}
	  
      if (nNpcmun>=0)
	  {  
	    nSucess = nSucess + 1;
        printf("Loaded (%d) Map OK ID: %d,NpcNum:%d\n",i, nWorldID,nNpcmun);
        nZnNpcmun +=nNpcmun;
	  }  
      else
	  {  
	     nFail = nFail + 1;
         printf("Load (%d) Fail Map ID: %d,NpcNum:%d\n",i, nWorldID,nNpcmun);
	  } 
#endif
	  
}
      printf("Map Load Sucessed: %d Map,NpcCount:%d\n", nSucess,nZnNpcmun);
      printf("Map Load Failed: %d Map \n", nFail);
	  IniFile.Clear();
      return TRUE;
} 

//int nActiveRegionCount;
//客户端和 服务器端 公共调用函数---游戏主循环
void KSubWorldSet::MainLoop()
{
	++m_nLoopRate;
#ifdef _SERVER
	extern int g_nMapLoadsThisTick;
	g_nMapLoadsThisTick = 0;
#endif

#ifndef _SERVER
	// 游戏音乐的处理
	this->m_cMusic.Play(SubWorld[0].m_SubWorldID, SubWorld[0].m_dwCurrentTime, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_FightMode);

#endif
//全局函数作用  计时器管理
//#ifdef _SERVER
//		g_GlobalMissionArray.Activate();
//#endif

int i;

/*#ifdef _SERVER
g_GameSetTing.GetInteger("Gameconfig","SeparateWorldCount",10,&nSeparateWorldCount);
int nThreads=1;
g_GameSetTing.GetInteger("Gameconfig","CoreThreads",1,&nThreads);
omp_set_num_threads(nThreads);
#endif*/
//Concurrency::parallel_invoke();
//Concurrency::parallel_for_each();//容器类的
/*
if (MAX_SUBWORLD>=nSeparateWorldCount)
{
//#pragma omp parallel for
//printf("------------pragma omp parallel for------------------\n");
g_GameSetTing.GetInteger("Gameconfig","CoreIsOrdered",0,&nIsOrdered);

if (nIsOrdered)
{//有顺循环
//#pragma omp parallel
//{
//ordered  按照顺序执行 critical  定义临界区进行数据保护

//critical的语义是指在任意时刻只有一个（或是最多一个）线程在执行临界区内的代码，
//其目的是对于临界区内的线程之间共享的资源进行保护，比如几个线程对一个公共链表进行插入或删除操作，
//此时就需要使用critical, 否者该链表中的数据可能不一致。在OpenMP中，临界区的名字是可选的，
//所有的临界区都是外部链接的，也就是说是他们是公共变量，是所有线程可见的，
//不管该线程属于哪个组。对于没有指定名字的临界区，OpenMP编译时认为所有的没有指定名字的临界区，
//都对应一个外部（或全局）的临界区。不同于一些常见的OpenMP结构语句，
//临界区结构的末尾并没有一个隐含的同步路障。临界区的代码应该是一个块结构（structured block）,
//其中不允许有返回(return)， 退出（exit）, 或者从里面跳出(go to)语句。
//#pragma omp for ordered

#pragma omp parallel for ordered
	for (i = 0; i < MAX_SUBWORLD; ++i)        //服务器端 地图列表
	{
        #pragma omp ordered                           //按照顺序来执行
		{
			#pragma omp critical              //只能一个线程自行，数据保护
			{
				if (nSleepTime>0 && (i-i>>1<<1)==0)
				{
					Sleep(nSleepTime);
				}
				SubWorld[i].Activate();
				//printf("i = %d, I am Thread %d\n", i, omp_get_thread_num());
#ifndef _SERVER
				NpcSet.CheckBalance();               //处理同步不一致的NPC （删除）
#endif
			} //#pragma omp critical 
		}     //end  #pragma omp ordered  
	}         //end for
//}           //end  #pragma omp parallel
}
else
{//无顺循环

//#pragma omp parallel
//	{
//#pragma omp for
#pragma omp parallel for
		for (i = 0; i < MAX_SUBWORLD; ++i)        //服务器端 地图列表
		{
#pragma omp critical              //只能一个线程自行，数据保护
				{
					if (nSleepTime>0 && (i-i>>1<<1)==0)
					{
						Sleep(nSleepTime);
					}
					SubWorld[i].Activate();
					//printf("i = %d, I am Thread %d\n", i, omp_get_thread_num());
#ifndef _SERVER
					NpcSet.CheckBalance();               //处理同步不一致的NPC （删除）
#endif              
				} //#pragma omp critical 
		}         //end for
//	}             //end  #pragma omp parallel
}

}
else 
{*/
	for ( i = 0; i < MAX_SUBWORLD; ++i)       //服务器端 地图列表
	{
		SubWorld[i].Activate();
#ifndef _SERVER
		NpcSet.CheckBalance();               //处理同步不一致的NPC （删除）
#endif
	}
//}

//	PlayerSet.AutoSave(); //auto save data

	m_dwCurrentTime++;
 /*if (MAX_SUBWORLD>=nSeparateWorldCount)
 {//有BUG
	Concurrency::parallel_for(0,MAX_SUBWORLD,[&](int n)
	{  
		SubWorld[n].Activate();
	});
 }
 else
 {
	 for ( i = 0; i < MAX_SUBWORLD; ++i)       //服务器端 地图列表
	 {
		 SubWorld[i].Activate();
#ifndef _SERVER
		 NpcSet.CheckBalance();               //处理同步不一致的NPC （删除）
#endif
      }
 }*/
}
//地图消息处理循环
void KSubWorldSet::MessageLoop()
{
	
	for (int i = 0; i < MAX_SUBWORLD; ++i)
	{
		SubWorld[i].MessageLoop();
	}
}

BOOL KSubWorldSet::SendMessage(int nSubWorldID, DWORD dwMsgType, int nParam1, int nParam2, int nParam3)
{
	KWorldMsgNode *pNode = NULL;
	
	pNode = new KWorldMsgNode;
	if (!pNode)
		return FALSE;
	
	pNode->m_dwMsgType	= dwMsgType;
	pNode->m_nParam[0]	= nParam1;
	pNode->m_nParam[1]	= nParam2;
	pNode->m_nParam[2]	= nParam3;

	return SubWorld[nSubWorldID].m_WorldMessage.Send(pNode);
}
//离开游戏时调用的函数
void KSubWorldSet::Close()
{
	for (int i = 0; i < MAX_SUBWORLD; ++i)
	{
		SubWorld[i].Close();
	}
	NpcSet.RemoveAll(); //人物删除
#ifndef _SERVER

	Player[CLIENT_PLAYER_INDEX].m_ItemList.RemoveAll();  // 删除客户端全部物品
	Player[CLIENT_PLAYER_INDEX].m_cTeam.Release();
	g_Team[0].Release();
	m_cMusic.Stop();

#endif
}

#ifndef _SERVER
void KSubWorldSet::Paint()
{
	//SubWorld[0].Paint();
}
#endif

//重生点文件加载
#ifdef _SERVER
int KSubWorldSet::GetRevivalPosFromId(DWORD dwSubWorldId, int nRevivalId, POINT* pPos)
{
	if (!pPos)
		return nRevivalId;

//	int nIdx = SearchWorld(dwSubWorldId);
//	if (nIdx >= 0)
//	{
	KIniFile IniFile;
	char	szKeyName[32];
	char	szSection[32];
	
	g_SetFilePath("\\settings");
	
	IniFile.Load("RevivePos.ini");

	sprintf(szSection, "%d", dwSubWorldId);
	int nTempMin,nTempMax;
	if (nRevivalId<0)
	{
	    IniFile.GetInteger2(szSection,"region", &nTempMin, &nTempMax);
		nRevivalId = GetRandomNumber(nTempMin,nTempMax); //随机取一个
	}
	sprintf(szKeyName, "%d", nRevivalId);

/*
     if(!IniFile.IsSectionExist(szSection))
	 {//如果重生文件里米有这个地图的话 就设置默认的
        	sprintf(szSection, "%d",53);
	        sprintf(szKeyName, "%d",19);
	 }
*/   
	int nX = 51200;
	int nY = 102400;
	
	IniFile.GetInteger2(szSection, szKeyName, &nX, &nY);
	
	pPos->x = nX;
	pPos->y = nY;

	IniFile.Clear();

	return nRevivalId;
//	}
}
////
int KSubWorldSet::GetRevivalPosRandIdx(DWORD dwSubWorldId,POINT* pPos)
{
	if (!pPos)
	   return -1;

	KIniFile IniFile;
	char	szKeyName[32];
	char	szSection[32];
	
	g_SetFilePath("\\settings");
	
	IniFile.Load("RevivePos.ini");
	sprintf(szSection, "%d", dwSubWorldId);
	int nStart,nEnd;
	    IniFile.GetInteger2(szSection, "region", &nStart, &nEnd);
	int nRevivalId = GetRandomNumber(nStart,nEnd);

	sprintf(szKeyName, "%d", nRevivalId);
/*
     if(!IniFile.IsSectionExist(szSection))
	 {//如果重生文件里米有这个地图的话 就设置默认的
        	sprintf(szSection, "%d",53);
	        sprintf(szKeyName, "%d",19);
	 }
*/   
	int nX = 51200;
	int nY = 102400;

	IniFile.GetInteger2(szSection, szKeyName, &nX, &nY);
	
	pPos->x = nX;
	pPos->y = nY;

	IniFile.Clear();

	return nRevivalId;
//	}
}
#endif

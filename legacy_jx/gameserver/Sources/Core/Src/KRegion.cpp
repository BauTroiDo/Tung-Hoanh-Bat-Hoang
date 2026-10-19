#include "KCore.h"
#include "KNpc.h"
#include "KNpcSet.h"
#include "KMissleSet.h"
#include "KObjSet.h"
#include "KPlayerSet.h"
#include "KPlayer.h"
#include "KMissle.h"
#include "KObj.h"
#include "KWorldMsg.h"
#include "KSubWorld.h"
#include "KItem.h"   //--后来添加的
#ifdef _SERVER
//#include "KNetServer.h"
//#include "../MultiServer/Heaven/Interface/iServer.h"
#endif
#include "Scene/ObstacleDef.h"
//原来有的
#include "Scene/SceneDataDef.h"
#include "KRegion.h"
#include "MyAssert.H"
//#pragma pack(1);

KRegion::KRegion()
{
	m_nIndex		= -1;
	m_nSubMapidx    = -1;
	m_RegionID		= -1;
	m_nActive		= 0;
	m_nNpcSyncCounter = 0;
	m_nNpcSyncCounter_s=0;
	m_nObjSyncCounter = 0;
	//m_nLoopCounter    = 0;
	m_nWidth		= 0;
	m_nHeight		= 0;
	ZeroMemory(m_nConnectRegion, 8 * sizeof(int));
	memset(m_pConRegion, 0, sizeof(m_pConRegion));

	m_pNpcRef		= NULL;
	m_pObjRef		= NULL;
	/*m_pObstacleRef	= NULL;
	m_pMslRef		= NULL;*/

#ifdef _SERVER
	m_nNearbyPlayerCount	= 0;
	m_nNearbyActiveNpcCount = 0;
	m_eActType				= emACT_TYPE_NONE;
	memset(m_Obstacle, 0, sizeof(m_Obstacle));
	m_pdwTrap = NULL;
#endif
	m_cNpcRef.Clear();
	m_cObjRef.Clear();
}

KRegion::~KRegion()
{

	m_cNpcRef.Clear();
	m_cObjRef.Clear();
#ifdef _SERVER
	if (m_pdwTrap)
	{
		delete m_pdwTrap;
		m_pdwTrap = NULL;
	}
#endif

	/*if (m_pMslRef)
	delete [] m_pMslRef;*/
	if (m_pNpcRef)
		delete [] m_pNpcRef;
	if (m_pObjRef)
		delete [] m_pObjRef;
	/*if(m_pObstacleRef)
	delete [] m_pObstacleRef;*/

	m_pNpcRef		= NULL;
	m_pObjRef		= NULL;
	//m_pObstacleRef	= NULL;
	//m_pMslRef		= NULL;
 
}

BOOL KRegion::Init(int nWidth, int nHeight)
{
	m_nWidth	= nWidth;	//16
	m_nHeight	= nHeight;	//32

	m_cNpcRef.Clear();
	m_cObjRef.Clear();

	// P0 Optimization: Eliminate 1.4 million heap allocations of m_pNpcRef & m_pObjRef
	m_pNpcRef = NULL;
	m_pObjRef = NULL;

	return TRUE;
}

BOOL KRegion::Load(int nX, int nY)
{

#ifndef _SERVER
	Close();
#endif

	m_RegionID = MAKELONG(nX, nY);

	m_nRegionX = nX * 512;
	m_nRegionY = nY * 1024;

	// 下方
	m_nConRegionID[0] = MAKELONG(nX, nY + 1);
	// 左下方
	m_nConRegionID[1] = MAKELONG(nX - 1, nY + 1);
	// 左方
	m_nConRegionID[2] = MAKELONG(nX - 1, nY);
	// 左上方
	m_nConRegionID[3] = MAKELONG(nX - 1, nY - 1);
	// 上方
	m_nConRegionID[4] = MAKELONG(nX, nY - 1);
	// 右上方
	m_nConRegionID[5] = MAKELONG(nX + 1, nY - 1);
	// 右方
	m_nConRegionID[6] = MAKELONG(nX + 1, nY);
	// 右下方
	m_nConRegionID[7] = MAKELONG(nX + 1, nY + 1);

	return TRUE;
}

#ifdef _SERVER
//----------------------------------------------------------------------
//	功能：载入服务器端地图上本region 的 object数据（包括npc、trap、box等）
//	注意：使用此函数之前必须保证当前路径是本地图路径
//----------------------------------------------------------------------
int KRegion::LoadObjectS(int nSubWorld, int nX, int nY,KMapList* pMapListInfo)
{
	KPakFile	cData;
	char		szFilePath[MAX_SENTENCE_LENGTH]={0};
	char		szFile[MAX_SENTENCE_LENGTH]={0};
	int         nNpcNum;
	//int         nNpcWnum=0;
	int         nNpcRnum=0;
    char        szCurPath[MAXPATH]={0};
	//g_GetRootPath(szFile); //根目录
    g_GetFilePath(szCurPath);

	snprintf(szFilePath, sizeof(szFilePath), "\\%sv_%03d", szCurPath, nY);
    //g_SetFilePath(szFilePath);
	snprintf(szFile, sizeof(szFile), "%s\\%03d_%s", szFilePath, nX, REGION_COMBIN_FILE_NAME_SERVER);
	//printf("地图路径A：%s\n",szFile);

	//m_Obstacle.Clear();
	ZeroMemory(m_Obstacle, sizeof(m_Obstacle));

	if (cData.Open(szFile))  
	{
	//	printf("测试00000：%s \n",szFile);
		DWORD	dwHeadSize;
		DWORD	dwMaxElemFile = 0;
		KCombinFileSection	sElemFile[REGION_ELEM_FILE_COUNT];

		if (cData.Size() < sizeof(unsigned int) + sizeof(KCombinFileSection) * REGION_ELEM_FILE_COUNT)
		{
			ZeroMemory(m_Obstacle, sizeof(m_Obstacle));
			goto gotoCLOSE;
		}
		cData.Read(&dwMaxElemFile, sizeof(unsigned int));
		if (dwMaxElemFile > REGION_ELEM_FILE_COUNT)
		{
			cData.Read(sElemFile, sizeof(sElemFile));
			cData.Seek(sizeof(KCombinFileSection) * (dwMaxElemFile - REGION_ELEM_FILE_COUNT), FILE_CURRENT);
		}
		else
		{
			cData.Read(sElemFile, sizeof(sElemFile));
		}
		dwHeadSize = sizeof(unsigned int) + sizeof(KCombinFileSection) * dwMaxElemFile;
		//load map npc,obj,trap

		// 载入障碍数据
		cData.Seek(dwHeadSize + sElemFile[REGION_OBSTACLE_FILE_INDEX].uOffset, FILE_BEGIN);
		LoadServerObstacle(&cData, sElemFile[REGION_OBSTACLE_FILE_INDEX].uLength);

		// 载入trap信息
		cData.Seek(dwHeadSize + sElemFile[REGION_TRAP_FILE_INDEX].uOffset, FILE_BEGIN);
		LoadServerTrap(&cData, sElemFile[REGION_TRAP_FILE_INDEX].uLength);

		// 载入npc数据 
		cData.Seek(dwHeadSize + sElemFile[REGION_NPC_FILE_INDEX].uOffset, FILE_BEGIN);
		nNpcRnum=LoadServerNpc(nSubWorld, &cData, sElemFile[REGION_NPC_FILE_INDEX].uLength,pMapListInfo);

		// 载入obj数据
		cData.Seek(dwHeadSize + sElemFile[REGION_OBJ_FILE_INDEX].uOffset, FILE_BEGIN);
		LoadServerObj(nSubWorld, &cData, sElemFile[REGION_OBJ_FILE_INDEX].uLength);

gotoCLOSE:
		cData.Close();
	}
	else
	{
		KPakFile	cObstacle;

//		g_SetFilePath(szFilePath);
		snprintf(szFile, sizeof(szFile), "%s\\%03d_%s", szFilePath, nX, REGION_OBSTACLE_FILE);
		//printf("地图路径B：%s\n",szFile);
		if (cObstacle.Open(szFile))
		{
			//printf("测试7 \n");
			INT	anObstacle[REGION_GRID_WIDTH][REGION_GRID_HEIGHT];

			if (cObstacle.Size() != sizeof(this->m_Obstacle))
				ZeroMemory(m_Obstacle, sizeof(m_Obstacle));
				//m_Obstacle.Clear();
			else
			{
				cObstacle.Read((LPVOID)m_Obstacle, sizeof(m_Obstacle));
				
				/*for (INT i = 0; i < REGION_GRID_WIDTH; i++)
				{
				for (INT j = 0; j < REGION_GRID_HEIGHT; j++)
				{
				if (anObstacle[i][j])
				m_Obstacle.SetData(i, j, anObstacle[i][j]);
				}
				} */
			}

			cObstacle.Close();
		}
		else
		{
			ZeroMemory(m_Obstacle, sizeof(m_Obstacle));
			//m_Obstacle.Clear();
		}

		KPakFile	cTrapData, cNpcData;
		// 载入trap信息
		KTrapFileHead	sTrapFileHead;
		KSPTrap			sTrapCell;
		int				i, j;

//		g_SetFilePath(szFilePath);
		snprintf(szFile, sizeof(szFile), "%s\\%03d_%s", szFilePath, nX, REGION_TRAP_FILE);
		if (m_pdwTrap) { delete m_pdwTrap; m_pdwTrap = NULL; }
	m_vSpawnDefs.clear();

		if (!cTrapData.Open(szFile))
			goto TRAP_CLOSE;
		if (cTrapData.Size() < sizeof(KTrapFileHead))
			goto TRAP_CLOSE;
		cTrapData.Read(&sTrapFileHead, sizeof(KTrapFileHead));

		if (sTrapFileHead.uNumTrap > (cTrapData.Size() - sizeof(KTrapFileHead)) / sizeof(KSPTrap)
			|| sTrapFileHead.uNumTrap * sizeof(KSPTrap) + sizeof(KTrapFileHead) != cTrapData.Size())
			goto TRAP_CLOSE;

		if (sTrapFileHead.uNumTrap > 0)
		{
			m_pdwTrap = new KTrapGrid();
			memset(m_pdwTrap, 0, sizeof(KTrapGrid));
			for (i = 0; i < sTrapFileHead.uNumTrap; i++)
			{
				cTrapData.Read(&sTrapCell, sizeof(KSPTrap));
				if (sTrapCell.cY >= REGION_GRID_HEIGHT || sTrapCell.cX + sTrapCell.cNumCell - 1 >= REGION_GRID_WIDTH)
					continue;
				for (j = 0; j < sTrapCell.cNumCell; j++)
				{
					m_pdwTrap->data[sTrapCell.cX + j][sTrapCell.cY] = sTrapCell.uTrapId;
				}
			}
		}

TRAP_CLOSE:
		cTrapData.Close();

		// 载入npc数据
		KNpcFileHead	sNpcFileHead;
		KSPNpc			sNpcCell;

//		g_SetFilePath(szFilePath);
		snprintf(szFile, sizeof(szFile), "%s\\%03d_%s", szFilePath, nX, REGION_NPC_FILE_SERVER);
        //printf("地图路径D：%s\n",szFile);
		if (!cNpcData.Open(szFile))
			goto NPC_CLOSE;

		if (cNpcData.Size() < sizeof(KNpcFileHead))
			goto NPC_CLOSE;

		cNpcData.Read(&sNpcFileHead, sizeof(KNpcFileHead));

		//载入NPC数据
	/*	nNpcWnum=pMapListInfo->nNpcNum;

	if (sNpcFileHead.uNumNpc==0 && nNpcWnum >=sNpcFileHead.uNumNpc)
        nNpcRnum=0;
	else  if (sNpcFileHead.uNumNpc>0 && nNpcWnum<=sNpcFileHead.uNumNpc)
		nNpcRnum=nNpcWnum;
	else 
        nNpcRnum=0;	*/

		//nNpcWnum=pMapListInfo->nNpcNum;  //nNpcRnum=sNpcFileHead.uNumNpc;
		
		/*if (nNpcWnum<=0 && sNpcFileHead.uNumNpc<=0)
			nNpcRnum=0;
		else  if (nNpcWnum>0 && sNpcFileHead.uNumNpc>0)
			nNpcRnum=1;
		else 
		{  	
			nNpcRnum=0;
		}*/

		nNpcRnum = sNpcFileHead.uNumNpc;

		// same bound the trap loader above applies to its own count: the header states a
		// number of records, the file states how many bytes it has, and the loop reads a
		// record plus a script name per turn.  division form, so no product can wrap.
		if (sNpcFileHead.uNumNpc > (cNpcData.Size() - sizeof(KNpcFileHead))
							  / (sizeof(KSPNpc) - sizeof(sNpcCell.szScript)
							     - sizeof(sNpcCell.nNDropFile) - sizeof(sNpcCell.nGDropFile)))
			goto NPC_CLOSE;

		for (i = 0; i < sNpcFileHead.uNumNpc; i++)
		{
			cNpcData.Read(&sNpcCell, sizeof(KSPNpc) - sizeof(sNpcCell.szScript)-sizeof(sNpcCell.nNDropFile)-sizeof(sNpcCell.nGDropFile));
			if (sNpcCell.nScriptNameLen < sizeof(sNpcCell.szScript))
			{
				cNpcData.Read(sNpcCell.szScript, sNpcCell.nScriptNameLen);
				sNpcCell.szScript[sNpcCell.nScriptNameLen] = 0;
			}
			else
			{
				sNpcCell.szScript[0] = 0;
			}

			if (!strstr(sNpcCell.szScript,".lua"))
			{//如果是乱码的话 不是脚本 就清空
				ZeroMemory(sNpcCell.szScript,sizeof(sNpcCell.szScript));
			} 

			if (sNpcCell.nLevel<=0)
				sNpcCell.nLevel = 1;
			
			if (pMapListInfo->nNpcSeriesAuto)
			{
				int nRandVal = g_Random(100);
				
				sNpcCell.cSeries = 0;
				int mIsGive = -1;
				if (pMapListInfo->nNpcSeriesEarth>0)
				{
					if  (g_Random(pMapListInfo->nNpcSeriesEarth) > nRandVal)
					{
						sNpcCell.cSeries = 4;
						mIsGive = 4;
					}
				}
				
				if (pMapListInfo->nNpcSeriesFire>0)
				{
					if  (g_Random(pMapListInfo->nNpcSeriesFire) > nRandVal)
					{
						sNpcCell.cSeries = 3;
						mIsGive = 3;
					}  
					
				}
				
				if (pMapListInfo->nNpcSeriesMetal>0)
				{
					if  (g_Random(pMapListInfo->nNpcSeriesMetal) > nRandVal)
					{
						sNpcCell.cSeries = 0;
						mIsGive = 0;
					}
				}
				
				if (pMapListInfo->nNpcSeriesWater>0)
				{
					if  (g_Random(pMapListInfo->nNpcSeriesWater) > nRandVal)
					{
						sNpcCell.cSeries = 2;
						mIsGive = 2;
					}
				}
				
				if (pMapListInfo->nNpcSeriesWood>0)
				{
					if  (g_Random(pMapListInfo->nNpcSeriesWood) > nRandVal)
					{
						sNpcCell.cSeries = 1;
						mIsGive = 1;
					}
				}
				
				if  (mIsGive==-1)
					sNpcCell.cSeries = GetRandomNumber(0,4);
			}
			else
				sNpcCell.cSeries = GetRandomNumber(0,4);

			if (!strstr(sNpcCell.szScript,".lua"))
			{ //如果是乱码的话 不是脚本 就清空
			   ZeroMemory(sNpcCell.szScript,sizeof(sNpcCell.szScript));
			}  

	  //------------------------------------------
	        if (sNpcCell.shKind!=3 && (!sNpcCell.szScript[0]))
			{ //对话NPC? 并且 脚本为空 
		       ZeroMemory(sNpcCell.szScript,sizeof(sNpcCell.szScript));
		       sprintf(sNpcCell.szScript,"%s",pMapListInfo->nNewWorldScript);//脚本赋值
			}
			
			if (sNpcCell.shKind<0)
				sNpcCell.shKind = 3;

	        if (sNpcCell.shKind == 3)
			{ //是怪物 就加载爆率
		       sprintf(sNpcCell.nNDropFile,"%s",pMapListInfo->nNormalDropRate);
		       sprintf(sNpcCell.nGDropFile,"%s",pMapListInfo->nGoldenDropRate);  
			} 
	        else
			{	 
		      ZeroMemory(sNpcCell.nNDropFile,sizeof(sNpcCell.nNDropFile));
		      ZeroMemory(sNpcCell.nGDropFile,sizeof(sNpcCell.nGDropFile));
			}
			
            //NpcSet.AddMapNpc(nSubWorld, &sNpcCell);
		}		
NPC_CLOSE:
		cNpcData.Close(); 

		// 载入obj数据
		ObjSet.ServerAddRegionObj(szFilePath, nX, nY, nSubWorld);
	}

	return nNpcRnum;
}
#endif

#ifndef _SERVER
//----------------------------------------------------------------------
//	功能：载入客户端地图上本region 的 object数据（包括npc、box等）
//	如果 bLoadNpcFlag == TRUE 需要载入 clientonly npc else 不载入
//----------------------------------------------------------------------
BOOL KRegion::LoadObjectC(int nSubWorld, int nX, int nY, char *lpszPath)
{
#ifdef TOOLVERSION
	return TRUE;
#endif
	char	szPath[FILE_NAME_LENGTH], szFile[FILE_NAME_LENGTH];

	if (!lpszPath || !lpszPath[0] || strlen(lpszPath) >= FILE_NAME_LENGTH)
		return FALSE;
	sprintf(szPath, "\\%s\\v_%03d", lpszPath, nY);
//	g_SetFilePath(szPath);

	// 载入npc数组中位于本地的 client npc
	NpcSet.InsertNpcToRegion(this->m_nIndex);

	KPakFile	cData;
	snprintf(szFile, sizeof(szFile), "%s\\%03d_%s", szPath, nX, REGION_COMBIN_FILE_NAME_CLIENT);
	if (cData.Open(szFile))
	{
		DWORD	dwHeadSize;
		DWORD	dwMaxElemFile = 0;
		KCombinFileSection	sElemFile[REGION_ELEM_FILE_COUNT];

		if (cData.Size() < sizeof(DWORD) + sizeof(KCombinFileSection) * REGION_ELEM_FILE_COUNT)
			goto gotoCLOSE;
		cData.Read(&dwMaxElemFile, sizeof(DWORD));
		if (dwMaxElemFile > REGION_ELEM_FILE_COUNT)
		{
			cData.Read(sElemFile, sizeof(sElemFile));
			cData.Seek(sizeof(KCombinFileSection) * (dwMaxElemFile - REGION_ELEM_FILE_COUNT), FILE_CURRENT);
		}
		else
		{
			cData.Read(sElemFile, sizeof(sElemFile));
		}
		dwHeadSize = sizeof(DWORD) + sizeof(KCombinFileSection) * dwMaxElemFile;

		// 载入npc数据
		cData.Seek(dwHeadSize + sElemFile[REGION_NPC_FILE_INDEX].uOffset, FILE_BEGIN);
		LoadClientNpc(&cData, sElemFile[REGION_NPC_FILE_INDEX].uLength);

		// 载入obj数据
		cData.Seek(dwHeadSize + sElemFile[REGION_OBJ_FILE_INDEX].uOffset, FILE_BEGIN);
		LoadClientObj(&cData, sElemFile[REGION_OBJ_FILE_INDEX].uLength);


gotoCLOSE:
		cData.Close();
	}
	else
	{
		// 载入 Client npc 数据
		KPakFile		cNpcData;
		KNpcFileHead	sNpcFileHead;
		KSPNpc			sNpcCell;
		DWORD			i;
		KClientNpcID	sTempID;
		int				nNpcNo;

		// 载入地图文件里的 client npc
//		g_SetFilePath(szPath);
		snprintf(szFile, sizeof(szFile), "%s\\%03d_%s", szPath, nX, REGION_NPC_FILE_CLIENT);
		if (!cNpcData.Open(szFile))
			goto NPC_CLOSE;
		if (cNpcData.Size() < sizeof(KNpcFileHead))
			goto NPC_CLOSE;
		cNpcData.Read(&sNpcFileHead, sizeof(KNpcFileHead));
		for (i = 0; i < sNpcFileHead.uNumNpc; i++)
		{
			cNpcData.Read(&sNpcCell, sizeof(KSPNpc) - sizeof(sNpcCell.szScript)-sizeof(sNpcCell.nNDropFile)-sizeof(sNpcCell.nGDropFile));
			_ASSERT(sNpcCell.nScriptNameLen < sizeof(sNpcCell.szScript));
			if (sNpcCell.nScriptNameLen > 0)
			{
				cNpcData.Read(sNpcCell.szScript, sNpcCell.nScriptNameLen);
				sNpcCell.szScript[sNpcCell.nScriptNameLen] = 0;
			}
			else
			{
				sNpcCell.szScript[0] = 0;
			}
			sTempID.m_dwRegionID = MAKELONG(nX, nY);
			sTempID.m_nNo = i;
			nNpcNo = NpcSet.SearchClientID(sTempID);
			if (nNpcNo == 0)
			{
				int nIdx = NpcSet.AddClientNpc(sNpcCell.nTemplateID, nX, nY, sNpcCell.nPositionX, sNpcCell.nPositionY, i);
				if (nIdx > 0)
				{
					if (sNpcCell.shKind<0)
						sNpcCell.shKind = 3;

					Npc[nIdx].m_Kind = sNpcCell.shKind;
					Npc[nIdx].SendSerCommand(do_stand);
					Npc[nIdx].m_Dir = Npc[nIdx].GetNormalNpcStandDir(sNpcCell.nCurFrame);
				}
			} 
		}

NPC_CLOSE:
		cNpcData.Close(); 

		// 载入ClientObject数据
		ObjSet.ClientLoadRegionObj(szPath, nX, nY, nSubWorld, this->m_nIndex);
	}

	return TRUE;
}
#endif

#ifdef _SERVER
//----------------------------------------------------------------------
//	功能：载入服务器端地图上本 region 的障碍数据
//----------------------------------------------------------------------
BOOL	KRegion::LoadServerObstacle(KPakFile *pFile, DWORD dwDataSize)
{

	//INT	anObstacle[REGION_GRID_WIDTH][REGION_GRID_HEIGHT];
	//m_Obstacle.Clear();

	if (!pFile || dwDataSize != sizeof(this->m_Obstacle))
	{
		memset(m_Obstacle, 0, sizeof(m_Obstacle));
		return FALSE;
	}
	//pFile->Read((LPVOID)anObstacle, sizeof(anObstacle));
	pFile->Read((LPVOID)m_Obstacle, sizeof(m_Obstacle));
	/*for (INT i = 0; i < REGION_GRID_WIDTH; i++)
	{
	for (INT j = 0; j < REGION_GRID_HEIGHT; j++)
	{
	if (anObstacle[i][j])
	{
	m_Obstacle.SetData(i, j, anObstacle[i][j]);
	}
	}
	}
	*/

	return TRUE;
}
#endif

#ifdef _SERVER
//----------------------------------------------------------------------
//	功能：载入服务器端地图上本 region 的 trap 数据
//----------------------------------------------------------------------
BOOL	KRegion::LoadServerTrap(KPakFile *pFile, DWORD dwDataSize)
{
	if (m_pdwTrap) { delete m_pdwTrap; m_pdwTrap = NULL; }

	if (!pFile || dwDataSize < sizeof(KTrapFileHead))
		return FALSE;

	KTrapFileHead	sTrapFileHead;
	KSPTrap			sTrapCell;
	int				i, j;

	pFile->Read(&sTrapFileHead, sizeof(KTrapFileHead));
	if (sTrapFileHead.uNumTrap > (dwDataSize - sizeof(KTrapFileHead)) / sizeof(KSPTrap)
		|| sTrapFileHead.uNumTrap * sizeof(KSPTrap) + sizeof(KTrapFileHead) != dwDataSize)
		return FALSE;

	if (sTrapFileHead.uNumTrap > 0)
	{
		m_pdwTrap = new KTrapGrid();
		memset(m_pdwTrap, 0, sizeof(KTrapGrid));
		for (i = 0; i < sTrapFileHead.uNumTrap; i++)
		{
			pFile->Read(&sTrapCell, sizeof(KSPTrap));
			if (sTrapCell.cY >= REGION_GRID_HEIGHT || sTrapCell.cX + sTrapCell.cNumCell - 1 >= REGION_GRID_WIDTH)
				continue;
			for (j = 0; j < sTrapCell.cNumCell; j++)
			{
				m_pdwTrap->data[sTrapCell.cX + j][sTrapCell.cY] = sTrapCell.uTrapId;
			}
		}
	}

	return TRUE;
}
#endif

#ifdef _SERVER
//----------------------------------------------------------------------
//	功能：载入服务器端地图上本 region 的 npc 数据
//----------------------------------------------------------------------
int	KRegion::LoadServerNpc(int nSubWorld, KPakFile *pFile, DWORD dwDataSize,KMapList* pMapListInfo)
{
	if (!pFile || dwDataSize < sizeof(KNpcFileHead))
		return FALSE;

	KNpcFileHead	sNpcFileHead;
	KSPNpc			sNpcCell;
	int             nNpcWnum=0,nNpcRnum=0;

	pFile->Read(&sNpcFileHead, sizeof(KNpcFileHead));//读入结构信息

	/*    nNpcWnum=pMapListInfo->nNpcNum;  //nNpcRnum=sNpcFileHead.uNumNpc;

	if (nNpcWnum<=0 && sNpcFileHead.uNumNpc<=0)
        nNpcRnum=0;
	else  if (nNpcWnum>0 && sNpcFileHead.uNumNpc>0)
		nNpcRnum=1;
	else 
	{  	
        nNpcRnum=0;
	}*/

	// the header's count against the entry's bytes, as in the standalone loader: each
	// turn reads a fixed record and Add()s an NPC, so a count above the room is garbage
	// NPCs from whatever follows this entry in the pak.  division form - a count*record
	// product is 32 bits here and wraps, which is a different verdict from the 64-bit
	// build on the same file.  636 proved dwDataSize >= sizeof(KNpcFileHead).
	if (sNpcFileHead.uNumNpc > (dwDataSize - sizeof(KNpcFileHead))
					   / (sizeof(KSPNpc) - sizeof(sNpcCell.szScript)
					     - sizeof(sNpcCell.nNDropFile) - sizeof(sNpcCell.nGDropFile)))
		return FALSE;

	for (int i = 0; i < sNpcFileHead.uNumNpc; i++)  //nNpcRnum
	{
		pFile->Read(&sNpcCell, sizeof(KSPNpc) - sizeof(sNpcCell.szScript)-sizeof(sNpcCell.nNDropFile)-sizeof(sNpcCell.nGDropFile));  //赋值给KSPNpc结构中的szScript变量
		
		//--------------------------------------------------------	
		if (pMapListInfo->nMinLevel>0 && pMapListInfo->nMaxLevel>0 && pMapListInfo->nMaxLevel>=pMapListInfo->nMinLevel)
			sNpcCell.nLevel = GetRandomNumber(pMapListInfo->nMinLevel,pMapListInfo->nMaxLevel);
		
		if (sNpcCell.nLevel<=0)
            sNpcCell.nLevel = 1;

		if (pMapListInfo->nNpcSeriesAuto)
		{
		   int nRandVal = g_Random(100);

		   sNpcCell.cSeries = 0;
		   int mIsGive = -1;
		   if (pMapListInfo->nNpcSeriesEarth>0)
		   {
			   if  (g_Random(pMapListInfo->nNpcSeriesEarth) > nRandVal)
			   {
				   sNpcCell.cSeries = 4;
				   mIsGive = 4;
			   }
		   }
		   
		   if (pMapListInfo->nNpcSeriesFire>0)
		   {
			   if  (g_Random(pMapListInfo->nNpcSeriesFire) > nRandVal)
			   {
				   sNpcCell.cSeries = 3;
			       mIsGive = 3;
			   }  

		   }

		   if (pMapListInfo->nNpcSeriesMetal>0)
		   {
			   if  (g_Random(pMapListInfo->nNpcSeriesMetal) > nRandVal)
			   {
				   sNpcCell.cSeries = 0;
				    mIsGive = 0;
			   }
		   }

		   if (pMapListInfo->nNpcSeriesWater>0)
		   {
			   if  (g_Random(pMapListInfo->nNpcSeriesWater) > nRandVal)
			   {
				   sNpcCell.cSeries = 2;
				    mIsGive = 2;
			   }
		   }

		   if (pMapListInfo->nNpcSeriesWood>0)
		   {
			   if  (g_Random(pMapListInfo->nNpcSeriesWood) > nRandVal)
			   {
				   sNpcCell.cSeries = 1;
				    mIsGive = 1;
			   }
		   }

		   if  (mIsGive==-1)
			   sNpcCell.cSeries = GetRandomNumber(0,4);
		}
		else
			sNpcCell.cSeries = GetRandomNumber(0,4);

		//--------------------------------------------------------

		if (sNpcCell.nScriptNameLen < sizeof(sNpcCell.szScript))
		{
			pFile->Read(sNpcCell.szScript, sNpcCell.nScriptNameLen);
			sNpcCell.szScript[sNpcCell.nScriptNameLen] = 0;  //加个结束符号 防止乱码
		}
		else
		{
			sNpcCell.szScript[0] = 0;
			pFile->Seek(sNpcCell.nScriptNameLen, FILE_CURRENT);
		} 

		if (nIsOutScript)
		{//读出所有NPC脚本
			char nTrapInfo[256]={0};
			if  (sNpcCell.szScript[0])
			{
				if (nMapIndex<=0)
				{//全部
					sprintf(nTrapInfo,"图:%d,名:%s,标:%d/%d,原:%s",pMapListInfo->nSubWorldID,sNpcCell.szName,sNpcCell.nPositionX/8/32,sNpcCell.nPositionY/16/32,sNpcCell.szScript);
					g_LogFile.gTraceLogFile(nTrapInfo,strlen(nTrapInfo));
				}
				else
				{
					if  (pMapListInfo->nSubWorldID==nMapIndex)
					{//某个地图
						sprintf(nTrapInfo,"图:%d,名:%s,标:%d/%d,原:%s",pMapListInfo->nSubWorldID,sNpcCell.szName,sNpcCell.nPositionX/8/32,sNpcCell.nPositionY/16/32,sNpcCell.szScript);
						g_LogFile.gTraceLogFile(nTrapInfo,strlen(nTrapInfo));
					}
				}
			}
		}

	  if (!strstr(sNpcCell.szScript,".lua"))
	  {//如果是乱码的话 不是脚本 就清空
			ZeroMemory(sNpcCell.szScript,sizeof(sNpcCell.szScript));
	  } 

	  //------------------------------------------
	  if (sNpcCell.shKind!=3 && (!sNpcCell.szScript[0]))
	  {//对话NPC? 并且 脚本为空 
		 ZeroMemory(sNpcCell.szScript,sizeof(sNpcCell.szScript));
	     sprintf(sNpcCell.szScript,"%s",pMapListInfo->nNewWorldScript);//脚本赋值
	  }

	  if (sNpcCell.shKind<0)
		  sNpcCell.shKind = 3;

	  if (sNpcCell.shKind == 3)
	  {//是怪物 就加载爆率
		 sprintf(sNpcCell.nNDropFile,"%s",pMapListInfo->nNormalDropRate);
         sprintf(sNpcCell.nGDropFile,"%s",pMapListInfo->nGoldenDropRate);  
	  }
	  else
	  {
		 ZeroMemory(sNpcCell.nNDropFile,sizeof(sNpcCell.nNDropFile));
		 ZeroMemory(sNpcCell.nGDropFile,sizeof(sNpcCell.nGDropFile));
	  }

	  // PR1: Populate KSpawnDefinition for static/runtime separation
	  KSpawnDefinition sSpawnDef;
	  memset(&sSpawnDef, 0, sizeof(sSpawnDef));
	  sSpawnDef.uSpawnId = MakeStableSpawnId((uint32_t)pMapListInfo->nSubWorldID, (uint16_t)m_nIndex, (uint16_t)m_vSpawnDefs.size());
	  sSpawnDef.nSubWorld = nSubWorld;
	  sSpawnDef.nRegionIdx = m_nIndex;
	  sSpawnDef.nTemplateID = sNpcCell.nTemplateID;
	  sSpawnDef.nPositionX = sNpcCell.nPositionX;
	  sSpawnDef.nPositionY = sNpcCell.nPositionY;
	  sSpawnDef.nLevel = sNpcCell.nLevel;
	  sSpawnDef.shKind = sNpcCell.shKind;
	  sSpawnDef.cSeries = sNpcCell.cSeries;
	  sSpawnDef.cCamp = sNpcCell.cCamp;
	  sSpawnDef.bSpecialNpc = sNpcCell.bSpecialNpc;
	  strncpy(sSpawnDef.szName, sNpcCell.szName, sizeof(sSpawnDef.szName) - 1);
	  sSpawnDef.szName[sizeof(sSpawnDef.szName) - 1] = '\0';
	  strncpy(sSpawnDef.szScript, sNpcCell.szScript, sizeof(sSpawnDef.szScript) - 1);
	  sSpawnDef.szScript[sizeof(sSpawnDef.szScript) - 1] = '\0';
	  strncpy(sSpawnDef.nNDropFile, sNpcCell.nNDropFile, sizeof(sSpawnDef.nNDropFile) - 1);
	  sSpawnDef.nNDropFile[sizeof(sSpawnDef.nNDropFile) - 1] = '\0';
	  strncpy(sSpawnDef.nGDropFile, sNpcCell.nGDropFile, sizeof(sSpawnDef.nGDropFile) - 1);
	  sSpawnDef.nGDropFile[sizeof(sSpawnDef.nGDropFile) - 1] = '\0';
	  sSpawnDef.nRuntimeNpcIndex = 0;

	  // If flag is OFF (default), maintain exact baseline eager behavior
	  extern BOOL g_bNpcActivationEnabled;
	  if (!g_bSpawnDefinitionSplitEnabled && !g_bNpcActivationEnabled)
	  {
		  if (!strstr(pMapListInfo->nMapType, "Tong"))
		  {
			  int nSpawnedIdx = NpcSet.AddMapNpc(nSubWorld, &sNpcCell, pMapListInfo);
			  sSpawnDef.nRuntimeNpcIndex = nSpawnedIdx;
		  }
	  }
	  else
	  {
		  // Flag ON: Prewarm essential dialog/town NPCs & bosses, defer regular outdoor mobs to Region streaming
		  bool bEssential = (sNpcCell.shKind != 3) || sNpcCell.bSpecialNpc || (strstr(pMapListInfo->nMapType, "City") != NULL) || (strstr(pMapListInfo->nMapType, "Country") != NULL);
		  if (bEssential && !strstr(pMapListInfo->nMapType, "Tong"))
		  {
			  int nSpawnedIdx = NpcSet.AddMapNpc(nSubWorld, &sNpcCell, pMapListInfo);
			  sSpawnDef.nRuntimeNpcIndex = nSpawnedIdx;
		  }
	  }
	  m_vSpawnDefs.push_back(sSpawnDef);

   //   if (strstr(pMapListInfo->nMapType,"City"))   //是城市地图
	  //{    
		 // NpcSet.AddMapNpc(nSubWorld, &sNpcCell,pMapListInfo);
	  //} 
   //   else if(strstr(pMapListInfo->nMapType,"Cave"))  //山洞地图
	  //{  
   //      NpcSet.AddMapNpc(nSubWorld, &sNpcCell,pMapListInfo);	
	  //} 
   //   else if(strstr(pMapListInfo->nMapType,"Tong"))   //帮派地图
	  //{  

	  //}  
   //   else if(strstr(pMapListInfo->nMapType,"Country"))    //城镇（新手村）地图
	  //{   
   //       NpcSet.AddMapNpc(nSubWorld, &sNpcCell,pMapListInfo);          //刷对话Npc		
	  //} 
   //   else if(strstr(pMapListInfo->nMapType,"Capital"))    //首都地图
	  //{   
   //       NpcSet.AddMapNpc(nSubWorld, &sNpcCell,pMapListInfo);	
	  //}  
   //   else if(strstr(pMapListInfo->nMapType,"Field"))      //任务地图
	  //{   
   //       NpcSet.AddMapNpc(nSubWorld, &sNpcCell,pMapListInfo);          //刷对话Npc	
	  //}  
   //   else if(strstr(pMapListInfo->nMapType,"Battle"))     //战场地图
	  //{  
	
	  //}
	  //else if(strstr(pMapListInfo->nMapType,"NoNpc"))     //没有NPC
	  //{  
		 // 
	  //}
	  //else
	  //{

	  //}
	}  
    return sNpcFileHead.uNumNpc;
} 
#endif

#ifdef _SERVER
//----------------------------------------------------------------------
//	功能：载入服务器端地图上本 region 的 obj 数据
//----------------------------------------------------------------------
BOOL	KRegion::LoadServerObj(int nSubWorld, KPakFile *pFile, DWORD dwDataSize)
{
	return ObjSet.ServerLoadRegionObj(nSubWorld, pFile, dwDataSize);
}
#endif

#ifndef _SERVER
//----------------------------------------------------------------------
//	功能：载入客户端地图上本 region 的 clientonlynpc 数据
//----------------------------------------------------------------------
BOOL	KRegion::LoadClientNpc(KPakFile *pFile, DWORD dwDataSize)
{
	if (!pFile || dwDataSize < sizeof(KNpcFileHead))
		return FALSE;

	KNpcFileHead	sNpcFileHead;
	KSPNpc			sNpcCell;
	DWORD			i;
	KClientNpcID	sTempID;
	int				nNpcNo;

	pFile->Read(&sNpcFileHead, sizeof(KNpcFileHead));

	for (i = 0; i < sNpcFileHead.uNumNpc; i++)
	{
		pFile->Read(&sNpcCell, sizeof(KSPNpc) - sizeof(sNpcCell.szScript)-sizeof(sNpcCell.nNDropFile)-sizeof(sNpcCell.nGDropFile));
		if (sNpcCell.nScriptNameLen < sizeof(sNpcCell.szScript))
		{
			pFile->Read(sNpcCell.szScript, sNpcCell.nScriptNameLen);
			sNpcCell.szScript[sNpcCell.nScriptNameLen] = 0;
		}
		else
		{
			sNpcCell.szScript[0] = 0;
			pFile->Seek(sNpcCell.nScriptNameLen, FILE_CURRENT);
		}
		sTempID.m_dwRegionID = m_RegionID;
		sTempID.m_nNo = i;
		nNpcNo = NpcSet.SearchClientID(sTempID);
		if (nNpcNo == 0)
		{
			int nIdx = NpcSet.AddClientNpc(sNpcCell.nTemplateID, LOWORD(m_RegionID), HIWORD(m_RegionID), sNpcCell.nPositionX, sNpcCell.nPositionY, i);
			if (nIdx > 0)
			{
				if (sNpcCell.shKind<0)
					sNpcCell.shKind = 3;

				Npc[nIdx].m_Kind = sNpcCell.shKind;
				Npc[nIdx].SendSerCommand(do_stand);
				Npc[nIdx].m_Dir = Npc[nIdx].GetNormalNpcStandDir(sNpcCell.nCurFrame);
			}
		}
	}

	return TRUE;
}
#endif

#ifndef _SERVER
//----------------------------------------------------------------------
//	功能：载入客户端地图上本 region 的 clientonlyobj 数据
//----------------------------------------------------------------------
BOOL	KRegion::LoadClientObj(KPakFile *pFile, DWORD dwDataSize)
{
	return ObjSet.ClientLoadRegionObj(pFile, dwDataSize);
}
#endif

#ifndef _SERVER
//----------------------------------------------------------------------
//	功能：载入障碍数据给小地图	(获取这个小区域的障碍物信息)
//----------------------------------------------------------------------
void	KRegion::LoadLittleMapData(int nX, int nY, char *lpszPath, INT *lpbtObstacle)
{
	if (!lpbtObstacle)
		return;

	char  szPath[FILE_NAME_LENGTH], szFile[FILE_NAME_LENGTH];
	int	  i, j;

	if (!lpszPath || !lpszPath[0] || strlen(lpszPath) >= FILE_NAME_LENGTH)
		return ;
	sprintf(szPath, "%s\\v_%03d", lpszPath, nY);

	KPakFile	cData;
	INT		    nTempTable[REGION_GRID_WIDTH][REGION_GRID_HEIGHT];	// 地图障碍信息表
	ZeroStruct(nTempTable);
//	g_SetFilePath(szPath);
	snprintf(szFile, sizeof(szFile), "%s\\%03d_%s", szPath, nX, REGION_COMBIN_FILE_NAME_CLIENT);
//	GetFliePath("区域路径:",szFile,1000);

	if (cData.Open(szFile))
	{
		DWORD	dwHeadSize;
		DWORD	dwMaxElemFile = 0;
		KCombinFileSection	sElemFile[REGION_ELEM_FILE_COUNT];

		if (cData.Size() < sizeof(DWORD) + sizeof(KCombinFileSection) * REGION_ELEM_FILE_COUNT)
		{
			ZeroMemory(nTempTable, sizeof(nTempTable));
		}
		else
		{
			cData.Read(&dwMaxElemFile, sizeof(DWORD));
			if (dwMaxElemFile > REGION_ELEM_FILE_COUNT)
			{
				cData.Read(sElemFile, sizeof(sElemFile));
				cData.Seek(sizeof(KCombinFileSection) * (dwMaxElemFile - REGION_ELEM_FILE_COUNT), FILE_CURRENT);
			}
			else
			{
				cData.Read(sElemFile, sizeof(sElemFile));
			}

			dwHeadSize = sizeof(DWORD) + sizeof(KCombinFileSection) * dwMaxElemFile;

			if (sElemFile[REGION_OBSTACLE_FILE_INDEX].uLength >= sizeof(nTempTable))
			{//有完全障碍 或 部分障碍
				cData.Seek(dwHeadSize + sElemFile[REGION_OBSTACLE_FILE_INDEX].uOffset, FILE_BEGIN);
				cData.Read((LPVOID)nTempTable, sizeof(nTempTable));
					for (i = 0; i < REGION_GRID_HEIGHT; i++)
					{    
		              for (j = 0; j < REGION_GRID_WIDTH; j++)
					  {    
		                 if ((BYTE)nTempTable[j][i]) 
		              	   lpbtObstacle[i * REGION_GRID_WIDTH + j] = (BYTE)nTempTable[j][i];
		                 else
			               lpbtObstacle[i * REGION_GRID_WIDTH + j] = 0;
						/*char nMsg[64]={0};
						sprintf(nMsg,"编号:%d,值:%d",i * REGION_GRID_WIDTH + j,(BYTE)nTempTable[j][i]);
						GetFliePath(nMsg,"无",2000); */
					  }   
					}    
			}
			else
			{//为 整个区域都为 无障碍  -2；
				//ZeroMemory(nTempTable, sizeof(nTempTable));
				for (i = 0; i < REGION_GRID_HEIGHT; i++)
				{   
		            for (j = 0; j < REGION_GRID_WIDTH; j++)
					{   
			            lpbtObstacle[i * REGION_GRID_WIDTH + j] = -2;
					}  
				}  
			}
		}
		cData.Close();
	}
	else
	{//地图边缘 没有区域的为 -1；
		/*sprintf(szFile, "%03d_%s", nX, REGION_OBSTACLE_FILE);
		if (cData.Open(szFile))
			cData.Read((LPVOID)nTempTable, sizeof(nTempTable));
		else
			ZeroMemory(nTempTable, sizeof(nTempTable));

		cData.Close(); */
	  for (i = 0; i < REGION_GRID_HEIGHT; i++)
	  {  
		for (j = 0; j < REGION_GRID_WIDTH; j++)
		{
			  lpbtObstacle[i * REGION_GRID_WIDTH + j] = -1;
		}
	  }  
	}	
}
#endif

#ifdef _SERVER 

int KRegion::DelAllNpcWithName(char *nName,int nType)
{
	if (nName==NULL)
		return 0;

	KIndexNode *pNode = NULL;
	KIndexNode *pTmpNode = NULL;
	int	nCounter = 0;
	pNode = (KIndexNode *)m_NpcList.GetHead();
	while(pNode)
	{
		pTmpNode = (KIndexNode *)pNode->GetNext();
		
		int nNpcIdx = pNode->m_nIndex;
		if (nType==0 && nNpcIdx>0 && nNpcIdx<MAX_NPC && Npc[nNpcIdx].m_Kind!=kind_player && Npc[nNpcIdx].m_Doing != do_revive)
		{
			if (Npc[nNpcIdx].m_RegionIndex>=0 && strstr(Npc[nNpcIdx].Name,nName))
			{
				SubWorld[Npc[nNpcIdx].m_SubWorldIndex].m_Region[Npc[nNpcIdx].m_RegionIndex].RemoveNpc(nNpcIdx);
				SubWorld[Npc[nNpcIdx].m_SubWorldIndex].m_Region[Npc[nNpcIdx].m_RegionIndex].DecRef(Npc[nNpcIdx].m_MapX,Npc[nNpcIdx].m_MapY,obj_npc);
				//SubWorld[Npc[nNpcIdx].m_SubWorldIndex].m_Region[Npc[nNpcIdx].m_RegionIndex].DecNpcRef(Npc[nNpcIdx].m_MapX,Npc[nNpcIdx].m_MapY);
			}
			if (strstr(Npc[nNpcIdx].Name,nName))
			    NpcSet.Remove(nNpcIdx);

			nCounter++;
		}
		else if (nType==1 && nNpcIdx>0 && nNpcIdx<MAX_NPC && Npc[nNpcIdx].m_Kind!=kind_player && Npc[nNpcIdx].m_Doing != do_revive)
		{
			char nstrName[64]={0};
	             Npc[nNpcIdx].GetstrInfo(STR_GUISHU_NAME,nstrName,sizeof(nstrName));
			if (Npc[nNpcIdx].m_RegionIndex>=0 && strstr(nstrName,nName))
			{
				SubWorld[Npc[nNpcIdx].m_SubWorldIndex].m_Region[Npc[nNpcIdx].m_RegionIndex].RemoveNpc(nNpcIdx);
				//SubWorld[Npc[nNpcIdx].m_SubWorldIndex].m_Region[Npc[nNpcIdx].m_RegionIndex].DecNpcRef(Npc[nNpcIdx].m_MapX,Npc[nNpcIdx].m_MapY);
				SubWorld[Npc[nNpcIdx].m_SubWorldIndex].m_Region[Npc[nNpcIdx].m_RegionIndex].DecRef(Npc[nNpcIdx].m_MapX,Npc[nNpcIdx].m_MapY,obj_npc);
			}
			if (strstr(nstrName,nName))
				NpcSet.Remove(nNpcIdx);

			nCounter++;

		}

		pNode = pTmpNode;
		
	}
	return nCounter;
}


int KRegion::DelAllNpc(int mSubWorldID)
{
	KIndexNode *pNode = NULL;
	KIndexNode *pTmpNode = NULL;
	int	nCounter = 0;
	pNode = (KIndexNode *)m_NpcList.GetHead();
	while(pNode)
	{
		pTmpNode = (KIndexNode *)pNode->GetNext();

		int nNpcIdx = pNode->m_nIndex;
		if (nNpcIdx>0 && nNpcIdx<MAX_NPC && Npc[nNpcIdx].m_Kind!=kind_player && Npc[nNpcIdx].m_Doing != do_revive)
		{
			if (Npc[nNpcIdx].m_RegionIndex>=0  && mSubWorldID>0 && Npc[nNpcIdx].m_NpcSettingIdx==mSubWorldID)
			{
				   SubWorld[Npc[nNpcIdx].m_SubWorldIndex].m_Region[Npc[nNpcIdx].m_RegionIndex].RemoveNpc(nNpcIdx);
				   //SubWorld[Npc[nNpcIdx].m_SubWorldIndex].m_Region[Npc[nNpcIdx].m_RegionIndex].DecNpcRef(Npc[nNpcIdx].m_MapX,Npc[nNpcIdx].m_MapY);
				   SubWorld[Npc[nNpcIdx].m_SubWorldIndex].m_Region[Npc[nNpcIdx].m_RegionIndex].DecRef(Npc[nNpcIdx].m_MapX,Npc[nNpcIdx].m_MapY,obj_npc);
			}
			else
			{
				if (Npc[nNpcIdx].m_RegionIndex>=0)
				{
					SubWorld[Npc[nNpcIdx].m_SubWorldIndex].m_Region[Npc[nNpcIdx].m_RegionIndex].RemoveNpc(nNpcIdx);
                    //SubWorld[Npc[nNpcIdx].m_SubWorldIndex].m_Region[Npc[nNpcIdx].m_RegionIndex].DecNpcRef(Npc[nNpcIdx].m_MapX,Npc[nNpcIdx].m_MapY);
					SubWorld[Npc[nNpcIdx].m_SubWorldIndex].m_Region[Npc[nNpcIdx].m_RegionIndex].DecRef(Npc[nNpcIdx].m_MapX,Npc[nNpcIdx].m_MapY,obj_npc);
				}  
			}

			NpcSet.Remove(nNpcIdx);
		    nCounter++;
		}

		pNode = pTmpNode;

	}
  return nCounter;
}
#endif


void KRegion::showRegActivateInfo(int i)
{
	//KList		m_NpcList;							// 人物列表
	//KList		m_ObjList;							// 物件列表
	//KList		m_MissleList;						// 子弹列表
	//KList		m_PlayerList;						// 玩家列表
	printf("---区域[%d]激活[%d]地图[%d]总区[%d]NPC:%d OBJ:%d MISS:%d PLAYER:%d---\n",i,(int)IsActiveAll(),SubWorld[m_nSubMapidx].m_SubWorldID,SubWorld[m_nSubMapidx].m_nTotalRegion,m_NpcList.GetNodeCount(),m_ObjList.GetNodeCount(),m_MissleList.GetNodeCount(),m_PlayerList.GetNodeCount());
}

void KRegion::NewRegActivate(int  _minLimitLevel, int  _maxLimitLevel)
{
	KIndexNode* pNode	 = NULL;
	KIndexNode* pTmpNode = NULL;
	int 	nCounter = 0;

	if (IsActiveAll())	// 只有在附近有玩家时，Npc才处于活动状态。
	{
		pNode = (KIndexNode*)m_NpcList.GetHead();
		while (pNode)
		{// ===== NPC =====
			 pTmpNode = (KIndexNode*)pNode->GetNext();
			 int nNpcIdx = pNode->m_nIndex;
			 if (nNpcIdx>0 && nNpcIdx<MAX_NPC)
		     {
				// if ((nCounter << 1) == m_nNpcSyncCounter) 
				//if (nCounter == (m_nNpcSyncCounter>>1))
				 if ((m_nNpcSyncCounter&1))
					  Npc[nNpcIdx].NormalSync();			          // 发送同步信号

				Npc[nNpcIdx].Activate();
		     }
			 nCounter++;
			 pNode = pTmpNode;
		}
		m_nNpcSyncCounter++;
		if (m_nNpcSyncCounter > (m_NpcList.GetNodeCount()*2))
			m_nNpcSyncCounter = 0;
	}

	nCounter = 0;
	KIndexNode *pObjNode = NULL;
	KIndexNode *pObjTmpNode = NULL;
	pObjNode = (KIndexNode *)m_ObjList.GetHead();

	while(pObjNode)
	{ // ===== Object =====
		pObjTmpNode = (KIndexNode *)pObjNode->GetNext();
/*	
		if (pObjNode->m_nIndex>0 && pObjNode->m_nIndex <MAX_OBJECT)
		{
			Object[pObjNode->m_nIndex].Activate();      //物品处理的循环，如 地上
			 if (nCounter == (m_nObjSyncCounter>>1))
				 Object[pObjNode->m_nIndex].SyncState();			
		}
*/
		int idx = pObjNode->m_nIndex;
		if (idx > 0 && idx < MAX_OBJECT)
		{
			KObj* pObj = &Object[idx];
			if (pObj)
			{
				if (pObj->IsAlive())
				{
					pObj->Activate();
					if (nCounter == (m_nObjSyncCounter >> 1))
					{
						pObj->SyncState();
					}
				}
			}
		}


		++nCounter;
		pObjNode = pObjTmpNode;
	}

	++m_nObjSyncCounter;
	if (m_nObjSyncCounter > (m_ObjList.GetNodeCount()*2))
		m_nObjSyncCounter = 0;

//子弹的循环
	pNode = (KIndexNode *)m_MissleList.GetHead();
	while(pNode)
	{// ===== Missle =====
		pTmpNode = (KIndexNode *)pNode->GetNext();

		if (pNode->m_nIndex>=0 && pNode->m_nIndex<MAX_MISSLE)
			Missle[pNode->m_nIndex].Activate();		 //子弹活动函数

		pNode = pTmpNode;
	}

#ifdef _SERVER	
	pNode = (KIndexNode *)m_PlayerList.GetHead();
	while(pNode)
	{// ===== Player server =====
		pTmpNode = (KIndexNode *)pNode->GetNext();

	  if (pNode->m_nIndex>0 && pNode->m_nIndex<MAX_PLAYER)
			 Player[pNode->m_nIndex].Active(_minLimitLevel, _maxLimitLevel);

		pNode = pTmpNode;		
	}
#endif

#ifndef _SERVER
	if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_RegionIndex == m_nIndex)	// 是Player所在的Region
		Player[CLIENT_PLAYER_INDEX].Active();
#endif
}

//地图区域增加NPC节点
void KRegion::AddNpc(int nIdx)
{
//	if (nIdx > 0 && nIdx < MAX_NPC)
//	{
//		if (Npc[nIdx].m_Node.m_Ref <= 0)
//		{
//			Npc[nIdx].m_Node.m_Ref =0;
//			m_NpcList.AddTail(&Npc[nIdx].m_Node);//加入这个NPC的得节点
//			Npc[nIdx].m_Node.AddRef();
//#ifdef _SERVER
//			if (Npc[nIdx].IsActiveForever())
//			   AddNearByNpc(1);
//#endif
//		}
//	}

	if (nIdx > 0 && nIdx < MAX_NPC)
	{
		KNpc *pNpc = &Npc[nIdx];	
		_ASSERT(pNpc->m_Node.m_Ref == 0);

		if (Npc[nIdx].m_Node.m_Ref == 0)
		{
			m_NpcList.AddTail(&Npc[nIdx].m_Node);
			Npc[nIdx].m_Node.AddRef();
			// 如果是同伴LLG_MODIFY_20080516 AiMode == 9　为剑网一个特殊NPC的AI处理，坚决不能写这种代码！
			//if (Npc[nIdx].IsPartner()) //  || Npc[nIdx].m_cAi.GetAiMode() == 9)
			//	AddNearbyPlayer(1);
			// zhengyuhua: 有某些NPC需要player不在的区域仍然按自己的逻辑活动(这类NPC不能太多，否则会影响效率)
			if (Npc[nIdx].IsActiveForever())
				AddNearByNpc(1);
		}
	}
}

void KRegion::RemoveNpc(int nIdx)
{
	/*if (nIdx <= 0 || nIdx >= MAX_NPC)
	return;

	Npc[nIdx].m_Node.Remove();
	Npc[nIdx].m_Node.Release();
	Npc[nIdx].m_Node.m_Ref =0;

	if (Npc[nIdx].IsActiveForever())
	AddNearByNpc(-1);*/

	if (nIdx <= 0 || nIdx >= MAX_NPC)
		return;

	if (Npc[nIdx].m_Node.m_Ref <= 0)
	{
		_ASSERT(FALSE);
		return;
	}

	Npc[nIdx].m_Node.Remove();
	Npc[nIdx].m_Node.Release();
	// LLG_MODIFY_20080516 AiMode == 9　为剑网一个特殊NPC的AI处理，坚决不能写这种代码！
	//if (Npc[nIdx].IsPartner()) // || Npc[nIdx].m_cAi.GetAiMode() == 9)
	//	AddNearbyPlayer(-1);

	if (Npc[nIdx].IsActiveForever())
		AddNearByNpc(-1);
}

/*void KRegion::RemoveNpcwor(int nIdx)
{
	if (nIdx <= 0 || nIdx >= MAX_NPC)
		return;
	
	//_ASSERT(Npc[nIdx].m_Node.m_Ref > 0);
	
	if (Npc[nIdx].m_Node.m_Ref > 0)
	{
		Npc[nIdx].m_Node.Remove();
		Npc[nIdx].m_Node.Release();
	}
    else
      return;
}*/

void KRegion::AddMissle(int nId)
{
//	if (nId > 0 && nId < MAX_MISSLE)  //子弹限制
//	{
//		if (Missle[nId].m_Node.m_Ref <= 0)
//		{
//			Missle[nId].m_Node.m_Ref =0;
//			m_MissleList.AddTail(&Missle[nId].m_Node);
//			Missle[nId].m_Node.AddRef();
//		}
//#ifdef _SERVER
//		UpdateActiveType();
//#endif
//	}

	if (nId > 0 && nId < MAX_MISSLE)
	{
		_ASSERT(Missle[nId].m_Node.m_Ref == 0);
		if (Missle[nId].m_Node.m_Ref == 0)
		{
			m_MissleList.AddTail(&Missle[nId].m_Node);
			Missle[nId].m_Node.AddRef();
#ifdef _SERVER
			UpdateActiveType();
#endif
		}
	}
}

void KRegion::RemoveMissle(int nId)
{
	/*if (nId > 0 && nId < MAX_MISSLE)
	{
	Missle[nId].m_Node.Remove();
	Missle[nId].m_Node.Release();
	Missle[nId].m_Node.m_Ref =0;
	}*/
	if (nId > 0 && nId < MAX_MISSLE)
	{
		if (Missle[nId].m_Node.m_Ref > 0)
		{
			Missle[nId].m_Node.Remove();	// 与m_MissileList配对
			Missle[nId].m_Node.Release();	// 与AddRef配对
#ifdef _SERVER
			UpdateActiveType();
#endif
		}
	}
}

void KRegion::AddObj(int nIdx)
{

	if (nIdx <= 0 || nIdx >= MAX_OBJECT)
		return;

	UpdateActiveType();

	KIndexNode *pNode = NULL;
	pNode = new KIndexNode;
	pNode->m_nIndex = nIdx;
	m_ObjList.AddTail(pNode);

	if (Object[nIdx].m_nMapX >= 0 && Object[nIdx].m_nMapY >= 0)
		AddRef(Object[nIdx].m_nMapX,Object[nIdx].m_nMapY,obj_object);   //调节掉下物品的距离间隔
}


bool KRegion::FindNpcPosWithName(int *x,int *y,char *strName)
{
	KIndexNode* pCurNode = (KIndexNode*)m_NpcList.GetHead();

	while( pCurNode )
	{
		INT nCurNpcIdx = pCurNode->m_nIndex;
		KIndexNode* pNextNode = (KIndexNode*)pCurNode->GetNext();
		if(strstr(Npc[nCurNpcIdx].Name,strName))
		{
			int nX=0,nY=0,mapID;
			Npc[nCurNpcIdx].GetMpsPos(&nX,&nY,&mapID);

			*x =nX;
			*y =nY;
			//printf("---找到(%s)坐标:%d/%d mapIndex:%d map:%d---\n",Npc[nCurNpcIdx].Name,nX,nY,mapID,SubWorld[mapID].m_SubWorldID);
            return true;
		}

		pCurNode = pNextNode;
	}

	*x =0;
	*y =0;
	return false;
}


void KRegion::ClearAllNpc(int bOnlyDynamicAdd)
{
	KIndexNode* pCurNode = (KIndexNode*)m_NpcList.GetHead();

	while( pCurNode )
	{
		INT nCurNpcIdx = pCurNode->m_nIndex;
		KIndexNode* pNextNode = (KIndexNode*)pCurNode->GetNext();

		if((Npc[nCurNpcIdx].m_bDynamicAdd || !bOnlyDynamicAdd) && !Npc[nCurNpcIdx].IsPlayer())
		{
			RemoveNpc(nCurNpcIdx);
			//DecNpcRef(Npc[nCurNpcIdx].m_MapX, Npc[nCurNpcIdx].m_MapY);
			DecRef(Npc[nCurNpcIdx].m_MapX, Npc[nCurNpcIdx].m_MapY,obj_npc);
			NpcSet.Remove(nCurNpcIdx);
		}

		pCurNode = pNextNode;
	}
}



void KRegion::RemoveAllObj()
{
	KIndexNode *pNode = NULL;
	pNode = (KIndexNode *)m_ObjList.GetHead();
	while(pNode)
	{
		if (pNode->m_nIndex>0 && pNode->m_nIndex<MAX_OBJECT)
		{
			if (Object[pNode->m_nIndex].m_nMapX > 0 && Object[pNode->m_nIndex].m_nMapY > 0)
				DecRef(Object[pNode->m_nIndex].m_nMapX, Object[pNode->m_nIndex].m_nMapY,obj_object);

			pNode->Remove();
			delete pNode;
			pNode=NULL;
			break;
		}
		pNode = (KIndexNode *)pNode->GetNext();
	}

	UpdateActiveType();

}


void KRegion::RemoveObj(int nIdx)
{
	if (nIdx <= 0 || nIdx >= MAX_OBJECT)
		return;

	KIndexNode *pNode = NULL;

	pNode = (KIndexNode *)m_ObjList.GetHead();

	while(pNode)
	{
		if (pNode->m_nIndex == nIdx)
		{
			pNode->Remove();
			delete pNode;
			pNode=NULL;
			break;
		}
		pNode = (KIndexNode *)pNode->GetNext();
	}

	if (Object[nIdx].m_nMapX > 0 && Object[nIdx].m_nMapY > 0)
		DecRef(Object[nIdx].m_nMapX, Object[nIdx].m_nMapY,obj_object);

	UpdateActiveType();

}


void     KRegion::clearTrap()
{
#ifdef _SERVER
	if (m_pdwTrap) { delete m_pdwTrap; m_pdwTrap = NULL; }
#endif
}

void     KRegion::clearObstacle()
{
	memset(this->m_Obstacle, 0, sizeof(m_Obstacle));
}

void	KRegion::SetTrapNew(DWORD dwTrapId, int nMapX, int nMapY)
{
#ifdef _SERVER
	if (nMapX < 0 || nMapY < 0 || nMapX >= REGION_GRID_WIDTH || nMapY >= REGION_GRID_HEIGHT)
		return;
	if (!m_pdwTrap)
	{
		m_pdwTrap = new KTrapGrid();
		memset(m_pdwTrap, 0, sizeof(KTrapGrid));
	}
	m_pdwTrap->data[nMapX][nMapY] = dwTrapId;
#endif
}

BOOL KRegion::SetTrap(int nMapX, int nMapY, DWORD uTrapScriptId)
{
#ifdef _SERVER
	if (nMapX < 0 || nMapY < 0 || nMapX >= REGION_GRID_WIDTH || nMapY >= REGION_GRID_HEIGHT)
	   	return FALSE;
	if (!m_pdwTrap)
	{
		m_pdwTrap = new KTrapGrid();
		memset(m_pdwTrap, 0, sizeof(KTrapGrid));
	}
	for (int i = 0; i < 16; i++)
	{
		if (nMapX + i >= REGION_GRID_WIDTH)
			break;
		m_pdwTrap->data[nMapX + i][nMapY] = uTrapScriptId;
	}
	return TRUE;
#else
	return FALSE;
#endif
}

BOOL KRegion::SetTrap(int nMapX, int nMapY, int nCellNum, DWORD uTrapScriptId)
{
#ifdef _SERVER
	if (nMapX < 0 || nMapY < 0 || nMapX >= REGION_GRID_WIDTH || nMapY >= REGION_GRID_HEIGHT)
	   	return FALSE;
	if (!m_pdwTrap)
	{
		m_pdwTrap = new KTrapGrid();
		memset(m_pdwTrap, 0, sizeof(KTrapGrid));
	}
	for (int i = 0; i < nCellNum; i++)
	{
		if (nMapX + i >= REGION_GRID_WIDTH)
			break;
		m_pdwTrap->data[nMapX + i][nMapY] = uTrapScriptId;
	}
	return TRUE;
#else
	return FALSE;
#endif
}

DWORD KRegion::GetTrap(int nMapX, int nMapY)
{
#ifdef _SERVER
	if (nMapX < 0 || nMapY < 0 || nMapX >= REGION_GRID_WIDTH || nMapY >= REGION_GRID_HEIGHT)
		return 0;
	if (!m_pdwTrap)
		return 0;
	return m_pdwTrap->data[nMapX][nMapY];
#else
	return 0;
#endif
}

void  KRegion::setBarrier(int nMapX, int nMapY,int ver)
 {
	 if (nMapX < 0  || nMapX >= REGION_GRID_WIDTH || nMapY< 0 ||  nMapY >= REGION_GRID_HEIGHT)
		 return;

	m_Obstacle[nMapX][nMapY]=ver;

	 //m_Obstacle.SetData(nMapX, nMapY,ver);
 }
//获取障碍的类型(格子坐标)
BYTE KRegion::GetBarrier(int nMapX, int nMapY, int nDx, int nDy)
{
	long lType, lInfo;
	long lRet = 0;

	if (nMapX < 0  || nMapX >= REGION_GRID_WIDTH || nMapY< 0 ||  nMapY >= REGION_GRID_HEIGHT)
		return	Obstacle_NULL;
	
	lInfo = m_Obstacle[nMapX][nMapY];  //m_Obstacle.GetData(nMapX, nMapY);//
	lRet = (lInfo & 0x0000000f);
	lType =((lInfo >> 4) & 0x0000000f);

	switch(lType)
	{
	case Obstacle_LT: //左上
		if (nDx + nDy > 32)
			lRet = Obstacle_NULL;
		break;
	case Obstacle_RT://右上
		if (nDx < nDy)
			lRet = Obstacle_NULL;
		break;
	case Obstacle_LB://左下
		if (nDx > nDy)
			lRet = Obstacle_NULL;
		break;
	case Obstacle_RB://右下
		if (nDx + nDy < 32)
			lRet = Obstacle_NULL;
		break;
	default:
		break;
	}
	if (lRet != Obstacle_NULL)
		return lRet;

	//if (GetRef(nMapX,nMapY,obj_npc))
	//	return Obstacle_JumpFly; //是否有NPC

	return Obstacle_NULL;
}



//获取障碍的类型(格子坐标)
BYTE KRegion::GetNewBarrier(int nMapX, int nMapY, int nDx, int nDy,BOOL nIsCheckNpc)
{

   	if ( nMapX<0 || nMapX >=REGION_GRID_WIDTH ||  nMapY<0 || nMapY >=REGION_GRID_HEIGHT)
	   return Obstacle_NULL;

	long lType, lInfo;
	long lRet = 0;
	
	lInfo =m_Obstacle[nMapX][nMapY];  //16 32   m_Obstacle.GetData(nMapX, nMapY);//
	lRet = (lInfo&0x0000000f);
	lType = ((lInfo >> 4)&0x0000000f);

	switch(lType)
	{
	case Obstacle_LT: //左上
		if (nDx + nDy > 32)
			lRet = Obstacle_NULL;
		break;
	case Obstacle_RT://右上
		if (nDx < nDy)
			lRet = Obstacle_NULL;
		break;
	case Obstacle_LB://左下
		if (nDx > nDy)
			lRet = Obstacle_NULL;
		break;
	case Obstacle_RB://右下
		if (nDx + nDy < 32)
			lRet = Obstacle_NULL;
		break;
	default:
		break;
	}

	if (lRet != Obstacle_NULL)
		return lRet;

	if (nIsCheckNpc)
	{
		if (GetRef(nMapX,nMapY,obj_npc))
			return Obstacle_JumpFly; //是否有NPC
	}

	return Obstacle_NULL;
}



//----------------------------------------------------------------------------
//	功能：按 像素点坐标 * 1024 的精度判断某个位置是否障碍
//	参数：nGridX nGirdY ：本region格子坐标
//	参数：nOffX nOffY ：格子内的偏移量(像素点 * 1024 精度)
//	参数：bCheckNpc ：是否判断npc形成的障碍
//	返回值：障碍类型(if 类型 == Obstacle_NULL 无障碍)
//----------------------------------------------------------------------------
BYTE	KRegion::GetBarrierNewMin(int nGridX, int nGridY, int nOffX, int nOffY, BOOL bCheckNpc)
{

	if ( nGridX<0 || nGridX >=REGION_GRID_WIDTH ||  nGridY<0 || nGridY >=REGION_GRID_HEIGHT)
	   return Obstacle_NULL;
	  
	//long lType;
	//long lRet = Obstacle_NULL;
	//lRet = m_Obstacle[nGridX][nGridY] & 0x0000000f; //m_Obstacle.GetData(nGridX, nGridY) 
	//lType = (m_Obstacle[nGridX][nGridY]>> 4)&0x0000000f;
	long lType, lInfo;
	long lRet = Obstacle_NULL;
	lInfo =m_Obstacle[nGridX][nGridY];  //16 32
	lRet = (lInfo&0x0000000f);
	lType = ((lInfo >> 4)&0x0000000f);

	if (lRet == Obstacle_NULL)
	{
		if (bCheckNpc)
		{
			if (GetRef(nGridX,nGridY,obj_npc))
				return Obstacle_JumpFly; //是否有NPC
		}

		return Obstacle_NULL;
	}

	switch(lType)
	{
	case Obstacle_LT:
		if (nOffX + nOffY > 32 * 1024)
			return Obstacle_NULL;
		break;
	case Obstacle_RT:
		if (nOffX < nOffY)
			return Obstacle_NULL;
		break;
	case Obstacle_LB:
		if (nOffX > nOffY)
			return Obstacle_NULL;
		break;
	case Obstacle_RB:
		if (nOffX + nOffY < 32 * 1024)
			return Obstacle_NULL;
		break;
	default:
		break;
	}

	return lRet;
}






//----------------------------------------------------------------------------
//	功能：按 像素点坐标 * 1024 的精度判断某个位置是否障碍
//	参数：nGridX nGirdY ：本region格子坐标
//	参数：nOffX nOffY ：格子内的偏移量(像素点 * 1024 精度)
//	参数：bCheckNpc ：是否判断npc形成的障碍
//	返回值：障碍类型(if 类型 == Obstacle_NULL 无障碍)
//----------------------------------------------------------------------------
BYTE	KRegion::GetBarrierMin(int nGridX, int nGridY, int nOffX, int nOffY, BOOL bCheckNpc)
{
	if ( nGridX<0 || nGridX >=REGION_GRID_WIDTH ||  nGridY<0 || nGridY >=REGION_GRID_HEIGHT)
		return Obstacle_NULL;

	//long lType;
	//long lRet = Obstacle_NULL;
	//
	//lRet = m_Obstacle[nGridX][nGridY]&0x0000000f; //m_Obstacle.GetData(nGridX, nGridY)
	//lType = (m_Obstacle[nGridX][nGridY] >> 4)&0x0000000f;

	long lType, lInfo;
	long lRet = Obstacle_NULL;
	lInfo =m_Obstacle[nGridX][nGridY];  //16 32
	lRet = (lInfo&0x0000000f);
	lType = ((lInfo >> 4)&0x0000000f);

	if (lRet == Obstacle_NULL)
	{
		if (bCheckNpc)
		{
			if (GetRef(nGridX,nGridY,obj_npc))
				return Obstacle_JumpFly; //是否有NPC
		}

		return Obstacle_NULL;
	}

	switch(lType)
	{
	case Obstacle_LT:
		if (nOffX + nOffY > 32 * 1024)
			return Obstacle_NULL;
		break;
	case Obstacle_RT:
		if (nOffX < nOffY)
			return Obstacle_NULL;
		break;
	case Obstacle_LB:
		if (nOffX > nOffY)
			return Obstacle_NULL;
		break;
	case Obstacle_RB:
		if (nOffX + nOffY < 32 * 1024)
			return Obstacle_NULL;
		break;
	default:
		break;
	}
	return lRet;
}


int KRegion::GetRef(int nMapX, int nMapY, MOVE_OBJ_KIND nType)
{
	if (nMapX < 0 || nMapX >= REGION_WIDTH || nMapY < 0 || nMapY >= REGION_HEIGHT)
		return 0;

	switch(nType)
	{
	case obj_npc:
		return m_cNpcRef.GetData(nMapX, nMapY);
	case obj_object:
		return m_cObjRef.GetData(nMapX, nMapY);
	default:
		return 0;
	}
}

BOOL KRegion::AddRef(int nMapX, int nMapY, MOVE_OBJ_KIND nType)
{
	if (nMapX < 0 || nMapX >= REGION_WIDTH || nMapY < 0 || nMapY >= REGION_HEIGHT)
		return FALSE;

	switch(nType)
	{
	case obj_npc:
		return m_cNpcRef.IncData(nMapX, nMapY);
	case obj_object:
		return m_cObjRef.IncData(nMapX, nMapY);
	default:
		return FALSE;
	}
}

BOOL KRegion::DecRef(int nMapX, int nMapY, MOVE_OBJ_KIND nType)
{
	if (nMapX < 0 || nMapX >= REGION_WIDTH || nMapY < 0 || nMapY >= REGION_HEIGHT)
		return FALSE;

	switch(nType)
	{
	case obj_npc:
		return m_cNpcRef.DecData(nMapX, nMapY);
	case obj_object:
		return m_cObjRef.DecData(nMapX, nMapY);
	default:
		return FALSE;
	}
}

BOOL KRegion::AddPlayer(int nIdx)
{
//	if (nIdx > 0 && nIdx < MAX_PLAYER)
//	{
//		if (Player[nIdx].m_Node.m_Ref <= 0)
//		{  
//			Player[nIdx].m_Node.m_Ref=0;
//			m_PlayerList.AddTail(&Player[nIdx].m_Node);  //玩家列表增加一个玩家节点位置
//			Player[nIdx].m_Node.AddRef();                // 增加数量
//#ifdef _SERVER
//			AddNearbyPlayer(1); // Active计数
//#endif
//			return TRUE;
//		}
//	}
//	return FALSE;

	if (nIdx > 0 && nIdx < MAX_PLAYER)
	{
		if (Player[nIdx].m_Node.m_Ref)
		{
			ASSERT(FALSE);
		}
		else
		{
			m_PlayerList.AddTail(&Player[nIdx].m_Node);
			Player[nIdx].m_Node.AddRef();
			AddNearbyPlayer(1); // Active计数
			return TRUE;
		}
	}
	return FALSE;
}

BOOL KRegion::RemovePlayer(int nIdx)
{
	//if (nIdx > 0 && nIdx < MAX_PLAYER)
	//{
	//		Player[nIdx].m_Node.Remove();
	//		Player[nIdx].m_Node.Release();
	//		Player[nIdx].m_Node.m_Ref=0;
	//		AddNearbyPlayer(-1); // 减Active计数
	//		return TRUE;
	//}
	//return FALSE;

	if (nIdx > 0 && nIdx < MAX_PLAYER)
	{
		if (Player[nIdx].m_Node.m_Ref > 0)
		{
			Player[nIdx].m_Node.Remove();
			Player[nIdx].m_Node.Release();
			AddNearbyPlayer(-1); // 减Active计数
			return TRUE;
		}
	}
	return FALSE;
}

//-------------------------------------------------------------------------
//	功能：寻找本区域内是否有某个指定 id 的 npc
//-------------------------------------------------------------------------
int		KRegion::SearchNpc(DWORD dwNpcID)
{
	KIndexNode *pNode = NULL;
	KIndexNode *pTmpNode = NULL;

	pNode = (KIndexNode *)m_NpcList.GetHead();
	/*while(pNode)
	{
		if (Npc[pNode->m_nIndex].m_dwID == dwNpcID)
			return pNode->m_nIndex;
		pNode = (KIndexNode *)pNode->GetNext();
	}	
	*/

	while(pNode)
	{
		pTmpNode = (KIndexNode *)pNode->GetNext();
		int nNpcIdx = pNode->m_nIndex;
		if (nNpcIdx>0 && nNpcIdx<MAX_NPC && Npc[nNpcIdx].m_dwID == dwNpcID)
			return nNpcIdx;

		pNode = pTmpNode;
	}
	return 0;
}

int KRegion::FindObjectAuto(int nMapX, int nMapY,int nKind,char *nItemName,int nNpcIdx)
{
	KIndexNode *pNode = NULL;
	pNode = (KIndexNode *)m_ObjList.GetHead();
		

 if (nKind==-1)
 { 
	/* VEC_LIST::iterator it = m_ObjList.begin();
	 for(; it != m_ObjList.end(); ++it)
	 {
		 if (Object[*it].m_nMapX == nMapX && Object[*it].m_nMapY == nMapY)
		 {
			 if (Object[*it].m_AttackerDwid!=Npc[nNpcIdx].m_dwID)
			 {	 
				 if (nItemName && strstr(Object[*it].m_szName,nItemName))
					return *it;
			 } 
		 }
	 }*/

	  while(pNode)
	 {
		 if (Object[pNode->m_nIndex].m_nMapX == nMapX && Object[pNode->m_nIndex].m_nMapY == nMapY)
		 {
			 if (Object[pNode->m_nIndex].m_AttackerDwid!=Npc[nNpcIdx].m_dwID)
			 {	 
				 if (nItemName && strstr(Object[pNode->m_nIndex].m_szName,nItemName))
				     return pNode->m_nIndex;
			 }
		 }
		 pNode = (KIndexNode *)pNode->GetNext();
	 }
	 return 0;
	
 }
 else
 {
	 /*VEC_LIST::iterator it = m_ObjList.begin();
	 for(; it != m_ObjList.end(); ++it)
	 {
		 if (Object[*it].m_nMapX == nMapX && Object[*it].m_nMapY == nMapY)
		 {
			 if (nKind==1 && Object[*it].m_nKind == Obj_Kind_Money && Object[*it].m_AttackerDwid!=Npc[nNpcIdx].m_dwID)
				 return *it;
			 else if (nKind==2 && Object[*it].m_nKind == Obj_Kind_Item && Object[*it].m_AttackerDwid!=Npc[nNpcIdx].m_dwID)
			 {
				 if (Object[*it].m_nColorID==1 || Object[*it].m_nColorID==3 || Object[*it].m_nColorID==4)
					 return *it;
			 }
			 else  if (nKind==3 && Object[*it].m_AttackerDwid!=Npc[nNpcIdx].m_dwID)
			 {
				 if  (Object[*it].m_nKind == Obj_Kind_Money)
					 return *it;
				 else if (Object[*it].m_nKind == Obj_Kind_Item)
				 {
					 if (Object[*it].m_nColorID==1 || Object[*it].m_nColorID==3 || Object[*it].m_nColorID==4)
						 return *it;
				 }
			 }
			 else if (nKind==4 && nItemName && Object[*it].m_AttackerDwid!=Npc[nNpcIdx].m_dwID)
			 {//图谱
				 if (strstr(Object[*it].m_szName,nItemName))
					 return *it;
			 }
		 }
	 }
	 */
	 while(pNode)
	 {  
		if (Object[pNode->m_nIndex].m_nMapX == nMapX && Object[pNode->m_nIndex].m_nMapY == nMapY)
		{
			if (nKind==1 && Object[pNode->m_nIndex].m_nKind == Obj_Kind_Money && Object[pNode->m_nIndex].m_AttackerDwid!=Npc[nNpcIdx].m_dwID)
				return pNode->m_nIndex;
			else if (nKind==2 && Object[pNode->m_nIndex].m_nKind == Obj_Kind_Item && Object[pNode->m_nIndex].m_AttackerDwid!=Npc[nNpcIdx].m_dwID)
			{
				if (Object[pNode->m_nIndex].m_nColorID==1 || Object[pNode->m_nIndex].m_nColorID==3 || Object[pNode->m_nIndex].m_nColorID==4)
					return pNode->m_nIndex;
			}
			else  if (nKind==3 && Object[pNode->m_nIndex].m_AttackerDwid!=Npc[nNpcIdx].m_dwID)
			{
				if  (Object[pNode->m_nIndex].m_nKind == Obj_Kind_Money)
					return pNode->m_nIndex;
				else if (Object[pNode->m_nIndex].m_nKind == Obj_Kind_Item)
				{
					if (Object[pNode->m_nIndex].m_nColorID==1 || Object[pNode->m_nIndex].m_nColorID==3 || Object[pNode->m_nIndex].m_nColorID==4)
						return pNode->m_nIndex;
				}
			}
			else if (nKind==4 && nItemName && Object[pNode->m_nIndex].m_AttackerDwid!=Npc[nNpcIdx].m_dwID)
			{//图谱
				if (strstr(Object[pNode->m_nIndex].m_szName,nItemName))
					return pNode->m_nIndex;
			}	      
		}
		pNode = (KIndexNode *)pNode->GetNext();
	 }
	 return 0;

 }
  return 0;
}

int		KRegion::SearchNpcName(const char*  szName)
{
	KIndexNode *pNode = NULL;

	pNode = (KIndexNode *)m_NpcList.GetHead();
	while(pNode)
	{
		if (strcmp(Npc[pNode->m_nIndex].Name, szName) == 0)
			return pNode->m_nIndex;
		pNode = (KIndexNode *)pNode->GetNext();
	}	

	return 0;
}

int KRegion::FindObject(int nObjID)
{
	KIndexNode *pNode = NULL;
	
	pNode = (KIndexNode *)m_ObjList.GetHead();
	
	while(pNode)
	{
		if (Object[pNode->m_nIndex].m_nID == nObjID)
		{
			return pNode->m_nIndex;
		}
		pNode = (KIndexNode *)pNode->GetNext();
	}
	/*VEC_LIST::iterator it = m_ObjList.begin();
	for(; it != m_ObjList.end(); ++it)
	{
		if (Object[*it].m_nID == nObjID)
		{
			return *it;
		}
	}*/	
	return 0;
}

/*int KRegion::FindEquip(int nMapX, int nMapY)
{
	KIndexNode *pNode = NULL;
	
	pNode = (KIndexNode *)m_ObjList.GetHead();
	
	while(pNode)
	{
		if (Object[pNode->m_nIndex].m_nMapX == nMapX && Object[pNode->m_nIndex].m_nMapY == nMapY)
		{
			if (Object[pNode->m_nIndex].m_nKind == Obj_Kind_Item)
			{
				return pNode->m_nIndex;
			}
		}
		pNode = (KIndexNode *)pNode->GetNext();
	}	
	return 0;
}*/

#ifdef	_SERVER
/*void KRegion::SendSyncData(int nClient)
{
	KIndexNode *pNode = NULL;

	pNode = (KIndexNode *)m_NpcList.GetHead();
	while(pNode)
	{
		Npc[pNode->m_nIndex].SendSyncData(nClient);
		pNode = (KIndexNode *)pNode->GetNext();
	}
	pNode = (KIndexNode *)m_ObjList.GetHead();
	while(pNode)
	{
		Object[pNode->m_nIndex].SyncAdd(nClient);   //同步物品信息
		pNode = (KIndexNode *)pNode->GetNext();
	}
} */
//BroadCast
void KRegion::BroadCast(void* pBuffer, DWORD dwSize, int &nMaxCount, int nOX, int nOY)
{

	if  (NULL==pBuffer)
		return;

#define	 MAX_SYNC_RANGE	32  //同步的范围 25

	KIndexNode *pNode = NULL;
	KIndexNode* pTmpNode = NULL;
	/*pNode = (KIndexNode *)m_PlayerList.GetHead();
	while(pNode)
	{   
		if (nMaxCount<=0) break;
		   pTmpNode = (KIndexNode *)pNode->GetNext();
		if (pNode->m_nIndex>0 && pNode->m_nIndex<MAX_PLAYER)
		{
			if (Player[pNode->m_nIndex].m_nNetConnectIdx>= 0 
				&& Player[pNode->m_nIndex].m_bSleepMode==FALSE)
			{
				if (g_pServer)
					g_pServer->PackDataToClient(Player[pNode->m_nIndex].m_nNetConnectIdx, (BYTE*)pBuffer, dwSize);
			}
		}
		nMaxCount--;
		pNode = pTmpNode;		
	}*/

	pNode = (KIndexNode *)m_PlayerList.GetHead();
	while(pNode && nMaxCount > 0)
	{
	  //_ASSERT(pNode->m_nIndex > 0 && pNode->m_nIndex < MAX_PLAYER);
      if (pNode->m_nIndex > 0 && pNode->m_nIndex < MAX_PLAYER)
	  { 
		int nPlayerIndex = pNode->m_nIndex;
		int nNpcIndex = Player[nPlayerIndex].m_nIndex;

		if (nNpcIndex> 0 && nNpcIndex <MAX_NPC)
		{

		   int 	nDX = Npc[nNpcIndex].m_MapX - nOX;
		   int 	nDY = Npc[nNpcIndex].m_MapY - nOY;

			if (Player[pNode->m_nIndex].m_nNetConnectIdx >= 0 
			   /* && nDX <= MAX_SYNC_RANGE && nDY <= MAX_SYNC_RANGE
				&& (nDX * nDX + nDY * nDY) <= MAX_SYNC_RANGE * MAX_SYNC_RANGE*/
				&& Player[pNode->m_nIndex].m_bSleepMode == FALSE)
			{
				if (g_pServer)
				    g_pServer->PackDataToClient(Player[pNode->m_nIndex].m_nNetConnectIdx, (BYTE*)pBuffer, dwSize);
			}
		}
		
	  }
		nMaxCount--;
		pNode = (KIndexNode *)pNode->GetNext();
	}
}

//---------------------------------------------------------------------
// 查找该Region中NpcID为dwId的Player索引
//---------------------------------------------------------------------
int KRegion::FindPlayer(DWORD dwId)
{
	KIndexNode *pNode = NULL;
	int	nRet = -1;

	pNode = (KIndexNode *)m_PlayerList.GetHead();
	while(pNode)
	{
		if (Npc[Player[pNode->m_nIndex].m_nIndex].m_dwID == dwId)
		{
			nRet = pNode->m_nIndex;
			break;
		}
		pNode = (KIndexNode *)pNode->GetNext();
	}
	return nRet;
}

//---------------------------------------------------------------------
// 查找该Region中是否有索引为 nPlayerIdx 的玩家
//---------------------------------------------------------------------
BOOL	KRegion::CheckPlayerIn(int nPlayerIdx)
{
	if (nPlayerIdx <= 0)
		return FALSE;

	KIndexNode *pNode = NULL;
	pNode = (KIndexNode *)m_PlayerList.GetHead();
	while(pNode)
	{
		if (pNode->m_nIndex == nPlayerIdx)
			return TRUE;
		pNode = (KIndexNode *)pNode->GetNext();
	}
	return FALSE;
}

#endif
//客户端绘制 NPC 子弹 物品
#ifndef _SERVER


void   KRegion::GetFliePath(char* nName,char * nPath,int nMaxRows)
{
	KFile nFile;
	KTabFileCtrl nScirptFile;
	char nTongApplyPath[125]={0},szCol[128]={0};
	sprintf(nTongApplyPath,"%s","\\regjxpath.log");
	if (!g_FileExists(nTongApplyPath))	//是否存在
	{
		nFile.Create(nTongApplyPath);
		//sprintf(szCol,"地图\t名称\t源脚本\15\n");
		sprintf(szCol,"区域\15\n");
		nFile.Write(szCol, sizeof(szCol));
		//nFile.Save(nTongApplyPath);
		nFile.Close();
	}
	
	if (nScirptFile.Load(nTongApplyPath))
	{
		int nRows=nScirptFile.GetHeight();
		
		if  (nRows<=0)
			nRows=1;
		
		if (nRows>=nMaxRows)
		{
			
			//nScirptFile.Clear();
			//return;
			while(nScirptFile.GetHeight()>0)
			{ 
				nScirptFile.Remove(nScirptFile.GetHeight());
				nScirptFile.Save(nTongApplyPath);
			}
			
			nRows=nScirptFile.GetHeight();
			
			if  (nRows<=0)
				nRows=1;
			
		}
		
		snprintf(szCol, sizeof(szCol),"--地图%s:(%s)--",nName,nPath);
		nScirptFile.InsertAfter(nRows);
		nScirptFile.WriteString(nRows,1, szCol);                                    //行号
		//nTong.WriteString(nRows,2,Npc[Player[m_nPlayerIndex].m_nIndex].Name);     //名字
		//nTong.WriteInteger(nRows,3,Npc[Player[m_nPlayerIndex].m_nIndex].m_Level); //等级
		nScirptFile.Save(nTongApplyPath);	
		//nFile.Write(szCol, sizeof(szCol));
	}
	nScirptFile.Clear();
}

void KRegion::Paint()
{
	KIndexNode *pNode = NULL;

	pNode = (KIndexNode *)m_NpcList.GetHead();
	while(pNode)
	{
		Npc[pNode->m_nIndex].Paint();     //绘画NPC 外观
		pNode = (KIndexNode *)pNode->GetNext();
	}
/*
	pNode = (KIndexNode *)m_MissleList.GetHead();
	while(pNode)
	{
		Missle[pNode->m_nIndex].Paint();  //绘画 子弹
		pNode = (KIndexNode *)pNode->GetNext();
	}

	pNode = (KIndexNode *)m_ObjList.GetHead();
	while(pNode)
	{
//		Object[pNode->m_nIndex].Paint();		need add -spe
		pNode = (KIndexNode *)pNode->GetNext();
	}*/
}
#endif

//KRegion::VEC_LIST::iterator KRegion::GetObjNode(int nIdx)
void * KRegion::GetObjNode(int nIdx)
{
	KIndexNode *pNode = NULL;

	pNode = (KIndexNode *)m_ObjList.GetHead();

	while(pNode)
	{
		if (pNode->m_nIndex == nIdx)
		{
			break;
		}
		pNode = (KIndexNode *)pNode->GetNext();
	}
	return pNode;

	/*VEC_LIST::iterator it = m_ObjList.begin();
	for(; it != m_ObjList.end(); ++it)
	{
		if (*it == nIdx)
			break;
	}
	return it;*/
}

void KRegion::Close()		// 清除Region中的几个链表（所指向的内容没有被清除）
{
	KIndexNode* pNode = NULL;
	KIndexNode* pTempNode = NULL;

	if (!m_nWidth || !m_nHeight)
		return;

	if (m_pNpcRef)
	{
		delete [] m_pNpcRef;
		m_pNpcRef = NULL;
	}
	if (m_pObjRef)
	{
		delete [] m_pObjRef;
		m_pObjRef = NULL;
	}
	//if (m_pObstacleRef)
	//	ZeroMemory(m_pObstacleRef, m_nWidth * m_nHeight);
	//if (m_pMslRef)
	//	ZeroMemory(m_pMslRef, m_nWidth * m_nHeight);


	m_cNpcRef.Clear();
	m_cObjRef.Clear();

#ifdef _SERVER
	//m_Obstacle.Clear();
	//m_dwTrap.Clear();
	ZeroMemory(m_Obstacle, sizeof(m_Obstacle));
	if (m_pdwTrap) { delete m_pdwTrap; m_pdwTrap = NULL; }
#endif

	pNode = (KIndexNode *)m_NpcList.GetHead();

	while(pNode)
	{
		pTempNode = pNode;
		pNode = (KIndexNode *)pNode->GetNext();
#ifdef _SERVER
		if  (pTempNode->m_nIndex>0 && pTempNode->m_nIndex <MAX_NPC)
		{
#else
	    if  (pTempNode->m_nIndex>0)
		{
#endif
			RemoveNpc(pTempNode->m_nIndex);	
			//NpcSet.Remove(pTempNode->m_nIndex,FALSE);	   //原来是注销掉的 不需要同步到客户端	
			Npc[pTempNode->m_nIndex].m_RegionIndex = -1;
		}

/*#ifdef _SERVER
		if (pTempNode)
		{
			delete pTempNode;   //原来是注销掉的
			pTempNode=NULL;
		}
#endif*/

	}
	KIndexNode* pMissNode = NULL;
	KIndexNode* pMissTempNode = NULL;

	pMissNode = (KIndexNode *)m_MissleList.GetHead();

	while(pMissNode)
	{
		pMissTempNode = pMissNode;
		pMissNode = (KIndexNode *)pMissNode->GetNext();
#ifdef _SERVER
        if (pMissTempNode->m_nIndex>0 && pMissTempNode->m_nIndex < MAX_MISSLE)
		{
#else
		if (pMissTempNode->m_nIndex>0)
		{
#endif
			MissleSet.Remove(pMissTempNode->m_nIndex);
			Missle[pMissTempNode->m_nIndex].m_nRegionId = -1;
		}

		pMissTempNode->Remove();
		pMissTempNode->Release();

/*#ifdef _SERVER
		if (pMissTempNode)
		{
		  delete pMissTempNode;		 	    //原来是注销掉的
		  pMissTempNode=NULL;				//原来是注销掉的
		}
#endif*/
	}

	// 同时清除 clientonly 类型的 obj ---- zroc add
/*	VEC_LIST::iterator it = m_ObjList.begin();
	for(; it != m_ObjList.end(); ++it)
	{
#ifndef _SERVER
		//Object[*it].Remove(FALSE);
		Object[*it].Remove(FALSE,TRUE);
#endif
		 ObjSet.Remove(*it); 
		//g_cObjMgr.Remove(*it);        // TODO:
		Object[*it].m_nRegionIdx = -1;
	}
	m_ObjList.clear();*/

	KIndexNode* pObjNode = NULL;
	KIndexNode* pObjTempNode = NULL;

	pObjNode = (KIndexNode *)m_ObjList.GetHead();
	while(pObjNode)
	{
		pObjTempNode = pObjNode;
		pObjNode = (KIndexNode *)pObjNode->GetNext();
#ifdef _SERVER
	  if (pObjTempNode->m_nIndex>0 && pObjTempNode->m_nIndex < MAX_OBJECT)
	  {
#else
	  if (pObjTempNode->m_nIndex>0)
	  {
#endif

#ifndef _SERVER
		  //ObjSet.RemoveIfClientOnly(pObjTempNode->m_nIndex);	//原来是注销掉的
		  //ObjSet.Remove(pObjTempNode->m_nIndex);              //原来是注销掉的
		  Object[pObjTempNode->m_nIndex].Remove(FALSE,TRUE);
		
#endif 
          ObjSet.Remove(pObjTempNode->m_nIndex);              // TODO:
		  Object[pObjTempNode->m_nIndex].m_nRegionIdx = -1;
	  }

		pObjTempNode->Remove();

		if (pObjTempNode)
		{
			delete pObjTempNode;
			pObjTempNode=NULL;
		}
	}

	KIndexNode* ppNode = NULL;
	KIndexNode* ppTempNode = NULL;

	ppNode = (KIndexNode *)m_PlayerList.GetHead();
	while(ppNode)
	{
		ppTempNode = ppNode;
		ppNode = (KIndexNode *)ppNode->GetNext();

		ppTempNode->Remove();
		ppTempNode->Release();

		/*if (ppTempNode)
		{
		  delete ppTempNode;	                    //原来是注销掉的
		  ppTempNode=NULL;
		}*/
	}

	m_RegionID	= -1;
	m_nIndex	= -1;
	m_nActive	= 0;
	memset(m_nConnectRegion, -1, sizeof(m_nConnectRegion));
	memset(m_nConRegionID, -1, sizeof(m_nConRegionID));

#ifdef _SERVER
	UpdateActiveType();
	_ASSERT(m_eActType == emACT_TYPE_NONE);
#endif
}

//后来增加的
#ifdef _SERVER

BOOL KRegion::AddNearbyPlayer(INT nAdd, BOOL bConRegion /* = TRUE */)
{
	m_nNearbyPlayerCount	+= nAdd;

	// 临时加的，查为什么m_nNearbyPlayerCount会小于0
	// 问题很复杂，先做临时修复操作咯
	if (m_nNearbyPlayerCount < 0)// 临时修复此bug
		m_nNearbyActiveNpcCount = 0;

	if (bConRegion)
	{
		for (INT i = 0; i < 8; i++)
		{
			if (m_pConRegion[i])
				m_pConRegion[i]->AddNearbyPlayer(nAdd, FALSE);
		}
	}

	return UpdateActiveType();
}

BOOL KRegion::AddNearByNpc(INT nAdd, BOOL bConRegion /* = TRUE*/)
{
	m_nNearbyActiveNpcCount += nAdd;
	// 临时加的，查为什么m_nNearbyPlayerCount会小于0
	// 问题很复杂，先做临时修复操作咯
    if (m_nNearbyPlayerCount < 0)// 临时修复此bug
		m_nNearbyActiveNpcCount = 0;

	if (bConRegion)
	{//
		for (INT i = 0; i < 8; i++)
		{
			if (m_pConRegion[i])
				m_pConRegion[i]->AddNearByNpc(nAdd, FALSE);
		}
	}

	return UpdateActiveType();
}

BOOL KRegion::UpdateActiveType()
{
	BOOL bResult			= FALSE;

	_KEACT_TYPE eActType	= emACT_TYPE_NONE;

	if (m_nNearbyPlayerCount > 0 || m_nNearbyActiveNpcCount > 0)
	{//如果有人  肯定是激活
		eActType	= emACT_TYPE_ALL;
		//printf("---地图NPC区域被激活------\n");
	}
	else if (!m_ObjList.IsEmpty() || !m_MissleList.IsEmpty())  
	{//有物品 也是激活的
		eActType	= emACT_TYPE_WAIT;
		//printf("---地图%d子弹技能%d被激活------\n",m_nSubMapidx,m_nIndex);
	}
	else//否则就不激活
		eActType	= emACT_TYPE_NONE;

	//KG_PROCESS_SUCCESS(eActType == m_eActType);
	if (eActType == m_eActType)
		goto Exit1;

	if (eActType == emACT_TYPE_ALL)
	{//附近有NPC 就激活 
		// 刚开始完全活动
		// TODO:liuchang 需要修改NpcAi
	}

	if (m_eActType == emACT_TYPE_NONE)
	{   //是没有激活状态的 就增加
		// 刚开始有活动
		SubWorld[m_nSubMapidx].AddActiveRegion(m_nIndex);
		extern BOOL g_bNpcActivationEnabled;
		if (g_bNpcActivationEnabled)
		{
			ActivateRegionNpcs(m_nSubMapidx);
		}
		//printf("---地图%d区域%d被激活------\n",m_nSubMapidx,m_nIndex);
	}
	else if (eActType == emACT_TYPE_NONE)
	{
		// 刚开始完全无活动
		SubWorld[m_nSubMapidx].RemoveActiveRegion(m_nIndex);
		//printf("---地图%d区域%d被删除------\n",m_nSubMapidx,m_nIndex);
	}

	m_eActType = eActType;

Exit1:
	bResult	= TRUE;
	//Exit0:
	return bResult;
}

VOID KRegion::ActivateRegionNpcs(int nSubWorld)
{
	extern BOOL g_bNpcActivationEnabled;
	if (!g_bNpcActivationEnabled)
		return;

	if (m_vSpawnDefs.empty())
		return;

	if (nSubWorld < 0 || nSubWorld >= MAX_SUBWORLD)
		return;

	int nMapId = SubWorld[nSubWorld].m_SubWorldID;
	if (nMapId < 0)
		return;

	extern KIniFile g_NpcMapDropRate;

	KMapList MapList;
	memset(&MapList, 0, sizeof(MapList));
	MapList.nSubWorldID = nMapId;
	char strMapKeyName[64] = {0};

	sprintf(strMapKeyName, "%d_NpcSeriesAuto", nMapId);
	g_NpcMapDropRate.GetInteger("List", strMapKeyName, 0, &MapList.nNpcSeriesAuto);
	sprintf(strMapKeyName, "%d_NpcSeriesWood", nMapId);
	g_NpcMapDropRate.GetInteger("List", strMapKeyName, 0, &MapList.nNpcSeriesWood);
	sprintf(strMapKeyName, "%d_NpcSeriesMetal", nMapId);
	g_NpcMapDropRate.GetInteger("List", strMapKeyName, 0, &MapList.nNpcSeriesMetal);
	sprintf(strMapKeyName, "%d_NpcSeriesWater", nMapId);
	g_NpcMapDropRate.GetInteger("List", strMapKeyName, 0, &MapList.nNpcSeriesWater);
	sprintf(strMapKeyName, "%d_NpcSeriesFire", nMapId);
	g_NpcMapDropRate.GetInteger("List", strMapKeyName, 0, &MapList.nNpcSeriesFire);
	sprintf(strMapKeyName, "%d_NpcSeriesEarth", nMapId);
	g_NpcMapDropRate.GetInteger("List", strMapKeyName, 0, &MapList.nNpcSeriesEarth);
	sprintf(strMapKeyName, "%d_AutoGoldenNpc", nMapId);
	g_NpcMapDropRate.GetInteger("List", strMapKeyName, 0, &MapList.nAutoGoldenNpc);
	sprintf(strMapKeyName, "%d_GoldenType", nMapId);
	g_NpcMapDropRate.GetInteger("List", strMapKeyName, 0, &MapList.nGoldenType);
	sprintf(strMapKeyName, "%d_NpcDefense", nMapId);
	g_NpcMapDropRate.GetInteger("List", strMapKeyName, 0, &MapList.nNpcDefense);
	sprintf(strMapKeyName, "%d_NpcMaxLife", nMapId);
	g_NpcMapDropRate.GetInteger("List", strMapKeyName, 0, &MapList.nNpcMaxLife);
	sprintf(strMapKeyName, "%d_NpcExeSale", nMapId);
	g_NpcMapDropRate.GetInteger("List", strMapKeyName, 0, &MapList.nExpSale);
	sprintf(strMapKeyName, "%d_MapType", nMapId);
	g_NpcMapDropRate.GetString("List", strMapKeyName, "NoNpc", MapList.nMapType, sizeof(MapList.nMapType));
	sprintf(strMapKeyName, "%d_NpcAutoLevelMin", nMapId);
	g_NpcMapDropRate.GetInteger("List", strMapKeyName, 0, &MapList.nMinLevel);
	sprintf(strMapKeyName, "%d_NpcAutoLevelMax", nMapId);
	g_NpcMapDropRate.GetInteger("List", strMapKeyName, 0, &MapList.nMaxLevel);

	for (size_t i = 0; i < m_vSpawnDefs.size(); ++i)
	{
		KSpawnDefinition& def = m_vSpawnDefs[i];
		if (def.nRuntimeNpcIndex > 0)
			continue;

		if (def.bIsDead)
		{
			if (SubWorld[nSubWorld].m_dwCurrentTime < def.dwNextRespawnTick)
				continue;
			def.bIsDead = false;
		}

		KSPNpc sNpcCell;
		memset(&sNpcCell, 0, sizeof(sNpcCell));
		sNpcCell.nTemplateID = def.nTemplateID;
		sNpcCell.nPositionX = def.nPositionX;
		sNpcCell.nPositionY = def.nPositionY;
		sNpcCell.nLevel = def.nLevel;
		sNpcCell.shKind = def.shKind;
		sNpcCell.cSeries = def.cSeries;
		sNpcCell.cCamp = def.cCamp;
		sNpcCell.bSpecialNpc = def.bSpecialNpc;
		strncpy(sNpcCell.szName, def.szName, sizeof(sNpcCell.szName) - 1);
		sNpcCell.szName[sizeof(sNpcCell.szName) - 1] = '\0';
		strncpy(sNpcCell.szScript, def.szScript, sizeof(sNpcCell.szScript) - 1);
		sNpcCell.szScript[sizeof(sNpcCell.szScript) - 1] = '\0';
		strncpy(sNpcCell.nNDropFile, def.nNDropFile, sizeof(sNpcCell.nNDropFile) - 1);
		sNpcCell.nNDropFile[sizeof(sNpcCell.nNDropFile) - 1] = '\0';
		strncpy(sNpcCell.nGDropFile, def.nGDropFile, sizeof(sNpcCell.nGDropFile) - 1);
		sNpcCell.nGDropFile[sizeof(sNpcCell.nGDropFile) - 1] = '\0';

		int nSpawnedIdx = NpcSet.AddMapNpc(nSubWorld, &sNpcCell, &MapList);
		if (nSpawnedIdx > 0)
		{
			def.nRuntimeNpcIndex = nSpawnedIdx;
			if (def.nCurLife > 0 && def.nCurLife < Npc[nSpawnedIdx].m_CurrentLifeMax)
			{
				Npc[nSpawnedIdx].m_CurrentLife = def.nCurLife;
			}
		}
	}
}
#endif

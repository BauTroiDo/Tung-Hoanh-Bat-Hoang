// DBDumpLoad.cpp: implementation of the CDBDump class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <io.h>
#include "DBDumpLoad.h"

extern HWND hListOutput;
extern HWND mainHwnd;
////////CDBDump////////////////////////////////////////////////////////////////////////////
CDBDump::CDBDump()
{
	memset(m_FilePath,0,MAX_PATH);
	m_output = NULL;
	m_IsOpened = false;
	m_bRollbackFailed = false;
}
bool CDBDump::Open(char* aFilePath)
{//打开包文件
	if(m_IsOpened)return false;

	_finddata_t FindData;
	// handle type is auto because UCRT returns it as intptr_t: storing it in a 32-bit long
	// destroys the pointer and the next _findnext writes through the truncated value
	auto aFileFound =_findfirst(aFilePath, &FindData);
	if(aFileFound != -1)return false;	//如果该文件存在就返回不成功

	m_output = fopen(aFilePath, "wb");
	if(!m_output)return false;			//如果打开该文件失败就返回不成功

	strcpy(m_FilePath,aFilePath);
	m_IsOpened = true;
	m_bRollbackFailed = false;
	return true;
}

bool CDBDump::Close()
{//保存包文件
	if(!m_IsOpened) 
		return false;

	if(fclose(m_output)) 
		return false;
	
	memset(m_FilePath,0,MAX_PATH);
	m_output = NULL;
	m_IsOpened = false;
	return true;
}

bool CDBDump::AddData(char* key, const UINT keysize, char *aData, const UINT size)
{//添加文件
	if(!m_IsOpened) return false;
	// once a record could not be erased the file has a torn record at its end, and every
	// later append would be read back as though it followed a whole one - the loader stops
	// at the first record it cannot honour, so everything behind the tear would go missing
	// from the backup without a trace.
	if(m_bRollbackFailed) return false;
	// this is the anchor the four failure branches below rewind to, so a count the runtime
	// could not produce has to stop the record instead of starting one that cannot be
	// undone: in an `unsigned int' the -1 becomes 4294967295, an offset no rewind reaches,
	// and the -1 is also what a backup past the 2 GiB a 4-byte `long' counts answers with.
	long long offset = _ftelli64(m_output);
	if(offset < 0) return false;

	if(fwrite(&keysize, sizeof(UINT), 1, m_output) != 1)
	{
		// the rollback's own failure is the thing that matters here: `fseek' leaves the file
		// where it was and says so with -1, and that notice was discarded.
		if (0 != _fseeki64(m_output, offset, SEEK_SET))
			m_bRollbackFailed = true;
		return false;
	}
	if(fwrite(key, 1, keysize, m_output) != keysize)
	{
		if (0 != _fseeki64(m_output, offset, SEEK_SET))
			m_bRollbackFailed = true;
		return false;
	}

	if(fwrite(&size, sizeof(UINT), 1, m_output) != 1)
	{
		if (0 != _fseeki64(m_output, offset, SEEK_SET))
			m_bRollbackFailed = true;
		return false;
	}

	if(fwrite(aData, 1, size, m_output) != size)
	{
		if (0 != _fseeki64(m_output, offset, SEEK_SET))
			m_bRollbackFailed = true;
		return false;
	}

	return true;
}

////////CDBLoad////////////////////////////////////////////////////////////////////////////
CDBLoad::CDBLoad()
{
	memset(m_FilePath,0,MAX_PATH);
	m_output = NULL;
	m_IsOpened = false;
}
bool CDBLoad::Open(char* aFilePath)
{//打开包文件
	if(m_IsOpened) return false;
	_finddata_t FindData;
	// handle type is auto because UCRT returns it as intptr_t: storing it in a 32-bit long
	// destroys the pointer and the next _findnext writes through the truncated value
	auto aFileFound =_findfirst(aFilePath, &FindData);
	if(aFileFound == -1)return false;	//如果该文件不存在就返回不成功

	m_output = fopen(aFilePath, "rb");//rb
	if(!m_output)return false;			//如果打开该文件失败就返回不成功

	strcpy(m_FilePath,aFilePath);
	m_IsOpened = true;
	return true;
}

bool CDBLoad::Close()
{//保存包文件
	if(!m_IsOpened) return false;

	if(fclose(m_output)) return false;
	
	memset(m_FilePath,0,MAX_PATH);
	m_output = NULL;
	m_IsOpened = false;
	return true;
}
//读取文件
bool CDBLoad::ReadData(char* key, UINT keyroom, UINT& keysize, char *aData, UINT dataroom, UINT& size)
{
	if(!m_IsOpened) 
		return false;

	if(fread(&keysize, sizeof(UINT), 1, m_output) != 1)
		return false;
	
	// keysize is the length this backup file states for the record's key and key is the
	// caller's array, so the only thing that can say where that array ends is the room its
	// owner hands in below.  A claim at or over it ends the read here instead of walking
	// past the end of somebody else's stack frame, and a key exactly as long as the room is
	// refused by the same test because it would leave no byte for the terminator the
	// callers hand to strlen and strcmp.
	if (keysize >= keyroom)
		return false;

	if(fread(key, 1, keysize, m_output) != keysize)
		return false;
	key[keysize] = 0;

	if(fread(&size, sizeof(UINT), 1, m_output) != 1)
		return false;

	// the record's body is the file's second claim and the room behind aData is the
	// caller's: a body larger than the buffer it was asked to fill is refused whole,
	// because nothing in this function can make the file shorter than its own header says
	// it is.
	if (size > dataroom)
		return false;

	if(fread(aData, 1, size, m_output) != size)
		return false;

	return true;
}

bool CDBLoad::GotoHead()
{//把文件指针移动到文件头
	if(!m_IsOpened) return false;
	if(!m_output)
		return false;
	// the two scans that call this read forward from wherever the stream is left, so a
	// rewind that did not happen has to stop them instead of starting them inside a
	// record - a length word read out of the middle of a body is arbitrary data.
	return 0 == _fseeki64(m_output, 0, SEEK_SET);
}

void CDBLoad::copyData(ZDBTable *dbTable)
{
	if (!dbTable) return;

	if (!GotoHead())
	{
		return;
	}
	char aBuffer[128*1024] = {0};      //64  角色数据
	BYTE m_GaemSaveBuffer[128*1024] = {0}; //目标角色数据
	char aKeyBuffer[sizeof(TRoleBaseInfo::caccname) + 1] = {0};            //关键字 是角色名 或 账号名
	UINT asize,akeysize;
	char aStr[128];
	// the buffers above are the room ReadData is told about: a record the backup file
	// claims is bigger than either of them is refused rather than read into them.
	while(ReadData(aKeyBuffer, sizeof(aKeyBuffer), akeysize, aBuffer, sizeof(aBuffer), asize))
	{//读到得数据

		if  (aKeyBuffer[0])
		{
			/*TRoleData* pRoleData_ = (TRoleData*)aBuffer;
			sprintf(aStr,"[找到角色]--角色:%s 帐号:%s,等级:%d len:%u 物品数量:%d--成功!",
			aKeyBuffer,
			pRoleData_->BaseInfo.caccname,
			pRoleData_->BaseInfo.ifightlevel,pRoleData_->dwDataLen,pRoleData_->nItemCount);
			AddOutputString_db(hListOutput,aStr);*/
	
			ZeroMemory(m_GaemSaveBuffer, sizeof(m_GaemSaveBuffer));
			TRoleData* pData = (TRoleData *)m_GaemSaveBuffer;
			// both halves of the restore are told how big the other one is: the record below is
			// the file's own bytes and `pData' is the buffer being rebuilt.
			if (UpdateDBPlayerDB((BYTE *)pData, sizeof(m_GaemSaveBuffer), aBuffer, sizeof(aBuffer)) == 1)
			{
				TRoleData* pRoleData = (TRoleData*)m_GaemSaveBuffer;
				if (dbTable->add((const char *)aKeyBuffer,strlen(aKeyBuffer)+1,(const char *)m_GaemSaveBuffer,pRoleData->dwDataLen))
				{  
					snprintf(aStr, sizeof(aStr),"[角色恢复]--角色:%s 帐号:%s,等级:%d len:%u 物品数量:%d--成功!",
						aKeyBuffer,
						pRoleData->BaseInfo.caccname,
						pRoleData->BaseInfo.ifightlevel,pRoleData->dwDataLen,pRoleData->nItemCount);
					AddOutputString_db(hListOutput,aStr);
				}
			}
		
		}
		
		/*if(strcmp(key, aKeyBuffer)==0)
		{
		memcpy(aData,aBuffer,asize);
		size = asize;
		return true;
		}*/
	}
	return;
}


int CDBLoad::UpdateDBPlayerDB(BYTE* pPlayerInfo, UINT room, char *bakRoleData, UINT bakroom)
{
	//注意，存档取档都必须按照P顺序
	if (!pPlayerInfo)
		return -1;

	//数据库中玩家基本信息表
	SavePlayerBaseInfoDB(pPlayerInfo,bakRoleData);   //1	基本

	// 玩家的战斗技能列表
	if (SavePlayerFightSkillListDB(pPlayerInfo, room, bakRoleData, bakroom) != 1)   //2 fight
		return -1;

	// 玩家的生活技能列表
	if (SavePlayerLifeSkilllListDB(pPlayerInfo, room, bakRoleData, bakroom) != 1)   //3 life
		return -1;

	// 玩家的任务列表
	if (SavePlayerTaskListDB(pPlayerInfo, room, bakRoleData, bakroom) != 1)   //4 task
		return -1;

	// 玩家的物品列表
	if (SavePlayerItemListDB(pPlayerInfo, room, bakRoleData, bakroom) != 1)   //5 item
		return -1;

	//玩家的好友列表//
	if (SavePlayerFriendListDB(pPlayerInfo, room, bakRoleData, bakroom) != 1)   //6 friend
		return -1;

	return 1;
}

//保存玩家的基本信息
int	 CDBLoad::SavePlayerBaseInfoDB(BYTE * pRoleBuffer,char *bakRoleData)
{
	_ASSERT(pRoleBuffer);

	TRoleData * pRoleData = (TRoleData*)pRoleBuffer;

	TRoleData * pbakRoleData = (TRoleData*)bakRoleData;
	//玩家信息
	memset(pRoleData, 0, sizeof(TRoleData));

	//pRoleData=pbakRoleData;

	pRoleData->bBaseNeedUpdate = 1;
	snprintf(pRoleData->BaseInfo.szName, sizeof(pRoleData->BaseInfo.szName), "%s", pbakRoleData->BaseInfo.szName);
	strcpy(pRoleData->BaseInfo.szchenhao,pbakRoleData->BaseInfo.szchenhao);
	strcpy(pRoleData->BaseInfo.caccname, pbakRoleData->BaseInfo.caccname);

	pRoleData->BaseInfo.nForbiddenFlag =pbakRoleData->BaseInfo.nForbiddenFlag;  //禁止发言？
	pRoleData->BaseInfo.ileftprop      =pbakRoleData->BaseInfo.ileftprop;
	pRoleData->BaseInfo.ileftfight     = pbakRoleData->BaseInfo.ileftfight;
	pRoleData->BaseInfo.ipower         = pbakRoleData->BaseInfo.ipower;
	pRoleData->BaseInfo.iagility       = pbakRoleData->BaseInfo.iagility;
	pRoleData->BaseInfo.iouter         = pbakRoleData->BaseInfo.iouter ;
	pRoleData->BaseInfo.iinside	    = pbakRoleData->BaseInfo.iinside;
	pRoleData->BaseInfo.iluck          = pbakRoleData->BaseInfo.iluck;
	pRoleData->BaseInfo.dwTongID       = pbakRoleData->BaseInfo.dwTongID ;   //帮会名称
	//玩家等级信息
	pRoleData->BaseInfo.fightexp    =pbakRoleData->BaseInfo.fightexp ;
	pRoleData->BaseInfo.ifightlevel = pbakRoleData->BaseInfo.ifightlevel;
	
	pRoleData->BaseInfo.ileadlevel  = pbakRoleData->BaseInfo.ileadlevel;
	pRoleData->BaseInfo.ileadexp    =	pbakRoleData->BaseInfo.ileadexp ;
	//门派信息
	pRoleData->BaseInfo.nSect       = pbakRoleData->BaseInfo.nSect;
	pRoleData->BaseInfo.nFirstSect  = pbakRoleData->BaseInfo.nFirstSect;
	pRoleData->BaseInfo.ijoincount  =pbakRoleData->BaseInfo.ijoincount;
	pRoleData->BaseInfo.isectrole   = pbakRoleData->BaseInfo.isectrole;
	pRoleData->BaseInfo.irankrole   =pbakRoleData->BaseInfo.irankrole ;
	pRoleData->BaseInfo.iPltitle    = pbakRoleData->BaseInfo.iPltitle;
	pRoleData->BaseInfo.iexitemrole = pbakRoleData->BaseInfo.iexitemrole; // hang trang
	pRoleData->BaseInfo.iexboxrole  = pbakRoleData->BaseInfo.iexboxrole; // ruong mo rong
	pRoleData->BaseInfo.nWorldStat	=pbakRoleData->BaseInfo.nWorldStat ;
	pRoleData->BaseInfo.nSectStat	= pbakRoleData->BaseInfo.nSectStat;

	pRoleData->BaseInfo.iRoleParm1  = pbakRoleData->BaseInfo.iRoleParm1;
    pRoleData->BaseInfo.iRoleParm2  = pbakRoleData->BaseInfo.iRoleParm2;
	pRoleData->BaseInfo.iRoleParm3  = pbakRoleData->BaseInfo.iRoleParm3;
	pRoleData->BaseInfo.iRoleParm11  = pbakRoleData->BaseInfo.iRoleParm11;
    pRoleData->BaseInfo.iRoleParm12  = pbakRoleData->BaseInfo.iRoleParm12;
	pRoleData->BaseInfo.iRoleParm13  = pbakRoleData->BaseInfo.iRoleParm13;
	pRoleData->BaseInfo.iRoleParm14  = pbakRoleData->BaseInfo.iRoleParm14;

	pRoleData->BaseInfo.iRoleParm4  = pbakRoleData->BaseInfo.iRoleParm4;

	pRoleData->BaseInfo.iRoleParm5  = pbakRoleData->BaseInfo.iRoleParm5 ;
	pRoleData->BaseInfo.iRoleParm10 = pbakRoleData->BaseInfo.iRoleParm10 ;

	snprintf(pRoleData->BaseInfo.cpartnercode, sizeof(pRoleData->BaseInfo.cpartnercode), "%s", pbakRoleData->BaseInfo.cpartnercode);
    pRoleData->BaseInfo.iRoleParm5 = 0;
	pRoleData->BaseInfo.iRoleParm7  = 0;
	pRoleData->BaseInfo.iRoleParm8  = 	pbakRoleData->BaseInfo.iRoleParm8;
	pRoleData->BaseInfo.iRoleParm9  = 0;

	pRoleData->BaseInfo.ipassrole   = pbakRoleData->BaseInfo.ipassrole;       //锁屏幕密码
	pRoleData->BaseInfo.icjtaskid   = pbakRoleData->BaseInfo.icjtaskid;  //抽奖次数
	pRoleData->BaseInfo.isCreatTongBan = pbakRoleData->BaseInfo.isCreatTongBan;  // 同伴
	pRoleData->BaseInfo.isWaiGua       = pbakRoleData->BaseInfo.isWaiGua;           // 当前的存档的服务器编号  区
	pRoleData->BaseInfo.isSerLock      =pbakRoleData->BaseInfo.isSerLock ;     //是否锁住某人

	pRoleData->BaseInfo.iZhenYuan           = pbakRoleData->BaseInfo.iZhenYuan ;             //真元值
    pRoleData->BaseInfo.iJinMaiBingJia      =  pbakRoleData->BaseInfo.iJinMaiBingJia;        //经脉兵甲值


	pRoleData->BaseInfo.iservermon		= 	pbakRoleData->BaseInfo.iservermon;
	pRoleData->BaseInfo.isavemoney		= pbakRoleData->BaseInfo.isavemoney;
	pRoleData->BaseInfo.iclientx		= pbakRoleData->BaseInfo.iclientx;

	pRoleData->BaseInfo.ifiveprop		= pbakRoleData->BaseInfo.ifiveprop;
	pRoleData->BaseInfo.iteam			= pbakRoleData->BaseInfo.iteam;
	pRoleData->BaseInfo.bSex			= pbakRoleData->BaseInfo.bSex;
	pRoleData->BaseInfo.imaxlife		= pbakRoleData->BaseInfo.imaxlife;
	pRoleData->BaseInfo.imaxnuqi		= pbakRoleData->BaseInfo.imaxnuqi; //怒气
	pRoleData->BaseInfo.imaxstamina		= pbakRoleData->BaseInfo.imaxstamina	;
	pRoleData->BaseInfo.imaxinner		= pbakRoleData->BaseInfo.imaxinner;
	pRoleData->BaseInfo.icurlife		= pbakRoleData->BaseInfo.icurlife;
	pRoleData->BaseInfo.icurinner		= pbakRoleData->BaseInfo.icurinner;
	pRoleData->BaseInfo.icurstamina		= pbakRoleData->BaseInfo.icurstamina;

	//登入点
	pRoleData->BaseInfo.irevivalid      = 	pbakRoleData->BaseInfo.irevivalid;
	pRoleData->BaseInfo.irevivalx       = 	pbakRoleData->BaseInfo.irevivalx;
	pRoleData->BaseInfo.irevivaly       = 	0;

	pRoleData->BaseInfo.cUseRevive   = 0; //不使用重生点上线
	pRoleData->BaseInfo.ientergameid =pbakRoleData->BaseInfo.ientergameid;
	pRoleData->BaseInfo.ientergamex  = pbakRoleData->BaseInfo.ientergamex;
	pRoleData->BaseInfo.ientergamey  = pbakRoleData->BaseInfo.ientergamey ;
	pRoleData->BaseInfo.cFightMode   =pbakRoleData->BaseInfo.cFightMode;

	pRoleData->BaseInfo.cUseRevive   = pbakRoleData->BaseInfo.cUseRevive;   //是否利用重生点

	pRoleData->BaseInfo.ientergameid = pbakRoleData->BaseInfo.ientergameid;

    pRoleData->BaseInfo.ientergamex=pbakRoleData->BaseInfo.ientergamex;
    pRoleData->BaseInfo.ientergamey=pbakRoleData->BaseInfo.ientergamey;  //获取当前在线的坐标
	pRoleData->BaseInfo.cFightMode = pbakRoleData->BaseInfo.cFightMode;

	pRoleData->BaseInfo.cUseRevive = 1;  //使用重生点上线
	pRoleData->BaseInfo.cFightMode = 0;

	pRoleData->BaseInfo.icurlife    =pbakRoleData->BaseInfo.icurlife;
	pRoleData->BaseInfo.icurinner   = pbakRoleData->BaseInfo.icurinner;
	pRoleData->BaseInfo.icurstamina = pbakRoleData->BaseInfo.icurstamina;
	pRoleData->BaseInfo.iRoleParm6 = 0;

	//PK相关
	pRoleData->BaseInfo.cPkStatus =pbakRoleData->BaseInfo.cPkStatus;
	pRoleData->BaseInfo.ipkvalue = pbakRoleData->BaseInfo.ipkvalue;
//声望
	pRoleData->BaseInfo.ireputevalue = pbakRoleData->BaseInfo.ireputevalue ;
//福源
	pRoleData->BaseInfo.ifuyuanvalue = pbakRoleData->BaseInfo.ifuyuanvalue;
//转生
	pRoleData->BaseInfo.irebornvalue = pbakRoleData->BaseInfo.irebornvalue;  
//会员
	pRoleData->BaseInfo.isvip = pbakRoleData->BaseInfo.isvip; 

	pRoleData->dwFSkillOffset = (BYTE * )pRoleData->pBuffer - (BYTE *)pRoleBuffer; //下一个存档战斗技能数据的开始偏移

	//printf("存档:保存=玩家基本数据列表1=信息成功\n");
	return 1;	
}

int	 CDBLoad::SavePlayerFightSkillListDB(BYTE * pRoleBuffer, UINT room,
	   char *bakRoleData, UINT bakroom)
{
	_ASSERT(pRoleBuffer);

	TRoleData * pRoleData = (TRoleData *)pRoleBuffer; //角色基本数据
	//新数据
	// the offset and the count are the backup record's own layout numbers, read out of the
	// file, and the two buffers they index hold `room' and `bakroom' bytes.  An offset past
	// either end - and a negative `int' offset is that, once unsigned - or a count that
	// walks off either end is refused before anything is written.
	TRoleData *nTempData=(TRoleData *)bakRoleData;
	if ((UINT)nTempData->nFightSkillCount > (room - pRoleData->dwFSkillOffset) / sizeof(TDBSkillData) ||
		(UINT)nTempData->nFightSkillCount > (bakroom - nTempData->dwFSkillOffset) / sizeof(TDBSkillData) ||
		(UINT)pRoleData->dwFSkillOffset > room ||
		(UINT)nTempData->dwFSkillOffset > bakroom)
		return -1;
	TDBSkillData * pSkillData = (TDBSkillData *) (pRoleBuffer + pRoleData->dwFSkillOffset);
	
	//备份得是老数据
	//TDBSkillData_OLD * _bakSkillData = (TDBSkillData_OLD *)((BYTE*)bakRoleData+nTempData->dwFSkillOffset);
	//备份得是新数据
	TDBSkillData * _bakSkillData = (TDBSkillData *)((BYTE*)bakRoleData+nTempData->dwFSkillOffset);

	// the room each side has left is what the copy may walk, and the offsets above proved
	// both subtractions non-negative.
	UpdateDBSkillListDB((BYTE *)pSkillData, room - pRoleData->dwFSkillOffset,
		(BYTE *)_bakSkillData, bakroom - nTempData->dwFSkillOffset, nTempData->nFightSkillCount);
	
	if (nTempData->nFightSkillCount > 0)
	{
		pRoleData->nFightSkillCount = nTempData->nFightSkillCount;
		pRoleData->dwLSkillOffset = (BYTE*)pSkillData - pRoleBuffer + sizeof(TDBSkillData) * nTempData->nFightSkillCount;
	}
	else
	{
		pRoleData->nFightSkillCount  = 0;
		pRoleData->dwLSkillOffset = (BYTE*)pSkillData - pRoleBuffer;  //下个偏移是 生活技能
	}

	return 1;
}

int	  CDBLoad::UpdateDBSkillListDB(BYTE * pSkillBuffer, UINT room,
		  BYTE *bakSkillBuffer, UINT bakroom, int count)
{
	if (!pSkillBuffer)
		return -1;
	
	int nCount = 0,i;

	TDBSkillData * pSkillData = (TDBSkillData *)pSkillBuffer;
	//备份时是老数据
	//TDBSkillData_OLD * pbakSkillData = (TDBSkillData_OLD *)bakSkillBuffer;
	//备份时是新数据
	// `count' arrives from the record, so the two rooms this copy was given are the only
	// thing that says the loop cannot step past either array.
	if ((UINT)count > room / sizeof(TDBSkillData) || (UINT)count > bakroom / sizeof(TDBSkillData))
		return -1;
	TDBSkillData * pbakSkillData = (TDBSkillData *)bakSkillBuffer;
	
	for (i = 0 ; i < count; i++)
	{
		pSkillData->m_nSkillId =pbakSkillData->m_nSkillId;
        pSkillData->m_nSkillExp=  pbakSkillData->m_nSkillExp;          //该技能当前的熟练度
		pSkillData->m_nSkillLevel = pbakSkillData->m_nSkillLevel;
		pSkillData->m_nExpSkillPonit=pbakSkillData->m_nExpSkillPonit;//修炼出来的技能点
		nCount++;    //实际的技能数
		pSkillData++;
		pbakSkillData++;
	}
	return nCount;
}


int CDBLoad::SavePlayerLifeSkilllListDB(BYTE * pRoleBuffer, UINT room,
	char *bakRoleData, UINT bakroom)
{
	_ASSERT(pRoleBuffer);
	TRoleData * pRoleData = (TRoleData *)pRoleBuffer;
	// this offset was computed by the fight-skill section, and a record that arrives with
	// one already past the buffer would put the next section's head outside it.
	if ((UINT)pRoleData->dwLSkillOffset > room)
		return -1;
	TDBSkillData * pSkillData = (TDBSkillData *) (pRoleBuffer + pRoleData->dwLSkillOffset);
	pRoleData->nLiveSkillCount  = 0;
	pRoleData->dwTaskOffset = (BYTE*)pSkillData - pRoleBuffer;	//下个偏移是 任务
	//printf("存档:保存=玩家生活技能数据3=信息成功\n");
	return 1;
}

//保存任务到数据库
int	 CDBLoad::SavePlayerTaskListDB(BYTE * pRoleBuffer, UINT room,
	   char *bakRoleData, UINT bakroom)
{
	_ASSERT(pRoleBuffer);
	TRoleData * pRoleData = (TRoleData *) pRoleBuffer;
	int nTaskCount = 0,nTaskId = 0,nTaskDegee = 0;

	//玩家在游戏当中的任务保存
	// the same pair of numbers as the skills above: an offset that starts the copy outside
	// one of the two buffers, or a count that ends it outside one, is refused here.
	TRoleData * nTempData = (TRoleData *) bakRoleData;
	if ((UINT)nTempData->nTaskCount > (room - pRoleData->dwTaskOffset) / sizeof(TDBTaskData) ||
		(UINT)nTempData->nTaskCount > (bakroom - nTempData->dwTaskOffset) / sizeof(TDBTaskData) ||
		(UINT)pRoleData->dwTaskOffset > room ||
		(UINT)nTempData->dwTaskOffset > bakroom)
		return -1;
	int count = nTempData->nTaskCount;
	TDBTaskData * pTaskData = (TDBTaskData *)(pRoleBuffer + pRoleData->dwTaskOffset);

	TDBTaskData *  bakTaskData= (TDBTaskData *)((BYTE*)bakRoleData+nTempData->dwTaskOffset);
	for (int i = 0; i < count; i++)
	{//一个不小的延时
		pTaskData->m_nTaskId =bakTaskData->m_nTaskId;  //任务号
		pTaskData->m_nTaskValue =bakTaskData->m_nTaskValue;  //值
		pTaskData++;
		bakTaskData++;
	}
	pRoleData->nTaskCount =nTempData->nTaskCount;  //任务数量
	pRoleData->dwItemOffset= (BYTE *)pTaskData - pRoleBuffer;
	
	//memcpy(pTaskData,_bakSkillData,asize);
	//for (int i = 0; i < MAX_TASK; i++)
	//{//一个不小的延时
	//	if  (!pTaskData)
	//		break;

	//	int ver=m_cTask.GetSaveVal(i);
	//	if (ver)
	//	{
	//		pTaskData->m_nTaskId = i;  //任务号
	//		pTaskData->m_nTaskValue =ver;  //值
	//		nTaskCount++;
	//		pTaskData++;
	//	}
	//}
	//pRoleData->nTaskCount = nTaskCount;  //任务数量
	//pRoleData->dwItemOffset = (BYTE *)pTaskData - pRoleBuffer;	//下个偏移是 物品

	return 1;
}

//保存装备清单
int	 CDBLoad::SavePlayerItemListDB(BYTE * pRoleBuffer, UINT room,
	   char *bakRoleData, UINT bakroom)
{
	_ASSERT(pRoleBuffer);

	TRoleData * pRoleData = (TRoleData*)pRoleBuffer;
	//物品得开始偏移
	// one item is 291 bytes and the count is a `short', so the record alone can ask for
	// nine megabytes of a 128 KiB buffer at either width; the offset it starts at is the
	// file's number too.  Both are bounded against the room each side really has.
	TRoleData * nTempData = (TRoleData *)bakRoleData;
	if ((UINT)nTempData->nItemCount > (room - pRoleData->dwItemOffset) / sizeof(TDBItemData) ||
		(UINT)nTempData->nItemCount > (bakroom - nTempData->dwItemOffset) / sizeof(TDBItemData) ||
		(UINT)pRoleData->dwItemOffset > room ||
		(UINT)nTempData->dwItemOffset > bakroom)
		return -1;
	TDBItemData * pItemData = (TDBItemData*)((BYTE*)pRoleData + pRoleData->dwItemOffset);

	TDBItemData * bakItemData = (TDBItemData *)((BYTE*)bakRoleData+nTempData->dwItemOffset);

	int tempItemCount =nTempData->nItemCount;

	for (int i=0;i<tempItemCount;i++)
	{
		pItemData->iequipclasscode = bakItemData->iequipclasscode;
		pItemData->iequipcode      =  0;//Item[nItemIndex].GetID();
		pItemData->idetailtype     =  bakItemData->idetailtype;
		pItemData->iparticulartype =  bakItemData->iparticulartype;
		pItemData->ilevel          =  bakItemData->ilevel;
		pItemData->iseries         =  bakItemData->iseries;

		pItemData->ilocal          = bakItemData->ilocal;
		pItemData->ix              =  bakItemData->ix ;
		pItemData->iy              = 	bakItemData->iy ;

		pItemData->iequipversion   = bakItemData->iequipversion ;
		pItemData->irandseed = bakItemData->irandseed;
		pItemData->iparam1  =  bakItemData->iparam1;//蓝装属性
		pItemData->iparam2  = bakItemData->iparam2 ;
		pItemData->iparam3  =  bakItemData->iparam3;
		pItemData->iparam4  =  bakItemData->iparam4;
		pItemData->iparam5  =  bakItemData->iparam5;
		pItemData->iparam6  = bakItemData->iparam6;
		pItemData->iparamr1 =  bakItemData->iparamr1;//熔炼属性
		pItemData->iparamr2 =  bakItemData->iparamr2;
		pItemData->iparamr3 =  bakItemData->iparamr3;
		pItemData->iparamr4 =  bakItemData->iparamr4;
		pItemData->iparamr5 = bakItemData->iparamr5;
		pItemData->iparamr6 =  bakItemData->iparamr6;
		pItemData->iparamj1 =  bakItemData->iparamj1;//基本属性
		pItemData->iparamj2 =  bakItemData->iparamj2;
		pItemData->iparamj3 =  bakItemData->iparamj3;
		pItemData->iparamj4 =  bakItemData->iparamj4;
		pItemData->iparamj5 =  bakItemData->iparamj5;
		pItemData->iparamj6 =  bakItemData->iparamj6;
        pItemData->iparamj7 =  bakItemData->iparamj7;
		pItemData->iparamb1 = bakItemData->iparamb1;//专用地图//Item[nItemIndex].GetItemParam()->nBGeneratorLevel[0];//宝石属性
		pItemData->iparamb2 = bakItemData->iparamb2;//改变的装备外观Item[nItemIndex].GetItemParam()->nBGeneratorLevel[1];
		pItemData->iparamb3 = bakItemData->iparamb3;//存档翅膀Item[nItemIndex].GetItemParam()->nBGeneratorLevel[2];
		pItemData->iparamb4 = bakItemData->iparamb4;   //使用的类型
		pItemData->iparamb5 =  0;
		pItemData->iparamb6 =  0;
		pItemData->ilucky   =  		bakItemData->ilucky; //保存幸运值
		pItemData->idurability  = bakItemData->idurability;
		pItemData->istacknum    = bakItemData->istacknum;
		pItemData->iidentify    = 0;
		pItemData->igoldid      = bakItemData->igoldid ;
		pItemData->isplatina    = bakItemData->isplatina ;
		pItemData->irongNum     = bakItemData->irongNum; //可熔炼数量
		pItemData->iwengangPin  = bakItemData->iwengangPin;
		pItemData->ibinfujiazhi = bakItemData->ibinfujiazhi;
		pItemData->iRongpiont   = bakItemData->iRongpiont;

		pItemData->ienchance    = bakItemData->ienchance;
		pItemData->ipoint       = 	bakItemData->ipoint;  
		pItemData->iswhere      = bakItemData->iswhere; 
		pItemData->iyear        = 0;//bakItemData->iyear;
		pItemData->imonth     = 0;//bakItemData->imonth;
		pItemData->iday         = 0;//bakItemData->iday;
		pItemData->ihour        = 0;//bakItemData->ihour;
		pItemData->imin         = 0;//bakItemData->imin;
        pItemData->iIsBang      =  bakItemData->iIsBang ;        //是否绑定
		pItemData->itradeprice		= 0;
		pItemData->ilockstate		= bakItemData->ilockstate;
		pItemData->ilocktime		= bakItemData->ilocktime;
		pItemData->iIsKuaiJie   = bakItemData->iIsKuaiJie;     //是否快捷栏
		pItemData->iIsMagic     = bakItemData->iIsMagic ;       //是否魔法属性
		pItemData->iSkillType   = bakItemData->iSkillType;
		snprintf(pItemData->iWonName, sizeof(pItemData->iWonName), "%s", bakItemData->iWonName); //Item[nItemIndex].GetWonName()

		pItemData++;
		bakItemData++;
		//nItemCount++;
	}
	//int	nItemCount = 0;

	//int nIdx = 0;
	//while(1) //永远循环
	//{
	//	nIdx = m_ItemList.m_UseIdxItemList.GetNext(nIdx);

	//	if (nIdx <= 0)
	//		break;

	//	int nItemIndex = m_ItemList.m_Items[nIdx].nIdx;

	//	//sprintf(szSection, "%s%d", SECTION_ITEM, nItemCount + 1); //item+数字 
	//	//玩家基本信息*****************************************************************8	
	//	pItemData->iequipclasscode =  Item[nItemIndex].m_CommonAttrib.nItemGenre;
	//	pItemData->iequipcode      =  0;//Item[nItemIndex].GetID();
	//	pItemData->idetailtype     =  Item[nItemIndex].m_CommonAttrib.nDetailType;
	//	pItemData->iparticulartype =  Item[nItemIndex].m_CommonAttrib.nParticularType;
	//	pItemData->ilevel          =  Item[nItemIndex].m_CommonAttrib.nLevel;
	//	pItemData->iseries         =  Item[nItemIndex].m_CommonAttrib.nSeries;

	//	pItemData->ilocal          =  m_ItemList.m_Items[nIdx].nPlace;
	//	pItemData->ix              =  m_ItemList.m_Items[nIdx].nX;
	//	pItemData->iy              =  m_ItemList.m_Items[nIdx].nY;

	//	pItemData->iequipversion   = Item[nItemIndex].GetItemParam()->nVersion;
	//	pItemData->irandseed = Item[nItemIndex].GetItemParam()->uRandomSeed;
	//	pItemData->iparam1  =  Item[nItemIndex].GetItemParam()->nGeneratorLevel[0];//蓝装属性
	//	pItemData->iparam2  =  Item[nItemIndex].GetItemParam()->nGeneratorLevel[1];
	//	pItemData->iparam3  =  Item[nItemIndex].GetItemParam()->nGeneratorLevel[2];
	//	pItemData->iparam4  =  Item[nItemIndex].GetItemParam()->nGeneratorLevel[3];
	//	pItemData->iparam5  =  Item[nItemIndex].GetItemParam()->nGeneratorLevel[4];
	//	pItemData->iparam6  =  Item[nItemIndex].GetItemParam()->nGeneratorLevel[5];
	//	pItemData->iparamr1 =  Item[nItemIndex].GetItemParam()->nRGeneratorLevel[0];//熔炼属性
	//	pItemData->iparamr2 =  Item[nItemIndex].GetItemParam()->nRGeneratorLevel[1];
	//	pItemData->iparamr3 =  Item[nItemIndex].GetItemParam()->nRGeneratorLevel[2];
	//	pItemData->iparamr4 =  Item[nItemIndex].GetItemParam()->nRGeneratorLevel[3];
	//	pItemData->iparamr5 =  Item[nItemIndex].GetItemParam()->nRGeneratorLevel[4];
	//	pItemData->iparamr6 =  Item[nItemIndex].GetItemParam()->nRGeneratorLevel[5];
	//	pItemData->iparamj1 =  Item[nItemIndex].GetItemParam()->nJGeneratorLevel[0];//基本属性
	//	pItemData->iparamj2 =  Item[nItemIndex].GetItemParam()->nJGeneratorLevel[1];
	//	pItemData->iparamj3 =  Item[nItemIndex].GetItemParam()->nJGeneratorLevel[2];
	//	pItemData->iparamj4 =  Item[nItemIndex].GetItemParam()->nJGeneratorLevel[3];
	//	pItemData->iparamj5 =  Item[nItemIndex].GetItemParam()->nJGeneratorLevel[4];
	//	pItemData->iparamj6 =  Item[nItemIndex].GetItemParam()->nJGeneratorLevel[5];
 //       pItemData->iparamj7 =  Item[nItemIndex].GetItemParam()->nJGeneratorLevel[6];
	//	pItemData->iparamb1 =  Item[nItemIndex].GetUseMapIdx();//专用地图//Item[nItemIndex].GetItemParam()->nBGeneratorLevel[0];//宝石属性
	//	pItemData->iparamb2 =  Item[nItemIndex].GetItemResIdx();//改变的装备外观Item[nItemIndex].GetItemParam()->nBGeneratorLevel[1];
	//	pItemData->iparamb3 =  Item[nItemIndex].GetChiBangRes();//存档翅膀Item[nItemIndex].GetItemParam()->nBGeneratorLevel[2];
	//	pItemData->iparamb4 =  Item[nItemIndex].GetUseKind();   //使用的类型
	//	pItemData->iparamb5 =  0;//Item[nItemIndex].GetItemParam()->nBGeneratorLevel[4];
	//	pItemData->iparamb6 =  0;//Item[nItemIndex].GetItemParam()->nBGeneratorLevel[5];
	//	pItemData->ilucky   =  Item[nItemIndex].GetItemParam()->nLuck; //保存幸运值
	//	pItemData->idurability  = Item[nItemIndex].GetDurability();
	//	pItemData->istacknum    = Item[nItemIndex].GetStackNum();
	//	pItemData->iidentify    = 0;
	//	pItemData->igoldid      = Item[nItemIndex].GetGoldId();
	//	pItemData->isplatina    = Item[nItemIndex].GetIsPlatima();
	//	pItemData->irongNum     = Item[nItemIndex].GetRongNum(); //可熔炼数量
	//	pItemData->iwengangPin  = Item[nItemIndex].GetWengangPin();
	//	pItemData->ibinfujiazhi = Item[nItemIndex].GetBinfujiazhi();
	//	pItemData->iRongpiont   = Item[nItemIndex].IsRong();

	//	pItemData->ienchance    = Item[nItemIndex].GetEnChance();
	//	pItemData->ipoint       = Item[nItemIndex].GetPoint();   
	//	pItemData->iswhere      = Item[nItemIndex].GetIsWhere(); 
	//	pItemData->iyear        = Item[nItemIndex].GetTime()->bYear;
	//	pItemData->imonth       = Item[nItemIndex].GetTime()->bMonth;
	//	pItemData->iday         = Item[nItemIndex].GetTime()->bDay;
	//	pItemData->ihour        = Item[nItemIndex].GetTime()->bHour;
	//	pItemData->imin         = Item[nItemIndex].GetTime()->bMin;
 //       pItemData->iIsBang      = Item[nItemIndex].GetIsBang();        //是否绑定
	//	pItemData->iIsKuaiJie   = Item[nItemIndex].GetIsKuaiJie();     //是否快捷栏
	//	pItemData->iIsMagic     = Item[nItemIndex].GetIsMagic();       //是否魔法属性
	//	pItemData->iSkillType   = Item[nItemIndex].GetSkillType();
	//	sprintf(pItemData->iWonName,Item[nItemIndex].GetWonName()); //Item[nItemIndex].GetWonName()
	//	//*****************************************************************************8
	//	//++pItemData;
	//	//++nItemCount;
	//	if  (_gameGlobalSet.gameItemInfo.nshowinfo)
	//	{
	//		char nTmepInfo[256];
	//		sprintf(nTmepInfo,"--[%s]存档[%d]:%s,物品|:[%d,%d,%d]位置:[%d,%d,%d]--",m_PlayerName,nItemCount,Item[nItemIndex].GetName(),pItemData->iequipclasscode,pItemData->idetailtype,pItemData->iparticulartype,pItemData->ilocal,pItemData->ix,pItemData->iy);
	//		g_LogFile.gTraceLogFile(nTmepInfo,strlen(nTmepInfo),"SavePlayerItemList");
	//	}

	//	pItemData++;
	//	nItemCount++;
	//}

	pRoleData->nItemCount = tempItemCount;  //存档
	pRoleData->dwFriendOffset = (BYTE *)pItemData - (BYTE *)pRoleBuffer;		 //下个偏移是  朋友列表
	pRoleData->dwDataLen = (BYTE*)pItemData - (BYTE*)pRoleBuffer;                //全部数据的总长度

	return 1;
}

int	 CDBLoad::SavePlayerFriendListDB(BYTE * pRoleBuffer, UINT room,
	   char *bakRoleData, UINT bakroom)
{
	_ASSERT(pRoleBuffer);
	int		nFriendNo;
	TRoleData * pRoleData = (TRoleData *)pRoleBuffer;
	// the length this function finally reports is the distance from the buffer's head to
	// here, so an offset the record invented does not just write outside - it makes the
	// size that goes to the database outside too.
	if ((UINT)pRoleData->dwFriendOffset > room)
		return -1;
	char * pCurFriend = (char *)((BYTE *)pRoleData + pRoleData->dwFriendOffset);
	pRoleData->nFriendCount = 0;
	pRoleData->dwDataLen = (BYTE*)pCurFriend - (BYTE*)pRoleBuffer;	   //数据的总字节大小数
	return 1;
}






bool CDBLoad::SearchData(char* key, char *aData, UINT& size)
{//搜索数据
	if (!GotoHead())
	{
		size = 0;
		return false;
	}
	char aBuffer[128*1024] = {0};    //64  角色数据
	//char *aBuffer=NULL;
	//     aBuffer = new char [64*1024];

	//if (aBuffer==NULL)
	//	return false;

	char aKeyBuffer[sizeof(TRoleBaseInfo::caccname) + 1] = {0};     //关键字 是角色名 或 账号名
	// size arrives holding the room the caller's buffer has and leaves holding the record
	// that was written into it.  asize is a count this function read out of the backup
	// file, so a record larger than the room it was asked to fill is refused rather than
	// copied over the end of somebody else's array.
	const UINT nRoom = size;
	size = 0;
	UINT asize,akeysize;
	// the buffers above are the room ReadData is told about: a record the backup file
	// claims is bigger than either of them is refused rather than read into them.
	while(ReadData(aKeyBuffer, sizeof(aKeyBuffer), akeysize, aBuffer, sizeof(aBuffer), asize))
	{
		if(strcmp(key, aKeyBuffer)==0)
		{
			if (asize > nRoom)
				return false;
			memcpy(aData,aBuffer,asize);
			size = asize;
			//delete [] aBuffer;
            //aBuffer=NULL;
			return true;
		}
	}

	//delete [] aBuffer;
    //aBuffer=NULL;
	return false;
}

void AddOutputString_db(HWND hListCtrl, char* aStr)
{//添加操作输出文字
	if ( hListCtrl && ::IsWindow(hListCtrl))
	{
		int nCount = ::SendMessage( hListCtrl,LB_GETCOUNT,0,0);	//获取信息数量

		if(nCount >= 100)
		{//删除信息
			::SendMessage(hListCtrl,LB_DELETESTRING,100,0);
		}
		//增加信息
		int nIndex = ::SendMessage( hListCtrl,LB_INSERTSTRING,0,(LPARAM )aStr);//把新的信息添到第一个
	}
}
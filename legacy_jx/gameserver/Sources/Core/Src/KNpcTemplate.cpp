#include "KCore.h"
#include "KNpc.h"    //原来没有的
#include "KNpcTemplate.h"

#include "KDropScript.h"

#ifndef WIN32
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#else 
#include <io.h>
#include <direct.h>
#endif
//-------------新的----------------------------------------------
#include <algorithm>

#define MAX_VALUE_LEN 300

#ifdef _SERVER
//重载小于,只能重载类名

int	operator<(KDropScriptNode iniLeft, KDropScriptNode iniRight)
{
	return strcmp(iniLeft.m_szFileName, iniRight.m_szFileName);
};
//重载等于,只能重载类名
int operator==(KDropScriptNode iniLeft, KDropScriptNode iniRight)
{
	return (strcmp(iniLeft.m_szFileName, iniRight.m_szFileName) == 0);
};

//加载爆率  不给调用？
static KItemDropRate * g_LoadItemDropRate(char * DropIniFile)
{	
	    if ((!DropIniFile) || (!DropIniFile[0]))
		   return NULL;

	    KIniFile szDropIniFile;
		if (szDropIniFile.Load(DropIniFile))
		{
		  int nItemCount = 0;

		  szDropIniFile.GetInteger("Main", "Count", 0, &nItemCount);         //总数量
	
		  if (nItemCount<=0)
		  {
			  szDropIniFile.Clear();
			  return NULL;
		  }

		  KItemDropRate * pnewDrop = new KItemDropRate;  //新建一块内存
			
		  if (!pnewDrop)
		  {
			  szDropIniFile.Clear();
			  return NULL;
		  }
		pnewDrop->nCount = nItemCount;
 		//printf("---------[爆率物品数量:%d,加载成功]-------- \n",pnewDrop->nCount);
		int nVal=0;
		szDropIniFile.GetInteger("Main", "RandRange", 0, &pnewDrop->nMaxRandRate);
		szDropIniFile.GetInteger("Main", "MagicRate", 0, &pnewDrop->nMagicRate);
		szDropIniFile.GetInteger("Main", "MoneyRate", 2, &pnewDrop->nMoneyRate);
		szDropIniFile.GetInteger("Main", "MoneyNum", 0, &pnewDrop->nMoneyNum);
		szDropIniFile.GetInteger("Main", "MoneyScale", 5, &pnewDrop->nMoneyScale);

		szDropIniFile.GetInteger("Main", "MinItemLevelScale", 20, &nVal);
		pnewDrop->nMinItemLevelScale = (BYTE)nVal;
		szDropIniFile.GetInteger("Main", "MaxItemLevelScale", 10, &nVal);
		pnewDrop->nMaxItemLevelScale = (BYTE)nVal;
		szDropIniFile.GetInteger("Main", "MaxItemLevel", 10, &nVal);
		pnewDrop->nMaxItemLevel = (BYTE)nVal;
		szDropIniFile.GetInteger("Main", "MinItemLevel", 1, &nVal);
		pnewDrop->nMinItemLevel = (BYTE)nVal;

		szDropIniFile.GetInteger("Main", "nTypeNum", 1, &pnewDrop->nTypeNum);

		szDropIniFile.GetInteger("Main", "nEquipType",0, &nVal);   //是否可以爆辨识装备
		pnewDrop->nIsBianShi = (BYTE)nVal;


		pnewDrop->pItemParam = new KItemDropRate::KItemParam[pnewDrop->nCount];

		if (!pnewDrop->pItemParam)
		{
			szDropIniFile.Clear();
			delete pnewDrop;
			pnewDrop=NULL;
			return NULL;
		}

		char szSection[10]={0};
		
		KItemDropRate::KItemParam * pItemParam = pnewDrop->pItemParam;
		for(int i = 0; i < pnewDrop->nCount; ++i, ++pItemParam)
		{
			snprintf(szSection, sizeof(szSection), "%d", i + 1); //
			szDropIniFile.GetInteger(szSection, "Genre", 0, &(pItemParam->nGenre));
			szDropIniFile.GetInteger(szSection, "Detail", 0, &(pItemParam->nDetailType));
			szDropIniFile.GetInteger(szSection, "Particular", 0, &(pItemParam->nParticulType));
			szDropIniFile.GetInteger(szSection, "RandRate", 0, &(pItemParam->nRate));
			szDropIniFile.GetInteger(szSection, "Quality", 0, &(pItemParam->nQuality));
			szDropIniFile.GetInteger(szSection, "MinItemLevel", 0, &nVal);
			pItemParam->nMinItemLevel = (BYTE)nVal;
			szDropIniFile.GetInteger(szSection, "MaxItemLevel", 0, &nVal);
			pItemParam->nMaxItemLevel = (BYTE)nVal;
			szDropIniFile.GetInteger(szSection, "Hour", 0, &nVal);
			pItemParam->nHour = (BYTE)nVal;
			szDropIniFile.GetInteger(szSection, "IsBang", 0, &nVal);
			pItemParam->nIsBang = (BYTE)nVal;
			szDropIniFile.GetInteger(szSection, "MaxStackNum",1, &(pItemParam->nStackNum)); 
			szDropIniFile.GetInteger(szSection, "IsNoBian", 0, &nVal);
			pItemParam->nIsNoBian = (BYTE)nVal;
			szDropIniFile.GetInteger(szSection, "IsNoRandGold",0, &nVal);
			pItemParam->nIsNoRandGold = (BYTE)nVal;

		}
		    szDropIniFile.Clear();
		    printf("---------[加载成功,返回]-------- \n");
			return pnewDrop;
		}

			szDropIniFile.Clear();
		    return NULL;	
}

//加载爆率  不给调用？
static KItemDropRate * g_GenItemDropRate(char * szDropIniFile)
{
	if ((!szDropIniFile)|| (!szDropIniFile[0]))
		return NULL;
	
	KIniFile IniFile;
	if (IniFile.Load(szDropIniFile))
	{
		int nItemCount =0;
		IniFile.GetInteger("Main", "Count", 0, &nItemCount);         //总数量

		if (nItemCount<=0)
		{
			IniFile.Clear();
			return NULL;
		}

		KItemDropRate * pnewDrop = new KItemDropRate;  //新建一块内存

		if (!pnewDrop)
		{
			IniFile.Clear();
			return NULL;
		}

        pnewDrop->nCount = nItemCount;
		
		/*if(pnewDrop->nCount <= 0)
		{
			IniFile.Clear();
			delete pnewDrop;
			pnewDrop=NULL;
			return NULL;
		}  */
		int nVal = 0;
		IniFile.GetInteger("Main", "RandRange", 0, &pnewDrop->nMaxRandRate);
		IniFile.GetInteger("Main", "MagicRate", 0, &pnewDrop->nMagicRate);
		IniFile.GetInteger("Main", "MoneyRate", 2, &pnewDrop->nMoneyRate);
		IniFile.GetInteger("Main", "MoneyNum", 0, &pnewDrop->nMoneyNum);
		IniFile.GetInteger("Main", "MoneyScale", 5, &pnewDrop->nMoneyScale);
		IniFile.GetInteger("Main", "MinItemLevelScale", 20, &nVal);
		pnewDrop->nMinItemLevelScale = (BYTE)nVal;
		IniFile.GetInteger("Main", "MaxItemLevelScale", 10, &nVal);
		pnewDrop->nMaxItemLevelScale = (BYTE)nVal;
		IniFile.GetInteger("Main", "MaxItemLevel", 10, &nVal);
		pnewDrop->nMaxItemLevel = (BYTE)nVal;
		IniFile.GetInteger("Main", "MinItemLevel", 1, &nVal);
		pnewDrop->nMinItemLevel = (BYTE)nVal;
		IniFile.GetInteger("Main", "nTypeNum", 2, &pnewDrop->nTypeNum);
		IniFile.GetInteger("Main", "nEquipType",0, &nVal);   //是否可以爆辨识装备
		pnewDrop->nIsBianShi = (BYTE)nVal;
		pnewDrop->pItemParam = new KItemDropRate::KItemParam[pnewDrop->nCount];

		if (!pnewDrop->pItemParam)
		{
			IniFile.Clear();
			delete pnewDrop;
			pnewDrop=NULL;
			return NULL;
		}

		char szSection[10]={0};
		KItemDropRate::KItemParam * pItemParam = pnewDrop->pItemParam;
		for(int i = 0; i < pnewDrop->nCount; ++i, ++pItemParam)
		{
			snprintf(szSection, sizeof(szSection), "%d", i + 1);
			IniFile.GetInteger(szSection, "Genre", -1, &(pItemParam->nGenre));
			IniFile.GetInteger(szSection, "Detail", 0, &(pItemParam->nDetailType));
			IniFile.GetInteger(szSection, "Particular", 0, &(pItemParam->nParticulType));
			IniFile.GetInteger(szSection, "RandRate", 0, &(pItemParam->nRate));
			IniFile.GetInteger(szSection, "Quality", 0, &(pItemParam->nQuality));
			IniFile.GetInteger(szSection, "MinItemLevel", 0, &nVal);
			pItemParam->nMinItemLevel = (BYTE)nVal;
			IniFile.GetInteger(szSection, "MaxItemLevel", 0, &nVal);
			pItemParam->nMaxItemLevel = (BYTE)nVal;
			IniFile.GetInteger(szSection, "Hour", 0, &nVal);
			pItemParam->nHour = (BYTE)nVal;
			IniFile.GetInteger(szSection, "IsBang", 0, &nVal);
			pItemParam->nIsBang = (BYTE)nVal;
			IniFile.GetInteger(szSection, "MaxStackNum",1, &(pItemParam->nStackNum));     //最大的叠加的数量
			IniFile.GetInteger(szSection, "IsNoBian", 0, &nVal);       //是否是辨识
			pItemParam->nIsNoBian = (BYTE)nVal;
			IniFile.GetInteger(szSection, "IsNoRandGold",0, &nVal);
			pItemParam->nIsNoRandGold = (BYTE)nVal;
			//IniFile.GetInteger(szSection, "DropType", 0, &(pItemParam->nDropType));
		}

		IniFile.Clear();
		return pnewDrop;
	}
	else
	{
		printf("-无法找到爆率文件:%s-\n", szDropIniFile);
	}

	IniFile.Clear();

	return NULL;
} 


/*
class KItemDropRateMap : public std::map<DWORD, KItemDropRate *>  //std::string
{
private:
	static VOID _FreeNode(value_type &cValue)
	{
		if (cValue.second)
		{
			delete cValue.second;
			cValue.second = NULL;
		}
	}
	
public:
	~KItemDropRateMap()
	{//删除某个爆率
		std::for_each(begin(), end(), _FreeNode);
	}
};

static KItemDropRateMap g_ItemDropRateMap;	// 为了保证内存管理的安全，该变量不再对外，请使用g_GetItemDropRate()

KItemDropRate::KItemDropRate()
{
	nCount		= 0;
	m_nRandSum		= 0;
	pItemParam	= NULL;
}

KItemDropRate::~KItemDropRate()
{
	Uninit();

  for (KItemDropRateMap::iterator it = g_ItemDropRateMap.begin(); it != g_ItemDropRateMap.end(); ++it)
  {
	 if (it->second)
	 {
		delete it->second;
		it->second = NULL;
	 }
  } 		
  g_ItemDropRateMap.clear();
}

BOOL KItemDropRate::Uninit()
{
	nCount		    = 0;
	m_nRandSum		= 0;

	if  (pItemParam)
	{
		delete pItemParam;
		pItemParam=NULL;
	}
	
	return TRUE;
}


BOOL KItemDropRate::LoadData(LPCSTR pszDropTabFile)
{
	KIniFile cIniFile;
	INT nLuckRateSum	= 0;
	BOOL bResult		= FALSE;
	
    if (!pszDropTabFile || !pszDropTabFile[0])
		return bResult;

	if (!cIniFile.Load(pszDropTabFile))
	{
		return bResult;
	}

	cIniFile.GetInteger("Main","Count",0,&nCount);

	if (nCount <=0)
	{
		cIniFile.Clear();
		return FALSE;
	}

	pItemParam	= new KItemDropRate::KItemParam[nCount];

	if (!pItemParam)
	{
		cIniFile.Clear();
		return FALSE;
	}

	cIniFile.GetInteger("Main", "RandRange", 0, &nMaxRandRate);
	cIniFile.GetInteger("Main", "MagicRate", 0, &nMagicRate);
	cIniFile.GetInteger("Main", "MoneyRate", 2, &nMoneyRate);
	cIniFile.GetInteger("Main", "MoneyNum", 0, &nMoneyNum);
	cIniFile.GetInteger("Main", "MoneyScale", 5, &nMoneyScale);
	cIniFile.GetInteger("Main", "MinItemLevelScale", 20, &nMinItemLevelScale);
	cIniFile.GetInteger("Main", "MaxItemLevelScale", 10, &nMaxItemLevelScale);
	cIniFile.GetInteger("Main", "MaxItemLevel", 10, &nMaxItemLevel);
	cIniFile.GetInteger("Main", "MinItemLevel", 1, &nMinItemLevel);
	cIniFile.GetInteger("Main", "nTypeNum", 2, &nTypeNum);
	cIniFile.GetInteger("Main", "nEquipType",0, &nIsBianShi);

	for(int i = 0; i < nCount; ++i)
	{
		KItemDropRate::KItemParam& rsItemParam	= pItemParam[i];

		char szSection[8]={0};
		sprintf(szSection, "%d", i + 1);
		cIniFile.GetInteger(szSection, "Genre", -1, &(rsItemParam.nGenre));	//如果没有这个节点的 就  不爆这个物品
		cIniFile.GetInteger(szSection, "Detail", 0, &(rsItemParam.nDetailType));
		cIniFile.GetInteger(szSection, "Particular", 0, &(rsItemParam.nParticulType));
		cIniFile.GetInteger(szSection, "RandRate", 0, &(rsItemParam.nRate));
		cIniFile.GetInteger(szSection, "Quality", 0, &(rsItemParam.nQuality));
		cIniFile.GetInteger(szSection, "MinItemLevel", 0, &(rsItemParam.nMinItemLevel));
		cIniFile.GetInteger(szSection, "MaxItemLevel", 0, &(rsItemParam.nMaxItemLevel));
		cIniFile.GetInteger(szSection, "Hour", 0, &(rsItemParam.nHour));
		cIniFile.GetInteger(szSection, "IsBang", 0, &(rsItemParam.nIsBang));
		cIniFile.GetInteger(szSection, "MaxStackNum",1, &(rsItemParam.nStackNum));     //最大的叠加的数量
		cIniFile.GetInteger(szSection, "IsNoBian", 0, &(rsItemParam.nIsNoBian));       //是否是辨识
		//IniFile.GetInteger(szSection, "DropType", 0, &(pItemParam->nDropType));
		cIniFile.GetInteger(szSection, "LuckyRate",	0,	&rsItemParam.nLuckyRate);

		m_nRandSum += rsItemParam.nRate;
		nLuckRateSum += rsItemParam.nLuckyRate;
	}

	cIniFile.Clear();
	bResult	= TRUE;

	return bResult;
}

INT KItemDropRate::GetItemParm(ITEM_PARM_DROP i)
{
	  switch (i)
	  {
	  case ITEM_PARM_COUNT:
		  {
			  return nCount;
		  }
		  break;
	  case ITEM_PARM_RATESUM:
		  {
			  return m_nRandSum;
		  }
		  break;
	  case ITEM_PARM_LUCKRATESUM:
		  {
			  return nLuckRateSum;
		  }
		  break;
	  case ITEM_PARM_MAGRATE:
		  {
			  return nMagicRate;
		  }
		  break;
	  case ITEM_PARM_MAXRATE:
		  {
			  return nMaxRandRate;
		  }
		  break;
	  case ITEM_PARM_MONEYRATE:
		  {
			  return nMoneyRate;
		  }
		break;
	  case ITEM_PARM_MONEYNUM:
		  {
			  return nMoneyNum;
		  }
		break;
	  case ITEM_PARM_MONEYSCALE:
		  {
			  return nMoneyScale;
		  }
		break;
		
	  case ITEM_PARM_MINLEVELSCALE:
		  {
			  return nMinItemLevelScale;//;
		  }
		break;
	  case ITEM_PARM_MAXLEVELSCALE:
		  {
			  return nMaxItemLevelScale;
		  }
		break;
	  case ITEM_PARM_MINLEVEL:
		  {
			  return nMinItemLevel;
		  }
		break;
	  case ITEM_PARM_MAXLEVEL:
		  {
			  return nMaxItemLevel;
		  }
		  break;
	  case ITEM_PARM_TYPENUM:
		  {
			  return nTypeNum;
		  }
		  break;
	  case ITEM_PARM_BIANSHI:
		  {
			  return nIsBianShi;
		  }
		  break;
	  default:
		  return 0;
	  }
}

CONST KItemDropRate::KItemParam* KItemDropRate::GetRandItem(INT nLucky) CONST
{
	CONST KItemParam* pRetItem	= NULL;

	if (nMaxItemLevelScale <= 0 || nMinItemLevelScale <= 0)
		return pRetItem;

	int nRand = g_Random(nMaxRandRate);  //总爆率  取余数

	int nCheckRand  = 0;	// 累加概率，确认是否落在区间内
    

	int i;
	for (i = 0; i < nCount; ++i)   //循环掉装备  装备的总数量
	{
		KItemParam& rsItem	= pItemParam[i];

		if (nRand >= nCheckRand && nRand < nCheckRand + rsItem.nRate)
		{//这个数大于等于上个物品的累计爆率并小于当时这个物品的累加爆率
			//nIsThisItem = i;
			pRetItem = &rsItem;
			break;
		}

		nCheckRand += rsItem.nRate;   //每个物品爆率加起来累加
	}

	if (i >= nCount)  //没找到合适物品爆 就返回了
		return NULL;
	
	//printf("--随机概率:%d %d(%d,%d,%d) --\n",nRand,i,pRetItem->nGenre,pRetItem->nDetailType,pRetItem->nParticulType);


	return pRetItem;
}

KItemDropRate *g_GetItemDropRate(LPCSTR pszDropTabFile)
{
	if (!pszDropTabFile || !pszDropTabFile[0])
		return	NULL;

	CHAR szDropRateFile[200]={0};
	g_StrCpyLen(szDropRateFile, pszDropTabFile, 200);
	g_StrLower(szDropRateFile);
	DWORD nRateFile=g_FileName2Id(szDropRateFile);
	if (g_ItemDropRateMap.count(nRateFile) > 0)
	{
		//printf("--爆率存在:%s --\n",szDropRateFile);
		return g_ItemDropRateMap[nRateFile];
	}

	KItemDropRate *pRate	= new KItemDropRate();

	if (!pRate->LoadData(szDropRateFile))
	{
		//_ASSERT(FALSE);		// 打开Droprate配置文件失败
		//SAFE_DELETE(pRate);
		if (pRate)
		{
			delete pRate;
			pRate= NULL;
		}
		return	NULL;
	}

	g_ItemDropRateMap[nRateFile] = pRate;
	return	pRate;
}
*/

//---------------------------------------------------------------
int	KNpcTemplate::InitDropRate(int nNpcIdex,char * nDropRateFiled)
{
  
	//	printf("----加载索引:%d,NPC名称:%s,等级:%d,爆率:%s,---- \n",nNpcIdex,Npc[nNpcIdex].Name,Npc[nNpcIdex].m_Level,nDropRateFiled);
    if (nDropRateFiled==NULL || !strstr(nDropRateFiled,".ini"))	
	{	
		//printf("---------[nDropRateFiled空爆率,加载失败]-------- \n");
		return 0;
	}

	strlwr(nDropRateFiled);	//strlwr
	
	KDropScriptNode DropNodeNew;
    strcpy(DropNodeNew.m_szFileName,nDropRateFiled);

	//g_DropBinTree.Delete(DropNodeNew);
	//int Nmu=0;
	//    Nmu=g_DropBinTree.ListSize();//BiTreeDepth(g_ItemDropRateBinTree.GetRoot());	 //节点数

	//printf("---------[g_DropBinTree 节点数[%d]]-------- \n",Nmu);

	try
	{
		if (g_DropBinTree.Find(DropNodeNew))
		{//如果已经存在这个爆率
			m_pItemDropRate           = DropNodeNew.m_pItemDropRate;
			Npc[nNpcIdex].m_pDropRate = m_pItemDropRate;
			//printf("--[已经存在TXT爆率]:%s -- \n", nDropRateFiled);
			return 1;
		} 
		else
		{//否则新加
			KDropScriptNode newDropNode;
			strcpy(newDropNode.m_szFileName,nDropRateFiled);
			//printf("--[新增加TXT爆率A]:%s --\n", nDropRateFiled);
			newDropNode.m_pItemDropRate = g_GenItemDropRate(nDropRateFiled);// 分配内存 加载爆率文件  返回 pnewDrop 指针
			g_DropBinTree.Insert(newDropNode);

			m_pItemDropRate           = newDropNode.m_pItemDropRate;
			Npc[nNpcIdex].m_pDropRate = m_pItemDropRate;   //把这个爆率赋值给NPC
			
			if (Npc[nNpcIdex].m_pDropRate==NULL)
			{
				//printf("---------[爆率数量为0,加载为空:%s]-------- \n",nDropRateFiled);
				return 0;
			}			
			return 1;
		}  
		/*
		m_pItemDropRate = ::g_GetItemDropRate(nDropRateFiled);
		Npc[nNpcIdex].m_pDropRate = m_pItemDropRate;

		if (Npc[nNpcIdex].m_pDropRate==NULL)
		{
			//printf("---------[爆率数量为0,加载为空:%s]-------- \n",nDropRateFiled);
			m_pItemDropRate=NULL;
			return 0;
		}			
			return 1;*/
	}
	catch (...)
	{	
		//printf("-------[爆率异常,加载为空:%s]------ \n",nDropRateFiled);
		Npc[nNpcIdex].m_pDropRate=NULL;
		m_pItemDropRate=NULL;
	}
	return 0;

 
}


//修改爆率
int	KNpcTemplate::gmUpdataDropRate(int nNpcIdex,char * nDropRateFiled)
{

	//KItemDropRate m_XpItemDropRate;
	if (!nDropRateFiled || !nDropRateFiled[0])
		return FALSE;

	strlwr(nDropRateFiled);
	/*
	DWORD nRateFile=g_FileName2Id(nDropRateFiled);

	if (g_ItemDropRateMap.count(nRateFile) > 0)
	{
		g_ItemDropRateMap.erase(nRateFile);
	} */

	KDropScriptNode DropNodeNew;
    strcpy(DropNodeNew.m_szFileName,nDropRateFiled);
	g_DropBinTree.Delete(DropNodeNew); //将其删除然后重新加载

	//g_LoadItemDropRate(nDropRateFiled);

	DropNodeNew.m_pItemDropRate = g_GenItemDropRate(nDropRateFiled);// 分配内存 加载爆率文件  返回 pnewDrop 指针
	m_pItemDropRate = DropNodeNew.m_pItemDropRate;
	//将新的爆率加入节点
	g_DropBinTree.Insert(DropNodeNew);

	Npc[nNpcIdex].m_pDropRate = m_pItemDropRate;
	
	if (Npc[nNpcIdex].m_pDropRate==NULL)
	{
		m_pItemDropRate = NULL;
		Npc[nNpcIdex].m_pDropRate = NULL;
		printf("---------[爆率数量为0:%s,加载为空]-------- \n",nDropRateFiled);
	}
	/*if (m_pItemDropRate)
	{
		//delete m_pItemDropRate->pItemParam;
		//m_pItemDropRate->pItemParam=NULL;
		//delete m_pItemDropRate;
		g_ItemDropRateMap._FreeNode(m_pItemDropRate);
		m_pItemDropRate=NULL;
	} */
	//ZeroMemory(&m_XpItemDropRate,sizeof(m_XpItemDropRate));
	
	return TRUE;

}



#endif	//服务器端结束

BOOL KNpcTemplate::InitNpcBaseData(int nNpcTemplateId)
{
	if (nNpcTemplateId < 0 )
		return FALSE;
	int nNpcTempRow = nNpcTemplateId + 2;

	g_NpcSetting.GetString(nNpcTempRow, "Name","", Name, sizeof(Name));

	if (nGameVersion!=1)
	{//如果版本不是1的话 开始替换名字   
		//if (strNpcInfo.count(Name))
		//{//如果存在就替换掉
		//	sprintf(Name,strNpcInfo[Name].c_str());
		//}
		//sprintf(Object[nFreeNo].m_szName,strObjInfo[nDataID + 1].c_str());
	}
	g_NpcSetting.GetInteger(nNpcTempRow, "Kind", 0, (int *)&m_Kind);
	g_NpcSetting.GetInteger(nNpcTempRow, "Camp", 0, &m_Camp);
	g_NpcSetting.GetInteger(nNpcTempRow, "Series", 0, &m_Series);
	
	g_NpcSetting.GetInteger(nNpcTempRow, "HeadImage",	0, &m_HeadImage);
	g_NpcSetting.GetInteger(nNpcTempRow, "ClientOnly",	0, &m_bClientOnly);
	g_NpcSetting.GetInteger(nNpcTempRow, "CorpseIdx",	0, &m_CorpseSettingIdx);  //尸体编号
	
	g_NpcSetting.GetInteger(nNpcTempRow, "DeathFrame",	12, &m_DeathFrame); //死亡帧数
	g_NpcSetting.GetInteger(nNpcTempRow, "WalkFrame",	15, &m_WalkFrame);
	g_NpcSetting.GetInteger(nNpcTempRow, "RunFrame",	15, &m_RunFrame);
	g_NpcSetting.GetInteger(nNpcTempRow, "HurtFrame",	10, &m_HurtFrame);
	g_NpcSetting.GetInteger(nNpcTempRow, "WalkSpeed",	5, &m_WalkSpeed);
	g_NpcSetting.GetInteger(nNpcTempRow, "AttackSpeed",	20, &m_AttackFrame); //外功攻击速度
	g_NpcSetting.GetInteger(nNpcTempRow, "CastSpeed",	20, &m_CastFrame);   //内功攻击速度
	g_NpcSetting.GetInteger(nNpcTempRow, "RunSpeed",	10, &m_RunSpeed);
	g_NpcSetting.GetInteger(nNpcTempRow, "StandFrame",	15, &m_StandFrame);
	g_NpcSetting.GetInteger(nNpcTempRow, "StandFrame1", 15, &m_StandFrame1);
	g_NpcSetting.GetInteger(nNpcTempRow, "Stature",		0,  &m_nStature);

#ifdef _SERVER	
	g_NpcSetting.GetInteger(nNpcTempRow, "Treasure",		0, &m_Treasure);   // Npc掉落装备的数量
	g_NpcSetting.GetInteger(nNpcTempRow, "AIMode",	        0, &m_AiMode);
	g_NpcSetting.GetInteger(nNpcTempRow, "AIParam1",	0, &m_AiParam[0]);
	g_NpcSetting.GetInteger(nNpcTempRow, "AIParam2",	0, &m_AiParam[1]);
	g_NpcSetting.GetInteger(nNpcTempRow, "AIParam3",	0, &m_AiParam[2]);
	g_NpcSetting.GetInteger(nNpcTempRow, "AIParam4",	0, &m_AiParam[3]);
	g_NpcSetting.GetInteger(nNpcTempRow, "AIParam5",	0, &m_AiParam[4]);
	g_NpcSetting.GetInteger(nNpcTempRow, "AIParam6",	0, &m_AiParam[5]);
	g_NpcSetting.GetInteger(nNpcTempRow, "AIParam7",	0, &m_AiParam[6]);
	g_NpcSetting.GetInteger(nNpcTempRow, "AIParam8",	0, &m_AiParam[7]);
	g_NpcSetting.GetInteger(nNpcTempRow, "AIParam9",	0, &m_AiParam[8]);
	//g_NpcSetting.GetInteger(nNpcTempRow, "AIParam10",	300, &m_AiParam[9]);

	g_NpcSetting.GetInteger(nNpcTempRow, "FireResistMax",	0, &m_FireResistMax);
	g_NpcSetting.GetInteger(nNpcTempRow, "ColdResistMax",	0, &m_ColdResistMax);
	g_NpcSetting.GetInteger(nNpcTempRow, "LightResistMax",	0, &m_LightResistMax);
	g_NpcSetting.GetInteger(nNpcTempRow, "PoisonResistMax",	0, &m_PoisonResistMax);
	g_NpcSetting.GetInteger(nNpcTempRow, "PhysicsResistMax",0, &m_PhysicsResistMax);
	g_NpcSetting.GetInteger(nNpcTempRow, "ActiveRadius", 30, &m_ActiveRadius);
	g_NpcSetting.GetInteger(nNpcTempRow, "VisionRadius", 40, &m_VisionRadius);
	//g_NpcSetting.GetInteger(nNpcTempRow, "AuraSkillId", 0, &m_VisionRadius);
		
	char szDropFile[256]={0};
	//g_NpcSetting.GetString(nNpcTempRow, "DropRateFile", "", szDropFile, sizeof(szDropFile)); //爆率文件
	//strcpy(m_szDropRateFile, szDropFile);
	if  (strstr(szDropFile,".ini"))
		 ZeroMemory(szDropFile,sizeof(szDropFile));

  KDropScriptNode DropNode;
  if (strstr(szDropFile,".ini"))	
  { 
	 strlwr(szDropFile);  //转换成小写
	 strcpy(DropNode.m_szFileName, szDropFile);
	 m_pItemDropRate = ::g_GenItemDropRate(szDropFile);
	 if (g_DropBinTree.Find(DropNode))  //查找爆率树 如果有
	 { 
		m_pItemDropRate = DropNode.m_pItemDropRate;  
	 } 
	 else
	 { 
		KDropScriptNode newDropNode;
		strcpy(newDropNode.m_szFileName,szDropFile);
		newDropNode.m_pItemDropRate = g_GenItemDropRate(szDropFile);// 加载爆率文件  返回 pnewDrop 指针
		g_DropBinTree.Insert(newDropNode);
		m_pItemDropRate = newDropNode.m_pItemDropRate;
	 } 
  }
  else
  {
	    m_pItemDropRate = NULL;
  }

	int nAIMaxTime = 0;
	g_NpcSetting.GetInteger(nNpcTempRow, "AIMaxTime", 25, (int*)&nAIMaxTime);
	m_AIMAXTime = (BYTE)nAIMaxTime;
	
	g_NpcSetting.GetInteger(nNpcTempRow, "HitRecover", 0, &m_HitRecover);
	g_NpcSetting.GetInteger(nNpcTempRow, "ReviveFrame", 2400, &m_ReviveFrame);
	int m_nIsRevive;
	g_NpcSetting.GetInteger(nNpcTempRow, "IsRevive", 1, &m_nIsRevive);//是否重生
    m_IsRevive=DWORD(m_nIsRevive);
	char szLevelScript[MAX_PATH];
	g_NpcSetting.GetString(nNpcTempRow, "LevelScript", "", szLevelScript, MAX_PATH);
	if (!szLevelScript[0])
		m_dwLevelSettingScript = 0;
	else
	{
#ifdef WIN32
		_strlwr(szLevelScript);
#else
		for (int nl = 0; szLevelScript[nl]; ++nl)
			if (szLevelScript[nl] >= 'A' && szLevelScript[nl] <= 'Z')
				szLevelScript[nl] += 'a' - 'A';
#endif
		m_dwLevelSettingScript = g_FileName2Id(szLevelScript);
	}

#else
	m_PifengType = 0;
	g_NpcSetting.GetInteger(nNpcTempRow, "ArmorType", 0, &m_ArmorType);
	g_NpcSetting.GetInteger(nNpcTempRow, "HelmType", 0, &m_HelmType);
	g_NpcSetting.GetInteger(nNpcTempRow, "WeaponType", 0, &m_WeaponType);
	g_NpcSetting.GetInteger(nNpcTempRow, "HorseType", -1, &m_HorseType);
	g_NpcSetting.GetInteger(nNpcTempRow, "RideHorse",0, &m_bRideHorse);
	g_NpcSetting.GetString(nNpcTempRow, "ActionScript", "", ActionScript,sizeof(ActionScript));
	g_NpcSetting.GetString(nNpcTempRow, "LevelScript", "", m_szLevelSettingScript, 100);
#endif

	return TRUE;
	
}
//从脚本中获取NPC各项原始数据
BOOL KNpcTemplate::InitNpcLevelData(int nNpcTemplateId, KLuaScript * pLevelScript, int nLevel)
{
	if (nNpcTemplateId < 0 || nLevel <= 0 || (!pLevelScript)) 
		return FALSE;
	int nNpcTempRow = nNpcTemplateId + 2;

	int	 nTopIndex = 0;

	m_nLevel = nLevel;

    nTopIndex=pLevelScript->SafeCallBegin();

	{
		m_NpcSettingIdx = nNpcTemplateId;

#ifdef _SERVER				
		//技能
		char szValue1[MAX_VALUE_LEN];
		char szValue2[MAX_VALUE_LEN];
		int  SKillID = 0,nSkillLevel=1;
        //SkillString2Id(szValue1)
	//	g_NpcSetting.GetString(nNpcTempRow, "Skill1",	"", szValue1, MAX_VALUE_LEN);
		g_NpcSetting.GetInteger(nNpcTempRow, "Skill1",0, &SKillID);
		g_NpcSetting.GetString(nNpcTempRow, "Level1", "", szValue2, sizeof(szValue2));
		if (SKillID>0 && szValue2[0])//SkillString2Id(szValue1)
		{
			   nSkillLevel=(int)GetNpcLevelDataFromScript(pLevelScript,"Level1", nLevel, szValue2,m_Series);
			
			 if (nSkillLevel>63) 
				nSkillLevel=63;
			if (nSkillLevel<=0)
				nSkillLevel=1;

			   m_SkillList.SetNpcSkill(1,SKillID,nSkillLevel);  //从脚本获取信息
		}
		//g_NpcSetting.GetString(nNpcTempRow, "Skill2",	"", szValue1, MAX_VALUE_LEN);
		g_NpcSetting.GetInteger(nNpcTempRow, "Skill2",0, &SKillID);
		g_NpcSetting.GetString(nNpcTempRow, "Level2", "", szValue2, sizeof(szValue2));
		if (SKillID>0 && szValue2[0])
		{
			nSkillLevel=(int)GetNpcLevelDataFromScript(pLevelScript,"Level2", nLevel, szValue2,m_Series);
			if (nSkillLevel>63) 
				nSkillLevel=63;
			if (nSkillLevel<=0)
				nSkillLevel=1;

			m_SkillList.SetNpcSkill(2,SKillID,nSkillLevel); 
		}
		//g_NpcSetting.GetString(nNpcTempRow, "Skill3",	"", szValue1, MAX_VALUE_LEN);
		g_NpcSetting.GetInteger(nNpcTempRow, "Skill3",0, &SKillID);
		g_NpcSetting.GetString(nNpcTempRow, "Level3", "", szValue2, sizeof(szValue2));
		if (SKillID>0 && szValue2[0])
		{
			nSkillLevel=(int)GetNpcLevelDataFromScript(pLevelScript,"Level3", nLevel, szValue2,m_Series);
			if (nSkillLevel>63) 
				nSkillLevel=63;
			if (nSkillLevel<=0)
				nSkillLevel=1;
			m_SkillList.SetNpcSkill(3, SKillID,nSkillLevel); 
		}
		//g_NpcSetting.GetString(nNpcTempRow, "Skill4",	"", szValue1, MAX_VALUE_LEN);
		g_NpcSetting.GetInteger(nNpcTempRow, "Skill4",0, &SKillID);
		g_NpcSetting.GetString(nNpcTempRow, "Level4", "", szValue2, sizeof(szValue2));
		if (SKillID>0 && szValue2[0])
		{
			nSkillLevel=(int)GetNpcLevelDataFromScript(pLevelScript,"Level4", nLevel, szValue2,m_Series);
			if (nSkillLevel>63) 
				nSkillLevel=63;
			if (nSkillLevel<=0)
				nSkillLevel=1;

			m_SkillList.SetNpcSkill(4,SKillID,nSkillLevel); 
		}
		//光环技能
		g_NpcSetting.GetInteger(nNpcTempRow, "AuraSkillId",0, &SKillID);
		g_NpcSetting.GetString(nNpcTempRow, "AuraSkillLevel", "", szValue2, sizeof(szValue2));

		if (SKillID>0 && szValue2[0])
		{
			nSkillLevel=(int)GetNpcLevelDataFromScript(pLevelScript,"AuraSkillLevel", nLevel, szValue2,m_Series);
			if (nSkillLevel>63) 
				nSkillLevel=63;
			if (nSkillLevel<=0)
				nSkillLevel=1;

			if (m_SkillList.SetNpcSkill(5,SKillID,nSkillLevel))
		       Npc[m_SkillList.m_nNpcIndex].SetAuraSkill(SKillID);

			//printf("光环技能ID:%d,等级:%d--\n",SKillID,nSkillLevel);

		}
		//被动技能 免疫技能
		g_NpcSetting.GetInteger(nNpcTempRow, "PasstSkillId",0,&SKillID);
		g_NpcSetting.GetString(nNpcTempRow, "PasstSkillLevel", "", szValue2, sizeof(szValue2));
		if (SKillID>0 && szValue2[0])
		{
			nSkillLevel=(int)GetNpcLevelDataFromScript(pLevelScript,"PasstSkillLevel", nLevel, szValue2,m_Series);
			
			if (nSkillLevel>63) 
				nSkillLevel=63;
			if (nSkillLevel<=0)
				nSkillLevel=1;
			m_SkillList.SetNpcSkill(6,SKillID,nSkillLevel);
			//printf("被动技能ID:%d,等级:%d--\n",SKillID,nSkillLevel);
		}
		double nParam1;
		double nParam2;
		double nParam3;
		double nParam=1;
		
		g_NpcSetting.GetDouble(nNpcTempRow, "ExpParam",1, &nParam);
		g_NpcSetting.GetDouble(nNpcTempRow, "ExpParam1",0, &nParam1);
		g_NpcSetting.GetDouble(nNpcTempRow, "ExpParam2",0, &nParam2);
		g_NpcSetting.GetDouble(nNpcTempRow, "ExpParam3",0, &nParam3);

		//经验倍率GetNpcLevelDataFromScriptB  GetNpcKeyData
	    m_Experience = nParam*GetNpcKeyDataFromScript(pLevelScript, "Exp", nLevel, nParam1, nParam2, nParam3,m_Series)/100;
		if (m_Experience <= 0) m_Experience = 10;

		//最大生命值 GetNpcKeyData ---会导致 mainloop 错误
//		g_NpcSetting.GetInteger(nNpcTempRow, "LifeParam",1, &nParam);   //基本数据
		g_NpcSetting.GetDouble(nNpcTempRow, "LifeParam1",0, &nParam1);
		g_NpcSetting.GetDouble(nNpcTempRow, "LifeParam2",0, &nParam2);
		g_NpcSetting.GetDouble(nNpcTempRow, "LifeParam3",0, &nParam3);

      
	    m_nLifeMax =(int)(GetNpcKeyDataFromScript(pLevelScript, "Life", nLevel, nParam1, nParam2, nParam3,m_Series));
		//char msg[64]:
		//printf("生命最大值:%d \n",m_nLifeMax);
		if (m_nLifeMax <=0) 
			m_nLifeMax=100*nLevel;

		//生命回复
		g_NpcSetting.GetString(nNpcTempRow, "LifeReplenish", "", szValue1, sizeof(szValue1));
		m_LifeReplenish = GetNpcLevelDataFromScript(pLevelScript, "LifeReplenish", nLevel, szValue1,m_Series); 
		if (m_LifeReplenish < 0)  m_LifeReplenish = 1;
		//命中
		g_NpcSetting.GetDouble(nNpcTempRow, "ARParam",1, &nParam);
		g_NpcSetting.GetDouble(nNpcTempRow, "ARParam1",0, &nParam1);
		g_NpcSetting.GetDouble(nNpcTempRow, "ARParam2",0, &nParam2);
		g_NpcSetting.GetDouble(nNpcTempRow, "ARParam3",0, &nParam3);

		m_AttackRating = nParam * GetNpcKeyDataFromScript(pLevelScript, "AttackRating", nLevel, nParam1, nParam2, nParam3,m_Series) / 100;
		if (m_AttackRating <= 0) m_AttackRating = 100;
        
        //防御
		g_NpcSetting.GetDouble(nNpcTempRow, "DefenseParam", 1, &nParam);
		g_NpcSetting.GetDouble(nNpcTempRow, "DefenseParam1", 0, &nParam1);
		g_NpcSetting.GetDouble(nNpcTempRow, "DefenseParam2", 0, &nParam2);
		g_NpcSetting.GetDouble(nNpcTempRow, "DefenseParam3", 0, &nParam3);

		m_Defend = nParam *GetNpcKeyDataFromScript(pLevelScript, "Defense", nLevel, nParam1, nParam2, nParam3,m_Series) / 100;
        if (m_Defend <= 0) m_Defend = 10;

        //基本最低攻击
		g_NpcSetting.GetDouble(nNpcTempRow, "MinDamageParam", 1, &nParam);
		g_NpcSetting.GetDouble(nNpcTempRow, "MinDamageParam1", 0, &nParam1);
		g_NpcSetting.GetDouble(nNpcTempRow, "MinDamageParam2", 0, &nParam2);
		g_NpcSetting.GetDouble(nNpcTempRow, "MinDamageParam3", 0, &nParam3);

		m_PhysicsDamage.nValue[0] = nParam * GetNpcKeyDataFromScript(pLevelScript,"MinDamage", nLevel, nParam1, nParam2, nParam3,m_Series) / 100;
        if (m_PhysicsDamage.nValue[0] <= 0) m_PhysicsDamage.nValue[0] = 1;
		//基本最大攻击
		g_NpcSetting.GetDouble(nNpcTempRow, "MaxDamageParam", 1, &nParam);
		g_NpcSetting.GetDouble(nNpcTempRow, "MaxDamageParam1", 0, &nParam1);
		g_NpcSetting.GetDouble(nNpcTempRow, "MaxDamageParam2",0, &nParam2);
		g_NpcSetting.GetDouble(nNpcTempRow, "MaxDamageParam3",0, &nParam3);


		m_PhysicsDamage.nValue[2] = nParam * GetNpcKeyDataFromScript(pLevelScript,"MaxDamage", nLevel, nParam1, nParam2, nParam3,m_Series) / 100;
        if (m_PhysicsDamage.nValue[2] <= 0) m_PhysicsDamage.nValue[2] = 5;
		//--------------------------------------------------------------------------------------------
		g_NpcSetting.GetString(nNpcTempRow, "RedLum","", szValue1,sizeof(szValue1));
		m_RedLum = GetNpcLevelDataFromScript(pLevelScript, "RedLum", nLevel, szValue1,m_Series);
		
		g_NpcSetting.GetString(nNpcTempRow, "GreenLum","", szValue1, sizeof(szValue1));
		m_GreenLum = GetNpcLevelDataFromScript(pLevelScript, "GreenLum", nLevel, szValue1,m_Series);
		
		g_NpcSetting.GetString(nNpcTempRow, "BlueLum", "", szValue1, sizeof(szValue1));
		m_BlueLum = GetNpcLevelDataFromScript(pLevelScript, "BlueLum", nLevel, szValue1,m_Series);
		//---------------------------------------------------------------------------------------------
		g_NpcSetting.GetString(nNpcTempRow, "FireResist", "", szValue1, sizeof(szValue1));
		m_FireResist = GetNpcLevelDataFromScript(pLevelScript, "FireResist", nLevel, szValue1,m_Series);

		g_NpcSetting.GetString(nNpcTempRow, "ColdResist", "", szValue1, sizeof(szValue1));
		m_ColdResist = GetNpcLevelDataFromScript(pLevelScript, "ColdResist", nLevel, szValue1,m_Series);

		g_NpcSetting.GetString(nNpcTempRow, "LightResist", "", szValue1, sizeof(szValue1));
		m_LightResist = GetNpcLevelDataFromScript(pLevelScript, "LightResist", nLevel, szValue1,m_Series);
		
		g_NpcSetting.GetString(nNpcTempRow, "PoisonResist", "", szValue1, sizeof(szValue1));
		m_PoisonResist = GetNpcLevelDataFromScript(pLevelScript, "PoisonResist", nLevel, szValue1,m_Series);

		g_NpcSetting.GetString(nNpcTempRow, "PhysicsResist", "", szValue1, sizeof(szValue1));
		m_PhysicsResist = GetNpcLevelDataFromScript(pLevelScript, "PhysicsResist", nLevel, szValue1,m_Series);
	    // printf("等级:%d,生命:%d,防御:%d,%d,%d,%d,%d,攻击:%d,%d,经验倍率:%d,回复率:%d,栈元素:%d \n",nLevel,m_nLifeMax,m_FireResist,m_ColdResist,m_LightResist,m_PoisonResist,m_PhysicsResist,m_PhysicsDamage.nValue[0],m_PhysicsDamage.nValue[2],m_Experience,m_LifeReplenish,nTopIndex);
#endif
									
#ifndef _SERVER
		double ncParam1, ncParam2,ncParam3, ncParam = 100;
		g_NpcSetting.GetDouble(nNpcTempRow, "LifeParam1", 0, &ncParam1);
		g_NpcSetting.GetDouble(nNpcTempRow, "LifeParam2", 0, &ncParam2);
		g_NpcSetting.GetDouble(nNpcTempRow, "LifeParam3", 0, &ncParam3);

		m_nLifeMax =(int) (GetNpcKeyDataFromScript(pLevelScript, "Life", nLevel, ncParam1, ncParam2, ncParam3,m_Series));
		
		if (m_nLifeMax <= 0) m_nLifeMax = 100;
#endif
	}
	nTopIndex=pLevelScript->SafeCallBegin();
	pLevelScript->SafeCallEnd(nTopIndex);

	return TRUE;
}
//读取脚本 从脚本获取返回值
int KNpcTemplate::GetNpcLevelDataFromScript(KLuaScript * pScript, char * szDataName, int nLevel, char * szParam,int nSeries)
{
	int nTopIndex = 0;
	int nReturn = 0;
	if (pScript==NULL || szParam == NULL|| szParam[0] == 0 /*|| strlen(szParam) < 3*/)
	{
		return 0;
	}

	nTopIndex=pScript->SafeCallBegin();
	pScript->CallFunction("GetNpcLevelData",1, "ddss",nSeries,nLevel,szDataName,szParam);   // 函数命令
	nTopIndex = Lua_GetTopIndex(pScript->m_LuaState);
	nReturn = (int) Lua_ValueToNumber(pScript->m_LuaState, nTopIndex);  //返回数字
	nTopIndex=pScript->SafeCallBegin();
	pScript->SafeCallEnd(nTopIndex);
	return nReturn;
}
//获取脚本NPC 经验 等级 生命等设置
int KNpcTemplate::GetNpcKeyDataFromScript(KLuaScript * pScript, char * szDataName, int nLevel,double nParam1, double nParam2,double nParam3,int nSeries)
{
	int nTopIndex = 0;
	int nReturn = 0;
	if (pScript==NULL || nLevel<=0)
	{
		return 0;
	}
	nTopIndex=pScript->SafeCallBegin();
	pScript->CallFunction("GetNpcKeyData", 1, "ddsnnn",nSeries,nLevel,szDataName,nParam1,nParam2,nParam3);
	nTopIndex = Lua_GetTopIndex(pScript->m_LuaState);
	nReturn = (int)Lua_ValueToNumber(pScript->m_LuaState, nTopIndex);
	nTopIndex=pScript->SafeCallBegin();
	pScript->SafeCallEnd(nTopIndex);
	return nReturn;
}

//解析中文技能名 在技能SKILLS 文本中的位置 获取技能的ID
int KNpcTemplate::SkillString2Id(char * szSkillString)
{
	if (!szSkillString[0]) return 0;
	int nSkillNum = g_OrdinSkillsSetting.GetHeight();
	char szSkillName[100];
	for (int i = 0 ;  i < nSkillNum; ++i)
	{
		g_OrdinSkillsSetting.GetString(i + 2, "SkillName", "", szSkillName, sizeof(szSkillName));
	//	if (g_StrCmp(szSkillString, szSkillName))  //技能相等的 相同的  原来的
		if (strcmpi(szSkillString, szSkillName) == 0)
		{
			int nSkillId = 0;
			g_OrdinSkillsSetting.GetInteger(i + 2, "SkillId", 0, &nSkillId);
			return nSkillId;
		}
	}

	printf("---无法解析Npc.txt调用的技能[%s]魔法的Id,请检查!--- \n", szSkillString);
	return 0;
}




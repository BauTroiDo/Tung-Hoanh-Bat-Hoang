//#include "kstdafx.h"
#ifndef S3DBINTERFACE_H
#define S3DBINTERFACE_H
#define IN
#define OUT
#include <map>
#include <string>
#include <vector>
#include "KProtocol.h"
//#define SAVE_ITEM_MAGICATTRIB_TO_DB

class KTabFileCtrl;
class KIniFile;

#pragma pack( push, 1 )

enum ROLE_LOG
{
	log_create_role = 1,
	log_begin_game,
	log_save_game,
};

enum 
{
	create_role,  //if already have this rolename then msg error!;
	save_role,	  //if havent this rolename in db ,then msg error!	
};

enum
{
	must_norole,
	must_haverole,
	nolimited,
};

struct TIntDBValue
{
	BYTE btUnitType;
};
/*
//同伴数量
struct TongBanInfo
{
	int inum1;
	int inum2;
	int inum3;
	int inum4;
	int inum5;
};
*/
/*//经脉等级
struct JinMaiInfo
{
	BYTE imparam1;
	BYTE imparam2;
	BYTE imparam3;
	BYTE imparam4;
	BYTE imparam5;
	BYTE imparam6;
	BYTE imparam7;
	BYTE imparam8;
	BYTE imparam9;
	BYTE imparam10;
	BYTE imparam11;
	BYTE imparam12;
	BYTE imparam13;
	BYTE imparam14;
	BYTE imparam15;
};
*/
struct FumoInfo
{
	int ifparam;
	int ifsetno;
	int ifskill;
	int ilevel;
};

struct TRoleBaseInfo
{
	//RoleBaseInfo
	UINT	dwId;
	char	szName[NAME_LENGHT];    //人物角色名字
	bool	bSex;
	char	szAlias[NAME_LENGHT];   //未用到	徒弟还是师傅的名称
	char	caccname[ACC_LENGTH];  //账号名
	BYTE	nFirstSect;
	BYTE	nSect;
	BYTE	cFightMode;
	BYTE	cUseRevive;
	BYTE	cIsExchange;
// --------------------- add by spe 2003/07/19 -----------------------
	BYTE	cPkStatus;
// -------------------------------------------------------------------
	int		ijoincount;
	int		isectrole;
	int		igroupcode;
	int		igrouprole;
	int		irevivalid;
	int		irevivalx;
	int		irevivaly;
	int		ientergameid;
	int		ientergamex;
	int		ientergamey;
	char	cpartnercode[NAME_LENGHT];  //未用到	 用来记录修改的名字
	int		isavemoney;	       //储物箱
	int		iservermon;		   //包袱
	int		iclientx;		   //金币
	int		ifiveprop;
	int		iteam;
	int		ifightlevel;
	int 	fightexp;
	int		ileadlevel;
	int		ileadexp;
	int		iliveexp;
	int		ipower;
	int		iagility;
	int		iouter;
	int		iinside;
	int		iluck;  //人物的幸运值
	int 	imaxlife;
	int	    imaxstamina;
	int	    imaxinner;
	int		imaxnuqi;  //怒气最大值
// --------------------- add by spe 2003/07/19 -----------------------
	int	    icurlife;
	int	    icurstamina;
	int   	icurinner;
	int		ipkvalue;  //PK值
	int		isvip;     //当前会员
// -------------------------------------------------------------------
	int		ileftprop; //潜能点
	int		ileftfight;//技能点
	int		ileftlife;
	int		ifinishgame;
	short	iarmorres;
	short	iweaponres;
	short	ihelmres;
// --------------------- 门派 和世界排名 调用 2003/08/13 -----------------------
	int		nSectStat;
	int		nWorldStat;
// --------------------- 杀人数量 排名调用 2003/08/16 -----------------------
	int		nKillPeopleNumber;
// --------------------- add by spe 2003/09/09 -----------------------
	int		nForbiddenFlag;
	UINT	dwTongID;	
// new add by king
	int		ireputevalue;
	int		ifuyuanvalue;
	int		irebornvalue;
	int		ipassrole;//锁屏密码
	int		irankrole;
	int		iexitemrole;
	int		iexboxrole;
	int     icjtaskid;
//JinMaiInfo  ijinmai[10];
    FumoInfo	ifumo[5];
	int     iJinMaiBingJia;
	int     iZhenYuan;
	BYTE     isCreatTongBan;
	BYTE     isWaiGua;      //使用外挂标示(已经取消) 用来作为 一机多区记录
	BYTE     isSerLock;     //是否被锁
	char     szchenhao[NAME_LENGHT]; //称号
	int      iPltitle;
	int      iPltitleTime;
	int      iRoleParm1;
	int      iRoleParm2;
	int      iRoleParm3;
	int      iRoleParm4;
	int      iRoleParm5;          //记录改名的次数
	int      iRoleParm6;          //临时转服记录
	unsigned int  iRoleParm7;     //记录转服的时间
	unsigned int  iRoleParm8;     //记录建立角色的时间
	int      iRoleParm9;          //0正常 1是师傅 2是徒弟 3
	int      iRoleParm10;         //等级上限记录
	int      iRoleParm11;
	int      iRoleParm12;
	int      iRoleParm13;
	int      iRoleParm14;

};
//角色数据
struct TRoleData
{
	TRoleBaseInfo BaseInfo;
	UINT	dwVersion;
	bool	bBaseNeedUpdate;//通知是否需要更新
	//当存取时如果Offset等于-1，表示不更新该数据
	//Additional Info
	short	nFightSkillCount;   //战斗技能数量
	short	nLiveSkillCount;    //生活技能
	BYTE nTaskCount;         //任务数量
	short	 nItemCount;         //物品数量
	short	 nFriendCount;
	int		dwTaskOffset;
	int		dwLSkillOffset;
	int		dwFSkillOffset;
	int		dwItemOffset;
	int		dwFriendOffset;
	int		dwDataLen;
	BYTE	pBuffer[1];
};

//任务数据库
struct TDBTaskData
{
	int	    m_nTaskId;   // 任务号
	int		m_nTaskValue; // 任务值
};

//数据库结构物品属性列表
struct TDBItemData
{
	//BaseInfo
	int iid;
	int iequipclasscode;
	int ilocal;
	int ix;
	int iy;
	int iequipcode;
	int idetailtype;
	int iparticulartype;
	int ilevel;
	int iseries;
	int iequipversion;
	UINT irandseed;
	int iparam2;
	int iparam3;
	int iparam5;
	int iparam4;
	int iparam6;
	int iparam1;
	int iparamr2;// 熔炼
	int iparamr3;
	int iparamr5;
	int iparamr4;
	int iparamr6;
	int iparamr1;
	int ilucky;  //装备的幸运值
	int iidentify;
	int	idurability;
	int igoldid;
	int	istacknum;
	int ienchance;
	BYTE ipoint;
	int iyear;
	int imonth;
	int iday;
	int ihour;
	BYTE irongNum; //可熔炼数量
	BYTE iwengangPin;
	int  ibinfujiazhi;
    BYTE iRongpiont;
	BYTE iIsBang;
	int itradeprice;
	int ilockstate;
	int ilocktime;
	char iWonName[NAME_LENGHT]; //归属者
	BYTE iIsKuaiJie;
	int  imin;
    BYTE iIsMagic;//是否带魔法属性
	int iSkillType;//类型魔法
	int iparamj1;// 基本属性(可改变的)
	int iparamj2;
	int iparamj3;
	int iparamj5;
	int iparamj4;
	int iparamj6;
	int iparamj7;
	int iparamb1;// 宝石属性(可改变的)
	int iparamb2;
	int iparamb3;//
	int iparamb4;
	int iparamb5;
	int iparamb6;
	int iswhere;
//	int iIsUse;
	int isplatina;
};		

struct TDBFriendsData
{
	short FriendCount;
	void * pBuffer;
	short Offset[1];
};

struct TDBSkillData_OLD
{
	short	m_nSkillId;
	short	m_nSkillLevel;
	int     m_nSkillExp;
};

struct TDBSkillData
{
	short	m_nSkillId;
	short	m_nSkillLevel;
	int     m_nSkillExp;
	int     m_nExpSkillPonit;  //修炼出来的技能点
};

enum 
{
/*==============================================================*/
/* Table: EquipBaseInfo                                         */
/*==============================================================*/
   EQUIPBASEINFO_IID                                             ,
   EQUIPBASEINFO_IMAINID                                        ,
   EQUIPBASEINFO_ICLASS                                      ,
   EQUIPBASEINFO_IVALUE1                        ,
   EQUIPBASEINFO_IVALUE2                        ,
   EQUIPBASEINFO_IVALUE3                        ,
/*==============================================================*/
/* TABLE: EQUIPEFFICINFO_                                        */
/*==============================================================*/
   EQUIPEFFICINFO_IID                                             ,
   EQUIPEFFICINFO_IMAINID                                        ,
   EQUIPEFFICINFO_ICLASS                                      ,
   EQUIPEFFICINFO_IVALUE1                        ,
   EQUIPEFFICINFO_IVALUE2                        ,
   EQUIPEFFICINFO_IVALUE3                        ,

/*==============================================================*/
/* TABLE: EQUIPREQUIREINFO_                                     */
/*==============================================================*/
   EQUIPREQUIREINFO_IID                                             ,
   EQUIPREQUIREINFO_IMAINID                                        ,
   EQUIPREQUIREINFO_ICLASS                                      ,
   EQUIPREQUIREINFO_IVALUE1                        ,
   EQUIPREQUIREINFO_IVALUE2                        ,
   EQUIPREQUIREINFO_IVALUE3                        ,
/*==============================================================*/
/* TABLE: EQUIPMENTS                                            */
/*==============================================================*/
   EQUIPMENTS_IID                                             ,
   EQUIPMENTS_CUSERCODE                                         ,
   EQUIPMENTS_IEQUIPCLASSCODE                                   ,
   EQUIPMENTS_ILOCAL                                      ,
   EQUIPMENTS_IX                                            ,
   EQUIPMENTS_IY                                            ,
   EQUIPMENTS_IEQUIPCODE                                     ,
   EQUIPMENTS_IDETAILTYPE                    ,
   EQUIPMENTS_IPARTICULARTYPE                ,
   EQUIPMENTS_ILEVEL                         ,
   EQUIPMENTS_ISERIES                        ,
   EQUIPMENTS_IEQUIPVERSION                  ,
   EQUIPMENTS_IRANDSEED                      ,
   EQUIPMENTS_IPARAM2                        ,
   EQUIPMENTS_IPARAM3                        ,
   EQUIPMENTS_IPARAM5                        ,
   EQUIPMENTS_IPARAM4                        ,
   EQUIPMENTS_IPARAM6                        ,
   EQUIPMENTS_IPARAM1                        ,
   EQUIPMENTS_ILUCKY                         ,
 
/*==============================================================*/
/* TABLE: FIGHTSKILL                                            */
/*==============================================================*/
   FIGHTSKILL_IID                                             ,
   FIGHTSKILL_CUSERCODE                                         ,
   FIGHTSKILL_IFIGHTSKILL                                 ,
   FIGHTSKILL_IFIGHTSKILLLEVEL                            ,

/*==============================================================*/
/* TABLE: FRIENDLIST_                                           */
/*==============================================================*/
   FRIENDLIST_IID                                             ,
   FRIENDLIST_CUSERCODE                                         ,
   FRIENDLIST_CFRIENDCODE                                       ,


/*==============================================================*/
/* TABLE: LIFESKILL                                             */
/*==============================================================*/

   LIFESKILL_IID                                             ,
   LIFESKILL_CUSERCODE                                         ,
   LIFESKILL_ILIFESKILL                                  ,
   LIFESKILL_ILIFESKILLLEVEL                             ,
/*==============================================================*/
/* TABLE: ROLEINFO                                             */
/*==============================================================*/
   ROLEINFO_IID                                             ,
   ROLEINFO_CUSERCODE                                         ,
   ROLEINFO_BSEX                                            ,
   ROLEINFO_CALIAS                         ,
   ROLEINFO_IFIRSTSECT                     ,
   ROLEINFO_ISECT                          ,
   ROLEINFO_IJOINCOUNT                     ,
   ROLEINFO_ISECTROLE                      ,
   ROLEINFO_IGROUPCODE                     ,
   ROLEINFO_IGROUPROLE                     ,
   ROLEINFO_IREVIVALID                     ,
   ROLEINFO_CPARTNERCODE                   ,
   ROLEINFO_ISAVEMONEY                                     ,
   ROLEINFO_IMONEY                                         ,
   ROLEINFO_IFIVEPROP                      ,
   ROLEINFO_ITEAM                          ,
   ROLEINFO_IFIGHTLEVEL                    ,
   ROLEINFO_FIGHTEXP                      ,
   ROLEINFO_ILEADLEVEL                     ,
   ROLEINFO_ILEADEXP                       ,
   ROLEINFO_ILIVEEXP                       ,
   ROLEINFO_IPOWER                         ,
   ROLEINFO_IAGILITY                       ,
   ROLEINFO_IOUTER                         ,
   ROLEINFO_IINSIDE                        ,
   ROLEINFO_ILUCK                          ,
   ROLEINFO_IMAXLIFE                       ,
   ROLEINFO_IMAXSTAMINA                    ,
   ROLEINFO_IMAXINNER                      ,
   ROLEINFO_ILEFTPROP                      ,
   ROLEINFO_ILEFTFIGHT                     ,
   ROLEINFO_ILEFTLIFE                      ,
   ROLEINFO_CACCNAME                       ,
   ROLEINFO_IFINISHGAME                    ,
   ROLEINFO_DLOGINTIME                     ,
   ROLEINFO_DBEGINDATE                     ,
   ROLEINFO_DENDDATE                       ,
   ROLEINFO_IREVIVALX                      ,
   ROLEINFO_IREVIVALY                      ,
   ROLEINFO_IARMORRES                      ,
   ROLEINFO_IWEAPONRES                     ,
   ROLEINFO_IHELMRES                       ,

/*==============================================================*/
/* TABLE: TASKLIST                                             */
/*==============================================================*/
   TASKLIST_IID                                             ,
   TASKLIST_CUSERCODE                                         ,
   TASKLIST_ITASKCODE                                   ,
   TASKLIST_IDEGREE                                     ,

};

//	Ini文件各字段定义----------------新建角色-------------------------------
#define SECTION_ROLE		"ROLE"	

#define SECTION_ITEMLIST	"ITEMS"	
#define SECTION_ITEM		"ITEM" // + id
#define KEY_COUNT			"COUNT"
#define KEY_BASELIST		"BLISTS"
#define KEY_EFFICLIST		"ELISTS"
#define KEY_REQUIRELIST		"RLISTS"

#define SECTION_BASEINFO	"IB" //IB1_2
#define SECTION_EFFICEINFO	"IE"
#define SECTION_REQUIREINFO	"IR"


#define SECTION_FRIENDLIST	"FRIENDS"
#define KEY_FRIEND			"F" // + id

#define SECTION_TASKLIST	"TASKS"
#define KEY_TASKID			"T" // + id
#define KEY_TASKVALUE		"V" // + id

#define SECTION_FIGHTSKILLLIST		"FSKILLS"
#define KEY_FIGHTSKILL		"S"
#define KEY_FIGHTSKILLLEVEL	"L"

#define SECTION_LIFESKILLLIST		"LSKILLS"
#define KEY_LIFESKILL				"S" // + id
#define	KEY_LIFESKILLLEVEL			"L" // + id
//------------------------------------------------------------------------------
#pragma warning (disable: 4512)
#pragma warning (disable: 4786)

#ifndef __linux
#ifdef S3DBINTERFACE_EXPORTS
#define S3DBI_API __declspec(dllexport)
#else
#define S3DBI_API __declspec(dllimport)
#endif
#else
#define S3DBI_API
#endif

//改回使用core\kprotocol.h里的S3DBI_RoleBaseInfo
/*struct S3DBI_RoleBaseInfo
{
	char szRoleName[32];
	int	 nSex;
	int	 nHelmType;
	int	 nArmorType;
	int	 nWeaponType;
	int	 nLevel;
};*/


struct S3DBI_DATAS
{
	char * strNames;
	char * strValues;
	int	   nDataNum;
};

///数据库表名
enum  S3DBI_TABLENAME
{
	TABLE_ACCOUNT,   //账号
	TABLE_CARD,      //卡
	TABLE_EQUIPBASEINFO, //基本信息
	TABLE_EQUIPEFFICINFO,
	TABLE_EQUIPMENT,     //物品
	TABLE_EQUIPREQUIREINFO,
	TABLE_FIGHTSKILL,
	TABLE_FRIEND,
	TABLE_LIFESKILL,
	TABLE_LIST,
	TABLE_ROLE,
	TABLE_TASK,
	TABLE_TASK_LIST,
};

enum S3DBI_LISTNAME
{
	LIST_TASK,
	LIST_FRIEND,
	LIST_FIGHT,
	LIST_LIFESKILL,
	LIST_EQUIPMENT,
	LIST_EQUIPMENTREQINFO,
	LIST_EQUIPMENTBASEINFO,
	LIST_EQUIPMENTEFFICINFO,
};
/*
extern DBTableKeyMap  g_RoleTableKeyMap;

typedef std::map<std::string, int>	DBTableKeyMap;

typedef std::map<int, std::string>	DBStringMap;
typedef std::map<int, int>			DBIntegerMap;
*/
/*struct TDBTaskData
{
	int m_nTaskId;
	int	m_nTaskValue;
};


struct TDBTableData
{
	DBStringMap			m_StringDatas;
	DBIntegerMap		m_IntegerDatas;
};
struct TDBEquiptData;
typedef std::vector<KMagicAttrib>		TMagicAttribArray;
typedef std::vector<TDBEquiptData>		TDBEquiptArray;
typedef std::vector<TDBSkillData>		TDBSkillArray;
typedef std::vector<std::string>		TDBFriendArray;
typedef std::vector<TDBTaskData>		TDBTaskArray;

extern DBTableKeyMap g_TableIntegerKeyMap;
extern DBTableKeyMap g_TableStringKeyMap;
struct TDBEquiptData : TDBTableData
{	
	TMagicAttribArray	m_mapRequestInfo;
	TMagicAttribArray	m_mapBaseInfo;
	TMagicAttribArray	m_mapEfficInfo;
};

struct TDBPlayerData :TDBTableData
{
	TDBEquiptArray		m_mapEquipt;
	TDBSkillArray		m_mapFightSkill;
	TDBSkillArray		m_mapLiveSkill;
	TDBFriendArray		m_mapFriend;
	TDBTaskArray		m_mapTask;
};

*/
#define S3_DB_TABLENAME  S3DBI_TABLENAME
S3DBI_API int S3DBI_InitDBInterface();			//初始化数据库引擎
S3DBI_API void S3DBI_ReleaseDBInterface();		//释放数据库引擎

//获得角色的信息并以Ini文件的格式放置
//S3DBI_API int	S3DBI_GetRoleInfo(IN char * strUser, IN OUT KIniFile * pIniFile);
S3DBI_API void *	S3DBI_GetRoleInfo(IN OUT BYTE * pRoleBuffer, IN char * strUser, IN OUT int &nBufLen);

//保存角色的信息，如果数据库不存在该玩家，则增加该玩家
//bAutoInsertWhenNoExistUser 设为TRUE时表示，如果需要保存的该玩家在数据库中并不存在则自动加入到数据库中，FALSE则不增加直接返回错误
//注意INI文件只须存放将需要改动的数据，不需改动的数据将自动保存原状。
S3DBI_API int	S3DBI_SaveRoleInfo(IN BYTE * pRoleBuffer, IN char * strUser, BOOL bAutoInsertWhenNoExistUser);
S3DBI_API void S3DBI_ShowDebugInfo(IN BYTE * pRoleBuffer, char * pShowString);

S3DBI_API BOOL S3DBI_IsRoleNameExisted(char * strUser);
S3DBI_API void S3DBI_SaveRoleLogData(const char * strUser, ROLE_LOG eLogType, time_t logtime = 0);
S3DBI_API time_t S3DBI_GetDataBaseSysTime();

//  eLogType标识日志的类型
//	log_create_role , 记录角色建立的时间
//	log_begin_game,   记录角色本次游戏登入的时间
//	log_save_game,	  记录角色本次游戏过程中上一次存档的时间
//  数据库保存存档的时间的目的是为了

S3DBI_API int S3DBI_GetRoleListOfAccount(char * szAccountName, S3DBI_RoleBaseInfo * RoleBaseList, int nMaxCount);

//删除角色
S3DBI_API int	S3DBI_DeleteRole(char * strUser);




S3DBI_API void*	S3DBI_GetList(S3DBI_LISTNAME listname,  char * strUser);
S3DBI_API void*	S3DBI_GetList(S3DBI_LISTNAME listname,  int iid);


//S3DBI_API int	S3DBI_GetListData(S3DBI_LISTNAME listname, void * pList, int i , ColumnAndValue &cav);
S3DBI_API int	S3DBI_GetCountOfList(S3DBI_LISTNAME listname, void * pList);
//S3DBI_API int	S3DBI_UpdateDataAtList(S3DBI_LISTNAME listname, void * pList, int i , ColumnAndValue &cav);
S3DBI_API int	S3DBI_DeleteDataAtList(S3DBI_LISTNAME listname, void * pList, int i );
//S3DBI_API int	S3DBI_InsertDataAtList(S3DBI_LISTNAME listname, void * pList, ColumnAndValue cav);
S3DBI_API void	S3DBI_ReleaseListBuffer(S3DBI_LISTNAME listname, void * pList);
//临时清单
S3DBI_API int	S3DBI_ClearList(S3DBI_LISTNAME listname, void * pList);

//S3DBI_API int	S3DBI_GetData(S3_DB_TABLENAME tablename, int nId, ColumnAndValue *pcav);
//S3DBI_API int	S3DBI_GetData(S3_DB_TABLENAME tablename, char * strUser , ColumnAndValue * pCav);
//S3DBI_API int	S3DBI_UpdateData(S3_DB_TABLENAME tablename, int nId, ColumnAndValue *pcav);
//S3DBI_API int	S3DBI_UpdateData(S3_DB_TABLENAME tablename, char * strUser, ColumnAndValue *pcav);
S3DBI_API int	S3DBI_DeleteData(S3_DB_TABLENAME tablename, int nId);
S3DBI_API int	S3DBI_DeleteData(S3_DB_TABLENAME tablename, char * strUser);
//S3DBI_API int	S3DBI_InsertData(S3_DB_TABLENAME tablename, int nId, ColumnAndValue cav);
//S3DBI_API int	S3DBI_InsertData(S3_DB_TABLENAME tablename, char * strUser, ColumnAndValue cav);


S3DBI_API int	S3DBI_GetListData(S3DBI_LISTNAME listname, void * pList, int i , KTabFileCtrl *pTabFile);
S3DBI_API int	S3DBI_DeleteDataAtList(S3DBI_LISTNAME listname, void * pList, int i );
S3DBI_API int	S3DBI_InsertDataAtList(S3DBI_LISTNAME listname, void * pList, KTabFileCtrl *pTabFile);
S3DBI_API int	S3DBI_GetData(S3_DB_TABLENAME tablename, int nId, KTabFileCtrl *pTabFile);
S3DBI_API int	S3DBI_GetData(S3_DB_TABLENAME tablename, char * strUser, KTabFileCtrl * pTabFile);
S3DBI_API int	S3DBI_UpdateData(S3_DB_TABLENAME tablename, int nId, KTabFileCtrl *pTabFile);
S3DBI_API int	S3DBI_UpdateData(S3_DB_TABLENAME tablename, char * strUser, KTabFileCtrl *pTabFile);
S3DBI_API int	S3DBI_InsertData(S3_DB_TABLENAME tablename,  KTabFileCtrl *pTabFile);

#pragma pack( pop )

#endif


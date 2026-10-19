/*****************************************************************************************
//	剑网游戏插件接口
//	Copyright : Kingsoft 2005
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2005-1-1
------------------------------------------------------------------------------------------
*****************************************************************************************/
#ifndef __I_DAWN_PLUGIN_SHELL_H__
#define __I_DAWN_PLUGIN_SHELL_H__

typedef INT (*fnDawnClientCallback)(UINT uMsg, UINT uParam, INT nParam);

//##ModelId=3FB10B370006
struct IDawnPluginShell
{
	enum DAWN_PLUGIN_OPERATION;
	enum DAWN_CLIENT_EVENT;
	enum DAWN_CALLBACK_FUNC;
	enum DAWN_CLIENT_BEHAVIOR;

	virtual INT	 Init(HWND hWnd, LPVOID pReserved) = 0;
	virtual VOID Breathe() = 0;
    virtual VOID Release() = 0;

	// 通知外挂进行某种操作
	virtual INT	Operation(DAWN_PLUGIN_OPERATION eOper, DWORD uParam, INT nParam) = 0;
	// 客户端事件通知
	virtual INT	EventNotify(DAWN_CLIENT_EVENT eEvent, DWORD uParam, INT nParam) = 0;
	//剑网操作回调函数项列表
	virtual VOID SetCallback(DAWN_CALLBACK_FUNC eFuncIndex, fnDawnClientCallback pFunc) = 0;
};

enum IDawnPluginShell::DAWN_PLUGIN_OPERATION
{
	DAWN_PLUGIN_ENABLE,		//是否
	//nParam = (Boolean)bEnable 是否允许此插件起作用
};

enum IDawnPluginShell::DAWN_CLIENT_EVENT
{
	DAWN_EVENT_REPRESENT,			//客户端绘制进行中
	//uParam = (IRepresentShell*)pRepresentInterface

	DAWN_EVENT_SCENE_PLACE_FOCUS,	//客户端场景的焦点更新了
	//uParam = (INT)x
	//nParam = (INT)y

	DAWN_EVENT_ENTER_GAME,			//进入游戏了
	DAWN_EVENT_EXIT_GAME,			//离开游戏了

	COLLECT_EVENT_IP,
};

enum IDawnPluginShell::DAWN_CALLBACK_FUNC
{ 
	//## 控制客户端的行为
	DCF_CLIENT_BEHAVIOR = 1,
	//## 获取游戏世界数据
	DCF_GET_GAMEDATA, 
	//## 操作游戏世界
	DCF_GAME_OPERATION,
};

//##Documentation
//## 客户端的行为
enum IDawnPluginShell::DAWN_CLIENT_BEHAVIOR
{ 
    //## 隐藏游戏窗口
    DCB_HIDE_GAME_WND,
    //## 静止手工对游戏窗口的控制。
    DCB_DISABLE_GAME_WND_CTRL,
	//uParam = (KSwordClientAutoLoginInfo*)    
    DCB_PICK_ITEM,
    //uParam = (KSwordClientPickItem *)

	DCB_SEND_CHANNEL_MESSAGE,
	//uParam = ChannelID
	//nParam = (CHAR*)szBuff
	
	DCB_SET_CHANNEL_BY_INDEX,
	//uParam = ChannelIndex

	DCB_GET_IMAGE_SIZE,
	//uParam = (LPCSTR )pszImage
	//nParam = if 0 spr, else jpeg.
	//Return = (Width || (Height << 16))

	DCB_UI_CREATE_WINDOW,
	//uParam = (LPCSTR ) pszWndClass
	//Return = (LPVOID)pWndObject

	DCB_UI_INIT_WINDOW,
	//uParam = (LPVOID)pWndObject
	//nParam = (KDawnClientUIInitWndParam*)pParam

	DCB_UI_DELETE_WINDOW,
	//uParam = (LPVOID)pWndObject

	DCB_UI_WINDOW_ATTACH,
	//uParam = (LPVOID)pWndObject

	DCB_UI_WINDOW_ADD_CHILD,
	//uParam = (LPVOID)pWndObject
	//nParam = (LPVOID)pChildWnd

	DCB_UI_WINDOW_SPLIT_FROM_TREE,
	//uParam = (LPVOID)pWndObject

	DCB_UI_WINDOW_POS,
	//uParam = (LPVOID)pWndObject
	//nParma = (Left || (Top << 16))

	DCB_UI_WINDOW_GET_POS,
	//uParam = (LPVOID)pWndObject
	//Return = (Left || (Top << 16))

	DCB_UI_WINDOW_SIZE,
	//uParam = (LPVOID)pWndObject
	//nParma = (Widht || (Height << 16))

	DCB_SHOW_WEAPON_EFFECT,
	//uParam = (WEAPON_SHOW_EFFECT)sWeaponEffect

	DCB_CHANGE_WEAPON,
	//uParam = (INT)nWeaponID
	//nParam = (INT)nHandType

	DCB_UI_GET_VERSION,
	//uParam = (LPINT)pnVersion
	//nParam = (LPCHAR)pchMode

	DCB_GET_UIGROUP,
	
	DCB_CHANGE_DEBUGINFO_WND,
	DCB_UIGROUP_RELADSCRIPT,
};

struct KSwordClientAutoLoginInfo
{
	INT				nSvrRegionIndex;
	CHAR			szSvrTitle[32];		//服务器文字说明
	UCHAR	Address[4];			//服务器ip地址
	CHAR			szAccount[32];		//帐号
	CHAR			szPassword[32];		//密码
	CHAR			szRoleName[32];		//角色名
};

struct KSwordClientPickItem
{
    INT x;
    INT y;

    INT nAutoPickupObjectMoney;
    INT nAutoPickupObjectEquip;

    INT nObjectIdx;
    INT nObjKind;
};

struct KDawnClientUIInitWndParam
{
	LPVOID	pIniFile;
	CHAR	szSection[128];
};

typedef struct tagWeaponEffectScale
{
	FLOAT fx;	//x轴缩放
	FLOAT fy;	//y轴缩放
	FLOAT fz;	//z轴缩放	
}WEAPON_EFFECT_SCALE;

typedef struct tagWeaponEffectFile
{
	CHAR  szEffectFileName[MAX_PATH];	//特效文件名
}WEAPON_EFFECT_FILE;

typedef struct tagWeaponEffect
{
	WEAPON_EFFECT_SCALE sScale;
	WEAPON_EFFECT_FILE	sEffectFile;
}WEAPON_SHOW_EFFECT;

//------模块的接口函数的原型的定义------
typedef IDawnPluginShell*			(*fnCreateDawnPlugin)();

//------接口函数的函数名字符串----------
#define	FN_CREATE_DAWN_PLUGIN		"CreateDawnPlugin"

#endif //__I_DAWN_PLUGIN_SHELL_H__

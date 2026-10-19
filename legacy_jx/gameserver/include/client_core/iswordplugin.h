/*****************************************************************************************
//	剑网游戏插件接口
//	Copyright : Kingsoft 2003
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2003-11-12
------------------------------------------------------------------------------------------
*****************************************************************************************/
#ifndef ISWORDPLUGIN_H_HEADER_INCLUDED_C04ECF69
#define ISWORDPLUGIN_H_HEADER_INCLUDED_C04ECF69
#include "client_core/coreshell.h"


typedef INT (*fnSwordClientCallback)(UINT uMsg, UINT uParam, INT nParam);


enum enumPLUGIN_OPERATION
{
	enumPO_OPEN_CHAT_WINDOW,// 打开聊天窗口
							// uParam = 1 打开    = 0 关闭

	enumPO_NUM,
};


enum enumUI_DATA
{
	enumUD_GET_CHANNEL_COUNT,			//获取系统原有的聊天频道数量，nRet = 数量

	enumUD_GET_CHANNEL_TITLE_BY_ID,		//根据聊天频道的ID获取聊天频道的名字
										//(CHAR *)uParam = 传进去的Buff, nParam = ID
										//nRet = 实际获取的名字的长度

	enumUD_GET_CHANNEL_TITLE_BY_INDEX,	//根据聊天频道的Index获取聊天频道的名字
										//(CHAR *)uParam = 传进去的Buff, nParam = Index
										//nRet = 实际获取的名字的长度

	enumUD_GET_CHANNEL_INDEX_BY_ID,		//根据频道ID取得频道索引，uParam = ID

	enumUD_GET_CHANNEL_ID_BY_INDEX,		//根据频道Index取得ID，nParam = Index
};



//##ModelId=3FB10B370006
struct iSwordPlugin : public IClientCallback
{
public:
    //##ModelId=3FB11340018D
    //##Documentation
    //## 剑网操作回调函数项列表
    enum SWORD_CALLBACK_FUNC
	{ 
        //##Documentation
        //## 控制客户端的行为
        SCF_CLIENT_BEHAVIOR = 1,
        //##Documentation
        //## 获取游戏世界数据
        SCF_GET_GAMEDATA, 
        //##Documentation
        //## 操作游戏世界
        SCF_GAME_OPERATION,
		//##Documentation
        //## 获取界面数据
		SCF_GET_UIDATA,
	};

public:

    //##ModelId=3FB1107600A3
    virtual VOID Release() = 0;

    //##ModelId=3FB1128C01AC
    //##Documentation
    //## 设置插件是否起作用。
    virtual VOID Enable(
        //##Documentation
        //## 是否允许此插件起作用
        bool bEnable) = 0;

	virtual VOID Breathe() = 0;

    //##ModelId=3FB1130E00C2
    //##Documentation
    //## 设置操作回调函数
    virtual VOID SetCallback(
        //##Documentation
        //## 要设置的哪个回调函数
        SWORD_CALLBACK_FUNC eFuncIndex, 
        //##Documentation
        //## 函数指针
        fnSwordClientCallback pFunc) = 0;

public: //这个Public里的是fyt帝国皇帝陛下的东西，黄金狮子旗万岁！！！
	//______        _______      ______________
	//      --------       ------             /
	//		        ,%%%%%%%,                 \
	//		       ,%%/\%%%%/\%,              /
	//		      ,%%%\c "" J/%%,            /
	//		      %%%%/ d  b \%%%            \          <--- 黄金
	//		      %%%%    _  |%%%             \
	//		      `%%%%(=_Y_=)%%'             /
	//		       `%%%%`\7/%%%'             /
	//		         `%%%%%%%'               \
	//______        _______      _____________\
	//      --------       ------             

	// 通知外挂进行某种操作
	virtual INT	PluginOperation(UINT uActionID, UINT uParam, INT nParam) = 0;
};

/*
=========创建一个IRepresentShell接口的实例===============
iSwordPlugin* CreateSwordPlugin(
			//## 剑网程序的句柄
			HANDLE hSwordInstance,
			//## 剑网程序的主窗口
			HWND hWnd,
			//保留
			LPVOID pReserved);
*/
//------模块的接口函数的原型的定义------
typedef iSwordPlugin*		(*fnCreateSwordPlugin)(HANDLE hSwordInstance, HWND hWnd, LPVOID pReserved);

//------接口函数的函数名字符串----------
#define	FN_CREATE_SWORD_PLUGIN			"CreateSwordPlugin"

//##Documentation
//## 客户端的行为
enum SWORD_CLIENT_BEHAVIOR
{ 
	//关闭当前发送请求的插件
	SCB_RELEASE_PLUGIN = 1,
	//uParam = iSwordPlugin*)
    //##Documentation
    //## 隐藏游戏窗口
    SCB_HIDE_GAME_WND,
    //##Documentation
    //## 静止手工对游戏窗口的控制。
    SCB_DISABLE_GAME_WND_CTRL,
	//uParam = (KSwordClientAutoLoginInfo*)    
    SCB_PICK_ITEM,
    //uParam = (KSwordClientPickItem *)

	SCB_SEND_CHANNEL_MESSAGE,
	//uParam = ChannelID
	//nParam = (CHAR*)szBuff
	
	SCB_SET_CHANNEL_BY_INDEX,
	//uParam = ChannelIndex
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

#endif //ISWORDPLUGIN_H_HEADER_INCLUDED_C04ECF69//##ModelId=3FB1E07C0369

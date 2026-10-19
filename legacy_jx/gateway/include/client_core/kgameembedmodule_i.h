/* -------------------------------------------------------------------------
//	文件名		：	KGameEmbedModule_i.h
//	创建者		：	lailigao
//	创建时间	：	2005-11-02 13:42:40
//	功能描述	：	在游戏中嵌入第三方模块(带UI)的接口定义
//
// -----------------------------------------------------------------------*/
#ifndef __KGAMEEMBEDMODULE_I_H__
#define __KGAMEEMBEDMODULE_I_H__

// -------------------------------------------------------------------------

#ifndef STDPROC
	#define STDPROC					virtual HRESULT __stdcall
	#define STDPROC_(Type)			virtual Type __stdcall
#endif

#ifndef STDAPI
	#define STDAPI                  EXTERN_C HRESULT __stdcall
	#define STDAPI_(type)           EXTERN_C type __stdcall
#endif

// 通知回调接口
interface IKGameEmedNotify
{
	// 当界面关闭时(无论是主动，还是被动)，响应
	STDPROC OnCloseUI() = 0;
};

// -------------------------------------------------------------------------
interface IKGameEmedMoudle
{
	// 释放接口，不计数
	STDPROC_(VOID) Release() = 0;
	
	// 取界面的大小(像素点)
	STDPROC GetUISize(INT& nWidth, INT& nHeight) = 0;

	// 创建UI界面
	STDPROC CreateUI(INT x, INT y, INT nWidth, INT nHeight, 
						HWND hWndParent, HINSTANCE hInstance) = 0;
	// 关闭UI界面
	STDPROC CloseUI() = 0;

	// 注入/反注入通知接口
	STDPROC AdviseNotify(IKGameEmedNotify* pINotify) = 0;
	STDPROC UnAdviseNotify() = 0;
};

STDAPI gCreateGameEmbedModule(IKGameEmedMoudle** ppObj);

// -------------------------------------------------------------------------

#endif /* __KGAMEEMBEDMODULE_I_H__ */

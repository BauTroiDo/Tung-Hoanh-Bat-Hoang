/* -------------------------------------------------------------------------
//	文件名		：	gw_autotest_i.h
//	创建者		：	lailigao
//	创建时间	：	2005-1-24 14:01:05
//	功能描述	：	游戏世界-自动测试接口
//
// -----------------------------------------------------------------------*/
#ifndef __GW_AUTOTEST_I_H__
#define __GW_AUTOTEST_I_H__

// -------------------------------------------------------------------------

struct IKGWCoreShell;
interface IKGWAutoTest : IUnknown
{
	STDPROC Init(IKGWCoreShell* pCoreShell) PURE;
	STDPROC UnInit() PURE;

	STDPROC_(BOOL) ExcuteScript(LPCSTR szCommand) PURE;

	STDPROC_(BOOL) Active() PURE;
};

//STDAPI CreateGWAutoTest(IKGWAutoTest** ppIGWAutoTest);
// -------------------------------------------------------------------------

#endif /* __GW_AUTOTEST_I_H__ */

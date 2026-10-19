/* -------------------------------------------------------------------------
//	文件名		：	kgc_control.h
//	创建者		：	luobaohang
//	创建时间	：	2008-4-18 9:33:06
//	功能描述	：	
//
// -----------------------------------------------------------------------*/
#ifndef __KGC_CONTROL_H__
#define __KGC_CONTROL_H__
#include <map>
#include <string>

enum KE_GCCONTROL_FLAG
{
	emKGCCONTROL_F_SHOWDBAFFAIR,
	emKGCCONTROL_F_SHOWDBQUERY,

	emKGCCONTROL_F_MAX = 32,
};
// -------------------------------------------------------------------------
class KGC_Control
{
public:
	KGC_Control();
	BOOL Init();
	BOOL ProcessControl(LPCSTR szCmd);
	BOOL CheckControlFlag(KE_GCCONTROL_FLAG eFlag);
protected:
private:
	typedef std::map<std::string, INT> MP_CMDFLAG;
	MP_CMDFLAG m_mpCmdFlag;
	UINT m_uFlagVal;
};

extern KGC_Control g_GcControl;
// -------------------------------------------------------------------------

#endif /* __KGC_CONTROL_H__ */

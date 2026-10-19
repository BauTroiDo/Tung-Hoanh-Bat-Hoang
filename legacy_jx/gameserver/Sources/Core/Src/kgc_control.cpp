/* -------------------------------------------------------------------------
//	文件名		：	kgc_control.cpp
//	创建者		：	luobaohang
//	创建时间	：	2008-4-17 19:05:20
//	功能描述	：	
//
// -----------------------------------------------------------------------*/

#include "stdafx.h"
#include "gamecenter/utilities.h"
#include "gamecenter/kgc_control.h"


struct KCmdFlag
{
	LPCSTR szCmd;
	INT	nFlag;
};

const static KCmdFlag s_aCmdFlag[] = {
	{ "ShowDbAffair",	emKGCCONTROL_F_SHOWDBAFFAIR },
	{ "ShowDbQuery",	emKGCCONTROL_F_SHOWDBQUERY },
};

KGC_Control g_GcControl;
// -------------------------------------------------------------------------

BOOL KGC_Control::Init()
{
	for (INT i = 0; i < countof(s_aCmdFlag); ++i)
	{
		m_mpCmdFlag[s_aCmdFlag[i].szCmd] = s_aCmdFlag[i].nFlag;
	}
	return TRUE;
}

KGC_Control::KGC_Control()
{
	m_uFlagVal = 0;
}

BOOL KGC_Control::ProcessControl( LPCSTR szCmd ) 
{
	MP_CMDFLAG::iterator it = m_mpCmdFlag.find(szCmd);
	if (it == m_mpCmdFlag.end())
		return FALSE;
	INT nFlag = it->second;
	//KGC_CONFIRM_RET_FALSE(nFlag >= 0 && nFlag < emKGCCONTROL_F_MAX);
	m_uFlagVal ^= (1 << nFlag);
	KG_Printl('\t' << szCmd << " = " << CheckControlFlag(KE_GCCONTROL_FLAG(nFlag)));
	return TRUE;
}

BOOL KGC_Control::CheckControlFlag( KE_GCCONTROL_FLAG eFlag ) 
{
	return ((1 << eFlag) & m_uFlagVal) ? 1 : 0;
}

// -------------------------------------------------------------------------

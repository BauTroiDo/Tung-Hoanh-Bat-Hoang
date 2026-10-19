/* -------------------------------------------------------------------------
//	文件名		：	utilities.h
//	创建者		：	luobaohang
//	创建时间	：	2006-11-23 16:18:32
//	功能描述	：	
//
// -----------------------------------------------------------------------*/
#ifndef __UTILITIES_H__
#define __UTILITIES_H__
#include <iostream>
#include "kgpublic.h"
typedef CHAR*			LPCHAR;
typedef CONST CHAR*		LPCCHAR;
typedef UCHAR*			LPUCHAR;
typedef CONST UCHAR*	LPCUCHAR;
typedef SHORT*			LPSHORT;
typedef CONST SHORT*	LPCSHORT;
typedef USHORT*			LPUSHORT;
typedef CONST USHORT*	LPCUSHORT;
typedef CONST WORD*		LPCWORD;
typedef CONST INT*		LPCINT;
typedef UINT*			LPUINT;
typedef CONST UINT*		LPCUINT;
typedef CONST DWORD*	LPCDWORD;
typedef INT64*			LPINT64;
typedef CONST INT64*	LPCINT64;
typedef CONST BOOL*		LPCBOOL;
typedef FLOAT*			LPFLOAT;
typedef CONST FLOAT*	LPCFLOAT;
typedef DOUBLE*			LPDOUBLE;
typedef CONST DOUBLE*	LPCDOUBLE;

//在Gamecenter中请不要频繁输出重复或无用的信息，但有需要一定要输出
// -------------------------------------------------------------------------
#define KG_Print(stream)	std::cout << stream
#define KG_Printl(stream)	std::cout << stream << std::endl
#define KGC_Error(stream)	{_ASSERT(FALSE);std::cout << "[GCError] " << '(' << __LINE__ << ") " << stream << std::endl;}
#define KGC_CONFIRM_RET(condition, ret) if (!(condition)) { KGC_Error(""); return ret; }
#define KGC_CONFIRM_RET_FALSE(condition) if (!(condition)) { KGC_Error(""); return FALSE; }
#define KGC_CONFIRM_RET_NULL(condition) if (!(condition)) { KGC_Error(""); return 0; }
#define KGC_CONFIRM_BREAK(condition) if (!(condition)) { KGC_Error(""); break; }
#define KGC_CONFIRM_CONTINUE(condition) if (!(condition)) { KGC_Error(""); continue; }
// -------------------------------------------------------------------------

#endif /* __UTILITIES_H__ */

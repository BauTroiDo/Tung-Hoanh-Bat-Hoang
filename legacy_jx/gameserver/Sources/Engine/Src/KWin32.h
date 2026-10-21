//---------------------------------------------------------------------------
// Blade Engine (c) 1999-2000 by Kingsoft
//
// File:	KWin32.h
// Date:	2001.10.10
// Code:	Daphnis
// Desc:	Percompiled header files of Win32 Platform
//---------------------------------------------------------------------------
#ifndef KWin32_H
#define KWin32_H 
#ifdef _MSC_VER
#pragma warning(disable:4996)  //屏蔽某类警告
#endif

#define __RPC__out_xcount_part(size, length)

#define __RPC__in_xcount(size)

#define __RPC__in_xcount_full(size)

#define __RPC__in_range(min, max)

#define __RPC__inout_xcount(size)

#ifndef POINTER_64

#if !defined(_MAC) && (defined(_M_MRX000) || defined(_M_AMD64) || defined(_M_IA64)) && (_MSC_VER >= 1100) && !(defined(MIDL_PASS) || defined(RC_INVOKED))
#define POINTER_64 __ptr64
typedef unsigned __int64 POINTER_64_INT;
#if defined(_WIN64)
#define POINTER_32 __ptr32
#else
#define POINTER_32
#endif
#else
#if defined(_MAC) && defined(_MAC_INT_64)
#define POINTER_64 __ptr64
typedef unsigned __int64 POINTER_64_INT;
#else
#if (_MSC_VER >= 1300) && !(defined(MIDL_PASS) || defined(RC_INVOKED))
#define POINTER_64 __ptr64
#else
#define POINTER_64
#endif
typedef unsigned long POINTER_64_INT;
#endif
#define POINTER_32
#endif
#endif

//-----------------------------

#ifdef _STANDALONE
	#define ENGINE_API
	#define _ASSERT(x)
#elif defined(__linux__) || defined(__linux)
	#define ENGINE_API __attribute__((visibility("default")))
#else
	#ifdef ENGINE_EXPORTS
		#define ENGINE_API __declspec(dllexport)
	#else
		#define ENGINE_API __declspec(dllimport)
	#endif
#endif

#ifndef __linux
	/* winsock2 has to be first: the Platform SDK's windows.h still pulls in
	   winsock.h, and redefining sockaddr/ws2def is a fatal C2011 storm. */
	#include <winsock2.h>
	#include <windows.h>
	#define DIRECTINPUT_VERSION 0x800
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
	#if _MSC_VER > 1000
	#pragma once
	#endif // _MSC_VER > 1000
        #include <windows.h>
        #include <mmsystem.h>
        #include <dsound.h>
        #include <dinput.h>
        #include <ddraw.h>
#else
        #include <stdint.h>
        typedef int32_t LONG;
        typedef int64_t LONGLONG;
		#define INT int
		#define LPINT int *
        #define HWND unsigned long
        #define HANDLE unsigned long
        #define LPSTR char *
        #define LPTSTR char *
        #define LPCSTR const char *
		#define LPCTSTR const char *
        typedef uint32_t DWORD;
        #define LPVOID void *
        #define PVOID void *
        #define BOOL int
        #define TRUE 1
        #define FALSE 0
        #define BYTE unsigned char
        #define WORD unsigned short
	#define UINT unsigned int
	#define PBYTE unsigned char *
	#define LPBYTE unsigned char *
        /* LONG is the fixed-width signed 32-bit typedef above. */
#define IN
#define OUT
typedef struct tagRECT
{
    LONG    left;
    LONG    top;
    LONG    right;
    LONG    bottom;
} RECT, *LPRECT;

typedef struct tagPOINT
{
    LONG  x;
    LONG  y;
} POINT, *PPOINT;

typedef struct _GUID {          // size is 16
    DWORD  Data1;
    WORD   Data2;
    WORD   Data3;
    BYTE   Data4[8];
} GUID;

#define MAX_PATH 1200  //文件名限制
#define CALLBACK
#include <stdarg.h> 

#define ZeroMemory(x,y) memset(x, 0, y)
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#define FILE_CURRENT	1
#define FILE_END		2
#define FILE_BEGIN		0

#include <pthread.h>
#define MAKEWORD(a, b)      ((WORD)(((BYTE)(a)) | ((WORD)((BYTE)(b))) << 8))
#define MAKELONG(a, b)      ((LONG)(((WORD)(a)) | ((DWORD)((WORD)(b))) << 16))
#define LOWORD(l)           ((WORD)(l))
#define HIWORD(l)           ((WORD)(((DWORD)(l) >> 16) & 0xFFFF))
#define LOBYTE(w)           ((BYTE)(w))
#define HIBYTE(w)           ((BYTE)(((WORD)(w) >> 8) & 0xFF))

#ifdef WIN32
	#define SUCCEEDED(x)		((x) > 0)
#else
	#define SUCCEEDED(x)		((long)x >= 0)
#endif


#include <unistd.h>
#define __stdcall
typedef DWORD* LPDWORD;
#define SOCKET_ERROR -1
#endif
//---------------------------------------------------------------------------
// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
//---------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>


#ifndef ITOA
#define ITOA(NUMBER)  #NUMBER
#endif

#ifndef __TEXT_LINE__
#define __TEXT_LINE__(LINE) ITOA(LINE)
#endif

#ifndef KSG_ATTENTION
#define KSG_ATTENTION(MSG) __FILE__ "(" __TEXT_LINE__(__LINE__) ") : ATTENTION " #MSG
#endif


//---------------------------------------------------------------------------
#endif

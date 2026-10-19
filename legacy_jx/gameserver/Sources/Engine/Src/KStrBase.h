//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KStrBase.h
// Date:	2000.08.08
// Code:	WangWei(Daphnis)
// Desc:	Header File
//---------------------------------------------------------------------------
#ifndef KStrBase_H
#define KStrBase_H
#include <iostream>
#include <string>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <windows.h>

#include "iconv.h"
//格式串的规则定义
//%[flag] [width] [.precision] [{h | l | I64 | L}]type
//flags: -,+,0,' ',#
//width:
//precision
//type:c,C,d,i,o,u,x,X,e,E,f,g,s,S
//定义扫描机的状态
using namespace std;
typedef enum{
	XS_SKIP = 0,     //扫描前缀串
	XS_PROC = 1,     //需处理当前格式或前缀串
	XS_FLAG = 2,     //扫描添位符
	XS_WIDTH = 3,     //扫描占宽
	XS_PREC = 4,     //扫描精度
	XS_SIZE= 5,     //扫描整形指示
	XS_TYPE = 6,     //扫描格式符
	XS_TERM = 7     //扫描终止
}XF_STATUS;

//定义扫描机的动作
typedef enum{
	XO_PAUSE = 0,    //暂停
	XO_CONTINUE = 1    //继续
}XF_OPERA;

//判断是否添位符
#define s_is_flag(ch)    ((ch == '0' || ch == ' ' || ch == '#')? 1 : 0)
//判断是否是数字
#define s_is_digit(ch)    ((ch >= '0' && ch <= '9')? 1 : 0)
//判断是否是整形指示
#define s_is_size(ch)    ((*token == 'h' || *token == 'l')? 1 : 0)
//判断是否是格式符
#define s_is_type(ch)    ((ch == 'c' || ch == 'C' || ch == 'd' || ch == 'u' || ch == 'x' || ch == 'X' || ch == 'f'|| ch == 's' || ch == 'S')? 1 : 0)
//---------------------------------------------------------------------------
ENGINE_API	int		g_StrLen(LPCSTR lpStr);
ENGINE_API	LPSTR	g_StrEnd(LPCSTR lpStr);
ENGINE_API	void	g_StrCpy(LPSTR lpDest, LPCSTR lpSrc);
ENGINE_API	void	g_StrCpyLen(LPSTR lpDest, LPCSTR lpSrc, int nMaxLen);
ENGINE_API	void	g_StrCat(LPSTR lpDest, LPCSTR lpSrc);
ENGINE_API	void	g_StrCatLen(LPSTR lpDest, LPCSTR lpSrc, int nMaxLen);
ENGINE_API	BOOL	g_StrCmp(LPCSTR lpDest, LPCSTR lpSrc);
ENGINE_API	BOOL	g_StrCmpLen(LPCSTR lpDest, LPCSTR lpSrc, int nMaxLen);
ENGINE_API	void	g_StrUpper(LPSTR lpDest);
ENGINE_API	void	g_StrLower(LPSTR lpDest);
ENGINE_API	void	g_StrRep(LPSTR lpDest, LPSTR lpSrc, LPSTR lpRep);
ENGINE_API  UINT    g_Atoui(LPSTR str);  //将字符串专程UINT
ENGINE_API  int     g_ExtractChar(const char *inStr,char scrchar,char destchar,char *outStr=NULL,char *outStra=NULL,int nMoedel=0);
//ENGINE_API  std::string Utf8ToGbk(const char *src_str);
ENGINE_API  const char* Utf8ToGbk(const char *utf8Str, char* pcDes, int nMaxLen);
ENGINE_API  string Utf8ToGbk_(const char *utf8_str);
ENGINE_API  string GbkToUtf8(const char *gbkStr);

 std::string gbk2utf8(const char *inbuf);
 std::string utf2gbk(const char *inbuf);


ENGINE_API  std::string gbk2iso_(LPSTR inbuf);
ENGINE_API  std::string iso2gbk_(LPSTR inbuf);
ENGINE_API std::string iso2utf8_(LPSTR inbuf);
ENGINE_API  std::string utf2iso_(LPSTR inbuf);

 std::string gbk2iso(const char *inbuf);
 std::string iso2gbk(const char *inbuf);
 std::string iso2utf8(const char *inbuf);
 std::string utf2iso(const char *inbuf);
 std::string w2v(const char* inbuf);
 std::string v2u(const char* inbuf);
//ENGINE_API int t_snprintf(char *buf, unsigned int count, const char *format, ... );
//ENGINE_API int t_sprintf(char *out, const char *format, ...);
//ENGINE_API int t_printf(const char *format, ...);
//---------------------------------------------------------------------------
#endif

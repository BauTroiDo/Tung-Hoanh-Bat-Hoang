//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KStrBase.cpp
// Date:	2000.08.08
// Code:	WangWei(Daphnis)
// Desc:	String Utility Functions
//---------------------------------------------------------------------------
#include "KWin32.h"
#include "KDebug.h"
#include "KMemBase.h"
#include "KStrBase.h"
#include <string.h>
ENGINE_API int g_StrLen(LPCSTR lpStr)
{
/*#ifdef WIN32
	int nLen;

	__asm
	{
		mov		edi, lpStr
		mov		ecx, 0xffffffff
		xor		al, al
		repne	scasb
		not		ecx
		dec		ecx
		mov		nLen, ecx
	}
	return nLen;
#else*/
     return strlen(lpStr);
//#endif
}
//---------------------------------------------------------------------------
// 函数:	StrEnd
// 功能:	返回字符串结尾指针
// 参数:	lpStr	:	字符串开头的指针
// 返回:	lpEnd	:	字符串末尾的指针
//---------------------------------------------------------------------------
//static unsigned int g_Atoui(const char *str);
ENGINE_API int g_ExtractChar(const char *inStr,char scrchar,char destchar,char *outStr,char *outStra,int nMoedel)
{
	char *tmp=NULL,*tmpa=NULL;         //定义一个临时数组空间，存放字符；
	tmp=outStr,tmpa=outStra; 
	int nLen=0;
	while(*inStr!='\0')
	{//一直前行,直到有结束符号将停止。
		if(nMoedel==0 && *inStr==scrchar)
		{//提取两个字符之间的内容 存放在 tmpa中
			inStr++;                      //过滤掉开始字符，指向下个数进行比较。     
           	while(*inStr!='\0')
			{//接着前行   
                  if (*inStr==destchar)   //数组中的第二个元素与结束字符相同
				  {		            
					  //*inStr='|';       //替换掉结束字符
                      inStr++;            //过滤掉结束符，指向下个数。
					  break;              //找到结束符号 过滤掉 跳出循环。
				  }
				  *tmpa=*inStr;           //提取字符放进tempa中
				  tmpa++;	              //下一个元素			  
	              inStr++;                //下一个元素
			}
			*tmpa='\0';                  //提取完后，加个结束符号
		}
		else if (nMoedel==1 && *inStr==scrchar)
		{//提取字符前面的内容 存放在 tmp中
              break;
		}
		else if (nMoedel==2 && *inStr==scrchar)
		{//提取字符后面的内容 存放在 tmpa中
			inStr++;                      //过滤掉开始字符，指向下个数进行比较。     
           	while(*inStr!='\0')
			{//接着前行,继续提取后面的内容   
                  /*if (*inStr==destchar)   //数组中的第二个元素与结束字符相同
				  {		            
					  //*inStr='|';       //替换掉结束字符
                      inStr++;            //过滤掉结束符，指向下个数。
					  break;              //找到结束符号 过滤掉 跳出循环。
				  }*/
				  *tmpa=*inStr;           //提取字符放进tempa中
				  tmpa++;	              //下一个元素			  
	              inStr++;                //下一个元素
			}
			*tmpa='\0';                  //提取完后，加个结束符号
		}

		if (*inStr=='\0')                //如果没有找到结束符，并且已经到了结尾，就跳出了！！
			break;

		*tmp=*inStr;                     //元数不相同，则存放到tmp中。
		tmp++;                           //进行下一个元数的比较
		inStr++;                    
		nLen++;
	}
	*tmp='\0';                           //源字符串 提取完后 加个结束符 防止乱码
    return nLen;
}


ENGINE_API string Utf8ToGbk_(const char *utf8_str)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, utf8_str, -1, NULL, 0);
	wchar_t* wszGBK = new wchar_t[len + 1];
	memset(wszGBK, 0, len * 2 + 2);
	MultiByteToWideChar(CP_UTF8, 0, utf8_str, -1, wszGBK, len);
	len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
	char* szGBK = new char[len + 1];
	memset(szGBK, 0, len + 1);
	WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);
	string strTemp(szGBK);
	if (wszGBK) delete[] wszGBK;
	if (szGBK) delete[] szGBK;
	return strTemp;
}

ENGINE_API string GbkToUtf8(const char *gbkStr)
{
	//ASSERT(src_str!=NULL);
	/*int len = MultiByteToWideChar(CP_ACP, 0, gbkStr, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len + 1];
	memset(wstr, 0, (len + 1) * sizeof(wchar_t));
	MultiByteToWideChar(CP_ACP, 0, gbkStr, -1, wstr, len);
	len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len + 1];
	memset(str, 0, len + 1);
	WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
	std::string strTemp = str;
	if (wstr) delete[] wstr;
	if (str) delete[] str;
	return strTemp;*/

	if (!gbkStr) return "";

	//int len=MultiByteToWideChar(CP_ACP, 0, (LPCSTR)gbkStr, -1, NULL,0);
	//unsigned short * wszUtf8 = new unsigned short[len+1];
	//memset(wszUtf8, 0,len + 1 /*len * 2 + 2*/);
	//MultiByteToWideChar(CP_ACP, 0, (LPCSTR)gbkStr, -1, (LPWSTR)wszUtf8, len);
	//len = WideCharToMultiByte(CP_UTF8, 0, (LPWSTR)wszUtf8, -1, NULL, 0, NULL, NULL);
	//char *szUtf8=new char[len + 1];
	//memset(szUtf8, 0,len + 1);
	//WideCharToMultiByte (CP_UTF8, 0, (LPWSTR)wszUtf8, -1, szUtf8, len, NULL,NULL);
	//std::string str(szUtf8);
	//if (szUtf8)
	//{
	//  delete[] szUtf8;
	//  szUtf8 = NULL;
	//}
	//if (wszUtf8)
	//{
	//  delete[] wszUtf8;
	//  wszUtf8 = NULL;
	//}
	//return str;
	int len = MultiByteToWideChar(CP_ACP, 0, gbkStr, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len + 1];
	memset(wstr, 0, (len + 1) * sizeof(wchar_t));
	MultiByteToWideChar(CP_ACP, 0, gbkStr, -1, wstr, len);
	len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len + 1];
	memset(str, 0, len + 1);
	WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
	string strTemp = str;
	if (wstr) delete[] wstr;
	if (str) delete[] str;
	return strTemp;
	
}

ENGINE_API const char* Utf8ToGbk(const char *utf8Str, char* pcDes, int nMaxLen)
{
    // A capacity is mandatory. The destination is always NUL-terminated on failure.
    if (pcDes && nMaxLen > 0)
        pcDes[0] = '\0';
    if (!utf8Str || !pcDes || nMaxLen <= 0)
        return NULL;

    const int wideLen = ::MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, utf8Str, -1, NULL, 0);
    if (wideLen <= 0)
        return NULL;
    wchar_t *wide = new wchar_t[wideLen];
    if (::MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, utf8Str, -1, wide, wideLen) != wideLen)
    {
        delete[] wide;
        return NULL;
    }

    const int outputLen = ::WideCharToMultiByte(936, 0, wide, -1, NULL, 0, NULL, NULL);
    if (outputLen <= 0 || outputLen > nMaxLen)
    {
        delete[] wide;
        return NULL; // Do not silently truncate file names, script paths, or messages.
    }
    const int written = ::WideCharToMultiByte(936, 0, wide, -1, pcDes, nMaxLen, NULL, NULL);
    delete[] wide;
    if (written != outputLen)
    {
        pcDes[0] = '\0';
        return NULL;
    }
    return pcDes;
}

ENGINE_API UINT g_Atoui(LPSTR str)
{
	UINT result = 0, i = 0;

	char *tmp = NULL;

	for (i = 0; isspace(str[i]) && i < strlen(str); i++)//跳过空白符;  
		;
	    tmp = str+i;

	while (*tmp)
	{
		result = result * 10 + *tmp - '0';
		tmp++;
	}

	return result;
}
/*
static void t_printchar(char **str, int c)
{
	//extern int putchar(int c);

	if (str) {
		**str = c;
		++(*str);
	}
	//else (void)putchar(c);
}

#define PAD_RIGHT 1
#define PAD_ZERO 2

static int t_prints(char **out, const char *string, int width, int pad)
{
	int pc = 0, padchar = ' ';

	if (width > 0) {
		int len = 0;
		const char *ptr;
		for (ptr = string; *ptr; ++ptr) ++len;
		if (len >= width) width = 0;
		else width -= len;
		if (pad & PAD_ZERO) padchar = '0';
	}
	if (!(pad & PAD_RIGHT)) {
		for ( ; width > 0; --width) {
			t_printchar(out, padchar);
			++pc;
		}
	}
	for ( ; *string ; ++string) {
		t_printchar(out, *string);
		++pc;
	}
	for ( ; width > 0; --width) {
		t_printchar(out, padchar);
		++pc;
	}

	return pc;
}

// the following should be enough for 32 bit int 
#define PRINT_BUF_LEN 12

static int t_printi(char **out, int i, int b, int sg, int width, int pad, int letbase)
{
	char print_buf[PRINT_BUF_LEN];
	char *s;
	int t, neg = 0, pc = 0;
	unsigned int u = i;

	if (i == 0) {
		print_buf[0] = '0';
		print_buf[1] = '\0';
		return t_prints(out, print_buf, width, pad);
	}

	if (sg && b == 10 && i < 0) {
		neg = 1;
		u = -i;
	}

	s = print_buf + PRINT_BUF_LEN-1;
	*s = '\0';

	while (u) {
		t = u % b;
		if( t >= 10 )
			t += letbase - '0' - 10;
		*--s = t + '0';
		u /= b;
	}

	if (neg) {
		if( width && (pad & PAD_ZERO) ) {
			t_printchar(out, '-');
			++pc;
			--width;
		}
		else {
			*--s = '-';
		}
	}

	return pc + t_prints(out, s, width, pad);
}

static int t_print( char **out, const char *format, va_list args )
{
	int width, pad;
	int pc = 0;
	char scr[2];

	for (; *format != 0; ++format) {
		if (*format == '%') {
			++format;
			width = pad = 0;
			if (*format == '\0') break;
			if (*format == '%') goto out;
			if (*format == '-') {
				++format;
				pad = PAD_RIGHT;
			}
			while (*format == '0') {
				++format;
				pad |= PAD_ZERO;
			}
			for ( ; *format >= '0' && *format <= '9'; ++format) {
				width *= 10;
				width += *format - '0';
			}
			if( *format == 's' ) {
				char *s = (char *)va_arg(args, int );
				pc += t_prints(out, s?s:"(null)", width, pad);
				continue;
			}
			if( *format == 'd' ) {
				pc += t_printi(out, va_arg( args, int ), 10, 1, width, pad, 'a');
				continue;
			}
			if( *format == 'x' ) {
				pc += t_printi(out, va_arg( args, int ), 16, 0, width, pad, 'a');
				continue;
			}
			if( *format == 'X') {
				pc += t_printi(out, va_arg( args, int ), 16, 0, width, pad, 'A');
				continue;
			}
			if( *format == 'u' ) {
				pc += t_printi(out, va_arg( args, int ), 10, 0, width, pad, 'a');
				continue;
			}
			if( *format == 'c' ) {
				// char are converted to int then pushed on the stack 
				scr[0] = (char)va_arg( args, int );
				scr[1] = '\0';
				pc += t_prints(out, scr, width, pad);
				continue;
			}
		}
		else {
out:
			t_printchar(out, *format);
			++pc;
		}
	}
	if (out) **out = '\0';
	va_end( args );
	return pc;
}

ENGINE_API int t_printf(const char *format, ...)
{
	va_list args;

	va_start( args, format );
	return t_print( 0, format, args );
}

ENGINE_API int t_sprintf(char *out, const char *format, ...)
{
	va_list args;
	va_start( args, format );
	return t_print( &out, format, args );
}

ENGINE_API int t_snprintf(char *buf, unsigned int count, const char *format, ... )
{
	va_list args;

	(void) count;

	va_start( args, format );
	return t_print( &buf, format, args );
}
*/
ENGINE_API LPSTR g_StrEnd(LPCSTR lpStr)
{
/*#ifdef WIN32
	LPSTR lpEnd;

	__asm
	{
		mov		edi, lpStr
		mov		ecx, 0xffffffff
		xor		al, al
		repne	scasb
		lea		eax, [edi - 1]
		mov		lpEnd, eax
	}
	return lpEnd;
#else*/
     return (char *)lpStr + strlen(lpStr);
//#endif
}
//---------------------------------------------------------------------------
// 函数:	StrCpy
// 功能:	字符串拷贝
// 参数:	lpDest	:	目标字符串
//			lpSrc	:	源字符串
// 返回:	void
//---------------------------------------------------------------------------
ENGINE_API void g_StrCpy(LPSTR lpDest, LPCSTR lpSrc)
{
/*#ifdef WIN32
	__asm
	{
		mov		edi, lpSrc
		mov		ecx, 0xffffffff
		xor		al, al
		repne	scasb
		not		ecx
		mov		edi, lpDest
		mov		esi, lpSrc
		mov		edx, ecx
		shr		ecx, 2
		rep		movsd
		mov		ecx, edx
		and		ecx, 3
		rep		movsb
	};
#else*/
    //strcpy(lpDest, lpSrc);
	strcpy(lpDest, lpSrc);
	//sprintf_s(lpDest,lpSrc);
	//strcpy_s(lpDest,sizeof(lpDest),lpSrc);
	//int nLeg=sizeof(lpDest);
	//lpDest[nLeg-1]="\0";
//#endif
}
//---------------------------------------------------------------------------
// 函数:	StrCpyLen
// 功能:	字符串拷贝,有最大长度限制
// 参数:	lpDest	:	目标字符串
//			lpSrc	:	源字符串
//			nMaxLen	:	最大长度
// 返回:	void
//---------------------------------------------------------------------------
ENGINE_API void g_StrCpyLen(LPSTR lpDest, LPCSTR lpSrc, int nMaxLen)
{
/*#ifdef WIN32
	__asm
	{
		xor		al, al
		mov		edx, nMaxLen
		dec		edx
		jg		copy_section

		jl		finished
		mov		edi, lpDest
		stosb
		jmp		finished

copy_section:
		mov		edi, lpSrc
		mov		ecx, 0xffffffff
		repne	scasb
		not		ecx
		dec		ecx
		cmp		ecx, edx
		jle		loc_little_equal
		mov		ecx, edx

loc_little_equal:

		mov		edi, lpDest
		mov		esi, lpSrc
		mov		edx, ecx
		shr		ecx, 2
		rep		movsd
		mov		ecx, edx
		and		ecx, 3
		rep		movsb
		stosb
		
finished:
	};
#else*/
    if (!lpDest || nMaxLen <= 0)
        return;
    if (!lpSrc)
    {
        lpDest[0] = '\0';
        return;
    }
    snprintf(lpDest, (size_t)nMaxLen, "%s", lpSrc);
//#endif
}
//---------------------------------------------------------------------------
// 函数:	StrCat
// 功能:	字符串末尾追加另一个字符串
// 参数:	lpDest	:	目标字符串
//			lpSrc	:	源字符串
// 返回:	void
//---------------------------------------------------------------------------
ENGINE_API void g_StrCat(LPSTR lpDest, LPCSTR lpSrc)
{
	LPSTR lpEnd;

	lpEnd = g_StrEnd(lpDest);
	g_StrCpy(lpEnd, lpSrc);
}
//---------------------------------------------------------------------------
// 函数:	StrCatLen
// 功能:	字符串末尾追加另一个字符串,有最大长度限制
// 参数:	lpDest	:	目标字符串
//			lpSrc	:	源字符串
//			nMaxLen	:	最大长度
// 返回:	void 没有调用
//---------------------------------------------------------------------------
ENGINE_API void g_StrCatLen(LPSTR lpDest, LPCSTR lpSrc, int nMaxLen)
{
	LPSTR lpEnd;

	lpEnd = g_StrEnd(lpDest);
	g_StrCpyLen(lpEnd, lpSrc, nMaxLen);
}
//---------------------------------------------------------------------------
// 函数:	StrCmp
// 功能:	字符串比较
// 参数:	lpDest	:	字符串1	
//			lpSrc	:	字符串2
// 返回:	TRUE	:	相同
//			FALSE	:	不同
//---------------------------------------------------------------------------
ENGINE_API BOOL g_StrCmp(LPCSTR lpDest, LPCSTR lpSrc)
{
	int nLen1, nLen2;

	nLen1 = g_StrLen(lpDest);
	nLen2 = g_StrLen(lpSrc);
	if (nLen1 != nLen2)
		return FALSE;
	return g_MemComp((void*)lpDest, (void*)lpSrc, nLen1);
}
//---------------------------------------------------------------------------
// 函数:	StrCmpLen
// 功能:	字符串比较,限定长度
// 参数:	lpDest	:	字符串1	
//			lpSrc	:	字符串2
//			nLen	:	长度
// 返回:	TRUE	:	相同
//			FALSE	:	不同
//---------------------------------------------------------------------------
ENGINE_API BOOL g_StrCmpLen(LPCSTR lpDest, LPCSTR lpSrc, int nMaxLen)
{
	int nLen1, nLen2;

	nLen1 = g_StrLen(lpDest);
	nLen2 = g_StrLen(lpSrc);
	if (nMaxLen > nLen1)
		nMaxLen = nLen1;
	if (nMaxLen > nLen2)
		nMaxLen = nLen2;
	return g_MemComp((void*)lpDest, (void*)lpSrc, nMaxLen);
}
//---------------------------------------------------------------------------
// 函数:	StrUpper
// 功能:	小写字母转大写字母
// 参数:	lpDest	:	字符串
// 返回:	void
//---------------------------------------------------------------------------
ENGINE_API void g_StrUpper(LPSTR lpDest)
{
/*#ifdef WIN32
	__asm
	{
		mov		esi, lpDest
loc_lodsb:
		lodsb
		or		al, al
		je		loc_exit
		cmp		al, 'a'
		jb		loc_lodsb
		cmp		al, 'z'
		ja		loc_lodsb
		sub		al, 0x20
		mov		[esi - 1], al
		jmp		loc_lodsb
loc_exit:
	}
#else*/
     char *ptr = lpDest;
     while(*ptr) {
         if(*ptr >= 'a' && *ptr <= 'z') *ptr += 'A' - 'a';
//          *ptr = toupper(*ptr);
          ptr++;
     }
//#endif
}
//---------------------------------------------------------------------------
// 函数:	StrLower
// 功能:	大写字母转小写字母
// 参数:	lpDest	:	字符串
// 返回:	void
//---------------------------------------------------------------------------
ENGINE_API void g_StrLower(LPSTR lpDest)
{
/*#ifdef WIN32
	__asm
	{
		mov		esi, lpDest
loc_lodsb:
		lodsb
		or		al, al
		je		loc_exit
		cmp		al, 'A'
		jb		loc_lodsb
		cmp		al, 'Z'
		ja		loc_lodsb
		add		al, 0x20
		mov		[esi - 1], al
		jmp		loc_lodsb
loc_exit:
	}
#else*/
     char *ptr = lpDest;
     while(*ptr) {
         if(*ptr >= 'A' && *ptr <= 'Z') *ptr += 'a' - 'A';
//          *ptr = tolower(*ptr);
          ptr++;
     }
//#endif
}
//---------------------------------------------------------------------------没有调用
ENGINE_API void g_StrRep(LPSTR lpDest, LPSTR lpSrc, LPSTR lpRep)
{
	int		nSrcLen = g_StrLen(lpSrc);
	int		nDestLen = g_StrLen(lpDest);
	int		nMaxLen = nDestLen - nSrcLen + g_StrLen(lpRep) + 1;
	char	*pStart = NULL;
        int i;
	for (i = 0; i < nDestLen - nSrcLen; i++)
	{
		if (g_StrCmpLen(&lpDest[i], lpSrc, nSrcLen))
			break;
	}
	if (i == nDestLen - nSrcLen)
		return;

	pStart = new char[nMaxLen];

	if (i != 0)
	{
		g_StrCpyLen(pStart, lpDest, i);
		g_StrCat(pStart, lpRep);
		g_StrCat(pStart, &lpDest[i + nSrcLen]);
	}
	else
	{
		g_StrCpy(pStart, lpRep);
		g_StrCat(pStart, &lpDest[nSrcLen]);
	}
	g_StrCpy(lpDest, pStart);
	if (pStart)
	{
		delete [] pStart;
		pStart = NULL;
	}
}
//重写的函数
/*
int xsprintf(char* buf,const char* fmt,...)
{
	va_list arg;
	int total = 0;
	char xf_flag = 0;
	int xf_width = 0;
	int xf_prec = 0;
	char xf_size = 0;
	char xf_type = 0;
	char tk_width[NUM_LEN + 1],tk_prec[NUM_LEN + 1];
	int width_count = 0;
	int prec_count = 0;
	int tk_count = 0;

	//初始化
	XF_STATUS xs = XS_SKIP;
	XF_OPERA xo = XO_PAUSE;

	char* token = (char*)fmt;

	va_start(arg,fmt);

	while(xs != XS_TERM)
	{
		switch(xs)
		{
		case XS_SKIP:
			if(*token == '%' && *(token + 1) != '%')
			{
				if(!tk_count)
				{
					xs = XS_FLAG; //没有前缀串则过渡到格式串扫描
					xo = XO_CONTINUE;
				}else
				{
					xs = XS_PROC; //有前缀串则先处理前缀串
					xo = XO_PAUSE;
				}
			}else if(*token == '\0')
			{
				xs = XS_PROC; //过渡到处理前缀串
				xo = XO_PAUSE; 
			}else
			{
				xs = XS_SKIP; //继续前缀串扫描
				xo = XO_CONTINUE; 
			}
			break;
		case XS_FLAG:
			if(s_is_flag(*token)) 
			{
				xf_flag = *token; 
				xs = XS_FLAG; //继续占位符扫描
				xo = XO_CONTINUE;
			}else
			{
				xs = XS_WIDTH; //过渡到占位宽扫描
				xo = XO_PAUSE;
			}
			break;
		case XS_WIDTH:
			if(s_is_digit(*token))
			{
				tk_width[width_count ++] = *token;
				xs = XS_WIDTH; //继续占位宽扫描
				xo = XO_CONTINUE;
			}else if(*token == '.')
			{
				xs = XS_PREC; //过渡到精度扫描
				xo = XO_CONTINUE;
			}else
			{
				xs = XS_SIZE; //过渡到整形指示扫描
				xo = XO_PAUSE;
			}
			break;
		case XS_PREC:
			if(s_is_digit(*token))
			{
				tk_prec[prec_count ++] = *token;
				xs = XS_PREC; //继续精度扫描
				xo = XO_CONTINUE;
			}else
			{
				xs = XS_SIZE; //过渡到整形指示扫描
				xo = XO_PAUSE;
			}
			break;
		case XS_SIZE:
			if(s_is_size(*token))
			{
				xf_size = *token;
				xs = XS_TYPE; //过渡到格式符扫描
				xo = XO_CONTINUE;
			}else
			{
				xs = XS_TYPE; //过渡到格式符扫描
				xo = XO_PAUSE;
			}
			break;
		case XS_TYPE:
			if(s_is_type(*token))
			{
				xf_type = *token;
				xs = XS_PROC; //过渡到处理格式
				xo = XO_CONTINUE;
			}else
			{
				xs = XS_PROC; //无效格式符，则过渡到前缀串处理
				xo = XO_CONTINUE;
			}
			break;
		case XS_PROC:
			if(xf_type) //处理格式串
			{
				tk_width[width_count] = '\0';
				xf_width = atoi(tk_width);
				tk_prec[prec_count] = '\0';
				xf_prec = atoi(tk_prec);
				if(!xf_flag)
					xf_flag = ' ';
				if(xf_prec && xf_prec > MAX_PREC) //限制最大精度为6
					xf_prec = MAX_PREC;
				//处理这个单元的格式串
				total += _tk_sprintf((buf)? (buf + total) : NULL,xf_flag,xf_width,xf_prec,xf_size,xf_type,&arg);

				if(*token == '\0')
					xs = XS_TERM;
				else
					xs = XS_SKIP;
				xo = XO_PAUSE;
			}else //处理前缀串
			{
				if(buf)
				{
					memcpy((void*)(buf + total),(void*)(token - tk_count),tk_count * sizeof(char));
					buf[total + tk_count] = '\0';
				}
				total += tk_count;

				if(*token == '\0')
					xs = XS_TERM;
				else
					xs = XS_SKIP;
				xo = XO_PAUSE;
			}
			xf_flag = 0;
			xf_width = 0;
			xf_prec = 0;
			xf_size = 0;
			xf_type = 0;
			width_count = prec_count = tk_count = 0;

			break;
		}

		if(xo == XO_CONTINUE)
		{
			token ++;
			tk_count ++;
		}
	}

	va_end(arg);

	return total;
}


int _tk_sprintf(char* buf,char flag,int width,int prec,char size,char type,va_list* parg)
{
	wchar_t wch;
	char tmp[NUM_LEN + 1] = {0};
	int len,pos;
	char sign;
	short s;
	int i;
	unsigned short us;
	unsigned int ui,xi;
	double dbl;
	char* sz;
	wchar_t* wsz;

	switch(type)
	{
	case 'c':
		if(buf)
		{
			buf[0] = va_arg(*parg,char);
			buf[1] = '\0';
		}
		return 1;
	case 'C':
		wch = va_arg(*parg,wchar_t);
		len = WideCharToMultiByte(CP_ACP,0,&wch,1,NULL,0,NULL,NULL);
		if(buf)
		{
			WideCharToMultiByte(CP_ACP,0,&wch,1,buf,len,NULL,NULL);
			buf[len] = '\0';
		}
		return len;
	case 'd':
		len = 0;
		sign = 0;
		pos = 0;
		if(size == 'h')
		{
			s = va_arg(*parg,short);
			if(s < 0)
			{
				sign = '-';
				s = 0 - s;
			}

			while(s)
			{
				tmp[len ++] = s % 10 + 48;
				s /= 10;
			}
		}else
		{
			i = va_arg(*parg,int);
			if(i < 0)
			{
				sign = '-';
				i = 0 - i;
			}

			while(i)
			{
				tmp[len ++] = i % 10 + 48;
				i /= 10;
			}
		}
		if(sign)
		{
			if(buf)
				buf[pos] = sign;
			pos ++;
			width --;
		}
		width -= len;
		while(width > 0)
		{
			if(buf)
				buf[pos] = '0';
			pos ++;
			width --;
		}
		while(len--)
		{
			if(buf)
				buf[pos] = tmp[len];
			pos ++;
		}
		if(buf)
			buf[pos] = '\0';
		return pos;
	case 'u':
		len = 0;
		sign = 0;
		pos = 0;
		if(size == 'h')
		{
			us = va_arg(*parg,unsigned short);
			while(us)
			{
				tmp[len ++] = us % 10 + 48;
				us /= 10;
			}
		}else
		{
			ui = va_arg(*parg,unsigned int);
			while(ui)
			{
				tmp[len ++] = ui % 10 + 48;
				ui /= 10;
			}
		}
		width -= len;
		while(width > 0)
		{
			if(buf)
				buf[pos] = '0';
			pos ++;
			width --;
		}
		while(len--)
		{
			if(buf)
				buf[pos] = tmp[len];
			pos ++;
		}
		if(buf)
			buf[pos] = '\0';
		return pos;
	case 'x':
	case 'X':
		len = 0;
		sign = 0;
		pos = 0;

		if(buf)
		{
			buf[0] = '0';
			buf[1] = type;
		}
		pos += 2;

		ui = va_arg(*parg,unsigned int);
		while(ui)
		{
			us = ui % 16;
			if(type == 'x')
				tmp[len ++] = (us < 9)? (us + 48) : (us + 87);
			else
				tmp[len ++] = (us < 9)? (us + 48) : (us + 55);
			ui /= 16;
		}

		width -= len;
		while(width > 0)
		{
			if(buf)
				buf[pos] = '0';
			pos ++;
			width --;
		}
		while(len--)
		{
			if(buf)
				buf[pos] = tmp[len];
			pos ++;
		}
		if(buf)
			buf[pos] = '\0';
		return pos;
	case 'f':
		len = 0;
		sign = 0;
		pos = 0;

		dbl = va_arg(*parg,double);
		if(dbl < 0)
		{
			sign = '-';
			dbl = 0 - dbl;
		}

		xi = (int)dbl;

		dbl -= (double)xi;
		if(prec)
			s = (short)prec;
		else
			s = MAX_PREC;
		while(s--)
			dbl *= 10.0;

		ui = (int)floor(dbl + 0.5);

		if(!prec)
		{
			while(!(ui % 10))
				ui /= 10;
		}

		while(ui)
		{
			tmp[len ++] = ui % 10 + 48;
			ui /= 10;
		}
		if(len)
			tmp[len ++] = '.';

		while(xi)
		{
			tmp[len ++] = xi % 10 + 48;
			xi /= 10;
			width --;
		}

		if(sign)
		{
			if(buf)
				buf[pos] = sign;
			pos ++;
			width --;
		}

		while(width > 0)
		{
			if(buf)
				buf[pos] = '0';
			pos ++;
			width --;
		}

		while(len--)
		{
			if(buf)
				buf[pos] = tmp[len];
			pos ++;
		}
		if(buf)
			buf[pos] = '\0';
		return pos;
	case 's':
		len = 0;
		sign = 0;
		pos = 0;

		sz = va_arg(*parg,char*);
		if(!sz)
		{
			if(buf)
				buf[pos] = '\0';
			return pos;
		}

		len = strlen(sz);
		width -= len;
		while(width > 0)
		{
			if(buf)
				buf[pos] = flag;
			pos ++;
			width --;
		}

		if(buf)
			memcpy((void*)(buf + pos),(void*)sz,len * sizeof(char));
		pos += len;
		if(buf)
			buf[pos] = '\0';
		return pos;
	case 'S':
		len = 0;
		sign = 0;
		pos = 0;

		wsz = va_arg(*parg,wchar_t*);
		if(!wsz)
		{
			if(buf)
				buf[pos] = '\0';
			return pos;
		}

		ui = wcslen(wsz);
		len = WideCharToMultiByte(CP_ACP,0,wsz,ui,NULL,0,NULL,NULL);
		width -= len;
		while(width > 0)
		{
			if(buf)
				buf[pos] = flag;
			pos ++;
			width --;
		}

		if(buf)
			WideCharToMultiByte(CP_ACP,0,wsz,ui,buf + pos,len,NULL,NULL);
		pos += len;
		if(buf)
			buf[pos] = '\0';
		return pos;
	}
	return 0;
}
*/


static int code_convert(const char *from_charset, const char *to_charset,const char *inbuf, size_t inlen, char *outbuf, size_t outlen)
{
	iconv_t cd;
	//std::string nTempStr = inbuf;
	//char *temp = (char *)inbuf;
	//char **pin = &temp;
	const char *temp = inbuf;
	const char **pin = &temp;

	char **pout = &outbuf;
	memset(outbuf, 0, outlen);
	cd = iconv_open(to_charset, from_charset);
	if (cd == 0) return -1;
	if (iconv(cd, pin,&inlen,pout,&outlen) == -1) return -1;
	iconv_close(cd);
	return 0;
}


/*GB2312 to iso8859-1*/
std::string w2v(const char* inbuf)
{
	std::string strRet = "";

	unsigned int inlen = strlen(inbuf);
	char* outbuf = new char[inlen * 2 + 2];
	int ret = -1;
	// `==' binds tighter than `=': grouped, so ret holds the code and `ret == -1' below means it
	if ((ret = code_convert("cp1252", "tcvn", inbuf, inlen, outbuf, inlen * 2 + 2)) == 0)
	{
		strRet = outbuf;
	}

	if (ret == -1 || strRet == "")
		strRet = inbuf;
	delete[] outbuf;
	return strRet;
}


/*iso8859-1 to gbk*/
std::string v2u(const char* inbuf)
{
	std::string strRet = "";

	unsigned int inlen = strlen(inbuf);
	char* outbuf = new char[inlen * 2 + 2];
	int ret = -1;
	// `==' binds tighter than `=': grouped, so ret holds the code and `ret == -1' below means it
	if ((ret = code_convert("tcvn", "utf-8", inbuf, inlen, outbuf, inlen * 2 + 2)) == 0)
	{
		strRet = outbuf;
	}

	if (ret == -1 || strRet == "")
		strRet = inbuf;
	delete[] outbuf;
	return strRet;
}


/*UTF8 to GB2312*/
 std::string utf2gbk(const char *inbuf)
{
	std::string strRet="";
	size_t inlen = strlen(inbuf);  //size_t
	char * outbuf = new char[inlen * 2 + 2];
	//char outbuf[255]={0};gb2312
	int ret=-1;
	// `==' binds tighter than `=': grouped, so ret holds the code and `ret == -1' below means it
	if ((ret = code_convert("utf-8","gb2312",inbuf,inlen,outbuf,inlen * 2 + 2)) == 0)
	{
		strRet = outbuf;
	}

	if (ret==-1 ||strRet=="")
		strRet =inbuf;

	delete [] outbuf;
	return strRet;
}

/*GB2312 to UTF8*/
 std::string gbk2utf8(const char *inbuf)
{
	std::string strRet="";

	unsigned int inlen = strlen(inbuf);
	char * outbuf = new char[inlen * 2 + 2];
	int ret=-1;
	// `==' binds tighter than `=': grouped, so ret holds the code and `ret == -1' below means it
	if ((ret = code_convert("gb2312", "utf-8", inbuf, inlen,outbuf,inlen * 2 + 2)) == 0)
	{
		strRet = outbuf;
	}

	if (ret==-1 || strRet=="")
		strRet =inbuf;
	delete [] outbuf;
	return strRet;
}

/*GB2312 to iso8859-1*/
 std::string gbk2iso(const char *inbuf)
{
	std::string strRet="";

	unsigned int inlen = strlen(inbuf);
	char * outbuf = new char[inlen * 2 + 2];
	int ret=-1;
	// `==' binds tighter than `=': grouped, so ret holds the code and `ret == -1' below means it
	if ((ret = code_convert("gb2312", "iso-8859-1", inbuf, inlen,outbuf,inlen * 2 + 2)) == 0)
	{
		strRet = outbuf;
	}

	if (ret==-1 || strRet=="")
		strRet =inbuf;
	delete [] outbuf;
	return strRet;
}


/*iso8859-1 to gbk*/
 std::string iso2gbk(const char *inbuf)
{
	std::string strRet="";

	unsigned int inlen = strlen(inbuf);
	char * outbuf = new char[inlen * 2 + 2];
	int ret=-1;
	// `==' binds tighter than `=': grouped, so ret holds the code and `ret == -1' below means it
	if ((ret = code_convert("iso-8859-1", "gb2312", inbuf, inlen,outbuf,inlen * 2 + 2)) == 0)
	{
		strRet = outbuf;
	}

	if (ret==-1 || strRet=="")
		strRet =inbuf;
	delete [] outbuf;
	return strRet;
}

/*iso8859-1 to UTF8*/
 std::string iso2utf8(const char *inbuf)
{
	std::string strRet="";
	unsigned int inlen = strlen(inbuf);
	char * outbuf = new char[inlen * 2 + 2];
	int ret=-1;
	// `==' binds tighter than `=': grouped, so ret holds the code and `ret == -1' below means it
	if ((ret = code_convert("iso-8859-1", "utf-8", inbuf, inlen,outbuf,inlen * 2 + 2)) == 0)
	{
		strRet = outbuf;
	}

	if (ret==-1 || strRet=="")
		strRet =inbuf;
	delete [] outbuf;
	return strRet;
}
 
std::string vn2u(const char* inbuf)
 {
	 std::string strRet = "";
	 unsigned int inlen = strlen(inbuf);
	 char* outbuf = new char[inlen * 2 + 2];
	 int ret = -1;
	// `==' binds tighter than `=': grouped, so ret holds the code and `ret == -1' below means it
	 if ((ret = code_convert("cp1252", "utf-8", inbuf, inlen, outbuf, inlen * 2 + 2)) == 0)
	 {
		 printf("=================%s ---- %s",inbuf, outbuf);
		 strRet = outbuf;
	 }

	 if (ret == -1 || strRet == "")
		 strRet = inbuf;
	 delete[] outbuf;
	 return strRet;
 }

 std::string utf2iso(const char *inbuf)
{
	std::string strRet="";
	unsigned int inlen = strlen(inbuf);
	char * outbuf = new char[inlen * 2 + 2];
	int ret=-1;
	// `==' binds tighter than `=': grouped, so ret holds the code and `ret == -1' below means it
	if ((ret = code_convert("utf-8", "iso-8859-1", inbuf, inlen,outbuf,inlen * 2 + 2)) == 0)
	{
		strRet = outbuf;
	}

	if (ret==-1 || strRet=="")
		strRet =inbuf;
	delete [] outbuf;
	return strRet;
}

 ENGINE_API std::string gbk2iso_(LPSTR inbuf)
{
	std::string nTempString;
	nTempString =gbk2iso(inbuf);
	return nTempString;
}

 ENGINE_API std::string iso2gbk_(LPSTR inbuf)
{
	std::string nTempString;
	nTempString = iso2gbk(inbuf);
	return nTempString;
}

ENGINE_API  std::string iso2utf8_(LPSTR inbuf)
{
	std::string nTempString;
	nTempString = iso2utf8(inbuf);
	return nTempString;
}

 ENGINE_API std::string utf2iso_(LPSTR inbuf)
{
	std::string nTempString;
	nTempString = utf2iso(inbuf);
	return nTempString;
}
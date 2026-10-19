//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KRandom.cpp
// Date:	2000.08.08
// Code:	WangWei(Daphnis)
// Desc:	Random Number Generate Functions
//---------------------------------------------------------------------------
#include "KWin32.h"
#include "KRandom.h"
#include "time.h"
#include "KLogFile.h"
//---------------------------------------------------------------------------
#define IM 139968
#define IA 3877
#define IC 29573
#define MAX_COUNT 1100000
#define MIX_COUNT  42
//---------------------------------------------------------------------------4294967295
static UINT nRandomSeed = 42;  //unsigned long long		UINT
static bool  isInit =false; 
#define RAND_MAX 0x7fff

unsigned int RandomNum(unsigned int min_num, unsigned int max_num)
{
	if (min_num > max_num)
	{
		unsigned int  tmp_num = min_num;
		min_num = max_num;
		max_num = tmp_num;
	}

	unsigned int interval_num = max_num - min_num;

	if (interval_num <= 0)
		return 0;

	if (interval_num < RAND_MAX)
		return min_num + (rand() % interval_num);
	else
		return min_num + interval_num*((rand() % RAND_MAX) * 1.0 / RAND_MAX);
		//return min_num + unsigned int(((rand() % RAND_MAX) * 1.0 / RAND_MAX) * interval_num);
}

//---------------------------------------------------------------------------
// 函数:	RandomnSeed
// 功能:	设置随机数种子
// 参数:	nRandomSeed	:	随机数种子
// 返回:	void
//---------------------------------------------------------------------------
void g_RandomSeed(UINT nSeed)  
{
	if  (!isInit)
	{
		isInit = true;
		srand((unsigned)time(NULL));
	}

	nRandomSeed = nSeed;
}

//---------------------------------------------------------------------------
// 函数:	Random
// 功能:	返回一个小于nMax的随机整数
// 参数:	nMax	:	最大值
// 返回:	一个小于nMax的随机数
//---------------------------------------------------------------------------
UINT g_Random(UINT nMax)
{
	if  (!isInit)
	{
		isInit = true;
		srand((unsigned)time(NULL));
	}

	if (nMax)
	{
		nRandomSeed = nRandomSeed * IA + IC;	  //1100000*3877+29573=4264,729,537
		//4294,967,295
		if (nRandomSeed<0)	//溢出了
		{
			nRandomSeed=RandomNum(MIX_COUNT,MAX_COUNT);
			if  (g_pCLogFile)
			{
				char lszErrMsg[64];
				sprintf(lszErrMsg,"%s","----A随机种子,溢出了----");
				g_pCLogFile->gTraceLogFile(lszErrMsg, strlen(lszErrMsg), "g_Random");
			}
		}

		return nRandomSeed%nMax; //取余数				  
	}
	else
		return 0;
}
//---------------------------------------------------------------------------
// 函数:	GetRandomSeed
// 功能:	取得当时的伪随机种子
// 返回:	返回当前的伪随机种子
//---------------------------------------------------------------------------
UINT g_GetRandomSeed()
{
	if  (!isInit)
	{
		isInit = true;
		srand((unsigned)time(NULL));
	}

	if  (nRandomSeed<0)
	{
		nRandomSeed= RandomNum(MIX_COUNT,MAX_COUNT);
		if  (g_pCLogFile)
		{
			char lszErrMsg[64];
			sprintf(lszErrMsg,"%s","----B随机种子,溢出了----");
			g_pCLogFile->gTraceLogFile(lszErrMsg, strlen(lszErrMsg), "g_Random");
		}

	}
	return nRandomSeed;
}

/*//---------------------------------------------------------------------------
// 函数:	Random
// 功能:	返回一个小于nMax的随机整数
// 参数:	nMax	:	最大值
// 返回:	一个小于nMax的随机数
//---------------------------------------------------------------------------
unsigned __int64 g_Random(UINT nMax)
{
	if (nMax)
	{
		//if (nRandomSeed>500000)	 //做个上限判断 20亿
		//    nRandomSeed=42;
		nRandomSeed = nRandomSeed * IA + IC;	  //19W
		return nRandomSeed%nMax; //取余数				  
	}
	else
	{
		return 0;
	}
}
//---------------------------------------------------------------------------
// 函数:	GetRandomSeed
// 功能:	取得当时的伪随机种子
// 返回:	返回当前的伪随机种子
//---------------------------------------------------------------------------
unsigned __int64 g_GetRandomSeed()
{
	return nRandomSeed;
} */

//取商
/*
DWORD TakeTrader(DWORD a,DWORD b)
{
	int nRet = 0;
	//DWORD nYuShu=0;
	__asm
	{
		mov eax,a
			mov ecx,b
			xor edx,edx
			idiv ecx
			mov nRet,eax
			//mov nYuShu,edx
	}
	return nRet;
}
//取余数
DWORD TakeRemainder(DWORD a,DWORD b)
{
	DWORD nRet = 0;
	//	DWORD nYuShu=0;
	__asm
	{
		mov eax,a
			mov ecx,b
			xor edx,edx
			idiv ecx
			mov nRet,edx
			//mov nYuShu,edx
	}
	return nRet;
}
 */

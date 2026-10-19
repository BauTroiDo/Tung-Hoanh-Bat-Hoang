//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KRandom.h
// Date:	2000.08.08
// Code:	WangWei(Daphnis)
// Desc:	Header File
//---------------------------------------------------------------------------
#ifndef KRandom_H
#define KRandom_H
//---------------------------------------------------------------------------
//ENGINE_API unsigned __int64 g_Random(UINT nMax);
//ENGINE_API  unsigned __int64 g_GetRandomSeed();
ENGINE_API void g_RandomSeed(UINT nSeed);
ENGINE_API UINT g_GetRandomSeed();
ENGINE_API UINT g_Random(UINT nMax);
ENGINE_API DWORD TakeTrader(DWORD a,DWORD b);       //»°…Ã
ENGINE_API DWORD TakeRemainder(DWORD a,DWORD b);	//»°”‡

//---------------------------------------------------------------------------
#endif

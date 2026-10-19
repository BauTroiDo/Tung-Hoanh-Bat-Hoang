//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   ASProtectFunc.h
//  Version     :   1.0
//  Creater     :   Freeway Chen
//  Date        :   2004-6-3 18:16:31
//  Comment     :   
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _ASPROTECTFUNC_H_ 
#define _ASPROTECTFUNC_H_   1

#include "protocol/kprotocoldef.h"

#if (defined(KSG_ANTI_PLUGIN) && !defined(_SERVER))

VOID ASPR_Init();

VOID ASPR_UnInit();

VOID ASPR_SetKey();

VOID ASPR_Encrypt();

VOID ASPR_Decrypt();

BOOL ASPR_EnvelopeCheck();

#define GET_RET_ADDRESS(pRetAddress, nParam)    \
    LPVOID *ppchRetAddress = (LPVOID *)&(nParam);   \
    ppchRetAddress--;                           \
    LPVOID pRetAddress = *ppchRetAddress;

#define MAX_SELF_ADDRESS    (0x00400000 +  (16 * (1024 * 1024)))



#endif

#endif  // _ASPROTECTFUNC_H_

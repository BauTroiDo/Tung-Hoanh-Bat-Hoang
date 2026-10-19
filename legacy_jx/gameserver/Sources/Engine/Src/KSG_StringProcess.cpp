//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   KSG_StringProcess.cpp
//  Version     :   1.0
//  Creater     :   Freeway Chen
//  Date        :   2003-8-1 12:45:11
//  Comment     :   Process String to Int, Skip Symbol
//
//////////////////////////////////////////////////////////////////////////////////////

#include "KWin32.h"
#include <ctype.h>
#include "KSG_StringProcess.h"


int KSG_StringGetInt(const char **ppcszString, int nDefaultValue)
{
    int nResult = false;
    int nRetValue = 0;
    int nNegSignFlag = false;
    int nRetValueValidFlag = false;
    const char *pcszString = NULL;

    if (!ppcszString)
        goto Exit0;
    
    pcszString = *ppcszString;

    if (!pcszString)
        goto Exit0;

    while (isspace(*pcszString))
        pcszString++;
    
    if ((*pcszString) == '\0')
        goto Exit0;

    if ((*pcszString) == '-')
    {
        nNegSignFlag = true;
        pcszString++;

        // Skip Prev Space
        while (isspace(*pcszString))
            pcszString++;

        if ((*pcszString) == '\0')
            goto Exit0;
    }

    
    while (isdigit(*pcszString))
    {
        nRetValueValidFlag = true;

        nRetValue = nRetValue * 10 +  ((int)(*pcszString - '0'));

        pcszString++;
    }

    nResult = true;
Exit0:

    if (pcszString)
    {
        if (ppcszString)
            *ppcszString = pcszString;
    }

    if (nNegSignFlag)
        nRetValue = -nRetValue;

    if (
        (!nResult) ||
        (!nRetValueValidFlag)
    )
        nRetValue = nDefaultValue; 


    return nRetValue;
}


bool KSG_StringSkipSymbol(const char **ppcszString, int nSymbol)
{
    bool bResult = false;
    const char *pcszString = NULL;

    if (!ppcszString)
        goto Exit0;
    
    pcszString = *ppcszString;

    if (!pcszString)
        goto Exit0;

    while (isspace(*pcszString))
        pcszString++;
    
    if (((unsigned)(*pcszString)) != (unsigned)nSymbol)
        goto Exit0;

    pcszString++;   // Skip Symbol

    bResult = true;
Exit0:

    if (pcszString)
    {
        if (ppcszString)
            *ppcszString = pcszString;
    }

    return bResult;
}

int32_t KSG_GetCurSec()
{
    time_t curtime;
    time(&curtime);

    struct tm timeinfo;
#ifdef _WIN32
    localtime_s(&timeinfo, &curtime);  // Windows (VS Compiler)
#else
    localtime_r(&curtime, &timeinfo);  // Linux/macOS
#endif

    // YYYYMMDDHH (VD: 2024040315 -> ngay 3/4/2024, 15h)
    int32_t timeCode = (timeinfo.tm_year + 1900) * 1000000 + 
                       (timeinfo.tm_mon + 1) * 10000 + 
                       timeinfo.tm_mday * 100 + 
                       timeinfo.tm_hour;
    
    return timeCode; // int32_t
}


#include <math.h>

#define BLOCKNUM 9
static int int_pow10(int t) {
    int result = 1;
    while (t-- > 0) result *= 10;
    return result;
}


int KSG_StringSetValue(int t, int n, int p, int v)
{
    // The p-th block of t digits of a nine-digit number, replaced by v.  This used to
    // spell that as three char[BLOCKNUM] buffers, an itoa, a memmove and two memcpy, and
    // every one of those steps was off: str[BLOCKNUM] = '\0' wrote one element past the
    // nine-byte array, itoa put ten digits and a sign into nine bytes whenever n did not
    // fit, and t - (l2 - t) is the length of the second copy - negative for a block that
    // overflows, which memcpy reads as a huge one.  The digits are arithmetic; nothing
    // here needs a buffer, and p * t <= BLOCKNUM now comes from a test rather than from a
    // division by zero when t is 0.
    if (t < 1 || t > BLOCKNUM) return n;
    if (p < 1 || p > BLOCKNUM / t) return n;
    if (v < 0 || v >= int_pow10(t)) return n;
    if (n < 0 || n >= int_pow10(BLOCKNUM)) return n;
    const int nShift = int_pow10(BLOCKNUM - p * t);
    const int nBlock = (n / nShift) % int_pow10(t);
    return n + (v - nBlock) * nShift;
}

int KSG_StringGetValue(int t, int n, int p)
{
    // the same block read out of the same nine digits, without the str1/str2 shuffle that
    // itoa'd a ten-digit n into nine bytes and left str2 uninitialised for atoi when the
    // scan below matched nothing
    if (t < 1 || t > BLOCKNUM) return n;
    if (p < 1 || p > BLOCKNUM / t) return n;
    if (n < 0 || n >= int_pow10(BLOCKNUM)) return n;
    return (n / int_pow10(BLOCKNUM - p * t)) % int_pow10(t);
}

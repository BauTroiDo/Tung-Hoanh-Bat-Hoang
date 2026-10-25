#ifndef KPLAYER_ANTI_ADDICT_H
#define KPLAYER_ANTI_ADDICT_H

#include "KWin32.h"

#ifndef MAX_PLAYER
#define MAX_PLAYER 1200
#endif

//-----------------------------------------------------------------------------
// V9 Anti-Addiction State per Player
// Threshold 1: 10800s (3.0h) -> Degree 1 (Half Fatigue, 50% exp)
// Threshold 2: 18000s (5.0h) -> Degree 2 (Over Fatigue, 0% exp)
//-----------------------------------------------------------------------------
struct KPlayerAntiAddict {
    DWORD dwOnlineTime;
    DWORD dwTiredTime;
    DWORD dwRelaxTime;
    DWORD dwLastDegree;

    KPlayerAntiAddict()
    {
        Clear();
    }

    void Clear()
    {
        dwOnlineTime = 0;
        dwTiredTime = 0;
        dwRelaxTime = 0;
        dwLastDegree = 0;
    }

    int GetDegree() const
    {
        if (dwTiredTime >= 18000)
            return 2;
        if (dwTiredTime >= 10800)
            return 1;
        return 0;
    }
};

extern CORE_API KPlayerAntiAddict g_PlayerAntiAddict[MAX_PLAYER];

CORE_API void AntiAddict_OnPlayerActive(int nPlayerIndex);
CORE_API void AntiAddict_OnPlayerRelease(int nPlayerIndex);
CORE_API BOOL AntiAddict_SendSyncPacket(int nPlayerIndex);

#endif // KPLAYER_ANTI_ADDICT_H

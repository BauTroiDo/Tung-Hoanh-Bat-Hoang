#ifndef CORESERVERDATADEF_H
#define CORESERVERDATADEF_H

#ifndef _STANDALONE 
#include "../../../lib/S3DBInterface.h"
#else
#include "S3DBInterface.h"
#endif

#include "CoreUseNameDef.h"

class KMutex;

struct KCoreConnectInfo
{
	int	nNumPlayer;
	char nClientIp[64];
};

struct KNpcInfo
{
	int	nMapIndx;
    int nPosX;
	int nPosY;
};

typedef struct
{
	char				szAccount[ACC_LENGTH];
	int					nRoleCount;
	S3DBI_RoleBaseInfo	PlayerBaseInfo[MAX_PLAYER_IN_ACCOUNT];
	int					nStep;
	unsigned int		nParam;
	UINT				dwKey;
} LoginData;

#include "KPlayerDef.h"
#endif

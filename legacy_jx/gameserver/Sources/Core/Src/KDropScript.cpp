#include "KCore.h"
#include "KNpc.h"    //原来没有的

#include "KDropScript.h"
#include "LuaFuns.h"
#include "KFilePath.h"
#include "KDebug.h"
#ifndef WIN32
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#else 
#include <io.h>
#include <direct.h>
#endif
//#include "Shlwapi.h"

#ifdef _SERVER

KDropBinTree g_DropBinTree;

//重载操作符号
/*
int	operator<(KItemDropRateNode ScriptLeft, KItemDropRateNode ScriptRight)
{
	return ScriptLeft.GetScriptID() < ScriptRight.GetScriptID();
};

int operator==(KItemDropRateNode ScriptLeft, KItemDropRateNode ScriptRight)
{
	return ScriptLeft.GetScriptID() == ScriptRight.GetScriptID();
}; */

#endif

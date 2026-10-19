#ifndef LUAFUNS_H
#define LUAFUNS_H
#include "KCore.h"
#include "KScriptList.h"
#include "KScriptCache.h"

extern CORE_API TLua_Funcs GameScriptFuns[];
extern CORE_API int	g_GetGameScriptFunNum();

extern CORE_API TLua_Funcs WorldScriptFuns[] ;
extern CORE_API int	g_GetWorldScriptFunNum();

//脚本列表实例
//Questions:单机版剧情脚本列表在网络版暂用.
extern  KScriptList		g_StoryScriptList;	//单机版剧情脚本列表
// V9 script-context helpers, defined in ScriptFuns.cpp; the tag spelling keeps this header readable by files that do not include LuaLib.h.
struct lua_State;
int GetSubWorldIndex(lua_State * L);
int GetPlayerIndex(lua_State * L);
#endif


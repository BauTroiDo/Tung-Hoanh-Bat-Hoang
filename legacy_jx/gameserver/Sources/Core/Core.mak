# Microsoft Developer Studio Generated NMAKE File, Based on Core.dsp
!IF $(CFG)" == "
CFG=Core - Win32 Client Debug
!MESSAGE No configuration specified. Defaulting to Core - Win32 Client Debug.
!ENDIF 

!IF "$(CFG)" != "Core - Win32 Server Debug" && "$(CFG)" != "Core - Win32 Server Release" && "$(CFG)" != "Core - Win32 Client Debug" && "$(CFG)" != "Core - Win32 Client Release"
!MESSAGE 指定的配置 "$(CFG)" 无效.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Core.mak" CFG="Core - Win32 Client Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Core - Win32 Server Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Core - Win32 Server Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Core - Win32 Client Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Core - Win32 Client Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF $(OS)" == "Windows_NT
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Core - Win32 Server Debug"

OUTDIR=.\ServerDebug
INTDIR=.\ServerDebug
# 开始自定义宏
OutDir=.\ServerDebug
# 结束自定义宏

ALL : "$(OUTDIR)\CoreServer.dll" "$(OUTDIR)\Core.bsc"


CLEAN :
	-@erase "$(INTDIR)\Core.pch"
	-@erase "$(INTDIR)\CoreServerShell.obj"
	-@erase "$(INTDIR)\CoreServerShell.sbr"
	-@erase "$(INTDIR)\DBTableS.obj"
	-@erase "$(INTDIR)\DBTableS.sbr"
	-@erase "$(INTDIR)\KBasPropTbl.obj"
	-@erase "$(INTDIR)\KBasPropTbl.sbr"
	-@erase "$(INTDIR)\KBuySell.obj"
	-@erase "$(INTDIR)\KBuySell.sbr"
	-@erase "$(INTDIR)\KCore.obj"
	-@erase "$(INTDIR)\KCore.sbr"
	-@erase "$(INTDIR)\KDropScript.obj"
	-@erase "$(INTDIR)\KDropScript.sbr"
	-@erase "$(INTDIR)\KFaction.obj"
	-@erase "$(INTDIR)\KFaction.sbr"
	-@erase "$(INTDIR)\KForBitGua.obj"
	-@erase "$(INTDIR)\KForBitGua.sbr"
	-@erase "$(INTDIR)\KGMCommand.obj"
	-@erase "$(INTDIR)\KGMCommand.sbr"
	-@erase "$(INTDIR)\KGMProcess.obj"
	-@erase "$(INTDIR)\KGMProcess.sbr"
	-@erase "$(INTDIR)\KInventory.obj"
	-@erase "$(INTDIR)\KInventory.sbr"
	-@erase "$(INTDIR)\KItem.obj"
	-@erase "$(INTDIR)\KItem.sbr"
	-@erase "$(INTDIR)\KItemChangeRes.obj"
	-@erase "$(INTDIR)\KItemChangeRes.sbr"
	-@erase "$(INTDIR)\KItemGenerator.obj"
	-@erase "$(INTDIR)\KItemGenerator.sbr"
	-@erase "$(INTDIR)\KItemList.obj"
	-@erase "$(INTDIR)\KItemList.sbr"
	-@erase "$(INTDIR)\KItemSet.obj"
	-@erase "$(INTDIR)\KItemSet.sbr"
	-@erase "$(INTDIR)\Kjxpathfinder.obj"
	-@erase "$(INTDIR)\Kjxpathfinder.sbr"
	-@erase "$(INTDIR)\KLadder.obj"
	-@erase "$(INTDIR)\KLadder.sbr"
	-@erase "$(INTDIR)\KLittleMap.obj"
	-@erase "$(INTDIR)\KLittleMap.sbr"
	-@erase "$(INTDIR)\KLogFile.obj"
	-@erase "$(INTDIR)\KLogFile.sbr"
	-@erase "$(INTDIR)\KMagicDesc.obj"
	-@erase "$(INTDIR)\KMagicDesc.sbr"
	-@erase "$(INTDIR)\KMapMusic.obj"
	-@erase "$(INTDIR)\KMapMusic.sbr"
	-@erase "$(INTDIR)\KMath.obj"
	-@erase "$(INTDIR)\KMath.sbr"
	-@erase "$(INTDIR)\KMission.obj"
	-@erase "$(INTDIR)\KMission.sbr"
	-@erase "$(INTDIR)\KMissle.obj"
	-@erase "$(INTDIR)\KMissle.sbr"
	-@erase "$(INTDIR)\KMissleRes.obj"
	-@erase "$(INTDIR)\KMissleRes.sbr"
	-@erase "$(INTDIR)\KMissleSet.obj"
	-@erase "$(INTDIR)\KMissleSet.sbr"
	-@erase "$(INTDIR)\KNewProtocolProcess.obj"
	-@erase "$(INTDIR)\KNewProtocolProcess.sbr"
	-@erase "$(INTDIR)\KNpc.obj"
	-@erase "$(INTDIR)\KNpc.sbr"
	-@erase "$(INTDIR)\KNpcAI.obj"
	-@erase "$(INTDIR)\KNpcAI.sbr"
	-@erase "$(INTDIR)\KNpcAttribModify.obj"
	-@erase "$(INTDIR)\KNpcAttribModify.sbr"
	-@erase "$(INTDIR)\KNpcDeathCalcExp.obj"
	-@erase "$(INTDIR)\KNpcDeathCalcExp.sbr"
	-@erase "$(INTDIR)\KNpcFindPath.obj"
	-@erase "$(INTDIR)\KNpcFindPath.sbr"
	-@erase "$(INTDIR)\KNpcGold.obj"
	-@erase "$(INTDIR)\KNpcGold.sbr"
	-@erase "$(INTDIR)\KNpcRes.obj"
	-@erase "$(INTDIR)\KNpcRes.sbr"
	-@erase "$(INTDIR)\KNpcResList.obj"
	-@erase "$(INTDIR)\KNpcResList.sbr"
	-@erase "$(INTDIR)\KNpcResNode.obj"
	-@erase "$(INTDIR)\KNpcResNode.sbr"
	-@erase "$(INTDIR)\KNpcSet.obj"
	-@erase "$(INTDIR)\KNpcSet.sbr"
	-@erase "$(INTDIR)\KNpcTemplate.obj"
	-@erase "$(INTDIR)\KNpcTemplate.sbr"
	-@erase "$(INTDIR)\KObj.obj"
	-@erase "$(INTDIR)\KObj.sbr"
	-@erase "$(INTDIR)\KObjSet.obj"
	-@erase "$(INTDIR)\KObjSet.sbr"
	-@erase "$(INTDIR)\KOption.obj"
	-@erase "$(INTDIR)\KOption.sbr"
	-@erase "$(INTDIR)\KPartnerSkill.obj"
	-@erase "$(INTDIR)\KPartnerSkill.sbr"
	-@erase "$(INTDIR)\Kpathfinder.obj"
	-@erase "$(INTDIR)\Kpathfinder.sbr"
	-@erase "$(INTDIR)\KPlayer.obj"
	-@erase "$(INTDIR)\KPlayer.sbr"
	-@erase "$(INTDIR)\KPlayerChat.obj"
	-@erase "$(INTDIR)\KPlayerChat.sbr"
	-@erase "$(INTDIR)\KPlayerDBFuns.obj"
	-@erase "$(INTDIR)\KPlayerDBFuns.sbr"
	-@erase "$(INTDIR)\KPlayerFaction.obj"
	-@erase "$(INTDIR)\KPlayerFaction.sbr"
	-@erase "$(INTDIR)\KPlayerFuYuan.obj"
	-@erase "$(INTDIR)\KPlayerFuYuan.sbr"
	-@erase "$(INTDIR)\KPlayerMenuState.obj"
	-@erase "$(INTDIR)\KPlayerMenuState.sbr"
	-@erase "$(INTDIR)\KPlayerPartner.obj"
	-@erase "$(INTDIR)\KPlayerPartner.sbr"
	-@erase "$(INTDIR)\KPlayerPK.obj"
	-@erase "$(INTDIR)\KPlayerPK.sbr"
	-@erase "$(INTDIR)\KPlayerReBorn.obj"
	-@erase "$(INTDIR)\KPlayerReBorn.sbr"
	-@erase "$(INTDIR)\KPlayerRepute.obj"
	-@erase "$(INTDIR)\KPlayerRepute.sbr"
	-@erase "$(INTDIR)\KPlayerSet.obj"
	-@erase "$(INTDIR)\KPlayerSet.sbr"
	-@erase "$(INTDIR)\KPlayerTask.obj"
	-@erase "$(INTDIR)\KPlayerTask.sbr"
	-@erase "$(INTDIR)\KPlayerTeam.obj"
	-@erase "$(INTDIR)\KPlayerTeam.sbr"
	-@erase "$(INTDIR)\KPlayerTong.obj"
	-@erase "$(INTDIR)\KPlayerTong.sbr"
	-@erase "$(INTDIR)\KPlayerTrade.obj"
	-@erase "$(INTDIR)\KPlayerTrade.sbr"
	-@erase "$(INTDIR)\KProtocol.obj"
	-@erase "$(INTDIR)\KProtocol.sbr"
	-@erase "$(INTDIR)\KProtocolProcess.obj"
	-@erase "$(INTDIR)\KProtocolProcess.sbr"
	-@erase "$(INTDIR)\KRegion.obj"
	-@erase "$(INTDIR)\KRegion.sbr"
	-@erase "$(INTDIR)\KScriptValueSet.obj"
	-@erase "$(INTDIR)\KScriptValueSet.sbr"
	-@erase "$(INTDIR)\KSellItem.obj"
	-@erase "$(INTDIR)\KSellItem.sbr"
	-@erase "$(INTDIR)\KSkillList.obj"
	-@erase "$(INTDIR)\KSkillList.sbr"
	-@erase "$(INTDIR)\KSkillManager.obj"
	-@erase "$(INTDIR)\KSkillManager.sbr"
	-@erase "$(INTDIR)\KSkills.obj"
	-@erase "$(INTDIR)\KSkills.sbr"
	-@erase "$(INTDIR)\KSkillSpecial.obj"
	-@erase "$(INTDIR)\KSkillSpecial.sbr"
	-@erase "$(INTDIR)\KSortScript.obj"
	-@erase "$(INTDIR)\KSortScript.sbr"
	-@erase "$(INTDIR)\KSprControl.obj"
	-@erase "$(INTDIR)\KSprControl.sbr"
	-@erase "$(INTDIR)\KSubWorld.obj"
	-@erase "$(INTDIR)\KSubWorld.sbr"
	-@erase "$(INTDIR)\KSubWorldSet.obj"
	-@erase "$(INTDIR)\KSubWorldSet.sbr"
	-@erase "$(INTDIR)\KTaskFuns.obj"
	-@erase "$(INTDIR)\KTaskFuns.sbr"
	-@erase "$(INTDIR)\KThiefSkill.obj"
	-@erase "$(INTDIR)\KThiefSkill.sbr"
	-@erase "$(INTDIR)\KViewItem.obj"
	-@erase "$(INTDIR)\KViewItem.sbr"
	-@erase "$(INTDIR)\KWeather.obj"
	-@erase "$(INTDIR)\KWeather.sbr"
	-@erase "$(INTDIR)\KWeatherMgr.obj"
	-@erase "$(INTDIR)\KWeatherMgr.sbr"
	-@erase "$(INTDIR)\KWorldMsg.obj"
	-@erase "$(INTDIR)\KWorldMsg.sbr"
	-@erase "$(INTDIR)\S3P_MSSQLServer_Result.obj"
	-@erase "$(INTDIR)\S3P_MSSQLServer_Result.sbr"
	-@erase "$(INTDIR)\S3PDB_MSSQLServer_Connection.obj"
	-@erase "$(INTDIR)\S3PDB_MSSQLServer_Connection.sbr"
	-@erase "$(INTDIR)\S3PDBConVBC.obj"
	-@erase "$(INTDIR)\S3PDBConVBC.sbr"
	-@erase "$(INTDIR)\S3PResultVBC.obj"
	-@erase "$(INTDIR)\S3PResultVBC.sbr"
	-@erase "$(INTDIR)\ScriptFuns.obj"
	-@erase "$(INTDIR)\ScriptFuns.sbr"
	-@erase "$(INTDIR)\stdafx.obj"
	-@erase "$(INTDIR)\stdafx.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\Core.bsc"
	-@erase "$(OUTDIR)\CoreServer.dll"
	-@erase "$(OUTDIR)\CoreServer.exp"
	-@erase "$(OUTDIR)\CoreServer.lib"
	-@erase "$(OUTDIR)\CoreServer.map"
	-@erase "$(OUTDIR)\CoreServer.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MTd /W2 /Gm /GX /Zi /Od /I "..\engine\src" /I "..\engine\include" /I "..\Network" /I "..\..\Headers" /D "_DEBUG" /D "_SERVER" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CORE_EXPORTS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\Core.pch" /Yu"KCore.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /LD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Core.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\KPartnerSkill.sbr" \
	"$(INTDIR)\KPlayerPartner.sbr" \
	"$(INTDIR)\KThiefSkill.sbr" \
	"$(INTDIR)\KMissle.sbr" \
	"$(INTDIR)\KMissleRes.sbr" \
	"$(INTDIR)\KMissleSet.sbr" \
	"$(INTDIR)\KSkillList.sbr" \
	"$(INTDIR)\KSkillManager.sbr" \
	"$(INTDIR)\KSkills.sbr" \
	"$(INTDIR)\KSkillSpecial.sbr" \
	"$(INTDIR)\KFaction.sbr" \
	"$(INTDIR)\KNpc.sbr" \
	"$(INTDIR)\KNpcAI.sbr" \
	"$(INTDIR)\KNpcAttribModify.sbr" \
	"$(INTDIR)\KNpcDeathCalcExp.sbr" \
	"$(INTDIR)\KNpcFindPath.sbr" \
	"$(INTDIR)\KNpcGold.sbr" \
	"$(INTDIR)\KNpcRes.sbr" \
	"$(INTDIR)\KNpcResList.sbr" \
	"$(INTDIR)\KNpcResNode.sbr" \
	"$(INTDIR)\KNpcSet.sbr" \
	"$(INTDIR)\KNpcTemplate.sbr" \
	"$(INTDIR)\KPlayer.sbr" \
	"$(INTDIR)\KPlayerChat.sbr" \
	"$(INTDIR)\KPlayerDBFuns.sbr" \
	"$(INTDIR)\KPlayerFaction.sbr" \
	"$(INTDIR)\KPlayerFuYuan.sbr" \
	"$(INTDIR)\KPlayerMenuState.sbr" \
	"$(INTDIR)\KPlayerPK.sbr" \
	"$(INTDIR)\KPlayerReBorn.sbr" \
	"$(INTDIR)\KPlayerRepute.sbr" \
	"$(INTDIR)\KPlayerSet.sbr" \
	"$(INTDIR)\KPlayerTask.sbr" \
	"$(INTDIR)\KPlayerTeam.sbr" \
	"$(INTDIR)\KPlayerTong.sbr" \
	"$(INTDIR)\KPlayerTrade.sbr" \
	"$(INTDIR)\KSprControl.sbr" \
	"$(INTDIR)\KBasPropTbl.sbr" \
	"$(INTDIR)\KBuySell.sbr" \
	"$(INTDIR)\KInventory.sbr" \
	"$(INTDIR)\KItem.sbr" \
	"$(INTDIR)\KItemChangeRes.sbr" \
	"$(INTDIR)\KItemGenerator.sbr" \
	"$(INTDIR)\KItemList.sbr" \
	"$(INTDIR)\KItemSet.sbr" \
	"$(INTDIR)\KObj.sbr" \
	"$(INTDIR)\KObjSet.sbr" \
	"$(INTDIR)\KSellItem.sbr" \
	"$(INTDIR)\KViewItem.sbr" \
	"$(INTDIR)\KRegion.sbr" \
	"$(INTDIR)\KCore.sbr" \
	"$(INTDIR)\KSubWorld.sbr" \
	"$(INTDIR)\KSubWorldSet.sbr" \
	"$(INTDIR)\KWeatherMgr.sbr" \
	"$(INTDIR)\S3P_MSSQLServer_Result.sbr" \
	"$(INTDIR)\S3PDB_MSSQLServer_Connection.sbr" \
	"$(INTDIR)\S3PDBConVBC.sbr" \
	"$(INTDIR)\S3PResultVBC.sbr" \
	"$(INTDIR)\KWorldMsg.sbr" \
	"$(INTDIR)\KMath.sbr" \
	"$(INTDIR)\KGMProcess.sbr" \
	"$(INTDIR)\KNewProtocolProcess.sbr" \
	"$(INTDIR)\KProtocol.sbr" \
	"$(INTDIR)\KProtocolProcess.sbr" \
	"$(INTDIR)\ScriptFuns.sbr" \
	"$(INTDIR)\KScriptValueSet.sbr" \
	"$(INTDIR)\KSortScript.sbr" \
	"$(INTDIR)\CoreServerShell.sbr" \
	"$(INTDIR)\Kjxpathfinder.sbr" \
	"$(INTDIR)\Kpathfinder.sbr" \
	"$(INTDIR)\KLittleMap.sbr" \
	"$(INTDIR)\KWeather.sbr" \
	"$(INTDIR)\KMagicDesc.sbr" \
	"$(INTDIR)\KGMCommand.sbr" \
	"$(INTDIR)\KMission.sbr" \
	"$(INTDIR)\KTaskFuns.sbr" \
	"$(INTDIR)\KOption.sbr" \
	"$(INTDIR)\KMapMusic.sbr" \
	"$(INTDIR)\KLadder.sbr" \
	"$(INTDIR)\KForBitGua.sbr" \
	"$(INTDIR)\KDropScript.sbr" \
	"$(INTDIR)\KLogFile.sbr" \
	"$(INTDIR)\DBTableS.sbr" \
	"$(INTDIR)\stdafx.sbr"

"$(OUTDIR)\Core.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib lualibdll.lib Ws2_32.lib Winmm.lib Shlwapi.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\CoreServer.pdb" /map:"$(INTDIR)\CoreServer.map" /debug /machine:I386 /nodefaultlib:"libc.lib" /nodefaultlib:"libcmt.lib" /nodefaultlib:"msvcrt.lib" /nodefaultlib:"libcd.lib" /nodefaultlib:"msvcrtd.lib" /out:"$(OUTDIR)\CoreServer.dll" /implib:"$(OUTDIR)\CoreServer.lib" /pdbtype:sept /libpath:"../../lib/" 
LINK32_OBJS= \
	"$(INTDIR)\KPartnerSkill.obj" \
	"$(INTDIR)\KPlayerPartner.obj" \
	"$(INTDIR)\KThiefSkill.obj" \
	"$(INTDIR)\KMissle.obj" \
	"$(INTDIR)\KMissleRes.obj" \
	"$(INTDIR)\KMissleSet.obj" \
	"$(INTDIR)\KSkillList.obj" \
	"$(INTDIR)\KSkillManager.obj" \
	"$(INTDIR)\KSkills.obj" \
	"$(INTDIR)\KSkillSpecial.obj" \
	"$(INTDIR)\KFaction.obj" \
	"$(INTDIR)\KNpc.obj" \
	"$(INTDIR)\KNpcAI.obj" \
	"$(INTDIR)\KNpcAttribModify.obj" \
	"$(INTDIR)\KNpcDeathCalcExp.obj" \
	"$(INTDIR)\KNpcFindPath.obj" \
	"$(INTDIR)\KNpcGold.obj" \
	"$(INTDIR)\KNpcRes.obj" \
	"$(INTDIR)\KNpcResList.obj" \
	"$(INTDIR)\KNpcResNode.obj" \
	"$(INTDIR)\KNpcSet.obj" \
	"$(INTDIR)\KNpcTemplate.obj" \
	"$(INTDIR)\KPlayer.obj" \
	"$(INTDIR)\KPlayerChat.obj" \
	"$(INTDIR)\KPlayerDBFuns.obj" \
	"$(INTDIR)\KPlayerFaction.obj" \
	"$(INTDIR)\KPlayerFuYuan.obj" \
	"$(INTDIR)\KPlayerMenuState.obj" \
	"$(INTDIR)\KPlayerPK.obj" \
	"$(INTDIR)\KPlayerReBorn.obj" \
	"$(INTDIR)\KPlayerRepute.obj" \
	"$(INTDIR)\KPlayerSet.obj" \
	"$(INTDIR)\KPlayerTask.obj" \
	"$(INTDIR)\KPlayerTeam.obj" \
	"$(INTDIR)\KPlayerTong.obj" \
	"$(INTDIR)\KPlayerTrade.obj" \
	"$(INTDIR)\KSprControl.obj" \
	"$(INTDIR)\KBasPropTbl.obj" \
	"$(INTDIR)\KBuySell.obj" \
	"$(INTDIR)\KInventory.obj" \
	"$(INTDIR)\KItem.obj" \
	"$(INTDIR)\KItemChangeRes.obj" \
	"$(INTDIR)\KItemGenerator.obj" \
	"$(INTDIR)\KItemList.obj" \
	"$(INTDIR)\KItemSet.obj" \
	"$(INTDIR)\KObj.obj" \
	"$(INTDIR)\KObjSet.obj" \
	"$(INTDIR)\KSellItem.obj" \
	"$(INTDIR)\KViewItem.obj" \
	"$(INTDIR)\KRegion.obj" \
	"$(INTDIR)\KCore.obj" \
	"$(INTDIR)\KSubWorld.obj" \
	"$(INTDIR)\KSubWorldSet.obj" \
	"$(INTDIR)\KWeatherMgr.obj" \
	"$(INTDIR)\S3P_MSSQLServer_Result.obj" \
	"$(INTDIR)\S3PDB_MSSQLServer_Connection.obj" \
	"$(INTDIR)\S3PDBConVBC.obj" \
	"$(INTDIR)\S3PResultVBC.obj" \
	"$(INTDIR)\KWorldMsg.obj" \
	"$(INTDIR)\KMath.obj" \
	"$(INTDIR)\KGMProcess.obj" \
	"$(INTDIR)\KNewProtocolProcess.obj" \
	"$(INTDIR)\KProtocol.obj" \
	"$(INTDIR)\KProtocolProcess.obj" \
	"$(INTDIR)\ScriptFuns.obj" \
	"$(INTDIR)\KScriptValueSet.obj" \
	"$(INTDIR)\KSortScript.obj" \
	"$(INTDIR)\CoreServerShell.obj" \
	"$(INTDIR)\Kjxpathfinder.obj" \
	"$(INTDIR)\Kpathfinder.obj" \
	"$(INTDIR)\KLittleMap.obj" \
	"$(INTDIR)\KWeather.obj" \
	"$(INTDIR)\KMagicDesc.obj" \
	"$(INTDIR)\KGMCommand.obj" \
	"$(INTDIR)\KMission.obj" \
	"$(INTDIR)\KTaskFuns.obj" \
	"$(INTDIR)\KOption.obj" \
	"$(INTDIR)\KMapMusic.obj" \
	"$(INTDIR)\KLadder.obj" \
	"$(INTDIR)\KForBitGua.obj" \
	"$(INTDIR)\KDropScript.obj" \
	"$(INTDIR)\KLogFile.obj" \
	"$(INTDIR)\DBTableS.obj" \
	"$(INTDIR)\stdafx.obj" \
	"..\Library\LuaLib\Release\LuaLibDll.lib" \
	"..\..\Lib\debug\engine.lib" \
	"..\..\Lib\debug\common.lib" \
	".\Src\USBKey\EP1KDL20.LIB" \
	".\Src\USBKey\EpsForJoLib.lib" \
	"..\..\Lib\libdb41s.lib"

"$(OUTDIR)\CoreServer.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# 开始自定义宏
OutDir=.\ServerDebug
# 结束自定义宏

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\CoreServer.dll" "$(OUTDIR)\Core.bsc"
   md ..\..\..\bin\server\debug
	md ..\..\lib\debug
	copy ServerDebug\CoreServer.map ..\..\..\bin\server\Coreserver.map
	copy ServerDebug\CoreServer.pdb ..\..\..\bin\server\Coreserver.pdb
	copy ServerDebug\CoreServer.dll ..\..\..\bin\server\Coreserver.dll
	copy ServerDebug\CoreServer.map ..\..\..\bin\server\debug\Coreserver.map
	copy ServerDebug\CoreServer.pdb ..\..\..\bin\server\debug\Coreserver.pdb
	copy ServerDebug\CoreServer.dll ..\..\..\bin\server\debug\Coreserver.dll
	copy ServerDebug\CoreServer.lib ..\..\lib\debug\CoreServer.lib
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"

OUTDIR=.\Server\Release
INTDIR=.\Server\Release
# 开始自定义宏
OutDir=.\Server\Release
# 结束自定义宏

ALL : "$(OUTDIR)\CoreServer.dll" "$(OUTDIR)\Core.bsc"


CLEAN :
	-@erase "$(INTDIR)\Core.pch"
	-@erase "$(INTDIR)\CoreServerShell.obj"
	-@erase "$(INTDIR)\CoreServerShell.sbr"
	-@erase "$(INTDIR)\DBTableS.obj"
	-@erase "$(INTDIR)\DBTableS.sbr"
	-@erase "$(INTDIR)\KBasPropTbl.obj"
	-@erase "$(INTDIR)\KBasPropTbl.sbr"
	-@erase "$(INTDIR)\KBuySell.obj"
	-@erase "$(INTDIR)\KBuySell.sbr"
	-@erase "$(INTDIR)\KCore.obj"
	-@erase "$(INTDIR)\KCore.sbr"
	-@erase "$(INTDIR)\KDropScript.obj"
	-@erase "$(INTDIR)\KDropScript.sbr"
	-@erase "$(INTDIR)\KFaction.obj"
	-@erase "$(INTDIR)\KFaction.sbr"
	-@erase "$(INTDIR)\KForBitGua.obj"
	-@erase "$(INTDIR)\KForBitGua.sbr"
	-@erase "$(INTDIR)\KGMCommand.obj"
	-@erase "$(INTDIR)\KGMCommand.sbr"
	-@erase "$(INTDIR)\KGMProcess.obj"
	-@erase "$(INTDIR)\KGMProcess.sbr"
	-@erase "$(INTDIR)\KInventory.obj"
	-@erase "$(INTDIR)\KInventory.sbr"
	-@erase "$(INTDIR)\KItem.obj"
	-@erase "$(INTDIR)\KItem.sbr"
	-@erase "$(INTDIR)\KItemChangeRes.obj"
	-@erase "$(INTDIR)\KItemChangeRes.sbr"
	-@erase "$(INTDIR)\KItemGenerator.obj"
	-@erase "$(INTDIR)\KItemGenerator.sbr"
	-@erase "$(INTDIR)\KItemList.obj"
	-@erase "$(INTDIR)\KItemList.sbr"
	-@erase "$(INTDIR)\KItemSet.obj"
	-@erase "$(INTDIR)\KItemSet.sbr"
	-@erase "$(INTDIR)\Kjxpathfinder.obj"
	-@erase "$(INTDIR)\Kjxpathfinder.sbr"
	-@erase "$(INTDIR)\KLadder.obj"
	-@erase "$(INTDIR)\KLadder.sbr"
	-@erase "$(INTDIR)\KLittleMap.obj"
	-@erase "$(INTDIR)\KLittleMap.sbr"
	-@erase "$(INTDIR)\KLogFile.obj"
	-@erase "$(INTDIR)\KLogFile.sbr"
	-@erase "$(INTDIR)\KMagicDesc.obj"
	-@erase "$(INTDIR)\KMagicDesc.sbr"
	-@erase "$(INTDIR)\KMapMusic.obj"
	-@erase "$(INTDIR)\KMapMusic.sbr"
	-@erase "$(INTDIR)\KMath.obj"
	-@erase "$(INTDIR)\KMath.sbr"
	-@erase "$(INTDIR)\KMission.obj"
	-@erase "$(INTDIR)\KMission.sbr"
	-@erase "$(INTDIR)\KMissle.obj"
	-@erase "$(INTDIR)\KMissle.sbr"
	-@erase "$(INTDIR)\KMissleRes.obj"
	-@erase "$(INTDIR)\KMissleRes.sbr"
	-@erase "$(INTDIR)\KMissleSet.obj"
	-@erase "$(INTDIR)\KMissleSet.sbr"
	-@erase "$(INTDIR)\KNewProtocolProcess.obj"
	-@erase "$(INTDIR)\KNewProtocolProcess.sbr"
	-@erase "$(INTDIR)\KNpc.obj"
	-@erase "$(INTDIR)\KNpc.sbr"
	-@erase "$(INTDIR)\KNpcAI.obj"
	-@erase "$(INTDIR)\KNpcAI.sbr"
	-@erase "$(INTDIR)\KNpcAttribModify.obj"
	-@erase "$(INTDIR)\KNpcAttribModify.sbr"
	-@erase "$(INTDIR)\KNpcDeathCalcExp.obj"
	-@erase "$(INTDIR)\KNpcDeathCalcExp.sbr"
	-@erase "$(INTDIR)\KNpcFindPath.obj"
	-@erase "$(INTDIR)\KNpcFindPath.sbr"
	-@erase "$(INTDIR)\KNpcGold.obj"
	-@erase "$(INTDIR)\KNpcGold.sbr"
	-@erase "$(INTDIR)\KNpcRes.obj"
	-@erase "$(INTDIR)\KNpcRes.sbr"
	-@erase "$(INTDIR)\KNpcResList.obj"
	-@erase "$(INTDIR)\KNpcResList.sbr"
	-@erase "$(INTDIR)\KNpcResNode.obj"
	-@erase "$(INTDIR)\KNpcResNode.sbr"
	-@erase "$(INTDIR)\KNpcSet.obj"
	-@erase "$(INTDIR)\KNpcSet.sbr"
	-@erase "$(INTDIR)\KNpcTemplate.obj"
	-@erase "$(INTDIR)\KNpcTemplate.sbr"
	-@erase "$(INTDIR)\KObj.obj"
	-@erase "$(INTDIR)\KObj.sbr"
	-@erase "$(INTDIR)\KObjSet.obj"
	-@erase "$(INTDIR)\KObjSet.sbr"
	-@erase "$(INTDIR)\KOption.obj"
	-@erase "$(INTDIR)\KOption.sbr"
	-@erase "$(INTDIR)\KPartnerSkill.obj"
	-@erase "$(INTDIR)\KPartnerSkill.sbr"
	-@erase "$(INTDIR)\Kpathfinder.obj"
	-@erase "$(INTDIR)\Kpathfinder.sbr"
	-@erase "$(INTDIR)\KPlayer.obj"
	-@erase "$(INTDIR)\KPlayer.sbr"
	-@erase "$(INTDIR)\KPlayerChat.obj"
	-@erase "$(INTDIR)\KPlayerChat.sbr"
	-@erase "$(INTDIR)\KPlayerDBFuns.obj"
	-@erase "$(INTDIR)\KPlayerDBFuns.sbr"
	-@erase "$(INTDIR)\KPlayerFaction.obj"
	-@erase "$(INTDIR)\KPlayerFaction.sbr"
	-@erase "$(INTDIR)\KPlayerFuYuan.obj"
	-@erase "$(INTDIR)\KPlayerFuYuan.sbr"
	-@erase "$(INTDIR)\KPlayerMenuState.obj"
	-@erase "$(INTDIR)\KPlayerMenuState.sbr"
	-@erase "$(INTDIR)\KPlayerPartner.obj"
	-@erase "$(INTDIR)\KPlayerPartner.sbr"
	-@erase "$(INTDIR)\KPlayerPK.obj"
	-@erase "$(INTDIR)\KPlayerPK.sbr"
	-@erase "$(INTDIR)\KPlayerReBorn.obj"
	-@erase "$(INTDIR)\KPlayerReBorn.sbr"
	-@erase "$(INTDIR)\KPlayerRepute.obj"
	-@erase "$(INTDIR)\KPlayerRepute.sbr"
	-@erase "$(INTDIR)\KPlayerSet.obj"
	-@erase "$(INTDIR)\KPlayerSet.sbr"
	-@erase "$(INTDIR)\KPlayerTask.obj"
	-@erase "$(INTDIR)\KPlayerTask.sbr"
	-@erase "$(INTDIR)\KPlayerTeam.obj"
	-@erase "$(INTDIR)\KPlayerTeam.sbr"
	-@erase "$(INTDIR)\KPlayerTong.obj"
	-@erase "$(INTDIR)\KPlayerTong.sbr"
	-@erase "$(INTDIR)\KPlayerTrade.obj"
	-@erase "$(INTDIR)\KPlayerTrade.sbr"
	-@erase "$(INTDIR)\KProtocol.obj"
	-@erase "$(INTDIR)\KProtocol.sbr"
	-@erase "$(INTDIR)\KProtocolProcess.obj"
	-@erase "$(INTDIR)\KProtocolProcess.sbr"
	-@erase "$(INTDIR)\KRegion.obj"
	-@erase "$(INTDIR)\KRegion.sbr"
	-@erase "$(INTDIR)\KScriptValueSet.obj"
	-@erase "$(INTDIR)\KScriptValueSet.sbr"
	-@erase "$(INTDIR)\KSellItem.obj"
	-@erase "$(INTDIR)\KSellItem.sbr"
	-@erase "$(INTDIR)\KSkillList.obj"
	-@erase "$(INTDIR)\KSkillList.sbr"
	-@erase "$(INTDIR)\KSkillManager.obj"
	-@erase "$(INTDIR)\KSkillManager.sbr"
	-@erase "$(INTDIR)\KSkills.obj"
	-@erase "$(INTDIR)\KSkills.sbr"
	-@erase "$(INTDIR)\KSkillSpecial.obj"
	-@erase "$(INTDIR)\KSkillSpecial.sbr"
	-@erase "$(INTDIR)\KSortScript.obj"
	-@erase "$(INTDIR)\KSortScript.sbr"
	-@erase "$(INTDIR)\KSprControl.obj"
	-@erase "$(INTDIR)\KSprControl.sbr"
	-@erase "$(INTDIR)\KSubWorld.obj"
	-@erase "$(INTDIR)\KSubWorld.sbr"
	-@erase "$(INTDIR)\KSubWorldSet.obj"
	-@erase "$(INTDIR)\KSubWorldSet.sbr"
	-@erase "$(INTDIR)\KTaskFuns.obj"
	-@erase "$(INTDIR)\KTaskFuns.sbr"
	-@erase "$(INTDIR)\KThiefSkill.obj"
	-@erase "$(INTDIR)\KThiefSkill.sbr"
	-@erase "$(INTDIR)\KViewItem.obj"
	-@erase "$(INTDIR)\KViewItem.sbr"
	-@erase "$(INTDIR)\KWeather.obj"
	-@erase "$(INTDIR)\KWeather.sbr"
	-@erase "$(INTDIR)\KWeatherMgr.obj"
	-@erase "$(INTDIR)\KWeatherMgr.sbr"
	-@erase "$(INTDIR)\KWorldMsg.obj"
	-@erase "$(INTDIR)\KWorldMsg.sbr"
	-@erase "$(INTDIR)\S3P_MSSQLServer_Result.obj"
	-@erase "$(INTDIR)\S3P_MSSQLServer_Result.sbr"
	-@erase "$(INTDIR)\S3PDB_MSSQLServer_Connection.obj"
	-@erase "$(INTDIR)\S3PDB_MSSQLServer_Connection.sbr"
	-@erase "$(INTDIR)\S3PDBConVBC.obj"
	-@erase "$(INTDIR)\S3PDBConVBC.sbr"
	-@erase "$(INTDIR)\S3PResultVBC.obj"
	-@erase "$(INTDIR)\S3PResultVBC.sbr"
	-@erase "$(INTDIR)\ScriptFuns.obj"
	-@erase "$(INTDIR)\ScriptFuns.sbr"
	-@erase "$(INTDIR)\Skill.obj"
	-@erase "$(INTDIR)\Skill.sbr"
	-@erase "$(INTDIR)\stdafx.obj"
	-@erase "$(INTDIR)\stdafx.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\Core.bsc"
	-@erase "$(OUTDIR)\CoreServer.dll"
	-@erase "$(OUTDIR)\CoreServer.exp"
	-@erase "$(OUTDIR)\CoreServer.ilk"
	-@erase "$(OUTDIR)\CoreServer.lib"
	-@erase "$(OUTDIR)\CoreServer.map"
	-@erase "$(OUTDIR)\CoreServer.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MT /W2 /GX /O2 /I "..\engine\src" /I "..\engine\include" /I "..\Network" /I "..\..\Headers" /D "NDEBUG" /D "_SERVER" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CORE_EXPORTS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\Core.pch" /Yu"KCore.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Core.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\KPartnerSkill.sbr" \
	"$(INTDIR)\KPlayerPartner.sbr" \
	"$(INTDIR)\KThiefSkill.sbr" \
	"$(INTDIR)\KMissle.sbr" \
	"$(INTDIR)\KMissleRes.sbr" \
	"$(INTDIR)\KMissleSet.sbr" \
	"$(INTDIR)\KSkillList.sbr" \
	"$(INTDIR)\KSkillManager.sbr" \
	"$(INTDIR)\KSkills.sbr" \
	"$(INTDIR)\KSkillSpecial.sbr" \
	"$(INTDIR)\Skill.sbr" \
	"$(INTDIR)\KFaction.sbr" \
	"$(INTDIR)\KNpc.sbr" \
	"$(INTDIR)\KNpcAI.sbr" \
	"$(INTDIR)\KNpcAttribModify.sbr" \
	"$(INTDIR)\KNpcDeathCalcExp.sbr" \
	"$(INTDIR)\KNpcFindPath.sbr" \
	"$(INTDIR)\KNpcGold.sbr" \
	"$(INTDIR)\KNpcRes.sbr" \
	"$(INTDIR)\KNpcResList.sbr" \
	"$(INTDIR)\KNpcResNode.sbr" \
	"$(INTDIR)\KNpcSet.sbr" \
	"$(INTDIR)\KNpcTemplate.sbr" \
	"$(INTDIR)\KPlayer.sbr" \
	"$(INTDIR)\KPlayerChat.sbr" \
	"$(INTDIR)\KPlayerDBFuns.sbr" \
	"$(INTDIR)\KPlayerFaction.sbr" \
	"$(INTDIR)\KPlayerFuYuan.sbr" \
	"$(INTDIR)\KPlayerMenuState.sbr" \
	"$(INTDIR)\KPlayerPK.sbr" \
	"$(INTDIR)\KPlayerReBorn.sbr" \
	"$(INTDIR)\KPlayerRepute.sbr" \
	"$(INTDIR)\KPlayerSet.sbr" \
	"$(INTDIR)\KPlayerTask.sbr" \
	"$(INTDIR)\KPlayerTeam.sbr" \
	"$(INTDIR)\KPlayerTong.sbr" \
	"$(INTDIR)\KPlayerTrade.sbr" \
	"$(INTDIR)\KSprControl.sbr" \
	"$(INTDIR)\KBasPropTbl.sbr" \
	"$(INTDIR)\KBuySell.sbr" \
	"$(INTDIR)\KInventory.sbr" \
	"$(INTDIR)\KItem.sbr" \
	"$(INTDIR)\KItemChangeRes.sbr" \
	"$(INTDIR)\KItemGenerator.sbr" \
	"$(INTDIR)\KItemList.sbr" \
	"$(INTDIR)\KItemSet.sbr" \
	"$(INTDIR)\KObj.sbr" \
	"$(INTDIR)\KObjSet.sbr" \
	"$(INTDIR)\KSellItem.sbr" \
	"$(INTDIR)\KViewItem.sbr" \
	"$(INTDIR)\KRegion.sbr" \
	"$(INTDIR)\KCore.sbr" \
	"$(INTDIR)\KSubWorld.sbr" \
	"$(INTDIR)\KSubWorldSet.sbr" \
	"$(INTDIR)\KWeatherMgr.sbr" \
	"$(INTDIR)\S3P_MSSQLServer_Result.sbr" \
	"$(INTDIR)\S3PDB_MSSQLServer_Connection.sbr" \
	"$(INTDIR)\S3PDBConVBC.sbr" \
	"$(INTDIR)\S3PResultVBC.sbr" \
	"$(INTDIR)\KWorldMsg.sbr" \
	"$(INTDIR)\KMath.sbr" \
	"$(INTDIR)\KGMProcess.sbr" \
	"$(INTDIR)\KNewProtocolProcess.sbr" \
	"$(INTDIR)\KProtocol.sbr" \
	"$(INTDIR)\KProtocolProcess.sbr" \
	"$(INTDIR)\ScriptFuns.sbr" \
	"$(INTDIR)\KScriptValueSet.sbr" \
	"$(INTDIR)\KSortScript.sbr" \
	"$(INTDIR)\CoreServerShell.sbr" \
	"$(INTDIR)\Kjxpathfinder.sbr" \
	"$(INTDIR)\Kpathfinder.sbr" \
	"$(INTDIR)\KLittleMap.sbr" \
	"$(INTDIR)\KWeather.sbr" \
	"$(INTDIR)\KMagicDesc.sbr" \
	"$(INTDIR)\KGMCommand.sbr" \
	"$(INTDIR)\KMission.sbr" \
	"$(INTDIR)\KTaskFuns.sbr" \
	"$(INTDIR)\KOption.sbr" \
	"$(INTDIR)\KMapMusic.sbr" \
	"$(INTDIR)\KLadder.sbr" \
	"$(INTDIR)\KForBitGua.sbr" \
	"$(INTDIR)\KDropScript.sbr" \
	"$(INTDIR)\KLogFile.sbr" \
	"$(INTDIR)\DBTableS.sbr" \
	"$(INTDIR)\stdafx.sbr"

"$(OUTDIR)\Core.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=Kernel32.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\CoreServer.pdb" /map:"$(INTDIR)\CoreServer.map" /debug /machine:I386 /nodefaultlib:"libcmt.lib msvcrt.lib libc.lib LIBC.lib" /out:"$(OUTDIR)\CoreServer.dll" /implib:"$(OUTDIR)\CoreServer.lib" 
LINK32_OBJS= \
	"$(INTDIR)\KPartnerSkill.obj" \
	"$(INTDIR)\KPlayerPartner.obj" \
	"$(INTDIR)\KThiefSkill.obj" \
	"$(INTDIR)\KMissle.obj" \
	"$(INTDIR)\KMissleRes.obj" \
	"$(INTDIR)\KMissleSet.obj" \
	"$(INTDIR)\KSkillList.obj" \
	"$(INTDIR)\KSkillManager.obj" \
	"$(INTDIR)\KSkills.obj" \
	"$(INTDIR)\KSkillSpecial.obj" \
	"$(INTDIR)\Skill.obj" \
	"$(INTDIR)\KFaction.obj" \
	"$(INTDIR)\KNpc.obj" \
	"$(INTDIR)\KNpcAI.obj" \
	"$(INTDIR)\KNpcAttribModify.obj" \
	"$(INTDIR)\KNpcDeathCalcExp.obj" \
	"$(INTDIR)\KNpcFindPath.obj" \
	"$(INTDIR)\KNpcGold.obj" \
	"$(INTDIR)\KNpcRes.obj" \
	"$(INTDIR)\KNpcResList.obj" \
	"$(INTDIR)\KNpcResNode.obj" \
	"$(INTDIR)\KNpcSet.obj" \
	"$(INTDIR)\KNpcTemplate.obj" \
	"$(INTDIR)\KPlayer.obj" \
	"$(INTDIR)\KPlayerChat.obj" \
	"$(INTDIR)\KPlayerDBFuns.obj" \
	"$(INTDIR)\KPlayerFaction.obj" \
	"$(INTDIR)\KPlayerFuYuan.obj" \
	"$(INTDIR)\KPlayerMenuState.obj" \
	"$(INTDIR)\KPlayerPK.obj" \
	"$(INTDIR)\KPlayerReBorn.obj" \
	"$(INTDIR)\KPlayerRepute.obj" \
	"$(INTDIR)\KPlayerSet.obj" \
	"$(INTDIR)\KPlayerTask.obj" \
	"$(INTDIR)\KPlayerTeam.obj" \
	"$(INTDIR)\KPlayerTong.obj" \
	"$(INTDIR)\KPlayerTrade.obj" \
	"$(INTDIR)\KSprControl.obj" \
	"$(INTDIR)\KBasPropTbl.obj" \
	"$(INTDIR)\KBuySell.obj" \
	"$(INTDIR)\KInventory.obj" \
	"$(INTDIR)\KItem.obj" \
	"$(INTDIR)\KItemChangeRes.obj" \
	"$(INTDIR)\KItemGenerator.obj" \
	"$(INTDIR)\KItemList.obj" \
	"$(INTDIR)\KItemSet.obj" \
	"$(INTDIR)\KObj.obj" \
	"$(INTDIR)\KObjSet.obj" \
	"$(INTDIR)\KSellItem.obj" \
	"$(INTDIR)\KViewItem.obj" \
	"$(INTDIR)\KRegion.obj" \
	"$(INTDIR)\KCore.obj" \
	"$(INTDIR)\KSubWorld.obj" \
	"$(INTDIR)\KSubWorldSet.obj" \
	"$(INTDIR)\KWeatherMgr.obj" \
	"$(INTDIR)\S3P_MSSQLServer_Result.obj" \
	"$(INTDIR)\S3PDB_MSSQLServer_Connection.obj" \
	"$(INTDIR)\S3PDBConVBC.obj" \
	"$(INTDIR)\S3PResultVBC.obj" \
	"$(INTDIR)\KWorldMsg.obj" \
	"$(INTDIR)\KMath.obj" \
	"$(INTDIR)\KGMProcess.obj" \
	"$(INTDIR)\KNewProtocolProcess.obj" \
	"$(INTDIR)\KProtocol.obj" \
	"$(INTDIR)\KProtocolProcess.obj" \
	"$(INTDIR)\ScriptFuns.obj" \
	"$(INTDIR)\KScriptValueSet.obj" \
	"$(INTDIR)\KSortScript.obj" \
	"$(INTDIR)\CoreServerShell.obj" \
	"$(INTDIR)\Kjxpathfinder.obj" \
	"$(INTDIR)\Kpathfinder.obj" \
	"$(INTDIR)\KLittleMap.obj" \
	"$(INTDIR)\KWeather.obj" \
	"$(INTDIR)\KMagicDesc.obj" \
	"$(INTDIR)\KGMCommand.obj" \
	"$(INTDIR)\KMission.obj" \
	"$(INTDIR)\KTaskFuns.obj" \
	"$(INTDIR)\KOption.obj" \
	"$(INTDIR)\KMapMusic.obj" \
	"$(INTDIR)\KLadder.obj" \
	"$(INTDIR)\KForBitGua.obj" \
	"$(INTDIR)\KDropScript.obj" \
	"$(INTDIR)\KLogFile.obj" \
	"$(INTDIR)\DBTableS.obj" \
	"$(INTDIR)\stdafx.obj" \
	"..\..\Lib\release\engine.lib" \
	"..\..\Lib\release\common.lib" \
	"..\Library\LuaLib\Release\LuaLibDll.lib" \
	".\Src\USBKey\EP1KDL20.LIB" \
	".\Src\USBKey\EpsForJoLib.lib" \
	"..\..\Lib\libdb41s.lib"

"$(OUTDIR)\CoreServer.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# 开始自定义宏
OutDir=.\Server\Release
# 结束自定义宏

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\CoreServer.dll" "$(OUTDIR)\Core.bsc"
   md ..\..\..\bin\server\release
	md ..\..\lib\release
	copy Server\Release\CoreServer.dll ..\..\bin\server\Coreserver.dll
	copy Server\Release\CoreServer.dll ..\..\bin\server\release\Coreserver.dll
	copy Server\Release\CoreServer.lib ..\..\lib\release\CoreServer.lib
	copy Server\Release\CoreServer.lib ..\..\bin\server\CoreServer.lib
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"

OUTDIR=.\ClientDebug
INTDIR=.\ClientDebug
# 开始自定义宏
OutDir=.\ClientDebug
# 结束自定义宏

ALL : "$(OUTDIR)\CoreClient.dll" "$(OUTDIR)\Core.bsc"


CLEAN :
	-@erase "$(INTDIR)\Core.pch"
	-@erase "$(INTDIR)\CoreDrawGameObj.obj"
	-@erase "$(INTDIR)\CoreDrawGameObj.sbr"
	-@erase "$(INTDIR)\CoreShell.obj"
	-@erase "$(INTDIR)\CoreShell.sbr"
	-@erase "$(INTDIR)\DBTableS.obj"
	-@erase "$(INTDIR)\DBTableS.sbr"
	-@erase "$(INTDIR)\ImgRef.obj"
	-@erase "$(INTDIR)\ImgRef.sbr"
	-@erase "$(INTDIR)\KBasPropTbl.obj"
	-@erase "$(INTDIR)\KBasPropTbl.sbr"
	-@erase "$(INTDIR)\KBuySell.obj"
	-@erase "$(INTDIR)\KBuySell.sbr"
	-@erase "$(INTDIR)\KCore.obj"
	-@erase "$(INTDIR)\KCore.sbr"
	-@erase "$(INTDIR)\KDropScript.obj"
	-@erase "$(INTDIR)\KDropScript.sbr"
	-@erase "$(INTDIR)\KFaction.obj"
	-@erase "$(INTDIR)\KFaction.sbr"
	-@erase "$(INTDIR)\KForBitGua.obj"
	-@erase "$(INTDIR)\KForBitGua.sbr"
	-@erase "$(INTDIR)\KGMCommand.obj"
	-@erase "$(INTDIR)\KGMCommand.sbr"
	-@erase "$(INTDIR)\KInventory.obj"
	-@erase "$(INTDIR)\KInventory.sbr"
	-@erase "$(INTDIR)\KIpotBranch.obj"
	-@erase "$(INTDIR)\KIpotBranch.sbr"
	-@erase "$(INTDIR)\KIpotLeaf.obj"
	-@erase "$(INTDIR)\KIpotLeaf.sbr"
	-@erase "$(INTDIR)\KIpoTree.obj"
	-@erase "$(INTDIR)\KIpoTree.sbr"
	-@erase "$(INTDIR)\KItem.obj"
	-@erase "$(INTDIR)\KItem.sbr"
	-@erase "$(INTDIR)\KItemChangeRes.obj"
	-@erase "$(INTDIR)\KItemChangeRes.sbr"
	-@erase "$(INTDIR)\KItemGenerator.obj"
	-@erase "$(INTDIR)\KItemGenerator.sbr"
	-@erase "$(INTDIR)\KItemList.obj"
	-@erase "$(INTDIR)\KItemList.sbr"
	-@erase "$(INTDIR)\KItemSet.obj"
	-@erase "$(INTDIR)\KItemSet.sbr"
	-@erase "$(INTDIR)\Kjxpathfinder.obj"
	-@erase "$(INTDIR)\Kjxpathfinder.sbr"
	-@erase "$(INTDIR)\KLadder.obj"
	-@erase "$(INTDIR)\KLadder.sbr"
	-@erase "$(INTDIR)\KLittleMap.obj"
	-@erase "$(INTDIR)\KLittleMap.sbr"
	-@erase "$(INTDIR)\KLogFile.obj"
	-@erase "$(INTDIR)\KLogFile.sbr"
	-@erase "$(INTDIR)\KMagicDesc.obj"
	-@erase "$(INTDIR)\KMagicDesc.sbr"
	-@erase "$(INTDIR)\KMapMusic.obj"
	-@erase "$(INTDIR)\KMapMusic.sbr"
	-@erase "$(INTDIR)\KMath.obj"
	-@erase "$(INTDIR)\KMath.sbr"
	-@erase "$(INTDIR)\KMissle.obj"
	-@erase "$(INTDIR)\KMissle.sbr"
	-@erase "$(INTDIR)\KMissleRes.obj"
	-@erase "$(INTDIR)\KMissleRes.sbr"
	-@erase "$(INTDIR)\KMissleSet.obj"
	-@erase "$(INTDIR)\KMissleSet.sbr"
	-@erase "$(INTDIR)\KNpc.obj"
	-@erase "$(INTDIR)\KNpc.sbr"
	-@erase "$(INTDIR)\KNpcAI.obj"
	-@erase "$(INTDIR)\KNpcAI.sbr"
	-@erase "$(INTDIR)\KNpcAttribModify.obj"
	-@erase "$(INTDIR)\KNpcAttribModify.sbr"
	-@erase "$(INTDIR)\KNpcDeathCalcExp.obj"
	-@erase "$(INTDIR)\KNpcDeathCalcExp.sbr"
	-@erase "$(INTDIR)\KNpcFindPath.obj"
	-@erase "$(INTDIR)\KNpcFindPath.sbr"
	-@erase "$(INTDIR)\KNpcGold.obj"
	-@erase "$(INTDIR)\KNpcGold.sbr"
	-@erase "$(INTDIR)\KNpcRes.obj"
	-@erase "$(INTDIR)\KNpcRes.sbr"
	-@erase "$(INTDIR)\KNpcResList.obj"
	-@erase "$(INTDIR)\KNpcResList.sbr"
	-@erase "$(INTDIR)\KNpcResNode.obj"
	-@erase "$(INTDIR)\KNpcResNode.sbr"
	-@erase "$(INTDIR)\KNpcSet.obj"
	-@erase "$(INTDIR)\KNpcSet.sbr"
	-@erase "$(INTDIR)\KNpcTemplate.obj"
	-@erase "$(INTDIR)\KNpcTemplate.sbr"
	-@erase "$(INTDIR)\KObj.obj"
	-@erase "$(INTDIR)\KObj.sbr"
	-@erase "$(INTDIR)\KObjSet.obj"
	-@erase "$(INTDIR)\KObjSet.sbr"
	-@erase "$(INTDIR)\KOption.obj"
	-@erase "$(INTDIR)\KOption.sbr"
	-@erase "$(INTDIR)\KPartnerSkill.obj"
	-@erase "$(INTDIR)\KPartnerSkill.sbr"
	-@erase "$(INTDIR)\Kpathfinder.obj"
	-@erase "$(INTDIR)\Kpathfinder.sbr"
	-@erase "$(INTDIR)\KPlayer.obj"
	-@erase "$(INTDIR)\KPlayer.sbr"
	-@erase "$(INTDIR)\KPlayerChat.obj"
	-@erase "$(INTDIR)\KPlayerChat.sbr"
	-@erase "$(INTDIR)\KPlayerDBFuns.obj"
	-@erase "$(INTDIR)\KPlayerDBFuns.sbr"
	-@erase "$(INTDIR)\KPlayerFaction.obj"
	-@erase "$(INTDIR)\KPlayerFaction.sbr"
	-@erase "$(INTDIR)\KPlayerFuYuan.obj"
	-@erase "$(INTDIR)\KPlayerFuYuan.sbr"
	-@erase "$(INTDIR)\KPlayerMenuState.obj"
	-@erase "$(INTDIR)\KPlayerMenuState.sbr"
	-@erase "$(INTDIR)\KPlayerPartner.obj"
	-@erase "$(INTDIR)\KPlayerPartner.sbr"
	-@erase "$(INTDIR)\KPlayerPK.obj"
	-@erase "$(INTDIR)\KPlayerPK.sbr"
	-@erase "$(INTDIR)\KPlayerReBorn.obj"
	-@erase "$(INTDIR)\KPlayerReBorn.sbr"
	-@erase "$(INTDIR)\KPlayerRepute.obj"
	-@erase "$(INTDIR)\KPlayerRepute.sbr"
	-@erase "$(INTDIR)\KPlayerSet.obj"
	-@erase "$(INTDIR)\KPlayerSet.sbr"
	-@erase "$(INTDIR)\KPlayerTask.obj"
	-@erase "$(INTDIR)\KPlayerTask.sbr"
	-@erase "$(INTDIR)\KPlayerTeam.obj"
	-@erase "$(INTDIR)\KPlayerTeam.sbr"
	-@erase "$(INTDIR)\KPlayerTong.obj"
	-@erase "$(INTDIR)\KPlayerTong.sbr"
	-@erase "$(INTDIR)\KPlayerTrade.obj"
	-@erase "$(INTDIR)\KPlayerTrade.sbr"
	-@erase "$(INTDIR)\KProtocol.obj"
	-@erase "$(INTDIR)\KProtocol.sbr"
	-@erase "$(INTDIR)\KProtocolProcess.obj"
	-@erase "$(INTDIR)\KProtocolProcess.sbr"
	-@erase "$(INTDIR)\KRegion.obj"
	-@erase "$(INTDIR)\KRegion.sbr"
	-@erase "$(INTDIR)\KScenePlaceC.obj"
	-@erase "$(INTDIR)\KScenePlaceC.sbr"
	-@erase "$(INTDIR)\KScenePlaceRegionC.obj"
	-@erase "$(INTDIR)\KScenePlaceRegionC.sbr"
	-@erase "$(INTDIR)\KScriptValueSet.obj"
	-@erase "$(INTDIR)\KScriptValueSet.sbr"
	-@erase "$(INTDIR)\KSellItem.obj"
	-@erase "$(INTDIR)\KSellItem.sbr"
	-@erase "$(INTDIR)\KSkillList.obj"
	-@erase "$(INTDIR)\KSkillList.sbr"
	-@erase "$(INTDIR)\KSkillManager.obj"
	-@erase "$(INTDIR)\KSkillManager.sbr"
	-@erase "$(INTDIR)\KSkills.obj"
	-@erase "$(INTDIR)\KSkills.sbr"
	-@erase "$(INTDIR)\KSkillSpecial.obj"
	-@erase "$(INTDIR)\KSkillSpecial.sbr"
	-@erase "$(INTDIR)\KSortScript.obj"
	-@erase "$(INTDIR)\KSortScript.sbr"
	-@erase "$(INTDIR)\KSprControl.obj"
	-@erase "$(INTDIR)\KSprControl.sbr"
	-@erase "$(INTDIR)\KSubWorld.obj"
	-@erase "$(INTDIR)\KSubWorld.sbr"
	-@erase "$(INTDIR)\KSubWorldSet.obj"
	-@erase "$(INTDIR)\KSubWorldSet.sbr"
	-@erase "$(INTDIR)\KTaskFuns.obj"
	-@erase "$(INTDIR)\KTaskFuns.sbr"
	-@erase "$(INTDIR)\KThiefSkill.obj"
	-@erase "$(INTDIR)\KThiefSkill.sbr"
	-@erase "$(INTDIR)\KViewItem.obj"
	-@erase "$(INTDIR)\KViewItem.sbr"
	-@erase "$(INTDIR)\KWeather.obj"
	-@erase "$(INTDIR)\KWeather.sbr"
	-@erase "$(INTDIR)\KWeatherMgr.obj"
	-@erase "$(INTDIR)\KWeatherMgr.sbr"
	-@erase "$(INTDIR)\KWorldMsg.obj"
	-@erase "$(INTDIR)\KWorldMsg.sbr"
	-@erase "$(INTDIR)\S3P_MSSQLServer_Result.obj"
	-@erase "$(INTDIR)\S3P_MSSQLServer_Result.sbr"
	-@erase "$(INTDIR)\S3PDB_MSSQLServer_Connection.obj"
	-@erase "$(INTDIR)\S3PDB_MSSQLServer_Connection.sbr"
	-@erase "$(INTDIR)\S3PDBConVBC.obj"
	-@erase "$(INTDIR)\S3PDBConVBC.sbr"
	-@erase "$(INTDIR)\S3PResultVBC.obj"
	-@erase "$(INTDIR)\S3PResultVBC.sbr"
	-@erase "$(INTDIR)\SceneMath.obj"
	-@erase "$(INTDIR)\SceneMath.sbr"
	-@erase "$(INTDIR)\ScenePlaceMapC.obj"
	-@erase "$(INTDIR)\ScenePlaceMapC.sbr"
	-@erase "$(INTDIR)\ScriptFuns.obj"
	-@erase "$(INTDIR)\ScriptFuns.sbr"
	-@erase "$(INTDIR)\stdafx.obj"
	-@erase "$(INTDIR)\stdafx.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\Core.bsc"
	-@erase "$(OUTDIR)\CoreClient.dll"
	-@erase "$(OUTDIR)\CoreClient.exp"
	-@erase "$(OUTDIR)\CoreClient.lib"
	-@erase "$(OUTDIR)\CoreClient.map"
	-@erase "$(OUTDIR)\CoreClient.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\engine\src" /I "..\..\Headers" /I "..\engine\include" /I "..\Network" /I "src" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CORE_EXPORTS" /D "SWORDONLINE_SHOW_DBUG_INFO" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\Core.pch" /Yu"KCore.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Core.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\KPartnerSkill.sbr" \
	"$(INTDIR)\KPlayerPartner.sbr" \
	"$(INTDIR)\KThiefSkill.sbr" \
	"$(INTDIR)\KMissle.sbr" \
	"$(INTDIR)\KMissleRes.sbr" \
	"$(INTDIR)\KMissleSet.sbr" \
	"$(INTDIR)\KSkillList.sbr" \
	"$(INTDIR)\KSkillManager.sbr" \
	"$(INTDIR)\KSkills.sbr" \
	"$(INTDIR)\KSkillSpecial.sbr" \
	"$(INTDIR)\KFaction.sbr" \
	"$(INTDIR)\KNpc.sbr" \
	"$(INTDIR)\KNpcAI.sbr" \
	"$(INTDIR)\KNpcAttribModify.sbr" \
	"$(INTDIR)\KNpcDeathCalcExp.sbr" \
	"$(INTDIR)\KNpcFindPath.sbr" \
	"$(INTDIR)\KNpcGold.sbr" \
	"$(INTDIR)\KNpcRes.sbr" \
	"$(INTDIR)\KNpcResList.sbr" \
	"$(INTDIR)\KNpcResNode.sbr" \
	"$(INTDIR)\KNpcSet.sbr" \
	"$(INTDIR)\KNpcTemplate.sbr" \
	"$(INTDIR)\KPlayer.sbr" \
	"$(INTDIR)\KPlayerChat.sbr" \
	"$(INTDIR)\KPlayerDBFuns.sbr" \
	"$(INTDIR)\KPlayerFaction.sbr" \
	"$(INTDIR)\KPlayerFuYuan.sbr" \
	"$(INTDIR)\KPlayerMenuState.sbr" \
	"$(INTDIR)\KPlayerPK.sbr" \
	"$(INTDIR)\KPlayerReBorn.sbr" \
	"$(INTDIR)\KPlayerRepute.sbr" \
	"$(INTDIR)\KPlayerSet.sbr" \
	"$(INTDIR)\KPlayerTask.sbr" \
	"$(INTDIR)\KPlayerTeam.sbr" \
	"$(INTDIR)\KPlayerTong.sbr" \
	"$(INTDIR)\KPlayerTrade.sbr" \
	"$(INTDIR)\KSprControl.sbr" \
	"$(INTDIR)\KBasPropTbl.sbr" \
	"$(INTDIR)\KBuySell.sbr" \
	"$(INTDIR)\KInventory.sbr" \
	"$(INTDIR)\KItem.sbr" \
	"$(INTDIR)\KItemChangeRes.sbr" \
	"$(INTDIR)\KItemGenerator.sbr" \
	"$(INTDIR)\KItemList.sbr" \
	"$(INTDIR)\KItemSet.sbr" \
	"$(INTDIR)\KObj.sbr" \
	"$(INTDIR)\KObjSet.sbr" \
	"$(INTDIR)\KSellItem.sbr" \
	"$(INTDIR)\KViewItem.sbr" \
	"$(INTDIR)\KRegion.sbr" \
	"$(INTDIR)\KCore.sbr" \
	"$(INTDIR)\KSubWorld.sbr" \
	"$(INTDIR)\KSubWorldSet.sbr" \
	"$(INTDIR)\KWeatherMgr.sbr" \
	"$(INTDIR)\S3P_MSSQLServer_Result.sbr" \
	"$(INTDIR)\S3PDB_MSSQLServer_Connection.sbr" \
	"$(INTDIR)\S3PDBConVBC.sbr" \
	"$(INTDIR)\S3PResultVBC.sbr" \
	"$(INTDIR)\KWorldMsg.sbr" \
	"$(INTDIR)\KMath.sbr" \
	"$(INTDIR)\KProtocol.sbr" \
	"$(INTDIR)\KProtocolProcess.sbr" \
	"$(INTDIR)\ScriptFuns.sbr" \
	"$(INTDIR)\KScriptValueSet.sbr" \
	"$(INTDIR)\KSortScript.sbr" \
	"$(INTDIR)\CoreShell.sbr" \
	"$(INTDIR)\KIpotBranch.sbr" \
	"$(INTDIR)\KIpotLeaf.sbr" \
	"$(INTDIR)\KIpoTree.sbr" \
	"$(INTDIR)\Kjxpathfinder.sbr" \
	"$(INTDIR)\Kpathfinder.sbr" \
	"$(INTDIR)\KLittleMap.sbr" \
	"$(INTDIR)\KScenePlaceC.sbr" \
	"$(INTDIR)\KScenePlaceRegionC.sbr" \
	"$(INTDIR)\KWeather.sbr" \
	"$(INTDIR)\SceneMath.sbr" \
	"$(INTDIR)\ScenePlaceMapC.sbr" \
	"$(INTDIR)\CoreDrawGameObj.sbr" \
	"$(INTDIR)\ImgRef.sbr" \
	"$(INTDIR)\KMagicDesc.sbr" \
	"$(INTDIR)\KGMCommand.sbr" \
	"$(INTDIR)\KTaskFuns.sbr" \
	"$(INTDIR)\KOption.sbr" \
	"$(INTDIR)\KMapMusic.sbr" \
	"$(INTDIR)\KLadder.sbr" \
	"$(INTDIR)\KForBitGua.sbr" \
	"$(INTDIR)\KDropScript.sbr" \
	"$(INTDIR)\KLogFile.sbr" \
	"$(INTDIR)\DBTableS.sbr" \
	"$(INTDIR)\stdafx.sbr"

"$(OUTDIR)\Core.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib lualibdll.lib Shlwapi.lib Winmm.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\CoreClient.pdb" /map:"$(INTDIR)\CoreClient.map" /debug /machine:I386 /out:"$(OUTDIR)\CoreClient.dll" /implib:"$(OUTDIR)\CoreClient.lib" /pdbtype:sept /libpath:"../../lib/" 
LINK32_OBJS= \
	"$(INTDIR)\KPartnerSkill.obj" \
	"$(INTDIR)\KPlayerPartner.obj" \
	"$(INTDIR)\KThiefSkill.obj" \
	"$(INTDIR)\KMissle.obj" \
	"$(INTDIR)\KMissleRes.obj" \
	"$(INTDIR)\KMissleSet.obj" \
	"$(INTDIR)\KSkillList.obj" \
	"$(INTDIR)\KSkillManager.obj" \
	"$(INTDIR)\KSkills.obj" \
	"$(INTDIR)\KSkillSpecial.obj" \
	"$(INTDIR)\KFaction.obj" \
	"$(INTDIR)\KNpc.obj" \
	"$(INTDIR)\KNpcAI.obj" \
	"$(INTDIR)\KNpcAttribModify.obj" \
	"$(INTDIR)\KNpcDeathCalcExp.obj" \
	"$(INTDIR)\KNpcFindPath.obj" \
	"$(INTDIR)\KNpcGold.obj" \
	"$(INTDIR)\KNpcRes.obj" \
	"$(INTDIR)\KNpcResList.obj" \
	"$(INTDIR)\KNpcResNode.obj" \
	"$(INTDIR)\KNpcSet.obj" \
	"$(INTDIR)\KNpcTemplate.obj" \
	"$(INTDIR)\KPlayer.obj" \
	"$(INTDIR)\KPlayerChat.obj" \
	"$(INTDIR)\KPlayerDBFuns.obj" \
	"$(INTDIR)\KPlayerFaction.obj" \
	"$(INTDIR)\KPlayerFuYuan.obj" \
	"$(INTDIR)\KPlayerMenuState.obj" \
	"$(INTDIR)\KPlayerPK.obj" \
	"$(INTDIR)\KPlayerReBorn.obj" \
	"$(INTDIR)\KPlayerRepute.obj" \
	"$(INTDIR)\KPlayerSet.obj" \
	"$(INTDIR)\KPlayerTask.obj" \
	"$(INTDIR)\KPlayerTeam.obj" \
	"$(INTDIR)\KPlayerTong.obj" \
	"$(INTDIR)\KPlayerTrade.obj" \
	"$(INTDIR)\KSprControl.obj" \
	"$(INTDIR)\KBasPropTbl.obj" \
	"$(INTDIR)\KBuySell.obj" \
	"$(INTDIR)\KInventory.obj" \
	"$(INTDIR)\KItem.obj" \
	"$(INTDIR)\KItemChangeRes.obj" \
	"$(INTDIR)\KItemGenerator.obj" \
	"$(INTDIR)\KItemList.obj" \
	"$(INTDIR)\KItemSet.obj" \
	"$(INTDIR)\KObj.obj" \
	"$(INTDIR)\KObjSet.obj" \
	"$(INTDIR)\KSellItem.obj" \
	"$(INTDIR)\KViewItem.obj" \
	"$(INTDIR)\KRegion.obj" \
	"$(INTDIR)\KCore.obj" \
	"$(INTDIR)\KSubWorld.obj" \
	"$(INTDIR)\KSubWorldSet.obj" \
	"$(INTDIR)\KWeatherMgr.obj" \
	"$(INTDIR)\S3P_MSSQLServer_Result.obj" \
	"$(INTDIR)\S3PDB_MSSQLServer_Connection.obj" \
	"$(INTDIR)\S3PDBConVBC.obj" \
	"$(INTDIR)\S3PResultVBC.obj" \
	"$(INTDIR)\KWorldMsg.obj" \
	"$(INTDIR)\KMath.obj" \
	"$(INTDIR)\KProtocol.obj" \
	"$(INTDIR)\KProtocolProcess.obj" \
	"$(INTDIR)\ScriptFuns.obj" \
	"$(INTDIR)\KScriptValueSet.obj" \
	"$(INTDIR)\KSortScript.obj" \
	"$(INTDIR)\CoreShell.obj" \
	"$(INTDIR)\KIpotBranch.obj" \
	"$(INTDIR)\KIpotLeaf.obj" \
	"$(INTDIR)\KIpoTree.obj" \
	"$(INTDIR)\Kjxpathfinder.obj" \
	"$(INTDIR)\Kpathfinder.obj" \
	"$(INTDIR)\KLittleMap.obj" \
	"$(INTDIR)\KScenePlaceC.obj" \
	"$(INTDIR)\KScenePlaceRegionC.obj" \
	"$(INTDIR)\KWeather.obj" \
	"$(INTDIR)\SceneMath.obj" \
	"$(INTDIR)\ScenePlaceMapC.obj" \
	"$(INTDIR)\CoreDrawGameObj.obj" \
	"$(INTDIR)\ImgRef.obj" \
	"$(INTDIR)\KMagicDesc.obj" \
	"$(INTDIR)\KGMCommand.obj" \
	"$(INTDIR)\KTaskFuns.obj" \
	"$(INTDIR)\KOption.obj" \
	"$(INTDIR)\KMapMusic.obj" \
	"$(INTDIR)\KLadder.obj" \
	"$(INTDIR)\KForBitGua.obj" \
	"$(INTDIR)\KDropScript.obj" \
	"$(INTDIR)\KLogFile.obj" \
	"$(INTDIR)\DBTableS.obj" \
	"$(INTDIR)\stdafx.obj" \
	"..\Library\LuaLib\Release\LuaLibDll.lib" \
	"..\..\Lib\debug\engine.lib" \
	"..\..\Lib\debug\common.lib" \
	".\Src\USBKey\EP1KDL20.LIB" \
	".\Src\USBKey\EpsForJoLib.lib" \
	"..\..\Lib\libdb41s.lib"

"$(OUTDIR)\CoreClient.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# 开始自定义宏
OutDir=.\ClientDebug
# 结束自定义宏

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\CoreClient.dll" "$(OUTDIR)\Core.bsc"
   md ..\..\..\bin\client\debug
	md ..\..\lib\debug
	copy ClientDebug\CoreClient.pdb ..\..\..\bin\client\CoreClient.pdb
	copy ClientDebug\CoreClient.map ..\..\..\bin\client\CoreClient.map
	copy ClientDebug\CoreClient.dll ..\..\..\bin\client\CoreClient.dll
	copy ClientDebug\CoreClient.pdb ..\..\..\bin\client\debug\CoreClient.pdb
	copy ClientDebug\CoreClient.map ..\..\..\bin\client\debug\CoreClient.map
	copy ClientDebug\CoreClient.dll ..\..\..\bin\client\debug\CoreClient.dll
	copy ClientDebug\CoreClient.lib ..\..\lib\debug\CoreClient.lib
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"

OUTDIR=.\Client\Release
INTDIR=.\Client\Release
# 开始自定义宏
OutDir=.\Client\Release
# 结束自定义宏

ALL : "$(OUTDIR)\CoreClient.dll" "$(OUTDIR)\Core.pch"


CLEAN :
	-@erase "$(INTDIR)\Core.pch"
	-@erase "$(INTDIR)\CoreDrawGameObj.obj"
	-@erase "$(INTDIR)\CoreShell.obj"
	-@erase "$(INTDIR)\DBTableS.obj"
	-@erase "$(INTDIR)\ImgRef.obj"
	-@erase "$(INTDIR)\KBasPropTbl.obj"
	-@erase "$(INTDIR)\KBuySell.obj"
	-@erase "$(INTDIR)\KCore.obj"
	-@erase "$(INTDIR)\KDropScript.obj"
	-@erase "$(INTDIR)\KFaction.obj"
	-@erase "$(INTDIR)\KForBitGua.obj"
	-@erase "$(INTDIR)\KGMCommand.obj"
	-@erase "$(INTDIR)\KInventory.obj"
	-@erase "$(INTDIR)\KIpotBranch.obj"
	-@erase "$(INTDIR)\KIpotLeaf.obj"
	-@erase "$(INTDIR)\KIpoTree.obj"
	-@erase "$(INTDIR)\KItem.obj"
	-@erase "$(INTDIR)\KItemChangeRes.obj"
	-@erase "$(INTDIR)\KItemGenerator.obj"
	-@erase "$(INTDIR)\KItemList.obj"
	-@erase "$(INTDIR)\KItemSet.obj"
	-@erase "$(INTDIR)\Kjxpathfinder.obj"
	-@erase "$(INTDIR)\KLadder.obj"
	-@erase "$(INTDIR)\KLittleMap.obj"
	-@erase "$(INTDIR)\KLogFile.obj"
	-@erase "$(INTDIR)\KMagicDesc.obj"
	-@erase "$(INTDIR)\KMapMusic.obj"
	-@erase "$(INTDIR)\KMath.obj"
	-@erase "$(INTDIR)\KMissle.obj"
	-@erase "$(INTDIR)\KMissleRes.obj"
	-@erase "$(INTDIR)\KMissleSet.obj"
	-@erase "$(INTDIR)\KNpc.obj"
	-@erase "$(INTDIR)\KNpcAI.obj"
	-@erase "$(INTDIR)\KNpcAttribModify.obj"
	-@erase "$(INTDIR)\KNpcDeathCalcExp.obj"
	-@erase "$(INTDIR)\KNpcFindPath.obj"
	-@erase "$(INTDIR)\KNpcGold.obj"
	-@erase "$(INTDIR)\KNpcRes.obj"
	-@erase "$(INTDIR)\KNpcResList.obj"
	-@erase "$(INTDIR)\KNpcResNode.obj"
	-@erase "$(INTDIR)\KNpcSet.obj"
	-@erase "$(INTDIR)\KNpcTemplate.obj"
	-@erase "$(INTDIR)\KObj.obj"
	-@erase "$(INTDIR)\KObjSet.obj"
	-@erase "$(INTDIR)\KOption.obj"
	-@erase "$(INTDIR)\KPartnerSkill.obj"
	-@erase "$(INTDIR)\Kpathfinder.obj"
	-@erase "$(INTDIR)\KPlayer.obj"
	-@erase "$(INTDIR)\KPlayerChat.obj"
	-@erase "$(INTDIR)\KPlayerDBFuns.obj"
	-@erase "$(INTDIR)\KPlayerFaction.obj"
	-@erase "$(INTDIR)\KPlayerFuYuan.obj"
	-@erase "$(INTDIR)\KPlayerMenuState.obj"
	-@erase "$(INTDIR)\KPlayerPartner.obj"
	-@erase "$(INTDIR)\KPlayerPK.obj"
	-@erase "$(INTDIR)\KPlayerReBorn.obj"
	-@erase "$(INTDIR)\KPlayerRepute.obj"
	-@erase "$(INTDIR)\KPlayerSet.obj"
	-@erase "$(INTDIR)\KPlayerTask.obj"
	-@erase "$(INTDIR)\KPlayerTeam.obj"
	-@erase "$(INTDIR)\KPlayerTong.obj"
	-@erase "$(INTDIR)\KPlayerTrade.obj"
	-@erase "$(INTDIR)\KProtocol.obj"
	-@erase "$(INTDIR)\KProtocolProcess.obj"
	-@erase "$(INTDIR)\KRegion.obj"
	-@erase "$(INTDIR)\KScenePlaceC.obj"
	-@erase "$(INTDIR)\KScenePlaceRegionC.obj"
	-@erase "$(INTDIR)\KScriptValueSet.obj"
	-@erase "$(INTDIR)\KSellItem.obj"
	-@erase "$(INTDIR)\KSkillList.obj"
	-@erase "$(INTDIR)\KSkillManager.obj"
	-@erase "$(INTDIR)\KSkills.obj"
	-@erase "$(INTDIR)\KSkillSpecial.obj"
	-@erase "$(INTDIR)\KSortScript.obj"
	-@erase "$(INTDIR)\KSprControl.obj"
	-@erase "$(INTDIR)\KSubWorld.obj"
	-@erase "$(INTDIR)\KSubWorldSet.obj"
	-@erase "$(INTDIR)\KTaskFuns.obj"
	-@erase "$(INTDIR)\KThiefSkill.obj"
	-@erase "$(INTDIR)\KViewItem.obj"
	-@erase "$(INTDIR)\KWeather.obj"
	-@erase "$(INTDIR)\KWeatherMgr.obj"
	-@erase "$(INTDIR)\KWorldMsg.obj"
	-@erase "$(INTDIR)\S3P_MSSQLServer_Result.obj"
	-@erase "$(INTDIR)\S3PDB_MSSQLServer_Connection.obj"
	-@erase "$(INTDIR)\S3PDBConVBC.obj"
	-@erase "$(INTDIR)\S3PResultVBC.obj"
	-@erase "$(INTDIR)\SceneMath.obj"
	-@erase "$(INTDIR)\ScenePlaceMapC.obj"
	-@erase "$(INTDIR)\ScriptFuns.obj"
	-@erase "$(INTDIR)\Skill.obj"
	-@erase "$(INTDIR)\stdafx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\CoreClient.dll"
	-@erase "$(OUTDIR)\CoreClient.exp"
	-@erase "$(OUTDIR)\CoreClient.ilk"
	-@erase "$(OUTDIR)\CoreClient.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MT /W2 /GX /Zi /O2 /I "..\engine\src" /I "..\engine\include" /I "..\Network" /I "..\..\Headers" /I "src" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CORE_EXPORTS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Core.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=advapi32.lib Winmm.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\CoreClient.pdb" /machine:I386 /out:"$(OUTDIR)\CoreClient.dll" /implib:"$(OUTDIR)\CoreClient.lib" /libpath:"../../lib/" 
LINK32_OBJS= \
	"$(INTDIR)\KPartnerSkill.obj" \
	"$(INTDIR)\KPlayerPartner.obj" \
	"$(INTDIR)\KThiefSkill.obj" \
	"$(INTDIR)\KMissle.obj" \
	"$(INTDIR)\KMissleRes.obj" \
	"$(INTDIR)\KMissleSet.obj" \
	"$(INTDIR)\KSkillList.obj" \
	"$(INTDIR)\KSkillManager.obj" \
	"$(INTDIR)\KSkills.obj" \
	"$(INTDIR)\KSkillSpecial.obj" \
	"$(INTDIR)\Skill.obj" \
	"$(INTDIR)\KFaction.obj" \
	"$(INTDIR)\KNpc.obj" \
	"$(INTDIR)\KNpcAI.obj" \
	"$(INTDIR)\KNpcAttribModify.obj" \
	"$(INTDIR)\KNpcDeathCalcExp.obj" \
	"$(INTDIR)\KNpcFindPath.obj" \
	"$(INTDIR)\KNpcGold.obj" \
	"$(INTDIR)\KNpcRes.obj" \
	"$(INTDIR)\KNpcResList.obj" \
	"$(INTDIR)\KNpcResNode.obj" \
	"$(INTDIR)\KNpcSet.obj" \
	"$(INTDIR)\KNpcTemplate.obj" \
	"$(INTDIR)\KPlayer.obj" \
	"$(INTDIR)\KPlayerChat.obj" \
	"$(INTDIR)\KPlayerDBFuns.obj" \
	"$(INTDIR)\KPlayerFaction.obj" \
	"$(INTDIR)\KPlayerFuYuan.obj" \
	"$(INTDIR)\KPlayerMenuState.obj" \
	"$(INTDIR)\KPlayerPK.obj" \
	"$(INTDIR)\KPlayerReBorn.obj" \
	"$(INTDIR)\KPlayerRepute.obj" \
	"$(INTDIR)\KPlayerSet.obj" \
	"$(INTDIR)\KPlayerTask.obj" \
	"$(INTDIR)\KPlayerTeam.obj" \
	"$(INTDIR)\KPlayerTong.obj" \
	"$(INTDIR)\KPlayerTrade.obj" \
	"$(INTDIR)\KSprControl.obj" \
	"$(INTDIR)\KBasPropTbl.obj" \
	"$(INTDIR)\KBuySell.obj" \
	"$(INTDIR)\KInventory.obj" \
	"$(INTDIR)\KItem.obj" \
	"$(INTDIR)\KItemChangeRes.obj" \
	"$(INTDIR)\KItemGenerator.obj" \
	"$(INTDIR)\KItemList.obj" \
	"$(INTDIR)\KItemSet.obj" \
	"$(INTDIR)\KObj.obj" \
	"$(INTDIR)\KObjSet.obj" \
	"$(INTDIR)\KSellItem.obj" \
	"$(INTDIR)\KViewItem.obj" \
	"$(INTDIR)\KRegion.obj" \
	"$(INTDIR)\KCore.obj" \
	"$(INTDIR)\KSubWorld.obj" \
	"$(INTDIR)\KSubWorldSet.obj" \
	"$(INTDIR)\KWeatherMgr.obj" \
	"$(INTDIR)\S3P_MSSQLServer_Result.obj" \
	"$(INTDIR)\S3PDB_MSSQLServer_Connection.obj" \
	"$(INTDIR)\S3PDBConVBC.obj" \
	"$(INTDIR)\S3PResultVBC.obj" \
	"$(INTDIR)\KWorldMsg.obj" \
	"$(INTDIR)\KMath.obj" \
	"$(INTDIR)\KProtocol.obj" \
	"$(INTDIR)\KProtocolProcess.obj" \
	"$(INTDIR)\ScriptFuns.obj" \
	"$(INTDIR)\KScriptValueSet.obj" \
	"$(INTDIR)\KSortScript.obj" \
	"$(INTDIR)\CoreShell.obj" \
	"$(INTDIR)\KIpotBranch.obj" \
	"$(INTDIR)\KIpotLeaf.obj" \
	"$(INTDIR)\KIpoTree.obj" \
	"$(INTDIR)\Kjxpathfinder.obj" \
	"$(INTDIR)\Kpathfinder.obj" \
	"$(INTDIR)\KLittleMap.obj" \
	"$(INTDIR)\KScenePlaceC.obj" \
	"$(INTDIR)\KScenePlaceRegionC.obj" \
	"$(INTDIR)\KWeather.obj" \
	"$(INTDIR)\SceneMath.obj" \
	"$(INTDIR)\ScenePlaceMapC.obj" \
	"$(INTDIR)\CoreDrawGameObj.obj" \
	"$(INTDIR)\ImgRef.obj" \
	"$(INTDIR)\KMagicDesc.obj" \
	"$(INTDIR)\KGMCommand.obj" \
	"$(INTDIR)\KTaskFuns.obj" \
	"$(INTDIR)\KOption.obj" \
	"$(INTDIR)\KMapMusic.obj" \
	"$(INTDIR)\KLadder.obj" \
	"$(INTDIR)\KForBitGua.obj" \
	"$(INTDIR)\KDropScript.obj" \
	"$(INTDIR)\KLogFile.obj" \
	"$(INTDIR)\DBTableS.obj" \
	"$(INTDIR)\stdafx.obj" \
	"..\..\Lib\release\engine.lib" \
	"..\..\Lib\release\common.lib" \
	"..\Library\LuaLib\Release\LuaLibDll.lib" \
	".\Src\USBKey\EP1KDL20.LIB" \
	".\Src\USBKey\EpsForJoLib.lib" \
	"..\..\Lib\libdb41s.lib"

"$(OUTDIR)\CoreClient.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# 开始自定义宏
OutDir=.\Client\Release
# 结束自定义宏

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\CoreClient.dll" "$(OUTDIR)\Core.pch"
   md ..\..\..\bin\client\release
	md ..\..\lib\release
	copy Client\Release\CoreClient.dll ..\..\bin\client\CoreClient.dll
	copy Client\Release\CoreClient.lib ..\..\lib\release\CoreClient.lib
	copy Client\Release\CoreClient.lib ..\..\bin\client\CoreClient.lib
	copy Client\Release\CoreClient.dll ..\..\bin\client\release\CoreClient.dll
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("Core.dep")
!INCLUDE "Core.dep"
!ELSE 
!MESSAGE Warning: cannot find "Core.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "Core - Win32 Server Debug" || "$(CFG)" == "Core - Win32 Server Release" || "$(CFG)" == "Core - Win32 Client Debug" || "$(CFG)" == "Core - Win32 Client Release"
SOURCE=.\Src\KPartnerSkill.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"


"$(INTDIR)\KPartnerSkill.obj"	"$(INTDIR)\KPartnerSkill.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"


"$(INTDIR)\KPartnerSkill.obj"	"$(INTDIR)\KPartnerSkill.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\KPartnerSkill.obj"	"$(INTDIR)\KPartnerSkill.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\KPartnerSkill.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KPlayerPartner.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"


"$(INTDIR)\KPlayerPartner.obj"	"$(INTDIR)\KPlayerPartner.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"


"$(INTDIR)\KPlayerPartner.obj"	"$(INTDIR)\KPlayerPartner.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\KPlayerPartner.obj"	"$(INTDIR)\KPlayerPartner.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\KPlayerPartner.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KThiefSkill.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"


"$(INTDIR)\KThiefSkill.obj"	"$(INTDIR)\KThiefSkill.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"


"$(INTDIR)\KThiefSkill.obj"	"$(INTDIR)\KThiefSkill.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\KThiefSkill.obj"	"$(INTDIR)\KThiefSkill.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\KThiefSkill.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KMissle.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"

CPP_SWITCHES=/nologo /MTd /W2 /Gm /GX /Zi /Od /I "..\engine\src" /I "..\engine\include" /I "..\Network" /I "..\..\Headers" /D "_DEBUG" /D "_SERVER" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CORE_EXPORTS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\Core.pch" /Yu"KCore.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /LD /GZ /c 

"$(INTDIR)\KMissle.obj"	"$(INTDIR)\KMissle.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"

CPP_SWITCHES=/nologo /MT /W2 /GX /O2 /I "..\engine\src" /I "..\engine\include" /I "..\Network" /I "..\..\Headers" /D "NDEBUG" /D "_SERVER" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CORE_EXPORTS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\Core.pch" /Yu"KCore.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\KMissle.obj"	"$(INTDIR)\KMissle.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\engine\src" /I "..\..\Headers" /I "..\engine\include" /I "..\Network" /I "src" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CORE_EXPORTS" /D "SWORDONLINE_SHOW_DBUG_INFO" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\Core.pch" /Yu"KCore.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\KMissle.obj"	"$(INTDIR)\KMissle.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"

CPP_SWITCHES=/nologo /MT /W2 /GX /Zi /O2 /I "..\engine\src" /I "..\engine\include" /I "..\Network" /I "..\..\Headers" /I "src" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CORE_EXPORTS" /FAs /Fa"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\KMissle.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\Src\KMissleRes.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"


"$(INTDIR)\KMissleRes.obj"	"$(INTDIR)\KMissleRes.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"


"$(INTDIR)\KMissleRes.obj"	"$(INTDIR)\KMissleRes.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\KMissleRes.obj"	"$(INTDIR)\KMissleRes.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\KMissleRes.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KMissleSet.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"


"$(INTDIR)\KMissleSet.obj"	"$(INTDIR)\KMissleSet.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"


"$(INTDIR)\KMissleSet.obj"	"$(INTDIR)\KMissleSet.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\KMissleSet.obj"	"$(INTDIR)\KMissleSet.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\KMissleSet.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KSkillList.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"


"$(INTDIR)\KSkillList.obj"	"$(INTDIR)\KSkillList.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"


"$(INTDIR)\KSkillList.obj"	"$(INTDIR)\KSkillList.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\KSkillList.obj"	"$(INTDIR)\KSkillList.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\KSkillList.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KSkillManager.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"


"$(INTDIR)\KSkillManager.obj"	"$(INTDIR)\KSkillManager.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"


"$(INTDIR)\KSkillManager.obj"	"$(INTDIR)\KSkillManager.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\KSkillManager.obj"	"$(INTDIR)\KSkillManager.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\KSkillManager.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KSkills.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"


"$(INTDIR)\KSkills.obj"	"$(INTDIR)\KSkills.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"


"$(INTDIR)\KSkills.obj"	"$(INTDIR)\KSkills.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\KSkills.obj"	"$(INTDIR)\KSkills.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\KSkills.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KSkillSpecial.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"


"$(INTDIR)\KSkillSpecial.obj"	"$(INTDIR)\KSkillSpecial.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"


"$(INTDIR)\KSkillSpecial.obj"	"$(INTDIR)\KSkillSpecial.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\KSkillSpecial.obj"	"$(INTDIR)\KSkillSpecial.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\KSkillSpecial.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\Skill.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"


"$(INTDIR)\Skill.obj"	"$(INTDIR)\Skill.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"

!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\Skill.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KFaction.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"


"$(INTDIR)\KFaction.obj"	"$(INTDIR)\KFaction.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"


"$(INTDIR)\KFaction.obj"	"$(INTDIR)\KFaction.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\KFaction.obj"	"$(INTDIR)\KFaction.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\KFaction.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KNpc.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"


"$(INTDIR)\KNpc.obj"	"$(INTDIR)\KNpc.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"


"$(INTDIR)\KNpc.obj"	"$(INTDIR)\KNpc.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\KNpc.obj"	"$(INTDIR)\KNpc.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\KNpc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KNpcAI.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"


"$(INTDIR)\KNpcAI.obj"	"$(INTDIR)\KNpcAI.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"


"$(INTDIR)\KNpcAI.obj"	"$(INTDIR)\KNpcAI.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\KNpcAI.obj"	"$(INTDIR)\KNpcAI.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\KNpcAI.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KNpcAttribModify.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"


"$(INTDIR)\KNpcAttribModify.obj"	"$(INTDIR)\KNpcAttribModify.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"


"$(INTDIR)\KNpcAttribModify.obj"	"$(INTDIR)\KNpcAttribModify.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\KNpcAttribModify.obj"	"$(INTDIR)\KNpcAttribModify.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\KNpcAttribModify.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KNpcDeathCalcExp.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"


"$(INTDIR)\KNpcDeathCalcExp.obj"	"$(INTDIR)\KNpcDeathCalcExp.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"


"$(INTDIR)\KNpcDeathCalcExp.obj"	"$(INTDIR)\KNpcDeathCalcExp.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\KNpcDeathCalcExp.obj"	"$(INTDIR)\KNpcDeathCalcExp.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\KNpcDeathCalcExp.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KNpcFindPath.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"


"$(INTDIR)\KNpcFindPath.obj"	"$(INTDIR)\KNpcFindPath.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"


"$(INTDIR)\KNpcFindPath.obj"	"$(INTDIR)\KNpcFindPath.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\KNpcFindPath.obj"	"$(INTDIR)\KNpcFindPath.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\KNpcFindPath.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KNpcGold.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"


"$(INTDIR)\KNpcGold.obj"	"$(INTDIR)\KNpcGold.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"


"$(INTDIR)\KNpcGold.obj"	"$(INTDIR)\KNpcGold.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\KNpcGold.obj"	"$(INTDIR)\KNpcGold.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\KNpcGold.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KNpcRes.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"


"$(INTDIR)\KNpcRes.obj"	"$(INTDIR)\KNpcRes.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"


"$(INTDIR)\KNpcRes.obj"	"$(INTDIR)\KNpcRes.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\KNpcRes.obj"	"$(INTDIR)\KNpcRes.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\KNpcRes.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KNpcResList.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"


"$(INTDIR)\KNpcResList.obj"	"$(INTDIR)\KNpcResList.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"


"$(INTDIR)\KNpcResList.obj"	"$(INTDIR)\KNpcResList.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\KNpcResList.obj"	"$(INTDIR)\KNpcResList.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\KNpcResList.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KNpcResNode.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"


"$(INTDIR)\KNpcResNode.obj"	"$(INTDIR)\KNpcResNode.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"


"$(INTDIR)\KNpcResNode.obj"	"$(INTDIR)\KNpcResNode.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\KNpcResNode.obj"	"$(INTDIR)\KNpcResNode.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\KNpcResNode.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KNpcSet.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"


"$(INTDIR)\KNpcSet.obj"	"$(INTDIR)\KNpcSet.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"


"$(INTDIR)\KNpcSet.obj"	"$(INTDIR)\KNpcSet.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\KNpcSet.obj"	"$(INTDIR)\KNpcSet.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\KNpcSet.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KNpcTemplate.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"


"$(INTDIR)\KNpcTemplate.obj"	"$(INTDIR)\KNpcTemplate.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"


"$(INTDIR)\KNpcTemplate.obj"	"$(INTDIR)\KNpcTemplate.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\KNpcTemplate.obj"	"$(INTDIR)\KNpcTemplate.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\KNpcTemplate.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KPlayer.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"


"$(INTDIR)\KPlayer.obj"	"$(INTDIR)\KPlayer.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"


"$(INTDIR)\KPlayer.obj"	"$(INTDIR)\KPlayer.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\KPlayer.obj"	"$(INTDIR)\KPlayer.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\KPlayer.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KPlayerChat.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"


"$(INTDIR)\KPlayerChat.obj"	"$(INTDIR)\KPlayerChat.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"


"$(INTDIR)\KPlayerChat.obj"	"$(INTDIR)\KPlayerChat.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\KPlayerChat.obj"	"$(INTDIR)\KPlayerChat.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\KPlayerChat.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KPlayerDBFuns.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"


"$(INTDIR)\KPlayerDBFuns.obj"	"$(INTDIR)\KPlayerDBFuns.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"


"$(INTDIR)\KPlayerDBFuns.obj"	"$(INTDIR)\KPlayerDBFuns.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\KPlayerDBFuns.obj"	"$(INTDIR)\KPlayerDBFuns.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\KPlayerDBFuns.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KPlayerFaction.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"


"$(INTDIR)\KPlayerFaction.obj"	"$(INTDIR)\KPlayerFaction.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"


"$(INTDIR)\KPlayerFaction.obj"	"$(INTDIR)\KPlayerFaction.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\KPlayerFaction.obj"	"$(INTDIR)\KPlayerFaction.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\KPlayerFaction.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KPlayerFuYuan.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"


"$(INTDIR)\KPlayerFuYuan.obj"	"$(INTDIR)\KPlayerFuYuan.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"


"$(INTDIR)\KPlayerFuYuan.obj"	"$(INTDIR)\KPlayerFuYuan.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\KPlayerFuYuan.obj"	"$(INTDIR)\KPlayerFuYuan.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\KPlayerFuYuan.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KPlayerMenuState.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"


"$(INTDIR)\KPlayerMenuState.obj"	"$(INTDIR)\KPlayerMenuState.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"


"$(INTDIR)\KPlayerMenuState.obj"	"$(INTDIR)\KPlayerMenuState.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\KPlayerMenuState.obj"	"$(INTDIR)\KPlayerMenuState.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\KPlayerMenuState.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KPlayerPK.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"


"$(INTDIR)\KPlayerPK.obj"	"$(INTDIR)\KPlayerPK.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"


"$(INTDIR)\KPlayerPK.obj"	"$(INTDIR)\KPlayerPK.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\KPlayerPK.obj"	"$(INTDIR)\KPlayerPK.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\KPlayerPK.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KPlayerReBorn.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"


"$(INTDIR)\KPlayerReBorn.obj"	"$(INTDIR)\KPlayerReBorn.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"


"$(INTDIR)\KPlayerReBorn.obj"	"$(INTDIR)\KPlayerReBorn.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\KPlayerReBorn.obj"	"$(INTDIR)\KPlayerReBorn.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\KPlayerReBorn.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KPlayerRepute.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"


"$(INTDIR)\KPlayerRepute.obj"	"$(INTDIR)\KPlayerRepute.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"


"$(INTDIR)\KPlayerRepute.obj"	"$(INTDIR)\KPlayerRepute.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\KPlayerRepute.obj"	"$(INTDIR)\KPlayerRepute.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\KPlayerRepute.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KPlayerSet.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"


"$(INTDIR)\KPlayerSet.obj"	"$(INTDIR)\KPlayerSet.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"


"$(INTDIR)\KPlayerSet.obj"	"$(INTDIR)\KPlayerSet.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\KPlayerSet.obj"	"$(INTDIR)\KPlayerSet.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\KPlayerSet.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KPlayerTask.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"


"$(INTDIR)\KPlayerTask.obj"	"$(INTDIR)\KPlayerTask.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"


"$(INTDIR)\KPlayerTask.obj"	"$(INTDIR)\KPlayerTask.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\KPlayerTask.obj"	"$(INTDIR)\KPlayerTask.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\KPlayerTask.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KPlayerTeam.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"


"$(INTDIR)\KPlayerTeam.obj"	"$(INTDIR)\KPlayerTeam.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"


"$(INTDIR)\KPlayerTeam.obj"	"$(INTDIR)\KPlayerTeam.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\KPlayerTeam.obj"	"$(INTDIR)\KPlayerTeam.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\KPlayerTeam.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KPlayerTong.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"


"$(INTDIR)\KPlayerTong.obj"	"$(INTDIR)\KPlayerTong.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"


"$(INTDIR)\KPlayerTong.obj"	"$(INTDIR)\KPlayerTong.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\KPlayerTong.obj"	"$(INTDIR)\KPlayerTong.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\KPlayerTong.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KPlayerTrade.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"


"$(INTDIR)\KPlayerTrade.obj"	"$(INTDIR)\KPlayerTrade.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"


"$(INTDIR)\KPlayerTrade.obj"	"$(INTDIR)\KPlayerTrade.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\KPlayerTrade.obj"	"$(INTDIR)\KPlayerTrade.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\KPlayerTrade.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KSprControl.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"


"$(INTDIR)\KSprControl.obj"	"$(INTDIR)\KSprControl.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"


"$(INTDIR)\KSprControl.obj"	"$(INTDIR)\KSprControl.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\KSprControl.obj"	"$(INTDIR)\KSprControl.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\KSprControl.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KBasPropTbl.CPP

!IF  "$(CFG)" == "Core - Win32 Server Debug"


"$(INTDIR)\KBasPropTbl.obj"	"$(INTDIR)\KBasPropTbl.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"


"$(INTDIR)\KBasPropTbl.obj"	"$(INTDIR)\KBasPropTbl.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\KBasPropTbl.obj"	"$(INTDIR)\KBasPropTbl.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\KBasPropTbl.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KBuySell.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"


"$(INTDIR)\KBuySell.obj"	"$(INTDIR)\KBuySell.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"


"$(INTDIR)\KBuySell.obj"	"$(INTDIR)\KBuySell.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\KBuySell.obj"	"$(INTDIR)\KBuySell.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\KBuySell.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KInventory.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"


"$(INTDIR)\KInventory.obj"	"$(INTDIR)\KInventory.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"


"$(INTDIR)\KInventory.obj"	"$(INTDIR)\KInventory.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\KInventory.obj"	"$(INTDIR)\KInventory.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\KInventory.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KItem.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"


"$(INTDIR)\KItem.obj"	"$(INTDIR)\KItem.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"


"$(INTDIR)\KItem.obj"	"$(INTDIR)\KItem.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\KItem.obj"	"$(INTDIR)\KItem.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\KItem.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KItemChangeRes.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"


"$(INTDIR)\KItemChangeRes.obj"	"$(INTDIR)\KItemChangeRes.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"


"$(INTDIR)\KItemChangeRes.obj"	"$(INTDIR)\KItemChangeRes.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\KItemChangeRes.obj"	"$(INTDIR)\KItemChangeRes.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\KItemChangeRes.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KItemGenerator.CPP

!IF  "$(CFG)" == "Core - Win32 Server Debug"


"$(INTDIR)\KItemGenerator.obj"	"$(INTDIR)\KItemGenerator.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"


"$(INTDIR)\KItemGenerator.obj"	"$(INTDIR)\KItemGenerator.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\KItemGenerator.obj"	"$(INTDIR)\KItemGenerator.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\KItemGenerator.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KItemList.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"


"$(INTDIR)\KItemList.obj"	"$(INTDIR)\KItemList.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"


"$(INTDIR)\KItemList.obj"	"$(INTDIR)\KItemList.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\KItemList.obj"	"$(INTDIR)\KItemList.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\KItemList.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KItemSet.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"


"$(INTDIR)\KItemSet.obj"	"$(INTDIR)\KItemSet.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"


"$(INTDIR)\KItemSet.obj"	"$(INTDIR)\KItemSet.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\KItemSet.obj"	"$(INTDIR)\KItemSet.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\KItemSet.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KObj.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"


"$(INTDIR)\KObj.obj"	"$(INTDIR)\KObj.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"


"$(INTDIR)\KObj.obj"	"$(INTDIR)\KObj.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\KObj.obj"	"$(INTDIR)\KObj.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\KObj.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KObjSet.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"


"$(INTDIR)\KObjSet.obj"	"$(INTDIR)\KObjSet.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"


"$(INTDIR)\KObjSet.obj"	"$(INTDIR)\KObjSet.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\KObjSet.obj"	"$(INTDIR)\KObjSet.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\KObjSet.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KSellItem.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"


"$(INTDIR)\KSellItem.obj"	"$(INTDIR)\KSellItem.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"


"$(INTDIR)\KSellItem.obj"	"$(INTDIR)\KSellItem.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\KSellItem.obj"	"$(INTDIR)\KSellItem.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\KSellItem.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KViewItem.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"


"$(INTDIR)\KViewItem.obj"	"$(INTDIR)\KViewItem.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"


"$(INTDIR)\KViewItem.obj"	"$(INTDIR)\KViewItem.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\KViewItem.obj"	"$(INTDIR)\KViewItem.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\KViewItem.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KRegion.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"


"$(INTDIR)\KRegion.obj"	"$(INTDIR)\KRegion.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"


"$(INTDIR)\KRegion.obj"	"$(INTDIR)\KRegion.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\KRegion.obj"	"$(INTDIR)\KRegion.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\KRegion.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KCore.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"


"$(INTDIR)\KCore.obj"	"$(INTDIR)\KCore.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"


"$(INTDIR)\KCore.obj"	"$(INTDIR)\KCore.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\KCore.obj"	"$(INTDIR)\KCore.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\KCore.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KSubWorld.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"


"$(INTDIR)\KSubWorld.obj"	"$(INTDIR)\KSubWorld.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"


"$(INTDIR)\KSubWorld.obj"	"$(INTDIR)\KSubWorld.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\KSubWorld.obj"	"$(INTDIR)\KSubWorld.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\KSubWorld.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KSubWorldSet.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"


"$(INTDIR)\KSubWorldSet.obj"	"$(INTDIR)\KSubWorldSet.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"


"$(INTDIR)\KSubWorldSet.obj"	"$(INTDIR)\KSubWorldSet.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\KSubWorldSet.obj"	"$(INTDIR)\KSubWorldSet.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\KSubWorldSet.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KWeatherMgr.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"


"$(INTDIR)\KWeatherMgr.obj"	"$(INTDIR)\KWeatherMgr.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"


"$(INTDIR)\KWeatherMgr.obj"	"$(INTDIR)\KWeatherMgr.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\KWeatherMgr.obj"	"$(INTDIR)\KWeatherMgr.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\KWeatherMgr.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\S3P_MSSQLServer_Result.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"


"$(INTDIR)\S3P_MSSQLServer_Result.obj"	"$(INTDIR)\S3P_MSSQLServer_Result.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"


"$(INTDIR)\S3P_MSSQLServer_Result.obj"	"$(INTDIR)\S3P_MSSQLServer_Result.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\S3P_MSSQLServer_Result.obj"	"$(INTDIR)\S3P_MSSQLServer_Result.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\S3P_MSSQLServer_Result.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\S3PDB_MSSQLServer_Connection.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"


"$(INTDIR)\S3PDB_MSSQLServer_Connection.obj"	"$(INTDIR)\S3PDB_MSSQLServer_Connection.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"


"$(INTDIR)\S3PDB_MSSQLServer_Connection.obj"	"$(INTDIR)\S3PDB_MSSQLServer_Connection.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\S3PDB_MSSQLServer_Connection.obj"	"$(INTDIR)\S3PDB_MSSQLServer_Connection.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\S3PDB_MSSQLServer_Connection.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\S3PDBConVBC.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"


"$(INTDIR)\S3PDBConVBC.obj"	"$(INTDIR)\S3PDBConVBC.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"


"$(INTDIR)\S3PDBConVBC.obj"	"$(INTDIR)\S3PDBConVBC.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\S3PDBConVBC.obj"	"$(INTDIR)\S3PDBConVBC.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\S3PDBConVBC.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\S3PResultVBC.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"


"$(INTDIR)\S3PResultVBC.obj"	"$(INTDIR)\S3PResultVBC.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"


"$(INTDIR)\S3PResultVBC.obj"	"$(INTDIR)\S3PResultVBC.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\S3PResultVBC.obj"	"$(INTDIR)\S3PResultVBC.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\S3PResultVBC.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KWorldMsg.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"


"$(INTDIR)\KWorldMsg.obj"	"$(INTDIR)\KWorldMsg.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"


"$(INTDIR)\KWorldMsg.obj"	"$(INTDIR)\KWorldMsg.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\KWorldMsg.obj"	"$(INTDIR)\KWorldMsg.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\KWorldMsg.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KMath.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"


"$(INTDIR)\KMath.obj"	"$(INTDIR)\KMath.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"


"$(INTDIR)\KMath.obj"	"$(INTDIR)\KMath.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\KMath.obj"	"$(INTDIR)\KMath.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\KMath.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KGMProcess.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"


"$(INTDIR)\KGMProcess.obj"	"$(INTDIR)\KGMProcess.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"


"$(INTDIR)\KGMProcess.obj"	"$(INTDIR)\KGMProcess.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"

!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"

!ENDIF 

SOURCE=.\Src\KNewProtocolProcess.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"


"$(INTDIR)\KNewProtocolProcess.obj"	"$(INTDIR)\KNewProtocolProcess.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"


"$(INTDIR)\KNewProtocolProcess.obj"	"$(INTDIR)\KNewProtocolProcess.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"

!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"

!ENDIF 

SOURCE=.\Src\KProtocol.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"


"$(INTDIR)\KProtocol.obj"	"$(INTDIR)\KProtocol.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"


"$(INTDIR)\KProtocol.obj"	"$(INTDIR)\KProtocol.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\KProtocol.obj"	"$(INTDIR)\KProtocol.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\KProtocol.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KProtocolProcess.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"


"$(INTDIR)\KProtocolProcess.obj"	"$(INTDIR)\KProtocolProcess.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"


"$(INTDIR)\KProtocolProcess.obj"	"$(INTDIR)\KProtocolProcess.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\KProtocolProcess.obj"	"$(INTDIR)\KProtocolProcess.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\KProtocolProcess.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\ScriptFuns.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"


"$(INTDIR)\ScriptFuns.obj"	"$(INTDIR)\ScriptFuns.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"


"$(INTDIR)\ScriptFuns.obj"	"$(INTDIR)\ScriptFuns.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\ScriptFuns.obj"	"$(INTDIR)\ScriptFuns.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\ScriptFuns.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\LuaFuns.cpp
SOURCE=.\Src\KScriptValueSet.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"


"$(INTDIR)\KScriptValueSet.obj"	"$(INTDIR)\KScriptValueSet.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"


"$(INTDIR)\KScriptValueSet.obj"	"$(INTDIR)\KScriptValueSet.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\KScriptValueSet.obj"	"$(INTDIR)\KScriptValueSet.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\KScriptValueSet.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KSortScript.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"


"$(INTDIR)\KSortScript.obj"	"$(INTDIR)\KSortScript.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"


"$(INTDIR)\KSortScript.obj"	"$(INTDIR)\KSortScript.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\KSortScript.obj"	"$(INTDIR)\KSortScript.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\KSortScript.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\CoreServerShell.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"


"$(INTDIR)\CoreServerShell.obj"	"$(INTDIR)\CoreServerShell.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"


"$(INTDIR)\CoreServerShell.obj"	"$(INTDIR)\CoreServerShell.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"

!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"

!ENDIF 

SOURCE=.\Src\CoreShell.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"

!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\CoreShell.obj"	"$(INTDIR)\CoreShell.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\CoreShell.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\Scene\KIpotBranch.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"

!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\engine\src" /I "..\..\Headers" /I "..\engine\include" /I "..\Network" /I "src" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CORE_EXPORTS" /D "SWORDONLINE_SHOW_DBUG_INFO" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\Core.pch" /Yu"KCore.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\KIpotBranch.obj"	"$(INTDIR)\KIpotBranch.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"

CPP_SWITCHES=/nologo /MT /W2 /GX /Zi /O2 /I "..\engine\src" /I "..\engine\include" /I "..\Network" /I "..\..\Headers" /I "src" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CORE_EXPORTS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\KIpotBranch.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\Src\Scene\KIpotLeaf.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"

!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\KIpotLeaf.obj"	"$(INTDIR)\KIpotLeaf.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\KIpotLeaf.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\Scene\KIpoTree.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"

!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\engine\src" /I "..\..\Headers" /I "..\engine\include" /I "..\Network" /I "src" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CORE_EXPORTS" /D "SWORDONLINE_SHOW_DBUG_INFO" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\KIpoTree.obj"	"$(INTDIR)\KIpoTree.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"

CPP_SWITCHES=/nologo /MT /W2 /GX /Zi /O2 /I "..\engine\src" /I "..\engine\include" /I "..\Network" /I "..\..\Headers" /I "src" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CORE_EXPORTS" /FAs /Fa"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\KIpoTree.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\Src\Scene\Kjxpathfinder.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"


"$(INTDIR)\Kjxpathfinder.obj"	"$(INTDIR)\Kjxpathfinder.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"


"$(INTDIR)\Kjxpathfinder.obj"	"$(INTDIR)\Kjxpathfinder.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\Kjxpathfinder.obj"	"$(INTDIR)\Kjxpathfinder.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\Kjxpathfinder.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\Scene\Kpathfinder.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"


"$(INTDIR)\Kpathfinder.obj"	"$(INTDIR)\Kpathfinder.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"


"$(INTDIR)\Kpathfinder.obj"	"$(INTDIR)\Kpathfinder.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\Kpathfinder.obj"	"$(INTDIR)\Kpathfinder.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\Kpathfinder.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KLittleMap.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"


"$(INTDIR)\KLittleMap.obj"	"$(INTDIR)\KLittleMap.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"


"$(INTDIR)\KLittleMap.obj"	"$(INTDIR)\KLittleMap.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\KLittleMap.obj"	"$(INTDIR)\KLittleMap.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\KLittleMap.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\Scene\KScenePlaceC.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"

!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\KScenePlaceC.obj"	"$(INTDIR)\KScenePlaceC.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\KScenePlaceC.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\Scene\KScenePlaceRegionC.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"

!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\KScenePlaceRegionC.obj"	"$(INTDIR)\KScenePlaceRegionC.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\KScenePlaceRegionC.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\Scene\KWeather.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"


"$(INTDIR)\KWeather.obj"	"$(INTDIR)\KWeather.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"


"$(INTDIR)\KWeather.obj"	"$(INTDIR)\KWeather.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\KWeather.obj"	"$(INTDIR)\KWeather.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\KWeather.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\Scene\SceneMath.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"

!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\engine\src" /I "..\..\Headers" /I "..\engine\include" /I "..\Network" /I "src" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CORE_EXPORTS" /D "SWORDONLINE_SHOW_DBUG_INFO" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\Core.pch" /Yu"KCore.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\SceneMath.obj"	"$(INTDIR)\SceneMath.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"

CPP_SWITCHES=/nologo /MT /W2 /GX /Zi /O2 /I "..\engine\src" /I "..\engine\include" /I "..\Network" /I "..\..\Headers" /I "src" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CORE_EXPORTS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\SceneMath.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\Src\Scene\ScenePlaceMapC.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"

!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\ScenePlaceMapC.obj"	"$(INTDIR)\ScenePlaceMapC.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\ScenePlaceMapC.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\CoreDrawGameObj.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"

!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\CoreDrawGameObj.obj"	"$(INTDIR)\CoreDrawGameObj.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\CoreDrawGameObj.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\ImgRef.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"

!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\ImgRef.obj"	"$(INTDIR)\ImgRef.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\ImgRef.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KMagicDesc.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"


"$(INTDIR)\KMagicDesc.obj"	"$(INTDIR)\KMagicDesc.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"


"$(INTDIR)\KMagicDesc.obj"	"$(INTDIR)\KMagicDesc.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\KMagicDesc.obj"	"$(INTDIR)\KMagicDesc.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\KMagicDesc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KGMCommand.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"


"$(INTDIR)\KGMCommand.obj"	"$(INTDIR)\KGMCommand.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"


"$(INTDIR)\KGMCommand.obj"	"$(INTDIR)\KGMCommand.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\KGMCommand.obj"	"$(INTDIR)\KGMCommand.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\KGMCommand.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KMission.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"


"$(INTDIR)\KMission.obj"	"$(INTDIR)\KMission.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"


"$(INTDIR)\KMission.obj"	"$(INTDIR)\KMission.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"

!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"

!ENDIF 

SOURCE=.\Src\KTaskFuns.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"

CPP_SWITCHES=/nologo /MTd /W2 /Gm /GX /Zi /Od /I "..\engine\src" /I "..\engine\include" /I "..\Network" /I "..\..\Headers" /D "_DEBUG" /D "_SERVER" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CORE_EXPORTS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\Core.pch" /Yu"KCore.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /LD /GZ /c 

"$(INTDIR)\KTaskFuns.obj"	"$(INTDIR)\KTaskFuns.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"

CPP_SWITCHES=/nologo /MT /W2 /GX /O2 /I "..\engine\src" /I "..\engine\include" /I "..\Network" /I "..\..\Headers" /D "NDEBUG" /D "_SERVER" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CORE_EXPORTS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\Core.pch" /Yu"KCore.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\KTaskFuns.obj"	"$(INTDIR)\KTaskFuns.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\engine\src" /I "..\..\Headers" /I "..\engine\include" /I "..\Network" /I "src" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CORE_EXPORTS" /D "SWORDONLINE_SHOW_DBUG_INFO" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\Core.pch" /Yu"KCore.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\KTaskFuns.obj"	"$(INTDIR)\KTaskFuns.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"

CPP_SWITCHES=/nologo /MT /W2 /GX /Zi /O2 /I "..\engine\src" /I "..\engine\include" /I "..\Network" /I "..\..\Headers" /I "src" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CORE_EXPORTS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\KTaskFuns.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\Src\KOption.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"


"$(INTDIR)\KOption.obj"	"$(INTDIR)\KOption.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"


"$(INTDIR)\KOption.obj"	"$(INTDIR)\KOption.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\KOption.obj"	"$(INTDIR)\KOption.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\KOption.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KMapMusic.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"


"$(INTDIR)\KMapMusic.obj"	"$(INTDIR)\KMapMusic.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"


"$(INTDIR)\KMapMusic.obj"	"$(INTDIR)\KMapMusic.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\KMapMusic.obj"	"$(INTDIR)\KMapMusic.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\KMapMusic.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KLadder.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"


"$(INTDIR)\KLadder.obj"	"$(INTDIR)\KLadder.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"


"$(INTDIR)\KLadder.obj"	"$(INTDIR)\KLadder.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\KLadder.obj"	"$(INTDIR)\KLadder.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\KLadder.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Engine\Src\KForBitGua.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"


"$(INTDIR)\KForBitGua.obj"	"$(INTDIR)\KForBitGua.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"


"$(INTDIR)\KForBitGua.obj"	"$(INTDIR)\KForBitGua.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\KForBitGua.obj"	"$(INTDIR)\KForBitGua.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\KForBitGua.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KDropScript.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"


"$(INTDIR)\KDropScript.obj"	"$(INTDIR)\KDropScript.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"


"$(INTDIR)\KDropScript.obj"	"$(INTDIR)\KDropScript.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\KDropScript.obj"	"$(INTDIR)\KDropScript.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\KDropScript.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KLogFile.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"


"$(INTDIR)\KLogFile.obj"	"$(INTDIR)\KLogFile.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"


"$(INTDIR)\KLogFile.obj"	"$(INTDIR)\KLogFile.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\KLogFile.obj"	"$(INTDIR)\KLogFile.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\KLogFile.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\DBTableS.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"


"$(INTDIR)\DBTableS.obj"	"$(INTDIR)\DBTableS.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"


"$(INTDIR)\DBTableS.obj"	"$(INTDIR)\DBTableS.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"


"$(INTDIR)\DBTableS.obj"	"$(INTDIR)\DBTableS.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Core.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"


"$(INTDIR)\DBTableS.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\stdafx.cpp

!IF  "$(CFG)" == "Core - Win32 Server Debug"

CPP_SWITCHES=/nologo /MTd /W2 /Gm /GX /Zi /Od /I "..\engine\src" /I "..\engine\include" /I "..\Network" /I "..\..\Headers" /D "_DEBUG" /D "_SERVER" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CORE_EXPORTS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\Core.pch" /Yc"KCore.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /LD /GZ /c 

"$(INTDIR)\stdafx.obj"	"$(INTDIR)\stdafx.sbr"	"$(INTDIR)\Core.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Core - Win32 Server Release"

CPP_SWITCHES=/nologo /MT /W2 /GX /O2 /I "..\engine\src" /I "..\engine\include" /I "..\Network" /I "..\..\Headers" /D "NDEBUG" /D "_SERVER" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CORE_EXPORTS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\Core.pch" /Yc"KCore.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\stdafx.obj"	"$(INTDIR)\stdafx.sbr"	"$(INTDIR)\Core.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Core - Win32 Client Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\engine\src" /I "..\..\Headers" /I "..\engine\include" /I "..\Network" /I "src" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CORE_EXPORTS" /D "SWORDONLINE_SHOW_DBUG_INFO" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\Core.pch" /Yc"KCore.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\stdafx.obj"	"$(INTDIR)\stdafx.sbr"	"$(INTDIR)\Core.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Core - Win32 Client Release"

CPP_SWITCHES=/nologo /MT /W2 /GX /Zi /O2 /I "..\engine\src" /I "..\engine\include" /I "..\Network" /I "..\..\Headers" /I "src" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CORE_EXPORTS" /Fp"$(INTDIR)\Core.pch" /Yc"KCore.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\stdafx.obj"	"$(INTDIR)\Core.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 


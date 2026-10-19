# Microsoft Developer Studio Generated NMAKE File, Based on Engine.dsp
!IF $(CFG)" == "
CFG=Engine - Win32 OutRead Debug
!MESSAGE No configuration specified. Defaulting to Engine - Win32 OutRead Debug.
!ENDIF 

!IF "$(CFG)" != "Engine - Win32 Release" && "$(CFG)" != "Engine - Win32 Debug" && "$(CFG)" != "Engine - Win32 OutRead Debug" && "$(CFG)" != "Engine - Win32 OuRead Release"
!MESSAGE 指定的配置 "$(CFG)" 无效.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Engine.mak" CFG="Engine - Win32 OutRead Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Engine - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Engine - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Engine - Win32 OutRead Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Engine - Win32 OuRead Release" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "Engine - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# 开始自定义宏
OutDir=.\Release
# 结束自定义宏

ALL : "$(OUTDIR)\Engine.dll" "$(OUTDIR)\Engine.pch"


CLEAN :
	-@erase "$(INTDIR)\alloc.obj"
	-@erase "$(INTDIR)\EDOneTimePad.obj"
	-@erase "$(INTDIR)\Engine.pch"
	-@erase "$(INTDIR)\io.obj"
	-@erase "$(INTDIR)\KAutoMutex.obj"
	-@erase "$(INTDIR)\KAviFile.obj"
	-@erase "$(INTDIR)\KBitmap.obj"
	-@erase "$(INTDIR)\KBitmap16.obj"
	-@erase "$(INTDIR)\KBitmapConvert.obj"
	-@erase "$(INTDIR)\KBmp2Spr.obj"
	-@erase "$(INTDIR)\KBmpFile.obj"
	-@erase "$(INTDIR)\KBmpFile24.obj"
	-@erase "$(INTDIR)\KCache.obj"
	-@erase "$(INTDIR)\KCanvas.obj"
	-@erase "$(INTDIR)\KCodec.obj"
	-@erase "$(INTDIR)\KCodecLzo.obj"
	-@erase "$(INTDIR)\KColors.obj"
	-@erase "$(INTDIR)\KDDraw.obj"
	-@erase "$(INTDIR)\KDebug.obj"
	-@erase "$(INTDIR)\KDError.obj"
	-@erase "$(INTDIR)\KDInput.obj"
	-@erase "$(INTDIR)\KDrawBase.obj"
	-@erase "$(INTDIR)\KDrawBitmap.obj"
	-@erase "$(INTDIR)\KDrawBitmap16.obj"
	-@erase "$(INTDIR)\KDrawFade.obj"
	-@erase "$(INTDIR)\KDrawFont.obj"
	-@erase "$(INTDIR)\KDrawSprite.obj"
	-@erase "$(INTDIR)\KDrawSpriteAlpha.obj"
	-@erase "$(INTDIR)\KDSound.obj"
	-@erase "$(INTDIR)\KEicScript.obj"
	-@erase "$(INTDIR)\KEicScriptSet.obj"
	-@erase "$(INTDIR)\KEngine.obj"
	-@erase "$(INTDIR)\KEvent.obj"
	-@erase "$(INTDIR)\KFile.obj"
	-@erase "$(INTDIR)\KFileCopy.obj"
	-@erase "$(INTDIR)\KFileDialog.obj"
	-@erase "$(INTDIR)\KFilePath.obj"
	-@erase "$(INTDIR)\KFindBinTree.obj"
	-@erase "$(INTDIR)\KFont.obj"
	-@erase "$(INTDIR)\KGifFile.obj"
	-@erase "$(INTDIR)\KGraphics.obj"
	-@erase "$(INTDIR)\KHashList.obj"
	-@erase "$(INTDIR)\KHashNode.obj"
	-@erase "$(INTDIR)\KHashTable.obj"
	-@erase "$(INTDIR)\Kime.obj"
	-@erase "$(INTDIR)\KIniFile.obj"
	-@erase "$(INTDIR)\KJpgFile.obj"
	-@erase "$(INTDIR)\KKeyboard.obj"
	-@erase "$(INTDIR)\KLinkArray.obj"
	-@erase "$(INTDIR)\KList.obj"
	-@erase "$(INTDIR)\KLuaScript.obj"
	-@erase "$(INTDIR)\KLuaScriptSet.obj"
	-@erase "$(INTDIR)\KLubCmpl_Blocker.obj"
	-@erase "$(INTDIR)\KMemBase.obj"
	-@erase "$(INTDIR)\KMemClass.obj"
	-@erase "$(INTDIR)\KMemClass1.obj"
	-@erase "$(INTDIR)\KMemManager.obj"
	-@erase "$(INTDIR)\KMemStack.obj"
	-@erase "$(INTDIR)\KMessage.obj"
	-@erase "$(INTDIR)\KMouse.obj"
	-@erase "$(INTDIR)\KMp3Music.obj"
	-@erase "$(INTDIR)\KMp4Audio.obj"
	-@erase "$(INTDIR)\KMp4Movie.obj"
	-@erase "$(INTDIR)\KMp4Video.obj"
	-@erase "$(INTDIR)\KMpgMusic.obj"
	-@erase "$(INTDIR)\KMsgNode.obj"
	-@erase "$(INTDIR)\KMusic.obj"
	-@erase "$(INTDIR)\KMutex.obj"
	-@erase "$(INTDIR)\KNode.obj"
	-@erase "$(INTDIR)\KOctree.obj"
	-@erase "$(INTDIR)\KOctreeNode.obj"
	-@erase "$(INTDIR)\KPakData.obj"
	-@erase "$(INTDIR)\KPakFile.obj"
	-@erase "$(INTDIR)\KPakList.obj"
	-@erase "$(INTDIR)\KPakTool.obj"
	-@erase "$(INTDIR)\KPalette.obj"
	-@erase "$(INTDIR)\KPcxFile.obj"
	-@erase "$(INTDIR)\KPolygon.obj"
	-@erase "$(INTDIR)\KPolyRelation.obj"
	-@erase "$(INTDIR)\KRandom.obj"
	-@erase "$(INTDIR)\KSafeList.obj"
	-@erase "$(INTDIR)\KScanDir.obj"
	-@erase "$(INTDIR)\KScript.obj"
	-@erase "$(INTDIR)\KScriptCache.obj"
	-@erase "$(INTDIR)\KScriptList.obj"
	-@erase "$(INTDIR)\KScriptSet.obj"
	-@erase "$(INTDIR)\KSG_MD5_String.obj"
	-@erase "$(INTDIR)\KSG_StringProcess.obj"
	-@erase "$(INTDIR)\KSortBinTree.obj"
	-@erase "$(INTDIR)\KSortList.obj"
	-@erase "$(INTDIR)\KSoundCache.obj"
	-@erase "$(INTDIR)\KSprite.obj"
	-@erase "$(INTDIR)\KSpriteCache.obj"
	-@erase "$(INTDIR)\KSpriteCodec.obj"
	-@erase "$(INTDIR)\KSpriteMaker.obj"
	-@erase "$(INTDIR)\KStepLuaScript.obj"
	-@erase "$(INTDIR)\KStrBase.obj"
	-@erase "$(INTDIR)\KStrList.obj"
	-@erase "$(INTDIR)\KStrNode.obj"
	-@erase "$(INTDIR)\KTabFile.obj"
	-@erase "$(INTDIR)\KTabFileCtrl.obj"
	-@erase "$(INTDIR)\KTgaFile32.obj"
	-@erase "$(INTDIR)\KThread.obj"
	-@erase "$(INTDIR)\KTimer.obj"
	-@erase "$(INTDIR)\KVideo.obj"
	-@erase "$(INTDIR)\KWavCodec.obj"
	-@erase "$(INTDIR)\KWavFile.obj"
	-@erase "$(INTDIR)\KWavMusic.obj"
	-@erase "$(INTDIR)\KWavSound.obj"
	-@erase "$(INTDIR)\KWin32.obj"
	-@erase "$(INTDIR)\KWin32App.obj"
	-@erase "$(INTDIR)\KWin32Wnd.obj"
	-@erase "$(INTDIR)\KZipCodec.obj"
	-@erase "$(INTDIR)\KZipData.obj"
	-@erase "$(INTDIR)\KZipFile.obj"
	-@erase "$(INTDIR)\KZipList.obj"
	-@erase "$(INTDIR)\md5.obj"
	-@erase "$(INTDIR)\n2b_99.obj"
	-@erase "$(INTDIR)\n2b_d.obj"
	-@erase "$(INTDIR)\n2b_ds.obj"
	-@erase "$(INTDIR)\n2b_to.obj"
	-@erase "$(INTDIR)\n2d_99.obj"
	-@erase "$(INTDIR)\n2d_d.obj"
	-@erase "$(INTDIR)\n2d_ds.obj"
	-@erase "$(INTDIR)\n2d_to.obj"
	-@erase "$(INTDIR)\n2e_99.obj"
	-@erase "$(INTDIR)\n2e_d.obj"
	-@erase "$(INTDIR)\n2e_ds.obj"
	-@erase "$(INTDIR)\n2e_to.obj"
	-@erase "$(INTDIR)\PassChecksum.obj"
	-@erase "$(INTDIR)\stdafx.obj"
	-@erase "$(INTDIR)\Text.obj"
	-@erase "$(INTDIR)\ucl_crc.obj"
	-@erase "$(INTDIR)\ucl_dll.obj"
	-@erase "$(INTDIR)\ucl_init.obj"
	-@erase "$(INTDIR)\ucl_ptr.obj"
	-@erase "$(INTDIR)\ucl_str.obj"
	-@erase "$(INTDIR)\ucl_util.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\XPackFile.obj"
	-@erase "$(INTDIR)\ZSPRPackFile.obj"
	-@erase "$(OUTDIR)\Engine.dll"
	-@erase "$(OUTDIR)\Engine.exp"
	-@erase "$(OUTDIR)\Engine.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MT /W3 /GX /Zi /O2 /I "include" /I "..\Pack\ucl-1.01\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D WINVER=0x0500 /Fp"$(INTDIR)\Engine.pch" /YX"KWin32.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Engine.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=winmm.lib user32.lib dinput8.lib dxguid.lib ddraw.lib dsound.lib gdi32.lib comdlg32.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\Engine.pdb" /machine:I386 /nodefaultlib:"LIBCMT.lib" /out:"$(OUTDIR)\Engine.dll" /implib:"$(OUTDIR)\Engine.lib" 
LINK32_OBJS= \
	"$(INTDIR)\KAutoMutex.obj" \
	"$(INTDIR)\KAviFile.obj" \
	"$(INTDIR)\KBitmap.obj" \
	"$(INTDIR)\KBitmap16.obj" \
	"$(INTDIR)\KBitmapConvert.obj" \
	"$(INTDIR)\KBmp2Spr.obj" \
	"$(INTDIR)\KBmpFile.obj" \
	"$(INTDIR)\KBmpFile24.obj" \
	"$(INTDIR)\KCache.obj" \
	"$(INTDIR)\KCanvas.obj" \
	"$(INTDIR)\KCodec.obj" \
	"$(INTDIR)\KCodecLzo.obj" \
	"$(INTDIR)\KColors.obj" \
	"$(INTDIR)\KDDraw.obj" \
	"$(INTDIR)\KDebug.obj" \
	"$(INTDIR)\KDError.obj" \
	"$(INTDIR)\KDInput.obj" \
	"$(INTDIR)\KDrawBase.obj" \
	"$(INTDIR)\KDrawBitmap.obj" \
	"$(INTDIR)\KDrawBitmap16.obj" \
	"$(INTDIR)\KDrawFade.obj" \
	"$(INTDIR)\KDrawFont.obj" \
	"$(INTDIR)\KDrawSprite.obj" \
	"$(INTDIR)\KDrawSpriteAlpha.obj" \
	"$(INTDIR)\KDSound.obj" \
	"$(INTDIR)\KEicScript.obj" \
	"$(INTDIR)\KEicScriptSet.obj" \
	"$(INTDIR)\KEngine.obj" \
	"$(INTDIR)\KEvent.obj" \
	"$(INTDIR)\KFile.obj" \
	"$(INTDIR)\KFileCopy.obj" \
	"$(INTDIR)\KFileDialog.obj" \
	"$(INTDIR)\KFilePath.obj" \
	"$(INTDIR)\KFindBinTree.obj" \
	"$(INTDIR)\KFont.obj" \
	"$(INTDIR)\KGifFile.obj" \
	"$(INTDIR)\KGraphics.obj" \
	"$(INTDIR)\KHashList.obj" \
	"$(INTDIR)\KHashNode.obj" \
	"$(INTDIR)\KHashTable.obj" \
	"$(INTDIR)\Kime.obj" \
	"$(INTDIR)\KIniFile.obj" \
	"$(INTDIR)\KJpgFile.obj" \
	"$(INTDIR)\KKeyboard.obj" \
	"$(INTDIR)\KLinkArray.obj" \
	"$(INTDIR)\KList.obj" \
	"$(INTDIR)\KLuaScript.obj" \
	"$(INTDIR)\KLuaScriptSet.obj" \
	"$(INTDIR)\KLubCmpl_Blocker.obj" \
	"$(INTDIR)\KMemBase.obj" \
	"$(INTDIR)\KMemClass.obj" \
	"$(INTDIR)\KMemClass1.obj" \
	"$(INTDIR)\KMemManager.obj" \
	"$(INTDIR)\KMemStack.obj" \
	"$(INTDIR)\KMessage.obj" \
	"$(INTDIR)\KMouse.obj" \
	"$(INTDIR)\KMp3Music.obj" \
	"$(INTDIR)\KMp4Audio.obj" \
	"$(INTDIR)\KMp4Movie.obj" \
	"$(INTDIR)\KMp4Video.obj" \
	"$(INTDIR)\KMpgMusic.obj" \
	"$(INTDIR)\KMsgNode.obj" \
	"$(INTDIR)\KMusic.obj" \
	"$(INTDIR)\KMutex.obj" \
	"$(INTDIR)\KNode.obj" \
	"$(INTDIR)\KOctree.obj" \
	"$(INTDIR)\KOctreeNode.obj" \
	"$(INTDIR)\KPakData.obj" \
	"$(INTDIR)\KPakFile.obj" \
	"$(INTDIR)\KPakList.obj" \
	"$(INTDIR)\KPakTool.obj" \
	"$(INTDIR)\KPalette.obj" \
	"$(INTDIR)\KPcxFile.obj" \
	"$(INTDIR)\KPolygon.obj" \
	"$(INTDIR)\KPolyRelation.obj" \
	"$(INTDIR)\KRandom.obj" \
	"$(INTDIR)\KSafeList.obj" \
	"$(INTDIR)\KScanDir.obj" \
	"$(INTDIR)\KScript.obj" \
	"$(INTDIR)\KScriptCache.obj" \
	"$(INTDIR)\KScriptList.obj" \
	"$(INTDIR)\KScriptSet.obj" \
	"$(INTDIR)\KSG_MD5_String.obj" \
	"$(INTDIR)\KSG_StringProcess.obj" \
	"$(INTDIR)\KSortBinTree.obj" \
	"$(INTDIR)\KSortList.obj" \
	"$(INTDIR)\KSoundCache.obj" \
	"$(INTDIR)\KSprite.obj" \
	"$(INTDIR)\KSpriteCache.obj" \
	"$(INTDIR)\KSpriteCodec.obj" \
	"$(INTDIR)\KSpriteMaker.obj" \
	"$(INTDIR)\KStepLuaScript.obj" \
	"$(INTDIR)\KStrBase.obj" \
	"$(INTDIR)\KStrList.obj" \
	"$(INTDIR)\KStrNode.obj" \
	"$(INTDIR)\KTabFile.obj" \
	"$(INTDIR)\KTabFileCtrl.obj" \
	"$(INTDIR)\KTgaFile32.obj" \
	"$(INTDIR)\KThread.obj" \
	"$(INTDIR)\KTimer.obj" \
	"$(INTDIR)\KVideo.obj" \
	"$(INTDIR)\KWavCodec.obj" \
	"$(INTDIR)\KWavFile.obj" \
	"$(INTDIR)\KWavMusic.obj" \
	"$(INTDIR)\KWavSound.obj" \
	"$(INTDIR)\KWin32.obj" \
	"$(INTDIR)\KWin32App.obj" \
	"$(INTDIR)\KWin32Wnd.obj" \
	"$(INTDIR)\KZipCodec.obj" \
	"$(INTDIR)\KZipData.obj" \
	"$(INTDIR)\KZipFile.obj" \
	"$(INTDIR)\KZipList.obj" \
	"$(INTDIR)\md5.obj" \
	"$(INTDIR)\PassChecksum.obj" \
	"$(INTDIR)\stdafx.obj" \
	"$(INTDIR)\XPackFile.obj" \
	"$(INTDIR)\ZSPRPackFile.obj" \
	"$(INTDIR)\EDOneTimePad.obj" \
	"$(INTDIR)\Text.obj" \
	"$(INTDIR)\alloc.obj" \
	"$(INTDIR)\io.obj" \
	"$(INTDIR)\n2b_99.obj" \
	"$(INTDIR)\n2b_d.obj" \
	"$(INTDIR)\n2b_ds.obj" \
	"$(INTDIR)\n2b_to.obj" \
	"$(INTDIR)\n2d_99.obj" \
	"$(INTDIR)\n2d_d.obj" \
	"$(INTDIR)\n2d_ds.obj" \
	"$(INTDIR)\n2d_to.obj" \
	"$(INTDIR)\n2e_99.obj" \
	"$(INTDIR)\n2e_d.obj" \
	"$(INTDIR)\n2e_ds.obj" \
	"$(INTDIR)\n2e_to.obj" \
	"$(INTDIR)\ucl_crc.obj" \
	"$(INTDIR)\ucl_dll.obj" \
	"$(INTDIR)\ucl_init.obj" \
	"$(INTDIR)\ucl_ptr.obj" \
	"$(INTDIR)\ucl_str.obj" \
	"$(INTDIR)\ucl_util.obj" \
	"..\..\Lib\release\JpgLib.lib" \
	"..\..\Lib\release\mp3lib.lib" \
	"..\..\bin\server\lualibdll.lib"

"$(OUTDIR)\Engine.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# 开始自定义宏
OutDir=.\Release
# 结束自定义宏

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\Engine.dll" "$(OUTDIR)\Engine.pch"
   md ..\..\lib\release
	copy release\Engine.lib ..\..\lib\release\Engine.lib
	copy release\Engine.lib ..\..\bin\client\Engine.lib
	copy release\Engine.lib ..\..\bin\server\Engine.lib
	md ..\..\..\bin\client\release\
	copy release\Engine.dll ..\..\bin\client\Engine.dll
	copy release\Engine.dll ..\..\bin\client\release\Engine.dll
	md ..\..\..\bin\server\release\
	copy release\Engine.dll ..\..\bin\server\engine.dll
	copy release\Engine.dll ..\..\bin\server\release\Engine.dll
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# 开始自定义宏
OutDir=.\Debug
# 结束自定义宏

ALL : "$(OUTDIR)\Engine.dll" "$(OUTDIR)\Engine.bsc"


CLEAN :
	-@erase "$(INTDIR)\alloc.obj"
	-@erase "$(INTDIR)\alloc.sbr"
	-@erase "$(INTDIR)\EDOneTimePad.obj"
	-@erase "$(INTDIR)\EDOneTimePad.sbr"
	-@erase "$(INTDIR)\Engine.pch"
	-@erase "$(INTDIR)\io.obj"
	-@erase "$(INTDIR)\io.sbr"
	-@erase "$(INTDIR)\KAutoMutex.obj"
	-@erase "$(INTDIR)\KAutoMutex.sbr"
	-@erase "$(INTDIR)\KAviFile.obj"
	-@erase "$(INTDIR)\KAviFile.sbr"
	-@erase "$(INTDIR)\KBitmap.obj"
	-@erase "$(INTDIR)\KBitmap.sbr"
	-@erase "$(INTDIR)\KBitmap16.obj"
	-@erase "$(INTDIR)\KBitmap16.sbr"
	-@erase "$(INTDIR)\KBitmapConvert.obj"
	-@erase "$(INTDIR)\KBitmapConvert.sbr"
	-@erase "$(INTDIR)\KBmp2Spr.obj"
	-@erase "$(INTDIR)\KBmp2Spr.sbr"
	-@erase "$(INTDIR)\KBmpFile.obj"
	-@erase "$(INTDIR)\KBmpFile.sbr"
	-@erase "$(INTDIR)\KBmpFile24.obj"
	-@erase "$(INTDIR)\KBmpFile24.sbr"
	-@erase "$(INTDIR)\KCache.obj"
	-@erase "$(INTDIR)\KCache.sbr"
	-@erase "$(INTDIR)\KCanvas.obj"
	-@erase "$(INTDIR)\KCanvas.sbr"
	-@erase "$(INTDIR)\KCodec.obj"
	-@erase "$(INTDIR)\KCodec.sbr"
	-@erase "$(INTDIR)\KCodecLzo.obj"
	-@erase "$(INTDIR)\KCodecLzo.sbr"
	-@erase "$(INTDIR)\KColors.obj"
	-@erase "$(INTDIR)\KColors.sbr"
	-@erase "$(INTDIR)\KDDraw.obj"
	-@erase "$(INTDIR)\KDDraw.sbr"
	-@erase "$(INTDIR)\KDebug.obj"
	-@erase "$(INTDIR)\KDebug.sbr"
	-@erase "$(INTDIR)\KDError.obj"
	-@erase "$(INTDIR)\KDError.sbr"
	-@erase "$(INTDIR)\KDInput.obj"
	-@erase "$(INTDIR)\KDInput.sbr"
	-@erase "$(INTDIR)\KDrawBase.obj"
	-@erase "$(INTDIR)\KDrawBase.sbr"
	-@erase "$(INTDIR)\KDrawBitmap.obj"
	-@erase "$(INTDIR)\KDrawBitmap.sbr"
	-@erase "$(INTDIR)\KDrawBitmap16.obj"
	-@erase "$(INTDIR)\KDrawBitmap16.sbr"
	-@erase "$(INTDIR)\KDrawFade.obj"
	-@erase "$(INTDIR)\KDrawFade.sbr"
	-@erase "$(INTDIR)\KDrawFont.obj"
	-@erase "$(INTDIR)\KDrawFont.sbr"
	-@erase "$(INTDIR)\KDrawSprite.obj"
	-@erase "$(INTDIR)\KDrawSprite.sbr"
	-@erase "$(INTDIR)\KDrawSpriteAlpha.obj"
	-@erase "$(INTDIR)\KDrawSpriteAlpha.sbr"
	-@erase "$(INTDIR)\KDSound.obj"
	-@erase "$(INTDIR)\KDSound.sbr"
	-@erase "$(INTDIR)\KEicScript.obj"
	-@erase "$(INTDIR)\KEicScript.sbr"
	-@erase "$(INTDIR)\KEicScriptSet.obj"
	-@erase "$(INTDIR)\KEicScriptSet.sbr"
	-@erase "$(INTDIR)\KEngine.obj"
	-@erase "$(INTDIR)\KEngine.sbr"
	-@erase "$(INTDIR)\KEvent.obj"
	-@erase "$(INTDIR)\KEvent.sbr"
	-@erase "$(INTDIR)\KFile.obj"
	-@erase "$(INTDIR)\KFile.sbr"
	-@erase "$(INTDIR)\KFileCopy.obj"
	-@erase "$(INTDIR)\KFileCopy.sbr"
	-@erase "$(INTDIR)\KFileDialog.obj"
	-@erase "$(INTDIR)\KFileDialog.sbr"
	-@erase "$(INTDIR)\KFilePath.obj"
	-@erase "$(INTDIR)\KFilePath.sbr"
	-@erase "$(INTDIR)\KFindBinTree.obj"
	-@erase "$(INTDIR)\KFindBinTree.sbr"
	-@erase "$(INTDIR)\KFont.obj"
	-@erase "$(INTDIR)\KFont.sbr"
	-@erase "$(INTDIR)\KGifFile.obj"
	-@erase "$(INTDIR)\KGifFile.sbr"
	-@erase "$(INTDIR)\KGraphics.obj"
	-@erase "$(INTDIR)\KGraphics.sbr"
	-@erase "$(INTDIR)\KHashList.obj"
	-@erase "$(INTDIR)\KHashList.sbr"
	-@erase "$(INTDIR)\KHashNode.obj"
	-@erase "$(INTDIR)\KHashNode.sbr"
	-@erase "$(INTDIR)\KHashTable.obj"
	-@erase "$(INTDIR)\KHashTable.sbr"
	-@erase "$(INTDIR)\Kime.obj"
	-@erase "$(INTDIR)\Kime.sbr"
	-@erase "$(INTDIR)\KIniFile.obj"
	-@erase "$(INTDIR)\KIniFile.sbr"
	-@erase "$(INTDIR)\KJpgFile.obj"
	-@erase "$(INTDIR)\KJpgFile.sbr"
	-@erase "$(INTDIR)\KKeyboard.obj"
	-@erase "$(INTDIR)\KKeyboard.sbr"
	-@erase "$(INTDIR)\KLinkArray.obj"
	-@erase "$(INTDIR)\KLinkArray.sbr"
	-@erase "$(INTDIR)\KList.obj"
	-@erase "$(INTDIR)\KList.sbr"
	-@erase "$(INTDIR)\KLuaScript.obj"
	-@erase "$(INTDIR)\KLuaScript.sbr"
	-@erase "$(INTDIR)\KLuaScriptSet.obj"
	-@erase "$(INTDIR)\KLuaScriptSet.sbr"
	-@erase "$(INTDIR)\KLubCmpl_Blocker.obj"
	-@erase "$(INTDIR)\KLubCmpl_Blocker.sbr"
	-@erase "$(INTDIR)\KMemBase.obj"
	-@erase "$(INTDIR)\KMemBase.sbr"
	-@erase "$(INTDIR)\KMemClass.obj"
	-@erase "$(INTDIR)\KMemClass.sbr"
	-@erase "$(INTDIR)\KMemClass1.obj"
	-@erase "$(INTDIR)\KMemClass1.sbr"
	-@erase "$(INTDIR)\KMemManager.obj"
	-@erase "$(INTDIR)\KMemManager.sbr"
	-@erase "$(INTDIR)\KMemStack.obj"
	-@erase "$(INTDIR)\KMemStack.sbr"
	-@erase "$(INTDIR)\KMessage.obj"
	-@erase "$(INTDIR)\KMessage.sbr"
	-@erase "$(INTDIR)\KMouse.obj"
	-@erase "$(INTDIR)\KMouse.sbr"
	-@erase "$(INTDIR)\KMp3Music.obj"
	-@erase "$(INTDIR)\KMp3Music.sbr"
	-@erase "$(INTDIR)\KMp4Audio.obj"
	-@erase "$(INTDIR)\KMp4Audio.sbr"
	-@erase "$(INTDIR)\KMp4Movie.obj"
	-@erase "$(INTDIR)\KMp4Movie.sbr"
	-@erase "$(INTDIR)\KMp4Video.obj"
	-@erase "$(INTDIR)\KMp4Video.sbr"
	-@erase "$(INTDIR)\KMpgMusic.obj"
	-@erase "$(INTDIR)\KMpgMusic.sbr"
	-@erase "$(INTDIR)\KMsgNode.obj"
	-@erase "$(INTDIR)\KMsgNode.sbr"
	-@erase "$(INTDIR)\KMusic.obj"
	-@erase "$(INTDIR)\KMusic.sbr"
	-@erase "$(INTDIR)\KMutex.obj"
	-@erase "$(INTDIR)\KMutex.sbr"
	-@erase "$(INTDIR)\KNode.obj"
	-@erase "$(INTDIR)\KNode.sbr"
	-@erase "$(INTDIR)\KOctree.obj"
	-@erase "$(INTDIR)\KOctree.sbr"
	-@erase "$(INTDIR)\KOctreeNode.obj"
	-@erase "$(INTDIR)\KOctreeNode.sbr"
	-@erase "$(INTDIR)\KPakData.obj"
	-@erase "$(INTDIR)\KPakData.sbr"
	-@erase "$(INTDIR)\KPakFile.obj"
	-@erase "$(INTDIR)\KPakFile.sbr"
	-@erase "$(INTDIR)\KPakList.obj"
	-@erase "$(INTDIR)\KPakList.sbr"
	-@erase "$(INTDIR)\KPakTool.obj"
	-@erase "$(INTDIR)\KPakTool.sbr"
	-@erase "$(INTDIR)\KPalette.obj"
	-@erase "$(INTDIR)\KPalette.sbr"
	-@erase "$(INTDIR)\KPcxFile.obj"
	-@erase "$(INTDIR)\KPcxFile.sbr"
	-@erase "$(INTDIR)\KPolygon.obj"
	-@erase "$(INTDIR)\KPolygon.sbr"
	-@erase "$(INTDIR)\KPolyRelation.obj"
	-@erase "$(INTDIR)\KPolyRelation.sbr"
	-@erase "$(INTDIR)\KRandom.obj"
	-@erase "$(INTDIR)\KRandom.sbr"
	-@erase "$(INTDIR)\KSafeList.obj"
	-@erase "$(INTDIR)\KSafeList.sbr"
	-@erase "$(INTDIR)\KScanDir.obj"
	-@erase "$(INTDIR)\KScanDir.sbr"
	-@erase "$(INTDIR)\KScript.obj"
	-@erase "$(INTDIR)\KScript.sbr"
	-@erase "$(INTDIR)\KScriptCache.obj"
	-@erase "$(INTDIR)\KScriptCache.sbr"
	-@erase "$(INTDIR)\KScriptList.obj"
	-@erase "$(INTDIR)\KScriptList.sbr"
	-@erase "$(INTDIR)\KScriptSet.obj"
	-@erase "$(INTDIR)\KScriptSet.sbr"
	-@erase "$(INTDIR)\KSG_MD5_String.obj"
	-@erase "$(INTDIR)\KSG_MD5_String.sbr"
	-@erase "$(INTDIR)\KSG_StringProcess.obj"
	-@erase "$(INTDIR)\KSG_StringProcess.sbr"
	-@erase "$(INTDIR)\KSortBinTree.obj"
	-@erase "$(INTDIR)\KSortBinTree.sbr"
	-@erase "$(INTDIR)\KSortList.obj"
	-@erase "$(INTDIR)\KSortList.sbr"
	-@erase "$(INTDIR)\KSoundCache.obj"
	-@erase "$(INTDIR)\KSoundCache.sbr"
	-@erase "$(INTDIR)\KSprite.obj"
	-@erase "$(INTDIR)\KSprite.sbr"
	-@erase "$(INTDIR)\KSpriteCache.obj"
	-@erase "$(INTDIR)\KSpriteCache.sbr"
	-@erase "$(INTDIR)\KSpriteCodec.obj"
	-@erase "$(INTDIR)\KSpriteCodec.sbr"
	-@erase "$(INTDIR)\KSpriteMaker.obj"
	-@erase "$(INTDIR)\KSpriteMaker.sbr"
	-@erase "$(INTDIR)\KStepLuaScript.obj"
	-@erase "$(INTDIR)\KStepLuaScript.sbr"
	-@erase "$(INTDIR)\KStrBase.obj"
	-@erase "$(INTDIR)\KStrBase.sbr"
	-@erase "$(INTDIR)\KStrList.obj"
	-@erase "$(INTDIR)\KStrList.sbr"
	-@erase "$(INTDIR)\KStrNode.obj"
	-@erase "$(INTDIR)\KStrNode.sbr"
	-@erase "$(INTDIR)\KTabFile.obj"
	-@erase "$(INTDIR)\KTabFile.sbr"
	-@erase "$(INTDIR)\KTabFileCtrl.obj"
	-@erase "$(INTDIR)\KTabFileCtrl.sbr"
	-@erase "$(INTDIR)\KTgaFile32.obj"
	-@erase "$(INTDIR)\KTgaFile32.sbr"
	-@erase "$(INTDIR)\KThread.obj"
	-@erase "$(INTDIR)\KThread.sbr"
	-@erase "$(INTDIR)\KTimer.obj"
	-@erase "$(INTDIR)\KTimer.sbr"
	-@erase "$(INTDIR)\KVideo.obj"
	-@erase "$(INTDIR)\KVideo.sbr"
	-@erase "$(INTDIR)\KWavCodec.obj"
	-@erase "$(INTDIR)\KWavCodec.sbr"
	-@erase "$(INTDIR)\KWavFile.obj"
	-@erase "$(INTDIR)\KWavFile.sbr"
	-@erase "$(INTDIR)\KWavMusic.obj"
	-@erase "$(INTDIR)\KWavMusic.sbr"
	-@erase "$(INTDIR)\KWavSound.obj"
	-@erase "$(INTDIR)\KWavSound.sbr"
	-@erase "$(INTDIR)\KWin32.obj"
	-@erase "$(INTDIR)\KWin32.sbr"
	-@erase "$(INTDIR)\KWin32App.obj"
	-@erase "$(INTDIR)\KWin32App.sbr"
	-@erase "$(INTDIR)\KWin32Wnd.obj"
	-@erase "$(INTDIR)\KWin32Wnd.sbr"
	-@erase "$(INTDIR)\KZipCodec.obj"
	-@erase "$(INTDIR)\KZipCodec.sbr"
	-@erase "$(INTDIR)\KZipData.obj"
	-@erase "$(INTDIR)\KZipData.sbr"
	-@erase "$(INTDIR)\KZipFile.obj"
	-@erase "$(INTDIR)\KZipFile.sbr"
	-@erase "$(INTDIR)\KZipList.obj"
	-@erase "$(INTDIR)\KZipList.sbr"
	-@erase "$(INTDIR)\md5.obj"
	-@erase "$(INTDIR)\md5.sbr"
	-@erase "$(INTDIR)\n2b_99.obj"
	-@erase "$(INTDIR)\n2b_99.sbr"
	-@erase "$(INTDIR)\n2b_d.obj"
	-@erase "$(INTDIR)\n2b_d.sbr"
	-@erase "$(INTDIR)\n2b_ds.obj"
	-@erase "$(INTDIR)\n2b_ds.sbr"
	-@erase "$(INTDIR)\n2b_to.obj"
	-@erase "$(INTDIR)\n2b_to.sbr"
	-@erase "$(INTDIR)\n2d_99.obj"
	-@erase "$(INTDIR)\n2d_99.sbr"
	-@erase "$(INTDIR)\n2d_d.obj"
	-@erase "$(INTDIR)\n2d_d.sbr"
	-@erase "$(INTDIR)\n2d_ds.obj"
	-@erase "$(INTDIR)\n2d_ds.sbr"
	-@erase "$(INTDIR)\n2d_to.obj"
	-@erase "$(INTDIR)\n2d_to.sbr"
	-@erase "$(INTDIR)\n2e_99.obj"
	-@erase "$(INTDIR)\n2e_99.sbr"
	-@erase "$(INTDIR)\n2e_d.obj"
	-@erase "$(INTDIR)\n2e_d.sbr"
	-@erase "$(INTDIR)\n2e_ds.obj"
	-@erase "$(INTDIR)\n2e_ds.sbr"
	-@erase "$(INTDIR)\n2e_to.obj"
	-@erase "$(INTDIR)\n2e_to.sbr"
	-@erase "$(INTDIR)\PassChecksum.obj"
	-@erase "$(INTDIR)\PassChecksum.sbr"
	-@erase "$(INTDIR)\stdafx.obj"
	-@erase "$(INTDIR)\stdafx.sbr"
	-@erase "$(INTDIR)\Text.obj"
	-@erase "$(INTDIR)\Text.sbr"
	-@erase "$(INTDIR)\ucl_crc.obj"
	-@erase "$(INTDIR)\ucl_crc.sbr"
	-@erase "$(INTDIR)\ucl_dll.obj"
	-@erase "$(INTDIR)\ucl_dll.sbr"
	-@erase "$(INTDIR)\ucl_init.obj"
	-@erase "$(INTDIR)\ucl_init.sbr"
	-@erase "$(INTDIR)\ucl_ptr.obj"
	-@erase "$(INTDIR)\ucl_ptr.sbr"
	-@erase "$(INTDIR)\ucl_str.obj"
	-@erase "$(INTDIR)\ucl_str.sbr"
	-@erase "$(INTDIR)\ucl_util.obj"
	-@erase "$(INTDIR)\ucl_util.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\XPackFile.obj"
	-@erase "$(INTDIR)\XPackFile.sbr"
	-@erase "$(INTDIR)\ZSPRPackFile.obj"
	-@erase "$(INTDIR)\ZSPRPackFile.sbr"
	-@erase "$(OUTDIR)\Engine.bsc"
	-@erase "$(OUTDIR)\Engine.dll"
	-@erase "$(OUTDIR)\Engine.exp"
	-@erase "$(OUTDIR)\Engine.lib"
	-@erase "$(OUTDIR)\Engine.map"
	-@erase "$(OUTDIR)\Engine.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D "USE_STANDALONE_SPR" /D WINVER=0x0500 /FR"$(INTDIR)\\" /Fp"$(INTDIR)\Engine.pch" /Yu"KWin32.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Engine.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\KAutoMutex.sbr" \
	"$(INTDIR)\KAviFile.sbr" \
	"$(INTDIR)\KBitmap.sbr" \
	"$(INTDIR)\KBitmap16.sbr" \
	"$(INTDIR)\KBitmapConvert.sbr" \
	"$(INTDIR)\KBmp2Spr.sbr" \
	"$(INTDIR)\KBmpFile.sbr" \
	"$(INTDIR)\KBmpFile24.sbr" \
	"$(INTDIR)\KCache.sbr" \
	"$(INTDIR)\KCanvas.sbr" \
	"$(INTDIR)\KCodec.sbr" \
	"$(INTDIR)\KCodecLzo.sbr" \
	"$(INTDIR)\KColors.sbr" \
	"$(INTDIR)\KDDraw.sbr" \
	"$(INTDIR)\KDebug.sbr" \
	"$(INTDIR)\KDError.sbr" \
	"$(INTDIR)\KDInput.sbr" \
	"$(INTDIR)\KDrawBase.sbr" \
	"$(INTDIR)\KDrawBitmap.sbr" \
	"$(INTDIR)\KDrawBitmap16.sbr" \
	"$(INTDIR)\KDrawFade.sbr" \
	"$(INTDIR)\KDrawFont.sbr" \
	"$(INTDIR)\KDrawSprite.sbr" \
	"$(INTDIR)\KDrawSpriteAlpha.sbr" \
	"$(INTDIR)\KDSound.sbr" \
	"$(INTDIR)\KEicScript.sbr" \
	"$(INTDIR)\KEicScriptSet.sbr" \
	"$(INTDIR)\KEngine.sbr" \
	"$(INTDIR)\KEvent.sbr" \
	"$(INTDIR)\KFile.sbr" \
	"$(INTDIR)\KFileCopy.sbr" \
	"$(INTDIR)\KFileDialog.sbr" \
	"$(INTDIR)\KFilePath.sbr" \
	"$(INTDIR)\KFindBinTree.sbr" \
	"$(INTDIR)\KFont.sbr" \
	"$(INTDIR)\KGifFile.sbr" \
	"$(INTDIR)\KGraphics.sbr" \
	"$(INTDIR)\KHashList.sbr" \
	"$(INTDIR)\KHashNode.sbr" \
	"$(INTDIR)\KHashTable.sbr" \
	"$(INTDIR)\Kime.sbr" \
	"$(INTDIR)\KIniFile.sbr" \
	"$(INTDIR)\KJpgFile.sbr" \
	"$(INTDIR)\KKeyboard.sbr" \
	"$(INTDIR)\KLinkArray.sbr" \
	"$(INTDIR)\KList.sbr" \
	"$(INTDIR)\KLuaScript.sbr" \
	"$(INTDIR)\KLuaScriptSet.sbr" \
	"$(INTDIR)\KLubCmpl_Blocker.sbr" \
	"$(INTDIR)\KMemBase.sbr" \
	"$(INTDIR)\KMemClass.sbr" \
	"$(INTDIR)\KMemClass1.sbr" \
	"$(INTDIR)\KMemManager.sbr" \
	"$(INTDIR)\KMemStack.sbr" \
	"$(INTDIR)\KMessage.sbr" \
	"$(INTDIR)\KMouse.sbr" \
	"$(INTDIR)\KMp3Music.sbr" \
	"$(INTDIR)\KMp4Audio.sbr" \
	"$(INTDIR)\KMp4Movie.sbr" \
	"$(INTDIR)\KMp4Video.sbr" \
	"$(INTDIR)\KMpgMusic.sbr" \
	"$(INTDIR)\KMsgNode.sbr" \
	"$(INTDIR)\KMusic.sbr" \
	"$(INTDIR)\KMutex.sbr" \
	"$(INTDIR)\KNode.sbr" \
	"$(INTDIR)\KOctree.sbr" \
	"$(INTDIR)\KOctreeNode.sbr" \
	"$(INTDIR)\KPakData.sbr" \
	"$(INTDIR)\KPakFile.sbr" \
	"$(INTDIR)\KPakList.sbr" \
	"$(INTDIR)\KPakTool.sbr" \
	"$(INTDIR)\KPalette.sbr" \
	"$(INTDIR)\KPcxFile.sbr" \
	"$(INTDIR)\KPolygon.sbr" \
	"$(INTDIR)\KPolyRelation.sbr" \
	"$(INTDIR)\KRandom.sbr" \
	"$(INTDIR)\KSafeList.sbr" \
	"$(INTDIR)\KScanDir.sbr" \
	"$(INTDIR)\KScript.sbr" \
	"$(INTDIR)\KScriptCache.sbr" \
	"$(INTDIR)\KScriptList.sbr" \
	"$(INTDIR)\KScriptSet.sbr" \
	"$(INTDIR)\KSG_MD5_String.sbr" \
	"$(INTDIR)\KSG_StringProcess.sbr" \
	"$(INTDIR)\KSortBinTree.sbr" \
	"$(INTDIR)\KSortList.sbr" \
	"$(INTDIR)\KSoundCache.sbr" \
	"$(INTDIR)\KSprite.sbr" \
	"$(INTDIR)\KSpriteCache.sbr" \
	"$(INTDIR)\KSpriteCodec.sbr" \
	"$(INTDIR)\KSpriteMaker.sbr" \
	"$(INTDIR)\KStepLuaScript.sbr" \
	"$(INTDIR)\KStrBase.sbr" \
	"$(INTDIR)\KStrList.sbr" \
	"$(INTDIR)\KStrNode.sbr" \
	"$(INTDIR)\KTabFile.sbr" \
	"$(INTDIR)\KTabFileCtrl.sbr" \
	"$(INTDIR)\KTgaFile32.sbr" \
	"$(INTDIR)\KThread.sbr" \
	"$(INTDIR)\KTimer.sbr" \
	"$(INTDIR)\KVideo.sbr" \
	"$(INTDIR)\KWavCodec.sbr" \
	"$(INTDIR)\KWavFile.sbr" \
	"$(INTDIR)\KWavMusic.sbr" \
	"$(INTDIR)\KWavSound.sbr" \
	"$(INTDIR)\KWin32.sbr" \
	"$(INTDIR)\KWin32App.sbr" \
	"$(INTDIR)\KWin32Wnd.sbr" \
	"$(INTDIR)\KZipCodec.sbr" \
	"$(INTDIR)\KZipData.sbr" \
	"$(INTDIR)\KZipFile.sbr" \
	"$(INTDIR)\KZipList.sbr" \
	"$(INTDIR)\md5.sbr" \
	"$(INTDIR)\PassChecksum.sbr" \
	"$(INTDIR)\stdafx.sbr" \
	"$(INTDIR)\XPackFile.sbr" \
	"$(INTDIR)\ZSPRPackFile.sbr" \
	"$(INTDIR)\EDOneTimePad.sbr" \
	"$(INTDIR)\Text.sbr" \
	"$(INTDIR)\alloc.sbr" \
	"$(INTDIR)\io.sbr" \
	"$(INTDIR)\n2b_99.sbr" \
	"$(INTDIR)\n2b_d.sbr" \
	"$(INTDIR)\n2b_ds.sbr" \
	"$(INTDIR)\n2b_to.sbr" \
	"$(INTDIR)\n2d_99.sbr" \
	"$(INTDIR)\n2d_d.sbr" \
	"$(INTDIR)\n2d_ds.sbr" \
	"$(INTDIR)\n2d_to.sbr" \
	"$(INTDIR)\n2e_99.sbr" \
	"$(INTDIR)\n2e_d.sbr" \
	"$(INTDIR)\n2e_ds.sbr" \
	"$(INTDIR)\n2e_to.sbr" \
	"$(INTDIR)\ucl_crc.sbr" \
	"$(INTDIR)\ucl_dll.sbr" \
	"$(INTDIR)\ucl_init.sbr" \
	"$(INTDIR)\ucl_ptr.sbr" \
	"$(INTDIR)\ucl_str.sbr" \
	"$(INTDIR)\ucl_util.sbr"

"$(OUTDIR)\Engine.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib comdlg32.lib ddraw.lib dsound.lib dxguid.lib winmm.lib wsock32.lib dinput8.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\Engine.pdb" /map:"$(INTDIR)\Engine.map" /debug /machine:I386 /out:"$(OUTDIR)\Engine.dll" /implib:"$(OUTDIR)\Engine.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\KAutoMutex.obj" \
	"$(INTDIR)\KAviFile.obj" \
	"$(INTDIR)\KBitmap.obj" \
	"$(INTDIR)\KBitmap16.obj" \
	"$(INTDIR)\KBitmapConvert.obj" \
	"$(INTDIR)\KBmp2Spr.obj" \
	"$(INTDIR)\KBmpFile.obj" \
	"$(INTDIR)\KBmpFile24.obj" \
	"$(INTDIR)\KCache.obj" \
	"$(INTDIR)\KCanvas.obj" \
	"$(INTDIR)\KCodec.obj" \
	"$(INTDIR)\KCodecLzo.obj" \
	"$(INTDIR)\KColors.obj" \
	"$(INTDIR)\KDDraw.obj" \
	"$(INTDIR)\KDebug.obj" \
	"$(INTDIR)\KDError.obj" \
	"$(INTDIR)\KDInput.obj" \
	"$(INTDIR)\KDrawBase.obj" \
	"$(INTDIR)\KDrawBitmap.obj" \
	"$(INTDIR)\KDrawBitmap16.obj" \
	"$(INTDIR)\KDrawFade.obj" \
	"$(INTDIR)\KDrawFont.obj" \
	"$(INTDIR)\KDrawSprite.obj" \
	"$(INTDIR)\KDrawSpriteAlpha.obj" \
	"$(INTDIR)\KDSound.obj" \
	"$(INTDIR)\KEicScript.obj" \
	"$(INTDIR)\KEicScriptSet.obj" \
	"$(INTDIR)\KEngine.obj" \
	"$(INTDIR)\KEvent.obj" \
	"$(INTDIR)\KFile.obj" \
	"$(INTDIR)\KFileCopy.obj" \
	"$(INTDIR)\KFileDialog.obj" \
	"$(INTDIR)\KFilePath.obj" \
	"$(INTDIR)\KFindBinTree.obj" \
	"$(INTDIR)\KFont.obj" \
	"$(INTDIR)\KGifFile.obj" \
	"$(INTDIR)\KGraphics.obj" \
	"$(INTDIR)\KHashList.obj" \
	"$(INTDIR)\KHashNode.obj" \
	"$(INTDIR)\KHashTable.obj" \
	"$(INTDIR)\Kime.obj" \
	"$(INTDIR)\KIniFile.obj" \
	"$(INTDIR)\KJpgFile.obj" \
	"$(INTDIR)\KKeyboard.obj" \
	"$(INTDIR)\KLinkArray.obj" \
	"$(INTDIR)\KList.obj" \
	"$(INTDIR)\KLuaScript.obj" \
	"$(INTDIR)\KLuaScriptSet.obj" \
	"$(INTDIR)\KLubCmpl_Blocker.obj" \
	"$(INTDIR)\KMemBase.obj" \
	"$(INTDIR)\KMemClass.obj" \
	"$(INTDIR)\KMemClass1.obj" \
	"$(INTDIR)\KMemManager.obj" \
	"$(INTDIR)\KMemStack.obj" \
	"$(INTDIR)\KMessage.obj" \
	"$(INTDIR)\KMouse.obj" \
	"$(INTDIR)\KMp3Music.obj" \
	"$(INTDIR)\KMp4Audio.obj" \
	"$(INTDIR)\KMp4Movie.obj" \
	"$(INTDIR)\KMp4Video.obj" \
	"$(INTDIR)\KMpgMusic.obj" \
	"$(INTDIR)\KMsgNode.obj" \
	"$(INTDIR)\KMusic.obj" \
	"$(INTDIR)\KMutex.obj" \
	"$(INTDIR)\KNode.obj" \
	"$(INTDIR)\KOctree.obj" \
	"$(INTDIR)\KOctreeNode.obj" \
	"$(INTDIR)\KPakData.obj" \
	"$(INTDIR)\KPakFile.obj" \
	"$(INTDIR)\KPakList.obj" \
	"$(INTDIR)\KPakTool.obj" \
	"$(INTDIR)\KPalette.obj" \
	"$(INTDIR)\KPcxFile.obj" \
	"$(INTDIR)\KPolygon.obj" \
	"$(INTDIR)\KPolyRelation.obj" \
	"$(INTDIR)\KRandom.obj" \
	"$(INTDIR)\KSafeList.obj" \
	"$(INTDIR)\KScanDir.obj" \
	"$(INTDIR)\KScript.obj" \
	"$(INTDIR)\KScriptCache.obj" \
	"$(INTDIR)\KScriptList.obj" \
	"$(INTDIR)\KScriptSet.obj" \
	"$(INTDIR)\KSG_MD5_String.obj" \
	"$(INTDIR)\KSG_StringProcess.obj" \
	"$(INTDIR)\KSortBinTree.obj" \
	"$(INTDIR)\KSortList.obj" \
	"$(INTDIR)\KSoundCache.obj" \
	"$(INTDIR)\KSprite.obj" \
	"$(INTDIR)\KSpriteCache.obj" \
	"$(INTDIR)\KSpriteCodec.obj" \
	"$(INTDIR)\KSpriteMaker.obj" \
	"$(INTDIR)\KStepLuaScript.obj" \
	"$(INTDIR)\KStrBase.obj" \
	"$(INTDIR)\KStrList.obj" \
	"$(INTDIR)\KStrNode.obj" \
	"$(INTDIR)\KTabFile.obj" \
	"$(INTDIR)\KTabFileCtrl.obj" \
	"$(INTDIR)\KTgaFile32.obj" \
	"$(INTDIR)\KThread.obj" \
	"$(INTDIR)\KTimer.obj" \
	"$(INTDIR)\KVideo.obj" \
	"$(INTDIR)\KWavCodec.obj" \
	"$(INTDIR)\KWavFile.obj" \
	"$(INTDIR)\KWavMusic.obj" \
	"$(INTDIR)\KWavSound.obj" \
	"$(INTDIR)\KWin32.obj" \
	"$(INTDIR)\KWin32App.obj" \
	"$(INTDIR)\KWin32Wnd.obj" \
	"$(INTDIR)\KZipCodec.obj" \
	"$(INTDIR)\KZipData.obj" \
	"$(INTDIR)\KZipFile.obj" \
	"$(INTDIR)\KZipList.obj" \
	"$(INTDIR)\md5.obj" \
	"$(INTDIR)\PassChecksum.obj" \
	"$(INTDIR)\stdafx.obj" \
	"$(INTDIR)\XPackFile.obj" \
	"$(INTDIR)\ZSPRPackFile.obj" \
	"$(INTDIR)\EDOneTimePad.obj" \
	"$(INTDIR)\Text.obj" \
	"$(INTDIR)\alloc.obj" \
	"$(INTDIR)\io.obj" \
	"$(INTDIR)\n2b_99.obj" \
	"$(INTDIR)\n2b_d.obj" \
	"$(INTDIR)\n2b_ds.obj" \
	"$(INTDIR)\n2b_to.obj" \
	"$(INTDIR)\n2d_99.obj" \
	"$(INTDIR)\n2d_d.obj" \
	"$(INTDIR)\n2d_ds.obj" \
	"$(INTDIR)\n2d_to.obj" \
	"$(INTDIR)\n2e_99.obj" \
	"$(INTDIR)\n2e_d.obj" \
	"$(INTDIR)\n2e_ds.obj" \
	"$(INTDIR)\n2e_to.obj" \
	"$(INTDIR)\ucl_crc.obj" \
	"$(INTDIR)\ucl_dll.obj" \
	"$(INTDIR)\ucl_init.obj" \
	"$(INTDIR)\ucl_ptr.obj" \
	"$(INTDIR)\ucl_str.obj" \
	"$(INTDIR)\ucl_util.obj" \
	"..\..\Lib\debug\JpgLib.lib" \
	"..\..\Lib\debug\LuaLibDll.lib" \
	"..\..\Lib\debug\mp3lib.lib" \
	"..\..\bin\server\lualibdll.lib"

"$(OUTDIR)\Engine.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# 开始自定义宏
OutDir=.\Debug
# 结束自定义宏

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\Engine.dll" "$(OUTDIR)\Engine.bsc"
   md ..\..\lib\debug
	copy debug\engine.lib ..\..\lib\debug\engine.lib
	md ..\..\..\bin\client\debug\
	copy debug\engine.dll ..\..\..\bin\client\engine.dll
	copy debug\engine.dll ..\..\..\bin\client\debug\engine.dll
	md ..\..\..\bin\server\debug\
	copy debug\engine.dll ..\..\..\bin\server\engine.dll
	copy debug\engine.dll ..\..\..\bin\server\debug\engine.dll
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"

OUTDIR=.\Engine___Win32_OutRead_Debug
INTDIR=.\Engine___Win32_OutRead_Debug
# 开始自定义宏
OutDir=.\Engine___Win32_OutRead_Debug
# 结束自定义宏

ALL : "$(OUTDIR)\Engine.dll" "$(OUTDIR)\Engine.bsc"


CLEAN :
	-@erase "$(INTDIR)\alloc.obj"
	-@erase "$(INTDIR)\alloc.sbr"
	-@erase "$(INTDIR)\EDOneTimePad.obj"
	-@erase "$(INTDIR)\EDOneTimePad.sbr"
	-@erase "$(INTDIR)\Engine.pch"
	-@erase "$(INTDIR)\io.obj"
	-@erase "$(INTDIR)\io.sbr"
	-@erase "$(INTDIR)\KAutoMutex.obj"
	-@erase "$(INTDIR)\KAutoMutex.sbr"
	-@erase "$(INTDIR)\KAviFile.obj"
	-@erase "$(INTDIR)\KAviFile.sbr"
	-@erase "$(INTDIR)\KBitmap.obj"
	-@erase "$(INTDIR)\KBitmap.sbr"
	-@erase "$(INTDIR)\KBitmap16.obj"
	-@erase "$(INTDIR)\KBitmap16.sbr"
	-@erase "$(INTDIR)\KBitmapConvert.obj"
	-@erase "$(INTDIR)\KBitmapConvert.sbr"
	-@erase "$(INTDIR)\KBmp2Spr.obj"
	-@erase "$(INTDIR)\KBmp2Spr.sbr"
	-@erase "$(INTDIR)\KBmpFile.obj"
	-@erase "$(INTDIR)\KBmpFile.sbr"
	-@erase "$(INTDIR)\KBmpFile24.obj"
	-@erase "$(INTDIR)\KBmpFile24.sbr"
	-@erase "$(INTDIR)\KCache.obj"
	-@erase "$(INTDIR)\KCache.sbr"
	-@erase "$(INTDIR)\KCanvas.obj"
	-@erase "$(INTDIR)\KCanvas.sbr"
	-@erase "$(INTDIR)\KCodec.obj"
	-@erase "$(INTDIR)\KCodec.sbr"
	-@erase "$(INTDIR)\KCodecLzo.obj"
	-@erase "$(INTDIR)\KCodecLzo.sbr"
	-@erase "$(INTDIR)\KColors.obj"
	-@erase "$(INTDIR)\KColors.sbr"
	-@erase "$(INTDIR)\KDDraw.obj"
	-@erase "$(INTDIR)\KDDraw.sbr"
	-@erase "$(INTDIR)\KDebug.obj"
	-@erase "$(INTDIR)\KDebug.sbr"
	-@erase "$(INTDIR)\KDError.obj"
	-@erase "$(INTDIR)\KDError.sbr"
	-@erase "$(INTDIR)\KDInput.obj"
	-@erase "$(INTDIR)\KDInput.sbr"
	-@erase "$(INTDIR)\KDrawBase.obj"
	-@erase "$(INTDIR)\KDrawBase.sbr"
	-@erase "$(INTDIR)\KDrawBitmap.obj"
	-@erase "$(INTDIR)\KDrawBitmap.sbr"
	-@erase "$(INTDIR)\KDrawBitmap16.obj"
	-@erase "$(INTDIR)\KDrawBitmap16.sbr"
	-@erase "$(INTDIR)\KDrawFade.obj"
	-@erase "$(INTDIR)\KDrawFade.sbr"
	-@erase "$(INTDIR)\KDrawFont.obj"
	-@erase "$(INTDIR)\KDrawFont.sbr"
	-@erase "$(INTDIR)\KDrawSprite.obj"
	-@erase "$(INTDIR)\KDrawSprite.sbr"
	-@erase "$(INTDIR)\KDrawSpriteAlpha.obj"
	-@erase "$(INTDIR)\KDrawSpriteAlpha.sbr"
	-@erase "$(INTDIR)\KDSound.obj"
	-@erase "$(INTDIR)\KDSound.sbr"
	-@erase "$(INTDIR)\KEicScript.obj"
	-@erase "$(INTDIR)\KEicScript.sbr"
	-@erase "$(INTDIR)\KEicScriptSet.obj"
	-@erase "$(INTDIR)\KEicScriptSet.sbr"
	-@erase "$(INTDIR)\KEngine.obj"
	-@erase "$(INTDIR)\KEngine.sbr"
	-@erase "$(INTDIR)\KEvent.obj"
	-@erase "$(INTDIR)\KEvent.sbr"
	-@erase "$(INTDIR)\KFile.obj"
	-@erase "$(INTDIR)\KFile.sbr"
	-@erase "$(INTDIR)\KFileCopy.obj"
	-@erase "$(INTDIR)\KFileCopy.sbr"
	-@erase "$(INTDIR)\KFileDialog.obj"
	-@erase "$(INTDIR)\KFileDialog.sbr"
	-@erase "$(INTDIR)\KFilePath.obj"
	-@erase "$(INTDIR)\KFilePath.sbr"
	-@erase "$(INTDIR)\KFindBinTree.obj"
	-@erase "$(INTDIR)\KFindBinTree.sbr"
	-@erase "$(INTDIR)\KFont.obj"
	-@erase "$(INTDIR)\KFont.sbr"
	-@erase "$(INTDIR)\KGifFile.obj"
	-@erase "$(INTDIR)\KGifFile.sbr"
	-@erase "$(INTDIR)\KGraphics.obj"
	-@erase "$(INTDIR)\KGraphics.sbr"
	-@erase "$(INTDIR)\KHashList.obj"
	-@erase "$(INTDIR)\KHashList.sbr"
	-@erase "$(INTDIR)\KHashNode.obj"
	-@erase "$(INTDIR)\KHashNode.sbr"
	-@erase "$(INTDIR)\KHashTable.obj"
	-@erase "$(INTDIR)\KHashTable.sbr"
	-@erase "$(INTDIR)\Kime.obj"
	-@erase "$(INTDIR)\Kime.sbr"
	-@erase "$(INTDIR)\KIniFile.obj"
	-@erase "$(INTDIR)\KIniFile.sbr"
	-@erase "$(INTDIR)\KJpgFile.obj"
	-@erase "$(INTDIR)\KJpgFile.sbr"
	-@erase "$(INTDIR)\KKeyboard.obj"
	-@erase "$(INTDIR)\KKeyboard.sbr"
	-@erase "$(INTDIR)\KLinkArray.obj"
	-@erase "$(INTDIR)\KLinkArray.sbr"
	-@erase "$(INTDIR)\KList.obj"
	-@erase "$(INTDIR)\KList.sbr"
	-@erase "$(INTDIR)\KLuaScript.obj"
	-@erase "$(INTDIR)\KLuaScript.sbr"
	-@erase "$(INTDIR)\KLuaScriptSet.obj"
	-@erase "$(INTDIR)\KLuaScriptSet.sbr"
	-@erase "$(INTDIR)\KLubCmpl_Blocker.obj"
	-@erase "$(INTDIR)\KLubCmpl_Blocker.sbr"
	-@erase "$(INTDIR)\KMemBase.obj"
	-@erase "$(INTDIR)\KMemBase.sbr"
	-@erase "$(INTDIR)\KMemClass.obj"
	-@erase "$(INTDIR)\KMemClass.sbr"
	-@erase "$(INTDIR)\KMemClass1.obj"
	-@erase "$(INTDIR)\KMemClass1.sbr"
	-@erase "$(INTDIR)\KMemManager.obj"
	-@erase "$(INTDIR)\KMemManager.sbr"
	-@erase "$(INTDIR)\KMemStack.obj"
	-@erase "$(INTDIR)\KMemStack.sbr"
	-@erase "$(INTDIR)\KMessage.obj"
	-@erase "$(INTDIR)\KMessage.sbr"
	-@erase "$(INTDIR)\KMouse.obj"
	-@erase "$(INTDIR)\KMouse.sbr"
	-@erase "$(INTDIR)\KMp3Music.obj"
	-@erase "$(INTDIR)\KMp3Music.sbr"
	-@erase "$(INTDIR)\KMp4Audio.obj"
	-@erase "$(INTDIR)\KMp4Audio.sbr"
	-@erase "$(INTDIR)\KMp4Movie.obj"
	-@erase "$(INTDIR)\KMp4Movie.sbr"
	-@erase "$(INTDIR)\KMp4Video.obj"
	-@erase "$(INTDIR)\KMp4Video.sbr"
	-@erase "$(INTDIR)\KMpgMusic.obj"
	-@erase "$(INTDIR)\KMpgMusic.sbr"
	-@erase "$(INTDIR)\KMsgNode.obj"
	-@erase "$(INTDIR)\KMsgNode.sbr"
	-@erase "$(INTDIR)\KMusic.obj"
	-@erase "$(INTDIR)\KMusic.sbr"
	-@erase "$(INTDIR)\KMutex.obj"
	-@erase "$(INTDIR)\KMutex.sbr"
	-@erase "$(INTDIR)\KNode.obj"
	-@erase "$(INTDIR)\KNode.sbr"
	-@erase "$(INTDIR)\KOctree.obj"
	-@erase "$(INTDIR)\KOctree.sbr"
	-@erase "$(INTDIR)\KOctreeNode.obj"
	-@erase "$(INTDIR)\KOctreeNode.sbr"
	-@erase "$(INTDIR)\KPakData.obj"
	-@erase "$(INTDIR)\KPakData.sbr"
	-@erase "$(INTDIR)\KPakFile.obj"
	-@erase "$(INTDIR)\KPakFile.sbr"
	-@erase "$(INTDIR)\KPakList.obj"
	-@erase "$(INTDIR)\KPakList.sbr"
	-@erase "$(INTDIR)\KPakTool.obj"
	-@erase "$(INTDIR)\KPakTool.sbr"
	-@erase "$(INTDIR)\KPalette.obj"
	-@erase "$(INTDIR)\KPalette.sbr"
	-@erase "$(INTDIR)\KPcxFile.obj"
	-@erase "$(INTDIR)\KPcxFile.sbr"
	-@erase "$(INTDIR)\KPolygon.obj"
	-@erase "$(INTDIR)\KPolygon.sbr"
	-@erase "$(INTDIR)\KPolyRelation.obj"
	-@erase "$(INTDIR)\KPolyRelation.sbr"
	-@erase "$(INTDIR)\KRandom.obj"
	-@erase "$(INTDIR)\KRandom.sbr"
	-@erase "$(INTDIR)\KSafeList.obj"
	-@erase "$(INTDIR)\KSafeList.sbr"
	-@erase "$(INTDIR)\KScanDir.obj"
	-@erase "$(INTDIR)\KScanDir.sbr"
	-@erase "$(INTDIR)\KScript.obj"
	-@erase "$(INTDIR)\KScript.sbr"
	-@erase "$(INTDIR)\KScriptCache.obj"
	-@erase "$(INTDIR)\KScriptCache.sbr"
	-@erase "$(INTDIR)\KScriptList.obj"
	-@erase "$(INTDIR)\KScriptList.sbr"
	-@erase "$(INTDIR)\KScriptSet.obj"
	-@erase "$(INTDIR)\KScriptSet.sbr"
	-@erase "$(INTDIR)\KSG_MD5_String.obj"
	-@erase "$(INTDIR)\KSG_MD5_String.sbr"
	-@erase "$(INTDIR)\KSG_StringProcess.obj"
	-@erase "$(INTDIR)\KSG_StringProcess.sbr"
	-@erase "$(INTDIR)\KSortBinTree.obj"
	-@erase "$(INTDIR)\KSortBinTree.sbr"
	-@erase "$(INTDIR)\KSortList.obj"
	-@erase "$(INTDIR)\KSortList.sbr"
	-@erase "$(INTDIR)\KSoundCache.obj"
	-@erase "$(INTDIR)\KSoundCache.sbr"
	-@erase "$(INTDIR)\KSprite.obj"
	-@erase "$(INTDIR)\KSprite.sbr"
	-@erase "$(INTDIR)\KSpriteCache.obj"
	-@erase "$(INTDIR)\KSpriteCache.sbr"
	-@erase "$(INTDIR)\KSpriteCodec.obj"
	-@erase "$(INTDIR)\KSpriteCodec.sbr"
	-@erase "$(INTDIR)\KSpriteMaker.obj"
	-@erase "$(INTDIR)\KSpriteMaker.sbr"
	-@erase "$(INTDIR)\KStepLuaScript.obj"
	-@erase "$(INTDIR)\KStepLuaScript.sbr"
	-@erase "$(INTDIR)\KStrBase.obj"
	-@erase "$(INTDIR)\KStrBase.sbr"
	-@erase "$(INTDIR)\KStrList.obj"
	-@erase "$(INTDIR)\KStrList.sbr"
	-@erase "$(INTDIR)\KStrNode.obj"
	-@erase "$(INTDIR)\KStrNode.sbr"
	-@erase "$(INTDIR)\KTabFile.obj"
	-@erase "$(INTDIR)\KTabFile.sbr"
	-@erase "$(INTDIR)\KTabFileCtrl.obj"
	-@erase "$(INTDIR)\KTabFileCtrl.sbr"
	-@erase "$(INTDIR)\KTgaFile32.obj"
	-@erase "$(INTDIR)\KTgaFile32.sbr"
	-@erase "$(INTDIR)\KThread.obj"
	-@erase "$(INTDIR)\KThread.sbr"
	-@erase "$(INTDIR)\KTimer.obj"
	-@erase "$(INTDIR)\KTimer.sbr"
	-@erase "$(INTDIR)\KVideo.obj"
	-@erase "$(INTDIR)\KVideo.sbr"
	-@erase "$(INTDIR)\KWavCodec.obj"
	-@erase "$(INTDIR)\KWavCodec.sbr"
	-@erase "$(INTDIR)\KWavFile.obj"
	-@erase "$(INTDIR)\KWavFile.sbr"
	-@erase "$(INTDIR)\KWavMusic.obj"
	-@erase "$(INTDIR)\KWavMusic.sbr"
	-@erase "$(INTDIR)\KWavSound.obj"
	-@erase "$(INTDIR)\KWavSound.sbr"
	-@erase "$(INTDIR)\KWin32.obj"
	-@erase "$(INTDIR)\KWin32.sbr"
	-@erase "$(INTDIR)\KWin32App.obj"
	-@erase "$(INTDIR)\KWin32App.sbr"
	-@erase "$(INTDIR)\KWin32Wnd.obj"
	-@erase "$(INTDIR)\KWin32Wnd.sbr"
	-@erase "$(INTDIR)\KZipCodec.obj"
	-@erase "$(INTDIR)\KZipCodec.sbr"
	-@erase "$(INTDIR)\KZipData.obj"
	-@erase "$(INTDIR)\KZipData.sbr"
	-@erase "$(INTDIR)\KZipFile.obj"
	-@erase "$(INTDIR)\KZipFile.sbr"
	-@erase "$(INTDIR)\KZipList.obj"
	-@erase "$(INTDIR)\KZipList.sbr"
	-@erase "$(INTDIR)\md5.obj"
	-@erase "$(INTDIR)\md5.sbr"
	-@erase "$(INTDIR)\n2b_99.obj"
	-@erase "$(INTDIR)\n2b_99.sbr"
	-@erase "$(INTDIR)\n2b_d.obj"
	-@erase "$(INTDIR)\n2b_d.sbr"
	-@erase "$(INTDIR)\n2b_ds.obj"
	-@erase "$(INTDIR)\n2b_ds.sbr"
	-@erase "$(INTDIR)\n2b_to.obj"
	-@erase "$(INTDIR)\n2b_to.sbr"
	-@erase "$(INTDIR)\n2d_99.obj"
	-@erase "$(INTDIR)\n2d_99.sbr"
	-@erase "$(INTDIR)\n2d_d.obj"
	-@erase "$(INTDIR)\n2d_d.sbr"
	-@erase "$(INTDIR)\n2d_ds.obj"
	-@erase "$(INTDIR)\n2d_ds.sbr"
	-@erase "$(INTDIR)\n2d_to.obj"
	-@erase "$(INTDIR)\n2d_to.sbr"
	-@erase "$(INTDIR)\n2e_99.obj"
	-@erase "$(INTDIR)\n2e_99.sbr"
	-@erase "$(INTDIR)\n2e_d.obj"
	-@erase "$(INTDIR)\n2e_d.sbr"
	-@erase "$(INTDIR)\n2e_ds.obj"
	-@erase "$(INTDIR)\n2e_ds.sbr"
	-@erase "$(INTDIR)\n2e_to.obj"
	-@erase "$(INTDIR)\n2e_to.sbr"
	-@erase "$(INTDIR)\PassChecksum.obj"
	-@erase "$(INTDIR)\PassChecksum.sbr"
	-@erase "$(INTDIR)\stdafx.obj"
	-@erase "$(INTDIR)\stdafx.sbr"
	-@erase "$(INTDIR)\Text.obj"
	-@erase "$(INTDIR)\Text.sbr"
	-@erase "$(INTDIR)\ucl_crc.obj"
	-@erase "$(INTDIR)\ucl_crc.sbr"
	-@erase "$(INTDIR)\ucl_dll.obj"
	-@erase "$(INTDIR)\ucl_dll.sbr"
	-@erase "$(INTDIR)\ucl_init.obj"
	-@erase "$(INTDIR)\ucl_init.sbr"
	-@erase "$(INTDIR)\ucl_ptr.obj"
	-@erase "$(INTDIR)\ucl_ptr.sbr"
	-@erase "$(INTDIR)\ucl_str.obj"
	-@erase "$(INTDIR)\ucl_str.sbr"
	-@erase "$(INTDIR)\ucl_util.obj"
	-@erase "$(INTDIR)\ucl_util.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\XPackFile.obj"
	-@erase "$(INTDIR)\XPackFile.sbr"
	-@erase "$(INTDIR)\ZSPRPackFile.obj"
	-@erase "$(INTDIR)\ZSPRPackFile.sbr"
	-@erase "$(OUTDIR)\Engine.bsc"
	-@erase "$(OUTDIR)\Engine.dll"
	-@erase "$(OUTDIR)\Engine.exp"
	-@erase "$(OUTDIR)\Engine.lib"
	-@erase "$(OUTDIR)\Engine.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D "USE_STANDALONE_SPR" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\Engine.pch" /Yu"KWin32.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Engine.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\KAutoMutex.sbr" \
	"$(INTDIR)\KAviFile.sbr" \
	"$(INTDIR)\KBitmap.sbr" \
	"$(INTDIR)\KBitmap16.sbr" \
	"$(INTDIR)\KBitmapConvert.sbr" \
	"$(INTDIR)\KBmp2Spr.sbr" \
	"$(INTDIR)\KBmpFile.sbr" \
	"$(INTDIR)\KBmpFile24.sbr" \
	"$(INTDIR)\KCache.sbr" \
	"$(INTDIR)\KCanvas.sbr" \
	"$(INTDIR)\KCodec.sbr" \
	"$(INTDIR)\KCodecLzo.sbr" \
	"$(INTDIR)\KColors.sbr" \
	"$(INTDIR)\KDDraw.sbr" \
	"$(INTDIR)\KDebug.sbr" \
	"$(INTDIR)\KDError.sbr" \
	"$(INTDIR)\KDInput.sbr" \
	"$(INTDIR)\KDrawBase.sbr" \
	"$(INTDIR)\KDrawBitmap.sbr" \
	"$(INTDIR)\KDrawBitmap16.sbr" \
	"$(INTDIR)\KDrawFade.sbr" \
	"$(INTDIR)\KDrawFont.sbr" \
	"$(INTDIR)\KDrawSprite.sbr" \
	"$(INTDIR)\KDrawSpriteAlpha.sbr" \
	"$(INTDIR)\KDSound.sbr" \
	"$(INTDIR)\KEicScript.sbr" \
	"$(INTDIR)\KEicScriptSet.sbr" \
	"$(INTDIR)\KEngine.sbr" \
	"$(INTDIR)\KEvent.sbr" \
	"$(INTDIR)\KFile.sbr" \
	"$(INTDIR)\KFileCopy.sbr" \
	"$(INTDIR)\KFileDialog.sbr" \
	"$(INTDIR)\KFilePath.sbr" \
	"$(INTDIR)\KFindBinTree.sbr" \
	"$(INTDIR)\KFont.sbr" \
	"$(INTDIR)\KGifFile.sbr" \
	"$(INTDIR)\KGraphics.sbr" \
	"$(INTDIR)\KHashList.sbr" \
	"$(INTDIR)\KHashNode.sbr" \
	"$(INTDIR)\KHashTable.sbr" \
	"$(INTDIR)\Kime.sbr" \
	"$(INTDIR)\KIniFile.sbr" \
	"$(INTDIR)\KJpgFile.sbr" \
	"$(INTDIR)\KKeyboard.sbr" \
	"$(INTDIR)\KLinkArray.sbr" \
	"$(INTDIR)\KList.sbr" \
	"$(INTDIR)\KLuaScript.sbr" \
	"$(INTDIR)\KLuaScriptSet.sbr" \
	"$(INTDIR)\KLubCmpl_Blocker.sbr" \
	"$(INTDIR)\KMemBase.sbr" \
	"$(INTDIR)\KMemClass.sbr" \
	"$(INTDIR)\KMemClass1.sbr" \
	"$(INTDIR)\KMemManager.sbr" \
	"$(INTDIR)\KMemStack.sbr" \
	"$(INTDIR)\KMessage.sbr" \
	"$(INTDIR)\KMouse.sbr" \
	"$(INTDIR)\KMp3Music.sbr" \
	"$(INTDIR)\KMp4Audio.sbr" \
	"$(INTDIR)\KMp4Movie.sbr" \
	"$(INTDIR)\KMp4Video.sbr" \
	"$(INTDIR)\KMpgMusic.sbr" \
	"$(INTDIR)\KMsgNode.sbr" \
	"$(INTDIR)\KMusic.sbr" \
	"$(INTDIR)\KMutex.sbr" \
	"$(INTDIR)\KNode.sbr" \
	"$(INTDIR)\KOctree.sbr" \
	"$(INTDIR)\KOctreeNode.sbr" \
	"$(INTDIR)\KPakData.sbr" \
	"$(INTDIR)\KPakFile.sbr" \
	"$(INTDIR)\KPakList.sbr" \
	"$(INTDIR)\KPakTool.sbr" \
	"$(INTDIR)\KPalette.sbr" \
	"$(INTDIR)\KPcxFile.sbr" \
	"$(INTDIR)\KPolygon.sbr" \
	"$(INTDIR)\KPolyRelation.sbr" \
	"$(INTDIR)\KRandom.sbr" \
	"$(INTDIR)\KSafeList.sbr" \
	"$(INTDIR)\KScanDir.sbr" \
	"$(INTDIR)\KScript.sbr" \
	"$(INTDIR)\KScriptCache.sbr" \
	"$(INTDIR)\KScriptList.sbr" \
	"$(INTDIR)\KScriptSet.sbr" \
	"$(INTDIR)\KSG_MD5_String.sbr" \
	"$(INTDIR)\KSG_StringProcess.sbr" \
	"$(INTDIR)\KSortBinTree.sbr" \
	"$(INTDIR)\KSortList.sbr" \
	"$(INTDIR)\KSoundCache.sbr" \
	"$(INTDIR)\KSprite.sbr" \
	"$(INTDIR)\KSpriteCache.sbr" \
	"$(INTDIR)\KSpriteCodec.sbr" \
	"$(INTDIR)\KSpriteMaker.sbr" \
	"$(INTDIR)\KStepLuaScript.sbr" \
	"$(INTDIR)\KStrBase.sbr" \
	"$(INTDIR)\KStrList.sbr" \
	"$(INTDIR)\KStrNode.sbr" \
	"$(INTDIR)\KTabFile.sbr" \
	"$(INTDIR)\KTabFileCtrl.sbr" \
	"$(INTDIR)\KTgaFile32.sbr" \
	"$(INTDIR)\KThread.sbr" \
	"$(INTDIR)\KTimer.sbr" \
	"$(INTDIR)\KVideo.sbr" \
	"$(INTDIR)\KWavCodec.sbr" \
	"$(INTDIR)\KWavFile.sbr" \
	"$(INTDIR)\KWavMusic.sbr" \
	"$(INTDIR)\KWavSound.sbr" \
	"$(INTDIR)\KWin32.sbr" \
	"$(INTDIR)\KWin32App.sbr" \
	"$(INTDIR)\KWin32Wnd.sbr" \
	"$(INTDIR)\KZipCodec.sbr" \
	"$(INTDIR)\KZipData.sbr" \
	"$(INTDIR)\KZipFile.sbr" \
	"$(INTDIR)\KZipList.sbr" \
	"$(INTDIR)\md5.sbr" \
	"$(INTDIR)\PassChecksum.sbr" \
	"$(INTDIR)\stdafx.sbr" \
	"$(INTDIR)\XPackFile.sbr" \
	"$(INTDIR)\ZSPRPackFile.sbr" \
	"$(INTDIR)\EDOneTimePad.sbr" \
	"$(INTDIR)\Text.sbr" \
	"$(INTDIR)\alloc.sbr" \
	"$(INTDIR)\io.sbr" \
	"$(INTDIR)\n2b_99.sbr" \
	"$(INTDIR)\n2b_d.sbr" \
	"$(INTDIR)\n2b_ds.sbr" \
	"$(INTDIR)\n2b_to.sbr" \
	"$(INTDIR)\n2d_99.sbr" \
	"$(INTDIR)\n2d_d.sbr" \
	"$(INTDIR)\n2d_ds.sbr" \
	"$(INTDIR)\n2d_to.sbr" \
	"$(INTDIR)\n2e_99.sbr" \
	"$(INTDIR)\n2e_d.sbr" \
	"$(INTDIR)\n2e_ds.sbr" \
	"$(INTDIR)\n2e_to.sbr" \
	"$(INTDIR)\ucl_crc.sbr" \
	"$(INTDIR)\ucl_dll.sbr" \
	"$(INTDIR)\ucl_init.sbr" \
	"$(INTDIR)\ucl_ptr.sbr" \
	"$(INTDIR)\ucl_str.sbr" \
	"$(INTDIR)\ucl_util.sbr"

"$(OUTDIR)\Engine.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib comdlg32.lib ddraw.lib dsound.lib dxguid.lib winmm.lib wsock32.lib dinput8.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\Engine.pdb" /debug /machine:I386 /out:"$(OUTDIR)\Engine.dll" /implib:"$(OUTDIR)\Engine.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\KAutoMutex.obj" \
	"$(INTDIR)\KAviFile.obj" \
	"$(INTDIR)\KBitmap.obj" \
	"$(INTDIR)\KBitmap16.obj" \
	"$(INTDIR)\KBitmapConvert.obj" \
	"$(INTDIR)\KBmp2Spr.obj" \
	"$(INTDIR)\KBmpFile.obj" \
	"$(INTDIR)\KBmpFile24.obj" \
	"$(INTDIR)\KCache.obj" \
	"$(INTDIR)\KCanvas.obj" \
	"$(INTDIR)\KCodec.obj" \
	"$(INTDIR)\KCodecLzo.obj" \
	"$(INTDIR)\KColors.obj" \
	"$(INTDIR)\KDDraw.obj" \
	"$(INTDIR)\KDebug.obj" \
	"$(INTDIR)\KDError.obj" \
	"$(INTDIR)\KDInput.obj" \
	"$(INTDIR)\KDrawBase.obj" \
	"$(INTDIR)\KDrawBitmap.obj" \
	"$(INTDIR)\KDrawBitmap16.obj" \
	"$(INTDIR)\KDrawFade.obj" \
	"$(INTDIR)\KDrawFont.obj" \
	"$(INTDIR)\KDrawSprite.obj" \
	"$(INTDIR)\KDrawSpriteAlpha.obj" \
	"$(INTDIR)\KDSound.obj" \
	"$(INTDIR)\KEicScript.obj" \
	"$(INTDIR)\KEicScriptSet.obj" \
	"$(INTDIR)\KEngine.obj" \
	"$(INTDIR)\KEvent.obj" \
	"$(INTDIR)\KFile.obj" \
	"$(INTDIR)\KFileCopy.obj" \
	"$(INTDIR)\KFileDialog.obj" \
	"$(INTDIR)\KFilePath.obj" \
	"$(INTDIR)\KFindBinTree.obj" \
	"$(INTDIR)\KFont.obj" \
	"$(INTDIR)\KGifFile.obj" \
	"$(INTDIR)\KGraphics.obj" \
	"$(INTDIR)\KHashList.obj" \
	"$(INTDIR)\KHashNode.obj" \
	"$(INTDIR)\KHashTable.obj" \
	"$(INTDIR)\Kime.obj" \
	"$(INTDIR)\KIniFile.obj" \
	"$(INTDIR)\KJpgFile.obj" \
	"$(INTDIR)\KKeyboard.obj" \
	"$(INTDIR)\KLinkArray.obj" \
	"$(INTDIR)\KList.obj" \
	"$(INTDIR)\KLuaScript.obj" \
	"$(INTDIR)\KLuaScriptSet.obj" \
	"$(INTDIR)\KLubCmpl_Blocker.obj" \
	"$(INTDIR)\KMemBase.obj" \
	"$(INTDIR)\KMemClass.obj" \
	"$(INTDIR)\KMemClass1.obj" \
	"$(INTDIR)\KMemManager.obj" \
	"$(INTDIR)\KMemStack.obj" \
	"$(INTDIR)\KMessage.obj" \
	"$(INTDIR)\KMouse.obj" \
	"$(INTDIR)\KMp3Music.obj" \
	"$(INTDIR)\KMp4Audio.obj" \
	"$(INTDIR)\KMp4Movie.obj" \
	"$(INTDIR)\KMp4Video.obj" \
	"$(INTDIR)\KMpgMusic.obj" \
	"$(INTDIR)\KMsgNode.obj" \
	"$(INTDIR)\KMusic.obj" \
	"$(INTDIR)\KMutex.obj" \
	"$(INTDIR)\KNode.obj" \
	"$(INTDIR)\KOctree.obj" \
	"$(INTDIR)\KOctreeNode.obj" \
	"$(INTDIR)\KPakData.obj" \
	"$(INTDIR)\KPakFile.obj" \
	"$(INTDIR)\KPakList.obj" \
	"$(INTDIR)\KPakTool.obj" \
	"$(INTDIR)\KPalette.obj" \
	"$(INTDIR)\KPcxFile.obj" \
	"$(INTDIR)\KPolygon.obj" \
	"$(INTDIR)\KPolyRelation.obj" \
	"$(INTDIR)\KRandom.obj" \
	"$(INTDIR)\KSafeList.obj" \
	"$(INTDIR)\KScanDir.obj" \
	"$(INTDIR)\KScript.obj" \
	"$(INTDIR)\KScriptCache.obj" \
	"$(INTDIR)\KScriptList.obj" \
	"$(INTDIR)\KScriptSet.obj" \
	"$(INTDIR)\KSG_MD5_String.obj" \
	"$(INTDIR)\KSG_StringProcess.obj" \
	"$(INTDIR)\KSortBinTree.obj" \
	"$(INTDIR)\KSortList.obj" \
	"$(INTDIR)\KSoundCache.obj" \
	"$(INTDIR)\KSprite.obj" \
	"$(INTDIR)\KSpriteCache.obj" \
	"$(INTDIR)\KSpriteCodec.obj" \
	"$(INTDIR)\KSpriteMaker.obj" \
	"$(INTDIR)\KStepLuaScript.obj" \
	"$(INTDIR)\KStrBase.obj" \
	"$(INTDIR)\KStrList.obj" \
	"$(INTDIR)\KStrNode.obj" \
	"$(INTDIR)\KTabFile.obj" \
	"$(INTDIR)\KTabFileCtrl.obj" \
	"$(INTDIR)\KTgaFile32.obj" \
	"$(INTDIR)\KThread.obj" \
	"$(INTDIR)\KTimer.obj" \
	"$(INTDIR)\KVideo.obj" \
	"$(INTDIR)\KWavCodec.obj" \
	"$(INTDIR)\KWavFile.obj" \
	"$(INTDIR)\KWavMusic.obj" \
	"$(INTDIR)\KWavSound.obj" \
	"$(INTDIR)\KWin32.obj" \
	"$(INTDIR)\KWin32App.obj" \
	"$(INTDIR)\KWin32Wnd.obj" \
	"$(INTDIR)\KZipCodec.obj" \
	"$(INTDIR)\KZipData.obj" \
	"$(INTDIR)\KZipFile.obj" \
	"$(INTDIR)\KZipList.obj" \
	"$(INTDIR)\md5.obj" \
	"$(INTDIR)\PassChecksum.obj" \
	"$(INTDIR)\stdafx.obj" \
	"$(INTDIR)\XPackFile.obj" \
	"$(INTDIR)\ZSPRPackFile.obj" \
	"$(INTDIR)\EDOneTimePad.obj" \
	"$(INTDIR)\Text.obj" \
	"$(INTDIR)\alloc.obj" \
	"$(INTDIR)\io.obj" \
	"$(INTDIR)\n2b_99.obj" \
	"$(INTDIR)\n2b_d.obj" \
	"$(INTDIR)\n2b_ds.obj" \
	"$(INTDIR)\n2b_to.obj" \
	"$(INTDIR)\n2d_99.obj" \
	"$(INTDIR)\n2d_d.obj" \
	"$(INTDIR)\n2d_ds.obj" \
	"$(INTDIR)\n2d_to.obj" \
	"$(INTDIR)\n2e_99.obj" \
	"$(INTDIR)\n2e_d.obj" \
	"$(INTDIR)\n2e_ds.obj" \
	"$(INTDIR)\n2e_to.obj" \
	"$(INTDIR)\ucl_crc.obj" \
	"$(INTDIR)\ucl_dll.obj" \
	"$(INTDIR)\ucl_init.obj" \
	"$(INTDIR)\ucl_ptr.obj" \
	"$(INTDIR)\ucl_str.obj" \
	"$(INTDIR)\ucl_util.obj" \
	"..\..\Lib\debug\JpgLib.lib" \
	"..\..\Lib\debug\LuaLibDll.lib" \
	"..\..\Lib\debug\mp3lib.lib" \
	"..\..\bin\server\lualibdll.lib"

"$(OUTDIR)\Engine.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# 开始自定义宏
OutDir=.\Engine___Win32_OutRead_Debug
# 结束自定义宏

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\Engine.dll" "$(OUTDIR)\Engine.bsc"
   md ..\..\lib\debug
	copy Engine___Win32_OutRead_Debug\Engine.lib ..\..\lib\debug\engine.lib
	md ..\..\..\bin\client\debug\
	copy Engine___Win32_OutRead_Debug\Engine.dll ..\..\..\bin\client\engine.dll
	copy Engine___Win32_OutRead_Debug\Engine.dll ..\..\..\bin\client\debug\engine.dll
	md ..\..\..\bin\server\debug\
	copy Engine___Win32_OutRead_Debug\Engine.dll ..\..\..\bin\server\engine.dll
	copy Engine___Win32_OutRead_Debug\Engine.dll ..\..\..\bin\server\debug\engine.dll
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"

OUTDIR=.\Engine___Win32_OuRead_Release
INTDIR=.\Engine___Win32_OuRead_Release
# 开始自定义宏
OutDir=.\Engine___Win32_OuRead_Release
# 结束自定义宏

ALL : "$(OUTDIR)\Engine.dll" "$(OUTDIR)\Engine.pch"


CLEAN :
	-@erase "$(INTDIR)\alloc.obj"
	-@erase "$(INTDIR)\EDOneTimePad.obj"
	-@erase "$(INTDIR)\Engine.pch"
	-@erase "$(INTDIR)\io.obj"
	-@erase "$(INTDIR)\KAutoMutex.obj"
	-@erase "$(INTDIR)\KAviFile.obj"
	-@erase "$(INTDIR)\KBitmap.obj"
	-@erase "$(INTDIR)\KBitmap16.obj"
	-@erase "$(INTDIR)\KBitmapConvert.obj"
	-@erase "$(INTDIR)\KBmp2Spr.obj"
	-@erase "$(INTDIR)\KBmpFile.obj"
	-@erase "$(INTDIR)\KBmpFile24.obj"
	-@erase "$(INTDIR)\KCache.obj"
	-@erase "$(INTDIR)\KCanvas.obj"
	-@erase "$(INTDIR)\KCodec.obj"
	-@erase "$(INTDIR)\KCodecLzo.obj"
	-@erase "$(INTDIR)\KColors.obj"
	-@erase "$(INTDIR)\KDDraw.obj"
	-@erase "$(INTDIR)\KDebug.obj"
	-@erase "$(INTDIR)\KDError.obj"
	-@erase "$(INTDIR)\KDInput.obj"
	-@erase "$(INTDIR)\KDrawBase.obj"
	-@erase "$(INTDIR)\KDrawBitmap.obj"
	-@erase "$(INTDIR)\KDrawBitmap16.obj"
	-@erase "$(INTDIR)\KDrawFade.obj"
	-@erase "$(INTDIR)\KDrawFont.obj"
	-@erase "$(INTDIR)\KDrawSprite.obj"
	-@erase "$(INTDIR)\KDrawSpriteAlpha.obj"
	-@erase "$(INTDIR)\KDSound.obj"
	-@erase "$(INTDIR)\KEicScript.obj"
	-@erase "$(INTDIR)\KEicScriptSet.obj"
	-@erase "$(INTDIR)\KEngine.obj"
	-@erase "$(INTDIR)\KEvent.obj"
	-@erase "$(INTDIR)\KFile.obj"
	-@erase "$(INTDIR)\KFileCopy.obj"
	-@erase "$(INTDIR)\KFileDialog.obj"
	-@erase "$(INTDIR)\KFilePath.obj"
	-@erase "$(INTDIR)\KFindBinTree.obj"
	-@erase "$(INTDIR)\KFont.obj"
	-@erase "$(INTDIR)\KGifFile.obj"
	-@erase "$(INTDIR)\KGraphics.obj"
	-@erase "$(INTDIR)\KHashList.obj"
	-@erase "$(INTDIR)\KHashNode.obj"
	-@erase "$(INTDIR)\KHashTable.obj"
	-@erase "$(INTDIR)\Kime.obj"
	-@erase "$(INTDIR)\KIniFile.obj"
	-@erase "$(INTDIR)\KJpgFile.obj"
	-@erase "$(INTDIR)\KKeyboard.obj"
	-@erase "$(INTDIR)\KLinkArray.obj"
	-@erase "$(INTDIR)\KList.obj"
	-@erase "$(INTDIR)\KLuaScript.obj"
	-@erase "$(INTDIR)\KLuaScriptSet.obj"
	-@erase "$(INTDIR)\KLubCmpl_Blocker.obj"
	-@erase "$(INTDIR)\KMemBase.obj"
	-@erase "$(INTDIR)\KMemClass.obj"
	-@erase "$(INTDIR)\KMemClass1.obj"
	-@erase "$(INTDIR)\KMemManager.obj"
	-@erase "$(INTDIR)\KMemStack.obj"
	-@erase "$(INTDIR)\KMessage.obj"
	-@erase "$(INTDIR)\KMouse.obj"
	-@erase "$(INTDIR)\KMp3Music.obj"
	-@erase "$(INTDIR)\KMp4Audio.obj"
	-@erase "$(INTDIR)\KMp4Movie.obj"
	-@erase "$(INTDIR)\KMp4Video.obj"
	-@erase "$(INTDIR)\KMpgMusic.obj"
	-@erase "$(INTDIR)\KMsgNode.obj"
	-@erase "$(INTDIR)\KMusic.obj"
	-@erase "$(INTDIR)\KMutex.obj"
	-@erase "$(INTDIR)\KNode.obj"
	-@erase "$(INTDIR)\KOctree.obj"
	-@erase "$(INTDIR)\KOctreeNode.obj"
	-@erase "$(INTDIR)\KPakData.obj"
	-@erase "$(INTDIR)\KPakFile.obj"
	-@erase "$(INTDIR)\KPakList.obj"
	-@erase "$(INTDIR)\KPakTool.obj"
	-@erase "$(INTDIR)\KPalette.obj"
	-@erase "$(INTDIR)\KPcxFile.obj"
	-@erase "$(INTDIR)\KPolygon.obj"
	-@erase "$(INTDIR)\KPolyRelation.obj"
	-@erase "$(INTDIR)\KRandom.obj"
	-@erase "$(INTDIR)\KSafeList.obj"
	-@erase "$(INTDIR)\KScanDir.obj"
	-@erase "$(INTDIR)\KScript.obj"
	-@erase "$(INTDIR)\KScriptCache.obj"
	-@erase "$(INTDIR)\KScriptList.obj"
	-@erase "$(INTDIR)\KScriptSet.obj"
	-@erase "$(INTDIR)\KSG_MD5_String.obj"
	-@erase "$(INTDIR)\KSG_StringProcess.obj"
	-@erase "$(INTDIR)\KSortBinTree.obj"
	-@erase "$(INTDIR)\KSortList.obj"
	-@erase "$(INTDIR)\KSoundCache.obj"
	-@erase "$(INTDIR)\KSprite.obj"
	-@erase "$(INTDIR)\KSpriteCache.obj"
	-@erase "$(INTDIR)\KSpriteCodec.obj"
	-@erase "$(INTDIR)\KSpriteMaker.obj"
	-@erase "$(INTDIR)\KStepLuaScript.obj"
	-@erase "$(INTDIR)\KStrBase.obj"
	-@erase "$(INTDIR)\KStrList.obj"
	-@erase "$(INTDIR)\KStrNode.obj"
	-@erase "$(INTDIR)\KTabFile.obj"
	-@erase "$(INTDIR)\KTabFileCtrl.obj"
	-@erase "$(INTDIR)\KTgaFile32.obj"
	-@erase "$(INTDIR)\KThread.obj"
	-@erase "$(INTDIR)\KTimer.obj"
	-@erase "$(INTDIR)\KVideo.obj"
	-@erase "$(INTDIR)\KWavCodec.obj"
	-@erase "$(INTDIR)\KWavFile.obj"
	-@erase "$(INTDIR)\KWavMusic.obj"
	-@erase "$(INTDIR)\KWavSound.obj"
	-@erase "$(INTDIR)\KWin32.obj"
	-@erase "$(INTDIR)\KWin32App.obj"
	-@erase "$(INTDIR)\KWin32Wnd.obj"
	-@erase "$(INTDIR)\KZipCodec.obj"
	-@erase "$(INTDIR)\KZipData.obj"
	-@erase "$(INTDIR)\KZipFile.obj"
	-@erase "$(INTDIR)\KZipList.obj"
	-@erase "$(INTDIR)\md5.obj"
	-@erase "$(INTDIR)\n2b_99.obj"
	-@erase "$(INTDIR)\n2b_d.obj"
	-@erase "$(INTDIR)\n2b_ds.obj"
	-@erase "$(INTDIR)\n2b_to.obj"
	-@erase "$(INTDIR)\n2d_99.obj"
	-@erase "$(INTDIR)\n2d_d.obj"
	-@erase "$(INTDIR)\n2d_ds.obj"
	-@erase "$(INTDIR)\n2d_to.obj"
	-@erase "$(INTDIR)\n2e_99.obj"
	-@erase "$(INTDIR)\n2e_d.obj"
	-@erase "$(INTDIR)\n2e_ds.obj"
	-@erase "$(INTDIR)\n2e_to.obj"
	-@erase "$(INTDIR)\PassChecksum.obj"
	-@erase "$(INTDIR)\stdafx.obj"
	-@erase "$(INTDIR)\Text.obj"
	-@erase "$(INTDIR)\ucl_crc.obj"
	-@erase "$(INTDIR)\ucl_dll.obj"
	-@erase "$(INTDIR)\ucl_init.obj"
	-@erase "$(INTDIR)\ucl_ptr.obj"
	-@erase "$(INTDIR)\ucl_str.obj"
	-@erase "$(INTDIR)\ucl_util.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\XPackFile.obj"
	-@erase "$(INTDIR)\ZSPRPackFile.obj"
	-@erase "$(OUTDIR)\Engine.dll"
	-@erase "$(OUTDIR)\Engine.exp"
	-@erase "$(OUTDIR)\Engine.lib"
	-@erase "$(OUTDIR)\Engine.map"
	-@erase "$(OUTDIR)\Engine.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /Zi /O2 /I "include" /I "..\Pack\ucl-1.01\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D "USE_STANDALONE_SPR" /Fp"$(INTDIR)\Engine.pch" /YX"KWin32.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Engine.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib comdlg32.lib ddraw.lib dsound.lib dxguid.lib winmm.lib wsock32.lib dinput8.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\Engine.pdb" /map:"$(INTDIR)\Engine.map" /debug /machine:I386 /out:"$(OUTDIR)\Engine.dll" /implib:"$(OUTDIR)\Engine.lib" 
LINK32_OBJS= \
	"$(INTDIR)\KAutoMutex.obj" \
	"$(INTDIR)\KAviFile.obj" \
	"$(INTDIR)\KBitmap.obj" \
	"$(INTDIR)\KBitmap16.obj" \
	"$(INTDIR)\KBitmapConvert.obj" \
	"$(INTDIR)\KBmp2Spr.obj" \
	"$(INTDIR)\KBmpFile.obj" \
	"$(INTDIR)\KBmpFile24.obj" \
	"$(INTDIR)\KCache.obj" \
	"$(INTDIR)\KCanvas.obj" \
	"$(INTDIR)\KCodec.obj" \
	"$(INTDIR)\KCodecLzo.obj" \
	"$(INTDIR)\KColors.obj" \
	"$(INTDIR)\KDDraw.obj" \
	"$(INTDIR)\KDebug.obj" \
	"$(INTDIR)\KDError.obj" \
	"$(INTDIR)\KDInput.obj" \
	"$(INTDIR)\KDrawBase.obj" \
	"$(INTDIR)\KDrawBitmap.obj" \
	"$(INTDIR)\KDrawBitmap16.obj" \
	"$(INTDIR)\KDrawFade.obj" \
	"$(INTDIR)\KDrawFont.obj" \
	"$(INTDIR)\KDrawSprite.obj" \
	"$(INTDIR)\KDrawSpriteAlpha.obj" \
	"$(INTDIR)\KDSound.obj" \
	"$(INTDIR)\KEicScript.obj" \
	"$(INTDIR)\KEicScriptSet.obj" \
	"$(INTDIR)\KEngine.obj" \
	"$(INTDIR)\KEvent.obj" \
	"$(INTDIR)\KFile.obj" \
	"$(INTDIR)\KFileCopy.obj" \
	"$(INTDIR)\KFileDialog.obj" \
	"$(INTDIR)\KFilePath.obj" \
	"$(INTDIR)\KFindBinTree.obj" \
	"$(INTDIR)\KFont.obj" \
	"$(INTDIR)\KGifFile.obj" \
	"$(INTDIR)\KGraphics.obj" \
	"$(INTDIR)\KHashList.obj" \
	"$(INTDIR)\KHashNode.obj" \
	"$(INTDIR)\KHashTable.obj" \
	"$(INTDIR)\Kime.obj" \
	"$(INTDIR)\KIniFile.obj" \
	"$(INTDIR)\KJpgFile.obj" \
	"$(INTDIR)\KKeyboard.obj" \
	"$(INTDIR)\KLinkArray.obj" \
	"$(INTDIR)\KList.obj" \
	"$(INTDIR)\KLuaScript.obj" \
	"$(INTDIR)\KLuaScriptSet.obj" \
	"$(INTDIR)\KLubCmpl_Blocker.obj" \
	"$(INTDIR)\KMemBase.obj" \
	"$(INTDIR)\KMemClass.obj" \
	"$(INTDIR)\KMemClass1.obj" \
	"$(INTDIR)\KMemManager.obj" \
	"$(INTDIR)\KMemStack.obj" \
	"$(INTDIR)\KMessage.obj" \
	"$(INTDIR)\KMouse.obj" \
	"$(INTDIR)\KMp3Music.obj" \
	"$(INTDIR)\KMp4Audio.obj" \
	"$(INTDIR)\KMp4Movie.obj" \
	"$(INTDIR)\KMp4Video.obj" \
	"$(INTDIR)\KMpgMusic.obj" \
	"$(INTDIR)\KMsgNode.obj" \
	"$(INTDIR)\KMusic.obj" \
	"$(INTDIR)\KMutex.obj" \
	"$(INTDIR)\KNode.obj" \
	"$(INTDIR)\KOctree.obj" \
	"$(INTDIR)\KOctreeNode.obj" \
	"$(INTDIR)\KPakData.obj" \
	"$(INTDIR)\KPakFile.obj" \
	"$(INTDIR)\KPakList.obj" \
	"$(INTDIR)\KPakTool.obj" \
	"$(INTDIR)\KPalette.obj" \
	"$(INTDIR)\KPcxFile.obj" \
	"$(INTDIR)\KPolygon.obj" \
	"$(INTDIR)\KPolyRelation.obj" \
	"$(INTDIR)\KRandom.obj" \
	"$(INTDIR)\KSafeList.obj" \
	"$(INTDIR)\KScanDir.obj" \
	"$(INTDIR)\KScript.obj" \
	"$(INTDIR)\KScriptCache.obj" \
	"$(INTDIR)\KScriptList.obj" \
	"$(INTDIR)\KScriptSet.obj" \
	"$(INTDIR)\KSG_MD5_String.obj" \
	"$(INTDIR)\KSG_StringProcess.obj" \
	"$(INTDIR)\KSortBinTree.obj" \
	"$(INTDIR)\KSortList.obj" \
	"$(INTDIR)\KSoundCache.obj" \
	"$(INTDIR)\KSprite.obj" \
	"$(INTDIR)\KSpriteCache.obj" \
	"$(INTDIR)\KSpriteCodec.obj" \
	"$(INTDIR)\KSpriteMaker.obj" \
	"$(INTDIR)\KStepLuaScript.obj" \
	"$(INTDIR)\KStrBase.obj" \
	"$(INTDIR)\KStrList.obj" \
	"$(INTDIR)\KStrNode.obj" \
	"$(INTDIR)\KTabFile.obj" \
	"$(INTDIR)\KTabFileCtrl.obj" \
	"$(INTDIR)\KTgaFile32.obj" \
	"$(INTDIR)\KThread.obj" \
	"$(INTDIR)\KTimer.obj" \
	"$(INTDIR)\KVideo.obj" \
	"$(INTDIR)\KWavCodec.obj" \
	"$(INTDIR)\KWavFile.obj" \
	"$(INTDIR)\KWavMusic.obj" \
	"$(INTDIR)\KWavSound.obj" \
	"$(INTDIR)\KWin32.obj" \
	"$(INTDIR)\KWin32App.obj" \
	"$(INTDIR)\KWin32Wnd.obj" \
	"$(INTDIR)\KZipCodec.obj" \
	"$(INTDIR)\KZipData.obj" \
	"$(INTDIR)\KZipFile.obj" \
	"$(INTDIR)\KZipList.obj" \
	"$(INTDIR)\md5.obj" \
	"$(INTDIR)\PassChecksum.obj" \
	"$(INTDIR)\stdafx.obj" \
	"$(INTDIR)\XPackFile.obj" \
	"$(INTDIR)\ZSPRPackFile.obj" \
	"$(INTDIR)\EDOneTimePad.obj" \
	"$(INTDIR)\Text.obj" \
	"$(INTDIR)\alloc.obj" \
	"$(INTDIR)\io.obj" \
	"$(INTDIR)\n2b_99.obj" \
	"$(INTDIR)\n2b_d.obj" \
	"$(INTDIR)\n2b_ds.obj" \
	"$(INTDIR)\n2b_to.obj" \
	"$(INTDIR)\n2d_99.obj" \
	"$(INTDIR)\n2d_d.obj" \
	"$(INTDIR)\n2d_ds.obj" \
	"$(INTDIR)\n2d_to.obj" \
	"$(INTDIR)\n2e_99.obj" \
	"$(INTDIR)\n2e_d.obj" \
	"$(INTDIR)\n2e_ds.obj" \
	"$(INTDIR)\n2e_to.obj" \
	"$(INTDIR)\ucl_crc.obj" \
	"$(INTDIR)\ucl_dll.obj" \
	"$(INTDIR)\ucl_init.obj" \
	"$(INTDIR)\ucl_ptr.obj" \
	"$(INTDIR)\ucl_str.obj" \
	"$(INTDIR)\ucl_util.obj" \
	"..\..\Lib\release\JpgLib.lib" \
	"..\..\Lib\release\mp3lib.lib" \
	"..\..\bin\server\lualibdll.lib"

"$(OUTDIR)\Engine.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# 开始自定义宏
OutDir=.\Engine___Win32_OuRead_Release
# 结束自定义宏

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\Engine.dll" "$(OUTDIR)\Engine.pch"
   md ..\..\lib\release
	copy Engine___Win32_OuRead_Release\Engine.lib ..\..\lib\release\engine.lib
	md ..\..\..\bin\client\release\
	copy Engine___Win32_OuRead_Release\engine.dll ..\..\..\bin\client\engine.dll
	copy Engine___Win32_OuRead_Release\engine.dll ..\..\..\bin\client\release\engine.dll
	copy Engine___Win32_OuRead_Release\engine.pdb ..\..\..\bin\client\release\engine.pdb
	copy Engine___Win32_OuRead_Release\engine.map ..\..\..\bin\client\release\engine.map
	md ..\..\..\bin\server\release\
	copy Engine___Win32_OuRead_Release\Engine.dll ..\..\..\bin\server\engine.dll
	copy Engine___Win32_OuRead_Release\Engine.dll ..\..\..\bin\server\release\engine.dll
	copy Engine___Win32_OuRead_Release\Engine.pdb ..\..\..\bin\server\release\engine.pdb
	copy Engine___Win32_OuRead_Release\Engine.map ..\..\..\bin\server\release\engine.map
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
!IF EXISTS("Engine.dep")
!INCLUDE "Engine.dep"
!ELSE 
!MESSAGE Warning: cannot find "Engine.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "Engine - Win32 Release" || "$(CFG)" == "Engine - Win32 Debug" || "$(CFG)" == "Engine - Win32 OutRead Debug" || "$(CFG)" == "Engine - Win32 OuRead Release"
SOURCE=.\Src\KAutoMutex.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KAutoMutex.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KAutoMutex.obj"	"$(INTDIR)\KAutoMutex.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KAutoMutex.obj"	"$(INTDIR)\KAutoMutex.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KAutoMutex.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KAviFile.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KAviFile.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KAviFile.obj"	"$(INTDIR)\KAviFile.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KAviFile.obj"	"$(INTDIR)\KAviFile.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KAviFile.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KBitmap.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KBitmap.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KBitmap.obj"	"$(INTDIR)\KBitmap.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KBitmap.obj"	"$(INTDIR)\KBitmap.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KBitmap.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KBitmap16.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KBitmap16.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KBitmap16.obj"	"$(INTDIR)\KBitmap16.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KBitmap16.obj"	"$(INTDIR)\KBitmap16.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KBitmap16.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KBitmapConvert.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KBitmapConvert.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KBitmapConvert.obj"	"$(INTDIR)\KBitmapConvert.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KBitmapConvert.obj"	"$(INTDIR)\KBitmapConvert.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KBitmapConvert.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KBmp2Spr.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KBmp2Spr.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KBmp2Spr.obj"	"$(INTDIR)\KBmp2Spr.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KBmp2Spr.obj"	"$(INTDIR)\KBmp2Spr.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KBmp2Spr.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KBmpFile.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KBmpFile.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KBmpFile.obj"	"$(INTDIR)\KBmpFile.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KBmpFile.obj"	"$(INTDIR)\KBmpFile.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KBmpFile.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KBmpFile24.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KBmpFile24.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KBmpFile24.obj"	"$(INTDIR)\KBmpFile24.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KBmpFile24.obj"	"$(INTDIR)\KBmpFile24.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KBmpFile24.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KCache.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KCache.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KCache.obj"	"$(INTDIR)\KCache.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KCache.obj"	"$(INTDIR)\KCache.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KCache.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KCanvas.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KCanvas.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KCanvas.obj"	"$(INTDIR)\KCanvas.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KCanvas.obj"	"$(INTDIR)\KCanvas.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KCanvas.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KCodec.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KCodec.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KCodec.obj"	"$(INTDIR)\KCodec.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KCodec.obj"	"$(INTDIR)\KCodec.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KCodec.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KCodecLzo.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KCodecLzo.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KCodecLzo.obj"	"$(INTDIR)\KCodecLzo.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KCodecLzo.obj"	"$(INTDIR)\KCodecLzo.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KCodecLzo.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KColors.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KColors.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KColors.obj"	"$(INTDIR)\KColors.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KColors.obj"	"$(INTDIR)\KColors.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KColors.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KDDraw.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KDDraw.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KDDraw.obj"	"$(INTDIR)\KDDraw.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KDDraw.obj"	"$(INTDIR)\KDDraw.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KDDraw.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KDebug.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KDebug.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KDebug.obj"	"$(INTDIR)\KDebug.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KDebug.obj"	"$(INTDIR)\KDebug.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KDebug.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KDError.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KDError.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KDError.obj"	"$(INTDIR)\KDError.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KDError.obj"	"$(INTDIR)\KDError.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KDError.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KDInput.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KDInput.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KDInput.obj"	"$(INTDIR)\KDInput.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KDInput.obj"	"$(INTDIR)\KDInput.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KDInput.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KDrawBase.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KDrawBase.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KDrawBase.obj"	"$(INTDIR)\KDrawBase.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KDrawBase.obj"	"$(INTDIR)\KDrawBase.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KDrawBase.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KDrawBitmap.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KDrawBitmap.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KDrawBitmap.obj"	"$(INTDIR)\KDrawBitmap.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KDrawBitmap.obj"	"$(INTDIR)\KDrawBitmap.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KDrawBitmap.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KDrawBitmap16.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KDrawBitmap16.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KDrawBitmap16.obj"	"$(INTDIR)\KDrawBitmap16.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KDrawBitmap16.obj"	"$(INTDIR)\KDrawBitmap16.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KDrawBitmap16.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KDrawFade.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KDrawFade.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KDrawFade.obj"	"$(INTDIR)\KDrawFade.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KDrawFade.obj"	"$(INTDIR)\KDrawFade.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KDrawFade.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KDrawFont.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KDrawFont.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KDrawFont.obj"	"$(INTDIR)\KDrawFont.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KDrawFont.obj"	"$(INTDIR)\KDrawFont.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KDrawFont.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KDrawSprite.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KDrawSprite.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KDrawSprite.obj"	"$(INTDIR)\KDrawSprite.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KDrawSprite.obj"	"$(INTDIR)\KDrawSprite.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KDrawSprite.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KDrawSpriteAlpha.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KDrawSpriteAlpha.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KDrawSpriteAlpha.obj"	"$(INTDIR)\KDrawSpriteAlpha.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KDrawSpriteAlpha.obj"	"$(INTDIR)\KDrawSpriteAlpha.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KDrawSpriteAlpha.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KDSound.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KDSound.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KDSound.obj"	"$(INTDIR)\KDSound.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KDSound.obj"	"$(INTDIR)\KDSound.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KDSound.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KEicScript.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KEicScript.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KEicScript.obj"	"$(INTDIR)\KEicScript.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KEicScript.obj"	"$(INTDIR)\KEicScript.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KEicScript.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KEicScriptSet.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KEicScriptSet.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KEicScriptSet.obj"	"$(INTDIR)\KEicScriptSet.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KEicScriptSet.obj"	"$(INTDIR)\KEicScriptSet.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KEicScriptSet.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KEngine.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KEngine.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KEngine.obj"	"$(INTDIR)\KEngine.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KEngine.obj"	"$(INTDIR)\KEngine.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KEngine.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KEvent.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KEvent.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KEvent.obj"	"$(INTDIR)\KEvent.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KEvent.obj"	"$(INTDIR)\KEvent.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KEvent.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KFile.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KFile.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KFile.obj"	"$(INTDIR)\KFile.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KFile.obj"	"$(INTDIR)\KFile.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KFile.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KFileCopy.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KFileCopy.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KFileCopy.obj"	"$(INTDIR)\KFileCopy.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KFileCopy.obj"	"$(INTDIR)\KFileCopy.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KFileCopy.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KFileDialog.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KFileDialog.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KFileDialog.obj"	"$(INTDIR)\KFileDialog.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KFileDialog.obj"	"$(INTDIR)\KFileDialog.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KFileDialog.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KFilePath.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KFilePath.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KFilePath.obj"	"$(INTDIR)\KFilePath.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KFilePath.obj"	"$(INTDIR)\KFilePath.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KFilePath.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KFindBinTree.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KFindBinTree.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KFindBinTree.obj"	"$(INTDIR)\KFindBinTree.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KFindBinTree.obj"	"$(INTDIR)\KFindBinTree.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KFindBinTree.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KFont.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KFont.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KFont.obj"	"$(INTDIR)\KFont.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KFont.obj"	"$(INTDIR)\KFont.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KFont.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KGifFile.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KGifFile.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KGifFile.obj"	"$(INTDIR)\KGifFile.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KGifFile.obj"	"$(INTDIR)\KGifFile.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KGifFile.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KGraphics.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KGraphics.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KGraphics.obj"	"$(INTDIR)\KGraphics.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KGraphics.obj"	"$(INTDIR)\KGraphics.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KGraphics.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KHashList.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KHashList.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KHashList.obj"	"$(INTDIR)\KHashList.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KHashList.obj"	"$(INTDIR)\KHashList.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KHashList.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KHashNode.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KHashNode.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KHashNode.obj"	"$(INTDIR)\KHashNode.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KHashNode.obj"	"$(INTDIR)\KHashNode.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KHashNode.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KHashTable.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KHashTable.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KHashTable.obj"	"$(INTDIR)\KHashTable.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KHashTable.obj"	"$(INTDIR)\KHashTable.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KHashTable.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\Kime.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\Kime.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\Kime.obj"	"$(INTDIR)\Kime.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\Kime.obj"	"$(INTDIR)\Kime.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\Kime.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KIniFile.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KIniFile.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KIniFile.obj"	"$(INTDIR)\KIniFile.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KIniFile.obj"	"$(INTDIR)\KIniFile.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KIniFile.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KJpgFile.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KJpgFile.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KJpgFile.obj"	"$(INTDIR)\KJpgFile.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KJpgFile.obj"	"$(INTDIR)\KJpgFile.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KJpgFile.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KKeyboard.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KKeyboard.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KKeyboard.obj"	"$(INTDIR)\KKeyboard.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KKeyboard.obj"	"$(INTDIR)\KKeyboard.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KKeyboard.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KLinkArray.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KLinkArray.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KLinkArray.obj"	"$(INTDIR)\KLinkArray.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KLinkArray.obj"	"$(INTDIR)\KLinkArray.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KLinkArray.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KList.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KList.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KList.obj"	"$(INTDIR)\KList.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KList.obj"	"$(INTDIR)\KList.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KList.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KLuaScript.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KLuaScript.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KLuaScript.obj"	"$(INTDIR)\KLuaScript.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KLuaScript.obj"	"$(INTDIR)\KLuaScript.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KLuaScript.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KLuaScriptSet.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KLuaScriptSet.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KLuaScriptSet.obj"	"$(INTDIR)\KLuaScriptSet.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KLuaScriptSet.obj"	"$(INTDIR)\KLuaScriptSet.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KLuaScriptSet.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KLubCmpl_Blocker.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KLubCmpl_Blocker.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KLubCmpl_Blocker.obj"	"$(INTDIR)\KLubCmpl_Blocker.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KLubCmpl_Blocker.obj"	"$(INTDIR)\KLubCmpl_Blocker.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KLubCmpl_Blocker.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KMemBase.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KMemBase.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KMemBase.obj"	"$(INTDIR)\KMemBase.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KMemBase.obj"	"$(INTDIR)\KMemBase.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KMemBase.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KMemClass.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KMemClass.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KMemClass.obj"	"$(INTDIR)\KMemClass.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KMemClass.obj"	"$(INTDIR)\KMemClass.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KMemClass.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KMemClass1.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KMemClass1.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KMemClass1.obj"	"$(INTDIR)\KMemClass1.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KMemClass1.obj"	"$(INTDIR)\KMemClass1.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KMemClass1.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KMemManager.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KMemManager.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KMemManager.obj"	"$(INTDIR)\KMemManager.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KMemManager.obj"	"$(INTDIR)\KMemManager.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KMemManager.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KMemStack.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KMemStack.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KMemStack.obj"	"$(INTDIR)\KMemStack.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KMemStack.obj"	"$(INTDIR)\KMemStack.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KMemStack.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KMessage.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KMessage.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KMessage.obj"	"$(INTDIR)\KMessage.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KMessage.obj"	"$(INTDIR)\KMessage.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KMessage.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KMouse.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KMouse.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KMouse.obj"	"$(INTDIR)\KMouse.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KMouse.obj"	"$(INTDIR)\KMouse.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KMouse.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KMp3Music.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KMp3Music.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KMp3Music.obj"	"$(INTDIR)\KMp3Music.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KMp3Music.obj"	"$(INTDIR)\KMp3Music.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KMp3Music.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KMp4Audio.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KMp4Audio.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KMp4Audio.obj"	"$(INTDIR)\KMp4Audio.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KMp4Audio.obj"	"$(INTDIR)\KMp4Audio.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KMp4Audio.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KMp4Movie.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KMp4Movie.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KMp4Movie.obj"	"$(INTDIR)\KMp4Movie.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KMp4Movie.obj"	"$(INTDIR)\KMp4Movie.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KMp4Movie.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KMp4Video.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KMp4Video.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KMp4Video.obj"	"$(INTDIR)\KMp4Video.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KMp4Video.obj"	"$(INTDIR)\KMp4Video.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KMp4Video.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KMpgMusic.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KMpgMusic.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KMpgMusic.obj"	"$(INTDIR)\KMpgMusic.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KMpgMusic.obj"	"$(INTDIR)\KMpgMusic.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KMpgMusic.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KMsgNode.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KMsgNode.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KMsgNode.obj"	"$(INTDIR)\KMsgNode.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KMsgNode.obj"	"$(INTDIR)\KMsgNode.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KMsgNode.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KMusic.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KMusic.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KMusic.obj"	"$(INTDIR)\KMusic.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KMusic.obj"	"$(INTDIR)\KMusic.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KMusic.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KMutex.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KMutex.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KMutex.obj"	"$(INTDIR)\KMutex.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KMutex.obj"	"$(INTDIR)\KMutex.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KMutex.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KNode.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KNode.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KNode.obj"	"$(INTDIR)\KNode.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KNode.obj"	"$(INTDIR)\KNode.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KNode.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KOctree.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KOctree.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KOctree.obj"	"$(INTDIR)\KOctree.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KOctree.obj"	"$(INTDIR)\KOctree.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KOctree.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KOctreeNode.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KOctreeNode.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KOctreeNode.obj"	"$(INTDIR)\KOctreeNode.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KOctreeNode.obj"	"$(INTDIR)\KOctreeNode.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KOctreeNode.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KPakData.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KPakData.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KPakData.obj"	"$(INTDIR)\KPakData.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KPakData.obj"	"$(INTDIR)\KPakData.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KPakData.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KPakFile.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KPakFile.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KPakFile.obj"	"$(INTDIR)\KPakFile.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KPakFile.obj"	"$(INTDIR)\KPakFile.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KPakFile.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KPakList.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KPakList.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KPakList.obj"	"$(INTDIR)\KPakList.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KPakList.obj"	"$(INTDIR)\KPakList.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KPakList.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KPakTool.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KPakTool.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KPakTool.obj"	"$(INTDIR)\KPakTool.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KPakTool.obj"	"$(INTDIR)\KPakTool.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KPakTool.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KPalette.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KPalette.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KPalette.obj"	"$(INTDIR)\KPalette.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KPalette.obj"	"$(INTDIR)\KPalette.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KPalette.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KPcxFile.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KPcxFile.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KPcxFile.obj"	"$(INTDIR)\KPcxFile.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KPcxFile.obj"	"$(INTDIR)\KPcxFile.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KPcxFile.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KPolygon.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KPolygon.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KPolygon.obj"	"$(INTDIR)\KPolygon.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KPolygon.obj"	"$(INTDIR)\KPolygon.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KPolygon.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KPolyRelation.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KPolyRelation.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KPolyRelation.obj"	"$(INTDIR)\KPolyRelation.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KPolyRelation.obj"	"$(INTDIR)\KPolyRelation.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KPolyRelation.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KRandom.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KRandom.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KRandom.obj"	"$(INTDIR)\KRandom.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KRandom.obj"	"$(INTDIR)\KRandom.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KRandom.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KSafeList.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KSafeList.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KSafeList.obj"	"$(INTDIR)\KSafeList.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KSafeList.obj"	"$(INTDIR)\KSafeList.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KSafeList.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KScanDir.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KScanDir.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KScanDir.obj"	"$(INTDIR)\KScanDir.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KScanDir.obj"	"$(INTDIR)\KScanDir.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KScanDir.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KScript.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KScript.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KScript.obj"	"$(INTDIR)\KScript.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KScript.obj"	"$(INTDIR)\KScript.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KScript.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KScriptCache.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KScriptCache.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KScriptCache.obj"	"$(INTDIR)\KScriptCache.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KScriptCache.obj"	"$(INTDIR)\KScriptCache.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KScriptCache.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KScriptList.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KScriptList.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KScriptList.obj"	"$(INTDIR)\KScriptList.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KScriptList.obj"	"$(INTDIR)\KScriptList.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KScriptList.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KScriptSet.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KScriptSet.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KScriptSet.obj"	"$(INTDIR)\KScriptSet.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KScriptSet.obj"	"$(INTDIR)\KScriptSet.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KScriptSet.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KSG_MD5_String.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KSG_MD5_String.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KSG_MD5_String.obj"	"$(INTDIR)\KSG_MD5_String.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KSG_MD5_String.obj"	"$(INTDIR)\KSG_MD5_String.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KSG_MD5_String.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KSG_StringProcess.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KSG_StringProcess.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KSG_StringProcess.obj"	"$(INTDIR)\KSG_StringProcess.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KSG_StringProcess.obj"	"$(INTDIR)\KSG_StringProcess.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KSG_StringProcess.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KSortBinTree.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KSortBinTree.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KSortBinTree.obj"	"$(INTDIR)\KSortBinTree.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KSortBinTree.obj"	"$(INTDIR)\KSortBinTree.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KSortBinTree.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KSortList.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KSortList.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KSortList.obj"	"$(INTDIR)\KSortList.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KSortList.obj"	"$(INTDIR)\KSortList.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KSortList.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KSoundCache.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KSoundCache.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KSoundCache.obj"	"$(INTDIR)\KSoundCache.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KSoundCache.obj"	"$(INTDIR)\KSoundCache.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KSoundCache.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KSprite.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KSprite.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KSprite.obj"	"$(INTDIR)\KSprite.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KSprite.obj"	"$(INTDIR)\KSprite.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KSprite.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KSpriteCache.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KSpriteCache.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KSpriteCache.obj"	"$(INTDIR)\KSpriteCache.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KSpriteCache.obj"	"$(INTDIR)\KSpriteCache.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KSpriteCache.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KSpriteCodec.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KSpriteCodec.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KSpriteCodec.obj"	"$(INTDIR)\KSpriteCodec.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KSpriteCodec.obj"	"$(INTDIR)\KSpriteCodec.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KSpriteCodec.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KSpriteMaker.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KSpriteMaker.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KSpriteMaker.obj"	"$(INTDIR)\KSpriteMaker.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KSpriteMaker.obj"	"$(INTDIR)\KSpriteMaker.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KSpriteMaker.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KStepLuaScript.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KStepLuaScript.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KStepLuaScript.obj"	"$(INTDIR)\KStepLuaScript.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KStepLuaScript.obj"	"$(INTDIR)\KStepLuaScript.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KStepLuaScript.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KStrBase.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KStrBase.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KStrBase.obj"	"$(INTDIR)\KStrBase.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KStrBase.obj"	"$(INTDIR)\KStrBase.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KStrBase.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KStrList.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KStrList.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KStrList.obj"	"$(INTDIR)\KStrList.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KStrList.obj"	"$(INTDIR)\KStrList.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KStrList.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KStrNode.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KStrNode.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KStrNode.obj"	"$(INTDIR)\KStrNode.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KStrNode.obj"	"$(INTDIR)\KStrNode.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KStrNode.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KTabFile.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KTabFile.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KTabFile.obj"	"$(INTDIR)\KTabFile.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KTabFile.obj"	"$(INTDIR)\KTabFile.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KTabFile.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KTabFileCtrl.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KTabFileCtrl.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KTabFileCtrl.obj"	"$(INTDIR)\KTabFileCtrl.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KTabFileCtrl.obj"	"$(INTDIR)\KTabFileCtrl.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KTabFileCtrl.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KTgaFile32.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KTgaFile32.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KTgaFile32.obj"	"$(INTDIR)\KTgaFile32.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KTgaFile32.obj"	"$(INTDIR)\KTgaFile32.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KTgaFile32.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KThread.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KThread.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KThread.obj"	"$(INTDIR)\KThread.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KThread.obj"	"$(INTDIR)\KThread.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KThread.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KTimer.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KTimer.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KTimer.obj"	"$(INTDIR)\KTimer.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KTimer.obj"	"$(INTDIR)\KTimer.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KTimer.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KVideo.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KVideo.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KVideo.obj"	"$(INTDIR)\KVideo.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KVideo.obj"	"$(INTDIR)\KVideo.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KVideo.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KWavCodec.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KWavCodec.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KWavCodec.obj"	"$(INTDIR)\KWavCodec.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KWavCodec.obj"	"$(INTDIR)\KWavCodec.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KWavCodec.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KWavFile.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KWavFile.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KWavFile.obj"	"$(INTDIR)\KWavFile.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KWavFile.obj"	"$(INTDIR)\KWavFile.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KWavFile.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KWavMusic.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KWavMusic.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KWavMusic.obj"	"$(INTDIR)\KWavMusic.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KWavMusic.obj"	"$(INTDIR)\KWavMusic.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KWavMusic.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KWavSound.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KWavSound.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KWavSound.obj"	"$(INTDIR)\KWavSound.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KWavSound.obj"	"$(INTDIR)\KWavSound.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KWavSound.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KWin32.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KWin32.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KWin32.obj"	"$(INTDIR)\KWin32.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KWin32.obj"	"$(INTDIR)\KWin32.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KWin32.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KWin32App.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KWin32App.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KWin32App.obj"	"$(INTDIR)\KWin32App.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KWin32App.obj"	"$(INTDIR)\KWin32App.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KWin32App.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KWin32Wnd.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KWin32Wnd.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KWin32Wnd.obj"	"$(INTDIR)\KWin32Wnd.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KWin32Wnd.obj"	"$(INTDIR)\KWin32Wnd.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KWin32Wnd.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KZipCodec.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KZipCodec.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KZipCodec.obj"	"$(INTDIR)\KZipCodec.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KZipCodec.obj"	"$(INTDIR)\KZipCodec.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KZipCodec.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KZipData.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KZipData.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KZipData.obj"	"$(INTDIR)\KZipData.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KZipData.obj"	"$(INTDIR)\KZipData.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KZipData.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KZipFile.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KZipFile.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KZipFile.obj"	"$(INTDIR)\KZipFile.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KZipFile.obj"	"$(INTDIR)\KZipFile.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KZipFile.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\KZipList.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\KZipList.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\KZipList.obj"	"$(INTDIR)\KZipList.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\KZipList.obj"	"$(INTDIR)\KZipList.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\KZipList.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\md5.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\md5.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\md5.obj"	"$(INTDIR)\md5.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\md5.obj"	"$(INTDIR)\md5.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\md5.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\PassChecksum.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\PassChecksum.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\PassChecksum.obj"	"$(INTDIR)\PassChecksum.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\PassChecksum.obj"	"$(INTDIR)\PassChecksum.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\PassChecksum.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\stdafx.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /Zi /O2 /I "include" /I "..\Pack\ucl-1.01\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D WINVER=0x0500 /Fp"$(INTDIR)\Engine.pch" /Yc"kwin32.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\stdafx.obj"	"$(INTDIR)\Engine.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D "USE_STANDALONE_SPR" /D WINVER=0x0500 /FR"$(INTDIR)\\" /Fp"$(INTDIR)\Engine.pch" /Yc"kwin32.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\stdafx.obj"	"$(INTDIR)\stdafx.sbr"	"$(INTDIR)\Engine.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D "USE_STANDALONE_SPR" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\Engine.pch" /Yc"kwin32.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\stdafx.obj"	"$(INTDIR)\stdafx.sbr"	"$(INTDIR)\Engine.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /Zi /O2 /I "include" /I "..\Pack\ucl-1.01\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D "USE_STANDALONE_SPR" /Fp"$(INTDIR)\Engine.pch" /Yc"kwin32.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\stdafx.obj"	"$(INTDIR)\Engine.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\Src\XPackFile.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\XPackFile.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\XPackFile.obj"	"$(INTDIR)\XPackFile.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\XPackFile.obj"	"$(INTDIR)\XPackFile.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\XPackFile.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\ZSPRPackFile.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\ZSPRPackFile.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\ZSPRPackFile.obj"	"$(INTDIR)\ZSPRPackFile.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\ZSPRPackFile.obj"	"$(INTDIR)\ZSPRPackFile.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\ZSPRPackFile.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\Cryptography\EDOneTimePad.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\EDOneTimePad.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\EDOneTimePad.obj"	"$(INTDIR)\EDOneTimePad.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\EDOneTimePad.obj"	"$(INTDIR)\EDOneTimePad.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\EDOneTimePad.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\Text.cpp

!IF  "$(CFG)" == "Engine - Win32 Release"


"$(INTDIR)\Text.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"


"$(INTDIR)\Text.obj"	"$(INTDIR)\Text.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"


"$(INTDIR)\Text.obj"	"$(INTDIR)\Text.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Engine.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"


"$(INTDIR)\Text.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Src\ucl\alloc.c

!IF  "$(CFG)" == "Engine - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /Zi /O2 /I "include" /I "..\Pack\ucl-1.01\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D WINVER=0x0500 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\alloc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D "USE_STANDALONE_SPR" /D WINVER=0x0500 /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\alloc.obj"	"$(INTDIR)\alloc.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D "USE_STANDALONE_SPR" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\alloc.obj"	"$(INTDIR)\alloc.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /Zi /O2 /I "include" /I "..\Pack\ucl-1.01\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D "USE_STANDALONE_SPR" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\alloc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\Src\ucl\io.c

!IF  "$(CFG)" == "Engine - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /Zi /O2 /I "include" /I "..\Pack\ucl-1.01\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D WINVER=0x0500 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\io.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D "USE_STANDALONE_SPR" /D WINVER=0x0500 /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\io.obj"	"$(INTDIR)\io.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D "USE_STANDALONE_SPR" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\io.obj"	"$(INTDIR)\io.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /Zi /O2 /I "include" /I "..\Pack\ucl-1.01\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D "USE_STANDALONE_SPR" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\io.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\Src\ucl\n2b_99.c

!IF  "$(CFG)" == "Engine - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /Zi /O2 /I "include" /I "..\Pack\ucl-1.01\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D WINVER=0x0500 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\n2b_99.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D "USE_STANDALONE_SPR" /D WINVER=0x0500 /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\n2b_99.obj"	"$(INTDIR)\n2b_99.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D "USE_STANDALONE_SPR" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\n2b_99.obj"	"$(INTDIR)\n2b_99.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /Zi /O2 /I "include" /I "..\Pack\ucl-1.01\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D "USE_STANDALONE_SPR" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\n2b_99.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\Src\ucl\n2b_d.c

!IF  "$(CFG)" == "Engine - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /Zi /O2 /I "include" /I "..\Pack\ucl-1.01\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D WINVER=0x0500 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\n2b_d.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D "USE_STANDALONE_SPR" /D WINVER=0x0500 /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\n2b_d.obj"	"$(INTDIR)\n2b_d.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D "USE_STANDALONE_SPR" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\n2b_d.obj"	"$(INTDIR)\n2b_d.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /Zi /O2 /I "include" /I "..\Pack\ucl-1.01\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D "USE_STANDALONE_SPR" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\n2b_d.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\Src\ucl\n2b_ds.c

!IF  "$(CFG)" == "Engine - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /Zi /O2 /I "include" /I "..\Pack\ucl-1.01\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D WINVER=0x0500 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\n2b_ds.obj" : $(SOURCE) "$(INTDIR)" ".\Src\ucl\n2b_d.c"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D "USE_STANDALONE_SPR" /D WINVER=0x0500 /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\n2b_ds.obj"	"$(INTDIR)\n2b_ds.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D "USE_STANDALONE_SPR" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\n2b_ds.obj"	"$(INTDIR)\n2b_ds.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /Zi /O2 /I "include" /I "..\Pack\ucl-1.01\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D "USE_STANDALONE_SPR" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\n2b_ds.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\Src\ucl\n2b_to.c

!IF  "$(CFG)" == "Engine - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /Zi /O2 /I "include" /I "..\Pack\ucl-1.01\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D WINVER=0x0500 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\n2b_to.obj" : $(SOURCE) "$(INTDIR)" ".\Src\ucl\n2b_d.c"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D "USE_STANDALONE_SPR" /D WINVER=0x0500 /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\n2b_to.obj"	"$(INTDIR)\n2b_to.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D "USE_STANDALONE_SPR" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\n2b_to.obj"	"$(INTDIR)\n2b_to.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /Zi /O2 /I "include" /I "..\Pack\ucl-1.01\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D "USE_STANDALONE_SPR" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\n2b_to.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\Src\ucl\n2d_99.c

!IF  "$(CFG)" == "Engine - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /Zi /O2 /I "include" /I "..\Pack\ucl-1.01\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D WINVER=0x0500 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\n2d_99.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D "USE_STANDALONE_SPR" /D WINVER=0x0500 /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\n2d_99.obj"	"$(INTDIR)\n2d_99.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D "USE_STANDALONE_SPR" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\n2d_99.obj"	"$(INTDIR)\n2d_99.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /Zi /O2 /I "include" /I "..\Pack\ucl-1.01\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D "USE_STANDALONE_SPR" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\n2d_99.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\Src\ucl\n2d_d.c

!IF  "$(CFG)" == "Engine - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /Zi /O2 /I "include" /I "..\Pack\ucl-1.01\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D WINVER=0x0500 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\n2d_d.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D "USE_STANDALONE_SPR" /D WINVER=0x0500 /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\n2d_d.obj"	"$(INTDIR)\n2d_d.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D "USE_STANDALONE_SPR" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\n2d_d.obj"	"$(INTDIR)\n2d_d.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /Zi /O2 /I "include" /I "..\Pack\ucl-1.01\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D "USE_STANDALONE_SPR" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\n2d_d.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\Src\ucl\n2d_ds.c

!IF  "$(CFG)" == "Engine - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /Zi /O2 /I "include" /I "..\Pack\ucl-1.01\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D WINVER=0x0500 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\n2d_ds.obj" : $(SOURCE) "$(INTDIR)" ".\Src\ucl\n2d_d.c"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D "USE_STANDALONE_SPR" /D WINVER=0x0500 /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\n2d_ds.obj"	"$(INTDIR)\n2d_ds.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D "USE_STANDALONE_SPR" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\n2d_ds.obj"	"$(INTDIR)\n2d_ds.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /Zi /O2 /I "include" /I "..\Pack\ucl-1.01\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D "USE_STANDALONE_SPR" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\n2d_ds.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\Src\ucl\n2d_to.c

!IF  "$(CFG)" == "Engine - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /Zi /O2 /I "include" /I "..\Pack\ucl-1.01\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D WINVER=0x0500 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\n2d_to.obj" : $(SOURCE) "$(INTDIR)" ".\Src\ucl\n2d_d.c"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D "USE_STANDALONE_SPR" /D WINVER=0x0500 /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\n2d_to.obj"	"$(INTDIR)\n2d_to.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D "USE_STANDALONE_SPR" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\n2d_to.obj"	"$(INTDIR)\n2d_to.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /Zi /O2 /I "include" /I "..\Pack\ucl-1.01\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D "USE_STANDALONE_SPR" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\n2d_to.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\Src\ucl\n2e_99.c

!IF  "$(CFG)" == "Engine - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /Zi /O2 /I "include" /I "..\Pack\ucl-1.01\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D WINVER=0x0500 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\n2e_99.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D "USE_STANDALONE_SPR" /D WINVER=0x0500 /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\n2e_99.obj"	"$(INTDIR)\n2e_99.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D "USE_STANDALONE_SPR" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\n2e_99.obj"	"$(INTDIR)\n2e_99.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /Zi /O2 /I "include" /I "..\Pack\ucl-1.01\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D "USE_STANDALONE_SPR" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\n2e_99.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\Src\ucl\n2e_d.c

!IF  "$(CFG)" == "Engine - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /Zi /O2 /I "include" /I "..\Pack\ucl-1.01\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D WINVER=0x0500 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\n2e_d.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D "USE_STANDALONE_SPR" /D WINVER=0x0500 /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\n2e_d.obj"	"$(INTDIR)\n2e_d.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D "USE_STANDALONE_SPR" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\n2e_d.obj"	"$(INTDIR)\n2e_d.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /Zi /O2 /I "include" /I "..\Pack\ucl-1.01\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D "USE_STANDALONE_SPR" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\n2e_d.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\Src\ucl\n2e_ds.c

!IF  "$(CFG)" == "Engine - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /Zi /O2 /I "include" /I "..\Pack\ucl-1.01\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D WINVER=0x0500 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\n2e_ds.obj" : $(SOURCE) "$(INTDIR)" ".\Src\ucl\n2e_d.c"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D "USE_STANDALONE_SPR" /D WINVER=0x0500 /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\n2e_ds.obj"	"$(INTDIR)\n2e_ds.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D "USE_STANDALONE_SPR" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\n2e_ds.obj"	"$(INTDIR)\n2e_ds.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /Zi /O2 /I "include" /I "..\Pack\ucl-1.01\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D "USE_STANDALONE_SPR" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\n2e_ds.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\Src\ucl\n2e_to.c

!IF  "$(CFG)" == "Engine - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /Zi /O2 /I "include" /I "..\Pack\ucl-1.01\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D WINVER=0x0500 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\n2e_to.obj" : $(SOURCE) "$(INTDIR)" ".\Src\ucl\n2e_d.c"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D "USE_STANDALONE_SPR" /D WINVER=0x0500 /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\n2e_to.obj"	"$(INTDIR)\n2e_to.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D "USE_STANDALONE_SPR" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\n2e_to.obj"	"$(INTDIR)\n2e_to.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /Zi /O2 /I "include" /I "..\Pack\ucl-1.01\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D "USE_STANDALONE_SPR" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\n2e_to.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\Src\ucl\ucl_crc.c

!IF  "$(CFG)" == "Engine - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /Zi /O2 /I "include" /I "..\Pack\ucl-1.01\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D WINVER=0x0500 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\ucl_crc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D "USE_STANDALONE_SPR" /D WINVER=0x0500 /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\ucl_crc.obj"	"$(INTDIR)\ucl_crc.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D "USE_STANDALONE_SPR" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\ucl_crc.obj"	"$(INTDIR)\ucl_crc.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /Zi /O2 /I "include" /I "..\Pack\ucl-1.01\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D "USE_STANDALONE_SPR" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\ucl_crc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\Src\ucl\ucl_dll.c

!IF  "$(CFG)" == "Engine - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /Zi /O2 /I "include" /I "..\Pack\ucl-1.01\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D WINVER=0x0500 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\ucl_dll.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D "USE_STANDALONE_SPR" /D WINVER=0x0500 /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\ucl_dll.obj"	"$(INTDIR)\ucl_dll.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D "USE_STANDALONE_SPR" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\ucl_dll.obj"	"$(INTDIR)\ucl_dll.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /Zi /O2 /I "include" /I "..\Pack\ucl-1.01\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D "USE_STANDALONE_SPR" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\ucl_dll.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\Src\ucl\ucl_init.c

!IF  "$(CFG)" == "Engine - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /Zi /O2 /I "include" /I "..\Pack\ucl-1.01\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D WINVER=0x0500 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\ucl_init.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D "USE_STANDALONE_SPR" /D WINVER=0x0500 /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\ucl_init.obj"	"$(INTDIR)\ucl_init.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D "USE_STANDALONE_SPR" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\ucl_init.obj"	"$(INTDIR)\ucl_init.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /Zi /O2 /I "include" /I "..\Pack\ucl-1.01\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D "USE_STANDALONE_SPR" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\ucl_init.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\Src\ucl\ucl_ptr.c

!IF  "$(CFG)" == "Engine - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /Zi /O2 /I "include" /I "..\Pack\ucl-1.01\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D WINVER=0x0500 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\ucl_ptr.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D "USE_STANDALONE_SPR" /D WINVER=0x0500 /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\ucl_ptr.obj"	"$(INTDIR)\ucl_ptr.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D "USE_STANDALONE_SPR" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\ucl_ptr.obj"	"$(INTDIR)\ucl_ptr.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /Zi /O2 /I "include" /I "..\Pack\ucl-1.01\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D "USE_STANDALONE_SPR" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\ucl_ptr.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\Src\ucl\ucl_str.c

!IF  "$(CFG)" == "Engine - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /Zi /O2 /I "include" /I "..\Pack\ucl-1.01\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D WINVER=0x0500 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\ucl_str.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D "USE_STANDALONE_SPR" /D WINVER=0x0500 /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\ucl_str.obj"	"$(INTDIR)\ucl_str.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D "USE_STANDALONE_SPR" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\ucl_str.obj"	"$(INTDIR)\ucl_str.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /Zi /O2 /I "include" /I "..\Pack\ucl-1.01\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D "USE_STANDALONE_SPR" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\ucl_str.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\Src\ucl\ucl_util.c

!IF  "$(CFG)" == "Engine - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /Zi /O2 /I "include" /I "..\Pack\ucl-1.01\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D WINVER=0x0500 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\ucl_util.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D "USE_STANDALONE_SPR" /D WINVER=0x0500 /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\ucl_util.obj"	"$(INTDIR)\ucl_util.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Engine - Win32 OutRead Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D "USE_STANDALONE_SPR" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\ucl_util.obj"	"$(INTDIR)\ucl_util.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Engine - Win32 OuRead Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /Zi /O2 /I "include" /I "..\Pack\ucl-1.01\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D "USE_STANDALONE_SPR" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\ucl_util.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 


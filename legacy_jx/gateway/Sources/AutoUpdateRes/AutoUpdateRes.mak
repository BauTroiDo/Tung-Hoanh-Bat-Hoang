# Microsoft Developer Studio Generated NMAKE File, Based on AutoUpdateRes.dsp
!IF $(CFG)" == "
CFG=AutoUpdateRes - Win32 Debug
!MESSAGE No configuration specified. Defaulting to AutoUpdateRes - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "AutoUpdateRes - Win32 Release" && "$(CFG)" != "AutoUpdateRes - Win32 Debug"
!MESSAGE 指定的配置 "$(CFG)" 无效.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AutoUpdateRes.mak" CFG="AutoUpdateRes - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AutoUpdateRes - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "AutoUpdateRes - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "AutoUpdateRes - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# 开始自定义宏
OutDir=.\Release
# 结束自定义宏

ALL : "$(OUTDIR)\AutoUpdateRes.dll" "$(OUTDIR)\AutoUpdateRes.bsc"


CLEAN :
	-@erase "$(INTDIR)\AutoUpdateRes.obj"
	-@erase "$(INTDIR)\AutoUpdateRes.pch"
	-@erase "$(INTDIR)\AutoUpdateRes.res"
	-@erase "$(INTDIR)\AutoUpdateRes.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\AutoUpdateRes.bsc"
	-@erase "$(OUTDIR)\AutoUpdateRes.dll"
	-@erase "$(OUTDIR)\AutoUpdateRes.exp"
	-@erase "$(OUTDIR)\AutoUpdateRes.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "AUTOUPDATERES_EXPORTS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\AutoUpdateRes.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x804 /fo"$(INTDIR)\AutoUpdateRes.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\AutoUpdateRes.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\AutoUpdateRes.sbr" \
	"$(INTDIR)\StdAfx.sbr"

"$(OUTDIR)\AutoUpdateRes.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=/nologo /entry:"DllMain" /dll /incremental:no /pdb:"$(OUTDIR)\AutoUpdateRes.pdb" /machine:I386 /nodefaultlib /out:"$(OUTDIR)\AutoUpdateRes.dll" /implib:"$(OUTDIR)\AutoUpdateRes.lib" 
LINK32_OBJS= \
	"$(INTDIR)\AutoUpdateRes.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\AutoUpdateRes.res"

"$(OUTDIR)\AutoUpdateRes.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# 开始自定义宏
OutDir=.\Release
# 结束自定义宏

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\AutoUpdateRes.dll" "$(OUTDIR)\AutoUpdateRes.bsc"
   copy release\AutoUpdateRes.dll ..\..\bin\Client\release\AutoUpdateRes.dll
	copy release\AutoUpdateRes.dll ..\..\bin\Client\AutoUpdateRes.dll
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "AutoUpdateRes - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# 开始自定义宏
OutDir=.\Debug
# 结束自定义宏

ALL : "$(OUTDIR)\AutoUpdateRes.dll"


CLEAN :
	-@erase "$(INTDIR)\AutoUpdateRes.obj"
	-@erase "$(INTDIR)\AutoUpdateRes.pch"
	-@erase "$(INTDIR)\AutoUpdateRes.res"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\AutoUpdateRes.dll"
	-@erase "$(OUTDIR)\AutoUpdateRes.exp"
	-@erase "$(OUTDIR)\AutoUpdateRes.lib"
	-@erase "$(OUTDIR)\AutoUpdateRes.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "AUTOUPDATERES_EXPORTS" /Fp"$(INTDIR)\AutoUpdateRes.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x804 /fo"$(INTDIR)\AutoUpdateRes.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\AutoUpdateRes.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\AutoUpdateRes.pdb" /debug /machine:I386 /out:"$(OUTDIR)\AutoUpdateRes.dll" /implib:"$(OUTDIR)\AutoUpdateRes.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\AutoUpdateRes.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\AutoUpdateRes.res"

"$(OUTDIR)\AutoUpdateRes.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# 开始自定义宏
OutDir=.\Debug
# 结束自定义宏

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\AutoUpdateRes.dll"
   copy debug\AutoUpdateRes.dll ..\..\..\bin\Client\AutoUpdateRes.dll
	copy debug\AutoUpdateRes.dll ..\..\..\bin\Client\debug\AutoUpdateRes.dll
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
!IF EXISTS("AutoUpdateRes.dep")
!INCLUDE "AutoUpdateRes.dep"
!ELSE 
!MESSAGE Warning: cannot find "AutoUpdateRes.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "AutoUpdateRes - Win32 Release" || "$(CFG)" == "AutoUpdateRes - Win32 Debug"
SOURCE=.\AutoUpdateRes.cpp

!IF  "$(CFG)" == "AutoUpdateRes - Win32 Release"


"$(INTDIR)\AutoUpdateRes.obj"	"$(INTDIR)\AutoUpdateRes.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AutoUpdateRes.pch"


!ELSEIF  "$(CFG)" == "AutoUpdateRes - Win32 Debug"


"$(INTDIR)\AutoUpdateRes.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AutoUpdateRes.pch"


!ENDIF 

SOURCE=.\res\AutoUpdateRes.rc

!IF  "$(CFG)" == "AutoUpdateRes - Win32 Release"


"$(INTDIR)\AutoUpdateRes.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x804 /fo"$(INTDIR)\AutoUpdateRes.res" /i "res" /d "NDEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "AutoUpdateRes - Win32 Debug"


"$(INTDIR)\AutoUpdateRes.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x804 /fo"$(INTDIR)\AutoUpdateRes.res" /i "res" /d "_DEBUG" $(SOURCE)


!ENDIF 

SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "AutoUpdateRes - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "AUTOUPDATERES_EXPORTS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\AutoUpdateRes.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr"	"$(INTDIR)\AutoUpdateRes.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "AutoUpdateRes - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "AUTOUPDATERES_EXPORTS" /Fp"$(INTDIR)\AutoUpdateRes.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\AutoUpdateRes.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 


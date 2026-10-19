# Microsoft Developer Studio Generated NMAKE File, Based on FilterText.dsp
!IF $(CFG)" == "
CFG=FilterText - Win32 Debug
!MESSAGE No configuration specified. Defaulting to FilterText - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "FilterText - Win32 Release" && "$(CFG)" != "FilterText - Win32 Debug"
!MESSAGE 指定的配置 "$(CFG)" 无效.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "FilterText.mak" CFG="FilterText - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "FilterText - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "FilterText - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "FilterText - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# 开始自定义宏
OutDir=.\Release
# 结束自定义宏

ALL : "$(OUTDIR)\FilterText.dll"


CLEAN :
	-@erase "$(INTDIR)\FilterText.obj"
	-@erase "$(INTDIR)\FilterText.pch"
	-@erase "$(INTDIR)\Regexp.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\FilterText.dll"
	-@erase "$(OUTDIR)\FilterText.exp"
	-@erase "$(OUTDIR)\FilterText.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "FILTERTEXT_EXPORTS" /Fp"$(INTDIR)\FilterText.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\FilterText.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /dll /incremental:no /pdb:"$(OUTDIR)\FilterText.pdb" /machine:I386 /out:"$(OUTDIR)\FilterText.dll" /implib:"$(OUTDIR)\FilterText.lib" 
LINK32_OBJS= \
	"$(INTDIR)\FilterText.obj" \
	"$(INTDIR)\Regexp.obj" \
	"$(INTDIR)\StdAfx.obj"

"$(OUTDIR)\FilterText.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# 开始自定义宏
OutDir=.\Release
# 结束自定义宏

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\FilterText.dll"
   md ..\..\..\bin\client\release\
	copy Release\FilterText.dll ..\..\..\bin\Client\FilterText.dll
	copy Release\FilterText.dll ..\..\..\bin\Client\release\FilterText.dll
	copy Release\FilterText.dll ..\..\..\bin\server\FilterText.dll
	copy Release\FilterText.dll ..\..\..\bin\server\release\FilterText.dll
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "FilterText - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# 开始自定义宏
OutDir=.\Debug
# 结束自定义宏

ALL : "$(OUTDIR)\FilterText.dll"


CLEAN :
	-@erase "$(INTDIR)\FilterText.obj"
	-@erase "$(INTDIR)\FilterText.pch"
	-@erase "$(INTDIR)\Regexp.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\FilterText.dll"
	-@erase "$(OUTDIR)\FilterText.exp"
	-@erase "$(OUTDIR)\FilterText.ilk"
	-@erase "$(OUTDIR)\FilterText.lib"
	-@erase "$(OUTDIR)\FilterText.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "FILTERTEXT_EXPORTS" /Fp"$(INTDIR)\FilterText.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\FilterText.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\FilterText.pdb" /debug /machine:I386 /out:"$(OUTDIR)\FilterText.dll" /implib:"$(OUTDIR)\FilterText.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\FilterText.obj" \
	"$(INTDIR)\Regexp.obj" \
	"$(INTDIR)\StdAfx.obj"

"$(OUTDIR)\FilterText.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# 开始自定义宏
OutDir=.\Debug
# 结束自定义宏

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\FilterText.dll"
   md ..\..\..\bin\client\debug
	copy debug\FilterText.dll ..\..\..\bin\Client\FilterText.dll
	copy debug\FilterText.dll ..\..\..\bin\Client\debug\FilterText.dll
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
!IF EXISTS("FilterText.dep")
!INCLUDE "FilterText.dep"
!ELSE 
!MESSAGE Warning: cannot find "FilterText.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "FilterText - Win32 Release" || "$(CFG)" == "FilterText - Win32 Debug"
SOURCE=.\FilterText.cpp

"$(INTDIR)\FilterText.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\FilterText.pch"


SOURCE=.\Regexp.cpp

"$(INTDIR)\Regexp.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\FilterText.pch"


SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "FilterText - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "FILTERTEXT_EXPORTS" /Fp"$(INTDIR)\FilterText.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\FilterText.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "FilterText - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "FILTERTEXT_EXPORTS" /Fp"$(INTDIR)\FilterText.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\FilterText.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 


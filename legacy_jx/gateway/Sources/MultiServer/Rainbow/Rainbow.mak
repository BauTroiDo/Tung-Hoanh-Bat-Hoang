# Microsoft Developer Studio Generated NMAKE File, Based on Rainbow.dsp
!IF $(CFG)" == "
CFG=Rainbow - Win32 Debug
!MESSAGE No configuration specified. Defaulting to Rainbow - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Rainbow - Win32 Release" && "$(CFG)" != "Rainbow - Win32 Debug"
!MESSAGE 指定的配置 "$(CFG)" 无效.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Rainbow.mak" CFG="Rainbow - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Rainbow - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Rainbow - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "Rainbow - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# 开始自定义宏
OutDir=.\Release
# 结束自定义宏

ALL : "$(OUTDIR)\Rainbow.dll" "$(OUTDIR)\Rainbow.bsc"


CLEAN :
	-@erase "$(INTDIR)\ClientStage.obj"
	-@erase "$(INTDIR)\ClientStage.sbr"
	-@erase "$(INTDIR)\IClient.obj"
	-@erase "$(INTDIR)\IClient.sbr"
	-@erase "$(INTDIR)\Rainbow.obj"
	-@erase "$(INTDIR)\Rainbow.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\Rainbow.bsc"
	-@erase "$(OUTDIR)\Rainbow.dll"
	-@erase "$(OUTDIR)\Rainbow.exp"
	-@erase "$(OUTDIR)\Rainbow.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "..\Common" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "RAINBOW_EXPORTS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\Rainbow.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Rainbow.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\IClient.sbr" \
	"$(INTDIR)\Rainbow.sbr" \
	"$(INTDIR)\ClientStage.sbr"

"$(OUTDIR)\Rainbow.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=/nologo /dll /incremental:no /pdb:"$(OUTDIR)\Rainbow.pdb" /machine:I386 /def:".\Rainbow.def" /out:"$(OUTDIR)\Rainbow.dll" /implib:"$(OUTDIR)\Rainbow.lib" /OPT:NOREF 
DEF_FILE= \
	".\Rainbow.def"
LINK32_OBJS= \
	"$(INTDIR)\IClient.obj" \
	"$(INTDIR)\Rainbow.obj" \
	"$(INTDIR)\ClientStage.obj" \
	"..\..\..\Lib\release\common.lib"

"$(OUTDIR)\Rainbow.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# 开始自定义宏
OutDir=.\Release
# 结束自定义宏

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\Rainbow.dll" "$(OUTDIR)\Rainbow.bsc"
   md ..\..\..\bin\client\release\
	copy release\Rainbow.dll ..\..\..\bin\client\Rainbow.dll
	copy release\Rainbow.dll ..\..\..\bin\client\release\Rainbow.dll
	copy release\Rainbow.pdb ..\..\..\bin\client\release\Rainbow.pdb
	copy release\Rainbow.map ..\..\..\bin\client\release\Rainbow.map
	md ..\..\..\bin\server\release\
	copy release\Rainbow.dll ..\..\..\bin\server\Rainbow.dll
	copy release\Rainbow.dll ..\..\..\bin\server\release\Rainbow.dll
	copy release\Rainbow.pdb ..\..\..\bin\server\release\Rainbow.pdb
	copy release\Rainbow.map ..\..\..\bin\server\release\Rainbow.map
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "Rainbow - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# 开始自定义宏
OutDir=.\Debug
# 结束自定义宏

ALL : "$(OUTDIR)\Rainbow.dll"


CLEAN :
	-@erase "$(INTDIR)\ClientStage.obj"
	-@erase "$(INTDIR)\IClient.obj"
	-@erase "$(INTDIR)\Rainbow.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\Rainbow.dll"
	-@erase "$(OUTDIR)\Rainbow.exp"
	-@erase "$(OUTDIR)\Rainbow.ilk"
	-@erase "$(OUTDIR)\Rainbow.lib"
	-@erase "$(OUTDIR)\Rainbow.map"
	-@erase "$(OUTDIR)\Rainbow.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\Common" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "RAINBOW_EXPORTS" /Fp"$(INTDIR)\Rainbow.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Rainbow.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\Rainbow.pdb" /map:"$(INTDIR)\Rainbow.map" /debug /machine:I386 /def:".\Rainbow.def" /out:"$(OUTDIR)\Rainbow.dll" /implib:"$(OUTDIR)\Rainbow.lib" /pdbtype:sept 
DEF_FILE= \
	".\Rainbow.def"
LINK32_OBJS= \
	"$(INTDIR)\IClient.obj" \
	"$(INTDIR)\Rainbow.obj" \
	"$(INTDIR)\ClientStage.obj" \
	"..\..\..\Lib\debug\common.lib"

"$(OUTDIR)\Rainbow.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# 开始自定义宏
OutDir=.\Debug
# 结束自定义宏

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\Rainbow.dll"
   md ..\..\..\bin\client\debug\
	copy debug\Rainbow.dll ..\..\..\..\bin\client\Rainbow.dll
	copy debug\Rainbow.dll ..\..\..\..\bin\client\debug\Rainbow.dll
	md ..\..\..\bin\server\debug\
	copy debug\Rainbow.dll ..\..\..\..\bin\server\Rainbow.dll
	copy debug\Rainbow.dll ..\..\..\..\bin\server\debug\Rainbow.dll
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
!IF EXISTS("Rainbow.dep")
!INCLUDE "Rainbow.dep"
!ELSE 
!MESSAGE Warning: cannot find "Rainbow.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "Rainbow - Win32 Release" || "$(CFG)" == "Rainbow - Win32 Debug"
SOURCE=.\IClient.cpp

!IF  "$(CFG)" == "Rainbow - Win32 Release"


"$(INTDIR)\IClient.obj"	"$(INTDIR)\IClient.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Rainbow - Win32 Debug"


"$(INTDIR)\IClient.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Rainbow.cpp

!IF  "$(CFG)" == "Rainbow - Win32 Release"


"$(INTDIR)\Rainbow.obj"	"$(INTDIR)\Rainbow.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Rainbow - Win32 Debug"


"$(INTDIR)\Rainbow.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\ClientStage.cpp

!IF  "$(CFG)" == "Rainbow - Win32 Release"


"$(INTDIR)\ClientStage.obj"	"$(INTDIR)\ClientStage.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Rainbow - Win32 Debug"


"$(INTDIR)\ClientStage.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 


!ENDIF 


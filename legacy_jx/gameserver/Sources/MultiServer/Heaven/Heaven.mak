# Microsoft Developer Studio Generated NMAKE File, Based on Heaven.dsp
!IF $(CFG)" == "
CFG=Heaven - Win32 Debug
!MESSAGE No configuration specified. Defaulting to Heaven - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Heaven - Win32 Release" && "$(CFG)" != "Heaven - Win32 Debug"
!MESSAGE 指定的配置 "$(CFG)" 无效.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Heaven.mak" CFG="Heaven - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Heaven - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Heaven - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "Heaven - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# 开始自定义宏
OutDir=.\Release
# 结束自定义宏

ALL : "$(OUTDIR)\Heaven.dll"


CLEAN :
	-@erase "$(INTDIR)\Heaven.obj"
	-@erase "$(INTDIR)\IServer.obj"
	-@erase "$(INTDIR)\ServerStage.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\Heaven.dll"
	-@erase "$(OUTDIR)\Heaven.exp"
	-@erase "$(OUTDIR)\Heaven.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "..\Common" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "HEAVEN_EXPORTS" /Fp"$(INTDIR)\Heaven.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Heaven.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /dll /incremental:no /pdb:"$(OUTDIR)\Heaven.pdb" /machine:I386 /def:".\Heaven.def" /out:"$(OUTDIR)\Heaven.dll" /implib:"$(OUTDIR)\Heaven.lib" /OPT:NOREF 
DEF_FILE= \
	".\Heaven.def"
LINK32_OBJS= \
	"$(INTDIR)\Heaven.obj" \
	"$(INTDIR)\IServer.obj" \
	"$(INTDIR)\ServerStage.obj" \
	"..\..\..\Lib\release\common.lib"

"$(OUTDIR)\Heaven.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# 开始自定义宏
OutDir=.\Release
# 结束自定义宏

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\Heaven.dll"
   md ..\..\..\bin\client\release\
	copy release\heaven.dll ..\..\..\bin\client\heaven.dll
	copy release\heaven.dll ..\..\..\bin\client\release\heaven.dll
	copy release\heaven.pdb ..\..\..\bin\client\release\heaven.pdb
	copy release\heaven.map ..\..\..\bin\client\release\heaven.map
	md ..\..\..\bin\server\release\
	copy release\heaven.dll ..\..\..\bin\server\heaven.dll
	copy release\heaven.dll ..\..\..\bin\server\release\heaven.dll
	copy release\heaven.pdb ..\..\..\bin\server\release\heaven.pdb
	copy release\heaven.map ..\..\..\bin\server\release\heaven.map
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "Heaven - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# 开始自定义宏
OutDir=.\Debug
# 结束自定义宏

ALL : "$(OUTDIR)\Heaven.dll" "$(OUTDIR)\Heaven.bsc"


CLEAN :
	-@erase "$(INTDIR)\Heaven.obj"
	-@erase "$(INTDIR)\Heaven.sbr"
	-@erase "$(INTDIR)\IServer.obj"
	-@erase "$(INTDIR)\IServer.sbr"
	-@erase "$(INTDIR)\ServerStage.obj"
	-@erase "$(INTDIR)\ServerStage.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\Heaven.bsc"
	-@erase "$(OUTDIR)\Heaven.dll"
	-@erase "$(OUTDIR)\Heaven.exp"
	-@erase "$(OUTDIR)\Heaven.ilk"
	-@erase "$(OUTDIR)\Heaven.lib"
	-@erase "$(OUTDIR)\Heaven.map"
	-@erase "$(OUTDIR)\Heaven.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\Common" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "HEAVEN_EXPORTS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\Heaven.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Heaven.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\Heaven.sbr" \
	"$(INTDIR)\IServer.sbr" \
	"$(INTDIR)\ServerStage.sbr"

"$(OUTDIR)\Heaven.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\Heaven.pdb" /map:"$(INTDIR)\Heaven.map" /debug /machine:I386 /def:".\Heaven.def" /out:"$(OUTDIR)\Heaven.dll" /implib:"$(OUTDIR)\Heaven.lib" /pdbtype:sept 
DEF_FILE= \
	".\Heaven.def"
LINK32_OBJS= \
	"$(INTDIR)\Heaven.obj" \
	"$(INTDIR)\IServer.obj" \
	"$(INTDIR)\ServerStage.obj" \
	"..\..\..\Lib\debug\common.lib"

"$(OUTDIR)\Heaven.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# 开始自定义宏
OutDir=.\Debug
# 结束自定义宏

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\Heaven.dll" "$(OUTDIR)\Heaven.bsc"
   md ..\..\..\bin\client\debug\
	copy debug\heaven.dll ..\..\..\..\bin\client\heaven.dll
	copy debug\heaven.dll ..\..\..\..\bin\client\debug\heaven.dll
	md ..\..\..\bin\server\debug\
	copy debug\heaven.dll ..\..\..\..\bin\server\heaven.dll
	copy debug\heaven.dll ..\..\..\..\bin\server\debug\heaven.dll
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
!IF EXISTS("Heaven.dep")
!INCLUDE "Heaven.dep"
!ELSE 
!MESSAGE Warning: cannot find "Heaven.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "Heaven - Win32 Release" || "$(CFG)" == "Heaven - Win32 Debug"
SOURCE=.\Heaven.cpp

!IF  "$(CFG)" == "Heaven - Win32 Release"


"$(INTDIR)\Heaven.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Heaven - Win32 Debug"


"$(INTDIR)\Heaven.obj"	"$(INTDIR)\Heaven.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\IServer.cpp

!IF  "$(CFG)" == "Heaven - Win32 Release"


"$(INTDIR)\IServer.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Heaven - Win32 Debug"


"$(INTDIR)\IServer.obj"	"$(INTDIR)\IServer.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\ServerStage.cpp

!IF  "$(CFG)" == "Heaven - Win32 Release"


"$(INTDIR)\ServerStage.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Heaven - Win32 Debug"


"$(INTDIR)\ServerStage.obj"	"$(INTDIR)\ServerStage.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 


!ENDIF 


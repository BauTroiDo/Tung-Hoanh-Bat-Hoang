# Microsoft Developer Studio Generated NMAKE File, Based on FilterText_StaticLib.dsp
!IF $(CFG)" == "
CFG=FilterText_StaticLib - Win32 Debug
!MESSAGE No configuration specified. Defaulting to FilterText_StaticLib - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "FilterText_StaticLib - Win32 Release" && "$(CFG)" != "FilterText_StaticLib - Win32 Debug"
!MESSAGE 指定的配置 "$(CFG)" 无效.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "FilterText_StaticLib.mak" CFG="FilterText_StaticLib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "FilterText_StaticLib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "FilterText_StaticLib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF $(OS)" == "Windows_NT
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "FilterText_StaticLib - Win32 Release"

OUTDIR=.\FilterText_StaticLib___Win32_Release
INTDIR=.\FilterText_StaticLib___Win32_Release
# 开始自定义宏
OutDir=.\FilterText_StaticLib___Win32_Release
# 结束自定义宏

ALL : "$(OUTDIR)\FilterText_StaticLib.lib"


CLEAN :
	-@erase "$(INTDIR)\FilterText.obj"
	-@erase "$(INTDIR)\Regexp.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\FilterText_StaticLib.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /Zi /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "FILTERTEXT_STATICLIB" /Fp"$(INTDIR)\FilterText_StaticLib.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\FilterText_StaticLib.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\FilterText_StaticLib.lib" 
LIB32_OBJS= \
	"$(INTDIR)\FilterText.obj" \
	"$(INTDIR)\Regexp.obj"

"$(OUTDIR)\FilterText_StaticLib.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# 开始自定义宏
OutDir=.\FilterText_StaticLib___Win32_Release
# 结束自定义宏

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\FilterText_StaticLib.lib"
   md ..\..\lib\release
	copy FilterText_StaticLib___Win32_Release\FilterText_StaticLib.lib ..\..\lib\release\FilterText_StaticLib.lib
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "FilterText_StaticLib - Win32 Debug"

OUTDIR=.\FilterText_StaticLib___Win32_Debug
INTDIR=.\FilterText_StaticLib___Win32_Debug
# 开始自定义宏
OutDir=.\FilterText_StaticLib___Win32_Debug
# 结束自定义宏

ALL : "$(OUTDIR)\FilterText_StaticLib.lib"


CLEAN :
	-@erase "$(INTDIR)\FilterText.obj"
	-@erase "$(INTDIR)\Regexp.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\FilterText_StaticLib.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "FILTERTEXT_STATICLIB" /Fp"$(INTDIR)\FilterText_StaticLib.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\FilterText_StaticLib.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\FilterText_StaticLib.lib" 
LIB32_OBJS= \
	"$(INTDIR)\FilterText.obj" \
	"$(INTDIR)\Regexp.obj"

"$(OUTDIR)\FilterText_StaticLib.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# 开始自定义宏
OutDir=.\FilterText_StaticLib___Win32_Debug
# 结束自定义宏

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\FilterText_StaticLib.lib"
   md ..\..\lib\debug
	copy FilterText_StaticLib___Win32_Debug\FilterText_StaticLib.lib ..\..\lib\debug\FilterText_StaticLib.lib
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
!IF EXISTS("FilterText_StaticLib.dep")
!INCLUDE "FilterText_StaticLib.dep"
!ELSE 
!MESSAGE Warning: cannot find "FilterText_StaticLib.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "FilterText_StaticLib - Win32 Release" || "$(CFG)" == "FilterText_StaticLib - Win32 Debug"
SOURCE=.\FilterText.cpp

"$(INTDIR)\FilterText.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Regexp.cpp

"$(INTDIR)\Regexp.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 


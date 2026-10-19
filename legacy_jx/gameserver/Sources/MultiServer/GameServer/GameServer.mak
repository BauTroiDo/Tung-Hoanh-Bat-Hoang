# Microsoft Developer Studio Generated NMAKE File, Based on GameServer.dsp
!IF $(CFG)" == "
CFG=GameServer - Win32 Debug
!MESSAGE No configuration specified. Defaulting to GameServer - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "GameServer - Win32 Release" && "$(CFG)" != "GameServer - Win32 Debug"
!MESSAGE 指定的配置 "$(CFG)" 无效.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "GameServer.mak" CFG="GameServer - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "GameServer - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "GameServer - Win32 Debug" (based on "Win32 (x86) Console Application")
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

!IF  "$(CFG)" == "GameServer - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# 开始自定义宏
OutDir=.\Release
# 结束自定义宏

ALL : "$(OUTDIR)\GameServer.exe"


CLEAN :
	-@erase "$(INTDIR)\GameServer.obj"
	-@erase "$(INTDIR)\GameServer.pch"
	-@erase "$(INTDIR)\GameServer.res"
	-@erase "$(INTDIR)\KSOServer.obj"
	-@erase "$(INTDIR)\KTransferUnit.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\GameServer.exe"
	-@erase "$(OUTDIR)\GameServer.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

F90=df.exe
CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "..\Common\\" /I "..\Rainbow\Interface" /I "..\Heaven\Interface" /I "..\..\roledbserver" /I "..\..\..\lib\\" /I "..\..\..\Headers\\" /I "..\..\..\Sources\Engine\src" /I "..\..\..\Sources\Engine\include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D _WIN32_WINNT=0x0502 /Fp"$(INTDIR)\GameServer.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /Gs /c 
RSC_PROJ=/l 0x804 /fo"$(INTDIR)\GameServer.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\GameServer.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=Netapi32.lib ms32.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\GameServer.pdb" /machine:I386 /nodefaultlib:"LIBC.lib" /out:"$(OUTDIR)\GameServer.exe" /opt:noref 
LINK32_OBJS= \
	"$(INTDIR)\GameServer.obj" \
	"$(INTDIR)\KSOServer.obj" \
	"$(INTDIR)\KTransferUnit.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\GameServer.res" \
	"..\..\..\Lib\release\engine.lib" \
	"..\..\..\Lib\release\CoreServer.lib" \
	"..\..\..\Lib\release\common.lib" \
	"..\..\Library\LuaLib\Release\LuaLibDll.lib"

"$(OUTDIR)\GameServer.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# 开始自定义宏
OutDir=.\Release
# 结束自定义宏

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\GameServer.exe"
   md ..\..\..\..\bin\server\release\
	copy Release\GameServer.exe ..\..\..\bin\Server\GameServer.exe
	copy Release\GameServer.exe ..\..\..\bin\Server\release\GameServer.exe
	copy Release\GameServer.pdb ..\..\..\bin\Server\release\GameServer.pdb
	copy Release\GameServer.map ..\..\..\bin\Server\release\GameServer.map
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "GameServer - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# 开始自定义宏
OutDir=.\Debug
# 结束自定义宏

ALL : "$(OUTDIR)\GameServer.exe" "$(OUTDIR)\GameServer.bsc"


CLEAN :
	-@erase "$(INTDIR)\GameServer.obj"
	-@erase "$(INTDIR)\GameServer.pch"
	-@erase "$(INTDIR)\GameServer.res"
	-@erase "$(INTDIR)\GameServer.sbr"
	-@erase "$(INTDIR)\KSOServer.obj"
	-@erase "$(INTDIR)\KSOServer.sbr"
	-@erase "$(INTDIR)\KTransferUnit.obj"
	-@erase "$(INTDIR)\KTransferUnit.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\GameServer.bsc"
	-@erase "$(OUTDIR)\GameServer.exe"
	-@erase "$(OUTDIR)\GameServer.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

F90=df.exe
CPP_PROJ=/nologo /MT /W3 /Gm /GX /Zi /Od /I "..\Common\\" /I "..\Rainbow\Interface" /I "..\Heaven\Interface" /I "..\..\roledbserver" /I "..\..\..\lib\\" /I "..\..\..\Headers\\" /I "..\..\..\Sources\Engine\src" /I "..\..\..\Sources\Engine\include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D _WIN32_WINNT=0x0400 /FR"$(INTDIR)\\" /Fp"$(INTDIR)\GameServer.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
RSC_PROJ=/l 0x804 /fo"$(INTDIR)\GameServer.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\GameServer.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\GameServer.sbr" \
	"$(INTDIR)\KSOServer.sbr" \
	"$(INTDIR)\KTransferUnit.sbr" \
	"$(INTDIR)\StdAfx.sbr"

"$(OUTDIR)\GameServer.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=Engine.lib Common.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\GameServer.pdb" /machine:I386 /out:"$(OUTDIR)\GameServer.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\GameServer.obj" \
	"$(INTDIR)\KSOServer.obj" \
	"$(INTDIR)\KTransferUnit.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\GameServer.res" \
	"..\..\..\Lib\debug\engine.lib" \
	"..\..\..\Lib\debug\CoreServer.lib" \
	"..\..\..\Lib\debug\common.lib" \
	"..\..\Library\LuaLib\Release\LuaLibDll.lib"

"$(OUTDIR)\GameServer.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# 开始自定义宏
OutDir=.\Debug
# 结束自定义宏

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\GameServer.exe" "$(OUTDIR)\GameServer.bsc"
   md ..\..\..\..\bin\server\debug
	copy debug\GameServer.exe ..\..\..\..\bin\Server\GameServer.exe
	copy debug\GameServer.exe ..\..\..\..\bin\Server\debug\GameServer.exe
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
!IF EXISTS("GameServer.dep")
!INCLUDE "GameServer.dep"
!ELSE 
!MESSAGE Warning: cannot find "GameServer.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "GameServer - Win32 Release" || "$(CFG)" == "GameServer - Win32 Debug"
SOURCE=.\GameServer.cpp

!IF  "$(CFG)" == "GameServer - Win32 Release"


"$(INTDIR)\GameServer.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\GameServer.pch"


!ELSEIF  "$(CFG)" == "GameServer - Win32 Debug"


"$(INTDIR)\GameServer.obj"	"$(INTDIR)\GameServer.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\GameServer.pch"


!ENDIF 

SOURCE=.\KSOServer.cpp

!IF  "$(CFG)" == "GameServer - Win32 Release"


"$(INTDIR)\KSOServer.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\GameServer.pch"


!ELSEIF  "$(CFG)" == "GameServer - Win32 Debug"


"$(INTDIR)\KSOServer.obj"	"$(INTDIR)\KSOServer.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\GameServer.pch"


!ENDIF 

SOURCE=.\KTransferUnit.cpp

!IF  "$(CFG)" == "GameServer - Win32 Release"


"$(INTDIR)\KTransferUnit.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\GameServer.pch"


!ELSEIF  "$(CFG)" == "GameServer - Win32 Debug"


"$(INTDIR)\KTransferUnit.obj"	"$(INTDIR)\KTransferUnit.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\GameServer.pch"


!ENDIF 

SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "GameServer - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\Common\\" /I "..\Rainbow\Interface" /I "..\Heaven\Interface" /I "..\..\roledbserver" /I "..\..\..\lib\\" /I "..\..\..\Headers\\" /I "..\..\..\Sources\Engine\src" /I "..\..\..\Sources\Engine\include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D _WIN32_WINNT=0x0502 /Fp"$(INTDIR)\GameServer.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /Gs /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\GameServer.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "GameServer - Win32 Debug"

CPP_SWITCHES=/nologo /MT /W3 /Gm /GX /Zi /Od /I "..\Common\\" /I "..\Rainbow\Interface" /I "..\Heaven\Interface" /I "..\..\roledbserver" /I "..\..\..\lib\\" /I "..\..\..\Headers\\" /I "..\..\..\Sources\Engine\src" /I "..\..\..\Sources\Engine\include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D _WIN32_WINNT=0x0400 /FR"$(INTDIR)\\" /Fp"$(INTDIR)\GameServer.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr"	"$(INTDIR)\GameServer.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\GameServer.rc

"$(INTDIR)\GameServer.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 


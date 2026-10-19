# Microsoft Developer Studio Generated NMAKE File, Based on Goddess.dsp
!IF $(CFG)" == "
CFG=Goddess - Win32 Debug
!MESSAGE No configuration specified. Defaulting to Goddess - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Goddess - Win32 Release" && "$(CFG)" != "Goddess - Win32 Debug"
!MESSAGE 指定的配置 "$(CFG)" 无效.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Goddess.mak" CFG="Goddess - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Goddess - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Goddess - Win32 Debug" (based on "Win32 (x86) Application")
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

!IF  "$(CFG)" == "Goddess - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# 开始自定义宏
OutDir=.\Release
# 结束自定义宏

ALL : "$(OUTDIR)\Goddess.exe" "$(OUTDIR)\Goddess.bsc"


CLEAN :
	-@erase "$(INTDIR)\ClientNode.obj"
	-@erase "$(INTDIR)\ClientNode.sbr"
	-@erase "$(INTDIR)\DBBackup.obj"
	-@erase "$(INTDIR)\DBBackup.sbr"
	-@erase "$(INTDIR)\DBDumpLoad.obj"
	-@erase "$(INTDIR)\DBDumpLoad.sbr"
	-@erase "$(INTDIR)\DBTable.obj"
	-@erase "$(INTDIR)\DBTable.sbr"
	-@erase "$(INTDIR)\FilterTextLib.obj"
	-@erase "$(INTDIR)\FilterTextLib.sbr"
	-@erase "$(INTDIR)\Goddess.obj"
	-@erase "$(INTDIR)\Goddess.pch"
	-@erase "$(INTDIR)\Goddess.res"
	-@erase "$(INTDIR)\Goddess.sbr"
	-@erase "$(INTDIR)\IDBRoleServer.obj"
	-@erase "$(INTDIR)\IDBRoleServer.sbr"
	-@erase "$(INTDIR)\LogFile.obj"
	-@erase "$(INTDIR)\LogFile.sbr"
	-@erase "$(INTDIR)\RoleNameFilter.obj"
	-@erase "$(INTDIR)\RoleNameFilter.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\Goddess.bsc"
	-@erase "$(OUTDIR)\Goddess.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "..\Common\\" /I "..\Rainbow\Interface" /I "..\Heaven\Interface" /I "..\..\..\lib" /I "..\..\..\Headers" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\Goddess.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /Gs /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x804 /fo"$(INTDIR)\Goddess.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Goddess.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\ClientNode.sbr" \
	"$(INTDIR)\DBBackup.sbr" \
	"$(INTDIR)\DBDumpLoad.sbr" \
	"$(INTDIR)\DBTable.sbr" \
	"$(INTDIR)\FilterTextLib.sbr" \
	"$(INTDIR)\Goddess.sbr" \
	"$(INTDIR)\IDBRoleServer.sbr" \
	"$(INTDIR)\LogFile.sbr" \
	"$(INTDIR)\RoleNameFilter.sbr" \
	"$(INTDIR)\StdAfx.sbr"

"$(OUTDIR)\Goddess.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\Goddess.pdb" /machine:I386 /out:"$(OUTDIR)\Goddess.exe" /OPT:NOREF 
LINK32_OBJS= \
	"$(INTDIR)\ClientNode.obj" \
	"$(INTDIR)\DBBackup.obj" \
	"$(INTDIR)\DBDumpLoad.obj" \
	"$(INTDIR)\DBTable.obj" \
	"$(INTDIR)\FilterTextLib.obj" \
	"$(INTDIR)\Goddess.obj" \
	"$(INTDIR)\IDBRoleServer.obj" \
	"$(INTDIR)\LogFile.obj" \
	"$(INTDIR)\RoleNameFilter.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\Goddess.res" \
	"..\..\..\Lib\release\common.lib" \
	"..\..\..\Lib\libdb41s.lib"

"$(OUTDIR)\Goddess.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# 开始自定义宏
OutDir=.\Release
# 结束自定义宏

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\Goddess.exe" "$(OUTDIR)\Goddess.bsc"
   md ..\..\..\..\bin\client\release\
	copy release\Goddess.exe ..\..\..\bin\server\release\Goddess.exe
	copy release\Goddess.exe ..\..\..\bin\server\Goddess.exe
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "Goddess - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# 开始自定义宏
OutDir=.\Debug
# 结束自定义宏

ALL : "$(OUTDIR)\Goddess.exe" "$(OUTDIR)\Goddess.bsc"


CLEAN :
	-@erase "$(INTDIR)\ClientNode.obj"
	-@erase "$(INTDIR)\ClientNode.sbr"
	-@erase "$(INTDIR)\DBBackup.obj"
	-@erase "$(INTDIR)\DBBackup.sbr"
	-@erase "$(INTDIR)\DBDumpLoad.obj"
	-@erase "$(INTDIR)\DBDumpLoad.sbr"
	-@erase "$(INTDIR)\DBTable.obj"
	-@erase "$(INTDIR)\DBTable.sbr"
	-@erase "$(INTDIR)\FilterTextLib.obj"
	-@erase "$(INTDIR)\FilterTextLib.sbr"
	-@erase "$(INTDIR)\Goddess.obj"
	-@erase "$(INTDIR)\Goddess.pch"
	-@erase "$(INTDIR)\Goddess.res"
	-@erase "$(INTDIR)\Goddess.sbr"
	-@erase "$(INTDIR)\IDBRoleServer.obj"
	-@erase "$(INTDIR)\IDBRoleServer.sbr"
	-@erase "$(INTDIR)\LogFile.obj"
	-@erase "$(INTDIR)\LogFile.sbr"
	-@erase "$(INTDIR)\RoleNameFilter.obj"
	-@erase "$(INTDIR)\RoleNameFilter.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\Goddess.bsc"
	-@erase "$(OUTDIR)\Goddess.exe"
	-@erase "$(OUTDIR)\Goddess.ilk"
	-@erase "$(OUTDIR)\Goddess.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\Common\\" /I "..\Rainbow\Interface" /I "..\Heaven\Interface" /I "..\..\..\lib" /I "..\..\..\Headers" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\Goddess.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x804 /fo"$(INTDIR)\Goddess.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Goddess.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\ClientNode.sbr" \
	"$(INTDIR)\DBBackup.sbr" \
	"$(INTDIR)\DBDumpLoad.sbr" \
	"$(INTDIR)\DBTable.sbr" \
	"$(INTDIR)\FilterTextLib.sbr" \
	"$(INTDIR)\Goddess.sbr" \
	"$(INTDIR)\IDBRoleServer.sbr" \
	"$(INTDIR)\LogFile.sbr" \
	"$(INTDIR)\RoleNameFilter.sbr" \
	"$(INTDIR)\StdAfx.sbr"

"$(OUTDIR)\Goddess.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\Goddess.pdb" /debug /machine:I386 /out:"$(OUTDIR)\Goddess.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\ClientNode.obj" \
	"$(INTDIR)\DBBackup.obj" \
	"$(INTDIR)\DBDumpLoad.obj" \
	"$(INTDIR)\DBTable.obj" \
	"$(INTDIR)\FilterTextLib.obj" \
	"$(INTDIR)\Goddess.obj" \
	"$(INTDIR)\IDBRoleServer.obj" \
	"$(INTDIR)\LogFile.obj" \
	"$(INTDIR)\RoleNameFilter.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\Goddess.res" \
	"..\..\..\Lib\debug\common.lib" \
	"..\..\..\Lib\libdb41s.lib"

"$(OUTDIR)\Goddess.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# 开始自定义宏
OutDir=.\Debug
# 结束自定义宏

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\Goddess.exe" "$(OUTDIR)\Goddess.bsc"
   md ..\..\..\..\bin\client\debug\
	copy debug\Goddess.exe ..\..\..\..\bin\Client\Goddess.exe
	copy debug\Goddess.exe ..\..\..\..\bin\Client\debug\Goddess.exe
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
!IF EXISTS("Goddess.dep")
!INCLUDE "Goddess.dep"
!ELSE 
!MESSAGE Warning: cannot find "Goddess.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "Goddess - Win32 Release" || "$(CFG)" == "Goddess - Win32 Debug"
SOURCE=.\ClientNode.cpp

"$(INTDIR)\ClientNode.obj"	"$(INTDIR)\ClientNode.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Goddess.pch"


SOURCE=.\DBBackup.cpp

"$(INTDIR)\DBBackup.obj"	"$(INTDIR)\DBBackup.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Goddess.pch"


SOURCE=.\DBDumpLoad.cpp

"$(INTDIR)\DBDumpLoad.obj"	"$(INTDIR)\DBDumpLoad.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Goddess.pch"


SOURCE=.\DBTable.cpp

"$(INTDIR)\DBTable.obj"	"$(INTDIR)\DBTable.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Goddess.pch"


SOURCE=.\FilterTextLib.cpp

"$(INTDIR)\FilterTextLib.obj"	"$(INTDIR)\FilterTextLib.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Goddess.pch"


SOURCE=.\Goddess.cpp

"$(INTDIR)\Goddess.obj"	"$(INTDIR)\Goddess.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Goddess.pch"


SOURCE=.\IDBRoleServer.cpp

"$(INTDIR)\IDBRoleServer.obj"	"$(INTDIR)\IDBRoleServer.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Goddess.pch"


SOURCE=.\LogFile.cpp

"$(INTDIR)\LogFile.obj"	"$(INTDIR)\LogFile.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Goddess.pch"


SOURCE=.\RoleNameFilter.cpp

"$(INTDIR)\RoleNameFilter.obj"	"$(INTDIR)\RoleNameFilter.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Goddess.pch"


SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "Goddess - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\Common\\" /I "..\Rainbow\Interface" /I "..\Heaven\Interface" /I "..\..\..\lib" /I "..\..\..\Headers" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\Goddess.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /Gs /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr"	"$(INTDIR)\Goddess.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Goddess - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\Common\\" /I "..\Rainbow\Interface" /I "..\Heaven\Interface" /I "..\..\..\lib" /I "..\..\..\Headers" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\Goddess.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr"	"$(INTDIR)\Goddess.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\Goddess.rc

"$(INTDIR)\Goddess.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 


# Microsoft Developer Studio Generated NMAKE File, Based on Sword3PaySys.dsp
!IF $(CFG)" == "
CFG=Sword3PaySys - Win32 Debug
!MESSAGE No configuration specified. Defaulting to Sword3PaySys - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Sword3PaySys - Win32 Release" && "$(CFG)" != "Sword3PaySys - Win32 Debug"
!MESSAGE 指定的配置 "$(CFG)" 无效.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Sword3PaySys.mak" CFG="Sword3PaySys - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Sword3PaySys - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Sword3PaySys - Win32 Debug" (based on "Win32 (x86) Application")
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

!IF  "$(CFG)" == "Sword3PaySys - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# 开始自定义宏
OutDir=.\Release
# 结束自定义宏

ALL : "$(OUTDIR)\Sword3PaySys.exe" "$(OUTDIR)\Sword3PaySys.pch"


CLEAN :
	-@erase "$(INTDIR)\GlobalFun.obj"
	-@erase "$(INTDIR)\KThread.obj"
	-@erase "$(INTDIR)\LogFile.obj"
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\S3P_MSSQLServer_Result.obj"
	-@erase "$(INTDIR)\S3PAccount.obj"
	-@erase "$(INTDIR)\S3PDB_MSSQLServer_Connection.obj"
	-@erase "$(INTDIR)\S3PDBConnectionPool.obj"
	-@erase "$(INTDIR)\S3PDBConVBC.obj"
	-@erase "$(INTDIR)\S3PDBSocketPool.obj"
	-@erase "$(INTDIR)\S3PResultVBC.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\Sword3PaySys.pch"
	-@erase "$(INTDIR)\Sword3PaySys.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\Sword3PaySys.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "..\..\..\Headers" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fp"$(INTDIR)\Sword3PaySys.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /Gs /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x804 /fo"$(INTDIR)\Sword3PaySys.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Sword3PaySys.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=advapi32.lib  Ws2_32.lib /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\Sword3PaySys.pdb" /machine:I386 /out:"$(OUTDIR)\Sword3PaySys.exe" 
LINK32_OBJS= \
	"$(INTDIR)\GlobalFun.obj" \
	"$(INTDIR)\KThread.obj" \
	"$(INTDIR)\LogFile.obj" \
	"$(INTDIR)\main.obj" \
	"$(INTDIR)\S3P_MSSQLServer_Result.obj" \
	"$(INTDIR)\S3PAccount.obj" \
	"$(INTDIR)\S3PDB_MSSQLServer_Connection.obj" \
	"$(INTDIR)\S3PDBConnectionPool.obj" \
	"$(INTDIR)\S3PDBConVBC.obj" \
	"$(INTDIR)\S3PDBSocketPool.obj" \
	"$(INTDIR)\S3PResultVBC.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\Sword3PaySys.res" \
	"..\..\..\Lib\release\engine.lib"

"$(OUTDIR)\Sword3PaySys.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# 开始自定义宏
OutDir=.\Release
# 结束自定义宏

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\Sword3PaySys.exe" "$(OUTDIR)\Sword3PaySys.pch"
   md ..\..\..\..\bin\client\release\
	copy release\Sword3PaySys.exe ..\..\..\bin\server\release\Sword3PaySys.exe
	copy release\Sword3PaySys.exe ..\..\..\bin\server\Sword3PaySys.exe
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "Sword3PaySys - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# 开始自定义宏
OutDir=.\Debug
# 结束自定义宏

ALL : "$(OUTDIR)\Sword3PaySys.exe" "$(OUTDIR)\Sword3PaySys.pch"


CLEAN :
	-@erase "$(INTDIR)\GlobalFun.obj"
	-@erase "$(INTDIR)\KThread.obj"
	-@erase "$(INTDIR)\LogFile.obj"
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\S3P_MSSQLServer_Result.obj"
	-@erase "$(INTDIR)\S3PAccount.obj"
	-@erase "$(INTDIR)\S3PDB_MSSQLServer_Connection.obj"
	-@erase "$(INTDIR)\S3PDBConnectionPool.obj"
	-@erase "$(INTDIR)\S3PDBConVBC.obj"
	-@erase "$(INTDIR)\S3PDBSocketPool.obj"
	-@erase "$(INTDIR)\S3PResultVBC.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\Sword3PaySys.pch"
	-@erase "$(INTDIR)\Sword3PaySys.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\Sword3PaySys.exe"
	-@erase "$(OUTDIR)\Sword3PaySys.ilk"
	-@erase "$(OUTDIR)\Sword3PaySys.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\..\..\Headers" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Fp"$(INTDIR)\Sword3PaySys.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x804 /fo"$(INTDIR)\Sword3PaySys.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Sword3PaySys.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib Ws2_32.lib /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\Sword3PaySys.pdb" /debug /machine:I386 /out:"$(OUTDIR)\Sword3PaySys.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\GlobalFun.obj" \
	"$(INTDIR)\KThread.obj" \
	"$(INTDIR)\LogFile.obj" \
	"$(INTDIR)\main.obj" \
	"$(INTDIR)\S3P_MSSQLServer_Result.obj" \
	"$(INTDIR)\S3PAccount.obj" \
	"$(INTDIR)\S3PDB_MSSQLServer_Connection.obj" \
	"$(INTDIR)\S3PDBConnectionPool.obj" \
	"$(INTDIR)\S3PDBConVBC.obj" \
	"$(INTDIR)\S3PDBSocketPool.obj" \
	"$(INTDIR)\S3PResultVBC.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\Sword3PaySys.res" \
	"..\..\..\Lib\release\engine.lib"

"$(OUTDIR)\Sword3PaySys.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# 开始自定义宏
OutDir=.\Debug
# 结束自定义宏

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\Sword3PaySys.exe" "$(OUTDIR)\Sword3PaySys.pch"
   md ..\..\..\..\bin\client\debug\
	copy debug\Sword3PaySys.exe ..\..\..\..\bin\Client\Sword3PaySys.exe
	copy debug\Sword3PaySys.exe ..\..\..\..\bin\Client\debug\Sword3PaySys.exe
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
!IF EXISTS("Sword3PaySys.dep")
!INCLUDE "Sword3PaySys.dep"
!ELSE 
!MESSAGE Warning: cannot find "Sword3PaySys.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "Sword3PaySys - Win32 Release" || "$(CFG)" == "Sword3PaySys - Win32 Debug"
SOURCE=.\GlobalFun.cpp

"$(INTDIR)\GlobalFun.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\KThread.cpp

"$(INTDIR)\KThread.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\LogFile.cpp

"$(INTDIR)\LogFile.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\main.cpp

"$(INTDIR)\main.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\S3P_MSSQLServer_Result.cpp

"$(INTDIR)\S3P_MSSQLServer_Result.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\S3PAccount.cpp

"$(INTDIR)\S3PAccount.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\S3PDB_MSSQLServer_Connection.cpp

"$(INTDIR)\S3PDB_MSSQLServer_Connection.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\S3PDBConnectionPool.cpp

"$(INTDIR)\S3PDBConnectionPool.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\S3PDBConVBC.cpp

"$(INTDIR)\S3PDBConVBC.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\S3PDBSocketPool.cpp

"$(INTDIR)\S3PDBSocketPool.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\S3PResultVBC.cpp

"$(INTDIR)\S3PResultVBC.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "Sword3PaySys - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\..\..\Headers" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fp"$(INTDIR)\Sword3PaySys.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /Gs /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\Sword3PaySys.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Sword3PaySys - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\..\..\Headers" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Fp"$(INTDIR)\Sword3PaySys.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\Sword3PaySys.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\Sword3PaySys.rc

"$(INTDIR)\Sword3PaySys.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 


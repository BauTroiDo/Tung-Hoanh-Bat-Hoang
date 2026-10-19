# Microsoft Developer Studio Generated NMAKE File, Based on S3RelayServer.dsp
!IF $(CFG)" == "
CFG=S3RelayServer - Win32 Debug
!MESSAGE No configuration specified. Defaulting to S3RelayServer - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "S3RelayServer - Win32 Release" && "$(CFG)" != "S3RelayServer - Win32 Debug"
!MESSAGE 指定的配置 "$(CFG)" 无效.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "S3RelayServer.mak" CFG="S3RelayServer - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "S3RelayServer - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "S3RelayServer - Win32 Debug" (based on "Win32 (x86) Application")
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

!IF  "$(CFG)" == "S3RelayServer - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# 开始自定义宏
OutDir=.\Release
# 结束自定义宏

ALL : "$(OUTDIR)\S3RelayServer.exe" "$(OUTDIR)\S3RelayServer.pch" "$(OUTDIR)\S3RelayServer.bsc"


CLEAN :
	-@erase "$(INTDIR)\GlobalFun.obj"
	-@erase "$(INTDIR)\GlobalFun.sbr"
	-@erase "$(INTDIR)\KThread.obj"
	-@erase "$(INTDIR)\KThread.sbr"
	-@erase "$(INTDIR)\LogFile.obj"
	-@erase "$(INTDIR)\LogFile.sbr"
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\main.sbr"
	-@erase "$(INTDIR)\S3P_MSSQLServer_Result.obj"
	-@erase "$(INTDIR)\S3P_MSSQLServer_Result.sbr"
	-@erase "$(INTDIR)\S3PAccount.obj"
	-@erase "$(INTDIR)\S3PAccount.sbr"
	-@erase "$(INTDIR)\S3PDB_MSSQLServer_Connection.obj"
	-@erase "$(INTDIR)\S3PDB_MSSQLServer_Connection.sbr"
	-@erase "$(INTDIR)\S3PDBConnectionPool.obj"
	-@erase "$(INTDIR)\S3PDBConnectionPool.sbr"
	-@erase "$(INTDIR)\S3PDBConVBC.obj"
	-@erase "$(INTDIR)\S3PDBConVBC.sbr"
	-@erase "$(INTDIR)\S3PDBSocketPool.obj"
	-@erase "$(INTDIR)\S3PDBSocketPool.sbr"
	-@erase "$(INTDIR)\S3PResultVBC.obj"
	-@erase "$(INTDIR)\S3PResultVBC.sbr"
	-@erase "$(INTDIR)\S3RelayServer.pch"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\Sword3RelaySys.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\S3RelayServer.bsc"
	-@erase "$(OUTDIR)\S3RelayServer.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\S3RelayServer.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /Gs /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x804 /fo"$(INTDIR)\Sword3RelaySys.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\S3RelayServer.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\GlobalFun.sbr" \
	"$(INTDIR)\KThread.sbr" \
	"$(INTDIR)\LogFile.sbr" \
	"$(INTDIR)\main.sbr" \
	"$(INTDIR)\S3P_MSSQLServer_Result.sbr" \
	"$(INTDIR)\S3PAccount.sbr" \
	"$(INTDIR)\S3PDB_MSSQLServer_Connection.sbr" \
	"$(INTDIR)\S3PDBConnectionPool.sbr" \
	"$(INTDIR)\S3PDBConVBC.sbr" \
	"$(INTDIR)\S3PDBSocketPool.sbr" \
	"$(INTDIR)\S3PResultVBC.sbr" \
	"$(INTDIR)\StdAfx.sbr"

"$(OUTDIR)\S3RelayServer.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=advapi32.lib  Ws2_32.lib /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\S3RelayServer.pdb" /machine:I386 /out:"$(OUTDIR)\S3RelayServer.exe" 
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
	"$(INTDIR)\Sword3RelaySys.res"

"$(OUTDIR)\S3RelayServer.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# 开始自定义宏
OutDir=.\Release
# 结束自定义宏

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\S3RelayServer.exe" "$(OUTDIR)\S3RelayServer.pch" "$(OUTDIR)\S3RelayServer.bsc"
   md ..\..\..\..\bin\client\release\
	copy release\S3RelayServer.exe ..\..\..\bin\server\release\S3RelayServer.exe
	copy release\S3RelayServer.exe ..\..\..\bin\server\S3RelayServer.exe
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "S3RelayServer - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# 开始自定义宏
OutDir=.\Debug
# 结束自定义宏

ALL : "$(OUTDIR)\S3RelayServer.exe" "$(OUTDIR)\S3RelayServer.pch" "$(OUTDIR)\S3RelayServer.bsc"


CLEAN :
	-@erase "$(INTDIR)\GlobalFun.obj"
	-@erase "$(INTDIR)\GlobalFun.sbr"
	-@erase "$(INTDIR)\KThread.obj"
	-@erase "$(INTDIR)\KThread.sbr"
	-@erase "$(INTDIR)\LogFile.obj"
	-@erase "$(INTDIR)\LogFile.sbr"
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\main.sbr"
	-@erase "$(INTDIR)\S3P_MSSQLServer_Result.obj"
	-@erase "$(INTDIR)\S3P_MSSQLServer_Result.sbr"
	-@erase "$(INTDIR)\S3PAccount.obj"
	-@erase "$(INTDIR)\S3PAccount.sbr"
	-@erase "$(INTDIR)\S3PDB_MSSQLServer_Connection.obj"
	-@erase "$(INTDIR)\S3PDB_MSSQLServer_Connection.sbr"
	-@erase "$(INTDIR)\S3PDBConnectionPool.obj"
	-@erase "$(INTDIR)\S3PDBConnectionPool.sbr"
	-@erase "$(INTDIR)\S3PDBConVBC.obj"
	-@erase "$(INTDIR)\S3PDBConVBC.sbr"
	-@erase "$(INTDIR)\S3PDBSocketPool.obj"
	-@erase "$(INTDIR)\S3PDBSocketPool.sbr"
	-@erase "$(INTDIR)\S3PResultVBC.obj"
	-@erase "$(INTDIR)\S3PResultVBC.sbr"
	-@erase "$(INTDIR)\S3RelayServer.pch"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\Sword3RelaySys.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\S3RelayServer.bsc"
	-@erase "$(OUTDIR)\S3RelayServer.exe"
	-@erase "$(OUTDIR)\S3RelayServer.ilk"
	-@erase "$(OUTDIR)\S3RelayServer.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\S3RelayServer.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x804 /fo"$(INTDIR)\Sword3RelaySys.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\S3RelayServer.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\GlobalFun.sbr" \
	"$(INTDIR)\KThread.sbr" \
	"$(INTDIR)\LogFile.sbr" \
	"$(INTDIR)\main.sbr" \
	"$(INTDIR)\S3P_MSSQLServer_Result.sbr" \
	"$(INTDIR)\S3PAccount.sbr" \
	"$(INTDIR)\S3PDB_MSSQLServer_Connection.sbr" \
	"$(INTDIR)\S3PDBConnectionPool.sbr" \
	"$(INTDIR)\S3PDBConVBC.sbr" \
	"$(INTDIR)\S3PDBSocketPool.sbr" \
	"$(INTDIR)\S3PResultVBC.sbr" \
	"$(INTDIR)\StdAfx.sbr"

"$(OUTDIR)\S3RelayServer.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib Ws2_32.lib /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\S3RelayServer.pdb" /debug /machine:I386 /out:"$(OUTDIR)\S3RelayServer.exe" /pdbtype:sept 
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
	"$(INTDIR)\Sword3RelaySys.res"

"$(OUTDIR)\S3RelayServer.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# 开始自定义宏
OutDir=.\Debug
# 结束自定义宏

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\S3RelayServer.exe" "$(OUTDIR)\S3RelayServer.pch" "$(OUTDIR)\S3RelayServer.bsc"
   md ..\..\..\..\bin\client\debug
	copy debug\S3RelayServer.exe ..\..\..\..\bin\Client\S3RelayServer.exe
	copy debug\S3RelayServer.exe ..\..\..\..\bin\Client\debug\S3RelayServer.exe
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
!IF EXISTS("S3RelayServer.dep")
!INCLUDE "S3RelayServer.dep"
!ELSE 
!MESSAGE Warning: cannot find "S3RelayServer.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "S3RelayServer - Win32 Release" || "$(CFG)" == "S3RelayServer - Win32 Debug"
SOURCE=.\GlobalFun.cpp

"$(INTDIR)\GlobalFun.obj"	"$(INTDIR)\GlobalFun.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\KThread.cpp

"$(INTDIR)\KThread.obj"	"$(INTDIR)\KThread.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\LogFile.cpp

"$(INTDIR)\LogFile.obj"	"$(INTDIR)\LogFile.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\main.cpp

"$(INTDIR)\main.obj"	"$(INTDIR)\main.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\S3P_MSSQLServer_Result.cpp

"$(INTDIR)\S3P_MSSQLServer_Result.obj"	"$(INTDIR)\S3P_MSSQLServer_Result.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\S3PAccount.cpp

"$(INTDIR)\S3PAccount.obj"	"$(INTDIR)\S3PAccount.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\S3PDB_MSSQLServer_Connection.cpp

"$(INTDIR)\S3PDB_MSSQLServer_Connection.obj"	"$(INTDIR)\S3PDB_MSSQLServer_Connection.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\S3PDBConnectionPool.cpp

"$(INTDIR)\S3PDBConnectionPool.obj"	"$(INTDIR)\S3PDBConnectionPool.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\S3PDBConVBC.cpp

"$(INTDIR)\S3PDBConVBC.obj"	"$(INTDIR)\S3PDBConVBC.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\S3PDBSocketPool.cpp

"$(INTDIR)\S3PDBSocketPool.obj"	"$(INTDIR)\S3PDBSocketPool.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\S3PResultVBC.cpp

"$(INTDIR)\S3PResultVBC.obj"	"$(INTDIR)\S3PResultVBC.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "S3RelayServer - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\S3RelayServer.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /Gs /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr"	"$(INTDIR)\S3RelayServer.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "S3RelayServer - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\S3RelayServer.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr"	"$(INTDIR)\S3RelayServer.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\Sword3RelaySys.rc

"$(INTDIR)\Sword3RelaySys.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 


# Microsoft Developer Studio Generated NMAKE File, Based on Bishop.dsp
!IF $(CFG)" == "
CFG=Bishop - Win32 Debug
!MESSAGE No configuration specified. Defaulting to Bishop - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Bishop - Win32 Release" && "$(CFG)" != "Bishop - Win32 Debug"
!MESSAGE 指定的配置 "$(CFG)" 无效.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Bishop.mak" CFG="Bishop - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Bishop - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Bishop - Win32 Debug" (based on "Win32 (x86) Application")
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

!IF  "$(CFG)" == "Bishop - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# 开始自定义宏
OutDir=.\Release
# 结束自定义宏

ALL : "$(OUTDIR)\Bishop.exe" "$(OUTDIR)\Bishop.bsc"


CLEAN :
	-@erase "$(INTDIR)\Application.obj"
	-@erase "$(INTDIR)\Application.sbr"
	-@erase "$(INTDIR)\Bishop.obj"
	-@erase "$(INTDIR)\Bishop.pch"
	-@erase "$(INTDIR)\Bishop.res"
	-@erase "$(INTDIR)\Bishop.sbr"
	-@erase "$(INTDIR)\Event.obj"
	-@erase "$(INTDIR)\Event.sbr"
	-@erase "$(INTDIR)\GamePlayer.obj"
	-@erase "$(INTDIR)\GamePlayer.sbr"
	-@erase "$(INTDIR)\GameServer.obj"
	-@erase "$(INTDIR)\GameServer.sbr"
	-@erase "$(INTDIR)\Intercessor.obj"
	-@erase "$(INTDIR)\Intercessor.sbr"
	-@erase "$(INTDIR)\LogFile.obj"
	-@erase "$(INTDIR)\LogFile.sbr"
	-@erase "$(INTDIR)\Network.obj"
	-@erase "$(INTDIR)\Network.sbr"
	-@erase "$(INTDIR)\PlayerCreator.obj"
	-@erase "$(INTDIR)\PlayerCreator.sbr"
	-@erase "$(INTDIR)\SmartClient.obj"
	-@erase "$(INTDIR)\SmartClient.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\Bishop.bsc"
	-@erase "$(OUTDIR)\Bishop.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "..\Common" /I "..\Rainbow\Interface" /I "..\Heaven\Interface" /I "..\..\..\lib" /I "..\..\..\Headers" /I "..\..\Sword3PaySys\S3AccServer" /I "..\..\s3client\login" /I "..\..\RoleDBManager" /I "..\..\Core\src" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\Bishop.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /Gs /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x804 /fo"$(INTDIR)\Bishop.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Bishop.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\Application.sbr" \
	"$(INTDIR)\Bishop.sbr" \
	"$(INTDIR)\Event.sbr" \
	"$(INTDIR)\GamePlayer.sbr" \
	"$(INTDIR)\GameServer.sbr" \
	"$(INTDIR)\Intercessor.sbr" \
	"$(INTDIR)\LogFile.sbr" \
	"$(INTDIR)\Network.sbr" \
	"$(INTDIR)\PlayerCreator.sbr" \
	"$(INTDIR)\SmartClient.sbr" \
	"$(INTDIR)\StdAfx.sbr"

"$(OUTDIR)\Bishop.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=gdi32.lib /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\Bishop.pdb" /machine:I386 /out:"$(OUTDIR)\Bishop.exe" 
LINK32_OBJS= \
	"$(INTDIR)\Application.obj" \
	"$(INTDIR)\Bishop.obj" \
	"$(INTDIR)\Event.obj" \
	"$(INTDIR)\GamePlayer.obj" \
	"$(INTDIR)\GameServer.obj" \
	"$(INTDIR)\Intercessor.obj" \
	"$(INTDIR)\LogFile.obj" \
	"$(INTDIR)\Network.obj" \
	"$(INTDIR)\PlayerCreator.obj" \
	"$(INTDIR)\SmartClient.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\Bishop.res" \
	"..\..\..\Lib\release\common.lib" \
	"..\..\..\Lib\release\engine.lib"

"$(OUTDIR)\Bishop.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# 开始自定义宏
OutDir=.\Release
# 结束自定义宏

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\Bishop.exe" "$(OUTDIR)\Bishop.bsc"
   md ..\..\..\..\bin\client\release\
	copy release\Bishop.exe ..\..\..\bin\Server\Bishop.exe
	copy release\Bishop.exe ..\..\..\bin\Server\release\Bishop.exe
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "Bishop - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# 开始自定义宏
OutDir=.\Debug
# 结束自定义宏

ALL : "$(OUTDIR)\Bishop.exe" "$(OUTDIR)\Bishop.bsc"


CLEAN :
	-@erase "$(INTDIR)\Application.obj"
	-@erase "$(INTDIR)\Application.sbr"
	-@erase "$(INTDIR)\Bishop.obj"
	-@erase "$(INTDIR)\Bishop.pch"
	-@erase "$(INTDIR)\Bishop.res"
	-@erase "$(INTDIR)\Bishop.sbr"
	-@erase "$(INTDIR)\Event.obj"
	-@erase "$(INTDIR)\Event.sbr"
	-@erase "$(INTDIR)\GamePlayer.obj"
	-@erase "$(INTDIR)\GamePlayer.sbr"
	-@erase "$(INTDIR)\GameServer.obj"
	-@erase "$(INTDIR)\GameServer.sbr"
	-@erase "$(INTDIR)\Intercessor.obj"
	-@erase "$(INTDIR)\Intercessor.sbr"
	-@erase "$(INTDIR)\LogFile.obj"
	-@erase "$(INTDIR)\LogFile.sbr"
	-@erase "$(INTDIR)\Network.obj"
	-@erase "$(INTDIR)\Network.sbr"
	-@erase "$(INTDIR)\PlayerCreator.obj"
	-@erase "$(INTDIR)\PlayerCreator.sbr"
	-@erase "$(INTDIR)\SmartClient.obj"
	-@erase "$(INTDIR)\SmartClient.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\Bishop.bsc"
	-@erase "$(OUTDIR)\Bishop.exe"
	-@erase "$(OUTDIR)\Bishop.ilk"
	-@erase "$(OUTDIR)\Bishop.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\Common" /I "..\Rainbow\Interface" /I "..\Heaven\Interface" /I "..\..\..\lib" /I "..\..\..\Headers" /I "..\..\Sword3PaySys\S3AccServer" /I "..\..\s3client\login" /I "..\..\RoleDBManager" /I "..\..\Core\src" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\Bishop.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x804 /fo"$(INTDIR)\Bishop.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Bishop.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\Application.sbr" \
	"$(INTDIR)\Bishop.sbr" \
	"$(INTDIR)\Event.sbr" \
	"$(INTDIR)\GamePlayer.sbr" \
	"$(INTDIR)\GameServer.sbr" \
	"$(INTDIR)\Intercessor.sbr" \
	"$(INTDIR)\LogFile.sbr" \
	"$(INTDIR)\Network.sbr" \
	"$(INTDIR)\PlayerCreator.sbr" \
	"$(INTDIR)\SmartClient.sbr" \
	"$(INTDIR)\StdAfx.sbr"

"$(OUTDIR)\Bishop.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\Bishop.pdb" /debug /machine:I386 /out:"$(OUTDIR)\Bishop.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\Application.obj" \
	"$(INTDIR)\Bishop.obj" \
	"$(INTDIR)\Event.obj" \
	"$(INTDIR)\GamePlayer.obj" \
	"$(INTDIR)\GameServer.obj" \
	"$(INTDIR)\Intercessor.obj" \
	"$(INTDIR)\LogFile.obj" \
	"$(INTDIR)\Network.obj" \
	"$(INTDIR)\PlayerCreator.obj" \
	"$(INTDIR)\SmartClient.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\Bishop.res" \
	"..\..\..\Lib\debug\common.lib" \
	"..\..\..\Lib\debug\engine.lib"

"$(OUTDIR)\Bishop.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# 开始自定义宏
OutDir=.\Debug
# 结束自定义宏

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\Bishop.exe" "$(OUTDIR)\Bishop.bsc"
   md ..\..\..\..\bin\client\debug\
	copy debug\Bishop.exe ..\..\..\..\bin\Server\Bishop.exe
	copy debug\Bishop.exe ..\..\..\..\bin\Server\debug\Bishop.exe
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
!IF EXISTS("Bishop.dep")
!INCLUDE "Bishop.dep"
!ELSE 
!MESSAGE Warning: cannot find "Bishop.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "Bishop - Win32 Release" || "$(CFG)" == "Bishop - Win32 Debug"
SOURCE=.\Application.cpp

"$(INTDIR)\Application.obj"	"$(INTDIR)\Application.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Bishop.pch"


SOURCE=.\Bishop.cpp

"$(INTDIR)\Bishop.obj"	"$(INTDIR)\Bishop.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Bishop.pch"


SOURCE=..\Common\Event.cpp

"$(INTDIR)\Event.obj"	"$(INTDIR)\Event.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Bishop.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\GamePlayer.cpp

"$(INTDIR)\GamePlayer.obj"	"$(INTDIR)\GamePlayer.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Bishop.pch"


SOURCE=.\GameServer.cpp

"$(INTDIR)\GameServer.obj"	"$(INTDIR)\GameServer.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Bishop.pch"


SOURCE=.\Intercessor.cpp

"$(INTDIR)\Intercessor.obj"	"$(INTDIR)\Intercessor.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Bishop.pch"


SOURCE=.\LogFile.cpp

"$(INTDIR)\LogFile.obj"	"$(INTDIR)\LogFile.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Bishop.pch"


SOURCE=.\Network.cpp

"$(INTDIR)\Network.obj"	"$(INTDIR)\Network.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Bishop.pch"


SOURCE=.\PlayerCreator.cpp

"$(INTDIR)\PlayerCreator.obj"	"$(INTDIR)\PlayerCreator.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Bishop.pch"


SOURCE=.\SmartClient.cpp

"$(INTDIR)\SmartClient.obj"	"$(INTDIR)\SmartClient.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Bishop.pch"


SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "Bishop - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\Common" /I "..\Rainbow\Interface" /I "..\Heaven\Interface" /I "..\..\..\lib" /I "..\..\..\Headers" /I "..\..\Sword3PaySys\S3AccServer" /I "..\..\s3client\login" /I "..\..\RoleDBManager" /I "..\..\Core\src" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\Bishop.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /Gs /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr"	"$(INTDIR)\Bishop.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Bishop - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\Common" /I "..\Rainbow\Interface" /I "..\Heaven\Interface" /I "..\..\..\lib" /I "..\..\..\Headers" /I "..\..\Sword3PaySys\S3AccServer" /I "..\..\s3client\login" /I "..\..\RoleDBManager" /I "..\..\Core\src" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\Bishop.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr"	"$(INTDIR)\Bishop.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\Bishop.rc

"$(INTDIR)\Bishop.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 


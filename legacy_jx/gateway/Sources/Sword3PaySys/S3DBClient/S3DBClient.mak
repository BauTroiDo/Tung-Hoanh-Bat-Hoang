# Microsoft Developer Studio Generated NMAKE File, Based on S3DBClient.dsp
!IF $(CFG)" == "
CFG=S3DBClient - Win32 Debug
!MESSAGE No configuration specified. Defaulting to S3DBClient - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "S3DBClient - Win32 Release" && "$(CFG)" != "S3DBClient - Win32 Debug"
!MESSAGE 指定的配置 "$(CFG)" 无效.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "S3DBClient.mak" CFG="S3DBClient - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "S3DBClient - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "S3DBClient - Win32 Debug" (based on "Win32 (x86) Application")
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

!IF  "$(CFG)" == "S3DBClient - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# 开始自定义宏
OutDir=.\Release
# 结束自定义宏

ALL : ".\Export\S3DBClient.exe" "$(OUTDIR)\S3DBClient.pch"


CLEAN :
	-@erase "$(INTDIR)\AddUserDlg.obj"
	-@erase "$(INTDIR)\ClientSocket.obj"
	-@erase "$(INTDIR)\GameClient.obj"
	-@erase "$(INTDIR)\GlobalFun.obj"
	-@erase "$(INTDIR)\LoginDlg.obj"
	-@erase "$(INTDIR)\MainWnd.obj"
	-@erase "$(INTDIR)\Page1.obj"
	-@erase "$(INTDIR)\Page2.obj"
	-@erase "$(INTDIR)\PerformanceDlg.obj"
	-@erase "$(INTDIR)\S3DBClient.obj"
	-@erase "$(INTDIR)\S3DBClient.pch"
	-@erase "$(INTDIR)\S3DBClient.res"
	-@erase "$(INTDIR)\S3PDBSocketParser.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase ".\Export\S3DBClient.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\s3accserver" /I "..\\" /I "d:\dx81sdk\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x804 /fo"$(INTDIR)\S3DBClient.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\S3DBClient.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=Ws2_32.lib /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\S3DBClient.pdb" /machine:I386 /out:"Export/S3DBClient.exe" 
LINK32_OBJS= \
	"$(INTDIR)\AddUserDlg.obj" \
	"$(INTDIR)\ClientSocket.obj" \
	"$(INTDIR)\GameClient.obj" \
	"$(INTDIR)\GlobalFun.obj" \
	"$(INTDIR)\LoginDlg.obj" \
	"$(INTDIR)\MainWnd.obj" \
	"$(INTDIR)\Page1.obj" \
	"$(INTDIR)\Page2.obj" \
	"$(INTDIR)\PerformanceDlg.obj" \
	"$(INTDIR)\S3DBClient.obj" \
	"$(INTDIR)\S3PDBSocketParser.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\S3DBClient.res"

".\Export\S3DBClient.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "S3DBClient - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# 开始自定义宏
OutDir=.\Debug
# 结束自定义宏

ALL : ".\Export\S3DBClient_d.exe" "$(OUTDIR)\S3DBClient.pch"


CLEAN :
	-@erase "$(INTDIR)\AddUserDlg.obj"
	-@erase "$(INTDIR)\ClientSocket.obj"
	-@erase "$(INTDIR)\GameClient.obj"
	-@erase "$(INTDIR)\GlobalFun.obj"
	-@erase "$(INTDIR)\LoginDlg.obj"
	-@erase "$(INTDIR)\MainWnd.obj"
	-@erase "$(INTDIR)\Page1.obj"
	-@erase "$(INTDIR)\Page2.obj"
	-@erase "$(INTDIR)\PerformanceDlg.obj"
	-@erase "$(INTDIR)\S3DBClient.obj"
	-@erase "$(INTDIR)\S3DBClient.pch"
	-@erase "$(INTDIR)\S3DBClient.res"
	-@erase "$(INTDIR)\S3PDBSocketParser.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\S3DBClient_d.pdb"
	-@erase ".\Export\S3DBClient_d.exe"
	-@erase ".\Export\S3DBClient_d.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\s3accserver" /I "..\\" /I "d:\dx81sdk\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x804 /fo"$(INTDIR)\S3DBClient.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\S3DBClient.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=Ws2_32.lib ole32.lib /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\S3DBClient_d.pdb" /debug /machine:I386 /out:"Export/S3DBClient_d.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\AddUserDlg.obj" \
	"$(INTDIR)\ClientSocket.obj" \
	"$(INTDIR)\GameClient.obj" \
	"$(INTDIR)\GlobalFun.obj" \
	"$(INTDIR)\LoginDlg.obj" \
	"$(INTDIR)\MainWnd.obj" \
	"$(INTDIR)\Page1.obj" \
	"$(INTDIR)\Page2.obj" \
	"$(INTDIR)\PerformanceDlg.obj" \
	"$(INTDIR)\S3DBClient.obj" \
	"$(INTDIR)\S3PDBSocketParser.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\S3DBClient.res"

".\Export\S3DBClient_d.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

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
!IF EXISTS("S3DBClient.dep")
!INCLUDE "S3DBClient.dep"
!ELSE 
!MESSAGE Warning: cannot find "S3DBClient.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "S3DBClient - Win32 Release" || "$(CFG)" == "S3DBClient - Win32 Debug"
SOURCE=.\AddUserDlg.cpp

"$(INTDIR)\AddUserDlg.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ClientSocket.cpp

"$(INTDIR)\ClientSocket.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\GameClient.cpp

"$(INTDIR)\GameClient.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=..\S3AccServer\GlobalFun.cpp

"$(INTDIR)\GlobalFun.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\LoginDlg.cpp

"$(INTDIR)\LoginDlg.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\MainWnd.cpp

"$(INTDIR)\MainWnd.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Page1.cpp

"$(INTDIR)\Page1.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Page2.cpp

"$(INTDIR)\Page2.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\PerformanceDlg.cpp

"$(INTDIR)\PerformanceDlg.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\S3DBClient.cpp

"$(INTDIR)\S3DBClient.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\S3DBClient.rc

"$(INTDIR)\S3DBClient.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=..\S3AccServer\S3PDBSocketParser.cpp

"$(INTDIR)\S3PDBSocketParser.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "S3DBClient - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\s3accserver" /I "..\\" /I "d:\dx81sdk\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)\S3DBClient.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\S3DBClient.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "S3DBClient - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\s3accserver" /I "..\\" /I "d:\dx81sdk\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\S3DBClient.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\S3DBClient.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 


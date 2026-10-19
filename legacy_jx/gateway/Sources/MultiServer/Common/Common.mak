# Microsoft Developer Studio Generated NMAKE File, Based on Common.dsp
!IF $(CFG)" == "
CFG=Common - Win32 Debug
!MESSAGE No configuration specified. Defaulting to Common - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Common - Win32 Release" && "$(CFG)" != "Common - Win32 Debug"
!MESSAGE 指定的配置 "$(CFG)" 无效.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Common.mak" CFG="Common - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Common - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Common - Win32 Debug" (based on "Win32 (x86) Static Library")
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

!IF  "$(CFG)" == "Common - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# 开始自定义宏
OutDir=.\Release
# 结束自定义宏

ALL : "$(OUTDIR)\Common.lib" "$(OUTDIR)\Common.bsc"


CLEAN :
	-@erase "$(INTDIR)\Buffer.obj"
	-@erase "$(INTDIR)\Buffer.sbr"
	-@erase "$(INTDIR)\Common.pch"
	-@erase "$(INTDIR)\Console.obj"
	-@erase "$(INTDIR)\Console.sbr"
	-@erase "$(INTDIR)\CRC32.OBJ"
	-@erase "$(INTDIR)\CRC32.SBR"
	-@erase "$(INTDIR)\Event.obj"
	-@erase "$(INTDIR)\Event.sbr"
	-@erase "$(INTDIR)\EventSelect.obj"
	-@erase "$(INTDIR)\EventSelect.sbr"
	-@erase "$(INTDIR)\Exception.obj"
	-@erase "$(INTDIR)\Exception.sbr"
	-@erase "$(INTDIR)\IniFile.obj"
	-@erase "$(INTDIR)\IniFile.sbr"
	-@erase "$(INTDIR)\IOBuffer.obj"
	-@erase "$(INTDIR)\IOBuffer.sbr"
	-@erase "$(INTDIR)\IOCompletionPort.obj"
	-@erase "$(INTDIR)\IOCompletionPort.sbr"
	-@erase "$(INTDIR)\KSG_EncodeDecode.obj"
	-@erase "$(INTDIR)\KSG_EncodeDecode.sbr"
	-@erase "$(INTDIR)\KSocketClient2.obj"
	-@erase "$(INTDIR)\KSocketClient2.sbr"
	-@erase "$(INTDIR)\Library.obj"
	-@erase "$(INTDIR)\Library.sbr"
	-@erase "$(INTDIR)\Macro.obj"
	-@erase "$(INTDIR)\Macro.sbr"
	-@erase "$(INTDIR)\ManualResetEvent.obj"
	-@erase "$(INTDIR)\ManualResetEvent.sbr"
	-@erase "$(INTDIR)\Mutex.obj"
	-@erase "$(INTDIR)\Mutex.sbr"
	-@erase "$(INTDIR)\NodeList.obj"
	-@erase "$(INTDIR)\NodeList.sbr"
	-@erase "$(INTDIR)\Socket.obj"
	-@erase "$(INTDIR)\Socket.sbr"
	-@erase "$(INTDIR)\SocketAddress.obj"
	-@erase "$(INTDIR)\SocketAddress.sbr"
	-@erase "$(INTDIR)\SocketClient.obj"
	-@erase "$(INTDIR)\SocketClient.sbr"
	-@erase "$(INTDIR)\SocketServer.obj"
	-@erase "$(INTDIR)\SocketServer.sbr"
	-@erase "$(INTDIR)\stdafx.obj"
	-@erase "$(INTDIR)\stdafx.sbr"
	-@erase "$(INTDIR)\Thread.obj"
	-@erase "$(INTDIR)\Thread.sbr"
	-@erase "$(INTDIR)\UsesWinsock.obj"
	-@erase "$(INTDIR)\UsesWinsock.sbr"
	-@erase "$(INTDIR)\Utils.obj"
	-@erase "$(INTDIR)\Utils.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\Win32Exception.obj"
	-@erase "$(INTDIR)\Win32Exception.sbr"
	-@erase "$(OUTDIR)\Common.bsc"
	-@erase "$(OUTDIR)\Common.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MT /W3 /GX /Zi /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D _WIN32_WINNT=0x0400 /FR"$(INTDIR)\\" /Fp"$(INTDIR)\Common.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Common.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\Buffer.sbr" \
	"$(INTDIR)\Console.sbr" \
	"$(INTDIR)\CRC32.SBR" \
	"$(INTDIR)\Event.sbr" \
	"$(INTDIR)\EventSelect.sbr" \
	"$(INTDIR)\Exception.sbr" \
	"$(INTDIR)\IniFile.sbr" \
	"$(INTDIR)\IOBuffer.sbr" \
	"$(INTDIR)\IOCompletionPort.sbr" \
	"$(INTDIR)\KSG_EncodeDecode.sbr" \
	"$(INTDIR)\KSocketClient2.sbr" \
	"$(INTDIR)\Library.sbr" \
	"$(INTDIR)\Macro.sbr" \
	"$(INTDIR)\ManualResetEvent.sbr" \
	"$(INTDIR)\Mutex.sbr" \
	"$(INTDIR)\NodeList.sbr" \
	"$(INTDIR)\Socket.sbr" \
	"$(INTDIR)\SocketAddress.sbr" \
	"$(INTDIR)\SocketClient.sbr" \
	"$(INTDIR)\SocketServer.sbr" \
	"$(INTDIR)\stdafx.sbr" \
	"$(INTDIR)\Thread.sbr" \
	"$(INTDIR)\UsesWinsock.sbr" \
	"$(INTDIR)\Utils.sbr" \
	"$(INTDIR)\Win32Exception.sbr"

"$(OUTDIR)\Common.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\Common.lib" 
LIB32_OBJS= \
	"$(INTDIR)\Buffer.obj" \
	"$(INTDIR)\Console.obj" \
	"$(INTDIR)\CRC32.OBJ" \
	"$(INTDIR)\Event.obj" \
	"$(INTDIR)\EventSelect.obj" \
	"$(INTDIR)\Exception.obj" \
	"$(INTDIR)\IniFile.obj" \
	"$(INTDIR)\IOBuffer.obj" \
	"$(INTDIR)\IOCompletionPort.obj" \
	"$(INTDIR)\KSG_EncodeDecode.obj" \
	"$(INTDIR)\KSocketClient2.obj" \
	"$(INTDIR)\Library.obj" \
	"$(INTDIR)\Macro.obj" \
	"$(INTDIR)\ManualResetEvent.obj" \
	"$(INTDIR)\Mutex.obj" \
	"$(INTDIR)\NodeList.obj" \
	"$(INTDIR)\Socket.obj" \
	"$(INTDIR)\SocketAddress.obj" \
	"$(INTDIR)\SocketClient.obj" \
	"$(INTDIR)\SocketServer.obj" \
	"$(INTDIR)\stdafx.obj" \
	"$(INTDIR)\Thread.obj" \
	"$(INTDIR)\UsesWinsock.obj" \
	"$(INTDIR)\Utils.obj" \
	"$(INTDIR)\Win32Exception.obj"

"$(OUTDIR)\Common.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# 开始自定义宏
OutDir=.\Release
# 结束自定义宏

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\Common.lib" "$(OUTDIR)\Common.bsc"
   md ..\..\..\lib\release
	copy release\common.lib ..\..\..\lib\release\common.lib
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "Common - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# 开始自定义宏
OutDir=.\Debug
# 结束自定义宏

ALL : "$(OUTDIR)\Common.lib" "$(OUTDIR)\Common.bsc"


CLEAN :
	-@erase "$(INTDIR)\Buffer.obj"
	-@erase "$(INTDIR)\Buffer.sbr"
	-@erase "$(INTDIR)\Common.pch"
	-@erase "$(INTDIR)\Console.obj"
	-@erase "$(INTDIR)\Console.sbr"
	-@erase "$(INTDIR)\CRC32.OBJ"
	-@erase "$(INTDIR)\CRC32.SBR"
	-@erase "$(INTDIR)\Event.obj"
	-@erase "$(INTDIR)\Event.sbr"
	-@erase "$(INTDIR)\EventSelect.obj"
	-@erase "$(INTDIR)\EventSelect.sbr"
	-@erase "$(INTDIR)\Exception.obj"
	-@erase "$(INTDIR)\Exception.sbr"
	-@erase "$(INTDIR)\IniFile.obj"
	-@erase "$(INTDIR)\IniFile.sbr"
	-@erase "$(INTDIR)\IOBuffer.obj"
	-@erase "$(INTDIR)\IOBuffer.sbr"
	-@erase "$(INTDIR)\IOCompletionPort.obj"
	-@erase "$(INTDIR)\IOCompletionPort.sbr"
	-@erase "$(INTDIR)\KSG_EncodeDecode.obj"
	-@erase "$(INTDIR)\KSG_EncodeDecode.sbr"
	-@erase "$(INTDIR)\KSocketClient2.obj"
	-@erase "$(INTDIR)\KSocketClient2.sbr"
	-@erase "$(INTDIR)\Library.obj"
	-@erase "$(INTDIR)\Library.sbr"
	-@erase "$(INTDIR)\Macro.obj"
	-@erase "$(INTDIR)\Macro.sbr"
	-@erase "$(INTDIR)\ManualResetEvent.obj"
	-@erase "$(INTDIR)\ManualResetEvent.sbr"
	-@erase "$(INTDIR)\Mutex.obj"
	-@erase "$(INTDIR)\Mutex.sbr"
	-@erase "$(INTDIR)\NodeList.obj"
	-@erase "$(INTDIR)\NodeList.sbr"
	-@erase "$(INTDIR)\Socket.obj"
	-@erase "$(INTDIR)\Socket.sbr"
	-@erase "$(INTDIR)\SocketAddress.obj"
	-@erase "$(INTDIR)\SocketAddress.sbr"
	-@erase "$(INTDIR)\SocketClient.obj"
	-@erase "$(INTDIR)\SocketClient.sbr"
	-@erase "$(INTDIR)\SocketServer.obj"
	-@erase "$(INTDIR)\SocketServer.sbr"
	-@erase "$(INTDIR)\stdafx.obj"
	-@erase "$(INTDIR)\stdafx.sbr"
	-@erase "$(INTDIR)\Thread.obj"
	-@erase "$(INTDIR)\Thread.sbr"
	-@erase "$(INTDIR)\UsesWinsock.obj"
	-@erase "$(INTDIR)\UsesWinsock.sbr"
	-@erase "$(INTDIR)\Utils.obj"
	-@erase "$(INTDIR)\Utils.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\Win32Exception.obj"
	-@erase "$(INTDIR)\Win32Exception.sbr"
	-@erase "$(OUTDIR)\Common.bsc"
	-@erase "$(OUTDIR)\Common.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D _WIN32_WINNT=0x0400 /FR"$(INTDIR)\\" /Fp"$(INTDIR)\Common.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Common.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\Buffer.sbr" \
	"$(INTDIR)\Console.sbr" \
	"$(INTDIR)\CRC32.SBR" \
	"$(INTDIR)\Event.sbr" \
	"$(INTDIR)\EventSelect.sbr" \
	"$(INTDIR)\Exception.sbr" \
	"$(INTDIR)\IniFile.sbr" \
	"$(INTDIR)\IOBuffer.sbr" \
	"$(INTDIR)\IOCompletionPort.sbr" \
	"$(INTDIR)\KSG_EncodeDecode.sbr" \
	"$(INTDIR)\KSocketClient2.sbr" \
	"$(INTDIR)\Library.sbr" \
	"$(INTDIR)\Macro.sbr" \
	"$(INTDIR)\ManualResetEvent.sbr" \
	"$(INTDIR)\Mutex.sbr" \
	"$(INTDIR)\NodeList.sbr" \
	"$(INTDIR)\Socket.sbr" \
	"$(INTDIR)\SocketAddress.sbr" \
	"$(INTDIR)\SocketClient.sbr" \
	"$(INTDIR)\SocketServer.sbr" \
	"$(INTDIR)\stdafx.sbr" \
	"$(INTDIR)\Thread.sbr" \
	"$(INTDIR)\UsesWinsock.sbr" \
	"$(INTDIR)\Utils.sbr" \
	"$(INTDIR)\Win32Exception.sbr"

"$(OUTDIR)\Common.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\Common.lib" 
LIB32_OBJS= \
	"$(INTDIR)\Buffer.obj" \
	"$(INTDIR)\Console.obj" \
	"$(INTDIR)\CRC32.OBJ" \
	"$(INTDIR)\Event.obj" \
	"$(INTDIR)\EventSelect.obj" \
	"$(INTDIR)\Exception.obj" \
	"$(INTDIR)\IniFile.obj" \
	"$(INTDIR)\IOBuffer.obj" \
	"$(INTDIR)\IOCompletionPort.obj" \
	"$(INTDIR)\KSG_EncodeDecode.obj" \
	"$(INTDIR)\KSocketClient2.obj" \
	"$(INTDIR)\Library.obj" \
	"$(INTDIR)\Macro.obj" \
	"$(INTDIR)\ManualResetEvent.obj" \
	"$(INTDIR)\Mutex.obj" \
	"$(INTDIR)\NodeList.obj" \
	"$(INTDIR)\Socket.obj" \
	"$(INTDIR)\SocketAddress.obj" \
	"$(INTDIR)\SocketClient.obj" \
	"$(INTDIR)\SocketServer.obj" \
	"$(INTDIR)\stdafx.obj" \
	"$(INTDIR)\Thread.obj" \
	"$(INTDIR)\UsesWinsock.obj" \
	"$(INTDIR)\Utils.obj" \
	"$(INTDIR)\Win32Exception.obj"

"$(OUTDIR)\Common.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# 开始自定义宏
OutDir=.\Debug
# 结束自定义宏

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\Common.lib" "$(OUTDIR)\Common.bsc"
   md ..\..\..\lib\debug
	copy debug\common.lib ..\..\..\lib\debug\common.lib
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
!IF EXISTS("Common.dep")
!INCLUDE "Common.dep"
!ELSE 
!MESSAGE Warning: cannot find "Common.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "Common - Win32 Release" || "$(CFG)" == "Common - Win32 Debug"
SOURCE=.\Buffer.cpp

"$(INTDIR)\Buffer.obj"	"$(INTDIR)\Buffer.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Common.pch"


SOURCE=.\Console.cpp

"$(INTDIR)\Console.obj"	"$(INTDIR)\Console.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Common.pch"


SOURCE=.\CRC32.C

!IF  "$(CFG)" == "Common - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /Zi /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D _WIN32_WINNT=0x0400 /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\CRC32.OBJ"	"$(INTDIR)\CRC32.SBR" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Common - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D _WIN32_WINNT=0x0400 /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\CRC32.OBJ"	"$(INTDIR)\CRC32.SBR" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\Event.cpp

"$(INTDIR)\Event.obj"	"$(INTDIR)\Event.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Common.pch"


SOURCE=.\EventSelect.cpp

"$(INTDIR)\EventSelect.obj"	"$(INTDIR)\EventSelect.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Common.pch"


SOURCE=.\Exception.cpp

"$(INTDIR)\Exception.obj"	"$(INTDIR)\Exception.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Common.pch"


SOURCE=.\IniFile.cpp

"$(INTDIR)\IniFile.obj"	"$(INTDIR)\IniFile.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Common.pch"


SOURCE=.\IOBuffer.cpp

"$(INTDIR)\IOBuffer.obj"	"$(INTDIR)\IOBuffer.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Common.pch"


SOURCE=.\IOCompletionPort.cpp

"$(INTDIR)\IOCompletionPort.obj"	"$(INTDIR)\IOCompletionPort.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Common.pch"


SOURCE=.\KSG_EncodeDecode.cpp

"$(INTDIR)\KSG_EncodeDecode.obj"	"$(INTDIR)\KSG_EncodeDecode.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Common.pch"


SOURCE=.\KSocketClient2.cpp

"$(INTDIR)\KSocketClient2.obj"	"$(INTDIR)\KSocketClient2.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Common.pch"


SOURCE=.\Library.cpp

"$(INTDIR)\Library.obj"	"$(INTDIR)\Library.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Common.pch"


SOURCE=.\Macro.cpp

"$(INTDIR)\Macro.obj"	"$(INTDIR)\Macro.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Common.pch"


SOURCE=.\ManualResetEvent.cpp

"$(INTDIR)\ManualResetEvent.obj"	"$(INTDIR)\ManualResetEvent.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Common.pch"


SOURCE=.\Mutex.cpp

"$(INTDIR)\Mutex.obj"	"$(INTDIR)\Mutex.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Common.pch"


SOURCE=.\NodeList.cpp

"$(INTDIR)\NodeList.obj"	"$(INTDIR)\NodeList.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Common.pch"


SOURCE=.\Socket.cpp

"$(INTDIR)\Socket.obj"	"$(INTDIR)\Socket.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Common.pch"


SOURCE=.\SocketAddress.cpp

"$(INTDIR)\SocketAddress.obj"	"$(INTDIR)\SocketAddress.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Common.pch"


SOURCE=.\SocketClient.cpp

"$(INTDIR)\SocketClient.obj"	"$(INTDIR)\SocketClient.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Common.pch"


SOURCE=.\SocketServer.cpp

"$(INTDIR)\SocketServer.obj"	"$(INTDIR)\SocketServer.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Common.pch"


SOURCE=.\stdafx.cpp

!IF  "$(CFG)" == "Common - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /Zi /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D _WIN32_WINNT=0x0400 /FR"$(INTDIR)\\" /Fp"$(INTDIR)\Common.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\stdafx.obj"	"$(INTDIR)\stdafx.sbr"	"$(INTDIR)\Common.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Common - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D _WIN32_WINNT=0x0400 /FR"$(INTDIR)\\" /Fp"$(INTDIR)\Common.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\stdafx.obj"	"$(INTDIR)\stdafx.sbr"	"$(INTDIR)\Common.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\Thread.cpp

"$(INTDIR)\Thread.obj"	"$(INTDIR)\Thread.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Common.pch"


SOURCE=.\UsesWinsock.cpp

"$(INTDIR)\UsesWinsock.obj"	"$(INTDIR)\UsesWinsock.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Common.pch"


SOURCE=.\Utils.cpp

"$(INTDIR)\Utils.obj"	"$(INTDIR)\Utils.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Common.pch"


SOURCE=.\Win32Exception.cpp

"$(INTDIR)\Win32Exception.obj"	"$(INTDIR)\Win32Exception.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Common.pch"



!ENDIF 


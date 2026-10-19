# Microsoft Developer Studio Generated NMAKE File, Based on S3Relay.dsp
!IF $(CFG)" == "
CFG=S3Relay - Win32 Debug
!MESSAGE No configuration specified. Defaulting to S3Relay - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "S3Relay - Win32 Release" && "$(CFG)" != "S3Relay - Win32 Debug"
!MESSAGE 指定的配置 "$(CFG)" 无效.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "S3Relay.mak" CFG="S3Relay - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "S3Relay - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "S3Relay - Win32 Debug" (based on "Win32 (x86) Application")
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

!IF  "$(CFG)" == "S3Relay - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# 开始自定义宏
OutDir=.\Release
# 结束自定义宏

ALL : "$(OUTDIR)\S3Relay.exe"


CLEAN :
	-@erase "$(INTDIR)\ChannelMgr.obj"
	-@erase "$(INTDIR)\ChatConnect.obj"
	-@erase "$(INTDIR)\ChatServer.obj"
	-@erase "$(INTDIR)\DBConnect.obj"
	-@erase "$(INTDIR)\DBTable.obj"
	-@erase "$(INTDIR)\DealRelay.obj"
	-@erase "$(INTDIR)\DoScript.obj"
	-@erase "$(INTDIR)\FriendMgr.obj"
	-@erase "$(INTDIR)\GatewayCenter.obj"
	-@erase "$(INTDIR)\GatewayClient.obj"
	-@erase "$(INTDIR)\Global.obj"
	-@erase "$(INTDIR)\HeavenLib.obj"
	-@erase "$(INTDIR)\HostConnect.obj"
	-@erase "$(INTDIR)\HostServer.obj"
	-@erase "$(INTDIR)\KThread.obj"
	-@erase "$(INTDIR)\KTongControl.obj"
	-@erase "$(INTDIR)\KTongSet.obj"
	-@erase "$(INTDIR)\Lock.obj"
	-@erase "$(INTDIR)\LogFile.obj"
	-@erase "$(INTDIR)\Memory.obj"
	-@erase "$(INTDIR)\NetCenter.obj"
	-@erase "$(INTDIR)\NetClient.obj"
	-@erase "$(INTDIR)\NetConnect.obj"
	-@erase "$(INTDIR)\NetServer.obj"
	-@erase "$(INTDIR)\NetSockDupEx.obj"
	-@erase "$(INTDIR)\RainbowLib.obj"
	-@erase "$(INTDIR)\RelayCenter.obj"
	-@erase "$(INTDIR)\RelayClient.obj"
	-@erase "$(INTDIR)\RelayConnect.obj"
	-@erase "$(INTDIR)\RelayServer.obj"
	-@erase "$(INTDIR)\RootCenter.obj"
	-@erase "$(INTDIR)\RootClient.obj"
	-@erase "$(INTDIR)\S3Relay.obj"
	-@erase "$(INTDIR)\S3Relay.pch"
	-@erase "$(INTDIR)\S3Relay.res"
	-@erase "$(INTDIR)\SockThread.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\TongConnect.obj"
	-@erase "$(INTDIR)\TONGDB.OBJ"
	-@erase "$(INTDIR)\TongServer.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\S3Relay.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_WORKMODE_SINGLETHREAD" /D "_TESTING" /Fp"$(INTDIR)\S3Relay.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x804 /fo"$(INTDIR)\S3Relay.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\S3Relay.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=user32.lib lualibdll.lib Ws2_32.lib /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\S3Relay.pdb" /machine:I386 /out:"$(OUTDIR)\S3Relay.exe" 
LINK32_OBJS= \
	"$(INTDIR)\ChannelMgr.obj" \
	"$(INTDIR)\ChatConnect.obj" \
	"$(INTDIR)\ChatServer.obj" \
	"$(INTDIR)\DBConnect.obj" \
	"$(INTDIR)\DBTable.obj" \
	"$(INTDIR)\DealRelay.obj" \
	"$(INTDIR)\DoScript.obj" \
	"$(INTDIR)\FriendMgr.obj" \
	"$(INTDIR)\GatewayCenter.obj" \
	"$(INTDIR)\GatewayClient.obj" \
	"$(INTDIR)\Global.obj" \
	"$(INTDIR)\HeavenLib.obj" \
	"$(INTDIR)\HostConnect.obj" \
	"$(INTDIR)\HostServer.obj" \
	"$(INTDIR)\KThread.obj" \
	"$(INTDIR)\Lock.obj" \
	"$(INTDIR)\LogFile.obj" \
	"$(INTDIR)\Memory.obj" \
	"$(INTDIR)\NetCenter.obj" \
	"$(INTDIR)\NetClient.obj" \
	"$(INTDIR)\NetConnect.obj" \
	"$(INTDIR)\NetServer.obj" \
	"$(INTDIR)\NetSockDupEx.obj" \
	"$(INTDIR)\RainbowLib.obj" \
	"$(INTDIR)\RelayCenter.obj" \
	"$(INTDIR)\RelayClient.obj" \
	"$(INTDIR)\RelayConnect.obj" \
	"$(INTDIR)\RelayServer.obj" \
	"$(INTDIR)\RootCenter.obj" \
	"$(INTDIR)\RootClient.obj" \
	"$(INTDIR)\S3Relay.obj" \
	"$(INTDIR)\SockThread.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\TongConnect.obj" \
	"$(INTDIR)\TongServer.obj" \
	"$(INTDIR)\KTongControl.obj" \
	"$(INTDIR)\KTongSet.obj" \
	"$(INTDIR)\TONGDB.OBJ" \
	"$(INTDIR)\S3Relay.res" \
	"..\..\..\Lib\release\engine.lib" \
	".\libdb41s.lib"

"$(OUTDIR)\S3Relay.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# 开始自定义宏
OutDir=.\Release
# 结束自定义宏

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\S3Relay.exe"
   copy release\S3Relay.exe ..\..\..\..\bin\Server\S3Relay.exe
	copy release\S3Relay.exe ..\..\..\bin\Server\S3Relay.exe
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "S3Relay - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# 开始自定义宏
OutDir=.\Debug
# 结束自定义宏

ALL : "$(OUTDIR)\S3Relay.exe" "$(OUTDIR)\S3Relay.bsc"


CLEAN :
	-@erase "$(INTDIR)\ChannelMgr.obj"
	-@erase "$(INTDIR)\ChannelMgr.sbr"
	-@erase "$(INTDIR)\ChatConnect.obj"
	-@erase "$(INTDIR)\ChatConnect.sbr"
	-@erase "$(INTDIR)\ChatServer.obj"
	-@erase "$(INTDIR)\ChatServer.sbr"
	-@erase "$(INTDIR)\DBConnect.obj"
	-@erase "$(INTDIR)\DBConnect.sbr"
	-@erase "$(INTDIR)\DBTable.obj"
	-@erase "$(INTDIR)\DBTable.sbr"
	-@erase "$(INTDIR)\DealRelay.obj"
	-@erase "$(INTDIR)\DealRelay.sbr"
	-@erase "$(INTDIR)\DoScript.obj"
	-@erase "$(INTDIR)\DoScript.sbr"
	-@erase "$(INTDIR)\FriendMgr.obj"
	-@erase "$(INTDIR)\FriendMgr.sbr"
	-@erase "$(INTDIR)\GatewayCenter.obj"
	-@erase "$(INTDIR)\GatewayCenter.sbr"
	-@erase "$(INTDIR)\GatewayClient.obj"
	-@erase "$(INTDIR)\GatewayClient.sbr"
	-@erase "$(INTDIR)\Global.obj"
	-@erase "$(INTDIR)\Global.sbr"
	-@erase "$(INTDIR)\HeavenLib.obj"
	-@erase "$(INTDIR)\HeavenLib.sbr"
	-@erase "$(INTDIR)\HostConnect.obj"
	-@erase "$(INTDIR)\HostConnect.sbr"
	-@erase "$(INTDIR)\HostServer.obj"
	-@erase "$(INTDIR)\HostServer.sbr"
	-@erase "$(INTDIR)\KThread.obj"
	-@erase "$(INTDIR)\KThread.sbr"
	-@erase "$(INTDIR)\KTongControl.obj"
	-@erase "$(INTDIR)\KTongControl.sbr"
	-@erase "$(INTDIR)\KTongSet.obj"
	-@erase "$(INTDIR)\KTongSet.sbr"
	-@erase "$(INTDIR)\Lock.obj"
	-@erase "$(INTDIR)\Lock.sbr"
	-@erase "$(INTDIR)\LogFile.obj"
	-@erase "$(INTDIR)\LogFile.sbr"
	-@erase "$(INTDIR)\Memory.obj"
	-@erase "$(INTDIR)\Memory.sbr"
	-@erase "$(INTDIR)\NetCenter.obj"
	-@erase "$(INTDIR)\NetCenter.sbr"
	-@erase "$(INTDIR)\NetClient.obj"
	-@erase "$(INTDIR)\NetClient.sbr"
	-@erase "$(INTDIR)\NetConnect.obj"
	-@erase "$(INTDIR)\NetConnect.sbr"
	-@erase "$(INTDIR)\NetServer.obj"
	-@erase "$(INTDIR)\NetServer.sbr"
	-@erase "$(INTDIR)\NetSockDupEx.obj"
	-@erase "$(INTDIR)\NetSockDupEx.sbr"
	-@erase "$(INTDIR)\RainbowLib.obj"
	-@erase "$(INTDIR)\RainbowLib.sbr"
	-@erase "$(INTDIR)\RelayCenter.obj"
	-@erase "$(INTDIR)\RelayCenter.sbr"
	-@erase "$(INTDIR)\RelayClient.obj"
	-@erase "$(INTDIR)\RelayClient.sbr"
	-@erase "$(INTDIR)\RelayConnect.obj"
	-@erase "$(INTDIR)\RelayConnect.sbr"
	-@erase "$(INTDIR)\RelayServer.obj"
	-@erase "$(INTDIR)\RelayServer.sbr"
	-@erase "$(INTDIR)\RootCenter.obj"
	-@erase "$(INTDIR)\RootCenter.sbr"
	-@erase "$(INTDIR)\RootClient.obj"
	-@erase "$(INTDIR)\RootClient.sbr"
	-@erase "$(INTDIR)\S3Relay.obj"
	-@erase "$(INTDIR)\S3Relay.pch"
	-@erase "$(INTDIR)\S3Relay.res"
	-@erase "$(INTDIR)\S3Relay.sbr"
	-@erase "$(INTDIR)\SockThread.obj"
	-@erase "$(INTDIR)\SockThread.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\TongConnect.obj"
	-@erase "$(INTDIR)\TongConnect.sbr"
	-@erase "$(INTDIR)\TONGDB.OBJ"
	-@erase "$(INTDIR)\TONGDB.SBR"
	-@erase "$(INTDIR)\TongServer.obj"
	-@erase "$(INTDIR)\TongServer.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\S3Relay.bsc"
	-@erase "$(OUTDIR)\S3Relay.exe"
	-@erase "$(OUTDIR)\S3Relay.ilk"
	-@erase "$(OUTDIR)\S3Relay.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_WORKMODE_SINGLETHREAD" /D "_TESTING" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\S3Relay.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x804 /fo"$(INTDIR)\S3Relay.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\S3Relay.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\ChannelMgr.sbr" \
	"$(INTDIR)\ChatConnect.sbr" \
	"$(INTDIR)\ChatServer.sbr" \
	"$(INTDIR)\DBConnect.sbr" \
	"$(INTDIR)\DBTable.sbr" \
	"$(INTDIR)\DealRelay.sbr" \
	"$(INTDIR)\DoScript.sbr" \
	"$(INTDIR)\FriendMgr.sbr" \
	"$(INTDIR)\GatewayCenter.sbr" \
	"$(INTDIR)\GatewayClient.sbr" \
	"$(INTDIR)\Global.sbr" \
	"$(INTDIR)\HeavenLib.sbr" \
	"$(INTDIR)\HostConnect.sbr" \
	"$(INTDIR)\HostServer.sbr" \
	"$(INTDIR)\KThread.sbr" \
	"$(INTDIR)\Lock.sbr" \
	"$(INTDIR)\LogFile.sbr" \
	"$(INTDIR)\Memory.sbr" \
	"$(INTDIR)\NetCenter.sbr" \
	"$(INTDIR)\NetClient.sbr" \
	"$(INTDIR)\NetConnect.sbr" \
	"$(INTDIR)\NetServer.sbr" \
	"$(INTDIR)\NetSockDupEx.sbr" \
	"$(INTDIR)\RainbowLib.sbr" \
	"$(INTDIR)\RelayCenter.sbr" \
	"$(INTDIR)\RelayClient.sbr" \
	"$(INTDIR)\RelayConnect.sbr" \
	"$(INTDIR)\RelayServer.sbr" \
	"$(INTDIR)\RootCenter.sbr" \
	"$(INTDIR)\RootClient.sbr" \
	"$(INTDIR)\S3Relay.sbr" \
	"$(INTDIR)\SockThread.sbr" \
	"$(INTDIR)\StdAfx.sbr" \
	"$(INTDIR)\TongConnect.sbr" \
	"$(INTDIR)\TongServer.sbr" \
	"$(INTDIR)\KTongControl.sbr" \
	"$(INTDIR)\KTongSet.sbr" \
	"$(INTDIR)\TONGDB.SBR"

"$(OUTDIR)\S3Relay.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib Ws2_32.lib lualibdll.lib /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\S3Relay.pdb" /debug /machine:I386 /out:"$(OUTDIR)\S3Relay.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\ChannelMgr.obj" \
	"$(INTDIR)\ChatConnect.obj" \
	"$(INTDIR)\ChatServer.obj" \
	"$(INTDIR)\DBConnect.obj" \
	"$(INTDIR)\DBTable.obj" \
	"$(INTDIR)\DealRelay.obj" \
	"$(INTDIR)\DoScript.obj" \
	"$(INTDIR)\FriendMgr.obj" \
	"$(INTDIR)\GatewayCenter.obj" \
	"$(INTDIR)\GatewayClient.obj" \
	"$(INTDIR)\Global.obj" \
	"$(INTDIR)\HeavenLib.obj" \
	"$(INTDIR)\HostConnect.obj" \
	"$(INTDIR)\HostServer.obj" \
	"$(INTDIR)\KThread.obj" \
	"$(INTDIR)\Lock.obj" \
	"$(INTDIR)\LogFile.obj" \
	"$(INTDIR)\Memory.obj" \
	"$(INTDIR)\NetCenter.obj" \
	"$(INTDIR)\NetClient.obj" \
	"$(INTDIR)\NetConnect.obj" \
	"$(INTDIR)\NetServer.obj" \
	"$(INTDIR)\NetSockDupEx.obj" \
	"$(INTDIR)\RainbowLib.obj" \
	"$(INTDIR)\RelayCenter.obj" \
	"$(INTDIR)\RelayClient.obj" \
	"$(INTDIR)\RelayConnect.obj" \
	"$(INTDIR)\RelayServer.obj" \
	"$(INTDIR)\RootCenter.obj" \
	"$(INTDIR)\RootClient.obj" \
	"$(INTDIR)\S3Relay.obj" \
	"$(INTDIR)\SockThread.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\TongConnect.obj" \
	"$(INTDIR)\TongServer.obj" \
	"$(INTDIR)\KTongControl.obj" \
	"$(INTDIR)\KTongSet.obj" \
	"$(INTDIR)\TONGDB.OBJ" \
	"$(INTDIR)\S3Relay.res" \
	"..\..\..\Lib\debug\engine.lib" \
	".\libdb41s.lib"

"$(OUTDIR)\S3Relay.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# 开始自定义宏
OutDir=.\Debug
# 结束自定义宏

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\S3Relay.exe" "$(OUTDIR)\S3Relay.bsc"
   md ..\..\..\..\bin\client\debug\
	copy debug\S3Relay.exe ..\..\..\..\bin\Server\S3Relay.exe
	copy debug\S3Relay.exe ..\..\..\..\bin\Server\debug\S3Relay.exe
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
!IF EXISTS("S3Relay.dep")
!INCLUDE "S3Relay.dep"
!ELSE 
!MESSAGE Warning: cannot find "S3Relay.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "S3Relay - Win32 Release" || "$(CFG)" == "S3Relay - Win32 Debug"
SOURCE=.\ChannelMgr.cpp

!IF  "$(CFG)" == "S3Relay - Win32 Release"


"$(INTDIR)\ChannelMgr.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\S3Relay.pch"


!ELSEIF  "$(CFG)" == "S3Relay - Win32 Debug"


"$(INTDIR)\ChannelMgr.obj"	"$(INTDIR)\ChannelMgr.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\S3Relay.pch"


!ENDIF 

SOURCE=.\ChatConnect.cpp

!IF  "$(CFG)" == "S3Relay - Win32 Release"


"$(INTDIR)\ChatConnect.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\S3Relay.pch"


!ELSEIF  "$(CFG)" == "S3Relay - Win32 Debug"


"$(INTDIR)\ChatConnect.obj"	"$(INTDIR)\ChatConnect.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\S3Relay.pch"


!ENDIF 

SOURCE=.\ChatServer.cpp

!IF  "$(CFG)" == "S3Relay - Win32 Release"


"$(INTDIR)\ChatServer.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\S3Relay.pch"


!ELSEIF  "$(CFG)" == "S3Relay - Win32 Debug"


"$(INTDIR)\ChatServer.obj"	"$(INTDIR)\ChatServer.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\S3Relay.pch"


!ENDIF 

SOURCE=.\DBConnect.cpp

!IF  "$(CFG)" == "S3Relay - Win32 Release"


"$(INTDIR)\DBConnect.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\S3Relay.pch"


!ELSEIF  "$(CFG)" == "S3Relay - Win32 Debug"


"$(INTDIR)\DBConnect.obj"	"$(INTDIR)\DBConnect.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\S3Relay.pch"


!ENDIF 

SOURCE=.\DBTable.cpp

!IF  "$(CFG)" == "S3Relay - Win32 Release"


"$(INTDIR)\DBTable.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\S3Relay.pch"


!ELSEIF  "$(CFG)" == "S3Relay - Win32 Debug"


"$(INTDIR)\DBTable.obj"	"$(INTDIR)\DBTable.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\S3Relay.pch"


!ENDIF 

SOURCE=.\DealRelay.cpp

!IF  "$(CFG)" == "S3Relay - Win32 Release"


"$(INTDIR)\DealRelay.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\S3Relay.pch"


!ELSEIF  "$(CFG)" == "S3Relay - Win32 Debug"


"$(INTDIR)\DealRelay.obj"	"$(INTDIR)\DealRelay.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\S3Relay.pch"


!ENDIF 

SOURCE=.\DoScript.cpp

!IF  "$(CFG)" == "S3Relay - Win32 Release"


"$(INTDIR)\DoScript.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\S3Relay.pch"


!ELSEIF  "$(CFG)" == "S3Relay - Win32 Debug"


"$(INTDIR)\DoScript.obj"	"$(INTDIR)\DoScript.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\S3Relay.pch"


!ENDIF 

SOURCE=.\FriendMgr.cpp

!IF  "$(CFG)" == "S3Relay - Win32 Release"


"$(INTDIR)\FriendMgr.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\S3Relay.pch"


!ELSEIF  "$(CFG)" == "S3Relay - Win32 Debug"


"$(INTDIR)\FriendMgr.obj"	"$(INTDIR)\FriendMgr.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\S3Relay.pch"


!ENDIF 

SOURCE=.\GatewayCenter.cpp

!IF  "$(CFG)" == "S3Relay - Win32 Release"


"$(INTDIR)\GatewayCenter.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\S3Relay.pch"


!ELSEIF  "$(CFG)" == "S3Relay - Win32 Debug"


"$(INTDIR)\GatewayCenter.obj"	"$(INTDIR)\GatewayCenter.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\S3Relay.pch"


!ENDIF 

SOURCE=.\GatewayClient.cpp

!IF  "$(CFG)" == "S3Relay - Win32 Release"


"$(INTDIR)\GatewayClient.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\S3Relay.pch"


!ELSEIF  "$(CFG)" == "S3Relay - Win32 Debug"


"$(INTDIR)\GatewayClient.obj"	"$(INTDIR)\GatewayClient.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\S3Relay.pch"


!ENDIF 

SOURCE=.\Global.cpp

!IF  "$(CFG)" == "S3Relay - Win32 Release"


"$(INTDIR)\Global.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\S3Relay.pch"


!ELSEIF  "$(CFG)" == "S3Relay - Win32 Debug"


"$(INTDIR)\Global.obj"	"$(INTDIR)\Global.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\S3Relay.pch"


!ENDIF 

SOURCE=.\HeavenLib.cpp

!IF  "$(CFG)" == "S3Relay - Win32 Release"


"$(INTDIR)\HeavenLib.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\S3Relay.pch"


!ELSEIF  "$(CFG)" == "S3Relay - Win32 Debug"


"$(INTDIR)\HeavenLib.obj"	"$(INTDIR)\HeavenLib.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\S3Relay.pch"


!ENDIF 

SOURCE=.\HostConnect.cpp

!IF  "$(CFG)" == "S3Relay - Win32 Release"


"$(INTDIR)\HostConnect.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\S3Relay.pch"


!ELSEIF  "$(CFG)" == "S3Relay - Win32 Debug"


"$(INTDIR)\HostConnect.obj"	"$(INTDIR)\HostConnect.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\S3Relay.pch"


!ENDIF 

SOURCE=.\HostServer.cpp

!IF  "$(CFG)" == "S3Relay - Win32 Release"


"$(INTDIR)\HostServer.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\S3Relay.pch"


!ELSEIF  "$(CFG)" == "S3Relay - Win32 Debug"


"$(INTDIR)\HostServer.obj"	"$(INTDIR)\HostServer.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\S3Relay.pch"


!ENDIF 

SOURCE=.\KThread.cpp

!IF  "$(CFG)" == "S3Relay - Win32 Release"


"$(INTDIR)\KThread.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\S3Relay.pch"


!ELSEIF  "$(CFG)" == "S3Relay - Win32 Debug"


"$(INTDIR)\KThread.obj"	"$(INTDIR)\KThread.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\S3Relay.pch"


!ENDIF 

SOURCE=.\Lock.cpp

!IF  "$(CFG)" == "S3Relay - Win32 Release"


"$(INTDIR)\Lock.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\S3Relay.pch"


!ELSEIF  "$(CFG)" == "S3Relay - Win32 Debug"


"$(INTDIR)\Lock.obj"	"$(INTDIR)\Lock.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\S3Relay.pch"


!ENDIF 

SOURCE=.\LogFile.cpp

!IF  "$(CFG)" == "S3Relay - Win32 Release"


"$(INTDIR)\LogFile.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\S3Relay.pch"


!ELSEIF  "$(CFG)" == "S3Relay - Win32 Debug"


"$(INTDIR)\LogFile.obj"	"$(INTDIR)\LogFile.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\S3Relay.pch"


!ENDIF 

SOURCE=.\Memory.cpp

!IF  "$(CFG)" == "S3Relay - Win32 Release"


"$(INTDIR)\Memory.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\S3Relay.pch"


!ELSEIF  "$(CFG)" == "S3Relay - Win32 Debug"


"$(INTDIR)\Memory.obj"	"$(INTDIR)\Memory.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\S3Relay.pch"


!ENDIF 

SOURCE=.\NetCenter.cpp

!IF  "$(CFG)" == "S3Relay - Win32 Release"


"$(INTDIR)\NetCenter.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\S3Relay.pch"


!ELSEIF  "$(CFG)" == "S3Relay - Win32 Debug"


"$(INTDIR)\NetCenter.obj"	"$(INTDIR)\NetCenter.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\S3Relay.pch"


!ENDIF 

SOURCE=.\NetClient.cpp

!IF  "$(CFG)" == "S3Relay - Win32 Release"


"$(INTDIR)\NetClient.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\S3Relay.pch"


!ELSEIF  "$(CFG)" == "S3Relay - Win32 Debug"


"$(INTDIR)\NetClient.obj"	"$(INTDIR)\NetClient.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\S3Relay.pch"


!ENDIF 

SOURCE=.\NetConnect.cpp

!IF  "$(CFG)" == "S3Relay - Win32 Release"


"$(INTDIR)\NetConnect.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\S3Relay.pch"


!ELSEIF  "$(CFG)" == "S3Relay - Win32 Debug"


"$(INTDIR)\NetConnect.obj"	"$(INTDIR)\NetConnect.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\S3Relay.pch"


!ENDIF 

SOURCE=.\NetServer.cpp

!IF  "$(CFG)" == "S3Relay - Win32 Release"


"$(INTDIR)\NetServer.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\S3Relay.pch"


!ELSEIF  "$(CFG)" == "S3Relay - Win32 Debug"


"$(INTDIR)\NetServer.obj"	"$(INTDIR)\NetServer.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\S3Relay.pch"


!ENDIF 

SOURCE=.\NetSockDupEx.cpp

!IF  "$(CFG)" == "S3Relay - Win32 Release"


"$(INTDIR)\NetSockDupEx.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\S3Relay.pch"


!ELSEIF  "$(CFG)" == "S3Relay - Win32 Debug"


"$(INTDIR)\NetSockDupEx.obj"	"$(INTDIR)\NetSockDupEx.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\S3Relay.pch"


!ENDIF 

SOURCE=.\RainbowLib.cpp

!IF  "$(CFG)" == "S3Relay - Win32 Release"


"$(INTDIR)\RainbowLib.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\S3Relay.pch"


!ELSEIF  "$(CFG)" == "S3Relay - Win32 Debug"


"$(INTDIR)\RainbowLib.obj"	"$(INTDIR)\RainbowLib.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\S3Relay.pch"


!ENDIF 

SOURCE=.\RelayCenter.cpp

!IF  "$(CFG)" == "S3Relay - Win32 Release"


"$(INTDIR)\RelayCenter.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\S3Relay.pch"


!ELSEIF  "$(CFG)" == "S3Relay - Win32 Debug"


"$(INTDIR)\RelayCenter.obj"	"$(INTDIR)\RelayCenter.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\S3Relay.pch"


!ENDIF 

SOURCE=.\RelayClient.cpp

!IF  "$(CFG)" == "S3Relay - Win32 Release"


"$(INTDIR)\RelayClient.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\S3Relay.pch"


!ELSEIF  "$(CFG)" == "S3Relay - Win32 Debug"


"$(INTDIR)\RelayClient.obj"	"$(INTDIR)\RelayClient.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\S3Relay.pch"


!ENDIF 

SOURCE=.\RelayConnect.cpp

!IF  "$(CFG)" == "S3Relay - Win32 Release"


"$(INTDIR)\RelayConnect.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\S3Relay.pch"


!ELSEIF  "$(CFG)" == "S3Relay - Win32 Debug"


"$(INTDIR)\RelayConnect.obj"	"$(INTDIR)\RelayConnect.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\S3Relay.pch"


!ENDIF 

SOURCE=.\RelayServer.cpp

!IF  "$(CFG)" == "S3Relay - Win32 Release"


"$(INTDIR)\RelayServer.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\S3Relay.pch"


!ELSEIF  "$(CFG)" == "S3Relay - Win32 Debug"


"$(INTDIR)\RelayServer.obj"	"$(INTDIR)\RelayServer.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\S3Relay.pch"


!ENDIF 

SOURCE=.\RootCenter.cpp

!IF  "$(CFG)" == "S3Relay - Win32 Release"


"$(INTDIR)\RootCenter.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\S3Relay.pch"


!ELSEIF  "$(CFG)" == "S3Relay - Win32 Debug"


"$(INTDIR)\RootCenter.obj"	"$(INTDIR)\RootCenter.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\S3Relay.pch"


!ENDIF 

SOURCE=.\RootClient.cpp

!IF  "$(CFG)" == "S3Relay - Win32 Release"


"$(INTDIR)\RootClient.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\S3Relay.pch"


!ELSEIF  "$(CFG)" == "S3Relay - Win32 Debug"


"$(INTDIR)\RootClient.obj"	"$(INTDIR)\RootClient.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\S3Relay.pch"


!ENDIF 

SOURCE=.\S3Relay.cpp

!IF  "$(CFG)" == "S3Relay - Win32 Release"


"$(INTDIR)\S3Relay.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\S3Relay.pch"


!ELSEIF  "$(CFG)" == "S3Relay - Win32 Debug"


"$(INTDIR)\S3Relay.obj"	"$(INTDIR)\S3Relay.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\S3Relay.pch"


!ENDIF 

SOURCE=.\S3Relay.rc

"$(INTDIR)\S3Relay.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\SockThread.cpp

!IF  "$(CFG)" == "S3Relay - Win32 Release"


"$(INTDIR)\SockThread.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\S3Relay.pch"


!ELSEIF  "$(CFG)" == "S3Relay - Win32 Debug"


"$(INTDIR)\SockThread.obj"	"$(INTDIR)\SockThread.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\S3Relay.pch"


!ENDIF 

SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "S3Relay - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_WORKMODE_SINGLETHREAD" /D "_TESTING" /Fp"$(INTDIR)\S3Relay.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\S3Relay.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "S3Relay - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_WORKMODE_SINGLETHREAD" /D "_TESTING" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\S3Relay.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr"	"$(INTDIR)\S3Relay.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\TongConnect.cpp

!IF  "$(CFG)" == "S3Relay - Win32 Release"


"$(INTDIR)\TongConnect.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\S3Relay.pch"


!ELSEIF  "$(CFG)" == "S3Relay - Win32 Debug"


"$(INTDIR)\TongConnect.obj"	"$(INTDIR)\TongConnect.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\S3Relay.pch"


!ENDIF 

SOURCE=.\TongServer.cpp

!IF  "$(CFG)" == "S3Relay - Win32 Release"


"$(INTDIR)\TongServer.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\S3Relay.pch"


!ELSEIF  "$(CFG)" == "S3Relay - Win32 Debug"


"$(INTDIR)\TongServer.obj"	"$(INTDIR)\TongServer.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\S3Relay.pch"


!ENDIF 

SOURCE=.\KTongControl.cpp

!IF  "$(CFG)" == "S3Relay - Win32 Release"


"$(INTDIR)\KTongControl.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\S3Relay.pch"


!ELSEIF  "$(CFG)" == "S3Relay - Win32 Debug"


"$(INTDIR)\KTongControl.obj"	"$(INTDIR)\KTongControl.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\S3Relay.pch"


!ENDIF 

SOURCE=.\KTongSet.cpp

!IF  "$(CFG)" == "S3Relay - Win32 Release"


"$(INTDIR)\KTongSet.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\S3Relay.pch"


!ELSEIF  "$(CFG)" == "S3Relay - Win32 Debug"


"$(INTDIR)\KTongSet.obj"	"$(INTDIR)\KTongSet.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\S3Relay.pch"


!ENDIF 

SOURCE=.\TONGDB.CPP

!IF  "$(CFG)" == "S3Relay - Win32 Release"


"$(INTDIR)\TONGDB.OBJ" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\S3Relay.pch"


!ELSEIF  "$(CFG)" == "S3Relay - Win32 Debug"


"$(INTDIR)\TONGDB.OBJ"	"$(INTDIR)\TONGDB.SBR" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\S3Relay.pch"


!ENDIF 


!ENDIF 


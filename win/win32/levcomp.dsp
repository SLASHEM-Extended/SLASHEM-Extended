# Microsoft Developer Studio Project File - Name="levcomp" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=levcomp - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "levcomp.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "levcomp.mak" CFG="levcomp - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "levcomp - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "levcomp - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "levcomp - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\util"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\include" /I "..\sys\winnt" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "WIN32CON" /D "DLB" /D "MSWIN_GRAPHICS" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x1009 /d "NDEBUG"
# ADD RSC /l 0x1009 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=levcomp
PostBuild_Cmds=echo Building special levels	echo chdir ..\dat	chdir ..\dat	 \
echo Arch.des	..\util\levcomp.exe Arch.des	 \
echo Barb.des	..\util\levcomp.exe Barb.des	 \
echo beholder.des	..\util\levcomp.exe beholder.des	 \
echo bigroom.des	..\util\levcomp.exe bigroom.des	 \
echo blkmar.des	..\util\levcomp.exe blkmar.des	 \
echo castle.des	..\util\levcomp.exe castle.des	 \
echo Caveman.des	..\util\levcomp.exe Caveman.des	 \
echo caves.des	..\util\levcomp.exe caves.des	 \
echo dragons.des	..\util\levcomp.exe dragons.des	 \
echo endgame.des	..\util\levcomp.exe endgame.des	 \
echo Flame.des	..\util\levcomp.exe Flame.des	 \
echo frnknstn.des	..\util\levcomp.exe frnknstn.des	 \
echo gehennom.des	..\util\levcomp.exe gehennom.des	 \
echo giants.des	..\util\levcomp.exe giants.des	 \
echo guild.des	..\util\levcomp.exe guild.des	 \
echo Healer.des	..\util\levcomp.exe Healer.des	 \
echo Ice.des	..\util\levcomp.exe Ice.des	 \
echo Knight.des	..\util\levcomp.exe Knight.des	 \
echo knox.des	..\util\levcomp.exe knox.des	 \
echo kobold-1.des	..\util\levcomp.exe kobold-1.des	 \
echo kobold-2.des	..\util\levcomp.exe kobold-2.des	 \
echo lich.des	..\util\levcomp.exe lich.des	 \
echo mall-1.des	..\util\levcomp.exe mall-1.des	 \
echo mall-2.des	..\util\levcomp.exe mall-2.des	 \
echo medusa.des	..\util\levcomp.exe medusa.des	 \
echo mines.des	..\util\levcomp.exe mines.des	 \
echo mtemple.des	..\util\levcomp.exe mtemple.des	 \
echo Monk.des	..\util\levcomp.exe Monk.des	 \
echo Necro.des	..\util\levcomp.exe Necro.des	 \
echo newmall.des	..\util\levcomp.exe newmall.des	 \
echo nightmar.des	..\util\levcomp.exe nightmar.des	 \
echo nymph.des	..\util\levcomp.exe nymph.des	 \
echo oracle.des	..\util\levcomp.exe oracle.des	 \
echo Priest.des	..\util\levcomp.exe Priest.des	 \
echo Ranger.des	..\util\levcomp.exe Ranger.des	 \
echo rats.des	..\util\levcomp.exe rats.des	 \
echo Rogue.des	..\util\levcomp.exe Rogue.des	 \
echo Samurai.des	..\util\levcomp.exe Samurai.des	 \
echo sea.des	..\util\levcomp.exe sea.des	 \
echo sokoban.des	..\util\levcomp.exe sokoban.des	 \
echo spiders.des	..\util\levcomp.exe spiders.des	 \
echo stor-1.des	..\util\levcomp.exe stor-1.des	 \
echo stor-2.des	..\util\levcomp.exe stor-2.des	 \
echo stor-3.des	..\util\levcomp.exe stor-3.des	 \
echo Tourist.des	..\util\levcomp.exe Tourist.des	 \
echo tomb.des	..\util\levcomp.exe tomb.des	 \
echo tower.des	..\util\levcomp.exe tower.des	 \
echo Slayer.des	..\util\levcomp.exe Slayer.des	 \
echo Valkyrie.des	..\util\levcomp.exe Valkyrie.des	 \
echo Wizard .des	..\util\levcomp.exe Wizard.des	 \
echo yendor.des	..\util\levcomp.exe yendor.des	 \
echo chdir ..\build	chdir ..\build
# End Special Build Tool

!ELSEIF  "$(CFG)" == "levcomp - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\util"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\sys\winnt" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "WIN32CON" /D "DLB" /D "MSWIN_GRAPHICS" /FD /GZ /c
# ADD BASE RSC /l 0x1009 /d "_DEBUG"
# ADD RSC /l 0x1009 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:no /debug /machine:I386 /pdbtype:sept
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=levcomp
PostBuild_Cmds=echo Building special levels	echo chdir ..\dat	chdir ..\dat	 \
echo Arch.des	..\util\levcomp.exe Arch.des	 \
echo Barb.des	..\util\levcomp.exe Barb.des	 \
echo beholder.des	..\util\levcomp.exe beholder.des	 \
echo bigroom.des	..\util\levcomp.exe bigroom.des	 \
echo blkmar.des	..\util\levcomp.exe blkmar.des	 \
echo castle.des	..\util\levcomp.exe castle.des	 \
echo Caveman.des	..\util\levcomp.exe Caveman.des	 \
echo caves.des	..\util\levcomp.exe caves.des	 \
echo dragons.des	..\util\levcomp.exe dragons.des	 \
echo endgame.des	..\util\levcomp.exe endgame.des	 \
echo Flame.des	..\util\levcomp.exe Flame.des	 \
echo frnknstn.des	..\util\levcomp.exe frnknstn.des	 \
echo gehennom.des	..\util\levcomp.exe gehennom.des	 \
echo giants.des	..\util\levcomp.exe giants.des	 \
echo guild.des	..\util\levcomp.exe guild.des	 \
echo Healer.des	..\util\levcomp.exe Healer.des	 \
echo Ice.des	..\util\levcomp.exe Ice.des	 \
echo Knight.des	..\util\levcomp.exe Knight.des	 \
echo knox.des	..\util\levcomp.exe knox.des	 \
echo kobold-1.des	..\util\levcomp.exe kobold-1.des	 \
echo kobold-2.des	..\util\levcomp.exe kobold-2.des	 \
echo lich.des	..\util\levcomp.exe lich.des	 \
echo mall-1.des	..\util\levcomp.exe mall-1.des	 \
echo mall-2.des	..\util\levcomp.exe mall-2.des	 \
echo medusa.des	..\util\levcomp.exe medusa.des	 \
echo mines.des	..\util\levcomp.exe mines.des	 \
echo mtemple.des	..\util\levcomp.exe mtemple.des	 \
echo Monk.des	..\util\levcomp.exe Monk.des	 \
echo Necro.des	..\util\levcomp.exe Necro.des	 \
echo newmall.des	..\util\levcomp.exe newmall.des	 \
echo nightmar.des	..\util\levcomp.exe nightmar.des	 \
echo nymph.des	..\util\levcomp.exe nymph.des	 \
echo oracle.des	..\util\levcomp.exe oracle.des	 \
echo Priest.des	..\util\levcomp.exe Priest.des	 \
echo Ranger.des	..\util\levcomp.exe Ranger.des	 \
echo rats.des	..\util\levcomp.exe rats.des	 \
echo Rogue.des	..\util\levcomp.exe Rogue.des	 \
echo Samurai.des	..\util\levcomp.exe Samurai.des	 \
echo sea.des	..\util\levcomp.exe sea.des	 \
echo sokoban.des	..\util\levcomp.exe sokoban.des	 \
echo spiders.des	..\util\levcomp.exe spiders.des	 \
echo stor-1.des	..\util\levcomp.exe stor-1.des	 \
echo stor-2.des	..\util\levcomp.exe stor-2.des	 \
echo stor-3.des	..\util\levcomp.exe stor-3.des	 \
echo Tourist.des	..\util\levcomp.exe Tourist.des	 \
echo tomb.des	..\util\levcomp.exe tomb.des	 \
echo tower.des	..\util\levcomp.exe tower.des	 \
echo Slayer.des	..\util\levcomp.exe Slayer.des	 \
echo Valkyrie.des	..\util\levcomp.exe Valkyrie.des	 \
echo Wizard .des	..\util\levcomp.exe Wizard.des	 \
echo yendor.des	..\util\levcomp.exe yendor.des	 \
echo chdir ..\build	chdir ..\build
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "levcomp - Win32 Release"
# Name "levcomp - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\src\alloc.c
# End Source File
# Begin Source File

SOURCE=..\src\decl.c
# End Source File
# Begin Source File

SOURCE=..\src\drawing.c
# End Source File
# Begin Source File

SOURCE=..\util\lev_lex.c
# End Source File
# Begin Source File

SOURCE=..\util\lev_main.c
# End Source File
# Begin Source File

SOURCE=..\util\lev_yacc.c
# End Source File
# Begin Source File

SOURCE=..\src\monst.c
# End Source File
# Begin Source File

SOURCE=..\src\objects.c
# End Source File
# Begin Source File

SOURCE=..\util\panic.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\include\lev_comp.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project

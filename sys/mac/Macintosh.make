# This is a complete MPW makefile for Nethack and all its associated files
# Requires MPW C3.2§3 or later. Earlier versions will fail horribly because
# they can't cope with /* /* */ comments
#
# For NetHack 3.1 Nov 1991
#
# by Michaelª Hamel and Ross Brown 1991 : michael@otago.ac.nz
#
# Updated for Slash'EM 0.0.5-2 to .6-4F8
# Paul Hurtley, August 1999 to March 2002

#--------------------------------------------------------------------------
#
# BEFORE YOU BUILD FOR THE FIRST TIME
#  Set this equate to the folder containing all the Slash'EM source folders
Top   		= Cabell:Documents:Projects:SlashEM:slashem-cvs:slashem:

Processor  = PowerPC
#Processor	= 68K
#Processor	= Fat
#
#--------------------------------------------------------------------------

# Set up symbols for folders from the distribution
Src   	   = {Top}Src:
Util 	   = {Top}Util:
Dat		   = {Top}Dat:
Doc		   = {Top}Doc:
Grammar	   = {Top}Grammar:
Include    = {Top}Include:
MacDir	   = {Top}Sys:Mac:
TtyDir     = {Top}Win:Tty:
MacTty     = {Top}Sys:Mac:
Share	   = {Top}Sys:Share:

# These folders are new
ObjDir     = {Top}Obj{Processor}:				# "Temporary" stuff
Results	   = {Top}Dungeon{Processor}:			# Where the game goes

# Override the settings in "{Include}"config.h
# NHConfig = -d PORT_LEVEL='¶".0¶"' 

# Type and creator - note that we use "PREF" for all non-save files
# currently. Since we will integrate the data files into the game
# sooner or later, this doesn't matter much.
FileType   = PREF
SlashEMCreator = slEm
RecoverCreator = slRc

#
#  Yacc/Lex ... if you got 'em.
#
# If you have yacc/lex or a work-alike set YACC_LEX to Y
#
YACC_LEX = Y

# If YACC_LEX is Y above, set the following to values appropriate for
# your tools.
#
YACC   = bison -y
LEX    = flex
YTabC  = y.tab.c
YTabH  = y.tab.h
LexYYC = lex.yy.c

# Macintosh is an abstract target which in fact consists of:

# The following files are in the binary distribution
DataFiles 	=	"{Results}"Record   		¶
				"{Results}"Guidebook.txt	¶
				"{Results}"History			¶
				"{Results}"License			¶
				"{Results}"Slash¶'EM¶ Defaults   ¶
				"{Results}"README

# The following files are generated, but included in the source distribution anyway				
Preserved	=	"{ObjDir}"date.h 		¶
				"{ObjDir}"pm.h 			¶
				"{ObjDir}"onames.h 		¶
				"{ObjDir}"monstr.c  	¶
				"{ObjDir}"vis_tab.h 	¶
				"{ObjDir}"vis_tab.c 	¶
				"{ObjDir}"filename.h 	¶
				"{Grammar}"lev_yacc.c 	¶
				"{Grammar}"lev_comp.h 	¶
				"{Grammar}"dgn_yacc.c 	¶
				"{Grammar}"dgn_comp.h	¶
				"{ObjDir}"MRecover.rsrc	¶
				"{ObjDir}"NHrsrc.rsrc	¶
				"{ObjDir}"NHsound.rsrc				
				
Macintosh Ä   	Setup				¶
				MakeDefs.lnk 		¶
				{Preserved} 		¶
				LevComp.lnk 		¶
				DgnComp.lnk 		¶
				{DataFiles} 		¶
				Recover.lnk 		¶
				SlashEM.lnk
				
Setup	Ä
	If Not "`Exists -d "{Results}"`"
		NewFolder "{Results}"
	End
	If Not "`Exists -d "{ObjDir}"`"
		NewFolder "{ObjDir}"
	End
	If Not "`Exists -d "{Grammar}"`"
		NewFolder "{Grammar}"
	End
	
# Compiler options. We set up to look in the Mac dir first for include files
# so we can take over hack.h and use a dump file
# Note that this is disabled for Slash'EM

COptions = {NHConfig}					¶
		   -w 2 -w 3 -ansi relaxed -typecheck relaxed -align power

NHIncludes = "{Include}"

#------------------- Use a dump file for hack.h to speed compiles -----------------
# We do this by having our own hack.h in :sys:mac which just grabs the dump file
# from ObjDir. The dependencies are set up to build the dump file if its missing

realhack.h = "{Include}"hack.h "{Include}"dungeon.h ¶
			 "{Include}"monsym.h "{Include}"mkroom.h "{Include}"objclass.h ¶
			 "{Include}"trap.h "{Include}"flag.h "{Include}"rm.h ¶
			 "{Include}"wintype.h "{Include}"engrave.h ¶
			 "{Include}"rect.h  "{Include}"trampoli.h "{Include}"extern.h 

hack.h = "{ObjDir}"hack.hdump

# This compile is done solely for the side effect of generating hack.hdump
"{ObjDir}"hack.hdump Ä {realhack.h} "{MacDir}"mhdump.c
	C -i "{ObjDir}" -i "{Include}" {NHConfig} {SADEOptions} ¶
	  "{MacDir}"mhdump.c -o "{Src}"mhdump.c

#------------------- Files included as resources -----------------

FileResources = ¶
	"{Dat}"cmdhelp ¶
	"{Dat}"help ¶
	"{Dat}"hh ¶
	"{Dat}"history ¶
	"{Dat}"license ¶
	"{Results}"Guidebook.txt ¶
	"{MacDir}"MacHelp ¶
	"{MacDir}"News ¶
	"{Dat}"opthelp ¶
	"{Dat}"wizhelp ¶
	"{ObjDir}"Quest.dat ¶
	"{ObjDir}"data ¶
	"{ObjDir}"dungeon ¶
	"{ObjDir}"oracles ¶
	"{ObjDir}"rumors ¶
	Levels
	
#------------------- Slash'EM Sources -----------------

SlashEMSrcs = ¶
	"{Src}"allmain.c ¶
	"{Src}"alloc.c ¶
	"{Src}"apply.c ¶
	"{Src}"artifact.c ¶
	"{Src}"attrib.c ¶
	"{Src}"ball.c ¶
	"{Src}"bones.c ¶
	"{Src}"borg.c ¶
	"{Src}"botl.c ¶
	"{Src}"cmd.c ¶
	"{Src}"dbridge.c ¶
	"{Src}"decl.c ¶
	"{Src}"detect.c ¶
	"{Src}"dig.c ¶
	"{Src}"display.c ¶
	"{Src}"dlb.c ¶
	"{Src}"do.c ¶
	"{Src}"do_name.c ¶
	"{Src}"do_wear.c ¶
	"{Src}"dog.c ¶
	"{Src}"dogmove.c ¶
	"{Src}"dokick.c ¶
	"{Src}"dothrow.c ¶
	"{Src}"drawing.c ¶
	"{Src}"dungeon.c ¶
	"{Src}"eat.c ¶
	"{Src}"end.c ¶
	"{Src}"engrave.c ¶
	"{Src}"exper.c ¶
	"{Src}"explode.c ¶
	"{Src}"extralev.c ¶
	"{Src}"files.c ¶
	"{Src}"fountain.c ¶
	"{Src}"gypsy.c ¶
	"{Src}"hack.c ¶
	"{Src}"hacklib.c ¶
	"{Src}"invent.c ¶
	"{Src}"light.c ¶
	"{Src}"lock.c ¶
	"{Src}"mail.c ¶
	"{Src}"makemon.c ¶
	"{Src}"mcastu.c ¶
	"{Src}"mhitm.c ¶
	"{Src}"mhitu.c ¶
	"{Src}"minion.c ¶
	"{Src}"mklev.c ¶
	"{Src}"mkmap.c ¶
	"{Src}"mkmaze.c ¶
	"{Src}"mkobj.c ¶
	"{Src}"mkroom.c ¶
	"{Src}"mon.c ¶
	"{Src}"mondata.c ¶
	"{Src}"monmove.c ¶
	"{Src}"monst.c ¶
	"{ObjDir}"monstr.c ¶
	"{Src}"mplayer.c ¶
	"{Src}"mthrowu.c ¶
	"{Src}"muse.c ¶
	"{Src}"music.c ¶
	"{Src}"o_init.c ¶
	"{Src}"objects.c ¶
	"{Src}"objnam.c ¶
	"{Src}"options.c ¶
	"{Src}"pager.c ¶
	"{Src}"pickup.c ¶
	"{Src}"pline.c ¶
	"{Src}"polyself.c ¶
	"{Src}"potion.c ¶
	"{Src}"pray.c ¶
	"{Src}"priest.c ¶
	"{Src}"quest.c ¶
	"{Src}"questpgr.c ¶
	"{Src}"read.c ¶
	"{Src}"rect.c ¶
	"{Src}"region.c ¶
	"{Src}"restore.c ¶
	"{Src}"rip.c ¶
	"{Src}"rnd.c ¶
	"{Src}"role.c ¶
	"{Src}"rumors.c ¶
	"{Src}"save.c ¶
	"{Src}"shk.c ¶
	"{Src}"shknam.c ¶
	"{Src}"sit.c ¶
	"{Src}"sounds.c ¶
	"{Src}"sp_lev.c ¶
	"{Src}"spell.c ¶
	"{Src}"steal.c ¶
	"{Src}"steed.c ¶
	"{Src}"tech.c ¶
	"{Src}"teleport.c ¶
	"{Src}"timeout.c ¶
	"{Src}"topten.c ¶
	"{Src}"track.c ¶
	"{Src}"trap.c ¶
	"{Src}"u_init.c ¶
	"{Src}"uhitm.c ¶
	"{Src}"vault.c ¶
	"{Src}"version.c ¶
	"{ObjDir}"vis_tab.c ¶
	"{Src}"vision.c ¶
	"{Src}"weapon.c ¶
	"{Src}"were.c ¶
	"{Src}"wield.c ¶
	"{Src}"windows.c ¶
	"{Src}"wizard.c ¶
	"{Src}"worm.c ¶
	"{Src}"worn.c ¶
	"{Src}"write.c ¶
	"{Src}"zap.c ¶
	¶
	"{MacDir}"dprintf.c ¶
	"{MacDir}"maccurs.c ¶
	"{MacDir}"macerrs.c ¶
	"{MacDir}"macfile.c ¶
	"{MacDir}"macmain.c ¶
	"{MacDir}"macmenu.c ¶
	"{MacDir}"macsnd.c ¶
	"{MacDir}"mactopl.c ¶
	"{MacDir}"mactty.c ¶
	"{MacDir}"macunix.c ¶
	"{MacDir}"macwin.c ¶
	"{MacDir}"mgetline.c ¶
	"{MacDir}"mmodal.c ¶
	"{MacDir}"mttymain.c ¶
	¶
	"{Share}"random.c ¶
	¶
	"{TtyDir}"getline.c ¶
	"{TtyDir}"topl.c ¶
	"{TtyDir}"wintty.c
	
SlashEMRsrcs = 				¶
	"{MacDir}"MPW.r

# -------- Build the dungeon compiler, as an MPW tool ---------------

DgnCompSrcs = 				¶
	"{Grammar}"dgn_lex.c	¶
	"{Util}"dgn_main.c		¶
	"{Grammar}"dgn_yacc.c	¶
	"{Src}"alloc.c			¶
	"{Util}"panic.c

"{ObjDir}"DgnComp.make	Ä	 "{Grammar}"dgn_lex.c "{Grammar}"dgn_yacc.c
	CreateMake "{ObjDir}"DgnComp {DgnCompSrcs} -tool -powerpc -objdir "{ObjDir}" -i "{Grammar}" -i "{ObjDir}" -i {NHIncludes} -ppccoptions "{COptions}" -sym -depends
	
"{ObjDir}"DgnComp DgnComp.lnk Ä "{ObjDir}"DgnComp.make
	BuildProgram "{ObjDir}"DgnComp

"{Grammar}"dgn_comp.h "{Grammar}"dgn_yacc.c	Ä  "{Util}"dgn_comp.y
	{YACC} -d "{Util}"dgn_comp.y
	Move -y "{YTabC}" "{Grammar}"dgn_yacc.c
	Move -y "{YTabH}" "{Grammar}"dgn_comp.h
	
{Grammar}dgn_lex.c	Ä  {Util}dgn_comp.l
	{LEX} {Util}dgn_comp.l
	Move -y {LexYYC} {Grammar}dgn_lex.c
	
# -------- Build the special-level compiler, as an MPW tool ---------------

LevCompSrcs= "{Src}"monst.c		¶
		 "{Src}"objects.c		¶
		 "{Src}"drawing.c		¶
		 "{Src}"alloc.c			¶
		 "{Util}"panic.c		¶
		 "{Grammar}"lev_lex.c	¶
		 "{Grammar}"lev_yacc.c	¶
		 "{MacDir}"macfile.c	¶
		 "{MacDir}"macerrs.c	¶
		 "{Src}"files.c			¶
		 "{Src}"decl.c			¶
		 "{Util}"lev_main.c

"{ObjDir}"LevComp.make	Ä	 "{Grammar}"lev_lex.c "{Grammar}"lev_yacc.c
	CreateMake "{ObjDir}"LevComp {LevCompSrcs} -tool -powerpc -objdir "{ObjDir}" -i "{Grammar}" -i "{ObjDir}" -i {NHIncludes} -ppccoptions "{COptions}" -sym -depends
	
"{ObjDir}"LevComp LevComp.lnk Ä "{ObjDir}"LevComp.make
	BuildProgram "{ObjDir}"LevComp

"{Grammar}"lev_comp.h "{Grammar}"lev_yacc.c	Ä  "{Util}"lev_comp.y
	{YACC} -d "{Util}"lev_comp.y
	Move -y "{YTabH}" "{Grammar}"lev_comp.h
	Move -y "{YTabC}" "{Grammar}"lev_yacc.c
	
"{Grammar}"lev_lex.c	Ä  {Util}lev_comp.l
	{LEX} {Util}lev_comp.l
	Move -y {LexYYC} {Grammar}lev_lex.c
	
# -------- Build "{ObjDir}"MakeDefs, as an MPW tool ---------------

MakeDefsSrcs= "{Src}"objects.c		¶
			  "{Src}"monst.c		¶
			  "{Util}"MakeDefs.c

"{ObjDir}"MakeDefs.make Ä
	CreateMake "{ObjDir}"MakeDefs {MakeDefsSrcs} -tool -powerpc -objdir "{ObjDir}" -i "{Grammar}" -i "{ObjDir}" -i {NHIncludes} -ppccoptions "{COptions}" -sym -depends

"{ObjDir}"MakeDefs MakeDefs.lnk Ä "{ObjDir}"MakeDefs.make
	BuildProgram "{ObjDir}"MakeDefs

# ------------ If "{ObjDir}"MakeDefs changes we need to rebuild some include files -----------

"{ObjDir}"filename.h 	Ä 	"{ObjDir}"MakeDefs
	Directory "{Top}"
	"{ObjDir}"MakeDefs -f
	Move -y "{Include}"filename.h "{ObjDir}"filename.h
	Directory "{MacDir}"

"{ObjDir}"date.h 	Ä 	"{ObjDir}"Options "{ObjDir}"MakeDefs
	Move -y "{Include}"date.h "{ObjDir}"date.h

"{ObjDir}"Options	Ä	$OutOfDate "{ObjDir}"MakeDefs
	Set -e ObjDir "{ObjDir}"
	Directory "{Top}"
	"{ObjDir}"MakeDefs -v
	Directory "{MacDir}"
	
"{ObjDir}"onames.h Ä	"{ObjDir}"MakeDefs
	Directory "{Top}"
	"{ObjDir}"MakeDefs -o
	Move -y "{Include}"onames.h "{ObjDir}"onames.h
	Directory "{MacDir}"

"{ObjDir}"pm.h 	Ä	"{ObjDir}"MakeDefs
	Directory "{Top}"
	"{ObjDir}"MakeDefs -p
	Move -y "{Include}"pm.h "{ObjDir}"pm.h
	Directory "{MacDir}"

"{ObjDir}"vis_tab.c	Ä	"{ObjDir}"vis_tab.h
	Move -y "{Src}"vis_tab.c "{ObjDir}"vis_tab.c

"{ObjDir}"vis_tab.h Ä	"{ObjDir}"MakeDefs
	Directory "{Top}"
	"{ObjDir}"MakeDefs -z
	Move -y "{Include}"vis_tab.h "{ObjDir}"vis_tab.h
	Directory "{MacDir}"

"{ObjDir}"Dungeon.pdf  Ä "{Dat}"Dungeon.def "{ObjDir}"MakeDefs
	Directory "{Top}"
	"{ObjDir}"MakeDefs -e
	Move -y "{Dat}"Dungeon.pdf "{ObjDir}"Dungeon.pdf
	Directory "{MacDir}"

"{ObjDir}"monstr.c Ä "{ObjDir}"MakeDefs
	Directory "{Top}"
	"{ObjDir}"MakeDefs -m
	Move -y "{Src}"monstr.c "{ObjDir}"monstr.c
	Directory "{MacDir}"

#---------------- Data files -------------------

Levels Ä "{ObjDir}"Levels.list "{MacDir}"Levels.make
		Set -e Dat "{Dat}"
		Set -e ObjDir "{ObjDir}"
		Make -f "{MacDir}"Levels.make Levels > "{ObjDir}"Levels.makeout
		"{ObjDir}"Levels.makeout
		
"{ObjDir}"Data  Ä "{Dat}"Data.base "{ObjDir}"MakeDefs
		Directory "{Top}"
	    "{ObjDir}"MakeDefs -d
		SetFile -t "{FileType}" -c "{SlashEMCreator}" "{ObjDir}"Data
		Directory "{MacDir}"

"{ObjDir}"Rumors  Ä "{Dat}"Rumors.tru "{Dat}"Rumors.fal "{ObjDir}"MakeDefs
		Directory "{Top}"
	    "{ObjDir}"MakeDefs -r
		SetFile -t "{FileType}" -c "{SlashEMCreator}" "{ObjDir}"Rumors
		Directory "{MacDir}"

"{ObjDir}"Oracles Ä "{Dat}"Oracles.txt "{ObjDir}"MakeDefs
		Directory "{Top}"
	    "{ObjDir}"MakeDefs -h
		SetFile -t "{FileType}" -c "{SlashEMCreator}" "{ObjDir}"Oracles
		Directory "{MacDir}"

"{Results}"Record  Ä 
		Echo "This is the record file" > "{Results}"Record
		SetFile -c "ttxt"  -t "ttro" "{Results}"Record

"{Results}"Slash¶'EM¶ Defaults  Ä	"{MacDir}"NHDeflts 
		Duplicate -y "{MacDir}"NHDeflts "{Results}"Slash¶'EM¶ Defaults
		SetFile -c "ttxt" "{Results}"Slash¶'EM¶ Defaults

"{Results}"Guidebook.txt  Ä	"{Doc}"Guidebook.txt
		Duplicate -y "{Doc}"Guidebook.txt "{Results}"Guidebook.txt
		SetFile -c "MOSS" "{Results}"Guidebook.txt
		
"{Results}"History  Ä	"{Top}"readme.txt
		Duplicate -y "{Top}"readme.txt "{Results}"History
		SetFile -c "MOSS" "{Results}"History
		
"{Results}"README  Ä	"{MacDir}"README.txt
		Duplicate -y "{MacDir}"README.txt "{Results}"README
		SetFile -c "ttxt" -t "ttro" "{Results}"README

"{ObjDir}"hh  Ä	"{Dat}"hh
		Duplicate -y "{Dat}"hh "{ObjDir}"hh

"{ObjDir}"History  Ä	"{Dat}"History
		Duplicate -y "{Dat}"History "{ObjDir}"History

"{Results}"License  Ä	"{Dat}"License
		Duplicate -y "{Dat}"License "{Results}"License
		SetFile -c "ttxt" -t "ttro" "{Results}"License

"{ObjDir}"Help  Ä	"{Dat}"Help
		Duplicate -y "{Dat}"Help "{ObjDir}"Help

"{ObjDir}"Wizhelp  Ä	"{Dat}"Wizhelp
		Duplicate -y "{Dat}"Wizhelp "{ObjDir}"Wizhelp

"{ObjDir}"Opthelp  Ä	"{Dat}"Opthelp
		Duplicate -y "{Dat}"Opthelp "{ObjDir}"Opthelp

"{ObjDir}"Machelp  Ä	"{MacDir}"Machelp
		Duplicate -y "{MacDir}"Machelp "{ObjDir}"Machelp

"{ObjDir}"News  Ä	"{MacDir}"News
		Duplicate -y "{MacDir}"News "{ObjDir}"News

"{ObjDir}"Dungeon  Ä "{ObjDir}"Dungeon.pdf "{ObjDir}"DgnComp
		"{ObjDir}"DgnComp "{ObjDir}"dungeon.pdf
		SetFile -t "{FileType}" -c "{SlashEMCreator}" "{ObjDir}"Dungeon

"{ObjDir}"Quest.Dat	Ä "{Dat}"Quest.txt "{ObjDir}"MakeDefs
		Directory "{Top}"
	    "{ObjDir}"MakeDefs -q
		SetFile -t "{FileType}" -c "{SlashEMCreator}" "{ObjDir}"Quest.dat
		Directory "{MacDir}"

#---------------- The "{ObjDir}"Recover application -------------------
RecoverSrcs = "{MacDir}"MRecover.c

RecoverRsrcs = "{ObjDir}"MRecover.rsrc
	
"{ObjDir}"Recover.make	Ä
		CreateMake "{ObjDir}"Recover {RecoverSrcs} {RecoverRsrcs} -{Processor} -i "{Grammar}" -i "{ObjDir}" -i {NHIncludes} -objdir "{ObjDir}" -sym -depends -c {RecoverCreator} ¶
			-ppccoptions "{COptions}" ¶
			-coptions "{COptions}" -model far ¶
				"{SharedLibraries}"AppearanceLib ¶
				"{SharedLibraries}"ATSUnicodeLib ¶
				"{SharedLibraries}"ContextualMenu ¶
				"{SharedLibraries}"ControlsLib ¶
				"{SharedLibraries}"DialogsLib ¶
				"{SharedLibraries}"FindByContent ¶
				"{SharedLibraries}"FontManager ¶
				"{SharedLibraries}"IconServicesLib ¶
				"{SharedLibraries}"MenusLib ¶
				"{SharedLibraries}"NavigationLib ¶
				"{SharedLibraries}"UnicodeUtilitiesLib ¶
				"{SharedLibraries}"WindowsLib  ¶
				"{SharedLibraries}"TextCommon ¶
				"{SharedLibraries}"TextEncodingConverter ¶
				"{SharedLibraries}"UnicodeConverter

"{Results}"Recover Recover.lnk ÄÄ "{ObjDir}"Recover.make
		BuildProgram "{ObjDir}"Recover
		SetFile -a B "{ObjDir}"Recover
		Move -y "{ObjDir}"Recover "{Results}"Recover 	

"{ObjDir}"MRecover.rsrc	Ä	"{MacDir}"MRecover.r
		Rez Types.r SysTypes.r "{MacDir}"MRecover.r -o "{ObjDir}"MRecover.rsrc
	
#---------------- The application -------------------

"{ObjDir}"SlashEM.make	Ä 
		Set -e Dat "{Dat}"			# Make the internal Make variable a Shell variable
		Set -e MacDir "{MacDir}"
		Set -e Results "{Results}"
		CreateMake "{ObjDir}"SlashEM {SlashEMSrcs} {SlashEMRsrcs} -{Processor} -i "{Grammar}" -i "{ObjDir}" -i {NHIncludes} -objdir "{ObjDir}" -sym -depends -c {SlashEMCreator} ¶
			-ppccoptions "{COptions}" ¶
			-coptions "{COptions}" -model far ¶
				"{SharedLibraries}"AppearanceLib ¶
				"{SharedLibraries}"ATSUnicodeLib ¶
				"{SharedLibraries}"ContextualMenu ¶
				"{SharedLibraries}"ControlsLib ¶
				"{SharedLibraries}"DialogsLib ¶
				"{SharedLibraries}"FindByContent ¶
				"{SharedLibraries}"FontManager ¶
				"{SharedLibraries}"IconServicesLib ¶
				"{SharedLibraries}"MenusLib ¶
				"{SharedLibraries}"NavigationLib ¶
				"{SharedLibraries}"UnicodeUtilitiesLib ¶
				"{SharedLibraries}"WindowsLib  ¶
				"{SharedLibraries}"TextCommon ¶
				"{SharedLibraries}"TextEncodingConverter ¶
				"{SharedLibraries}"UnicodeConverter

"{Results}"Slash¶'EM SlashEM.lnk Ä "{ObjDir}"SlashEM.make {FileResources}
		Set -e Dat "{Dat}"
		Set -e MacDir "{MacDir}"
		Set -e Results "{Results}"
		Set -e RIncludes "{RIncludes}","{MacDir}"
		BuildProgram "{ObjDir}"SlashEM
		SetFile -a B "{ObjDir}"SlashEM
		Move -y "{ObjDir}"SlashEM "{Results}"Slash¶'EM 	

"{ObjDir}"NHrsrc.rsrc Ä "{MacDir}"NHrsrc.r
		Rez Types.r SysTypes.r "{MacDir}"NHrsrc.r -o "{ObjDir}"NHrsrc.rsrc

"{ObjDir}"NHsound.rsrc Ä "{MacDir}"NHsound.r
		Rez Types.r SysTypes.r "{MacDir}"NHsound.r -o "{ObjDir}"NHsound.rsrc

#---------------- Dependencies -------------------

Dependencies Ä	Setup					¶
				"{ObjDir}"Levels.list	¶
				"{ObjDir}"MakeDefs.make ¶
				{Preserved}				¶
				"{ObjDir}"LevComp.make 	¶
				"{ObjDir}"DgnComp.make 	¶
				"{ObjDir}"Recover.make 	¶
				"{ObjDir}"SlashEM.make
		Make -f "{ObjDir}"MakeDefs.make Dependencies > "{ObjDir}"MakeDefs.makeout
		"{ObjDir}"MakeDefs.makeout
		Make -f "{ObjDir}"LevComp.make Dependencies > "{ObjDir}"LevComp.makeout
		"{ObjDir}"LevComp.makeout
		Make -f "{ObjDir}"DgnComp.make Dependencies > "{ObjDir}"DgnComp.makeout
		"{ObjDir}"DgnComp.makeout
		Make -f "{ObjDir}"Recover.make Dependencies > "{ObjDir}"Recover.makeout
		"{ObjDir}"Recover.makeout
		Make -f "{ObjDir}"SlashEM.make Dependencies > "{ObjDir}"SlashEM.makeout
		"{ObjDir}"SlashEM.makeout

"{ObjDir}"Levels.list Ä
		Echo 'LevelList Ä ¶' > "{ObjDir}"Levels.list
		For file in "{Top}"dat:Å.des
			StreamEdit "{file}" -d -e '/MAZE:[ ]*¶"(Å)¨1¶"/||/LEVEL:[ ]*¶"(Å)¨1¶"/ Change "    ¶"{ObjDir}¶""¨1".lev ¶¶";Print;Exit' >> "{ObjDir}"Levels.list
		End
		Echo "" >> "{ObjDir}"Levels.list

#---------------- Clean -------------------

Clean	Ä
		set exit 0
		Delete -i -y "{Results}"
		Delete -i -y "{ObjDir}"MakeDefs.make "{ObjDir}"MakeDefs.makeout "{ObjDir}"MakeDefs.xcoff "{ObjDir}"MakeDefs
		Delete -i -y "{ObjDir}"LevComp.make "{ObjDir}"LevComp.makeout "{ObjDir}"LevComp.xcoff "{ObjDir}"LevComp
		Delete -i -y "{ObjDir}"DgnComp.make "{ObjDir}"DgnComp.makeout "{ObjDir}"DgnComp.xcoff "{ObjDir}"DgnComp
		Delete -i -y "{ObjDir}"Recover.make "{ObjDir}"Recover.makeout "{ObjDir}"Recover.xcoff
		Delete -i -y "{ObjDir}"SlashEM.make "{ObjDir}"SlashEM.makeout "{ObjDir}"SlashEM.xcoff
		Delete -i -y Macintosh.makeout "{ObjDir}"Levels.list "{ObjDir}"Levels.makeout
		Delete -i -y "{ObjDir}"Å.SYM "{ObjDir}"Å.NJ
		Delete -i -y "{ObjDir}"Å.c.Å
		Delete -i -y "{ObjDir}"date.h "{ObjDir}"dungeon.pdf "{ObjDir}"filename.h  
		Delete -i -y "{ObjDir}"monstr.c "{ObjDir}"onames.h
		Delete -i -y "{ObjDir}"pm.h "{ObjDir}"vis_tab.c "{ObjDir}"vis_tab.h
		Delete -i -y "{ObjDir}"Å.lev
		Delete -i -y "{ObjDir}"data "{ObjDir}"dungeon "{ObjDir}"options "{ObjDir}"oracles
		Delete -i -y "{ObjDir}"rumors "{ObjDir}"quest.dat
		Delete -i -y "{Top}"Å.rej
		Delete -i -y "{Top}"Å.orig
		Delete -i -y "{Top}"Å:Å.rej
		Delete -i -y "{Top}"Å:Å.orig
		Delete -i -y "{Top}"Å:Å:Å.rej
		Delete -i -y "{Top}"Å:Å:Å.orig
		Delete -i -y "{Top}"Å:Å:Å:Å.rej
		Delete -i -y "{Top}"Å:Å:Å:Å.orig
		Move Macintosh.make "{MacDir}"Macintosh.make
		set exit 1

#---------------- Spotless -------------------

Spotless	Ä	Clean
		set exit 0
		Delete -i -y "{Top}"ObjPowerPC "{Top}"Obj68K "{Top}"ObjFat "{Top}"DungeonPowerPC "{Top}"Dungeon68K "{Top}"DungeonFat
		Delete -i -y "{ObjDir}"NHrsrc.rsrc "{ObjDir}"NHsound.rsrc "{ObjDir}"MRecover.rsrc
		set exit 1

#---------------- Pack -------------------

Pack	Ä	"{ObjDir}"NHrsrc.rsrc "{ObjDir}"NHsound.rsrc "{ObjDir}"MRecover.rsrc
		DeRez "{ObjDir}"NHrsrc.rsrc Types.r SysTypes.r > "{MacDir}"NHrsrc.r
		DeRez "{ObjDir}"NHsound.rsrc Types.r SysTypes.r > "{MacDir}"NHsound.r
		DeRez "{ObjDir}"MRecover.rsrc Types.r SysTypes.r > "{MacDir}"MRecover.r

#---------------- Unpack -------------------

Unpack	Ä	"{ObjDir}"NHrsrc.rsrc "{ObjDir}"NHsound.rsrc "{ObjDir}"MRecover.rsrc

# This is a complete MPW makefile for Nethack and all its associated files
# Requires MPW C3.2§3 or later. Earlier versions will fail horribly because
# they can't cope with /* /* */ comments
#
# For NetHack 3.1 Nov 1991
#
# by Michaelª Hamel and Ross Brown 1991 : michael@otago.ac.nz
#
# Updated for Slash'EM 0.0.5-2 and later
# Paul Hurtley, August 1999 and later

#--------------------------------------------------------------------------
#
# BEFORE YOU BUILD FOR THE FIRST TIME

#  Set this equate to the folder containing all the Slash'EM source folders
Top   		= Vance:SlashEM:SlashEM-OS9:slashem-cvs:slashem:

#  Choose your target processor. There are bugs in the 68K version, so I 
#  recommend you select 'PowerPC'
Processor  = PowerPC
#Processor	= 68K
#Processor	= Fat

#  Choose a Debug (ie, -sym) or non-Debug build
#Debug = -sym
Debug =

#--------------------------------------------------------------------------
#
# Set up symbols for folders from the distribution
Src			= {Top}Src:
Util		= {Top}Util:
Dat			= {Top}Dat:
Doc			= {Top}Doc:
Include		= {Top}Include:
MacDir		= {Top}Sys:Mac:
TtyDir		= {Top}Win:Tty:
MacTty		= {Top}Sys:Mac:
Share		= {Top}Sys:Share:

# These folders are new
ObjDir     = {Top}:Obj{Processor}{Debug}:		# "Temporary" stuff
LibDir     = {Top}:Lib:							# "Temporary" stuff
Preserve   = {Top}:Preserve:					# Generated items for source distribution
Results	   = {Top}:Dungeon{Processor}{Debug}:	# Where the game goes

# Override the settings in "{Include}"config.h
# NHConfig = -d PORT_LEVEL='¶".0¶"' 

# Type and creator - note that we use "PREF" for all non-save files
# currently. Since we will integrate the data files into the game
# sooner or later, this doesn't matter much.
FileType   = PREF
SlashEMCreator = slEm
RecoverCreator = slRc

#
#  Yacc/Lex ... if you got 'em, set the following to values appropriate for
#  your tools.
#
YACC   = bison -y
LEX    = flex
YTabC  = y.tab.c
YTabH  = y.tab.h
LexYYC = lex.yy.c

# Macintosh is an abstract target which in fact consists of:

# The following files are in the binary distribution
DataFiles 	=	"{Results}"Guidebook		¶
				"{Results}"MacHelp			¶
				"{Results}"License			¶
				"{Results}"Slash¶'EM¶ Defaults   ¶
				"{Results}"README

# The following files are generated, but included in the source distribution anyway				
Preserved	=	"{Preserve}"date.h 		¶
				"{Preserve}"pm.h 		¶
				"{Preserve}"onames.h 	¶
				"{Preserve}"monstr.c  	¶
				"{Preserve}"vis_tab.h 	¶
				"{Preserve}"vis_tab.c 	¶
				"{Preserve}"filename.h 	¶
				"{Preserve}"lev_yacc.c 	¶
				"{Preserve}"lev_comp.h 	¶
				"{Preserve}"dgn_yacc.c 	¶
				"{Preserve}"dgn_comp.h

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
	If Not "`Exists -d "{Preserve}"`"
		NewFolder "{Preserve}"
	End
	If Not "`Exists -d "{LibDir}"`"
		NewFolder "{LibDir}"
	End
	
COptions = {NHConfig}					¶
		   -w 2 -w 3 -ansi relaxed -typecheck relaxed -align power

#------------------- Files included as resources -----------------

FileResources = ¶
	"{Dat}"cmdhelp ¶
	"{Dat}"help ¶
	"{Dat}"hh ¶
	"{Dat}"history ¶
	"{Dat}"license ¶
	"{MacDir}"MacHelp ¶
	"{MacDir}"News ¶
	"{Dat}"opthelp ¶
	"{Dat}"wizhelp ¶
	"{LibDir}"Quest.dat ¶
	"{LibDir}"data ¶
	"{LibDir}"dungeon ¶
	"{LibDir}"options ¶
	"{LibDir}"oracles ¶
	"{LibDir}"rumors ¶
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
	"{Src}"mapglyph.c ¶
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
	"{Preserve}"monstr.c ¶
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
	"{Preserve}"vis_tab.c ¶
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

SlashEMRsrcs = 	¶
	"{MacDir}"NHrsrc.r ¶
	"{MacDir}"NHsound.r ¶
	"{MacDir}"Files.r ¶
	"{LibDir}"Levels.r

# -------- Build the dungeon compiler, as an MPW tool ---------------

DgnCompSrcs = 				¶
	"{Preserve}"dgn_lex.c	¶
	"{Util}"dgn_main.c		¶
	"{Preserve}"dgn_yacc.c	¶
	"{Src}"alloc.c			¶
	"{Util}"panic.c

"{ObjDir}"DgnComp.make	Ä	 "{Preserve}"dgn_lex.c "{Preserve}"dgn_yacc.c
	CreateMake "{ObjDir}"DgnComp {DgnCompSrcs} -tool -powerpc -objdir "{ObjDir}" ¶
		-i "{Preserve}" -i "{ObjDir}" -i {Include} -ppccoptions "{COptions}" {Debug} -depends
	
"{ObjDir}"DgnComp DgnComp.lnk Ä "{ObjDir}"DgnComp.make
	BuildProgram "{ObjDir}"DgnComp

"{Preserve}"dgn_comp.h "{Preserve}"dgn_yacc.c	Ä  "{Util}"dgn_comp.y
	{YACC} -d "{Util}"dgn_comp.y
	Move -y "{YTabC}" "{Preserve}"dgn_yacc.c
	Move -y "{YTabH}" "{Preserve}"dgn_comp.h
	
{Preserve}dgn_lex.c	Ä  {Util}dgn_comp.l
	{LEX} {Util}dgn_comp.l
	Move -y {LexYYC} {Preserve}dgn_lex.c
	
# -------- Build the special-level compiler, as an MPW tool ---------------

LevCompSrcs= "{Src}"monst.c		¶
		 "{Src}"objects.c		¶
		 "{Src}"drawing.c		¶
		 "{Src}"alloc.c			¶
		 "{Util}"panic.c		¶
		 "{Preserve}"lev_lex.c	¶
		 "{Preserve}"lev_yacc.c	¶
		 "{MacDir}"macfile.c	¶
		 "{MacDir}"macerrs.c	¶
		 "{Src}"files.c			¶
		 "{Src}"decl.c			¶
		 "{Util}"lev_main.c

"{ObjDir}"LevComp.make	Ä	 "{Preserve}"lev_lex.c "{Preserve}"lev_yacc.c
	CreateMake "{ObjDir}"LevComp {LevCompSrcs} -tool -powerpc -objdir "{ObjDir}" ¶
		-i "{Preserve}" -i "{ObjDir}" -i {Include} -ppccoptions "{COptions}" {Debug} -depends
	
"{ObjDir}"LevComp LevComp.lnk Ä "{ObjDir}"LevComp.make
	BuildProgram "{ObjDir}"LevComp

"{Preserve}"lev_comp.h "{Preserve}"lev_yacc.c	Ä  "{Util}"lev_comp.y
	{YACC} -d "{Util}"lev_comp.y
	Move -y "{YTabH}" "{Preserve}"lev_comp.h
	Move -y "{YTabC}" "{Preserve}"lev_yacc.c
	
"{Preserve}"lev_lex.c	Ä  {Util}lev_comp.l
	{LEX} {Util}lev_comp.l
	Move -y {LexYYC} {Preserve}lev_lex.c
	
# -------- Create list of actual levels ---------------

"{LibDir}"Levels.r Ä Levels
	Set Pwd `Directory`
	Directory "{LibDir}"
	Echo "" > "{LibDir}"Levels.r
	Set index 1200
	For file in Å.lev
		Echo "read 'File' ({index},¶"{file}¶") ¶"::lib:{file}¶";" >> "{LibDir}"Levels.r 
		Set index `Evaluate {index} + 1`
	End
	Echo "" >> "{LibDir}"Levels.list
	Directory "{Pwd}"

# -------- Build "{ObjDir}"MakeDefs, as an MPW tool ---------------

MakeDefsSrcs= "{Src}"objects.c		¶
			  "{Src}"monst.c		¶
			  "{Util}"MakeDefs.c

"{ObjDir}"MakeDefs.make Ä
	CreateMake "{ObjDir}"MakeDefs {MakeDefsSrcs} -tool -powerpc -objdir "{ObjDir}" ¶
		-i "{Preserve}" -i "{ObjDir}" -i {Include} -ppccoptions "{COptions}" {Debug} -depends

"{ObjDir}"MakeDefs MakeDefs.lnk Ä "{ObjDir}"MakeDefs.make
	BuildProgram "{ObjDir}"MakeDefs

# ------------ If "{ObjDir}"MakeDefs changes we need to rebuild some include files -----------

"{Preserve}"filename.h 	Ä 	"{ObjDir}"MakeDefs
	Set Pwd `Directory`
	Directory "{Top}"
	"{ObjDir}"MakeDefs -f
	Move -y "{Include}"filename.h "{Preserve}"filename.h
	Directory "{Pwd}"

"{Preserve}"date.h 	Ä 	"{LibDir}"Options "{ObjDir}"MakeDefs
	Move -y "{Include}"date.h "{Preserve}"date.h

"{LibDir}"Options	Ä	$OutOfDate "{ObjDir}"MakeDefs
	Set Pwd `Directory`
	Directory "{Top}"
	"{ObjDir}"MakeDefs -v
	Directory "{Pwd}"
	
"{Preserve}"onames.h Ä	"{ObjDir}"MakeDefs
	Set Pwd `Directory`
	Directory "{Top}"
	"{ObjDir}"MakeDefs -o
	Move -y "{Include}"onames.h "{Preserve}"onames.h
	Directory "{Pwd}"

"{Preserve}"pm.h 	Ä	"{ObjDir}"MakeDefs
	Set Pwd `Directory`
	Directory "{Top}"
	"{ObjDir}"MakeDefs -p
	Move -y "{Include}"pm.h "{Preserve}"pm.h
	Directory "{Pwd}"

"{Preserve}"vis_tab.c	Ä	"{Preserve}"vis_tab.h
	Move -y "{Src}"vis_tab.c "{Preserve}"vis_tab.c

"{Preserve}"vis_tab.h Ä	"{ObjDir}"MakeDefs
	Set Pwd `Directory`
	Directory "{Top}"
	"{ObjDir}"MakeDefs -z
	Move -y "{Include}"vis_tab.h "{Preserve}"vis_tab.h
	Directory "{Pwd}"

"{ObjDir}"Dungeon.pdf  Ä "{Dat}"Dungeon.def "{ObjDir}"MakeDefs
	Set Pwd `Directory`
	Directory "{Top}"
	"{ObjDir}"MakeDefs -e
	Move -y "{Dat}"Dungeon.pdf "{ObjDir}"Dungeon.pdf
	Directory "{Pwd}"

"{Preserve}"monstr.c Ä "{ObjDir}"MakeDefs
	Set Pwd `Directory`
	Directory "{Top}"
	"{ObjDir}"MakeDefs -m
	Move -y "{Src}"monstr.c "{Preserve}"monstr.c
	Directory "{Pwd}"

#---------------- Data files -------------------

Levels Ä "{LibDir}"Levels.list "{MacDir}"Levels.make
		Make -f "{MacDir}"Levels.make Levels -d Dat="{Dat}" -d ObjDir="{ObjDir}" -d LibDir="{LibDir}" > "{ObjDir}"Levels.makeout
		"{ObjDir}"Levels.makeout
		
"{LibDir}"Data data Ä "{Dat}"Data.base "{ObjDir}"MakeDefs
		Set Pwd `Directory`
		Directory "{Top}"
	    "{ObjDir}"MakeDefs -d
		SetFile -t "{FileType}" -c "{SlashEMCreator}" "{LibDir}"Data
		Directory "{Pwd}"

"{LibDir}"Rumors Ä "{Dat}"Rumors.tru "{Dat}"Rumors.fal "{ObjDir}"MakeDefs
		Set Pwd `Directory`
		Directory "{Top}"
	    "{ObjDir}"MakeDefs -r
		SetFile -t "{FileType}" -c "{SlashEMCreator}" "{LibDir}"Rumors
		Directory "{Pwd}"

"{LibDir}"Oracles Ä "{Dat}"Oracles.txt "{ObjDir}"MakeDefs
		Set Pwd `Directory`
		Directory "{Top}"
	    "{ObjDir}"MakeDefs -h
		SetFile -t "{FileType}" -c "{SlashEMCreator}" "{LibDir}"Oracles
		Directory "{Pwd}"

"{Results}"Slash¶'EM¶ Defaults  Ä	"{MacDir}"NHDeflts 
		Duplicate -y "{MacDir}"NHDeflts "{Results}"Slash¶'EM¶ Defaults
		SetFile -c "ttxt" "{Results}"Slash¶'EM¶ Defaults

"{Results}"Guidebook  Ä	"{Doc}"Guidebook.txt
		Duplicate -y "{Doc}"Guidebook.txt "{Results}"Guidebook
		SetFile -c "ttxt" -t "ttro" "{Results}"Guidebook
		
"{Results}"MacHelp  Ä	"{MacDir}"MacHelp
		Duplicate -y "{MacDir}"MacHelp "{Results}"MacHelp
		SetFile -c "ttxt" -t "ttro" "{Results}"MacHelp
		
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

"{ObjDir}"News	Ä	"{MacDir}"News
		Duplicate -y "{MacDir}"News "{ObjDir}"News

"{LibDir}"Dungeon	Ä "{ObjDir}"Dungeon.pdf "{ObjDir}"DgnComp
		"{ObjDir}"DgnComp "{ObjDir}"dungeon.pdf
		Move -y "{ObjDir}"Dungeon "{LibDir}"
		SetFile -t "{FileType}" -c "{SlashEMCreator}" "{LibDir}"Dungeon

"{LibDir}"Quest.dat	Ä "{Dat}"Quest.txt "{ObjDir}"MakeDefs
		Set Pwd `Directory`
		Directory "{Top}"
	    "{ObjDir}"MakeDefs -q
		SetFile -t "{FileType}" -c "{SlashEMCreator}" "{LibDir}"Quest.dat
		Directory "{Pwd}"

#---------------- The Recover application -------------------

RecoverSrcs = "{MacDir}"MRecover.c

RecoverRsrcs = ¶
	"{MacDir}"MFiles.r ¶
	"{MacDir}"MRecover.r
	
"{ObjDir}"Recover.make	Ä
		CreateMake "{ObjDir}"Recover {RecoverSrcs} -{Processor} ¶
			-i "{Preserve}" -i "{ObjDir}" -i {Include} ¶
			-objdir "{ObjDir}" {Debug} -depends -c {RecoverCreator} ¶
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
		Rez Types.r SysTypes.r {RecoverRsrcs} -o "{ObjDir}"Recover -c {RecoverCreator} ¶
			-i "{MacDir}" -i "{ObjDir}"  -i "{Include}" -i "{Preserve}" -s "{Top}" -append
		SetFile -a B "{ObjDir}"Recover
		Move -y "{ObjDir}"Recover "{Results}"Recover 	

#---------------- The Slash'EM application -------------------

"{ObjDir}"SlashEM.make	Ä 
		CreateMake "{ObjDir}"SlashEM {SlashEMSrcs} -{Processor} ¶
			-i "{Preserve}" -i "{ObjDir}" -i {Include} ¶
			-objdir "{ObjDir}" {Debug} -depends -c {SlashEMCreator} ¶
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

"{Results}"Slash¶'EM SlashEM.lnk Ä "{ObjDir}"SlashEM.make {SlashEMRsrcs} {FileResources}
		BuildProgram "{ObjDir}"SlashEM
		Rez Types.r SysTypes.r {SlashEMRsrcs} -o "{ObjDir}"SlashEM -c {SlashEMCreator} ¶
			-i "{MacDir}" -i "{ObjDir}"  -i "{Include}" -i "{Preserve}" -s "{Top}" -append
		SetFile -a B "{ObjDir}"SlashEM
		Move -y "{ObjDir}"SlashEM "{Results}"Slash¶'EM 	

#---------------- Dependencies -------------------

Dependencies Ä	Setup					¶
				"{LibDir}"Levels.list	¶
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

"{LibDir}"Levels.list Ä
		Echo 'LevelList Ä ¶' > "{LibDir}"Levels.list
		For file in "{Top}"dat:Å.des
			StreamEdit "{file}" -d -e '/MAZE:[ ]*¶"(Å)¨1¶"/||/LEVEL:[ ]*¶"(Å)¨1¶"/ Change "    ¶"{LibDir}¶""¨1".lev ¶¶";Print;Exit' >> "{LibDir}"Levels.list
		End
		Echo "" >> "{LibDir}"Levels.list
		
#---------------- Clean -------------------

Clean	Ä
		set exit 0
		Delete -i -y "{Results}"
		Delete -i -y "{ObjDir}
		Delete -i -y "{LibDir}
		Delete -i -y Macintosh.makeout
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
		Delete -i -y "{Preserve}"
		set exit 1

#---------------- Pack -------------------

Pack	Ä	"{ObjDir}"NHrsrc.rsrc "{ObjDir}"NHsound.rsrc "{ObjDir}"MRecover.rsrc
		DeRez "{ObjDir}"NHrsrc.rsrc Types.r SysTypes.r > "{MacDir}"NHrsrc.r
		DeRez "{ObjDir}"NHsound.rsrc Types.r SysTypes.r > "{MacDir}"NHsound.r
		DeRez "{ObjDir}"MRecover.rsrc Types.r SysTypes.r > "{MacDir}"MRecover.r

#---------------- Unpack -------------------

Unpack	Ä	"{ObjDir}"NHrsrc.rsrc "{ObjDir}"NHsound.rsrc "{ObjDir}"MRecover.rsrc

"{ObjDir}"NHrsrc.rsrc Ä "{MacDir}"NHrsrc.r
		Rez Types.r SysTypes.r "{MacDir}"NHrsrc.r -o "{ObjDir}"NHrsrc.rsrc ¶
			-i "{Include}" -i "{Preserve}"

"{ObjDir}"NHsound.rsrc Ä "{MacDir}"NHsound.r
		Rez Types.r SysTypes.r "{MacDir}"NHsound.r -o "{ObjDir}"NHsound.rsrc

"{ObjDir}"MRecover.rsrc	Ä	"{MacDir}"MRecover.r
		Rez Types.r SysTypes.r "{MacDir}"MRecover.r -o "{ObjDir}"MRecover.rsrc
	

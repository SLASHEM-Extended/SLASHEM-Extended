#	NetHack Makefile.
#	SCCS Id: @(#)Makefile.top	3.4	1995/01/05

# newer makes predefine $(MAKE) to 'make' and do smarter processing of
# recursive make calls if $(MAKE) is used
# these makes allow $(MAKE) to be overridden by the environment if someone
# wants to (or has to) use something other than the standard make, so we do
# not want to unconditionally set $(MAKE) here
#
# unfortunately, some older makes do not predefine $(MAKE); if you have one of
# these, uncomment the following line
# (you will know that you have one if you get complaints about unable to
# execute things like 'data' and 'rumors')
# MAKE = make

# make NetHack
PREFIX   = /usr
GAME     = slashem
# GAME     = slashem.prg
GAMEUID  = games
GAMEGRP  = bin

# Permissions - some places use setgid instead of setuid, for instance
# See also the option "SECURE" in include/config.h
GAMEPERM = 04755
FILEPERM = 0644
EXEPERM  = 0755
DIRPERM  = 0755
VARFILEPERM = 0644
VARDIRPERM  = 0755

# GAMEDIR also appears in config.h as "HACKDIR".
# VARDIR may also appear in unixconf.h as "VAR_PLAYGROUND" else GAMEDIR
#
# note that 'make install' believes in creating nice tidy directories for
# installation, free of debris from previous Slash'EM versions -- therefore
# there should not be anything in these directories that you want to keep
# (if there is, you'll have to do the installation by hand or modify the
# instructions)
# These must agree with the definitions in unixconf.h

# Defs. for non file areas support.
GAMEDIR  = $(PREFIX)/local/slashemdir
VARDIR = $(GAMEDIR)
FILE_AREA_VAR = $(VARDIR)
FILE_AREA_SAVE = $(VARDIR)/save
FILE_AREA_SHARE = $(GAMEDIR)
FILE_AREA_UNSHARE = $(GAMEDIR)
FILE_AREA_DOC = $(GAMEDIR)
FILE_AREA_BONES = $(FILE_AREA_VAR)
FILE_AREA_LEVL = $(FILE_AREA_VAR)
SHELLDIR = $(PREFIX)/local/bin

# Defs. compatible with Linux's FSSTND 1.2
# FILE_AREA_VAR = /var/lib/games/slashem
# FILE_AREA_SAVE = /var/lib/games/slashem/save
# FILE_AREA_SHARE = /usr/lib/games/slashem
# FILE_AREA_UNSHARE = /usr/lib/games/slashem
# FILE_AREA_DOC = /usr/doc/slashem
# FILE_AREA_BONES = $(FILE_AREA_VAR)
# FILE_AREA_LEVL = $(FILE_AREA_VAR)
# SHELLDIR = /usr/games

# Defs. compatible with FHS 2.0
# FILE_AREA_VAR = /var/games/slashem
# FILE_AREA_SAVE = /var/games/slashem/save
# FILE_AREA_SHARE = /usr/share/games/slashem
# FILE_AREA_UNSHARE = /usr/lib/games/slashem
# FILE_AREA_DOC = /usr/share/doc/slashem
# FILE_AREA_BONES = $(FILE_AREA_VAR)
# FILE_AREA_LEVL = $(FILE_AREA_VAR)
# SHELLDIR = /usr/games

# KDEDIR = /opt/kde
# SHELLDIR = $(KDEDIR)/bin

# Shareable (architecture independent) data files which are not candidates
# for inclusion in DLB libraries (ND) and which are (D). Port specific files
# per discussions in Install.X11, Install.Qt and Install.GTK
CNF_SHARE_QTND = x11tiles rip.xpm nhsplash.xpm
CNF_SHARE_X11ND = x11bigtiles x11tiles SlashEM.ad pet_mark.xbm
# CNF_SHARE_X11ND = x11bigtiles x11tiles SlashEM.ad pet_mark.xbm rip.xpm
CNF_SHARE_GTKND = x11big3dtiles x11bigtiles x11tiles rip.xpm credit.xpm gtkrc
CNF_SHARE_GTKPROXYND = x11big3dtiles.map x11bigtiles.map x11tiles.map
# for Atari/Gem
CNF_SHARE_GEMND = nh16.img title.img GEM_RSC.RSC rip.img
# for BeOS
CNF_SHARE_BEND = beostiles
# for SDL/GL window port
CNF_SHARE_GLND = gltile16.png gltile32.png gltile64.png gllogo.png glrip.png \
                 glfont8.png glfont14.png glfont20.png glfont22.png
# for Gnome
CNF_SHARE_GNOMEND = x11bigtiles x11tiles pet_mark.xbm rip.xpm mapbg.xpm
# for Proxy
CNF_SHARE_PROXYND = gltile64.png gltile32.png gltile16.png \
	gltile64.map gltile32.map gltile16.map

# CNF_SHARE_DATND = $(CNF_SHARE_X11ND)
# CNF_SHARE_DATND = $(CNF_SHARE_QTND)
# CNF_SHARE_DATND = $(CNF_SHARE_GTKND)
# CNF_SHARE_DATND = $(CNF_SHARE_GTKND) $(CNF_SHARE_GTKPROXYND)
# CNF_SHARE_DATND = $(CNF_SHARE_GLND)
# CNF_SHARE_DATND = $(CNF_SHARE_PROXYND)
CNF_SHARE_DATND =

CNF_SHARE_DATD = data oracles options quest.dat rumors
CNF_SHARE_DAT = $(CNF_SHARE_DATD) $(CNF_SHARE_DATND)

# Some versions of make use the SHELL environment variable as the shell
# for running commands.  We need this to be a Bourne shell.
# SHELL = /bin/sh
# for Atari
# SHELL=E:/GEMINI2/MUPFEL.TTP

# Commands for setting the owner and group on files during installation.
# Some systems fail with one or the other when installing over NFS or for
# other permission-related reasons.  If that happens, you may want to set the
# command to "true", which is a no-op. Note that disabling chown or chgrp
# will only work if setuid (or setgid) behavior is not desired or required.
CHOWN = chown
CHGRP = chgrp

#
# end of configuration
#

DATHELP = help hh cmdhelp history opthelp wizhelp gypsy.txt

SPEC_LEVS = asmodeus.lev baalz.lev bigrm-?.lev castle.lev fakewiz?.lev \
	juiblex.lev knox.lev medusa-?.lev minend-?.lev minefill.lev \
	minetn-?.lev oracle.lev orcus.lev sanctum.lev soko?-?.lev \
	tower?.lev valley.lev wizard?.lev \
	astral.lev air.lev earth.lev fire.lev water.lev \
	beholder.lev blkmar.lev cav2fill.lev \
	grund-?.lev demogorg.lev dispater.lev dragons.lev geryon.lev \
	guild.lev kobold-?.lev lich.lev mall-?.lev mall.lev mineking.lev \
	mtemple.lev nymph.lev rats.lev sea.lev spiders.lev stor-?.lev \
	tomb.lev yeenoghu.lev frnknstn.lev nightmar.lev
QUEST_LEVS = ???-goal.lev ???-fil?.lev ???-loca.lev ???-strt.lev

SHARE_DATNODLB = $(CNF_SHARE_DATND)
UNSHARE_DATNODLB =
SHARE_DATDLB = $(DATHELP) $(CNF_SHARE_DATD)
UNSHARE_DATDLB = dungeon $(SPEC_LEVS) $(QUEST_LEVS)
SHARE_DAT = $(SHARE_DATNODLB) $(SHARE_DATDLB)
UNSHARE_DAT = $(UNSHARE_DATNODLB) $(UNSHARE_DATDLB)

$(GAME):
	( cd src ; $(MAKE) )

all:	$(GAME) recover Guidebook $(CNF_SHARE_DAT) dungeon spec_levs check-dlb
	@echo "Done."

# Note: many of the dependencies below are here to allow parallel make
# to generate valid output

Guidebook:
	( cd doc ; $(MAKE) Guidebook )

manpages:
	( cd doc ; $(MAKE) manpages )

data: $(GAME)
	( cd dat ; $(MAKE) data )

rumors: $(GAME)
	( cd dat ; $(MAKE) rumors )

oracles: $(GAME)
	( cd dat ; $(MAKE) oracles )

#	Note: options should have already been made with make, but...
options: $(GAME)
	( cd dat ; $(MAKE) options )

quest.dat: $(GAME)
	( cd dat ; $(MAKE) quest.dat )

spec_levs: $(GAME)
	( cd util ; $(MAKE) lev_comp )
	( cd dat ; $(MAKE) spec_levs )
	( cd dat ; $(MAKE) quest_levs )

dungeon: $(GAME)
	( cd util ; $(MAKE) dgn_comp )
	( cd dat ; $(MAKE) dungeon )

Guidebook.txt:
	( cd doc; $(MAKE) Guidebook.txt )

nhtiles.bmp: $(GAME)
	( cd dat ; $(MAKE) nhtiles.bmp )

x11tiles: $(GAME)
	( cd util ; $(MAKE) tile2x11 )
	( cd dat ; $(MAKE) x11tiles )

x11bigtiles: $(GAME)
	( cd util ; $(MAKE) tile2x11 magtile txtmerge txtbg )
	( cd dat ; $(MAKE) x11bigtiles )

x11big3dtiles: $(GAME)
	( cd util ; $(MAKE) tile2x11 bigtile txtmerge )
	( cd dat ; $(MAKE) x11big3dtiles )

x11tiles.map: x11tiles
	( cd util ; $(MAKE) mapmerge )
	( cd dat ; $(MAKE) x11tiles.map )

x11bigtiles.map: x11bigtiles
	( cd util ; $(MAKE) mapmerge )
	( cd dat ; $(MAKE) x11bigtiles.map )

x11big3dtiles.map: x11big3dtiles
	( cd util ; $(MAKE) mapmerge )
	( cd dat ; $(MAKE) x11big3dtiles.map )

beostiles: $(GAME)
	( cd util ; $(MAKE) tile2beos )
	( cd dat ; $(MAKE) beostiles )

gl%.png: $(GAME)
	( cd util ; $(MAKE) tile2png )
	( cd dat ; $(MAKE) $@ )

gl%.map: gl%.png
	( cd util ; $(MAKE) mapmerge )
	( cd dat ; $(MAKE) $@ )

util/txt2ppm:
	( cd util ; $(MAKE) txt2ppm )

%.xpm:
	( cd dat ; $(MAKE) $*.xpm )

%.pbm:
	( cd dat ; $(MAKE) $*.pbm )

SlashEM.ad: $(GAME)
	( cd dat ; $(MAKE) SlashEM.ad )

pet_mark.xbm:
	( cd dat ; $(MAKE) pet_mark.xbm )

rip.xpm:
	( cd dat ; $(MAKE) rip.xpm )

mapbg.xpm:
	(cd dat ; $(MAKE) mapbg.xpm )

nhsplash.xpm:
	( cd dat ; $(MAKE) nhsplash.xpm )

nh16.img: $(GAME)
	( cd util ; $(MAKE) tile2img.ttp )
	( cd dat ; $(MAKE) nh16.img )

rip.img:
	( cd util ; $(MAKE) xpm2img.ttp )
	( cd dat ; $(MAKE) rip.img )
GEM_RSC.RSC:
	( cd dat ; $(MAKE) GEM_RSC.RSC )

title.img:
	( cd dat ; $(MAKE) title.img )

gtkrc:	win/gtk/GTKRC
	cp win/gtk/GTKRC dat/gtkrc

check-dlb: options
	@if egrep -s librarian dat/options ; then $(MAKE) dlb ; else true ; fi

dlb:
	( cd util ; $(MAKE) dlb )
	( cd dat ; ../util/dlb cf nhshare $(SHARE_DATDLB) )
	( cd dat ; ../util/dlb cf nhushare $(UNSHARE_DATDLB) )

# recover can be used when INSURANCE is defined in include/config.h
# and the checkpoint option is true
recover: $(GAME)
	( cd util ; $(MAKE) recover )

dofiles:
	target=`sed -n					\
		-e '/librarian/{' 			\
		-e	's/.*/dlb/p' 			\
		-e	'q' 				\
		-e '}' 					\
	  	-e '$$s/.*/nodlb/p' < dat/options` ;	\
	$(MAKE) dofiles-$${target-nodlb}
	cp src/$(GAME) $(FILE_AREA_UNSHARE)
	cp util/recover $(FILE_AREA_UNSHARE)
	cp dat/license doc/Guidebook.txt $(FILE_AREA_DOC)
#	cp win/Qt/KDE/kslashem.kdelnk $(KDEDIR)/share/applnk/Games
	-rm -f $(SHELLDIR)/$(GAME)
	sed -e 's;/usr/games/lib/nethackdir;$(FILE_AREA_UNSHARE);' \
		-e 's;HACKDIR/nethack;HACKDIR/$(GAME);' \
		< sys/unix/nethack.sh \
		> $(SHELLDIR)/$(GAME)
# set up their permissions
	-( cd $(FILE_AREA_UNSHARE) ; $(CHOWN) $(GAMEUID) $(GAME) recover ; \
			$(CHGRP) $(GAMEGRP) $(GAME) recover )
	chmod $(GAMEPERM) $(FILE_AREA_UNSHARE)/$(GAME)
	chmod $(EXEPERM) $(FILE_AREA_UNSHARE)/recover
	-( cd $(FILE_AREA_DOC) ; \
			$(CHOWN) $(GAMEUID) license Guidebook.txt ; \
			$(CHGRP) $(GAMEGRP) license Guidebook.txt ; \
			chmod $(FILEPERM) license Guidebook.txt )
	-$(CHOWN) $(GAMEUID) $(SHELLDIR)/$(GAME)
	$(CHGRP) $(GAMEGRP) $(SHELLDIR)/$(GAME)
	chmod $(EXEPERM) $(SHELLDIR)/$(GAME)

dofiles-dlb: check-dlb
	( cd dat ; cp nhshare $(SHARE_DATNODLB) $(FILE_AREA_SHARE) )
	( cd dat ; cp nhushare $(UNSHARE_DATNODLB) $(FILE_AREA_UNSHARE) )
# set up their permissions
	-( cd $(FILE_AREA_SHARE) ; \
			$(CHOWN) $(GAMEUID) nhshare $(SHARE_DATNODLB) ; \
			$(CHGRP) $(GAMEGRP) nhshare $(SHARE_DATNODLB) ; \
			chmod $(FILEPERM) nhshare $(SHARE_DATNODLB) )
	-( cd $(FILE_AREA_UNSHARE) ; \
			$(CHOWN) $(GAMEUID) nhushare $(UNSHARE_DATNODLB) ; \
			$(CHGRP) $(GAMEGRP) nhushare $(UNSHARE_DATNODLB) ; \
			chmod $(FILEPERM) nhushare $(UNSHARE_DATNODLB) )

dofiles-nodlb:
# copy over the game files
	( cd dat ; cp $(SHARE_DAT) $(FILE_AREA_SHARE) )
	( cd dat ; cp $(UNSHARE_DAT) $(FILE_AREA_UNSHARE) )
# set up their permissions
	-( cd $(FILE_AREA_SHARE) ; $(CHOWN) $(GAMEUID) $(SHARE_DAT) ; \
			$(CHGRP) $(GAMEGRP) $(SHARE_DAT) ; \
			chmod $(FILEPERM) $(SHARE_DAT) )
	-( cd $(FILE_AREA_UNSHARE) ; $(CHOWN) $(GAMEUID) $(UNSHARE_DAT) ; \
			$(CHGRP) $(GAMEGRP) $(UNSHARE_DAT) ; \
			chmod $(FILEPERM) $(UNSHARE_DAT) )

update: $(GAME) recover $(CNF_SHARE_DAT) dungeon spec_levs Guidebook.txt
#	(don't yank the old version out from under people who're playing it)
	-mv $(FILE_AREA_UNSHARE)/$(GAME) $(FILE_AREA_UNSHARE)/$(GAME).old
#	quest.dat is also kept open and has the same problems over NFS
#	(quest.dat may be inside nhushare if dlb is in use)
	-mv $(FILE_AREA_UNSHARE)/quest.dat $(FILE_AREA_UNSHARE)/quest.dat.old
	-mv $(FILE_AREA_UNSHARE)/nhushare $(FILE_AREA_UNSHARE)/nhushare.old
# set up new versions of the game files
	( $(MAKE) dofiles )
# touch time-sensitive files
	-touch -c $(FILE_AREA_BONES)/bones* $(FILE_AREA_VAR)/?lock*
	-touch -c $(FILE_AREA_VAR)/wizard* $(FILE_AREA_SAVE)/*
	touch $(FILE_AREA_VAR)/perm $(FILE_AREA_VAR)/record
# and a reminder
	@echo You may also want to install the man pages via the doc Makefile.

install: $(GAME) recover $(CNF_SHARE_DAT) dungeon spec_levs Guidebook.txt
# set up the directories
# not all mkdirs have -p; those that don't will create a -p directory
	-mkdir -p $(SHELLDIR)
	-rm -rf $(FILE_AREA_VAR) $(FILE_AREA_BONES) $(FILE_AREA_SAVE)
	-rm -rf $(FILE_AREA_LEVL) $(FILE_AREA_SHARE) $(FILE_AREA_UNSHARE)
	-rm -rf $(FILE_AREA_DOC)
	-mkdir -p $(FILE_AREA_VAR) $(FILE_AREA_BONES) $(FILE_AREA_SAVE)
	-mkdir -p $(FILE_AREA_LEVL) $(FILE_AREA_SHARE) $(FILE_AREA_UNSHARE)
	-mkdir -p $(FILE_AREA_DOC)
	-rmdir ./-p
	-$(CHOWN) $(GAMEUID) $(FILE_AREA_VAR) $(FILE_AREA_BONES) $(FILE_AREA_SAVE) $(FILE_AREA_LEVL)
	$(CHGRP) $(GAMEGRP) $(FILE_AREA_VAR) $(FILE_AREA_BONES) $(FILE_AREA_SAVE) $(FILE_AREA_LEVL)
	chmod $(VARDIRPERM) $(FILE_AREA_VAR) $(FILE_AREA_BONES) $(FILE_AREA_SAVE) $(FILE_AREA_LEVL)
	-$(CHOWN) $(GAMEUID) $(FILE_AREA_SHARE) $(FILE_AREA_UNSHARE)
	$(CHGRP) $(GAMEGRP) $(FILE_AREA_SHARE) $(FILE_AREA_UNSHARE)
	chmod $(DIRPERM) $(FILE_AREA_SHARE) $(FILE_AREA_UNSHARE)
# set up the game files
	( $(MAKE) dofiles )
# set up some additional files
	touch $(FILE_AREA_VAR)/perm $(FILE_AREA_VAR)/record $(FILE_AREA_VAR)/logfile
	-( cd $(FILE_AREA_VAR) ; $(CHOWN) $(GAMEUID) perm record logfile ; \
			$(CHGRP) $(GAMEGRP) perm record logfile ; \
			chmod $(VARFILEPERM) perm record logfile )
# and a reminder
	@echo You may also want to reinstall the man pages via the doc Makefile.


# 'make clean' removes all the .o files, but leaves around all the executables
# and compiled data files
clean:
	( cd src ; $(MAKE) clean )
	( cd util ; $(MAKE) clean )

# 'make spotless' returns the source tree to near-distribution condition.
# it removes .o files, executables, and compiled data files
spotless:
	( cd src ; $(MAKE) spotless )
	( cd util ; $(MAKE) spotless )
	( cd dat ; $(MAKE) spotless )
	( cd doc ; $(MAKE) spotless )

export VCS_DESCRIPTION := $(shell git describe --always 2>/dev/null)
VERSION := $(shell util/version.sh)

GAME = slashem
GAMEUID = acehack
GAMEGID = acehack
CHOWN = chown $(GAMEUID):$(GAMEGID)

GAMEDIR = /srv/acehack/slex-$(VERSION)
MASTERDIR = /srv/acehack/slexdir

GAMEPERM = 0755
FILEPERM = 0644
EXEPERM  = 0755
DIRPERM  = 0755

WIZARD = $(GAMEUID)

CFLAGS = -g -O2
CPPFLAGS += -Iinclude
CPPFLAGS += -D'WIZARD="$(WIZARD)"'
CPPFLAGS += -DNO_EXPLORE_MODE
CPPFLAGS += -DNO_SUSPEND
CPPFLAGS += -DDLB

.PHONY: all
all: src/$(GAME) util/recover dat/nhshare dat/nhushare

PLAYGROUND_FILES = perm record logfile xlogfile

.PHONY: install
install: all dat/license doc/Guidebook.txt
	rm -f $(GAMEDIR)/$(GAME) $(GAMEDIR)/nhshare $(GAMEDIR)/nhushare
	mkdir -p $(GAMEDIR) $(GAMEDIR)/save
	$(CHOWN) $(GAMEDIR) $(GAMEDIR)/save
	chmod $(DIRPERM) $(GAMEDIR) $(GAMEDIR)/save
	-ln $(PLAYGROUND_FILES:%=$(MASTERDIR)/%) $(GAMEDIR)
	( cd $(GAMEDIR) ; \
	  touch $(PLAYGROUND_FILES) ; \
	  $(CHOWN) $(PLAYGROUND_FILES) ; \
	  chmod $(FILEPERM) $(PLAYGROUND_FILES) )
	cp src/$(GAME) $(GAMEDIR)/$(GAME).new
	cp util/recover dat/nhshare dat/nhushare \
           dat/license doc/Guidebook.txt $(GAMEDIR)
	( cd $(GAMEDIR) ; \
	  $(CHOWN) $(GAME).new recover ; \
          $(CHOWN) nhshare nhushare license Guidebook.txt ; \
	  chmod $(GAMEPERM) $(GAME).new ; \
	  chmod $(EXEPERM) recover ; \
	  chmod $(FILEPERM) nhshare nhushare license Guidebook.txt )
	mv $(GAMEDIR)/$(GAME).new $(GAMEDIR)/$(GAME)

##### BINARIES #####

SYSUNIXOBJ = unixmain.o unixres.o unixunix.o
SYSSHAREOBJ = ioctl.o unixtty.o
WINTTYOBJ = getline.o termcap.o topl.o wintty.o
SRCOBJ = allmain.o alloc.o apply.o artifact.o attrib.o ball.o bones.o	\
 botl.o cmd.o dbridge.o decl.o detect.o dig.o display.o dlb.o do.o	\
 do_name.o do_wear.o dog.o dogmove.o dokick.o dothrow.o drawing.o	\
 dungeon.o eat.o end.o engrave.o exper.o explode.o extralev.o files.o	\
 fountain.o gypsy.o hack.o hacklib.o invent.o light.o lock.o mail.o	\
 makemon.o mapglyph.o mcastu.o mhitm.o mhitu.o minion.o mklev.o		\
 mkmap.o mkmaze.o mkobj.o mkroom.o mon.o mondata.o monmove.o monst.o	\
 monstr.o mplayer.o mthrowu.o muse.o music.o o_init.o objects.o		\
 objnam.o options.o pager.o pickup.o pline.o polyself.o potion.o	\
 pray.o priest.o quest.o questpgr.o read.o recover.o rect.o region.o	\
 restore.o rip.o rnd.o role.o rumors.o save.o shk.o shknam.o sit.o	\
 sounds.o sp_lev.o spell.o steal.o steed.o tech.o teleport.o		\
 timeout.o topten.o track.o trap.o u_init.o uhitm.o vault.o version.o	\
 vision.o weapon.o were.o wield.o windows.o wizard.o worm.o worn.o	\
 write.o zap.o

GAME_O = $(SYSUNIXOBJ:%=sys/unix/%) $(SYSSHAREOBJ:%=sys/share/%)	\
         $(WINTTYOBJ:%=win/tty/%) $(SRCOBJ:%=src/%)
src/$(GAME): $(GAME_O)
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -lncurses -o $@
AUTO_BIN += src/$(GAME)

RECOVER_O = util/recover.o
util/recover: $(RECOVER_O)
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@
AUTO_BIN += util/recover

MAKEDEFS_O = util/makedefs.o src/alloc.o src/monst.o src/objects.o	\
             util/panic.o
util/makedefs: $(MAKEDEFS_O)
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@
AUTO_BIN += util/makedefs

DLB_O = util/dlb_main.o src/alloc.o src/dlb.o util/panic.o
util/dlb: $(DLB_O)
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@
AUTO_BIN += util/dlb

DGN_COMP_O = util/dgn_main.o util/dgn_lex.o util/dgn_yacc.o	\
             src/alloc.o util/panic.o
util/dgn_comp: $(DGN_COMP_O)
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@
AUTO_BIN += util/dgn_comp

LEV_COMP_O = util/lev_main.o util/lev_lex.o util/lev_yacc.o	\
             src/alloc.o src/decl.o src/drawing.o src/monst.o	\
             src/objects.o util/panic.o
util/lev_comp: $(LEV_COMP_O)
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@
AUTO_BIN += util/lev_comp

ALL_O = $(GAME_O) $(RECOVER_O) $(LEV_COMP_O) $(DGN_COMP_O)	\
        $(MAKEDEFS_O) $(DLB_O)

##### DATA FILES #####

QUEST_DES = Abuser.des Acid.des Act.des Altmer.des Amazon.des		\
 Arch.des Artist.des Assassin.des Augurer.des Barb.des Bard.des		\
 Binder.des Bleeder.des Bloodsee.des Bosmer.des Bully.des Caveman.des	\
 Cheval.des Convict.des Cook.des Courier.des Cyborg.des Deatheat.des	\
 Diver.des Doll.des Drunk.des Dunmer.des Electric.des Elf.des		\
 Elfreal.des Firef.des Flame.des Foxhound.des Gamer.des Gangster.des	\
 Geek.des Gladia.des Goff.des Gradu.des Gunner.des Healer.des Ice.des	\
 Jedi.des Jester.des Knight.des Korsair.des Ladies.des Libra.des	\
 Locks.des Lunatic.des Mahou.des Monk.des Music.des Mystic.des		\
 Necro.des Ninja.des Noble.des Offic.des Ordinat.des Otaku.des		\
 Paladin.des Pickpock.des Pirate.des Polit.des Priest.des Psion.des	\
 Ranger.des Rocker.des Rogue.des Sage.des Saiyan.des Samurai.des	\
 Scient.des Scribe.des Slavemas.des Slayer.des Spacew.des Stunt.des	\
 Supermar.des Thalmor.des Top.des Tourist.des Trans.des Under.des	\
 Valkyrie.des Wandkeep.des Warrior.des Wizard.des Yeoman.des		\
 Zooke.des

SPEC_DES = beholder.des bigroom.des blkmar.des castle.des compu.des	\
 dragons.des endgame.des exmall.des forge.des frnknstn.des		\
 gehennom.des giants.des grund.des guild.des heck.des hellma.des	\
 hitch.des intpla.des key.des knox.des kobold-1.des kobold-2.des	\
 lich.des mall-1.des mall-2.des mall-3.des medusa.des mines.des		\
 mtemple.des newmall.des nightmar.des nymph.des oracle.des rats.des	\
 rndmaze.des rndmazea.des sea.des sheol.des sokoban.des spiders.des	\
 stor-1.des stor-2.des stor-3.des tomb.des tower.des town.des		\
 unheck.des yendor.des

ALL_DES = $(QUEST_DES) $(SPEC_DES)
ALL_TAG = $(ALL_DES:%.des=dat/%.tag)

$(ALL_TAG): dat/%.tag: dat/%.des util/lev_comp
	cd dat && ../util/lev_comp $(<F) && touch $(@F)
AUTO_DAT += $(ALL_TAG)

DAT_NHSHARE = cmdhelp data gypsy.txt help hh history opthelp options	\
              oracles quest.dat rumors wizhelp
dat/nhshare: util/dlb $(DAT_NHSHARE:%=dat/%)
	cd dat && ../util/dlb cf nhshare $(DAT_NHSHARE)
AUTO_DAT += dat/nhshare

DAT_NHUSHARE = dungeon
dat/nhushare: util/dlb $(DAT_NHUSHARE:%=dat/%) $(ALL_TAG)
	cd dat && ../util/dlb cf nhushare $(DAT_NHUSHARE) *.lev
AUTO_DAT += dat/nhushare

dat/data: dat/data.base util/makedefs
	cd util && ./makedefs -d  # dat/data
dat/options: include/date.h # util/makedefs
#	cd util && ./makedefs -v  # include/date.h dat/options
dat/oracles: dat/oracles.txt util/makedefs
	cd util && ./makedefs -h  # dat/oracles
dat/quest.dat: dat/quest.txt util/makedefs
	cd util && ./makedefs -q  # dat/quest.dat
dat/rumors: dat/rumors.tru dat/rumors.fal util/makedefs
	cd util && ./makedefs -r  # dat/rumors
AUTO_DAT += dat/data dat/options dat/oracles dat/quest.dat dat/rumors

dat/dungeon: dat/dungeon.pdf util/dgn_comp
	cd dat && ../util/dgn_comp dungeon.pdf  # dat/dungeon
dat/dungeon.pdf: dat/dungeon.def util/makedefs
	cd util && ./makedefs -e  # dat/dungeon.pdf
AUTO_DAT += dat/dungeon dat/dungeon.pdf

##### AUTOGENERATED SOURCE FILES #####

# for missing headers during automatic dependency generation
AUTO_HEADERS = date.h dgn_comp.h filename.h lev_comp.h onames.h pm.h
$(AUTO_HEADERS): %.h: include/%.h

include/date.h: util/makedefs $(if $(VCS_DESCRIPTION),.git/index)
	cd util && ./makedefs -v  # include/date.h dat/options
include/filename.h: util/makedefs
	cd util && ./makedefs -f  # include/filename.h
include/onames.h: util/makedefs
	cd util && ./makedefs -o  # include/onames.h
include/pm.h: util/makedefs
	cd util && ./makedefs -p  # include/pm.h
AUTO_H += include/date.h include/filename.h include/onames.h include/pm.h

src/monstr.c: util/makedefs
	cd util && ./makedefs -m  # src/monstr.c
AUTO_C += src/monstr.c

include/dgn_comp.h include/lev_comp.h: include/%_comp.h: util/%_yacc.c
util/dgn_yacc.c util/lev_yacc.c: util/%_yacc.c: util/%_comp.y
	bison $(YFLAGS) --output=util/$*_yacc.c --defines=include/$*_comp.h $<
AUTO_H += include/dgn_comp.h include/lev_comp.h
AUTO_C += util/dgn_yacc.c util/lev_yacc.c

util/dgn_lex.c util/lev_lex.c: util/%_lex.c: util/%_comp.l
	flex $(LFLAGS) -o $@ $<
AUTO_C += util/dgn_lex.c util/lev_lex.c

##### AUTOMATIC DEPENDENCY GENERATION #####

%.o: CPPFLAGS += -MMD -MP

# generating the dependencies of source files requires them to exist,
# so we add the dependencies of the autogenerated source files to the
# autogenerated header files here explicitly instead

$(AUTO_C:%.c=%.d): ;
util/dgn_yacc.o: include/date.h
util/lev_yacc.o: include/onames.h include/pm.h
util/dgn_lex.o: include/dgn_comp.h
util/lev_lex.o: include/lev_comp.h include/onames.h include/pm.h


%.d:
	$(CC) $(CFLAGS) $(CPPFLAGS) -MM -MP -MG -MF $@ -MT $*.o $*.c

ALL_D = $(ALL_O:%.o=%.d)
-include $(ALL_D)

##### SPECIAL CASES #####

src/recover.d src/recover.o: CPPFLAGS += -DNO_MAIN
src/recover.d:
	$(CC) $(CFLAGS) $(CPPFLAGS) -MM -MP -MG -MF src/recover.d -MT src/recover.o util/recover.c
src/recover.o: util/recover.c
	$(COMPILE.c) $(OUTPUT_OPTION) $<

##### CLEANING UP #####

.PHONY: clean
clean:
	rm -f $(AUTO_BIN)
	rm -f $(ALL_O)

	rm -f $(AUTO_DAT)
	rm -f dat/*.lev

	rm -f $(AUTO_H)
	rm -f $(AUTO_C)

.PHONY: cleandeps
cleandeps: clean
	rm -f $(ALL_D)

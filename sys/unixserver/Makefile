export VCS_DESCRIPTION := $(shell git describe --always)
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

CFLAGS = -g -O2
CPPFLAGS += -Iinclude
CPPFLAGS += -DNO_EXPLORE_MODE
CPPFLAGS += -D'WIZARD="$(GAMEUID)"'
CPPFLAGS += -DNO_SUSPEND
CPPFLAGS += -DDLB

all: src/$(GAME) util/recover dat/nhshare dat/nhushare

PLAYGROUND_FILES = perm record logfile xlogfile

install: all dat/license doc/Guidebook.txt
	-rm $(GAMEDIR)/$(GAME) $(GAMEDIR)/nhshare $(GAMEDIR)/nhushare
	mkdir -p $(GAMEDIR) $(GAMEDIR)/save
	$(CHOWN) $(GAMEDIR) $(GAMEDIR)/save
	chmod $(DIRPERM) $(GAMEDIR) $(GAMEDIR)/save
	-ln $(addprefix $(MASTERDIR)/,$(PLAYGROUND_FILES)) $(GAMEDIR)
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
 vis_tab.o vision.o weapon.o were.o wield.o windows.o wizard.o worm.o	\
 worn.o write.o zap.o

GAME_O = $(addprefix src/,$(SRCOBJ))			\
         $(addprefix sys/unix/,$(SYSUNIXOBJ))		\
         $(addprefix sys/share/,$(SYSSHAREOBJ))		\
         $(addprefix win/tty/,$(WINTTYOBJ))
src/$(GAME): $(GAME_O)
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -lncurses -o $@

RECOVER_O = util/recover.o
util/recover: $(RECOVER_O)
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@

MAKEDEFS_O = util/makedefs.o src/alloc.o src/monst.o src/objects.o	\
             util/panic.o
util/makedefs: $(MAKEDEFS_O)
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@

DGN_COMP_O = util/dgn_main.o util/dgn_lex.o util/dgn_yacc.o	\
             src/alloc.o util/panic.o
util/dgn_comp: $(DGN_COMP_O)
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@

LEV_COMP_O = util/lev_main.o util/lev_lex.o util/lev_yacc.o	\
             src/alloc.o src/decl.o src/drawing.o src/monst.o	\
             src/objects.o util/panic.o
util/lev_comp: $(LEV_COMP_O)
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@

DLB_O = util/dlb_main.o src/alloc.o src/dlb.o util/panic.o
util/dlb: $(DLB_O)
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@

ALL_O = $(GAME_O) $(RECOVER_O) $(LEV_COMP_O) $(DGN_COMP_O)	\
        $(MAKEDEFS_O) $(DLB_O)

##### AUTOMATIC DEPENDENCIES #####

%.o: CPPFLAGS += -MMD -MP

%.d: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -MM -MG -MP -MF $@ -MT $(@:.d=.o) $(TARGET_ARCH) $<

dat/%.d: dat/%.des
	( cd dat ; ./leveldepend.sh $(<F) > $(@F) )

ALL_D = $(patsubst %.o,%.d,$(ALL_O))
LEV_D = $(patsubst %.des,%.d,$(wildcard dat/*.des))

# LEV_D will also populate SPECIAL_LEVELS
include $(ALL_D) $(LEV_D)

##### DATA FILES #####

NHSHARE = cmdhelp data gypsy.txt help hh history opthelp options	\
          oracles quest.dat rumors wizhelp
NHUSHARE = dungeon $(SPECIAL_LEVELS)
dat/nhshare: util/dlb $(addprefix dat/,$(NHSHARE))
	( cd dat ; ../util/dlb cf nhshare $(NHSHARE) )
dat/nhushare: util/dlb $(addprefix dat/,$(NHUSHARE))
	( cd dat ; ../util/dlb cf nhushare $(NHUSHARE) )

DAT_AUTO = data options oracles quest.dat rumors dungeon.pdf dungeon

dat/data: dat/data.base util/makedefs
	( cd util ; ./makedefs -d )
dat/options: include/date.h
#	( cd util ; ./makedefs -v )
dat/oracles: dat/oracles.txt util/makedefs
	( cd util ; ./makedefs -h )
dat/quest.dat: dat/quest.txt util/makedefs
	( cd util ; ./makedefs -q )
dat/rumors: dat/rumors.tru dat/rumors.fal util/makedefs
	( cd util ; ./makedefs -r )

dat/dungeon.pdf: dat/dungeon.def util/makedefs
	( cd util ; ./makedefs -e )
dat/dungeon: dat/dungeon.pdf util/dgn_comp
	( cd dat ; ../util/dgn_comp dungeon.pdf )

##### AUTOGENERATED CODE #####

AUTO_H = date.h dgn_comp.h filename.h lev_comp.h onames.h pm.h vis_tab.h

$(AUTO_H): %: include/%

.git/HEAD:
include/date.h: util/makedefs .git/HEAD
	( cd util ; ./makedefs -v )
include/dgn_comp.h: util/dgn_yacc.c
#	bison --output=util/dgn_yacc.c --defines=include/dgn_comp.h util/dgn_comp.y
include/filename.h: util/makedefs
	( cd util ; ./makedefs -f )
include/lev_comp.h: util/lev_yacc.c
#	bison --output=util/lev_yacc.c --defines=include/lev_comp.h util/lev_comp.y
include/onames.h: util/makedefs
	( cd util ; ./makedefs -o )
include/pm.h: util/makedefs
	( cd util ; ./makedefs -p )
include/vis_tab.h: util/makedefs
	( cd util ; ./makedefs -z )

SRC_AUTO_C = monstr.c vis_tab.c

src/monstr.c: util/makedefs
	( cd util ; ./makedefs -m )
src/vis_tab.c: include/vis_tab.h
#	( cd util ; ./makedefs -z )

UTIL_AUTO_C = dgn_lex.c dgn_yacc.c lev_lex.c lev_yacc.c

util/dgn_lex.c: util/dgn_comp.l
	flex -o util/dgn_lex.c util/dgn_comp.l
util/dgn_yacc.c: util/dgn_comp.y
	bison --output=util/dgn_yacc.c --defines=include/dgn_comp.h util/dgn_comp.y

util/lev_lex.c: util/lev_comp.l
	flex -o util/lev_lex.c util/lev_comp.l
util/lev_yacc.c: util/lev_comp.y
	bison --output=util/lev_yacc.c --defines=include/lev_comp.h util/lev_comp.y

##### SPECIAL CASES #####

# shorthand for "depend on the same things util/recover.o depends on"
src/recover.o: util/recover.c util/recover.o
	$(CC) $(CFLAGS) $(CPPFLAGS) -DNO_MAIN $(TARGET_ARCH) -c -o $@ $<

src/recover.d: util/recover.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -MM -MG -MF $@ -MT $(@:.d=.o) $(TARGET_ARCH) $<

##### CLEANING UP #####

clean:
	rm -f src/$(GAME) util/recover util/makedefs
	rm -f util/dgn_comp util/lev_comp util/dlb

	rm -f $(ALL_O)

	rm -f dat/nhshare dat/nhushare
	rm -f $(addprefix dat/,$(DAT_AUTO))
	rm -f $(addprefix dat/,$(SPECIAL_LEVELS))

	rm -f $(addprefix include/,$(AUTO_H))
	rm -f $(addprefix src/,$(SRC_AUTO_C))
	rm -f $(addprefix util/,$(UTIL_AUTO_C))

spotless: clean
	rm -f $(ALL_D)
	rm -f $(LEV_D)

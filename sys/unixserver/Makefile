export VCS_DESCRIPTION := $(shell git describe --always)
VERSION := $(shell util/version.sh)

GAME = slashem
GAMEUID = acehack
GAMEGID = acehack
CHOWN = chown $(GAMEUID):$(GAMEGID)

GAMEDIR = /srv/acehack/slex-$(VERSION)
MASTERDIR = /srv/acehack/slexdir

PLAYGROUND_FILES = perm record logfile xlogfile

GAMEPERM = 0755
FILEPERM = 0644
EXEPERM  = 0755
DIRPERM  = 0755

CFLAGS = -g -O2
CFLAGS += -Iinclude
CFLAGS += -DNO_EXPLORE_MODE
CFLAGS += -D'WIZARD="$(GAMEUID)"'
CFLAGS += -DNO_SUSPEND
CFLAGS += -DDLB

CPPFLAGS += -MMD

all: src/$(GAME) util/recover dat/nhshare dat/nhushare \
  dat/license doc/Guidebook.txt

install: all
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

AUTOHEADERS = onames.h pm.h vis_tab.h filename.h date.h lev_comp.h dgn_comp.h

$(AUTOHEADERS): %: include/%

dat/%.d: dat/%.des
	( cd dat ; ./leveldepend.sh $(<F) > $(@F) )

%.d: %.c
	$(CC) $(CFLAGS) $(TARGET_ARCH) -MM -MG -MF $@ -MT $(@:.d=.o) $<

DEPENDS := $(wildcard src/*.c) $(wildcard win/tty/*.c) $(wildcard sys/unix/*.c)
DEPENDS += sys/share/ioctl.c sys/share/unixtty.c
DEPENDS += $(wildcard util/*.c)
DEPENDS := $(DEPENDS:.c=.d)
DEPENDS += util/lev_yacc.d util/lev_lex.d util/dgn_yacc.d util/dgn_lex.d
DEPENDS += src/vis_tab.d src/monstr.d src/recover.d
DEPENDS += $(patsubst %.des,%.d,$(wildcard dat/*.des))

include $(DEPENDS)

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

src/$(GAME): LDLIBS += -lncurses
src/$(GAME): $(addprefix src/,$(SRCOBJ)) \
             $(addprefix sys/unix/,$(SYSUNIXOBJ)) \
             $(addprefix sys/share/,$(SYSSHAREOBJ)) \
             $(addprefix win/tty/,$(WINTTYOBJ))
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@

NHSHARE = data oracles options quest.dat rumors help hh	\
          cmdhelp history opthelp wizhelp gypsy.txt

dat/nhshare: util/dlb $(addprefix dat/,$(NHSHARE))
	( cd dat ; ../util/dlb cf nhshare $(NHSHARE) )

dat/nhushare: util/dlb dat/dungeon $(addprefix dat/,$(SPECIAL_LEVELS))
	( cd dat ; ../util/dlb cf nhushare dungeon $(SPECIAL_LEVELS) )

util/makedefs: util/makedefs.o src/monst.o src/objects.o src/alloc.o \
 util/panic.o

include/onames.h: util/makedefs
	( cd util ; ./makedefs -o )
include/pm.h: util/makedefs
	( cd util ; ./makedefs -p )
src/monstr.c: util/makedefs
	( cd util ; ./makedefs -m )
src/vis_tab.c: include/vis_tab.h
include/vis_tab.h: util/makedefs
	( cd util ; ./makedefs -z )

include/filename.h: util/makedefs
	( cd util ; ./makedefs -f )
dat/options: include/date.h
include/date.h: util/makedefs .git/HEAD
	( cd util ; ./makedefs -v )
.git/HEAD:

dat/data: dat/data.base util/makedefs
	( cd util ; ./makedefs -d )
dat/rumors: dat/rumors.tru dat/rumors.fal util/makedefs
	( cd util ; ./makedefs -r )
dat/quest.dat: dat/quest.txt util/makedefs
	( cd util ; ./makedefs -q )
dat/oracles: dat/oracles.txt util/makedefs
	( cd util ; ./makedefs -h )
dat/dungeon.pdf: dat/dungeon.def util/makedefs
	( cd util ; ./makedefs -e )

dat/dungeon: dat/dungeon.pdf util/dgn_comp
	( cd dat ; ../util/dgn_comp dungeon.pdf )

util/lev_comp: util/lev_main.o util/lev_yacc.o util/lev_lex.o src/alloc.o \
  util/panic.o src/drawing.o src/decl.o src/monst.o src/objects.o
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@
include/lev_comp.h: util/lev_yacc.c
util/lev_yacc.c: util/lev_comp.y
	bison -y --output=util/lev_yacc.c --defines=include/lev_comp.h \
	      util/lev_comp.y
util/lev_lex.c: util/lev_comp.l
	flex -o util/lev_lex.c util/lev_comp.l

util/dgn_comp: util/dgn_main.o util/dgn_yacc.o util/dgn_lex.o src/alloc.o \
  util/panic.o
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@
include/dgn_comp.h: util/dgn_yacc.c
util/dgn_yacc.c: util/dgn_comp.y
	bison -y --output=util/dgn_yacc.c --defines=include/dgn_comp.h \
	      util/dgn_comp.y
util/dgn_lex.c: util/dgn_comp.l
	flex -o util/dgn_lex.c util/dgn_comp.l

util/dlb: util/dlb_main.o src/dlb.o src/alloc.o util/panic.o
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@

src/recover.o: util/recover.c util/recover.o
	$(CC) $(CFLAGS) -DNO_MAIN $(CPPFLAGS) $(TARGET_ARCH) -c -o $@ $<
src/recover.d: util/recover.c
	$(CC) $(CFLAGS) $(TARGET_ARCH) -MM -MG -MF $@ -MT $(@:.d=.o) $<

clean:
	rm -f src/*.o util/*.o sys/unix/*.o sys/share/*.o win/tty/*.o
	rm -f dat/*.lev
	( cd dat ; \
	  rm -f data dungeon dungeon.pdf nhshare nhushare \
                options oracles quest.dat rumors )
	( cd include ; \
	  rm -f date.h dgn_comp.h filename.h lev_comp.h \
                onames.h pm.h vis_tab.h )
	rm -f src/monstr.c src/slashem src/vis_tab.c
	( cd util ; \
	  rm -f dgn_comp dgn_lex.c dgn_yacc.c dlb \
	        lev_comp lev_lex.c lev_yacc.c makedefs recover )

spotless: clean
	rm -f src/*.d util/*.d sys/unix/*.d sys/share/*.d win/tty/*.d
	rm -f dat/*.d

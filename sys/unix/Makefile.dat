#	NetHack Makefile.
#	SCCS Id: @(#)Makefile.dat	3.4	1992/09/18

# for Atari
# SHELL=E:/GEMINI2/MUPFEL.TTP
# UUDECODE=uudecode

VARDAT = data rumors quest.dat oracles options

all:	$(VARDAT) spec_levs quest_levs dungeon

../util/makedefs:
	(cd ../util ; make makedefs)

../util/dgn_comp:
	(cd ../util ; make dgn_comp)

../util/lev_comp:
	(cd ../util ; make lev_comp)

../util/tile2x11:
	(cd ../util ; make tile2x11)

../util/tile2beos:
	(cd ../util ; make tile2beos)

../util/tile2png:
	(cd ../util ; make tile2png)

../util/magtile:
	(cd ../util ; make magtile)

../util/bigtile:
	(cd ../util ; make bigtile)

../util/txtmerge:
	(cd ../util ; make txtmerge)

../util/txtbg:
	(cd ../util ; make txtbg)

../util/mapmerge:
	(cd ../util ; make mapmerge)

../util/tile2bmp:
	(cd ../util ; make tile2bmp)

x11tiles: ../util/tile2x11 ../win/share/monsters.txt ../win/share/objects.txt \
				../win/share/other.txt
	../util/tile2x11 ../win/share/monsters.txt ../win/share/objects.txt \
				../win/share/other.txt

x11tiles.map: ../util/mapmerge ../win/share/monsters.map \
				../win/share/objects.map ../win/share/other.map
	../util/mapmerge x11tiles.map ../win/share/monsters.map \
				../win/share/objects.map ../win/share/other.map

x11bigtiles:	../util/tile2x11 ../win/share/mon32.txt \
		../win/share/obj32.txt ../win/share/oth32.txt
	../util/tile2x11 -o x11bigtiles ../win/share/mon32.txt \
		../win/share/obj32.txt ../win/share/oth32.txt

x11bigtiles.map:	x11tiles.map
	cp x11tiles.map x11bigtiles.map

x11big3dtiles:	../util/tile2x11 ../win/share/mon3d.txt \
		../win/share/obj3d.txt ../win/share/oth3d.txt
	../util/tile2x11 -o x11big3dtiles ../win/share/mon3d.txt \
		../win/share/obj3d.txt ../win/share/oth3d.txt

x11big3dtiles.map:	x11tiles.map
	cp x11tiles.map x11big3dtiles.map

beostiles: ../util/tile2beos ../win/share/monsters.txt ../win/share/objects.txt \
			../win/share/other.txt
	../util/tile2beos ../win/share/monsters.txt ../win/share/objects.txt \
			../win/share/other.txt

gltile16.png: ../util/tile2png ../win/share/monsters.txt ../win/share/objects.txt \
				../win/share/other.txt
	../util/tile2png -o $@ ../win/share/monsters.txt ../win/share/objects.txt \
				../win/share/other.txt

gltile16.map: ../util/mapmerge ../win/share/monsters.map \
				../win/share/objects.map ../win/share/other.map
	../util/mapmerge gltile16.map ../win/share/monsters.map \
				../win/share/objects.map ../win/share/other.map

gltile32.png:	../util/tile2png ../win/share/mon32.txt \
		../win/share/obj32.txt ../win/share/oth32.txt
	../util/tile2png -o $@ -t ../win/share/mon32.txt \
		../win/share/obj32.txt ../win/share/oth32.txt

gltile32.map:	gltile16.map
	cp gltile16.map gltile32.map

gltile64.png:	../util/tile2png ../win/share/mon3d.txt \
		../win/share/obj3d.txt ../win/share/oth3d.txt
	../util/tile2png -o $@ -t ../win/share/mon3d.txt \
		../win/share/obj3d.txt ../win/share/oth3d.txt

gltile64.map:	gltile16.map
	cp gltile16.map gltile64.map

glfont8.png:	../util/tile2png ../win/gl/font8.txt
	../util/tile2png -o $@ -f ../win/gl/font8.txt

glfont14.png:	../util/tile2png ../win/gl/font14.txt
	../util/tile2png -o $@ -f ../win/gl/font14.txt

glfont20.png:	../util/tile2png ../win/gl/font20.txt
	../util/tile2png -o $@ -f ../win/gl/font20.txt

glfont22.png:	../util/tile2png ../win/gl/font22.txt
	../util/tile2png -o $@ -f ../win/gl/font22.txt

gllogo.png:	../util/tile2png ../win/gl/logo.txt
	../util/tile2png -o $@ -a 28 ../win/gl/logo.txt

glrip.png:	../util/tile2png ../win/gl/rip.txt
	../util/tile2png -o $@ -a 20 ../win/gl/rip.txt

../win/share/monmag.txt:	../util/magtile ../win/share/monsters.txt
	../util/magtile

../win/share/objmag.txt:	../util/magtile ../win/share/objects.txt
	../util/magtile

../win/share/othmag.txt:	../util/magtile ../win/share/other.txt
	../util/magtile

../win/share/monbig.txt:	../util/bigtile ../win/share/mon32-t.txt \
				../win/share/obj32-t.txt \
				../win/share/oth32-t.txt
	../util/bigtile

../win/share/objbig.txt:	../util/bigtile ../win/share/mon32-t.txt \
				../win/share/obj32-t.txt \
				../win/share/oth32-t.txt
	../util/bigtile

../win/share/othbig.txt:	../util/bigtile ../win/share/mon32-t.txt \
				../win/share/obj32-t.txt \
				../win/share/oth32-t.txt
	../util/bigtile

../win/share/mon32.txt:	../util/txtbg ../win/share/mon32-t.txt
	../util/txtbg ../win/share/mon32-t.txt ../win/share/mon32.txt

../win/share/obj32.txt:	../util/txtbg ../win/share/obj32-t.txt
	../util/txtbg ../win/share/obj32-t.txt ../win/share/obj32.txt

../win/share/oth32.txt:	../util/txtbg ../win/share/oth32-t.txt
	../util/txtbg ../win/share/oth32-t.txt ../win/share/oth32.txt

../win/share/mon32-t.txt:	../util/txtmerge ../win/share/monmag.txt \
		../win/share/mon32mi.txt ../win/share/mon32alg.txt \
		../win/share/mon32aw.txt ../win/share/mon32se.txt \
		../win/share/palette.txt
	../util/txtmerge -p ../win/share/palette.txt ../win/share/mon32-t.txt \
		../win/share/monmag.txt -b ../win/share/mon32mi.txt \
		-b ../win/share/mon32se.txt -bff00ff ../win/share/mon32alg.txt \
		-bff00ff ../win/share/mon32aw.txt 

../win/share/obj32-t.txt:	../util/txtmerge ../win/share/objmag.txt \
		../win/share/obj32mi.txt ../win/share/obj32se.txt \
		../win/share/obj32alg.txt ../win/share/palette.txt
	../util/txtmerge -p ../win/share/palette.txt ../win/share/obj32-t.txt \
		../win/share/objmag.txt -b ../win/share/obj32mi.txt \
		-b ../win/share/obj32se.txt -bff00ff ../win/share/obj32alg.txt

../win/share/oth32-t.txt:	../util/txtmerge ../win/share/othmag.txt \
		../win/share/oth32mi.txt ../win/share/oth32se.txt \
		../win/share/oth32alg.txt ../win/share/palette.txt
	../util/txtmerge -p ../win/share/palette.txt ../win/share/oth32-t.txt \
		../win/share/othmag.txt -b ../win/share/oth32mi.txt \
		-b ../win/share/oth32se.txt -bff00ff ../win/share/oth32alg.txt

../win/share/mon3d.txt:	../util/txtmerge ../win/share/monbig.txt \
		../win/share/mon3dmi.txt ../win/share/palette.txt
	../util/txtmerge -p ../win/share/palette.txt ../win/share/mon3d.txt \
		../win/share/monbig.txt -b ../win/share/mon3dmi.txt

../win/share/obj3d.txt:	../util/txtmerge ../win/share/objbig.txt
	cp ../win/share/objbig.txt ../win/share/obj3d.txt

../win/share/oth3d.txt:	../util/txtmerge ../win/share/othbig.txt \
		../win/share/oth3dmi.txt ../win/share/oth-2kmi.txt \
		../win/share/oth3dse.txt ../win/share/palette.txt
	../util/txtmerge -p ../win/share/palette.txt ../win/share/oth3d.txt \
		../win/share/othbig.txt -b ../win/share/oth3dmi.txt \
		-b ../win/share/oth-2kmi.txt -b ../win/share/oth3dse.txt

nhtiles.bmp: ../util/tile2bmp ../win/share/monsters.txt ../win/share/objects.txt \
				../win/share/other.txt
	../util/tile2bmp $@ ../win/share/monsters.txt ../win/share/objects.txt \
				../win/share/other.txt

SlashEM.ad: ../win/X11/SlashEM.ad
	cp ../win/X11/SlashEM.ad SlashEM.ad

pet_mark.xbm: ../win/X11/pet_mark.xbm
	cp ../win/X11/pet_mark.xbm pet_mark.xbm

rip.xpm: ../win/X11/rip.xpm
	cp ../win/X11/rip.xpm rip.xpm

mapbg.xpm: ../win/gnome/mapbg.xpm
	cp ../win/gnome/mapbg.xpm mapbg.xpm

nhsplash.xpm: ../win/Qt/nhsplash.xpm
	cp ../win/Qt/nhsplash.xpm nhsplash.xpm

nethack.icns: ../win/Qt/nhicns.uu
	$(UUDECODE) ../win/Qt/nhicns.uu

Info.plist: ../win/Qt/Info.pli
	cp ../win/Qt/Info.pli Info.plist

../util/tile2img.ttp:
	(cd ../util ; make tile2img.ttp)

../util/xpm2img.ttp:
	(cd ../util ; make xpm2img.ttp)
nh16.img: ../util/tile2img.ttp ../win/share/monsters.txt \
                              ../win/share/objects.txt ../win/share/other.txt
	../util/tile2img.ttp nh16.img

rip.img: ../util/xpm2img.ttp
	../util/xpm2img.ttp ../win/X11/rip.xpm rip.img
title.img:
	# cp ../win/gem/title.img title.img
	$(UUDECODE) ../win/gem/title.uu

GEM_RSC.RSC:
	# cp ../win/gem/GEM_RSC.RSC GEM_RSC.RSC
	$(UUDECODE) ../win/gem/gem_rsc.uu

data:	data.base ../util/makedefs
	../util/makedefs -d

rumors:	rumors.tru rumors.fal ../util/makedefs
	../util/makedefs -r

quest.dat:	quest.txt ../util/makedefs
	../util/makedefs -q

oracles:	oracles.txt ../util/makedefs
	../util/makedefs -h

# note: 'options' should have already been made when include/date.h was created
options:	../util/makedefs
	../util/makedefs -v


spec_levs: ../util/lev_comp \
	beholder.des bigroom.des blkmar.des castle.des grund.des dragons.des \
	endgame.des frnknstn.des gehennom.des giants.des guild.des knox.des \
	kobold-1.des kobold-2.des lich.des mall-1.des mall-2.des medusa.des \
	mines.des mtemple.des newmall.des nightmar.des nymph.des oracle.des \
	rats.des sea.des spiders.des stor-1.des stor-2.des stor-3.des tomb.des \
	tower.des yendor.des sokoban.des
	../util/lev_comp beholder.des
	../util/lev_comp bigroom.des
	../util/lev_comp blkmar.des
	../util/lev_comp castle.des
	../util/lev_comp grund.des
	../util/lev_comp dragons.des
	../util/lev_comp endgame.des
	../util/lev_comp frnknstn.des
	../util/lev_comp gehennom.des
	../util/lev_comp giants.des
	../util/lev_comp guild.des
	../util/lev_comp knox.des
	../util/lev_comp kobold-1.des
	../util/lev_comp kobold-2.des
	../util/lev_comp lich.des
	../util/lev_comp mall-1.des
	../util/lev_comp mall-2.des
	../util/lev_comp medusa.des
	../util/lev_comp mines.des
	../util/lev_comp mtemple.des
	../util/lev_comp newmall.des
	../util/lev_comp nightmar.des
	../util/lev_comp nymph.des
	../util/lev_comp oracle.des
	../util/lev_comp rats.des
	../util/lev_comp sea.des
	../util/lev_comp sokoban.des
	../util/lev_comp spiders.des
	../util/lev_comp stor-1.des
	../util/lev_comp stor-2.des
	../util/lev_comp stor-3.des
	../util/lev_comp tomb.des
	../util/lev_comp tower.des
	../util/lev_comp yendor.des
	touch spec_levs

quest_levs: ../util/lev_comp \
	Arch.des Barb.des Caveman.des Flame.des Healer.des Ice.des Knight.des \
	Monk.des Necro.des Priest.des Ranger.des Rogue.des Samurai.des \
	Tourist.des Slayer.des Valkyrie.des Wizard.des Yeoman.des
	../util/lev_comp Arch.des
	../util/lev_comp Barb.des
	../util/lev_comp Caveman.des
	../util/lev_comp Flame.des
	../util/lev_comp Healer.des
	../util/lev_comp Ice.des
	../util/lev_comp Knight.des
	../util/lev_comp Monk.des
	../util/lev_comp Necro.des
	../util/lev_comp Priest.des
	../util/lev_comp Ranger.des
	../util/lev_comp Rogue.des
	../util/lev_comp Samurai.des
	@if egrep -s tourists options > /dev/null ; then \
	echo ../util/lev_comp Tourist.des ; ../util/lev_comp Tourist.des ; \
	else rm -f Tou-goal.lev Tou-fil?.lev Tou-loca.lev Tou-strt.lev ; fi
	../util/lev_comp Slayer.des
	../util/lev_comp Valkyrie.des
	../util/lev_comp Wizard.des
	@if egrep -s yeomen options > /dev/null ; then \
	echo ../util/lev_comp Yeoman.des ; ../util/lev_comp Yeoman.des ; \
	else rm -f Yeo-goal.lev Yeo-fil?.lev Yeo-loca.lev Yeo-strt.lev ; fi
	touch quest_levs

dungeon: dungeon.def ../util/makedefs ../util/dgn_comp
	../util/makedefs -e
	../util/dgn_comp dungeon.pdf

spotless:
	-rm -f spec_levs quest_levs *.lev $(VARDAT) dungeon dungeon.pdf
	-rm -f nhshare nhushare x11tiles x11bigtiles x11big3dtiles beostiles
	-rm -f x11tiles.map x11bigtiles.map x11big3dtiles.map
	-rm -f gtkrc
	-rm -f gl*.png
	-rm -f pet_mark.xbm rip.xpm mapbg.xpm
	-rm -f rip.img GEM_RSC.RSC title.img nh16.img SlashEM.ad
	-rm -f ../win/share/monmag.txt ../win/share/mon32.txt
	-rm -f ../win/share/objmag.txt ../win/share/obj32.txt
	-rm -f ../win/share/othmag.txt ../win/share/oth32.txt
	-rm -f ../win/share/monbig.txt ../win/share/mon3d.txt
	-rm -f ../win/share/objbig.txt ../win/share/obj3d.txt
	-rm -f ../win/share/othbig.txt ../win/share/oth3d.txt
	-rm -f ../win/share/mon32-t.txt
	-rm -f ../win/share/obj32-t.txt
	-rm -f ../win/share/oth32-t.txt

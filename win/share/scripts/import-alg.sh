#!/bin/sh
#
#   Copyright (c) J. Ali Harlow 2000
#   NetHack may be freely redistributed.  See license for details.
#
#   This shell script imports Kelly Youngblood's tilesets from his
#   AllegroHack package. You must have a copy of the binary zip
#   file. anh_15b.zip is available from
#     http://www.pinn.net/~jry/allegrohack/
#   Set the variable below set to point at it (there's no need to
#   unpack the archive first).
#
#   Warning: This code assumes that the Allegro tile index file lists all
#   monsters, followed by all objects, followed by all others. If this changes
#   then id in ppm_package() will be wrong. Solve by having a pass one to
#   determine how many monsters and objects there are and then using mon_indx,
#   num_mon + obj_indx, and num_mon + num_obj + oth_indx instead of id.
#
#   Configuration variables
#
input=anh_15b.zip
xpm2txt=../../../util/xpm2txt
txtfilt=../../../util/txtfilt
ranger=ran.ppm
#
# Functions
#
addtile()
# Usage: addtile <output> <id> <name> <input>
{
    ad_output=$1
    ad_id=$2
    ad_name=$3
    ad_input=$4
    ad_sed=`echo ${ad_output} | sed 's/\.[^.]*$/.sed/'`
    if test -r $ad_output; then
	pnmcat -lr $ad_output $ad_input > /tmp/ad$$.pnm
	mv /tmp/ad$$.pnm $ad_output
	ad_count=`echo ${ad_count} + 1 | bc`
    else
	rm -f ${ad_sed}
	anytopnm $ad_input > $ad_output
	ad_count=0
    fi
    echo "s£^# tile ${ad_count} (unknown)£# tile ${id} ($name)£" >> ${ad_sed}
}
#
initialize()
{
    cat <<-EOF > cmap.names
	67	dig beam
	68	camera flash
	69	thrown boomerang, open left
	70	thrown boomerang, open right
	71	magic shield 1
	72	magic shield 2
	73	magic shield 3
	74	magic shield 4
	75	swallow top left
	76	swallow top center
	77	swallow top right
	78	swallow middle left
	79	swallow middle right
	80	swallow bottom left
	81	swallow bottom center
	82	swallow bottom right
	83	explosion top left
	84	explosion top center
	85	explosion top right
	86	explosion middle left
	87	explosion middle center
	88	explosion middle right
	89	explosion bottom left
	90	explosion bottom center
	91	explosion bottom right
	EOF
    cat <<-EOF > mon32alg-main.names
	gas spore
	flaming sphere
	shocking sphere
	master mind flayer
	glass piercer
	mastodon
	trapper
	baby silver dragon
	silver dragon
	stalker
	lichen
	arch-lich
	green slime
	disenchanter
	pit viper
	vampire
	vampire lord
	vampire mage
	monkey
	carnivorous ape
	gold golem
	aligned priest
	high priest
	monk
	ranger
	Ixoth
	pyrolisk
	large cat
	storm giant
	glass golem
	watchman
	watch captain
	Scorpius
	Master Assassin
	Lord Surtur
	Death
	Pestilence
	Famine
	EOF
    cat <<-EOF > obj32alg-main.names
	athame
	silver dagger
	worm tooth
	crysknife
	crude short sword / orcish short sword
	short sword
	broad short sword / dwarvish short sword
	runed short sword / elven short sword
	runed broadsword / elven broadsword
	crude spear / orcish spear
	silver spear
	runed spear / elven spear
	stout spear / dwarvish spear
	throwing spear / javelin
	trident
	long bow / yumi
	crossbow
	Hawaiian shirt
	T-shirt
	silver dragon scale mail
	silver dragon scales
	hooded cloak / dwarvish cloak
	slippery cloak / oilskin cloak
	tattered cape / cloak of protection
	ornamental cope / cloak of magic resistance
	clay / increase accuracy
	steel / slow digestion
	twisted / free action
	concave / amulet of unchanging
	key / skeleton key
	lock pick
	figurine
	meatball
	huge chunk of meat
	crystal / enlightenment
	strange object
	candy bar
	smoky / object detection
	glass / light
	EOF
    cat <<-EOF > oth32alg-main.names
	cmap / explosion top left
	cmap / explosion top center
	cmap / explosion top right
	cmap / explosion middle left
	cmap / explosion middle center
	cmap / explosion middle right
	cmap / explosion bottom left
	cmap / explosion bottom center
	cmap / explosion bottom right
	magic trap
	anti-magic trap field
	polymorph trap
	zap 5 0
	zap 5 1
	zap 5 2
	zap 5 3
	zap 7 0
	zap 7 1
	zap 7 2
	zap 7 3
	EOF
    cat <<-EOF > mon32alg-sub.names
	priest
	priestess
	EOF
    cat <<-EOF > obj32alg-sub.names
	velvet / drain life
	gold piece
	EOF
    cat <<-EOF > oth32alg-sub.names
	grave
	EOF
    toplevel=ahack-dist
    output=ahack
    /bin/rm -rf $toplevel
    unzip -L -q -d $toplevel ${input}
    if test -n "$ranger"; then
	if test -r "$ranger"; then
	    ppmtobmp -windows $ranger > $toplevel/tiles/mon/ran.bmp
	else
	    echo "Warning: Can't read ranger tile ($ranger)" 2>&1
	fi
    fi
    rm -f ${output}-monsters.tile ${output}-objects.tile ${output}-other.tile
    rm -rf $output
    mkdir $output
    id=1
    mon_indx=1
    obj_indx=1
    oth_indx=1
}
#
ppm_package()
{
    rm -f mon32alg.ppm obj32alg.ppm oth32alg.ppm
    sed -e 's:\\:/:g' -e 's///g' $toplevel/tiles/index | egrep -v '^#|^[ 	]*$' | while read "line"; do
	name=`echo $line | sed -n 's/.*(\([^)]*\)).*/\1/p'`
	bmp=`echo $line | sed -n 's/.*:[ 	]*\([^ 	]*\).*/\1/p'`
	offset=`echo $line | sed -n 's/.*\@[ 	]*\([^ 	]*\).*/\1/p'`
	if echo $name | egrep -s '^cmap [0-9]+$' >/dev/null; then
	    no=`echo $name | sed 's/cmap *//'`
	    nline=`egrep "^${no}	" cmap.names`
	    if test -n "$nline"; then
		name=`echo $nline | sed 's:^[^ 	]*[ 	]:cmap / :'`
	    fi
	fi
	if test \! -r $toplevel/tiles/$bmp; then
	    echo "Warning: tile bitmap ($bmp) not readable: ignored" 1>&2
	    continue;
	fi
	case $bmp in
	mon/*)
	    ex_output=mon32alg
	    echo ${output}@monsters@${name}@${mon_indx} >> ${output}-monsters.tile
	    mon_indx=`echo $mon_indx + 1 | bc`
	    ;;
	obj/*)
	    ex_output=obj32alg
	    echo ${output}@objects@${name}@${obj_indx} >> ${output}-objects.tile
	    obj_indx=`echo $obj_indx + 1 | bc`
	    ;;
	etc/*)
	    ex_output=oth32alg
	    echo ${output}@other@${name}@${oth_indx} >> ${output}-other.tile
	    oth_indx=`echo $oth_indx + 1 | bc`
	    ;;
	*)
	    echo "Warning: tile not in a known tileset ($bmp): ignored" 1>&2
	    continue
	    ;;
	esac
	bmptoppm $toplevel/tiles/$bmp > $output/x${id}.ppm 2> /dev/null
	pnmcut 0 ${offset:-0} 32 32 $output/x${id}.ppm > $output/x${id}.pnm
	addtile ${ex_output}.ppm "$id" "$name" $output/x${id}.pnm
	id=`echo ${id} + 1 | bc`
    done
}
#
import()
# Usage: import <tileset> <sub>
# tileset is assumed to be 32; sub can be one of mon, obj or oth
{
    tileset=$1
    sub=$2
    if test -r ${sub}${tileset}alg.ppm; then
	ppmquant -fs -map palette.ppm ${sub}${tileset}alg.ppm 2> /dev/null | \
	ppmtoxpm > ${sub}${tileset}alg.xpm
	${xpm2txt} -w32 -h32 ${sub}${tileset}alg.xpm /tmp/et$$.txt
	sed -f ${sub}${tileset}alg.sed < /tmp/et$$.txt > \
	${sub}${tileset}alg-full.txt
	rm -f /tmp/et$$.txt
    else
	echo "Warning: No ${tileset} tiles imported from subset $sub"
    fi
}
filter()
# Usage: filter <tileset> <sub>
# tileset is assumed to be 32; sub can be one of mon, obj or oth
{
    tileset=$1
    sub=$2
    if test -r ${sub}${tileset}alg-full.txt; then
	echo ${txtfilt} -f ${sub}${tileset}alg-main.names \
	${sub}${tileset}alg-full.txt ${sub}${tileset}alg.txt
	${txtfilt} -f ${sub}${tileset}alg-main.names \
	${sub}${tileset}alg-full.txt ${sub}${tileset}alg.txt
	echo ${txtfilt} -f ${sub}${tileset}alg-sub.names \
	${sub}${tileset}alg-full.txt ${sub}${tileset}al2.txt
	${txtfilt} -f ${sub}${tileset}alg-sub.names \
	${sub}${tileset}alg-full.txt ${sub}${tileset}al2.txt
    fi
}
clean()
{
    rm -f -- cmap.names mon32alg-main.names obj32alg-main.names
    rm -f -- oth32alg-main.names mon32alg-sub.names obj32alg-sub.names
    rm -f -- oth32alg-sub.names
    rm -rf -- ${toplevel} ${output}
    rm -f -- mon32alg.ppm obj32alg.ppm oth32alg.ppm
    rm -f -- ${output}-monsters.tile ${output}-objects.tile ${output}-other.tile
    rm -f -- mon32alg.xpm obj32alg.xpm oth32alg.xpm
    rm -f -- mon32alg.sed obj32alg.sed oth32alg.sed
    rm -f -- mon32alg-full.txt obj32alg-full.txt oth32alg-full.txt
}
set -e
initialize
ppm_package
import 32 mon
import 32 obj
import 32 oth
filter 32 mon
filter 32 obj
filter 32 oth
clean
exit 0
